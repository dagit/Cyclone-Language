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
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];extern char
Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];
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
char*tag;struct _dyneither_ptr f1;};int Cyc_strptrcmp(struct _dyneither_ptr*s1,
struct _dyneither_ptr*s2);struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[
11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char
Cyc_Dict_Absent[11];struct _tuple3{void*f1;void*f2;};struct _tuple3*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple3*Cyc_Dict_rchoose(
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
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(
void*,void*);struct _tuple4{unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_okay_szofarg(void*t);struct _tuple5{union
Cyc_Absyn_Cnst_union f1;int f2;};static struct _tuple5 Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp*);static union Cyc_Absyn_Cnst_union Cyc_Evexp_promote_const(
union Cyc_Absyn_Cnst_union cn){union Cyc_Absyn_Cnst_union _tmp0=cn;void*_tmp1;char
_tmp2;void*_tmp3;short _tmp4;_LL1: if((_tmp0.Char_c).tag != 0)goto _LL3;_tmp1=(_tmp0.Char_c).f1;
_tmp2=(_tmp0.Char_c).f2;_LL2: return(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp5;(_tmp5.Int_c).tag=2;(_tmp5.Int_c).f1=(void*)_tmp1;(_tmp5.Int_c).f2=(int)
_tmp2;_tmp5;});_LL3: if((_tmp0.Short_c).tag != 1)goto _LL5;_tmp3=(_tmp0.Short_c).f1;
_tmp4=(_tmp0.Short_c).f2;_LL4: return(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp6;(_tmp6.Int_c).tag=2;(_tmp6.Int_c).f1=(void*)_tmp3;(_tmp6.Int_c).f2=(int)
_tmp4;_tmp6;});_LL5:;_LL6: return cn;_LL0:;}struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e){union Cyc_Absyn_Cnst_union _tmp8;int _tmp9;struct _tuple5
_tmp7=Cyc_Evexp_eval_const_exp(e);_tmp8=_tmp7.f1;_tmp9=_tmp7.f2;if(!_tmp9)return({
struct _tuple4 _tmpA;_tmpA.f1=0;_tmpA.f2=0;_tmpA;});{union Cyc_Absyn_Cnst_union
_tmpB=Cyc_Evexp_promote_const(_tmp8);int _tmpC;_LL8: if((_tmpB.Int_c).tag != 2)goto
_LLA;_tmpC=(_tmpB.Int_c).f2;_LL9: return({struct _tuple4 _tmpD;_tmpD.f1=(
unsigned int)_tmpC;_tmpD.f2=1;_tmpD;});_LLA:;_LLB:({void*_tmpE=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmpF="expecting unsigned int";_tag_dyneither(_tmpF,sizeof(
char),23);}),_tag_dyneither(_tmpE,sizeof(void*),0));});return({struct _tuple4
_tmp10;_tmp10.f1=0;_tmp10.f2=1;_tmp10;});_LL7:;}}struct _tuple6{int f1;int f2;};
static struct _tuple6 Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*e){union Cyc_Absyn_Cnst_union
_tmp12;int _tmp13;struct _tuple5 _tmp11=Cyc_Evexp_eval_const_exp(e);_tmp12=_tmp11.f1;
_tmp13=_tmp11.f2;if(!_tmp13)return({struct _tuple6 _tmp14;_tmp14.f1=0;_tmp14.f2=0;
_tmp14;});{union Cyc_Absyn_Cnst_union _tmp15=Cyc_Evexp_promote_const(_tmp12);int
_tmp16;long long _tmp17;_LLD: if((_tmp15.Int_c).tag != 2)goto _LLF;_tmp16=(_tmp15.Int_c).f2;
_LLE: return({struct _tuple6 _tmp18;_tmp18.f1=_tmp16 != 0;_tmp18.f2=1;_tmp18;});_LLF:
if((_tmp15.LongLong_c).tag != 3)goto _LL11;_tmp17=(_tmp15.LongLong_c).f2;_LL10:
return({struct _tuple6 _tmp19;_tmp19.f1=_tmp17 != 0;_tmp19.f2=1;_tmp19;});_LL11: if((
_tmp15.Null_c).tag != 6)goto _LL13;_LL12: return({struct _tuple6 _tmp1A;_tmp1A.f1=0;
_tmp1A.f2=0;_tmp1A;});_LL13:;_LL14:({void*_tmp1B=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp1C="expecting bool";_tag_dyneither(_tmp1C,sizeof(char),15);}),
_tag_dyneither(_tmp1B,sizeof(void*),0));});return({struct _tuple6 _tmp1D;_tmp1D.f1=
0;_tmp1D.f2=0;_tmp1D;});_LLC:;}}struct _tuple7{void*f1;union Cyc_Absyn_Cnst_union
f2;};static struct _tuple5 Cyc_Evexp_eval_const_unprimop(void*p,struct Cyc_Absyn_Exp*
e){union Cyc_Absyn_Cnst_union _tmp1F;int _tmp20;struct _tuple5 _tmp1E=Cyc_Evexp_eval_const_exp(
e);_tmp1F=_tmp1E.f1;_tmp20=_tmp1E.f2;if(!_tmp20)return({struct _tuple5 _tmp21;
_tmp21.f1=_tmp1F;_tmp21.f2=0;_tmp21;});{struct _tuple7 _tmp23=({struct _tuple7
_tmp22;_tmp22.f1=p;_tmp22.f2=_tmp1F;_tmp22;});void*_tmp24;void*_tmp25;union Cyc_Absyn_Cnst_union
_tmp26;void*_tmp27;int _tmp28;void*_tmp29;union Cyc_Absyn_Cnst_union _tmp2A;void*
_tmp2B;int _tmp2C;void*_tmp2D;union Cyc_Absyn_Cnst_union _tmp2E;int _tmp2F;void*
_tmp30;union Cyc_Absyn_Cnst_union _tmp31;_LL16: _tmp24=_tmp23.f1;if((int)_tmp24 != 0)
goto _LL18;_LL17: goto _LL15;_LL18: _tmp25=_tmp23.f1;if((int)_tmp25 != 2)goto _LL1A;
_tmp26=_tmp23.f2;if(((_tmp23.f2).Int_c).tag != 2)goto _LL1A;_tmp27=(_tmp26.Int_c).f1;
_tmp28=(_tmp26.Int_c).f2;_LL19: _tmp1F=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp32;(_tmp32.Int_c).tag=2;(_tmp32.Int_c).f1=(void*)((void*)0);(_tmp32.Int_c).f2=
- _tmp28;_tmp32;});goto _LL15;_LL1A: _tmp29=_tmp23.f1;if((int)_tmp29 != 12)goto _LL1C;
_tmp2A=_tmp23.f2;if(((_tmp23.f2).Int_c).tag != 2)goto _LL1C;_tmp2B=(_tmp2A.Int_c).f1;
_tmp2C=(_tmp2A.Int_c).f2;_LL1B: _tmp1F=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp33;(_tmp33.Int_c).tag=2;(_tmp33.Int_c).f1=(void*)((void*)1);(_tmp33.Int_c).f2=
~ _tmp2C;_tmp33;});goto _LL15;_LL1C: _tmp2D=_tmp23.f1;if((int)_tmp2D != 11)goto _LL1E;
_tmp2E=_tmp23.f2;if(((_tmp23.f2).Int_c).tag != 2)goto _LL1E;_tmp2F=(_tmp2E.Int_c).f2;
_LL1D: _tmp1F=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp34;(
_tmp34.Int_c).tag=2;(_tmp34.Int_c).f1=(void*)((void*)0);(_tmp34.Int_c).f2=_tmp2F
== 0?1: 0;_tmp34;});goto _LL15;_LL1E: _tmp30=_tmp23.f1;if((int)_tmp30 != 11)goto
_LL20;_tmp31=_tmp23.f2;if(((_tmp23.f2).Null_c).tag != 6)goto _LL20;_LL1F: _tmp1F=(
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp35;(_tmp35.Int_c).tag=2;(
_tmp35.Int_c).f1=(void*)((void*)0);(_tmp35.Int_c).f2=1;_tmp35;});goto _LL15;_LL20:;
_LL21:({void*_tmp36=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp37="bad constant expression";
_tag_dyneither(_tmp37,sizeof(char),24);}),_tag_dyneither(_tmp36,sizeof(void*),0));});
goto _LL15;_LL15:;}return({struct _tuple5 _tmp38;_tmp38.f1=_tmp1F;_tmp38.f2=1;
_tmp38;});}struct _tuple8{void*f1;int f2;};static struct _tuple5 Cyc_Evexp_eval_const_binprimop(
void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){union Cyc_Absyn_Cnst_union
_tmp3A;int _tmp3B;struct _tuple5 _tmp39=Cyc_Evexp_eval_const_exp(e1);_tmp3A=_tmp39.f1;
_tmp3B=_tmp39.f2;{union Cyc_Absyn_Cnst_union _tmp3D;int _tmp3E;struct _tuple5 _tmp3C=
Cyc_Evexp_eval_const_exp(e2);_tmp3D=_tmp3C.f1;_tmp3E=_tmp3C.f2;if(!_tmp3B  || !
_tmp3E)return({struct _tuple5 _tmp3F;_tmp3F.f1=_tmp3A;_tmp3F.f2=0;_tmp3F;});_tmp3A=
Cyc_Evexp_promote_const(_tmp3A);_tmp3D=Cyc_Evexp_promote_const(_tmp3D);{void*s1;
void*s2;int i1;int i2;{union Cyc_Absyn_Cnst_union _tmp40=_tmp3A;void*_tmp41;int
_tmp42;_LL23: if((_tmp40.Int_c).tag != 2)goto _LL25;_tmp41=(_tmp40.Int_c).f1;_tmp42=(
_tmp40.Int_c).f2;_LL24: s1=_tmp41;i1=_tmp42;goto _LL22;_LL25:;_LL26:({void*_tmp43=
0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp44="bad constant expression";
_tag_dyneither(_tmp44,sizeof(char),24);}),_tag_dyneither(_tmp43,sizeof(void*),0));});
return({struct _tuple5 _tmp45;_tmp45.f1=_tmp3A;_tmp45.f2=1;_tmp45;});_LL22:;}{
union Cyc_Absyn_Cnst_union _tmp46=_tmp3D;void*_tmp47;int _tmp48;_LL28: if((_tmp46.Int_c).tag
!= 2)goto _LL2A;_tmp47=(_tmp46.Int_c).f1;_tmp48=(_tmp46.Int_c).f2;_LL29: s2=_tmp47;
i2=_tmp48;goto _LL27;_LL2A:;_LL2B:({void*_tmp49=0;Cyc_Tcutil_terr(e2->loc,({const
char*_tmp4A="bad constant expression";_tag_dyneither(_tmp4A,sizeof(char),24);}),
_tag_dyneither(_tmp49,sizeof(void*),0));});return({struct _tuple5 _tmp4B;_tmp4B.f1=
_tmp3A;_tmp4B.f2=1;_tmp4B;});_LL27:;}{void*_tmp4C=p;_LL2D: if((int)_tmp4C != 3)
goto _LL2F;_LL2E: goto _LL30;_LL2F: if((int)_tmp4C != 4)goto _LL31;_LL30: if(i2 == 0){({
void*_tmp4D=0;Cyc_Tcutil_terr(e2->loc,({const char*_tmp4E="division by zero in constant expression";
_tag_dyneither(_tmp4E,sizeof(char),40);}),_tag_dyneither(_tmp4D,sizeof(void*),0));});
return({struct _tuple5 _tmp4F;_tmp4F.f1=_tmp3A;_tmp4F.f2=1;_tmp4F;});}goto _LL2C;
_LL31:;_LL32: goto _LL2C;_LL2C:;}{int has_u_arg=s1 == (void*)1  || s2 == (void*)1;
unsigned int u1=(unsigned int)i1;unsigned int u2=(unsigned int)i2;int i3=0;
unsigned int u3=0;int b3=1;int use_i3=0;int use_u3=0;int use_b3=0;{struct _tuple8
_tmp51=({struct _tuple8 _tmp50;_tmp50.f1=p;_tmp50.f2=has_u_arg;_tmp50;});void*
_tmp52;int _tmp53;void*_tmp54;int _tmp55;void*_tmp56;int _tmp57;void*_tmp58;int
_tmp59;void*_tmp5A;int _tmp5B;void*_tmp5C;int _tmp5D;void*_tmp5E;int _tmp5F;void*
_tmp60;int _tmp61;void*_tmp62;int _tmp63;void*_tmp64;int _tmp65;void*_tmp66;void*
_tmp67;void*_tmp68;int _tmp69;void*_tmp6A;int _tmp6B;void*_tmp6C;int _tmp6D;void*
_tmp6E;int _tmp6F;void*_tmp70;int _tmp71;void*_tmp72;int _tmp73;void*_tmp74;int
_tmp75;void*_tmp76;int _tmp77;void*_tmp78;void*_tmp79;void*_tmp7A;void*_tmp7B;
void*_tmp7C;void*_tmp7D;_LL34: _tmp52=_tmp51.f1;if((int)_tmp52 != 0)goto _LL36;
_tmp53=_tmp51.f2;if(_tmp53 != 0)goto _LL36;_LL35: i3=i1 + i2;use_i3=1;goto _LL33;
_LL36: _tmp54=_tmp51.f1;if((int)_tmp54 != 1)goto _LL38;_tmp55=_tmp51.f2;if(_tmp55 != 
0)goto _LL38;_LL37: i3=i1 * i2;use_i3=1;goto _LL33;_LL38: _tmp56=_tmp51.f1;if((int)
_tmp56 != 2)goto _LL3A;_tmp57=_tmp51.f2;if(_tmp57 != 0)goto _LL3A;_LL39: i3=i1 - i2;
use_i3=1;goto _LL33;_LL3A: _tmp58=_tmp51.f1;if((int)_tmp58 != 3)goto _LL3C;_tmp59=
_tmp51.f2;if(_tmp59 != 0)goto _LL3C;_LL3B: i3=i1 / i2;use_i3=1;goto _LL33;_LL3C:
_tmp5A=_tmp51.f1;if((int)_tmp5A != 4)goto _LL3E;_tmp5B=_tmp51.f2;if(_tmp5B != 0)
goto _LL3E;_LL3D: i3=i1 % i2;use_i3=1;goto _LL33;_LL3E: _tmp5C=_tmp51.f1;if((int)
_tmp5C != 0)goto _LL40;_tmp5D=_tmp51.f2;if(_tmp5D != 1)goto _LL40;_LL3F: u3=u1 + u2;
use_u3=1;goto _LL33;_LL40: _tmp5E=_tmp51.f1;if((int)_tmp5E != 1)goto _LL42;_tmp5F=
_tmp51.f2;if(_tmp5F != 1)goto _LL42;_LL41: u3=u1 * u2;use_u3=1;goto _LL33;_LL42:
_tmp60=_tmp51.f1;if((int)_tmp60 != 2)goto _LL44;_tmp61=_tmp51.f2;if(_tmp61 != 1)
goto _LL44;_LL43: u3=u1 - u2;use_u3=1;goto _LL33;_LL44: _tmp62=_tmp51.f1;if((int)
_tmp62 != 3)goto _LL46;_tmp63=_tmp51.f2;if(_tmp63 != 1)goto _LL46;_LL45: u3=u1 / u2;
use_u3=1;goto _LL33;_LL46: _tmp64=_tmp51.f1;if((int)_tmp64 != 4)goto _LL48;_tmp65=
_tmp51.f2;if(_tmp65 != 1)goto _LL48;_LL47: u3=u1 % u2;use_u3=1;goto _LL33;_LL48:
_tmp66=_tmp51.f1;if((int)_tmp66 != 5)goto _LL4A;_LL49: b3=i1 == i2;use_b3=1;goto
_LL33;_LL4A: _tmp67=_tmp51.f1;if((int)_tmp67 != 6)goto _LL4C;_LL4B: b3=i1 != i2;
use_b3=1;goto _LL33;_LL4C: _tmp68=_tmp51.f1;if((int)_tmp68 != 7)goto _LL4E;_tmp69=
_tmp51.f2;if(_tmp69 != 0)goto _LL4E;_LL4D: b3=i1 > i2;use_b3=1;goto _LL33;_LL4E:
_tmp6A=_tmp51.f1;if((int)_tmp6A != 8)goto _LL50;_tmp6B=_tmp51.f2;if(_tmp6B != 0)
goto _LL50;_LL4F: b3=i1 < i2;use_b3=1;goto _LL33;_LL50: _tmp6C=_tmp51.f1;if((int)
_tmp6C != 9)goto _LL52;_tmp6D=_tmp51.f2;if(_tmp6D != 0)goto _LL52;_LL51: b3=i1 >= i2;
use_b3=1;goto _LL33;_LL52: _tmp6E=_tmp51.f1;if((int)_tmp6E != 10)goto _LL54;_tmp6F=
_tmp51.f2;if(_tmp6F != 0)goto _LL54;_LL53: b3=i1 <= i2;use_b3=1;goto _LL33;_LL54:
_tmp70=_tmp51.f1;if((int)_tmp70 != 7)goto _LL56;_tmp71=_tmp51.f2;if(_tmp71 != 1)
goto _LL56;_LL55: b3=u1 > u2;use_b3=1;goto _LL33;_LL56: _tmp72=_tmp51.f1;if((int)
_tmp72 != 8)goto _LL58;_tmp73=_tmp51.f2;if(_tmp73 != 1)goto _LL58;_LL57: b3=u1 < u2;
use_b3=1;goto _LL33;_LL58: _tmp74=_tmp51.f1;if((int)_tmp74 != 9)goto _LL5A;_tmp75=
_tmp51.f2;if(_tmp75 != 1)goto _LL5A;_LL59: b3=u1 >= u2;use_b3=1;goto _LL33;_LL5A:
_tmp76=_tmp51.f1;if((int)_tmp76 != 10)goto _LL5C;_tmp77=_tmp51.f2;if(_tmp77 != 1)
goto _LL5C;_LL5B: b3=u1 <= u2;use_b3=1;goto _LL33;_LL5C: _tmp78=_tmp51.f1;if((int)
_tmp78 != 13)goto _LL5E;_LL5D: u3=u1 & u2;use_u3=1;goto _LL33;_LL5E: _tmp79=_tmp51.f1;
if((int)_tmp79 != 14)goto _LL60;_LL5F: u3=u1 | u2;use_u3=1;goto _LL33;_LL60: _tmp7A=
_tmp51.f1;if((int)_tmp7A != 15)goto _LL62;_LL61: u3=u1 ^ u2;use_u3=1;goto _LL33;_LL62:
_tmp7B=_tmp51.f1;if((int)_tmp7B != 16)goto _LL64;_LL63: u3=u1 << u2;use_u3=1;goto
_LL33;_LL64: _tmp7C=_tmp51.f1;if((int)_tmp7C != 17)goto _LL66;_LL65: u3=u1 >> u2;
use_u3=1;goto _LL33;_LL66: _tmp7D=_tmp51.f1;if((int)_tmp7D != 18)goto _LL68;_LL67:({
void*_tmp7E=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp7F=">>> NOT IMPLEMENTED";
_tag_dyneither(_tmp7F,sizeof(char),20);}),_tag_dyneither(_tmp7E,sizeof(void*),0));});
goto _LL69;_LL68:;_LL69:({void*_tmp80=0;Cyc_Tcutil_terr(e1->loc,({const char*
_tmp81="bad constant expression";_tag_dyneither(_tmp81,sizeof(char),24);}),
_tag_dyneither(_tmp80,sizeof(void*),0));});return({struct _tuple5 _tmp82;_tmp82.f1=
_tmp3A;_tmp82.f2=1;_tmp82;});_LL33:;}if(use_i3)return({struct _tuple5 _tmp83;
_tmp83.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp84;(_tmp84.Int_c).tag=
2;(_tmp84.Int_c).f1=(void*)((void*)0);(_tmp84.Int_c).f2=i3;_tmp84;});_tmp83.f2=1;
_tmp83;});if(use_u3)return({struct _tuple5 _tmp85;_tmp85.f1=(union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmp86;(_tmp86.Int_c).tag=2;(_tmp86.Int_c).f1=(void*)((
void*)1);(_tmp86.Int_c).f2=(int)u3;_tmp86;});_tmp85.f2=1;_tmp85;});if(use_b3)
return({struct _tuple5 _tmp87;_tmp87.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp88;(_tmp88.Int_c).tag=2;(_tmp88.Int_c).f1=(void*)((void*)0);(_tmp88.Int_c).f2=
b3?1: 0;_tmp88;});_tmp87.f2=1;_tmp87;});({void*_tmp89=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp8A="Evexp::eval_const_binop";
_tag_dyneither(_tmp8A,sizeof(char),24);}),_tag_dyneither(_tmp89,sizeof(void*),0));});}}}}
static struct _tuple5 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e){struct _tuple5
ans;{void*_tmp8B=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp8C;struct Cyc_Absyn_Exp*
_tmp8D;struct Cyc_Absyn_Exp*_tmp8E;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_Absyn_Exp*
_tmp90;struct Cyc_Absyn_Exp*_tmp91;struct Cyc_Absyn_Exp*_tmp92;struct Cyc_Absyn_Exp*
_tmp93;void*_tmp94;struct Cyc_List_List*_tmp95;void*_tmp96;void*_tmp97;struct Cyc_Absyn_Exp*
_tmp98;struct Cyc_Absyn_Enumfield*_tmp99;struct Cyc_Absyn_Enumfield*_tmp9A;_LL6B:
if(*((int*)_tmp8B)!= 0)goto _LL6D;_tmp8C=((struct Cyc_Absyn_Const_e_struct*)_tmp8B)->f1;
_LL6C: return({struct _tuple5 _tmp9B;_tmp9B.f1=_tmp8C;_tmp9B.f2=1;_tmp9B;});_LL6D:
if(*((int*)_tmp8B)!= 6)goto _LL6F;_tmp8D=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp8B)->f1;_tmp8E=((struct Cyc_Absyn_Conditional_e_struct*)_tmp8B)->f2;_tmp8F=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp8B)->f3;_LL6E: {int _tmp9D;int _tmp9E;
struct _tuple6 _tmp9C=Cyc_Evexp_eval_const_bool_exp(_tmp8D);_tmp9D=_tmp9C.f1;
_tmp9E=_tmp9C.f2;if(!_tmp9E){Cyc_Evexp_eval_const_exp(_tmp8E);Cyc_Evexp_eval_const_exp(
_tmp8F);return({struct _tuple5 _tmp9F;_tmp9F.f1=(union Cyc_Absyn_Cnst_union)({union
Cyc_Absyn_Cnst_union _tmpA0;(_tmpA0.Int_c).tag=2;(_tmpA0.Int_c).f1=(void*)((void*)
0);(_tmpA0.Int_c).f2=0;_tmpA0;});_tmp9F.f2=0;_tmp9F;});}ans=_tmp9D?Cyc_Evexp_eval_const_exp(
_tmp8E): Cyc_Evexp_eval_const_exp(_tmp8F);goto _LL6A;}_LL6F: if(*((int*)_tmp8B)!= 7)
goto _LL71;_tmp90=((struct Cyc_Absyn_And_e_struct*)_tmp8B)->f1;_tmp91=((struct Cyc_Absyn_And_e_struct*)
_tmp8B)->f2;_LL70: {int _tmpA2;int _tmpA3;struct _tuple6 _tmpA1=Cyc_Evexp_eval_const_bool_exp(
_tmp90);_tmpA2=_tmpA1.f1;_tmpA3=_tmpA1.f2;if(!_tmpA3){Cyc_Evexp_eval_const_exp(
_tmp91);return({struct _tuple5 _tmpA4;_tmpA4.f1=(union Cyc_Absyn_Cnst_union)({union
Cyc_Absyn_Cnst_union _tmpA5;(_tmpA5.Int_c).tag=2;(_tmpA5.Int_c).f1=(void*)((void*)
0);(_tmpA5.Int_c).f2=0;_tmpA5;});_tmpA4.f2=0;_tmpA4;});}if(_tmpA2)ans=Cyc_Evexp_eval_const_exp(
_tmp91);else{ans=({struct _tuple5 _tmpA6;_tmpA6.f1=(union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmpA7;(_tmpA7.Int_c).tag=2;(_tmpA7.Int_c).f1=(void*)((
void*)0);(_tmpA7.Int_c).f2=0;_tmpA7;});_tmpA6.f2=1;_tmpA6;});}goto _LL6A;}_LL71:
if(*((int*)_tmp8B)!= 8)goto _LL73;_tmp92=((struct Cyc_Absyn_Or_e_struct*)_tmp8B)->f1;
_tmp93=((struct Cyc_Absyn_Or_e_struct*)_tmp8B)->f2;_LL72: {int _tmpA9;int _tmpAA;
struct _tuple6 _tmpA8=Cyc_Evexp_eval_const_bool_exp(_tmp92);_tmpA9=_tmpA8.f1;
_tmpAA=_tmpA8.f2;if(!_tmpAA){Cyc_Evexp_eval_const_exp(_tmp93);return({struct
_tuple5 _tmpAB;_tmpAB.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmpAC;(_tmpAC.Int_c).tag=2;(_tmpAC.Int_c).f1=(void*)((void*)0);(_tmpAC.Int_c).f2=
0;_tmpAC;});_tmpAB.f2=0;_tmpAB;});}if(!_tmpA9)ans=Cyc_Evexp_eval_const_exp(
_tmp93);else{ans=({struct _tuple5 _tmpAD;_tmpAD.f1=(union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmpAE;(_tmpAE.Int_c).tag=2;(_tmpAE.Int_c).f1=(void*)((
void*)0);(_tmpAE.Int_c).f2=1;_tmpAE;});_tmpAD.f2=1;_tmpAD;});}goto _LL6A;}_LL73:
if(*((int*)_tmp8B)!= 3)goto _LL75;_tmp94=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp8B)->f1;_tmp95=((struct Cyc_Absyn_Primop_e_struct*)_tmp8B)->f2;_LL74: if(
_tmp95 == 0){({void*_tmpAF=0;Cyc_Tcutil_terr(e->loc,({const char*_tmpB0="bad static expression (no args to primop)";
_tag_dyneither(_tmpB0,sizeof(char),42);}),_tag_dyneither(_tmpAF,sizeof(void*),0));});
return({struct _tuple5 _tmpB1;_tmpB1.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmpB2;(_tmpB2.Int_c).tag=2;(_tmpB2.Int_c).f1=(void*)((void*)0);(_tmpB2.Int_c).f2=
0;_tmpB2;});_tmpB1.f2=1;_tmpB1;});}if(_tmp95->tl == 0){ans=Cyc_Evexp_eval_const_unprimop(
_tmp94,(struct Cyc_Absyn_Exp*)_tmp95->hd);goto _LL6A;}if(((struct Cyc_List_List*)
_check_null(_tmp95->tl))->tl == 0){ans=Cyc_Evexp_eval_const_binprimop(_tmp94,(
struct Cyc_Absyn_Exp*)_tmp95->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp95->tl))->hd);goto _LL6A;}({void*_tmpB3=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmpB4="bad static expression (too many args to primop)";
_tag_dyneither(_tmpB4,sizeof(char),48);}),_tag_dyneither(_tmpB3,sizeof(void*),0));});
return({struct _tuple5 _tmpB5;_tmpB5.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmpB6;(_tmpB6.Int_c).tag=2;(_tmpB6.Int_c).f1=(void*)((void*)0);(_tmpB6.Int_c).f2=
0;_tmpB6;});_tmpB5.f2=1;_tmpB5;});_LL75: if(*((int*)_tmp8B)!= 39)goto _LL77;_tmp96=(
void*)((struct Cyc_Absyn_Valueof_e_struct*)_tmp8B)->f1;_LL76:{void*_tmpB7=Cyc_Tcutil_compress(
_tmp96);struct Cyc_Absyn_Exp*_tmpB8;_LL86: if(_tmpB7 <= (void*)4)goto _LL88;if(*((
int*)_tmpB7)!= 17)goto _LL88;_tmpB8=((struct Cyc_Absyn_ValueofType_struct*)_tmpB7)->f1;
_LL87: return Cyc_Evexp_eval_const_exp(_tmpB8);_LL88:;_LL89: goto _LL85;_LL85:;}goto
_LL78;_LL77: if(*((int*)_tmp8B)!= 18)goto _LL79;_LL78: goto _LL7A;_LL79: if(*((int*)
_tmp8B)!= 19)goto _LL7B;_LL7A: goto _LL7C;_LL7B: if(*((int*)_tmp8B)!= 20)goto _LL7D;
_LL7C: ans=({struct _tuple5 _tmpB9;_tmpB9.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmpBA;(_tmpBA.Int_c).tag=2;(_tmpBA.Int_c).f1=(void*)((void*)0);(_tmpBA.Int_c).f2=
0;_tmpBA;});_tmpB9.f2=0;_tmpB9;});goto _LL6A;_LL7D: if(*((int*)_tmp8B)!= 15)goto
_LL7F;_tmp97=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp8B)->f1;_tmp98=((struct
Cyc_Absyn_Cast_e_struct*)_tmp8B)->f2;_LL7E: ans=Cyc_Evexp_eval_const_exp(_tmp98);
if(ans.f2){struct _tuple7 _tmpBC=({struct _tuple7 _tmpBB;_tmpBB.f1=Cyc_Tcutil_compress(
_tmp97);_tmpBB.f2=ans.f1;_tmpBB;});void*_tmpBD;void*_tmpBE;void*_tmpBF;union Cyc_Absyn_Cnst_union
_tmpC0;void*_tmpC1;char _tmpC2;void*_tmpC3;void*_tmpC4;void*_tmpC5;union Cyc_Absyn_Cnst_union
_tmpC6;void*_tmpC7;short _tmpC8;void*_tmpC9;void*_tmpCA;void*_tmpCB;union Cyc_Absyn_Cnst_union
_tmpCC;void*_tmpCD;int _tmpCE;void*_tmpCF;void*_tmpD0;union Cyc_Absyn_Cnst_union
_tmpD1;void*_tmpD2;char _tmpD3;void*_tmpD4;void*_tmpD5;union Cyc_Absyn_Cnst_union
_tmpD6;void*_tmpD7;short _tmpD8;void*_tmpD9;void*_tmpDA;union Cyc_Absyn_Cnst_union
_tmpDB;void*_tmpDC;int _tmpDD;_LL8B: _tmpBD=_tmpBC.f1;if(_tmpBD <= (void*)4)goto
_LL8D;if(*((int*)_tmpBD)!= 5)goto _LL8D;_tmpBE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpBD)->f1;_tmpBF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBD)->f2;_tmpC0=
_tmpBC.f2;if(((_tmpBC.f2).Char_c).tag != 0)goto _LL8D;_tmpC1=(_tmpC0.Char_c).f1;
_tmpC2=(_tmpC0.Char_c).f2;_LL8C: _tmpC4=_tmpBE;_tmpC5=_tmpBF;_tmpC7=_tmpC1;_tmpC8=(
short)_tmpC2;goto _LL8E;_LL8D: _tmpC3=_tmpBC.f1;if(_tmpC3 <= (void*)4)goto _LL8F;if(*((
int*)_tmpC3)!= 5)goto _LL8F;_tmpC4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpC3)->f1;_tmpC5=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpC3)->f2;_tmpC6=
_tmpBC.f2;if(((_tmpBC.f2).Short_c).tag != 1)goto _LL8F;_tmpC7=(_tmpC6.Short_c).f1;
_tmpC8=(_tmpC6.Short_c).f2;_LL8E: _tmpCA=_tmpC4;_tmpCB=_tmpC5;_tmpCD=_tmpC7;
_tmpCE=(int)_tmpC8;goto _LL90;_LL8F: _tmpC9=_tmpBC.f1;if(_tmpC9 <= (void*)4)goto
_LL91;if(*((int*)_tmpC9)!= 5)goto _LL91;_tmpCA=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpC9)->f1;_tmpCB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpC9)->f2;_tmpCC=
_tmpBC.f2;if(((_tmpBC.f2).Int_c).tag != 2)goto _LL91;_tmpCD=(_tmpCC.Int_c).f1;
_tmpCE=(_tmpCC.Int_c).f2;_LL90: if(_tmpCA != _tmpCD)ans.f1=(union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmpDE;(_tmpDE.Int_c).tag=2;(_tmpDE.Int_c).f1=(void*)
_tmpCA;(_tmpDE.Int_c).f2=_tmpCE;_tmpDE;});goto _LL8A;_LL91: _tmpCF=_tmpBC.f1;if(
_tmpCF <= (void*)4)goto _LL93;if(*((int*)_tmpCF)!= 18)goto _LL93;_tmpD0=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmpCF)->f1;_tmpD1=_tmpBC.f2;if(((_tmpBC.f2).Char_c).tag
!= 0)goto _LL93;_tmpD2=(_tmpD1.Char_c).f1;_tmpD3=(_tmpD1.Char_c).f2;_LL92: _tmpD5=
_tmpD0;_tmpD7=_tmpD2;_tmpD8=(short)_tmpD3;goto _LL94;_LL93: _tmpD4=_tmpBC.f1;if(
_tmpD4 <= (void*)4)goto _LL95;if(*((int*)_tmpD4)!= 18)goto _LL95;_tmpD5=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmpD4)->f1;_tmpD6=_tmpBC.f2;if(((_tmpBC.f2).Short_c).tag
!= 1)goto _LL95;_tmpD7=(_tmpD6.Short_c).f1;_tmpD8=(_tmpD6.Short_c).f2;_LL94:
_tmpDA=_tmpD5;_tmpDC=_tmpD7;_tmpDD=(int)_tmpD8;goto _LL96;_LL95: _tmpD9=_tmpBC.f1;
if(_tmpD9 <= (void*)4)goto _LL97;if(*((int*)_tmpD9)!= 18)goto _LL97;_tmpDA=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmpD9)->f1;_tmpDB=_tmpBC.f2;if(((_tmpBC.f2).Int_c).tag
!= 2)goto _LL97;_tmpDC=(_tmpDB.Int_c).f1;_tmpDD=(_tmpDB.Int_c).f2;_LL96: if(_tmpDD
< 0)({void*_tmpDF=0;Cyc_Tcutil_terr(e->loc,({const char*_tmpE0="cannot cast negative number to a tag type";
_tag_dyneither(_tmpE0,sizeof(char),42);}),_tag_dyneither(_tmpDF,sizeof(void*),0));});
Cyc_Tcutil_unify(_tmpDA,(void*)({struct Cyc_Absyn_ValueofType_struct*_tmpE1=
_cycalloc(sizeof(*_tmpE1));_tmpE1[0]=({struct Cyc_Absyn_ValueofType_struct _tmpE2;
_tmpE2.tag=17;_tmpE2.f1=Cyc_Absyn_const_exp(ans.f1,0);_tmpE2;});_tmpE1;}));ans.f1=(
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmpE3;(_tmpE3.Int_c).tag=2;(
_tmpE3.Int_c).f1=(void*)((void*)1);(_tmpE3.Int_c).f2=_tmpDD;_tmpE3;});goto _LL8A;
_LL97:;_LL98:({struct Cyc_String_pa_struct _tmpE6;_tmpE6.tag=0;_tmpE6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp97));{void*
_tmpE4[1]={& _tmpE6};Cyc_Tcutil_terr(e->loc,({const char*_tmpE5="cannot cast to %s";
_tag_dyneither(_tmpE5,sizeof(char),18);}),_tag_dyneither(_tmpE4,sizeof(void*),1));}});
goto _LL8A;_LL8A:;}goto _LL6A;_LL7F: if(*((int*)_tmp8B)!= 34)goto _LL81;_tmp99=((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp8B)->f3;_LL80: _tmp9A=_tmp99;goto _LL82;
_LL81: if(*((int*)_tmp8B)!= 33)goto _LL83;_tmp9A=((struct Cyc_Absyn_Enum_e_struct*)
_tmp8B)->f3;_LL82: ans=Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)_check_null(((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp9A))->tag));goto _LL6A;_LL83:;_LL84:({
struct Cyc_String_pa_struct _tmpE9;_tmpE9.tag=0;_tmpE9.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{void*_tmpE7[1]={& _tmpE9};Cyc_Tcutil_terr(
e->loc,({const char*_tmpE8="bad static expression %s";_tag_dyneither(_tmpE8,
sizeof(char),25);}),_tag_dyneither(_tmpE7,sizeof(void*),1));}});return({struct
_tuple5 _tmpEA;_tmpEA.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmpEB;(_tmpEB.Int_c).tag=2;(_tmpEB.Int_c).f1=(void*)((void*)0);(_tmpEB.Int_c).f2=
0;_tmpEB;});_tmpEA.f2=1;_tmpEA;});_LL6A:;}if(ans.f2)(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Const_e_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC[0]=({
struct Cyc_Absyn_Const_e_struct _tmpED;_tmpED.tag=0;_tmpED.f1=ans.f1;_tmpED;});
_tmpEC;})));return ans;}int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e){void*
_tmpEE=(void*)e->r;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Exp*_tmpF0;struct
Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_Absyn_Exp*_tmpF3;
struct Cyc_Absyn_Exp*_tmpF4;struct Cyc_Absyn_Exp*_tmpF5;void*_tmpF6;struct Cyc_List_List*
_tmpF7;struct Cyc_Absyn_Exp*_tmpF8;_LL9A: if(*((int*)_tmpEE)!= 34)goto _LL9C;_LL9B:
goto _LL9D;_LL9C: if(*((int*)_tmpEE)!= 33)goto _LL9E;_LL9D: goto _LL9F;_LL9E: if(*((
int*)_tmpEE)!= 18)goto _LLA0;_LL9F: goto _LLA1;_LLA0: if(*((int*)_tmpEE)!= 19)goto
_LLA2;_LLA1: goto _LLA3;_LLA2: if(*((int*)_tmpEE)!= 20)goto _LLA4;_LLA3: goto _LLA5;
_LLA4: if(*((int*)_tmpEE)!= 0)goto _LLA6;_LLA5: return 1;_LLA6: if(*((int*)_tmpEE)!= 
6)goto _LLA8;_tmpEF=((struct Cyc_Absyn_Conditional_e_struct*)_tmpEE)->f1;_tmpF0=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpEE)->f2;_tmpF1=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpEE)->f3;_LLA7: return(Cyc_Evexp_c_can_eval(_tmpEF) && Cyc_Evexp_c_can_eval(
_tmpF0)) && Cyc_Evexp_c_can_eval(_tmpF1);_LLA8: if(*((int*)_tmpEE)!= 7)goto _LLAA;
_tmpF2=((struct Cyc_Absyn_And_e_struct*)_tmpEE)->f1;_tmpF3=((struct Cyc_Absyn_And_e_struct*)
_tmpEE)->f2;_LLA9: _tmpF4=_tmpF2;_tmpF5=_tmpF3;goto _LLAB;_LLAA: if(*((int*)_tmpEE)
!= 8)goto _LLAC;_tmpF4=((struct Cyc_Absyn_Or_e_struct*)_tmpEE)->f1;_tmpF5=((struct
Cyc_Absyn_Or_e_struct*)_tmpEE)->f2;_LLAB: return Cyc_Evexp_c_can_eval(_tmpF4) && 
Cyc_Evexp_c_can_eval(_tmpF5);_LLAC: if(*((int*)_tmpEE)!= 3)goto _LLAE;_tmpF6=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpEE)->f1;_tmpF7=((struct Cyc_Absyn_Primop_e_struct*)
_tmpEE)->f2;_LLAD: for(0;_tmpF7 != 0;_tmpF7=_tmpF7->tl){if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_tmpF7->hd))return 0;}return 1;_LLAE: if(*((int*)_tmpEE)!= 39)
goto _LLB0;_LLAF: return 0;_LLB0: if(*((int*)_tmpEE)!= 15)goto _LLB2;_tmpF8=((struct
Cyc_Absyn_Cast_e_struct*)_tmpEE)->f2;_LLB1: return Cyc_Evexp_c_can_eval(_tmpF8);
_LLB2:;_LLB3: return 0;_LL99:;}static int Cyc_Evexp_const_exp_case_number(struct Cyc_Absyn_Exp*
e){void*_tmpF9=(void*)e->r;_LLB5: if(*((int*)_tmpF9)!= 0)goto _LLB7;_LLB6: return 1;
_LLB7: if(*((int*)_tmpF9)!= 6)goto _LLB9;_LLB8: return 2;_LLB9: if(*((int*)_tmpF9)!= 
3)goto _LLBB;_LLBA: return 3;_LLBB: if(*((int*)_tmpF9)!= 18)goto _LLBD;_LLBC: goto
_LLBE;_LLBD: if(*((int*)_tmpF9)!= 19)goto _LLBF;_LLBE: return 4;_LLBF: if(*((int*)
_tmpF9)!= 20)goto _LLC1;_LLC0: return 5;_LLC1: if(*((int*)_tmpF9)!= 15)goto _LLC3;
_LLC2: return 6;_LLC3: if(*((int*)_tmpF9)!= 7)goto _LLC5;_LLC4: return 7;_LLC5: if(*((
int*)_tmpF9)!= 8)goto _LLC7;_LLC6: return 8;_LLC7: if(*((int*)_tmpF9)!= 39)goto _LLC9;
_LLC8: return 9;_LLC9:;_LLCA:({struct Cyc_String_pa_struct _tmpFC;_tmpFC.tag=0;
_tmpFC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{
void*_tmpFA[1]={& _tmpFC};Cyc_Tcutil_terr(e->loc,({const char*_tmpFB="bad static expression %s";
_tag_dyneither(_tmpFB,sizeof(char),25);}),_tag_dyneither(_tmpFA,sizeof(void*),1));}});
return 0;_LLB4:;}int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){unsigned int _tmpFE;int _tmpFF;struct _tuple4 _tmpFD=Cyc_Evexp_eval_const_uint_exp(
e1);_tmpFE=_tmpFD.f1;_tmpFF=_tmpFD.f2;{unsigned int _tmp101;int _tmp102;struct
_tuple4 _tmp100=Cyc_Evexp_eval_const_uint_exp(e2);_tmp101=_tmp100.f1;_tmp102=
_tmp100.f2;if(_tmpFF  && _tmp102)return(int)(_tmpFE - _tmp101);{int _tmp103=Cyc_Evexp_const_exp_case_number(
e1);int _tmp104=Cyc_Evexp_const_exp_case_number(e2);if(_tmp103 != _tmp104)return
_tmp103 - _tmp104;{struct _tuple3 _tmp106=({struct _tuple3 _tmp105;_tmp105.f1=(void*)
e1->r;_tmp105.f2=(void*)e2->r;_tmp105;});void*_tmp107;struct Cyc_Absyn_Exp*
_tmp108;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Exp*_tmp10A;void*_tmp10B;
struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*
_tmp10E;void*_tmp10F;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp111;
void*_tmp112;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp114;void*
_tmp115;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp117;void*_tmp118;
struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;void*_tmp11B;void*
_tmp11C;struct Cyc_List_List*_tmp11D;void*_tmp11E;void*_tmp11F;struct Cyc_List_List*
_tmp120;void*_tmp121;void*_tmp122;void*_tmp123;void*_tmp124;void*_tmp125;void*
_tmp126;void*_tmp127;struct Cyc_Absyn_Exp*_tmp128;void*_tmp129;struct Cyc_Absyn_Exp*
_tmp12A;void*_tmp12B;void*_tmp12C;void*_tmp12D;struct Cyc_Absyn_Exp*_tmp12E;void*
_tmp12F;struct Cyc_Absyn_Exp*_tmp130;void*_tmp131;void*_tmp132;void*_tmp133;void*
_tmp134;void*_tmp135;void*_tmp136;void*_tmp137;void*_tmp138;struct Cyc_Absyn_Exp*
_tmp139;void*_tmp13A;void*_tmp13B;struct Cyc_Absyn_Exp*_tmp13C;void*_tmp13D;void*
_tmp13E;void*_tmp13F;void*_tmp140;_LLCC: _tmp107=_tmp106.f1;if(*((int*)_tmp107)!= 
6)goto _LLCE;_tmp108=((struct Cyc_Absyn_Conditional_e_struct*)_tmp107)->f1;_tmp109=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp107)->f2;_tmp10A=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp107)->f3;_tmp10B=_tmp106.f2;if(*((int*)_tmp10B)!= 6)goto _LLCE;_tmp10C=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp10B)->f1;_tmp10D=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp10B)->f2;_tmp10E=((struct Cyc_Absyn_Conditional_e_struct*)_tmp10B)->f3;_LLCD: {
int _tmp141=Cyc_Evexp_const_exp_cmp(_tmp10A,_tmp10E);if(_tmp141 != 0)return _tmp141;
_tmp110=_tmp108;_tmp111=_tmp109;_tmp113=_tmp10C;_tmp114=_tmp10D;goto _LLCF;}_LLCE:
_tmp10F=_tmp106.f1;if(*((int*)_tmp10F)!= 7)goto _LLD0;_tmp110=((struct Cyc_Absyn_And_e_struct*)
_tmp10F)->f1;_tmp111=((struct Cyc_Absyn_And_e_struct*)_tmp10F)->f2;_tmp112=
_tmp106.f2;if(*((int*)_tmp112)!= 7)goto _LLD0;_tmp113=((struct Cyc_Absyn_And_e_struct*)
_tmp112)->f1;_tmp114=((struct Cyc_Absyn_And_e_struct*)_tmp112)->f2;_LLCF: _tmp116=
_tmp110;_tmp117=_tmp111;_tmp119=_tmp113;_tmp11A=_tmp114;goto _LLD1;_LLD0: _tmp115=
_tmp106.f1;if(*((int*)_tmp115)!= 8)goto _LLD2;_tmp116=((struct Cyc_Absyn_Or_e_struct*)
_tmp115)->f1;_tmp117=((struct Cyc_Absyn_Or_e_struct*)_tmp115)->f2;_tmp118=_tmp106.f2;
if(*((int*)_tmp118)!= 8)goto _LLD2;_tmp119=((struct Cyc_Absyn_Or_e_struct*)_tmp118)->f1;
_tmp11A=((struct Cyc_Absyn_Or_e_struct*)_tmp118)->f2;_LLD1: {int _tmp142=Cyc_Evexp_const_exp_cmp(
_tmp116,_tmp119);if(_tmp142 != 0)return _tmp142;return Cyc_Evexp_const_exp_cmp(
_tmp117,_tmp11A);}_LLD2: _tmp11B=_tmp106.f1;if(*((int*)_tmp11B)!= 3)goto _LLD4;
_tmp11C=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp11B)->f1;_tmp11D=((struct
Cyc_Absyn_Primop_e_struct*)_tmp11B)->f2;_tmp11E=_tmp106.f2;if(*((int*)_tmp11E)!= 
3)goto _LLD4;_tmp11F=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp11E)->f1;
_tmp120=((struct Cyc_Absyn_Primop_e_struct*)_tmp11E)->f2;_LLD3: {int _tmp143=(int)
_tmp11C - (int)_tmp11F;if(_tmp143 != 0)return _tmp143;for(0;_tmp11D != 0  && _tmp120
!= 0;(_tmp11D=_tmp11D->tl,_tmp120=_tmp120->tl)){int _tmp144=Cyc_Evexp_const_exp_cmp((
struct Cyc_Absyn_Exp*)_tmp11D->hd,(struct Cyc_Absyn_Exp*)_tmp120->hd);if(_tmp144 != 
0)return _tmp144;}return 0;}_LLD4: _tmp121=_tmp106.f1;if(*((int*)_tmp121)!= 18)goto
_LLD6;_tmp122=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp121)->f1;_tmp123=
_tmp106.f2;if(*((int*)_tmp123)!= 18)goto _LLD6;_tmp124=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp123)->f1;_LLD5: return Cyc_Tcutil_typecmp(_tmp122,_tmp124);_LLD6: _tmp125=
_tmp106.f1;if(*((int*)_tmp125)!= 18)goto _LLD8;_tmp126=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp125)->f1;_tmp127=_tmp106.f2;if(*((int*)_tmp127)!= 19)goto _LLD8;_tmp128=((
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp127)->f1;_LLD7: if(_tmp128->topt == 0)({
void*_tmp145=0;Cyc_Tcutil_terr(e2->loc,({const char*_tmp146="cannot handle sizeof(exp) here -- use sizeof(type)";
_tag_dyneither(_tmp146,sizeof(char),51);}),_tag_dyneither(_tmp145,sizeof(void*),
0));});return Cyc_Tcutil_typecmp(_tmp126,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp128->topt))->v);_LLD8: _tmp129=_tmp106.f1;if(*((int*)_tmp129)!= 19)goto _LLDA;
_tmp12A=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp129)->f1;_tmp12B=_tmp106.f2;
if(*((int*)_tmp12B)!= 18)goto _LLDA;_tmp12C=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp12B)->f1;_LLD9: if(_tmp12A->topt == 0)({void*_tmp147=0;Cyc_Tcutil_terr(e1->loc,({
const char*_tmp148="cannot handle sizeof(exp) here -- use sizeof(type)";
_tag_dyneither(_tmp148,sizeof(char),51);}),_tag_dyneither(_tmp147,sizeof(void*),
0));});return Cyc_Tcutil_typecmp((void*)((struct Cyc_Core_Opt*)_check_null(_tmp12A->topt))->v,
_tmp12C);_LLDA: _tmp12D=_tmp106.f1;if(*((int*)_tmp12D)!= 19)goto _LLDC;_tmp12E=((
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp12D)->f1;_tmp12F=_tmp106.f2;if(*((int*)
_tmp12F)!= 19)goto _LLDC;_tmp130=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp12F)->f1;
_LLDB: if(_tmp12E->topt == 0)({void*_tmp149=0;Cyc_Tcutil_terr(e1->loc,({const char*
_tmp14A="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_dyneither(
_tmp14A,sizeof(char),51);}),_tag_dyneither(_tmp149,sizeof(void*),0));});if(
_tmp130->topt == 0)({void*_tmp14B=0;Cyc_Tcutil_terr(e2->loc,({const char*_tmp14C="cannot handle sizeof(exp) here -- use sizeof(type)";
_tag_dyneither(_tmp14C,sizeof(char),51);}),_tag_dyneither(_tmp14B,sizeof(void*),
0));});return Cyc_Tcutil_typecmp((void*)((struct Cyc_Core_Opt*)_check_null(_tmp12E->topt))->v,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp130->topt))->v);_LLDC: _tmp131=
_tmp106.f1;if(*((int*)_tmp131)!= 20)goto _LLDE;_tmp132=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp131)->f1;_tmp133=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp131)->f2;
_tmp134=_tmp106.f2;if(*((int*)_tmp134)!= 20)goto _LLDE;_tmp135=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp134)->f1;_tmp136=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp134)->f2;
_LLDD: {int _tmp14D=Cyc_Tcutil_typecmp(_tmp132,_tmp135);if(_tmp14D != 0)return
_tmp14D;{struct _tuple3 _tmp14F=({struct _tuple3 _tmp14E;_tmp14E.f1=_tmp133;_tmp14E.f2=
_tmp136;_tmp14E;});void*_tmp150;void*_tmp151;void*_tmp152;void*_tmp153;void*
_tmp154;struct _dyneither_ptr*_tmp155;void*_tmp156;struct _dyneither_ptr*_tmp157;
void*_tmp158;unsigned int _tmp159;void*_tmp15A;unsigned int _tmp15B;_LLE5: _tmp150=
_tmp14F.f1;if(*((int*)_tmp150)!= 0)goto _LLE7;_tmp151=_tmp14F.f2;if(*((int*)
_tmp151)!= 1)goto _LLE7;_LLE6: return - 1;_LLE7: _tmp152=_tmp14F.f1;if(*((int*)
_tmp152)!= 1)goto _LLE9;_tmp153=_tmp14F.f2;if(*((int*)_tmp153)!= 0)goto _LLE9;
_LLE8: return 1;_LLE9: _tmp154=_tmp14F.f1;if(*((int*)_tmp154)!= 0)goto _LLEB;_tmp155=((
struct Cyc_Absyn_StructField_struct*)_tmp154)->f1;_tmp156=_tmp14F.f2;if(*((int*)
_tmp156)!= 0)goto _LLEB;_tmp157=((struct Cyc_Absyn_StructField_struct*)_tmp156)->f1;
_LLEA: return Cyc_strptrcmp(_tmp155,_tmp157);_LLEB: _tmp158=_tmp14F.f1;if(*((int*)
_tmp158)!= 1)goto _LLE4;_tmp159=((struct Cyc_Absyn_TupleIndex_struct*)_tmp158)->f1;
_tmp15A=_tmp14F.f2;if(*((int*)_tmp15A)!= 1)goto _LLE4;_tmp15B=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp15A)->f1;_LLEC: return(int)(_tmp159 - _tmp15B);_LLE4:;}}_LLDE: _tmp137=_tmp106.f1;
if(*((int*)_tmp137)!= 15)goto _LLE0;_tmp138=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp137)->f1;_tmp139=((struct Cyc_Absyn_Cast_e_struct*)_tmp137)->f2;_tmp13A=
_tmp106.f2;if(*((int*)_tmp13A)!= 15)goto _LLE0;_tmp13B=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp13A)->f1;_tmp13C=((struct Cyc_Absyn_Cast_e_struct*)_tmp13A)->f2;_LLDF: {int
_tmp15C=Cyc_Tcutil_typecmp(_tmp138,_tmp13B);if(_tmp15C != 0)return _tmp15C;return
Cyc_Evexp_const_exp_cmp(_tmp139,_tmp13C);}_LLE0: _tmp13D=_tmp106.f1;if(*((int*)
_tmp13D)!= 39)goto _LLE2;_tmp13E=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp13D)->f1;_tmp13F=_tmp106.f2;if(*((int*)_tmp13F)!= 39)goto _LLE2;_tmp140=(void*)((
struct Cyc_Absyn_Valueof_e_struct*)_tmp13F)->f1;_LLE1: if(Cyc_Tcutil_unify(_tmp13E,
_tmp140))return 0;return Cyc_Tcutil_typecmp(_tmp13E,_tmp140);_LLE2:;_LLE3:({void*
_tmp15D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp15E="Evexp::const_exp_cmp, unexpected case";_tag_dyneither(_tmp15E,
sizeof(char),38);}),_tag_dyneither(_tmp15D,sizeof(void*),0));});_LLCB:;}}}}int
Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){return
Cyc_Evexp_const_exp_cmp(e1,e2)== 0;}int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){unsigned int _tmp160;int _tmp161;struct _tuple4 _tmp15F=
Cyc_Evexp_eval_const_uint_exp(e1);_tmp160=_tmp15F.f1;_tmp161=_tmp15F.f2;{
unsigned int _tmp163;int _tmp164;struct _tuple4 _tmp162=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp163=_tmp162.f1;_tmp164=_tmp162.f2;if(_tmp161  && _tmp164)return _tmp160 <= 
_tmp163;return Cyc_Evexp_same_const_exp(e1,e2);}}struct _tuple9{struct Cyc_Absyn_Tqual
f1;void*f2;};int Cyc_Evexp_okay_szofarg(void*t){void*_tmp165=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp166;struct Cyc_Core_Opt*_tmp167;struct Cyc_Core_Opt
_tmp168;void*_tmp169;struct Cyc_Absyn_DatatypeFieldInfo _tmp16A;union Cyc_Absyn_DatatypeFieldInfoU_union
_tmp16B;struct Cyc_Absyn_Datatypefield*_tmp16C;struct Cyc_List_List*_tmp16D;struct
Cyc_Absyn_ArrayInfo _tmp16E;void*_tmp16F;struct Cyc_Absyn_Exp*_tmp170;struct Cyc_Absyn_AggrInfo
_tmp171;union Cyc_Absyn_AggrInfoU_union _tmp172;struct Cyc_List_List*_tmp173;struct
Cyc_Absyn_Typedefdecl*_tmp174;_LLEE: if((int)_tmp165 != 0)goto _LLF0;_LLEF: return 0;
_LLF0: if(_tmp165 <= (void*)4)goto _LL104;if(*((int*)_tmp165)!= 1)goto _LLF2;_tmp166=((
struct Cyc_Absyn_VarType_struct*)_tmp165)->f1;_LLF1: {void*_tmp175=Cyc_Tcutil_tvar_kind(
_tmp166);_LL127: if((int)_tmp175 != 2)goto _LL129;_LL128: return 1;_LL129:;_LL12A:
return 0;_LL126:;}_LLF2: if(*((int*)_tmp165)!= 0)goto _LLF4;_tmp167=((struct Cyc_Absyn_Evar_struct*)
_tmp165)->f1;if(_tmp167 == 0)goto _LLF4;_tmp168=*_tmp167;_tmp169=(void*)_tmp168.v;
if((int)_tmp169 != 2)goto _LLF4;_LLF3: return 1;_LLF4: if(*((int*)_tmp165)!= 0)goto
_LLF6;_LLF5: return 0;_LLF6: if(*((int*)_tmp165)!= 2)goto _LLF8;_LLF7: return 1;_LLF8:
if(*((int*)_tmp165)!= 3)goto _LLFA;_tmp16A=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp165)->f1;_tmp16B=_tmp16A.field_info;if((((((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp165)->f1).field_info).KnownDatatypefield).tag != 1)goto _LLFA;_tmp16C=(_tmp16B.KnownDatatypefield).f2;
_LLF9: _tmp16D=_tmp16C->typs;goto _LLFB;_LLFA: if(*((int*)_tmp165)!= 9)goto _LLFC;
_tmp16D=((struct Cyc_Absyn_TupleType_struct*)_tmp165)->f1;_LLFB: for(0;_tmp16D != 0;
_tmp16D=_tmp16D->tl){if(!Cyc_Evexp_okay_szofarg((*((struct _tuple9*)_tmp16D->hd)).f2))
return 0;}return 1;_LLFC: if(*((int*)_tmp165)!= 3)goto _LLFE;_LLFD: return 0;_LLFE: if(*((
int*)_tmp165)!= 4)goto _LL100;_LLFF: return 1;_LL100: if(*((int*)_tmp165)!= 18)goto
_LL102;_LL101: goto _LL103;_LL102: if(*((int*)_tmp165)!= 5)goto _LL104;_LL103: goto
_LL105;_LL104: if((int)_tmp165 != 1)goto _LL106;_LL105: goto _LL107;_LL106: if(_tmp165
<= (void*)4)goto _LL118;if(*((int*)_tmp165)!= 6)goto _LL108;_LL107: return 1;_LL108:
if(*((int*)_tmp165)!= 7)goto _LL10A;_tmp16E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp165)->f1;_tmp16F=(void*)_tmp16E.elt_type;_tmp170=_tmp16E.num_elts;_LL109:
return _tmp170 != 0;_LL10A: if(*((int*)_tmp165)!= 8)goto _LL10C;_LL10B: return 0;
_LL10C: if(*((int*)_tmp165)!= 14)goto _LL10E;_LL10D: return 1;_LL10E: if(*((int*)
_tmp165)!= 15)goto _LL110;_LL10F: return 1;_LL110: if(*((int*)_tmp165)!= 10)goto
_LL112;_tmp171=((struct Cyc_Absyn_AggrType_struct*)_tmp165)->f1;_tmp172=_tmp171.aggr_info;
_LL111: {struct Cyc_Absyn_Aggrdecl*ad;{union Cyc_Absyn_AggrInfoU_union _tmp176=
_tmp172;struct Cyc_Absyn_Aggrdecl**_tmp177;struct Cyc_Absyn_Aggrdecl*_tmp178;
_LL12C: if((_tmp176.UnknownAggr).tag != 0)goto _LL12E;_LL12D:({void*_tmp179=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp17A="szof on unchecked StructType";_tag_dyneither(_tmp17A,sizeof(char),29);}),
_tag_dyneither(_tmp179,sizeof(void*),0));});_LL12E: if((_tmp176.KnownAggr).tag != 
1)goto _LL12B;_tmp177=(_tmp176.KnownAggr).f1;_tmp178=*_tmp177;_LL12F: ad=_tmp178;
_LL12B:;}if(ad->impl == 0)return 0;_tmp173=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;goto _LL113;}_LL112: if(*((int*)_tmp165)!= 11)goto
_LL114;_tmp173=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp165)->f2;_LL113: for(0;
_tmp173 != 0;_tmp173=_tmp173->tl){if(!Cyc_Evexp_okay_szofarg((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp173->hd)->type))return 0;}return 1;_LL114: if(*((int*)_tmp165)!= 12)goto _LL116;
_LL115: goto _LL117;_LL116: if(*((int*)_tmp165)!= 13)goto _LL118;_LL117: return 1;
_LL118: if((int)_tmp165 != 2)goto _LL11A;_LL119: goto _LL11B;_LL11A: if((int)_tmp165 != 
3)goto _LL11C;_LL11B: goto _LL11D;_LL11C: if(_tmp165 <= (void*)4)goto _LL11E;if(*((int*)
_tmp165)!= 20)goto _LL11E;_LL11D: goto _LL11F;_LL11E: if(_tmp165 <= (void*)4)goto
_LL120;if(*((int*)_tmp165)!= 19)goto _LL120;_LL11F: goto _LL121;_LL120: if(_tmp165 <= (
void*)4)goto _LL122;if(*((int*)_tmp165)!= 17)goto _LL122;_LL121: goto _LL123;_LL122:
if(_tmp165 <= (void*)4)goto _LL124;if(*((int*)_tmp165)!= 21)goto _LL124;_LL123:
return 0;_LL124: if(_tmp165 <= (void*)4)goto _LLED;if(*((int*)_tmp165)!= 16)goto
_LLED;_tmp174=((struct Cyc_Absyn_TypedefType_struct*)_tmp165)->f3;_LL125: if(
_tmp174 == 0  || _tmp174->kind == 0)({void*_tmp17B=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp17E;_tmp17E.tag=0;_tmp17E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp17C[1]={& _tmp17E};Cyc_aprintf(({const char*
_tmp17D="szof typedeftype %s";_tag_dyneither(_tmp17D,sizeof(char),20);}),
_tag_dyneither(_tmp17C,sizeof(void*),1));}}),_tag_dyneither(_tmp17B,sizeof(void*),
0));});return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp174->kind))->v == (void*)
2;_LLED:;}
