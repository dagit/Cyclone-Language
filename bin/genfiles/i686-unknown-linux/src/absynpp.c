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
void*v;};struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];typedef struct{
int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*
x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2);int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2);extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*
f);struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*
Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();struct Cyc_PP_Doc*Cyc_PP_line_doc();
struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s);struct Cyc_PP_Doc*Cyc_PP_textptr(
struct _dyneither_ptr*p);struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,
int w);struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);struct Cyc_PP_Doc*Cyc_PP_cat(
struct _dyneither_ptr);struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct
Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),
struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_seql(
struct _dyneither_ptr sep,struct Cyc_List_List*l0);struct Cyc_PP_Doc*Cyc_PP_ppseql(
struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
struct Cyc_PP_Doc*Cyc_PP_group(struct _dyneither_ptr start,struct _dyneither_ptr stop,
struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_egroup(
struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,
struct Cyc_List_List*l);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;
struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];
struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dyneither_ptr source;
struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};extern char
Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple0{union Cyc_Absyn_Nmspace_union
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;int is_flat;};
struct Cyc_Absyn_UnknownDatatype_struct{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
f1;};struct Cyc_Absyn_KnownDatatype_struct{int tag;struct Cyc_Absyn_Datatypedecl**
f1;};union Cyc_Absyn_DatatypeInfoU_union{struct Cyc_Absyn_UnknownDatatype_struct
UnknownDatatype;struct Cyc_Absyn_KnownDatatype_struct KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU_union datatype_info;struct Cyc_List_List*targs;struct
Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*
datatype_name;struct _tuple0*field_name;int is_extensible;};struct Cyc_Absyn_UnknownDatatypefield_struct{
int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo f1;};struct Cyc_Absyn_KnownDatatypefield_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};union
Cyc_Absyn_DatatypeFieldInfoU_union{struct Cyc_Absyn_UnknownDatatypefield_struct
UnknownDatatypefield;struct Cyc_Absyn_KnownDatatypefield_struct KnownDatatypefield;
};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
f1;};union Cyc_Absyn_AggrInfoU_union{struct Cyc_Absyn_UnknownAggr_struct
UnknownAggr;struct Cyc_Absyn_KnownAggr_struct KnownAggr;};struct Cyc_Absyn_AggrInfo{
union Cyc_Absyn_AggrInfoU_union aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;struct Cyc_Absyn_Conref*
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_Evar_struct{int tag;
struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};struct
Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{
int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{
int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*
f1;};struct Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
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
_dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
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
f1;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{
struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*loc;void*annot;};
struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct
_tuple2{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple2 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple2 f2;struct _tuple2 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple2 f2;};struct Cyc_Absyn_TryCatch_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{
int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;
struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo
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
struct Cyc_Core_Opt*fields;int is_extensible;int is_flat;};struct Cyc_Absyn_Enumfield{
struct _tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};
struct Cyc_Absyn_Enumdecl{void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};
struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};
struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*
f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_struct{int tag;struct Cyc_Absyn_Tvar*
f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;
};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;
struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*
f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_bounds_one;struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple3{void*f1;
struct _tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU_union);
struct Cyc_Buffer_t;unsigned int Cyc_strlen(struct _dyneither_ptr s);int Cyc_strptrcmp(
struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_str_sepstr(
struct Cyc_List_List*,struct _dyneither_ptr);struct _tuple4{unsigned int f1;int f2;};
struct _tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple5{void*f1;void*f2;};struct _tuple5*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple5*Cyc_Dict_rchoose(
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
le;int allow_valueof;};void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern int Cyc_Absynpp_print_scopes;void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);struct
_dyneither_ptr Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_typ2cstring(
void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(void*);struct _dyneither_ptr
Cyc_Absynpp_kindbound2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _dyneither_ptr
Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct _dyneither_ptr Cyc_Absynpp_prim2string(
void*p);struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);struct
_dyneither_ptr Cyc_Absynpp_scope2string(void*sc);int Cyc_Absynpp_is_anon_aggrtype(
void*t);extern struct _dyneither_ptr Cyc_Absynpp_cyc_string;extern struct
_dyneither_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Absynpp_prim2str(void*p);int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s);struct _tuple6{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple6*arg);struct
_tuple7{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};struct _tuple7
Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);extern
void*Cyc_Cyclone_c_compiler;struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*dp);struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List*cs);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
void*);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*);struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int
inprec,struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct
_tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst_union);struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
void*);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*
es);struct _tuple9{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_PP_Doc*
Cyc_Absynpp_de2doc(struct _tuple9*de);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*dopt);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct
Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd);static int Cyc_Absynpp_expand_typedefs;static int Cyc_Absynpp_qvar_to_Cids;
static char _tmp0[4]="Cyc";struct _dyneither_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,
_tmp0 + 4};struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
static int Cyc_Absynpp_add_cyc_prefix;static int Cyc_Absynpp_to_VC;static int Cyc_Absynpp_decls_first;
static int Cyc_Absynpp_rewrite_temp_tvars;static int Cyc_Absynpp_print_all_tvars;
static int Cyc_Absynpp_print_all_kinds;static int Cyc_Absynpp_print_all_effects;
static int Cyc_Absynpp_print_using_stmts;static int Cyc_Absynpp_print_externC_stmts;
static int Cyc_Absynpp_print_full_evars;static int Cyc_Absynpp_generate_line_directives;
static int Cyc_Absynpp_use_curr_namespace;static int Cyc_Absynpp_print_zeroterm;
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;struct Cyc_Absynpp_Params;
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){Cyc_Absynpp_expand_typedefs=
fs->expand_typedefs;Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
fs->add_cyc_prefix;Cyc_Absynpp_to_VC=fs->to_VC;Cyc_Absynpp_decls_first=fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars;Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;Cyc_Absynpp_print_all_effects=
fs->print_all_effects;Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;Cyc_Absynpp_print_externC_stmts=
fs->print_externC_stmts;Cyc_Absynpp_print_full_evars=fs->print_full_evars;Cyc_Absynpp_print_zeroterm=
fs->print_zeroterm;Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;Cyc_Absynpp_curr_namespace=
fs->curr_namespace;}struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,
1,0,0,0,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,0,
0,0,1,0,0,1,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,
0,1,0,0,0,0,0,0,0,0,0,0,0};struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,
0,0,0,0,0,0,0,1,1,0,1,0,0,0};static void Cyc_Absynpp_curr_namespace_add(struct
_dyneither_ptr*v){Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,({
struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v;_tmp1->tl=0;
_tmp1;}));}static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){if(((struct
Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;else{Cyc_Absynpp_suppr_last(&((
struct Cyc_List_List*)_check_null(*l))->tl);}}static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c){switch(c){case '\a': _LL0:
return({const char*_tmp2="\\a";_tag_dyneither(_tmp2,sizeof(char),3);});case '\b':
_LL1: return({const char*_tmp3="\\b";_tag_dyneither(_tmp3,sizeof(char),3);});case '\f':
_LL2: return({const char*_tmp4="\\f";_tag_dyneither(_tmp4,sizeof(char),3);});case '\n':
_LL3: return({const char*_tmp5="\\n";_tag_dyneither(_tmp5,sizeof(char),3);});case '\r':
_LL4: return({const char*_tmp6="\\r";_tag_dyneither(_tmp6,sizeof(char),3);});case '\t':
_LL5: return({const char*_tmp7="\\t";_tag_dyneither(_tmp7,sizeof(char),3);});case '\v':
_LL6: return({const char*_tmp8="\\v";_tag_dyneither(_tmp8,sizeof(char),3);});case '\\':
_LL7: return({const char*_tmp9="\\\\";_tag_dyneither(_tmp9,sizeof(char),3);});case
'"': _LL8: return({const char*_tmpA="\"";_tag_dyneither(_tmpA,sizeof(char),2);});
case '\'': _LL9: return({const char*_tmpB="\\'";_tag_dyneither(_tmpB,sizeof(char),3);});
default: _LLA: if(c >= ' '  && c <= '~'){struct _dyneither_ptr _tmpC=Cyc_Core_new_string(
2);({struct _dyneither_ptr _tmpD=_dyneither_ptr_plus(_tmpC,sizeof(char),0);char
_tmpE=*((char*)_check_dyneither_subscript(_tmpD,sizeof(char),0));char _tmpF=c;if(
_get_dyneither_size(_tmpD,sizeof(char))== 1  && (_tmpE == '\000'  && _tmpF != '\000'))
_throw_arraybounds();*((char*)_tmpD.curr)=_tmpF;});return(struct _dyneither_ptr)
_tmpC;}else{struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);int j=0;({struct
_dyneither_ptr _tmp11=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++);char _tmp12=*((
char*)_check_dyneither_subscript(_tmp11,sizeof(char),0));char _tmp13='\\';if(
_get_dyneither_size(_tmp11,sizeof(char))== 1  && (_tmp12 == '\000'  && _tmp13 != '\000'))
_throw_arraybounds();*((char*)_tmp11.curr)=_tmp13;});({struct _dyneither_ptr
_tmp14=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++);char _tmp15=*((char*)
_check_dyneither_subscript(_tmp14,sizeof(char),0));char _tmp16=(char)('0' + ((
unsigned char)c >> 6 & 3));if(_get_dyneither_size(_tmp14,sizeof(char))== 1  && (
_tmp15 == '\000'  && _tmp16 != '\000'))_throw_arraybounds();*((char*)_tmp14.curr)=
_tmp16;});({struct _dyneither_ptr _tmp17=_dyneither_ptr_plus(_tmp10,sizeof(char),j
++);char _tmp18=*((char*)_check_dyneither_subscript(_tmp17,sizeof(char),0));char
_tmp19=(char)('0' + (c >> 3 & 7));if(_get_dyneither_size(_tmp17,sizeof(char))== 1
 && (_tmp18 == '\000'  && _tmp19 != '\000'))_throw_arraybounds();*((char*)_tmp17.curr)=
_tmp19;});({struct _dyneither_ptr _tmp1A=_dyneither_ptr_plus(_tmp10,sizeof(char),j
++);char _tmp1B=*((char*)_check_dyneither_subscript(_tmp1A,sizeof(char),0));char
_tmp1C=(char)('0' + (c & 7));if(_get_dyneither_size(_tmp1A,sizeof(char))== 1  && (
_tmp1B == '\000'  && _tmp1C != '\000'))_throw_arraybounds();*((char*)_tmp1A.curr)=
_tmp1C;});return(struct _dyneither_ptr)_tmp10;}}}static int Cyc_Absynpp_special(
struct _dyneither_ptr s){int sz=(int)(_get_dyneither_size(s,sizeof(char))- 1);{int i=
0;for(0;i < sz;++ i){char c=*((const char*)_check_dyneither_subscript(s,sizeof(char),
i));if(((c <= ' '  || c >= '~') || c == '"') || c == '\\')return 1;}}return 0;}struct
_dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr s){if(!Cyc_Absynpp_special(
s))return s;{int n=(int)(_get_dyneither_size(s,sizeof(char))- 1);if(n > 0  && *((
const char*)_check_dyneither_subscript(s,sizeof(char),n))== '\000')-- n;{int len=0;{
int i=0;for(0;i <= n;++ i){char _tmp1D=*((const char*)_check_dyneither_subscript(s,
sizeof(char),i));_LLD: if(_tmp1D != '\a')goto _LLF;_LLE: goto _LL10;_LLF: if(_tmp1D != '\b')
goto _LL11;_LL10: goto _LL12;_LL11: if(_tmp1D != '\f')goto _LL13;_LL12: goto _LL14;_LL13:
if(_tmp1D != '\n')goto _LL15;_LL14: goto _LL16;_LL15: if(_tmp1D != '\r')goto _LL17;
_LL16: goto _LL18;_LL17: if(_tmp1D != '\t')goto _LL19;_LL18: goto _LL1A;_LL19: if(_tmp1D
!= '\v')goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp1D != '\\')goto _LL1D;_LL1C: goto
_LL1E;_LL1D: if(_tmp1D != '"')goto _LL1F;_LL1E: len +=2;goto _LLC;_LL1F:;_LL20: if(
_tmp1D >= ' '  && _tmp1D <= '~')++ len;else{len +=4;}goto _LLC;_LLC:;}}{struct
_dyneither_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));int j=0;{int i=0;for(0;
i <= n;++ i){char _tmp1E=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:({struct _dyneither_ptr _tmp1F=
_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp20=*((char*)
_check_dyneither_subscript(_tmp1F,sizeof(char),0));char _tmp21='\\';if(
_get_dyneither_size(_tmp1F,sizeof(char))== 1  && (_tmp20 == '\000'  && _tmp21 != '\000'))
_throw_arraybounds();*((char*)_tmp1F.curr)=_tmp21;});({struct _dyneither_ptr
_tmp22=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp23=*((char*)
_check_dyneither_subscript(_tmp22,sizeof(char),0));char _tmp24='a';if(
_get_dyneither_size(_tmp22,sizeof(char))== 1  && (_tmp23 == '\000'  && _tmp24 != '\000'))
_throw_arraybounds();*((char*)_tmp22.curr)=_tmp24;});goto _LL21;_LL24: if(_tmp1E != '\b')
goto _LL26;_LL25:({struct _dyneither_ptr _tmp25=_dyneither_ptr_plus(t,sizeof(char),
j ++);char _tmp26=*((char*)_check_dyneither_subscript(_tmp25,sizeof(char),0));char
_tmp27='\\';if(_get_dyneither_size(_tmp25,sizeof(char))== 1  && (_tmp26 == '\000'
 && _tmp27 != '\000'))_throw_arraybounds();*((char*)_tmp25.curr)=_tmp27;});({
struct _dyneither_ptr _tmp28=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp29=*((
char*)_check_dyneither_subscript(_tmp28,sizeof(char),0));char _tmp2A='b';if(
_get_dyneither_size(_tmp28,sizeof(char))== 1  && (_tmp29 == '\000'  && _tmp2A != '\000'))
_throw_arraybounds();*((char*)_tmp28.curr)=_tmp2A;});goto _LL21;_LL26: if(_tmp1E != '\f')
goto _LL28;_LL27:({struct _dyneither_ptr _tmp2B=_dyneither_ptr_plus(t,sizeof(char),
j ++);char _tmp2C=*((char*)_check_dyneither_subscript(_tmp2B,sizeof(char),0));char
_tmp2D='\\';if(_get_dyneither_size(_tmp2B,sizeof(char))== 1  && (_tmp2C == '\000'
 && _tmp2D != '\000'))_throw_arraybounds();*((char*)_tmp2B.curr)=_tmp2D;});({
struct _dyneither_ptr _tmp2E=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp2F=*((
char*)_check_dyneither_subscript(_tmp2E,sizeof(char),0));char _tmp30='f';if(
_get_dyneither_size(_tmp2E,sizeof(char))== 1  && (_tmp2F == '\000'  && _tmp30 != '\000'))
_throw_arraybounds();*((char*)_tmp2E.curr)=_tmp30;});goto _LL21;_LL28: if(_tmp1E != '\n')
goto _LL2A;_LL29:({struct _dyneither_ptr _tmp31=_dyneither_ptr_plus(t,sizeof(char),
j ++);char _tmp32=*((char*)_check_dyneither_subscript(_tmp31,sizeof(char),0));char
_tmp33='\\';if(_get_dyneither_size(_tmp31,sizeof(char))== 1  && (_tmp32 == '\000'
 && _tmp33 != '\000'))_throw_arraybounds();*((char*)_tmp31.curr)=_tmp33;});({
struct _dyneither_ptr _tmp34=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp35=*((
char*)_check_dyneither_subscript(_tmp34,sizeof(char),0));char _tmp36='n';if(
_get_dyneither_size(_tmp34,sizeof(char))== 1  && (_tmp35 == '\000'  && _tmp36 != '\000'))
_throw_arraybounds();*((char*)_tmp34.curr)=_tmp36;});goto _LL21;_LL2A: if(_tmp1E != '\r')
goto _LL2C;_LL2B:({struct _dyneither_ptr _tmp37=_dyneither_ptr_plus(t,sizeof(char),
j ++);char _tmp38=*((char*)_check_dyneither_subscript(_tmp37,sizeof(char),0));char
_tmp39='\\';if(_get_dyneither_size(_tmp37,sizeof(char))== 1  && (_tmp38 == '\000'
 && _tmp39 != '\000'))_throw_arraybounds();*((char*)_tmp37.curr)=_tmp39;});({
struct _dyneither_ptr _tmp3A=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp3B=*((
char*)_check_dyneither_subscript(_tmp3A,sizeof(char),0));char _tmp3C='r';if(
_get_dyneither_size(_tmp3A,sizeof(char))== 1  && (_tmp3B == '\000'  && _tmp3C != '\000'))
_throw_arraybounds();*((char*)_tmp3A.curr)=_tmp3C;});goto _LL21;_LL2C: if(_tmp1E != '\t')
goto _LL2E;_LL2D:({struct _dyneither_ptr _tmp3D=_dyneither_ptr_plus(t,sizeof(char),
j ++);char _tmp3E=*((char*)_check_dyneither_subscript(_tmp3D,sizeof(char),0));char
_tmp3F='\\';if(_get_dyneither_size(_tmp3D,sizeof(char))== 1  && (_tmp3E == '\000'
 && _tmp3F != '\000'))_throw_arraybounds();*((char*)_tmp3D.curr)=_tmp3F;});({
struct _dyneither_ptr _tmp40=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp41=*((
char*)_check_dyneither_subscript(_tmp40,sizeof(char),0));char _tmp42='t';if(
_get_dyneither_size(_tmp40,sizeof(char))== 1  && (_tmp41 == '\000'  && _tmp42 != '\000'))
_throw_arraybounds();*((char*)_tmp40.curr)=_tmp42;});goto _LL21;_LL2E: if(_tmp1E != '\v')
goto _LL30;_LL2F:({struct _dyneither_ptr _tmp43=_dyneither_ptr_plus(t,sizeof(char),
j ++);char _tmp44=*((char*)_check_dyneither_subscript(_tmp43,sizeof(char),0));char
_tmp45='\\';if(_get_dyneither_size(_tmp43,sizeof(char))== 1  && (_tmp44 == '\000'
 && _tmp45 != '\000'))_throw_arraybounds();*((char*)_tmp43.curr)=_tmp45;});({
struct _dyneither_ptr _tmp46=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp47=*((
char*)_check_dyneither_subscript(_tmp46,sizeof(char),0));char _tmp48='v';if(
_get_dyneither_size(_tmp46,sizeof(char))== 1  && (_tmp47 == '\000'  && _tmp48 != '\000'))
_throw_arraybounds();*((char*)_tmp46.curr)=_tmp48;});goto _LL21;_LL30: if(_tmp1E != '\\')
goto _LL32;_LL31:({struct _dyneither_ptr _tmp49=_dyneither_ptr_plus(t,sizeof(char),
j ++);char _tmp4A=*((char*)_check_dyneither_subscript(_tmp49,sizeof(char),0));char
_tmp4B='\\';if(_get_dyneither_size(_tmp49,sizeof(char))== 1  && (_tmp4A == '\000'
 && _tmp4B != '\000'))_throw_arraybounds();*((char*)_tmp49.curr)=_tmp4B;});({
struct _dyneither_ptr _tmp4C=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp4D=*((
char*)_check_dyneither_subscript(_tmp4C,sizeof(char),0));char _tmp4E='\\';if(
_get_dyneither_size(_tmp4C,sizeof(char))== 1  && (_tmp4D == '\000'  && _tmp4E != '\000'))
_throw_arraybounds();*((char*)_tmp4C.curr)=_tmp4E;});goto _LL21;_LL32: if(_tmp1E != '"')
goto _LL34;_LL33:({struct _dyneither_ptr _tmp4F=_dyneither_ptr_plus(t,sizeof(char),
j ++);char _tmp50=*((char*)_check_dyneither_subscript(_tmp4F,sizeof(char),0));char
_tmp51='\\';if(_get_dyneither_size(_tmp4F,sizeof(char))== 1  && (_tmp50 == '\000'
 && _tmp51 != '\000'))_throw_arraybounds();*((char*)_tmp4F.curr)=_tmp51;});({
struct _dyneither_ptr _tmp52=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp53=*((
char*)_check_dyneither_subscript(_tmp52,sizeof(char),0));char _tmp54='"';if(
_get_dyneither_size(_tmp52,sizeof(char))== 1  && (_tmp53 == '\000'  && _tmp54 != '\000'))
_throw_arraybounds();*((char*)_tmp52.curr)=_tmp54;});goto _LL21;_LL34:;_LL35: if(
_tmp1E >= ' '  && _tmp1E <= '~')({struct _dyneither_ptr _tmp55=_dyneither_ptr_plus(t,
sizeof(char),j ++);char _tmp56=*((char*)_check_dyneither_subscript(_tmp55,sizeof(
char),0));char _tmp57=_tmp1E;if(_get_dyneither_size(_tmp55,sizeof(char))== 1  && (
_tmp56 == '\000'  && _tmp57 != '\000'))_throw_arraybounds();*((char*)_tmp55.curr)=
_tmp57;});else{({struct _dyneither_ptr _tmp58=_dyneither_ptr_plus(t,sizeof(char),j
++);char _tmp59=*((char*)_check_dyneither_subscript(_tmp58,sizeof(char),0));char
_tmp5A='\\';if(_get_dyneither_size(_tmp58,sizeof(char))== 1  && (_tmp59 == '\000'
 && _tmp5A != '\000'))_throw_arraybounds();*((char*)_tmp58.curr)=_tmp5A;});({
struct _dyneither_ptr _tmp5B=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp5C=*((
char*)_check_dyneither_subscript(_tmp5B,sizeof(char),0));char _tmp5D=(char)('0' + (
_tmp1E >> 6 & 7));if(_get_dyneither_size(_tmp5B,sizeof(char))== 1  && (_tmp5C == '\000'
 && _tmp5D != '\000'))_throw_arraybounds();*((char*)_tmp5B.curr)=_tmp5D;});({
struct _dyneither_ptr _tmp5E=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp5F=*((
char*)_check_dyneither_subscript(_tmp5E,sizeof(char),0));char _tmp60=(char)('0' + (
_tmp1E >> 3 & 7));if(_get_dyneither_size(_tmp5E,sizeof(char))== 1  && (_tmp5F == '\000'
 && _tmp60 != '\000'))_throw_arraybounds();*((char*)_tmp5E.curr)=_tmp60;});({
struct _dyneither_ptr _tmp61=_dyneither_ptr_plus(t,sizeof(char),j ++);char _tmp62=*((
char*)_check_dyneither_subscript(_tmp61,sizeof(char),0));char _tmp63=(char)('0' + (
_tmp1E & 7));if(_get_dyneither_size(_tmp61,sizeof(char))== 1  && (_tmp62 == '\000'
 && _tmp63 != '\000'))_throw_arraybounds();*((char*)_tmp61.curr)=_tmp63;});}goto
_LL21;_LL21:;}}return(struct _dyneither_ptr)t;}}}}static char _tmp64[9]="restrict";
static struct _dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};
static char _tmp65[9]="volatile";static struct _dyneither_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dyneither_ptr
Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string;static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*
l=0;if(tq.q_restrict)l=({struct Cyc_List_List*_tmp67=_cycalloc(sizeof(*_tmp67));
_tmp67->hd=Cyc_Absynpp_restrict_sp;_tmp67->tl=l;_tmp67;});if(tq.q_volatile)l=({
struct Cyc_List_List*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->hd=Cyc_Absynpp_volatile_sp;
_tmp68->tl=l;_tmp68;});if(tq.print_const)l=({struct Cyc_List_List*_tmp69=
_cycalloc(sizeof(*_tmp69));_tmp69->hd=Cyc_Absynpp_const_sp;_tmp69->tl=l;_tmp69;});
return Cyc_PP_egroup(({const char*_tmp6A="";_tag_dyneither(_tmp6A,sizeof(char),1);}),({
const char*_tmp6B=" ";_tag_dyneither(_tmp6B,sizeof(char),2);}),({const char*_tmp6C=" ";
_tag_dyneither(_tmp6C,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));}
struct _dyneither_ptr Cyc_Absynpp_kind2string(void*k){void*_tmp6D=k;_LL37: if((int)
_tmp6D != 0)goto _LL39;_LL38: return({const char*_tmp6E="A";_tag_dyneither(_tmp6E,
sizeof(char),2);});_LL39: if((int)_tmp6D != 1)goto _LL3B;_LL3A: return({const char*
_tmp6F="M";_tag_dyneither(_tmp6F,sizeof(char),2);});_LL3B: if((int)_tmp6D != 2)
goto _LL3D;_LL3C: return({const char*_tmp70="B";_tag_dyneither(_tmp70,sizeof(char),
2);});_LL3D: if((int)_tmp6D != 3)goto _LL3F;_LL3E: return({const char*_tmp71="R";
_tag_dyneither(_tmp71,sizeof(char),2);});_LL3F: if((int)_tmp6D != 4)goto _LL41;
_LL40: return({const char*_tmp72="UR";_tag_dyneither(_tmp72,sizeof(char),3);});
_LL41: if((int)_tmp6D != 5)goto _LL43;_LL42: return({const char*_tmp73="TR";
_tag_dyneither(_tmp73,sizeof(char),3);});_LL43: if((int)_tmp6D != 6)goto _LL45;
_LL44: return({const char*_tmp74="E";_tag_dyneither(_tmp74,sizeof(char),2);});
_LL45: if((int)_tmp6D != 7)goto _LL36;_LL46: return({const char*_tmp75="I";
_tag_dyneither(_tmp75,sizeof(char),2);});_LL36:;}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(
void*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}struct _dyneither_ptr Cyc_Absynpp_kindbound2string(
void*c){void*_tmp76=Cyc_Absyn_compress_kb(c);void*_tmp77;void*_tmp78;_LL48: if(*((
int*)_tmp76)!= 0)goto _LL4A;_tmp77=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp76)->f1;
_LL49: return Cyc_Absynpp_kind2string(_tmp77);_LL4A: if(*((int*)_tmp76)!= 1)goto
_LL4C;_LL4B: if(Cyc_PP_tex_output)return({const char*_tmp79="{?}";_tag_dyneither(
_tmp79,sizeof(char),4);});else{return({const char*_tmp7A="?";_tag_dyneither(
_tmp7A,sizeof(char),2);});}_LL4C: if(*((int*)_tmp76)!= 2)goto _LL47;_tmp78=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp76)->f2;_LL4D: return Cyc_Absynpp_kind2string(
_tmp78);_LL47:;}struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){void*_tmp7B=
Cyc_Absyn_compress_kb(c);void*_tmp7C;void*_tmp7D;_LL4F: if(*((int*)_tmp7B)!= 0)
goto _LL51;_tmp7C=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp7B)->f1;_LL50: return
Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7C));_LL51: if(*((int*)_tmp7B)!= 1)goto
_LL53;_LL52: if(Cyc_PP_tex_output)return Cyc_PP_text_width(({const char*_tmp7E="{?}";
_tag_dyneither(_tmp7E,sizeof(char),4);}),1);else{return Cyc_PP_text(({const char*
_tmp7F="?";_tag_dyneither(_tmp7F,sizeof(char),2);}));}_LL53: if(*((int*)_tmp7B)!= 
2)goto _LL4E;_tmp7D=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp7B)->f2;_LL54:
return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7D));_LL4E:;}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(
struct Cyc_List_List*ts){return Cyc_PP_egroup(({const char*_tmp80="<";
_tag_dyneither(_tmp80,sizeof(char),2);}),({const char*_tmp81=">";_tag_dyneither(
_tmp81,sizeof(char),2);}),({const char*_tmp82=",";_tag_dyneither(_tmp82,sizeof(
char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(
struct Cyc_Absyn_Tvar*tv){void*_tmp83=Cyc_Absyn_compress_kb((void*)tv->kind);void*
_tmp84;void*_tmp85;void*_tmp86;_LL56: if(*((int*)_tmp83)!= 1)goto _LL58;_LL57: goto
_LL59;_LL58: if(*((int*)_tmp83)!= 0)goto _LL5A;_tmp84=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp83)->f1;if((int)_tmp84 != 2)goto _LL5A;_LL59: return Cyc_PP_textptr(tv->name);
_LL5A: if(*((int*)_tmp83)!= 2)goto _LL5C;_tmp85=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp83)->f2;_LL5B: _tmp86=_tmp85;goto _LL5D;_LL5C: if(*((int*)_tmp83)!= 0)goto _LL55;
_tmp86=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp83)->f1;_LL5D: return({struct
Cyc_PP_Doc*_tmp87[3];_tmp87[2]=Cyc_Absynpp_kind2doc(_tmp86);_tmp87[1]=Cyc_PP_text(({
const char*_tmp88="::";_tag_dyneither(_tmp88,sizeof(char),3);}));_tmp87[0]=Cyc_PP_textptr(
tv->name);Cyc_PP_cat(_tag_dyneither(_tmp87,sizeof(struct Cyc_PP_Doc*),3));});
_LL55:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){return
Cyc_PP_egroup(({const char*_tmp89="<";_tag_dyneither(_tmp89,sizeof(char),2);}),({
const char*_tmp8A=">";_tag_dyneither(_tmp8A,sizeof(char),2);}),({const char*_tmp8B=",";
_tag_dyneither(_tmp8B,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,
tvs));}static struct _dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){
return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)return Cyc_Absynpp_ktvars2doc(tvs);return Cyc_PP_egroup(({
const char*_tmp8C="<";_tag_dyneither(_tmp8C,sizeof(char),2);}),({const char*_tmp8D=">";
_tag_dyneither(_tmp8D,sizeof(char),2);}),({const char*_tmp8E=",";_tag_dyneither(
_tmp8E,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(
struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_get_name,tvs)));}struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple10*t){return Cyc_Absynpp_tqtd2doc((*
t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return Cyc_PP_group(({const char*_tmp8F="(";_tag_dyneither(_tmp8F,sizeof(char),2);}),({
const char*_tmp90=")";_tag_dyneither(_tmp90,sizeof(char),2);}),({const char*_tmp91=",";
_tag_dyneither(_tmp91,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){void*_tmp92=att;_LL5F: if((
int)_tmp92 != 0)goto _LL61;_LL60: goto _LL62;_LL61: if((int)_tmp92 != 1)goto _LL63;
_LL62: goto _LL64;_LL63: if((int)_tmp92 != 2)goto _LL65;_LL64: return Cyc_PP_nil_doc();
_LL65:;_LL66: return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL5E:;}struct
Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){for(0;atts != 0;
atts=atts->tl){void*_tmp93=(void*)atts->hd;_LL68: if((int)_tmp93 != 0)goto _LL6A;
_LL69: return Cyc_PP_text(({const char*_tmp94=" _stdcall ";_tag_dyneither(_tmp94,
sizeof(char),11);}));_LL6A: if((int)_tmp93 != 1)goto _LL6C;_LL6B: return Cyc_PP_text(({
const char*_tmp95=" _cdecl ";_tag_dyneither(_tmp95,sizeof(char),9);}));_LL6C: if((
int)_tmp93 != 2)goto _LL6E;_LL6D: return Cyc_PP_text(({const char*_tmp96=" _fastcall ";
_tag_dyneither(_tmp96,sizeof(char),12);}));_LL6E:;_LL6F: goto _LL67;_LL67:;}return
Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*
atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp97=(void*)atts2->hd;_LL71: if((int)_tmp97 != 0)goto _LL73;_LL72: goto _LL74;
_LL73: if((int)_tmp97 != 1)goto _LL75;_LL74: goto _LL76;_LL75: if((int)_tmp97 != 2)goto
_LL77;_LL76: goto _LL70;_LL77:;_LL78: hasatt=1;goto _LL70;_LL70:;}}if(!hasatt)return
Cyc_PP_nil_doc();return({struct Cyc_PP_Doc*_tmp98[3];_tmp98[2]=Cyc_PP_text(({
const char*_tmp9D=")";_tag_dyneither(_tmp9D,sizeof(char),2);}));_tmp98[1]=Cyc_PP_group(({
const char*_tmp9A="";_tag_dyneither(_tmp9A,sizeof(char),1);}),({const char*_tmp9B="";
_tag_dyneither(_tmp9B,sizeof(char),1);}),({const char*_tmp9C=" ";_tag_dyneither(
_tmp9C,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));_tmp98[0]=
Cyc_PP_text(({const char*_tmp99=" __declspec(";_tag_dyneither(_tmp99,sizeof(char),
13);}));Cyc_PP_cat(_tag_dyneither(_tmp98,sizeof(struct Cyc_PP_Doc*),3));});}
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(
a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){if(atts == 0)
return Cyc_PP_nil_doc();{void*_tmp9E=Cyc_Cyclone_c_compiler;_LL7A: if((int)_tmp9E
!= 1)goto _LL7C;_LL7B: return Cyc_Absynpp_noncallconv2doc(atts);_LL7C: if((int)
_tmp9E != 0)goto _LL79;_LL7D: return({struct Cyc_PP_Doc*_tmp9F[2];_tmp9F[1]=Cyc_PP_group(({
const char*_tmpA1="((";_tag_dyneither(_tmpA1,sizeof(char),3);}),({const char*
_tmpA2="))";_tag_dyneither(_tmpA2,sizeof(char),3);}),({const char*_tmpA3=",";
_tag_dyneither(_tmpA3,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));_tmp9F[
0]=Cyc_PP_text(({const char*_tmpA0=" __attribute__";_tag_dyneither(_tmpA0,sizeof(
char),15);}));Cyc_PP_cat(_tag_dyneither(_tmp9F,sizeof(struct Cyc_PP_Doc*),2));});
_LL79:;}}int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){if(tms == 0)
return 0;{void*_tmpA4=(void*)tms->hd;_LL7F: if(*((int*)_tmpA4)!= 2)goto _LL81;_LL80:
return 1;_LL81: if(*((int*)_tmpA4)!= 5)goto _LL83;_LL82: {void*_tmpA5=Cyc_Cyclone_c_compiler;
_LL86: if((int)_tmpA5 != 0)goto _LL88;_LL87: return 0;_LL88:;_LL89: return Cyc_Absynpp_next_is_pointer(
tms->tl);_LL85:;}_LL83:;_LL84: return 0;_LL7E:;}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(
void*t);static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*
Cyc_Absynpp_question(){if(!((unsigned int)Cyc_Absynpp_cache_question)){if(Cyc_PP_tex_output)
Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*
_tmpA6="{?}";_tag_dyneither(_tmpA6,sizeof(char),4);}),1);else{Cyc_Absynpp_cache_question=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpA7="?";_tag_dyneither(_tmpA7,
sizeof(char),2);}));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_lb=(
struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*_tmpA8="{\\lb}";_tag_dyneither(
_tmpA8,sizeof(char),6);}),1);else{Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text(({
const char*_tmpA9="{";_tag_dyneither(_tmpA9,sizeof(char),2);}));}}return(struct
Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output)Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(({
const char*_tmpAA="{\\rb}";_tag_dyneither(_tmpAA,sizeof(char),6);}),1);else{Cyc_Absynpp_cache_rb=(
struct Cyc_PP_Doc*)Cyc_PP_text(({const char*_tmpAB="}";_tag_dyneither(_tmpAB,
sizeof(char),2);}));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){if(Cyc_PP_tex_output)Cyc_Absynpp_cache_dollar=(
struct Cyc_PP_Doc*)Cyc_PP_text_width(({const char*_tmpAC="\\$";_tag_dyneither(
_tmpAC,sizeof(char),3);}),1);else{Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)
Cyc_PP_text(({const char*_tmpAD="$";_tag_dyneither(_tmpAD,sizeof(char),2);}));}}
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}struct Cyc_PP_Doc*
Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){return({
struct Cyc_PP_Doc*_tmpAE[3];_tmpAE[2]=Cyc_Absynpp_rb();_tmpAE[1]=Cyc_PP_seq(sep,
ss);_tmpAE[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmpAE,sizeof(struct Cyc_PP_Doc*),
3));});}static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){for(0;tms != 0;
tms=tms->tl){void*_tmpAF=(void*)tms->hd;void*_tmpB0;struct Cyc_List_List*_tmpB1;
_LL8B: if(*((int*)_tmpAF)!= 0)goto _LL8D;_LL8C:({void*_tmpB2=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmpB3="Carray_mod ";_tag_dyneither(_tmpB3,sizeof(char),12);}),
_tag_dyneither(_tmpB2,sizeof(void*),0));});goto _LL8A;_LL8D: if(*((int*)_tmpAF)!= 
1)goto _LL8F;_LL8E:({void*_tmpB4=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpB5="ConstArray_mod ";
_tag_dyneither(_tmpB5,sizeof(char),16);}),_tag_dyneither(_tmpB4,sizeof(void*),0));});
goto _LL8A;_LL8F: if(*((int*)_tmpAF)!= 3)goto _LL91;_tmpB0=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpAF)->f1;if(*((int*)_tmpB0)!= 1)goto _LL91;_tmpB1=((struct Cyc_Absyn_WithTypes_struct*)
_tmpB0)->f1;_LL90:({void*_tmpB6=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpB7="Function_mod(";
_tag_dyneither(_tmpB7,sizeof(char),14);}),_tag_dyneither(_tmpB6,sizeof(void*),0));});
for(0;_tmpB1 != 0;_tmpB1=_tmpB1->tl){struct Cyc_Core_Opt*_tmpB8=(*((struct _tuple1*)
_tmpB1->hd)).f1;if(_tmpB8 == 0)({void*_tmpB9=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmpBA="?";_tag_dyneither(_tmpBA,sizeof(char),2);}),_tag_dyneither(_tmpB9,
sizeof(void*),0));});else{({void*_tmpBB=0;Cyc_fprintf(Cyc_stderr,*((struct
_dyneither_ptr*)_tmpB8->v),_tag_dyneither(_tmpBB,sizeof(void*),0));});}if(_tmpB1->tl
!= 0)({void*_tmpBC=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpBD=",";
_tag_dyneither(_tmpBD,sizeof(char),2);}),_tag_dyneither(_tmpBC,sizeof(void*),0));});}({
void*_tmpBE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpBF=") ";_tag_dyneither(
_tmpBF,sizeof(char),3);}),_tag_dyneither(_tmpBE,sizeof(void*),0));});goto _LL8A;
_LL91: if(*((int*)_tmpAF)!= 3)goto _LL93;_LL92:({void*_tmpC0=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmpC1="Function_mod()";_tag_dyneither(_tmpC1,sizeof(char),15);}),
_tag_dyneither(_tmpC0,sizeof(void*),0));});goto _LL8A;_LL93: if(*((int*)_tmpAF)!= 
5)goto _LL95;_LL94:({void*_tmpC2=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpC3="Attributes_mod ";
_tag_dyneither(_tmpC3,sizeof(char),16);}),_tag_dyneither(_tmpC2,sizeof(void*),0));});
goto _LL8A;_LL95: if(*((int*)_tmpAF)!= 4)goto _LL97;_LL96:({void*_tmpC4=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmpC5="TypeParams_mod ";_tag_dyneither(_tmpC5,sizeof(
char),16);}),_tag_dyneither(_tmpC4,sizeof(void*),0));});goto _LL8A;_LL97: if(*((
int*)_tmpAF)!= 2)goto _LL8A;_LL98:({void*_tmpC6=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmpC7="Pointer_mod ";_tag_dyneither(_tmpC7,sizeof(char),13);}),
_tag_dyneither(_tmpC6,sizeof(void*),0));});goto _LL8A;_LL8A:;}({void*_tmpC8=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmpC9="\n";_tag_dyneither(_tmpC9,sizeof(char),2);}),
_tag_dyneither(_tmpC8,sizeof(void*),0));});}struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(
int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);struct Cyc_PP_Doc*p_rest=({
struct Cyc_PP_Doc*_tmp103[3];_tmp103[2]=Cyc_PP_text(({const char*_tmp105=")";
_tag_dyneither(_tmp105,sizeof(char),2);}));_tmp103[1]=rest;_tmp103[0]=Cyc_PP_text(({
const char*_tmp104="(";_tag_dyneither(_tmp104,sizeof(char),2);}));Cyc_PP_cat(
_tag_dyneither(_tmp103,sizeof(struct Cyc_PP_Doc*),3));});void*_tmpCA=(void*)tms->hd;
struct Cyc_Absyn_Conref*_tmpCB;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Conref*
_tmpCD;void*_tmpCE;struct Cyc_List_List*_tmpCF;struct Cyc_List_List*_tmpD0;struct
Cyc_Position_Segment*_tmpD1;int _tmpD2;struct Cyc_Absyn_PtrAtts _tmpD3;void*_tmpD4;
struct Cyc_Absyn_Conref*_tmpD5;struct Cyc_Absyn_Conref*_tmpD6;struct Cyc_Absyn_Conref*
_tmpD7;struct Cyc_Absyn_Tqual _tmpD8;_LL9A: if(*((int*)_tmpCA)!= 0)goto _LL9C;_tmpCB=((
struct Cyc_Absyn_Carray_mod_struct*)_tmpCA)->f1;_LL9B: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*_tmpD9[2];_tmpD9[1]=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpCB)?Cyc_PP_text(({const
char*_tmpDA="[]@zeroterm";_tag_dyneither(_tmpDA,sizeof(char),12);})): Cyc_PP_text(({
const char*_tmpDB="[]";_tag_dyneither(_tmpDB,sizeof(char),3);}));_tmpD9[0]=rest;
Cyc_PP_cat(_tag_dyneither(_tmpD9,sizeof(struct Cyc_PP_Doc*),2));});_LL9C: if(*((
int*)_tmpCA)!= 1)goto _LL9E;_tmpCC=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpCA)->f1;_tmpCD=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCA)->f2;_LL9D:
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*
_tmpDC[4];_tmpDC[3]=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpCD)?Cyc_PP_text(({const char*_tmpDE="]@zeroterm";_tag_dyneither(_tmpDE,
sizeof(char),11);})): Cyc_PP_text(({const char*_tmpDF="]";_tag_dyneither(_tmpDF,
sizeof(char),2);}));_tmpDC[2]=Cyc_Absynpp_exp2doc(_tmpCC);_tmpDC[1]=Cyc_PP_text(({
const char*_tmpDD="[";_tag_dyneither(_tmpDD,sizeof(char),2);}));_tmpDC[0]=rest;
Cyc_PP_cat(_tag_dyneither(_tmpDC,sizeof(struct Cyc_PP_Doc*),4));});_LL9E: if(*((
int*)_tmpCA)!= 3)goto _LLA0;_tmpCE=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpCA)->f1;_LL9F: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{void*
_tmpE0=_tmpCE;struct Cyc_List_List*_tmpE1;int _tmpE2;struct Cyc_Absyn_VarargInfo*
_tmpE3;struct Cyc_Core_Opt*_tmpE4;struct Cyc_List_List*_tmpE5;struct Cyc_List_List*
_tmpE6;struct Cyc_Position_Segment*_tmpE7;_LLA7: if(*((int*)_tmpE0)!= 1)goto _LLA9;
_tmpE1=((struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f1;_tmpE2=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE0)->f2;_tmpE3=((struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f3;_tmpE4=((
struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f4;_tmpE5=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE0)->f5;_LLA8: return({struct Cyc_PP_Doc*_tmpE8[2];_tmpE8[1]=Cyc_Absynpp_funargs2doc(
_tmpE1,_tmpE2,_tmpE3,_tmpE4,_tmpE5);_tmpE8[0]=rest;Cyc_PP_cat(_tag_dyneither(
_tmpE8,sizeof(struct Cyc_PP_Doc*),2));});_LLA9: if(*((int*)_tmpE0)!= 0)goto _LLA6;
_tmpE6=((struct Cyc_Absyn_NoTypes_struct*)_tmpE0)->f1;_tmpE7=((struct Cyc_Absyn_NoTypes_struct*)
_tmpE0)->f2;_LLAA: return({struct Cyc_PP_Doc*_tmpE9[2];_tmpE9[1]=Cyc_PP_group(({
const char*_tmpEA="(";_tag_dyneither(_tmpEA,sizeof(char),2);}),({const char*_tmpEB=")";
_tag_dyneither(_tmpEB,sizeof(char),2);}),({const char*_tmpEC=",";_tag_dyneither(
_tmpEC,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmpE6));
_tmpE9[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmpE9,sizeof(struct Cyc_PP_Doc*),2));});
_LLA6:;}_LLA0: if(*((int*)_tmpCA)!= 5)goto _LLA2;_tmpCF=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpCA)->f2;_LLA1: {void*_tmpED=Cyc_Cyclone_c_compiler;_LLAC: if((int)_tmpED != 0)
goto _LLAE;_LLAD: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;return({
struct Cyc_PP_Doc*_tmpEE[2];_tmpEE[1]=Cyc_Absynpp_atts2doc(_tmpCF);_tmpEE[0]=rest;
Cyc_PP_cat(_tag_dyneither(_tmpEE,sizeof(struct Cyc_PP_Doc*),2));});_LLAE: if((int)
_tmpED != 1)goto _LLAB;_LLAF: if(Cyc_Absynpp_next_is_pointer(tms->tl))return({
struct Cyc_PP_Doc*_tmpEF[2];_tmpEF[1]=rest;_tmpEF[0]=Cyc_Absynpp_callconv2doc(
_tmpCF);Cyc_PP_cat(_tag_dyneither(_tmpEF,sizeof(struct Cyc_PP_Doc*),2));});return
rest;_LLAB:;}_LLA2: if(*((int*)_tmpCA)!= 4)goto _LLA4;_tmpD0=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpCA)->f1;_tmpD1=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCA)->f2;_tmpD2=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCA)->f3;_LLA3: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;if(_tmpD2)return({struct Cyc_PP_Doc*_tmpF0[2];_tmpF0[1]=Cyc_Absynpp_ktvars2doc(
_tmpD0);_tmpF0[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmpF0,sizeof(struct Cyc_PP_Doc*),
2));});else{return({struct Cyc_PP_Doc*_tmpF1[2];_tmpF1[1]=Cyc_Absynpp_tvars2doc(
_tmpD0);_tmpF1[0]=rest;Cyc_PP_cat(_tag_dyneither(_tmpF1,sizeof(struct Cyc_PP_Doc*),
2));});}_LLA4: if(*((int*)_tmpCA)!= 2)goto _LL99;_tmpD3=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpCA)->f1;_tmpD4=(void*)_tmpD3.rgn;_tmpD5=_tmpD3.nullable;_tmpD6=_tmpD3.bounds;
_tmpD7=_tmpD3.zero_term;_tmpD8=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCA)->f2;
_LLA5: {struct Cyc_PP_Doc*ptr;{void*_tmpF2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmpD6);struct Cyc_Absyn_Exp*_tmpF3;_LLB1: if((int)_tmpF2 != 0)goto _LLB3;_LLB2: ptr=
Cyc_Absynpp_question();goto _LLB0;_LLB3: if(_tmpF2 <= (void*)1)goto _LLB0;if(*((int*)
_tmpF2)!= 0)goto _LLB0;_tmpF3=((struct Cyc_Absyn_Upper_b_struct*)_tmpF2)->f1;_LLB4:
ptr=Cyc_PP_text(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,
_tmpD5)?({const char*_tmpF4="*";_tag_dyneither(_tmpF4,sizeof(char),2);}):({const
char*_tmpF5="@";_tag_dyneither(_tmpF5,sizeof(char),2);}));{unsigned int _tmpF7;
int _tmpF8;struct _tuple4 _tmpF6=Cyc_Evexp_eval_const_uint_exp(_tmpF3);_tmpF7=
_tmpF6.f1;_tmpF8=_tmpF6.f2;if(!_tmpF8  || _tmpF7 != 1)ptr=({struct Cyc_PP_Doc*
_tmpF9[4];_tmpF9[3]=Cyc_Absynpp_rb();_tmpF9[2]=Cyc_Absynpp_exp2doc(_tmpF3);
_tmpF9[1]=Cyc_Absynpp_lb();_tmpF9[0]=ptr;Cyc_PP_cat(_tag_dyneither(_tmpF9,
sizeof(struct Cyc_PP_Doc*),4));});goto _LLB0;}_LLB0:;}if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpD7))ptr=({struct Cyc_PP_Doc*_tmpFA[2];_tmpFA[1]=Cyc_PP_text(({const char*
_tmpFB="@zeroterm ";_tag_dyneither(_tmpFB,sizeof(char),11);}));_tmpFA[0]=ptr;Cyc_PP_cat(
_tag_dyneither(_tmpFA,sizeof(struct Cyc_PP_Doc*),2));});else{if(is_char_ptr  && !((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpD7))ptr=({
struct Cyc_PP_Doc*_tmpFC[2];_tmpFC[1]=Cyc_PP_text(({const char*_tmpFD="@nozeroterm ";
_tag_dyneither(_tmpFD,sizeof(char),13);}));_tmpFC[0]=ptr;Cyc_PP_cat(
_tag_dyneither(_tmpFC,sizeof(struct Cyc_PP_Doc*),2));});}}{void*_tmpFE=Cyc_Tcutil_compress(
_tmpD4);_LLB6: if((int)_tmpFE != 2)goto _LLB8;_LLB7: goto _LLB5;_LLB8:;_LLB9: ptr=({
struct Cyc_PP_Doc*_tmpFF[3];_tmpFF[2]=Cyc_PP_text(({const char*_tmp100=" ";
_tag_dyneither(_tmp100,sizeof(char),2);}));_tmpFF[1]=Cyc_Absynpp_typ2doc(_tmpD4);
_tmpFF[0]=ptr;Cyc_PP_cat(_tag_dyneither(_tmpFF,sizeof(struct Cyc_PP_Doc*),3));});
_LLB5:;}ptr=({struct Cyc_PP_Doc*_tmp101[2];_tmp101[1]=Cyc_Absynpp_tqual2doc(
_tmpD8);_tmp101[0]=ptr;Cyc_PP_cat(_tag_dyneither(_tmp101,sizeof(struct Cyc_PP_Doc*),
2));});return({struct Cyc_PP_Doc*_tmp102[2];_tmp102[1]=rest;_tmp102[0]=ptr;Cyc_PP_cat(
_tag_dyneither(_tmp102,sizeof(struct Cyc_PP_Doc*),2));});}_LL99:;}}struct Cyc_PP_Doc*
Cyc_Absynpp_rgn2doc(void*t){void*_tmp106=Cyc_Tcutil_compress(t);_LLBB: if((int)
_tmp106 != 2)goto _LLBD;_LLBC: return Cyc_PP_text(({const char*_tmp107="`H";
_tag_dyneither(_tmp107,sizeof(char),3);}));_LLBD: if((int)_tmp106 != 3)goto _LLBF;
_LLBE: return Cyc_PP_text(({const char*_tmp108="`U";_tag_dyneither(_tmp108,sizeof(
char),3);}));_LLBF:;_LLC0: return Cyc_Absynpp_ntyp2doc(t);_LLBA:;}static void Cyc_Absynpp_effects2docs(
struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){void*_tmp109=
Cyc_Tcutil_compress(t);void*_tmp10A;struct Cyc_List_List*_tmp10B;_LLC2: if(_tmp109
<= (void*)4)goto _LLC6;if(*((int*)_tmp109)!= 19)goto _LLC4;_tmp10A=(void*)((struct
Cyc_Absyn_AccessEff_struct*)_tmp109)->f1;_LLC3:*rgions=({struct Cyc_List_List*
_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->hd=Cyc_Absynpp_rgn2doc(_tmp10A);
_tmp10C->tl=*rgions;_tmp10C;});goto _LLC1;_LLC4: if(*((int*)_tmp109)!= 20)goto
_LLC6;_tmp10B=((struct Cyc_Absyn_JoinEff_struct*)_tmp109)->f1;_LLC5: for(0;_tmp10B
!= 0;_tmp10B=_tmp10B->tl){Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp10B->hd);}
goto _LLC1;_LLC6:;_LLC7:*effects=({struct Cyc_List_List*_tmp10D=_cycalloc(sizeof(*
_tmp10D));_tmp10D->hd=Cyc_Absynpp_typ2doc(t);_tmp10D->tl=*effects;_tmp10D;});
goto _LLC1;_LLC1:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct Cyc_List_List*
rgions=0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(& rgions,& effects,
t);rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
rgions);effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0)return Cyc_PP_group(({const char*_tmp10E="";
_tag_dyneither(_tmp10E,sizeof(char),1);}),({const char*_tmp10F="";_tag_dyneither(
_tmp10F,sizeof(char),1);}),({const char*_tmp110="+";_tag_dyneither(_tmp110,
sizeof(char),2);}),effects);else{struct Cyc_PP_Doc*_tmp111=Cyc_Absynpp_group_braces(({
const char*_tmp116=",";_tag_dyneither(_tmp116,sizeof(char),2);}),rgions);return
Cyc_PP_group(({const char*_tmp112="";_tag_dyneither(_tmp112,sizeof(char),1);}),({
const char*_tmp113="";_tag_dyneither(_tmp113,sizeof(char),1);}),({const char*
_tmp114="+";_tag_dyneither(_tmp114,sizeof(char),2);}),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,({
struct Cyc_List_List*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->hd=_tmp111;
_tmp115->tl=0;_tmp115;})));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(void*k){
void*_tmp117=k;_LLC9: if((int)_tmp117 != 0)goto _LLCB;_LLCA: return Cyc_PP_text(({
const char*_tmp118="struct ";_tag_dyneither(_tmp118,sizeof(char),8);}));_LLCB: if((
int)_tmp117 != 1)goto _LLC8;_LLCC: return Cyc_PP_text(({const char*_tmp119="union ";
_tag_dyneither(_tmp119,sizeof(char),7);}));_LLC8:;}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(
void*t){struct Cyc_PP_Doc*s;{void*_tmp11A=t;struct Cyc_Core_Opt*_tmp11B;struct Cyc_Core_Opt*
_tmp11C;int _tmp11D;struct Cyc_Core_Opt*_tmp11E;struct Cyc_Absyn_Tvar*_tmp11F;
struct Cyc_Absyn_DatatypeInfo _tmp120;union Cyc_Absyn_DatatypeInfoU_union _tmp121;
struct Cyc_List_List*_tmp122;struct Cyc_Core_Opt*_tmp123;struct Cyc_Absyn_DatatypeFieldInfo
_tmp124;union Cyc_Absyn_DatatypeFieldInfoU_union _tmp125;struct Cyc_List_List*
_tmp126;void*_tmp127;void*_tmp128;int _tmp129;struct Cyc_List_List*_tmp12A;struct
Cyc_Absyn_AggrInfo _tmp12B;union Cyc_Absyn_AggrInfoU_union _tmp12C;struct Cyc_List_List*
_tmp12D;void*_tmp12E;struct Cyc_List_List*_tmp12F;struct Cyc_List_List*_tmp130;
struct _tuple0*_tmp131;struct Cyc_Absyn_Exp*_tmp132;struct _tuple0*_tmp133;struct
Cyc_List_List*_tmp134;struct Cyc_Absyn_Typedefdecl*_tmp135;void*_tmp136;void*
_tmp137;void*_tmp138;void*_tmp139;void*_tmp13A;_LLCE: if(_tmp11A <= (void*)4)goto
_LLD4;if(*((int*)_tmp11A)!= 7)goto _LLD0;_LLCF: return Cyc_PP_text(({const char*
_tmp13B="[[[array]]]";_tag_dyneither(_tmp13B,sizeof(char),12);}));_LLD0: if(*((
int*)_tmp11A)!= 8)goto _LLD2;_LLD1: return Cyc_PP_nil_doc();_LLD2: if(*((int*)
_tmp11A)!= 4)goto _LLD4;_LLD3: return Cyc_PP_nil_doc();_LLD4: if((int)_tmp11A != 0)
goto _LLD6;_LLD5: s=Cyc_PP_text(({const char*_tmp13C="void";_tag_dyneither(_tmp13C,
sizeof(char),5);}));goto _LLCD;_LLD6: if(_tmp11A <= (void*)4)goto _LLE0;if(*((int*)
_tmp11A)!= 0)goto _LLD8;_tmp11B=((struct Cyc_Absyn_Evar_struct*)_tmp11A)->f1;
_tmp11C=((struct Cyc_Absyn_Evar_struct*)_tmp11A)->f2;_tmp11D=((struct Cyc_Absyn_Evar_struct*)
_tmp11A)->f3;_tmp11E=((struct Cyc_Absyn_Evar_struct*)_tmp11A)->f4;_LLD7: if(
_tmp11C != 0)return Cyc_Absynpp_ntyp2doc((void*)_tmp11C->v);else{s=({struct Cyc_PP_Doc*
_tmp13D[6];_tmp13D[5]=_tmp11B == 0?Cyc_Absynpp_question(): Cyc_Absynpp_kind2doc((
void*)_tmp11B->v);_tmp13D[4]=Cyc_PP_text(({const char*_tmp143=")::";
_tag_dyneither(_tmp143,sizeof(char),4);}));_tmp13D[3]=(!Cyc_Absynpp_print_full_evars
 || _tmp11E == 0)?Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)
_tmp11E->v);_tmp13D[2]=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp142;_tmp142.tag=1;_tmp142.f1=(unsigned long)_tmp11D;{void*_tmp140[1]={&
_tmp142};Cyc_aprintf(({const char*_tmp141="%d";_tag_dyneither(_tmp141,sizeof(char),
3);}),_tag_dyneither(_tmp140,sizeof(void*),1));}}));_tmp13D[1]=Cyc_PP_text(({
const char*_tmp13F="(";_tag_dyneither(_tmp13F,sizeof(char),2);}));_tmp13D[0]=Cyc_PP_text(({
const char*_tmp13E="%";_tag_dyneither(_tmp13E,sizeof(char),2);}));Cyc_PP_cat(
_tag_dyneither(_tmp13D,sizeof(struct Cyc_PP_Doc*),6));});}goto _LLCD;_LLD8: if(*((
int*)_tmp11A)!= 1)goto _LLDA;_tmp11F=((struct Cyc_Absyn_VarType_struct*)_tmp11A)->f1;
_LLD9: s=Cyc_PP_textptr(_tmp11F->name);if(Cyc_Absynpp_print_all_kinds)s=({struct
Cyc_PP_Doc*_tmp144[3];_tmp144[2]=Cyc_Absynpp_kindbound2doc((void*)_tmp11F->kind);
_tmp144[1]=Cyc_PP_text(({const char*_tmp145="::";_tag_dyneither(_tmp145,sizeof(
char),3);}));_tmp144[0]=s;Cyc_PP_cat(_tag_dyneither(_tmp144,sizeof(struct Cyc_PP_Doc*),
3));});if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(_tmp11F))s=({
struct Cyc_PP_Doc*_tmp146[3];_tmp146[2]=Cyc_PP_text(({const char*_tmp148=" */";
_tag_dyneither(_tmp148,sizeof(char),4);}));_tmp146[1]=s;_tmp146[0]=Cyc_PP_text(({
const char*_tmp147="_ /* ";_tag_dyneither(_tmp147,sizeof(char),6);}));Cyc_PP_cat(
_tag_dyneither(_tmp146,sizeof(struct Cyc_PP_Doc*),3));});goto _LLCD;_LLDA: if(*((
int*)_tmp11A)!= 2)goto _LLDC;_tmp120=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp11A)->f1;_tmp121=_tmp120.datatype_info;_tmp122=_tmp120.targs;_tmp123=_tmp120.rgn;
_LLDB:{union Cyc_Absyn_DatatypeInfoU_union _tmp149=_tmp121;struct Cyc_Absyn_UnknownDatatypeInfo
_tmp14A;struct _tuple0*_tmp14B;int _tmp14C;struct Cyc_Absyn_Datatypedecl**_tmp14D;
struct Cyc_Absyn_Datatypedecl*_tmp14E;struct Cyc_Absyn_Datatypedecl _tmp14F;struct
_tuple0*_tmp150;int _tmp151;_LL103: if((_tmp149.UnknownDatatype).tag != 0)goto
_LL105;_tmp14A=(_tmp149.UnknownDatatype).f1;_tmp14B=_tmp14A.name;_tmp14C=_tmp14A.is_extensible;
_LL104: _tmp150=_tmp14B;_tmp151=_tmp14C;goto _LL106;_LL105: if((_tmp149.KnownDatatype).tag
!= 1)goto _LL102;_tmp14D=(_tmp149.KnownDatatype).f1;_tmp14E=*_tmp14D;_tmp14F=*
_tmp14E;_tmp150=_tmp14F.name;_tmp151=_tmp14F.is_extensible;_LL106: {struct Cyc_PP_Doc*
_tmp152=Cyc_PP_text(({const char*_tmp159="datatype ";_tag_dyneither(_tmp159,
sizeof(char),10);}));struct Cyc_PP_Doc*_tmp153=_tmp151?Cyc_PP_text(({const char*
_tmp158="@extensible ";_tag_dyneither(_tmp158,sizeof(char),13);})): Cyc_PP_nil_doc();
void*r=(unsigned int)_tmp123?(void*)_tmp123->v:(void*)2;{void*_tmp154=Cyc_Tcutil_compress(
r);_LL108: if((int)_tmp154 != 2)goto _LL10A;_LL109: s=({struct Cyc_PP_Doc*_tmp155[4];
_tmp155[3]=Cyc_Absynpp_tps2doc(_tmp122);_tmp155[2]=Cyc_Absynpp_qvar2doc(_tmp150);
_tmp155[1]=_tmp152;_tmp155[0]=_tmp153;Cyc_PP_cat(_tag_dyneither(_tmp155,sizeof(
struct Cyc_PP_Doc*),4));});goto _LL107;_LL10A:;_LL10B: s=({struct Cyc_PP_Doc*_tmp156[
6];_tmp156[5]=Cyc_Absynpp_tps2doc(_tmp122);_tmp156[4]=Cyc_Absynpp_qvar2doc(
_tmp150);_tmp156[3]=Cyc_PP_text(({const char*_tmp157=" ";_tag_dyneither(_tmp157,
sizeof(char),2);}));_tmp156[2]=Cyc_Absynpp_typ2doc(r);_tmp156[1]=_tmp152;_tmp156[
0]=_tmp153;Cyc_PP_cat(_tag_dyneither(_tmp156,sizeof(struct Cyc_PP_Doc*),6));});
goto _LL107;_LL107:;}goto _LL102;}_LL102:;}goto _LLCD;_LLDC: if(*((int*)_tmp11A)!= 3)
goto _LLDE;_tmp124=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp11A)->f1;
_tmp125=_tmp124.field_info;_tmp126=_tmp124.targs;_LLDD:{union Cyc_Absyn_DatatypeFieldInfoU_union
_tmp15A=_tmp125;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp15B;struct _tuple0*
_tmp15C;struct _tuple0*_tmp15D;int _tmp15E;struct Cyc_Absyn_Datatypedecl*_tmp15F;
struct Cyc_Absyn_Datatypedecl _tmp160;struct _tuple0*_tmp161;int _tmp162;struct Cyc_Absyn_Datatypefield*
_tmp163;struct Cyc_Absyn_Datatypefield _tmp164;struct _tuple0*_tmp165;_LL10D: if((
_tmp15A.UnknownDatatypefield).tag != 0)goto _LL10F;_tmp15B=(_tmp15A.UnknownDatatypefield).f1;
_tmp15C=_tmp15B.datatype_name;_tmp15D=_tmp15B.field_name;_tmp15E=_tmp15B.is_extensible;
_LL10E: _tmp161=_tmp15C;_tmp162=_tmp15E;_tmp165=_tmp15D;goto _LL110;_LL10F: if((
_tmp15A.KnownDatatypefield).tag != 1)goto _LL10C;_tmp15F=(_tmp15A.KnownDatatypefield).f1;
_tmp160=*_tmp15F;_tmp161=_tmp160.name;_tmp162=_tmp160.is_extensible;_tmp163=(
_tmp15A.KnownDatatypefield).f2;_tmp164=*_tmp163;_tmp165=_tmp164.name;_LL110: {
struct Cyc_PP_Doc*_tmp166=Cyc_PP_text(_tmp162?({const char*_tmp169="@extensible datatype ";
_tag_dyneither(_tmp169,sizeof(char),22);}):({const char*_tmp16A="datatype ";
_tag_dyneither(_tmp16A,sizeof(char),10);}));s=({struct Cyc_PP_Doc*_tmp167[4];
_tmp167[3]=Cyc_Absynpp_qvar2doc(_tmp165);_tmp167[2]=Cyc_PP_text(({const char*
_tmp168=".";_tag_dyneither(_tmp168,sizeof(char),2);}));_tmp167[1]=Cyc_Absynpp_qvar2doc(
_tmp161);_tmp167[0]=_tmp166;Cyc_PP_cat(_tag_dyneither(_tmp167,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL10C;}_LL10C:;}goto _LLCD;_LLDE: if(*((int*)_tmp11A)!= 5)goto _LLE0;
_tmp127=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp11A)->f1;_tmp128=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp11A)->f2;_LLDF: {struct _dyneither_ptr sns;
struct _dyneither_ptr ts;{void*_tmp16B=_tmp127;_LL112: if((int)_tmp16B != 2)goto
_LL114;_LL113: goto _LL115;_LL114: if((int)_tmp16B != 0)goto _LL116;_LL115: sns=({
const char*_tmp16C="";_tag_dyneither(_tmp16C,sizeof(char),1);});goto _LL111;_LL116:
if((int)_tmp16B != 1)goto _LL111;_LL117: sns=({const char*_tmp16D="unsigned ";
_tag_dyneither(_tmp16D,sizeof(char),10);});goto _LL111;_LL111:;}{void*_tmp16E=
_tmp128;_LL119: if((int)_tmp16E != 0)goto _LL11B;_LL11A:{void*_tmp16F=_tmp127;
_LL124: if((int)_tmp16F != 2)goto _LL126;_LL125: sns=({const char*_tmp170="";
_tag_dyneither(_tmp170,sizeof(char),1);});goto _LL123;_LL126: if((int)_tmp16F != 0)
goto _LL128;_LL127: sns=({const char*_tmp171="signed ";_tag_dyneither(_tmp171,
sizeof(char),8);});goto _LL123;_LL128: if((int)_tmp16F != 1)goto _LL123;_LL129: sns=({
const char*_tmp172="unsigned ";_tag_dyneither(_tmp172,sizeof(char),10);});goto
_LL123;_LL123:;}ts=({const char*_tmp173="char";_tag_dyneither(_tmp173,sizeof(char),
5);});goto _LL118;_LL11B: if((int)_tmp16E != 1)goto _LL11D;_LL11C: ts=({const char*
_tmp174="short";_tag_dyneither(_tmp174,sizeof(char),6);});goto _LL118;_LL11D: if((
int)_tmp16E != 2)goto _LL11F;_LL11E: ts=({const char*_tmp175="int";_tag_dyneither(
_tmp175,sizeof(char),4);});goto _LL118;_LL11F: if((int)_tmp16E != 3)goto _LL121;
_LL120: ts=({const char*_tmp176="long";_tag_dyneither(_tmp176,sizeof(char),5);});
goto _LL118;_LL121: if((int)_tmp16E != 4)goto _LL118;_LL122:{void*_tmp177=Cyc_Cyclone_c_compiler;
_LL12B: if((int)_tmp177 != 0)goto _LL12D;_LL12C: ts=({const char*_tmp178="long long";
_tag_dyneither(_tmp178,sizeof(char),10);});goto _LL12A;_LL12D: if((int)_tmp177 != 1)
goto _LL12A;_LL12E: ts=({const char*_tmp179="__int64";_tag_dyneither(_tmp179,
sizeof(char),8);});goto _LL12A;_LL12A:;}goto _LL118;_LL118:;}s=Cyc_PP_text((struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp17D;_tmp17D.tag=0;_tmp17D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)ts);{struct Cyc_String_pa_struct
_tmp17C;_tmp17C.tag=0;_tmp17C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
sns);{void*_tmp17A[2]={& _tmp17C,& _tmp17D};Cyc_aprintf(({const char*_tmp17B="%s%s";
_tag_dyneither(_tmp17B,sizeof(char),5);}),_tag_dyneither(_tmp17A,sizeof(void*),2));}}}));
goto _LLCD;}_LLE0: if((int)_tmp11A != 1)goto _LLE2;_LLE1: s=Cyc_PP_text(({const char*
_tmp17E="float";_tag_dyneither(_tmp17E,sizeof(char),6);}));goto _LLCD;_LLE2: if(
_tmp11A <= (void*)4)goto _LLF8;if(*((int*)_tmp11A)!= 6)goto _LLE4;_tmp129=((struct
Cyc_Absyn_DoubleType_struct*)_tmp11A)->f1;_LLE3: if(_tmp129)s=Cyc_PP_text(({const
char*_tmp17F="long double";_tag_dyneither(_tmp17F,sizeof(char),12);}));else{s=
Cyc_PP_text(({const char*_tmp180="double";_tag_dyneither(_tmp180,sizeof(char),7);}));}
goto _LLCD;_LLE4: if(*((int*)_tmp11A)!= 9)goto _LLE6;_tmp12A=((struct Cyc_Absyn_TupleType_struct*)
_tmp11A)->f1;_LLE5: s=({struct Cyc_PP_Doc*_tmp181[2];_tmp181[1]=Cyc_Absynpp_args2doc(
_tmp12A);_tmp181[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(_tag_dyneither(_tmp181,
sizeof(struct Cyc_PP_Doc*),2));});goto _LLCD;_LLE6: if(*((int*)_tmp11A)!= 10)goto
_LLE8;_tmp12B=((struct Cyc_Absyn_AggrType_struct*)_tmp11A)->f1;_tmp12C=_tmp12B.aggr_info;
_tmp12D=_tmp12B.targs;_LLE7: {void*_tmp183;struct _tuple0*_tmp184;struct _tuple3
_tmp182=Cyc_Absyn_aggr_kinded_name(_tmp12C);_tmp183=_tmp182.f1;_tmp184=_tmp182.f2;
s=({struct Cyc_PP_Doc*_tmp185[3];_tmp185[2]=Cyc_Absynpp_tps2doc(_tmp12D);_tmp185[
1]=Cyc_Absynpp_qvar2doc(_tmp184);_tmp185[0]=Cyc_Absynpp_aggr_kind2doc(_tmp183);
Cyc_PP_cat(_tag_dyneither(_tmp185,sizeof(struct Cyc_PP_Doc*),3));});goto _LLCD;}
_LLE8: if(*((int*)_tmp11A)!= 11)goto _LLEA;_tmp12E=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp11A)->f1;_tmp12F=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp11A)->f2;_LLE9: s=({
struct Cyc_PP_Doc*_tmp186[4];_tmp186[3]=Cyc_Absynpp_rb();_tmp186[2]=Cyc_PP_nest(2,
Cyc_Absynpp_aggrfields2doc(_tmp12F));_tmp186[1]=Cyc_Absynpp_lb();_tmp186[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp12E);Cyc_PP_cat(_tag_dyneither(_tmp186,sizeof(struct Cyc_PP_Doc*),4));});goto
_LLCD;_LLEA: if(*((int*)_tmp11A)!= 13)goto _LLEC;_tmp130=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp11A)->f1;_LLEB: s=({struct Cyc_PP_Doc*_tmp187[4];_tmp187[3]=Cyc_Absynpp_rb();
_tmp187[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp130));_tmp187[1]=Cyc_Absynpp_lb();
_tmp187[0]=Cyc_PP_text(({const char*_tmp188="enum ";_tag_dyneither(_tmp188,
sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp187,sizeof(struct Cyc_PP_Doc*),
4));});goto _LLCD;_LLEC: if(*((int*)_tmp11A)!= 12)goto _LLEE;_tmp131=((struct Cyc_Absyn_EnumType_struct*)
_tmp11A)->f1;_LLED: s=({struct Cyc_PP_Doc*_tmp189[2];_tmp189[1]=Cyc_Absynpp_qvar2doc(
_tmp131);_tmp189[0]=Cyc_PP_text(({const char*_tmp18A="enum ";_tag_dyneither(
_tmp18A,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp189,sizeof(struct Cyc_PP_Doc*),
2));});goto _LLCD;_LLEE: if(*((int*)_tmp11A)!= 17)goto _LLF0;_tmp132=((struct Cyc_Absyn_ValueofType_struct*)
_tmp11A)->f1;_LLEF: s=({struct Cyc_PP_Doc*_tmp18B[3];_tmp18B[2]=Cyc_PP_text(({
const char*_tmp18D=")";_tag_dyneither(_tmp18D,sizeof(char),2);}));_tmp18B[1]=Cyc_Absynpp_exp2doc(
_tmp132);_tmp18B[0]=Cyc_PP_text(({const char*_tmp18C="valueof_t(";_tag_dyneither(
_tmp18C,sizeof(char),11);}));Cyc_PP_cat(_tag_dyneither(_tmp18B,sizeof(struct Cyc_PP_Doc*),
3));});goto _LLCD;_LLF0: if(*((int*)_tmp11A)!= 16)goto _LLF2;_tmp133=((struct Cyc_Absyn_TypedefType_struct*)
_tmp11A)->f1;_tmp134=((struct Cyc_Absyn_TypedefType_struct*)_tmp11A)->f2;_tmp135=((
struct Cyc_Absyn_TypedefType_struct*)_tmp11A)->f3;_LLF1: s=({struct Cyc_PP_Doc*
_tmp18E[2];_tmp18E[1]=Cyc_Absynpp_tps2doc(_tmp134);_tmp18E[0]=Cyc_Absynpp_qvar2doc(
_tmp133);Cyc_PP_cat(_tag_dyneither(_tmp18E,sizeof(struct Cyc_PP_Doc*),2));});goto
_LLCD;_LLF2: if(*((int*)_tmp11A)!= 14)goto _LLF4;_tmp136=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp11A)->f1;_LLF3: s=({struct Cyc_PP_Doc*_tmp18F[3];_tmp18F[2]=Cyc_PP_text(({
const char*_tmp191=">";_tag_dyneither(_tmp191,sizeof(char),2);}));_tmp18F[1]=Cyc_Absynpp_rgn2doc(
_tmp136);_tmp18F[0]=Cyc_PP_text(({const char*_tmp190="region_t<";_tag_dyneither(
_tmp190,sizeof(char),10);}));Cyc_PP_cat(_tag_dyneither(_tmp18F,sizeof(struct Cyc_PP_Doc*),
3));});goto _LLCD;_LLF4: if(*((int*)_tmp11A)!= 15)goto _LLF6;_tmp137=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp11A)->f1;_tmp138=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp11A)->f2;_LLF5: s=({struct Cyc_PP_Doc*_tmp192[5];_tmp192[4]=Cyc_PP_text(({
const char*_tmp195=">";_tag_dyneither(_tmp195,sizeof(char),2);}));_tmp192[3]=Cyc_Absynpp_rgn2doc(
_tmp138);_tmp192[2]=Cyc_PP_text(({const char*_tmp194=",";_tag_dyneither(_tmp194,
sizeof(char),2);}));_tmp192[1]=Cyc_Absynpp_rgn2doc(_tmp137);_tmp192[0]=Cyc_PP_text(({
const char*_tmp193="dynregion_t<";_tag_dyneither(_tmp193,sizeof(char),13);}));Cyc_PP_cat(
_tag_dyneither(_tmp192,sizeof(struct Cyc_PP_Doc*),5));});goto _LLCD;_LLF6: if(*((
int*)_tmp11A)!= 18)goto _LLF8;_tmp139=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp11A)->f1;_LLF7: s=({struct Cyc_PP_Doc*_tmp196[3];_tmp196[2]=Cyc_PP_text(({
const char*_tmp198=">";_tag_dyneither(_tmp198,sizeof(char),2);}));_tmp196[1]=Cyc_Absynpp_typ2doc(
_tmp139);_tmp196[0]=Cyc_PP_text(({const char*_tmp197="tag_t<";_tag_dyneither(
_tmp197,sizeof(char),7);}));Cyc_PP_cat(_tag_dyneither(_tmp196,sizeof(struct Cyc_PP_Doc*),
3));});goto _LLCD;_LLF8: if((int)_tmp11A != 3)goto _LLFA;_LLF9: goto _LLFB;_LLFA: if((
int)_tmp11A != 2)goto _LLFC;_LLFB: s=Cyc_Absynpp_rgn2doc(t);goto _LLCD;_LLFC: if(
_tmp11A <= (void*)4)goto _LLFE;if(*((int*)_tmp11A)!= 21)goto _LLFE;_tmp13A=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp11A)->f1;_LLFD: s=({struct Cyc_PP_Doc*_tmp199[
3];_tmp199[2]=Cyc_PP_text(({const char*_tmp19B=")";_tag_dyneither(_tmp19B,sizeof(
char),2);}));_tmp199[1]=Cyc_Absynpp_typ2doc(_tmp13A);_tmp199[0]=Cyc_PP_text(({
const char*_tmp19A="regions(";_tag_dyneither(_tmp19A,sizeof(char),9);}));Cyc_PP_cat(
_tag_dyneither(_tmp199,sizeof(struct Cyc_PP_Doc*),3));});goto _LLCD;_LLFE: if(
_tmp11A <= (void*)4)goto _LL100;if(*((int*)_tmp11A)!= 19)goto _LL100;_LLFF: goto
_LL101;_LL100: if(_tmp11A <= (void*)4)goto _LLCD;if(*((int*)_tmp11A)!= 20)goto _LLCD;
_LL101: s=Cyc_Absynpp_eff2doc(t);goto _LLCD;_LLCD:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct
_dyneither_ptr*)vo->v));}struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple5*
cmp){struct _tuple5 _tmp19D;void*_tmp19E;void*_tmp19F;struct _tuple5*_tmp19C=cmp;
_tmp19D=*_tmp19C;_tmp19E=_tmp19D.f1;_tmp19F=_tmp19D.f2;return({struct Cyc_PP_Doc*
_tmp1A0[3];_tmp1A0[2]=Cyc_Absynpp_rgn2doc(_tmp19F);_tmp1A0[1]=Cyc_PP_text(({
const char*_tmp1A1=" > ";_tag_dyneither(_tmp1A1,sizeof(char),4);}));_tmp1A0[0]=
Cyc_Absynpp_rgn2doc(_tmp19E);Cyc_PP_cat(_tag_dyneither(_tmp1A0,sizeof(struct Cyc_PP_Doc*),
3));});}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){return Cyc_PP_group(({
const char*_tmp1A2="";_tag_dyneither(_tmp1A2,sizeof(char),1);}),({const char*
_tmp1A3="";_tag_dyneither(_tmp1A3,sizeof(char),1);}),({const char*_tmp1A4=",";
_tag_dyneither(_tmp1A4,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,
po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple1*t){struct Cyc_Core_Opt*
dopt=(*t).f1 == 0?0:({struct Cyc_Core_Opt*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));
_tmp1A5->v=Cyc_PP_text(*((struct _dyneither_ptr*)((struct Cyc_Core_Opt*)
_check_null((*t).f1))->v));_tmp1A5;});return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,
dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp1A6=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs)_tmp1A6=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1A6,({struct
Cyc_List_List*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7->hd=Cyc_PP_text(({
const char*_tmp1A8="...";_tag_dyneither(_tmp1A8,sizeof(char),4);}));_tmp1A7->tl=0;
_tmp1A7;}));else{if(cyc_varargs != 0){struct Cyc_PP_Doc*_tmp1A9=({struct Cyc_PP_Doc*
_tmp1AB[3];_tmp1AB[2]=Cyc_Absynpp_funarg2doc(({struct _tuple1*_tmp1AF=_cycalloc(
sizeof(*_tmp1AF));_tmp1AF->f1=cyc_varargs->name;_tmp1AF->f2=cyc_varargs->tq;
_tmp1AF->f3=(void*)cyc_varargs->type;_tmp1AF;}));_tmp1AB[1]=cyc_varargs->inject?
Cyc_PP_text(({const char*_tmp1AD=" inject ";_tag_dyneither(_tmp1AD,sizeof(char),9);})):
Cyc_PP_text(({const char*_tmp1AE=" ";_tag_dyneither(_tmp1AE,sizeof(char),2);}));
_tmp1AB[0]=Cyc_PP_text(({const char*_tmp1AC="...";_tag_dyneither(_tmp1AC,sizeof(
char),4);}));Cyc_PP_cat(_tag_dyneither(_tmp1AB,sizeof(struct Cyc_PP_Doc*),3));});
_tmp1A6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(_tmp1A6,({struct Cyc_List_List*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));
_tmp1AA->hd=_tmp1A9;_tmp1AA->tl=0;_tmp1AA;}));}}{struct Cyc_PP_Doc*_tmp1B0=Cyc_PP_group(({
const char*_tmp1B8="";_tag_dyneither(_tmp1B8,sizeof(char),1);}),({const char*
_tmp1B9="";_tag_dyneither(_tmp1B9,sizeof(char),1);}),({const char*_tmp1BA=",";
_tag_dyneither(_tmp1BA,sizeof(char),2);}),_tmp1A6);if(effopt != 0  && Cyc_Absynpp_print_all_effects)
_tmp1B0=({struct Cyc_PP_Doc*_tmp1B1[3];_tmp1B1[2]=Cyc_Absynpp_eff2doc((void*)
effopt->v);_tmp1B1[1]=Cyc_PP_text(({const char*_tmp1B2=";";_tag_dyneither(_tmp1B2,
sizeof(char),2);}));_tmp1B1[0]=_tmp1B0;Cyc_PP_cat(_tag_dyneither(_tmp1B1,sizeof(
struct Cyc_PP_Doc*),3));});if(rgn_po != 0)_tmp1B0=({struct Cyc_PP_Doc*_tmp1B3[3];
_tmp1B3[2]=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp1B3[1]=Cyc_PP_text(({const char*
_tmp1B4=":";_tag_dyneither(_tmp1B4,sizeof(char),2);}));_tmp1B3[0]=_tmp1B0;Cyc_PP_cat(
_tag_dyneither(_tmp1B3,sizeof(struct Cyc_PP_Doc*),3));});return({struct Cyc_PP_Doc*
_tmp1B5[3];_tmp1B5[2]=Cyc_PP_text(({const char*_tmp1B7=")";_tag_dyneither(_tmp1B7,
sizeof(char),2);}));_tmp1B5[1]=_tmp1B0;_tmp1B5[0]=Cyc_PP_text(({const char*
_tmp1B6="(";_tag_dyneither(_tmp1B6,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(
_tmp1B5,sizeof(struct Cyc_PP_Doc*),3));});}}struct _tuple1*Cyc_Absynpp_arg_mk_opt(
struct _tuple6*arg){return({struct _tuple1*_tmp1BB=_cycalloc(sizeof(*_tmp1BB));
_tmp1BB->f1=({struct Cyc_Core_Opt*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));_tmp1BC->v=(*
arg).f1;_tmp1BC;});_tmp1BB->f2=(*arg).f2;_tmp1BB->f3=(*arg).f3;_tmp1BB;});}
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*
v);}struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){struct Cyc_List_List*
_tmp1BD=0;int match;{union Cyc_Absyn_Nmspace_union _tmp1BE=(*q).f1;struct Cyc_List_List*
_tmp1BF;struct Cyc_List_List*_tmp1C0;_LL130: if((_tmp1BE.Loc_n).tag != 0)goto _LL132;
_LL131: _tmp1BF=0;goto _LL133;_LL132: if((_tmp1BE.Rel_n).tag != 1)goto _LL134;_tmp1BF=(
_tmp1BE.Rel_n).f1;_LL133: match=0;_tmp1BD=_tmp1BF;goto _LL12F;_LL134: if((_tmp1BE.Abs_n).tag
!= 2)goto _LL12F;_tmp1C0=(_tmp1BE.Abs_n).f1;_LL135: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1C0,Cyc_Absynpp_curr_namespace);
_tmp1BD=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*
_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1->hd=Cyc_Absynpp_cyc_stringptr;
_tmp1C1->tl=_tmp1C0;_tmp1C1;}): _tmp1C0;goto _LL12F;_LL12F:;}if(Cyc_Absynpp_qvar_to_Cids)
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1BD,({struct Cyc_List_List*_tmp1C2=
_cycalloc(sizeof(*_tmp1C2));_tmp1C2->hd=(*q).f2;_tmp1C2->tl=0;_tmp1C2;})),({
const char*_tmp1C3="_";_tag_dyneither(_tmp1C3,sizeof(char),2);}));else{if(match)
return*(*q).f2;else{return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1BD,({struct
Cyc_List_List*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4->hd=(*q).f2;_tmp1C4->tl=
0;_tmp1C4;})),({const char*_tmp1C5="::";_tag_dyneither(_tmp1C5,sizeof(char),3);}));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){return Cyc_PP_text(Cyc_Absynpp_qvar2string(
q));}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){struct
_dyneither_ptr _tmp1C6=Cyc_Absynpp_qvar2string(q);if(Cyc_PP_tex_output)return Cyc_PP_text_width((
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(({const
char*_tmp1C7="\\textbf{";_tag_dyneither(_tmp1C7,sizeof(char),9);}),(struct
_dyneither_ptr)_tmp1C6),({const char*_tmp1C8="}";_tag_dyneither(_tmp1C8,sizeof(
char),2);})),(int)Cyc_strlen((struct _dyneither_ptr)_tmp1C6));else{return Cyc_PP_text(
_tmp1C6);}}struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace_union _tmp1C9=(*v).f1;struct Cyc_List_List*_tmp1CA;struct
Cyc_List_List*_tmp1CB;_LL137: if((_tmp1C9.Loc_n).tag != 0)goto _LL139;_LL138: goto
_LL13A;_LL139: if((_tmp1C9.Rel_n).tag != 1)goto _LL13B;_tmp1CA=(_tmp1C9.Rel_n).f1;
if(_tmp1CA != 0)goto _LL13B;_LL13A: return*(*v).f2;_LL13B: if((_tmp1C9.Abs_n).tag != 
2)goto _LL13D;_tmp1CB=(_tmp1C9.Abs_n).f1;_LL13C: if(((int(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp1CB,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{goto _LL13E;}_LL13D:;_LL13E: return(struct _dyneither_ptr)Cyc_strconcat(({
const char*_tmp1CC="/* bad namespace : */ ";_tag_dyneither(_tmp1CC,sizeof(char),
23);}),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));_LL136:;}else{return*(*
v).f2;}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){return Cyc_PP_text(
Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v){struct _dyneither_ptr _tmp1CD=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output)return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((
struct _dyneither_ptr)Cyc_strconcat(({const char*_tmp1CE="\\textbf{";
_tag_dyneither(_tmp1CE,sizeof(char),9);}),(struct _dyneither_ptr)_tmp1CD),({const
char*_tmp1CF="}";_tag_dyneither(_tmp1CF,sizeof(char),2);})),(int)Cyc_strlen((
struct _dyneither_ptr)_tmp1CD));else{return Cyc_PP_text(_tmp1CD);}}struct Cyc_PP_Doc*
Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),
t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){void*_tmp1D0=(void*)e->r;
void*_tmp1D1;struct Cyc_Absyn_Exp*_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D3;_LL140: if(*((
int*)_tmp1D0)!= 0)goto _LL142;_LL141: goto _LL143;_LL142: if(*((int*)_tmp1D0)!= 1)
goto _LL144;_LL143: goto _LL145;_LL144: if(*((int*)_tmp1D0)!= 2)goto _LL146;_LL145:
return 10000;_LL146: if(*((int*)_tmp1D0)!= 3)goto _LL148;_tmp1D1=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D0)->f1;_LL147: {void*_tmp1D4=_tmp1D1;_LL191: if((int)_tmp1D4 != 0)goto _LL193;
_LL192: return 100;_LL193: if((int)_tmp1D4 != 1)goto _LL195;_LL194: return 110;_LL195:
if((int)_tmp1D4 != 2)goto _LL197;_LL196: return 100;_LL197: if((int)_tmp1D4 != 3)goto
_LL199;_LL198: goto _LL19A;_LL199: if((int)_tmp1D4 != 4)goto _LL19B;_LL19A: return 110;
_LL19B: if((int)_tmp1D4 != 5)goto _LL19D;_LL19C: goto _LL19E;_LL19D: if((int)_tmp1D4 != 
6)goto _LL19F;_LL19E: return 70;_LL19F: if((int)_tmp1D4 != 7)goto _LL1A1;_LL1A0: goto
_LL1A2;_LL1A1: if((int)_tmp1D4 != 8)goto _LL1A3;_LL1A2: goto _LL1A4;_LL1A3: if((int)
_tmp1D4 != 9)goto _LL1A5;_LL1A4: goto _LL1A6;_LL1A5: if((int)_tmp1D4 != 10)goto _LL1A7;
_LL1A6: return 80;_LL1A7: if((int)_tmp1D4 != 11)goto _LL1A9;_LL1A8: goto _LL1AA;_LL1A9:
if((int)_tmp1D4 != 12)goto _LL1AB;_LL1AA: return 130;_LL1AB: if((int)_tmp1D4 != 13)
goto _LL1AD;_LL1AC: return 60;_LL1AD: if((int)_tmp1D4 != 14)goto _LL1AF;_LL1AE: return
40;_LL1AF: if((int)_tmp1D4 != 15)goto _LL1B1;_LL1B0: return 50;_LL1B1: if((int)_tmp1D4
!= 16)goto _LL1B3;_LL1B2: return 90;_LL1B3: if((int)_tmp1D4 != 17)goto _LL1B5;_LL1B4:
return 80;_LL1B5: if((int)_tmp1D4 != 18)goto _LL1B7;_LL1B6: return 80;_LL1B7: if((int)
_tmp1D4 != 19)goto _LL190;_LL1B8: return 140;_LL190:;}_LL148: if(*((int*)_tmp1D0)!= 4)
goto _LL14A;_LL149: return 20;_LL14A: if(*((int*)_tmp1D0)!= 5)goto _LL14C;_LL14B:
return 130;_LL14C: if(*((int*)_tmp1D0)!= 6)goto _LL14E;_LL14D: return 30;_LL14E: if(*((
int*)_tmp1D0)!= 7)goto _LL150;_LL14F: return 35;_LL150: if(*((int*)_tmp1D0)!= 8)goto
_LL152;_LL151: return 30;_LL152: if(*((int*)_tmp1D0)!= 9)goto _LL154;_LL153: return 10;
_LL154: if(*((int*)_tmp1D0)!= 10)goto _LL156;_LL155: goto _LL157;_LL156: if(*((int*)
_tmp1D0)!= 11)goto _LL158;_LL157: return 140;_LL158: if(*((int*)_tmp1D0)!= 12)goto
_LL15A;_LL159: return 130;_LL15A: if(*((int*)_tmp1D0)!= 13)goto _LL15C;_tmp1D2=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D0)->f1;_LL15B: return Cyc_Absynpp_exp_prec(
_tmp1D2);_LL15C: if(*((int*)_tmp1D0)!= 14)goto _LL15E;_tmp1D3=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1D0)->f1;_LL15D: return Cyc_Absynpp_exp_prec(_tmp1D3);_LL15E: if(*((int*)
_tmp1D0)!= 15)goto _LL160;_LL15F: return 120;_LL160: if(*((int*)_tmp1D0)!= 17)goto
_LL162;_LL161: return 15;_LL162: if(*((int*)_tmp1D0)!= 16)goto _LL164;_LL163: goto
_LL165;_LL164: if(*((int*)_tmp1D0)!= 18)goto _LL166;_LL165: goto _LL167;_LL166: if(*((
int*)_tmp1D0)!= 19)goto _LL168;_LL167: goto _LL169;_LL168: if(*((int*)_tmp1D0)!= 39)
goto _LL16A;_LL169: goto _LL16B;_LL16A: if(*((int*)_tmp1D0)!= 20)goto _LL16C;_LL16B:
goto _LL16D;_LL16C: if(*((int*)_tmp1D0)!= 21)goto _LL16E;_LL16D: goto _LL16F;_LL16E:
if(*((int*)_tmp1D0)!= 22)goto _LL170;_LL16F: return 130;_LL170: if(*((int*)_tmp1D0)
!= 23)goto _LL172;_LL171: goto _LL173;_LL172: if(*((int*)_tmp1D0)!= 24)goto _LL174;
_LL173: goto _LL175;_LL174: if(*((int*)_tmp1D0)!= 25)goto _LL176;_LL175: return 140;
_LL176: if(*((int*)_tmp1D0)!= 26)goto _LL178;_LL177: return 150;_LL178: if(*((int*)
_tmp1D0)!= 27)goto _LL17A;_LL179: goto _LL17B;_LL17A: if(*((int*)_tmp1D0)!= 28)goto
_LL17C;_LL17B: goto _LL17D;_LL17C: if(*((int*)_tmp1D0)!= 29)goto _LL17E;_LL17D: goto
_LL17F;_LL17E: if(*((int*)_tmp1D0)!= 30)goto _LL180;_LL17F: goto _LL181;_LL180: if(*((
int*)_tmp1D0)!= 31)goto _LL182;_LL181: goto _LL183;_LL182: if(*((int*)_tmp1D0)!= 32)
goto _LL184;_LL183: goto _LL185;_LL184: if(*((int*)_tmp1D0)!= 33)goto _LL186;_LL185:
goto _LL187;_LL186: if(*((int*)_tmp1D0)!= 34)goto _LL188;_LL187: goto _LL189;_LL188:
if(*((int*)_tmp1D0)!= 35)goto _LL18A;_LL189: goto _LL18B;_LL18A: if(*((int*)_tmp1D0)
!= 36)goto _LL18C;_LL18B: goto _LL18D;_LL18C: if(*((int*)_tmp1D0)!= 37)goto _LL18E;
_LL18D: return 140;_LL18E: if(*((int*)_tmp1D0)!= 38)goto _LL13F;_LL18F: return 10000;
_LL13F:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(
0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*s;{void*_tmp1D5=(void*)e->r;
union Cyc_Absyn_Cnst_union _tmp1D6;struct _tuple0*_tmp1D7;struct _tuple0*_tmp1D8;
void*_tmp1D9;struct Cyc_List_List*_tmp1DA;struct Cyc_Absyn_Exp*_tmp1DB;struct Cyc_Core_Opt*
_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DD;struct Cyc_Absyn_Exp*_tmp1DE;void*_tmp1DF;
struct Cyc_Absyn_Exp*_tmp1E0;struct Cyc_Absyn_Exp*_tmp1E1;struct Cyc_Absyn_Exp*
_tmp1E2;struct Cyc_Absyn_Exp*_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E4;struct Cyc_Absyn_Exp*
_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E6;struct Cyc_Absyn_Exp*_tmp1E7;struct Cyc_Absyn_Exp*
_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_List_List*_tmp1EA;struct Cyc_Absyn_Exp*
_tmp1EB;struct Cyc_List_List*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1ED;struct Cyc_Absyn_Exp*
_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;void*_tmp1F0;struct Cyc_Absyn_Exp*_tmp1F1;
struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_Absyn_Exp*_tmp1F3;struct Cyc_Absyn_Exp*
_tmp1F4;void*_tmp1F5;struct Cyc_Absyn_Exp*_tmp1F6;void*_tmp1F7;void*_tmp1F8;void*
_tmp1F9;struct _dyneither_ptr*_tmp1FA;void*_tmp1FB;void*_tmp1FC;unsigned int
_tmp1FD;struct Cyc_List_List*_tmp1FE;void*_tmp1FF;struct Cyc_Absyn_Exp*_tmp200;
struct Cyc_Absyn_Exp*_tmp201;struct _dyneither_ptr*_tmp202;struct Cyc_Absyn_Exp*
_tmp203;struct _dyneither_ptr*_tmp204;struct Cyc_Absyn_Exp*_tmp205;struct Cyc_Absyn_Exp*
_tmp206;struct Cyc_List_List*_tmp207;struct _tuple1*_tmp208;struct Cyc_List_List*
_tmp209;struct Cyc_List_List*_tmp20A;struct Cyc_Absyn_Vardecl*_tmp20B;struct Cyc_Absyn_Exp*
_tmp20C;struct Cyc_Absyn_Exp*_tmp20D;struct _tuple0*_tmp20E;struct Cyc_List_List*
_tmp20F;struct Cyc_List_List*_tmp210;struct Cyc_List_List*_tmp211;struct Cyc_List_List*
_tmp212;struct Cyc_Absyn_Datatypefield*_tmp213;struct _tuple0*_tmp214;struct
_tuple0*_tmp215;struct Cyc_Absyn_MallocInfo _tmp216;int _tmp217;struct Cyc_Absyn_Exp*
_tmp218;void**_tmp219;struct Cyc_Absyn_Exp*_tmp21A;struct Cyc_Absyn_Exp*_tmp21B;
struct Cyc_Absyn_Exp*_tmp21C;struct Cyc_Core_Opt*_tmp21D;struct Cyc_List_List*
_tmp21E;struct Cyc_Absyn_Stmt*_tmp21F;_LL1BA: if(*((int*)_tmp1D5)!= 0)goto _LL1BC;
_tmp1D6=((struct Cyc_Absyn_Const_e_struct*)_tmp1D5)->f1;_LL1BB: s=Cyc_Absynpp_cnst2doc(
_tmp1D6);goto _LL1B9;_LL1BC: if(*((int*)_tmp1D5)!= 1)goto _LL1BE;_tmp1D7=((struct
Cyc_Absyn_Var_e_struct*)_tmp1D5)->f1;_LL1BD: _tmp1D8=_tmp1D7;goto _LL1BF;_LL1BE:
if(*((int*)_tmp1D5)!= 2)goto _LL1C0;_tmp1D8=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp1D5)->f1;_LL1BF: s=Cyc_Absynpp_qvar2doc(_tmp1D8);goto _LL1B9;_LL1C0: if(*((int*)
_tmp1D5)!= 3)goto _LL1C2;_tmp1D9=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D5)->f1;_tmp1DA=((struct Cyc_Absyn_Primop_e_struct*)_tmp1D5)->f2;_LL1C1: s=
Cyc_Absynpp_primapp2doc(myprec,_tmp1D9,_tmp1DA);goto _LL1B9;_LL1C2: if(*((int*)
_tmp1D5)!= 4)goto _LL1C4;_tmp1DB=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1D5)->f1;
_tmp1DC=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1D5)->f2;_tmp1DD=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1D5)->f3;_LL1C3: s=({struct Cyc_PP_Doc*_tmp220[5];_tmp220[4]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1DD);_tmp220[3]=Cyc_PP_text(({const char*_tmp222="= ";_tag_dyneither(
_tmp222,sizeof(char),3);}));_tmp220[2]=_tmp1DC == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((
void*)_tmp1DC->v);_tmp220[1]=Cyc_PP_text(({const char*_tmp221=" ";_tag_dyneither(
_tmp221,sizeof(char),2);}));_tmp220[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DB);
Cyc_PP_cat(_tag_dyneither(_tmp220,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1B9;
_LL1C4: if(*((int*)_tmp1D5)!= 5)goto _LL1C6;_tmp1DE=((struct Cyc_Absyn_Increment_e_struct*)
_tmp1D5)->f1;_tmp1DF=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp1D5)->f2;
_LL1C5: {struct Cyc_PP_Doc*_tmp223=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DE);{void*
_tmp224=_tmp1DF;_LL20D: if((int)_tmp224 != 0)goto _LL20F;_LL20E: s=({struct Cyc_PP_Doc*
_tmp225[2];_tmp225[1]=_tmp223;_tmp225[0]=Cyc_PP_text(({const char*_tmp226="++";
_tag_dyneither(_tmp226,sizeof(char),3);}));Cyc_PP_cat(_tag_dyneither(_tmp225,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL20C;_LL20F: if((int)_tmp224 != 2)goto
_LL211;_LL210: s=({struct Cyc_PP_Doc*_tmp227[2];_tmp227[1]=_tmp223;_tmp227[0]=Cyc_PP_text(({
const char*_tmp228="--";_tag_dyneither(_tmp228,sizeof(char),3);}));Cyc_PP_cat(
_tag_dyneither(_tmp227,sizeof(struct Cyc_PP_Doc*),2));});goto _LL20C;_LL211: if((
int)_tmp224 != 1)goto _LL213;_LL212: s=({struct Cyc_PP_Doc*_tmp229[2];_tmp229[1]=Cyc_PP_text(({
const char*_tmp22A="++";_tag_dyneither(_tmp22A,sizeof(char),3);}));_tmp229[0]=
_tmp223;Cyc_PP_cat(_tag_dyneither(_tmp229,sizeof(struct Cyc_PP_Doc*),2));});goto
_LL20C;_LL213: if((int)_tmp224 != 3)goto _LL20C;_LL214: s=({struct Cyc_PP_Doc*_tmp22B[
2];_tmp22B[1]=Cyc_PP_text(({const char*_tmp22C="--";_tag_dyneither(_tmp22C,
sizeof(char),3);}));_tmp22B[0]=_tmp223;Cyc_PP_cat(_tag_dyneither(_tmp22B,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL20C;_LL20C:;}goto _LL1B9;}_LL1C6: if(*((int*)
_tmp1D5)!= 6)goto _LL1C8;_tmp1E0=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1D5)->f1;
_tmp1E1=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1D5)->f2;_tmp1E2=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1D5)->f3;_LL1C7: s=({struct Cyc_PP_Doc*_tmp22D[5];_tmp22D[4]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E2);_tmp22D[3]=Cyc_PP_text(({const char*_tmp22F=" : ";_tag_dyneither(
_tmp22F,sizeof(char),4);}));_tmp22D[2]=Cyc_Absynpp_exp2doc_prec(0,_tmp1E1);
_tmp22D[1]=Cyc_PP_text(({const char*_tmp22E=" ? ";_tag_dyneither(_tmp22E,sizeof(
char),4);}));_tmp22D[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E0);Cyc_PP_cat(
_tag_dyneither(_tmp22D,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1B9;_LL1C8: if(*((
int*)_tmp1D5)!= 7)goto _LL1CA;_tmp1E3=((struct Cyc_Absyn_And_e_struct*)_tmp1D5)->f1;
_tmp1E4=((struct Cyc_Absyn_And_e_struct*)_tmp1D5)->f2;_LL1C9: s=({struct Cyc_PP_Doc*
_tmp230[3];_tmp230[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E4);_tmp230[1]=Cyc_PP_text(({
const char*_tmp231=" && ";_tag_dyneither(_tmp231,sizeof(char),5);}));_tmp230[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E3);Cyc_PP_cat(_tag_dyneither(_tmp230,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL1B9;_LL1CA: if(*((int*)_tmp1D5)!= 8)goto
_LL1CC;_tmp1E5=((struct Cyc_Absyn_Or_e_struct*)_tmp1D5)->f1;_tmp1E6=((struct Cyc_Absyn_Or_e_struct*)
_tmp1D5)->f2;_LL1CB: s=({struct Cyc_PP_Doc*_tmp232[3];_tmp232[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E6);_tmp232[1]=Cyc_PP_text(({const char*_tmp233=" || ";_tag_dyneither(
_tmp233,sizeof(char),5);}));_tmp232[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E5);
Cyc_PP_cat(_tag_dyneither(_tmp232,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1B9;
_LL1CC: if(*((int*)_tmp1D5)!= 9)goto _LL1CE;_tmp1E7=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1D5)->f1;_tmp1E8=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1D5)->f2;_LL1CD: s=({
struct Cyc_PP_Doc*_tmp234[5];_tmp234[4]=Cyc_PP_text(({const char*_tmp237=")";
_tag_dyneither(_tmp237,sizeof(char),2);}));_tmp234[3]=Cyc_Absynpp_exp2doc(
_tmp1E8);_tmp234[2]=Cyc_PP_text(({const char*_tmp236=", ";_tag_dyneither(_tmp236,
sizeof(char),3);}));_tmp234[1]=Cyc_Absynpp_exp2doc(_tmp1E7);_tmp234[0]=Cyc_PP_text(({
const char*_tmp235="(";_tag_dyneither(_tmp235,sizeof(char),2);}));Cyc_PP_cat(
_tag_dyneither(_tmp234,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1B9;_LL1CE: if(*((
int*)_tmp1D5)!= 10)goto _LL1D0;_tmp1E9=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp1D5)->f1;_tmp1EA=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1D5)->f2;_LL1CF:
s=({struct Cyc_PP_Doc*_tmp238[4];_tmp238[3]=Cyc_PP_text(({const char*_tmp23A=")";
_tag_dyneither(_tmp23A,sizeof(char),2);}));_tmp238[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp1EA);_tmp238[1]=Cyc_PP_text(({const char*_tmp239="(";_tag_dyneither(
_tmp239,sizeof(char),2);}));_tmp238[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E9);
Cyc_PP_cat(_tag_dyneither(_tmp238,sizeof(struct Cyc_PP_Doc*),4));});goto _LL1B9;
_LL1D0: if(*((int*)_tmp1D5)!= 11)goto _LL1D2;_tmp1EB=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1D5)->f1;_tmp1EC=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1D5)->f2;_LL1D1: s=({
struct Cyc_PP_Doc*_tmp23B[4];_tmp23B[3]=Cyc_PP_text(({const char*_tmp23D=")";
_tag_dyneither(_tmp23D,sizeof(char),2);}));_tmp23B[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp1EC);_tmp23B[1]=Cyc_PP_text(({const char*_tmp23C="(";_tag_dyneither(
_tmp23C,sizeof(char),2);}));_tmp23B[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1EB);
Cyc_PP_cat(_tag_dyneither(_tmp23B,sizeof(struct Cyc_PP_Doc*),4));});goto _LL1B9;
_LL1D2: if(*((int*)_tmp1D5)!= 12)goto _LL1D4;_tmp1ED=((struct Cyc_Absyn_Throw_e_struct*)
_tmp1D5)->f1;_LL1D3: s=({struct Cyc_PP_Doc*_tmp23E[2];_tmp23E[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1ED);_tmp23E[0]=Cyc_PP_text(({const char*_tmp23F="throw ";
_tag_dyneither(_tmp23F,sizeof(char),7);}));Cyc_PP_cat(_tag_dyneither(_tmp23E,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL1B9;_LL1D4: if(*((int*)_tmp1D5)!= 13)goto
_LL1D6;_tmp1EE=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D5)->f1;_LL1D5: s=
Cyc_Absynpp_exp2doc_prec(inprec,_tmp1EE);goto _LL1B9;_LL1D6: if(*((int*)_tmp1D5)!= 
14)goto _LL1D8;_tmp1EF=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1D5)->f1;
_LL1D7: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1EF);goto _LL1B9;_LL1D8: if(*((int*)
_tmp1D5)!= 15)goto _LL1DA;_tmp1F0=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1D5)->f1;
_tmp1F1=((struct Cyc_Absyn_Cast_e_struct*)_tmp1D5)->f2;_LL1D9: s=({struct Cyc_PP_Doc*
_tmp240[4];_tmp240[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F1);_tmp240[2]=Cyc_PP_text(({
const char*_tmp242=")";_tag_dyneither(_tmp242,sizeof(char),2);}));_tmp240[1]=Cyc_Absynpp_typ2doc(
_tmp1F0);_tmp240[0]=Cyc_PP_text(({const char*_tmp241="(";_tag_dyneither(_tmp241,
sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp240,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL1B9;_LL1DA: if(*((int*)_tmp1D5)!= 16)goto _LL1DC;_tmp1F2=((struct Cyc_Absyn_Address_e_struct*)
_tmp1D5)->f1;_LL1DB: s=({struct Cyc_PP_Doc*_tmp243[2];_tmp243[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F2);_tmp243[0]=Cyc_PP_text(({const char*_tmp244="&";_tag_dyneither(
_tmp244,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp243,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL1B9;_LL1DC: if(*((int*)_tmp1D5)!= 17)goto _LL1DE;_tmp1F3=((struct Cyc_Absyn_New_e_struct*)
_tmp1D5)->f1;_tmp1F4=((struct Cyc_Absyn_New_e_struct*)_tmp1D5)->f2;_LL1DD: if(
_tmp1F3 == 0)s=({struct Cyc_PP_Doc*_tmp245[2];_tmp245[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F4);_tmp245[0]=Cyc_PP_text(({const char*_tmp246="new ";_tag_dyneither(
_tmp246,sizeof(char),5);}));Cyc_PP_cat(_tag_dyneither(_tmp245,sizeof(struct Cyc_PP_Doc*),
2));});else{s=({struct Cyc_PP_Doc*_tmp247[4];_tmp247[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F4);_tmp247[2]=Cyc_PP_text(({const char*_tmp249=") ";_tag_dyneither(
_tmp249,sizeof(char),3);}));_tmp247[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp1F3);_tmp247[0]=Cyc_PP_text(({const char*_tmp248="rnew(";_tag_dyneither(
_tmp248,sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp247,sizeof(struct Cyc_PP_Doc*),
4));});}goto _LL1B9;_LL1DE: if(*((int*)_tmp1D5)!= 18)goto _LL1E0;_tmp1F5=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp1D5)->f1;_LL1DF: s=({struct Cyc_PP_Doc*
_tmp24A[3];_tmp24A[2]=Cyc_PP_text(({const char*_tmp24C=")";_tag_dyneither(_tmp24C,
sizeof(char),2);}));_tmp24A[1]=Cyc_Absynpp_typ2doc(_tmp1F5);_tmp24A[0]=Cyc_PP_text(({
const char*_tmp24B="sizeof(";_tag_dyneither(_tmp24B,sizeof(char),8);}));Cyc_PP_cat(
_tag_dyneither(_tmp24A,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1B9;_LL1E0: if(*((
int*)_tmp1D5)!= 19)goto _LL1E2;_tmp1F6=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp1D5)->f1;_LL1E1: s=({struct Cyc_PP_Doc*_tmp24D[3];_tmp24D[2]=Cyc_PP_text(({
const char*_tmp24F=")";_tag_dyneither(_tmp24F,sizeof(char),2);}));_tmp24D[1]=Cyc_Absynpp_exp2doc(
_tmp1F6);_tmp24D[0]=Cyc_PP_text(({const char*_tmp24E="sizeof(";_tag_dyneither(
_tmp24E,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp24D,sizeof(struct Cyc_PP_Doc*),
3));});goto _LL1B9;_LL1E2: if(*((int*)_tmp1D5)!= 39)goto _LL1E4;_tmp1F7=(void*)((
struct Cyc_Absyn_Valueof_e_struct*)_tmp1D5)->f1;_LL1E3: s=({struct Cyc_PP_Doc*
_tmp250[3];_tmp250[2]=Cyc_PP_text(({const char*_tmp252=")";_tag_dyneither(_tmp252,
sizeof(char),2);}));_tmp250[1]=Cyc_Absynpp_typ2doc(_tmp1F7);_tmp250[0]=Cyc_PP_text(({
const char*_tmp251="valueof(";_tag_dyneither(_tmp251,sizeof(char),9);}));Cyc_PP_cat(
_tag_dyneither(_tmp250,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1B9;_LL1E4: if(*((
int*)_tmp1D5)!= 20)goto _LL1E6;_tmp1F8=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1D5)->f1;_tmp1F9=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D5)->f2;
if(*((int*)_tmp1F9)!= 0)goto _LL1E6;_tmp1FA=((struct Cyc_Absyn_StructField_struct*)
_tmp1F9)->f1;_LL1E5: s=({struct Cyc_PP_Doc*_tmp253[5];_tmp253[4]=Cyc_PP_text(({
const char*_tmp256=")";_tag_dyneither(_tmp256,sizeof(char),2);}));_tmp253[3]=Cyc_PP_textptr(
_tmp1FA);_tmp253[2]=Cyc_PP_text(({const char*_tmp255=",";_tag_dyneither(_tmp255,
sizeof(char),2);}));_tmp253[1]=Cyc_Absynpp_typ2doc(_tmp1F8);_tmp253[0]=Cyc_PP_text(({
const char*_tmp254="offsetof(";_tag_dyneither(_tmp254,sizeof(char),10);}));Cyc_PP_cat(
_tag_dyneither(_tmp253,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1B9;_LL1E6: if(*((
int*)_tmp1D5)!= 20)goto _LL1E8;_tmp1FB=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1D5)->f1;_tmp1FC=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D5)->f2;
if(*((int*)_tmp1FC)!= 1)goto _LL1E8;_tmp1FD=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp1FC)->f1;_LL1E7: s=({struct Cyc_PP_Doc*_tmp257[5];_tmp257[4]=Cyc_PP_text(({
const char*_tmp25D=")";_tag_dyneither(_tmp25D,sizeof(char),2);}));_tmp257[3]=Cyc_PP_text((
struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp25C;_tmp25C.tag=1;_tmp25C.f1=(
unsigned long)((int)_tmp1FD);{void*_tmp25A[1]={& _tmp25C};Cyc_aprintf(({const char*
_tmp25B="%d";_tag_dyneither(_tmp25B,sizeof(char),3);}),_tag_dyneither(_tmp25A,
sizeof(void*),1));}}));_tmp257[2]=Cyc_PP_text(({const char*_tmp259=",";
_tag_dyneither(_tmp259,sizeof(char),2);}));_tmp257[1]=Cyc_Absynpp_typ2doc(
_tmp1FB);_tmp257[0]=Cyc_PP_text(({const char*_tmp258="offsetof(";_tag_dyneither(
_tmp258,sizeof(char),10);}));Cyc_PP_cat(_tag_dyneither(_tmp257,sizeof(struct Cyc_PP_Doc*),
5));});goto _LL1B9;_LL1E8: if(*((int*)_tmp1D5)!= 21)goto _LL1EA;_tmp1FE=((struct Cyc_Absyn_Gentyp_e_struct*)
_tmp1D5)->f1;_tmp1FF=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp1D5)->f2;
_LL1E9: s=({struct Cyc_PP_Doc*_tmp25E[4];_tmp25E[3]=Cyc_PP_text(({const char*
_tmp260=")";_tag_dyneither(_tmp260,sizeof(char),2);}));_tmp25E[2]=Cyc_Absynpp_typ2doc(
_tmp1FF);_tmp25E[1]=Cyc_Absynpp_tvars2doc(_tmp1FE);_tmp25E[0]=Cyc_PP_text(({
const char*_tmp25F="__gen(";_tag_dyneither(_tmp25F,sizeof(char),7);}));Cyc_PP_cat(
_tag_dyneither(_tmp25E,sizeof(struct Cyc_PP_Doc*),4));});goto _LL1B9;_LL1EA: if(*((
int*)_tmp1D5)!= 22)goto _LL1EC;_tmp200=((struct Cyc_Absyn_Deref_e_struct*)_tmp1D5)->f1;
_LL1EB: s=({struct Cyc_PP_Doc*_tmp261[2];_tmp261[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp200);_tmp261[0]=Cyc_PP_text(({const char*_tmp262="*";_tag_dyneither(
_tmp262,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp261,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL1B9;_LL1EC: if(*((int*)_tmp1D5)!= 23)goto _LL1EE;_tmp201=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp1D5)->f1;_tmp202=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D5)->f2;_LL1ED:
s=({struct Cyc_PP_Doc*_tmp263[3];_tmp263[2]=Cyc_PP_textptr(_tmp202);_tmp263[1]=
Cyc_PP_text(({const char*_tmp264=".";_tag_dyneither(_tmp264,sizeof(char),2);}));
_tmp263[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp201);Cyc_PP_cat(_tag_dyneither(
_tmp263,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1B9;_LL1EE: if(*((int*)_tmp1D5)!= 
24)goto _LL1F0;_tmp203=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1D5)->f1;_tmp204=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1D5)->f2;_LL1EF: s=({struct Cyc_PP_Doc*
_tmp265[3];_tmp265[2]=Cyc_PP_textptr(_tmp204);_tmp265[1]=Cyc_PP_text(({const char*
_tmp266="->";_tag_dyneither(_tmp266,sizeof(char),3);}));_tmp265[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp203);Cyc_PP_cat(_tag_dyneither(_tmp265,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL1B9;_LL1F0: if(*((int*)_tmp1D5)!= 25)goto _LL1F2;_tmp205=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1D5)->f1;_tmp206=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1D5)->f2;_LL1F1: s=({
struct Cyc_PP_Doc*_tmp267[4];_tmp267[3]=Cyc_PP_text(({const char*_tmp269="]";
_tag_dyneither(_tmp269,sizeof(char),2);}));_tmp267[2]=Cyc_Absynpp_exp2doc(
_tmp206);_tmp267[1]=Cyc_PP_text(({const char*_tmp268="[";_tag_dyneither(_tmp268,
sizeof(char),2);}));_tmp267[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp205);Cyc_PP_cat(
_tag_dyneither(_tmp267,sizeof(struct Cyc_PP_Doc*),4));});goto _LL1B9;_LL1F2: if(*((
int*)_tmp1D5)!= 26)goto _LL1F4;_tmp207=((struct Cyc_Absyn_Tuple_e_struct*)_tmp1D5)->f1;
_LL1F3: s=({struct Cyc_PP_Doc*_tmp26A[4];_tmp26A[3]=Cyc_PP_text(({const char*
_tmp26C=")";_tag_dyneither(_tmp26C,sizeof(char),2);}));_tmp26A[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp207);_tmp26A[1]=Cyc_PP_text(({const char*_tmp26B="(";_tag_dyneither(
_tmp26B,sizeof(char),2);}));_tmp26A[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(
_tag_dyneither(_tmp26A,sizeof(struct Cyc_PP_Doc*),4));});goto _LL1B9;_LL1F4: if(*((
int*)_tmp1D5)!= 27)goto _LL1F6;_tmp208=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp1D5)->f1;_tmp209=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp1D5)->f2;_LL1F5:
s=({struct Cyc_PP_Doc*_tmp26D[4];_tmp26D[3]=Cyc_Absynpp_group_braces(({const char*
_tmp270=",";_tag_dyneither(_tmp270,sizeof(char),2);}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp209));_tmp26D[2]=Cyc_PP_text(({const char*_tmp26F=")";_tag_dyneither(_tmp26F,
sizeof(char),2);}));_tmp26D[1]=Cyc_Absynpp_typ2doc((*_tmp208).f3);_tmp26D[0]=Cyc_PP_text(({
const char*_tmp26E="(";_tag_dyneither(_tmp26E,sizeof(char),2);}));Cyc_PP_cat(
_tag_dyneither(_tmp26D,sizeof(struct Cyc_PP_Doc*),4));});goto _LL1B9;_LL1F6: if(*((
int*)_tmp1D5)!= 28)goto _LL1F8;_tmp20A=((struct Cyc_Absyn_Array_e_struct*)_tmp1D5)->f1;
_LL1F7: s=Cyc_Absynpp_group_braces(({const char*_tmp271=",";_tag_dyneither(_tmp271,
sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp20A));goto _LL1B9;
_LL1F8: if(*((int*)_tmp1D5)!= 29)goto _LL1FA;_tmp20B=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1D5)->f1;_tmp20C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D5)->f2;
_tmp20D=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D5)->f3;_LL1F9: s=({struct
Cyc_PP_Doc*_tmp272[8];_tmp272[7]=Cyc_Absynpp_rb();_tmp272[6]=Cyc_Absynpp_exp2doc(
_tmp20D);_tmp272[5]=Cyc_PP_text(({const char*_tmp275=" : ";_tag_dyneither(_tmp275,
sizeof(char),4);}));_tmp272[4]=Cyc_Absynpp_exp2doc(_tmp20C);_tmp272[3]=Cyc_PP_text(({
const char*_tmp274=" < ";_tag_dyneither(_tmp274,sizeof(char),4);}));_tmp272[2]=
Cyc_PP_text(*(*_tmp20B->name).f2);_tmp272[1]=Cyc_PP_text(({const char*_tmp273="for ";
_tag_dyneither(_tmp273,sizeof(char),5);}));_tmp272[0]=Cyc_Absynpp_lb();Cyc_PP_cat(
_tag_dyneither(_tmp272,sizeof(struct Cyc_PP_Doc*),8));});goto _LL1B9;_LL1FA: if(*((
int*)_tmp1D5)!= 30)goto _LL1FC;_tmp20E=((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp1D5)->f1;_tmp20F=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp1D5)->f2;_tmp210=((
struct Cyc_Absyn_Aggregate_e_struct*)_tmp1D5)->f3;_LL1FB: {struct Cyc_List_List*
_tmp276=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp210);s=({struct Cyc_PP_Doc*
_tmp277[2];_tmp277[1]=Cyc_Absynpp_group_braces(({const char*_tmp278=",";
_tag_dyneither(_tmp278,sizeof(char),2);}),_tmp20F != 0?({struct Cyc_List_List*
_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279->hd=Cyc_Absynpp_tps2doc(_tmp20F);
_tmp279->tl=_tmp276;_tmp279;}): _tmp276);_tmp277[0]=Cyc_Absynpp_qvar2doc(_tmp20E);
Cyc_PP_cat(_tag_dyneither(_tmp277,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1B9;}
_LL1FC: if(*((int*)_tmp1D5)!= 31)goto _LL1FE;_tmp211=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1D5)->f2;_LL1FD: s=Cyc_Absynpp_group_braces(({const char*_tmp27A=",";
_tag_dyneither(_tmp27A,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp211));goto _LL1B9;_LL1FE: if(*((int*)_tmp1D5)!= 32)goto _LL200;_tmp212=((struct
Cyc_Absyn_Datatype_e_struct*)_tmp1D5)->f1;_tmp213=((struct Cyc_Absyn_Datatype_e_struct*)
_tmp1D5)->f3;_LL1FF: if(_tmp212 == 0)s=Cyc_Absynpp_qvar2doc(_tmp213->name);else{s=({
struct Cyc_PP_Doc*_tmp27B[2];_tmp27B[1]=Cyc_PP_egroup(({const char*_tmp27C="(";
_tag_dyneither(_tmp27C,sizeof(char),2);}),({const char*_tmp27D=")";_tag_dyneither(
_tmp27D,sizeof(char),2);}),({const char*_tmp27E=",";_tag_dyneither(_tmp27E,
sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp212));_tmp27B[0]=
Cyc_Absynpp_qvar2doc(_tmp213->name);Cyc_PP_cat(_tag_dyneither(_tmp27B,sizeof(
struct Cyc_PP_Doc*),2));});}goto _LL1B9;_LL200: if(*((int*)_tmp1D5)!= 33)goto _LL202;
_tmp214=((struct Cyc_Absyn_Enum_e_struct*)_tmp1D5)->f1;_LL201: s=Cyc_Absynpp_qvar2doc(
_tmp214);goto _LL1B9;_LL202: if(*((int*)_tmp1D5)!= 34)goto _LL204;_tmp215=((struct
Cyc_Absyn_AnonEnum_e_struct*)_tmp1D5)->f1;_LL203: s=Cyc_Absynpp_qvar2doc(_tmp215);
goto _LL1B9;_LL204: if(*((int*)_tmp1D5)!= 35)goto _LL206;_tmp216=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp1D5)->f1;_tmp217=_tmp216.is_calloc;_tmp218=_tmp216.rgn;_tmp219=_tmp216.elt_type;
_tmp21A=_tmp216.num_elts;_LL205: if(_tmp217){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((
void**)_check_null(_tmp219)),0);if(_tmp218 == 0)s=({struct Cyc_PP_Doc*_tmp27F[5];
_tmp27F[4]=Cyc_PP_text(({const char*_tmp282=")";_tag_dyneither(_tmp282,sizeof(
char),2);}));_tmp27F[3]=Cyc_Absynpp_exp2doc(st);_tmp27F[2]=Cyc_PP_text(({const
char*_tmp281=",";_tag_dyneither(_tmp281,sizeof(char),2);}));_tmp27F[1]=Cyc_Absynpp_exp2doc(
_tmp21A);_tmp27F[0]=Cyc_PP_text(({const char*_tmp280="calloc(";_tag_dyneither(
_tmp280,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp27F,sizeof(struct Cyc_PP_Doc*),
5));});else{s=({struct Cyc_PP_Doc*_tmp283[7];_tmp283[6]=Cyc_PP_text(({const char*
_tmp287=")";_tag_dyneither(_tmp287,sizeof(char),2);}));_tmp283[5]=Cyc_Absynpp_exp2doc(
st);_tmp283[4]=Cyc_PP_text(({const char*_tmp286=",";_tag_dyneither(_tmp286,
sizeof(char),2);}));_tmp283[3]=Cyc_Absynpp_exp2doc(_tmp21A);_tmp283[2]=Cyc_PP_text(({
const char*_tmp285=",";_tag_dyneither(_tmp285,sizeof(char),2);}));_tmp283[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp218);_tmp283[0]=Cyc_PP_text(({const char*_tmp284="rcalloc(";
_tag_dyneither(_tmp284,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp283,
sizeof(struct Cyc_PP_Doc*),7));});}}else{struct Cyc_Absyn_Exp*new_e;if(_tmp219 == 0)
new_e=_tmp21A;else{new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp219,0),
_tmp21A,0);}if(_tmp218 == 0)s=({struct Cyc_PP_Doc*_tmp288[3];_tmp288[2]=Cyc_PP_text(({
const char*_tmp28A=")";_tag_dyneither(_tmp28A,sizeof(char),2);}));_tmp288[1]=Cyc_Absynpp_exp2doc(
new_e);_tmp288[0]=Cyc_PP_text(({const char*_tmp289="malloc(";_tag_dyneither(
_tmp289,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp288,sizeof(struct Cyc_PP_Doc*),
3));});else{s=({struct Cyc_PP_Doc*_tmp28B[5];_tmp28B[4]=Cyc_PP_text(({const char*
_tmp28E=")";_tag_dyneither(_tmp28E,sizeof(char),2);}));_tmp28B[3]=Cyc_Absynpp_exp2doc(
new_e);_tmp28B[2]=Cyc_PP_text(({const char*_tmp28D=",";_tag_dyneither(_tmp28D,
sizeof(char),2);}));_tmp28B[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp218);
_tmp28B[0]=Cyc_PP_text(({const char*_tmp28C="rmalloc(";_tag_dyneither(_tmp28C,
sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp28B,sizeof(struct Cyc_PP_Doc*),
5));});}}goto _LL1B9;_LL206: if(*((int*)_tmp1D5)!= 36)goto _LL208;_tmp21B=((struct
Cyc_Absyn_Swap_e_struct*)_tmp1D5)->f1;_tmp21C=((struct Cyc_Absyn_Swap_e_struct*)
_tmp1D5)->f2;_LL207: s=({struct Cyc_PP_Doc*_tmp28F[5];_tmp28F[4]=Cyc_PP_text(({
const char*_tmp292=")";_tag_dyneither(_tmp292,sizeof(char),2);}));_tmp28F[3]=Cyc_Absynpp_exp2doc(
_tmp21C);_tmp28F[2]=Cyc_PP_text(({const char*_tmp291=",";_tag_dyneither(_tmp291,
sizeof(char),2);}));_tmp28F[1]=Cyc_Absynpp_exp2doc(_tmp21B);_tmp28F[0]=Cyc_PP_text(({
const char*_tmp290="cycswap(";_tag_dyneither(_tmp290,sizeof(char),9);}));Cyc_PP_cat(
_tag_dyneither(_tmp28F,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1B9;_LL208: if(*((
int*)_tmp1D5)!= 37)goto _LL20A;_tmp21D=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp1D5)->f1;_tmp21E=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1D5)->f2;
_LL209: s=Cyc_Absynpp_group_braces(({const char*_tmp293=",";_tag_dyneither(_tmp293,
sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp21E));goto _LL1B9;
_LL20A: if(*((int*)_tmp1D5)!= 38)goto _LL1B9;_tmp21F=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp1D5)->f1;_LL20B: s=({struct Cyc_PP_Doc*_tmp294[7];_tmp294[6]=Cyc_PP_text(({
const char*_tmp296=")";_tag_dyneither(_tmp296,sizeof(char),2);}));_tmp294[5]=Cyc_Absynpp_rb();
_tmp294[4]=Cyc_PP_blank_doc();_tmp294[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp21F));_tmp294[2]=Cyc_PP_blank_doc();_tmp294[1]=Cyc_Absynpp_lb();_tmp294[0]=
Cyc_PP_text(({const char*_tmp295="(";_tag_dyneither(_tmp295,sizeof(char),2);}));
Cyc_PP_cat(_tag_dyneither(_tmp294,sizeof(struct Cyc_PP_Doc*),7));});goto _LL1B9;
_LL1B9:;}if(inprec >= myprec)s=({struct Cyc_PP_Doc*_tmp297[3];_tmp297[2]=Cyc_PP_text(({
const char*_tmp299=")";_tag_dyneither(_tmp299,sizeof(char),2);}));_tmp297[1]=s;
_tmp297[0]=Cyc_PP_text(({const char*_tmp298="(";_tag_dyneither(_tmp298,sizeof(
char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp297,sizeof(struct Cyc_PP_Doc*),3));});
return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){void*_tmp29A=d;
struct Cyc_Absyn_Exp*_tmp29B;struct _dyneither_ptr*_tmp29C;_LL216: if(*((int*)
_tmp29A)!= 0)goto _LL218;_tmp29B=((struct Cyc_Absyn_ArrayElement_struct*)_tmp29A)->f1;
_LL217: return({struct Cyc_PP_Doc*_tmp29D[3];_tmp29D[2]=Cyc_PP_text(({const char*
_tmp29F="]";_tag_dyneither(_tmp29F,sizeof(char),2);}));_tmp29D[1]=Cyc_Absynpp_exp2doc(
_tmp29B);_tmp29D[0]=Cyc_PP_text(({const char*_tmp29E=".[";_tag_dyneither(_tmp29E,
sizeof(char),3);}));Cyc_PP_cat(_tag_dyneither(_tmp29D,sizeof(struct Cyc_PP_Doc*),
3));});_LL218: if(*((int*)_tmp29A)!= 1)goto _LL215;_tmp29C=((struct Cyc_Absyn_FieldName_struct*)
_tmp29A)->f1;_LL219: return({struct Cyc_PP_Doc*_tmp2A0[2];_tmp2A0[1]=Cyc_PP_textptr(
_tmp29C);_tmp2A0[0]=Cyc_PP_text(({const char*_tmp2A1=".";_tag_dyneither(_tmp2A1,
sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp2A0,sizeof(struct Cyc_PP_Doc*),
2));});_LL215:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple9*de){if((*de).f1
== 0)return Cyc_Absynpp_exp2doc((*de).f2);else{return({struct Cyc_PP_Doc*_tmp2A2[2];
_tmp2A2[1]=Cyc_Absynpp_exp2doc((*de).f2);_tmp2A2[0]=Cyc_PP_egroup(({const char*
_tmp2A3="";_tag_dyneither(_tmp2A3,sizeof(char),1);}),({const char*_tmp2A4="=";
_tag_dyneither(_tmp2A4,sizeof(char),2);}),({const char*_tmp2A5="=";_tag_dyneither(
_tmp2A5,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));Cyc_PP_cat(
_tag_dyneither(_tmp2A2,sizeof(struct Cyc_PP_Doc*),2));});}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(
int inprec,struct Cyc_List_List*es){return Cyc_PP_group(({const char*_tmp2A6="";
_tag_dyneither(_tmp2A6,sizeof(char),1);}),({const char*_tmp2A7="";_tag_dyneither(
_tmp2A7,sizeof(char),1);}),({const char*_tmp2A8=",";_tag_dyneither(_tmp2A8,
sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),
int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst_union c){union Cyc_Absyn_Cnst_union
_tmp2A9=c;void*_tmp2AA;char _tmp2AB;void*_tmp2AC;short _tmp2AD;void*_tmp2AE;int
_tmp2AF;void*_tmp2B0;int _tmp2B1;void*_tmp2B2;int _tmp2B3;void*_tmp2B4;long long
_tmp2B5;struct _dyneither_ptr _tmp2B6;struct _dyneither_ptr _tmp2B7;_LL21B: if((
_tmp2A9.Char_c).tag != 0)goto _LL21D;_tmp2AA=(_tmp2A9.Char_c).f1;_tmp2AB=(_tmp2A9.Char_c).f2;
_LL21C: return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp2BA;_tmp2BA.tag=0;_tmp2BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_char_escape(_tmp2AB));{void*_tmp2B8[1]={& _tmp2BA};Cyc_aprintf(({
const char*_tmp2B9="'%s'";_tag_dyneither(_tmp2B9,sizeof(char),5);}),
_tag_dyneither(_tmp2B8,sizeof(void*),1));}}));_LL21D: if((_tmp2A9.Short_c).tag != 
1)goto _LL21F;_tmp2AC=(_tmp2A9.Short_c).f1;_tmp2AD=(_tmp2A9.Short_c).f2;_LL21E:
return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp2BD;_tmp2BD.tag=
1;_tmp2BD.f1=(unsigned long)((int)_tmp2AD);{void*_tmp2BB[1]={& _tmp2BD};Cyc_aprintf(({
const char*_tmp2BC="%d";_tag_dyneither(_tmp2BC,sizeof(char),3);}),_tag_dyneither(
_tmp2BB,sizeof(void*),1));}}));_LL21F: if((_tmp2A9.Int_c).tag != 2)goto _LL221;
_tmp2AE=(_tmp2A9.Int_c).f1;if((int)_tmp2AE != 2)goto _LL221;_tmp2AF=(_tmp2A9.Int_c).f2;
_LL220: _tmp2B1=_tmp2AF;goto _LL222;_LL221: if((_tmp2A9.Int_c).tag != 2)goto _LL223;
_tmp2B0=(_tmp2A9.Int_c).f1;if((int)_tmp2B0 != 0)goto _LL223;_tmp2B1=(_tmp2A9.Int_c).f2;
_LL222: return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp2C0;
_tmp2C0.tag=1;_tmp2C0.f1=(unsigned long)_tmp2B1;{void*_tmp2BE[1]={& _tmp2C0};Cyc_aprintf(({
const char*_tmp2BF="%d";_tag_dyneither(_tmp2BF,sizeof(char),3);}),_tag_dyneither(
_tmp2BE,sizeof(void*),1));}}));_LL223: if((_tmp2A9.Int_c).tag != 2)goto _LL225;
_tmp2B2=(_tmp2A9.Int_c).f1;if((int)_tmp2B2 != 1)goto _LL225;_tmp2B3=(_tmp2A9.Int_c).f2;
_LL224: return Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp2C3;
_tmp2C3.tag=1;_tmp2C3.f1=(unsigned int)_tmp2B3;{void*_tmp2C1[1]={& _tmp2C3};Cyc_aprintf(({
const char*_tmp2C2="%u";_tag_dyneither(_tmp2C2,sizeof(char),3);}),_tag_dyneither(
_tmp2C1,sizeof(void*),1));}}));_LL225: if((_tmp2A9.LongLong_c).tag != 3)goto _LL227;
_tmp2B4=(_tmp2A9.LongLong_c).f1;_tmp2B5=(_tmp2A9.LongLong_c).f2;_LL226: return Cyc_PP_text(({
const char*_tmp2C4="<<FIX LONG LONG CONSTANT>>";_tag_dyneither(_tmp2C4,sizeof(
char),27);}));_LL227: if((_tmp2A9.Float_c).tag != 4)goto _LL229;_tmp2B6=(_tmp2A9.Float_c).f1;
_LL228: return Cyc_PP_text(_tmp2B6);_LL229: if((_tmp2A9.Null_c).tag != 6)goto _LL22B;
_LL22A: return Cyc_PP_text(({const char*_tmp2C5="NULL";_tag_dyneither(_tmp2C5,
sizeof(char),5);}));_LL22B: if((_tmp2A9.String_c).tag != 5)goto _LL21A;_tmp2B7=(
_tmp2A9.String_c).f1;_LL22C: return({struct Cyc_PP_Doc*_tmp2C6[3];_tmp2C6[2]=Cyc_PP_text(({
const char*_tmp2C8="\"";_tag_dyneither(_tmp2C8,sizeof(char),2);}));_tmp2C6[1]=Cyc_PP_text(
Cyc_Absynpp_string_escape(_tmp2B7));_tmp2C6[0]=Cyc_PP_text(({const char*_tmp2C7="\"";
_tag_dyneither(_tmp2C7,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp2C6,
sizeof(struct Cyc_PP_Doc*),3));});_LL21A:;}struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(
int inprec,void*p,struct Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(
p);if(p == (void*)((void*)19)){if(es == 0  || es->tl != 0)(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));_tmp2C9[0]=({
struct Cyc_Core_Failure_struct _tmp2CA;_tmp2CA.tag=Cyc_Core_Failure;_tmp2CA.f1=(
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp2CD;_tmp2CD.tag=0;_tmp2CD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72));{void*
_tmp2CB[1]={& _tmp2CD};Cyc_aprintf(({const char*_tmp2CC="Absynpp::primapp2doc Numelts: %s with bad args";
_tag_dyneither(_tmp2CC,sizeof(char),47);}),_tag_dyneither(_tmp2CB,sizeof(void*),
1));}});_tmp2CA;});_tmp2C9;}));return({struct Cyc_PP_Doc*_tmp2CE[3];_tmp2CE[2]=
Cyc_PP_text(({const char*_tmp2D0=")";_tag_dyneither(_tmp2D0,sizeof(char),2);}));
_tmp2CE[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_tmp2CE[0]=Cyc_PP_text(({
const char*_tmp2CF="numelts(";_tag_dyneither(_tmp2CF,sizeof(char),9);}));Cyc_PP_cat(
_tag_dyneither(_tmp2CE,sizeof(struct Cyc_PP_Doc*),3));});}else{struct Cyc_List_List*
ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2D1=_cycalloc(
sizeof(*_tmp2D1));_tmp2D1[0]=({struct Cyc_Core_Failure_struct _tmp2D2;_tmp2D2.tag=
Cyc_Core_Failure;_tmp2D2.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp2D5;_tmp2D5.tag=0;_tmp2D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_PP_string_of_doc(ps,72));{void*_tmp2D3[1]={& _tmp2D5};Cyc_aprintf(({const char*
_tmp2D4="Absynpp::primapp2doc: %s with no args";_tag_dyneither(_tmp2D4,sizeof(
char),38);}),_tag_dyneither(_tmp2D3,sizeof(void*),1));}});_tmp2D2;});_tmp2D1;}));
else{if(ds->tl == 0)return({struct Cyc_PP_Doc*_tmp2D6[3];_tmp2D6[2]=(struct Cyc_PP_Doc*)
ds->hd;_tmp2D6[1]=Cyc_PP_text(({const char*_tmp2D7=" ";_tag_dyneither(_tmp2D7,
sizeof(char),2);}));_tmp2D6[0]=ps;Cyc_PP_cat(_tag_dyneither(_tmp2D6,sizeof(
struct Cyc_PP_Doc*),3));});else{if(((struct Cyc_List_List*)_check_null(ds->tl))->tl
!= 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp2D8=_cycalloc(
sizeof(*_tmp2D8));_tmp2D8[0]=({struct Cyc_Core_Failure_struct _tmp2D9;_tmp2D9.tag=
Cyc_Core_Failure;_tmp2D9.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp2DC;_tmp2DC.tag=0;_tmp2DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_PP_string_of_doc(ps,72));{void*_tmp2DA[1]={& _tmp2DC};Cyc_aprintf(({const char*
_tmp2DB="Absynpp::primapp2doc: %s with more than 2 args";_tag_dyneither(_tmp2DB,
sizeof(char),47);}),_tag_dyneither(_tmp2DA,sizeof(void*),1));}});_tmp2D9;});
_tmp2D8;}));else{return({struct Cyc_PP_Doc*_tmp2DD[5];_tmp2DD[4]=(struct Cyc_PP_Doc*)((
struct Cyc_List_List*)_check_null(ds->tl))->hd;_tmp2DD[3]=Cyc_PP_text(({const char*
_tmp2DF=" ";_tag_dyneither(_tmp2DF,sizeof(char),2);}));_tmp2DD[2]=ps;_tmp2DD[1]=
Cyc_PP_text(({const char*_tmp2DE=" ";_tag_dyneither(_tmp2DE,sizeof(char),2);}));
_tmp2DD[0]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_dyneither(_tmp2DD,sizeof(
struct Cyc_PP_Doc*),5));});}}}}}struct _dyneither_ptr Cyc_Absynpp_prim2str(void*p){
void*_tmp2E0=p;_LL22E: if((int)_tmp2E0 != 0)goto _LL230;_LL22F: return({const char*
_tmp2E1="+";_tag_dyneither(_tmp2E1,sizeof(char),2);});_LL230: if((int)_tmp2E0 != 1)
goto _LL232;_LL231: return({const char*_tmp2E2="*";_tag_dyneither(_tmp2E2,sizeof(
char),2);});_LL232: if((int)_tmp2E0 != 2)goto _LL234;_LL233: return({const char*
_tmp2E3="-";_tag_dyneither(_tmp2E3,sizeof(char),2);});_LL234: if((int)_tmp2E0 != 3)
goto _LL236;_LL235: return({const char*_tmp2E4="/";_tag_dyneither(_tmp2E4,sizeof(
char),2);});_LL236: if((int)_tmp2E0 != 4)goto _LL238;_LL237: return({const char*
_tmp2E5="%";_tag_dyneither(_tmp2E5,sizeof(char),2);});_LL238: if((int)_tmp2E0 != 5)
goto _LL23A;_LL239: return({const char*_tmp2E6="==";_tag_dyneither(_tmp2E6,sizeof(
char),3);});_LL23A: if((int)_tmp2E0 != 6)goto _LL23C;_LL23B: return({const char*
_tmp2E7="!=";_tag_dyneither(_tmp2E7,sizeof(char),3);});_LL23C: if((int)_tmp2E0 != 
7)goto _LL23E;_LL23D: return({const char*_tmp2E8=">";_tag_dyneither(_tmp2E8,sizeof(
char),2);});_LL23E: if((int)_tmp2E0 != 8)goto _LL240;_LL23F: return({const char*
_tmp2E9="<";_tag_dyneither(_tmp2E9,sizeof(char),2);});_LL240: if((int)_tmp2E0 != 9)
goto _LL242;_LL241: return({const char*_tmp2EA=">=";_tag_dyneither(_tmp2EA,sizeof(
char),3);});_LL242: if((int)_tmp2E0 != 10)goto _LL244;_LL243: return({const char*
_tmp2EB="<=";_tag_dyneither(_tmp2EB,sizeof(char),3);});_LL244: if((int)_tmp2E0 != 
11)goto _LL246;_LL245: return({const char*_tmp2EC="!";_tag_dyneither(_tmp2EC,
sizeof(char),2);});_LL246: if((int)_tmp2E0 != 12)goto _LL248;_LL247: return({const
char*_tmp2ED="~";_tag_dyneither(_tmp2ED,sizeof(char),2);});_LL248: if((int)
_tmp2E0 != 13)goto _LL24A;_LL249: return({const char*_tmp2EE="&";_tag_dyneither(
_tmp2EE,sizeof(char),2);});_LL24A: if((int)_tmp2E0 != 14)goto _LL24C;_LL24B: return({
const char*_tmp2EF="|";_tag_dyneither(_tmp2EF,sizeof(char),2);});_LL24C: if((int)
_tmp2E0 != 15)goto _LL24E;_LL24D: return({const char*_tmp2F0="^";_tag_dyneither(
_tmp2F0,sizeof(char),2);});_LL24E: if((int)_tmp2E0 != 16)goto _LL250;_LL24F: return({
const char*_tmp2F1="<<";_tag_dyneither(_tmp2F1,sizeof(char),3);});_LL250: if((int)
_tmp2E0 != 17)goto _LL252;_LL251: return({const char*_tmp2F2=">>";_tag_dyneither(
_tmp2F2,sizeof(char),3);});_LL252: if((int)_tmp2E0 != 18)goto _LL254;_LL253: return({
const char*_tmp2F3=">>>";_tag_dyneither(_tmp2F3,sizeof(char),4);});_LL254: if((int)
_tmp2E0 != 19)goto _LL22D;_LL255: return({const char*_tmp2F4="numelts";
_tag_dyneither(_tmp2F4,sizeof(char),8);});_LL22D:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
void*p){return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp2F5=(void*)s->r;_LL257: if(_tmp2F5 <= (void*)1)
goto _LL259;if(*((int*)_tmp2F5)!= 11)goto _LL259;_LL258: return 1;_LL259:;_LL25A:
return 0;_LL256:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st){
struct Cyc_PP_Doc*s;{void*_tmp2F6=(void*)st->r;struct Cyc_Absyn_Exp*_tmp2F7;struct
Cyc_Absyn_Stmt*_tmp2F8;struct Cyc_Absyn_Stmt*_tmp2F9;struct Cyc_Absyn_Exp*_tmp2FA;
struct Cyc_Absyn_Exp*_tmp2FB;struct Cyc_Absyn_Stmt*_tmp2FC;struct Cyc_Absyn_Stmt*
_tmp2FD;struct _tuple2 _tmp2FE;struct Cyc_Absyn_Exp*_tmp2FF;struct Cyc_Absyn_Stmt*
_tmp300;struct _dyneither_ptr*_tmp301;struct Cyc_Absyn_Exp*_tmp302;struct _tuple2
_tmp303;struct Cyc_Absyn_Exp*_tmp304;struct _tuple2 _tmp305;struct Cyc_Absyn_Exp*
_tmp306;struct Cyc_Absyn_Stmt*_tmp307;struct Cyc_Absyn_Exp*_tmp308;struct Cyc_List_List*
_tmp309;struct Cyc_List_List*_tmp30A;struct Cyc_List_List*_tmp30B;struct Cyc_Absyn_Decl*
_tmp30C;struct Cyc_Absyn_Stmt*_tmp30D;struct _dyneither_ptr*_tmp30E;struct Cyc_Absyn_Stmt*
_tmp30F;struct Cyc_Absyn_Stmt*_tmp310;struct _tuple2 _tmp311;struct Cyc_Absyn_Exp*
_tmp312;struct Cyc_Absyn_Stmt*_tmp313;struct Cyc_List_List*_tmp314;struct Cyc_Absyn_Exp*
_tmp315;_LL25C: if((int)_tmp2F6 != 0)goto _LL25E;_LL25D: s=Cyc_PP_text(({const char*
_tmp316=";";_tag_dyneither(_tmp316,sizeof(char),2);}));goto _LL25B;_LL25E: if(
_tmp2F6 <= (void*)1)goto _LL260;if(*((int*)_tmp2F6)!= 0)goto _LL260;_tmp2F7=((
struct Cyc_Absyn_Exp_s_struct*)_tmp2F6)->f1;_LL25F: s=({struct Cyc_PP_Doc*_tmp317[2];
_tmp317[1]=Cyc_PP_text(({const char*_tmp318=";";_tag_dyneither(_tmp318,sizeof(
char),2);}));_tmp317[0]=Cyc_Absynpp_exp2doc(_tmp2F7);Cyc_PP_cat(_tag_dyneither(
_tmp317,sizeof(struct Cyc_PP_Doc*),2));});goto _LL25B;_LL260: if(_tmp2F6 <= (void*)1)
goto _LL262;if(*((int*)_tmp2F6)!= 1)goto _LL262;_tmp2F8=((struct Cyc_Absyn_Seq_s_struct*)
_tmp2F6)->f1;_tmp2F9=((struct Cyc_Absyn_Seq_s_struct*)_tmp2F6)->f2;_LL261: if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp2F8))s=({struct Cyc_PP_Doc*_tmp319[7];_tmp319[6]=
Cyc_Absynpp_is_declaration(_tmp2F9)?({struct Cyc_PP_Doc*_tmp31A[5];_tmp31A[4]=Cyc_PP_line_doc();
_tmp31A[3]=Cyc_Absynpp_rb();_tmp31A[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp2F9));_tmp31A[1]=Cyc_PP_blank_doc();_tmp31A[0]=Cyc_Absynpp_lb();Cyc_PP_cat(
_tag_dyneither(_tmp31A,sizeof(struct Cyc_PP_Doc*),5));}): Cyc_Absynpp_stmt2doc(
_tmp2F9);_tmp319[5]=Cyc_PP_line_doc();_tmp319[4]=Cyc_Absynpp_rb();_tmp319[3]=Cyc_PP_line_doc();
_tmp319[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2F8));_tmp319[1]=Cyc_PP_blank_doc();
_tmp319[0]=Cyc_Absynpp_lb();Cyc_PP_cat(_tag_dyneither(_tmp319,sizeof(struct Cyc_PP_Doc*),
7));});else{if(Cyc_Absynpp_is_declaration(_tmp2F9))s=({struct Cyc_PP_Doc*_tmp31B[
7];_tmp31B[6]=Cyc_PP_line_doc();_tmp31B[5]=Cyc_Absynpp_rb();_tmp31B[4]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp2F9));_tmp31B[3]=Cyc_PP_blank_doc();_tmp31B[2]=Cyc_Absynpp_lb();
_tmp31B[1]=Cyc_PP_line_doc();_tmp31B[0]=Cyc_Absynpp_stmt2doc(_tmp2F8);Cyc_PP_cat(
_tag_dyneither(_tmp31B,sizeof(struct Cyc_PP_Doc*),7));});else{s=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,({const char*_tmp31C="";_tag_dyneither(
_tmp31C,sizeof(char),1);}),({struct Cyc_List_List*_tmp31D=_cycalloc(sizeof(*
_tmp31D));_tmp31D->hd=_tmp2F8;_tmp31D->tl=({struct Cyc_List_List*_tmp31E=
_cycalloc(sizeof(*_tmp31E));_tmp31E->hd=_tmp2F9;_tmp31E->tl=0;_tmp31E;});_tmp31D;}));}}}
else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,({
const char*_tmp31F="";_tag_dyneither(_tmp31F,sizeof(char),1);}),({struct Cyc_List_List*
_tmp320=_cycalloc(sizeof(*_tmp320));_tmp320->hd=_tmp2F8;_tmp320->tl=({struct Cyc_List_List*
_tmp321=_cycalloc(sizeof(*_tmp321));_tmp321->hd=_tmp2F9;_tmp321->tl=0;_tmp321;});
_tmp320;}));}goto _LL25B;_LL262: if(_tmp2F6 <= (void*)1)goto _LL264;if(*((int*)
_tmp2F6)!= 2)goto _LL264;_tmp2FA=((struct Cyc_Absyn_Return_s_struct*)_tmp2F6)->f1;
_LL263: if(_tmp2FA == 0)s=Cyc_PP_text(({const char*_tmp322="return;";_tag_dyneither(
_tmp322,sizeof(char),8);}));else{s=({struct Cyc_PP_Doc*_tmp323[3];_tmp323[2]=Cyc_PP_text(({
const char*_tmp325=";";_tag_dyneither(_tmp325,sizeof(char),2);}));_tmp323[1]=
_tmp2FA == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2FA);
_tmp323[0]=Cyc_PP_text(({const char*_tmp324="return ";_tag_dyneither(_tmp324,
sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp323,sizeof(struct Cyc_PP_Doc*),
3));});}goto _LL25B;_LL264: if(_tmp2F6 <= (void*)1)goto _LL266;if(*((int*)_tmp2F6)!= 
3)goto _LL266;_tmp2FB=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2F6)->f1;_tmp2FC=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2F6)->f2;_tmp2FD=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp2F6)->f3;_LL265: {int print_else;{void*_tmp326=(void*)_tmp2FD->r;_LL281: if((
int)_tmp326 != 0)goto _LL283;_LL282: print_else=0;goto _LL280;_LL283:;_LL284:
print_else=1;goto _LL280;_LL280:;}s=({struct Cyc_PP_Doc*_tmp327[8];_tmp327[7]=
print_else?({struct Cyc_PP_Doc*_tmp32B[6];_tmp32B[5]=Cyc_Absynpp_rb();_tmp32B[4]=
Cyc_PP_line_doc();_tmp32B[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp32D[2];_tmp32D[
1]=Cyc_Absynpp_stmt2doc(_tmp2FD);_tmp32D[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_dyneither(_tmp32D,sizeof(struct Cyc_PP_Doc*),2));}));_tmp32B[2]=Cyc_Absynpp_lb();
_tmp32B[1]=Cyc_PP_text(({const char*_tmp32C="else ";_tag_dyneither(_tmp32C,
sizeof(char),6);}));_tmp32B[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(
_tmp32B,sizeof(struct Cyc_PP_Doc*),6));}): Cyc_PP_nil_doc();_tmp327[6]=Cyc_Absynpp_rb();
_tmp327[5]=Cyc_PP_line_doc();_tmp327[4]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp32A[
2];_tmp32A[1]=Cyc_Absynpp_stmt2doc(_tmp2FC);_tmp32A[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_dyneither(_tmp32A,sizeof(struct Cyc_PP_Doc*),2));}));_tmp327[3]=Cyc_Absynpp_lb();
_tmp327[2]=Cyc_PP_text(({const char*_tmp329=") ";_tag_dyneither(_tmp329,sizeof(
char),3);}));_tmp327[1]=Cyc_Absynpp_exp2doc(_tmp2FB);_tmp327[0]=Cyc_PP_text(({
const char*_tmp328="if (";_tag_dyneither(_tmp328,sizeof(char),5);}));Cyc_PP_cat(
_tag_dyneither(_tmp327,sizeof(struct Cyc_PP_Doc*),8));});goto _LL25B;}_LL266: if(
_tmp2F6 <= (void*)1)goto _LL268;if(*((int*)_tmp2F6)!= 4)goto _LL268;_tmp2FE=((
struct Cyc_Absyn_While_s_struct*)_tmp2F6)->f1;_tmp2FF=_tmp2FE.f1;_tmp300=((struct
Cyc_Absyn_While_s_struct*)_tmp2F6)->f2;_LL267: s=({struct Cyc_PP_Doc*_tmp32E[7];
_tmp32E[6]=Cyc_Absynpp_rb();_tmp32E[5]=Cyc_PP_line_doc();_tmp32E[4]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp331[2];_tmp331[1]=Cyc_Absynpp_stmt2doc(_tmp300);_tmp331[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp331,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp32E[3]=Cyc_Absynpp_lb();_tmp32E[2]=Cyc_PP_text(({const char*_tmp330=") ";
_tag_dyneither(_tmp330,sizeof(char),3);}));_tmp32E[1]=Cyc_Absynpp_exp2doc(
_tmp2FF);_tmp32E[0]=Cyc_PP_text(({const char*_tmp32F="while (";_tag_dyneither(
_tmp32F,sizeof(char),8);}));Cyc_PP_cat(_tag_dyneither(_tmp32E,sizeof(struct Cyc_PP_Doc*),
7));});goto _LL25B;_LL268: if(_tmp2F6 <= (void*)1)goto _LL26A;if(*((int*)_tmp2F6)!= 
5)goto _LL26A;_LL269: s=Cyc_PP_text(({const char*_tmp332="break;";_tag_dyneither(
_tmp332,sizeof(char),7);}));goto _LL25B;_LL26A: if(_tmp2F6 <= (void*)1)goto _LL26C;
if(*((int*)_tmp2F6)!= 6)goto _LL26C;_LL26B: s=Cyc_PP_text(({const char*_tmp333="continue;";
_tag_dyneither(_tmp333,sizeof(char),10);}));goto _LL25B;_LL26C: if(_tmp2F6 <= (void*)
1)goto _LL26E;if(*((int*)_tmp2F6)!= 7)goto _LL26E;_tmp301=((struct Cyc_Absyn_Goto_s_struct*)
_tmp2F6)->f1;_LL26D: s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp336;_tmp336.tag=0;_tmp336.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp301);{void*_tmp334[1]={& _tmp336};Cyc_aprintf(({const char*_tmp335="goto %s;";
_tag_dyneither(_tmp335,sizeof(char),9);}),_tag_dyneither(_tmp334,sizeof(void*),1));}}));
goto _LL25B;_LL26E: if(_tmp2F6 <= (void*)1)goto _LL270;if(*((int*)_tmp2F6)!= 8)goto
_LL270;_tmp302=((struct Cyc_Absyn_For_s_struct*)_tmp2F6)->f1;_tmp303=((struct Cyc_Absyn_For_s_struct*)
_tmp2F6)->f2;_tmp304=_tmp303.f1;_tmp305=((struct Cyc_Absyn_For_s_struct*)_tmp2F6)->f3;
_tmp306=_tmp305.f1;_tmp307=((struct Cyc_Absyn_For_s_struct*)_tmp2F6)->f4;_LL26F: s=({
struct Cyc_PP_Doc*_tmp337[11];_tmp337[10]=Cyc_Absynpp_rb();_tmp337[9]=Cyc_PP_line_doc();
_tmp337[8]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp33C[2];_tmp33C[1]=Cyc_Absynpp_stmt2doc(
_tmp307);_tmp33C[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp33C,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp337[7]=Cyc_Absynpp_lb();_tmp337[6]=Cyc_PP_text(({
const char*_tmp33B=") ";_tag_dyneither(_tmp33B,sizeof(char),3);}));_tmp337[5]=Cyc_Absynpp_exp2doc(
_tmp306);_tmp337[4]=Cyc_PP_text(({const char*_tmp33A="; ";_tag_dyneither(_tmp33A,
sizeof(char),3);}));_tmp337[3]=Cyc_Absynpp_exp2doc(_tmp304);_tmp337[2]=Cyc_PP_text(({
const char*_tmp339="; ";_tag_dyneither(_tmp339,sizeof(char),3);}));_tmp337[1]=Cyc_Absynpp_exp2doc(
_tmp302);_tmp337[0]=Cyc_PP_text(({const char*_tmp338="for(";_tag_dyneither(
_tmp338,sizeof(char),5);}));Cyc_PP_cat(_tag_dyneither(_tmp337,sizeof(struct Cyc_PP_Doc*),
11));});goto _LL25B;_LL270: if(_tmp2F6 <= (void*)1)goto _LL272;if(*((int*)_tmp2F6)!= 
9)goto _LL272;_tmp308=((struct Cyc_Absyn_Switch_s_struct*)_tmp2F6)->f1;_tmp309=((
struct Cyc_Absyn_Switch_s_struct*)_tmp2F6)->f2;_LL271: s=({struct Cyc_PP_Doc*
_tmp33D[8];_tmp33D[7]=Cyc_Absynpp_rb();_tmp33D[6]=Cyc_PP_line_doc();_tmp33D[5]=
Cyc_Absynpp_switchclauses2doc(_tmp309);_tmp33D[4]=Cyc_PP_line_doc();_tmp33D[3]=
Cyc_Absynpp_lb();_tmp33D[2]=Cyc_PP_text(({const char*_tmp33F=") ";_tag_dyneither(
_tmp33F,sizeof(char),3);}));_tmp33D[1]=Cyc_Absynpp_exp2doc(_tmp308);_tmp33D[0]=
Cyc_PP_text(({const char*_tmp33E="switch (";_tag_dyneither(_tmp33E,sizeof(char),9);}));
Cyc_PP_cat(_tag_dyneither(_tmp33D,sizeof(struct Cyc_PP_Doc*),8));});goto _LL25B;
_LL272: if(_tmp2F6 <= (void*)1)goto _LL274;if(*((int*)_tmp2F6)!= 10)goto _LL274;
_tmp30A=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp2F6)->f1;if(_tmp30A != 0)goto
_LL274;_LL273: s=Cyc_PP_text(({const char*_tmp340="fallthru;";_tag_dyneither(
_tmp340,sizeof(char),10);}));goto _LL25B;_LL274: if(_tmp2F6 <= (void*)1)goto _LL276;
if(*((int*)_tmp2F6)!= 10)goto _LL276;_tmp30B=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp2F6)->f1;_LL275: s=({struct Cyc_PP_Doc*_tmp341[3];_tmp341[2]=Cyc_PP_text(({
const char*_tmp343=");";_tag_dyneither(_tmp343,sizeof(char),3);}));_tmp341[1]=Cyc_Absynpp_exps2doc_prec(
20,_tmp30B);_tmp341[0]=Cyc_PP_text(({const char*_tmp342="fallthru(";
_tag_dyneither(_tmp342,sizeof(char),10);}));Cyc_PP_cat(_tag_dyneither(_tmp341,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL25B;_LL276: if(_tmp2F6 <= (void*)1)goto
_LL278;if(*((int*)_tmp2F6)!= 11)goto _LL278;_tmp30C=((struct Cyc_Absyn_Decl_s_struct*)
_tmp2F6)->f1;_tmp30D=((struct Cyc_Absyn_Decl_s_struct*)_tmp2F6)->f2;_LL277: s=({
struct Cyc_PP_Doc*_tmp344[3];_tmp344[2]=Cyc_Absynpp_stmt2doc(_tmp30D);_tmp344[1]=
Cyc_PP_line_doc();_tmp344[0]=Cyc_Absynpp_decl2doc(_tmp30C);Cyc_PP_cat(
_tag_dyneither(_tmp344,sizeof(struct Cyc_PP_Doc*),3));});goto _LL25B;_LL278: if(
_tmp2F6 <= (void*)1)goto _LL27A;if(*((int*)_tmp2F6)!= 12)goto _LL27A;_tmp30E=((
struct Cyc_Absyn_Label_s_struct*)_tmp2F6)->f1;_tmp30F=((struct Cyc_Absyn_Label_s_struct*)
_tmp2F6)->f2;_LL279: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(
_tmp30F))s=({struct Cyc_PP_Doc*_tmp345[7];_tmp345[6]=Cyc_Absynpp_rb();_tmp345[5]=
Cyc_PP_line_doc();_tmp345[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp30F));
_tmp345[3]=Cyc_PP_line_doc();_tmp345[2]=Cyc_Absynpp_lb();_tmp345[1]=Cyc_PP_text(({
const char*_tmp346=": ";_tag_dyneither(_tmp346,sizeof(char),3);}));_tmp345[0]=Cyc_PP_textptr(
_tmp30E);Cyc_PP_cat(_tag_dyneither(_tmp345,sizeof(struct Cyc_PP_Doc*),7));});
else{s=({struct Cyc_PP_Doc*_tmp347[3];_tmp347[2]=Cyc_Absynpp_stmt2doc(_tmp30F);
_tmp347[1]=Cyc_PP_text(({const char*_tmp348=": ";_tag_dyneither(_tmp348,sizeof(
char),3);}));_tmp347[0]=Cyc_PP_textptr(_tmp30E);Cyc_PP_cat(_tag_dyneither(
_tmp347,sizeof(struct Cyc_PP_Doc*),3));});}goto _LL25B;_LL27A: if(_tmp2F6 <= (void*)
1)goto _LL27C;if(*((int*)_tmp2F6)!= 13)goto _LL27C;_tmp310=((struct Cyc_Absyn_Do_s_struct*)
_tmp2F6)->f1;_tmp311=((struct Cyc_Absyn_Do_s_struct*)_tmp2F6)->f2;_tmp312=_tmp311.f1;
_LL27B: s=({struct Cyc_PP_Doc*_tmp349[9];_tmp349[8]=Cyc_PP_text(({const char*
_tmp34C=");";_tag_dyneither(_tmp34C,sizeof(char),3);}));_tmp349[7]=Cyc_Absynpp_exp2doc(
_tmp312);_tmp349[6]=Cyc_PP_text(({const char*_tmp34B=" while (";_tag_dyneither(
_tmp34B,sizeof(char),9);}));_tmp349[5]=Cyc_Absynpp_rb();_tmp349[4]=Cyc_PP_line_doc();
_tmp349[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp310));_tmp349[2]=Cyc_PP_line_doc();
_tmp349[1]=Cyc_Absynpp_lb();_tmp349[0]=Cyc_PP_text(({const char*_tmp34A="do ";
_tag_dyneither(_tmp34A,sizeof(char),4);}));Cyc_PP_cat(_tag_dyneither(_tmp349,
sizeof(struct Cyc_PP_Doc*),9));});goto _LL25B;_LL27C: if(_tmp2F6 <= (void*)1)goto
_LL27E;if(*((int*)_tmp2F6)!= 14)goto _LL27E;_tmp313=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp2F6)->f1;_tmp314=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp2F6)->f2;_LL27D: s=({
struct Cyc_PP_Doc*_tmp34D[12];_tmp34D[11]=Cyc_Absynpp_rb();_tmp34D[10]=Cyc_PP_line_doc();
_tmp34D[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp314));_tmp34D[8]=Cyc_PP_line_doc();
_tmp34D[7]=Cyc_Absynpp_lb();_tmp34D[6]=Cyc_PP_text(({const char*_tmp34F=" catch ";
_tag_dyneither(_tmp34F,sizeof(char),8);}));_tmp34D[5]=Cyc_Absynpp_rb();_tmp34D[4]=
Cyc_PP_line_doc();_tmp34D[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp313));
_tmp34D[2]=Cyc_PP_line_doc();_tmp34D[1]=Cyc_Absynpp_lb();_tmp34D[0]=Cyc_PP_text(({
const char*_tmp34E="try ";_tag_dyneither(_tmp34E,sizeof(char),5);}));Cyc_PP_cat(
_tag_dyneither(_tmp34D,sizeof(struct Cyc_PP_Doc*),12));});goto _LL25B;_LL27E: if(
_tmp2F6 <= (void*)1)goto _LL25B;if(*((int*)_tmp2F6)!= 15)goto _LL25B;_tmp315=((
struct Cyc_Absyn_ResetRegion_s_struct*)_tmp2F6)->f1;_LL27F: s=({struct Cyc_PP_Doc*
_tmp350[3];_tmp350[2]=Cyc_PP_text(({const char*_tmp352=");";_tag_dyneither(
_tmp352,sizeof(char),3);}));_tmp350[1]=Cyc_Absynpp_exp2doc(_tmp315);_tmp350[0]=
Cyc_PP_text(({const char*_tmp351="reset_region(";_tag_dyneither(_tmp351,sizeof(
char),14);}));Cyc_PP_cat(_tag_dyneither(_tmp350,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL25B;_LL25B:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*
p){struct Cyc_PP_Doc*s;{void*_tmp353=(void*)p->r;void*_tmp354;int _tmp355;char
_tmp356;struct _dyneither_ptr _tmp357;struct Cyc_Absyn_Vardecl*_tmp358;struct Cyc_Absyn_Pat*
_tmp359;struct Cyc_Absyn_Pat _tmp35A;void*_tmp35B;struct Cyc_Absyn_Vardecl*_tmp35C;
struct Cyc_Absyn_Pat*_tmp35D;struct Cyc_Absyn_Tvar*_tmp35E;struct Cyc_Absyn_Vardecl*
_tmp35F;struct Cyc_List_List*_tmp360;int _tmp361;struct Cyc_Absyn_Pat*_tmp362;
struct Cyc_Absyn_Vardecl*_tmp363;struct Cyc_Absyn_Pat*_tmp364;struct Cyc_Absyn_Pat
_tmp365;void*_tmp366;struct Cyc_Absyn_Vardecl*_tmp367;struct Cyc_Absyn_Pat*_tmp368;
struct _tuple0*_tmp369;struct _tuple0*_tmp36A;struct Cyc_List_List*_tmp36B;int
_tmp36C;struct Cyc_Absyn_AggrInfo _tmp36D;union Cyc_Absyn_AggrInfoU_union _tmp36E;
struct Cyc_List_List*_tmp36F;struct Cyc_List_List*_tmp370;int _tmp371;struct Cyc_Absyn_Enumfield*
_tmp372;struct Cyc_Absyn_Enumfield*_tmp373;struct Cyc_Absyn_Datatypefield*_tmp374;
struct Cyc_List_List*_tmp375;struct Cyc_Absyn_Datatypefield*_tmp376;struct Cyc_List_List*
_tmp377;int _tmp378;struct Cyc_Absyn_Exp*_tmp379;_LL286: if((int)_tmp353 != 0)goto
_LL288;_LL287: s=Cyc_PP_text(({const char*_tmp37A="_";_tag_dyneither(_tmp37A,
sizeof(char),2);}));goto _LL285;_LL288: if((int)_tmp353 != 1)goto _LL28A;_LL289: s=
Cyc_PP_text(({const char*_tmp37B="NULL";_tag_dyneither(_tmp37B,sizeof(char),5);}));
goto _LL285;_LL28A: if(_tmp353 <= (void*)2)goto _LL28C;if(*((int*)_tmp353)!= 7)goto
_LL28C;_tmp354=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp353)->f1;_tmp355=((
struct Cyc_Absyn_Int_p_struct*)_tmp353)->f2;_LL28B: if(_tmp354 != (void*)((void*)1))
s=Cyc_PP_text((struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp37E;_tmp37E.tag=
1;_tmp37E.f1=(unsigned long)_tmp355;{void*_tmp37C[1]={& _tmp37E};Cyc_aprintf(({
const char*_tmp37D="%d";_tag_dyneither(_tmp37D,sizeof(char),3);}),_tag_dyneither(
_tmp37C,sizeof(void*),1));}}));else{s=Cyc_PP_text((struct _dyneither_ptr)({struct
Cyc_Int_pa_struct _tmp381;_tmp381.tag=1;_tmp381.f1=(unsigned int)_tmp355;{void*
_tmp37F[1]={& _tmp381};Cyc_aprintf(({const char*_tmp380="%u";_tag_dyneither(
_tmp380,sizeof(char),3);}),_tag_dyneither(_tmp37F,sizeof(void*),1));}}));}goto
_LL285;_LL28C: if(_tmp353 <= (void*)2)goto _LL28E;if(*((int*)_tmp353)!= 8)goto
_LL28E;_tmp356=((struct Cyc_Absyn_Char_p_struct*)_tmp353)->f1;_LL28D: s=Cyc_PP_text((
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp384;_tmp384.tag=0;_tmp384.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp356));{
void*_tmp382[1]={& _tmp384};Cyc_aprintf(({const char*_tmp383="'%s'";_tag_dyneither(
_tmp383,sizeof(char),5);}),_tag_dyneither(_tmp382,sizeof(void*),1));}}));goto
_LL285;_LL28E: if(_tmp353 <= (void*)2)goto _LL290;if(*((int*)_tmp353)!= 9)goto
_LL290;_tmp357=((struct Cyc_Absyn_Float_p_struct*)_tmp353)->f1;_LL28F: s=Cyc_PP_text(
_tmp357);goto _LL285;_LL290: if(_tmp353 <= (void*)2)goto _LL292;if(*((int*)_tmp353)
!= 0)goto _LL292;_tmp358=((struct Cyc_Absyn_Var_p_struct*)_tmp353)->f1;_tmp359=((
struct Cyc_Absyn_Var_p_struct*)_tmp353)->f2;_tmp35A=*_tmp359;_tmp35B=(void*)
_tmp35A.r;if((int)_tmp35B != 0)goto _LL292;_LL291: s=Cyc_Absynpp_qvar2doc(_tmp358->name);
goto _LL285;_LL292: if(_tmp353 <= (void*)2)goto _LL294;if(*((int*)_tmp353)!= 0)goto
_LL294;_tmp35C=((struct Cyc_Absyn_Var_p_struct*)_tmp353)->f1;_tmp35D=((struct Cyc_Absyn_Var_p_struct*)
_tmp353)->f2;_LL293: s=({struct Cyc_PP_Doc*_tmp385[3];_tmp385[2]=Cyc_Absynpp_pat2doc(
_tmp35D);_tmp385[1]=Cyc_PP_text(({const char*_tmp386=" as ";_tag_dyneither(
_tmp386,sizeof(char),5);}));_tmp385[0]=Cyc_Absynpp_qvar2doc(_tmp35C->name);Cyc_PP_cat(
_tag_dyneither(_tmp385,sizeof(struct Cyc_PP_Doc*),3));});goto _LL285;_LL294: if(
_tmp353 <= (void*)2)goto _LL296;if(*((int*)_tmp353)!= 2)goto _LL296;_tmp35E=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp353)->f1;_tmp35F=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp353)->f2;_LL295: s=({struct Cyc_PP_Doc*_tmp387[4];_tmp387[3]=Cyc_PP_text(({
const char*_tmp389=">";_tag_dyneither(_tmp389,sizeof(char),2);}));_tmp387[2]=Cyc_PP_textptr(
Cyc_Absynpp_get_name(_tmp35E));_tmp387[1]=Cyc_PP_text(({const char*_tmp388="<";
_tag_dyneither(_tmp388,sizeof(char),2);}));_tmp387[0]=Cyc_Absynpp_qvar2doc(
_tmp35F->name);Cyc_PP_cat(_tag_dyneither(_tmp387,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL285;_LL296: if(_tmp353 <= (void*)2)goto _LL298;if(*((int*)_tmp353)!= 3)goto
_LL298;_tmp360=((struct Cyc_Absyn_Tuple_p_struct*)_tmp353)->f1;_tmp361=((struct
Cyc_Absyn_Tuple_p_struct*)_tmp353)->f2;_LL297: s=({struct Cyc_PP_Doc*_tmp38A[4];
_tmp38A[3]=_tmp361?Cyc_PP_text(({const char*_tmp38D=", ...)";_tag_dyneither(
_tmp38D,sizeof(char),7);})): Cyc_PP_text(({const char*_tmp38E=")";_tag_dyneither(
_tmp38E,sizeof(char),2);}));_tmp38A[2]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_pat2doc,({const char*_tmp38C=",";_tag_dyneither(_tmp38C,sizeof(char),
2);}),_tmp360);_tmp38A[1]=Cyc_PP_text(({const char*_tmp38B="(";_tag_dyneither(
_tmp38B,sizeof(char),2);}));_tmp38A[0]=Cyc_Absynpp_dollar();Cyc_PP_cat(
_tag_dyneither(_tmp38A,sizeof(struct Cyc_PP_Doc*),4));});goto _LL285;_LL298: if(
_tmp353 <= (void*)2)goto _LL29A;if(*((int*)_tmp353)!= 4)goto _LL29A;_tmp362=((
struct Cyc_Absyn_Pointer_p_struct*)_tmp353)->f1;_LL299: s=({struct Cyc_PP_Doc*
_tmp38F[2];_tmp38F[1]=Cyc_Absynpp_pat2doc(_tmp362);_tmp38F[0]=Cyc_PP_text(({
const char*_tmp390="&";_tag_dyneither(_tmp390,sizeof(char),2);}));Cyc_PP_cat(
_tag_dyneither(_tmp38F,sizeof(struct Cyc_PP_Doc*),2));});goto _LL285;_LL29A: if(
_tmp353 <= (void*)2)goto _LL29C;if(*((int*)_tmp353)!= 1)goto _LL29C;_tmp363=((
struct Cyc_Absyn_Reference_p_struct*)_tmp353)->f1;_tmp364=((struct Cyc_Absyn_Reference_p_struct*)
_tmp353)->f2;_tmp365=*_tmp364;_tmp366=(void*)_tmp365.r;if((int)_tmp366 != 0)goto
_LL29C;_LL29B: s=({struct Cyc_PP_Doc*_tmp391[2];_tmp391[1]=Cyc_Absynpp_qvar2doc(
_tmp363->name);_tmp391[0]=Cyc_PP_text(({const char*_tmp392="*";_tag_dyneither(
_tmp392,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp391,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL285;_LL29C: if(_tmp353 <= (void*)2)goto _LL29E;if(*((int*)_tmp353)!= 
1)goto _LL29E;_tmp367=((struct Cyc_Absyn_Reference_p_struct*)_tmp353)->f1;_tmp368=((
struct Cyc_Absyn_Reference_p_struct*)_tmp353)->f2;_LL29D: s=({struct Cyc_PP_Doc*
_tmp393[4];_tmp393[3]=Cyc_Absynpp_pat2doc(_tmp368);_tmp393[2]=Cyc_PP_text(({
const char*_tmp395=" as ";_tag_dyneither(_tmp395,sizeof(char),5);}));_tmp393[1]=
Cyc_Absynpp_qvar2doc(_tmp367->name);_tmp393[0]=Cyc_PP_text(({const char*_tmp394="*";
_tag_dyneither(_tmp394,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp393,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL285;_LL29E: if(_tmp353 <= (void*)2)goto
_LL2A0;if(*((int*)_tmp353)!= 12)goto _LL2A0;_tmp369=((struct Cyc_Absyn_UnknownId_p_struct*)
_tmp353)->f1;_LL29F: s=Cyc_Absynpp_qvar2doc(_tmp369);goto _LL285;_LL2A0: if(_tmp353
<= (void*)2)goto _LL2A2;if(*((int*)_tmp353)!= 13)goto _LL2A2;_tmp36A=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp353)->f1;_tmp36B=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp353)->f2;
_tmp36C=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp353)->f3;_LL2A1: {struct
_dyneither_ptr term=_tmp36C?({const char*_tmp399=", ...)";_tag_dyneither(_tmp399,
sizeof(char),7);}):({const char*_tmp39A=")";_tag_dyneither(_tmp39A,sizeof(char),2);});
s=({struct Cyc_PP_Doc*_tmp396[2];_tmp396[1]=Cyc_PP_group(({const char*_tmp397="(";
_tag_dyneither(_tmp397,sizeof(char),2);}),term,({const char*_tmp398=",";
_tag_dyneither(_tmp398,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp36B));_tmp396[0]=Cyc_Absynpp_qvar2doc(_tmp36A);Cyc_PP_cat(_tag_dyneither(
_tmp396,sizeof(struct Cyc_PP_Doc*),2));});goto _LL285;}_LL2A2: if(_tmp353 <= (void*)
2)goto _LL2A4;if(*((int*)_tmp353)!= 5)goto _LL2A4;_tmp36D=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp353)->f1;_tmp36E=_tmp36D.aggr_info;_tmp36F=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp353)->f2;_tmp370=((struct Cyc_Absyn_Aggr_p_struct*)_tmp353)->f3;_tmp371=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp353)->f4;_LL2A3: {struct _dyneither_ptr term=
_tmp371?({const char*_tmp3A3=", ...}";_tag_dyneither(_tmp3A3,sizeof(char),7);}):({
const char*_tmp3A4="}";_tag_dyneither(_tmp3A4,sizeof(char),2);});struct _tuple0*
_tmp39C;struct _tuple3 _tmp39B=Cyc_Absyn_aggr_kinded_name(_tmp36E);_tmp39C=_tmp39B.f2;
s=({struct Cyc_PP_Doc*_tmp39D[4];_tmp39D[3]=Cyc_PP_group(({const char*_tmp3A1="";
_tag_dyneither(_tmp3A1,sizeof(char),1);}),term,({const char*_tmp3A2=",";
_tag_dyneither(_tmp3A2,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp370));_tmp39D[2]=Cyc_PP_egroup(({const char*_tmp39E="[";_tag_dyneither(
_tmp39E,sizeof(char),2);}),({const char*_tmp39F="]";_tag_dyneither(_tmp39F,
sizeof(char),2);}),({const char*_tmp3A0=",";_tag_dyneither(_tmp3A0,sizeof(char),2);}),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
_tmp36F)));_tmp39D[1]=Cyc_Absynpp_lb();_tmp39D[0]=Cyc_Absynpp_qvar2doc(_tmp39C);
Cyc_PP_cat(_tag_dyneither(_tmp39D,sizeof(struct Cyc_PP_Doc*),4));});goto _LL285;}
_LL2A4: if(_tmp353 <= (void*)2)goto _LL2A6;if(*((int*)_tmp353)!= 10)goto _LL2A6;
_tmp372=((struct Cyc_Absyn_Enum_p_struct*)_tmp353)->f2;_LL2A5: _tmp373=_tmp372;
goto _LL2A7;_LL2A6: if(_tmp353 <= (void*)2)goto _LL2A8;if(*((int*)_tmp353)!= 11)goto
_LL2A8;_tmp373=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp353)->f2;_LL2A7: s=Cyc_Absynpp_qvar2doc(
_tmp373->name);goto _LL285;_LL2A8: if(_tmp353 <= (void*)2)goto _LL2AA;if(*((int*)
_tmp353)!= 6)goto _LL2AA;_tmp374=((struct Cyc_Absyn_Datatype_p_struct*)_tmp353)->f2;
_tmp375=((struct Cyc_Absyn_Datatype_p_struct*)_tmp353)->f3;if(_tmp375 != 0)goto
_LL2AA;_LL2A9: s=Cyc_Absynpp_qvar2doc(_tmp374->name);goto _LL285;_LL2AA: if(_tmp353
<= (void*)2)goto _LL2AC;if(*((int*)_tmp353)!= 6)goto _LL2AC;_tmp376=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp353)->f2;_tmp377=((struct Cyc_Absyn_Datatype_p_struct*)_tmp353)->f3;_tmp378=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp353)->f4;_LL2AB: {struct _dyneither_ptr
term=_tmp378?({const char*_tmp3A8=", ...)";_tag_dyneither(_tmp3A8,sizeof(char),7);}):({
const char*_tmp3A9=")";_tag_dyneither(_tmp3A9,sizeof(char),2);});s=({struct Cyc_PP_Doc*
_tmp3A5[2];_tmp3A5[1]=Cyc_PP_egroup(({const char*_tmp3A6="(";_tag_dyneither(
_tmp3A6,sizeof(char),2);}),term,({const char*_tmp3A7=",";_tag_dyneither(_tmp3A7,
sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp377));_tmp3A5[0]=
Cyc_Absynpp_qvar2doc(_tmp376->name);Cyc_PP_cat(_tag_dyneither(_tmp3A5,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL285;}_LL2AC: if(_tmp353 <= (void*)2)goto _LL285;
if(*((int*)_tmp353)!= 14)goto _LL285;_tmp379=((struct Cyc_Absyn_Exp_p_struct*)
_tmp353)->f1;_LL2AD: s=Cyc_Absynpp_exp2doc(_tmp379);goto _LL285;_LL285:;}return s;}
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*dp){return({struct Cyc_PP_Doc*
_tmp3AA[2];_tmp3AA[1]=Cyc_Absynpp_pat2doc((*dp).f2);_tmp3AA[0]=Cyc_PP_egroup(({
const char*_tmp3AB="";_tag_dyneither(_tmp3AB,sizeof(char),1);}),({const char*
_tmp3AC="=";_tag_dyneither(_tmp3AC,sizeof(char),2);}),({const char*_tmp3AD="=";
_tag_dyneither(_tmp3AD,sizeof(char),2);}),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));
Cyc_PP_cat(_tag_dyneither(_tmp3AA,sizeof(struct Cyc_PP_Doc*),2));});}struct Cyc_PP_Doc*
Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){if(c->where_clause
== 0  && (void*)(c->pattern)->r == (void*)((void*)0))return({struct Cyc_PP_Doc*
_tmp3AE[2];_tmp3AE[1]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3B0[2];_tmp3B0[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp3B0[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp3B0,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp3AE[0]=Cyc_PP_text(({const char*_tmp3AF="default: ";
_tag_dyneither(_tmp3AF,sizeof(char),10);}));Cyc_PP_cat(_tag_dyneither(_tmp3AE,
sizeof(struct Cyc_PP_Doc*),2));});else{if(c->where_clause == 0)return({struct Cyc_PP_Doc*
_tmp3B1[4];_tmp3B1[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3B4[2];_tmp3B4[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp3B4[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp3B4,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp3B1[2]=Cyc_PP_text(({const char*_tmp3B3=": ";
_tag_dyneither(_tmp3B3,sizeof(char),3);}));_tmp3B1[1]=Cyc_Absynpp_pat2doc(c->pattern);
_tmp3B1[0]=Cyc_PP_text(({const char*_tmp3B2="case ";_tag_dyneither(_tmp3B2,
sizeof(char),6);}));Cyc_PP_cat(_tag_dyneither(_tmp3B1,sizeof(struct Cyc_PP_Doc*),
4));});else{return({struct Cyc_PP_Doc*_tmp3B5[6];_tmp3B5[5]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp3B9[2];_tmp3B9[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp3B9[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp3B9,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp3B5[4]=Cyc_PP_text(({const char*_tmp3B8=": ";_tag_dyneither(_tmp3B8,sizeof(
char),3);}));_tmp3B5[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause));
_tmp3B5[2]=Cyc_PP_text(({const char*_tmp3B7=" && ";_tag_dyneither(_tmp3B7,sizeof(
char),5);}));_tmp3B5[1]=Cyc_Absynpp_pat2doc(c->pattern);_tmp3B5[0]=Cyc_PP_text(({
const char*_tmp3B6="case ";_tag_dyneither(_tmp3B6,sizeof(char),6);}));Cyc_PP_cat(
_tag_dyneither(_tmp3B5,sizeof(struct Cyc_PP_Doc*),6));});}}}struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,({const char*_tmp3BA="";
_tag_dyneither(_tmp3BA,sizeof(char),1);}),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);
else{return({struct Cyc_PP_Doc*_tmp3BB[3];_tmp3BB[2]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_check_null(f->tag));_tmp3BB[1]=Cyc_PP_text(({const char*_tmp3BC=" = ";
_tag_dyneither(_tmp3BC,sizeof(char),4);}));_tmp3BB[0]=Cyc_Absynpp_qvar2doc(f->name);
Cyc_PP_cat(_tag_dyneither(_tmp3BB,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,({const char*
_tmp3BD=",";_tag_dyneither(_tmp3BD,sizeof(char),2);}),fs);}static struct Cyc_PP_Doc*
Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return Cyc_Absynpp_qvar2doc(v->name);}
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct
_dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,({
const char*_tmp3BE=",";_tag_dyneither(_tmp3BE,sizeof(char),2);}),vds);}struct Cyc_PP_Doc*
Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl
_tmp3C0;void*_tmp3C1;struct _tuple0*_tmp3C2;struct Cyc_Absyn_Tqual _tmp3C3;void*
_tmp3C4;struct Cyc_Absyn_Exp*_tmp3C5;struct Cyc_List_List*_tmp3C6;struct Cyc_Absyn_Vardecl*
_tmp3BF=vd;_tmp3C0=*_tmp3BF;_tmp3C1=(void*)_tmp3C0.sc;_tmp3C2=_tmp3C0.name;
_tmp3C3=_tmp3C0.tq;_tmp3C4=(void*)_tmp3C0.type;_tmp3C5=_tmp3C0.initializer;
_tmp3C6=_tmp3C0.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(
_tmp3C2);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp3C6);struct Cyc_PP_Doc*
beforenamedoc;{void*_tmp3C7=Cyc_Cyclone_c_compiler;_LL2AF: if((int)_tmp3C7 != 0)
goto _LL2B1;_LL2B0: beforenamedoc=attsdoc;goto _LL2AE;_LL2B1: if((int)_tmp3C7 != 1)
goto _LL2AE;_LL2B2:{void*_tmp3C8=Cyc_Tcutil_compress(_tmp3C4);struct Cyc_Absyn_FnInfo
_tmp3C9;struct Cyc_List_List*_tmp3CA;_LL2B4: if(_tmp3C8 <= (void*)4)goto _LL2B6;if(*((
int*)_tmp3C8)!= 8)goto _LL2B6;_tmp3C9=((struct Cyc_Absyn_FnType_struct*)_tmp3C8)->f1;
_tmp3CA=_tmp3C9.attributes;_LL2B5: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp3CA);
goto _LL2B3;_LL2B6:;_LL2B7: beforenamedoc=Cyc_PP_nil_doc();goto _LL2B3;_LL2B3:;}
goto _LL2AE;_LL2AE:;}{struct Cyc_PP_Doc*tmp_doc;{void*_tmp3CB=Cyc_Cyclone_c_compiler;
_LL2B9: if((int)_tmp3CB != 0)goto _LL2BB;_LL2BA: tmp_doc=Cyc_PP_nil_doc();goto _LL2B8;
_LL2BB: if((int)_tmp3CB != 1)goto _LL2B8;_LL2BC: tmp_doc=attsdoc;goto _LL2B8;_LL2B8:;}
s=({struct Cyc_PP_Doc*_tmp3CC[5];_tmp3CC[4]=Cyc_PP_text(({const char*_tmp3D1=";";
_tag_dyneither(_tmp3D1,sizeof(char),2);}));_tmp3CC[3]=_tmp3C5 == 0?Cyc_PP_nil_doc():({
struct Cyc_PP_Doc*_tmp3CF[2];_tmp3CF[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp3C5);_tmp3CF[0]=Cyc_PP_text(({const char*_tmp3D0=" = ";_tag_dyneither(_tmp3D0,
sizeof(char),4);}));Cyc_PP_cat(_tag_dyneither(_tmp3CF,sizeof(struct Cyc_PP_Doc*),
2));});_tmp3CC[2]=Cyc_Absynpp_tqtd2doc(_tmp3C3,_tmp3C4,({struct Cyc_Core_Opt*
_tmp3CD=_cycalloc(sizeof(*_tmp3CD));_tmp3CD->v=({struct Cyc_PP_Doc*_tmp3CE[2];
_tmp3CE[1]=sn;_tmp3CE[0]=beforenamedoc;Cyc_PP_cat(_tag_dyneither(_tmp3CE,sizeof(
struct Cyc_PP_Doc*),2));});_tmp3CD;}));_tmp3CC[1]=Cyc_Absynpp_scope2doc(_tmp3C1);
_tmp3CC[0]=tmp_doc;Cyc_PP_cat(_tag_dyneither(_tmp3CC,sizeof(struct Cyc_PP_Doc*),5));});
return s;}}}struct _tuple11{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;}
;struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple11*x){struct _tuple0*_tmp3D3;
struct _tuple11 _tmp3D2=*x;_tmp3D3=_tmp3D2.f2;return Cyc_Absynpp_qvar2doc(_tmp3D3);}
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{
void*_tmp3D4=(void*)d->r;struct Cyc_Absyn_Fndecl*_tmp3D5;struct Cyc_Absyn_Aggrdecl*
_tmp3D6;struct Cyc_Absyn_Vardecl*_tmp3D7;struct Cyc_Absyn_Tvar*_tmp3D8;struct Cyc_Absyn_Vardecl*
_tmp3D9;int _tmp3DA;struct Cyc_Absyn_Exp*_tmp3DB;struct Cyc_Absyn_Exp*_tmp3DC;
struct Cyc_Absyn_Tvar*_tmp3DD;struct Cyc_Absyn_Vardecl*_tmp3DE;struct Cyc_Absyn_Datatypedecl*
_tmp3DF;struct Cyc_Absyn_Datatypedecl _tmp3E0;void*_tmp3E1;struct _tuple0*_tmp3E2;
struct Cyc_List_List*_tmp3E3;struct Cyc_Core_Opt*_tmp3E4;int _tmp3E5;int _tmp3E6;
struct Cyc_Absyn_Pat*_tmp3E7;struct Cyc_Absyn_Exp*_tmp3E8;struct Cyc_List_List*
_tmp3E9;struct Cyc_Absyn_Enumdecl*_tmp3EA;struct Cyc_Absyn_Enumdecl _tmp3EB;void*
_tmp3EC;struct _tuple0*_tmp3ED;struct Cyc_Core_Opt*_tmp3EE;struct Cyc_Absyn_Typedefdecl*
_tmp3EF;struct _dyneither_ptr*_tmp3F0;struct Cyc_List_List*_tmp3F1;struct _tuple0*
_tmp3F2;struct Cyc_List_List*_tmp3F3;struct Cyc_List_List*_tmp3F4;struct Cyc_List_List*
_tmp3F5;struct Cyc_List_List*_tmp3F6;_LL2BE: if(_tmp3D4 <= (void*)2)goto _LL2DA;if(*((
int*)_tmp3D4)!= 1)goto _LL2C0;_tmp3D5=((struct Cyc_Absyn_Fn_d_struct*)_tmp3D4)->f1;
_LL2BF: {void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp402=_cycalloc(sizeof(*
_tmp402));_tmp402[0]=({struct Cyc_Absyn_FnType_struct _tmp403;_tmp403.tag=8;
_tmp403.f1=({struct Cyc_Absyn_FnInfo _tmp404;_tmp404.tvars=_tmp3D5->tvs;_tmp404.effect=
_tmp3D5->effect;_tmp404.ret_typ=(void*)((void*)_tmp3D5->ret_type);_tmp404.args=((
struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp3D5->args);_tmp404.c_varargs=_tmp3D5->c_varargs;
_tmp404.cyc_varargs=_tmp3D5->cyc_varargs;_tmp404.rgn_po=_tmp3D5->rgn_po;_tmp404.attributes=
0;_tmp404;});_tmp403;});_tmp402;});struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp3D5->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp3D5->is_inline){void*
_tmp3F7=Cyc_Cyclone_c_compiler;_LL2DF: if((int)_tmp3F7 != 0)goto _LL2E1;_LL2E0:
inlinedoc=Cyc_PP_text(({const char*_tmp3F8="inline ";_tag_dyneither(_tmp3F8,
sizeof(char),8);}));goto _LL2DE;_LL2E1: if((int)_tmp3F7 != 1)goto _LL2DE;_LL2E2:
inlinedoc=Cyc_PP_text(({const char*_tmp3F9="__inline ";_tag_dyneither(_tmp3F9,
sizeof(char),10);}));goto _LL2DE;_LL2DE:;}else{inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc((void*)_tmp3D5->sc);struct Cyc_PP_Doc*
beforenamedoc;{void*_tmp3FA=Cyc_Cyclone_c_compiler;_LL2E4: if((int)_tmp3FA != 0)
goto _LL2E6;_LL2E5: beforenamedoc=attsdoc;goto _LL2E3;_LL2E6: if((int)_tmp3FA != 1)
goto _LL2E3;_LL2E7: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp3D5->attributes);
goto _LL2E3;_LL2E3:;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp3D5->name);struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(
0),t,({struct Cyc_Core_Opt*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->v=({
struct Cyc_PP_Doc*_tmp401[2];_tmp401[1]=namedoc;_tmp401[0]=beforenamedoc;Cyc_PP_cat(
_tag_dyneither(_tmp401,sizeof(struct Cyc_PP_Doc*),2));});_tmp400;}));struct Cyc_PP_Doc*
bodydoc=({struct Cyc_PP_Doc*_tmp3FE[5];_tmp3FE[4]=Cyc_Absynpp_rb();_tmp3FE[3]=Cyc_PP_line_doc();
_tmp3FE[2]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp3FF[2];_tmp3FF[1]=Cyc_Absynpp_stmt2doc(
_tmp3D5->body);_tmp3FF[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp3FF,
sizeof(struct Cyc_PP_Doc*),2));}));_tmp3FE[1]=Cyc_Absynpp_lb();_tmp3FE[0]=Cyc_PP_blank_doc();
Cyc_PP_cat(_tag_dyneither(_tmp3FE,sizeof(struct Cyc_PP_Doc*),5));});s=({struct Cyc_PP_Doc*
_tmp3FB[4];_tmp3FB[3]=bodydoc;_tmp3FB[2]=tqtddoc;_tmp3FB[1]=scopedoc;_tmp3FB[0]=
inlinedoc;Cyc_PP_cat(_tag_dyneither(_tmp3FB,sizeof(struct Cyc_PP_Doc*),4));});{
void*_tmp3FC=Cyc_Cyclone_c_compiler;_LL2E9: if((int)_tmp3FC != 1)goto _LL2EB;_LL2EA:
s=({struct Cyc_PP_Doc*_tmp3FD[2];_tmp3FD[1]=s;_tmp3FD[0]=attsdoc;Cyc_PP_cat(
_tag_dyneither(_tmp3FD,sizeof(struct Cyc_PP_Doc*),2));});goto _LL2E8;_LL2EB:;
_LL2EC: goto _LL2E8;_LL2E8:;}goto _LL2BD;}}}_LL2C0: if(*((int*)_tmp3D4)!= 6)goto
_LL2C2;_tmp3D6=((struct Cyc_Absyn_Aggr_d_struct*)_tmp3D4)->f1;_LL2C1: if(_tmp3D6->impl
== 0)s=({struct Cyc_PP_Doc*_tmp405[5];_tmp405[4]=Cyc_PP_text(({const char*_tmp406=";";
_tag_dyneither(_tmp406,sizeof(char),2);}));_tmp405[3]=Cyc_Absynpp_ktvars2doc(
_tmp3D6->tvs);_tmp405[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3D6->name);_tmp405[1]=Cyc_Absynpp_aggr_kind2doc((
void*)_tmp3D6->kind);_tmp405[0]=Cyc_Absynpp_scope2doc((void*)_tmp3D6->sc);Cyc_PP_cat(
_tag_dyneither(_tmp405,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*
_tmp407[12];_tmp407[11]=Cyc_PP_text(({const char*_tmp40B=";";_tag_dyneither(
_tmp40B,sizeof(char),2);}));_tmp407[10]=Cyc_Absynpp_atts2doc(_tmp3D6->attributes);
_tmp407[9]=Cyc_Absynpp_rb();_tmp407[8]=Cyc_PP_line_doc();_tmp407[7]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp40A[2];_tmp40A[1]=Cyc_Absynpp_aggrfields2doc(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3D6->impl))->fields);_tmp40A[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_dyneither(_tmp40A,sizeof(struct Cyc_PP_Doc*),2));}));_tmp407[6]=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3D6->impl))->rgn_po == 0?Cyc_PP_nil_doc():({
struct Cyc_PP_Doc*_tmp408[2];_tmp408[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3D6->impl))->rgn_po);_tmp408[0]=Cyc_PP_text(({const char*_tmp409=":";
_tag_dyneither(_tmp409,sizeof(char),2);}));Cyc_PP_cat(_tag_dyneither(_tmp408,
sizeof(struct Cyc_PP_Doc*),2));});_tmp407[5]=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3D6->impl))->exist_vars);_tmp407[4]=Cyc_Absynpp_lb();_tmp407[3]=
Cyc_PP_blank_doc();_tmp407[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3D6->name);_tmp407[1]=
Cyc_Absynpp_aggr_kind2doc((void*)_tmp3D6->kind);_tmp407[0]=Cyc_Absynpp_scope2doc((
void*)_tmp3D6->sc);Cyc_PP_cat(_tag_dyneither(_tmp407,sizeof(struct Cyc_PP_Doc*),
12));});}goto _LL2BD;_LL2C2: if(*((int*)_tmp3D4)!= 0)goto _LL2C4;_tmp3D7=((struct
Cyc_Absyn_Var_d_struct*)_tmp3D4)->f1;_LL2C3: s=Cyc_Absynpp_vardecl2doc(_tmp3D7);
goto _LL2BD;_LL2C4: if(*((int*)_tmp3D4)!= 4)goto _LL2C6;_tmp3D8=((struct Cyc_Absyn_Region_d_struct*)
_tmp3D4)->f1;_tmp3D9=((struct Cyc_Absyn_Region_d_struct*)_tmp3D4)->f2;_tmp3DA=((
struct Cyc_Absyn_Region_d_struct*)_tmp3D4)->f3;_tmp3DB=((struct Cyc_Absyn_Region_d_struct*)
_tmp3D4)->f4;_LL2C5: s=({struct Cyc_PP_Doc*_tmp40C[8];_tmp40C[7]=Cyc_PP_text(({
const char*_tmp414=";";_tag_dyneither(_tmp414,sizeof(char),2);}));_tmp40C[6]=
_tmp3DB != 0?({struct Cyc_PP_Doc*_tmp411[3];_tmp411[2]=Cyc_PP_text(({const char*
_tmp413=")";_tag_dyneither(_tmp413,sizeof(char),2);}));_tmp411[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp3DB);_tmp411[0]=Cyc_PP_text(({const char*_tmp412=" = open(";
_tag_dyneither(_tmp412,sizeof(char),9);}));Cyc_PP_cat(_tag_dyneither(_tmp411,
sizeof(struct Cyc_PP_Doc*),3));}): Cyc_PP_nil_doc();_tmp40C[5]=_tmp3DA?Cyc_PP_text(({
const char*_tmp410=" @resetable";_tag_dyneither(_tmp410,sizeof(char),12);})): Cyc_PP_nil_doc();
_tmp40C[4]=Cyc_Absynpp_qvar2doc(_tmp3D9->name);_tmp40C[3]=Cyc_PP_text(({const
char*_tmp40F=">";_tag_dyneither(_tmp40F,sizeof(char),2);}));_tmp40C[2]=Cyc_PP_textptr(
Cyc_Absynpp_get_name(_tmp3D8));_tmp40C[1]=Cyc_PP_text(({const char*_tmp40E="<";
_tag_dyneither(_tmp40E,sizeof(char),2);}));_tmp40C[0]=Cyc_PP_text(({const char*
_tmp40D="region";_tag_dyneither(_tmp40D,sizeof(char),7);}));Cyc_PP_cat(
_tag_dyneither(_tmp40C,sizeof(struct Cyc_PP_Doc*),8));});goto _LL2BD;_LL2C6: if(*((
int*)_tmp3D4)!= 5)goto _LL2C8;_tmp3DC=((struct Cyc_Absyn_Alias_d_struct*)_tmp3D4)->f1;
_tmp3DD=((struct Cyc_Absyn_Alias_d_struct*)_tmp3D4)->f2;_tmp3DE=((struct Cyc_Absyn_Alias_d_struct*)
_tmp3D4)->f3;_LL2C7: s=({struct Cyc_PP_Doc*_tmp415[8];_tmp415[7]=Cyc_PP_text(({
const char*_tmp41A=";";_tag_dyneither(_tmp41A,sizeof(char),2);}));_tmp415[6]=Cyc_Absynpp_exp2doc(
_tmp3DC);_tmp415[5]=Cyc_PP_text(({const char*_tmp419=" = ";_tag_dyneither(_tmp419,
sizeof(char),4);}));_tmp415[4]=Cyc_Absynpp_qvar2doc(_tmp3DE->name);_tmp415[3]=
Cyc_PP_text(({const char*_tmp418="> ";_tag_dyneither(_tmp418,sizeof(char),3);}));
_tmp415[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp3DD));_tmp415[1]=Cyc_PP_text(({
const char*_tmp417=" <";_tag_dyneither(_tmp417,sizeof(char),3);}));_tmp415[0]=Cyc_PP_text(({
const char*_tmp416="alias";_tag_dyneither(_tmp416,sizeof(char),6);}));Cyc_PP_cat(
_tag_dyneither(_tmp415,sizeof(struct Cyc_PP_Doc*),8));});goto _LL2BD;_LL2C8: if(*((
int*)_tmp3D4)!= 7)goto _LL2CA;_tmp3DF=((struct Cyc_Absyn_Datatype_d_struct*)
_tmp3D4)->f1;_tmp3E0=*_tmp3DF;_tmp3E1=(void*)_tmp3E0.sc;_tmp3E2=_tmp3E0.name;
_tmp3E3=_tmp3E0.tvs;_tmp3E4=_tmp3E0.fields;_tmp3E5=_tmp3E0.is_extensible;_tmp3E6=
_tmp3E0.is_flat;_LL2C9: if(_tmp3E4 == 0)s=({struct Cyc_PP_Doc*_tmp41B[7];_tmp41B[6]=
Cyc_PP_text(({const char*_tmp41F=";";_tag_dyneither(_tmp41F,sizeof(char),2);}));
_tmp41B[5]=Cyc_Absynpp_ktvars2doc(_tmp3E3);_tmp41B[4]=_tmp3E5?Cyc_Absynpp_qvar2bolddoc(
_tmp3E2): Cyc_Absynpp_typedef_name2bolddoc(_tmp3E2);_tmp41B[3]=_tmp3E6?Cyc_PP_text(({
const char*_tmp41E="__attribute__((flat)) ";_tag_dyneither(_tmp41E,sizeof(char),
23);})): Cyc_PP_blank_doc();_tmp41B[2]=Cyc_PP_text(({const char*_tmp41D="datatype ";
_tag_dyneither(_tmp41D,sizeof(char),10);}));_tmp41B[1]=_tmp3E5?Cyc_PP_text(({
const char*_tmp41C="@extensible";_tag_dyneither(_tmp41C,sizeof(char),12);})): Cyc_PP_blank_doc();
_tmp41B[0]=Cyc_Absynpp_scope2doc(_tmp3E1);Cyc_PP_cat(_tag_dyneither(_tmp41B,
sizeof(struct Cyc_PP_Doc*),7));});else{s=({struct Cyc_PP_Doc*_tmp420[12];_tmp420[
11]=Cyc_PP_text(({const char*_tmp425=";";_tag_dyneither(_tmp425,sizeof(char),2);}));
_tmp420[10]=Cyc_Absynpp_rb();_tmp420[9]=Cyc_PP_line_doc();_tmp420[8]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp424[2];_tmp424[1]=Cyc_Absynpp_datatypefields2doc((
struct Cyc_List_List*)_tmp3E4->v);_tmp424[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_dyneither(_tmp424,sizeof(struct Cyc_PP_Doc*),2));}));_tmp420[7]=Cyc_Absynpp_lb();
_tmp420[6]=Cyc_PP_blank_doc();_tmp420[5]=Cyc_Absynpp_ktvars2doc(_tmp3E3);_tmp420[
4]=_tmp3E5?Cyc_Absynpp_qvar2bolddoc(_tmp3E2): Cyc_Absynpp_typedef_name2bolddoc(
_tmp3E2);_tmp420[3]=Cyc_PP_text(({const char*_tmp423="datatype ";_tag_dyneither(
_tmp423,sizeof(char),10);}));_tmp420[2]=_tmp3E6?Cyc_PP_text(({const char*_tmp422="__attribute__((flat)) ";
_tag_dyneither(_tmp422,sizeof(char),23);})): Cyc_PP_blank_doc();_tmp420[1]=
_tmp3E5?Cyc_PP_text(({const char*_tmp421="@extensible ";_tag_dyneither(_tmp421,
sizeof(char),13);})): Cyc_PP_blank_doc();_tmp420[0]=Cyc_Absynpp_scope2doc(_tmp3E1);
Cyc_PP_cat(_tag_dyneither(_tmp420,sizeof(struct Cyc_PP_Doc*),12));});}goto _LL2BD;
_LL2CA: if(*((int*)_tmp3D4)!= 2)goto _LL2CC;_tmp3E7=((struct Cyc_Absyn_Let_d_struct*)
_tmp3D4)->f1;_tmp3E8=((struct Cyc_Absyn_Let_d_struct*)_tmp3D4)->f3;_LL2CB: s=({
struct Cyc_PP_Doc*_tmp426[5];_tmp426[4]=Cyc_PP_text(({const char*_tmp429=";";
_tag_dyneither(_tmp429,sizeof(char),2);}));_tmp426[3]=Cyc_Absynpp_exp2doc(
_tmp3E8);_tmp426[2]=Cyc_PP_text(({const char*_tmp428=" = ";_tag_dyneither(_tmp428,
sizeof(char),4);}));_tmp426[1]=Cyc_Absynpp_pat2doc(_tmp3E7);_tmp426[0]=Cyc_PP_text(({
const char*_tmp427="let ";_tag_dyneither(_tmp427,sizeof(char),5);}));Cyc_PP_cat(
_tag_dyneither(_tmp426,sizeof(struct Cyc_PP_Doc*),5));});goto _LL2BD;_LL2CC: if(*((
int*)_tmp3D4)!= 3)goto _LL2CE;_tmp3E9=((struct Cyc_Absyn_Letv_d_struct*)_tmp3D4)->f1;
_LL2CD: s=({struct Cyc_PP_Doc*_tmp42A[3];_tmp42A[2]=Cyc_PP_text(({const char*
_tmp42C=";";_tag_dyneither(_tmp42C,sizeof(char),2);}));_tmp42A[1]=Cyc_Absynpp_ids2doc(
_tmp3E9);_tmp42A[0]=Cyc_PP_text(({const char*_tmp42B="let ";_tag_dyneither(
_tmp42B,sizeof(char),5);}));Cyc_PP_cat(_tag_dyneither(_tmp42A,sizeof(struct Cyc_PP_Doc*),
3));});goto _LL2BD;_LL2CE: if(*((int*)_tmp3D4)!= 8)goto _LL2D0;_tmp3EA=((struct Cyc_Absyn_Enum_d_struct*)
_tmp3D4)->f1;_tmp3EB=*_tmp3EA;_tmp3EC=(void*)_tmp3EB.sc;_tmp3ED=_tmp3EB.name;
_tmp3EE=_tmp3EB.fields;_LL2CF: if(_tmp3EE == 0)s=({struct Cyc_PP_Doc*_tmp42D[4];
_tmp42D[3]=Cyc_PP_text(({const char*_tmp42F=";";_tag_dyneither(_tmp42F,sizeof(
char),2);}));_tmp42D[2]=Cyc_Absynpp_typedef_name2bolddoc(_tmp3ED);_tmp42D[1]=Cyc_PP_text(({
const char*_tmp42E="enum ";_tag_dyneither(_tmp42E,sizeof(char),6);}));_tmp42D[0]=
Cyc_Absynpp_scope2doc(_tmp3EC);Cyc_PP_cat(_tag_dyneither(_tmp42D,sizeof(struct
Cyc_PP_Doc*),4));});else{s=({struct Cyc_PP_Doc*_tmp430[9];_tmp430[8]=Cyc_PP_text(({
const char*_tmp433=";";_tag_dyneither(_tmp433,sizeof(char),2);}));_tmp430[7]=Cyc_Absynpp_rb();
_tmp430[6]=Cyc_PP_line_doc();_tmp430[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp432[
2];_tmp432[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp3EE->v);
_tmp432[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_dyneither(_tmp432,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp430[4]=Cyc_Absynpp_lb();_tmp430[3]=Cyc_PP_blank_doc();_tmp430[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp3ED);_tmp430[1]=Cyc_PP_text(({const char*_tmp431="enum ";_tag_dyneither(
_tmp431,sizeof(char),6);}));_tmp430[0]=Cyc_Absynpp_scope2doc(_tmp3EC);Cyc_PP_cat(
_tag_dyneither(_tmp430,sizeof(struct Cyc_PP_Doc*),9));});}goto _LL2BD;_LL2D0: if(*((
int*)_tmp3D4)!= 9)goto _LL2D2;_tmp3EF=((struct Cyc_Absyn_Typedef_d_struct*)_tmp3D4)->f1;
_LL2D1: {void*t;if(_tmp3EF->defn != 0)t=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3EF->defn))->v;else{t=Cyc_Absyn_new_evar(_tmp3EF->kind,0);}s=({struct Cyc_PP_Doc*
_tmp434[4];_tmp434[3]=Cyc_PP_text(({const char*_tmp438=";";_tag_dyneither(_tmp438,
sizeof(char),2);}));_tmp434[2]=Cyc_Absynpp_atts2doc(_tmp3EF->atts);_tmp434[1]=
Cyc_Absynpp_tqtd2doc(_tmp3EF->tq,t,({struct Cyc_Core_Opt*_tmp436=_cycalloc(
sizeof(*_tmp436));_tmp436->v=({struct Cyc_PP_Doc*_tmp437[2];_tmp437[1]=Cyc_Absynpp_tvars2doc(
_tmp3EF->tvs);_tmp437[0]=Cyc_Absynpp_typedef_name2bolddoc(_tmp3EF->name);Cyc_PP_cat(
_tag_dyneither(_tmp437,sizeof(struct Cyc_PP_Doc*),2));});_tmp436;}));_tmp434[0]=
Cyc_PP_text(({const char*_tmp435="typedef ";_tag_dyneither(_tmp435,sizeof(char),9);}));
Cyc_PP_cat(_tag_dyneither(_tmp434,sizeof(struct Cyc_PP_Doc*),4));});goto _LL2BD;}
_LL2D2: if(*((int*)_tmp3D4)!= 10)goto _LL2D4;_tmp3F0=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp3D4)->f1;_tmp3F1=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3D4)->f2;_LL2D3:
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp3F0);s=({
struct Cyc_PP_Doc*_tmp439[8];_tmp439[7]=Cyc_Absynpp_rb();_tmp439[6]=Cyc_PP_line_doc();
_tmp439[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp43B="";_tag_dyneither(_tmp43B,sizeof(char),1);}),_tmp3F1);_tmp439[
4]=Cyc_PP_line_doc();_tmp439[3]=Cyc_Absynpp_lb();_tmp439[2]=Cyc_PP_blank_doc();
_tmp439[1]=Cyc_PP_textptr(_tmp3F0);_tmp439[0]=Cyc_PP_text(({const char*_tmp43A="namespace ";
_tag_dyneither(_tmp43A,sizeof(char),11);}));Cyc_PP_cat(_tag_dyneither(_tmp439,
sizeof(struct Cyc_PP_Doc*),8));});if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL2BD;_LL2D4: if(*((int*)_tmp3D4)!= 11)goto _LL2D6;_tmp3F2=((struct Cyc_Absyn_Using_d_struct*)
_tmp3D4)->f1;_tmp3F3=((struct Cyc_Absyn_Using_d_struct*)_tmp3D4)->f2;_LL2D5: if(
Cyc_Absynpp_print_using_stmts)s=({struct Cyc_PP_Doc*_tmp43C[8];_tmp43C[7]=Cyc_Absynpp_rb();
_tmp43C[6]=Cyc_PP_line_doc();_tmp43C[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp43E="";_tag_dyneither(_tmp43E,sizeof(char),
1);}),_tmp3F3);_tmp43C[4]=Cyc_PP_line_doc();_tmp43C[3]=Cyc_Absynpp_lb();_tmp43C[
2]=Cyc_PP_blank_doc();_tmp43C[1]=Cyc_Absynpp_qvar2doc(_tmp3F2);_tmp43C[0]=Cyc_PP_text(({
const char*_tmp43D="using ";_tag_dyneither(_tmp43D,sizeof(char),7);}));Cyc_PP_cat(
_tag_dyneither(_tmp43C,sizeof(struct Cyc_PP_Doc*),8));});else{s=({struct Cyc_PP_Doc*
_tmp43F[11];_tmp43F[10]=Cyc_PP_text(({const char*_tmp444=" */";_tag_dyneither(
_tmp444,sizeof(char),4);}));_tmp43F[9]=Cyc_Absynpp_rb();_tmp43F[8]=Cyc_PP_text(({
const char*_tmp443="/* ";_tag_dyneither(_tmp443,sizeof(char),4);}));_tmp43F[7]=
Cyc_PP_line_doc();_tmp43F[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp442="";_tag_dyneither(_tmp442,sizeof(char),
1);}),_tmp3F3);_tmp43F[5]=Cyc_PP_line_doc();_tmp43F[4]=Cyc_PP_text(({const char*
_tmp441=" */";_tag_dyneither(_tmp441,sizeof(char),4);}));_tmp43F[3]=Cyc_Absynpp_lb();
_tmp43F[2]=Cyc_PP_blank_doc();_tmp43F[1]=Cyc_Absynpp_qvar2doc(_tmp3F2);_tmp43F[0]=
Cyc_PP_text(({const char*_tmp440="/* using ";_tag_dyneither(_tmp440,sizeof(char),
10);}));Cyc_PP_cat(_tag_dyneither(_tmp43F,sizeof(struct Cyc_PP_Doc*),11));});}
goto _LL2BD;_LL2D6: if(*((int*)_tmp3D4)!= 12)goto _LL2D8;_tmp3F4=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp3D4)->f1;_LL2D7: if(Cyc_Absynpp_print_externC_stmts)s=({struct Cyc_PP_Doc*
_tmp445[6];_tmp445[5]=Cyc_Absynpp_rb();_tmp445[4]=Cyc_PP_line_doc();_tmp445[3]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp447="";_tag_dyneither(_tmp447,sizeof(char),1);}),_tmp3F4);_tmp445[
2]=Cyc_PP_line_doc();_tmp445[1]=Cyc_Absynpp_lb();_tmp445[0]=Cyc_PP_text(({const
char*_tmp446="extern \"C\" ";_tag_dyneither(_tmp446,sizeof(char),12);}));Cyc_PP_cat(
_tag_dyneither(_tmp445,sizeof(struct Cyc_PP_Doc*),6));});else{s=({struct Cyc_PP_Doc*
_tmp448[9];_tmp448[8]=Cyc_PP_text(({const char*_tmp44D=" */";_tag_dyneither(
_tmp44D,sizeof(char),4);}));_tmp448[7]=Cyc_Absynpp_rb();_tmp448[6]=Cyc_PP_text(({
const char*_tmp44C="/* ";_tag_dyneither(_tmp44C,sizeof(char),4);}));_tmp448[5]=
Cyc_PP_line_doc();_tmp448[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp44B="";_tag_dyneither(_tmp44B,sizeof(char),
1);}),_tmp3F4);_tmp448[3]=Cyc_PP_line_doc();_tmp448[2]=Cyc_PP_text(({const char*
_tmp44A=" */";_tag_dyneither(_tmp44A,sizeof(char),4);}));_tmp448[1]=Cyc_Absynpp_lb();
_tmp448[0]=Cyc_PP_text(({const char*_tmp449="/* extern \"C\" ";_tag_dyneither(
_tmp449,sizeof(char),15);}));Cyc_PP_cat(_tag_dyneither(_tmp448,sizeof(struct Cyc_PP_Doc*),
9));});}goto _LL2BD;_LL2D8: if(*((int*)_tmp3D4)!= 13)goto _LL2DA;_tmp3F5=((struct
Cyc_Absyn_ExternCinclude_d_struct*)_tmp3D4)->f1;_tmp3F6=((struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp3D4)->f2;_LL2D9: if(Cyc_Absynpp_print_externC_stmts){struct Cyc_PP_Doc*exs_doc;
if(_tmp3F6 != 0)exs_doc=({struct Cyc_PP_Doc*_tmp44E[7];_tmp44E[6]=Cyc_Absynpp_rb();
_tmp44E[5]=Cyc_PP_line_doc();_tmp44E[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct _tuple11*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_export2doc,({const char*_tmp450=",";_tag_dyneither(_tmp450,sizeof(
char),2);}),_tmp3F6);_tmp44E[3]=Cyc_PP_line_doc();_tmp44E[2]=Cyc_Absynpp_lb();
_tmp44E[1]=Cyc_PP_text(({const char*_tmp44F=" export ";_tag_dyneither(_tmp44F,
sizeof(char),9);}));_tmp44E[0]=Cyc_Absynpp_rb();Cyc_PP_cat(_tag_dyneither(
_tmp44E,sizeof(struct Cyc_PP_Doc*),7));});else{exs_doc=Cyc_Absynpp_rb();}s=({
struct Cyc_PP_Doc*_tmp451[6];_tmp451[5]=exs_doc;_tmp451[4]=Cyc_PP_line_doc();
_tmp451[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,({
const char*_tmp453="";_tag_dyneither(_tmp453,sizeof(char),1);}),_tmp3F5);_tmp451[
2]=Cyc_PP_line_doc();_tmp451[1]=Cyc_Absynpp_lb();_tmp451[0]=Cyc_PP_text(({const
char*_tmp452="extern \"C include\" ";_tag_dyneither(_tmp452,sizeof(char),20);}));
Cyc_PP_cat(_tag_dyneither(_tmp451,sizeof(struct Cyc_PP_Doc*),6));});}else{s=({
struct Cyc_PP_Doc*_tmp454[9];_tmp454[8]=Cyc_PP_text(({const char*_tmp459=" */";
_tag_dyneither(_tmp459,sizeof(char),4);}));_tmp454[7]=Cyc_Absynpp_rb();_tmp454[6]=
Cyc_PP_text(({const char*_tmp458="/* ";_tag_dyneither(_tmp458,sizeof(char),4);}));
_tmp454[5]=Cyc_PP_line_doc();_tmp454[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,({const char*_tmp457="";_tag_dyneither(_tmp457,sizeof(char),
1);}),_tmp3F5);_tmp454[3]=Cyc_PP_line_doc();_tmp454[2]=Cyc_PP_text(({const char*
_tmp456=" */";_tag_dyneither(_tmp456,sizeof(char),4);}));_tmp454[1]=Cyc_Absynpp_lb();
_tmp454[0]=Cyc_PP_text(({const char*_tmp455="/* extern \"C include\" ";
_tag_dyneither(_tmp455,sizeof(char),23);}));Cyc_PP_cat(_tag_dyneither(_tmp454,
sizeof(struct Cyc_PP_Doc*),9));});}goto _LL2BD;_LL2DA: if((int)_tmp3D4 != 0)goto
_LL2DC;_LL2DB: s=({struct Cyc_PP_Doc*_tmp45A[2];_tmp45A[1]=Cyc_Absynpp_lb();
_tmp45A[0]=Cyc_PP_text(({const char*_tmp45B="__cyclone_port_on__;";_tag_dyneither(
_tmp45B,sizeof(char),21);}));Cyc_PP_cat(_tag_dyneither(_tmp45A,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL2BD;_LL2DC: if((int)_tmp3D4 != 1)goto _LL2BD;_LL2DD: s=({struct Cyc_PP_Doc*
_tmp45C[2];_tmp45C[1]=Cyc_Absynpp_lb();_tmp45C[0]=Cyc_PP_text(({const char*
_tmp45D="__cyclone_port_off__;";_tag_dyneither(_tmp45D,sizeof(char),22);}));Cyc_PP_cat(
_tag_dyneither(_tmp45C,sizeof(struct Cyc_PP_Doc*),2));});goto _LL2BD;_LL2BD:;}
return s;}int Cyc_Absynpp_print_scopes=1;struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
void*sc){if(!Cyc_Absynpp_print_scopes)return Cyc_PP_nil_doc();{void*_tmp45E=sc;
_LL2EE: if((int)_tmp45E != 0)goto _LL2F0;_LL2EF: return Cyc_PP_text(({const char*
_tmp45F="static ";_tag_dyneither(_tmp45F,sizeof(char),8);}));_LL2F0: if((int)
_tmp45E != 2)goto _LL2F2;_LL2F1: return Cyc_PP_nil_doc();_LL2F2: if((int)_tmp45E != 3)
goto _LL2F4;_LL2F3: return Cyc_PP_text(({const char*_tmp460="extern ";_tag_dyneither(
_tmp460,sizeof(char),8);}));_LL2F4: if((int)_tmp45E != 4)goto _LL2F6;_LL2F5: return
Cyc_PP_text(({const char*_tmp461="extern \"C\" ";_tag_dyneither(_tmp461,sizeof(
char),12);}));_LL2F6: if((int)_tmp45E != 1)goto _LL2F8;_LL2F7: return Cyc_PP_text(({
const char*_tmp462="abstract ";_tag_dyneither(_tmp462,sizeof(char),10);}));_LL2F8:
if((int)_tmp45E != 5)goto _LL2ED;_LL2F9: return Cyc_PP_text(({const char*_tmp463="register ";
_tag_dyneither(_tmp463,sizeof(char),10);}));_LL2ED:;}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp464=t;struct Cyc_Absyn_Tvar*_tmp465;struct Cyc_List_List*_tmp466;
_LL2FB: if(_tmp464 <= (void*)4)goto _LL2FF;if(*((int*)_tmp464)!= 1)goto _LL2FD;
_tmp465=((struct Cyc_Absyn_VarType_struct*)_tmp464)->f1;_LL2FC: return Cyc_Tcutil_is_temp_tvar(
_tmp465);_LL2FD: if(*((int*)_tmp464)!= 20)goto _LL2FF;_tmp466=((struct Cyc_Absyn_JoinEff_struct*)
_tmp464)->f1;_LL2FE: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp466);_LL2FF:;_LL300: return 0;_LL2FA:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp467=t;void**_tmp468;void*_tmp469;_LL302: if(_tmp467 <= (void*)4)goto
_LL308;if(*((int*)_tmp467)!= 11)goto _LL304;_LL303: return 1;_LL304: if(*((int*)
_tmp467)!= 13)goto _LL306;_LL305: return 1;_LL306: if(*((int*)_tmp467)!= 16)goto
_LL308;_tmp468=((struct Cyc_Absyn_TypedefType_struct*)_tmp467)->f4;if(_tmp468 == 0)
goto _LL308;_tmp469=*_tmp468;_LL307: return Cyc_Absynpp_is_anon_aggrtype(_tmp469);
_LL308:;_LL309: return 0;_LL301:;}static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 
0){struct _tuple5 _tmp46B=({struct _tuple5 _tmp46A;_tmp46A.f1=(void*)tms->hd;_tmp46A.f2=(
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_tmp46A;});void*_tmp46C;
void*_tmp46D;_LL30B: _tmp46C=_tmp46B.f1;if(*((int*)_tmp46C)!= 2)goto _LL30D;
_tmp46D=_tmp46B.f2;if(*((int*)_tmp46D)!= 3)goto _LL30D;_LL30C: return({struct Cyc_List_List*
_tmp46E=_region_malloc(r,sizeof(*_tmp46E));_tmp46E->hd=(void*)((void*)tms->hd);
_tmp46E->tl=({struct Cyc_List_List*_tmp46F=_region_malloc(r,sizeof(*_tmp46F));
_tmp46F->hd=(void*)((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd);
_tmp46F->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)
_check_null(tms->tl))->tl);_tmp46F;});_tmp46E;});_LL30D:;_LL30E: return({struct
Cyc_List_List*_tmp470=_region_malloc(r,sizeof(*_tmp470));_tmp470->hd=(void*)atts;
_tmp470->tl=tms;_tmp470;});_LL30A:;}else{return({struct Cyc_List_List*_tmp471=
_region_malloc(r,sizeof(*_tmp471));_tmp471->hd=(void*)atts;_tmp471->tl=tms;
_tmp471;});}}struct _tuple7 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t){void*_tmp472=t;struct Cyc_Absyn_ArrayInfo _tmp473;void*_tmp474;struct
Cyc_Absyn_Tqual _tmp475;struct Cyc_Absyn_Exp*_tmp476;struct Cyc_Absyn_Conref*
_tmp477;struct Cyc_Position_Segment*_tmp478;struct Cyc_Absyn_PtrInfo _tmp479;void*
_tmp47A;struct Cyc_Absyn_Tqual _tmp47B;struct Cyc_Absyn_PtrAtts _tmp47C;struct Cyc_Absyn_FnInfo
_tmp47D;struct Cyc_List_List*_tmp47E;struct Cyc_Core_Opt*_tmp47F;void*_tmp480;
struct Cyc_List_List*_tmp481;int _tmp482;struct Cyc_Absyn_VarargInfo*_tmp483;struct
Cyc_List_List*_tmp484;struct Cyc_List_List*_tmp485;struct Cyc_Core_Opt*_tmp486;
struct Cyc_Core_Opt*_tmp487;int _tmp488;struct _tuple0*_tmp489;struct Cyc_List_List*
_tmp48A;void**_tmp48B;_LL310: if(_tmp472 <= (void*)4)goto _LL31A;if(*((int*)_tmp472)
!= 7)goto _LL312;_tmp473=((struct Cyc_Absyn_ArrayType_struct*)_tmp472)->f1;_tmp474=(
void*)_tmp473.elt_type;_tmp475=_tmp473.tq;_tmp476=_tmp473.num_elts;_tmp477=
_tmp473.zero_term;_tmp478=_tmp473.zt_loc;_LL311: {struct Cyc_Absyn_Tqual _tmp48D;
void*_tmp48E;struct Cyc_List_List*_tmp48F;struct _tuple7 _tmp48C=Cyc_Absynpp_to_tms(
r,_tmp475,_tmp474);_tmp48D=_tmp48C.f1;_tmp48E=_tmp48C.f2;_tmp48F=_tmp48C.f3;{
void*tm;if(_tmp476 == 0)tm=(void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp490=
_region_malloc(r,sizeof(*_tmp490));_tmp490[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp491;_tmp491.tag=0;_tmp491.f1=_tmp477;_tmp491.f2=_tmp478;_tmp491;});_tmp490;});
else{tm=(void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp492=_region_malloc(r,
sizeof(*_tmp492));_tmp492[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp493;
_tmp493.tag=1;_tmp493.f1=(struct Cyc_Absyn_Exp*)_tmp476;_tmp493.f2=_tmp477;
_tmp493.f3=_tmp478;_tmp493;});_tmp492;});}return({struct _tuple7 _tmp494;_tmp494.f1=
_tmp48D;_tmp494.f2=_tmp48E;_tmp494.f3=({struct Cyc_List_List*_tmp495=
_region_malloc(r,sizeof(*_tmp495));_tmp495->hd=(void*)tm;_tmp495->tl=_tmp48F;
_tmp495;});_tmp494;});}}_LL312: if(*((int*)_tmp472)!= 4)goto _LL314;_tmp479=((
struct Cyc_Absyn_PointerType_struct*)_tmp472)->f1;_tmp47A=(void*)_tmp479.elt_typ;
_tmp47B=_tmp479.elt_tq;_tmp47C=_tmp479.ptr_atts;_LL313: {struct Cyc_Absyn_Tqual
_tmp497;void*_tmp498;struct Cyc_List_List*_tmp499;struct _tuple7 _tmp496=Cyc_Absynpp_to_tms(
r,_tmp47B,_tmp47A);_tmp497=_tmp496.f1;_tmp498=_tmp496.f2;_tmp499=_tmp496.f3;
_tmp499=({struct Cyc_List_List*_tmp49A=_region_malloc(r,sizeof(*_tmp49A));_tmp49A->hd=(
void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp49B=_region_malloc(r,
sizeof(*_tmp49B));_tmp49B[0]=({struct Cyc_Absyn_Pointer_mod_struct _tmp49C;_tmp49C.tag=
2;_tmp49C.f1=_tmp47C;_tmp49C.f2=tq;_tmp49C;});_tmp49B;}));_tmp49A->tl=_tmp499;
_tmp49A;});return({struct _tuple7 _tmp49D;_tmp49D.f1=_tmp497;_tmp49D.f2=_tmp498;
_tmp49D.f3=_tmp499;_tmp49D;});}_LL314: if(*((int*)_tmp472)!= 8)goto _LL316;_tmp47D=((
struct Cyc_Absyn_FnType_struct*)_tmp472)->f1;_tmp47E=_tmp47D.tvars;_tmp47F=
_tmp47D.effect;_tmp480=(void*)_tmp47D.ret_typ;_tmp481=_tmp47D.args;_tmp482=
_tmp47D.c_varargs;_tmp483=_tmp47D.cyc_varargs;_tmp484=_tmp47D.rgn_po;_tmp485=
_tmp47D.attributes;_LL315: if(!Cyc_Absynpp_print_all_tvars){if(_tmp47F == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((
void*)_tmp47F->v)){_tmp47F=0;_tmp47E=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar,_tmp47E);}{struct Cyc_Absyn_Tqual _tmp49F;void*
_tmp4A0;struct Cyc_List_List*_tmp4A1;struct _tuple7 _tmp49E=Cyc_Absynpp_to_tms(r,
Cyc_Absyn_empty_tqual(0),_tmp480);_tmp49F=_tmp49E.f1;_tmp4A0=_tmp49E.f2;_tmp4A1=
_tmp49E.f3;{struct Cyc_List_List*tms=_tmp4A1;{void*_tmp4A2=Cyc_Cyclone_c_compiler;
_LL31D: if((int)_tmp4A2 != 0)goto _LL31F;_LL31E: if(_tmp485 != 0)tms=Cyc_Absynpp_bubble_attributes(
r,(void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp4A3=_region_malloc(r,
sizeof(*_tmp4A3));_tmp4A3[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp4A4;
_tmp4A4.tag=5;_tmp4A4.f1=0;_tmp4A4.f2=_tmp485;_tmp4A4;});_tmp4A3;}),tms);tms=({
struct Cyc_List_List*_tmp4A5=_region_malloc(r,sizeof(*_tmp4A5));_tmp4A5->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp4A6=_region_malloc(r,sizeof(*
_tmp4A6));_tmp4A6[0]=({struct Cyc_Absyn_Function_mod_struct _tmp4A7;_tmp4A7.tag=3;
_tmp4A7.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp4A8=
_region_malloc(r,sizeof(*_tmp4A8));_tmp4A8[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp4A9;_tmp4A9.tag=1;_tmp4A9.f1=_tmp481;_tmp4A9.f2=_tmp482;_tmp4A9.f3=_tmp483;
_tmp4A9.f4=_tmp47F;_tmp4A9.f5=_tmp484;_tmp4A9;});_tmp4A8;}));_tmp4A7;});_tmp4A6;}));
_tmp4A5->tl=tms;_tmp4A5;});goto _LL31C;_LL31F: if((int)_tmp4A2 != 1)goto _LL31C;
_LL320: tms=({struct Cyc_List_List*_tmp4AA=_region_malloc(r,sizeof(*_tmp4AA));
_tmp4AA->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp4AB=
_region_malloc(r,sizeof(*_tmp4AB));_tmp4AB[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp4AC;_tmp4AC.tag=3;_tmp4AC.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp4AD=_region_malloc(r,sizeof(*_tmp4AD));_tmp4AD[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp4AE;_tmp4AE.tag=1;_tmp4AE.f1=_tmp481;_tmp4AE.f2=_tmp482;_tmp4AE.f3=_tmp483;
_tmp4AE.f4=_tmp47F;_tmp4AE.f5=_tmp484;_tmp4AE;});_tmp4AD;}));_tmp4AC;});_tmp4AB;}));
_tmp4AA->tl=tms;_tmp4AA;});for(0;_tmp485 != 0;_tmp485=_tmp485->tl){void*_tmp4AF=(
void*)_tmp485->hd;_LL322: if((int)_tmp4AF != 0)goto _LL324;_LL323: goto _LL325;_LL324:
if((int)_tmp4AF != 1)goto _LL326;_LL325: goto _LL327;_LL326: if((int)_tmp4AF != 2)goto
_LL328;_LL327: tms=({struct Cyc_List_List*_tmp4B0=_region_malloc(r,sizeof(*_tmp4B0));
_tmp4B0->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp4B1=
_region_malloc(r,sizeof(*_tmp4B1));_tmp4B1[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp4B2;_tmp4B2.tag=5;_tmp4B2.f1=0;_tmp4B2.f2=({struct Cyc_List_List*_tmp4B3=
_cycalloc(sizeof(*_tmp4B3));_tmp4B3->hd=(void*)((void*)_tmp485->hd);_tmp4B3->tl=
0;_tmp4B3;});_tmp4B2;});_tmp4B1;}));_tmp4B0->tl=tms;_tmp4B0;});goto AfterAtts;
_LL328:;_LL329: goto _LL321;_LL321:;}goto _LL31C;_LL31C:;}AfterAtts: if(_tmp47E != 0)
tms=({struct Cyc_List_List*_tmp4B4=_region_malloc(r,sizeof(*_tmp4B4));_tmp4B4->hd=(
void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp4B5=_region_malloc(r,
sizeof(*_tmp4B5));_tmp4B5[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp4B6;
_tmp4B6.tag=4;_tmp4B6.f1=_tmp47E;_tmp4B6.f2=0;_tmp4B6.f3=1;_tmp4B6;});_tmp4B5;}));
_tmp4B4->tl=tms;_tmp4B4;});return({struct _tuple7 _tmp4B7;_tmp4B7.f1=_tmp49F;
_tmp4B7.f2=_tmp4A0;_tmp4B7.f3=tms;_tmp4B7;});}}_LL316: if(*((int*)_tmp472)!= 0)
goto _LL318;_tmp486=((struct Cyc_Absyn_Evar_struct*)_tmp472)->f1;_tmp487=((struct
Cyc_Absyn_Evar_struct*)_tmp472)->f2;_tmp488=((struct Cyc_Absyn_Evar_struct*)
_tmp472)->f3;_LL317: if(_tmp487 == 0)return({struct _tuple7 _tmp4B8;_tmp4B8.f1=tq;
_tmp4B8.f2=t;_tmp4B8.f3=0;_tmp4B8;});else{return Cyc_Absynpp_to_tms(r,tq,(void*)
_tmp487->v);}_LL318: if(*((int*)_tmp472)!= 16)goto _LL31A;_tmp489=((struct Cyc_Absyn_TypedefType_struct*)
_tmp472)->f1;_tmp48A=((struct Cyc_Absyn_TypedefType_struct*)_tmp472)->f2;_tmp48B=((
struct Cyc_Absyn_TypedefType_struct*)_tmp472)->f4;_LL319: if((_tmp48B == 0  || !Cyc_Absynpp_expand_typedefs)
 || Cyc_Absynpp_is_anon_aggrtype(*_tmp48B))return({struct _tuple7 _tmp4B9;_tmp4B9.f1=
tq;_tmp4B9.f2=t;_tmp4B9.f3=0;_tmp4B9;});else{return Cyc_Absynpp_to_tms(r,tq,*
_tmp48B);}_LL31A:;_LL31B: return({struct _tuple7 _tmp4BA;_tmp4BA.f1=tq;_tmp4BA.f2=t;
_tmp4BA.f3=0;_tmp4BA;});_LL30F:;}static int Cyc_Absynpp_is_char_ptr(void*t){void*
_tmp4BB=t;struct Cyc_Core_Opt*_tmp4BC;struct Cyc_Core_Opt _tmp4BD;void*_tmp4BE;
struct Cyc_Absyn_PtrInfo _tmp4BF;void*_tmp4C0;_LL32B: if(_tmp4BB <= (void*)4)goto
_LL32F;if(*((int*)_tmp4BB)!= 0)goto _LL32D;_tmp4BC=((struct Cyc_Absyn_Evar_struct*)
_tmp4BB)->f2;if(_tmp4BC == 0)goto _LL32D;_tmp4BD=*_tmp4BC;_tmp4BE=(void*)_tmp4BD.v;
_LL32C: return Cyc_Absynpp_is_char_ptr(_tmp4BE);_LL32D: if(*((int*)_tmp4BB)!= 4)
goto _LL32F;_tmp4BF=((struct Cyc_Absyn_PointerType_struct*)_tmp4BB)->f1;_tmp4C0=(
void*)_tmp4BF.elt_typ;_LL32E: L: {void*_tmp4C1=_tmp4C0;struct Cyc_Core_Opt*_tmp4C2;
struct Cyc_Core_Opt _tmp4C3;void*_tmp4C4;void**_tmp4C5;void*_tmp4C6;void*_tmp4C7;
_LL332: if(_tmp4C1 <= (void*)4)goto _LL338;if(*((int*)_tmp4C1)!= 0)goto _LL334;
_tmp4C2=((struct Cyc_Absyn_Evar_struct*)_tmp4C1)->f2;if(_tmp4C2 == 0)goto _LL334;
_tmp4C3=*_tmp4C2;_tmp4C4=(void*)_tmp4C3.v;_LL333: _tmp4C0=_tmp4C4;goto L;_LL334:
if(*((int*)_tmp4C1)!= 16)goto _LL336;_tmp4C5=((struct Cyc_Absyn_TypedefType_struct*)
_tmp4C1)->f4;if(_tmp4C5 == 0)goto _LL336;_tmp4C6=*_tmp4C5;_LL335: _tmp4C0=_tmp4C6;
goto L;_LL336: if(*((int*)_tmp4C1)!= 5)goto _LL338;_tmp4C7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4C1)->f2;if((int)_tmp4C7 != 0)goto _LL338;_LL337: return 1;_LL338:;_LL339: return
0;_LL331:;}_LL32F:;_LL330: return 0;_LL32A:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){struct _RegionHandle
_tmp4C8=_new_region("temp");struct _RegionHandle*temp=& _tmp4C8;_push_region(temp);{
struct Cyc_Absyn_Tqual _tmp4CA;void*_tmp4CB;struct Cyc_List_List*_tmp4CC;struct
_tuple7 _tmp4C9=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp4CA=_tmp4C9.f1;_tmp4CB=
_tmp4C9.f2;_tmp4CC=_tmp4C9.f3;_tmp4CC=Cyc_List_imp_rev(_tmp4CC);if(_tmp4CC == 0
 && dopt == 0){struct Cyc_PP_Doc*_tmp4CE=({struct Cyc_PP_Doc*_tmp4CD[2];_tmp4CD[1]=
Cyc_Absynpp_ntyp2doc(_tmp4CB);_tmp4CD[0]=Cyc_Absynpp_tqual2doc(_tmp4CA);Cyc_PP_cat(
_tag_dyneither(_tmp4CD,sizeof(struct Cyc_PP_Doc*),2));});_npop_handler(0);return
_tmp4CE;}else{struct Cyc_PP_Doc*_tmp4D1=({struct Cyc_PP_Doc*_tmp4CF[4];_tmp4CF[3]=
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(
struct Cyc_PP_Doc*)dopt->v,_tmp4CC);_tmp4CF[2]=Cyc_PP_text(({const char*_tmp4D0=" ";
_tag_dyneither(_tmp4D0,sizeof(char),2);}));_tmp4CF[1]=Cyc_Absynpp_ntyp2doc(
_tmp4CB);_tmp4CF[0]=Cyc_Absynpp_tqual2doc(_tmp4CA);Cyc_PP_cat(_tag_dyneither(
_tmp4CF,sizeof(struct Cyc_PP_Doc*),4));});_npop_handler(0);return _tmp4D1;}};
_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*
f){void*_tmp4D2=Cyc_Cyclone_c_compiler;_LL33B: if((int)_tmp4D2 != 0)goto _LL33D;
_LL33C: if(f->width != 0)return({struct Cyc_PP_Doc*_tmp4D3[5];_tmp4D3[4]=Cyc_PP_text(({
const char*_tmp4D6=";";_tag_dyneither(_tmp4D6,sizeof(char),2);}));_tmp4D3[3]=Cyc_Absynpp_atts2doc(
f->attributes);_tmp4D3[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(
f->width));_tmp4D3[1]=Cyc_PP_text(({const char*_tmp4D5=":";_tag_dyneither(_tmp4D5,
sizeof(char),2);}));_tmp4D3[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,({
struct Cyc_Core_Opt*_tmp4D4=_cycalloc(sizeof(*_tmp4D4));_tmp4D4->v=Cyc_PP_textptr(
f->name);_tmp4D4;}));Cyc_PP_cat(_tag_dyneither(_tmp4D3,sizeof(struct Cyc_PP_Doc*),
5));});else{return({struct Cyc_PP_Doc*_tmp4D7[3];_tmp4D7[2]=Cyc_PP_text(({const
char*_tmp4D9=";";_tag_dyneither(_tmp4D9,sizeof(char),2);}));_tmp4D7[1]=Cyc_Absynpp_atts2doc(
f->attributes);_tmp4D7[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,({struct Cyc_Core_Opt*
_tmp4D8=_cycalloc(sizeof(*_tmp4D8));_tmp4D8->v=Cyc_PP_textptr(f->name);_tmp4D8;}));
Cyc_PP_cat(_tag_dyneither(_tmp4D7,sizeof(struct Cyc_PP_Doc*),3));});}_LL33D: if((
int)_tmp4D2 != 1)goto _LL33A;_LL33E: if(f->width != 0)return({struct Cyc_PP_Doc*
_tmp4DA[5];_tmp4DA[4]=Cyc_PP_text(({const char*_tmp4DD=";";_tag_dyneither(_tmp4DD,
sizeof(char),2);}));_tmp4DA[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(f->width));_tmp4DA[2]=Cyc_PP_text(({const char*_tmp4DC=":";
_tag_dyneither(_tmp4DC,sizeof(char),2);}));_tmp4DA[1]=Cyc_Absynpp_tqtd2doc(f->tq,(
void*)f->type,({struct Cyc_Core_Opt*_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB->v=
Cyc_PP_textptr(f->name);_tmp4DB;}));_tmp4DA[0]=Cyc_Absynpp_atts2doc(f->attributes);
Cyc_PP_cat(_tag_dyneither(_tmp4DA,sizeof(struct Cyc_PP_Doc*),5));});else{return({
struct Cyc_PP_Doc*_tmp4DE[3];_tmp4DE[2]=Cyc_PP_text(({const char*_tmp4E0=";";
_tag_dyneither(_tmp4E0,sizeof(char),2);}));_tmp4DE[1]=Cyc_Absynpp_tqtd2doc(f->tq,(
void*)f->type,({struct Cyc_Core_Opt*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));_tmp4DF->v=
Cyc_PP_textptr(f->name);_tmp4DF;}));_tmp4DE[0]=Cyc_Absynpp_atts2doc(f->attributes);
Cyc_PP_cat(_tag_dyneither(_tmp4DE,sizeof(struct Cyc_PP_Doc*),3));});}_LL33A:;}
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct
_dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,({
const char*_tmp4E1="";_tag_dyneither(_tmp4E1,sizeof(char),1);}),fields);}struct
Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){return({
struct Cyc_PP_Doc*_tmp4E2[3];_tmp4E2[2]=f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(
f->typs);_tmp4E2[1]=Cyc_Absynpp_typedef_name2doc(f->name);_tmp4E2[0]=Cyc_Absynpp_scope2doc((
void*)f->sc);Cyc_PP_cat(_tag_dyneither(_tmp4E2,sizeof(struct Cyc_PP_Doc*),3));});}
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,({
const char*_tmp4E3=",";_tag_dyneither(_tmp4E3,sizeof(char),2);}),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);({void*_tmp4E4=0;Cyc_fprintf(
f,({const char*_tmp4E5="\n";_tag_dyneither(_tmp4E5,sizeof(char),2);}),
_tag_dyneither(_tmp4E4,sizeof(void*),0));});}}struct _dyneither_ptr Cyc_Absynpp_decllist2string(
struct Cyc_List_List*tdl){return Cyc_PP_string_of_doc(Cyc_PP_seql(({const char*
_tmp4E6="";_tag_dyneither(_tmp4E6,sizeof(char),1);}),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_decl2doc,tdl)),72);}struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(
Cyc_Absynpp_stmt2doc(s),72);}struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){
return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}struct _dyneither_ptr Cyc_Absynpp_typ2cstring(
void*t){int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;Cyc_Absynpp_add_cyc_prefix=0;{struct _dyneither_ptr s=
Cyc_Absynpp_typ2string(t);Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
old_add_cyc_prefix;return s;}}struct _dyneither_ptr Cyc_Absynpp_prim2string(void*p){
return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}struct _dyneither_ptr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(void*sc){return Cyc_PP_string_of_doc(
Cyc_Absynpp_scope2doc(sc),72);}
