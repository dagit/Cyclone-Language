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
void*v;};int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_rlist(struct
_RegionHandle*,struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);
void*Cyc_List_hd(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_tl(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void Cyc_List_iter_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rappend(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct
Cyc_List_List*Cyc_List_delete_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,
void*x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char
Cyc_Set_Absent[11];typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}
__value;}Cyc___mbstate_t;typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;
typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct
_dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_rempty(struct
_RegionHandle*,int(*cmp)(void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict d,
void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct
Cyc_Dict_Dict d,void*k,void**ans);void Cyc_Dict_iter(void(*f)(void*,void*),struct
Cyc_Dict_Dict d);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct
Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,
void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct Cyc_Dict_Dict
Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2);int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),
struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct _tuple0{void*f1;void*f2;};
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);struct
Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;int Cyc_Position_segment_equals(
struct Cyc_Position_Segment*,struct Cyc_Position_Segment*);struct _dyneither_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dyneither_ptr source;
struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};extern char
Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;int is_flat;};
struct Cyc_Absyn_UnknownDatatype_struct{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
f1;};struct Cyc_Absyn_KnownDatatype_struct{int tag;struct Cyc_Absyn_Datatypedecl**
f1;};union Cyc_Absyn_DatatypeInfoU_union{struct Cyc_Absyn_UnknownDatatype_struct
UnknownDatatype;struct Cyc_Absyn_KnownDatatype_struct KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU_union datatype_info;struct Cyc_List_List*targs;struct
Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*
datatype_name;struct _tuple1*field_name;int is_extensible;};struct Cyc_Absyn_UnknownDatatypefield_struct{
int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo f1;};struct Cyc_Absyn_KnownDatatypefield_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};union
Cyc_Absyn_DatatypeFieldInfoU_union{struct Cyc_Absyn_UnknownDatatypefield_struct
UnknownDatatypefield;struct Cyc_Absyn_KnownDatatypefield_struct KnownDatatypefield;
};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple1*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
_dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple2{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple2*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
_tuple3{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
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
struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{void*sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;void*ret_type;
struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;
struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};
struct Cyc_Absyn_Datatypedecl{void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;
struct Cyc_Core_Opt*fields;int is_extensible;int is_flat;};struct Cyc_Absyn_Enumfield{
struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};
struct Cyc_Absyn_Enumdecl{void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};
struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*
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
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_nontagged_union_type(
void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;
struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct
Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;int allow_valueof;};void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_typecmp(void*,void*);int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct _tuple4{unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_UniquePlace{struct
Cyc_CfFlowInfo_Place place;struct Cyc_List_List*path;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct Cyc_CfFlowInfo_LessNumelts_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqNumelts_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};union Cyc_CfFlowInfo_RelnOp_union{struct Cyc_CfFlowInfo_EqualConst_struct
EqualConst;struct Cyc_CfFlowInfo_LessVar_struct LessVar;struct Cyc_CfFlowInfo_LessNumelts_struct
LessNumelts;struct Cyc_CfFlowInfo_LessConst_struct LessConst;struct Cyc_CfFlowInfo_LessEqNumelts_struct
LessEqNumelts;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp_union
rop;};struct Cyc_CfFlowInfo_TagCmp{void*cmp;void*bd;};char Cyc_CfFlowInfo_HasTagCmps[
15]="\000\000\000\000HasTagCmps\000";struct Cyc_CfFlowInfo_HasTagCmps_struct{char*
tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_IsZero[11]="\000\000\000\000IsZero\000";
char Cyc_CfFlowInfo_NotZero[12]="\000\000\000\000NotZero\000";struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[13]="\000\000\000\000UnknownZ\000";
struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};struct Cyc_List_List*
Cyc_CfFlowInfo_copy_tagcmps(struct _RegionHandle*,struct Cyc_List_List*);struct Cyc_CfFlowInfo_PlaceL_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UnknownL_struct{int
tag;};union Cyc_CfFlowInfo_AbsLVal_union{struct Cyc_CfFlowInfo_PlaceL_struct PlaceL;
struct Cyc_CfFlowInfo_UnknownL_struct UnknownL;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;int f1;
struct _dyneither_ptr f2;};int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*
set,void*place,struct Cyc_Position_Segment*loc);int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_equals(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_uplace_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_CfFlowInfo_and_consume(struct _RegionHandle*,struct Cyc_CfFlowInfo_ConsumeInfo
c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);int Cyc_CfFlowInfo_consume_approx(struct
Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*
f);int Cyc_CfFlowInfo_root_cmp(void*,void*);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo_union f1,union Cyc_CfFlowInfo_FlowInfo_union f2);void
Cyc_CfFlowInfo_print_absrval(void*rval);void Cyc_CfFlowInfo_print_initlevel(void*
il);void Cyc_CfFlowInfo_print_root(void*root);void Cyc_CfFlowInfo_print_place(
struct Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict
p,void(*pr)(void*));void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(
void*));void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);void Cyc_CfFlowInfo_print_flow(
union Cyc_CfFlowInfo_FlowInfo_union f);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct
Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,
struct Cyc_List_List*);void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
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
struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_consume_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);void Cyc_CfFlowInfo_check_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo_union f);struct _tuple6{struct Cyc_CfFlowInfo_ConsumeInfo
f1;union Cyc_CfFlowInfo_FlowInfo_union f2;};struct _tuple6 Cyc_CfFlowInfo_save_consume_info(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo_union f,int clear);
union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo_union
f,struct Cyc_CfFlowInfo_ConsumeInfo c,int may_consume_only);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);int Cyc_CfFlowInfo_place_cmp(struct
Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_root_cmp(
void*r1,void*r2);struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r){static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,(union Cyc_Absyn_Cnst_union)((
struct Cyc_Absyn_Null_c_struct){6})};static struct Cyc_Absyn_Exp dummy_exp={0,(void*)((
void*)& dummy_rawexp),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};return({struct Cyc_CfFlowInfo_FlowEnv*
_tmp0=_region_malloc(r,sizeof(*_tmp0));_tmp0->r=r;_tmp0->unknown_none=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownR_struct*_tmpE=_region_malloc(r,sizeof(*
_tmpE));_tmpE[0]=({struct Cyc_CfFlowInfo_UnknownR_struct _tmpF;_tmpF.tag=0;_tmpF.f1=(
void*)((void*)0);_tmpF;});_tmpE;}));_tmp0->unknown_this=(void*)((void*)({struct
Cyc_CfFlowInfo_UnknownR_struct*_tmpC=_region_malloc(r,sizeof(*_tmpC));_tmpC[0]=({
struct Cyc_CfFlowInfo_UnknownR_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(void*)((void*)1);
_tmpD;});_tmpC;}));_tmp0->unknown_all=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownR_struct*
_tmpA=_region_malloc(r,sizeof(*_tmpA));_tmpA[0]=({struct Cyc_CfFlowInfo_UnknownR_struct
_tmpB;_tmpB.tag=0;_tmpB.f1=(void*)((void*)2);_tmpB;});_tmpA;}));_tmp0->esc_none=(
void*)((void*)({struct Cyc_CfFlowInfo_Esc_struct*_tmp8=_region_malloc(r,sizeof(*
_tmp8));_tmp8[0]=({struct Cyc_CfFlowInfo_Esc_struct _tmp9;_tmp9.tag=1;_tmp9.f1=(
void*)((void*)0);_tmp9;});_tmp8;}));_tmp0->esc_this=(void*)((void*)({struct Cyc_CfFlowInfo_Esc_struct*
_tmp6=_region_malloc(r,sizeof(*_tmp6));_tmp6[0]=({struct Cyc_CfFlowInfo_Esc_struct
_tmp7;_tmp7.tag=1;_tmp7.f1=(void*)((void*)1);_tmp7;});_tmp6;}));_tmp0->esc_all=(
void*)((void*)({struct Cyc_CfFlowInfo_Esc_struct*_tmp4=_region_malloc(r,sizeof(*
_tmp4));_tmp4[0]=({struct Cyc_CfFlowInfo_Esc_struct _tmp5;_tmp5.tag=1;_tmp5.f1=(
void*)((void*)2);_tmp5;});_tmp4;}));_tmp0->mt_flowdict=Cyc_Dict_rempty(r,Cyc_CfFlowInfo_root_cmp);
_tmp0->mt_place_set=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp);
_tmp0->dummy_place=({struct Cyc_CfFlowInfo_Place*_tmp1=_region_malloc(r,sizeof(*
_tmp1));_tmp1->root=(void*)((void*)({struct Cyc_CfFlowInfo_MallocPt_struct*_tmp2=
_region_malloc(r,sizeof(*_tmp2));_tmp2[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp3;_tmp3.tag=1;_tmp3.f1=& dummy_exp;_tmp3.f2=(void*)((void*)0);_tmp3;});_tmp2;}));
_tmp1->fields=0;_tmp1;});_tmp0;});}int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*
set,void*place,struct Cyc_Position_Segment*loc){if(set != 0){if(((int(*)(struct Cyc_Dict_Dict
d,void*k))Cyc_Dict_member)(*set,place))return 1;else{*set=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,struct Cyc_Position_Segment*v))Cyc_Dict_insert)(*set,
place,loc);}}return 0;}struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct
Cyc_CfFlowInfo_Place*place){void*_tmp13;struct Cyc_List_List*_tmp14;struct Cyc_CfFlowInfo_Place
_tmp12=*place;_tmp13=(void*)_tmp12.root;_tmp14=_tmp12.fields;{void*_tmp15=_tmp13;
struct Cyc_Absyn_Vardecl*_tmp16;_LL1: if(*((int*)_tmp15)!= 0)goto _LL3;_tmp16=((
struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp15)->f1;_LL2: if(_tmp14 == 0)return Cyc_Absynpp_qvar2string(
_tmp16->name);else{return(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp19;_tmp19.tag=0;_tmp19.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp16->name));{void*_tmp17[1]={& _tmp19};Cyc_aprintf(({const char*_tmp18="reachable from %s";
_tag_dyneither(_tmp18,sizeof(char),18);}),_tag_dyneither(_tmp17,sizeof(void*),1));}});}
_LL3:;_LL4:({void*_tmp1A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(({const char*_tmp1B="error locations not for VarRoots";
_tag_dyneither(_tmp1B,sizeof(char),33);}),_tag_dyneither(_tmp1A,sizeof(void*),0));});
_LL0:;}}struct _dyneither_ptr Cyc_CfFlowInfo_uplace_err_string(struct Cyc_CfFlowInfo_UniquePlace*
place){return Cyc_CfFlowInfo_place_err_string(& place->place);}static void Cyc_CfFlowInfo_unique_err(
void*place,struct _dyneither_ptr(*toString)(void*),struct _dyneither_ptr err_msg1,
struct _dyneither_ptr err_msg2,struct Cyc_Position_Segment*consumed_loc,struct Cyc_Position_Segment*
loc){struct _dyneither_ptr _tmp1C=toString(place);if(!Cyc_Position_segment_equals(
consumed_loc,loc)){struct _dyneither_ptr _tmp1D=Cyc_Position_string_of_segment(
consumed_loc);({struct Cyc_String_pa_struct _tmp20;_tmp20.tag=0;_tmp20.f1=(struct
_dyneither_ptr)_tmp1D;{struct Cyc_String_pa_struct _tmp1F;_tmp1F.tag=0;_tmp1F.f1=(
struct _dyneither_ptr)_tmp1C;{void*_tmp1E[2]={& _tmp1F,& _tmp20};Cyc_Tcutil_terr(
loc,err_msg1,_tag_dyneither(_tmp1E,sizeof(void*),2));}}});}else{({struct Cyc_String_pa_struct
_tmp22;_tmp22.tag=0;_tmp22.f1=(struct _dyneither_ptr)_tmp1C;{void*_tmp21[1]={&
_tmp22};Cyc_Tcutil_terr(loc,err_msg2,_tag_dyneither(_tmp21,sizeof(void*),1));}});}}
struct _tuple7{int f1;struct _dyneither_ptr(*f2)(void*);};static struct Cyc_Position_Segment*
Cyc_CfFlowInfo_combine_consume_f(struct _tuple7*env,void*place,struct Cyc_Position_Segment*
loc1,struct Cyc_Position_Segment*loc2){struct _tuple7 _tmp24;int _tmp25;struct
_dyneither_ptr(*_tmp26)(void*);struct _tuple7*_tmp23=env;_tmp24=*_tmp23;_tmp25=
_tmp24.f1;_tmp26=_tmp24.f2;if(_tmp25)Cyc_CfFlowInfo_unique_err(place,_tmp26,({
const char*_tmp27="May consume non-aliased pointer %s more than once (cf. %s)";
_tag_dyneither(_tmp27,sizeof(char),59);}),({const char*_tmp28="May consume non-aliased pointer %s more than once";
_tag_dyneither(_tmp28,sizeof(char),50);}),loc1,loc2);return loc1;}struct _tuple8{
int f1;struct _dyneither_ptr(*f2)(struct Cyc_CfFlowInfo_Place*place);};struct Cyc_Dict_Dict
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int
disjoint){struct _tuple8 _tmp29=({struct _tuple8 _tmp2B;_tmp2B.f1=disjoint;_tmp2B.f2=
Cyc_CfFlowInfo_place_err_string;_tmp2B;});struct Cyc_Dict_Dict _tmp2A=((struct Cyc_Dict_Dict(*)(
struct Cyc_Position_Segment*(*f)(struct _tuple8*,struct Cyc_CfFlowInfo_Place*,
struct Cyc_Position_Segment*,struct Cyc_Position_Segment*),struct _tuple8*env,
struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)((struct Cyc_Position_Segment*(*)(
struct _tuple8*env,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc1,struct Cyc_Position_Segment*loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp29,
s1,s2);return _tmp2A;}struct _tuple9{int f1;struct _dyneither_ptr(*f2)(struct Cyc_CfFlowInfo_UniquePlace*
place);};struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_uplace_set(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2,int disjoint){struct _tuple9 _tmp2C=({struct _tuple9 _tmp2D;
_tmp2D.f1=disjoint;_tmp2D.f2=Cyc_CfFlowInfo_uplace_err_string;_tmp2D;});return((
struct Cyc_Dict_Dict(*)(struct Cyc_Position_Segment*(*f)(struct _tuple9*,struct Cyc_CfFlowInfo_UniquePlace*,
struct Cyc_Position_Segment*,struct Cyc_Position_Segment*),struct _tuple9*env,
struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)((struct Cyc_Position_Segment*(*)(
struct _tuple9*env,struct Cyc_CfFlowInfo_UniquePlace*place,struct Cyc_Position_Segment*
loc1,struct Cyc_Position_Segment*loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp2C,
s1,s2);}struct _tuple10{void*f1;struct Cyc_Position_Segment*f2;};int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){if((int)s1.t == (int)s2.t)return 1;{
struct _handler_cons _tmp2E;_push_handler(& _tmp2E);{int _tmp30=0;if(setjmp(_tmp2E.handler))
_tmp30=1;if(!_tmp30){{struct _RegionHandle _tmp31=_new_region("temp");struct
_RegionHandle*temp=& _tmp31;_push_region(temp);{struct Cyc_Iter_Iter _tmp32=((
struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
temp,s1);struct _tuple10 _tmp33=*((struct _tuple10*(*)(struct _RegionHandle*r,struct
Cyc_Dict_Dict d))Cyc_Dict_rchoose)(temp,s1);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple10*))Cyc_Iter_next)(_tmp32,& _tmp33)){void*_tmp34=_tmp33.f1;if(!((int(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(s2,_tmp34)){int _tmp35=0;
_npop_handler(1);return _tmp35;}}{int _tmp36=1;_npop_handler(1);return _tmp36;}};
_pop_region(temp);};_pop_handler();}else{void*_tmp2F=(void*)_exn_thrown;void*
_tmp38=_tmp2F;_LL6:;_LL7: return 1;_LL8:;_LL9:(void)_throw(_tmp38);_LL5:;}}}}int
Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(
s2,s1);}int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct
_dyneither_ptr*f){int n=0;for(0;(unsigned int)fs;fs=fs->tl){struct _dyneither_ptr*
_tmp39=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;if(Cyc_strptrcmp(_tmp39,f)== 0)
return n;n=n + 1;}(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp3A=
_cycalloc(sizeof(*_tmp3A));_tmp3A[0]=({struct Cyc_Core_Impossible_struct _tmp3B;
_tmp3B.tag=Cyc_Core_Impossible;_tmp3B.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp3E;_tmp3E.tag=0;_tmp3E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{
void*_tmp3C[1]={& _tmp3E};Cyc_aprintf(({const char*_tmp3D="get_field_index_fs failed: %s";
_tag_dyneither(_tmp3D,sizeof(char),30);}),_tag_dyneither(_tmp3C,sizeof(void*),1));}});
_tmp3B;});_tmp3A;}));}int Cyc_CfFlowInfo_get_field_index(void*t,struct
_dyneither_ptr*f){void*_tmp3F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp40;union Cyc_Absyn_AggrInfoU_union _tmp41;struct Cyc_List_List*_tmp42;_LLB: if(
_tmp3F <= (void*)4)goto _LLF;if(*((int*)_tmp3F)!= 10)goto _LLD;_tmp40=((struct Cyc_Absyn_AggrType_struct*)
_tmp3F)->f1;_tmp41=_tmp40.aggr_info;_LLC: {struct Cyc_List_List*_tmp43=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp41))->impl))->fields;
_tmp42=_tmp43;goto _LLE;}_LLD: if(*((int*)_tmp3F)!= 11)goto _LLF;_tmp42=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp3F)->f2;_LLE: return Cyc_CfFlowInfo_get_field_index_fs(_tmp42,f);_LLF:;_LL10:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp44=_cycalloc(sizeof(*
_tmp44));_tmp44[0]=({struct Cyc_Core_Impossible_struct _tmp45;_tmp45.tag=Cyc_Core_Impossible;
_tmp45.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp48;_tmp48.tag=0;
_tmp48.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{
void*_tmp46[1]={& _tmp48};Cyc_aprintf(({const char*_tmp47="get_field_index failed: %s";
_tag_dyneither(_tmp47,sizeof(char),27);}),_tag_dyneither(_tmp46,sizeof(void*),1));}});
_tmp45;});_tmp44;}));_LLA:;}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)
r1 == (int)r2)return 0;{struct _tuple0 _tmp4A=({struct _tuple0 _tmp49;_tmp49.f1=r1;
_tmp49.f2=r2;_tmp49;});void*_tmp4B;struct Cyc_Absyn_Vardecl*_tmp4C;void*_tmp4D;
struct Cyc_Absyn_Vardecl*_tmp4E;void*_tmp4F;void*_tmp50;void*_tmp51;struct Cyc_Absyn_Exp*
_tmp52;void*_tmp53;struct Cyc_Absyn_Exp*_tmp54;void*_tmp55;void*_tmp56;void*
_tmp57;int _tmp58;void*_tmp59;int _tmp5A;_LL12: _tmp4B=_tmp4A.f1;if(*((int*)_tmp4B)
!= 0)goto _LL14;_tmp4C=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp4B)->f1;_tmp4D=
_tmp4A.f2;if(*((int*)_tmp4D)!= 0)goto _LL14;_tmp4E=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp4D)->f1;_LL13: return(int)_tmp4C - (int)_tmp4E;_LL14: _tmp4F=_tmp4A.f1;if(*((
int*)_tmp4F)!= 0)goto _LL16;_LL15: return - 1;_LL16: _tmp50=_tmp4A.f2;if(*((int*)
_tmp50)!= 0)goto _LL18;_LL17: return 1;_LL18: _tmp51=_tmp4A.f1;if(*((int*)_tmp51)!= 
1)goto _LL1A;_tmp52=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp51)->f1;_tmp53=
_tmp4A.f2;if(*((int*)_tmp53)!= 1)goto _LL1A;_tmp54=((struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp53)->f1;_LL19: return(int)_tmp52 - (int)_tmp54;_LL1A: _tmp55=_tmp4A.f1;if(*((
int*)_tmp55)!= 1)goto _LL1C;_LL1B: return - 1;_LL1C: _tmp56=_tmp4A.f2;if(*((int*)
_tmp56)!= 1)goto _LL1E;_LL1D: return 1;_LL1E: _tmp57=_tmp4A.f1;if(*((int*)_tmp57)!= 
2)goto _LL11;_tmp58=((struct Cyc_CfFlowInfo_InitParam_struct*)_tmp57)->f1;_tmp59=
_tmp4A.f2;if(*((int*)_tmp59)!= 2)goto _LL11;_tmp5A=((struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp59)->f1;_LL1F: return _tmp58 - _tmp5A;_LL11:;}}static int Cyc_CfFlowInfo_intlist_cmp(
struct Cyc_List_List*list1,struct Cyc_List_List*list2){return((int(*)(int(*cmp)(
int,int),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,
list1,list2);}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct
Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)p2)return 0;{void*_tmp5C;struct Cyc_List_List*
_tmp5D;struct Cyc_CfFlowInfo_Place _tmp5B=*p1;_tmp5C=(void*)_tmp5B.root;_tmp5D=
_tmp5B.fields;{void*_tmp5F;struct Cyc_List_List*_tmp60;struct Cyc_CfFlowInfo_Place
_tmp5E=*p2;_tmp5F=(void*)_tmp5E.root;_tmp60=_tmp5E.fields;{int i=Cyc_CfFlowInfo_root_cmp(
_tmp5C,_tmp5F);if(i != 0)return i;return Cyc_CfFlowInfo_intlist_cmp(_tmp5D,_tmp60);}}}}
int Cyc_CfFlowInfo_uplace_cmp(struct Cyc_CfFlowInfo_UniquePlace*p1,struct Cyc_CfFlowInfo_UniquePlace*
p2){if((int)p1 == (int)p2)return 0;{struct Cyc_CfFlowInfo_Place _tmp62;struct Cyc_List_List*
_tmp63;struct Cyc_CfFlowInfo_UniquePlace _tmp61=*p1;_tmp62=_tmp61.place;_tmp63=
_tmp61.path;{struct Cyc_CfFlowInfo_Place _tmp65;struct Cyc_List_List*_tmp66;struct
Cyc_CfFlowInfo_UniquePlace _tmp64=*p2;_tmp65=_tmp64.place;_tmp66=_tmp64.path;{int
i=Cyc_CfFlowInfo_place_cmp(& _tmp62,& _tmp65);if(i != 0)return i;return((int(*)(int(*
cmp)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_CfFlowInfo_intlist_cmp,_tmp63,_tmp66);}}}}static
struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_List_List*sl=0;void*_tmp68;struct Cyc_List_List*_tmp69;struct Cyc_CfFlowInfo_Place
_tmp67=*p;_tmp68=(void*)_tmp67.root;_tmp69=_tmp67.fields;{void*_tmp6A=_tmp68;
struct Cyc_Absyn_Vardecl*_tmp6B;struct Cyc_Absyn_Exp*_tmp6C;int _tmp6D;_LL21: if(*((
int*)_tmp6A)!= 0)goto _LL23;_tmp6B=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp6A)->f1;
_LL22: sl=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=({
struct _dyneither_ptr*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F[0]=({struct Cyc_String_pa_struct
_tmp72;_tmp72.tag=0;_tmp72.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*
_tmp6B->name).f2);{void*_tmp70[1]={& _tmp72};Cyc_aprintf(({const char*_tmp71="%s";
_tag_dyneither(_tmp71,sizeof(char),3);}),_tag_dyneither(_tmp70,sizeof(void*),1));}});
_tmp6F;});_tmp6E->tl=sl;_tmp6E;});goto _LL20;_LL23: if(*((int*)_tmp6A)!= 1)goto
_LL25;_tmp6C=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp6A)->f1;_LL24: sl=({
struct Cyc_List_List*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->hd=({struct
_dyneither_ptr*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=({struct Cyc_Int_pa_struct
_tmp77;_tmp77.tag=1;_tmp77.f1=(unsigned long)((int)_tmp6C);{void*_tmp75[1]={&
_tmp77};Cyc_aprintf(({const char*_tmp76="mpt%d";_tag_dyneither(_tmp76,sizeof(char),
6);}),_tag_dyneither(_tmp75,sizeof(void*),1));}});_tmp74;});_tmp73->tl=sl;_tmp73;});
goto _LL20;_LL25: if(*((int*)_tmp6A)!= 2)goto _LL20;_tmp6D=((struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp6A)->f1;_LL26: sl=({struct Cyc_List_List*_tmp78=_cycalloc(sizeof(*_tmp78));
_tmp78->hd=({struct _dyneither_ptr*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79[0]=({
struct Cyc_Int_pa_struct _tmp7C;_tmp7C.tag=1;_tmp7C.f1=(unsigned long)_tmp6D;{void*
_tmp7A[1]={& _tmp7C};Cyc_aprintf(({const char*_tmp7B="param%d";_tag_dyneither(
_tmp7B,sizeof(char),8);}),_tag_dyneither(_tmp7A,sizeof(void*),1));}});_tmp79;});
_tmp78->tl=sl;_tmp78;});goto _LL20;_LL20:;}for(0;_tmp69 != 0;_tmp69=_tmp69->tl){sl=({
struct Cyc_List_List*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->hd=({struct
_dyneither_ptr*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E[0]=({struct Cyc_Int_pa_struct
_tmp81;_tmp81.tag=1;_tmp81.f1=(unsigned long)((int)_tmp69->hd);{void*_tmp7F[1]={&
_tmp81};Cyc_aprintf(({const char*_tmp80="%d";_tag_dyneither(_tmp80,sizeof(char),3);}),
_tag_dyneither(_tmp7F,sizeof(void*),1));}});_tmp7E;});_tmp7D->tl=sl;_tmp7D;});}{
struct _dyneither_ptr*_tmp82=({struct _dyneither_ptr*_tmp87=_cycalloc(sizeof(*
_tmp87));_tmp87[0]=({struct Cyc_String_pa_struct _tmp8A;_tmp8A.tag=0;_tmp8A.f1=(
struct _dyneither_ptr)({const char*_tmp8B="";_tag_dyneither(_tmp8B,sizeof(char),1);});{
void*_tmp88[1]={& _tmp8A};Cyc_aprintf(({const char*_tmp89="%s";_tag_dyneither(
_tmp89,sizeof(char),3);}),_tag_dyneither(_tmp88,sizeof(void*),1));}});_tmp87;});
for(0;sl != 0;sl=sl->tl){*_tmp82=({struct Cyc_String_pa_struct _tmp86;_tmp86.tag=0;
_tmp86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp82);{struct Cyc_String_pa_struct
_tmp85;_tmp85.tag=0;_tmp85.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)sl->hd));{void*_tmp83[2]={& _tmp85,& _tmp86};Cyc_aprintf(({
const char*_tmp84="%s.%s";_tag_dyneither(_tmp84,sizeof(char),6);}),_tag_dyneither(
_tmp83,sizeof(void*),2));}}});}return _tmp82;}}static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,void*t,void*leafval);struct
_dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_List_List*fs,void*leafval){unsigned int sz=(unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(fs);struct _dyneither_ptr d=({unsigned int
_tmp90=sz;void**_tmp91=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),
_tmp90));struct _dyneither_ptr _tmp93=_tag_dyneither(_tmp91,sizeof(void*),_tmp90);{
unsigned int _tmp92=_tmp90;unsigned int i;for(i=0;i < _tmp92;i ++){_tmp91[i]=(void*)
fenv->unknown_all;}}_tmp93;});{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){struct Cyc_Absyn_Aggrfield
_tmp8D;struct _dyneither_ptr*_tmp8E;void*_tmp8F;struct Cyc_Absyn_Aggrfield*_tmp8C=(
struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;_tmp8D=*
_tmp8C;_tmp8E=_tmp8D.name;_tmp8F=(void*)_tmp8D.type;if(_get_dyneither_size(*
_tmp8E,sizeof(char))!= 1)*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=
Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,_tmp8F,leafval);}}return d;}struct _tuple11{
struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,void*t,void*leafval){if(Cyc_Absyn_is_nontagged_union_type(
t))return(void*)fenv->unknown_all;{void*_tmp94=Cyc_Tcutil_compress(t);struct Cyc_Absyn_DatatypeFieldInfo
_tmp95;union Cyc_Absyn_DatatypeFieldInfoU_union _tmp96;struct Cyc_Absyn_Datatypefield*
_tmp97;struct Cyc_List_List*_tmp98;struct Cyc_Absyn_AggrInfo _tmp99;union Cyc_Absyn_AggrInfoU_union
_tmp9A;void*_tmp9B;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_ArrayInfo _tmp9D;
void*_tmp9E;struct Cyc_Absyn_Conref*_tmp9F;void*_tmpA0;struct Cyc_Absyn_PtrInfo
_tmpA1;struct Cyc_Absyn_PtrAtts _tmpA2;struct Cyc_Absyn_Conref*_tmpA3;_LL28: if(
_tmp94 <= (void*)4)goto _LL36;if(*((int*)_tmp94)!= 3)goto _LL2A;_tmp95=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp94)->f1;_tmp96=_tmp95.field_info;if((((((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp94)->f1).field_info).KnownDatatypefield).tag != 1)goto _LL2A;_tmp97=(_tmp96.KnownDatatypefield).f2;
_LL29: if(_tmp97->typs == 0)return leafval;_tmp98=_tmp97->typs;goto _LL2B;_LL2A: if(*((
int*)_tmp94)!= 9)goto _LL2C;_tmp98=((struct Cyc_Absyn_TupleType_struct*)_tmp94)->f1;
_LL2B: {unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp98);struct _dyneither_ptr d=({unsigned int _tmpA6=sz;void**_tmpA7=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmpA6));struct _dyneither_ptr
_tmpA9=_tag_dyneither(_tmpA7,sizeof(void*),_tmpA6);{unsigned int _tmpA8=_tmpA6;
unsigned int i;for(i=0;i < _tmpA8;i ++){_tmpA7[i]=(void*)fenv->unknown_all;}}_tmpA9;});{
int i=0;for(0;i < sz;++ i){*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=
Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,(*((struct _tuple11*)((struct Cyc_List_List*)
_check_null(_tmp98))->hd)).f2,leafval);_tmp98=_tmp98->tl;}}return(void*)({struct
Cyc_CfFlowInfo_Aggregate_struct*_tmpA4=_region_malloc(fenv->r,sizeof(*_tmpA4));
_tmpA4[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmpA5;_tmpA5.tag=4;_tmpA5.f1=0;
_tmpA5.f2=d;_tmpA5;});_tmpA4;});}_LL2C: if(*((int*)_tmp94)!= 10)goto _LL2E;_tmp99=((
struct Cyc_Absyn_AggrType_struct*)_tmp94)->f1;_tmp9A=_tmp99.aggr_info;_LL2D: {
struct Cyc_Absyn_Aggrdecl*_tmpAA=Cyc_Absyn_get_known_aggrdecl(_tmp9A);if(_tmpAA->impl
== 0)goto _LL27;_tmp9B=(void*)_tmpAA->kind;_tmp9C=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpAA->impl))->fields;goto _LL2F;}_LL2E: if(*((int*)_tmp94)!= 11)goto
_LL30;_tmp9B=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp94)->f1;_tmp9C=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp94)->f2;_LL2F: return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpAB=_region_malloc(fenv->r,sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpAC;_tmpAC.tag=4;_tmpAC.f1=_tmp9B == (void*)1;_tmpAC.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(
fenv,_tmp9C,leafval);_tmpAC;});_tmpAB;});_LL30: if(*((int*)_tmp94)!= 7)goto _LL32;
_tmp9D=((struct Cyc_Absyn_ArrayType_struct*)_tmp94)->f1;_tmp9E=(void*)_tmp9D.elt_type;
_tmp9F=_tmp9D.zero_term;if(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp9F))goto _LL32;_LL31: return allow_zeroterm  && Cyc_Tcutil_bits_only(_tmp9E)?(
void*)fenv->unknown_all: leafval;_LL32: if(*((int*)_tmp94)!= 18)goto _LL34;_tmpA0=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp94)->f1;_LL33: {void*_tmpAD=leafval;
void*_tmpAE;void*_tmpAF;_LL39: if(_tmpAD <= (void*)3)goto _LL3D;if(*((int*)_tmpAD)
!= 0)goto _LL3B;_tmpAE=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpAD)->f1;
if((int)_tmpAE != 2)goto _LL3B;_LL3A: goto _LL3C;_LL3B: if(*((int*)_tmpAD)!= 1)goto
_LL3D;_tmpAF=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmpAD)->f1;if((int)
_tmpAF != 2)goto _LL3D;_LL3C: goto _LL3E;_LL3D: if((int)_tmpAD != 0)goto _LL3F;_LL3E:
goto _LL40;_LL3F: if((int)_tmpAD != 1)goto _LL41;_LL40: return(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*
_tmpB0=_region_malloc(fenv->r,sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmpB1;_tmpB1.tag=3;_tmpB1.f1=({struct Cyc_CfFlowInfo_TagCmp*_tmpB2[1];_tmpB2[0]=({
struct Cyc_CfFlowInfo_TagCmp*_tmpB3=_region_malloc(fenv->r,sizeof(*_tmpB3));
_tmpB3->cmp=(void*)((void*)5);_tmpB3->bd=(void*)_tmpA0;_tmpB3;});((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(fenv->r,
_tag_dyneither(_tmpB2,sizeof(struct Cyc_CfFlowInfo_TagCmp*),1));});_tmpB1;});
_tmpB0;});_LL41:;_LL42: return leafval;_LL38:;}_LL34: if(*((int*)_tmp94)!= 4)goto
_LL36;_tmpA1=((struct Cyc_Absyn_PointerType_struct*)_tmp94)->f1;_tmpA2=_tmpA1.ptr_atts;
_tmpA3=_tmpA2.nullable;if(!(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpA3)))goto _LL36;_LL35:{void*_tmpB4=leafval;void*_tmpB5;void*_tmpB6;_LL44:
if(_tmpB4 <= (void*)3)goto _LL48;if(*((int*)_tmpB4)!= 0)goto _LL46;_tmpB5=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpB4)->f1;if((int)_tmpB5 != 1)goto _LL46;
_LL45: return(void*)2;_LL46: if(*((int*)_tmpB4)!= 0)goto _LL48;_tmpB6=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpB4)->f1;if((int)_tmpB6 != 2)goto _LL48;
_LL47: return(void*)1;_LL48:;_LL49: goto _LL43;_LL43:;}goto _LL27;_LL36:;_LL37: goto
_LL27;_LL27:;}return Cyc_Tcutil_bits_only(t)?(void*)fenv->unknown_all: leafval;}
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,void*
leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,t,leafval);}struct _tuple12{
struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*f2;};static int Cyc_CfFlowInfo_prefix_of_member(
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){struct _RegionHandle
_tmpB7=_new_region("r");struct _RegionHandle*r=& _tmpB7;_push_region(r);{struct
_tuple12 _tmpB8=({struct _tuple12 _tmpC3;_tmpC3.f1=place;_tmpC3.f2=0;_tmpC3;});
struct Cyc_Iter_Iter _tmpB9=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,
struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple12*))Cyc_Iter_next)(_tmpB9,& _tmpB8)){struct Cyc_CfFlowInfo_Place*
_tmpBA=_tmpB8.f1;void*_tmpBC;struct Cyc_List_List*_tmpBD;struct Cyc_CfFlowInfo_Place
_tmpBB=*place;_tmpBC=(void*)_tmpBB.root;_tmpBD=_tmpBB.fields;{void*_tmpBF;struct
Cyc_List_List*_tmpC0;struct Cyc_CfFlowInfo_Place _tmpBE=*_tmpBA;_tmpBF=(void*)
_tmpBE.root;_tmpC0=_tmpBE.fields;if(Cyc_CfFlowInfo_root_cmp(_tmpBC,_tmpBF)!= 0)
continue;for(0;_tmpBD != 0  && _tmpC0 != 0;(_tmpBD=_tmpBD->tl,_tmpC0=_tmpC0->tl)){
if((int)_tmpBD->hd != (int)_tmpC0->hd)break;}if(_tmpBD == 0){int _tmpC1=1;
_npop_handler(0);return _tmpC1;}}}{int _tmpC2=0;_npop_handler(0);return _tmpC2;}};
_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*
places;};static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,
struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place))pile->places=({struct
Cyc_List_List*_tmpC4=_region_malloc(pile->rgn,sizeof(*_tmpC4));_tmpC4->hd=place;
_tmpC4->tl=pile->places;_tmpC4;});}static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*
pile,void*r){void*_tmpC5=r;struct Cyc_CfFlowInfo_Place*_tmpC6;int _tmpC7;struct
_dyneither_ptr _tmpC8;_LL4B: if(_tmpC5 <= (void*)3)goto _LL4F;if(*((int*)_tmpC5)!= 2)
goto _LL4D;_tmpC6=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpC5)->f1;_LL4C: Cyc_CfFlowInfo_add_place(
pile,_tmpC6);return;_LL4D: if(*((int*)_tmpC5)!= 4)goto _LL4F;_tmpC7=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpC5)->f1;_tmpC8=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpC5)->f2;_LL4E:{
int i=0;for(0;i < _get_dyneither_size(_tmpC8,sizeof(void*));++ i){Cyc_CfFlowInfo_add_places(
pile,((void**)_tmpC8.curr)[i]);}}return;_LL4F:;_LL50: return;_LL4A:;}static void*
Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*
old_val){void*_tmpC9=old_val;int _tmpCA;struct _dyneither_ptr _tmpCB;_LL52: if(
_tmpC9 <= (void*)3)goto _LL54;if(*((int*)_tmpC9)!= 4)goto _LL54;_tmpCA=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpC9)->f1;_tmpCB=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpC9)->f2;_LL53: {
struct _dyneither_ptr d2=({unsigned int _tmpCE=_get_dyneither_size(_tmpCB,sizeof(
void*));void**_tmpCF=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmpCE));
struct _dyneither_ptr _tmpD1=_tag_dyneither(_tmpCF,sizeof(void*),_tmpCE);{
unsigned int _tmpD0=_tmpCE;unsigned int i;for(i=0;i < _tmpD0;i ++){_tmpCF[i]=Cyc_CfFlowInfo_insert_place_inner(
r,new_val,((void**)_tmpCB.curr)[(int)i]);}}_tmpD1;});return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpCC=_region_malloc(r,sizeof(*_tmpCC));_tmpCC[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpCD;_tmpCD.tag=4;_tmpCD.f1=_tmpCA;_tmpCD.f2=d2;_tmpCD;});_tmpCC;});}_LL54:;
_LL55: return new_val;_LL51:;}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(
struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){void*_tmpD2=*((void**)
_check_dyneither_subscript(d,sizeof(void*),n));if(_tmpD2 == rval)return d;{struct
_dyneither_ptr res=({unsigned int _tmpD3=_get_dyneither_size(d,sizeof(void*));void**
_tmpD4=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmpD3));struct
_dyneither_ptr _tmpD6=_tag_dyneither(_tmpD4,sizeof(void*),_tmpD3);{unsigned int
_tmpD5=_tmpD3;unsigned int i;for(i=0;i < _tmpD5;i ++){_tmpD4[i]=((void**)d.curr)[(
int)i];}}_tmpD6;});*((void**)_check_dyneither_subscript(res,sizeof(void*),n))=
rval;return res;}}struct _tuple13{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 
0)return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{struct _tuple13
_tmpD8=({struct _tuple13 _tmpD7;_tmpD7.f1=fs;_tmpD7.f2=old_val;_tmpD7;});struct Cyc_List_List*
_tmpD9;struct Cyc_List_List _tmpDA;int _tmpDB;struct Cyc_List_List*_tmpDC;void*
_tmpDD;int _tmpDE;struct _dyneither_ptr _tmpDF;_LL57: _tmpD9=_tmpD8.f1;if(_tmpD9 == 0)
goto _LL59;_tmpDA=*_tmpD9;_tmpDB=(int)_tmpDA.hd;_tmpDC=_tmpDA.tl;_tmpDD=_tmpD8.f2;
if(_tmpDD <= (void*)3)goto _LL59;if(*((int*)_tmpDD)!= 4)goto _LL59;_tmpDE=((struct
Cyc_CfFlowInfo_Aggregate_struct*)_tmpDD)->f1;_tmpDF=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpDD)->f2;_LL58: {void*_tmpE0=Cyc_CfFlowInfo_insert_place_outer(r,_tmpDC,*((
void**)_check_dyneither_subscript(_tmpDF,sizeof(void*),_tmpDB)),new_val);struct
_dyneither_ptr _tmpE1=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmpDF,_tmpDB,_tmpE0);if(
_tmpE1.curr == _tmpDF.curr)return old_val;else{return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpE2=_region_malloc(r,sizeof(*_tmpE2));_tmpE2[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpE3;_tmpE3.tag=4;_tmpE3.f1=_tmpDE;_tmpE3.f2=_tmpE1;_tmpE3;});_tmpE2;});}}
_LL59:;_LL5A:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpE4=
_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=({struct Cyc_Core_Impossible_struct _tmpE5;
_tmpE5.tag=Cyc_Core_Impossible;_tmpE5.f1=({const char*_tmpE6="bad insert place";
_tag_dyneither(_tmpE6,sizeof(char),17);});_tmpE5;});_tmpE4;}));_LL56:;}}static
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict
d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*_tmpE7=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(pile->places))->hd;pile->places=((struct Cyc_List_List*)
_check_null(pile->places))->tl;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
all_changed,_tmpE7,0);{void*oldval;void*newval;{struct _handler_cons _tmpE8;
_push_handler(& _tmpE8);{int _tmpEA=0;if(setjmp(_tmpE8.handler))_tmpEA=1;if(!
_tmpEA){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmpE7);;_pop_handler();}else{void*
_tmpE9=(void*)_exn_thrown;void*_tmpEC=_tmpE9;_LL5C: if(_tmpEC != Cyc_Dict_Absent)
goto _LL5E;_LL5D: continue;_LL5E:;_LL5F:(void)_throw(_tmpEC);_LL5B:;}}}{void*
_tmpED=Cyc_CfFlowInfo_initlevel(fenv,d,oldval);_LL61: if((int)_tmpED != 2)goto
_LL63;_LL62: newval=(void*)fenv->esc_all;goto _LL60;_LL63: if((int)_tmpED != 1)goto
_LL65;_LL64: newval=(void*)fenv->esc_this;goto _LL60;_LL65: if((int)_tmpED != 0)goto
_LL60;_LL66: newval=(void*)fenv->esc_none;goto _LL60;_LL60:;}Cyc_CfFlowInfo_add_places(
pile,oldval);{void*_tmpEF;struct Cyc_List_List*_tmpF0;struct Cyc_CfFlowInfo_Place
_tmpEE=*_tmpE7;_tmpEF=(void*)_tmpEE.root;_tmpF0=_tmpEE.fields;d=Cyc_Dict_insert(
d,_tmpEF,Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmpF0,Cyc_Dict_lookup(d,
_tmpEF),newval));}}}return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct
_RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};static void*Cyc_CfFlowInfo_initlevel_approx(
void*r){void*_tmpF1=r;void*_tmpF2;void*_tmpF3;_LL68: if(_tmpF1 <= (void*)3)goto
_LL6C;if(*((int*)_tmpF1)!= 0)goto _LL6A;_tmpF2=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpF1)->f1;_LL69: return _tmpF2;_LL6A: if(*((int*)_tmpF1)!= 1)goto _LL6C;_tmpF3=(
void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmpF1)->f1;_LL6B: return _tmpF3;_LL6C:
if((int)_tmpF1 != 0)goto _LL6E;_LL6D: goto _LL6F;_LL6E: if((int)_tmpF1 != 1)goto _LL70;
_LL6F: return(void*)2;_LL70: if((int)_tmpF1 != 2)goto _LL72;_LL71: return(void*)1;
_LL72: if(_tmpF1 <= (void*)3)goto _LL74;if(*((int*)_tmpF1)!= 3)goto _LL74;_LL73:
return(void*)2;_LL74:;_LL75:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4[0]=({struct Cyc_Core_Impossible_struct
_tmpF5;_tmpF5.tag=Cyc_Core_Impossible;_tmpF5.f1=({const char*_tmpF6="initlevel_approx";
_tag_dyneither(_tmpF6,sizeof(char),17);});_tmpF5;});_tmpF4;}));_LL67:;}static
void*Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,
void*acc){void*this_ans;if(acc == (void*)0)return(void*)0;{void*_tmpF7=r;int
_tmpF8;struct _dyneither_ptr _tmpF9;int _tmpFA;struct _dyneither_ptr _tmpFB;struct Cyc_CfFlowInfo_Place*
_tmpFC;_LL77: if(_tmpF7 <= (void*)3)goto _LL7D;if(*((int*)_tmpF7)!= 4)goto _LL79;
_tmpF8=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpF7)->f1;if(_tmpF8 != 1)goto
_LL79;_tmpF9=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpF7)->f2;_LL78: {
unsigned int _tmpFD=_get_dyneither_size(_tmpF9,sizeof(void*));this_ans=(void*)0;{
int i=0;for(0;i < _tmpFD;++ i){this_ans=Cyc_CfFlowInfo_initlevel_rec(env,*((void**)
_check_dyneither_subscript(_tmpF9,sizeof(void*),i)),(void*)2);}}goto _LL76;}_LL79:
if(*((int*)_tmpF7)!= 4)goto _LL7B;_tmpFA=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpF7)->f1;if(_tmpFA != 0)goto _LL7B;_tmpFB=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpF7)->f2;_LL7A: {unsigned int _tmpFE=_get_dyneither_size(_tmpFB,sizeof(void*));
this_ans=(void*)2;{int i=0;for(0;i < _tmpFE;++ i){this_ans=Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmpFB,sizeof(void*),i)),this_ans);}}
goto _LL76;}_LL7B: if(*((int*)_tmpF7)!= 2)goto _LL7D;_tmpFC=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpF7)->f1;_LL7C: if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct
Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(
Cyc_CfFlowInfo_place_cmp,env->seen,_tmpFC))this_ans=(void*)2;else{env->seen=({
struct Cyc_List_List*_tmpFF=_region_malloc(env->e,sizeof(*_tmpFF));_tmpFF->hd=
_tmpFC;_tmpFF->tl=env->seen;_tmpFF;});this_ans=Cyc_CfFlowInfo_initlevel_rec(env,
Cyc_CfFlowInfo_lookup_place(env->d,_tmpFC),(void*)2);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == (void*)0)this_ans=(void*)1;}goto _LL76;
_LL7D:;_LL7E: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL76:;}if(this_ans == (
void*)0)return(void*)0;if(this_ans == (void*)1  || acc == (void*)1)return(void*)1;
return(void*)2;}void*Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,
struct Cyc_Dict_Dict d,void*r){struct _RegionHandle*_tmp100=env->r;struct Cyc_CfFlowInfo_InitlevelEnv
_tmp101=({struct Cyc_CfFlowInfo_InitlevelEnv _tmp102;_tmp102.e=_tmp100;_tmp102.d=d;
_tmp102.seen=0;_tmp102;});return Cyc_CfFlowInfo_initlevel_rec(& _tmp101,r,(void*)2);}
struct _tuple14{void*f1;int f2;};void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place){void*_tmp104;struct Cyc_List_List*_tmp105;
struct Cyc_CfFlowInfo_Place _tmp103=*place;_tmp104=(void*)_tmp103.root;_tmp105=
_tmp103.fields;{void*_tmp106=Cyc_Dict_lookup(d,_tmp104);for(0;_tmp105 != 0;
_tmp105=_tmp105->tl){struct _tuple14 _tmp108=({struct _tuple14 _tmp107;_tmp107.f1=
_tmp106;_tmp107.f2=(int)_tmp105->hd;_tmp107;});void*_tmp109;int _tmp10A;struct
_dyneither_ptr _tmp10B;int _tmp10C;_LL80: _tmp109=_tmp108.f1;if(_tmp109 <= (void*)3)
goto _LL82;if(*((int*)_tmp109)!= 4)goto _LL82;_tmp10A=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp109)->f1;_tmp10B=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp109)->f2;
_tmp10C=_tmp108.f2;_LL81: _tmp106=*((void**)_check_dyneither_subscript(_tmp10B,
sizeof(void*),_tmp10C));goto _LL7F;_LL82:;_LL83:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D[0]=({struct Cyc_Core_Impossible_struct
_tmp10E;_tmp10E.tag=Cyc_Core_Impossible;_tmp10E.f1=({const char*_tmp10F="bad lookup_place";
_tag_dyneither(_tmp10F,sizeof(char),17);});_tmp10E;});_tmp10D;}));_LL7F:;}return
_tmp106;}}static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){void*_tmp110=rval;
int _tmp111;struct _dyneither_ptr _tmp112;_LL85: if(_tmp110 <= (void*)3)goto _LL89;if(*((
int*)_tmp110)!= 1)goto _LL87;_LL86: return 0;_LL87: if(*((int*)_tmp110)!= 4)goto
_LL89;_tmp111=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp110)->f1;_tmp112=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp110)->f2;_LL88: {unsigned int sz=
_get_dyneither_size(_tmp112,sizeof(void*));{int i=0;for(0;i < sz;++ i){if(!Cyc_CfFlowInfo_is_rval_unescaped(((
void**)_tmp112.curr)[i]))return 0;}}return 1;}_LL89:;_LL8A: return 1;_LL84:;}int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){return Cyc_CfFlowInfo_is_rval_unescaped(
Cyc_CfFlowInfo_lookup_place(d,place));}struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,void*r){struct _RegionHandle*_tmp113=fenv->r;struct Cyc_CfFlowInfo_EscPile*
pile=({struct Cyc_CfFlowInfo_EscPile*_tmp114=_region_malloc(_tmp113,sizeof(*
_tmp114));_tmp114->rgn=_tmp113;_tmp114->places=0;_tmp114;});Cyc_CfFlowInfo_add_places(
pile,r);return Cyc_CfFlowInfo_escape_these(fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{
struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict
d;struct Cyc_Position_Segment*loc;};static void*Cyc_CfFlowInfo_assign_place_inner(
struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){struct _tuple0 _tmp116=({
struct _tuple0 _tmp115;_tmp115.f1=oldval;_tmp115.f2=newval;_tmp115;});void*_tmp117;
void*_tmp118;struct Cyc_CfFlowInfo_Place*_tmp119;void*_tmp11A;void*_tmp11B;int
_tmp11C;struct _dyneither_ptr _tmp11D;void*_tmp11E;int _tmp11F;struct _dyneither_ptr
_tmp120;void*_tmp121;void*_tmp122;_LL8C: _tmp117=_tmp116.f1;if(_tmp117 <= (void*)3)
goto _LL8E;if(*((int*)_tmp117)!= 1)goto _LL8E;_tmp118=_tmp116.f2;if(_tmp118 <= (
void*)3)goto _LL8E;if(*((int*)_tmp118)!= 2)goto _LL8E;_tmp119=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp118)->f1;_LL8D: Cyc_CfFlowInfo_add_place(env->pile,_tmp119);goto _LL8F;_LL8E:
_tmp11A=_tmp116.f1;if(_tmp11A <= (void*)3)goto _LL90;if(*((int*)_tmp11A)!= 1)goto
_LL90;_LL8F: if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= (void*)2)({
void*_tmp123=0;Cyc_Tcutil_terr(env->loc,({const char*_tmp124="assignment puts possibly-uninitialized data in an escaped location";
_tag_dyneither(_tmp124,sizeof(char),67);}),_tag_dyneither(_tmp123,sizeof(void*),
0));});return(void*)(env->fenv)->esc_all;_LL90: _tmp11B=_tmp116.f1;if(_tmp11B <= (
void*)3)goto _LL92;if(*((int*)_tmp11B)!= 4)goto _LL92;_tmp11C=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp11B)->f1;_tmp11D=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp11B)->f2;
_tmp11E=_tmp116.f2;if(_tmp11E <= (void*)3)goto _LL92;if(*((int*)_tmp11E)!= 4)goto
_LL92;_tmp11F=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp11E)->f1;_tmp120=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp11E)->f2;_LL91: {struct _dyneither_ptr
new_d=({unsigned int _tmp127=_get_dyneither_size(_tmp11D,sizeof(void*));void**
_tmp128=(void**)_region_malloc((env->fenv)->r,_check_times(sizeof(void*),_tmp127));
struct _dyneither_ptr _tmp12A=_tag_dyneither(_tmp128,sizeof(void*),_tmp127);{
unsigned int _tmp129=_tmp127;unsigned int i;for(i=0;i < _tmp129;i ++){_tmp128[i]=Cyc_CfFlowInfo_assign_place_inner(
env,((void**)_tmp11D.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp120,
sizeof(void*),(int)i)));}}_tmp12A;});int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp11D,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp11D.curr)[i]){
change=1;break;}}}if(!change)return oldval;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp11D,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp120,sizeof(void*),i))){change=1;break;}}}if(!
change)return newval;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp125=
_region_malloc((env->fenv)->r,sizeof(*_tmp125));_tmp125[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp126;_tmp126.tag=4;_tmp126.f1=_tmp11C;_tmp126.f2=new_d;_tmp126;});_tmp125;});}
_LL92: _tmp121=_tmp116.f2;if(_tmp121 <= (void*)3)goto _LL94;if(*((int*)_tmp121)!= 1)
goto _LL94;_tmp122=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp121)->f1;_LL93: {
void*_tmp12B=_tmp122;_LL97: if((int)_tmp12B != 0)goto _LL99;_LL98: return(void*)(env->fenv)->unknown_none;
_LL99: if((int)_tmp12B != 1)goto _LL9B;_LL9A: return(void*)(env->fenv)->unknown_this;
_LL9B: if((int)_tmp12B != 2)goto _LL96;_LL9C: return(void*)(env->fenv)->unknown_all;
_LL96:;}_LL94:;_LL95: return newval;_LL8B:;}static void*Cyc_CfFlowInfo_assign_place_outer(
struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,void*oldval,void*
newval){if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple13 _tmp12D=({struct _tuple13 _tmp12C;_tmp12C.f1=fs;_tmp12C.f2=oldval;
_tmp12C;});struct Cyc_List_List*_tmp12E;struct Cyc_List_List _tmp12F;int _tmp130;
struct Cyc_List_List*_tmp131;void*_tmp132;int _tmp133;struct _dyneither_ptr _tmp134;
_LL9E: _tmp12E=_tmp12D.f1;if(_tmp12E == 0)goto _LLA0;_tmp12F=*_tmp12E;_tmp130=(int)
_tmp12F.hd;_tmp131=_tmp12F.tl;_tmp132=_tmp12D.f2;if(_tmp132 <= (void*)3)goto _LLA0;
if(*((int*)_tmp132)!= 4)goto _LLA0;_tmp133=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp132)->f1;_tmp134=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp132)->f2;_LL9F: {
void*_tmp135=Cyc_CfFlowInfo_assign_place_outer(env,_tmp131,*((void**)
_check_dyneither_subscript(_tmp134,sizeof(void*),_tmp130)),newval);struct
_dyneither_ptr _tmp136=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp134,
_tmp130,_tmp135);if(_tmp136.curr == _tmp134.curr)return oldval;else{return(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp137=_region_malloc((env->fenv)->r,
sizeof(*_tmp137));_tmp137[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp138;
_tmp138.tag=4;_tmp138.f1=_tmp133;_tmp138.f2=_tmp136;_tmp138;});_tmp137;});}}
_LLA0:;_LLA1:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp139=
_cycalloc(sizeof(*_tmp139));_tmp139[0]=({struct Cyc_Core_Impossible_struct _tmp13A;
_tmp13A.tag=Cyc_Core_Impossible;_tmp13A.f1=({const char*_tmp13B="bad assign place";
_tag_dyneither(_tmp13B,sizeof(char),17);});_tmp13A;});_tmp139;}));_LL9D:;}}
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){((int(*)(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
all_changed,place,0);{struct _RegionHandle*_tmp13C=fenv->r;struct Cyc_CfFlowInfo_Place
_tmp13E;void*_tmp13F;struct Cyc_List_List*_tmp140;struct Cyc_CfFlowInfo_Place*
_tmp13D=place;_tmp13E=*_tmp13D;_tmp13F=(void*)_tmp13E.root;_tmp140=_tmp13E.fields;{
struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv _tmp141;
_tmp141.fenv=fenv;_tmp141.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp142=
_region_malloc(_tmp13C,sizeof(*_tmp142));_tmp142->rgn=_tmp13C;_tmp142->places=0;
_tmp142;});_tmp141.d=d;_tmp141.loc=loc;_tmp141;});void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmp140,Cyc_Dict_lookup(d,_tmp13F),r);return Cyc_CfFlowInfo_escape_these(fenv,
env.pile,all_changed,Cyc_Dict_insert(d,_tmp13F,newval));}}}struct Cyc_CfFlowInfo_JoinEnv{
struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict
d1;struct Cyc_Dict_Dict d2;};enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither
 = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{
struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict
chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2);static struct Cyc_List_List*Cyc_CfFlowInfo_join_tag_cmps(
struct _RegionHandle*r,struct Cyc_List_List*l1,struct Cyc_List_List*l2){if(l1 == l2)
return l1;{struct Cyc_List_List*_tmp143=0;for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp145;void*_tmp146;void*_tmp147;struct Cyc_CfFlowInfo_TagCmp*_tmp144=(struct Cyc_CfFlowInfo_TagCmp*)
l2->hd;_tmp145=*_tmp144;_tmp146=(void*)_tmp145.cmp;_tmp147=(void*)_tmp145.bd;{
int found=0;void*joined_cmp=(void*)10;{struct Cyc_List_List*_tmp148=l1;for(0;
_tmp148 != 0;_tmp148=_tmp148->tl){struct Cyc_CfFlowInfo_TagCmp _tmp14A;void*_tmp14B;
void*_tmp14C;struct Cyc_CfFlowInfo_TagCmp*_tmp149=(struct Cyc_CfFlowInfo_TagCmp*)
_tmp148->hd;_tmp14A=*_tmp149;_tmp14B=(void*)_tmp14A.cmp;_tmp14C=(void*)_tmp14A.bd;
if(Cyc_Tcutil_typecmp(_tmp147,_tmp14C)== 0){found=1;if(_tmp14B == _tmp146){
joined_cmp=_tmp14B;break;}}}}if(found)_tmp143=({struct Cyc_List_List*_tmp14D=
_region_malloc(r,sizeof(*_tmp14D));_tmp14D->hd=({struct Cyc_CfFlowInfo_TagCmp*
_tmp14E=_region_malloc(r,sizeof(*_tmp14E));_tmp14E->cmp=(void*)joined_cmp;
_tmp14E->bd=(void*)_tmp147;_tmp14E;});_tmp14D->tl=_tmp143;_tmp14D;});}}return
_tmp143;}}static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*
env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{struct _RegionHandle*_tmp14F=(
env->fenv)->r;{struct _tuple0 _tmp151=({struct _tuple0 _tmp150;_tmp150.f1=r1;_tmp150.f2=
r2;_tmp150;});void*_tmp152;struct Cyc_CfFlowInfo_Place*_tmp153;void*_tmp154;
struct Cyc_CfFlowInfo_Place*_tmp155;void*_tmp156;struct Cyc_CfFlowInfo_Place*
_tmp157;void*_tmp158;struct Cyc_CfFlowInfo_Place*_tmp159;void*_tmp15A;void*
_tmp15B;void*_tmp15C;void*_tmp15D;void*_tmp15E;int _tmp15F;struct _dyneither_ptr
_tmp160;void*_tmp161;int _tmp162;struct _dyneither_ptr _tmp163;void*_tmp164;struct
Cyc_List_List*_tmp165;void*_tmp166;struct Cyc_List_List*_tmp167;void*_tmp168;void*
_tmp169;_LLA3: _tmp152=_tmp151.f1;if(_tmp152 <= (void*)3)goto _LLA5;if(*((int*)
_tmp152)!= 2)goto _LLA5;_tmp153=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp152)->f1;
_tmp154=_tmp151.f2;if(_tmp154 <= (void*)3)goto _LLA5;if(*((int*)_tmp154)!= 2)goto
_LLA5;_tmp155=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp154)->f1;_LLA4: if(Cyc_CfFlowInfo_place_cmp(
_tmp153,_tmp155)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmp153);Cyc_CfFlowInfo_add_place(
env->pile,_tmp155);goto _LLA2;_LLA5: _tmp156=_tmp151.f1;if(_tmp156 <= (void*)3)goto
_LLA7;if(*((int*)_tmp156)!= 2)goto _LLA7;_tmp157=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp156)->f1;_LLA6: Cyc_CfFlowInfo_add_place(env->pile,_tmp157);goto _LLA2;_LLA7:
_tmp158=_tmp151.f2;if(_tmp158 <= (void*)3)goto _LLA9;if(*((int*)_tmp158)!= 2)goto
_LLA9;_tmp159=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp158)->f1;_LLA8: Cyc_CfFlowInfo_add_place(
env->pile,_tmp159);goto _LLA2;_LLA9: _tmp15A=_tmp151.f1;if((int)_tmp15A != 1)goto
_LLAB;_tmp15B=_tmp151.f2;if((int)_tmp15B != 2)goto _LLAB;_LLAA: goto _LLAC;_LLAB:
_tmp15C=_tmp151.f1;if((int)_tmp15C != 2)goto _LLAD;_tmp15D=_tmp151.f2;if((int)
_tmp15D != 1)goto _LLAD;_LLAC: return(void*)2;_LLAD: _tmp15E=_tmp151.f1;if(_tmp15E <= (
void*)3)goto _LLAF;if(*((int*)_tmp15E)!= 4)goto _LLAF;_tmp15F=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp15E)->f1;_tmp160=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp15E)->f2;
_tmp161=_tmp151.f2;if(_tmp161 <= (void*)3)goto _LLAF;if(*((int*)_tmp161)!= 4)goto
_LLAF;_tmp162=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp161)->f1;_tmp163=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp161)->f2;_LLAE: {struct _dyneither_ptr
new_d=({unsigned int _tmp16C=_get_dyneither_size(_tmp160,sizeof(void*));void**
_tmp16D=(void**)_region_malloc(_tmp14F,_check_times(sizeof(void*),_tmp16C));
struct _dyneither_ptr _tmp16F=_tag_dyneither(_tmp16D,sizeof(void*),_tmp16C);{
unsigned int _tmp16E=_tmp16C;unsigned int i;for(i=0;i < _tmp16E;i ++){_tmp16D[i]=((
void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(
env,0,((void**)_tmp160.curr)[(int)i],*((void**)_check_dyneither_subscript(
_tmp163,sizeof(void*),(int)i)));}}_tmp16F;});int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp160,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp160.curr)[i]){
change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp160,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp163,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp16A=
_region_malloc(_tmp14F,sizeof(*_tmp16A));_tmp16A[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp16B;_tmp16B.tag=4;_tmp16B.f1=_tmp15F;_tmp16B.f2=new_d;_tmp16B;});_tmp16A;});}
_LLAF: _tmp164=_tmp151.f1;if(_tmp164 <= (void*)3)goto _LLB1;if(*((int*)_tmp164)!= 3)
goto _LLB1;_tmp165=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp164)->f1;_tmp166=
_tmp151.f2;if(_tmp166 <= (void*)3)goto _LLB1;if(*((int*)_tmp166)!= 3)goto _LLB1;
_tmp167=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp166)->f1;_LLB0: {struct Cyc_List_List*
_tmp170=Cyc_CfFlowInfo_join_tag_cmps(_tmp14F,_tmp165,_tmp167);if(_tmp170 == 
_tmp165)return r1;return(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*_tmp171=
_region_malloc(_tmp14F,sizeof(*_tmp171));_tmp171[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmp172;_tmp172.tag=3;_tmp172.f1=_tmp170;_tmp172;});_tmp171;});}_LLB1: _tmp168=
_tmp151.f1;if(_tmp168 <= (void*)3)goto _LLB3;if(*((int*)_tmp168)!= 3)goto _LLB3;
_LLB2: return r2;_LLB3: _tmp169=_tmp151.f2;if(_tmp169 <= (void*)3)goto _LLB5;if(*((
int*)_tmp169)!= 3)goto _LLB5;_LLB4: return r1;_LLB5:;_LLB6: goto _LLA2;_LLA2:;}{void*
il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);void*il2=Cyc_CfFlowInfo_initlevel(
env->fenv,env->d2,r2);struct _tuple0 _tmp174=({struct _tuple0 _tmp173;_tmp173.f1=r1;
_tmp173.f2=r2;_tmp173;});void*_tmp175;void*_tmp176;_LLB8: _tmp175=_tmp174.f1;if(
_tmp175 <= (void*)3)goto _LLBA;if(*((int*)_tmp175)!= 1)goto _LLBA;_LLB9: goto _LLBB;
_LLBA: _tmp176=_tmp174.f2;if(_tmp176 <= (void*)3)goto _LLBC;if(*((int*)_tmp176)!= 1)
goto _LLBC;_LLBB: {struct _tuple0 _tmp178=({struct _tuple0 _tmp177;_tmp177.f1=il1;
_tmp177.f2=il2;_tmp177;});void*_tmp179;void*_tmp17A;void*_tmp17B;void*_tmp17C;
_LLBF: _tmp179=_tmp178.f2;if((int)_tmp179 != 0)goto _LLC1;_LLC0: goto _LLC2;_LLC1:
_tmp17A=_tmp178.f1;if((int)_tmp17A != 0)goto _LLC3;_LLC2: return(void*)(env->fenv)->esc_none;
_LLC3: _tmp17B=_tmp178.f2;if((int)_tmp17B != 1)goto _LLC5;_LLC4: goto _LLC6;_LLC5:
_tmp17C=_tmp178.f1;if((int)_tmp17C != 1)goto _LLC7;_LLC6: return(void*)(env->fenv)->esc_this;
_LLC7:;_LLC8: return(void*)(env->fenv)->esc_all;_LLBE:;}_LLBC:;_LLBD: {struct
_tuple0 _tmp17E=({struct _tuple0 _tmp17D;_tmp17D.f1=il1;_tmp17D.f2=il2;_tmp17D;});
void*_tmp17F;void*_tmp180;void*_tmp181;void*_tmp182;_LLCA: _tmp17F=_tmp17E.f2;if((
int)_tmp17F != 0)goto _LLCC;_LLCB: goto _LLCD;_LLCC: _tmp180=_tmp17E.f1;if((int)
_tmp180 != 0)goto _LLCE;_LLCD: return(void*)(env->fenv)->unknown_none;_LLCE: _tmp181=
_tmp17E.f2;if((int)_tmp181 != 1)goto _LLD0;_LLCF: goto _LLD1;_LLD0: _tmp182=_tmp17E.f1;
if((int)_tmp182 != 1)goto _LLD2;_LLD1: return(void*)(env->fenv)->unknown_this;_LLD2:;
_LLD3: return(void*)(env->fenv)->unknown_all;_LLC9:;}_LLB7:;}}}struct _tuple15{
union Cyc_CfFlowInfo_RelnOp_union f1;union Cyc_CfFlowInfo_RelnOp_union f2;};static
int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp_union r1,union Cyc_CfFlowInfo_RelnOp_union
r2){struct _tuple15 _tmp184=({struct _tuple15 _tmp183;_tmp183.f1=r1;_tmp183.f2=r2;
_tmp183;});union Cyc_CfFlowInfo_RelnOp_union _tmp185;unsigned int _tmp186;union Cyc_CfFlowInfo_RelnOp_union
_tmp187;unsigned int _tmp188;union Cyc_CfFlowInfo_RelnOp_union _tmp189;struct Cyc_Absyn_Vardecl*
_tmp18A;union Cyc_CfFlowInfo_RelnOp_union _tmp18B;struct Cyc_Absyn_Vardecl*_tmp18C;
union Cyc_CfFlowInfo_RelnOp_union _tmp18D;struct Cyc_Absyn_Vardecl*_tmp18E;union Cyc_CfFlowInfo_RelnOp_union
_tmp18F;struct Cyc_Absyn_Vardecl*_tmp190;union Cyc_CfFlowInfo_RelnOp_union _tmp191;
unsigned int _tmp192;union Cyc_CfFlowInfo_RelnOp_union _tmp193;unsigned int _tmp194;
union Cyc_CfFlowInfo_RelnOp_union _tmp195;struct Cyc_Absyn_Vardecl*_tmp196;union Cyc_CfFlowInfo_RelnOp_union
_tmp197;struct Cyc_Absyn_Vardecl*_tmp198;_LLD5: _tmp185=_tmp184.f1;if(((_tmp184.f1).EqualConst).tag
!= 0)goto _LLD7;_tmp186=(_tmp185.EqualConst).f1;_tmp187=_tmp184.f2;if(((_tmp184.f2).EqualConst).tag
!= 0)goto _LLD7;_tmp188=(_tmp187.EqualConst).f1;_LLD6: return _tmp186 == _tmp188;
_LLD7: _tmp189=_tmp184.f1;if(((_tmp184.f1).LessVar).tag != 1)goto _LLD9;_tmp18A=(
_tmp189.LessVar).f1;_tmp18B=_tmp184.f2;if(((_tmp184.f2).LessVar).tag != 1)goto
_LLD9;_tmp18C=(_tmp18B.LessVar).f1;_LLD8: return _tmp18A == _tmp18C;_LLD9: _tmp18D=
_tmp184.f1;if(((_tmp184.f1).LessNumelts).tag != 2)goto _LLDB;_tmp18E=(_tmp18D.LessNumelts).f1;
_tmp18F=_tmp184.f2;if(((_tmp184.f2).LessNumelts).tag != 2)goto _LLDB;_tmp190=(
_tmp18F.LessNumelts).f1;_LLDA: return _tmp18E == _tmp190;_LLDB: _tmp191=_tmp184.f1;
if(((_tmp184.f1).LessConst).tag != 3)goto _LLDD;_tmp192=(_tmp191.LessConst).f1;
_tmp193=_tmp184.f2;if(((_tmp184.f2).LessConst).tag != 3)goto _LLDD;_tmp194=(
_tmp193.LessConst).f1;_LLDC: return _tmp192 == _tmp194;_LLDD: _tmp195=_tmp184.f1;if(((
_tmp184.f1).LessEqNumelts).tag != 4)goto _LLDF;_tmp196=(_tmp195.LessEqNumelts).f1;
_tmp197=_tmp184.f2;if(((_tmp184.f2).LessEqNumelts).tag != 4)goto _LLDF;_tmp198=(
_tmp197.LessEqNumelts).f1;_LLDE: return _tmp196 == _tmp198;_LLDF:;_LLE0: return 0;
_LLD4:;}static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct _RegionHandle*
r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){if(r1s == r2s)return r1s;{
struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*_tmp199=r1s;for(0;
_tmp199 != 0;_tmp199=_tmp199->tl){struct Cyc_CfFlowInfo_Reln*_tmp19A=(struct Cyc_CfFlowInfo_Reln*)
_tmp199->hd;int found=0;{struct Cyc_List_List*_tmp19B=r2s;for(0;_tmp19B != 0;
_tmp19B=_tmp19B->tl){struct Cyc_CfFlowInfo_Reln*_tmp19C=(struct Cyc_CfFlowInfo_Reln*)
_tmp19B->hd;if(_tmp19A == _tmp19C  || _tmp19A->vd == _tmp19C->vd  && Cyc_CfFlowInfo_same_relop(
_tmp19A->rop,_tmp19C->rop)){res=({struct Cyc_List_List*_tmp19D=_region_malloc(r,
sizeof(*_tmp19D));_tmp19D->hd=_tmp19A;_tmp19D->tl=res;_tmp19D;});found=1;break;}}}
if(!found)diff=1;}}if(!diff)res=r1s;return res;}}static void Cyc_CfFlowInfo_remove_f(
struct Cyc_List_List**l,struct Cyc_CfFlowInfo_Place*place){struct _handler_cons
_tmp19E;_push_handler(& _tmp19E);{int _tmp1A0=0;if(setjmp(_tmp19E.handler))_tmp1A0=
1;if(!_tmp1A0){*l=((struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_delete_cmp)(Cyc_CfFlowInfo_place_cmp,*l,place);;_pop_handler();}else{
void*_tmp19F=(void*)_exn_thrown;void*_tmp1A2=_tmp19F;_LLE2: if(_tmp1A2 != Cyc_Core_Not_found)
goto _LLE4;_LLE3: goto _LLE1;_LLE4:;_LLE5:(void)_throw(_tmp1A2);_LLE1:;}}}static
struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_or_consume(struct _RegionHandle*r,
struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2){struct Cyc_List_List*
mc=c2.may_consume;struct _RegionHandle*_tmp1A3=r;{struct Cyc_List_List*_tmp1A4=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(
_tmp1A3,c1.may_consume);((void(*)(void(*f)(struct Cyc_List_List**,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List**env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_CfFlowInfo_remove_f,&
_tmp1A4,c2.may_consume);while(_tmp1A4 != 0){mc=({struct Cyc_List_List*_tmp1A5=
_region_malloc(r,sizeof(*_tmp1A5));_tmp1A5->hd=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmp1A4))->hd;_tmp1A5->tl=mc;_tmp1A5;});_tmp1A4=((
struct Cyc_List_List*)_check_null(_tmp1A4))->tl;}}return({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1A6;_tmp1A6.consumed=Cyc_CfFlowInfo_union_place_set(c1.consumed,c2.consumed,
0);_tmp1A6.may_consume=mc;_tmp1A6;});}struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(
struct _RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*_tmp1A7=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,c1.may_consume,
c2.may_consume);return({struct Cyc_CfFlowInfo_ConsumeInfo _tmp1A8;_tmp1A8.consumed=
Cyc_CfFlowInfo_union_place_set(c1.consumed,c2.consumed,1);_tmp1A8.may_consume=
_tmp1A7;_tmp1A8;});}struct _tuple16{union Cyc_CfFlowInfo_FlowInfo_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo_union f1,union
Cyc_CfFlowInfo_FlowInfo_union f2,int or_consumed){struct _tuple16 _tmp1AA=({struct
_tuple16 _tmp1A9;_tmp1A9.f1=f1;_tmp1A9.f2=f2;_tmp1A9;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp1AB;union Cyc_CfFlowInfo_FlowInfo_union _tmp1AC;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1AD;struct Cyc_Dict_Dict _tmp1AE;struct Cyc_List_List*_tmp1AF;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1B0;union Cyc_CfFlowInfo_FlowInfo_union _tmp1B1;struct Cyc_Dict_Dict _tmp1B2;
struct Cyc_List_List*_tmp1B3;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1B4;_LLE7:
_tmp1AB=_tmp1AA.f1;if(((_tmp1AA.f1).BottomFL).tag != 0)goto _LLE9;_LLE8: return f2;
_LLE9: _tmp1AC=_tmp1AA.f2;if(((_tmp1AA.f2).BottomFL).tag != 0)goto _LLEB;_LLEA:
return f1;_LLEB: _tmp1AD=_tmp1AA.f1;if(((_tmp1AA.f1).ReachableFL).tag != 1)goto
_LLE6;_tmp1AE=(_tmp1AD.ReachableFL).f1;_tmp1AF=(_tmp1AD.ReachableFL).f2;_tmp1B0=(
_tmp1AD.ReachableFL).f3;_tmp1B1=_tmp1AA.f2;if(((_tmp1AA.f2).ReachableFL).tag != 1)
goto _LLE6;_tmp1B2=(_tmp1B1.ReachableFL).f1;_tmp1B3=(_tmp1B1.ReachableFL).f2;
_tmp1B4=(_tmp1B1.ReachableFL).f3;_LLEC: if(((_tmp1AE.t == _tmp1B2.t  && _tmp1AF == 
_tmp1B3) && _tmp1B0.may_consume == _tmp1B4.may_consume) && (_tmp1B0.consumed).t == (
_tmp1B4.consumed).t)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))
return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle*_tmp1B5=fenv->r;struct Cyc_CfFlowInfo_JoinEnv _tmp1B6=({struct Cyc_CfFlowInfo_JoinEnv
_tmp1BB;_tmp1BB.fenv=fenv;_tmp1BB.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp1BC=
_region_malloc(_tmp1B5,sizeof(*_tmp1BC));_tmp1BC->rgn=_tmp1B5;_tmp1BC->places=0;
_tmp1BC;});_tmp1BB.d1=_tmp1AE;_tmp1BB.d2=_tmp1B2;_tmp1BB;});struct Cyc_Dict_Dict
_tmp1B7=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,
void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_absRval,& _tmp1B6,_tmp1AE,_tmp1B2);
struct Cyc_List_List*_tmp1B8=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp1AF,_tmp1B3);
struct Cyc_CfFlowInfo_ConsumeInfo _tmp1B9=or_consumed?Cyc_CfFlowInfo_or_consume(
fenv->r,_tmp1B0,_tmp1B4): Cyc_CfFlowInfo_and_consume(fenv->r,_tmp1B0,_tmp1B4);
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1BA;(_tmp1BA.ReachableFL).tag=1;(_tmp1BA.ReachableFL).f1=Cyc_CfFlowInfo_escape_these(
fenv,_tmp1B6.pile,all_changed,_tmp1B7);(_tmp1BA.ReachableFL).f2=_tmp1B8;(_tmp1BA.ReachableFL).f3=
_tmp1B9;_tmp1BA;});}_LLE6:;}struct _tuple17{union Cyc_CfFlowInfo_FlowInfo_union f1;
union Cyc_CfFlowInfo_FlowInfo_union f2;union Cyc_CfFlowInfo_FlowInfo_union f3;};
struct _tuple5 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict*all_changed,struct _tuple5 pr1,struct _tuple5 pr2,int or_consumed){
union Cyc_CfFlowInfo_FlowInfo_union _tmp1BE;void*_tmp1BF;struct _tuple5 _tmp1BD=pr1;
_tmp1BE=_tmp1BD.f1;_tmp1BF=_tmp1BD.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp1C1;
void*_tmp1C2;struct _tuple5 _tmp1C0=pr2;_tmp1C1=_tmp1C0.f1;_tmp1C2=_tmp1C0.f2;{
union Cyc_CfFlowInfo_FlowInfo_union outflow=Cyc_CfFlowInfo_join_flow(fenv,
all_changed,_tmp1BE,_tmp1C1,or_consumed);struct _tuple17 _tmp1C4=({struct _tuple17
_tmp1C3;_tmp1C3.f1=_tmp1BE;_tmp1C3.f2=_tmp1C1;_tmp1C3.f3=outflow;_tmp1C3;});
union Cyc_CfFlowInfo_FlowInfo_union _tmp1C5;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1C6;union Cyc_CfFlowInfo_FlowInfo_union _tmp1C7;struct Cyc_Dict_Dict _tmp1C8;
union Cyc_CfFlowInfo_FlowInfo_union _tmp1C9;struct Cyc_Dict_Dict _tmp1CA;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1CB;struct Cyc_Dict_Dict _tmp1CC;struct Cyc_List_List*_tmp1CD;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1CE;_LLEE: _tmp1C5=_tmp1C4.f1;if(((_tmp1C4.f1).BottomFL).tag != 0)goto _LLF0;
_LLEF: return({struct _tuple5 _tmp1CF;_tmp1CF.f1=outflow;_tmp1CF.f2=_tmp1C2;_tmp1CF;});
_LLF0: _tmp1C6=_tmp1C4.f2;if(((_tmp1C4.f2).BottomFL).tag != 0)goto _LLF2;_LLF1:
return({struct _tuple5 _tmp1D0;_tmp1D0.f1=outflow;_tmp1D0.f2=_tmp1BF;_tmp1D0;});
_LLF2: _tmp1C7=_tmp1C4.f1;if(((_tmp1C4.f1).ReachableFL).tag != 1)goto _LLF4;_tmp1C8=(
_tmp1C7.ReachableFL).f1;_tmp1C9=_tmp1C4.f2;if(((_tmp1C4.f2).ReachableFL).tag != 1)
goto _LLF4;_tmp1CA=(_tmp1C9.ReachableFL).f1;_tmp1CB=_tmp1C4.f3;if(((_tmp1C4.f3).ReachableFL).tag
!= 1)goto _LLF4;_tmp1CC=(_tmp1CB.ReachableFL).f1;_tmp1CD=(_tmp1CB.ReachableFL).f2;
_tmp1CE=(_tmp1CB.ReachableFL).f3;_LLF3: if(((int(*)(int ignore,void*r1,void*r2))
Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp1BF,_tmp1C2))return({struct _tuple5
_tmp1D1;_tmp1D1.f1=outflow;_tmp1D1.f2=_tmp1C2;_tmp1D1;});if(((int(*)(int ignore,
void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp1C2,_tmp1BF))
return({struct _tuple5 _tmp1D2;_tmp1D2.f1=outflow;_tmp1D2.f2=_tmp1BF;_tmp1D2;});{
struct _RegionHandle*_tmp1D3=fenv->r;struct Cyc_CfFlowInfo_JoinEnv _tmp1D4=({struct
Cyc_CfFlowInfo_JoinEnv _tmp1D8;_tmp1D8.fenv=fenv;_tmp1D8.pile=({struct Cyc_CfFlowInfo_EscPile*
_tmp1D9=_region_malloc(_tmp1D3,sizeof(*_tmp1D9));_tmp1D9->rgn=_tmp1D3;_tmp1D9->places=
0;_tmp1D9;});_tmp1D8.d1=_tmp1C8;_tmp1D8.d2=_tmp1CA;_tmp1D8;});void*_tmp1D5=((
void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(&
_tmp1D4,0,_tmp1BF,_tmp1C2);return({struct _tuple5 _tmp1D6;_tmp1D6.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1D7;(_tmp1D7.ReachableFL).tag=1;(_tmp1D7.ReachableFL).f1=
Cyc_CfFlowInfo_escape_these(fenv,_tmp1D4.pile,all_changed,_tmp1CC);(_tmp1D7.ReachableFL).f2=
_tmp1CD;(_tmp1D7.ReachableFL).f3=_tmp1CE;_tmp1D7;});_tmp1D6.f2=_tmp1D5;_tmp1D6;});}
_LLF4:;_LLF5:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1DA=
_cycalloc(sizeof(*_tmp1DA));_tmp1DA[0]=({struct Cyc_Core_Impossible_struct _tmp1DB;
_tmp1DB.tag=Cyc_Core_Impossible;_tmp1DB.f1=({const char*_tmp1DC="join_flow_and_rval: BottomFL outflow";
_tag_dyneither(_tmp1DC,sizeof(char),37);});_tmp1DB;});_tmp1DA;}));_LLED:;}}}
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,int,
void*,void*);static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*
env,void*r1,void*r2){int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,
env->curr_place);int changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,
env->curr_place);if(changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
if(changed1){if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))
return r1;env->changed=Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(
env->curr_place,env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{struct
_tuple0 _tmp1DE=({struct _tuple0 _tmp1DD;_tmp1DD.f1=r1;_tmp1DD.f2=r2;_tmp1DD;});
void*_tmp1DF;int _tmp1E0;struct _dyneither_ptr _tmp1E1;void*_tmp1E2;int _tmp1E3;
struct _dyneither_ptr _tmp1E4;_LLF7: _tmp1DF=_tmp1DE.f1;if(_tmp1DF <= (void*)3)goto
_LLF9;if(*((int*)_tmp1DF)!= 4)goto _LLF9;_tmp1E0=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1DF)->f1;_tmp1E1=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1DF)->f2;
_tmp1E2=_tmp1DE.f2;if(_tmp1E2 <= (void*)3)goto _LLF9;if(*((int*)_tmp1E2)!= 4)goto
_LLF9;_tmp1E3=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1E2)->f1;_tmp1E4=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1E2)->f2;_LLF8: {struct _dyneither_ptr
new_d=({unsigned int _tmp1E7=_get_dyneither_size(_tmp1E1,sizeof(void*));void**
_tmp1E8=(void**)_region_malloc(((env->joinenv).fenv)->r,_check_times(sizeof(void*),
_tmp1E7));struct _dyneither_ptr _tmp1EA=_tag_dyneither(_tmp1E8,sizeof(void*),
_tmp1E7);{unsigned int _tmp1E9=_tmp1E7;unsigned int i;for(i=0;i < _tmp1E9;i ++){
_tmp1E8[i]=Cyc_CfFlowInfo_after_absRval_d(env,(int)i,((void**)_tmp1E1.curr)[(int)
i],*((void**)_check_dyneither_subscript(_tmp1E4,sizeof(void*),(int)i)));}}
_tmp1EA;});int change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp1E1,sizeof(void*));
++ i){if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)
_tmp1E1.curr)[i]){change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i
< _get_dyneither_size(_tmp1E1,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp1E4,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1E5=
_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp1E5));_tmp1E5[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp1E6;_tmp1E6.tag=4;_tmp1E6.f1=_tmp1E0;_tmp1E6.f2=new_d;_tmp1E6;});_tmp1E5;});}
_LLF9:;_LLFA:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1EB=
_cycalloc(sizeof(*_tmp1EB));_tmp1EB[0]=({struct Cyc_Core_Impossible_struct _tmp1EC;
_tmp1EC.tag=Cyc_Core_Impossible;_tmp1EC.f1=({const char*_tmp1ED="after_absRval -- non-aggregates!";
_tag_dyneither(_tmp1ED,sizeof(char),33);});_tmp1EC;});_tmp1EB;}));_LLF6:;}}
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,int
key,void*r1,void*r2){if(r1 == r2)return r1;{struct Cyc_List_List**_tmp1EE=env->last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged _tmp1EF=env->changed;*env->last_field_cell=({
struct Cyc_List_List*_tmp1F0=_region_malloc(((env->joinenv).fenv)->r,sizeof(*
_tmp1F0));_tmp1F0->hd=(void*)key;_tmp1F0->tl=0;_tmp1F0;});env->last_field_cell=(
struct Cyc_List_List**)&((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{
void*_tmp1F1=Cyc_CfFlowInfo_after_absRval(env,r1,r2);env->last_field_cell=
_tmp1EE;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=0;env->changed=
_tmp1EF;return _tmp1F1;}}}static void*Cyc_CfFlowInfo_after_root(struct Cyc_CfFlowInfo_AfterEnv*
env,void*root,void*r1,void*r2){if(r1 == r2)return r1;*env->curr_place=({struct Cyc_CfFlowInfo_Place
_tmp1F2;_tmp1F2.root=(void*)root;_tmp1F2.fields=0;_tmp1F2;});env->last_field_cell=(
struct Cyc_List_List**)&(env->curr_place)->fields;env->changed=Cyc_CfFlowInfo_Neither;
return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*
all_changed,union Cyc_CfFlowInfo_FlowInfo_union f1,union Cyc_CfFlowInfo_FlowInfo_union
f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){struct _tuple16 _tmp1F4=({
struct _tuple16 _tmp1F3;_tmp1F3.f1=f1;_tmp1F3.f2=f2;_tmp1F3;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp1F5;union Cyc_CfFlowInfo_FlowInfo_union _tmp1F6;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1F7;struct Cyc_Dict_Dict _tmp1F8;struct Cyc_List_List*_tmp1F9;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1FA;union Cyc_CfFlowInfo_FlowInfo_union _tmp1FB;struct Cyc_Dict_Dict _tmp1FC;
struct Cyc_List_List*_tmp1FD;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1FE;_LLFC:
_tmp1F5=_tmp1F4.f1;if(((_tmp1F4.f1).BottomFL).tag != 0)goto _LLFE;_LLFD: goto _LLFF;
_LLFE: _tmp1F6=_tmp1F4.f2;if(((_tmp1F4.f2).BottomFL).tag != 0)goto _LL100;_LLFF:
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1FF;(_tmp1FF.BottomFL).tag=0;_tmp1FF;});_LL100: _tmp1F7=_tmp1F4.f1;if(((
_tmp1F4.f1).ReachableFL).tag != 1)goto _LLFB;_tmp1F8=(_tmp1F7.ReachableFL).f1;
_tmp1F9=(_tmp1F7.ReachableFL).f2;_tmp1FA=(_tmp1F7.ReachableFL).f3;_tmp1FB=
_tmp1F4.f2;if(((_tmp1F4.f2).ReachableFL).tag != 1)goto _LLFB;_tmp1FC=(_tmp1FB.ReachableFL).f1;
_tmp1FD=(_tmp1FB.ReachableFL).f2;_tmp1FE=(_tmp1FB.ReachableFL).f3;_LL101: if(((
_tmp1F8.t == _tmp1FC.t  && _tmp1F9 == _tmp1FD) && _tmp1FA.may_consume == _tmp1FE.may_consume)
 && (_tmp1FA.consumed).t == (_tmp1FE.consumed).t)return f1;{struct _RegionHandle*
_tmp200=fenv->r;struct Cyc_CfFlowInfo_Place*_tmp201=fenv->dummy_place;struct Cyc_CfFlowInfo_AfterEnv
_tmp202=({struct Cyc_CfFlowInfo_AfterEnv _tmp207;_tmp207.joinenv=({struct Cyc_CfFlowInfo_JoinEnv
_tmp208;_tmp208.fenv=fenv;_tmp208.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp209=
_region_malloc(_tmp200,sizeof(*_tmp209));_tmp209->rgn=_tmp200;_tmp209->places=0;
_tmp209;});_tmp208.d1=_tmp1F8;_tmp208.d2=_tmp1FC;_tmp208;});_tmp207.chg1=chg1;
_tmp207.chg2=chg2;_tmp207.curr_place=_tmp201;_tmp207.last_field_cell=(struct Cyc_List_List**)&
_tmp201->fields;_tmp207.changed=Cyc_CfFlowInfo_Neither;_tmp207;});struct Cyc_Dict_Dict
_tmp203=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,
void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,& _tmp202,_tmp1F8,_tmp1FC);
struct Cyc_List_List*_tmp204=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp1F9,_tmp1FD);
struct Cyc_CfFlowInfo_ConsumeInfo _tmp205=Cyc_CfFlowInfo_and_consume(fenv->r,
_tmp1FA,_tmp1FE);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp206;(_tmp206.ReachableFL).tag=1;(_tmp206.ReachableFL).f1=Cyc_CfFlowInfo_escape_these(
fenv,(_tmp202.joinenv).pile,all_changed,_tmp203);(_tmp206.ReachableFL).f2=
_tmp204;(_tmp206.ReachableFL).f3=_tmp205;_tmp206;});}_LLFB:;}static int Cyc_CfFlowInfo_tag_cmps_lessthan_approx(
struct Cyc_List_List*l1,struct Cyc_List_List*l2){for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp20B;void*_tmp20C;void*_tmp20D;struct Cyc_CfFlowInfo_TagCmp*_tmp20A=(struct Cyc_CfFlowInfo_TagCmp*)
l2->hd;_tmp20B=*_tmp20A;_tmp20C=(void*)_tmp20B.cmp;_tmp20D=(void*)_tmp20B.bd;{
struct Cyc_List_List*_tmp20E=l1;for(0;_tmp20E != 0;_tmp20E=_tmp20E->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp210;void*_tmp211;void*_tmp212;struct Cyc_CfFlowInfo_TagCmp*_tmp20F=(struct Cyc_CfFlowInfo_TagCmp*)
_tmp20E->hd;_tmp210=*_tmp20F;_tmp211=(void*)_tmp210.cmp;_tmp212=(void*)_tmp210.bd;
if(_tmp211 == _tmp20C  && Cyc_Tcutil_typecmp(_tmp212,_tmp20D)== 0)break;}if(
_tmp20E == 0)return 0;}}return 1;}static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp214=({struct
_tuple0 _tmp213;_tmp213.f1=r1;_tmp213.f2=r2;_tmp213;});void*_tmp215;struct Cyc_CfFlowInfo_Place*
_tmp216;void*_tmp217;struct Cyc_CfFlowInfo_Place*_tmp218;void*_tmp219;void*
_tmp21A;void*_tmp21B;int _tmp21C;struct _dyneither_ptr _tmp21D;void*_tmp21E;int
_tmp21F;struct _dyneither_ptr _tmp220;void*_tmp221;void*_tmp222;void*_tmp223;void*
_tmp224;void*_tmp225;void*_tmp226;void*_tmp227;struct Cyc_List_List*_tmp228;void*
_tmp229;struct Cyc_List_List*_tmp22A;void*_tmp22B;_LL103: _tmp215=_tmp214.f1;if(
_tmp215 <= (void*)3)goto _LL105;if(*((int*)_tmp215)!= 2)goto _LL105;_tmp216=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp215)->f1;_tmp217=_tmp214.f2;if(
_tmp217 <= (void*)3)goto _LL105;if(*((int*)_tmp217)!= 2)goto _LL105;_tmp218=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp217)->f1;_LL104: return Cyc_CfFlowInfo_place_cmp(
_tmp216,_tmp218)== 0;_LL105: _tmp219=_tmp214.f1;if(_tmp219 <= (void*)3)goto _LL107;
if(*((int*)_tmp219)!= 2)goto _LL107;_LL106: goto _LL108;_LL107: _tmp21A=_tmp214.f2;
if(_tmp21A <= (void*)3)goto _LL109;if(*((int*)_tmp21A)!= 2)goto _LL109;_LL108:
return 0;_LL109: _tmp21B=_tmp214.f1;if(_tmp21B <= (void*)3)goto _LL10B;if(*((int*)
_tmp21B)!= 4)goto _LL10B;_tmp21C=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp21B)->f1;
_tmp21D=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp21B)->f2;_tmp21E=_tmp214.f2;
if(_tmp21E <= (void*)3)goto _LL10B;if(*((int*)_tmp21E)!= 4)goto _LL10B;_tmp21F=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp21E)->f1;_tmp220=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp21E)->f2;_LL10A: if(_tmp21D.curr == _tmp220.curr)return 1;{int i=0;for(0;i < 
_get_dyneither_size(_tmp21D,sizeof(void*));++ i){if(!((int(*)(int ignore,void*r1,
void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp21D.curr)[i],*((
void**)_check_dyneither_subscript(_tmp220,sizeof(void*),i))))return 0;}}return 1;
_LL10B: _tmp221=_tmp214.f2;if((int)_tmp221 != 2)goto _LL10D;_LL10C: return r1 == (void*)
1;_LL10D: _tmp222=_tmp214.f2;if((int)_tmp222 != 0)goto _LL10F;_LL10E: goto _LL110;
_LL10F: _tmp223=_tmp214.f2;if((int)_tmp223 != 1)goto _LL111;_LL110: return 0;_LL111:
_tmp224=_tmp214.f1;if(_tmp224 <= (void*)3)goto _LL113;if(*((int*)_tmp224)!= 1)goto
_LL113;_tmp225=_tmp214.f2;if(_tmp225 <= (void*)3)goto _LL113;if(*((int*)_tmp225)!= 
1)goto _LL113;_LL112: goto _LL102;_LL113: _tmp226=_tmp214.f1;if(_tmp226 <= (void*)3)
goto _LL115;if(*((int*)_tmp226)!= 1)goto _LL115;_LL114: return 0;_LL115: _tmp227=
_tmp214.f1;if(_tmp227 <= (void*)3)goto _LL117;if(*((int*)_tmp227)!= 3)goto _LL117;
_tmp228=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp227)->f1;_tmp229=_tmp214.f2;
if(_tmp229 <= (void*)3)goto _LL117;if(*((int*)_tmp229)!= 3)goto _LL117;_tmp22A=((
struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp229)->f1;_LL116: return Cyc_CfFlowInfo_tag_cmps_lessthan_approx(
_tmp228,_tmp22A);_LL117: _tmp22B=_tmp214.f2;if(_tmp22B <= (void*)3)goto _LL119;if(*((
int*)_tmp22B)!= 3)goto _LL119;_LL118: return 0;_LL119:;_LL11A: goto _LL102;_LL102:;}{
struct _tuple0 _tmp22D=({struct _tuple0 _tmp22C;_tmp22C.f1=Cyc_CfFlowInfo_initlevel_approx(
r1);_tmp22C.f2=Cyc_CfFlowInfo_initlevel_approx(r2);_tmp22C;});void*_tmp22E;void*
_tmp22F;void*_tmp230;void*_tmp231;void*_tmp232;void*_tmp233;_LL11C: _tmp22E=
_tmp22D.f1;if((int)_tmp22E != 2)goto _LL11E;_tmp22F=_tmp22D.f2;if((int)_tmp22F != 2)
goto _LL11E;_LL11D: return 1;_LL11E: _tmp230=_tmp22D.f2;if((int)_tmp230 != 0)goto
_LL120;_LL11F: return 1;_LL120: _tmp231=_tmp22D.f1;if((int)_tmp231 != 0)goto _LL122;
_LL121: return 0;_LL122: _tmp232=_tmp22D.f2;if((int)_tmp232 != 1)goto _LL124;_LL123:
return 1;_LL124: _tmp233=_tmp22D.f1;if((int)_tmp233 != 1)goto _LL11B;_LL125: return 0;
_LL11B:;}}static int Cyc_CfFlowInfo_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*
r1s){if(r1s == r2s)return 1;for(0;r1s != 0;r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*
_tmp234=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;int found=0;{struct Cyc_List_List*
_tmp235=r2s;for(0;_tmp235 != 0;_tmp235=_tmp235->tl){struct Cyc_CfFlowInfo_Reln*
_tmp236=(struct Cyc_CfFlowInfo_Reln*)_tmp235->hd;if(_tmp234 == _tmp236  || _tmp234->vd
== _tmp236->vd  && Cyc_CfFlowInfo_same_relop(_tmp234->rop,_tmp236->rop)){found=1;
break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_consume_approx(struct Cyc_CfFlowInfo_ConsumeInfo
c1,struct Cyc_CfFlowInfo_ConsumeInfo c2){if(c1.may_consume != c2.may_consume){
struct Cyc_List_List*_tmp237=c1.may_consume;struct Cyc_List_List*_tmp238=c2.may_consume;
for(0;_tmp238 != 0  && _tmp237 != 0;_tmp238=_tmp238->tl){if((unsigned int)_tmp237 == (
unsigned int)_tmp238)goto compare_consumed_set;{int _tmp239=Cyc_CfFlowInfo_place_cmp((
struct Cyc_CfFlowInfo_Place*)_tmp237->hd,(struct Cyc_CfFlowInfo_Place*)_tmp238->hd);
if(_tmp239 == 0)_tmp237=_tmp237->tl;}}if(_tmp237 != 0)return 0;}
compare_consumed_set: return((int(*)(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2))
Cyc_CfFlowInfo_place_set_subset)(c1.consumed,c2.consumed);}int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo_union f1,union Cyc_CfFlowInfo_FlowInfo_union f2){
struct _tuple16 _tmp23B=({struct _tuple16 _tmp23A;_tmp23A.f1=f1;_tmp23A.f2=f2;
_tmp23A;});union Cyc_CfFlowInfo_FlowInfo_union _tmp23C;union Cyc_CfFlowInfo_FlowInfo_union
_tmp23D;union Cyc_CfFlowInfo_FlowInfo_union _tmp23E;struct Cyc_Dict_Dict _tmp23F;
struct Cyc_List_List*_tmp240;struct Cyc_CfFlowInfo_ConsumeInfo _tmp241;union Cyc_CfFlowInfo_FlowInfo_union
_tmp242;struct Cyc_Dict_Dict _tmp243;struct Cyc_List_List*_tmp244;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp245;_LL127: _tmp23C=_tmp23B.f1;if(((_tmp23B.f1).BottomFL).tag != 0)goto _LL129;
_LL128: return 1;_LL129: _tmp23D=_tmp23B.f2;if(((_tmp23B.f2).BottomFL).tag != 0)goto
_LL12B;_LL12A: return 0;_LL12B: _tmp23E=_tmp23B.f1;if(((_tmp23B.f1).ReachableFL).tag
!= 1)goto _LL126;_tmp23F=(_tmp23E.ReachableFL).f1;_tmp240=(_tmp23E.ReachableFL).f2;
_tmp241=(_tmp23E.ReachableFL).f3;_tmp242=_tmp23B.f2;if(((_tmp23B.f2).ReachableFL).tag
!= 1)goto _LL126;_tmp243=(_tmp242.ReachableFL).f1;_tmp244=(_tmp242.ReachableFL).f2;
_tmp245=(_tmp242.ReachableFL).f3;_LL12C: if(((_tmp23F.t == _tmp243.t  && _tmp240 == 
_tmp244) && _tmp241.may_consume == _tmp245.may_consume) && (_tmp241.consumed).t == (
_tmp245.consumed).t)return 1;return(Cyc_Dict_forall_intersect(Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp23F,_tmp243) && Cyc_CfFlowInfo_relns_approx(_tmp240,_tmp244)) && Cyc_CfFlowInfo_consume_approx(
_tmp241,_tmp245);_LL126:;}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(
struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){struct
Cyc_List_List*p;int found=0;for(p=rs;!found  && p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*
_tmp246=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp246->vd == v){found=1;break;}{
union Cyc_CfFlowInfo_RelnOp_union _tmp247=_tmp246->rop;struct Cyc_Absyn_Vardecl*
_tmp248;struct Cyc_Absyn_Vardecl*_tmp249;struct Cyc_Absyn_Vardecl*_tmp24A;_LL12E:
if((_tmp247.LessVar).tag != 1)goto _LL130;_tmp248=(_tmp247.LessVar).f1;_LL12F:
_tmp249=_tmp248;goto _LL131;_LL130: if((_tmp247.LessNumelts).tag != 2)goto _LL132;
_tmp249=(_tmp247.LessNumelts).f1;_LL131: _tmp24A=_tmp249;goto _LL133;_LL132: if((
_tmp247.LessEqNumelts).tag != 4)goto _LL134;_tmp24A=(_tmp247.LessEqNumelts).f1;
_LL133: if(v == _tmp24A)found=1;goto _LL12D;_LL134:;_LL135: goto _LL12D;_LL12D:;}}if(
!found)return rs;{struct Cyc_List_List*_tmp24B=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*
_tmp24C=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp24C->vd != v){{union Cyc_CfFlowInfo_RelnOp_union
_tmp24D=_tmp24C->rop;struct Cyc_Absyn_Vardecl*_tmp24E;struct Cyc_Absyn_Vardecl*
_tmp24F;struct Cyc_Absyn_Vardecl*_tmp250;_LL137: if((_tmp24D.LessVar).tag != 1)goto
_LL139;_tmp24E=(_tmp24D.LessVar).f1;_LL138: _tmp24F=_tmp24E;goto _LL13A;_LL139: if((
_tmp24D.LessNumelts).tag != 2)goto _LL13B;_tmp24F=(_tmp24D.LessNumelts).f1;_LL13A:
_tmp250=_tmp24F;goto _LL13C;_LL13B: if((_tmp24D.LessEqNumelts).tag != 4)goto _LL13D;
_tmp250=(_tmp24D.LessEqNumelts).f1;_LL13C: if(v == _tmp250)continue;goto _LL136;
_LL13D:;_LL13E: goto _LL136;_LL136:;}_tmp24B=({struct Cyc_List_List*_tmp251=
_region_malloc(rgn,sizeof(*_tmp251));_tmp251->hd=_tmp24C;_tmp251->tl=_tmp24B;
_tmp251;});}}return _tmp24B;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*
_tmp252=(void*)e->r;void*_tmp253;struct Cyc_Absyn_Vardecl*_tmp254;void*_tmp255;
struct Cyc_Absyn_Vardecl*_tmp256;void*_tmp257;struct Cyc_Absyn_Vardecl*_tmp258;
void*_tmp259;struct Cyc_Absyn_Vardecl*_tmp25A;_LL140: if(*((int*)_tmp252)!= 1)goto
_LL142;_tmp253=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp252)->f2;if(_tmp253 <= (
void*)1)goto _LL142;if(*((int*)_tmp253)!= 0)goto _LL142;_tmp254=((struct Cyc_Absyn_Global_b_struct*)
_tmp253)->f1;_LL141: _tmp256=_tmp254;goto _LL143;_LL142: if(*((int*)_tmp252)!= 1)
goto _LL144;_tmp255=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp252)->f2;if(
_tmp255 <= (void*)1)goto _LL144;if(*((int*)_tmp255)!= 2)goto _LL144;_tmp256=((
struct Cyc_Absyn_Param_b_struct*)_tmp255)->f1;_LL143: _tmp258=_tmp256;goto _LL145;
_LL144: if(*((int*)_tmp252)!= 1)goto _LL146;_tmp257=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp252)->f2;if(_tmp257 <= (void*)1)goto _LL146;if(*((int*)_tmp257)!= 3)goto _LL146;
_tmp258=((struct Cyc_Absyn_Local_b_struct*)_tmp257)->f1;_LL145: _tmp25A=_tmp258;
goto _LL147;_LL146: if(*((int*)_tmp252)!= 1)goto _LL148;_tmp259=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp252)->f2;if(_tmp259 <= (void*)1)goto _LL148;if(*((int*)_tmp259)!= 4)goto _LL148;
_tmp25A=((struct Cyc_Absyn_Pat_b_struct*)_tmp259)->f1;_LL147: if(!_tmp25A->escapes)
return Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp25A);goto _LL13F;_LL148:;_LL149: goto
_LL13F;_LL13F:;}return r;}struct Cyc_CfFlowInfo_TagCmp*Cyc_CfFlowInfo_copy_tagcmp(
struct _RegionHandle*r2,struct Cyc_CfFlowInfo_TagCmp*t){return({struct Cyc_CfFlowInfo_TagCmp*
_tmp25B=_region_malloc(r2,sizeof(*_tmp25B));_tmp25B[0]=*t;_tmp25B;});}struct Cyc_List_List*
Cyc_CfFlowInfo_copy_tagcmps(struct _RegionHandle*r2,struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_CfFlowInfo_TagCmp*(*
f)(struct _RegionHandle*,struct Cyc_CfFlowInfo_TagCmp*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_CfFlowInfo_copy_tagcmp,r2,ts);}
struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(struct _RegionHandle*r2,struct
Cyc_CfFlowInfo_Reln*r){return({struct Cyc_CfFlowInfo_Reln*_tmp25C=_region_malloc(
r2,sizeof(*_tmp25C));_tmp25C[0]=*r;_tmp25C;});}struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(
struct _RegionHandle*r2,struct Cyc_List_List*relns){return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*f)(struct _RegionHandle*,struct
Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*
r1,struct Cyc_List_List*r2){for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){struct
Cyc_CfFlowInfo_Reln*_tmp25D=(struct Cyc_CfFlowInfo_Reln*)r1->hd;struct Cyc_CfFlowInfo_Reln*
_tmp25E=(struct Cyc_CfFlowInfo_Reln*)r2->hd;if(_tmp25D->vd != _tmp25E->vd  || !Cyc_CfFlowInfo_same_relop(
_tmp25D->rop,_tmp25E->rop))return 0;}if(r1 != 0  || r2 != 0)return 0;else{return 1;}}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,
struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)
return r;r=Cyc_CfFlowInfo_reln_kill_var(rgn,r,v);{void*_tmp25F=(void*)e->r;struct
Cyc_Absyn_MallocInfo _tmp260;struct Cyc_Absyn_Exp*_tmp261;int _tmp262;_LL14B: if(*((
int*)_tmp25F)!= 35)goto _LL14D;_tmp260=((struct Cyc_Absyn_Malloc_e_struct*)_tmp25F)->f1;
_tmp261=_tmp260.num_elts;_tmp262=_tmp260.fat_result;if(_tmp262 != 1)goto _LL14D;
_LL14C: malloc_loop: {void*_tmp263=(void*)_tmp261->r;struct Cyc_Absyn_Exp*_tmp264;
void*_tmp265;struct Cyc_Absyn_Vardecl*_tmp266;void*_tmp267;struct Cyc_Absyn_Vardecl*
_tmp268;void*_tmp269;struct Cyc_Absyn_Vardecl*_tmp26A;void*_tmp26B;struct Cyc_Absyn_Vardecl*
_tmp26C;_LL150: if(*((int*)_tmp263)!= 15)goto _LL152;_tmp264=((struct Cyc_Absyn_Cast_e_struct*)
_tmp263)->f2;_LL151: _tmp261=_tmp264;goto malloc_loop;_LL152: if(*((int*)_tmp263)!= 
1)goto _LL154;_tmp265=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp263)->f2;if(
_tmp265 <= (void*)1)goto _LL154;if(*((int*)_tmp265)!= 4)goto _LL154;_tmp266=((
struct Cyc_Absyn_Pat_b_struct*)_tmp265)->f1;_LL153: _tmp268=_tmp266;goto _LL155;
_LL154: if(*((int*)_tmp263)!= 1)goto _LL156;_tmp267=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp263)->f2;if(_tmp267 <= (void*)1)goto _LL156;if(*((int*)_tmp267)!= 3)goto _LL156;
_tmp268=((struct Cyc_Absyn_Local_b_struct*)_tmp267)->f1;_LL155: _tmp26A=_tmp268;
goto _LL157;_LL156: if(*((int*)_tmp263)!= 1)goto _LL158;_tmp269=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp263)->f2;if(_tmp269 <= (void*)1)goto _LL158;if(*((int*)_tmp269)!= 2)goto _LL158;
_tmp26A=((struct Cyc_Absyn_Param_b_struct*)_tmp269)->f1;_LL157: _tmp26C=_tmp26A;
goto _LL159;_LL158: if(*((int*)_tmp263)!= 1)goto _LL15A;_tmp26B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp263)->f2;if(_tmp26B <= (void*)1)goto _LL15A;if(*((int*)_tmp26B)!= 0)goto _LL15A;
_tmp26C=((struct Cyc_Absyn_Global_b_struct*)_tmp26B)->f1;_LL159: if(_tmp26C->escapes)
return r;return({struct Cyc_List_List*_tmp26D=_region_malloc(rgn,sizeof(*_tmp26D));
_tmp26D->hd=({struct Cyc_CfFlowInfo_Reln*_tmp26E=_region_malloc(rgn,sizeof(*
_tmp26E));_tmp26E->vd=_tmp26C;_tmp26E->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp26F;(_tmp26F.LessEqNumelts).tag=4;(_tmp26F.LessEqNumelts).f1=
v;_tmp26F;});_tmp26E;});_tmp26D->tl=r;_tmp26D;});_LL15A:;_LL15B: return r;_LL14F:;}
_LL14D:;_LL14E: goto _LL14A;_LL14A:;}{void*_tmp270=Cyc_Tcutil_compress((void*)v->type);
_LL15D: if(_tmp270 <= (void*)4)goto _LL15F;if(*((int*)_tmp270)!= 5)goto _LL15F;
_LL15E: goto _LL15C;_LL15F:;_LL160: return r;_LL15C:;}loop: {void*_tmp271=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp272;union Cyc_Absyn_Cnst_union _tmp273;int _tmp274;void*
_tmp275;struct Cyc_List_List*_tmp276;struct Cyc_List_List _tmp277;struct Cyc_List_List*
_tmp278;struct Cyc_List_List _tmp279;struct Cyc_Absyn_Exp*_tmp27A;void*_tmp27B;
struct Cyc_List_List*_tmp27C;struct Cyc_List_List _tmp27D;struct Cyc_Absyn_Exp*
_tmp27E;_LL162: if(*((int*)_tmp271)!= 15)goto _LL164;_tmp272=((struct Cyc_Absyn_Cast_e_struct*)
_tmp271)->f2;_LL163: e=_tmp272;goto loop;_LL164: if(*((int*)_tmp271)!= 0)goto _LL166;
_tmp273=((struct Cyc_Absyn_Const_e_struct*)_tmp271)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp271)->f1).Int_c).tag != 2)goto _LL166;_tmp274=(_tmp273.Int_c).f2;_LL165: return({
struct Cyc_List_List*_tmp27F=_region_malloc(rgn,sizeof(*_tmp27F));_tmp27F->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp280=_region_malloc(rgn,sizeof(*_tmp280));_tmp280->vd=
v;_tmp280->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp281;(_tmp281.EqualConst).tag=0;(_tmp281.EqualConst).f1=(unsigned int)_tmp274;
_tmp281;});_tmp280;});_tmp27F->tl=r;_tmp27F;});_LL166: if(*((int*)_tmp271)!= 3)
goto _LL168;_tmp275=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp271)->f1;if((
int)_tmp275 != 4)goto _LL168;_tmp276=((struct Cyc_Absyn_Primop_e_struct*)_tmp271)->f2;
if(_tmp276 == 0)goto _LL168;_tmp277=*_tmp276;_tmp278=_tmp277.tl;if(_tmp278 == 0)
goto _LL168;_tmp279=*_tmp278;_tmp27A=(struct Cyc_Absyn_Exp*)_tmp279.hd;_LL167:{
void*_tmp282=(void*)_tmp27A->r;void*_tmp283;struct Cyc_List_List*_tmp284;struct
Cyc_List_List _tmp285;struct Cyc_Absyn_Exp*_tmp286;_LL16D: if(*((int*)_tmp282)!= 3)
goto _LL16F;_tmp283=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp282)->f1;
_tmp284=((struct Cyc_Absyn_Primop_e_struct*)_tmp282)->f2;if(_tmp284 == 0)goto
_LL16F;_tmp285=*_tmp284;_tmp286=(struct Cyc_Absyn_Exp*)_tmp285.hd;_LL16E:{void*
_tmp287=(void*)_tmp286->r;void*_tmp288;struct Cyc_Absyn_Vardecl*_tmp289;void*
_tmp28A;struct Cyc_Absyn_Vardecl*_tmp28B;void*_tmp28C;struct Cyc_Absyn_Vardecl*
_tmp28D;void*_tmp28E;struct Cyc_Absyn_Vardecl*_tmp28F;_LL172: if(*((int*)_tmp287)
!= 1)goto _LL174;_tmp288=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp287)->f2;if(
_tmp288 <= (void*)1)goto _LL174;if(*((int*)_tmp288)!= 0)goto _LL174;_tmp289=((
struct Cyc_Absyn_Global_b_struct*)_tmp288)->f1;_LL173: _tmp28B=_tmp289;goto _LL175;
_LL174: if(*((int*)_tmp287)!= 1)goto _LL176;_tmp28A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp287)->f2;if(_tmp28A <= (void*)1)goto _LL176;if(*((int*)_tmp28A)!= 3)goto _LL176;
_tmp28B=((struct Cyc_Absyn_Local_b_struct*)_tmp28A)->f1;_LL175: _tmp28D=_tmp28B;
goto _LL177;_LL176: if(*((int*)_tmp287)!= 1)goto _LL178;_tmp28C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp287)->f2;if(_tmp28C <= (void*)1)goto _LL178;if(*((int*)_tmp28C)!= 2)goto _LL178;
_tmp28D=((struct Cyc_Absyn_Param_b_struct*)_tmp28C)->f1;_LL177: _tmp28F=_tmp28D;
goto _LL179;_LL178: if(*((int*)_tmp287)!= 1)goto _LL17A;_tmp28E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp287)->f2;if(_tmp28E <= (void*)1)goto _LL17A;if(*((int*)_tmp28E)!= 4)goto _LL17A;
_tmp28F=((struct Cyc_Absyn_Pat_b_struct*)_tmp28E)->f1;_LL179: if(_tmp28F->escapes)
return r;return({struct Cyc_List_List*_tmp290=_region_malloc(rgn,sizeof(*_tmp290));
_tmp290->hd=({struct Cyc_CfFlowInfo_Reln*_tmp291=_region_malloc(rgn,sizeof(*
_tmp291));_tmp291->vd=v;_tmp291->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union
Cyc_CfFlowInfo_RelnOp_union _tmp292;(_tmp292.LessNumelts).tag=2;(_tmp292.LessNumelts).f1=
_tmp28F;_tmp292;});_tmp291;});_tmp290->tl=r;_tmp290;});_LL17A:;_LL17B: goto _LL171;
_LL171:;}goto _LL16C;_LL16F:;_LL170: goto _LL16C;_LL16C:;}goto _LL161;_LL168: if(*((
int*)_tmp271)!= 3)goto _LL16A;_tmp27B=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp271)->f1;_tmp27C=((struct Cyc_Absyn_Primop_e_struct*)_tmp271)->f2;if(_tmp27C
== 0)goto _LL16A;_tmp27D=*_tmp27C;_tmp27E=(struct Cyc_Absyn_Exp*)_tmp27D.hd;_LL169:{
void*_tmp293=(void*)_tmp27E->r;void*_tmp294;struct Cyc_Absyn_Vardecl*_tmp295;void*
_tmp296;struct Cyc_Absyn_Vardecl*_tmp297;void*_tmp298;struct Cyc_Absyn_Vardecl*
_tmp299;void*_tmp29A;struct Cyc_Absyn_Vardecl*_tmp29B;_LL17D: if(*((int*)_tmp293)
!= 1)goto _LL17F;_tmp294=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp293)->f2;if(
_tmp294 <= (void*)1)goto _LL17F;if(*((int*)_tmp294)!= 0)goto _LL17F;_tmp295=((
struct Cyc_Absyn_Global_b_struct*)_tmp294)->f1;_LL17E: _tmp297=_tmp295;goto _LL180;
_LL17F: if(*((int*)_tmp293)!= 1)goto _LL181;_tmp296=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp293)->f2;if(_tmp296 <= (void*)1)goto _LL181;if(*((int*)_tmp296)!= 3)goto _LL181;
_tmp297=((struct Cyc_Absyn_Local_b_struct*)_tmp296)->f1;_LL180: _tmp299=_tmp297;
goto _LL182;_LL181: if(*((int*)_tmp293)!= 1)goto _LL183;_tmp298=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp293)->f2;if(_tmp298 <= (void*)1)goto _LL183;if(*((int*)_tmp298)!= 2)goto _LL183;
_tmp299=((struct Cyc_Absyn_Param_b_struct*)_tmp298)->f1;_LL182: _tmp29B=_tmp299;
goto _LL184;_LL183: if(*((int*)_tmp293)!= 1)goto _LL185;_tmp29A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp293)->f2;if(_tmp29A <= (void*)1)goto _LL185;if(*((int*)_tmp29A)!= 4)goto _LL185;
_tmp29B=((struct Cyc_Absyn_Pat_b_struct*)_tmp29A)->f1;_LL184: if(_tmp29B->escapes)
return r;return({struct Cyc_List_List*_tmp29C=_region_malloc(rgn,sizeof(*_tmp29C));
_tmp29C->hd=({struct Cyc_CfFlowInfo_Reln*_tmp29D=_region_malloc(rgn,sizeof(*
_tmp29D));_tmp29D->vd=v;_tmp29D->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union
Cyc_CfFlowInfo_RelnOp_union _tmp29E;(_tmp29E.LessEqNumelts).tag=4;(_tmp29E.LessEqNumelts).f1=
_tmp29B;_tmp29E;});_tmp29D;});_tmp29C->tl=r;_tmp29C;});_LL185:;_LL186: goto _LL17C;
_LL17C:;}goto _LL161;_LL16A:;_LL16B: goto _LL161;_LL161:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*_tmp29F=(void*)e1->r;void*
_tmp2A0;struct Cyc_Absyn_Vardecl*_tmp2A1;void*_tmp2A2;struct Cyc_Absyn_Vardecl*
_tmp2A3;void*_tmp2A4;struct Cyc_Absyn_Vardecl*_tmp2A5;void*_tmp2A6;struct Cyc_Absyn_Vardecl*
_tmp2A7;_LL188: if(*((int*)_tmp29F)!= 1)goto _LL18A;_tmp2A0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp29F)->f2;if(_tmp2A0 <= (void*)1)goto _LL18A;if(*((int*)_tmp2A0)!= 0)goto _LL18A;
_tmp2A1=((struct Cyc_Absyn_Global_b_struct*)_tmp2A0)->f1;_LL189: _tmp2A3=_tmp2A1;
goto _LL18B;_LL18A: if(*((int*)_tmp29F)!= 1)goto _LL18C;_tmp2A2=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp29F)->f2;if(_tmp2A2 <= (void*)1)goto _LL18C;if(*((int*)_tmp2A2)!= 2)goto _LL18C;
_tmp2A3=((struct Cyc_Absyn_Param_b_struct*)_tmp2A2)->f1;_LL18B: _tmp2A5=_tmp2A3;
goto _LL18D;_LL18C: if(*((int*)_tmp29F)!= 1)goto _LL18E;_tmp2A4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp29F)->f2;if(_tmp2A4 <= (void*)1)goto _LL18E;if(*((int*)_tmp2A4)!= 3)goto _LL18E;
_tmp2A5=((struct Cyc_Absyn_Local_b_struct*)_tmp2A4)->f1;_LL18D: _tmp2A7=_tmp2A5;
goto _LL18F;_LL18E: if(*((int*)_tmp29F)!= 1)goto _LL190;_tmp2A6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp29F)->f2;if(_tmp2A6 <= (void*)1)goto _LL190;if(*((int*)_tmp2A6)!= 4)goto _LL190;
_tmp2A7=((struct Cyc_Absyn_Pat_b_struct*)_tmp2A6)->f1;_LL18F: if(!_tmp2A7->escapes)
return Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp2A7,e2);goto _LL187;_LL190:;_LL191:
goto _LL187;_LL187:;}return r;}static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*
r){({struct Cyc_String_pa_struct _tmp2AA;_tmp2AA.tag=0;_tmp2AA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((r->vd)->name));{
void*_tmp2A8[1]={& _tmp2AA};Cyc_fprintf(Cyc_stderr,({const char*_tmp2A9="%s";
_tag_dyneither(_tmp2A9,sizeof(char),3);}),_tag_dyneither(_tmp2A8,sizeof(void*),1));}});{
union Cyc_CfFlowInfo_RelnOp_union _tmp2AB=r->rop;unsigned int _tmp2AC;struct Cyc_Absyn_Vardecl*
_tmp2AD;struct Cyc_Absyn_Vardecl*_tmp2AE;unsigned int _tmp2AF;struct Cyc_Absyn_Vardecl*
_tmp2B0;_LL193: if((_tmp2AB.EqualConst).tag != 0)goto _LL195;_tmp2AC=(_tmp2AB.EqualConst).f1;
_LL194:({struct Cyc_Int_pa_struct _tmp2B3;_tmp2B3.tag=1;_tmp2B3.f1=(unsigned long)((
int)_tmp2AC);{void*_tmp2B1[1]={& _tmp2B3};Cyc_fprintf(Cyc_stderr,({const char*
_tmp2B2="==%d";_tag_dyneither(_tmp2B2,sizeof(char),5);}),_tag_dyneither(_tmp2B1,
sizeof(void*),1));}});goto _LL192;_LL195: if((_tmp2AB.LessVar).tag != 1)goto _LL197;
_tmp2AD=(_tmp2AB.LessVar).f1;_LL196:({struct Cyc_String_pa_struct _tmp2B6;_tmp2B6.tag=
0;_tmp2B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp2AD->name));{void*_tmp2B4[1]={& _tmp2B6};Cyc_fprintf(Cyc_stderr,({const char*
_tmp2B5="<%s";_tag_dyneither(_tmp2B5,sizeof(char),4);}),_tag_dyneither(_tmp2B4,
sizeof(void*),1));}});goto _LL192;_LL197: if((_tmp2AB.LessNumelts).tag != 2)goto
_LL199;_tmp2AE=(_tmp2AB.LessNumelts).f1;_LL198:({struct Cyc_String_pa_struct
_tmp2B9;_tmp2B9.tag=0;_tmp2B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp2AE->name));{void*_tmp2B7[1]={& _tmp2B9};Cyc_fprintf(
Cyc_stderr,({const char*_tmp2B8="<numelts(%s)";_tag_dyneither(_tmp2B8,sizeof(char),
13);}),_tag_dyneither(_tmp2B7,sizeof(void*),1));}});goto _LL192;_LL199: if((
_tmp2AB.LessConst).tag != 3)goto _LL19B;_tmp2AF=(_tmp2AB.LessConst).f1;_LL19A:({
struct Cyc_Int_pa_struct _tmp2BC;_tmp2BC.tag=1;_tmp2BC.f1=(unsigned long)((int)
_tmp2AF);{void*_tmp2BA[1]={& _tmp2BC};Cyc_fprintf(Cyc_stderr,({const char*_tmp2BB="<%d";
_tag_dyneither(_tmp2BB,sizeof(char),4);}),_tag_dyneither(_tmp2BA,sizeof(void*),1));}});
goto _LL192;_LL19B: if((_tmp2AB.LessEqNumelts).tag != 4)goto _LL192;_tmp2B0=(_tmp2AB.LessEqNumelts).f1;
_LL19C:({struct Cyc_String_pa_struct _tmp2BF;_tmp2BF.tag=0;_tmp2BF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2B0->name));{
void*_tmp2BD[1]={& _tmp2BF};Cyc_fprintf(Cyc_stderr,({const char*_tmp2BE="<=numelts(%s)";
_tag_dyneither(_tmp2BE,sizeof(char),14);}),_tag_dyneither(_tmp2BD,sizeof(void*),
1));}});goto _LL192;_LL192:;}}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*
r){for(0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)r->hd);
if(r->tl != 0)({void*_tmp2C0=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2C1=",";
_tag_dyneither(_tmp2C1,sizeof(char),2);}),_tag_dyneither(_tmp2C0,sizeof(void*),0));});}}
void Cyc_CfFlowInfo_print_initlevel(void*il){void*_tmp2C2=il;_LL19E: if((int)
_tmp2C2 != 0)goto _LL1A0;_LL19F:({void*_tmp2C3=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmp2C4="uninitialized";_tag_dyneither(_tmp2C4,sizeof(char),14);}),
_tag_dyneither(_tmp2C3,sizeof(void*),0));});goto _LL19D;_LL1A0: if((int)_tmp2C2 != 
1)goto _LL1A2;_LL1A1:({void*_tmp2C5=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2C6="this-initialized";
_tag_dyneither(_tmp2C6,sizeof(char),17);}),_tag_dyneither(_tmp2C5,sizeof(void*),
0));});goto _LL19D;_LL1A2: if((int)_tmp2C2 != 2)goto _LL19D;_LL1A3:({void*_tmp2C7=0;
Cyc_fprintf(Cyc_stderr,({const char*_tmp2C8="all-initialized";_tag_dyneither(
_tmp2C8,sizeof(char),16);}),_tag_dyneither(_tmp2C7,sizeof(void*),0));});goto
_LL19D;_LL19D:;}void Cyc_CfFlowInfo_print_root(void*root){void*_tmp2C9=root;
struct Cyc_Absyn_Vardecl*_tmp2CA;struct Cyc_Absyn_Exp*_tmp2CB;void*_tmp2CC;_LL1A5:
if(*((int*)_tmp2C9)!= 0)goto _LL1A7;_tmp2CA=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp2C9)->f1;_LL1A6:({struct Cyc_String_pa_struct _tmp2CF;_tmp2CF.tag=0;_tmp2CF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2CA->name));{
void*_tmp2CD[1]={& _tmp2CF};Cyc_fprintf(Cyc_stderr,({const char*_tmp2CE="Root(%s)";
_tag_dyneither(_tmp2CE,sizeof(char),9);}),_tag_dyneither(_tmp2CD,sizeof(void*),1));}});
goto _LL1A4;_LL1A7: if(*((int*)_tmp2C9)!= 1)goto _LL1A9;_tmp2CB=((struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp2C9)->f1;_tmp2CC=(void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp2C9)->f2;
_LL1A8:({struct Cyc_String_pa_struct _tmp2D3;_tmp2D3.tag=0;_tmp2D3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2CC));{struct
Cyc_String_pa_struct _tmp2D2;_tmp2D2.tag=0;_tmp2D2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp2CB));{void*_tmp2D0[2]={& _tmp2D2,&
_tmp2D3};Cyc_fprintf(Cyc_stderr,({const char*_tmp2D1="MallocPt(%s,%s)";
_tag_dyneither(_tmp2D1,sizeof(char),16);}),_tag_dyneither(_tmp2D0,sizeof(void*),
2));}}});goto _LL1A4;_LL1A9: if(*((int*)_tmp2C9)!= 2)goto _LL1A4;_LL1AA:({void*
_tmp2D4=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2D5="InitParam(_,_)";
_tag_dyneither(_tmp2D5,sizeof(char),15);}),_tag_dyneither(_tmp2D4,sizeof(void*),
0));});goto _LL1A4;_LL1A4:;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*
p){Cyc_CfFlowInfo_print_root((void*)p->root);{struct Cyc_List_List*_tmp2D6=p->fields;
while(_tmp2D6 != 0){({struct Cyc_Int_pa_struct _tmp2D9;_tmp2D9.tag=1;_tmp2D9.f1=(
unsigned long)((int)_tmp2D6->hd);{void*_tmp2D7[1]={& _tmp2D9};Cyc_fprintf(Cyc_stderr,({
const char*_tmp2D8=".%d";_tag_dyneither(_tmp2D8,sizeof(char),4);}),_tag_dyneither(
_tmp2D7,sizeof(void*),1));}});_tmp2D6=_tmp2D6->tl;}({void*_tmp2DA=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp2DB=" ";_tag_dyneither(_tmp2DB,sizeof(char),2);}),
_tag_dyneither(_tmp2DA,sizeof(void*),0));});}}void Cyc_CfFlowInfo_print_list(
struct Cyc_List_List*x,void(*pr)(void*)){int _tmp2DC=1;while(x != 0){if(!_tmp2DC){({
void*_tmp2DD=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2DE=", ";_tag_dyneither(
_tmp2DE,sizeof(char),3);}),_tag_dyneither(_tmp2DD,sizeof(void*),0));});_tmp2DC=0;}
pr((void*)x->hd);x=x->tl;}({void*_tmp2DF=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp2E0="\n";_tag_dyneither(_tmp2E0,sizeof(char),2);}),_tag_dyneither(_tmp2DF,
sizeof(void*),0));});}void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,
void(*pr)(void*)){struct _RegionHandle _tmp2E1=_new_region("r");struct
_RegionHandle*r=& _tmp2E1;_push_region(r);{struct _handler_cons _tmp2E2;
_push_handler(& _tmp2E2);{int _tmp2E4=0;if(setjmp(_tmp2E2.handler))_tmp2E4=1;if(!
_tmp2E4){{struct Cyc_Iter_Iter _tmp2E5=((struct Cyc_Iter_Iter(*)(struct
_RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);struct _tuple10
_tmp2E6=*((struct _tuple10*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
r,p);int first=1;({void*_tmp2E7=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2E8="{ ";
_tag_dyneither(_tmp2E8,sizeof(char),3);}),_tag_dyneither(_tmp2E7,sizeof(void*),0));});
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple10*))Cyc_Iter_next)(_tmp2E5,&
_tmp2E6)){void*_tmp2E9=_tmp2E6.f1;if(!first){({void*_tmp2EA=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp2EB=", ";_tag_dyneither(_tmp2EB,sizeof(char),3);}),_tag_dyneither(
_tmp2EA,sizeof(void*),0));});first=0;}pr(_tmp2E9);}({void*_tmp2EC=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp2ED="}\n";_tag_dyneither(_tmp2ED,sizeof(char),3);}),
_tag_dyneither(_tmp2EC,sizeof(void*),0));});};_pop_handler();}else{void*_tmp2E3=(
void*)_exn_thrown;void*_tmp2EF=_tmp2E3;_LL1AC:;_LL1AD:({void*_tmp2F0=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp2F1="{ }\n";_tag_dyneither(_tmp2F1,sizeof(char),5);}),
_tag_dyneither(_tmp2F0,sizeof(void*),0));});_npop_handler(0);return;_LL1AE:;
_LL1AF:(void)_throw(_tmp2EF);_LL1AB:;}}};_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(
void*rval){void*_tmp2F2=rval;void*_tmp2F3;void*_tmp2F4;struct Cyc_CfFlowInfo_Place*
_tmp2F5;int _tmp2F6;struct _dyneither_ptr _tmp2F7;_LL1B1: if((int)_tmp2F2 != 0)goto
_LL1B3;_LL1B2:({void*_tmp2F8=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2F9="Zero";
_tag_dyneither(_tmp2F9,sizeof(char),5);}),_tag_dyneither(_tmp2F8,sizeof(void*),0));});
goto _LL1B0;_LL1B3: if((int)_tmp2F2 != 1)goto _LL1B5;_LL1B4:({void*_tmp2FA=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp2FB="NotZeroAll";_tag_dyneither(_tmp2FB,sizeof(char),
11);}),_tag_dyneither(_tmp2FA,sizeof(void*),0));});goto _LL1B0;_LL1B5: if((int)
_tmp2F2 != 2)goto _LL1B7;_LL1B6:({void*_tmp2FC=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmp2FD="NotZeroThis";_tag_dyneither(_tmp2FD,sizeof(char),12);}),
_tag_dyneither(_tmp2FC,sizeof(void*),0));});goto _LL1B0;_LL1B7: if(_tmp2F2 <= (void*)
3)goto _LL1B9;if(*((int*)_tmp2F2)!= 0)goto _LL1B9;_tmp2F3=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp2F2)->f1;_LL1B8:({void*_tmp2FE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2FF="Unknown(";
_tag_dyneither(_tmp2FF,sizeof(char),9);}),_tag_dyneither(_tmp2FE,sizeof(void*),0));});
Cyc_CfFlowInfo_print_initlevel(_tmp2F3);({void*_tmp300=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp301=")";_tag_dyneither(_tmp301,sizeof(char),2);}),_tag_dyneither(
_tmp300,sizeof(void*),0));});goto _LL1B0;_LL1B9: if(_tmp2F2 <= (void*)3)goto _LL1BB;
if(*((int*)_tmp2F2)!= 1)goto _LL1BB;_tmp2F4=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp2F2)->f1;_LL1BA:({void*_tmp302=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp303="Esc(";
_tag_dyneither(_tmp303,sizeof(char),5);}),_tag_dyneither(_tmp302,sizeof(void*),0));});
Cyc_CfFlowInfo_print_initlevel(_tmp2F4);({void*_tmp304=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp305=")";_tag_dyneither(_tmp305,sizeof(char),2);}),_tag_dyneither(
_tmp304,sizeof(void*),0));});goto _LL1B0;_LL1BB: if(_tmp2F2 <= (void*)3)goto _LL1BD;
if(*((int*)_tmp2F2)!= 2)goto _LL1BD;_tmp2F5=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp2F2)->f1;_LL1BC:({void*_tmp306=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp307="AddrOf(";
_tag_dyneither(_tmp307,sizeof(char),8);}),_tag_dyneither(_tmp306,sizeof(void*),0));});
Cyc_CfFlowInfo_print_place(_tmp2F5);({void*_tmp308=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp309=")";_tag_dyneither(_tmp309,sizeof(char),2);}),_tag_dyneither(
_tmp308,sizeof(void*),0));});goto _LL1B0;_LL1BD: if(_tmp2F2 <= (void*)3)goto _LL1BF;
if(*((int*)_tmp2F2)!= 3)goto _LL1BF;_LL1BE:({void*_tmp30A=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp30B="TagCmps(?)";_tag_dyneither(_tmp30B,sizeof(char),11);}),
_tag_dyneither(_tmp30A,sizeof(void*),0));});goto _LL1B0;_LL1BF: if(_tmp2F2 <= (void*)
3)goto _LL1B0;if(*((int*)_tmp2F2)!= 4)goto _LL1B0;_tmp2F6=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2F2)->f1;_tmp2F7=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2F2)->f2;
_LL1C0: if(_tmp2F6)({void*_tmp30C=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp30D="AggrUnion{";
_tag_dyneither(_tmp30D,sizeof(char),11);}),_tag_dyneither(_tmp30C,sizeof(void*),
0));});else{({void*_tmp30E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp30F="AggrStruct{";
_tag_dyneither(_tmp30F,sizeof(char),12);}),_tag_dyneither(_tmp30E,sizeof(void*),
0));});}{int i=0;for(0;i < _get_dyneither_size(_tmp2F7,sizeof(void*));++ i){Cyc_CfFlowInfo_print_absrval(((
void**)_tmp2F7.curr)[i]);if(i + 1 < _get_dyneither_size(_tmp2F7,sizeof(void*)))({
void*_tmp310=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp311=",";_tag_dyneither(
_tmp311,sizeof(char),2);}),_tag_dyneither(_tmp310,sizeof(void*),0));});}}({void*
_tmp312=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp313="}";_tag_dyneither(_tmp313,
sizeof(char),2);}),_tag_dyneither(_tmp312,sizeof(void*),0));});goto _LL1B0;_LL1B0:;}
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){({void*_tmp314=
0;Cyc_fprintf(Cyc_stderr,({const char*_tmp315="    ";_tag_dyneither(_tmp315,
sizeof(char),5);}),_tag_dyneither(_tmp314,sizeof(void*),0));});Cyc_CfFlowInfo_print_root(
root);({void*_tmp316=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp317=" --> ";
_tag_dyneither(_tmp317,sizeof(char),6);}),_tag_dyneither(_tmp316,sizeof(void*),0));});
Cyc_CfFlowInfo_print_absrval(rval);({void*_tmp318=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp319="\n";_tag_dyneither(_tmp319,sizeof(char),2);}),_tag_dyneither(
_tmp318,sizeof(void*),0));});}void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict
d){Cyc_Dict_iter(Cyc_CfFlowInfo_print_flow_mapping,d);}void Cyc_CfFlowInfo_print_flow(
union Cyc_CfFlowInfo_FlowInfo_union f){union Cyc_CfFlowInfo_FlowInfo_union _tmp31A=f;
struct Cyc_Dict_Dict _tmp31B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp31C;_LL1C2: if((
_tmp31A.BottomFL).tag != 0)goto _LL1C4;_LL1C3:({void*_tmp31D=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp31E="  BottomFL\n";_tag_dyneither(_tmp31E,sizeof(char),12);}),
_tag_dyneither(_tmp31D,sizeof(void*),0));});goto _LL1C1;_LL1C4: if((_tmp31A.ReachableFL).tag
!= 1)goto _LL1C1;_tmp31B=(_tmp31A.ReachableFL).f1;_tmp31C=(_tmp31A.ReachableFL).f3;
_LL1C5:({void*_tmp31F=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp320="  ReachableFL:\n";
_tag_dyneither(_tmp320,sizeof(char),16);}),_tag_dyneither(_tmp31F,sizeof(void*),
0));});Cyc_Dict_iter(Cyc_CfFlowInfo_print_flow_mapping,_tmp31B);({void*_tmp321=0;
Cyc_fprintf(Cyc_stderr,({const char*_tmp322="\n    consumed: ";_tag_dyneither(
_tmp322,sizeof(char),16);}),_tag_dyneither(_tmp321,sizeof(void*),0));});((void(*)(
struct Cyc_Dict_Dict p,void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(
_tmp31C.consumed,Cyc_CfFlowInfo_print_place);({void*_tmp323=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp324="\n    may_consume: ";_tag_dyneither(_tmp324,sizeof(char),19);}),
_tag_dyneither(_tmp323,sizeof(void*),0));});((void(*)(struct Cyc_List_List*x,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(_tmp31C.may_consume,
Cyc_CfFlowInfo_print_place);({void*_tmp325=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp326="\n";_tag_dyneither(_tmp326,sizeof(char),2);}),_tag_dyneither(_tmp325,
sizeof(void*),0));});goto _LL1C1;_LL1C1:;}int Cyc_CfFlowInfo_contains_region(void*
rgn,void*t){void*_tmp327=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp328;
struct Cyc_Absyn_DatatypeInfo _tmp329;struct Cyc_List_List*_tmp32A;struct Cyc_Core_Opt*
_tmp32B;struct Cyc_List_List*_tmp32C;struct Cyc_Absyn_AggrInfo _tmp32D;struct Cyc_List_List*
_tmp32E;struct Cyc_Absyn_DatatypeFieldInfo _tmp32F;struct Cyc_List_List*_tmp330;
struct Cyc_List_List*_tmp331;struct Cyc_Absyn_PtrInfo _tmp332;void*_tmp333;struct
Cyc_Absyn_PtrAtts _tmp334;void*_tmp335;struct Cyc_List_List*_tmp336;struct Cyc_List_List*
_tmp337;struct Cyc_Absyn_ArrayInfo _tmp338;void*_tmp339;void*_tmp33A;void*_tmp33B;
void*_tmp33C;void*_tmp33D;void*_tmp33E;_LL1C7: if((int)_tmp327 != 0)goto _LL1C9;
_LL1C8: goto _LL1CA;_LL1C9: if(_tmp327 <= (void*)4)goto _LL1CB;if(*((int*)_tmp327)!= 
5)goto _LL1CB;_LL1CA: goto _LL1CC;_LL1CB: if((int)_tmp327 != 1)goto _LL1CD;_LL1CC: goto
_LL1CE;_LL1CD: if(_tmp327 <= (void*)4)goto _LL1D5;if(*((int*)_tmp327)!= 6)goto
_LL1CF;_LL1CE: goto _LL1D0;_LL1CF: if(*((int*)_tmp327)!= 12)goto _LL1D1;_LL1D0: goto
_LL1D2;_LL1D1: if(*((int*)_tmp327)!= 13)goto _LL1D3;_LL1D2: goto _LL1D4;_LL1D3: if(*((
int*)_tmp327)!= 18)goto _LL1D5;_LL1D4: goto _LL1D6;_LL1D5: if((int)_tmp327 != 2)goto
_LL1D7;_LL1D6: goto _LL1D8;_LL1D7: if(_tmp327 <= (void*)4)goto _LL1D9;if(*((int*)
_tmp327)!= 0)goto _LL1D9;_LL1D8: return 0;_LL1D9: if((int)_tmp327 != 3)goto _LL1DB;
_LL1DA: {void*_tmp33F=rgn;_LL1FC: if((int)_tmp33F != 0)goto _LL1FE;_LL1FD: return 1;
_LL1FE:;_LL1FF: return 0;_LL1FB:;}_LL1DB: if(_tmp327 <= (void*)4)goto _LL1DD;if(*((
int*)_tmp327)!= 1)goto _LL1DD;_tmp328=((struct Cyc_Absyn_VarType_struct*)_tmp327)->f1;
_LL1DC: {void*_tmp340=rgn;struct Cyc_Absyn_Tvar*_tmp341;_LL201: if((int)_tmp340 != 
0)goto _LL203;_LL202: return 0;_LL203: if(_tmp340 <= (void*)1)goto _LL200;if(*((int*)
_tmp340)!= 0)goto _LL200;_tmp341=((struct Cyc_CfFlowInfo_Region_k_struct*)_tmp340)->f1;
_LL204: return Cyc_Absyn_tvar_cmp(_tmp328,_tmp341)== 0;_LL200:;}_LL1DD: if(_tmp327
<= (void*)4)goto _LL1DF;if(*((int*)_tmp327)!= 2)goto _LL1DF;_tmp329=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp327)->f1;_tmp32A=_tmp329.targs;_tmp32B=_tmp329.rgn;_LL1DE: if((unsigned int)
_tmp32B  && Cyc_CfFlowInfo_contains_region(rgn,(void*)_tmp32B->v))return 1;_tmp32C=
_tmp32A;goto _LL1E0;_LL1DF: if(_tmp327 <= (void*)4)goto _LL1E1;if(*((int*)_tmp327)!= 
16)goto _LL1E1;_tmp32C=((struct Cyc_Absyn_TypedefType_struct*)_tmp327)->f2;_LL1E0:
_tmp32E=_tmp32C;goto _LL1E2;_LL1E1: if(_tmp327 <= (void*)4)goto _LL1E3;if(*((int*)
_tmp327)!= 10)goto _LL1E3;_tmp32D=((struct Cyc_Absyn_AggrType_struct*)_tmp327)->f1;
_tmp32E=_tmp32D.targs;_LL1E2: _tmp330=_tmp32E;goto _LL1E4;_LL1E3: if(_tmp327 <= (
void*)4)goto _LL1E5;if(*((int*)_tmp327)!= 3)goto _LL1E5;_tmp32F=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp327)->f1;_tmp330=_tmp32F.targs;_LL1E4: _tmp331=_tmp330;goto _LL1E6;_LL1E5: if(
_tmp327 <= (void*)4)goto _LL1E7;if(*((int*)_tmp327)!= 20)goto _LL1E7;_tmp331=((
struct Cyc_Absyn_JoinEff_struct*)_tmp327)->f1;_LL1E6: return Cyc_List_exists_c(Cyc_CfFlowInfo_contains_region,
rgn,_tmp331);_LL1E7: if(_tmp327 <= (void*)4)goto _LL1E9;if(*((int*)_tmp327)!= 4)
goto _LL1E9;_tmp332=((struct Cyc_Absyn_PointerType_struct*)_tmp327)->f1;_tmp333=(
void*)_tmp332.elt_typ;_tmp334=_tmp332.ptr_atts;_tmp335=(void*)_tmp334.rgn;_LL1E8:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp335) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp333);_LL1E9: if(_tmp327 <= (void*)4)goto _LL1EB;if(*((int*)_tmp327)!= 8)
goto _LL1EB;_LL1EA: return 0;_LL1EB: if(_tmp327 <= (void*)4)goto _LL1ED;if(*((int*)
_tmp327)!= 9)goto _LL1ED;_tmp336=((struct Cyc_Absyn_TupleType_struct*)_tmp327)->f1;
_LL1EC: for(0;_tmp336 != 0;_tmp336=_tmp336->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(*((struct _tuple11*)_tmp336->hd)).f2))return 1;}return 0;_LL1ED: if(_tmp327 <= (
void*)4)goto _LL1EF;if(*((int*)_tmp327)!= 11)goto _LL1EF;_tmp337=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp327)->f2;_LL1EE: for(0;_tmp337 != 0;_tmp337=_tmp337->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp337->hd)->type))return 1;}return 0;
_LL1EF: if(_tmp327 <= (void*)4)goto _LL1F1;if(*((int*)_tmp327)!= 7)goto _LL1F1;
_tmp338=((struct Cyc_Absyn_ArrayType_struct*)_tmp327)->f1;_tmp339=(void*)_tmp338.elt_type;
_LL1F0: _tmp33A=_tmp339;goto _LL1F2;_LL1F1: if(_tmp327 <= (void*)4)goto _LL1F3;if(*((
int*)_tmp327)!= 19)goto _LL1F3;_tmp33A=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp327)->f1;_LL1F2: _tmp33B=_tmp33A;goto _LL1F4;_LL1F3: if(_tmp327 <= (void*)4)goto
_LL1F5;if(*((int*)_tmp327)!= 21)goto _LL1F5;_tmp33B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp327)->f1;_LL1F4: return Cyc_CfFlowInfo_contains_region(rgn,_tmp33B);_LL1F5: if(
_tmp327 <= (void*)4)goto _LL1F7;if(*((int*)_tmp327)!= 15)goto _LL1F7;_tmp33C=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp327)->f1;_tmp33D=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp327)->f2;_LL1F6: return Cyc_CfFlowInfo_contains_region(rgn,_tmp33C) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp33D);_LL1F7: if(_tmp327 <= (void*)4)goto _LL1F9;if(*((int*)_tmp327)!= 14)
goto _LL1F9;_tmp33E=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp327)->f1;
_LL1F8: return 0;_LL1F9: if(_tmp327 <= (void*)4)goto _LL1C6;if(*((int*)_tmp327)!= 17)
goto _LL1C6;_LL1FA: return 0;_LL1C6:;}struct _tuple18{struct Cyc_CfFlowInfo_FlowEnv*
f1;struct Cyc_Dict_Dict f2;void*f3;};static void Cyc_CfFlowInfo_kill_root(struct
_tuple18*env,void*root,void*rval){struct _tuple18 _tmp343;struct Cyc_CfFlowInfo_FlowEnv*
_tmp344;struct Cyc_Dict_Dict _tmp345;struct Cyc_Dict_Dict*_tmp346;void*_tmp347;
struct _tuple18*_tmp342=env;_tmp343=*_tmp342;_tmp344=_tmp343.f1;_tmp345=_tmp343.f2;
_tmp346=(struct Cyc_Dict_Dict*)&(*_tmp342).f2;_tmp347=_tmp343.f3;{void*_tmp348=
root;struct Cyc_Absyn_Vardecl*_tmp349;void*_tmp34A;_LL206: if(*((int*)_tmp348)!= 0)
goto _LL208;_tmp349=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp348)->f1;_LL207:
if(Cyc_CfFlowInfo_contains_region(_tmp347,(void*)_tmp349->type))rval=Cyc_CfFlowInfo_typ_to_absrval(
_tmp344,(void*)_tmp349->type,(void*)_tmp344->unknown_none);*_tmp346=Cyc_Dict_insert(*
_tmp346,root,rval);goto _LL205;_LL208: if(*((int*)_tmp348)!= 1)goto _LL20A;_tmp34A=(
void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp348)->f2;_LL209: if(!Cyc_CfFlowInfo_contains_region(
_tmp347,_tmp34A))*_tmp346=Cyc_Dict_insert(*_tmp346,root,rval);goto _LL205;_LL20A:
if(*((int*)_tmp348)!= 2)goto _LL205;_LL20B: goto _LL205;_LL205:;}}static struct Cyc_Dict_Dict
Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict
fd,void*rgn){void*_tmp34B=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp34C;
_LL20D: if(_tmp34B <= (void*)4)goto _LL20F;if(*((int*)_tmp34B)!= 1)goto _LL20F;
_tmp34C=((struct Cyc_Absyn_VarType_struct*)_tmp34B)->f1;_LL20E: {struct Cyc_CfFlowInfo_Region_k_struct
v=({struct Cyc_CfFlowInfo_Region_k_struct _tmp34E;_tmp34E.tag=0;_tmp34E.f1=_tmp34C;
_tmp34E;});void*v2=(void*)& v;struct _tuple18 env=({struct _tuple18 _tmp34D;_tmp34D.f1=
fenv;_tmp34D.f2=fenv->mt_flowdict;_tmp34D.f3=v2;_tmp34D;});((void(*)(void(*f)(
struct _tuple18*,void*,void*),struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f2;}_LL20F: if((int)_tmp34B != 3)goto
_LL211;_LL210: {struct _tuple18 env=({struct _tuple18 _tmp34F;_tmp34F.f1=fenv;
_tmp34F.f2=fenv->mt_flowdict;_tmp34F.f3=(void*)0;_tmp34F;});((void(*)(void(*f)(
struct _tuple18*,void*,void*),struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f2;}_LL211:;_LL212:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp350=_cycalloc(sizeof(*_tmp350));_tmp350[0]=({
struct Cyc_Core_Impossible_struct _tmp351;_tmp351.tag=Cyc_Core_Impossible;_tmp351.f1=({
const char*_tmp352="kill_flowdict_region";_tag_dyneither(_tmp352,sizeof(char),21);});
_tmp351;});_tmp350;}));_LL20C:;}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn){return 0;}union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union
f,void*rgn){union Cyc_CfFlowInfo_FlowInfo_union _tmp353=f;struct Cyc_Dict_Dict
_tmp354;struct Cyc_List_List*_tmp355;struct Cyc_CfFlowInfo_ConsumeInfo _tmp356;
_LL214: if((_tmp353.BottomFL).tag != 0)goto _LL216;_LL215: return f;_LL216: if((
_tmp353.ReachableFL).tag != 1)goto _LL213;_tmp354=(_tmp353.ReachableFL).f1;_tmp355=(
_tmp353.ReachableFL).f2;_tmp356=(_tmp353.ReachableFL).f3;_LL217: {struct Cyc_Dict_Dict
_tmp357=Cyc_CfFlowInfo_kill_flowdict_region(fenv,_tmp354,rgn);struct Cyc_List_List*
_tmp358=Cyc_CfFlowInfo_kill_relns_region(_tmp355,rgn);return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp359;(_tmp359.ReachableFL).tag=1;(_tmp359.ReachableFL).f1=
_tmp357;(_tmp359.ReachableFL).f2=_tmp358;(_tmp359.ReachableFL).f3=_tmp356;
_tmp359;});}_LL213:;}static struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of_rec(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds,struct Cyc_List_List*
path){void*_tmp35A=(void*)e->r;struct Cyc_Absyn_Exp*_tmp35B;struct Cyc_Absyn_Exp*
_tmp35C;struct Cyc_Absyn_Exp*_tmp35D;void*_tmp35E;struct Cyc_Absyn_Vardecl*_tmp35F;
void*_tmp360;struct Cyc_Absyn_Vardecl*_tmp361;void*_tmp362;struct Cyc_Absyn_Vardecl*
_tmp363;void*_tmp364;struct Cyc_Absyn_Vardecl*_tmp365;struct Cyc_Absyn_Exp*_tmp366;
struct _dyneither_ptr*_tmp367;struct Cyc_Absyn_Exp*_tmp368;struct Cyc_Absyn_Exp*
_tmp369;struct Cyc_Absyn_Exp*_tmp36A;struct Cyc_Absyn_Exp*_tmp36B;struct
_dyneither_ptr*_tmp36C;_LL219: if(*((int*)_tmp35A)!= 15)goto _LL21B;_tmp35B=((
struct Cyc_Absyn_Cast_e_struct*)_tmp35A)->f2;_LL21A: _tmp35C=_tmp35B;goto _LL21C;
_LL21B: if(*((int*)_tmp35A)!= 13)goto _LL21D;_tmp35C=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp35A)->f1;_LL21C: _tmp35D=_tmp35C;goto _LL21E;_LL21D: if(*((int*)_tmp35A)!= 14)
goto _LL21F;_tmp35D=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp35A)->f1;_LL21E:
return Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp35D,flds,path);_LL21F: if(*((int*)
_tmp35A)!= 1)goto _LL221;_tmp35E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp35A)->f2;
if(_tmp35E <= (void*)1)goto _LL221;if(*((int*)_tmp35E)!= 2)goto _LL221;_tmp35F=((
struct Cyc_Absyn_Param_b_struct*)_tmp35E)->f1;_LL220: _tmp361=_tmp35F;goto _LL222;
_LL221: if(*((int*)_tmp35A)!= 1)goto _LL223;_tmp360=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp35A)->f2;if(_tmp360 <= (void*)1)goto _LL223;if(*((int*)_tmp360)!= 3)goto _LL223;
_tmp361=((struct Cyc_Absyn_Local_b_struct*)_tmp360)->f1;_LL222: _tmp363=_tmp361;
goto _LL224;_LL223: if(*((int*)_tmp35A)!= 1)goto _LL225;_tmp362=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp35A)->f2;if(_tmp362 <= (void*)1)goto _LL225;if(*((int*)_tmp362)!= 4)goto _LL225;
_tmp363=((struct Cyc_Absyn_Pat_b_struct*)_tmp362)->f1;_LL224: return({struct Cyc_CfFlowInfo_UniquePlace*
_tmp36D=_region_malloc(r,sizeof(*_tmp36D));_tmp36D->place=({struct Cyc_CfFlowInfo_Place
_tmp36E;_tmp36E.root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp36F=_region_malloc(r,sizeof(*_tmp36F));_tmp36F[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp370;_tmp370.tag=0;_tmp370.f1=_tmp363;_tmp370;});_tmp36F;}));_tmp36E.fields=
flds;_tmp36E;});_tmp36D->path=path;_tmp36D;});_LL225: if(*((int*)_tmp35A)!= 1)
goto _LL227;_tmp364=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp35A)->f2;if(
_tmp364 <= (void*)1)goto _LL227;if(*((int*)_tmp364)!= 0)goto _LL227;_tmp365=((
struct Cyc_Absyn_Global_b_struct*)_tmp364)->f1;_LL226:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp371=_cycalloc(sizeof(*_tmp371));_tmp371[0]=({
struct Cyc_Core_Impossible_struct _tmp372;_tmp372.tag=Cyc_Core_Impossible;_tmp372.f1=({
const char*_tmp373="non-unique path";_tag_dyneither(_tmp373,sizeof(char),16);});
_tmp372;});_tmp371;}));_LL227: if(*((int*)_tmp35A)!= 24)goto _LL229;_tmp366=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp35A)->f1;_tmp367=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp35A)->f2;_LL228:{void*_tmp374=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp366->topt))->v);struct Cyc_Absyn_PtrInfo _tmp375;void*_tmp376;
_LL232: if(_tmp374 <= (void*)4)goto _LL234;if(*((int*)_tmp374)!= 4)goto _LL234;
_tmp375=((struct Cyc_Absyn_PointerType_struct*)_tmp374)->f1;_tmp376=(void*)
_tmp375.elt_typ;_LL233: if(!Cyc_Absyn_is_union_type(_tmp376))flds=({struct Cyc_List_List*
_tmp377=_region_malloc(r,sizeof(*_tmp377));_tmp377->hd=(void*)Cyc_CfFlowInfo_get_field_index(
_tmp376,_tmp367);_tmp377->tl=flds;_tmp377;});goto _LL231;_LL234:;_LL235:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp378=_cycalloc(sizeof(*
_tmp378));_tmp378[0]=({struct Cyc_Core_Impossible_struct _tmp379;_tmp379.tag=Cyc_Core_Impossible;
_tmp379.f1=({const char*_tmp37A="anal_Rexp: AggrArrow ptr";_tag_dyneither(_tmp37A,
sizeof(char),25);});_tmp379;});_tmp378;}));_LL231:;}_tmp368=_tmp366;goto _LL22A;
_LL229: if(*((int*)_tmp35A)!= 22)goto _LL22B;_tmp368=((struct Cyc_Absyn_Deref_e_struct*)
_tmp35A)->f1;_LL22A: return Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp368,0,({
struct Cyc_List_List*_tmp37B=_region_malloc(r,sizeof(*_tmp37B));_tmp37B->hd=flds;
_tmp37B->tl=path;_tmp37B;}));_LL22B: if(*((int*)_tmp35A)!= 25)goto _LL22D;_tmp369=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp35A)->f1;_tmp36A=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp35A)->f2;_LL22C: {void*_tmp37C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp369->topt))->v);struct Cyc_Absyn_PtrInfo _tmp37D;void*_tmp37E;
struct Cyc_Absyn_PtrAtts _tmp37F;struct Cyc_Absyn_Conref*_tmp380;_LL237: if(_tmp37C
<= (void*)4)goto _LL23B;if(*((int*)_tmp37C)!= 9)goto _LL239;_LL238: {unsigned int
_tmp381=(Cyc_Evexp_eval_const_uint_exp(_tmp36A)).f1;return Cyc_CfFlowInfo_unique_place_of_rec(
r,_tmp369,({struct Cyc_List_List*_tmp382=_region_malloc(r,sizeof(*_tmp382));
_tmp382->hd=(void*)((int)_tmp381);_tmp382->tl=flds;_tmp382;}),path);}_LL239: if(*((
int*)_tmp37C)!= 4)goto _LL23B;_tmp37D=((struct Cyc_Absyn_PointerType_struct*)
_tmp37C)->f1;_tmp37E=(void*)_tmp37D.elt_typ;_tmp37F=_tmp37D.ptr_atts;_tmp380=
_tmp37F.bounds;_LL23A:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383[0]=({struct Cyc_Core_Impossible_struct
_tmp384;_tmp384.tag=Cyc_Core_Impossible;_tmp384.f1=({const char*_tmp385="subscripts unsupported for unique places";
_tag_dyneither(_tmp385,sizeof(char),41);});_tmp384;});_tmp383;}));_LL23B:;_LL23C:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp386=_cycalloc(sizeof(*
_tmp386));_tmp386[0]=({struct Cyc_Core_Impossible_struct _tmp387;_tmp387.tag=Cyc_Core_Impossible;
_tmp387.f1=({const char*_tmp388="unique place subscript -- bad type";
_tag_dyneither(_tmp388,sizeof(char),35);});_tmp387;});_tmp386;}));_LL236:;}
_LL22D: if(*((int*)_tmp35A)!= 23)goto _LL22F;_tmp36B=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp35A)->f1;_tmp36C=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp35A)->f2;_LL22E:
if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp36B->topt))->v))(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp389=_cycalloc(sizeof(*
_tmp389));_tmp389[0]=({struct Cyc_Core_Impossible_struct _tmp38A;_tmp38A.tag=Cyc_Core_Impossible;
_tmp38A.f1=({const char*_tmp38B="unions can't contain unique places";
_tag_dyneither(_tmp38B,sizeof(char),35);});_tmp38A;});_tmp389;}));{int _tmp38C=
Cyc_CfFlowInfo_get_field_index((void*)((struct Cyc_Core_Opt*)_check_null(_tmp36B->topt))->v,
_tmp36C);return Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp36B,({struct Cyc_List_List*
_tmp38D=_region_malloc(r,sizeof(*_tmp38D));_tmp38D->hd=(void*)_tmp38C;_tmp38D->tl=
flds;_tmp38D;}),path);}_LL22F:;_LL230:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp38E=_cycalloc(sizeof(*_tmp38E));_tmp38E[0]=({struct Cyc_Core_Impossible_struct
_tmp38F;_tmp38F.tag=Cyc_Core_Impossible;_tmp38F.f1=({const char*_tmp390="bad unique path";
_tag_dyneither(_tmp390,sizeof(char),16);});_tmp38F;});_tmp38E;}));_LL218:;}
struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of(struct
_RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_CfFlowInfo_unique_place_of_rec(
r,e,0,0);}struct _tuple19{struct Cyc_Dict_Dict*f1;struct Cyc_Dict_Dict f2;struct Cyc_Position_Segment*
f3;};static void Cyc_CfFlowInfo_consume_f(struct _tuple19*env,struct Cyc_CfFlowInfo_Place*
place){struct _tuple19 _tmp392;struct Cyc_Dict_Dict*_tmp393;struct Cyc_Dict_Dict
_tmp394;struct Cyc_Position_Segment*_tmp395;struct _tuple19*_tmp391=env;_tmp392=*
_tmp391;_tmp393=_tmp392.f1;_tmp394=_tmp392.f2;_tmp395=_tmp392.f3;{void*rval=(
void*)0;if(Cyc_Dict_lookup_bool(_tmp394,(void*)place->root,& rval) && rval == (
void*)0)return;if(((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)((struct
Cyc_Dict_Dict*)_tmp393,place,_tmp395)){struct Cyc_Position_Segment*_tmp396=((
struct Cyc_Position_Segment*(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))
Cyc_Dict_lookup)(*_tmp393,place);((void(*)(struct Cyc_CfFlowInfo_Place*place,
struct _dyneither_ptr(*toString)(struct Cyc_CfFlowInfo_Place*),struct
_dyneither_ptr err_msg1,struct _dyneither_ptr err_msg2,struct Cyc_Position_Segment*
consumed_loc,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_unique_err)(place,
Cyc_CfFlowInfo_place_err_string,({const char*_tmp397="May consume non-aliased pointer %s more than once (cf. %s)";
_tag_dyneither(_tmp397,sizeof(char),59);}),({const char*_tmp398="May consume non-aliased pointer %s more than once";
_tag_dyneither(_tmp398,sizeof(char),50);}),_tmp396,_tmp395);}}}union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f){union Cyc_CfFlowInfo_FlowInfo_union _tmp399=f;struct Cyc_Dict_Dict _tmp39A;struct
Cyc_List_List*_tmp39B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp39C;_LL23E: if((_tmp399.BottomFL).tag
!= 0)goto _LL240;_LL23F: return f;_LL240: if((_tmp399.ReachableFL).tag != 1)goto
_LL23D;_tmp39A=(_tmp399.ReachableFL).f1;_tmp39B=(_tmp399.ReachableFL).f2;_tmp39C=(
_tmp399.ReachableFL).f3;_LL241: if(_tmp39C.may_consume == 0)return f;{struct Cyc_Dict_Dict
_tmp39D=_tmp39C.consumed;struct _tuple19 _tmp39E=({struct _tuple19 _tmp3A1;_tmp3A1.f1=&
_tmp39D;_tmp3A1.f2=_tmp39A;_tmp3A1.f3=loc;_tmp3A1;});((void(*)(void(*f)(struct
_tuple19*,struct Cyc_CfFlowInfo_Place*),struct _tuple19*env,struct Cyc_List_List*x))
Cyc_List_iter_c)(Cyc_CfFlowInfo_consume_f,& _tmp39E,_tmp39C.may_consume);return(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp39F;(
_tmp39F.ReachableFL).tag=1;(_tmp39F.ReachableFL).f1=_tmp39A;(_tmp39F.ReachableFL).f2=
_tmp39B;(_tmp39F.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp3A0;
_tmp3A0.consumed=_tmp39D;_tmp3A0.may_consume=0;_tmp3A0;});_tmp39F;});}_LL23D:;}
void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f){union Cyc_CfFlowInfo_FlowInfo_union _tmp3A2=f;struct Cyc_Dict_Dict _tmp3A3;struct
Cyc_List_List*_tmp3A4;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3A5;_LL243: if((_tmp3A2.BottomFL).tag
!= 0)goto _LL245;_LL244: return;_LL245: if((_tmp3A2.ReachableFL).tag != 1)goto _LL242;
_tmp3A3=(_tmp3A2.ReachableFL).f1;_tmp3A4=(_tmp3A2.ReachableFL).f2;_tmp3A5=(
_tmp3A2.ReachableFL).f3;_LL246: {struct Cyc_List_List*_tmp3A6=_tmp3A5.may_consume;
while(_tmp3A6 != 0){if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))
Cyc_Dict_member)(_tmp3A5.consumed,(struct Cyc_CfFlowInfo_Place*)_tmp3A6->hd)){
struct Cyc_Position_Segment*_tmp3A7=((struct Cyc_Position_Segment*(*)(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_lookup)(_tmp3A5.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp3A6->hd);((void(*)(struct Cyc_CfFlowInfo_Place*place,struct _dyneither_ptr(*
toString)(struct Cyc_CfFlowInfo_Place*),struct _dyneither_ptr err_msg1,struct
_dyneither_ptr err_msg2,struct Cyc_Position_Segment*consumed_loc,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_unique_err)((struct Cyc_CfFlowInfo_Place*)_tmp3A6->hd,Cyc_CfFlowInfo_place_err_string,({
const char*_tmp3A8="Read through possibly consumed unique pointer %s (cf. %s)";
_tag_dyneither(_tmp3A8,sizeof(char),58);}),({const char*_tmp3A9="Read through possibly consumed unique pointer %s";
_tag_dyneither(_tmp3A9,sizeof(char),49);}),_tmp3A7,loc);break;}_tmp3A6=_tmp3A6->tl;}
return;}_LL242:;}union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_drop_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f){union Cyc_CfFlowInfo_FlowInfo_union
_tmp3AA=f;struct Cyc_Dict_Dict _tmp3AB;struct Cyc_List_List*_tmp3AC;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3AD;_LL248: if((_tmp3AA.BottomFL).tag != 0)goto _LL24A;_LL249: return f;_LL24A:
if((_tmp3AA.ReachableFL).tag != 1)goto _LL247;_tmp3AB=(_tmp3AA.ReachableFL).f1;
_tmp3AC=(_tmp3AA.ReachableFL).f2;_tmp3AD=(_tmp3AA.ReachableFL).f3;_LL24B: _tmp3AD.may_consume=
0;return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp3AE;(_tmp3AE.ReachableFL).tag=1;(_tmp3AE.ReachableFL).f1=_tmp3AB;(_tmp3AE.ReachableFL).f2=
_tmp3AC;(_tmp3AE.ReachableFL).f3=_tmp3AD;_tmp3AE;});_LL247:;}union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f){Cyc_CfFlowInfo_check_unique_rvals(loc,f);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp3AF=f;struct Cyc_Dict_Dict _tmp3B0;struct Cyc_List_List*_tmp3B1;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3B2;_LL24D: if((_tmp3AF.BottomFL).tag != 0)goto _LL24F;_LL24E: return f;_LL24F:
if((_tmp3AF.ReachableFL).tag != 1)goto _LL24C;_tmp3B0=(_tmp3AF.ReachableFL).f1;
_tmp3B1=(_tmp3AF.ReachableFL).f2;_tmp3B2=(_tmp3AF.ReachableFL).f3;_LL250: _tmp3B2.may_consume=
0;return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp3B3;(_tmp3B3.ReachableFL).tag=1;(_tmp3B3.ReachableFL).f1=_tmp3B0;(_tmp3B3.ReachableFL).f2=
_tmp3B1;(_tmp3B3.ReachableFL).f3=_tmp3B2;_tmp3B3;});_LL24C:;}}struct _tuple6 Cyc_CfFlowInfo_save_consume_info(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union f,int clear){
union Cyc_CfFlowInfo_FlowInfo_union _tmp3B4=f;struct Cyc_Dict_Dict _tmp3B5;struct Cyc_List_List*
_tmp3B6;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3B7;_LL252: if((_tmp3B4.BottomFL).tag
!= 0)goto _LL254;_LL253: return({struct _tuple6 _tmp3B8;_tmp3B8.f1=({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3B9;_tmp3B9.consumed=fenv->mt_place_set;_tmp3B9.may_consume=0;_tmp3B9;});
_tmp3B8.f2=f;_tmp3B8;});_LL254: if((_tmp3B4.ReachableFL).tag != 1)goto _LL251;
_tmp3B5=(_tmp3B4.ReachableFL).f1;_tmp3B6=(_tmp3B4.ReachableFL).f2;_tmp3B7=(
_tmp3B4.ReachableFL).f3;_LL255: if(clear)return({struct _tuple6 _tmp3BA;_tmp3BA.f1=
_tmp3B7;_tmp3BA.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp3BB;(_tmp3BB.ReachableFL).tag=1;(_tmp3BB.ReachableFL).f1=_tmp3B5;(_tmp3BB.ReachableFL).f2=
_tmp3B6;(_tmp3BB.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp3BC;
_tmp3BC.consumed=fenv->mt_place_set;_tmp3BC.may_consume=0;_tmp3BC;});_tmp3BB;});
_tmp3BA;});else{return({struct _tuple6 _tmp3BD;_tmp3BD.f1=_tmp3B7;_tmp3BD.f2=f;
_tmp3BD;});}_LL251:;}union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_restore_consume_info(
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_CfFlowInfo_ConsumeInfo c,int
may_consume_only){union Cyc_CfFlowInfo_FlowInfo_union _tmp3BE=f;struct Cyc_Dict_Dict
_tmp3BF;struct Cyc_List_List*_tmp3C0;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3C1;
_LL257: if((_tmp3BE.BottomFL).tag != 0)goto _LL259;_LL258: return f;_LL259: if((
_tmp3BE.ReachableFL).tag != 1)goto _LL256;_tmp3BF=(_tmp3BE.ReachableFL).f1;_tmp3C0=(
_tmp3BE.ReachableFL).f2;_tmp3C1=(_tmp3BE.ReachableFL).f3;_LL25A: if(
may_consume_only)return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp3C2;(_tmp3C2.ReachableFL).tag=1;(_tmp3C2.ReachableFL).f1=_tmp3BF;(_tmp3C2.ReachableFL).f2=
_tmp3C0;(_tmp3C2.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp3C3;
_tmp3C3.consumed=_tmp3C1.consumed;_tmp3C3.may_consume=c.may_consume;_tmp3C3;});
_tmp3C2;});return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp3C4;(_tmp3C4.ReachableFL).tag=1;(_tmp3C4.ReachableFL).f1=_tmp3BF;(_tmp3C4.ReachableFL).f2=
_tmp3C0;(_tmp3C4.ReachableFL).f3=c;_tmp3C4;});_LL256:;}
