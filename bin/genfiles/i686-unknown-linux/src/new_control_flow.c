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
f1;};struct Cyc_Absyn_Region_d_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
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
struct _RegionHandle*,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*,void*t);int Cyc_Tcutil_is_noreturn(void*);struct _tuple4{
unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};
struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
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
void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(union
Cyc_CfFlowInfo_FlowInfo_union f1,union Cyc_CfFlowInfo_FlowInfo_union f2);void Cyc_CfFlowInfo_print_place(
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
fenv,outflow,0);_tmp20=_tmp1F.f1;_tmp21=_tmp20.consumed;{struct _RegionHandle*
_tmp22=fenv->r;{struct Cyc_Iter_Iter _tmp23=((struct Cyc_Iter_Iter(*)(struct
_RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(_tmp22,*((struct Cyc_Dict_Dict*)
_check_null(out_unique_places)));struct _tuple10 _tmp24=*((struct _tuple10*(*)(
struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(_tmp22,*
out_unique_places);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple10*))Cyc_Iter_next)(
_tmp23,& _tmp24)){struct Cyc_CfFlowInfo_Place*_tmp25=_tmp24.f1;if(!((int(*)(struct
Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp21,_tmp25))({
struct Cyc_String_pa_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_CfFlowInfo_place_err_string(_tmp25));{void*_tmp26[1]={&
_tmp28};Cyc_Tcutil_terr(_tmp24.f2,({const char*_tmp27="Failed to consume unique variable %s";
_tag_dyneither(_tmp27,sizeof(char),37);}),_tag_dyneither(_tmp26,sizeof(void*),1));}});}}*
out_unique_places=old_unique_places;}}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo_union inflow,void*r){union Cyc_CfFlowInfo_FlowInfo_union
_tmp29=inflow;struct Cyc_Dict_Dict _tmp2A;struct Cyc_List_List*_tmp2B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2C;_LLB: if((_tmp29.BottomFL).tag != 0)goto _LLD;_LLC: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp2D;(_tmp2D.BottomFL).tag=0;_tmp2D;});_LLD:
if((_tmp29.ReachableFL).tag != 1)goto _LLA;_tmp2A=(_tmp29.ReachableFL).f1;_tmp2B=(
_tmp29.ReachableFL).f2;_tmp2C=(_tmp29.ReachableFL).f3;_LLE: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp2A,r)!= (void*)2)({void*_tmp2E=0;Cyc_Tcutil_terr(loc,({const char*
_tmp2F="expression may not be fully initialized";_tag_dyneither(_tmp2F,sizeof(
char),40);}),_tag_dyneither(_tmp2E,sizeof(void*),0));});{struct Cyc_Dict_Dict
_tmp30=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp2A,env->all_changed,r);if(
_tmp2A.t == _tmp30.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union _tmp31=({
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
Cyc_List_List*es,int arg1_unconsumed){struct _RegionHandle*_tmp5D=env->r;union Cyc_CfFlowInfo_FlowInfo_union
_tmp5F;struct Cyc_List_List*_tmp60;struct _tuple11 _tmp5E=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp5D,env,inflow,es,arg1_unconsumed);_tmp5F=_tmp5E.f1;_tmp60=_tmp5E.f2;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp61=_tmp5F;struct Cyc_Dict_Dict _tmp62;_LL15: if((
_tmp61.ReachableFL).tag != 1)goto _LL17;_tmp62=(_tmp61.ReachableFL).f1;_LL16: for(0;
_tmp60 != 0;(_tmp60=_tmp60->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp62,(void*)_tmp60->hd)== (void*)0)({
void*_tmp63=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,({const char*_tmp64="expression may not be initialized";
_tag_dyneither(_tmp64,sizeof(char),34);}),_tag_dyneither(_tmp63,sizeof(void*),0));});}
goto _LL14;_LL17: if((_tmp61.BottomFL).tag != 0)goto _LL14;_LL18: goto _LL14;_LL14:;}
return({struct _tuple5 _tmp65;_tmp65.f1=_tmp5F;_tmp65.f2=(void*)(env->fenv)->unknown_all;
_tmp65;});}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_notzero(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Absyn_Exp*e,void*il){union
Cyc_CfFlowInfo_FlowInfo_union _tmp66=outflow;struct Cyc_Dict_Dict _tmp67;struct Cyc_List_List*
_tmp68;struct Cyc_CfFlowInfo_ConsumeInfo _tmp69;_LL1A: if((_tmp66.BottomFL).tag != 0)
goto _LL1C;_LL1B: return outflow;_LL1C: if((_tmp66.ReachableFL).tag != 1)goto _LL19;
_tmp67=(_tmp66.ReachableFL).f1;_tmp68=(_tmp66.ReachableFL).f2;_tmp69=(_tmp66.ReachableFL).f3;
_LL1D: {union Cyc_CfFlowInfo_AbsLVal_union _tmp6A=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp6B;_LL1F: if((_tmp6A.UnknownL).tag
!= 1)goto _LL21;_LL20: return outflow;_LL21: if((_tmp6A.PlaceL).tag != 0)goto _LL1E;
_tmp6B=(_tmp6A.PlaceL).f1;_LL22: {void*nzval=il == (void*)2?(void*)1:(void*)2;
union Cyc_CfFlowInfo_FlowInfo_union _tmp6C=({union Cyc_CfFlowInfo_FlowInfo_union
_tmp6D;(_tmp6D.ReachableFL).tag=1;(_tmp6D.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp67,env->all_changed,_tmp6B,nzval);(_tmp6D.ReachableFL).f2=
_tmp68;(_tmp6D.ReachableFL).f3=_tmp69;_tmp6D;});return(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp6C;}_LL1E:;}_LL19:;}static struct _tuple8 Cyc_NewControlFlow_splitzero(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Absyn_Exp*e,void*il){union Cyc_CfFlowInfo_FlowInfo_union _tmp6E=
outflow;struct Cyc_Dict_Dict _tmp6F;struct Cyc_List_List*_tmp70;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp71;_LL24: if((_tmp6E.BottomFL).tag != 0)goto _LL26;_LL25: return({struct _tuple8
_tmp72;_tmp72.f1=outflow;_tmp72.f2=outflow;_tmp72;});_LL26: if((_tmp6E.ReachableFL).tag
!= 1)goto _LL23;_tmp6F=(_tmp6E.ReachableFL).f1;_tmp70=(_tmp6E.ReachableFL).f2;
_tmp71=(_tmp6E.ReachableFL).f3;_LL27: {union Cyc_CfFlowInfo_AbsLVal_union _tmp73=(
Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp74;
_LL29: if((_tmp73.UnknownL).tag != 1)goto _LL2B;_LL2A: return({struct _tuple8 _tmp75;
_tmp75.f1=outflow;_tmp75.f2=outflow;_tmp75;});_LL2B: if((_tmp73.PlaceL).tag != 0)
goto _LL28;_tmp74=(_tmp73.PlaceL).f1;_LL2C: {void*nzval=il == (void*)2?(void*)1:(
void*)2;return({struct _tuple8 _tmp76;_tmp76.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp78;(_tmp78.ReachableFL).tag=1;(_tmp78.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp6F,env->all_changed,_tmp74,
nzval);(_tmp78.ReachableFL).f2=_tmp70;(_tmp78.ReachableFL).f3=_tmp71;_tmp78;});
_tmp76.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp77;(_tmp77.ReachableFL).tag=1;(_tmp77.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp6F,env->all_changed,_tmp74,(void*)0);(_tmp77.ReachableFL).f2=
_tmp70;(_tmp77.ReachableFL).f3=_tmp71;_tmp77;});_tmp76;});}_LL28:;}_LL23:;}
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_if_tagcmp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union flow,struct Cyc_Absyn_Exp*e1,void*r1,void*p,
void*r2){union Cyc_CfFlowInfo_FlowInfo_union _tmp79=flow;struct Cyc_Dict_Dict _tmp7A;
struct Cyc_List_List*_tmp7B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp7C;_LL2E: if((
_tmp79.BottomFL).tag != 0)goto _LL30;_LL2F: return flow;_LL30: if((_tmp79.ReachableFL).tag
!= 1)goto _LL2D;_tmp7A=(_tmp79.ReachableFL).f1;_tmp7B=(_tmp79.ReachableFL).f2;
_tmp7C=(_tmp79.ReachableFL).f3;_LL31: {void*_tmp7D=r2;struct Cyc_List_List*_tmp7E;
_LL33: if(_tmp7D <= (void*)3)goto _LL35;if(*((int*)_tmp7D)!= 3)goto _LL35;_tmp7E=((
struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp7D)->f1;_LL34: {union Cyc_CfFlowInfo_AbsLVal_union
_tmp7F=(Cyc_NewControlFlow_anal_Lexp(env,flow,e1)).f2;struct Cyc_CfFlowInfo_Place*
_tmp80;_LL38: if((_tmp7F.UnknownL).tag != 1)goto _LL3A;_LL39: return flow;_LL3A: if((
_tmp7F.PlaceL).tag != 0)goto _LL37;_tmp80=(_tmp7F.PlaceL).f1;_LL3B: {struct Cyc_List_List*
new_cl;{void*_tmp81=r1;struct Cyc_List_List*_tmp82;void*_tmp83;_LL3D: if(_tmp81 <= (
void*)3)goto _LL41;if(*((int*)_tmp81)!= 3)goto _LL3F;_tmp82=((struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp81)->f1;_LL3E: new_cl=_tmp82;goto _LL3C;_LL3F: if(*((int*)_tmp81)!= 0)goto _LL41;
_tmp83=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp81)->f1;if((int)_tmp83
!= 2)goto _LL41;_LL40: goto _LL42;_LL41: if((int)_tmp81 != 0)goto _LL43;_LL42: goto
_LL44;_LL43: if((int)_tmp81 != 1)goto _LL45;_LL44: new_cl=0;goto _LL3C;_LL45:;_LL46:
return flow;_LL3C:;}for(0;_tmp7E != 0;_tmp7E=_tmp7E->tl){void*new_cmp;{struct
_tuple0 _tmp85=({struct _tuple0 _tmp84;_tmp84.f1=p;_tmp84.f2=(void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp7E->hd)->cmp;_tmp84;});void*_tmp86;void*_tmp87;void*_tmp88;void*_tmp89;_LL48:
_tmp86=_tmp85.f1;if((int)_tmp86 != 8)goto _LL4A;_LL49: goto _LL4B;_LL4A: _tmp87=
_tmp85.f2;if((int)_tmp87 != 8)goto _LL4C;_LL4B: new_cmp=(void*)8;goto _LL47;_LL4C:
_tmp88=_tmp85.f1;if((int)_tmp88 != 5)goto _LL4E;_tmp89=_tmp85.f2;if((int)_tmp89 != 
5)goto _LL4E;_LL4D: new_cmp=(void*)5;goto _LL47;_LL4E:;_LL4F: new_cmp=(void*)10;goto
_LL47;_LL47:;}new_cl=({struct Cyc_List_List*_tmp8A=_region_malloc(env->r,sizeof(*
_tmp8A));_tmp8A->hd=({struct Cyc_CfFlowInfo_TagCmp*_tmp8B=_region_malloc(env->r,
sizeof(*_tmp8B));_tmp8B->cmp=(void*)new_cmp;_tmp8B->bd=(void*)((void*)((struct
Cyc_CfFlowInfo_TagCmp*)_tmp7E->hd)->bd);_tmp8B;});_tmp8A->tl=new_cl;_tmp8A;});}
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp8C;(_tmp8C.ReachableFL).tag=1;(_tmp8C.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e1->loc,_tmp7A,env->all_changed,_tmp80,(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp8D=_region_malloc(env->r,sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmp8E;_tmp8E.tag=3;_tmp8E.f1=new_cl;_tmp8E;});_tmp8D;}));(_tmp8C.ReachableFL).f2=
_tmp7B;(_tmp8C.ReachableFL).f3=_tmp7C;_tmp8C;});}_LL37:;}_LL35:;_LL36: return flow;
_LL32:;}_LL2D:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct Cyc_NewControlFlow_mt_unknownz_v={
Cyc_CfFlowInfo_UnknownZ,0};static struct _tuple5 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp91=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp92;void*_tmp93;struct Cyc_Absyn_PtrAtts _tmp94;struct
Cyc_Absyn_Conref*_tmp95;struct Cyc_Absyn_Conref*_tmp96;_LL51: if(_tmp91 <= (void*)4)
goto _LL53;if(*((int*)_tmp91)!= 4)goto _LL53;_tmp92=((struct Cyc_Absyn_PointerType_struct*)
_tmp91)->f1;_tmp93=(void*)_tmp92.elt_typ;_tmp94=_tmp92.ptr_atts;_tmp95=_tmp94.bounds;
_tmp96=_tmp94.zero_term;_LL52: {union Cyc_CfFlowInfo_FlowInfo_union _tmp97=f;
struct Cyc_Dict_Dict _tmp98;struct Cyc_List_List*_tmp99;_LL56: if((_tmp97.BottomFL).tag
!= 0)goto _LL58;_LL57: return({struct _tuple5 _tmp9A;_tmp9A.f1=f;_tmp9A.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp93,(void*)(env->fenv)->unknown_all);_tmp9A;});_LL58: if((_tmp97.ReachableFL).tag
!= 1)goto _LL55;_tmp98=(_tmp97.ReachableFL).f1;_tmp99=(_tmp97.ReachableFL).f2;
_LL59: if(Cyc_Tcutil_is_bound_one(_tmp95)){void*_tmp9B=r;struct Cyc_CfFlowInfo_Place*
_tmp9C;void*_tmp9D;_LL5B: if((int)_tmp9B != 1)goto _LL5D;_LL5C: goto _LL5E;_LL5D: if((
int)_tmp9B != 2)goto _LL5F;_LL5E:{void*_tmp9E=(void*)e->annot;struct Cyc_List_List*
_tmp9F;_LL68: if(*((void**)_tmp9E)!= Cyc_CfFlowInfo_NotZero)goto _LL6A;_tmp9F=((
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp9E)->f1;_LL69: if(!Cyc_CfFlowInfo_same_relns(
_tmp99,_tmp9F))goto _LL6B;goto _LL67;_LL6A:;_LL6B:{void*_tmpA0=(void*)e->r;_LL6D:
if(*((int*)_tmpA0)!= 25)goto _LL6F;_LL6E:(void*)(e->annot=(void*)((void*)({struct
Cyc_CfFlowInfo_NotZero_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1[0]=({
struct Cyc_CfFlowInfo_NotZero_struct _tmpA2;_tmpA2.tag=Cyc_CfFlowInfo_NotZero;
_tmpA2.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp99);_tmpA2;});
_tmpA1;})));goto _LL6C;_LL6F:;_LL70:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_notzero_v));
goto _LL6C;_LL6C:;}goto _LL67;_LL67:;}goto _LL5A;_LL5F: if(_tmp9B <= (void*)3)goto
_LL61;if(*((int*)_tmp9B)!= 2)goto _LL61;_tmp9C=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp9B)->f1;_LL60:{void*_tmpA3=(void*)e->annot;struct Cyc_List_List*_tmpA4;_LL72:
if(*((void**)_tmpA3)!= Cyc_CfFlowInfo_NotZero)goto _LL74;_tmpA4=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpA3)->f1;_LL73: if(!Cyc_CfFlowInfo_same_relns(_tmp99,_tmpA4))goto _LL75;goto
_LL71;_LL74:;_LL75:{void*_tmpA5=(void*)e->r;_LL77: if(*((int*)_tmpA5)!= 25)goto
_LL79;_LL78:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmpA7;_tmpA7.tag=Cyc_CfFlowInfo_NotZero;_tmpA7.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp99);_tmpA7;});_tmpA6;})));goto _LL76;_LL79:;_LL7A:(void*)(e->annot=(void*)((
void*)& Cyc_NewControlFlow_mt_notzero_v));goto _LL76;_LL76:;}goto _LL71;_LL71:;}
return({struct _tuple5 _tmpA8;_tmpA8.f1=f;_tmpA8.f2=Cyc_CfFlowInfo_lookup_place(
_tmp98,_tmp9C);_tmpA8;});_LL61: if((int)_tmp9B != 0)goto _LL63;_LL62:(void*)(e->annot=(
void*)((void*)Cyc_CfFlowInfo_IsZero));return({struct _tuple5 _tmpA9;_tmpA9.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmpAA;(
_tmpAA.BottomFL).tag=0;_tmpAA;});_tmpA9.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,
_tmp93,(void*)(env->fenv)->unknown_all);_tmpA9;});_LL63: if(_tmp9B <= (void*)3)
goto _LL65;if(*((int*)_tmp9B)!= 0)goto _LL65;_tmp9D=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp9B)->f1;_LL64: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp9D);goto _LL66;
_LL65:;_LL66:{void*_tmpAB=(void*)e->r;_LL7C: if(*((int*)_tmpAB)!= 25)goto _LL7E;
_LL7D:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmpAD;_tmpAD.tag=Cyc_CfFlowInfo_UnknownZ;_tmpAD.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp99);_tmpAD;});_tmpAC;})));goto _LL7B;_LL7E:;_LL7F:(void*)(
e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_unknownz_v));goto _LL7B;_LL7B:;}
goto _LL5A;_LL5A:;}else{void*_tmpAE=(void*)e->annot;struct Cyc_List_List*_tmpAF;
_LL81: if(*((void**)_tmpAE)!= Cyc_CfFlowInfo_UnknownZ)goto _LL83;_tmpAF=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmpAE)->f1;_LL82: if(!Cyc_CfFlowInfo_same_relns(
_tmp99,_tmpAF))goto _LL84;goto _LL80;_LL83:;_LL84:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _tmpB1;_tmpB1.tag=Cyc_CfFlowInfo_UnknownZ;
_tmpB1.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp99);_tmpB1;});
_tmpB0;})));goto _LL80;_LL80:;}{void*_tmpB2=Cyc_CfFlowInfo_initlevel(env->fenv,
_tmp98,r);_LL86: if((int)_tmpB2 != 0)goto _LL88;_LL87:({void*_tmpB3=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmpB4="dereference of possibly uninitialized pointer";
_tag_dyneither(_tmpB4,sizeof(char),46);}),_tag_dyneither(_tmpB3,sizeof(void*),0));});
goto _LL89;_LL88: if((int)_tmpB2 != 2)goto _LL8A;_LL89: return({struct _tuple5 _tmpB5;
_tmpB5.f1=f;_tmpB5.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp93,(void*)(env->fenv)->unknown_all);
_tmpB5;});_LL8A: if((int)_tmpB2 != 1)goto _LL85;_LL8B: return({struct _tuple5 _tmpB6;
_tmpB6.f1=f;_tmpB6.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp93,(void*)(env->fenv)->unknown_none);
_tmpB6;});_LL85:;}_LL55:;}_LL53:;_LL54:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Core_Impossible_struct
_tmpB8;_tmpB8.tag=Cyc_Core_Impossible;_tmpB8.f1=({const char*_tmpB9="right deref of non-pointer-type";
_tag_dyneither(_tmpB9,sizeof(char),32);});_tmpB8;});_tmpB7;}));_LL50:;}static
struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*
rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmpBA=(void*)e1->r;void*_tmpBB;struct Cyc_Absyn_Vardecl*_tmpBC;void*_tmpBD;
struct Cyc_Absyn_Vardecl*_tmpBE;void*_tmpBF;struct Cyc_Absyn_Vardecl*_tmpC0;void*
_tmpC1;struct Cyc_Absyn_Vardecl*_tmpC2;_LL8D: if(*((int*)_tmpBA)!= 1)goto _LL8F;
_tmpBB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpBB <= (void*)1)
goto _LL8F;if(*((int*)_tmpBB)!= 4)goto _LL8F;_tmpBC=((struct Cyc_Absyn_Pat_b_struct*)
_tmpBB)->f1;_LL8E: _tmpBE=_tmpBC;goto _LL90;_LL8F: if(*((int*)_tmpBA)!= 1)goto _LL91;
_tmpBD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpBD <= (void*)1)
goto _LL91;if(*((int*)_tmpBD)!= 3)goto _LL91;_tmpBE=((struct Cyc_Absyn_Local_b_struct*)
_tmpBD)->f1;_LL90: _tmpC0=_tmpBE;goto _LL92;_LL91: if(*((int*)_tmpBA)!= 1)goto _LL93;
_tmpBF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpBF <= (void*)1)
goto _LL93;if(*((int*)_tmpBF)!= 2)goto _LL93;_tmpC0=((struct Cyc_Absyn_Param_b_struct*)
_tmpBF)->f1;_LL92: _tmpC2=_tmpC0;goto _LL94;_LL93: if(*((int*)_tmpBA)!= 1)goto _LL95;
_tmpC1=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpC1 <= (void*)1)
goto _LL95;if(*((int*)_tmpC1)!= 0)goto _LL95;_tmpC2=((struct Cyc_Absyn_Global_b_struct*)
_tmpC1)->f1;_LL94: if(!_tmpC2->escapes){void*_tmpC3=(void*)e2->r;void*_tmpC4;
struct Cyc_Absyn_Vardecl*_tmpC5;void*_tmpC6;struct Cyc_Absyn_Vardecl*_tmpC7;void*
_tmpC8;struct Cyc_Absyn_Vardecl*_tmpC9;void*_tmpCA;struct Cyc_Absyn_Vardecl*_tmpCB;
_LL98: if(*((int*)_tmpC3)!= 1)goto _LL9A;_tmpC4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpC4 <= (void*)1)goto _LL9A;if(*((int*)_tmpC4)!= 4)goto _LL9A;
_tmpC5=((struct Cyc_Absyn_Pat_b_struct*)_tmpC4)->f1;_LL99: _tmpC7=_tmpC5;goto _LL9B;
_LL9A: if(*((int*)_tmpC3)!= 1)goto _LL9C;_tmpC6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpC6 <= (void*)1)goto _LL9C;if(*((int*)_tmpC6)!= 3)goto _LL9C;
_tmpC7=((struct Cyc_Absyn_Local_b_struct*)_tmpC6)->f1;_LL9B: _tmpC9=_tmpC7;goto
_LL9D;_LL9C: if(*((int*)_tmpC3)!= 1)goto _LL9E;_tmpC8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpC8 <= (void*)1)goto _LL9E;if(*((int*)_tmpC8)!= 2)goto _LL9E;
_tmpC9=((struct Cyc_Absyn_Param_b_struct*)_tmpC8)->f1;_LL9D: _tmpCB=_tmpC9;goto
_LL9F;_LL9E: if(*((int*)_tmpC3)!= 1)goto _LLA0;_tmpCA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpCA <= (void*)1)goto _LLA0;if(*((int*)_tmpCA)!= 0)goto _LLA0;
_tmpCB=((struct Cyc_Absyn_Global_b_struct*)_tmpCA)->f1;_LL9F: if(!_tmpCB->escapes){
int found=0;{struct Cyc_List_List*_tmpCC=relns;for(0;_tmpCC != 0;_tmpCC=_tmpCC->tl){
struct Cyc_CfFlowInfo_Reln*_tmpCD=(struct Cyc_CfFlowInfo_Reln*)_tmpCC->hd;if(
_tmpCD->vd == _tmpCB){union Cyc_CfFlowInfo_RelnOp_union _tmpCE=_tmpCD->rop;struct
Cyc_Absyn_Vardecl*_tmpCF;_LLA3: if((_tmpCE.LessNumelts).tag != 2)goto _LLA5;_tmpCF=(
_tmpCE.LessNumelts).f1;if(!(_tmpCF == _tmpC2))goto _LLA5;_LLA4: return relns;_LLA5:;
_LLA6: continue;_LLA2:;}}}if(!found)return({struct Cyc_List_List*_tmpD0=
_region_malloc(rgn,sizeof(*_tmpD0));_tmpD0->hd=({struct Cyc_CfFlowInfo_Reln*
_tmpD1=_region_malloc(rgn,sizeof(*_tmpD1));_tmpD1->vd=_tmpCB;_tmpD1->rop=(union
Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmpD2;(_tmpD2.LessNumelts).tag=
2;(_tmpD2.LessNumelts).f1=_tmpC2;_tmpD2;});_tmpD1;});_tmpD0->tl=relns;_tmpD0;});}
return relns;_LLA0:;_LLA1: return relns;_LL97:;}else{return relns;}_LL95:;_LL96:
return relns;_LL8C:;}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_may_consume_place(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,
struct Cyc_CfFlowInfo_Place*place,void*t,struct Cyc_List_List**ps){struct Cyc_CfFlowInfo_FlowEnv*
_tmpD3=env->fenv;struct Cyc_List_List*_tmpD4=Cyc_NewControlFlow_noalias_ptrs_rec(
env,place,t);if(ps != 0)*ps=_tmpD4;if(_tmpD4 == 0)return cinfo;cinfo.may_consume=((
struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(
_tmpD3->r,Cyc_CfFlowInfo_place_cmp,_tmpD4,cinfo.may_consume);return cinfo;}struct
_tuple12{union Cyc_CfFlowInfo_AbsLVal_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_may_consume_exp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct _RegionHandle*_tmpD5=env->r;if(Cyc_Tcutil_is_noalias_path(
_tmpD5,e)){struct Cyc_CfFlowInfo_FlowEnv*_tmpD6=env->fenv;union Cyc_CfFlowInfo_AbsLVal_union
_tmpD8;struct _tuple7 _tmpD7=Cyc_NewControlFlow_anal_Lexp(env,inflow,e);_tmpD8=
_tmpD7.f2;{struct _tuple12 _tmpDA=({struct _tuple12 _tmpD9;_tmpD9.f1=_tmpD8;_tmpD9.f2=
inflow;_tmpD9;});union Cyc_CfFlowInfo_AbsLVal_union _tmpDB;struct Cyc_CfFlowInfo_Place*
_tmpDC;union Cyc_CfFlowInfo_FlowInfo_union _tmpDD;struct Cyc_Dict_Dict _tmpDE;struct
Cyc_List_List*_tmpDF;struct Cyc_CfFlowInfo_ConsumeInfo _tmpE0;_LLA8: _tmpDB=_tmpDA.f1;
if(((_tmpDA.f1).PlaceL).tag != 0)goto _LLAA;_tmpDC=(_tmpDB.PlaceL).f1;_tmpDD=
_tmpDA.f2;if(((_tmpDA.f2).ReachableFL).tag != 1)goto _LLAA;_tmpDE=(_tmpDD.ReachableFL).f1;
_tmpDF=(_tmpDD.ReachableFL).f2;_tmpE0=(_tmpDD.ReachableFL).f3;_LLA9: {struct Cyc_CfFlowInfo_ConsumeInfo
_tmpE1=Cyc_NewControlFlow_may_consume_place(env,_tmpE0,_tmpDC,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
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
rval) && Cyc_CfFlowInfo_initlevel(env->fenv,outdict,rval)!= (void*)0) && rval != (
void*)0){void*_tmpEA=_tmpE8;struct Cyc_Absyn_Vardecl*_tmpEB;_LLAD: if(*((int*)
_tmpEA)!= 0)goto _LLAF;_tmpEB=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmpEA)->f1;
_LLAE: {struct _dyneither_ptr _tmpEC=Cyc_Absynpp_qvar2string(_tmpEB->name);if(
_tmpE9 == 0)({struct Cyc_String_pa_struct _tmpEF;_tmpEF.tag=0;_tmpEF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmpEC);{void*_tmpED[1]={& _tmpEF};Cyc_Tcutil_warn(
loc,({const char*_tmpEE="may clobber unique pointer %s";_tag_dyneither(_tmpEE,
sizeof(char),30);}),_tag_dyneither(_tmpED,sizeof(void*),1));}});else{({struct Cyc_String_pa_struct
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
outdict,struct Cyc_Absyn_Exp*e){struct _RegionHandle*_tmpF5=env->r;if(Cyc_Tcutil_is_noalias_path(
_tmpF5,e)){struct Cyc_CfFlowInfo_FlowEnv*_tmpF6=env->fenv;struct Cyc_List_List*
_tmpF7=0;cinfo=Cyc_NewControlFlow_may_consume_place(env,cinfo,p,(void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v,(struct Cyc_List_List**)& _tmpF7);while(
_tmpF7 != 0){cinfo=Cyc_NewControlFlow_consume_place(env,(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmpF7))->hd,cinfo,outdict,e->loc);_tmpF7=((
struct Cyc_List_List*)_check_null(_tmpF7))->tl;}}return cinfo;}static struct Cyc_List_List*
Cyc_NewControlFlow_noalias_ptrs_aux(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*ts){struct Cyc_List_List*l=0;struct Cyc_CfFlowInfo_Place
_tmpF9;void*_tmpFA;struct Cyc_List_List*_tmpFB;struct Cyc_CfFlowInfo_Place*_tmpF8=
p;_tmpF9=*_tmpF8;_tmpFA=(void*)_tmpF9.root;_tmpFB=_tmpF9.fields;{int fld=0;for(0;
ts != 0;(fld ++,ts=ts->tl)){void*_tmpFC=(void*)ts->hd;if(Cyc_Tcutil_is_noalias_pointer(
_tmpFC)){struct Cyc_List_List*_tmpFD=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
env->r,_tmpFB,({int _tmp100[1];_tmp100[0]=fld;((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(
_tmp100,sizeof(int),1));}));l=((struct Cyc_List_List*(*)(struct _RegionHandle*,int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmpFE=_region_malloc(env->r,sizeof(*_tmpFE));_tmpFE->hd=({
struct Cyc_CfFlowInfo_Place*_tmpFF=_region_malloc(env->r,sizeof(*_tmpFF));_tmpFF->root=(
void*)_tmpFA;_tmpFF->fields=_tmpFD;_tmpFF;});_tmpFE->tl=0;_tmpFE;}),l);}else{if(
Cyc_Absyn_is_aggr_type(_tmpFC)){struct Cyc_List_List*_tmp101=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
env->r,_tmpFB,({int _tmp104[1];_tmp104[0]=fld;((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(
_tmp104,sizeof(int),1));}));struct Cyc_CfFlowInfo_Place*_tmp102=({struct Cyc_CfFlowInfo_Place*
_tmp103=_region_malloc(env->r,sizeof(*_tmp103));_tmp103->root=(void*)_tmpFA;
_tmp103->fields=_tmp101;_tmp103;});l=((struct Cyc_List_List*(*)(struct
_RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,
l,Cyc_NewControlFlow_noalias_ptrs_rec(env,_tmp102,_tmpFC));}}}}return l;}struct
_tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t){if(Cyc_Tcutil_is_noalias_pointer(
t))return({struct Cyc_List_List*_tmp105=_region_malloc(env->r,sizeof(*_tmp105));
_tmp105->hd=p;_tmp105->tl=0;_tmp105;});{void*_tmp106=Cyc_Tcutil_compress(t);
struct Cyc_List_List*_tmp107;struct Cyc_List_List*_tmp108;struct Cyc_Absyn_AggrInfo
_tmp109;union Cyc_Absyn_AggrInfoU_union _tmp10A;struct Cyc_Absyn_Aggrdecl**_tmp10B;
struct Cyc_List_List*_tmp10C;struct Cyc_Absyn_AggrInfo _tmp10D;union Cyc_Absyn_AggrInfoU_union
_tmp10E;struct Cyc_Absyn_TunionFieldInfo _tmp10F;union Cyc_Absyn_TunionFieldInfoU_union
_tmp110;struct Cyc_List_List*_tmp111;_LLB2: if(_tmp106 <= (void*)4)goto _LLBE;if(*((
int*)_tmp106)!= 9)goto _LLB4;_tmp107=((struct Cyc_Absyn_TupleType_struct*)_tmp106)->f1;
_LLB3: {struct Cyc_List_List*_tmp112=0;while(_tmp107 != 0){_tmp112=({struct Cyc_List_List*
_tmp113=_region_malloc(env->r,sizeof(*_tmp113));_tmp113->hd=(void*)(*((struct
_tuple13*)_tmp107->hd)).f2;_tmp113->tl=_tmp112;_tmp113;});_tmp107=_tmp107->tl;}
_tmp112=Cyc_List_imp_rev(_tmp112);return Cyc_NewControlFlow_noalias_ptrs_aux(env,
p,_tmp112);}_LLB4: if(*((int*)_tmp106)!= 11)goto _LLB6;_tmp108=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp106)->f2;_LLB5: {struct Cyc_List_List*_tmp114=0;while(_tmp108 != 0){_tmp114=({
struct Cyc_List_List*_tmp115=_region_malloc(env->r,sizeof(*_tmp115));_tmp115->hd=(
void*)((void*)((struct Cyc_Absyn_Aggrfield*)_tmp108->hd)->type);_tmp115->tl=
_tmp114;_tmp115;});_tmp108=_tmp108->tl;}_tmp114=Cyc_List_imp_rev(_tmp114);return
Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp114);}_LLB6: if(*((int*)_tmp106)!= 
10)goto _LLB8;_tmp109=((struct Cyc_Absyn_AggrType_struct*)_tmp106)->f1;_tmp10A=
_tmp109.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp106)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LLB8;_tmp10B=(_tmp10A.KnownAggr).f1;_tmp10C=_tmp109.targs;_LLB7: if((*
_tmp10B)->impl == 0)return 0;else{struct Cyc_List_List*_tmp116=0;struct
_RegionHandle*_tmp117=env->r;{struct Cyc_List_List*_tmp118=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(_tmp117,_tmp117,(*_tmp10B)->tvs,_tmp10C);struct Cyc_List_List*
_tmp119=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp10B)->impl))->fields;
while(_tmp119 != 0){_tmp116=({struct Cyc_List_List*_tmp11A=_region_malloc(env->r,
sizeof(*_tmp11A));_tmp11A->hd=(void*)Cyc_Tcutil_rsubstitute(_tmp117,_tmp118,(
void*)((struct Cyc_Absyn_Aggrfield*)_tmp119->hd)->type);_tmp11A->tl=_tmp116;
_tmp11A;});_tmp119=_tmp119->tl;}}_tmp116=Cyc_List_imp_rev(_tmp116);return Cyc_NewControlFlow_noalias_ptrs_aux(
env,p,_tmp116);}_LLB8: if(*((int*)_tmp106)!= 10)goto _LLBA;_tmp10D=((struct Cyc_Absyn_AggrType_struct*)
_tmp106)->f1;_tmp10E=_tmp10D.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp106)->f1).aggr_info).UnknownAggr).tag != 0)goto _LLBA;_LLB9:({void*_tmp11B=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp11C="got unknown aggr in noalias_ptrs_rec";_tag_dyneither(_tmp11C,
sizeof(char),37);}),_tag_dyneither(_tmp11B,sizeof(void*),0));});_LLBA: if(*((int*)
_tmp106)!= 2)goto _LLBC;_LLBB: {struct _RegionHandle*_tmp11D=env->r;if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp11D,t))return({struct Cyc_List_List*_tmp11E=_region_malloc(env->r,sizeof(*
_tmp11E));_tmp11E->hd=p;_tmp11E->tl=0;_tmp11E;});else{return 0;}}_LLBC: if(*((int*)
_tmp106)!= 3)goto _LLBE;_tmp10F=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp106)->f1;
_tmp110=_tmp10F.field_info;_tmp111=_tmp10F.targs;_LLBD: {union Cyc_Absyn_TunionFieldInfoU_union
_tmp11F=_tmp110;struct Cyc_Absyn_Tuniondecl*_tmp120;struct Cyc_Absyn_Tunionfield*
_tmp121;_LLC1: if((_tmp11F.UnknownTunionfield).tag != 0)goto _LLC3;_LLC2:({void*
_tmp122=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp123="got unknown tunion field in noalias_ptrs_rec";_tag_dyneither(
_tmp123,sizeof(char),45);}),_tag_dyneither(_tmp122,sizeof(void*),0));});_LLC3:
if((_tmp11F.KnownTunionfield).tag != 1)goto _LLC0;_tmp120=(_tmp11F.KnownTunionfield).f1;
_tmp121=(_tmp11F.KnownTunionfield).f2;_LLC4: {struct Cyc_List_List*_tmp124=0;
struct _RegionHandle*_tmp125=env->r;{struct Cyc_List_List*_tmp126=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(_tmp125,_tmp125,_tmp120->tvs,_tmp111);struct Cyc_List_List*
_tmp127=_tmp121->typs;while(_tmp127 != 0){_tmp124=({struct Cyc_List_List*_tmp128=
_region_malloc(env->r,sizeof(*_tmp128));_tmp128->hd=(void*)Cyc_Tcutil_rsubstitute(
_tmp125,_tmp126,(*((struct _tuple13*)_tmp127->hd)).f2);_tmp128->tl=_tmp124;
_tmp128;});_tmp127=_tmp127->tl;}}_tmp124=Cyc_List_imp_rev(_tmp124);return Cyc_NewControlFlow_noalias_ptrs_aux(
env,p,_tmp124);}_LLC0:;}_LLBE:;_LLBF: return 0;_LLB1:;}}static struct _tuple5 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal_union lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_CfFlowInfo_ConsumeInfo outer_cinfo,struct Cyc_Position_Segment*
loc){outflow=Cyc_CfFlowInfo_consume_unique_rvals(loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp129=outflow;struct Cyc_Dict_Dict _tmp12A;struct Cyc_List_List*_tmp12B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp12C;_LLC6: if((_tmp129.BottomFL).tag != 0)goto _LLC8;_LLC7: return({struct
_tuple5 _tmp12D;_tmp12D.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp12E;(_tmp12E.BottomFL).tag=0;_tmp12E;});_tmp12D.f2=rval;_tmp12D;});_LLC8: if((
_tmp129.ReachableFL).tag != 1)goto _LLC5;_tmp12A=(_tmp129.ReachableFL).f1;_tmp12B=(
_tmp129.ReachableFL).f2;_tmp12C=(_tmp129.ReachableFL).f3;_LLC9: _tmp12C=Cyc_CfFlowInfo_and_consume(
fenv->r,outer_cinfo,_tmp12C);{union Cyc_CfFlowInfo_AbsLVal_union _tmp12F=lval;
struct Cyc_CfFlowInfo_Place*_tmp130;_LLCB: if((_tmp12F.PlaceL).tag != 0)goto _LLCD;
_tmp130=(_tmp12F.PlaceL).f1;_LLCC: _tmp12C=Cyc_NewControlFlow_consume_assignment(
env,_tmp130,_tmp12C,_tmp12A,lexp);_tmp12A=Cyc_CfFlowInfo_assign_place(fenv,loc,
_tmp12A,env->all_changed,_tmp130,rval);_tmp12B=Cyc_CfFlowInfo_reln_assign_exp(
fenv->r,_tmp12B,lexp,rexp);outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union
Cyc_CfFlowInfo_FlowInfo_union _tmp131;(_tmp131.ReachableFL).tag=1;(_tmp131.ReachableFL).f1=
_tmp12A;(_tmp131.ReachableFL).f2=_tmp12B;(_tmp131.ReachableFL).f3=_tmp12C;
_tmp131;});Cyc_NewControlFlow_update_tryflow(env,outflow);return({struct _tuple5
_tmp132;_tmp132.f1=outflow;_tmp132.f2=rval;_tmp132;});_LLCD: if((_tmp12F.UnknownL).tag
!= 1)goto _LLCA;_LLCE: return({struct _tuple5 _tmp133;_tmp133.f1=Cyc_NewControlFlow_use_Rval(
env,rexp->loc,outflow,rval);_tmp133.f2=rval;_tmp133;});_LLCA:;}_LLC5:;}}struct
_tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp134=env->fenv;struct Cyc_Dict_Dict
d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo cinfo;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp135=inflow;struct Cyc_Dict_Dict _tmp136;struct Cyc_List_List*_tmp137;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp138;_LLD0: if((_tmp135.BottomFL).tag != 0)goto _LLD2;_LLD1: return({struct
_tuple5 _tmp139;_tmp139.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp13A;(_tmp13A.BottomFL).tag=0;_tmp13A;});_tmp139.f2=(void*)_tmp134->unknown_all;
_tmp139;});_LLD2: if((_tmp135.ReachableFL).tag != 1)goto _LLCF;_tmp136=(_tmp135.ReachableFL).f1;
_tmp137=(_tmp135.ReachableFL).f2;_tmp138=(_tmp135.ReachableFL).f3;_LLD3: d=
_tmp136;relns=_tmp137;cinfo=_tmp138;_LLCF:;}{void*_tmp13B=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp13C;void*_tmp13D;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13F;
struct Cyc_Absyn_Exp*_tmp140;union Cyc_Absyn_Cnst_union _tmp141;union Cyc_Absyn_Cnst_union
_tmp142;int _tmp143;union Cyc_Absyn_Cnst_union _tmp144;void*_tmp145;struct Cyc_List_List*
_tmp146;void*_tmp147;void*_tmp148;struct Cyc_Absyn_Vardecl*_tmp149;void*_tmp14A;
struct Cyc_Absyn_Vardecl*_tmp14B;void*_tmp14C;struct Cyc_Absyn_Vardecl*_tmp14D;
void*_tmp14E;struct Cyc_List_List*_tmp14F;struct Cyc_Absyn_Exp*_tmp150;struct Cyc_Absyn_Exp*
_tmp151;struct Cyc_Core_Opt*_tmp152;struct Cyc_Core_Opt _tmp153;struct Cyc_Absyn_Exp*
_tmp154;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Core_Opt*_tmp156;struct Cyc_Absyn_Exp*
_tmp157;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Exp*
_tmp15A;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_List_List*_tmp15C;struct Cyc_Absyn_MallocInfo
_tmp15D;int _tmp15E;struct Cyc_Absyn_Exp*_tmp15F;void**_tmp160;struct Cyc_Absyn_Exp*
_tmp161;int _tmp162;struct Cyc_Absyn_Exp*_tmp163;struct Cyc_Absyn_Exp*_tmp164;
struct Cyc_Absyn_Exp*_tmp165;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Exp*
_tmp167;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*_tmp169;struct
_dyneither_ptr*_tmp16A;struct Cyc_Absyn_Exp*_tmp16B;struct _dyneither_ptr*_tmp16C;
struct Cyc_Absyn_Exp*_tmp16D;struct Cyc_Absyn_Exp*_tmp16E;struct Cyc_Absyn_Exp*
_tmp16F;struct Cyc_Absyn_Exp*_tmp170;struct Cyc_Absyn_Exp*_tmp171;struct Cyc_Absyn_Exp*
_tmp172;struct Cyc_Absyn_Exp*_tmp173;struct Cyc_Absyn_Exp*_tmp174;struct Cyc_Absyn_Exp*
_tmp175;struct Cyc_List_List*_tmp176;struct Cyc_Absyn_Tuniondecl*_tmp177;struct Cyc_List_List*
_tmp178;struct Cyc_List_List*_tmp179;struct Cyc_List_List*_tmp17A;struct Cyc_Absyn_Aggrdecl*
_tmp17B;struct Cyc_Absyn_Aggrdecl _tmp17C;struct Cyc_Absyn_AggrdeclImpl*_tmp17D;
struct Cyc_Absyn_AggrdeclImpl _tmp17E;struct Cyc_List_List*_tmp17F;struct Cyc_List_List*
_tmp180;struct Cyc_Absyn_Vardecl*_tmp181;struct Cyc_Absyn_Exp*_tmp182;struct Cyc_Absyn_Exp*
_tmp183;int _tmp184;struct Cyc_Absyn_Stmt*_tmp185;void*_tmp186;_LLD5: if(*((int*)
_tmp13B)!= 15)goto _LLD7;_tmp13C=((struct Cyc_Absyn_Cast_e_struct*)_tmp13B)->f2;
_tmp13D=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp13B)->f4;if((int)_tmp13D != 2)
goto _LLD7;_LLD6: {union Cyc_CfFlowInfo_FlowInfo_union _tmp188;void*_tmp189;struct
_tuple5 _tmp187=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp13C);_tmp188=_tmp187.f1;
_tmp189=_tmp187.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp18B;void*_tmp18C;
struct _tuple5 _tmp18A=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp188,_tmp13C,
_tmp189);_tmp18B=_tmp18A.f1;_tmp18C=_tmp18A.f2;return({struct _tuple5 _tmp18D;
_tmp18D.f1=_tmp18B;_tmp18D.f2=_tmp189;_tmp18D;});}}_LLD7: if(*((int*)_tmp13B)!= 
15)goto _LLD9;_tmp13E=((struct Cyc_Absyn_Cast_e_struct*)_tmp13B)->f2;_LLD8: _tmp13F=
_tmp13E;goto _LLDA;_LLD9: if(*((int*)_tmp13B)!= 13)goto _LLDB;_tmp13F=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp13B)->f1;_LLDA: _tmp140=_tmp13F;goto _LLDC;_LLDB: if(*((int*)_tmp13B)!= 14)goto
_LLDD;_tmp140=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp13B)->f1;_LLDC: return
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp140);_LLDD: if(*((int*)_tmp13B)!= 0)
goto _LLDF;_tmp141=((struct Cyc_Absyn_Const_e_struct*)_tmp13B)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmp13B)->f1).Null_c).tag != 6)goto _LLDF;_LLDE: goto
_LLE0;_LLDF: if(*((int*)_tmp13B)!= 0)goto _LLE1;_tmp142=((struct Cyc_Absyn_Const_e_struct*)
_tmp13B)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp13B)->f1).Int_c).tag != 2)
goto _LLE1;_tmp143=(_tmp142.Int_c).f2;if(_tmp143 != 0)goto _LLE1;_LLE0: return({
struct _tuple5 _tmp18E;_tmp18E.f1=inflow;_tmp18E.f2=(void*)0;_tmp18E;});_LLE1: if(*((
int*)_tmp13B)!= 0)goto _LLE3;_tmp144=((struct Cyc_Absyn_Const_e_struct*)_tmp13B)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp13B)->f1).Int_c).tag != 2)goto _LLE3;
_LLE2: goto _LLE4;_LLE3: if(*((int*)_tmp13B)!= 1)goto _LLE5;_tmp145=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp13B)->f2;if(_tmp145 <= (void*)1)goto _LLE5;if(*((int*)
_tmp145)!= 1)goto _LLE5;_LLE4: return({struct _tuple5 _tmp18F;_tmp18F.f1=inflow;
_tmp18F.f2=(void*)1;_tmp18F;});_LLE5: if(*((int*)_tmp13B)!= 32)goto _LLE7;_tmp146=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp13B)->f1;if(_tmp146 != 0)goto _LLE7;_LLE6:
goto _LLE8;_LLE7: if(*((int*)_tmp13B)!= 0)goto _LLE9;_LLE8: goto _LLEA;_LLE9: if(*((
int*)_tmp13B)!= 19)goto _LLEB;_LLEA: goto _LLEC;_LLEB: if(*((int*)_tmp13B)!= 18)goto
_LLED;_LLEC: goto _LLEE;_LLED: if(*((int*)_tmp13B)!= 20)goto _LLEF;_LLEE: goto _LLF0;
_LLEF: if(*((int*)_tmp13B)!= 21)goto _LLF1;_LLF0: goto _LLF2;_LLF1: if(*((int*)
_tmp13B)!= 34)goto _LLF3;_LLF2: goto _LLF4;_LLF3: if(*((int*)_tmp13B)!= 33)goto _LLF5;
_LLF4: return({struct _tuple5 _tmp190;_tmp190.f1=inflow;_tmp190.f2=(void*)_tmp134->unknown_all;
_tmp190;});_LLF5: if(*((int*)_tmp13B)!= 1)goto _LLF7;_tmp147=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13B)->f2;if(_tmp147 <= (void*)1)goto _LLF7;if(*((int*)_tmp147)!= 0)goto _LLF7;
_LLF6: return({struct _tuple5 _tmp191;_tmp191.f1=inflow;_tmp191.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp134,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp134->unknown_all);
_tmp191;});_LLF7: if(*((int*)_tmp13B)!= 1)goto _LLF9;_tmp148=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13B)->f2;if(_tmp148 <= (void*)1)goto _LLF9;if(*((int*)_tmp148)!= 2)goto _LLF9;
_tmp149=((struct Cyc_Absyn_Param_b_struct*)_tmp148)->f1;_LLF8: _tmp14B=_tmp149;
goto _LLFA;_LLF9: if(*((int*)_tmp13B)!= 1)goto _LLFB;_tmp14A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13B)->f2;if(_tmp14A <= (void*)1)goto _LLFB;if(*((int*)_tmp14A)!= 3)goto _LLFB;
_tmp14B=((struct Cyc_Absyn_Local_b_struct*)_tmp14A)->f1;_LLFA: _tmp14D=_tmp14B;
goto _LLFC;_LLFB: if(*((int*)_tmp13B)!= 1)goto _LLFD;_tmp14C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13B)->f2;if(_tmp14C <= (void*)1)goto _LLFD;if(*((int*)_tmp14C)!= 4)goto _LLFD;
_tmp14D=((struct Cyc_Absyn_Pat_b_struct*)_tmp14C)->f1;_LLFC: inflow=Cyc_NewControlFlow_may_consume_exp(
env,inflow,e);return({struct _tuple5 _tmp192;_tmp192.f1=inflow;_tmp192.f2=Cyc_Dict_lookup(
d,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp193=_region_malloc(env->r,
sizeof(*_tmp193));_tmp193[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp194;
_tmp194.tag=0;_tmp194.f1=_tmp14D;_tmp194;});_tmp193;}));_tmp192;});_LLFD: if(*((
int*)_tmp13B)!= 3)goto _LLFF;_tmp14E=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp13B)->f1;_tmp14F=((struct Cyc_Absyn_Primop_e_struct*)_tmp13B)->f2;_LLFE: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp196;void*_tmp197;struct _tuple5 _tmp195=Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmp14F,0);_tmp196=_tmp195.f1;_tmp197=_tmp195.f2;{void*_tmp198=
_tmp14E;_LL13E: if((int)_tmp198 != 0)goto _LL140;_LL13F: goto _LL141;_LL140: if((int)
_tmp198 != 2)goto _LL142;_LL141: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp14F))->hd)->loc,_tmp196);goto _LL13D;_LL142:;
_LL143: _tmp196=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp14F))->hd)->loc,_tmp196);goto _LL13D;_LL13D:;}
return({struct _tuple5 _tmp199;_tmp199.f1=_tmp196;_tmp199.f2=_tmp197;_tmp199;});}
_LLFF: if(*((int*)_tmp13B)!= 5)goto _LL101;_tmp150=((struct Cyc_Absyn_Increment_e_struct*)
_tmp13B)->f1;_LL100: {struct Cyc_List_List _tmp19A=({struct Cyc_List_List _tmp1A9;
_tmp1A9.hd=_tmp150;_tmp1A9.tl=0;_tmp1A9;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp19C;struct _tuple5 _tmp19B=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct
Cyc_List_List*)& _tmp19A,0);_tmp19C=_tmp19B.f1;Cyc_CfFlowInfo_check_unique_rvals(
_tmp150->loc,_tmp19C);{union Cyc_CfFlowInfo_AbsLVal_union _tmp19E;struct _tuple7
_tmp19D=Cyc_NewControlFlow_anal_Lexp(env,_tmp19C,_tmp150);_tmp19E=_tmp19D.f2;{
struct _tuple12 _tmp1A0=({struct _tuple12 _tmp19F;_tmp19F.f1=_tmp19E;_tmp19F.f2=
_tmp19C;_tmp19F;});union Cyc_CfFlowInfo_AbsLVal_union _tmp1A1;struct Cyc_CfFlowInfo_Place*
_tmp1A2;union Cyc_CfFlowInfo_FlowInfo_union _tmp1A3;struct Cyc_Dict_Dict _tmp1A4;
struct Cyc_List_List*_tmp1A5;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1A6;_LL145:
_tmp1A1=_tmp1A0.f1;if(((_tmp1A0.f1).PlaceL).tag != 0)goto _LL147;_tmp1A2=(_tmp1A1.PlaceL).f1;
_tmp1A3=_tmp1A0.f2;if(((_tmp1A0.f2).ReachableFL).tag != 1)goto _LL147;_tmp1A4=(
_tmp1A3.ReachableFL).f1;_tmp1A5=(_tmp1A3.ReachableFL).f2;_tmp1A6=(_tmp1A3.ReachableFL).f3;
_LL146: _tmp1A5=Cyc_CfFlowInfo_reln_kill_exp(_tmp134->r,_tmp1A5,_tmp150);_tmp19C=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp1A7;(
_tmp1A7.ReachableFL).tag=1;(_tmp1A7.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
_tmp134,_tmp150->loc,_tmp1A4,env->all_changed,_tmp1A2,(void*)_tmp134->unknown_all);(
_tmp1A7.ReachableFL).f2=_tmp1A5;(_tmp1A7.ReachableFL).f3=_tmp1A6;_tmp1A7;});goto
_LL144;_LL147:;_LL148: goto _LL144;_LL144:;}return({struct _tuple5 _tmp1A8;_tmp1A8.f1=
_tmp19C;_tmp1A8.f2=(void*)_tmp134->unknown_all;_tmp1A8;});}}_LL101: if(*((int*)
_tmp13B)!= 4)goto _LL103;_tmp151=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13B)->f1;
_tmp152=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13B)->f2;if(_tmp152 == 0)goto
_LL103;_tmp153=*_tmp152;_tmp154=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13B)->f3;
_LL102: {struct Cyc_List_List _tmp1AA=({struct Cyc_List_List _tmp1B9;_tmp1B9.hd=
_tmp154;_tmp1B9.tl=0;_tmp1B9;});struct Cyc_List_List _tmp1AB=({struct Cyc_List_List
_tmp1B8;_tmp1B8.hd=_tmp151;_tmp1B8.tl=(struct Cyc_List_List*)& _tmp1AA;_tmp1B8;});
union Cyc_CfFlowInfo_FlowInfo_union _tmp1AD;struct _tuple5 _tmp1AC=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp1AB,1);_tmp1AD=_tmp1AC.f1;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp1AF;struct _tuple7 _tmp1AE=Cyc_NewControlFlow_anal_Lexp(env,_tmp1AD,_tmp151);
_tmp1AF=_tmp1AE.f2;_tmp1AD=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp1AD);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp1B0=_tmp1AD;struct Cyc_Dict_Dict _tmp1B1;
struct Cyc_List_List*_tmp1B2;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1B3;_LL14A: if((
_tmp1B0.ReachableFL).tag != 1)goto _LL14C;_tmp1B1=(_tmp1B0.ReachableFL).f1;_tmp1B2=(
_tmp1B0.ReachableFL).f2;_tmp1B3=(_tmp1B0.ReachableFL).f3;_LL14B:{union Cyc_CfFlowInfo_AbsLVal_union
_tmp1B4=_tmp1AF;struct Cyc_CfFlowInfo_Place*_tmp1B5;_LL14F: if((_tmp1B4.PlaceL).tag
!= 0)goto _LL151;_tmp1B5=(_tmp1B4.PlaceL).f1;_LL150: _tmp1B3=Cyc_NewControlFlow_consume_assignment(
env,_tmp1B5,_tmp1B3,_tmp1B1,_tmp151);_tmp1B2=Cyc_CfFlowInfo_reln_kill_exp(
_tmp134->r,_tmp1B2,_tmp151);_tmp1B1=Cyc_CfFlowInfo_assign_place(_tmp134,_tmp151->loc,
_tmp1B1,env->all_changed,_tmp1B5,(void*)_tmp134->unknown_all);_tmp1AD=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1B6;(_tmp1B6.ReachableFL).tag=1;(_tmp1B6.ReachableFL).f1=
_tmp1B1;(_tmp1B6.ReachableFL).f2=_tmp1B2;(_tmp1B6.ReachableFL).f3=_tmp1B3;
_tmp1B6;});goto _LL14E;_LL151: if((_tmp1B4.UnknownL).tag != 1)goto _LL14E;_LL152:
goto _LL14E;_LL14E:;}goto _LL149;_LL14C:;_LL14D: goto _LL149;_LL149:;}return({struct
_tuple5 _tmp1B7;_tmp1B7.f1=_tmp1AD;_tmp1B7.f2=(void*)_tmp134->unknown_all;_tmp1B7;});}}
_LL103: if(*((int*)_tmp13B)!= 4)goto _LL105;_tmp155=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp13B)->f1;_tmp156=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13B)->f2;if(
_tmp156 != 0)goto _LL105;_tmp157=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13B)->f3;
_LL104: {struct Cyc_Dict_Dict*_tmp1BA=env->all_changed;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1BC;union Cyc_CfFlowInfo_FlowInfo_union _tmp1BD;struct _tuple6 _tmp1BB=Cyc_CfFlowInfo_save_consume_info(
_tmp134,inflow,1);_tmp1BC=_tmp1BB.f1;_tmp1BD=_tmp1BB.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
empty_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp1D8;_tmp1D8.consumed=_tmp134->mt_place_set;
_tmp1D8.may_consume=0;_tmp1D8;});struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=
empty_consume;int init_consume=0;while(1){env->all_changed=({struct Cyc_Dict_Dict*
_tmp1BE=_region_malloc(env->r,sizeof(*_tmp1BE));_tmp1BE[0]=_tmp134->mt_place_set;
_tmp1BE;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp1C0;union Cyc_CfFlowInfo_AbsLVal_union
_tmp1C1;struct _tuple7 _tmp1BF=Cyc_NewControlFlow_anal_Lexp(env,_tmp1BD,_tmp155);
_tmp1C0=_tmp1BF.f1;_tmp1C1=_tmp1BF.f2;{struct Cyc_Dict_Dict _tmp1C2=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));env->all_changed=({struct Cyc_Dict_Dict*_tmp1C3=
_region_malloc(env->r,sizeof(*_tmp1C3));_tmp1C3[0]=_tmp134->mt_place_set;_tmp1C3;});{
union Cyc_CfFlowInfo_FlowInfo_union _tmp1C5;void*_tmp1C6;struct _tuple5 _tmp1C4=Cyc_NewControlFlow_anal_Rexp(
env,_tmp1BD,_tmp157);_tmp1C5=_tmp1C4.f1;_tmp1C6=_tmp1C4.f2;{struct Cyc_Dict_Dict
_tmp1C7=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo_union
_tmp1C8=Cyc_CfFlowInfo_after_flow(_tmp134,(struct Cyc_Dict_Dict*)& _tmp1C2,_tmp1C0,
_tmp1C5,_tmp1C2,_tmp1C7);union Cyc_CfFlowInfo_FlowInfo_union _tmp1C9=Cyc_CfFlowInfo_join_flow(
_tmp134,_tmp1BA,_tmp1BD,_tmp1C8,1);struct Cyc_CfFlowInfo_ConsumeInfo _tmp1CB;
struct _tuple6 _tmp1CA=Cyc_CfFlowInfo_save_consume_info(_tmp134,_tmp1C9,0);_tmp1CB=
_tmp1CA.f1;if(init_consume){if(!Cyc_CfFlowInfo_consume_approx(outflow_consume,
_tmp1CB)){({void*_tmp1CC=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1CD="sanity consumed: ";
_tag_dyneither(_tmp1CD,sizeof(char),18);}),_tag_dyneither(_tmp1CC,sizeof(void*),
0));});((void(*)(struct Cyc_Dict_Dict p,void(*pr)(struct Cyc_CfFlowInfo_Place*)))
Cyc_CfFlowInfo_print_dict_set)(outflow_consume.consumed,Cyc_CfFlowInfo_print_place);({
void*_tmp1CE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1CF="\ncurrent consumed: ";
_tag_dyneither(_tmp1CF,sizeof(char),20);}),_tag_dyneither(_tmp1CE,sizeof(void*),
0));});((void(*)(struct Cyc_Dict_Dict p,void(*pr)(struct Cyc_CfFlowInfo_Place*)))
Cyc_CfFlowInfo_print_dict_set)(_tmp1CB.consumed,Cyc_CfFlowInfo_print_place);({
void*_tmp1D0=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1D1="\nsanity may_consume: ";
_tag_dyneither(_tmp1D1,sizeof(char),22);}),_tag_dyneither(_tmp1D0,sizeof(void*),
0));});((void(*)(struct Cyc_List_List*p,void(*pr)(struct Cyc_CfFlowInfo_Place*)))
Cyc_CfFlowInfo_print_list)(outflow_consume.may_consume,Cyc_CfFlowInfo_print_place);({
void*_tmp1D2=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1D3="\ncurrent may_consume: ";
_tag_dyneither(_tmp1D3,sizeof(char),23);}),_tag_dyneither(_tmp1D2,sizeof(void*),
0));});((void(*)(struct Cyc_List_List*p,void(*pr)(struct Cyc_CfFlowInfo_Place*)))
Cyc_CfFlowInfo_print_list)(_tmp1CB.may_consume,Cyc_CfFlowInfo_print_place);({
void*_tmp1D4=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1D5="\n";_tag_dyneither(
_tmp1D5,sizeof(char),2);}),_tag_dyneither(_tmp1D4,sizeof(void*),0));});({void*
_tmp1D6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1D7="assignOp_e failed to preserve consume info";_tag_dyneither(
_tmp1D7,sizeof(char),43);}),_tag_dyneither(_tmp1D6,sizeof(void*),0));});}_tmp1BD=
Cyc_CfFlowInfo_restore_consume_info(_tmp1BD,outflow_consume,0);}outflow_consume=
_tmp1CB;init_consume=1;if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp1C9,_tmp1BD)){
if(_tmp1BA == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=
Cyc_CfFlowInfo_union_place_set(*_tmp1BA,Cyc_CfFlowInfo_union_place_set(_tmp1C2,
_tmp1C7,0),0);}return Cyc_NewControlFlow_do_assign(_tmp134,env,_tmp1C8,_tmp155,
_tmp1C1,_tmp157,_tmp1C6,_tmp1BC,e->loc);}_tmp1BD=Cyc_CfFlowInfo_restore_consume_info(
_tmp1C9,empty_consume,0);}}}}}}}_LL105: if(*((int*)_tmp13B)!= 9)goto _LL107;
_tmp158=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp13B)->f1;_tmp159=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp13B)->f2;_LL106: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1DA;void*_tmp1DB;
struct _tuple5 _tmp1D9=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp158);_tmp1DA=
_tmp1D9.f1;_tmp1DB=_tmp1D9.f2;_tmp1DA=Cyc_CfFlowInfo_drop_unique_rvals(_tmp158->loc,
_tmp1DA);return Cyc_NewControlFlow_anal_Rexp(env,_tmp1DA,_tmp159);}_LL107: if(*((
int*)_tmp13B)!= 12)goto _LL109;_tmp15A=((struct Cyc_Absyn_Throw_e_struct*)_tmp13B)->f1;
_LL108: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1DD;void*_tmp1DE;struct _tuple5
_tmp1DC=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp15A);_tmp1DD=_tmp1DC.f1;
_tmp1DE=_tmp1DC.f2;_tmp1DD=Cyc_CfFlowInfo_consume_unique_rvals(_tmp15A->loc,
_tmp1DD);Cyc_NewControlFlow_use_Rval(env,_tmp15A->loc,_tmp1DD,_tmp1DE);return({
struct _tuple5 _tmp1DF;_tmp1DF.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1E0;(_tmp1E0.BottomFL).tag=0;_tmp1E0;});_tmp1DF.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp134,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp134->unknown_all);
_tmp1DF;});}_LL109: if(*((int*)_tmp13B)!= 11)goto _LL10B;_tmp15B=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp13B)->f1;_tmp15C=((struct Cyc_Absyn_FnCall_e_struct*)_tmp13B)->f2;_LL10A: {
struct _RegionHandle*_tmp1E1=env->r;union Cyc_CfFlowInfo_FlowInfo_union _tmp1E4;
struct Cyc_List_List*_tmp1E5;struct _tuple11 _tmp1E3=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp1E1,env,inflow,({struct Cyc_List_List*_tmp1E2=_region_malloc(_tmp1E1,sizeof(*
_tmp1E2));_tmp1E2->hd=_tmp15B;_tmp1E2->tl=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(_tmp1E1,_tmp15C);_tmp1E2;}),
1);_tmp1E4=_tmp1E3.f1;_tmp1E5=_tmp1E3.f2;_tmp1E4=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp1E4);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1E6=Cyc_NewControlFlow_use_Rval(
env,_tmp15B->loc,_tmp1E4,(void*)((struct Cyc_List_List*)_check_null(_tmp1E5))->hd);
_tmp1E5=_tmp1E5->tl;{struct Cyc_List_List*init_params=0;{void*_tmp1E7=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp15B->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1E8;void*_tmp1E9;_LL154: if(_tmp1E7 <= (void*)4)goto _LL156;if(*((int*)_tmp1E7)
!= 4)goto _LL156;_tmp1E8=((struct Cyc_Absyn_PointerType_struct*)_tmp1E7)->f1;
_tmp1E9=(void*)_tmp1E8.elt_typ;_LL155:{void*_tmp1EA=Cyc_Tcutil_compress(_tmp1E9);
struct Cyc_Absyn_FnInfo _tmp1EB;struct Cyc_List_List*_tmp1EC;_LL159: if(_tmp1EA <= (
void*)4)goto _LL15B;if(*((int*)_tmp1EA)!= 8)goto _LL15B;_tmp1EB=((struct Cyc_Absyn_FnType_struct*)
_tmp1EA)->f1;_tmp1EC=_tmp1EB.attributes;_LL15A: for(0;_tmp1EC != 0;_tmp1EC=_tmp1EC->tl){
void*_tmp1ED=(void*)_tmp1EC->hd;int _tmp1EE;_LL15E: if(_tmp1ED <= (void*)17)goto
_LL160;if(*((int*)_tmp1ED)!= 4)goto _LL160;_tmp1EE=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1ED)->f1;_LL15F: init_params=({struct Cyc_List_List*_tmp1EF=_region_malloc(
_tmp1E1,sizeof(*_tmp1EF));_tmp1EF->hd=(void*)_tmp1EE;_tmp1EF->tl=init_params;
_tmp1EF;});goto _LL15D;_LL160:;_LL161: goto _LL15D;_LL15D:;}goto _LL158;_LL15B:;
_LL15C:({void*_tmp1F0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1F1="anal_Rexp: bad function type";
_tag_dyneither(_tmp1F1,sizeof(char),29);}),_tag_dyneither(_tmp1F0,sizeof(void*),
0));});_LL158:;}goto _LL153;_LL156:;_LL157:({void*_tmp1F2=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F3="anal_Rexp: bad function type";
_tag_dyneither(_tmp1F3,sizeof(char),29);}),_tag_dyneither(_tmp1F2,sizeof(void*),
0));});_LL153:;}{int i=1;for(0;_tmp1E5 != 0;(((_tmp1E5=_tmp1E5->tl,_tmp15C=((
struct Cyc_List_List*)_check_null(_tmp15C))->tl)),++ i)){if(!((int(*)(struct Cyc_List_List*
l,int x))Cyc_List_memq)(init_params,i)){_tmp1E6=Cyc_NewControlFlow_use_Rval(env,((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp15C))->hd)->loc,
_tmp1E6,(void*)_tmp1E5->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp1F4=
_tmp1E4;struct Cyc_Dict_Dict _tmp1F5;_LL163: if((_tmp1F4.BottomFL).tag != 0)goto
_LL165;_LL164: goto _LL162;_LL165: if((_tmp1F4.ReachableFL).tag != 1)goto _LL162;
_tmp1F5=(_tmp1F4.ReachableFL).f1;_LL166: if(Cyc_CfFlowInfo_initlevel(env->fenv,
_tmp1F5,(void*)_tmp1E5->hd)== (void*)0)({void*_tmp1F6=0;Cyc_Tcutil_terr(((struct
Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp15C))->hd)->loc,({const
char*_tmp1F7="expression may not be initialized";_tag_dyneither(_tmp1F7,sizeof(
char),34);}),_tag_dyneither(_tmp1F6,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1F8=_tmp1E6;struct Cyc_Dict_Dict _tmp1F9;struct Cyc_List_List*_tmp1FA;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1FB;_LL168: if((_tmp1F8.BottomFL).tag != 0)goto _LL16A;_LL169: goto _LL167;_LL16A:
if((_tmp1F8.ReachableFL).tag != 1)goto _LL167;_tmp1F9=(_tmp1F8.ReachableFL).f1;
_tmp1FA=(_tmp1F8.ReachableFL).f2;_tmp1FB=(_tmp1F8.ReachableFL).f3;_LL16B: {
struct Cyc_Dict_Dict _tmp1FC=Cyc_CfFlowInfo_escape_deref(_tmp134,_tmp1F9,env->all_changed,(
void*)_tmp1E5->hd);{void*_tmp1FD=(void*)_tmp1E5->hd;struct Cyc_CfFlowInfo_Place*
_tmp1FE;_LL16D: if(_tmp1FD <= (void*)3)goto _LL16F;if(*((int*)_tmp1FD)!= 2)goto
_LL16F;_tmp1FE=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1FD)->f1;_LL16E:{
void*_tmp1FF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp15C))->hd)->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp200;void*_tmp201;_LL172: if(_tmp1FF <= (void*)4)goto
_LL174;if(*((int*)_tmp1FF)!= 4)goto _LL174;_tmp200=((struct Cyc_Absyn_PointerType_struct*)
_tmp1FF)->f1;_tmp201=(void*)_tmp200.elt_typ;_LL173: _tmp1FC=Cyc_CfFlowInfo_assign_place(
_tmp134,((struct Cyc_Absyn_Exp*)_tmp15C->hd)->loc,_tmp1FC,env->all_changed,
_tmp1FE,Cyc_CfFlowInfo_typ_to_absrval(_tmp134,_tmp201,(void*)_tmp134->esc_all));
goto _LL171;_LL174:;_LL175:({void*_tmp202=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp203="anal_Rexp:bad type for initialized arg";
_tag_dyneither(_tmp203,sizeof(char),39);}),_tag_dyneither(_tmp202,sizeof(void*),
0));});_LL171:;}goto _LL16C;_LL16F:;_LL170: goto _LL16C;_LL16C:;}_tmp1E6=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp204;(_tmp204.ReachableFL).tag=1;(_tmp204.ReachableFL).f1=
_tmp1FC;(_tmp204.ReachableFL).f2=_tmp1FA;(_tmp204.ReachableFL).f3=_tmp1FB;
_tmp204;});goto _LL167;}_LL167:;}goto _LL162;_LL162:;}}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp15B->topt))->v))return({struct
_tuple5 _tmp205;_tmp205.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp206;(_tmp206.BottomFL).tag=0;_tmp206;});_tmp205.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp134,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp134->unknown_all);
_tmp205;});else{return({struct _tuple5 _tmp207;_tmp207.f1=_tmp1E6;_tmp207.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp134,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp134->unknown_all);
_tmp207;});}}}}_LL10B: if(*((int*)_tmp13B)!= 35)goto _LL10D;_tmp15D=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp13B)->f1;_tmp15E=_tmp15D.is_calloc;_tmp15F=_tmp15D.rgn;_tmp160=_tmp15D.elt_type;
_tmp161=_tmp15D.num_elts;_tmp162=_tmp15D.fat_result;_LL10C: {void*root=(void*)({
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp219=_region_malloc(_tmp134->r,sizeof(*
_tmp219));_tmp219[0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp21A;_tmp21A.tag=1;
_tmp21A.f1=_tmp161;_tmp21A.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp21A;});_tmp219;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp218=_region_malloc(_tmp134->r,sizeof(*_tmp218));_tmp218->root=(void*)root;
_tmp218->fields=0;_tmp218;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp216=_region_malloc(_tmp134->r,sizeof(*_tmp216));_tmp216[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp217;_tmp217.tag=2;_tmp217.f1=place;_tmp217;});_tmp216;});void*place_val=
_tmp162?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(_tmp134,*((void**)_check_null(
_tmp160)),(void*)_tmp134->unknown_none);union Cyc_CfFlowInfo_FlowInfo_union
outflow;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct
Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,
place,0);if(_tmp15F != 0){struct _RegionHandle*_tmp208=env->r;union Cyc_CfFlowInfo_FlowInfo_union
_tmp20B;struct Cyc_List_List*_tmp20C;struct _tuple11 _tmp20A=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp208,env,inflow,({struct Cyc_Absyn_Exp*_tmp209[2];_tmp209[1]=_tmp161;_tmp209[0]=(
struct Cyc_Absyn_Exp*)_tmp15F;((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(_tmp208,_tag_dyneither(_tmp209,sizeof(
struct Cyc_Absyn_Exp*),2));}),0);_tmp20B=_tmp20A.f1;_tmp20C=_tmp20A.f2;outflow=
_tmp20B;}else{union Cyc_CfFlowInfo_FlowInfo_union _tmp20E;struct _tuple5 _tmp20D=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp161);_tmp20E=_tmp20D.f1;outflow=_tmp20E;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp161->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp20F=outflow;struct
Cyc_Dict_Dict _tmp210;struct Cyc_List_List*_tmp211;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp212;_LL177: if((_tmp20F.BottomFL).tag != 0)goto _LL179;_LL178: return({struct
_tuple5 _tmp213;_tmp213.f1=outflow;_tmp213.f2=rval;_tmp213;});_LL179: if((_tmp20F.ReachableFL).tag
!= 1)goto _LL176;_tmp210=(_tmp20F.ReachableFL).f1;_tmp211=(_tmp20F.ReachableFL).f2;
_tmp212=(_tmp20F.ReachableFL).f3;_LL17A: return({struct _tuple5 _tmp214;_tmp214.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp215;(
_tmp215.ReachableFL).tag=1;(_tmp215.ReachableFL).f1=Cyc_Dict_insert(_tmp210,root,
place_val);(_tmp215.ReachableFL).f2=_tmp211;(_tmp215.ReachableFL).f3=_tmp212;
_tmp215;});_tmp214.f2=rval;_tmp214;});_LL176:;}}_LL10D: if(*((int*)_tmp13B)!= 36)
goto _LL10F;_tmp163=((struct Cyc_Absyn_Swap_e_struct*)_tmp13B)->f1;_tmp164=((
struct Cyc_Absyn_Swap_e_struct*)_tmp13B)->f2;_LL10E: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo_union outflow;struct _RegionHandle*_tmp21B=
env->r;{union Cyc_CfFlowInfo_FlowInfo_union _tmp21E;struct Cyc_List_List*_tmp21F;
struct _tuple11 _tmp21D=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp21B,env,inflow,({
struct Cyc_Absyn_Exp*_tmp21C[2];_tmp21C[1]=_tmp164;_tmp21C[0]=_tmp163;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp21B,
_tag_dyneither(_tmp21C,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp21E=_tmp21D.f1;
_tmp21F=_tmp21D.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp21F))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp21F->tl))->hd;outflow=
_tmp21E;}Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,outflow);{union Cyc_CfFlowInfo_AbsLVal_union
_tmp221;struct _tuple7 _tmp220=Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmp163);
_tmp221=_tmp220.f2;{union Cyc_CfFlowInfo_AbsLVal_union _tmp223;struct _tuple7
_tmp222=Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmp164);_tmp223=_tmp222.f2;{
union Cyc_CfFlowInfo_FlowInfo_union _tmp224=outflow;struct Cyc_Dict_Dict _tmp225;
struct Cyc_List_List*_tmp226;struct Cyc_CfFlowInfo_ConsumeInfo _tmp227;_LL17C: if((
_tmp224.ReachableFL).tag != 1)goto _LL17E;_tmp225=(_tmp224.ReachableFL).f1;_tmp226=(
_tmp224.ReachableFL).f2;_tmp227=(_tmp224.ReachableFL).f3;_LL17D:{union Cyc_CfFlowInfo_AbsLVal_union
_tmp228=_tmp221;struct Cyc_CfFlowInfo_Place*_tmp229;_LL181: if((_tmp228.PlaceL).tag
!= 0)goto _LL183;_tmp229=(_tmp228.PlaceL).f1;_LL182: _tmp225=Cyc_CfFlowInfo_assign_place(
_tmp134,_tmp163->loc,_tmp225,env->all_changed,_tmp229,right_rval);goto _LL180;
_LL183: if((_tmp228.UnknownL).tag != 1)goto _LL180;_LL184: goto _LL180;_LL180:;}{
union Cyc_CfFlowInfo_AbsLVal_union _tmp22A=_tmp223;struct Cyc_CfFlowInfo_Place*
_tmp22B;_LL186: if((_tmp22A.PlaceL).tag != 0)goto _LL188;_tmp22B=(_tmp22A.PlaceL).f1;
_LL187: _tmp225=Cyc_CfFlowInfo_assign_place(_tmp134,_tmp164->loc,_tmp225,env->all_changed,
_tmp22B,left_rval);goto _LL185;_LL188: if((_tmp22A.UnknownL).tag != 1)goto _LL185;
_LL189: goto _LL185;_LL185:;}_tmp226=Cyc_CfFlowInfo_reln_kill_exp(_tmp134->r,
_tmp226,_tmp163);_tmp226=Cyc_CfFlowInfo_reln_kill_exp(_tmp134->r,_tmp226,_tmp164);
outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp22C;(_tmp22C.ReachableFL).tag=1;(_tmp22C.ReachableFL).f1=_tmp225;(_tmp22C.ReachableFL).f2=
_tmp226;(_tmp22C.ReachableFL).f3=_tmp227;_tmp22C;});goto _LL17B;_LL17E:;_LL17F:
goto _LL17B;_LL17B:;}return({struct _tuple5 _tmp22D;_tmp22D.f1=outflow;_tmp22D.f2=(
void*)_tmp134->unknown_all;_tmp22D;});}}}_LL10F: if(*((int*)_tmp13B)!= 17)goto
_LL111;_tmp165=((struct Cyc_Absyn_New_e_struct*)_tmp13B)->f1;_tmp166=((struct Cyc_Absyn_New_e_struct*)
_tmp13B)->f2;_LL110: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp240=_region_malloc(_tmp134->r,sizeof(*_tmp240));_tmp240[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp241;_tmp241.tag=1;_tmp241.f1=_tmp166;_tmp241.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp241;});_tmp240;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp23F=_region_malloc(_tmp134->r,sizeof(*_tmp23F));
_tmp23F->root=(void*)root;_tmp23F->fields=0;_tmp23F;});void*rval=(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp23D=_region_malloc(_tmp134->r,sizeof(*
_tmp23D));_tmp23D[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp23E;_tmp23E.tag=
2;_tmp23E.f1=place;_tmp23E;});_tmp23D;});((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo_union outflow;void*
place_val;if(_tmp165 != 0){struct _RegionHandle*_tmp22E=env->r;union Cyc_CfFlowInfo_FlowInfo_union
_tmp231;struct Cyc_List_List*_tmp232;struct _tuple11 _tmp230=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp22E,env,inflow,({struct Cyc_Absyn_Exp*_tmp22F[2];_tmp22F[1]=_tmp166;_tmp22F[0]=(
struct Cyc_Absyn_Exp*)_tmp165;((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(_tmp22E,_tag_dyneither(_tmp22F,sizeof(
struct Cyc_Absyn_Exp*),2));}),0);_tmp231=_tmp230.f1;_tmp232=_tmp230.f2;outflow=
_tmp231;place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp232))->tl))->hd;}else{union Cyc_CfFlowInfo_FlowInfo_union _tmp234;
void*_tmp235;struct _tuple5 _tmp233=Cyc_NewControlFlow_anal_Rexp(env,inflow,
_tmp166);_tmp234=_tmp233.f1;_tmp235=_tmp233.f2;outflow=_tmp234;place_val=_tmp235;}
outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp166->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp236=outflow;struct Cyc_Dict_Dict _tmp237;struct Cyc_List_List*_tmp238;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp239;_LL18B: if((_tmp236.BottomFL).tag != 0)goto _LL18D;_LL18C: return({struct
_tuple5 _tmp23A;_tmp23A.f1=outflow;_tmp23A.f2=rval;_tmp23A;});_LL18D: if((_tmp236.ReachableFL).tag
!= 1)goto _LL18A;_tmp237=(_tmp236.ReachableFL).f1;_tmp238=(_tmp236.ReachableFL).f2;
_tmp239=(_tmp236.ReachableFL).f3;_LL18E: return({struct _tuple5 _tmp23B;_tmp23B.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp23C;(
_tmp23C.ReachableFL).tag=1;(_tmp23C.ReachableFL).f1=Cyc_Dict_insert(_tmp237,root,
place_val);(_tmp23C.ReachableFL).f2=_tmp238;(_tmp23C.ReachableFL).f3=_tmp239;
_tmp23C;});_tmp23B.f2=rval;_tmp23B;});_LL18A:;}}}_LL111: if(*((int*)_tmp13B)!= 16)
goto _LL113;_tmp167=((struct Cyc_Absyn_Address_e_struct*)_tmp13B)->f1;_LL112: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp243;struct _tuple5 _tmp242=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp167);_tmp243=_tmp242.f1;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp245;
struct _tuple6 _tmp244=Cyc_CfFlowInfo_save_consume_info(env->fenv,_tmp243,0);
_tmp245=_tmp244.f1;{union Cyc_CfFlowInfo_FlowInfo_union _tmp247;union Cyc_CfFlowInfo_AbsLVal_union
_tmp248;struct _tuple7 _tmp246=Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmp167);
_tmp247=_tmp246.f1;_tmp248=_tmp246.f2;_tmp247=Cyc_CfFlowInfo_restore_consume_info(
_tmp247,_tmp245,0);{union Cyc_CfFlowInfo_AbsLVal_union _tmp249=_tmp248;struct Cyc_CfFlowInfo_Place*
_tmp24A;_LL190: if((_tmp249.UnknownL).tag != 1)goto _LL192;_LL191: return({struct
_tuple5 _tmp24B;_tmp24B.f1=_tmp247;_tmp24B.f2=(void*)1;_tmp24B;});_LL192: if((
_tmp249.PlaceL).tag != 0)goto _LL18F;_tmp24A=(_tmp249.PlaceL).f1;_LL193: return({
struct _tuple5 _tmp24C;_tmp24C.f1=_tmp247;_tmp24C.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp24D=_region_malloc(env->r,sizeof(*_tmp24D));_tmp24D[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp24E;_tmp24E.tag=2;_tmp24E.f1=_tmp24A;_tmp24E;});_tmp24D;});_tmp24C;});_LL18F:;}}}}
_LL113: if(*((int*)_tmp13B)!= 22)goto _LL115;_tmp168=((struct Cyc_Absyn_Deref_e_struct*)
_tmp13B)->f1;_LL114: {union Cyc_CfFlowInfo_FlowInfo_union _tmp250;void*_tmp251;
struct _tuple5 _tmp24F=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp168);_tmp250=
_tmp24F.f1;_tmp251=_tmp24F.f2;_tmp250=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp250);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp250,_tmp168,_tmp251);}
_LL115: if(*((int*)_tmp13B)!= 23)goto _LL117;_tmp169=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp13B)->f1;_tmp16A=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp13B)->f2;_LL116: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp253;void*_tmp254;struct _tuple5 _tmp252=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp169);_tmp253=_tmp252.f1;_tmp254=_tmp252.f2;_tmp253=Cyc_CfFlowInfo_drop_unique_rvals(
e->loc,_tmp253);_tmp253=Cyc_NewControlFlow_may_consume_exp(env,_tmp253,e);if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp169->topt))->v))return({struct
_tuple5 _tmp255;_tmp255.f1=_tmp253;_tmp255.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp134,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp134->unknown_all);
_tmp255;});{void*_tmp256=_tmp254;struct _dyneither_ptr _tmp257;_LL195: if(_tmp256 <= (
void*)3)goto _LL197;if(*((int*)_tmp256)!= 4)goto _LL197;_tmp257=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp256)->f1;_LL196: {int _tmp258=Cyc_CfFlowInfo_get_field_index((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp169->topt))->v,_tmp16A);return({struct _tuple5
_tmp259;_tmp259.f1=_tmp253;_tmp259.f2=*((void**)_check_dyneither_subscript(
_tmp257,sizeof(void*),_tmp258));_tmp259;});}_LL197:;_LL198:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp25A=_cycalloc(sizeof(*_tmp25A));_tmp25A[0]=({
struct Cyc_Core_Impossible_struct _tmp25B;_tmp25B.tag=Cyc_Core_Impossible;_tmp25B.f1=(
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp25E;_tmp25E.tag=0;_tmp25E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{void*
_tmp25C[1]={& _tmp25E};Cyc_aprintf(({const char*_tmp25D="anal_Rexp: AggrMember: %s";
_tag_dyneither(_tmp25D,sizeof(char),26);}),_tag_dyneither(_tmp25C,sizeof(void*),
1));}});_tmp25B;});_tmp25A;}));_LL194:;}}_LL117: if(*((int*)_tmp13B)!= 24)goto
_LL119;_tmp16B=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp13B)->f1;_tmp16C=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp13B)->f2;_LL118: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp260;void*_tmp261;struct _tuple5 _tmp25F=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp16B);_tmp260=_tmp25F.f1;_tmp261=_tmp25F.f2;_tmp260=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp260);{union Cyc_CfFlowInfo_FlowInfo_union _tmp263;void*_tmp264;struct
_tuple5 _tmp262=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp260,_tmp16B,_tmp261);
_tmp263=_tmp262.f1;_tmp264=_tmp262.f2;{void*_tmp265=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp16B->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp266;void*_tmp267;_LL19A: if(_tmp265 <= (void*)4)goto _LL19C;if(*((int*)_tmp265)
!= 4)goto _LL19C;_tmp266=((struct Cyc_Absyn_PointerType_struct*)_tmp265)->f1;
_tmp267=(void*)_tmp266.elt_typ;_LL19B: if(Cyc_Absyn_is_union_type(_tmp267))return({
struct _tuple5 _tmp268;_tmp268.f1=_tmp263;_tmp268.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp134,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp134->unknown_all);
_tmp268;});{void*_tmp269=_tmp264;struct _dyneither_ptr _tmp26A;_LL19F: if(_tmp269 <= (
void*)3)goto _LL1A1;if(*((int*)_tmp269)!= 4)goto _LL1A1;_tmp26A=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp269)->f1;_LL1A0: {int _tmp26B=Cyc_CfFlowInfo_get_field_index(_tmp267,_tmp16C);
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
_LL119: if(*((int*)_tmp13B)!= 6)goto _LL11B;_tmp16D=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp13B)->f1;_tmp16E=((struct Cyc_Absyn_Conditional_e_struct*)_tmp13B)->f2;
_tmp16F=((struct Cyc_Absyn_Conditional_e_struct*)_tmp13B)->f3;_LL11A: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp274;union Cyc_CfFlowInfo_FlowInfo_union _tmp275;struct _tuple8 _tmp273=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp16D);_tmp274=_tmp273.f1;_tmp275=_tmp273.f2;_tmp274=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp16D->loc,_tmp274);_tmp275=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp16D->loc,
_tmp275);{struct _tuple5 _tmp276=Cyc_NewControlFlow_anal_Rexp(env,_tmp274,_tmp16E);
struct _tuple5 _tmp277=Cyc_NewControlFlow_anal_Rexp(env,_tmp275,_tmp16F);return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp134,env->all_changed,_tmp276,_tmp277,1);}}_LL11B: if(*((int*)_tmp13B)!= 7)
goto _LL11D;_tmp170=((struct Cyc_Absyn_And_e_struct*)_tmp13B)->f1;_tmp171=((struct
Cyc_Absyn_And_e_struct*)_tmp13B)->f2;_LL11C: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp279;union Cyc_CfFlowInfo_FlowInfo_union _tmp27A;struct _tuple8 _tmp278=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp170);_tmp279=_tmp278.f1;_tmp27A=_tmp278.f2;_tmp279=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp170->loc,_tmp279);_tmp27A=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp170->loc,
_tmp27A);{union Cyc_CfFlowInfo_FlowInfo_union _tmp27C;void*_tmp27D;struct _tuple5
_tmp27B=Cyc_NewControlFlow_anal_Rexp(env,_tmp279,_tmp171);_tmp27C=_tmp27B.f1;
_tmp27D=_tmp27B.f2;_tmp27C=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp171->loc,
_tmp27C);{struct _tuple5 _tmp27E=({struct _tuple5 _tmp281;_tmp281.f1=_tmp27C;_tmp281.f2=
_tmp27D;_tmp281;});struct _tuple5 _tmp27F=({struct _tuple5 _tmp280;_tmp280.f1=
_tmp27A;_tmp280.f2=(void*)((void*)0);_tmp280;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp134,env->all_changed,_tmp27E,_tmp27F,0);}}}_LL11D: if(*((int*)_tmp13B)!= 8)
goto _LL11F;_tmp172=((struct Cyc_Absyn_Or_e_struct*)_tmp13B)->f1;_tmp173=((struct
Cyc_Absyn_Or_e_struct*)_tmp13B)->f2;_LL11E: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp283;union Cyc_CfFlowInfo_FlowInfo_union _tmp284;struct _tuple8 _tmp282=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp172);_tmp283=_tmp282.f1;_tmp284=_tmp282.f2;_tmp283=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp172->loc,_tmp283);_tmp284=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp172->loc,
_tmp284);{union Cyc_CfFlowInfo_FlowInfo_union _tmp286;void*_tmp287;struct _tuple5
_tmp285=Cyc_NewControlFlow_anal_Rexp(env,_tmp284,_tmp173);_tmp286=_tmp285.f1;
_tmp287=_tmp285.f2;_tmp286=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp173->loc,
_tmp286);{struct _tuple5 _tmp288=({struct _tuple5 _tmp28B;_tmp28B.f1=_tmp286;_tmp28B.f2=
_tmp287;_tmp28B;});struct _tuple5 _tmp289=({struct _tuple5 _tmp28A;_tmp28A.f1=
_tmp283;_tmp28A.f2=(void*)((void*)1);_tmp28A;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp134,env->all_changed,_tmp288,_tmp289,0);}}}_LL11F: if(*((int*)_tmp13B)!= 25)
goto _LL121;_tmp174=((struct Cyc_Absyn_Subscript_e_struct*)_tmp13B)->f1;_tmp175=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp13B)->f2;_LL120: {struct _RegionHandle*
_tmp28C=env->r;union Cyc_CfFlowInfo_FlowInfo_union _tmp28F;struct Cyc_List_List*
_tmp290;struct _tuple11 _tmp28E=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp28C,
env,inflow,({struct Cyc_Absyn_Exp*_tmp28D[2];_tmp28D[1]=_tmp175;_tmp28D[0]=
_tmp174;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
_tmp28C,_tag_dyneither(_tmp28D,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp28F=
_tmp28E.f1;_tmp290=_tmp28E.f2;_tmp28F=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp175->loc,_tmp28F);{union Cyc_CfFlowInfo_FlowInfo_union _tmp291=_tmp28F;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp292=_tmp28F;struct Cyc_Dict_Dict _tmp293;struct
Cyc_List_List*_tmp294;struct Cyc_CfFlowInfo_ConsumeInfo _tmp295;_LL1A4: if((_tmp292.ReachableFL).tag
!= 1)goto _LL1A6;_tmp293=(_tmp292.ReachableFL).f1;_tmp294=(_tmp292.ReachableFL).f2;
_tmp295=(_tmp292.ReachableFL).f3;_LL1A5: if(Cyc_CfFlowInfo_initlevel(env->fenv,
_tmp293,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp290))->tl))->hd)== (void*)0)({void*_tmp296=0;Cyc_Tcutil_terr(
_tmp175->loc,({const char*_tmp297="expression may not be initialized";
_tag_dyneither(_tmp297,sizeof(char),34);}),_tag_dyneither(_tmp296,sizeof(void*),
0));});{struct Cyc_List_List*_tmp298=Cyc_NewControlFlow_add_subscript_reln(
_tmp134->r,_tmp294,_tmp174,_tmp175);if(_tmp294 != _tmp298)_tmp291=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp299;(_tmp299.ReachableFL).tag=1;(_tmp299.ReachableFL).f1=
_tmp293;(_tmp299.ReachableFL).f2=_tmp298;(_tmp299.ReachableFL).f3=_tmp295;
_tmp299;});goto _LL1A3;}_LL1A6:;_LL1A7: goto _LL1A3;_LL1A3:;}{void*_tmp29A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp174->topt))->v);struct Cyc_List_List*
_tmp29B;struct Cyc_Absyn_PtrInfo _tmp29C;struct Cyc_Absyn_PtrAtts _tmp29D;struct Cyc_Absyn_Conref*
_tmp29E;_LL1A9: if(_tmp29A <= (void*)4)goto _LL1AD;if(*((int*)_tmp29A)!= 9)goto
_LL1AB;_tmp29B=((struct Cyc_Absyn_TupleType_struct*)_tmp29A)->f1;_LL1AA: {void*
_tmp29F=(void*)((struct Cyc_List_List*)_check_null(_tmp290))->hd;struct
_dyneither_ptr _tmp2A0;_LL1B0: if(_tmp29F <= (void*)3)goto _LL1B2;if(*((int*)_tmp29F)
!= 4)goto _LL1B2;_tmp2A0=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp29F)->f1;
_LL1B1: _tmp291=Cyc_NewControlFlow_may_consume_exp(env,_tmp291,e);{unsigned int i=(
Cyc_Evexp_eval_const_uint_exp(_tmp175)).f1;return({struct _tuple5 _tmp2A1;_tmp2A1.f1=
_tmp291;_tmp2A1.f2=*((void**)_check_dyneither_subscript(_tmp2A0,sizeof(void*),(
int)i));_tmp2A1;});}_LL1B2:;_LL1B3:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2[0]=({struct Cyc_Core_Impossible_struct
_tmp2A3;_tmp2A3.tag=Cyc_Core_Impossible;_tmp2A3.f1=({const char*_tmp2A4="anal_Rexp: Subscript";
_tag_dyneither(_tmp2A4,sizeof(char),21);});_tmp2A3;});_tmp2A2;}));_LL1AF:;}
_LL1AB: if(*((int*)_tmp29A)!= 4)goto _LL1AD;_tmp29C=((struct Cyc_Absyn_PointerType_struct*)
_tmp29A)->f1;_tmp29D=_tmp29C.ptr_atts;_tmp29E=_tmp29D.bounds;_LL1AC: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp2A6;void*_tmp2A7;struct _tuple5 _tmp2A5=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp28F,_tmp174,(void*)((struct Cyc_List_List*)_check_null(_tmp290))->hd);
_tmp2A6=_tmp2A5.f1;_tmp2A7=_tmp2A5.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp2A8=
_tmp2A6;_LL1B5: if((_tmp2A8.BottomFL).tag != 0)goto _LL1B7;_LL1B6: return({struct
_tuple5 _tmp2A9;_tmp2A9.f1=_tmp2A6;_tmp2A9.f2=_tmp2A7;_tmp2A9;});_LL1B7:;_LL1B8:
return({struct _tuple5 _tmp2AA;_tmp2AA.f1=_tmp291;_tmp2AA.f2=_tmp2A7;_tmp2AA;});
_LL1B4:;}}_LL1AD:;_LL1AE:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB[0]=({struct Cyc_Core_Impossible_struct
_tmp2AC;_tmp2AC.tag=Cyc_Core_Impossible;_tmp2AC.f1=({const char*_tmp2AD="anal_Rexp: Subscript -- bad type";
_tag_dyneither(_tmp2AD,sizeof(char),33);});_tmp2AC;});_tmp2AB;}));_LL1A8:;}}}
_LL121: if(*((int*)_tmp13B)!= 32)goto _LL123;_tmp176=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp13B)->f1;_tmp177=((struct Cyc_Absyn_Tunion_e_struct*)_tmp13B)->f2;_LL122: if(
_tmp177->is_flat){struct _RegionHandle*_tmp2AE=env->r;union Cyc_CfFlowInfo_FlowInfo_union
_tmp2B0;struct Cyc_List_List*_tmp2B1;struct _tuple11 _tmp2AF=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp2AE,env,inflow,_tmp176,0);_tmp2B0=_tmp2AF.f1;_tmp2B1=_tmp2AF.f2;_tmp2B0=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2B0);for(0;(unsigned int)_tmp176;(_tmp176=_tmp176->tl,_tmp2B1=_tmp2B1->tl)){
_tmp2B0=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)_tmp176->hd)->loc,
_tmp2B0,(void*)((struct Cyc_List_List*)_check_null(_tmp2B1))->hd);}return({struct
_tuple5 _tmp2B2;_tmp2B2.f1=_tmp2B0;_tmp2B2.f2=(void*)_tmp134->unknown_all;_tmp2B2;});}
_tmp178=_tmp176;goto _LL124;_LL123: if(*((int*)_tmp13B)!= 26)goto _LL125;_tmp178=((
struct Cyc_Absyn_Tuple_e_struct*)_tmp13B)->f1;_LL124: {struct _RegionHandle*
_tmp2B3=env->r;union Cyc_CfFlowInfo_FlowInfo_union _tmp2B5;struct Cyc_List_List*
_tmp2B6;struct _tuple11 _tmp2B4=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2B3,
env,inflow,_tmp178,0);_tmp2B5=_tmp2B4.f1;_tmp2B6=_tmp2B4.f2;_tmp2B5=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2B5);{struct _dyneither_ptr aggrdict=({unsigned int _tmp2BA=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp178);void**
_tmp2BB=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp2BA));
struct _dyneither_ptr _tmp2BE=_tag_dyneither(_tmp2BB,sizeof(void*),_tmp2BA);{
unsigned int _tmp2BC=_tmp2BA;unsigned int i;for(i=0;i < _tmp2BC;i ++){_tmp2BB[i]=({
void*_tmp2BD=(void*)((struct Cyc_List_List*)_check_null(_tmp2B6))->hd;_tmp2B6=
_tmp2B6->tl;_tmp2BD;});}}_tmp2BE;});return({struct _tuple5 _tmp2B7;_tmp2B7.f1=
_tmp2B5;_tmp2B7.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2B8=
_region_malloc(env->r,sizeof(*_tmp2B8));_tmp2B8[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp2B9;_tmp2B9.tag=4;_tmp2B9.f1=aggrdict;_tmp2B9;});_tmp2B8;});_tmp2B7;});}}
_LL125: if(*((int*)_tmp13B)!= 31)goto _LL127;_tmp179=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp13B)->f2;_LL126: {struct Cyc_List_List*fs;{void*_tmp2BF=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_List_List*
_tmp2C0;_LL1BA: if(_tmp2BF <= (void*)4)goto _LL1BC;if(*((int*)_tmp2BF)!= 11)goto
_LL1BC;_tmp2C0=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2BF)->f2;_LL1BB: fs=
_tmp2C0;goto _LL1B9;_LL1BC:;_LL1BD:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2C1=_cycalloc(sizeof(*_tmp2C1));_tmp2C1[0]=({struct Cyc_Core_Impossible_struct
_tmp2C2;_tmp2C2.tag=Cyc_Core_Impossible;_tmp2C2.f1=({const char*_tmp2C3="anal_Rexp:anon struct has bad type";
_tag_dyneither(_tmp2C3,sizeof(char),35);});_tmp2C2;});_tmp2C1;}));_LL1B9:;}
_tmp17A=_tmp179;_tmp17F=fs;goto _LL128;}_LL127: if(*((int*)_tmp13B)!= 30)goto
_LL129;_tmp17A=((struct Cyc_Absyn_Struct_e_struct*)_tmp13B)->f3;_tmp17B=((struct
Cyc_Absyn_Struct_e_struct*)_tmp13B)->f4;if(_tmp17B == 0)goto _LL129;_tmp17C=*
_tmp17B;_tmp17D=_tmp17C.impl;if(_tmp17D == 0)goto _LL129;_tmp17E=*_tmp17D;_tmp17F=
_tmp17E.fields;_LL128: {struct _RegionHandle*_tmp2C4=env->r;union Cyc_CfFlowInfo_FlowInfo_union
_tmp2C6;struct Cyc_List_List*_tmp2C7;struct _tuple11 _tmp2C5=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp2C4,env,inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp2C4,(struct Cyc_Absyn_Exp*(*)(
struct _tuple14*))Cyc_Core_snd,_tmp17A),0);_tmp2C6=_tmp2C5.f1;_tmp2C7=_tmp2C5.f2;
_tmp2C6=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp2C6);{struct
_dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp134,_tmp17F,(
void*)_tmp134->unknown_all);{int i=0;for(0;_tmp2C7 != 0;(((_tmp2C7=_tmp2C7->tl,
_tmp17A=_tmp17A->tl)),++ i)){struct Cyc_List_List*ds=(*((struct _tuple14*)((struct
Cyc_List_List*)_check_null(_tmp17A))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*
_tmp2C8=(void*)ds->hd;struct _dyneither_ptr*_tmp2C9;_LL1BF: if(*((int*)_tmp2C8)!= 
0)goto _LL1C1;_LL1C0:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2CA=_cycalloc(sizeof(*_tmp2CA));_tmp2CA[0]=({struct Cyc_Core_Impossible_struct
_tmp2CB;_tmp2CB.tag=Cyc_Core_Impossible;_tmp2CB.f1=({const char*_tmp2CC="anal_Rexp:Struct_e";
_tag_dyneither(_tmp2CC,sizeof(char),19);});_tmp2CB;});_tmp2CA;}));_LL1C1: if(*((
int*)_tmp2C8)!= 1)goto _LL1BE;_tmp2C9=((struct Cyc_Absyn_FieldName_struct*)_tmp2C8)->f1;
_LL1C2: {int _tmp2CD=Cyc_CfFlowInfo_get_field_index_fs(_tmp17F,_tmp2C9);*((void**)
_check_dyneither_subscript(aggrdict,sizeof(void*),_tmp2CD))=(void*)_tmp2C7->hd;}
_LL1BE:;}}}return({struct _tuple5 _tmp2CE;_tmp2CE.f1=_tmp2C6;_tmp2CE.f2=(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2CF=_region_malloc(env->r,sizeof(*
_tmp2CF));_tmp2CF[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp2D0;_tmp2D0.tag=
4;_tmp2D0.f1=aggrdict;_tmp2D0;});_tmp2CF;});_tmp2CE;});}}_LL129: if(*((int*)
_tmp13B)!= 30)goto _LL12B;_LL12A:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2D1=_cycalloc(sizeof(*_tmp2D1));_tmp2D1[0]=({struct Cyc_Core_Impossible_struct
_tmp2D2;_tmp2D2.tag=Cyc_Core_Impossible;_tmp2D2.f1=({const char*_tmp2D3="anal_Rexp:missing aggrdeclimpl";
_tag_dyneither(_tmp2D3,sizeof(char),31);});_tmp2D2;});_tmp2D1;}));_LL12B: if(*((
int*)_tmp13B)!= 28)goto _LL12D;_tmp180=((struct Cyc_Absyn_Array_e_struct*)_tmp13B)->f1;
_LL12C: {struct _RegionHandle*_tmp2D4=env->r;struct Cyc_List_List*_tmp2D5=((struct
Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmp2D4,(struct Cyc_Absyn_Exp*(*)(struct
_tuple14*))Cyc_Core_snd,_tmp180);union Cyc_CfFlowInfo_FlowInfo_union _tmp2D7;
struct Cyc_List_List*_tmp2D8;struct _tuple11 _tmp2D6=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp2D4,env,inflow,_tmp2D5,0);_tmp2D7=_tmp2D6.f1;_tmp2D8=_tmp2D6.f2;_tmp2D7=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2D7);for(0;_tmp2D8 != 0;(_tmp2D8=_tmp2D8->tl,_tmp2D5=_tmp2D5->tl)){
_tmp2D7=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2D5))->hd)->loc,_tmp2D7,(void*)_tmp2D8->hd);}return({struct
_tuple5 _tmp2D9;_tmp2D9.f1=_tmp2D7;_tmp2D9.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp134,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp134->unknown_all);
_tmp2D9;});}_LL12D: if(*((int*)_tmp13B)!= 29)goto _LL12F;_tmp181=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp13B)->f1;_tmp182=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp13B)->f2;
_tmp183=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp13B)->f3;_tmp184=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp13B)->f4;_LL12E: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp2DB;void*_tmp2DC;struct _tuple5 _tmp2DA=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp182);_tmp2DB=_tmp2DA.f1;_tmp2DC=_tmp2DA.f2;_tmp2DB=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp182->loc,_tmp2DB);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2DD=_tmp2DB;struct
Cyc_Dict_Dict _tmp2DE;struct Cyc_List_List*_tmp2DF;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2E0;_LL1C4: if((_tmp2DD.BottomFL).tag != 0)goto _LL1C6;_LL1C5: return({struct
_tuple5 _tmp2E1;_tmp2E1.f1=_tmp2DB;_tmp2E1.f2=(void*)_tmp134->unknown_all;_tmp2E1;});
_LL1C6: if((_tmp2DD.ReachableFL).tag != 1)goto _LL1C3;_tmp2DE=(_tmp2DD.ReachableFL).f1;
_tmp2DF=(_tmp2DD.ReachableFL).f2;_tmp2E0=(_tmp2DD.ReachableFL).f3;_LL1C7: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp2DE,_tmp2DC)== (void*)0)({void*_tmp2E2=0;Cyc_Tcutil_terr(_tmp182->loc,({
const char*_tmp2E3="expression may not be initialized";_tag_dyneither(_tmp2E3,
sizeof(char),34);}),_tag_dyneither(_tmp2E2,sizeof(void*),0));});{struct Cyc_List_List*
new_relns=_tmp2DF;comp_loop: {void*_tmp2E4=(void*)_tmp182->r;struct Cyc_Absyn_Exp*
_tmp2E5;void*_tmp2E6;struct Cyc_Absyn_Vardecl*_tmp2E7;void*_tmp2E8;struct Cyc_Absyn_Vardecl*
_tmp2E9;void*_tmp2EA;struct Cyc_Absyn_Vardecl*_tmp2EB;void*_tmp2EC;struct Cyc_Absyn_Vardecl*
_tmp2ED;union Cyc_Absyn_Cnst_union _tmp2EE;int _tmp2EF;void*_tmp2F0;struct Cyc_List_List*
_tmp2F1;struct Cyc_List_List _tmp2F2;struct Cyc_Absyn_Exp*_tmp2F3;_LL1C9: if(*((int*)
_tmp2E4)!= 15)goto _LL1CB;_tmp2E5=((struct Cyc_Absyn_Cast_e_struct*)_tmp2E4)->f2;
_LL1CA: _tmp182=_tmp2E5;goto comp_loop;_LL1CB: if(*((int*)_tmp2E4)!= 1)goto _LL1CD;
_tmp2E6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E4)->f2;if(_tmp2E6 <= (void*)
1)goto _LL1CD;if(*((int*)_tmp2E6)!= 0)goto _LL1CD;_tmp2E7=((struct Cyc_Absyn_Global_b_struct*)
_tmp2E6)->f1;if(!(!_tmp2E7->escapes))goto _LL1CD;_LL1CC: _tmp2E9=_tmp2E7;goto
_LL1CE;_LL1CD: if(*((int*)_tmp2E4)!= 1)goto _LL1CF;_tmp2E8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E4)->f2;if(_tmp2E8 <= (void*)1)goto _LL1CF;if(*((int*)_tmp2E8)!= 3)goto _LL1CF;
_tmp2E9=((struct Cyc_Absyn_Local_b_struct*)_tmp2E8)->f1;if(!(!_tmp2E9->escapes))
goto _LL1CF;_LL1CE: _tmp2EB=_tmp2E9;goto _LL1D0;_LL1CF: if(*((int*)_tmp2E4)!= 1)goto
_LL1D1;_tmp2EA=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E4)->f2;if(_tmp2EA <= (
void*)1)goto _LL1D1;if(*((int*)_tmp2EA)!= 4)goto _LL1D1;_tmp2EB=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2EA)->f1;if(!(!_tmp2EB->escapes))goto _LL1D1;_LL1D0: _tmp2ED=_tmp2EB;goto
_LL1D2;_LL1D1: if(*((int*)_tmp2E4)!= 1)goto _LL1D3;_tmp2EC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E4)->f2;if(_tmp2EC <= (void*)1)goto _LL1D3;if(*((int*)_tmp2EC)!= 2)goto _LL1D3;
_tmp2ED=((struct Cyc_Absyn_Param_b_struct*)_tmp2EC)->f1;if(!(!_tmp2ED->escapes))
goto _LL1D3;_LL1D2: new_relns=({struct Cyc_List_List*_tmp2F4=_region_malloc(env->r,
sizeof(*_tmp2F4));_tmp2F4->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2F5=_region_malloc(
env->r,sizeof(*_tmp2F5));_tmp2F5->vd=_tmp181;_tmp2F5->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp2F6;(_tmp2F6.LessVar).tag=1;(_tmp2F6.LessVar).f1=
_tmp2ED;(_tmp2F6.LessVar).f2=(void*)((void*)_tmp2ED->type);_tmp2F6;});_tmp2F5;});
_tmp2F4->tl=_tmp2DF;_tmp2F4;});goto _LL1C8;_LL1D3: if(*((int*)_tmp2E4)!= 0)goto
_LL1D5;_tmp2EE=((struct Cyc_Absyn_Const_e_struct*)_tmp2E4)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp2E4)->f1).Int_c).tag != 2)goto _LL1D5;_tmp2EF=(_tmp2EE.Int_c).f2;_LL1D4:
new_relns=({struct Cyc_List_List*_tmp2F7=_region_malloc(env->r,sizeof(*_tmp2F7));
_tmp2F7->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2F8=_region_malloc(env->r,sizeof(*
_tmp2F8));_tmp2F8->vd=_tmp181;_tmp2F8->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp2F9;(_tmp2F9.LessConst).tag=3;(_tmp2F9.LessConst).f1=(
unsigned int)_tmp2EF;_tmp2F9;});_tmp2F8;});_tmp2F7->tl=_tmp2DF;_tmp2F7;});goto
_LL1C8;_LL1D5: if(*((int*)_tmp2E4)!= 3)goto _LL1D7;_tmp2F0=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp2E4)->f1;_tmp2F1=((struct Cyc_Absyn_Primop_e_struct*)_tmp2E4)->f2;if(_tmp2F1
== 0)goto _LL1D7;_tmp2F2=*_tmp2F1;_tmp2F3=(struct Cyc_Absyn_Exp*)_tmp2F2.hd;_LL1D6:{
void*_tmp2FA=(void*)_tmp2F3->r;void*_tmp2FB;struct Cyc_Absyn_Vardecl*_tmp2FC;void*
_tmp2FD;struct Cyc_Absyn_Vardecl*_tmp2FE;void*_tmp2FF;struct Cyc_Absyn_Vardecl*
_tmp300;void*_tmp301;struct Cyc_Absyn_Vardecl*_tmp302;_LL1DA: if(*((int*)_tmp2FA)
!= 1)goto _LL1DC;_tmp2FB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2FA)->f2;if(
_tmp2FB <= (void*)1)goto _LL1DC;if(*((int*)_tmp2FB)!= 0)goto _LL1DC;_tmp2FC=((
struct Cyc_Absyn_Global_b_struct*)_tmp2FB)->f1;if(!(!_tmp2FC->escapes))goto _LL1DC;
_LL1DB: _tmp2FE=_tmp2FC;goto _LL1DD;_LL1DC: if(*((int*)_tmp2FA)!= 1)goto _LL1DE;
_tmp2FD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2FA)->f2;if(_tmp2FD <= (void*)
1)goto _LL1DE;if(*((int*)_tmp2FD)!= 3)goto _LL1DE;_tmp2FE=((struct Cyc_Absyn_Local_b_struct*)
_tmp2FD)->f1;if(!(!_tmp2FE->escapes))goto _LL1DE;_LL1DD: _tmp300=_tmp2FE;goto
_LL1DF;_LL1DE: if(*((int*)_tmp2FA)!= 1)goto _LL1E0;_tmp2FF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2FA)->f2;if(_tmp2FF <= (void*)1)goto _LL1E0;if(*((int*)_tmp2FF)!= 4)goto _LL1E0;
_tmp300=((struct Cyc_Absyn_Pat_b_struct*)_tmp2FF)->f1;if(!(!_tmp300->escapes))
goto _LL1E0;_LL1DF: _tmp302=_tmp300;goto _LL1E1;_LL1E0: if(*((int*)_tmp2FA)!= 1)goto
_LL1E2;_tmp301=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2FA)->f2;if(_tmp301 <= (
void*)1)goto _LL1E2;if(*((int*)_tmp301)!= 2)goto _LL1E2;_tmp302=((struct Cyc_Absyn_Param_b_struct*)
_tmp301)->f1;if(!(!_tmp302->escapes))goto _LL1E2;_LL1E1: new_relns=({struct Cyc_List_List*
_tmp303=_region_malloc(env->r,sizeof(*_tmp303));_tmp303->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp304=_region_malloc(env->r,sizeof(*_tmp304));_tmp304->vd=_tmp181;_tmp304->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp305;(
_tmp305.LessNumelts).tag=2;(_tmp305.LessNumelts).f1=_tmp302;_tmp305;});_tmp304;});
_tmp303->tl=_tmp2DF;_tmp303;});goto _LL1D9;_LL1E2:;_LL1E3: goto _LL1D9;_LL1D9:;}
goto _LL1C8;_LL1D7:;_LL1D8: goto _LL1C8;_LL1C8:;}if(_tmp2DF != new_relns)_tmp2DB=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp306;(
_tmp306.ReachableFL).tag=1;(_tmp306.ReachableFL).f1=_tmp2DE;(_tmp306.ReachableFL).f2=
new_relns;(_tmp306.ReachableFL).f3=_tmp2E0;_tmp306;});{void*_tmp307=_tmp2DC;
_LL1E5: if((int)_tmp307 != 0)goto _LL1E7;_LL1E6: return({struct _tuple5 _tmp308;
_tmp308.f1=_tmp2DB;_tmp308.f2=(void*)_tmp134->unknown_all;_tmp308;});_LL1E7: if((
int)_tmp307 != 2)goto _LL1E9;_LL1E8: goto _LL1EA;_LL1E9: if((int)_tmp307 != 1)goto
_LL1EB;_LL1EA: goto _LL1EC;_LL1EB: if(_tmp307 <= (void*)3)goto _LL1ED;if(*((int*)
_tmp307)!= 2)goto _LL1ED;_LL1EC: {struct Cyc_List_List _tmp309=({struct Cyc_List_List
_tmp315;_tmp315.hd=_tmp181;_tmp315.tl=0;_tmp315;});_tmp2DB=Cyc_NewControlFlow_add_vars(
_tmp134,_tmp2DB,(struct Cyc_List_List*)& _tmp309,(void*)_tmp134->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp30B;void*_tmp30C;struct _tuple5 _tmp30A=Cyc_NewControlFlow_anal_Rexp(
env,_tmp2DB,_tmp183);_tmp30B=_tmp30A.f1;_tmp30C=_tmp30A.f2;_tmp30B=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp183->loc,_tmp30B);{union Cyc_CfFlowInfo_FlowInfo_union _tmp30D=_tmp30B;struct
Cyc_Dict_Dict _tmp30E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp30F;_LL1F0: if((_tmp30D.BottomFL).tag
!= 0)goto _LL1F2;_LL1F1: return({struct _tuple5 _tmp310;_tmp310.f1=_tmp30B;_tmp310.f2=(
void*)_tmp134->unknown_all;_tmp310;});_LL1F2: if((_tmp30D.ReachableFL).tag != 1)
goto _LL1EF;_tmp30E=(_tmp30D.ReachableFL).f1;_tmp30F=(_tmp30D.ReachableFL).f3;
_LL1F3: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp30E,_tmp30C)!= (void*)2){({void*
_tmp311=0;Cyc_Tcutil_terr(_tmp182->loc,({const char*_tmp312="expression may not be initialized";
_tag_dyneither(_tmp312,sizeof(char),34);}),_tag_dyneither(_tmp311,sizeof(void*),
0));});return({struct _tuple5 _tmp313;_tmp313.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp314;(_tmp314.BottomFL).tag=0;_tmp314;});
_tmp313.f2=(void*)_tmp134->unknown_all;_tmp313;});}_LL1EF:;}_tmp2DB=_tmp30B;goto
_LL1EE;}}_LL1ED:;_LL1EE: while(1){struct Cyc_List_List _tmp316=({struct Cyc_List_List
_tmp322;_tmp322.hd=_tmp181;_tmp322.tl=0;_tmp322;});_tmp2DB=Cyc_NewControlFlow_add_vars(
_tmp134,_tmp2DB,(struct Cyc_List_List*)& _tmp316,(void*)_tmp134->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp318;void*_tmp319;struct _tuple5 _tmp317=Cyc_NewControlFlow_anal_Rexp(
env,_tmp2DB,_tmp183);_tmp318=_tmp317.f1;_tmp319=_tmp317.f2;_tmp318=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp183->loc,_tmp318);{union Cyc_CfFlowInfo_FlowInfo_union _tmp31A=_tmp318;struct
Cyc_Dict_Dict _tmp31B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp31C;_LL1F5: if((_tmp31A.BottomFL).tag
!= 0)goto _LL1F7;_LL1F6: goto _LL1F4;_LL1F7: if((_tmp31A.ReachableFL).tag != 1)goto
_LL1F4;_tmp31B=(_tmp31A.ReachableFL).f1;_tmp31C=(_tmp31A.ReachableFL).f3;_LL1F8:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp31B,_tmp319)!= (void*)2){({void*
_tmp31D=0;Cyc_Tcutil_terr(_tmp182->loc,({const char*_tmp31E="expression may not be initialized";
_tag_dyneither(_tmp31E,sizeof(char),34);}),_tag_dyneither(_tmp31D,sizeof(void*),
0));});return({struct _tuple5 _tmp31F;_tmp31F.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp320;(_tmp320.BottomFL).tag=0;_tmp320;});
_tmp31F.f2=(void*)_tmp134->unknown_all;_tmp31F;});}_LL1F4:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp321=Cyc_CfFlowInfo_join_flow(_tmp134,env->all_changed,_tmp2DB,_tmp318,1);if(
Cyc_CfFlowInfo_flow_lessthan_approx(_tmp321,_tmp2DB))break;_tmp2DB=_tmp321;}}}
return({struct _tuple5 _tmp323;_tmp323.f1=_tmp2DB;_tmp323.f2=(void*)_tmp134->unknown_all;
_tmp323;});_LL1E4:;}}_LL1C3:;}}_LL12F: if(*((int*)_tmp13B)!= 38)goto _LL131;
_tmp185=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp13B)->f1;_LL130: while(1){union
Cyc_CfFlowInfo_FlowInfo_union*_tmp325;struct _tuple9 _tmp324=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,_tmp185);_tmp325=_tmp324.f2;inflow=*_tmp325;{void*_tmp326=(void*)
_tmp185->r;struct Cyc_Absyn_Stmt*_tmp327;struct Cyc_Absyn_Stmt*_tmp328;struct Cyc_Absyn_Decl*
_tmp329;struct Cyc_Absyn_Stmt*_tmp32A;struct Cyc_Absyn_Exp*_tmp32B;_LL1FA: if(
_tmp326 <= (void*)1)goto _LL200;if(*((int*)_tmp326)!= 1)goto _LL1FC;_tmp327=((
struct Cyc_Absyn_Seq_s_struct*)_tmp326)->f1;_tmp328=((struct Cyc_Absyn_Seq_s_struct*)
_tmp326)->f2;_LL1FB: inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp327);
_tmp185=_tmp328;goto _LL1F9;_LL1FC: if(*((int*)_tmp326)!= 11)goto _LL1FE;_tmp329=((
struct Cyc_Absyn_Decl_s_struct*)_tmp326)->f1;_tmp32A=((struct Cyc_Absyn_Decl_s_struct*)
_tmp326)->f2;_LL1FD: inflow=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp329);
_tmp185=_tmp32A;goto _LL1F9;_LL1FE: if(*((int*)_tmp326)!= 0)goto _LL200;_tmp32B=((
struct Cyc_Absyn_Exp_s_struct*)_tmp326)->f1;_LL1FF: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp32B);_LL200:;_LL201:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp32C=_cycalloc(sizeof(*_tmp32C));_tmp32C[0]=({struct Cyc_Core_Impossible_struct
_tmp32D;_tmp32D.tag=Cyc_Core_Impossible;_tmp32D.f1=({const char*_tmp32E="analyze_Rexp: ill-formed StmtExp";
_tag_dyneither(_tmp32E,sizeof(char),33);});_tmp32D;});_tmp32C;}));_LL1F9:;}}
_LL131: if(*((int*)_tmp13B)!= 1)goto _LL133;_tmp186=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13B)->f2;if((int)_tmp186 != 0)goto _LL133;_LL132: goto _LL134;_LL133: if(*((int*)
_tmp13B)!= 2)goto _LL135;_LL134: goto _LL136;_LL135: if(*((int*)_tmp13B)!= 10)goto
_LL137;_LL136: goto _LL138;_LL137: if(*((int*)_tmp13B)!= 37)goto _LL139;_LL138: goto
_LL13A;_LL139: if(*((int*)_tmp13B)!= 27)goto _LL13B;_LL13A: goto _LL13C;_LL13B: if(*((
int*)_tmp13B)!= 39)goto _LLD4;_LL13C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp32F=_cycalloc(sizeof(*_tmp32F));_tmp32F[0]=({struct Cyc_Core_Impossible_struct
_tmp330;_tmp330.tag=Cyc_Core_Impossible;_tmp330.f1=({const char*_tmp331="anal_Rexp, unexpected exp form";
_tag_dyneither(_tmp331,sizeof(char),31);});_tmp330;});_tmp32F;}));_LLD4:;}}
static struct _tuple7 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo_union
f,void*r,struct Cyc_List_List*flds){struct Cyc_CfFlowInfo_FlowEnv*_tmp332=env->fenv;
void*_tmp333=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp334;void*_tmp335;struct Cyc_Absyn_PtrAtts _tmp336;
struct Cyc_Absyn_Conref*_tmp337;struct Cyc_Absyn_Conref*_tmp338;_LL203: if(_tmp333
<= (void*)4)goto _LL205;if(*((int*)_tmp333)!= 4)goto _LL205;_tmp334=((struct Cyc_Absyn_PointerType_struct*)
_tmp333)->f1;_tmp335=(void*)_tmp334.elt_typ;_tmp336=_tmp334.ptr_atts;_tmp337=
_tmp336.bounds;_tmp338=_tmp336.zero_term;_LL204: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp339=f;struct Cyc_Dict_Dict _tmp33A;struct Cyc_List_List*_tmp33B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp33C;_LL208: if((_tmp339.BottomFL).tag != 0)goto _LL20A;_LL209: return({struct
_tuple7 _tmp33D;_tmp33D.f1=f;_tmp33D.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp33E;(_tmp33E.UnknownL).tag=1;_tmp33E;});
_tmp33D;});_LL20A: if((_tmp339.ReachableFL).tag != 1)goto _LL207;_tmp33A=(_tmp339.ReachableFL).f1;
_tmp33B=(_tmp339.ReachableFL).f2;_tmp33C=(_tmp339.ReachableFL).f3;_LL20B: if(Cyc_Tcutil_is_bound_one(
_tmp337)){void*_tmp33F=r;struct Cyc_CfFlowInfo_Place*_tmp340;struct Cyc_CfFlowInfo_Place
_tmp341;void*_tmp342;struct Cyc_List_List*_tmp343;void*_tmp344;_LL20D: if((int)
_tmp33F != 1)goto _LL20F;_LL20E: goto _LL210;_LL20F: if((int)_tmp33F != 2)goto _LL211;
_LL210:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp345=_cycalloc(sizeof(*_tmp345));_tmp345[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp346;_tmp346.tag=Cyc_CfFlowInfo_NotZero;_tmp346.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp33B);_tmp346;});_tmp345;})));goto _LL20C;_LL211: if(
_tmp33F <= (void*)3)goto _LL213;if(*((int*)_tmp33F)!= 2)goto _LL213;_tmp340=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp33F)->f1;_tmp341=*_tmp340;_tmp342=(
void*)_tmp341.root;_tmp343=_tmp341.fields;_LL212:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_NotZero_struct*_tmp347=_cycalloc(sizeof(*_tmp347));_tmp347[
0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp348;_tmp348.tag=Cyc_CfFlowInfo_NotZero;
_tmp348.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp33B);_tmp348;});
_tmp347;})));return({struct _tuple7 _tmp349;_tmp349.f1=f;_tmp349.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp34A;(_tmp34A.PlaceL).tag=0;(_tmp34A.PlaceL).f1=({
struct Cyc_CfFlowInfo_Place*_tmp34B=_region_malloc(_tmp332->r,sizeof(*_tmp34B));
_tmp34B->root=(void*)_tmp342;_tmp34B->fields=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp332->r,_tmp343,flds);_tmp34B;});_tmp34A;});_tmp349;});_LL213: if((int)_tmp33F
!= 0)goto _LL215;_LL214:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));
return({struct _tuple7 _tmp34C;_tmp34C.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp34E;(_tmp34E.BottomFL).tag=0;_tmp34E;});
_tmp34C.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp34D;(_tmp34D.UnknownL).tag=1;_tmp34D;});_tmp34C;});_LL215: if(_tmp33F <= (void*)
3)goto _LL217;if(*((int*)_tmp33F)!= 0)goto _LL217;_tmp344=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp33F)->f1;_LL216: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp344);goto
_LL218;_LL217:;_LL218:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp34F=_cycalloc(sizeof(*_tmp34F));_tmp34F[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp350;_tmp350.tag=Cyc_CfFlowInfo_UnknownZ;_tmp350.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp33B);_tmp350;});_tmp34F;})));_LL20C:;}else{(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp351=_cycalloc(sizeof(*
_tmp351));_tmp351[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp352;_tmp352.tag=
Cyc_CfFlowInfo_UnknownZ;_tmp352.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp33B);_tmp352;});_tmp351;})));}if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp33A,
r)== (void*)0)({void*_tmp353=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp354="dereference of possibly uninitialized pointer";
_tag_dyneither(_tmp354,sizeof(char),46);}),_tag_dyneither(_tmp353,sizeof(void*),
0));});return({struct _tuple7 _tmp355;_tmp355.f1=f;_tmp355.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp356;(_tmp356.UnknownL).tag=1;_tmp356;});
_tmp355;});_LL207:;}_LL205:;_LL206:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp357=_cycalloc(sizeof(*_tmp357));_tmp357[0]=({struct Cyc_Core_Impossible_struct
_tmp358;_tmp358.tag=Cyc_Core_Impossible;_tmp358.f1=({const char*_tmp359="left deref of non-pointer-type";
_tag_dyneither(_tmp359,sizeof(char),31);});_tmp358;});_tmp357;}));_LL202:;}
static struct _tuple7 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*_tmp35A=env->fenv;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp35B=inflow;struct Cyc_Dict_Dict _tmp35C;struct Cyc_List_List*
_tmp35D;_LL21A: if((_tmp35B.BottomFL).tag != 0)goto _LL21C;_LL21B: return({struct
_tuple7 _tmp35E;_tmp35E.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp360;(_tmp360.BottomFL).tag=0;_tmp360;});_tmp35E.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp35F;(_tmp35F.UnknownL).tag=1;_tmp35F;});
_tmp35E;});_LL21C: if((_tmp35B.ReachableFL).tag != 1)goto _LL219;_tmp35C=(_tmp35B.ReachableFL).f1;
_tmp35D=(_tmp35B.ReachableFL).f2;_LL21D: d=_tmp35C;_LL219:;}{void*_tmp361=(void*)
e->r;struct Cyc_Absyn_Exp*_tmp362;struct Cyc_Absyn_Exp*_tmp363;struct Cyc_Absyn_Exp*
_tmp364;void*_tmp365;struct Cyc_Absyn_Vardecl*_tmp366;void*_tmp367;struct Cyc_Absyn_Vardecl*
_tmp368;void*_tmp369;struct Cyc_Absyn_Vardecl*_tmp36A;void*_tmp36B;struct Cyc_Absyn_Vardecl*
_tmp36C;struct Cyc_Absyn_Exp*_tmp36D;struct _dyneither_ptr*_tmp36E;struct Cyc_Absyn_Exp*
_tmp36F;struct Cyc_Absyn_Exp*_tmp370;struct Cyc_Absyn_Exp*_tmp371;struct Cyc_Absyn_Exp*
_tmp372;struct _dyneither_ptr*_tmp373;_LL21F: if(*((int*)_tmp361)!= 15)goto _LL221;
_tmp362=((struct Cyc_Absyn_Cast_e_struct*)_tmp361)->f2;_LL220: _tmp363=_tmp362;
goto _LL222;_LL221: if(*((int*)_tmp361)!= 13)goto _LL223;_tmp363=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp361)->f1;_LL222: _tmp364=_tmp363;goto _LL224;_LL223: if(*((int*)_tmp361)!= 14)
goto _LL225;_tmp364=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp361)->f1;_LL224:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp364,flds);_LL225: if(*((int*)
_tmp361)!= 1)goto _LL227;_tmp365=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp361)->f2;
if(_tmp365 <= (void*)1)goto _LL227;if(*((int*)_tmp365)!= 2)goto _LL227;_tmp366=((
struct Cyc_Absyn_Param_b_struct*)_tmp365)->f1;_LL226: _tmp368=_tmp366;goto _LL228;
_LL227: if(*((int*)_tmp361)!= 1)goto _LL229;_tmp367=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp361)->f2;if(_tmp367 <= (void*)1)goto _LL229;if(*((int*)_tmp367)!= 3)goto _LL229;
_tmp368=((struct Cyc_Absyn_Local_b_struct*)_tmp367)->f1;_LL228: _tmp36A=_tmp368;
goto _LL22A;_LL229: if(*((int*)_tmp361)!= 1)goto _LL22B;_tmp369=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp361)->f2;if(_tmp369 <= (void*)1)goto _LL22B;if(*((int*)_tmp369)!= 4)goto _LL22B;
_tmp36A=((struct Cyc_Absyn_Pat_b_struct*)_tmp369)->f1;_LL22A: return({struct
_tuple7 _tmp374;_tmp374.f1=inflow;_tmp374.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp375;(_tmp375.PlaceL).tag=0;(_tmp375.PlaceL).f1=({
struct Cyc_CfFlowInfo_Place*_tmp376=_region_malloc(env->r,sizeof(*_tmp376));
_tmp376->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp377=
_region_malloc(env->r,sizeof(*_tmp377));_tmp377[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp378;_tmp378.tag=0;_tmp378.f1=_tmp36A;_tmp378;});_tmp377;}));_tmp376->fields=
flds;_tmp376;});_tmp375;});_tmp374;});_LL22B: if(*((int*)_tmp361)!= 1)goto _LL22D;
_tmp36B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp361)->f2;if(_tmp36B <= (void*)
1)goto _LL22D;if(*((int*)_tmp36B)!= 0)goto _LL22D;_tmp36C=((struct Cyc_Absyn_Global_b_struct*)
_tmp36B)->f1;_LL22C: return({struct _tuple7 _tmp379;_tmp379.f1=inflow;_tmp379.f2=(
union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union _tmp37A;(
_tmp37A.UnknownL).tag=1;_tmp37A;});_tmp379;});_LL22D: if(*((int*)_tmp361)!= 24)
goto _LL22F;_tmp36D=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp361)->f1;_tmp36E=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp361)->f2;_LL22E:{void*_tmp37B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp36D->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp37C;void*_tmp37D;_LL238: if(_tmp37B <= (void*)4)goto _LL23A;if(*((int*)_tmp37B)
!= 4)goto _LL23A;_tmp37C=((struct Cyc_Absyn_PointerType_struct*)_tmp37B)->f1;
_tmp37D=(void*)_tmp37C.elt_typ;_LL239: if(!Cyc_Absyn_is_union_type(_tmp37D))flds=({
struct Cyc_List_List*_tmp37E=_region_malloc(env->r,sizeof(*_tmp37E));_tmp37E->hd=(
void*)Cyc_CfFlowInfo_get_field_index(_tmp37D,_tmp36E);_tmp37E->tl=flds;_tmp37E;});
goto _LL237;_LL23A:;_LL23B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp37F=_cycalloc(sizeof(*_tmp37F));_tmp37F[0]=({struct Cyc_Core_Impossible_struct
_tmp380;_tmp380.tag=Cyc_Core_Impossible;_tmp380.f1=({const char*_tmp381="anal_Rexp: AggrArrow ptr";
_tag_dyneither(_tmp381,sizeof(char),25);});_tmp380;});_tmp37F;}));_LL237:;}
_tmp36F=_tmp36D;goto _LL230;_LL22F: if(*((int*)_tmp361)!= 22)goto _LL231;_tmp36F=((
struct Cyc_Absyn_Deref_e_struct*)_tmp361)->f1;_LL230: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp383;void*_tmp384;struct _tuple5 _tmp382=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp36F);_tmp383=_tmp382.f1;_tmp384=_tmp382.f2;_tmp383=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp383);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp36F,_tmp383,
_tmp384,flds);}_LL231: if(*((int*)_tmp361)!= 25)goto _LL233;_tmp370=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp361)->f1;_tmp371=((struct Cyc_Absyn_Subscript_e_struct*)_tmp361)->f2;_LL232: {
void*_tmp385=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp370->topt))->v);struct Cyc_Absyn_PtrInfo _tmp386;struct Cyc_Absyn_PtrAtts
_tmp387;struct Cyc_Absyn_Conref*_tmp388;_LL23D: if(_tmp385 <= (void*)4)goto _LL241;
if(*((int*)_tmp385)!= 9)goto _LL23F;_LL23E: {unsigned int _tmp389=(Cyc_Evexp_eval_const_uint_exp(
_tmp371)).f1;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp370,({struct
Cyc_List_List*_tmp38A=_region_malloc(env->r,sizeof(*_tmp38A));_tmp38A->hd=(void*)((
int)_tmp389);_tmp38A->tl=flds;_tmp38A;}));}_LL23F: if(*((int*)_tmp385)!= 4)goto
_LL241;_tmp386=((struct Cyc_Absyn_PointerType_struct*)_tmp385)->f1;_tmp387=
_tmp386.ptr_atts;_tmp388=_tmp387.bounds;_LL240: {struct _RegionHandle*_tmp38B=env->r;
union Cyc_CfFlowInfo_FlowInfo_union _tmp38E;struct Cyc_List_List*_tmp38F;struct
_tuple11 _tmp38D=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp38B,env,inflow,({
struct Cyc_Absyn_Exp*_tmp38C[2];_tmp38C[1]=_tmp371;_tmp38C[0]=_tmp370;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp38B,
_tag_dyneither(_tmp38C,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp38E=_tmp38D.f1;
_tmp38F=_tmp38D.f2;_tmp38E=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp371->loc,
_tmp38E);{union Cyc_CfFlowInfo_FlowInfo_union _tmp390=_tmp38E;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp391=_tmp38E;struct Cyc_Dict_Dict _tmp392;struct Cyc_List_List*_tmp393;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp394;_LL244: if((_tmp391.ReachableFL).tag != 1)goto _LL246;_tmp392=(_tmp391.ReachableFL).f1;
_tmp393=(_tmp391.ReachableFL).f2;_tmp394=(_tmp391.ReachableFL).f3;_LL245: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp392,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp38F))->tl))->hd)== (void*)0)({void*_tmp395=0;Cyc_Tcutil_terr(
_tmp371->loc,({const char*_tmp396="expression may not be initialized";
_tag_dyneither(_tmp396,sizeof(char),34);}),_tag_dyneither(_tmp395,sizeof(void*),
0));});{struct Cyc_List_List*_tmp397=Cyc_NewControlFlow_add_subscript_reln(
_tmp35A->r,_tmp393,_tmp370,_tmp371);if(_tmp393 != _tmp397)_tmp390=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp398;(_tmp398.ReachableFL).tag=1;(_tmp398.ReachableFL).f1=
_tmp392;(_tmp398.ReachableFL).f2=_tmp397;(_tmp398.ReachableFL).f3=_tmp394;
_tmp398;});goto _LL243;}_LL246:;_LL247: goto _LL243;_LL243:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp39A;union Cyc_CfFlowInfo_AbsLVal_union _tmp39B;struct _tuple7 _tmp399=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp370,_tmp38E,(void*)((struct Cyc_List_List*)_check_null(_tmp38F))->hd,
flds);_tmp39A=_tmp399.f1;_tmp39B=_tmp399.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp39C=_tmp39A;_LL249: if((_tmp39C.BottomFL).tag != 0)goto _LL24B;_LL24A: return({
struct _tuple7 _tmp39D;_tmp39D.f1=_tmp39A;_tmp39D.f2=_tmp39B;_tmp39D;});_LL24B:;
_LL24C: return({struct _tuple7 _tmp39E;_tmp39E.f1=_tmp390;_tmp39E.f2=_tmp39B;
_tmp39E;});_LL248:;}}}}_LL241:;_LL242:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F[0]=({struct Cyc_Core_Impossible_struct
_tmp3A0;_tmp3A0.tag=Cyc_Core_Impossible;_tmp3A0.f1=({const char*_tmp3A1="anal_Lexp: Subscript -- bad type";
_tag_dyneither(_tmp3A1,sizeof(char),33);});_tmp3A0;});_tmp39F;}));_LL23C:;}
_LL233: if(*((int*)_tmp361)!= 23)goto _LL235;_tmp372=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp361)->f1;_tmp373=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp361)->f2;_LL234:
if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp372->topt))->v))
return({struct _tuple7 _tmp3A2;_tmp3A2.f1=inflow;_tmp3A2.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp3A3;(_tmp3A3.UnknownL).tag=1;_tmp3A3;});
_tmp3A2;});return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp372,({struct Cyc_List_List*
_tmp3A4=_region_malloc(env->r,sizeof(*_tmp3A4));_tmp3A4->hd=(void*)Cyc_CfFlowInfo_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp372->topt))->v,_tmp373);_tmp3A4->tl=
flds;_tmp3A4;}));_LL235:;_LL236: return({struct _tuple7 _tmp3A5;_tmp3A5.f1=(union
Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp3A7;(
_tmp3A7.BottomFL).tag=0;_tmp3A7;});_tmp3A5.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp3A6;(_tmp3A6.UnknownL).tag=1;_tmp3A6;});
_tmp3A5;});_LL21E:;}}static struct _tuple7 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3A9;struct _tuple6 _tmp3A8=Cyc_CfFlowInfo_save_consume_info(env->fenv,inflow,0);
_tmp3A9=_tmp3A8.f1;{union Cyc_CfFlowInfo_FlowInfo_union _tmp3AB;union Cyc_CfFlowInfo_AbsLVal_union
_tmp3AC;struct _tuple7 _tmp3AA=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,e,0);
_tmp3AB=_tmp3AA.f1;_tmp3AC=_tmp3AA.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp3AE;
struct _tuple6 _tmp3AD=Cyc_CfFlowInfo_save_consume_info(env->fenv,inflow,0);
_tmp3AE=_tmp3AD.f1;if(_tmp3A9.may_consume != _tmp3AE.may_consume  || (_tmp3A9.consumed).t
!= (_tmp3AE.consumed).t)({void*_tmp3AF=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp3B0="oops: anal_Lexp modified consume lists";
_tag_dyneither(_tmp3B0,sizeof(char),39);}),_tag_dyneither(_tmp3AF,sizeof(void*),
0));});return({struct _tuple7 _tmp3B1;_tmp3B1.f1=_tmp3AB;_tmp3B1.f2=_tmp3AC;
_tmp3B1;});}}}static struct _tuple8 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp3B2=env->fenv;{void*_tmp3B3=(void*)e->r;struct Cyc_Absyn_Exp*_tmp3B4;struct
Cyc_Absyn_Exp*_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B6;struct Cyc_Absyn_Exp*_tmp3B7;
struct Cyc_Absyn_Exp*_tmp3B8;struct Cyc_Absyn_Exp*_tmp3B9;struct Cyc_Absyn_Exp*
_tmp3BA;struct Cyc_Absyn_Exp*_tmp3BB;struct Cyc_Absyn_Exp*_tmp3BC;void*_tmp3BD;
struct Cyc_List_List*_tmp3BE;struct Cyc_List_List _tmp3BF;struct Cyc_Absyn_Exp*
_tmp3C0;struct Cyc_List_List*_tmp3C1;void*_tmp3C2;struct Cyc_List_List*_tmp3C3;
_LL24E: if(*((int*)_tmp3B3)!= 6)goto _LL250;_tmp3B4=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp3B3)->f1;_tmp3B5=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3B3)->f2;
_tmp3B6=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3B3)->f3;_LL24F: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3C5;union Cyc_CfFlowInfo_FlowInfo_union _tmp3C6;struct _tuple8 _tmp3C4=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3B4);_tmp3C5=_tmp3C4.f1;_tmp3C6=_tmp3C4.f2;_tmp3C5=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3B4->loc,_tmp3C5);_tmp3C6=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3B4->loc,
_tmp3C6);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3C8;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3C9;struct _tuple8 _tmp3C7=Cyc_NewControlFlow_anal_test(env,_tmp3C5,_tmp3B5);
_tmp3C8=_tmp3C7.f1;_tmp3C9=_tmp3C7.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp3CB;
union Cyc_CfFlowInfo_FlowInfo_union _tmp3CC;struct _tuple8 _tmp3CA=Cyc_NewControlFlow_anal_test(
env,_tmp3C6,_tmp3B6);_tmp3CB=_tmp3CA.f1;_tmp3CC=_tmp3CA.f2;return({struct _tuple8
_tmp3CD;_tmp3CD.f1=Cyc_CfFlowInfo_join_flow(_tmp3B2,env->all_changed,_tmp3C8,
_tmp3CB,1);_tmp3CD.f2=Cyc_CfFlowInfo_join_flow(_tmp3B2,env->all_changed,_tmp3C9,
_tmp3CC,1);_tmp3CD;});}}}_LL250: if(*((int*)_tmp3B3)!= 7)goto _LL252;_tmp3B7=((
struct Cyc_Absyn_And_e_struct*)_tmp3B3)->f1;_tmp3B8=((struct Cyc_Absyn_And_e_struct*)
_tmp3B3)->f2;_LL251: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3CF;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D0;struct _tuple8 _tmp3CE=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3B7);
_tmp3CF=_tmp3CE.f1;_tmp3D0=_tmp3CE.f2;_tmp3CF=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3B7->loc,_tmp3CF);_tmp3D0=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3B7->loc,
_tmp3D0);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3D2;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D3;struct _tuple8 _tmp3D1=Cyc_NewControlFlow_anal_test(env,_tmp3CF,_tmp3B8);
_tmp3D2=_tmp3D1.f1;_tmp3D3=_tmp3D1.f2;_tmp3D2=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3B8->loc,_tmp3D2);_tmp3D3=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3B8->loc,
_tmp3D3);return({struct _tuple8 _tmp3D4;_tmp3D4.f1=_tmp3D2;_tmp3D4.f2=Cyc_CfFlowInfo_join_flow(
_tmp3B2,env->all_changed,_tmp3D0,_tmp3D3,0);_tmp3D4;});}}_LL252: if(*((int*)
_tmp3B3)!= 8)goto _LL254;_tmp3B9=((struct Cyc_Absyn_Or_e_struct*)_tmp3B3)->f1;
_tmp3BA=((struct Cyc_Absyn_Or_e_struct*)_tmp3B3)->f2;_LL253: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D6;union Cyc_CfFlowInfo_FlowInfo_union _tmp3D7;struct _tuple8 _tmp3D5=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3B9);_tmp3D6=_tmp3D5.f1;_tmp3D7=_tmp3D5.f2;_tmp3D6=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3B9->loc,_tmp3D6);_tmp3D7=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3B9->loc,
_tmp3D7);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3D9;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3DA;struct _tuple8 _tmp3D8=Cyc_NewControlFlow_anal_test(env,_tmp3D7,_tmp3BA);
_tmp3D9=_tmp3D8.f1;_tmp3DA=_tmp3D8.f2;_tmp3D9=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3BA->loc,_tmp3D9);_tmp3DA=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3BA->loc,
_tmp3DA);return({struct _tuple8 _tmp3DB;_tmp3DB.f1=Cyc_CfFlowInfo_join_flow(
_tmp3B2,env->all_changed,_tmp3D6,_tmp3D9,0);_tmp3DB.f2=_tmp3DA;_tmp3DB;});}}
_LL254: if(*((int*)_tmp3B3)!= 9)goto _LL256;_tmp3BB=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp3B3)->f1;_tmp3BC=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp3B3)->f2;_LL255: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp3DD;void*_tmp3DE;struct _tuple5 _tmp3DC=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp3BB);_tmp3DD=_tmp3DC.f1;_tmp3DE=_tmp3DC.f2;_tmp3DD=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp3BB->loc,_tmp3DD);return Cyc_NewControlFlow_anal_test(env,_tmp3DD,_tmp3BC);}
_LL256: if(*((int*)_tmp3B3)!= 3)goto _LL258;_tmp3BD=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B3)->f1;if((int)_tmp3BD != 11)goto _LL258;_tmp3BE=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B3)->f2;if(_tmp3BE == 0)goto _LL258;_tmp3BF=*_tmp3BE;_tmp3C0=(struct Cyc_Absyn_Exp*)
_tmp3BF.hd;_tmp3C1=_tmp3BF.tl;if(_tmp3C1 != 0)goto _LL258;_LL257: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3E0;union Cyc_CfFlowInfo_FlowInfo_union _tmp3E1;struct _tuple8 _tmp3DF=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3C0);_tmp3E0=_tmp3DF.f1;_tmp3E1=_tmp3DF.f2;return({struct _tuple8
_tmp3E2;_tmp3E2.f1=_tmp3E1;_tmp3E2.f2=_tmp3E0;_tmp3E2;});}_LL258: if(*((int*)
_tmp3B3)!= 3)goto _LL25A;_tmp3C2=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B3)->f1;_tmp3C3=((struct Cyc_Absyn_Primop_e_struct*)_tmp3B3)->f2;_LL259: {
void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo_union f;struct _RegionHandle*_tmp3E3=
env->r;{union Cyc_CfFlowInfo_FlowInfo_union _tmp3E5;struct Cyc_List_List*_tmp3E6;
struct _tuple11 _tmp3E4=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp3E3,env,inflow,
_tmp3C3,0);_tmp3E5=_tmp3E4.f1;_tmp3E6=_tmp3E4.f2;r1=(void*)((struct Cyc_List_List*)
_check_null(_tmp3E6))->hd;r2=(void*)((struct Cyc_List_List*)_check_null(_tmp3E6->tl))->hd;
f=_tmp3E5;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp3E7=f;struct Cyc_Dict_Dict
_tmp3E8;struct Cyc_List_List*_tmp3E9;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3EA;
_LL25D: if((_tmp3E7.BottomFL).tag != 0)goto _LL25F;_LL25E: return({struct _tuple8
_tmp3EB;_tmp3EB.f1=f;_tmp3EB.f2=f;_tmp3EB;});_LL25F: if((_tmp3E7.ReachableFL).tag
!= 1)goto _LL25C;_tmp3E8=(_tmp3E7.ReachableFL).f1;_tmp3E9=(_tmp3E7.ReachableFL).f2;
_tmp3EA=(_tmp3E7.ReachableFL).f3;_LL260: {struct Cyc_Absyn_Exp*_tmp3EC=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3C3))->hd;struct Cyc_Absyn_Exp*_tmp3ED=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3C3->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp3E8,r1)== (void*)0)({void*_tmp3EE=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmp3C3->hd)->loc,({const char*_tmp3EF="expression may not be initialized";
_tag_dyneither(_tmp3EF,sizeof(char),34);}),_tag_dyneither(_tmp3EE,sizeof(void*),
0));});if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp3E8,r2)== (void*)0)({void*
_tmp3F0=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3C3->tl))->hd)->loc,({const char*_tmp3F1="expression may not be initialized";
_tag_dyneither(_tmp3F1,sizeof(char),34);}),_tag_dyneither(_tmp3F0,sizeof(void*),
0));});if(_tmp3C2 == (void*)5  || _tmp3C2 == (void*)6){struct _tuple0 _tmp3F3=({
struct _tuple0 _tmp3F2;_tmp3F2.f1=r1;_tmp3F2.f2=r2;_tmp3F2;});void*_tmp3F4;void*
_tmp3F5;void*_tmp3F6;void*_tmp3F7;void*_tmp3F8;void*_tmp3F9;void*_tmp3FA;void*
_tmp3FB;void*_tmp3FC;void*_tmp3FD;void*_tmp3FE;void*_tmp3FF;void*_tmp400;void*
_tmp401;void*_tmp402;void*_tmp403;void*_tmp404;void*_tmp405;void*_tmp406;void*
_tmp407;_LL262: _tmp3F4=_tmp3F3.f1;if(_tmp3F4 <= (void*)3)goto _LL264;if(*((int*)
_tmp3F4)!= 0)goto _LL264;_tmp3F5=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp3F4)->f1;_tmp3F6=_tmp3F3.f2;if((int)_tmp3F6 != 0)goto _LL264;_LL263: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp409;union Cyc_CfFlowInfo_FlowInfo_union _tmp40A;struct _tuple8 _tmp408=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp3EC,_tmp3F5);_tmp409=_tmp408.f1;_tmp40A=_tmp408.f2;{void*_tmp40B=
_tmp3C2;_LL277: if((int)_tmp40B != 5)goto _LL279;_LL278: return({struct _tuple8
_tmp40C;_tmp40C.f1=_tmp40A;_tmp40C.f2=_tmp409;_tmp40C;});_LL279: if((int)_tmp40B
!= 6)goto _LL27B;_LL27A: return({struct _tuple8 _tmp40D;_tmp40D.f1=_tmp409;_tmp40D.f2=
_tmp40A;_tmp40D;});_LL27B:;_LL27C:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp40E=_cycalloc(sizeof(*_tmp40E));_tmp40E[0]=({struct Cyc_Core_Impossible_struct
_tmp40F;_tmp40F.tag=Cyc_Core_Impossible;_tmp40F.f1=({const char*_tmp410="anal_test, zero-split";
_tag_dyneither(_tmp410,sizeof(char),22);});_tmp40F;});_tmp40E;}));_LL276:;}}
_LL264: _tmp3F7=_tmp3F3.f1;if((int)_tmp3F7 != 0)goto _LL266;_tmp3F8=_tmp3F3.f2;if(
_tmp3F8 <= (void*)3)goto _LL266;if(*((int*)_tmp3F8)!= 0)goto _LL266;_tmp3F9=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3F8)->f1;_LL265: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp412;union Cyc_CfFlowInfo_FlowInfo_union _tmp413;struct _tuple8 _tmp411=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp3ED,_tmp3F9);_tmp412=_tmp411.f1;_tmp413=_tmp411.f2;{void*_tmp414=
_tmp3C2;_LL27E: if((int)_tmp414 != 5)goto _LL280;_LL27F: return({struct _tuple8
_tmp415;_tmp415.f1=_tmp413;_tmp415.f2=_tmp412;_tmp415;});_LL280: if((int)_tmp414
!= 6)goto _LL282;_LL281: return({struct _tuple8 _tmp416;_tmp416.f1=_tmp412;_tmp416.f2=
_tmp413;_tmp416;});_LL282:;_LL283:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417[0]=({struct Cyc_Core_Impossible_struct
_tmp418;_tmp418.tag=Cyc_Core_Impossible;_tmp418.f1=({const char*_tmp419="anal_test, zero-split";
_tag_dyneither(_tmp419,sizeof(char),22);});_tmp418;});_tmp417;}));_LL27D:;}}
_LL266: _tmp3FA=_tmp3F3.f1;if((int)_tmp3FA != 0)goto _LL268;_tmp3FB=_tmp3F3.f2;if((
int)_tmp3FB != 0)goto _LL268;_LL267: if(_tmp3C2 == (void*)5)return({struct _tuple8
_tmp41A;_tmp41A.f1=f;_tmp41A.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp41B;(_tmp41B.BottomFL).tag=0;_tmp41B;});_tmp41A;});else{return({struct
_tuple8 _tmp41C;_tmp41C.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp41D;(_tmp41D.BottomFL).tag=0;_tmp41D;});_tmp41C.f2=f;_tmp41C;});}_LL268:
_tmp3FC=_tmp3F3.f1;if((int)_tmp3FC != 0)goto _LL26A;_tmp3FD=_tmp3F3.f2;if((int)
_tmp3FD != 1)goto _LL26A;_LL269: goto _LL26B;_LL26A: _tmp3FE=_tmp3F3.f1;if((int)
_tmp3FE != 0)goto _LL26C;_tmp3FF=_tmp3F3.f2;if((int)_tmp3FF != 2)goto _LL26C;_LL26B:
goto _LL26D;_LL26C: _tmp400=_tmp3F3.f1;if((int)_tmp400 != 0)goto _LL26E;_tmp401=
_tmp3F3.f2;if(_tmp401 <= (void*)3)goto _LL26E;if(*((int*)_tmp401)!= 2)goto _LL26E;
_LL26D: goto _LL26F;_LL26E: _tmp402=_tmp3F3.f1;if((int)_tmp402 != 1)goto _LL270;
_tmp403=_tmp3F3.f2;if((int)_tmp403 != 0)goto _LL270;_LL26F: goto _LL271;_LL270:
_tmp404=_tmp3F3.f1;if((int)_tmp404 != 2)goto _LL272;_tmp405=_tmp3F3.f2;if((int)
_tmp405 != 0)goto _LL272;_LL271: goto _LL273;_LL272: _tmp406=_tmp3F3.f1;if(_tmp406 <= (
void*)3)goto _LL274;if(*((int*)_tmp406)!= 2)goto _LL274;_tmp407=_tmp3F3.f2;if((int)
_tmp407 != 0)goto _LL274;_LL273: if(_tmp3C2 == (void*)6)return({struct _tuple8 _tmp41E;
_tmp41E.f1=f;_tmp41E.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp41F;(_tmp41F.BottomFL).tag=0;_tmp41F;});_tmp41E;});else{return({struct
_tuple8 _tmp420;_tmp420.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp421;(_tmp421.BottomFL).tag=0;_tmp421;});_tmp420.f2=f;_tmp420;});}_LL274:;
_LL275: goto _LL261;_LL261:;}{struct _tuple0 _tmp423=({struct _tuple0 _tmp422;_tmp422.f1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3EC->topt))->v);
_tmp422.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3ED->topt))->v);
_tmp422;});void*_tmp424;void*_tmp425;void*_tmp426;void*_tmp427;void*_tmp428;void*
_tmp429;_LL285: _tmp424=_tmp423.f1;if(_tmp424 <= (void*)4)goto _LL287;if(*((int*)
_tmp424)!= 5)goto _LL287;_tmp425=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp424)->f1;
if((int)_tmp425 != 1)goto _LL287;_LL286: goto _LL288;_LL287: _tmp426=_tmp423.f2;if(
_tmp426 <= (void*)4)goto _LL289;if(*((int*)_tmp426)!= 5)goto _LL289;_tmp427=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp426)->f1;if((int)_tmp427 != 1)goto _LL289;
_LL288: goto _LL28A;_LL289: _tmp428=_tmp423.f1;if(_tmp428 <= (void*)4)goto _LL28B;if(*((
int*)_tmp428)!= 18)goto _LL28B;_LL28A: goto _LL28C;_LL28B: _tmp429=_tmp423.f2;if(
_tmp429 <= (void*)4)goto _LL28D;if(*((int*)_tmp429)!= 18)goto _LL28D;_LL28C: goto
_LL284;_LL28D:;_LL28E: return({struct _tuple8 _tmp42A;_tmp42A.f1=f;_tmp42A.f2=f;
_tmp42A;});_LL284:;}{void*_tmp42B=_tmp3C2;_LL290: if((int)_tmp42B != 5)goto _LL292;
_LL291: {union Cyc_CfFlowInfo_FlowInfo_union _tmp42C=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3EC,r1,(void*)5,r2);_tmp42C=Cyc_NewControlFlow_if_tagcmp(env,_tmp42C,
_tmp3ED,r2,(void*)5,r1);return({struct _tuple8 _tmp42D;_tmp42D.f1=_tmp42C;_tmp42D.f2=
f;_tmp42D;});}_LL292: if((int)_tmp42B != 6)goto _LL294;_LL293: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp42E=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3EC,r1,(void*)5,r2);_tmp42E=Cyc_NewControlFlow_if_tagcmp(
env,_tmp42E,_tmp3ED,r2,(void*)5,r1);return({struct _tuple8 _tmp42F;_tmp42F.f1=f;
_tmp42F.f2=_tmp42E;_tmp42F;});}_LL294: if((int)_tmp42B != 7)goto _LL296;_LL295: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp430=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3ED,r2,(void*)8,r1);union Cyc_CfFlowInfo_FlowInfo_union _tmp431=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3EC,r1,(void*)10,r2);return({struct _tuple8 _tmp432;_tmp432.f1=_tmp430;
_tmp432.f2=_tmp431;_tmp432;});}_LL296: if((int)_tmp42B != 9)goto _LL298;_LL297: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp433=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3ED,r2,(void*)10,r1);union Cyc_CfFlowInfo_FlowInfo_union _tmp434=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3EC,r1,(void*)8,r2);return({struct _tuple8 _tmp435;_tmp435.f1=_tmp433;
_tmp435.f2=_tmp434;_tmp435;});}_LL298: if((int)_tmp42B != 8)goto _LL29A;_LL299: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp436=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3EC,r1,(void*)8,r2);union Cyc_CfFlowInfo_FlowInfo_union _tmp437=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3ED,r2,(void*)10,r1);{union Cyc_CfFlowInfo_FlowInfo_union _tmp438=
_tmp436;struct Cyc_Dict_Dict _tmp439;struct Cyc_CfFlowInfo_ConsumeInfo _tmp43A;
_LL29F: if((_tmp438.BottomFL).tag != 0)goto _LL2A1;_LL2A0:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B[0]=({
struct Cyc_Core_Impossible_struct _tmp43C;_tmp43C.tag=Cyc_Core_Impossible;_tmp43C.f1=({
const char*_tmp43D="anal_test, Lt";_tag_dyneither(_tmp43D,sizeof(char),14);});
_tmp43C;});_tmp43B;}));_LL2A1: if((_tmp438.ReachableFL).tag != 1)goto _LL29E;
_tmp439=(_tmp438.ReachableFL).f1;_tmp43A=(_tmp438.ReachableFL).f3;_LL2A2: _tmp3E8=
_tmp439;_tmp3EA=_tmp43A;_LL29E:;}{void*_tmp43E=(void*)_tmp3EC->r;void*_tmp43F;
struct Cyc_Absyn_Vardecl*_tmp440;void*_tmp441;struct Cyc_Absyn_Vardecl*_tmp442;
void*_tmp443;struct Cyc_Absyn_Vardecl*_tmp444;void*_tmp445;struct Cyc_Absyn_Vardecl*
_tmp446;_LL2A4: if(*((int*)_tmp43E)!= 1)goto _LL2A6;_tmp43F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp43E)->f2;if(_tmp43F <= (void*)1)goto _LL2A6;if(*((int*)_tmp43F)!= 0)goto _LL2A6;
_tmp440=((struct Cyc_Absyn_Global_b_struct*)_tmp43F)->f1;if(!(!_tmp440->escapes))
goto _LL2A6;_LL2A5: _tmp442=_tmp440;goto _LL2A7;_LL2A6: if(*((int*)_tmp43E)!= 1)goto
_LL2A8;_tmp441=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp43E)->f2;if(_tmp441 <= (
void*)1)goto _LL2A8;if(*((int*)_tmp441)!= 3)goto _LL2A8;_tmp442=((struct Cyc_Absyn_Local_b_struct*)
_tmp441)->f1;if(!(!_tmp442->escapes))goto _LL2A8;_LL2A7: _tmp444=_tmp442;goto
_LL2A9;_LL2A8: if(*((int*)_tmp43E)!= 1)goto _LL2AA;_tmp443=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp43E)->f2;if(_tmp443 <= (void*)1)goto _LL2AA;if(*((int*)_tmp443)!= 4)goto _LL2AA;
_tmp444=((struct Cyc_Absyn_Pat_b_struct*)_tmp443)->f1;if(!(!_tmp444->escapes))
goto _LL2AA;_LL2A9: _tmp446=_tmp444;goto _LL2AB;_LL2AA: if(*((int*)_tmp43E)!= 1)goto
_LL2AC;_tmp445=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp43E)->f2;if(_tmp445 <= (
void*)1)goto _LL2AC;if(*((int*)_tmp445)!= 2)goto _LL2AC;_tmp446=((struct Cyc_Absyn_Param_b_struct*)
_tmp445)->f1;if(!(!_tmp446->escapes))goto _LL2AC;_LL2AB: {void*_tmp447=(void*)
_tmp3ED->r;void*_tmp448;struct Cyc_Absyn_Vardecl*_tmp449;void*_tmp44A;struct Cyc_Absyn_Vardecl*
_tmp44B;void*_tmp44C;struct Cyc_Absyn_Vardecl*_tmp44D;void*_tmp44E;struct Cyc_Absyn_Vardecl*
_tmp44F;union Cyc_Absyn_Cnst_union _tmp450;int _tmp451;void*_tmp452;struct Cyc_List_List*
_tmp453;struct Cyc_List_List _tmp454;struct Cyc_Absyn_Exp*_tmp455;_LL2AF: if(*((int*)
_tmp447)!= 1)goto _LL2B1;_tmp448=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp447)->f2;
if(_tmp448 <= (void*)1)goto _LL2B1;if(*((int*)_tmp448)!= 0)goto _LL2B1;_tmp449=((
struct Cyc_Absyn_Global_b_struct*)_tmp448)->f1;if(!(!_tmp449->escapes))goto _LL2B1;
_LL2B0: _tmp44B=_tmp449;goto _LL2B2;_LL2B1: if(*((int*)_tmp447)!= 1)goto _LL2B3;
_tmp44A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp447)->f2;if(_tmp44A <= (void*)
1)goto _LL2B3;if(*((int*)_tmp44A)!= 3)goto _LL2B3;_tmp44B=((struct Cyc_Absyn_Local_b_struct*)
_tmp44A)->f1;if(!(!_tmp44B->escapes))goto _LL2B3;_LL2B2: _tmp44D=_tmp44B;goto
_LL2B4;_LL2B3: if(*((int*)_tmp447)!= 1)goto _LL2B5;_tmp44C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp447)->f2;if(_tmp44C <= (void*)1)goto _LL2B5;if(*((int*)_tmp44C)!= 4)goto _LL2B5;
_tmp44D=((struct Cyc_Absyn_Pat_b_struct*)_tmp44C)->f1;if(!(!_tmp44D->escapes))
goto _LL2B5;_LL2B4: _tmp44F=_tmp44D;goto _LL2B6;_LL2B5: if(*((int*)_tmp447)!= 1)goto
_LL2B7;_tmp44E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp447)->f2;if(_tmp44E <= (
void*)1)goto _LL2B7;if(*((int*)_tmp44E)!= 2)goto _LL2B7;_tmp44F=((struct Cyc_Absyn_Param_b_struct*)
_tmp44E)->f1;if(!(!_tmp44F->escapes))goto _LL2B7;_LL2B6: {struct _RegionHandle*
_tmp456=(env->fenv)->r;struct Cyc_List_List*_tmp457=({struct Cyc_List_List*_tmp45A=
_region_malloc(_tmp456,sizeof(*_tmp45A));_tmp45A->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp45B=_region_malloc(_tmp456,sizeof(*_tmp45B));_tmp45B->vd=_tmp446;_tmp45B->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp45C;(
_tmp45C.LessVar).tag=1;(_tmp45C.LessVar).f1=_tmp44F;(_tmp45C.LessVar).f2=(void*)((
void*)_tmp44F->type);_tmp45C;});_tmp45B;});_tmp45A->tl=_tmp3E9;_tmp45A;});return({
struct _tuple8 _tmp458;_tmp458.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp459;(_tmp459.ReachableFL).tag=1;(_tmp459.ReachableFL).f1=_tmp3E8;(_tmp459.ReachableFL).f2=
_tmp457;(_tmp459.ReachableFL).f3=_tmp3EA;_tmp459;});_tmp458.f2=_tmp437;_tmp458;});}
_LL2B7: if(*((int*)_tmp447)!= 0)goto _LL2B9;_tmp450=((struct Cyc_Absyn_Const_e_struct*)
_tmp447)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp447)->f1).Int_c).tag != 2)
goto _LL2B9;_tmp451=(_tmp450.Int_c).f2;_LL2B8: {struct _RegionHandle*_tmp45D=(env->fenv)->r;
struct Cyc_List_List*_tmp45E=({struct Cyc_List_List*_tmp461=_region_malloc(_tmp45D,
sizeof(*_tmp461));_tmp461->hd=({struct Cyc_CfFlowInfo_Reln*_tmp462=_region_malloc(
_tmp45D,sizeof(*_tmp462));_tmp462->vd=_tmp446;_tmp462->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp463;(_tmp463.LessConst).tag=3;(_tmp463.LessConst).f1=(
unsigned int)_tmp451;_tmp463;});_tmp462;});_tmp461->tl=_tmp3E9;_tmp461;});return({
struct _tuple8 _tmp45F;_tmp45F.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp460;(_tmp460.ReachableFL).tag=1;(_tmp460.ReachableFL).f1=_tmp3E8;(_tmp460.ReachableFL).f2=
_tmp45E;(_tmp460.ReachableFL).f3=_tmp3EA;_tmp460;});_tmp45F.f2=_tmp437;_tmp45F;});}
_LL2B9: if(*((int*)_tmp447)!= 3)goto _LL2BB;_tmp452=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp447)->f1;_tmp453=((struct Cyc_Absyn_Primop_e_struct*)_tmp447)->f2;if(_tmp453
== 0)goto _LL2BB;_tmp454=*_tmp453;_tmp455=(struct Cyc_Absyn_Exp*)_tmp454.hd;_LL2BA: {
void*_tmp464=(void*)_tmp455->r;void*_tmp465;struct Cyc_Absyn_Vardecl*_tmp466;void*
_tmp467;struct Cyc_Absyn_Vardecl*_tmp468;void*_tmp469;struct Cyc_Absyn_Vardecl*
_tmp46A;void*_tmp46B;struct Cyc_Absyn_Vardecl*_tmp46C;_LL2BE: if(*((int*)_tmp464)
!= 1)goto _LL2C0;_tmp465=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp464)->f2;if(
_tmp465 <= (void*)1)goto _LL2C0;if(*((int*)_tmp465)!= 0)goto _LL2C0;_tmp466=((
struct Cyc_Absyn_Global_b_struct*)_tmp465)->f1;if(!(!_tmp466->escapes))goto _LL2C0;
_LL2BF: _tmp468=_tmp466;goto _LL2C1;_LL2C0: if(*((int*)_tmp464)!= 1)goto _LL2C2;
_tmp467=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp464)->f2;if(_tmp467 <= (void*)
1)goto _LL2C2;if(*((int*)_tmp467)!= 3)goto _LL2C2;_tmp468=((struct Cyc_Absyn_Local_b_struct*)
_tmp467)->f1;if(!(!_tmp468->escapes))goto _LL2C2;_LL2C1: _tmp46A=_tmp468;goto
_LL2C3;_LL2C2: if(*((int*)_tmp464)!= 1)goto _LL2C4;_tmp469=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp464)->f2;if(_tmp469 <= (void*)1)goto _LL2C4;if(*((int*)_tmp469)!= 4)goto _LL2C4;
_tmp46A=((struct Cyc_Absyn_Pat_b_struct*)_tmp469)->f1;if(!(!_tmp46A->escapes))
goto _LL2C4;_LL2C3: _tmp46C=_tmp46A;goto _LL2C5;_LL2C4: if(*((int*)_tmp464)!= 1)goto
_LL2C6;_tmp46B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp464)->f2;if(_tmp46B <= (
void*)1)goto _LL2C6;if(*((int*)_tmp46B)!= 2)goto _LL2C6;_tmp46C=((struct Cyc_Absyn_Param_b_struct*)
_tmp46B)->f1;if(!(!_tmp46C->escapes))goto _LL2C6;_LL2C5: {struct _RegionHandle*
_tmp46D=(env->fenv)->r;struct Cyc_List_List*_tmp46E=({struct Cyc_List_List*_tmp471=
_region_malloc(_tmp46D,sizeof(*_tmp471));_tmp471->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp472=_region_malloc(_tmp46D,sizeof(*_tmp472));_tmp472->vd=_tmp446;_tmp472->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp473;(
_tmp473.LessNumelts).tag=2;(_tmp473.LessNumelts).f1=_tmp46C;_tmp473;});_tmp472;});
_tmp471->tl=_tmp3E9;_tmp471;});return({struct _tuple8 _tmp46F;_tmp46F.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp470;(_tmp470.ReachableFL).tag=1;(_tmp470.ReachableFL).f1=
_tmp3E8;(_tmp470.ReachableFL).f2=_tmp46E;(_tmp470.ReachableFL).f3=_tmp3EA;
_tmp470;});_tmp46F.f2=_tmp437;_tmp46F;});}_LL2C6:;_LL2C7: return({struct _tuple8
_tmp474;_tmp474.f1=_tmp436;_tmp474.f2=_tmp437;_tmp474;});_LL2BD:;}_LL2BB:;_LL2BC:
return({struct _tuple8 _tmp475;_tmp475.f1=_tmp436;_tmp475.f2=_tmp437;_tmp475;});
_LL2AE:;}_LL2AC:;_LL2AD: return({struct _tuple8 _tmp476;_tmp476.f1=_tmp436;_tmp476.f2=
_tmp437;_tmp476;});_LL2A3:;}}_LL29A: if((int)_tmp42B != 10)goto _LL29C;_LL29B: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp477=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3EC,r1,(void*)10,r2);union Cyc_CfFlowInfo_FlowInfo_union _tmp478=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3ED,r1,(void*)8,r1);{union Cyc_CfFlowInfo_FlowInfo_union _tmp479=_tmp477;
struct Cyc_Dict_Dict _tmp47A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp47B;_LL2C9: if((
_tmp479.BottomFL).tag != 0)goto _LL2CB;_LL2CA:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp47C=_cycalloc(sizeof(*_tmp47C));_tmp47C[0]=({struct Cyc_Core_Impossible_struct
_tmp47D;_tmp47D.tag=Cyc_Core_Impossible;_tmp47D.f1=({const char*_tmp47E="anal_test, Lte";
_tag_dyneither(_tmp47E,sizeof(char),15);});_tmp47D;});_tmp47C;}));_LL2CB: if((
_tmp479.ReachableFL).tag != 1)goto _LL2C8;_tmp47A=(_tmp479.ReachableFL).f1;_tmp47B=(
_tmp479.ReachableFL).f3;_LL2CC: _tmp3E8=_tmp47A;_tmp3EA=_tmp47B;_LL2C8:;}{void*
_tmp47F=(void*)_tmp3EC->r;void*_tmp480;struct Cyc_Absyn_Vardecl*_tmp481;void*
_tmp482;struct Cyc_Absyn_Vardecl*_tmp483;void*_tmp484;struct Cyc_Absyn_Vardecl*
_tmp485;void*_tmp486;struct Cyc_Absyn_Vardecl*_tmp487;_LL2CE: if(*((int*)_tmp47F)
!= 1)goto _LL2D0;_tmp480=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp47F)->f2;if(
_tmp480 <= (void*)1)goto _LL2D0;if(*((int*)_tmp480)!= 0)goto _LL2D0;_tmp481=((
struct Cyc_Absyn_Global_b_struct*)_tmp480)->f1;if(!(!_tmp481->escapes))goto _LL2D0;
_LL2CF: _tmp483=_tmp481;goto _LL2D1;_LL2D0: if(*((int*)_tmp47F)!= 1)goto _LL2D2;
_tmp482=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp47F)->f2;if(_tmp482 <= (void*)
1)goto _LL2D2;if(*((int*)_tmp482)!= 3)goto _LL2D2;_tmp483=((struct Cyc_Absyn_Local_b_struct*)
_tmp482)->f1;if(!(!_tmp483->escapes))goto _LL2D2;_LL2D1: _tmp485=_tmp483;goto
_LL2D3;_LL2D2: if(*((int*)_tmp47F)!= 1)goto _LL2D4;_tmp484=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp47F)->f2;if(_tmp484 <= (void*)1)goto _LL2D4;if(*((int*)_tmp484)!= 4)goto _LL2D4;
_tmp485=((struct Cyc_Absyn_Pat_b_struct*)_tmp484)->f1;if(!(!_tmp485->escapes))
goto _LL2D4;_LL2D3: _tmp487=_tmp485;goto _LL2D5;_LL2D4: if(*((int*)_tmp47F)!= 1)goto
_LL2D6;_tmp486=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp47F)->f2;if(_tmp486 <= (
void*)1)goto _LL2D6;if(*((int*)_tmp486)!= 2)goto _LL2D6;_tmp487=((struct Cyc_Absyn_Param_b_struct*)
_tmp486)->f1;if(!(!_tmp487->escapes))goto _LL2D6;_LL2D5: {void*_tmp488=(void*)
_tmp3ED->r;void*_tmp489;struct Cyc_List_List*_tmp48A;struct Cyc_List_List _tmp48B;
struct Cyc_Absyn_Exp*_tmp48C;_LL2D9: if(*((int*)_tmp488)!= 3)goto _LL2DB;_tmp489=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp488)->f1;_tmp48A=((struct Cyc_Absyn_Primop_e_struct*)
_tmp488)->f2;if(_tmp48A == 0)goto _LL2DB;_tmp48B=*_tmp48A;_tmp48C=(struct Cyc_Absyn_Exp*)
_tmp48B.hd;_LL2DA: {void*_tmp48D=(void*)_tmp48C->r;void*_tmp48E;struct Cyc_Absyn_Vardecl*
_tmp48F;void*_tmp490;struct Cyc_Absyn_Vardecl*_tmp491;void*_tmp492;struct Cyc_Absyn_Vardecl*
_tmp493;void*_tmp494;struct Cyc_Absyn_Vardecl*_tmp495;_LL2DE: if(*((int*)_tmp48D)
!= 1)goto _LL2E0;_tmp48E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp48D)->f2;if(
_tmp48E <= (void*)1)goto _LL2E0;if(*((int*)_tmp48E)!= 0)goto _LL2E0;_tmp48F=((
struct Cyc_Absyn_Global_b_struct*)_tmp48E)->f1;if(!(!_tmp48F->escapes))goto _LL2E0;
_LL2DF: _tmp491=_tmp48F;goto _LL2E1;_LL2E0: if(*((int*)_tmp48D)!= 1)goto _LL2E2;
_tmp490=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp48D)->f2;if(_tmp490 <= (void*)
1)goto _LL2E2;if(*((int*)_tmp490)!= 3)goto _LL2E2;_tmp491=((struct Cyc_Absyn_Local_b_struct*)
_tmp490)->f1;if(!(!_tmp491->escapes))goto _LL2E2;_LL2E1: _tmp493=_tmp491;goto
_LL2E3;_LL2E2: if(*((int*)_tmp48D)!= 1)goto _LL2E4;_tmp492=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp48D)->f2;if(_tmp492 <= (void*)1)goto _LL2E4;if(*((int*)_tmp492)!= 4)goto _LL2E4;
_tmp493=((struct Cyc_Absyn_Pat_b_struct*)_tmp492)->f1;if(!(!_tmp493->escapes))
goto _LL2E4;_LL2E3: _tmp495=_tmp493;goto _LL2E5;_LL2E4: if(*((int*)_tmp48D)!= 1)goto
_LL2E6;_tmp494=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp48D)->f2;if(_tmp494 <= (
void*)1)goto _LL2E6;if(*((int*)_tmp494)!= 2)goto _LL2E6;_tmp495=((struct Cyc_Absyn_Param_b_struct*)
_tmp494)->f1;if(!(!_tmp495->escapes))goto _LL2E6;_LL2E5: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp496=env->fenv;struct Cyc_List_List*_tmp497=({struct Cyc_List_List*_tmp49A=
_region_malloc(_tmp496->r,sizeof(*_tmp49A));_tmp49A->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp49B=_region_malloc(_tmp496->r,sizeof(*_tmp49B));_tmp49B->vd=_tmp487;_tmp49B->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp49C;(
_tmp49C.LessEqNumelts).tag=4;(_tmp49C.LessEqNumelts).f1=_tmp495;_tmp49C;});
_tmp49B;});_tmp49A->tl=_tmp3E9;_tmp49A;});return({struct _tuple8 _tmp498;_tmp498.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp499;(
_tmp499.ReachableFL).tag=1;(_tmp499.ReachableFL).f1=_tmp3E8;(_tmp499.ReachableFL).f2=
_tmp497;(_tmp499.ReachableFL).f3=_tmp3EA;_tmp499;});_tmp498.f2=_tmp478;_tmp498;});}
_LL2E6:;_LL2E7: return({struct _tuple8 _tmp49D;_tmp49D.f1=_tmp477;_tmp49D.f2=
_tmp478;_tmp49D;});_LL2DD:;}_LL2DB:;_LL2DC: return({struct _tuple8 _tmp49E;_tmp49E.f1=
_tmp477;_tmp49E.f2=_tmp478;_tmp49E;});_LL2D8:;}_LL2D6:;_LL2D7: return({struct
_tuple8 _tmp49F;_tmp49F.f1=_tmp477;_tmp49F.f2=_tmp478;_tmp49F;});_LL2CD:;}}_LL29C:;
_LL29D: return({struct _tuple8 _tmp4A0;_tmp4A0.f1=f;_tmp4A0.f2=f;_tmp4A0;});_LL28F:;}}
_LL25C:;}}_LL25A:;_LL25B: goto _LL24D;_LL24D:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp4A2;void*_tmp4A3;struct _tuple5 _tmp4A1=Cyc_NewControlFlow_anal_Rexp(env,
inflow,e);_tmp4A2=_tmp4A1.f1;_tmp4A3=_tmp4A1.f2;_tmp4A2=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp4A2);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4A4=_tmp4A2;struct Cyc_Dict_Dict
_tmp4A5;_LL2E9: if((_tmp4A4.BottomFL).tag != 0)goto _LL2EB;_LL2EA: return({struct
_tuple8 _tmp4A6;_tmp4A6.f1=_tmp4A2;_tmp4A6.f2=_tmp4A2;_tmp4A6;});_LL2EB: if((
_tmp4A4.ReachableFL).tag != 1)goto _LL2E8;_tmp4A5=(_tmp4A4.ReachableFL).f1;_LL2EC: {
void*_tmp4A7=_tmp4A3;void*_tmp4A8;void*_tmp4A9;void*_tmp4AA;_LL2EE: if((int)
_tmp4A7 != 0)goto _LL2F0;_LL2EF: return({struct _tuple8 _tmp4AB;_tmp4AB.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4AC;(_tmp4AC.BottomFL).tag=0;_tmp4AC;});
_tmp4AB.f2=_tmp4A2;_tmp4AB;});_LL2F0: if((int)_tmp4A7 != 2)goto _LL2F2;_LL2F1: goto
_LL2F3;_LL2F2: if((int)_tmp4A7 != 1)goto _LL2F4;_LL2F3: goto _LL2F5;_LL2F4: if(_tmp4A7
<= (void*)3)goto _LL2F6;if(*((int*)_tmp4A7)!= 2)goto _LL2F6;_LL2F5: return({struct
_tuple8 _tmp4AD;_tmp4AD.f1=_tmp4A2;_tmp4AD.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4AE;(_tmp4AE.BottomFL).tag=0;_tmp4AE;});
_tmp4AD;});_LL2F6: if(_tmp4A7 <= (void*)3)goto _LL2F8;if(*((int*)_tmp4A7)!= 0)goto
_LL2F8;_tmp4A8=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp4A7)->f1;if((
int)_tmp4A8 != 0)goto _LL2F8;_LL2F7: goto _LL2F9;_LL2F8: if(_tmp4A7 <= (void*)3)goto
_LL2FA;if(*((int*)_tmp4A7)!= 1)goto _LL2FA;_tmp4A9=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp4A7)->f1;if((int)_tmp4A9 != 0)goto _LL2FA;_LL2F9:({void*_tmp4AF=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp4B0="expression may not be initialized";_tag_dyneither(
_tmp4B0,sizeof(char),34);}),_tag_dyneither(_tmp4AF,sizeof(void*),0));});return({
struct _tuple8 _tmp4B1;_tmp4B1.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4B3;(_tmp4B3.BottomFL).tag=0;_tmp4B3;});_tmp4B1.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4B2;(_tmp4B2.BottomFL).tag=0;_tmp4B2;});
_tmp4B1;});_LL2FA: if(_tmp4A7 <= (void*)3)goto _LL2FC;if(*((int*)_tmp4A7)!= 0)goto
_LL2FC;_tmp4AA=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp4A7)->f1;
_LL2FB: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4A2,e,_tmp4AA);_LL2FC:
if(_tmp4A7 <= (void*)3)goto _LL2FE;if(*((int*)_tmp4A7)!= 1)goto _LL2FE;_LL2FD:
return({struct _tuple8 _tmp4B4;_tmp4B4.f1=_tmp4A2;_tmp4B4.f2=_tmp4A2;_tmp4B4;});
_LL2FE: if(_tmp4A7 <= (void*)3)goto _LL300;if(*((int*)_tmp4A7)!= 3)goto _LL300;
_LL2FF: return({struct _tuple8 _tmp4B5;_tmp4B5.f1=_tmp4A2;_tmp4B5.f2=_tmp4A2;
_tmp4B5;});_LL300: if(_tmp4A7 <= (void*)3)goto _LL2ED;if(*((int*)_tmp4A7)!= 4)goto
_LL2ED;_LL301:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp4B6=
_cycalloc(sizeof(*_tmp4B6));_tmp4B6[0]=({struct Cyc_Core_Impossible_struct _tmp4B7;
_tmp4B7.tag=Cyc_Core_Impossible;_tmp4B7.f1=({const char*_tmp4B8="anal_test";
_tag_dyneither(_tmp4B8,sizeof(char),10);});_tmp4B7;});_tmp4B6;}));_LL2ED:;}
_LL2E8:;}}}static void Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*
loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union flow){
union Cyc_CfFlowInfo_FlowInfo_union _tmp4B9=flow;struct Cyc_Dict_Dict _tmp4BA;_LL303:
if((_tmp4B9.BottomFL).tag != 0)goto _LL305;_LL304: return;_LL305: if((_tmp4B9.ReachableFL).tag
!= 1)goto _LL302;_tmp4BA=(_tmp4B9.ReachableFL).f1;_LL306:{struct Cyc_List_List*
_tmp4BB=env->param_roots;for(0;_tmp4BB != 0;_tmp4BB=_tmp4BB->tl){if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp4BA,Cyc_CfFlowInfo_lookup_place(_tmp4BA,(struct Cyc_CfFlowInfo_Place*)
_tmp4BB->hd))!= (void*)2)({void*_tmp4BC=0;Cyc_Tcutil_terr(loc,({const char*
_tmp4BD="function may not initialize all the parameters with attribute 'initializes'";
_tag_dyneither(_tmp4BD,sizeof(char),76);}),_tag_dyneither(_tmp4BC,sizeof(void*),
0));});}}return;_LL302:;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp4BE=env->fenv;for(0;
scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp4C0;struct Cyc_Core_Opt*
_tmp4C1;struct Cyc_Absyn_Exp*_tmp4C2;struct Cyc_Absyn_Stmt*_tmp4C3;struct Cyc_Position_Segment*
_tmp4C4;struct Cyc_Absyn_Switch_clause*_tmp4BF=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp4C0=*_tmp4BF;_tmp4C1=_tmp4C0.pat_vars;_tmp4C2=_tmp4C0.where_clause;
_tmp4C3=_tmp4C0.body;_tmp4C4=_tmp4C0.loc;{union Cyc_CfFlowInfo_FlowInfo_union
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp4BE,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp4C1))->v,(void*)_tmp4BE->unknown_all,_tmp4C4);
union Cyc_CfFlowInfo_FlowInfo_union clause_outflow;if(_tmp4C2 != 0){struct Cyc_Absyn_Exp*
wexp=(struct Cyc_Absyn_Exp*)_tmp4C2;union Cyc_CfFlowInfo_FlowInfo_union _tmp4C6;
union Cyc_CfFlowInfo_FlowInfo_union _tmp4C7;struct _tuple8 _tmp4C5=Cyc_NewControlFlow_anal_test(
env,clause_inflow,wexp);_tmp4C6=_tmp4C5.f1;_tmp4C7=_tmp4C5.f2;_tmp4C6=Cyc_CfFlowInfo_readthrough_unique_rvals(
wexp->loc,_tmp4C6);_tmp4C7=Cyc_CfFlowInfo_readthrough_unique_rvals(wexp->loc,
_tmp4C7);inflow=_tmp4C7;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp4C6,
_tmp4C3);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,
_tmp4C3);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp4C8=clause_outflow;_LL308: if((
_tmp4C8.BottomFL).tag != 0)goto _LL30A;_LL309: goto _LL307;_LL30A:;_LL30B: if(scs->tl
== 0)return clause_outflow;else{if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(
scs->tl))->hd)->pat_vars))->v != 0)({void*_tmp4C9=0;Cyc_Tcutil_terr(_tmp4C3->loc,({
const char*_tmp4CA="switch clause may implicitly fallthru";_tag_dyneither(_tmp4CA,
sizeof(char),38);}),_tag_dyneither(_tmp4C9,sizeof(void*),0));});else{({void*
_tmp4CB=0;Cyc_Tcutil_warn(_tmp4C3->loc,({const char*_tmp4CC="switch clause may implicitly fallthru";
_tag_dyneither(_tmp4CC,sizeof(char),38);}),_tag_dyneither(_tmp4CB,sizeof(void*),
0));});}Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}goto
_LL307;_LL307:;}}}return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4CD;(_tmp4CD.BottomFL).tag=0;_tmp4CD;});}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union outflow;struct
Cyc_NewControlFlow_CFStmtAnnot*_tmp4CF;union Cyc_CfFlowInfo_FlowInfo_union*
_tmp4D0;struct _tuple9 _tmp4CE=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);
_tmp4CF=_tmp4CE.f1;_tmp4D0=_tmp4CE.f2;inflow=*_tmp4D0;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp4D1=env->fenv;{void*_tmp4D2=(void*)s->r;struct Cyc_Absyn_Exp*_tmp4D3;struct
Cyc_Absyn_Exp*_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D5;struct Cyc_Absyn_Stmt*_tmp4D6;
struct Cyc_Absyn_Stmt*_tmp4D7;struct Cyc_Absyn_Exp*_tmp4D8;struct Cyc_Absyn_Stmt*
_tmp4D9;struct Cyc_Absyn_Stmt*_tmp4DA;struct _tuple3 _tmp4DB;struct Cyc_Absyn_Exp*
_tmp4DC;struct Cyc_Absyn_Stmt*_tmp4DD;struct Cyc_Absyn_Stmt*_tmp4DE;struct Cyc_Absyn_Stmt*
_tmp4DF;struct _tuple3 _tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;struct Cyc_Absyn_Stmt*
_tmp4E2;struct Cyc_Absyn_Exp*_tmp4E3;struct _tuple3 _tmp4E4;struct Cyc_Absyn_Exp*
_tmp4E5;struct Cyc_Absyn_Stmt*_tmp4E6;struct _tuple3 _tmp4E7;struct Cyc_Absyn_Exp*
_tmp4E8;struct Cyc_Absyn_Stmt*_tmp4E9;struct Cyc_Absyn_Stmt*_tmp4EA;struct Cyc_Absyn_Stmt*
_tmp4EB;struct Cyc_List_List*_tmp4EC;struct Cyc_Absyn_Switch_clause**_tmp4ED;
struct Cyc_Absyn_Switch_clause*_tmp4EE;struct Cyc_Absyn_Stmt*_tmp4EF;struct Cyc_Absyn_Stmt*
_tmp4F0;struct Cyc_Absyn_Stmt*_tmp4F1;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_List_List*
_tmp4F3;struct Cyc_Absyn_Stmt*_tmp4F4;struct Cyc_List_List*_tmp4F5;struct Cyc_Absyn_Decl*
_tmp4F6;struct Cyc_Absyn_Decl _tmp4F7;void*_tmp4F8;struct Cyc_Absyn_Exp*_tmp4F9;
struct Cyc_Absyn_Tvar*_tmp4FA;struct Cyc_Absyn_Vardecl*_tmp4FB;struct Cyc_Absyn_Stmt*
_tmp4FC;struct Cyc_Absyn_Decl*_tmp4FD;struct Cyc_Absyn_Stmt*_tmp4FE;struct Cyc_Absyn_Stmt*
_tmp4FF;struct Cyc_Absyn_Exp*_tmp500;_LL30D: if((int)_tmp4D2 != 0)goto _LL30F;_LL30E:
return inflow;_LL30F: if(_tmp4D2 <= (void*)1)goto _LL335;if(*((int*)_tmp4D2)!= 2)
goto _LL311;_tmp4D3=((struct Cyc_Absyn_Return_s_struct*)_tmp4D2)->f1;if(_tmp4D3 != 
0)goto _LL311;_LL310: if(env->noreturn)({void*_tmp501=0;Cyc_Tcutil_terr(s->loc,({
const char*_tmp502="`noreturn' function might return";_tag_dyneither(_tmp502,
sizeof(char),33);}),_tag_dyneither(_tmp501,sizeof(void*),0));});Cyc_NewControlFlow_check_init_params(
s->loc,env,inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp503;(_tmp503.BottomFL).tag=0;_tmp503;});_LL311: if(*((int*)_tmp4D2)!= 2)goto
_LL313;_tmp4D4=((struct Cyc_Absyn_Return_s_struct*)_tmp4D2)->f1;_LL312: if(env->noreturn)({
void*_tmp504=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp505="`noreturn' function might return";
_tag_dyneither(_tmp505,sizeof(char),33);}),_tag_dyneither(_tmp504,sizeof(void*),
0));});{union Cyc_CfFlowInfo_FlowInfo_union _tmp507;void*_tmp508;struct _tuple5
_tmp506=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
_check_null(_tmp4D4));_tmp507=_tmp506.f1;_tmp508=_tmp506.f2;_tmp507=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4D4->loc,_tmp507);_tmp507=Cyc_NewControlFlow_use_Rval(env,_tmp4D4->loc,
_tmp507,_tmp508);Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp507);return(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp509;(
_tmp509.BottomFL).tag=0;_tmp509;});}_LL313: if(*((int*)_tmp4D2)!= 0)goto _LL315;
_tmp4D5=((struct Cyc_Absyn_Exp_s_struct*)_tmp4D2)->f1;_LL314: outflow=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4D5)).f1;goto _LL30C;_LL315: if(*((int*)_tmp4D2)!= 1)goto _LL317;
_tmp4D6=((struct Cyc_Absyn_Seq_s_struct*)_tmp4D2)->f1;_tmp4D7=((struct Cyc_Absyn_Seq_s_struct*)
_tmp4D2)->f2;_LL316: outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4D6),_tmp4D7);goto _LL30C;_LL317: if(*((int*)_tmp4D2)!= 3)goto
_LL319;_tmp4D8=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4D2)->f1;_tmp4D9=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4D2)->f2;_tmp4DA=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp4D2)->f3;_LL318: {union Cyc_CfFlowInfo_FlowInfo_union _tmp50B;union Cyc_CfFlowInfo_FlowInfo_union
_tmp50C;struct _tuple8 _tmp50A=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4D8);
_tmp50B=_tmp50A.f1;_tmp50C=_tmp50A.f2;_tmp50B=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4D8->loc,_tmp50B);_tmp50C=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4D8->loc,
_tmp50C);outflow=Cyc_CfFlowInfo_join_flow(_tmp4D1,env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp50B,_tmp4D9),Cyc_NewControlFlow_anal_stmt(env,_tmp50C,_tmp4DA),1);goto
_LL30C;}_LL319: if(*((int*)_tmp4D2)!= 4)goto _LL31B;_tmp4DB=((struct Cyc_Absyn_While_s_struct*)
_tmp4D2)->f1;_tmp4DC=_tmp4DB.f1;_tmp4DD=_tmp4DB.f2;_tmp4DE=((struct Cyc_Absyn_While_s_struct*)
_tmp4D2)->f2;_LL31A: {union Cyc_CfFlowInfo_FlowInfo_union*_tmp50E;struct _tuple9
_tmp50D=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp4DD);_tmp50E=_tmp50D.f2;{
union Cyc_CfFlowInfo_FlowInfo_union _tmp50F=*_tmp50E;union Cyc_CfFlowInfo_FlowInfo_union
_tmp511;union Cyc_CfFlowInfo_FlowInfo_union _tmp512;struct _tuple8 _tmp510=Cyc_NewControlFlow_anal_test(
env,_tmp50F,_tmp4DC);_tmp511=_tmp510.f1;_tmp512=_tmp510.f2;_tmp511=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4DC->loc,_tmp511);_tmp512=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4DC->loc,
_tmp512);{union Cyc_CfFlowInfo_FlowInfo_union _tmp513=Cyc_NewControlFlow_anal_stmt(
env,_tmp511,_tmp4DE);Cyc_NewControlFlow_update_flow(env,_tmp4DD,_tmp513);outflow=
_tmp512;goto _LL30C;}}}_LL31B: if(*((int*)_tmp4D2)!= 13)goto _LL31D;_tmp4DF=((
struct Cyc_Absyn_Do_s_struct*)_tmp4D2)->f1;_tmp4E0=((struct Cyc_Absyn_Do_s_struct*)
_tmp4D2)->f2;_tmp4E1=_tmp4E0.f1;_tmp4E2=_tmp4E0.f2;_LL31C: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp514=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4DF);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp516;struct _tuple9 _tmp515=Cyc_NewControlFlow_pre_stmt_check(env,_tmp514,
_tmp4E2);_tmp516=_tmp515.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp517=*_tmp516;
union Cyc_CfFlowInfo_FlowInfo_union _tmp519;union Cyc_CfFlowInfo_FlowInfo_union
_tmp51A;struct _tuple8 _tmp518=Cyc_NewControlFlow_anal_test(env,_tmp517,_tmp4E1);
_tmp519=_tmp518.f1;_tmp51A=_tmp518.f2;Cyc_NewControlFlow_update_flow(env,_tmp4DF,
_tmp519);outflow=_tmp51A;goto _LL30C;}}_LL31D: if(*((int*)_tmp4D2)!= 8)goto _LL31F;
_tmp4E3=((struct Cyc_Absyn_For_s_struct*)_tmp4D2)->f1;_tmp4E4=((struct Cyc_Absyn_For_s_struct*)
_tmp4D2)->f2;_tmp4E5=_tmp4E4.f1;_tmp4E6=_tmp4E4.f2;_tmp4E7=((struct Cyc_Absyn_For_s_struct*)
_tmp4D2)->f3;_tmp4E8=_tmp4E7.f1;_tmp4E9=_tmp4E7.f2;_tmp4EA=((struct Cyc_Absyn_For_s_struct*)
_tmp4D2)->f4;_LL31E: {union Cyc_CfFlowInfo_FlowInfo_union _tmp51B=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4E3)).f1;_tmp51B=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4E3->loc,
_tmp51B);{union Cyc_CfFlowInfo_FlowInfo_union*_tmp51D;struct _tuple9 _tmp51C=Cyc_NewControlFlow_pre_stmt_check(
env,_tmp51B,_tmp4E6);_tmp51D=_tmp51C.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp51E=*_tmp51D;union Cyc_CfFlowInfo_FlowInfo_union _tmp520;union Cyc_CfFlowInfo_FlowInfo_union
_tmp521;struct _tuple8 _tmp51F=Cyc_NewControlFlow_anal_test(env,_tmp51E,_tmp4E5);
_tmp520=_tmp51F.f1;_tmp521=_tmp51F.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp522=
Cyc_NewControlFlow_anal_stmt(env,_tmp520,_tmp4EA);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp524;struct _tuple9 _tmp523=Cyc_NewControlFlow_pre_stmt_check(env,_tmp522,
_tmp4E9);_tmp524=_tmp523.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp525=*_tmp524;
union Cyc_CfFlowInfo_FlowInfo_union _tmp526=(Cyc_NewControlFlow_anal_Rexp(env,
_tmp525,_tmp4E8)).f1;_tmp526=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4E8->loc,
_tmp526);Cyc_NewControlFlow_update_flow(env,_tmp4E6,_tmp526);outflow=_tmp521;
goto _LL30C;}}}}}_LL31F: if(*((int*)_tmp4D2)!= 5)goto _LL321;_tmp4EB=((struct Cyc_Absyn_Break_s_struct*)
_tmp4D2)->f1;if(_tmp4EB != 0)goto _LL321;_LL320: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp527;(_tmp527.BottomFL).tag=0;_tmp527;});
_LL321: if(*((int*)_tmp4D2)!= 10)goto _LL323;_tmp4EC=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp4D2)->f1;_tmp4ED=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp4D2)->f2;if(
_tmp4ED == 0)goto _LL323;_tmp4EE=*_tmp4ED;_LL322: {struct _RegionHandle*_tmp528=env->r;
union Cyc_CfFlowInfo_FlowInfo_union _tmp52A;struct Cyc_List_List*_tmp52B;struct
_tuple11 _tmp529=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp528,env,inflow,
_tmp4EC,0);_tmp52A=_tmp529.f1;_tmp52B=_tmp529.f2;for(0;_tmp52B != 0;(_tmp52B=
_tmp52B->tl,_tmp4EC=_tmp4EC->tl)){_tmp52A=Cyc_NewControlFlow_use_Rval(env,((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4EC))->hd)->loc,
_tmp52A,(void*)_tmp52B->hd);}_tmp52A=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,
_tmp52A);_tmp52A=Cyc_NewControlFlow_add_vars(_tmp4D1,_tmp52A,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp4EE->pat_vars))->v,(void*)_tmp4D1->unknown_all,
s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp4EE->body,
_tmp52A);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp52C;(_tmp52C.BottomFL).tag=0;_tmp52C;});}_LL323: if(*((int*)_tmp4D2)!= 5)goto
_LL325;_tmp4EF=((struct Cyc_Absyn_Break_s_struct*)_tmp4D2)->f1;_LL324: _tmp4F0=
_tmp4EF;goto _LL326;_LL325: if(*((int*)_tmp4D2)!= 6)goto _LL327;_tmp4F0=((struct Cyc_Absyn_Continue_s_struct*)
_tmp4D2)->f1;_LL326: _tmp4F1=_tmp4F0;goto _LL328;_LL327: if(*((int*)_tmp4D2)!= 7)
goto _LL329;_tmp4F1=((struct Cyc_Absyn_Goto_s_struct*)_tmp4D2)->f2;_LL328: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp52D=_tmp4CF->encloser;struct Cyc_Absyn_Stmt*
_tmp52E=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp4F1)))->encloser;while(_tmp52E != _tmp52D){struct Cyc_Absyn_Stmt*_tmp52F=(Cyc_NewControlFlow_get_stmt_annot(
_tmp52D))->encloser;if(_tmp52F == _tmp52D){({void*_tmp530=0;Cyc_Tcutil_terr(s->loc,({
const char*_tmp531="goto enters local scope or exception handler";_tag_dyneither(
_tmp531,sizeof(char),45);}),_tag_dyneither(_tmp530,sizeof(void*),0));});break;}
_tmp52D=_tmp52F;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_check_null(_tmp4F1),inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({union
Cyc_CfFlowInfo_FlowInfo_union _tmp532;(_tmp532.BottomFL).tag=0;_tmp532;});_LL329:
if(*((int*)_tmp4D2)!= 9)goto _LL32B;_tmp4F2=((struct Cyc_Absyn_Switch_s_struct*)
_tmp4D2)->f1;_tmp4F3=((struct Cyc_Absyn_Switch_s_struct*)_tmp4D2)->f2;_LL32A: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp534;void*_tmp535;struct _tuple5 _tmp533=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4F2);_tmp534=_tmp533.f1;_tmp535=_tmp533.f2;_tmp534=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4F2->loc,_tmp534);_tmp534=Cyc_NewControlFlow_use_Rval(env,_tmp4F2->loc,
_tmp534,_tmp535);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp534,_tmp4F3);goto
_LL30C;}_LL32B: if(*((int*)_tmp4D2)!= 14)goto _LL32D;_tmp4F4=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp4D2)->f1;_tmp4F5=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4D2)->f2;_LL32C: {
int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo_union old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo_union s1_outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4F4);union Cyc_CfFlowInfo_FlowInfo_union
scs_inflow=env->tryflow;env->in_try=old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(
env,scs_inflow);{union Cyc_CfFlowInfo_FlowInfo_union scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp4F5);{union Cyc_CfFlowInfo_FlowInfo_union _tmp536=scs_outflow;
_LL338: if((_tmp536.BottomFL).tag != 0)goto _LL33A;_LL339: goto _LL337;_LL33A:;_LL33B:({
void*_tmp537=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp538="last catch clause may implicitly fallthru";
_tag_dyneither(_tmp538,sizeof(char),42);}),_tag_dyneither(_tmp537,sizeof(void*),
0));});_LL337:;}outflow=s1_outflow;goto _LL30C;}}}_LL32D: if(*((int*)_tmp4D2)!= 11)
goto _LL32F;_tmp4F6=((struct Cyc_Absyn_Decl_s_struct*)_tmp4D2)->f1;_tmp4F7=*
_tmp4F6;_tmp4F8=(void*)_tmp4F7.r;if(_tmp4F8 <= (void*)2)goto _LL32F;if(*((int*)
_tmp4F8)!= 5)goto _LL32F;_tmp4F9=((struct Cyc_Absyn_Alias_d_struct*)_tmp4F8)->f1;
_tmp4FA=((struct Cyc_Absyn_Alias_d_struct*)_tmp4F8)->f2;_tmp4FB=((struct Cyc_Absyn_Alias_d_struct*)
_tmp4F8)->f3;_tmp4FC=((struct Cyc_Absyn_Decl_s_struct*)_tmp4D2)->f2;_LL32E: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp53A;void*_tmp53B;struct _tuple5 _tmp539=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4F9);_tmp53A=_tmp539.f1;_tmp53B=_tmp539.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
_tmp53D;struct Cyc_List_List*_tmp53E;struct _tuple6 _tmp53C=Cyc_CfFlowInfo_save_consume_info(
_tmp4D1,_tmp53A,0);_tmp53D=_tmp53C.f1;_tmp53E=_tmp53D.may_consume;_tmp53A=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4F9->loc,_tmp53A);_tmp53A=Cyc_NewControlFlow_use_Rval(env,_tmp4F9->loc,
_tmp53A,_tmp53B);{struct Cyc_List_List _tmp53F=({struct Cyc_List_List _tmp546;
_tmp546.hd=_tmp4FB;_tmp546.tl=0;_tmp546;});_tmp53A=Cyc_NewControlFlow_add_vars(
_tmp4D1,_tmp53A,(struct Cyc_List_List*)& _tmp53F,(void*)_tmp4D1->unknown_all,s->loc);
outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp53A,_tmp4FC);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp540=outflow;struct Cyc_Dict_Dict _tmp541;struct Cyc_List_List*_tmp542;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp543;_LL33D: if((_tmp540.BottomFL).tag != 0)goto _LL33F;_LL33E: goto _LL33C;_LL33F:
if((_tmp540.ReachableFL).tag != 1)goto _LL33C;_tmp541=(_tmp540.ReachableFL).f1;
_tmp542=(_tmp540.ReachableFL).f2;_tmp543=(_tmp540.ReachableFL).f3;_LL340: while(
_tmp53E != 0){struct Cyc_Dict_Dict _tmp544=_tmp543.consumed;_tmp543.consumed=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))
Cyc_Dict_rdelete)(_tmp4D1->r,_tmp543.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp53E->hd);if((_tmp543.consumed).t != _tmp544.t);_tmp53E=_tmp53E->tl;}outflow=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp545;(
_tmp545.ReachableFL).tag=1;(_tmp545.ReachableFL).f1=_tmp541;(_tmp545.ReachableFL).f2=
_tmp542;(_tmp545.ReachableFL).f3=_tmp543;_tmp545;});goto _LL33C;_LL33C:;}goto
_LL30C;}}}_LL32F: if(*((int*)_tmp4D2)!= 11)goto _LL331;_tmp4FD=((struct Cyc_Absyn_Decl_s_struct*)
_tmp4D2)->f1;_tmp4FE=((struct Cyc_Absyn_Decl_s_struct*)_tmp4D2)->f2;_LL330:
outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp4FD),_tmp4FE);goto _LL30C;_LL331: if(*((int*)_tmp4D2)!= 12)goto _LL333;_tmp4FF=((
struct Cyc_Absyn_Label_s_struct*)_tmp4D2)->f2;_LL332: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4FF);goto _LL30C;_LL333: if(*((int*)_tmp4D2)!= 15)goto _LL335;
_tmp500=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp4D2)->f1;_LL334: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp548;void*_tmp549;struct _tuple5 _tmp547=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp500);_tmp548=_tmp547.f1;_tmp549=_tmp547.f2;_tmp548=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp500->loc,_tmp548);{union Cyc_CfFlowInfo_FlowInfo_union _tmp54A=Cyc_NewControlFlow_use_Rval(
env,_tmp500->loc,_tmp548,_tmp549);{void*_tmp54B=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp500->topt))->v);void*_tmp54C;_LL342: if(
_tmp54B <= (void*)4)goto _LL344;if(*((int*)_tmp54B)!= 14)goto _LL344;_tmp54C=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp54B)->f1;_LL343: outflow=Cyc_CfFlowInfo_kill_flow_region(
_tmp4D1,_tmp548,_tmp54C);goto _LL341;_LL344:;_LL345:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D[0]=({
struct Cyc_Core_Impossible_struct _tmp54E;_tmp54E.tag=Cyc_Core_Impossible;_tmp54E.f1=({
const char*_tmp54F="anal_stmt -- reset_region";_tag_dyneither(_tmp54F,sizeof(char),
26);});_tmp54E;});_tmp54D;}));_LL341:;}goto _LL30C;}}_LL335:;_LL336:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp550=_cycalloc(sizeof(*_tmp550));
_tmp550[0]=({struct Cyc_Core_Impossible_struct _tmp551;_tmp551.tag=Cyc_Core_Impossible;
_tmp551.f1=({const char*_tmp552="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_dyneither(_tmp552,sizeof(char),56);});_tmp551;});_tmp550;}));_LL30C:;}
outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp553=outflow;struct Cyc_CfFlowInfo_ConsumeInfo _tmp554;_LL347: if((_tmp553.ReachableFL).tag
!= 1)goto _LL349;_tmp554=(_tmp553.ReachableFL).f3;_LL348: goto _LL346;_LL349:;
_LL34A: goto _LL346;_LL346:;}return outflow;}}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*
fd);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct
Cyc_Absyn_Decl*decl){void*_tmp555=(void*)decl->r;struct Cyc_Absyn_Vardecl*_tmp556;
struct Cyc_Core_Opt*_tmp557;struct Cyc_Core_Opt _tmp558;struct Cyc_List_List*_tmp559;
struct Cyc_Absyn_Exp*_tmp55A;struct Cyc_List_List*_tmp55B;struct Cyc_Absyn_Fndecl*
_tmp55C;struct Cyc_Absyn_Tvar*_tmp55D;struct Cyc_Absyn_Vardecl*_tmp55E;int _tmp55F;
struct Cyc_Absyn_Exp*_tmp560;_LL34C: if(_tmp555 <= (void*)2)goto _LL356;if(*((int*)
_tmp555)!= 0)goto _LL34E;_tmp556=((struct Cyc_Absyn_Var_d_struct*)_tmp555)->f1;
_LL34D: {struct Cyc_List_List _tmp561=({struct Cyc_List_List _tmp570;_tmp570.hd=
_tmp556;_tmp570.tl=0;_tmp570;});inflow=Cyc_NewControlFlow_add_vars(env->fenv,
inflow,(struct Cyc_List_List*)& _tmp561,(void*)(env->fenv)->unknown_none,decl->loc);{
struct Cyc_Absyn_Exp*_tmp562=_tmp556->initializer;if(_tmp562 == 0)return inflow;{
union Cyc_CfFlowInfo_FlowInfo_union _tmp564;void*_tmp565;struct _tuple5 _tmp563=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)_tmp562);_tmp564=_tmp563.f1;_tmp565=_tmp563.f2;
_tmp564=Cyc_CfFlowInfo_consume_unique_rvals(_tmp562->loc,_tmp564);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp566=_tmp564;struct Cyc_Dict_Dict _tmp567;struct Cyc_List_List*_tmp568;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp569;_LL359: if((_tmp566.BottomFL).tag != 0)goto _LL35B;_LL35A: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp56A;(_tmp56A.BottomFL).tag=0;_tmp56A;});
_LL35B: if((_tmp566.ReachableFL).tag != 1)goto _LL358;_tmp567=(_tmp566.ReachableFL).f1;
_tmp568=(_tmp566.ReachableFL).f2;_tmp569=(_tmp566.ReachableFL).f3;_LL35C: _tmp567=
Cyc_CfFlowInfo_assign_place(env->fenv,decl->loc,_tmp567,env->all_changed,({
struct Cyc_CfFlowInfo_Place*_tmp56B=_region_malloc(env->r,sizeof(*_tmp56B));
_tmp56B->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp56C=
_region_malloc(env->r,sizeof(*_tmp56C));_tmp56C[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp56D;_tmp56D.tag=0;_tmp56D.f1=_tmp556;_tmp56D;});_tmp56C;}));_tmp56B->fields=
0;_tmp56B;}),_tmp565);_tmp568=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp568,
_tmp556,(struct Cyc_Absyn_Exp*)_check_null(_tmp556->initializer));{union Cyc_CfFlowInfo_FlowInfo_union
_tmp56E=({union Cyc_CfFlowInfo_FlowInfo_union _tmp56F;(_tmp56F.ReachableFL).tag=1;(
_tmp56F.ReachableFL).f1=_tmp567;(_tmp56F.ReachableFL).f2=_tmp568;(_tmp56F.ReachableFL).f3=
_tmp569;_tmp56F;});Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp56E);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp56E;}_LL358:;}}}}_LL34E:
if(*((int*)_tmp555)!= 2)goto _LL350;_tmp557=((struct Cyc_Absyn_Let_d_struct*)
_tmp555)->f2;if(_tmp557 == 0)goto _LL350;_tmp558=*_tmp557;_tmp559=(struct Cyc_List_List*)
_tmp558.v;_tmp55A=((struct Cyc_Absyn_Let_d_struct*)_tmp555)->f3;_LL34F: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp572;void*_tmp573;struct _tuple5 _tmp571=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp55A);_tmp572=_tmp571.f1;_tmp573=_tmp571.f2;_tmp572=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp55A->loc,_tmp572);_tmp572=Cyc_NewControlFlow_use_Rval(env,_tmp55A->loc,
_tmp572,_tmp573);return Cyc_NewControlFlow_add_vars(env->fenv,_tmp572,_tmp559,(
void*)(env->fenv)->unknown_all,decl->loc);}_LL350: if(*((int*)_tmp555)!= 3)goto
_LL352;_tmp55B=((struct Cyc_Absyn_Letv_d_struct*)_tmp555)->f1;_LL351: return Cyc_NewControlFlow_add_vars(
env->fenv,inflow,_tmp55B,(void*)(env->fenv)->unknown_none,decl->loc);_LL352: if(*((
int*)_tmp555)!= 1)goto _LL354;_tmp55C=((struct Cyc_Absyn_Fn_d_struct*)_tmp555)->f1;
_LL353: Cyc_NewControlFlow_check_nested_fun(env->fenv,inflow,_tmp55C);{void*
_tmp574=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp55C->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp575=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp55C->fn_vardecl);
return Cyc_NewControlFlow_add_vars(env->fenv,inflow,({struct Cyc_List_List*_tmp576=
_region_malloc(env->r,sizeof(*_tmp576));_tmp576->hd=_tmp575;_tmp576->tl=0;
_tmp576;}),(void*)(env->fenv)->unknown_all,decl->loc);}_LL354: if(*((int*)_tmp555)
!= 4)goto _LL356;_tmp55D=((struct Cyc_Absyn_Region_d_struct*)_tmp555)->f1;_tmp55E=((
struct Cyc_Absyn_Region_d_struct*)_tmp555)->f2;_tmp55F=((struct Cyc_Absyn_Region_d_struct*)
_tmp555)->f3;_tmp560=((struct Cyc_Absyn_Region_d_struct*)_tmp555)->f4;_LL355: if(
_tmp560 != 0){struct Cyc_Absyn_Exp*_tmp577=(struct Cyc_Absyn_Exp*)_tmp560;union Cyc_CfFlowInfo_FlowInfo_union
_tmp579;void*_tmp57A;struct _tuple5 _tmp578=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp577);_tmp579=_tmp578.f1;_tmp57A=_tmp578.f2;_tmp579=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp577->loc,_tmp579);inflow=Cyc_NewControlFlow_use_Rval(env,_tmp577->loc,
_tmp579,_tmp57A);}{struct Cyc_List_List _tmp57B=({struct Cyc_List_List _tmp57C;
_tmp57C.hd=_tmp55E;_tmp57C.tl=0;_tmp57C;});return Cyc_NewControlFlow_add_vars(env->fenv,
inflow,(struct Cyc_List_List*)& _tmp57B,(void*)(env->fenv)->unknown_all,decl->loc);}
_LL356:;_LL357:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp57D=
_cycalloc(sizeof(*_tmp57D));_tmp57D[0]=({struct Cyc_Core_Impossible_struct _tmp57E;
_tmp57E.tag=Cyc_Core_Impossible;_tmp57E.f1=({const char*_tmp57F="anal_decl: unexpected decl variant";
_tag_dyneither(_tmp57F,sizeof(char),35);});_tmp57E;});_tmp57D;}));_LL34B:;}
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle _tmp580=_new_region("frgn");struct _RegionHandle*frgn=& _tmp580;
_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);Cyc_NewControlFlow_check_nested_fun(fenv,(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp581;(_tmp581.ReachableFL).tag=1;(_tmp581.ReachableFL).f1=
fenv->mt_flowdict;(_tmp581.ReachableFL).f2=0;(_tmp581.ReachableFL).f3=({struct
Cyc_CfFlowInfo_ConsumeInfo _tmp582;_tmp582.consumed=fenv->mt_place_set;_tmp582.may_consume=
0;_tmp582;});_tmp581;}),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(
void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp583=fenv->r;struct Cyc_Position_Segment*_tmp584=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(void*)fenv->unknown_all,_tmp584);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo_union _tmp585=inflow;struct Cyc_Dict_Dict
_tmp586;struct Cyc_List_List*_tmp587;struct Cyc_CfFlowInfo_ConsumeInfo _tmp588;
_LL35E: if((_tmp585.BottomFL).tag != 0)goto _LL360;_LL35F:({void*_tmp589=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp58A="check_fun";_tag_dyneither(_tmp58A,sizeof(char),10);}),_tag_dyneither(
_tmp589,sizeof(void*),0));});_LL360: if((_tmp585.ReachableFL).tag != 1)goto _LL35D;
_tmp586=(_tmp585.ReachableFL).f1;_tmp587=(_tmp585.ReachableFL).f2;_tmp588=(
_tmp585.ReachableFL).f3;_LL361: {struct Cyc_List_List*atts;{void*_tmp58B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo
_tmp58C;struct Cyc_List_List*_tmp58D;_LL363: if(_tmp58B <= (void*)4)goto _LL365;if(*((
int*)_tmp58B)!= 8)goto _LL365;_tmp58C=((struct Cyc_Absyn_FnType_struct*)_tmp58B)->f1;
_tmp58D=_tmp58C.attributes;_LL364: atts=_tmp58D;goto _LL362;_LL365:;_LL366:({void*
_tmp58E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp58F="check_fun: non-function type cached with fndecl_t";
_tag_dyneither(_tmp58F,sizeof(char),50);}),_tag_dyneither(_tmp58E,sizeof(void*),
0));});_LL362:;}for(0;atts != 0;atts=atts->tl){void*_tmp590=(void*)atts->hd;int
_tmp591;_LL368: if(_tmp590 <= (void*)17)goto _LL36A;if(*((int*)_tmp590)!= 4)goto
_LL36A;_tmp591=((struct Cyc_Absyn_Initializes_att_struct*)_tmp590)->f1;_LL369: {
unsigned int j=(unsigned int)_tmp591;if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){({
void*_tmp592=0;Cyc_Tcutil_terr(_tmp584,({const char*_tmp593="initializes attribute exceeds number of parameters";
_tag_dyneither(_tmp593,sizeof(char),51);}),_tag_dyneither(_tmp592,sizeof(void*),
0));});continue;}{struct Cyc_Absyn_Vardecl*_tmp594=((struct Cyc_Absyn_Vardecl*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp595=Cyc_Tcutil_compress((
void*)_tmp594->type);struct Cyc_Absyn_PtrInfo _tmp596;void*_tmp597;struct Cyc_Absyn_PtrAtts
_tmp598;struct Cyc_Absyn_Conref*_tmp599;struct Cyc_Absyn_Conref*_tmp59A;struct Cyc_Absyn_Conref*
_tmp59B;_LL36D: if(_tmp595 <= (void*)4)goto _LL36F;if(*((int*)_tmp595)!= 4)goto
_LL36F;_tmp596=((struct Cyc_Absyn_PointerType_struct*)_tmp595)->f1;_tmp597=(void*)
_tmp596.elt_typ;_tmp598=_tmp596.ptr_atts;_tmp599=_tmp598.nullable;_tmp59A=
_tmp598.bounds;_tmp59B=_tmp598.zero_term;_LL36E: if(((int(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_val)(_tmp599))({void*_tmp59C=0;Cyc_Tcutil_terr(_tmp584,({
const char*_tmp59D="initializes attribute not allowed on nullable pointers";
_tag_dyneither(_tmp59D,sizeof(char),55);}),_tag_dyneither(_tmp59C,sizeof(void*),
0));});if(!Cyc_Tcutil_is_bound_one(_tmp59A))({void*_tmp59E=0;Cyc_Tcutil_terr(
_tmp584,({const char*_tmp59F="initializes attribute allowed only on pointers of size 1";
_tag_dyneither(_tmp59F,sizeof(char),57);}),_tag_dyneither(_tmp59E,sizeof(void*),
0));});if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp59B))({
void*_tmp5A0=0;Cyc_Tcutil_terr(_tmp584,({const char*_tmp5A1="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_dyneither(_tmp5A1,sizeof(char),77);}),_tag_dyneither(_tmp5A0,sizeof(void*),
0));});{struct Cyc_CfFlowInfo_InitParam_struct*_tmp5A2=({struct Cyc_CfFlowInfo_InitParam_struct*
_tmp5AA=_region_malloc(_tmp583,sizeof(*_tmp5AA));_tmp5AA[0]=({struct Cyc_CfFlowInfo_InitParam_struct
_tmp5AB;_tmp5AB.tag=2;_tmp5AB.f1=(int)j;_tmp5AB.f2=(void*)_tmp597;_tmp5AB;});
_tmp5AA;});struct Cyc_CfFlowInfo_Place*_tmp5A3=({struct Cyc_CfFlowInfo_Place*
_tmp5A9=_region_malloc(_tmp583,sizeof(*_tmp5A9));_tmp5A9->root=(void*)((void*)
_tmp5A2);_tmp5A9->fields=0;_tmp5A9;});_tmp586=Cyc_Dict_insert(_tmp586,(void*)
_tmp5A2,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp597,(void*)fenv->esc_none));
_tmp586=Cyc_Dict_insert(_tmp586,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp5A4=_region_malloc(_tmp583,sizeof(*_tmp5A4));_tmp5A4[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp5A5;_tmp5A5.tag=0;_tmp5A5.f1=_tmp594;_tmp5A5;});_tmp5A4;}),(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp5A6=_region_malloc(_tmp583,sizeof(*_tmp5A6));
_tmp5A6[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp5A7;_tmp5A7.tag=2;_tmp5A7.f1=
_tmp5A3;_tmp5A7;});_tmp5A6;}));param_roots=({struct Cyc_List_List*_tmp5A8=
_region_malloc(_tmp583,sizeof(*_tmp5A8));_tmp5A8->hd=_tmp5A3;_tmp5A8->tl=
param_roots;_tmp5A8;});goto _LL36C;}_LL36F:;_LL370:({void*_tmp5AC=0;Cyc_Tcutil_terr(
_tmp584,({const char*_tmp5AD="initializes attribute on non-pointer";
_tag_dyneither(_tmp5AD,sizeof(char),37);}),_tag_dyneither(_tmp5AC,sizeof(void*),
0));});_LL36C:;}goto _LL367;}}_LL36A:;_LL36B: goto _LL367;_LL367:;}inflow=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp5AE;(_tmp5AE.ReachableFL).tag=1;(_tmp5AE.ReachableFL).f1=
_tmp586;(_tmp5AE.ReachableFL).f2=_tmp587;(_tmp5AE.ReachableFL).f3=_tmp588;
_tmp5AE;});}_LL35D:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp583,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp5B7=_region_malloc(_tmp583,sizeof(*_tmp5B7));_tmp5B7->r=_tmp583;_tmp5B7->fenv=
fenv;_tmp5B7->iterate_again=1;_tmp5B7->iteration_num=0;_tmp5B7->in_try=0;_tmp5B7->tryflow=
inflow;_tmp5B7->all_changed=0;_tmp5B7->noreturn=noreturn;_tmp5B7->param_roots=
param_roots;_tmp5B7->flow_table=flow_table;_tmp5B7;});union Cyc_CfFlowInfo_FlowInfo_union
outflow=inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5AF=outflow;_LL372: if((_tmp5AF.BottomFL).tag != 0)goto _LL374;_LL373: goto
_LL371;_LL374:;_LL375: Cyc_NewControlFlow_check_init_params(_tmp584,env,outflow);
if(noreturn)({void*_tmp5B0=0;Cyc_Tcutil_terr(_tmp584,({const char*_tmp5B1="`noreturn' function might (implicitly) return";
_tag_dyneither(_tmp5B1,sizeof(char),46);}),_tag_dyneither(_tmp5B0,sizeof(void*),
0));});else{void*_tmp5B2=Cyc_Tcutil_compress((void*)fd->ret_type);_LL377: if((int)
_tmp5B2 != 0)goto _LL379;_LL378: goto _LL376;_LL379: if(_tmp5B2 <= (void*)4)goto _LL37B;
if(*((int*)_tmp5B2)!= 6)goto _LL37B;_LL37A: goto _LL37C;_LL37B: if((int)_tmp5B2 != 1)
goto _LL37D;_LL37C: goto _LL37E;_LL37D: if(_tmp5B2 <= (void*)4)goto _LL37F;if(*((int*)
_tmp5B2)!= 5)goto _LL37F;_LL37E:({void*_tmp5B3=0;Cyc_Tcutil_warn(_tmp584,({const
char*_tmp5B4="function may not return a value";_tag_dyneither(_tmp5B4,sizeof(
char),32);}),_tag_dyneither(_tmp5B3,sizeof(void*),0));});goto _LL376;_LL37F:;
_LL380:({void*_tmp5B5=0;Cyc_Tcutil_terr(_tmp584,({const char*_tmp5B6="function may not return a value";
_tag_dyneither(_tmp5B6,sizeof(char),32);}),_tag_dyneither(_tmp5B5,sizeof(void*),
0));});goto _LL376;_LL376:;}goto _LL371;_LL371:;}}}}void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*_tmp5B8=(void*)((struct Cyc_Absyn_Decl*)
ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp5B9;struct Cyc_List_List*_tmp5BA;struct Cyc_List_List*
_tmp5BB;struct Cyc_List_List*_tmp5BC;_LL382: if(_tmp5B8 <= (void*)2)goto _LL38C;if(*((
int*)_tmp5B8)!= 1)goto _LL384;_tmp5B9=((struct Cyc_Absyn_Fn_d_struct*)_tmp5B8)->f1;
_LL383: Cyc_NewControlFlow_check_fun(_tmp5B9);goto _LL381;_LL384: if(*((int*)
_tmp5B8)!= 12)goto _LL386;_tmp5BA=((struct Cyc_Absyn_ExternC_d_struct*)_tmp5B8)->f1;
_LL385: _tmp5BB=_tmp5BA;goto _LL387;_LL386: if(*((int*)_tmp5B8)!= 11)goto _LL388;
_tmp5BB=((struct Cyc_Absyn_Using_d_struct*)_tmp5B8)->f2;_LL387: _tmp5BC=_tmp5BB;
goto _LL389;_LL388: if(*((int*)_tmp5B8)!= 10)goto _LL38A;_tmp5BC=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp5B8)->f2;_LL389: Cyc_NewControlFlow_cf_check(_tmp5BC);goto _LL381;_LL38A: if(*((
int*)_tmp5B8)!= 13)goto _LL38C;_LL38B: goto _LL381;_LL38C:;_LL38D: goto _LL381;_LL381:;}}
