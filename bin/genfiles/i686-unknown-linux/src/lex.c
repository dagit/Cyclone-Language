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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};struct
Cyc_Core_NewRegion Cyc_Core_new_dynregion();extern char Cyc_Core_Open_Region[16];
extern char Cyc_Core_Free_Region[16];void Cyc_Core_free_dynregion(struct
_DynRegionHandle*);typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}
__value;}Cyc___mbstate_t;typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;
typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);extern char Cyc_List_Nth[8];extern char Cyc_Lexing_Error[10];
struct Cyc_Lexing_Error_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{
void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct
_dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int
lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);char
Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);int Cyc_Lexing_lexeme_start(
struct Cyc_Lexing_lexbuf*);int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*
cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);void Cyc_Set_iter(void(*f)(void*),
struct Cyc_Set_Set*s);extern char Cyc_Set_Absent[11];unsigned int Cyc_strlen(struct
_dyneither_ptr s);int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
struct _dyneither_ptr Cyc_zstrncpy(struct _dyneither_ptr,struct _dyneither_ptr,
unsigned int);struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,
unsigned int n);struct Cyc_Xarray_Xarray{struct _RegionHandle*r;struct
_dyneither_ptr elmts;int num_elmts;};void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,
int);struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct _RegionHandle*,int,void*);
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);int Cyc_Xarray_add_ind(struct
Cyc_Xarray_Xarray*,void*);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;
struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];
struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_lex(
struct Cyc_Position_Segment*,struct _dyneither_ptr);struct Cyc_Position_Error*Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[
14];void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Absyn_Loc_n_struct{
int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct
Loc_n;struct Cyc_Absyn_Rel_n_struct Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};
struct _tuple0{union Cyc_Absyn_Nmspace_union f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Conref;
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
_tuple0*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};union Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct
UnknownTunion;struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{
union Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*tunion_name;struct
_tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int
tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};union Cyc_Absyn_TunionFieldInfoU_union{
struct Cyc_Absyn_UnknownTunionfield_struct UnknownTunionfield;struct Cyc_Absyn_KnownTunionfield_struct
KnownTunionfield;};struct Cyc_Absyn_TunionFieldInfo{union Cyc_Absyn_TunionFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
_dyneither_ptr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Subscript_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Struct_e_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tunion_e_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*
f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;
};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};
struct _tuple2{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
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
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Tunion_p_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*
f3;int f4;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;int f2;};struct Cyc_Absyn_Char_p_struct{
int tag;char f1;};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple0*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Region_d_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Declaration_spec;struct Cyc_Declarator;
struct Cyc_Abstractdeclarator;struct _tuple3{void*f1;int f2;};struct Cyc_Int_tok_struct{
int tag;struct _tuple3 f1;};struct Cyc_Char_tok_struct{int tag;char f1;};struct Cyc_String_tok_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Stringopt_tok_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_QualId_tok_struct{int tag;struct _tuple0*f1;};struct _tuple4{struct
Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Absyn_Conref*f3;};
struct Cyc_YY1_struct{int tag;struct _tuple4*f1;};struct Cyc_YY2_struct{int tag;
struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int tag;struct Cyc_Absyn_Exp*f1;}
;struct Cyc_YY4_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_YY5_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY7_struct{int tag;void*f1;};struct Cyc_YY8_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_YY9_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_YY10_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY11_struct{int tag;struct Cyc_Absyn_Pat*
f1;};struct _tuple5{struct Cyc_List_List*f1;int f2;};struct Cyc_YY12_struct{int tag;
struct _tuple5*f1;};struct Cyc_YY13_struct{int tag;struct Cyc_List_List*f1;};struct
_tuple6{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_YY14_struct{
int tag;struct _tuple6*f1;};struct Cyc_YY15_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY16_struct{int tag;struct _tuple5*f1;};struct Cyc_YY17_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_YY18_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY19_struct{int tag;struct Cyc_Declaration_spec*f1;};struct _tuple7{
struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_YY20_struct{int tag;
struct _tuple7*f1;};struct Cyc_YY21_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_YY22_struct{int tag;void*f1;};struct Cyc_YY23_struct{int tag;void*f1;};struct
Cyc_YY24_struct{int tag;void*f1;};struct Cyc_YY25_struct{int tag;struct Cyc_Absyn_Tqual
f1;};struct Cyc_YY26_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY27_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY28_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY29_struct{int tag;struct Cyc_Declarator*f1;};struct Cyc_YY30_struct{
int tag;struct Cyc_Abstractdeclarator*f1;};struct Cyc_YY31_struct{int tag;int f1;};
struct Cyc_YY32_struct{int tag;void*f1;};struct Cyc_YY33_struct{int tag;struct Cyc_Absyn_Tunionfield*
f1;};struct Cyc_YY34_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_YY35_struct{
int tag;struct _tuple8*f1;};struct Cyc_YY36_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY37_struct{int tag;struct _tuple1*f1;};struct Cyc_YY38_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple9{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*
f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_YY39_struct{int tag;
struct _tuple9*f1;};struct Cyc_YY40_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_YY41_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY42_struct{int tag;
void*f1;};struct Cyc_YY43_struct{int tag;void*f1;};struct Cyc_YY44_struct{int tag;
void*f1;};struct Cyc_YY45_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY46_struct{
int tag;void*f1;};struct Cyc_YY47_struct{int tag;struct Cyc_Absyn_Enumfield*f1;};
struct Cyc_YY48_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY49_struct{int
tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY50_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY51_struct{int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_YY52_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YYINITIALSVAL_struct{int tag;};union Cyc_YYSTYPE_union{
struct Cyc_Int_tok_struct Int_tok;struct Cyc_Char_tok_struct Char_tok;struct Cyc_String_tok_struct
String_tok;struct Cyc_Stringopt_tok_struct Stringopt_tok;struct Cyc_QualId_tok_struct
QualId_tok;struct Cyc_YY1_struct YY1;struct Cyc_YY2_struct YY2;struct Cyc_YY3_struct
YY3;struct Cyc_YY4_struct YY4;struct Cyc_YY5_struct YY5;struct Cyc_YY6_struct YY6;
struct Cyc_YY7_struct YY7;struct Cyc_YY8_struct YY8;struct Cyc_YY9_struct YY9;struct
Cyc_YY10_struct YY10;struct Cyc_YY11_struct YY11;struct Cyc_YY12_struct YY12;struct
Cyc_YY13_struct YY13;struct Cyc_YY14_struct YY14;struct Cyc_YY15_struct YY15;struct
Cyc_YY16_struct YY16;struct Cyc_YY17_struct YY17;struct Cyc_YY18_struct YY18;struct
Cyc_YY19_struct YY19;struct Cyc_YY20_struct YY20;struct Cyc_YY21_struct YY21;struct
Cyc_YY22_struct YY22;struct Cyc_YY23_struct YY23;struct Cyc_YY24_struct YY24;struct
Cyc_YY25_struct YY25;struct Cyc_YY26_struct YY26;struct Cyc_YY27_struct YY27;struct
Cyc_YY28_struct YY28;struct Cyc_YY29_struct YY29;struct Cyc_YY30_struct YY30;struct
Cyc_YY31_struct YY31;struct Cyc_YY32_struct YY32;struct Cyc_YY33_struct YY33;struct
Cyc_YY34_struct YY34;struct Cyc_YY35_struct YY35;struct Cyc_YY36_struct YY36;struct
Cyc_YY37_struct YY37;struct Cyc_YY38_struct YY38;struct Cyc_YY39_struct YY39;struct
Cyc_YY40_struct YY40;struct Cyc_YY41_struct YY41;struct Cyc_YY42_struct YY42;struct
Cyc_YY43_struct YY43;struct Cyc_YY44_struct YY44;struct Cyc_YY45_struct YY45;struct
Cyc_YY46_struct YY46;struct Cyc_YY47_struct YY47;struct Cyc_YY48_struct YY48;struct
Cyc_YY49_struct YY49;struct Cyc_YY50_struct YY50;struct Cyc_YY51_struct YY51;struct
Cyc_YY52_struct YY52;struct Cyc_YYINITIALSVAL_struct YYINITIALSVAL;};struct Cyc_Yyltype{
int timestamp;int first_line;int first_column;int last_line;int last_column;};extern
struct Cyc_Yyltype Cyc_yylloc;extern union Cyc_YYSTYPE_union Cyc_yylval;struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict
Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_insert(struct
Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
struct _tuple10{void*f1;void*f2;};struct _tuple10*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple10*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);void Cyc_yyerror(struct _dyneither_ptr s){Cyc_Position_post_error(
Cyc_Position_mk_err_parse(Cyc_Position_segment_of_abs(Cyc_yylloc.first_line,Cyc_yylloc.last_line),
s));}struct Cyc_Lex_Trie;struct Cyc_Lex_Zero_struct{int tag;};struct Cyc_Lex_One_struct{
int tag;int f1;struct Cyc_Lex_Trie*f2;};struct Cyc_Lex_Many_struct{int tag;struct Cyc_Lex_Trie**
f1;};union Cyc_Lex_TrieChildren_union{struct Cyc_Lex_Zero_struct Zero;struct Cyc_Lex_One_struct
One;struct Cyc_Lex_Many_struct Many;};struct Cyc_Lex_Trie{union Cyc_Lex_TrieChildren_union
children;int shared_str;};static int Cyc_Lex_num_kws=0;static struct _dyneither_ptr
Cyc_Lex_kw_nums={(void*)0,(void*)0,(void*)(0 + 0)};struct Cyc_Lex_DynTrie{struct
_DynRegionHandle*dyn;struct Cyc_Lex_Trie*t;};struct Cyc_Lex_DynTrieSymbols{struct
_DynRegionHandle*dyn;struct Cyc_Lex_Trie*t;struct Cyc_Xarray_Xarray*symbols;};
static struct Cyc_Lex_DynTrieSymbols*Cyc_Lex_ids_trie=0;static struct Cyc_Lex_DynTrie*
Cyc_Lex_typedefs_trie=0;static int Cyc_Lex_comment_depth=0;static struct _tuple3 Cyc_Lex_token_int={(
void*)0,0};static char _tmp0[8]="*bogus*";static struct _dyneither_ptr Cyc_Lex_bogus_string={
_tmp0,_tmp0,_tmp0 + 8};static struct _tuple0 Cyc_Lex_token_id_pair={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Lex_bogus_string};static char Cyc_Lex_token_char='\000';
static char _tmp2[1]="";static struct _dyneither_ptr Cyc_Lex_token_string={_tmp2,
_tmp2,_tmp2 + 1};static struct _tuple0*Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair;
static int Cyc_Lex_runaway_start=0;static void Cyc_Lex_err(struct _dyneither_ptr msg,
struct Cyc_Lexing_lexbuf*lb){struct Cyc_Position_Segment*s=Cyc_Position_segment_of_abs(
Cyc_Lexing_lexeme_start(lb),Cyc_Lexing_lexeme_end(lb));Cyc_Position_post_error(
Cyc_Position_mk_err_lex(s,msg));}static void Cyc_Lex_runaway_err(struct
_dyneither_ptr msg,struct Cyc_Lexing_lexbuf*lb){struct Cyc_Position_Segment*s=Cyc_Position_segment_of_abs(
Cyc_Lex_runaway_start,Cyc_Lexing_lexeme_start(lb));Cyc_Position_post_error(Cyc_Position_mk_err_lex(
s,msg));}struct _tuple11{struct _dyneither_ptr f1;short f2;int f3;};static char _tmp3[
14]="__attribute__";static char _tmp4[9]="abstract";static char _tmp5[6]="alias";
static char _tmp6[5]="auto";static char _tmp7[6]="break";static char _tmp8[7]="calloc";
static char _tmp9[5]="case";static char _tmpA[6]="catch";static char _tmpB[5]="char";
static char _tmpC[6]="const";static char _tmpD[9]="continue";static char _tmpE[8]="default";
static char _tmpF[3]="do";static char _tmp10[7]="double";static char _tmp11[12]="dynregion_t";
static char _tmp12[5]="else";static char _tmp13[5]="enum";static char _tmp14[7]="export";
static char _tmp15[7]="extern";static char _tmp16[9]="fallthru";static char _tmp17[9]="__flat__";
static char _tmp18[6]="float";static char _tmp19[4]="for";static char _tmp1A[6]="__gen";
static char _tmp1B[5]="goto";static char _tmp1C[3]="if";static char _tmp1D[7]="inline";
static char _tmp1E[11]="__inline__";static char _tmp1F[4]="int";static char _tmp20[4]="let";
static char _tmp21[5]="long";static char _tmp22[7]="malloc";static char _tmp23[10]="namespace";
static char _tmp24[4]="new";static char _tmp25[11]="NOZEROTERM";static char _tmp26[5]="NULL";
static char _tmp27[8]="numelts";static char _tmp28[9]="offsetof";static char _tmp29[20]="__cyclone_port_on__";
static char _tmp2A[21]="__cyclone_port_off__";static char _tmp2B[8]="rcalloc";static
char _tmp2C[9]="region_t";static char _tmp2D[7]="region";static char _tmp2E[8]="regions";
static char _tmp2F[9]="register";static char _tmp30[13]="reset_region";static char
_tmp31[9]="restrict";static char _tmp32[7]="return";static char _tmp33[8]="rmalloc";
static char _tmp34[5]="rnew";static char _tmp35[6]="short";static char _tmp36[7]="signed";
static char _tmp37[7]="sizeof";static char _tmp38[7]="static";static char _tmp39[7]="struct";
static char _tmp3A[8]="cycswap";static char _tmp3B[7]="switch";static char _tmp3C[6]="tag_t";
static char _tmp3D[6]="throw";static char _tmp3E[4]="try";static char _tmp3F[7]="tunion";
static char _tmp40[8]="typedef";static char _tmp41[6]="union";static char _tmp42[9]="unsigned";
static char _tmp43[6]="using";static char _tmp44[8]="valueof";static char _tmp45[10]="valueof_t";
static char _tmp46[5]="void";static char _tmp47[9]="volatile";static char _tmp48[6]="while";
static char _tmp49[8]="xtunion";static char _tmp4A[9]="ZEROTERM";static struct
_tuple11 Cyc_Lex_rw_array[72]={{{_tmp3,_tmp3,_tmp3 + 14},361,1},{{_tmp4,_tmp4,
_tmp4 + 9},300,0},{{_tmp5,_tmp5,_tmp5 + 6},324,0},{{_tmp6,_tmp6,_tmp6 + 5},258,1},{{
_tmp7,_tmp7,_tmp7 + 6},290,1},{{_tmp8,_tmp8,_tmp8 + 7},308,0},{{_tmp9,_tmp9,_tmp9 + 
5},277,1},{{_tmpA,_tmpA,_tmpA + 6},297,1},{{_tmpB,_tmpB,_tmpB + 5},264,1},{{_tmpC,
_tmpC,_tmpC + 6},272,1},{{_tmpD,_tmpD,_tmpD + 9},289,1},{{_tmpE,_tmpE,_tmpE + 8},
278,1},{{_tmpF,_tmpF,_tmpF + 3},286,1},{{_tmp10,_tmp10,_tmp10 + 7},269,1},{{_tmp11,
_tmp11,_tmp11 + 12},323,0},{{_tmp12,_tmp12,_tmp12 + 5},283,1},{{_tmp13,_tmp13,
_tmp13 + 5},292,1},{{_tmp14,_tmp14,_tmp14 + 7},298,0},{{_tmp15,_tmp15,_tmp15 + 7},
261,1},{{_tmp16,_tmp16,_tmp16 + 9},301,0},{{_tmp17,_tmp17,_tmp17 + 9},322,0},{{
_tmp18,_tmp18,_tmp18 + 6},268,1},{{_tmp19,_tmp19,_tmp19 + 4},287,1},{{_tmp1A,
_tmp1A,_tmp1A + 6},317,0},{{_tmp1B,_tmp1B,_tmp1B + 5},288,1},{{_tmp1C,_tmp1C,
_tmp1C + 3},282,1},{{_tmp1D,_tmp1D,_tmp1D + 7},279,1},{{_tmp1E,_tmp1E,_tmp1E + 11},
279,1},{{_tmp1F,_tmp1F,_tmp1F + 4},266,1},{{_tmp20,_tmp20,_tmp20 + 4},294,0},{{
_tmp21,_tmp21,_tmp21 + 5},267,1},{{_tmp22,_tmp22,_tmp22 + 7},306,0},{{_tmp23,
_tmp23,_tmp23 + 10},303,0},{{_tmp24,_tmp24,_tmp24 + 4},299,0},{{_tmp25,_tmp25,
_tmp25 + 11},318,0},{{_tmp26,_tmp26,_tmp26 + 5},293,0},{{_tmp27,_tmp27,_tmp27 + 8},
325,0},{{_tmp28,_tmp28,_tmp28 + 9},281,1},{{_tmp29,_tmp29,_tmp29 + 20},320,0},{{
_tmp2A,_tmp2A,_tmp2A + 21},321,0},{{_tmp2B,_tmp2B,_tmp2B + 8},309,0},{{_tmp2C,
_tmp2C,_tmp2C + 9},311,0},{{_tmp2D,_tmp2D,_tmp2D + 7},313,0},{{_tmp2E,_tmp2E,
_tmp2E + 8},315,0},{{_tmp2F,_tmp2F,_tmp2F + 9},259,1},{{_tmp30,_tmp30,_tmp30 + 13},
316,0},{{_tmp31,_tmp31,_tmp31 + 9},274,1},{{_tmp32,_tmp32,_tmp32 + 7},291,1},{{
_tmp33,_tmp33,_tmp33 + 8},307,0},{{_tmp34,_tmp34,_tmp34 + 5},314,0},{{_tmp35,
_tmp35,_tmp35 + 6},265,1},{{_tmp36,_tmp36,_tmp36 + 7},270,1},{{_tmp37,_tmp37,
_tmp37 + 7},280,1},{{_tmp38,_tmp38,_tmp38 + 7},260,1},{{_tmp39,_tmp39,_tmp39 + 7},
275,1},{{_tmp3A,_tmp3A,_tmp3A + 8},310,0},{{_tmp3B,_tmp3B,_tmp3B + 7},284,1},{{
_tmp3C,_tmp3C,_tmp3C + 6},312,0},{{_tmp3D,_tmp3D,_tmp3D + 6},295,0},{{_tmp3E,
_tmp3E,_tmp3E + 4},296,0},{{_tmp3F,_tmp3F,_tmp3F + 7},304,0},{{_tmp40,_tmp40,
_tmp40 + 8},262,1},{{_tmp41,_tmp41,_tmp41 + 6},276,1},{{_tmp42,_tmp42,_tmp42 + 9},
271,1},{{_tmp43,_tmp43,_tmp43 + 6},302,0},{{_tmp44,_tmp44,_tmp44 + 8},326,0},{{
_tmp45,_tmp45,_tmp45 + 10},327,0},{{_tmp46,_tmp46,_tmp46 + 5},263,1},{{_tmp47,
_tmp47,_tmp47 + 9},273,1},{{_tmp48,_tmp48,_tmp48 + 6},285,1},{{_tmp49,_tmp49,
_tmp49 + 8},305,0},{{_tmp4A,_tmp4A,_tmp4A + 9},319,0}};static int Cyc_Lex_num_keywords(
int include_cyclone_keywords){int sum=0;{unsigned int i=0;for(0;i < 72;++ i){if(
include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)i]).f3)++ sum;}}return sum;}
static int Cyc_Lex_trie_char(int c){if(c >= 95)return c - 59;else{if(c > 64)return c - 55;}
return c - 48;}static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,
struct Cyc_Lex_Trie*t,struct _dyneither_ptr buff,int offset,int len){int i=offset;int
last=(offset + len)- 1;while(i <= last){union Cyc_Lex_TrieChildren_union _tmp4B=((
struct Cyc_Lex_Trie*)_check_null(t))->children;struct Cyc_Lex_Trie**_tmp4C;int
_tmp4D;struct Cyc_Lex_Trie*_tmp4E;_LL1: if((_tmp4B.Many).tag != 2)goto _LL3;_tmp4C=(
_tmp4B.Many).f1;_LL2: {int ch=Cyc_Lex_trie_char((int)*((const char*)
_check_dyneither_subscript(buff,sizeof(char),i)));if(_tmp4C[
_check_known_subscript_notnull(64,ch)]== 0)_tmp4C[_check_known_subscript_notnull(
64,ch)]=({struct Cyc_Lex_Trie*_tmp4F=_region_malloc(r,sizeof(*_tmp4F));_tmp4F->children=(
union Cyc_Lex_TrieChildren_union)({union Cyc_Lex_TrieChildren_union _tmp50;(_tmp50.Zero).tag=
0;_tmp50;});_tmp4F->shared_str=0;_tmp4F;});t=_tmp4C[
_check_known_subscript_notnull(64,ch)];++ i;goto _LL0;}_LL3: if((_tmp4B.One).tag != 
1)goto _LL5;_tmp4D=(_tmp4B.One).f1;_tmp4E=(_tmp4B.One).f2;_LL4: if(_tmp4D == *((
const char*)_check_dyneither_subscript(buff,sizeof(char),i)))t=_tmp4E;else{struct
Cyc_Lex_Trie**_tmp51=({unsigned int _tmp56=64;struct Cyc_Lex_Trie**_tmp57=(struct
Cyc_Lex_Trie**)_region_malloc(r,_check_times(sizeof(struct Cyc_Lex_Trie*),_tmp56));{
unsigned int _tmp58=_tmp56;unsigned int j;for(j=0;j < _tmp58;j ++){_tmp57[j]=0;}}
_tmp57;});_tmp51[_check_known_subscript_notnull(64,Cyc_Lex_trie_char(_tmp4D))]=
_tmp4E;{int _tmp52=Cyc_Lex_trie_char((int)*((const char*)
_check_dyneither_subscript(buff,sizeof(char),i)));_tmp51[
_check_known_subscript_notnull(64,_tmp52)]=({struct Cyc_Lex_Trie*_tmp53=
_region_malloc(r,sizeof(*_tmp53));_tmp53->children=(union Cyc_Lex_TrieChildren_union)({
union Cyc_Lex_TrieChildren_union _tmp54;(_tmp54.Zero).tag=0;_tmp54;});_tmp53->shared_str=
0;_tmp53;});t->children=(union Cyc_Lex_TrieChildren_union)({union Cyc_Lex_TrieChildren_union
_tmp55;(_tmp55.Many).tag=2;(_tmp55.Many).f1=_tmp51;_tmp55;});t=_tmp51[
_check_known_subscript_notnull(64,_tmp52)];}}++ i;goto _LL0;_LL5: if((_tmp4B.Zero).tag
!= 0)goto _LL0;_LL6: while(i <= last){struct Cyc_Lex_Trie*_tmp59=({struct Cyc_Lex_Trie*
_tmp5B=_region_malloc(r,sizeof(*_tmp5B));_tmp5B->children=(union Cyc_Lex_TrieChildren_union)({
union Cyc_Lex_TrieChildren_union _tmp5C;(_tmp5C.Zero).tag=0;_tmp5C;});_tmp5B->shared_str=
0;_tmp5B;});((struct Cyc_Lex_Trie*)_check_null(t))->children=(union Cyc_Lex_TrieChildren_union)({
union Cyc_Lex_TrieChildren_union _tmp5A;(_tmp5A.One).tag=1;(_tmp5A.One).f1=(int)*((
const char*)_check_dyneither_subscript(buff,sizeof(char),i ++));(_tmp5A.One).f2=
_tmp59;_tmp5A;});t=_tmp59;}return t;_LL0:;}return t;}static int Cyc_Lex_str_index(
struct _dyneither_ptr buff,int offset,int len){struct _DynRegionHandle*_tmp5E;struct
Cyc_Lex_Trie*_tmp5F;struct Cyc_Xarray_Xarray*_tmp60;struct Cyc_Lex_DynTrieSymbols
_tmp5D=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(Cyc_Lex_ids_trie));_tmp5E=
_tmp5D.dyn;_tmp5F=_tmp5D.t;_tmp60=_tmp5D.symbols;{struct _DynRegionFrame _tmp61;
struct _RegionHandle*d=_open_dynregion(& _tmp61,_tmp5E);{struct Cyc_Lex_Trie*_tmp62=
Cyc_Lex_trie_lookup(d,_tmp5F,buff,offset,len);if(_tmp62->shared_str == 0){struct
_dyneither_ptr _tmp63=Cyc_Core_new_string((unsigned int)(len + 1));Cyc_zstrncpy(
_dyneither_ptr_decrease_size(_tmp63,sizeof(char),1),(struct _dyneither_ptr)
_dyneither_ptr_plus(buff,sizeof(char),offset),(unsigned int)len);{int ans=((int(*)(
struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp60,({
struct _dyneither_ptr*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64[0]=(struct
_dyneither_ptr)_tmp63;_tmp64;}));_tmp62->shared_str=ans;}}{int _tmp65=_tmp62->shared_str;
_npop_handler(0);return _tmp65;}};_pop_dynregion(d);}}static int Cyc_Lex_str_index_lbuf(
struct Cyc_Lexing_lexbuf*lbuf){return Cyc_Lex_str_index((struct _dyneither_ptr)lbuf->lex_buffer,
lbuf->lex_start_pos,lbuf->lex_curr_pos - lbuf->lex_start_pos);}static void Cyc_Lex_insert_typedef(
struct _dyneither_ptr*sptr){struct _dyneither_ptr _tmp66=*sptr;struct
_DynRegionHandle*_tmp68;struct Cyc_Lex_Trie*_tmp69;struct Cyc_Lex_DynTrie _tmp67=*((
struct Cyc_Lex_DynTrie*)_check_null(Cyc_Lex_typedefs_trie));_tmp68=_tmp67.dyn;
_tmp69=_tmp67.t;{struct _DynRegionFrame _tmp6A;struct _RegionHandle*d=
_open_dynregion(& _tmp6A,_tmp68);{struct Cyc_Lex_Trie*_tmp6B=Cyc_Lex_trie_lookup(d,
_tmp69,_tmp66,0,(int)(_get_dyneither_size(_tmp66,sizeof(char))- 1));((struct Cyc_Lex_Trie*)
_check_null(_tmp6B))->shared_str=1;}_npop_handler(0);return;;_pop_dynregion(d);}}
static struct _dyneither_ptr*Cyc_Lex_get_symbol(int symbol_num){struct
_DynRegionHandle*_tmp6D;struct Cyc_Lex_Trie*_tmp6E;struct Cyc_Xarray_Xarray*_tmp6F;
struct Cyc_Lex_DynTrieSymbols _tmp6C=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(
Cyc_Lex_ids_trie));_tmp6D=_tmp6C.dyn;_tmp6E=_tmp6C.t;_tmp6F=_tmp6C.symbols;{
struct _DynRegionFrame _tmp70;struct _RegionHandle*d=_open_dynregion(& _tmp70,_tmp6D);{
struct _dyneither_ptr*_tmp71=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp6F,symbol_num);_npop_handler(0);return _tmp71;};
_pop_dynregion(d);}}static int Cyc_Lex_int_of_char(char c){if('0' <= c  && c <= '9')
return c - '0';else{if('a' <= c  && c <= 'f')return(10 + c)- 'a';else{if('A' <= c  && c
<= 'F')return(10 + c)- 'A';else{(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp73;_tmp73.tag=Cyc_Core_Invalid_argument;_tmp73.f1=({const char*_tmp74="string to integer conversion";
_tag_dyneither(_tmp74,sizeof(char),29);});_tmp73;});_tmp72;}));}}}}static struct
_tuple3 Cyc_Lex_intconst(struct Cyc_Lexing_lexbuf*lbuf,int start,int end,int base){
unsigned int n=0;int end2=lbuf->lex_curr_pos - end;struct _dyneither_ptr buff=lbuf->lex_buffer;
int i=start + lbuf->lex_start_pos;{int i=start + lbuf->lex_start_pos;for(0;i < end2;
++ i){char c=*((char*)_check_dyneither_subscript(buff,sizeof(char),i));switch(c){
case 'u': _LL7: goto _LL8;case 'U': _LL8: return({struct _tuple3 _tmp75;_tmp75.f1=(void*)
1;_tmp75.f2=(int)n;_tmp75;});case 'l': _LL9: break;case 'L': _LLA: break;default: _LLB:
n=n * base + (unsigned int)Cyc_Lex_int_of_char(c);break;}}}return({struct _tuple3
_tmp76;_tmp76.f1=(void*)0;_tmp76.f2=(int)n;_tmp76;});}char Cyc_Lex_string_buffer_v[
11]={'x','x','x','x','x','x','x','x','x','x','\000'};struct _dyneither_ptr Cyc_Lex_string_buffer={(
void*)Cyc_Lex_string_buffer_v,(void*)Cyc_Lex_string_buffer_v,(void*)(Cyc_Lex_string_buffer_v
+ 11)};int Cyc_Lex_string_pos=0;void Cyc_Lex_store_string_char(char c){int sz=(int)(
_get_dyneither_size(Cyc_Lex_string_buffer,sizeof(char))- 1);if(Cyc_Lex_string_pos
>= sz){int newsz=sz;while(Cyc_Lex_string_pos >= newsz){newsz=newsz * 2;}{struct
_dyneither_ptr str=({unsigned int _tmp77=(unsigned int)newsz;char*_tmp78=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp77 + 1));struct _dyneither_ptr
_tmp7A=_tag_dyneither(_tmp78,sizeof(char),_tmp77 + 1);{unsigned int _tmp79=_tmp77;
unsigned int i;for(i=0;i < _tmp79;i ++){_tmp78[i]=i < sz?*((char*)
_check_dyneither_subscript(Cyc_Lex_string_buffer,sizeof(char),(int)i)):'\000';}
_tmp78[_tmp79]=(char)0;}_tmp7A;});Cyc_Lex_string_buffer=str;}}({struct
_dyneither_ptr _tmp7B=_dyneither_ptr_plus(Cyc_Lex_string_buffer,sizeof(char),Cyc_Lex_string_pos);
char _tmp7C=*((char*)_check_dyneither_subscript(_tmp7B,sizeof(char),0));char
_tmp7D=c;if(_get_dyneither_size(_tmp7B,sizeof(char))== 1  && (_tmp7C == '\000'  && 
_tmp7D != '\000'))_throw_arraybounds();*((char*)_tmp7B.curr)=_tmp7D;});++ Cyc_Lex_string_pos;}
struct _dyneither_ptr Cyc_Lex_get_stored_string(){struct _dyneither_ptr str=Cyc_substring((
struct _dyneither_ptr)Cyc_Lex_string_buffer,0,(unsigned int)Cyc_Lex_string_pos);
Cyc_Lex_string_pos=0;return str;}struct Cyc_Lex_Ldecls{struct Cyc_Set_Set*typedefs;
struct Cyc_Set_Set*namespaces;};struct Cyc_Lex_Lvis{struct Cyc_List_List*
current_namespace;struct Cyc_List_List*imported_namespaces;};struct Cyc_Lex_Lstate{
struct Cyc_List_List*lstack;struct Cyc_Dict_Dict decls;};static struct Cyc_Core_Opt*
Cyc_Lex_lstate=0;static void Cyc_Lex_typedef_init(){struct Cyc_Lex_Lvis*_tmp7E=({
struct Cyc_Lex_Lvis*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->current_namespace=0;
_tmp84->imported_namespaces=0;_tmp84;});struct Cyc_List_List*_tmp7F=({struct Cyc_List_List*
_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->hd=_tmp7E;_tmp83->tl=0;_tmp83;});
struct Cyc_Dict_Dict init_decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(
int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),
0,({struct Cyc_Lex_Ldecls*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->typedefs=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Set_empty)(Cyc_zstrptrcmp);_tmp82->namespaces=((struct Cyc_Set_Set*(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp);
_tmp82;}));Cyc_Lex_lstate=({struct Cyc_Core_Opt*_tmp80=_cycalloc(sizeof(*_tmp80));
_tmp80->v=({struct Cyc_Lex_Lstate*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->lstack=
_tmp7F;_tmp81->decls=init_decls;_tmp81;});_tmp80;});}static struct Cyc_List_List*
Cyc_Lex_get_absolute_namespace(struct Cyc_List_List*ns){struct _dyneither_ptr*n=(
struct _dyneither_ptr*)ns->hd;{struct Cyc_List_List*ls=(struct Cyc_List_List*)((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;
for(0;ls != 0;ls=ls->tl){struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;struct
Cyc_List_List*x=({struct Cyc_List_List*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->hd=
lv->current_namespace;_tmp85->tl=lv->imported_namespaces;_tmp85;});for(0;x != 0;x=((
struct Cyc_List_List*)_check_null(x))->tl){struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)((
struct Cyc_List_List*)_check_null(x))->hd);if(((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(ld->namespaces,n))return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((struct Cyc_List_List*)((
struct Cyc_List_List*)_check_null(x))->hd,(struct Cyc_List_List*)ns);}}}Cyc_yyerror((
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp88;_tmp88.tag=0;_tmp88.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_str_sepstr((struct Cyc_List_List*)
ns,({const char*_tmp89="::";_tag_dyneither(_tmp89,sizeof(char),3);})));{void*
_tmp86[1]={& _tmp88};Cyc_aprintf(({const char*_tmp87="undeclared namespace %s";
_tag_dyneither(_tmp87,sizeof(char),24);}),_tag_dyneither(_tmp86,sizeof(void*),1));}}));
return 0;}static void Cyc_Lex_recompute_typedefs(){struct _DynRegionHandle*_tmp8B;
struct Cyc_Lex_Trie*_tmp8C;struct Cyc_Lex_DynTrie _tmp8A=*((struct Cyc_Lex_DynTrie*)
_check_null(Cyc_Lex_typedefs_trie));_tmp8B=_tmp8A.dyn;_tmp8C=_tmp8A.t;Cyc_Core_free_dynregion(
_tmp8B);{struct _DynRegionHandle*_tmp8E;struct Cyc_Core_NewRegion _tmp8D=Cyc_Core_new_dynregion();
_tmp8E=_tmp8D.dynregion;{struct _DynRegionFrame _tmp8F;struct _RegionHandle*d2=
_open_dynregion(& _tmp8F,_tmp8E);Cyc_Lex_typedefs_trie=({struct Cyc_Lex_DynTrie*
_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->dyn=(struct _DynRegionHandle*)_tmp8E;
_tmp90->t=(struct Cyc_Lex_Trie*)({struct Cyc_Lex_Trie*_tmp91=_region_malloc(d2,
sizeof(*_tmp91));_tmp91->children=(union Cyc_Lex_TrieChildren_union)({union Cyc_Lex_TrieChildren_union
_tmp92;(_tmp92.Zero).tag=0;_tmp92;});_tmp91->shared_str=0;_tmp91;});_tmp90;});{
struct Cyc_List_List*ls=(struct Cyc_List_List*)((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->lstack;for(0;ls != 0;ls=ls->tl){struct Cyc_Lex_Lvis*
lv=(struct Cyc_Lex_Lvis*)ls->hd;struct Cyc_List_List*x=({struct Cyc_List_List*
_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->hd=lv->current_namespace;_tmp93->tl=lv->imported_namespaces;
_tmp93;});for(0;x != 0;x=((struct Cyc_List_List*)_check_null(x))->tl){struct Cyc_Lex_Ldecls*
ld=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(
struct Cyc_List_List*)((struct Cyc_List_List*)_check_null(x))->hd);((void(*)(void(*
f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,
ld->typedefs);}}};_pop_dynregion(d2);}}}static int Cyc_Lex_is_typedef_in_namespace(
struct Cyc_List_List*ns,struct _dyneither_ptr*v){struct Cyc_List_List*ans=Cyc_Lex_get_absolute_namespace(
ns);struct _handler_cons _tmp94;_push_handler(& _tmp94);{int _tmp96=0;if(setjmp(
_tmp94.handler))_tmp96=1;if(!_tmp96){{struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ans);int _tmp97=((int(*)(
struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ld->typedefs,v);
_npop_handler(0);return _tmp97;};_pop_handler();}else{void*_tmp95=(void*)
_exn_thrown;void*_tmp99=_tmp95;_LLE: if(_tmp99 != Cyc_Dict_Absent)goto _LL10;_LLF:
return 0;_LL10:;_LL11:(void)_throw(_tmp99);_LLD:;}}}static int Cyc_Lex_is_typedef(
struct Cyc_List_List*ns,struct _dyneither_ptr*v){if(ns != 0)return Cyc_Lex_is_typedef_in_namespace((
struct Cyc_List_List*)ns,v);{struct _dyneither_ptr _tmp9A=*v;int len=(int)(
_get_dyneither_size(_tmp9A,sizeof(char))- 1);struct _DynRegionHandle*_tmp9C;
struct Cyc_Lex_Trie*_tmp9D;struct Cyc_Lex_DynTrie _tmp9B=*((struct Cyc_Lex_DynTrie*)
_check_null(Cyc_Lex_typedefs_trie));_tmp9C=_tmp9B.dyn;_tmp9D=_tmp9B.t;{struct
_DynRegionFrame _tmp9E;struct _RegionHandle*d=_open_dynregion(& _tmp9E,_tmp9C);{int
i=0;for(0;i < len;++ i){union Cyc_Lex_TrieChildren_union _tmp9F=((struct Cyc_Lex_Trie*)
_check_null(_tmp9D))->children;int _tmpA0;struct Cyc_Lex_Trie*_tmpA1;struct Cyc_Lex_Trie*
_tmpA2;struct Cyc_Lex_Trie**_tmpA3;_LL13: if((_tmp9F.Zero).tag != 0)goto _LL15;_LL14: {
int _tmpA4=0;_npop_handler(0);return _tmpA4;}_LL15: if((_tmp9F.One).tag != 1)goto
_LL17;_tmpA0=(_tmp9F.One).f1;_tmpA1=(_tmp9F.One).f2;if(!(_tmpA0 != *((const char*)
_check_dyneither_subscript(_tmp9A,sizeof(char),i))))goto _LL17;_LL16: {int _tmpA5=
0;_npop_handler(0);return _tmpA5;}_LL17: if((_tmp9F.One).tag != 1)goto _LL19;_tmpA2=(
_tmp9F.One).f2;_LL18: _tmp9D=_tmpA2;goto _LL12;_LL19: if((_tmp9F.Many).tag != 2)goto
_LL12;_tmpA3=(_tmp9F.Many).f1;_LL1A: {struct Cyc_Lex_Trie*_tmpA6=_tmpA3[
_check_known_subscript_notnull(64,Cyc_Lex_trie_char((int)*((const char*)
_check_dyneither_subscript(_tmp9A,sizeof(char),i))))];if(_tmpA6 == 0){int _tmpA7=0;
_npop_handler(0);return _tmpA7;}_tmp9D=_tmpA6;goto _LL12;}_LL12:;}}{int _tmpA8=((
struct Cyc_Lex_Trie*)_check_null(_tmp9D))->shared_str;_npop_handler(0);return
_tmpA8;};_pop_dynregion(d);}}}void Cyc_Lex_enter_namespace(struct _dyneither_ptr*s){
struct Cyc_List_List*ns=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct
Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_List_List*new_ns=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(ns,({struct Cyc_List_List*_tmpAD=
_cycalloc(sizeof(*_tmpAD));_tmpAD->hd=s;_tmpAD->tl=0;_tmpAD;}));((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=({struct Cyc_List_List*
_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->hd=({struct Cyc_Lex_Lvis*_tmpAA=
_cycalloc(sizeof(*_tmpAA));_tmpAA->current_namespace=new_ns;_tmpAA->imported_namespaces=
0;_tmpAA;});_tmpA9->tl=(struct Cyc_List_List*)((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->lstack;_tmpA9;});{struct Cyc_Lex_Ldecls*ld=((
struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,
ns);if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
ld->namespaces,s)){((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*
v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->decls,ns,({struct Cyc_Lex_Ldecls*_tmpAB=_cycalloc(sizeof(*
_tmpAB));_tmpAB->typedefs=ld->typedefs;_tmpAB->namespaces=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(ld->namespaces,s);
_tmpAB;}));((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*
v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->decls,new_ns,({struct Cyc_Lex_Ldecls*_tmpAC=_cycalloc(
sizeof(*_tmpAC));_tmpAC->typedefs=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp);_tmpAC->namespaces=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Set_empty)(Cyc_zstrptrcmp);_tmpAC;}));}((void(*)(void(*f)(struct
_dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((
struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,
new_ns))->typedefs);}}void Cyc_Lex_leave_namespace(){((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=(struct Cyc_List_List*)
_check_null((((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->tl);
Cyc_Lex_recompute_typedefs();}void Cyc_Lex_enter_using(struct _tuple0*q){struct Cyc_List_List*
ns;{union Cyc_Absyn_Nmspace_union _tmpAE=(*q).f1;struct Cyc_List_List*_tmpAF;struct
Cyc_List_List*_tmpB0;_LL1C: if((_tmpAE.Loc_n).tag != 0)goto _LL1E;_LL1D: ns=(struct
Cyc_List_List*)({struct Cyc_List_List*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->hd=(*
q).f2;_tmpB1->tl=0;_tmpB1;});goto _LL1B;_LL1E: if((_tmpAE.Rel_n).tag != 1)goto _LL20;
_tmpAF=(_tmpAE.Rel_n).f1;_LL1F: _tmpB0=_tmpAF;goto _LL21;_LL20: if((_tmpAE.Abs_n).tag
!= 2)goto _LL1B;_tmpB0=(_tmpAE.Abs_n).f1;_LL21: ns=(struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmpB0,({struct Cyc_List_List*_tmpB2=_cycalloc(sizeof(*_tmpB2));
_tmpB2->hd=(*q).f2;_tmpB2->tl=0;_tmpB2;})));goto _LL1B;_LL1B:;}{struct Cyc_List_List*
_tmpB3=Cyc_Lex_get_absolute_namespace(ns);struct Cyc_List_List*_tmpB4=((struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;((
struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=({struct Cyc_List_List*
_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->hd=_tmpB3;_tmpB5->tl=_tmpB4;_tmpB5;});((
void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((
struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,
_tmpB3))->typedefs);}}void Cyc_Lex_leave_using(){struct Cyc_List_List*_tmpB6=((
struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;((struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=((
struct Cyc_List_List*)_check_null(_tmpB6))->tl;Cyc_Lex_recompute_typedefs();}void
Cyc_Lex_register_typedef(struct _tuple0*q){struct Cyc_List_List*_tmpB7=((struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_Dict_Dict _tmpB8=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls;struct Cyc_Lex_Ldecls*_tmpB9=((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmpB8,_tmpB7);
struct Cyc_Lex_Ldecls*_tmpBA=({struct Cyc_Lex_Ldecls*_tmpBB=_cycalloc(sizeof(*
_tmpBB));_tmpBB->typedefs=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_insert)(_tmpB9->typedefs,(*q).f2);_tmpBB->namespaces=
_tmpB9->namespaces;_tmpBB;});((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(_tmpB8,_tmpB7,
_tmpBA);Cyc_Lex_insert_typedef((*q).f2);}static short Cyc_Lex_process_id(struct Cyc_Lexing_lexbuf*
lbuf){int symbol_num=Cyc_Lex_str_index_lbuf(lbuf);if(symbol_num <= Cyc_Lex_num_kws)
return(short)*((int*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(int),
symbol_num - 1));{struct _dyneither_ptr*_tmpBC=Cyc_Lex_get_symbol(symbol_num);Cyc_Lex_token_string=*
_tmpBC;if(Cyc_Lex_is_typedef(0,_tmpBC))return 358;return 352;}}static short Cyc_Lex_process_qual_id(
struct Cyc_Lexing_lexbuf*lbuf){int i=lbuf->lex_start_pos;int end=lbuf->lex_curr_pos;
struct _dyneither_ptr s=lbuf->lex_buffer;struct Cyc_List_List*rev_vs=0;while(i < end){
int start=i;for(0;i < end  && *((char*)_check_dyneither_subscript(s,sizeof(char),i))
!= ':';++ i){;}if(start == i)(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Core_Impossible_struct
_tmpBE;_tmpBE.tag=Cyc_Core_Impossible;_tmpBE.f1=({const char*_tmpBF="bad namespace";
_tag_dyneither(_tmpBF,sizeof(char),14);});_tmpBE;});_tmpBD;}));{int vlen=i - start;
struct _dyneither_ptr*v=Cyc_Lex_get_symbol(Cyc_Lex_str_index((struct
_dyneither_ptr)s,start,vlen));rev_vs=({struct Cyc_List_List*_tmpC0=_cycalloc(
sizeof(*_tmpC0));_tmpC0->hd=v;_tmpC0->tl=rev_vs;_tmpC0;});i +=2;}}if(rev_vs == 0)(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpC1=_cycalloc(sizeof(*
_tmpC1));_tmpC1[0]=({struct Cyc_Core_Impossible_struct _tmpC2;_tmpC2.tag=Cyc_Core_Impossible;
_tmpC2.f1=({const char*_tmpC3="bad namespace";_tag_dyneither(_tmpC3,sizeof(char),
14);});_tmpC2;});_tmpC1;}));{struct _dyneither_ptr*v=(struct _dyneither_ptr*)
rev_vs->hd;struct Cyc_List_List*vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(rev_vs->tl);Cyc_Lex_token_qvar=({struct _tuple0*_tmpC4=
_cycalloc(sizeof(*_tmpC4));_tmpC4->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmpC5;(_tmpC5.Rel_n).tag=1;(_tmpC5.Rel_n).f1=vs;_tmpC5;});_tmpC4->f2=v;_tmpC4;});
if(Cyc_Lex_is_typedef(vs,v))return 360;return 359;}}int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*);int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*);
int Cyc_yylex(struct Cyc_Lexing_lexbuf*lbuf){int ans=((int(*)(struct Cyc_Lexing_lexbuf*))
Cyc_Lex_token)(lbuf);Cyc_yylloc.first_line=((int(*)(struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme_start)(lbuf);Cyc_yylloc.last_line=((int(*)(struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme_end)(lbuf);switch(ans){case 352: _LL22: Cyc_yylval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmpC6;(_tmpC6.String_tok).tag=2;(_tmpC6.String_tok).f1=
Cyc_Lex_token_string;_tmpC6;});break;case 358: _LL23: Cyc_yylval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmpC7;(_tmpC7.String_tok).tag=2;(_tmpC7.String_tok).f1=
Cyc_Lex_token_string;_tmpC7;});break;case 359: _LL24: Cyc_yylval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmpC8;(_tmpC8.QualId_tok).tag=4;(_tmpC8.QualId_tok).f1=
Cyc_Lex_token_qvar;_tmpC8;});break;case 360: _LL25: Cyc_yylval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmpC9;(_tmpC9.QualId_tok).tag=4;(_tmpC9.QualId_tok).f1=
Cyc_Lex_token_qvar;_tmpC9;});break;case 357: _LL26: Cyc_yylval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmpCA;(_tmpCA.String_tok).tag=2;(_tmpCA.String_tok).f1=
Cyc_Lex_token_string;_tmpCA;});break;case 353: _LL27: Cyc_yylval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmpCB;(_tmpCB.Int_tok).tag=0;(_tmpCB.Int_tok).f1=Cyc_Lex_token_int;
_tmpCB;});break;case 355: _LL28: Cyc_yylval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmpCC;(_tmpCC.Char_tok).tag=1;(_tmpCC.Char_tok).f1=Cyc_Lex_token_char;_tmpCC;});
break;case 356: _LL29: Cyc_yylval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmpCD;(_tmpCD.String_tok).tag=2;(_tmpCD.String_tok).f1=Cyc_Lex_token_string;
_tmpCD;});break;case 354: _LL2A: Cyc_yylval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmpCE;(_tmpCE.String_tok).tag=2;(_tmpCE.String_tok).f1=Cyc_Lex_token_string;
_tmpCE;});break;default: _LL2B: break;}return ans;}const int Cyc_Lex_lex_base[150]=(
const int[150]){0,113,17,83,16,2,- 3,- 1,- 2,- 17,- 18,4,118,119,- 19,5,- 13,- 12,85,- 14,
- 11,- 4,- 5,- 6,- 7,- 8,- 9,- 10,192,215,111,- 15,166,- 18,- 55,8,30,- 39,18,31,116,139,32,
138,135,137,159,250,325,83,82,84,94,395,85,470,528,90,- 54,- 22,- 28,603,678,- 23,
753,90,811,886,961,99,123,- 26,134,- 31,- 25,- 34,1036,1065,363,117,128,292,1075,
1105,1141,335,1166,1178,1248,133,143,1318,1356,137,147,191,201,193,203,- 6,- 38,
199,- 20,- 37,6,157,1288,- 33,- 16,- 32,- 17,2,1396,168,169,279,171,174,176,185,186,
187,188,189,191,1469,1553,- 52,- 46,- 45,- 44,- 43,- 42,- 41,- 40,- 47,- 50,- 51,311,193,-
48,- 49,- 53,- 27,- 24,- 21,310,- 35,12,425};const int Cyc_Lex_lex_backtrk[150]=(const
int[150]){- 1,- 1,- 1,5,3,4,- 1,- 1,- 1,- 1,- 1,16,1,19,- 1,2,- 1,- 1,14,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,15,14,- 1,- 1,- 1,- 1,35,54,- 1,54,54,54,54,54,54,54,54,54,5,7,54,54,54,
54,0,54,54,54,54,- 1,- 1,- 1,3,1,- 1,0,- 1,- 1,2,2,- 1,29,- 1,28,- 1,- 1,- 1,9,7,- 1,7,7,- 1,
8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,- 1,- 1,36,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,34,35};const int Cyc_Lex_lex_default[150]=(const int[150]){34,
9,3,3,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,0,- 1,0,0,- 1,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,0,- 1,0,0,-
1,- 1,0,146,- 1,- 1,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,
0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,101,0,0,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,0,- 1,- 1,0,0,0,0,0,0,146,0,- 1,- 1};const
int Cyc_Lex_lex_trans[1810]=(const int[1810]){0,0,0,0,0,0,0,0,0,35,35,35,35,35,33,
6,103,149,149,149,149,149,147,0,0,0,0,0,147,0,0,148,35,36,37,38,0,39,40,41,149,
142,42,43,7,44,45,46,47,48,48,48,48,48,48,48,48,48,49,4,50,51,52,8,5,53,53,53,53,
53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,17,145,14,54,
55,56,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,
53,10,57,- 1,11,32,32,6,- 1,32,15,30,30,30,30,30,30,30,30,75,72,16,73,20,63,12,66,
- 1,32,59,7,16,143,19,70,66,17,31,31,31,31,31,31,31,31,18,18,18,18,18,18,18,18,32,
32,144,- 1,32,108,31,19,105,71,106,106,106,106,106,106,106,106,106,106,74,33,109,
32,110,7,100,25,107,25,13,101,141,140,103,136,20,104,135,60,134,21,22,23,23,102,
23,22,22,133,132,131,130,129,24,128,112,137,25,25,26,25,27,0,28,29,29,29,29,29,
29,29,29,29,29,23,23,0,0,22,22,58,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,
29,6,- 1,0,24,24,99,99,29,29,29,29,29,29,0,0,0,29,29,29,29,29,29,0,76,0,87,87,87,
87,87,87,87,87,88,88,24,24,99,99,29,29,29,29,29,29,137,78,147,0,0,148,0,0,89,138,
138,138,138,138,138,138,138,90,0,0,91,- 1,82,82,82,82,82,82,82,82,82,82,137,78,0,
0,0,0,0,0,89,139,139,139,139,139,139,139,139,90,0,14,91,76,0,77,77,77,77,77,77,
77,77,77,77,86,86,86,86,86,86,86,86,86,86,0,78,- 1,0,0,0,0,0,79,0,0,0,0,81,0,81,0,
80,82,82,82,82,82,82,82,82,82,82,0,0,0,0,0,78,0,0,0,0,0,0,79,149,149,149,149,149,
0,0,0,80,64,64,64,64,64,64,64,64,64,64,65,0,- 1,0,149,0,0,64,64,64,64,64,64,64,64,
64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,0,0,0,0,64,0,64,64,64,64,
64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,62,62,62,62,62,
62,62,62,62,62,0,0,0,0,0,0,0,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,
62,62,62,62,62,62,62,62,62,0,0,0,0,62,- 1,62,62,62,62,62,62,62,62,62,62,62,62,62,
62,62,62,62,62,62,62,62,62,62,62,62,62,61,61,61,61,61,61,61,61,61,61,61,61,61,61,
61,61,61,61,61,61,61,61,61,61,61,61,0,0,0,0,61,0,61,61,61,61,61,61,61,61,61,61,
61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,0,
0,0,0,0,0,0,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,
61,61,61,0,0,0,0,61,0,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,
61,61,61,61,61,61,61,62,62,62,62,62,62,62,62,62,62,0,0,0,0,0,0,0,62,62,62,62,62,
62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,0,0,0,0,62,0,62,
62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,64,64,
64,64,64,64,64,64,64,64,65,0,0,0,0,0,0,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
64,64,64,64,64,64,64,64,64,64,64,64,0,0,0,0,64,0,64,64,64,64,64,64,64,64,64,64,
64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,67,67,67,67,67,67,67,67,67,67,67,
67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,0,0,0,0,68,0,67,67,67,67,67,67,67,
67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,
67,67,69,0,0,0,0,0,0,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,
67,67,67,67,67,67,0,0,0,0,67,0,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,
67,67,67,67,67,67,67,67,67,67,68,68,68,68,68,68,68,68,68,68,0,0,0,0,0,0,0,68,68,
68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,0,0,0,0,
68,0,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
68,83,83,83,83,83,83,83,83,83,83,0,0,0,0,0,0,0,0,0,0,0,84,27,0,0,0,0,76,27,77,77,
77,77,77,77,77,77,77,77,82,82,82,82,82,82,82,82,82,82,0,78,0,0,84,27,0,0,79,0,0,
27,26,0,0,0,0,80,26,0,83,83,83,83,83,83,83,83,83,83,0,0,0,78,0,0,0,0,0,0,79,84,
27,0,26,0,0,0,27,80,26,85,0,85,0,0,86,86,86,86,86,86,86,86,86,86,0,0,0,0,0,0,0,
84,27,0,0,0,0,0,27,86,86,86,86,86,86,86,86,86,86,76,0,87,87,87,87,87,87,87,87,88,
88,27,0,0,0,0,0,27,0,0,0,0,78,0,0,0,0,0,0,97,0,0,0,0,0,0,0,0,98,0,0,0,0,27,0,0,0,
0,0,27,0,0,0,0,78,0,0,0,0,0,0,97,0,0,0,0,0,0,0,76,98,88,88,88,88,88,88,88,88,88,
88,0,0,0,0,0,0,0,0,0,0,0,78,0,0,0,0,0,0,95,0,0,0,0,0,0,0,0,96,0,0,106,106,106,
106,106,106,106,106,106,106,0,0,0,78,0,0,0,0,0,0,95,84,27,0,0,0,0,0,27,96,92,92,
92,92,92,92,92,92,92,92,0,0,0,0,0,0,0,92,92,92,92,92,92,84,27,0,0,0,0,0,27,0,0,0,
0,0,0,0,92,92,92,92,92,92,92,92,92,92,0,92,92,92,92,92,92,92,92,92,92,92,92,0,0,
0,113,0,93,0,0,114,0,0,0,0,0,94,0,0,115,115,115,115,115,115,115,115,0,92,92,92,
92,92,92,116,0,0,0,0,93,0,0,0,0,0,0,0,0,94,0,0,0,0,0,0,0,0,0,0,0,0,0,0,117,0,0,0,
0,118,119,0,0,0,120,0,0,0,0,0,0,0,121,0,0,0,122,0,123,0,124,0,125,126,126,126,
126,126,126,126,126,126,126,0,0,0,0,0,0,0,126,126,126,126,126,126,126,126,126,
126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,0,0,0,0,0,0,
126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,
126,126,126,126,126,126,127,0,0,0,0,0,0,0,0,126,126,126,126,126,126,126,126,126,
126,0,0,0,0,0,0,0,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,
126,126,126,126,126,126,126,126,126,126,126,0,0,0,0,0,0,126,126,126,126,126,126,
126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0};const int Cyc_Lex_lex_check[1810]=(const int[1810]){- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,11,15,104,35,35,35,35,35,148,- 1,- 1,- 1,- 1,- 1,38,
- 1,- 1,38,0,0,0,0,- 1,0,0,0,35,111,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,4,2,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,39,42,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,3,1,12,12,13,3,12,13,18,18,18,18,18,18,
18,18,49,50,50,50,51,54,1,65,41,12,57,12,13,40,52,52,69,13,30,30,30,30,30,30,30,
30,13,13,13,13,13,13,13,13,32,32,40,41,32,44,43,13,45,70,45,45,45,45,45,45,45,45,
45,45,72,44,44,32,43,32,46,79,105,80,1,46,113,114,101,116,13,101,117,57,118,13,
13,89,90,46,13,93,94,119,120,121,122,123,13,124,41,139,13,79,13,80,13,- 1,13,28,
28,28,28,28,28,28,28,28,28,89,90,- 1,- 1,93,94,0,28,28,28,28,28,28,29,29,29,29,29,
29,29,29,29,29,2,38,- 1,95,96,97,98,29,29,29,29,29,29,- 1,- 1,- 1,28,28,28,28,28,28,
- 1,47,- 1,47,47,47,47,47,47,47,47,47,47,95,96,97,98,29,29,29,29,29,29,115,47,146,
- 1,- 1,146,- 1,- 1,47,115,115,115,115,115,115,115,115,47,- 1,- 1,47,3,81,81,81,81,81,
81,81,81,81,81,138,47,- 1,- 1,- 1,- 1,- 1,- 1,47,138,138,138,138,138,138,138,138,47,- 1,
1,47,48,- 1,48,48,48,48,48,48,48,48,48,48,85,85,85,85,85,85,85,85,85,85,- 1,48,41,
- 1,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,78,- 1,78,- 1,48,78,78,78,78,78,78,78,78,78,78,- 1,- 1,
- 1,- 1,- 1,48,- 1,- 1,- 1,- 1,- 1,- 1,48,149,149,149,149,149,- 1,- 1,- 1,48,53,53,53,53,53,
53,53,53,53,53,53,- 1,101,- 1,149,- 1,- 1,53,53,53,53,53,53,53,53,53,53,53,53,53,53,
53,53,53,53,53,53,53,53,53,53,53,53,- 1,- 1,- 1,- 1,53,- 1,53,53,53,53,53,53,53,53,53,
53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,55,55,55,55,55,55,55,55,55,55,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,55,
55,55,55,55,55,55,- 1,- 1,- 1,- 1,55,146,55,55,55,55,55,55,55,55,55,55,55,55,55,55,
55,55,55,55,55,55,55,55,55,55,55,55,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,
56,56,56,56,56,56,56,56,56,56,56,- 1,- 1,- 1,- 1,56,- 1,56,56,56,56,56,56,56,56,56,56,
56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,61,61,61,61,61,61,61,61,61,61,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,
61,61,61,61,61,- 1,- 1,- 1,- 1,61,- 1,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,
61,61,61,61,61,61,61,61,61,61,62,62,62,62,62,62,62,62,62,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,- 1,
- 1,- 1,- 1,62,- 1,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,
62,62,62,62,64,64,64,64,64,64,64,64,64,64,64,- 1,- 1,- 1,- 1,- 1,- 1,64,64,64,64,64,64,
64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,- 1,- 1,- 1,- 1,64,- 1,64,
64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,66,66,
66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,- 1,- 1,- 1,
- 1,66,- 1,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
66,66,67,67,67,67,67,67,67,67,67,67,67,- 1,- 1,- 1,- 1,- 1,- 1,67,67,67,67,67,67,67,67,
67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,- 1,- 1,- 1,- 1,67,- 1,67,67,67,
67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,67,68,68,68,68,
68,68,68,68,68,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
68,68,68,68,68,68,68,68,68,68,68,68,- 1,- 1,- 1,- 1,68,- 1,68,68,68,68,68,68,68,68,68,
68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,76,76,76,76,76,76,76,76,76,76,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,76,76,- 1,- 1,- 1,- 1,77,76,77,77,77,77,77,77,77,77,
77,77,82,82,82,82,82,82,82,82,82,82,- 1,77,- 1,- 1,76,76,- 1,- 1,77,- 1,- 1,76,82,- 1,- 1,
- 1,- 1,77,82,- 1,83,83,83,83,83,83,83,83,83,83,- 1,- 1,- 1,77,- 1,- 1,- 1,- 1,- 1,- 1,77,83,
83,- 1,82,- 1,- 1,- 1,83,77,82,84,- 1,84,- 1,- 1,84,84,84,84,84,84,84,84,84,84,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,83,83,- 1,- 1,- 1,- 1,- 1,83,86,86,86,86,86,86,86,86,86,86,87,- 1,87,87,87,
87,87,87,87,87,87,87,86,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,
- 1,- 1,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,87,88,88,88,88,88,88,88,88,88,88,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,- 1,- 1,
106,106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,88,106,106,
- 1,- 1,- 1,- 1,- 1,106,88,91,91,91,91,91,91,91,91,91,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,91,91,
91,91,91,91,106,106,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,
92,92,92,- 1,91,91,91,91,91,91,92,92,92,92,92,92,- 1,- 1,- 1,112,- 1,92,- 1,- 1,112,- 1,
- 1,- 1,- 1,- 1,92,- 1,- 1,112,112,112,112,112,112,112,112,- 1,92,92,92,92,92,92,112,- 1,
- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
112,- 1,- 1,- 1,- 1,112,112,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,- 1,- 1,112,- 1,
112,- 1,112,- 1,112,125,125,125,125,125,125,125,125,125,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,- 1,- 1,- 1,- 1,- 1,- 1,125,125,125,125,125,125,125,125,125,
125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,125,126,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,126,126,126,126,126,126,126,126,126,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,
126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,
126,126,126,126,126,- 1,- 1,- 1,- 1,- 1,- 1,126,126,126,126,126,126,126,126,126,126,
126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};int Cyc_Lex_lex_engine(
int start_state,struct Cyc_Lexing_lexbuf*lbuf){int state;int base;int backtrk;int c;
state=start_state;if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{state=(- state)- 1;}while(1){base=Cyc_Lex_lex_base[
_check_known_subscript_notnull(150,state)];if(base < 0)return(- base)- 1;backtrk=
Cyc_Lex_lex_backtrk[_check_known_subscript_notnull(150,state)];if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos
>= lbuf->lex_buffer_len){if(!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}
else{c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),
lbuf->lex_curr_pos ++));if(c == - 1)c=256;}if(Cyc_Lex_lex_check[
_check_known_subscript_notnull(1810,base + c)]== state)state=Cyc_Lex_lex_trans[
_check_known_subscript_notnull(1810,base + c)];else{state=Cyc_Lex_lex_default[
_check_known_subscript_notnull(150,state)];}if(state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({struct Cyc_Lexing_Error_struct
_tmpD0;_tmpD0.tag=Cyc_Lexing_Error;_tmpD0.f1=({const char*_tmpD1="empty token";
_tag_dyneither(_tmpD1,sizeof(char),12);});_tmpD0;});_tmpCF;}));else{return lbuf->lex_last_action;}}
else{if(c == 256)lbuf->lex_eof_reached=0;}}}int Cyc_Lex_token_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL2D: return(int)Cyc_Lex_process_id(lexbuf);case 1: _LL2E: return(int)Cyc_Lex_process_id(
lexbuf);case 2: _LL2F: return(int)Cyc_Lex_process_qual_id(lexbuf);case 3: _LL30: Cyc_Lex_token_string=*
Cyc_Lex_get_symbol(Cyc_Lex_str_index_lbuf(lexbuf));return 357;case 4: _LL31: Cyc_Lex_token_int=
Cyc_Lex_intconst(lexbuf,2,0,16);return 353;case 5: _LL32: Cyc_Lex_token_int=Cyc_Lex_intconst(
lexbuf,0,0,8);return 353;case 6: _LL33: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,
0,10);return 353;case 7: _LL34: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 353;case 8: _LL35: Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf);return 356;case 9: _LL36: Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf);return 356;case 10: _LL37: return 335;case 11: _LL38: return 336;case 12: _LL39:
return 333;case 13: _LL3A: return 334;case 14: _LL3B: return 329;case 15: _LL3C: return 330;
case 16: _LL3D: return 342;case 17: _LL3E: return 343;case 18: _LL3F: return 339;case 19:
_LL40: return 340;case 20: _LL41: return 341;case 21: _LL42: return 348;case 22: _LL43:
return 347;case 23: _LL44: return 346;case 24: _LL45: return 344;case 25: _LL46: return 345;
case 26: _LL47: return 337;case 27: _LL48: return 338;case 28: _LL49: return 331;case 29:
_LL4A: return 332;case 30: _LL4B: return 350;case 31: _LL4C: return 328;case 32: _LL4D:
return 349;case 33: _LL4E: return 351;case 34: _LL4F: return Cyc_Lex_token(lexbuf);case 35:
_LL50: return Cyc_Lex_token(lexbuf);case 36: _LL51: return Cyc_Lex_token(lexbuf);case
37: _LL52: Cyc_Lex_comment_depth=1;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(
lexbuf);Cyc_Lex_comment(lexbuf);return Cyc_Lex_token(lexbuf);case 38: _LL53: Cyc_Lex_string_pos=
0;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);while(Cyc_Lex_strng(
lexbuf)){;}Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 354;case 39: _LL54: Cyc_Lex_token_char='\a';return 355;case 40: _LL55: Cyc_Lex_token_char='\b';
return 355;case 41: _LL56: Cyc_Lex_token_char='\f';return 355;case 42: _LL57: Cyc_Lex_token_char='\n';
return 355;case 43: _LL58: Cyc_Lex_token_char='\r';return 355;case 44: _LL59: Cyc_Lex_token_char='\t';
return 355;case 45: _LL5A: Cyc_Lex_token_char='\v';return 355;case 46: _LL5B: Cyc_Lex_token_char='\\';
return 355;case 47: _LL5C: Cyc_Lex_token_char='\'';return 355;case 48: _LL5D: Cyc_Lex_token_char='"';
return 355;case 49: _LL5E: Cyc_Lex_token_char='?';return 355;case 50: _LL5F: Cyc_Lex_token_char=(
char)(Cyc_Lex_intconst(lexbuf,2,1,8)).f2;return 355;case 51: _LL60: Cyc_Lex_token_char=(
char)(Cyc_Lex_intconst(lexbuf,3,1,16)).f2;return 355;case 52: _LL61: Cyc_Lex_token_char=
Cyc_Lexing_lexeme_char(lexbuf,1);return 355;case 53: _LL62: return - 1;case 54: _LL63:
return(int)Cyc_Lexing_lexeme_char(lexbuf,0);default: _LL64:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_token_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2[0]=({struct Cyc_Lexing_Error_struct
_tmpD3;_tmpD3.tag=Cyc_Lexing_Error;_tmpD3.f1=({const char*_tmpD4="some action didn't return!";
_tag_dyneither(_tmpD4,sizeof(char),27);});_tmpD3;});_tmpD2;}));}int Cyc_Lex_token(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_token_rec(lexbuf,0);}int Cyc_Lex_strng_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL66: return 1;case 1: _LL67: return 0;case 2: _LL68:
return 1;case 3: _LL69: Cyc_Lex_store_string_char('\a');return 1;case 4: _LL6A: Cyc_Lex_store_string_char('\b');
return 1;case 5: _LL6B: Cyc_Lex_store_string_char('\f');return 1;case 6: _LL6C: Cyc_Lex_store_string_char('\n');
return 1;case 7: _LL6D: Cyc_Lex_store_string_char('\r');return 1;case 8: _LL6E: Cyc_Lex_store_string_char('\t');
return 1;case 9: _LL6F: Cyc_Lex_store_string_char('\v');return 1;case 10: _LL70: Cyc_Lex_store_string_char('\\');
return 1;case 11: _LL71: Cyc_Lex_store_string_char('\'');return 1;case 12: _LL72: Cyc_Lex_store_string_char('"');
return 1;case 13: _LL73: Cyc_Lex_store_string_char('?');return 1;case 14: _LL74: Cyc_Lex_store_string_char((
char)(Cyc_Lex_intconst(lexbuf,1,0,8)).f2);return 1;case 15: _LL75: Cyc_Lex_store_string_char((
char)(Cyc_Lex_intconst(lexbuf,2,0,16)).f2);return 1;case 16: _LL76: Cyc_Lex_store_string_char(
Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 17: _LL77: Cyc_Lex_runaway_err(({
const char*_tmpD5="string ends in newline";_tag_dyneither(_tmpD5,sizeof(char),23);}),
lexbuf);return 0;case 18: _LL78: Cyc_Lex_runaway_err(({const char*_tmpD6="unterminated string";
_tag_dyneither(_tmpD6,sizeof(char),20);}),lexbuf);return 0;case 19: _LL79: Cyc_Lex_err(({
const char*_tmpD7="bad character following backslash in string";_tag_dyneither(
_tmpD7,sizeof(char),44);}),lexbuf);return 1;default: _LL7A:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_strng_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=({struct Cyc_Lexing_Error_struct
_tmpD9;_tmpD9.tag=Cyc_Lexing_Error;_tmpD9.f1=({const char*_tmpDA="some action didn't return!";
_tag_dyneither(_tmpDA,sizeof(char),27);});_tmpD9;});_tmpD8;}));}int Cyc_Lex_strng(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_rec(lexbuf,1);}int Cyc_Lex_comment_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL7C: ++ Cyc_Lex_comment_depth;return Cyc_Lex_comment(
lexbuf);case 1: _LL7D: -- Cyc_Lex_comment_depth;if(Cyc_Lex_comment_depth > 0)return
Cyc_Lex_comment(lexbuf);return 0;case 2: _LL7E: Cyc_Lex_runaway_err(({const char*
_tmpDB="unterminated comment";_tag_dyneither(_tmpDB,sizeof(char),21);}),lexbuf);
return 0;case 3: _LL7F: return Cyc_Lex_comment(lexbuf);case 4: _LL80: return Cyc_Lex_comment(
lexbuf);case 5: _LL81: return Cyc_Lex_comment(lexbuf);default: _LL82:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_comment_rec(lexbuf,lexstate);}(int)_throw((void*)({struct
Cyc_Lexing_Error_struct*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC[0]=({struct Cyc_Lexing_Error_struct
_tmpDD;_tmpDD.tag=Cyc_Lexing_Error;_tmpDD.f1=({const char*_tmpDE="some action didn't return!";
_tag_dyneither(_tmpDE,sizeof(char),27);});_tmpDD;});_tmpDC;}));}int Cyc_Lex_comment(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_comment_rec(lexbuf,2);}void Cyc_Lex_lex_init(
int include_cyclone_keywords){if(Cyc_Lex_ids_trie != 0){struct _DynRegionHandle*
_tmpE0;struct Cyc_Lex_DynTrieSymbols _tmpDF=*((struct Cyc_Lex_DynTrieSymbols*)
_check_null(Cyc_Lex_ids_trie));_tmpE0=_tmpDF.dyn;Cyc_Core_free_dynregion(_tmpE0);}
if(Cyc_Lex_typedefs_trie != 0){struct _DynRegionHandle*_tmpE2;struct Cyc_Lex_DynTrie
_tmpE1=*((struct Cyc_Lex_DynTrie*)_check_null(Cyc_Lex_typedefs_trie));_tmpE2=
_tmpE1.dyn;Cyc_Core_free_dynregion(_tmpE2);}{struct _DynRegionHandle*_tmpE4;
struct Cyc_Core_NewRegion _tmpE3=Cyc_Core_new_dynregion();_tmpE4=_tmpE3.dynregion;{
struct _DynRegionFrame _tmpE5;struct _RegionHandle*id_rgn=_open_dynregion(& _tmpE5,
_tmpE4);{struct Cyc_Xarray_Xarray*symbols=((struct Cyc_Xarray_Xarray*(*)(struct
_RegionHandle*,int,struct _dyneither_ptr*))Cyc_Xarray_rcreate)(id_rgn,101,({
struct _dyneither_ptr*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9[0]=(struct
_dyneither_ptr)({const char*_tmpEA="";_tag_dyneither(_tmpEA,sizeof(char),1);});
_tmpE9;}));((void(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add)(
symbols,& Cyc_Lex_bogus_string);Cyc_Lex_ids_trie=({struct Cyc_Lex_DynTrieSymbols*
_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->dyn=(struct _DynRegionHandle*)_tmpE4;
_tmpE6->t=(struct Cyc_Lex_Trie*)({struct Cyc_Lex_Trie*_tmpE7=_region_malloc(id_rgn,
sizeof(*_tmpE7));_tmpE7->children=(union Cyc_Lex_TrieChildren_union)({union Cyc_Lex_TrieChildren_union
_tmpE8;(_tmpE8.Zero).tag=0;_tmpE8;});_tmpE7->shared_str=0;_tmpE7;});_tmpE6->symbols=(
struct Cyc_Xarray_Xarray*)symbols;_tmpE6;});}{struct _DynRegionHandle*_tmpEC;
struct Cyc_Core_NewRegion _tmpEB=Cyc_Core_new_dynregion();_tmpEC=_tmpEB.dynregion;{
struct _DynRegionFrame _tmpED;struct _RegionHandle*typedefs_rgn=_open_dynregion(&
_tmpED,_tmpEC);Cyc_Lex_typedefs_trie=({struct Cyc_Lex_DynTrie*_tmpEE=_cycalloc(
sizeof(*_tmpEE));_tmpEE->dyn=(struct _DynRegionHandle*)_tmpEC;_tmpEE->t=(struct
Cyc_Lex_Trie*)({struct Cyc_Lex_Trie*_tmpEF=_region_malloc(typedefs_rgn,sizeof(*
_tmpEF));_tmpEF->children=(union Cyc_Lex_TrieChildren_union)({union Cyc_Lex_TrieChildren_union
_tmpF0;(_tmpF0.Zero).tag=0;_tmpF0;});_tmpEF->shared_str=0;_tmpEF;});_tmpEE;});
Cyc_Lex_num_kws=Cyc_Lex_num_keywords(include_cyclone_keywords);Cyc_Lex_kw_nums=({
unsigned int _tmpF1=(unsigned int)Cyc_Lex_num_kws;int*_tmpF2=(int*)
_cycalloc_atomic(_check_times(sizeof(int),_tmpF1));struct _dyneither_ptr _tmpF4=
_tag_dyneither(_tmpF2,sizeof(int),_tmpF1);{unsigned int _tmpF3=_tmpF1;
unsigned int i;for(i=0;i < _tmpF3;i ++){_tmpF2[i]=0;}}_tmpF4;});{unsigned int i=0;
unsigned int rwsze=72;{unsigned int j=0;for(0;j < rwsze;++ j){if(
include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)j]).f3){struct _dyneither_ptr
_tmpF5=(Cyc_Lex_rw_array[(int)j]).f1;Cyc_Lex_str_index(_tmpF5,0,(int)Cyc_strlen((
struct _dyneither_ptr)_tmpF5));*((int*)_check_dyneither_subscript(Cyc_Lex_kw_nums,
sizeof(int),(int)i))=(int)(Cyc_Lex_rw_array[(int)j]).f2;++ i;}}}Cyc_Lex_typedef_init();
Cyc_Lex_comment_depth=0;};_pop_dynregion(typedefs_rgn);}};_pop_dynregion(id_rgn);}}}
