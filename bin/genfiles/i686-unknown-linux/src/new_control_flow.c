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
extern char Cyc_Position_Nocontext[14];int Cyc_Position_error_p();struct Cyc_Absyn_Loc_n_struct{
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_aggr_type(void*t);struct Cyc_RgnOrder_RgnPO;
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
le;int allow_valueof;};void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_path(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);int Cyc_Tcutil_is_noreturn(
void*);struct _tuple4{unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
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
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct _dyneither_ptr f1;};int Cyc_CfFlowInfo_update_place_set(
struct Cyc_Dict_Dict*set,void*place,struct Cyc_Position_Segment*loc);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int
disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict consumed;struct Cyc_List_List*
may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(struct
_RegionHandle*,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2);int Cyc_CfFlowInfo_consume_approx(struct Cyc_CfFlowInfo_ConsumeInfo c1,struct
Cyc_CfFlowInfo_ConsumeInfo c2);struct Cyc_CfFlowInfo_BottomFL_struct{int tag;};
struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;struct Cyc_List_List*
f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*
f);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,
void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo_union
f1,union Cyc_CfFlowInfo_FlowInfo_union f2);void Cyc_CfFlowInfo_print_place(struct
Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,
void(*pr)(void*));void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(
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
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,
union Cyc_CfFlowInfo_FlowInfo_union,union Cyc_CfFlowInfo_FlowInfo_union,int);
struct _tuple5{union Cyc_CfFlowInfo_FlowInfo_union f1;void*f2;};struct _tuple5 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple5 pr1,
struct _tuple5 pr2,int);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_after_flow(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo_union,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo_union f,void*rgn);struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_consume_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);void Cyc_CfFlowInfo_check_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo_union f);struct _tuple6{struct Cyc_CfFlowInfo_ConsumeInfo
f1;union Cyc_CfFlowInfo_FlowInfo_union f2;};struct _tuple6 Cyc_CfFlowInfo_save_consume_info(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo_union f,int clear);
union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo_union
f,struct Cyc_CfFlowInfo_ConsumeInfo c,int may_consume_only);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
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
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){(void*)(enclosee->annot=(
void*)((void*)({struct Cyc_NewControlFlow_CFAnnot_struct*_tmp0=_cycalloc(sizeof(*
_tmp0));_tmp0[0]=({struct Cyc_NewControlFlow_CFAnnot_struct _tmp1;_tmp1.tag=Cyc_NewControlFlow_CFAnnot;
_tmp1.f1=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp2;_tmp2.encloser=encloser;
_tmp2.visited=0;_tmp2;});_tmp1;});_tmp0;})));}struct Cyc_NewControlFlow_AnalEnv{
struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int
iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo_union tryflow;struct Cyc_Dict_Dict*
all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_Hashtable_Table*
flow_table;};static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Stmt*);
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Decl*);struct _tuple7{union Cyc_CfFlowInfo_FlowInfo_union
f1;union Cyc_CfFlowInfo_AbsLVal_union f2;};static struct _tuple7 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);
static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);struct _tuple8{union Cyc_CfFlowInfo_FlowInfo_union
f1;union Cyc_CfFlowInfo_FlowInfo_union f2;};static struct _tuple8 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,void*t);static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){void*_tmp3=(void*)s->annot;
struct Cyc_NewControlFlow_CFStmtAnnot _tmp4;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp5;_LL1: if(*((void**)_tmp3)!= Cyc_NewControlFlow_CFAnnot)goto _LL3;_tmp4=((
struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((
struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_LL2: return _tmp5;_LL3:;_LL4:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp6=_cycalloc(sizeof(*
_tmp6));_tmp6[0]=({struct Cyc_Core_Impossible_struct _tmp7;_tmp7.tag=Cyc_Core_Impossible;
_tmp7.f1=({const char*_tmp8="ControlFlow -- wrong stmt annotation";_tag_dyneither(
_tmp8,sizeof(char),37);});_tmp7;});_tmp6;}));_LL0:;}static union Cyc_CfFlowInfo_FlowInfo_union*
Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*
s){union Cyc_CfFlowInfo_FlowInfo_union**sflow=((union Cyc_CfFlowInfo_FlowInfo_union**(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(
env->flow_table,s);if(sflow == 0){union Cyc_CfFlowInfo_FlowInfo_union*res=({union
Cyc_CfFlowInfo_FlowInfo_union*_tmp9=_region_malloc(env->r,sizeof(*_tmp9));_tmp9[
0]=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmpA;(
_tmpA.BottomFL).tag=0;_tmpA;});_tmp9;});((void(*)(struct Cyc_Hashtable_Table*t,
struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo_union*val))Cyc_Hashtable_insert)(
env->flow_table,s,res);return res;}return*sflow;}struct _tuple9{struct Cyc_NewControlFlow_CFStmtAnnot*
f1;union Cyc_CfFlowInfo_FlowInfo_union*f2;};static struct _tuple9 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo_union*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,
s);*_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC,1);
_tmpB->visited=env->iteration_num;return({struct _tuple9 _tmpD;_tmpD.f1=_tmpB;
_tmpD.f2=_tmpC;_tmpD;});}static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union new_flow){if(env->in_try){env->tryflow=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,new_flow,env->tryflow,1);{struct Cyc_CfFlowInfo_ConsumeInfo
_tmpF;struct Cyc_Dict_Dict _tmp10;struct Cyc_List_List*_tmp11;struct _tuple6 _tmpE=
Cyc_CfFlowInfo_save_consume_info(env->fenv,env->tryflow,0);_tmpF=_tmpE.f1;_tmp10=
_tmpF.consumed;_tmp11=_tmpF.may_consume;env->tryflow=Cyc_CfFlowInfo_restore_consume_info(
env->tryflow,({struct Cyc_CfFlowInfo_ConsumeInfo _tmp12;_tmp12.consumed=_tmp10;
_tmp12.may_consume=0;_tmp12;}),0);}}}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo_union
flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp13=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo_union*_tmp14=Cyc_NewControlFlow_get_stmt_flow(
env,s);union Cyc_CfFlowInfo_FlowInfo_union _tmp15=Cyc_CfFlowInfo_join_flow(env->fenv,
env->all_changed,flow,*_tmp14,1);if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp15,*
_tmp14)){*_tmp14=_tmp15;if(_tmp13->visited == env->iteration_num)env->iterate_again=
1;}}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_add_vars(struct
Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_List_List*
vds,void*leafval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo_union
_tmp16=inflow;struct Cyc_Dict_Dict _tmp17;struct Cyc_List_List*_tmp18;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp19;_LL6: if((_tmp16.BottomFL).tag != 0)goto _LL8;_LL7: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1A;(_tmp1A.BottomFL).tag=0;_tmp1A;});_LL8:
if((_tmp16.ReachableFL).tag != 1)goto _LL5;_tmp17=(_tmp16.ReachableFL).f1;_tmp18=(
_tmp16.ReachableFL).f2;_tmp19=(_tmp16.ReachableFL).f3;_LL9: for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp1B=({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp1C=_region_malloc(fenv->r,sizeof(*_tmp1C));_tmp1C[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp1D;});_tmp1C;});
_tmp17=Cyc_Dict_insert(_tmp17,(void*)_tmp1B,Cyc_CfFlowInfo_typ_to_absrval(fenv,(
void*)((struct Cyc_Absyn_Vardecl*)vds->hd)->type,leafval));}return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1E;(_tmp1E.ReachableFL).tag=1;(_tmp1E.ReachableFL).f1=
_tmp17;(_tmp1E.ReachableFL).f2=_tmp18;(_tmp1E.ReachableFL).f3=_tmp19;_tmp1E;});
_LL5:;}struct _tuple10{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*
f2;};static void Cyc_NewControlFlow_remove_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,
union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Dict_Dict*out_unique_places,
struct Cyc_Dict_Dict old_unique_places){struct Cyc_CfFlowInfo_ConsumeInfo _tmp20;
struct Cyc_Dict_Dict _tmp21;struct _tuple6 _tmp1F=Cyc_CfFlowInfo_save_consume_info(
fenv,outflow,0);_tmp20=_tmp1F.f1;_tmp21=_tmp20.consumed;{struct _RegionHandle
_tmp22=_new_region("tmp");struct _RegionHandle*tmp=& _tmp22;_push_region(tmp);{
struct Cyc_Iter_Iter _tmp23=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,
struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(tmp,*((struct Cyc_Dict_Dict*)
_check_null(out_unique_places)));struct _tuple10 _tmp24=*((struct _tuple10*(*)(
struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(tmp,*
out_unique_places);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple10*))Cyc_Iter_next)(
_tmp23,& _tmp24)){struct Cyc_CfFlowInfo_Place*_tmp25=_tmp24.f1;if(!((int(*)(struct
Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp21,_tmp25))({
struct Cyc_String_pa_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_CfFlowInfo_place_err_string(_tmp25));{void*_tmp26[1]={&
_tmp28};Cyc_Tcutil_terr(_tmp24.f2,({const char*_tmp27="Failed to consume unique variable %s";
_tag_dyneither(_tmp27,sizeof(char),37);}),_tag_dyneither(_tmp26,sizeof(void*),1));}});}};
_pop_region(tmp);}*out_unique_places=old_unique_places;}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo_union inflow,void*r){union Cyc_CfFlowInfo_FlowInfo_union
_tmp29=inflow;struct Cyc_Dict_Dict _tmp2A;struct Cyc_List_List*_tmp2B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2C;_LLB: if((_tmp29.BottomFL).tag != 0)goto _LLD;_LLC: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp2D;(_tmp2D.BottomFL).tag=0;_tmp2D;});_LLD:
if((_tmp29.ReachableFL).tag != 1)goto _LLA;_tmp2A=(_tmp29.ReachableFL).f1;_tmp2B=(
_tmp29.ReachableFL).f2;_tmp2C=(_tmp29.ReachableFL).f3;_LLE: if(Cyc_CfFlowInfo_initlevel(
_tmp2A,r)!= (void*)2)({void*_tmp2E=0;Cyc_Tcutil_terr(loc,({const char*_tmp2F="expression may not be fully initialized";
_tag_dyneither(_tmp2F,sizeof(char),40);}),_tag_dyneither(_tmp2E,sizeof(void*),0));});{
struct Cyc_Dict_Dict _tmp30=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp2A,env->all_changed,
r);if(_tmp2A.t == _tmp30.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union _tmp31=({
union Cyc_CfFlowInfo_FlowInfo_union _tmp32;(_tmp32.ReachableFL).tag=1;(_tmp32.ReachableFL).f1=
_tmp30;(_tmp32.ReachableFL).f2=_tmp2B;(_tmp32.ReachableFL).f3=_tmp2C;_tmp32;});
Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)_tmp31);
return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp31;}}_LLA:;}struct _tuple11{union
Cyc_CfFlowInfo_FlowInfo_union f1;struct Cyc_List_List*f2;};static struct _tuple11 Cyc_NewControlFlow_anal_unordered_Rexps(
struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*es,int arg1_unconsumed){if(es == 0)return({struct
_tuple11 _tmp33;_tmp33.f1=inflow;_tmp33.f2=0;_tmp33;});if(es->tl == 0){union Cyc_CfFlowInfo_FlowInfo_union
_tmp35;void*_tmp36;struct _tuple5 _tmp34=Cyc_NewControlFlow_anal_Rexp(env,inflow,(
struct Cyc_Absyn_Exp*)es->hd);_tmp35=_tmp34.f1;_tmp36=_tmp34.f2;return({struct
_tuple11 _tmp37;_tmp37.f1=_tmp35;_tmp37.f2=({struct Cyc_List_List*_tmp38=
_region_malloc(rgn,sizeof(*_tmp38));_tmp38->hd=(void*)_tmp36;_tmp38->tl=0;_tmp38;});
_tmp37;});}{struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;struct Cyc_Dict_Dict
this_all_changed;union Cyc_CfFlowInfo_FlowInfo_union old_inflow;union Cyc_CfFlowInfo_FlowInfo_union
outflow;struct Cyc_List_List*rvals;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3A;union
Cyc_CfFlowInfo_FlowInfo_union _tmp3B;struct _tuple6 _tmp39=Cyc_CfFlowInfo_save_consume_info(
env->fenv,inflow,1);_tmp3A=_tmp39.f1;_tmp3B=_tmp39.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
outflow_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp5C;_tmp5C.consumed=(env->fenv)->mt_place_set;
_tmp5C.may_consume=0;_tmp5C;});int init_consume=0;do{this_all_changed=(env->fenv)->mt_place_set;
_tmp3B=Cyc_CfFlowInfo_restore_consume_info(_tmp3B,_tmp3A,0);env->all_changed=({
struct Cyc_Dict_Dict*_tmp3C=_region_malloc(env->r,sizeof(*_tmp3C));_tmp3C[0]=(env->fenv)->mt_place_set;
_tmp3C;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp3E;void*_tmp3F;struct _tuple5
_tmp3D=Cyc_NewControlFlow_anal_Rexp(env,_tmp3B,(struct Cyc_Absyn_Exp*)es->hd);
_tmp3E=_tmp3D.f1;_tmp3F=_tmp3D.f2;outflow=_tmp3E;rvals=({struct Cyc_List_List*
_tmp40=_region_malloc(rgn,sizeof(*_tmp40));_tmp40->hd=(void*)_tmp3F;_tmp40->tl=0;
_tmp40;});this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((
struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);if(arg1_unconsumed){union
Cyc_CfFlowInfo_FlowInfo_union _tmp41=outflow;struct Cyc_Dict_Dict _tmp42;struct Cyc_List_List*
_tmp43;struct Cyc_CfFlowInfo_ConsumeInfo _tmp44;_LL10: if((_tmp41.BottomFL).tag != 0)
goto _LL12;_LL11: goto _LLF;_LL12: if((_tmp41.ReachableFL).tag != 1)goto _LLF;_tmp42=(
_tmp41.ReachableFL).f1;_tmp43=(_tmp41.ReachableFL).f2;_tmp44=(_tmp41.ReachableFL).f3;
_LL13: outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp45;(_tmp45.ReachableFL).tag=1;(_tmp45.ReachableFL).f1=_tmp42;(_tmp45.ReachableFL).f2=
_tmp43;(_tmp45.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp46;_tmp46.consumed=
_tmp44.consumed;_tmp46.may_consume=_tmp3A.may_consume;_tmp46;});_tmp45;});_LLF:;}{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){env->all_changed=({
struct Cyc_Dict_Dict*_tmp47=_region_malloc(env->r,sizeof(*_tmp47));_tmp47[0]=(env->fenv)->mt_place_set;
_tmp47;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp49;void*_tmp4A;struct _tuple5
_tmp48=Cyc_NewControlFlow_anal_Rexp(env,_tmp3B,(struct Cyc_Absyn_Exp*)es2->hd);
_tmp49=_tmp48.f1;_tmp4A=_tmp48.f2;rvals=({struct Cyc_List_List*_tmp4B=
_region_malloc(rgn,sizeof(*_tmp4B));_tmp4B->hd=(void*)_tmp4A;_tmp4B->tl=rvals;
_tmp4B;});outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp49,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);}}}{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp4D;struct _tuple6 _tmp4C=Cyc_CfFlowInfo_save_consume_info(
env->fenv,outflow,0);_tmp4D=_tmp4C.f1;if(init_consume){if(!Cyc_CfFlowInfo_consume_approx(
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
old_inflow=Cyc_CfFlowInfo_restore_consume_info(_tmp3B,outflow_consume,0);}else{
old_inflow=_tmp3B;}init_consume=1;outflow_consume=_tmp4D;_tmp3B=Cyc_CfFlowInfo_join_flow(
env->fenv,outer_all_changed,_tmp3B,outflow,1);}}}while(!Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp3B,old_inflow));if(outer_all_changed == 0)env->all_changed=0;else{env->all_changed=({
struct Cyc_Dict_Dict*_tmp5A=_region_malloc(env->r,sizeof(*_tmp5A));_tmp5A[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0);_tmp5A;});}Cyc_NewControlFlow_update_tryflow(
env,outflow);return({struct _tuple11 _tmp5B;_tmp5B.f1=outflow;_tmp5B.f2=Cyc_List_imp_rev(
rvals);_tmp5B;});}}}static struct _tuple5 Cyc_NewControlFlow_anal_use_ints(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct
Cyc_List_List*es,int arg1_unconsumed){struct _RegionHandle _tmp5D=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp5D;_push_region(rgn);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5F;struct Cyc_List_List*_tmp60;struct _tuple11 _tmp5E=Cyc_NewControlFlow_anal_unordered_Rexps(
rgn,env,inflow,es,arg1_unconsumed);_tmp5F=_tmp5E.f1;_tmp60=_tmp5E.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp61=_tmp5F;struct Cyc_Dict_Dict _tmp62;_LL15: if((_tmp61.ReachableFL).tag != 1)
goto _LL17;_tmp62=(_tmp61.ReachableFL).f1;_LL16: for(0;_tmp60 != 0;(_tmp60=_tmp60->tl,
es=((struct Cyc_List_List*)_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(
_tmp62,(void*)_tmp60->hd)== (void*)0)({void*_tmp63=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,({const char*_tmp64="expression may not be initialized";
_tag_dyneither(_tmp64,sizeof(char),34);}),_tag_dyneither(_tmp63,sizeof(void*),0));});}
goto _LL14;_LL17: if((_tmp61.BottomFL).tag != 0)goto _LL14;_LL18: goto _LL14;_LL14:;}{
struct _tuple5 _tmp66=({struct _tuple5 _tmp65;_tmp65.f1=_tmp5F;_tmp65.f2=(void*)(env->fenv)->unknown_all;
_tmp65;});_npop_handler(0);return _tmp66;}};_pop_region(rgn);}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Absyn_Exp*e,void*il){
union Cyc_CfFlowInfo_FlowInfo_union _tmp67=outflow;struct Cyc_Dict_Dict _tmp68;
struct Cyc_List_List*_tmp69;struct Cyc_CfFlowInfo_ConsumeInfo _tmp6A;_LL1A: if((
_tmp67.BottomFL).tag != 0)goto _LL1C;_LL1B: return outflow;_LL1C: if((_tmp67.ReachableFL).tag
!= 1)goto _LL19;_tmp68=(_tmp67.ReachableFL).f1;_tmp69=(_tmp67.ReachableFL).f2;
_tmp6A=(_tmp67.ReachableFL).f3;_LL1D: {union Cyc_CfFlowInfo_AbsLVal_union _tmp6B=(
Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp6C;
_LL1F: if((_tmp6B.UnknownL).tag != 1)goto _LL21;_LL20: return outflow;_LL21: if((
_tmp6B.PlaceL).tag != 0)goto _LL1E;_tmp6C=(_tmp6B.PlaceL).f1;_LL22: {void*nzval=il
== (void*)2?(void*)1:(void*)2;union Cyc_CfFlowInfo_FlowInfo_union _tmp6D=({union
Cyc_CfFlowInfo_FlowInfo_union _tmp6E;(_tmp6E.ReachableFL).tag=1;(_tmp6E.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp68,env->all_changed,_tmp6C,
nzval);(_tmp6E.ReachableFL).f2=_tmp69;(_tmp6E.ReachableFL).f3=_tmp6A;_tmp6E;});
return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp6D;}_LL1E:;}_LL19:;}static struct
_tuple8 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union outflow,
struct Cyc_Absyn_Exp*e,void*il){union Cyc_CfFlowInfo_FlowInfo_union _tmp6F=outflow;
struct Cyc_Dict_Dict _tmp70;struct Cyc_List_List*_tmp71;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp72;_LL24: if((_tmp6F.BottomFL).tag != 0)goto _LL26;_LL25: return({struct _tuple8
_tmp73;_tmp73.f1=outflow;_tmp73.f2=outflow;_tmp73;});_LL26: if((_tmp6F.ReachableFL).tag
!= 1)goto _LL23;_tmp70=(_tmp6F.ReachableFL).f1;_tmp71=(_tmp6F.ReachableFL).f2;
_tmp72=(_tmp6F.ReachableFL).f3;_LL27: {union Cyc_CfFlowInfo_AbsLVal_union _tmp74=(
Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp75;
_LL29: if((_tmp74.UnknownL).tag != 1)goto _LL2B;_LL2A: return({struct _tuple8 _tmp76;
_tmp76.f1=outflow;_tmp76.f2=outflow;_tmp76;});_LL2B: if((_tmp74.PlaceL).tag != 0)
goto _LL28;_tmp75=(_tmp74.PlaceL).f1;_LL2C: {void*nzval=il == (void*)2?(void*)1:(
void*)2;return({struct _tuple8 _tmp77;_tmp77.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp79;(_tmp79.ReachableFL).tag=1;(_tmp79.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp70,env->all_changed,_tmp75,
nzval);(_tmp79.ReachableFL).f2=_tmp71;(_tmp79.ReachableFL).f3=_tmp72;_tmp79;});
_tmp77.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp78;(_tmp78.ReachableFL).tag=1;(_tmp78.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp70,env->all_changed,_tmp75,(void*)0);(_tmp78.ReachableFL).f2=
_tmp71;(_tmp78.ReachableFL).f3=_tmp72;_tmp78;});_tmp77;});}_LL28:;}_LL23:;}
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_if_tagcmp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union flow,struct Cyc_Absyn_Exp*e1,void*r1,void*p,
void*r2){union Cyc_CfFlowInfo_FlowInfo_union _tmp7A=flow;struct Cyc_Dict_Dict _tmp7B;
struct Cyc_List_List*_tmp7C;struct Cyc_CfFlowInfo_ConsumeInfo _tmp7D;_LL2E: if((
_tmp7A.BottomFL).tag != 0)goto _LL30;_LL2F: return flow;_LL30: if((_tmp7A.ReachableFL).tag
!= 1)goto _LL2D;_tmp7B=(_tmp7A.ReachableFL).f1;_tmp7C=(_tmp7A.ReachableFL).f2;
_tmp7D=(_tmp7A.ReachableFL).f3;_LL31: {void*_tmp7E=r2;struct Cyc_List_List*_tmp7F;
_LL33: if(_tmp7E <= (void*)3)goto _LL35;if(*((int*)_tmp7E)!= 3)goto _LL35;_tmp7F=((
struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp7E)->f1;_LL34: {union Cyc_CfFlowInfo_AbsLVal_union
_tmp80=(Cyc_NewControlFlow_anal_Lexp(env,flow,e1)).f2;struct Cyc_CfFlowInfo_Place*
_tmp81;_LL38: if((_tmp80.UnknownL).tag != 1)goto _LL3A;_LL39: return flow;_LL3A: if((
_tmp80.PlaceL).tag != 0)goto _LL37;_tmp81=(_tmp80.PlaceL).f1;_LL3B: {struct Cyc_List_List*
new_cl;{void*_tmp82=r1;struct Cyc_List_List*_tmp83;void*_tmp84;_LL3D: if(_tmp82 <= (
void*)3)goto _LL41;if(*((int*)_tmp82)!= 3)goto _LL3F;_tmp83=((struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp82)->f1;_LL3E: new_cl=_tmp83;goto _LL3C;_LL3F: if(*((int*)_tmp82)!= 0)goto _LL41;
_tmp84=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp82)->f1;if((int)_tmp84
!= 2)goto _LL41;_LL40: goto _LL42;_LL41: if((int)_tmp82 != 0)goto _LL43;_LL42: goto
_LL44;_LL43: if((int)_tmp82 != 1)goto _LL45;_LL44: new_cl=0;goto _LL3C;_LL45:;_LL46:
return flow;_LL3C:;}for(0;_tmp7F != 0;_tmp7F=_tmp7F->tl){void*new_cmp;{struct
_tuple0 _tmp86=({struct _tuple0 _tmp85;_tmp85.f1=p;_tmp85.f2=(void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp7F->hd)->cmp;_tmp85;});void*_tmp87;void*_tmp88;void*_tmp89;void*_tmp8A;_LL48:
_tmp87=_tmp86.f1;if((int)_tmp87 != 8)goto _LL4A;_LL49: goto _LL4B;_LL4A: _tmp88=
_tmp86.f2;if((int)_tmp88 != 8)goto _LL4C;_LL4B: new_cmp=(void*)8;goto _LL47;_LL4C:
_tmp89=_tmp86.f1;if((int)_tmp89 != 5)goto _LL4E;_tmp8A=_tmp86.f2;if((int)_tmp8A != 
5)goto _LL4E;_LL4D: new_cmp=(void*)5;goto _LL47;_LL4E:;_LL4F: new_cmp=(void*)10;goto
_LL47;_LL47:;}new_cl=({struct Cyc_List_List*_tmp8B=_region_malloc(env->r,sizeof(*
_tmp8B));_tmp8B->hd=({struct Cyc_CfFlowInfo_TagCmp*_tmp8C=_region_malloc(env->r,
sizeof(*_tmp8C));_tmp8C->cmp=(void*)new_cmp;_tmp8C->bd=(void*)((void*)((struct
Cyc_CfFlowInfo_TagCmp*)_tmp7F->hd)->bd);_tmp8C;});_tmp8B->tl=new_cl;_tmp8B;});}
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp8D;(_tmp8D.ReachableFL).tag=1;(_tmp8D.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e1->loc,_tmp7B,env->all_changed,_tmp81,(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp8E=_region_malloc(env->r,sizeof(*_tmp8E));_tmp8E[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmp8F;_tmp8F.tag=3;_tmp8F.f1=new_cl;_tmp8F;});_tmp8E;}));(_tmp8D.ReachableFL).f2=
_tmp7C;(_tmp8D.ReachableFL).f3=_tmp7D;_tmp8D;});}_LL37:;}_LL35:;_LL36: return flow;
_LL32:;}_LL2D:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct Cyc_NewControlFlow_mt_unknownz_v={
Cyc_CfFlowInfo_UnknownZ,0};static struct _tuple5 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp92=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp93;void*_tmp94;struct Cyc_Absyn_PtrAtts _tmp95;struct
Cyc_Absyn_Conref*_tmp96;struct Cyc_Absyn_Conref*_tmp97;_LL51: if(_tmp92 <= (void*)4)
goto _LL53;if(*((int*)_tmp92)!= 4)goto _LL53;_tmp93=((struct Cyc_Absyn_PointerType_struct*)
_tmp92)->f1;_tmp94=(void*)_tmp93.elt_typ;_tmp95=_tmp93.ptr_atts;_tmp96=_tmp95.bounds;
_tmp97=_tmp95.zero_term;_LL52: {union Cyc_CfFlowInfo_FlowInfo_union _tmp98=f;
struct Cyc_Dict_Dict _tmp99;struct Cyc_List_List*_tmp9A;_LL56: if((_tmp98.BottomFL).tag
!= 0)goto _LL58;_LL57: return({struct _tuple5 _tmp9B;_tmp9B.f1=f;_tmp9B.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp94,(void*)(env->fenv)->unknown_all);_tmp9B;});_LL58: if((_tmp98.ReachableFL).tag
!= 1)goto _LL55;_tmp99=(_tmp98.ReachableFL).f1;_tmp9A=(_tmp98.ReachableFL).f2;
_LL59: if(Cyc_Tcutil_is_bound_one(_tmp96)){void*_tmp9C=r;struct Cyc_CfFlowInfo_Place*
_tmp9D;void*_tmp9E;_LL5B: if((int)_tmp9C != 1)goto _LL5D;_LL5C: goto _LL5E;_LL5D: if((
int)_tmp9C != 2)goto _LL5F;_LL5E:{void*_tmp9F=(void*)e->annot;struct Cyc_List_List*
_tmpA0;_LL68: if(*((void**)_tmp9F)!= Cyc_CfFlowInfo_NotZero)goto _LL6A;_tmpA0=((
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp9F)->f1;_LL69: if(!Cyc_CfFlowInfo_same_relns(
_tmp9A,_tmpA0))goto _LL6B;goto _LL67;_LL6A:;_LL6B:{void*_tmpA1=(void*)e->r;_LL6D:
if(*((int*)_tmpA1)!= 25)goto _LL6F;_LL6E:(void*)(e->annot=(void*)((void*)({struct
Cyc_CfFlowInfo_NotZero_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({
struct Cyc_CfFlowInfo_NotZero_struct _tmpA3;_tmpA3.tag=Cyc_CfFlowInfo_NotZero;
_tmpA3.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9A);_tmpA3;});
_tmpA2;})));goto _LL6C;_LL6F:;_LL70:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_notzero_v));
goto _LL6C;_LL6C:;}goto _LL67;_LL67:;}goto _LL5A;_LL5F: if(_tmp9C <= (void*)3)goto
_LL61;if(*((int*)_tmp9C)!= 2)goto _LL61;_tmp9D=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp9C)->f1;_LL60:{void*_tmpA4=(void*)e->annot;struct Cyc_List_List*_tmpA5;_LL72:
if(*((void**)_tmpA4)!= Cyc_CfFlowInfo_NotZero)goto _LL74;_tmpA5=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpA4)->f1;_LL73: if(!Cyc_CfFlowInfo_same_relns(_tmp9A,_tmpA5))goto _LL75;goto
_LL71;_LL74:;_LL75:{void*_tmpA6=(void*)e->r;_LL77: if(*((int*)_tmpA6)!= 25)goto
_LL79;_LL78:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmpA8;_tmpA8.tag=Cyc_CfFlowInfo_NotZero;_tmpA8.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp9A);_tmpA8;});_tmpA7;})));goto _LL76;_LL79:;_LL7A:(void*)(e->annot=(void*)((
void*)& Cyc_NewControlFlow_mt_notzero_v));goto _LL76;_LL76:;}goto _LL71;_LL71:;}
return({struct _tuple5 _tmpA9;_tmpA9.f1=f;_tmpA9.f2=Cyc_CfFlowInfo_lookup_place(
_tmp99,_tmp9D);_tmpA9;});_LL61: if((int)_tmp9C != 0)goto _LL63;_LL62:(void*)(e->annot=(
void*)((void*)Cyc_CfFlowInfo_IsZero));return({struct _tuple5 _tmpAA;_tmpAA.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmpAB;(
_tmpAB.BottomFL).tag=0;_tmpAB;});_tmpAA.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,
_tmp94,(void*)(env->fenv)->unknown_all);_tmpAA;});_LL63: if(_tmp9C <= (void*)3)
goto _LL65;if(*((int*)_tmp9C)!= 0)goto _LL65;_tmp9E=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp9C)->f1;_LL64: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp9E);goto _LL66;
_LL65:;_LL66:{void*_tmpAC=(void*)e->r;_LL7C: if(*((int*)_tmpAC)!= 25)goto _LL7E;
_LL7D:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmpAE;_tmpAE.tag=Cyc_CfFlowInfo_UnknownZ;_tmpAE.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp9A);_tmpAE;});_tmpAD;})));goto _LL7B;_LL7E:;_LL7F:(void*)(
e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_unknownz_v));goto _LL7B;_LL7B:;}
goto _LL5A;_LL5A:;}else{void*_tmpAF=(void*)e->annot;struct Cyc_List_List*_tmpB0;
_LL81: if(*((void**)_tmpAF)!= Cyc_CfFlowInfo_UnknownZ)goto _LL83;_tmpB0=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmpAF)->f1;_LL82: if(!Cyc_CfFlowInfo_same_relns(
_tmp9A,_tmpB0))goto _LL84;goto _LL80;_LL83:;_LL84:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _tmpB2;_tmpB2.tag=Cyc_CfFlowInfo_UnknownZ;
_tmpB2.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9A);_tmpB2;});
_tmpB1;})));goto _LL80;_LL80:;}{void*_tmpB3=Cyc_CfFlowInfo_initlevel(_tmp99,r);
_LL86: if((int)_tmpB3 != 0)goto _LL88;_LL87:({void*_tmpB4=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmpB5="dereference of possibly uninitialized pointer";_tag_dyneither(
_tmpB5,sizeof(char),46);}),_tag_dyneither(_tmpB4,sizeof(void*),0));});goto _LL89;
_LL88: if((int)_tmpB3 != 2)goto _LL8A;_LL89: return({struct _tuple5 _tmpB6;_tmpB6.f1=f;
_tmpB6.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp94,(void*)(env->fenv)->unknown_all);
_tmpB6;});_LL8A: if((int)_tmpB3 != 1)goto _LL85;_LL8B: return({struct _tuple5 _tmpB7;
_tmpB7.f1=f;_tmpB7.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp94,(void*)(env->fenv)->unknown_none);
_tmpB7;});_LL85:;}_LL55:;}_LL53:;_LL54:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=({struct Cyc_Core_Impossible_struct
_tmpB9;_tmpB9.tag=Cyc_Core_Impossible;_tmpB9.f1=({const char*_tmpBA="right deref of non-pointer-type";
_tag_dyneither(_tmpBA,sizeof(char),32);});_tmpB9;});_tmpB8;}));_LL50:;}static
struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*
rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmpBB=(void*)e1->r;void*_tmpBC;struct Cyc_Absyn_Vardecl*_tmpBD;void*_tmpBE;
struct Cyc_Absyn_Vardecl*_tmpBF;void*_tmpC0;struct Cyc_Absyn_Vardecl*_tmpC1;void*
_tmpC2;struct Cyc_Absyn_Vardecl*_tmpC3;_LL8D: if(*((int*)_tmpBB)!= 1)goto _LL8F;
_tmpBC=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBB)->f2;if(_tmpBC <= (void*)1)
goto _LL8F;if(*((int*)_tmpBC)!= 4)goto _LL8F;_tmpBD=((struct Cyc_Absyn_Pat_b_struct*)
_tmpBC)->f1;_LL8E: _tmpBF=_tmpBD;goto _LL90;_LL8F: if(*((int*)_tmpBB)!= 1)goto _LL91;
_tmpBE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBB)->f2;if(_tmpBE <= (void*)1)
goto _LL91;if(*((int*)_tmpBE)!= 3)goto _LL91;_tmpBF=((struct Cyc_Absyn_Local_b_struct*)
_tmpBE)->f1;_LL90: _tmpC1=_tmpBF;goto _LL92;_LL91: if(*((int*)_tmpBB)!= 1)goto _LL93;
_tmpC0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBB)->f2;if(_tmpC0 <= (void*)1)
goto _LL93;if(*((int*)_tmpC0)!= 2)goto _LL93;_tmpC1=((struct Cyc_Absyn_Param_b_struct*)
_tmpC0)->f1;_LL92: _tmpC3=_tmpC1;goto _LL94;_LL93: if(*((int*)_tmpBB)!= 1)goto _LL95;
_tmpC2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBB)->f2;if(_tmpC2 <= (void*)1)
goto _LL95;if(*((int*)_tmpC2)!= 0)goto _LL95;_tmpC3=((struct Cyc_Absyn_Global_b_struct*)
_tmpC2)->f1;_LL94: if(!_tmpC3->escapes){void*_tmpC4=(void*)e2->r;void*_tmpC5;
struct Cyc_Absyn_Vardecl*_tmpC6;void*_tmpC7;struct Cyc_Absyn_Vardecl*_tmpC8;void*
_tmpC9;struct Cyc_Absyn_Vardecl*_tmpCA;void*_tmpCB;struct Cyc_Absyn_Vardecl*_tmpCC;
_LL98: if(*((int*)_tmpC4)!= 1)goto _LL9A;_tmpC5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC4)->f2;if(_tmpC5 <= (void*)1)goto _LL9A;if(*((int*)_tmpC5)!= 4)goto _LL9A;
_tmpC6=((struct Cyc_Absyn_Pat_b_struct*)_tmpC5)->f1;_LL99: _tmpC8=_tmpC6;goto _LL9B;
_LL9A: if(*((int*)_tmpC4)!= 1)goto _LL9C;_tmpC7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC4)->f2;if(_tmpC7 <= (void*)1)goto _LL9C;if(*((int*)_tmpC7)!= 3)goto _LL9C;
_tmpC8=((struct Cyc_Absyn_Local_b_struct*)_tmpC7)->f1;_LL9B: _tmpCA=_tmpC8;goto
_LL9D;_LL9C: if(*((int*)_tmpC4)!= 1)goto _LL9E;_tmpC9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC4)->f2;if(_tmpC9 <= (void*)1)goto _LL9E;if(*((int*)_tmpC9)!= 2)goto _LL9E;
_tmpCA=((struct Cyc_Absyn_Param_b_struct*)_tmpC9)->f1;_LL9D: _tmpCC=_tmpCA;goto
_LL9F;_LL9E: if(*((int*)_tmpC4)!= 1)goto _LLA0;_tmpCB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC4)->f2;if(_tmpCB <= (void*)1)goto _LLA0;if(*((int*)_tmpCB)!= 0)goto _LLA0;
_tmpCC=((struct Cyc_Absyn_Global_b_struct*)_tmpCB)->f1;_LL9F: if(!_tmpCC->escapes){
int found=0;{struct Cyc_List_List*_tmpCD=relns;for(0;_tmpCD != 0;_tmpCD=_tmpCD->tl){
struct Cyc_CfFlowInfo_Reln*_tmpCE=(struct Cyc_CfFlowInfo_Reln*)_tmpCD->hd;if(
_tmpCE->vd == _tmpCC){union Cyc_CfFlowInfo_RelnOp_union _tmpCF=_tmpCE->rop;struct
Cyc_Absyn_Vardecl*_tmpD0;_LLA3: if((_tmpCF.LessNumelts).tag != 2)goto _LLA5;_tmpD0=(
_tmpCF.LessNumelts).f1;if(!(_tmpD0 == _tmpC3))goto _LLA5;_LLA4: return relns;_LLA5:;
_LLA6: continue;_LLA2:;}}}if(!found)return({struct Cyc_List_List*_tmpD1=
_region_malloc(rgn,sizeof(*_tmpD1));_tmpD1->hd=({struct Cyc_CfFlowInfo_Reln*
_tmpD2=_region_malloc(rgn,sizeof(*_tmpD2));_tmpD2->vd=_tmpCC;_tmpD2->rop=(union
Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmpD3;(_tmpD3.LessNumelts).tag=
2;(_tmpD3.LessNumelts).f1=_tmpC3;_tmpD3;});_tmpD2;});_tmpD1->tl=relns;_tmpD1;});}
return relns;_LLA0:;_LLA1: return relns;_LL97:;}else{return relns;}_LL95:;_LL96:
return relns;_LL8C:;}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_may_consume_place(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,
struct Cyc_CfFlowInfo_Place*place,void*t,struct Cyc_List_List**ps){struct Cyc_CfFlowInfo_FlowEnv*
_tmpD4=env->fenv;struct Cyc_List_List*_tmpD5=Cyc_NewControlFlow_noalias_ptrs_rec(
env,place,t);if(ps != 0)*ps=_tmpD5;if(_tmpD5 == 0)return cinfo;cinfo.may_consume=((
struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(
_tmpD4->r,Cyc_CfFlowInfo_place_cmp,_tmpD5,cinfo.may_consume);return cinfo;}struct
_tuple12{union Cyc_CfFlowInfo_AbsLVal_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_may_consume_exp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_noalias_path(e)){struct Cyc_CfFlowInfo_FlowEnv*
_tmpD6=env->fenv;union Cyc_CfFlowInfo_AbsLVal_union _tmpD8;struct _tuple7 _tmpD7=Cyc_NewControlFlow_anal_Lexp(
env,inflow,e);_tmpD8=_tmpD7.f2;{struct _tuple12 _tmpDA=({struct _tuple12 _tmpD9;
_tmpD9.f1=_tmpD8;_tmpD9.f2=inflow;_tmpD9;});union Cyc_CfFlowInfo_AbsLVal_union
_tmpDB;struct Cyc_CfFlowInfo_Place*_tmpDC;union Cyc_CfFlowInfo_FlowInfo_union
_tmpDD;struct Cyc_Dict_Dict _tmpDE;struct Cyc_List_List*_tmpDF;struct Cyc_CfFlowInfo_ConsumeInfo
_tmpE0;_LLA8: _tmpDB=_tmpDA.f1;if(((_tmpDA.f1).PlaceL).tag != 0)goto _LLAA;_tmpDC=(
_tmpDB.PlaceL).f1;_tmpDD=_tmpDA.f2;if(((_tmpDA.f2).ReachableFL).tag != 1)goto
_LLAA;_tmpDE=(_tmpDD.ReachableFL).f1;_tmpDF=(_tmpDD.ReachableFL).f2;_tmpE0=(
_tmpDD.ReachableFL).f3;_LLA9: {struct Cyc_CfFlowInfo_ConsumeInfo _tmpE1=Cyc_NewControlFlow_may_consume_place(
env,_tmpE0,_tmpDC,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0);
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmpE2;(_tmpE2.ReachableFL).tag=1;(_tmpE2.ReachableFL).f1=_tmpDE;(_tmpE2.ReachableFL).f2=
_tmpDF;(_tmpE2.ReachableFL).f3=_tmpE1;_tmpE2;});}_LLAA:;_LLAB:({struct Cyc_String_pa_struct
_tmpE5;_tmpE5.tag=0;_tmpE5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmpE3[1]={& _tmpE5};Cyc_fprintf(Cyc_stderr,({const char*_tmpE4="Oops---no location for noalias_path |%s|\n";
_tag_dyneither(_tmpE4,sizeof(char),42);}),_tag_dyneither(_tmpE3,sizeof(void*),1));}});
return inflow;_LLA7:;}}return inflow;}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_consume_place(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo
cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Position_Segment*loc){if(!((int(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(cinfo.consumed,
p)){struct Cyc_CfFlowInfo_Place _tmpE7;void*_tmpE8;struct Cyc_List_List*_tmpE9;
struct Cyc_CfFlowInfo_Place*_tmpE6=p;_tmpE7=*_tmpE6;_tmpE8=(void*)_tmpE7.root;
_tmpE9=_tmpE7.fields;{void*rval=(void*)0;if((Cyc_Dict_lookup_bool(outdict,_tmpE8,&
rval) && Cyc_CfFlowInfo_initlevel(outdict,rval)!= (void*)0) && rval != (void*)0){
void*_tmpEA=_tmpE8;struct Cyc_Absyn_Vardecl*_tmpEB;_LLAD: if(*((int*)_tmpEA)!= 0)
goto _LLAF;_tmpEB=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmpEA)->f1;_LLAE: {
struct _dyneither_ptr _tmpEC=Cyc_Absynpp_qvar2string(_tmpEB->name);if(_tmpE9 == 0)({
struct Cyc_String_pa_struct _tmpEF;_tmpEF.tag=0;_tmpEF.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmpEC);{void*_tmpED[1]={& _tmpEF};Cyc_Tcutil_warn(loc,({
const char*_tmpEE="may clobber unique pointer %s";_tag_dyneither(_tmpEE,sizeof(
char),30);}),_tag_dyneither(_tmpED,sizeof(void*),1));}});else{({struct Cyc_String_pa_struct
_tmpF2;_tmpF2.tag=0;_tmpF2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmpEC);{void*_tmpF0[1]={& _tmpF2};Cyc_Tcutil_warn(loc,({const char*_tmpF1="may clobber unique pointer contained in %s";
_tag_dyneither(_tmpF1,sizeof(char),43);}),_tag_dyneither(_tmpF0,sizeof(void*),1));}});}
goto _LLAC;}_LLAF:;_LLB0:({void*_tmpF3=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpF4="warning locations not for VarRoots";
_tag_dyneither(_tmpF4,sizeof(char),35);}),_tag_dyneither(_tmpF3,sizeof(void*),0));});
_LLAC:;}}}if(env->all_changed == 0)cinfo.consumed=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_rdelete)((
env->fenv)->r,cinfo.consumed,p);return cinfo;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_consume_assignment(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict
outdict,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_noalias_path(e)){struct Cyc_CfFlowInfo_FlowEnv*
_tmpF5=env->fenv;struct Cyc_List_List*_tmpF6=0;cinfo=Cyc_NewControlFlow_may_consume_place(
env,cinfo,p,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(struct Cyc_List_List**)&
_tmpF6);while(_tmpF6 != 0){cinfo=Cyc_NewControlFlow_consume_place(env,(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmpF6))->hd,cinfo,outdict,e->loc);_tmpF6=((
struct Cyc_List_List*)_check_null(_tmpF6))->tl;}}return cinfo;}static struct Cyc_List_List*
Cyc_NewControlFlow_noalias_ptrs_aux(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*ts){struct Cyc_List_List*l=0;struct Cyc_CfFlowInfo_Place
_tmpF8;void*_tmpF9;struct Cyc_List_List*_tmpFA;struct Cyc_CfFlowInfo_Place*_tmpF7=
p;_tmpF8=*_tmpF7;_tmpF9=(void*)_tmpF8.root;_tmpFA=_tmpF8.fields;{int fld=0;for(0;
ts != 0;(fld ++,ts=ts->tl)){void*_tmpFB=(void*)ts->hd;if(Cyc_Tcutil_is_noalias_pointer(
_tmpFB)){struct Cyc_List_List*_tmpFC=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
env->r,_tmpFA,({int _tmpFF[1];_tmpFF[0]=fld;((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(
_tmpFF,sizeof(int),1));}));l=((struct Cyc_List_List*(*)(struct _RegionHandle*,int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmpFD=_region_malloc(env->r,sizeof(*_tmpFD));_tmpFD->hd=({
struct Cyc_CfFlowInfo_Place*_tmpFE=_region_malloc(env->r,sizeof(*_tmpFE));_tmpFE->root=(
void*)_tmpF9;_tmpFE->fields=_tmpFC;_tmpFE;});_tmpFD->tl=0;_tmpFD;}),l);}else{if(
Cyc_Absyn_is_aggr_type(_tmpFB)){struct Cyc_List_List*_tmp100=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
env->r,_tmpFA,({int _tmp103[1];_tmp103[0]=fld;((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(
_tmp103,sizeof(int),1));}));struct Cyc_CfFlowInfo_Place*_tmp101=({struct Cyc_CfFlowInfo_Place*
_tmp102=_region_malloc(env->r,sizeof(*_tmp102));_tmp102->root=(void*)_tmpF9;
_tmp102->fields=_tmp100;_tmp102;});l=((struct Cyc_List_List*(*)(struct
_RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,
l,Cyc_NewControlFlow_noalias_ptrs_rec(env,_tmp101,_tmpFB));}}}}return l;}struct
_tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t){if(Cyc_Tcutil_is_noalias_pointer(
t))return({struct Cyc_List_List*_tmp104=_region_malloc(env->r,sizeof(*_tmp104));
_tmp104->hd=p;_tmp104->tl=0;_tmp104;});{void*_tmp105=Cyc_Tcutil_compress(t);
struct Cyc_List_List*_tmp106;struct Cyc_List_List*_tmp107;struct Cyc_Absyn_AggrInfo
_tmp108;union Cyc_Absyn_AggrInfoU_union _tmp109;struct Cyc_Absyn_Aggrdecl**_tmp10A;
struct Cyc_List_List*_tmp10B;struct Cyc_Absyn_AggrInfo _tmp10C;union Cyc_Absyn_AggrInfoU_union
_tmp10D;struct Cyc_Absyn_TunionFieldInfo _tmp10E;union Cyc_Absyn_TunionFieldInfoU_union
_tmp10F;struct Cyc_List_List*_tmp110;_LLB2: if(_tmp105 <= (void*)4)goto _LLBE;if(*((
int*)_tmp105)!= 9)goto _LLB4;_tmp106=((struct Cyc_Absyn_TupleType_struct*)_tmp105)->f1;
_LLB3: {struct Cyc_List_List*_tmp111=0;while(_tmp106 != 0){_tmp111=({struct Cyc_List_List*
_tmp112=_region_malloc(env->r,sizeof(*_tmp112));_tmp112->hd=(void*)(*((struct
_tuple13*)_tmp106->hd)).f2;_tmp112->tl=_tmp111;_tmp112;});_tmp106=_tmp106->tl;}
_tmp111=Cyc_List_imp_rev(_tmp111);return Cyc_NewControlFlow_noalias_ptrs_aux(env,
p,_tmp111);}_LLB4: if(*((int*)_tmp105)!= 11)goto _LLB6;_tmp107=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp105)->f2;_LLB5: {struct Cyc_List_List*_tmp113=0;while(_tmp107 != 0){_tmp113=({
struct Cyc_List_List*_tmp114=_region_malloc(env->r,sizeof(*_tmp114));_tmp114->hd=(
void*)((void*)((struct Cyc_Absyn_Aggrfield*)_tmp107->hd)->type);_tmp114->tl=
_tmp113;_tmp114;});_tmp107=_tmp107->tl;}_tmp113=Cyc_List_imp_rev(_tmp113);return
Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp113);}_LLB6: if(*((int*)_tmp105)!= 
10)goto _LLB8;_tmp108=((struct Cyc_Absyn_AggrType_struct*)_tmp105)->f1;_tmp109=
_tmp108.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp105)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LLB8;_tmp10A=(_tmp109.KnownAggr).f1;_tmp10B=_tmp108.targs;_LLB7: if((*
_tmp10A)->impl == 0)return 0;else{struct Cyc_List_List*_tmp115=0;{struct
_RegionHandle _tmp116=_new_region("rgn");struct _RegionHandle*rgn=& _tmp116;
_push_region(rgn);{struct Cyc_List_List*_tmp117=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmp10A)->tvs,_tmp10B);struct Cyc_List_List*_tmp118=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp10A)->impl))->fields;while(
_tmp118 != 0){_tmp115=({struct Cyc_List_List*_tmp119=_region_malloc(env->r,sizeof(*
_tmp119));_tmp119->hd=(void*)Cyc_Tcutil_rsubstitute(rgn,_tmp117,(void*)((struct
Cyc_Absyn_Aggrfield*)_tmp118->hd)->type);_tmp119->tl=_tmp115;_tmp119;});_tmp118=
_tmp118->tl;}};_pop_region(rgn);}_tmp115=Cyc_List_imp_rev(_tmp115);return Cyc_NewControlFlow_noalias_ptrs_aux(
env,p,_tmp115);}_LLB8: if(*((int*)_tmp105)!= 10)goto _LLBA;_tmp10C=((struct Cyc_Absyn_AggrType_struct*)
_tmp105)->f1;_tmp10D=_tmp10C.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp105)->f1).aggr_info).UnknownAggr).tag != 0)goto _LLBA;_LLB9:({void*_tmp11A=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp11B="got unknown aggr in noalias_ptrs_rec";_tag_dyneither(_tmp11B,
sizeof(char),37);}),_tag_dyneither(_tmp11A,sizeof(void*),0));});_LLBA: if(*((int*)
_tmp105)!= 2)goto _LLBC;_LLBB: if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return({
struct Cyc_List_List*_tmp11C=_region_malloc(env->r,sizeof(*_tmp11C));_tmp11C->hd=
p;_tmp11C->tl=0;_tmp11C;});else{return 0;}_LLBC: if(*((int*)_tmp105)!= 3)goto _LLBE;
_tmp10E=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp105)->f1;_tmp10F=_tmp10E.field_info;
_tmp110=_tmp10E.targs;_LLBD: {union Cyc_Absyn_TunionFieldInfoU_union _tmp11D=
_tmp10F;struct Cyc_Absyn_Tuniondecl*_tmp11E;struct Cyc_Absyn_Tunionfield*_tmp11F;
_LLC1: if((_tmp11D.UnknownTunionfield).tag != 0)goto _LLC3;_LLC2:({void*_tmp120=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp121="got unknown tunion field in noalias_ptrs_rec";_tag_dyneither(
_tmp121,sizeof(char),45);}),_tag_dyneither(_tmp120,sizeof(void*),0));});_LLC3:
if((_tmp11D.KnownTunionfield).tag != 1)goto _LLC0;_tmp11E=(_tmp11D.KnownTunionfield).f1;
_tmp11F=(_tmp11D.KnownTunionfield).f2;_LLC4: {struct Cyc_List_List*_tmp122=0;{
struct _RegionHandle _tmp123=_new_region("rgn");struct _RegionHandle*rgn=& _tmp123;
_push_region(rgn);{struct Cyc_List_List*_tmp124=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp11E->tvs,_tmp110);struct Cyc_List_List*_tmp125=
_tmp11F->typs;while(_tmp125 != 0){_tmp122=({struct Cyc_List_List*_tmp126=
_region_malloc(env->r,sizeof(*_tmp126));_tmp126->hd=(void*)Cyc_Tcutil_rsubstitute(
rgn,_tmp124,(*((struct _tuple13*)_tmp125->hd)).f2);_tmp126->tl=_tmp122;_tmp126;});
_tmp125=_tmp125->tl;}};_pop_region(rgn);}_tmp122=Cyc_List_imp_rev(_tmp122);
return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp122);}_LLC0:;}_LLBE:;_LLBF:
return 0;_LLB1:;}}static struct _tuple5 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal_union lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_CfFlowInfo_ConsumeInfo outer_cinfo,struct Cyc_Position_Segment*
loc){outflow=Cyc_CfFlowInfo_consume_unique_rvals(loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp127=outflow;struct Cyc_Dict_Dict _tmp128;struct Cyc_List_List*_tmp129;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp12A;_LLC6: if((_tmp127.BottomFL).tag != 0)goto _LLC8;_LLC7: return({struct
_tuple5 _tmp12B;_tmp12B.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp12C;(_tmp12C.BottomFL).tag=0;_tmp12C;});_tmp12B.f2=rval;_tmp12B;});_LLC8: if((
_tmp127.ReachableFL).tag != 1)goto _LLC5;_tmp128=(_tmp127.ReachableFL).f1;_tmp129=(
_tmp127.ReachableFL).f2;_tmp12A=(_tmp127.ReachableFL).f3;_LLC9: _tmp12A=Cyc_CfFlowInfo_and_consume(
fenv->r,outer_cinfo,_tmp12A);{union Cyc_CfFlowInfo_AbsLVal_union _tmp12D=lval;
struct Cyc_CfFlowInfo_Place*_tmp12E;_LLCB: if((_tmp12D.PlaceL).tag != 0)goto _LLCD;
_tmp12E=(_tmp12D.PlaceL).f1;_LLCC: _tmp12A=Cyc_NewControlFlow_consume_assignment(
env,_tmp12E,_tmp12A,_tmp128,lexp);_tmp128=Cyc_CfFlowInfo_assign_place(fenv,loc,
_tmp128,env->all_changed,_tmp12E,rval);_tmp129=Cyc_CfFlowInfo_reln_assign_exp(
fenv->r,_tmp129,lexp,rexp);outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union
Cyc_CfFlowInfo_FlowInfo_union _tmp12F;(_tmp12F.ReachableFL).tag=1;(_tmp12F.ReachableFL).f1=
_tmp128;(_tmp12F.ReachableFL).f2=_tmp129;(_tmp12F.ReachableFL).f3=_tmp12A;
_tmp12F;});Cyc_NewControlFlow_update_tryflow(env,outflow);return({struct _tuple5
_tmp130;_tmp130.f1=outflow;_tmp130.f2=rval;_tmp130;});_LLCD: if((_tmp12D.UnknownL).tag
!= 1)goto _LLCA;_LLCE: return({struct _tuple5 _tmp131;_tmp131.f1=Cyc_NewControlFlow_use_Rval(
env,rexp->loc,outflow,rval);_tmp131.f2=rval;_tmp131;});_LLCA:;}_LLC5:;}}struct
_tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp132=env->fenv;struct Cyc_Dict_Dict
d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo cinfo;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp133=inflow;struct Cyc_Dict_Dict _tmp134;struct Cyc_List_List*_tmp135;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp136;_LLD0: if((_tmp133.BottomFL).tag != 0)goto _LLD2;_LLD1: return({struct
_tuple5 _tmp137;_tmp137.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp138;(_tmp138.BottomFL).tag=0;_tmp138;});_tmp137.f2=(void*)_tmp132->unknown_all;
_tmp137;});_LLD2: if((_tmp133.ReachableFL).tag != 1)goto _LLCF;_tmp134=(_tmp133.ReachableFL).f1;
_tmp135=(_tmp133.ReachableFL).f2;_tmp136=(_tmp133.ReachableFL).f3;_LLD3: d=
_tmp134;relns=_tmp135;cinfo=_tmp136;_LLCF:;}{void*_tmp139=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp13A;void*_tmp13B;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_Absyn_Exp*_tmp13D;
struct Cyc_Absyn_Exp*_tmp13E;union Cyc_Absyn_Cnst_union _tmp13F;union Cyc_Absyn_Cnst_union
_tmp140;int _tmp141;union Cyc_Absyn_Cnst_union _tmp142;void*_tmp143;struct Cyc_List_List*
_tmp144;void*_tmp145;void*_tmp146;struct Cyc_Absyn_Vardecl*_tmp147;void*_tmp148;
struct Cyc_Absyn_Vardecl*_tmp149;void*_tmp14A;struct Cyc_Absyn_Vardecl*_tmp14B;
void*_tmp14C;struct Cyc_List_List*_tmp14D;struct Cyc_Absyn_Exp*_tmp14E;struct Cyc_Absyn_Exp*
_tmp14F;struct Cyc_Core_Opt*_tmp150;struct Cyc_Core_Opt _tmp151;struct Cyc_Absyn_Exp*
_tmp152;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Core_Opt*_tmp154;struct Cyc_Absyn_Exp*
_tmp155;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_Absyn_Exp*_tmp157;struct Cyc_Absyn_Exp*
_tmp158;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_List_List*_tmp15A;struct Cyc_Absyn_MallocInfo
_tmp15B;int _tmp15C;struct Cyc_Absyn_Exp*_tmp15D;void**_tmp15E;struct Cyc_Absyn_Exp*
_tmp15F;int _tmp160;struct Cyc_Absyn_Exp*_tmp161;struct Cyc_Absyn_Exp*_tmp162;
struct Cyc_Absyn_Exp*_tmp163;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_Absyn_Exp*
_tmp165;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Exp*_tmp167;struct
_dyneither_ptr*_tmp168;struct Cyc_Absyn_Exp*_tmp169;struct _dyneither_ptr*_tmp16A;
struct Cyc_Absyn_Exp*_tmp16B;struct Cyc_Absyn_Exp*_tmp16C;struct Cyc_Absyn_Exp*
_tmp16D;struct Cyc_Absyn_Exp*_tmp16E;struct Cyc_Absyn_Exp*_tmp16F;struct Cyc_Absyn_Exp*
_tmp170;struct Cyc_Absyn_Exp*_tmp171;struct Cyc_Absyn_Exp*_tmp172;struct Cyc_Absyn_Exp*
_tmp173;struct Cyc_List_List*_tmp174;struct Cyc_Absyn_Tuniondecl*_tmp175;struct Cyc_List_List*
_tmp176;struct Cyc_List_List*_tmp177;struct Cyc_List_List*_tmp178;struct Cyc_Absyn_Aggrdecl*
_tmp179;struct Cyc_Absyn_Aggrdecl _tmp17A;struct Cyc_Absyn_AggrdeclImpl*_tmp17B;
struct Cyc_Absyn_AggrdeclImpl _tmp17C;struct Cyc_List_List*_tmp17D;struct Cyc_List_List*
_tmp17E;struct Cyc_Absyn_Vardecl*_tmp17F;struct Cyc_Absyn_Exp*_tmp180;struct Cyc_Absyn_Exp*
_tmp181;int _tmp182;struct Cyc_Absyn_Stmt*_tmp183;void*_tmp184;_LLD5: if(*((int*)
_tmp139)!= 15)goto _LLD7;_tmp13A=((struct Cyc_Absyn_Cast_e_struct*)_tmp139)->f2;
_tmp13B=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp139)->f4;if((int)_tmp13B != 2)
goto _LLD7;_LLD6: {union Cyc_CfFlowInfo_FlowInfo_union _tmp186;void*_tmp187;struct
_tuple5 _tmp185=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp13A);_tmp186=_tmp185.f1;
_tmp187=_tmp185.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp189;void*_tmp18A;
struct _tuple5 _tmp188=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp186,_tmp13A,
_tmp187);_tmp189=_tmp188.f1;_tmp18A=_tmp188.f2;return({struct _tuple5 _tmp18B;
_tmp18B.f1=_tmp189;_tmp18B.f2=_tmp187;_tmp18B;});}}_LLD7: if(*((int*)_tmp139)!= 
15)goto _LLD9;_tmp13C=((struct Cyc_Absyn_Cast_e_struct*)_tmp139)->f2;_LLD8: _tmp13D=
_tmp13C;goto _LLDA;_LLD9: if(*((int*)_tmp139)!= 13)goto _LLDB;_tmp13D=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp139)->f1;_LLDA: _tmp13E=_tmp13D;goto _LLDC;_LLDB: if(*((int*)_tmp139)!= 14)goto
_LLDD;_tmp13E=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp139)->f1;_LLDC: return
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp13E);_LLDD: if(*((int*)_tmp139)!= 0)
goto _LLDF;_tmp13F=((struct Cyc_Absyn_Const_e_struct*)_tmp139)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmp139)->f1).Null_c).tag != 6)goto _LLDF;_LLDE: goto
_LLE0;_LLDF: if(*((int*)_tmp139)!= 0)goto _LLE1;_tmp140=((struct Cyc_Absyn_Const_e_struct*)
_tmp139)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp139)->f1).Int_c).tag != 2)
goto _LLE1;_tmp141=(_tmp140.Int_c).f2;if(_tmp141 != 0)goto _LLE1;_LLE0: return({
struct _tuple5 _tmp18C;_tmp18C.f1=inflow;_tmp18C.f2=(void*)0;_tmp18C;});_LLE1: if(*((
int*)_tmp139)!= 0)goto _LLE3;_tmp142=((struct Cyc_Absyn_Const_e_struct*)_tmp139)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp139)->f1).Int_c).tag != 2)goto _LLE3;
_LLE2: goto _LLE4;_LLE3: if(*((int*)_tmp139)!= 1)goto _LLE5;_tmp143=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp139)->f2;if(_tmp143 <= (void*)1)goto _LLE5;if(*((int*)
_tmp143)!= 1)goto _LLE5;_LLE4: return({struct _tuple5 _tmp18D;_tmp18D.f1=inflow;
_tmp18D.f2=(void*)1;_tmp18D;});_LLE5: if(*((int*)_tmp139)!= 32)goto _LLE7;_tmp144=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp139)->f1;if(_tmp144 != 0)goto _LLE7;_LLE6:
goto _LLE8;_LLE7: if(*((int*)_tmp139)!= 0)goto _LLE9;_LLE8: goto _LLEA;_LLE9: if(*((
int*)_tmp139)!= 19)goto _LLEB;_LLEA: goto _LLEC;_LLEB: if(*((int*)_tmp139)!= 18)goto
_LLED;_LLEC: goto _LLEE;_LLED: if(*((int*)_tmp139)!= 20)goto _LLEF;_LLEE: goto _LLF0;
_LLEF: if(*((int*)_tmp139)!= 21)goto _LLF1;_LLF0: goto _LLF2;_LLF1: if(*((int*)
_tmp139)!= 34)goto _LLF3;_LLF2: goto _LLF4;_LLF3: if(*((int*)_tmp139)!= 33)goto _LLF5;
_LLF4: return({struct _tuple5 _tmp18E;_tmp18E.f1=inflow;_tmp18E.f2=(void*)_tmp132->unknown_all;
_tmp18E;});_LLF5: if(*((int*)_tmp139)!= 1)goto _LLF7;_tmp145=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp139)->f2;if(_tmp145 <= (void*)1)goto _LLF7;if(*((int*)_tmp145)!= 0)goto _LLF7;
_LLF6: return({struct _tuple5 _tmp18F;_tmp18F.f1=inflow;_tmp18F.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp132,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp132->unknown_all);
_tmp18F;});_LLF7: if(*((int*)_tmp139)!= 1)goto _LLF9;_tmp146=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp139)->f2;if(_tmp146 <= (void*)1)goto _LLF9;if(*((int*)_tmp146)!= 2)goto _LLF9;
_tmp147=((struct Cyc_Absyn_Param_b_struct*)_tmp146)->f1;_LLF8: _tmp149=_tmp147;
goto _LLFA;_LLF9: if(*((int*)_tmp139)!= 1)goto _LLFB;_tmp148=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp139)->f2;if(_tmp148 <= (void*)1)goto _LLFB;if(*((int*)_tmp148)!= 3)goto _LLFB;
_tmp149=((struct Cyc_Absyn_Local_b_struct*)_tmp148)->f1;_LLFA: _tmp14B=_tmp149;
goto _LLFC;_LLFB: if(*((int*)_tmp139)!= 1)goto _LLFD;_tmp14A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp139)->f2;if(_tmp14A <= (void*)1)goto _LLFD;if(*((int*)_tmp14A)!= 4)goto _LLFD;
_tmp14B=((struct Cyc_Absyn_Pat_b_struct*)_tmp14A)->f1;_LLFC: inflow=Cyc_NewControlFlow_may_consume_exp(
env,inflow,e);return({struct _tuple5 _tmp190;_tmp190.f1=inflow;_tmp190.f2=Cyc_Dict_lookup(
d,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp191=_region_malloc(env->r,
sizeof(*_tmp191));_tmp191[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp192;
_tmp192.tag=0;_tmp192.f1=_tmp14B;_tmp192;});_tmp191;}));_tmp190;});_LLFD: if(*((
int*)_tmp139)!= 3)goto _LLFF;_tmp14C=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp139)->f1;_tmp14D=((struct Cyc_Absyn_Primop_e_struct*)_tmp139)->f2;_LLFE: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp194;void*_tmp195;struct _tuple5 _tmp193=Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmp14D,0);_tmp194=_tmp193.f1;_tmp195=_tmp193.f2;{void*_tmp196=
_tmp14C;_LL13E: if((int)_tmp196 != 0)goto _LL140;_LL13F: goto _LL141;_LL140: if((int)
_tmp196 != 2)goto _LL142;_LL141: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp14D))->hd)->loc,_tmp194);goto _LL13D;_LL142:;
_LL143: _tmp194=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp14D))->hd)->loc,_tmp194);goto _LL13D;_LL13D:;}
return({struct _tuple5 _tmp197;_tmp197.f1=_tmp194;_tmp197.f2=_tmp195;_tmp197;});}
_LLFF: if(*((int*)_tmp139)!= 5)goto _LL101;_tmp14E=((struct Cyc_Absyn_Increment_e_struct*)
_tmp139)->f1;_LL100: {struct Cyc_List_List _tmp198=({struct Cyc_List_List _tmp1A7;
_tmp1A7.hd=_tmp14E;_tmp1A7.tl=0;_tmp1A7;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp19A;struct _tuple5 _tmp199=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct
Cyc_List_List*)& _tmp198,0);_tmp19A=_tmp199.f1;Cyc_CfFlowInfo_check_unique_rvals(
_tmp14E->loc,_tmp19A);{union Cyc_CfFlowInfo_AbsLVal_union _tmp19C;struct _tuple7
_tmp19B=Cyc_NewControlFlow_anal_Lexp(env,_tmp19A,_tmp14E);_tmp19C=_tmp19B.f2;{
struct _tuple12 _tmp19E=({struct _tuple12 _tmp19D;_tmp19D.f1=_tmp19C;_tmp19D.f2=
_tmp19A;_tmp19D;});union Cyc_CfFlowInfo_AbsLVal_union _tmp19F;struct Cyc_CfFlowInfo_Place*
_tmp1A0;union Cyc_CfFlowInfo_FlowInfo_union _tmp1A1;struct Cyc_Dict_Dict _tmp1A2;
struct Cyc_List_List*_tmp1A3;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1A4;_LL145:
_tmp19F=_tmp19E.f1;if(((_tmp19E.f1).PlaceL).tag != 0)goto _LL147;_tmp1A0=(_tmp19F.PlaceL).f1;
_tmp1A1=_tmp19E.f2;if(((_tmp19E.f2).ReachableFL).tag != 1)goto _LL147;_tmp1A2=(
_tmp1A1.ReachableFL).f1;_tmp1A3=(_tmp1A1.ReachableFL).f2;_tmp1A4=(_tmp1A1.ReachableFL).f3;
_LL146: _tmp1A3=Cyc_CfFlowInfo_reln_kill_exp(_tmp132->r,_tmp1A3,_tmp14E);_tmp19A=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp1A5;(
_tmp1A5.ReachableFL).tag=1;(_tmp1A5.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
_tmp132,_tmp14E->loc,_tmp1A2,env->all_changed,_tmp1A0,(void*)_tmp132->unknown_all);(
_tmp1A5.ReachableFL).f2=_tmp1A3;(_tmp1A5.ReachableFL).f3=_tmp1A4;_tmp1A5;});goto
_LL144;_LL147:;_LL148: goto _LL144;_LL144:;}return({struct _tuple5 _tmp1A6;_tmp1A6.f1=
_tmp19A;_tmp1A6.f2=(void*)_tmp132->unknown_all;_tmp1A6;});}}_LL101: if(*((int*)
_tmp139)!= 4)goto _LL103;_tmp14F=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp139)->f1;
_tmp150=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp139)->f2;if(_tmp150 == 0)goto
_LL103;_tmp151=*_tmp150;_tmp152=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp139)->f3;
_LL102: {struct Cyc_List_List _tmp1A8=({struct Cyc_List_List _tmp1B7;_tmp1B7.hd=
_tmp152;_tmp1B7.tl=0;_tmp1B7;});struct Cyc_List_List _tmp1A9=({struct Cyc_List_List
_tmp1B6;_tmp1B6.hd=_tmp14F;_tmp1B6.tl=(struct Cyc_List_List*)& _tmp1A8;_tmp1B6;});
union Cyc_CfFlowInfo_FlowInfo_union _tmp1AB;struct _tuple5 _tmp1AA=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp1A9,1);_tmp1AB=_tmp1AA.f1;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp1AD;struct _tuple7 _tmp1AC=Cyc_NewControlFlow_anal_Lexp(env,_tmp1AB,_tmp14F);
_tmp1AD=_tmp1AC.f2;_tmp1AB=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp1AB);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp1AE=_tmp1AB;struct Cyc_Dict_Dict _tmp1AF;
struct Cyc_List_List*_tmp1B0;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1B1;_LL14A: if((
_tmp1AE.ReachableFL).tag != 1)goto _LL14C;_tmp1AF=(_tmp1AE.ReachableFL).f1;_tmp1B0=(
_tmp1AE.ReachableFL).f2;_tmp1B1=(_tmp1AE.ReachableFL).f3;_LL14B:{union Cyc_CfFlowInfo_AbsLVal_union
_tmp1B2=_tmp1AD;struct Cyc_CfFlowInfo_Place*_tmp1B3;_LL14F: if((_tmp1B2.PlaceL).tag
!= 0)goto _LL151;_tmp1B3=(_tmp1B2.PlaceL).f1;_LL150: _tmp1B1=Cyc_NewControlFlow_consume_assignment(
env,_tmp1B3,_tmp1B1,_tmp1AF,_tmp14F);_tmp1B0=Cyc_CfFlowInfo_reln_kill_exp(
_tmp132->r,_tmp1B0,_tmp14F);_tmp1AF=Cyc_CfFlowInfo_assign_place(_tmp132,_tmp14F->loc,
_tmp1AF,env->all_changed,_tmp1B3,(void*)_tmp132->unknown_all);_tmp1AB=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1B4;(_tmp1B4.ReachableFL).tag=1;(_tmp1B4.ReachableFL).f1=
_tmp1AF;(_tmp1B4.ReachableFL).f2=_tmp1B0;(_tmp1B4.ReachableFL).f3=_tmp1B1;
_tmp1B4;});goto _LL14E;_LL151: if((_tmp1B2.UnknownL).tag != 1)goto _LL14E;_LL152:
goto _LL14E;_LL14E:;}goto _LL149;_LL14C:;_LL14D: goto _LL149;_LL149:;}return({struct
_tuple5 _tmp1B5;_tmp1B5.f1=_tmp1AB;_tmp1B5.f2=(void*)_tmp132->unknown_all;_tmp1B5;});}}
_LL103: if(*((int*)_tmp139)!= 4)goto _LL105;_tmp153=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp139)->f1;_tmp154=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp139)->f2;if(
_tmp154 != 0)goto _LL105;_tmp155=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp139)->f3;
_LL104: {struct Cyc_Dict_Dict*_tmp1B8=env->all_changed;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1BA;union Cyc_CfFlowInfo_FlowInfo_union _tmp1BB;struct _tuple6 _tmp1B9=Cyc_CfFlowInfo_save_consume_info(
_tmp132,inflow,1);_tmp1BA=_tmp1B9.f1;_tmp1BB=_tmp1B9.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
empty_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp1D6;_tmp1D6.consumed=_tmp132->mt_place_set;
_tmp1D6.may_consume=0;_tmp1D6;});struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=
empty_consume;int init_consume=0;while(1){env->all_changed=({struct Cyc_Dict_Dict*
_tmp1BC=_region_malloc(env->r,sizeof(*_tmp1BC));_tmp1BC[0]=_tmp132->mt_place_set;
_tmp1BC;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp1BE;union Cyc_CfFlowInfo_AbsLVal_union
_tmp1BF;struct _tuple7 _tmp1BD=Cyc_NewControlFlow_anal_Lexp(env,_tmp1BB,_tmp153);
_tmp1BE=_tmp1BD.f1;_tmp1BF=_tmp1BD.f2;{struct Cyc_Dict_Dict _tmp1C0=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));env->all_changed=({struct Cyc_Dict_Dict*_tmp1C1=
_region_malloc(env->r,sizeof(*_tmp1C1));_tmp1C1[0]=_tmp132->mt_place_set;_tmp1C1;});{
union Cyc_CfFlowInfo_FlowInfo_union _tmp1C3;void*_tmp1C4;struct _tuple5 _tmp1C2=Cyc_NewControlFlow_anal_Rexp(
env,_tmp1BB,_tmp155);_tmp1C3=_tmp1C2.f1;_tmp1C4=_tmp1C2.f2;{struct Cyc_Dict_Dict
_tmp1C5=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo_union
_tmp1C6=Cyc_CfFlowInfo_after_flow(_tmp132,(struct Cyc_Dict_Dict*)& _tmp1C0,_tmp1BE,
_tmp1C3,_tmp1C0,_tmp1C5);union Cyc_CfFlowInfo_FlowInfo_union _tmp1C7=Cyc_CfFlowInfo_join_flow(
_tmp132,_tmp1B8,_tmp1BB,_tmp1C6,1);struct Cyc_CfFlowInfo_ConsumeInfo _tmp1C9;
struct _tuple6 _tmp1C8=Cyc_CfFlowInfo_save_consume_info(_tmp132,_tmp1C7,0);_tmp1C9=
_tmp1C8.f1;if(init_consume){if(!Cyc_CfFlowInfo_consume_approx(outflow_consume,
_tmp1C9)){({void*_tmp1CA=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1CB="sanity consumed: ";
_tag_dyneither(_tmp1CB,sizeof(char),18);}),_tag_dyneither(_tmp1CA,sizeof(void*),
0));});((void(*)(struct Cyc_Dict_Dict p,void(*pr)(struct Cyc_CfFlowInfo_Place*)))
Cyc_CfFlowInfo_print_dict_set)(outflow_consume.consumed,Cyc_CfFlowInfo_print_place);({
void*_tmp1CC=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1CD="\ncurrent consumed: ";
_tag_dyneither(_tmp1CD,sizeof(char),20);}),_tag_dyneither(_tmp1CC,sizeof(void*),
0));});((void(*)(struct Cyc_Dict_Dict p,void(*pr)(struct Cyc_CfFlowInfo_Place*)))
Cyc_CfFlowInfo_print_dict_set)(_tmp1C9.consumed,Cyc_CfFlowInfo_print_place);({
void*_tmp1CE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1CF="\nsanity may_consume: ";
_tag_dyneither(_tmp1CF,sizeof(char),22);}),_tag_dyneither(_tmp1CE,sizeof(void*),
0));});((void(*)(struct Cyc_List_List*p,void(*pr)(struct Cyc_CfFlowInfo_Place*)))
Cyc_CfFlowInfo_print_list)(outflow_consume.may_consume,Cyc_CfFlowInfo_print_place);({
void*_tmp1D0=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1D1="\ncurrent may_consume: ";
_tag_dyneither(_tmp1D1,sizeof(char),23);}),_tag_dyneither(_tmp1D0,sizeof(void*),
0));});((void(*)(struct Cyc_List_List*p,void(*pr)(struct Cyc_CfFlowInfo_Place*)))
Cyc_CfFlowInfo_print_list)(_tmp1C9.may_consume,Cyc_CfFlowInfo_print_place);({
void*_tmp1D2=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1D3="\n";_tag_dyneither(
_tmp1D3,sizeof(char),2);}),_tag_dyneither(_tmp1D2,sizeof(void*),0));});({void*
_tmp1D4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1D5="assignOp_e failed to preserve consume info";_tag_dyneither(
_tmp1D5,sizeof(char),43);}),_tag_dyneither(_tmp1D4,sizeof(void*),0));});}_tmp1BB=
Cyc_CfFlowInfo_restore_consume_info(_tmp1BB,outflow_consume,0);}outflow_consume=
_tmp1C9;init_consume=1;if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1C7,_tmp1BB)){
if(_tmp1B8 == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=
Cyc_CfFlowInfo_union_place_set(*_tmp1B8,Cyc_CfFlowInfo_union_place_set(_tmp1C0,
_tmp1C5,0),0);}return Cyc_NewControlFlow_do_assign(_tmp132,env,_tmp1C6,_tmp153,
_tmp1BF,_tmp155,_tmp1C4,_tmp1BA,e->loc);}_tmp1BB=Cyc_CfFlowInfo_restore_consume_info(
_tmp1C7,empty_consume,0);}}}}}}}_LL105: if(*((int*)_tmp139)!= 9)goto _LL107;
_tmp156=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp139)->f1;_tmp157=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp139)->f2;_LL106: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1D8;void*_tmp1D9;
struct _tuple5 _tmp1D7=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp156);_tmp1D8=
_tmp1D7.f1;_tmp1D9=_tmp1D7.f2;_tmp1D8=Cyc_CfFlowInfo_drop_unique_rvals(_tmp156->loc,
_tmp1D8);return Cyc_NewControlFlow_anal_Rexp(env,_tmp1D8,_tmp157);}_LL107: if(*((
int*)_tmp139)!= 12)goto _LL109;_tmp158=((struct Cyc_Absyn_Throw_e_struct*)_tmp139)->f1;
_LL108: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1DB;void*_tmp1DC;struct _tuple5
_tmp1DA=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp158);_tmp1DB=_tmp1DA.f1;
_tmp1DC=_tmp1DA.f2;_tmp1DB=Cyc_CfFlowInfo_consume_unique_rvals(_tmp158->loc,
_tmp1DB);Cyc_NewControlFlow_use_Rval(env,_tmp158->loc,_tmp1DB,_tmp1DC);return({
struct _tuple5 _tmp1DD;_tmp1DD.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1DE;(_tmp1DE.BottomFL).tag=0;_tmp1DE;});_tmp1DD.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp132,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp132->unknown_all);
_tmp1DD;});}_LL109: if(*((int*)_tmp139)!= 11)goto _LL10B;_tmp159=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp139)->f1;_tmp15A=((struct Cyc_Absyn_FnCall_e_struct*)_tmp139)->f2;_LL10A: {
struct _RegionHandle _tmp1DF=_new_region("temp");struct _RegionHandle*temp=& _tmp1DF;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1E2;struct Cyc_List_List*
_tmp1E3;struct _tuple11 _tmp1E1=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_List_List*_tmp1E0=_region_malloc(temp,sizeof(*_tmp1E0));
_tmp1E0->hd=_tmp159;_tmp1E0->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp15A);_tmp1E0;}),1);_tmp1E2=
_tmp1E1.f1;_tmp1E3=_tmp1E1.f2;_tmp1E2=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp1E2);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1E4=Cyc_NewControlFlow_use_Rval(
env,_tmp159->loc,_tmp1E2,(void*)((struct Cyc_List_List*)_check_null(_tmp1E3))->hd);
_tmp1E3=_tmp1E3->tl;{struct Cyc_List_List*init_params=0;{void*_tmp1E5=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp159->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1E6;void*_tmp1E7;_LL154: if(_tmp1E5 <= (void*)4)goto _LL156;if(*((int*)_tmp1E5)
!= 4)goto _LL156;_tmp1E6=((struct Cyc_Absyn_PointerType_struct*)_tmp1E5)->f1;
_tmp1E7=(void*)_tmp1E6.elt_typ;_LL155:{void*_tmp1E8=Cyc_Tcutil_compress(_tmp1E7);
struct Cyc_Absyn_FnInfo _tmp1E9;struct Cyc_List_List*_tmp1EA;_LL159: if(_tmp1E8 <= (
void*)4)goto _LL15B;if(*((int*)_tmp1E8)!= 8)goto _LL15B;_tmp1E9=((struct Cyc_Absyn_FnType_struct*)
_tmp1E8)->f1;_tmp1EA=_tmp1E9.attributes;_LL15A: for(0;_tmp1EA != 0;_tmp1EA=_tmp1EA->tl){
void*_tmp1EB=(void*)_tmp1EA->hd;int _tmp1EC;_LL15E: if(_tmp1EB <= (void*)17)goto
_LL160;if(*((int*)_tmp1EB)!= 4)goto _LL160;_tmp1EC=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1EB)->f1;_LL15F: init_params=({struct Cyc_List_List*_tmp1ED=_region_malloc(
temp,sizeof(*_tmp1ED));_tmp1ED->hd=(void*)_tmp1EC;_tmp1ED->tl=init_params;
_tmp1ED;});goto _LL15D;_LL160:;_LL161: goto _LL15D;_LL15D:;}goto _LL158;_LL15B:;
_LL15C:({void*_tmp1EE=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1EF="anal_Rexp: bad function type";
_tag_dyneither(_tmp1EF,sizeof(char),29);}),_tag_dyneither(_tmp1EE,sizeof(void*),
0));});_LL158:;}goto _LL153;_LL156:;_LL157:({void*_tmp1F0=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F1="anal_Rexp: bad function type";
_tag_dyneither(_tmp1F1,sizeof(char),29);}),_tag_dyneither(_tmp1F0,sizeof(void*),
0));});_LL153:;}{int i=1;for(0;_tmp1E3 != 0;(((_tmp1E3=_tmp1E3->tl,_tmp15A=((
struct Cyc_List_List*)_check_null(_tmp15A))->tl)),++ i)){if(!((int(*)(struct Cyc_List_List*
l,int x))Cyc_List_memq)(init_params,i)){_tmp1E4=Cyc_NewControlFlow_use_Rval(env,((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp15A))->hd)->loc,
_tmp1E4,(void*)_tmp1E3->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp1F2=
_tmp1E2;struct Cyc_Dict_Dict _tmp1F3;_LL163: if((_tmp1F2.BottomFL).tag != 0)goto
_LL165;_LL164: goto _LL162;_LL165: if((_tmp1F2.ReachableFL).tag != 1)goto _LL162;
_tmp1F3=(_tmp1F2.ReachableFL).f1;_LL166: if(Cyc_CfFlowInfo_initlevel(_tmp1F3,(
void*)_tmp1E3->hd)== (void*)0)({void*_tmp1F4=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp15A))->hd)->loc,({const char*_tmp1F5="expression may not be initialized";
_tag_dyneither(_tmp1F5,sizeof(char),34);}),_tag_dyneither(_tmp1F4,sizeof(void*),
0));});{union Cyc_CfFlowInfo_FlowInfo_union _tmp1F6=_tmp1E4;struct Cyc_Dict_Dict
_tmp1F7;struct Cyc_List_List*_tmp1F8;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1F9;
_LL168: if((_tmp1F6.BottomFL).tag != 0)goto _LL16A;_LL169: goto _LL167;_LL16A: if((
_tmp1F6.ReachableFL).tag != 1)goto _LL167;_tmp1F7=(_tmp1F6.ReachableFL).f1;_tmp1F8=(
_tmp1F6.ReachableFL).f2;_tmp1F9=(_tmp1F6.ReachableFL).f3;_LL16B: {struct Cyc_Dict_Dict
_tmp1FA=Cyc_CfFlowInfo_escape_deref(_tmp132,_tmp1F7,env->all_changed,(void*)
_tmp1E3->hd);{void*_tmp1FB=(void*)_tmp1E3->hd;struct Cyc_CfFlowInfo_Place*_tmp1FC;
_LL16D: if(_tmp1FB <= (void*)3)goto _LL16F;if(*((int*)_tmp1FB)!= 2)goto _LL16F;
_tmp1FC=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1FB)->f1;_LL16E:{void*
_tmp1FD=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp15A))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1FE;void*_tmp1FF;_LL172: if(_tmp1FD <= (void*)4)goto _LL174;if(*((int*)_tmp1FD)
!= 4)goto _LL174;_tmp1FE=((struct Cyc_Absyn_PointerType_struct*)_tmp1FD)->f1;
_tmp1FF=(void*)_tmp1FE.elt_typ;_LL173: _tmp1FA=Cyc_CfFlowInfo_assign_place(
_tmp132,((struct Cyc_Absyn_Exp*)_tmp15A->hd)->loc,_tmp1FA,env->all_changed,
_tmp1FC,Cyc_CfFlowInfo_typ_to_absrval(_tmp132,_tmp1FF,(void*)_tmp132->esc_all));
goto _LL171;_LL174:;_LL175:({void*_tmp200=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp201="anal_Rexp:bad type for initialized arg";
_tag_dyneither(_tmp201,sizeof(char),39);}),_tag_dyneither(_tmp200,sizeof(void*),
0));});_LL171:;}goto _LL16C;_LL16F:;_LL170: goto _LL16C;_LL16C:;}_tmp1E4=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp202;(_tmp202.ReachableFL).tag=1;(_tmp202.ReachableFL).f1=
_tmp1FA;(_tmp202.ReachableFL).f2=_tmp1F8;(_tmp202.ReachableFL).f3=_tmp1F9;
_tmp202;});goto _LL167;}_LL167:;}goto _LL162;_LL162:;}}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp159->topt))->v)){struct _tuple5
_tmp205=({struct _tuple5 _tmp203;_tmp203.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp204;(_tmp204.BottomFL).tag=0;_tmp204;});
_tmp203.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp132,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,(void*)_tmp132->unknown_all);_tmp203;});_npop_handler(0);
return _tmp205;}else{struct _tuple5 _tmp207=({struct _tuple5 _tmp206;_tmp206.f1=
_tmp1E4;_tmp206.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp132,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,(void*)_tmp132->unknown_all);_tmp206;});_npop_handler(0);
return _tmp207;}}}};_pop_region(temp);}_LL10B: if(*((int*)_tmp139)!= 35)goto _LL10D;
_tmp15B=((struct Cyc_Absyn_Malloc_e_struct*)_tmp139)->f1;_tmp15C=_tmp15B.is_calloc;
_tmp15D=_tmp15B.rgn;_tmp15E=_tmp15B.elt_type;_tmp15F=_tmp15B.num_elts;_tmp160=
_tmp15B.fat_result;_LL10C: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp219=_region_malloc(_tmp132->r,sizeof(*_tmp219));_tmp219[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp21A;_tmp21A.tag=1;_tmp21A.f1=_tmp15F;_tmp21A.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp21A;});_tmp219;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp218=_region_malloc(_tmp132->r,sizeof(*_tmp218));
_tmp218->root=(void*)root;_tmp218->fields=0;_tmp218;});void*rval=(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp216=_region_malloc(_tmp132->r,sizeof(*
_tmp216));_tmp216[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp217;_tmp217.tag=
2;_tmp217.f1=place;_tmp217;});_tmp216;});void*place_val=_tmp160?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(
_tmp132,*((void**)_check_null(_tmp15E)),(void*)_tmp132->unknown_none);union Cyc_CfFlowInfo_FlowInfo_union
outflow;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct
Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,
place,0);if(_tmp15D != 0){struct _RegionHandle _tmp208=_new_region("temp");struct
_RegionHandle*temp=& _tmp208;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp20B;struct Cyc_List_List*_tmp20C;struct _tuple11 _tmp20A=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp209[2];_tmp209[1]=_tmp15F;_tmp209[0]=(
struct Cyc_Absyn_Exp*)_tmp15D;((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp209,sizeof(struct
Cyc_Absyn_Exp*),2));}),0);_tmp20B=_tmp20A.f1;_tmp20C=_tmp20A.f2;outflow=_tmp20B;};
_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo_union _tmp20E;struct _tuple5
_tmp20D=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp15F);_tmp20E=_tmp20D.f1;
outflow=_tmp20E;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp15F->loc,
outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp20F=outflow;struct Cyc_Dict_Dict
_tmp210;struct Cyc_List_List*_tmp211;struct Cyc_CfFlowInfo_ConsumeInfo _tmp212;
_LL177: if((_tmp20F.BottomFL).tag != 0)goto _LL179;_LL178: return({struct _tuple5
_tmp213;_tmp213.f1=outflow;_tmp213.f2=rval;_tmp213;});_LL179: if((_tmp20F.ReachableFL).tag
!= 1)goto _LL176;_tmp210=(_tmp20F.ReachableFL).f1;_tmp211=(_tmp20F.ReachableFL).f2;
_tmp212=(_tmp20F.ReachableFL).f3;_LL17A: return({struct _tuple5 _tmp214;_tmp214.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp215;(
_tmp215.ReachableFL).tag=1;(_tmp215.ReachableFL).f1=Cyc_Dict_insert(_tmp210,root,
place_val);(_tmp215.ReachableFL).f2=_tmp211;(_tmp215.ReachableFL).f3=_tmp212;
_tmp215;});_tmp214.f2=rval;_tmp214;});_LL176:;}}_LL10D: if(*((int*)_tmp139)!= 36)
goto _LL10F;_tmp161=((struct Cyc_Absyn_Swap_e_struct*)_tmp139)->f1;_tmp162=((
struct Cyc_Absyn_Swap_e_struct*)_tmp139)->f2;_LL10E: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo_union outflow;{struct _RegionHandle _tmp21B=
_new_region("temp");struct _RegionHandle*temp=& _tmp21B;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp21E;struct Cyc_List_List*_tmp21F;struct _tuple11
_tmp21D=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*
_tmp21C[2];_tmp21C[1]=_tmp162;_tmp21C[0]=_tmp161;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(
_tmp21C,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp21E=_tmp21D.f1;_tmp21F=
_tmp21D.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp21F))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp21F->tl))->hd;outflow=
_tmp21E;};_pop_region(temp);}Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
outflow);{union Cyc_CfFlowInfo_AbsLVal_union _tmp221;struct _tuple7 _tmp220=Cyc_NewControlFlow_anal_Lexp(
env,outflow,_tmp161);_tmp221=_tmp220.f2;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp223;struct _tuple7 _tmp222=Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmp162);
_tmp223=_tmp222.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp224=outflow;struct Cyc_Dict_Dict
_tmp225;struct Cyc_List_List*_tmp226;struct Cyc_CfFlowInfo_ConsumeInfo _tmp227;
_LL17C: if((_tmp224.ReachableFL).tag != 1)goto _LL17E;_tmp225=(_tmp224.ReachableFL).f1;
_tmp226=(_tmp224.ReachableFL).f2;_tmp227=(_tmp224.ReachableFL).f3;_LL17D:{union
Cyc_CfFlowInfo_AbsLVal_union _tmp228=_tmp221;struct Cyc_CfFlowInfo_Place*_tmp229;
_LL181: if((_tmp228.PlaceL).tag != 0)goto _LL183;_tmp229=(_tmp228.PlaceL).f1;_LL182:
_tmp225=Cyc_CfFlowInfo_assign_place(_tmp132,_tmp161->loc,_tmp225,env->all_changed,
_tmp229,right_rval);goto _LL180;_LL183: if((_tmp228.UnknownL).tag != 1)goto _LL180;
_LL184: goto _LL180;_LL180:;}{union Cyc_CfFlowInfo_AbsLVal_union _tmp22A=_tmp223;
struct Cyc_CfFlowInfo_Place*_tmp22B;_LL186: if((_tmp22A.PlaceL).tag != 0)goto _LL188;
_tmp22B=(_tmp22A.PlaceL).f1;_LL187: _tmp225=Cyc_CfFlowInfo_assign_place(_tmp132,
_tmp162->loc,_tmp225,env->all_changed,_tmp22B,left_rval);goto _LL185;_LL188: if((
_tmp22A.UnknownL).tag != 1)goto _LL185;_LL189: goto _LL185;_LL185:;}_tmp226=Cyc_CfFlowInfo_reln_kill_exp(
_tmp132->r,_tmp226,_tmp161);_tmp226=Cyc_CfFlowInfo_reln_kill_exp(_tmp132->r,
_tmp226,_tmp162);outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp22C;(_tmp22C.ReachableFL).tag=1;(_tmp22C.ReachableFL).f1=_tmp225;(_tmp22C.ReachableFL).f2=
_tmp226;(_tmp22C.ReachableFL).f3=_tmp227;_tmp22C;});goto _LL17B;_LL17E:;_LL17F:
goto _LL17B;_LL17B:;}return({struct _tuple5 _tmp22D;_tmp22D.f1=outflow;_tmp22D.f2=(
void*)_tmp132->unknown_all;_tmp22D;});}}}_LL10F: if(*((int*)_tmp139)!= 17)goto
_LL111;_tmp163=((struct Cyc_Absyn_New_e_struct*)_tmp139)->f1;_tmp164=((struct Cyc_Absyn_New_e_struct*)
_tmp139)->f2;_LL110: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp240=_region_malloc(_tmp132->r,sizeof(*_tmp240));_tmp240[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp241;_tmp241.tag=1;_tmp241.f1=_tmp164;_tmp241.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp241;});_tmp240;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp23F=_region_malloc(_tmp132->r,sizeof(*_tmp23F));
_tmp23F->root=(void*)root;_tmp23F->fields=0;_tmp23F;});void*rval=(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp23D=_region_malloc(_tmp132->r,sizeof(*
_tmp23D));_tmp23D[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp23E;_tmp23E.tag=
2;_tmp23E.f1=place;_tmp23E;});_tmp23D;});((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo_union outflow;void*
place_val;if(_tmp163 != 0){struct _RegionHandle _tmp22E=_new_region("temp");struct
_RegionHandle*temp=& _tmp22E;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp231;struct Cyc_List_List*_tmp232;struct _tuple11 _tmp230=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,({struct Cyc_Absyn_Exp*_tmp22F[2];_tmp22F[1]=_tmp164;_tmp22F[0]=(
struct Cyc_Absyn_Exp*)_tmp163;((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp22F,sizeof(struct
Cyc_Absyn_Exp*),2));}),0);_tmp231=_tmp230.f1;_tmp232=_tmp230.f2;outflow=_tmp231;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp232))->tl))->hd;};_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo_union
_tmp234;void*_tmp235;struct _tuple5 _tmp233=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp164);_tmp234=_tmp233.f1;_tmp235=_tmp233.f2;outflow=_tmp234;place_val=
_tmp235;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp164->loc,outflow);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp236=outflow;struct Cyc_Dict_Dict _tmp237;
struct Cyc_List_List*_tmp238;struct Cyc_CfFlowInfo_ConsumeInfo _tmp239;_LL18B: if((
_tmp236.BottomFL).tag != 0)goto _LL18D;_LL18C: return({struct _tuple5 _tmp23A;_tmp23A.f1=
outflow;_tmp23A.f2=rval;_tmp23A;});_LL18D: if((_tmp236.ReachableFL).tag != 1)goto
_LL18A;_tmp237=(_tmp236.ReachableFL).f1;_tmp238=(_tmp236.ReachableFL).f2;_tmp239=(
_tmp236.ReachableFL).f3;_LL18E: return({struct _tuple5 _tmp23B;_tmp23B.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp23C;(_tmp23C.ReachableFL).tag=1;(_tmp23C.ReachableFL).f1=
Cyc_Dict_insert(_tmp237,root,place_val);(_tmp23C.ReachableFL).f2=_tmp238;(
_tmp23C.ReachableFL).f3=_tmp239;_tmp23C;});_tmp23B.f2=rval;_tmp23B;});_LL18A:;}}}
_LL111: if(*((int*)_tmp139)!= 16)goto _LL113;_tmp165=((struct Cyc_Absyn_Address_e_struct*)
_tmp139)->f1;_LL112: {union Cyc_CfFlowInfo_FlowInfo_union _tmp243;struct _tuple5
_tmp242=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp165);_tmp243=_tmp242.f1;{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp245;struct _tuple6 _tmp244=Cyc_CfFlowInfo_save_consume_info(
env->fenv,_tmp243,0);_tmp245=_tmp244.f1;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp247;union Cyc_CfFlowInfo_AbsLVal_union _tmp248;struct _tuple7 _tmp246=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmp165);_tmp247=_tmp246.f1;_tmp248=_tmp246.f2;_tmp247=Cyc_CfFlowInfo_restore_consume_info(
_tmp247,_tmp245,0);{union Cyc_CfFlowInfo_AbsLVal_union _tmp249=_tmp248;struct Cyc_CfFlowInfo_Place*
_tmp24A;_LL190: if((_tmp249.UnknownL).tag != 1)goto _LL192;_LL191: return({struct
_tuple5 _tmp24B;_tmp24B.f1=_tmp247;_tmp24B.f2=(void*)1;_tmp24B;});_LL192: if((
_tmp249.PlaceL).tag != 0)goto _LL18F;_tmp24A=(_tmp249.PlaceL).f1;_LL193: return({
struct _tuple5 _tmp24C;_tmp24C.f1=_tmp247;_tmp24C.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp24D=_region_malloc(env->r,sizeof(*_tmp24D));_tmp24D[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp24E;_tmp24E.tag=2;_tmp24E.f1=_tmp24A;_tmp24E;});_tmp24D;});_tmp24C;});_LL18F:;}}}}
_LL113: if(*((int*)_tmp139)!= 22)goto _LL115;_tmp166=((struct Cyc_Absyn_Deref_e_struct*)
_tmp139)->f1;_LL114: {union Cyc_CfFlowInfo_FlowInfo_union _tmp250;void*_tmp251;
struct _tuple5 _tmp24F=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp166);_tmp250=
_tmp24F.f1;_tmp251=_tmp24F.f2;_tmp250=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp250);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp250,_tmp166,_tmp251);}
_LL115: if(*((int*)_tmp139)!= 23)goto _LL117;_tmp167=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp139)->f1;_tmp168=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp139)->f2;_LL116: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp253;void*_tmp254;struct _tuple5 _tmp252=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp167);_tmp253=_tmp252.f1;_tmp254=_tmp252.f2;_tmp253=Cyc_CfFlowInfo_drop_unique_rvals(
e->loc,_tmp253);_tmp253=Cyc_NewControlFlow_may_consume_exp(env,_tmp253,e);if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp167->topt))->v))return({struct
_tuple5 _tmp255;_tmp255.f1=_tmp253;_tmp255.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp132,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp132->unknown_all);
_tmp255;});{void*_tmp256=_tmp254;struct _dyneither_ptr _tmp257;_LL195: if(_tmp256 <= (
void*)3)goto _LL197;if(*((int*)_tmp256)!= 4)goto _LL197;_tmp257=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp256)->f1;_LL196: {int _tmp258=Cyc_CfFlowInfo_get_field_index((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp167->topt))->v,_tmp168);return({struct _tuple5
_tmp259;_tmp259.f1=_tmp253;_tmp259.f2=*((void**)_check_dyneither_subscript(
_tmp257,sizeof(void*),_tmp258));_tmp259;});}_LL197:;_LL198:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp25A=_cycalloc(sizeof(*_tmp25A));_tmp25A[0]=({
struct Cyc_Core_Impossible_struct _tmp25B;_tmp25B.tag=Cyc_Core_Impossible;_tmp25B.f1=(
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp25E;_tmp25E.tag=0;_tmp25E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{void*
_tmp25C[1]={& _tmp25E};Cyc_aprintf(({const char*_tmp25D="anal_Rexp: AggrMember: %s";
_tag_dyneither(_tmp25D,sizeof(char),26);}),_tag_dyneither(_tmp25C,sizeof(void*),
1));}});_tmp25B;});_tmp25A;}));_LL194:;}}_LL117: if(*((int*)_tmp139)!= 24)goto
_LL119;_tmp169=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp139)->f1;_tmp16A=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp139)->f2;_LL118: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp260;void*_tmp261;struct _tuple5 _tmp25F=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp169);_tmp260=_tmp25F.f1;_tmp261=_tmp25F.f2;_tmp260=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp260);{union Cyc_CfFlowInfo_FlowInfo_union _tmp263;void*_tmp264;struct
_tuple5 _tmp262=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp260,_tmp169,_tmp261);
_tmp263=_tmp262.f1;_tmp264=_tmp262.f2;{void*_tmp265=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp169->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp266;void*_tmp267;_LL19A: if(_tmp265 <= (void*)4)goto _LL19C;if(*((int*)_tmp265)
!= 4)goto _LL19C;_tmp266=((struct Cyc_Absyn_PointerType_struct*)_tmp265)->f1;
_tmp267=(void*)_tmp266.elt_typ;_LL19B: if(Cyc_Absyn_is_union_type(_tmp267))return({
struct _tuple5 _tmp268;_tmp268.f1=_tmp263;_tmp268.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp132,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp132->unknown_all);
_tmp268;});{void*_tmp269=_tmp264;struct _dyneither_ptr _tmp26A;_LL19F: if(_tmp269 <= (
void*)3)goto _LL1A1;if(*((int*)_tmp269)!= 4)goto _LL1A1;_tmp26A=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp269)->f1;_LL1A0: {int _tmp26B=Cyc_CfFlowInfo_get_field_index(_tmp267,_tmp16A);
return({struct _tuple5 _tmp26C;_tmp26C.f1=_tmp263;_tmp26C.f2=*((void**)
_check_dyneither_subscript(_tmp26A,sizeof(void*),_tmp26B));_tmp26C;});}_LL1A1:;
_LL1A2:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp26D=_cycalloc(
sizeof(*_tmp26D));_tmp26D[0]=({struct Cyc_Core_Impossible_struct _tmp26E;_tmp26E.tag=
Cyc_Core_Impossible;_tmp26E.f1=({const char*_tmp26F="anal_Rexp: AggrArrow";
_tag_dyneither(_tmp26F,sizeof(char),21);});_tmp26E;});_tmp26D;}));_LL19E:;}
_LL19C:;_LL19D:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp270=
_cycalloc(sizeof(*_tmp270));_tmp270[0]=({struct Cyc_Core_Impossible_struct _tmp271;
_tmp271.tag=Cyc_Core_Impossible;_tmp271.f1=({const char*_tmp272="anal_Rexp: AggrArrow ptr";
_tag_dyneither(_tmp272,sizeof(char),25);});_tmp271;});_tmp270;}));_LL199:;}}}
_LL119: if(*((int*)_tmp139)!= 6)goto _LL11B;_tmp16B=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp139)->f1;_tmp16C=((struct Cyc_Absyn_Conditional_e_struct*)_tmp139)->f2;
_tmp16D=((struct Cyc_Absyn_Conditional_e_struct*)_tmp139)->f3;_LL11A: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp274;union Cyc_CfFlowInfo_FlowInfo_union _tmp275;struct _tuple8 _tmp273=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp16B);_tmp274=_tmp273.f1;_tmp275=_tmp273.f2;_tmp274=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp16B->loc,_tmp274);_tmp275=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp16B->loc,
_tmp275);{struct _tuple5 _tmp276=Cyc_NewControlFlow_anal_Rexp(env,_tmp274,_tmp16C);
struct _tuple5 _tmp277=Cyc_NewControlFlow_anal_Rexp(env,_tmp275,_tmp16D);return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp132,env->all_changed,_tmp276,_tmp277,1);}}_LL11B: if(*((int*)_tmp139)!= 7)
goto _LL11D;_tmp16E=((struct Cyc_Absyn_And_e_struct*)_tmp139)->f1;_tmp16F=((struct
Cyc_Absyn_And_e_struct*)_tmp139)->f2;_LL11C: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp279;union Cyc_CfFlowInfo_FlowInfo_union _tmp27A;struct _tuple8 _tmp278=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp16E);_tmp279=_tmp278.f1;_tmp27A=_tmp278.f2;_tmp279=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp16E->loc,_tmp279);_tmp27A=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp16E->loc,
_tmp27A);{union Cyc_CfFlowInfo_FlowInfo_union _tmp27C;void*_tmp27D;struct _tuple5
_tmp27B=Cyc_NewControlFlow_anal_Rexp(env,_tmp279,_tmp16F);_tmp27C=_tmp27B.f1;
_tmp27D=_tmp27B.f2;_tmp27C=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp16F->loc,
_tmp27C);{struct _tuple5 _tmp27E=({struct _tuple5 _tmp281;_tmp281.f1=_tmp27C;_tmp281.f2=
_tmp27D;_tmp281;});struct _tuple5 _tmp27F=({struct _tuple5 _tmp280;_tmp280.f1=
_tmp27A;_tmp280.f2=(void*)((void*)0);_tmp280;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp132,env->all_changed,_tmp27E,_tmp27F,0);}}}_LL11D: if(*((int*)_tmp139)!= 8)
goto _LL11F;_tmp170=((struct Cyc_Absyn_Or_e_struct*)_tmp139)->f1;_tmp171=((struct
Cyc_Absyn_Or_e_struct*)_tmp139)->f2;_LL11E: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp283;union Cyc_CfFlowInfo_FlowInfo_union _tmp284;struct _tuple8 _tmp282=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp170);_tmp283=_tmp282.f1;_tmp284=_tmp282.f2;_tmp283=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp170->loc,_tmp283);_tmp284=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp170->loc,
_tmp284);{union Cyc_CfFlowInfo_FlowInfo_union _tmp286;void*_tmp287;struct _tuple5
_tmp285=Cyc_NewControlFlow_anal_Rexp(env,_tmp284,_tmp171);_tmp286=_tmp285.f1;
_tmp287=_tmp285.f2;_tmp286=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp171->loc,
_tmp286);{struct _tuple5 _tmp288=({struct _tuple5 _tmp28B;_tmp28B.f1=_tmp286;_tmp28B.f2=
_tmp287;_tmp28B;});struct _tuple5 _tmp289=({struct _tuple5 _tmp28A;_tmp28A.f1=
_tmp283;_tmp28A.f2=(void*)((void*)1);_tmp28A;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp132,env->all_changed,_tmp288,_tmp289,0);}}}_LL11F: if(*((int*)_tmp139)!= 25)
goto _LL121;_tmp172=((struct Cyc_Absyn_Subscript_e_struct*)_tmp139)->f1;_tmp173=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp139)->f2;_LL120: {struct _RegionHandle
_tmp28C=_new_region("temp");struct _RegionHandle*temp=& _tmp28C;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp28F;struct Cyc_List_List*_tmp290;struct
_tuple11 _tmp28E=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct
Cyc_Absyn_Exp*_tmp28D[2];_tmp28D[1]=_tmp173;_tmp28D[0]=_tmp172;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(
_tmp28D,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp28F=_tmp28E.f1;_tmp290=
_tmp28E.f2;_tmp28F=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp173->loc,_tmp28F);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp291=_tmp28F;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp292=_tmp28F;struct Cyc_Dict_Dict _tmp293;struct Cyc_List_List*_tmp294;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp295;_LL1A4: if((_tmp292.ReachableFL).tag != 1)goto _LL1A6;_tmp293=(_tmp292.ReachableFL).f1;
_tmp294=(_tmp292.ReachableFL).f2;_tmp295=(_tmp292.ReachableFL).f3;_LL1A5: if(Cyc_CfFlowInfo_initlevel(
_tmp293,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp290))->tl))->hd)== (void*)0)({void*_tmp296=0;Cyc_Tcutil_terr(
_tmp173->loc,({const char*_tmp297="expression may not be initialized";
_tag_dyneither(_tmp297,sizeof(char),34);}),_tag_dyneither(_tmp296,sizeof(void*),
0));});{struct Cyc_List_List*_tmp298=Cyc_NewControlFlow_add_subscript_reln(
_tmp132->r,_tmp294,_tmp172,_tmp173);if(_tmp294 != _tmp298)_tmp291=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp299;(_tmp299.ReachableFL).tag=1;(_tmp299.ReachableFL).f1=
_tmp293;(_tmp299.ReachableFL).f2=_tmp298;(_tmp299.ReachableFL).f3=_tmp295;
_tmp299;});goto _LL1A3;}_LL1A6:;_LL1A7: goto _LL1A3;_LL1A3:;}{void*_tmp29A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp172->topt))->v);struct Cyc_List_List*
_tmp29B;struct Cyc_Absyn_PtrInfo _tmp29C;struct Cyc_Absyn_PtrAtts _tmp29D;struct Cyc_Absyn_Conref*
_tmp29E;_LL1A9: if(_tmp29A <= (void*)4)goto _LL1AD;if(*((int*)_tmp29A)!= 9)goto
_LL1AB;_tmp29B=((struct Cyc_Absyn_TupleType_struct*)_tmp29A)->f1;_LL1AA: {void*
_tmp29F=(void*)((struct Cyc_List_List*)_check_null(_tmp290))->hd;struct
_dyneither_ptr _tmp2A0;_LL1B0: if(_tmp29F <= (void*)3)goto _LL1B2;if(*((int*)_tmp29F)
!= 4)goto _LL1B2;_tmp2A0=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp29F)->f1;
_LL1B1: _tmp291=Cyc_NewControlFlow_may_consume_exp(env,_tmp291,e);{unsigned int i=(
Cyc_Evexp_eval_const_uint_exp(_tmp173)).f1;struct _tuple5 _tmp2A2=({struct _tuple5
_tmp2A1;_tmp2A1.f1=_tmp291;_tmp2A1.f2=*((void**)_check_dyneither_subscript(
_tmp2A0,sizeof(void*),(int)i));_tmp2A1;});_npop_handler(0);return _tmp2A2;}_LL1B2:;
_LL1B3:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2A3=_cycalloc(
sizeof(*_tmp2A3));_tmp2A3[0]=({struct Cyc_Core_Impossible_struct _tmp2A4;_tmp2A4.tag=
Cyc_Core_Impossible;_tmp2A4.f1=({const char*_tmp2A5="anal_Rexp: Subscript";
_tag_dyneither(_tmp2A5,sizeof(char),21);});_tmp2A4;});_tmp2A3;}));_LL1AF:;}
_LL1AB: if(*((int*)_tmp29A)!= 4)goto _LL1AD;_tmp29C=((struct Cyc_Absyn_PointerType_struct*)
_tmp29A)->f1;_tmp29D=_tmp29C.ptr_atts;_tmp29E=_tmp29D.bounds;_LL1AC: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp2A7;void*_tmp2A8;struct _tuple5 _tmp2A6=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp28F,_tmp172,(void*)((struct Cyc_List_List*)_check_null(_tmp290))->hd);
_tmp2A7=_tmp2A6.f1;_tmp2A8=_tmp2A6.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp2A9=
_tmp2A7;_LL1B5: if((_tmp2A9.BottomFL).tag != 0)goto _LL1B7;_LL1B6: {struct _tuple5
_tmp2AB=({struct _tuple5 _tmp2AA;_tmp2AA.f1=_tmp2A7;_tmp2AA.f2=_tmp2A8;_tmp2AA;});
_npop_handler(0);return _tmp2AB;}_LL1B7:;_LL1B8: {struct _tuple5 _tmp2AD=({struct
_tuple5 _tmp2AC;_tmp2AC.f1=_tmp291;_tmp2AC.f2=_tmp2A8;_tmp2AC;});_npop_handler(0);
return _tmp2AD;}_LL1B4:;}}_LL1AD:;_LL1AE:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2AE=_cycalloc(sizeof(*_tmp2AE));_tmp2AE[0]=({struct Cyc_Core_Impossible_struct
_tmp2AF;_tmp2AF.tag=Cyc_Core_Impossible;_tmp2AF.f1=({const char*_tmp2B0="anal_Rexp: Subscript -- bad type";
_tag_dyneither(_tmp2B0,sizeof(char),33);});_tmp2AF;});_tmp2AE;}));_LL1A8:;}}};
_pop_region(temp);}_LL121: if(*((int*)_tmp139)!= 32)goto _LL123;_tmp174=((struct
Cyc_Absyn_Tunion_e_struct*)_tmp139)->f1;_tmp175=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp139)->f2;_LL122: if(_tmp175->is_flat){struct _RegionHandle _tmp2B1=_new_region("temp");
struct _RegionHandle*temp=& _tmp2B1;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2B3;struct Cyc_List_List*_tmp2B4;struct _tuple11 _tmp2B2=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp174,0);_tmp2B3=_tmp2B2.f1;_tmp2B4=_tmp2B2.f2;_tmp2B3=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2B3);for(0;(unsigned int)_tmp174;(_tmp174=_tmp174->tl,_tmp2B4=_tmp2B4->tl)){
_tmp2B3=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)_tmp174->hd)->loc,
_tmp2B3,(void*)((struct Cyc_List_List*)_check_null(_tmp2B4))->hd);}{struct _tuple5
_tmp2B6=({struct _tuple5 _tmp2B5;_tmp2B5.f1=_tmp2B3;_tmp2B5.f2=(void*)_tmp132->unknown_all;
_tmp2B5;});_npop_handler(0);return _tmp2B6;}};_pop_region(temp);}_tmp176=_tmp174;
goto _LL124;_LL123: if(*((int*)_tmp139)!= 26)goto _LL125;_tmp176=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp139)->f1;_LL124: {struct _RegionHandle _tmp2B7=_new_region("temp");struct
_RegionHandle*temp=& _tmp2B7;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2B9;struct Cyc_List_List*_tmp2BA;struct _tuple11 _tmp2B8=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp176,0);_tmp2B9=_tmp2B8.f1;_tmp2BA=_tmp2B8.f2;_tmp2B9=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2B9);{struct _dyneither_ptr aggrdict=({unsigned int _tmp2BF=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp176);void**
_tmp2C0=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp2BF));
struct _dyneither_ptr _tmp2C3=_tag_dyneither(_tmp2C0,sizeof(void*),_tmp2BF);{
unsigned int _tmp2C1=_tmp2BF;unsigned int i;for(i=0;i < _tmp2C1;i ++){_tmp2C0[i]=({
void*_tmp2C2=(void*)((struct Cyc_List_List*)_check_null(_tmp2BA))->hd;_tmp2BA=
_tmp2BA->tl;_tmp2C2;});}}_tmp2C3;});struct _tuple5 _tmp2BE=({struct _tuple5 _tmp2BB;
_tmp2BB.f1=_tmp2B9;_tmp2BB.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2BC=_region_malloc(env->r,sizeof(*_tmp2BC));_tmp2BC[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp2BD;_tmp2BD.tag=4;_tmp2BD.f1=aggrdict;_tmp2BD;});_tmp2BC;});_tmp2BB;});
_npop_handler(0);return _tmp2BE;}};_pop_region(temp);}_LL125: if(*((int*)_tmp139)
!= 31)goto _LL127;_tmp177=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp139)->f2;
_LL126: {struct Cyc_List_List*fs;{void*_tmp2C4=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_List_List*_tmp2C5;_LL1BA:
if(_tmp2C4 <= (void*)4)goto _LL1BC;if(*((int*)_tmp2C4)!= 11)goto _LL1BC;_tmp2C5=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp2C4)->f2;_LL1BB: fs=_tmp2C5;goto _LL1B9;
_LL1BC:;_LL1BD:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2C6=
_cycalloc(sizeof(*_tmp2C6));_tmp2C6[0]=({struct Cyc_Core_Impossible_struct _tmp2C7;
_tmp2C7.tag=Cyc_Core_Impossible;_tmp2C7.f1=({const char*_tmp2C8="anal_Rexp:anon struct has bad type";
_tag_dyneither(_tmp2C8,sizeof(char),35);});_tmp2C7;});_tmp2C6;}));_LL1B9:;}
_tmp178=_tmp177;_tmp17D=fs;goto _LL128;}_LL127: if(*((int*)_tmp139)!= 30)goto
_LL129;_tmp178=((struct Cyc_Absyn_Struct_e_struct*)_tmp139)->f3;_tmp179=((struct
Cyc_Absyn_Struct_e_struct*)_tmp139)->f4;if(_tmp179 == 0)goto _LL129;_tmp17A=*
_tmp179;_tmp17B=_tmp17A.impl;if(_tmp17B == 0)goto _LL129;_tmp17C=*_tmp17B;_tmp17D=
_tmp17C.fields;_LL128: {struct _RegionHandle _tmp2C9=_new_region("temp");struct
_RegionHandle*temp=& _tmp2C9;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2CB;struct Cyc_List_List*_tmp2CC;struct _tuple11 _tmp2CA=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple14*))Cyc_Core_snd,_tmp178),0);_tmp2CB=_tmp2CA.f1;_tmp2CC=_tmp2CA.f2;
_tmp2CB=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp2CB);{struct
_dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp132,_tmp17D,(
void*)_tmp132->unknown_all);{int i=0;for(0;_tmp2CC != 0;(((_tmp2CC=_tmp2CC->tl,
_tmp178=_tmp178->tl)),++ i)){struct Cyc_List_List*ds=(*((struct _tuple14*)((struct
Cyc_List_List*)_check_null(_tmp178))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*
_tmp2CD=(void*)ds->hd;struct _dyneither_ptr*_tmp2CE;_LL1BF: if(*((int*)_tmp2CD)!= 
0)goto _LL1C1;_LL1C0:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2CF=_cycalloc(sizeof(*_tmp2CF));_tmp2CF[0]=({struct Cyc_Core_Impossible_struct
_tmp2D0;_tmp2D0.tag=Cyc_Core_Impossible;_tmp2D0.f1=({const char*_tmp2D1="anal_Rexp:Struct_e";
_tag_dyneither(_tmp2D1,sizeof(char),19);});_tmp2D0;});_tmp2CF;}));_LL1C1: if(*((
int*)_tmp2CD)!= 1)goto _LL1BE;_tmp2CE=((struct Cyc_Absyn_FieldName_struct*)_tmp2CD)->f1;
_LL1C2: {int _tmp2D2=Cyc_CfFlowInfo_get_field_index_fs(_tmp17D,_tmp2CE);*((void**)
_check_dyneither_subscript(aggrdict,sizeof(void*),_tmp2D2))=(void*)_tmp2CC->hd;}
_LL1BE:;}}}{struct _tuple5 _tmp2D6=({struct _tuple5 _tmp2D3;_tmp2D3.f1=_tmp2CB;
_tmp2D3.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2D4=_region_malloc(
env->r,sizeof(*_tmp2D4));_tmp2D4[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp2D5;_tmp2D5.tag=4;_tmp2D5.f1=aggrdict;_tmp2D5;});_tmp2D4;});_tmp2D3;});
_npop_handler(0);return _tmp2D6;}}};_pop_region(temp);}_LL129: if(*((int*)_tmp139)
!= 30)goto _LL12B;_LL12A:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7[0]=({struct Cyc_Core_Impossible_struct
_tmp2D8;_tmp2D8.tag=Cyc_Core_Impossible;_tmp2D8.f1=({const char*_tmp2D9="anal_Rexp:missing aggrdeclimpl";
_tag_dyneither(_tmp2D9,sizeof(char),31);});_tmp2D8;});_tmp2D7;}));_LL12B: if(*((
int*)_tmp139)!= 28)goto _LL12D;_tmp17E=((struct Cyc_Absyn_Array_e_struct*)_tmp139)->f1;
_LL12C: {struct _RegionHandle _tmp2DA=_new_region("temp");struct _RegionHandle*temp=&
_tmp2DA;_push_region(temp);{struct Cyc_List_List*_tmp2DB=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple14*),struct Cyc_List_List*
x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple14*))Cyc_Core_snd,
_tmp17E);union Cyc_CfFlowInfo_FlowInfo_union _tmp2DD;struct Cyc_List_List*_tmp2DE;
struct _tuple11 _tmp2DC=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
_tmp2DB,0);_tmp2DD=_tmp2DC.f1;_tmp2DE=_tmp2DC.f2;_tmp2DD=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2DD);for(0;_tmp2DE != 0;(_tmp2DE=_tmp2DE->tl,_tmp2DB=_tmp2DB->tl)){
_tmp2DD=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2DB))->hd)->loc,_tmp2DD,(void*)_tmp2DE->hd);}{struct _tuple5
_tmp2E0=({struct _tuple5 _tmp2DF;_tmp2DF.f1=_tmp2DD;_tmp2DF.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp132,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp132->unknown_all);
_tmp2DF;});_npop_handler(0);return _tmp2E0;}};_pop_region(temp);}_LL12D: if(*((int*)
_tmp139)!= 29)goto _LL12F;_tmp17F=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp139)->f1;_tmp180=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp139)->f2;
_tmp181=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp139)->f3;_tmp182=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp139)->f4;_LL12E: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp2E2;void*_tmp2E3;struct _tuple5 _tmp2E1=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp180);_tmp2E2=_tmp2E1.f1;_tmp2E3=_tmp2E1.f2;_tmp2E2=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp180->loc,_tmp2E2);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2E4=_tmp2E2;struct
Cyc_Dict_Dict _tmp2E5;struct Cyc_List_List*_tmp2E6;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2E7;_LL1C4: if((_tmp2E4.BottomFL).tag != 0)goto _LL1C6;_LL1C5: return({struct
_tuple5 _tmp2E8;_tmp2E8.f1=_tmp2E2;_tmp2E8.f2=(void*)_tmp132->unknown_all;_tmp2E8;});
_LL1C6: if((_tmp2E4.ReachableFL).tag != 1)goto _LL1C3;_tmp2E5=(_tmp2E4.ReachableFL).f1;
_tmp2E6=(_tmp2E4.ReachableFL).f2;_tmp2E7=(_tmp2E4.ReachableFL).f3;_LL1C7: if(Cyc_CfFlowInfo_initlevel(
_tmp2E5,_tmp2E3)== (void*)0)({void*_tmp2E9=0;Cyc_Tcutil_terr(_tmp180->loc,({
const char*_tmp2EA="expression may not be initialized";_tag_dyneither(_tmp2EA,
sizeof(char),34);}),_tag_dyneither(_tmp2E9,sizeof(void*),0));});{struct Cyc_List_List*
new_relns=_tmp2E6;comp_loop: {void*_tmp2EB=(void*)_tmp180->r;struct Cyc_Absyn_Exp*
_tmp2EC;void*_tmp2ED;struct Cyc_Absyn_Vardecl*_tmp2EE;void*_tmp2EF;struct Cyc_Absyn_Vardecl*
_tmp2F0;void*_tmp2F1;struct Cyc_Absyn_Vardecl*_tmp2F2;void*_tmp2F3;struct Cyc_Absyn_Vardecl*
_tmp2F4;union Cyc_Absyn_Cnst_union _tmp2F5;int _tmp2F6;void*_tmp2F7;struct Cyc_List_List*
_tmp2F8;struct Cyc_List_List _tmp2F9;struct Cyc_Absyn_Exp*_tmp2FA;_LL1C9: if(*((int*)
_tmp2EB)!= 15)goto _LL1CB;_tmp2EC=((struct Cyc_Absyn_Cast_e_struct*)_tmp2EB)->f2;
_LL1CA: _tmp180=_tmp2EC;goto comp_loop;_LL1CB: if(*((int*)_tmp2EB)!= 1)goto _LL1CD;
_tmp2ED=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2EB)->f2;if(_tmp2ED <= (void*)
1)goto _LL1CD;if(*((int*)_tmp2ED)!= 0)goto _LL1CD;_tmp2EE=((struct Cyc_Absyn_Global_b_struct*)
_tmp2ED)->f1;if(!(!_tmp2EE->escapes))goto _LL1CD;_LL1CC: _tmp2F0=_tmp2EE;goto
_LL1CE;_LL1CD: if(*((int*)_tmp2EB)!= 1)goto _LL1CF;_tmp2EF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2EB)->f2;if(_tmp2EF <= (void*)1)goto _LL1CF;if(*((int*)_tmp2EF)!= 3)goto _LL1CF;
_tmp2F0=((struct Cyc_Absyn_Local_b_struct*)_tmp2EF)->f1;if(!(!_tmp2F0->escapes))
goto _LL1CF;_LL1CE: _tmp2F2=_tmp2F0;goto _LL1D0;_LL1CF: if(*((int*)_tmp2EB)!= 1)goto
_LL1D1;_tmp2F1=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2EB)->f2;if(_tmp2F1 <= (
void*)1)goto _LL1D1;if(*((int*)_tmp2F1)!= 4)goto _LL1D1;_tmp2F2=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2F1)->f1;if(!(!_tmp2F2->escapes))goto _LL1D1;_LL1D0: _tmp2F4=_tmp2F2;goto
_LL1D2;_LL1D1: if(*((int*)_tmp2EB)!= 1)goto _LL1D3;_tmp2F3=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2EB)->f2;if(_tmp2F3 <= (void*)1)goto _LL1D3;if(*((int*)_tmp2F3)!= 2)goto _LL1D3;
_tmp2F4=((struct Cyc_Absyn_Param_b_struct*)_tmp2F3)->f1;if(!(!_tmp2F4->escapes))
goto _LL1D3;_LL1D2: new_relns=({struct Cyc_List_List*_tmp2FB=_region_malloc(env->r,
sizeof(*_tmp2FB));_tmp2FB->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2FC=_region_malloc(
env->r,sizeof(*_tmp2FC));_tmp2FC->vd=_tmp17F;_tmp2FC->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp2FD;(_tmp2FD.LessVar).tag=1;(_tmp2FD.LessVar).f1=
_tmp2F4;(_tmp2FD.LessVar).f2=(void*)((void*)_tmp2F4->type);_tmp2FD;});_tmp2FC;});
_tmp2FB->tl=_tmp2E6;_tmp2FB;});goto _LL1C8;_LL1D3: if(*((int*)_tmp2EB)!= 0)goto
_LL1D5;_tmp2F5=((struct Cyc_Absyn_Const_e_struct*)_tmp2EB)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp2EB)->f1).Int_c).tag != 2)goto _LL1D5;_tmp2F6=(_tmp2F5.Int_c).f2;_LL1D4:
new_relns=({struct Cyc_List_List*_tmp2FE=_region_malloc(env->r,sizeof(*_tmp2FE));
_tmp2FE->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2FF=_region_malloc(env->r,sizeof(*
_tmp2FF));_tmp2FF->vd=_tmp17F;_tmp2FF->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp300;(_tmp300.LessConst).tag=3;(_tmp300.LessConst).f1=(
unsigned int)_tmp2F6;_tmp300;});_tmp2FF;});_tmp2FE->tl=_tmp2E6;_tmp2FE;});goto
_LL1C8;_LL1D5: if(*((int*)_tmp2EB)!= 3)goto _LL1D7;_tmp2F7=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp2EB)->f1;_tmp2F8=((struct Cyc_Absyn_Primop_e_struct*)_tmp2EB)->f2;if(_tmp2F8
== 0)goto _LL1D7;_tmp2F9=*_tmp2F8;_tmp2FA=(struct Cyc_Absyn_Exp*)_tmp2F9.hd;_LL1D6:{
void*_tmp301=(void*)_tmp2FA->r;void*_tmp302;struct Cyc_Absyn_Vardecl*_tmp303;void*
_tmp304;struct Cyc_Absyn_Vardecl*_tmp305;void*_tmp306;struct Cyc_Absyn_Vardecl*
_tmp307;void*_tmp308;struct Cyc_Absyn_Vardecl*_tmp309;_LL1DA: if(*((int*)_tmp301)
!= 1)goto _LL1DC;_tmp302=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp301)->f2;if(
_tmp302 <= (void*)1)goto _LL1DC;if(*((int*)_tmp302)!= 0)goto _LL1DC;_tmp303=((
struct Cyc_Absyn_Global_b_struct*)_tmp302)->f1;if(!(!_tmp303->escapes))goto _LL1DC;
_LL1DB: _tmp305=_tmp303;goto _LL1DD;_LL1DC: if(*((int*)_tmp301)!= 1)goto _LL1DE;
_tmp304=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp301)->f2;if(_tmp304 <= (void*)
1)goto _LL1DE;if(*((int*)_tmp304)!= 3)goto _LL1DE;_tmp305=((struct Cyc_Absyn_Local_b_struct*)
_tmp304)->f1;if(!(!_tmp305->escapes))goto _LL1DE;_LL1DD: _tmp307=_tmp305;goto
_LL1DF;_LL1DE: if(*((int*)_tmp301)!= 1)goto _LL1E0;_tmp306=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp301)->f2;if(_tmp306 <= (void*)1)goto _LL1E0;if(*((int*)_tmp306)!= 4)goto _LL1E0;
_tmp307=((struct Cyc_Absyn_Pat_b_struct*)_tmp306)->f1;if(!(!_tmp307->escapes))
goto _LL1E0;_LL1DF: _tmp309=_tmp307;goto _LL1E1;_LL1E0: if(*((int*)_tmp301)!= 1)goto
_LL1E2;_tmp308=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp301)->f2;if(_tmp308 <= (
void*)1)goto _LL1E2;if(*((int*)_tmp308)!= 2)goto _LL1E2;_tmp309=((struct Cyc_Absyn_Param_b_struct*)
_tmp308)->f1;if(!(!_tmp309->escapes))goto _LL1E2;_LL1E1: new_relns=({struct Cyc_List_List*
_tmp30A=_region_malloc(env->r,sizeof(*_tmp30A));_tmp30A->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp30B=_region_malloc(env->r,sizeof(*_tmp30B));_tmp30B->vd=_tmp17F;_tmp30B->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp30C;(
_tmp30C.LessNumelts).tag=2;(_tmp30C.LessNumelts).f1=_tmp309;_tmp30C;});_tmp30B;});
_tmp30A->tl=_tmp2E6;_tmp30A;});goto _LL1D9;_LL1E2:;_LL1E3: goto _LL1D9;_LL1D9:;}
goto _LL1C8;_LL1D7:;_LL1D8: goto _LL1C8;_LL1C8:;}if(_tmp2E6 != new_relns)_tmp2E2=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp30D;(
_tmp30D.ReachableFL).tag=1;(_tmp30D.ReachableFL).f1=_tmp2E5;(_tmp30D.ReachableFL).f2=
new_relns;(_tmp30D.ReachableFL).f3=_tmp2E7;_tmp30D;});{void*_tmp30E=_tmp2E3;
_LL1E5: if((int)_tmp30E != 0)goto _LL1E7;_LL1E6: return({struct _tuple5 _tmp30F;
_tmp30F.f1=_tmp2E2;_tmp30F.f2=(void*)_tmp132->unknown_all;_tmp30F;});_LL1E7: if((
int)_tmp30E != 2)goto _LL1E9;_LL1E8: goto _LL1EA;_LL1E9: if((int)_tmp30E != 1)goto
_LL1EB;_LL1EA: goto _LL1EC;_LL1EB: if(_tmp30E <= (void*)3)goto _LL1ED;if(*((int*)
_tmp30E)!= 2)goto _LL1ED;_LL1EC: {struct Cyc_List_List _tmp310=({struct Cyc_List_List
_tmp31C;_tmp31C.hd=_tmp17F;_tmp31C.tl=0;_tmp31C;});_tmp2E2=Cyc_NewControlFlow_add_vars(
_tmp132,_tmp2E2,(struct Cyc_List_List*)& _tmp310,(void*)_tmp132->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp312;void*_tmp313;struct _tuple5 _tmp311=Cyc_NewControlFlow_anal_Rexp(
env,_tmp2E2,_tmp181);_tmp312=_tmp311.f1;_tmp313=_tmp311.f2;_tmp312=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp181->loc,_tmp312);{union Cyc_CfFlowInfo_FlowInfo_union _tmp314=_tmp312;struct
Cyc_Dict_Dict _tmp315;struct Cyc_CfFlowInfo_ConsumeInfo _tmp316;_LL1F0: if((_tmp314.BottomFL).tag
!= 0)goto _LL1F2;_LL1F1: return({struct _tuple5 _tmp317;_tmp317.f1=_tmp312;_tmp317.f2=(
void*)_tmp132->unknown_all;_tmp317;});_LL1F2: if((_tmp314.ReachableFL).tag != 1)
goto _LL1EF;_tmp315=(_tmp314.ReachableFL).f1;_tmp316=(_tmp314.ReachableFL).f3;
_LL1F3: if(Cyc_CfFlowInfo_initlevel(_tmp315,_tmp313)!= (void*)2){({void*_tmp318=0;
Cyc_Tcutil_terr(_tmp180->loc,({const char*_tmp319="expression may not be initialized";
_tag_dyneither(_tmp319,sizeof(char),34);}),_tag_dyneither(_tmp318,sizeof(void*),
0));});return({struct _tuple5 _tmp31A;_tmp31A.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp31B;(_tmp31B.BottomFL).tag=0;_tmp31B;});
_tmp31A.f2=(void*)_tmp132->unknown_all;_tmp31A;});}_LL1EF:;}_tmp2E2=_tmp312;goto
_LL1EE;}}_LL1ED:;_LL1EE: while(1){struct Cyc_List_List _tmp31D=({struct Cyc_List_List
_tmp329;_tmp329.hd=_tmp17F;_tmp329.tl=0;_tmp329;});_tmp2E2=Cyc_NewControlFlow_add_vars(
_tmp132,_tmp2E2,(struct Cyc_List_List*)& _tmp31D,(void*)_tmp132->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp31F;void*_tmp320;struct _tuple5 _tmp31E=Cyc_NewControlFlow_anal_Rexp(
env,_tmp2E2,_tmp181);_tmp31F=_tmp31E.f1;_tmp320=_tmp31E.f2;_tmp31F=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp181->loc,_tmp31F);{union Cyc_CfFlowInfo_FlowInfo_union _tmp321=_tmp31F;struct
Cyc_Dict_Dict _tmp322;struct Cyc_CfFlowInfo_ConsumeInfo _tmp323;_LL1F5: if((_tmp321.BottomFL).tag
!= 0)goto _LL1F7;_LL1F6: goto _LL1F4;_LL1F7: if((_tmp321.ReachableFL).tag != 1)goto
_LL1F4;_tmp322=(_tmp321.ReachableFL).f1;_tmp323=(_tmp321.ReachableFL).f3;_LL1F8:
if(Cyc_CfFlowInfo_initlevel(_tmp322,_tmp320)!= (void*)2){({void*_tmp324=0;Cyc_Tcutil_terr(
_tmp180->loc,({const char*_tmp325="expression may not be initialized";
_tag_dyneither(_tmp325,sizeof(char),34);}),_tag_dyneither(_tmp324,sizeof(void*),
0));});return({struct _tuple5 _tmp326;_tmp326.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp327;(_tmp327.BottomFL).tag=0;_tmp327;});
_tmp326.f2=(void*)_tmp132->unknown_all;_tmp326;});}_LL1F4:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp328=Cyc_CfFlowInfo_join_flow(_tmp132,env->all_changed,_tmp2E2,_tmp31F,1);if(
Cyc_CfFlowInfo_flow_lessthan_approx(_tmp328,_tmp2E2))break;_tmp2E2=_tmp328;}}}
return({struct _tuple5 _tmp32A;_tmp32A.f1=_tmp2E2;_tmp32A.f2=(void*)_tmp132->unknown_all;
_tmp32A;});_LL1E4:;}}_LL1C3:;}}_LL12F: if(*((int*)_tmp139)!= 38)goto _LL131;
_tmp183=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp139)->f1;_LL130: while(1){union
Cyc_CfFlowInfo_FlowInfo_union*_tmp32C;struct _tuple9 _tmp32B=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,_tmp183);_tmp32C=_tmp32B.f2;inflow=*_tmp32C;{void*_tmp32D=(void*)
_tmp183->r;struct Cyc_Absyn_Stmt*_tmp32E;struct Cyc_Absyn_Stmt*_tmp32F;struct Cyc_Absyn_Decl*
_tmp330;struct Cyc_Absyn_Stmt*_tmp331;struct Cyc_Absyn_Exp*_tmp332;_LL1FA: if(
_tmp32D <= (void*)1)goto _LL200;if(*((int*)_tmp32D)!= 1)goto _LL1FC;_tmp32E=((
struct Cyc_Absyn_Seq_s_struct*)_tmp32D)->f1;_tmp32F=((struct Cyc_Absyn_Seq_s_struct*)
_tmp32D)->f2;_LL1FB: inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp32E);
_tmp183=_tmp32F;goto _LL1F9;_LL1FC: if(*((int*)_tmp32D)!= 11)goto _LL1FE;_tmp330=((
struct Cyc_Absyn_Decl_s_struct*)_tmp32D)->f1;_tmp331=((struct Cyc_Absyn_Decl_s_struct*)
_tmp32D)->f2;_LL1FD: inflow=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp330);
_tmp183=_tmp331;goto _LL1F9;_LL1FE: if(*((int*)_tmp32D)!= 0)goto _LL200;_tmp332=((
struct Cyc_Absyn_Exp_s_struct*)_tmp32D)->f1;_LL1FF: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp332);_LL200:;_LL201:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp333=_cycalloc(sizeof(*_tmp333));_tmp333[0]=({struct Cyc_Core_Impossible_struct
_tmp334;_tmp334.tag=Cyc_Core_Impossible;_tmp334.f1=({const char*_tmp335="analyze_Rexp: ill-formed StmtExp";
_tag_dyneither(_tmp335,sizeof(char),33);});_tmp334;});_tmp333;}));_LL1F9:;}}
_LL131: if(*((int*)_tmp139)!= 1)goto _LL133;_tmp184=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp139)->f2;if((int)_tmp184 != 0)goto _LL133;_LL132: goto _LL134;_LL133: if(*((int*)
_tmp139)!= 2)goto _LL135;_LL134: goto _LL136;_LL135: if(*((int*)_tmp139)!= 10)goto
_LL137;_LL136: goto _LL138;_LL137: if(*((int*)_tmp139)!= 37)goto _LL139;_LL138: goto
_LL13A;_LL139: if(*((int*)_tmp139)!= 27)goto _LL13B;_LL13A: goto _LL13C;_LL13B: if(*((
int*)_tmp139)!= 39)goto _LLD4;_LL13C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp336=_cycalloc(sizeof(*_tmp336));_tmp336[0]=({struct Cyc_Core_Impossible_struct
_tmp337;_tmp337.tag=Cyc_Core_Impossible;_tmp337.f1=({const char*_tmp338="anal_Rexp, unexpected exp form";
_tag_dyneither(_tmp338,sizeof(char),31);});_tmp337;});_tmp336;}));_LLD4:;}}
static struct _tuple7 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo_union
f,void*r,struct Cyc_List_List*flds){struct Cyc_CfFlowInfo_FlowEnv*_tmp339=env->fenv;
void*_tmp33A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp33B;void*_tmp33C;struct Cyc_Absyn_PtrAtts _tmp33D;
struct Cyc_Absyn_Conref*_tmp33E;struct Cyc_Absyn_Conref*_tmp33F;_LL203: if(_tmp33A
<= (void*)4)goto _LL205;if(*((int*)_tmp33A)!= 4)goto _LL205;_tmp33B=((struct Cyc_Absyn_PointerType_struct*)
_tmp33A)->f1;_tmp33C=(void*)_tmp33B.elt_typ;_tmp33D=_tmp33B.ptr_atts;_tmp33E=
_tmp33D.bounds;_tmp33F=_tmp33D.zero_term;_LL204: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp340=f;struct Cyc_Dict_Dict _tmp341;struct Cyc_List_List*_tmp342;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp343;_LL208: if((_tmp340.BottomFL).tag != 0)goto _LL20A;_LL209: return({struct
_tuple7 _tmp344;_tmp344.f1=f;_tmp344.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp345;(_tmp345.UnknownL).tag=1;_tmp345;});
_tmp344;});_LL20A: if((_tmp340.ReachableFL).tag != 1)goto _LL207;_tmp341=(_tmp340.ReachableFL).f1;
_tmp342=(_tmp340.ReachableFL).f2;_tmp343=(_tmp340.ReachableFL).f3;_LL20B: if(Cyc_Tcutil_is_bound_one(
_tmp33E)){void*_tmp346=r;struct Cyc_CfFlowInfo_Place*_tmp347;struct Cyc_CfFlowInfo_Place
_tmp348;void*_tmp349;struct Cyc_List_List*_tmp34A;void*_tmp34B;_LL20D: if((int)
_tmp346 != 1)goto _LL20F;_LL20E: goto _LL210;_LL20F: if((int)_tmp346 != 2)goto _LL211;
_LL210:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp34D;_tmp34D.tag=Cyc_CfFlowInfo_NotZero;_tmp34D.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp342);_tmp34D;});_tmp34C;})));goto _LL20C;_LL211: if(
_tmp346 <= (void*)3)goto _LL213;if(*((int*)_tmp346)!= 2)goto _LL213;_tmp347=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp346)->f1;_tmp348=*_tmp347;_tmp349=(
void*)_tmp348.root;_tmp34A=_tmp348.fields;_LL212:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_NotZero_struct*_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E[
0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp34F;_tmp34F.tag=Cyc_CfFlowInfo_NotZero;
_tmp34F.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp342);_tmp34F;});
_tmp34E;})));return({struct _tuple7 _tmp350;_tmp350.f1=f;_tmp350.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp351;(_tmp351.PlaceL).tag=0;(_tmp351.PlaceL).f1=({
struct Cyc_CfFlowInfo_Place*_tmp352=_region_malloc(_tmp339->r,sizeof(*_tmp352));
_tmp352->root=(void*)_tmp349;_tmp352->fields=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp339->r,_tmp34A,flds);_tmp352;});_tmp351;});_tmp350;});_LL213: if((int)_tmp346
!= 0)goto _LL215;_LL214:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));
return({struct _tuple7 _tmp353;_tmp353.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp355;(_tmp355.BottomFL).tag=0;_tmp355;});
_tmp353.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp354;(_tmp354.UnknownL).tag=1;_tmp354;});_tmp353;});_LL215: if(_tmp346 <= (void*)
3)goto _LL217;if(*((int*)_tmp346)!= 0)goto _LL217;_tmp34B=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp346)->f1;_LL216: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp34B);goto
_LL218;_LL217:;_LL218:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp356=_cycalloc(sizeof(*_tmp356));_tmp356[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp357;_tmp357.tag=Cyc_CfFlowInfo_UnknownZ;_tmp357.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp342);_tmp357;});_tmp356;})));_LL20C:;}else{(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp358=_cycalloc(sizeof(*
_tmp358));_tmp358[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp359;_tmp359.tag=
Cyc_CfFlowInfo_UnknownZ;_tmp359.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp342);_tmp359;});_tmp358;})));}if(Cyc_CfFlowInfo_initlevel(_tmp341,r)== (void*)
0)({void*_tmp35A=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp35B="dereference of possibly uninitialized pointer";
_tag_dyneither(_tmp35B,sizeof(char),46);}),_tag_dyneither(_tmp35A,sizeof(void*),
0));});return({struct _tuple7 _tmp35C;_tmp35C.f1=f;_tmp35C.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp35D;(_tmp35D.UnknownL).tag=1;_tmp35D;});
_tmp35C;});_LL207:;}_LL205:;_LL206:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp35E=_cycalloc(sizeof(*_tmp35E));_tmp35E[0]=({struct Cyc_Core_Impossible_struct
_tmp35F;_tmp35F.tag=Cyc_Core_Impossible;_tmp35F.f1=({const char*_tmp360="left deref of non-pointer-type";
_tag_dyneither(_tmp360,sizeof(char),31);});_tmp35F;});_tmp35E;}));_LL202:;}
static struct _tuple7 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*_tmp361=env->fenv;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp362=inflow;struct Cyc_Dict_Dict _tmp363;struct Cyc_List_List*
_tmp364;_LL21A: if((_tmp362.BottomFL).tag != 0)goto _LL21C;_LL21B: return({struct
_tuple7 _tmp365;_tmp365.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp367;(_tmp367.BottomFL).tag=0;_tmp367;});_tmp365.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp366;(_tmp366.UnknownL).tag=1;_tmp366;});
_tmp365;});_LL21C: if((_tmp362.ReachableFL).tag != 1)goto _LL219;_tmp363=(_tmp362.ReachableFL).f1;
_tmp364=(_tmp362.ReachableFL).f2;_LL21D: d=_tmp363;_LL219:;}{void*_tmp368=(void*)
e->r;struct Cyc_Absyn_Exp*_tmp369;struct Cyc_Absyn_Exp*_tmp36A;struct Cyc_Absyn_Exp*
_tmp36B;void*_tmp36C;struct Cyc_Absyn_Vardecl*_tmp36D;void*_tmp36E;struct Cyc_Absyn_Vardecl*
_tmp36F;void*_tmp370;struct Cyc_Absyn_Vardecl*_tmp371;void*_tmp372;struct Cyc_Absyn_Vardecl*
_tmp373;struct Cyc_Absyn_Exp*_tmp374;struct _dyneither_ptr*_tmp375;struct Cyc_Absyn_Exp*
_tmp376;struct Cyc_Absyn_Exp*_tmp377;struct Cyc_Absyn_Exp*_tmp378;struct Cyc_Absyn_Exp*
_tmp379;struct _dyneither_ptr*_tmp37A;_LL21F: if(*((int*)_tmp368)!= 15)goto _LL221;
_tmp369=((struct Cyc_Absyn_Cast_e_struct*)_tmp368)->f2;_LL220: _tmp36A=_tmp369;
goto _LL222;_LL221: if(*((int*)_tmp368)!= 13)goto _LL223;_tmp36A=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp368)->f1;_LL222: _tmp36B=_tmp36A;goto _LL224;_LL223: if(*((int*)_tmp368)!= 14)
goto _LL225;_tmp36B=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp368)->f1;_LL224:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp36B,flds);_LL225: if(*((int*)
_tmp368)!= 1)goto _LL227;_tmp36C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp368)->f2;
if(_tmp36C <= (void*)1)goto _LL227;if(*((int*)_tmp36C)!= 2)goto _LL227;_tmp36D=((
struct Cyc_Absyn_Param_b_struct*)_tmp36C)->f1;_LL226: _tmp36F=_tmp36D;goto _LL228;
_LL227: if(*((int*)_tmp368)!= 1)goto _LL229;_tmp36E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp368)->f2;if(_tmp36E <= (void*)1)goto _LL229;if(*((int*)_tmp36E)!= 3)goto _LL229;
_tmp36F=((struct Cyc_Absyn_Local_b_struct*)_tmp36E)->f1;_LL228: _tmp371=_tmp36F;
goto _LL22A;_LL229: if(*((int*)_tmp368)!= 1)goto _LL22B;_tmp370=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp368)->f2;if(_tmp370 <= (void*)1)goto _LL22B;if(*((int*)_tmp370)!= 4)goto _LL22B;
_tmp371=((struct Cyc_Absyn_Pat_b_struct*)_tmp370)->f1;_LL22A: return({struct
_tuple7 _tmp37B;_tmp37B.f1=inflow;_tmp37B.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp37C;(_tmp37C.PlaceL).tag=0;(_tmp37C.PlaceL).f1=({
struct Cyc_CfFlowInfo_Place*_tmp37D=_region_malloc(env->r,sizeof(*_tmp37D));
_tmp37D->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp37E=
_region_malloc(env->r,sizeof(*_tmp37E));_tmp37E[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp37F;_tmp37F.tag=0;_tmp37F.f1=_tmp371;_tmp37F;});_tmp37E;}));_tmp37D->fields=
flds;_tmp37D;});_tmp37C;});_tmp37B;});_LL22B: if(*((int*)_tmp368)!= 1)goto _LL22D;
_tmp372=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp368)->f2;if(_tmp372 <= (void*)
1)goto _LL22D;if(*((int*)_tmp372)!= 0)goto _LL22D;_tmp373=((struct Cyc_Absyn_Global_b_struct*)
_tmp372)->f1;_LL22C: return({struct _tuple7 _tmp380;_tmp380.f1=inflow;_tmp380.f2=(
union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union _tmp381;(
_tmp381.UnknownL).tag=1;_tmp381;});_tmp380;});_LL22D: if(*((int*)_tmp368)!= 24)
goto _LL22F;_tmp374=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp368)->f1;_tmp375=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp368)->f2;_LL22E:{void*_tmp382=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp374->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp383;void*_tmp384;_LL238: if(_tmp382 <= (void*)4)goto _LL23A;if(*((int*)_tmp382)
!= 4)goto _LL23A;_tmp383=((struct Cyc_Absyn_PointerType_struct*)_tmp382)->f1;
_tmp384=(void*)_tmp383.elt_typ;_LL239: if(!Cyc_Absyn_is_union_type(_tmp384))flds=({
struct Cyc_List_List*_tmp385=_region_malloc(env->r,sizeof(*_tmp385));_tmp385->hd=(
void*)Cyc_CfFlowInfo_get_field_index(_tmp384,_tmp375);_tmp385->tl=flds;_tmp385;});
goto _LL237;_LL23A:;_LL23B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp386=_cycalloc(sizeof(*_tmp386));_tmp386[0]=({struct Cyc_Core_Impossible_struct
_tmp387;_tmp387.tag=Cyc_Core_Impossible;_tmp387.f1=({const char*_tmp388="anal_Rexp: AggrArrow ptr";
_tag_dyneither(_tmp388,sizeof(char),25);});_tmp387;});_tmp386;}));_LL237:;}
_tmp376=_tmp374;goto _LL230;_LL22F: if(*((int*)_tmp368)!= 22)goto _LL231;_tmp376=((
struct Cyc_Absyn_Deref_e_struct*)_tmp368)->f1;_LL230: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp38A;void*_tmp38B;struct _tuple5 _tmp389=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp376);_tmp38A=_tmp389.f1;_tmp38B=_tmp389.f2;_tmp38A=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp38A);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp376,_tmp38A,
_tmp38B,flds);}_LL231: if(*((int*)_tmp368)!= 25)goto _LL233;_tmp377=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp368)->f1;_tmp378=((struct Cyc_Absyn_Subscript_e_struct*)_tmp368)->f2;_LL232: {
void*_tmp38C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp377->topt))->v);struct Cyc_Absyn_PtrInfo _tmp38D;struct Cyc_Absyn_PtrAtts
_tmp38E;struct Cyc_Absyn_Conref*_tmp38F;_LL23D: if(_tmp38C <= (void*)4)goto _LL241;
if(*((int*)_tmp38C)!= 9)goto _LL23F;_LL23E: {unsigned int _tmp390=(Cyc_Evexp_eval_const_uint_exp(
_tmp378)).f1;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp377,({struct
Cyc_List_List*_tmp391=_region_malloc(env->r,sizeof(*_tmp391));_tmp391->hd=(void*)((
int)_tmp390);_tmp391->tl=flds;_tmp391;}));}_LL23F: if(*((int*)_tmp38C)!= 4)goto
_LL241;_tmp38D=((struct Cyc_Absyn_PointerType_struct*)_tmp38C)->f1;_tmp38E=
_tmp38D.ptr_atts;_tmp38F=_tmp38E.bounds;_LL240: {struct _RegionHandle _tmp392=
_new_region("temp");struct _RegionHandle*temp=& _tmp392;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp395;struct Cyc_List_List*_tmp396;struct _tuple11
_tmp394=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*
_tmp393[2];_tmp393[1]=_tmp378;_tmp393[0]=_tmp377;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(
_tmp393,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp395=_tmp394.f1;_tmp396=
_tmp394.f2;_tmp395=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp378->loc,_tmp395);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp397=_tmp395;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp398=_tmp395;struct Cyc_Dict_Dict _tmp399;struct Cyc_List_List*_tmp39A;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp39B;_LL244: if((_tmp398.ReachableFL).tag != 1)goto _LL246;_tmp399=(_tmp398.ReachableFL).f1;
_tmp39A=(_tmp398.ReachableFL).f2;_tmp39B=(_tmp398.ReachableFL).f3;_LL245: if(Cyc_CfFlowInfo_initlevel(
_tmp399,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp396))->tl))->hd)== (void*)0)({void*_tmp39C=0;Cyc_Tcutil_terr(
_tmp378->loc,({const char*_tmp39D="expression may not be initialized";
_tag_dyneither(_tmp39D,sizeof(char),34);}),_tag_dyneither(_tmp39C,sizeof(void*),
0));});{struct Cyc_List_List*_tmp39E=Cyc_NewControlFlow_add_subscript_reln(
_tmp361->r,_tmp39A,_tmp377,_tmp378);if(_tmp39A != _tmp39E)_tmp397=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp39F;(_tmp39F.ReachableFL).tag=1;(_tmp39F.ReachableFL).f1=
_tmp399;(_tmp39F.ReachableFL).f2=_tmp39E;(_tmp39F.ReachableFL).f3=_tmp39B;
_tmp39F;});goto _LL243;}_LL246:;_LL247: goto _LL243;_LL243:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A1;union Cyc_CfFlowInfo_AbsLVal_union _tmp3A2;struct _tuple7 _tmp3A0=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp377,_tmp395,(void*)((struct Cyc_List_List*)_check_null(_tmp396))->hd,
flds);_tmp3A1=_tmp3A0.f1;_tmp3A2=_tmp3A0.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A3=_tmp3A1;_LL249: if((_tmp3A3.BottomFL).tag != 0)goto _LL24B;_LL24A: {struct
_tuple7 _tmp3A5=({struct _tuple7 _tmp3A4;_tmp3A4.f1=_tmp3A1;_tmp3A4.f2=_tmp3A2;
_tmp3A4;});_npop_handler(0);return _tmp3A5;}_LL24B:;_LL24C: {struct _tuple7 _tmp3A7=({
struct _tuple7 _tmp3A6;_tmp3A6.f1=_tmp397;_tmp3A6.f2=_tmp3A2;_tmp3A6;});
_npop_handler(0);return _tmp3A7;}_LL248:;}}}};_pop_region(temp);}_LL241:;_LL242:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp3A8=_cycalloc(sizeof(*
_tmp3A8));_tmp3A8[0]=({struct Cyc_Core_Impossible_struct _tmp3A9;_tmp3A9.tag=Cyc_Core_Impossible;
_tmp3A9.f1=({const char*_tmp3AA="anal_Lexp: Subscript -- bad type";_tag_dyneither(
_tmp3AA,sizeof(char),33);});_tmp3A9;});_tmp3A8;}));_LL23C:;}_LL233: if(*((int*)
_tmp368)!= 23)goto _LL235;_tmp379=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp368)->f1;
_tmp37A=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp368)->f2;_LL234: if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp379->topt))->v))return({struct
_tuple7 _tmp3AB;_tmp3AB.f1=inflow;_tmp3AB.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp3AC;(_tmp3AC.UnknownL).tag=1;_tmp3AC;});
_tmp3AB;});return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp379,({struct Cyc_List_List*
_tmp3AD=_region_malloc(env->r,sizeof(*_tmp3AD));_tmp3AD->hd=(void*)Cyc_CfFlowInfo_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp379->topt))->v,_tmp37A);_tmp3AD->tl=
flds;_tmp3AD;}));_LL235:;_LL236: return({struct _tuple7 _tmp3AE;_tmp3AE.f1=(union
Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp3B0;(
_tmp3B0.BottomFL).tag=0;_tmp3B0;});_tmp3AE.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp3AF;(_tmp3AF.UnknownL).tag=1;_tmp3AF;});
_tmp3AE;});_LL21E:;}}static struct _tuple7 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3B2;struct _tuple6 _tmp3B1=Cyc_CfFlowInfo_save_consume_info(env->fenv,inflow,0);
_tmp3B2=_tmp3B1.f1;{union Cyc_CfFlowInfo_FlowInfo_union _tmp3B4;union Cyc_CfFlowInfo_AbsLVal_union
_tmp3B5;struct _tuple7 _tmp3B3=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,e,0);
_tmp3B4=_tmp3B3.f1;_tmp3B5=_tmp3B3.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp3B7;
struct _tuple6 _tmp3B6=Cyc_CfFlowInfo_save_consume_info(env->fenv,inflow,0);
_tmp3B7=_tmp3B6.f1;if(_tmp3B2.may_consume != _tmp3B7.may_consume  || (_tmp3B2.consumed).t
!= (_tmp3B7.consumed).t)({void*_tmp3B8=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp3B9="oops: anal_Lexp modified consume lists";
_tag_dyneither(_tmp3B9,sizeof(char),39);}),_tag_dyneither(_tmp3B8,sizeof(void*),
0));});return({struct _tuple7 _tmp3BA;_tmp3BA.f1=_tmp3B4;_tmp3BA.f2=_tmp3B5;
_tmp3BA;});}}}static struct _tuple8 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp3BB=env->fenv;{void*_tmp3BC=(void*)e->r;struct Cyc_Absyn_Exp*_tmp3BD;struct
Cyc_Absyn_Exp*_tmp3BE;struct Cyc_Absyn_Exp*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3C0;
struct Cyc_Absyn_Exp*_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C2;struct Cyc_Absyn_Exp*
_tmp3C3;struct Cyc_Absyn_Exp*_tmp3C4;struct Cyc_Absyn_Exp*_tmp3C5;void*_tmp3C6;
struct Cyc_List_List*_tmp3C7;struct Cyc_List_List _tmp3C8;struct Cyc_Absyn_Exp*
_tmp3C9;struct Cyc_List_List*_tmp3CA;void*_tmp3CB;struct Cyc_List_List*_tmp3CC;
_LL24E: if(*((int*)_tmp3BC)!= 6)goto _LL250;_tmp3BD=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp3BC)->f1;_tmp3BE=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3BC)->f2;
_tmp3BF=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3BC)->f3;_LL24F: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3CE;union Cyc_CfFlowInfo_FlowInfo_union _tmp3CF;struct _tuple8 _tmp3CD=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3BD);_tmp3CE=_tmp3CD.f1;_tmp3CF=_tmp3CD.f2;_tmp3CE=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3BD->loc,_tmp3CE);_tmp3CF=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3BD->loc,
_tmp3CF);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3D1;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D2;struct _tuple8 _tmp3D0=Cyc_NewControlFlow_anal_test(env,_tmp3CE,_tmp3BE);
_tmp3D1=_tmp3D0.f1;_tmp3D2=_tmp3D0.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp3D4;
union Cyc_CfFlowInfo_FlowInfo_union _tmp3D5;struct _tuple8 _tmp3D3=Cyc_NewControlFlow_anal_test(
env,_tmp3CF,_tmp3BF);_tmp3D4=_tmp3D3.f1;_tmp3D5=_tmp3D3.f2;return({struct _tuple8
_tmp3D6;_tmp3D6.f1=Cyc_CfFlowInfo_join_flow(_tmp3BB,env->all_changed,_tmp3D1,
_tmp3D4,1);_tmp3D6.f2=Cyc_CfFlowInfo_join_flow(_tmp3BB,env->all_changed,_tmp3D2,
_tmp3D5,1);_tmp3D6;});}}}_LL250: if(*((int*)_tmp3BC)!= 7)goto _LL252;_tmp3C0=((
struct Cyc_Absyn_And_e_struct*)_tmp3BC)->f1;_tmp3C1=((struct Cyc_Absyn_And_e_struct*)
_tmp3BC)->f2;_LL251: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3D8;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D9;struct _tuple8 _tmp3D7=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3C0);
_tmp3D8=_tmp3D7.f1;_tmp3D9=_tmp3D7.f2;_tmp3D8=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3C0->loc,_tmp3D8);_tmp3D9=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3C0->loc,
_tmp3D9);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3DB;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3DC;struct _tuple8 _tmp3DA=Cyc_NewControlFlow_anal_test(env,_tmp3D8,_tmp3C1);
_tmp3DB=_tmp3DA.f1;_tmp3DC=_tmp3DA.f2;_tmp3DB=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3C1->loc,_tmp3DB);_tmp3DC=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3C1->loc,
_tmp3DC);return({struct _tuple8 _tmp3DD;_tmp3DD.f1=_tmp3DB;_tmp3DD.f2=Cyc_CfFlowInfo_join_flow(
_tmp3BB,env->all_changed,_tmp3D9,_tmp3DC,0);_tmp3DD;});}}_LL252: if(*((int*)
_tmp3BC)!= 8)goto _LL254;_tmp3C2=((struct Cyc_Absyn_Or_e_struct*)_tmp3BC)->f1;
_tmp3C3=((struct Cyc_Absyn_Or_e_struct*)_tmp3BC)->f2;_LL253: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3DF;union Cyc_CfFlowInfo_FlowInfo_union _tmp3E0;struct _tuple8 _tmp3DE=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3C2);_tmp3DF=_tmp3DE.f1;_tmp3E0=_tmp3DE.f2;_tmp3DF=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3C2->loc,_tmp3DF);_tmp3E0=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3C2->loc,
_tmp3E0);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3E2;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3E3;struct _tuple8 _tmp3E1=Cyc_NewControlFlow_anal_test(env,_tmp3E0,_tmp3C3);
_tmp3E2=_tmp3E1.f1;_tmp3E3=_tmp3E1.f2;_tmp3E2=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3C3->loc,_tmp3E2);_tmp3E3=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3C3->loc,
_tmp3E3);return({struct _tuple8 _tmp3E4;_tmp3E4.f1=Cyc_CfFlowInfo_join_flow(
_tmp3BB,env->all_changed,_tmp3DF,_tmp3E2,0);_tmp3E4.f2=_tmp3E3;_tmp3E4;});}}
_LL254: if(*((int*)_tmp3BC)!= 9)goto _LL256;_tmp3C4=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp3BC)->f1;_tmp3C5=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp3BC)->f2;_LL255: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp3E6;void*_tmp3E7;struct _tuple5 _tmp3E5=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp3C4);_tmp3E6=_tmp3E5.f1;_tmp3E7=_tmp3E5.f2;_tmp3E6=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp3C4->loc,_tmp3E6);return Cyc_NewControlFlow_anal_test(env,_tmp3E6,_tmp3C5);}
_LL256: if(*((int*)_tmp3BC)!= 3)goto _LL258;_tmp3C6=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3BC)->f1;if((int)_tmp3C6 != 11)goto _LL258;_tmp3C7=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3BC)->f2;if(_tmp3C7 == 0)goto _LL258;_tmp3C8=*_tmp3C7;_tmp3C9=(struct Cyc_Absyn_Exp*)
_tmp3C8.hd;_tmp3CA=_tmp3C8.tl;if(_tmp3CA != 0)goto _LL258;_LL257: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3E9;union Cyc_CfFlowInfo_FlowInfo_union _tmp3EA;struct _tuple8 _tmp3E8=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3C9);_tmp3E9=_tmp3E8.f1;_tmp3EA=_tmp3E8.f2;return({struct _tuple8
_tmp3EB;_tmp3EB.f1=_tmp3EA;_tmp3EB.f2=_tmp3E9;_tmp3EB;});}_LL258: if(*((int*)
_tmp3BC)!= 3)goto _LL25A;_tmp3CB=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3BC)->f1;_tmp3CC=((struct Cyc_Absyn_Primop_e_struct*)_tmp3BC)->f2;_LL259: {
void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo_union f;{struct _RegionHandle _tmp3EC=
_new_region("temp");struct _RegionHandle*temp=& _tmp3EC;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp3EE;struct Cyc_List_List*_tmp3EF;struct _tuple11
_tmp3ED=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp3CC,0);
_tmp3EE=_tmp3ED.f1;_tmp3EF=_tmp3ED.f2;r1=(void*)((struct Cyc_List_List*)
_check_null(_tmp3EF))->hd;r2=(void*)((struct Cyc_List_List*)_check_null(_tmp3EF->tl))->hd;
f=_tmp3EE;};_pop_region(temp);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp3F0=f;
struct Cyc_Dict_Dict _tmp3F1;struct Cyc_List_List*_tmp3F2;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3F3;_LL25D: if((_tmp3F0.BottomFL).tag != 0)goto _LL25F;_LL25E: return({struct
_tuple8 _tmp3F4;_tmp3F4.f1=f;_tmp3F4.f2=f;_tmp3F4;});_LL25F: if((_tmp3F0.ReachableFL).tag
!= 1)goto _LL25C;_tmp3F1=(_tmp3F0.ReachableFL).f1;_tmp3F2=(_tmp3F0.ReachableFL).f2;
_tmp3F3=(_tmp3F0.ReachableFL).f3;_LL260: {struct Cyc_Absyn_Exp*_tmp3F5=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3CC))->hd;struct Cyc_Absyn_Exp*_tmp3F6=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3CC->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
_tmp3F1,r1)== (void*)0)({void*_tmp3F7=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmp3CC->hd)->loc,({const char*_tmp3F8="expression may not be initialized";
_tag_dyneither(_tmp3F8,sizeof(char),34);}),_tag_dyneither(_tmp3F7,sizeof(void*),
0));});if(Cyc_CfFlowInfo_initlevel(_tmp3F1,r2)== (void*)0)({void*_tmp3F9=0;Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3CC->tl))->hd)->loc,({
const char*_tmp3FA="expression may not be initialized";_tag_dyneither(_tmp3FA,
sizeof(char),34);}),_tag_dyneither(_tmp3F9,sizeof(void*),0));});if(_tmp3CB == (
void*)5  || _tmp3CB == (void*)6){struct _tuple0 _tmp3FC=({struct _tuple0 _tmp3FB;
_tmp3FB.f1=r1;_tmp3FB.f2=r2;_tmp3FB;});void*_tmp3FD;void*_tmp3FE;void*_tmp3FF;
void*_tmp400;void*_tmp401;void*_tmp402;void*_tmp403;void*_tmp404;void*_tmp405;
void*_tmp406;void*_tmp407;void*_tmp408;void*_tmp409;void*_tmp40A;void*_tmp40B;
void*_tmp40C;void*_tmp40D;void*_tmp40E;void*_tmp40F;void*_tmp410;_LL262: _tmp3FD=
_tmp3FC.f1;if(_tmp3FD <= (void*)3)goto _LL264;if(*((int*)_tmp3FD)!= 0)goto _LL264;
_tmp3FE=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3FD)->f1;_tmp3FF=
_tmp3FC.f2;if((int)_tmp3FF != 0)goto _LL264;_LL263: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp412;union Cyc_CfFlowInfo_FlowInfo_union _tmp413;struct _tuple8 _tmp411=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp3F5,_tmp3FE);_tmp412=_tmp411.f1;_tmp413=_tmp411.f2;{void*_tmp414=
_tmp3CB;_LL277: if((int)_tmp414 != 5)goto _LL279;_LL278: return({struct _tuple8
_tmp415;_tmp415.f1=_tmp413;_tmp415.f2=_tmp412;_tmp415;});_LL279: if((int)_tmp414
!= 6)goto _LL27B;_LL27A: return({struct _tuple8 _tmp416;_tmp416.f1=_tmp412;_tmp416.f2=
_tmp413;_tmp416;});_LL27B:;_LL27C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417[0]=({struct Cyc_Core_Impossible_struct
_tmp418;_tmp418.tag=Cyc_Core_Impossible;_tmp418.f1=({const char*_tmp419="anal_test, zero-split";
_tag_dyneither(_tmp419,sizeof(char),22);});_tmp418;});_tmp417;}));_LL276:;}}
_LL264: _tmp400=_tmp3FC.f1;if((int)_tmp400 != 0)goto _LL266;_tmp401=_tmp3FC.f2;if(
_tmp401 <= (void*)3)goto _LL266;if(*((int*)_tmp401)!= 0)goto _LL266;_tmp402=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp401)->f1;_LL265: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp41B;union Cyc_CfFlowInfo_FlowInfo_union _tmp41C;struct _tuple8 _tmp41A=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp3F6,_tmp402);_tmp41B=_tmp41A.f1;_tmp41C=_tmp41A.f2;{void*_tmp41D=
_tmp3CB;_LL27E: if((int)_tmp41D != 5)goto _LL280;_LL27F: return({struct _tuple8
_tmp41E;_tmp41E.f1=_tmp41C;_tmp41E.f2=_tmp41B;_tmp41E;});_LL280: if((int)_tmp41D
!= 6)goto _LL282;_LL281: return({struct _tuple8 _tmp41F;_tmp41F.f1=_tmp41B;_tmp41F.f2=
_tmp41C;_tmp41F;});_LL282:;_LL283:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420[0]=({struct Cyc_Core_Impossible_struct
_tmp421;_tmp421.tag=Cyc_Core_Impossible;_tmp421.f1=({const char*_tmp422="anal_test, zero-split";
_tag_dyneither(_tmp422,sizeof(char),22);});_tmp421;});_tmp420;}));_LL27D:;}}
_LL266: _tmp403=_tmp3FC.f1;if((int)_tmp403 != 0)goto _LL268;_tmp404=_tmp3FC.f2;if((
int)_tmp404 != 0)goto _LL268;_LL267: if(_tmp3CB == (void*)5)return({struct _tuple8
_tmp423;_tmp423.f1=f;_tmp423.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp424;(_tmp424.BottomFL).tag=0;_tmp424;});_tmp423;});else{return({struct
_tuple8 _tmp425;_tmp425.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp426;(_tmp426.BottomFL).tag=0;_tmp426;});_tmp425.f2=f;_tmp425;});}_LL268:
_tmp405=_tmp3FC.f1;if((int)_tmp405 != 0)goto _LL26A;_tmp406=_tmp3FC.f2;if((int)
_tmp406 != 1)goto _LL26A;_LL269: goto _LL26B;_LL26A: _tmp407=_tmp3FC.f1;if((int)
_tmp407 != 0)goto _LL26C;_tmp408=_tmp3FC.f2;if((int)_tmp408 != 2)goto _LL26C;_LL26B:
goto _LL26D;_LL26C: _tmp409=_tmp3FC.f1;if((int)_tmp409 != 0)goto _LL26E;_tmp40A=
_tmp3FC.f2;if(_tmp40A <= (void*)3)goto _LL26E;if(*((int*)_tmp40A)!= 2)goto _LL26E;
_LL26D: goto _LL26F;_LL26E: _tmp40B=_tmp3FC.f1;if((int)_tmp40B != 1)goto _LL270;
_tmp40C=_tmp3FC.f2;if((int)_tmp40C != 0)goto _LL270;_LL26F: goto _LL271;_LL270:
_tmp40D=_tmp3FC.f1;if((int)_tmp40D != 2)goto _LL272;_tmp40E=_tmp3FC.f2;if((int)
_tmp40E != 0)goto _LL272;_LL271: goto _LL273;_LL272: _tmp40F=_tmp3FC.f1;if(_tmp40F <= (
void*)3)goto _LL274;if(*((int*)_tmp40F)!= 2)goto _LL274;_tmp410=_tmp3FC.f2;if((int)
_tmp410 != 0)goto _LL274;_LL273: if(_tmp3CB == (void*)6)return({struct _tuple8 _tmp427;
_tmp427.f1=f;_tmp427.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp428;(_tmp428.BottomFL).tag=0;_tmp428;});_tmp427;});else{return({struct
_tuple8 _tmp429;_tmp429.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp42A;(_tmp42A.BottomFL).tag=0;_tmp42A;});_tmp429.f2=f;_tmp429;});}_LL274:;
_LL275: goto _LL261;_LL261:;}{struct _tuple0 _tmp42C=({struct _tuple0 _tmp42B;_tmp42B.f1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F5->topt))->v);
_tmp42B.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F6->topt))->v);
_tmp42B;});void*_tmp42D;void*_tmp42E;void*_tmp42F;void*_tmp430;void*_tmp431;void*
_tmp432;_LL285: _tmp42D=_tmp42C.f1;if(_tmp42D <= (void*)4)goto _LL287;if(*((int*)
_tmp42D)!= 5)goto _LL287;_tmp42E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp42D)->f1;
if((int)_tmp42E != 1)goto _LL287;_LL286: goto _LL288;_LL287: _tmp42F=_tmp42C.f2;if(
_tmp42F <= (void*)4)goto _LL289;if(*((int*)_tmp42F)!= 5)goto _LL289;_tmp430=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp42F)->f1;if((int)_tmp430 != 1)goto _LL289;
_LL288: goto _LL28A;_LL289: _tmp431=_tmp42C.f1;if(_tmp431 <= (void*)4)goto _LL28B;if(*((
int*)_tmp431)!= 18)goto _LL28B;_LL28A: goto _LL28C;_LL28B: _tmp432=_tmp42C.f2;if(
_tmp432 <= (void*)4)goto _LL28D;if(*((int*)_tmp432)!= 18)goto _LL28D;_LL28C: goto
_LL284;_LL28D:;_LL28E: return({struct _tuple8 _tmp433;_tmp433.f1=f;_tmp433.f2=f;
_tmp433;});_LL284:;}{void*_tmp434=_tmp3CB;_LL290: if((int)_tmp434 != 5)goto _LL292;
_LL291: {union Cyc_CfFlowInfo_FlowInfo_union _tmp435=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3F5,r1,(void*)5,r2);_tmp435=Cyc_NewControlFlow_if_tagcmp(env,_tmp435,
_tmp3F6,r2,(void*)5,r1);return({struct _tuple8 _tmp436;_tmp436.f1=_tmp435;_tmp436.f2=
f;_tmp436;});}_LL292: if((int)_tmp434 != 6)goto _LL294;_LL293: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp437=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F5,r1,(void*)5,r2);_tmp437=Cyc_NewControlFlow_if_tagcmp(
env,_tmp437,_tmp3F6,r2,(void*)5,r1);return({struct _tuple8 _tmp438;_tmp438.f1=f;
_tmp438.f2=_tmp437;_tmp438;});}_LL294: if((int)_tmp434 != 7)goto _LL296;_LL295: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp439=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3F6,r2,(void*)8,r1);union Cyc_CfFlowInfo_FlowInfo_union _tmp43A=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3F5,r1,(void*)10,r2);return({struct _tuple8 _tmp43B;_tmp43B.f1=_tmp439;
_tmp43B.f2=_tmp43A;_tmp43B;});}_LL296: if((int)_tmp434 != 9)goto _LL298;_LL297: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp43C=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3F6,r2,(void*)10,r1);union Cyc_CfFlowInfo_FlowInfo_union _tmp43D=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3F5,r1,(void*)8,r2);return({struct _tuple8 _tmp43E;_tmp43E.f1=_tmp43C;
_tmp43E.f2=_tmp43D;_tmp43E;});}_LL298: if((int)_tmp434 != 8)goto _LL29A;_LL299: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp43F=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3F5,r1,(void*)8,r2);union Cyc_CfFlowInfo_FlowInfo_union _tmp440=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3F6,r2,(void*)10,r1);{union Cyc_CfFlowInfo_FlowInfo_union _tmp441=
_tmp43F;struct Cyc_Dict_Dict _tmp442;struct Cyc_CfFlowInfo_ConsumeInfo _tmp443;
_LL29F: if((_tmp441.BottomFL).tag != 0)goto _LL2A1;_LL2A0:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444[0]=({
struct Cyc_Core_Impossible_struct _tmp445;_tmp445.tag=Cyc_Core_Impossible;_tmp445.f1=({
const char*_tmp446="anal_test, Lt";_tag_dyneither(_tmp446,sizeof(char),14);});
_tmp445;});_tmp444;}));_LL2A1: if((_tmp441.ReachableFL).tag != 1)goto _LL29E;
_tmp442=(_tmp441.ReachableFL).f1;_tmp443=(_tmp441.ReachableFL).f3;_LL2A2: _tmp3F1=
_tmp442;_tmp3F3=_tmp443;_LL29E:;}{void*_tmp447=(void*)_tmp3F5->r;void*_tmp448;
struct Cyc_Absyn_Vardecl*_tmp449;void*_tmp44A;struct Cyc_Absyn_Vardecl*_tmp44B;
void*_tmp44C;struct Cyc_Absyn_Vardecl*_tmp44D;void*_tmp44E;struct Cyc_Absyn_Vardecl*
_tmp44F;_LL2A4: if(*((int*)_tmp447)!= 1)goto _LL2A6;_tmp448=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp447)->f2;if(_tmp448 <= (void*)1)goto _LL2A6;if(*((int*)_tmp448)!= 0)goto _LL2A6;
_tmp449=((struct Cyc_Absyn_Global_b_struct*)_tmp448)->f1;if(!(!_tmp449->escapes))
goto _LL2A6;_LL2A5: _tmp44B=_tmp449;goto _LL2A7;_LL2A6: if(*((int*)_tmp447)!= 1)goto
_LL2A8;_tmp44A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp447)->f2;if(_tmp44A <= (
void*)1)goto _LL2A8;if(*((int*)_tmp44A)!= 3)goto _LL2A8;_tmp44B=((struct Cyc_Absyn_Local_b_struct*)
_tmp44A)->f1;if(!(!_tmp44B->escapes))goto _LL2A8;_LL2A7: _tmp44D=_tmp44B;goto
_LL2A9;_LL2A8: if(*((int*)_tmp447)!= 1)goto _LL2AA;_tmp44C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp447)->f2;if(_tmp44C <= (void*)1)goto _LL2AA;if(*((int*)_tmp44C)!= 4)goto _LL2AA;
_tmp44D=((struct Cyc_Absyn_Pat_b_struct*)_tmp44C)->f1;if(!(!_tmp44D->escapes))
goto _LL2AA;_LL2A9: _tmp44F=_tmp44D;goto _LL2AB;_LL2AA: if(*((int*)_tmp447)!= 1)goto
_LL2AC;_tmp44E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp447)->f2;if(_tmp44E <= (
void*)1)goto _LL2AC;if(*((int*)_tmp44E)!= 2)goto _LL2AC;_tmp44F=((struct Cyc_Absyn_Param_b_struct*)
_tmp44E)->f1;if(!(!_tmp44F->escapes))goto _LL2AC;_LL2AB: {void*_tmp450=(void*)
_tmp3F6->r;void*_tmp451;struct Cyc_Absyn_Vardecl*_tmp452;void*_tmp453;struct Cyc_Absyn_Vardecl*
_tmp454;void*_tmp455;struct Cyc_Absyn_Vardecl*_tmp456;void*_tmp457;struct Cyc_Absyn_Vardecl*
_tmp458;union Cyc_Absyn_Cnst_union _tmp459;int _tmp45A;void*_tmp45B;struct Cyc_List_List*
_tmp45C;struct Cyc_List_List _tmp45D;struct Cyc_Absyn_Exp*_tmp45E;_LL2AF: if(*((int*)
_tmp450)!= 1)goto _LL2B1;_tmp451=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp450)->f2;
if(_tmp451 <= (void*)1)goto _LL2B1;if(*((int*)_tmp451)!= 0)goto _LL2B1;_tmp452=((
struct Cyc_Absyn_Global_b_struct*)_tmp451)->f1;if(!(!_tmp452->escapes))goto _LL2B1;
_LL2B0: _tmp454=_tmp452;goto _LL2B2;_LL2B1: if(*((int*)_tmp450)!= 1)goto _LL2B3;
_tmp453=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp450)->f2;if(_tmp453 <= (void*)
1)goto _LL2B3;if(*((int*)_tmp453)!= 3)goto _LL2B3;_tmp454=((struct Cyc_Absyn_Local_b_struct*)
_tmp453)->f1;if(!(!_tmp454->escapes))goto _LL2B3;_LL2B2: _tmp456=_tmp454;goto
_LL2B4;_LL2B3: if(*((int*)_tmp450)!= 1)goto _LL2B5;_tmp455=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp450)->f2;if(_tmp455 <= (void*)1)goto _LL2B5;if(*((int*)_tmp455)!= 4)goto _LL2B5;
_tmp456=((struct Cyc_Absyn_Pat_b_struct*)_tmp455)->f1;if(!(!_tmp456->escapes))
goto _LL2B5;_LL2B4: _tmp458=_tmp456;goto _LL2B6;_LL2B5: if(*((int*)_tmp450)!= 1)goto
_LL2B7;_tmp457=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp450)->f2;if(_tmp457 <= (
void*)1)goto _LL2B7;if(*((int*)_tmp457)!= 2)goto _LL2B7;_tmp458=((struct Cyc_Absyn_Param_b_struct*)
_tmp457)->f1;if(!(!_tmp458->escapes))goto _LL2B7;_LL2B6: {struct _RegionHandle*
_tmp45F=(env->fenv)->r;struct Cyc_List_List*_tmp460=({struct Cyc_List_List*_tmp463=
_region_malloc(_tmp45F,sizeof(*_tmp463));_tmp463->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp464=_region_malloc(_tmp45F,sizeof(*_tmp464));_tmp464->vd=_tmp44F;_tmp464->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp465;(
_tmp465.LessVar).tag=1;(_tmp465.LessVar).f1=_tmp458;(_tmp465.LessVar).f2=(void*)((
void*)_tmp458->type);_tmp465;});_tmp464;});_tmp463->tl=_tmp3F2;_tmp463;});return({
struct _tuple8 _tmp461;_tmp461.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp462;(_tmp462.ReachableFL).tag=1;(_tmp462.ReachableFL).f1=_tmp3F1;(_tmp462.ReachableFL).f2=
_tmp460;(_tmp462.ReachableFL).f3=_tmp3F3;_tmp462;});_tmp461.f2=_tmp440;_tmp461;});}
_LL2B7: if(*((int*)_tmp450)!= 0)goto _LL2B9;_tmp459=((struct Cyc_Absyn_Const_e_struct*)
_tmp450)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp450)->f1).Int_c).tag != 2)
goto _LL2B9;_tmp45A=(_tmp459.Int_c).f2;_LL2B8: {struct _RegionHandle*_tmp466=(env->fenv)->r;
struct Cyc_List_List*_tmp467=({struct Cyc_List_List*_tmp46A=_region_malloc(_tmp466,
sizeof(*_tmp46A));_tmp46A->hd=({struct Cyc_CfFlowInfo_Reln*_tmp46B=_region_malloc(
_tmp466,sizeof(*_tmp46B));_tmp46B->vd=_tmp44F;_tmp46B->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp46C;(_tmp46C.LessConst).tag=3;(_tmp46C.LessConst).f1=(
unsigned int)_tmp45A;_tmp46C;});_tmp46B;});_tmp46A->tl=_tmp3F2;_tmp46A;});return({
struct _tuple8 _tmp468;_tmp468.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp469;(_tmp469.ReachableFL).tag=1;(_tmp469.ReachableFL).f1=_tmp3F1;(_tmp469.ReachableFL).f2=
_tmp467;(_tmp469.ReachableFL).f3=_tmp3F3;_tmp469;});_tmp468.f2=_tmp440;_tmp468;});}
_LL2B9: if(*((int*)_tmp450)!= 3)goto _LL2BB;_tmp45B=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp450)->f1;_tmp45C=((struct Cyc_Absyn_Primop_e_struct*)_tmp450)->f2;if(_tmp45C
== 0)goto _LL2BB;_tmp45D=*_tmp45C;_tmp45E=(struct Cyc_Absyn_Exp*)_tmp45D.hd;_LL2BA: {
void*_tmp46D=(void*)_tmp45E->r;void*_tmp46E;struct Cyc_Absyn_Vardecl*_tmp46F;void*
_tmp470;struct Cyc_Absyn_Vardecl*_tmp471;void*_tmp472;struct Cyc_Absyn_Vardecl*
_tmp473;void*_tmp474;struct Cyc_Absyn_Vardecl*_tmp475;_LL2BE: if(*((int*)_tmp46D)
!= 1)goto _LL2C0;_tmp46E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp46D)->f2;if(
_tmp46E <= (void*)1)goto _LL2C0;if(*((int*)_tmp46E)!= 0)goto _LL2C0;_tmp46F=((
struct Cyc_Absyn_Global_b_struct*)_tmp46E)->f1;if(!(!_tmp46F->escapes))goto _LL2C0;
_LL2BF: _tmp471=_tmp46F;goto _LL2C1;_LL2C0: if(*((int*)_tmp46D)!= 1)goto _LL2C2;
_tmp470=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp46D)->f2;if(_tmp470 <= (void*)
1)goto _LL2C2;if(*((int*)_tmp470)!= 3)goto _LL2C2;_tmp471=((struct Cyc_Absyn_Local_b_struct*)
_tmp470)->f1;if(!(!_tmp471->escapes))goto _LL2C2;_LL2C1: _tmp473=_tmp471;goto
_LL2C3;_LL2C2: if(*((int*)_tmp46D)!= 1)goto _LL2C4;_tmp472=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp46D)->f2;if(_tmp472 <= (void*)1)goto _LL2C4;if(*((int*)_tmp472)!= 4)goto _LL2C4;
_tmp473=((struct Cyc_Absyn_Pat_b_struct*)_tmp472)->f1;if(!(!_tmp473->escapes))
goto _LL2C4;_LL2C3: _tmp475=_tmp473;goto _LL2C5;_LL2C4: if(*((int*)_tmp46D)!= 1)goto
_LL2C6;_tmp474=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp46D)->f2;if(_tmp474 <= (
void*)1)goto _LL2C6;if(*((int*)_tmp474)!= 2)goto _LL2C6;_tmp475=((struct Cyc_Absyn_Param_b_struct*)
_tmp474)->f1;if(!(!_tmp475->escapes))goto _LL2C6;_LL2C5: {struct _RegionHandle*
_tmp476=(env->fenv)->r;struct Cyc_List_List*_tmp477=({struct Cyc_List_List*_tmp47A=
_region_malloc(_tmp476,sizeof(*_tmp47A));_tmp47A->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp47B=_region_malloc(_tmp476,sizeof(*_tmp47B));_tmp47B->vd=_tmp44F;_tmp47B->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp47C;(
_tmp47C.LessNumelts).tag=2;(_tmp47C.LessNumelts).f1=_tmp475;_tmp47C;});_tmp47B;});
_tmp47A->tl=_tmp3F2;_tmp47A;});return({struct _tuple8 _tmp478;_tmp478.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp479;(_tmp479.ReachableFL).tag=1;(_tmp479.ReachableFL).f1=
_tmp3F1;(_tmp479.ReachableFL).f2=_tmp477;(_tmp479.ReachableFL).f3=_tmp3F3;
_tmp479;});_tmp478.f2=_tmp440;_tmp478;});}_LL2C6:;_LL2C7: return({struct _tuple8
_tmp47D;_tmp47D.f1=_tmp43F;_tmp47D.f2=_tmp440;_tmp47D;});_LL2BD:;}_LL2BB:;_LL2BC:
return({struct _tuple8 _tmp47E;_tmp47E.f1=_tmp43F;_tmp47E.f2=_tmp440;_tmp47E;});
_LL2AE:;}_LL2AC:;_LL2AD: return({struct _tuple8 _tmp47F;_tmp47F.f1=_tmp43F;_tmp47F.f2=
_tmp440;_tmp47F;});_LL2A3:;}}_LL29A: if((int)_tmp434 != 10)goto _LL29C;_LL29B: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp480=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3F5,r1,(void*)10,r2);union Cyc_CfFlowInfo_FlowInfo_union _tmp481=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3F6,r1,(void*)8,r1);{union Cyc_CfFlowInfo_FlowInfo_union _tmp482=_tmp480;
struct Cyc_Dict_Dict _tmp483;struct Cyc_CfFlowInfo_ConsumeInfo _tmp484;_LL2C9: if((
_tmp482.BottomFL).tag != 0)goto _LL2CB;_LL2CA:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp485=_cycalloc(sizeof(*_tmp485));_tmp485[0]=({struct Cyc_Core_Impossible_struct
_tmp486;_tmp486.tag=Cyc_Core_Impossible;_tmp486.f1=({const char*_tmp487="anal_test, Lte";
_tag_dyneither(_tmp487,sizeof(char),15);});_tmp486;});_tmp485;}));_LL2CB: if((
_tmp482.ReachableFL).tag != 1)goto _LL2C8;_tmp483=(_tmp482.ReachableFL).f1;_tmp484=(
_tmp482.ReachableFL).f3;_LL2CC: _tmp3F1=_tmp483;_tmp3F3=_tmp484;_LL2C8:;}{void*
_tmp488=(void*)_tmp3F5->r;void*_tmp489;struct Cyc_Absyn_Vardecl*_tmp48A;void*
_tmp48B;struct Cyc_Absyn_Vardecl*_tmp48C;void*_tmp48D;struct Cyc_Absyn_Vardecl*
_tmp48E;void*_tmp48F;struct Cyc_Absyn_Vardecl*_tmp490;_LL2CE: if(*((int*)_tmp488)
!= 1)goto _LL2D0;_tmp489=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp488)->f2;if(
_tmp489 <= (void*)1)goto _LL2D0;if(*((int*)_tmp489)!= 0)goto _LL2D0;_tmp48A=((
struct Cyc_Absyn_Global_b_struct*)_tmp489)->f1;if(!(!_tmp48A->escapes))goto _LL2D0;
_LL2CF: _tmp48C=_tmp48A;goto _LL2D1;_LL2D0: if(*((int*)_tmp488)!= 1)goto _LL2D2;
_tmp48B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp488)->f2;if(_tmp48B <= (void*)
1)goto _LL2D2;if(*((int*)_tmp48B)!= 3)goto _LL2D2;_tmp48C=((struct Cyc_Absyn_Local_b_struct*)
_tmp48B)->f1;if(!(!_tmp48C->escapes))goto _LL2D2;_LL2D1: _tmp48E=_tmp48C;goto
_LL2D3;_LL2D2: if(*((int*)_tmp488)!= 1)goto _LL2D4;_tmp48D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp488)->f2;if(_tmp48D <= (void*)1)goto _LL2D4;if(*((int*)_tmp48D)!= 4)goto _LL2D4;
_tmp48E=((struct Cyc_Absyn_Pat_b_struct*)_tmp48D)->f1;if(!(!_tmp48E->escapes))
goto _LL2D4;_LL2D3: _tmp490=_tmp48E;goto _LL2D5;_LL2D4: if(*((int*)_tmp488)!= 1)goto
_LL2D6;_tmp48F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp488)->f2;if(_tmp48F <= (
void*)1)goto _LL2D6;if(*((int*)_tmp48F)!= 2)goto _LL2D6;_tmp490=((struct Cyc_Absyn_Param_b_struct*)
_tmp48F)->f1;if(!(!_tmp490->escapes))goto _LL2D6;_LL2D5: {void*_tmp491=(void*)
_tmp3F6->r;void*_tmp492;struct Cyc_List_List*_tmp493;struct Cyc_List_List _tmp494;
struct Cyc_Absyn_Exp*_tmp495;_LL2D9: if(*((int*)_tmp491)!= 3)goto _LL2DB;_tmp492=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp491)->f1;_tmp493=((struct Cyc_Absyn_Primop_e_struct*)
_tmp491)->f2;if(_tmp493 == 0)goto _LL2DB;_tmp494=*_tmp493;_tmp495=(struct Cyc_Absyn_Exp*)
_tmp494.hd;_LL2DA: {void*_tmp496=(void*)_tmp495->r;void*_tmp497;struct Cyc_Absyn_Vardecl*
_tmp498;void*_tmp499;struct Cyc_Absyn_Vardecl*_tmp49A;void*_tmp49B;struct Cyc_Absyn_Vardecl*
_tmp49C;void*_tmp49D;struct Cyc_Absyn_Vardecl*_tmp49E;_LL2DE: if(*((int*)_tmp496)
!= 1)goto _LL2E0;_tmp497=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp496)->f2;if(
_tmp497 <= (void*)1)goto _LL2E0;if(*((int*)_tmp497)!= 0)goto _LL2E0;_tmp498=((
struct Cyc_Absyn_Global_b_struct*)_tmp497)->f1;if(!(!_tmp498->escapes))goto _LL2E0;
_LL2DF: _tmp49A=_tmp498;goto _LL2E1;_LL2E0: if(*((int*)_tmp496)!= 1)goto _LL2E2;
_tmp499=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp496)->f2;if(_tmp499 <= (void*)
1)goto _LL2E2;if(*((int*)_tmp499)!= 3)goto _LL2E2;_tmp49A=((struct Cyc_Absyn_Local_b_struct*)
_tmp499)->f1;if(!(!_tmp49A->escapes))goto _LL2E2;_LL2E1: _tmp49C=_tmp49A;goto
_LL2E3;_LL2E2: if(*((int*)_tmp496)!= 1)goto _LL2E4;_tmp49B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp496)->f2;if(_tmp49B <= (void*)1)goto _LL2E4;if(*((int*)_tmp49B)!= 4)goto _LL2E4;
_tmp49C=((struct Cyc_Absyn_Pat_b_struct*)_tmp49B)->f1;if(!(!_tmp49C->escapes))
goto _LL2E4;_LL2E3: _tmp49E=_tmp49C;goto _LL2E5;_LL2E4: if(*((int*)_tmp496)!= 1)goto
_LL2E6;_tmp49D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp496)->f2;if(_tmp49D <= (
void*)1)goto _LL2E6;if(*((int*)_tmp49D)!= 2)goto _LL2E6;_tmp49E=((struct Cyc_Absyn_Param_b_struct*)
_tmp49D)->f1;if(!(!_tmp49E->escapes))goto _LL2E6;_LL2E5: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp49F=env->fenv;struct Cyc_List_List*_tmp4A0=({struct Cyc_List_List*_tmp4A3=
_region_malloc(_tmp49F->r,sizeof(*_tmp4A3));_tmp4A3->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp4A4=_region_malloc(_tmp49F->r,sizeof(*_tmp4A4));_tmp4A4->vd=_tmp490;_tmp4A4->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp4A5;(
_tmp4A5.LessEqNumelts).tag=4;(_tmp4A5.LessEqNumelts).f1=_tmp49E;_tmp4A5;});
_tmp4A4;});_tmp4A3->tl=_tmp3F2;_tmp4A3;});return({struct _tuple8 _tmp4A1;_tmp4A1.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp4A2;(
_tmp4A2.ReachableFL).tag=1;(_tmp4A2.ReachableFL).f1=_tmp3F1;(_tmp4A2.ReachableFL).f2=
_tmp4A0;(_tmp4A2.ReachableFL).f3=_tmp3F3;_tmp4A2;});_tmp4A1.f2=_tmp481;_tmp4A1;});}
_LL2E6:;_LL2E7: return({struct _tuple8 _tmp4A6;_tmp4A6.f1=_tmp480;_tmp4A6.f2=
_tmp481;_tmp4A6;});_LL2DD:;}_LL2DB:;_LL2DC: return({struct _tuple8 _tmp4A7;_tmp4A7.f1=
_tmp480;_tmp4A7.f2=_tmp481;_tmp4A7;});_LL2D8:;}_LL2D6:;_LL2D7: return({struct
_tuple8 _tmp4A8;_tmp4A8.f1=_tmp480;_tmp4A8.f2=_tmp481;_tmp4A8;});_LL2CD:;}}_LL29C:;
_LL29D: return({struct _tuple8 _tmp4A9;_tmp4A9.f1=f;_tmp4A9.f2=f;_tmp4A9;});_LL28F:;}}
_LL25C:;}}_LL25A:;_LL25B: goto _LL24D;_LL24D:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp4AB;void*_tmp4AC;struct _tuple5 _tmp4AA=Cyc_NewControlFlow_anal_Rexp(env,
inflow,e);_tmp4AB=_tmp4AA.f1;_tmp4AC=_tmp4AA.f2;_tmp4AB=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp4AB);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4AD=_tmp4AB;struct Cyc_Dict_Dict
_tmp4AE;_LL2E9: if((_tmp4AD.BottomFL).tag != 0)goto _LL2EB;_LL2EA: return({struct
_tuple8 _tmp4AF;_tmp4AF.f1=_tmp4AB;_tmp4AF.f2=_tmp4AB;_tmp4AF;});_LL2EB: if((
_tmp4AD.ReachableFL).tag != 1)goto _LL2E8;_tmp4AE=(_tmp4AD.ReachableFL).f1;_LL2EC: {
void*_tmp4B0=_tmp4AC;void*_tmp4B1;void*_tmp4B2;void*_tmp4B3;_LL2EE: if((int)
_tmp4B0 != 0)goto _LL2F0;_LL2EF: return({struct _tuple8 _tmp4B4;_tmp4B4.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4B5;(_tmp4B5.BottomFL).tag=0;_tmp4B5;});
_tmp4B4.f2=_tmp4AB;_tmp4B4;});_LL2F0: if((int)_tmp4B0 != 2)goto _LL2F2;_LL2F1: goto
_LL2F3;_LL2F2: if((int)_tmp4B0 != 1)goto _LL2F4;_LL2F3: goto _LL2F5;_LL2F4: if(_tmp4B0
<= (void*)3)goto _LL2F6;if(*((int*)_tmp4B0)!= 2)goto _LL2F6;_LL2F5: return({struct
_tuple8 _tmp4B6;_tmp4B6.f1=_tmp4AB;_tmp4B6.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4B7;(_tmp4B7.BottomFL).tag=0;_tmp4B7;});
_tmp4B6;});_LL2F6: if(_tmp4B0 <= (void*)3)goto _LL2F8;if(*((int*)_tmp4B0)!= 0)goto
_LL2F8;_tmp4B1=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp4B0)->f1;if((
int)_tmp4B1 != 0)goto _LL2F8;_LL2F7: goto _LL2F9;_LL2F8: if(_tmp4B0 <= (void*)3)goto
_LL2FA;if(*((int*)_tmp4B0)!= 1)goto _LL2FA;_tmp4B2=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp4B0)->f1;if((int)_tmp4B2 != 0)goto _LL2FA;_LL2F9:({void*_tmp4B8=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp4B9="expression may not be initialized";_tag_dyneither(
_tmp4B9,sizeof(char),34);}),_tag_dyneither(_tmp4B8,sizeof(void*),0));});return({
struct _tuple8 _tmp4BA;_tmp4BA.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4BC;(_tmp4BC.BottomFL).tag=0;_tmp4BC;});_tmp4BA.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4BB;(_tmp4BB.BottomFL).tag=0;_tmp4BB;});
_tmp4BA;});_LL2FA: if(_tmp4B0 <= (void*)3)goto _LL2FC;if(*((int*)_tmp4B0)!= 0)goto
_LL2FC;_tmp4B3=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp4B0)->f1;
_LL2FB: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4AB,e,_tmp4B3);_LL2FC:
if(_tmp4B0 <= (void*)3)goto _LL2FE;if(*((int*)_tmp4B0)!= 1)goto _LL2FE;_LL2FD:
return({struct _tuple8 _tmp4BD;_tmp4BD.f1=_tmp4AB;_tmp4BD.f2=_tmp4AB;_tmp4BD;});
_LL2FE: if(_tmp4B0 <= (void*)3)goto _LL300;if(*((int*)_tmp4B0)!= 3)goto _LL300;
_LL2FF: return({struct _tuple8 _tmp4BE;_tmp4BE.f1=_tmp4AB;_tmp4BE.f2=_tmp4AB;
_tmp4BE;});_LL300: if(_tmp4B0 <= (void*)3)goto _LL2ED;if(*((int*)_tmp4B0)!= 4)goto
_LL2ED;_LL301:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp4BF=
_cycalloc(sizeof(*_tmp4BF));_tmp4BF[0]=({struct Cyc_Core_Impossible_struct _tmp4C0;
_tmp4C0.tag=Cyc_Core_Impossible;_tmp4C0.f1=({const char*_tmp4C1="anal_test";
_tag_dyneither(_tmp4C1,sizeof(char),10);});_tmp4C0;});_tmp4BF;}));_LL2ED:;}
_LL2E8:;}}}static void Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*
loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union flow){
union Cyc_CfFlowInfo_FlowInfo_union _tmp4C2=flow;struct Cyc_Dict_Dict _tmp4C3;_LL303:
if((_tmp4C2.BottomFL).tag != 0)goto _LL305;_LL304: return;_LL305: if((_tmp4C2.ReachableFL).tag
!= 1)goto _LL302;_tmp4C3=(_tmp4C2.ReachableFL).f1;_LL306:{struct Cyc_List_List*
_tmp4C4=env->param_roots;for(0;_tmp4C4 != 0;_tmp4C4=_tmp4C4->tl){if(Cyc_CfFlowInfo_initlevel(
_tmp4C3,Cyc_CfFlowInfo_lookup_place(_tmp4C3,(struct Cyc_CfFlowInfo_Place*)_tmp4C4->hd))
!= (void*)2)({void*_tmp4C5=0;Cyc_Tcutil_terr(loc,({const char*_tmp4C6="function may not initialize all the parameters with attribute 'initializes'";
_tag_dyneither(_tmp4C6,sizeof(char),76);}),_tag_dyneither(_tmp4C5,sizeof(void*),
0));});}}return;_LL302:;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp4C7=env->fenv;for(0;
scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp4C9;struct Cyc_Core_Opt*
_tmp4CA;struct Cyc_Absyn_Exp*_tmp4CB;struct Cyc_Absyn_Stmt*_tmp4CC;struct Cyc_Position_Segment*
_tmp4CD;struct Cyc_Absyn_Switch_clause*_tmp4C8=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp4C9=*_tmp4C8;_tmp4CA=_tmp4C9.pat_vars;_tmp4CB=_tmp4C9.where_clause;
_tmp4CC=_tmp4C9.body;_tmp4CD=_tmp4C9.loc;{union Cyc_CfFlowInfo_FlowInfo_union
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp4C7,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp4CA))->v,(void*)_tmp4C7->unknown_all,_tmp4CD);
union Cyc_CfFlowInfo_FlowInfo_union clause_outflow;if(_tmp4CB != 0){struct Cyc_Absyn_Exp*
wexp=(struct Cyc_Absyn_Exp*)_tmp4CB;union Cyc_CfFlowInfo_FlowInfo_union _tmp4CF;
union Cyc_CfFlowInfo_FlowInfo_union _tmp4D0;struct _tuple8 _tmp4CE=Cyc_NewControlFlow_anal_test(
env,clause_inflow,wexp);_tmp4CF=_tmp4CE.f1;_tmp4D0=_tmp4CE.f2;_tmp4CF=Cyc_CfFlowInfo_readthrough_unique_rvals(
wexp->loc,_tmp4CF);_tmp4D0=Cyc_CfFlowInfo_readthrough_unique_rvals(wexp->loc,
_tmp4D0);inflow=_tmp4D0;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp4CF,
_tmp4CC);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,
_tmp4CC);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp4D1=clause_outflow;_LL308: if((
_tmp4D1.BottomFL).tag != 0)goto _LL30A;_LL309: goto _LL307;_LL30A:;_LL30B: if(scs->tl
== 0)return clause_outflow;else{if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(
scs->tl))->hd)->pat_vars))->v != 0)({void*_tmp4D2=0;Cyc_Tcutil_terr(_tmp4CC->loc,({
const char*_tmp4D3="switch clause may implicitly fallthru";_tag_dyneither(_tmp4D3,
sizeof(char),38);}),_tag_dyneither(_tmp4D2,sizeof(void*),0));});else{({void*
_tmp4D4=0;Cyc_Tcutil_warn(_tmp4CC->loc,({const char*_tmp4D5="switch clause may implicitly fallthru";
_tag_dyneither(_tmp4D5,sizeof(char),38);}),_tag_dyneither(_tmp4D4,sizeof(void*),
0));});}Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}goto
_LL307;_LL307:;}}}return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4D6;(_tmp4D6.BottomFL).tag=0;_tmp4D6;});}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union outflow;struct
Cyc_NewControlFlow_CFStmtAnnot*_tmp4D8;union Cyc_CfFlowInfo_FlowInfo_union*
_tmp4D9;struct _tuple9 _tmp4D7=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);
_tmp4D8=_tmp4D7.f1;_tmp4D9=_tmp4D7.f2;inflow=*_tmp4D9;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp4DA=env->fenv;{void*_tmp4DB=(void*)s->r;struct Cyc_Absyn_Exp*_tmp4DC;struct
Cyc_Absyn_Exp*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DE;struct Cyc_Absyn_Stmt*_tmp4DF;
struct Cyc_Absyn_Stmt*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;struct Cyc_Absyn_Stmt*
_tmp4E2;struct Cyc_Absyn_Stmt*_tmp4E3;struct _tuple3 _tmp4E4;struct Cyc_Absyn_Exp*
_tmp4E5;struct Cyc_Absyn_Stmt*_tmp4E6;struct Cyc_Absyn_Stmt*_tmp4E7;struct Cyc_Absyn_Stmt*
_tmp4E8;struct _tuple3 _tmp4E9;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_Absyn_Stmt*
_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EC;struct _tuple3 _tmp4ED;struct Cyc_Absyn_Exp*
_tmp4EE;struct Cyc_Absyn_Stmt*_tmp4EF;struct _tuple3 _tmp4F0;struct Cyc_Absyn_Exp*
_tmp4F1;struct Cyc_Absyn_Stmt*_tmp4F2;struct Cyc_Absyn_Stmt*_tmp4F3;struct Cyc_Absyn_Stmt*
_tmp4F4;struct Cyc_List_List*_tmp4F5;struct Cyc_Absyn_Switch_clause**_tmp4F6;
struct Cyc_Absyn_Switch_clause*_tmp4F7;struct Cyc_Absyn_Stmt*_tmp4F8;struct Cyc_Absyn_Stmt*
_tmp4F9;struct Cyc_Absyn_Stmt*_tmp4FA;struct Cyc_Absyn_Exp*_tmp4FB;struct Cyc_List_List*
_tmp4FC;struct Cyc_Absyn_Stmt*_tmp4FD;struct Cyc_List_List*_tmp4FE;struct Cyc_Absyn_Decl*
_tmp4FF;struct Cyc_Absyn_Stmt*_tmp500;struct Cyc_Absyn_Stmt*_tmp501;struct Cyc_Absyn_Tvar*
_tmp502;struct Cyc_Absyn_Vardecl*_tmp503;int _tmp504;struct Cyc_Absyn_Exp*_tmp505;
struct Cyc_Absyn_Stmt*_tmp506;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_Absyn_Exp*
_tmp508;struct Cyc_Absyn_Tvar*_tmp509;struct Cyc_Absyn_Vardecl*_tmp50A;struct Cyc_Absyn_Stmt*
_tmp50B;_LL30D: if((int)_tmp4DB != 0)goto _LL30F;_LL30E: return inflow;_LL30F: if(
_tmp4DB <= (void*)1)goto _LL337;if(*((int*)_tmp4DB)!= 2)goto _LL311;_tmp4DC=((
struct Cyc_Absyn_Return_s_struct*)_tmp4DB)->f1;if(_tmp4DC != 0)goto _LL311;_LL310:
if(env->noreturn)({void*_tmp50C=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp50D="`noreturn' function might return";
_tag_dyneither(_tmp50D,sizeof(char),33);}),_tag_dyneither(_tmp50C,sizeof(void*),
0));});Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp50E;(_tmp50E.BottomFL).tag=0;_tmp50E;});
_LL311: if(*((int*)_tmp4DB)!= 2)goto _LL313;_tmp4DD=((struct Cyc_Absyn_Return_s_struct*)
_tmp4DB)->f1;_LL312: if(env->noreturn)({void*_tmp50F=0;Cyc_Tcutil_terr(s->loc,({
const char*_tmp510="`noreturn' function might return";_tag_dyneither(_tmp510,
sizeof(char),33);}),_tag_dyneither(_tmp50F,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp512;void*_tmp513;struct _tuple5 _tmp511=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp4DD));_tmp512=_tmp511.f1;_tmp513=
_tmp511.f2;_tmp512=Cyc_CfFlowInfo_consume_unique_rvals(_tmp4DD->loc,_tmp512);
_tmp512=Cyc_NewControlFlow_use_Rval(env,_tmp4DD->loc,_tmp512,_tmp513);Cyc_NewControlFlow_check_init_params(
s->loc,env,_tmp512);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp514;(_tmp514.BottomFL).tag=0;_tmp514;});}_LL313: if(*((int*)_tmp4DB)!= 0)goto
_LL315;_tmp4DE=((struct Cyc_Absyn_Exp_s_struct*)_tmp4DB)->f1;_LL314: outflow=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4DE)).f1;goto _LL30C;_LL315: if(*((int*)_tmp4DB)!= 1)goto _LL317;
_tmp4DF=((struct Cyc_Absyn_Seq_s_struct*)_tmp4DB)->f1;_tmp4E0=((struct Cyc_Absyn_Seq_s_struct*)
_tmp4DB)->f2;_LL316: outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4DF),_tmp4E0);goto _LL30C;_LL317: if(*((int*)_tmp4DB)!= 3)goto
_LL319;_tmp4E1=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4DB)->f1;_tmp4E2=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4DB)->f2;_tmp4E3=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp4DB)->f3;_LL318: {union Cyc_CfFlowInfo_FlowInfo_union _tmp516;union Cyc_CfFlowInfo_FlowInfo_union
_tmp517;struct _tuple8 _tmp515=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4E1);
_tmp516=_tmp515.f1;_tmp517=_tmp515.f2;_tmp516=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4E1->loc,_tmp516);_tmp517=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4E1->loc,
_tmp517);outflow=Cyc_CfFlowInfo_join_flow(_tmp4DA,env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp516,_tmp4E2),Cyc_NewControlFlow_anal_stmt(env,_tmp517,_tmp4E3),1);goto
_LL30C;}_LL319: if(*((int*)_tmp4DB)!= 4)goto _LL31B;_tmp4E4=((struct Cyc_Absyn_While_s_struct*)
_tmp4DB)->f1;_tmp4E5=_tmp4E4.f1;_tmp4E6=_tmp4E4.f2;_tmp4E7=((struct Cyc_Absyn_While_s_struct*)
_tmp4DB)->f2;_LL31A: {union Cyc_CfFlowInfo_FlowInfo_union*_tmp519;struct _tuple9
_tmp518=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp4E6);_tmp519=_tmp518.f2;{
union Cyc_CfFlowInfo_FlowInfo_union _tmp51A=*_tmp519;union Cyc_CfFlowInfo_FlowInfo_union
_tmp51C;union Cyc_CfFlowInfo_FlowInfo_union _tmp51D;struct _tuple8 _tmp51B=Cyc_NewControlFlow_anal_test(
env,_tmp51A,_tmp4E5);_tmp51C=_tmp51B.f1;_tmp51D=_tmp51B.f2;_tmp51C=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4E5->loc,_tmp51C);_tmp51D=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4E5->loc,
_tmp51D);{union Cyc_CfFlowInfo_FlowInfo_union _tmp51E=Cyc_NewControlFlow_anal_stmt(
env,_tmp51C,_tmp4E7);Cyc_NewControlFlow_update_flow(env,_tmp4E6,_tmp51E);outflow=
_tmp51D;goto _LL30C;}}}_LL31B: if(*((int*)_tmp4DB)!= 13)goto _LL31D;_tmp4E8=((
struct Cyc_Absyn_Do_s_struct*)_tmp4DB)->f1;_tmp4E9=((struct Cyc_Absyn_Do_s_struct*)
_tmp4DB)->f2;_tmp4EA=_tmp4E9.f1;_tmp4EB=_tmp4E9.f2;_LL31C: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp51F=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4E8);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp521;struct _tuple9 _tmp520=Cyc_NewControlFlow_pre_stmt_check(env,_tmp51F,
_tmp4EB);_tmp521=_tmp520.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp522=*_tmp521;
union Cyc_CfFlowInfo_FlowInfo_union _tmp524;union Cyc_CfFlowInfo_FlowInfo_union
_tmp525;struct _tuple8 _tmp523=Cyc_NewControlFlow_anal_test(env,_tmp522,_tmp4EA);
_tmp524=_tmp523.f1;_tmp525=_tmp523.f2;Cyc_NewControlFlow_update_flow(env,_tmp4E8,
_tmp524);outflow=_tmp525;goto _LL30C;}}_LL31D: if(*((int*)_tmp4DB)!= 8)goto _LL31F;
_tmp4EC=((struct Cyc_Absyn_For_s_struct*)_tmp4DB)->f1;_tmp4ED=((struct Cyc_Absyn_For_s_struct*)
_tmp4DB)->f2;_tmp4EE=_tmp4ED.f1;_tmp4EF=_tmp4ED.f2;_tmp4F0=((struct Cyc_Absyn_For_s_struct*)
_tmp4DB)->f3;_tmp4F1=_tmp4F0.f1;_tmp4F2=_tmp4F0.f2;_tmp4F3=((struct Cyc_Absyn_For_s_struct*)
_tmp4DB)->f4;_LL31E: {union Cyc_CfFlowInfo_FlowInfo_union _tmp526=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4EC)).f1;_tmp526=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4EC->loc,
_tmp526);{union Cyc_CfFlowInfo_FlowInfo_union*_tmp528;struct _tuple9 _tmp527=Cyc_NewControlFlow_pre_stmt_check(
env,_tmp526,_tmp4EF);_tmp528=_tmp527.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp529=*_tmp528;union Cyc_CfFlowInfo_FlowInfo_union _tmp52B;union Cyc_CfFlowInfo_FlowInfo_union
_tmp52C;struct _tuple8 _tmp52A=Cyc_NewControlFlow_anal_test(env,_tmp529,_tmp4EE);
_tmp52B=_tmp52A.f1;_tmp52C=_tmp52A.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp52D=
Cyc_NewControlFlow_anal_stmt(env,_tmp52B,_tmp4F3);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp52F;struct _tuple9 _tmp52E=Cyc_NewControlFlow_pre_stmt_check(env,_tmp52D,
_tmp4F2);_tmp52F=_tmp52E.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp530=*_tmp52F;
union Cyc_CfFlowInfo_FlowInfo_union _tmp531=(Cyc_NewControlFlow_anal_Rexp(env,
_tmp530,_tmp4F1)).f1;_tmp531=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4F1->loc,
_tmp531);Cyc_NewControlFlow_update_flow(env,_tmp4EF,_tmp531);outflow=_tmp52C;
goto _LL30C;}}}}}_LL31F: if(*((int*)_tmp4DB)!= 5)goto _LL321;_tmp4F4=((struct Cyc_Absyn_Break_s_struct*)
_tmp4DB)->f1;if(_tmp4F4 != 0)goto _LL321;_LL320: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp532;(_tmp532.BottomFL).tag=0;_tmp532;});
_LL321: if(*((int*)_tmp4DB)!= 10)goto _LL323;_tmp4F5=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp4DB)->f1;_tmp4F6=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp4DB)->f2;if(
_tmp4F6 == 0)goto _LL323;_tmp4F7=*_tmp4F6;_LL322: {struct _RegionHandle _tmp533=
_new_region("temp");struct _RegionHandle*temp=& _tmp533;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp535;struct Cyc_List_List*_tmp536;struct _tuple11
_tmp534=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp4F5,0);
_tmp535=_tmp534.f1;_tmp536=_tmp534.f2;for(0;_tmp536 != 0;(_tmp536=_tmp536->tl,
_tmp4F5=_tmp4F5->tl)){_tmp535=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4F5))->hd)->loc,_tmp535,(void*)_tmp536->hd);}
_tmp535=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,_tmp535);_tmp535=Cyc_NewControlFlow_add_vars(
_tmp4DA,_tmp535,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4F7->pat_vars))->v,(
void*)_tmp4DA->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp4F7->body,_tmp535);{union Cyc_CfFlowInfo_FlowInfo_union _tmp538=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp537;(_tmp537.BottomFL).tag=0;_tmp537;});
_npop_handler(0);return _tmp538;}};_pop_region(temp);}_LL323: if(*((int*)_tmp4DB)
!= 5)goto _LL325;_tmp4F8=((struct Cyc_Absyn_Break_s_struct*)_tmp4DB)->f1;_LL324:
_tmp4F9=_tmp4F8;goto _LL326;_LL325: if(*((int*)_tmp4DB)!= 6)goto _LL327;_tmp4F9=((
struct Cyc_Absyn_Continue_s_struct*)_tmp4DB)->f1;_LL326: _tmp4FA=_tmp4F9;goto
_LL328;_LL327: if(*((int*)_tmp4DB)!= 7)goto _LL329;_tmp4FA=((struct Cyc_Absyn_Goto_s_struct*)
_tmp4DB)->f2;_LL328: if(env->iteration_num == 1){struct Cyc_Absyn_Stmt*_tmp539=
_tmp4D8->encloser;struct Cyc_Absyn_Stmt*_tmp53A=(Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)_check_null(_tmp4FA)))->encloser;while(_tmp53A != _tmp539){
struct Cyc_Absyn_Stmt*_tmp53B=(Cyc_NewControlFlow_get_stmt_annot(_tmp539))->encloser;
if(_tmp53B == _tmp539){({void*_tmp53C=0;Cyc_Tcutil_terr(s->loc,({const char*
_tmp53D="goto enters local scope or exception handler";_tag_dyneither(_tmp53D,
sizeof(char),45);}),_tag_dyneither(_tmp53C,sizeof(void*),0));});break;}_tmp539=
_tmp53B;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_check_null(
_tmp4FA),inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp53E;(_tmp53E.BottomFL).tag=0;_tmp53E;});_LL329: if(*((int*)_tmp4DB)!= 9)goto
_LL32B;_tmp4FB=((struct Cyc_Absyn_Switch_s_struct*)_tmp4DB)->f1;_tmp4FC=((struct
Cyc_Absyn_Switch_s_struct*)_tmp4DB)->f2;_LL32A: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp540;void*_tmp541;struct _tuple5 _tmp53F=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp4FB);_tmp540=_tmp53F.f1;_tmp541=_tmp53F.f2;_tmp540=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4FB->loc,_tmp540);_tmp540=Cyc_NewControlFlow_use_Rval(env,_tmp4FB->loc,
_tmp540,_tmp541);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp540,_tmp4FC);goto
_LL30C;}_LL32B: if(*((int*)_tmp4DB)!= 14)goto _LL32D;_tmp4FD=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp4DB)->f1;_tmp4FE=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4DB)->f2;_LL32C: {
int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo_union old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo_union s1_outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4FD);union Cyc_CfFlowInfo_FlowInfo_union
scs_inflow=env->tryflow;env->in_try=old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(
env,scs_inflow);{union Cyc_CfFlowInfo_FlowInfo_union scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp4FE);{union Cyc_CfFlowInfo_FlowInfo_union _tmp542=scs_outflow;
_LL33A: if((_tmp542.BottomFL).tag != 0)goto _LL33C;_LL33B: goto _LL339;_LL33C:;_LL33D:({
void*_tmp543=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp544="last catch clause may implicitly fallthru";
_tag_dyneither(_tmp544,sizeof(char),42);}),_tag_dyneither(_tmp543,sizeof(void*),
0));});_LL339:;}outflow=s1_outflow;goto _LL30C;}}}_LL32D: if(*((int*)_tmp4DB)!= 11)
goto _LL32F;_tmp4FF=((struct Cyc_Absyn_Decl_s_struct*)_tmp4DB)->f1;_tmp500=((
struct Cyc_Absyn_Decl_s_struct*)_tmp4DB)->f2;_LL32E: outflow=Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp4FF),_tmp500);goto _LL30C;_LL32F:
if(*((int*)_tmp4DB)!= 12)goto _LL331;_tmp501=((struct Cyc_Absyn_Label_s_struct*)
_tmp4DB)->f2;_LL330: outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp501);
goto _LL30C;_LL331: if(*((int*)_tmp4DB)!= 15)goto _LL333;_tmp502=((struct Cyc_Absyn_Region_s_struct*)
_tmp4DB)->f1;_tmp503=((struct Cyc_Absyn_Region_s_struct*)_tmp4DB)->f2;_tmp504=((
struct Cyc_Absyn_Region_s_struct*)_tmp4DB)->f3;_tmp505=((struct Cyc_Absyn_Region_s_struct*)
_tmp4DB)->f4;_tmp506=((struct Cyc_Absyn_Region_s_struct*)_tmp4DB)->f5;_LL332: if(
_tmp505 != 0){struct Cyc_Absyn_Exp*_tmp545=(struct Cyc_Absyn_Exp*)_tmp505;union Cyc_CfFlowInfo_FlowInfo_union
_tmp547;void*_tmp548;struct _tuple5 _tmp546=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp545);_tmp547=_tmp546.f1;_tmp548=_tmp546.f2;_tmp547=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp545->loc,_tmp547);inflow=Cyc_NewControlFlow_use_Rval(env,_tmp545->loc,
_tmp547,_tmp548);}{struct Cyc_List_List _tmp549=({struct Cyc_List_List _tmp54A;
_tmp54A.hd=_tmp503;_tmp54A.tl=0;_tmp54A;});inflow=Cyc_NewControlFlow_add_vars(
_tmp4DA,inflow,(struct Cyc_List_List*)& _tmp549,(void*)_tmp4DA->unknown_all,
_tmp506->loc);outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp506);goto
_LL30C;}_LL333: if(*((int*)_tmp4DB)!= 16)goto _LL335;_tmp507=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp4DB)->f1;_LL334: {union Cyc_CfFlowInfo_FlowInfo_union _tmp54C;void*_tmp54D;
struct _tuple5 _tmp54B=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp507);_tmp54C=
_tmp54B.f1;_tmp54D=_tmp54B.f2;_tmp54C=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp507->loc,_tmp54C);{union Cyc_CfFlowInfo_FlowInfo_union _tmp54E=Cyc_NewControlFlow_use_Rval(
env,_tmp507->loc,_tmp54C,_tmp54D);{void*_tmp54F=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp507->topt))->v);void*_tmp550;_LL33F: if(
_tmp54F <= (void*)4)goto _LL341;if(*((int*)_tmp54F)!= 14)goto _LL341;_tmp550=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp54F)->f1;_LL340: outflow=Cyc_CfFlowInfo_kill_flow_region(
_tmp4DA,_tmp54C,_tmp550);goto _LL33E;_LL341:;_LL342:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551[0]=({
struct Cyc_Core_Impossible_struct _tmp552;_tmp552.tag=Cyc_Core_Impossible;_tmp552.f1=({
const char*_tmp553="anal_stmt -- reset_region";_tag_dyneither(_tmp553,sizeof(char),
26);});_tmp552;});_tmp551;}));_LL33E:;}goto _LL30C;}}_LL335: if(*((int*)_tmp4DB)!= 
17)goto _LL337;_tmp508=((struct Cyc_Absyn_Alias_s_struct*)_tmp4DB)->f1;_tmp509=((
struct Cyc_Absyn_Alias_s_struct*)_tmp4DB)->f2;_tmp50A=((struct Cyc_Absyn_Alias_s_struct*)
_tmp4DB)->f3;_tmp50B=((struct Cyc_Absyn_Alias_s_struct*)_tmp4DB)->f4;_LL336: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp555;void*_tmp556;struct _tuple5 _tmp554=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp508);_tmp555=_tmp554.f1;_tmp556=_tmp554.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
_tmp558;struct Cyc_List_List*_tmp559;struct _tuple6 _tmp557=Cyc_CfFlowInfo_save_consume_info(
_tmp4DA,_tmp555,0);_tmp558=_tmp557.f1;_tmp559=_tmp558.may_consume;_tmp555=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp508->loc,_tmp555);_tmp555=Cyc_NewControlFlow_use_Rval(env,_tmp508->loc,
_tmp555,_tmp556);{struct Cyc_List_List _tmp55A=({struct Cyc_List_List _tmp561;
_tmp561.hd=_tmp50A;_tmp561.tl=0;_tmp561;});_tmp555=Cyc_NewControlFlow_add_vars(
_tmp4DA,_tmp555,(struct Cyc_List_List*)& _tmp55A,(void*)_tmp4DA->unknown_all,s->loc);
outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp555,_tmp50B);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp55B=outflow;struct Cyc_Dict_Dict _tmp55C;struct Cyc_List_List*_tmp55D;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp55E;_LL344: if((_tmp55B.BottomFL).tag != 0)goto _LL346;_LL345: goto _LL343;_LL346:
if((_tmp55B.ReachableFL).tag != 1)goto _LL343;_tmp55C=(_tmp55B.ReachableFL).f1;
_tmp55D=(_tmp55B.ReachableFL).f2;_tmp55E=(_tmp55B.ReachableFL).f3;_LL347: while(
_tmp559 != 0){struct Cyc_Dict_Dict _tmp55F=_tmp55E.consumed;_tmp55E.consumed=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))
Cyc_Dict_rdelete)(_tmp4DA->r,_tmp55E.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp559->hd);if((_tmp55E.consumed).t != _tmp55F.t);_tmp559=_tmp559->tl;}outflow=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp560;(
_tmp560.ReachableFL).tag=1;(_tmp560.ReachableFL).f1=_tmp55C;(_tmp560.ReachableFL).f2=
_tmp55D;(_tmp560.ReachableFL).f3=_tmp55E;_tmp560;});goto _LL343;_LL343:;}goto
_LL30C;}}}_LL337:;_LL338:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562[0]=({struct Cyc_Core_Impossible_struct
_tmp563;_tmp563.tag=Cyc_Core_Impossible;_tmp563.f1=({const char*_tmp564="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_dyneither(_tmp564,sizeof(char),56);});_tmp563;});_tmp562;}));_LL30C:;}
outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp565=outflow;struct Cyc_CfFlowInfo_ConsumeInfo _tmp566;_LL349: if((_tmp565.ReachableFL).tag
!= 1)goto _LL34B;_tmp566=(_tmp565.ReachableFL).f3;_LL34A: goto _LL348;_LL34B:;
_LL34C: goto _LL348;_LL348:;}return outflow;}}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*
fd);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct
Cyc_Absyn_Decl*decl){void*_tmp567=(void*)decl->r;struct Cyc_Absyn_Vardecl*_tmp568;
struct Cyc_Core_Opt*_tmp569;struct Cyc_Core_Opt _tmp56A;struct Cyc_List_List*_tmp56B;
struct Cyc_Absyn_Exp*_tmp56C;struct Cyc_List_List*_tmp56D;struct Cyc_Absyn_Fndecl*
_tmp56E;_LL34E: if(_tmp567 <= (void*)2)goto _LL356;if(*((int*)_tmp567)!= 0)goto
_LL350;_tmp568=((struct Cyc_Absyn_Var_d_struct*)_tmp567)->f1;_LL34F: {struct Cyc_List_List
_tmp56F=({struct Cyc_List_List _tmp57E;_tmp57E.hd=_tmp568;_tmp57E.tl=0;_tmp57E;});
inflow=Cyc_NewControlFlow_add_vars(env->fenv,inflow,(struct Cyc_List_List*)&
_tmp56F,(void*)(env->fenv)->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*_tmp570=
_tmp568->initializer;if(_tmp570 == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp572;void*_tmp573;struct _tuple5 _tmp571=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_tmp570);_tmp572=_tmp571.f1;_tmp573=_tmp571.f2;
_tmp572=Cyc_CfFlowInfo_consume_unique_rvals(_tmp570->loc,_tmp572);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp574=_tmp572;struct Cyc_Dict_Dict _tmp575;struct Cyc_List_List*_tmp576;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp577;_LL359: if((_tmp574.BottomFL).tag != 0)goto _LL35B;_LL35A: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp578;(_tmp578.BottomFL).tag=0;_tmp578;});
_LL35B: if((_tmp574.ReachableFL).tag != 1)goto _LL358;_tmp575=(_tmp574.ReachableFL).f1;
_tmp576=(_tmp574.ReachableFL).f2;_tmp577=(_tmp574.ReachableFL).f3;_LL35C: _tmp575=
Cyc_CfFlowInfo_assign_place(env->fenv,decl->loc,_tmp575,env->all_changed,({
struct Cyc_CfFlowInfo_Place*_tmp579=_region_malloc(env->r,sizeof(*_tmp579));
_tmp579->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp57A=
_region_malloc(env->r,sizeof(*_tmp57A));_tmp57A[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp57B;_tmp57B.tag=0;_tmp57B.f1=_tmp568;_tmp57B;});_tmp57A;}));_tmp579->fields=
0;_tmp579;}),_tmp573);_tmp576=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp576,
_tmp568,(struct Cyc_Absyn_Exp*)_check_null(_tmp568->initializer));{union Cyc_CfFlowInfo_FlowInfo_union
_tmp57C=({union Cyc_CfFlowInfo_FlowInfo_union _tmp57D;(_tmp57D.ReachableFL).tag=1;(
_tmp57D.ReachableFL).f1=_tmp575;(_tmp57D.ReachableFL).f2=_tmp576;(_tmp57D.ReachableFL).f3=
_tmp577;_tmp57D;});Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp57C);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp57C;}_LL358:;}}}}_LL350:
if(*((int*)_tmp567)!= 2)goto _LL352;_tmp569=((struct Cyc_Absyn_Let_d_struct*)
_tmp567)->f2;if(_tmp569 == 0)goto _LL352;_tmp56A=*_tmp569;_tmp56B=(struct Cyc_List_List*)
_tmp56A.v;_tmp56C=((struct Cyc_Absyn_Let_d_struct*)_tmp567)->f3;_LL351: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp580;void*_tmp581;struct _tuple5 _tmp57F=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp56C);_tmp580=_tmp57F.f1;_tmp581=_tmp57F.f2;_tmp580=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp56C->loc,_tmp580);_tmp580=Cyc_NewControlFlow_use_Rval(env,_tmp56C->loc,
_tmp580,_tmp581);return Cyc_NewControlFlow_add_vars(env->fenv,_tmp580,_tmp56B,(
void*)(env->fenv)->unknown_all,decl->loc);}_LL352: if(*((int*)_tmp567)!= 3)goto
_LL354;_tmp56D=((struct Cyc_Absyn_Letv_d_struct*)_tmp567)->f1;_LL353: return Cyc_NewControlFlow_add_vars(
env->fenv,inflow,_tmp56D,(void*)(env->fenv)->unknown_none,decl->loc);_LL354: if(*((
int*)_tmp567)!= 1)goto _LL356;_tmp56E=((struct Cyc_Absyn_Fn_d_struct*)_tmp567)->f1;
_LL355: Cyc_NewControlFlow_check_nested_fun(env->fenv,inflow,_tmp56E);{void*
_tmp582=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp56E->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp583=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp56E->fn_vardecl);
return Cyc_NewControlFlow_add_vars(env->fenv,inflow,({struct Cyc_List_List*_tmp584=
_region_malloc(env->r,sizeof(*_tmp584));_tmp584->hd=_tmp583;_tmp584->tl=0;
_tmp584;}),(void*)(env->fenv)->unknown_all,decl->loc);}_LL356:;_LL357:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp585=_cycalloc(sizeof(*
_tmp585));_tmp585[0]=({struct Cyc_Core_Impossible_struct _tmp586;_tmp586.tag=Cyc_Core_Impossible;
_tmp586.f1=({const char*_tmp587="anal_decl: unexpected decl variant";
_tag_dyneither(_tmp587,sizeof(char),35);});_tmp586;});_tmp585;}));_LL34D:;}
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle _tmp588=_new_region("frgn");struct _RegionHandle*frgn=& _tmp588;
_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);Cyc_NewControlFlow_check_nested_fun(fenv,(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp589;(_tmp589.ReachableFL).tag=1;(_tmp589.ReachableFL).f1=
fenv->mt_flowdict;(_tmp589.ReachableFL).f2=0;(_tmp589.ReachableFL).f3=({struct
Cyc_CfFlowInfo_ConsumeInfo _tmp58A;_tmp58A.consumed=fenv->mt_place_set;_tmp58A.may_consume=
0;_tmp58A;});_tmp589;}),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(
void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp58B=fenv->r;struct Cyc_Position_Segment*_tmp58C=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(void*)fenv->unknown_all,_tmp58C);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo_union _tmp58D=inflow;struct Cyc_Dict_Dict
_tmp58E;struct Cyc_List_List*_tmp58F;struct Cyc_CfFlowInfo_ConsumeInfo _tmp590;
_LL35E: if((_tmp58D.BottomFL).tag != 0)goto _LL360;_LL35F:({void*_tmp591=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp592="check_fun";_tag_dyneither(_tmp592,sizeof(char),10);}),_tag_dyneither(
_tmp591,sizeof(void*),0));});_LL360: if((_tmp58D.ReachableFL).tag != 1)goto _LL35D;
_tmp58E=(_tmp58D.ReachableFL).f1;_tmp58F=(_tmp58D.ReachableFL).f2;_tmp590=(
_tmp58D.ReachableFL).f3;_LL361: {struct Cyc_List_List*atts;{void*_tmp593=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo
_tmp594;struct Cyc_List_List*_tmp595;_LL363: if(_tmp593 <= (void*)4)goto _LL365;if(*((
int*)_tmp593)!= 8)goto _LL365;_tmp594=((struct Cyc_Absyn_FnType_struct*)_tmp593)->f1;
_tmp595=_tmp594.attributes;_LL364: atts=_tmp595;goto _LL362;_LL365:;_LL366:({void*
_tmp596=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp597="check_fun: non-function type cached with fndecl_t";
_tag_dyneither(_tmp597,sizeof(char),50);}),_tag_dyneither(_tmp596,sizeof(void*),
0));});_LL362:;}for(0;atts != 0;atts=atts->tl){void*_tmp598=(void*)atts->hd;int
_tmp599;_LL368: if(_tmp598 <= (void*)17)goto _LL36A;if(*((int*)_tmp598)!= 4)goto
_LL36A;_tmp599=((struct Cyc_Absyn_Initializes_att_struct*)_tmp598)->f1;_LL369: {
unsigned int j=(unsigned int)_tmp599;if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp59A=0;Cyc_Tcutil_terr(_tmp58C,({const char*_tmp59B="initializes attribute exceeds number of parameters";
_tag_dyneither(_tmp59B,sizeof(char),51);}),_tag_dyneither(_tmp59A,sizeof(void*),
0));});continue;}{struct Cyc_Absyn_Vardecl*_tmp59C=((struct Cyc_Absyn_Vardecl*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp59D=Cyc_Tcutil_compress((
void*)_tmp59C->type);struct Cyc_Absyn_PtrInfo _tmp59E;void*_tmp59F;struct Cyc_Absyn_PtrAtts
_tmp5A0;struct Cyc_Absyn_Conref*_tmp5A1;struct Cyc_Absyn_Conref*_tmp5A2;struct Cyc_Absyn_Conref*
_tmp5A3;_LL36D: if(_tmp59D <= (void*)4)goto _LL36F;if(*((int*)_tmp59D)!= 4)goto
_LL36F;_tmp59E=((struct Cyc_Absyn_PointerType_struct*)_tmp59D)->f1;_tmp59F=(void*)
_tmp59E.elt_typ;_tmp5A0=_tmp59E.ptr_atts;_tmp5A1=_tmp5A0.nullable;_tmp5A2=
_tmp5A0.bounds;_tmp5A3=_tmp5A0.zero_term;_LL36E: if(((int(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_val)(_tmp5A1))({void*_tmp5A4=0;Cyc_Tcutil_terr(_tmp58C,({
const char*_tmp5A5="initializes attribute not allowed on nullable pointers";
_tag_dyneither(_tmp5A5,sizeof(char),55);}),_tag_dyneither(_tmp5A4,sizeof(void*),
0));});if(!Cyc_Tcutil_is_bound_one(_tmp5A2))({void*_tmp5A6=0;Cyc_Tcutil_terr(
_tmp58C,({const char*_tmp5A7="initializes attribute allowed only on pointers of size 1";
_tag_dyneither(_tmp5A7,sizeof(char),57);}),_tag_dyneither(_tmp5A6,sizeof(void*),
0));});if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp5A3))({
void*_tmp5A8=0;Cyc_Tcutil_terr(_tmp58C,({const char*_tmp5A9="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_dyneither(_tmp5A9,sizeof(char),77);}),_tag_dyneither(_tmp5A8,sizeof(void*),
0));});{struct Cyc_CfFlowInfo_InitParam_struct*_tmp5AA=({struct Cyc_CfFlowInfo_InitParam_struct*
_tmp5B2=_region_malloc(_tmp58B,sizeof(*_tmp5B2));_tmp5B2[0]=({struct Cyc_CfFlowInfo_InitParam_struct
_tmp5B3;_tmp5B3.tag=2;_tmp5B3.f1=(int)j;_tmp5B3.f2=(void*)_tmp59F;_tmp5B3;});
_tmp5B2;});struct Cyc_CfFlowInfo_Place*_tmp5AB=({struct Cyc_CfFlowInfo_Place*
_tmp5B1=_region_malloc(_tmp58B,sizeof(*_tmp5B1));_tmp5B1->root=(void*)((void*)
_tmp5AA);_tmp5B1->fields=0;_tmp5B1;});_tmp58E=Cyc_Dict_insert(_tmp58E,(void*)
_tmp5AA,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp59F,(void*)fenv->esc_none));
_tmp58E=Cyc_Dict_insert(_tmp58E,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp5AC=_region_malloc(_tmp58B,sizeof(*_tmp5AC));_tmp5AC[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp5AD;_tmp5AD.tag=0;_tmp5AD.f1=_tmp59C;_tmp5AD;});_tmp5AC;}),(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp5AE=_region_malloc(_tmp58B,sizeof(*_tmp5AE));
_tmp5AE[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp5AF;_tmp5AF.tag=2;_tmp5AF.f1=
_tmp5AB;_tmp5AF;});_tmp5AE;}));param_roots=({struct Cyc_List_List*_tmp5B0=
_region_malloc(_tmp58B,sizeof(*_tmp5B0));_tmp5B0->hd=_tmp5AB;_tmp5B0->tl=
param_roots;_tmp5B0;});goto _LL36C;}_LL36F:;_LL370:({void*_tmp5B4=0;Cyc_Tcutil_terr(
_tmp58C,({const char*_tmp5B5="initializes attribute on non-pointer";
_tag_dyneither(_tmp5B5,sizeof(char),37);}),_tag_dyneither(_tmp5B4,sizeof(void*),
0));});_LL36C:;}goto _LL367;}}_LL36A:;_LL36B: goto _LL367;_LL367:;}inflow=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp5B6;(_tmp5B6.ReachableFL).tag=1;(_tmp5B6.ReachableFL).f1=
_tmp58E;(_tmp5B6.ReachableFL).f2=_tmp58F;(_tmp5B6.ReachableFL).f3=_tmp590;
_tmp5B6;});}_LL35D:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp58B,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp5BF=_region_malloc(_tmp58B,sizeof(*_tmp5BF));_tmp5BF->r=_tmp58B;_tmp5BF->fenv=
fenv;_tmp5BF->iterate_again=1;_tmp5BF->iteration_num=0;_tmp5BF->in_try=0;_tmp5BF->tryflow=
inflow;_tmp5BF->all_changed=0;_tmp5BF->noreturn=noreturn;_tmp5BF->param_roots=
param_roots;_tmp5BF->flow_table=flow_table;_tmp5BF;});union Cyc_CfFlowInfo_FlowInfo_union
outflow=inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5B7=outflow;_LL372: if((_tmp5B7.BottomFL).tag != 0)goto _LL374;_LL373: goto
_LL371;_LL374:;_LL375: Cyc_NewControlFlow_check_init_params(_tmp58C,env,outflow);
if(noreturn)({void*_tmp5B8=0;Cyc_Tcutil_terr(_tmp58C,({const char*_tmp5B9="`noreturn' function might (implicitly) return";
_tag_dyneither(_tmp5B9,sizeof(char),46);}),_tag_dyneither(_tmp5B8,sizeof(void*),
0));});else{void*_tmp5BA=Cyc_Tcutil_compress((void*)fd->ret_type);_LL377: if((int)
_tmp5BA != 0)goto _LL379;_LL378: goto _LL376;_LL379: if(_tmp5BA <= (void*)4)goto _LL37B;
if(*((int*)_tmp5BA)!= 6)goto _LL37B;_LL37A: goto _LL37C;_LL37B: if((int)_tmp5BA != 1)
goto _LL37D;_LL37C: goto _LL37E;_LL37D: if(_tmp5BA <= (void*)4)goto _LL37F;if(*((int*)
_tmp5BA)!= 5)goto _LL37F;_LL37E:({void*_tmp5BB=0;Cyc_Tcutil_warn(_tmp58C,({const
char*_tmp5BC="function may not return a value";_tag_dyneither(_tmp5BC,sizeof(
char),32);}),_tag_dyneither(_tmp5BB,sizeof(void*),0));});goto _LL376;_LL37F:;
_LL380:({void*_tmp5BD=0;Cyc_Tcutil_terr(_tmp58C,({const char*_tmp5BE="function may not return a value";
_tag_dyneither(_tmp5BE,sizeof(char),32);}),_tag_dyneither(_tmp5BD,sizeof(void*),
0));});goto _LL376;_LL376:;}goto _LL371;_LL371:;}}}}void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*_tmp5C0=(void*)((struct Cyc_Absyn_Decl*)
ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp5C1;struct Cyc_List_List*_tmp5C2;struct Cyc_List_List*
_tmp5C3;struct Cyc_List_List*_tmp5C4;_LL382: if(_tmp5C0 <= (void*)2)goto _LL38C;if(*((
int*)_tmp5C0)!= 1)goto _LL384;_tmp5C1=((struct Cyc_Absyn_Fn_d_struct*)_tmp5C0)->f1;
_LL383: Cyc_NewControlFlow_check_fun(_tmp5C1);goto _LL381;_LL384: if(*((int*)
_tmp5C0)!= 10)goto _LL386;_tmp5C2=((struct Cyc_Absyn_ExternC_d_struct*)_tmp5C0)->f1;
_LL385: _tmp5C3=_tmp5C2;goto _LL387;_LL386: if(*((int*)_tmp5C0)!= 9)goto _LL388;
_tmp5C3=((struct Cyc_Absyn_Using_d_struct*)_tmp5C0)->f2;_LL387: _tmp5C4=_tmp5C3;
goto _LL389;_LL388: if(*((int*)_tmp5C0)!= 8)goto _LL38A;_tmp5C4=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp5C0)->f2;_LL389: Cyc_NewControlFlow_cf_check(_tmp5C4);goto _LL381;_LL38A: if(*((
int*)_tmp5C0)!= 11)goto _LL38C;_LL38B: goto _LL381;_LL38C:;_LL38D: goto _LL381;_LL381:;}}
