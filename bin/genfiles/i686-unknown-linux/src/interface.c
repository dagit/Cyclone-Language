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
void*v;};void*Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];typedef struct{
int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
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
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(struct
Cyc_List_List*x);extern char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];struct
_union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple0{union
Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
loc;};struct _union_Constraint_Eq_constr{int tag;void*val;};struct
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,
struct Cyc_List_List*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;extern struct _tuple0*
Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init);void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*
eff_typ,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(
void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict
d,void*accum);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict
d);struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d);struct
_tuple9{void*f1;void*f2;};struct _tuple9*Cyc_Dict_rchoose(struct _RegionHandle*r,
struct Cyc_Dict_Dict d);struct _tuple9*Cyc_Dict_rchoose(struct _RegionHandle*,struct
Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
le;int allow_valueof;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
struct Cyc_Interface_I;struct Cyc_Interface_I*Cyc_Interface_empty();struct Cyc_Interface_I*
Cyc_Interface_final();struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict
ae);struct _tuple10{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info);struct Cyc_Interface_I*
Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct
_tuple10*info);struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*
li,struct Cyc_List_List*linfo);struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(
struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*
linfo);void Cyc_Interface_print(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);void Cyc_Interface_save(
struct Cyc_Interface_I*,struct Cyc___cycFILE*);struct Cyc_Interface_I*Cyc_Interface_load(
struct Cyc___cycFILE*);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct
_dyneither_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_all_effects: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*);void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int
var_default_init,struct Cyc_List_List*ds);extern char Cyc_Lexing_Error[10];struct
Cyc_Lexing_Error_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{
void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct
_dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int
lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct
_union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;
struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple11{
struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*
f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple11*val;};struct
_union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;void*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*val;};struct
_union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY11{
int tag;struct Cyc_Absyn_Pat*val;};struct _tuple12{struct Cyc_List_List*f1;int f2;};
struct _union_YYSTYPE_YY12{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY13{
int tag;struct Cyc_List_List*val;};struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple13*val;};struct
_union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY16{
int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Absyn_Fndecl*
val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec*val;};struct _tuple14{
struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY20{int
tag;struct _tuple14*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY22{int tag;void*val;};struct _union_YYSTYPE_YY23{int
tag;void*val;};struct _union_YYSTYPE_YY24{int tag;void*val;};struct
_union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY26{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY29{int tag;struct Cyc_Declarator*val;};struct _union_YYSTYPE_YY30{
int tag;struct Cyc_Abstractdeclarator*val;};struct _union_YYSTYPE_YY31{int tag;int
val;};struct _union_YYSTYPE_YY32{int tag;void*val;};struct _union_YYSTYPE_YY33{int
tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct
Cyc_List_List*val;};struct _tuple15{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple15*val;
};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY37{int tag;struct _tuple7*val;};struct _union_YYSTYPE_YY38{int tag;
struct Cyc_List_List*val;};struct _tuple16{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*
f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int
tag;struct _tuple16*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;void*val;
};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct
_union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct
_union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{
int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};
struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct
_union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct
_union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok
Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_YY1
YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct
_union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;
struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9
YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct
_union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14
YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct
_union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19
YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct
_union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24
YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct
_union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29
YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct
_union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34
YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct
_union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39
YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct
_union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44
YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct
_union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49
YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct
_union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54
YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int
timestamp;int first_line;int first_column;int last_line;int last_column;};void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*
Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_function_type(void*t);void*Cyc_Tcutil_kind_to_bound(
void*k);extern char Cyc_Tcdecl_Incompatible[17];struct Cyc_Tcdecl_Xdatatypefielddecl{
struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};void Cyc_Tcdecl_merr(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(
struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(
struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(
struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);void Cyc_Lex_lex_init();
struct Cyc_Interface_Ienv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict
datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefdecls;
struct Cyc_Dict_Dict vardecls;struct Cyc_Dict_Dict xdatatypefielddecls;};struct Cyc_Interface_I{
struct Cyc_Dict_Dict imports;struct Cyc_Dict_Dict exports;};static struct Cyc_Interface_Ienv*
Cyc_Interface_new_ienv(){return({struct Cyc_Interface_Ienv*_tmp0=_cycalloc(
sizeof(*_tmp0));_tmp0->aggrdecls=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp0->datatypedecls=((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Dict_empty)(Cyc_strptrcmp);_tmp0->enumdecls=((struct Cyc_Dict_Dict(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);
_tmp0->typedefdecls=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp0->vardecls=((struct
Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(
Cyc_strptrcmp);_tmp0->xdatatypefielddecls=((struct Cyc_Dict_Dict(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);
_tmp0;});}static struct Cyc_Interface_Ienv*Cyc_Interface_lazy_new_ienv(){static
struct Cyc_Interface_Ienv*ie=0;if(ie == 0)ie=(struct Cyc_Interface_Ienv*)Cyc_Interface_new_ienv();
return(struct Cyc_Interface_Ienv*)ie;}static struct Cyc_Interface_Ienv*Cyc_Interface_skel2i_f(
void*ignored){return Cyc_Interface_new_ienv();}static struct Cyc_Interface_I*Cyc_Interface_skel2i(
struct Cyc_Dict_Dict skel){return({struct Cyc_Interface_I*_tmp1=_cycalloc(sizeof(*
_tmp1));_tmp1->imports=((struct Cyc_Dict_Dict(*)(struct Cyc_Interface_Ienv*(*f)(
void*),struct Cyc_Dict_Dict d))Cyc_Dict_map)(Cyc_Interface_skel2i_f,skel);_tmp1->exports=((
struct Cyc_Dict_Dict(*)(struct Cyc_Interface_Ienv*(*f)(void*),struct Cyc_Dict_Dict d))
Cyc_Dict_map)(Cyc_Interface_skel2i_f,skel);_tmp1;});}struct Cyc_Interface_I*Cyc_Interface_empty(){
return({struct Cyc_Interface_I*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->imports=((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(
Cyc_Absyn_varlist_cmp);_tmp2->exports=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct
Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp);
_tmp2;});}struct Cyc_Interface_I*Cyc_Interface_final(){struct Cyc_Interface_I*
_tmp3=Cyc_Interface_empty();struct Cyc_Interface_Ienv*_tmp4=Cyc_Interface_new_ienv();
struct Cyc_Absyn_Datatypedecl*_tmp5=({struct Cyc_Absyn_Datatypedecl*_tmp15=
_cycalloc(sizeof(*_tmp15));_tmp15[0]=*Cyc_Absyn_exn_tud;_tmp15;});_tmp5->sc=(
void*)2;_tmp4->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmp4->datatypedecls,(*
Cyc_Absyn_exn_name).f2,_tmp5);{struct Cyc_List_List*_tmp6=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp6 != 0;
_tmp6=_tmp6->tl){struct Cyc_Absyn_Datatypefield*_tmp7=({struct Cyc_Absyn_Datatypefield*
_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA[0]=*((struct Cyc_Absyn_Datatypefield*)_tmp6->hd);
_tmpA;});_tmp7->sc=(void*)2;{struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp8=({struct
Cyc_Tcdecl_Xdatatypefielddecl*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->base=_tmp5;
_tmp9->field=_tmp7;_tmp9;});_tmp4->xdatatypefielddecls=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Tcdecl_Xdatatypefielddecl*
v))Cyc_Dict_insert)(_tmp4->xdatatypefielddecls,(*((struct Cyc_Absyn_Datatypefield*)
_tmp6->hd)->name).f2,_tmp8);}}}_tmp3->exports=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(_tmp3->exports,
0,_tmp4);{struct Cyc_Interface_Ienv*_tmpB=Cyc_Interface_new_ienv();struct _tuple0*
qmain=({struct _tuple0*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->f1=({union Cyc_Absyn_Nmspace
_tmp14;(_tmp14.Abs_n).val=0;(_tmp14.Abs_n).tag=2;_tmp14;});_tmp11->f2=({struct
_dyneither_ptr*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12[0]=(struct _dyneither_ptr)({
const char*_tmp13="main";_tag_dyneither(_tmp13,sizeof(char),5);});_tmp12;});
_tmp11;});struct Cyc_Absyn_Vardecl*_tmpC=Cyc_Absyn_new_vardecl(qmain,Cyc_Absyn_function_typ(
0,0,Cyc_Absyn_sint_typ,({struct Cyc_List_List*_tmpD=_cycalloc(sizeof(*_tmpD));
_tmpD->hd=({struct _tuple7*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->f1=0;_tmp10->f2=
Cyc_Absyn_empty_tqual(0);_tmp10->f3=Cyc_Absyn_uint_typ;_tmp10;});_tmpD->tl=({
struct Cyc_List_List*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->hd=({struct _tuple7*
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF->f1=0;_tmpF->f2=Cyc_Absyn_empty_tqual(0);
_tmpF->f3=Cyc_Absyn_dyneither_typ(Cyc_Absyn_dyneither_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);_tmpF;});_tmpE->tl=0;_tmpE;});_tmpD;}),0,0,0,0),0);
_tmpC->sc=(void*)3;_tmpB->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpB->vardecls,(*
qmain).f2,_tmpC);_tmp3->imports=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(_tmp3->exports,
0,_tmpB);return _tmp3;}}static void Cyc_Interface_err(struct _dyneither_ptr msg){({
void*_tmp16=0;Cyc_Tcutil_terr(0,msg,_tag_dyneither(_tmp16,sizeof(void*),0));});}
static void*Cyc_Interface_invalid_arg(struct _dyneither_ptr s){(int)_throw((void*)({
struct Cyc_Core_Invalid_argument_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17[
0]=({struct Cyc_Core_Invalid_argument_struct _tmp18;_tmp18.tag=Cyc_Core_Invalid_argument;
_tmp18.f1=s;_tmp18;});_tmp17;}));}static void Cyc_Interface_fields_err(struct
_dyneither_ptr sc,struct _dyneither_ptr t,struct _tuple0*n){Cyc_Interface_err((
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp1D;_tmp1D.tag=0;_tmp1D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(n));{struct
Cyc_String_pa_struct _tmp1C;_tmp1C.tag=0;_tmp1C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)t);{struct Cyc_String_pa_struct _tmp1B;_tmp1B.tag=0;_tmp1B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)sc);{void*_tmp19[3]={& _tmp1B,& _tmp1C,&
_tmp1D};Cyc_aprintf(({const char*_tmp1A="fields of %s %s %s have never been defined";
_tag_dyneither(_tmp1A,sizeof(char),43);}),_tag_dyneither(_tmp19,sizeof(void*),3));}}}}));}
static void Cyc_Interface_body_err(struct _dyneither_ptr sc,struct _tuple0*n){Cyc_Interface_err((
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp21;_tmp21.tag=0;_tmp21.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(n));{struct
Cyc_String_pa_struct _tmp20;_tmp20.tag=0;_tmp20.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)sc);{void*_tmp1E[2]={& _tmp20,& _tmp21};Cyc_aprintf(({const char*
_tmp1F="the body of %s function %s has never been defined";_tag_dyneither(_tmp1F,
sizeof(char),50);}),_tag_dyneither(_tmp1E,sizeof(void*),2));}}}));}static void Cyc_Interface_static_err(
struct _dyneither_ptr obj1,struct _tuple0*name1,struct _dyneither_ptr obj2,struct
_tuple0*name2){if(obj1.curr != (_tag_dyneither(0,0,0)).curr)Cyc_Interface_err((
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp27;_tmp27.tag=0;_tmp27.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(name2));{
struct Cyc_String_pa_struct _tmp26;_tmp26.tag=0;_tmp26.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)obj2);{struct Cyc_String_pa_struct _tmp25;_tmp25.tag=0;_tmp25.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(name1));{
struct Cyc_String_pa_struct _tmp24;_tmp24.tag=0;_tmp24.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)obj1);{void*_tmp22[4]={& _tmp24,& _tmp25,& _tmp26,& _tmp27};Cyc_aprintf(({
const char*_tmp23="declaration of %s %s relies on static %s %s";_tag_dyneither(
_tmp23,sizeof(char),44);}),_tag_dyneither(_tmp22,sizeof(void*),4));}}}}}));}
static void Cyc_Interface_abstract_err(struct _dyneither_ptr obj1,struct _tuple0*
name1,struct _dyneither_ptr obj2,struct _tuple0*name2){if(obj1.curr != (
_tag_dyneither(0,0,0)).curr)Cyc_Interface_err((struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
name2));{struct Cyc_String_pa_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)obj2);{struct Cyc_String_pa_struct _tmp2B;
_tmp2B.tag=0;_tmp2B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
name1));{struct Cyc_String_pa_struct _tmp2A;_tmp2A.tag=0;_tmp2A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)obj1);{void*_tmp28[4]={& _tmp2A,& _tmp2B,&
_tmp2C,& _tmp2D};Cyc_aprintf(({const char*_tmp29="declaration of %s %s relies on fields of abstract %s %s";
_tag_dyneither(_tmp29,sizeof(char),56);}),_tag_dyneither(_tmp28,sizeof(void*),4));}}}}}));}
struct Cyc_Interface_Seen{struct Cyc_Dict_Dict aggrs;struct Cyc_Dict_Dict datatypes;}
;static struct Cyc_Interface_Seen*Cyc_Interface_new_seen(){return({struct Cyc_Interface_Seen*
_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->aggrs=((struct Cyc_Dict_Dict(*)(int(*
cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp2E->datatypes=((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp);_tmp2E;});}static int Cyc_Interface_check_public_type(struct
Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct
_tuple0*name,void*t);static int Cyc_Interface_check_public_type_list(struct Cyc_Dict_Dict
ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*
f)(void*),struct Cyc_List_List*l){int _tmp2F=1;for(0;l != 0;l=l->tl){if(!Cyc_Interface_check_public_type(
ae,seen,obj,name,f((void*)l->hd)))_tmp2F=0;}return _tmp2F;}static int Cyc_Interface_check_public_aggrdecl(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Aggrdecl*d){{
struct _handler_cons _tmp30;_push_handler(& _tmp30);{int _tmp32=0;if(setjmp(_tmp30.handler))
_tmp32=1;if(!_tmp32){{int _tmp33=((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))
Cyc_Dict_lookup)(seen->aggrs,d->name);_npop_handler(0);return _tmp33;};
_pop_handler();}else{void*_tmp31=(void*)_exn_thrown;void*_tmp35=_tmp31;_LL1: if(
_tmp35 != Cyc_Dict_Absent)goto _LL3;_LL2: goto _LL0;_LL3:;_LL4:(void)_throw(_tmp35);
_LL0:;}}}{int _tmp36=1;seen->aggrs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp36);if(d->impl != 
0){struct Cyc_List_List*_tmp37=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(d->impl))->fields;
for(0;_tmp37 != 0;_tmp37=_tmp37->tl){if(!Cyc_Interface_check_public_type(ae,seen,({
const char*_tmp38="type";_tag_dyneither(_tmp38,sizeof(char),5);}),d->name,((
struct Cyc_Absyn_Aggrfield*)_tmp37->hd)->type))_tmp36=0;}}seen->aggrs=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,
_tmp36);return _tmp36;}}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};static
void*Cyc_Interface_get_type1(struct _tuple17*x){return(*x).f2;}static void*Cyc_Interface_get_type2(
struct _tuple7*x){return(*x).f3;}static int Cyc_Interface_check_public_datatypedecl(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Datatypedecl*
d){{struct _handler_cons _tmp39;_push_handler(& _tmp39);{int _tmp3B=0;if(setjmp(
_tmp39.handler))_tmp3B=1;if(!_tmp3B){{int _tmp3C=((int(*)(struct Cyc_Dict_Dict d,
struct _tuple0*k))Cyc_Dict_lookup)(seen->datatypes,d->name);_npop_handler(0);
return _tmp3C;};_pop_handler();}else{void*_tmp3A=(void*)_exn_thrown;void*_tmp3E=
_tmp3A;_LL6: if(_tmp3E != Cyc_Dict_Absent)goto _LL8;_LL7: goto _LL5;_LL8:;_LL9:(void)
_throw(_tmp3E);_LL5:;}}}{int _tmp3F=1;seen->datatypes=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->datatypes,d->name,
_tmp3F);if(d->fields != 0){struct Cyc_List_List*_tmp40=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(d->fields))->v;for(0;_tmp40 != 0;_tmp40=_tmp40->tl){
if(!((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct
_dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple17*),struct Cyc_List_List*
l))Cyc_Interface_check_public_type_list)(ae,seen,({const char*_tmp41="datatype";
_tag_dyneither(_tmp41,sizeof(char),9);}),d->name,Cyc_Interface_get_type1,((
struct Cyc_Absyn_Datatypefield*)_tmp40->hd)->typs))_tmp3F=0;}}seen->datatypes=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(
seen->datatypes,d->name,_tmp3F);return _tmp3F;}}static int Cyc_Interface_check_public_enumdecl(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Enumdecl*d){
return 1;}static int Cyc_Interface_check_public_typedefdecl(struct Cyc_Dict_Dict ae,
struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*d){if(d->defn != 0)
return Cyc_Interface_check_public_type(ae,seen,(struct _dyneither_ptr)
_tag_dyneither(0,0,0),d->name,(void*)((struct Cyc_Core_Opt*)_check_null(d->defn))->v);
else{return 1;}}static int Cyc_Interface_check_public_vardecl(struct Cyc_Dict_Dict ae,
struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Vardecl*d){return Cyc_Interface_check_public_type(
ae,seen,({const char*_tmp42="variable";_tag_dyneither(_tmp42,sizeof(char),9);}),d->name,
d->type);}static struct Cyc_List_List*Cyc_Interface_get_abs_ns(union Cyc_Absyn_Nmspace
ns){union Cyc_Absyn_Nmspace _tmp43=ns;struct Cyc_List_List*_tmp44;_LLB: if((_tmp43.Abs_n).tag
!= 2)goto _LLD;_tmp44=(struct Cyc_List_List*)(_tmp43.Abs_n).val;_LLC: return _tmp44;
_LLD:;_LLE:((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const
char*_tmp45="get_abs_ns";_tag_dyneither(_tmp45,sizeof(char),11);}));_LLA:;}
static int Cyc_Interface_check_public_type(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*
seen,struct _dyneither_ptr obj,struct _tuple0*name,void*t){void*_tmp46=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp47;void*_tmp48;struct Cyc_Absyn_ArrayInfo _tmp49;
void*_tmp4A;void**_tmp4B;void*_tmp4C;struct Cyc_Absyn_FnInfo _tmp4D;void*_tmp4E;
struct Cyc_List_List*_tmp4F;struct Cyc_Absyn_VarargInfo*_tmp50;struct Cyc_List_List*
_tmp51;struct Cyc_Absyn_AggrInfo _tmp52;union Cyc_Absyn_AggrInfoU _tmp53;struct Cyc_List_List*
_tmp54;struct _tuple0*_tmp55;struct Cyc_Absyn_DatatypeInfo _tmp56;union Cyc_Absyn_DatatypeInfoU
_tmp57;struct Cyc_Absyn_Datatypedecl**_tmp58;struct Cyc_Absyn_Datatypedecl*_tmp59;
struct Cyc_List_List*_tmp5A;struct Cyc_Absyn_DatatypeFieldInfo _tmp5B;union Cyc_Absyn_DatatypeFieldInfoU
_tmp5C;struct _tuple1 _tmp5D;struct Cyc_Absyn_Datatypedecl*_tmp5E;struct Cyc_Absyn_Datatypefield*
_tmp5F;struct Cyc_List_List*_tmp60;_LL10: if(_tmp46 <= (void*)4)goto _LL22;if(*((int*)
_tmp46)!= 4)goto _LL12;_tmp47=((struct Cyc_Absyn_PointerType_struct*)_tmp46)->f1;
_tmp48=_tmp47.elt_typ;_LL11: _tmp4A=_tmp48;goto _LL13;_LL12: if(*((int*)_tmp46)!= 7)
goto _LL14;_tmp49=((struct Cyc_Absyn_ArrayType_struct*)_tmp46)->f1;_tmp4A=_tmp49.elt_type;
_LL13: _tmp4C=_tmp4A;goto _LL15;_LL14: if(*((int*)_tmp46)!= 16)goto _LL16;_tmp4B=((
struct Cyc_Absyn_TypedefType_struct*)_tmp46)->f4;if(_tmp4B == 0)goto _LL16;_tmp4C=*
_tmp4B;_LL15: return Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp4C);
_LL16: if(*((int*)_tmp46)!= 8)goto _LL18;_tmp4D=((struct Cyc_Absyn_FnType_struct*)
_tmp46)->f1;_tmp4E=_tmp4D.ret_typ;_tmp4F=_tmp4D.args;_tmp50=_tmp4D.cyc_varargs;
_LL17: {int b=((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct
_dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple7*),struct Cyc_List_List*
l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type2,
_tmp4F) && Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp4E);if(_tmp50 != 
0){void*_tmp62;struct Cyc_Absyn_VarargInfo _tmp61=*_tmp50;_tmp62=_tmp61.type;b=Cyc_Interface_check_public_type(
ae,seen,obj,name,_tmp62);}return b;}_LL18: if(*((int*)_tmp46)!= 9)goto _LL1A;_tmp51=((
struct Cyc_Absyn_TupleType_struct*)_tmp46)->f1;_LL19: return((int(*)(struct Cyc_Dict_Dict
ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*
f)(struct _tuple17*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(
ae,seen,obj,name,Cyc_Interface_get_type1,_tmp51);_LL1A: if(*((int*)_tmp46)!= 10)
goto _LL1C;_tmp52=((struct Cyc_Absyn_AggrType_struct*)_tmp46)->f1;_tmp53=_tmp52.aggr_info;
_tmp54=_tmp52.targs;_LL1B: {struct Cyc_Absyn_Aggrdecl*_tmp63=Cyc_Absyn_get_known_aggrdecl(
_tmp53);if(_tmp63->sc == (void*)0){Cyc_Interface_static_err(obj,name,({const char*
_tmp64="type";_tag_dyneither(_tmp64,sizeof(char),5);}),_tmp63->name);return 0;}
return Cyc_Interface_check_public_type_list(ae,seen,obj,name,Cyc_Core_identity,
_tmp54) && Cyc_Interface_check_public_aggrdecl(ae,seen,_tmp63);}_LL1C: if(*((int*)
_tmp46)!= 12)goto _LL1E;_tmp55=((struct Cyc_Absyn_EnumType_struct*)_tmp46)->f1;
_LL1D: {struct _tuple0 _tmp66;union Cyc_Absyn_Nmspace _tmp67;struct _dyneither_ptr*
_tmp68;struct _tuple0*_tmp65=_tmp55;_tmp66=*_tmp65;_tmp67=_tmp66.f1;_tmp68=_tmp66.f2;{
struct Cyc_List_List*_tmp69=Cyc_Interface_get_abs_ns(_tmp67);struct Cyc_Absyn_Enumdecl*
ed;{struct _handler_cons _tmp6A;_push_handler(& _tmp6A);{int _tmp6C=0;if(setjmp(
_tmp6A.handler))_tmp6C=1;if(!_tmp6C){{struct Cyc_Tcenv_Genv*_tmp6D=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp69);ed=*((
struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp6D->enumdecls,_tmp68);};_pop_handler();}else{void*_tmp6B=(void*)_exn_thrown;
void*_tmp6F=_tmp6B;_LL25: if(_tmp6F != Cyc_Dict_Absent)goto _LL27;_LL26:((int(*)(
struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct
Cyc_String_pa_struct _tmp72;_tmp72.tag=0;_tmp72.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp55));{void*_tmp70[1]={& _tmp72};Cyc_aprintf(({
const char*_tmp71="check_public_type (can't find enum %s)";_tag_dyneither(_tmp71,
sizeof(char),39);}),_tag_dyneither(_tmp70,sizeof(void*),1));}}));_LL27:;_LL28:(
void)_throw(_tmp6F);_LL24:;}}}if(ed->sc == (void*)0){Cyc_Interface_static_err(obj,
_tmp55,({const char*_tmp73="enum";_tag_dyneither(_tmp73,sizeof(char),5);}),ed->name);
return 0;}return 1;}}_LL1E: if(*((int*)_tmp46)!= 2)goto _LL20;_tmp56=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp46)->f1;_tmp57=_tmp56.datatype_info;if((_tmp57.KnownDatatype).tag != 2)goto
_LL20;_tmp58=(struct Cyc_Absyn_Datatypedecl**)(_tmp57.KnownDatatype).val;_tmp59=*
_tmp58;_tmp5A=_tmp56.targs;_LL1F: {struct _tuple0 _tmp75;union Cyc_Absyn_Nmspace
_tmp76;struct _dyneither_ptr*_tmp77;struct _tuple0*_tmp74=_tmp59->name;_tmp75=*
_tmp74;_tmp76=_tmp75.f1;_tmp77=_tmp75.f2;{struct Cyc_List_List*_tmp78=Cyc_Interface_get_abs_ns(
_tmp76);struct Cyc_Absyn_Datatypedecl*tud;{struct _handler_cons _tmp79;
_push_handler(& _tmp79);{int _tmp7B=0;if(setjmp(_tmp79.handler))_tmp7B=1;if(!
_tmp7B){{struct Cyc_Tcenv_Genv*_tmp7C=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp78);tud=*((struct Cyc_Absyn_Datatypedecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7C->datatypedecls,
_tmp77);};_pop_handler();}else{void*_tmp7A=(void*)_exn_thrown;void*_tmp7E=_tmp7A;
_LL2A: if(_tmp7E != Cyc_Dict_Absent)goto _LL2C;_LL2B:((int(*)(struct _dyneither_ptr s))
Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp81;_tmp81.tag=0;_tmp81.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp59->name));{void*_tmp7F[1]={& _tmp81};Cyc_aprintf(({const char*_tmp80="check_public_type (can't find datatype %s)";
_tag_dyneither(_tmp80,sizeof(char),43);}),_tag_dyneither(_tmp7F,sizeof(void*),1));}}));
_LL2C:;_LL2D:(void)_throw(_tmp7E);_LL29:;}}}if(tud->sc == (void*)0){Cyc_Interface_static_err(
obj,name,({const char*_tmp82="datatype";_tag_dyneither(_tmp82,sizeof(char),9);}),
tud->name);return 0;}return Cyc_Interface_check_public_type_list(ae,seen,obj,name,
Cyc_Core_identity,_tmp5A) && Cyc_Interface_check_public_datatypedecl(ae,seen,tud);}}
_LL20: if(*((int*)_tmp46)!= 3)goto _LL22;_tmp5B=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp46)->f1;_tmp5C=_tmp5B.field_info;if((_tmp5C.KnownDatatypefield).tag != 2)goto
_LL22;_tmp5D=(struct _tuple1)(_tmp5C.KnownDatatypefield).val;_tmp5E=_tmp5D.f1;
_tmp5F=_tmp5D.f2;_tmp60=_tmp5B.targs;_LL21: {struct _tuple0 _tmp84;union Cyc_Absyn_Nmspace
_tmp85;struct _dyneither_ptr*_tmp86;struct _tuple0*_tmp83=_tmp5E->name;_tmp84=*
_tmp83;_tmp85=_tmp84.f1;_tmp86=_tmp84.f2;{struct Cyc_List_List*_tmp87=Cyc_Interface_get_abs_ns(
_tmp85);struct Cyc_Absyn_Datatypedecl*tud;{struct _handler_cons _tmp88;
_push_handler(& _tmp88);{int _tmp8A=0;if(setjmp(_tmp88.handler))_tmp8A=1;if(!
_tmp8A){{struct Cyc_Tcenv_Genv*_tmp8B=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp87);tud=*((struct Cyc_Absyn_Datatypedecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8B->datatypedecls,
_tmp86);};_pop_handler();}else{void*_tmp89=(void*)_exn_thrown;void*_tmp8D=_tmp89;
_LL2F: if(_tmp8D != Cyc_Dict_Absent)goto _LL31;_LL30:((int(*)(struct _dyneither_ptr s))
Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp90;_tmp90.tag=0;_tmp90.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp5E->name));{void*_tmp8E[1]={& _tmp90};Cyc_aprintf(({const char*_tmp8F="check_public_type (can't find datatype %s and search its fields)";
_tag_dyneither(_tmp8F,sizeof(char),65);}),_tag_dyneither(_tmp8E,sizeof(void*),1));}}));
_LL31:;_LL32:(void)_throw(_tmp8D);_LL2E:;}}}if(tud->fields == 0)((int(*)(struct
_dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp93;_tmp93.tag=0;_tmp93.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tud->name));{void*_tmp91[1]={& _tmp93};Cyc_aprintf(({const char*_tmp92="check_public_type (datatype %s has no fields)";
_tag_dyneither(_tmp92,sizeof(char),46);}),_tag_dyneither(_tmp91,sizeof(void*),1));}}));{
struct Cyc_Absyn_Datatypefield*tuf1=0;{struct Cyc_List_List*_tmp94=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp94 != 0;_tmp94=_tmp94->tl){
if(Cyc_strptrcmp((*_tmp5F->name).f2,(*((struct Cyc_Absyn_Datatypefield*)_tmp94->hd)->name).f2)
== 0){tuf1=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)
_tmp94->hd);break;}}}if(tuf1 == 0)((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp97;_tmp97.tag=0;_tmp97.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5F->name));{
void*_tmp95[1]={& _tmp97};Cyc_aprintf(({const char*_tmp96="check_public_type (can't find datatypefield %s)";
_tag_dyneither(_tmp96,sizeof(char),48);}),_tag_dyneither(_tmp95,sizeof(void*),1));}}));{
struct Cyc_Absyn_Datatypefield*tuf=(struct Cyc_Absyn_Datatypefield*)tuf1;if(tud->sc
== (void*)0){Cyc_Interface_static_err(obj,name,({const char*_tmp98="datatype";
_tag_dyneither(_tmp98,sizeof(char),9);}),tud->name);return 0;}if(tud->sc == (void*)
1){Cyc_Interface_abstract_err(obj,name,({const char*_tmp99="datatype";
_tag_dyneither(_tmp99,sizeof(char),9);}),tud->name);return 0;}if(tuf->sc == (void*)
0){Cyc_Interface_static_err(obj,name,(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp9C;_tmp9C.tag=0;_tmp9C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp9A[1]={& _tmp9C};Cyc_aprintf(({const char*_tmp9B="field %s of";
_tag_dyneither(_tmp9B,sizeof(char),12);}),_tag_dyneither(_tmp9A,sizeof(void*),1));}}),
tud->name);return 0;}return Cyc_Interface_check_public_type_list(ae,seen,obj,name,
Cyc_Core_identity,_tmp60) && Cyc_Interface_check_public_datatypedecl(ae,seen,tud);}}}}
_LL22:;_LL23: return 1;_LLF:;}struct _tuple18{struct Cyc_Interface_Ienv*f1;struct Cyc_Interface_Ienv*
f2;int f3;struct Cyc_Dict_Dict f4;struct Cyc_Interface_Seen*f5;struct Cyc_Interface_I*
f6;};static void Cyc_Interface_extract_aggrdecl(struct _tuple18*env,struct
_dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl**dp){struct _tuple18 _tmp9E;struct Cyc_Interface_Ienv*
_tmp9F;struct Cyc_Interface_Ienv*_tmpA0;int _tmpA1;struct Cyc_Dict_Dict _tmpA2;
struct Cyc_Interface_Seen*_tmpA3;struct _tuple18*_tmp9D=env;_tmp9E=*_tmp9D;_tmp9F=
_tmp9E.f1;_tmpA0=_tmp9E.f2;_tmpA1=_tmp9E.f3;_tmpA2=_tmp9E.f4;_tmpA3=_tmp9E.f5;{
struct Cyc_Absyn_Aggrdecl*_tmpA4=*dp;void*_tmpA5=_tmpA4->sc;_LL34: if((int)_tmpA5
!= 0)goto _LL36;_LL35: if(_tmpA1  && _tmpA4->impl == 0)Cyc_Interface_fields_err(({
const char*_tmpA6="static";_tag_dyneither(_tmpA6,sizeof(char),7);}),({const char*
_tmpA7="struct/union";_tag_dyneither(_tmpA7,sizeof(char),13);}),_tmpA4->name);
goto _LL33;_LL36: if((int)_tmpA5 != 1)goto _LL38;_LL37: if(_tmpA4->impl == 0){if(
_tmpA1)Cyc_Interface_fields_err(({const char*_tmpA8="abstract";_tag_dyneither(
_tmpA8,sizeof(char),9);}),({const char*_tmpA9="struct/union";_tag_dyneither(
_tmpA9,sizeof(char),13);}),_tmpA4->name);}else{_tmpA4=({struct Cyc_Absyn_Aggrdecl*
_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=*_tmpA4;_tmpAA;});_tmpA4->impl=0;}
if(Cyc_Interface_check_public_aggrdecl(_tmpA2,_tmpA3,_tmpA4))_tmpA0->aggrdecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*
v))Cyc_Dict_insert)(_tmpA0->aggrdecls,x,_tmpA4);goto _LL33;_LL38: if((int)_tmpA5 != 
2)goto _LL3A;_LL39: if(_tmpA4->impl == 0){Cyc_Interface_fields_err(({const char*
_tmpAB="public";_tag_dyneither(_tmpAB,sizeof(char),7);}),({const char*_tmpAC="struct/union";
_tag_dyneither(_tmpAC,sizeof(char),13);}),_tmpA4->name);_tmpA4=({struct Cyc_Absyn_Aggrdecl*
_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=*_tmpA4;_tmpAD;});_tmpA4->sc=(void*)
1;}if(Cyc_Interface_check_public_aggrdecl(_tmpA2,_tmpA3,_tmpA4))_tmpA0->aggrdecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*
v))Cyc_Dict_insert)(_tmpA0->aggrdecls,x,_tmpA4);goto _LL33;_LL3A: if((int)_tmpA5 != 
4)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if((int)_tmpA5 != 3)goto _LL3E;_LL3D: if(Cyc_Interface_check_public_aggrdecl(
_tmpA2,_tmpA3,_tmpA4))_tmp9F->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp9F->aggrdecls,
x,_tmpA4);goto _LL33;_LL3E: if((int)_tmpA5 != 5)goto _LL33;_LL3F:((int(*)(struct
_dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmpAE="add_aggrdecl";
_tag_dyneither(_tmpAE,sizeof(char),13);}));goto _LL33;_LL33:;}}static void Cyc_Interface_extract_enumdecl(
struct _tuple18*env,struct _dyneither_ptr*x,struct Cyc_Absyn_Enumdecl**dp){struct
_tuple18 _tmpB0;struct Cyc_Interface_Ienv*_tmpB1;struct Cyc_Interface_Ienv*_tmpB2;
int _tmpB3;struct Cyc_Dict_Dict _tmpB4;struct Cyc_Interface_Seen*_tmpB5;struct
_tuple18*_tmpAF=env;_tmpB0=*_tmpAF;_tmpB1=_tmpB0.f1;_tmpB2=_tmpB0.f2;_tmpB3=
_tmpB0.f3;_tmpB4=_tmpB0.f4;_tmpB5=_tmpB0.f5;{struct Cyc_Absyn_Enumdecl*_tmpB6=*dp;
void*_tmpB7=_tmpB6->sc;_LL41: if((int)_tmpB7 != 0)goto _LL43;_LL42: if(_tmpB3  && 
_tmpB6->fields == 0)Cyc_Interface_fields_err(({const char*_tmpB8="static";
_tag_dyneither(_tmpB8,sizeof(char),7);}),({const char*_tmpB9="enum";
_tag_dyneither(_tmpB9,sizeof(char),5);}),_tmpB6->name);goto _LL40;_LL43: if((int)
_tmpB7 != 1)goto _LL45;_LL44: if(_tmpB6->fields == 0){if(_tmpB3)Cyc_Interface_fields_err(({
const char*_tmpBA="abstract";_tag_dyneither(_tmpBA,sizeof(char),9);}),({const char*
_tmpBB="enum";_tag_dyneither(_tmpBB,sizeof(char),5);}),_tmpB6->name);}else{
_tmpB6=({struct Cyc_Absyn_Enumdecl*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=*
_tmpB6;_tmpBC;});_tmpB6->fields=0;}if(Cyc_Interface_check_public_enumdecl(_tmpB4,
_tmpB5,_tmpB6))_tmpB2->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmpB2->enumdecls,
x,_tmpB6);goto _LL40;_LL45: if((int)_tmpB7 != 2)goto _LL47;_LL46: if(_tmpB6->fields == 
0){Cyc_Interface_fields_err(({const char*_tmpBD="public";_tag_dyneither(_tmpBD,
sizeof(char),7);}),({const char*_tmpBE="enum";_tag_dyneither(_tmpBE,sizeof(char),
5);}),_tmpB6->name);_tmpB6=({struct Cyc_Absyn_Enumdecl*_tmpBF=_cycalloc(sizeof(*
_tmpBF));_tmpBF[0]=*_tmpB6;_tmpBF;});_tmpB6->sc=(void*)1;}if(Cyc_Interface_check_public_enumdecl(
_tmpB4,_tmpB5,_tmpB6))_tmpB2->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmpB2->enumdecls,
x,_tmpB6);goto _LL40;_LL47: if((int)_tmpB7 != 4)goto _LL49;_LL48: goto _LL4A;_LL49: if((
int)_tmpB7 != 3)goto _LL4B;_LL4A: if(Cyc_Interface_check_public_enumdecl(_tmpB4,
_tmpB5,_tmpB6))_tmpB1->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmpB1->enumdecls,
x,_tmpB6);goto _LL40;_LL4B: if((int)_tmpB7 != 5)goto _LL40;_LL4C:((int(*)(struct
_dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmpC0="add_enumdecl";
_tag_dyneither(_tmpC0,sizeof(char),13);}));goto _LL40;_LL40:;}}static void Cyc_Interface_extract_xdatatypefielddecl(
struct Cyc_Interface_I*i,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_Absyn_Datatypefield*
f){struct Cyc_List_List*_tmpC1=0;{union Cyc_Absyn_Nmspace _tmpC2=(*f->name).f1;
struct Cyc_List_List*_tmpC3;struct Cyc_List_List*_tmpC4;_LL4E: if((_tmpC2.Rel_n).tag
!= 1)goto _LL50;_tmpC3=(struct Cyc_List_List*)(_tmpC2.Rel_n).val;_LL4F: _tmpC4=
_tmpC3;goto _LL51;_LL50: if((_tmpC2.Abs_n).tag != 2)goto _LL52;_tmpC4=(struct Cyc_List_List*)(
_tmpC2.Abs_n).val;_LL51: _tmpC1=_tmpC4;goto _LL4D;_LL52:;_LL53: goto _LL4D;_LL4D:;}{
struct Cyc_Dict_Dict*dict;{void*_tmpC5=f->sc;_LL55: if((int)_tmpC5 != 0)goto _LL57;
_LL56: return;_LL57: if((int)_tmpC5 != 3)goto _LL59;_LL58: dict=& i->imports;goto _LL54;
_LL59: if((int)_tmpC5 != 2)goto _LL5B;_LL5A: dict=& i->exports;goto _LL54;_LL5B:;_LL5C:((
int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const char*_tmpC6="add_xdatatypefielddecl";
_tag_dyneither(_tmpC6,sizeof(char),23);}));_LL54:;}{struct Cyc_Interface_Ienv*env;{
struct _handler_cons _tmpC7;_push_handler(& _tmpC7);{int _tmpC9=0;if(setjmp(_tmpC7.handler))
_tmpC9=1;if(!_tmpC9){env=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(*dict,_tmpC1);;_pop_handler();}else{void*
_tmpC8=(void*)_exn_thrown;void*_tmpCB=_tmpC8;_LL5E: if(_tmpCB != Cyc_Dict_Absent)
goto _LL60;_LL5F: env=Cyc_Interface_new_ienv();*dict=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*
dict,_tmpC1,env);goto _LL5D;_LL60:;_LL61:(void)_throw(_tmpCB);_LL5D:;}}}env->xdatatypefielddecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Tcdecl_Xdatatypefielddecl*
v))Cyc_Dict_insert)(env->xdatatypefielddecls,(*f->name).f2,({struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->base=d;_tmpCC->field=f;_tmpCC;}));}}}
static void Cyc_Interface_extract_datatypedecl(struct _tuple18*env,struct
_dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl**dp){struct _tuple18 _tmpCE;struct
Cyc_Interface_Ienv*_tmpCF;struct Cyc_Interface_Ienv*_tmpD0;int _tmpD1;struct Cyc_Dict_Dict
_tmpD2;struct Cyc_Interface_Seen*_tmpD3;struct Cyc_Interface_I*_tmpD4;struct
_tuple18*_tmpCD=env;_tmpCE=*_tmpCD;_tmpCF=_tmpCE.f1;_tmpD0=_tmpCE.f2;_tmpD1=
_tmpCE.f3;_tmpD2=_tmpCE.f4;_tmpD3=_tmpCE.f5;_tmpD4=_tmpCE.f6;{struct Cyc_Absyn_Datatypedecl*
_tmpD5=*dp;void*_tmpD6=_tmpD5->sc;_LL63: if((int)_tmpD6 != 0)goto _LL65;_LL64: if((!
_tmpD5->is_extensible  && _tmpD1) && _tmpD5->fields == 0)Cyc_Interface_fields_err(({
const char*_tmpD7="static";_tag_dyneither(_tmpD7,sizeof(char),7);}),({const char*
_tmpD8="datatype";_tag_dyneither(_tmpD8,sizeof(char),9);}),_tmpD5->name);goto
_LL62;_LL65: if((int)_tmpD6 != 1)goto _LL67;_LL66: if(_tmpD5->fields == 0){if(!_tmpD5->is_extensible
 && _tmpD1)Cyc_Interface_fields_err(({const char*_tmpD9="abstract";_tag_dyneither(
_tmpD9,sizeof(char),9);}),({const char*_tmpDA="datatype";_tag_dyneither(_tmpDA,
sizeof(char),9);}),_tmpD5->name);}else{_tmpD5=({struct Cyc_Absyn_Datatypedecl*
_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=*_tmpD5;_tmpDB;});_tmpD5->fields=0;}
if(Cyc_Interface_check_public_datatypedecl(_tmpD2,_tmpD3,_tmpD5))_tmpD0->datatypedecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*
v))Cyc_Dict_insert)(_tmpD0->datatypedecls,x,_tmpD5);goto _LL62;_LL67: if((int)
_tmpD6 != 2)goto _LL69;_LL68: _tmpD5=({struct Cyc_Absyn_Datatypedecl*_tmpDC=
_cycalloc(sizeof(*_tmpDC));_tmpDC[0]=*_tmpD5;_tmpDC;});if(!_tmpD5->is_extensible
 && _tmpD5->fields == 0){Cyc_Interface_fields_err(({const char*_tmpDD="public";
_tag_dyneither(_tmpDD,sizeof(char),7);}),({const char*_tmpDE="datatype";
_tag_dyneither(_tmpDE,sizeof(char),9);}),_tmpD5->name);_tmpD5->sc=(void*)1;}if(
Cyc_Interface_check_public_datatypedecl(_tmpD2,_tmpD3,_tmpD5)){if(_tmpD5->is_extensible
 && _tmpD5->fields != 0){struct Cyc_List_List*_tmpDF=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmpD5->fields))->v;_tmpD5->fields=0;{struct Cyc_List_List*
_tmpE0=_tmpDF;for(0;_tmpE0 != 0;_tmpE0=_tmpE0->tl){Cyc_Interface_extract_xdatatypefielddecl(
_tmpD4,_tmpD5,(struct Cyc_Absyn_Datatypefield*)_tmpE0->hd);}}}_tmpD0->datatypedecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*
v))Cyc_Dict_insert)(_tmpD0->datatypedecls,x,_tmpD5);}goto _LL62;_LL69: if((int)
_tmpD6 != 4)goto _LL6B;_LL6A:((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({
const char*_tmpE1="extract_datatypedecl";_tag_dyneither(_tmpE1,sizeof(char),21);}));
_LL6B: if((int)_tmpD6 != 3)goto _LL6D;_LL6C: if(Cyc_Interface_check_public_datatypedecl(
_tmpD2,_tmpD3,_tmpD5)){if(_tmpD5->is_extensible  && _tmpD5->fields != 0){_tmpD5=({
struct Cyc_Absyn_Datatypedecl*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2[0]=*_tmpD5;
_tmpE2;});{struct Cyc_List_List*_tmpE3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmpD5->fields))->v;_tmpD5->fields=0;{struct Cyc_List_List*_tmpE4=
_tmpE3;for(0;_tmpE4 != 0;_tmpE4=_tmpE4->tl){Cyc_Interface_extract_xdatatypefielddecl(
_tmpD4,_tmpD5,(struct Cyc_Absyn_Datatypefield*)_tmpE4->hd);}}}}_tmpCF->datatypedecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*
v))Cyc_Dict_insert)(_tmpCF->datatypedecls,x,_tmpD5);}goto _LL62;_LL6D: if((int)
_tmpD6 != 5)goto _LL62;_LL6E:((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({
const char*_tmpE5="add_datatypedecl";_tag_dyneither(_tmpE5,sizeof(char),17);}));
goto _LL62;_LL62:;}}static void Cyc_Interface_extract_typedef(struct _tuple18*env,
struct _dyneither_ptr*x,struct Cyc_Absyn_Typedefdecl*d){struct _tuple18 _tmpE7;
struct Cyc_Interface_Ienv*_tmpE8;struct Cyc_Dict_Dict _tmpE9;struct Cyc_Interface_Seen*
_tmpEA;struct _tuple18*_tmpE6=env;_tmpE7=*_tmpE6;_tmpE8=_tmpE7.f2;_tmpE9=_tmpE7.f4;
_tmpEA=_tmpE7.f5;if(Cyc_Interface_check_public_typedefdecl(_tmpE9,_tmpEA,d))
_tmpE8->typedefdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmpE8->typedefdecls,
x,d);}static void Cyc_Interface_extract_ordinarie(struct _tuple18*env,struct
_dyneither_ptr*x,struct _tuple5*v){struct _tuple18 _tmpEC;struct Cyc_Interface_Ienv*
_tmpED;struct Cyc_Interface_Ienv*_tmpEE;int _tmpEF;struct Cyc_Dict_Dict _tmpF0;
struct Cyc_Interface_Seen*_tmpF1;struct _tuple18*_tmpEB=env;_tmpEC=*_tmpEB;_tmpED=
_tmpEC.f1;_tmpEE=_tmpEC.f2;_tmpEF=_tmpEC.f3;_tmpF0=_tmpEC.f4;_tmpF1=_tmpEC.f5;{
void*_tmpF2=(*v).f1;void*_tmpF3=_tmpF2;void*_tmpF4;_LL70: if(*((int*)_tmpF3)!= 0)
goto _LL72;_tmpF4=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmpF3)->f1;_LL71:{void*
_tmpF5=_tmpF4;struct Cyc_Absyn_Fndecl*_tmpF6;struct Cyc_Absyn_Vardecl*_tmpF7;_LL75:
if(_tmpF5 <= (void*)1)goto _LL79;if(*((int*)_tmpF5)!= 1)goto _LL77;_tmpF6=((struct
Cyc_Absyn_Funname_b_struct*)_tmpF5)->f1;_LL76: {struct Cyc_Absyn_Vardecl*_tmpF8=({
struct Cyc_Absyn_Vardecl*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->sc=_tmpF6->sc;
_tmpF9->name=_tmpF6->name;_tmpF9->tq=Cyc_Absyn_empty_tqual(0);_tmpF9->type=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmpF6->cached_typ))->v;_tmpF9->initializer=0;
_tmpF9->rgn=0;_tmpF9->attributes=0;_tmpF9->escapes=0;_tmpF9;});_tmpEF=0;_tmpF7=
_tmpF8;goto _LL78;}_LL77: if(*((int*)_tmpF5)!= 0)goto _LL79;_tmpF7=((struct Cyc_Absyn_Global_b_struct*)
_tmpF5)->f1;_LL78: if(_tmpF7->initializer != 0){_tmpF7=({struct Cyc_Absyn_Vardecl*
_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA[0]=*_tmpF7;_tmpFA;});_tmpF7->initializer=
0;}{void*_tmpFB=_tmpF7->sc;_LL7C: if((int)_tmpFB != 0)goto _LL7E;_LL7D: if(_tmpEF
 && Cyc_Tcutil_is_function_type(_tmpF7->type))Cyc_Interface_body_err(({const char*
_tmpFC="static";_tag_dyneither(_tmpFC,sizeof(char),7);}),_tmpF7->name);goto _LL7B;
_LL7E: if((int)_tmpFB != 5)goto _LL80;_LL7F: goto _LL81;_LL80: if((int)_tmpFB != 1)goto
_LL82;_LL81:((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(({const
char*_tmpFD="extract_ordinarie";_tag_dyneither(_tmpFD,sizeof(char),18);}));_LL82:
if((int)_tmpFB != 2)goto _LL84;_LL83: if(_tmpEF  && Cyc_Tcutil_is_function_type(
_tmpF7->type))Cyc_Interface_body_err(({const char*_tmpFE="public";_tag_dyneither(
_tmpFE,sizeof(char),7);}),_tmpF7->name);if(Cyc_Interface_check_public_vardecl(
_tmpF0,_tmpF1,_tmpF7))_tmpEE->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpEE->vardecls,
x,_tmpF7);goto _LL7B;_LL84: if((int)_tmpFB != 4)goto _LL86;_LL85: goto _LL87;_LL86: if((
int)_tmpFB != 3)goto _LL7B;_LL87: if(Cyc_Interface_check_public_vardecl(_tmpF0,
_tmpF1,_tmpF7))_tmpED->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpED->vardecls,
x,_tmpF7);goto _LL7B;_LL7B:;}goto _LL74;_LL79:;_LL7A: goto _LL74;_LL74:;}goto _LL6F;
_LL72:;_LL73: goto _LL6F;_LL6F:;}}struct _tuple19{struct Cyc_Interface_I*f1;int f2;
struct Cyc_Dict_Dict f3;struct Cyc_Interface_Seen*f4;};static void Cyc_Interface_extract_f(
struct _tuple19*env_f,struct Cyc_List_List*ns,struct Cyc_Tcenv_Genv*ge){struct
_tuple19 _tmp100;struct Cyc_Interface_I*_tmp101;int _tmp102;struct Cyc_Dict_Dict
_tmp103;struct Cyc_Interface_Seen*_tmp104;struct _tuple19*_tmpFF=env_f;_tmp100=*
_tmpFF;_tmp101=_tmp100.f1;_tmp102=_tmp100.f2;_tmp103=_tmp100.f3;_tmp104=_tmp100.f4;{
struct _tuple18 _tmp105=({struct _tuple18 _tmp106;_tmp106.f1=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp101->imports,ns);
_tmp106.f2=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(_tmp101->exports,ns);_tmp106.f3=_tmp102;_tmp106.f4=_tmp103;
_tmp106.f5=_tmp104;_tmp106.f6=_tmp101;_tmp106;});((void(*)(void(*f)(struct
_tuple18*,struct _dyneither_ptr*,struct Cyc_Absyn_Aggrdecl**),struct _tuple18*env,
struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_aggrdecl,& _tmp105,
ge->aggrdecls);((void(*)(void(*f)(struct _tuple18*,struct _dyneither_ptr*,struct
Cyc_Absyn_Datatypedecl**),struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_Interface_extract_datatypedecl,& _tmp105,ge->datatypedecls);((void(*)(void(*f)(
struct _tuple18*,struct _dyneither_ptr*,struct Cyc_Absyn_Enumdecl**),struct _tuple18*
env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_enumdecl,&
_tmp105,ge->enumdecls);((void(*)(void(*f)(struct _tuple18*,struct _dyneither_ptr*,
struct Cyc_Absyn_Typedefdecl*),struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_Interface_extract_typedef,& _tmp105,ge->typedefs);((void(*)(void(*f)(struct
_tuple18*,struct _dyneither_ptr*,struct _tuple5*),struct _tuple18*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_Interface_extract_ordinarie,& _tmp105,ge->ordinaries);}}
static struct Cyc_Interface_I*Cyc_Interface_gen_extract(struct Cyc_Dict_Dict ae,int
check_complete_defs){struct _tuple19 env=({struct _tuple19 _tmp107;_tmp107.f1=((
struct Cyc_Interface_I*(*)(struct Cyc_Dict_Dict skel))Cyc_Interface_skel2i)(ae);
_tmp107.f2=check_complete_defs;_tmp107.f3=ae;_tmp107.f4=Cyc_Interface_new_seen();
_tmp107;});((void(*)(void(*f)(struct _tuple19*,struct Cyc_List_List*,struct Cyc_Tcenv_Genv*),
struct _tuple19*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_f,&
env,ae);return env.f1;}struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict
ae){return Cyc_Interface_gen_extract(ae,1);}inline static void Cyc_Interface_check_err(
struct _dyneither_ptr*msg1,struct _dyneither_ptr msg2){({struct Cyc_String_pa_struct
_tmp10A;_tmp10A.tag=0;_tmp10A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
msg2);{void*_tmp108[1]={& _tmp10A};Cyc_Tcdecl_merr(0,msg1,({const char*_tmp109="%s";
_tag_dyneither(_tmp109,sizeof(char),3);}),_tag_dyneither(_tmp108,sizeof(void*),1));}});}
struct _tuple20{int f1;struct Cyc_Dict_Dict f2;int(*f3)(void*,void*,struct
_dyneither_ptr*);struct _dyneither_ptr f4;struct _dyneither_ptr*f5;};static void Cyc_Interface_incl_dict_f(
struct _tuple20*env,struct _dyneither_ptr*x,void*y1){struct _tuple20 _tmp10C;int
_tmp10D;int*_tmp10E;struct Cyc_Dict_Dict _tmp10F;int(*_tmp110)(void*,void*,struct
_dyneither_ptr*);struct _dyneither_ptr _tmp111;struct _dyneither_ptr*_tmp112;struct
_tuple20*_tmp10B=env;_tmp10C=*_tmp10B;_tmp10D=_tmp10C.f1;_tmp10E=(int*)&(*
_tmp10B).f1;_tmp10F=_tmp10C.f2;_tmp110=_tmp10C.f3;_tmp111=_tmp10C.f4;_tmp112=
_tmp10C.f5;{struct _handler_cons _tmp113;_push_handler(& _tmp113);{int _tmp115=0;if(
setjmp(_tmp113.handler))_tmp115=1;if(!_tmp115){{void*_tmp116=((void*(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp10F,x);if(!_tmp110(
y1,_tmp116,_tmp112))*_tmp10E=0;};_pop_handler();}else{void*_tmp114=(void*)
_exn_thrown;void*_tmp118=_tmp114;_LL89: if(_tmp118 != Cyc_Dict_Absent)goto _LL8B;
_LL8A: Cyc_Interface_check_err(_tmp112,(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp11C;_tmp11C.tag=0;_tmp11C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*x);{
struct Cyc_String_pa_struct _tmp11B;_tmp11B.tag=0;_tmp11B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp111);{void*_tmp119[2]={& _tmp11B,& _tmp11C};Cyc_aprintf(({
const char*_tmp11A="%s %s is missing";_tag_dyneither(_tmp11A,sizeof(char),17);}),
_tag_dyneither(_tmp119,sizeof(void*),2));}}}));*_tmp10E=0;goto _LL88;_LL8B:;_LL8C:(
void)_throw(_tmp118);_LL88:;}}}}static int Cyc_Interface_incl_dict(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(void*,void*,struct _dyneither_ptr*),
struct _dyneither_ptr t,struct _dyneither_ptr*msg){struct _tuple20 _tmp11D=({struct
_tuple20 _tmp11E;_tmp11E.f1=1;_tmp11E.f2=dic2;_tmp11E.f3=incl_f;_tmp11E.f4=t;
_tmp11E.f5=msg;_tmp11E;});((void(*)(void(*f)(struct _tuple20*,struct
_dyneither_ptr*,void*),struct _tuple20*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_Interface_incl_dict_f,& _tmp11D,dic1);return _tmp11D.f1;}static int Cyc_Interface_incl_aggrdecl(
struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct _dyneither_ptr*msg){
struct Cyc_Absyn_Aggrdecl*_tmp11F=Cyc_Tcdecl_merge_aggrdecl(d0,d1,0,msg);if(
_tmp11F == 0)return 0;if((struct Cyc_Absyn_Aggrdecl*)d0 != _tmp11F){Cyc_Interface_check_err(
msg,(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp122;_tmp122.tag=0;
_tmp122.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
d1->name));{void*_tmp120[1]={& _tmp122};Cyc_aprintf(({const char*_tmp121="declaration of type %s discloses too much information";
_tag_dyneither(_tmp121,sizeof(char),54);}),_tag_dyneither(_tmp120,sizeof(void*),
1));}}));return 0;}return 1;}static int Cyc_Interface_incl_datatypedecl(struct Cyc_Absyn_Datatypedecl*
d0,struct Cyc_Absyn_Datatypedecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Datatypedecl*
_tmp123=Cyc_Tcdecl_merge_datatypedecl(d0,d1,0,msg);if(_tmp123 == 0)return 0;if((
struct Cyc_Absyn_Datatypedecl*)d0 != _tmp123){Cyc_Interface_check_err(msg,(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp126;_tmp126.tag=0;_tmp126.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));{
void*_tmp124[1]={& _tmp126};Cyc_aprintf(({const char*_tmp125="declaration of datatype %s discloses too much information";
_tag_dyneither(_tmp125,sizeof(char),58);}),_tag_dyneither(_tmp124,sizeof(void*),
1));}}));return 0;}return 1;}static int Cyc_Interface_incl_enumdecl(struct Cyc_Absyn_Enumdecl*
d0,struct Cyc_Absyn_Enumdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Enumdecl*
_tmp127=Cyc_Tcdecl_merge_enumdecl(d0,d1,0,msg);if(_tmp127 == 0)return 0;if((struct
Cyc_Absyn_Enumdecl*)d0 != _tmp127){Cyc_Interface_check_err(msg,(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp12A;_tmp12A.tag=0;_tmp12A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));{
void*_tmp128[1]={& _tmp12A};Cyc_aprintf(({const char*_tmp129="declaration of enum %s discloses too much information";
_tag_dyneither(_tmp129,sizeof(char),54);}),_tag_dyneither(_tmp128,sizeof(void*),
1));}}));return 0;}return 1;}static int Cyc_Interface_incl_vardecl(struct Cyc_Absyn_Vardecl*
d0,struct Cyc_Absyn_Vardecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Vardecl*
_tmp12B=Cyc_Tcdecl_merge_vardecl(d0,d1,0,msg);if(_tmp12B == 0)return 0;if((struct
Cyc_Absyn_Vardecl*)d0 != _tmp12B){Cyc_Interface_check_err(msg,(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp12E;_tmp12E.tag=0;_tmp12E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));{
void*_tmp12C[1]={& _tmp12E};Cyc_aprintf(({const char*_tmp12D="declaration of variable %s discloses too much information";
_tag_dyneither(_tmp12D,sizeof(char),58);}),_tag_dyneither(_tmp12C,sizeof(void*),
1));}}));return 0;}return 1;}static int Cyc_Interface_incl_typedefdecl(struct Cyc_Absyn_Typedefdecl*
d0,struct Cyc_Absyn_Typedefdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Typedefdecl*
_tmp12F=Cyc_Tcdecl_merge_typedefdecl(d0,d1,0,msg);if(_tmp12F == 0)return 0;if((
struct Cyc_Absyn_Typedefdecl*)d0 != _tmp12F){Cyc_Interface_check_err(msg,(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp132;_tmp132.tag=0;_tmp132.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name));{
void*_tmp130[1]={& _tmp132};Cyc_aprintf(({const char*_tmp131="declaration of typedef %s discloses too much information";
_tag_dyneither(_tmp131,sizeof(char),57);}),_tag_dyneither(_tmp130,sizeof(void*),
1));}}));return 0;}return 1;}static int Cyc_Interface_incl_xdatatypefielddecl(struct
Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct
_dyneither_ptr*msg){struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp133=Cyc_Tcdecl_merge_xdatatypefielddecl(
d0,d1,0,msg);if(_tmp133 == 0)return 0;if((struct Cyc_Tcdecl_Xdatatypefielddecl*)d0
!= _tmp133){Cyc_Interface_check_err(msg,(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp136;_tmp136.tag=0;_tmp136.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((d1->field)->name));{void*_tmp134[1]={& _tmp136};Cyc_aprintf(({
const char*_tmp135="declaration of xdatatypefield %s discloses too much information";
_tag_dyneither(_tmp135,sizeof(char),64);}),_tag_dyneither(_tmp134,sizeof(void*),
1));}}));return 0;}return 1;}struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*
merge_f)(void*,void*,struct Cyc_Position_Segment*,struct _dyneither_ptr*),struct
_dyneither_ptr t,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_ienv(
struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*
msg){int _tmp137=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*
incl_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*),
struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->aggrdecls,
ie2->aggrdecls,Cyc_Interface_incl_aggrdecl,({const char*_tmp142="type";
_tag_dyneither(_tmp142,sizeof(char),5);}),msg);int _tmp138=((int(*)(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Datatypedecl*,struct
Cyc_Absyn_Datatypedecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct
_dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->datatypedecls,ie2->datatypedecls,
Cyc_Interface_incl_datatypedecl,({const char*_tmp141="datatype";_tag_dyneither(
_tmp141,sizeof(char),9);}),msg);int _tmp139=((int(*)(struct Cyc_Dict_Dict dic1,
struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(
ie1->enumdecls,ie2->enumdecls,Cyc_Interface_incl_enumdecl,({const char*_tmp140="enum";
_tag_dyneither(_tmp140,sizeof(char),5);}),msg);int _tmp13A=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct
Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Position_Segment*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct
_dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,ie2->typedefdecls,((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_typedefdecl,({const char*_tmp13F="typedef";
_tag_dyneither(_tmp13F,sizeof(char),8);}),msg)!= 0;int _tmp13B=((int(*)(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(
ie1->vardecls,ie2->vardecls,Cyc_Interface_incl_vardecl,({const char*_tmp13E="variable";
_tag_dyneither(_tmp13E,sizeof(char),9);}),msg);int _tmp13C=((int(*)(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Tcdecl_Xdatatypefielddecl*,
struct Cyc_Tcdecl_Xdatatypefielddecl*,struct _dyneither_ptr*),struct _dyneither_ptr
t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->xdatatypefielddecls,
ie2->xdatatypefielddecls,Cyc_Interface_incl_xdatatypefielddecl,({const char*
_tmp13D="xdatatypefield";_tag_dyneither(_tmp13D,sizeof(char),15);}),msg);return((((
_tmp137  && _tmp138) && _tmp139) && _tmp13A) && _tmp13B) && _tmp13C;}struct
_tuple21{int f1;struct Cyc_Dict_Dict f2;struct _dyneither_ptr*f3;};static void Cyc_Interface_incl_ns_f(
struct _tuple21*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie1){struct
_tuple21 _tmp144;int _tmp145;int*_tmp146;struct Cyc_Dict_Dict _tmp147;struct
_dyneither_ptr*_tmp148;struct _tuple21*_tmp143=env;_tmp144=*_tmp143;_tmp145=
_tmp144.f1;_tmp146=(int*)&(*_tmp143).f1;_tmp147=_tmp144.f2;_tmp148=_tmp144.f3;{
struct Cyc_Interface_Ienv*ie2;{struct _handler_cons _tmp149;_push_handler(& _tmp149);{
int _tmp14B=0;if(setjmp(_tmp149.handler))_tmp14B=1;if(!_tmp14B){ie2=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp147,ns);;
_pop_handler();}else{void*_tmp14A=(void*)_exn_thrown;void*_tmp14D=_tmp14A;_LL8E:
if(_tmp14D != Cyc_Dict_Absent)goto _LL90;_LL8F: ie2=Cyc_Interface_lazy_new_ienv();
goto _LL8D;_LL90:;_LL91:(void)_throw(_tmp14D);_LL8D:;}}}if(!Cyc_Interface_incl_ienv(
ie1,ie2,_tmp148))*_tmp146=0;}}static int Cyc_Interface_incl_ns(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr*msg){struct _tuple21 _tmp14E=({
struct _tuple21 _tmp14F;_tmp14F.f1=1;_tmp14F.f2=dic2;_tmp14F.f3=msg;_tmp14F;});((
void(*)(void(*f)(struct _tuple21*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_incl_ns_f,&
_tmp14E,dic1);return _tmp14E.f1;}int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*
i1,struct Cyc_Interface_I*i2,struct _tuple10*info){struct _handler_cons _tmp150;
_push_handler(& _tmp150);{int _tmp152=0;if(setjmp(_tmp150.handler))_tmp152=1;if(!
_tmp152){{int _tmp153=1;struct _dyneither_ptr*msg=0;if(info != 0)msg=({struct
_dyneither_ptr*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154[0]=(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp158;_tmp158.tag=0;_tmp158.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2);{struct Cyc_String_pa_struct
_tmp157;_tmp157.tag=0;_tmp157.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*
info).f1);{void*_tmp155[2]={& _tmp157,& _tmp158};Cyc_aprintf(({const char*_tmp156="checking inclusion of %s exports into %s exports,";
_tag_dyneither(_tmp156,sizeof(char),50);}),_tag_dyneither(_tmp155,sizeof(void*),
2));}}});_tmp154;});if(!Cyc_Interface_incl_ns(i1->exports,i2->exports,msg))
_tmp153=0;if(info != 0)msg=({struct _dyneither_ptr*_tmp159=_cycalloc(sizeof(*
_tmp159));_tmp159[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp15D;
_tmp15D.tag=0;_tmp15D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1);{
struct Cyc_String_pa_struct _tmp15C;_tmp15C.tag=0;_tmp15C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)(*info).f2);{void*_tmp15A[2]={& _tmp15C,& _tmp15D};Cyc_aprintf(({
const char*_tmp15B="checking inclusion of %s imports into %s imports,";
_tag_dyneither(_tmp15B,sizeof(char),50);}),_tag_dyneither(_tmp15A,sizeof(void*),
2));}}});_tmp159;});if(!Cyc_Interface_incl_ns(i2->imports,i1->imports,msg))
_tmp153=0;{int _tmp15E=_tmp153;_npop_handler(0);return _tmp15E;}};_pop_handler();}
else{void*_tmp151=(void*)_exn_thrown;void*_tmp160=_tmp151;_LL93: if(_tmp160 != Cyc_Tcdecl_Incompatible)
goto _LL95;_LL94: return 0;_LL95:;_LL96:(void)_throw(_tmp160);_LL92:;}}}struct
_tuple22{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct Cyc_Dict_Dict f4;
void*(*f5)(void*,void*,struct Cyc_Position_Segment*,struct _dyneither_ptr*);struct
_dyneither_ptr f6;struct _dyneither_ptr*f7;};void Cyc_Interface_compat_merge_dict_f(
struct _tuple22*env,struct _dyneither_ptr*x,void*y2){struct _tuple22 _tmp162;int
_tmp163;int*_tmp164;struct Cyc_Dict_Dict _tmp165;struct Cyc_Dict_Dict*_tmp166;
struct Cyc_Dict_Dict _tmp167;struct Cyc_Dict_Dict _tmp168;void*(*_tmp169)(void*,void*,
struct Cyc_Position_Segment*,struct _dyneither_ptr*);struct _dyneither_ptr _tmp16A;
struct _dyneither_ptr*_tmp16B;struct _tuple22*_tmp161=env;_tmp162=*_tmp161;_tmp163=
_tmp162.f1;_tmp164=(int*)&(*_tmp161).f1;_tmp165=_tmp162.f2;_tmp166=(struct Cyc_Dict_Dict*)&(*
_tmp161).f2;_tmp167=_tmp162.f3;_tmp168=_tmp162.f4;_tmp169=_tmp162.f5;_tmp16A=
_tmp162.f6;_tmp16B=_tmp162.f7;{void*y;{struct _handler_cons _tmp16C;_push_handler(&
_tmp16C);{int _tmp16E=0;if(setjmp(_tmp16C.handler))_tmp16E=1;if(!_tmp16E){{void*
_tmp16F=((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp167,x);void*_tmp170=_tmp169(_tmp16F,y2,0,_tmp16B);if(!((unsigned int)_tmp170)){*
_tmp164=0;_npop_handler(0);return;}y=(void*)_tmp170;};_pop_handler();}else{void*
_tmp16D=(void*)_exn_thrown;void*_tmp172=_tmp16D;_LL98: if(_tmp172 != Cyc_Dict_Absent)
goto _LL9A;_LL99: y=y2;goto _LL97;_LL9A:;_LL9B:(void)_throw(_tmp172);_LL97:;}}}{
struct _handler_cons _tmp173;_push_handler(& _tmp173);{int _tmp175=0;if(setjmp(
_tmp173.handler))_tmp175=1;if(!_tmp175){{void*_tmp176=((void*(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp168,x);void*_tmp177=_tmp169(
_tmp176,y,0,_tmp16B);if(_tmp177 != (void*)_tmp176){if((unsigned int)_tmp177)Cyc_Interface_check_err(
_tmp16B,(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp17B;_tmp17B.tag=0;
_tmp17B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*x);{struct Cyc_String_pa_struct
_tmp17A;_tmp17A.tag=0;_tmp17A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp16A);{void*_tmp178[2]={& _tmp17A,& _tmp17B};Cyc_aprintf(({const char*_tmp179="abstract %s %s is being imported as non-abstract";
_tag_dyneither(_tmp179,sizeof(char),49);}),_tag_dyneither(_tmp178,sizeof(void*),
2));}}}));*_tmp164=0;}};_pop_handler();}else{void*_tmp174=(void*)_exn_thrown;
void*_tmp17D=_tmp174;_LL9D: if(_tmp17D != Cyc_Dict_Absent)goto _LL9F;_LL9E: if(*
_tmp164)*_tmp166=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,void*v))Cyc_Dict_insert)(*_tmp166,x,y);goto _LL9C;_LL9F:;_LLA0:(
void)_throw(_tmp17D);_LL9C:;}}}}}struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*
merge_f)(void*,void*,struct Cyc_Position_Segment*,struct _dyneither_ptr*),struct
_dyneither_ptr t,struct _dyneither_ptr*msg){struct _tuple22 _tmp17E=({struct _tuple22
_tmp180;_tmp180.f1=1;_tmp180.f2=dic1;_tmp180.f3=dic1;_tmp180.f4=excl;_tmp180.f5=
merge_f;_tmp180.f6=t;_tmp180.f7=msg;_tmp180;});((void(*)(void(*f)(struct _tuple22*,
struct _dyneither_ptr*,void*),struct _tuple22*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_Interface_compat_merge_dict_f,& _tmp17E,dic2);if(_tmp17E.f1)return({struct Cyc_Dict_Dict*
_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F[0]=_tmp17E.f2;_tmp17F;});return 0;}
struct Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(struct Cyc_Interface_Ienv*
ie1,struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*iexcl,struct
_dyneither_ptr*msg){struct Cyc_Dict_Dict*_tmp181=((struct Cyc_Dict_Dict*(*)(struct
Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Aggrdecl*(*
merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->aggrdecls,ie2->aggrdecls,iexcl->aggrdecls,Cyc_Tcdecl_merge_aggrdecl,({const
char*_tmp18D="type";_tag_dyneither(_tmp18D,sizeof(char),5);}),msg);struct Cyc_Dict_Dict*
_tmp182=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Datatypedecl*(*merge_f)(struct Cyc_Absyn_Datatypedecl*,
struct Cyc_Absyn_Datatypedecl*,struct Cyc_Position_Segment*,struct _dyneither_ptr*),
struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->datatypedecls,ie2->datatypedecls,iexcl->datatypedecls,Cyc_Tcdecl_merge_datatypedecl,({
const char*_tmp18C="datatype";_tag_dyneither(_tmp18C,sizeof(char),9);}),msg);
struct Cyc_Dict_Dict*_tmp183=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,
struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Enumdecl*(*
merge_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->enumdecls,ie2->enumdecls,iexcl->enumdecls,Cyc_Tcdecl_merge_enumdecl,({const
char*_tmp18B="enum";_tag_dyneither(_tmp18B,sizeof(char),5);}),msg);struct Cyc_Dict_Dict*
_tmp184=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,struct _dyneither_ptr*),
struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls,ie2->typedefdecls,iexcl->typedefdecls,Cyc_Tcdecl_merge_typedefdecl,({
const char*_tmp18A="typedef";_tag_dyneither(_tmp18A,sizeof(char),8);}),msg);
struct Cyc_Dict_Dict*_tmp185=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,
struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Vardecl*(*
merge_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->vardecls,ie2->vardecls,iexcl->vardecls,Cyc_Tcdecl_merge_vardecl,({const char*
_tmp189="variable";_tag_dyneither(_tmp189,sizeof(char),9);}),msg);struct Cyc_Dict_Dict*
_tmp186=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct Cyc_Dict_Dict excl,struct Cyc_Tcdecl_Xdatatypefielddecl*(*merge_f)(
struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct
Cyc_Position_Segment*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct
_dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->xdatatypefielddecls,
ie2->xdatatypefielddecls,iexcl->xdatatypefielddecls,Cyc_Tcdecl_merge_xdatatypefielddecl,({
const char*_tmp188="xdatatypefield";_tag_dyneither(_tmp188,sizeof(char),15);}),
msg);if(((((!((unsigned int)_tmp181) || !((unsigned int)_tmp182)) || !((
unsigned int)_tmp183)) || !((unsigned int)_tmp184)) || !((unsigned int)_tmp185))
 || !((unsigned int)_tmp186))return 0;return({struct Cyc_Interface_Ienv*_tmp187=
_cycalloc(sizeof(*_tmp187));_tmp187->aggrdecls=*_tmp181;_tmp187->datatypedecls=*
_tmp182;_tmp187->enumdecls=*_tmp183;_tmp187->typedefdecls=*_tmp184;_tmp187->vardecls=*
_tmp185;_tmp187->xdatatypefielddecls=*_tmp186;_tmp187;});}struct _tuple23{int f1;
struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct Cyc_Dict_Dict f4;struct
_dyneither_ptr*f5;};void Cyc_Interface_compat_merge_ns_f(struct _tuple23*env,
struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2){struct _tuple23 _tmp18F;int
_tmp190;int*_tmp191;struct Cyc_Dict_Dict _tmp192;struct Cyc_Dict_Dict*_tmp193;
struct Cyc_Dict_Dict _tmp194;struct Cyc_Dict_Dict _tmp195;struct _dyneither_ptr*
_tmp196;struct _tuple23*_tmp18E=env;_tmp18F=*_tmp18E;_tmp190=_tmp18F.f1;_tmp191=(
int*)&(*_tmp18E).f1;_tmp192=_tmp18F.f2;_tmp193=(struct Cyc_Dict_Dict*)&(*_tmp18E).f2;
_tmp194=_tmp18F.f3;_tmp195=_tmp18F.f4;_tmp196=_tmp18F.f5;{struct Cyc_Interface_Ienv*
ie1;struct Cyc_Interface_Ienv*iexcl;{struct _handler_cons _tmp197;_push_handler(&
_tmp197);{int _tmp199=0;if(setjmp(_tmp197.handler))_tmp199=1;if(!_tmp199){iexcl=((
struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
_tmp195,ns);;_pop_handler();}else{void*_tmp198=(void*)_exn_thrown;void*_tmp19B=
_tmp198;_LLA2: if(_tmp19B != Cyc_Dict_Absent)goto _LLA4;_LLA3: iexcl=Cyc_Interface_lazy_new_ienv();
goto _LLA1;_LLA4:;_LLA5:(void)_throw(_tmp19B);_LLA1:;}}}{struct _handler_cons
_tmp19C;_push_handler(& _tmp19C);{int _tmp19E=0;if(setjmp(_tmp19C.handler))_tmp19E=
1;if(!_tmp19E){ie1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(_tmp194,ns);{struct Cyc_Interface_Ienv*_tmp19F=Cyc_Interface_compat_merge_ienv(
Cyc_Interface_lazy_new_ienv(),ie1,iexcl,_tmp196);if(_tmp19F == 0){*_tmp191=0;
_npop_handler(0);return;}ie1=(struct Cyc_Interface_Ienv*)_tmp19F;};_pop_handler();}
else{void*_tmp19D=(void*)_exn_thrown;void*_tmp1A1=_tmp19D;_LLA7: if(_tmp1A1 != Cyc_Dict_Absent)
goto _LLA9;_LLA8: ie1=Cyc_Interface_lazy_new_ienv();goto _LLA6;_LLA9:;_LLAA:(void)
_throw(_tmp1A1);_LLA6:;}}}{struct Cyc_Interface_Ienv*_tmp1A2=Cyc_Interface_compat_merge_ienv(
ie1,ie2,iexcl,_tmp196);if(_tmp1A2 == 0)*_tmp191=0;else{if(*_tmp191)*_tmp193=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*
v))Cyc_Dict_insert)(*_tmp193,ns,(struct Cyc_Interface_Ienv*)_tmp1A2);}}}}struct
Cyc_Dict_Dict*Cyc_Interface_compat_merge_ns(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct Cyc_Dict_Dict excl,struct _dyneither_ptr*msg){struct Cyc_Dict_Dict d=((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(
Cyc_Absyn_varlist_cmp);struct _tuple23 _tmp1A3=({struct _tuple23 _tmp1A5;_tmp1A5.f1=
1;_tmp1A5.f2=d;_tmp1A5.f3=dic1;_tmp1A5.f4=excl;_tmp1A5.f5=msg;_tmp1A5;});((void(*)(
void(*f)(struct _tuple23*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct
_tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_ns_f,&
_tmp1A3,dic2);if(_tmp1A3.f1)return({struct Cyc_Dict_Dict*_tmp1A4=_cycalloc(
sizeof(*_tmp1A4));_tmp1A4[0]=_tmp1A3.f2;_tmp1A4;});return 0;}struct _tuple24{int f1;
struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct _dyneither_ptr f4;struct
_dyneither_ptr*f5;};void Cyc_Interface_disj_merge_dict_f(struct _tuple24*env,
struct _dyneither_ptr*x,void*y){struct _tuple24 _tmp1A7;int _tmp1A8;int*_tmp1A9;
struct Cyc_Dict_Dict _tmp1AA;struct Cyc_Dict_Dict*_tmp1AB;struct Cyc_Dict_Dict
_tmp1AC;struct _dyneither_ptr _tmp1AD;struct _dyneither_ptr*_tmp1AE;struct _tuple24*
_tmp1A6=env;_tmp1A7=*_tmp1A6;_tmp1A8=_tmp1A7.f1;_tmp1A9=(int*)&(*_tmp1A6).f1;
_tmp1AA=_tmp1A7.f2;_tmp1AB=(struct Cyc_Dict_Dict*)&(*_tmp1A6).f2;_tmp1AC=_tmp1A7.f3;
_tmp1AD=_tmp1A7.f4;_tmp1AE=_tmp1A7.f5;if(((int(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_member)(_tmp1AC,x)){Cyc_Interface_check_err(_tmp1AE,(
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp1B2;_tmp1B2.tag=0;_tmp1B2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*x);{struct Cyc_String_pa_struct
_tmp1B1;_tmp1B1.tag=0;_tmp1B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp1AD);{void*_tmp1AF[2]={& _tmp1B1,& _tmp1B2};Cyc_aprintf(({const char*_tmp1B0="%s %s is exported more than once";
_tag_dyneither(_tmp1B0,sizeof(char),33);}),_tag_dyneither(_tmp1AF,sizeof(void*),
2));}}}));*_tmp1A9=0;}else{if(*_tmp1A9)*_tmp1AB=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*_tmp1AB,x,y);}}
struct Cyc_Dict_Dict*Cyc_Interface_disj_merge_dict(struct Cyc_Dict_Dict dic1,struct
Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg){struct _tuple24
_tmp1B3=({struct _tuple24 _tmp1B5;_tmp1B5.f1=1;_tmp1B5.f2=dic1;_tmp1B5.f3=dic1;
_tmp1B5.f4=t;_tmp1B5.f5=msg;_tmp1B5;});((void(*)(void(*f)(struct _tuple24*,struct
_dyneither_ptr*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_Interface_disj_merge_dict_f,& _tmp1B3,dic2);if(_tmp1B3.f1)return({struct Cyc_Dict_Dict*
_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4[0]=_tmp1B3.f2;_tmp1B4;});return 0;}
struct Cyc_Interface_Ienv*Cyc_Interface_disj_merge_ienv(struct Cyc_Interface_Ienv*
ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*msg){struct Cyc_Dict_Dict*
_tmp1B6=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,
struct Cyc_Absyn_Aggrdecl*,struct Cyc_Position_Segment*,struct _dyneither_ptr*),
struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->aggrdecls,ie2->aggrdecls,((struct Cyc_Dict_Dict(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_aggrdecl,({
const char*_tmp1C2="type";_tag_dyneither(_tmp1C2,sizeof(char),5);}),msg);struct
Cyc_Dict_Dict*_tmp1B7=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct
Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(
ie1->datatypedecls,ie2->datatypedecls,({const char*_tmp1C1="datatype";
_tag_dyneither(_tmp1C1,sizeof(char),9);}),msg);struct Cyc_Dict_Dict*_tmp1B8=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct
_dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->enumdecls,
ie2->enumdecls,({const char*_tmp1C0="enum";_tag_dyneither(_tmp1C0,sizeof(char),5);}),
msg);struct Cyc_Dict_Dict*_tmp1B9=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*
merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls,ie2->typedefdecls,((struct Cyc_Dict_Dict(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_typedefdecl,({
const char*_tmp1BF="typedef";_tag_dyneither(_tmp1BF,sizeof(char),8);}),msg);
struct Cyc_Dict_Dict*_tmp1BA=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,
struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(
ie1->vardecls,ie2->vardecls,({const char*_tmp1BE="variable";_tag_dyneither(
_tmp1BE,sizeof(char),9);}),msg);struct Cyc_Dict_Dict*_tmp1BB=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct
_dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,({
const char*_tmp1BD="xdatatypefield";_tag_dyneither(_tmp1BD,sizeof(char),15);}),
msg);if(((((!((unsigned int)_tmp1B6) || !((unsigned int)_tmp1B7)) || !((
unsigned int)_tmp1B8)) || !((unsigned int)_tmp1B9)) || !((unsigned int)_tmp1BA))
 || !((unsigned int)_tmp1BB))return 0;return({struct Cyc_Interface_Ienv*_tmp1BC=
_cycalloc(sizeof(*_tmp1BC));_tmp1BC->aggrdecls=*_tmp1B6;_tmp1BC->datatypedecls=*
_tmp1B7;_tmp1BC->enumdecls=*_tmp1B8;_tmp1BC->typedefdecls=*_tmp1B9;_tmp1BC->vardecls=*
_tmp1BA;_tmp1BC->xdatatypefielddecls=*_tmp1BB;_tmp1BC;});}struct _tuple25{int f1;
struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct _dyneither_ptr*f4;};void Cyc_Interface_disj_merge_ns_f(
struct _tuple25*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2){struct
_tuple25 _tmp1C4;int _tmp1C5;int*_tmp1C6;struct Cyc_Dict_Dict _tmp1C7;struct Cyc_Dict_Dict*
_tmp1C8;struct Cyc_Dict_Dict _tmp1C9;struct _dyneither_ptr*_tmp1CA;struct _tuple25*
_tmp1C3=env;_tmp1C4=*_tmp1C3;_tmp1C5=_tmp1C4.f1;_tmp1C6=(int*)&(*_tmp1C3).f1;
_tmp1C7=_tmp1C4.f2;_tmp1C8=(struct Cyc_Dict_Dict*)&(*_tmp1C3).f2;_tmp1C9=_tmp1C4.f3;
_tmp1CA=_tmp1C4.f4;{struct Cyc_Interface_Ienv*ie1;{struct _handler_cons _tmp1CB;
_push_handler(& _tmp1CB);{int _tmp1CD=0;if(setjmp(_tmp1CB.handler))_tmp1CD=1;if(!
_tmp1CD){ie1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(_tmp1C9,ns);;_pop_handler();}else{void*_tmp1CC=(void*)
_exn_thrown;void*_tmp1CF=_tmp1CC;_LLAC: if(_tmp1CF != Cyc_Dict_Absent)goto _LLAE;
_LLAD: ie1=Cyc_Interface_lazy_new_ienv();goto _LLAB;_LLAE:;_LLAF:(void)_throw(
_tmp1CF);_LLAB:;}}}{struct Cyc_Interface_Ienv*_tmp1D0=Cyc_Interface_disj_merge_ienv(
ie1,ie2,_tmp1CA);if(_tmp1D0 == 0)*_tmp1C6=0;else{if(*_tmp1C6)*_tmp1C8=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*
_tmp1C8,ns,(struct Cyc_Interface_Ienv*)_tmp1D0);}}}}struct Cyc_Dict_Dict*Cyc_Interface_disj_merge_ns(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr*msg){struct
_tuple25 _tmp1D1=({struct _tuple25 _tmp1D3;_tmp1D3.f1=1;_tmp1D3.f2=dic1;_tmp1D3.f3=
dic1;_tmp1D3.f4=msg;_tmp1D3;});((void(*)(void(*f)(struct _tuple25*,struct Cyc_List_List*,
struct Cyc_Interface_Ienv*),struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_Interface_disj_merge_ns_f,& _tmp1D1,dic2);if(_tmp1D1.f1)return({struct Cyc_Dict_Dict*
_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2[0]=_tmp1D1.f2;_tmp1D2;});return 0;}
struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*
i2,struct _tuple10*info){struct _handler_cons _tmp1D4;_push_handler(& _tmp1D4);{int
_tmp1D6=0;if(setjmp(_tmp1D4.handler))_tmp1D6=1;if(!_tmp1D6){{struct
_dyneither_ptr*msg=0;if(info != 0)msg=({struct _dyneither_ptr*_tmp1D7=_cycalloc(
sizeof(*_tmp1D7));_tmp1D7[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp1DB;_tmp1DB.tag=0;_tmp1DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*
info).f2);{struct Cyc_String_pa_struct _tmp1DA;_tmp1DA.tag=0;_tmp1DA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)(*info).f1);{void*_tmp1D8[2]={& _tmp1DA,&
_tmp1DB};Cyc_aprintf(({const char*_tmp1D9="merging exports of %s and %s,";
_tag_dyneither(_tmp1D9,sizeof(char),30);}),_tag_dyneither(_tmp1D8,sizeof(void*),
2));}}});_tmp1D7;});{struct Cyc_Dict_Dict*_tmp1DC=Cyc_Interface_disj_merge_ns(i1->exports,
i2->exports,msg);if(_tmp1DC == 0){struct Cyc_Interface_I*_tmp1DD=0;_npop_handler(0);
return _tmp1DD;}if(info != 0)msg=({struct _dyneither_ptr*_tmp1DE=_cycalloc(sizeof(*
_tmp1DE));_tmp1DE[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp1E2;
_tmp1E2.tag=0;_tmp1E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2);{
struct Cyc_String_pa_struct _tmp1E1;_tmp1E1.tag=0;_tmp1E1.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)(*info).f1);{void*_tmp1DF[2]={& _tmp1E1,& _tmp1E2};Cyc_aprintf(({
const char*_tmp1E0="merging imports of %s and %s,";_tag_dyneither(_tmp1E0,sizeof(
char),30);}),_tag_dyneither(_tmp1DF,sizeof(void*),2));}}});_tmp1DE;});{struct Cyc_Dict_Dict*
_tmp1E3=Cyc_Interface_compat_merge_ns(i1->imports,i2->imports,*_tmp1DC,msg);if(
_tmp1E3 == 0){struct Cyc_Interface_I*_tmp1E4=0;_npop_handler(0);return _tmp1E4;}{
struct Cyc_Interface_I*_tmp1E6=({struct Cyc_Interface_I*_tmp1E5=_cycalloc(sizeof(*
_tmp1E5));_tmp1E5->imports=*_tmp1E3;_tmp1E5->exports=*_tmp1DC;_tmp1E5;});
_npop_handler(0);return _tmp1E6;}}}};_pop_handler();}else{void*_tmp1D5=(void*)
_exn_thrown;void*_tmp1E8=_tmp1D5;_LLB1: if(_tmp1E8 != Cyc_Tcdecl_Incompatible)goto
_LLB3;_LLB2: return 0;_LLB3:;_LLB4:(void)_throw(_tmp1E8);_LLB0:;}}}struct Cyc_Interface_I*
Cyc_Interface_merge_list(struct Cyc_List_List*li,struct Cyc_List_List*linfo){if(li
== 0)return(struct Cyc_Interface_I*)Cyc_Interface_empty();{struct Cyc_Interface_I*
curr_i=(struct Cyc_Interface_I*)li->hd;struct _dyneither_ptr*curr_info=linfo != 0?(
struct _dyneither_ptr*)((struct _dyneither_ptr*)linfo->hd): 0;li=li->tl;if(linfo != 
0)linfo=linfo->tl;for(0;li != 0;li=li->tl){struct Cyc_Interface_I*_tmp1E9=Cyc_Interface_merge(
curr_i,(struct Cyc_Interface_I*)li->hd,curr_info != 0  && linfo != 0?({struct
_tuple10*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF->f1=*curr_info;_tmp1EF->f2=*((
struct _dyneither_ptr*)linfo->hd);_tmp1EF;}): 0);if(_tmp1E9 == 0)return 0;curr_i=(
struct Cyc_Interface_I*)_tmp1E9;if(curr_info != 0)curr_info=linfo != 0?({struct
_dyneither_ptr*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA[0]=(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp1EE;_tmp1EE.tag=0;_tmp1EE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)linfo->hd));{
struct Cyc_String_pa_struct _tmp1ED;_tmp1ED.tag=0;_tmp1ED.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*curr_info);{void*_tmp1EB[2]={& _tmp1ED,& _tmp1EE};Cyc_aprintf(({
const char*_tmp1EC="%s+%s";_tag_dyneither(_tmp1EC,sizeof(char),6);}),
_tag_dyneither(_tmp1EB,sizeof(void*),2));}}});_tmp1EA;}): 0;if(linfo != 0)linfo=
linfo->tl;}return(struct Cyc_Interface_I*)curr_i;}}struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(
struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*
linfo){if(la == 0)return(struct Cyc_Interface_I*)Cyc_Interface_empty();{struct Cyc_Interface_I*
curr_i=get((void*)la->hd);struct _dyneither_ptr*curr_info=linfo != 0?(struct
_dyneither_ptr*)((struct _dyneither_ptr*)linfo->hd): 0;la=la->tl;if(linfo != 0)
linfo=linfo->tl;for(0;la != 0;la=la->tl){struct Cyc_Interface_I*_tmp1F0=Cyc_Interface_merge(
curr_i,get((void*)la->hd),curr_info != 0  && linfo != 0?({struct _tuple10*_tmp1F6=
_cycalloc(sizeof(*_tmp1F6));_tmp1F6->f1=*curr_info;_tmp1F6->f2=*((struct
_dyneither_ptr*)linfo->hd);_tmp1F6;}): 0);if(_tmp1F0 == 0)return 0;curr_i=(struct
Cyc_Interface_I*)_tmp1F0;if(curr_info != 0)curr_info=linfo != 0?({struct
_dyneither_ptr*_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1[0]=(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp1F5;_tmp1F5.tag=0;_tmp1F5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)linfo->hd));{
struct Cyc_String_pa_struct _tmp1F4;_tmp1F4.tag=0;_tmp1F4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*curr_info);{void*_tmp1F2[2]={& _tmp1F4,& _tmp1F5};Cyc_aprintf(({
const char*_tmp1F3="%s+%s";_tag_dyneither(_tmp1F3,sizeof(char),6);}),
_tag_dyneither(_tmp1F2,sizeof(void*),2));}}});_tmp1F1;}): 0;if(linfo != 0)linfo=
linfo->tl;}return(struct Cyc_Interface_I*)curr_i;}}static struct Cyc_List_List*Cyc_Interface_add_namespace(
struct Cyc_List_List*tds,struct Cyc_List_List*ns){if(ns == 0)return tds;return({
struct Cyc_List_List*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));_tmp1F7->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp1F8=_cycalloc(sizeof(*_tmp1F8));
_tmp1F8[0]=({struct Cyc_Absyn_Namespace_d_struct _tmp1F9;_tmp1F9.tag=10;_tmp1F9.f1=(
struct _dyneither_ptr*)ns->hd;_tmp1F9.f2=Cyc_Interface_add_namespace(tds,ns->tl);
_tmp1F9;});_tmp1F8;}),0);_tmp1F7->tl=0;_tmp1F7;});}static struct Cyc_List_List*Cyc_Interface_add_aggrdecl(
struct _dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp1FA=_cycalloc(sizeof(*_tmp1FA));_tmp1FA->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp1FB=_cycalloc(sizeof(*_tmp1FB));
_tmp1FB[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp1FC;_tmp1FC.tag=6;_tmp1FC.f1=d;
_tmp1FC;});_tmp1FB;}),0);_tmp1FA->tl=tds;_tmp1FA;});}static struct Cyc_List_List*
Cyc_Interface_add_aggrdecl_header(struct _dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl*
d,struct Cyc_List_List*tds){d=({struct Cyc_Absyn_Aggrdecl*_tmp1FD=_cycalloc(
sizeof(*_tmp1FD));_tmp1FD[0]=*d;_tmp1FD;});d->impl=0;if(d->sc != (void*)4)d->sc=(
void*)3;return({struct Cyc_List_List*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));_tmp1FE->hd=
Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp1FF=_cycalloc(
sizeof(*_tmp1FF));_tmp1FF[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp200;_tmp200.tag=
6;_tmp200.f1=d;_tmp200;});_tmp1FF;}),0);_tmp1FE->tl=tds;_tmp1FE;});}static struct
Cyc_List_List*Cyc_Interface_add_datatypedecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl*
d,struct Cyc_List_List*tds){return({struct Cyc_List_List*_tmp201=_cycalloc(sizeof(*
_tmp201));_tmp201->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_struct*
_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202[0]=({struct Cyc_Absyn_Datatype_d_struct
_tmp203;_tmp203.tag=7;_tmp203.f1=d;_tmp203;});_tmp202;}),0);_tmp201->tl=tds;
_tmp201;});}static char _tmp204[2]="_";static struct _dyneither_ptr Cyc_Interface_us={
_tmp204,_tmp204,_tmp204 + 2};static struct _dyneither_ptr*Cyc_Interface_us_p=& Cyc_Interface_us;
static struct _tuple17*Cyc_Interface_rewrite_datatypefield_type(struct _tuple17*x){
return({struct _tuple17*_tmp205=_cycalloc(sizeof(*_tmp205));_tmp205->f1=Cyc_Absyn_empty_tqual(
0);_tmp205->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp206=_cycalloc(sizeof(*
_tmp206));_tmp206[0]=({struct Cyc_Absyn_VarType_struct _tmp207;_tmp207.tag=1;
_tmp207.f1=({struct Cyc_Absyn_Tvar*_tmp208=_cycalloc(sizeof(*_tmp208));_tmp208->name=
Cyc_Interface_us_p;_tmp208->identity=- 1;_tmp208->kind=Cyc_Tcutil_kind_to_bound((
void*)2);_tmp208;});_tmp207;});_tmp206;});_tmp205;});}static struct Cyc_Absyn_Datatypefield*
Cyc_Interface_rewrite_datatypefield(struct Cyc_Absyn_Datatypefield*f){f=({struct
Cyc_Absyn_Datatypefield*_tmp209=_cycalloc(sizeof(*_tmp209));_tmp209[0]=*f;
_tmp209;});f->typs=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct _tuple17*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield_type,f->typs);
return f;}static struct Cyc_List_List*Cyc_Interface_add_datatypedecl_header(struct
_dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){d=({
struct Cyc_Absyn_Datatypedecl*_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A[0]=*d;
_tmp20A;});if(d->fields != 0)d->fields=({struct Cyc_Core_Opt*_tmp20B=_cycalloc(
sizeof(*_tmp20B));_tmp20B->v=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Datatypefield*(*
f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield,(
struct Cyc_List_List*)(d->fields)->v);_tmp20B;});if(d->sc != (void*)4)d->sc=(void*)
3;return({struct Cyc_List_List*_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C->hd=
Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_struct*_tmp20D=_cycalloc(
sizeof(*_tmp20D));_tmp20D[0]=({struct Cyc_Absyn_Datatype_d_struct _tmp20E;_tmp20E.tag=
7;_tmp20E.f1=d;_tmp20E;});_tmp20D;}),0);_tmp20C->tl=tds;_tmp20C;});}static struct
Cyc_List_List*Cyc_Interface_add_enumdecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Enumdecl*
d,struct Cyc_List_List*tds){return({struct Cyc_List_List*_tmp20F=_cycalloc(sizeof(*
_tmp20F));_tmp20F->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp211;_tmp211.tag=8;_tmp211.f1=d;_tmp211;});_tmp210;}),0);_tmp20F->tl=tds;
_tmp20F;});}static struct Cyc_List_List*Cyc_Interface_add_enumdecl_header(struct
_dyneither_ptr*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){d=({struct
Cyc_Absyn_Enumdecl*_tmp212=_cycalloc(sizeof(*_tmp212));_tmp212[0]=*d;_tmp212;});
d->fields=0;if(d->sc != (void*)4)d->sc=(void*)3;return({struct Cyc_List_List*
_tmp213=_cycalloc(sizeof(*_tmp213));_tmp213->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Enum_d_struct*_tmp214=_cycalloc(sizeof(*_tmp214));_tmp214[0]=({
struct Cyc_Absyn_Enum_d_struct _tmp215;_tmp215.tag=8;_tmp215.f1=d;_tmp215;});
_tmp214;}),0);_tmp213->tl=tds;_tmp213;});}static struct Cyc_List_List*Cyc_Interface_add_typedef(
struct _dyneither_ptr*x,struct Cyc_Absyn_Typedefdecl*d,struct Cyc_List_List*tds){
return({struct Cyc_List_List*_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Typedef_d_struct*_tmp217=_cycalloc(sizeof(*_tmp217));
_tmp217[0]=({struct Cyc_Absyn_Typedef_d_struct _tmp218;_tmp218.tag=9;_tmp218.f1=d;
_tmp218;});_tmp217;}),0);_tmp216->tl=tds;_tmp216;});}static struct Cyc_List_List*
Cyc_Interface_add_vardecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Vardecl*d,
struct Cyc_List_List*tds){return({struct Cyc_List_List*_tmp219=_cycalloc(sizeof(*
_tmp219));_tmp219->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp21A=_cycalloc(sizeof(*_tmp21A));_tmp21A[0]=({struct Cyc_Absyn_Var_d_struct
_tmp21B;_tmp21B.tag=0;_tmp21B.f1=d;_tmp21B;});_tmp21A;}),0);_tmp219->tl=tds;
_tmp219;});}static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl(
struct _dyneither_ptr*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*
tds){struct Cyc_Tcdecl_Xdatatypefielddecl _tmp21D;struct Cyc_Absyn_Datatypedecl*
_tmp21E;struct Cyc_Absyn_Datatypefield*_tmp21F;struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp21C=d;_tmp21D=*_tmp21C;_tmp21E=_tmp21D.base;_tmp21F=_tmp21D.field;_tmp21E=({
struct Cyc_Absyn_Datatypedecl*_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220[0]=*
_tmp21E;_tmp220;});_tmp21E->fields=({struct Cyc_Core_Opt*_tmp221=_cycalloc(
sizeof(*_tmp221));_tmp221->v=({struct Cyc_List_List*_tmp222=_cycalloc(sizeof(*
_tmp222));_tmp222->hd=_tmp21F;_tmp222->tl=0;_tmp222;});_tmp221;});_tmp21E->sc=(
void*)3;return({struct Cyc_List_List*_tmp223=_cycalloc(sizeof(*_tmp223));_tmp223->hd=
Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_struct*_tmp224=_cycalloc(
sizeof(*_tmp224));_tmp224[0]=({struct Cyc_Absyn_Datatype_d_struct _tmp225;_tmp225.tag=
7;_tmp225.f1=_tmp21E;_tmp225;});_tmp224;}),0);_tmp223->tl=tds;_tmp223;});}static
struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl_header(struct
_dyneither_ptr*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds){
struct Cyc_Tcdecl_Xdatatypefielddecl _tmp227;struct Cyc_Absyn_Datatypedecl*_tmp228;
struct Cyc_Absyn_Datatypefield*_tmp229;struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp226=d;_tmp227=*_tmp226;_tmp228=_tmp227.base;_tmp229=_tmp227.field;_tmp228=({
struct Cyc_Absyn_Datatypedecl*_tmp22A=_cycalloc(sizeof(*_tmp22A));_tmp22A[0]=*
_tmp228;_tmp22A;});_tmp229=Cyc_Interface_rewrite_datatypefield(_tmp229);_tmp229->sc=(
void*)3;_tmp228->fields=({struct Cyc_Core_Opt*_tmp22B=_cycalloc(sizeof(*_tmp22B));
_tmp22B->v=({struct Cyc_List_List*_tmp22C=_cycalloc(sizeof(*_tmp22C));_tmp22C->hd=
_tmp229;_tmp22C->tl=0;_tmp22C;});_tmp22B;});_tmp228->sc=(void*)3;return({struct
Cyc_List_List*_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Datatype_d_struct*_tmp22E=_cycalloc(sizeof(*_tmp22E));
_tmp22E[0]=({struct Cyc_Absyn_Datatype_d_struct _tmp22F;_tmp22F.tag=7;_tmp22F.f1=
_tmp228;_tmp22F;});_tmp22E;}),0);_tmp22D->tl=tds;_tmp22D;});}static void Cyc_Interface_print_ns_headers(
struct Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct
Cyc_List_List*_tmp230=0;_tmp230=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*
f)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct
Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl_header,
ie->aggrdecls,_tmp230);_tmp230=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(
struct _dyneither_ptr*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct
Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_datatypedecl_header,
ie->datatypedecls,_tmp230);_tmp230=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*
f)(struct _dyneither_ptr*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct
Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl_header,
ie->enumdecls,_tmp230);if(_tmp230 != 0){_tmp230=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp230);_tmp230=Cyc_Interface_add_namespace(
_tmp230,ns);Cyc_Absynpp_decllist2file(_tmp230,f);}}static void Cyc_Interface_print_ns_xdatatypefielddecl_headers(
struct Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct
Cyc_List_List*_tmp231=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_dyneither_ptr*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),
struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xdatatypefielddecl_header,
ie->xdatatypefielddecls,0);if(_tmp231 != 0){_tmp231=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp231);_tmp231=Cyc_Interface_add_namespace(
_tmp231,ns);Cyc_Absynpp_decllist2file(_tmp231,f);}}static void Cyc_Interface_print_ns_typedefs(
struct Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct
Cyc_List_List*_tmp232=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_dyneither_ptr*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_typedef,ie->typedefdecls,
0);if(_tmp232 != 0){_tmp232=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp232);_tmp232=Cyc_Interface_add_namespace(_tmp232,ns);Cyc_Absynpp_decllist2file(
_tmp232,f);}}static void Cyc_Interface_print_ns_decls(struct Cyc___cycFILE*f,struct
Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*_tmp233=0;
_tmp233=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,
struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*
accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl,ie->aggrdecls,_tmp233);_tmp233=((
struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Datatypedecl*,
struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_datatypedecl,ie->datatypedecls,_tmp233);_tmp233=((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Enumdecl*,struct
Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_enumdecl,ie->enumdecls,_tmp233);_tmp233=((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Vardecl*,struct
Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_vardecl,ie->vardecls,_tmp233);_tmp233=((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Tcdecl_Xdatatypefielddecl*,
struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_xdatatypefielddecl,ie->xdatatypefielddecls,_tmp233);if(_tmp233
!= 0){_tmp233=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp233);_tmp233=Cyc_Interface_add_namespace(_tmp233,ns);Cyc_Absynpp_decllist2file(
_tmp233,f);}}void Cyc_Interface_print(struct Cyc_Interface_I*i,struct Cyc___cycFILE*
f){Cyc_Absynpp_set_params(& Cyc_Absynpp_cyci_params_r);({void*_tmp234=0;Cyc_fprintf(
f,({const char*_tmp235="/****** needed (headers) ******/\n";_tag_dyneither(
_tmp235,sizeof(char),34);}),_tag_dyneither(_tmp234,sizeof(void*),0));});((void(*)(
void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,
f,i->imports);({void*_tmp236=0;Cyc_fprintf(f,({const char*_tmp237="\n/****** provided (headers) ******/\n";
_tag_dyneither(_tmp237,sizeof(char),37);}),_tag_dyneither(_tmp236,sizeof(void*),
0));});((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,
f,i->exports);({void*_tmp238=0;Cyc_fprintf(f,({const char*_tmp239="\n/****** needed (headers of xdatatypefielddecls) ******/\n";
_tag_dyneither(_tmp239,sizeof(char),58);}),_tag_dyneither(_tmp238,sizeof(void*),
0));});((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_xdatatypefielddecl_headers,
f,i->imports);({void*_tmp23A=0;Cyc_fprintf(f,({const char*_tmp23B="\n/****** provided (headers of xdatatypefielddecls) ******/\n";
_tag_dyneither(_tmp23B,sizeof(char),60);}),_tag_dyneither(_tmp23A,sizeof(void*),
0));});((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_xdatatypefielddecl_headers,
f,i->exports);({void*_tmp23C=0;Cyc_fprintf(f,({const char*_tmp23D="\n/****** provided (typedefs) ******/\n";
_tag_dyneither(_tmp23D,sizeof(char),38);}),_tag_dyneither(_tmp23C,sizeof(void*),
0));});((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_typedefs,
f,i->exports);({void*_tmp23E=0;Cyc_fprintf(f,({const char*_tmp23F="\n/****** needed (declarations) ******/\n";
_tag_dyneither(_tmp23F,sizeof(char),40);}),_tag_dyneither(_tmp23E,sizeof(void*),
0));});((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,
f,i->imports);({void*_tmp240=0;Cyc_fprintf(f,({const char*_tmp241="\n/****** provided (declarations) ******/\n";
_tag_dyneither(_tmp241,sizeof(char),42);}),_tag_dyneither(_tmp240,sizeof(void*),
0));});((void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,
f,i->exports);}struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*f){
Cyc_Lex_lex_init();{struct Cyc_List_List*_tmp242=Cyc_Parse_parse_file(f);struct
_RegionHandle _tmp243=_new_region("r");struct _RegionHandle*r=& _tmp243;
_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp244=Cyc_Tcenv_tc_init(r);Cyc_Tc_tc(r,
_tmp244,0,_tmp242);{struct Cyc_Interface_I*_tmp245=Cyc_Interface_gen_extract(
_tmp244->ae,0);_npop_handler(0);return _tmp245;}};_pop_region(r);}}void Cyc_Interface_save(
struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){Cyc_Interface_print(i,f);}struct
Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*f){return Cyc_Interface_parse(
f);}
