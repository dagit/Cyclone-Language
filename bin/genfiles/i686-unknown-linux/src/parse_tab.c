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
void*v;};extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
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
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(
struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};void Cyc_List_iter(void(*f)(
void*),struct Cyc_List_List*x);void*Cyc_List_fold_right(void*(*f)(void*,void*),
struct Cyc_List_List*x,void*accum);struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_flatten(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_rsplit(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
unsigned int Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct _dyneither_ptr
s1,struct _dyneither_ptr s2);int Cyc_zstrcmp(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src);
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned int n);
struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;
int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct _dyneither_ptr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_parse(struct
Cyc_Position_Segment*,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct
_dyneither_ptr*f2;};enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract
 = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register
 = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int
real_const;struct Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz
 = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz
 = 4};enum Cyc_Absyn_Kind{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind
 = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_UniqueRgnKind  = 4,Cyc_Absyn_TopRgnKind  = 5,
Cyc_Absyn_EffKind  = 6,Cyc_Absyn_IntKind  = 7};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed
 = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA
 = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};
struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};
struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{
struct _union_Constraint_Eq_constr Eq_constr;struct
_union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr
No_constr;};struct Cyc_Absyn_Eq_kb_struct{int tag;enum Cyc_Absyn_Kind f1;};struct
Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;enum Cyc_Absyn_Kind f2;};struct Cyc_Absyn_Tvar{struct
_dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_struct{
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};extern
struct Cyc_Absyn_DynEither_b_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{
struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;
union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*
ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct
Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_struct{int tag;void*f1;};struct
Cyc_Absyn_Thin_ptrqual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{int tag;
};struct Cyc_Absyn_Zeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{
int tag;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct
Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct
Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;
struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3
val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};
union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct
_union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{
union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_VoidType_struct{
int tag;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;}
;struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
f1;};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
struct Cyc_Absyn_IntType_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of
f2;};struct Cyc_Absyn_FloatType_struct{int tag;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,
Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct
Cyc_Absyn_Stdcall_att_struct{int tag;};struct Cyc_Absyn_Cdecl_att_struct{int tag;};
struct Cyc_Absyn_Fastcall_att_struct{int tag;};struct Cyc_Absyn_Noreturn_att_struct{
int tag;};struct Cyc_Absyn_Const_att_struct{int tag;};struct Cyc_Absyn_Aligned_att_struct{
int tag;int f1;};struct Cyc_Absyn_Packed_att_struct{int tag;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_struct{int tag;};
struct Cyc_Absyn_Shared_att_struct{int tag;};struct Cyc_Absyn_Unused_att_struct{int
tag;};struct Cyc_Absyn_Weak_att_struct{int tag;};struct Cyc_Absyn_Dllimport_att_struct{
int tag;};struct Cyc_Absyn_Dllexport_att_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_struct{
int tag;};struct Cyc_Absyn_Constructor_att_struct{int tag;};struct Cyc_Absyn_Destructor_att_struct{
int tag;};struct Cyc_Absyn_No_check_memory_usage_att_struct{int tag;};struct Cyc_Absyn_Format_att_struct{
int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{
int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{int tag;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dyneither_ptr f1;};extern struct Cyc_Absyn_Stdcall_att_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_struct Cyc_Absyn_Cdecl_att_val;extern struct Cyc_Absyn_Fastcall_att_struct
Cyc_Absyn_Fastcall_att_val;extern struct Cyc_Absyn_Noreturn_att_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_struct Cyc_Absyn_Const_att_val;extern struct Cyc_Absyn_Packed_att_struct
Cyc_Absyn_Packed_att_val;extern struct Cyc_Absyn_Shared_att_struct Cyc_Absyn_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_struct Cyc_Absyn_Unused_att_val;extern struct Cyc_Absyn_Weak_att_struct
Cyc_Absyn_Weak_att_val;extern struct Cyc_Absyn_Dllimport_att_struct Cyc_Absyn_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_struct Cyc_Absyn_Dllexport_att_val;extern
struct Cyc_Absyn_No_instrument_function_att_struct Cyc_Absyn_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_struct Cyc_Absyn_Constructor_att_val;extern
struct Cyc_Absyn_Destructor_att_struct Cyc_Absyn_Destructor_att_val;extern struct
Cyc_Absyn_No_check_memory_usage_att_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_struct Cyc_Absyn_Pure_att_val;struct Cyc_Absyn_Carray_mod_struct{
int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct
_tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7
val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct
_union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times
 = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,
Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte
 = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor
 = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,
Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc
 = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,
Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion
 = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**
elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct
Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;
};struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnknownCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*
f3;};struct Cyc_Absyn_Throw_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{
int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct
Cyc_Absyn_Address_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{
int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{
int tag;struct Cyc_List_List*f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
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
struct Cyc_Absyn_Skip_s_struct{int tag;};struct Cyc_Absyn_Exp_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;
};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*loc;struct Cyc_List_List*
non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_struct{int tag;}
;struct Cyc_Absyn_Var_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*
f2;};struct Cyc_Absyn_Reference_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct
Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{int tag;struct Cyc_Absyn_Tvar*
f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{int tag;struct Cyc_List_List*
f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};
struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*
f3;int f4;};struct Cyc_Absyn_Null_p_struct{int tag;};struct Cyc_Absyn_Int_p_struct{
int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Enum_p_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{
int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*
r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope
sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;
};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope
sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum Cyc_Absyn_Scope
sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};
struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*
fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*defn;struct
Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*
f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*
f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Datatype_d_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{
int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int
tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};extern struct Cyc_Absyn_Porton_d_struct
Cyc_Absyn_Porton_d_val;extern struct Cyc_Absyn_Portoff_d_struct Cyc_Absyn_Portoff_d_val;
struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};int Cyc_Absyn_is_qvar_qualified(struct _tuple1*);struct Cyc_Absyn_Tqual
Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(
void*x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();extern union Cyc_Absyn_Constraint*
Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(int);void*Cyc_Absyn_starb_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
enum Cyc_Absyn_Sign,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(
enum Cyc_Absyn_Primop,struct Cyc_List_List*es,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*
exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);struct Cyc_Absyn_Decl*
Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple1*
n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(
enum Cyc_Absyn_Scope s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_extensible,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(void*a);extern int Cyc_Absyn_porting_c_code;
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct
Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};struct _tuple10{void*f1;void*f2;};struct
_tuple10*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct
_tuple10*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;
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
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_NotLoop_j_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_struct{
int tag;};struct Cyc_Tcenv_FnEnd_j_struct{int tag;};struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*
Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_compress(void*t);extern struct Cyc_Core_Opt
Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_trk;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;
extern struct Cyc_Core_Opt Cyc_Tcutil_ik;struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(
enum Cyc_Absyn_Kind k);void*Cyc_Tcutil_kind_to_bound(enum Cyc_Absyn_Kind k);struct
Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};void Cyc_Lex_register_typedef(struct _tuple1*s);void Cyc_Lex_enter_namespace(
struct _dyneither_ptr*);void Cyc_Lex_leave_namespace();void Cyc_Lex_enter_using(
struct _tuple1*);void Cyc_Lex_leave_using();int Cyc_Parse_no_register=0;struct Cyc_Parse_Signed_spec_struct{
int tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Unsigned_spec_struct{int
tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Short_spec_struct{int tag;
struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Long_spec_struct{int tag;struct
Cyc_Position_Segment*f1;};struct Cyc_Parse_Type_spec_struct{int tag;void*f1;struct
Cyc_Position_Segment*f2;};struct Cyc_Parse_Decl_spec_struct{int tag;struct Cyc_Absyn_Decl*
f1;};enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc  = 0,Cyc_Parse_Extern_sc
 = 1,Cyc_Parse_ExternC_sc  = 2,Cyc_Parse_Static_sc  = 3,Cyc_Parse_Auto_sc  = 4,Cyc_Parse_Register_sc
 = 5,Cyc_Parse_Abstract_sc  = 6};struct Cyc_Parse_Declaration_spec{struct Cyc_Core_Opt*
sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int is_inline;struct Cyc_List_List*
attributes;};struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;
};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _tuple11{
void*f1;struct Cyc_Core_Opt*f2;};static struct _tuple11 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);struct _tuple12{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple12
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
static struct Cyc_List_List*Cyc_Parse_parse_result=0;static void Cyc_Parse_err(
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){Cyc_Position_post_error(
Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Parse_err((struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);(int)_throw((void*)({
struct Cyc_Position_Exit_struct*_tmp0=_cycalloc_atomic(sizeof(*_tmp0));_tmp0[0]=({
struct Cyc_Position_Exit_struct _tmp1;_tmp1.tag=Cyc_Position_Exit;_tmp1;});_tmp0;}));}
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){({
struct Cyc_String_pa_struct _tmp5;_tmp5.tag=0;_tmp5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg);{struct Cyc_String_pa_struct _tmp4;_tmp4.tag=0;_tmp4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(sg));{
void*_tmp2[2]={& _tmp4,& _tmp5};Cyc_fprintf(Cyc_stderr,({const char*_tmp3="%s: Warning: Cyclone does not yet support %s\n";
_tag_dyneither(_tmp3,sizeof(char),46);}),_tag_dyneither(_tmp2,sizeof(void*),2));}}});
return;}struct _tuple13{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;
union Cyc_Absyn_Constraint*f3;void*f4;};static struct _tuple13 Cyc_Parse_collapse_pointer_quals(
struct Cyc_Position_Segment*loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*
bound,void*rgn,struct Cyc_List_List*pqs){union Cyc_Absyn_Constraint*zeroterm=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();for(0;pqs != 0;pqs=pqs->tl){
void*_tmp6=(void*)pqs->hd;struct Cyc_Absyn_Exp*_tmpE;void*_tmp10;_LL1: {struct Cyc_Absyn_Zeroterm_ptrqual_struct*
_tmp7=(struct Cyc_Absyn_Zeroterm_ptrqual_struct*)_tmp6;if(_tmp7->tag != 4)goto _LL3;}
_LL2: zeroterm=Cyc_Absyn_true_conref;goto _LL0;_LL3: {struct Cyc_Absyn_Nozeroterm_ptrqual_struct*
_tmp8=(struct Cyc_Absyn_Nozeroterm_ptrqual_struct*)_tmp6;if(_tmp8->tag != 5)goto
_LL5;}_LL4: zeroterm=Cyc_Absyn_false_conref;goto _LL0;_LL5: {struct Cyc_Absyn_Nullable_ptrqual_struct*
_tmp9=(struct Cyc_Absyn_Nullable_ptrqual_struct*)_tmp6;if(_tmp9->tag != 7)goto _LL7;}
_LL6: nullable=Cyc_Absyn_true_conref;goto _LL0;_LL7: {struct Cyc_Absyn_Notnull_ptrqual_struct*
_tmpA=(struct Cyc_Absyn_Notnull_ptrqual_struct*)_tmp6;if(_tmpA->tag != 6)goto _LL9;}
_LL8: nullable=Cyc_Absyn_false_conref;goto _LL0;_LL9: {struct Cyc_Absyn_Fat_ptrqual_struct*
_tmpB=(struct Cyc_Absyn_Fat_ptrqual_struct*)_tmp6;if(_tmpB->tag != 3)goto _LLB;}
_LLA: bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL0;_LLB: {struct Cyc_Absyn_Thin_ptrqual_struct*
_tmpC=(struct Cyc_Absyn_Thin_ptrqual_struct*)_tmp6;if(_tmpC->tag != 2)goto _LLD;}
_LLC: bound=Cyc_Absyn_bounds_one_conref;goto _LL0;_LLD: {struct Cyc_Absyn_Numelts_ptrqual_struct*
_tmpD=(struct Cyc_Absyn_Numelts_ptrqual_struct*)_tmp6;if(_tmpD->tag != 0)goto _LLF;
else{_tmpE=_tmpD->f1;}}_LLE: bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11[
0]=({struct Cyc_Absyn_Upper_b_struct _tmp12;_tmp12.tag=1;_tmp12.f1=_tmpE;_tmp12;});
_tmp11;}));goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_struct*_tmpF=(struct
Cyc_Absyn_Region_ptrqual_struct*)_tmp6;if(_tmpF->tag != 1)goto _LL0;else{_tmp10=(
void*)_tmpF->f1;}}_LL10: rgn=_tmp10;goto _LL0;_LL0:;}return({struct _tuple13 _tmp13;
_tmp13.f1=nullable;_tmp13.f2=bound;_tmp13.f3=zeroterm;_tmp13.f4=rgn;_tmp13;});}
static int Cyc_Parse_enum_counter=0;struct _tuple1*Cyc_Parse_gensym_enum(){return({
struct _tuple1*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->f1=Cyc_Absyn_Rel_n(0);
_tmp14->f2=({struct _dyneither_ptr*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15[0]=(
struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp18;_tmp18.tag=1;_tmp18.f1=(
unsigned long)Cyc_Parse_enum_counter ++;{void*_tmp16[1]={& _tmp18};Cyc_aprintf(({
const char*_tmp17="__anonymous_enum_%d__";_tag_dyneither(_tmp17,sizeof(char),22);}),
_tag_dyneither(_tmp16,sizeof(void*),1));}});_tmp15;});_tmp14;});}struct _tuple14{
struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct
Cyc_List_List*f5;};struct _tuple15{struct _tuple14*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*
loc,struct _tuple15*field_info){struct _tuple15 _tmp1A;struct _tuple14*_tmp1B;struct
_tuple14 _tmp1C;struct _tuple1*_tmp1D;struct Cyc_Absyn_Tqual _tmp1E;void*_tmp1F;
struct Cyc_List_List*_tmp20;struct Cyc_List_List*_tmp21;struct Cyc_Absyn_Exp*_tmp22;
struct _tuple15*_tmp19=field_info;_tmp1A=*_tmp19;_tmp1B=_tmp1A.f1;_tmp1C=*_tmp1B;
_tmp1D=_tmp1C.f1;_tmp1E=_tmp1C.f2;_tmp1F=_tmp1C.f3;_tmp20=_tmp1C.f4;_tmp21=
_tmp1C.f5;_tmp22=_tmp1A.f2;if(_tmp20 != 0)Cyc_Parse_err(({const char*_tmp23="bad type params in struct field";
_tag_dyneither(_tmp23,sizeof(char),32);}),loc);if(Cyc_Absyn_is_qvar_qualified(
_tmp1D))Cyc_Parse_err(({const char*_tmp24="struct or union field cannot be qualified with a namespace";
_tag_dyneither(_tmp24,sizeof(char),59);}),loc);return({struct Cyc_Absyn_Aggrfield*
_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->name=(*_tmp1D).f2;_tmp25->tq=_tmp1E;
_tmp25->type=_tmp1F;_tmp25->width=_tmp22;_tmp25->attributes=_tmp21;_tmp25;});}
static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*loc){return(
void*)({struct Cyc_Parse_Type_spec_struct*_tmp26=_cycalloc(sizeof(*_tmp26));
_tmp26[0]=({struct Cyc_Parse_Type_spec_struct _tmp27;_tmp27.tag=4;_tmp27.f1=(void*)
t;_tmp27.f2=loc;_tmp27;});_tmp26;});}static void*Cyc_Parse_array2ptr(void*t,int
argposn){void*_tmp28=t;struct Cyc_Absyn_ArrayInfo _tmp2A;void*_tmp2B;struct Cyc_Absyn_Tqual
_tmp2C;struct Cyc_Absyn_Exp*_tmp2D;union Cyc_Absyn_Constraint*_tmp2E;struct Cyc_Position_Segment*
_tmp2F;_LL12: {struct Cyc_Absyn_ArrayType_struct*_tmp29=(struct Cyc_Absyn_ArrayType_struct*)
_tmp28;if(_tmp29->tag != 9)goto _LL14;else{_tmp2A=_tmp29->f1;_tmp2B=_tmp2A.elt_type;
_tmp2C=_tmp2A.tq;_tmp2D=_tmp2A.num_elts;_tmp2E=_tmp2A.zero_term;_tmp2F=_tmp2A.zt_loc;}}
_LL13: return Cyc_Absyn_starb_typ(_tmp2B,argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0):(void*)& Cyc_Absyn_HeapRgn_val,_tmp2C,_tmp2D == 0?(void*)& Cyc_Absyn_DynEither_b_val:(
void*)((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp30=_cycalloc(sizeof(*_tmp30));
_tmp30[0]=({struct Cyc_Absyn_Upper_b_struct _tmp31;_tmp31.tag=1;_tmp31.f1=(struct
Cyc_Absyn_Exp*)_tmp2D;_tmp31;});_tmp30;})),_tmp2E);_LL14:;_LL15: return t;_LL11:;}
static void Cyc_Parse_arg_array2ptr(struct _tuple8*x){(*x).f3=Cyc_Parse_array2ptr((*
x).f3,1);}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple16*
Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct _tuple8*t){return({
struct _tuple16*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->f1=(*t).f2;_tmp32->f2=(*
t).f3;_tmp32;});}static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*
d){void*_tmp33=d->r;struct Cyc_Absyn_Vardecl*_tmp35;_LL17: {struct Cyc_Absyn_Var_d_struct*
_tmp34=(struct Cyc_Absyn_Var_d_struct*)_tmp33;if(_tmp34->tag != 0)goto _LL19;else{
_tmp35=_tmp34->f1;}}_LL18: return _tmp35;_LL19:;_LL1A:({void*_tmp36=0;((int(*)(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Parse_abort)(d->loc,({const char*_tmp37="bad declaration in `forarray' statement";
_tag_dyneither(_tmp37,sizeof(char),40);}),_tag_dyneither(_tmp36,sizeof(void*),0));});
_LL16:;}static int Cyc_Parse_is_typeparam(void*tm){void*_tmp38=tm;_LL1C: {struct
Cyc_Absyn_TypeParams_mod_struct*_tmp39=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp38;if(_tmp39->tag != 4)goto _LL1E;}_LL1D: return 1;_LL1E:;_LL1F: return 0;_LL1B:;}
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){if(Cyc_zstrcmp((
struct _dyneither_ptr)s,({const char*_tmp3A="`H";_tag_dyneither(_tmp3A,sizeof(char),
3);}))== 0)return(void*)& Cyc_Absyn_HeapRgn_val;else{if(Cyc_zstrcmp((struct
_dyneither_ptr)s,({const char*_tmp3B="`U";_tag_dyneither(_tmp3B,sizeof(char),3);}))
== 0)return(void*)& Cyc_Absyn_UniqueRgn_val;else{return(void*)({struct Cyc_Absyn_VarType_struct*
_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=({struct Cyc_Absyn_VarType_struct
_tmp3D;_tmp3D.tag=2;_tmp3D.f1=({struct Cyc_Absyn_Tvar*_tmp3E=_cycalloc(sizeof(*
_tmp3E));_tmp3E->name=({struct _dyneither_ptr*_tmp3F=_cycalloc(sizeof(*_tmp3F));
_tmp3F[0]=s;_tmp3F;});_tmp3E->identity=- 1;_tmp3E->kind=k;_tmp3E;});_tmp3D;});
_tmp3C;});}}}static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*
t){void*k;{void*_tmp40=Cyc_Absyn_compress_kb(t->kind);enum Cyc_Absyn_Kind _tmp43;
_LL21: {struct Cyc_Absyn_Unknown_kb_struct*_tmp41=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp40;if(_tmp41->tag != 1)goto _LL23;}_LL22: k=(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp45;_tmp45.tag=1;_tmp45.f1=0;_tmp45;});_tmp44;});goto _LL20;_LL23: {struct Cyc_Absyn_Less_kb_struct*
_tmp42=(struct Cyc_Absyn_Less_kb_struct*)_tmp40;if(_tmp42->tag != 2)goto _LL25;
else{_tmp43=_tmp42->f2;}}_LL24: k=(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp46=
_cycalloc(sizeof(*_tmp46));_tmp46[0]=({struct Cyc_Absyn_Less_kb_struct _tmp47;
_tmp47.tag=2;_tmp47.f1=0;_tmp47.f2=_tmp43;_tmp47;});_tmp46;});goto _LL20;_LL25:;
_LL26: k=_tmp40;goto _LL20;_LL20:;}return({struct Cyc_Absyn_Tvar*_tmp48=_cycalloc(
sizeof(*_tmp48));_tmp48->name=t->name;_tmp48->identity=- 1;_tmp48->kind=k;_tmp48;});}
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*loc,
void*t){void*_tmp49=t;struct Cyc_Absyn_Tvar*_tmp4B;_LL28: {struct Cyc_Absyn_VarType_struct*
_tmp4A=(struct Cyc_Absyn_VarType_struct*)_tmp49;if(_tmp4A->tag != 2)goto _LL2A;
else{_tmp4B=_tmp4A->f1;}}_LL29: return _tmp4B;_LL2A:;_LL2B:({void*_tmp4C=0;((int(*)(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Parse_abort)(loc,({const char*_tmp4D="expecting a list of type variables, not types";
_tag_dyneither(_tmp4D,sizeof(char),46);}),_tag_dyneither(_tmp4C,sizeof(void*),0));});
_LL27:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){return(void*)({
struct Cyc_Absyn_VarType_struct*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=({
struct Cyc_Absyn_VarType_struct _tmp4F;_tmp4F.tag=2;_tmp4F.f1=pr;_tmp4F;});_tmp4E;});}
static void Cyc_Parse_set_vartyp_kind(void*t,enum Cyc_Absyn_Kind k,int leq){void*
_tmp50=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp52;struct Cyc_Absyn_Tvar
_tmp53;void*_tmp54;void**_tmp55;_LL2D: {struct Cyc_Absyn_VarType_struct*_tmp51=(
struct Cyc_Absyn_VarType_struct*)_tmp50;if(_tmp51->tag != 2)goto _LL2F;else{_tmp52=
_tmp51->f1;_tmp53=*_tmp52;_tmp54=_tmp53.kind;_tmp55=(void**)&(*_tmp51->f1).kind;}}
_LL2E: {void*_tmp56=Cyc_Absyn_compress_kb(*_tmp55);_LL32: {struct Cyc_Absyn_Unknown_kb_struct*
_tmp57=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp56;if(_tmp57->tag != 1)goto _LL34;}
_LL33: if(!leq)*_tmp55=Cyc_Tcutil_kind_to_bound(k);else{*_tmp55=(void*)({struct
Cyc_Absyn_Less_kb_struct*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp59;_tmp59.tag=2;_tmp59.f1=0;_tmp59.f2=k;_tmp59;});_tmp58;});}return;_LL34:;
_LL35: return;_LL31:;}_LL2F:;_LL30: return;_LL2C:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp5A=(void*)tms->hd;void*_tmp5C;
_LL37: {struct Cyc_Absyn_Function_mod_struct*_tmp5B=(struct Cyc_Absyn_Function_mod_struct*)
_tmp5A;if(_tmp5B->tag != 3)goto _LL39;else{_tmp5C=(void*)_tmp5B->f1;}}_LL38: if(tms->tl
== 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)
 && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){void*_tmp5D=_tmp5C;
struct Cyc_List_List*_tmp60;_LL3C: {struct Cyc_Absyn_WithTypes_struct*_tmp5E=(
struct Cyc_Absyn_WithTypes_struct*)_tmp5D;if(_tmp5E->tag != 1)goto _LL3E;}_LL3D:({
void*_tmp61=0;Cyc_Tcutil_warn(loc,({const char*_tmp62="function declaration with both new- and old-style parameter declarations; ignoring old-style";
_tag_dyneither(_tmp62,sizeof(char),93);}),_tag_dyneither(_tmp61,sizeof(void*),0));});
return tms;_LL3E: {struct Cyc_Absyn_NoTypes_struct*_tmp5F=(struct Cyc_Absyn_NoTypes_struct*)
_tmp5D;if(_tmp5F->tag != 0)goto _LL3B;else{_tmp60=_tmp5F->f1;}}_LL3F: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp60)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds))({void*_tmp63=0;((int(*)(struct Cyc_Position_Segment*sg,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*
_tmp64="wrong number of parameter declarations in old-style function declaration";
_tag_dyneither(_tmp64,sizeof(char),73);}),_tag_dyneither(_tmp63,sizeof(void*),0));});{
struct Cyc_List_List*_tmp65=0;for(0;_tmp60 != 0;_tmp60=_tmp60->tl){struct Cyc_List_List*
_tmp66=tds;for(0;_tmp66 != 0;_tmp66=_tmp66->tl){struct Cyc_Absyn_Decl*_tmp67=(
struct Cyc_Absyn_Decl*)_tmp66->hd;void*_tmp68=_tmp67->r;struct Cyc_Absyn_Vardecl*
_tmp6A;_LL41: {struct Cyc_Absyn_Var_d_struct*_tmp69=(struct Cyc_Absyn_Var_d_struct*)
_tmp68;if(_tmp69->tag != 0)goto _LL43;else{_tmp6A=_tmp69->f1;}}_LL42: if(Cyc_zstrptrcmp((*
_tmp6A->name).f2,(struct _dyneither_ptr*)_tmp60->hd)!= 0)continue;if(_tmp6A->initializer
!= 0)({void*_tmp6B=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp67->loc,({const char*_tmp6C="initializer found in parameter declaration";
_tag_dyneither(_tmp6C,sizeof(char),43);}),_tag_dyneither(_tmp6B,sizeof(void*),0));});
if(Cyc_Absyn_is_qvar_qualified(_tmp6A->name))({void*_tmp6D=0;((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp67->loc,({
const char*_tmp6E="namespaces forbidden in parameter declarations";_tag_dyneither(
_tmp6E,sizeof(char),47);}),_tag_dyneither(_tmp6D,sizeof(void*),0));});_tmp65=({
struct Cyc_List_List*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->hd=({struct _tuple8*
_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->f1=({struct Cyc_Core_Opt*_tmp71=
_cycalloc(sizeof(*_tmp71));_tmp71->v=(*_tmp6A->name).f2;_tmp71;});_tmp70->f2=
_tmp6A->tq;_tmp70->f3=_tmp6A->type;_tmp70;});_tmp6F->tl=_tmp65;_tmp6F;});goto L;
_LL43:;_LL44:({void*_tmp72=0;((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp67->loc,({const
char*_tmp73="nonvariable declaration in parameter type";_tag_dyneither(_tmp73,
sizeof(char),42);}),_tag_dyneither(_tmp72,sizeof(void*),0));});_LL40:;}L: if(
_tmp66 == 0)({struct Cyc_String_pa_struct _tmp76;_tmp76.tag=0;_tmp76.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp60->hd));{
void*_tmp74[1]={& _tmp76};((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*
_tmp75="%s is not given a type";_tag_dyneither(_tmp75,sizeof(char),23);}),
_tag_dyneither(_tmp74,sizeof(void*),1));}});}return({struct Cyc_List_List*_tmp77=
_cycalloc(sizeof(*_tmp77));_tmp77->hd=(void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp79;_tmp79.tag=3;_tmp79.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp7B;_tmp7B.tag=1;_tmp7B.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp65);_tmp7B.f2=0;_tmp7B.f3=0;_tmp7B.f4=0;_tmp7B.f5=0;_tmp7B;});
_tmp7A;}));_tmp79;});_tmp78;});_tmp77->tl=0;_tmp77;});}_LL3B:;}goto _LL3A;_LL39:;
_LL3A: return({struct Cyc_List_List*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->hd=(
void*)tms->hd;_tmp7C->tl=Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);_tmp7C;});
_LL36:;}}struct _tuple17{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;
};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,
struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,
struct Cyc_Position_Segment*loc){if(tds != 0)d=({struct Cyc_Parse_Declarator*_tmp7D=
_cycalloc(sizeof(*_tmp7D));_tmp7D->id=d->id;_tmp7D->tms=Cyc_Parse_oldstyle2newstyle(
d->tms,tds,loc);_tmp7D;});{enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;struct Cyc_List_List*
tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);int is_inline=0;struct Cyc_List_List*
atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;
tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)
dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;
if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0)switch((enum Cyc_Parse_Storage_class)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc))->v){
case Cyc_Parse_Extern_sc: _LL45: sc=Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc:
_LL46: sc=Cyc_Absyn_Static;break;default: _LL47: Cyc_Parse_err(({const char*_tmp7E="bad storage class on function";
_tag_dyneither(_tmp7E,sizeof(char),30);}),loc);break;}}{void*_tmp80;struct Cyc_Core_Opt*
_tmp81;struct _tuple11 _tmp7F=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmp80=
_tmp7F.f1;_tmp81=_tmp7F.f2;{struct Cyc_Absyn_Tqual _tmp83;void*_tmp84;struct Cyc_List_List*
_tmp85;struct Cyc_List_List*_tmp86;struct _tuple12 _tmp82=Cyc_Parse_apply_tms(tq,
_tmp80,atts,d->tms);_tmp83=_tmp82.f1;_tmp84=_tmp82.f2;_tmp85=_tmp82.f3;_tmp86=
_tmp82.f4;if(_tmp81 != 0)({void*_tmp87=0;Cyc_Tcutil_warn(loc,({const char*_tmp88="nested type declaration within function prototype";
_tag_dyneither(_tmp88,sizeof(char),50);}),_tag_dyneither(_tmp87,sizeof(void*),0));});
if(_tmp85 != 0)({void*_tmp89=0;Cyc_Tcutil_warn(loc,({const char*_tmp8A="bad type params, ignoring";
_tag_dyneither(_tmp8A,sizeof(char),26);}),_tag_dyneither(_tmp89,sizeof(void*),0));});{
void*_tmp8B=_tmp84;struct Cyc_Absyn_FnInfo _tmp8D;struct Cyc_List_List*_tmp8E;
struct Cyc_Core_Opt*_tmp8F;void*_tmp90;struct Cyc_List_List*_tmp91;int _tmp92;
struct Cyc_Absyn_VarargInfo*_tmp93;struct Cyc_List_List*_tmp94;struct Cyc_List_List*
_tmp95;_LL4A: {struct Cyc_Absyn_FnType_struct*_tmp8C=(struct Cyc_Absyn_FnType_struct*)
_tmp8B;if(_tmp8C->tag != 10)goto _LL4C;else{_tmp8D=_tmp8C->f1;_tmp8E=_tmp8D.tvars;
_tmp8F=_tmp8D.effect;_tmp90=_tmp8D.ret_typ;_tmp91=_tmp8D.args;_tmp92=_tmp8D.c_varargs;
_tmp93=_tmp8D.cyc_varargs;_tmp94=_tmp8D.rgn_po;_tmp95=_tmp8D.attributes;}}_LL4B: {
struct Cyc_List_List*_tmp96=0;{struct Cyc_List_List*_tmp97=_tmp91;for(0;_tmp97 != 0;
_tmp97=_tmp97->tl){struct _tuple8 _tmp99;struct Cyc_Core_Opt*_tmp9A;struct Cyc_Absyn_Tqual
_tmp9B;void*_tmp9C;struct _tuple8*_tmp98=(struct _tuple8*)_tmp97->hd;_tmp99=*
_tmp98;_tmp9A=_tmp99.f1;_tmp9B=_tmp99.f2;_tmp9C=_tmp99.f3;if(_tmp9A == 0){Cyc_Parse_err(({
const char*_tmp9D="missing argument variable in function prototype";
_tag_dyneither(_tmp9D,sizeof(char),48);}),loc);_tmp96=({struct Cyc_List_List*
_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->hd=({struct _tuple17*_tmp9F=_cycalloc(
sizeof(*_tmp9F));_tmp9F->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),"?",sizeof(char),2);_tmp9F->f2=_tmp9B;_tmp9F->f3=_tmp9C;_tmp9F;});
_tmp9E->tl=_tmp96;_tmp9E;});}else{_tmp96=({struct Cyc_List_List*_tmpA1=_cycalloc(
sizeof(*_tmpA1));_tmpA1->hd=({struct _tuple17*_tmpA2=_cycalloc(sizeof(*_tmpA2));
_tmpA2->f1=(struct _dyneither_ptr*)_tmp9A->v;_tmpA2->f2=_tmp9B;_tmpA2->f3=_tmp9C;
_tmpA2;});_tmpA1->tl=_tmp96;_tmpA1;});}}}return({struct Cyc_Absyn_Fndecl*_tmpA3=
_cycalloc(sizeof(*_tmpA3));_tmpA3->sc=sc;_tmpA3->name=d->id;_tmpA3->tvs=_tmp8E;
_tmpA3->is_inline=is_inline;_tmpA3->effect=_tmp8F;_tmpA3->ret_type=_tmp90;_tmpA3->args=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp96);_tmpA3->c_varargs=
_tmp92;_tmpA3->cyc_varargs=_tmp93;_tmpA3->rgn_po=_tmp94;_tmpA3->body=body;_tmpA3->cached_typ=
0;_tmpA3->param_vardecls=0;_tmpA3->fn_vardecl=0;_tmpA3->attributes=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp95,_tmp86);
_tmpA3;});}_LL4C:;_LL4D:({void*_tmpA4=0;((int(*)(struct Cyc_Position_Segment*sg,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*
_tmpA5="declarator is not a function prototype";_tag_dyneither(_tmpA5,sizeof(
char),39);}),_tag_dyneither(_tmpA4,sizeof(void*),0));});_LL49:;}}}}}static char
_tmpA6[52]="at most one type may appear within a type specifier";static struct
_dyneither_ptr Cyc_Parse_msg1={_tmpA6,_tmpA6,_tmpA6 + 52};static char _tmpA7[63]="const or volatile may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg2={_tmpA7,_tmpA7,_tmpA7 + 63};static char
_tmpA8[50]="type specifier includes more than one declaration";static struct
_dyneither_ptr Cyc_Parse_msg3={_tmpA8,_tmpA8,_tmpA8 + 50};static char _tmpA9[60]="sign specifier may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg4={_tmpA9,_tmpA9,_tmpA9 + 60};static struct
_tuple11 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){struct Cyc_Core_Opt*declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;
void*t=(void*)& Cyc_Absyn_VoidType_val;enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;
enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;struct Cyc_Position_Segment*last_loc=loc;
for(0;ts != 0;ts=ts->tl){void*_tmpAA=(void*)ts->hd;void*_tmpAC;struct Cyc_Position_Segment*
_tmpAD;struct Cyc_Position_Segment*_tmpAF;struct Cyc_Position_Segment*_tmpB1;
struct Cyc_Position_Segment*_tmpB3;struct Cyc_Position_Segment*_tmpB5;struct Cyc_Absyn_Decl*
_tmpB7;_LL4F: {struct Cyc_Parse_Type_spec_struct*_tmpAB=(struct Cyc_Parse_Type_spec_struct*)
_tmpAA;if(_tmpAB->tag != 4)goto _LL51;else{_tmpAC=(void*)_tmpAB->f1;_tmpAD=_tmpAB->f2;}}
_LL50: if(seen_type)Cyc_Parse_err(Cyc_Parse_msg1,_tmpAD);last_loc=_tmpAD;
seen_type=1;t=_tmpAC;goto _LL4E;_LL51: {struct Cyc_Parse_Signed_spec_struct*_tmpAE=(
struct Cyc_Parse_Signed_spec_struct*)_tmpAA;if(_tmpAE->tag != 0)goto _LL53;else{
_tmpAF=_tmpAE->f1;}}_LL52: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmpAF);if(
seen_type)Cyc_Parse_err(({const char*_tmpB8="signed qualifier must come before type";
_tag_dyneither(_tmpB8,sizeof(char),39);}),_tmpAF);last_loc=_tmpAF;seen_sign=1;
sgn=Cyc_Absyn_Signed;goto _LL4E;_LL53: {struct Cyc_Parse_Unsigned_spec_struct*
_tmpB0=(struct Cyc_Parse_Unsigned_spec_struct*)_tmpAA;if(_tmpB0->tag != 1)goto
_LL55;else{_tmpB1=_tmpB0->f1;}}_LL54: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,
_tmpB1);if(seen_type)Cyc_Parse_err(({const char*_tmpB9="signed qualifier must come before type";
_tag_dyneither(_tmpB9,sizeof(char),39);}),_tmpB1);last_loc=_tmpB1;seen_sign=1;
sgn=Cyc_Absyn_Unsigned;goto _LL4E;_LL55: {struct Cyc_Parse_Short_spec_struct*
_tmpB2=(struct Cyc_Parse_Short_spec_struct*)_tmpAA;if(_tmpB2->tag != 2)goto _LL57;
else{_tmpB3=_tmpB2->f1;}}_LL56: if(seen_size)Cyc_Parse_err(({const char*_tmpBA="integral size may appear only once within a type specifier";
_tag_dyneither(_tmpBA,sizeof(char),59);}),_tmpB3);if(seen_type)Cyc_Parse_err(({
const char*_tmpBB="short modifier must come before base type";_tag_dyneither(
_tmpBB,sizeof(char),42);}),_tmpB3);last_loc=_tmpB3;seen_size=1;sz=Cyc_Absyn_Short_sz;
goto _LL4E;_LL57: {struct Cyc_Parse_Long_spec_struct*_tmpB4=(struct Cyc_Parse_Long_spec_struct*)
_tmpAA;if(_tmpB4->tag != 3)goto _LL59;else{_tmpB5=_tmpB4->f1;}}_LL58: if(seen_type)
Cyc_Parse_err(({const char*_tmpBC="long modifier must come before base type";
_tag_dyneither(_tmpBC,sizeof(char),41);}),_tmpB5);if(seen_size)switch(sz){case
Cyc_Absyn_Long_sz: _LL5B: sz=Cyc_Absyn_LongLong_sz;break;default: _LL5C: Cyc_Parse_err(({
const char*_tmpBD="extra long in type specifier";_tag_dyneither(_tmpBD,sizeof(
char),29);}),_tmpB5);break;}else{sz=Cyc_Absyn_Long_sz;}last_loc=_tmpB5;seen_size=
1;goto _LL4E;_LL59: {struct Cyc_Parse_Decl_spec_struct*_tmpB6=(struct Cyc_Parse_Decl_spec_struct*)
_tmpAA;if(_tmpB6->tag != 5)goto _LL4E;else{_tmpB7=_tmpB6->f1;}}_LL5A: last_loc=
_tmpB7->loc;if(declopt == 0  && !seen_type){seen_type=1;{void*_tmpBE=_tmpB7->r;
struct Cyc_Absyn_Aggrdecl*_tmpC0;struct Cyc_Absyn_Datatypedecl*_tmpC2;struct Cyc_Absyn_Enumdecl*
_tmpC4;_LL5F: {struct Cyc_Absyn_Aggr_d_struct*_tmpBF=(struct Cyc_Absyn_Aggr_d_struct*)
_tmpBE;if(_tmpBF->tag != 6)goto _LL61;else{_tmpC0=_tmpBF->f1;}}_LL60: {struct Cyc_List_List*
_tmpC5=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmpC0->tvs));t=(void*)({struct Cyc_Absyn_AggrType_struct*_tmpC6=_cycalloc(
sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_AggrType_struct _tmpC7;_tmpC7.tag=12;
_tmpC7.f1=({struct Cyc_Absyn_AggrInfo _tmpC8;_tmpC8.aggr_info=Cyc_Absyn_UnknownAggr(
_tmpC0->kind,_tmpC0->name,0);_tmpC8.targs=_tmpC5;_tmpC8;});_tmpC7;});_tmpC6;});
if(_tmpC0->impl != 0)declopt=({struct Cyc_Core_Opt*_tmpC9=_cycalloc(sizeof(*_tmpC9));
_tmpC9->v=_tmpB7;_tmpC9;});goto _LL5E;}_LL61: {struct Cyc_Absyn_Datatype_d_struct*
_tmpC1=(struct Cyc_Absyn_Datatype_d_struct*)_tmpBE;if(_tmpC1->tag != 7)goto _LL63;
else{_tmpC2=_tmpC1->f1;}}_LL62: {struct Cyc_List_List*_tmpCA=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmpC2->tvs));t=(void*)({
struct Cyc_Absyn_DatatypeType_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({
struct Cyc_Absyn_DatatypeType_struct _tmpCC;_tmpCC.tag=3;_tmpCC.f1=({struct Cyc_Absyn_DatatypeInfo
_tmpCD;_tmpCD.datatype_info=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**
_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=_tmpC2;_tmpCE;}));_tmpCD.targs=
_tmpCA;_tmpCD;});_tmpCC;});_tmpCB;});if(_tmpC2->fields != 0)declopt=({struct Cyc_Core_Opt*
_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->v=_tmpB7;_tmpCF;});goto _LL5E;}_LL63: {
struct Cyc_Absyn_Enum_d_struct*_tmpC3=(struct Cyc_Absyn_Enum_d_struct*)_tmpBE;if(
_tmpC3->tag != 8)goto _LL65;else{_tmpC4=_tmpC3->f1;}}_LL64: t=(void*)({struct Cyc_Absyn_EnumType_struct*
_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=({struct Cyc_Absyn_EnumType_struct
_tmpD1;_tmpD1.tag=14;_tmpD1.f1=_tmpC4->name;_tmpD1.f2=0;_tmpD1;});_tmpD0;});
declopt=({struct Cyc_Core_Opt*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->v=_tmpB7;
_tmpD2;});goto _LL5E;_LL65:;_LL66:({void*_tmpD3=0;((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpB7->loc,({
const char*_tmpD4="bad declaration within type specifier";_tag_dyneither(_tmpD4,
sizeof(char),38);}),_tag_dyneither(_tmpD3,sizeof(void*),0));});_LL5E:;}}else{Cyc_Parse_err(
Cyc_Parse_msg3,_tmpB7->loc);}goto _LL4E;_LL4E:;}if(!seen_type){if(!seen_sign  && !
seen_size)({void*_tmpD5=0;Cyc_Tcutil_warn(last_loc,({const char*_tmpD6="missing type within specifier";
_tag_dyneither(_tmpD6,sizeof(char),30);}),_tag_dyneither(_tmpD5,sizeof(void*),0));});
t=Cyc_Absyn_int_typ(sgn,sz);}else{if(seen_sign){void*_tmpD7=t;enum Cyc_Absyn_Sign
_tmpD9;enum Cyc_Absyn_Size_of _tmpDA;_LL68: {struct Cyc_Absyn_IntType_struct*
_tmpD8=(struct Cyc_Absyn_IntType_struct*)_tmpD7;if(_tmpD8->tag != 6)goto _LL6A;
else{_tmpD9=_tmpD8->f1;_tmpDA=_tmpD8->f2;}}_LL69: if(_tmpD9 != sgn)t=Cyc_Absyn_int_typ(
sgn,_tmpDA);goto _LL67;_LL6A:;_LL6B: Cyc_Parse_err(({const char*_tmpDB="sign specification on non-integral type";
_tag_dyneither(_tmpDB,sizeof(char),40);}),last_loc);goto _LL67;_LL67:;}if(
seen_size){void*_tmpDC=t;enum Cyc_Absyn_Sign _tmpDE;enum Cyc_Absyn_Size_of _tmpDF;
_LL6D: {struct Cyc_Absyn_IntType_struct*_tmpDD=(struct Cyc_Absyn_IntType_struct*)
_tmpDC;if(_tmpDD->tag != 6)goto _LL6F;else{_tmpDE=_tmpDD->f1;_tmpDF=_tmpDD->f2;}}
_LL6E: if(_tmpDF != sz)t=Cyc_Absyn_int_typ(_tmpDE,sz);goto _LL6C;_LL6F: {struct Cyc_Absyn_DoubleType_struct*
_tmpE0=(struct Cyc_Absyn_DoubleType_struct*)_tmpDC;if(_tmpE0->tag != 8)goto _LL71;}
_LL70: t=Cyc_Absyn_double_typ(1);goto _LL6C;_LL71:;_LL72: Cyc_Parse_err(({const char*
_tmpE1="size qualifier on non-integral type";_tag_dyneither(_tmpE1,sizeof(char),
36);}),last_loc);goto _LL6C;_LL6C:;}}return({struct _tuple11 _tmpE2;_tmpE2.f1=t;
_tmpE2.f2=declopt;_tmpE2;});}static struct Cyc_List_List*Cyc_Parse_apply_tmss(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*ds,
struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{struct Cyc_Parse_Declarator*
_tmpE3=(struct Cyc_Parse_Declarator*)ds->hd;struct _tuple1*_tmpE4=_tmpE3->id;
struct Cyc_Absyn_Tqual _tmpE6;void*_tmpE7;struct Cyc_List_List*_tmpE8;struct Cyc_List_List*
_tmpE9;struct _tuple12 _tmpE5=Cyc_Parse_apply_tms(tq,t,shared_atts,_tmpE3->tms);
_tmpE6=_tmpE5.f1;_tmpE7=_tmpE5.f2;_tmpE8=_tmpE5.f3;_tmpE9=_tmpE5.f4;if(ds->tl == 
0)return({struct Cyc_List_List*_tmpEA=_region_malloc(r,sizeof(*_tmpEA));_tmpEA->hd=({
struct _tuple14*_tmpEB=_region_malloc(r,sizeof(*_tmpEB));_tmpEB->f1=_tmpE4;_tmpEB->f2=
_tmpE6;_tmpEB->f3=_tmpE7;_tmpEB->f4=_tmpE8;_tmpEB->f5=_tmpE9;_tmpEB;});_tmpEA->tl=
0;_tmpEA;});else{return({struct Cyc_List_List*_tmpEC=_region_malloc(r,sizeof(*
_tmpEC));_tmpEC->hd=({struct _tuple14*_tmpED=_region_malloc(r,sizeof(*_tmpED));
_tmpED->f1=_tmpE4;_tmpED->f2=_tmpE6;_tmpED->f3=_tmpE7;_tmpED->f4=_tmpE8;_tmpED->f5=
_tmpE9;_tmpED;});_tmpEC->tl=Cyc_Parse_apply_tmss(r,_tmpE6,Cyc_Tcutil_copy_type(t),
ds->tl,shared_atts);_tmpEC;});}}}static struct _tuple12 Cyc_Parse_apply_tms(struct
Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){if(
tms == 0)return({struct _tuple12 _tmpEE;_tmpEE.f1=tq;_tmpEE.f2=t;_tmpEE.f3=0;_tmpEE.f4=
atts;_tmpEE;});{void*_tmpEF=(void*)tms->hd;union Cyc_Absyn_Constraint*_tmpF1;
struct Cyc_Position_Segment*_tmpF2;struct Cyc_Absyn_Exp*_tmpF4;union Cyc_Absyn_Constraint*
_tmpF5;struct Cyc_Position_Segment*_tmpF6;void*_tmpF8;struct Cyc_List_List*_tmpFA;
struct Cyc_Position_Segment*_tmpFB;struct Cyc_Absyn_PtrAtts _tmpFD;struct Cyc_Absyn_Tqual
_tmpFE;struct Cyc_Position_Segment*_tmp100;struct Cyc_List_List*_tmp101;_LL74: {
struct Cyc_Absyn_Carray_mod_struct*_tmpF0=(struct Cyc_Absyn_Carray_mod_struct*)
_tmpEF;if(_tmpF0->tag != 0)goto _LL76;else{_tmpF1=_tmpF0->f1;_tmpF2=_tmpF0->f2;}}
_LL75: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,
tq,0,_tmpF1,_tmpF2),atts,tms->tl);_LL76: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmpF3=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmpEF;if(_tmpF3->tag != 1)goto
_LL78;else{_tmpF4=_tmpF3->f1;_tmpF5=_tmpF3->f2;_tmpF6=_tmpF3->f3;}}_LL77: return
Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)
_tmpF4,_tmpF5,_tmpF6),atts,tms->tl);_LL78: {struct Cyc_Absyn_Function_mod_struct*
_tmpF7=(struct Cyc_Absyn_Function_mod_struct*)_tmpEF;if(_tmpF7->tag != 3)goto _LL7A;
else{_tmpF8=(void*)_tmpF7->f1;}}_LL79: {void*_tmp102=_tmpF8;struct Cyc_List_List*
_tmp104;int _tmp105;struct Cyc_Absyn_VarargInfo*_tmp106;struct Cyc_Core_Opt*_tmp107;
struct Cyc_List_List*_tmp108;struct Cyc_Position_Segment*_tmp10A;_LL81: {struct Cyc_Absyn_WithTypes_struct*
_tmp103=(struct Cyc_Absyn_WithTypes_struct*)_tmp102;if(_tmp103->tag != 1)goto _LL83;
else{_tmp104=_tmp103->f1;_tmp105=_tmp103->f2;_tmp106=_tmp103->f3;_tmp107=_tmp103->f4;
_tmp108=_tmp103->f5;}}_LL82: {struct Cyc_List_List*typvars=0;struct Cyc_List_List*
fn_atts=0;struct Cyc_List_List*new_atts=0;{struct Cyc_List_List*as=atts;for(0;as != 
0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd))fn_atts=({struct Cyc_List_List*
_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->hd=(void*)as->hd;_tmp10B->tl=
fn_atts;_tmp10B;});else{new_atts=({struct Cyc_List_List*_tmp10C=_cycalloc(sizeof(*
_tmp10C));_tmp10C->hd=(void*)as->hd;_tmp10C->tl=new_atts;_tmp10C;});}}}if(tms->tl
!= 0){void*_tmp10D=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;
struct Cyc_List_List*_tmp10F;_LL86: {struct Cyc_Absyn_TypeParams_mod_struct*
_tmp10E=(struct Cyc_Absyn_TypeParams_mod_struct*)_tmp10D;if(_tmp10E->tag != 4)goto
_LL88;else{_tmp10F=_tmp10E->f1;}}_LL87: typvars=_tmp10F;tms=tms->tl;goto _LL85;
_LL88:;_LL89: goto _LL85;_LL85:;}if(((((!_tmp105  && _tmp106 == 0) && _tmp104 != 0)
 && _tmp104->tl == 0) && (*((struct _tuple8*)_tmp104->hd)).f1 == 0) && (*((struct
_tuple8*)_tmp104->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)_tmp104=0;t=Cyc_Parse_array2ptr(
t,0);((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Parse_arg_array2ptr,_tmp104);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(
tq.loc),Cyc_Absyn_function_typ(typvars,_tmp107,t,_tmp104,_tmp105,_tmp106,_tmp108,
fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}_LL83: {struct
Cyc_Absyn_NoTypes_struct*_tmp109=(struct Cyc_Absyn_NoTypes_struct*)_tmp102;if(
_tmp109->tag != 0)goto _LL80;else{_tmp10A=_tmp109->f2;}}_LL84:({void*_tmp110=0;((
int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(_tmp10A,({const char*_tmp111="function declaration without parameter types";
_tag_dyneither(_tmp111,sizeof(char),45);}),_tag_dyneither(_tmp110,sizeof(void*),
0));});_LL80:;}_LL7A: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpF9=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpEF;if(_tmpF9->tag != 4)goto _LL7C;else{_tmpFA=_tmpF9->f1;_tmpFB=_tmpF9->f2;}}
_LL7B: if(tms->tl == 0)return({struct _tuple12 _tmp112;_tmp112.f1=tq;_tmp112.f2=t;
_tmp112.f3=_tmpFA;_tmp112.f4=atts;_tmp112;});({void*_tmp113=0;((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpFB,({
const char*_tmp114="type parameters must appear before function arguments in declarator";
_tag_dyneither(_tmp114,sizeof(char),68);}),_tag_dyneither(_tmp113,sizeof(void*),
0));});_LL7C: {struct Cyc_Absyn_Pointer_mod_struct*_tmpFC=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpEF;if(_tmpFC->tag != 2)goto _LL7E;else{_tmpFD=_tmpFC->f1;_tmpFE=_tmpFC->f2;}}
_LL7D: return Cyc_Parse_apply_tms(_tmpFE,(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115[0]=({struct Cyc_Absyn_PointerType_struct
_tmp116;_tmp116.tag=5;_tmp116.f1=({struct Cyc_Absyn_PtrInfo _tmp117;_tmp117.elt_typ=
t;_tmp117.elt_tq=tq;_tmp117.ptr_atts=_tmpFD;_tmp117;});_tmp116;});_tmp115;}),
atts,tms->tl);_LL7E: {struct Cyc_Absyn_Attributes_mod_struct*_tmpFF=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpEF;if(_tmpFF->tag != 5)goto _LL73;else{_tmp100=_tmpFF->f1;_tmp101=_tmpFF->f2;}}
_LL7F: return Cyc_Parse_apply_tms(tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp101),tms->tl);_LL73:;}}void*
Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*loc){
void*_tmp119;struct Cyc_Core_Opt*_tmp11A;struct _tuple11 _tmp118=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp119=_tmp118.f1;_tmp11A=_tmp118.f2;if(_tmp11A != 0)({void*_tmp11B=0;
Cyc_Tcutil_warn(loc,({const char*_tmp11C="ignoring nested type declaration(s) in specifier list";
_tag_dyneither(_tmp11C,sizeof(char),54);}),_tag_dyneither(_tmp11B,sizeof(void*),
0));});return _tmp119;}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment*loc,struct _tuple14*t){struct _tuple14 _tmp11E;struct
_tuple1*_tmp11F;struct Cyc_Absyn_Tqual _tmp120;void*_tmp121;struct Cyc_List_List*
_tmp122;struct Cyc_List_List*_tmp123;struct _tuple14*_tmp11D=t;_tmp11E=*_tmp11D;
_tmp11F=_tmp11E.f1;_tmp120=_tmp11E.f2;_tmp121=_tmp11E.f3;_tmp122=_tmp11E.f4;
_tmp123=_tmp11E.f5;Cyc_Lex_register_typedef(_tmp11F);{struct Cyc_Core_Opt*kind;
struct Cyc_Core_Opt*type;{void*_tmp124=_tmp121;struct Cyc_Core_Opt*_tmp126;_LL8B: {
struct Cyc_Absyn_Evar_struct*_tmp125=(struct Cyc_Absyn_Evar_struct*)_tmp124;if(
_tmp125->tag != 1)goto _LL8D;else{_tmp126=_tmp125->f1;}}_LL8C: type=0;if(_tmp126 == 
0)kind=(struct Cyc_Core_Opt*)& Cyc_Tcutil_bk;else{kind=_tmp126;}goto _LL8A;_LL8D:;
_LL8E: kind=0;type=({struct Cyc_Core_Opt*_tmp127=_cycalloc(sizeof(*_tmp127));
_tmp127->v=_tmp121;_tmp127;});goto _LL8A;_LL8A:;}return Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Typedef_d_struct*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128[0]=({
struct Cyc_Absyn_Typedef_d_struct _tmp129;_tmp129.tag=9;_tmp129.f1=({struct Cyc_Absyn_Typedefdecl*
_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->name=_tmp11F;_tmp12A->tvs=_tmp122;
_tmp12A->kind=kind;_tmp12A->defn=type;_tmp12A->atts=_tmp123;_tmp12A->tq=_tmp120;
_tmp12A;});_tmp129;});_tmp128;}),loc);}}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({
struct Cyc_Absyn_Decl_s_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B[0]=({
struct Cyc_Absyn_Decl_s_struct _tmp12C;_tmp12C.tag=12;_tmp12C.f1=d;_tmp12C.f2=s;
_tmp12C;});_tmp12B;}),d->loc);}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc){struct _RegionHandle _tmp12D=_new_region("mkrgn");struct _RegionHandle*mkrgn=&
_tmp12D;_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp12F;struct Cyc_Absyn_Tqual
_tmp130;struct Cyc_List_List*_tmp131;struct Cyc_List_List*_tmp132;struct Cyc_Parse_Declaration_spec*
_tmp12E=ds;_tmp12F=*_tmp12E;_tmp130=_tmp12F.tq;_tmp131=_tmp12F.type_specs;
_tmp132=_tmp12F.attributes;if(_tmp130.loc == 0)_tmp130.loc=tqual_loc;if(ds->is_inline)
Cyc_Parse_err(({const char*_tmp133="inline is allowed only on function definitions";
_tag_dyneither(_tmp133,sizeof(char),47);}),loc);if(_tmp131 == 0){Cyc_Parse_err(({
const char*_tmp134="missing type specifiers in declaration";_tag_dyneither(
_tmp134,sizeof(char),39);}),loc);{struct Cyc_List_List*_tmp135=0;_npop_handler(0);
return _tmp135;}}{enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;int istypedef=0;if(ds->sc
!= 0)switch((enum Cyc_Parse_Storage_class)((struct Cyc_Core_Opt*)_check_null(ds->sc))->v){
case Cyc_Parse_Typedef_sc: _LL8F: istypedef=1;break;case Cyc_Parse_Extern_sc: _LL90: s=
Cyc_Absyn_Extern;break;case Cyc_Parse_ExternC_sc: _LL91: s=Cyc_Absyn_ExternC;break;
case Cyc_Parse_Static_sc: _LL92: s=Cyc_Absyn_Static;break;case Cyc_Parse_Auto_sc:
_LL93: s=Cyc_Absyn_Public;break;case Cyc_Parse_Register_sc: _LL94: if(Cyc_Parse_no_register)
s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}break;case Cyc_Parse_Abstract_sc:
_LL95: s=Cyc_Absyn_Abstract;break;}{struct Cyc_List_List*_tmp137;struct Cyc_List_List*
_tmp138;struct _tuple0 _tmp136=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(mkrgn,mkrgn,ids);
_tmp137=_tmp136.f1;_tmp138=_tmp136.f2;{int exps_empty=1;{struct Cyc_List_List*es=
_tmp138;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd != 0){exps_empty=
0;break;}}}{struct _tuple11 _tmp139=Cyc_Parse_collapse_type_specifiers(_tmp131,loc);
if(_tmp137 == 0){void*_tmp13B;struct Cyc_Core_Opt*_tmp13C;struct _tuple11 _tmp13A=
_tmp139;_tmp13B=_tmp13A.f1;_tmp13C=_tmp13A.f2;if(_tmp13C != 0){struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)_tmp13C->v;{void*_tmp13D=d->r;struct Cyc_Absyn_Enumdecl*
_tmp13F;struct Cyc_Absyn_Aggrdecl*_tmp141;struct Cyc_Absyn_Datatypedecl*_tmp143;
_LL98: {struct Cyc_Absyn_Enum_d_struct*_tmp13E=(struct Cyc_Absyn_Enum_d_struct*)
_tmp13D;if(_tmp13E->tag != 8)goto _LL9A;else{_tmp13F=_tmp13E->f1;}}_LL99: _tmp13F->sc=
s;if(_tmp132 != 0)Cyc_Parse_err(({const char*_tmp144="bad attributes on enum";
_tag_dyneither(_tmp144,sizeof(char),23);}),loc);goto _LL97;_LL9A: {struct Cyc_Absyn_Aggr_d_struct*
_tmp140=(struct Cyc_Absyn_Aggr_d_struct*)_tmp13D;if(_tmp140->tag != 6)goto _LL9C;
else{_tmp141=_tmp140->f1;}}_LL9B: _tmp141->sc=s;_tmp141->attributes=_tmp132;goto
_LL97;_LL9C: {struct Cyc_Absyn_Datatype_d_struct*_tmp142=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp13D;if(_tmp142->tag != 7)goto _LL9E;else{_tmp143=_tmp142->f1;}}_LL9D: _tmp143->sc=
s;if(_tmp132 != 0)Cyc_Parse_err(({const char*_tmp145="bad attributes on datatype";
_tag_dyneither(_tmp145,sizeof(char),27);}),loc);goto _LL97;_LL9E:;_LL9F: Cyc_Parse_err(({
const char*_tmp146="bad declaration";_tag_dyneither(_tmp146,sizeof(char),16);}),
loc);{struct Cyc_List_List*_tmp147=0;_npop_handler(0);return _tmp147;}_LL97:;}{
struct Cyc_List_List*_tmp149=({struct Cyc_List_List*_tmp148=_cycalloc(sizeof(*
_tmp148));_tmp148->hd=d;_tmp148->tl=0;_tmp148;});_npop_handler(0);return _tmp149;}}
else{void*_tmp14A=_tmp13B;struct Cyc_Absyn_AggrInfo _tmp14C;union Cyc_Absyn_AggrInfoU
_tmp14D;struct _tuple3 _tmp14E;enum Cyc_Absyn_AggrKind _tmp14F;struct _tuple1*
_tmp150;struct Cyc_List_List*_tmp151;struct Cyc_Absyn_DatatypeInfo _tmp153;union Cyc_Absyn_DatatypeInfoU
_tmp154;struct Cyc_Absyn_Datatypedecl**_tmp155;struct Cyc_Absyn_DatatypeInfo
_tmp157;union Cyc_Absyn_DatatypeInfoU _tmp158;struct Cyc_Absyn_UnknownDatatypeInfo
_tmp159;struct _tuple1*_tmp15A;int _tmp15B;struct Cyc_List_List*_tmp15C;struct
_tuple1*_tmp15E;struct Cyc_List_List*_tmp160;_LLA1: {struct Cyc_Absyn_AggrType_struct*
_tmp14B=(struct Cyc_Absyn_AggrType_struct*)_tmp14A;if(_tmp14B->tag != 12)goto _LLA3;
else{_tmp14C=_tmp14B->f1;_tmp14D=_tmp14C.aggr_info;if((_tmp14D.UnknownAggr).tag
!= 1)goto _LLA3;_tmp14E=(struct _tuple3)(_tmp14D.UnknownAggr).val;_tmp14F=_tmp14E.f1;
_tmp150=_tmp14E.f2;_tmp151=_tmp14C.targs;}}_LLA2: {struct Cyc_List_List*_tmp161=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp151);struct Cyc_Absyn_Aggrdecl*_tmp162=({struct Cyc_Absyn_Aggrdecl*_tmp168=
_cycalloc(sizeof(*_tmp168));_tmp168->kind=_tmp14F;_tmp168->sc=s;_tmp168->name=
_tmp150;_tmp168->tvs=_tmp161;_tmp168->impl=0;_tmp168->attributes=0;_tmp168;});
if(_tmp132 != 0)Cyc_Parse_err(({const char*_tmp163="bad attributes on type declaration";
_tag_dyneither(_tmp163,sizeof(char),35);}),loc);{struct Cyc_List_List*_tmp167=({
struct Cyc_List_List*_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp165=_cycalloc(sizeof(*_tmp165));
_tmp165[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp166;_tmp166.tag=6;_tmp166.f1=
_tmp162;_tmp166;});_tmp165;}),loc);_tmp164->tl=0;_tmp164;});_npop_handler(0);
return _tmp167;}}_LLA3: {struct Cyc_Absyn_DatatypeType_struct*_tmp152=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp14A;if(_tmp152->tag != 3)goto _LLA5;else{_tmp153=_tmp152->f1;_tmp154=_tmp153.datatype_info;
if((_tmp154.KnownDatatype).tag != 2)goto _LLA5;_tmp155=(struct Cyc_Absyn_Datatypedecl**)(
_tmp154.KnownDatatype).val;}}_LLA4: if(_tmp132 != 0)Cyc_Parse_err(({const char*
_tmp169="bad attributes on datatype";_tag_dyneither(_tmp169,sizeof(char),27);}),
loc);{struct Cyc_List_List*_tmp16D=({struct Cyc_List_List*_tmp16A=_cycalloc(
sizeof(*_tmp16A));_tmp16A->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_struct*
_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B[0]=({struct Cyc_Absyn_Datatype_d_struct
_tmp16C;_tmp16C.tag=7;_tmp16C.f1=*_tmp155;_tmp16C;});_tmp16B;}),loc);_tmp16A->tl=
0;_tmp16A;});_npop_handler(0);return _tmp16D;}_LLA5: {struct Cyc_Absyn_DatatypeType_struct*
_tmp156=(struct Cyc_Absyn_DatatypeType_struct*)_tmp14A;if(_tmp156->tag != 3)goto
_LLA7;else{_tmp157=_tmp156->f1;_tmp158=_tmp157.datatype_info;if((_tmp158.UnknownDatatype).tag
!= 1)goto _LLA7;_tmp159=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp158.UnknownDatatype).val;
_tmp15A=_tmp159.name;_tmp15B=_tmp159.is_extensible;_tmp15C=_tmp157.targs;}}_LLA6: {
struct Cyc_List_List*_tmp16E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp15C);struct Cyc_Absyn_Decl*_tmp16F=
Cyc_Absyn_datatype_decl(s,_tmp15A,_tmp16E,0,_tmp15B,loc);if(_tmp132 != 0)Cyc_Parse_err(({
const char*_tmp170="bad attributes on datatype";_tag_dyneither(_tmp170,sizeof(
char),27);}),loc);{struct Cyc_List_List*_tmp172=({struct Cyc_List_List*_tmp171=
_cycalloc(sizeof(*_tmp171));_tmp171->hd=_tmp16F;_tmp171->tl=0;_tmp171;});
_npop_handler(0);return _tmp172;}}_LLA7: {struct Cyc_Absyn_EnumType_struct*_tmp15D=(
struct Cyc_Absyn_EnumType_struct*)_tmp14A;if(_tmp15D->tag != 14)goto _LLA9;else{
_tmp15E=_tmp15D->f1;}}_LLA8: {struct Cyc_Absyn_Enumdecl*_tmp173=({struct Cyc_Absyn_Enumdecl*
_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A->sc=s;_tmp17A->name=_tmp15E;_tmp17A->fields=
0;_tmp17A;});if(_tmp132 != 0)Cyc_Parse_err(({const char*_tmp174="bad attributes on enum";
_tag_dyneither(_tmp174,sizeof(char),23);}),loc);{struct Cyc_List_List*_tmp179=({
struct Cyc_List_List*_tmp175=_cycalloc(sizeof(*_tmp175));_tmp175->hd=({struct Cyc_Absyn_Decl*
_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->r=(void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp178;_tmp178.tag=8;_tmp178.f1=_tmp173;_tmp178;});_tmp177;});_tmp176->loc=loc;
_tmp176;});_tmp175->tl=0;_tmp175;});_npop_handler(0);return _tmp179;}}_LLA9: {
struct Cyc_Absyn_AnonEnumType_struct*_tmp15F=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp14A;if(_tmp15F->tag != 15)goto _LLAB;else{_tmp160=_tmp15F->f1;}}_LLAA: {struct
Cyc_Absyn_Enumdecl*_tmp17B=({struct Cyc_Absyn_Enumdecl*_tmp182=_cycalloc(sizeof(*
_tmp182));_tmp182->sc=s;_tmp182->name=Cyc_Parse_gensym_enum();_tmp182->fields=({
struct Cyc_Core_Opt*_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183->v=_tmp160;
_tmp183;});_tmp182;});if(_tmp132 != 0)Cyc_Parse_err(({const char*_tmp17C="bad attributes on enum";
_tag_dyneither(_tmp17C,sizeof(char),23);}),loc);{struct Cyc_List_List*_tmp181=({
struct Cyc_List_List*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D->hd=({struct Cyc_Absyn_Decl*
_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E->r=(void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp180;_tmp180.tag=8;_tmp180.f1=_tmp17B;_tmp180;});_tmp17F;});_tmp17E->loc=loc;
_tmp17E;});_tmp17D->tl=0;_tmp17D;});_npop_handler(0);return _tmp181;}}_LLAB:;
_LLAC: Cyc_Parse_err(({const char*_tmp184="missing declarator";_tag_dyneither(
_tmp184,sizeof(char),19);}),loc);{struct Cyc_List_List*_tmp185=0;_npop_handler(0);
return _tmp185;}_LLA0:;}}else{void*t=_tmp139.f1;struct Cyc_List_List*_tmp186=Cyc_Parse_apply_tmss(
mkrgn,_tmp130,t,_tmp137,_tmp132);if(istypedef){if(!exps_empty)Cyc_Parse_err(({
const char*_tmp187="initializer in typedef declaration";_tag_dyneither(_tmp187,
sizeof(char),35);}),loc);{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple14*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp186);if(_tmp139.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(
_tmp139.f2)->v;{void*_tmp188=d->r;struct Cyc_Absyn_Aggrdecl*_tmp18A;struct Cyc_Absyn_Datatypedecl*
_tmp18C;struct Cyc_Absyn_Enumdecl*_tmp18E;_LLAE: {struct Cyc_Absyn_Aggr_d_struct*
_tmp189=(struct Cyc_Absyn_Aggr_d_struct*)_tmp188;if(_tmp189->tag != 6)goto _LLB0;
else{_tmp18A=_tmp189->f1;}}_LLAF: _tmp18A->sc=s;_tmp18A->attributes=_tmp132;
_tmp132=0;goto _LLAD;_LLB0: {struct Cyc_Absyn_Datatype_d_struct*_tmp18B=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp188;if(_tmp18B->tag != 7)goto _LLB2;else{_tmp18C=_tmp18B->f1;}}_LLB1: _tmp18C->sc=
s;goto _LLAD;_LLB2: {struct Cyc_Absyn_Enum_d_struct*_tmp18D=(struct Cyc_Absyn_Enum_d_struct*)
_tmp188;if(_tmp18D->tag != 8)goto _LLB4;else{_tmp18E=_tmp18D->f1;}}_LLB3: _tmp18E->sc=
s;goto _LLAD;_LLB4:;_LLB5: Cyc_Parse_err(({const char*_tmp18F="declaration within typedef is not a struct, union, or datatype";
_tag_dyneither(_tmp18F,sizeof(char),63);}),loc);goto _LLAD;_LLAD:;}decls=({struct
Cyc_List_List*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190->hd=d;_tmp190->tl=
decls;_tmp190;});}{struct Cyc_List_List*_tmp191=decls;_npop_handler(0);return
_tmp191;}}}else{if(_tmp139.f2 != 0)Cyc_Parse_unimp(({const char*_tmp192="nested type declaration within declarator";
_tag_dyneither(_tmp192,sizeof(char),42);}),loc);{struct Cyc_List_List*decls=0;{
struct Cyc_List_List*_tmp193=_tmp186;for(0;_tmp193 != 0;(_tmp193=_tmp193->tl,
_tmp138=_tmp138->tl)){struct _tuple14 _tmp195;struct _tuple1*_tmp196;struct Cyc_Absyn_Tqual
_tmp197;void*_tmp198;struct Cyc_List_List*_tmp199;struct Cyc_List_List*_tmp19A;
struct _tuple14*_tmp194=(struct _tuple14*)_tmp193->hd;_tmp195=*_tmp194;_tmp196=
_tmp195.f1;_tmp197=_tmp195.f2;_tmp198=_tmp195.f3;_tmp199=_tmp195.f4;_tmp19A=
_tmp195.f5;if(_tmp199 != 0)({void*_tmp19B=0;Cyc_Tcutil_warn(loc,({const char*
_tmp19C="bad type params, ignoring";_tag_dyneither(_tmp19C,sizeof(char),26);}),
_tag_dyneither(_tmp19B,sizeof(void*),0));});if(_tmp138 == 0)({void*_tmp19D=0;((
int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*_tmp19E="unexpected NULL in parse!";
_tag_dyneither(_tmp19E,sizeof(char),26);}),_tag_dyneither(_tmp19D,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*_tmp19F=(struct Cyc_Absyn_Exp*)_tmp138->hd;struct Cyc_Absyn_Vardecl*
_tmp1A0=Cyc_Absyn_new_vardecl(_tmp196,_tmp198,_tmp19F);_tmp1A0->tq=_tmp197;
_tmp1A0->sc=s;_tmp1A0->attributes=_tmp19A;{struct Cyc_Absyn_Decl*_tmp1A1=({struct
Cyc_Absyn_Decl*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->r=(void*)({struct Cyc_Absyn_Var_d_struct*
_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4[0]=({struct Cyc_Absyn_Var_d_struct
_tmp1A5;_tmp1A5.tag=0;_tmp1A5.f1=_tmp1A0;_tmp1A5;});_tmp1A4;});_tmp1A3->loc=loc;
_tmp1A3;});decls=({struct Cyc_List_List*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));
_tmp1A2->hd=_tmp1A1;_tmp1A2->tl=decls;_tmp1A2;});}}}}{struct Cyc_List_List*
_tmp1A6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);
_npop_handler(0);return _tmp1A6;}}}}}}}}};_pop_region(mkrgn);}static enum Cyc_Absyn_Kind
Cyc_Parse_id_to_kind(struct _dyneither_ptr s,struct Cyc_Position_Segment*loc){if(
Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 
2)switch(*((const char*)_check_dyneither_subscript(s,sizeof(char),0))){case 'A':
_LLB6: return Cyc_Absyn_AnyKind;case 'M': _LLB7: return Cyc_Absyn_MemKind;case 'B':
_LLB8: return Cyc_Absyn_BoxKind;case 'R': _LLB9: return Cyc_Absyn_RgnKind;case 'U':
_LLBA: if(*((const char*)_check_dyneither_subscript(s,sizeof(char),1))== 'R')
return Cyc_Absyn_UniqueRgnKind;else{break;}case 'T': _LLBB: if(*((const char*)
_check_dyneither_subscript(s,sizeof(char),1))== 'R')return Cyc_Absyn_TopRgnKind;
else{break;}case 'E': _LLBC: return Cyc_Absyn_EffKind;case 'I': _LLBD: return Cyc_Absyn_IntKind;
default: _LLBE: break;}Cyc_Parse_err((struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp1AA;_tmp1AA.tag=1;_tmp1AA.f1=(unsigned long)((int)Cyc_strlen((struct
_dyneither_ptr)s));{struct Cyc_String_pa_struct _tmp1A9;_tmp1A9.tag=0;_tmp1A9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s);{void*_tmp1A7[2]={& _tmp1A9,&
_tmp1AA};Cyc_aprintf(({const char*_tmp1A8="bad kind: %s; strlen=%d";
_tag_dyneither(_tmp1A8,sizeof(char),24);}),_tag_dyneither(_tmp1A7,sizeof(void*),
2));}}}),loc);return Cyc_Absyn_BoxKind;}static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(
struct Cyc_Absyn_Pat*p){void*_tmp1AB=p->r;struct _tuple1*_tmp1AD;struct Cyc_Absyn_Vardecl*
_tmp1AF;struct Cyc_Absyn_Pat*_tmp1B0;struct Cyc_Absyn_Pat _tmp1B1;void*_tmp1B2;
struct Cyc_Absyn_Pat*_tmp1B5;enum Cyc_Absyn_Sign _tmp1B8;int _tmp1B9;char _tmp1BB;
struct _dyneither_ptr _tmp1BD;struct _tuple1*_tmp1BF;struct Cyc_List_List*_tmp1C0;
int _tmp1C1;struct Cyc_Absyn_Exp*_tmp1C3;_LLC1: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp1AC=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp1AB;if(_tmp1AC->tag != 14)goto
_LLC3;else{_tmp1AD=_tmp1AC->f1;}}_LLC2: return Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnknownId_e_struct*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));_tmp1C4[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp1C5;_tmp1C5.tag=2;_tmp1C5.f1=_tmp1AD;
_tmp1C5;});_tmp1C4;}),p->loc);_LLC3: {struct Cyc_Absyn_Reference_p_struct*_tmp1AE=(
struct Cyc_Absyn_Reference_p_struct*)_tmp1AB;if(_tmp1AE->tag != 2)goto _LLC5;else{
_tmp1AF=_tmp1AE->f1;_tmp1B0=_tmp1AE->f2;_tmp1B1=*_tmp1B0;_tmp1B2=_tmp1B1.r;{
struct Cyc_Absyn_Wild_p_struct*_tmp1B3=(struct Cyc_Absyn_Wild_p_struct*)_tmp1B2;
if(_tmp1B3->tag != 0)goto _LLC5;}}}_LLC4: return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));
_tmp1C6[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp1C7;_tmp1C7.tag=2;_tmp1C7.f1=
_tmp1AF->name;_tmp1C7;});_tmp1C6;}),p->loc),p->loc);_LLC5: {struct Cyc_Absyn_Pointer_p_struct*
_tmp1B4=(struct Cyc_Absyn_Pointer_p_struct*)_tmp1AB;if(_tmp1B4->tag != 5)goto _LLC7;
else{_tmp1B5=_tmp1B4->f1;}}_LLC6: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(
_tmp1B5),p->loc);_LLC7: {struct Cyc_Absyn_Null_p_struct*_tmp1B6=(struct Cyc_Absyn_Null_p_struct*)
_tmp1AB;if(_tmp1B6->tag != 8)goto _LLC9;}_LLC8: return Cyc_Absyn_null_exp(p->loc);
_LLC9: {struct Cyc_Absyn_Int_p_struct*_tmp1B7=(struct Cyc_Absyn_Int_p_struct*)
_tmp1AB;if(_tmp1B7->tag != 9)goto _LLCB;else{_tmp1B8=_tmp1B7->f1;_tmp1B9=_tmp1B7->f2;}}
_LLCA: return Cyc_Absyn_int_exp(_tmp1B8,_tmp1B9,p->loc);_LLCB: {struct Cyc_Absyn_Char_p_struct*
_tmp1BA=(struct Cyc_Absyn_Char_p_struct*)_tmp1AB;if(_tmp1BA->tag != 10)goto _LLCD;
else{_tmp1BB=_tmp1BA->f1;}}_LLCC: return Cyc_Absyn_char_exp(_tmp1BB,p->loc);_LLCD: {
struct Cyc_Absyn_Float_p_struct*_tmp1BC=(struct Cyc_Absyn_Float_p_struct*)_tmp1AB;
if(_tmp1BC->tag != 11)goto _LLCF;else{_tmp1BD=_tmp1BC->f1;}}_LLCE: return Cyc_Absyn_float_exp(
_tmp1BD,p->loc);_LLCF: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp1BE=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp1AB;if(_tmp1BE->tag != 15)goto _LLD1;else{_tmp1BF=_tmp1BE->f1;_tmp1C0=_tmp1BE->f2;
_tmp1C1=_tmp1BE->f3;if(_tmp1C1 != 0)goto _LLD1;}}_LLD0: {struct Cyc_Absyn_Exp*e1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp1C8=_cycalloc(
sizeof(*_tmp1C8));_tmp1C8[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp1C9;_tmp1C9.tag=
2;_tmp1C9.f1=_tmp1BF;_tmp1C9;});_tmp1C8;}),p->loc);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp1C0);return Cyc_Absyn_unknowncall_exp(e1,
es,p->loc);}_LLD1: {struct Cyc_Absyn_Exp_p_struct*_tmp1C2=(struct Cyc_Absyn_Exp_p_struct*)
_tmp1AB;if(_tmp1C2->tag != 16)goto _LLD3;else{_tmp1C3=_tmp1C2->f1;}}_LLD2: return
_tmp1C3;_LLD3:;_LLD4: Cyc_Parse_err(({const char*_tmp1CA="cannot mix patterns and expressions in case";
_tag_dyneither(_tmp1CA,sizeof(char),44);}),p->loc);return Cyc_Absyn_null_exp(p->loc);
_LLC0:;}union Cyc_YYSTYPE;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple6 val;};
struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{
int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;
struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple1*
val;};struct _tuple18{struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*f2;
union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple18*
val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*
val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple19{struct Cyc_List_List*
f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple19*val;};struct
_union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple20{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple20*val;
};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY16{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY17{int tag;
struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Parse_Declaration_spec*val;};
struct _tuple21{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct
_union_YYSTYPE_YY20{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY21{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Parse_Storage_class
val;};struct _union_YYSTYPE_YY23{int tag;void*val;};struct _union_YYSTYPE_YY24{int
tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual
val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Parse_Declarator*
val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator*val;};
struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum 
Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*
val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple22{
struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
_union_YYSTYPE_YY35{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY36{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};
struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple23{struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple23*val;};
struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{
int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;enum Cyc_Absyn_Kind val;};
struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;
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
YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp1CC[14]="$(sign_t,int)";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp1CC,
_tmp1CC,_tmp1CC + 14}};struct _tuple6 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){struct
_tuple6 yyzzz;{union Cyc_YYSTYPE _tmp1CD=yy1;struct _tuple6 _tmp1CE;_LLD6: if((_tmp1CD.Int_tok).tag
!= 1)goto _LLD8;_tmp1CE=(struct _tuple6)(_tmp1CD.Int_tok).val;_LLD7: yyzzz=_tmp1CE;
goto _LLD5;_LLD8:;_LLD9:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLD5:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple6 yy1){return({union Cyc_YYSTYPE
_tmp1CF;(_tmp1CF.Int_tok).val=yy1;(_tmp1CF.Int_tok).tag=1;_tmp1CF;});}static char
_tmp1D1[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{
_tmp1D1,_tmp1D1,_tmp1D1 + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){char
yyzzz;{union Cyc_YYSTYPE _tmp1D2=yy1;char _tmp1D3;_LLDB: if((_tmp1D2.Char_tok).tag != 
2)goto _LLDD;_tmp1D3=(char)(_tmp1D2.Char_tok).val;_LLDC: yyzzz=_tmp1D3;goto _LLDA;
_LLDD:;_LLDE:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLDA:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp1D4;(_tmp1D4.Char_tok).val=
yy1;(_tmp1D4.Char_tok).tag=2;_tmp1D4;});}static char _tmp1D6[13]="string_t<`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{
_tmp1D6,_tmp1D6,_tmp1D6 + 13}};struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE
yy1){struct _dyneither_ptr yyzzz;{union Cyc_YYSTYPE _tmp1D7=yy1;struct _dyneither_ptr
_tmp1D8;_LLE0: if((_tmp1D7.String_tok).tag != 3)goto _LLE2;_tmp1D8=(struct
_dyneither_ptr)(_tmp1D7.String_tok).val;_LLE1: yyzzz=_tmp1D8;goto _LLDF;_LLE2:;
_LLE3:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLDF:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_String_tok(struct _dyneither_ptr yy1){return({union Cyc_YYSTYPE _tmp1D9;(_tmp1D9.String_tok).val=
yy1;(_tmp1D9.String_tok).tag=3;_tmp1D9;});}static char _tmp1DB[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1DB,
_tmp1DB,_tmp1DB + 56}};struct _tuple18*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){struct
_tuple18*yyzzz;{union Cyc_YYSTYPE _tmp1DC=yy1;struct _tuple18*_tmp1DD;_LLE5: if((
_tmp1DC.YY1).tag != 6)goto _LLE7;_tmp1DD=(struct _tuple18*)(_tmp1DC.YY1).val;_LLE6:
yyzzz=_tmp1DD;goto _LLE4;_LLE7:;_LLE8:(int)_throw((void*)& Cyc_yyfail_YY1);_LLE4:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY1(struct _tuple18*yy1){return({union Cyc_YYSTYPE
_tmp1DE;(_tmp1DE.YY1).val=yy1;(_tmp1DE.YY1).tag=6;_tmp1DE;});}static char _tmp1E0[
19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{
_tmp1E0,_tmp1E0,_tmp1E0 + 19}};union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE
yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp1E1=yy1;union Cyc_Absyn_Constraint*
_tmp1E2;_LLEA: if((_tmp1E1.YY2).tag != 7)goto _LLEC;_tmp1E2=(union Cyc_Absyn_Constraint*)(
_tmp1E1.YY2).val;_LLEB: yyzzz=_tmp1E2;goto _LLE9;_LLEC:;_LLED:(int)_throw((void*)&
Cyc_yyfail_YY2);_LLE9:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*
yy1){return({union Cyc_YYSTYPE _tmp1E3;(_tmp1E3.YY2).val=yy1;(_tmp1E3.YY2).tag=7;
_tmp1E3;});}static char _tmp1E5[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={
Cyc_Core_Failure,{_tmp1E5,_tmp1E5,_tmp1E5 + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE _tmp1E6=yy1;
struct Cyc_Absyn_Exp*_tmp1E7;_LLEF: if((_tmp1E6.YY3).tag != 8)goto _LLF1;_tmp1E7=(
struct Cyc_Absyn_Exp*)(_tmp1E6.YY3).val;_LLF0: yyzzz=_tmp1E7;goto _LLEE;_LLF1:;
_LLF2:(int)_throw((void*)& Cyc_yyfail_YY3);_LLEE:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY3(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp1E8;(_tmp1E8.YY3).val=
yy1;(_tmp1E8.YY3).tag=8;_tmp1E8;});}static char _tmp1EA[10]="exp_opt_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1EA,_tmp1EA,
_tmp1EA + 10}};struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*
yyzzz;{union Cyc_YYSTYPE _tmp1EB=yy1;struct Cyc_Absyn_Exp*_tmp1EC;_LLF4: if((_tmp1EB.YY4).tag
!= 9)goto _LLF6;_tmp1EC=(struct Cyc_Absyn_Exp*)(_tmp1EB.YY4).val;_LLF5: yyzzz=
_tmp1EC;goto _LLF3;_LLF6:;_LLF7:(int)_throw((void*)& Cyc_yyfail_YY4);_LLF3:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE
_tmp1ED;(_tmp1ED.YY4).val=yy1;(_tmp1ED.YY4).tag=9;_tmp1ED;});}static char _tmp1EF[
17]="list_t<exp_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{
_tmp1EF,_tmp1EF,_tmp1EF + 17}};struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp1F0=yy1;struct Cyc_List_List*
_tmp1F1;_LLF9: if((_tmp1F0.YY5).tag != 10)goto _LLFB;_tmp1F1=(struct Cyc_List_List*)(
_tmp1F0.YY5).val;_LLFA: yyzzz=_tmp1F1;goto _LLF8;_LLFB:;_LLFC:(int)_throw((void*)&
Cyc_yyfail_YY5);_LLF8:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp1F2;(_tmp1F2.YY5).val=yy1;(_tmp1F2.YY5).tag=10;
_tmp1F2;});}static char _tmp1F4[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1F4,
_tmp1F4,_tmp1F4 + 47}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp1F5=yy1;struct Cyc_List_List*
_tmp1F6;_LLFE: if((_tmp1F5.YY6).tag != 11)goto _LL100;_tmp1F6=(struct Cyc_List_List*)(
_tmp1F5.YY6).val;_LLFF: yyzzz=_tmp1F6;goto _LLFD;_LL100:;_LL101:(int)_throw((void*)&
Cyc_yyfail_YY6);_LLFD:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp1F7;(_tmp1F7.YY6).val=yy1;(_tmp1F7.YY6).tag=11;
_tmp1F7;});}static char _tmp1F9[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1F9,_tmp1F9,_tmp1F9 + 9}};enum Cyc_Absyn_Primop
Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){enum Cyc_Absyn_Primop yyzzz;{union Cyc_YYSTYPE
_tmp1FA=yy1;enum Cyc_Absyn_Primop _tmp1FB;_LL103: if((_tmp1FA.YY7).tag != 12)goto
_LL105;_tmp1FB=(enum Cyc_Absyn_Primop)(_tmp1FA.YY7).val;_LL104: yyzzz=_tmp1FB;
goto _LL102;_LL105:;_LL106:(int)_throw((void*)& Cyc_yyfail_YY7);_LL102:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY7(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE
_tmp1FC;(_tmp1FC.YY7).val=yy1;(_tmp1FC.YY7).tag=12;_tmp1FC;});}static char _tmp1FE[
19]="opt_t<primop_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{
_tmp1FE,_tmp1FE,_tmp1FE + 19}};struct Cyc_Core_Opt*Cyc_yyget_YY8(union Cyc_YYSTYPE
yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp1FF=yy1;struct Cyc_Core_Opt*
_tmp200;_LL108: if((_tmp1FF.YY8).tag != 13)goto _LL10A;_tmp200=(struct Cyc_Core_Opt*)(
_tmp1FF.YY8).val;_LL109: yyzzz=_tmp200;goto _LL107;_LL10A:;_LL10B:(int)_throw((
void*)& Cyc_yyfail_YY8);_LL107:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY8(struct Cyc_Core_Opt*
yy1){return({union Cyc_YYSTYPE _tmp201;(_tmp201.YY8).val=yy1;(_tmp201.YY8).tag=13;
_tmp201;});}static char _tmp203[7]="qvar_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{_tmp203,_tmp203,_tmp203 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(
union Cyc_YYSTYPE yy1){struct _tuple1*yyzzz;{union Cyc_YYSTYPE _tmp204=yy1;struct
_tuple1*_tmp205;_LL10D: if((_tmp204.QualId_tok).tag != 5)goto _LL10F;_tmp205=(
struct _tuple1*)(_tmp204.QualId_tok).val;_LL10E: yyzzz=_tmp205;goto _LL10C;_LL10F:;
_LL110:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL10C:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_QualId_tok(struct _tuple1*yy1){return({union Cyc_YYSTYPE _tmp206;(_tmp206.QualId_tok).val=
yy1;(_tmp206.QualId_tok).tag=5;_tmp206;});}static char _tmp208[7]="stmt_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp208,_tmp208,
_tmp208 + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Stmt*
yyzzz;{union Cyc_YYSTYPE _tmp209=yy1;struct Cyc_Absyn_Stmt*_tmp20A;_LL112: if((
_tmp209.YY9).tag != 14)goto _LL114;_tmp20A=(struct Cyc_Absyn_Stmt*)(_tmp209.YY9).val;
_LL113: yyzzz=_tmp20A;goto _LL111;_LL114:;_LL115:(int)_throw((void*)& Cyc_yyfail_YY9);
_LL111:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Stmt*yy1){return({
union Cyc_YYSTYPE _tmp20B;(_tmp20B.YY9).val=yy1;(_tmp20B.YY9).tag=14;_tmp20B;});}
static char _tmp20D[27]="list_t<switch_clause_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp20D,_tmp20D,_tmp20D + 27}};struct Cyc_List_List*
Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp20E=yy1;struct Cyc_List_List*_tmp20F;_LL117: if((_tmp20E.YY10).tag != 15)goto
_LL119;_tmp20F=(struct Cyc_List_List*)(_tmp20E.YY10).val;_LL118: yyzzz=_tmp20F;
goto _LL116;_LL119:;_LL11A:(int)_throw((void*)& Cyc_yyfail_YY10);_LL116:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp210;(_tmp210.YY10).val=yy1;(_tmp210.YY10).tag=15;_tmp210;});}static char
_tmp212[6]="pat_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{
_tmp212,_tmp212,_tmp212 + 6}};struct Cyc_Absyn_Pat*Cyc_yyget_YY11(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE _tmp213=yy1;struct Cyc_Absyn_Pat*
_tmp214;_LL11C: if((_tmp213.YY11).tag != 16)goto _LL11E;_tmp214=(struct Cyc_Absyn_Pat*)(
_tmp213.YY11).val;_LL11D: yyzzz=_tmp214;goto _LL11B;_LL11E:;_LL11F:(int)_throw((
void*)& Cyc_yyfail_YY11);_LL11B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY11(struct Cyc_Absyn_Pat*
yy1){return({union Cyc_YYSTYPE _tmp215;(_tmp215.YY11).val=yy1;(_tmp215.YY11).tag=
16;_tmp215;});}static char _tmp217[28]="$(list_t<pat_t,`H>,bool)@`H";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp217,_tmp217,
_tmp217 + 28}};struct _tuple19*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){struct _tuple19*
yyzzz;{union Cyc_YYSTYPE _tmp218=yy1;struct _tuple19*_tmp219;_LL121: if((_tmp218.YY12).tag
!= 17)goto _LL123;_tmp219=(struct _tuple19*)(_tmp218.YY12).val;_LL122: yyzzz=
_tmp219;goto _LL120;_LL123:;_LL124:(int)_throw((void*)& Cyc_yyfail_YY12);_LL120:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY12(struct _tuple19*yy1){return({union Cyc_YYSTYPE
_tmp21A;(_tmp21A.YY12).val=yy1;(_tmp21A.YY12).tag=17;_tmp21A;});}static char
_tmp21C[17]="list_t<pat_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY13={
Cyc_Core_Failure,{_tmp21C,_tmp21C,_tmp21C + 17}};struct Cyc_List_List*Cyc_yyget_YY13(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp21D=yy1;
struct Cyc_List_List*_tmp21E;_LL126: if((_tmp21D.YY13).tag != 18)goto _LL128;_tmp21E=(
struct Cyc_List_List*)(_tmp21D.YY13).val;_LL127: yyzzz=_tmp21E;goto _LL125;_LL128:;
_LL129:(int)_throw((void*)& Cyc_yyfail_YY13);_LL125:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp21F;(_tmp21F.YY13).val=
yy1;(_tmp21F.YY13).tag=18;_tmp21F;});}static char _tmp221[36]="$(list_t<designator_t,`H>,pat_t)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp221,
_tmp221,_tmp221 + 36}};struct _tuple20*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){struct
_tuple20*yyzzz;{union Cyc_YYSTYPE _tmp222=yy1;struct _tuple20*_tmp223;_LL12B: if((
_tmp222.YY14).tag != 19)goto _LL12D;_tmp223=(struct _tuple20*)(_tmp222.YY14).val;
_LL12C: yyzzz=_tmp223;goto _LL12A;_LL12D:;_LL12E:(int)_throw((void*)& Cyc_yyfail_YY14);
_LL12A:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY14(struct _tuple20*yy1){return({union
Cyc_YYSTYPE _tmp224;(_tmp224.YY14).val=yy1;(_tmp224.YY14).tag=19;_tmp224;});}
static char _tmp226[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp226,_tmp226,
_tmp226 + 47}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp227=yy1;struct Cyc_List_List*_tmp228;_LL130: if((
_tmp227.YY15).tag != 20)goto _LL132;_tmp228=(struct Cyc_List_List*)(_tmp227.YY15).val;
_LL131: yyzzz=_tmp228;goto _LL12F;_LL132:;_LL133:(int)_throw((void*)& Cyc_yyfail_YY15);
_LL12F:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY15(struct Cyc_List_List*yy1){return({
union Cyc_YYSTYPE _tmp229;(_tmp229.YY15).val=yy1;(_tmp229.YY15).tag=20;_tmp229;});}
static char _tmp22B[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp22B,
_tmp22B,_tmp22B + 58}};struct _tuple19*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){struct
_tuple19*yyzzz;{union Cyc_YYSTYPE _tmp22C=yy1;struct _tuple19*_tmp22D;_LL135: if((
_tmp22C.YY16).tag != 21)goto _LL137;_tmp22D=(struct _tuple19*)(_tmp22C.YY16).val;
_LL136: yyzzz=_tmp22D;goto _LL134;_LL137:;_LL138:(int)_throw((void*)& Cyc_yyfail_YY16);
_LL134:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY16(struct _tuple19*yy1){return({union
Cyc_YYSTYPE _tmp22E;(_tmp22E.YY16).val=yy1;(_tmp22E.YY16).tag=21;_tmp22E;});}
static char _tmp230[9]="fndecl_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY17={
Cyc_Core_Failure,{_tmp230,_tmp230,_tmp230 + 9}};struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE _tmp231=yy1;
struct Cyc_Absyn_Fndecl*_tmp232;_LL13A: if((_tmp231.YY17).tag != 22)goto _LL13C;
_tmp232=(struct Cyc_Absyn_Fndecl*)(_tmp231.YY17).val;_LL13B: yyzzz=_tmp232;goto
_LL139;_LL13C:;_LL13D:(int)_throw((void*)& Cyc_yyfail_YY17);_LL139:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE
_tmp233;(_tmp233.YY17).val=yy1;(_tmp233.YY17).tag=22;_tmp233;});}static char
_tmp235[18]="list_t<decl_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={
Cyc_Core_Failure,{_tmp235,_tmp235,_tmp235 + 18}};struct Cyc_List_List*Cyc_yyget_YY18(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp236=yy1;
struct Cyc_List_List*_tmp237;_LL13F: if((_tmp236.YY18).tag != 23)goto _LL141;_tmp237=(
struct Cyc_List_List*)(_tmp236.YY18).val;_LL140: yyzzz=_tmp237;goto _LL13E;_LL141:;
_LL142:(int)_throw((void*)& Cyc_yyfail_YY18);_LL13E:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY18(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp238;(_tmp238.YY18).val=
yy1;(_tmp238.YY18).tag=23;_tmp238;});}static char _tmp23A[12]="decl_spec_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp23A,_tmp23A,
_tmp23A + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE
yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE _tmp23B=yy1;struct
Cyc_Parse_Declaration_spec*_tmp23C;_LL144: if((_tmp23B.YY19).tag != 24)goto _LL146;
_tmp23C=(struct Cyc_Parse_Declaration_spec*)(_tmp23B.YY19).val;_LL145: yyzzz=
_tmp23C;goto _LL143;_LL146:;_LL147:(int)_throw((void*)& Cyc_yyfail_YY19);_LL143:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*yy1){
return({union Cyc_YYSTYPE _tmp23D;(_tmp23D.YY19).val=yy1;(_tmp23D.YY19).tag=24;
_tmp23D;});}static char _tmp23F[29]="$(declarator_t,exp_opt_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp23F,_tmp23F,_tmp23F + 29}};struct _tuple21*
Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){struct _tuple21*yyzzz;{union Cyc_YYSTYPE
_tmp240=yy1;struct _tuple21*_tmp241;_LL149: if((_tmp240.YY20).tag != 25)goto _LL14B;
_tmp241=(struct _tuple21*)(_tmp240.YY20).val;_LL14A: yyzzz=_tmp241;goto _LL148;
_LL14B:;_LL14C:(int)_throw((void*)& Cyc_yyfail_YY20);_LL148:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY20(struct _tuple21*yy1){return({union Cyc_YYSTYPE _tmp242;(_tmp242.YY20).val=
yy1;(_tmp242.YY20).tag=25;_tmp242;});}static char _tmp244[40]="list_t<$(declarator_t,exp_opt_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp244,
_tmp244,_tmp244 + 40}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp245=yy1;struct Cyc_List_List*
_tmp246;_LL14E: if((_tmp245.YY21).tag != 26)goto _LL150;_tmp246=(struct Cyc_List_List*)(
_tmp245.YY21).val;_LL14F: yyzzz=_tmp246;goto _LL14D;_LL150:;_LL151:(int)_throw((
void*)& Cyc_yyfail_YY21);_LL14D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY21(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp247;(_tmp247.YY21).val=yy1;(_tmp247.YY21).tag=
26;_tmp247;});}static char _tmp249[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp249,_tmp249,_tmp249 + 16}};enum Cyc_Parse_Storage_class
Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){enum Cyc_Parse_Storage_class yyzzz;{union Cyc_YYSTYPE
_tmp24A=yy1;enum Cyc_Parse_Storage_class _tmp24B;_LL153: if((_tmp24A.YY22).tag != 
27)goto _LL155;_tmp24B=(enum Cyc_Parse_Storage_class)(_tmp24A.YY22).val;_LL154:
yyzzz=_tmp24B;goto _LL152;_LL155:;_LL156:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL152:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class yy1){
return({union Cyc_YYSTYPE _tmp24C;(_tmp24C.YY22).val=yy1;(_tmp24C.YY22).tag=27;
_tmp24C;});}static char _tmp24E[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp24E,_tmp24E,_tmp24E + 17}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp24F=yy1;void*_tmp250;_LL158:
if((_tmp24F.YY23).tag != 28)goto _LL15A;_tmp250=(void*)(_tmp24F.YY23).val;_LL159:
yyzzz=_tmp250;goto _LL157;_LL15A:;_LL15B:(int)_throw((void*)& Cyc_yyfail_YY23);
_LL157:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY23(void*yy1){return({union Cyc_YYSTYPE
_tmp251;(_tmp251.YY23).val=yy1;(_tmp251.YY23).tag=28;_tmp251;});}static char
_tmp253[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY24={
Cyc_Core_Failure,{_tmp253,_tmp253,_tmp253 + 12}};enum Cyc_Absyn_AggrKind Cyc_yyget_YY24(
union Cyc_YYSTYPE yy1){enum Cyc_Absyn_AggrKind yyzzz;{union Cyc_YYSTYPE _tmp254=yy1;
enum Cyc_Absyn_AggrKind _tmp255;_LL15D: if((_tmp254.YY24).tag != 29)goto _LL15F;
_tmp255=(enum Cyc_Absyn_AggrKind)(_tmp254.YY24).val;_LL15E: yyzzz=_tmp255;goto
_LL15C;_LL15F:;_LL160:(int)_throw((void*)& Cyc_yyfail_YY24);_LL15C:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE
_tmp256;(_tmp256.YY24).val=yy1;(_tmp256.YY24).tag=29;_tmp256;});}static char
_tmp258[8]="tqual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{
_tmp258,_tmp258,_tmp258 + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE _tmp259=yy1;struct Cyc_Absyn_Tqual
_tmp25A;_LL162: if((_tmp259.YY25).tag != 30)goto _LL164;_tmp25A=(struct Cyc_Absyn_Tqual)(
_tmp259.YY25).val;_LL163: yyzzz=_tmp25A;goto _LL161;_LL164:;_LL165:(int)_throw((
void*)& Cyc_yyfail_YY25);_LL161:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual
yy1){return({union Cyc_YYSTYPE _tmp25B;(_tmp25B.YY25).val=yy1;(_tmp25B.YY25).tag=
30;_tmp25B;});}static char _tmp25D[23]="list_t<aggrfield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp25D,_tmp25D,_tmp25D + 23}};struct Cyc_List_List*
Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp25E=yy1;struct Cyc_List_List*_tmp25F;_LL167: if((_tmp25E.YY26).tag != 31)goto
_LL169;_tmp25F=(struct Cyc_List_List*)(_tmp25E.YY26).val;_LL168: yyzzz=_tmp25F;
goto _LL166;_LL169:;_LL16A:(int)_throw((void*)& Cyc_yyfail_YY26);_LL166:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp260;(_tmp260.YY26).val=yy1;(_tmp260.YY26).tag=31;_tmp260;});}static char
_tmp262[34]="list_t<list_t<aggrfield_t,`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp262,_tmp262,_tmp262 + 34}};struct Cyc_List_List*
Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp263=yy1;struct Cyc_List_List*_tmp264;_LL16C: if((_tmp263.YY27).tag != 32)goto
_LL16E;_tmp264=(struct Cyc_List_List*)(_tmp263.YY27).val;_LL16D: yyzzz=_tmp264;
goto _LL16B;_LL16E:;_LL16F:(int)_throw((void*)& Cyc_yyfail_YY27);_LL16B:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp265;(_tmp265.YY27).val=yy1;(_tmp265.YY27).tag=32;_tmp265;});}static char
_tmp267[31]="list_t<type_modifier_t<`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp267,_tmp267,_tmp267 + 31}};struct Cyc_List_List*
Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp268=yy1;struct Cyc_List_List*_tmp269;_LL171: if((_tmp268.YY28).tag != 33)goto
_LL173;_tmp269=(struct Cyc_List_List*)(_tmp268.YY28).val;_LL172: yyzzz=_tmp269;
goto _LL170;_LL173:;_LL174:(int)_throw((void*)& Cyc_yyfail_YY28);_LL170:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp26A;(_tmp26A.YY28).val=yy1;(_tmp26A.YY28).tag=33;_tmp26A;});}static char
_tmp26C[13]="declarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={
Cyc_Core_Failure,{_tmp26C,_tmp26C,_tmp26C + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(
union Cyc_YYSTYPE yy1){struct Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE _tmp26D=
yy1;struct Cyc_Parse_Declarator*_tmp26E;_LL176: if((_tmp26D.YY29).tag != 34)goto
_LL178;_tmp26E=(struct Cyc_Parse_Declarator*)(_tmp26D.YY29).val;_LL177: yyzzz=
_tmp26E;goto _LL175;_LL178:;_LL179:(int)_throw((void*)& Cyc_yyfail_YY29);_LL175:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*yy1){return({
union Cyc_YYSTYPE _tmp26F;(_tmp26F.YY29).val=yy1;(_tmp26F.YY29).tag=34;_tmp26F;});}
static char _tmp271[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp271,_tmp271,_tmp271 + 21}};struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{
union Cyc_YYSTYPE _tmp272=yy1;struct Cyc_Parse_Abstractdeclarator*_tmp273;_LL17B:
if((_tmp272.YY30).tag != 35)goto _LL17D;_tmp273=(struct Cyc_Parse_Abstractdeclarator*)(
_tmp272.YY30).val;_LL17C: yyzzz=_tmp273;goto _LL17A;_LL17D:;_LL17E:(int)_throw((
void*)& Cyc_yyfail_YY30);_LL17A:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*
yy1){return({union Cyc_YYSTYPE _tmp274;(_tmp274.YY30).val=yy1;(_tmp274.YY30).tag=
35;_tmp274;});}static char _tmp276[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp276,_tmp276,_tmp276 + 5}};int Cyc_yyget_YY31(
union Cyc_YYSTYPE yy1){int yyzzz;{union Cyc_YYSTYPE _tmp277=yy1;int _tmp278;_LL180: if((
_tmp277.YY31).tag != 36)goto _LL182;_tmp278=(int)(_tmp277.YY31).val;_LL181: yyzzz=
_tmp278;goto _LL17F;_LL182:;_LL183:(int)_throw((void*)& Cyc_yyfail_YY31);_LL17F:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp279;(
_tmp279.YY31).val=yy1;(_tmp279.YY31).tag=36;_tmp279;});}static char _tmp27B[8]="scope_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp27B,
_tmp27B,_tmp27B + 8}};enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;{union Cyc_YYSTYPE _tmp27C=yy1;enum Cyc_Absyn_Scope
_tmp27D;_LL185: if((_tmp27C.YY32).tag != 37)goto _LL187;_tmp27D=(enum Cyc_Absyn_Scope)(
_tmp27C.YY32).val;_LL186: yyzzz=_tmp27D;goto _LL184;_LL187:;_LL188:(int)_throw((
void*)& Cyc_yyfail_YY32);_LL184:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope
yy1){return({union Cyc_YYSTYPE _tmp27E;(_tmp27E.YY32).val=yy1;(_tmp27E.YY32).tag=
37;_tmp27E;});}static char _tmp280[16]="datatypefield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp280,_tmp280,_tmp280 + 16}};struct Cyc_Absyn_Datatypefield*
Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Datatypefield*yyzzz;{union
Cyc_YYSTYPE _tmp281=yy1;struct Cyc_Absyn_Datatypefield*_tmp282;_LL18A: if((_tmp281.YY33).tag
!= 38)goto _LL18C;_tmp282=(struct Cyc_Absyn_Datatypefield*)(_tmp281.YY33).val;
_LL18B: yyzzz=_tmp282;goto _LL189;_LL18C:;_LL18D:(int)_throw((void*)& Cyc_yyfail_YY33);
_LL189:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){
return({union Cyc_YYSTYPE _tmp283;(_tmp283.YY33).val=yy1;(_tmp283.YY33).tag=38;
_tmp283;});}static char _tmp285[27]="list_t<datatypefield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp285,_tmp285,_tmp285 + 27}};struct Cyc_List_List*
Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp286=yy1;struct Cyc_List_List*_tmp287;_LL18F: if((_tmp286.YY34).tag != 39)goto
_LL191;_tmp287=(struct Cyc_List_List*)(_tmp286.YY34).val;_LL190: yyzzz=_tmp287;
goto _LL18E;_LL191:;_LL192:(int)_throw((void*)& Cyc_yyfail_YY34);_LL18E:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp288;(_tmp288.YY34).val=yy1;(_tmp288.YY34).tag=39;_tmp288;});}static char
_tmp28A[55]="$(tqual_t,list_t<type_specifier_t,`H>,attributes_t)@`H";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp28A,_tmp28A,
_tmp28A + 55}};struct _tuple22*Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){struct _tuple22*
yyzzz;{union Cyc_YYSTYPE _tmp28B=yy1;struct _tuple22*_tmp28C;_LL194: if((_tmp28B.YY35).tag
!= 40)goto _LL196;_tmp28C=(struct _tuple22*)(_tmp28B.YY35).val;_LL195: yyzzz=
_tmp28C;goto _LL193;_LL196:;_LL197:(int)_throw((void*)& Cyc_yyfail_YY35);_LL193:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY35(struct _tuple22*yy1){return({union Cyc_YYSTYPE
_tmp28D;(_tmp28D.YY35).val=yy1;(_tmp28D.YY35).tag=40;_tmp28D;});}static char
_tmp28F[17]="list_t<var_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY36={
Cyc_Core_Failure,{_tmp28F,_tmp28F,_tmp28F + 17}};struct Cyc_List_List*Cyc_yyget_YY36(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp290=yy1;
struct Cyc_List_List*_tmp291;_LL199: if((_tmp290.YY36).tag != 41)goto _LL19B;_tmp291=(
struct Cyc_List_List*)(_tmp290.YY36).val;_LL19A: yyzzz=_tmp291;goto _LL198;_LL19B:;
_LL19C:(int)_throw((void*)& Cyc_yyfail_YY36);_LL198:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp292;(_tmp292.YY36).val=
yy1;(_tmp292.YY36).tag=41;_tmp292;});}static char _tmp294[37]="$(opt_t<var_t,`H>,tqual_t,type_t)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp294,
_tmp294,_tmp294 + 37}};struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){struct
_tuple8*yyzzz;{union Cyc_YYSTYPE _tmp295=yy1;struct _tuple8*_tmp296;_LL19E: if((
_tmp295.YY37).tag != 42)goto _LL1A0;_tmp296=(struct _tuple8*)(_tmp295.YY37).val;
_LL19F: yyzzz=_tmp296;goto _LL19D;_LL1A0:;_LL1A1:(int)_throw((void*)& Cyc_yyfail_YY37);
_LL19D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union
Cyc_YYSTYPE _tmp297;(_tmp297.YY37).val=yy1;(_tmp297.YY37).tag=42;_tmp297;});}
static char _tmp299[48]="list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp299,_tmp299,
_tmp299 + 48}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp29A=yy1;struct Cyc_List_List*_tmp29B;_LL1A3: if((
_tmp29A.YY38).tag != 43)goto _LL1A5;_tmp29B=(struct Cyc_List_List*)(_tmp29A.YY38).val;
_LL1A4: yyzzz=_tmp29B;goto _LL1A2;_LL1A5:;_LL1A6:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL1A2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({
union Cyc_YYSTYPE _tmp29C;(_tmp29C.YY38).val=yy1;(_tmp29C.YY38).tag=43;_tmp29C;});}
static char _tmp29E[127]="$(list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,opt_t<type_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp29E,
_tmp29E,_tmp29E + 127}};struct _tuple23*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){struct
_tuple23*yyzzz;{union Cyc_YYSTYPE _tmp29F=yy1;struct _tuple23*_tmp2A0;_LL1A8: if((
_tmp29F.YY39).tag != 44)goto _LL1AA;_tmp2A0=(struct _tuple23*)(_tmp29F.YY39).val;
_LL1A9: yyzzz=_tmp2A0;goto _LL1A7;_LL1AA:;_LL1AB:(int)_throw((void*)& Cyc_yyfail_YY39);
_LL1A7:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY39(struct _tuple23*yy1){return({union
Cyc_YYSTYPE _tmp2A1;(_tmp2A1.YY39).val=yy1;(_tmp2A1.YY39).tag=44;_tmp2A1;});}
static char _tmp2A3[18]="list_t<type_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY40={
Cyc_Core_Failure,{_tmp2A3,_tmp2A3,_tmp2A3 + 18}};struct Cyc_List_List*Cyc_yyget_YY40(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2A4=yy1;
struct Cyc_List_List*_tmp2A5;_LL1AD: if((_tmp2A4.YY40).tag != 45)goto _LL1AF;_tmp2A5=(
struct Cyc_List_List*)(_tmp2A4.YY40).val;_LL1AE: yyzzz=_tmp2A5;goto _LL1AC;_LL1AF:;
_LL1B0:(int)_throw((void*)& Cyc_yyfail_YY40);_LL1AC:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2A6;(_tmp2A6.YY40).val=
yy1;(_tmp2A6.YY40).tag=45;_tmp2A6;});}static char _tmp2A8[24]="list_t<designator_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2A8,
_tmp2A8,_tmp2A8 + 24}};struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2A9=yy1;struct Cyc_List_List*
_tmp2AA;_LL1B2: if((_tmp2A9.YY41).tag != 46)goto _LL1B4;_tmp2AA=(struct Cyc_List_List*)(
_tmp2A9.YY41).val;_LL1B3: yyzzz=_tmp2AA;goto _LL1B1;_LL1B4:;_LL1B5:(int)_throw((
void*)& Cyc_yyfail_YY41);_LL1B1:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp2AB;(_tmp2AB.YY41).val=yy1;(_tmp2AB.YY41).tag=
46;_tmp2AB;});}static char _tmp2AD[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp2AD,_tmp2AD,_tmp2AD + 13}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp2AE=yy1;void*_tmp2AF;_LL1B7:
if((_tmp2AE.YY42).tag != 47)goto _LL1B9;_tmp2AF=(void*)(_tmp2AE.YY42).val;_LL1B8:
yyzzz=_tmp2AF;goto _LL1B6;_LL1B9:;_LL1BA:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1B6:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE
_tmp2B0;(_tmp2B0.YY42).val=yy1;(_tmp2B0.YY42).tag=47;_tmp2B0;});}static char
_tmp2B2[7]="kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{
_tmp2B2,_tmp2B2,_tmp2B2 + 7}};enum Cyc_Absyn_Kind Cyc_yyget_YY43(union Cyc_YYSTYPE
yy1){enum Cyc_Absyn_Kind yyzzz;{union Cyc_YYSTYPE _tmp2B3=yy1;enum Cyc_Absyn_Kind
_tmp2B4;_LL1BC: if((_tmp2B3.YY43).tag != 48)goto _LL1BE;_tmp2B4=(enum Cyc_Absyn_Kind)(
_tmp2B3.YY43).val;_LL1BD: yyzzz=_tmp2B4;goto _LL1BB;_LL1BE:;_LL1BF:(int)_throw((
void*)& Cyc_yyfail_YY43);_LL1BB:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY43(enum Cyc_Absyn_Kind
yy1){return({union Cyc_YYSTYPE _tmp2B5;(_tmp2B5.YY43).val=yy1;(_tmp2B5.YY43).tag=
48;_tmp2B5;});}static char _tmp2B7[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2B7,_tmp2B7,_tmp2B7 + 7}};void*Cyc_yyget_YY44(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp2B8=yy1;void*_tmp2B9;_LL1C1:
if((_tmp2B8.YY44).tag != 49)goto _LL1C3;_tmp2B9=(void*)(_tmp2B8.YY44).val;_LL1C2:
yyzzz=_tmp2B9;goto _LL1C0;_LL1C3:;_LL1C4:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1C0:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE
_tmp2BA;(_tmp2BA.YY44).val=yy1;(_tmp2BA.YY44).tag=49;_tmp2BA;});}static char
_tmp2BC[23]="list_t<attribute_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY45={
Cyc_Core_Failure,{_tmp2BC,_tmp2BC,_tmp2BC + 23}};struct Cyc_List_List*Cyc_yyget_YY45(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2BD=yy1;
struct Cyc_List_List*_tmp2BE;_LL1C6: if((_tmp2BD.YY45).tag != 50)goto _LL1C8;_tmp2BE=(
struct Cyc_List_List*)(_tmp2BD.YY45).val;_LL1C7: yyzzz=_tmp2BE;goto _LL1C5;_LL1C8:;
_LL1C9:(int)_throw((void*)& Cyc_yyfail_YY45);_LL1C5:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2BF;(_tmp2BF.YY45).val=
yy1;(_tmp2BF.YY45).tag=50;_tmp2BF;});}static char _tmp2C1[12]="attribute_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp2C1,_tmp2C1,
_tmp2C1 + 12}};void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp2C2=yy1;void*_tmp2C3;_LL1CB: if((_tmp2C2.YY46).tag != 51)goto _LL1CD;_tmp2C3=(
void*)(_tmp2C2.YY46).val;_LL1CC: yyzzz=_tmp2C3;goto _LL1CA;_LL1CD:;_LL1CE:(int)
_throw((void*)& Cyc_yyfail_YY46);_LL1CA:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY46(
void*yy1){return({union Cyc_YYSTYPE _tmp2C4;(_tmp2C4.YY46).val=yy1;(_tmp2C4.YY46).tag=
51;_tmp2C4;});}static char _tmp2C6[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp2C6,_tmp2C6,_tmp2C6 + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union Cyc_YYSTYPE
_tmp2C7=yy1;struct Cyc_Absyn_Enumfield*_tmp2C8;_LL1D0: if((_tmp2C7.YY47).tag != 52)
goto _LL1D2;_tmp2C8=(struct Cyc_Absyn_Enumfield*)(_tmp2C7.YY47).val;_LL1D1: yyzzz=
_tmp2C8;goto _LL1CF;_LL1D2:;_LL1D3:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1CF:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({
union Cyc_YYSTYPE _tmp2C9;(_tmp2C9.YY47).val=yy1;(_tmp2C9.YY47).tag=52;_tmp2C9;});}
static char _tmp2CB[23]="list_t<enumfield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp2CB,_tmp2CB,_tmp2CB + 23}};struct Cyc_List_List*
Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2CC=yy1;struct Cyc_List_List*_tmp2CD;_LL1D5: if((_tmp2CC.YY48).tag != 53)goto
_LL1D7;_tmp2CD=(struct Cyc_List_List*)(_tmp2CC.YY48).val;_LL1D6: yyzzz=_tmp2CD;
goto _LL1D4;_LL1D7:;_LL1D8:(int)_throw((void*)& Cyc_yyfail_YY48);_LL1D4:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp2CE;(_tmp2CE.YY48).val=yy1;(_tmp2CE.YY48).tag=53;_tmp2CE;});}static char
_tmp2D0[17]="opt_t<type_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={
Cyc_Core_Failure,{_tmp2D0,_tmp2D0,_tmp2D0 + 17}};struct Cyc_Core_Opt*Cyc_yyget_YY49(
union Cyc_YYSTYPE yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp2D1=yy1;
struct Cyc_Core_Opt*_tmp2D2;_LL1DA: if((_tmp2D1.YY49).tag != 54)goto _LL1DC;_tmp2D2=(
struct Cyc_Core_Opt*)(_tmp2D1.YY49).val;_LL1DB: yyzzz=_tmp2D2;goto _LL1D9;_LL1DC:;
_LL1DD:(int)_throw((void*)& Cyc_yyfail_YY49);_LL1D9:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY49(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp2D3;(_tmp2D3.YY49).val=
yy1;(_tmp2D3.YY49).tag=54;_tmp2D3;});}static char _tmp2D5[31]="list_t<$(type_t,type_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp2D5,
_tmp2D5,_tmp2D5 + 31}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2D6=yy1;struct Cyc_List_List*
_tmp2D7;_LL1DF: if((_tmp2D6.YY50).tag != 55)goto _LL1E1;_tmp2D7=(struct Cyc_List_List*)(
_tmp2D6.YY50).val;_LL1E0: yyzzz=_tmp2D7;goto _LL1DE;_LL1E1:;_LL1E2:(int)_throw((
void*)& Cyc_yyfail_YY50);_LL1DE:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp2D8;(_tmp2D8.YY50).val=yy1;(_tmp2D8.YY50).tag=
55;_tmp2D8;});}static char _tmp2DA[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp2DA,_tmp2DA,_tmp2DA + 15}};union Cyc_Absyn_Constraint*
Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE
_tmp2DB=yy1;union Cyc_Absyn_Constraint*_tmp2DC;_LL1E4: if((_tmp2DB.YY51).tag != 56)
goto _LL1E6;_tmp2DC=(union Cyc_Absyn_Constraint*)(_tmp2DB.YY51).val;_LL1E5: yyzzz=
_tmp2DC;goto _LL1E3;_LL1E6:;_LL1E7:(int)_throw((void*)& Cyc_yyfail_YY51);_LL1E3:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){return({
union Cyc_YYSTYPE _tmp2DD;(_tmp2DD.YY51).val=yy1;(_tmp2DD.YY51).tag=56;_tmp2DD;});}
static char _tmp2DF[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp2DF,_tmp2DF,
_tmp2DF + 45}};struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2E0=yy1;struct Cyc_List_List*_tmp2E1;_LL1E9: if((
_tmp2E0.YY52).tag != 57)goto _LL1EB;_tmp2E1=(struct Cyc_List_List*)(_tmp2E0.YY52).val;
_LL1EA: yyzzz=_tmp2E1;goto _LL1E8;_LL1EB:;_LL1EC:(int)_throw((void*)& Cyc_yyfail_YY52);
_LL1E8:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({
union Cyc_YYSTYPE _tmp2E2;(_tmp2E2.YY52).val=yy1;(_tmp2E2.YY52).tag=57;_tmp2E2;});}
static char _tmp2E4[15]="pointer_qual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY53={
Cyc_Core_Failure,{_tmp2E4,_tmp2E4,_tmp2E4 + 15}};void*Cyc_yyget_YY53(union Cyc_YYSTYPE
yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp2E5=yy1;void*_tmp2E6;_LL1EE: if((_tmp2E5.YY53).tag
!= 58)goto _LL1F0;_tmp2E6=(void*)(_tmp2E5.YY53).val;_LL1EF: yyzzz=_tmp2E6;goto
_LL1ED;_LL1F0:;_LL1F1:(int)_throw((void*)& Cyc_yyfail_YY53);_LL1ED:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY53(void*yy1){return({union Cyc_YYSTYPE _tmp2E7;(_tmp2E7.YY53).val=
yy1;(_tmp2E7.YY53).tag=58;_tmp2E7;});}static char _tmp2E9[16]="pointer_quals_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp2E9,
_tmp2E9,_tmp2E9 + 16}};struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2EA=yy1;struct Cyc_List_List*
_tmp2EB;_LL1F3: if((_tmp2EA.YY54).tag != 59)goto _LL1F5;_tmp2EB=(struct Cyc_List_List*)(
_tmp2EA.YY54).val;_LL1F4: yyzzz=_tmp2EB;goto _LL1F2;_LL1F5:;_LL1F6:(int)_throw((
void*)& Cyc_yyfail_YY54);_LL1F2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp2EC;(_tmp2EC.YY54).val=yy1;(_tmp2EC.YY54).tag=
59;_tmp2EC;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int
last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){return({struct Cyc_Yyltype
_tmp2ED;_tmp2ED.timestamp=0;_tmp2ED.first_line=0;_tmp2ED.first_column=0;_tmp2ED.last_line=
0;_tmp2ED.last_column=0;_tmp2ED;});}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
static short Cyc_yytranslate[372]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,143,2,2,127,141,138,2,124,125,132,135,120,139,129,140,2,2,2,2,
2,2,2,2,2,2,128,117,122,121,123,134,133,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,130,2,131,137,126,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,118,136,119,142,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,
45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,
72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,
99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116};static
char _tmp2EE[2]="$";static char _tmp2EF[6]="error";static char _tmp2F0[12]="$undefined.";
static char _tmp2F1[5]="AUTO";static char _tmp2F2[9]="REGISTER";static char _tmp2F3[7]="STATIC";
static char _tmp2F4[7]="EXTERN";static char _tmp2F5[8]="TYPEDEF";static char _tmp2F6[5]="VOID";
static char _tmp2F7[5]="CHAR";static char _tmp2F8[6]="SHORT";static char _tmp2F9[4]="INT";
static char _tmp2FA[5]="LONG";static char _tmp2FB[6]="FLOAT";static char _tmp2FC[7]="DOUBLE";
static char _tmp2FD[7]="SIGNED";static char _tmp2FE[9]="UNSIGNED";static char _tmp2FF[
6]="CONST";static char _tmp300[9]="VOLATILE";static char _tmp301[9]="RESTRICT";
static char _tmp302[7]="STRUCT";static char _tmp303[6]="UNION";static char _tmp304[5]="CASE";
static char _tmp305[8]="DEFAULT";static char _tmp306[7]="INLINE";static char _tmp307[7]="SIZEOF";
static char _tmp308[9]="OFFSETOF";static char _tmp309[3]="IF";static char _tmp30A[5]="ELSE";
static char _tmp30B[7]="SWITCH";static char _tmp30C[6]="WHILE";static char _tmp30D[3]="DO";
static char _tmp30E[4]="FOR";static char _tmp30F[5]="GOTO";static char _tmp310[9]="CONTINUE";
static char _tmp311[6]="BREAK";static char _tmp312[7]="RETURN";static char _tmp313[5]="ENUM";
static char _tmp314[8]="NULL_kw";static char _tmp315[4]="LET";static char _tmp316[6]="THROW";
static char _tmp317[4]="TRY";static char _tmp318[6]="CATCH";static char _tmp319[7]="EXPORT";
static char _tmp31A[4]="NEW";static char _tmp31B[9]="ABSTRACT";static char _tmp31C[9]="FALLTHRU";
static char _tmp31D[6]="USING";static char _tmp31E[10]="NAMESPACE";static char _tmp31F[
9]="DATATYPE";static char _tmp320[8]="XTUNION";static char _tmp321[7]="TUNION";
static char _tmp322[7]="MALLOC";static char _tmp323[8]="RMALLOC";static char _tmp324[7]="CALLOC";
static char _tmp325[8]="RCALLOC";static char _tmp326[5]="SWAP";static char _tmp327[9]="REGION_T";
static char _tmp328[6]="TAG_T";static char _tmp329[7]="REGION";static char _tmp32A[5]="RNEW";
static char _tmp32B[8]="REGIONS";static char _tmp32C[13]="RESET_REGION";static char
_tmp32D[4]="GEN";static char _tmp32E[16]="NOZEROTERM_QUAL";static char _tmp32F[14]="ZEROTERM_QUAL";
static char _tmp330[12]="REGION_QUAL";static char _tmp331[7]="PORTON";static char
_tmp332[8]="PORTOFF";static char _tmp333[12]="DYNREGION_T";static char _tmp334[6]="ALIAS";
static char _tmp335[8]="NUMELTS";static char _tmp336[8]="VALUEOF";static char _tmp337[
10]="VALUEOF_T";static char _tmp338[9]="TAGCHECK";static char _tmp339[13]="NUMELTS_QUAL";
static char _tmp33A[10]="THIN_QUAL";static char _tmp33B[9]="FAT_QUAL";static char
_tmp33C[13]="NOTNULL_QUAL";static char _tmp33D[14]="NULLABLE_QUAL";static char
_tmp33E[12]="TAGGED_QUAL";static char _tmp33F[16]="EXTENSIBLE_QUAL";static char
_tmp340[15]="RESETABLE_QUAL";static char _tmp341[7]="PTR_OP";static char _tmp342[7]="INC_OP";
static char _tmp343[7]="DEC_OP";static char _tmp344[8]="LEFT_OP";static char _tmp345[9]="RIGHT_OP";
static char _tmp346[6]="LE_OP";static char _tmp347[6]="GE_OP";static char _tmp348[6]="EQ_OP";
static char _tmp349[6]="NE_OP";static char _tmp34A[7]="AND_OP";static char _tmp34B[6]="OR_OP";
static char _tmp34C[11]="MUL_ASSIGN";static char _tmp34D[11]="DIV_ASSIGN";static char
_tmp34E[11]="MOD_ASSIGN";static char _tmp34F[11]="ADD_ASSIGN";static char _tmp350[11]="SUB_ASSIGN";
static char _tmp351[12]="LEFT_ASSIGN";static char _tmp352[13]="RIGHT_ASSIGN";static
char _tmp353[11]="AND_ASSIGN";static char _tmp354[11]="XOR_ASSIGN";static char
_tmp355[10]="OR_ASSIGN";static char _tmp356[9]="ELLIPSIS";static char _tmp357[11]="LEFT_RIGHT";
static char _tmp358[12]="COLON_COLON";static char _tmp359[11]="IDENTIFIER";static
char _tmp35A[17]="INTEGER_CONSTANT";static char _tmp35B[7]="STRING";static char
_tmp35C[19]="CHARACTER_CONSTANT";static char _tmp35D[18]="FLOATING_CONSTANT";
static char _tmp35E[9]="TYPE_VAR";static char _tmp35F[13]="TYPEDEF_NAME";static char
_tmp360[16]="QUAL_IDENTIFIER";static char _tmp361[18]="QUAL_TYPEDEF_NAME";static
char _tmp362[10]="ATTRIBUTE";static char _tmp363[4]="';'";static char _tmp364[4]="'{'";
static char _tmp365[4]="'}'";static char _tmp366[4]="','";static char _tmp367[4]="'='";
static char _tmp368[4]="'<'";static char _tmp369[4]="'>'";static char _tmp36A[4]="'('";
static char _tmp36B[4]="')'";static char _tmp36C[4]="'_'";static char _tmp36D[4]="'$'";
static char _tmp36E[4]="':'";static char _tmp36F[4]="'.'";static char _tmp370[4]="'['";
static char _tmp371[4]="']'";static char _tmp372[4]="'*'";static char _tmp373[4]="'@'";
static char _tmp374[4]="'?'";static char _tmp375[4]="'+'";static char _tmp376[4]="'|'";
static char _tmp377[4]="'^'";static char _tmp378[4]="'&'";static char _tmp379[4]="'-'";
static char _tmp37A[4]="'/'";static char _tmp37B[4]="'%'";static char _tmp37C[4]="'~'";
static char _tmp37D[4]="'!'";static char _tmp37E[5]="prog";static char _tmp37F[17]="translation_unit";
static char _tmp380[12]="export_list";static char _tmp381[19]="export_list_values";
static char _tmp382[21]="external_declaration";static char _tmp383[20]="function_definition";
static char _tmp384[21]="function_definition2";static char _tmp385[13]="using_action";
static char _tmp386[15]="unusing_action";static char _tmp387[17]="namespace_action";
static char _tmp388[19]="unnamespace_action";static char _tmp389[12]="declaration";
static char _tmp38A[19]="resetable_qual_opt";static char _tmp38B[17]="declaration_list";
static char _tmp38C[23]="declaration_specifiers";static char _tmp38D[24]="storage_class_specifier";
static char _tmp38E[15]="attributes_opt";static char _tmp38F[11]="attributes";static
char _tmp390[15]="attribute_list";static char _tmp391[10]="attribute";static char
_tmp392[15]="type_specifier";static char _tmp393[25]="type_specifier_notypedef";
static char _tmp394[5]="kind";static char _tmp395[15]="type_qualifier";static char
_tmp396[15]="enum_specifier";static char _tmp397[11]="enum_field";static char
_tmp398[22]="enum_declaration_list";static char _tmp399[26]="struct_or_union_specifier";
static char _tmp39A[16]="type_params_opt";static char _tmp39B[16]="struct_or_union";
static char _tmp39C[24]="struct_declaration_list";static char _tmp39D[25]="struct_declaration_list0";
static char _tmp39E[21]="init_declarator_list";static char _tmp39F[22]="init_declarator_list0";
static char _tmp3A0[16]="init_declarator";static char _tmp3A1[19]="struct_declaration";
static char _tmp3A2[25]="specifier_qualifier_list";static char _tmp3A3[35]="notypedef_specifier_qualifier_list";
static char _tmp3A4[23]="struct_declarator_list";static char _tmp3A5[24]="struct_declarator_list0";
static char _tmp3A6[18]="struct_declarator";static char _tmp3A7[19]="datatype_specifier";
static char _tmp3A8[14]="qual_datatype";static char _tmp3A9[19]="datatypefield_list";
static char _tmp3AA[20]="datatypefield_scope";static char _tmp3AB[14]="datatypefield";
static char _tmp3AC[11]="declarator";static char _tmp3AD[23]="declarator_withtypedef";
static char _tmp3AE[18]="direct_declarator";static char _tmp3AF[30]="direct_declarator_withtypedef";
static char _tmp3B0[8]="pointer";static char _tmp3B1[12]="one_pointer";static char
_tmp3B2[14]="pointer_quals";static char _tmp3B3[13]="pointer_qual";static char
_tmp3B4[23]="pointer_null_and_bound";static char _tmp3B5[14]="pointer_bound";
static char _tmp3B6[18]="zeroterm_qual_opt";static char _tmp3B7[8]="rgn_opt";static
char _tmp3B8[11]="tqual_list";static char _tmp3B9[20]="parameter_type_list";static
char _tmp3BA[9]="type_var";static char _tmp3BB[16]="optional_effect";static char
_tmp3BC[19]="optional_rgn_order";static char _tmp3BD[10]="rgn_order";static char
_tmp3BE[16]="optional_inject";static char _tmp3BF[11]="effect_set";static char
_tmp3C0[14]="atomic_effect";static char _tmp3C1[11]="region_set";static char _tmp3C2[
15]="parameter_list";static char _tmp3C3[22]="parameter_declaration";static char
_tmp3C4[16]="identifier_list";static char _tmp3C5[17]="identifier_list0";static
char _tmp3C6[12]="initializer";static char _tmp3C7[18]="array_initializer";static
char _tmp3C8[17]="initializer_list";static char _tmp3C9[12]="designation";static
char _tmp3CA[16]="designator_list";static char _tmp3CB[11]="designator";static char
_tmp3CC[10]="type_name";static char _tmp3CD[14]="any_type_name";static char _tmp3CE[
15]="type_name_list";static char _tmp3CF[20]="abstract_declarator";static char
_tmp3D0[27]="direct_abstract_declarator";static char _tmp3D1[10]="statement";
static char _tmp3D2[13]="open_exp_opt";static char _tmp3D3[18]="labeled_statement";
static char _tmp3D4[21]="expression_statement";static char _tmp3D5[19]="compound_statement";
static char _tmp3D6[16]="block_item_list";static char _tmp3D7[20]="selection_statement";
static char _tmp3D8[15]="switch_clauses";static char _tmp3D9[20]="iteration_statement";
static char _tmp3DA[15]="jump_statement";static char _tmp3DB[12]="exp_pattern";
static char _tmp3DC[20]="conditional_pattern";static char _tmp3DD[19]="logical_or_pattern";
static char _tmp3DE[20]="logical_and_pattern";static char _tmp3DF[21]="inclusive_or_pattern";
static char _tmp3E0[21]="exclusive_or_pattern";static char _tmp3E1[12]="and_pattern";
static char _tmp3E2[17]="equality_pattern";static char _tmp3E3[19]="relational_pattern";
static char _tmp3E4[14]="shift_pattern";static char _tmp3E5[17]="additive_pattern";
static char _tmp3E6[23]="multiplicative_pattern";static char _tmp3E7[13]="cast_pattern";
static char _tmp3E8[14]="unary_pattern";static char _tmp3E9[16]="postfix_pattern";
static char _tmp3EA[16]="primary_pattern";static char _tmp3EB[8]="pattern";static
char _tmp3EC[19]="tuple_pattern_list";static char _tmp3ED[20]="tuple_pattern_list0";
static char _tmp3EE[14]="field_pattern";static char _tmp3EF[19]="field_pattern_list";
static char _tmp3F0[20]="field_pattern_list0";static char _tmp3F1[11]="expression";
static char _tmp3F2[22]="assignment_expression";static char _tmp3F3[20]="assignment_operator";
static char _tmp3F4[23]="conditional_expression";static char _tmp3F5[20]="constant_expression";
static char _tmp3F6[22]="logical_or_expression";static char _tmp3F7[23]="logical_and_expression";
static char _tmp3F8[24]="inclusive_or_expression";static char _tmp3F9[24]="exclusive_or_expression";
static char _tmp3FA[15]="and_expression";static char _tmp3FB[20]="equality_expression";
static char _tmp3FC[22]="relational_expression";static char _tmp3FD[17]="shift_expression";
static char _tmp3FE[20]="additive_expression";static char _tmp3FF[26]="multiplicative_expression";
static char _tmp400[16]="cast_expression";static char _tmp401[17]="unary_expression";
static char _tmp402[15]="unary_operator";static char _tmp403[19]="postfix_expression";
static char _tmp404[19]="primary_expression";static char _tmp405[25]="argument_expression_list";
static char _tmp406[26]="argument_expression_list0";static char _tmp407[9]="constant";
static char _tmp408[20]="qual_opt_identifier";static char _tmp409[17]="qual_opt_typedef";
static char _tmp40A[18]="struct_union_name";static char _tmp40B[11]="field_name";
static char _tmp40C[12]="right_angle";static struct _dyneither_ptr Cyc_yytname[287]={{
_tmp2EE,_tmp2EE,_tmp2EE + 2},{_tmp2EF,_tmp2EF,_tmp2EF + 6},{_tmp2F0,_tmp2F0,
_tmp2F0 + 12},{_tmp2F1,_tmp2F1,_tmp2F1 + 5},{_tmp2F2,_tmp2F2,_tmp2F2 + 9},{_tmp2F3,
_tmp2F3,_tmp2F3 + 7},{_tmp2F4,_tmp2F4,_tmp2F4 + 7},{_tmp2F5,_tmp2F5,_tmp2F5 + 8},{
_tmp2F6,_tmp2F6,_tmp2F6 + 5},{_tmp2F7,_tmp2F7,_tmp2F7 + 5},{_tmp2F8,_tmp2F8,
_tmp2F8 + 6},{_tmp2F9,_tmp2F9,_tmp2F9 + 4},{_tmp2FA,_tmp2FA,_tmp2FA + 5},{_tmp2FB,
_tmp2FB,_tmp2FB + 6},{_tmp2FC,_tmp2FC,_tmp2FC + 7},{_tmp2FD,_tmp2FD,_tmp2FD + 7},{
_tmp2FE,_tmp2FE,_tmp2FE + 9},{_tmp2FF,_tmp2FF,_tmp2FF + 6},{_tmp300,_tmp300,
_tmp300 + 9},{_tmp301,_tmp301,_tmp301 + 9},{_tmp302,_tmp302,_tmp302 + 7},{_tmp303,
_tmp303,_tmp303 + 6},{_tmp304,_tmp304,_tmp304 + 5},{_tmp305,_tmp305,_tmp305 + 8},{
_tmp306,_tmp306,_tmp306 + 7},{_tmp307,_tmp307,_tmp307 + 7},{_tmp308,_tmp308,
_tmp308 + 9},{_tmp309,_tmp309,_tmp309 + 3},{_tmp30A,_tmp30A,_tmp30A + 5},{_tmp30B,
_tmp30B,_tmp30B + 7},{_tmp30C,_tmp30C,_tmp30C + 6},{_tmp30D,_tmp30D,_tmp30D + 3},{
_tmp30E,_tmp30E,_tmp30E + 4},{_tmp30F,_tmp30F,_tmp30F + 5},{_tmp310,_tmp310,
_tmp310 + 9},{_tmp311,_tmp311,_tmp311 + 6},{_tmp312,_tmp312,_tmp312 + 7},{_tmp313,
_tmp313,_tmp313 + 5},{_tmp314,_tmp314,_tmp314 + 8},{_tmp315,_tmp315,_tmp315 + 4},{
_tmp316,_tmp316,_tmp316 + 6},{_tmp317,_tmp317,_tmp317 + 4},{_tmp318,_tmp318,
_tmp318 + 6},{_tmp319,_tmp319,_tmp319 + 7},{_tmp31A,_tmp31A,_tmp31A + 4},{_tmp31B,
_tmp31B,_tmp31B + 9},{_tmp31C,_tmp31C,_tmp31C + 9},{_tmp31D,_tmp31D,_tmp31D + 6},{
_tmp31E,_tmp31E,_tmp31E + 10},{_tmp31F,_tmp31F,_tmp31F + 9},{_tmp320,_tmp320,
_tmp320 + 8},{_tmp321,_tmp321,_tmp321 + 7},{_tmp322,_tmp322,_tmp322 + 7},{_tmp323,
_tmp323,_tmp323 + 8},{_tmp324,_tmp324,_tmp324 + 7},{_tmp325,_tmp325,_tmp325 + 8},{
_tmp326,_tmp326,_tmp326 + 5},{_tmp327,_tmp327,_tmp327 + 9},{_tmp328,_tmp328,
_tmp328 + 6},{_tmp329,_tmp329,_tmp329 + 7},{_tmp32A,_tmp32A,_tmp32A + 5},{_tmp32B,
_tmp32B,_tmp32B + 8},{_tmp32C,_tmp32C,_tmp32C + 13},{_tmp32D,_tmp32D,_tmp32D + 4},{
_tmp32E,_tmp32E,_tmp32E + 16},{_tmp32F,_tmp32F,_tmp32F + 14},{_tmp330,_tmp330,
_tmp330 + 12},{_tmp331,_tmp331,_tmp331 + 7},{_tmp332,_tmp332,_tmp332 + 8},{_tmp333,
_tmp333,_tmp333 + 12},{_tmp334,_tmp334,_tmp334 + 6},{_tmp335,_tmp335,_tmp335 + 8},{
_tmp336,_tmp336,_tmp336 + 8},{_tmp337,_tmp337,_tmp337 + 10},{_tmp338,_tmp338,
_tmp338 + 9},{_tmp339,_tmp339,_tmp339 + 13},{_tmp33A,_tmp33A,_tmp33A + 10},{_tmp33B,
_tmp33B,_tmp33B + 9},{_tmp33C,_tmp33C,_tmp33C + 13},{_tmp33D,_tmp33D,_tmp33D + 14},{
_tmp33E,_tmp33E,_tmp33E + 12},{_tmp33F,_tmp33F,_tmp33F + 16},{_tmp340,_tmp340,
_tmp340 + 15},{_tmp341,_tmp341,_tmp341 + 7},{_tmp342,_tmp342,_tmp342 + 7},{_tmp343,
_tmp343,_tmp343 + 7},{_tmp344,_tmp344,_tmp344 + 8},{_tmp345,_tmp345,_tmp345 + 9},{
_tmp346,_tmp346,_tmp346 + 6},{_tmp347,_tmp347,_tmp347 + 6},{_tmp348,_tmp348,
_tmp348 + 6},{_tmp349,_tmp349,_tmp349 + 6},{_tmp34A,_tmp34A,_tmp34A + 7},{_tmp34B,
_tmp34B,_tmp34B + 6},{_tmp34C,_tmp34C,_tmp34C + 11},{_tmp34D,_tmp34D,_tmp34D + 11},{
_tmp34E,_tmp34E,_tmp34E + 11},{_tmp34F,_tmp34F,_tmp34F + 11},{_tmp350,_tmp350,
_tmp350 + 11},{_tmp351,_tmp351,_tmp351 + 12},{_tmp352,_tmp352,_tmp352 + 13},{
_tmp353,_tmp353,_tmp353 + 11},{_tmp354,_tmp354,_tmp354 + 11},{_tmp355,_tmp355,
_tmp355 + 10},{_tmp356,_tmp356,_tmp356 + 9},{_tmp357,_tmp357,_tmp357 + 11},{_tmp358,
_tmp358,_tmp358 + 12},{_tmp359,_tmp359,_tmp359 + 11},{_tmp35A,_tmp35A,_tmp35A + 17},{
_tmp35B,_tmp35B,_tmp35B + 7},{_tmp35C,_tmp35C,_tmp35C + 19},{_tmp35D,_tmp35D,
_tmp35D + 18},{_tmp35E,_tmp35E,_tmp35E + 9},{_tmp35F,_tmp35F,_tmp35F + 13},{_tmp360,
_tmp360,_tmp360 + 16},{_tmp361,_tmp361,_tmp361 + 18},{_tmp362,_tmp362,_tmp362 + 10},{
_tmp363,_tmp363,_tmp363 + 4},{_tmp364,_tmp364,_tmp364 + 4},{_tmp365,_tmp365,
_tmp365 + 4},{_tmp366,_tmp366,_tmp366 + 4},{_tmp367,_tmp367,_tmp367 + 4},{_tmp368,
_tmp368,_tmp368 + 4},{_tmp369,_tmp369,_tmp369 + 4},{_tmp36A,_tmp36A,_tmp36A + 4},{
_tmp36B,_tmp36B,_tmp36B + 4},{_tmp36C,_tmp36C,_tmp36C + 4},{_tmp36D,_tmp36D,
_tmp36D + 4},{_tmp36E,_tmp36E,_tmp36E + 4},{_tmp36F,_tmp36F,_tmp36F + 4},{_tmp370,
_tmp370,_tmp370 + 4},{_tmp371,_tmp371,_tmp371 + 4},{_tmp372,_tmp372,_tmp372 + 4},{
_tmp373,_tmp373,_tmp373 + 4},{_tmp374,_tmp374,_tmp374 + 4},{_tmp375,_tmp375,
_tmp375 + 4},{_tmp376,_tmp376,_tmp376 + 4},{_tmp377,_tmp377,_tmp377 + 4},{_tmp378,
_tmp378,_tmp378 + 4},{_tmp379,_tmp379,_tmp379 + 4},{_tmp37A,_tmp37A,_tmp37A + 4},{
_tmp37B,_tmp37B,_tmp37B + 4},{_tmp37C,_tmp37C,_tmp37C + 4},{_tmp37D,_tmp37D,
_tmp37D + 4},{_tmp37E,_tmp37E,_tmp37E + 5},{_tmp37F,_tmp37F,_tmp37F + 17},{_tmp380,
_tmp380,_tmp380 + 12},{_tmp381,_tmp381,_tmp381 + 19},{_tmp382,_tmp382,_tmp382 + 21},{
_tmp383,_tmp383,_tmp383 + 20},{_tmp384,_tmp384,_tmp384 + 21},{_tmp385,_tmp385,
_tmp385 + 13},{_tmp386,_tmp386,_tmp386 + 15},{_tmp387,_tmp387,_tmp387 + 17},{
_tmp388,_tmp388,_tmp388 + 19},{_tmp389,_tmp389,_tmp389 + 12},{_tmp38A,_tmp38A,
_tmp38A + 19},{_tmp38B,_tmp38B,_tmp38B + 17},{_tmp38C,_tmp38C,_tmp38C + 23},{
_tmp38D,_tmp38D,_tmp38D + 24},{_tmp38E,_tmp38E,_tmp38E + 15},{_tmp38F,_tmp38F,
_tmp38F + 11},{_tmp390,_tmp390,_tmp390 + 15},{_tmp391,_tmp391,_tmp391 + 10},{
_tmp392,_tmp392,_tmp392 + 15},{_tmp393,_tmp393,_tmp393 + 25},{_tmp394,_tmp394,
_tmp394 + 5},{_tmp395,_tmp395,_tmp395 + 15},{_tmp396,_tmp396,_tmp396 + 15},{_tmp397,
_tmp397,_tmp397 + 11},{_tmp398,_tmp398,_tmp398 + 22},{_tmp399,_tmp399,_tmp399 + 26},{
_tmp39A,_tmp39A,_tmp39A + 16},{_tmp39B,_tmp39B,_tmp39B + 16},{_tmp39C,_tmp39C,
_tmp39C + 24},{_tmp39D,_tmp39D,_tmp39D + 25},{_tmp39E,_tmp39E,_tmp39E + 21},{
_tmp39F,_tmp39F,_tmp39F + 22},{_tmp3A0,_tmp3A0,_tmp3A0 + 16},{_tmp3A1,_tmp3A1,
_tmp3A1 + 19},{_tmp3A2,_tmp3A2,_tmp3A2 + 25},{_tmp3A3,_tmp3A3,_tmp3A3 + 35},{
_tmp3A4,_tmp3A4,_tmp3A4 + 23},{_tmp3A5,_tmp3A5,_tmp3A5 + 24},{_tmp3A6,_tmp3A6,
_tmp3A6 + 18},{_tmp3A7,_tmp3A7,_tmp3A7 + 19},{_tmp3A8,_tmp3A8,_tmp3A8 + 14},{
_tmp3A9,_tmp3A9,_tmp3A9 + 19},{_tmp3AA,_tmp3AA,_tmp3AA + 20},{_tmp3AB,_tmp3AB,
_tmp3AB + 14},{_tmp3AC,_tmp3AC,_tmp3AC + 11},{_tmp3AD,_tmp3AD,_tmp3AD + 23},{
_tmp3AE,_tmp3AE,_tmp3AE + 18},{_tmp3AF,_tmp3AF,_tmp3AF + 30},{_tmp3B0,_tmp3B0,
_tmp3B0 + 8},{_tmp3B1,_tmp3B1,_tmp3B1 + 12},{_tmp3B2,_tmp3B2,_tmp3B2 + 14},{_tmp3B3,
_tmp3B3,_tmp3B3 + 13},{_tmp3B4,_tmp3B4,_tmp3B4 + 23},{_tmp3B5,_tmp3B5,_tmp3B5 + 14},{
_tmp3B6,_tmp3B6,_tmp3B6 + 18},{_tmp3B7,_tmp3B7,_tmp3B7 + 8},{_tmp3B8,_tmp3B8,
_tmp3B8 + 11},{_tmp3B9,_tmp3B9,_tmp3B9 + 20},{_tmp3BA,_tmp3BA,_tmp3BA + 9},{_tmp3BB,
_tmp3BB,_tmp3BB + 16},{_tmp3BC,_tmp3BC,_tmp3BC + 19},{_tmp3BD,_tmp3BD,_tmp3BD + 10},{
_tmp3BE,_tmp3BE,_tmp3BE + 16},{_tmp3BF,_tmp3BF,_tmp3BF + 11},{_tmp3C0,_tmp3C0,
_tmp3C0 + 14},{_tmp3C1,_tmp3C1,_tmp3C1 + 11},{_tmp3C2,_tmp3C2,_tmp3C2 + 15},{
_tmp3C3,_tmp3C3,_tmp3C3 + 22},{_tmp3C4,_tmp3C4,_tmp3C4 + 16},{_tmp3C5,_tmp3C5,
_tmp3C5 + 17},{_tmp3C6,_tmp3C6,_tmp3C6 + 12},{_tmp3C7,_tmp3C7,_tmp3C7 + 18},{
_tmp3C8,_tmp3C8,_tmp3C8 + 17},{_tmp3C9,_tmp3C9,_tmp3C9 + 12},{_tmp3CA,_tmp3CA,
_tmp3CA + 16},{_tmp3CB,_tmp3CB,_tmp3CB + 11},{_tmp3CC,_tmp3CC,_tmp3CC + 10},{
_tmp3CD,_tmp3CD,_tmp3CD + 14},{_tmp3CE,_tmp3CE,_tmp3CE + 15},{_tmp3CF,_tmp3CF,
_tmp3CF + 20},{_tmp3D0,_tmp3D0,_tmp3D0 + 27},{_tmp3D1,_tmp3D1,_tmp3D1 + 10},{
_tmp3D2,_tmp3D2,_tmp3D2 + 13},{_tmp3D3,_tmp3D3,_tmp3D3 + 18},{_tmp3D4,_tmp3D4,
_tmp3D4 + 21},{_tmp3D5,_tmp3D5,_tmp3D5 + 19},{_tmp3D6,_tmp3D6,_tmp3D6 + 16},{
_tmp3D7,_tmp3D7,_tmp3D7 + 20},{_tmp3D8,_tmp3D8,_tmp3D8 + 15},{_tmp3D9,_tmp3D9,
_tmp3D9 + 20},{_tmp3DA,_tmp3DA,_tmp3DA + 15},{_tmp3DB,_tmp3DB,_tmp3DB + 12},{
_tmp3DC,_tmp3DC,_tmp3DC + 20},{_tmp3DD,_tmp3DD,_tmp3DD + 19},{_tmp3DE,_tmp3DE,
_tmp3DE + 20},{_tmp3DF,_tmp3DF,_tmp3DF + 21},{_tmp3E0,_tmp3E0,_tmp3E0 + 21},{
_tmp3E1,_tmp3E1,_tmp3E1 + 12},{_tmp3E2,_tmp3E2,_tmp3E2 + 17},{_tmp3E3,_tmp3E3,
_tmp3E3 + 19},{_tmp3E4,_tmp3E4,_tmp3E4 + 14},{_tmp3E5,_tmp3E5,_tmp3E5 + 17},{
_tmp3E6,_tmp3E6,_tmp3E6 + 23},{_tmp3E7,_tmp3E7,_tmp3E7 + 13},{_tmp3E8,_tmp3E8,
_tmp3E8 + 14},{_tmp3E9,_tmp3E9,_tmp3E9 + 16},{_tmp3EA,_tmp3EA,_tmp3EA + 16},{
_tmp3EB,_tmp3EB,_tmp3EB + 8},{_tmp3EC,_tmp3EC,_tmp3EC + 19},{_tmp3ED,_tmp3ED,
_tmp3ED + 20},{_tmp3EE,_tmp3EE,_tmp3EE + 14},{_tmp3EF,_tmp3EF,_tmp3EF + 19},{
_tmp3F0,_tmp3F0,_tmp3F0 + 20},{_tmp3F1,_tmp3F1,_tmp3F1 + 11},{_tmp3F2,_tmp3F2,
_tmp3F2 + 22},{_tmp3F3,_tmp3F3,_tmp3F3 + 20},{_tmp3F4,_tmp3F4,_tmp3F4 + 23},{
_tmp3F5,_tmp3F5,_tmp3F5 + 20},{_tmp3F6,_tmp3F6,_tmp3F6 + 22},{_tmp3F7,_tmp3F7,
_tmp3F7 + 23},{_tmp3F8,_tmp3F8,_tmp3F8 + 24},{_tmp3F9,_tmp3F9,_tmp3F9 + 24},{
_tmp3FA,_tmp3FA,_tmp3FA + 15},{_tmp3FB,_tmp3FB,_tmp3FB + 20},{_tmp3FC,_tmp3FC,
_tmp3FC + 22},{_tmp3FD,_tmp3FD,_tmp3FD + 17},{_tmp3FE,_tmp3FE,_tmp3FE + 20},{
_tmp3FF,_tmp3FF,_tmp3FF + 26},{_tmp400,_tmp400,_tmp400 + 16},{_tmp401,_tmp401,
_tmp401 + 17},{_tmp402,_tmp402,_tmp402 + 15},{_tmp403,_tmp403,_tmp403 + 19},{
_tmp404,_tmp404,_tmp404 + 19},{_tmp405,_tmp405,_tmp405 + 25},{_tmp406,_tmp406,
_tmp406 + 26},{_tmp407,_tmp407,_tmp407 + 9},{_tmp408,_tmp408,_tmp408 + 20},{_tmp409,
_tmp409,_tmp409 + 17},{_tmp40A,_tmp40A,_tmp40A + 18},{_tmp40B,_tmp40B,_tmp40B + 11},{
_tmp40C,_tmp40C,_tmp40C + 12}};static short Cyc_yyr1[510]={0,144,145,145,145,145,
145,145,145,145,145,145,146,146,147,147,148,148,148,149,149,149,149,150,150,151,
152,153,154,155,155,155,155,155,155,155,156,156,157,157,158,158,158,158,158,158,
158,158,158,158,159,159,159,159,159,159,159,160,160,161,162,162,163,163,163,163,
163,163,164,164,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,
165,165,165,165,165,165,166,167,167,167,168,168,168,169,169,170,170,170,171,171,
171,171,171,172,172,173,173,174,174,175,175,176,177,177,178,178,179,180,180,180,
180,180,180,181,181,181,181,181,181,182,183,183,184,184,184,185,185,185,186,186,
187,187,187,187,188,188,188,189,189,190,190,191,191,192,192,192,192,192,192,192,
192,192,192,193,193,193,193,193,193,193,193,193,193,193,194,194,195,196,196,197,
197,197,197,197,197,197,197,198,198,198,199,199,200,200,200,201,201,201,202,202,
203,203,203,203,204,204,205,205,206,206,207,207,208,208,209,209,210,210,210,210,
211,211,212,212,213,213,213,214,215,215,216,216,217,217,217,217,218,218,218,218,
219,220,220,221,221,222,222,223,223,223,223,223,224,224,225,225,225,226,226,226,
226,226,226,226,226,226,226,226,227,227,227,227,227,227,227,228,228,229,230,230,
231,231,232,232,232,232,232,232,233,233,233,233,233,233,234,234,234,234,234,234,
235,235,235,235,235,235,235,235,235,235,235,235,235,235,236,236,236,236,236,236,
236,236,237,238,238,239,239,240,240,241,241,242,242,243,243,244,244,244,245,245,
245,245,245,246,246,246,247,247,247,248,248,248,248,249,249,250,250,250,250,250,
250,250,251,252,253,253,253,253,253,253,253,253,253,253,253,253,253,253,253,254,
254,254,255,255,256,256,257,257,257,258,258,259,259,260,260,260,261,261,261,261,
261,261,261,261,261,261,261,262,262,262,262,262,262,262,263,264,264,265,265,266,
266,267,267,268,268,269,269,269,270,270,270,270,270,271,271,271,272,272,272,273,
273,273,273,274,274,275,275,275,275,275,275,275,275,275,275,275,275,275,275,275,
275,275,275,275,275,276,276,276,277,277,277,277,277,277,277,277,277,277,278,278,
278,278,278,278,278,278,278,279,280,280,281,281,281,281,282,282,283,283,284,284,
285,285,286,286};static short Cyc_yyr2[510]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,1,1,
2,3,3,4,3,4,2,1,2,1,2,3,5,3,7,5,8,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,
6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,4,6,4,1,4,1,1,1,1,5,2,4,
1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,
2,3,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,
2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,
1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,
3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,
6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,
1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,1,3,4,4,5,4,
2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,
5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,
2,6,6,5,4,6,9,11,4,6,6,4,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,
1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[1032]={0,18,50,51,52,53,55,70,73,
74,75,76,77,78,79,80,93,94,95,111,112,46,0,0,56,0,0,144,0,90,0,0,0,0,0,0,0,37,
500,210,502,501,503,0,0,71,0,196,196,195,1,0,16,0,0,17,0,0,40,48,42,68,44,81,82,
0,83,0,36,155,0,180,183,84,159,109,54,53,47,0,97,499,0,500,496,497,498,109,0,370,
0,0,0,0,233,0,372,373,25,27,0,0,0,0,0,0,0,0,145,0,0,0,0,0,0,0,193,194,2,0,0,0,0,
0,29,0,117,118,120,41,49,43,45,113,504,505,109,109,36,38,36,0,19,0,212,0,168,156,
181,190,189,0,0,187,188,191,192,201,183,0,69,0,54,101,0,99,0,500,380,0,0,0,0,0,0,
0,0,0,0,0,0,109,0,0,0,0,0,486,0,0,0,0,0,473,471,472,0,397,399,413,421,423,425,
427,429,431,434,439,442,445,449,0,451,474,485,484,0,381,379,32,0,0,109,0,0,0,127,
123,125,251,253,0,0,9,10,0,0,0,109,506,507,211,92,0,0,160,72,231,0,228,0,3,0,5,0,
281,0,30,0,0,36,20,0,114,115,0,108,0,142,0,0,0,0,0,0,0,0,0,0,0,0,500,284,286,0,
292,288,0,290,274,275,276,0,277,278,279,0,39,21,120,258,0,218,234,0,0,214,212,0,
198,0,0,0,203,57,202,184,0,0,102,98,0,0,374,0,0,394,0,0,0,0,247,390,395,0,392,0,
459,0,415,449,0,416,417,0,0,0,0,0,0,0,0,0,0,452,453,36,0,0,0,455,456,454,0,371,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,403,404,405,406,407,408,409,410,411,412,
402,0,457,0,480,481,0,0,0,488,0,109,387,388,0,385,0,235,0,0,0,0,254,226,0,128,
133,129,131,124,126,212,0,260,252,261,509,508,0,86,89,0,87,0,91,107,63,62,0,60,
161,212,230,157,260,232,169,170,0,85,197,26,0,28,0,0,0,0,119,121,237,236,22,104,
116,0,0,0,135,136,138,0,109,109,150,0,0,0,0,0,0,36,0,321,322,323,0,0,325,0,0,0,0,
293,289,120,291,287,285,0,167,219,0,0,0,225,213,220,164,0,0,0,214,166,200,199,
162,198,0,0,204,58,110,0,103,420,100,96,383,384,250,0,391,246,248,378,0,0,0,0,
238,242,0,0,0,0,0,0,0,0,0,0,0,0,0,0,487,494,0,493,398,422,0,424,426,428,430,432,
433,437,438,435,436,440,441,443,444,446,447,448,401,400,479,476,0,478,0,0,0,375,
0,382,31,0,376,0,0,255,134,130,132,0,214,0,198,0,262,0,212,0,273,257,0,0,109,0,0,
0,127,0,109,0,212,0,179,158,229,4,6,0,34,0,0,139,122,0,0,214,143,152,151,0,0,146,
0,0,0,300,0,0,0,0,0,320,324,0,0,0,0,283,36,23,259,212,0,222,0,0,215,0,165,218,
206,163,186,185,204,182,0,7,0,249,393,396,458,0,0,239,0,243,463,0,0,0,0,0,467,
470,0,0,0,0,0,450,490,0,0,477,475,0,0,386,389,377,256,227,269,0,263,264,198,0,0,
214,198,0,88,0,214,0,0,0,59,61,0,171,0,0,214,0,198,0,0,0,137,140,113,141,153,150,
150,0,0,0,0,0,0,0,0,0,0,0,0,0,300,326,0,0,24,214,0,223,221,0,214,0,205,0,8,0,0,0,
240,244,0,0,0,0,418,419,462,479,478,492,0,495,414,489,491,0,268,266,272,271,0,
265,198,0,113,0,66,64,65,172,178,175,0,177,173,198,0,33,0,0,149,148,294,300,0,0,
0,0,0,0,328,329,331,333,335,337,339,341,344,349,352,355,359,361,368,369,0,36,297,
306,0,0,0,0,0,0,0,0,0,0,327,280,208,224,216,207,212,13,0,0,461,460,0,245,464,0,0,
469,468,482,0,270,267,35,0,0,176,174,282,106,0,0,0,300,0,365,0,0,362,36,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,363,301,0,308,0,0,0,316,0,0,0,0,0,0,299,0,214,12,
14,0,0,0,0,483,105,0,154,295,296,0,0,0,0,300,302,332,0,334,336,338,340,342,343,
347,348,345,346,350,351,353,354,356,357,358,0,307,309,310,0,318,317,0,312,0,0,0,
217,209,15,0,0,0,0,298,364,0,360,303,0,36,311,319,313,314,0,241,465,0,67,0,0,330,
300,304,315,0,367,366,305,466,0,0,0};static short Cyc_yydefgoto[143]={1029,50,685,
877,51,52,285,53,457,54,459,55,56,140,57,58,525,226,443,444,227,61,241,228,63,
163,164,64,160,65,262,263,125,126,127,264,229,424,472,473,474,66,67,646,647,648,
68,475,69,448,70,71,157,158,72,116,521,314,682,606,73,607,515,673,507,511,512,
419,307,249,93,94,546,465,547,329,330,331,230,300,301,608,430,288,461,289,290,
291,292,293,761,294,295,838,839,840,841,842,843,844,845,846,847,848,849,850,851,
852,853,332,409,410,333,334,335,296,197,396,198,531,199,200,201,202,203,204,205,
206,207,208,209,210,211,212,213,563,564,214,215,75,878,242,434};static short Cyc_yypact[
1032]={2762,- -32768,- -32768,- -32768,- -32768,- 78,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,3216,456,4445,- -32768,- 60,- 10,- -32768,- 6,7,39,73,51,93,94,325,192,-
-32768,- -32768,167,- -32768,- -32768,- -32768,159,634,194,185,132,132,- -32768,-
-32768,2627,- -32768,370,720,- -32768,313,189,3216,3216,3216,- -32768,3216,- -32768,
- -32768,656,- -32768,- 60,3136,165,107,726,815,- -32768,- -32768,198,271,295,- -32768,
- 60,311,- -32768,4538,242,- -32768,- -32768,- -32768,198,5805,- -32768,312,347,4538,
344,354,346,- -32768,70,- -32768,- -32768,3364,3364,2627,2627,3364,387,5805,520,-
-32768,277,397,585,386,277,3586,5805,- -32768,- -32768,- -32768,2627,2896,2627,2896,
42,- -32768,415,434,- -32768,3056,- -32768,- -32768,- -32768,- -32768,3586,- -32768,-
-32768,198,222,1653,- -32768,3136,189,- -32768,3364,3290,4209,- -32768,165,- -32768,
- -32768,- -32768,440,442,- -32768,- -32768,- -32768,- -32768,158,815,3364,- -32768,
2896,- -32768,457,475,499,- 60,47,- -32768,4538,221,1012,5982,498,5805,5846,507,519,
536,551,553,198,560,599,601,6074,6074,- -32768,2212,603,6115,6115,6115,- -32768,-
-32768,- -32768,204,- -32768,- -32768,- 22,612,641,598,671,663,140,710,- 69,261,-
-32768,505,6115,349,13,- -32768,689,3596,719,- -32768,- -32768,728,5805,198,3596,
731,199,3586,3660,3586,578,- -32768,85,85,- -32768,- -32768,181,717,338,198,- -32768,
- -32768,- -32768,- -32768,44,744,- -32768,- -32768,668,448,- -32768,758,- -32768,766,-
-32768,782,789,791,- -32768,585,4305,3136,- -32768,793,3586,- -32768,600,796,- 60,
806,805,383,811,3729,813,829,821,822,4401,3729,233,816,814,- -32768,- -32768,817,
1794,1794,189,1794,- -32768,- -32768,- -32768,824,- -32768,- -32768,- -32768,- 67,-
-32768,- -32768,825,810,126,842,- -32768,80,826,830,381,827,752,819,3364,5805,-
-32768,838,- -32768,- -32768,126,834,- 60,- -32768,5805,843,- -32768,844,845,- -32768,
277,5805,4538,287,- -32768,- -32768,- -32768,847,850,2212,- -32768,3586,- -32768,-
-32768,3825,- -32768,871,5805,5805,5805,5805,5805,848,5805,3586,228,2212,- -32768,
- -32768,1935,840,530,5805,- -32768,- -32768,- -32768,5805,- -32768,6115,5805,6115,
6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,
5805,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,5805,- -32768,277,- -32768,- -32768,4494,277,5805,- -32768,849,198,-
-32768,- -32768,851,854,4538,- -32768,403,1012,853,3364,- -32768,855,860,- -32768,
3660,3660,3660,- -32768,- -32768,1463,4587,148,- -32768,251,- -32768,- -32768,80,-
-32768,- -32768,3364,- -32768,874,- -32768,862,- -32768,858,859,863,- -32768,1156,-
-32768,321,412,- -32768,- -32768,- -32768,3586,- -32768,- -32768,- -32768,2627,- -32768,
2627,878,869,864,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,738,
5805,875,879,- -32768,867,474,198,198,837,5805,5805,876,883,5805,961,2076,885,-
-32768,- -32768,- -32768,414,962,- -32768,4683,5805,3729,2348,- -32768,- -32768,3056,
- -32768,- -32768,- -32768,3364,- -32768,- -32768,3586,881,258,- -32768,- -32768,868,-
-32768,80,882,3512,830,- -32768,- -32768,- -32768,- -32768,752,- 26,884,846,- -32768,-
-32768,2492,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,886,- -32768,
- -32768,- -32768,- -32768,3226,887,896,899,- -32768,- -32768,764,4305,537,898,900,
902,543,3586,546,894,613,901,904,5941,- -32768,- -32768,903,905,- -32768,612,83,641,
598,671,663,140,140,710,710,710,710,- 69,- 69,261,261,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,906,- -32768,48,3364,4113,- -32768,6233,- -32768,-
-32768,908,- -32768,98,917,- -32768,- -32768,- -32768,- -32768,907,830,909,752,913,
251,3364,3438,4776,- -32768,- -32768,85,912,198,759,911,44,2976,915,198,3364,3290,
4872,- -32768,321,- -32768,- -32768,- -32768,914,- -32768,930,715,- -32768,- -32768,600,
5805,830,- -32768,- -32768,- -32768,926,- 60,458,563,580,5805,873,606,923,4968,5061,
492,- -32768,- -32768,933,935,928,627,- -32768,3136,- -32768,810,937,3364,- -32768,
938,80,- -32768,939,- -32768,152,- -32768,- -32768,- -32768,- -32768,846,- -32768,943,-
-32768,2627,- -32768,- -32768,- -32768,946,306,944,- -32768,3921,- -32768,- -32768,
5805,1040,5805,5846,942,- -32768,- -32768,277,277,946,945,4113,- -32768,- -32768,
5805,5805,- -32768,- -32768,126,778,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,947,- -32768,- -32768,752,126,948,830,752,940,- -32768,5805,830,636,949,950,
- -32768,- -32768,951,- -32768,126,952,830,955,752,967,5805,982,- -32768,- -32768,
3586,- -32768,976,86,837,3729,983,977,3522,975,986,3729,5805,5154,556,5247,574,
5340,873,- -32768,989,992,- -32768,830,105,- -32768,- -32768,956,830,3586,- -32768,
510,- -32768,985,987,5805,- -32768,- -32768,4305,659,990,993,- -32768,871,- -32768,
996,999,- -32768,798,- -32768,- -32768,- -32768,- -32768,4113,- -32768,- -32768,- -32768,
- -32768,1000,- -32768,752,619,3586,1007,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,1004,- -32768,- -32768,752,666,- -32768,998,3586,- -32768,- -32768,1090,873,
1013,6207,1008,2348,6115,1009,- -32768,88,- -32768,1043,1010,783,832,243,841,514,
377,- -32768,- -32768,- -32768,- -32768,1048,6115,1935,- -32768,- -32768,669,3729,679,
5433,3729,683,5526,5619,645,1024,- -32768,- -32768,- -32768,- -32768,1025,- -32768,
937,- -32768,1028,673,- -32768,- -32768,174,- -32768,- -32768,3586,1123,- -32768,-
-32768,- -32768,4017,- -32768,- -32768,- -32768,1030,1031,- -32768,- -32768,- -32768,-
-32768,686,3729,1033,873,2212,- -32768,3586,1034,- -32768,1327,6115,5805,6115,6115,
6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,5805,
- -32768,- -32768,1036,- -32768,3729,3729,690,- -32768,3729,3729,699,3729,707,5712,-
-32768,80,830,- -32768,- -32768,520,5805,1035,1037,- -32768,- -32768,1047,- -32768,-
-32768,- -32768,1044,1054,1060,6115,873,- -32768,612,266,641,598,598,663,140,140,
710,710,710,710,- 69,- 69,261,261,- -32768,- -32768,- -32768,322,- -32768,- -32768,-
-32768,3729,- -32768,- -32768,3729,- -32768,3729,3729,716,- -32768,- -32768,- -32768,
812,1056,3586,1057,- -32768,946,539,- -32768,- -32768,5805,1327,- -32768,- -32768,-
-32768,- -32768,3729,- -32768,- -32768,1058,- -32768,1059,1061,- -32768,873,- -32768,-
-32768,1062,- -32768,- -32768,- -32768,- -32768,1189,1190,- -32768};static short Cyc_yypgoto[
143]={- -32768,136,- -32768,244,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- 51,- -32768,- 105,22,- -32768,- -32768,0,569,- -32768,55,- 170,1081,5,- -32768,
- -32768,- 134,- -32768,138,1161,- 715,- -32768,- -32768,- -32768,941,953,681,453,-
-32768,- -32768,558,- -32768,- -32768,179,- -32768,- -32768,95,- 199,1130,- 397,14,-
-32768,1045,- -32768,- -32768,1153,- 342,- -32768,523,- 138,- 62,- 111,- 416,262,532,538,
- 420,- 471,- 106,- 411,- 124,- -32768,- 222,- 153,- 548,- 339,- -32768,888,- 154,144,- 120,-
58,- 318,56,- -32768,- -32768,- -32768,4,- 270,- -32768,- 349,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,1,988,- -32768,674,795,- -32768,292,614,-
-32768,- 173,- 293,- 150,- 358,- 355,- 365,852,- 356,- 344,- 239,- 337,- 305,- 181,714,460,
865,- -32768,- 352,- -32768,- 4,374,- 61,46,- 298,- 25};static short Cyc_yytable[6372]={
59,339,548,569,135,62,305,566,248,360,361,362,568,616,571,498,499,139,501,96,308,
59,342,260,95,343,62,572,573,535,397,76,322,306,357,536,827,464,671,317,578,579,
631,78,111,715,135,38,447,589,503,59,630,363,41,60,62,422,59,59,59,441,59,62,62,
62,379,62,59,146,380,365,142,62,580,581,60,139,96,630,129,130,131,168,132,148,
559,286,96,297,141,644,645,218,674,315,668,99,893,679,587,677,59,59,590,631,60,
62,62,433,611,136,366,60,60,60,100,60,404,59,59,59,59,60,62,62,62,62,59,101,720,
611,261,62,574,575,576,577,59,112,59,508,662,62,298,62,405,146,530,255,141,442,
128,238,169,530,102,60,60,799,287,59,141,418,256,96,62,96,363,170,323,429,431,
104,60,60,60,60,638,713,678,909,542,60,543,529,452,118,222,450,103,722,39,60,223,
60,517,556,509,558,582,583,584,363,452,- 147,244,435,432,297,437,711,96,431,38,
105,60,408,106,96,433,41,910,719,408,171,751,421,373,374,872,44,423,433,286,286,
299,286,233,234,433,108,510,428,231,232,504,624,235,432,115,422,422,422,548,251,
252,253,254,506,59,449,375,376,467,62,81,724,431,304,39,624,426,109,266,268,505,
- 212,427,476,- 212,43,141,110,313,59,59,143,59,144,62,62,527,62,363,145,38,318,
530,758,113,436,949,41,432,286,124,287,287,114,287,39,809,44,526,60,433,813,417,
349,159,47,48,49,363,96,694,821,485,364,537,917,918,324,492,38,84,187,85,86,60,
60,41,60,159,19,20,325,750,169,493,267,353,299,789,189,59,494,871,- 234,414,62,-
234,874,170,919,920,43,548,875,39,510,123,612,74,613,440,670,287,708,196,614,500,
806,239,625,363,810,452,161,240,248,785,381,1007,665,80,97,237,98,700,382,383,
823,162,796,797,96,538,452,96,60,596,239,784,452,327,328,74,240,868,421,421,421,
739,74,423,423,423,166,615,74,398,399,400,656,216,43,739,134,428,137,363,626,74,
627,623,418,629,139,1008,628,510,165,217,523,97,59,363,59,449,219,62,439,62,789,
97,221,530,891,637,788,714,401,220,727,74,74,402,403,358,134,896,901,476,74,59,
119,120,742,38,62,726,74,74,74,74,41,304,236,59,516,728,744,666,62,741,481,141,
925,482,245,60,413,60,74,743,926,927,38,597,243,141,363,674,40,41,42,59,996,681,
659,257,62,363,74,446,96,801,418,165,60,427,97,593,97,793,967,968,794,789,964,
664,958,258,60,966,774,969,963,600,384,625,38,311,779,312,882,453,491,41,970,971,
454,79,754,452,319,755,452,617,38,788,60,976,977,930,40,41,42,97,96,731,632,320,
633,717,97,470,385,386,387,388,389,390,391,392,393,394,768,510,615,363,1006,297,
642,643,38,978,979,321,451,338,40,41,42,395,38,358,876,629,344,74,40,41,42,549,
476,962,451,553,478,555,345,483,358,239,1018,667,923,363,476,240,924,476,561,907,
363,567,1022,346,74,695,363,684,59,363,788,699,773,62,701,1027,862,929,347,363,
348,972,973,974,975,906,363,350,59,681,141,756,802,62,865,38,165,363,591,703,399,
400,41,363,807,426,97,367,757,1019,38,427,44,47,48,49,40,41,42,819,47,48,49,60,
135,899,351,470,352,363,359,471,250,950,762,47,48,49,369,892,401,1021,363,60,38,
704,403,980,981,982,363,41,959,43,960,772,371,372,96,814,733,44,310,854,815,942,
38,945,363,47,48,49,40,41,42,649,650,133,38,653,368,657,363,1005,40,41,42,883,97,
363,663,97,363,947,897,446,948,931,247,377,378,427,363,47,48,49,363,933,286,453,
406,937,370,363,955,831,775,265,987,519,520,858,363,451,783,38,451,990,247,411,
363,40,41,42,74,992,74,1020,412,363,121,122,470,438,1013,644,645,451,38,1016,47,
48,49,451,40,41,42,43,416,59,286,47,48,49,62,470,16,17,18,734,735,736,445,47,48,
49,466,603,604,605,455,287,149,150,151,510,692,693,456,337,135,340,340,152,153,
154,155,156,759,760,803,804,354,355,458,74,462,340,340,340,420,59,425,460,60,468,
62,477,97,932,888,889,936,913,914,915,916,479,340,524,921,922,480,287,1014,363,
829,830,484,487,486,488,489,495,497,496,502,265,433,259,765,767,506,522,513,518,
528,43,466,956,286,514,550,551,552,532,60,365,560,540,533,534,97,541,592,554,562,
595,601,594,565,599,602,619,618,620,622,621,634,635,636,790,985,986,654,639,988,
989,641,991,451,585,640,651,652,658,672,660,669,691,675,59,680,586,451,689,62,
451,562,690,686,696,702,697,753,698,706,812,710,705,718,709,39,287,712,721,732,
723,340,737,748,747,825,740,610,340,1009,725,752,1010,763,1011,1012,81,769,770,
771,304,859,861,776,864,74,867,782,778,60,707,791,786,795,873,1023,798,811,805,
808,816,817,818,820,881,340,822,340,340,340,340,340,340,340,340,340,340,340,340,
340,340,340,340,340,824,826,828,832,833,856,82,857,869,247,562,870,879,562,880,
885,884,894,326,898,900,167,84,886,85,86,887,890,41,247,196,895,87,902,905,97,
247,911,88,908,89,90,928,327,328,943,91,944,912,946,951,953,92,954,957,984,935,
1001,134,939,941,961,999,1000,466,1002,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
247,1003,1004,1015,1017,1024,1025,340,1026,1028,247,1030,1031,738,997,22,246,358,
107,247,749,463,147,117,965,316,781,27,995,466,780,598,777,415,0,28,29,688,469,
557,539,855,983,570,0,0,0,32,0,0,730,34,0,0,0,0,994,0,35,36,0,0,0,998,746,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,302,0,0,38,0,562,0,0,39,40,41,42,43,304,340,0,0,0,0,
0,446,0,45,46,0,0,427,0,47,48,49,0,0,0,247,0,0,0,0,0,0,0,0,0,420,0,0,466,247,0,0,
0,792,0,0,0,0,0,0,0,0,466,134,0,800,0,0,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,759,760,21,172,173,269,340,270,271,272,273,274,275,276,277,22,81,
23,174,278,0,0,175,24,279,0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,280,181,0,0,
0,0,0,32,33,182,183,34,184,0,466,0,0,0,35,36,37,0,185,186,340,0,0,0,0,466,0,0,0,
0,0,0,340,0,0,0,0,0,0,265,0,281,84,187,85,86,39,40,41,42,43,282,138,- 300,0,0,0,0,
188,0,45,284,0,0,0,0,190,0,247,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,265,0,0,0,0,0,22,0,0,466,0,0,0,0,0,247,0,0,
27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,0,0,0,
904,0,0,340,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,302,0,340,0,0,0,0,0,39,40,0,42,43,304,
0,0,0,0,0,0,426,0,45,46,0,0,427,0,47,48,49,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,340,0,340,340,340,340,340,340,340,340,340,340,340,340,340,340,340,
340,340,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,340,0,21,172,173,269,0,270,271,272,273,274,275,276,277,22,81,23,174,278,0,
0,175,24,279,0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,280,181,0,0,0,0,340,32,
33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,281,84,187,85,86,39,40,41,42,43,282,138,283,0,0,0,0,188,0,45,284,0,0,0,0,
190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,21,172,173,269,0,270,271,272,273,274,275,276,277,22,81,23,174,278,0,0,
175,24,279,0,0,27,0,0,176,177,178,179,0,28,29,- 36,180,0,280,181,0,0,0,0,0,32,33,
182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,281,84,187,85,86,39,40,41,42,43,282,138,0,0,0,0,0,188,0,45,284,0,0,0,0,190,
0,0,191,0,0,192,193,0,0,194,195,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,0,0,21,172,173,269,0,270,271,272,273,274,275,276,277,22,81,23,174,278,0,0,175,
24,279,0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,280,181,0,0,0,0,0,32,33,182,
183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,281,84,187,85,86,39,40,41,42,43,282,138,0,0,0,0,0,188,0,45,284,0,0,0,0,190,0,0,
191,0,0,192,193,0,0,194,195,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,21,172,173,0,0,0,0,0,0,0,0,0,0,22,81,23,174,0,0,0,175,24,0,0,0,27,0,0,176,177,
178,179,0,28,29,0,180,0,0,181,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,
0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,39,40,41,42,
43,655,0,0,0,0,0,0,188,0,45,284,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,172,173,0,0,0,0,0,0,0,0,0,0,22,81,0,174,
0,0,0,175,0,0,0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,0,181,0,0,0,0,0,32,0,
182,183,34,184,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,38,84,187,85,86,39,40,41,42,43,0,356,0,0,0,0,0,188,0,45,284,0,0,0,0,190,0,0,
191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,172,173,
0,0,0,0,0,0,0,0,0,0,22,81,0,174,0,0,0,175,0,0,0,0,27,0,0,176,177,178,179,0,28,29,
0,180,0,0,181,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,39,40,41,42,43,0,0,0,0,0,0,0,
188,0,45,284,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,- 11,1,0,2,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,
683,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,
0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,39,
40,41,42,43,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,
0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,
0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,39,40,41,
42,43,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,
25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,
36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,39,40,41,42,43,
0,0,0,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,
0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,
0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,38,0,0,0,0,39,40,41,42,43,
22,0,- 11,0,0,0,0,44,0,45,46,0,27,0,0,47,48,49,0,0,28,29,0,0,0,0,0,0,0,0,0,0,32,0,
0,0,34,0,0,0,0,0,0,35,36,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
21,0,0,38,0,0,0,0,39,40,41,42,43,22,0,23,0,0,0,0,470,24,45,46,0,27,0,0,47,48,49,
0,0,28,29,- 36,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,35,36,37,2,3,4,77,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,0,42,43,22,138,23,
0,259,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,
0,0,0,0,0,35,36,37,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,
0,0,0,0,0,39,40,0,42,43,22,138,0,0,0,0,0,0,24,45,46,81,27,0,0,0,0,0,0,0,28,29,0,
0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,0,0,0,82,0,0,0,0,0,0,0,0,22,39,40,687,42,43,167,84,0,85,86,0,27,41,0,
45,46,87,0,0,28,29,0,88,0,89,90,0,327,328,0,91,32,0,0,0,34,92,0,0,0,0,0,35,36,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,302,0,0,303,0,0,0,22,39,40,
0,42,43,304,0,0,0,0,0,27,0,0,45,46,0,0,0,28,29,0,0,224,0,0,0,0,0,0,0,32,0,0,0,34,
0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,22,39,40,0,42,43,0,225,0,0,0,0,27,0,0,45,46,0,0,0,28,29,0,0,0,0,0,0,0,0,0,
0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,
0,0,0,302,0,0,0,0,834,835,22,39,40,0,42,43,304,0,0,0,0,81,27,0,0,45,46,0,0,0,28,
29,0,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,0,0,0,82,0,676,0,0,0,0,0,0,22,39,40,0,42,43,167,84,0,85,86,81,
27,41,0,45,46,87,0,0,28,29,0,836,0,89,90,0,0,0,0,91,32,0,837,0,34,92,193,0,0,194,
195,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,82,0,0,0,0,0,0,0,0,
22,39,40,407,42,43,167,84,0,85,86,0,27,41,0,45,46,87,0,0,28,29,0,88,0,89,90,0,0,
0,0,91,32,0,0,0,34,92,0,0,0,0,0,35,36,0,0,0,0,0,0,0,0,0,0,0,0,172,173,269,0,270,
271,272,273,274,275,276,277,0,81,0,174,278,0,39,175,0,279,43,0,0,0,0,176,177,178,
179,0,45,46,0,180,0,280,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,
186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,281,84,187,85,86,0,0,41,0,0,282,
138,0,0,172,173,0,188,0,0,189,544,0,0,0,190,0,81,191,174,0,192,193,175,0,194,195,
0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,
0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,
41,0,0,0,341,545,0,172,173,0,188,0,0,189,0,327,328,0,190,0,81,191,174,0,192,193,
175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,
0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,
187,85,86,0,0,41,0,0,0,341,787,0,172,173,0,188,0,0,189,0,327,328,0,190,0,81,191,
174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,181,0,0,0,0,
0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,341,952,0,172,173,0,188,0,0,189,0,327,328,0,
190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,341,0,0,172,173,0,188,0,0,189,0,
327,328,0,190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,
0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,0,0,0,172,173,0,188,0,
0,189,0,0,0,309,190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,
0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,341,0,0,172,173,
0,188,0,0,189,0,0,0,0,190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,
178,179,0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,81,0,185,
186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,490,172,
173,0,0,0,0,188,0,0,189,0,0,0,81,190,174,0,191,82,175,192,193,0,0,194,195,0,176,
177,178,179,0,0,83,84,180,85,86,181,0,41,0,0,0,87,0,182,183,0,184,88,0,89,90,0,0,
0,81,91,185,186,0,0,0,92,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,
41,0,0,0,172,173,0,0,0,0,188,588,0,189,0,0,0,81,190,174,0,191,82,175,192,193,0,0,
194,195,0,176,177,178,179,0,0,167,84,180,85,86,181,0,41,0,0,0,87,0,182,183,0,184,
88,0,89,90,0,0,0,0,91,185,186,0,0,0,92,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,
187,85,86,0,0,41,0,0,0,0,0,0,172,173,0,188,0,0,189,0,0,0,609,190,0,81,191,174,0,
192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,
182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,188,661,0,189,0,0,0,81,190,174,0,
191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,181,0,0,0,0,0,0,
0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,38,84,187,85,86,0,0,41,0,0,0,0,0,0,172,173,0,188,0,0,189,0,0,0,729,190,0,81,
191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,181,0,0,
0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,0,0,0,172,173,0,188,0,0,189,0,0,0,745,
190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,764,172,173,0,0,0,0,188,0,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,766,172,173,0,0,0,0,188,0,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,188,860,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,188,863,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,866,172,173,0,0,0,0,188,0,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,188,934,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,188,938,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,188,940,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,188,993,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,172,173,0,0,0,182,183,0,184,0,0,0,0,81,0,0,0,0,185,186,0,0,0,0,0,0,0,176,
177,178,179,0,0,0,0,0,0,0,181,0,0,38,84,187,85,86,182,183,41,184,0,0,0,0,0,0,0,0,
188,185,186,189,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,0,0,38,84,187,85,
86,0,0,41,0,0,0,341,0,172,173,0,0,188,0,0,189,0,0,0,0,190,81,0,191,0,0,192,193,0,
0,194,195,0,0,0,176,177,178,179,0,0,0,0,0,0,0,181,0,0,172,173,0,0,0,182,183,0,
184,0,0,0,0,81,0,0,0,0,185,186,0,0,0,0,0,0,0,176,177,178,179,0,0,0,0,0,0,0,181,0,
0,38,84,187,85,86,182,183,41,184,0,0,707,0,0,0,0,0,188,185,186,189,0,0,0,0,190,0,
0,191,0,0,192,193,0,0,194,195,0,0,0,0,38,84,187,85,86,0,0,41,0,0,172,173,0,0,0,0,
0,336,0,0,189,0,0,81,0,190,0,0,191,0,0,192,193,0,0,194,195,176,177,178,179,0,0,0,
0,0,0,0,181,0,0,172,173,0,0,0,182,183,0,184,0,0,0,0,81,0,0,0,0,185,186,0,0,0,0,0,
0,0,176,177,178,179,0,0,0,0,0,0,0,181,0,0,38,84,187,85,86,182,183,41,184,0,0,0,0,
0,0,0,0,353,185,186,189,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,0,0,38,
84,187,85,86,0,0,41,0,0,172,173,0,0,0,0,0,188,0,0,189,0,0,81,0,190,0,0,191,0,0,
192,193,0,0,194,195,176,177,178,179,0,0,0,0,0,0,0,181,81,0,0,0,0,0,0,182,183,0,
184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,
187,85,86,0,0,41,0,0,0,82,0,0,0,0,0,903,0,0,189,0,0,716,0,190,167,84,191,85,86,
192,193,41,0,194,195,87,0,0,0,0,0,88,0,89,90,0,0,0,0,91,0,0,0,0,0,92};static
short Cyc_yycheck[6372]={0,174,341,368,65,0,144,365,114,190,191,192,367,433,370,
285,286,68,288,23,144,21,175,128,23,175,21,371,372,327,211,109,166,144,188,328,
751,259,509,159,377,378,453,21,44,593,107,107,247,401,117,51,449,120,114,0,51,
227,58,59,60,17,62,58,59,60,135,62,68,69,139,93,68,68,379,380,21,128,82,476,58,
59,60,82,62,71,356,138,92,140,68,5,6,92,514,157,507,107,813,125,398,517,102,103,
402,516,51,102,103,135,428,65,134,58,59,60,122,62,105,119,120,121,122,68,119,120,
121,122,128,122,601,449,128,128,373,374,375,376,138,44,140,61,494,138,140,140,
133,147,321,107,128,107,57,107,107,328,117,102,103,707,138,161,140,225,122,169,
161,171,120,122,169,229,87,122,119,120,121,122,471,131,522,93,336,128,338,319,
247,51,118,247,117,607,112,138,124,140,307,351,118,353,381,382,383,120,265,119,
111,232,123,260,235,128,216,87,107,122,161,216,124,223,135,114,134,125,223,87,
642,227,88,89,125,124,227,135,285,286,141,288,102,103,135,49,304,229,100,101,120,
446,104,123,118,421,422,423,593,119,120,121,122,107,260,247,122,123,260,260,38,
609,87,117,112,470,124,106,136,137,301,125,130,265,128,116,260,124,126,285,286,
122,288,124,285,286,317,288,120,130,107,161,471,651,106,120,128,114,123,356,117,
285,286,124,288,112,728,124,314,260,135,733,119,181,122,132,133,134,120,329,548,
743,272,125,329,88,89,112,278,107,108,109,110,111,285,286,114,288,122,20,21,126,
641,107,117,129,124,258,693,127,356,124,774,117,222,356,120,779,122,122,123,116,
707,780,112,433,59,122,0,124,238,119,356,560,88,130,287,725,107,446,120,729,449,
118,113,497,690,132,128,500,22,23,106,25,554,140,141,745,109,703,704,411,121,470,
414,356,411,107,108,476,129,130,44,113,769,421,422,423,623,51,421,422,423,118,
430,57,83,84,85,486,124,116,637,65,426,67,120,122,70,124,446,509,448,500,128,130,
514,79,107,311,82,457,120,459,446,117,457,125,459,804,92,121,641,811,470,693,592,
124,120,613,102,103,129,130,188,107,824,832,470,111,486,117,118,627,107,486,612,
119,120,121,122,114,117,112,500,120,613,627,500,500,626,124,486,132,127,125,457,
221,459,141,627,140,141,107,117,124,500,120,944,113,114,115,528,945,525,117,117,
528,120,161,124,541,711,601,166,486,130,169,406,171,699,912,913,699,889,909,496,
902,120,500,911,668,914,908,416,56,623,107,124,676,124,789,120,277,114,915,916,
125,118,117,637,120,120,640,436,107,804,528,921,922,856,113,114,115,216,595,617,
457,119,459,595,223,124,94,95,96,97,98,99,100,101,102,103,117,672,611,120,962,
665,477,478,107,923,924,121,247,124,113,114,115,121,107,336,119,630,124,258,113,
114,115,344,623,908,265,348,267,350,124,270,353,107,108,504,135,120,637,113,139,
640,125,837,120,366,1008,124,287,125,120,528,665,120,889,125,665,665,125,1021,
117,855,124,120,124,917,918,919,920,836,120,124,685,681,665,125,714,685,117,107,
319,120,403,83,84,85,114,120,726,124,329,92,125,1004,107,130,124,132,133,134,113,
114,115,741,132,133,134,665,782,828,124,124,124,120,124,128,115,884,125,132,133,
134,137,117,124,1008,120,685,107,129,130,925,926,927,120,114,903,116,905,125,90,
91,759,120,619,124,145,759,125,117,107,875,120,132,133,134,113,114,115,480,481,
118,107,484,136,486,120,961,113,114,115,125,411,120,495,414,120,117,125,124,120,
125,114,86,87,130,120,132,133,134,120,125,856,120,118,125,138,120,125,756,669,
133,125,64,65,762,120,446,685,107,449,125,144,107,120,113,114,115,457,125,459,
1007,107,120,117,118,124,123,125,5,6,470,107,1000,132,133,134,476,113,114,115,
116,124,856,908,132,133,134,856,124,17,18,19,107,108,109,125,132,133,134,259,421,
422,423,119,856,64,65,66,944,119,120,119,172,948,174,175,75,76,77,78,79,22,23,
119,120,185,186,119,528,112,190,191,192,226,908,228,121,856,119,908,118,541,860,
119,120,863,137,138,90,91,118,211,312,86,87,124,908,119,120,754,755,124,107,124,
117,117,124,124,128,119,263,135,121,655,656,107,131,125,125,119,116,341,900,1008,
128,345,346,347,119,908,93,125,119,123,123,595,120,122,124,359,120,120,125,363,
125,119,118,107,124,120,125,107,117,123,696,933,934,30,117,937,938,128,940,623,
384,120,124,118,117,135,42,124,107,125,1008,125,396,637,125,1008,640,401,120,131,
120,125,120,647,120,119,732,120,125,119,125,112,1008,125,125,121,125,321,125,107,
124,747,125,427,328,987,131,119,990,124,992,993,38,118,117,125,117,763,764,119,
766,685,768,118,123,1008,118,25,122,125,112,1013,125,131,125,125,125,125,125,125,
786,365,125,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,
131,117,124,118,125,128,92,119,117,426,494,117,125,497,125,120,124,108,104,119,
28,107,108,125,110,111,125,125,114,446,836,125,118,118,124,759,453,92,124,128,
126,127,92,129,130,119,132,120,136,119,25,119,138,120,119,117,862,108,782,865,
866,125,125,124,548,119,8,9,10,11,12,13,14,15,16,17,18,19,20,21,497,125,120,125,
125,125,125,471,125,125,507,0,0,622,948,37,113,903,35,516,640,258,70,48,910,158,
681,49,944,593,676,414,672,223,- 1,57,58,541,263,352,330,759,928,369,- 1,- 1,- 1,69,
- 1,- 1,614,73,- 1,- 1,- 1,- 1,942,- 1,80,81,- 1,- 1,- 1,949,628,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,107,- 1,651,- 1,- 1,112,113,114,115,116,117,560,
- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,130,- 1,132,133,134,- 1,- 1,- 1,613,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,623,- 1,- 1,693,627,- 1,- 1,- 1,698,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,707,948,- 1,
710,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,
26,27,641,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,
52,53,54,55,- 1,57,58,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,69,70,71,72,73,74,- 1,789,- 1,-
1,- 1,80,81,82,- 1,84,85,699,- 1,- 1,- 1,- 1,804,- 1,- 1,- 1,- 1,- 1,- 1,711,- 1,- 1,- 1,- 1,- 1,
- 1,751,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,124,- 1,
126,127,- 1,- 1,- 1,- 1,132,- 1,780,135,- 1,- 1,138,139,- 1,- 1,142,143,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,813,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,
889,- 1,- 1,- 1,- 1,- 1,828,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,834,- 1,- 1,837,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,855,- 1,- 1,- 1,- 1,- 1,112,113,- 1,115,116,
117,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,130,- 1,132,133,134,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,909,- 1,911,912,913,914,
915,916,917,918,919,920,921,922,923,924,925,926,927,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,961,- 1,24,25,26,
27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,
54,55,- 1,57,58,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,1007,69,70,71,72,73,74,- 1,- 1,- 1,- 1,- 1,
80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,124,- 1,126,127,-
1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,
41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,
- 1,69,70,71,72,73,74,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,
118,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,
142,143,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,
29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,
- 1,57,58,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,69,70,71,72,73,74,- 1,- 1,- 1,- 1,- 1,80,81,82,
- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,
109,110,111,112,113,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,-
1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,
44,45,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,69,70,
71,72,73,74,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,- 1,- 1,- 1,
- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,
63,- 1,- 1,- 1,- 1,- 1,69,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,84,85,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,
114,115,116,- 1,118,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,
138,139,- 1,- 1,142,143,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,
55,- 1,57,58,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,69,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,80,81,
- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,
108,109,110,111,112,113,114,115,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,
- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,
- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,
69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,112,113,114,115,116,- 1,- 1,119,-
1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,133,134,0,1,- 1,3,4,5,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,
- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
67,68,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,112,113,114,115,116,- 1,- 1,
119,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,133,134,0,1,- 1,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,67,68,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,112,113,114,115,116,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,133,134,1,- 1,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,67,68,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,112,113,114,115,
116,37,- 1,119,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,49,- 1,- 1,132,133,134,- 1,- 1,57,58,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,107,- 1,- 1,- 1,- 1,112,113,114,
115,116,37,- 1,39,- 1,- 1,- 1,- 1,124,45,126,127,- 1,49,- 1,- 1,132,133,134,- 1,- 1,57,58,
59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,3,4,5,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,
- 1,115,116,37,118,39,- 1,121,- 1,- 1,- 1,45,126,127,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,3,4,5,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,
- 1,115,116,37,118,- 1,- 1,- 1,- 1,- 1,- 1,45,126,127,38,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
112,113,104,115,116,107,108,- 1,110,111,- 1,49,114,- 1,126,127,118,- 1,- 1,57,58,- 1,
124,- 1,126,127,- 1,129,130,- 1,132,69,- 1,- 1,- 1,73,138,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,107,- 1,- 1,- 1,
37,112,113,- 1,115,116,117,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,126,127,- 1,- 1,- 1,57,58,- 1,- 1,
61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,
- 1,115,116,- 1,118,- 1,- 1,- 1,- 1,49,- 1,- 1,126,127,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,- 1,- 1,25,26,37,112,113,- 1,115,116,
117,- 1,- 1,- 1,- 1,38,49,- 1,- 1,126,127,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,-
1,- 1,- 1,- 1,- 1,- 1,92,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,107,108,- 1,
110,111,38,49,114,- 1,126,127,118,- 1,- 1,57,58,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,
69,- 1,135,- 1,73,138,139,- 1,- 1,142,143,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,104,115,116,107,108,-
1,110,111,- 1,49,114,- 1,126,127,118,- 1,- 1,57,58,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,
69,- 1,- 1,- 1,73,138,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,
26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,112,44,- 1,46,116,- 1,- 1,- 1,- 1,
52,53,54,55,- 1,126,127,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,118,- 1,- 1,25,26,- 1,124,- 1,- 1,127,
32,- 1,- 1,- 1,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,
- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,
110,111,- 1,- 1,114,- 1,- 1,- 1,118,119,- 1,25,26,- 1,124,- 1,- 1,127,- 1,129,130,- 1,132,-
1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,-
1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,
114,- 1,- 1,- 1,118,119,- 1,25,26,- 1,124,- 1,- 1,127,- 1,129,130,- 1,132,- 1,38,135,40,- 1,
138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,-
1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,
118,119,- 1,25,26,- 1,124,- 1,- 1,127,- 1,129,130,- 1,132,- 1,38,135,40,- 1,138,139,44,-
1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,118,- 1,- 1,25,
26,- 1,124,- 1,- 1,127,- 1,129,130,- 1,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,-
1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,124,- 1,- 1,
127,- 1,- 1,- 1,131,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,
108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,118,- 1,- 1,25,26,- 1,124,- 1,- 1,127,- 1,- 1,- 1,- 1,
132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,84,85,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,
- 1,114,- 1,- 1,117,25,26,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,92,44,
138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,107,108,60,110,111,63,- 1,114,- 1,- 1,- 1,
118,- 1,71,72,- 1,74,124,- 1,126,127,- 1,- 1,- 1,38,132,84,85,- 1,- 1,- 1,138,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,
26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,92,44,138,139,- 1,- 1,142,
143,- 1,52,53,54,55,- 1,- 1,107,108,60,110,111,63,- 1,114,- 1,- 1,- 1,118,- 1,71,72,- 1,
74,124,- 1,126,127,- 1,- 1,- 1,- 1,132,84,85,- 1,- 1,- 1,138,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,
124,- 1,- 1,127,- 1,- 1,- 1,131,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,
52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,
- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,
- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,
114,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,124,- 1,- 1,127,- 1,- 1,- 1,131,132,- 1,38,135,40,- 1,
138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,-
1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,- 1,
- 1,- 1,25,26,- 1,124,- 1,- 1,127,- 1,- 1,- 1,131,132,- 1,38,135,40,- 1,138,139,44,- 1,142,
143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,-
1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,25,26,- 1,- 1,- 1,- 1,
124,- 1,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,
108,109,110,111,- 1,- 1,114,- 1,- 1,117,25,26,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,38,
132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,-
1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,-
1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,
124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,
108,109,110,111,- 1,- 1,114,- 1,- 1,117,25,26,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,38,
132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,-
1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,-
1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,
124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,
108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,
132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,-
1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,25,26,- 1,- 1,- 1,71,72,-
1,74,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,63,- 1,- 1,107,108,109,110,111,71,72,114,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
124,84,85,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,- 1,- 1,- 1,- 1,
107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,118,- 1,25,26,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,
- 1,132,38,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,63,- 1,- 1,25,26,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,84,85,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,107,108,109,110,111,71,
72,114,74,- 1,- 1,118,- 1,- 1,- 1,- 1,- 1,124,84,85,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,
138,139,- 1,- 1,142,143,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,25,26,- 1,-
1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,38,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,
52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,25,26,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,
38,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,
- 1,107,108,109,110,111,71,72,114,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,84,85,127,- 1,- 1,
- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,- 1,- 1,- 1,- 1,107,108,109,110,111,
- 1,- 1,114,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,38,- 1,132,- 1,- 1,135,- 1,
- 1,138,139,- 1,- 1,142,143,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,38,- 1,- 1,- 1,- 1,- 1,-
1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,92,- 1,- 1,- 1,
- 1,- 1,124,- 1,- 1,127,- 1,- 1,104,- 1,132,107,108,135,110,111,138,139,114,- 1,142,143,
118,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,- 1,- 1,- 1,138};char Cyc_Yystack_overflow[
17]="Yystack_overflow";struct Cyc_Yystack_overflow_struct{char*tag;int f1;};struct
Cyc_Yystack_overflow_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
void Cyc_yyerror(struct _dyneither_ptr);int Cyc_yylex(struct Cyc_Lexing_lexbuf*);
static int Cyc_yychar=(int)'\000';union Cyc_YYSTYPE Cyc_yylval={.YYINITIALSVAL={60,0}};
static int Cyc_yynerrs=0;struct _tuple24{struct Cyc_Position_Segment*f1;struct
_tuple1*f2;int f3;};struct _tuple25{struct _dyneither_ptr f1;void*f2;};static char
_tmp4A8[8]="stdcall";static char _tmp4A9[6]="cdecl";static char _tmp4AA[9]="fastcall";
static char _tmp4AB[9]="noreturn";static char _tmp4AC[6]="const";static char _tmp4AD[8]="aligned";
static char _tmp4AE[7]="packed";static char _tmp4AF[7]="shared";static char _tmp4B0[7]="unused";
static char _tmp4B1[5]="weak";static char _tmp4B2[10]="dllimport";static char _tmp4B3[
10]="dllexport";static char _tmp4B4[23]="no_instrument_function";static char _tmp4B5[
12]="constructor";static char _tmp4B6[11]="destructor";static char _tmp4B7[22]="no_check_memory_usage";
static char _tmp4B8[5]="pure";struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){struct _RegionHandle
_tmp40E=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp40E;
_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmp7C0=10000;short*_tmp7C1=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmp7C0));{unsigned int
_tmp7C2=_tmp7C0;unsigned int i;for(i=0;i < _tmp7C2;i ++){_tmp7C1[i]=(short)0;}}
_tmp7C1;});int yyvsp_offset;union Cyc_YYSTYPE*yyvs=(union Cyc_YYSTYPE*)({
unsigned int _tmp7BD=10000;union Cyc_YYSTYPE*_tmp7BE=(union Cyc_YYSTYPE*)
_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp7BD));{
unsigned int _tmp7BF=_tmp7BD;unsigned int i;for(i=0;i < _tmp7BF;i ++){_tmp7BE[i]=Cyc_yylval;}}
_tmp7BE;});int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({
unsigned int _tmp7BA=10000;struct Cyc_Yyltype*_tmp7BB=(struct Cyc_Yyltype*)
_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp7BA));{
unsigned int _tmp7BC=_tmp7BA;unsigned int i;for(i=0;i < _tmp7BC;i ++){_tmp7BB[i]=Cyc_yynewloc();}}
_tmp7BB;});int yystacksize=10000;union Cyc_YYSTYPE yyval=Cyc_yylval;int yylen;
yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=
0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){Cyc_yyerror(({const char*_tmp40F="parser stack overflow";_tag_dyneither(
_tmp40F,sizeof(char),22);}));(int)_throw((void*)& Cyc_Yystack_overflow_val);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1032,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 371?(int)Cyc_yytranslate[_check_known_subscript_notnull(372,Cyc_yychar)]:
287;}yyn +=yychar1;if((yyn < 0  || yyn > 6371) || Cyc_yycheck[
_check_known_subscript_notnull(6372,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6372,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1031){int
_tmp410=0;_npop_handler(0);return _tmp410;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)-- yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1032,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(510,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL1F7: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL1F8: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp411;_push_handler(& _tmp411);{int _tmp413=0;if(setjmp(_tmp411.handler))_tmp413=
1;if(!_tmp413){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp412=(void*)_exn_thrown;void*
_tmp415=_tmp412;_LL1FB: {struct Cyc_Core_Failure_struct*_tmp416=(struct Cyc_Core_Failure_struct*)
_tmp415;if(_tmp416->tag != Cyc_Core_Failure)goto _LL1FD;}_LL1FC: x=0;goto _LL1FA;
_LL1FD:;_LL1FE:(void)_throw(_tmp415);_LL1FA:;}}}{struct _handler_cons _tmp417;
_push_handler(& _tmp417);{int _tmp419=0;if(setjmp(_tmp417.handler))_tmp419=1;if(!
_tmp419){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;
_pop_handler();}else{void*_tmp418=(void*)_exn_thrown;void*_tmp41B=_tmp418;_LL200: {
struct Cyc_Core_Failure_struct*_tmp41C=(struct Cyc_Core_Failure_struct*)_tmp41B;
if(_tmp41C->tag != Cyc_Core_Failure)goto _LL202;}_LL201: y=0;goto _LL1FF;_LL202:;
_LL203:(void)_throw(_tmp41B);_LL1FF:;}}}yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));break;}
case 3: _LL1F9: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp41D=_cycalloc(sizeof(*
_tmp41D));_tmp41D->hd=({struct Cyc_Absyn_Decl*_tmp41E=_cycalloc(sizeof(*_tmp41E));
_tmp41E->r=(void*)({struct Cyc_Absyn_Using_d_struct*_tmp41F=_cycalloc(sizeof(*
_tmp41F));_tmp41F[0]=({struct Cyc_Absyn_Using_d_struct _tmp420;_tmp420.tag=11;
_tmp420.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp420.f2=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp420;});_tmp41F;});_tmp41E->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp41E;});
_tmp41D->tl=0;_tmp41D;}));Cyc_Lex_leave_using();break;case 4: _LL204: yyval=Cyc_YY18(({
struct Cyc_List_List*_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421->hd=({struct Cyc_Absyn_Decl*
_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422->r=(void*)({struct Cyc_Absyn_Using_d_struct*
_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423[0]=({struct Cyc_Absyn_Using_d_struct
_tmp424;_tmp424.tag=11;_tmp424.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp424.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp424;});_tmp423;});
_tmp422->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp422;});_tmp421->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp421;}));break;case 5:
_LL205: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp425=_cycalloc(sizeof(*_tmp425));
_tmp425->hd=({struct Cyc_Absyn_Decl*_tmp426=_cycalloc(sizeof(*_tmp426));_tmp426->r=(
void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp427=_cycalloc(sizeof(*_tmp427));
_tmp427[0]=({struct Cyc_Absyn_Namespace_d_struct _tmp428;_tmp428.tag=10;_tmp428.f1=({
struct _dyneither_ptr*_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp429;});_tmp428.f2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp428;});
_tmp427;});_tmp426->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp426;});
_tmp425->tl=0;_tmp425;}));Cyc_Lex_leave_namespace();break;case 6: _LL206: {struct
_dyneither_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp42A;_push_handler(& _tmp42A);{int _tmp42C=0;if(setjmp(_tmp42A.handler))
_tmp42C=1;if(!_tmp42C){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp42B=(void*)_exn_thrown;void*_tmp42E=_tmp42B;_LL209: {struct Cyc_Core_Failure_struct*
_tmp42F=(struct Cyc_Core_Failure_struct*)_tmp42E;if(_tmp42F->tag != Cyc_Core_Failure)
goto _LL20B;}_LL20A: nspace=({const char*_tmp430="";_tag_dyneither(_tmp430,sizeof(
char),1);});goto _LL208;_LL20B:;_LL20C:(void)_throw(_tmp42E);_LL208:;}}}{struct
_handler_cons _tmp431;_push_handler(& _tmp431);{int _tmp433=0;if(setjmp(_tmp431.handler))
_tmp433=1;if(!_tmp433){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);;_pop_handler();}else{void*_tmp432=(void*)_exn_thrown;
void*_tmp435=_tmp432;_LL20E: {struct Cyc_Core_Failure_struct*_tmp436=(struct Cyc_Core_Failure_struct*)
_tmp435;if(_tmp436->tag != Cyc_Core_Failure)goto _LL210;}_LL20F: x=0;goto _LL20D;
_LL210:;_LL211:(void)_throw(_tmp435);_LL20D:;}}}{struct _handler_cons _tmp437;
_push_handler(& _tmp437);{int _tmp439=0;if(setjmp(_tmp437.handler))_tmp439=1;if(!
_tmp439){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;
_pop_handler();}else{void*_tmp438=(void*)_exn_thrown;void*_tmp43B=_tmp438;_LL213: {
struct Cyc_Core_Failure_struct*_tmp43C=(struct Cyc_Core_Failure_struct*)_tmp43B;
if(_tmp43C->tag != Cyc_Core_Failure)goto _LL215;}_LL214: y=0;goto _LL212;_LL215:;
_LL216:(void)_throw(_tmp43B);_LL212:;}}}yyval=Cyc_YY18(({struct Cyc_List_List*
_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D->hd=({struct Cyc_Absyn_Decl*_tmp43E=
_cycalloc(sizeof(*_tmp43E));_tmp43E->r=(void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp440;_tmp440.tag=10;_tmp440.f1=({struct _dyneither_ptr*_tmp441=_cycalloc(
sizeof(*_tmp441));_tmp441[0]=nspace;_tmp441;});_tmp440.f2=x;_tmp440;});_tmp43F;});
_tmp43E->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp43E;});_tmp43D->tl=y;_tmp43D;}));break;}case 7:
_LL207: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp442="C";
_tag_dyneither(_tmp442,sizeof(char),2);}))== 0)yyval=Cyc_YY18(({struct Cyc_List_List*
_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443->hd=({struct Cyc_Absyn_Decl*_tmp444=
_cycalloc(sizeof(*_tmp444));_tmp444->r=(void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp446;_tmp446.tag=12;_tmp446.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp446;});_tmp445;});
_tmp444->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp444;});_tmp443->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp443;}));else{if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),({const char*_tmp447="C include";_tag_dyneither(_tmp447,
sizeof(char),10);}))!= 0)Cyc_Parse_err(({const char*_tmp448="expecting \"C\" or \"C include\"";
_tag_dyneither(_tmp448,sizeof(char),29);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=Cyc_YY18(({
struct Cyc_List_List*_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449->hd=({struct Cyc_Absyn_Decl*
_tmp44A=_cycalloc(sizeof(*_tmp44A));_tmp44A->r=(void*)({struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp44B=_cycalloc(sizeof(*_tmp44B));_tmp44B[0]=({struct Cyc_Absyn_ExternCinclude_d_struct
_tmp44C;_tmp44C.tag=13;_tmp44C.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp44C.f2=0;_tmp44C;});
_tmp44B;});_tmp44A->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp44A;});
_tmp449->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp449;}));}break;case 8: _LL217: if(Cyc_strcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),({const char*_tmp44D="C include";_tag_dyneither(_tmp44D,
sizeof(char),10);}))!= 0)Cyc_Parse_err(({const char*_tmp44E="expecting \"C include\"";
_tag_dyneither(_tmp44E,sizeof(char),22);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
yyval=Cyc_YY18(({struct Cyc_List_List*_tmp44F=_cycalloc(sizeof(*_tmp44F));_tmp44F->hd=({
struct Cyc_Absyn_Decl*_tmp450=_cycalloc(sizeof(*_tmp450));_tmp450->r=(void*)({
struct Cyc_Absyn_ExternCinclude_d_struct*_tmp451=_cycalloc(sizeof(*_tmp451));
_tmp451[0]=({struct Cyc_Absyn_ExternCinclude_d_struct _tmp452;_tmp452.tag=13;
_tmp452.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]);_tmp452.f2=exs;_tmp452;});_tmp451;});_tmp450->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp450;});
_tmp44F->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp44F;}));break;}case 9: _LL218: yyval=Cyc_YY18(({struct Cyc_List_List*
_tmp453=_cycalloc(sizeof(*_tmp453));_tmp453->hd=({struct Cyc_Absyn_Decl*_tmp454=
_cycalloc(sizeof(*_tmp454));_tmp454->r=(void*)& Cyc_Absyn_Porton_d_val;_tmp454->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp454;});_tmp453->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp453;}));break;case 10:
_LL219: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp455=_cycalloc(sizeof(*_tmp455));
_tmp455->hd=({struct Cyc_Absyn_Decl*_tmp456=_cycalloc(sizeof(*_tmp456));_tmp456->r=(
void*)& Cyc_Absyn_Portoff_d_val;_tmp456->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp456;});
_tmp455->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp455;}));break;case 11: _LL21A: yyval=Cyc_YY18(0);break;case 12:
_LL21B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 13: _LL21C: yyval=Cyc_YY52(0);break;case 14: _LL21D: yyval=Cyc_YY52(({struct Cyc_List_List*
_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457->hd=({struct _tuple24*_tmp458=
_cycalloc(sizeof(*_tmp458));_tmp458->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp458->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp458->f3=0;_tmp458;});_tmp457->tl=0;_tmp457;}));break;case 15: _LL21E: yyval=Cyc_YY52(({
struct Cyc_List_List*_tmp459=_cycalloc(sizeof(*_tmp459));_tmp459->hd=({struct
_tuple24*_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp45A->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp45A->f3=0;_tmp45A;});_tmp459->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp459;}));break;case 16:
_LL21F: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp45B=_cycalloc(sizeof(*_tmp45B));
_tmp45B->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*_tmp45C=
_cycalloc(sizeof(*_tmp45C));_tmp45C[0]=({struct Cyc_Absyn_Fn_d_struct _tmp45D;
_tmp45D.tag=1;_tmp45D.f1=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp45D;});_tmp45C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp45B->tl=0;
_tmp45B;}));break;case 17: _LL220: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 18: _LL221: yyval=Cyc_YY18(0);break;case 19: _LL222: yyval=
Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 20:
_LL223: yyval=Cyc_YY17(Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp45E=
_cycalloc(sizeof(*_tmp45E));_tmp45E->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp45E;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 21:
_LL224: yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 22:
_LL225: yyval=Cyc_YY17(Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp45F=
_cycalloc(sizeof(*_tmp45F));_tmp45F->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp45F;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 23:
_LL226: yyval=Cyc_YY17(Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp460=
_cycalloc(sizeof(*_tmp460));_tmp460->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp460;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 24:
_LL227: yyval=Cyc_YY17(Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp461=
_cycalloc(sizeof(*_tmp461));_tmp461->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp461;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 25:
_LL228: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL229: Cyc_Lex_leave_using();
break;case 27: _LL22A: Cyc_Lex_enter_namespace(({struct _dyneither_ptr*_tmp462=
_cycalloc(sizeof(*_tmp462));_tmp462[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp462;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL22B: Cyc_Lex_leave_namespace();
break;case 29: _LL22C: yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
30: _LL22D: yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 31:
_LL22E: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp463=_cycalloc(sizeof(*_tmp463));
_tmp463->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp463->tl=0;
_tmp463;}));break;case 32: _LL22F: {struct Cyc_List_List*_tmp464=0;{struct Cyc_List_List*
_tmp465=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
for(0;_tmp465 != 0;_tmp465=_tmp465->tl){struct _dyneither_ptr*_tmp466=(struct
_dyneither_ptr*)_tmp465->hd;struct _tuple1*qv=({struct _tuple1*_tmp469=_cycalloc(
sizeof(*_tmp469));_tmp469->f1=Cyc_Absyn_Rel_n(0);_tmp469->f2=_tmp466;_tmp469;});
struct Cyc_Absyn_Vardecl*_tmp467=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
_tmp464=({struct Cyc_List_List*_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468->hd=
_tmp467;_tmp468->tl=_tmp464;_tmp468;});}}_tmp464=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp464);yyval=Cyc_YY18(({struct Cyc_List_List*
_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A->hd=Cyc_Absyn_letv_decl(_tmp464,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp46A->tl=0;
_tmp46A;}));break;}case 33: _LL230: if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp46B="`H";
_tag_dyneither(_tmp46B,sizeof(char),3);}))== 0)Cyc_Parse_err((struct
_dyneither_ptr)({void*_tmp46C=0;Cyc_aprintf(({const char*_tmp46D="bad occurrence of heap region";
_tag_dyneither(_tmp46D,sizeof(char),30);}),_tag_dyneither(_tmp46C,sizeof(void*),
0));}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp46E="`U";
_tag_dyneither(_tmp46E,sizeof(char),3);}))== 0)Cyc_Parse_err((struct
_dyneither_ptr)({void*_tmp46F=0;Cyc_aprintf(({const char*_tmp470="bad occurrence of unique region";
_tag_dyneither(_tmp470,sizeof(char),32);}),_tag_dyneither(_tmp46F,sizeof(void*),
0));}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp478=_cycalloc(sizeof(*_tmp478));_tmp478->name=({struct _dyneither_ptr*_tmp479=
_cycalloc(sizeof(*_tmp479));_tmp479[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp479;});_tmp478->identity=
- 1;_tmp478->kind=Cyc_Tcutil_kind_to_bound(Cyc_Absyn_RgnKind);_tmp478;});void*t=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp476=_cycalloc(sizeof(*_tmp476));
_tmp476[0]=({struct Cyc_Absyn_VarType_struct _tmp477;_tmp477.tag=2;_tmp477.f1=tv;
_tmp477;});_tmp476;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472->f1=Cyc_Absyn_Loc_n;_tmp472->f2=({
struct _dyneither_ptr*_tmp473=_cycalloc(sizeof(*_tmp473));_tmp473[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp473;});_tmp472;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp474=_cycalloc(sizeof(*_tmp474));
_tmp474[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp475;_tmp475.tag=16;_tmp475.f1=(
void*)t;_tmp475;});_tmp474;}),0);yyval=Cyc_YY18(({struct Cyc_List_List*_tmp471=
_cycalloc(sizeof(*_tmp471));_tmp471->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp471->tl=0;
_tmp471;}));break;}case 34: _LL231: if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({const char*_tmp47A="H";
_tag_dyneither(_tmp47A,sizeof(char),2);}))== 0)Cyc_Parse_err(({const char*_tmp47B="bad occurrence of heap region `H";
_tag_dyneither(_tmp47B,sizeof(char),33);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),({const char*_tmp47C="U";_tag_dyneither(_tmp47C,sizeof(
char),2);}))== 0)Cyc_Parse_err(({const char*_tmp47D="bad occurrence of unique region `U";
_tag_dyneither(_tmp47D,sizeof(char),35);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));if(Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]) && Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)Cyc_Parse_err(({
const char*_tmp47E="open regions cannot be @resetable";_tag_dyneither(_tmp47E,
sizeof(char),34);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp486=_cycalloc(sizeof(*_tmp486));_tmp486->name=({
struct _dyneither_ptr*_tmp487=_cycalloc(sizeof(*_tmp487));_tmp487[0]=(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp48A;_tmp48A.tag=0;_tmp48A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{void*_tmp488[1]={&
_tmp48A};Cyc_aprintf(({const char*_tmp489="`%s";_tag_dyneither(_tmp489,sizeof(
char),4);}),_tag_dyneither(_tmp488,sizeof(void*),1));}});_tmp487;});_tmp486->identity=
- 1;_tmp486->kind=Cyc_Tcutil_kind_to_bound(Cyc_Absyn_RgnKind);_tmp486;});void*t=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp484=_cycalloc(sizeof(*_tmp484));
_tmp484[0]=({struct Cyc_Absyn_VarType_struct _tmp485;_tmp485.tag=2;_tmp485.f1=tv;
_tmp485;});_tmp484;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp480=_cycalloc(sizeof(*_tmp480));_tmp480->f1=Cyc_Absyn_Loc_n;_tmp480->f2=({
struct _dyneither_ptr*_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp481;});_tmp480;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp482=_cycalloc(sizeof(*_tmp482));
_tmp482[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp483;_tmp483.tag=16;_tmp483.f1=(
void*)t;_tmp483;});_tmp482;}),0);yyval=Cyc_YY18(({struct Cyc_List_List*_tmp47F=
_cycalloc(sizeof(*_tmp47F));_tmp47F->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp47F->tl=0;
_tmp47F;}));break;}case 35: _LL232: {struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp48E=_cycalloc(sizeof(*_tmp48E));_tmp48E->name=({struct _dyneither_ptr*_tmp491=
_cycalloc(sizeof(*_tmp491));_tmp491[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp491;});_tmp48E->identity=
- 1;_tmp48E->kind=(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp48F=_cycalloc_atomic(
sizeof(*_tmp48F));_tmp48F[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp490;_tmp490.tag=0;
_tmp490.f1=Cyc_Absyn_RgnKind;_tmp490;});_tmp48F;});_tmp48E;});struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp48C=_cycalloc(sizeof(*_tmp48C));
_tmp48C->f1=Cyc_Absyn_Loc_n;_tmp48C->f2=({struct _dyneither_ptr*_tmp48D=_cycalloc(
sizeof(*_tmp48D));_tmp48D[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp48D;});_tmp48C;}),(
void*)& Cyc_Absyn_VoidType_val,0);yyval=Cyc_YY18(({struct Cyc_List_List*_tmp48B=
_cycalloc(sizeof(*_tmp48B));_tmp48B->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),tv,vd,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp48B->tl=0;
_tmp48B;}));break;}case 36: _LL233: yyval=Cyc_YY31(0);break;case 37: _LL234: yyval=Cyc_YY31(
1);break;case 38: _LL235: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 39: _LL236: yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 40: _LL237: yyval=
Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp492=_cycalloc(sizeof(*_tmp492));
_tmp492->sc=({struct Cyc_Core_Opt*_tmp493=_cycalloc_atomic(sizeof(*_tmp493));
_tmp493->v=(void*)Cyc_yyget_YY22(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp493;});_tmp492->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp492->type_specs=
0;_tmp492->is_inline=0;_tmp492->attributes=0;_tmp492;}));break;case 41: _LL238: if((
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc != 
0)({void*_tmp494=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp495="Only one storage class is allowed in a declaration";_tag_dyneither(
_tmp495,sizeof(char),51);}),_tag_dyneither(_tmp494,sizeof(void*),0));});yyval=
Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp496=_cycalloc(sizeof(*_tmp496));
_tmp496->sc=({struct Cyc_Core_Opt*_tmp497=_cycalloc_atomic(sizeof(*_tmp497));
_tmp497->v=(void*)Cyc_yyget_YY22(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp497;});_tmp496->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp496->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp496->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp496->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp496;}));
break;case 42: _LL239: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp498=
_cycalloc(sizeof(*_tmp498));_tmp498->sc=0;_tmp498->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp498->type_specs=({
struct Cyc_List_List*_tmp499=_cycalloc(sizeof(*_tmp499));_tmp499->hd=Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp499->tl=0;_tmp499;});
_tmp498->is_inline=0;_tmp498->attributes=0;_tmp498;}));break;case 43: _LL23A: yyval=
Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp49A=_cycalloc(sizeof(*_tmp49A));
_tmp49A->sc=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc;_tmp49A->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp49A->type_specs=({
struct Cyc_List_List*_tmp49B=_cycalloc(sizeof(*_tmp49B));_tmp49B->hd=Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp49B->tl=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp49B;});
_tmp49A->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp49A->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp49A;}));
break;case 44: _LL23B: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp49C=
_cycalloc(sizeof(*_tmp49C));_tmp49C->sc=0;_tmp49C->tq=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp49C->type_specs=0;
_tmp49C->is_inline=0;_tmp49C->attributes=0;_tmp49C;}));break;case 45: _LL23C: yyval=
Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp49D=_cycalloc(sizeof(*_tmp49D));
_tmp49D->sc=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc;_tmp49D->tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);_tmp49D->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp49D->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp49D->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp49D;}));
break;case 46: _LL23D: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp49E=
_cycalloc(sizeof(*_tmp49E));_tmp49E->sc=0;_tmp49E->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp49E->type_specs=
0;_tmp49E->is_inline=1;_tmp49E->attributes=0;_tmp49E;}));break;case 47: _LL23E:
yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp49F=_cycalloc(sizeof(*
_tmp49F));_tmp49F->sc=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc;_tmp49F->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp49F->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp49F->is_inline=1;_tmp49F->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp49F;}));
break;case 48: _LL23F: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp4A0=
_cycalloc(sizeof(*_tmp4A0));_tmp4A0->sc=0;_tmp4A0->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A0->type_specs=
0;_tmp4A0->is_inline=0;_tmp4A0->attributes=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A0;}));break;case 49:
_LL240: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp4A1=_cycalloc(
sizeof(*_tmp4A1));_tmp4A1->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp4A1->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp4A1->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp4A1->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp4A1->attributes=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp4A1;}));
break;case 50: _LL241: yyval=Cyc_YY22(Cyc_Parse_Auto_sc);break;case 51: _LL242: yyval=
Cyc_YY22(Cyc_Parse_Register_sc);break;case 52: _LL243: yyval=Cyc_YY22(Cyc_Parse_Static_sc);
break;case 53: _LL244: yyval=Cyc_YY22(Cyc_Parse_Extern_sc);break;case 54: _LL245: if(
Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp4A2="C";
_tag_dyneither(_tmp4A2,sizeof(char),2);}))!= 0)Cyc_Parse_err(({const char*_tmp4A3="only extern or extern \"C\" is allowed";
_tag_dyneither(_tmp4A3,sizeof(char),37);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY22(
Cyc_Parse_ExternC_sc);break;case 55: _LL246: yyval=Cyc_YY22(Cyc_Parse_Typedef_sc);
break;case 56: _LL247: yyval=Cyc_YY22(Cyc_Parse_Abstract_sc);break;case 57: _LL248:
yyval=Cyc_YY45(0);break;case 58: _LL249: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 59: _LL24A: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;case 60: _LL24B: yyval=
Cyc_YY45(({struct Cyc_List_List*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4->hd=
Cyc_yyget_YY46(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A4->tl=
0;_tmp4A4;}));break;case 61: _LL24C: yyval=Cyc_YY45(({struct Cyc_List_List*_tmp4A5=
_cycalloc(sizeof(*_tmp4A5));_tmp4A5->hd=Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4A5->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A5;}));break;case
62: _LL24D: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={6,- 1};static
struct _tuple25 att_map[17]={{{_tmp4A8,_tmp4A8,_tmp4A8 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{
_tmp4A9,_tmp4A9,_tmp4A9 + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp4AA,_tmp4AA,
_tmp4AA + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp4AB,_tmp4AB,_tmp4AB + 9},(
void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp4AC,_tmp4AC,_tmp4AC + 6},(void*)& Cyc_Absyn_Const_att_val},{{
_tmp4AD,_tmp4AD,_tmp4AD + 8},(void*)& att_aligned},{{_tmp4AE,_tmp4AE,_tmp4AE + 7},(
void*)& Cyc_Absyn_Packed_att_val},{{_tmp4AF,_tmp4AF,_tmp4AF + 7},(void*)& Cyc_Absyn_Shared_att_val},{{
_tmp4B0,_tmp4B0,_tmp4B0 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp4B1,_tmp4B1,
_tmp4B1 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp4B2,_tmp4B2,_tmp4B2 + 10},(void*)&
Cyc_Absyn_Dllimport_att_val},{{_tmp4B3,_tmp4B3,_tmp4B3 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{
_tmp4B4,_tmp4B4,_tmp4B4 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{
_tmp4B5,_tmp4B5,_tmp4B5 + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp4B6,
_tmp4B6,_tmp4B6 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp4B7,_tmp4B7,
_tmp4B7 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp4B8,_tmp4B8,
_tmp4B8 + 5},(void*)& Cyc_Absyn_Pure_att_val}};struct _dyneither_ptr _tmp4A6=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((
_get_dyneither_size(_tmp4A6,sizeof(char))> 4  && *((const char*)
_check_dyneither_subscript(_tmp4A6,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp4A6,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp4A6,sizeof(char),(int)(_get_dyneither_size(
_tmp4A6,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp4A6,sizeof(char),(int)(_get_dyneither_size(_tmp4A6,sizeof(char))- 3)))== '_')
_tmp4A6=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp4A6,2,
_get_dyneither_size(_tmp4A6,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp4A6,(struct _dyneither_ptr)(att_map[i]).f1)== 0){yyval=
Cyc_YY46((att_map[i]).f2);break;}}if(i == 17){Cyc_Parse_err(({const char*_tmp4A7="unrecognized attribute";
_tag_dyneither(_tmp4A7,sizeof(char),23);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY46((
void*)& Cyc_Absyn_Cdecl_att_val);}break;}}case 63: _LL24E: yyval=Cyc_YY46((void*)&
Cyc_Absyn_Const_att_val);break;case 64: _LL24F: {struct _dyneither_ptr _tmp4BA=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);int _tmp4BC;struct
_tuple6 _tmp4BB=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp4BC=_tmp4BB.f2;{void*a;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp4BA,({const char*_tmp4BD="regparm";_tag_dyneither(_tmp4BD,
sizeof(char),8);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4BA,({const char*
_tmp4BE="__regparm__";_tag_dyneither(_tmp4BE,sizeof(char),12);}))== 0){if(
_tmp4BC < 0  || _tmp4BC > 3)Cyc_Parse_err(({const char*_tmp4BF="regparm requires value between 0 and 3";
_tag_dyneither(_tmp4BF,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp4C0=_cycalloc_atomic(sizeof(*_tmp4C0));
_tmp4C0[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp4C1;_tmp4C1.tag=0;_tmp4C1.f1=
_tmp4BC;_tmp4C1;});_tmp4C0;});}else{if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4BA,({
const char*_tmp4C2="aligned";_tag_dyneither(_tmp4C2,sizeof(char),8);}))== 0  || 
Cyc_zstrcmp((struct _dyneither_ptr)_tmp4BA,({const char*_tmp4C3="__aligned__";
_tag_dyneither(_tmp4C3,sizeof(char),12);}))== 0){if(_tmp4BC < 0)Cyc_Parse_err(({
const char*_tmp4C4="aligned requires positive power of two";_tag_dyneither(
_tmp4C4,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp4BC;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp4C5="aligned requires positive power of two";
_tag_dyneither(_tmp4C5,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp4C6=_cycalloc_atomic(sizeof(*_tmp4C6));
_tmp4C6[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp4C7;_tmp4C7.tag=6;_tmp4C7.f1=
_tmp4BC;_tmp4C7;});_tmp4C6;});}}else{if(Cyc_zstrcmp((struct _dyneither_ptr)
_tmp4BA,({const char*_tmp4C8="initializes";_tag_dyneither(_tmp4C8,sizeof(char),12);}))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4BA,({const char*_tmp4C9="__initializes__";
_tag_dyneither(_tmp4C9,sizeof(char),16);}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*
_tmp4CA=_cycalloc_atomic(sizeof(*_tmp4CA));_tmp4CA[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp4CB;_tmp4CB.tag=20;_tmp4CB.f1=_tmp4BC;_tmp4CB;});_tmp4CA;});else{Cyc_Parse_err(({
const char*_tmp4CC="unrecognized attribute";_tag_dyneither(_tmp4CC,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}yyval=Cyc_YY46(
a);break;}}case 65: _LL250: {struct _dyneither_ptr _tmp4CD=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct _dyneither_ptr
_tmp4CE=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4CD,({const
char*_tmp4CF="section";_tag_dyneither(_tmp4CF,sizeof(char),8);}))== 0  || Cyc_zstrcmp((
struct _dyneither_ptr)_tmp4CD,({const char*_tmp4D0="__section__";_tag_dyneither(
_tmp4D0,sizeof(char),12);}))== 0)a=(void*)({struct Cyc_Absyn_Section_att_struct*
_tmp4D1=_cycalloc(sizeof(*_tmp4D1));_tmp4D1[0]=({struct Cyc_Absyn_Section_att_struct
_tmp4D2;_tmp4D2.tag=8;_tmp4D2.f1=_tmp4CE;_tmp4D2;});_tmp4D1;});else{Cyc_Parse_err(({
const char*_tmp4D3="unrecognized attribute";_tag_dyneither(_tmp4D3,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));a=(void*)& Cyc_Absyn_Cdecl_att_val;}yyval=Cyc_YY46(
a);break;}case 66: _LL251: {struct _dyneither_ptr _tmp4D4=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct _dyneither_ptr
_tmp4D5=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4D4,({const
char*_tmp4D6="__mode__";_tag_dyneither(_tmp4D6,sizeof(char),9);}))== 0)a=(void*)({
struct Cyc_Absyn_Mode_att_struct*_tmp4D7=_cycalloc(sizeof(*_tmp4D7));_tmp4D7[0]=({
struct Cyc_Absyn_Mode_att_struct _tmp4D8;_tmp4D8.tag=22;_tmp4D8.f1=_tmp4D5;_tmp4D8;});
_tmp4D7;});else{Cyc_Parse_err(({const char*_tmp4D9="unrecognized attribute";
_tag_dyneither(_tmp4D9,sizeof(char),23);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)&
Cyc_Absyn_Cdecl_att_val;}yyval=Cyc_YY46(a);break;}case 67: _LL252: {struct
_dyneither_ptr _tmp4DA=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dyneither_ptr _tmp4DB=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp4DD;struct _tuple6
_tmp4DC=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]);_tmp4DD=_tmp4DC.f2;{int _tmp4DF;struct _tuple6 _tmp4DE=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4DF=_tmp4DE.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;if(Cyc_zstrcmp((struct _dyneither_ptr)
_tmp4DA,({const char*_tmp4E0="format";_tag_dyneither(_tmp4E0,sizeof(char),7);}))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4DA,({const char*_tmp4E1="__format__";
_tag_dyneither(_tmp4E1,sizeof(char),11);}))== 0){if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp4DB,({const char*_tmp4E2="printf";_tag_dyneither(_tmp4E2,
sizeof(char),7);}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp4E3=
_cycalloc_atomic(sizeof(*_tmp4E3));_tmp4E3[0]=({struct Cyc_Absyn_Format_att_struct
_tmp4E4;_tmp4E4.tag=19;_tmp4E4.f1=Cyc_Absyn_Printf_ft;_tmp4E4.f2=_tmp4DD;_tmp4E4.f3=
_tmp4DF;_tmp4E4;});_tmp4E3;});else{if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4DB,({
const char*_tmp4E5="scanf";_tag_dyneither(_tmp4E5,sizeof(char),6);}))== 0)a=(void*)({
struct Cyc_Absyn_Format_att_struct*_tmp4E6=_cycalloc_atomic(sizeof(*_tmp4E6));
_tmp4E6[0]=({struct Cyc_Absyn_Format_att_struct _tmp4E7;_tmp4E7.tag=19;_tmp4E7.f1=
Cyc_Absyn_Scanf_ft;_tmp4E7.f2=_tmp4DD;_tmp4E7.f3=_tmp4DF;_tmp4E7;});_tmp4E6;});
else{Cyc_Parse_err(({const char*_tmp4E8="unrecognized format type";_tag_dyneither(
_tmp4E8,sizeof(char),25);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp4E9="unrecognized attribute";_tag_dyneither(_tmp4E9,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}yyval=Cyc_YY46(a);break;}}}case 68: _LL253: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 69: _LL254: yyval=
Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypedefType_struct*_tmp4EA=
_cycalloc(sizeof(*_tmp4EA));_tmp4EA[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp4EB;_tmp4EB.tag=18;_tmp4EB.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4EB.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4EB.f3=0;_tmp4EB.f4=
0;_tmp4EB;});_tmp4EA;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 70:
_LL255: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 71:
_LL256: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 72:
_LL257: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 73:
_LL258: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 74:
_LL259: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Short_spec_struct*_tmp4EC=
_cycalloc(sizeof(*_tmp4EC));_tmp4EC[0]=({struct Cyc_Parse_Short_spec_struct
_tmp4ED;_tmp4ED.tag=2;_tmp4ED.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4ED;});
_tmp4EC;}));break;case 75: _LL25A: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 76: _LL25B: yyval=Cyc_YY23((void*)({struct
Cyc_Parse_Long_spec_struct*_tmp4EE=_cycalloc(sizeof(*_tmp4EE));_tmp4EE[0]=({
struct Cyc_Parse_Long_spec_struct _tmp4EF;_tmp4EF.tag=3;_tmp4EF.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4EF;});
_tmp4EE;}));break;case 77: _LL25C: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 78: _LL25D: yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 79:
_LL25E: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Signed_spec_struct*_tmp4F0=
_cycalloc(sizeof(*_tmp4F0));_tmp4F0[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp4F1;_tmp4F1.tag=0;_tmp4F1.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4F1;});
_tmp4F0;}));break;case 80: _LL25F: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp4F3;_tmp4F3.tag=1;_tmp4F3.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4F3;});
_tmp4F2;}));break;case 81: _LL260: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 82: _LL261: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 83: _LL262: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 84: _LL263: yyval=
Cyc_YY23(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 85:
_LL264: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TupleType_struct*
_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4[0]=({struct Cyc_Absyn_TupleType_struct
_tmp4F5;_tmp4F5.tag=11;_tmp4F5.f1=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(
struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp4F5;});_tmp4F4;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 86: _LL265: yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));
_tmp4F6[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp4F7;_tmp4F7.tag=16;_tmp4F7.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4F7;});_tmp4F6;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 87:
_LL266: {void*_tmp4F8=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0);
yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp4FA;_tmp4FA.tag=17;_tmp4FA.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4FA.f2=(void*)
_tmp4F8;_tmp4FA;});_tmp4F9;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 88:
_LL267: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp4FC;_tmp4FC.tag=17;_tmp4FC.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4FC.f2=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4FC;});_tmp4FB;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 89: _LL268: yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TagType_struct*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));
_tmp4FD[0]=({struct Cyc_Absyn_TagType_struct _tmp4FE;_tmp4FE.tag=20;_tmp4FE.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4FE;});_tmp4FD;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 90:
_LL269: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_struct*
_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF[0]=({struct Cyc_Absyn_TagType_struct
_tmp500;_tmp500.tag=20;_tmp500.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ik,0);_tmp500;});_tmp4FF;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 91:
_LL26A: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_ValueofType_struct*
_tmp501=_cycalloc(sizeof(*_tmp501));_tmp501[0]=({struct Cyc_Absyn_ValueofType_struct
_tmp502;_tmp502.tag=19;_tmp502.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp502;});_tmp501;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 92: _LL26B: yyval=Cyc_YY43(Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 93: _LL26C: {struct Cyc_Position_Segment*
loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line): 0;yyval=Cyc_YY25(({
struct Cyc_Absyn_Tqual _tmp503;_tmp503.print_const=1;_tmp503.q_volatile=0;_tmp503.q_restrict=
0;_tmp503.real_const=1;_tmp503.loc=loc;_tmp503;}));break;}case 94: _LL26D: yyval=
Cyc_YY25(({struct Cyc_Absyn_Tqual _tmp504;_tmp504.print_const=0;_tmp504.q_volatile=
1;_tmp504.q_restrict=0;_tmp504.real_const=0;_tmp504.loc=0;_tmp504;}));break;case
95: _LL26E: yyval=Cyc_YY25(({struct Cyc_Absyn_Tqual _tmp505;_tmp505.print_const=0;
_tmp505.q_volatile=0;_tmp505.q_restrict=1;_tmp505.real_const=0;_tmp505.loc=0;
_tmp505;}));break;case 96: _LL26F: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp506=_cycalloc(sizeof(*_tmp506));_tmp506[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp507;_tmp507.tag=5;_tmp507.f1=({struct Cyc_Absyn_Decl*_tmp508=_cycalloc(
sizeof(*_tmp508));_tmp508->r=(void*)({struct Cyc_Absyn_Enum_d_struct*_tmp509=
_cycalloc(sizeof(*_tmp509));_tmp509[0]=({struct Cyc_Absyn_Enum_d_struct _tmp50A;
_tmp50A.tag=8;_tmp50A.f1=({struct Cyc_Absyn_Enumdecl*_tmp50B=_cycalloc(sizeof(*
_tmp50B));_tmp50B->sc=Cyc_Absyn_Public;_tmp50B->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp50B->fields=({struct
Cyc_Core_Opt*_tmp50C=_cycalloc(sizeof(*_tmp50C));_tmp50C->v=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp50C;});_tmp50B;});
_tmp50A;});_tmp509;});_tmp508->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp508;});
_tmp507;});_tmp506;}));break;case 97: _LL270: yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_EnumType_struct*_tmp50D=_cycalloc(sizeof(*_tmp50D));
_tmp50D[0]=({struct Cyc_Absyn_EnumType_struct _tmp50E;_tmp50E.tag=14;_tmp50E.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp50E.f2=0;_tmp50E;});_tmp50D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 98:
_LL271: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Type_spec_struct*_tmp50F=
_cycalloc(sizeof(*_tmp50F));_tmp50F[0]=({struct Cyc_Parse_Type_spec_struct _tmp510;
_tmp510.tag=4;_tmp510.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp512;_tmp512.tag=15;_tmp512.f1=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp512;});_tmp511;}));
_tmp510.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp510;});_tmp50F;}));break;case 99: _LL272: yyval=Cyc_YY47(({
struct Cyc_Absyn_Enumfield*_tmp513=_cycalloc(sizeof(*_tmp513));_tmp513->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp513->tag=0;_tmp513->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp513;}));break;case 100: _LL273: yyval=Cyc_YY47(({
struct Cyc_Absyn_Enumfield*_tmp514=_cycalloc(sizeof(*_tmp514));_tmp514->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp514->tag=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp514->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp514;}));
break;case 101: _LL274: yyval=Cyc_YY48(({struct Cyc_List_List*_tmp515=_cycalloc(
sizeof(*_tmp515));_tmp515->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp515->tl=0;_tmp515;}));break;case 102: _LL275: yyval=Cyc_YY48(({
struct Cyc_List_List*_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516->hd=Cyc_yyget_YY47(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp516->tl=0;
_tmp516;}));break;case 103: _LL276: yyval=Cyc_YY48(({struct Cyc_List_List*_tmp517=
_cycalloc(sizeof(*_tmp517));_tmp517->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp517->tl=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp517;}));break;case
104: _LL277: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmp518=_cycalloc(sizeof(*_tmp518));_tmp518[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmp519;_tmp519.tag=13;_tmp519.f1=Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp519.f2=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp519;});_tmp518;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 105: _LL278: {struct Cyc_List_List*_tmp51A=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]));struct Cyc_List_List*_tmp51B=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=Cyc_YY23((
void*)({struct Cyc_Parse_Decl_spec_struct*_tmp51C=_cycalloc(sizeof(*_tmp51C));
_tmp51C[0]=({struct Cyc_Parse_Decl_spec_struct _tmp51D;_tmp51D.tag=5;_tmp51D.f1=
Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 7)]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp51A,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp51B,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),1),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp51D;});
_tmp51C;}));break;}case 106: _LL279: {struct Cyc_List_List*_tmp51E=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp51F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));yyval=Cyc_YY23((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp520=_cycalloc(sizeof(*_tmp520));_tmp520[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp521;_tmp521.tag=5;_tmp521.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp51E,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp51F,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp521;});
_tmp520;}));break;}case 107: _LL27A: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_AggrType_struct*_tmp522=_cycalloc(sizeof(*_tmp522));_tmp522[0]=({
struct Cyc_Absyn_AggrType_struct _tmp523;_tmp523.tag=12;_tmp523.f1=({struct Cyc_Absyn_AggrInfo
_tmp524;_tmp524.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({struct Cyc_Core_Opt*
_tmp525=_cycalloc_atomic(sizeof(*_tmp525));_tmp525->v=(void*)1;_tmp525;}));
_tmp524.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp524;});_tmp523;});_tmp522;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 108:
_LL27B: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AggrType_struct*
_tmp526=_cycalloc(sizeof(*_tmp526));_tmp526[0]=({struct Cyc_Absyn_AggrType_struct
_tmp527;_tmp527.tag=12;_tmp527.f1=({struct Cyc_Absyn_AggrInfo _tmp528;_tmp528.aggr_info=
Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),0);_tmp528.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp528;});_tmp527;});
_tmp526;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 109: _LL27C: yyval=Cyc_YY40(0);break;case
110: _LL27D: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
break;case 111: _LL27E: yyval=Cyc_YY24(Cyc_Absyn_StructA);break;case 112: _LL27F:
yyval=Cyc_YY24(Cyc_Absyn_UnionA);break;case 113: _LL280: yyval=Cyc_YY26(0);break;
case 114: _LL281: yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY27(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))));
break;case 115: _LL282: yyval=Cyc_YY27(({struct Cyc_List_List*_tmp529=_cycalloc(
sizeof(*_tmp529));_tmp529->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp529->tl=0;_tmp529;}));break;case 116: _LL283: yyval=Cyc_YY27(({
struct Cyc_List_List*_tmp52A=_cycalloc(sizeof(*_tmp52A));_tmp52A->hd=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp52A->tl=Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp52A;}));break;
case 117: _LL284: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 118: _LL285: yyval=Cyc_YY21(({struct Cyc_List_List*
_tmp52B=_cycalloc(sizeof(*_tmp52B));_tmp52B->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp52B->tl=0;_tmp52B;}));
break;case 119: _LL286: yyval=Cyc_YY21(({struct Cyc_List_List*_tmp52C=_cycalloc(
sizeof(*_tmp52C));_tmp52C->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp52C->tl=Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp52C;}));break;case
120: _LL287: yyval=Cyc_YY20(({struct _tuple21*_tmp52D=_cycalloc(sizeof(*_tmp52D));
_tmp52D->f1=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp52D->f2=0;_tmp52D;}));break;case 121: _LL288: yyval=Cyc_YY20(({
struct _tuple21*_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp52E->f2=(struct
Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp52E;}));break;case 122: _LL289: {struct _RegionHandle _tmp52F=
_new_region("temp");struct _RegionHandle*temp=& _tmp52F;_push_region(temp);{struct
_tuple22 _tmp531;struct Cyc_Absyn_Tqual _tmp532;struct Cyc_List_List*_tmp533;struct
Cyc_List_List*_tmp534;struct _tuple22*_tmp530=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp531=*_tmp530;_tmp532=
_tmp531.f1;_tmp533=_tmp531.f2;_tmp534=_tmp531.f3;if(_tmp532.loc == 0)_tmp532.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp536;struct Cyc_List_List*
_tmp537;struct _tuple0 _tmp535=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(temp,temp,Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp536=_tmp535.f1;
_tmp537=_tmp535.f2;{void*_tmp538=Cyc_Parse_speclist2typ(_tmp533,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp539=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(
temp,_tmp532,_tmp538,_tmp536,_tmp534),_tmp537);yyval=Cyc_YY26(((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,struct _tuple15*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmp539));}}}_npop_handler(0);break;;_pop_region(
temp);}case 123: _LL28A: yyval=Cyc_YY35(({struct _tuple22*_tmp53A=_cycalloc(sizeof(*
_tmp53A));_tmp53A->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp53A->f2=({
struct Cyc_List_List*_tmp53B=_cycalloc(sizeof(*_tmp53B));_tmp53B->hd=Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp53B->tl=0;_tmp53B;});
_tmp53A->f3=0;_tmp53A;}));break;case 124: _LL28B: yyval=Cyc_YY35(({struct _tuple22*
_tmp53C=_cycalloc(sizeof(*_tmp53C));_tmp53C->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp53C->f2=({struct Cyc_List_List*
_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp53D->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp53D;});_tmp53C->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp53C;}));break;case 125: _LL28C: yyval=Cyc_YY35(({struct _tuple22*_tmp53E=
_cycalloc(sizeof(*_tmp53E));_tmp53E->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp53E->f2=0;_tmp53E->f3=0;
_tmp53E;}));break;case 126: _LL28D: yyval=Cyc_YY35(({struct _tuple22*_tmp53F=
_cycalloc(sizeof(*_tmp53F));_tmp53F->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp53F->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp53F->f3=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp53F;}));break;
case 127: _LL28E: yyval=Cyc_YY35(({struct _tuple22*_tmp540=_cycalloc(sizeof(*_tmp540));
_tmp540->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp540->f2=0;
_tmp540->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp540;}));break;case 128: _LL28F: yyval=Cyc_YY35(({struct _tuple22*
_tmp541=_cycalloc(sizeof(*_tmp541));_tmp541->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp541->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp541->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp541;}));break;case 129: _LL290: yyval=Cyc_YY35(({struct _tuple22*_tmp542=
_cycalloc(sizeof(*_tmp542));_tmp542->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp542->f2=({
struct Cyc_List_List*_tmp543=_cycalloc(sizeof(*_tmp543));_tmp543->hd=Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp543->tl=0;_tmp543;});
_tmp542->f3=0;_tmp542;}));break;case 130: _LL291: yyval=Cyc_YY35(({struct _tuple22*
_tmp544=_cycalloc(sizeof(*_tmp544));_tmp544->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp544->f2=({struct Cyc_List_List*
_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp545->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp545;});_tmp544->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp544;}));break;case 131: _LL292: yyval=Cyc_YY35(({struct _tuple22*_tmp546=
_cycalloc(sizeof(*_tmp546));_tmp546->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp546->f2=0;_tmp546->f3=0;
_tmp546;}));break;case 132: _LL293: yyval=Cyc_YY35(({struct _tuple22*_tmp547=
_cycalloc(sizeof(*_tmp547));_tmp547->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp547->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp547->f3=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp547;}));break;
case 133: _LL294: yyval=Cyc_YY35(({struct _tuple22*_tmp548=_cycalloc(sizeof(*_tmp548));
_tmp548->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp548->f2=0;
_tmp548->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp548;}));break;case 134: _LL295: yyval=Cyc_YY35(({struct _tuple22*
_tmp549=_cycalloc(sizeof(*_tmp549));_tmp549->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp549->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp549->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp549;}));break;case 135: _LL296: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 136: _LL297:
yyval=Cyc_YY21(({struct Cyc_List_List*_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A->hd=
Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp54A->tl=
0;_tmp54A;}));break;case 137: _LL298: yyval=Cyc_YY21(({struct Cyc_List_List*_tmp54B=
_cycalloc(sizeof(*_tmp54B));_tmp54B->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp54B->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp54B;}));break;
case 138: _LL299: yyval=Cyc_YY20(({struct _tuple21*_tmp54C=_cycalloc(sizeof(*_tmp54C));
_tmp54C->f1=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp54C->f2=0;_tmp54C;}));break;case 139: _LL29A: yyval=Cyc_YY20(({
struct _tuple21*_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D->f1=({struct Cyc_Parse_Declarator*
_tmp54E=_cycalloc(sizeof(*_tmp54E));_tmp54E->id=({struct _tuple1*_tmp54F=
_cycalloc(sizeof(*_tmp54F));_tmp54F->f1=Cyc_Absyn_Rel_n(0);_tmp54F->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);
_tmp54F;});_tmp54E->tms=0;_tmp54E;});_tmp54D->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp54D;}));break;case
140: _LL29B: yyval=Cyc_YY20(({struct _tuple21*_tmp551=_cycalloc(sizeof(*_tmp551));
_tmp551->f1=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp551->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp551;}));break;case 141:
_LL29C: {int _tmp552=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_List_List*_tmp553=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=Cyc_YY23((
void*)({struct Cyc_Parse_Decl_spec_struct*_tmp554=_cycalloc(sizeof(*_tmp554));
_tmp554[0]=({struct Cyc_Parse_Decl_spec_struct _tmp555;_tmp555.tag=5;_tmp555.f1=
Cyc_Absyn_datatype_decl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp553,({struct Cyc_Core_Opt*
_tmp556=_cycalloc(sizeof(*_tmp556));_tmp556->v=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp556;}),_tmp552,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp555;});
_tmp554;}));break;}case 142: _LL29D: {int _tmp557=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_DatatypeType_struct*_tmp558=_cycalloc(sizeof(*_tmp558));
_tmp558[0]=({struct Cyc_Absyn_DatatypeType_struct _tmp559;_tmp559.tag=3;_tmp559.f1=({
struct Cyc_Absyn_DatatypeInfo _tmp55A;_tmp55A.datatype_info=Cyc_Absyn_UnknownDatatype(({
struct Cyc_Absyn_UnknownDatatypeInfo _tmp55B;_tmp55B.name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp55B.is_extensible=
_tmp557;_tmp55B;}));_tmp55A.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp55A;});_tmp559;});
_tmp558;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}case 143: _LL29E: {int _tmp55C=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_DatatypeFieldType_struct*_tmp55D=_cycalloc(sizeof(*
_tmp55D));_tmp55D[0]=({struct Cyc_Absyn_DatatypeFieldType_struct _tmp55E;_tmp55E.tag=
4;_tmp55E.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp55F;_tmp55F.field_info=Cyc_Absyn_UnknownDatatypefield(({
struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp560;_tmp560.datatype_name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp560.field_name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp560.is_extensible=_tmp55C;_tmp560;}));_tmp55F.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp55F;});_tmp55E;});
_tmp55D;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}case 144: _LL29F: yyval=Cyc_YY31(0);break;case
145: _LL2A0: yyval=Cyc_YY31(1);break;case 146: _LL2A1: yyval=Cyc_YY34(({struct Cyc_List_List*
_tmp561=_cycalloc(sizeof(*_tmp561));_tmp561->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp561->tl=0;_tmp561;}));
break;case 147: _LL2A2: yyval=Cyc_YY34(({struct Cyc_List_List*_tmp562=_cycalloc(
sizeof(*_tmp562));_tmp562->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp562->tl=0;_tmp562;}));break;case 148: _LL2A3: yyval=
Cyc_YY34(({struct Cyc_List_List*_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563->hd=
Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp563->tl=Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp563;}));break;case 149: _LL2A4: yyval=Cyc_YY34(({struct Cyc_List_List*
_tmp564=_cycalloc(sizeof(*_tmp564));_tmp564->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp564->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp564;}));break;case
150: _LL2A5: yyval=Cyc_YY32(Cyc_Absyn_Public);break;case 151: _LL2A6: yyval=Cyc_YY32(
Cyc_Absyn_Extern);break;case 152: _LL2A7: yyval=Cyc_YY32(Cyc_Absyn_Static);break;
case 153: _LL2A8: yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp565=_cycalloc(
sizeof(*_tmp565));_tmp565->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp565->typs=0;_tmp565->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp565->sc=Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp565;}));break;case
154: _LL2A9: {struct Cyc_List_List*_tmp566=((struct Cyc_List_List*(*)(struct
_tuple16*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=Cyc_YY33(({
struct Cyc_Absyn_Datatypefield*_tmp567=_cycalloc(sizeof(*_tmp567));_tmp567->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp567->typs=_tmp566;_tmp567->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp567->sc=Cyc_yyget_YY32(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp567;}));break;}
case 155: _LL2AA: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 156: _LL2AB: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp568=
_cycalloc(sizeof(*_tmp568));_tmp568->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->id;_tmp568->tms=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp568;}));
break;case 157: _LL2AC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 158: _LL2AD: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*
_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->id;_tmp569->tms=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp569;}));
break;case 159: _LL2AE: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp56A=
_cycalloc(sizeof(*_tmp56A));_tmp56A->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp56A->tms=0;_tmp56A;}));
break;case 160: _LL2AF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 161: _LL2B0: {struct Cyc_Parse_Declarator*_tmp56B=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp56B->tms=({
struct Cyc_List_List*_tmp56C=_cycalloc(sizeof(*_tmp56C));_tmp56C->hd=(void*)({
struct Cyc_Absyn_Attributes_mod_struct*_tmp56D=_cycalloc(sizeof(*_tmp56D));
_tmp56D[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp56E;_tmp56E.tag=5;_tmp56E.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp56E.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp56E;});_tmp56D;});
_tmp56C->tl=_tmp56B->tms;_tmp56C;});yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;}case 162: _LL2B1: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*
_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp56F->tms=({
struct Cyc_List_List*_tmp570=_cycalloc(sizeof(*_tmp570));_tmp570->hd=(void*)({
struct Cyc_Absyn_Carray_mod_struct*_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571[0]=({
struct Cyc_Absyn_Carray_mod_struct _tmp572;_tmp572.tag=0;_tmp572.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp572.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp572;});
_tmp571;});_tmp570->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp570;});_tmp56F;}));break;case 163: _LL2B2: yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp573->tms=({
struct Cyc_List_List*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574->hd=(void*)({
struct Cyc_Absyn_ConstArray_mod_struct*_tmp575=_cycalloc(sizeof(*_tmp575));
_tmp575[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp576;_tmp576.tag=1;_tmp576.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp576.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp576.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp576;});_tmp575;});_tmp574->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp574;});
_tmp573;}));break;case 164: _LL2B3: {struct _tuple23 _tmp578;struct Cyc_List_List*
_tmp579;int _tmp57A;struct Cyc_Absyn_VarargInfo*_tmp57B;struct Cyc_Core_Opt*_tmp57C;
struct Cyc_List_List*_tmp57D;struct _tuple23*_tmp577=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp578=*_tmp577;_tmp579=
_tmp578.f1;_tmp57A=_tmp578.f2;_tmp57B=_tmp578.f3;_tmp57C=_tmp578.f4;_tmp57D=
_tmp578.f5;yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp57E=_cycalloc(sizeof(*
_tmp57E));_tmp57E->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp57E->tms=({struct Cyc_List_List*_tmp57F=_cycalloc(
sizeof(*_tmp57F));_tmp57F->hd=(void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp581;_tmp581.tag=3;_tmp581.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp582=_cycalloc(sizeof(*_tmp582));_tmp582[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp583;_tmp583.tag=1;_tmp583.f1=_tmp579;_tmp583.f2=_tmp57A;_tmp583.f3=_tmp57B;
_tmp583.f4=_tmp57C;_tmp583.f5=_tmp57D;_tmp583;});_tmp582;}));_tmp581;});_tmp580;});
_tmp57F->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp57F;});_tmp57E;}));break;}case 165: _LL2B4: yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp584->tms=({
struct Cyc_List_List*_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585->hd=(void*)({
struct Cyc_Absyn_Function_mod_struct*_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586[
0]=({struct Cyc_Absyn_Function_mod_struct _tmp587;_tmp587.tag=3;_tmp587.f1=(void*)((
void*)({struct Cyc_Absyn_WithTypes_struct*_tmp588=_cycalloc(sizeof(*_tmp588));
_tmp588[0]=({struct Cyc_Absyn_WithTypes_struct _tmp589;_tmp589.tag=1;_tmp589.f1=0;
_tmp589.f2=0;_tmp589.f3=0;_tmp589.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp589.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp589;});_tmp588;}));
_tmp587;});_tmp586;});_tmp585->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp585;});_tmp584;}));
break;case 166: _LL2B5: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp58A=
_cycalloc(sizeof(*_tmp58A));_tmp58A->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp58A->tms=({
struct Cyc_List_List*_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B->hd=(void*)({
struct Cyc_Absyn_Function_mod_struct*_tmp58C=_cycalloc(sizeof(*_tmp58C));_tmp58C[
0]=({struct Cyc_Absyn_Function_mod_struct _tmp58D;_tmp58D.tag=3;_tmp58D.f1=(void*)((
void*)({struct Cyc_Absyn_NoTypes_struct*_tmp58E=_cycalloc(sizeof(*_tmp58E));
_tmp58E[0]=({struct Cyc_Absyn_NoTypes_struct _tmp58F;_tmp58F.tag=0;_tmp58F.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp58F.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp58F;});
_tmp58E;}));_tmp58D;});_tmp58C;});_tmp58B->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp58B;});_tmp58A;}));
break;case 167: _LL2B6: {struct Cyc_List_List*_tmp590=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp591=_cycalloc(sizeof(*_tmp591));_tmp591->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp591->tms=({
struct Cyc_List_List*_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592->hd=(void*)({
struct Cyc_Absyn_TypeParams_mod_struct*_tmp593=_cycalloc(sizeof(*_tmp593));
_tmp593[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp594;_tmp594.tag=4;_tmp594.f1=
_tmp590;_tmp594.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp594.f3=0;
_tmp594;});_tmp593;});_tmp592->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp592;});_tmp591;}));
break;}case 168: _LL2B7: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp595=
_cycalloc(sizeof(*_tmp595));_tmp595->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp595->tms=({
struct Cyc_List_List*_tmp596=_cycalloc(sizeof(*_tmp596));_tmp596->hd=(void*)({
struct Cyc_Absyn_Attributes_mod_struct*_tmp597=_cycalloc(sizeof(*_tmp597));
_tmp597[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp598;_tmp598.tag=5;_tmp598.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp598.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp598;});_tmp597;});
_tmp596->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp596;});_tmp595;}));break;case 169: _LL2B8: yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599->id=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp599->tms=0;_tmp599;}));
break;case 170: _LL2B9: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp59A=
_cycalloc(sizeof(*_tmp59A));_tmp59A->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp59A->tms=0;_tmp59A;}));
break;case 171: _LL2BA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 172: _LL2BB: {struct Cyc_Parse_Declarator*_tmp59B=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp59B->tms=({
struct Cyc_List_List*_tmp59C=_cycalloc(sizeof(*_tmp59C));_tmp59C->hd=(void*)({
struct Cyc_Absyn_Attributes_mod_struct*_tmp59D=_cycalloc(sizeof(*_tmp59D));
_tmp59D[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp59E;_tmp59E.tag=5;_tmp59E.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp59E.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp59E;});_tmp59D;});
_tmp59C->tl=_tmp59B->tms;_tmp59C;});yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;}case 173: _LL2BC: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*
_tmp59F=_cycalloc(sizeof(*_tmp59F));_tmp59F->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp59F->tms=({
struct Cyc_List_List*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));_tmp5A0->hd=(void*)({
struct Cyc_Absyn_Carray_mod_struct*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1[0]=({
struct Cyc_Absyn_Carray_mod_struct _tmp5A2;_tmp5A2.tag=0;_tmp5A2.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5A2.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5A2;});
_tmp5A1;});_tmp5A0->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp5A0;});_tmp59F;}));break;case 174: _LL2BD: yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));_tmp5A3->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5A3->tms=({
struct Cyc_List_List*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));_tmp5A4->hd=(void*)({
struct Cyc_Absyn_ConstArray_mod_struct*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));
_tmp5A5[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp5A6;_tmp5A6.tag=1;_tmp5A6.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5A6.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp5A6.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5A6;});_tmp5A5;});_tmp5A4->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5A4;});
_tmp5A3;}));break;case 175: _LL2BE: {struct _tuple23 _tmp5A8;struct Cyc_List_List*
_tmp5A9;int _tmp5AA;struct Cyc_Absyn_VarargInfo*_tmp5AB;struct Cyc_Core_Opt*_tmp5AC;
struct Cyc_List_List*_tmp5AD;struct _tuple23*_tmp5A7=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5A8=*_tmp5A7;_tmp5A9=
_tmp5A8.f1;_tmp5AA=_tmp5A8.f2;_tmp5AB=_tmp5A8.f3;_tmp5AC=_tmp5A8.f4;_tmp5AD=
_tmp5A8.f5;yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp5AE=_cycalloc(sizeof(*
_tmp5AE));_tmp5AE->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5AE->tms=({struct Cyc_List_List*_tmp5AF=_cycalloc(
sizeof(*_tmp5AF));_tmp5AF->hd=(void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp5B1;_tmp5B1.tag=3;_tmp5B1.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp5B2=_cycalloc(sizeof(*_tmp5B2));_tmp5B2[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp5B3;_tmp5B3.tag=1;_tmp5B3.f1=_tmp5A9;_tmp5B3.f2=_tmp5AA;_tmp5B3.f3=_tmp5AB;
_tmp5B3.f4=_tmp5AC;_tmp5B3.f5=_tmp5AD;_tmp5B3;});_tmp5B2;}));_tmp5B1;});_tmp5B0;});
_tmp5AF->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp5AF;});_tmp5AE;}));break;}case 176: _LL2BF: yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));_tmp5B4->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5B4->tms=({
struct Cyc_List_List*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5->hd=(void*)({
struct Cyc_Absyn_Function_mod_struct*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6[
0]=({struct Cyc_Absyn_Function_mod_struct _tmp5B7;_tmp5B7.tag=3;_tmp5B7.f1=(void*)((
void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));
_tmp5B8[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5B9;_tmp5B9.tag=1;_tmp5B9.f1=0;
_tmp5B9.f2=0;_tmp5B9.f3=0;_tmp5B9.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5B9.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5B9;});_tmp5B8;}));
_tmp5B7;});_tmp5B6;});_tmp5B5->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5B5;});_tmp5B4;}));
break;case 177: _LL2C0: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp5BA=
_cycalloc(sizeof(*_tmp5BA));_tmp5BA->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5BA->tms=({
struct Cyc_List_List*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->hd=(void*)({
struct Cyc_Absyn_Function_mod_struct*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC[
0]=({struct Cyc_Absyn_Function_mod_struct _tmp5BD;_tmp5BD.tag=3;_tmp5BD.f1=(void*)((
void*)({struct Cyc_Absyn_NoTypes_struct*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));
_tmp5BE[0]=({struct Cyc_Absyn_NoTypes_struct _tmp5BF;_tmp5BF.tag=0;_tmp5BF.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5BF.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5BF;});
_tmp5BE;}));_tmp5BD;});_tmp5BC;});_tmp5BB->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5BB;});_tmp5BA;}));
break;case 178: _LL2C1: {struct Cyc_List_List*_tmp5C0=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));_tmp5C1->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5C1->tms=({
struct Cyc_List_List*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));_tmp5C2->hd=(void*)({
struct Cyc_Absyn_TypeParams_mod_struct*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));
_tmp5C3[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp5C4;_tmp5C4.tag=4;_tmp5C4.f1=
_tmp5C0;_tmp5C4.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5C4.f3=0;
_tmp5C4;});_tmp5C3;});_tmp5C2->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5C2;});_tmp5C1;}));
break;}case 179: _LL2C2: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp5C5=
_cycalloc(sizeof(*_tmp5C5));_tmp5C5->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp5C5->tms=({
struct Cyc_List_List*_tmp5C6=_cycalloc(sizeof(*_tmp5C6));_tmp5C6->hd=(void*)({
struct Cyc_Absyn_Attributes_mod_struct*_tmp5C7=_cycalloc(sizeof(*_tmp5C7));
_tmp5C7[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp5C8;_tmp5C8.tag=5;_tmp5C8.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5C8.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C8;});_tmp5C7;});
_tmp5C6->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp5C6;});_tmp5C5;}));break;case 180: _LL2C3: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 181: _LL2C4: yyval=
Cyc_YY28(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 182: _LL2C5: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp5C9=_cycalloc(sizeof(*_tmp5C9));_tmp5C9->hd=(void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5CA=_cycalloc(sizeof(*_tmp5CA));_tmp5CA[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5CB;_tmp5CB.tag=5;_tmp5CB.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp5CB.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5CB;});_tmp5CA;});_tmp5C9->tl=ans;_tmp5C9;});{struct Cyc_Absyn_PtrLoc*ptrloc=
0;struct Cyc_Position_Segment*_tmp5CD;union Cyc_Absyn_Constraint*_tmp5CE;union Cyc_Absyn_Constraint*
_tmp5CF;struct _tuple18 _tmp5CC=*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp5CD=_tmp5CC.f1;_tmp5CE=_tmp5CC.f2;_tmp5CF=_tmp5CC.f3;
if(Cyc_Absyn_porting_c_code)ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp5D0=_cycalloc(
sizeof(*_tmp5D0));_tmp5D0->ptr_loc=_tmp5CD;_tmp5D0->rgn_loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5D0->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line);_tmp5D0;});{union Cyc_Absyn_Constraint*_tmp5D2;
union Cyc_Absyn_Constraint*_tmp5D3;union Cyc_Absyn_Constraint*_tmp5D4;void*_tmp5D5;
struct _tuple13 _tmp5D1=Cyc_Parse_collapse_pointer_quals(_tmp5CD,_tmp5CE,_tmp5CF,
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY54(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));_tmp5D2=_tmp5D1.f1;
_tmp5D3=_tmp5D1.f2;_tmp5D4=_tmp5D1.f3;_tmp5D5=_tmp5D1.f4;ans=({struct Cyc_List_List*
_tmp5D6=_cycalloc(sizeof(*_tmp5D6));_tmp5D6->hd=(void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp5D7=_cycalloc(sizeof(*_tmp5D7));_tmp5D7[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp5D8;_tmp5D8.tag=2;_tmp5D8.f1=({struct Cyc_Absyn_PtrAtts _tmp5D9;_tmp5D9.rgn=
_tmp5D5;_tmp5D9.nullable=_tmp5D2;_tmp5D9.bounds=_tmp5D3;_tmp5D9.zero_term=
_tmp5D4;_tmp5D9.ptrloc=ptrloc;_tmp5D9;});_tmp5D8.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D8;});_tmp5D7;});
_tmp5D6->tl=ans;_tmp5D6;});yyval=Cyc_YY28(ans);break;}}}case 183: _LL2C6: yyval=Cyc_YY54(
0);break;case 184: _LL2C7: yyval=Cyc_YY54(({struct Cyc_List_List*_tmp5DA=_cycalloc(
sizeof(*_tmp5DA));_tmp5DA->hd=Cyc_yyget_YY53(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp5DA->tl=Cyc_yyget_YY54(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5DA;}));break;case 185:
_LL2C8: yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Numelts_ptrqual_struct*_tmp5DB=
_cycalloc(sizeof(*_tmp5DB));_tmp5DB[0]=({struct Cyc_Absyn_Numelts_ptrqual_struct
_tmp5DC;_tmp5DC.tag=0;_tmp5DC.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5DC;});_tmp5DB;}));
break;case 186: _LL2C9: yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Region_ptrqual_struct*
_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD[0]=({struct Cyc_Absyn_Region_ptrqual_struct
_tmp5DE;_tmp5DE.tag=1;_tmp5DE.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5DE;});_tmp5DD;}));
break;case 187: _LL2CA: yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Thin_ptrqual_struct*
_tmp5DF=_cycalloc_atomic(sizeof(*_tmp5DF));_tmp5DF[0]=({struct Cyc_Absyn_Thin_ptrqual_struct
_tmp5E0;_tmp5E0.tag=2;_tmp5E0;});_tmp5DF;}));break;case 188: _LL2CB: yyval=Cyc_YY53((
void*)({struct Cyc_Absyn_Fat_ptrqual_struct*_tmp5E1=_cycalloc_atomic(sizeof(*
_tmp5E1));_tmp5E1[0]=({struct Cyc_Absyn_Fat_ptrqual_struct _tmp5E2;_tmp5E2.tag=3;
_tmp5E2;});_tmp5E1;}));break;case 189: _LL2CC: yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Zeroterm_ptrqual_struct*
_tmp5E3=_cycalloc_atomic(sizeof(*_tmp5E3));_tmp5E3[0]=({struct Cyc_Absyn_Zeroterm_ptrqual_struct
_tmp5E4;_tmp5E4.tag=4;_tmp5E4;});_tmp5E3;}));break;case 190: _LL2CD: yyval=Cyc_YY53((
void*)({struct Cyc_Absyn_Nozeroterm_ptrqual_struct*_tmp5E5=_cycalloc_atomic(
sizeof(*_tmp5E5));_tmp5E5[0]=({struct Cyc_Absyn_Nozeroterm_ptrqual_struct _tmp5E6;
_tmp5E6.tag=5;_tmp5E6;});_tmp5E5;}));break;case 191: _LL2CE: yyval=Cyc_YY53((void*)({
struct Cyc_Absyn_Notnull_ptrqual_struct*_tmp5E7=_cycalloc_atomic(sizeof(*_tmp5E7));
_tmp5E7[0]=({struct Cyc_Absyn_Notnull_ptrqual_struct _tmp5E8;_tmp5E8.tag=6;_tmp5E8;});
_tmp5E7;}));break;case 192: _LL2CF: yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Nullable_ptrqual_struct*
_tmp5E9=_cycalloc_atomic(sizeof(*_tmp5E9));_tmp5E9[0]=({struct Cyc_Absyn_Nullable_ptrqual_struct
_tmp5EA;_tmp5EA.tag=7;_tmp5EA;});_tmp5E9;}));break;case 193: _LL2D0: {struct Cyc_Position_Segment*
loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);yyval=Cyc_YY1(({struct _tuple18*_tmp5EB=_cycalloc(
sizeof(*_tmp5EB));_tmp5EB->f1=loc;_tmp5EB->f2=Cyc_Absyn_true_conref;_tmp5EB->f3=
Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5EB;}));
break;}case 194: _LL2D1: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);yyval=Cyc_YY1(({
struct _tuple18*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC->f1=loc;_tmp5EC->f2=
Cyc_Absyn_false_conref;_tmp5EC->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5EC;}));break;}case 195:
_LL2D2: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);yyval=Cyc_YY1(({
struct _tuple18*_tmp5ED=_cycalloc(sizeof(*_tmp5ED));_tmp5ED->f1=loc;_tmp5ED->f2=
Cyc_Absyn_true_conref;_tmp5ED->f3=Cyc_Absyn_bounds_dyneither_conref;_tmp5ED;}));
break;}case 196: _LL2D3: yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);break;case 197:
_LL2D4: yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));
_tmp5EE[0]=({struct Cyc_Absyn_Upper_b_struct _tmp5EF;_tmp5EF.tag=1;_tmp5EF.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5EF;});_tmp5EE;})));
break;case 198: _LL2D5: yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 199: _LL2D6: yyval=Cyc_YY51(Cyc_Absyn_true_conref);break;case 200: _LL2D7:
yyval=Cyc_YY51(Cyc_Absyn_false_conref);break;case 201: _LL2D8: yyval=Cyc_YY44(Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,0));break;case 202: _LL2D9: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_TopRgnKind,
1);yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 203:
_LL2DA: yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,0));
break;case 204: _LL2DB: yyval=Cyc_YY25(Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line)));break;case
205: _LL2DC: yyval=Cyc_YY25(Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 206: _LL2DD:
yyval=Cyc_YY39(({struct _tuple23*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp5F0->f2=0;
_tmp5F0->f3=0;_tmp5F0->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp5F0->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F0;}));break;case 207:
_LL2DE: yyval=Cyc_YY39(({struct _tuple23*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));
_tmp5F1->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));
_tmp5F1->f2=1;_tmp5F1->f3=0;_tmp5F1->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5F1->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F1;}));break;case
208: _LL2DF: {struct _tuple8 _tmp5F3;struct Cyc_Core_Opt*_tmp5F4;struct Cyc_Absyn_Tqual
_tmp5F5;void*_tmp5F6;struct _tuple8*_tmp5F2=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5F3=*_tmp5F2;_tmp5F4=
_tmp5F3.f1;_tmp5F5=_tmp5F3.f2;_tmp5F6=_tmp5F3.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5F7=({struct Cyc_Absyn_VarargInfo*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9->name=
_tmp5F4;_tmp5F9->tq=_tmp5F5;_tmp5F9->type=_tmp5F6;_tmp5F9->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5F9;});yyval=
Cyc_YY39(({struct _tuple23*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8->f1=0;
_tmp5F8->f2=0;_tmp5F8->f3=_tmp5F7;_tmp5F8->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5F8->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F8;}));break;}}
case 209: _LL2E0: {struct _tuple8 _tmp5FB;struct Cyc_Core_Opt*_tmp5FC;struct Cyc_Absyn_Tqual
_tmp5FD;void*_tmp5FE;struct _tuple8*_tmp5FA=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5FB=*_tmp5FA;_tmp5FC=
_tmp5FB.f1;_tmp5FD=_tmp5FB.f2;_tmp5FE=_tmp5FB.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5FF=({struct Cyc_Absyn_VarargInfo*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->name=
_tmp5FC;_tmp601->tq=_tmp5FD;_tmp601->type=_tmp5FE;_tmp601->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp601;});yyval=
Cyc_YY39(({struct _tuple23*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp600->f2=0;
_tmp600->f3=_tmp5FF;_tmp600->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp600->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp600;}));break;}}
case 210: _LL2E1: yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp602=_cycalloc(sizeof(*_tmp602));_tmp602[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp603;_tmp603.tag=1;_tmp603.f1=0;_tmp603;});_tmp602;})));break;case 211: _LL2E2:
yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))));
break;case 212: _LL2E3: yyval=Cyc_YY49(0);break;case 213: _LL2E4: yyval=Cyc_YY49(({
struct Cyc_Core_Opt*_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604->v=(void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp606;_tmp606.tag=24;_tmp606.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp606;});_tmp605;});
_tmp604;}));break;case 214: _LL2E5: yyval=Cyc_YY50(0);break;case 215: _LL2E6: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 216: _LL2E7: {
struct Cyc_Absyn_Less_kb_struct*_tmp607=({struct Cyc_Absyn_Less_kb_struct*_tmp60E=
_cycalloc(sizeof(*_tmp60E));_tmp60E[0]=({struct Cyc_Absyn_Less_kb_struct _tmp60F;
_tmp60F.tag=2;_tmp60F.f1=0;_tmp60F.f2=Cyc_Absyn_TopRgnKind;_tmp60F;});_tmp60E;});
struct _dyneither_ptr _tmp608=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);void*_tmp609=Cyc_Parse_id2type(
_tmp608,(void*)_tmp607);yyval=Cyc_YY50(({struct Cyc_List_List*_tmp60A=_cycalloc(
sizeof(*_tmp60A));_tmp60A->hd=({struct _tuple10*_tmp60B=_cycalloc(sizeof(*_tmp60B));
_tmp60B->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp60C=_cycalloc(sizeof(*
_tmp60C));_tmp60C[0]=({struct Cyc_Absyn_JoinEff_struct _tmp60D;_tmp60D.tag=24;
_tmp60D.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]);_tmp60D;});_tmp60C;});_tmp60B->f2=_tmp609;_tmp60B;});_tmp60A->tl=0;
_tmp60A;}));break;}case 217: _LL2E8: {struct Cyc_Absyn_Less_kb_struct*_tmp610=({
struct Cyc_Absyn_Less_kb_struct*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617[0]=({
struct Cyc_Absyn_Less_kb_struct _tmp618;_tmp618.tag=2;_tmp618.f1=0;_tmp618.f2=Cyc_Absyn_TopRgnKind;
_tmp618;});_tmp617;});struct _dyneither_ptr _tmp611=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);void*_tmp612=Cyc_Parse_id2type(
_tmp611,(void*)_tmp610);yyval=Cyc_YY50(({struct Cyc_List_List*_tmp613=_cycalloc(
sizeof(*_tmp613));_tmp613->hd=({struct _tuple10*_tmp614=_cycalloc(sizeof(*_tmp614));
_tmp614->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp615=_cycalloc(sizeof(*
_tmp615));_tmp615[0]=({struct Cyc_Absyn_JoinEff_struct _tmp616;_tmp616.tag=24;
_tmp616.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]);_tmp616;});_tmp615;});_tmp614->f2=_tmp612;_tmp614;});_tmp613->tl=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp613;}));break;}
case 218: _LL2E9: yyval=Cyc_YY31(0);break;case 219: _LL2EA: if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),({const char*_tmp619="inject";_tag_dyneither(_tmp619,sizeof(char),
7);}))!= 0)Cyc_Parse_err(({const char*_tmp61A="missing type in function declaration";
_tag_dyneither(_tmp61A,sizeof(char),37);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY31(
1);break;case 220: _LL2EB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 221: _LL2EC: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 222: _LL2ED:
yyval=Cyc_YY40(0);break;case 223: _LL2EE: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 224: _LL2EF: yyval=Cyc_YY40(({struct Cyc_List_List*
_tmp61B=_cycalloc(sizeof(*_tmp61B));_tmp61B->hd=(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp61C=_cycalloc(sizeof(*_tmp61C));_tmp61C[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp61D;_tmp61D.tag=25;_tmp61D.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp61D;});_tmp61C;});
_tmp61B->tl=0;_tmp61B;}));break;case 225: _LL2F0: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_EffKind,0);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E->hd=
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp61E->tl=
0;_tmp61E;}));break;case 226: _LL2F1: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_TopRgnKind,1);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F->hd=(
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp620=_cycalloc(sizeof(*_tmp620));
_tmp620[0]=({struct Cyc_Absyn_AccessEff_struct _tmp621;_tmp621.tag=23;_tmp621.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp621;});_tmp620;});_tmp61F->tl=0;_tmp61F;}));break;case 227: _LL2F2: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Absyn_TopRgnKind,
1);yyval=Cyc_YY40(({struct Cyc_List_List*_tmp622=_cycalloc(sizeof(*_tmp622));
_tmp622->hd=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp623=_cycalloc(sizeof(*
_tmp623));_tmp623[0]=({struct Cyc_Absyn_AccessEff_struct _tmp624;_tmp624.tag=23;
_tmp624.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp624;});_tmp623;});_tmp622->tl=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp622;}));break;case 228:
_LL2F3: yyval=Cyc_YY38(({struct Cyc_List_List*_tmp625=_cycalloc(sizeof(*_tmp625));
_tmp625->hd=Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp625->tl=0;_tmp625;}));break;case 229: _LL2F4: yyval=Cyc_YY38(({
struct Cyc_List_List*_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp626->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp626;}));break;
case 230: _LL2F5: {struct _tuple22 _tmp628;struct Cyc_Absyn_Tqual _tmp629;struct Cyc_List_List*
_tmp62A;struct Cyc_List_List*_tmp62B;struct _tuple22*_tmp627=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp628=*_tmp627;_tmp629=
_tmp628.f1;_tmp62A=_tmp628.f2;_tmp62B=_tmp628.f3;if(_tmp629.loc == 0)_tmp629.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp62D;struct _tuple1*
_tmp62E;struct Cyc_List_List*_tmp62F;struct Cyc_Parse_Declarator*_tmp62C=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp62D=*_tmp62C;
_tmp62E=_tmp62D.id;_tmp62F=_tmp62D.tms;{void*_tmp630=Cyc_Parse_speclist2typ(
_tmp62A,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp632;void*_tmp633;struct
Cyc_List_List*_tmp634;struct Cyc_List_List*_tmp635;struct _tuple12 _tmp631=Cyc_Parse_apply_tms(
_tmp629,_tmp630,_tmp62B,_tmp62F);_tmp632=_tmp631.f1;_tmp633=_tmp631.f2;_tmp634=
_tmp631.f3;_tmp635=_tmp631.f4;if(_tmp634 != 0)Cyc_Parse_err(({const char*_tmp636="parameter with bad type params";
_tag_dyneither(_tmp636,sizeof(char),31);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp62E))Cyc_Parse_err(({const char*_tmp637="parameter cannot be qualified with a namespace";
_tag_dyneither(_tmp637,sizeof(char),47);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp638=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp63C=_cycalloc(sizeof(*
_tmp63C));_tmp63C->v=(*_tmp62E).f2;_tmp63C;});if(_tmp635 != 0)({void*_tmp639=0;
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp63A="extra attributes on parameter, ignoring";
_tag_dyneither(_tmp63A,sizeof(char),40);}),_tag_dyneither(_tmp639,sizeof(void*),
0));});yyval=Cyc_YY37(({struct _tuple8*_tmp63B=_cycalloc(sizeof(*_tmp63B));
_tmp63B->f1=_tmp638;_tmp63B->f2=_tmp632;_tmp63B->f3=_tmp633;_tmp63B;}));break;}}}}
case 231: _LL2F6: {struct _tuple22 _tmp63E;struct Cyc_Absyn_Tqual _tmp63F;struct Cyc_List_List*
_tmp640;struct Cyc_List_List*_tmp641;struct _tuple22*_tmp63D=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp63E=*_tmp63D;_tmp63F=
_tmp63E.f1;_tmp640=_tmp63E.f2;_tmp641=_tmp63E.f3;if(_tmp63F.loc == 0)_tmp63F.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);{void*_tmp642=Cyc_Parse_speclist2typ(_tmp640,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp641 != 0)({
void*_tmp643=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp644="bad attributes on parameter, ignoring";_tag_dyneither(_tmp644,sizeof(
char),38);}),_tag_dyneither(_tmp643,sizeof(void*),0));});yyval=Cyc_YY37(({struct
_tuple8*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645->f1=0;_tmp645->f2=_tmp63F;
_tmp645->f3=_tmp642;_tmp645;}));break;}}case 232: _LL2F7: {struct _tuple22 _tmp647;
struct Cyc_Absyn_Tqual _tmp648;struct Cyc_List_List*_tmp649;struct Cyc_List_List*
_tmp64A;struct _tuple22*_tmp646=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp647=*_tmp646;_tmp648=
_tmp647.f1;_tmp649=_tmp647.f2;_tmp64A=_tmp647.f3;if(_tmp648.loc == 0)_tmp648.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{void*_tmp64B=Cyc_Parse_speclist2typ(_tmp649,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp64C=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp64E;void*_tmp64F;struct Cyc_List_List*_tmp650;struct Cyc_List_List*
_tmp651;struct _tuple12 _tmp64D=Cyc_Parse_apply_tms(_tmp648,_tmp64B,_tmp64A,
_tmp64C);_tmp64E=_tmp64D.f1;_tmp64F=_tmp64D.f2;_tmp650=_tmp64D.f3;_tmp651=
_tmp64D.f4;if(_tmp650 != 0)({void*_tmp652=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp653="bad type parameters on formal argument, ignoring";_tag_dyneither(
_tmp653,sizeof(char),49);}),_tag_dyneither(_tmp652,sizeof(void*),0));});if(
_tmp651 != 0)({void*_tmp654=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp655="bad attributes on parameter, ignoring";_tag_dyneither(_tmp655,sizeof(
char),38);}),_tag_dyneither(_tmp654,sizeof(void*),0));});yyval=Cyc_YY37(({struct
_tuple8*_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656->f1=0;_tmp656->f2=_tmp64E;
_tmp656->f3=_tmp64F;_tmp656;}));break;}}case 233: _LL2F8: yyval=Cyc_YY36(((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 234: _LL2F9:
yyval=Cyc_YY36(({struct Cyc_List_List*_tmp657=_cycalloc(sizeof(*_tmp657));_tmp657->hd=({
struct _dyneither_ptr*_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp658;});_tmp657->tl=
0;_tmp657;}));break;case 235: _LL2FA: yyval=Cyc_YY36(({struct Cyc_List_List*_tmp659=
_cycalloc(sizeof(*_tmp659));_tmp659->hd=({struct _dyneither_ptr*_tmp65A=_cycalloc(
sizeof(*_tmp65A));_tmp65A[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp65A;});_tmp659->tl=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp659;}));break;
case 236: _LL2FB: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 237: _LL2FC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 238: _LL2FD: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp65B=_cycalloc(sizeof(*_tmp65B));
_tmp65B[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp65C;_tmp65C.tag=37;
_tmp65C.f1=0;_tmp65C.f2=0;_tmp65C;});_tmp65B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 239:
_LL2FE: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp65E;_tmp65E.tag=37;_tmp65E.f1=0;_tmp65E.f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp65E;});_tmp65D;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 240: _LL2FF: yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp65F=_cycalloc(sizeof(*_tmp65F));
_tmp65F[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp660;_tmp660.tag=37;
_tmp660.f1=0;_tmp660.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp660;});_tmp65F;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 241:
_LL300: {struct Cyc_Absyn_Vardecl*_tmp661=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664->f1=Cyc_Absyn_Loc_n;_tmp664->f2=({
struct _dyneither_ptr*_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp665;});_tmp664;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp661->tq).real_const=
1;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp663;_tmp663.tag=29;_tmp663.f1=_tmp661;_tmp663.f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp663.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp663.f4=0;
_tmp663;});_tmp662;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 242:
_LL301: yyval=Cyc_YY6(({struct Cyc_List_List*_tmp666=_cycalloc(sizeof(*_tmp666));
_tmp666->hd=({struct _tuple26*_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->f1=0;
_tmp667->f2=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp667;});_tmp666->tl=0;_tmp666;}));break;case 243: _LL302: yyval=Cyc_YY6(({struct
Cyc_List_List*_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668->hd=({struct _tuple26*
_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp669->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp669;});_tmp668->tl=
0;_tmp668;}));break;case 244: _LL303: yyval=Cyc_YY6(({struct Cyc_List_List*_tmp66A=
_cycalloc(sizeof(*_tmp66A));_tmp66A->hd=({struct _tuple26*_tmp66B=_cycalloc(
sizeof(*_tmp66B));_tmp66B->f1=0;_tmp66B->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp66B;});_tmp66A->tl=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp66A;}));break;
case 245: _LL304: yyval=Cyc_YY6(({struct Cyc_List_List*_tmp66C=_cycalloc(sizeof(*
_tmp66C));_tmp66C->hd=({struct _tuple26*_tmp66D=_cycalloc(sizeof(*_tmp66D));
_tmp66D->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp66D->f2=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp66D;});_tmp66C->tl=Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp66C;}));break;case
246: _LL305: yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
break;case 247: _LL306: yyval=Cyc_YY41(({struct Cyc_List_List*_tmp66E=_cycalloc(
sizeof(*_tmp66E));_tmp66E->hd=Cyc_yyget_YY42(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp66E->tl=0;_tmp66E;}));break;case 248: _LL307: yyval=Cyc_YY41(({
struct Cyc_List_List*_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F->hd=Cyc_yyget_YY42(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp66F->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp66F;}));break;
case 249: _LL308: yyval=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp670=_cycalloc(sizeof(*_tmp670));_tmp670[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp671;_tmp671.tag=0;_tmp671.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp671;});_tmp670;}));
break;case 250: _LL309: yyval=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp672=_cycalloc(sizeof(*_tmp672));_tmp672[0]=({struct Cyc_Absyn_FieldName_struct
_tmp673;_tmp673.tag=1;_tmp673.f1=({struct _dyneither_ptr*_tmp674=_cycalloc(
sizeof(*_tmp674));_tmp674[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp674;});_tmp673;});
_tmp672;}));break;case 251: _LL30A: {struct _tuple22 _tmp676;struct Cyc_Absyn_Tqual
_tmp677;struct Cyc_List_List*_tmp678;struct Cyc_List_List*_tmp679;struct _tuple22*
_tmp675=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp676=*_tmp675;_tmp677=_tmp676.f1;_tmp678=_tmp676.f2;_tmp679=_tmp676.f3;{void*
_tmp67A=Cyc_Parse_speclist2typ(_tmp678,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp679 != 0)({
void*_tmp67B=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp67C="ignoring attributes in type";_tag_dyneither(_tmp67C,sizeof(char),28);}),
_tag_dyneither(_tmp67B,sizeof(void*),0));});yyval=Cyc_YY37(({struct _tuple8*
_tmp67D=_cycalloc(sizeof(*_tmp67D));_tmp67D->f1=0;_tmp67D->f2=_tmp677;_tmp67D->f3=
_tmp67A;_tmp67D;}));break;}}case 252: _LL30B: {struct _tuple22 _tmp67F;struct Cyc_Absyn_Tqual
_tmp680;struct Cyc_List_List*_tmp681;struct Cyc_List_List*_tmp682;struct _tuple22*
_tmp67E=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp67F=*_tmp67E;_tmp680=_tmp67F.f1;_tmp681=_tmp67F.f2;_tmp682=_tmp67F.f3;{void*
_tmp683=Cyc_Parse_speclist2typ(_tmp681,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp684=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct _tuple12 _tmp685=Cyc_Parse_apply_tms(_tmp680,_tmp683,_tmp682,_tmp684);if(
_tmp685.f3 != 0)({void*_tmp686=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp687="bad type params, ignoring";_tag_dyneither(_tmp687,sizeof(char),26);}),
_tag_dyneither(_tmp686,sizeof(void*),0));});if(_tmp685.f4 != 0)({void*_tmp688=0;
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp689="bad specifiers, ignoring";
_tag_dyneither(_tmp689,sizeof(char),25);}),_tag_dyneither(_tmp688,sizeof(void*),
0));});yyval=Cyc_YY37(({struct _tuple8*_tmp68A=_cycalloc(sizeof(*_tmp68A));
_tmp68A->f1=0;_tmp68A->f2=_tmp685.f1;_tmp68A->f3=_tmp685.f2;_tmp68A;}));break;}}
case 253: _LL30C: yyval=Cyc_YY44((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);break;case 254: _LL30D:
yyval=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp68B=_cycalloc(sizeof(*
_tmp68B));_tmp68B[0]=({struct Cyc_Absyn_JoinEff_struct _tmp68C;_tmp68C.tag=24;
_tmp68C.f1=0;_tmp68C;});_tmp68B;}));break;case 255: _LL30E: yyval=Cyc_YY44((void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp68D=_cycalloc(sizeof(*_tmp68D));_tmp68D[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp68E;_tmp68E.tag=24;_tmp68E.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp68E;});_tmp68D;}));
break;case 256: _LL30F: yyval=Cyc_YY44((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp690;_tmp690.tag=25;_tmp690.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp690;});_tmp68F;}));
break;case 257: _LL310: yyval=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp692;_tmp692.tag=24;_tmp692.f1=({struct Cyc_List_List*_tmp693=_cycalloc(
sizeof(*_tmp693));_tmp693->hd=Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp693->tl=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp693;});_tmp692;});
_tmp691;}));break;case 258: _LL311: yyval=Cyc_YY40(({struct Cyc_List_List*_tmp694=
_cycalloc(sizeof(*_tmp694));_tmp694->hd=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp694->tl=0;_tmp694;}));
break;case 259: _LL312: yyval=Cyc_YY40(({struct Cyc_List_List*_tmp695=_cycalloc(
sizeof(*_tmp695));_tmp695->hd=Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp695->tl=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp695;}));break;case
260: _LL313: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*_tmp696=_cycalloc(
sizeof(*_tmp696));_tmp696->tms=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp696;}));break;case 261:
_LL314: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
262: _LL315: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*_tmp697=_cycalloc(
sizeof(*_tmp697));_tmp697->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp697;}));break;
case 263: _LL316: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 264: _LL317: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*_tmp698=
_cycalloc(sizeof(*_tmp698));_tmp698->tms=({struct Cyc_List_List*_tmp699=_cycalloc(
sizeof(*_tmp699));_tmp699->hd=(void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp69A=
_cycalloc(sizeof(*_tmp69A));_tmp69A[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp69B;_tmp69B.tag=0;_tmp69B.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp69B.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp69B;});
_tmp69A;});_tmp699->tl=0;_tmp699;});_tmp698;}));break;case 265: _LL318: yyval=Cyc_YY30(({
struct Cyc_Parse_Abstractdeclarator*_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C->tms=({
struct Cyc_List_List*_tmp69D=_cycalloc(sizeof(*_tmp69D));_tmp69D->hd=(void*)({
struct Cyc_Absyn_Carray_mod_struct*_tmp69E=_cycalloc(sizeof(*_tmp69E));_tmp69E[0]=({
struct Cyc_Absyn_Carray_mod_struct _tmp69F;_tmp69F.tag=0;_tmp69F.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp69F.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp69F;});
_tmp69E;});_tmp69D->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp69D;});_tmp69C;}));break;case 266: _LL319: yyval=Cyc_YY30(({
struct Cyc_Parse_Abstractdeclarator*_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0->tms=({
struct Cyc_List_List*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));_tmp6A1->hd=(void*)({
struct Cyc_Absyn_ConstArray_mod_struct*_tmp6A2=_cycalloc(sizeof(*_tmp6A2));
_tmp6A2[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp6A3;_tmp6A3.tag=1;_tmp6A3.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp6A3.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp6A3.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp6A3;});_tmp6A2;});_tmp6A1->tl=0;_tmp6A1;});
_tmp6A0;}));break;case 267: _LL31A: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*
_tmp6A4=_cycalloc(sizeof(*_tmp6A4));_tmp6A4->tms=({struct Cyc_List_List*_tmp6A5=
_cycalloc(sizeof(*_tmp6A5));_tmp6A5->hd=(void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6A7;_tmp6A7.tag=1;_tmp6A7.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6A7.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A7.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6A7;});
_tmp6A6;});_tmp6A5->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->tms;_tmp6A5;});_tmp6A4;}));break;case 268: _LL31B: yyval=Cyc_YY30(({
struct Cyc_Parse_Abstractdeclarator*_tmp6A8=_cycalloc(sizeof(*_tmp6A8));_tmp6A8->tms=({
struct Cyc_List_List*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));_tmp6A9->hd=(void*)({
struct Cyc_Absyn_Function_mod_struct*_tmp6AA=_cycalloc(sizeof(*_tmp6AA));_tmp6AA[
0]=({struct Cyc_Absyn_Function_mod_struct _tmp6AB;_tmp6AB.tag=3;_tmp6AB.f1=(void*)((
void*)({struct Cyc_Absyn_WithTypes_struct*_tmp6AC=_cycalloc(sizeof(*_tmp6AC));
_tmp6AC[0]=({struct Cyc_Absyn_WithTypes_struct _tmp6AD;_tmp6AD.tag=1;_tmp6AD.f1=0;
_tmp6AD.f2=0;_tmp6AD.f3=0;_tmp6AD.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6AD.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6AD;});_tmp6AC;}));
_tmp6AB;});_tmp6AA;});_tmp6A9->tl=0;_tmp6A9;});_tmp6A8;}));break;case 269: _LL31C: {
struct _tuple23 _tmp6AF;struct Cyc_List_List*_tmp6B0;int _tmp6B1;struct Cyc_Absyn_VarargInfo*
_tmp6B2;struct Cyc_Core_Opt*_tmp6B3;struct Cyc_List_List*_tmp6B4;struct _tuple23*
_tmp6AE=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp6AF=*_tmp6AE;_tmp6B0=_tmp6AF.f1;_tmp6B1=_tmp6AF.f2;_tmp6B2=_tmp6AF.f3;
_tmp6B3=_tmp6AF.f4;_tmp6B4=_tmp6AF.f5;yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*
_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5->tms=({struct Cyc_List_List*_tmp6B6=
_cycalloc(sizeof(*_tmp6B6));_tmp6B6->hd=(void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6B8;_tmp6B8.tag=3;_tmp6B8.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6BA;_tmp6BA.tag=1;_tmp6BA.f1=_tmp6B0;_tmp6BA.f2=_tmp6B1;_tmp6BA.f3=_tmp6B2;
_tmp6BA.f4=_tmp6B3;_tmp6BA.f5=_tmp6B4;_tmp6BA;});_tmp6B9;}));_tmp6B8;});_tmp6B7;});
_tmp6B6->tl=0;_tmp6B6;});_tmp6B5;}));break;}case 270: _LL31D: yyval=Cyc_YY30(({
struct Cyc_Parse_Abstractdeclarator*_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB->tms=({
struct Cyc_List_List*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC->hd=(void*)({
struct Cyc_Absyn_Function_mod_struct*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));_tmp6BD[
0]=({struct Cyc_Absyn_Function_mod_struct _tmp6BE;_tmp6BE.tag=3;_tmp6BE.f1=(void*)((
void*)({struct Cyc_Absyn_WithTypes_struct*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));
_tmp6BF[0]=({struct Cyc_Absyn_WithTypes_struct _tmp6C0;_tmp6C0.tag=1;_tmp6C0.f1=0;
_tmp6C0.f2=0;_tmp6C0.f3=0;_tmp6C0.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6C0.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6C0;});_tmp6BF;}));
_tmp6BE;});_tmp6BD;});_tmp6BC->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp6BC;});_tmp6BB;}));
break;case 271: _LL31E: {struct _tuple23 _tmp6C2;struct Cyc_List_List*_tmp6C3;int
_tmp6C4;struct Cyc_Absyn_VarargInfo*_tmp6C5;struct Cyc_Core_Opt*_tmp6C6;struct Cyc_List_List*
_tmp6C7;struct _tuple23*_tmp6C1=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6C2=*_tmp6C1;_tmp6C3=
_tmp6C2.f1;_tmp6C4=_tmp6C2.f2;_tmp6C5=_tmp6C2.f3;_tmp6C6=_tmp6C2.f4;_tmp6C7=
_tmp6C2.f5;yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*_tmp6C8=_cycalloc(
sizeof(*_tmp6C8));_tmp6C8->tms=({struct Cyc_List_List*_tmp6C9=_cycalloc(sizeof(*
_tmp6C9));_tmp6C9->hd=(void*)({struct Cyc_Absyn_Function_mod_struct*_tmp6CA=
_cycalloc(sizeof(*_tmp6CA));_tmp6CA[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6CB;_tmp6CB.tag=3;_tmp6CB.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6CC=_cycalloc(sizeof(*_tmp6CC));_tmp6CC[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6CD;_tmp6CD.tag=1;_tmp6CD.f1=_tmp6C3;_tmp6CD.f2=_tmp6C4;_tmp6CD.f3=_tmp6C5;
_tmp6CD.f4=_tmp6C6;_tmp6CD.f5=_tmp6C7;_tmp6CD;});_tmp6CC;}));_tmp6CB;});_tmp6CA;});
_tmp6C9->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp6C9;});_tmp6C8;}));break;}case 272: _LL31F: {struct
Cyc_List_List*_tmp6CE=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=Cyc_YY30(({
struct Cyc_Parse_Abstractdeclarator*_tmp6CF=_cycalloc(sizeof(*_tmp6CF));_tmp6CF->tms=({
struct Cyc_List_List*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));_tmp6D0->hd=(void*)({
struct Cyc_Absyn_TypeParams_mod_struct*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));
_tmp6D1[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp6D2;_tmp6D2.tag=4;_tmp6D2.f1=
_tmp6CE;_tmp6D2.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6D2.f3=0;
_tmp6D2;});_tmp6D1;});_tmp6D0->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6D0;});_tmp6CF;}));
break;}case 273: _LL320: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*
_tmp6D3=_cycalloc(sizeof(*_tmp6D3));_tmp6D3->tms=({struct Cyc_List_List*_tmp6D4=
_cycalloc(sizeof(*_tmp6D4));_tmp6D4->hd=(void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp6D5=_cycalloc(sizeof(*_tmp6D5));_tmp6D5[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp6D6;_tmp6D6.tag=5;_tmp6D6.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6D6.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6D6;});_tmp6D5;});
_tmp6D4->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp6D4;});_tmp6D3;}));break;case 274: _LL321: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 275: _LL322: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 276: _LL323:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 277:
_LL324: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
278: _LL325: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 279: _LL326: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 280: _LL327: yyval=Cyc_YY9(Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_ResetRegion_s_struct*
_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7[0]=({struct Cyc_Absyn_ResetRegion_s_struct
_tmp6D8;_tmp6D8.tag=16;_tmp6D8.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6D8;});_tmp6D7;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 281: _LL328: yyval=Cyc_YY4(0);break;case 282:
_LL329: if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp6D9="open";
_tag_dyneither(_tmp6D9,sizeof(char),5);}))!= 0)Cyc_Parse_err(({const char*_tmp6DA="expecting `open'";
_tag_dyneither(_tmp6DA,sizeof(char),17);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=Cyc_YY4((
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));break;case 283: _LL32A: yyval=Cyc_YY9(Cyc_Absyn_new_stmt((void*)({
struct Cyc_Absyn_Label_s_struct*_tmp6DB=_cycalloc(sizeof(*_tmp6DB));_tmp6DB[0]=({
struct Cyc_Absyn_Label_s_struct _tmp6DC;_tmp6DC.tag=13;_tmp6DC.f1=({struct
_dyneither_ptr*_tmp6DD=_cycalloc(sizeof(*_tmp6DD));_tmp6DD[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6DD;});_tmp6DC.f2=
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6DC;});
_tmp6DB;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 284: _LL32B: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 285: _LL32C: yyval=Cyc_YY9(Cyc_Absyn_exp_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 286:
_LL32D: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 287:
_LL32E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 288: _LL32F: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0)));
break;case 289: _LL330: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 290: _LL331:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 291:
_LL332: yyval=Cyc_YY9(Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 292:
_LL333: yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct
Cyc_Absyn_Fn_d_struct*_tmp6DE=_cycalloc(sizeof(*_tmp6DE));_tmp6DE[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp6DF;_tmp6DF.tag=1;_tmp6DF.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6DF;});_tmp6DE;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(
0)));break;case 293: _LL334: yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp6E0=_cycalloc(sizeof(*_tmp6E0));_tmp6E0[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp6E1;_tmp6E1.tag=1;_tmp6E1.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6E1;});_tmp6E0;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));break;case 294: _LL335: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 295: _LL336: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 296:
_LL337: yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 297:
_LL338: {struct Cyc_Absyn_Exp*_tmp6E2=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp6E3=_cycalloc(sizeof(*_tmp6E3));_tmp6E3[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp6E4;_tmp6E4.tag=2;_tmp6E4.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6E4;});_tmp6E3;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(_tmp6E2,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 298:
_LL339: {struct Cyc_Absyn_Exp*_tmp6E5=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(
Cyc_Absyn_switch_stmt(_tmp6E5,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 299:
_LL33A: yyval=Cyc_YY9(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 300:
_LL33B: yyval=Cyc_YY10(0);break;case 301: _LL33C: yyval=Cyc_YY10(({struct Cyc_List_List*
_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6E7=_cycalloc(sizeof(*_tmp6E7));_tmp6E7->pattern=Cyc_Absyn_new_pat((void*)&
Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp6E7->pat_vars=
0;_tmp6E7->where_clause=0;_tmp6E7->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E7->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6E7;});
_tmp6E6->tl=0;_tmp6E6;}));break;case 302: _LL33D: yyval=Cyc_YY10(({struct Cyc_List_List*
_tmp6E8=_cycalloc(sizeof(*_tmp6E8));_tmp6E8->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6E9=_cycalloc(sizeof(*_tmp6E9));_tmp6E9->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6E9->pat_vars=0;
_tmp6E9->where_clause=0;_tmp6E9->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp6E9->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp6E9;});_tmp6E8->tl=Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E8;}));break;case 303:
_LL33E: yyval=Cyc_YY10(({struct Cyc_List_List*_tmp6EA=_cycalloc(sizeof(*_tmp6EA));
_tmp6EA->hd=({struct Cyc_Absyn_Switch_clause*_tmp6EB=_cycalloc(sizeof(*_tmp6EB));
_tmp6EB->pattern=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp6EB->pat_vars=0;_tmp6EB->where_clause=0;_tmp6EB->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6EB->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp6EB;});
_tmp6EA->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp6EA;}));break;case 304: _LL33F: yyval=Cyc_YY10(({struct Cyc_List_List*
_tmp6EC=_cycalloc(sizeof(*_tmp6EC));_tmp6EC->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6ED=_cycalloc(sizeof(*_tmp6ED));_tmp6ED->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp6ED->pat_vars=0;
_tmp6ED->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6ED->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp6ED->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6ED;});
_tmp6EC->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp6EC;}));break;case 305: _LL340: yyval=Cyc_YY10(({struct Cyc_List_List*
_tmp6EE=_cycalloc(sizeof(*_tmp6EE));_tmp6EE->hd=({struct Cyc_Absyn_Switch_clause*
_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp6EF->pat_vars=0;
_tmp6EF->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6EF->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6EF->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6EF;});
_tmp6EE->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp6EE;}));break;case 306: _LL341: yyval=Cyc_YY9(Cyc_Absyn_while_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 307:
_LL342: yyval=Cyc_YY9(Cyc_Absyn_do_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 308:
_LL343: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 309:
_LL344: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 310:
_LL345: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 311:
_LL346: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
312: _LL347: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 313: _LL348: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 314:
_LL349: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 315:
_LL34A: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 316:
_LL34B: {struct Cyc_List_List*_tmp6F0=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp6F1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp6F0,_tmp6F1));break;}case 317: _LL34C: {struct
Cyc_List_List*_tmp6F2=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp6F3=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp6F2,
_tmp6F3));break;}case 318: _LL34D: {struct Cyc_List_List*_tmp6F4=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp6F5=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp6F4,_tmp6F5));break;}case 319: _LL34E: {struct
Cyc_List_List*_tmp6F6=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp6F7=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp6F6,_tmp6F7));break;}case 320: _LL34F: yyval=Cyc_YY9(
Cyc_Absyn_goto_stmt(({struct _dyneither_ptr*_tmp6F8=_cycalloc(sizeof(*_tmp6F8));
_tmp6F8[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp6F8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
321: _LL350: yyval=Cyc_YY9(Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
322: _LL351: yyval=Cyc_YY9(Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
323: _LL352: yyval=Cyc_YY9(Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
324: _LL353: yyval=Cyc_YY9(Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
325: _LL354: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
326: _LL355: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)));break;case
327: _LL356: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line)));break;case
328: _LL357: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 329: _LL358: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 330: _LL359: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 331: _LL35A: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 332: _LL35B: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 333:
_LL35C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
334: _LL35D: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 335:
_LL35E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
336: _LL35F: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 337: _LL360: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 338: _LL361: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 339:
_LL362: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
340: _LL363: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 341: _LL364: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 342: _LL365: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 343:
_LL366: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 344:
_LL367: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
345: _LL368: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 346:
_LL369: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 347:
_LL36A: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 348:
_LL36B: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 349:
_LL36C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
350: _LL36D: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 351: _LL36E: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 352:
_LL36F: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
353: _LL370: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 354: _LL371: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 355:
_LL372: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
356: _LL373: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 357: _LL374: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 358:
_LL375: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 359:
_LL376: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
360: _LL377: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,Cyc_Absyn_Unknown_coercion,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 361: _LL378: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 362: _LL379: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));break;case 363: _LL37A: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 364: _LL37B: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 365:
_LL37C: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 366:
_LL37D: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct
Cyc_Absyn_StructField_struct*_tmp6F9=_cycalloc(sizeof(*_tmp6F9));_tmp6F9[0]=({
struct Cyc_Absyn_StructField_struct _tmp6FA;_tmp6FA.tag=0;_tmp6FA.f1=({struct
_dyneither_ptr*_tmp6FB=_cycalloc(sizeof(*_tmp6FB));_tmp6FB[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6FB;});_tmp6FA;});
_tmp6F9;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 367: _LL37E: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp6FC=
_cycalloc_atomic(sizeof(*_tmp6FC));_tmp6FC[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp6FD;_tmp6FD.tag=1;_tmp6FD.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp6FD;});_tmp6FC;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 368: _LL37F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 369: _LL380: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 370: _LL381:
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 371:
_LL382: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 372: _LL383: {
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);{void*_tmp6FE=e->r;union Cyc_Absyn_Cnst _tmp700;struct _tuple4
_tmp701;enum Cyc_Absyn_Sign _tmp702;char _tmp703;union Cyc_Absyn_Cnst _tmp705;struct
_tuple5 _tmp706;enum Cyc_Absyn_Sign _tmp707;short _tmp708;union Cyc_Absyn_Cnst
_tmp70A;struct _tuple6 _tmp70B;enum Cyc_Absyn_Sign _tmp70C;int _tmp70D;union Cyc_Absyn_Cnst
_tmp70F;struct _dyneither_ptr _tmp710;union Cyc_Absyn_Cnst _tmp712;int _tmp713;union
Cyc_Absyn_Cnst _tmp715;struct _dyneither_ptr _tmp716;union Cyc_Absyn_Cnst _tmp718;
struct _tuple7 _tmp719;_LL386: {struct Cyc_Absyn_Const_e_struct*_tmp6FF=(struct Cyc_Absyn_Const_e_struct*)
_tmp6FE;if(_tmp6FF->tag != 0)goto _LL388;else{_tmp700=_tmp6FF->f1;if((_tmp700.Char_c).tag
!= 2)goto _LL388;_tmp701=(struct _tuple4)(_tmp700.Char_c).val;_tmp702=_tmp701.f1;
_tmp703=_tmp701.f2;}}_LL387: yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Char_p_struct*
_tmp71A=_cycalloc_atomic(sizeof(*_tmp71A));_tmp71A[0]=({struct Cyc_Absyn_Char_p_struct
_tmp71B;_tmp71B.tag=10;_tmp71B.f1=_tmp703;_tmp71B;});_tmp71A;}),e->loc));goto
_LL385;_LL388: {struct Cyc_Absyn_Const_e_struct*_tmp704=(struct Cyc_Absyn_Const_e_struct*)
_tmp6FE;if(_tmp704->tag != 0)goto _LL38A;else{_tmp705=_tmp704->f1;if((_tmp705.Short_c).tag
!= 3)goto _LL38A;_tmp706=(struct _tuple5)(_tmp705.Short_c).val;_tmp707=_tmp706.f1;
_tmp708=_tmp706.f2;}}_LL389: yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp71C=_cycalloc_atomic(sizeof(*_tmp71C));_tmp71C[0]=({struct Cyc_Absyn_Int_p_struct
_tmp71D;_tmp71D.tag=9;_tmp71D.f1=_tmp707;_tmp71D.f2=(int)_tmp708;_tmp71D;});
_tmp71C;}),e->loc));goto _LL385;_LL38A: {struct Cyc_Absyn_Const_e_struct*_tmp709=(
struct Cyc_Absyn_Const_e_struct*)_tmp6FE;if(_tmp709->tag != 0)goto _LL38C;else{
_tmp70A=_tmp709->f1;if((_tmp70A.Int_c).tag != 4)goto _LL38C;_tmp70B=(struct _tuple6)(
_tmp70A.Int_c).val;_tmp70C=_tmp70B.f1;_tmp70D=_tmp70B.f2;}}_LL38B: yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmp71E=_cycalloc_atomic(
sizeof(*_tmp71E));_tmp71E[0]=({struct Cyc_Absyn_Int_p_struct _tmp71F;_tmp71F.tag=9;
_tmp71F.f1=_tmp70C;_tmp71F.f2=_tmp70D;_tmp71F;});_tmp71E;}),e->loc));goto _LL385;
_LL38C: {struct Cyc_Absyn_Const_e_struct*_tmp70E=(struct Cyc_Absyn_Const_e_struct*)
_tmp6FE;if(_tmp70E->tag != 0)goto _LL38E;else{_tmp70F=_tmp70E->f1;if((_tmp70F.Float_c).tag
!= 6)goto _LL38E;_tmp710=(struct _dyneither_ptr)(_tmp70F.Float_c).val;}}_LL38D:
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*_tmp720=
_cycalloc(sizeof(*_tmp720));_tmp720[0]=({struct Cyc_Absyn_Float_p_struct _tmp721;
_tmp721.tag=11;_tmp721.f1=_tmp710;_tmp721;});_tmp720;}),e->loc));goto _LL385;
_LL38E: {struct Cyc_Absyn_Const_e_struct*_tmp711=(struct Cyc_Absyn_Const_e_struct*)
_tmp6FE;if(_tmp711->tag != 0)goto _LL390;else{_tmp712=_tmp711->f1;if((_tmp712.Null_c).tag
!= 1)goto _LL390;_tmp713=(int)(_tmp712.Null_c).val;}}_LL38F: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL385;_LL390: {struct Cyc_Absyn_Const_e_struct*
_tmp714=(struct Cyc_Absyn_Const_e_struct*)_tmp6FE;if(_tmp714->tag != 0)goto _LL392;
else{_tmp715=_tmp714->f1;if((_tmp715.String_c).tag != 7)goto _LL392;_tmp716=(
struct _dyneither_ptr)(_tmp715.String_c).val;}}_LL391: Cyc_Parse_err(({const char*
_tmp722="strings cannot occur within patterns";_tag_dyneither(_tmp722,sizeof(
char),37);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));goto _LL385;_LL392: {struct Cyc_Absyn_Const_e_struct*
_tmp717=(struct Cyc_Absyn_Const_e_struct*)_tmp6FE;if(_tmp717->tag != 0)goto _LL394;
else{_tmp718=_tmp717->f1;if((_tmp718.LongLong_c).tag != 5)goto _LL394;_tmp719=(
struct _tuple7)(_tmp718.LongLong_c).val;}}_LL393: Cyc_Parse_unimp(({const char*
_tmp723="long long's in patterns";_tag_dyneither(_tmp723,sizeof(char),24);}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL385;
_LL394:;_LL395: Cyc_Parse_err(({const char*_tmp724="bad constant in case";
_tag_dyneither(_tmp724,sizeof(char),21);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_LL385:;}break;}
case 373: _LL384: yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_struct*
_tmp725=_cycalloc(sizeof(*_tmp725));_tmp725[0]=({struct Cyc_Absyn_UnknownId_p_struct
_tmp726;_tmp726.tag=14;_tmp726.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp726;});_tmp725;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 374:
_LL396: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp727="as";
_tag_dyneither(_tmp727,sizeof(char),3);}))!= 0)Cyc_Parse_err(({const char*_tmp728="expecting `as'";
_tag_dyneither(_tmp728,sizeof(char),15);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_struct*_tmp729=_cycalloc(
sizeof(*_tmp729));_tmp729[0]=({struct Cyc_Absyn_Var_p_struct _tmp72A;_tmp72A.tag=1;
_tmp72A.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp72B=_cycalloc(sizeof(*
_tmp72B));_tmp72B->f1=Cyc_Absyn_Loc_n;_tmp72B->f2=({struct _dyneither_ptr*_tmp72C=
_cycalloc(sizeof(*_tmp72C));_tmp72C[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp72C;});_tmp72B;}),(
void*)& Cyc_Absyn_VoidType_val,0);_tmp72A.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp72A;});_tmp729;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)));break;case
375: _LL397: {struct Cyc_List_List*_tmp72E;int _tmp72F;struct _tuple19 _tmp72D=*Cyc_yyget_YY12(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp72E=_tmp72D.f1;
_tmp72F=_tmp72D.f2;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Tuple_p_struct*
_tmp730=_cycalloc(sizeof(*_tmp730));_tmp730[0]=({struct Cyc_Absyn_Tuple_p_struct
_tmp731;_tmp731.tag=4;_tmp731.f1=_tmp72E;_tmp731.f2=_tmp72F;_tmp731;});_tmp730;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}case 376: _LL398: {struct Cyc_List_List*_tmp733;
int _tmp734;struct _tuple19 _tmp732=*Cyc_yyget_YY12(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp733=_tmp732.f1;
_tmp734=_tmp732.f2;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownCall_p_struct*
_tmp735=_cycalloc(sizeof(*_tmp735));_tmp735[0]=({struct Cyc_Absyn_UnknownCall_p_struct
_tmp736;_tmp736.tag=15;_tmp736.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp736.f2=_tmp733;
_tmp736.f3=_tmp734;_tmp736;});_tmp735;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 377:
_LL399: {struct Cyc_List_List*_tmp738;int _tmp739;struct _tuple19 _tmp737=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp738=_tmp737.f1;
_tmp739=_tmp737.f2;{struct Cyc_List_List*_tmp73A=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp73B=_cycalloc(sizeof(*_tmp73B));
_tmp73B[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp73C;_tmp73C.tag=6;_tmp73C.f1=({
struct Cyc_Absyn_AggrInfo*_tmp73D=_cycalloc(sizeof(*_tmp73D));_tmp73D->aggr_info=
Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),0);_tmp73D->targs=0;
_tmp73D;});_tmp73C.f2=_tmp73A;_tmp73C.f3=_tmp738;_tmp73C.f4=_tmp739;_tmp73C;});
_tmp73B;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}}case 378: _LL39A: {struct Cyc_List_List*
_tmp73F;int _tmp740;struct _tuple19 _tmp73E=*Cyc_yyget_YY16(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp73F=_tmp73E.f1;
_tmp740=_tmp73E.f2;{struct Cyc_List_List*_tmp741=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp742=_cycalloc(sizeof(*_tmp742));
_tmp742[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp743;_tmp743.tag=6;_tmp743.f1=0;
_tmp743.f2=_tmp741;_tmp743.f3=_tmp73F;_tmp743.f4=_tmp740;_tmp743;});_tmp742;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}}case 379: _LL39B: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp744=_cycalloc(sizeof(*_tmp744));
_tmp744[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp745;_tmp745.tag=5;_tmp745.f1=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp745;});
_tmp744;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 380: _LL39C: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp746=_cycalloc(sizeof(*_tmp746));
_tmp746[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp747;_tmp747.tag=5;_tmp747.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp748=_cycalloc(
sizeof(*_tmp748));_tmp748[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp749;_tmp749.tag=
5;_tmp749.f1=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp749;});_tmp748;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp747;});
_tmp746;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 381: _LL39D: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Reference_p_struct*_tmp74A=_cycalloc(sizeof(*_tmp74A));
_tmp74A[0]=({struct Cyc_Absyn_Reference_p_struct _tmp74B;_tmp74B.tag=2;_tmp74B.f1=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp74C=_cycalloc(sizeof(*_tmp74C));
_tmp74C->f1=Cyc_Absyn_Loc_n;_tmp74C->f2=({struct _dyneither_ptr*_tmp74D=_cycalloc(
sizeof(*_tmp74D));_tmp74D[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp74D;});_tmp74C;}),(void*)&
Cyc_Absyn_VoidType_val,0);_tmp74B.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp74B;});_tmp74A;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 382:
_LL39E: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp74E="as";
_tag_dyneither(_tmp74E,sizeof(char),3);}))!= 0)Cyc_Parse_err(({const char*_tmp74F="expecting `as'";
_tag_dyneither(_tmp74F,sizeof(char),15);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp750=_cycalloc(
sizeof(*_tmp750));_tmp750[0]=({struct Cyc_Absyn_Reference_p_struct _tmp751;_tmp751.tag=
2;_tmp751.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp752=_cycalloc(sizeof(*
_tmp752));_tmp752->f1=Cyc_Absyn_Loc_n;_tmp752->f2=({struct _dyneither_ptr*_tmp753=
_cycalloc(sizeof(*_tmp753));_tmp753[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp753;});_tmp752;}),(
void*)& Cyc_Absyn_VoidType_val,0);_tmp751.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp751;});_tmp750;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)));break;case
383: _LL39F: {void*_tmp754=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Tcutil_kind_to_bound(
Cyc_Absyn_IntKind));yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp755=_cycalloc(sizeof(*_tmp755));_tmp755[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp756;_tmp756.tag=3;_tmp756.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp754);
_tmp756.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp757=_cycalloc(sizeof(*
_tmp757));_tmp757->f1=Cyc_Absyn_Loc_n;_tmp757->f2=({struct _dyneither_ptr*_tmp758=
_cycalloc(sizeof(*_tmp758));_tmp758[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp758;});_tmp757;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp759=_cycalloc(sizeof(*_tmp759));
_tmp759[0]=({struct Cyc_Absyn_TagType_struct _tmp75A;_tmp75A.tag=20;_tmp75A.f1=(
void*)_tmp754;_tmp75A;});_tmp759;}),0);_tmp756;});_tmp755;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 384:
_LL3A0: {struct Cyc_Absyn_Tvar*_tmp75B=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(
Cyc_Absyn_IntKind));yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp75C=_cycalloc(sizeof(*_tmp75C));_tmp75C[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp75D;_tmp75D.tag=3;_tmp75D.f1=_tmp75B;_tmp75D.f2=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp75E=_cycalloc(sizeof(*_tmp75E));_tmp75E->f1=Cyc_Absyn_Loc_n;
_tmp75E->f2=({struct _dyneither_ptr*_tmp75F=_cycalloc(sizeof(*_tmp75F));_tmp75F[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp75F;});_tmp75E;}),(void*)({struct Cyc_Absyn_TagType_struct*_tmp760=_cycalloc(
sizeof(*_tmp760));_tmp760[0]=({struct Cyc_Absyn_TagType_struct _tmp761;_tmp761.tag=
20;_tmp761.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp762=_cycalloc(
sizeof(*_tmp762));_tmp762[0]=({struct Cyc_Absyn_VarType_struct _tmp763;_tmp763.tag=
2;_tmp763.f1=_tmp75B;_tmp763;});_tmp762;}));_tmp761;});_tmp760;}),0);_tmp75D;});
_tmp75C;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}case 385: _LL3A1: yyval=Cyc_YY12(({struct
_tuple19*_tmp764=_cycalloc(sizeof(*_tmp764));_tmp764->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp764->f2=0;_tmp764;}));
break;case 386: _LL3A2: yyval=Cyc_YY12(({struct _tuple19*_tmp765=_cycalloc(sizeof(*
_tmp765));_tmp765->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp765->f2=1;_tmp765;}));break;case 387: _LL3A3: yyval=Cyc_YY12(({struct _tuple19*
_tmp766=_cycalloc(sizeof(*_tmp766));_tmp766->f1=0;_tmp766->f2=1;_tmp766;}));
break;case 388: _LL3A4: yyval=Cyc_YY13(({struct Cyc_List_List*_tmp767=_cycalloc(
sizeof(*_tmp767));_tmp767->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp767->tl=0;_tmp767;}));break;case 389: _LL3A5: yyval=Cyc_YY13(({
struct Cyc_List_List*_tmp768=_cycalloc(sizeof(*_tmp768));_tmp768->hd=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp768->tl=Cyc_yyget_YY13(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp768;}));break;
case 390: _LL3A6: yyval=Cyc_YY14(({struct _tuple20*_tmp769=_cycalloc(sizeof(*_tmp769));
_tmp769->f1=0;_tmp769->f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp769;}));break;case 391: _LL3A7: yyval=Cyc_YY14(({struct
_tuple20*_tmp76A=_cycalloc(sizeof(*_tmp76A));_tmp76A->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp76A->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp76A;}));break;case
392: _LL3A8: yyval=Cyc_YY16(({struct _tuple19*_tmp76B=_cycalloc(sizeof(*_tmp76B));
_tmp76B->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp76B->f2=0;_tmp76B;}));
break;case 393: _LL3A9: yyval=Cyc_YY16(({struct _tuple19*_tmp76C=_cycalloc(sizeof(*
_tmp76C));_tmp76C->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp76C->f2=1;_tmp76C;}));break;case 394: _LL3AA: yyval=Cyc_YY16(({struct _tuple19*
_tmp76D=_cycalloc(sizeof(*_tmp76D));_tmp76D->f1=0;_tmp76D->f2=1;_tmp76D;}));
break;case 395: _LL3AB: yyval=Cyc_YY15(({struct Cyc_List_List*_tmp76E=_cycalloc(
sizeof(*_tmp76E));_tmp76E->hd=Cyc_yyget_YY14(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp76E->tl=0;_tmp76E;}));break;case 396: _LL3AC: yyval=Cyc_YY15(({
struct Cyc_List_List*_tmp76F=_cycalloc(sizeof(*_tmp76F));_tmp76F->hd=Cyc_yyget_YY14(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp76F->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp76F;}));break;
case 397: _LL3AD: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 398: _LL3AE: yyval=Cyc_YY3(Cyc_Absyn_seq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 399:
_LL3AF: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
400: _LL3B0: yyval=Cyc_YY3(Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 401:
_LL3B1: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Swap_e_struct*
_tmp770=_cycalloc(sizeof(*_tmp770));_tmp770[0]=({struct Cyc_Absyn_Swap_e_struct
_tmp771;_tmp771.tag=36;_tmp771.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp771.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp771;});_tmp770;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 402: _LL3B2: yyval=Cyc_YY8(0);break;case 403:
_LL3B3: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp772=_cycalloc_atomic(sizeof(*
_tmp772));_tmp772->v=(void*)Cyc_Absyn_Times;_tmp772;}));break;case 404: _LL3B4:
yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp773=_cycalloc_atomic(sizeof(*_tmp773));
_tmp773->v=(void*)Cyc_Absyn_Div;_tmp773;}));break;case 405: _LL3B5: yyval=Cyc_YY8(({
struct Cyc_Core_Opt*_tmp774=_cycalloc_atomic(sizeof(*_tmp774));_tmp774->v=(void*)
Cyc_Absyn_Mod;_tmp774;}));break;case 406: _LL3B6: yyval=Cyc_YY8(({struct Cyc_Core_Opt*
_tmp775=_cycalloc_atomic(sizeof(*_tmp775));_tmp775->v=(void*)Cyc_Absyn_Plus;
_tmp775;}));break;case 407: _LL3B7: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp776=
_cycalloc_atomic(sizeof(*_tmp776));_tmp776->v=(void*)Cyc_Absyn_Minus;_tmp776;}));
break;case 408: _LL3B8: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp777=_cycalloc_atomic(
sizeof(*_tmp777));_tmp777->v=(void*)Cyc_Absyn_Bitlshift;_tmp777;}));break;case
409: _LL3B9: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp778=_cycalloc_atomic(sizeof(*
_tmp778));_tmp778->v=(void*)Cyc_Absyn_Bitlrshift;_tmp778;}));break;case 410:
_LL3BA: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp779=_cycalloc_atomic(sizeof(*
_tmp779));_tmp779->v=(void*)Cyc_Absyn_Bitand;_tmp779;}));break;case 411: _LL3BB:
yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp77A=_cycalloc_atomic(sizeof(*_tmp77A));
_tmp77A->v=(void*)Cyc_Absyn_Bitxor;_tmp77A;}));break;case 412: _LL3BC: yyval=Cyc_YY8(({
struct Cyc_Core_Opt*_tmp77B=_cycalloc_atomic(sizeof(*_tmp77B));_tmp77B->v=(void*)
Cyc_Absyn_Bitor;_tmp77B;}));break;case 413: _LL3BD: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 414: _LL3BE: yyval=
Cyc_YY3(Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 415:
_LL3BF: yyval=Cyc_YY3(Cyc_Absyn_throw_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 416:
_LL3C0: yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 417:
_LL3C1: yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 418:
_LL3C2: yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 419:
_LL3C3: yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 420:
_LL3C4: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
421: _LL3C5: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 422: _LL3C6: yyval=Cyc_YY3(Cyc_Absyn_or_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 423:
_LL3C7: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
424: _LL3C8: yyval=Cyc_YY3(Cyc_Absyn_and_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 425:
_LL3C9: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
426: _LL3CA: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 427:
_LL3CB: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
428: _LL3CC: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 429:
_LL3CD: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
430: _LL3CE: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 431:
_LL3CF: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
432: _LL3D0: yyval=Cyc_YY3(Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 433:
_LL3D1: yyval=Cyc_YY3(Cyc_Absyn_neq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 434:
_LL3D2: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
435: _LL3D3: yyval=Cyc_YY3(Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 436:
_LL3D4: yyval=Cyc_YY3(Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 437:
_LL3D5: yyval=Cyc_YY3(Cyc_Absyn_lte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 438:
_LL3D6: yyval=Cyc_YY3(Cyc_Absyn_gte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 439:
_LL3D7: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
440: _LL3D8: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 441:
_LL3D9: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 442:
_LL3DA: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
443: _LL3DB: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 444:
_LL3DC: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 445:
_LL3DD: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
446: _LL3DE: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 447:
_LL3DF: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 448:
_LL3E0: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 449:
_LL3E1: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
450: _LL3E2: yyval=Cyc_YY3(Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),1,Cyc_Absyn_Unknown_coercion,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 451: _LL3E3: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 452: _LL3E4: yyval=
Cyc_YY3(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 453:
_LL3E5: yyval=Cyc_YY3(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 454:
_LL3E6: yyval=Cyc_YY3(Cyc_Absyn_address_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 455:
_LL3E7: yyval=Cyc_YY3(Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 456:
_LL3E8: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
457: _LL3E9: yyval=Cyc_YY3(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 458:
_LL3EA: yyval=Cyc_YY3(Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 459:
_LL3EB: yyval=Cyc_YY3(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 460:
_LL3EC: yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp77C=_cycalloc(sizeof(*_tmp77C));_tmp77C[0]=({struct Cyc_Absyn_StructField_struct
_tmp77D;_tmp77D.tag=0;_tmp77D.f1=({struct _dyneither_ptr*_tmp77E=_cycalloc(
sizeof(*_tmp77E));_tmp77E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp77E;});_tmp77D;});
_tmp77C;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 461: _LL3ED: yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp77F=_cycalloc_atomic(sizeof(*
_tmp77F));_tmp77F[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp780;_tmp780.tag=1;
_tmp780.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2;_tmp780;});_tmp77F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 462:
_LL3EE: {struct Cyc_Position_Segment*_tmp781=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp782=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp781,Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
yyval=Cyc_YY3(Cyc_Absyn_gentyp_exp(_tmp782,(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 463:
_LL3EF: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp783=_cycalloc(sizeof(*_tmp783));_tmp783[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp784;_tmp784.tag=35;_tmp784.f1=({struct Cyc_Absyn_MallocInfo _tmp785;_tmp785.is_calloc=
0;_tmp785.rgn=0;_tmp785.elt_type=0;_tmp785.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp785.fat_result=0;
_tmp785;});_tmp784;});_tmp783;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 464:
_LL3F0: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp786=_cycalloc(sizeof(*_tmp786));_tmp786[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp787;_tmp787.tag=35;_tmp787.f1=({struct Cyc_Absyn_MallocInfo _tmp788;_tmp788.is_calloc=
0;_tmp788.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp788.elt_type=0;
_tmp788.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp788.fat_result=0;_tmp788;});_tmp787;});_tmp786;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 465:
_LL3F1: {void*_tmp78A;struct _tuple8 _tmp789=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp78A=_tmp789.f3;yyval=
Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp78B=
_cycalloc(sizeof(*_tmp78B));_tmp78B[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp78C;
_tmp78C.tag=35;_tmp78C.f1=({struct Cyc_Absyn_MallocInfo _tmp78D;_tmp78D.is_calloc=
1;_tmp78D.rgn=0;_tmp78D.elt_type=({void**_tmp78E=_cycalloc(sizeof(*_tmp78E));
_tmp78E[0]=_tmp78A;_tmp78E;});_tmp78D.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp78D.fat_result=0;
_tmp78D;});_tmp78C;});_tmp78B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 466:
_LL3F2: {void*_tmp790;struct _tuple8 _tmp78F=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp790=_tmp78F.f3;yyval=
Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp791=
_cycalloc(sizeof(*_tmp791));_tmp791[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp792;
_tmp792.tag=35;_tmp792.f1=({struct Cyc_Absyn_MallocInfo _tmp793;_tmp793.is_calloc=
1;_tmp793.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);_tmp793.elt_type=({void**
_tmp794=_cycalloc(sizeof(*_tmp794));_tmp794[0]=_tmp790;_tmp794;});_tmp793.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp793.fat_result=0;_tmp793;});_tmp792;});_tmp791;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 467:
_LL3F3: yyval=Cyc_YY3(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,({struct Cyc_Absyn_Exp*
_tmp795[1];_tmp795[0]=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp795,sizeof(struct Cyc_Absyn_Exp*),1));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 468:
_LL3F4: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tagcheck_e_struct*
_tmp796=_cycalloc(sizeof(*_tmp796));_tmp796[0]=({struct Cyc_Absyn_Tagcheck_e_struct
_tmp797;_tmp797.tag=39;_tmp797.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp797.f2=({struct
_dyneither_ptr*_tmp798=_cycalloc(sizeof(*_tmp798));_tmp798[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp798;});_tmp797;});
_tmp796;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 469: _LL3F5: yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Tagcheck_e_struct*_tmp799=_cycalloc(sizeof(*_tmp799));
_tmp799[0]=({struct Cyc_Absyn_Tagcheck_e_struct _tmp79A;_tmp79A.tag=39;_tmp79A.f1=
Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));_tmp79A.f2=({
struct _dyneither_ptr*_tmp79B=_cycalloc(sizeof(*_tmp79B));_tmp79B[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp79B;});_tmp79A;});
_tmp799;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 470: _LL3F6: {void*_tmp79D;struct _tuple8
_tmp79C=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp79D=_tmp79C.f3;yyval=Cyc_YY3(Cyc_Absyn_valueof_exp(_tmp79D,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 471:
_LL3F7: yyval=Cyc_YY7(Cyc_Absyn_Bitnot);break;case 472: _LL3F8: yyval=Cyc_YY7(Cyc_Absyn_Not);
break;case 473: _LL3F9: yyval=Cyc_YY7(Cyc_Absyn_Minus);break;case 474: _LL3FA: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 475: _LL3FB:
yyval=Cyc_YY3(Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 476:
_LL3FC: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 477:
_LL3FD: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 478:
_LL3FE: yyval=Cyc_YY3(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _dyneither_ptr*
_tmp79E=_cycalloc(sizeof(*_tmp79E));_tmp79E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp79E;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 479:
_LL3FF: yyval=Cyc_YY3(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _dyneither_ptr*
_tmp79F=_cycalloc(sizeof(*_tmp79F));_tmp79F[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp79F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 480:
_LL400: yyval=Cyc_YY3(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 481:
_LL401: yyval=Cyc_YY3(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 482:
_LL402: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_struct*
_tmp7A0=_cycalloc(sizeof(*_tmp7A0));_tmp7A0[0]=({struct Cyc_Absyn_CompoundLit_e_struct
_tmp7A1;_tmp7A1.tag=27;_tmp7A1.f1=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp7A1.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp7A1;});_tmp7A0;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 483: _LL403: yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp7A2=_cycalloc(sizeof(*_tmp7A2));
_tmp7A2[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp7A3;_tmp7A3.tag=27;_tmp7A3.f1=
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
_tmp7A3.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp7A3;});_tmp7A2;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 484:
_LL404: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp7A4=_cycalloc(sizeof(*_tmp7A4));_tmp7A4[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp7A5;_tmp7A5.tag=2;_tmp7A5.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7A5;});_tmp7A4;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 485:
_LL405: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
486: _LL406: yyval=Cyc_YY3(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 487:
_LL407: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 488: _LL408: yyval=Cyc_YY3(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 489:
_LL409: yyval=Cyc_YY3(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 490:
_LL40A: yyval=Cyc_YY3(Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 491:
_LL40B: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_struct*
_tmp7A6=_cycalloc(sizeof(*_tmp7A6));_tmp7A6[0]=({struct Cyc_Absyn_Aggregate_e_struct
_tmp7A7;_tmp7A7.tag=30;_tmp7A7.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp7A7.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7A7.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp7A7.f4=0;_tmp7A7;});
_tmp7A6;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 492: _LL40C: yyval=Cyc_YY3(Cyc_Absyn_stmt_exp(
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 493:
_LL40D: yyval=Cyc_YY5(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));break;
case 494: _LL40E: yyval=Cyc_YY5(({struct Cyc_List_List*_tmp7A8=_cycalloc(sizeof(*
_tmp7A8));_tmp7A8->hd=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7A8->tl=0;_tmp7A8;}));break;case 495: _LL40F: yyval=Cyc_YY5(({
struct Cyc_List_List*_tmp7A9=_cycalloc(sizeof(*_tmp7A9));_tmp7A9->hd=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7A9->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7A9;}));break;
case 496: _LL410: yyval=Cyc_YY3(Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 497:
_LL411: yyval=Cyc_YY3(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 498:
_LL412: yyval=Cyc_YY3(Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 499:
_LL413: yyval=Cyc_YY3(Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 500:
_LL414: yyval=Cyc_QualId_tok(({struct _tuple1*_tmp7AA=_cycalloc(sizeof(*_tmp7AA));
_tmp7AA->f1=Cyc_Absyn_Rel_n(0);_tmp7AA->f2=({struct _dyneither_ptr*_tmp7AB=
_cycalloc(sizeof(*_tmp7AB));_tmp7AB[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7AB;});_tmp7AA;}));
break;case 501: _LL415: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 502: _LL416: yyval=Cyc_QualId_tok(({struct _tuple1*_tmp7AC=
_cycalloc(sizeof(*_tmp7AC));_tmp7AC->f1=Cyc_Absyn_Rel_n(0);_tmp7AC->f2=({struct
_dyneither_ptr*_tmp7AD=_cycalloc(sizeof(*_tmp7AD));_tmp7AD[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7AD;});_tmp7AC;}));
break;case 503: _LL417: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 504: _LL418: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 505: _LL419: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 506: _LL41A: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 507: _LL41B:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 508:
_LL41C: break;case 509: _LL41D: yylex_buf->lex_curr_pos -=1;break;default: _LL41E:
break;}yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=yyval;++ yylsp_offset;if(
yylen == 0){(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line=
Cyc_yylloc.first_line;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_column=
Cyc_yylloc.first_column;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_column;}else{(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_column;}
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(510,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(143,yyn - 144)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
6371) && Cyc_yycheck[_check_known_subscript_notnull(6372,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(6372,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(143,yyn - 144)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1032,yystate)];if(yyn > - 32768  && yyn < 6371){int
sze=0;struct _dyneither_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 287 / 
sizeof(char*);++ x){if(Cyc_yycheck[_check_known_subscript_notnull(6372,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(287,x)])+ 15,count ++);}msg=({unsigned int _tmp7AE=(
unsigned int)(sze + 15);char*_tmp7AF=(char*)_region_malloc(yyregion,_check_times(
sizeof(char),_tmp7AE + 1));struct _dyneither_ptr _tmp7B1=_tag_dyneither(_tmp7AF,
sizeof(char),_tmp7AE + 1);{unsigned int _tmp7B0=_tmp7AE;unsigned int i;for(i=0;i < 
_tmp7B0;i ++){_tmp7AF[i]='\000';}_tmp7AF[_tmp7B0]=(char)0;}_tmp7B1;});Cyc_strcpy(
msg,({const char*_tmp7B2="parse error";_tag_dyneither(_tmp7B2,sizeof(char),12);}));
if(count < 5){count=0;for(x=yyn < 0?- yyn: 0;x < 287 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(6372,x + yyn)]== x){Cyc_strcat(msg,(struct
_dyneither_ptr)(count == 0?(struct _dyneither_ptr)({const char*_tmp7B3=", expecting `";
_tag_dyneither(_tmp7B3,sizeof(char),14);}):(struct _dyneither_ptr)({const char*
_tmp7B4=" or `";_tag_dyneither(_tmp7B4,sizeof(char),6);})));Cyc_strcat(msg,(
struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(287,x)]);Cyc_strcat(
msg,({const char*_tmp7B5="'";_tag_dyneither(_tmp7B5,sizeof(char),2);}));++ count;}}}
Cyc_yyerror((struct _dyneither_ptr)msg);}else{Cyc_yyerror(({const char*_tmp7B6="parse error";
_tag_dyneither(_tmp7B6,sizeof(char),12);}));}}goto yyerrlab1;yyerrlab1: if(
yyerrstatus == 3){if(Cyc_yychar == 0){int _tmp7B7=1;_npop_handler(0);return _tmp7B7;}
Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(
yyssp_offset == 0){int _tmp7B8=1;_npop_handler(0);return _tmp7B8;}-- yyvsp_offset;
yystate=(int)yyss[_check_known_subscript_notnull(10000,-- yyssp_offset)];--
yylsp_offset;yyerrhandle: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1032,
yystate)];if(yyn == - 32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6371) || 
Cyc_yycheck[_check_known_subscript_notnull(6372,yyn)]!= 1)goto yyerrdefault;yyn=(
int)Cyc_yytable[_check_known_subscript_notnull(6372,yyn)];if(yyn < 0){if(yyn == -
32768)goto yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 
1031){int _tmp7B9=0;_npop_handler(0);return _tmp7B9;}yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto yynewstate;};
_pop_region(yyregion);}void Cyc_yyprint(int i,union Cyc_YYSTYPE v){union Cyc_YYSTYPE
_tmp7C3=v;struct _tuple6 _tmp7C4;int _tmp7C5;char _tmp7C6;struct _dyneither_ptr
_tmp7C7;struct _tuple1*_tmp7C8;struct _tuple1 _tmp7C9;union Cyc_Absyn_Nmspace _tmp7CA;
struct _dyneither_ptr*_tmp7CB;_LL421: if((_tmp7C3.Int_tok).tag != 1)goto _LL423;
_tmp7C4=(struct _tuple6)(_tmp7C3.Int_tok).val;_tmp7C5=_tmp7C4.f2;_LL422:({struct
Cyc_Int_pa_struct _tmp7CE;_tmp7CE.tag=1;_tmp7CE.f1=(unsigned long)_tmp7C5;{void*
_tmp7CC[1]={& _tmp7CE};Cyc_fprintf(Cyc_stderr,({const char*_tmp7CD="%d";
_tag_dyneither(_tmp7CD,sizeof(char),3);}),_tag_dyneither(_tmp7CC,sizeof(void*),1));}});
goto _LL420;_LL423: if((_tmp7C3.Char_tok).tag != 2)goto _LL425;_tmp7C6=(char)(
_tmp7C3.Char_tok).val;_LL424:({struct Cyc_Int_pa_struct _tmp7D1;_tmp7D1.tag=1;
_tmp7D1.f1=(unsigned long)((int)_tmp7C6);{void*_tmp7CF[1]={& _tmp7D1};Cyc_fprintf(
Cyc_stderr,({const char*_tmp7D0="%c";_tag_dyneither(_tmp7D0,sizeof(char),3);}),
_tag_dyneither(_tmp7CF,sizeof(void*),1));}});goto _LL420;_LL425: if((_tmp7C3.String_tok).tag
!= 3)goto _LL427;_tmp7C7=(struct _dyneither_ptr)(_tmp7C3.String_tok).val;_LL426:({
struct Cyc_String_pa_struct _tmp7D4;_tmp7D4.tag=0;_tmp7D4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp7C7);{void*_tmp7D2[1]={& _tmp7D4};Cyc_fprintf(Cyc_stderr,({
const char*_tmp7D3="\"%s\"";_tag_dyneither(_tmp7D3,sizeof(char),5);}),
_tag_dyneither(_tmp7D2,sizeof(void*),1));}});goto _LL420;_LL427: if((_tmp7C3.QualId_tok).tag
!= 5)goto _LL429;_tmp7C8=(struct _tuple1*)(_tmp7C3.QualId_tok).val;_tmp7C9=*
_tmp7C8;_tmp7CA=_tmp7C9.f1;_tmp7CB=_tmp7C9.f2;_LL428: {struct Cyc_List_List*
_tmp7D5=0;{union Cyc_Absyn_Nmspace _tmp7D6=_tmp7CA;struct Cyc_List_List*_tmp7D7;
struct Cyc_List_List*_tmp7D8;int _tmp7D9;_LL42C: if((_tmp7D6.Rel_n).tag != 1)goto
_LL42E;_tmp7D7=(struct Cyc_List_List*)(_tmp7D6.Rel_n).val;_LL42D: _tmp7D5=_tmp7D7;
goto _LL42B;_LL42E: if((_tmp7D6.Abs_n).tag != 2)goto _LL430;_tmp7D8=(struct Cyc_List_List*)(
_tmp7D6.Abs_n).val;_LL42F: _tmp7D5=_tmp7D8;goto _LL42B;_LL430: if((_tmp7D6.Loc_n).tag
!= 3)goto _LL42B;_tmp7D9=(int)(_tmp7D6.Loc_n).val;_LL431: goto _LL42B;_LL42B:;}for(
0;_tmp7D5 != 0;_tmp7D5=_tmp7D5->tl){({struct Cyc_String_pa_struct _tmp7DC;_tmp7DC.tag=
0;_tmp7DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)_tmp7D5->hd));{void*_tmp7DA[1]={& _tmp7DC};Cyc_fprintf(Cyc_stderr,({
const char*_tmp7DB="%s::";_tag_dyneither(_tmp7DB,sizeof(char),5);}),
_tag_dyneither(_tmp7DA,sizeof(void*),1));}});}({struct Cyc_String_pa_struct
_tmp7DF;_tmp7DF.tag=0;_tmp7DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp7CB);{void*_tmp7DD[1]={& _tmp7DF};Cyc_fprintf(Cyc_stderr,({const char*_tmp7DE="%s::";
_tag_dyneither(_tmp7DE,sizeof(char),5);}),_tag_dyneither(_tmp7DD,sizeof(void*),1));}});
goto _LL420;}_LL429:;_LL42A:({void*_tmp7E0=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp7E1="?";_tag_dyneither(_tmp7E1,sizeof(char),2);}),_tag_dyneither(_tmp7E0,
sizeof(void*),0));});goto _LL420;_LL420:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(
f));return Cyc_Parse_parse_result;}
