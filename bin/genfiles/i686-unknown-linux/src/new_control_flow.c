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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);int
Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[16];
extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct
_dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*,int(*cmp)(void*,void*),
struct Cyc_List_List*a,struct Cyc_List_List*b);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_rzip(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{long
__pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;
struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
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
char*tag;struct _dyneither_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,
void**ans);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict
d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,
void*);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(
struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void**Cyc_Hashtable_lookup_opt(
struct Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dyneither_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};
extern char Cyc_Position_Nocontext[14];int Cyc_Position_error_p();struct
_union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple1{union
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
struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;struct Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct
_tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple3{void*f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct
_union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct
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
;struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*
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
_tuple4{void*f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};
struct _tuple5{void*f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5
val;};struct _tuple6{void*f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6
val;};struct _tuple7{void*f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;
struct _tuple7 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};
struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};struct Cyc_Absyn_VarargCallInfo{int num_varargs;
struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;
unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;
void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct
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
int tag;struct Cyc_List_List*f1;};struct _tuple8{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple8*f1;struct
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
f1;};struct Cyc_Absyn_Tagcheck_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{
struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*loc;void*annot;};
struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct
_tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_struct{
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
struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct
_tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];void*Cyc_Absyn_conref_val(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*
x);int Cyc_Absyn_is_nontagged_union_type(void*);int Cyc_Absyn_is_aggr_type(void*t);
struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
_dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*,void*t);int Cyc_Tcutil_is_noreturn(void*);struct _tuple10{
unsigned int f1;int f2;};struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};
struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_UniquePlace{struct
Cyc_CfFlowInfo_Place place;struct Cyc_List_List*path;};struct
_union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple11{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple11 val;};struct
_union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;};union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*,void*);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessConst(unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(
struct Cyc_Absyn_Vardecl*);struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;
union Cyc_CfFlowInfo_RelnOp rop;};struct Cyc_CfFlowInfo_TagCmp{void*cmp;void*bd;};
extern char Cyc_CfFlowInfo_HasTagCmps[15];struct Cyc_CfFlowInfo_HasTagCmps_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_IsZero[11];extern char
Cyc_CfFlowInfo_NotZero[12];struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct
Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[13];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};union
Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);union
Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;int f1;
struct _dyneither_ptr f2;};int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*
set,void*place,struct Cyc_Position_Segment*loc);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_CfFlowInfo_and_consume(struct _RegionHandle*,struct Cyc_CfFlowInfo_ConsumeInfo
c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);int Cyc_CfFlowInfo_consume_approx(struct
Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);struct
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};struct
_union_FlowInfo_ReachableFL{int tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{
struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL
ReachableFL;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*,struct Cyc_CfFlowInfo_ConsumeInfo);
struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*unknown_none;void*
unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict
mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*
f);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,
void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(union
Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);void Cyc_CfFlowInfo_print_place(
struct Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict
p,void(*pr)(void*));void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(
void*));struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,
struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,void*r);struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,
union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,int);struct _tuple13{
union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,
struct _tuple13 pr2,int);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,
union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo
f,void*rgn);struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;
};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo f);void Cyc_CfFlowInfo_check_unique_rvals(struct
Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo f);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo f);struct _tuple14{struct Cyc_CfFlowInfo_ConsumeInfo
f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple14 Cyc_CfFlowInfo_save_consume_info(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo f,int clear);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_CfFlowInfo_ConsumeInfo
c,int may_consume_only);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*
enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(struct
Cyc_List_List*ds);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_all_effects: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt*encloser;int visited;};static char Cyc_NewControlFlow_CFAnnot[
12]="\000\000\000\000CFAnnot\000";struct Cyc_NewControlFlow_CFAnnot_struct{char*
tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){enclosee->annot=(
void*)({struct Cyc_NewControlFlow_CFAnnot_struct*_tmp0=_cycalloc(sizeof(*_tmp0));
_tmp0[0]=({struct Cyc_NewControlFlow_CFAnnot_struct _tmp1;_tmp1.tag=Cyc_NewControlFlow_CFAnnot;
_tmp1.f1=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp2;_tmp2.encloser=encloser;
_tmp2.visited=0;_tmp2;});_tmp1;});_tmp0;});}struct Cyc_NewControlFlow_AnalEnv{
struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int
iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*
all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_Hashtable_Table*
flow_table;};static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple15{union Cyc_CfFlowInfo_FlowInfo
f1;union Cyc_CfFlowInfo_AbsLVal f2;};static struct _tuple15 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple16{union Cyc_CfFlowInfo_FlowInfo
f1;union Cyc_CfFlowInfo_FlowInfo f2;};static struct _tuple16 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,void*t);static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){void*_tmp3=s->annot;
struct Cyc_NewControlFlow_CFStmtAnnot _tmp4;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp5;_LL1: if(*((void**)_tmp3)!= Cyc_NewControlFlow_CFAnnot)goto _LL3;_tmp4=((
struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((
struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_LL2: return _tmp5;_LL3:;_LL4:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp6=_cycalloc(sizeof(*
_tmp6));_tmp6[0]=({struct Cyc_Core_Impossible_struct _tmp7;_tmp7.tag=Cyc_Core_Impossible;
_tmp7.f1=({const char*_tmp8="ControlFlow -- wrong stmt annotation";_tag_dyneither(
_tmp8,sizeof(char),37);});_tmp7;});_tmp6;}));_LL0:;}static union Cyc_CfFlowInfo_FlowInfo*
Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*
s){union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(
env->flow_table,s);if(sflow == 0){union Cyc_CfFlowInfo_FlowInfo*res=({union Cyc_CfFlowInfo_FlowInfo*
_tmp9=_region_malloc(env->r,sizeof(*_tmp9));_tmp9[0]=Cyc_CfFlowInfo_BottomFL();
_tmp9;});((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*
val))Cyc_Hashtable_insert)(env->flow_table,s,res);return res;}return*sflow;}
struct _tuple17{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*
f2;};static struct _tuple17 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmpA=Cyc_NewControlFlow_get_stmt_annot(s);union Cyc_CfFlowInfo_FlowInfo*_tmpB=
Cyc_NewControlFlow_get_stmt_flow(env,s);*_tmpB=Cyc_CfFlowInfo_join_flow(env->fenv,
env->all_changed,inflow,*_tmpB,1);_tmpA->visited=env->iteration_num;return({
struct _tuple17 _tmpC;_tmpC.f1=_tmpA;_tmpC.f2=_tmpB;_tmpC;});}static void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){if(
env->in_try){env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,
new_flow,env->tryflow,1);{struct Cyc_CfFlowInfo_ConsumeInfo _tmpE;struct Cyc_Dict_Dict
_tmpF;struct Cyc_List_List*_tmp10;struct _tuple14 _tmpD=Cyc_CfFlowInfo_save_consume_info(
env->fenv,env->tryflow,0);_tmpE=_tmpD.f1;_tmpF=_tmpE.consumed;_tmp10=_tmpE.may_consume;
env->tryflow=Cyc_CfFlowInfo_restore_consume_info(env->tryflow,({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp11;_tmp11.consumed=_tmpF;_tmp11.may_consume=0;_tmp11;}),0);}}}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo
flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp12=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmp13=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp14=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,
flow,*_tmp13,1);if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp14,*_tmp13)){*_tmp13=
_tmp14;if(_tmp12->visited == env->iteration_num)env->iterate_again=1;}}static
union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,
struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo _tmp15=inflow;int
_tmp16;struct _tuple12 _tmp17;struct Cyc_Dict_Dict _tmp18;struct Cyc_List_List*_tmp19;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp1A;_LL6: if((_tmp15.BottomFL).tag != 1)goto
_LL8;_tmp16=(int)(_tmp15.BottomFL).val;_LL7: return Cyc_CfFlowInfo_BottomFL();_LL8:
if((_tmp15.ReachableFL).tag != 2)goto _LL5;_tmp17=(struct _tuple12)(_tmp15.ReachableFL).val;
_tmp18=_tmp17.f1;_tmp19=_tmp17.f2;_tmp1A=_tmp17.f3;_LL9: for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp1B=({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp1C=_region_malloc(fenv->r,sizeof(*_tmp1C));_tmp1C[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp1D;});_tmp1C;});
_tmp18=Cyc_Dict_insert(_tmp18,(void*)_tmp1B,Cyc_CfFlowInfo_typ_to_absrval(fenv,((
struct Cyc_Absyn_Vardecl*)vds->hd)->type,leafval));}return Cyc_CfFlowInfo_ReachableFL(
_tmp18,_tmp19,_tmp1A);_LL5:;}struct _tuple18{struct Cyc_CfFlowInfo_Place*f1;struct
Cyc_Position_Segment*f2;};static void Cyc_NewControlFlow_remove_vars(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Dict_Dict*out_unique_places,
struct Cyc_Dict_Dict old_unique_places){struct Cyc_CfFlowInfo_ConsumeInfo _tmp1F;
struct Cyc_Dict_Dict _tmp20;struct _tuple14 _tmp1E=Cyc_CfFlowInfo_save_consume_info(
fenv,outflow,0);_tmp1F=_tmp1E.f1;_tmp20=_tmp1F.consumed;{struct _RegionHandle*
_tmp21=fenv->r;{struct Cyc_Iter_Iter _tmp22=((struct Cyc_Iter_Iter(*)(struct
_RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(_tmp21,*((struct Cyc_Dict_Dict*)
_check_null(out_unique_places)));struct _tuple18 _tmp23=*((struct _tuple18*(*)(
struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(_tmp21,*
out_unique_places);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(
_tmp22,& _tmp23)){struct Cyc_CfFlowInfo_Place*_tmp24=_tmp23.f1;if(!((int(*)(struct
Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp20,_tmp24))({
struct Cyc_String_pa_struct _tmp27;_tmp27.tag=0;_tmp27.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_CfFlowInfo_place_err_string(_tmp24));{void*_tmp25[1]={&
_tmp27};Cyc_Tcutil_terr(_tmp23.f2,({const char*_tmp26="Failed to consume unique variable %s";
_tag_dyneither(_tmp26,sizeof(char),37);}),_tag_dyneither(_tmp25,sizeof(void*),1));}});}}*
out_unique_places=old_unique_places;}}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
inflow,void*r){union Cyc_CfFlowInfo_FlowInfo _tmp28=inflow;int _tmp29;struct
_tuple12 _tmp2A;struct Cyc_Dict_Dict _tmp2B;struct Cyc_List_List*_tmp2C;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2D;_LLB: if((_tmp28.BottomFL).tag != 1)goto _LLD;_tmp29=(int)(_tmp28.BottomFL).val;
_LLC: return Cyc_CfFlowInfo_BottomFL();_LLD: if((_tmp28.ReachableFL).tag != 2)goto
_LLA;_tmp2A=(struct _tuple12)(_tmp28.ReachableFL).val;_tmp2B=_tmp2A.f1;_tmp2C=
_tmp2A.f2;_tmp2D=_tmp2A.f3;_LLE: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2B,r)
!= (void*)2)({void*_tmp2E=0;Cyc_Tcutil_terr(loc,({const char*_tmp2F="expression may not be fully initialized";
_tag_dyneither(_tmp2F,sizeof(char),40);}),_tag_dyneither(_tmp2E,sizeof(void*),0));});{
struct Cyc_Dict_Dict _tmp30=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp2B,env->all_changed,
r);if(_tmp2B.t == _tmp30.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo _tmp31=Cyc_CfFlowInfo_ReachableFL(
_tmp30,_tmp2C,_tmp2D);Cyc_NewControlFlow_update_tryflow(env,_tmp31);return _tmp31;}}
_LLA:;}struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
static struct _tuple19 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*
rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,
struct Cyc_List_List*es,int arg1_unconsumed){if(es == 0)return({struct _tuple19
_tmp32;_tmp32.f1=inflow;_tmp32.f2=0;_tmp32;});if(es->tl == 0){union Cyc_CfFlowInfo_FlowInfo
_tmp34;void*_tmp35;struct _tuple13 _tmp33=Cyc_NewControlFlow_anal_Rexp(env,inflow,(
struct Cyc_Absyn_Exp*)es->hd);_tmp34=_tmp33.f1;_tmp35=_tmp33.f2;return({struct
_tuple19 _tmp36;_tmp36.f1=_tmp34;_tmp36.f2=({struct Cyc_List_List*_tmp37=
_region_malloc(rgn,sizeof(*_tmp37));_tmp37->hd=(void*)_tmp35;_tmp37->tl=0;_tmp37;});
_tmp36;});}{struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;struct Cyc_Dict_Dict
this_all_changed;union Cyc_CfFlowInfo_FlowInfo old_inflow;union Cyc_CfFlowInfo_FlowInfo
outflow;struct Cyc_List_List*rvals;struct Cyc_CfFlowInfo_ConsumeInfo _tmp39;union
Cyc_CfFlowInfo_FlowInfo _tmp3A;struct _tuple14 _tmp38=Cyc_CfFlowInfo_save_consume_info(
env->fenv,inflow,1);_tmp39=_tmp38.f1;_tmp3A=_tmp38.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
outflow_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp5C;_tmp5C.consumed=(env->fenv)->mt_place_set;
_tmp5C.may_consume=0;_tmp5C;});int init_consume=0;do{this_all_changed=(env->fenv)->mt_place_set;
_tmp3A=Cyc_CfFlowInfo_restore_consume_info(_tmp3A,_tmp39,0);env->all_changed=({
struct Cyc_Dict_Dict*_tmp3B=_region_malloc(env->r,sizeof(*_tmp3B));_tmp3B[0]=(env->fenv)->mt_place_set;
_tmp3B;});{union Cyc_CfFlowInfo_FlowInfo _tmp3D;void*_tmp3E;struct _tuple13 _tmp3C=
Cyc_NewControlFlow_anal_Rexp(env,_tmp3A,(struct Cyc_Absyn_Exp*)es->hd);_tmp3D=
_tmp3C.f1;_tmp3E=_tmp3C.f2;outflow=_tmp3D;rvals=({struct Cyc_List_List*_tmp3F=
_region_malloc(rgn,sizeof(*_tmp3F));_tmp3F->hd=(void*)_tmp3E;_tmp3F->tl=0;_tmp3F;});
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)),0);if(arg1_unconsumed){union Cyc_CfFlowInfo_FlowInfo
_tmp40=outflow;int _tmp41;struct _tuple12 _tmp42;struct Cyc_Dict_Dict _tmp43;struct
Cyc_List_List*_tmp44;struct Cyc_CfFlowInfo_ConsumeInfo _tmp45;_LL10: if((_tmp40.BottomFL).tag
!= 1)goto _LL12;_tmp41=(int)(_tmp40.BottomFL).val;_LL11: goto _LLF;_LL12: if((_tmp40.ReachableFL).tag
!= 2)goto _LLF;_tmp42=(struct _tuple12)(_tmp40.ReachableFL).val;_tmp43=_tmp42.f1;
_tmp44=_tmp42.f2;_tmp45=_tmp42.f3;_LL13: outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp43,_tmp44,({struct Cyc_CfFlowInfo_ConsumeInfo _tmp46;_tmp46.consumed=_tmp45.consumed;
_tmp46.may_consume=_tmp39.may_consume;_tmp46;}));_LLF:;}{struct Cyc_List_List*es2=
es->tl;for(0;es2 != 0;es2=es2->tl){env->all_changed=({struct Cyc_Dict_Dict*_tmp47=
_region_malloc(env->r,sizeof(*_tmp47));_tmp47[0]=(env->fenv)->mt_place_set;
_tmp47;});{union Cyc_CfFlowInfo_FlowInfo _tmp49;void*_tmp4A;struct _tuple13 _tmp48=
Cyc_NewControlFlow_anal_Rexp(env,_tmp3A,(struct Cyc_Absyn_Exp*)es2->hd);_tmp49=
_tmp48.f1;_tmp4A=_tmp48.f2;rvals=({struct Cyc_List_List*_tmp4B=_region_malloc(rgn,
sizeof(*_tmp4B));_tmp4B->hd=(void*)_tmp4A;_tmp4B->tl=rvals;_tmp4B;});outflow=Cyc_CfFlowInfo_after_flow(
env->fenv,(struct Cyc_Dict_Dict*)& this_all_changed,outflow,_tmp49,
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)));
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)),0);}}}{struct Cyc_CfFlowInfo_ConsumeInfo _tmp4D;
struct _tuple14 _tmp4C=Cyc_CfFlowInfo_save_consume_info(env->fenv,outflow,0);
_tmp4D=_tmp4C.f1;if(init_consume){if(!Cyc_CfFlowInfo_consume_approx(
outflow_consume,_tmp4D)){({void*_tmp4E=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp4F="sanity consumed: ";_tag_dyneither(_tmp4F,sizeof(char),18);}),
_tag_dyneither(_tmp4E,sizeof(void*),0));});((void(*)(struct Cyc_Dict_Dict p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(outflow_consume.consumed,
Cyc_CfFlowInfo_print_place);({void*_tmp50=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp51="\ncurrent consumed: ";_tag_dyneither(_tmp51,sizeof(char),20);}),
_tag_dyneither(_tmp50,sizeof(void*),0));});((void(*)(struct Cyc_Dict_Dict p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(_tmp4D.consumed,
Cyc_CfFlowInfo_print_place);({void*_tmp52=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp53="\nsanity may_consume: ";_tag_dyneither(_tmp53,sizeof(char),22);}),
_tag_dyneither(_tmp52,sizeof(void*),0));});((void(*)(struct Cyc_List_List*p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(outflow_consume.may_consume,
Cyc_CfFlowInfo_print_place);({void*_tmp54=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp55="\ncurrent may_consume: ";_tag_dyneither(_tmp55,sizeof(char),23);}),
_tag_dyneither(_tmp54,sizeof(void*),0));});((void(*)(struct Cyc_List_List*p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(_tmp4D.may_consume,
Cyc_CfFlowInfo_print_place);({void*_tmp56=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp57="\n";_tag_dyneither(_tmp57,sizeof(char),2);}),_tag_dyneither(_tmp56,
sizeof(void*),0));});({void*_tmp58=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp59="anal_unordered_exps failed to preserve consumed info";
_tag_dyneither(_tmp59,sizeof(char),53);}),_tag_dyneither(_tmp58,sizeof(void*),0));});}
old_inflow=Cyc_CfFlowInfo_restore_consume_info(_tmp3A,outflow_consume,0);}else{
old_inflow=_tmp3A;}init_consume=1;outflow_consume=_tmp4D;_tmp3A=Cyc_CfFlowInfo_join_flow(
env->fenv,outer_all_changed,_tmp3A,outflow,1);}}}while(!Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp3A,old_inflow));if(outer_all_changed == 0)env->all_changed=0;else{env->all_changed=({
struct Cyc_Dict_Dict*_tmp5A=_region_malloc(env->r,sizeof(*_tmp5A));_tmp5A[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0);_tmp5A;});}Cyc_NewControlFlow_update_tryflow(
env,outflow);return({struct _tuple19 _tmp5B;_tmp5B.f1=outflow;_tmp5B.f2=Cyc_List_imp_rev(
rvals);_tmp5B;});}}}static struct _tuple13 Cyc_NewControlFlow_anal_use_ints(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*
es,int arg1_unconsumed){struct _RegionHandle*_tmp5D=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp5F;struct Cyc_List_List*_tmp60;struct _tuple19 _tmp5E=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp5D,env,inflow,es,arg1_unconsumed);_tmp5F=_tmp5E.f1;_tmp60=_tmp5E.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp61=_tmp5F;struct _tuple12 _tmp62;struct Cyc_Dict_Dict
_tmp63;int _tmp64;_LL15: if((_tmp61.ReachableFL).tag != 2)goto _LL17;_tmp62=(struct
_tuple12)(_tmp61.ReachableFL).val;_tmp63=_tmp62.f1;_LL16: for(0;_tmp60 != 0;(
_tmp60=_tmp60->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp63,(void*)_tmp60->hd)== (void*)0)({void*_tmp65=0;Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const
char*_tmp66="expression may not be initialized";_tag_dyneither(_tmp66,sizeof(
char),34);}),_tag_dyneither(_tmp65,sizeof(void*),0));});}goto _LL14;_LL17: if((
_tmp61.BottomFL).tag != 1)goto _LL14;_tmp64=(int)(_tmp61.BottomFL).val;_LL18: goto
_LL14;_LL14:;}return({struct _tuple13 _tmp67;_tmp67.f1=_tmp5F;_tmp67.f2=(env->fenv)->unknown_all;
_tmp67;});}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*e,void*il){union Cyc_CfFlowInfo_FlowInfo _tmp68=
outflow;int _tmp69;struct _tuple12 _tmp6A;struct Cyc_Dict_Dict _tmp6B;struct Cyc_List_List*
_tmp6C;struct Cyc_CfFlowInfo_ConsumeInfo _tmp6D;_LL1A: if((_tmp68.BottomFL).tag != 1)
goto _LL1C;_tmp69=(int)(_tmp68.BottomFL).val;_LL1B: return outflow;_LL1C: if((_tmp68.ReachableFL).tag
!= 2)goto _LL19;_tmp6A=(struct _tuple12)(_tmp68.ReachableFL).val;_tmp6B=_tmp6A.f1;
_tmp6C=_tmp6A.f2;_tmp6D=_tmp6A.f3;_LL1D: {union Cyc_CfFlowInfo_AbsLVal _tmp6E=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;int _tmp6F;struct Cyc_CfFlowInfo_Place*_tmp70;_LL1F: if((_tmp6E.UnknownL).tag
!= 2)goto _LL21;_tmp6F=(int)(_tmp6E.UnknownL).val;_LL20: return outflow;_LL21: if((
_tmp6E.PlaceL).tag != 1)goto _LL1E;_tmp70=(struct Cyc_CfFlowInfo_Place*)(_tmp6E.PlaceL).val;
_LL22: {void*nzval=il == (void*)2?(void*)1:(void*)2;union Cyc_CfFlowInfo_FlowInfo
_tmp71=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,
_tmp6B,env->all_changed,_tmp70,nzval),_tmp6C,_tmp6D);return _tmp71;}_LL1E:;}_LL19:;}
static struct _tuple16 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,
struct Cyc_Absyn_Exp*e,void*il){union Cyc_CfFlowInfo_FlowInfo _tmp72=outflow;int
_tmp73;struct _tuple12 _tmp74;struct Cyc_Dict_Dict _tmp75;struct Cyc_List_List*_tmp76;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp77;_LL24: if((_tmp72.BottomFL).tag != 1)goto
_LL26;_tmp73=(int)(_tmp72.BottomFL).val;_LL25: return({struct _tuple16 _tmp78;
_tmp78.f1=outflow;_tmp78.f2=outflow;_tmp78;});_LL26: if((_tmp72.ReachableFL).tag
!= 2)goto _LL23;_tmp74=(struct _tuple12)(_tmp72.ReachableFL).val;_tmp75=_tmp74.f1;
_tmp76=_tmp74.f2;_tmp77=_tmp74.f3;_LL27: {union Cyc_CfFlowInfo_AbsLVal _tmp79=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;int _tmp7A;struct Cyc_CfFlowInfo_Place*_tmp7B;_LL29: if((_tmp79.UnknownL).tag
!= 2)goto _LL2B;_tmp7A=(int)(_tmp79.UnknownL).val;_LL2A: return({struct _tuple16
_tmp7C;_tmp7C.f1=outflow;_tmp7C.f2=outflow;_tmp7C;});_LL2B: if((_tmp79.PlaceL).tag
!= 1)goto _LL28;_tmp7B=(struct Cyc_CfFlowInfo_Place*)(_tmp79.PlaceL).val;_LL2C: {
void*nzval=il == (void*)2?(void*)1:(void*)2;return({struct _tuple16 _tmp7D;_tmp7D.f1=
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp75,
env->all_changed,_tmp7B,nzval),_tmp76,_tmp77);_tmp7D.f2=Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp75,env->all_changed,_tmp7B,(
void*)0),_tmp76,_tmp77);_tmp7D;});}_LL28:;}_LL23:;}static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_if_tagcmp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
flow,struct Cyc_Absyn_Exp*e1,void*r1,void*p,void*r2){union Cyc_CfFlowInfo_FlowInfo
_tmp7E=flow;int _tmp7F;struct _tuple12 _tmp80;struct Cyc_Dict_Dict _tmp81;struct Cyc_List_List*
_tmp82;struct Cyc_CfFlowInfo_ConsumeInfo _tmp83;_LL2E: if((_tmp7E.BottomFL).tag != 1)
goto _LL30;_tmp7F=(int)(_tmp7E.BottomFL).val;_LL2F: return flow;_LL30: if((_tmp7E.ReachableFL).tag
!= 2)goto _LL2D;_tmp80=(struct _tuple12)(_tmp7E.ReachableFL).val;_tmp81=_tmp80.f1;
_tmp82=_tmp80.f2;_tmp83=_tmp80.f3;_LL31: {void*_tmp84=r2;struct Cyc_List_List*
_tmp85;_LL33: if(_tmp84 <= (void*)3)goto _LL35;if(*((int*)_tmp84)!= 3)goto _LL35;
_tmp85=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp84)->f1;_LL34: {union Cyc_CfFlowInfo_AbsLVal
_tmp86=(Cyc_NewControlFlow_anal_Lexp(env,flow,e1)).f2;int _tmp87;struct Cyc_CfFlowInfo_Place*
_tmp88;_LL38: if((_tmp86.UnknownL).tag != 2)goto _LL3A;_tmp87=(int)(_tmp86.UnknownL).val;
_LL39: return flow;_LL3A: if((_tmp86.PlaceL).tag != 1)goto _LL37;_tmp88=(struct Cyc_CfFlowInfo_Place*)(
_tmp86.PlaceL).val;_LL3B: {struct Cyc_List_List*new_cl;{void*_tmp89=r1;struct Cyc_List_List*
_tmp8A;void*_tmp8B;_LL3D: if(_tmp89 <= (void*)3)goto _LL41;if(*((int*)_tmp89)!= 3)
goto _LL3F;_tmp8A=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp89)->f1;_LL3E: new_cl=
_tmp8A;goto _LL3C;_LL3F: if(*((int*)_tmp89)!= 0)goto _LL41;_tmp8B=(void*)((struct
Cyc_CfFlowInfo_UnknownR_struct*)_tmp89)->f1;if((int)_tmp8B != 2)goto _LL41;_LL40:
goto _LL42;_LL41: if((int)_tmp89 != 0)goto _LL43;_LL42: goto _LL44;_LL43: if((int)
_tmp89 != 1)goto _LL45;_LL44: new_cl=0;goto _LL3C;_LL45:;_LL46: return flow;_LL3C:;}
for(0;_tmp85 != 0;_tmp85=_tmp85->tl){void*new_cmp;{struct _tuple0 _tmp8D=({struct
_tuple0 _tmp8C;_tmp8C.f1=p;_tmp8C.f2=((struct Cyc_CfFlowInfo_TagCmp*)_tmp85->hd)->cmp;
_tmp8C;});void*_tmp8E;void*_tmp8F;void*_tmp90;void*_tmp91;_LL48: _tmp8E=_tmp8D.f1;
if((int)_tmp8E != 8)goto _LL4A;_LL49: goto _LL4B;_LL4A: _tmp8F=_tmp8D.f2;if((int)
_tmp8F != 8)goto _LL4C;_LL4B: new_cmp=(void*)8;goto _LL47;_LL4C: _tmp90=_tmp8D.f1;if((
int)_tmp90 != 5)goto _LL4E;_tmp91=_tmp8D.f2;if((int)_tmp91 != 5)goto _LL4E;_LL4D:
new_cmp=(void*)5;goto _LL47;_LL4E:;_LL4F: new_cmp=(void*)10;goto _LL47;_LL47:;}
new_cl=({struct Cyc_List_List*_tmp92=_region_malloc(env->r,sizeof(*_tmp92));
_tmp92->hd=({struct Cyc_CfFlowInfo_TagCmp*_tmp93=_region_malloc(env->r,sizeof(*
_tmp93));_tmp93->cmp=new_cmp;_tmp93->bd=((struct Cyc_CfFlowInfo_TagCmp*)_tmp85->hd)->bd;
_tmp93;});_tmp92->tl=new_cl;_tmp92;});}return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e1->loc,_tmp81,env->all_changed,_tmp88,(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp94=_region_malloc(env->r,sizeof(*_tmp94));_tmp94[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmp95;_tmp95.tag=3;_tmp95.f1=new_cl;_tmp95;});_tmp94;})),_tmp82,_tmp83);}_LL37:;}
_LL35:;_LL36: return flow;_LL32:;}_LL2D:;}static struct Cyc_CfFlowInfo_NotZero_struct
Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct
Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};static struct
_tuple13 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,
union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*
e,void*r){void*_tmp98=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp99;void*_tmp9A;struct Cyc_Absyn_PtrAtts
_tmp9B;union Cyc_Absyn_Constraint*_tmp9C;union Cyc_Absyn_Constraint*_tmp9D;_LL51:
if(_tmp98 <= (void*)4)goto _LL53;if(*((int*)_tmp98)!= 4)goto _LL53;_tmp99=((struct
Cyc_Absyn_PointerType_struct*)_tmp98)->f1;_tmp9A=_tmp99.elt_typ;_tmp9B=_tmp99.ptr_atts;
_tmp9C=_tmp9B.bounds;_tmp9D=_tmp9B.zero_term;_LL52: {union Cyc_CfFlowInfo_FlowInfo
_tmp9E=f;int _tmp9F;struct _tuple12 _tmpA0;struct Cyc_Dict_Dict _tmpA1;struct Cyc_List_List*
_tmpA2;_LL56: if((_tmp9E.BottomFL).tag != 1)goto _LL58;_tmp9F=(int)(_tmp9E.BottomFL).val;
_LL57: return({struct _tuple13 _tmpA3;_tmpA3.f1=f;_tmpA3.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp9A,(env->fenv)->unknown_all);_tmpA3;});_LL58: if((_tmp9E.ReachableFL).tag
!= 2)goto _LL55;_tmpA0=(struct _tuple12)(_tmp9E.ReachableFL).val;_tmpA1=_tmpA0.f1;
_tmpA2=_tmpA0.f2;_LL59: if(Cyc_Tcutil_is_bound_one(_tmp9C)){void*_tmpA4=r;struct
Cyc_CfFlowInfo_Place*_tmpA5;void*_tmpA6;_LL5B: if((int)_tmpA4 != 1)goto _LL5D;_LL5C:
goto _LL5E;_LL5D: if((int)_tmpA4 != 2)goto _LL5F;_LL5E:{void*_tmpA7=e->annot;struct
Cyc_List_List*_tmpA8;_LL68: if(*((void**)_tmpA7)!= Cyc_CfFlowInfo_NotZero)goto
_LL6A;_tmpA8=((struct Cyc_CfFlowInfo_NotZero_struct*)_tmpA7)->f1;_LL69: if(!Cyc_CfFlowInfo_same_relns(
_tmpA2,_tmpA8))goto _LL6B;goto _LL67;_LL6A:;_LL6B:{void*_tmpA9=e->r;_LL6D: if(*((
int*)_tmpA9)!= 25)goto _LL6F;_LL6E: e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmpAB;_tmpAB.tag=Cyc_CfFlowInfo_NotZero;_tmpAB.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpA2);_tmpAB;});_tmpAA;});goto _LL6C;_LL6F:;_LL70: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL6C;_LL6C:;}goto _LL67;_LL67:;}goto _LL5A;_LL5F: if(_tmpA4 <= (void*)3)goto
_LL61;if(*((int*)_tmpA4)!= 2)goto _LL61;_tmpA5=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpA4)->f1;_LL60:{void*_tmpAC=e->annot;struct Cyc_List_List*_tmpAD;_LL72: if(*((
void**)_tmpAC)!= Cyc_CfFlowInfo_NotZero)goto _LL74;_tmpAD=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpAC)->f1;_LL73: if(!Cyc_CfFlowInfo_same_relns(_tmpA2,_tmpAD))goto _LL75;goto
_LL71;_LL74:;_LL75:{void*_tmpAE=e->r;_LL77: if(*((int*)_tmpAE)!= 25)goto _LL79;
_LL78: e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmpAF=_cycalloc(
sizeof(*_tmpAF));_tmpAF[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmpB0;_tmpB0.tag=
Cyc_CfFlowInfo_NotZero;_tmpB0.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpA2);_tmpB0;});_tmpAF;});goto _LL76;_LL79:;_LL7A: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL76;_LL76:;}goto _LL71;_LL71:;}return({struct _tuple13 _tmpB1;_tmpB1.f1=f;
_tmpB1.f2=Cyc_CfFlowInfo_lookup_place(_tmpA1,_tmpA5);_tmpB1;});_LL61: if((int)
_tmpA4 != 0)goto _LL63;_LL62: e->annot=(void*)Cyc_CfFlowInfo_IsZero;return({struct
_tuple13 _tmpB2;_tmpB2.f1=Cyc_CfFlowInfo_BottomFL();_tmpB2.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp9A,(env->fenv)->unknown_all);_tmpB2;});_LL63: if(_tmpA4 <= (void*)3)
goto _LL65;if(*((int*)_tmpA4)!= 0)goto _LL65;_tmpA6=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpA4)->f1;_LL64: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpA6);goto _LL66;
_LL65:;_LL66:{void*_tmpB3=e->r;_LL7C: if(*((int*)_tmpB3)!= 25)goto _LL7E;_LL7D: e->annot=(
void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));
_tmpB4[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmpB5;_tmpB5.tag=Cyc_CfFlowInfo_UnknownZ;
_tmpB5.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpA2);_tmpB5;});
_tmpB4;});goto _LL7B;_LL7E:;_LL7F: e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL7B;_LL7B:;}goto _LL5A;_LL5A:;}else{void*_tmpB6=e->annot;struct Cyc_List_List*
_tmpB7;_LL81: if(*((void**)_tmpB6)!= Cyc_CfFlowInfo_UnknownZ)goto _LL83;_tmpB7=((
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmpB6)->f1;_LL82: if(!Cyc_CfFlowInfo_same_relns(
_tmpA2,_tmpB7))goto _LL84;goto _LL80;_LL83:;_LL84: e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmpB9;_tmpB9.tag=Cyc_CfFlowInfo_UnknownZ;_tmpB9.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmpA2);_tmpB9;});_tmpB8;});goto _LL80;_LL80:;}{void*_tmpBA=
Cyc_CfFlowInfo_initlevel(env->fenv,_tmpA1,r);_LL86: if((int)_tmpBA != 0)goto _LL88;
_LL87:({void*_tmpBB=0;Cyc_Tcutil_terr(e->loc,({const char*_tmpBC="dereference of possibly uninitialized pointer";
_tag_dyneither(_tmpBC,sizeof(char),46);}),_tag_dyneither(_tmpBB,sizeof(void*),0));});
goto _LL89;_LL88: if((int)_tmpBA != 2)goto _LL8A;_LL89: return({struct _tuple13 _tmpBD;
_tmpBD.f1=f;_tmpBD.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp9A,(env->fenv)->unknown_all);
_tmpBD;});_LL8A: if((int)_tmpBA != 1)goto _LL85;_LL8B: return({struct _tuple13 _tmpBE;
_tmpBE.f1=f;_tmpBE.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp9A,(env->fenv)->unknown_none);
_tmpBE;});_LL85:;}_LL55:;}_LL53:;_LL54:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Core_Impossible_struct
_tmpC0;_tmpC0.tag=Cyc_Core_Impossible;_tmpC0.f1=({const char*_tmpC1="right deref of non-pointer-type";
_tag_dyneither(_tmpC1,sizeof(char),32);});_tmpC0;});_tmpBF;}));_LL50:;}static
struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*
rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmpC2=e1->r;void*_tmpC3;struct Cyc_Absyn_Vardecl*_tmpC4;void*_tmpC5;struct
Cyc_Absyn_Vardecl*_tmpC6;void*_tmpC7;struct Cyc_Absyn_Vardecl*_tmpC8;void*_tmpC9;
struct Cyc_Absyn_Vardecl*_tmpCA;_LL8D: if(*((int*)_tmpC2)!= 1)goto _LL8F;_tmpC3=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmpC2)->f2;if(_tmpC3 <= (void*)1)goto _LL8F;
if(*((int*)_tmpC3)!= 4)goto _LL8F;_tmpC4=((struct Cyc_Absyn_Pat_b_struct*)_tmpC3)->f1;
_LL8E: _tmpC6=_tmpC4;goto _LL90;_LL8F: if(*((int*)_tmpC2)!= 1)goto _LL91;_tmpC5=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmpC2)->f2;if(_tmpC5 <= (void*)1)goto _LL91;
if(*((int*)_tmpC5)!= 3)goto _LL91;_tmpC6=((struct Cyc_Absyn_Local_b_struct*)_tmpC5)->f1;
_LL90: _tmpC8=_tmpC6;goto _LL92;_LL91: if(*((int*)_tmpC2)!= 1)goto _LL93;_tmpC7=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmpC2)->f2;if(_tmpC7 <= (void*)1)goto _LL93;
if(*((int*)_tmpC7)!= 2)goto _LL93;_tmpC8=((struct Cyc_Absyn_Param_b_struct*)_tmpC7)->f1;
_LL92: _tmpCA=_tmpC8;goto _LL94;_LL93: if(*((int*)_tmpC2)!= 1)goto _LL95;_tmpC9=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmpC2)->f2;if(_tmpC9 <= (void*)1)goto _LL95;
if(*((int*)_tmpC9)!= 0)goto _LL95;_tmpCA=((struct Cyc_Absyn_Global_b_struct*)
_tmpC9)->f1;_LL94: if(!_tmpCA->escapes){void*_tmpCB=e2->r;void*_tmpCC;struct Cyc_Absyn_Vardecl*
_tmpCD;void*_tmpCE;struct Cyc_Absyn_Vardecl*_tmpCF;void*_tmpD0;struct Cyc_Absyn_Vardecl*
_tmpD1;void*_tmpD2;struct Cyc_Absyn_Vardecl*_tmpD3;_LL98: if(*((int*)_tmpCB)!= 1)
goto _LL9A;_tmpCC=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpCB)->f2;if(_tmpCC <= (
void*)1)goto _LL9A;if(*((int*)_tmpCC)!= 4)goto _LL9A;_tmpCD=((struct Cyc_Absyn_Pat_b_struct*)
_tmpCC)->f1;_LL99: _tmpCF=_tmpCD;goto _LL9B;_LL9A: if(*((int*)_tmpCB)!= 1)goto _LL9C;
_tmpCE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpCB)->f2;if(_tmpCE <= (void*)1)
goto _LL9C;if(*((int*)_tmpCE)!= 3)goto _LL9C;_tmpCF=((struct Cyc_Absyn_Local_b_struct*)
_tmpCE)->f1;_LL9B: _tmpD1=_tmpCF;goto _LL9D;_LL9C: if(*((int*)_tmpCB)!= 1)goto _LL9E;
_tmpD0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpCB)->f2;if(_tmpD0 <= (void*)1)
goto _LL9E;if(*((int*)_tmpD0)!= 2)goto _LL9E;_tmpD1=((struct Cyc_Absyn_Param_b_struct*)
_tmpD0)->f1;_LL9D: _tmpD3=_tmpD1;goto _LL9F;_LL9E: if(*((int*)_tmpCB)!= 1)goto _LLA0;
_tmpD2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpCB)->f2;if(_tmpD2 <= (void*)1)
goto _LLA0;if(*((int*)_tmpD2)!= 0)goto _LLA0;_tmpD3=((struct Cyc_Absyn_Global_b_struct*)
_tmpD2)->f1;_LL9F: if(!_tmpD3->escapes){int found=0;{struct Cyc_List_List*_tmpD4=
relns;for(0;_tmpD4 != 0;_tmpD4=_tmpD4->tl){struct Cyc_CfFlowInfo_Reln*_tmpD5=(
struct Cyc_CfFlowInfo_Reln*)_tmpD4->hd;if(_tmpD5->vd == _tmpD3){union Cyc_CfFlowInfo_RelnOp
_tmpD6=_tmpD5->rop;struct Cyc_Absyn_Vardecl*_tmpD7;_LLA3: if((_tmpD6.LessNumelts).tag
!= 3)goto _LLA5;_tmpD7=(struct Cyc_Absyn_Vardecl*)(_tmpD6.LessNumelts).val;if(!(
_tmpD7 == _tmpCA))goto _LLA5;_LLA4: return relns;_LLA5:;_LLA6: continue;_LLA2:;}}}if(
!found)return({struct Cyc_List_List*_tmpD8=_region_malloc(rgn,sizeof(*_tmpD8));
_tmpD8->hd=({struct Cyc_CfFlowInfo_Reln*_tmpD9=_region_malloc(rgn,sizeof(*_tmpD9));
_tmpD9->vd=_tmpD3;_tmpD9->rop=Cyc_CfFlowInfo_LessNumelts(_tmpCA);_tmpD9;});
_tmpD8->tl=relns;_tmpD8;});}return relns;_LLA0:;_LLA1: return relns;_LL97:;}else{
return relns;}_LL95:;_LL96: return relns;_LL8C:;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_may_consume_place(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_CfFlowInfo_Place*place,void*t,struct
Cyc_List_List**ps){struct Cyc_CfFlowInfo_FlowEnv*_tmpDA=env->fenv;struct Cyc_List_List*
_tmpDB=Cyc_NewControlFlow_noalias_ptrs_rec(env,place,t);if(ps != 0)*ps=_tmpDB;if(
_tmpDB == 0)return cinfo;cinfo.may_consume=((struct Cyc_List_List*(*)(struct
_RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(_tmpDA->r,Cyc_CfFlowInfo_place_cmp,
_tmpDB,cinfo.may_consume);return cinfo;}struct _tuple20{union Cyc_CfFlowInfo_AbsLVal
f1;union Cyc_CfFlowInfo_FlowInfo f2;};static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_may_consume_exp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e){struct _RegionHandle*_tmpDC=env->r;if(Cyc_Tcutil_is_noalias_path(
_tmpDC,e)){struct Cyc_CfFlowInfo_FlowEnv*_tmpDD=env->fenv;union Cyc_CfFlowInfo_AbsLVal
_tmpDF;struct _tuple15 _tmpDE=Cyc_NewControlFlow_anal_Lexp(env,inflow,e);_tmpDF=
_tmpDE.f2;{struct _tuple20 _tmpE1=({struct _tuple20 _tmpE0;_tmpE0.f1=_tmpDF;_tmpE0.f2=
inflow;_tmpE0;});union Cyc_CfFlowInfo_AbsLVal _tmpE2;struct Cyc_CfFlowInfo_Place*
_tmpE3;union Cyc_CfFlowInfo_FlowInfo _tmpE4;struct _tuple12 _tmpE5;struct Cyc_Dict_Dict
_tmpE6;struct Cyc_List_List*_tmpE7;struct Cyc_CfFlowInfo_ConsumeInfo _tmpE8;_LLA8:
_tmpE2=_tmpE1.f1;if((_tmpE2.PlaceL).tag != 1)goto _LLAA;_tmpE3=(struct Cyc_CfFlowInfo_Place*)(
_tmpE2.PlaceL).val;_tmpE4=_tmpE1.f2;if((_tmpE4.ReachableFL).tag != 2)goto _LLAA;
_tmpE5=(struct _tuple12)(_tmpE4.ReachableFL).val;_tmpE6=_tmpE5.f1;_tmpE7=_tmpE5.f2;
_tmpE8=_tmpE5.f3;_LLA9: {struct Cyc_CfFlowInfo_ConsumeInfo _tmpE9=Cyc_NewControlFlow_may_consume_place(
env,_tmpE8,_tmpE3,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0);
return Cyc_CfFlowInfo_ReachableFL(_tmpE6,_tmpE7,_tmpE9);}_LLAA:;_LLAB:({struct Cyc_String_pa_struct
_tmpEC;_tmpEC.tag=0;_tmpEC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmpEA[1]={& _tmpEC};Cyc_fprintf(Cyc_stderr,({const char*_tmpEB="Oops---no location for noalias_path |%s|\n";
_tag_dyneither(_tmpEB,sizeof(char),42);}),_tag_dyneither(_tmpEA,sizeof(void*),1));}});
return inflow;_LLA7:;}}return inflow;}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_consume_place(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo
cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Position_Segment*loc){if(!((int(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(cinfo.consumed,
p)){struct Cyc_CfFlowInfo_Place _tmpEE;void*_tmpEF;struct Cyc_List_List*_tmpF0;
struct Cyc_CfFlowInfo_Place*_tmpED=p;_tmpEE=*_tmpED;_tmpEF=_tmpEE.root;_tmpF0=
_tmpEE.fields;{void*rval=(void*)0;if((Cyc_Dict_lookup_bool(outdict,_tmpEF,& rval)
 && Cyc_CfFlowInfo_initlevel(env->fenv,outdict,rval)!= (void*)0) && rval != (void*)
0){void*_tmpF1=_tmpEF;struct Cyc_Absyn_Vardecl*_tmpF2;_LLAD: if(*((int*)_tmpF1)!= 
0)goto _LLAF;_tmpF2=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmpF1)->f1;_LLAE: {
struct _dyneither_ptr _tmpF3=Cyc_Absynpp_qvar2string(_tmpF2->name);if(_tmpF0 == 0)({
struct Cyc_String_pa_struct _tmpF6;_tmpF6.tag=0;_tmpF6.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmpF3);{void*_tmpF4[1]={& _tmpF6};Cyc_Tcutil_warn(loc,({
const char*_tmpF5="may clobber unique pointer %s";_tag_dyneither(_tmpF5,sizeof(
char),30);}),_tag_dyneither(_tmpF4,sizeof(void*),1));}});else{({struct Cyc_String_pa_struct
_tmpF9;_tmpF9.tag=0;_tmpF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmpF3);{void*_tmpF7[1]={& _tmpF9};Cyc_Tcutil_warn(loc,({const char*_tmpF8="may clobber unique pointer contained in %s";
_tag_dyneither(_tmpF8,sizeof(char),43);}),_tag_dyneither(_tmpF7,sizeof(void*),1));}});}
goto _LLAC;}_LLAF:;_LLB0:({void*_tmpFA=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpFB="warning locations not for VarRoots";
_tag_dyneither(_tmpFB,sizeof(char),35);}),_tag_dyneither(_tmpFA,sizeof(void*),0));});
_LLAC:;}}}if(env->all_changed == 0)cinfo.consumed=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_rdelete)((
env->fenv)->r,cinfo.consumed,p);return cinfo;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_consume_assignment(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict
outdict,struct Cyc_Absyn_Exp*e){struct _RegionHandle*_tmpFC=env->r;if(Cyc_Tcutil_is_noalias_path(
_tmpFC,e)){struct Cyc_CfFlowInfo_FlowEnv*_tmpFD=env->fenv;struct Cyc_List_List*
_tmpFE=0;cinfo=Cyc_NewControlFlow_may_consume_place(env,cinfo,p,(void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v,(struct Cyc_List_List**)& _tmpFE);while(
_tmpFE != 0){cinfo=Cyc_NewControlFlow_consume_place(env,(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmpFE))->hd,cinfo,outdict,e->loc);_tmpFE=((
struct Cyc_List_List*)_check_null(_tmpFE))->tl;}}return cinfo;}static struct Cyc_List_List*
Cyc_NewControlFlow_noalias_ptrs_aux(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*ts){struct Cyc_List_List*l=0;struct Cyc_CfFlowInfo_Place
_tmp100;void*_tmp101;struct Cyc_List_List*_tmp102;struct Cyc_CfFlowInfo_Place*
_tmpFF=p;_tmp100=*_tmpFF;_tmp101=_tmp100.root;_tmp102=_tmp100.fields;{int fld=0;
for(0;ts != 0;(fld ++,ts=ts->tl)){void*_tmp103=(void*)ts->hd;if(Cyc_Tcutil_is_noalias_pointer(
_tmp103)){struct Cyc_List_List*_tmp104=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
env->r,_tmp102,({int _tmp107[1];_tmp107[0]=fld;((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(
_tmp107,sizeof(int),1));}));l=((struct Cyc_List_List*(*)(struct _RegionHandle*,int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmp105=_region_malloc(env->r,sizeof(*_tmp105));_tmp105->hd=({
struct Cyc_CfFlowInfo_Place*_tmp106=_region_malloc(env->r,sizeof(*_tmp106));
_tmp106->root=_tmp101;_tmp106->fields=_tmp104;_tmp106;});_tmp105->tl=0;_tmp105;}),
l);}else{if(Cyc_Absyn_is_aggr_type(_tmp103)){struct Cyc_List_List*_tmp108=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(env->r,_tmp102,({int _tmp10B[1];_tmp10B[0]=fld;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(
_tmp10B,sizeof(int),1));}));struct Cyc_CfFlowInfo_Place*_tmp109=({struct Cyc_CfFlowInfo_Place*
_tmp10A=_region_malloc(env->r,sizeof(*_tmp10A));_tmp10A->root=_tmp101;_tmp10A->fields=
_tmp108;_tmp10A;});l=((struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,
struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,l,Cyc_NewControlFlow_noalias_ptrs_rec(
env,_tmp109,_tmp103));}}}}return l;}struct _tuple21{struct Cyc_Absyn_Tqual f1;void*
f2;};static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,void*t){if(Cyc_Tcutil_is_noalias_pointer(t))
return({struct Cyc_List_List*_tmp10C=_region_malloc(env->r,sizeof(*_tmp10C));
_tmp10C->hd=p;_tmp10C->tl=0;_tmp10C;});{void*_tmp10D=Cyc_Tcutil_compress(t);
struct Cyc_List_List*_tmp10E;struct Cyc_List_List*_tmp10F;struct Cyc_Absyn_AggrInfo
_tmp110;union Cyc_Absyn_AggrInfoU _tmp111;struct Cyc_Absyn_Aggrdecl**_tmp112;struct
Cyc_List_List*_tmp113;struct Cyc_Absyn_AggrInfo _tmp114;union Cyc_Absyn_AggrInfoU
_tmp115;struct _tuple3 _tmp116;struct Cyc_Absyn_DatatypeFieldInfo _tmp117;union Cyc_Absyn_DatatypeFieldInfoU
_tmp118;struct Cyc_List_List*_tmp119;_LLB2: if(_tmp10D <= (void*)4)goto _LLBE;if(*((
int*)_tmp10D)!= 9)goto _LLB4;_tmp10E=((struct Cyc_Absyn_TupleType_struct*)_tmp10D)->f1;
_LLB3: {struct Cyc_List_List*_tmp11A=0;while(_tmp10E != 0){_tmp11A=({struct Cyc_List_List*
_tmp11B=_region_malloc(env->r,sizeof(*_tmp11B));_tmp11B->hd=(void*)(*((struct
_tuple21*)_tmp10E->hd)).f2;_tmp11B->tl=_tmp11A;_tmp11B;});_tmp10E=_tmp10E->tl;}
_tmp11A=Cyc_List_imp_rev(_tmp11A);return Cyc_NewControlFlow_noalias_ptrs_aux(env,
p,_tmp11A);}_LLB4: if(*((int*)_tmp10D)!= 11)goto _LLB6;_tmp10F=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp10D)->f2;_LLB5: {struct Cyc_List_List*_tmp11C=0;while(_tmp10F != 0){_tmp11C=({
struct Cyc_List_List*_tmp11D=_region_malloc(env->r,sizeof(*_tmp11D));_tmp11D->hd=(
void*)((struct Cyc_Absyn_Aggrfield*)_tmp10F->hd)->type;_tmp11D->tl=_tmp11C;
_tmp11D;});_tmp10F=_tmp10F->tl;}_tmp11C=Cyc_List_imp_rev(_tmp11C);return Cyc_NewControlFlow_noalias_ptrs_aux(
env,p,_tmp11C);}_LLB6: if(*((int*)_tmp10D)!= 10)goto _LLB8;_tmp110=((struct Cyc_Absyn_AggrType_struct*)
_tmp10D)->f1;_tmp111=_tmp110.aggr_info;if((_tmp111.KnownAggr).tag != 2)goto _LLB8;
_tmp112=(struct Cyc_Absyn_Aggrdecl**)(_tmp111.KnownAggr).val;_tmp113=_tmp110.targs;
_LLB7: if((*_tmp112)->impl == 0)return 0;else{struct Cyc_List_List*_tmp11E=0;struct
_RegionHandle*_tmp11F=env->r;{struct Cyc_List_List*_tmp120=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(_tmp11F,_tmp11F,(*_tmp112)->tvs,_tmp113);struct Cyc_List_List*
_tmp121=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp112)->impl))->fields;
while(_tmp121 != 0){_tmp11E=({struct Cyc_List_List*_tmp122=_region_malloc(env->r,
sizeof(*_tmp122));_tmp122->hd=(void*)Cyc_Tcutil_rsubstitute(_tmp11F,_tmp120,((
struct Cyc_Absyn_Aggrfield*)_tmp121->hd)->type);_tmp122->tl=_tmp11E;_tmp122;});
_tmp121=_tmp121->tl;}}_tmp11E=Cyc_List_imp_rev(_tmp11E);return Cyc_NewControlFlow_noalias_ptrs_aux(
env,p,_tmp11E);}_LLB8: if(*((int*)_tmp10D)!= 10)goto _LLBA;_tmp114=((struct Cyc_Absyn_AggrType_struct*)
_tmp10D)->f1;_tmp115=_tmp114.aggr_info;if((_tmp115.UnknownAggr).tag != 1)goto
_LLBA;_tmp116=(struct _tuple3)(_tmp115.UnknownAggr).val;_LLB9:({void*_tmp123=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp124="got unknown aggr in noalias_ptrs_rec";_tag_dyneither(_tmp124,
sizeof(char),37);}),_tag_dyneither(_tmp123,sizeof(void*),0));});_LLBA: if(*((int*)
_tmp10D)!= 2)goto _LLBC;_LLBB: {struct _RegionHandle*_tmp125=env->r;if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp125,t))return({struct Cyc_List_List*_tmp126=_region_malloc(env->r,sizeof(*
_tmp126));_tmp126->hd=p;_tmp126->tl=0;_tmp126;});else{return 0;}}_LLBC: if(*((int*)
_tmp10D)!= 3)goto _LLBE;_tmp117=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp10D)->f1;_tmp118=_tmp117.field_info;_tmp119=_tmp117.targs;_LLBD: {union Cyc_Absyn_DatatypeFieldInfoU
_tmp127=_tmp118;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp128;struct _tuple2
_tmp129;struct Cyc_Absyn_Datatypedecl*_tmp12A;struct Cyc_Absyn_Datatypefield*
_tmp12B;_LLC1: if((_tmp127.UnknownDatatypefield).tag != 1)goto _LLC3;_tmp128=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp127.UnknownDatatypefield).val;
_LLC2:({void*_tmp12C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp12D="got unknown datatype field in noalias_ptrs_rec";
_tag_dyneither(_tmp12D,sizeof(char),47);}),_tag_dyneither(_tmp12C,sizeof(void*),
0));});_LLC3: if((_tmp127.KnownDatatypefield).tag != 2)goto _LLC0;_tmp129=(struct
_tuple2)(_tmp127.KnownDatatypefield).val;_tmp12A=_tmp129.f1;_tmp12B=_tmp129.f2;
_LLC4: {struct Cyc_List_List*_tmp12E=0;struct _RegionHandle*_tmp12F=env->r;{struct
Cyc_List_List*_tmp130=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp12F,_tmp12F,_tmp12A->tvs,_tmp119);struct Cyc_List_List*_tmp131=_tmp12B->typs;
while(_tmp131 != 0){_tmp12E=({struct Cyc_List_List*_tmp132=_region_malloc(env->r,
sizeof(*_tmp132));_tmp132->hd=(void*)Cyc_Tcutil_rsubstitute(_tmp12F,_tmp130,(*((
struct _tuple21*)_tmp131->hd)).f2);_tmp132->tl=_tmp12E;_tmp132;});_tmp131=_tmp131->tl;}}
_tmp12E=Cyc_List_imp_rev(_tmp12E);return Cyc_NewControlFlow_noalias_ptrs_aux(env,
p,_tmp12E);}_LLC0:;}_LLBE:;_LLBF: return 0;_LLB1:;}}static struct _tuple13 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_CfFlowInfo_ConsumeInfo outer_cinfo,struct Cyc_Position_Segment*
loc){outflow=Cyc_CfFlowInfo_consume_unique_rvals(loc,outflow);{union Cyc_CfFlowInfo_FlowInfo
_tmp133=outflow;int _tmp134;struct _tuple12 _tmp135;struct Cyc_Dict_Dict _tmp136;
struct Cyc_List_List*_tmp137;struct Cyc_CfFlowInfo_ConsumeInfo _tmp138;_LLC6: if((
_tmp133.BottomFL).tag != 1)goto _LLC8;_tmp134=(int)(_tmp133.BottomFL).val;_LLC7:
return({struct _tuple13 _tmp139;_tmp139.f1=Cyc_CfFlowInfo_BottomFL();_tmp139.f2=
rval;_tmp139;});_LLC8: if((_tmp133.ReachableFL).tag != 2)goto _LLC5;_tmp135=(struct
_tuple12)(_tmp133.ReachableFL).val;_tmp136=_tmp135.f1;_tmp137=_tmp135.f2;_tmp138=
_tmp135.f3;_LLC9: _tmp138=Cyc_CfFlowInfo_and_consume(fenv->r,outer_cinfo,_tmp138);{
union Cyc_CfFlowInfo_AbsLVal _tmp13A=lval;struct Cyc_CfFlowInfo_Place*_tmp13B;int
_tmp13C;_LLCB: if((_tmp13A.PlaceL).tag != 1)goto _LLCD;_tmp13B=(struct Cyc_CfFlowInfo_Place*)(
_tmp13A.PlaceL).val;_LLCC: _tmp138=Cyc_NewControlFlow_consume_assignment(env,
_tmp13B,_tmp138,_tmp136,lexp);_tmp136=Cyc_CfFlowInfo_assign_place(fenv,loc,
_tmp136,env->all_changed,_tmp13B,rval);_tmp137=Cyc_CfFlowInfo_reln_assign_exp(
fenv->r,_tmp137,lexp,rexp);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp136,_tmp137,
_tmp138);Cyc_NewControlFlow_update_tryflow(env,outflow);return({struct _tuple13
_tmp13D;_tmp13D.f1=outflow;_tmp13D.f2=rval;_tmp13D;});_LLCD: if((_tmp13A.UnknownL).tag
!= 2)goto _LLCA;_tmp13C=(int)(_tmp13A.UnknownL).val;_LLCE: return({struct _tuple13
_tmp13E;_tmp13E.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval);
_tmp13E.f2=rval;_tmp13E;});_LLCA:;}_LLC5:;}}struct _tuple22{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp13F=env->fenv;struct Cyc_Dict_Dict
d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo cinfo;{union Cyc_CfFlowInfo_FlowInfo
_tmp140=inflow;int _tmp141;struct _tuple12 _tmp142;struct Cyc_Dict_Dict _tmp143;
struct Cyc_List_List*_tmp144;struct Cyc_CfFlowInfo_ConsumeInfo _tmp145;_LLD0: if((
_tmp140.BottomFL).tag != 1)goto _LLD2;_tmp141=(int)(_tmp140.BottomFL).val;_LLD1:
return({struct _tuple13 _tmp146;_tmp146.f1=Cyc_CfFlowInfo_BottomFL();_tmp146.f2=
_tmp13F->unknown_all;_tmp146;});_LLD2: if((_tmp140.ReachableFL).tag != 2)goto _LLCF;
_tmp142=(struct _tuple12)(_tmp140.ReachableFL).val;_tmp143=_tmp142.f1;_tmp144=
_tmp142.f2;_tmp145=_tmp142.f3;_LLD3: d=_tmp143;relns=_tmp144;cinfo=_tmp145;_LLCF:;}{
void*_tmp147=e->r;struct Cyc_Absyn_Exp*_tmp148;void*_tmp149;struct Cyc_Absyn_Exp*
_tmp14A;struct Cyc_Absyn_Exp*_tmp14B;struct Cyc_Absyn_Exp*_tmp14C;union Cyc_Absyn_Cnst
_tmp14D;int _tmp14E;union Cyc_Absyn_Cnst _tmp14F;struct _tuple6 _tmp150;int _tmp151;
union Cyc_Absyn_Cnst _tmp152;struct _tuple6 _tmp153;void*_tmp154;struct Cyc_List_List*
_tmp155;void*_tmp156;void*_tmp157;struct Cyc_Absyn_Vardecl*_tmp158;void*_tmp159;
struct Cyc_Absyn_Vardecl*_tmp15A;void*_tmp15B;struct Cyc_Absyn_Vardecl*_tmp15C;
void*_tmp15D;struct Cyc_List_List*_tmp15E;struct Cyc_Absyn_Exp*_tmp15F;struct Cyc_Absyn_Exp*
_tmp160;struct Cyc_Core_Opt*_tmp161;struct Cyc_Core_Opt _tmp162;struct Cyc_Absyn_Exp*
_tmp163;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_Core_Opt*_tmp165;struct Cyc_Absyn_Exp*
_tmp166;struct Cyc_Absyn_Exp*_tmp167;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*
_tmp169;struct Cyc_Absyn_Exp*_tmp16A;struct Cyc_List_List*_tmp16B;struct Cyc_Absyn_MallocInfo
_tmp16C;int _tmp16D;struct Cyc_Absyn_Exp*_tmp16E;void**_tmp16F;struct Cyc_Absyn_Exp*
_tmp170;int _tmp171;struct Cyc_Absyn_Exp*_tmp172;struct Cyc_Absyn_Exp*_tmp173;
struct Cyc_Absyn_Exp*_tmp174;struct Cyc_Absyn_Exp*_tmp175;struct Cyc_Absyn_Exp*
_tmp176;struct Cyc_Absyn_Exp*_tmp177;struct Cyc_Absyn_Exp*_tmp178;struct
_dyneither_ptr*_tmp179;struct Cyc_Absyn_Exp*_tmp17A;struct _dyneither_ptr*_tmp17B;
struct Cyc_Absyn_Exp*_tmp17C;struct _dyneither_ptr*_tmp17D;struct Cyc_Absyn_Exp*
_tmp17E;struct Cyc_Absyn_Exp*_tmp17F;struct Cyc_Absyn_Exp*_tmp180;struct Cyc_Absyn_Exp*
_tmp181;struct Cyc_Absyn_Exp*_tmp182;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Exp*
_tmp184;struct Cyc_Absyn_Exp*_tmp185;struct Cyc_Absyn_Exp*_tmp186;struct Cyc_List_List*
_tmp187;struct Cyc_Absyn_Datatypedecl*_tmp188;struct Cyc_List_List*_tmp189;struct
Cyc_List_List*_tmp18A;struct Cyc_List_List*_tmp18B;struct Cyc_Absyn_Aggrdecl*
_tmp18C;struct Cyc_Absyn_Aggrdecl _tmp18D;void*_tmp18E;struct Cyc_Absyn_AggrdeclImpl*
_tmp18F;struct Cyc_Absyn_AggrdeclImpl _tmp190;struct Cyc_List_List*_tmp191;struct
Cyc_List_List*_tmp192;struct Cyc_Absyn_Vardecl*_tmp193;struct Cyc_Absyn_Exp*
_tmp194;struct Cyc_Absyn_Exp*_tmp195;int _tmp196;struct Cyc_Absyn_Stmt*_tmp197;void*
_tmp198;_LLD5: if(*((int*)_tmp147)!= 15)goto _LLD7;_tmp148=((struct Cyc_Absyn_Cast_e_struct*)
_tmp147)->f2;_tmp149=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp147)->f4;if((
int)_tmp149 != 2)goto _LLD7;_LLD6: {union Cyc_CfFlowInfo_FlowInfo _tmp19A;void*
_tmp19B;struct _tuple13 _tmp199=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp148);
_tmp19A=_tmp199.f1;_tmp19B=_tmp199.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp19D;void*
_tmp19E;struct _tuple13 _tmp19C=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp19A,
_tmp148,_tmp19B);_tmp19D=_tmp19C.f1;_tmp19E=_tmp19C.f2;return({struct _tuple13
_tmp19F;_tmp19F.f1=_tmp19D;_tmp19F.f2=_tmp19B;_tmp19F;});}}_LLD7: if(*((int*)
_tmp147)!= 15)goto _LLD9;_tmp14A=((struct Cyc_Absyn_Cast_e_struct*)_tmp147)->f2;
_LLD8: _tmp14B=_tmp14A;goto _LLDA;_LLD9: if(*((int*)_tmp147)!= 13)goto _LLDB;_tmp14B=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp147)->f1;_LLDA: _tmp14C=_tmp14B;goto
_LLDC;_LLDB: if(*((int*)_tmp147)!= 14)goto _LLDD;_tmp14C=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp147)->f1;_LLDC: return Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp14C);_LLDD:
if(*((int*)_tmp147)!= 0)goto _LLDF;_tmp14D=((struct Cyc_Absyn_Const_e_struct*)
_tmp147)->f1;if((_tmp14D.Null_c).tag != 1)goto _LLDF;_tmp14E=(int)(_tmp14D.Null_c).val;
_LLDE: goto _LLE0;_LLDF: if(*((int*)_tmp147)!= 0)goto _LLE1;_tmp14F=((struct Cyc_Absyn_Const_e_struct*)
_tmp147)->f1;if((_tmp14F.Int_c).tag != 4)goto _LLE1;_tmp150=(struct _tuple6)(
_tmp14F.Int_c).val;_tmp151=_tmp150.f2;if(_tmp151 != 0)goto _LLE1;_LLE0: return({
struct _tuple13 _tmp1A0;_tmp1A0.f1=inflow;_tmp1A0.f2=(void*)0;_tmp1A0;});_LLE1: if(*((
int*)_tmp147)!= 0)goto _LLE3;_tmp152=((struct Cyc_Absyn_Const_e_struct*)_tmp147)->f1;
if((_tmp152.Int_c).tag != 4)goto _LLE3;_tmp153=(struct _tuple6)(_tmp152.Int_c).val;
_LLE2: goto _LLE4;_LLE3: if(*((int*)_tmp147)!= 1)goto _LLE5;_tmp154=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp147)->f2;if(_tmp154 <= (void*)1)goto _LLE5;if(*((int*)
_tmp154)!= 1)goto _LLE5;_LLE4: return({struct _tuple13 _tmp1A1;_tmp1A1.f1=inflow;
_tmp1A1.f2=(void*)1;_tmp1A1;});_LLE5: if(*((int*)_tmp147)!= 32)goto _LLE7;_tmp155=((
struct Cyc_Absyn_Datatype_e_struct*)_tmp147)->f1;if(_tmp155 != 0)goto _LLE7;_LLE6:
goto _LLE8;_LLE7: if(*((int*)_tmp147)!= 0)goto _LLE9;_LLE8: goto _LLEA;_LLE9: if(*((
int*)_tmp147)!= 19)goto _LLEB;_LLEA: goto _LLEC;_LLEB: if(*((int*)_tmp147)!= 18)goto
_LLED;_LLEC: goto _LLEE;_LLED: if(*((int*)_tmp147)!= 20)goto _LLEF;_LLEE: goto _LLF0;
_LLEF: if(*((int*)_tmp147)!= 21)goto _LLF1;_LLF0: goto _LLF2;_LLF1: if(*((int*)
_tmp147)!= 34)goto _LLF3;_LLF2: goto _LLF4;_LLF3: if(*((int*)_tmp147)!= 33)goto _LLF5;
_LLF4: return({struct _tuple13 _tmp1A2;_tmp1A2.f1=inflow;_tmp1A2.f2=_tmp13F->unknown_all;
_tmp1A2;});_LLF5: if(*((int*)_tmp147)!= 1)goto _LLF7;_tmp156=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp147)->f2;if(_tmp156 <= (void*)1)goto _LLF7;if(*((int*)_tmp156)!= 0)goto _LLF7;
_LLF6: return({struct _tuple13 _tmp1A3;_tmp1A3.f1=inflow;_tmp1A3.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp13F,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp13F->unknown_all);
_tmp1A3;});_LLF7: if(*((int*)_tmp147)!= 1)goto _LLF9;_tmp157=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp147)->f2;if(_tmp157 <= (void*)1)goto _LLF9;if(*((int*)_tmp157)!= 2)goto _LLF9;
_tmp158=((struct Cyc_Absyn_Param_b_struct*)_tmp157)->f1;_LLF8: _tmp15A=_tmp158;
goto _LLFA;_LLF9: if(*((int*)_tmp147)!= 1)goto _LLFB;_tmp159=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp147)->f2;if(_tmp159 <= (void*)1)goto _LLFB;if(*((int*)_tmp159)!= 3)goto _LLFB;
_tmp15A=((struct Cyc_Absyn_Local_b_struct*)_tmp159)->f1;_LLFA: _tmp15C=_tmp15A;
goto _LLFC;_LLFB: if(*((int*)_tmp147)!= 1)goto _LLFD;_tmp15B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp147)->f2;if(_tmp15B <= (void*)1)goto _LLFD;if(*((int*)_tmp15B)!= 4)goto _LLFD;
_tmp15C=((struct Cyc_Absyn_Pat_b_struct*)_tmp15B)->f1;_LLFC: inflow=Cyc_NewControlFlow_may_consume_exp(
env,inflow,e);return({struct _tuple13 _tmp1A4;_tmp1A4.f1=inflow;_tmp1A4.f2=Cyc_Dict_lookup(
d,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp1A5=_region_malloc(env->r,
sizeof(*_tmp1A5));_tmp1A5[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp1A6;
_tmp1A6.tag=0;_tmp1A6.f1=_tmp15C;_tmp1A6;});_tmp1A5;}));_tmp1A4;});_LLFD: if(*((
int*)_tmp147)!= 3)goto _LLFF;_tmp15D=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp147)->f1;_tmp15E=((struct Cyc_Absyn_Primop_e_struct*)_tmp147)->f2;_LLFE: {
union Cyc_CfFlowInfo_FlowInfo _tmp1A8;void*_tmp1A9;struct _tuple13 _tmp1A7=Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmp15E,0);_tmp1A8=_tmp1A7.f1;_tmp1A9=_tmp1A7.f2;{void*_tmp1AA=
_tmp15D;_LL140: if((int)_tmp1AA != 0)goto _LL142;_LL141: goto _LL143;_LL142: if((int)
_tmp1AA != 2)goto _LL144;_LL143: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp15E))->hd)->loc,_tmp1A8);goto _LL13F;_LL144:;
_LL145: _tmp1A8=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp15E))->hd)->loc,_tmp1A8);goto _LL13F;_LL13F:;}
return({struct _tuple13 _tmp1AB;_tmp1AB.f1=_tmp1A8;_tmp1AB.f2=_tmp1A9;_tmp1AB;});}
_LLFF: if(*((int*)_tmp147)!= 5)goto _LL101;_tmp15F=((struct Cyc_Absyn_Increment_e_struct*)
_tmp147)->f1;_LL100: {struct Cyc_List_List _tmp1AC=({struct Cyc_List_List _tmp1BB;
_tmp1BB.hd=_tmp15F;_tmp1BB.tl=0;_tmp1BB;});union Cyc_CfFlowInfo_FlowInfo _tmp1AE;
struct _tuple13 _tmp1AD=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct Cyc_List_List*)&
_tmp1AC,0);_tmp1AE=_tmp1AD.f1;Cyc_CfFlowInfo_check_unique_rvals(_tmp15F->loc,
_tmp1AE);{union Cyc_CfFlowInfo_AbsLVal _tmp1B0;struct _tuple15 _tmp1AF=Cyc_NewControlFlow_anal_Lexp(
env,_tmp1AE,_tmp15F);_tmp1B0=_tmp1AF.f2;{struct _tuple20 _tmp1B2=({struct _tuple20
_tmp1B1;_tmp1B1.f1=_tmp1B0;_tmp1B1.f2=_tmp1AE;_tmp1B1;});union Cyc_CfFlowInfo_AbsLVal
_tmp1B3;struct Cyc_CfFlowInfo_Place*_tmp1B4;union Cyc_CfFlowInfo_FlowInfo _tmp1B5;
struct _tuple12 _tmp1B6;struct Cyc_Dict_Dict _tmp1B7;struct Cyc_List_List*_tmp1B8;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp1B9;_LL147: _tmp1B3=_tmp1B2.f1;if((_tmp1B3.PlaceL).tag
!= 1)goto _LL149;_tmp1B4=(struct Cyc_CfFlowInfo_Place*)(_tmp1B3.PlaceL).val;
_tmp1B5=_tmp1B2.f2;if((_tmp1B5.ReachableFL).tag != 2)goto _LL149;_tmp1B6=(struct
_tuple12)(_tmp1B5.ReachableFL).val;_tmp1B7=_tmp1B6.f1;_tmp1B8=_tmp1B6.f2;_tmp1B9=
_tmp1B6.f3;_LL148: _tmp1B8=Cyc_CfFlowInfo_reln_kill_exp(_tmp13F->r,_tmp1B8,
_tmp15F);_tmp1AE=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp13F,
_tmp15F->loc,_tmp1B7,env->all_changed,_tmp1B4,_tmp13F->unknown_all),_tmp1B8,
_tmp1B9);goto _LL146;_LL149:;_LL14A: goto _LL146;_LL146:;}return({struct _tuple13
_tmp1BA;_tmp1BA.f1=_tmp1AE;_tmp1BA.f2=_tmp13F->unknown_all;_tmp1BA;});}}_LL101:
if(*((int*)_tmp147)!= 4)goto _LL103;_tmp160=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp147)->f1;_tmp161=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp147)->f2;if(
_tmp161 == 0)goto _LL103;_tmp162=*_tmp161;_tmp163=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp147)->f3;_LL102: {struct Cyc_List_List _tmp1BC=({struct Cyc_List_List _tmp1CC;
_tmp1CC.hd=_tmp163;_tmp1CC.tl=0;_tmp1CC;});struct Cyc_List_List _tmp1BD=({struct
Cyc_List_List _tmp1CB;_tmp1CB.hd=_tmp160;_tmp1CB.tl=(struct Cyc_List_List*)&
_tmp1BC;_tmp1CB;});union Cyc_CfFlowInfo_FlowInfo _tmp1BF;struct _tuple13 _tmp1BE=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp1BD,1);_tmp1BF=_tmp1BE.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp1C1;struct _tuple15 _tmp1C0=Cyc_NewControlFlow_anal_Lexp(env,_tmp1BF,_tmp160);
_tmp1C1=_tmp1C0.f2;_tmp1BF=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp1BF);{
union Cyc_CfFlowInfo_FlowInfo _tmp1C2=_tmp1BF;struct _tuple12 _tmp1C3;struct Cyc_Dict_Dict
_tmp1C4;struct Cyc_List_List*_tmp1C5;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1C6;
_LL14C: if((_tmp1C2.ReachableFL).tag != 2)goto _LL14E;_tmp1C3=(struct _tuple12)(
_tmp1C2.ReachableFL).val;_tmp1C4=_tmp1C3.f1;_tmp1C5=_tmp1C3.f2;_tmp1C6=_tmp1C3.f3;
_LL14D:{union Cyc_CfFlowInfo_AbsLVal _tmp1C7=_tmp1C1;struct Cyc_CfFlowInfo_Place*
_tmp1C8;int _tmp1C9;_LL151: if((_tmp1C7.PlaceL).tag != 1)goto _LL153;_tmp1C8=(struct
Cyc_CfFlowInfo_Place*)(_tmp1C7.PlaceL).val;_LL152: _tmp1C6=Cyc_NewControlFlow_consume_assignment(
env,_tmp1C8,_tmp1C6,_tmp1C4,_tmp160);_tmp1C5=Cyc_CfFlowInfo_reln_kill_exp(
_tmp13F->r,_tmp1C5,_tmp160);_tmp1C4=Cyc_CfFlowInfo_assign_place(_tmp13F,_tmp160->loc,
_tmp1C4,env->all_changed,_tmp1C8,_tmp13F->unknown_all);_tmp1BF=Cyc_CfFlowInfo_ReachableFL(
_tmp1C4,_tmp1C5,_tmp1C6);goto _LL150;_LL153: if((_tmp1C7.UnknownL).tag != 2)goto
_LL150;_tmp1C9=(int)(_tmp1C7.UnknownL).val;_LL154: goto _LL150;_LL150:;}goto _LL14B;
_LL14E:;_LL14F: goto _LL14B;_LL14B:;}return({struct _tuple13 _tmp1CA;_tmp1CA.f1=
_tmp1BF;_tmp1CA.f2=_tmp13F->unknown_all;_tmp1CA;});}}_LL103: if(*((int*)_tmp147)
!= 4)goto _LL105;_tmp164=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp147)->f1;
_tmp165=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp147)->f2;if(_tmp165 != 0)goto
_LL105;_tmp166=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp147)->f3;_LL104: {struct
Cyc_Dict_Dict*_tmp1CD=env->all_changed;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1CF;
union Cyc_CfFlowInfo_FlowInfo _tmp1D0;struct _tuple14 _tmp1CE=Cyc_CfFlowInfo_save_consume_info(
_tmp13F,inflow,1);_tmp1CF=_tmp1CE.f1;_tmp1D0=_tmp1CE.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
empty_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp1EB;_tmp1EB.consumed=_tmp13F->mt_place_set;
_tmp1EB.may_consume=0;_tmp1EB;});struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=
empty_consume;int init_consume=0;while(1){env->all_changed=({struct Cyc_Dict_Dict*
_tmp1D1=_region_malloc(env->r,sizeof(*_tmp1D1));_tmp1D1[0]=_tmp13F->mt_place_set;
_tmp1D1;});{union Cyc_CfFlowInfo_FlowInfo _tmp1D3;union Cyc_CfFlowInfo_AbsLVal
_tmp1D4;struct _tuple15 _tmp1D2=Cyc_NewControlFlow_anal_Lexp(env,_tmp1D0,_tmp164);
_tmp1D3=_tmp1D2.f1;_tmp1D4=_tmp1D2.f2;{struct Cyc_Dict_Dict _tmp1D5=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));env->all_changed=({struct Cyc_Dict_Dict*_tmp1D6=
_region_malloc(env->r,sizeof(*_tmp1D6));_tmp1D6[0]=_tmp13F->mt_place_set;_tmp1D6;});{
union Cyc_CfFlowInfo_FlowInfo _tmp1D8;void*_tmp1D9;struct _tuple13 _tmp1D7=Cyc_NewControlFlow_anal_Rexp(
env,_tmp1D0,_tmp166);_tmp1D8=_tmp1D7.f1;_tmp1D9=_tmp1D7.f2;{struct Cyc_Dict_Dict
_tmp1DA=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo
_tmp1DB=Cyc_CfFlowInfo_after_flow(_tmp13F,(struct Cyc_Dict_Dict*)& _tmp1D5,_tmp1D3,
_tmp1D8,_tmp1D5,_tmp1DA);union Cyc_CfFlowInfo_FlowInfo _tmp1DC=Cyc_CfFlowInfo_join_flow(
_tmp13F,_tmp1CD,_tmp1D0,_tmp1DB,1);struct Cyc_CfFlowInfo_ConsumeInfo _tmp1DE;
struct _tuple14 _tmp1DD=Cyc_CfFlowInfo_save_consume_info(_tmp13F,_tmp1DC,0);
_tmp1DE=_tmp1DD.f1;if(init_consume){if(!Cyc_CfFlowInfo_consume_approx(
outflow_consume,_tmp1DE)){({void*_tmp1DF=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp1E0="sanity consumed: ";_tag_dyneither(_tmp1E0,sizeof(char),18);}),
_tag_dyneither(_tmp1DF,sizeof(void*),0));});((void(*)(struct Cyc_Dict_Dict p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(outflow_consume.consumed,
Cyc_CfFlowInfo_print_place);({void*_tmp1E1=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp1E2="\ncurrent consumed: ";_tag_dyneither(_tmp1E2,sizeof(char),20);}),
_tag_dyneither(_tmp1E1,sizeof(void*),0));});((void(*)(struct Cyc_Dict_Dict p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(_tmp1DE.consumed,
Cyc_CfFlowInfo_print_place);({void*_tmp1E3=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp1E4="\nsanity may_consume: ";_tag_dyneither(_tmp1E4,sizeof(char),22);}),
_tag_dyneither(_tmp1E3,sizeof(void*),0));});((void(*)(struct Cyc_List_List*p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(outflow_consume.may_consume,
Cyc_CfFlowInfo_print_place);({void*_tmp1E5=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp1E6="\ncurrent may_consume: ";_tag_dyneither(_tmp1E6,sizeof(char),23);}),
_tag_dyneither(_tmp1E5,sizeof(void*),0));});((void(*)(struct Cyc_List_List*p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(_tmp1DE.may_consume,
Cyc_CfFlowInfo_print_place);({void*_tmp1E7=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp1E8="\n";_tag_dyneither(_tmp1E8,sizeof(char),2);}),_tag_dyneither(_tmp1E7,
sizeof(void*),0));});({void*_tmp1E9=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1EA="assignOp_e failed to preserve consume info";
_tag_dyneither(_tmp1EA,sizeof(char),43);}),_tag_dyneither(_tmp1E9,sizeof(void*),
0));});}_tmp1D0=Cyc_CfFlowInfo_restore_consume_info(_tmp1D0,outflow_consume,0);}
outflow_consume=_tmp1DE;init_consume=1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp1DC,_tmp1D0)){if(_tmp1CD == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp1CD,Cyc_CfFlowInfo_union_place_set(
_tmp1D5,_tmp1DA,0),0);}return Cyc_NewControlFlow_do_assign(_tmp13F,env,_tmp1DB,
_tmp164,_tmp1D4,_tmp166,_tmp1D9,_tmp1CF,e->loc);}_tmp1D0=Cyc_CfFlowInfo_restore_consume_info(
_tmp1DC,empty_consume,0);}}}}}}}_LL105: if(*((int*)_tmp147)!= 9)goto _LL107;
_tmp167=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp147)->f1;_tmp168=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp147)->f2;_LL106: {union Cyc_CfFlowInfo_FlowInfo _tmp1ED;void*_tmp1EE;struct
_tuple13 _tmp1EC=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp167);_tmp1ED=_tmp1EC.f1;
_tmp1EE=_tmp1EC.f2;_tmp1ED=Cyc_CfFlowInfo_drop_unique_rvals(_tmp167->loc,_tmp1ED);
return Cyc_NewControlFlow_anal_Rexp(env,_tmp1ED,_tmp168);}_LL107: if(*((int*)
_tmp147)!= 12)goto _LL109;_tmp169=((struct Cyc_Absyn_Throw_e_struct*)_tmp147)->f1;
_LL108: {union Cyc_CfFlowInfo_FlowInfo _tmp1F0;void*_tmp1F1;struct _tuple13 _tmp1EF=
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp169);_tmp1F0=_tmp1EF.f1;_tmp1F1=
_tmp1EF.f2;_tmp1F0=Cyc_CfFlowInfo_consume_unique_rvals(_tmp169->loc,_tmp1F0);Cyc_NewControlFlow_use_Rval(
env,_tmp169->loc,_tmp1F0,_tmp1F1);return({struct _tuple13 _tmp1F2;_tmp1F2.f1=Cyc_CfFlowInfo_BottomFL();
_tmp1F2.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp13F,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp13F->unknown_all);_tmp1F2;});}_LL109: if(*((int*)
_tmp147)!= 11)goto _LL10B;_tmp16A=((struct Cyc_Absyn_FnCall_e_struct*)_tmp147)->f1;
_tmp16B=((struct Cyc_Absyn_FnCall_e_struct*)_tmp147)->f2;_LL10A: {struct
_RegionHandle*_tmp1F3=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp1F6;struct Cyc_List_List*
_tmp1F7;struct _tuple19 _tmp1F5=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp1F3,
env,inflow,({struct Cyc_List_List*_tmp1F4=_region_malloc(_tmp1F3,sizeof(*_tmp1F4));
_tmp1F4->hd=_tmp16A;_tmp1F4->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rcopy)(_tmp1F3,_tmp16B);_tmp1F4;}),1);_tmp1F6=
_tmp1F5.f1;_tmp1F7=_tmp1F5.f2;_tmp1F6=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp1F6);{union Cyc_CfFlowInfo_FlowInfo _tmp1F8=Cyc_NewControlFlow_use_Rval(env,
_tmp16A->loc,_tmp1F6,(void*)((struct Cyc_List_List*)_check_null(_tmp1F7))->hd);
_tmp1F7=_tmp1F7->tl;{struct Cyc_List_List*init_params=0;{void*_tmp1F9=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp16A->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1FA;void*_tmp1FB;_LL156: if(_tmp1F9 <= (void*)4)goto _LL158;if(*((int*)_tmp1F9)
!= 4)goto _LL158;_tmp1FA=((struct Cyc_Absyn_PointerType_struct*)_tmp1F9)->f1;
_tmp1FB=_tmp1FA.elt_typ;_LL157:{void*_tmp1FC=Cyc_Tcutil_compress(_tmp1FB);struct
Cyc_Absyn_FnInfo _tmp1FD;struct Cyc_List_List*_tmp1FE;_LL15B: if(_tmp1FC <= (void*)4)
goto _LL15D;if(*((int*)_tmp1FC)!= 8)goto _LL15D;_tmp1FD=((struct Cyc_Absyn_FnType_struct*)
_tmp1FC)->f1;_tmp1FE=_tmp1FD.attributes;_LL15C: for(0;_tmp1FE != 0;_tmp1FE=_tmp1FE->tl){
void*_tmp1FF=(void*)_tmp1FE->hd;int _tmp200;_LL160: if(_tmp1FF <= (void*)17)goto
_LL162;if(*((int*)_tmp1FF)!= 4)goto _LL162;_tmp200=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1FF)->f1;_LL161: init_params=({struct Cyc_List_List*_tmp201=_region_malloc(
_tmp1F3,sizeof(*_tmp201));_tmp201->hd=(void*)_tmp200;_tmp201->tl=init_params;
_tmp201;});goto _LL15F;_LL162:;_LL163: goto _LL15F;_LL15F:;}goto _LL15A;_LL15D:;
_LL15E:({void*_tmp202=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp203="anal_Rexp: bad function type";
_tag_dyneither(_tmp203,sizeof(char),29);}),_tag_dyneither(_tmp202,sizeof(void*),
0));});_LL15A:;}goto _LL155;_LL158:;_LL159:({void*_tmp204=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp205="anal_Rexp: bad function type";
_tag_dyneither(_tmp205,sizeof(char),29);}),_tag_dyneither(_tmp204,sizeof(void*),
0));});_LL155:;}{int i=1;for(0;_tmp1F7 != 0;(((_tmp1F7=_tmp1F7->tl,_tmp16B=((
struct Cyc_List_List*)_check_null(_tmp16B))->tl)),++ i)){if(!((int(*)(struct Cyc_List_List*
l,int x))Cyc_List_memq)(init_params,i)){_tmp1F8=Cyc_NewControlFlow_use_Rval(env,((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp16B))->hd)->loc,
_tmp1F8,(void*)_tmp1F7->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo _tmp206=
_tmp1F6;int _tmp207;struct _tuple12 _tmp208;struct Cyc_Dict_Dict _tmp209;_LL165: if((
_tmp206.BottomFL).tag != 1)goto _LL167;_tmp207=(int)(_tmp206.BottomFL).val;_LL166:
goto _LL164;_LL167: if((_tmp206.ReachableFL).tag != 2)goto _LL164;_tmp208=(struct
_tuple12)(_tmp206.ReachableFL).val;_tmp209=_tmp208.f1;_LL168: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp209,(void*)_tmp1F7->hd)== (void*)0)({void*_tmp20A=0;Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp16B))->hd)->loc,({
const char*_tmp20B="expression may not be initialized";_tag_dyneither(_tmp20B,
sizeof(char),34);}),_tag_dyneither(_tmp20A,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo
_tmp20C=_tmp1F8;int _tmp20D;struct _tuple12 _tmp20E;struct Cyc_Dict_Dict _tmp20F;
struct Cyc_List_List*_tmp210;struct Cyc_CfFlowInfo_ConsumeInfo _tmp211;_LL16A: if((
_tmp20C.BottomFL).tag != 1)goto _LL16C;_tmp20D=(int)(_tmp20C.BottomFL).val;_LL16B:
goto _LL169;_LL16C: if((_tmp20C.ReachableFL).tag != 2)goto _LL169;_tmp20E=(struct
_tuple12)(_tmp20C.ReachableFL).val;_tmp20F=_tmp20E.f1;_tmp210=_tmp20E.f2;_tmp211=
_tmp20E.f3;_LL16D: {struct Cyc_Dict_Dict _tmp212=Cyc_CfFlowInfo_escape_deref(
_tmp13F,_tmp20F,env->all_changed,(void*)_tmp1F7->hd);{void*_tmp213=(void*)
_tmp1F7->hd;struct Cyc_CfFlowInfo_Place*_tmp214;_LL16F: if(_tmp213 <= (void*)3)goto
_LL171;if(*((int*)_tmp213)!= 2)goto _LL171;_tmp214=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp213)->f1;_LL170:{void*_tmp215=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp16B))->hd)->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp216;void*_tmp217;_LL174: if(_tmp215 <= (void*)4)goto
_LL176;if(*((int*)_tmp215)!= 4)goto _LL176;_tmp216=((struct Cyc_Absyn_PointerType_struct*)
_tmp215)->f1;_tmp217=_tmp216.elt_typ;_LL175: _tmp212=Cyc_CfFlowInfo_assign_place(
_tmp13F,((struct Cyc_Absyn_Exp*)_tmp16B->hd)->loc,_tmp212,env->all_changed,
_tmp214,Cyc_CfFlowInfo_typ_to_absrval(_tmp13F,_tmp217,_tmp13F->esc_all));goto
_LL173;_LL176:;_LL177:({void*_tmp218=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp219="anal_Rexp:bad type for initialized arg";
_tag_dyneither(_tmp219,sizeof(char),39);}),_tag_dyneither(_tmp218,sizeof(void*),
0));});_LL173:;}goto _LL16E;_LL171:;_LL172: goto _LL16E;_LL16E:;}_tmp1F8=Cyc_CfFlowInfo_ReachableFL(
_tmp212,_tmp210,_tmp211);goto _LL169;}_LL169:;}goto _LL164;_LL164:;}}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp16A->topt))->v))return({struct
_tuple13 _tmp21A;_tmp21A.f1=Cyc_CfFlowInfo_BottomFL();_tmp21A.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp13F,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp13F->unknown_all);
_tmp21A;});else{return({struct _tuple13 _tmp21B;_tmp21B.f1=_tmp1F8;_tmp21B.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp13F,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp13F->unknown_all);
_tmp21B;});}}}}_LL10B: if(*((int*)_tmp147)!= 35)goto _LL10D;_tmp16C=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp147)->f1;_tmp16D=_tmp16C.is_calloc;_tmp16E=_tmp16C.rgn;_tmp16F=_tmp16C.elt_type;
_tmp170=_tmp16C.num_elts;_tmp171=_tmp16C.fat_result;_LL10C: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp22E=_region_malloc(_tmp13F->r,sizeof(*
_tmp22E));_tmp22E[0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp22F;_tmp22F.tag=1;
_tmp22F.f1=_tmp170;_tmp22F.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp22F;});_tmp22E;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp22D=_region_malloc(_tmp13F->r,sizeof(*_tmp22D));_tmp22D->root=root;_tmp22D->fields=
0;_tmp22D;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp22B=
_region_malloc(_tmp13F->r,sizeof(*_tmp22B));_tmp22B[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp22C;_tmp22C.tag=2;_tmp22C.f1=place;_tmp22C;});_tmp22B;});void*place_val=
_tmp171?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(_tmp13F,*((void**)_check_null(
_tmp16F)),_tmp13F->unknown_none);union Cyc_CfFlowInfo_FlowInfo outflow;((int(*)(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);if(_tmp16E != 0){
struct _RegionHandle*_tmp21C=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp21F;struct Cyc_List_List*
_tmp220;struct _tuple19 _tmp21E=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp21C,
env,inflow,({struct Cyc_Absyn_Exp*_tmp21D[2];_tmp21D[1]=_tmp170;_tmp21D[0]=(
struct Cyc_Absyn_Exp*)_tmp16E;((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(_tmp21C,_tag_dyneither(_tmp21D,sizeof(
struct Cyc_Absyn_Exp*),2));}),0);_tmp21F=_tmp21E.f1;_tmp220=_tmp21E.f2;outflow=
_tmp21F;}else{union Cyc_CfFlowInfo_FlowInfo _tmp222;struct _tuple13 _tmp221=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp170);_tmp222=_tmp221.f1;outflow=_tmp222;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp170->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo _tmp223=outflow;int _tmp224;
struct _tuple12 _tmp225;struct Cyc_Dict_Dict _tmp226;struct Cyc_List_List*_tmp227;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp228;_LL179: if((_tmp223.BottomFL).tag != 1)
goto _LL17B;_tmp224=(int)(_tmp223.BottomFL).val;_LL17A: return({struct _tuple13
_tmp229;_tmp229.f1=outflow;_tmp229.f2=rval;_tmp229;});_LL17B: if((_tmp223.ReachableFL).tag
!= 2)goto _LL178;_tmp225=(struct _tuple12)(_tmp223.ReachableFL).val;_tmp226=
_tmp225.f1;_tmp227=_tmp225.f2;_tmp228=_tmp225.f3;_LL17C: return({struct _tuple13
_tmp22A;_tmp22A.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_Dict_insert(_tmp226,root,
place_val),_tmp227,_tmp228);_tmp22A.f2=rval;_tmp22A;});_LL178:;}}_LL10D: if(*((
int*)_tmp147)!= 36)goto _LL10F;_tmp172=((struct Cyc_Absyn_Swap_e_struct*)_tmp147)->f1;
_tmp173=((struct Cyc_Absyn_Swap_e_struct*)_tmp147)->f2;_LL10E: {void*left_rval;
void*right_rval;union Cyc_CfFlowInfo_FlowInfo outflow;struct _RegionHandle*_tmp230=
env->r;{union Cyc_CfFlowInfo_FlowInfo _tmp233;struct Cyc_List_List*_tmp234;struct
_tuple19 _tmp232=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp230,env,inflow,({
struct Cyc_Absyn_Exp*_tmp231[2];_tmp231[1]=_tmp173;_tmp231[0]=_tmp172;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp230,
_tag_dyneither(_tmp231,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp233=_tmp232.f1;
_tmp234=_tmp232.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp234))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp234->tl))->hd;outflow=
_tmp233;}Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,outflow);{union Cyc_CfFlowInfo_AbsLVal
_tmp236;struct _tuple15 _tmp235=Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmp172);
_tmp236=_tmp235.f2;{union Cyc_CfFlowInfo_AbsLVal _tmp238;struct _tuple15 _tmp237=Cyc_NewControlFlow_anal_Lexp(
env,outflow,_tmp173);_tmp238=_tmp237.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp239=
outflow;struct _tuple12 _tmp23A;struct Cyc_Dict_Dict _tmp23B;struct Cyc_List_List*
_tmp23C;struct Cyc_CfFlowInfo_ConsumeInfo _tmp23D;_LL17E: if((_tmp239.ReachableFL).tag
!= 2)goto _LL180;_tmp23A=(struct _tuple12)(_tmp239.ReachableFL).val;_tmp23B=
_tmp23A.f1;_tmp23C=_tmp23A.f2;_tmp23D=_tmp23A.f3;_LL17F:{union Cyc_CfFlowInfo_AbsLVal
_tmp23E=_tmp236;struct Cyc_CfFlowInfo_Place*_tmp23F;int _tmp240;_LL183: if((_tmp23E.PlaceL).tag
!= 1)goto _LL185;_tmp23F=(struct Cyc_CfFlowInfo_Place*)(_tmp23E.PlaceL).val;_LL184:
_tmp23B=Cyc_CfFlowInfo_assign_place(_tmp13F,_tmp172->loc,_tmp23B,env->all_changed,
_tmp23F,right_rval);goto _LL182;_LL185: if((_tmp23E.UnknownL).tag != 2)goto _LL182;
_tmp240=(int)(_tmp23E.UnknownL).val;_LL186: goto _LL182;_LL182:;}{union Cyc_CfFlowInfo_AbsLVal
_tmp241=_tmp238;struct Cyc_CfFlowInfo_Place*_tmp242;int _tmp243;_LL188: if((_tmp241.PlaceL).tag
!= 1)goto _LL18A;_tmp242=(struct Cyc_CfFlowInfo_Place*)(_tmp241.PlaceL).val;_LL189:
_tmp23B=Cyc_CfFlowInfo_assign_place(_tmp13F,_tmp173->loc,_tmp23B,env->all_changed,
_tmp242,left_rval);goto _LL187;_LL18A: if((_tmp241.UnknownL).tag != 2)goto _LL187;
_tmp243=(int)(_tmp241.UnknownL).val;_LL18B: goto _LL187;_LL187:;}_tmp23C=Cyc_CfFlowInfo_reln_kill_exp(
_tmp13F->r,_tmp23C,_tmp172);_tmp23C=Cyc_CfFlowInfo_reln_kill_exp(_tmp13F->r,
_tmp23C,_tmp173);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp23B,_tmp23C,_tmp23D);
goto _LL17D;_LL180:;_LL181: goto _LL17D;_LL17D:;}return({struct _tuple13 _tmp244;
_tmp244.f1=outflow;_tmp244.f2=_tmp13F->unknown_all;_tmp244;});}}}_LL10F: if(*((
int*)_tmp147)!= 17)goto _LL111;_tmp174=((struct Cyc_Absyn_New_e_struct*)_tmp147)->f1;
_tmp175=((struct Cyc_Absyn_New_e_struct*)_tmp147)->f2;_LL110: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp258=_region_malloc(_tmp13F->r,sizeof(*
_tmp258));_tmp258[0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp259;_tmp259.tag=1;
_tmp259.f1=_tmp175;_tmp259.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp259;});_tmp258;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp257=_region_malloc(_tmp13F->r,sizeof(*_tmp257));_tmp257->root=root;_tmp257->fields=
0;_tmp257;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp255=
_region_malloc(_tmp13F->r,sizeof(*_tmp255));_tmp255[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp256;_tmp256.tag=2;_tmp256.f1=place;_tmp256;});_tmp255;});((int(*)(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo outflow;void*place_val;
if(_tmp174 != 0){struct _RegionHandle*_tmp245=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp248;struct Cyc_List_List*_tmp249;struct _tuple19 _tmp247=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp245,env,inflow,({struct Cyc_Absyn_Exp*_tmp246[2];_tmp246[1]=_tmp175;_tmp246[0]=(
struct Cyc_Absyn_Exp*)_tmp174;((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(_tmp245,_tag_dyneither(_tmp246,sizeof(
struct Cyc_Absyn_Exp*),2));}),0);_tmp248=_tmp247.f1;_tmp249=_tmp247.f2;outflow=
_tmp248;place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp249))->tl))->hd;}else{union Cyc_CfFlowInfo_FlowInfo _tmp24B;void*
_tmp24C;struct _tuple13 _tmp24A=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp175);
_tmp24B=_tmp24A.f1;_tmp24C=_tmp24A.f2;outflow=_tmp24B;place_val=_tmp24C;}outflow=
Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp175->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo
_tmp24D=outflow;int _tmp24E;struct _tuple12 _tmp24F;struct Cyc_Dict_Dict _tmp250;
struct Cyc_List_List*_tmp251;struct Cyc_CfFlowInfo_ConsumeInfo _tmp252;_LL18D: if((
_tmp24D.BottomFL).tag != 1)goto _LL18F;_tmp24E=(int)(_tmp24D.BottomFL).val;_LL18E:
return({struct _tuple13 _tmp253;_tmp253.f1=outflow;_tmp253.f2=rval;_tmp253;});
_LL18F: if((_tmp24D.ReachableFL).tag != 2)goto _LL18C;_tmp24F=(struct _tuple12)(
_tmp24D.ReachableFL).val;_tmp250=_tmp24F.f1;_tmp251=_tmp24F.f2;_tmp252=_tmp24F.f3;
_LL190: return({struct _tuple13 _tmp254;_tmp254.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_Dict_insert(
_tmp250,root,place_val),_tmp251,_tmp252);_tmp254.f2=rval;_tmp254;});_LL18C:;}}}
_LL111: if(*((int*)_tmp147)!= 16)goto _LL113;_tmp176=((struct Cyc_Absyn_Address_e_struct*)
_tmp147)->f1;_LL112: {union Cyc_CfFlowInfo_FlowInfo _tmp25B;struct _tuple13 _tmp25A=
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp176);_tmp25B=_tmp25A.f1;{struct Cyc_CfFlowInfo_ConsumeInfo
_tmp25D;struct _tuple14 _tmp25C=Cyc_CfFlowInfo_save_consume_info(env->fenv,_tmp25B,
0);_tmp25D=_tmp25C.f1;{union Cyc_CfFlowInfo_FlowInfo _tmp25F;union Cyc_CfFlowInfo_AbsLVal
_tmp260;struct _tuple15 _tmp25E=Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmp176);
_tmp25F=_tmp25E.f1;_tmp260=_tmp25E.f2;_tmp25F=Cyc_CfFlowInfo_restore_consume_info(
_tmp25F,_tmp25D,0);{union Cyc_CfFlowInfo_AbsLVal _tmp261=_tmp260;int _tmp262;struct
Cyc_CfFlowInfo_Place*_tmp263;_LL192: if((_tmp261.UnknownL).tag != 2)goto _LL194;
_tmp262=(int)(_tmp261.UnknownL).val;_LL193: return({struct _tuple13 _tmp264;_tmp264.f1=
_tmp25F;_tmp264.f2=(void*)1;_tmp264;});_LL194: if((_tmp261.PlaceL).tag != 1)goto
_LL191;_tmp263=(struct Cyc_CfFlowInfo_Place*)(_tmp261.PlaceL).val;_LL195: return({
struct _tuple13 _tmp265;_tmp265.f1=_tmp25F;_tmp265.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp266=_region_malloc(env->r,sizeof(*_tmp266));_tmp266[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp267;_tmp267.tag=2;_tmp267.f1=_tmp263;_tmp267;});_tmp266;});_tmp265;});_LL191:;}}}}
_LL113: if(*((int*)_tmp147)!= 22)goto _LL115;_tmp177=((struct Cyc_Absyn_Deref_e_struct*)
_tmp147)->f1;_LL114: {union Cyc_CfFlowInfo_FlowInfo _tmp269;void*_tmp26A;struct
_tuple13 _tmp268=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp177);_tmp269=_tmp268.f1;
_tmp26A=_tmp268.f2;_tmp269=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp269);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp269,_tmp177,_tmp26A);}
_LL115: if(*((int*)_tmp147)!= 23)goto _LL117;_tmp178=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp147)->f1;_tmp179=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp147)->f2;_LL116: {
union Cyc_CfFlowInfo_FlowInfo _tmp26C;void*_tmp26D;struct _tuple13 _tmp26B=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp178);_tmp26C=_tmp26B.f1;_tmp26D=_tmp26B.f2;_tmp26C=Cyc_CfFlowInfo_drop_unique_rvals(
e->loc,_tmp26C);_tmp26C=Cyc_NewControlFlow_may_consume_exp(env,_tmp26C,e);if(Cyc_Absyn_is_nontagged_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp178->topt))->v))return({struct
_tuple13 _tmp26E;_tmp26E.f1=_tmp26C;_tmp26E.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp13F,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp13F->unknown_all);
_tmp26E;});{void*_tmp26F=_tmp26D;int _tmp270;struct _dyneither_ptr _tmp271;_LL197:
if(_tmp26F <= (void*)3)goto _LL199;if(*((int*)_tmp26F)!= 4)goto _LL199;_tmp270=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp26F)->f1;_tmp271=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp26F)->f2;_LL198: {int _tmp272=Cyc_CfFlowInfo_get_field_index((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp178->topt))->v,_tmp179);return({struct _tuple13
_tmp273;_tmp273.f1=_tmp26C;_tmp273.f2=*((void**)_check_dyneither_subscript(
_tmp271,sizeof(void*),_tmp272));_tmp273;});}_LL199:;_LL19A:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274[0]=({
struct Cyc_Core_Impossible_struct _tmp275;_tmp275.tag=Cyc_Core_Impossible;_tmp275.f1=(
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp278;_tmp278.tag=0;_tmp278.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{void*
_tmp276[1]={& _tmp278};Cyc_aprintf(({const char*_tmp277="anal_Rexp: AggrMember: %s";
_tag_dyneither(_tmp277,sizeof(char),26);}),_tag_dyneither(_tmp276,sizeof(void*),
1));}});_tmp275;});_tmp274;}));_LL196:;}}_LL117: if(*((int*)_tmp147)!= 39)goto
_LL119;_tmp17A=((struct Cyc_Absyn_Tagcheck_e_struct*)_tmp147)->f1;_tmp17B=((
struct Cyc_Absyn_Tagcheck_e_struct*)_tmp147)->f2;_LL118: {union Cyc_CfFlowInfo_FlowInfo
_tmp27A;void*_tmp27B;struct _tuple13 _tmp279=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp17A);_tmp27A=_tmp279.f1;_tmp27B=_tmp279.f2;_tmp27A=Cyc_CfFlowInfo_drop_unique_rvals(
e->loc,_tmp27A);_tmp27A=Cyc_NewControlFlow_may_consume_exp(env,_tmp27A,e);return({
struct _tuple13 _tmp27C;_tmp27C.f1=_tmp27A;_tmp27C.f2=_tmp13F->unknown_all;_tmp27C;});}
_LL119: if(*((int*)_tmp147)!= 24)goto _LL11B;_tmp17C=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp147)->f1;_tmp17D=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp147)->f2;_LL11A: {
union Cyc_CfFlowInfo_FlowInfo _tmp27E;void*_tmp27F;struct _tuple13 _tmp27D=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp17C);_tmp27E=_tmp27D.f1;_tmp27F=_tmp27D.f2;_tmp27E=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp27E);{union Cyc_CfFlowInfo_FlowInfo _tmp281;void*_tmp282;struct _tuple13
_tmp280=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp27E,_tmp17C,_tmp27F);
_tmp281=_tmp280.f1;_tmp282=_tmp280.f2;{void*_tmp283=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp17C->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp284;void*_tmp285;_LL19C: if(_tmp283 <= (void*)4)goto _LL19E;if(*((int*)_tmp283)
!= 4)goto _LL19E;_tmp284=((struct Cyc_Absyn_PointerType_struct*)_tmp283)->f1;
_tmp285=_tmp284.elt_typ;_LL19D: if(Cyc_Absyn_is_nontagged_union_type(_tmp285))
return({struct _tuple13 _tmp286;_tmp286.f1=_tmp281;_tmp286.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp13F,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp13F->unknown_all);
_tmp286;});{void*_tmp287=_tmp282;int _tmp288;struct _dyneither_ptr _tmp289;_LL1A1:
if(_tmp287 <= (void*)3)goto _LL1A3;if(*((int*)_tmp287)!= 4)goto _LL1A3;_tmp288=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp287)->f1;_tmp289=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp287)->f2;_LL1A2: {int _tmp28A=Cyc_CfFlowInfo_get_field_index(_tmp285,_tmp17D);
return({struct _tuple13 _tmp28B;_tmp28B.f1=_tmp281;_tmp28B.f2=*((void**)
_check_dyneither_subscript(_tmp289,sizeof(void*),_tmp28A));_tmp28B;});}_LL1A3:;
_LL1A4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp28C=_cycalloc(
sizeof(*_tmp28C));_tmp28C[0]=({struct Cyc_Core_Impossible_struct _tmp28D;_tmp28D.tag=
Cyc_Core_Impossible;_tmp28D.f1=({const char*_tmp28E="anal_Rexp: AggrArrow";
_tag_dyneither(_tmp28E,sizeof(char),21);});_tmp28D;});_tmp28C;}));_LL1A0:;}
_LL19E:;_LL19F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp28F=
_cycalloc(sizeof(*_tmp28F));_tmp28F[0]=({struct Cyc_Core_Impossible_struct _tmp290;
_tmp290.tag=Cyc_Core_Impossible;_tmp290.f1=({const char*_tmp291="anal_Rexp: AggrArrow ptr";
_tag_dyneither(_tmp291,sizeof(char),25);});_tmp290;});_tmp28F;}));_LL19B:;}}}
_LL11B: if(*((int*)_tmp147)!= 6)goto _LL11D;_tmp17E=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp147)->f1;_tmp17F=((struct Cyc_Absyn_Conditional_e_struct*)_tmp147)->f2;
_tmp180=((struct Cyc_Absyn_Conditional_e_struct*)_tmp147)->f3;_LL11C: {union Cyc_CfFlowInfo_FlowInfo
_tmp293;union Cyc_CfFlowInfo_FlowInfo _tmp294;struct _tuple16 _tmp292=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp17E);_tmp293=_tmp292.f1;_tmp294=_tmp292.f2;_tmp293=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp17E->loc,_tmp293);_tmp294=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp17E->loc,
_tmp294);{struct _tuple13 _tmp295=Cyc_NewControlFlow_anal_Rexp(env,_tmp293,_tmp17F);
struct _tuple13 _tmp296=Cyc_NewControlFlow_anal_Rexp(env,_tmp294,_tmp180);return
Cyc_CfFlowInfo_join_flow_and_rval(_tmp13F,env->all_changed,_tmp295,_tmp296,1);}}
_LL11D: if(*((int*)_tmp147)!= 7)goto _LL11F;_tmp181=((struct Cyc_Absyn_And_e_struct*)
_tmp147)->f1;_tmp182=((struct Cyc_Absyn_And_e_struct*)_tmp147)->f2;_LL11E: {union
Cyc_CfFlowInfo_FlowInfo _tmp298;union Cyc_CfFlowInfo_FlowInfo _tmp299;struct
_tuple16 _tmp297=Cyc_NewControlFlow_anal_test(env,inflow,_tmp181);_tmp298=_tmp297.f1;
_tmp299=_tmp297.f2;_tmp298=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp181->loc,
_tmp298);_tmp299=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp181->loc,_tmp299);{
union Cyc_CfFlowInfo_FlowInfo _tmp29B;void*_tmp29C;struct _tuple13 _tmp29A=Cyc_NewControlFlow_anal_Rexp(
env,_tmp298,_tmp182);_tmp29B=_tmp29A.f1;_tmp29C=_tmp29A.f2;_tmp29B=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp182->loc,_tmp29B);{struct _tuple13 _tmp29D=({struct _tuple13 _tmp2A0;_tmp2A0.f1=
_tmp29B;_tmp2A0.f2=_tmp29C;_tmp2A0;});struct _tuple13 _tmp29E=({struct _tuple13
_tmp29F;_tmp29F.f1=_tmp299;_tmp29F.f2=(void*)((void*)0);_tmp29F;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp13F,env->all_changed,_tmp29D,_tmp29E,0);}}}_LL11F: if(*((int*)_tmp147)!= 8)
goto _LL121;_tmp183=((struct Cyc_Absyn_Or_e_struct*)_tmp147)->f1;_tmp184=((struct
Cyc_Absyn_Or_e_struct*)_tmp147)->f2;_LL120: {union Cyc_CfFlowInfo_FlowInfo _tmp2A2;
union Cyc_CfFlowInfo_FlowInfo _tmp2A3;struct _tuple16 _tmp2A1=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp183);_tmp2A2=_tmp2A1.f1;_tmp2A3=_tmp2A1.f2;_tmp2A2=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp183->loc,_tmp2A2);_tmp2A3=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp183->loc,
_tmp2A3);{union Cyc_CfFlowInfo_FlowInfo _tmp2A5;void*_tmp2A6;struct _tuple13 _tmp2A4=
Cyc_NewControlFlow_anal_Rexp(env,_tmp2A3,_tmp184);_tmp2A5=_tmp2A4.f1;_tmp2A6=
_tmp2A4.f2;_tmp2A5=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp184->loc,_tmp2A5);{
struct _tuple13 _tmp2A7=({struct _tuple13 _tmp2AA;_tmp2AA.f1=_tmp2A5;_tmp2AA.f2=
_tmp2A6;_tmp2AA;});struct _tuple13 _tmp2A8=({struct _tuple13 _tmp2A9;_tmp2A9.f1=
_tmp2A2;_tmp2A9.f2=(void*)((void*)1);_tmp2A9;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp13F,env->all_changed,_tmp2A7,_tmp2A8,0);}}}_LL121: if(*((int*)_tmp147)!= 25)
goto _LL123;_tmp185=((struct Cyc_Absyn_Subscript_e_struct*)_tmp147)->f1;_tmp186=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp147)->f2;_LL122: {struct _RegionHandle*
_tmp2AB=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp2AE;struct Cyc_List_List*_tmp2AF;
struct _tuple19 _tmp2AD=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2AB,env,inflow,({
struct Cyc_Absyn_Exp*_tmp2AC[2];_tmp2AC[1]=_tmp186;_tmp2AC[0]=_tmp185;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp2AB,
_tag_dyneither(_tmp2AC,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp2AE=_tmp2AD.f1;
_tmp2AF=_tmp2AD.f2;_tmp2AE=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp186->loc,
_tmp2AE);{union Cyc_CfFlowInfo_FlowInfo _tmp2B0=_tmp2AE;{union Cyc_CfFlowInfo_FlowInfo
_tmp2B1=_tmp2AE;struct _tuple12 _tmp2B2;struct Cyc_Dict_Dict _tmp2B3;struct Cyc_List_List*
_tmp2B4;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2B5;_LL1A6: if((_tmp2B1.ReachableFL).tag
!= 2)goto _LL1A8;_tmp2B2=(struct _tuple12)(_tmp2B1.ReachableFL).val;_tmp2B3=
_tmp2B2.f1;_tmp2B4=_tmp2B2.f2;_tmp2B5=_tmp2B2.f3;_LL1A7: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp2B3,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp2AF))->tl))->hd)== (void*)0)({void*_tmp2B6=0;Cyc_Tcutil_terr(
_tmp186->loc,({const char*_tmp2B7="expression may not be initialized";
_tag_dyneither(_tmp2B7,sizeof(char),34);}),_tag_dyneither(_tmp2B6,sizeof(void*),
0));});{struct Cyc_List_List*_tmp2B8=Cyc_NewControlFlow_add_subscript_reln(
_tmp13F->r,_tmp2B4,_tmp185,_tmp186);if(_tmp2B4 != _tmp2B8)_tmp2B0=Cyc_CfFlowInfo_ReachableFL(
_tmp2B3,_tmp2B8,_tmp2B5);goto _LL1A5;}_LL1A8:;_LL1A9: goto _LL1A5;_LL1A5:;}{void*
_tmp2B9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp185->topt))->v);
struct Cyc_List_List*_tmp2BA;struct Cyc_Absyn_PtrInfo _tmp2BB;struct Cyc_Absyn_PtrAtts
_tmp2BC;union Cyc_Absyn_Constraint*_tmp2BD;_LL1AB: if(_tmp2B9 <= (void*)4)goto
_LL1AF;if(*((int*)_tmp2B9)!= 9)goto _LL1AD;_tmp2BA=((struct Cyc_Absyn_TupleType_struct*)
_tmp2B9)->f1;_LL1AC: {void*_tmp2BE=(void*)((struct Cyc_List_List*)_check_null(
_tmp2AF))->hd;int _tmp2BF;struct _dyneither_ptr _tmp2C0;_LL1B2: if(_tmp2BE <= (void*)
3)goto _LL1B4;if(*((int*)_tmp2BE)!= 4)goto _LL1B4;_tmp2BF=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2BE)->f1;_tmp2C0=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2BE)->f2;
_LL1B3: _tmp2B0=Cyc_NewControlFlow_may_consume_exp(env,_tmp2B0,e);{unsigned int i=(
Cyc_Evexp_eval_const_uint_exp(_tmp186)).f1;return({struct _tuple13 _tmp2C1;_tmp2C1.f1=
_tmp2B0;_tmp2C1.f2=*((void**)_check_dyneither_subscript(_tmp2C0,sizeof(void*),(
int)i));_tmp2C1;});}_LL1B4:;_LL1B5:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2C2=_cycalloc(sizeof(*_tmp2C2));_tmp2C2[0]=({struct Cyc_Core_Impossible_struct
_tmp2C3;_tmp2C3.tag=Cyc_Core_Impossible;_tmp2C3.f1=({const char*_tmp2C4="anal_Rexp: Subscript";
_tag_dyneither(_tmp2C4,sizeof(char),21);});_tmp2C3;});_tmp2C2;}));_LL1B1:;}
_LL1AD: if(*((int*)_tmp2B9)!= 4)goto _LL1AF;_tmp2BB=((struct Cyc_Absyn_PointerType_struct*)
_tmp2B9)->f1;_tmp2BC=_tmp2BB.ptr_atts;_tmp2BD=_tmp2BC.bounds;_LL1AE: {union Cyc_CfFlowInfo_FlowInfo
_tmp2C6;void*_tmp2C7;struct _tuple13 _tmp2C5=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp2AE,_tmp185,(void*)((struct Cyc_List_List*)_check_null(_tmp2AF))->hd);
_tmp2C6=_tmp2C5.f1;_tmp2C7=_tmp2C5.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp2C8=
_tmp2C6;int _tmp2C9;_LL1B7: if((_tmp2C8.BottomFL).tag != 1)goto _LL1B9;_tmp2C9=(int)(
_tmp2C8.BottomFL).val;_LL1B8: return({struct _tuple13 _tmp2CA;_tmp2CA.f1=_tmp2C6;
_tmp2CA.f2=_tmp2C7;_tmp2CA;});_LL1B9:;_LL1BA: return({struct _tuple13 _tmp2CB;
_tmp2CB.f1=_tmp2B0;_tmp2CB.f2=_tmp2C7;_tmp2CB;});_LL1B6:;}}_LL1AF:;_LL1B0:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2CC=_cycalloc(sizeof(*
_tmp2CC));_tmp2CC[0]=({struct Cyc_Core_Impossible_struct _tmp2CD;_tmp2CD.tag=Cyc_Core_Impossible;
_tmp2CD.f1=({const char*_tmp2CE="anal_Rexp: Subscript -- bad type";_tag_dyneither(
_tmp2CE,sizeof(char),33);});_tmp2CD;});_tmp2CC;}));_LL1AA:;}}}_LL123: if(*((int*)
_tmp147)!= 32)goto _LL125;_tmp187=((struct Cyc_Absyn_Datatype_e_struct*)_tmp147)->f1;
_tmp188=((struct Cyc_Absyn_Datatype_e_struct*)_tmp147)->f2;_LL124: _tmp189=_tmp187;
goto _LL126;_LL125: if(*((int*)_tmp147)!= 26)goto _LL127;_tmp189=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp147)->f1;_LL126: {struct _RegionHandle*_tmp2CF=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp2D1;struct Cyc_List_List*_tmp2D2;struct _tuple19 _tmp2D0=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp2CF,env,inflow,_tmp189,0);_tmp2D1=_tmp2D0.f1;_tmp2D2=_tmp2D0.f2;_tmp2D1=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2D1);{struct _dyneither_ptr aggrdict=({unsigned int _tmp2D6=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp189);void**
_tmp2D7=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp2D6));
struct _dyneither_ptr _tmp2DA=_tag_dyneither(_tmp2D7,sizeof(void*),_tmp2D6);{
unsigned int _tmp2D8=_tmp2D6;unsigned int i;for(i=0;i < _tmp2D8;i ++){_tmp2D7[i]=({
void*_tmp2D9=(void*)((struct Cyc_List_List*)_check_null(_tmp2D2))->hd;_tmp2D2=
_tmp2D2->tl;_tmp2D9;});}}_tmp2DA;});return({struct _tuple13 _tmp2D3;_tmp2D3.f1=
_tmp2D1;_tmp2D3.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2D4=
_region_malloc(env->r,sizeof(*_tmp2D4));_tmp2D4[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp2D5;_tmp2D5.tag=4;_tmp2D5.f1=0;_tmp2D5.f2=aggrdict;_tmp2D5;});_tmp2D4;});
_tmp2D3;});}}_LL127: if(*((int*)_tmp147)!= 31)goto _LL129;_tmp18A=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp147)->f2;_LL128: {struct Cyc_List_List*fs;{void*_tmp2DB=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_List_List*
_tmp2DC;_LL1BC: if(_tmp2DB <= (void*)4)goto _LL1BE;if(*((int*)_tmp2DB)!= 11)goto
_LL1BE;_tmp2DC=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2DB)->f2;_LL1BD: fs=
_tmp2DC;goto _LL1BB;_LL1BE:;_LL1BF:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2DD=_cycalloc(sizeof(*_tmp2DD));_tmp2DD[0]=({struct Cyc_Core_Impossible_struct
_tmp2DE;_tmp2DE.tag=Cyc_Core_Impossible;_tmp2DE.f1=({const char*_tmp2DF="anal_Rexp:anon struct has bad type";
_tag_dyneither(_tmp2DF,sizeof(char),35);});_tmp2DE;});_tmp2DD;}));_LL1BB:;}
_tmp18B=_tmp18A;_tmp18E=(void*)0;_tmp191=fs;goto _LL12A;}_LL129: if(*((int*)
_tmp147)!= 30)goto _LL12B;_tmp18B=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp147)->f3;
_tmp18C=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp147)->f4;if(_tmp18C == 0)goto
_LL12B;_tmp18D=*_tmp18C;_tmp18E=_tmp18D.kind;_tmp18F=_tmp18D.impl;if(_tmp18F == 0)
goto _LL12B;_tmp190=*_tmp18F;_tmp191=_tmp190.fields;_LL12A: {struct _RegionHandle*
_tmp2E0=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp2E2;struct Cyc_List_List*_tmp2E3;
struct _tuple19 _tmp2E1=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2E0,env,inflow,((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct
_tuple22*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp2E0,(struct Cyc_Absyn_Exp*(*)(
struct _tuple22*))Cyc_Core_snd,_tmp18B),0);_tmp2E2=_tmp2E1.f1;_tmp2E3=_tmp2E1.f2;
_tmp2E2=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp2E2);{struct
_dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp13F,_tmp191,
_tmp13F->unknown_all);{int i=0;for(0;_tmp2E3 != 0;(((_tmp2E3=_tmp2E3->tl,_tmp18B=
_tmp18B->tl)),++ i)){struct Cyc_List_List*ds=(*((struct _tuple22*)((struct Cyc_List_List*)
_check_null(_tmp18B))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*_tmp2E4=(void*)ds->hd;
struct _dyneither_ptr*_tmp2E5;_LL1C1: if(*((int*)_tmp2E4)!= 0)goto _LL1C3;_LL1C2:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2E6=_cycalloc(sizeof(*
_tmp2E6));_tmp2E6[0]=({struct Cyc_Core_Impossible_struct _tmp2E7;_tmp2E7.tag=Cyc_Core_Impossible;
_tmp2E7.f1=({const char*_tmp2E8="anal_Rexp:Aggregate_e";_tag_dyneither(_tmp2E8,
sizeof(char),22);});_tmp2E7;});_tmp2E6;}));_LL1C3: if(*((int*)_tmp2E4)!= 1)goto
_LL1C0;_tmp2E5=((struct Cyc_Absyn_FieldName_struct*)_tmp2E4)->f1;_LL1C4: {int
_tmp2E9=Cyc_CfFlowInfo_get_field_index_fs(_tmp191,_tmp2E5);*((void**)
_check_dyneither_subscript(aggrdict,sizeof(void*),_tmp2E9))=(void*)_tmp2E3->hd;
if(_tmp18E == (void*)1){struct Cyc_Absyn_Exp*_tmp2EA=(*((struct _tuple22*)_tmp18B->hd)).f2;
_tmp2E2=Cyc_NewControlFlow_use_Rval(env,_tmp2EA->loc,_tmp2E2,(void*)_tmp2E3->hd);}}
_LL1C0:;}}}return({struct _tuple13 _tmp2EB;_tmp2EB.f1=_tmp2E2;_tmp2EB.f2=(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2EC=_region_malloc(env->r,sizeof(*
_tmp2EC));_tmp2EC[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp2ED;_tmp2ED.tag=
4;_tmp2ED.f1=_tmp18E == (void*)1;_tmp2ED.f2=aggrdict;_tmp2ED;});_tmp2EC;});
_tmp2EB;});}}_LL12B: if(*((int*)_tmp147)!= 30)goto _LL12D;_LL12C:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp2EE=_cycalloc(sizeof(*_tmp2EE));_tmp2EE[0]=({
struct Cyc_Core_Impossible_struct _tmp2EF;_tmp2EF.tag=Cyc_Core_Impossible;_tmp2EF.f1=({
const char*_tmp2F0="anal_Rexp:missing aggrdeclimpl";_tag_dyneither(_tmp2F0,
sizeof(char),31);});_tmp2EF;});_tmp2EE;}));_LL12D: if(*((int*)_tmp147)!= 28)goto
_LL12F;_tmp192=((struct Cyc_Absyn_Array_e_struct*)_tmp147)->f1;_LL12E: {struct
_RegionHandle*_tmp2F1=env->r;struct Cyc_List_List*_tmp2F2=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple22*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmp2F1,(struct Cyc_Absyn_Exp*(*)(struct _tuple22*))Cyc_Core_snd,
_tmp192);union Cyc_CfFlowInfo_FlowInfo _tmp2F4;struct Cyc_List_List*_tmp2F5;struct
_tuple19 _tmp2F3=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2F1,env,inflow,
_tmp2F2,0);_tmp2F4=_tmp2F3.f1;_tmp2F5=_tmp2F3.f2;_tmp2F4=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2F4);for(0;_tmp2F5 != 0;(_tmp2F5=_tmp2F5->tl,_tmp2F2=_tmp2F2->tl)){
_tmp2F4=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2F2))->hd)->loc,_tmp2F4,(void*)_tmp2F5->hd);}return({struct
_tuple13 _tmp2F6;_tmp2F6.f1=_tmp2F4;_tmp2F6.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp13F,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp13F->unknown_all);
_tmp2F6;});}_LL12F: if(*((int*)_tmp147)!= 29)goto _LL131;_tmp193=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp147)->f1;_tmp194=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp147)->f2;
_tmp195=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp147)->f3;_tmp196=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp147)->f4;_LL130: {union Cyc_CfFlowInfo_FlowInfo
_tmp2F8;void*_tmp2F9;struct _tuple13 _tmp2F7=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp194);_tmp2F8=_tmp2F7.f1;_tmp2F9=_tmp2F7.f2;_tmp2F8=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp194->loc,_tmp2F8);{union Cyc_CfFlowInfo_FlowInfo _tmp2FA=_tmp2F8;int _tmp2FB;
struct _tuple12 _tmp2FC;struct Cyc_Dict_Dict _tmp2FD;struct Cyc_List_List*_tmp2FE;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp2FF;_LL1C6: if((_tmp2FA.BottomFL).tag != 1)
goto _LL1C8;_tmp2FB=(int)(_tmp2FA.BottomFL).val;_LL1C7: return({struct _tuple13
_tmp300;_tmp300.f1=_tmp2F8;_tmp300.f2=_tmp13F->unknown_all;_tmp300;});_LL1C8: if((
_tmp2FA.ReachableFL).tag != 2)goto _LL1C5;_tmp2FC=(struct _tuple12)(_tmp2FA.ReachableFL).val;
_tmp2FD=_tmp2FC.f1;_tmp2FE=_tmp2FC.f2;_tmp2FF=_tmp2FC.f3;_LL1C9: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp2FD,_tmp2F9)== (void*)0)({void*_tmp301=0;Cyc_Tcutil_terr(_tmp194->loc,({
const char*_tmp302="expression may not be initialized";_tag_dyneither(_tmp302,
sizeof(char),34);}),_tag_dyneither(_tmp301,sizeof(void*),0));});{struct Cyc_List_List*
new_relns=_tmp2FE;comp_loop: {void*_tmp303=_tmp194->r;struct Cyc_Absyn_Exp*
_tmp304;void*_tmp305;struct Cyc_Absyn_Vardecl*_tmp306;void*_tmp307;struct Cyc_Absyn_Vardecl*
_tmp308;void*_tmp309;struct Cyc_Absyn_Vardecl*_tmp30A;void*_tmp30B;struct Cyc_Absyn_Vardecl*
_tmp30C;union Cyc_Absyn_Cnst _tmp30D;struct _tuple6 _tmp30E;int _tmp30F;void*_tmp310;
struct Cyc_List_List*_tmp311;struct Cyc_List_List _tmp312;struct Cyc_Absyn_Exp*
_tmp313;_LL1CB: if(*((int*)_tmp303)!= 15)goto _LL1CD;_tmp304=((struct Cyc_Absyn_Cast_e_struct*)
_tmp303)->f2;_LL1CC: _tmp194=_tmp304;goto comp_loop;_LL1CD: if(*((int*)_tmp303)!= 1)
goto _LL1CF;_tmp305=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp303)->f2;if(
_tmp305 <= (void*)1)goto _LL1CF;if(*((int*)_tmp305)!= 0)goto _LL1CF;_tmp306=((
struct Cyc_Absyn_Global_b_struct*)_tmp305)->f1;if(!(!_tmp306->escapes))goto _LL1CF;
_LL1CE: _tmp308=_tmp306;goto _LL1D0;_LL1CF: if(*((int*)_tmp303)!= 1)goto _LL1D1;
_tmp307=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp303)->f2;if(_tmp307 <= (void*)
1)goto _LL1D1;if(*((int*)_tmp307)!= 3)goto _LL1D1;_tmp308=((struct Cyc_Absyn_Local_b_struct*)
_tmp307)->f1;if(!(!_tmp308->escapes))goto _LL1D1;_LL1D0: _tmp30A=_tmp308;goto
_LL1D2;_LL1D1: if(*((int*)_tmp303)!= 1)goto _LL1D3;_tmp309=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp303)->f2;if(_tmp309 <= (void*)1)goto _LL1D3;if(*((int*)_tmp309)!= 4)goto _LL1D3;
_tmp30A=((struct Cyc_Absyn_Pat_b_struct*)_tmp309)->f1;if(!(!_tmp30A->escapes))
goto _LL1D3;_LL1D2: _tmp30C=_tmp30A;goto _LL1D4;_LL1D3: if(*((int*)_tmp303)!= 1)goto
_LL1D5;_tmp30B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp303)->f2;if(_tmp30B <= (
void*)1)goto _LL1D5;if(*((int*)_tmp30B)!= 2)goto _LL1D5;_tmp30C=((struct Cyc_Absyn_Param_b_struct*)
_tmp30B)->f1;if(!(!_tmp30C->escapes))goto _LL1D5;_LL1D4: new_relns=({struct Cyc_List_List*
_tmp314=_region_malloc(env->r,sizeof(*_tmp314));_tmp314->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp315=_region_malloc(env->r,sizeof(*_tmp315));_tmp315->vd=_tmp193;_tmp315->rop=
Cyc_CfFlowInfo_LessVar(_tmp30C,_tmp30C->type);_tmp315;});_tmp314->tl=_tmp2FE;
_tmp314;});goto _LL1CA;_LL1D5: if(*((int*)_tmp303)!= 0)goto _LL1D7;_tmp30D=((struct
Cyc_Absyn_Const_e_struct*)_tmp303)->f1;if((_tmp30D.Int_c).tag != 4)goto _LL1D7;
_tmp30E=(struct _tuple6)(_tmp30D.Int_c).val;_tmp30F=_tmp30E.f2;_LL1D6: new_relns=({
struct Cyc_List_List*_tmp316=_region_malloc(env->r,sizeof(*_tmp316));_tmp316->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp317=_region_malloc(env->r,sizeof(*_tmp317));
_tmp317->vd=_tmp193;_tmp317->rop=Cyc_CfFlowInfo_LessConst((unsigned int)_tmp30F);
_tmp317;});_tmp316->tl=_tmp2FE;_tmp316;});goto _LL1CA;_LL1D7: if(*((int*)_tmp303)
!= 3)goto _LL1D9;_tmp310=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp303)->f1;
_tmp311=((struct Cyc_Absyn_Primop_e_struct*)_tmp303)->f2;if(_tmp311 == 0)goto
_LL1D9;_tmp312=*_tmp311;_tmp313=(struct Cyc_Absyn_Exp*)_tmp312.hd;_LL1D8:{void*
_tmp318=_tmp313->r;void*_tmp319;struct Cyc_Absyn_Vardecl*_tmp31A;void*_tmp31B;
struct Cyc_Absyn_Vardecl*_tmp31C;void*_tmp31D;struct Cyc_Absyn_Vardecl*_tmp31E;
void*_tmp31F;struct Cyc_Absyn_Vardecl*_tmp320;_LL1DC: if(*((int*)_tmp318)!= 1)goto
_LL1DE;_tmp319=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp318)->f2;if(_tmp319 <= (
void*)1)goto _LL1DE;if(*((int*)_tmp319)!= 0)goto _LL1DE;_tmp31A=((struct Cyc_Absyn_Global_b_struct*)
_tmp319)->f1;if(!(!_tmp31A->escapes))goto _LL1DE;_LL1DD: _tmp31C=_tmp31A;goto
_LL1DF;_LL1DE: if(*((int*)_tmp318)!= 1)goto _LL1E0;_tmp31B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp318)->f2;if(_tmp31B <= (void*)1)goto _LL1E0;if(*((int*)_tmp31B)!= 3)goto _LL1E0;
_tmp31C=((struct Cyc_Absyn_Local_b_struct*)_tmp31B)->f1;if(!(!_tmp31C->escapes))
goto _LL1E0;_LL1DF: _tmp31E=_tmp31C;goto _LL1E1;_LL1E0: if(*((int*)_tmp318)!= 1)goto
_LL1E2;_tmp31D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp318)->f2;if(_tmp31D <= (
void*)1)goto _LL1E2;if(*((int*)_tmp31D)!= 4)goto _LL1E2;_tmp31E=((struct Cyc_Absyn_Pat_b_struct*)
_tmp31D)->f1;if(!(!_tmp31E->escapes))goto _LL1E2;_LL1E1: _tmp320=_tmp31E;goto
_LL1E3;_LL1E2: if(*((int*)_tmp318)!= 1)goto _LL1E4;_tmp31F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp318)->f2;if(_tmp31F <= (void*)1)goto _LL1E4;if(*((int*)_tmp31F)!= 2)goto _LL1E4;
_tmp320=((struct Cyc_Absyn_Param_b_struct*)_tmp31F)->f1;if(!(!_tmp320->escapes))
goto _LL1E4;_LL1E3: new_relns=({struct Cyc_List_List*_tmp321=_region_malloc(env->r,
sizeof(*_tmp321));_tmp321->hd=({struct Cyc_CfFlowInfo_Reln*_tmp322=_region_malloc(
env->r,sizeof(*_tmp322));_tmp322->vd=_tmp193;_tmp322->rop=Cyc_CfFlowInfo_LessNumelts(
_tmp320);_tmp322;});_tmp321->tl=_tmp2FE;_tmp321;});goto _LL1DB;_LL1E4:;_LL1E5:
goto _LL1DB;_LL1DB:;}goto _LL1CA;_LL1D9:;_LL1DA: goto _LL1CA;_LL1CA:;}if(_tmp2FE != 
new_relns)_tmp2F8=Cyc_CfFlowInfo_ReachableFL(_tmp2FD,new_relns,_tmp2FF);{void*
_tmp323=_tmp2F9;_LL1E7: if((int)_tmp323 != 0)goto _LL1E9;_LL1E8: return({struct
_tuple13 _tmp324;_tmp324.f1=_tmp2F8;_tmp324.f2=_tmp13F->unknown_all;_tmp324;});
_LL1E9: if((int)_tmp323 != 2)goto _LL1EB;_LL1EA: goto _LL1EC;_LL1EB: if((int)_tmp323 != 
1)goto _LL1ED;_LL1EC: goto _LL1EE;_LL1ED: if(_tmp323 <= (void*)3)goto _LL1EF;if(*((int*)
_tmp323)!= 2)goto _LL1EF;_LL1EE: {struct Cyc_List_List _tmp325=({struct Cyc_List_List
_tmp332;_tmp332.hd=_tmp193;_tmp332.tl=0;_tmp332;});_tmp2F8=Cyc_NewControlFlow_add_vars(
_tmp13F,_tmp2F8,(struct Cyc_List_List*)& _tmp325,_tmp13F->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo _tmp327;void*_tmp328;struct _tuple13 _tmp326=Cyc_NewControlFlow_anal_Rexp(
env,_tmp2F8,_tmp195);_tmp327=_tmp326.f1;_tmp328=_tmp326.f2;_tmp327=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp195->loc,_tmp327);{union Cyc_CfFlowInfo_FlowInfo _tmp329=_tmp327;int _tmp32A;
struct _tuple12 _tmp32B;struct Cyc_Dict_Dict _tmp32C;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp32D;_LL1F2: if((_tmp329.BottomFL).tag != 1)goto _LL1F4;_tmp32A=(int)(_tmp329.BottomFL).val;
_LL1F3: return({struct _tuple13 _tmp32E;_tmp32E.f1=_tmp327;_tmp32E.f2=_tmp13F->unknown_all;
_tmp32E;});_LL1F4: if((_tmp329.ReachableFL).tag != 2)goto _LL1F1;_tmp32B=(struct
_tuple12)(_tmp329.ReachableFL).val;_tmp32C=_tmp32B.f1;_tmp32D=_tmp32B.f3;_LL1F5:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp32C,_tmp328)!= (void*)2){({void*
_tmp32F=0;Cyc_Tcutil_terr(_tmp194->loc,({const char*_tmp330="expression may not be initialized";
_tag_dyneither(_tmp330,sizeof(char),34);}),_tag_dyneither(_tmp32F,sizeof(void*),
0));});return({struct _tuple13 _tmp331;_tmp331.f1=Cyc_CfFlowInfo_BottomFL();
_tmp331.f2=_tmp13F->unknown_all;_tmp331;});}_LL1F1:;}_tmp2F8=_tmp327;goto _LL1F0;}}
_LL1EF:;_LL1F0: while(1){struct Cyc_List_List _tmp333=({struct Cyc_List_List _tmp340;
_tmp340.hd=_tmp193;_tmp340.tl=0;_tmp340;});_tmp2F8=Cyc_NewControlFlow_add_vars(
_tmp13F,_tmp2F8,(struct Cyc_List_List*)& _tmp333,_tmp13F->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo _tmp335;void*_tmp336;struct _tuple13 _tmp334=Cyc_NewControlFlow_anal_Rexp(
env,_tmp2F8,_tmp195);_tmp335=_tmp334.f1;_tmp336=_tmp334.f2;_tmp335=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp195->loc,_tmp335);{union Cyc_CfFlowInfo_FlowInfo _tmp337=_tmp335;int _tmp338;
struct _tuple12 _tmp339;struct Cyc_Dict_Dict _tmp33A;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp33B;_LL1F7: if((_tmp337.BottomFL).tag != 1)goto _LL1F9;_tmp338=(int)(_tmp337.BottomFL).val;
_LL1F8: goto _LL1F6;_LL1F9: if((_tmp337.ReachableFL).tag != 2)goto _LL1F6;_tmp339=(
struct _tuple12)(_tmp337.ReachableFL).val;_tmp33A=_tmp339.f1;_tmp33B=_tmp339.f3;
_LL1FA: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp33A,_tmp336)!= (void*)2){({void*
_tmp33C=0;Cyc_Tcutil_terr(_tmp194->loc,({const char*_tmp33D="expression may not be initialized";
_tag_dyneither(_tmp33D,sizeof(char),34);}),_tag_dyneither(_tmp33C,sizeof(void*),
0));});return({struct _tuple13 _tmp33E;_tmp33E.f1=Cyc_CfFlowInfo_BottomFL();
_tmp33E.f2=_tmp13F->unknown_all;_tmp33E;});}_LL1F6:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp33F=Cyc_CfFlowInfo_join_flow(_tmp13F,env->all_changed,_tmp2F8,_tmp335,1);if(
Cyc_CfFlowInfo_flow_lessthan_approx(_tmp33F,_tmp2F8))break;_tmp2F8=_tmp33F;}}}
return({struct _tuple13 _tmp341;_tmp341.f1=_tmp2F8;_tmp341.f2=_tmp13F->unknown_all;
_tmp341;});_LL1E6:;}}_LL1C5:;}}_LL131: if(*((int*)_tmp147)!= 38)goto _LL133;
_tmp197=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp147)->f1;_LL132: while(1){union
Cyc_CfFlowInfo_FlowInfo*_tmp343;struct _tuple17 _tmp342=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,_tmp197);_tmp343=_tmp342.f2;inflow=*_tmp343;{void*_tmp344=_tmp197->r;
struct Cyc_Absyn_Stmt*_tmp345;struct Cyc_Absyn_Stmt*_tmp346;struct Cyc_Absyn_Decl*
_tmp347;struct Cyc_Absyn_Stmt*_tmp348;struct Cyc_Absyn_Exp*_tmp349;_LL1FC: if(
_tmp344 <= (void*)1)goto _LL202;if(*((int*)_tmp344)!= 1)goto _LL1FE;_tmp345=((
struct Cyc_Absyn_Seq_s_struct*)_tmp344)->f1;_tmp346=((struct Cyc_Absyn_Seq_s_struct*)
_tmp344)->f2;_LL1FD: inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp345);
_tmp197=_tmp346;goto _LL1FB;_LL1FE: if(*((int*)_tmp344)!= 11)goto _LL200;_tmp347=((
struct Cyc_Absyn_Decl_s_struct*)_tmp344)->f1;_tmp348=((struct Cyc_Absyn_Decl_s_struct*)
_tmp344)->f2;_LL1FF: inflow=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp347);
_tmp197=_tmp348;goto _LL1FB;_LL200: if(*((int*)_tmp344)!= 0)goto _LL202;_tmp349=((
struct Cyc_Absyn_Exp_s_struct*)_tmp344)->f1;_LL201: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp349);_LL202:;_LL203:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp34A=_cycalloc(sizeof(*_tmp34A));_tmp34A[0]=({struct Cyc_Core_Impossible_struct
_tmp34B;_tmp34B.tag=Cyc_Core_Impossible;_tmp34B.f1=({const char*_tmp34C="analyze_Rexp: ill-formed StmtExp";
_tag_dyneither(_tmp34C,sizeof(char),33);});_tmp34B;});_tmp34A;}));_LL1FB:;}}
_LL133: if(*((int*)_tmp147)!= 1)goto _LL135;_tmp198=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp147)->f2;if((int)_tmp198 != 0)goto _LL135;_LL134: goto _LL136;_LL135: if(*((int*)
_tmp147)!= 2)goto _LL137;_LL136: goto _LL138;_LL137: if(*((int*)_tmp147)!= 10)goto
_LL139;_LL138: goto _LL13A;_LL139: if(*((int*)_tmp147)!= 37)goto _LL13B;_LL13A: goto
_LL13C;_LL13B: if(*((int*)_tmp147)!= 27)goto _LL13D;_LL13C: goto _LL13E;_LL13D: if(*((
int*)_tmp147)!= 40)goto _LLD4;_LL13E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp34D=_cycalloc(sizeof(*_tmp34D));_tmp34D[0]=({struct Cyc_Core_Impossible_struct
_tmp34E;_tmp34E.tag=Cyc_Core_Impossible;_tmp34E.f1=({const char*_tmp34F="anal_Rexp, unexpected exp form";
_tag_dyneither(_tmp34F,sizeof(char),31);});_tmp34E;});_tmp34D;}));_LLD4:;}}
static struct _tuple15 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo
f,void*r,struct Cyc_List_List*flds){struct Cyc_CfFlowInfo_FlowEnv*_tmp350=env->fenv;
void*_tmp351=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp352;void*_tmp353;struct Cyc_Absyn_PtrAtts _tmp354;union
Cyc_Absyn_Constraint*_tmp355;union Cyc_Absyn_Constraint*_tmp356;_LL205: if(_tmp351
<= (void*)4)goto _LL207;if(*((int*)_tmp351)!= 4)goto _LL207;_tmp352=((struct Cyc_Absyn_PointerType_struct*)
_tmp351)->f1;_tmp353=_tmp352.elt_typ;_tmp354=_tmp352.ptr_atts;_tmp355=_tmp354.bounds;
_tmp356=_tmp354.zero_term;_LL206: {union Cyc_CfFlowInfo_FlowInfo _tmp357=f;int
_tmp358;struct _tuple12 _tmp359;struct Cyc_Dict_Dict _tmp35A;struct Cyc_List_List*
_tmp35B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp35C;_LL20A: if((_tmp357.BottomFL).tag
!= 1)goto _LL20C;_tmp358=(int)(_tmp357.BottomFL).val;_LL20B: return({struct
_tuple15 _tmp35D;_tmp35D.f1=f;_tmp35D.f2=Cyc_CfFlowInfo_UnknownL();_tmp35D;});
_LL20C: if((_tmp357.ReachableFL).tag != 2)goto _LL209;_tmp359=(struct _tuple12)(
_tmp357.ReachableFL).val;_tmp35A=_tmp359.f1;_tmp35B=_tmp359.f2;_tmp35C=_tmp359.f3;
_LL20D: if(Cyc_Tcutil_is_bound_one(_tmp355)){void*_tmp35E=r;struct Cyc_CfFlowInfo_Place*
_tmp35F;struct Cyc_CfFlowInfo_Place _tmp360;void*_tmp361;struct Cyc_List_List*
_tmp362;void*_tmp363;_LL20F: if((int)_tmp35E != 1)goto _LL211;_LL210: goto _LL212;
_LL211: if((int)_tmp35E != 2)goto _LL213;_LL212: e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp364=_cycalloc(sizeof(*_tmp364));_tmp364[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp365;_tmp365.tag=Cyc_CfFlowInfo_NotZero;_tmp365.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp35B);_tmp365;});_tmp364;});goto _LL20E;_LL213: if(_tmp35E
<= (void*)3)goto _LL215;if(*((int*)_tmp35E)!= 2)goto _LL215;_tmp35F=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp35E)->f1;_tmp360=*_tmp35F;_tmp361=_tmp360.root;_tmp362=_tmp360.fields;_LL214:
e->annot=(void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp366=_cycalloc(sizeof(*
_tmp366));_tmp366[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp367;_tmp367.tag=
Cyc_CfFlowInfo_NotZero;_tmp367.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp35B);_tmp367;});_tmp366;});return({struct _tuple15 _tmp368;_tmp368.f1=f;
_tmp368.f2=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*_tmp369=
_region_malloc(_tmp350->r,sizeof(*_tmp369));_tmp369->root=_tmp361;_tmp369->fields=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(_tmp350->r,_tmp362,flds);_tmp369;}));_tmp368;});_LL215: if((
int)_tmp35E != 0)goto _LL217;_LL216: e->annot=(void*)Cyc_CfFlowInfo_IsZero;return({
struct _tuple15 _tmp36A;_tmp36A.f1=Cyc_CfFlowInfo_BottomFL();_tmp36A.f2=Cyc_CfFlowInfo_UnknownL();
_tmp36A;});_LL217: if(_tmp35E <= (void*)3)goto _LL219;if(*((int*)_tmp35E)!= 0)goto
_LL219;_tmp363=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp35E)->f1;
_LL218: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp363);goto _LL21A;_LL219:;
_LL21A: e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp36B=_cycalloc(
sizeof(*_tmp36B));_tmp36B[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp36C;
_tmp36C.tag=Cyc_CfFlowInfo_UnknownZ;_tmp36C.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp35B);_tmp36C;});_tmp36B;});_LL20E:;}else{e->annot=(void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp36D=_cycalloc(sizeof(*_tmp36D));_tmp36D[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp36E;_tmp36E.tag=Cyc_CfFlowInfo_UnknownZ;_tmp36E.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp35B);_tmp36E;});_tmp36D;});}if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp35A,r)== (void*)0)({void*_tmp36F=0;Cyc_Tcutil_terr(e->loc,({const
char*_tmp370="dereference of possibly uninitialized pointer";_tag_dyneither(
_tmp370,sizeof(char),46);}),_tag_dyneither(_tmp36F,sizeof(void*),0));});return({
struct _tuple15 _tmp371;_tmp371.f1=f;_tmp371.f2=Cyc_CfFlowInfo_UnknownL();_tmp371;});
_LL209:;}_LL207:;_LL208:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp372=_cycalloc(sizeof(*_tmp372));_tmp372[0]=({struct Cyc_Core_Impossible_struct
_tmp373;_tmp373.tag=Cyc_Core_Impossible;_tmp373.f1=({const char*_tmp374="left deref of non-pointer-type";
_tag_dyneither(_tmp374,sizeof(char),31);});_tmp373;});_tmp372;}));_LL204:;}
static struct _tuple15 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*_tmp375=env->fenv;{union
Cyc_CfFlowInfo_FlowInfo _tmp376=inflow;int _tmp377;struct _tuple12 _tmp378;struct Cyc_Dict_Dict
_tmp379;struct Cyc_List_List*_tmp37A;_LL21C: if((_tmp376.BottomFL).tag != 1)goto
_LL21E;_tmp377=(int)(_tmp376.BottomFL).val;_LL21D: return({struct _tuple15 _tmp37B;
_tmp37B.f1=Cyc_CfFlowInfo_BottomFL();_tmp37B.f2=Cyc_CfFlowInfo_UnknownL();
_tmp37B;});_LL21E: if((_tmp376.ReachableFL).tag != 2)goto _LL21B;_tmp378=(struct
_tuple12)(_tmp376.ReachableFL).val;_tmp379=_tmp378.f1;_tmp37A=_tmp378.f2;_LL21F:
d=_tmp379;_LL21B:;}{void*_tmp37C=e->r;struct Cyc_Absyn_Exp*_tmp37D;struct Cyc_Absyn_Exp*
_tmp37E;struct Cyc_Absyn_Exp*_tmp37F;void*_tmp380;struct Cyc_Absyn_Vardecl*_tmp381;
void*_tmp382;struct Cyc_Absyn_Vardecl*_tmp383;void*_tmp384;struct Cyc_Absyn_Vardecl*
_tmp385;void*_tmp386;struct Cyc_Absyn_Vardecl*_tmp387;struct Cyc_Absyn_Exp*_tmp388;
struct _dyneither_ptr*_tmp389;struct Cyc_Absyn_Exp*_tmp38A;struct Cyc_Absyn_Exp*
_tmp38B;struct Cyc_Absyn_Exp*_tmp38C;struct Cyc_Absyn_Exp*_tmp38D;struct
_dyneither_ptr*_tmp38E;_LL221: if(*((int*)_tmp37C)!= 15)goto _LL223;_tmp37D=((
struct Cyc_Absyn_Cast_e_struct*)_tmp37C)->f2;_LL222: _tmp37E=_tmp37D;goto _LL224;
_LL223: if(*((int*)_tmp37C)!= 13)goto _LL225;_tmp37E=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp37C)->f1;_LL224: _tmp37F=_tmp37E;goto _LL226;_LL225: if(*((int*)_tmp37C)!= 14)
goto _LL227;_tmp37F=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp37C)->f1;_LL226:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp37F,flds);_LL227: if(*((int*)
_tmp37C)!= 1)goto _LL229;_tmp380=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp37C)->f2;
if(_tmp380 <= (void*)1)goto _LL229;if(*((int*)_tmp380)!= 2)goto _LL229;_tmp381=((
struct Cyc_Absyn_Param_b_struct*)_tmp380)->f1;_LL228: _tmp383=_tmp381;goto _LL22A;
_LL229: if(*((int*)_tmp37C)!= 1)goto _LL22B;_tmp382=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp37C)->f2;if(_tmp382 <= (void*)1)goto _LL22B;if(*((int*)_tmp382)!= 3)goto _LL22B;
_tmp383=((struct Cyc_Absyn_Local_b_struct*)_tmp382)->f1;_LL22A: _tmp385=_tmp383;
goto _LL22C;_LL22B: if(*((int*)_tmp37C)!= 1)goto _LL22D;_tmp384=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp37C)->f2;if(_tmp384 <= (void*)1)goto _LL22D;if(*((int*)_tmp384)!= 4)goto _LL22D;
_tmp385=((struct Cyc_Absyn_Pat_b_struct*)_tmp384)->f1;_LL22C: return({struct
_tuple15 _tmp38F;_tmp38F.f1=inflow;_tmp38F.f2=Cyc_CfFlowInfo_PlaceL(({struct Cyc_CfFlowInfo_Place*
_tmp390=_region_malloc(env->r,sizeof(*_tmp390));_tmp390->root=(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp391=_region_malloc(env->r,sizeof(*_tmp391));_tmp391[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp392;_tmp392.tag=0;_tmp392.f1=_tmp385;_tmp392;});_tmp391;});_tmp390->fields=
flds;_tmp390;}));_tmp38F;});_LL22D: if(*((int*)_tmp37C)!= 1)goto _LL22F;_tmp386=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp37C)->f2;if(_tmp386 <= (void*)1)goto
_LL22F;if(*((int*)_tmp386)!= 0)goto _LL22F;_tmp387=((struct Cyc_Absyn_Global_b_struct*)
_tmp386)->f1;_LL22E: return({struct _tuple15 _tmp393;_tmp393.f1=inflow;_tmp393.f2=
Cyc_CfFlowInfo_UnknownL();_tmp393;});_LL22F: if(*((int*)_tmp37C)!= 24)goto _LL231;
_tmp388=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp37C)->f1;_tmp389=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp37C)->f2;_LL230:{void*_tmp394=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp388->topt))->v);struct Cyc_Absyn_PtrInfo _tmp395;void*_tmp396;
_LL23A: if(_tmp394 <= (void*)4)goto _LL23C;if(*((int*)_tmp394)!= 4)goto _LL23C;
_tmp395=((struct Cyc_Absyn_PointerType_struct*)_tmp394)->f1;_tmp396=_tmp395.elt_typ;
_LL23B: if(!Cyc_Absyn_is_nontagged_union_type(_tmp396))flds=({struct Cyc_List_List*
_tmp397=_region_malloc(env->r,sizeof(*_tmp397));_tmp397->hd=(void*)Cyc_CfFlowInfo_get_field_index(
_tmp396,_tmp389);_tmp397->tl=flds;_tmp397;});goto _LL239;_LL23C:;_LL23D:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp398=_cycalloc(sizeof(*
_tmp398));_tmp398[0]=({struct Cyc_Core_Impossible_struct _tmp399;_tmp399.tag=Cyc_Core_Impossible;
_tmp399.f1=({const char*_tmp39A="anal_Rexp: AggrArrow ptr";_tag_dyneither(_tmp39A,
sizeof(char),25);});_tmp399;});_tmp398;}));_LL239:;}_tmp38A=_tmp388;goto _LL232;
_LL231: if(*((int*)_tmp37C)!= 22)goto _LL233;_tmp38A=((struct Cyc_Absyn_Deref_e_struct*)
_tmp37C)->f1;_LL232: {union Cyc_CfFlowInfo_FlowInfo _tmp39C;void*_tmp39D;struct
_tuple13 _tmp39B=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp38A);_tmp39C=_tmp39B.f1;
_tmp39D=_tmp39B.f2;_tmp39C=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp39C);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp38A,_tmp39C,_tmp39D,
flds);}_LL233: if(*((int*)_tmp37C)!= 25)goto _LL235;_tmp38B=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp37C)->f1;_tmp38C=((struct Cyc_Absyn_Subscript_e_struct*)_tmp37C)->f2;_LL234: {
void*_tmp39E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp38B->topt))->v);struct Cyc_Absyn_PtrInfo _tmp39F;struct Cyc_Absyn_PtrAtts
_tmp3A0;union Cyc_Absyn_Constraint*_tmp3A1;_LL23F: if(_tmp39E <= (void*)4)goto
_LL243;if(*((int*)_tmp39E)!= 9)goto _LL241;_LL240: {unsigned int _tmp3A2=(Cyc_Evexp_eval_const_uint_exp(
_tmp38C)).f1;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp38B,({struct
Cyc_List_List*_tmp3A3=_region_malloc(env->r,sizeof(*_tmp3A3));_tmp3A3->hd=(void*)((
int)_tmp3A2);_tmp3A3->tl=flds;_tmp3A3;}));}_LL241: if(*((int*)_tmp39E)!= 4)goto
_LL243;_tmp39F=((struct Cyc_Absyn_PointerType_struct*)_tmp39E)->f1;_tmp3A0=
_tmp39F.ptr_atts;_tmp3A1=_tmp3A0.bounds;_LL242: {struct _RegionHandle*_tmp3A4=env->r;
union Cyc_CfFlowInfo_FlowInfo _tmp3A7;struct Cyc_List_List*_tmp3A8;struct _tuple19
_tmp3A6=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp3A4,env,inflow,({struct Cyc_Absyn_Exp*
_tmp3A5[2];_tmp3A5[1]=_tmp38C;_tmp3A5[0]=_tmp38B;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp3A4,
_tag_dyneither(_tmp3A5,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp3A7=_tmp3A6.f1;
_tmp3A8=_tmp3A6.f2;_tmp3A7=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp38C->loc,
_tmp3A7);{union Cyc_CfFlowInfo_FlowInfo _tmp3A9=_tmp3A7;{union Cyc_CfFlowInfo_FlowInfo
_tmp3AA=_tmp3A7;struct _tuple12 _tmp3AB;struct Cyc_Dict_Dict _tmp3AC;struct Cyc_List_List*
_tmp3AD;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3AE;_LL246: if((_tmp3AA.ReachableFL).tag
!= 2)goto _LL248;_tmp3AB=(struct _tuple12)(_tmp3AA.ReachableFL).val;_tmp3AC=
_tmp3AB.f1;_tmp3AD=_tmp3AB.f2;_tmp3AE=_tmp3AB.f3;_LL247: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp3AC,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp3A8))->tl))->hd)== (void*)0)({void*_tmp3AF=0;Cyc_Tcutil_terr(
_tmp38C->loc,({const char*_tmp3B0="expression may not be initialized";
_tag_dyneither(_tmp3B0,sizeof(char),34);}),_tag_dyneither(_tmp3AF,sizeof(void*),
0));});{struct Cyc_List_List*_tmp3B1=Cyc_NewControlFlow_add_subscript_reln(
_tmp375->r,_tmp3AD,_tmp38B,_tmp38C);if(_tmp3AD != _tmp3B1)_tmp3A9=Cyc_CfFlowInfo_ReachableFL(
_tmp3AC,_tmp3B1,_tmp3AE);goto _LL245;}_LL248:;_LL249: goto _LL245;_LL245:;}{union
Cyc_CfFlowInfo_FlowInfo _tmp3B3;union Cyc_CfFlowInfo_AbsLVal _tmp3B4;struct _tuple15
_tmp3B2=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp38B,_tmp3A7,(void*)((
struct Cyc_List_List*)_check_null(_tmp3A8))->hd,flds);_tmp3B3=_tmp3B2.f1;_tmp3B4=
_tmp3B2.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp3B5=_tmp3B3;int _tmp3B6;_LL24B: if((
_tmp3B5.BottomFL).tag != 1)goto _LL24D;_tmp3B6=(int)(_tmp3B5.BottomFL).val;_LL24C:
return({struct _tuple15 _tmp3B7;_tmp3B7.f1=_tmp3B3;_tmp3B7.f2=_tmp3B4;_tmp3B7;});
_LL24D:;_LL24E: return({struct _tuple15 _tmp3B8;_tmp3B8.f1=_tmp3A9;_tmp3B8.f2=
_tmp3B4;_tmp3B8;});_LL24A:;}}}}_LL243:;_LL244:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9[0]=({struct Cyc_Core_Impossible_struct
_tmp3BA;_tmp3BA.tag=Cyc_Core_Impossible;_tmp3BA.f1=({const char*_tmp3BB="anal_Lexp: Subscript -- bad type";
_tag_dyneither(_tmp3BB,sizeof(char),33);});_tmp3BA;});_tmp3B9;}));_LL23E:;}
_LL235: if(*((int*)_tmp37C)!= 23)goto _LL237;_tmp38D=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp37C)->f1;_tmp38E=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp37C)->f2;_LL236:
if(Cyc_Absyn_is_nontagged_union_type((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp38D->topt))->v))return({struct _tuple15 _tmp3BC;_tmp3BC.f1=inflow;_tmp3BC.f2=
Cyc_CfFlowInfo_UnknownL();_tmp3BC;});return Cyc_NewControlFlow_anal_Lexp_rec(env,
inflow,_tmp38D,({struct Cyc_List_List*_tmp3BD=_region_malloc(env->r,sizeof(*
_tmp3BD));_tmp3BD->hd=(void*)Cyc_CfFlowInfo_get_field_index((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp38D->topt))->v,_tmp38E);_tmp3BD->tl=flds;_tmp3BD;}));_LL237:;
_LL238: return({struct _tuple15 _tmp3BE;_tmp3BE.f1=Cyc_CfFlowInfo_BottomFL();
_tmp3BE.f2=Cyc_CfFlowInfo_UnknownL();_tmp3BE;});_LL220:;}}static struct _tuple15
Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_ConsumeInfo _tmp3C0;struct
_tuple14 _tmp3BF=Cyc_CfFlowInfo_save_consume_info(env->fenv,inflow,0);_tmp3C0=
_tmp3BF.f1;{union Cyc_CfFlowInfo_FlowInfo _tmp3C2;union Cyc_CfFlowInfo_AbsLVal
_tmp3C3;struct _tuple15 _tmp3C1=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,e,0);
_tmp3C2=_tmp3C1.f1;_tmp3C3=_tmp3C1.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp3C5;
struct _tuple14 _tmp3C4=Cyc_CfFlowInfo_save_consume_info(env->fenv,inflow,0);
_tmp3C5=_tmp3C4.f1;if(_tmp3C0.may_consume != _tmp3C5.may_consume  || (_tmp3C0.consumed).t
!= (_tmp3C5.consumed).t)({void*_tmp3C6=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp3C7="oops: anal_Lexp modified consume lists";
_tag_dyneither(_tmp3C7,sizeof(char),39);}),_tag_dyneither(_tmp3C6,sizeof(void*),
0));});return({struct _tuple15 _tmp3C8;_tmp3C8.f1=_tmp3C2;_tmp3C8.f2=_tmp3C3;
_tmp3C8;});}}}static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp3C9=env->fenv;{void*_tmp3CA=e->r;struct Cyc_Absyn_Exp*_tmp3CB;struct Cyc_Absyn_Exp*
_tmp3CC;struct Cyc_Absyn_Exp*_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CE;struct Cyc_Absyn_Exp*
_tmp3CF;struct Cyc_Absyn_Exp*_tmp3D0;struct Cyc_Absyn_Exp*_tmp3D1;struct Cyc_Absyn_Exp*
_tmp3D2;struct Cyc_Absyn_Exp*_tmp3D3;void*_tmp3D4;struct Cyc_List_List*_tmp3D5;
struct Cyc_List_List _tmp3D6;struct Cyc_Absyn_Exp*_tmp3D7;struct Cyc_List_List*
_tmp3D8;void*_tmp3D9;struct Cyc_List_List*_tmp3DA;_LL250: if(*((int*)_tmp3CA)!= 6)
goto _LL252;_tmp3CB=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3CA)->f1;_tmp3CC=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp3CA)->f2;_tmp3CD=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp3CA)->f3;_LL251: {union Cyc_CfFlowInfo_FlowInfo _tmp3DC;union Cyc_CfFlowInfo_FlowInfo
_tmp3DD;struct _tuple16 _tmp3DB=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3CB);
_tmp3DC=_tmp3DB.f1;_tmp3DD=_tmp3DB.f2;_tmp3DC=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3CB->loc,_tmp3DC);_tmp3DD=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3CB->loc,
_tmp3DD);{union Cyc_CfFlowInfo_FlowInfo _tmp3DF;union Cyc_CfFlowInfo_FlowInfo
_tmp3E0;struct _tuple16 _tmp3DE=Cyc_NewControlFlow_anal_test(env,_tmp3DC,_tmp3CC);
_tmp3DF=_tmp3DE.f1;_tmp3E0=_tmp3DE.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp3E2;union
Cyc_CfFlowInfo_FlowInfo _tmp3E3;struct _tuple16 _tmp3E1=Cyc_NewControlFlow_anal_test(
env,_tmp3DD,_tmp3CD);_tmp3E2=_tmp3E1.f1;_tmp3E3=_tmp3E1.f2;return({struct
_tuple16 _tmp3E4;_tmp3E4.f1=Cyc_CfFlowInfo_join_flow(_tmp3C9,env->all_changed,
_tmp3DF,_tmp3E2,1);_tmp3E4.f2=Cyc_CfFlowInfo_join_flow(_tmp3C9,env->all_changed,
_tmp3E0,_tmp3E3,1);_tmp3E4;});}}}_LL252: if(*((int*)_tmp3CA)!= 7)goto _LL254;
_tmp3CE=((struct Cyc_Absyn_And_e_struct*)_tmp3CA)->f1;_tmp3CF=((struct Cyc_Absyn_And_e_struct*)
_tmp3CA)->f2;_LL253: {union Cyc_CfFlowInfo_FlowInfo _tmp3E6;union Cyc_CfFlowInfo_FlowInfo
_tmp3E7;struct _tuple16 _tmp3E5=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3CE);
_tmp3E6=_tmp3E5.f1;_tmp3E7=_tmp3E5.f2;_tmp3E6=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3CE->loc,_tmp3E6);_tmp3E7=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3CE->loc,
_tmp3E7);{union Cyc_CfFlowInfo_FlowInfo _tmp3E9;union Cyc_CfFlowInfo_FlowInfo
_tmp3EA;struct _tuple16 _tmp3E8=Cyc_NewControlFlow_anal_test(env,_tmp3E6,_tmp3CF);
_tmp3E9=_tmp3E8.f1;_tmp3EA=_tmp3E8.f2;_tmp3E9=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3CF->loc,_tmp3E9);_tmp3EA=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3CF->loc,
_tmp3EA);return({struct _tuple16 _tmp3EB;_tmp3EB.f1=_tmp3E9;_tmp3EB.f2=Cyc_CfFlowInfo_join_flow(
_tmp3C9,env->all_changed,_tmp3E7,_tmp3EA,0);_tmp3EB;});}}_LL254: if(*((int*)
_tmp3CA)!= 8)goto _LL256;_tmp3D0=((struct Cyc_Absyn_Or_e_struct*)_tmp3CA)->f1;
_tmp3D1=((struct Cyc_Absyn_Or_e_struct*)_tmp3CA)->f2;_LL255: {union Cyc_CfFlowInfo_FlowInfo
_tmp3ED;union Cyc_CfFlowInfo_FlowInfo _tmp3EE;struct _tuple16 _tmp3EC=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3D0);_tmp3ED=_tmp3EC.f1;_tmp3EE=_tmp3EC.f2;_tmp3ED=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3D0->loc,_tmp3ED);_tmp3EE=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3D0->loc,
_tmp3EE);{union Cyc_CfFlowInfo_FlowInfo _tmp3F0;union Cyc_CfFlowInfo_FlowInfo
_tmp3F1;struct _tuple16 _tmp3EF=Cyc_NewControlFlow_anal_test(env,_tmp3EE,_tmp3D1);
_tmp3F0=_tmp3EF.f1;_tmp3F1=_tmp3EF.f2;_tmp3F0=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3D1->loc,_tmp3F0);_tmp3F1=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3D1->loc,
_tmp3F1);return({struct _tuple16 _tmp3F2;_tmp3F2.f1=Cyc_CfFlowInfo_join_flow(
_tmp3C9,env->all_changed,_tmp3ED,_tmp3F0,0);_tmp3F2.f2=_tmp3F1;_tmp3F2;});}}
_LL256: if(*((int*)_tmp3CA)!= 9)goto _LL258;_tmp3D2=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp3CA)->f1;_tmp3D3=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp3CA)->f2;_LL257: {
union Cyc_CfFlowInfo_FlowInfo _tmp3F4;void*_tmp3F5;struct _tuple13 _tmp3F3=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp3D2);_tmp3F4=_tmp3F3.f1;_tmp3F5=_tmp3F3.f2;_tmp3F4=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp3D2->loc,_tmp3F4);return Cyc_NewControlFlow_anal_test(env,_tmp3F4,_tmp3D3);}
_LL258: if(*((int*)_tmp3CA)!= 3)goto _LL25A;_tmp3D4=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3CA)->f1;if((int)_tmp3D4 != 11)goto _LL25A;_tmp3D5=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3CA)->f2;if(_tmp3D5 == 0)goto _LL25A;_tmp3D6=*_tmp3D5;_tmp3D7=(struct Cyc_Absyn_Exp*)
_tmp3D6.hd;_tmp3D8=_tmp3D6.tl;if(_tmp3D8 != 0)goto _LL25A;_LL259: {union Cyc_CfFlowInfo_FlowInfo
_tmp3F7;union Cyc_CfFlowInfo_FlowInfo _tmp3F8;struct _tuple16 _tmp3F6=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3D7);_tmp3F7=_tmp3F6.f1;_tmp3F8=_tmp3F6.f2;return({struct _tuple16
_tmp3F9;_tmp3F9.f1=_tmp3F8;_tmp3F9.f2=_tmp3F7;_tmp3F9;});}_LL25A: if(*((int*)
_tmp3CA)!= 3)goto _LL25C;_tmp3D9=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3CA)->f1;_tmp3DA=((struct Cyc_Absyn_Primop_e_struct*)_tmp3CA)->f2;_LL25B: {
void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo f;struct _RegionHandle*_tmp3FA=env->r;{
union Cyc_CfFlowInfo_FlowInfo _tmp3FC;struct Cyc_List_List*_tmp3FD;struct _tuple19
_tmp3FB=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp3FA,env,inflow,_tmp3DA,0);
_tmp3FC=_tmp3FB.f1;_tmp3FD=_tmp3FB.f2;r1=(void*)((struct Cyc_List_List*)
_check_null(_tmp3FD))->hd;r2=(void*)((struct Cyc_List_List*)_check_null(_tmp3FD->tl))->hd;
f=_tmp3FC;}{union Cyc_CfFlowInfo_FlowInfo _tmp3FE=f;int _tmp3FF;struct _tuple12
_tmp400;struct Cyc_Dict_Dict _tmp401;struct Cyc_List_List*_tmp402;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp403;_LL25F: if((_tmp3FE.BottomFL).tag != 1)goto _LL261;_tmp3FF=(int)(_tmp3FE.BottomFL).val;
_LL260: return({struct _tuple16 _tmp404;_tmp404.f1=f;_tmp404.f2=f;_tmp404;});_LL261:
if((_tmp3FE.ReachableFL).tag != 2)goto _LL25E;_tmp400=(struct _tuple12)(_tmp3FE.ReachableFL).val;
_tmp401=_tmp400.f1;_tmp402=_tmp400.f2;_tmp403=_tmp400.f3;_LL262: {struct Cyc_Absyn_Exp*
_tmp405=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3DA))->hd;
struct Cyc_Absyn_Exp*_tmp406=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3DA->tl))->hd;if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp401,r1)
== (void*)0)({void*_tmp407=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp3DA->hd)->loc,({
const char*_tmp408="expression may not be initialized";_tag_dyneither(_tmp408,
sizeof(char),34);}),_tag_dyneither(_tmp407,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp401,r2)== (void*)0)({void*_tmp409=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3DA->tl))->hd)->loc,({const char*_tmp40A="expression may not be initialized";
_tag_dyneither(_tmp40A,sizeof(char),34);}),_tag_dyneither(_tmp409,sizeof(void*),
0));});if(_tmp3D9 == (void*)5  || _tmp3D9 == (void*)6){struct _tuple0 _tmp40C=({
struct _tuple0 _tmp40B;_tmp40B.f1=r1;_tmp40B.f2=r2;_tmp40B;});void*_tmp40D;void*
_tmp40E;void*_tmp40F;void*_tmp410;void*_tmp411;void*_tmp412;void*_tmp413;void*
_tmp414;void*_tmp415;void*_tmp416;void*_tmp417;void*_tmp418;void*_tmp419;void*
_tmp41A;void*_tmp41B;void*_tmp41C;void*_tmp41D;void*_tmp41E;void*_tmp41F;void*
_tmp420;_LL264: _tmp40D=_tmp40C.f1;if(_tmp40D <= (void*)3)goto _LL266;if(*((int*)
_tmp40D)!= 0)goto _LL266;_tmp40E=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp40D)->f1;_tmp40F=_tmp40C.f2;if((int)_tmp40F != 0)goto _LL266;_LL265: {union Cyc_CfFlowInfo_FlowInfo
_tmp422;union Cyc_CfFlowInfo_FlowInfo _tmp423;struct _tuple16 _tmp421=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp405,_tmp40E);_tmp422=_tmp421.f1;_tmp423=_tmp421.f2;{void*_tmp424=
_tmp3D9;_LL279: if((int)_tmp424 != 5)goto _LL27B;_LL27A: return({struct _tuple16
_tmp425;_tmp425.f1=_tmp423;_tmp425.f2=_tmp422;_tmp425;});_LL27B: if((int)_tmp424
!= 6)goto _LL27D;_LL27C: return({struct _tuple16 _tmp426;_tmp426.f1=_tmp422;_tmp426.f2=
_tmp423;_tmp426;});_LL27D:;_LL27E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp427=_cycalloc(sizeof(*_tmp427));_tmp427[0]=({struct Cyc_Core_Impossible_struct
_tmp428;_tmp428.tag=Cyc_Core_Impossible;_tmp428.f1=({const char*_tmp429="anal_test, zero-split";
_tag_dyneither(_tmp429,sizeof(char),22);});_tmp428;});_tmp427;}));_LL278:;}}
_LL266: _tmp410=_tmp40C.f1;if((int)_tmp410 != 0)goto _LL268;_tmp411=_tmp40C.f2;if(
_tmp411 <= (void*)3)goto _LL268;if(*((int*)_tmp411)!= 0)goto _LL268;_tmp412=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp411)->f1;_LL267: {union Cyc_CfFlowInfo_FlowInfo
_tmp42B;union Cyc_CfFlowInfo_FlowInfo _tmp42C;struct _tuple16 _tmp42A=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp406,_tmp412);_tmp42B=_tmp42A.f1;_tmp42C=_tmp42A.f2;{void*_tmp42D=
_tmp3D9;_LL280: if((int)_tmp42D != 5)goto _LL282;_LL281: return({struct _tuple16
_tmp42E;_tmp42E.f1=_tmp42C;_tmp42E.f2=_tmp42B;_tmp42E;});_LL282: if((int)_tmp42D
!= 6)goto _LL284;_LL283: return({struct _tuple16 _tmp42F;_tmp42F.f1=_tmp42B;_tmp42F.f2=
_tmp42C;_tmp42F;});_LL284:;_LL285:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp430=_cycalloc(sizeof(*_tmp430));_tmp430[0]=({struct Cyc_Core_Impossible_struct
_tmp431;_tmp431.tag=Cyc_Core_Impossible;_tmp431.f1=({const char*_tmp432="anal_test, zero-split";
_tag_dyneither(_tmp432,sizeof(char),22);});_tmp431;});_tmp430;}));_LL27F:;}}
_LL268: _tmp413=_tmp40C.f1;if((int)_tmp413 != 0)goto _LL26A;_tmp414=_tmp40C.f2;if((
int)_tmp414 != 0)goto _LL26A;_LL269: if(_tmp3D9 == (void*)5)return({struct _tuple16
_tmp433;_tmp433.f1=f;_tmp433.f2=Cyc_CfFlowInfo_BottomFL();_tmp433;});else{return({
struct _tuple16 _tmp434;_tmp434.f1=Cyc_CfFlowInfo_BottomFL();_tmp434.f2=f;_tmp434;});}
_LL26A: _tmp415=_tmp40C.f1;if((int)_tmp415 != 0)goto _LL26C;_tmp416=_tmp40C.f2;if((
int)_tmp416 != 1)goto _LL26C;_LL26B: goto _LL26D;_LL26C: _tmp417=_tmp40C.f1;if((int)
_tmp417 != 0)goto _LL26E;_tmp418=_tmp40C.f2;if((int)_tmp418 != 2)goto _LL26E;_LL26D:
goto _LL26F;_LL26E: _tmp419=_tmp40C.f1;if((int)_tmp419 != 0)goto _LL270;_tmp41A=
_tmp40C.f2;if(_tmp41A <= (void*)3)goto _LL270;if(*((int*)_tmp41A)!= 2)goto _LL270;
_LL26F: goto _LL271;_LL270: _tmp41B=_tmp40C.f1;if((int)_tmp41B != 1)goto _LL272;
_tmp41C=_tmp40C.f2;if((int)_tmp41C != 0)goto _LL272;_LL271: goto _LL273;_LL272:
_tmp41D=_tmp40C.f1;if((int)_tmp41D != 2)goto _LL274;_tmp41E=_tmp40C.f2;if((int)
_tmp41E != 0)goto _LL274;_LL273: goto _LL275;_LL274: _tmp41F=_tmp40C.f1;if(_tmp41F <= (
void*)3)goto _LL276;if(*((int*)_tmp41F)!= 2)goto _LL276;_tmp420=_tmp40C.f2;if((int)
_tmp420 != 0)goto _LL276;_LL275: if(_tmp3D9 == (void*)6)return({struct _tuple16
_tmp435;_tmp435.f1=f;_tmp435.f2=Cyc_CfFlowInfo_BottomFL();_tmp435;});else{return({
struct _tuple16 _tmp436;_tmp436.f1=Cyc_CfFlowInfo_BottomFL();_tmp436.f2=f;_tmp436;});}
_LL276:;_LL277: goto _LL263;_LL263:;}{struct _tuple0 _tmp438=({struct _tuple0 _tmp437;
_tmp437.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp405->topt))->v);
_tmp437.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp406->topt))->v);
_tmp437;});void*_tmp439;void*_tmp43A;void*_tmp43B;void*_tmp43C;void*_tmp43D;void*
_tmp43E;_LL287: _tmp439=_tmp438.f1;if(_tmp439 <= (void*)4)goto _LL289;if(*((int*)
_tmp439)!= 5)goto _LL289;_tmp43A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp439)->f1;
if((int)_tmp43A != 1)goto _LL289;_LL288: goto _LL28A;_LL289: _tmp43B=_tmp438.f2;if(
_tmp43B <= (void*)4)goto _LL28B;if(*((int*)_tmp43B)!= 5)goto _LL28B;_tmp43C=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp43B)->f1;if((int)_tmp43C != 1)goto _LL28B;
_LL28A: goto _LL28C;_LL28B: _tmp43D=_tmp438.f1;if(_tmp43D <= (void*)4)goto _LL28D;if(*((
int*)_tmp43D)!= 18)goto _LL28D;_LL28C: goto _LL28E;_LL28D: _tmp43E=_tmp438.f2;if(
_tmp43E <= (void*)4)goto _LL28F;if(*((int*)_tmp43E)!= 18)goto _LL28F;_LL28E: goto
_LL286;_LL28F:;_LL290: return({struct _tuple16 _tmp43F;_tmp43F.f1=f;_tmp43F.f2=f;
_tmp43F;});_LL286:;}{void*_tmp440=_tmp3D9;_LL292: if((int)_tmp440 != 5)goto _LL294;
_LL293: {union Cyc_CfFlowInfo_FlowInfo _tmp441=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp405,r1,(void*)5,r2);_tmp441=Cyc_NewControlFlow_if_tagcmp(env,_tmp441,_tmp406,
r2,(void*)5,r1);return({struct _tuple16 _tmp442;_tmp442.f1=_tmp441;_tmp442.f2=f;
_tmp442;});}_LL294: if((int)_tmp440 != 6)goto _LL296;_LL295: {union Cyc_CfFlowInfo_FlowInfo
_tmp443=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp405,r1,(void*)5,r2);_tmp443=Cyc_NewControlFlow_if_tagcmp(
env,_tmp443,_tmp406,r2,(void*)5,r1);return({struct _tuple16 _tmp444;_tmp444.f1=f;
_tmp444.f2=_tmp443;_tmp444;});}_LL296: if((int)_tmp440 != 7)goto _LL298;_LL297: {
union Cyc_CfFlowInfo_FlowInfo _tmp445=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp406,
r2,(void*)8,r1);union Cyc_CfFlowInfo_FlowInfo _tmp446=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp405,r1,(void*)10,r2);return({struct _tuple16 _tmp447;_tmp447.f1=_tmp445;
_tmp447.f2=_tmp446;_tmp447;});}_LL298: if((int)_tmp440 != 9)goto _LL29A;_LL299: {
union Cyc_CfFlowInfo_FlowInfo _tmp448=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp406,
r2,(void*)10,r1);union Cyc_CfFlowInfo_FlowInfo _tmp449=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp405,r1,(void*)8,r2);return({struct _tuple16 _tmp44A;_tmp44A.f1=_tmp448;
_tmp44A.f2=_tmp449;_tmp44A;});}_LL29A: if((int)_tmp440 != 8)goto _LL29C;_LL29B: {
union Cyc_CfFlowInfo_FlowInfo _tmp44B=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp405,
r1,(void*)8,r2);union Cyc_CfFlowInfo_FlowInfo _tmp44C=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp406,r2,(void*)10,r1);{union Cyc_CfFlowInfo_FlowInfo _tmp44D=_tmp44B;int
_tmp44E;struct _tuple12 _tmp44F;struct Cyc_Dict_Dict _tmp450;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp451;_LL2A1: if((_tmp44D.BottomFL).tag != 1)goto _LL2A3;_tmp44E=(int)(_tmp44D.BottomFL).val;
_LL2A2:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp452=_cycalloc(
sizeof(*_tmp452));_tmp452[0]=({struct Cyc_Core_Impossible_struct _tmp453;_tmp453.tag=
Cyc_Core_Impossible;_tmp453.f1=({const char*_tmp454="anal_test, Lt";
_tag_dyneither(_tmp454,sizeof(char),14);});_tmp453;});_tmp452;}));_LL2A3: if((
_tmp44D.ReachableFL).tag != 2)goto _LL2A0;_tmp44F=(struct _tuple12)(_tmp44D.ReachableFL).val;
_tmp450=_tmp44F.f1;_tmp451=_tmp44F.f3;_LL2A4: _tmp401=_tmp450;_tmp403=_tmp451;
_LL2A0:;}{void*_tmp455=_tmp405->r;void*_tmp456;struct Cyc_Absyn_Vardecl*_tmp457;
void*_tmp458;struct Cyc_Absyn_Vardecl*_tmp459;void*_tmp45A;struct Cyc_Absyn_Vardecl*
_tmp45B;void*_tmp45C;struct Cyc_Absyn_Vardecl*_tmp45D;_LL2A6: if(*((int*)_tmp455)
!= 1)goto _LL2A8;_tmp456=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp455)->f2;if(
_tmp456 <= (void*)1)goto _LL2A8;if(*((int*)_tmp456)!= 0)goto _LL2A8;_tmp457=((
struct Cyc_Absyn_Global_b_struct*)_tmp456)->f1;if(!(!_tmp457->escapes))goto _LL2A8;
_LL2A7: _tmp459=_tmp457;goto _LL2A9;_LL2A8: if(*((int*)_tmp455)!= 1)goto _LL2AA;
_tmp458=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp455)->f2;if(_tmp458 <= (void*)
1)goto _LL2AA;if(*((int*)_tmp458)!= 3)goto _LL2AA;_tmp459=((struct Cyc_Absyn_Local_b_struct*)
_tmp458)->f1;if(!(!_tmp459->escapes))goto _LL2AA;_LL2A9: _tmp45B=_tmp459;goto
_LL2AB;_LL2AA: if(*((int*)_tmp455)!= 1)goto _LL2AC;_tmp45A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp455)->f2;if(_tmp45A <= (void*)1)goto _LL2AC;if(*((int*)_tmp45A)!= 4)goto _LL2AC;
_tmp45B=((struct Cyc_Absyn_Pat_b_struct*)_tmp45A)->f1;if(!(!_tmp45B->escapes))
goto _LL2AC;_LL2AB: _tmp45D=_tmp45B;goto _LL2AD;_LL2AC: if(*((int*)_tmp455)!= 1)goto
_LL2AE;_tmp45C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp455)->f2;if(_tmp45C <= (
void*)1)goto _LL2AE;if(*((int*)_tmp45C)!= 2)goto _LL2AE;_tmp45D=((struct Cyc_Absyn_Param_b_struct*)
_tmp45C)->f1;if(!(!_tmp45D->escapes))goto _LL2AE;_LL2AD: {void*_tmp45E=_tmp406->r;
void*_tmp45F;struct Cyc_Absyn_Vardecl*_tmp460;void*_tmp461;struct Cyc_Absyn_Vardecl*
_tmp462;void*_tmp463;struct Cyc_Absyn_Vardecl*_tmp464;void*_tmp465;struct Cyc_Absyn_Vardecl*
_tmp466;union Cyc_Absyn_Cnst _tmp467;struct _tuple6 _tmp468;int _tmp469;void*_tmp46A;
struct Cyc_List_List*_tmp46B;struct Cyc_List_List _tmp46C;struct Cyc_Absyn_Exp*
_tmp46D;_LL2B1: if(*((int*)_tmp45E)!= 1)goto _LL2B3;_tmp45F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp45E)->f2;if(_tmp45F <= (void*)1)goto _LL2B3;if(*((int*)_tmp45F)!= 0)goto _LL2B3;
_tmp460=((struct Cyc_Absyn_Global_b_struct*)_tmp45F)->f1;if(!(!_tmp460->escapes))
goto _LL2B3;_LL2B2: _tmp462=_tmp460;goto _LL2B4;_LL2B3: if(*((int*)_tmp45E)!= 1)goto
_LL2B5;_tmp461=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp45E)->f2;if(_tmp461 <= (
void*)1)goto _LL2B5;if(*((int*)_tmp461)!= 3)goto _LL2B5;_tmp462=((struct Cyc_Absyn_Local_b_struct*)
_tmp461)->f1;if(!(!_tmp462->escapes))goto _LL2B5;_LL2B4: _tmp464=_tmp462;goto
_LL2B6;_LL2B5: if(*((int*)_tmp45E)!= 1)goto _LL2B7;_tmp463=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp45E)->f2;if(_tmp463 <= (void*)1)goto _LL2B7;if(*((int*)_tmp463)!= 4)goto _LL2B7;
_tmp464=((struct Cyc_Absyn_Pat_b_struct*)_tmp463)->f1;if(!(!_tmp464->escapes))
goto _LL2B7;_LL2B6: _tmp466=_tmp464;goto _LL2B8;_LL2B7: if(*((int*)_tmp45E)!= 1)goto
_LL2B9;_tmp465=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp45E)->f2;if(_tmp465 <= (
void*)1)goto _LL2B9;if(*((int*)_tmp465)!= 2)goto _LL2B9;_tmp466=((struct Cyc_Absyn_Param_b_struct*)
_tmp465)->f1;if(!(!_tmp466->escapes))goto _LL2B9;_LL2B8: {struct _RegionHandle*
_tmp46E=(env->fenv)->r;struct Cyc_List_List*_tmp46F=({struct Cyc_List_List*_tmp471=
_region_malloc(_tmp46E,sizeof(*_tmp471));_tmp471->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp472=_region_malloc(_tmp46E,sizeof(*_tmp472));_tmp472->vd=_tmp45D;_tmp472->rop=
Cyc_CfFlowInfo_LessVar(_tmp466,_tmp466->type);_tmp472;});_tmp471->tl=_tmp402;
_tmp471;});return({struct _tuple16 _tmp470;_tmp470.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp401,_tmp46F,_tmp403);_tmp470.f2=_tmp44C;_tmp470;});}_LL2B9: if(*((int*)
_tmp45E)!= 0)goto _LL2BB;_tmp467=((struct Cyc_Absyn_Const_e_struct*)_tmp45E)->f1;
if((_tmp467.Int_c).tag != 4)goto _LL2BB;_tmp468=(struct _tuple6)(_tmp467.Int_c).val;
_tmp469=_tmp468.f2;_LL2BA: {struct _RegionHandle*_tmp473=(env->fenv)->r;struct Cyc_List_List*
_tmp474=({struct Cyc_List_List*_tmp476=_region_malloc(_tmp473,sizeof(*_tmp476));
_tmp476->hd=({struct Cyc_CfFlowInfo_Reln*_tmp477=_region_malloc(_tmp473,sizeof(*
_tmp477));_tmp477->vd=_tmp45D;_tmp477->rop=Cyc_CfFlowInfo_LessConst((
unsigned int)_tmp469);_tmp477;});_tmp476->tl=_tmp402;_tmp476;});return({struct
_tuple16 _tmp475;_tmp475.f1=Cyc_CfFlowInfo_ReachableFL(_tmp401,_tmp474,_tmp403);
_tmp475.f2=_tmp44C;_tmp475;});}_LL2BB: if(*((int*)_tmp45E)!= 3)goto _LL2BD;_tmp46A=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp45E)->f1;_tmp46B=((struct Cyc_Absyn_Primop_e_struct*)
_tmp45E)->f2;if(_tmp46B == 0)goto _LL2BD;_tmp46C=*_tmp46B;_tmp46D=(struct Cyc_Absyn_Exp*)
_tmp46C.hd;_LL2BC: {void*_tmp478=_tmp46D->r;void*_tmp479;struct Cyc_Absyn_Vardecl*
_tmp47A;void*_tmp47B;struct Cyc_Absyn_Vardecl*_tmp47C;void*_tmp47D;struct Cyc_Absyn_Vardecl*
_tmp47E;void*_tmp47F;struct Cyc_Absyn_Vardecl*_tmp480;_LL2C0: if(*((int*)_tmp478)
!= 1)goto _LL2C2;_tmp479=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp478)->f2;if(
_tmp479 <= (void*)1)goto _LL2C2;if(*((int*)_tmp479)!= 0)goto _LL2C2;_tmp47A=((
struct Cyc_Absyn_Global_b_struct*)_tmp479)->f1;if(!(!_tmp47A->escapes))goto _LL2C2;
_LL2C1: _tmp47C=_tmp47A;goto _LL2C3;_LL2C2: if(*((int*)_tmp478)!= 1)goto _LL2C4;
_tmp47B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp478)->f2;if(_tmp47B <= (void*)
1)goto _LL2C4;if(*((int*)_tmp47B)!= 3)goto _LL2C4;_tmp47C=((struct Cyc_Absyn_Local_b_struct*)
_tmp47B)->f1;if(!(!_tmp47C->escapes))goto _LL2C4;_LL2C3: _tmp47E=_tmp47C;goto
_LL2C5;_LL2C4: if(*((int*)_tmp478)!= 1)goto _LL2C6;_tmp47D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp478)->f2;if(_tmp47D <= (void*)1)goto _LL2C6;if(*((int*)_tmp47D)!= 4)goto _LL2C6;
_tmp47E=((struct Cyc_Absyn_Pat_b_struct*)_tmp47D)->f1;if(!(!_tmp47E->escapes))
goto _LL2C6;_LL2C5: _tmp480=_tmp47E;goto _LL2C7;_LL2C6: if(*((int*)_tmp478)!= 1)goto
_LL2C8;_tmp47F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp478)->f2;if(_tmp47F <= (
void*)1)goto _LL2C8;if(*((int*)_tmp47F)!= 2)goto _LL2C8;_tmp480=((struct Cyc_Absyn_Param_b_struct*)
_tmp47F)->f1;if(!(!_tmp480->escapes))goto _LL2C8;_LL2C7: {struct _RegionHandle*
_tmp481=(env->fenv)->r;struct Cyc_List_List*_tmp482=({struct Cyc_List_List*_tmp484=
_region_malloc(_tmp481,sizeof(*_tmp484));_tmp484->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp485=_region_malloc(_tmp481,sizeof(*_tmp485));_tmp485->vd=_tmp45D;_tmp485->rop=
Cyc_CfFlowInfo_LessNumelts(_tmp480);_tmp485;});_tmp484->tl=_tmp402;_tmp484;});
return({struct _tuple16 _tmp483;_tmp483.f1=Cyc_CfFlowInfo_ReachableFL(_tmp401,
_tmp482,_tmp403);_tmp483.f2=_tmp44C;_tmp483;});}_LL2C8:;_LL2C9: return({struct
_tuple16 _tmp486;_tmp486.f1=_tmp44B;_tmp486.f2=_tmp44C;_tmp486;});_LL2BF:;}_LL2BD:;
_LL2BE: return({struct _tuple16 _tmp487;_tmp487.f1=_tmp44B;_tmp487.f2=_tmp44C;
_tmp487;});_LL2B0:;}_LL2AE:;_LL2AF: return({struct _tuple16 _tmp488;_tmp488.f1=
_tmp44B;_tmp488.f2=_tmp44C;_tmp488;});_LL2A5:;}}_LL29C: if((int)_tmp440 != 10)goto
_LL29E;_LL29D: {union Cyc_CfFlowInfo_FlowInfo _tmp489=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp405,r1,(void*)10,r2);union Cyc_CfFlowInfo_FlowInfo _tmp48A=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp406,r1,(void*)8,r1);{union Cyc_CfFlowInfo_FlowInfo _tmp48B=_tmp489;int
_tmp48C;struct _tuple12 _tmp48D;struct Cyc_Dict_Dict _tmp48E;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp48F;_LL2CB: if((_tmp48B.BottomFL).tag != 1)goto _LL2CD;_tmp48C=(int)(_tmp48B.BottomFL).val;
_LL2CC:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp490=_cycalloc(
sizeof(*_tmp490));_tmp490[0]=({struct Cyc_Core_Impossible_struct _tmp491;_tmp491.tag=
Cyc_Core_Impossible;_tmp491.f1=({const char*_tmp492="anal_test, Lte";
_tag_dyneither(_tmp492,sizeof(char),15);});_tmp491;});_tmp490;}));_LL2CD: if((
_tmp48B.ReachableFL).tag != 2)goto _LL2CA;_tmp48D=(struct _tuple12)(_tmp48B.ReachableFL).val;
_tmp48E=_tmp48D.f1;_tmp48F=_tmp48D.f3;_LL2CE: _tmp401=_tmp48E;_tmp403=_tmp48F;
_LL2CA:;}{void*_tmp493=_tmp405->r;void*_tmp494;struct Cyc_Absyn_Vardecl*_tmp495;
void*_tmp496;struct Cyc_Absyn_Vardecl*_tmp497;void*_tmp498;struct Cyc_Absyn_Vardecl*
_tmp499;void*_tmp49A;struct Cyc_Absyn_Vardecl*_tmp49B;_LL2D0: if(*((int*)_tmp493)
!= 1)goto _LL2D2;_tmp494=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp493)->f2;if(
_tmp494 <= (void*)1)goto _LL2D2;if(*((int*)_tmp494)!= 0)goto _LL2D2;_tmp495=((
struct Cyc_Absyn_Global_b_struct*)_tmp494)->f1;if(!(!_tmp495->escapes))goto _LL2D2;
_LL2D1: _tmp497=_tmp495;goto _LL2D3;_LL2D2: if(*((int*)_tmp493)!= 1)goto _LL2D4;
_tmp496=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp493)->f2;if(_tmp496 <= (void*)
1)goto _LL2D4;if(*((int*)_tmp496)!= 3)goto _LL2D4;_tmp497=((struct Cyc_Absyn_Local_b_struct*)
_tmp496)->f1;if(!(!_tmp497->escapes))goto _LL2D4;_LL2D3: _tmp499=_tmp497;goto
_LL2D5;_LL2D4: if(*((int*)_tmp493)!= 1)goto _LL2D6;_tmp498=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp493)->f2;if(_tmp498 <= (void*)1)goto _LL2D6;if(*((int*)_tmp498)!= 4)goto _LL2D6;
_tmp499=((struct Cyc_Absyn_Pat_b_struct*)_tmp498)->f1;if(!(!_tmp499->escapes))
goto _LL2D6;_LL2D5: _tmp49B=_tmp499;goto _LL2D7;_LL2D6: if(*((int*)_tmp493)!= 1)goto
_LL2D8;_tmp49A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp493)->f2;if(_tmp49A <= (
void*)1)goto _LL2D8;if(*((int*)_tmp49A)!= 2)goto _LL2D8;_tmp49B=((struct Cyc_Absyn_Param_b_struct*)
_tmp49A)->f1;if(!(!_tmp49B->escapes))goto _LL2D8;_LL2D7: {void*_tmp49C=_tmp406->r;
void*_tmp49D;struct Cyc_List_List*_tmp49E;struct Cyc_List_List _tmp49F;struct Cyc_Absyn_Exp*
_tmp4A0;_LL2DB: if(*((int*)_tmp49C)!= 3)goto _LL2DD;_tmp49D=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp49C)->f1;_tmp49E=((struct Cyc_Absyn_Primop_e_struct*)_tmp49C)->f2;if(_tmp49E
== 0)goto _LL2DD;_tmp49F=*_tmp49E;_tmp4A0=(struct Cyc_Absyn_Exp*)_tmp49F.hd;_LL2DC: {
void*_tmp4A1=_tmp4A0->r;void*_tmp4A2;struct Cyc_Absyn_Vardecl*_tmp4A3;void*
_tmp4A4;struct Cyc_Absyn_Vardecl*_tmp4A5;void*_tmp4A6;struct Cyc_Absyn_Vardecl*
_tmp4A7;void*_tmp4A8;struct Cyc_Absyn_Vardecl*_tmp4A9;_LL2E0: if(*((int*)_tmp4A1)
!= 1)goto _LL2E2;_tmp4A2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp4A1)->f2;if(
_tmp4A2 <= (void*)1)goto _LL2E2;if(*((int*)_tmp4A2)!= 0)goto _LL2E2;_tmp4A3=((
struct Cyc_Absyn_Global_b_struct*)_tmp4A2)->f1;if(!(!_tmp4A3->escapes))goto _LL2E2;
_LL2E1: _tmp4A5=_tmp4A3;goto _LL2E3;_LL2E2: if(*((int*)_tmp4A1)!= 1)goto _LL2E4;
_tmp4A4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp4A1)->f2;if(_tmp4A4 <= (void*)
1)goto _LL2E4;if(*((int*)_tmp4A4)!= 3)goto _LL2E4;_tmp4A5=((struct Cyc_Absyn_Local_b_struct*)
_tmp4A4)->f1;if(!(!_tmp4A5->escapes))goto _LL2E4;_LL2E3: _tmp4A7=_tmp4A5;goto
_LL2E5;_LL2E4: if(*((int*)_tmp4A1)!= 1)goto _LL2E6;_tmp4A6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp4A1)->f2;if(_tmp4A6 <= (void*)1)goto _LL2E6;if(*((int*)_tmp4A6)!= 4)goto _LL2E6;
_tmp4A7=((struct Cyc_Absyn_Pat_b_struct*)_tmp4A6)->f1;if(!(!_tmp4A7->escapes))
goto _LL2E6;_LL2E5: _tmp4A9=_tmp4A7;goto _LL2E7;_LL2E6: if(*((int*)_tmp4A1)!= 1)goto
_LL2E8;_tmp4A8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp4A1)->f2;if(_tmp4A8 <= (
void*)1)goto _LL2E8;if(*((int*)_tmp4A8)!= 2)goto _LL2E8;_tmp4A9=((struct Cyc_Absyn_Param_b_struct*)
_tmp4A8)->f1;if(!(!_tmp4A9->escapes))goto _LL2E8;_LL2E7: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp4AA=env->fenv;struct Cyc_List_List*_tmp4AB=({struct Cyc_List_List*_tmp4AD=
_region_malloc(_tmp4AA->r,sizeof(*_tmp4AD));_tmp4AD->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp4AE=_region_malloc(_tmp4AA->r,sizeof(*_tmp4AE));_tmp4AE->vd=_tmp49B;_tmp4AE->rop=
Cyc_CfFlowInfo_LessEqNumelts(_tmp4A9);_tmp4AE;});_tmp4AD->tl=_tmp402;_tmp4AD;});
return({struct _tuple16 _tmp4AC;_tmp4AC.f1=Cyc_CfFlowInfo_ReachableFL(_tmp401,
_tmp4AB,_tmp403);_tmp4AC.f2=_tmp48A;_tmp4AC;});}_LL2E8:;_LL2E9: return({struct
_tuple16 _tmp4AF;_tmp4AF.f1=_tmp489;_tmp4AF.f2=_tmp48A;_tmp4AF;});_LL2DF:;}_LL2DD:;
_LL2DE: return({struct _tuple16 _tmp4B0;_tmp4B0.f1=_tmp489;_tmp4B0.f2=_tmp48A;
_tmp4B0;});_LL2DA:;}_LL2D8:;_LL2D9: return({struct _tuple16 _tmp4B1;_tmp4B1.f1=
_tmp489;_tmp4B1.f2=_tmp48A;_tmp4B1;});_LL2CF:;}}_LL29E:;_LL29F: return({struct
_tuple16 _tmp4B2;_tmp4B2.f1=f;_tmp4B2.f2=f;_tmp4B2;});_LL291:;}}_LL25E:;}}_LL25C:;
_LL25D: goto _LL24F;_LL24F:;}{union Cyc_CfFlowInfo_FlowInfo _tmp4B4;void*_tmp4B5;
struct _tuple13 _tmp4B3=Cyc_NewControlFlow_anal_Rexp(env,inflow,e);_tmp4B4=_tmp4B3.f1;
_tmp4B5=_tmp4B3.f2;_tmp4B4=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp4B4);{union Cyc_CfFlowInfo_FlowInfo _tmp4B6=_tmp4B4;int _tmp4B7;struct _tuple12
_tmp4B8;struct Cyc_Dict_Dict _tmp4B9;_LL2EB: if((_tmp4B6.BottomFL).tag != 1)goto
_LL2ED;_tmp4B7=(int)(_tmp4B6.BottomFL).val;_LL2EC: return({struct _tuple16 _tmp4BA;
_tmp4BA.f1=_tmp4B4;_tmp4BA.f2=_tmp4B4;_tmp4BA;});_LL2ED: if((_tmp4B6.ReachableFL).tag
!= 2)goto _LL2EA;_tmp4B8=(struct _tuple12)(_tmp4B6.ReachableFL).val;_tmp4B9=
_tmp4B8.f1;_LL2EE: {void*_tmp4BB=_tmp4B5;void*_tmp4BC;void*_tmp4BD;void*_tmp4BE;
_LL2F0: if((int)_tmp4BB != 0)goto _LL2F2;_LL2F1: return({struct _tuple16 _tmp4BF;
_tmp4BF.f1=Cyc_CfFlowInfo_BottomFL();_tmp4BF.f2=_tmp4B4;_tmp4BF;});_LL2F2: if((
int)_tmp4BB != 2)goto _LL2F4;_LL2F3: goto _LL2F5;_LL2F4: if((int)_tmp4BB != 1)goto
_LL2F6;_LL2F5: goto _LL2F7;_LL2F6: if(_tmp4BB <= (void*)3)goto _LL2F8;if(*((int*)
_tmp4BB)!= 2)goto _LL2F8;_LL2F7: return({struct _tuple16 _tmp4C0;_tmp4C0.f1=_tmp4B4;
_tmp4C0.f2=Cyc_CfFlowInfo_BottomFL();_tmp4C0;});_LL2F8: if(_tmp4BB <= (void*)3)
goto _LL2FA;if(*((int*)_tmp4BB)!= 0)goto _LL2FA;_tmp4BC=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4BB)->f1;if((int)_tmp4BC != 0)goto _LL2FA;_LL2F9: goto _LL2FB;_LL2FA: if(_tmp4BB
<= (void*)3)goto _LL2FC;if(*((int*)_tmp4BB)!= 1)goto _LL2FC;_tmp4BD=(void*)((
struct Cyc_CfFlowInfo_Esc_struct*)_tmp4BB)->f1;if((int)_tmp4BD != 0)goto _LL2FC;
_LL2FB:({void*_tmp4C1=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp4C2="expression may not be initialized";
_tag_dyneither(_tmp4C2,sizeof(char),34);}),_tag_dyneither(_tmp4C1,sizeof(void*),
0));});return({struct _tuple16 _tmp4C3;_tmp4C3.f1=Cyc_CfFlowInfo_BottomFL();
_tmp4C3.f2=Cyc_CfFlowInfo_BottomFL();_tmp4C3;});_LL2FC: if(_tmp4BB <= (void*)3)
goto _LL2FE;if(*((int*)_tmp4BB)!= 0)goto _LL2FE;_tmp4BE=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4BB)->f1;_LL2FD: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4B4,e,
_tmp4BE);_LL2FE: if(_tmp4BB <= (void*)3)goto _LL300;if(*((int*)_tmp4BB)!= 1)goto
_LL300;_LL2FF: return({struct _tuple16 _tmp4C4;_tmp4C4.f1=_tmp4B4;_tmp4C4.f2=
_tmp4B4;_tmp4C4;});_LL300: if(_tmp4BB <= (void*)3)goto _LL302;if(*((int*)_tmp4BB)!= 
3)goto _LL302;_LL301: return({struct _tuple16 _tmp4C5;_tmp4C5.f1=_tmp4B4;_tmp4C5.f2=
_tmp4B4;_tmp4C5;});_LL302: if(_tmp4BB <= (void*)3)goto _LL2EF;if(*((int*)_tmp4BB)!= 
4)goto _LL2EF;_LL303:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp4C6=_cycalloc(sizeof(*_tmp4C6));_tmp4C6[0]=({struct Cyc_Core_Impossible_struct
_tmp4C7;_tmp4C7.tag=Cyc_Core_Impossible;_tmp4C7.f1=({const char*_tmp4C8="anal_test";
_tag_dyneither(_tmp4C8,sizeof(char),10);});_tmp4C7;});_tmp4C6;}));_LL2EF:;}
_LL2EA:;}}}static void Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*
loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){union
Cyc_CfFlowInfo_FlowInfo _tmp4C9=flow;int _tmp4CA;struct _tuple12 _tmp4CB;struct Cyc_Dict_Dict
_tmp4CC;_LL305: if((_tmp4C9.BottomFL).tag != 1)goto _LL307;_tmp4CA=(int)(_tmp4C9.BottomFL).val;
_LL306: return;_LL307: if((_tmp4C9.ReachableFL).tag != 2)goto _LL304;_tmp4CB=(struct
_tuple12)(_tmp4C9.ReachableFL).val;_tmp4CC=_tmp4CB.f1;_LL308:{struct Cyc_List_List*
_tmp4CD=env->param_roots;for(0;_tmp4CD != 0;_tmp4CD=_tmp4CD->tl){if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp4CC,Cyc_CfFlowInfo_lookup_place(_tmp4CC,(struct Cyc_CfFlowInfo_Place*)
_tmp4CD->hd))!= (void*)2)({void*_tmp4CE=0;Cyc_Tcutil_terr(loc,({const char*
_tmp4CF="function may not initialize all the parameters with attribute 'initializes'";
_tag_dyneither(_tmp4CF,sizeof(char),76);}),_tag_dyneither(_tmp4CE,sizeof(void*),
0));});}}return;_LL304:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp4D0=env->fenv;for(0;scs != 0;
scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp4D2;struct Cyc_Core_Opt*_tmp4D3;
struct Cyc_Absyn_Exp*_tmp4D4;struct Cyc_Absyn_Stmt*_tmp4D5;struct Cyc_Position_Segment*
_tmp4D6;struct Cyc_Absyn_Switch_clause*_tmp4D1=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp4D2=*_tmp4D1;_tmp4D3=_tmp4D2.pat_vars;_tmp4D4=_tmp4D2.where_clause;
_tmp4D5=_tmp4D2.body;_tmp4D6=_tmp4D2.loc;{union Cyc_CfFlowInfo_FlowInfo
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp4D0,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp4D3))->v,_tmp4D0->unknown_all,_tmp4D6);union
Cyc_CfFlowInfo_FlowInfo clause_outflow;if(_tmp4D4 != 0){struct Cyc_Absyn_Exp*wexp=(
struct Cyc_Absyn_Exp*)_tmp4D4;union Cyc_CfFlowInfo_FlowInfo _tmp4D8;union Cyc_CfFlowInfo_FlowInfo
_tmp4D9;struct _tuple16 _tmp4D7=Cyc_NewControlFlow_anal_test(env,clause_inflow,
wexp);_tmp4D8=_tmp4D7.f1;_tmp4D9=_tmp4D7.f2;_tmp4D8=Cyc_CfFlowInfo_readthrough_unique_rvals(
wexp->loc,_tmp4D8);_tmp4D9=Cyc_CfFlowInfo_readthrough_unique_rvals(wexp->loc,
_tmp4D9);inflow=_tmp4D9;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp4D8,
_tmp4D5);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,
_tmp4D5);}{union Cyc_CfFlowInfo_FlowInfo _tmp4DA=clause_outflow;int _tmp4DB;_LL30A:
if((_tmp4DA.BottomFL).tag != 1)goto _LL30C;_tmp4DB=(int)(_tmp4DA.BottomFL).val;
_LL30B: goto _LL309;_LL30C:;_LL30D: if(scs->tl == 0)return clause_outflow;else{if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0)({void*
_tmp4DC=0;Cyc_Tcutil_terr(_tmp4D5->loc,({const char*_tmp4DD="switch clause may implicitly fallthru";
_tag_dyneither(_tmp4DD,sizeof(char),38);}),_tag_dyneither(_tmp4DC,sizeof(void*),
0));});else{({void*_tmp4DE=0;Cyc_Tcutil_warn(_tmp4D5->loc,({const char*_tmp4DF="switch clause may implicitly fallthru";
_tag_dyneither(_tmp4DF,sizeof(char),38);}),_tag_dyneither(_tmp4DE,sizeof(void*),
0));});}Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}goto
_LL309;_LL309:;}}}return Cyc_CfFlowInfo_BottomFL();}static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp4E1;union Cyc_CfFlowInfo_FlowInfo*_tmp4E2;struct _tuple17 _tmp4E0=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp4E1=_tmp4E0.f1;_tmp4E2=_tmp4E0.f2;inflow=*_tmp4E2;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp4E3=env->fenv;{void*_tmp4E4=s->r;struct Cyc_Absyn_Exp*_tmp4E5;struct Cyc_Absyn_Exp*
_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E7;struct Cyc_Absyn_Stmt*_tmp4E8;struct Cyc_Absyn_Stmt*
_tmp4E9;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_Absyn_Stmt*_tmp4EB;struct Cyc_Absyn_Stmt*
_tmp4EC;struct _tuple9 _tmp4ED;struct Cyc_Absyn_Exp*_tmp4EE;struct Cyc_Absyn_Stmt*
_tmp4EF;struct Cyc_Absyn_Stmt*_tmp4F0;struct Cyc_Absyn_Stmt*_tmp4F1;struct _tuple9
_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F3;struct Cyc_Absyn_Stmt*_tmp4F4;struct Cyc_Absyn_Exp*
_tmp4F5;struct _tuple9 _tmp4F6;struct Cyc_Absyn_Exp*_tmp4F7;struct Cyc_Absyn_Stmt*
_tmp4F8;struct _tuple9 _tmp4F9;struct Cyc_Absyn_Exp*_tmp4FA;struct Cyc_Absyn_Stmt*
_tmp4FB;struct Cyc_Absyn_Stmt*_tmp4FC;struct Cyc_Absyn_Stmt*_tmp4FD;struct Cyc_List_List*
_tmp4FE;struct Cyc_Absyn_Switch_clause**_tmp4FF;struct Cyc_Absyn_Switch_clause*
_tmp500;struct Cyc_Absyn_Stmt*_tmp501;struct Cyc_Absyn_Stmt*_tmp502;struct Cyc_Absyn_Stmt*
_tmp503;struct Cyc_Absyn_Exp*_tmp504;struct Cyc_List_List*_tmp505;struct Cyc_Absyn_Stmt*
_tmp506;struct Cyc_List_List*_tmp507;struct Cyc_Absyn_Decl*_tmp508;struct Cyc_Absyn_Decl
_tmp509;void*_tmp50A;struct Cyc_Absyn_Exp*_tmp50B;struct Cyc_Absyn_Tvar*_tmp50C;
struct Cyc_Absyn_Vardecl*_tmp50D;struct Cyc_Absyn_Stmt*_tmp50E;struct Cyc_Absyn_Decl*
_tmp50F;struct Cyc_Absyn_Stmt*_tmp510;struct Cyc_Absyn_Stmt*_tmp511;struct Cyc_Absyn_Exp*
_tmp512;_LL30F: if((int)_tmp4E4 != 0)goto _LL311;_LL310: return inflow;_LL311: if(
_tmp4E4 <= (void*)1)goto _LL337;if(*((int*)_tmp4E4)!= 2)goto _LL313;_tmp4E5=((
struct Cyc_Absyn_Return_s_struct*)_tmp4E4)->f1;if(_tmp4E5 != 0)goto _LL313;_LL312:
if(env->noreturn)({void*_tmp513=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp514="`noreturn' function might return";
_tag_dyneither(_tmp514,sizeof(char),33);}),_tag_dyneither(_tmp513,sizeof(void*),
0));});Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return Cyc_CfFlowInfo_BottomFL();
_LL313: if(*((int*)_tmp4E4)!= 2)goto _LL315;_tmp4E6=((struct Cyc_Absyn_Return_s_struct*)
_tmp4E4)->f1;_LL314: if(env->noreturn)({void*_tmp515=0;Cyc_Tcutil_terr(s->loc,({
const char*_tmp516="`noreturn' function might return";_tag_dyneither(_tmp516,
sizeof(char),33);}),_tag_dyneither(_tmp515,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo
_tmp518;void*_tmp519;struct _tuple13 _tmp517=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp4E6));_tmp518=_tmp517.f1;_tmp519=
_tmp517.f2;_tmp518=Cyc_CfFlowInfo_consume_unique_rvals(_tmp4E6->loc,_tmp518);
_tmp518=Cyc_NewControlFlow_use_Rval(env,_tmp4E6->loc,_tmp518,_tmp519);Cyc_NewControlFlow_check_init_params(
s->loc,env,_tmp518);return Cyc_CfFlowInfo_BottomFL();}_LL315: if(*((int*)_tmp4E4)
!= 0)goto _LL317;_tmp4E7=((struct Cyc_Absyn_Exp_s_struct*)_tmp4E4)->f1;_LL316:
outflow=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp4E7)).f1;goto _LL30E;_LL317:
if(*((int*)_tmp4E4)!= 1)goto _LL319;_tmp4E8=((struct Cyc_Absyn_Seq_s_struct*)
_tmp4E4)->f1;_tmp4E9=((struct Cyc_Absyn_Seq_s_struct*)_tmp4E4)->f2;_LL318: outflow=
Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4E8),
_tmp4E9);goto _LL30E;_LL319: if(*((int*)_tmp4E4)!= 3)goto _LL31B;_tmp4EA=((struct
Cyc_Absyn_IfThenElse_s_struct*)_tmp4E4)->f1;_tmp4EB=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp4E4)->f2;_tmp4EC=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4E4)->f3;_LL31A: {
union Cyc_CfFlowInfo_FlowInfo _tmp51B;union Cyc_CfFlowInfo_FlowInfo _tmp51C;struct
_tuple16 _tmp51A=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4EA);_tmp51B=_tmp51A.f1;
_tmp51C=_tmp51A.f2;_tmp51B=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4EA->loc,
_tmp51B);_tmp51C=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4EA->loc,_tmp51C);
outflow=Cyc_CfFlowInfo_join_flow(_tmp4E3,env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp51B,_tmp4EB),Cyc_NewControlFlow_anal_stmt(env,_tmp51C,_tmp4EC),1);goto
_LL30E;}_LL31B: if(*((int*)_tmp4E4)!= 4)goto _LL31D;_tmp4ED=((struct Cyc_Absyn_While_s_struct*)
_tmp4E4)->f1;_tmp4EE=_tmp4ED.f1;_tmp4EF=_tmp4ED.f2;_tmp4F0=((struct Cyc_Absyn_While_s_struct*)
_tmp4E4)->f2;_LL31C: {union Cyc_CfFlowInfo_FlowInfo*_tmp51E;struct _tuple17 _tmp51D=
Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp4EF);_tmp51E=_tmp51D.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp51F=*_tmp51E;union Cyc_CfFlowInfo_FlowInfo _tmp521;
union Cyc_CfFlowInfo_FlowInfo _tmp522;struct _tuple16 _tmp520=Cyc_NewControlFlow_anal_test(
env,_tmp51F,_tmp4EE);_tmp521=_tmp520.f1;_tmp522=_tmp520.f2;_tmp521=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4EE->loc,_tmp521);_tmp522=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4EE->loc,
_tmp522);{union Cyc_CfFlowInfo_FlowInfo _tmp523=Cyc_NewControlFlow_anal_stmt(env,
_tmp521,_tmp4F0);Cyc_NewControlFlow_update_flow(env,_tmp4EF,_tmp523);outflow=
_tmp522;goto _LL30E;}}}_LL31D: if(*((int*)_tmp4E4)!= 13)goto _LL31F;_tmp4F1=((
struct Cyc_Absyn_Do_s_struct*)_tmp4E4)->f1;_tmp4F2=((struct Cyc_Absyn_Do_s_struct*)
_tmp4E4)->f2;_tmp4F3=_tmp4F2.f1;_tmp4F4=_tmp4F2.f2;_LL31E: {union Cyc_CfFlowInfo_FlowInfo
_tmp524=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4F1);union Cyc_CfFlowInfo_FlowInfo*
_tmp526;struct _tuple17 _tmp525=Cyc_NewControlFlow_pre_stmt_check(env,_tmp524,
_tmp4F4);_tmp526=_tmp525.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp527=*_tmp526;union
Cyc_CfFlowInfo_FlowInfo _tmp529;union Cyc_CfFlowInfo_FlowInfo _tmp52A;struct
_tuple16 _tmp528=Cyc_NewControlFlow_anal_test(env,_tmp527,_tmp4F3);_tmp529=
_tmp528.f1;_tmp52A=_tmp528.f2;Cyc_NewControlFlow_update_flow(env,_tmp4F1,_tmp529);
outflow=_tmp52A;goto _LL30E;}}_LL31F: if(*((int*)_tmp4E4)!= 8)goto _LL321;_tmp4F5=((
struct Cyc_Absyn_For_s_struct*)_tmp4E4)->f1;_tmp4F6=((struct Cyc_Absyn_For_s_struct*)
_tmp4E4)->f2;_tmp4F7=_tmp4F6.f1;_tmp4F8=_tmp4F6.f2;_tmp4F9=((struct Cyc_Absyn_For_s_struct*)
_tmp4E4)->f3;_tmp4FA=_tmp4F9.f1;_tmp4FB=_tmp4F9.f2;_tmp4FC=((struct Cyc_Absyn_For_s_struct*)
_tmp4E4)->f4;_LL320: {union Cyc_CfFlowInfo_FlowInfo _tmp52B=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4F5)).f1;_tmp52B=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4F5->loc,
_tmp52B);{union Cyc_CfFlowInfo_FlowInfo*_tmp52D;struct _tuple17 _tmp52C=Cyc_NewControlFlow_pre_stmt_check(
env,_tmp52B,_tmp4F8);_tmp52D=_tmp52C.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp52E=*
_tmp52D;union Cyc_CfFlowInfo_FlowInfo _tmp530;union Cyc_CfFlowInfo_FlowInfo _tmp531;
struct _tuple16 _tmp52F=Cyc_NewControlFlow_anal_test(env,_tmp52E,_tmp4F7);_tmp530=
_tmp52F.f1;_tmp531=_tmp52F.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp532=Cyc_NewControlFlow_anal_stmt(
env,_tmp530,_tmp4FC);union Cyc_CfFlowInfo_FlowInfo*_tmp534;struct _tuple17 _tmp533=
Cyc_NewControlFlow_pre_stmt_check(env,_tmp532,_tmp4FB);_tmp534=_tmp533.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp535=*_tmp534;union Cyc_CfFlowInfo_FlowInfo _tmp536=(Cyc_NewControlFlow_anal_Rexp(
env,_tmp535,_tmp4FA)).f1;_tmp536=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4FA->loc,
_tmp536);Cyc_NewControlFlow_update_flow(env,_tmp4F8,_tmp536);outflow=_tmp531;
goto _LL30E;}}}}}_LL321: if(*((int*)_tmp4E4)!= 5)goto _LL323;_tmp4FD=((struct Cyc_Absyn_Break_s_struct*)
_tmp4E4)->f1;if(_tmp4FD != 0)goto _LL323;_LL322: return Cyc_CfFlowInfo_BottomFL();
_LL323: if(*((int*)_tmp4E4)!= 10)goto _LL325;_tmp4FE=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp4E4)->f1;_tmp4FF=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp4E4)->f2;if(
_tmp4FF == 0)goto _LL325;_tmp500=*_tmp4FF;_LL324: {struct _RegionHandle*_tmp537=env->r;
union Cyc_CfFlowInfo_FlowInfo _tmp539;struct Cyc_List_List*_tmp53A;struct _tuple19
_tmp538=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp537,env,inflow,_tmp4FE,0);
_tmp539=_tmp538.f1;_tmp53A=_tmp538.f2;for(0;_tmp53A != 0;(_tmp53A=_tmp53A->tl,
_tmp4FE=_tmp4FE->tl)){_tmp539=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4FE))->hd)->loc,_tmp539,(void*)_tmp53A->hd);}
_tmp539=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,_tmp539);_tmp539=Cyc_NewControlFlow_add_vars(
_tmp4E3,_tmp539,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp500->pat_vars))->v,
_tmp4E3->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp500->body,_tmp539);return Cyc_CfFlowInfo_BottomFL();}_LL325: if(*((int*)
_tmp4E4)!= 5)goto _LL327;_tmp501=((struct Cyc_Absyn_Break_s_struct*)_tmp4E4)->f1;
_LL326: _tmp502=_tmp501;goto _LL328;_LL327: if(*((int*)_tmp4E4)!= 6)goto _LL329;
_tmp502=((struct Cyc_Absyn_Continue_s_struct*)_tmp4E4)->f1;_LL328: _tmp503=_tmp502;
goto _LL32A;_LL329: if(*((int*)_tmp4E4)!= 7)goto _LL32B;_tmp503=((struct Cyc_Absyn_Goto_s_struct*)
_tmp4E4)->f2;_LL32A: if(env->iteration_num == 1){struct Cyc_Absyn_Stmt*_tmp53B=
_tmp4E1->encloser;struct Cyc_Absyn_Stmt*_tmp53C=(Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)_check_null(_tmp503)))->encloser;while(_tmp53C != _tmp53B){
struct Cyc_Absyn_Stmt*_tmp53D=(Cyc_NewControlFlow_get_stmt_annot(_tmp53B))->encloser;
if(_tmp53D == _tmp53B){({void*_tmp53E=0;Cyc_Tcutil_terr(s->loc,({const char*
_tmp53F="goto enters local scope or exception handler";_tag_dyneither(_tmp53F,
sizeof(char),45);}),_tag_dyneither(_tmp53E,sizeof(void*),0));});break;}_tmp53B=
_tmp53D;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_check_null(
_tmp503),inflow);return Cyc_CfFlowInfo_BottomFL();_LL32B: if(*((int*)_tmp4E4)!= 9)
goto _LL32D;_tmp504=((struct Cyc_Absyn_Switch_s_struct*)_tmp4E4)->f1;_tmp505=((
struct Cyc_Absyn_Switch_s_struct*)_tmp4E4)->f2;_LL32C: {union Cyc_CfFlowInfo_FlowInfo
_tmp541;void*_tmp542;struct _tuple13 _tmp540=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp504);_tmp541=_tmp540.f1;_tmp542=_tmp540.f2;_tmp541=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp504->loc,_tmp541);_tmp541=Cyc_NewControlFlow_use_Rval(env,_tmp504->loc,
_tmp541,_tmp542);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp541,_tmp505);goto
_LL30E;}_LL32D: if(*((int*)_tmp4E4)!= 14)goto _LL32F;_tmp506=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp4E4)->f1;_tmp507=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4E4)->f2;_LL32E: {
int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp506);union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;env->in_try=
old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp507);{union Cyc_CfFlowInfo_FlowInfo _tmp543=scs_outflow;int
_tmp544;_LL33A: if((_tmp543.BottomFL).tag != 1)goto _LL33C;_tmp544=(int)(_tmp543.BottomFL).val;
_LL33B: goto _LL339;_LL33C:;_LL33D:({void*_tmp545=0;Cyc_Tcutil_terr(s->loc,({const
char*_tmp546="last catch clause may implicitly fallthru";_tag_dyneither(_tmp546,
sizeof(char),42);}),_tag_dyneither(_tmp545,sizeof(void*),0));});_LL339:;}outflow=
s1_outflow;goto _LL30E;}}}_LL32F: if(*((int*)_tmp4E4)!= 11)goto _LL331;_tmp508=((
struct Cyc_Absyn_Decl_s_struct*)_tmp4E4)->f1;_tmp509=*_tmp508;_tmp50A=_tmp509.r;
if(_tmp50A <= (void*)2)goto _LL331;if(*((int*)_tmp50A)!= 5)goto _LL331;_tmp50B=((
struct Cyc_Absyn_Alias_d_struct*)_tmp50A)->f1;_tmp50C=((struct Cyc_Absyn_Alias_d_struct*)
_tmp50A)->f2;_tmp50D=((struct Cyc_Absyn_Alias_d_struct*)_tmp50A)->f3;_tmp50E=((
struct Cyc_Absyn_Decl_s_struct*)_tmp4E4)->f2;_LL330: {union Cyc_CfFlowInfo_FlowInfo
_tmp548;void*_tmp549;struct _tuple13 _tmp547=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp50B);_tmp548=_tmp547.f1;_tmp549=_tmp547.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
_tmp54B;struct Cyc_List_List*_tmp54C;struct _tuple14 _tmp54A=Cyc_CfFlowInfo_save_consume_info(
_tmp4E3,_tmp548,0);_tmp54B=_tmp54A.f1;_tmp54C=_tmp54B.may_consume;_tmp548=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp50B->loc,_tmp548);_tmp548=Cyc_NewControlFlow_use_Rval(env,_tmp50B->loc,
_tmp548,_tmp549);{struct Cyc_List_List _tmp54D=({struct Cyc_List_List _tmp555;
_tmp555.hd=_tmp50D;_tmp555.tl=0;_tmp555;});_tmp548=Cyc_NewControlFlow_add_vars(
_tmp4E3,_tmp548,(struct Cyc_List_List*)& _tmp54D,_tmp4E3->unknown_all,s->loc);
outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp548,_tmp50E);{union Cyc_CfFlowInfo_FlowInfo
_tmp54E=outflow;int _tmp54F;struct _tuple12 _tmp550;struct Cyc_Dict_Dict _tmp551;
struct Cyc_List_List*_tmp552;struct Cyc_CfFlowInfo_ConsumeInfo _tmp553;_LL33F: if((
_tmp54E.BottomFL).tag != 1)goto _LL341;_tmp54F=(int)(_tmp54E.BottomFL).val;_LL340:
goto _LL33E;_LL341: if((_tmp54E.ReachableFL).tag != 2)goto _LL33E;_tmp550=(struct
_tuple12)(_tmp54E.ReachableFL).val;_tmp551=_tmp550.f1;_tmp552=_tmp550.f2;_tmp553=
_tmp550.f3;_LL342: while(_tmp54C != 0){struct Cyc_Dict_Dict _tmp554=_tmp553.consumed;
_tmp553.consumed=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,
struct Cyc_CfFlowInfo_Place*))Cyc_Dict_rdelete)(_tmp4E3->r,_tmp553.consumed,(
struct Cyc_CfFlowInfo_Place*)_tmp54C->hd);if((_tmp553.consumed).t != _tmp554.t);
_tmp54C=_tmp54C->tl;}outflow=Cyc_CfFlowInfo_ReachableFL(_tmp551,_tmp552,_tmp553);
goto _LL33E;_LL33E:;}goto _LL30E;}}}_LL331: if(*((int*)_tmp4E4)!= 11)goto _LL333;
_tmp50F=((struct Cyc_Absyn_Decl_s_struct*)_tmp4E4)->f1;_tmp510=((struct Cyc_Absyn_Decl_s_struct*)
_tmp4E4)->f2;_LL332: outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp50F),_tmp510);goto _LL30E;_LL333: if(*((int*)_tmp4E4)!= 12)goto
_LL335;_tmp511=((struct Cyc_Absyn_Label_s_struct*)_tmp4E4)->f2;_LL334: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp511);goto _LL30E;_LL335: if(*((int*)_tmp4E4)!= 15)goto _LL337;
_tmp512=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp4E4)->f1;_LL336: {union Cyc_CfFlowInfo_FlowInfo
_tmp557;void*_tmp558;struct _tuple13 _tmp556=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp512);_tmp557=_tmp556.f1;_tmp558=_tmp556.f2;_tmp557=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp512->loc,_tmp557);{union Cyc_CfFlowInfo_FlowInfo _tmp559=Cyc_NewControlFlow_use_Rval(
env,_tmp512->loc,_tmp557,_tmp558);{void*_tmp55A=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp512->topt))->v);void*_tmp55B;_LL344: if(
_tmp55A <= (void*)4)goto _LL346;if(*((int*)_tmp55A)!= 14)goto _LL346;_tmp55B=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp55A)->f1;_LL345: outflow=Cyc_CfFlowInfo_kill_flow_region(
_tmp4E3,_tmp557,_tmp55B);goto _LL343;_LL346:;_LL347:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C[0]=({
struct Cyc_Core_Impossible_struct _tmp55D;_tmp55D.tag=Cyc_Core_Impossible;_tmp55D.f1=({
const char*_tmp55E="anal_stmt -- reset_region";_tag_dyneither(_tmp55E,sizeof(char),
26);});_tmp55D;});_tmp55C;}));_LL343:;}goto _LL30E;}}_LL337:;_LL338:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp55F=_cycalloc(sizeof(*_tmp55F));
_tmp55F[0]=({struct Cyc_Core_Impossible_struct _tmp560;_tmp560.tag=Cyc_Core_Impossible;
_tmp560.f1=({const char*_tmp561="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_dyneither(_tmp561,sizeof(char),56);});_tmp560;});_tmp55F;}));_LL30E:;}
outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo
_tmp562=outflow;struct _tuple12 _tmp563;struct Cyc_CfFlowInfo_ConsumeInfo _tmp564;
_LL349: if((_tmp562.ReachableFL).tag != 2)goto _LL34B;_tmp563=(struct _tuple12)(
_tmp562.ReachableFL).val;_tmp564=_tmp563.f3;_LL34A: goto _LL348;_LL34B:;_LL34C:
goto _LL348;_LL348:;}return outflow;}}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){void*_tmp565=
decl->r;struct Cyc_Absyn_Vardecl*_tmp566;struct Cyc_Core_Opt*_tmp567;struct Cyc_Core_Opt
_tmp568;struct Cyc_List_List*_tmp569;struct Cyc_Absyn_Exp*_tmp56A;struct Cyc_List_List*
_tmp56B;struct Cyc_Absyn_Fndecl*_tmp56C;struct Cyc_Absyn_Tvar*_tmp56D;struct Cyc_Absyn_Vardecl*
_tmp56E;int _tmp56F;struct Cyc_Absyn_Exp*_tmp570;_LL34E: if(_tmp565 <= (void*)2)goto
_LL358;if(*((int*)_tmp565)!= 0)goto _LL350;_tmp566=((struct Cyc_Absyn_Var_d_struct*)
_tmp565)->f1;_LL34F: {struct Cyc_List_List _tmp571=({struct Cyc_List_List _tmp580;
_tmp580.hd=_tmp566;_tmp580.tl=0;_tmp580;});inflow=Cyc_NewControlFlow_add_vars(
env->fenv,inflow,(struct Cyc_List_List*)& _tmp571,(env->fenv)->unknown_none,decl->loc);{
struct Cyc_Absyn_Exp*_tmp572=_tmp566->initializer;if(_tmp572 == 0)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp574;void*_tmp575;struct _tuple13 _tmp573=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)_tmp572);_tmp574=_tmp573.f1;_tmp575=_tmp573.f2;
_tmp574=Cyc_CfFlowInfo_consume_unique_rvals(_tmp572->loc,_tmp574);{union Cyc_CfFlowInfo_FlowInfo
_tmp576=_tmp574;int _tmp577;struct _tuple12 _tmp578;struct Cyc_Dict_Dict _tmp579;
struct Cyc_List_List*_tmp57A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp57B;_LL35B: if((
_tmp576.BottomFL).tag != 1)goto _LL35D;_tmp577=(int)(_tmp576.BottomFL).val;_LL35C:
return Cyc_CfFlowInfo_BottomFL();_LL35D: if((_tmp576.ReachableFL).tag != 2)goto
_LL35A;_tmp578=(struct _tuple12)(_tmp576.ReachableFL).val;_tmp579=_tmp578.f1;
_tmp57A=_tmp578.f2;_tmp57B=_tmp578.f3;_LL35E: _tmp579=Cyc_CfFlowInfo_assign_place(
env->fenv,decl->loc,_tmp579,env->all_changed,({struct Cyc_CfFlowInfo_Place*
_tmp57C=_region_malloc(env->r,sizeof(*_tmp57C));_tmp57C->root=(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp57D=_region_malloc(env->r,sizeof(*_tmp57D));_tmp57D[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp57E;_tmp57E.tag=0;_tmp57E.f1=_tmp566;_tmp57E;});_tmp57D;});_tmp57C->fields=0;
_tmp57C;}),_tmp575);_tmp57A=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp57A,
_tmp566,(struct Cyc_Absyn_Exp*)_check_null(_tmp566->initializer));{union Cyc_CfFlowInfo_FlowInfo
_tmp57F=Cyc_CfFlowInfo_ReachableFL(_tmp579,_tmp57A,_tmp57B);Cyc_NewControlFlow_update_tryflow(
env,_tmp57F);return _tmp57F;}_LL35A:;}}}}_LL350: if(*((int*)_tmp565)!= 2)goto
_LL352;_tmp567=((struct Cyc_Absyn_Let_d_struct*)_tmp565)->f2;if(_tmp567 == 0)goto
_LL352;_tmp568=*_tmp567;_tmp569=(struct Cyc_List_List*)_tmp568.v;_tmp56A=((struct
Cyc_Absyn_Let_d_struct*)_tmp565)->f3;_LL351: {union Cyc_CfFlowInfo_FlowInfo
_tmp582;void*_tmp583;struct _tuple13 _tmp581=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp56A);_tmp582=_tmp581.f1;_tmp583=_tmp581.f2;_tmp582=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp56A->loc,_tmp582);_tmp582=Cyc_NewControlFlow_use_Rval(env,_tmp56A->loc,
_tmp582,_tmp583);return Cyc_NewControlFlow_add_vars(env->fenv,_tmp582,_tmp569,(
env->fenv)->unknown_all,decl->loc);}_LL352: if(*((int*)_tmp565)!= 3)goto _LL354;
_tmp56B=((struct Cyc_Absyn_Letv_d_struct*)_tmp565)->f1;_LL353: return Cyc_NewControlFlow_add_vars(
env->fenv,inflow,_tmp56B,(env->fenv)->unknown_none,decl->loc);_LL354: if(*((int*)
_tmp565)!= 1)goto _LL356;_tmp56C=((struct Cyc_Absyn_Fn_d_struct*)_tmp565)->f1;
_LL355: Cyc_NewControlFlow_check_nested_fun(env->fenv,inflow,_tmp56C);{void*
_tmp584=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp56C->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp585=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp56C->fn_vardecl);
return Cyc_NewControlFlow_add_vars(env->fenv,inflow,({struct Cyc_List_List*_tmp586=
_region_malloc(env->r,sizeof(*_tmp586));_tmp586->hd=_tmp585;_tmp586->tl=0;
_tmp586;}),(env->fenv)->unknown_all,decl->loc);}_LL356: if(*((int*)_tmp565)!= 4)
goto _LL358;_tmp56D=((struct Cyc_Absyn_Region_d_struct*)_tmp565)->f1;_tmp56E=((
struct Cyc_Absyn_Region_d_struct*)_tmp565)->f2;_tmp56F=((struct Cyc_Absyn_Region_d_struct*)
_tmp565)->f3;_tmp570=((struct Cyc_Absyn_Region_d_struct*)_tmp565)->f4;_LL357: if(
_tmp570 != 0){struct Cyc_Absyn_Exp*_tmp587=(struct Cyc_Absyn_Exp*)_tmp570;union Cyc_CfFlowInfo_FlowInfo
_tmp589;void*_tmp58A;struct _tuple13 _tmp588=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp587);_tmp589=_tmp588.f1;_tmp58A=_tmp588.f2;_tmp589=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp587->loc,_tmp589);inflow=Cyc_NewControlFlow_use_Rval(env,_tmp587->loc,
_tmp589,_tmp58A);}{struct Cyc_List_List _tmp58B=({struct Cyc_List_List _tmp58C;
_tmp58C.hd=_tmp56E;_tmp58C.tl=0;_tmp58C;});return Cyc_NewControlFlow_add_vars(env->fenv,
inflow,(struct Cyc_List_List*)& _tmp58B,(env->fenv)->unknown_all,decl->loc);}
_LL358:;_LL359:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp58D=
_cycalloc(sizeof(*_tmp58D));_tmp58D[0]=({struct Cyc_Core_Impossible_struct _tmp58E;
_tmp58E.tag=Cyc_Core_Impossible;_tmp58E.f1=({const char*_tmp58F="anal_decl: unexpected decl variant";
_tag_dyneither(_tmp58F,sizeof(char),35);});_tmp58E;});_tmp58D;}));_LL34D:;}
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle _tmp590=_new_region("frgn");struct _RegionHandle*frgn=& _tmp590;
_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,
0,({struct Cyc_CfFlowInfo_ConsumeInfo _tmp591;_tmp591.consumed=fenv->mt_place_set;
_tmp591.may_consume=0;_tmp591;})),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(
void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp592=fenv->r;struct Cyc_Position_Segment*_tmp593=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp593);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo _tmp594=inflow;int _tmp595;struct
_tuple12 _tmp596;struct Cyc_Dict_Dict _tmp597;struct Cyc_List_List*_tmp598;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp599;_LL360: if((_tmp594.BottomFL).tag != 1)goto _LL362;_tmp595=(int)(_tmp594.BottomFL).val;
_LL361:({void*_tmp59A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp59B="check_fun";_tag_dyneither(_tmp59B,sizeof(
char),10);}),_tag_dyneither(_tmp59A,sizeof(void*),0));});_LL362: if((_tmp594.ReachableFL).tag
!= 2)goto _LL35F;_tmp596=(struct _tuple12)(_tmp594.ReachableFL).val;_tmp597=
_tmp596.f1;_tmp598=_tmp596.f2;_tmp599=_tmp596.f3;_LL363: {struct Cyc_List_List*
atts;{void*_tmp59C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp59D;struct Cyc_List_List*_tmp59E;
_LL365: if(_tmp59C <= (void*)4)goto _LL367;if(*((int*)_tmp59C)!= 8)goto _LL367;
_tmp59D=((struct Cyc_Absyn_FnType_struct*)_tmp59C)->f1;_tmp59E=_tmp59D.attributes;
_LL366: atts=_tmp59E;goto _LL364;_LL367:;_LL368:({void*_tmp59F=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp5A0="check_fun: non-function type cached with fndecl_t";
_tag_dyneither(_tmp5A0,sizeof(char),50);}),_tag_dyneither(_tmp59F,sizeof(void*),
0));});_LL364:;}for(0;atts != 0;atts=atts->tl){void*_tmp5A1=(void*)atts->hd;int
_tmp5A2;_LL36A: if(_tmp5A1 <= (void*)17)goto _LL36C;if(*((int*)_tmp5A1)!= 4)goto
_LL36C;_tmp5A2=((struct Cyc_Absyn_Initializes_att_struct*)_tmp5A1)->f1;_LL36B: {
unsigned int j=(unsigned int)_tmp5A2;if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp5A3=0;Cyc_Tcutil_terr(_tmp593,({const char*_tmp5A4="initializes attribute exceeds number of parameters";
_tag_dyneither(_tmp5A4,sizeof(char),51);}),_tag_dyneither(_tmp5A3,sizeof(void*),
0));});continue;}{struct Cyc_Absyn_Vardecl*_tmp5A5=((struct Cyc_Absyn_Vardecl*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp5A6=Cyc_Tcutil_compress(
_tmp5A5->type);struct Cyc_Absyn_PtrInfo _tmp5A7;void*_tmp5A8;struct Cyc_Absyn_PtrAtts
_tmp5A9;union Cyc_Absyn_Constraint*_tmp5AA;union Cyc_Absyn_Constraint*_tmp5AB;
union Cyc_Absyn_Constraint*_tmp5AC;_LL36F: if(_tmp5A6 <= (void*)4)goto _LL371;if(*((
int*)_tmp5A6)!= 4)goto _LL371;_tmp5A7=((struct Cyc_Absyn_PointerType_struct*)
_tmp5A6)->f1;_tmp5A8=_tmp5A7.elt_typ;_tmp5A9=_tmp5A7.ptr_atts;_tmp5AA=_tmp5A9.nullable;
_tmp5AB=_tmp5A9.bounds;_tmp5AC=_tmp5A9.zero_term;_LL370: if(((int(*)(union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_val)(_tmp5AA))({void*_tmp5AD=0;Cyc_Tcutil_terr(_tmp593,({
const char*_tmp5AE="initializes attribute not allowed on nullable pointers";
_tag_dyneither(_tmp5AE,sizeof(char),55);}),_tag_dyneither(_tmp5AD,sizeof(void*),
0));});if(!Cyc_Tcutil_is_bound_one(_tmp5AB))({void*_tmp5AF=0;Cyc_Tcutil_terr(
_tmp593,({const char*_tmp5B0="initializes attribute allowed only on pointers of size 1";
_tag_dyneither(_tmp5B0,sizeof(char),57);}),_tag_dyneither(_tmp5AF,sizeof(void*),
0));});if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp5AC))({void*_tmp5B1=0;Cyc_Tcutil_terr(_tmp593,({const char*_tmp5B2="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_dyneither(_tmp5B2,sizeof(char),77);}),_tag_dyneither(_tmp5B1,sizeof(void*),
0));});{struct Cyc_CfFlowInfo_InitParam_struct*_tmp5B3=({struct Cyc_CfFlowInfo_InitParam_struct*
_tmp5BB=_region_malloc(_tmp592,sizeof(*_tmp5BB));_tmp5BB[0]=({struct Cyc_CfFlowInfo_InitParam_struct
_tmp5BC;_tmp5BC.tag=2;_tmp5BC.f1=(int)j;_tmp5BC.f2=(void*)_tmp5A8;_tmp5BC;});
_tmp5BB;});struct Cyc_CfFlowInfo_Place*_tmp5B4=({struct Cyc_CfFlowInfo_Place*
_tmp5BA=_region_malloc(_tmp592,sizeof(*_tmp5BA));_tmp5BA->root=(void*)_tmp5B3;
_tmp5BA->fields=0;_tmp5BA;});_tmp597=Cyc_Dict_insert(_tmp597,(void*)_tmp5B3,Cyc_CfFlowInfo_typ_to_absrval(
fenv,_tmp5A8,fenv->esc_none));_tmp597=Cyc_Dict_insert(_tmp597,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp5B5=_region_malloc(_tmp592,sizeof(*_tmp5B5));_tmp5B5[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp5B6;_tmp5B6.tag=0;_tmp5B6.f1=_tmp5A5;_tmp5B6;});_tmp5B5;}),(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp5B7=_region_malloc(_tmp592,sizeof(*_tmp5B7));
_tmp5B7[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp5B8;_tmp5B8.tag=2;_tmp5B8.f1=
_tmp5B4;_tmp5B8;});_tmp5B7;}));param_roots=({struct Cyc_List_List*_tmp5B9=
_region_malloc(_tmp592,sizeof(*_tmp5B9));_tmp5B9->hd=_tmp5B4;_tmp5B9->tl=
param_roots;_tmp5B9;});goto _LL36E;}_LL371:;_LL372:({void*_tmp5BD=0;Cyc_Tcutil_terr(
_tmp593,({const char*_tmp5BE="initializes attribute on non-pointer";
_tag_dyneither(_tmp5BE,sizeof(char),37);}),_tag_dyneither(_tmp5BD,sizeof(void*),
0));});_LL36E:;}goto _LL369;}}_LL36C:;_LL36D: goto _LL369;_LL369:;}inflow=Cyc_CfFlowInfo_ReachableFL(
_tmp597,_tmp598,_tmp599);}_LL35F:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp592,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp5C8=_region_malloc(_tmp592,sizeof(*_tmp5C8));_tmp5C8->r=_tmp592;_tmp5C8->fenv=
fenv;_tmp5C8->iterate_again=1;_tmp5C8->iteration_num=0;_tmp5C8->in_try=0;_tmp5C8->tryflow=
inflow;_tmp5C8->all_changed=0;_tmp5C8->noreturn=noreturn;_tmp5C8->param_roots=
param_roots;_tmp5C8->flow_table=flow_table;_tmp5C8;});union Cyc_CfFlowInfo_FlowInfo
outflow=inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{union Cyc_CfFlowInfo_FlowInfo
_tmp5BF=outflow;int _tmp5C0;_LL374: if((_tmp5BF.BottomFL).tag != 1)goto _LL376;
_tmp5C0=(int)(_tmp5BF.BottomFL).val;_LL375: goto _LL373;_LL376:;_LL377: Cyc_NewControlFlow_check_init_params(
_tmp593,env,outflow);if(noreturn)({void*_tmp5C1=0;Cyc_Tcutil_terr(_tmp593,({
const char*_tmp5C2="`noreturn' function might (implicitly) return";_tag_dyneither(
_tmp5C2,sizeof(char),46);}),_tag_dyneither(_tmp5C1,sizeof(void*),0));});else{
void*_tmp5C3=Cyc_Tcutil_compress(fd->ret_type);_LL379: if((int)_tmp5C3 != 0)goto
_LL37B;_LL37A: goto _LL378;_LL37B: if(_tmp5C3 <= (void*)4)goto _LL37D;if(*((int*)
_tmp5C3)!= 6)goto _LL37D;_LL37C: goto _LL37E;_LL37D: if((int)_tmp5C3 != 1)goto _LL37F;
_LL37E: goto _LL380;_LL37F: if(_tmp5C3 <= (void*)4)goto _LL381;if(*((int*)_tmp5C3)!= 
5)goto _LL381;_LL380:({void*_tmp5C4=0;Cyc_Tcutil_warn(_tmp593,({const char*_tmp5C5="function may not return a value";
_tag_dyneither(_tmp5C5,sizeof(char),32);}),_tag_dyneither(_tmp5C4,sizeof(void*),
0));});goto _LL378;_LL381:;_LL382:({void*_tmp5C6=0;Cyc_Tcutil_terr(_tmp593,({
const char*_tmp5C7="function may not return a value";_tag_dyneither(_tmp5C7,
sizeof(char),32);}),_tag_dyneither(_tmp5C6,sizeof(void*),0));});goto _LL378;
_LL378:;}goto _LL373;_LL373:;}}}}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*
ds){for(0;ds != 0;ds=ds->tl){void*_tmp5C9=((struct Cyc_Absyn_Decl*)ds->hd)->r;
struct Cyc_Absyn_Fndecl*_tmp5CA;struct Cyc_List_List*_tmp5CB;struct Cyc_List_List*
_tmp5CC;struct Cyc_List_List*_tmp5CD;_LL384: if(_tmp5C9 <= (void*)2)goto _LL38E;if(*((
int*)_tmp5C9)!= 1)goto _LL386;_tmp5CA=((struct Cyc_Absyn_Fn_d_struct*)_tmp5C9)->f1;
_LL385: Cyc_NewControlFlow_check_fun(_tmp5CA);goto _LL383;_LL386: if(*((int*)
_tmp5C9)!= 12)goto _LL388;_tmp5CB=((struct Cyc_Absyn_ExternC_d_struct*)_tmp5C9)->f1;
_LL387: _tmp5CC=_tmp5CB;goto _LL389;_LL388: if(*((int*)_tmp5C9)!= 11)goto _LL38A;
_tmp5CC=((struct Cyc_Absyn_Using_d_struct*)_tmp5C9)->f2;_LL389: _tmp5CD=_tmp5CC;
goto _LL38B;_LL38A: if(*((int*)_tmp5C9)!= 10)goto _LL38C;_tmp5CD=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp5C9)->f2;_LL38B: Cyc_NewControlFlow_cf_check(_tmp5CD);goto _LL383;_LL38C: if(*((
int*)_tmp5C9)!= 13)goto _LL38E;_LL38D: goto _LL383;_LL38E:;_LL38F: goto _LL383;_LL383:;}}
