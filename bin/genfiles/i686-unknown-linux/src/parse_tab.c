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
struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[16];
extern char Cyc_Core_Free_Region[16];typedef struct{int __count;union{unsigned int
__wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{long __pos;Cyc___mbstate_t
__state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;extern
struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
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
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};extern char
Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{char*tag;struct _dyneither_ptr
f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*
refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int
lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int
lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct
_dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(
struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void*Cyc_List_fold_right(
void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x);extern char Cyc_List_Nth[
8];struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*
r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x);unsigned int Cyc_strlen(struct
_dyneither_ptr s);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_zstrcmp(struct _dyneither_ptr,struct _dyneither_ptr);int Cyc_zstrptrcmp(
struct _dyneither_ptr*,struct _dyneither_ptr*);struct _dyneither_ptr Cyc_strcat(
struct _dyneither_ptr dest,struct _dyneither_ptr src);struct _dyneither_ptr Cyc_strcpy(
struct _dyneither_ptr dest,struct _dyneither_ptr src);struct _dyneither_ptr Cyc_substring(
struct _dyneither_ptr,int ofs,unsigned int n);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*
Cyc_Position_segment_of_abs(int,int);struct _dyneither_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dyneither_ptr source;
struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};struct Cyc_Position_Error*
Cyc_Position_mk_err_parse(struct Cyc_Position_Segment*,struct _dyneither_ptr);
extern char Cyc_Position_Nocontext[14];void Cyc_Position_post_error(struct Cyc_Position_Error*);
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_is_qvar_qualified(
struct _tuple1*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual
x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);void*Cyc_Absyn_int_typ(void*,void*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(int);void*Cyc_Absyn_starb_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zero_term);
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(
void*,struct Cyc_List_List*es,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
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
Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct
Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(void*s,struct _tuple1*n,struct Cyc_List_List*
ts,struct Cyc_Core_Opt*fs,int is_extensible,int is_flat,struct Cyc_Position_Segment*
loc);void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*
eff_typ,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(
void*a);extern int Cyc_Absyn_porting_c_code;struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct _tuple4{void*f1;void*f2;};struct _tuple4*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple4*Cyc_Dict_rchoose(
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
le;int allow_valueof;};void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_copy_type(void*t);void*
Cyc_Tcutil_compress(void*t);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct
Cyc_Core_Opt Cyc_Tcutil_trk;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt
Cyc_Tcutil_ik;struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(void*k);void*Cyc_Tcutil_kind_to_bound(
void*k);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Lex_register_typedef(
struct _tuple1*s);void Cyc_Lex_enter_namespace(struct _dyneither_ptr*);void Cyc_Lex_leave_namespace();
void Cyc_Lex_enter_using(struct _tuple1*);void Cyc_Lex_leave_using();int Cyc_Parse_no_register=
0;struct Cyc_Parse_Signed_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Unsigned_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Short_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct
Cyc_Parse_Long_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Type_spec_struct{
int tag;void*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Parse_Decl_spec_struct{
int tag;struct Cyc_Absyn_Decl*f1;};struct Cyc_Parse_Declaration_spec{struct Cyc_Core_Opt*
sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int is_inline;struct Cyc_List_List*
attributes;};struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;
};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _tuple5{
void*f1;struct Cyc_Core_Opt*f2;};static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);struct _tuple6{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple6
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
static struct Cyc_List_List*Cyc_Parse_parse_result=0;static void Cyc_Parse_err(
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){Cyc_Position_post_error(
Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Parse_err((struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);(int)_throw((void*)
Cyc_Position_Exit);}static void Cyc_Parse_unimp(struct _dyneither_ptr msg,struct Cyc_Position_Segment*
sg){({struct Cyc_String_pa_struct _tmp3;_tmp3.tag=0;_tmp3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg);{struct Cyc_String_pa_struct _tmp2;_tmp2.tag=0;_tmp2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(sg));{
void*_tmp0[2]={& _tmp2,& _tmp3};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="%s: Warning: Cyclone does not yet support %s\n";
_tag_dyneither(_tmp1,sizeof(char),46);}),_tag_dyneither(_tmp0,sizeof(void*),2));}}});
return;}static int Cyc_Parse_enum_counter=0;struct _tuple1*Cyc_Parse_gensym_enum(){
return({struct _tuple1*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp9;(_tmp9.Rel_n).tag=1;(_tmp9.Rel_n).f1=0;_tmp9;});
_tmp4->f2=({struct _dyneither_ptr*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=(
struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp8;_tmp8.tag=1;_tmp8.f1=(
unsigned long)Cyc_Parse_enum_counter ++;{void*_tmp6[1]={& _tmp8};Cyc_aprintf(({
const char*_tmp7="__anonymous_enum_%d__";_tag_dyneither(_tmp7,sizeof(char),22);}),
_tag_dyneither(_tmp6,sizeof(void*),1));}});_tmp5;});_tmp4;});}struct _tuple7{
struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct
Cyc_List_List*f5;};struct _tuple8{struct _tuple7*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*
loc,struct _tuple8*field_info){struct _tuple8 _tmpB;struct _tuple7*_tmpC;struct
_tuple7 _tmpD;struct _tuple1*_tmpE;struct Cyc_Absyn_Tqual _tmpF;void*_tmp10;struct
Cyc_List_List*_tmp11;struct Cyc_List_List*_tmp12;struct Cyc_Absyn_Exp*_tmp13;
struct _tuple8*_tmpA=field_info;_tmpB=*_tmpA;_tmpC=_tmpB.f1;_tmpD=*_tmpC;_tmpE=
_tmpD.f1;_tmpF=_tmpD.f2;_tmp10=_tmpD.f3;_tmp11=_tmpD.f4;_tmp12=_tmpD.f5;_tmp13=
_tmpB.f2;if(_tmp11 != 0)Cyc_Parse_err(({const char*_tmp14="bad type params in struct field";
_tag_dyneither(_tmp14,sizeof(char),32);}),loc);if(Cyc_Absyn_is_qvar_qualified(
_tmpE))Cyc_Parse_err(({const char*_tmp15="struct or union field cannot be qualified with a namespace";
_tag_dyneither(_tmp15,sizeof(char),59);}),loc);return({struct Cyc_Absyn_Aggrfield*
_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->name=(*_tmpE).f2;_tmp16->tq=_tmpF;
_tmp16->type=(void*)_tmp10;_tmp16->width=_tmp13;_tmp16->attributes=_tmp12;_tmp16;});}
static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*loc){return(
void*)({struct Cyc_Parse_Type_spec_struct*_tmp17=_cycalloc(sizeof(*_tmp17));
_tmp17[0]=({struct Cyc_Parse_Type_spec_struct _tmp18;_tmp18.tag=4;_tmp18.f1=(void*)
t;_tmp18.f2=loc;_tmp18;});_tmp17;});}static void*Cyc_Parse_array2ptr(void*t,int
argposn){void*_tmp19=t;struct Cyc_Absyn_ArrayInfo _tmp1A;void*_tmp1B;struct Cyc_Absyn_Tqual
_tmp1C;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Conref*_tmp1E;struct Cyc_Position_Segment*
_tmp1F;_LL1: if(_tmp19 <= (void*)4)goto _LL3;if(*((int*)_tmp19)!= 7)goto _LL3;_tmp1A=((
struct Cyc_Absyn_ArrayType_struct*)_tmp19)->f1;_tmp1B=(void*)_tmp1A.elt_type;
_tmp1C=_tmp1A.tq;_tmp1D=_tmp1A.num_elts;_tmp1E=_tmp1A.zero_term;_tmp1F=_tmp1A.zt_loc;
_LL2: return Cyc_Absyn_starb_typ(_tmp1B,argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0):(void*)2,_tmp1C,_tmp1D == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp21;_tmp21.tag=0;_tmp21.f1=(struct Cyc_Absyn_Exp*)_tmp1D;_tmp21;});_tmp20;}),
_tmp1E);_LL3:;_LL4: return t;_LL0:;}static void Cyc_Parse_arg_array2ptr(struct
_tuple2*x){(*x).f3=Cyc_Parse_array2ptr((*x).f3,1);}struct _tuple9{struct Cyc_Absyn_Tqual
f1;void*f2;};static struct _tuple9*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*
loc,struct _tuple2*t){return({struct _tuple9*_tmp22=_cycalloc(sizeof(*_tmp22));
_tmp22->f1=(*t).f2;_tmp22->f2=(*t).f3;_tmp22;});}static struct Cyc_Absyn_Vardecl*
Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){void*_tmp23=(void*)d->r;struct Cyc_Absyn_Vardecl*
_tmp24;_LL6: if(_tmp23 <= (void*)2)goto _LL8;if(*((int*)_tmp23)!= 0)goto _LL8;_tmp24=((
struct Cyc_Absyn_Var_d_struct*)_tmp23)->f1;_LL7: return _tmp24;_LL8:;_LL9:({void*
_tmp25=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(d->loc,({const char*_tmp26="bad declaration in `forarray' statement";
_tag_dyneither(_tmp26,sizeof(char),40);}),_tag_dyneither(_tmp25,sizeof(void*),0));});
_LL5:;}static int Cyc_Parse_is_typeparam(void*tm){void*_tmp27=tm;_LLB: if(*((int*)
_tmp27)!= 4)goto _LLD;_LLC: return 1;_LLD:;_LLE: return 0;_LLA:;}static void*Cyc_Parse_id2type(
struct _dyneither_ptr s,void*k){if(Cyc_zstrcmp((struct _dyneither_ptr)s,({const char*
_tmp28="`H";_tag_dyneither(_tmp28,sizeof(char),3);}))== 0)return(void*)2;else{
if(Cyc_zstrcmp((struct _dyneither_ptr)s,({const char*_tmp29="`U";_tag_dyneither(
_tmp29,sizeof(char),3);}))== 0)return(void*)3;else{return(void*)({struct Cyc_Absyn_VarType_struct*
_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A[0]=({struct Cyc_Absyn_VarType_struct
_tmp2B;_tmp2B.tag=1;_tmp2B.f1=({struct Cyc_Absyn_Tvar*_tmp2C=_cycalloc(sizeof(*
_tmp2C));_tmp2C->name=({struct _dyneither_ptr*_tmp2D=_cycalloc(sizeof(*_tmp2D));
_tmp2D[0]=s;_tmp2D;});_tmp2C->identity=- 1;_tmp2C->kind=(void*)k;_tmp2C;});_tmp2B;});
_tmp2A;});}}}static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*
t){void*k;{void*_tmp2E=Cyc_Absyn_compress_kb((void*)t->kind);void*_tmp2F;_LL10:
if(*((int*)_tmp2E)!= 1)goto _LL12;_LL11: k=(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp31;_tmp31.tag=1;_tmp31.f1=0;_tmp31;});_tmp30;});goto _LLF;_LL12: if(*((int*)
_tmp2E)!= 2)goto _LL14;_tmp2F=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp2E)->f2;
_LL13: k=(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp32=_cycalloc(sizeof(*_tmp32));
_tmp32[0]=({struct Cyc_Absyn_Less_kb_struct _tmp33;_tmp33.tag=2;_tmp33.f1=0;_tmp33.f2=(
void*)_tmp2F;_tmp33;});_tmp32;});goto _LLF;_LL14:;_LL15: k=_tmp2E;goto _LLF;_LLF:;}
return({struct Cyc_Absyn_Tvar*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->name=t->name;
_tmp34->identity=- 1;_tmp34->kind=(void*)k;_tmp34;});}static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*loc,void*t){void*_tmp35=t;struct
Cyc_Absyn_Tvar*_tmp36;_LL17: if(_tmp35 <= (void*)4)goto _LL19;if(*((int*)_tmp35)!= 
1)goto _LL19;_tmp36=((struct Cyc_Absyn_VarType_struct*)_tmp35)->f1;_LL18: return
_tmp36;_LL19:;_LL1A:({void*_tmp37=0;((int(*)(struct Cyc_Position_Segment*sg,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*
_tmp38="expecting a list of type variables, not types";_tag_dyneither(_tmp38,
sizeof(char),46);}),_tag_dyneither(_tmp37,sizeof(void*),0));});_LL16:;}static
void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){return(void*)({struct Cyc_Absyn_VarType_struct*
_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39[0]=({struct Cyc_Absyn_VarType_struct
_tmp3A;_tmp3A.tag=1;_tmp3A.f1=pr;_tmp3A;});_tmp39;});}static void Cyc_Parse_set_vartyp_kind(
void*t,void*k,int leq){void*_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp3C;struct Cyc_Absyn_Tvar _tmp3D;void*_tmp3E;void**_tmp3F;_LL1C: if(_tmp3B <= (
void*)4)goto _LL1E;if(*((int*)_tmp3B)!= 1)goto _LL1E;_tmp3C=((struct Cyc_Absyn_VarType_struct*)
_tmp3B)->f1;_tmp3D=*_tmp3C;_tmp3E=(void*)_tmp3D.kind;_tmp3F=(void**)&(*((struct
Cyc_Absyn_VarType_struct*)_tmp3B)->f1).kind;_LL1D: {void*_tmp40=Cyc_Absyn_compress_kb(*
_tmp3F);_LL21: if(*((int*)_tmp40)!= 1)goto _LL23;_LL22: if(!leq)*_tmp3F=Cyc_Tcutil_kind_to_bound(
k);else{*_tmp3F=(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp41=_cycalloc(sizeof(*
_tmp41));_tmp41[0]=({struct Cyc_Absyn_Less_kb_struct _tmp42;_tmp42.tag=2;_tmp42.f1=
0;_tmp42.f2=(void*)k;_tmp42;});_tmp41;});}return;_LL23:;_LL24: return;_LL20:;}
_LL1E:;_LL1F: return;_LL1B:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp43=(void*)tms->hd;void*_tmp44;
_LL26: if(*((int*)_tmp43)!= 3)goto _LL28;_tmp44=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp43)->f1;_LL27: if(tms->tl == 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 
0){void*_tmp45=_tmp44;struct Cyc_List_List*_tmp46;_LL2B: if(*((int*)_tmp45)!= 1)
goto _LL2D;_LL2C:({void*_tmp47=0;Cyc_Tcutil_warn(loc,({const char*_tmp48="function declaration with both new- and old-style parameter declarations; ignoring old-style";
_tag_dyneither(_tmp48,sizeof(char),93);}),_tag_dyneither(_tmp47,sizeof(void*),0));});
return tms;_LL2D: if(*((int*)_tmp45)!= 0)goto _LL2A;_tmp46=((struct Cyc_Absyn_NoTypes_struct*)
_tmp45)->f1;_LL2E: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp46)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds))({void*_tmp49=0;((int(*)(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Parse_abort)(loc,({const char*_tmp4A="wrong number of parameter declarations in old-style function declaration";
_tag_dyneither(_tmp4A,sizeof(char),73);}),_tag_dyneither(_tmp49,sizeof(void*),0));});{
struct Cyc_List_List*_tmp4B=0;for(0;_tmp46 != 0;_tmp46=_tmp46->tl){struct Cyc_List_List*
_tmp4C=tds;for(0;_tmp4C != 0;_tmp4C=_tmp4C->tl){struct Cyc_Absyn_Decl*_tmp4D=(
struct Cyc_Absyn_Decl*)_tmp4C->hd;void*_tmp4E=(void*)_tmp4D->r;struct Cyc_Absyn_Vardecl*
_tmp4F;_LL30: if(_tmp4E <= (void*)2)goto _LL32;if(*((int*)_tmp4E)!= 0)goto _LL32;
_tmp4F=((struct Cyc_Absyn_Var_d_struct*)_tmp4E)->f1;_LL31: if(Cyc_zstrptrcmp((*
_tmp4F->name).f2,(struct _dyneither_ptr*)_tmp46->hd)!= 0)continue;if(_tmp4F->initializer
!= 0)({void*_tmp50=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,({const char*_tmp51="initializer found in parameter declaration";
_tag_dyneither(_tmp51,sizeof(char),43);}),_tag_dyneither(_tmp50,sizeof(void*),0));});
if(Cyc_Absyn_is_qvar_qualified(_tmp4F->name))({void*_tmp52=0;((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,({
const char*_tmp53="namespaces forbidden in parameter declarations";_tag_dyneither(
_tmp53,sizeof(char),47);}),_tag_dyneither(_tmp52,sizeof(void*),0));});_tmp4B=({
struct Cyc_List_List*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->hd=({struct _tuple2*
_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->f1=({struct Cyc_Core_Opt*_tmp56=
_cycalloc(sizeof(*_tmp56));_tmp56->v=(*_tmp4F->name).f2;_tmp56;});_tmp55->f2=
_tmp4F->tq;_tmp55->f3=(void*)_tmp4F->type;_tmp55;});_tmp54->tl=_tmp4B;_tmp54;});
goto L;_LL32:;_LL33:({void*_tmp57=0;((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,({const
char*_tmp58="nonvariable declaration in parameter type";_tag_dyneither(_tmp58,
sizeof(char),42);}),_tag_dyneither(_tmp57,sizeof(void*),0));});_LL2F:;}L: if(
_tmp4C == 0)({struct Cyc_String_pa_struct _tmp5B;_tmp5B.tag=0;_tmp5B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp46->hd));{
void*_tmp59[1]={& _tmp5B};((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*
_tmp5A="%s is not given a type";_tag_dyneither(_tmp5A,sizeof(char),23);}),
_tag_dyneither(_tmp59,sizeof(void*),1));}});}return({struct Cyc_List_List*_tmp5C=
_cycalloc(sizeof(*_tmp5C));_tmp5C->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp5E;_tmp5E.tag=3;_tmp5E.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp60;_tmp60.tag=1;_tmp60.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp4B);_tmp60.f2=0;_tmp60.f3=0;_tmp60.f4=0;_tmp60.f5=0;_tmp60;});
_tmp5F;}));_tmp5E;});_tmp5D;}));_tmp5C->tl=0;_tmp5C;});}_LL2A:;}goto _LL29;_LL28:;
_LL29: return({struct Cyc_List_List*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->hd=(
void*)((void*)tms->hd);_tmp61->tl=Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);
_tmp61;});_LL25:;}}struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct Cyc_Core_Opt*
dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*
body,struct Cyc_Position_Segment*loc){if(tds != 0)d=({struct Cyc_Parse_Declarator*
_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->id=d->id;_tmp62->tms=Cyc_Parse_oldstyle2newstyle(
d->tms,tds,loc);_tmp62;});{void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual
tq=Cyc_Absyn_empty_tqual(0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){
tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)
dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;
atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc != 0){void*_tmp63=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v;_LL35: if((int)_tmp63 != 1)goto _LL37;
_LL36: sc=(void*)3;goto _LL34;_LL37: if((int)_tmp63 != 3)goto _LL39;_LL38: sc=(void*)0;
goto _LL34;_LL39:;_LL3A: Cyc_Parse_err(({const char*_tmp64="bad storage class on function";
_tag_dyneither(_tmp64,sizeof(char),30);}),loc);goto _LL34;_LL34:;}}{void*_tmp66;
struct Cyc_Core_Opt*_tmp67;struct _tuple5 _tmp65=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp66=_tmp65.f1;_tmp67=_tmp65.f2;{struct Cyc_Absyn_Tqual _tmp69;void*
_tmp6A;struct Cyc_List_List*_tmp6B;struct Cyc_List_List*_tmp6C;struct _tuple6 _tmp68=
Cyc_Parse_apply_tms(tq,_tmp66,atts,d->tms);_tmp69=_tmp68.f1;_tmp6A=_tmp68.f2;
_tmp6B=_tmp68.f3;_tmp6C=_tmp68.f4;if(_tmp67 != 0)({void*_tmp6D=0;Cyc_Tcutil_warn(
loc,({const char*_tmp6E="nested type declaration within function prototype";
_tag_dyneither(_tmp6E,sizeof(char),50);}),_tag_dyneither(_tmp6D,sizeof(void*),0));});
if(_tmp6B != 0)({void*_tmp6F=0;Cyc_Tcutil_warn(loc,({const char*_tmp70="bad type params, ignoring";
_tag_dyneither(_tmp70,sizeof(char),26);}),_tag_dyneither(_tmp6F,sizeof(void*),0));});{
void*_tmp71=_tmp6A;struct Cyc_Absyn_FnInfo _tmp72;struct Cyc_List_List*_tmp73;
struct Cyc_Core_Opt*_tmp74;void*_tmp75;struct Cyc_List_List*_tmp76;int _tmp77;
struct Cyc_Absyn_VarargInfo*_tmp78;struct Cyc_List_List*_tmp79;struct Cyc_List_List*
_tmp7A;_LL3C: if(_tmp71 <= (void*)4)goto _LL3E;if(*((int*)_tmp71)!= 8)goto _LL3E;
_tmp72=((struct Cyc_Absyn_FnType_struct*)_tmp71)->f1;_tmp73=_tmp72.tvars;_tmp74=
_tmp72.effect;_tmp75=(void*)_tmp72.ret_typ;_tmp76=_tmp72.args;_tmp77=_tmp72.c_varargs;
_tmp78=_tmp72.cyc_varargs;_tmp79=_tmp72.rgn_po;_tmp7A=_tmp72.attributes;_LL3D: {
struct Cyc_List_List*_tmp7B=0;{struct Cyc_List_List*_tmp7C=_tmp76;for(0;_tmp7C != 0;
_tmp7C=_tmp7C->tl){struct _tuple2 _tmp7E;struct Cyc_Core_Opt*_tmp7F;struct Cyc_Absyn_Tqual
_tmp80;void*_tmp81;struct _tuple2*_tmp7D=(struct _tuple2*)_tmp7C->hd;_tmp7E=*
_tmp7D;_tmp7F=_tmp7E.f1;_tmp80=_tmp7E.f2;_tmp81=_tmp7E.f3;if(_tmp7F == 0){Cyc_Parse_err(({
const char*_tmp82="missing argument variable in function prototype";
_tag_dyneither(_tmp82,sizeof(char),48);}),loc);_tmp7B=({struct Cyc_List_List*
_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->hd=({struct _tuple10*_tmp84=_cycalloc(
sizeof(*_tmp84));_tmp84->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),"?",sizeof(char),2);_tmp84->f2=_tmp80;_tmp84->f3=_tmp81;_tmp84;});
_tmp83->tl=_tmp7B;_tmp83;});}else{_tmp7B=({struct Cyc_List_List*_tmp86=_cycalloc(
sizeof(*_tmp86));_tmp86->hd=({struct _tuple10*_tmp87=_cycalloc(sizeof(*_tmp87));
_tmp87->f1=(struct _dyneither_ptr*)_tmp7F->v;_tmp87->f2=_tmp80;_tmp87->f3=_tmp81;
_tmp87;});_tmp86->tl=_tmp7B;_tmp86;});}}}return({struct Cyc_Absyn_Fndecl*_tmp88=
_cycalloc(sizeof(*_tmp88));_tmp88->sc=(void*)sc;_tmp88->name=d->id;_tmp88->tvs=
_tmp73;_tmp88->is_inline=is_inline;_tmp88->effect=_tmp74;_tmp88->ret_type=(void*)
_tmp75;_tmp88->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp7B);_tmp88->c_varargs=_tmp77;_tmp88->cyc_varargs=_tmp78;_tmp88->rgn_po=
_tmp79;_tmp88->body=body;_tmp88->cached_typ=0;_tmp88->param_vardecls=0;_tmp88->fn_vardecl=
0;_tmp88->attributes=Cyc_List_append(_tmp7A,_tmp6C);_tmp88;});}_LL3E:;_LL3F:({
void*_tmp89=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*_tmp8A="declarator is not a function prototype";
_tag_dyneither(_tmp8A,sizeof(char),39);}),_tag_dyneither(_tmp89,sizeof(void*),0));});
_LL3B:;}}}}}static char _tmp8B[52]="at most one type may appear within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg1={_tmp8B,_tmp8B,_tmp8B + 52};static char
_tmp8C[63]="const or volatile may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg2={_tmp8C,_tmp8C,_tmp8C + 63};static char
_tmp8D[50]="type specifier includes more than one declaration";static struct
_dyneither_ptr Cyc_Parse_msg3={_tmp8D,_tmp8D,_tmp8D + 50};static char _tmp8E[60]="sign specifier may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp8E,_tmp8E,_tmp8E + 60};static struct
_tuple5 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){struct Cyc_Core_Opt*declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;
void*t=(void*)0;void*sz=(void*)2;void*sgn=(void*)0;struct Cyc_Position_Segment*
last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp8F=(void*)ts->hd;void*_tmp90;
struct Cyc_Position_Segment*_tmp91;struct Cyc_Position_Segment*_tmp92;struct Cyc_Position_Segment*
_tmp93;struct Cyc_Position_Segment*_tmp94;struct Cyc_Position_Segment*_tmp95;
struct Cyc_Absyn_Decl*_tmp96;_LL41: if(*((int*)_tmp8F)!= 4)goto _LL43;_tmp90=(void*)((
struct Cyc_Parse_Type_spec_struct*)_tmp8F)->f1;_tmp91=((struct Cyc_Parse_Type_spec_struct*)
_tmp8F)->f2;_LL42: if(seen_type)Cyc_Parse_err(Cyc_Parse_msg1,_tmp91);last_loc=
_tmp91;seen_type=1;t=_tmp90;goto _LL40;_LL43: if(*((int*)_tmp8F)!= 0)goto _LL45;
_tmp92=((struct Cyc_Parse_Signed_spec_struct*)_tmp8F)->f1;_LL44: if(seen_sign)Cyc_Parse_err(
Cyc_Parse_msg4,_tmp92);if(seen_type)Cyc_Parse_err(({const char*_tmp97="signed qualifier must come before type";
_tag_dyneither(_tmp97,sizeof(char),39);}),_tmp92);last_loc=_tmp92;seen_sign=1;
sgn=(void*)0;goto _LL40;_LL45: if(*((int*)_tmp8F)!= 1)goto _LL47;_tmp93=((struct Cyc_Parse_Unsigned_spec_struct*)
_tmp8F)->f1;_LL46: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp93);if(seen_type)
Cyc_Parse_err(({const char*_tmp98="signed qualifier must come before type";
_tag_dyneither(_tmp98,sizeof(char),39);}),_tmp93);last_loc=_tmp93;seen_sign=1;
sgn=(void*)1;goto _LL40;_LL47: if(*((int*)_tmp8F)!= 2)goto _LL49;_tmp94=((struct Cyc_Parse_Short_spec_struct*)
_tmp8F)->f1;_LL48: if(seen_size)Cyc_Parse_err(({const char*_tmp99="integral size may appear only once within a type specifier";
_tag_dyneither(_tmp99,sizeof(char),59);}),_tmp94);if(seen_type)Cyc_Parse_err(({
const char*_tmp9A="short modifier must come before base type";_tag_dyneither(
_tmp9A,sizeof(char),42);}),_tmp94);last_loc=_tmp94;seen_size=1;sz=(void*)1;goto
_LL40;_LL49: if(*((int*)_tmp8F)!= 3)goto _LL4B;_tmp95=((struct Cyc_Parse_Long_spec_struct*)
_tmp8F)->f1;_LL4A: if(seen_type)Cyc_Parse_err(({const char*_tmp9B="long modifier must come before base type";
_tag_dyneither(_tmp9B,sizeof(char),41);}),_tmp95);if(seen_size){void*_tmp9C=sz;
_LL4E: if((int)_tmp9C != 3)goto _LL50;_LL4F: sz=(void*)4;goto _LL4D;_LL50:;_LL51: Cyc_Parse_err(({
const char*_tmp9D="extra long in type specifier";_tag_dyneither(_tmp9D,sizeof(
char),29);}),_tmp95);goto _LL4D;_LL4D:;}else{sz=(void*)3;}last_loc=_tmp95;
seen_size=1;goto _LL40;_LL4B: if(*((int*)_tmp8F)!= 5)goto _LL40;_tmp96=((struct Cyc_Parse_Decl_spec_struct*)
_tmp8F)->f1;_LL4C: last_loc=_tmp96->loc;if(declopt == 0  && !seen_type){seen_type=1;{
void*_tmp9E=(void*)_tmp96->r;struct Cyc_Absyn_Aggrdecl*_tmp9F;struct Cyc_Absyn_Datatypedecl*
_tmpA0;struct Cyc_Absyn_Enumdecl*_tmpA1;_LL53: if(_tmp9E <= (void*)2)goto _LL59;if(*((
int*)_tmp9E)!= 6)goto _LL55;_tmp9F=((struct Cyc_Absyn_Aggr_d_struct*)_tmp9E)->f1;
_LL54: {struct Cyc_List_List*_tmpA2=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmp9F->tvs));t=(void*)({struct Cyc_Absyn_AggrType_struct*
_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3[0]=({struct Cyc_Absyn_AggrType_struct
_tmpA4;_tmpA4.tag=10;_tmpA4.f1=({struct Cyc_Absyn_AggrInfo _tmpA5;_tmpA5.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmpA6;(_tmpA6.UnknownAggr).tag=
0;(_tmpA6.UnknownAggr).f1=(void*)((void*)_tmp9F->kind);(_tmpA6.UnknownAggr).f2=
_tmp9F->name;_tmpA6;});_tmpA5.targs=_tmpA2;_tmpA5;});_tmpA4;});_tmpA3;});if(
_tmp9F->impl != 0)declopt=({struct Cyc_Core_Opt*_tmpA7=_cycalloc(sizeof(*_tmpA7));
_tmpA7->v=_tmp96;_tmpA7;});goto _LL52;}_LL55: if(*((int*)_tmp9E)!= 7)goto _LL57;
_tmpA0=((struct Cyc_Absyn_Datatype_d_struct*)_tmp9E)->f1;_LL56: {struct Cyc_List_List*
_tmpA8=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmpA0->tvs));struct Cyc_Core_Opt*ropt=_tmpA0->is_flat?0:({struct Cyc_Core_Opt*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->v=(void*)((void*)2);_tmpAF;});t=(void*)({
struct Cyc_Absyn_DatatypeType_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({
struct Cyc_Absyn_DatatypeType_struct _tmpAA;_tmpAA.tag=2;_tmpAA.f1=({struct Cyc_Absyn_DatatypeInfo
_tmpAB;_tmpAB.datatype_info=(union Cyc_Absyn_DatatypeInfoU_union)({union Cyc_Absyn_DatatypeInfoU_union
_tmpAC;(_tmpAC.KnownDatatype).tag=1;(_tmpAC.KnownDatatype).f1=({struct Cyc_Absyn_Datatypedecl**
_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=_tmpA0;_tmpAD;});_tmpAC;});_tmpAB.targs=
_tmpA8;_tmpAB.rgn=ropt;_tmpAB;});_tmpAA;});_tmpA9;});if(_tmpA0->fields != 0)
declopt=({struct Cyc_Core_Opt*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->v=_tmp96;
_tmpAE;});goto _LL52;}_LL57: if(*((int*)_tmp9E)!= 8)goto _LL59;_tmpA1=((struct Cyc_Absyn_Enum_d_struct*)
_tmp9E)->f1;_LL58: t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmpB0=_cycalloc(
sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_EnumType_struct _tmpB1;_tmpB1.tag=12;
_tmpB1.f1=_tmpA1->name;_tmpB1.f2=0;_tmpB1;});_tmpB0;});declopt=({struct Cyc_Core_Opt*
_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->v=_tmp96;_tmpB2;});goto _LL52;_LL59:;
_LL5A:({void*_tmpB3=0;((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp96->loc,({const
char*_tmpB4="bad declaration within type specifier";_tag_dyneither(_tmpB4,
sizeof(char),38);}),_tag_dyneither(_tmpB3,sizeof(void*),0));});_LL52:;}}else{Cyc_Parse_err(
Cyc_Parse_msg3,_tmp96->loc);}goto _LL40;_LL40:;}if(!seen_type){if(!seen_sign  && !
seen_size)({void*_tmpB5=0;Cyc_Tcutil_warn(last_loc,({const char*_tmpB6="missing type within specifier";
_tag_dyneither(_tmpB6,sizeof(char),30);}),_tag_dyneither(_tmpB5,sizeof(void*),0));});
t=Cyc_Absyn_int_typ(sgn,sz);}else{if(seen_sign){void*_tmpB7=t;void*_tmpB8;void*
_tmpB9;_LL5C: if(_tmpB7 <= (void*)4)goto _LL5E;if(*((int*)_tmpB7)!= 5)goto _LL5E;
_tmpB8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB7)->f1;_tmpB9=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmpB7)->f2;_LL5D: if(_tmpB8 != sgn)t=Cyc_Absyn_int_typ(
sgn,_tmpB9);goto _LL5B;_LL5E:;_LL5F: Cyc_Parse_err(({const char*_tmpBA="sign specification on non-integral type";
_tag_dyneither(_tmpBA,sizeof(char),40);}),last_loc);goto _LL5B;_LL5B:;}if(
seen_size){void*_tmpBB=t;void*_tmpBC;void*_tmpBD;_LL61: if(_tmpBB <= (void*)4)goto
_LL65;if(*((int*)_tmpBB)!= 5)goto _LL63;_tmpBC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpBB)->f1;_tmpBD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBB)->f2;_LL62:
if(_tmpBD != sz)t=Cyc_Absyn_int_typ(_tmpBC,sz);goto _LL60;_LL63: if(*((int*)_tmpBB)
!= 6)goto _LL65;_LL64: t=Cyc_Absyn_double_typ(1);goto _LL60;_LL65:;_LL66: Cyc_Parse_err(({
const char*_tmpBE="size qualifier on non-integral type";_tag_dyneither(_tmpBE,
sizeof(char),36);}),last_loc);goto _LL60;_LL60:;}}return({struct _tuple5 _tmpBF;
_tmpBF.f1=t;_tmpBF.f2=declopt;_tmpBF;});}static struct Cyc_List_List*Cyc_Parse_apply_tmss(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*ds,
struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{struct Cyc_Parse_Declarator*
_tmpC0=(struct Cyc_Parse_Declarator*)ds->hd;struct _tuple1*_tmpC1=_tmpC0->id;
struct Cyc_Absyn_Tqual _tmpC3;void*_tmpC4;struct Cyc_List_List*_tmpC5;struct Cyc_List_List*
_tmpC6;struct _tuple6 _tmpC2=Cyc_Parse_apply_tms(tq,t,shared_atts,_tmpC0->tms);
_tmpC3=_tmpC2.f1;_tmpC4=_tmpC2.f2;_tmpC5=_tmpC2.f3;_tmpC6=_tmpC2.f4;if(ds->tl == 
0)return({struct Cyc_List_List*_tmpC7=_region_malloc(r,sizeof(*_tmpC7));_tmpC7->hd=({
struct _tuple7*_tmpC8=_region_malloc(r,sizeof(*_tmpC8));_tmpC8->f1=_tmpC1;_tmpC8->f2=
_tmpC3;_tmpC8->f3=_tmpC4;_tmpC8->f4=_tmpC5;_tmpC8->f5=_tmpC6;_tmpC8;});_tmpC7->tl=
0;_tmpC7;});else{return({struct Cyc_List_List*_tmpC9=_region_malloc(r,sizeof(*
_tmpC9));_tmpC9->hd=({struct _tuple7*_tmpCA=_region_malloc(r,sizeof(*_tmpCA));
_tmpCA->f1=_tmpC1;_tmpCA->f2=_tmpC3;_tmpCA->f3=_tmpC4;_tmpCA->f4=_tmpC5;_tmpCA->f5=
_tmpC6;_tmpCA;});_tmpC9->tl=Cyc_Parse_apply_tmss(r,_tmpC3,Cyc_Tcutil_copy_type(t),
ds->tl,shared_atts);_tmpC9;});}}}static struct _tuple6 Cyc_Parse_apply_tms(struct
Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){if(
tms == 0)return({struct _tuple6 _tmpCB;_tmpCB.f1=tq;_tmpCB.f2=t;_tmpCB.f3=0;_tmpCB.f4=
atts;_tmpCB;});{void*_tmpCC=(void*)tms->hd;struct Cyc_Absyn_Conref*_tmpCD;struct
Cyc_Position_Segment*_tmpCE;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Conref*
_tmpD0;struct Cyc_Position_Segment*_tmpD1;void*_tmpD2;struct Cyc_List_List*_tmpD3;
struct Cyc_Position_Segment*_tmpD4;struct Cyc_Absyn_PtrAtts _tmpD5;struct Cyc_Absyn_Tqual
_tmpD6;struct Cyc_Position_Segment*_tmpD7;struct Cyc_List_List*_tmpD8;_LL68: if(*((
int*)_tmpCC)!= 0)goto _LL6A;_tmpCD=((struct Cyc_Absyn_Carray_mod_struct*)_tmpCC)->f1;
_tmpCE=((struct Cyc_Absyn_Carray_mod_struct*)_tmpCC)->f2;_LL69: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,0,_tmpCD,_tmpCE),atts,tms->tl);
_LL6A: if(*((int*)_tmpCC)!= 1)goto _LL6C;_tmpCF=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpCC)->f1;_tmpD0=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCC)->f2;_tmpD1=((
struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCC)->f3;_LL6B: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmpCF,
_tmpD0,_tmpD1),atts,tms->tl);_LL6C: if(*((int*)_tmpCC)!= 3)goto _LL6E;_tmpD2=(void*)((
struct Cyc_Absyn_Function_mod_struct*)_tmpCC)->f1;_LL6D: {void*_tmpD9=_tmpD2;
struct Cyc_List_List*_tmpDA;int _tmpDB;struct Cyc_Absyn_VarargInfo*_tmpDC;struct Cyc_Core_Opt*
_tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_Position_Segment*_tmpDF;_LL75: if(*((
int*)_tmpD9)!= 1)goto _LL77;_tmpDA=((struct Cyc_Absyn_WithTypes_struct*)_tmpD9)->f1;
_tmpDB=((struct Cyc_Absyn_WithTypes_struct*)_tmpD9)->f2;_tmpDC=((struct Cyc_Absyn_WithTypes_struct*)
_tmpD9)->f3;_tmpDD=((struct Cyc_Absyn_WithTypes_struct*)_tmpD9)->f4;_tmpDE=((
struct Cyc_Absyn_WithTypes_struct*)_tmpD9)->f5;_LL76: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->hd=(void*)((
void*)as->hd);_tmpE0->tl=fn_atts;_tmpE0;});else{new_atts=({struct Cyc_List_List*
_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->hd=(void*)((void*)as->hd);_tmpE1->tl=
new_atts;_tmpE1;});}}}if(tms->tl != 0){void*_tmpE2=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;struct Cyc_List_List*_tmpE3;_LL7A: if(*((int*)_tmpE2)!= 
4)goto _LL7C;_tmpE3=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpE2)->f1;_LL7B:
typvars=_tmpE3;tms=tms->tl;goto _LL79;_LL7C:;_LL7D: goto _LL79;_LL79:;}if(((((!
_tmpDB  && _tmpDC == 0) && _tmpDA != 0) && _tmpDA->tl == 0) && (*((struct _tuple2*)
_tmpDA->hd)).f1 == 0) && (*((struct _tuple2*)_tmpDA->hd)).f3 == (void*)0)_tmpDA=0;t=
Cyc_Parse_array2ptr(t,0);((void(*)(void(*f)(struct _tuple2*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,_tmpDA);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(
tq.loc),Cyc_Absyn_function_typ(typvars,_tmpDD,t,_tmpDA,_tmpDB,_tmpDC,_tmpDE,
fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}_LL77: if(*((int*)
_tmpD9)!= 0)goto _LL74;_tmpDF=((struct Cyc_Absyn_NoTypes_struct*)_tmpD9)->f2;_LL78:({
void*_tmpE4=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpDF,({const char*_tmpE5="function declaration without parameter types";
_tag_dyneither(_tmpE5,sizeof(char),45);}),_tag_dyneither(_tmpE4,sizeof(void*),0));});
_LL74:;}_LL6E: if(*((int*)_tmpCC)!= 4)goto _LL70;_tmpD3=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpCC)->f1;_tmpD4=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCC)->f2;_LL6F:
if(tms->tl == 0)return({struct _tuple6 _tmpE6;_tmpE6.f1=tq;_tmpE6.f2=t;_tmpE6.f3=
_tmpD3;_tmpE6.f4=atts;_tmpE6;});({void*_tmpE7=0;((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD4,({
const char*_tmpE8="type parameters must appear before function arguments in declarator";
_tag_dyneither(_tmpE8,sizeof(char),68);}),_tag_dyneither(_tmpE7,sizeof(void*),0));});
_LL70: if(*((int*)_tmpCC)!= 2)goto _LL72;_tmpD5=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpCC)->f1;_tmpD6=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCC)->f2;_LL71:
return Cyc_Parse_apply_tms(_tmpD6,(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9[0]=({struct Cyc_Absyn_PointerType_struct
_tmpEA;_tmpEA.tag=4;_tmpEA.f1=({struct Cyc_Absyn_PtrInfo _tmpEB;_tmpEB.elt_typ=(
void*)t;_tmpEB.elt_tq=tq;_tmpEB.ptr_atts=_tmpD5;_tmpEB;});_tmpEA;});_tmpE9;}),
atts,tms->tl);_LL72: if(*((int*)_tmpCC)!= 5)goto _LL67;_tmpD7=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpCC)->f1;_tmpD8=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpCC)->f2;_LL73:
return Cyc_Parse_apply_tms(tq,t,Cyc_List_append(atts,_tmpD8),tms->tl);_LL67:;}}
void*Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*
loc){void*_tmpED;struct Cyc_Core_Opt*_tmpEE;struct _tuple5 _tmpEC=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmpED=_tmpEC.f1;_tmpEE=_tmpEC.f2;if(_tmpEE != 0)({void*_tmpEF=0;Cyc_Tcutil_warn(
loc,({const char*_tmpF0="ignoring nested type declaration(s) in specifier list";
_tag_dyneither(_tmpF0,sizeof(char),54);}),_tag_dyneither(_tmpEF,sizeof(void*),0));});
return _tmpED;}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*
loc,struct _tuple7*t){struct _tuple7 _tmpF2;struct _tuple1*_tmpF3;struct Cyc_Absyn_Tqual
_tmpF4;void*_tmpF5;struct Cyc_List_List*_tmpF6;struct Cyc_List_List*_tmpF7;struct
_tuple7*_tmpF1=t;_tmpF2=*_tmpF1;_tmpF3=_tmpF2.f1;_tmpF4=_tmpF2.f2;_tmpF5=_tmpF2.f3;
_tmpF6=_tmpF2.f4;_tmpF7=_tmpF2.f5;Cyc_Lex_register_typedef(_tmpF3);{struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*type;{void*_tmpF8=_tmpF5;struct Cyc_Core_Opt*_tmpF9;_LL7F:
if(_tmpF8 <= (void*)4)goto _LL81;if(*((int*)_tmpF8)!= 0)goto _LL81;_tmpF9=((struct
Cyc_Absyn_Evar_struct*)_tmpF8)->f1;_LL80: type=0;if(_tmpF9 == 0)kind=(struct Cyc_Core_Opt*)&
Cyc_Tcutil_bk;else{kind=_tmpF9;}goto _LL7E;_LL81:;_LL82: kind=0;type=({struct Cyc_Core_Opt*
_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->v=(void*)_tmpF5;_tmpFA;});goto _LL7E;
_LL7E:;}return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_struct*
_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB[0]=({struct Cyc_Absyn_Typedef_d_struct
_tmpFC;_tmpFC.tag=9;_tmpFC.f1=({struct Cyc_Absyn_Typedefdecl*_tmpFD=_cycalloc(
sizeof(*_tmpFD));_tmpFD->name=_tmpF3;_tmpFD->tvs=_tmpF6;_tmpFD->kind=kind;_tmpFD->defn=
type;_tmpFD->atts=_tmpF7;_tmpFD->tq=_tmpF4;_tmpFD;});_tmpFC;});_tmpFB;}),loc);}}
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct
Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Absyn_Decl_s_struct
_tmpFF;_tmpFF.tag=11;_tmpFF.f1=d;_tmpFF.f2=s;_tmpFF;});_tmpFE;}),d->loc);}static
struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,
struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*
f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*
accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}static struct Cyc_List_List*
Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*ds,struct Cyc_List_List*
ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*loc){struct
_RegionHandle _tmp100=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp100;
_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp102;struct Cyc_Absyn_Tqual
_tmp103;struct Cyc_List_List*_tmp104;struct Cyc_List_List*_tmp105;struct Cyc_Parse_Declaration_spec*
_tmp101=ds;_tmp102=*_tmp101;_tmp103=_tmp102.tq;_tmp104=_tmp102.type_specs;
_tmp105=_tmp102.attributes;if(_tmp103.loc == 0)_tmp103.loc=tqual_loc;if(ds->is_inline)
Cyc_Parse_err(({const char*_tmp106="inline is allowed only on function definitions";
_tag_dyneither(_tmp106,sizeof(char),47);}),loc);if(_tmp104 == 0){Cyc_Parse_err(({
const char*_tmp107="missing type specifiers in declaration";_tag_dyneither(
_tmp107,sizeof(char),39);}),loc);{struct Cyc_List_List*_tmp108=0;_npop_handler(0);
return _tmp108;}}{void*s=(void*)2;int istypedef=0;if(ds->sc != 0){void*_tmp109=(
void*)((struct Cyc_Core_Opt*)_check_null(ds->sc))->v;_LL84: if((int)_tmp109 != 0)
goto _LL86;_LL85: istypedef=1;goto _LL83;_LL86: if((int)_tmp109 != 1)goto _LL88;_LL87:
s=(void*)3;goto _LL83;_LL88: if((int)_tmp109 != 2)goto _LL8A;_LL89: s=(void*)4;goto
_LL83;_LL8A: if((int)_tmp109 != 3)goto _LL8C;_LL8B: s=(void*)0;goto _LL83;_LL8C: if((
int)_tmp109 != 4)goto _LL8E;_LL8D: s=(void*)2;goto _LL83;_LL8E: if((int)_tmp109 != 5)
goto _LL90;_LL8F: if(Cyc_Parse_no_register)s=(void*)2;else{s=(void*)5;}goto _LL83;
_LL90: if((int)_tmp109 != 6)goto _LL83;_LL91: s=(void*)1;goto _LL83;_LL83:;}{struct
Cyc_List_List*_tmp10B;struct Cyc_List_List*_tmp10C;struct _tuple0 _tmp10A=((struct
_tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))
Cyc_List_rsplit)(mkrgn,mkrgn,ids);_tmp10B=_tmp10A.f1;_tmp10C=_tmp10A.f2;{int
exps_empty=1;{struct Cyc_List_List*es=_tmp10C;for(0;es != 0;es=es->tl){if((struct
Cyc_Absyn_Exp*)es->hd != 0){exps_empty=0;break;}}}{struct _tuple5 _tmp10D=Cyc_Parse_collapse_type_specifiers(
_tmp104,loc);if(_tmp10B == 0){void*_tmp10F;struct Cyc_Core_Opt*_tmp110;struct
_tuple5 _tmp10E=_tmp10D;_tmp10F=_tmp10E.f1;_tmp110=_tmp10E.f2;if(_tmp110 != 0){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp110->v;{void*_tmp111=(void*)d->r;
struct Cyc_Absyn_Enumdecl*_tmp112;struct Cyc_Absyn_Aggrdecl*_tmp113;struct Cyc_Absyn_Datatypedecl*
_tmp114;_LL93: if(_tmp111 <= (void*)2)goto _LL99;if(*((int*)_tmp111)!= 8)goto _LL95;
_tmp112=((struct Cyc_Absyn_Enum_d_struct*)_tmp111)->f1;_LL94:(void*)(_tmp112->sc=(
void*)s);if(_tmp105 != 0)Cyc_Parse_err(({const char*_tmp115="bad attributes on enum";
_tag_dyneither(_tmp115,sizeof(char),23);}),loc);goto _LL92;_LL95: if(*((int*)
_tmp111)!= 6)goto _LL97;_tmp113=((struct Cyc_Absyn_Aggr_d_struct*)_tmp111)->f1;
_LL96:(void*)(_tmp113->sc=(void*)s);_tmp113->attributes=_tmp105;goto _LL92;_LL97:
if(*((int*)_tmp111)!= 7)goto _LL99;_tmp114=((struct Cyc_Absyn_Datatype_d_struct*)
_tmp111)->f1;_LL98:(void*)(_tmp114->sc=(void*)s);if(_tmp105 != 0)Cyc_Parse_err(({
const char*_tmp116="bad attributes on datatype";_tag_dyneither(_tmp116,sizeof(
char),27);}),loc);goto _LL92;_LL99:;_LL9A: Cyc_Parse_err(({const char*_tmp117="bad declaration";
_tag_dyneither(_tmp117,sizeof(char),16);}),loc);{struct Cyc_List_List*_tmp118=0;
_npop_handler(0);return _tmp118;}_LL92:;}{struct Cyc_List_List*_tmp11A=({struct Cyc_List_List*
_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->hd=d;_tmp119->tl=0;_tmp119;});
_npop_handler(0);return _tmp11A;}}else{void*_tmp11B=_tmp10F;struct Cyc_Absyn_AggrInfo
_tmp11C;union Cyc_Absyn_AggrInfoU_union _tmp11D;void*_tmp11E;struct _tuple1*_tmp11F;
struct Cyc_List_List*_tmp120;struct Cyc_Absyn_DatatypeInfo _tmp121;union Cyc_Absyn_DatatypeInfoU_union
_tmp122;struct Cyc_Absyn_Datatypedecl**_tmp123;struct Cyc_Absyn_DatatypeInfo
_tmp124;union Cyc_Absyn_DatatypeInfoU_union _tmp125;struct Cyc_Absyn_UnknownDatatypeInfo
_tmp126;struct _tuple1*_tmp127;int _tmp128;int _tmp129;struct Cyc_List_List*_tmp12A;
struct _tuple1*_tmp12B;struct Cyc_List_List*_tmp12C;_LL9C: if(_tmp11B <= (void*)4)
goto _LLA6;if(*((int*)_tmp11B)!= 10)goto _LL9E;_tmp11C=((struct Cyc_Absyn_AggrType_struct*)
_tmp11B)->f1;_tmp11D=_tmp11C.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp11B)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL9E;_tmp11E=(_tmp11D.UnknownAggr).f1;
_tmp11F=(_tmp11D.UnknownAggr).f2;_tmp120=_tmp11C.targs;_LL9D: {struct Cyc_List_List*
_tmp12D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp120);struct Cyc_Absyn_Aggrdecl*_tmp12E=({struct Cyc_Absyn_Aggrdecl*_tmp134=
_cycalloc(sizeof(*_tmp134));_tmp134->kind=(void*)_tmp11E;_tmp134->sc=(void*)s;
_tmp134->name=_tmp11F;_tmp134->tvs=_tmp12D;_tmp134->impl=0;_tmp134->attributes=0;
_tmp134;});if(_tmp105 != 0)Cyc_Parse_err(({const char*_tmp12F="bad attributes on type declaration";
_tag_dyneither(_tmp12F,sizeof(char),35);}),loc);{struct Cyc_List_List*_tmp133=({
struct Cyc_List_List*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp131=_cycalloc(sizeof(*_tmp131));
_tmp131[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp132;_tmp132.tag=6;_tmp132.f1=
_tmp12E;_tmp132;});_tmp131;}),loc);_tmp130->tl=0;_tmp130;});_npop_handler(0);
return _tmp133;}}_LL9E: if(*((int*)_tmp11B)!= 2)goto _LLA0;_tmp121=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp11B)->f1;_tmp122=_tmp121.datatype_info;if((((((struct Cyc_Absyn_DatatypeType_struct*)
_tmp11B)->f1).datatype_info).KnownDatatype).tag != 1)goto _LLA0;_tmp123=(_tmp122.KnownDatatype).f1;
_LL9F: if(_tmp105 != 0)Cyc_Parse_err(({const char*_tmp135="bad attributes on datatype";
_tag_dyneither(_tmp135,sizeof(char),27);}),loc);{struct Cyc_List_List*_tmp139=({
struct Cyc_List_List*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Datatype_d_struct*_tmp137=_cycalloc(sizeof(*_tmp137));
_tmp137[0]=({struct Cyc_Absyn_Datatype_d_struct _tmp138;_tmp138.tag=7;_tmp138.f1=*
_tmp123;_tmp138;});_tmp137;}),loc);_tmp136->tl=0;_tmp136;});_npop_handler(0);
return _tmp139;}_LLA0: if(*((int*)_tmp11B)!= 2)goto _LLA2;_tmp124=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp11B)->f1;_tmp125=_tmp124.datatype_info;if((((((struct Cyc_Absyn_DatatypeType_struct*)
_tmp11B)->f1).datatype_info).UnknownDatatype).tag != 0)goto _LLA2;_tmp126=(_tmp125.UnknownDatatype).f1;
_tmp127=_tmp126.name;_tmp128=_tmp126.is_extensible;_tmp129=_tmp126.is_flat;
_tmp12A=_tmp124.targs;_LLA1: {struct Cyc_List_List*_tmp13A=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp12A);
struct Cyc_Absyn_Decl*_tmp13B=Cyc_Absyn_datatype_decl(s,_tmp127,_tmp13A,0,_tmp128,
_tmp129,loc);if(_tmp105 != 0)Cyc_Parse_err(({const char*_tmp13C="bad attributes on datatype";
_tag_dyneither(_tmp13C,sizeof(char),27);}),loc);{struct Cyc_List_List*_tmp13E=({
struct Cyc_List_List*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->hd=_tmp13B;
_tmp13D->tl=0;_tmp13D;});_npop_handler(0);return _tmp13E;}}_LLA2: if(*((int*)
_tmp11B)!= 12)goto _LLA4;_tmp12B=((struct Cyc_Absyn_EnumType_struct*)_tmp11B)->f1;
_LLA3: {struct Cyc_Absyn_Enumdecl*_tmp13F=({struct Cyc_Absyn_Enumdecl*_tmp146=
_cycalloc(sizeof(*_tmp146));_tmp146->sc=(void*)s;_tmp146->name=_tmp12B;_tmp146->fields=
0;_tmp146;});if(_tmp105 != 0)Cyc_Parse_err(({const char*_tmp140="bad attributes on enum";
_tag_dyneither(_tmp140,sizeof(char),23);}),loc);{struct Cyc_List_List*_tmp145=({
struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->hd=({struct Cyc_Absyn_Decl*
_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp144;_tmp144.tag=8;_tmp144.f1=_tmp13F;_tmp144;});_tmp143;}));_tmp142->loc=loc;
_tmp142;});_tmp141->tl=0;_tmp141;});_npop_handler(0);return _tmp145;}}_LLA4: if(*((
int*)_tmp11B)!= 13)goto _LLA6;_tmp12C=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp11B)->f1;_LLA5: {struct Cyc_Absyn_Enumdecl*_tmp147=({struct Cyc_Absyn_Enumdecl*
_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E->sc=(void*)s;_tmp14E->name=Cyc_Parse_gensym_enum();
_tmp14E->fields=({struct Cyc_Core_Opt*_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F->v=
_tmp12C;_tmp14F;});_tmp14E;});if(_tmp105 != 0)Cyc_Parse_err(({const char*_tmp148="bad attributes on enum";
_tag_dyneither(_tmp148,sizeof(char),23);}),loc);{struct Cyc_List_List*_tmp14D=({
struct Cyc_List_List*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->hd=({struct Cyc_Absyn_Decl*
_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp14C;_tmp14C.tag=8;_tmp14C.f1=_tmp147;_tmp14C;});_tmp14B;}));_tmp14A->loc=loc;
_tmp14A;});_tmp149->tl=0;_tmp149;});_npop_handler(0);return _tmp14D;}}_LLA6:;
_LLA7: Cyc_Parse_err(({const char*_tmp150="missing declarator";_tag_dyneither(
_tmp150,sizeof(char),19);}),loc);{struct Cyc_List_List*_tmp151=0;_npop_handler(0);
return _tmp151;}_LL9B:;}}else{void*t=_tmp10D.f1;struct Cyc_List_List*_tmp152=Cyc_Parse_apply_tmss(
mkrgn,_tmp103,t,_tmp10B,_tmp105);if(istypedef){if(!exps_empty)Cyc_Parse_err(({
const char*_tmp153="initializer in typedef declaration";_tag_dyneither(_tmp153,
sizeof(char),35);}),loc);{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple7*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp152);if(_tmp10D.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(
_tmp10D.f2)->v;{void*_tmp154=(void*)d->r;struct Cyc_Absyn_Aggrdecl*_tmp155;struct
Cyc_Absyn_Datatypedecl*_tmp156;struct Cyc_Absyn_Enumdecl*_tmp157;_LLA9: if(_tmp154
<= (void*)2)goto _LLAF;if(*((int*)_tmp154)!= 6)goto _LLAB;_tmp155=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp154)->f1;_LLAA:(void*)(_tmp155->sc=(void*)s);_tmp155->attributes=_tmp105;
_tmp105=0;goto _LLA8;_LLAB: if(*((int*)_tmp154)!= 7)goto _LLAD;_tmp156=((struct Cyc_Absyn_Datatype_d_struct*)
_tmp154)->f1;_LLAC:(void*)(_tmp156->sc=(void*)s);goto _LLA8;_LLAD: if(*((int*)
_tmp154)!= 8)goto _LLAF;_tmp157=((struct Cyc_Absyn_Enum_d_struct*)_tmp154)->f1;
_LLAE:(void*)(_tmp157->sc=(void*)s);goto _LLA8;_LLAF:;_LLB0: Cyc_Parse_err(({const
char*_tmp158="declaration within typedef is not a struct, union, or datatype";
_tag_dyneither(_tmp158,sizeof(char),63);}),loc);goto _LLA8;_LLA8:;}decls=({struct
Cyc_List_List*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159->hd=d;_tmp159->tl=
decls;_tmp159;});}{struct Cyc_List_List*_tmp15A=decls;_npop_handler(0);return
_tmp15A;}}}else{if(_tmp10D.f2 != 0)Cyc_Parse_unimp(({const char*_tmp15B="nested type declaration within declarator";
_tag_dyneither(_tmp15B,sizeof(char),42);}),loc);{struct Cyc_List_List*decls=0;{
struct Cyc_List_List*_tmp15C=_tmp152;for(0;_tmp15C != 0;(_tmp15C=_tmp15C->tl,
_tmp10C=_tmp10C->tl)){struct _tuple7 _tmp15E;struct _tuple1*_tmp15F;struct Cyc_Absyn_Tqual
_tmp160;void*_tmp161;struct Cyc_List_List*_tmp162;struct Cyc_List_List*_tmp163;
struct _tuple7*_tmp15D=(struct _tuple7*)_tmp15C->hd;_tmp15E=*_tmp15D;_tmp15F=
_tmp15E.f1;_tmp160=_tmp15E.f2;_tmp161=_tmp15E.f3;_tmp162=_tmp15E.f4;_tmp163=
_tmp15E.f5;if(_tmp162 != 0)({void*_tmp164=0;Cyc_Tcutil_warn(loc,({const char*
_tmp165="bad type params, ignoring";_tag_dyneither(_tmp165,sizeof(char),26);}),
_tag_dyneither(_tmp164,sizeof(void*),0));});if(_tmp10C == 0)({void*_tmp166=0;((
int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*_tmp167="unexpected NULL in parse!";
_tag_dyneither(_tmp167,sizeof(char),26);}),_tag_dyneither(_tmp166,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*_tmp168=(struct Cyc_Absyn_Exp*)_tmp10C->hd;struct Cyc_Absyn_Vardecl*
_tmp169=Cyc_Absyn_new_vardecl(_tmp15F,_tmp161,_tmp168);_tmp169->tq=_tmp160;(void*)(
_tmp169->sc=(void*)s);_tmp169->attributes=_tmp163;{struct Cyc_Absyn_Decl*_tmp16A=({
struct Cyc_Absyn_Decl*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D[
0]=({struct Cyc_Absyn_Var_d_struct _tmp16E;_tmp16E.tag=0;_tmp16E.f1=_tmp169;
_tmp16E;});_tmp16D;}));_tmp16C->loc=loc;_tmp16C;});decls=({struct Cyc_List_List*
_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->hd=_tmp16A;_tmp16B->tl=decls;
_tmp16B;});}}}}{struct Cyc_List_List*_tmp16F=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp16F;}}}}}}}}};_pop_region(
mkrgn);}static void*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,struct Cyc_Position_Segment*
loc){if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct
_dyneither_ptr)s)== 2)switch(*((const char*)_check_dyneither_subscript(s,sizeof(
char),0))){case 'A': _LLB1: return(void*)0;case 'M': _LLB2: return(void*)1;case 'B':
_LLB3: return(void*)2;case 'R': _LLB4: return(void*)3;case 'U': _LLB5: if(*((const char*)
_check_dyneither_subscript(s,sizeof(char),1))== 'R')return(void*)4;else{break;}
case 'T': _LLB6: if(*((const char*)_check_dyneither_subscript(s,sizeof(char),1))== 'R')
return(void*)5;else{break;}case 'E': _LLB7: return(void*)6;case 'I': _LLB8: return(
void*)7;default: _LLB9: break;}Cyc_Parse_err((struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp173;_tmp173.tag=1;_tmp173.f1=(unsigned long)((int)Cyc_strlen((struct
_dyneither_ptr)s));{struct Cyc_String_pa_struct _tmp172;_tmp172.tag=0;_tmp172.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s);{void*_tmp170[2]={& _tmp172,&
_tmp173};Cyc_aprintf(({const char*_tmp171="bad kind: %s; strlen=%d";
_tag_dyneither(_tmp171,sizeof(char),24);}),_tag_dyneither(_tmp170,sizeof(void*),
2));}}}),loc);return(void*)2;}static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct
Cyc_Absyn_Pat*p){void*_tmp174=(void*)p->r;struct _tuple1*_tmp175;struct Cyc_Absyn_Vardecl*
_tmp176;struct Cyc_Absyn_Pat*_tmp177;struct Cyc_Absyn_Pat _tmp178;void*_tmp179;
struct Cyc_Absyn_Pat*_tmp17A;void*_tmp17B;int _tmp17C;char _tmp17D;struct
_dyneither_ptr _tmp17E;struct _tuple1*_tmp17F;struct Cyc_List_List*_tmp180;int
_tmp181;struct Cyc_Absyn_Exp*_tmp182;_LLBC: if(_tmp174 <= (void*)2)goto _LLC2;if(*((
int*)_tmp174)!= 12)goto _LLBE;_tmp175=((struct Cyc_Absyn_UnknownId_p_struct*)
_tmp174)->f1;_LLBD: return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp184;_tmp184.tag=2;_tmp184.f1=_tmp175;_tmp184;});_tmp183;}),p->loc);_LLBE: if(*((
int*)_tmp174)!= 1)goto _LLC0;_tmp176=((struct Cyc_Absyn_Reference_p_struct*)
_tmp174)->f1;_tmp177=((struct Cyc_Absyn_Reference_p_struct*)_tmp174)->f2;_tmp178=*
_tmp177;_tmp179=(void*)_tmp178.r;if((int)_tmp179 != 0)goto _LLC0;_LLBF: return Cyc_Absyn_deref_exp(
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp185=_cycalloc(
sizeof(*_tmp185));_tmp185[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp186;_tmp186.tag=
2;_tmp186.f1=_tmp176->name;_tmp186;});_tmp185;}),p->loc),p->loc);_LLC0: if(*((int*)
_tmp174)!= 4)goto _LLC2;_tmp17A=((struct Cyc_Absyn_Pointer_p_struct*)_tmp174)->f1;
_LLC1: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp17A),p->loc);_LLC2: if((
int)_tmp174 != 1)goto _LLC4;_LLC3: return Cyc_Absyn_null_exp(p->loc);_LLC4: if(
_tmp174 <= (void*)2)goto _LLCE;if(*((int*)_tmp174)!= 7)goto _LLC6;_tmp17B=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp174)->f1;_tmp17C=((struct Cyc_Absyn_Int_p_struct*)
_tmp174)->f2;_LLC5: return Cyc_Absyn_int_exp(_tmp17B,_tmp17C,p->loc);_LLC6: if(*((
int*)_tmp174)!= 8)goto _LLC8;_tmp17D=((struct Cyc_Absyn_Char_p_struct*)_tmp174)->f1;
_LLC7: return Cyc_Absyn_char_exp(_tmp17D,p->loc);_LLC8: if(*((int*)_tmp174)!= 9)
goto _LLCA;_tmp17E=((struct Cyc_Absyn_Float_p_struct*)_tmp174)->f1;_LLC9: return Cyc_Absyn_float_exp(
_tmp17E,p->loc);_LLCA: if(*((int*)_tmp174)!= 13)goto _LLCC;_tmp17F=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp174)->f1;_tmp180=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp174)->f2;
_tmp181=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp174)->f3;if(_tmp181 != 0)goto
_LLCC;_LLCB: {struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp188;_tmp188.tag=2;_tmp188.f1=_tmp17F;_tmp188;});_tmp187;}),p->loc);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp180);return Cyc_Absyn_unknowncall_exp(
e1,es,p->loc);}_LLCC: if(*((int*)_tmp174)!= 14)goto _LLCE;_tmp182=((struct Cyc_Absyn_Exp_p_struct*)
_tmp174)->f1;_LLCD: return _tmp182;_LLCE:;_LLCF: Cyc_Parse_err(({const char*_tmp189="cannot mix patterns and expressions in case";
_tag_dyneither(_tmp189,sizeof(char),44);}),p->loc);return Cyc_Absyn_null_exp(p->loc);
_LLBB:;}struct _tuple11{void*f1;int f2;};struct Cyc_Int_tok_struct{int tag;struct
_tuple11 f1;};struct Cyc_Char_tok_struct{int tag;char f1;};struct Cyc_String_tok_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Stringopt_tok_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_QualId_tok_struct{int tag;struct _tuple1*f1;};struct _tuple12{struct
Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Absyn_Conref*f3;};
struct Cyc_YY1_struct{int tag;struct _tuple12*f1;};struct Cyc_YY2_struct{int tag;
struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int tag;struct Cyc_Absyn_Exp*f1;}
;struct Cyc_YY4_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_YY5_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY7_struct{int tag;void*f1;};struct Cyc_YY8_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_YY9_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_YY10_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY11_struct{int tag;struct Cyc_Absyn_Pat*
f1;};struct _tuple13{struct Cyc_List_List*f1;int f2;};struct Cyc_YY12_struct{int tag;
struct _tuple13*f1;};struct Cyc_YY13_struct{int tag;struct Cyc_List_List*f1;};struct
_tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_YY14_struct{
int tag;struct _tuple14*f1;};struct Cyc_YY15_struct{int tag;struct Cyc_List_List*f1;}
;struct Cyc_YY16_struct{int tag;struct _tuple13*f1;};struct Cyc_YY17_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_YY18_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY19_struct{int tag;struct Cyc_Parse_Declaration_spec*f1;};struct
_tuple15{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_YY20_struct{
int tag;struct _tuple15*f1;};struct Cyc_YY21_struct{int tag;struct Cyc_List_List*f1;}
;struct Cyc_YY22_struct{int tag;void*f1;};struct Cyc_YY23_struct{int tag;void*f1;};
struct Cyc_YY24_struct{int tag;void*f1;};struct Cyc_YY25_struct{int tag;struct Cyc_Absyn_Tqual
f1;};struct Cyc_YY26_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY27_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY28_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY29_struct{int tag;struct Cyc_Parse_Declarator*f1;};struct Cyc_YY30_struct{
int tag;struct Cyc_Parse_Abstractdeclarator*f1;};struct Cyc_YY31_struct{int tag;int
f1;};struct Cyc_YY32_struct{int tag;void*f1;};struct Cyc_YY33_struct{int tag;struct
Cyc_Absyn_Datatypefield*f1;};struct Cyc_YY34_struct{int tag;struct Cyc_List_List*f1;
};struct _tuple16{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct Cyc_YY35_struct{int tag;struct _tuple16*f1;};struct Cyc_YY36_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY37_struct{int tag;struct _tuple2*f1;};
struct Cyc_YY38_struct{int tag;struct Cyc_List_List*f1;};struct _tuple17{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};struct Cyc_YY39_struct{int tag;struct _tuple17*f1;};struct Cyc_YY40_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY41_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY42_struct{int tag;void*f1;};struct Cyc_YY43_struct{int tag;void*f1;};
struct Cyc_YY44_struct{int tag;void*f1;};struct Cyc_YY45_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY46_struct{int tag;void*f1;};struct Cyc_YY47_struct{int tag;struct
Cyc_Absyn_Enumfield*f1;};struct Cyc_YY48_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY49_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY50_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY51_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_YY52_struct{int tag;struct Cyc_List_List*f1;};struct _tuple18{int f1;
int f2;};struct Cyc_YY53_struct{int tag;struct _tuple18 f1;};struct Cyc_YYINITIALSVAL_struct{
int tag;};union Cyc_YYSTYPE_union{struct Cyc_Int_tok_struct Int_tok;struct Cyc_Char_tok_struct
Char_tok;struct Cyc_String_tok_struct String_tok;struct Cyc_Stringopt_tok_struct
Stringopt_tok;struct Cyc_QualId_tok_struct QualId_tok;struct Cyc_YY1_struct YY1;
struct Cyc_YY2_struct YY2;struct Cyc_YY3_struct YY3;struct Cyc_YY4_struct YY4;struct
Cyc_YY5_struct YY5;struct Cyc_YY6_struct YY6;struct Cyc_YY7_struct YY7;struct Cyc_YY8_struct
YY8;struct Cyc_YY9_struct YY9;struct Cyc_YY10_struct YY10;struct Cyc_YY11_struct YY11;
struct Cyc_YY12_struct YY12;struct Cyc_YY13_struct YY13;struct Cyc_YY14_struct YY14;
struct Cyc_YY15_struct YY15;struct Cyc_YY16_struct YY16;struct Cyc_YY17_struct YY17;
struct Cyc_YY18_struct YY18;struct Cyc_YY19_struct YY19;struct Cyc_YY20_struct YY20;
struct Cyc_YY21_struct YY21;struct Cyc_YY22_struct YY22;struct Cyc_YY23_struct YY23;
struct Cyc_YY24_struct YY24;struct Cyc_YY25_struct YY25;struct Cyc_YY26_struct YY26;
struct Cyc_YY27_struct YY27;struct Cyc_YY28_struct YY28;struct Cyc_YY29_struct YY29;
struct Cyc_YY30_struct YY30;struct Cyc_YY31_struct YY31;struct Cyc_YY32_struct YY32;
struct Cyc_YY33_struct YY33;struct Cyc_YY34_struct YY34;struct Cyc_YY35_struct YY35;
struct Cyc_YY36_struct YY36;struct Cyc_YY37_struct YY37;struct Cyc_YY38_struct YY38;
struct Cyc_YY39_struct YY39;struct Cyc_YY40_struct YY40;struct Cyc_YY41_struct YY41;
struct Cyc_YY42_struct YY42;struct Cyc_YY43_struct YY43;struct Cyc_YY44_struct YY44;
struct Cyc_YY45_struct YY45;struct Cyc_YY46_struct YY46;struct Cyc_YY47_struct YY47;
struct Cyc_YY48_struct YY48;struct Cyc_YY49_struct YY49;struct Cyc_YY50_struct YY50;
struct Cyc_YY51_struct YY51;struct Cyc_YY52_struct YY52;struct Cyc_YY53_struct YY53;
struct Cyc_YYINITIALSVAL_struct YYINITIALSVAL;};static char _tmp18B[14]="$(sign_t,int)";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp18B,
_tmp18B,_tmp18B + 14}};struct _tuple11 Cyc_yyget_Int_tok(union Cyc_YYSTYPE_union yy1){
struct _tuple11 yyzzz;{union Cyc_YYSTYPE_union _tmp18C=yy1;struct _tuple11 _tmp18D;
_LLD1: if((_tmp18C.Int_tok).tag != 0)goto _LLD3;_tmp18D=(_tmp18C.Int_tok).f1;_LLD2:
yyzzz=_tmp18D;goto _LLD0;_LLD3:;_LLD4:(int)_throw((void*)& Cyc_yyfail_Int_tok);
_LLD0:;}return yyzzz;}static char _tmp18F[5]="char";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp18F,_tmp18F,_tmp18F + 5}};char Cyc_yyget_Char_tok(
union Cyc_YYSTYPE_union yy1){char yyzzz;{union Cyc_YYSTYPE_union _tmp190=yy1;char
_tmp191;_LLD6: if((_tmp190.Char_tok).tag != 1)goto _LLD8;_tmp191=(_tmp190.Char_tok).f1;
_LLD7: yyzzz=_tmp191;goto _LLD5;_LLD8:;_LLD9:(int)_throw((void*)& Cyc_yyfail_Char_tok);
_LLD5:;}return yyzzz;}static char _tmp193[9]="string_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp193,_tmp193,_tmp193 + 9}};struct
_dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE_union yy1){struct
_dyneither_ptr yyzzz;{union Cyc_YYSTYPE_union _tmp194=yy1;struct _dyneither_ptr
_tmp195;_LLDB: if((_tmp194.String_tok).tag != 2)goto _LLDD;_tmp195=(_tmp194.String_tok).f1;
_LLDC: yyzzz=_tmp195;goto _LLDA;_LLDD:;_LLDE:(int)_throw((void*)& Cyc_yyfail_String_tok);
_LLDA:;}return yyzzz;}static char _tmp197[54]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp197,
_tmp197,_tmp197 + 54}};struct _tuple12*Cyc_yyget_YY1(union Cyc_YYSTYPE_union yy1){
struct _tuple12*yyzzz;{union Cyc_YYSTYPE_union _tmp198=yy1;struct _tuple12*_tmp199;
_LLE0: if((_tmp198.YY1).tag != 5)goto _LLE2;_tmp199=(_tmp198.YY1).f1;_LLE1: yyzzz=
_tmp199;goto _LLDF;_LLE2:;_LLE3:(int)_throw((void*)& Cyc_yyfail_YY1);_LLDF:;}
return yyzzz;}static char _tmp19B[19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp19B,_tmp19B,_tmp19B + 19}};struct Cyc_Absyn_Conref*
Cyc_yyget_YY2(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Conref*yyzzz;{union Cyc_YYSTYPE_union
_tmp19C=yy1;struct Cyc_Absyn_Conref*_tmp19D;_LLE5: if((_tmp19C.YY2).tag != 6)goto
_LLE7;_tmp19D=(_tmp19C.YY2).f1;_LLE6: yyzzz=_tmp19D;goto _LLE4;_LLE7:;_LLE8:(int)
_throw((void*)& Cyc_yyfail_YY2);_LLE4:;}return yyzzz;}static char _tmp19F[6]="exp_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp19F,
_tmp19F,_tmp19F + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union Cyc_YYSTYPE_union yy1){
struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE_union _tmp1A0=yy1;struct Cyc_Absyn_Exp*
_tmp1A1;_LLEA: if((_tmp1A0.YY3).tag != 7)goto _LLEC;_tmp1A1=(_tmp1A0.YY3).f1;_LLEB:
yyzzz=_tmp1A1;goto _LLE9;_LLEC:;_LLED:(int)_throw((void*)& Cyc_yyfail_YY3);_LLE9:;}
return yyzzz;}static char _tmp1A3[10]="exp_opt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1A3,_tmp1A3,_tmp1A3 + 10}};struct Cyc_Absyn_Exp*
Cyc_yyget_YY4(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE_union
_tmp1A4=yy1;struct Cyc_Absyn_Exp*_tmp1A5;_LLEF: if((_tmp1A4.YY4).tag != 8)goto _LLF1;
_tmp1A5=(_tmp1A4.YY4).f1;_LLF0: yyzzz=_tmp1A5;goto _LLEE;_LLF1:;_LLF2:(int)_throw((
void*)& Cyc_yyfail_YY4);_LLEE:;}return yyzzz;}static char _tmp1A7[14]="list_t<exp_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1A7,
_tmp1A7,_tmp1A7 + 14}};struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1A8=yy1;struct Cyc_List_List*
_tmp1A9;_LLF4: if((_tmp1A8.YY5).tag != 9)goto _LLF6;_tmp1A9=(_tmp1A8.YY5).f1;_LLF5:
yyzzz=_tmp1A9;goto _LLF3;_LLF6:;_LLF7:(int)_throw((void*)& Cyc_yyfail_YY5);_LLF3:;}
return yyzzz;}static char _tmp1AB[39]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1AB,
_tmp1AB,_tmp1AB + 39}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1AC=yy1;struct Cyc_List_List*
_tmp1AD;_LLF9: if((_tmp1AC.YY6).tag != 10)goto _LLFB;_tmp1AD=(_tmp1AC.YY6).f1;_LLFA:
yyzzz=_tmp1AD;goto _LLF8;_LLFB:;_LLFC:(int)_throw((void*)& Cyc_yyfail_YY6);_LLF8:;}
return yyzzz;}static char _tmp1AF[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1AF,_tmp1AF,_tmp1AF + 9}};void*Cyc_yyget_YY7(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1B0=yy1;void*
_tmp1B1;_LLFE: if((_tmp1B0.YY7).tag != 11)goto _LL100;_tmp1B1=(_tmp1B0.YY7).f1;
_LLFF: yyzzz=_tmp1B1;goto _LLFD;_LL100:;_LL101:(int)_throw((void*)& Cyc_yyfail_YY7);
_LLFD:;}return yyzzz;}static char _tmp1B3[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp1B3,_tmp1B3,_tmp1B3 + 16}};struct Cyc_Core_Opt*
Cyc_yyget_YY8(union Cyc_YYSTYPE_union yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE_union
_tmp1B4=yy1;struct Cyc_Core_Opt*_tmp1B5;_LL103: if((_tmp1B4.YY8).tag != 12)goto
_LL105;_tmp1B5=(_tmp1B4.YY8).f1;_LL104: yyzzz=_tmp1B5;goto _LL102;_LL105:;_LL106:(
int)_throw((void*)& Cyc_yyfail_YY8);_LL102:;}return yyzzz;}static char _tmp1B7[7]="qvar_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{
_tmp1B7,_tmp1B7,_tmp1B7 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE_union
yy1){struct _tuple1*yyzzz;{union Cyc_YYSTYPE_union _tmp1B8=yy1;struct _tuple1*
_tmp1B9;_LL108: if((_tmp1B8.QualId_tok).tag != 4)goto _LL10A;_tmp1B9=(_tmp1B8.QualId_tok).f1;
_LL109: yyzzz=_tmp1B9;goto _LL107;_LL10A:;_LL10B:(int)_throw((void*)& Cyc_yyfail_QualId_tok);
_LL107:;}return yyzzz;}static char _tmp1BB[7]="stmt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp1BB,_tmp1BB,_tmp1BB + 7}};struct Cyc_Absyn_Stmt*
Cyc_yyget_YY9(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE_union
_tmp1BC=yy1;struct Cyc_Absyn_Stmt*_tmp1BD;_LL10D: if((_tmp1BC.YY9).tag != 13)goto
_LL10F;_tmp1BD=(_tmp1BC.YY9).f1;_LL10E: yyzzz=_tmp1BD;goto _LL10C;_LL10F:;_LL110:(
int)_throw((void*)& Cyc_yyfail_YY9);_LL10C:;}return yyzzz;}static char _tmp1BF[24]="list_t<switch_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp1BF,
_tmp1BF,_tmp1BF + 24}};struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1C0=yy1;struct Cyc_List_List*
_tmp1C1;_LL112: if((_tmp1C0.YY10).tag != 14)goto _LL114;_tmp1C1=(_tmp1C0.YY10).f1;
_LL113: yyzzz=_tmp1C1;goto _LL111;_LL114:;_LL115:(int)_throw((void*)& Cyc_yyfail_YY10);
_LL111:;}return yyzzz;}static char _tmp1C3[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp1C3,_tmp1C3,_tmp1C3 + 6}};struct Cyc_Absyn_Pat*
Cyc_yyget_YY11(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE_union
_tmp1C4=yy1;struct Cyc_Absyn_Pat*_tmp1C5;_LL117: if((_tmp1C4.YY11).tag != 15)goto
_LL119;_tmp1C5=(_tmp1C4.YY11).f1;_LL118: yyzzz=_tmp1C5;goto _LL116;_LL119:;_LL11A:(
int)_throw((void*)& Cyc_yyfail_YY11);_LL116:;}return yyzzz;}static char _tmp1C7[23]="$(list_t<pat_t>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp1C7,
_tmp1C7,_tmp1C7 + 23}};struct _tuple13*Cyc_yyget_YY12(union Cyc_YYSTYPE_union yy1){
struct _tuple13*yyzzz;{union Cyc_YYSTYPE_union _tmp1C8=yy1;struct _tuple13*_tmp1C9;
_LL11C: if((_tmp1C8.YY12).tag != 16)goto _LL11E;_tmp1C9=(_tmp1C8.YY12).f1;_LL11D:
yyzzz=_tmp1C9;goto _LL11B;_LL11E:;_LL11F:(int)_throw((void*)& Cyc_yyfail_YY12);
_LL11B:;}return yyzzz;}static char _tmp1CB[14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp1CB,_tmp1CB,_tmp1CB + 14}};struct Cyc_List_List*
Cyc_yyget_YY13(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp1CC=yy1;struct Cyc_List_List*_tmp1CD;_LL121: if((_tmp1CC.YY13).tag != 17)goto
_LL123;_tmp1CD=(_tmp1CC.YY13).f1;_LL122: yyzzz=_tmp1CD;goto _LL120;_LL123:;_LL124:(
int)_throw((void*)& Cyc_yyfail_YY13);_LL120:;}return yyzzz;}static char _tmp1CF[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp1CF,
_tmp1CF,_tmp1CF + 31}};struct _tuple14*Cyc_yyget_YY14(union Cyc_YYSTYPE_union yy1){
struct _tuple14*yyzzz;{union Cyc_YYSTYPE_union _tmp1D0=yy1;struct _tuple14*_tmp1D1;
_LL126: if((_tmp1D0.YY14).tag != 18)goto _LL128;_tmp1D1=(_tmp1D0.YY14).f1;_LL127:
yyzzz=_tmp1D1;goto _LL125;_LL128:;_LL129:(int)_throw((void*)& Cyc_yyfail_YY14);
_LL125:;}return yyzzz;}static char _tmp1D3[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp1D3,
_tmp1D3,_tmp1D3 + 39}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1D4=yy1;struct Cyc_List_List*
_tmp1D5;_LL12B: if((_tmp1D4.YY15).tag != 19)goto _LL12D;_tmp1D5=(_tmp1D4.YY15).f1;
_LL12C: yyzzz=_tmp1D5;goto _LL12A;_LL12D:;_LL12E:(int)_throw((void*)& Cyc_yyfail_YY15);
_LL12A:;}return yyzzz;}static char _tmp1D7[48]="$(list_t<$(list_t<designator_t>,pat_t)@>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp1D7,
_tmp1D7,_tmp1D7 + 48}};struct _tuple13*Cyc_yyget_YY16(union Cyc_YYSTYPE_union yy1){
struct _tuple13*yyzzz;{union Cyc_YYSTYPE_union _tmp1D8=yy1;struct _tuple13*_tmp1D9;
_LL130: if((_tmp1D8.YY16).tag != 20)goto _LL132;_tmp1D9=(_tmp1D8.YY16).f1;_LL131:
yyzzz=_tmp1D9;goto _LL12F;_LL132:;_LL133:(int)_throw((void*)& Cyc_yyfail_YY16);
_LL12F:;}return yyzzz;}static char _tmp1DB[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp1DB,_tmp1DB,_tmp1DB + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_YY17(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE_union
_tmp1DC=yy1;struct Cyc_Absyn_Fndecl*_tmp1DD;_LL135: if((_tmp1DC.YY17).tag != 21)
goto _LL137;_tmp1DD=(_tmp1DC.YY17).f1;_LL136: yyzzz=_tmp1DD;goto _LL134;_LL137:;
_LL138:(int)_throw((void*)& Cyc_yyfail_YY17);_LL134:;}return yyzzz;}static char
_tmp1DF[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={
Cyc_Core_Failure,{_tmp1DF,_tmp1DF,_tmp1DF + 15}};struct Cyc_List_List*Cyc_yyget_YY18(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp1E0=yy1;struct Cyc_List_List*_tmp1E1;_LL13A: if((_tmp1E0.YY18).tag != 22)goto
_LL13C;_tmp1E1=(_tmp1E0.YY18).f1;_LL13B: yyzzz=_tmp1E1;goto _LL139;_LL13C:;_LL13D:(
int)_throw((void*)& Cyc_yyfail_YY18);_LL139:;}return yyzzz;}static char _tmp1E3[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp1E3,
_tmp1E3,_tmp1E3 + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE_union
yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE_union _tmp1E4=yy1;
struct Cyc_Parse_Declaration_spec*_tmp1E5;_LL13F: if((_tmp1E4.YY19).tag != 23)goto
_LL141;_tmp1E5=(_tmp1E4.YY19).f1;_LL140: yyzzz=_tmp1E5;goto _LL13E;_LL141:;_LL142:(
int)_throw((void*)& Cyc_yyfail_YY19);_LL13E:;}return yyzzz;}static char _tmp1E7[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp1E7,
_tmp1E7,_tmp1E7 + 27}};struct _tuple15*Cyc_yyget_YY20(union Cyc_YYSTYPE_union yy1){
struct _tuple15*yyzzz;{union Cyc_YYSTYPE_union _tmp1E8=yy1;struct _tuple15*_tmp1E9;
_LL144: if((_tmp1E8.YY20).tag != 24)goto _LL146;_tmp1E9=(_tmp1E8.YY20).f1;_LL145:
yyzzz=_tmp1E9;goto _LL143;_LL146:;_LL147:(int)_throw((void*)& Cyc_yyfail_YY20);
_LL143:;}return yyzzz;}static char _tmp1EB[35]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp1EB,
_tmp1EB,_tmp1EB + 35}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1EC=yy1;struct Cyc_List_List*
_tmp1ED;_LL149: if((_tmp1EC.YY21).tag != 25)goto _LL14B;_tmp1ED=(_tmp1EC.YY21).f1;
_LL14A: yyzzz=_tmp1ED;goto _LL148;_LL14B:;_LL14C:(int)_throw((void*)& Cyc_yyfail_YY21);
_LL148:;}return yyzzz;}static char _tmp1EF[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp1EF,_tmp1EF,_tmp1EF + 16}};void*Cyc_yyget_YY22(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1F0=yy1;void*
_tmp1F1;_LL14E: if((_tmp1F0.YY22).tag != 26)goto _LL150;_tmp1F1=(_tmp1F0.YY22).f1;
_LL14F: yyzzz=_tmp1F1;goto _LL14D;_LL150:;_LL151:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL14D:;}return yyzzz;}static char _tmp1F3[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp1F3,_tmp1F3,_tmp1F3 + 17}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1F4=yy1;void*
_tmp1F5;_LL153: if((_tmp1F4.YY23).tag != 27)goto _LL155;_tmp1F5=(_tmp1F4.YY23).f1;
_LL154: yyzzz=_tmp1F5;goto _LL152;_LL155:;_LL156:(int)_throw((void*)& Cyc_yyfail_YY23);
_LL152:;}return yyzzz;}static char _tmp1F7[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp1F7,_tmp1F7,_tmp1F7 + 12}};void*Cyc_yyget_YY24(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1F8=yy1;void*
_tmp1F9;_LL158: if((_tmp1F8.YY24).tag != 28)goto _LL15A;_tmp1F9=(_tmp1F8.YY24).f1;
_LL159: yyzzz=_tmp1F9;goto _LL157;_LL15A:;_LL15B:(int)_throw((void*)& Cyc_yyfail_YY24);
_LL157:;}return yyzzz;}static char _tmp1FB[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp1FB,_tmp1FB,_tmp1FB + 8}};struct Cyc_Absyn_Tqual
Cyc_yyget_YY25(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE_union
_tmp1FC=yy1;struct Cyc_Absyn_Tqual _tmp1FD;_LL15D: if((_tmp1FC.YY25).tag != 29)goto
_LL15F;_tmp1FD=(_tmp1FC.YY25).f1;_LL15E: yyzzz=_tmp1FD;goto _LL15C;_LL15F:;_LL160:(
int)_throw((void*)& Cyc_yyfail_YY25);_LL15C:;}return yyzzz;}static char _tmp1FF[20]="list_t<aggrfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp1FF,
_tmp1FF,_tmp1FF + 20}};struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp200=yy1;struct Cyc_List_List*
_tmp201;_LL162: if((_tmp200.YY26).tag != 30)goto _LL164;_tmp201=(_tmp200.YY26).f1;
_LL163: yyzzz=_tmp201;goto _LL161;_LL164:;_LL165:(int)_throw((void*)& Cyc_yyfail_YY26);
_LL161:;}return yyzzz;}static char _tmp203[28]="list_t<list_t<aggrfield_t>>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp203,_tmp203,
_tmp203 + 28}};struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp204=yy1;struct Cyc_List_List*
_tmp205;_LL167: if((_tmp204.YY27).tag != 31)goto _LL169;_tmp205=(_tmp204.YY27).f1;
_LL168: yyzzz=_tmp205;goto _LL166;_LL169:;_LL16A:(int)_throw((void*)& Cyc_yyfail_YY27);
_LL166:;}return yyzzz;}static char _tmp207[24]="list_t<type_modifier_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp207,_tmp207,
_tmp207 + 24}};struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp208=yy1;struct Cyc_List_List*
_tmp209;_LL16C: if((_tmp208.YY28).tag != 32)goto _LL16E;_tmp209=(_tmp208.YY28).f1;
_LL16D: yyzzz=_tmp209;goto _LL16B;_LL16E:;_LL16F:(int)_throw((void*)& Cyc_yyfail_YY28);
_LL16B:;}return yyzzz;}static char _tmp20B[13]="declarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp20B,_tmp20B,_tmp20B + 13}};struct Cyc_Parse_Declarator*
Cyc_yyget_YY29(union Cyc_YYSTYPE_union yy1){struct Cyc_Parse_Declarator*yyzzz;{
union Cyc_YYSTYPE_union _tmp20C=yy1;struct Cyc_Parse_Declarator*_tmp20D;_LL171: if((
_tmp20C.YY29).tag != 33)goto _LL173;_tmp20D=(_tmp20C.YY29).f1;_LL172: yyzzz=_tmp20D;
goto _LL170;_LL173:;_LL174:(int)_throw((void*)& Cyc_yyfail_YY29);_LL170:;}return
yyzzz;}static char _tmp20F[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp20F,_tmp20F,_tmp20F + 21}};struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_YY30(union Cyc_YYSTYPE_union yy1){struct Cyc_Parse_Abstractdeclarator*
yyzzz;{union Cyc_YYSTYPE_union _tmp210=yy1;struct Cyc_Parse_Abstractdeclarator*
_tmp211;_LL176: if((_tmp210.YY30).tag != 34)goto _LL178;_tmp211=(_tmp210.YY30).f1;
_LL177: yyzzz=_tmp211;goto _LL175;_LL178:;_LL179:(int)_throw((void*)& Cyc_yyfail_YY30);
_LL175:;}return yyzzz;}static char _tmp213[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp213,_tmp213,_tmp213 + 5}};int Cyc_yyget_YY31(
union Cyc_YYSTYPE_union yy1){int yyzzz;{union Cyc_YYSTYPE_union _tmp214=yy1;int
_tmp215;_LL17B: if((_tmp214.YY31).tag != 35)goto _LL17D;_tmp215=(_tmp214.YY31).f1;
_LL17C: yyzzz=_tmp215;goto _LL17A;_LL17D:;_LL17E:(int)_throw((void*)& Cyc_yyfail_YY31);
_LL17A:;}return yyzzz;}static char _tmp217[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp217,_tmp217,_tmp217 + 8}};void*Cyc_yyget_YY32(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp218=yy1;void*
_tmp219;_LL180: if((_tmp218.YY32).tag != 36)goto _LL182;_tmp219=(_tmp218.YY32).f1;
_LL181: yyzzz=_tmp219;goto _LL17F;_LL182:;_LL183:(int)_throw((void*)& Cyc_yyfail_YY32);
_LL17F:;}return yyzzz;}static char _tmp21B[16]="datatypefield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp21B,_tmp21B,_tmp21B + 16}};struct Cyc_Absyn_Datatypefield*
Cyc_yyget_YY33(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Datatypefield*yyzzz;{
union Cyc_YYSTYPE_union _tmp21C=yy1;struct Cyc_Absyn_Datatypefield*_tmp21D;_LL185:
if((_tmp21C.YY33).tag != 37)goto _LL187;_tmp21D=(_tmp21C.YY33).f1;_LL186: yyzzz=
_tmp21D;goto _LL184;_LL187:;_LL188:(int)_throw((void*)& Cyc_yyfail_YY33);_LL184:;}
return yyzzz;}static char _tmp21F[24]="list_t<datatypefield_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp21F,_tmp21F,_tmp21F + 24}};struct Cyc_List_List*
Cyc_yyget_YY34(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp220=yy1;struct Cyc_List_List*_tmp221;_LL18A: if((_tmp220.YY34).tag != 38)goto
_LL18C;_tmp221=(_tmp220.YY34).f1;_LL18B: yyzzz=_tmp221;goto _LL189;_LL18C:;_LL18D:(
int)_throw((void*)& Cyc_yyfail_YY34);_LL189:;}return yyzzz;}static char _tmp223[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp223,
_tmp223,_tmp223 + 50}};struct _tuple16*Cyc_yyget_YY35(union Cyc_YYSTYPE_union yy1){
struct _tuple16*yyzzz;{union Cyc_YYSTYPE_union _tmp224=yy1;struct _tuple16*_tmp225;
_LL18F: if((_tmp224.YY35).tag != 39)goto _LL191;_tmp225=(_tmp224.YY35).f1;_LL190:
yyzzz=_tmp225;goto _LL18E;_LL191:;_LL192:(int)_throw((void*)& Cyc_yyfail_YY35);
_LL18E:;}return yyzzz;}static char _tmp227[14]="list_t<var_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp227,_tmp227,_tmp227 + 14}};struct Cyc_List_List*
Cyc_yyget_YY36(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp228=yy1;struct Cyc_List_List*_tmp229;_LL194: if((_tmp228.YY36).tag != 40)goto
_LL196;_tmp229=(_tmp228.YY36).f1;_LL195: yyzzz=_tmp229;goto _LL193;_LL196:;_LL197:(
int)_throw((void*)& Cyc_yyfail_YY36);_LL193:;}return yyzzz;}static char _tmp22B[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp22B,
_tmp22B,_tmp22B + 32}};struct _tuple2*Cyc_yyget_YY37(union Cyc_YYSTYPE_union yy1){
struct _tuple2*yyzzz;{union Cyc_YYSTYPE_union _tmp22C=yy1;struct _tuple2*_tmp22D;
_LL199: if((_tmp22C.YY37).tag != 41)goto _LL19B;_tmp22D=(_tmp22C.YY37).f1;_LL19A:
yyzzz=_tmp22D;goto _LL198;_LL19B:;_LL19C:(int)_throw((void*)& Cyc_yyfail_YY37);
_LL198:;}return yyzzz;}static char _tmp22F[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp22F,
_tmp22F,_tmp22F + 40}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp230=yy1;struct Cyc_List_List*
_tmp231;_LL19E: if((_tmp230.YY38).tag != 42)goto _LL1A0;_tmp231=(_tmp230.YY38).f1;
_LL19F: yyzzz=_tmp231;goto _LL19D;_LL1A0:;_LL1A1:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL19D:;}return yyzzz;}static char _tmp233[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp233,
_tmp233,_tmp233 + 107}};struct _tuple17*Cyc_yyget_YY39(union Cyc_YYSTYPE_union yy1){
struct _tuple17*yyzzz;{union Cyc_YYSTYPE_union _tmp234=yy1;struct _tuple17*_tmp235;
_LL1A3: if((_tmp234.YY39).tag != 43)goto _LL1A5;_tmp235=(_tmp234.YY39).f1;_LL1A4:
yyzzz=_tmp235;goto _LL1A2;_LL1A5:;_LL1A6:(int)_throw((void*)& Cyc_yyfail_YY39);
_LL1A2:;}return yyzzz;}static char _tmp237[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp237,_tmp237,_tmp237 + 15}};struct Cyc_List_List*
Cyc_yyget_YY40(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp238=yy1;struct Cyc_List_List*_tmp239;_LL1A8: if((_tmp238.YY40).tag != 44)goto
_LL1AA;_tmp239=(_tmp238.YY40).f1;_LL1A9: yyzzz=_tmp239;goto _LL1A7;_LL1AA:;_LL1AB:(
int)_throw((void*)& Cyc_yyfail_YY40);_LL1A7:;}return yyzzz;}static char _tmp23B[21]="list_t<designator_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp23B,
_tmp23B,_tmp23B + 21}};struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp23C=yy1;struct Cyc_List_List*
_tmp23D;_LL1AD: if((_tmp23C.YY41).tag != 45)goto _LL1AF;_tmp23D=(_tmp23C.YY41).f1;
_LL1AE: yyzzz=_tmp23D;goto _LL1AC;_LL1AF:;_LL1B0:(int)_throw((void*)& Cyc_yyfail_YY41);
_LL1AC:;}return yyzzz;}static char _tmp23F[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp23F,_tmp23F,_tmp23F + 13}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp240=yy1;void*
_tmp241;_LL1B2: if((_tmp240.YY42).tag != 46)goto _LL1B4;_tmp241=(_tmp240.YY42).f1;
_LL1B3: yyzzz=_tmp241;goto _LL1B1;_LL1B4:;_LL1B5:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1B1:;}return yyzzz;}static char _tmp243[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp243,_tmp243,_tmp243 + 7}};void*Cyc_yyget_YY43(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp244=yy1;void*
_tmp245;_LL1B7: if((_tmp244.YY43).tag != 47)goto _LL1B9;_tmp245=(_tmp244.YY43).f1;
_LL1B8: yyzzz=_tmp245;goto _LL1B6;_LL1B9:;_LL1BA:(int)_throw((void*)& Cyc_yyfail_YY43);
_LL1B6:;}return yyzzz;}static char _tmp247[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp247,_tmp247,_tmp247 + 7}};void*Cyc_yyget_YY44(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp248=yy1;void*
_tmp249;_LL1BC: if((_tmp248.YY44).tag != 48)goto _LL1BE;_tmp249=(_tmp248.YY44).f1;
_LL1BD: yyzzz=_tmp249;goto _LL1BB;_LL1BE:;_LL1BF:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1BB:;}return yyzzz;}static char _tmp24B[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp24B,_tmp24B,
_tmp24B + 20}};struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp24C=yy1;struct Cyc_List_List*
_tmp24D;_LL1C1: if((_tmp24C.YY45).tag != 49)goto _LL1C3;_tmp24D=(_tmp24C.YY45).f1;
_LL1C2: yyzzz=_tmp24D;goto _LL1C0;_LL1C3:;_LL1C4:(int)_throw((void*)& Cyc_yyfail_YY45);
_LL1C0:;}return yyzzz;}static char _tmp24F[12]="attribute_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp24F,_tmp24F,_tmp24F + 12}};void*Cyc_yyget_YY46(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp250=yy1;void*
_tmp251;_LL1C6: if((_tmp250.YY46).tag != 50)goto _LL1C8;_tmp251=(_tmp250.YY46).f1;
_LL1C7: yyzzz=_tmp251;goto _LL1C5;_LL1C8:;_LL1C9:(int)_throw((void*)& Cyc_yyfail_YY46);
_LL1C5:;}return yyzzz;}static char _tmp253[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp253,_tmp253,_tmp253 + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union
Cyc_YYSTYPE_union _tmp254=yy1;struct Cyc_Absyn_Enumfield*_tmp255;_LL1CB: if((
_tmp254.YY47).tag != 51)goto _LL1CD;_tmp255=(_tmp254.YY47).f1;_LL1CC: yyzzz=_tmp255;
goto _LL1CA;_LL1CD:;_LL1CE:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1CA:;}return
yyzzz;}static char _tmp257[20]="list_t<enumfield_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp257,_tmp257,_tmp257 + 20}};struct Cyc_List_List*
Cyc_yyget_YY48(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp258=yy1;struct Cyc_List_List*_tmp259;_LL1D0: if((_tmp258.YY48).tag != 52)goto
_LL1D2;_tmp259=(_tmp258.YY48).f1;_LL1D1: yyzzz=_tmp259;goto _LL1CF;_LL1D2:;_LL1D3:(
int)_throw((void*)& Cyc_yyfail_YY48);_LL1CF:;}return yyzzz;}static char _tmp25B[14]="opt_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp25B,
_tmp25B,_tmp25B + 14}};struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE_union
yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE_union _tmp25C=yy1;struct Cyc_Core_Opt*
_tmp25D;_LL1D5: if((_tmp25C.YY49).tag != 53)goto _LL1D7;_tmp25D=(_tmp25C.YY49).f1;
_LL1D6: yyzzz=_tmp25D;goto _LL1D4;_LL1D7:;_LL1D8:(int)_throw((void*)& Cyc_yyfail_YY49);
_LL1D4:;}return yyzzz;}static char _tmp25F[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp25F,_tmp25F,
_tmp25F + 26}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp260=yy1;struct Cyc_List_List*
_tmp261;_LL1DA: if((_tmp260.YY50).tag != 54)goto _LL1DC;_tmp261=(_tmp260.YY50).f1;
_LL1DB: yyzzz=_tmp261;goto _LL1D9;_LL1DC:;_LL1DD:(int)_throw((void*)& Cyc_yyfail_YY50);
_LL1D9:;}return yyzzz;}static char _tmp263[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp263,_tmp263,_tmp263 + 15}};struct Cyc_Absyn_Conref*
Cyc_yyget_YY51(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Conref*yyzzz;{union Cyc_YYSTYPE_union
_tmp264=yy1;struct Cyc_Absyn_Conref*_tmp265;_LL1DF: if((_tmp264.YY51).tag != 55)
goto _LL1E1;_tmp265=(_tmp264.YY51).f1;_LL1E0: yyzzz=_tmp265;goto _LL1DE;_LL1E1:;
_LL1E2:(int)_throw((void*)& Cyc_yyfail_YY51);_LL1DE:;}return yyzzz;}static char
_tmp267[40]="list_t<$(Position::seg_t,qvar_t,bool)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp267,_tmp267,_tmp267 + 40}};struct Cyc_List_List*
Cyc_yyget_YY52(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp268=yy1;struct Cyc_List_List*_tmp269;_LL1E4: if((_tmp268.YY52).tag != 56)goto
_LL1E6;_tmp269=(_tmp268.YY52).f1;_LL1E5: yyzzz=_tmp269;goto _LL1E3;_LL1E6:;_LL1E7:(
int)_throw((void*)& Cyc_yyfail_YY52);_LL1E3:;}return yyzzz;}static char _tmp26B[13]="$(bool,bool)";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp26B,
_tmp26B,_tmp26B + 13}};struct _tuple18 Cyc_yyget_YY53(union Cyc_YYSTYPE_union yy1){
struct _tuple18 yyzzz;{union Cyc_YYSTYPE_union _tmp26C=yy1;struct _tuple18 _tmp26D;
_LL1E9: if((_tmp26C.YY53).tag != 57)goto _LL1EB;_tmp26D=(_tmp26C.YY53).f1;_LL1EA:
yyzzz=_tmp26D;goto _LL1E8;_LL1EB:;_LL1EC:(int)_throw((void*)& Cyc_yyfail_YY53);
_LL1E8:;}return yyzzz;}struct Cyc_Yyltype{int timestamp;int first_line;int
first_column;int last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp26E;_tmp26E.timestamp=0;_tmp26E.first_line=0;
_tmp26E.first_column=0;_tmp26E.last_line=0;_tmp26E.last_column=0;_tmp26E;});}
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[366]={0,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,137,2,2,121,135,132,2,
118,119,126,129,114,133,123,134,2,2,2,2,2,2,2,2,2,2,122,111,116,115,117,128,127,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,124,2,125,131,120,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,112,130,113,136,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,
58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,
85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,
109,110};static char _tmp26F[2]="$";static char _tmp270[6]="error";static char _tmp271[
12]="$undefined.";static char _tmp272[5]="AUTO";static char _tmp273[9]="REGISTER";
static char _tmp274[7]="STATIC";static char _tmp275[7]="EXTERN";static char _tmp276[8]="TYPEDEF";
static char _tmp277[5]="VOID";static char _tmp278[5]="CHAR";static char _tmp279[6]="SHORT";
static char _tmp27A[4]="INT";static char _tmp27B[5]="LONG";static char _tmp27C[6]="FLOAT";
static char _tmp27D[7]="DOUBLE";static char _tmp27E[7]="SIGNED";static char _tmp27F[9]="UNSIGNED";
static char _tmp280[6]="CONST";static char _tmp281[9]="VOLATILE";static char _tmp282[9]="RESTRICT";
static char _tmp283[7]="STRUCT";static char _tmp284[6]="UNION";static char _tmp285[5]="CASE";
static char _tmp286[8]="DEFAULT";static char _tmp287[7]="INLINE";static char _tmp288[7]="SIZEOF";
static char _tmp289[9]="OFFSETOF";static char _tmp28A[3]="IF";static char _tmp28B[5]="ELSE";
static char _tmp28C[7]="SWITCH";static char _tmp28D[6]="WHILE";static char _tmp28E[3]="DO";
static char _tmp28F[4]="FOR";static char _tmp290[5]="GOTO";static char _tmp291[9]="CONTINUE";
static char _tmp292[6]="BREAK";static char _tmp293[7]="RETURN";static char _tmp294[5]="ENUM";
static char _tmp295[8]="NULL_kw";static char _tmp296[4]="LET";static char _tmp297[6]="THROW";
static char _tmp298[4]="TRY";static char _tmp299[6]="CATCH";static char _tmp29A[7]="EXPORT";
static char _tmp29B[4]="NEW";static char _tmp29C[9]="ABSTRACT";static char _tmp29D[9]="FALLTHRU";
static char _tmp29E[6]="USING";static char _tmp29F[10]="NAMESPACE";static char _tmp2A0[
9]="DATATYPE";static char _tmp2A1[8]="XTUNION";static char _tmp2A2[7]="TUNION";
static char _tmp2A3[7]="MALLOC";static char _tmp2A4[8]="RMALLOC";static char _tmp2A5[7]="CALLOC";
static char _tmp2A6[8]="RCALLOC";static char _tmp2A7[5]="SWAP";static char _tmp2A8[9]="REGION_T";
static char _tmp2A9[6]="TAG_T";static char _tmp2AA[7]="REGION";static char _tmp2AB[5]="RNEW";
static char _tmp2AC[8]="REGIONS";static char _tmp2AD[13]="RESET_REGION";static char
_tmp2AE[4]="GEN";static char _tmp2AF[16]="NOZEROTERM_QUAL";static char _tmp2B0[14]="ZEROTERM_QUAL";
static char _tmp2B1[7]="PORTON";static char _tmp2B2[8]="PORTOFF";static char _tmp2B3[8]="FLAT_kw";
static char _tmp2B4[12]="DYNREGION_T";static char _tmp2B5[6]="ALIAS";static char
_tmp2B6[8]="NUMELTS";static char _tmp2B7[8]="VALUEOF";static char _tmp2B8[10]="VALUEOF_T";
static char _tmp2B9[12]="TAGGED_QUAL";static char _tmp2BA[16]="EXTENSIBLE_QUAL";
static char _tmp2BB[15]="RESETABLE_QUAL";static char _tmp2BC[7]="PTR_OP";static char
_tmp2BD[7]="INC_OP";static char _tmp2BE[7]="DEC_OP";static char _tmp2BF[8]="LEFT_OP";
static char _tmp2C0[9]="RIGHT_OP";static char _tmp2C1[6]="LE_OP";static char _tmp2C2[6]="GE_OP";
static char _tmp2C3[6]="EQ_OP";static char _tmp2C4[6]="NE_OP";static char _tmp2C5[7]="AND_OP";
static char _tmp2C6[6]="OR_OP";static char _tmp2C7[11]="MUL_ASSIGN";static char
_tmp2C8[11]="DIV_ASSIGN";static char _tmp2C9[11]="MOD_ASSIGN";static char _tmp2CA[11]="ADD_ASSIGN";
static char _tmp2CB[11]="SUB_ASSIGN";static char _tmp2CC[12]="LEFT_ASSIGN";static
char _tmp2CD[13]="RIGHT_ASSIGN";static char _tmp2CE[11]="AND_ASSIGN";static char
_tmp2CF[11]="XOR_ASSIGN";static char _tmp2D0[10]="OR_ASSIGN";static char _tmp2D1[9]="ELLIPSIS";
static char _tmp2D2[11]="LEFT_RIGHT";static char _tmp2D3[12]="COLON_COLON";static
char _tmp2D4[11]="IDENTIFIER";static char _tmp2D5[17]="INTEGER_CONSTANT";static char
_tmp2D6[7]="STRING";static char _tmp2D7[19]="CHARACTER_CONSTANT";static char _tmp2D8[
18]="FLOATING_CONSTANT";static char _tmp2D9[9]="TYPE_VAR";static char _tmp2DA[13]="TYPEDEF_NAME";
static char _tmp2DB[16]="QUAL_IDENTIFIER";static char _tmp2DC[18]="QUAL_TYPEDEF_NAME";
static char _tmp2DD[10]="ATTRIBUTE";static char _tmp2DE[4]="';'";static char _tmp2DF[4]="'{'";
static char _tmp2E0[4]="'}'";static char _tmp2E1[4]="','";static char _tmp2E2[4]="'='";
static char _tmp2E3[4]="'<'";static char _tmp2E4[4]="'>'";static char _tmp2E5[4]="'('";
static char _tmp2E6[4]="')'";static char _tmp2E7[4]="'_'";static char _tmp2E8[4]="'$'";
static char _tmp2E9[4]="':'";static char _tmp2EA[4]="'.'";static char _tmp2EB[4]="'['";
static char _tmp2EC[4]="']'";static char _tmp2ED[4]="'*'";static char _tmp2EE[4]="'@'";
static char _tmp2EF[4]="'?'";static char _tmp2F0[4]="'+'";static char _tmp2F1[4]="'|'";
static char _tmp2F2[4]="'^'";static char _tmp2F3[4]="'&'";static char _tmp2F4[4]="'-'";
static char _tmp2F5[4]="'/'";static char _tmp2F6[4]="'%'";static char _tmp2F7[4]="'~'";
static char _tmp2F8[4]="'!'";static char _tmp2F9[5]="prog";static char _tmp2FA[17]="translation_unit";
static char _tmp2FB[12]="export_list";static char _tmp2FC[19]="export_list_values";
static char _tmp2FD[21]="external_declaration";static char _tmp2FE[20]="function_definition";
static char _tmp2FF[21]="function_definition2";static char _tmp300[13]="using_action";
static char _tmp301[15]="unusing_action";static char _tmp302[17]="namespace_action";
static char _tmp303[19]="unnamespace_action";static char _tmp304[12]="declaration";
static char _tmp305[19]="resetable_qual_opt";static char _tmp306[17]="declaration_list";
static char _tmp307[23]="declaration_specifiers";static char _tmp308[24]="storage_class_specifier";
static char _tmp309[15]="attributes_opt";static char _tmp30A[11]="attributes";static
char _tmp30B[15]="attribute_list";static char _tmp30C[10]="attribute";static char
_tmp30D[15]="type_specifier";static char _tmp30E[25]="type_specifier_notypedef";
static char _tmp30F[5]="kind";static char _tmp310[15]="type_qualifier";static char
_tmp311[15]="enum_specifier";static char _tmp312[11]="enum_field";static char
_tmp313[22]="enum_declaration_list";static char _tmp314[26]="struct_or_union_specifier";
static char _tmp315[16]="type_params_opt";static char _tmp316[16]="struct_or_union";
static char _tmp317[24]="struct_declaration_list";static char _tmp318[25]="struct_declaration_list0";
static char _tmp319[21]="init_declarator_list";static char _tmp31A[22]="init_declarator_list0";
static char _tmp31B[16]="init_declarator";static char _tmp31C[19]="struct_declaration";
static char _tmp31D[25]="specifier_qualifier_list";static char _tmp31E[35]="notypedef_specifier_qualifier_list";
static char _tmp31F[23]="struct_declarator_list";static char _tmp320[24]="struct_declarator_list0";
static char _tmp321[18]="struct_declarator";static char _tmp322[19]="datatype_specifier";
static char _tmp323[14]="qual_datatype";static char _tmp324[19]="datatypefield_list";
static char _tmp325[20]="datatypefield_scope";static char _tmp326[14]="datatypefield";
static char _tmp327[11]="declarator";static char _tmp328[23]="declarator_withtypedef";
static char _tmp329[18]="direct_declarator";static char _tmp32A[30]="direct_declarator_withtypedef";
static char _tmp32B[8]="pointer";static char _tmp32C[12]="one_pointer";static char
_tmp32D[23]="pointer_null_and_bound";static char _tmp32E[14]="pointer_bound";
static char _tmp32F[18]="zeroterm_qual_opt";static char _tmp330[12]="opt_rgn_opt";
static char _tmp331[8]="rgn_opt";static char _tmp332[11]="tqual_list";static char
_tmp333[20]="parameter_type_list";static char _tmp334[9]="type_var";static char
_tmp335[16]="optional_effect";static char _tmp336[19]="optional_rgn_order";static
char _tmp337[10]="rgn_order";static char _tmp338[16]="optional_inject";static char
_tmp339[11]="effect_set";static char _tmp33A[14]="atomic_effect";static char _tmp33B[
11]="region_set";static char _tmp33C[15]="parameter_list";static char _tmp33D[22]="parameter_declaration";
static char _tmp33E[16]="identifier_list";static char _tmp33F[17]="identifier_list0";
static char _tmp340[12]="initializer";static char _tmp341[18]="array_initializer";
static char _tmp342[17]="initializer_list";static char _tmp343[12]="designation";
static char _tmp344[16]="designator_list";static char _tmp345[11]="designator";
static char _tmp346[10]="type_name";static char _tmp347[14]="any_type_name";static
char _tmp348[15]="type_name_list";static char _tmp349[20]="abstract_declarator";
static char _tmp34A[27]="direct_abstract_declarator";static char _tmp34B[10]="statement";
static char _tmp34C[13]="open_exp_opt";static char _tmp34D[18]="labeled_statement";
static char _tmp34E[21]="expression_statement";static char _tmp34F[19]="compound_statement";
static char _tmp350[16]="block_item_list";static char _tmp351[20]="selection_statement";
static char _tmp352[15]="switch_clauses";static char _tmp353[20]="iteration_statement";
static char _tmp354[15]="jump_statement";static char _tmp355[12]="exp_pattern";
static char _tmp356[20]="conditional_pattern";static char _tmp357[19]="logical_or_pattern";
static char _tmp358[20]="logical_and_pattern";static char _tmp359[21]="inclusive_or_pattern";
static char _tmp35A[21]="exclusive_or_pattern";static char _tmp35B[12]="and_pattern";
static char _tmp35C[17]="equality_pattern";static char _tmp35D[19]="relational_pattern";
static char _tmp35E[14]="shift_pattern";static char _tmp35F[17]="additive_pattern";
static char _tmp360[23]="multiplicative_pattern";static char _tmp361[13]="cast_pattern";
static char _tmp362[14]="unary_pattern";static char _tmp363[16]="postfix_pattern";
static char _tmp364[16]="primary_pattern";static char _tmp365[8]="pattern";static
char _tmp366[19]="tuple_pattern_list";static char _tmp367[20]="tuple_pattern_list0";
static char _tmp368[14]="field_pattern";static char _tmp369[19]="field_pattern_list";
static char _tmp36A[20]="field_pattern_list0";static char _tmp36B[11]="expression";
static char _tmp36C[22]="assignment_expression";static char _tmp36D[20]="assignment_operator";
static char _tmp36E[23]="conditional_expression";static char _tmp36F[20]="constant_expression";
static char _tmp370[22]="logical_or_expression";static char _tmp371[23]="logical_and_expression";
static char _tmp372[24]="inclusive_or_expression";static char _tmp373[24]="exclusive_or_expression";
static char _tmp374[15]="and_expression";static char _tmp375[20]="equality_expression";
static char _tmp376[22]="relational_expression";static char _tmp377[17]="shift_expression";
static char _tmp378[20]="additive_expression";static char _tmp379[26]="multiplicative_expression";
static char _tmp37A[16]="cast_expression";static char _tmp37B[17]="unary_expression";
static char _tmp37C[15]="unary_operator";static char _tmp37D[19]="postfix_expression";
static char _tmp37E[19]="primary_expression";static char _tmp37F[25]="argument_expression_list";
static char _tmp380[26]="argument_expression_list0";static char _tmp381[9]="constant";
static char _tmp382[20]="qual_opt_identifier";static char _tmp383[17]="qual_opt_typedef";
static char _tmp384[18]="struct_union_name";static char _tmp385[11]="field_name";
static char _tmp386[12]="right_angle";static struct _dyneither_ptr Cyc_yytname[280]={{
_tmp26F,_tmp26F,_tmp26F + 2},{_tmp270,_tmp270,_tmp270 + 6},{_tmp271,_tmp271,
_tmp271 + 12},{_tmp272,_tmp272,_tmp272 + 5},{_tmp273,_tmp273,_tmp273 + 9},{_tmp274,
_tmp274,_tmp274 + 7},{_tmp275,_tmp275,_tmp275 + 7},{_tmp276,_tmp276,_tmp276 + 8},{
_tmp277,_tmp277,_tmp277 + 5},{_tmp278,_tmp278,_tmp278 + 5},{_tmp279,_tmp279,
_tmp279 + 6},{_tmp27A,_tmp27A,_tmp27A + 4},{_tmp27B,_tmp27B,_tmp27B + 5},{_tmp27C,
_tmp27C,_tmp27C + 6},{_tmp27D,_tmp27D,_tmp27D + 7},{_tmp27E,_tmp27E,_tmp27E + 7},{
_tmp27F,_tmp27F,_tmp27F + 9},{_tmp280,_tmp280,_tmp280 + 6},{_tmp281,_tmp281,
_tmp281 + 9},{_tmp282,_tmp282,_tmp282 + 9},{_tmp283,_tmp283,_tmp283 + 7},{_tmp284,
_tmp284,_tmp284 + 6},{_tmp285,_tmp285,_tmp285 + 5},{_tmp286,_tmp286,_tmp286 + 8},{
_tmp287,_tmp287,_tmp287 + 7},{_tmp288,_tmp288,_tmp288 + 7},{_tmp289,_tmp289,
_tmp289 + 9},{_tmp28A,_tmp28A,_tmp28A + 3},{_tmp28B,_tmp28B,_tmp28B + 5},{_tmp28C,
_tmp28C,_tmp28C + 7},{_tmp28D,_tmp28D,_tmp28D + 6},{_tmp28E,_tmp28E,_tmp28E + 3},{
_tmp28F,_tmp28F,_tmp28F + 4},{_tmp290,_tmp290,_tmp290 + 5},{_tmp291,_tmp291,
_tmp291 + 9},{_tmp292,_tmp292,_tmp292 + 6},{_tmp293,_tmp293,_tmp293 + 7},{_tmp294,
_tmp294,_tmp294 + 5},{_tmp295,_tmp295,_tmp295 + 8},{_tmp296,_tmp296,_tmp296 + 4},{
_tmp297,_tmp297,_tmp297 + 6},{_tmp298,_tmp298,_tmp298 + 4},{_tmp299,_tmp299,
_tmp299 + 6},{_tmp29A,_tmp29A,_tmp29A + 7},{_tmp29B,_tmp29B,_tmp29B + 4},{_tmp29C,
_tmp29C,_tmp29C + 9},{_tmp29D,_tmp29D,_tmp29D + 9},{_tmp29E,_tmp29E,_tmp29E + 6},{
_tmp29F,_tmp29F,_tmp29F + 10},{_tmp2A0,_tmp2A0,_tmp2A0 + 9},{_tmp2A1,_tmp2A1,
_tmp2A1 + 8},{_tmp2A2,_tmp2A2,_tmp2A2 + 7},{_tmp2A3,_tmp2A3,_tmp2A3 + 7},{_tmp2A4,
_tmp2A4,_tmp2A4 + 8},{_tmp2A5,_tmp2A5,_tmp2A5 + 7},{_tmp2A6,_tmp2A6,_tmp2A6 + 8},{
_tmp2A7,_tmp2A7,_tmp2A7 + 5},{_tmp2A8,_tmp2A8,_tmp2A8 + 9},{_tmp2A9,_tmp2A9,
_tmp2A9 + 6},{_tmp2AA,_tmp2AA,_tmp2AA + 7},{_tmp2AB,_tmp2AB,_tmp2AB + 5},{_tmp2AC,
_tmp2AC,_tmp2AC + 8},{_tmp2AD,_tmp2AD,_tmp2AD + 13},{_tmp2AE,_tmp2AE,_tmp2AE + 4},{
_tmp2AF,_tmp2AF,_tmp2AF + 16},{_tmp2B0,_tmp2B0,_tmp2B0 + 14},{_tmp2B1,_tmp2B1,
_tmp2B1 + 7},{_tmp2B2,_tmp2B2,_tmp2B2 + 8},{_tmp2B3,_tmp2B3,_tmp2B3 + 8},{_tmp2B4,
_tmp2B4,_tmp2B4 + 12},{_tmp2B5,_tmp2B5,_tmp2B5 + 6},{_tmp2B6,_tmp2B6,_tmp2B6 + 8},{
_tmp2B7,_tmp2B7,_tmp2B7 + 8},{_tmp2B8,_tmp2B8,_tmp2B8 + 10},{_tmp2B9,_tmp2B9,
_tmp2B9 + 12},{_tmp2BA,_tmp2BA,_tmp2BA + 16},{_tmp2BB,_tmp2BB,_tmp2BB + 15},{
_tmp2BC,_tmp2BC,_tmp2BC + 7},{_tmp2BD,_tmp2BD,_tmp2BD + 7},{_tmp2BE,_tmp2BE,
_tmp2BE + 7},{_tmp2BF,_tmp2BF,_tmp2BF + 8},{_tmp2C0,_tmp2C0,_tmp2C0 + 9},{_tmp2C1,
_tmp2C1,_tmp2C1 + 6},{_tmp2C2,_tmp2C2,_tmp2C2 + 6},{_tmp2C3,_tmp2C3,_tmp2C3 + 6},{
_tmp2C4,_tmp2C4,_tmp2C4 + 6},{_tmp2C5,_tmp2C5,_tmp2C5 + 7},{_tmp2C6,_tmp2C6,
_tmp2C6 + 6},{_tmp2C7,_tmp2C7,_tmp2C7 + 11},{_tmp2C8,_tmp2C8,_tmp2C8 + 11},{_tmp2C9,
_tmp2C9,_tmp2C9 + 11},{_tmp2CA,_tmp2CA,_tmp2CA + 11},{_tmp2CB,_tmp2CB,_tmp2CB + 11},{
_tmp2CC,_tmp2CC,_tmp2CC + 12},{_tmp2CD,_tmp2CD,_tmp2CD + 13},{_tmp2CE,_tmp2CE,
_tmp2CE + 11},{_tmp2CF,_tmp2CF,_tmp2CF + 11},{_tmp2D0,_tmp2D0,_tmp2D0 + 10},{
_tmp2D1,_tmp2D1,_tmp2D1 + 9},{_tmp2D2,_tmp2D2,_tmp2D2 + 11},{_tmp2D3,_tmp2D3,
_tmp2D3 + 12},{_tmp2D4,_tmp2D4,_tmp2D4 + 11},{_tmp2D5,_tmp2D5,_tmp2D5 + 17},{
_tmp2D6,_tmp2D6,_tmp2D6 + 7},{_tmp2D7,_tmp2D7,_tmp2D7 + 19},{_tmp2D8,_tmp2D8,
_tmp2D8 + 18},{_tmp2D9,_tmp2D9,_tmp2D9 + 9},{_tmp2DA,_tmp2DA,_tmp2DA + 13},{_tmp2DB,
_tmp2DB,_tmp2DB + 16},{_tmp2DC,_tmp2DC,_tmp2DC + 18},{_tmp2DD,_tmp2DD,_tmp2DD + 10},{
_tmp2DE,_tmp2DE,_tmp2DE + 4},{_tmp2DF,_tmp2DF,_tmp2DF + 4},{_tmp2E0,_tmp2E0,
_tmp2E0 + 4},{_tmp2E1,_tmp2E1,_tmp2E1 + 4},{_tmp2E2,_tmp2E2,_tmp2E2 + 4},{_tmp2E3,
_tmp2E3,_tmp2E3 + 4},{_tmp2E4,_tmp2E4,_tmp2E4 + 4},{_tmp2E5,_tmp2E5,_tmp2E5 + 4},{
_tmp2E6,_tmp2E6,_tmp2E6 + 4},{_tmp2E7,_tmp2E7,_tmp2E7 + 4},{_tmp2E8,_tmp2E8,
_tmp2E8 + 4},{_tmp2E9,_tmp2E9,_tmp2E9 + 4},{_tmp2EA,_tmp2EA,_tmp2EA + 4},{_tmp2EB,
_tmp2EB,_tmp2EB + 4},{_tmp2EC,_tmp2EC,_tmp2EC + 4},{_tmp2ED,_tmp2ED,_tmp2ED + 4},{
_tmp2EE,_tmp2EE,_tmp2EE + 4},{_tmp2EF,_tmp2EF,_tmp2EF + 4},{_tmp2F0,_tmp2F0,
_tmp2F0 + 4},{_tmp2F1,_tmp2F1,_tmp2F1 + 4},{_tmp2F2,_tmp2F2,_tmp2F2 + 4},{_tmp2F3,
_tmp2F3,_tmp2F3 + 4},{_tmp2F4,_tmp2F4,_tmp2F4 + 4},{_tmp2F5,_tmp2F5,_tmp2F5 + 4},{
_tmp2F6,_tmp2F6,_tmp2F6 + 4},{_tmp2F7,_tmp2F7,_tmp2F7 + 4},{_tmp2F8,_tmp2F8,
_tmp2F8 + 4},{_tmp2F9,_tmp2F9,_tmp2F9 + 5},{_tmp2FA,_tmp2FA,_tmp2FA + 17},{_tmp2FB,
_tmp2FB,_tmp2FB + 12},{_tmp2FC,_tmp2FC,_tmp2FC + 19},{_tmp2FD,_tmp2FD,_tmp2FD + 21},{
_tmp2FE,_tmp2FE,_tmp2FE + 20},{_tmp2FF,_tmp2FF,_tmp2FF + 21},{_tmp300,_tmp300,
_tmp300 + 13},{_tmp301,_tmp301,_tmp301 + 15},{_tmp302,_tmp302,_tmp302 + 17},{
_tmp303,_tmp303,_tmp303 + 19},{_tmp304,_tmp304,_tmp304 + 12},{_tmp305,_tmp305,
_tmp305 + 19},{_tmp306,_tmp306,_tmp306 + 17},{_tmp307,_tmp307,_tmp307 + 23},{
_tmp308,_tmp308,_tmp308 + 24},{_tmp309,_tmp309,_tmp309 + 15},{_tmp30A,_tmp30A,
_tmp30A + 11},{_tmp30B,_tmp30B,_tmp30B + 15},{_tmp30C,_tmp30C,_tmp30C + 10},{
_tmp30D,_tmp30D,_tmp30D + 15},{_tmp30E,_tmp30E,_tmp30E + 25},{_tmp30F,_tmp30F,
_tmp30F + 5},{_tmp310,_tmp310,_tmp310 + 15},{_tmp311,_tmp311,_tmp311 + 15},{_tmp312,
_tmp312,_tmp312 + 11},{_tmp313,_tmp313,_tmp313 + 22},{_tmp314,_tmp314,_tmp314 + 26},{
_tmp315,_tmp315,_tmp315 + 16},{_tmp316,_tmp316,_tmp316 + 16},{_tmp317,_tmp317,
_tmp317 + 24},{_tmp318,_tmp318,_tmp318 + 25},{_tmp319,_tmp319,_tmp319 + 21},{
_tmp31A,_tmp31A,_tmp31A + 22},{_tmp31B,_tmp31B,_tmp31B + 16},{_tmp31C,_tmp31C,
_tmp31C + 19},{_tmp31D,_tmp31D,_tmp31D + 25},{_tmp31E,_tmp31E,_tmp31E + 35},{
_tmp31F,_tmp31F,_tmp31F + 23},{_tmp320,_tmp320,_tmp320 + 24},{_tmp321,_tmp321,
_tmp321 + 18},{_tmp322,_tmp322,_tmp322 + 19},{_tmp323,_tmp323,_tmp323 + 14},{
_tmp324,_tmp324,_tmp324 + 19},{_tmp325,_tmp325,_tmp325 + 20},{_tmp326,_tmp326,
_tmp326 + 14},{_tmp327,_tmp327,_tmp327 + 11},{_tmp328,_tmp328,_tmp328 + 23},{
_tmp329,_tmp329,_tmp329 + 18},{_tmp32A,_tmp32A,_tmp32A + 30},{_tmp32B,_tmp32B,
_tmp32B + 8},{_tmp32C,_tmp32C,_tmp32C + 12},{_tmp32D,_tmp32D,_tmp32D + 23},{_tmp32E,
_tmp32E,_tmp32E + 14},{_tmp32F,_tmp32F,_tmp32F + 18},{_tmp330,_tmp330,_tmp330 + 12},{
_tmp331,_tmp331,_tmp331 + 8},{_tmp332,_tmp332,_tmp332 + 11},{_tmp333,_tmp333,
_tmp333 + 20},{_tmp334,_tmp334,_tmp334 + 9},{_tmp335,_tmp335,_tmp335 + 16},{_tmp336,
_tmp336,_tmp336 + 19},{_tmp337,_tmp337,_tmp337 + 10},{_tmp338,_tmp338,_tmp338 + 16},{
_tmp339,_tmp339,_tmp339 + 11},{_tmp33A,_tmp33A,_tmp33A + 14},{_tmp33B,_tmp33B,
_tmp33B + 11},{_tmp33C,_tmp33C,_tmp33C + 15},{_tmp33D,_tmp33D,_tmp33D + 22},{
_tmp33E,_tmp33E,_tmp33E + 16},{_tmp33F,_tmp33F,_tmp33F + 17},{_tmp340,_tmp340,
_tmp340 + 12},{_tmp341,_tmp341,_tmp341 + 18},{_tmp342,_tmp342,_tmp342 + 17},{
_tmp343,_tmp343,_tmp343 + 12},{_tmp344,_tmp344,_tmp344 + 16},{_tmp345,_tmp345,
_tmp345 + 11},{_tmp346,_tmp346,_tmp346 + 10},{_tmp347,_tmp347,_tmp347 + 14},{
_tmp348,_tmp348,_tmp348 + 15},{_tmp349,_tmp349,_tmp349 + 20},{_tmp34A,_tmp34A,
_tmp34A + 27},{_tmp34B,_tmp34B,_tmp34B + 10},{_tmp34C,_tmp34C,_tmp34C + 13},{
_tmp34D,_tmp34D,_tmp34D + 18},{_tmp34E,_tmp34E,_tmp34E + 21},{_tmp34F,_tmp34F,
_tmp34F + 19},{_tmp350,_tmp350,_tmp350 + 16},{_tmp351,_tmp351,_tmp351 + 20},{
_tmp352,_tmp352,_tmp352 + 15},{_tmp353,_tmp353,_tmp353 + 20},{_tmp354,_tmp354,
_tmp354 + 15},{_tmp355,_tmp355,_tmp355 + 12},{_tmp356,_tmp356,_tmp356 + 20},{
_tmp357,_tmp357,_tmp357 + 19},{_tmp358,_tmp358,_tmp358 + 20},{_tmp359,_tmp359,
_tmp359 + 21},{_tmp35A,_tmp35A,_tmp35A + 21},{_tmp35B,_tmp35B,_tmp35B + 12},{
_tmp35C,_tmp35C,_tmp35C + 17},{_tmp35D,_tmp35D,_tmp35D + 19},{_tmp35E,_tmp35E,
_tmp35E + 14},{_tmp35F,_tmp35F,_tmp35F + 17},{_tmp360,_tmp360,_tmp360 + 23},{
_tmp361,_tmp361,_tmp361 + 13},{_tmp362,_tmp362,_tmp362 + 14},{_tmp363,_tmp363,
_tmp363 + 16},{_tmp364,_tmp364,_tmp364 + 16},{_tmp365,_tmp365,_tmp365 + 8},{_tmp366,
_tmp366,_tmp366 + 19},{_tmp367,_tmp367,_tmp367 + 20},{_tmp368,_tmp368,_tmp368 + 14},{
_tmp369,_tmp369,_tmp369 + 19},{_tmp36A,_tmp36A,_tmp36A + 20},{_tmp36B,_tmp36B,
_tmp36B + 11},{_tmp36C,_tmp36C,_tmp36C + 22},{_tmp36D,_tmp36D,_tmp36D + 20},{
_tmp36E,_tmp36E,_tmp36E + 23},{_tmp36F,_tmp36F,_tmp36F + 20},{_tmp370,_tmp370,
_tmp370 + 22},{_tmp371,_tmp371,_tmp371 + 23},{_tmp372,_tmp372,_tmp372 + 24},{
_tmp373,_tmp373,_tmp373 + 24},{_tmp374,_tmp374,_tmp374 + 15},{_tmp375,_tmp375,
_tmp375 + 20},{_tmp376,_tmp376,_tmp376 + 22},{_tmp377,_tmp377,_tmp377 + 17},{
_tmp378,_tmp378,_tmp378 + 20},{_tmp379,_tmp379,_tmp379 + 26},{_tmp37A,_tmp37A,
_tmp37A + 16},{_tmp37B,_tmp37B,_tmp37B + 17},{_tmp37C,_tmp37C,_tmp37C + 15},{
_tmp37D,_tmp37D,_tmp37D + 19},{_tmp37E,_tmp37E,_tmp37E + 19},{_tmp37F,_tmp37F,
_tmp37F + 25},{_tmp380,_tmp380,_tmp380 + 26},{_tmp381,_tmp381,_tmp381 + 9},{_tmp382,
_tmp382,_tmp382 + 20},{_tmp383,_tmp383,_tmp383 + 17},{_tmp384,_tmp384,_tmp384 + 18},{
_tmp385,_tmp385,_tmp385 + 11},{_tmp386,_tmp386,_tmp386 + 12}};static short Cyc_yyr1[
500]={0,138,139,139,139,139,139,139,139,139,139,139,140,140,141,141,142,142,142,
143,143,143,143,144,144,145,146,147,148,149,149,149,149,149,149,149,150,150,151,
151,152,152,152,152,152,152,152,152,152,152,153,153,153,153,153,153,153,154,154,
155,156,156,157,157,157,157,157,157,158,158,159,159,159,159,159,159,159,159,159,
159,159,159,159,159,159,159,159,159,159,159,159,159,160,161,161,161,162,162,162,
163,163,164,164,164,165,165,165,165,166,166,167,167,168,168,169,169,170,171,171,
172,172,173,174,174,174,174,174,174,175,175,175,175,175,175,176,177,177,178,178,
178,179,179,179,180,180,180,181,181,181,181,182,182,182,183,183,184,184,185,185,
186,186,186,186,186,186,186,186,186,186,187,187,187,187,187,187,187,187,187,187,
187,188,188,189,190,190,190,191,191,192,192,192,193,193,193,194,194,194,195,195,
196,196,196,196,197,197,198,198,199,199,200,200,201,201,202,202,203,203,203,203,
204,204,205,205,206,206,206,207,208,208,209,209,210,210,210,210,211,211,211,211,
212,213,213,214,214,215,215,216,216,216,216,216,217,217,218,218,218,219,219,219,
219,219,219,219,219,219,219,219,220,220,220,220,220,220,220,221,221,222,223,223,
224,224,225,225,225,225,225,225,226,226,226,226,226,226,227,227,227,227,227,227,
228,228,228,228,228,228,228,228,228,228,228,228,228,228,229,229,229,229,229,229,
229,229,230,231,231,232,232,233,233,234,234,235,235,236,236,237,237,237,238,238,
238,238,238,239,239,239,240,240,240,241,241,241,241,242,242,243,243,243,243,243,
243,243,244,245,246,246,246,246,246,246,246,246,246,246,246,246,246,246,247,247,
247,248,248,249,249,250,250,250,251,251,252,252,253,253,253,254,254,254,254,254,
254,254,254,254,254,254,255,255,255,255,255,255,255,256,257,257,258,258,259,259,
260,260,261,261,262,262,262,263,263,263,263,263,264,264,264,265,265,265,266,266,
266,266,267,267,268,268,268,268,268,268,268,268,268,268,268,268,268,268,268,268,
268,268,269,269,269,270,270,270,270,270,270,270,270,270,270,271,271,271,271,271,
271,271,271,271,272,273,273,274,274,274,274,275,275,276,276,277,277,278,278,279,
279};static short Cyc_yyr2[500]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,1,1,2,3,3,4,3,4,
2,1,2,1,2,3,5,3,7,5,8,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,
4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,
9,8,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,3,7,4,6,1,2,
2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,2,2,
1,0,3,0,1,1,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,
2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,
4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,
7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,
1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,1,3,4,4,5,2,2,2,4,4,4,1,3,1,1,3,1,2,1,
3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,
3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,4,4,1,1,1,1,
4,3,4,3,3,2,2,6,7,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static
short Cyc_yydefact[1008]={0,18,50,51,52,53,55,70,73,74,75,76,77,78,79,80,93,94,95,
110,111,46,0,0,56,0,0,143,0,90,0,0,0,0,0,0,0,0,37,490,203,492,491,493,0,0,71,0,
186,186,185,1,0,16,0,0,17,0,0,40,48,42,68,44,81,82,0,83,191,36,155,0,180,188,84,
159,108,54,53,47,0,97,489,0,490,486,487,488,0,363,0,0,0,0,226,0,365,366,25,27,0,
0,0,0,144,0,0,0,0,145,0,0,0,0,0,0,0,183,184,2,0,0,0,0,0,29,0,116,117,119,41,49,
43,45,112,494,495,108,193,0,192,36,38,36,0,19,0,205,0,168,156,181,190,189,194,0,
69,0,54,101,0,99,0,490,372,0,0,0,0,0,0,0,0,0,0,0,108,0,0,0,0,476,0,0,0,0,0,463,
461,462,0,389,391,405,413,415,417,419,421,423,426,431,434,437,441,0,443,464,475,
474,0,373,371,32,0,0,108,0,0,0,126,122,124,244,246,0,0,9,10,0,0,0,108,496,497,
204,92,0,0,160,72,224,0,221,0,3,0,5,0,274,0,30,0,0,36,20,0,113,114,0,107,108,0,0,
0,0,0,0,0,0,0,0,0,0,490,277,279,0,285,281,0,283,267,268,269,0,270,271,272,0,39,
21,119,251,0,211,227,0,0,207,205,0,188,0,196,57,195,0,0,102,98,0,0,367,0,0,0,451,
0,407,441,0,408,409,0,0,0,0,0,0,0,0,0,444,445,36,0,0,0,447,448,446,0,364,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,395,396,397,398,399,400,401,402,403,404,394,0,
449,0,470,471,0,0,0,478,0,108,379,380,0,377,0,228,0,0,0,0,247,219,0,127,132,128,
130,123,125,205,0,253,245,254,499,498,0,86,89,0,87,0,91,0,63,62,0,60,161,205,223,
157,253,225,169,170,0,85,187,26,0,28,0,0,0,0,118,120,230,229,22,104,115,0,0,0,
134,135,137,0,108,0,141,0,0,0,0,0,0,36,0,314,315,316,0,0,318,0,0,0,0,286,282,119,
284,280,278,0,167,212,0,0,0,218,206,213,164,0,0,0,207,166,162,188,197,58,109,0,
103,412,100,96,375,376,0,0,0,231,0,0,235,0,0,0,240,0,0,0,0,0,0,0,0,0,0,0,477,484,
0,483,390,414,0,416,418,420,422,424,425,429,430,427,428,432,433,435,436,438,439,
440,393,392,469,466,0,468,0,0,0,368,0,374,31,386,0,382,387,0,384,369,0,0,248,133,
129,131,0,207,0,188,0,255,0,205,0,266,250,0,0,108,0,0,0,126,0,108,0,205,0,179,
158,222,4,6,0,34,0,0,138,121,0,0,207,108,150,0,0,0,293,0,0,0,0,0,313,317,0,0,0,0,
276,36,23,252,205,0,215,0,0,208,0,165,211,199,163,197,182,0,7,0,450,0,0,243,0,
232,0,236,239,241,455,0,0,0,0,0,459,460,0,0,0,442,480,0,0,467,465,0,0,378,381,
383,370,0,249,220,262,0,256,257,188,0,0,207,188,0,88,0,207,0,0,0,59,61,0,171,0,0,
207,0,188,0,0,0,136,139,112,142,152,151,0,0,146,0,0,0,0,0,0,0,0,0,0,0,0,0,293,
319,0,0,24,207,0,216,214,0,207,0,198,0,8,0,0,0,242,233,237,0,0,0,0,410,411,454,
482,0,485,406,479,481,0,385,388,261,259,265,264,0,258,188,0,112,0,66,64,65,172,
178,175,0,177,173,188,0,33,0,140,153,150,150,287,293,0,0,0,0,0,0,321,322,324,326,
328,330,332,334,337,342,345,348,352,354,361,362,0,36,290,299,0,0,0,0,0,0,0,0,0,0,
320,273,201,217,209,200,205,13,0,0,453,452,0,238,456,0,0,472,0,263,260,35,0,0,
176,174,275,106,0,149,148,0,0,293,0,358,0,0,355,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,356,294,0,301,0,0,0,309,0,0,0,0,0,0,292,0,207,12,14,0,0,0,0,473,105,0,
0,288,289,0,0,0,0,293,295,325,0,327,329,331,333,335,336,340,341,338,339,343,344,
346,347,349,350,351,0,300,302,303,0,311,310,0,305,0,0,0,210,202,15,0,0,0,0,154,
291,357,0,353,296,0,36,304,312,306,307,0,234,457,0,67,0,0,323,293,297,308,0,360,
359,298,458,0,0,0};static short Cyc_yydefgoto[142]={1005,51,655,852,52,53,278,54,
436,55,438,56,57,143,58,59,500,220,422,423,221,62,235,222,64,159,160,65,156,66,
256,257,126,127,128,258,223,403,451,452,453,67,68,726,727,728,69,454,70,427,71,
72,73,117,154,139,305,652,582,74,583,494,645,486,490,491,398,300,243,93,94,516,
444,517,518,519,520,224,293,294,584,409,281,440,282,283,284,285,286,734,287,288,
813,814,815,816,817,818,819,820,821,822,823,824,825,826,827,828,387,388,389,572,
573,574,289,191,375,192,506,193,194,195,196,197,198,199,200,201,202,203,204,205,
206,207,534,535,208,209,76,853,236,413};static short Cyc_yypact[1008]={2616,-
-32768,- -32768,- -32768,- -32768,7,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1159,145,
1117,- -32768,- 15,- 53,- -32768,24,36,59,84,162,113,166,174,303,239,- -32768,- -32768,
208,- -32768,- -32768,- -32768,203,598,234,236,274,274,- -32768,- -32768,2487,- -32768,
133,490,- -32768,358,496,1159,1159,1159,- -32768,1159,- -32768,- -32768,687,- -32768,
73,3118,267,201,254,548,- -32768,- -32768,297,307,325,- -32768,- 15,336,- -32768,3491,
154,- -32768,- -32768,- -32768,5449,- -32768,345,353,3491,364,372,373,- -32768,74,-
-32768,- -32768,3260,3260,2487,2487,- -32768,3260,392,5449,260,- -32768,170,386,410,
387,170,3464,5449,- -32768,- -32768,- -32768,2487,2744,2487,2744,- 62,- -32768,405,
427,- -32768,3044,- -32768,- -32768,- -32768,- -32768,3464,- -32768,- -32768,297,-
-32768,- 15,- -32768,1555,- -32768,3118,496,- -32768,3260,3192,3955,- -32768,267,-
-32768,- -32768,- -32768,82,3260,- -32768,2744,- -32768,451,458,460,- 15,- 39,- -32768,
3491,100,5626,455,5449,5493,461,466,474,528,535,297,559,563,5712,5712,- -32768,
2090,577,5756,5756,5756,- -32768,- -32768,- -32768,- 68,- -32768,- -32768,- 72,611,571,
572,614,543,159,558,224,169,- -32768,1028,5756,104,- 55,- -32768,641,855,654,-
-32768,- -32768,658,5449,297,855,651,126,3464,3532,3464,593,- -32768,72,72,- -32768,
- -32768,61,662,53,297,- -32768,- -32768,- -32768,- -32768,32,656,- -32768,- -32768,
1030,312,- -32768,680,- -32768,692,- -32768,697,719,726,- -32768,410,4045,3118,-
-32768,727,3464,- -32768,922,729,210,725,172,730,3595,731,744,739,741,4135,3595,
225,736,737,- -32768,- -32768,738,1690,1690,496,1690,- -32768,- -32768,- -32768,754,-
-32768,- -32768,- -32768,285,- -32768,- -32768,753,740,1,770,- -32768,107,756,750,318,
758,548,748,- -32768,769,- -32768,1,767,- 15,- -32768,5449,768,- -32768,765,772,2090,
- -32768,3464,- -32768,- -32768,3685,- -32768,797,5449,5449,5449,5449,5449,773,5449,
3464,2090,- -32768,- -32768,1825,766,368,5449,- -32768,- -32768,- -32768,5449,- -32768,
5756,5449,5756,5756,5756,5756,5756,5756,5756,5756,5756,5756,5756,5756,5756,5756,
5756,5756,5756,5449,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,5449,- -32768,170,- -32768,- -32768,4222,170,5449,-
-32768,778,297,- -32768,- -32768,779,783,3491,- -32768,333,5833,781,3260,- -32768,
787,789,- -32768,3532,3532,3532,- -32768,- -32768,2894,4309,232,- -32768,279,- -32768,
- -32768,107,- -32768,- -32768,3260,- -32768,785,- -32768,792,- -32768,788,786,793,-
-32768,2818,- -32768,294,365,- -32768,- -32768,- -32768,3464,- -32768,- -32768,- -32768,
2487,- -32768,2487,807,799,800,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,811,5449,803,809,- -32768,802,315,297,- 15,815,5449,5449,810,819,5449,904,
1960,825,- -32768,- -32768,- -32768,403,901,- -32768,4399,5449,3595,2220,- -32768,-
-32768,3044,- -32768,- -32768,- -32768,3260,- -32768,- -32768,3464,827,238,- -32768,-
-32768,818,- -32768,107,829,3396,750,- -32768,- -32768,548,649,- -32768,- -32768,2358,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,830,836,850,- -32768,170,5449,-
-32768,650,4045,191,- -32768,376,841,844,847,382,3464,388,845,848,858,5582,-
-32768,- -32768,849,860,- -32768,611,- 8,571,572,614,543,159,159,558,558,558,558,
224,224,169,169,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,866,-
-32768,- 67,3260,3865,- -32768,3946,- -32768,- -32768,- -32768,3491,- -32768,- -32768,
873,865,- -32768,- 10,882,- -32768,- -32768,- -32768,- -32768,872,750,874,548,867,279,
3260,3328,4486,- -32768,- -32768,72,879,297,626,880,32,2970,883,297,3260,3192,4576,
- -32768,294,- -32768,- -32768,- -32768,885,- -32768,900,1163,- -32768,- -32768,922,
5449,750,297,734,416,425,5449,749,426,887,4666,4753,415,- -32768,- -32768,894,896,
889,462,- -32768,3118,- -32768,740,899,3260,- -32768,898,107,- -32768,897,- -32768,
219,- -32768,- -32768,649,- -32768,903,- -32768,2487,905,291,902,- -32768,888,- -32768,
1267,- -32768,- -32768,- -32768,- -32768,5449,991,5449,5493,907,- -32768,- -32768,905,
908,3865,- -32768,- -32768,5449,5449,- -32768,- -32768,1,660,- -32768,- -32768,- -32768,
- -32768,5884,- -32768,- -32768,- -32768,909,- -32768,- -32768,548,1,916,750,548,895,-
-32768,5449,750,469,917,919,- -32768,- -32768,920,- -32768,1,923,750,926,548,928,
5449,910,- -32768,- -32768,3464,- -32768,- -32768,- -32768,934,- 15,445,3595,929,932,
957,933,943,3595,5449,4840,515,4927,599,5014,749,- -32768,946,949,- -32768,750,85,
- -32768,- -32768,958,750,3464,- -32768,344,- -32768,947,950,5449,- -32768,- -32768,-
-32768,4045,477,952,960,- -32768,797,- -32768,- -32768,668,- -32768,- -32768,- -32768,
- -32768,3865,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,953,- -32768,548,630,
3464,969,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,961,- -32768,- -32768,548,
489,- -32768,968,- -32768,967,86,734,1059,749,979,5842,974,2220,5756,973,- -32768,-
32,- -32768,1011,971,653,706,229,717,237,213,- -32768,- -32768,- -32768,- -32768,1012,
5756,1825,- -32768,- -32768,511,3595,555,5101,3595,564,5188,5275,634,986,- -32768,-
-32768,- -32768,- -32768,988,- -32768,899,- -32768,990,643,- -32768,- -32768,37,-
-32768,- -32768,3464,1079,- -32768,3775,- -32768,- -32768,- -32768,992,994,- -32768,-
-32768,- -32768,- -32768,3464,- -32768,- -32768,3595,999,749,2090,- -32768,3464,995,-
-32768,1420,5756,5449,5756,5756,5756,5756,5756,5756,5756,5756,5756,5756,5756,
5756,5756,5756,5756,5756,5756,5449,- -32768,- -32768,1004,- -32768,3595,3595,566,-
-32768,3595,3595,573,3595,574,5362,- -32768,107,750,- -32768,- -32768,260,5449,1008,
1010,- -32768,- -32768,1031,590,- -32768,- -32768,1017,1013,1020,5756,749,- -32768,
611,111,571,572,572,543,159,159,558,558,558,558,224,224,169,169,- -32768,- -32768,
- -32768,144,- -32768,- -32768,- -32768,3595,- -32768,- -32768,3595,- -32768,3595,3595,
623,- -32768,- -32768,- -32768,694,1022,3464,1023,- -32768,- -32768,905,354,- -32768,-
-32768,5449,1420,- -32768,- -32768,- -32768,- -32768,3595,- -32768,- -32768,1025,-
-32768,1027,1032,- -32768,749,- -32768,- -32768,1034,- -32768,- -32768,- -32768,-
-32768,1136,1147,- -32768};static short Cyc_yypgoto[142]={- -32768,128,- -32768,226,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 44,- -32768,- 107,20,-
-32768,- -32768,0,552,- -32768,95,- 193,1046,5,- -32768,- -32768,- 132,- -32768,457,
1125,- 682,- -32768,- -32768,- -32768,930,927,- 17,366,- -32768,- -32768,569,- -32768,-
-32768,33,- -32768,- -32768,79,- 153,1116,- 289,66,- -32768,- -32768,1139,62,- -32768,-
-32768,538,- 139,- 54,- 127,- 56,271,544,547,- 409,- 465,- 111,- 382,- 135,- -32768,- 251,-
161,- 531,- 362,- -32768,674,- 147,- 63,- 121,- 170,- 317,57,- -32768,- -32768,- -32768,- 42,
- 262,- -32768,- 7,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
164,977,- -32768,506,- -32768,- -32768,370,536,- -32768,- 137,- 421,- 146,- 334,- 333,-
341,851,- 331,- 275,- 91,- 225,- 223,132,465,468,- -32768,- -32768,- 313,- -32768,177,497,
- 65,- 30,- 370,68};static short Cyc_yytable[6017]={60,136,443,592,242,63,540,558,298,
322,537,561,301,539,140,344,477,478,542,480,299,60,254,643,323,142,63,145,401,
614,312,570,319,684,307,336,137,225,226,249,801,79,229,136,383,112,342,342,99,
420,607,343,60,408,250,884,345,63,682,60,60,60,165,60,63,63,63,560,63,60,149,429,
384,530,63,543,544,166,232,130,131,132,410,133,646,142,39,255,426,144,587,724,
725,42,660,61,885,279,241,290,306,291,60,60,640,866,342,63,63,690,77,587,691,607,
680,483,61,259,411,412,60,60,60,60,113,63,63,63,63,60,241,549,550,421,63,551,552,
129,151,606,100,60,410,60,659,771,63,61,63,144,149,342,101,410,61,61,61,60,61,
924,634,280,63,144,61,397,606,342,487,510,102,511,418,496,505,415,431,504,411,40,
119,377,378,379,528,529,216,95,40,411,412,238,217,138,431,103,721,61,61,- 147,96,
412,304,399,847,404,314,401,401,401,290,104,446,40,412,61,61,61,61,488,315,400,
380,292,61,342,402,381,382,105,227,228,40,983,279,279,61,279,61,396,259,352,353,
489,120,121,39,164,245,246,247,248,61,42,60,165,212,80,342,63,96,545,546,547,548,
- 227,984,663,- 227,96,166,233,600,39,144,354,355,234,60,60,42,60,106,63,63,308,63,
758,109,407,460,279,107,461,414,360,600,416,280,280,763,280,39,361,362,501,664,
428,110,42,731,892,893,505,514,515,339,340,341,45,485,111,464,19,20,455,155,570,
471,313,297,292,576,457,114,60,472,376,- 205,900,63,- 205,96,473,40,894,895,901,
902,61,405,642,593,358,115,280,406,359,489,479,601,39,484,431,498,242,898,41,42,
43,899,850,637,61,61,502,61,44,505,671,48,49,50,146,431,147,116,96,241,44,431,
148,233,757,96,588,482,589,234,342,400,400,400,590,44,402,402,402,241,591,602,
762,603,155,763,241,39,124,604,157,639,628,41,42,43,599,432,605,158,297,61,433,
495,449,397,142,60,638,60,489,649,63,683,63,568,39,710,342,162,613,698,41,42,43,
211,233,994,851,190,241,710,234,699,210,713,60,39,697,715,241,63,407,41,42,43,
213,714,231,241,60,505,712,342,425,63,144,214,532,215,406,342,428,553,554,555,
666,342,75,230,144,763,670,342,60,237,651,239,672,63,767,646,39,857,747,631,455,
251,342,42,81,97,752,98,397,768,762,741,693,45,342,342,61,636,61,601,729,48,49,
50,342,342,252,75,773,730,735,942,943,431,75,939,431,337,941,567,75,804,571,944,
805,61,650,722,135,608,309,609,96,75,905,96,310,241,318,61,311,342,161,748,324,
97,745,399,788,325,392,241,591,789,97,489,342,326,290,260,746,858,39,61,75,75,
122,123,342,42,135,605,125,870,75,995,762,152,153,45,945,946,75,75,75,75,937,48,
49,50,342,837,350,351,342,906,654,317,329,320,320,261,60,356,357,470,75,63,783,
333,334,327,695,787,320,320,320,244,328,75,60,651,144,795,161,63,702,97,677,881,
455,16,17,18,342,320,951,952,393,908,953,954,330,342,455,342,331,455,912,303,962,
337,342,342,419,136,846,965,967,521,338,849,346,525,39,527,347,337,348,432,259,
42,97,44,977,840,405,925,342,97,538,45,406,458,48,49,50,997,920,48,49,50,705,706,
707,686,934,61,935,687,843,241,342,430,724,725,865,989,96,342,917,349,96,342,75,
61,774,562,385,922,390,430,923,780,391,462,930,784,661,662,781,579,580,581,395,
259,732,733,775,776,424,320,75,797,417,793,861,862,756,888,889,806,279,39,445,
890,891,833,434,41,42,43,896,897,134,876,947,948,949,950,435,161,990,342,320,437,
320,320,320,320,320,320,320,320,320,320,320,320,320,320,320,320,320,992,621,622,
60,441,625,439,629,63,873,874,279,447,456,564,459,635,466,996,864,463,465,467,
280,468,571,474,241,476,445,136,475,869,522,523,524,971,489,96,481,253,412,933,
485,493,499,533,492,938,497,536,44,503,507,508,60,344,531,594,97,63,509,97,526,
907,82,563,911,829,566,565,556,575,577,578,280,595,597,596,598,610,96,611,557,39,
618,615,320,533,612,41,42,43,44,430,616,617,430,61,620,623,449,982,624,931,75,
626,75,630,48,49,50,279,83,586,632,882,641,430,644,647,656,657,658,430,386,619,
667,163,85,668,86,87,669,904,42,673,960,961,674,678,963,964,675,966,88,679,89,90,
998,61,689,320,91,809,810,60,681,688,92,40,63,1003,692,696,694,703,82,320,738,
740,708,75,719,711,718,280,736,742,743,744,533,297,749,533,760,751,755,765,676,
759,985,785,800,986,39,987,988,769,770,779,41,42,43,955,956,957,782,790,764,791,
792,449,807,794,83,450,796,999,802,48,49,50,808,704,798,445,831,832,844,163,85,
845,86,87,97,848,42,854,97,981,855,859,867,863,786,860,811,723,89,90,61,868,871,
320,91,363,872,812,875,799,92,187,877,880,188,189,883,430,886,903,918,445,887,
919,921,926,928,834,836,929,839,430,842,932,430,936,959,364,365,366,367,368,369,
370,371,372,373,701,974,975,856,978,39,979,976,980,320,1006,41,42,43,717,991,993,
374,1000,320,1001,1007,425,972,709,1002,75,1004,406,82,48,49,50,533,240,108,2,3,
4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,190,442,21,448,720,97,150,118,754,
970,750,753,665,394,778,22,0,445,541,830,0,0,83,24,766,0,910,27,0,914,916,445,0,
0,772,28,29,84,85,0,86,87,0,803,42,0,32,33,97,0,0,35,36,37,88,0,89,90,0,0,0,0,91,
0,0,0,0,337,92,0,0,135,0,0,940,0,0,0,0,0,0,0,0,39,40,41,0,43,44,41,42,43,958,879,
0,0,320,0,46,47,449,0,0,0,0,0,969,0,48,49,50,167,168,973,320,0,0,0,445,0,0,0,0,0,
82,0,169,0,0,0,170,445,0,0,0,0,0,0,171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,0,0,
0,0,177,178,0,0,0,0,0,179,180,0,0,320,0,320,320,320,320,320,320,320,320,320,320,
320,320,320,320,320,320,320,39,85,181,86,87,0,0,42,0,0,0,321,761,0,0,0,0,182,0,0,
183,0,514,515,0,184,0,0,185,0,445,186,187,320,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,135,0,0,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,732,733,21,167,
168,262,320,263,264,265,266,267,268,269,270,22,82,23,169,271,0,0,170,24,272,0,0,
27,0,0,171,172,173,174,0,28,29,0,175,0,273,176,0,0,0,0,32,33,34,177,178,35,36,37,
38,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,274,85,181,86,87,40,41,42,
43,44,275,141,- 293,0,0,0,0,182,0,46,277,0,0,0,0,184,0,0,185,0,0,186,187,0,0,188,
189,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,167,168,262,0,263,
264,265,266,267,268,269,270,22,82,23,169,271,0,0,170,24,272,0,0,27,0,0,171,172,
173,174,0,28,29,0,175,0,273,176,0,0,0,0,32,33,34,177,178,35,36,37,38,0,179,180,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,274,85,181,86,87,40,41,42,43,44,275,141,
276,0,0,0,0,182,0,46,277,0,0,0,0,184,0,0,185,0,0,186,187,0,0,188,189,2,3,4,78,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,167,168,262,0,263,264,265,266,267,
268,269,270,22,82,23,169,271,0,0,170,24,272,0,0,27,0,0,171,172,173,174,0,28,29,-
36,175,0,273,176,0,0,0,0,32,33,34,177,178,35,36,37,38,0,179,180,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,274,85,181,86,87,40,41,42,43,44,275,141,0,0,0,0,0,182,
0,46,277,0,0,0,0,184,0,0,185,0,0,186,187,0,0,188,189,2,3,4,78,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,21,167,168,262,0,263,264,265,266,267,268,269,270,22,
82,23,169,271,0,0,170,24,272,0,0,27,0,0,171,172,173,174,0,28,29,0,175,0,273,176,
0,0,0,0,32,33,34,177,178,35,36,37,38,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,274,85,181,86,87,40,41,42,43,44,275,141,0,0,0,0,0,182,0,46,277,0,0,0,0,
184,0,0,185,0,0,186,187,0,0,188,189,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,21,167,168,0,0,0,0,0,0,0,0,0,0,22,82,23,169,0,0,0,170,24,0,0,0,27,0,0,
171,172,173,174,0,28,29,0,175,0,0,176,0,0,0,0,32,33,34,177,178,35,36,37,38,0,179,
180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,40,41,42,43,44,627,
0,0,0,0,0,0,182,0,46,277,0,0,0,0,184,0,0,185,0,0,186,187,0,0,188,189,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,0,167,168,0,0,0,0,0,0,0,0,0,0,22,82,0,169,0,0,0,
170,0,0,0,0,27,0,0,171,172,173,174,0,28,29,0,175,0,0,176,0,0,0,0,32,33,0,177,178,
35,36,37,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,
40,41,42,43,44,0,335,0,0,0,0,0,182,0,46,277,0,0,0,0,184,0,0,185,0,0,186,187,0,0,
188,189,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,167,168,0,0,0,0,0,0,0,0,0,0,
22,82,0,169,0,0,0,170,0,0,0,0,27,0,0,171,172,173,174,0,28,29,0,175,0,0,176,0,0,0,
0,32,33,0,177,178,35,36,37,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
39,85,181,86,87,40,41,42,43,44,0,0,0,0,0,0,0,182,0,46,277,0,0,0,0,184,0,0,185,0,
0,186,187,0,0,188,189,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,653,0,24,0,25,26,27,0,0,0,0,0,0,0,28,
29,- 36,0,0,0,0,0,0,30,31,32,33,34,0,0,35,36,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,39,0,0,0,0,40,41,42,43,44,0,0,- 11,0,0,0,0,45,0,46,47,0,0,0,0,
48,49,50,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,
0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,
0,0,30,31,32,33,34,0,0,35,36,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,39,0,0,0,0,40,41,42,43,44,0,0,- 11,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,- 11,1,0,
2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,
22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,30,31,32,33,
34,0,0,35,36,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,40,
41,42,43,44,0,0,0,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,1,0,2,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,
25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,30,31,32,33,34,0,0,35,36,37,38,0,0,
0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,39,0,0,0,0,40,41,42,43,44,
22,0,- 11,0,0,0,0,45,0,46,47,0,27,0,0,48,49,50,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,
0,0,0,35,36,37,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,295,0,0,39,
0,0,0,0,40,41,42,43,44,297,0,22,0,0,0,0,425,0,46,47,0,0,406,27,48,49,50,0,0,0,0,
28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,36,37,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,295,0,0,0,0,0,0,0,40,41,0,43,44,297,0,22,0,0,0,0,405,0,46,
47,0,0,406,27,48,49,50,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,36,37,0,2,
3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,39,0,0,0,0,40,41,42,
43,44,22,0,23,0,0,0,0,449,24,46,47,0,27,0,0,48,49,50,0,0,28,29,- 36,0,0,0,0,0,0,0,
0,32,33,34,0,0,35,36,37,38,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
21,0,0,0,0,0,0,0,40,41,0,43,44,22,141,23,0,253,0,0,0,24,46,47,0,27,0,0,0,0,0,0,0,
28,29,0,0,0,0,0,0,0,0,0,32,33,34,0,0,35,36,37,38,0,0,0,0,0,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,141,0,0,0,0,0,0,0,46,47,0,
27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,36,37,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,295,0,0,296,0,0,0,22,40,41,0,43,44,297,0,0,
0,0,0,27,0,0,46,47,0,0,0,28,29,0,0,218,0,0,0,0,0,0,32,33,0,0,0,35,36,37,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,40,41,0,43,44,0,
219,0,0,0,0,27,0,0,46,47,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,36,37,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,295,0,0,0,0,0,0,22,40,41,0,43,
44,297,0,0,0,0,0,27,0,0,46,47,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,36,37,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,648,0,0,0,0,0,0,22,40,41,
0,43,44,0,0,0,0,0,0,27,0,0,46,47,0,0,0,28,29,0,0,0,0,0,0,82,0,0,32,33,0,0,0,35,
36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,40,
41,0,43,44,0,0,83,0,0,0,27,0,0,46,47,0,0,0,28,29,0,163,85,0,86,87,0,0,42,32,33,0,
0,0,35,36,37,0,88,0,89,90,0,0,0,0,91,0,0,167,168,262,92,263,264,265,266,267,268,
269,270,0,82,0,169,271,0,40,170,0,272,44,0,0,0,0,171,172,173,174,0,46,47,0,175,0,
273,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,274,85,181,86,87,0,0,42,0,0,275,141,0,0,167,168,0,182,0,0,183,512,0,0,
0,184,0,82,185,169,0,186,187,170,0,188,189,0,0,0,0,171,172,173,174,0,0,0,0,175,0,
0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,39,85,181,86,87,0,0,42,0,0,0,321,513,0,167,168,0,182,0,0,183,0,514,515,0,
184,0,82,185,169,0,186,187,170,0,188,189,0,0,0,0,171,172,173,174,0,0,0,0,175,0,0,
176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,39,85,181,86,87,0,0,42,0,0,0,321,927,0,167,168,0,182,0,0,183,0,514,515,0,
184,0,82,185,169,0,186,187,170,0,188,189,0,0,0,0,171,172,173,174,0,0,0,0,175,0,0,
176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,39,85,181,86,87,0,0,42,0,0,0,321,0,0,167,168,0,182,82,0,183,0,514,515,0,
184,0,82,185,169,0,186,187,170,0,188,189,0,0,0,0,171,172,173,174,0,0,0,0,175,0,0,
176,0,0,0,0,0,0,0,177,178,0,0,0,0,83,179,180,0,0,0,0,0,0,0,0,0,685,0,0,163,85,0,
86,87,0,0,42,0,39,85,181,86,87,0,0,42,88,0,89,90,0,0,167,168,91,182,0,0,183,0,92,
0,302,184,0,82,185,169,0,186,187,170,0,188,189,0,0,0,0,171,172,173,174,0,0,0,0,
175,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,0,321,0,0,167,168,0,182,0,0,183,0,0,0,0,
184,0,82,185,169,0,186,187,170,0,188,189,0,0,0,0,171,172,173,174,0,0,0,0,175,0,0,
176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,39,85,181,86,87,0,0,42,0,0,469,167,168,0,0,0,0,182,0,0,183,0,0,0,82,184,
169,0,185,0,170,186,187,0,0,188,189,0,171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,
0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,
85,181,86,87,0,0,42,0,0,0,167,168,0,0,0,0,182,559,0,183,0,0,0,82,184,169,0,185,0,
170,186,187,0,0,188,189,0,171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,
178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,
0,0,42,0,0,0,0,0,0,167,168,0,182,0,0,183,0,0,0,585,184,0,82,185,169,0,186,187,
170,0,188,189,0,0,0,0,171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,178,
0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,0,0,
42,0,0,0,167,168,0,0,0,0,182,633,0,183,0,0,0,82,184,169,0,185,0,170,186,187,0,0,
188,189,0,171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,
179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,0,0,
0,0,167,168,0,182,0,0,183,0,0,0,700,184,0,82,185,169,0,186,187,170,0,188,189,0,0,
0,0,171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,0,0,0,0,167,
168,0,182,0,0,183,0,0,0,716,184,0,82,185,169,0,186,187,170,0,188,189,0,0,0,0,171,
172,173,174,0,0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,737,167,168,0,0,0,0,
182,0,0,183,0,0,0,82,184,169,0,185,0,170,186,187,0,0,188,189,0,171,172,173,174,0,
0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,739,167,168,0,0,0,0,182,0,0,183,0,
0,0,82,184,169,0,185,0,170,186,187,0,0,188,189,0,171,172,173,174,0,0,0,0,175,0,0,
176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,39,85,181,86,87,0,0,42,0,0,0,167,168,0,0,0,0,182,835,0,183,0,0,0,82,184,
169,0,185,0,170,186,187,0,0,188,189,0,171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,
0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,
85,181,86,87,0,0,42,0,0,0,167,168,0,0,0,0,182,838,0,183,0,0,0,82,184,169,0,185,0,
170,186,187,0,0,188,189,0,171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,
178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,
0,0,42,0,0,841,167,168,0,0,0,0,182,0,0,183,0,0,0,82,184,169,0,185,0,170,186,187,
0,0,188,189,0,171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,
0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,0,
167,168,0,0,0,0,182,909,0,183,0,0,0,82,184,169,0,185,0,170,186,187,0,0,188,189,0,
171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,0,167,168,0,0,0,
0,182,913,0,183,0,0,0,82,184,169,0,185,0,170,186,187,0,0,188,189,0,171,172,173,
174,0,0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,0,167,168,0,0,0,0,182,915,0,
183,0,0,0,82,184,169,0,185,0,170,186,187,0,0,188,189,0,171,172,173,174,0,0,0,0,
175,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,0,167,168,0,0,0,0,182,968,0,183,0,0,0,
82,184,169,0,185,0,170,186,187,0,0,188,189,0,171,172,173,174,0,0,0,0,175,0,0,176,
0,0,0,0,0,167,168,177,178,0,0,0,0,0,179,180,0,0,82,0,0,0,0,0,0,0,0,0,0,0,0,0,171,
172,173,174,0,39,85,181,86,87,0,176,42,0,0,0,0,0,0,177,178,0,182,0,0,183,179,180,
0,0,184,0,0,185,0,0,186,187,0,0,188,189,0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,
0,321,0,167,168,0,0,182,0,0,183,0,0,0,0,184,82,0,185,0,0,186,187,0,0,188,189,0,0,
0,171,172,173,174,0,0,0,0,0,0,0,176,0,0,0,0,0,167,168,177,178,0,0,0,0,0,179,180,
0,0,82,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,173,174,0,39,85,181,86,87,0,176,42,0,0,
0,676,0,0,177,178,0,182,0,0,183,179,180,0,0,184,0,0,185,0,0,186,187,0,0,188,189,
0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,167,168,0,0,0,0,0,316,0,0,183,0,0,82,0,
184,0,0,185,0,0,186,187,0,0,188,189,171,172,173,174,0,0,0,0,0,0,0,176,0,0,0,0,0,
167,168,177,178,0,0,0,0,0,179,180,0,0,82,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,173,
174,0,39,85,181,86,87,0,176,42,0,0,0,0,0,0,177,178,0,332,0,0,183,179,180,0,0,184,
0,0,185,0,0,186,187,0,0,188,189,0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,167,168,
0,0,82,0,0,182,0,0,183,0,0,82,0,184,0,0,185,0,0,186,187,0,0,188,189,171,172,173,
174,0,0,0,0,0,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,83,179,180,82,0,0,0,0,0,0,0,
0,569,0,0,163,85,0,86,87,0,0,42,0,39,85,181,86,87,0,0,42,88,0,89,90,0,514,515,0,
91,878,0,0,183,0,92,0,0,184,0,83,185,0,0,186,187,0,0,188,189,0,0,777,0,0,163,85,
0,86,87,0,0,42,0,0,0,0,0,0,0,0,0,88,0,89,90,0,514,515,0,91,0,0,0,0,0,92};static
short Cyc_yycheck[6017]={0,66,253,412,115,0,347,377,147,170,344,381,147,346,68,87,
278,279,349,281,147,21,129,488,170,69,21,69,221,450,162,393,169,564,155,182,66,
100,101,101,722,21,105,108,99,45,114,114,101,17,432,119,52,223,116,87,128,52,125,
59,60,61,101,63,59,60,61,380,63,69,70,241,127,335,69,350,351,116,108,59,60,61,81,
63,493,129,101,129,241,69,407,5,6,108,515,0,128,141,115,143,154,143,102,103,486,
787,114,102,103,119,103,428,577,495,122,114,21,134,117,129,120,121,122,123,45,
120,121,122,123,129,147,356,357,101,129,358,359,58,72,428,116,141,81,143,514,676,
141,52,143,129,150,114,116,81,59,60,61,157,63,122,473,141,157,143,69,219,455,114,
61,316,111,318,119,300,311,114,241,309,117,106,52,77,78,79,331,332,112,23,106,
117,129,112,118,120,259,111,617,102,103,113,23,129,120,220,119,222,106,400,401,
402,254,49,254,106,129,120,121,122,123,112,120,221,118,144,129,114,221,123,124,
116,102,103,106,122,278,279,141,281,143,113,257,82,83,297,111,112,101,83,120,121,
122,123,157,108,254,101,92,112,114,254,83,352,353,354,355,111,122,518,114,92,116,
101,425,101,254,116,117,107,278,279,108,281,116,278,279,157,281,657,49,223,118,
335,118,121,226,126,449,229,278,279,662,281,101,134,135,305,115,241,100,108,623,
82,83,450,123,124,184,185,186,118,101,118,265,20,21,259,116,689,271,165,111,252,
395,123,100,335,111,205,119,126,335,122,165,118,106,116,117,134,135,254,118,113,
415,129,118,335,124,133,412,280,425,101,294,428,302,476,129,107,108,109,133,753,
479,278,279,307,281,110,515,526,126,127,128,116,449,118,112,210,405,110,455,124,
101,102,217,116,111,118,107,114,400,401,402,124,110,400,401,402,425,409,116,662,
118,116,776,432,101,59,124,112,483,465,107,108,109,425,114,427,103,111,335,119,
114,118,488,479,436,479,438,493,496,436,563,438,111,101,599,114,112,449,589,107,
108,109,101,101,102,113,88,476,613,107,589,118,603,465,101,588,603,486,465,405,
107,108,109,111,603,107,495,479,617,602,114,118,479,465,114,119,115,124,114,425,
360,361,362,119,114,0,106,479,862,119,114,503,118,500,119,119,503,670,919,101,
763,640,111,449,111,114,108,22,23,648,25,577,670,776,111,583,118,114,114,436,475,
438,599,119,126,127,128,114,114,114,45,680,119,119,887,888,613,52,884,616,182,
886,390,58,111,393,889,114,465,499,618,66,436,114,438,390,71,831,393,113,589,118,
479,115,114,80,641,118,83,119,599,114,118,215,603,587,119,92,644,114,118,637,137,
637,119,101,503,102,103,111,112,114,108,108,606,111,119,112,980,862,64,65,118,
890,891,120,121,122,123,883,126,127,128,114,111,84,85,114,119,503,167,176,169,
170,139,637,80,81,270,144,637,699,179,180,118,585,704,184,185,186,116,118,157,
655,651,637,714,162,655,593,165,531,811,599,17,18,19,114,205,896,897,216,119,898,
899,118,114,613,114,118,616,119,148,119,316,114,114,232,755,747,119,119,324,118,
752,86,328,101,330,130,332,131,114,722,108,210,110,119,111,118,859,114,217,345,
118,124,261,126,127,128,984,850,126,127,128,101,102,103,566,878,637,880,570,742,
753,114,241,5,6,111,119,566,114,111,132,570,114,252,655,683,382,112,111,101,259,
114,696,101,263,872,700,113,114,697,400,401,402,118,787,22,23,113,114,119,311,
280,716,117,712,113,114,655,131,132,729,831,101,253,84,85,735,113,107,108,109,80,
81,112,807,892,893,894,895,113,309,113,114,344,113,346,347,348,349,350,351,352,
353,354,355,356,357,358,359,360,361,362,975,459,460,831,106,463,115,465,831,804,
805,883,113,112,385,118,474,101,983,785,118,118,111,831,111,689,118,872,118,321,
923,122,798,325,326,327,920,919,689,113,115,129,877,101,122,125,338,119,883,119,
342,110,113,113,117,883,87,119,101,390,883,117,393,118,835,38,116,838,732,114,
119,363,119,114,113,883,112,119,118,114,101,732,111,375,101,456,111,450,380,117,
107,108,109,110,425,114,122,428,831,112,118,118,937,112,875,436,30,438,111,126,
127,128,984,86,406,42,812,118,449,129,119,119,114,101,455,98,457,114,101,102,114,
104,105,114,830,108,119,908,909,119,119,912,913,113,915,118,114,120,121,984,883,
114,515,126,25,26,984,119,113,132,106,984,997,119,125,119,115,38,531,627,628,119,
503,101,119,118,984,118,112,111,119,473,111,113,476,125,117,112,25,112,116,962,
125,111,965,101,967,968,119,119,119,107,108,109,900,901,902,119,119,667,119,119,
118,112,119,86,122,119,989,113,126,127,128,119,595,125,518,122,113,111,101,102,
111,104,105,566,106,108,119,570,936,119,118,102,119,703,114,118,619,120,121,984,
119,113,617,126,56,118,129,28,718,132,133,112,118,136,137,122,599,86,86,113,564,
130,114,113,25,113,736,737,114,739,613,741,113,616,119,111,88,89,90,91,92,93,94,
95,96,97,590,119,118,759,113,101,119,102,114,670,0,107,108,109,604,119,119,115,
119,680,119,0,118,923,598,119,655,119,124,38,126,127,128,623,114,36,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,811,252,24,257,616,689,71,49,651,919,644,
648,519,217,689,37,- 1,662,348,732,- 1,- 1,86,45,669,- 1,837,49,- 1,840,841,676,- 1,- 1,
679,57,58,101,102,- 1,104,105,- 1,727,108,- 1,68,69,732,- 1,- 1,73,74,75,118,- 1,120,
121,- 1,- 1,- 1,- 1,126,- 1,- 1,- 1,- 1,878,132,- 1,- 1,755,- 1,- 1,885,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,101,106,107,- 1,109,110,107,108,109,903,809,- 1,- 1,812,- 1,120,121,118,- 1,- 1,- 1,
- 1,- 1,917,- 1,126,127,128,25,26,924,830,- 1,- 1,- 1,763,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,-
1,- 1,44,776,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,884,- 1,886,887,888,889,890,891,892,893,
894,895,896,897,898,899,900,901,902,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,112,
113,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,123,124,- 1,126,- 1,- 1,129,- 1,862,132,133,936,- 1,
136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,923,- 1,- 1,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,983,29,30,31,32,33,34,35,36,
37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,63,
- 1,- 1,- 1,- 1,68,69,70,71,72,73,74,75,76,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,106,107,108,109,110,111,112,
113,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,
137,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,
31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,
58,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,71,72,73,74,75,76,- 1,78,79,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,106,107,108,
109,110,111,112,113,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,
132,133,- 1,- 1,136,137,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,
25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,
52,53,54,55,- 1,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,71,72,73,74,75,76,- 1,78,
79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,
104,105,106,107,108,109,110,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,
126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,
45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,71,
72,73,74,75,76,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,101,102,103,104,105,106,107,108,109,110,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,120,
121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,3,4,5,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,
40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,63,- 1,- 1,- 1,
- 1,68,69,70,71,72,73,74,75,76,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,106,107,108,109,110,111,- 1,- 1,- 1,- 1,- 1,
- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,63,- 1,
- 1,- 1,- 1,68,69,- 1,71,72,73,74,75,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,106,107,108,109,110,- 1,112,- 1,- 1,
- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,
63,- 1,- 1,- 1,- 1,68,69,- 1,71,72,73,74,75,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,106,107,108,109,110,- 1,- 1,-
1,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,
137,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,70,- 1,- 1,73,74,75,76,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,106,107,
108,109,110,- 1,- 1,113,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,127,128,0,1,- 1,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,
59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,70,- 1,- 1,73,74,75,76,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,106,107,108,109,110,
- 1,- 1,113,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,127,128,0,1,- 1,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,
- 1,- 1,- 1,66,67,68,69,70,- 1,- 1,73,74,75,76,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,127,128,1,- 1,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,
- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,
68,69,70,- 1,- 1,73,74,75,76,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,106,107,108,109,110,37,- 1,113,- 1,- 1,- 1,- 1,118,
- 1,120,121,- 1,49,- 1,- 1,126,127,128,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,
- 1,- 1,- 1,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,98,- 1,- 1,101,- 1,- 1,- 1,- 1,106,107,108,109,110,111,- 1,37,- 1,- 1,- 1,- 1,118,- 1,120,
121,- 1,- 1,124,49,126,127,128,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,
- 1,- 1,- 1,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,- 1,109,110,111,- 1,37,- 1,- 1,- 1,- 1,118,- 1,120,
121,- 1,- 1,124,49,126,127,128,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,
- 1,- 1,- 1,73,74,75,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,
- 1,- 1,101,- 1,- 1,- 1,- 1,106,107,108,109,110,37,- 1,39,- 1,- 1,- 1,- 1,118,45,120,121,- 1,
49,- 1,- 1,126,127,128,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,73,74,
75,76,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,106,107,- 1,109,110,37,112,39,- 1,115,- 1,- 1,- 1,45,120,121,- 1,49,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,73,74,75,76,- 1,- 1,- 1,- 1,
- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,
- 1,109,110,37,112,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,73,74,75,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,101,- 1,- 1,- 1,37,106,107,- 1,109,110,
111,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,120,121,- 1,- 1,- 1,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,68,
69,- 1,- 1,- 1,73,74,75,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,106,107,- 1,109,110,- 1,112,- 1,- 1,- 1,- 1,49,- 1,- 1,120,
121,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,73,74,75,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,37,
106,107,- 1,109,110,111,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,120,121,- 1,- 1,- 1,57,58,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,73,74,75,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,37,106,107,- 1,109,110,- 1,- 1,- 1,- 1,-
1,- 1,49,- 1,- 1,120,121,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,68,69,- 1,- 1,- 1,
73,74,75,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,37,106,107,- 1,109,110,- 1,- 1,86,- 1,- 1,- 1,49,- 1,- 1,120,121,- 1,- 1,- 1,
57,58,- 1,101,102,- 1,104,105,- 1,- 1,108,68,69,- 1,- 1,- 1,73,74,75,- 1,118,- 1,120,121,
- 1,- 1,- 1,- 1,126,- 1,- 1,25,26,27,132,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,106,
44,- 1,46,110,- 1,- 1,- 1,- 1,52,53,54,55,- 1,120,121,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,25,26,- 1,118,- 1,- 1,
121,32,- 1,- 1,- 1,126,- 1,38,129,40,- 1,132,133,44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,
- 1,108,- 1,- 1,- 1,112,113,- 1,25,26,- 1,118,- 1,- 1,121,- 1,123,124,- 1,126,- 1,38,129,40,
- 1,132,133,44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,-
1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,112,113,- 1,25,26,
- 1,118,- 1,- 1,121,- 1,123,124,- 1,126,- 1,38,129,40,- 1,132,133,44,- 1,136,137,- 1,- 1,-
1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,
- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,
103,104,105,- 1,- 1,108,- 1,- 1,- 1,112,- 1,- 1,25,26,- 1,118,38,- 1,121,- 1,123,124,- 1,
126,- 1,38,129,40,- 1,132,133,44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,86,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,98,- 1,- 1,101,102,- 1,104,105,- 1,- 1,108,- 1,101,102,103,104,105,- 1,- 1,108,118,- 1,
120,121,- 1,- 1,25,26,126,118,- 1,- 1,121,- 1,132,- 1,125,126,- 1,38,129,40,- 1,132,133,
44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,112,- 1,- 1,25,26,- 1,118,- 1,- 1,
121,- 1,- 1,- 1,- 1,126,- 1,38,129,40,- 1,132,133,44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,
- 1,108,- 1,- 1,111,25,26,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,
132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,-
1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,118,119,- 1,
121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,-
1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,
- 1,- 1,- 1,- 1,- 1,25,26,- 1,118,- 1,- 1,121,- 1,- 1,- 1,125,126,- 1,38,129,40,- 1,132,133,
44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,118,119,- 1,
121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,-
1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,
- 1,- 1,- 1,- 1,- 1,25,26,- 1,118,- 1,- 1,121,- 1,- 1,- 1,125,126,- 1,38,129,40,- 1,132,133,
44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,118,- 1,- 1,
121,- 1,- 1,- 1,125,126,- 1,38,129,40,- 1,132,133,44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,
- 1,108,- 1,- 1,111,25,26,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,
132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,-
1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,111,25,26,- 1,- 1,- 1,- 1,118,- 1,- 1,
121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,-
1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,118,119,- 1,121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,-
1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,-
1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,118,119,- 1,121,- 1,- 1,
- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,
- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,111,25,
26,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,
137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,-
1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,
102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,118,119,- 1,121,- 1,- 1,- 1,38,
126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,
- 1,- 1,118,119,- 1,121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,
53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,
105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,118,119,- 1,121,- 1,- 1,- 1,38,126,40,- 1,
129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,
118,119,- 1,121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,25,26,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,
38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,101,102,103,104,105,- 1,
63,108,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,118,- 1,- 1,121,78,79,- 1,- 1,126,- 1,- 1,129,- 1,- 1,
132,133,- 1,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,-
1,112,- 1,25,26,- 1,- 1,118,- 1,- 1,121,- 1,- 1,- 1,- 1,126,38,- 1,129,- 1,- 1,132,133,- 1,- 1,
136,137,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,25,26,71,72,
- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,
- 1,101,102,103,104,105,- 1,63,108,- 1,- 1,- 1,112,- 1,- 1,71,72,- 1,118,- 1,- 1,121,78,79,
- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,
104,105,- 1,- 1,108,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,- 1,38,- 1,126,- 1,- 1,
129,- 1,- 1,132,133,- 1,- 1,136,137,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,- 1,
- 1,25,26,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,52,53,54,55,- 1,101,102,103,104,105,- 1,63,108,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,118,-
1,- 1,121,78,79,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,25,26,- 1,- 1,38,- 1,- 1,118,- 1,- 1,121,- 1,- 1,
38,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,86,78,79,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,
- 1,- 1,101,102,- 1,104,105,- 1,- 1,108,- 1,101,102,103,104,105,- 1,- 1,108,118,- 1,120,
121,- 1,123,124,- 1,126,118,- 1,- 1,121,- 1,132,- 1,- 1,126,- 1,86,129,- 1,- 1,132,133,- 1,
- 1,136,137,- 1,- 1,98,- 1,- 1,101,102,- 1,104,105,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,118,- 1,120,121,- 1,123,124,- 1,126,- 1,- 1,- 1,- 1,- 1,132};char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible\000";
char Cyc_Yystack_overflow[21]="\000\000\000\000Yystack_overflow\000";void Cyc_yyerror(
struct _dyneither_ptr);int Cyc_yylex(struct Cyc_Lexing_lexbuf*);static int Cyc_yychar=(
int)'\000';union Cyc_YYSTYPE_union Cyc_yylval=(union Cyc_YYSTYPE_union)((struct Cyc_YYINITIALSVAL_struct){
58});static int Cyc_yynerrs=0;struct _tuple19{struct Cyc_Position_Segment*f1;struct
_tuple1*f2;int f3;};struct _tuple20{struct _dyneither_ptr f1;void*f2;};static char
_tmp457[8]="stdcall";static char _tmp458[6]="cdecl";static char _tmp459[9]="fastcall";
static char _tmp45A[9]="noreturn";static char _tmp45B[6]="const";static char _tmp45C[8]="aligned";
static char _tmp45D[7]="packed";static char _tmp45E[7]="shared";static char _tmp45F[7]="unused";
static char _tmp460[5]="weak";static char _tmp461[10]="dllimport";static char _tmp462[
10]="dllexport";static char _tmp463[23]="no_instrument_function";static char _tmp464[
12]="constructor";static char _tmp465[11]="destructor";static char _tmp466[22]="no_check_memory_usage";
static char _tmp467[5]="pure";struct _tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){struct _RegionHandle
_tmp388=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp388;
_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmp8C2=10000;short*_tmp8C3=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmp8C2));{unsigned int
_tmp8C4=_tmp8C2;unsigned int i;for(i=0;i < _tmp8C4;i ++){_tmp8C3[i]=(short)0;}}
_tmp8C3;});int yyvsp_offset;union Cyc_YYSTYPE_union*yyvs=(union Cyc_YYSTYPE_union*)({
unsigned int _tmp8BF=10000;union Cyc_YYSTYPE_union*_tmp8C0=(union Cyc_YYSTYPE_union*)
_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE_union),_tmp8BF));{
unsigned int _tmp8C1=_tmp8BF;unsigned int i;for(i=0;i < _tmp8C1;i ++){_tmp8C0[i]=Cyc_yylval;}}
_tmp8C0;});int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({
unsigned int _tmp8BC=10000;struct Cyc_Yyltype*_tmp8BD=(struct Cyc_Yyltype*)
_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp8BC));{
unsigned int _tmp8BE=_tmp8BC;unsigned int i;for(i=0;i < _tmp8BE;i ++){_tmp8BD[i]=Cyc_yynewloc();}}
_tmp8BD;});int yystacksize=10000;union Cyc_YYSTYPE_union yyval=Cyc_yylval;int yylen;
yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=
0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){Cyc_yyerror(({const char*_tmp389="parser stack overflow";_tag_dyneither(
_tmp389,sizeof(char),22);}));(int)_throw((void*)Cyc_Yystack_overflow);}}goto
yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1008,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 365?(int)Cyc_yytranslate[_check_known_subscript_notnull(366,Cyc_yychar)]:
280;}yyn +=yychar1;if((yyn < 0  || yyn > 6016) || Cyc_yycheck[
_check_known_subscript_notnull(6017,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6017,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1007){int
_tmp38A=0;_npop_handler(0);return _tmp38A;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)-- yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1008,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(500,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL1ED: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL1EE: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp38B;_push_handler(& _tmp38B);{int _tmp38D=0;if(setjmp(_tmp38B.handler))_tmp38D=
1;if(!_tmp38D){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp38C=(void*)_exn_thrown;void*
_tmp38F=_tmp38C;_LL1F1: if(*((void**)_tmp38F)!= Cyc_Core_Failure)goto _LL1F3;
_LL1F2: x=0;goto _LL1F0;_LL1F3:;_LL1F4:(void)_throw(_tmp38F);_LL1F0:;}}}{struct
_handler_cons _tmp390;_push_handler(& _tmp390);{int _tmp392=0;if(setjmp(_tmp390.handler))
_tmp392=1;if(!_tmp392){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);;_pop_handler();}else{void*_tmp391=(void*)_exn_thrown;void*
_tmp394=_tmp391;_LL1F6: if(*((void**)_tmp394)!= Cyc_Core_Failure)goto _LL1F8;
_LL1F7: y=0;goto _LL1F5;_LL1F8:;_LL1F9:(void)_throw(_tmp394);_LL1F5:;}}}yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp395;(_tmp395.YY18).tag=22;(
_tmp395.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(x,y);_tmp395;});break;}case 3: _LL1EF: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp396;(_tmp396.YY18).tag=22;(_tmp396.YY18).f1=({struct
Cyc_List_List*_tmp397=_cycalloc(sizeof(*_tmp397));_tmp397->hd=({struct Cyc_Absyn_Decl*
_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp399=_cycalloc(sizeof(*_tmp399));_tmp399[0]=({struct Cyc_Absyn_Using_d_struct
_tmp39A;_tmp39A.tag=11;_tmp39A.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp39A.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp39A;});_tmp399;}));
_tmp398->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp398;});_tmp397->tl=0;_tmp397;});_tmp396;});Cyc_Lex_leave_using();
break;case 4: _LL1FA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp39B;(
_tmp39B.YY18).tag=22;(_tmp39B.YY18).f1=({struct Cyc_List_List*_tmp39C=_cycalloc(
sizeof(*_tmp39C));_tmp39C->hd=({struct Cyc_Absyn_Decl*_tmp39D=_cycalloc(sizeof(*
_tmp39D));_tmp39D->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp39E=
_cycalloc(sizeof(*_tmp39E));_tmp39E[0]=({struct Cyc_Absyn_Using_d_struct _tmp39F;
_tmp39F.tag=11;_tmp39F.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp39F.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp39F;});_tmp39E;}));
_tmp39D->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp39D;});_tmp39C->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp39C;});_tmp39B;});
break;case 5: _LL1FB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3A0;(
_tmp3A0.YY18).tag=22;(_tmp3A0.YY18).f1=({struct Cyc_List_List*_tmp3A1=_cycalloc(
sizeof(*_tmp3A1));_tmp3A1->hd=({struct Cyc_Absyn_Decl*_tmp3A2=_cycalloc(sizeof(*
_tmp3A2));_tmp3A2->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp3A3=
_cycalloc(sizeof(*_tmp3A3));_tmp3A3[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp3A4;_tmp3A4.tag=10;_tmp3A4.f1=({struct _dyneither_ptr*_tmp3A5=_cycalloc(
sizeof(*_tmp3A5));_tmp3A5[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3A5;});_tmp3A4.f2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3A4;});
_tmp3A3;}));_tmp3A2->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp3A2;});
_tmp3A1->tl=0;_tmp3A1;});_tmp3A0;});Cyc_Lex_leave_namespace();break;case 6: _LL1FC: {
struct _dyneither_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp3A6;_push_handler(& _tmp3A6);{int _tmp3A8=0;if(setjmp(_tmp3A6.handler))
_tmp3A8=1;if(!_tmp3A8){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp3A7=(void*)_exn_thrown;void*_tmp3AA=_tmp3A7;_LL1FF: if(*((void**)_tmp3AA)
!= Cyc_Core_Failure)goto _LL201;_LL200: nspace=({const char*_tmp3AB="";
_tag_dyneither(_tmp3AB,sizeof(char),1);});goto _LL1FE;_LL201:;_LL202:(void)_throw(
_tmp3AA);_LL1FE:;}}}{struct _handler_cons _tmp3AC;_push_handler(& _tmp3AC);{int
_tmp3AE=0;if(setjmp(_tmp3AC.handler))_tmp3AE=1;if(!_tmp3AE){x=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}
else{void*_tmp3AD=(void*)_exn_thrown;void*_tmp3B0=_tmp3AD;_LL204: if(*((void**)
_tmp3B0)!= Cyc_Core_Failure)goto _LL206;_LL205: x=0;goto _LL203;_LL206:;_LL207:(
void)_throw(_tmp3B0);_LL203:;}}}{struct _handler_cons _tmp3B1;_push_handler(&
_tmp3B1);{int _tmp3B3=0;if(setjmp(_tmp3B1.handler))_tmp3B3=1;if(!_tmp3B3){y=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp3B2=(void*)_exn_thrown;void*_tmp3B5=_tmp3B2;_LL209: if(*((void**)_tmp3B5)
!= Cyc_Core_Failure)goto _LL20B;_LL20A: y=0;goto _LL208;_LL20B:;_LL20C:(void)_throw(
_tmp3B5);_LL208:;}}}yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3B6;(_tmp3B6.YY18).tag=22;(_tmp3B6.YY18).f1=({struct Cyc_List_List*_tmp3B7=
_cycalloc(sizeof(*_tmp3B7));_tmp3B7->hd=({struct Cyc_Absyn_Decl*_tmp3B8=_cycalloc(
sizeof(*_tmp3B8));_tmp3B8->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp3BA;_tmp3BA.tag=10;_tmp3BA.f1=({struct _dyneither_ptr*_tmp3BB=_cycalloc(
sizeof(*_tmp3BB));_tmp3BB[0]=nspace;_tmp3BB;});_tmp3BA.f2=x;_tmp3BA;});_tmp3B9;}));
_tmp3B8->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp3B8;});_tmp3B7->tl=y;_tmp3B7;});_tmp3B6;});
break;}case 7: _LL1FD: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp3BC="C";
_tag_dyneither(_tmp3BC,sizeof(char),2);}))== 0)yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3BD;(_tmp3BD.YY18).tag=22;(_tmp3BD.YY18).f1=({struct
Cyc_List_List*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->hd=({struct Cyc_Absyn_Decl*
_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp3C1;_tmp3C1.tag=12;_tmp3C1.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3C1;});_tmp3C0;}));
_tmp3BF->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3BF;});_tmp3BE->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3BE;});_tmp3BD;});else{
if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp3C2="C include";
_tag_dyneither(_tmp3C2,sizeof(char),10);}))!= 0)Cyc_Parse_err(({const char*
_tmp3C3="expecting \"C\" or \"C include\"";_tag_dyneither(_tmp3C3,sizeof(char),
29);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).last_line));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3C4;(_tmp3C4.YY18).tag=22;(_tmp3C4.YY18).f1=({struct Cyc_List_List*_tmp3C5=
_cycalloc(sizeof(*_tmp3C5));_tmp3C5->hd=({struct Cyc_Absyn_Decl*_tmp3C6=_cycalloc(
sizeof(*_tmp3C6));_tmp3C6->r=(void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp3C7=_cycalloc(sizeof(*_tmp3C7));_tmp3C7[0]=({struct Cyc_Absyn_ExternCinclude_d_struct
_tmp3C8;_tmp3C8.tag=13;_tmp3C8.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3C8.f2=0;_tmp3C8;});
_tmp3C7;}));_tmp3C6->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3C6;});
_tmp3C5->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3C5;});_tmp3C4;});}break;case 8: _LL20D: if(Cyc_strcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),({const char*_tmp3C9="C include";_tag_dyneither(_tmp3C9,
sizeof(char),10);}))!= 0)Cyc_Parse_err(({const char*_tmp3CA="expecting \"C include\"";
_tag_dyneither(_tmp3CA,sizeof(char),22);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3CB;(_tmp3CB.YY18).tag=
22;(_tmp3CB.YY18).f1=({struct Cyc_List_List*_tmp3CC=_cycalloc(sizeof(*_tmp3CC));
_tmp3CC->hd=({struct Cyc_Absyn_Decl*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));_tmp3CD->r=(
void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*_tmp3CE=_cycalloc(
sizeof(*_tmp3CE));_tmp3CE[0]=({struct Cyc_Absyn_ExternCinclude_d_struct _tmp3CF;
_tmp3CF.tag=13;_tmp3CF.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp3CF.f2=exs;_tmp3CF;});_tmp3CE;}));_tmp3CD->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3CD;});
_tmp3CC->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3CC;});_tmp3CB;});break;}case 9: _LL20E: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3D0;(_tmp3D0.YY18).tag=22;(_tmp3D0.YY18).f1=({struct
Cyc_List_List*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1->hd=({struct Cyc_Absyn_Decl*
_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->r=(void*)((void*)0);_tmp3D2->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3D2;});_tmp3D1->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3D1;});_tmp3D0;});
break;case 10: _LL20F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3D3;(_tmp3D3.YY18).tag=22;(_tmp3D3.YY18).f1=({struct Cyc_List_List*_tmp3D4=
_cycalloc(sizeof(*_tmp3D4));_tmp3D4->hd=({struct Cyc_Absyn_Decl*_tmp3D5=_cycalloc(
sizeof(*_tmp3D5));_tmp3D5->r=(void*)((void*)1);_tmp3D5->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3D5;});
_tmp3D4->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3D4;});_tmp3D3;});break;case 11: _LL210: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3D6;(_tmp3D6.YY18).tag=22;(_tmp3D6.YY18).f1=0;_tmp3D6;});
break;case 12: _LL211: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)];break;case 13: _LL212: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3D7;(_tmp3D7.YY52).tag=56;(_tmp3D7.YY52).f1=0;_tmp3D7;});break;case 14: _LL213:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3D8;(_tmp3D8.YY52).tag=
56;(_tmp3D8.YY52).f1=({struct Cyc_List_List*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));
_tmp3D9->hd=({struct _tuple19*_tmp3DA=_cycalloc(sizeof(*_tmp3DA));_tmp3DA->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3DA->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp3DA->f3=0;_tmp3DA;});_tmp3D9->tl=0;_tmp3D9;});_tmp3D8;});break;case 15: _LL214:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3DB;(_tmp3DB.YY52).tag=
56;(_tmp3DB.YY52).f1=({struct Cyc_List_List*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));
_tmp3DC->hd=({struct _tuple19*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));_tmp3DD->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3DD->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp3DD->f3=0;_tmp3DD;});_tmp3DC->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3DC;});_tmp3DB;});
break;case 16: _LL215: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3DE;(_tmp3DE.YY18).tag=22;(_tmp3DE.YY18).f1=({struct Cyc_List_List*_tmp3DF=
_cycalloc(sizeof(*_tmp3DF));_tmp3DF->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp3E0=_cycalloc(sizeof(*_tmp3E0));_tmp3E0[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp3E1;_tmp3E1.tag=1;_tmp3E1.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3E1;});_tmp3E0;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DF->tl=0;
_tmp3DF;});_tmp3DE;});break;case 17: _LL216: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 18: _LL217: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3E2;(_tmp3E2.YY18).tag=22;(
_tmp3E2.YY18).f1=0;_tmp3E2;});break;case 19: _LL218: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3E3;(_tmp3E3.YY17).tag=21;(_tmp3E3.YY17).f1=Cyc_Parse_make_function(
0,Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E3;});
break;case 20: _LL219: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E4;(_tmp3E4.YY17).tag=21;(_tmp3E4.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));_tmp3E5->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3E5;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E4;});
break;case 21: _LL21A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E6;(_tmp3E6.YY17).tag=21;(_tmp3E6.YY17).f1=Cyc_Parse_make_function(0,Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E6;});
break;case 22: _LL21B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E7;(_tmp3E7.YY17).tag=21;(_tmp3E7.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E8=_cycalloc(sizeof(*_tmp3E8));_tmp3E8->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3E8;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E7;});
break;case 23: _LL21C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E9;(_tmp3E9.YY17).tag=21;(_tmp3E9.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3EA;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E9;});
break;case 24: _LL21D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3EB;(_tmp3EB.YY17).tag=21;(_tmp3EB.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));_tmp3EC->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3EC;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3EB;});
break;case 25: _LL21E: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL21F: Cyc_Lex_leave_using();
break;case 27: _LL220: Cyc_Lex_enter_namespace(({struct _dyneither_ptr*_tmp3ED=
_cycalloc(sizeof(*_tmp3ED));_tmp3ED[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3ED;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL221: Cyc_Lex_leave_namespace();
break;case 29: _LL222: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3EE;(_tmp3EE.YY18).tag=22;(_tmp3EE.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp3EE;});
break;case 30: _LL223: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3EF;(_tmp3EF.YY18).tag=22;(_tmp3EF.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3EF;});
break;case 31: _LL224: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3F0;(_tmp3F0.YY18).tag=22;(_tmp3F0.YY18).f1=({struct Cyc_List_List*_tmp3F1=
_cycalloc(sizeof(*_tmp3F1));_tmp3F1->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3F1->tl=0;
_tmp3F1;});_tmp3F0;});break;case 32: _LL225: {struct Cyc_List_List*_tmp3F2=0;{
struct Cyc_List_List*_tmp3F3=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);for(0;_tmp3F3 != 0;_tmp3F3=_tmp3F3->tl){struct
_dyneither_ptr*_tmp3F4=(struct _dyneither_ptr*)_tmp3F3->hd;struct _tuple1*qv=({
struct _tuple1*_tmp3F7=_cycalloc(sizeof(*_tmp3F7));_tmp3F7->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp3F8;(_tmp3F8.Rel_n).tag=1;(_tmp3F8.Rel_n).f1=0;
_tmp3F8;});_tmp3F7->f2=_tmp3F4;_tmp3F7;});struct Cyc_Absyn_Vardecl*_tmp3F5=Cyc_Absyn_new_vardecl(
qv,Cyc_Absyn_wildtyp(0),0);_tmp3F2=({struct Cyc_List_List*_tmp3F6=_cycalloc(
sizeof(*_tmp3F6));_tmp3F6->hd=_tmp3F5;_tmp3F6->tl=_tmp3F2;_tmp3F6;});}}_tmp3F2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3F2);yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3F9;(_tmp3F9.YY18).tag=22;(
_tmp3F9.YY18).f1=({struct Cyc_List_List*_tmp3FA=_cycalloc(sizeof(*_tmp3FA));
_tmp3FA->hd=Cyc_Absyn_letv_decl(_tmp3F2,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3FA->tl=0;
_tmp3FA;});_tmp3F9;});break;}case 33: _LL226: if(Cyc_zstrcmp((struct _dyneither_ptr)
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({
const char*_tmp3FB="`H";_tag_dyneither(_tmp3FB,sizeof(char),3);}))== 0)Cyc_Parse_err((
struct _dyneither_ptr)({void*_tmp3FC=0;Cyc_aprintf(({const char*_tmp3FD="bad occurrence of heap region";
_tag_dyneither(_tmp3FD,sizeof(char),30);}),_tag_dyneither(_tmp3FC,sizeof(void*),
0));}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp3FE="`U";
_tag_dyneither(_tmp3FE,sizeof(char),3);}))== 0)Cyc_Parse_err((struct
_dyneither_ptr)({void*_tmp3FF=0;Cyc_aprintf(({const char*_tmp400="bad occurrence of unique region";
_tag_dyneither(_tmp400,sizeof(char),32);}),_tag_dyneither(_tmp3FF,sizeof(void*),
0));}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp40A=_cycalloc(sizeof(*_tmp40A));_tmp40A->name=({struct _dyneither_ptr*_tmp40B=
_cycalloc(sizeof(*_tmp40B));_tmp40B[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp40B;});_tmp40A->identity=
- 1;_tmp40A->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3);_tmp40A;});void*t=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp408=_cycalloc(sizeof(*_tmp408));
_tmp408[0]=({struct Cyc_Absyn_VarType_struct _tmp409;_tmp409.tag=1;_tmp409.f1=tv;
_tmp409;});_tmp408;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp403=_cycalloc(sizeof(*_tmp403));_tmp403->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp405;(_tmp405.Loc_n).tag=0;_tmp405;});_tmp403->f2=({
struct _dyneither_ptr*_tmp404=_cycalloc(sizeof(*_tmp404));_tmp404[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp404;});_tmp403;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp406=_cycalloc(sizeof(*_tmp406));
_tmp406[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp407;_tmp407.tag=14;_tmp407.f1=(
void*)t;_tmp407;});_tmp406;}),0);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp401;(_tmp401.YY18).tag=22;(_tmp401.YY18).f1=({struct Cyc_List_List*_tmp402=
_cycalloc(sizeof(*_tmp402));_tmp402->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp402->tl=0;
_tmp402;});_tmp401;});break;}case 34: _LL227: if(Cyc_zstrcmp((struct _dyneither_ptr)
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({
const char*_tmp40C="H";_tag_dyneither(_tmp40C,sizeof(char),2);}))== 0)Cyc_Parse_err(({
const char*_tmp40D="bad occurrence of heap region `H";_tag_dyneither(_tmp40D,
sizeof(char),33);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),({const char*_tmp40E="U";_tag_dyneither(_tmp40E,sizeof(
char),2);}))== 0)Cyc_Parse_err(({const char*_tmp40F="bad occurrence of unique region `U";
_tag_dyneither(_tmp40F,sizeof(char),35);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));if(Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]) && Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)Cyc_Parse_err(({
const char*_tmp410="open regions cannot be @resetable";_tag_dyneither(_tmp410,
sizeof(char),34);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A->name=({
struct _dyneither_ptr*_tmp41B=_cycalloc(sizeof(*_tmp41B));_tmp41B[0]=(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp41E;_tmp41E.tag=0;_tmp41E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{void*_tmp41C[1]={&
_tmp41E};Cyc_aprintf(({const char*_tmp41D="`%s";_tag_dyneither(_tmp41D,sizeof(
char),4);}),_tag_dyneither(_tmp41C,sizeof(void*),1));}});_tmp41B;});_tmp41A->identity=
- 1;_tmp41A->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3);_tmp41A;});void*t=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp418=_cycalloc(sizeof(*_tmp418));
_tmp418[0]=({struct Cyc_Absyn_VarType_struct _tmp419;_tmp419.tag=1;_tmp419.f1=tv;
_tmp419;});_tmp418;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp413=_cycalloc(sizeof(*_tmp413));_tmp413->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp415;(_tmp415.Loc_n).tag=0;_tmp415;});_tmp413->f2=({
struct _dyneither_ptr*_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp414;});_tmp413;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp416=_cycalloc(sizeof(*_tmp416));
_tmp416[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp417;_tmp417.tag=14;_tmp417.f1=(
void*)t;_tmp417;});_tmp416;}),0);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp411;(_tmp411.YY18).tag=22;(_tmp411.YY18).f1=({struct Cyc_List_List*_tmp412=
_cycalloc(sizeof(*_tmp412));_tmp412->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp412->tl=0;
_tmp412;});_tmp411;});break;}case 35: _LL228: {struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp424=_cycalloc(sizeof(*_tmp424));_tmp424->name=({struct _dyneither_ptr*_tmp427=
_cycalloc(sizeof(*_tmp427));_tmp427[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp427;});_tmp424->identity=
- 1;_tmp424->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp425=_cycalloc(
sizeof(*_tmp425));_tmp425[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp426;_tmp426.tag=0;
_tmp426.f1=(void*)((void*)3);_tmp426;});_tmp425;}));_tmp424;});struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp421=_cycalloc(sizeof(*_tmp421));
_tmp421->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp423;(
_tmp423.Loc_n).tag=0;_tmp423;});_tmp421->f2=({struct _dyneither_ptr*_tmp422=
_cycalloc(sizeof(*_tmp422));_tmp422[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp422;});_tmp421;}),(
void*)0,0);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp41F;(
_tmp41F.YY18).tag=22;(_tmp41F.YY18).f1=({struct Cyc_List_List*_tmp420=_cycalloc(
sizeof(*_tmp420));_tmp420->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),tv,vd,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp420->tl=0;
_tmp420;});_tmp41F;});break;}case 36: _LL229: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp428;(_tmp428.YY31).tag=35;(_tmp428.YY31).f1=0;_tmp428;});
break;case 37: _LL22A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp429;(_tmp429.YY31).tag=35;(_tmp429.YY31).f1=1;_tmp429;});break;case 38: _LL22B:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 39:
_LL22C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp42A;(_tmp42A.YY18).tag=
22;(_tmp42A.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp42A;});break;case 40: _LL22D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp42B;(_tmp42B.YY19).tag=23;(_tmp42B.YY19).f1=({struct
Cyc_Parse_Declaration_spec*_tmp42C=_cycalloc(sizeof(*_tmp42C));_tmp42C->sc=({
struct Cyc_Core_Opt*_tmp42D=_cycalloc(sizeof(*_tmp42D));_tmp42D->v=(void*)Cyc_yyget_YY22(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp42D;});_tmp42C->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp42C->type_specs=
0;_tmp42C->is_inline=0;_tmp42C->attributes=0;_tmp42C;});_tmp42B;});break;case 41:
_LL22E: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc
!= 0)({void*_tmp42E=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp42F="Only one storage class is allowed in a declaration";_tag_dyneither(
_tmp42F,sizeof(char),51);}),_tag_dyneither(_tmp42E,sizeof(void*),0));});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp430;(_tmp430.YY19).tag=23;(
_tmp430.YY19).f1=({struct Cyc_Parse_Declaration_spec*_tmp431=_cycalloc(sizeof(*
_tmp431));_tmp431->sc=({struct Cyc_Core_Opt*_tmp432=_cycalloc(sizeof(*_tmp432));
_tmp432->v=(void*)Cyc_yyget_YY22(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp432;});_tmp431->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp431->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp431->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp431->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp431;});
_tmp430;});break;case 42: _LL22F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp433;(_tmp433.YY19).tag=23;(_tmp433.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->sc=0;_tmp434->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp434->type_specs=({struct Cyc_List_List*_tmp435=
_cycalloc(sizeof(*_tmp435));_tmp435->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp435->tl=0;_tmp435;});
_tmp434->is_inline=0;_tmp434->attributes=0;_tmp434;});_tmp433;});break;case 43:
_LL230: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp436;(_tmp436.YY19).tag=
23;(_tmp436.YY19).f1=({struct Cyc_Parse_Declaration_spec*_tmp437=_cycalloc(
sizeof(*_tmp437));_tmp437->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp437->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp437->type_specs=({
struct Cyc_List_List*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->hd=(void*)Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp438->tl=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp438;});
_tmp437->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp437->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp437;});
_tmp436;});break;case 44: _LL231: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp439;(_tmp439.YY19).tag=23;(_tmp439.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->sc=0;_tmp43A->tq=Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp43A->type_specs=0;
_tmp43A->is_inline=0;_tmp43A->attributes=0;_tmp43A;});_tmp439;});break;case 45:
_LL232: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp43B;(_tmp43B.YY19).tag=
23;(_tmp43B.YY19).f1=({struct Cyc_Parse_Declaration_spec*_tmp43C=_cycalloc(
sizeof(*_tmp43C));_tmp43C->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp43C->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);_tmp43C->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp43C->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp43C->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp43C;});
_tmp43B;});break;case 46: _LL233: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp43D;(_tmp43D.YY19).tag=23;(_tmp43D.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp43E=_cycalloc(sizeof(*_tmp43E));_tmp43E->sc=0;_tmp43E->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp43E->type_specs=0;_tmp43E->is_inline=1;_tmp43E->attributes=
0;_tmp43E;});_tmp43D;});break;case 47: _LL234: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp43F;(_tmp43F.YY19).tag=23;(_tmp43F.YY19).f1=({struct
Cyc_Parse_Declaration_spec*_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp440->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp440->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp440->is_inline=1;_tmp440->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp440;});
_tmp43F;});break;case 48: _LL235: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp441;(_tmp441.YY19).tag=23;(_tmp441.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->sc=0;_tmp442->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp442->type_specs=0;_tmp442->is_inline=0;_tmp442->attributes=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp442;});
_tmp441;});break;case 49: _LL236: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp443;(_tmp443.YY19).tag=23;(_tmp443.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp444->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp444->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp444->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp444->attributes=Cyc_List_imp_append(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp444;});
_tmp443;});break;case 50: _LL237: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp445;(_tmp445.YY22).tag=26;(_tmp445.YY22).f1=(void*)((void*)4);_tmp445;});
break;case 51: _LL238: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp446;(_tmp446.YY22).tag=26;(_tmp446.YY22).f1=(void*)((void*)5);_tmp446;});
break;case 52: _LL239: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp447;(_tmp447.YY22).tag=26;(_tmp447.YY22).f1=(void*)((void*)3);_tmp447;});
break;case 53: _LL23A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp448;(_tmp448.YY22).tag=26;(_tmp448.YY22).f1=(void*)((void*)1);_tmp448;});
break;case 54: _LL23B: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp449="C";
_tag_dyneither(_tmp449,sizeof(char),2);}))!= 0)Cyc_Parse_err(({const char*_tmp44A="only extern or extern \"C\" is allowed";
_tag_dyneither(_tmp44A,sizeof(char),37);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp44B;(_tmp44B.YY22).tag=26;(_tmp44B.YY22).f1=(void*)((
void*)2);_tmp44B;});break;case 55: _LL23C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp44C;(_tmp44C.YY22).tag=26;(_tmp44C.YY22).f1=(void*)((void*)0);_tmp44C;});
break;case 56: _LL23D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp44D;(_tmp44D.YY22).tag=26;(_tmp44D.YY22).f1=(void*)((void*)6);_tmp44D;});
break;case 57: _LL23E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp44E;(_tmp44E.YY45).tag=49;(_tmp44E.YY45).f1=0;_tmp44E;});break;case 58: _LL23F:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 59:
_LL240: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 60: _LL241: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp44F;(
_tmp44F.YY45).tag=49;(_tmp44F.YY45).f1=({struct Cyc_List_List*_tmp450=_cycalloc(
sizeof(*_tmp450));_tmp450->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp450->tl=0;_tmp450;});
_tmp44F;});break;case 61: _LL242: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp451;(_tmp451.YY45).tag=49;(_tmp451.YY45).f1=({struct Cyc_List_List*_tmp452=
_cycalloc(sizeof(*_tmp452));_tmp452->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp452->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp452;});_tmp451;});
break;case 62: _LL243: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={1,- 1};
static struct _tuple20 att_map[17]={{{_tmp457,_tmp457,_tmp457 + 8},(void*)0},{{
_tmp458,_tmp458,_tmp458 + 6},(void*)1},{{_tmp459,_tmp459,_tmp459 + 9},(void*)2},{{
_tmp45A,_tmp45A,_tmp45A + 9},(void*)3},{{_tmp45B,_tmp45B,_tmp45B + 6},(void*)4},{{
_tmp45C,_tmp45C,_tmp45C + 8},(void*)& att_aligned},{{_tmp45D,_tmp45D,_tmp45D + 7},(
void*)5},{{_tmp45E,_tmp45E,_tmp45E + 7},(void*)7},{{_tmp45F,_tmp45F,_tmp45F + 7},(
void*)8},{{_tmp460,_tmp460,_tmp460 + 5},(void*)9},{{_tmp461,_tmp461,_tmp461 + 10},(
void*)10},{{_tmp462,_tmp462,_tmp462 + 10},(void*)11},{{_tmp463,_tmp463,_tmp463 + 
23},(void*)12},{{_tmp464,_tmp464,_tmp464 + 12},(void*)13},{{_tmp465,_tmp465,
_tmp465 + 11},(void*)14},{{_tmp466,_tmp466,_tmp466 + 22},(void*)15},{{_tmp467,
_tmp467,_tmp467 + 5},(void*)16}};struct _dyneither_ptr _tmp453=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((
_get_dyneither_size(_tmp453,sizeof(char))> 4  && *((const char*)
_check_dyneither_subscript(_tmp453,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp453,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp453,sizeof(char),(int)(_get_dyneither_size(
_tmp453,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp453,sizeof(char),(int)(_get_dyneither_size(_tmp453,sizeof(char))- 3)))== '_')
_tmp453=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp453,2,
_get_dyneither_size(_tmp453,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp453,(struct _dyneither_ptr)(att_map[i]).f1)== 0){yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp454;(_tmp454.YY46).tag=50;(
_tmp454.YY46).f1=(void*)(att_map[i]).f2;_tmp454;});break;}}if(i == 17){Cyc_Parse_err(({
const char*_tmp455="unrecognized attribute";_tag_dyneither(_tmp455,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp456;(_tmp456.YY46).tag=50;(_tmp456.YY46).f1=(void*)((void*)1);_tmp456;});}
break;}}case 63: _LL244: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp469;(_tmp469.YY46).tag=50;(_tmp469.YY46).f1=(void*)((void*)4);_tmp469;});
break;case 64: _LL245: {struct _dyneither_ptr _tmp46A=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);int _tmp46C;struct
_tuple11 _tmp46B=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp46C=_tmp46B.f2;{void*a;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp46A,({const char*_tmp46D="regparm";_tag_dyneither(_tmp46D,
sizeof(char),8);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp46A,({const char*
_tmp46E="__regparm__";_tag_dyneither(_tmp46E,sizeof(char),12);}))== 0){if(
_tmp46C < 0  || _tmp46C > 3)Cyc_Parse_err(({const char*_tmp46F="regparm requires value between 0 and 3";
_tag_dyneither(_tmp46F,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp470=_cycalloc_atomic(sizeof(*_tmp470));
_tmp470[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp471;_tmp471.tag=0;_tmp471.f1=
_tmp46C;_tmp471;});_tmp470;});}else{if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp46A,({
const char*_tmp472="aligned";_tag_dyneither(_tmp472,sizeof(char),8);}))== 0  || 
Cyc_zstrcmp((struct _dyneither_ptr)_tmp46A,({const char*_tmp473="__aligned__";
_tag_dyneither(_tmp473,sizeof(char),12);}))== 0){if(_tmp46C < 0)Cyc_Parse_err(({
const char*_tmp474="aligned requires positive power of two";_tag_dyneither(
_tmp474,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp46C;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp475="aligned requires positive power of two";
_tag_dyneither(_tmp475,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp476=_cycalloc_atomic(sizeof(*_tmp476));
_tmp476[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp477;_tmp477.tag=1;_tmp477.f1=
_tmp46C;_tmp477;});_tmp476;});}}else{if(Cyc_zstrcmp((struct _dyneither_ptr)
_tmp46A,({const char*_tmp478="initializes";_tag_dyneither(_tmp478,sizeof(char),12);}))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp46A,({const char*_tmp479="__initializes__";
_tag_dyneither(_tmp479,sizeof(char),16);}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*
_tmp47A=_cycalloc_atomic(sizeof(*_tmp47A));_tmp47A[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp47B;_tmp47B.tag=4;_tmp47B.f1=_tmp46C;_tmp47B;});_tmp47A;});else{Cyc_Parse_err(({
const char*_tmp47C="unrecognized attribute";_tag_dyneither(_tmp47C,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));a=(void*)1;}}}yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp47D;(_tmp47D.YY46).tag=50;(_tmp47D.YY46).f1=(void*)a;
_tmp47D;});break;}}case 65: _LL246: {struct _dyneither_ptr _tmp47E=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct
_dyneither_ptr _tmp47F=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp47E,({
const char*_tmp480="section";_tag_dyneither(_tmp480,sizeof(char),8);}))== 0  || 
Cyc_zstrcmp((struct _dyneither_ptr)_tmp47E,({const char*_tmp481="__section__";
_tag_dyneither(_tmp481,sizeof(char),12);}))== 0)a=(void*)({struct Cyc_Absyn_Section_att_struct*
_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482[0]=({struct Cyc_Absyn_Section_att_struct
_tmp483;_tmp483.tag=2;_tmp483.f1=_tmp47F;_tmp483;});_tmp482;});else{Cyc_Parse_err(({
const char*_tmp484="unrecognized attribute";_tag_dyneither(_tmp484,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));a=(void*)1;}yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp485;(_tmp485.YY46).tag=50;(_tmp485.YY46).f1=(void*)a;
_tmp485;});break;}case 66: _LL247: {struct _dyneither_ptr _tmp486=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct
_dyneither_ptr _tmp487=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp486,({
const char*_tmp488="__mode__";_tag_dyneither(_tmp488,sizeof(char),9);}))== 0)a=(
void*)({struct Cyc_Absyn_Mode_att_struct*_tmp489=_cycalloc(sizeof(*_tmp489));
_tmp489[0]=({struct Cyc_Absyn_Mode_att_struct _tmp48A;_tmp48A.tag=5;_tmp48A.f1=
_tmp487;_tmp48A;});_tmp489;});else{Cyc_Parse_err(({const char*_tmp48B="unrecognized attribute";
_tag_dyneither(_tmp48B,sizeof(char),23);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp48C;(_tmp48C.YY46).tag=
50;(_tmp48C.YY46).f1=(void*)a;_tmp48C;});break;}case 67: _LL248: {struct
_dyneither_ptr _tmp48D=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dyneither_ptr _tmp48E=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp490;struct
_tuple11 _tmp48F=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp490=_tmp48F.f2;{int _tmp492;struct _tuple11 _tmp491=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp492=_tmp491.f2;{
void*a=(void*)1;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp48D,({const char*_tmp493="format";
_tag_dyneither(_tmp493,sizeof(char),7);}))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp48D,({const char*_tmp494="__format__";_tag_dyneither(_tmp494,
sizeof(char),11);}))== 0){if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp48E,({const
char*_tmp495="printf";_tag_dyneither(_tmp495,sizeof(char),7);}))== 0)a=(void*)({
struct Cyc_Absyn_Format_att_struct*_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496[0]=({
struct Cyc_Absyn_Format_att_struct _tmp497;_tmp497.tag=3;_tmp497.f1=(void*)((void*)
0);_tmp497.f2=_tmp490;_tmp497.f3=_tmp492;_tmp497;});_tmp496;});else{if(Cyc_zstrcmp((
struct _dyneither_ptr)_tmp48E,({const char*_tmp498="scanf";_tag_dyneither(_tmp498,
sizeof(char),6);}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp499=
_cycalloc(sizeof(*_tmp499));_tmp499[0]=({struct Cyc_Absyn_Format_att_struct
_tmp49A;_tmp49A.tag=3;_tmp49A.f1=(void*)((void*)1);_tmp49A.f2=_tmp490;_tmp49A.f3=
_tmp492;_tmp49A;});_tmp499;});else{Cyc_Parse_err(({const char*_tmp49B="unrecognized format type";
_tag_dyneither(_tmp49B,sizeof(char),25);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp49C="unrecognized attribute";_tag_dyneither(_tmp49C,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp49D;(_tmp49D.YY46).tag=50;(_tmp49D.YY46).f1=(void*)a;_tmp49D;});break;}}}
case 68: _LL249: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 69: _LL24A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp49E;(_tmp49E.YY23).tag=27;(_tmp49E.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TypedefType_struct*_tmp49F=_cycalloc(sizeof(*_tmp49F));_tmp49F[0]=({
struct Cyc_Absyn_TypedefType_struct _tmp4A0;_tmp4A0.tag=16;_tmp4A0.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4A0.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4A0.f3=0;_tmp4A0.f4=
0;_tmp4A0;});_tmp49F;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp49E;});
break;case 70: _LL24B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4A1;(_tmp4A1.YY23).tag=27;(_tmp4A1.YY23).f1=(void*)Cyc_Parse_type_spec((void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4A1;});break;case 71: _LL24C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4A2;(_tmp4A2.YY23).tag=27;(_tmp4A2.YY23).f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A2;});
break;case 72: _LL24D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4A3;(_tmp4A3.YY23).tag=27;(_tmp4A3.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(
Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])),0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A3;});
break;case 73: _LL24E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4A4;(_tmp4A4.YY23).tag=27;(_tmp4A4.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_char_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4A4;});break;case 74: _LL24F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4A5;(_tmp4A5.YY23).tag=27;(_tmp4A5.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Short_spec_struct*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));
_tmp4A6[0]=({struct Cyc_Parse_Short_spec_struct _tmp4A7;_tmp4A7.tag=2;_tmp4A7.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4A7;});_tmp4A6;}));_tmp4A5;});break;case 75: _LL250:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4A8;(_tmp4A8.YY23).tag=
27;(_tmp4A8.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A8;});
break;case 76: _LL251: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4A9;(_tmp4A9.YY23).tag=27;(_tmp4A9.YY23).f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*
_tmp4AA=_cycalloc(sizeof(*_tmp4AA));_tmp4AA[0]=({struct Cyc_Parse_Long_spec_struct
_tmp4AB;_tmp4AB.tag=3;_tmp4AB.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4AB;});
_tmp4AA;}));_tmp4A9;});break;case 77: _LL252: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4AC;(_tmp4AC.YY23).tag=27;(_tmp4AC.YY23).f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4AC;});
break;case 78: _LL253: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4AD;(_tmp4AD.YY23).tag=27;(_tmp4AD.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_typ(
0),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4AD;});break;case 79: _LL254: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4AE;(_tmp4AE.YY23).tag=27;(_tmp4AE.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Signed_spec_struct*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));
_tmp4AF[0]=({struct Cyc_Parse_Signed_spec_struct _tmp4B0;_tmp4B0.tag=0;_tmp4B0.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4B0;});_tmp4AF;}));_tmp4AE;});break;case 80: _LL255:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4B1;(_tmp4B1.YY23).tag=
27;(_tmp4B1.YY23).f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp4B2=_cycalloc(sizeof(*_tmp4B2));_tmp4B2[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp4B3;_tmp4B3.tag=1;_tmp4B3.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4B3;});
_tmp4B2;}));_tmp4B1;});break;case 81: _LL256: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 82: _LL257: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 83: _LL258:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 84:
_LL259: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4B4;(_tmp4B4.YY23).tag=
27;(_tmp4B4.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4B4;});
break;case 85: _LL25A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4B5;(_tmp4B5.YY23).tag=27;(_tmp4B5.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TupleType_struct*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));_tmp4B6[0]=({
struct Cyc_Absyn_TupleType_struct _tmp4B7;_tmp4B7.tag=9;_tmp4B7.f1=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp4B7;});_tmp4B6;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4B5;});break;case 86: _LL25B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4B8;(_tmp4B8.YY23).tag=27;(_tmp4B8.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp4B9=_cycalloc(sizeof(*_tmp4B9));
_tmp4B9[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp4BA;_tmp4BA.tag=14;_tmp4BA.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4BA;});_tmp4B9;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4B8;});
break;case 87: _LL25C: {void*_tmp4BB=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,
0);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4BC;(_tmp4BC.YY23).tag=
27;(_tmp4BC.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp4BE;_tmp4BE.tag=15;_tmp4BE.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4BE.f2=(void*)
_tmp4BB;_tmp4BE;});_tmp4BD;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4BC;});
break;}case 88: _LL25D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4BF;(_tmp4BF.YY23).tag=27;(_tmp4BF.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_DynRgnType_struct*_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0[0]=({
struct Cyc_Absyn_DynRgnType_struct _tmp4C1;_tmp4C1.tag=15;_tmp4C1.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4C1.f2=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4C1;});_tmp4C0;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4BF;});
break;case 89: _LL25E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4C2;(_tmp4C2.YY23).tag=27;(_tmp4C2.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TagType_struct*_tmp4C3=_cycalloc(sizeof(*_tmp4C3));_tmp4C3[0]=({
struct Cyc_Absyn_TagType_struct _tmp4C4;_tmp4C4.tag=18;_tmp4C4.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4C4;});_tmp4C3;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4C2;});break;case 90: _LL25F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4C5;(_tmp4C5.YY23).tag=27;(_tmp4C5.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TagType_struct*_tmp4C6=_cycalloc(sizeof(*_tmp4C6));
_tmp4C6[0]=({struct Cyc_Absyn_TagType_struct _tmp4C7;_tmp4C7.tag=18;_tmp4C7.f1=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ik,0);_tmp4C7;});
_tmp4C6;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4C5;});break;case 91: _LL260: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4C8;(_tmp4C8.YY23).tag=27;(_tmp4C8.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_ValueofType_struct*_tmp4C9=_cycalloc(sizeof(*_tmp4C9));
_tmp4C9[0]=({struct Cyc_Absyn_ValueofType_struct _tmp4CA;_tmp4CA.tag=17;_tmp4CA.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4CA;});_tmp4C9;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4C8;});
break;case 92: _LL261: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4CB;(_tmp4CB.YY43).tag=47;(_tmp4CB.YY43).f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4CB;});
break;case 93: _LL262: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4CC;(_tmp4CC.YY25).tag=29;(_tmp4CC.YY25).f1=({struct Cyc_Absyn_Tqual _tmp4CD;
_tmp4CD.print_const=1;_tmp4CD.q_volatile=0;_tmp4CD.q_restrict=0;_tmp4CD.real_const=
1;_tmp4CD.loc=loc;_tmp4CD;});_tmp4CC;});break;}case 94: _LL263: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4CE;(_tmp4CE.YY25).tag=29;(_tmp4CE.YY25).f1=({struct
Cyc_Absyn_Tqual _tmp4CF;_tmp4CF.print_const=0;_tmp4CF.q_volatile=1;_tmp4CF.q_restrict=
0;_tmp4CF.real_const=0;_tmp4CF.loc=0;_tmp4CF;});_tmp4CE;});break;case 95: _LL264:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4D0;(_tmp4D0.YY25).tag=
29;(_tmp4D0.YY25).f1=({struct Cyc_Absyn_Tqual _tmp4D1;_tmp4D1.print_const=0;
_tmp4D1.q_volatile=0;_tmp4D1.q_restrict=1;_tmp4D1.real_const=0;_tmp4D1.loc=0;
_tmp4D1;});_tmp4D0;});break;case 96: _LL265: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4D2;(_tmp4D2.YY23).tag=27;(_tmp4D2.YY23).f1=(void*)((void*)({
struct Cyc_Parse_Decl_spec_struct*_tmp4D3=_cycalloc(sizeof(*_tmp4D3));_tmp4D3[0]=({
struct Cyc_Parse_Decl_spec_struct _tmp4D4;_tmp4D4.tag=5;_tmp4D4.f1=({struct Cyc_Absyn_Decl*
_tmp4D5=_cycalloc(sizeof(*_tmp4D5));_tmp4D5->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp4D6=_cycalloc(sizeof(*_tmp4D6));_tmp4D6[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp4D7;_tmp4D7.tag=8;_tmp4D7.f1=({struct Cyc_Absyn_Enumdecl*_tmp4D8=_cycalloc(
sizeof(*_tmp4D8));_tmp4D8->sc=(void*)((void*)2);_tmp4D8->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4D8->fields=({
struct Cyc_Core_Opt*_tmp4D9=_cycalloc(sizeof(*_tmp4D9));_tmp4D9->v=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4D9;});_tmp4D8;});
_tmp4D7;});_tmp4D6;}));_tmp4D5->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4D5;});
_tmp4D4;});_tmp4D3;}));_tmp4D2;});break;case 97: _LL266: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4DA;(_tmp4DA.YY23).tag=27;(_tmp4DA.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_EnumType_struct*_tmp4DB=_cycalloc(sizeof(*_tmp4DB));
_tmp4DB[0]=({struct Cyc_Absyn_EnumType_struct _tmp4DC;_tmp4DC.tag=12;_tmp4DC.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4DC.f2=0;_tmp4DC;});_tmp4DB;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4DA;});
break;case 98: _LL267: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4DD;(_tmp4DD.YY23).tag=27;(_tmp4DD.YY23).f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*
_tmp4DE=_cycalloc(sizeof(*_tmp4DE));_tmp4DE[0]=({struct Cyc_Parse_Type_spec_struct
_tmp4DF;_tmp4DF.tag=4;_tmp4DF.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp4E1;_tmp4E1.tag=13;_tmp4E1.f1=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4E1;});_tmp4E0;}));
_tmp4DF.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4DF;});_tmp4DE;}));_tmp4DD;});break;case 99: _LL268:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4E2;(_tmp4E2.YY47).tag=
51;(_tmp4E2.YY47).f1=({struct Cyc_Absyn_Enumfield*_tmp4E3=_cycalloc(sizeof(*
_tmp4E3));_tmp4E3->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4E3->tag=0;_tmp4E3->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4E3;});
_tmp4E2;});break;case 100: _LL269: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4E4;(_tmp4E4.YY47).tag=51;(_tmp4E4.YY47).f1=({struct Cyc_Absyn_Enumfield*
_tmp4E5=_cycalloc(sizeof(*_tmp4E5));_tmp4E5->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4E5->tag=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E5->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4E5;});_tmp4E4;});break;case 101: _LL26A: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4E6;(_tmp4E6.YY48).tag=52;(
_tmp4E6.YY48).f1=({struct Cyc_List_List*_tmp4E7=_cycalloc(sizeof(*_tmp4E7));
_tmp4E7->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4E7->tl=0;_tmp4E7;});_tmp4E6;});break;case 102: _LL26B: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4E8;(_tmp4E8.YY48).tag=52;(
_tmp4E8.YY48).f1=({struct Cyc_List_List*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));
_tmp4E9->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp4E9->tl=0;_tmp4E9;});_tmp4E8;});break;case 103: _LL26C:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4EA;(_tmp4EA.YY48).tag=
52;(_tmp4EA.YY48).f1=({struct Cyc_List_List*_tmp4EB=_cycalloc(sizeof(*_tmp4EB));
_tmp4EB->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp4EB->tl=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4EB;});_tmp4EA;});
break;case 104: _LL26D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4EC;(_tmp4EC.YY23).tag=27;(_tmp4EC.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_AnonAggrType_struct*_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED[
0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp4EE;_tmp4EE.tag=11;_tmp4EE.f1=(void*)
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp4EE.f2=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]);_tmp4EE;});_tmp4ED;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4EC;});
break;case 105: _LL26E: {struct Cyc_List_List*_tmp4EF=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp4F0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4F1;(_tmp4F1.YY23).tag=27;(_tmp4F1.YY23).f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp4F3;_tmp4F3.tag=5;_tmp4F3.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4EF,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4F0,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),1),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4F3;});
_tmp4F2;}));_tmp4F1;});break;}case 106: _LL26F: {struct Cyc_List_List*_tmp4F4=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]));struct Cyc_List_List*_tmp4F5=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4F6;(_tmp4F6.YY23).tag=27;(_tmp4F6.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Decl_spec_struct*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));
_tmp4F7[0]=({struct Cyc_Parse_Decl_spec_struct _tmp4F8;_tmp4F8.tag=5;_tmp4F8.f1=
Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4F4,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4F5,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4F8;});
_tmp4F7;}));_tmp4F6;});break;}case 107: _LL270: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4F9;(_tmp4F9.YY23).tag=27;(_tmp4F9.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));
_tmp4FA[0]=({struct Cyc_Absyn_AggrType_struct _tmp4FB;_tmp4FB.tag=10;_tmp4FB.f1=({
struct Cyc_Absyn_AggrInfo _tmp4FC;_tmp4FC.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp4FD;(_tmp4FD.UnknownAggr).tag=0;(_tmp4FD.UnknownAggr).f1=(
void*)Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);(
_tmp4FD.UnknownAggr).f2=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp4FD;});_tmp4FC.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4FC;});_tmp4FB;});
_tmp4FA;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4F9;});break;case 108: _LL271: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4FE;(_tmp4FE.YY40).tag=44;(_tmp4FE.YY40).f1=0;_tmp4FE;});
break;case 109: _LL272: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4FF;(_tmp4FF.YY40).tag=44;(_tmp4FF.YY40).f1=Cyc_List_imp_rev(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4FF;});break;
case 110: _LL273: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp500;(
_tmp500.YY24).tag=28;(_tmp500.YY24).f1=(void*)((void*)0);_tmp500;});break;case
111: _LL274: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp501;(
_tmp501.YY24).tag=28;(_tmp501.YY24).f1=(void*)((void*)1);_tmp501;});break;case
112: _LL275: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp502;(
_tmp502.YY26).tag=30;(_tmp502.YY26).f1=0;_tmp502;});break;case 113: _LL276: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp503;(_tmp503.YY26).tag=30;(
_tmp503.YY26).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));_tmp503;});break;
case 114: _LL277: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp504;(
_tmp504.YY27).tag=31;(_tmp504.YY27).f1=({struct Cyc_List_List*_tmp505=_cycalloc(
sizeof(*_tmp505));_tmp505->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp505->tl=0;_tmp505;});_tmp504;});break;case 115: _LL278:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp506;(_tmp506.YY27).tag=
31;(_tmp506.YY27).f1=({struct Cyc_List_List*_tmp507=_cycalloc(sizeof(*_tmp507));
_tmp507->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp507->tl=Cyc_yyget_YY27(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp507;});_tmp506;});break;case 116: _LL279: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp508;(_tmp508.YY21).tag=25;(_tmp508.YY21).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp508;});break;case
117: _LL27A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp509;(
_tmp509.YY21).tag=25;(_tmp509.YY21).f1=({struct Cyc_List_List*_tmp50A=_cycalloc(
sizeof(*_tmp50A));_tmp50A->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp50A->tl=0;_tmp50A;});_tmp509;});break;case 118: _LL27B:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp50B;(_tmp50B.YY21).tag=
25;(_tmp50B.YY21).f1=({struct Cyc_List_List*_tmp50C=_cycalloc(sizeof(*_tmp50C));
_tmp50C->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp50C->tl=Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp50C;});_tmp50B;});break;case 119: _LL27C: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp50D;(_tmp50D.YY20).tag=24;(_tmp50D.YY20).f1=({
struct _tuple15*_tmp50E=_cycalloc(sizeof(*_tmp50E));_tmp50E->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp50E->f2=0;_tmp50E;});
_tmp50D;});break;case 120: _LL27D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp50F;(_tmp50F.YY20).tag=24;(_tmp50F.YY20).f1=({struct _tuple15*_tmp510=
_cycalloc(sizeof(*_tmp510));_tmp510->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp510->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp510;});
_tmp50F;});break;case 121: _LL27E: {struct _RegionHandle _tmp511=_new_region("temp");
struct _RegionHandle*temp=& _tmp511;_push_region(temp);{struct _tuple16 _tmp513;
struct Cyc_Absyn_Tqual _tmp514;struct Cyc_List_List*_tmp515;struct Cyc_List_List*
_tmp516;struct _tuple16*_tmp512=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp513=*_tmp512;_tmp514=
_tmp513.f1;_tmp515=_tmp513.f2;_tmp516=_tmp513.f3;if(_tmp514.loc == 0)_tmp514.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp518;struct Cyc_List_List*
_tmp519;struct _tuple0 _tmp517=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(temp,temp,Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp518=_tmp517.f1;
_tmp519=_tmp517.f2;{void*_tmp51A=Cyc_Parse_speclist2typ(_tmp515,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp51B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(
temp,_tmp514,_tmp51A,_tmp518,_tmp516),_tmp519);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp51C;(_tmp51C.YY26).tag=30;(_tmp51C.YY26).f1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,
struct _tuple8*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp51B);
_tmp51C;});}}}_npop_handler(0);break;;_pop_region(temp);}case 122: _LL27F: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp51D;(_tmp51D.YY35).tag=39;(
_tmp51D.YY35).f1=({struct _tuple16*_tmp51E=_cycalloc(sizeof(*_tmp51E));_tmp51E->f1=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp51E->f2=({
struct Cyc_List_List*_tmp51F=_cycalloc(sizeof(*_tmp51F));_tmp51F->hd=(void*)Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp51F->tl=0;_tmp51F;});
_tmp51E->f3=0;_tmp51E;});_tmp51D;});break;case 123: _LL280: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp520;(_tmp520.YY35).tag=39;(_tmp520.YY35).f1=({struct
_tuple16*_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp521->f2=({struct Cyc_List_List*
_tmp522=_cycalloc(sizeof(*_tmp522));_tmp522->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp522->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp522;});_tmp521->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp521;});_tmp520;});break;case 124: _LL281: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp523;(_tmp523.YY35).tag=39;(_tmp523.YY35).f1=({struct
_tuple16*_tmp524=_cycalloc(sizeof(*_tmp524));_tmp524->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp524->f2=0;_tmp524->f3=0;
_tmp524;});_tmp523;});break;case 125: _LL282: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp525;(_tmp525.YY35).tag=39;(_tmp525.YY35).f1=({struct
_tuple16*_tmp526=_cycalloc(sizeof(*_tmp526));_tmp526->f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);
_tmp526->f2=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f2;_tmp526->f3=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp526;});_tmp525;});
break;case 126: _LL283: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp527;(_tmp527.YY35).tag=39;(_tmp527.YY35).f1=({struct _tuple16*_tmp528=
_cycalloc(sizeof(*_tmp528));_tmp528->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp528->f2=0;
_tmp528->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp528;});_tmp527;});break;case 127: _LL284: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp529;(_tmp529.YY35).tag=39;(_tmp529.YY35).f1=({struct
_tuple16*_tmp52A=_cycalloc(sizeof(*_tmp52A));_tmp52A->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp52A->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp52A->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp52A;});_tmp529;});break;case 128: _LL285: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp52B;(_tmp52B.YY35).tag=39;(_tmp52B.YY35).f1=({struct
_tuple16*_tmp52C=_cycalloc(sizeof(*_tmp52C));_tmp52C->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp52C->f2=({struct Cyc_List_List*_tmp52D=_cycalloc(
sizeof(*_tmp52D));_tmp52D->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp52D->tl=0;_tmp52D;});
_tmp52C->f3=0;_tmp52C;});_tmp52B;});break;case 129: _LL286: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp52E;(_tmp52E.YY35).tag=39;(_tmp52E.YY35).f1=({struct
_tuple16*_tmp52F=_cycalloc(sizeof(*_tmp52F));_tmp52F->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp52F->f2=({struct Cyc_List_List*
_tmp530=_cycalloc(sizeof(*_tmp530));_tmp530->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp530->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp530;});_tmp52F->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp52F;});_tmp52E;});break;case 130: _LL287: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp531;(_tmp531.YY35).tag=39;(_tmp531.YY35).f1=({struct
_tuple16*_tmp532=_cycalloc(sizeof(*_tmp532));_tmp532->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp532->f2=0;_tmp532->f3=0;
_tmp532;});_tmp531;});break;case 131: _LL288: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp533;(_tmp533.YY35).tag=39;(_tmp533.YY35).f1=({struct
_tuple16*_tmp534=_cycalloc(sizeof(*_tmp534));_tmp534->f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);
_tmp534->f2=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f2;_tmp534->f3=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp534;});_tmp533;});
break;case 132: _LL289: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp535;(_tmp535.YY35).tag=39;(_tmp535.YY35).f1=({struct _tuple16*_tmp536=
_cycalloc(sizeof(*_tmp536));_tmp536->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp536->f2=0;
_tmp536->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp536;});_tmp535;});break;case 133: _LL28A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp537;(_tmp537.YY35).tag=39;(_tmp537.YY35).f1=({struct
_tuple16*_tmp538=_cycalloc(sizeof(*_tmp538));_tmp538->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp538->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp538->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp538;});_tmp537;});break;case 134: _LL28B: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp539;(_tmp539.YY21).tag=25;(_tmp539.YY21).f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp539;});break;case 135:
_LL28C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp53A;(_tmp53A.YY21).tag=
25;(_tmp53A.YY21).f1=({struct Cyc_List_List*_tmp53B=_cycalloc(sizeof(*_tmp53B));
_tmp53B->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp53B->tl=0;_tmp53B;});_tmp53A;});break;case 136: _LL28D: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp53C;(_tmp53C.YY21).tag=25;(
_tmp53C.YY21).f1=({struct Cyc_List_List*_tmp53D=_cycalloc(sizeof(*_tmp53D));
_tmp53D->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp53D->tl=Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp53D;});_tmp53C;});break;case 137: _LL28E: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp53E;(_tmp53E.YY20).tag=24;(_tmp53E.YY20).f1=({
struct _tuple15*_tmp53F=_cycalloc(sizeof(*_tmp53F));_tmp53F->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp53F->f2=0;_tmp53F;});
_tmp53E;});break;case 138: _LL28F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp540;(_tmp540.YY20).tag=24;(_tmp540.YY20).f1=({struct _tuple15*_tmp541=
_cycalloc(sizeof(*_tmp541));_tmp541->f1=({struct Cyc_Parse_Declarator*_tmp542=
_cycalloc(sizeof(*_tmp542));_tmp542->id=({struct _tuple1*_tmp543=_cycalloc(
sizeof(*_tmp543));_tmp543->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp545;(_tmp545.Rel_n).tag=1;(_tmp545.Rel_n).f1=0;_tmp545;});_tmp543->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);
_tmp543;});_tmp542->tms=0;_tmp542;});_tmp541->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp541;});_tmp540;});
break;case 139: _LL290: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp546;(_tmp546.YY20).tag=24;(_tmp546.YY20).f1=({struct _tuple15*_tmp547=
_cycalloc(sizeof(*_tmp547));_tmp547->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp547->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp547;});
_tmp546;});break;case 140: _LL291: {int _tmp549;int _tmp54A;struct _tuple18 _tmp548=
Cyc_yyget_YY53(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp549=_tmp548.f1;_tmp54A=_tmp548.f2;{struct Cyc_List_List*_tmp54B=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp54C;(_tmp54C.YY23).tag=27;(_tmp54C.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Decl_spec_struct*_tmp54D=_cycalloc(sizeof(*_tmp54D));
_tmp54D[0]=({struct Cyc_Parse_Decl_spec_struct _tmp54E;_tmp54E.tag=5;_tmp54E.f1=
Cyc_Absyn_datatype_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp54B,({struct Cyc_Core_Opt*
_tmp54F=_cycalloc(sizeof(*_tmp54F));_tmp54F->v=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp54F;}),_tmp54A,
_tmp549,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp54E;});_tmp54D;}));_tmp54C;});break;}}case 141:
_LL292: {int _tmp551;int _tmp552;struct _tuple18 _tmp550=Cyc_yyget_YY53(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp551=_tmp550.f1;
_tmp552=_tmp550.f2;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp553;(
_tmp553.YY23).tag=27;(_tmp553.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_DatatypeType_struct*_tmp554=_cycalloc(sizeof(*_tmp554));_tmp554[
0]=({struct Cyc_Absyn_DatatypeType_struct _tmp555;_tmp555.tag=2;_tmp555.f1=({
struct Cyc_Absyn_DatatypeInfo _tmp556;_tmp556.datatype_info=(union Cyc_Absyn_DatatypeInfoU_union)({
union Cyc_Absyn_DatatypeInfoU_union _tmp557;(_tmp557.UnknownDatatype).tag=0;(
_tmp557.UnknownDatatype).f1=({struct Cyc_Absyn_UnknownDatatypeInfo _tmp558;_tmp558.name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp558.is_extensible=_tmp552;_tmp558.is_flat=_tmp551;_tmp558;});_tmp557;});
_tmp556.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp556.rgn=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp556;});_tmp555;});_tmp554;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp553;});
break;}case 142: _LL293: {int _tmp55A;int _tmp55B;struct _tuple18 _tmp559=Cyc_yyget_YY53(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp55A=_tmp559.f1;
_tmp55B=_tmp559.f2;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp55C;(
_tmp55C.YY23).tag=27;(_tmp55C.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp55D=_cycalloc(sizeof(*_tmp55D));
_tmp55D[0]=({struct Cyc_Absyn_DatatypeFieldType_struct _tmp55E;_tmp55E.tag=3;
_tmp55E.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp55F;_tmp55F.field_info=(union
Cyc_Absyn_DatatypeFieldInfoU_union)({union Cyc_Absyn_DatatypeFieldInfoU_union
_tmp560;(_tmp560.UnknownDatatypefield).tag=0;(_tmp560.UnknownDatatypefield).f1=({
struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp561;_tmp561.datatype_name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp561.field_name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp561.is_extensible=_tmp55B;_tmp561;});_tmp560;});_tmp55F.targs=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp55F;});_tmp55E;});
_tmp55D;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp55C;});break;}case 143: _LL294: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp562;(_tmp562.YY53).tag=57;(_tmp562.YY53).f1=({struct
_tuple18 _tmp563;_tmp563.f1=0;_tmp563.f2=0;_tmp563;});_tmp562;});break;case 144:
_LL295: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp564;(_tmp564.YY53).tag=
57;(_tmp564.YY53).f1=({struct _tuple18 _tmp565;_tmp565.f1=1;_tmp565.f2=0;_tmp565;});
_tmp564;});break;case 145: _LL296: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp566;(_tmp566.YY53).tag=57;(_tmp566.YY53).f1=({struct _tuple18 _tmp567;_tmp567.f1=
0;_tmp567.f2=1;_tmp567;});_tmp566;});break;case 146: _LL297: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp568;(_tmp568.YY34).tag=38;(_tmp568.YY34).f1=({struct
Cyc_List_List*_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569->hd=Cyc_yyget_YY33(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp569->tl=0;_tmp569;});
_tmp568;});break;case 147: _LL298: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp56A;(_tmp56A.YY34).tag=38;(_tmp56A.YY34).f1=({struct Cyc_List_List*_tmp56B=
_cycalloc(sizeof(*_tmp56B));_tmp56B->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp56B->tl=0;_tmp56B;});
_tmp56A;});break;case 148: _LL299: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp56C;(_tmp56C.YY34).tag=38;(_tmp56C.YY34).f1=({struct Cyc_List_List*_tmp56D=
_cycalloc(sizeof(*_tmp56D));_tmp56D->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp56D->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp56D;});_tmp56C;});
break;case 149: _LL29A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp56E;(_tmp56E.YY34).tag=38;(_tmp56E.YY34).f1=({struct Cyc_List_List*_tmp56F=
_cycalloc(sizeof(*_tmp56F));_tmp56F->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp56F->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp56F;});_tmp56E;});
break;case 150: _LL29B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp570;(_tmp570.YY32).tag=36;(_tmp570.YY32).f1=(void*)((void*)2);_tmp570;});
break;case 151: _LL29C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp571;(_tmp571.YY32).tag=36;(_tmp571.YY32).f1=(void*)((void*)3);_tmp571;});
break;case 152: _LL29D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp572;(_tmp572.YY32).tag=36;(_tmp572.YY32).f1=(void*)((void*)0);_tmp572;});
break;case 153: _LL29E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp573;(_tmp573.YY33).tag=37;(_tmp573.YY33).f1=({struct Cyc_Absyn_Datatypefield*
_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp574->typs=0;_tmp574->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp574->sc=(void*)Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp574;});_tmp573;});
break;case 154: _LL29F: {struct Cyc_List_List*_tmp575=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp576;(_tmp576.YY33).tag=37;(_tmp576.YY33).f1=({struct
Cyc_Absyn_Datatypefield*_tmp577=_cycalloc(sizeof(*_tmp577));_tmp577->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp577->typs=
_tmp575;_tmp577->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp577->sc=(
void*)Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp577;});_tmp576;});break;}case 155: _LL2A0: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 156: _LL2A1: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp578;(_tmp578.YY29).tag=33;(
_tmp578.YY29).f1=({struct Cyc_Parse_Declarator*_tmp579=_cycalloc(sizeof(*_tmp579));
_tmp579->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp579->tms=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp579;});_tmp578;});
break;case 157: _LL2A2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 158: _LL2A3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp57A;(_tmp57A.YY29).tag=33;(_tmp57A.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->id;_tmp57B->tms=Cyc_List_imp_append(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp57B;});
_tmp57A;});break;case 159: _LL2A4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp57C;(_tmp57C.YY29).tag=33;(_tmp57C.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp57D->tms=0;_tmp57D;});
_tmp57C;});break;case 160: _LL2A5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 161: _LL2A6: {struct Cyc_Parse_Declarator*_tmp57E=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp57E->tms=({
struct Cyc_List_List*_tmp57F=_cycalloc(sizeof(*_tmp57F));_tmp57F->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp580=_cycalloc(sizeof(*_tmp580));
_tmp580[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp581;_tmp581.tag=5;_tmp581.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp581.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp581;});_tmp580;}));
_tmp57F->tl=_tmp57E->tms;_tmp57F;});yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;}case 162: _LL2A7: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp582;(_tmp582.YY29).tag=33;(_tmp582.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp583->tms=({
struct Cyc_List_List*_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584->hd=(void*)((
void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp585=_cycalloc(sizeof(*_tmp585));
_tmp585[0]=({struct Cyc_Absyn_Carray_mod_struct _tmp586;_tmp586.tag=0;_tmp586.f1=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp586.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp586;});_tmp585;}));_tmp584->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp584;});
_tmp583;});_tmp582;});break;case 163: _LL2A8: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp587;(_tmp587.YY29).tag=33;(_tmp587.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp588->tms=({
struct Cyc_List_List*_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp58A=_cycalloc(sizeof(*_tmp58A));
_tmp58A[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp58B;_tmp58B.tag=1;_tmp58B.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp58B.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp58B.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp58B;});_tmp58A;}));_tmp589->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp589;});
_tmp588;});_tmp587;});break;case 164: _LL2A9: {struct _tuple17 _tmp58D;struct Cyc_List_List*
_tmp58E;int _tmp58F;struct Cyc_Absyn_VarargInfo*_tmp590;struct Cyc_Core_Opt*_tmp591;
struct Cyc_List_List*_tmp592;struct _tuple17*_tmp58C=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp58D=*_tmp58C;_tmp58E=
_tmp58D.f1;_tmp58F=_tmp58D.f2;_tmp590=_tmp58D.f3;_tmp591=_tmp58D.f4;_tmp592=
_tmp58D.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp593;(
_tmp593.YY29).tag=33;(_tmp593.YY29).f1=({struct Cyc_Parse_Declarator*_tmp594=
_cycalloc(sizeof(*_tmp594));_tmp594->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp594->tms=({
struct Cyc_List_List*_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp596=_cycalloc(sizeof(*_tmp596));
_tmp596[0]=({struct Cyc_Absyn_Function_mod_struct _tmp597;_tmp597.tag=3;_tmp597.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp598=_cycalloc(sizeof(*
_tmp598));_tmp598[0]=({struct Cyc_Absyn_WithTypes_struct _tmp599;_tmp599.tag=1;
_tmp599.f1=_tmp58E;_tmp599.f2=_tmp58F;_tmp599.f3=_tmp590;_tmp599.f4=_tmp591;
_tmp599.f5=_tmp592;_tmp599;});_tmp598;}));_tmp597;});_tmp596;}));_tmp595->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;
_tmp595;});_tmp594;});_tmp593;});break;}case 165: _LL2AA: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp59A;(_tmp59A.YY29).tag=33;(_tmp59A.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp59B->tms=({
struct Cyc_List_List*_tmp59C=_cycalloc(sizeof(*_tmp59C));_tmp59C->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp59D=_cycalloc(sizeof(*_tmp59D));
_tmp59D[0]=({struct Cyc_Absyn_Function_mod_struct _tmp59E;_tmp59E.tag=3;_tmp59E.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp59F=_cycalloc(sizeof(*
_tmp59F));_tmp59F[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5A0;_tmp5A0.tag=1;
_tmp5A0.f1=0;_tmp5A0.f2=0;_tmp5A0.f3=0;_tmp5A0.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5A0.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5A0;});_tmp59F;}));
_tmp59E;});_tmp59D;}));_tmp59C->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp59C;});_tmp59B;});
_tmp59A;});break;case 166: _LL2AB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5A1;(_tmp5A1.YY29).tag=33;(_tmp5A1.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5A2->tms=({
struct Cyc_List_List*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));_tmp5A3->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));
_tmp5A4[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5A5;_tmp5A5.tag=3;_tmp5A5.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));
_tmp5A6[0]=({struct Cyc_Absyn_NoTypes_struct _tmp5A7;_tmp5A7.tag=0;_tmp5A7.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5A7.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5A7;});
_tmp5A6;}));_tmp5A5;});_tmp5A4;}));_tmp5A3->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5A3;});_tmp5A2;});
_tmp5A1;});break;case 167: _LL2AC: {struct Cyc_List_List*_tmp5A8=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5A9;(_tmp5A9.YY29).tag=
33;(_tmp5A9.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5AA=_cycalloc(sizeof(*
_tmp5AA));_tmp5AA->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5AA->tms=({struct Cyc_List_List*_tmp5AB=_cycalloc(
sizeof(*_tmp5AB));_tmp5AB->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp5AC=_cycalloc(sizeof(*_tmp5AC));_tmp5AC[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp5AD;_tmp5AD.tag=4;_tmp5AD.f1=_tmp5A8;_tmp5AD.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5AD.f3=0;
_tmp5AD;});_tmp5AC;}));_tmp5AB->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5AB;});_tmp5AA;});
_tmp5A9;});break;}case 168: _LL2AD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5AE;(_tmp5AE.YY29).tag=33;(_tmp5AE.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp5AF->tms=({
struct Cyc_List_List*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));
_tmp5B1[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp5B2;_tmp5B2.tag=5;_tmp5B2.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5B2.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B2;});_tmp5B1;}));
_tmp5B0->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp5B0;});_tmp5AF;});_tmp5AE;});break;case 169: _LL2AE:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5B3;(_tmp5B3.YY29).tag=
33;(_tmp5B3.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5B4=_cycalloc(sizeof(*
_tmp5B4));_tmp5B4->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp5B4->tms=0;_tmp5B4;});_tmp5B3;});break;case 170: _LL2AF:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5B5;(_tmp5B5.YY29).tag=
33;(_tmp5B5.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5B6=_cycalloc(sizeof(*
_tmp5B6));_tmp5B6->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp5B6->tms=0;_tmp5B6;});_tmp5B5;});break;case 171: _LL2B0:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 172:
_LL2B1: {struct Cyc_Parse_Declarator*_tmp5B7=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5B7->tms=({struct Cyc_List_List*
_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5BA;_tmp5BA.tag=5;_tmp5BA.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5BA.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5BA;});_tmp5B9;}));_tmp5B8->tl=_tmp5B7->tms;_tmp5B8;});yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 173: _LL2B2:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5BB;(_tmp5BB.YY29).tag=
33;(_tmp5BB.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5BC=_cycalloc(sizeof(*
_tmp5BC));_tmp5BC->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5BC->tms=({struct Cyc_List_List*_tmp5BD=_cycalloc(
sizeof(*_tmp5BD));_tmp5BD->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp5BF;_tmp5BF.tag=0;_tmp5BF.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5BF.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5BF;});
_tmp5BE;}));_tmp5BD->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms;_tmp5BD;});_tmp5BC;});_tmp5BB;});break;case 174:
_LL2B3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5C0;(_tmp5C0.YY29).tag=
33;(_tmp5C0.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5C1=_cycalloc(sizeof(*
_tmp5C1));_tmp5C1->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp5C1->tms=({struct Cyc_List_List*_tmp5C2=_cycalloc(
sizeof(*_tmp5C2));_tmp5C2->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp5C3=_cycalloc(sizeof(*_tmp5C3));_tmp5C3[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp5C4;_tmp5C4.tag=1;_tmp5C4.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5C4.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C4.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5C4;});
_tmp5C3;}));_tmp5C2->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms;_tmp5C2;});_tmp5C1;});_tmp5C0;});break;case 175:
_LL2B4: {struct _tuple17 _tmp5C6;struct Cyc_List_List*_tmp5C7;int _tmp5C8;struct Cyc_Absyn_VarargInfo*
_tmp5C9;struct Cyc_Core_Opt*_tmp5CA;struct Cyc_List_List*_tmp5CB;struct _tuple17*
_tmp5C5=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5C6=*_tmp5C5;_tmp5C7=_tmp5C6.f1;_tmp5C8=_tmp5C6.f2;_tmp5C9=_tmp5C6.f3;
_tmp5CA=_tmp5C6.f4;_tmp5CB=_tmp5C6.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5CC;(_tmp5CC.YY29).tag=33;(_tmp5CC.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5CD=_cycalloc(sizeof(*_tmp5CD));_tmp5CD->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5CD->tms=({
struct Cyc_List_List*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5CF=_cycalloc(sizeof(*_tmp5CF));
_tmp5CF[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5D0;_tmp5D0.tag=3;_tmp5D0.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5D1=_cycalloc(sizeof(*
_tmp5D1));_tmp5D1[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5D2;_tmp5D2.tag=1;
_tmp5D2.f1=_tmp5C7;_tmp5D2.f2=_tmp5C8;_tmp5D2.f3=_tmp5C9;_tmp5D2.f4=_tmp5CA;
_tmp5D2.f5=_tmp5CB;_tmp5D2;});_tmp5D1;}));_tmp5D0;});_tmp5CF;}));_tmp5CE->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;
_tmp5CE;});_tmp5CD;});_tmp5CC;});break;}case 176: _LL2B5: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5D3;(_tmp5D3.YY29).tag=33;(_tmp5D3.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5D4->tms=({
struct Cyc_List_List*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));
_tmp5D6[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5D7;_tmp5D7.tag=3;_tmp5D7.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5D8=_cycalloc(sizeof(*
_tmp5D8));_tmp5D8[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5D9;_tmp5D9.tag=1;
_tmp5D9.f1=0;_tmp5D9.f2=0;_tmp5D9.f3=0;_tmp5D9.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5D9.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D9;});_tmp5D8;}));
_tmp5D7;});_tmp5D6;}));_tmp5D5->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5D5;});_tmp5D4;});
_tmp5D3;});break;case 177: _LL2B6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5DA;(_tmp5DA.YY29).tag=33;(_tmp5DA.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5DB=_cycalloc(sizeof(*_tmp5DB));_tmp5DB->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5DB->tms=({
struct Cyc_List_List*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));_tmp5DC->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));
_tmp5DD[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5DE;_tmp5DE.tag=3;_tmp5DE.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp5DF=_cycalloc(sizeof(*_tmp5DF));
_tmp5DF[0]=({struct Cyc_Absyn_NoTypes_struct _tmp5E0;_tmp5E0.tag=0;_tmp5E0.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E0.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5E0;});
_tmp5DF;}));_tmp5DE;});_tmp5DD;}));_tmp5DC->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5DC;});_tmp5DB;});
_tmp5DA;});break;case 178: _LL2B7: {struct Cyc_List_List*_tmp5E1=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5E2;(_tmp5E2.YY29).tag=
33;(_tmp5E2.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5E3=_cycalloc(sizeof(*
_tmp5E3));_tmp5E3->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5E3->tms=({struct Cyc_List_List*_tmp5E4=_cycalloc(
sizeof(*_tmp5E4));_tmp5E4->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp5E5=_cycalloc(sizeof(*_tmp5E5));_tmp5E5[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp5E6;_tmp5E6.tag=4;_tmp5E6.f1=_tmp5E1;_tmp5E6.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5E6.f3=0;
_tmp5E6;});_tmp5E5;}));_tmp5E4->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5E4;});_tmp5E3;});
_tmp5E2;});break;}case 179: _LL2B8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5E7;(_tmp5E7.YY29).tag=33;(_tmp5E7.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp5E8->tms=({
struct Cyc_List_List*_tmp5E9=_cycalloc(sizeof(*_tmp5E9));_tmp5E9->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp5EA=_cycalloc(sizeof(*_tmp5EA));
_tmp5EA[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp5EB;_tmp5EB.tag=5;_tmp5EB.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5EB.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5EB;});_tmp5EA;}));
_tmp5E9->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp5E9;});_tmp5E8;});_tmp5E7;});break;case 180: _LL2B9:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 181:
_LL2BA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5EC;(_tmp5EC.YY28).tag=
32;(_tmp5EC.YY28).f1=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5EC;});break;case 182:
_LL2BB: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp5ED=_cycalloc(sizeof(*_tmp5ED));_tmp5ED->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5EF;_tmp5EF.tag=5;_tmp5EF.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp5EF.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5EF;});_tmp5EE;}));_tmp5ED->tl=ans;_tmp5ED;});{struct Cyc_Absyn_PtrLoc*ptrloc=
0;if(Cyc_Absyn_porting_c_code)ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp5F0=_cycalloc(
sizeof(*_tmp5F0));_tmp5F0->ptr_loc=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1;_tmp5F0->rgn_loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5F0->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line);_tmp5F0;});ans=({struct Cyc_List_List*_tmp5F1=
_cycalloc(sizeof(*_tmp5F1));_tmp5F1->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp5F3;_tmp5F3.tag=2;_tmp5F3.f1=({struct Cyc_Absyn_PtrAtts _tmp5F4;_tmp5F4.rgn=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5F4.nullable=(*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])).f2;_tmp5F4.bounds=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f3;_tmp5F4.zero_term=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp5F4.ptrloc=ptrloc;_tmp5F4;});_tmp5F3.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F3;});_tmp5F2;}));
_tmp5F1->tl=ans;_tmp5F1;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5F5;(_tmp5F5.YY28).tag=32;(_tmp5F5.YY28).f1=ans;_tmp5F5;});break;}}case 183:
_LL2BC: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line): 0;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5F6;(_tmp5F6.YY1).tag=5;(
_tmp5F6.YY1).f1=({struct _tuple12*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7->f1=
loc;_tmp5F7->f2=Cyc_Absyn_true_conref;_tmp5F7->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F7;});_tmp5F6;});
break;}case 184: _LL2BD: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5F8;(_tmp5F8.YY1).tag=5;(_tmp5F8.YY1).f1=({struct _tuple12*_tmp5F9=_cycalloc(
sizeof(*_tmp5F9));_tmp5F9->f1=loc;_tmp5F9->f2=Cyc_Absyn_false_conref;_tmp5F9->f3=
Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F9;});
_tmp5F8;});break;}case 185: _LL2BE: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5FA;(_tmp5FA.YY1).tag=5;(_tmp5FA.YY1).f1=({struct _tuple12*_tmp5FB=_cycalloc(
sizeof(*_tmp5FB));_tmp5FB->f1=loc;_tmp5FB->f2=Cyc_Absyn_true_conref;_tmp5FB->f3=
Cyc_Absyn_bounds_dyneither_conref;_tmp5FB;});_tmp5FA;});break;}case 186: _LL2BF:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5FC;(_tmp5FC.YY2).tag=
6;(_tmp5FC.YY2).f1=Cyc_Absyn_bounds_one_conref;_tmp5FC;});break;case 187: _LL2C0:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5FD;(_tmp5FD.YY2).tag=
6;(_tmp5FD.YY2).f1=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp5FE=_cycalloc(sizeof(*_tmp5FE));_tmp5FE[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp5FF;_tmp5FF.tag=0;_tmp5FF.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5FF;});_tmp5FE;}));
_tmp5FD;});break;case 188: _LL2C1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp600;(_tmp600.YY51).tag=55;(_tmp600.YY51).f1=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp600;});break;case 189: _LL2C2: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp601;(_tmp601.YY51).tag=55;(_tmp601.YY51).f1=Cyc_Absyn_true_conref;
_tmp601;});break;case 190: _LL2C3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp602;(_tmp602.YY51).tag=55;(_tmp602.YY51).f1=Cyc_Absyn_false_conref;_tmp602;});
break;case 191: _LL2C4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp603;(_tmp603.YY49).tag=53;(_tmp603.YY49).f1=0;_tmp603;});break;case 192:
_LL2C5: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3,1);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp604;(_tmp604.YY49).tag=53;(_tmp604.YY49).f1=({struct
Cyc_Core_Opt*_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605->v=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp605;});_tmp604;});
break;case 193: _LL2C6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp606;(_tmp606.YY49).tag=53;(_tmp606.YY49).f1=({struct Cyc_Core_Opt*_tmp607=
_cycalloc(sizeof(*_tmp607));_tmp607->v=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0);_tmp607;});_tmp606;});break;case 194: _LL2C7: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp608;(_tmp608.YY44).tag=48;(_tmp608.YY44).f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,0);_tmp608;});break;case 195: _LL2C8: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)
5,1);yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
196: _LL2C9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp609;(
_tmp609.YY44).tag=48;(_tmp609.YY44).f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,0);_tmp609;});break;case 197: _LL2CA: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp60A;(_tmp60A.YY25).tag=29;(_tmp60A.YY25).f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).last_line));_tmp60A;});break;case 198: _LL2CB: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp60B;(_tmp60B.YY25).tag=29;(_tmp60B.YY25).f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp60B;});break;case
199: _LL2CC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp60C;(
_tmp60C.YY39).tag=43;(_tmp60C.YY39).f1=({struct _tuple17*_tmp60D=_cycalloc(
sizeof(*_tmp60D));_tmp60D->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp60D->f2=0;_tmp60D->f3=0;_tmp60D->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp60D->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60D;});_tmp60C;});
break;case 200: _LL2CD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp60E;(_tmp60E.YY39).tag=43;(_tmp60E.YY39).f1=({struct _tuple17*_tmp60F=
_cycalloc(sizeof(*_tmp60F));_tmp60F->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]));_tmp60F->f2=1;_tmp60F->f3=0;_tmp60F->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp60F->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60F;});_tmp60E;});
break;case 201: _LL2CE: {struct _tuple2 _tmp611;struct Cyc_Core_Opt*_tmp612;struct Cyc_Absyn_Tqual
_tmp613;void*_tmp614;struct _tuple2*_tmp610=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp611=*_tmp610;_tmp612=
_tmp611.f1;_tmp613=_tmp611.f2;_tmp614=_tmp611.f3;{struct Cyc_Absyn_VarargInfo*
_tmp615=({struct Cyc_Absyn_VarargInfo*_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618->name=
_tmp612;_tmp618->tq=_tmp613;_tmp618->type=(void*)_tmp614;_tmp618->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp618;});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp616;(_tmp616.YY39).tag=43;(
_tmp616.YY39).f1=({struct _tuple17*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->f1=
0;_tmp617->f2=0;_tmp617->f3=_tmp615;_tmp617->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp617->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp617;});_tmp616;});
break;}}case 202: _LL2CF: {struct _tuple2 _tmp61A;struct Cyc_Core_Opt*_tmp61B;struct
Cyc_Absyn_Tqual _tmp61C;void*_tmp61D;struct _tuple2*_tmp619=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp61A=*_tmp619;_tmp61B=
_tmp61A.f1;_tmp61C=_tmp61A.f2;_tmp61D=_tmp61A.f3;{struct Cyc_Absyn_VarargInfo*
_tmp61E=({struct Cyc_Absyn_VarargInfo*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->name=
_tmp61B;_tmp621->tq=_tmp61C;_tmp621->type=(void*)_tmp61D;_tmp621->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp621;});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp61F;(_tmp61F.YY39).tag=43;(
_tmp61F.YY39).f1=({struct _tuple17*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp620->f2=0;
_tmp620->f3=_tmp61E;_tmp620->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp620->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp620;});_tmp61F;});
break;}}case 203: _LL2D0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp622;(_tmp622.YY44).tag=48;(_tmp622.YY44).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp623=_cycalloc(sizeof(*_tmp623));_tmp623[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp624;_tmp624.tag=1;_tmp624.f1=0;_tmp624;});_tmp623;}));_tmp622;});break;case
204: _LL2D1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp625;(
_tmp625.YY44).tag=48;(_tmp625.YY44).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));
_tmp625;});break;case 205: _LL2D2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp626;(_tmp626.YY49).tag=53;(_tmp626.YY49).f1=0;_tmp626;});break;case 206:
_LL2D3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp627;(_tmp627.YY49).tag=
53;(_tmp627.YY49).f1=({struct Cyc_Core_Opt*_tmp628=_cycalloc(sizeof(*_tmp628));
_tmp628->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp629=_cycalloc(
sizeof(*_tmp629));_tmp629[0]=({struct Cyc_Absyn_JoinEff_struct _tmp62A;_tmp62A.tag=
20;_tmp62A.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp62A;});_tmp629;}));_tmp628;});_tmp627;});break;case 207:
_LL2D4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp62B;(_tmp62B.YY50).tag=
54;(_tmp62B.YY50).f1=0;_tmp62B;});break;case 208: _LL2D5: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 209: _LL2D6: {
struct Cyc_Absyn_Less_kb_struct*_tmp62C=({struct Cyc_Absyn_Less_kb_struct*_tmp634=
_cycalloc(sizeof(*_tmp634));_tmp634[0]=({struct Cyc_Absyn_Less_kb_struct _tmp635;
_tmp635.tag=2;_tmp635.f1=0;_tmp635.f2=(void*)((void*)5);_tmp635;});_tmp634;});
struct _dyneither_ptr _tmp62D=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);void*_tmp62E=Cyc_Parse_id2type(
_tmp62D,(void*)_tmp62C);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp62F;(_tmp62F.YY50).tag=54;(_tmp62F.YY50).f1=({struct Cyc_List_List*_tmp630=
_cycalloc(sizeof(*_tmp630));_tmp630->hd=({struct _tuple4*_tmp631=_cycalloc(
sizeof(*_tmp631));_tmp631->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp632=
_cycalloc(sizeof(*_tmp632));_tmp632[0]=({struct Cyc_Absyn_JoinEff_struct _tmp633;
_tmp633.tag=20;_tmp633.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp633;});_tmp632;});_tmp631->f2=_tmp62E;_tmp631;});
_tmp630->tl=0;_tmp630;});_tmp62F;});break;}case 210: _LL2D7: {struct Cyc_Absyn_Less_kb_struct*
_tmp636=({struct Cyc_Absyn_Less_kb_struct*_tmp63E=_cycalloc(sizeof(*_tmp63E));
_tmp63E[0]=({struct Cyc_Absyn_Less_kb_struct _tmp63F;_tmp63F.tag=2;_tmp63F.f1=0;
_tmp63F.f2=(void*)((void*)5);_tmp63F;});_tmp63E;});struct _dyneither_ptr _tmp637=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
void*_tmp638=Cyc_Parse_id2type(_tmp637,(void*)_tmp636);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp639;(_tmp639.YY50).tag=54;(_tmp639.YY50).f1=({struct
Cyc_List_List*_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A->hd=({struct _tuple4*
_tmp63B=_cycalloc(sizeof(*_tmp63B));_tmp63B->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp63D;_tmp63D.tag=20;_tmp63D.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp63D;});_tmp63C;});
_tmp63B->f2=_tmp638;_tmp63B;});_tmp63A->tl=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp63A;});_tmp639;});
break;}case 211: _LL2D8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp640;(_tmp640.YY31).tag=35;(_tmp640.YY31).f1=0;_tmp640;});break;case 212:
_LL2D9: if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp641="inject";
_tag_dyneither(_tmp641,sizeof(char),7);}))!= 0)Cyc_Parse_err(({const char*_tmp642="missing type in function declaration";
_tag_dyneither(_tmp642,sizeof(char),37);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp643;(_tmp643.YY31).tag=35;(_tmp643.YY31).f1=1;_tmp643;});
break;case 213: _LL2DA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 214: _LL2DB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp644;(_tmp644.YY40).tag=44;(_tmp644.YY40).f1=Cyc_List_imp_append(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp644;});break;case 215:
_LL2DC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp645;(_tmp645.YY40).tag=
44;(_tmp645.YY40).f1=0;_tmp645;});break;case 216: _LL2DD: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 217: _LL2DE:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp646;(_tmp646.YY40).tag=
44;(_tmp646.YY40).f1=({struct Cyc_List_List*_tmp647=_cycalloc(sizeof(*_tmp647));
_tmp647->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp648=_cycalloc(
sizeof(*_tmp648));_tmp648[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp649;_tmp649.tag=
21;_tmp649.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp649;});_tmp648;}));_tmp647->tl=0;_tmp647;});_tmp646;});
break;case 218: _LL2DF: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)6,0);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp64A;(_tmp64A.YY40).tag=44;(_tmp64A.YY40).f1=({struct
Cyc_List_List*_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp64B->tl=0;_tmp64B;});
_tmp64A;});break;case 219: _LL2E0: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)5,1);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp64C;(_tmp64C.YY40).tag=44;(_tmp64C.YY40).f1=({struct
Cyc_List_List*_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D->hd=(void*)((void*)({
struct Cyc_Absyn_AccessEff_struct*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp64F;_tmp64F.tag=19;_tmp64F.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp64F;});_tmp64E;}));
_tmp64D->tl=0;_tmp64D;});_tmp64C;});break;case 220: _LL2E1: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
void*)5,1);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp650;(
_tmp650.YY40).tag=44;(_tmp650.YY40).f1=({struct Cyc_List_List*_tmp651=_cycalloc(
sizeof(*_tmp651));_tmp651->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp653;_tmp653.tag=19;_tmp653.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp653;});_tmp652;}));
_tmp651->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp651;});_tmp650;});break;case 221: _LL2E2: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp654;(_tmp654.YY38).tag=42;(_tmp654.YY38).f1=({struct
Cyc_List_List*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp655->tl=0;_tmp655;});
_tmp654;});break;case 222: _LL2E3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp656;(_tmp656.YY38).tag=42;(_tmp656.YY38).f1=({struct Cyc_List_List*_tmp657=
_cycalloc(sizeof(*_tmp657));_tmp657->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp657->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp657;});_tmp656;});
break;case 223: _LL2E4: {struct _tuple16 _tmp659;struct Cyc_Absyn_Tqual _tmp65A;struct
Cyc_List_List*_tmp65B;struct Cyc_List_List*_tmp65C;struct _tuple16*_tmp658=Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp659=*_tmp658;
_tmp65A=_tmp659.f1;_tmp65B=_tmp659.f2;_tmp65C=_tmp659.f3;if(_tmp65A.loc == 0)
_tmp65A.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp65E;struct _tuple1*
_tmp65F;struct Cyc_List_List*_tmp660;struct Cyc_Parse_Declarator*_tmp65D=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp65E=*_tmp65D;
_tmp65F=_tmp65E.id;_tmp660=_tmp65E.tms;{void*_tmp661=Cyc_Parse_speclist2typ(
_tmp65B,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp663;void*_tmp664;struct
Cyc_List_List*_tmp665;struct Cyc_List_List*_tmp666;struct _tuple6 _tmp662=Cyc_Parse_apply_tms(
_tmp65A,_tmp661,_tmp65C,_tmp660);_tmp663=_tmp662.f1;_tmp664=_tmp662.f2;_tmp665=
_tmp662.f3;_tmp666=_tmp662.f4;if(_tmp665 != 0)Cyc_Parse_err(({const char*_tmp667="parameter with bad type params";
_tag_dyneither(_tmp667,sizeof(char),31);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp65F))Cyc_Parse_err(({const char*_tmp668="parameter cannot be qualified with a namespace";
_tag_dyneither(_tmp668,sizeof(char),47);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp669=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp66E=_cycalloc(sizeof(*
_tmp66E));_tmp66E->v=(*_tmp65F).f2;_tmp66E;});if(_tmp666 != 0)({void*_tmp66A=0;
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp66B="extra attributes on parameter, ignoring";
_tag_dyneither(_tmp66B,sizeof(char),40);}),_tag_dyneither(_tmp66A,sizeof(void*),
0));});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp66C;(_tmp66C.YY37).tag=
41;(_tmp66C.YY37).f1=({struct _tuple2*_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D->f1=
_tmp669;_tmp66D->f2=_tmp663;_tmp66D->f3=_tmp664;_tmp66D;});_tmp66C;});break;}}}}
case 224: _LL2E5: {struct _tuple16 _tmp670;struct Cyc_Absyn_Tqual _tmp671;struct Cyc_List_List*
_tmp672;struct Cyc_List_List*_tmp673;struct _tuple16*_tmp66F=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp670=*_tmp66F;_tmp671=
_tmp670.f1;_tmp672=_tmp670.f2;_tmp673=_tmp670.f3;if(_tmp671.loc == 0)_tmp671.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);{void*_tmp674=Cyc_Parse_speclist2typ(_tmp672,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp673 != 0)({
void*_tmp675=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp676="bad attributes on parameter, ignoring";_tag_dyneither(_tmp676,sizeof(
char),38);}),_tag_dyneither(_tmp675,sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp677;(_tmp677.YY37).tag=41;(_tmp677.YY37).f1=({struct
_tuple2*_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678->f1=0;_tmp678->f2=_tmp671;
_tmp678->f3=_tmp674;_tmp678;});_tmp677;});break;}}case 225: _LL2E6: {struct
_tuple16 _tmp67A;struct Cyc_Absyn_Tqual _tmp67B;struct Cyc_List_List*_tmp67C;struct
Cyc_List_List*_tmp67D;struct _tuple16*_tmp679=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp67A=*_tmp679;_tmp67B=
_tmp67A.f1;_tmp67C=_tmp67A.f2;_tmp67D=_tmp67A.f3;if(_tmp67B.loc == 0)_tmp67B.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{void*_tmp67E=Cyc_Parse_speclist2typ(_tmp67C,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp67F=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp681;void*_tmp682;struct Cyc_List_List*_tmp683;struct Cyc_List_List*
_tmp684;struct _tuple6 _tmp680=Cyc_Parse_apply_tms(_tmp67B,_tmp67E,_tmp67D,_tmp67F);
_tmp681=_tmp680.f1;_tmp682=_tmp680.f2;_tmp683=_tmp680.f3;_tmp684=_tmp680.f4;if(
_tmp683 != 0)({void*_tmp685=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp686="bad type parameters on formal argument, ignoring";_tag_dyneither(
_tmp686,sizeof(char),49);}),_tag_dyneither(_tmp685,sizeof(void*),0));});if(
_tmp684 != 0)({void*_tmp687=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp688="bad attributes on parameter, ignoring";_tag_dyneither(_tmp688,sizeof(
char),38);}),_tag_dyneither(_tmp687,sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp689;(_tmp689.YY37).tag=41;(_tmp689.YY37).f1=({struct
_tuple2*_tmp68A=_cycalloc(sizeof(*_tmp68A));_tmp68A->f1=0;_tmp68A->f2=_tmp681;
_tmp68A->f3=_tmp682;_tmp68A;});_tmp689;});break;}}case 226: _LL2E7: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp68B;(_tmp68B.YY36).tag=40;(_tmp68B.YY36).f1=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp68B;});break;case 227:
_LL2E8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp68C;(_tmp68C.YY36).tag=
40;(_tmp68C.YY36).f1=({struct Cyc_List_List*_tmp68D=_cycalloc(sizeof(*_tmp68D));
_tmp68D->hd=({struct _dyneither_ptr*_tmp68E=_cycalloc(sizeof(*_tmp68E));_tmp68E[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp68E;});_tmp68D->tl=0;_tmp68D;});_tmp68C;});break;case 228: _LL2E9: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp68F;(_tmp68F.YY36).tag=40;(_tmp68F.YY36).f1=({
struct Cyc_List_List*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690->hd=({struct
_dyneither_ptr*_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp691;});_tmp690->tl=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp690;});_tmp68F;});break;case 229: _LL2EA: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 230: _LL2EB: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 231: _LL2EC:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp692;(_tmp692.YY3).tag=
7;(_tmp692.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp694;_tmp694.tag=37;_tmp694.f1=0;_tmp694.f2=0;_tmp694;});_tmp693;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp692;});
break;case 232: _LL2ED: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp695;(_tmp695.YY3).tag=7;(_tmp695.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnresolvedMem_e_struct*_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp697;_tmp697.tag=37;_tmp697.f1=0;
_tmp697.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));
_tmp697;});_tmp696;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp695;});
break;case 233: _LL2EE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp698;(_tmp698.YY3).tag=7;(_tmp698.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnresolvedMem_e_struct*_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp69A;_tmp69A.tag=37;_tmp69A.f1=0;
_tmp69A.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp69A;});_tmp699;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp698;});
break;case 234: _LL2EF: {struct Cyc_Absyn_Vardecl*_tmp69B=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp69F=_cycalloc(sizeof(*_tmp69F));_tmp69F->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp6A1;(_tmp6A1.Loc_n).tag=0;_tmp6A1;});_tmp69F->f2=({
struct _dyneither_ptr*_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp6A0;});_tmp69F;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp69B->tq).real_const=
1;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp69C;(_tmp69C.YY3).tag=
7;(_tmp69C.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp69D=_cycalloc(sizeof(*_tmp69D));_tmp69D[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp69E;_tmp69E.tag=29;_tmp69E.f1=_tmp69B;_tmp69E.f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp69E.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp69E.f4=0;
_tmp69E;});_tmp69D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp69C;});
break;}case 235: _LL2F0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6A2;(_tmp6A2.YY6).tag=10;(_tmp6A2.YY6).f1=({struct Cyc_List_List*_tmp6A3=
_cycalloc(sizeof(*_tmp6A3));_tmp6A3->hd=({struct _tuple21*_tmp6A4=_cycalloc(
sizeof(*_tmp6A4));_tmp6A4->f1=0;_tmp6A4->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A4;});_tmp6A3->tl=0;
_tmp6A3;});_tmp6A2;});break;case 236: _LL2F1: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp6A5;(_tmp6A5.YY6).tag=10;(_tmp6A5.YY6).f1=({struct Cyc_List_List*
_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6->hd=({struct _tuple21*_tmp6A7=
_cycalloc(sizeof(*_tmp6A7));_tmp6A7->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6A7->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A7;});_tmp6A6->tl=
0;_tmp6A6;});_tmp6A5;});break;case 237: _LL2F2: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp6A8;(_tmp6A8.YY6).tag=10;(_tmp6A8.YY6).f1=({struct Cyc_List_List*
_tmp6A9=_cycalloc(sizeof(*_tmp6A9));_tmp6A9->hd=({struct _tuple21*_tmp6AA=
_cycalloc(sizeof(*_tmp6AA));_tmp6AA->f1=0;_tmp6AA->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6AA;});_tmp6A9->tl=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6A9;});_tmp6A8;});
break;case 238: _LL2F3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6AB;(_tmp6AB.YY6).tag=10;(_tmp6AB.YY6).f1=({struct Cyc_List_List*_tmp6AC=
_cycalloc(sizeof(*_tmp6AC));_tmp6AC->hd=({struct _tuple21*_tmp6AD=_cycalloc(
sizeof(*_tmp6AD));_tmp6AD->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp6AD->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6AD;});_tmp6AC->tl=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6AC;});_tmp6AB;});
break;case 239: _LL2F4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6AE;(_tmp6AE.YY41).tag=45;(_tmp6AE.YY41).f1=Cyc_List_imp_rev(Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp6AE;});break;
case 240: _LL2F5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6AF;(
_tmp6AF.YY41).tag=45;(_tmp6AF.YY41).f1=({struct Cyc_List_List*_tmp6B0=_cycalloc(
sizeof(*_tmp6B0));_tmp6B0->hd=(void*)Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B0->tl=0;_tmp6B0;});
_tmp6AF;});break;case 241: _LL2F6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B1;(_tmp6B1.YY41).tag=45;(_tmp6B1.YY41).f1=({struct Cyc_List_List*_tmp6B2=
_cycalloc(sizeof(*_tmp6B2));_tmp6B2->hd=(void*)Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B2->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6B2;});_tmp6B1;});
break;case 242: _LL2F7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B3;(_tmp6B3.YY42).tag=46;(_tmp6B3.YY42).f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp6B4=_cycalloc(sizeof(*_tmp6B4));_tmp6B4[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp6B5;_tmp6B5.tag=0;_tmp6B5.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6B5;});_tmp6B4;}));
_tmp6B3;});break;case 243: _LL2F8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6B6;(_tmp6B6.YY42).tag=46;(_tmp6B6.YY42).f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7[0]=({struct Cyc_Absyn_FieldName_struct
_tmp6B8;_tmp6B8.tag=1;_tmp6B8.f1=({struct _dyneither_ptr*_tmp6B9=_cycalloc(
sizeof(*_tmp6B9));_tmp6B9[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B9;});_tmp6B8;});
_tmp6B7;}));_tmp6B6;});break;case 244: _LL2F9: {struct _tuple16 _tmp6BB;struct Cyc_Absyn_Tqual
_tmp6BC;struct Cyc_List_List*_tmp6BD;struct Cyc_List_List*_tmp6BE;struct _tuple16*
_tmp6BA=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp6BB=*_tmp6BA;_tmp6BC=_tmp6BB.f1;_tmp6BD=_tmp6BB.f2;_tmp6BE=_tmp6BB.f3;{void*
_tmp6BF=Cyc_Parse_speclist2typ(_tmp6BD,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp6BE != 0)({
void*_tmp6C0=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6C1="ignoring attributes in type";_tag_dyneither(_tmp6C1,sizeof(char),28);}),
_tag_dyneither(_tmp6C0,sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp6C2;(_tmp6C2.YY37).tag=41;(_tmp6C2.YY37).f1=({struct
_tuple2*_tmp6C3=_cycalloc(sizeof(*_tmp6C3));_tmp6C3->f1=0;_tmp6C3->f2=_tmp6BC;
_tmp6C3->f3=_tmp6BF;_tmp6C3;});_tmp6C2;});break;}}case 245: _LL2FA: {struct
_tuple16 _tmp6C5;struct Cyc_Absyn_Tqual _tmp6C6;struct Cyc_List_List*_tmp6C7;struct
Cyc_List_List*_tmp6C8;struct _tuple16*_tmp6C4=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6C5=*_tmp6C4;_tmp6C6=
_tmp6C5.f1;_tmp6C7=_tmp6C5.f2;_tmp6C8=_tmp6C5.f3;{void*_tmp6C9=Cyc_Parse_speclist2typ(
_tmp6C7,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp6CA=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct _tuple6 _tmp6CB=
Cyc_Parse_apply_tms(_tmp6C6,_tmp6C9,_tmp6C8,_tmp6CA);if(_tmp6CB.f3 != 0)({void*
_tmp6CC=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6CD="bad type params, ignoring";_tag_dyneither(_tmp6CD,sizeof(char),26);}),
_tag_dyneither(_tmp6CC,sizeof(void*),0));});if(_tmp6CB.f4 != 0)({void*_tmp6CE=0;
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6CF="bad specifiers, ignoring";
_tag_dyneither(_tmp6CF,sizeof(char),25);}),_tag_dyneither(_tmp6CE,sizeof(void*),
0));});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6D0;(_tmp6D0.YY37).tag=
41;(_tmp6D0.YY37).f1=({struct _tuple2*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1->f1=
0;_tmp6D1->f2=_tmp6CB.f1;_tmp6D1->f3=_tmp6CB.f2;_tmp6D1;});_tmp6D0;});break;}}
case 246: _LL2FB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6D2;(
_tmp6D2.YY44).tag=48;(_tmp6D2.YY44).f1=(void*)(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp6D2;});break;case
247: _LL2FC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6D3;(
_tmp6D3.YY44).tag=48;(_tmp6D3.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6D5;_tmp6D5.tag=20;_tmp6D5.f1=0;_tmp6D5;});_tmp6D4;}));_tmp6D3;});break;case
248: _LL2FD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6D6;(
_tmp6D6.YY44).tag=48;(_tmp6D6.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6D8;_tmp6D8.tag=20;_tmp6D8.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6D8;});_tmp6D7;}));
_tmp6D6;});break;case 249: _LL2FE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6D9;(_tmp6D9.YY44).tag=48;(_tmp6D9.YY44).f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp6DA=_cycalloc(sizeof(*_tmp6DA));_tmp6DA[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp6DB;_tmp6DB.tag=21;_tmp6DB.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6DB;});_tmp6DA;}));
_tmp6D9;});break;case 250: _LL2FF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6DC;(_tmp6DC.YY44).tag=48;(_tmp6DC.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6DD=_cycalloc(sizeof(*_tmp6DD));_tmp6DD[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6DE;_tmp6DE.tag=20;_tmp6DE.f1=({struct Cyc_List_List*_tmp6DF=_cycalloc(
sizeof(*_tmp6DF));_tmp6DF->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6DF->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6DF;});_tmp6DE;});
_tmp6DD;}));_tmp6DC;});break;case 251: _LL300: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp6E0;(_tmp6E0.YY40).tag=44;(_tmp6E0.YY40).f1=({struct
Cyc_List_List*_tmp6E1=_cycalloc(sizeof(*_tmp6E1));_tmp6E1->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E1->tl=0;_tmp6E1;});
_tmp6E0;});break;case 252: _LL301: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6E2;(_tmp6E2.YY40).tag=44;(_tmp6E2.YY40).f1=({struct Cyc_List_List*_tmp6E3=
_cycalloc(sizeof(*_tmp6E3));_tmp6E3->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E3->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6E3;});_tmp6E2;});
break;case 253: _LL302: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6E4;(_tmp6E4.YY30).tag=34;(_tmp6E4.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6E5=_cycalloc(sizeof(*_tmp6E5));_tmp6E5->tms=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E5;});_tmp6E4;});
break;case 254: _LL303: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 255: _LL304: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6E6;(_tmp6E6.YY30).tag=34;(_tmp6E6.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6E7=_cycalloc(sizeof(*_tmp6E7));_tmp6E7->tms=Cyc_List_imp_append(Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp6E7;});
_tmp6E6;});break;case 256: _LL305: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 257: _LL306: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6E8;(_tmp6E8.YY30).tag=34;(_tmp6E8.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6E9=_cycalloc(sizeof(*_tmp6E9));_tmp6E9->tms=({struct Cyc_List_List*_tmp6EA=
_cycalloc(sizeof(*_tmp6EA));_tmp6EA->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp6EB=_cycalloc(sizeof(*_tmp6EB));_tmp6EB[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6EC;_tmp6EC.tag=0;_tmp6EC.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6EC.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6EC;});
_tmp6EB;}));_tmp6EA->tl=0;_tmp6EA;});_tmp6E9;});_tmp6E8;});break;case 258: _LL307:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6ED;(_tmp6ED.YY30).tag=
34;(_tmp6ED.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6EE=_cycalloc(
sizeof(*_tmp6EE));_tmp6EE->tms=({struct Cyc_List_List*_tmp6EF=_cycalloc(sizeof(*
_tmp6EF));_tmp6EF->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp6F0=
_cycalloc(sizeof(*_tmp6F0));_tmp6F0[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6F1;_tmp6F1.tag=0;_tmp6F1.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6F1.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6F1;});
_tmp6F0;}));_tmp6EF->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms;_tmp6EF;});_tmp6EE;});_tmp6ED;});break;case 259:
_LL308: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6F2;(_tmp6F2.YY30).tag=
34;(_tmp6F2.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6F3=_cycalloc(
sizeof(*_tmp6F3));_tmp6F3->tms=({struct Cyc_List_List*_tmp6F4=_cycalloc(sizeof(*
_tmp6F4));_tmp6F4->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6F5=_cycalloc(sizeof(*_tmp6F5));_tmp6F5[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6F6;_tmp6F6.tag=1;_tmp6F6.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6F6.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6F6.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6F6;});
_tmp6F5;}));_tmp6F4->tl=0;_tmp6F4;});_tmp6F3;});_tmp6F2;});break;case 260: _LL309:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6F7;(_tmp6F7.YY30).tag=
34;(_tmp6F7.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6F8=_cycalloc(
sizeof(*_tmp6F8));_tmp6F8->tms=({struct Cyc_List_List*_tmp6F9=_cycalloc(sizeof(*
_tmp6F9));_tmp6F9->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6FA=_cycalloc(sizeof(*_tmp6FA));_tmp6FA[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6FB;_tmp6FB.tag=1;_tmp6FB.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6FB.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6FB.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6FB;});
_tmp6FA;}));_tmp6F9->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms;_tmp6F9;});_tmp6F8;});_tmp6F7;});break;case 261:
_LL30A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6FC;(_tmp6FC.YY30).tag=
34;(_tmp6FC.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6FD=_cycalloc(
sizeof(*_tmp6FD));_tmp6FD->tms=({struct Cyc_List_List*_tmp6FE=_cycalloc(sizeof(*
_tmp6FE));_tmp6FE->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6FF=_cycalloc(sizeof(*_tmp6FF));_tmp6FF[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp700;_tmp700.tag=3;_tmp700.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp701=_cycalloc(sizeof(*_tmp701));_tmp701[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp702;_tmp702.tag=1;_tmp702.f1=0;_tmp702.f2=0;_tmp702.f3=0;_tmp702.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp702.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp702;});_tmp701;}));
_tmp700;});_tmp6FF;}));_tmp6FE->tl=0;_tmp6FE;});_tmp6FD;});_tmp6FC;});break;case
262: _LL30B: {struct _tuple17 _tmp704;struct Cyc_List_List*_tmp705;int _tmp706;struct
Cyc_Absyn_VarargInfo*_tmp707;struct Cyc_Core_Opt*_tmp708;struct Cyc_List_List*
_tmp709;struct _tuple17*_tmp703=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp704=*_tmp703;_tmp705=
_tmp704.f1;_tmp706=_tmp704.f2;_tmp707=_tmp704.f3;_tmp708=_tmp704.f4;_tmp709=
_tmp704.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp70A;(
_tmp70A.YY30).tag=34;(_tmp70A.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp70B=_cycalloc(sizeof(*_tmp70B));_tmp70B->tms=({struct Cyc_List_List*_tmp70C=
_cycalloc(sizeof(*_tmp70C));_tmp70C->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp70D=_cycalloc(sizeof(*_tmp70D));_tmp70D[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp70E;_tmp70E.tag=3;_tmp70E.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp70F=_cycalloc(sizeof(*_tmp70F));_tmp70F[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp710;_tmp710.tag=1;_tmp710.f1=_tmp705;_tmp710.f2=_tmp706;_tmp710.f3=_tmp707;
_tmp710.f4=_tmp708;_tmp710.f5=_tmp709;_tmp710;});_tmp70F;}));_tmp70E;});_tmp70D;}));
_tmp70C->tl=0;_tmp70C;});_tmp70B;});_tmp70A;});break;}case 263: _LL30C: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp711;(_tmp711.YY30).tag=34;(
_tmp711.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp712=_cycalloc(sizeof(*
_tmp712));_tmp712->tms=({struct Cyc_List_List*_tmp713=_cycalloc(sizeof(*_tmp713));
_tmp713->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp714=
_cycalloc(sizeof(*_tmp714));_tmp714[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp715;_tmp715.tag=3;_tmp715.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp716=_cycalloc(sizeof(*_tmp716));_tmp716[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp717;_tmp717.tag=1;_tmp717.f1=0;_tmp717.f2=0;_tmp717.f3=0;_tmp717.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp717.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp717;});_tmp716;}));
_tmp715;});_tmp714;}));_tmp713->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp713;});_tmp712;});
_tmp711;});break;case 264: _LL30D: {struct _tuple17 _tmp719;struct Cyc_List_List*
_tmp71A;int _tmp71B;struct Cyc_Absyn_VarargInfo*_tmp71C;struct Cyc_Core_Opt*_tmp71D;
struct Cyc_List_List*_tmp71E;struct _tuple17*_tmp718=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp719=*_tmp718;_tmp71A=
_tmp719.f1;_tmp71B=_tmp719.f2;_tmp71C=_tmp719.f3;_tmp71D=_tmp719.f4;_tmp71E=
_tmp719.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp71F;(
_tmp71F.YY30).tag=34;(_tmp71F.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp720=_cycalloc(sizeof(*_tmp720));_tmp720->tms=({struct Cyc_List_List*_tmp721=
_cycalloc(sizeof(*_tmp721));_tmp721->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp722=_cycalloc(sizeof(*_tmp722));_tmp722[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp723;_tmp723.tag=3;_tmp723.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp724=_cycalloc(sizeof(*_tmp724));_tmp724[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp725;_tmp725.tag=1;_tmp725.f1=_tmp71A;_tmp725.f2=_tmp71B;_tmp725.f3=_tmp71C;
_tmp725.f4=_tmp71D;_tmp725.f5=_tmp71E;_tmp725;});_tmp724;}));_tmp723;});_tmp722;}));
_tmp721->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp721;});_tmp720;});_tmp71F;});break;}case 265: _LL30E: {
struct Cyc_List_List*_tmp726=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp727;(_tmp727.YY30).tag=
34;(_tmp727.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp728=_cycalloc(
sizeof(*_tmp728));_tmp728->tms=({struct Cyc_List_List*_tmp729=_cycalloc(sizeof(*
_tmp729));_tmp729->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp72A=_cycalloc(sizeof(*_tmp72A));_tmp72A[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp72B;_tmp72B.tag=4;_tmp72B.f1=_tmp726;_tmp72B.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp72B.f3=0;
_tmp72B;});_tmp72A;}));_tmp729->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp729;});_tmp728;});
_tmp727;});break;}case 266: _LL30F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp72C;(_tmp72C.YY30).tag=34;(_tmp72C.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp72D=_cycalloc(sizeof(*_tmp72D));_tmp72D->tms=({struct Cyc_List_List*_tmp72E=
_cycalloc(sizeof(*_tmp72E));_tmp72E->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp72F=_cycalloc(sizeof(*_tmp72F));_tmp72F[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp730;_tmp730.tag=5;_tmp730.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp730.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp730;});_tmp72F;}));
_tmp72E->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp72E;});_tmp72D;});_tmp72C;});break;case 267: _LL310:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 268:
_LL311: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
269: _LL312: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 270: _LL313: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 271: _LL314: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 272: _LL315: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 273: _LL316: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp731;(_tmp731.YY9).tag=13;(_tmp731.YY9).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_ResetRegion_s_struct*_tmp732=_cycalloc(sizeof(*_tmp732));
_tmp732[0]=({struct Cyc_Absyn_ResetRegion_s_struct _tmp733;_tmp733.tag=15;_tmp733.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp733;});_tmp732;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp731;});
break;case 274: _LL317: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp734;(_tmp734.YY4).tag=8;(_tmp734.YY4).f1=0;_tmp734;});break;case 275: _LL318:
if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp735="open";
_tag_dyneither(_tmp735,sizeof(char),5);}))!= 0)Cyc_Parse_err(({const char*_tmp736="expecting `open'";
_tag_dyneither(_tmp736,sizeof(char),17);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp737;(_tmp737.YY4).tag=8;(_tmp737.YY4).f1=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp737;});break;case 276: _LL319: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp738;(_tmp738.YY9).tag=13;(_tmp738.YY9).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp739=_cycalloc(sizeof(*_tmp739));
_tmp739[0]=({struct Cyc_Absyn_Label_s_struct _tmp73A;_tmp73A.tag=12;_tmp73A.f1=({
struct _dyneither_ptr*_tmp73B=_cycalloc(sizeof(*_tmp73B));_tmp73B[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp73B;});_tmp73A.f2=
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp73A;});
_tmp739;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp738;});break;case 277: _LL31A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp73C;(_tmp73C.YY9).tag=13;(_tmp73C.YY9).f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp73C;});break;case 278: _LL31B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp73D;(_tmp73D.YY9).tag=13;(_tmp73D.YY9).f1=Cyc_Absyn_exp_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp73D;});
break;case 279: _LL31C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp73E;(_tmp73E.YY9).tag=13;(_tmp73E.YY9).f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp73E;});
break;case 280: _LL31D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 281: _LL31E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp73F;(_tmp73F.YY9).tag=13;(_tmp73F.YY9).f1=Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0));
_tmp73F;});break;case 282: _LL31F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp740;(_tmp740.YY9).tag=13;(_tmp740.YY9).f1=Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp740;});break;case 283:
_LL320: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
284: _LL321: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp741;(
_tmp741.YY9).tag=13;(_tmp741.YY9).f1=Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp741;});
break;case 285: _LL322: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp742;(_tmp742.YY9).tag=13;(_tmp742.YY9).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp743=_cycalloc(sizeof(*_tmp743));_tmp743[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp744;_tmp744.tag=1;_tmp744.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp744;});_tmp743;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp742;});break;case 286:
_LL323: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp745;(_tmp745.YY9).tag=
13;(_tmp745.YY9).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp746=_cycalloc(sizeof(*_tmp746));_tmp746[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp747;_tmp747.tag=1;_tmp747.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp747;});_tmp746;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]));_tmp745;});break;case 287: _LL324: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp748;(_tmp748.YY9).tag=13;(_tmp748.YY9).f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp748;});break;case 288: _LL325: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp749;(_tmp749.YY9).tag=13;(_tmp749.YY9).f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp749;});
break;case 289: _LL326: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp74A;(_tmp74A.YY9).tag=13;(_tmp74A.YY9).f1=Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp74A;});
break;case 290: _LL327: {struct Cyc_Absyn_Exp*_tmp74B=Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmp74D=_cycalloc(sizeof(*_tmp74D));_tmp74D[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp74E;_tmp74E.tag=2;_tmp74E.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp74E;});_tmp74D;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp74C;(_tmp74C.YY9).tag=13;(_tmp74C.YY9).f1=Cyc_Absyn_switch_stmt(_tmp74B,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp74C;});
break;}case 291: _LL328: {struct Cyc_Absyn_Exp*_tmp74F=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp750;(_tmp750.YY9).tag=13;(_tmp750.YY9).f1=
Cyc_Absyn_switch_stmt(_tmp74F,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp750;});
break;}case 292: _LL329: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp751;(_tmp751.YY9).tag=13;(_tmp751.YY9).f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp751;});
break;case 293: _LL32A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp752;(_tmp752.YY10).tag=14;(_tmp752.YY10).f1=0;_tmp752;});break;case 294:
_LL32B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp753;(_tmp753.YY10).tag=
14;(_tmp753.YY10).f1=({struct Cyc_List_List*_tmp754=_cycalloc(sizeof(*_tmp754));
_tmp754->hd=({struct Cyc_Absyn_Switch_clause*_tmp755=_cycalloc(sizeof(*_tmp755));
_tmp755->pattern=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp755->pat_vars=
0;_tmp755->where_clause=0;_tmp755->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp755->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp755;});
_tmp754->tl=0;_tmp754;});_tmp753;});break;case 295: _LL32C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp756;(_tmp756.YY10).tag=14;(_tmp756.YY10).f1=({struct
Cyc_List_List*_tmp757=_cycalloc(sizeof(*_tmp757));_tmp757->hd=({struct Cyc_Absyn_Switch_clause*
_tmp758=_cycalloc(sizeof(*_tmp758));_tmp758->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp758->pat_vars=0;
_tmp758->where_clause=0;_tmp758->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp758->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp758;});_tmp757->tl=Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp757;});_tmp756;});
break;case 296: _LL32D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp759;(_tmp759.YY10).tag=14;(_tmp759.YY10).f1=({struct Cyc_List_List*_tmp75A=
_cycalloc(sizeof(*_tmp75A));_tmp75A->hd=({struct Cyc_Absyn_Switch_clause*_tmp75B=
_cycalloc(sizeof(*_tmp75B));_tmp75B->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp75B->pat_vars=0;
_tmp75B->where_clause=0;_tmp75B->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp75B->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp75B;});
_tmp75A->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp75A;});_tmp759;});break;case 297: _LL32E: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp75C;(_tmp75C.YY10).tag=14;(_tmp75C.YY10).f1=({struct
Cyc_List_List*_tmp75D=_cycalloc(sizeof(*_tmp75D));_tmp75D->hd=({struct Cyc_Absyn_Switch_clause*
_tmp75E=_cycalloc(sizeof(*_tmp75E));_tmp75E->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp75E->pat_vars=0;
_tmp75E->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp75E->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp75E->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp75E;});
_tmp75D->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp75D;});_tmp75C;});break;case 298: _LL32F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp75F;(_tmp75F.YY10).tag=14;(_tmp75F.YY10).f1=({struct
Cyc_List_List*_tmp760=_cycalloc(sizeof(*_tmp760));_tmp760->hd=({struct Cyc_Absyn_Switch_clause*
_tmp761=_cycalloc(sizeof(*_tmp761));_tmp761->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp761->pat_vars=0;
_tmp761->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp761->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp761->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp761;});
_tmp760->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp760;});_tmp75F;});break;case 299: _LL330: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp762;(_tmp762.YY9).tag=13;(_tmp762.YY9).f1=Cyc_Absyn_while_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp762;});
break;case 300: _LL331: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp763;(_tmp763.YY9).tag=13;(_tmp763.YY9).f1=Cyc_Absyn_do_stmt(Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp763;});
break;case 301: _LL332: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp764;(_tmp764.YY9).tag=13;(_tmp764.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp764;});
break;case 302: _LL333: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp765;(_tmp765.YY9).tag=13;(_tmp765.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp765;});break;case 303: _LL334: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp766;(_tmp766.YY9).tag=13;(_tmp766.YY9).f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp766;});
break;case 304: _LL335: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp767;(_tmp767.YY9).tag=13;(_tmp767.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp767;});
break;case 305: _LL336: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp768;(_tmp768.YY9).tag=13;(_tmp768.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp768;});
break;case 306: _LL337: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp769;(_tmp769.YY9).tag=13;(_tmp769.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp769;});
break;case 307: _LL338: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp76A;(_tmp76A.YY9).tag=13;(_tmp76A.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76A;});
break;case 308: _LL339: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp76B;(_tmp76B.YY9).tag=13;(_tmp76B.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp76B;});
break;case 309: _LL33A: {struct Cyc_List_List*_tmp76C=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp76D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp76E;(_tmp76E.YY9).tag=13;(_tmp76E.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp76C,_tmp76D);_tmp76E;});break;}case 310: _LL33B: {struct Cyc_List_List*_tmp76F=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
struct Cyc_Absyn_Stmt*_tmp770=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp771;(_tmp771.YY9).tag=13;(_tmp771.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp76F,_tmp770);_tmp771;});break;}case 311: _LL33C: {struct Cyc_List_List*_tmp772=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
struct Cyc_Absyn_Stmt*_tmp773=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp774;(_tmp774.YY9).tag=13;(_tmp774.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp772,_tmp773);_tmp774;});break;}case 312: _LL33D: {struct Cyc_List_List*_tmp775=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
struct Cyc_Absyn_Stmt*_tmp776=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp777;(_tmp777.YY9).tag=13;(_tmp777.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp775,_tmp776);_tmp777;});break;}case 313: _LL33E: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp778;(_tmp778.YY9).tag=13;(_tmp778.YY9).f1=Cyc_Absyn_goto_stmt(({
struct _dyneither_ptr*_tmp779=_cycalloc(sizeof(*_tmp779));_tmp779[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp779;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp778;});
break;case 314: _LL33F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77A;(_tmp77A.YY9).tag=13;(_tmp77A.YY9).f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp77A;});
break;case 315: _LL340: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77B;(_tmp77B.YY9).tag=13;(_tmp77B.YY9).f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp77B;});
break;case 316: _LL341: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77C;(_tmp77C.YY9).tag=13;(_tmp77C.YY9).f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp77C;});
break;case 317: _LL342: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77D;(_tmp77D.YY9).tag=13;(_tmp77D.YY9).f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp77D;});
break;case 318: _LL343: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77E;(_tmp77E.YY9).tag=13;(_tmp77E.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp77E;});
break;case 319: _LL344: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77F;(_tmp77F.YY9).tag=13;(_tmp77F.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));_tmp77F;});
break;case 320: _LL345: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp780;(_tmp780.YY9).tag=13;(_tmp780.YY9).f1=Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));_tmp780;});
break;case 321: _LL346: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 322: _LL347: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 323: _LL348: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp781;(_tmp781.YY11).tag=15;(_tmp781.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp781;});
break;case 324: _LL349: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 325: _LL34A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp782;(_tmp782.YY11).tag=15;(_tmp782.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp782;});break;case 326: _LL34B: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 327: _LL34C: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp783;(_tmp783.YY11).tag=15;(
_tmp783.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp783;});
break;case 328: _LL34D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 329: _LL34E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp784;(_tmp784.YY11).tag=15;(_tmp784.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)14,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp784;});
break;case 330: _LL34F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 331: _LL350: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp785;(_tmp785.YY11).tag=15;(_tmp785.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)15,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp785;});
break;case 332: _LL351: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 333: _LL352: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp786;(_tmp786.YY11).tag=15;(_tmp786.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)13,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp786;});
break;case 334: _LL353: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 335: _LL354: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp787;(_tmp787.YY11).tag=15;(_tmp787.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp787;});break;case 336: _LL355: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp788;(_tmp788.YY11).tag=15;(_tmp788.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp788;});
break;case 337: _LL356: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 338: _LL357: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp789;(_tmp789.YY11).tag=15;(_tmp789.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp789;});break;case 339: _LL358: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp78A;(_tmp78A.YY11).tag=15;(_tmp78A.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78A;});
break;case 340: _LL359: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78B;(_tmp78B.YY11).tag=15;(_tmp78B.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp78B;});break;case 341: _LL35A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp78C;(_tmp78C.YY11).tag=15;(_tmp78C.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78C;});
break;case 342: _LL35B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 343: _LL35C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78D;(_tmp78D.YY11).tag=15;(_tmp78D.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)16,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78D;});
break;case 344: _LL35D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78E;(_tmp78E.YY11).tag=15;(_tmp78E.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)17,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78E;});
break;case 345: _LL35E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 346: _LL35F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78F;(_tmp78F.YY11).tag=15;(_tmp78F.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)0,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp78F;});
break;case 347: _LL360: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp790;(_tmp790.YY11).tag=15;(_tmp790.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)2,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp790;});
break;case 348: _LL361: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 349: _LL362: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp791;(_tmp791.YY11).tag=15;(_tmp791.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)1,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp791;});
break;case 350: _LL363: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp792;(_tmp792.YY11).tag=15;(_tmp792.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)3,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp792;});
break;case 351: _LL364: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp793;(_tmp793.YY11).tag=15;(_tmp793.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)4,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp793;});
break;case 352: _LL365: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 353: _LL366: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp794;(_tmp794.YY11).tag=15;(_tmp794.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp794;});break;case 354: _LL367: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 355: _LL368: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp795;(_tmp795.YY11).tag=15;(
_tmp795.YY11).f1=Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp795;});break;case 356:
_LL369: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp796;(_tmp796.YY11).tag=
15;(_tmp796.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp796;});
break;case 357: _LL36A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp797;(_tmp797.YY11).tag=15;(_tmp797.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp797;});break;case 358: _LL36B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp798;(_tmp798.YY11).tag=15;(_tmp798.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp798;});break;case 359: _LL36C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp799;(_tmp799.YY11).tag=15;(_tmp799.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*_tmp79A=
_cycalloc(sizeof(*_tmp79A));_tmp79A[0]=({struct Cyc_Absyn_StructField_struct
_tmp79B;_tmp79B.tag=0;_tmp79B.f1=({struct _dyneither_ptr*_tmp79C=_cycalloc(
sizeof(*_tmp79C));_tmp79C[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp79C;});_tmp79B;});
_tmp79A;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp799;});break;case 360: _LL36D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp79D;(_tmp79D.YY11).tag=15;(_tmp79D.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp79E=
_cycalloc_atomic(sizeof(*_tmp79E));_tmp79E[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp79F;_tmp79F.tag=1;_tmp79F.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp79F;});_tmp79E;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp79D;});break;case 361: _LL36E: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 362: _LL36F: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 363: _LL370:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7A0;(_tmp7A0.YY11).tag=
15;(_tmp7A0.YY11).f1=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7A0;});
break;case 364: _LL371: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A1;(_tmp7A1.YY11).tag=15;(_tmp7A1.YY11).f1=Cyc_Absyn_exp_pat(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp7A1;});break;
case 365: _LL372: {struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);{void*_tmp7A2=(void*)e->r;
union Cyc_Absyn_Cnst_union _tmp7A3;void*_tmp7A4;char _tmp7A5;union Cyc_Absyn_Cnst_union
_tmp7A6;void*_tmp7A7;short _tmp7A8;union Cyc_Absyn_Cnst_union _tmp7A9;void*_tmp7AA;
int _tmp7AB;union Cyc_Absyn_Cnst_union _tmp7AC;struct _dyneither_ptr _tmp7AD;union Cyc_Absyn_Cnst_union
_tmp7AE;union Cyc_Absyn_Cnst_union _tmp7AF;union Cyc_Absyn_Cnst_union _tmp7B0;_LL375:
if(*((int*)_tmp7A2)!= 0)goto _LL377;_tmp7A3=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A2)->f1).Char_c).tag != 
0)goto _LL377;_tmp7A4=(_tmp7A3.Char_c).f1;_tmp7A5=(_tmp7A3.Char_c).f2;_LL376:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7B1;(_tmp7B1.YY11).tag=
15;(_tmp7B1.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Char_p_struct*
_tmp7B2=_cycalloc_atomic(sizeof(*_tmp7B2));_tmp7B2[0]=({struct Cyc_Absyn_Char_p_struct
_tmp7B3;_tmp7B3.tag=8;_tmp7B3.f1=_tmp7A5;_tmp7B3;});_tmp7B2;}),e->loc);_tmp7B1;});
goto _LL374;_LL377: if(*((int*)_tmp7A2)!= 0)goto _LL379;_tmp7A6=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A2)->f1).Short_c).tag != 
1)goto _LL379;_tmp7A7=(_tmp7A6.Short_c).f1;_tmp7A8=(_tmp7A6.Short_c).f2;_LL378:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7B4;(_tmp7B4.YY11).tag=
15;(_tmp7B4.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp7B5=_cycalloc(sizeof(*_tmp7B5));_tmp7B5[0]=({struct Cyc_Absyn_Int_p_struct
_tmp7B6;_tmp7B6.tag=7;_tmp7B6.f1=(void*)_tmp7A7;_tmp7B6.f2=(int)_tmp7A8;_tmp7B6;});
_tmp7B5;}),e->loc);_tmp7B4;});goto _LL374;_LL379: if(*((int*)_tmp7A2)!= 0)goto
_LL37B;_tmp7A9=((struct Cyc_Absyn_Const_e_struct*)_tmp7A2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7A2)->f1).Int_c).tag != 2)goto _LL37B;_tmp7AA=(_tmp7A9.Int_c).f1;_tmp7AB=(
_tmp7A9.Int_c).f2;_LL37A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B7;(_tmp7B7.YY11).tag=15;(_tmp7B7.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Int_p_struct*_tmp7B8=_cycalloc(sizeof(*_tmp7B8));_tmp7B8[0]=({
struct Cyc_Absyn_Int_p_struct _tmp7B9;_tmp7B9.tag=7;_tmp7B9.f1=(void*)_tmp7AA;
_tmp7B9.f2=_tmp7AB;_tmp7B9;});_tmp7B8;}),e->loc);_tmp7B7;});goto _LL374;_LL37B:
if(*((int*)_tmp7A2)!= 0)goto _LL37D;_tmp7AC=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A2)->f1).Float_c).tag != 
4)goto _LL37D;_tmp7AD=(_tmp7AC.Float_c).f1;_LL37C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7BA;(_tmp7BA.YY11).tag=15;(_tmp7BA.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Float_p_struct*_tmp7BB=_cycalloc(sizeof(*_tmp7BB));
_tmp7BB[0]=({struct Cyc_Absyn_Float_p_struct _tmp7BC;_tmp7BC.tag=9;_tmp7BC.f1=
_tmp7AD;_tmp7BC;});_tmp7BB;}),e->loc);_tmp7BA;});goto _LL374;_LL37D: if(*((int*)
_tmp7A2)!= 0)goto _LL37F;_tmp7AE=((struct Cyc_Absyn_Const_e_struct*)_tmp7A2)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A2)->f1).Null_c).tag != 6)goto _LL37F;
_LL37E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7BD;(_tmp7BD.YY11).tag=
15;(_tmp7BD.YY11).f1=Cyc_Absyn_new_pat((void*)1,e->loc);_tmp7BD;});goto _LL374;
_LL37F: if(*((int*)_tmp7A2)!= 0)goto _LL381;_tmp7AF=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A2)->f1).String_c).tag
!= 5)goto _LL381;_LL380: Cyc_Parse_err(({const char*_tmp7BE="strings cannot occur within patterns";
_tag_dyneither(_tmp7BE,sizeof(char),37);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL374;
_LL381: if(*((int*)_tmp7A2)!= 0)goto _LL383;_tmp7B0=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A2)->f1).LongLong_c).tag
!= 3)goto _LL383;_LL382: Cyc_Parse_unimp(({const char*_tmp7BF="long long's in patterns";
_tag_dyneither(_tmp7BF,sizeof(char),24);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL374;
_LL383:;_LL384: Cyc_Parse_err(({const char*_tmp7C0="bad constant in case";
_tag_dyneither(_tmp7C0,sizeof(char),21);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_LL374:;}break;}
case 366: _LL373: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7C1;(
_tmp7C1.YY11).tag=15;(_tmp7C1.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_struct*
_tmp7C2=_cycalloc(sizeof(*_tmp7C2));_tmp7C2[0]=({struct Cyc_Absyn_UnknownId_p_struct
_tmp7C3;_tmp7C3.tag=12;_tmp7C3.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7C3;});_tmp7C2;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7C1;});
break;case 367: _LL385: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp7C4="as";
_tag_dyneither(_tmp7C4,sizeof(char),3);}))!= 0)Cyc_Parse_err(({const char*_tmp7C5="expecting `as'";
_tag_dyneither(_tmp7C5,sizeof(char),15);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7C6;(_tmp7C6.YY11).tag=15;(_tmp7C6.YY11).f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_struct*_tmp7C7=_cycalloc(
sizeof(*_tmp7C7));_tmp7C7[0]=({struct Cyc_Absyn_Var_p_struct _tmp7C8;_tmp7C8.tag=0;
_tmp7C8.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7C9=_cycalloc(sizeof(*
_tmp7C9));_tmp7C9->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp7CB;(_tmp7CB.Loc_n).tag=0;_tmp7CB;});_tmp7C9->f2=({struct _dyneither_ptr*
_tmp7CA=_cycalloc(sizeof(*_tmp7CA));_tmp7CA[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7CA;});_tmp7C9;}),(
void*)0,0);_tmp7C8.f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7C8;});_tmp7C7;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp7C6;});
break;case 368: _LL386: {struct Cyc_List_List*_tmp7CD;int _tmp7CE;struct _tuple13
_tmp7CC=*Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7CD=_tmp7CC.f1;_tmp7CE=_tmp7CC.f2;yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp7CF;(_tmp7CF.YY11).tag=15;(_tmp7CF.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Tuple_p_struct*_tmp7D0=_cycalloc(sizeof(*_tmp7D0));
_tmp7D0[0]=({struct Cyc_Absyn_Tuple_p_struct _tmp7D1;_tmp7D1.tag=3;_tmp7D1.f1=
_tmp7CD;_tmp7D1.f2=_tmp7CE;_tmp7D1;});_tmp7D0;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7CF;});
break;}case 369: _LL387: {struct Cyc_List_List*_tmp7D3;int _tmp7D4;struct _tuple13
_tmp7D2=*Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7D3=_tmp7D2.f1;_tmp7D4=_tmp7D2.f2;yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp7D5;(_tmp7D5.YY11).tag=15;(_tmp7D5.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp7D6=_cycalloc(sizeof(*_tmp7D6));
_tmp7D6[0]=({struct Cyc_Absyn_UnknownCall_p_struct _tmp7D7;_tmp7D7.tag=13;_tmp7D7.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp7D7.f2=_tmp7D3;_tmp7D7.f3=_tmp7D4;_tmp7D7;});_tmp7D6;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7D5;});
break;}case 370: _LL388: {struct Cyc_List_List*_tmp7D9;int _tmp7DA;struct _tuple13
_tmp7D8=*Cyc_yyget_YY16(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7D9=_tmp7D8.f1;_tmp7DA=_tmp7D8.f2;{struct Cyc_List_List*_tmp7DB=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7DC;(_tmp7DC.YY11).tag=15;(_tmp7DC.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Aggr_p_struct*_tmp7DD=_cycalloc(sizeof(*_tmp7DD));_tmp7DD[0]=({
struct Cyc_Absyn_Aggr_p_struct _tmp7DE;_tmp7DE.tag=5;_tmp7DE.f1=({struct Cyc_Absyn_AggrInfo
_tmp7DF;_tmp7DF.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmp7E0;(_tmp7E0.UnknownAggr).tag=0;(_tmp7E0.UnknownAggr).f1=(void*)((void*)0);(
_tmp7E0.UnknownAggr).f2=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp7E0;});_tmp7DF.targs=0;_tmp7DF;});_tmp7DE.f2=
_tmp7DB;_tmp7DE.f3=_tmp7D9;_tmp7DE.f4=_tmp7DA;_tmp7DE;});_tmp7DD;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7DC;});
break;}}case 371: _LL389: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7E1;(_tmp7E1.YY11).tag=15;(_tmp7E1.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Pointer_p_struct*_tmp7E2=_cycalloc(sizeof(*_tmp7E2));_tmp7E2[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp7E3;_tmp7E3.tag=4;_tmp7E3.f1=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7E3;});_tmp7E2;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7E1;});break;case 372: _LL38A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7E4;(_tmp7E4.YY11).tag=15;(_tmp7E4.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp7E5=_cycalloc(sizeof(*_tmp7E5));
_tmp7E5[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp7E6;_tmp7E6.tag=4;_tmp7E6.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp7E7=_cycalloc(
sizeof(*_tmp7E7));_tmp7E7[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp7E8;_tmp7E8.tag=
4;_tmp7E8.f1=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7E8;});_tmp7E7;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E6;});
_tmp7E5;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7E4;});break;case 373: _LL38B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7E9;(_tmp7E9.YY11).tag=15;(_tmp7E9.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Reference_p_struct*_tmp7EA=_cycalloc(sizeof(*_tmp7EA));
_tmp7EA[0]=({struct Cyc_Absyn_Reference_p_struct _tmp7EB;_tmp7EB.tag=1;_tmp7EB.f1=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7EC=_cycalloc(sizeof(*_tmp7EC));
_tmp7EC->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp7EE;(
_tmp7EE.Loc_n).tag=0;_tmp7EE;});_tmp7EC->f2=({struct _dyneither_ptr*_tmp7ED=
_cycalloc(sizeof(*_tmp7ED));_tmp7ED[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7ED;});_tmp7EC;}),(void*)
0,0);_tmp7EB.f2=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7EB;});
_tmp7EA;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7E9;});break;case 374: _LL38C: if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),({const char*_tmp7EF="as";_tag_dyneither(_tmp7EF,sizeof(
char),3);}))!= 0)Cyc_Parse_err(({const char*_tmp7F0="expecting `as'";
_tag_dyneither(_tmp7F0,sizeof(char),15);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7F1;(_tmp7F1.YY11).tag=15;(_tmp7F1.YY11).f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp7F2=_cycalloc(
sizeof(*_tmp7F2));_tmp7F2[0]=({struct Cyc_Absyn_Reference_p_struct _tmp7F3;_tmp7F3.tag=
1;_tmp7F3.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7F4=_cycalloc(sizeof(*
_tmp7F4));_tmp7F4->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp7F6;(_tmp7F6.Loc_n).tag=0;_tmp7F6;});_tmp7F4->f2=({struct _dyneither_ptr*
_tmp7F5=_cycalloc(sizeof(*_tmp7F5));_tmp7F5[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7F5;});_tmp7F4;}),(
void*)0,0);_tmp7F3.f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7F3;});_tmp7F2;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp7F1;});
break;case 375: _LL38D: {void*_tmp7F7=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Tcutil_kind_to_bound((
void*)7));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7F8;(_tmp7F8.YY11).tag=
15;(_tmp7F8.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp7F9=_cycalloc(sizeof(*_tmp7F9));_tmp7F9[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp7FA;_tmp7FA.tag=2;_tmp7FA.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp7F7);
_tmp7FA.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7FB=_cycalloc(sizeof(*
_tmp7FB));_tmp7FB->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp7FD;(_tmp7FD.Loc_n).tag=0;_tmp7FD;});_tmp7FB->f2=({struct _dyneither_ptr*
_tmp7FC=_cycalloc(sizeof(*_tmp7FC));_tmp7FC[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7FC;});_tmp7FB;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp7FE=_cycalloc(sizeof(*_tmp7FE));
_tmp7FE[0]=({struct Cyc_Absyn_TagType_struct _tmp7FF;_tmp7FF.tag=18;_tmp7FF.f1=(
void*)_tmp7F7;_tmp7FF;});_tmp7FE;}),0);_tmp7FA;});_tmp7F9;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7F8;});
break;}case 376: _LL38E: {struct Cyc_Absyn_Tvar*_tmp800=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound((
void*)7));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp801;(_tmp801.YY11).tag=
15;(_tmp801.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp802=_cycalloc(sizeof(*_tmp802));_tmp802[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp803;_tmp803.tag=2;_tmp803.f1=_tmp800;_tmp803.f2=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp804=_cycalloc(sizeof(*_tmp804));_tmp804->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp806;(_tmp806.Loc_n).tag=0;_tmp806;});_tmp804->f2=({
struct _dyneither_ptr*_tmp805=_cycalloc(sizeof(*_tmp805));_tmp805[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp805;});_tmp804;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp807=_cycalloc(sizeof(*_tmp807));
_tmp807[0]=({struct Cyc_Absyn_TagType_struct _tmp808;_tmp808.tag=18;_tmp808.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp809=_cycalloc(sizeof(*_tmp809));
_tmp809[0]=({struct Cyc_Absyn_VarType_struct _tmp80A;_tmp80A.tag=1;_tmp80A.f1=
_tmp800;_tmp80A;});_tmp809;}));_tmp808;});_tmp807;}),0);_tmp803;});_tmp802;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp801;});break;}case 377: _LL38F: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp80B;(_tmp80B.YY12).tag=16;(_tmp80B.YY12).f1=({struct
_tuple13*_tmp80C=_cycalloc(sizeof(*_tmp80C));_tmp80C->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp80C->f2=0;_tmp80C;});
_tmp80B;});break;case 378: _LL390: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp80D;(_tmp80D.YY12).tag=16;(_tmp80D.YY12).f1=({struct _tuple13*_tmp80E=
_cycalloc(sizeof(*_tmp80E));_tmp80E->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp80E->f2=1;_tmp80E;});_tmp80D;});break;case 379: _LL391:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp80F;(_tmp80F.YY12).tag=
16;(_tmp80F.YY12).f1=({struct _tuple13*_tmp810=_cycalloc(sizeof(*_tmp810));
_tmp810->f1=0;_tmp810->f2=1;_tmp810;});_tmp80F;});break;case 380: _LL392: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp811;(_tmp811.YY13).tag=17;(
_tmp811.YY13).f1=({struct Cyc_List_List*_tmp812=_cycalloc(sizeof(*_tmp812));
_tmp812->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp812->tl=0;_tmp812;});_tmp811;});break;case 381: _LL393: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp813;(_tmp813.YY13).tag=17;(
_tmp813.YY13).f1=({struct Cyc_List_List*_tmp814=_cycalloc(sizeof(*_tmp814));
_tmp814->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp814->tl=Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp814;});_tmp813;});break;case 382: _LL394: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp815;(_tmp815.YY14).tag=18;(_tmp815.YY14).f1=({
struct _tuple14*_tmp816=_cycalloc(sizeof(*_tmp816));_tmp816->f1=0;_tmp816->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp816;});_tmp815;});
break;case 383: _LL395: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp817;(_tmp817.YY14).tag=18;(_tmp817.YY14).f1=({struct _tuple14*_tmp818=
_cycalloc(sizeof(*_tmp818));_tmp818->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp818->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp818;});_tmp817;});
break;case 384: _LL396: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp819;(_tmp819.YY16).tag=20;(_tmp819.YY16).f1=({struct _tuple13*_tmp81A=
_cycalloc(sizeof(*_tmp81A));_tmp81A->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp81A->f2=0;_tmp81A;});_tmp819;});break;case 385: _LL397: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp81B;(_tmp81B.YY16).tag=20;(
_tmp81B.YY16).f1=({struct _tuple13*_tmp81C=_cycalloc(sizeof(*_tmp81C));_tmp81C->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp81C->f2=1;_tmp81C;});
_tmp81B;});break;case 386: _LL398: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp81D;(_tmp81D.YY16).tag=20;(_tmp81D.YY16).f1=({struct _tuple13*_tmp81E=
_cycalloc(sizeof(*_tmp81E));_tmp81E->f1=0;_tmp81E->f2=1;_tmp81E;});_tmp81D;});
break;case 387: _LL399: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp81F;(_tmp81F.YY15).tag=19;(_tmp81F.YY15).f1=({struct Cyc_List_List*_tmp820=
_cycalloc(sizeof(*_tmp820));_tmp820->hd=Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp820->tl=0;_tmp820;});
_tmp81F;});break;case 388: _LL39A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp821;(_tmp821.YY15).tag=19;(_tmp821.YY15).f1=({struct Cyc_List_List*_tmp822=
_cycalloc(sizeof(*_tmp822));_tmp822->hd=Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp822->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp822;});_tmp821;});
break;case 389: _LL39B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 390: _LL39C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp823;(_tmp823.YY3).tag=7;(_tmp823.YY3).f1=Cyc_Absyn_seq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp823;});
break;case 391: _LL39D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 392: _LL39E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp824;(_tmp824.YY3).tag=7;(_tmp824.YY3).f1=Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp824;});
break;case 393: _LL39F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp825;(_tmp825.YY3).tag=7;(_tmp825.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Swap_e_struct*_tmp826=_cycalloc(sizeof(*_tmp826));_tmp826[0]=({struct
Cyc_Absyn_Swap_e_struct _tmp827;_tmp827.tag=36;_tmp827.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp827.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp827;});_tmp826;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp825;});break;case 394: _LL3A0: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp828;(_tmp828.YY8).tag=12;(_tmp828.YY8).f1=0;_tmp828;});
break;case 395: _LL3A1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp829;(_tmp829.YY8).tag=12;(_tmp829.YY8).f1=({struct Cyc_Core_Opt*_tmp82A=
_cycalloc(sizeof(*_tmp82A));_tmp82A->v=(void*)((void*)1);_tmp82A;});_tmp829;});
break;case 396: _LL3A2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp82B;(_tmp82B.YY8).tag=12;(_tmp82B.YY8).f1=({struct Cyc_Core_Opt*_tmp82C=
_cycalloc(sizeof(*_tmp82C));_tmp82C->v=(void*)((void*)3);_tmp82C;});_tmp82B;});
break;case 397: _LL3A3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp82D;(_tmp82D.YY8).tag=12;(_tmp82D.YY8).f1=({struct Cyc_Core_Opt*_tmp82E=
_cycalloc(sizeof(*_tmp82E));_tmp82E->v=(void*)((void*)4);_tmp82E;});_tmp82D;});
break;case 398: _LL3A4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp82F;(_tmp82F.YY8).tag=12;(_tmp82F.YY8).f1=({struct Cyc_Core_Opt*_tmp830=
_cycalloc(sizeof(*_tmp830));_tmp830->v=(void*)((void*)0);_tmp830;});_tmp82F;});
break;case 399: _LL3A5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp831;(_tmp831.YY8).tag=12;(_tmp831.YY8).f1=({struct Cyc_Core_Opt*_tmp832=
_cycalloc(sizeof(*_tmp832));_tmp832->v=(void*)((void*)2);_tmp832;});_tmp831;});
break;case 400: _LL3A6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp833;(_tmp833.YY8).tag=12;(_tmp833.YY8).f1=({struct Cyc_Core_Opt*_tmp834=
_cycalloc(sizeof(*_tmp834));_tmp834->v=(void*)((void*)16);_tmp834;});_tmp833;});
break;case 401: _LL3A7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp835;(_tmp835.YY8).tag=12;(_tmp835.YY8).f1=({struct Cyc_Core_Opt*_tmp836=
_cycalloc(sizeof(*_tmp836));_tmp836->v=(void*)((void*)17);_tmp836;});_tmp835;});
break;case 402: _LL3A8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp837;(_tmp837.YY8).tag=12;(_tmp837.YY8).f1=({struct Cyc_Core_Opt*_tmp838=
_cycalloc(sizeof(*_tmp838));_tmp838->v=(void*)((void*)13);_tmp838;});_tmp837;});
break;case 403: _LL3A9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp839;(_tmp839.YY8).tag=12;(_tmp839.YY8).f1=({struct Cyc_Core_Opt*_tmp83A=
_cycalloc(sizeof(*_tmp83A));_tmp83A->v=(void*)((void*)15);_tmp83A;});_tmp839;});
break;case 404: _LL3AA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83B;(_tmp83B.YY8).tag=12;(_tmp83B.YY8).f1=({struct Cyc_Core_Opt*_tmp83C=
_cycalloc(sizeof(*_tmp83C));_tmp83C->v=(void*)((void*)14);_tmp83C;});_tmp83B;});
break;case 405: _LL3AB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 406: _LL3AC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83D;(_tmp83D.YY3).tag=7;(_tmp83D.YY3).f1=Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83D;});
break;case 407: _LL3AD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83E;(_tmp83E.YY3).tag=7;(_tmp83E.YY3).f1=Cyc_Absyn_throw_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83E;});
break;case 408: _LL3AE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83F;(_tmp83F.YY3).tag=7;(_tmp83F.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp83F;});
break;case 409: _LL3AF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp840;(_tmp840.YY3).tag=7;(_tmp840.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp840;});
break;case 410: _LL3B0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp841;(_tmp841.YY3).tag=7;(_tmp841.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp841;});
break;case 411: _LL3B1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp842;(_tmp842.YY3).tag=7;(_tmp842.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp842;});
break;case 412: _LL3B2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 413: _LL3B3: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 414: _LL3B4: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp843;(_tmp843.YY3).tag=7;(_tmp843.YY3).f1=Cyc_Absyn_or_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp843;});
break;case 415: _LL3B5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 416: _LL3B6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp844;(_tmp844.YY3).tag=7;(_tmp844.YY3).f1=Cyc_Absyn_and_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp844;});
break;case 417: _LL3B7: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 418: _LL3B8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp845;(_tmp845.YY3).tag=7;(_tmp845.YY3).f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp845;});
break;case 419: _LL3B9: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 420: _LL3BA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp846;(_tmp846.YY3).tag=7;(_tmp846.YY3).f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp846;});
break;case 421: _LL3BB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 422: _LL3BC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp847;(_tmp847.YY3).tag=7;(_tmp847.YY3).f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp847;});
break;case 423: _LL3BD: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 424: _LL3BE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp848;(_tmp848.YY3).tag=7;(_tmp848.YY3).f1=Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp848;});
break;case 425: _LL3BF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp849;(_tmp849.YY3).tag=7;(_tmp849.YY3).f1=Cyc_Absyn_neq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp849;});
break;case 426: _LL3C0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 427: _LL3C1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84A;(_tmp84A.YY3).tag=7;(_tmp84A.YY3).f1=Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84A;});
break;case 428: _LL3C2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84B;(_tmp84B.YY3).tag=7;(_tmp84B.YY3).f1=Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84B;});
break;case 429: _LL3C3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84C;(_tmp84C.YY3).tag=7;(_tmp84C.YY3).f1=Cyc_Absyn_lte_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84C;});
break;case 430: _LL3C4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84D;(_tmp84D.YY3).tag=7;(_tmp84D.YY3).f1=Cyc_Absyn_gte_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84D;});
break;case 431: _LL3C5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 432: _LL3C6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84E;(_tmp84E.YY3).tag=7;(_tmp84E.YY3).f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84E;});
break;case 433: _LL3C7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84F;(_tmp84F.YY3).tag=7;(_tmp84F.YY3).f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84F;});
break;case 434: _LL3C8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 435: _LL3C9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp850;(_tmp850.YY3).tag=7;(_tmp850.YY3).f1=Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp850;});
break;case 436: _LL3CA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp851;(_tmp851.YY3).tag=7;(_tmp851.YY3).f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp851;});
break;case 437: _LL3CB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 438: _LL3CC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp852;(_tmp852.YY3).tag=7;(_tmp852.YY3).f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp852;});
break;case 439: _LL3CD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp853;(_tmp853.YY3).tag=7;(_tmp853.YY3).f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp853;});
break;case 440: _LL3CE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp854;(_tmp854.YY3).tag=7;(_tmp854.YY3).f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp854;});
break;case 441: _LL3CF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 442: _LL3D0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp855;(_tmp855.YY3).tag=7;(_tmp855.YY3).f1=Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp855;});
break;case 443: _LL3D1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 444: _LL3D2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp856;(_tmp856.YY3).tag=7;(_tmp856.YY3).f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp856;});
break;case 445: _LL3D3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp857;(_tmp857.YY3).tag=7;(_tmp857.YY3).f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp857;});
break;case 446: _LL3D4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp858;(_tmp858.YY3).tag=7;(_tmp858.YY3).f1=Cyc_Absyn_address_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp858;});
break;case 447: _LL3D5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp859;(_tmp859.YY3).tag=7;(_tmp859.YY3).f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp859;});
break;case 448: _LL3D6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 449: _LL3D7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85A;(_tmp85A.YY3).tag=7;(_tmp85A.YY3).f1=Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85A;});
break;case 450: _LL3D8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85B;(_tmp85B.YY3).tag=7;(_tmp85B.YY3).f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85B;});
break;case 451: _LL3D9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85C;(_tmp85C.YY3).tag=7;(_tmp85C.YY3).f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85C;});
break;case 452: _LL3DA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85D;(_tmp85D.YY3).tag=7;(_tmp85D.YY3).f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct
Cyc_Absyn_StructField_struct*_tmp85E=_cycalloc(sizeof(*_tmp85E));_tmp85E[0]=({
struct Cyc_Absyn_StructField_struct _tmp85F;_tmp85F.tag=0;_tmp85F.f1=({struct
_dyneither_ptr*_tmp860=_cycalloc(sizeof(*_tmp860));_tmp860[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp860;});_tmp85F;});
_tmp85E;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp85D;});break;case 453: _LL3DB: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp861;(_tmp861.YY3).tag=7;(_tmp861.YY3).f1=Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp862=_cycalloc_atomic(sizeof(*
_tmp862));_tmp862[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp863;_tmp863.tag=1;
_tmp863.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2;_tmp863;});_tmp862;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp861;});
break;case 454: _LL3DC: {struct Cyc_Position_Segment*_tmp864=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp865=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp864,Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp866;(_tmp866.YY3).tag=
7;(_tmp866.YY3).f1=Cyc_Absyn_gentyp_exp(_tmp865,(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp866;});
break;}case 455: _LL3DD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp867;(_tmp867.YY3).tag=7;(_tmp867.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Malloc_e_struct*_tmp868=_cycalloc(sizeof(*_tmp868));_tmp868[0]=({
struct Cyc_Absyn_Malloc_e_struct _tmp869;_tmp869.tag=35;_tmp869.f1=({struct Cyc_Absyn_MallocInfo
_tmp86A;_tmp86A.is_calloc=0;_tmp86A.rgn=0;_tmp86A.elt_type=0;_tmp86A.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp86A.fat_result=0;_tmp86A;});_tmp869;});_tmp868;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp867;});
break;case 456: _LL3DE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86B;(_tmp86B.YY3).tag=7;(_tmp86B.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Malloc_e_struct*_tmp86C=_cycalloc(sizeof(*_tmp86C));_tmp86C[0]=({
struct Cyc_Absyn_Malloc_e_struct _tmp86D;_tmp86D.tag=35;_tmp86D.f1=({struct Cyc_Absyn_MallocInfo
_tmp86E;_tmp86E.is_calloc=0;_tmp86E.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp86E.elt_type=0;
_tmp86E.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp86E.fat_result=0;_tmp86E;});_tmp86D;});_tmp86C;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86B;});
break;case 457: _LL3DF: {void*_tmp870;struct _tuple2 _tmp86F=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp870=_tmp86F.f3;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp871;(_tmp871.YY3).tag=7;(
_tmp871.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp872=_cycalloc(sizeof(*_tmp872));_tmp872[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp873;_tmp873.tag=35;_tmp873.f1=({struct Cyc_Absyn_MallocInfo _tmp874;_tmp874.is_calloc=
1;_tmp874.rgn=0;_tmp874.elt_type=({void**_tmp875=_cycalloc(sizeof(*_tmp875));
_tmp875[0]=_tmp870;_tmp875;});_tmp874.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp874.fat_result=0;
_tmp874;});_tmp873;});_tmp872;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp871;});
break;}case 458: _LL3E0: {void*_tmp877;struct _tuple2 _tmp876=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp877=_tmp876.f3;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp878;(_tmp878.YY3).tag=7;(
_tmp878.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp879=_cycalloc(sizeof(*_tmp879));_tmp879[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp87A;_tmp87A.tag=35;_tmp87A.f1=({struct Cyc_Absyn_MallocInfo _tmp87B;_tmp87B.is_calloc=
1;_tmp87B.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);_tmp87B.elt_type=({void**
_tmp87C=_cycalloc(sizeof(*_tmp87C));_tmp87C[0]=_tmp877;_tmp87C;});_tmp87B.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp87B.fat_result=0;_tmp87B;});_tmp87A;});_tmp879;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp878;});
break;}case 459: _LL3E1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp87D;(_tmp87D.YY3).tag=7;(_tmp87D.YY3).f1=Cyc_Absyn_primop_exp((void*)19,({
struct Cyc_Absyn_Exp*_tmp87E[1];_tmp87E[0]=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp87E,sizeof(struct Cyc_Absyn_Exp*),
1));}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp87D;});break;case 460: _LL3E2: {void*_tmp880;
struct _tuple2 _tmp87F=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp880=_tmp87F.f3;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp881;(_tmp881.YY3).tag=7;(_tmp881.YY3).f1=Cyc_Absyn_valueof_exp(_tmp880,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp881;});
break;}case 461: _LL3E3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp882;(_tmp882.YY7).tag=11;(_tmp882.YY7).f1=(void*)((void*)12);_tmp882;});
break;case 462: _LL3E4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp883;(_tmp883.YY7).tag=11;(_tmp883.YY7).f1=(void*)((void*)11);_tmp883;});
break;case 463: _LL3E5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp884;(_tmp884.YY7).tag=11;(_tmp884.YY7).f1=(void*)((void*)2);_tmp884;});
break;case 464: _LL3E6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 465: _LL3E7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp885;(_tmp885.YY3).tag=7;(_tmp885.YY3).f1=Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp885;});
break;case 466: _LL3E8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp886;(_tmp886.YY3).tag=7;(_tmp886.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp886;});
break;case 467: _LL3E9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp887;(_tmp887.YY3).tag=7;(_tmp887.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp887;});
break;case 468: _LL3EA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp888;(_tmp888.YY3).tag=7;(_tmp888.YY3).f1=Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct
_dyneither_ptr*_tmp889=_cycalloc(sizeof(*_tmp889));_tmp889[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp889;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp888;});
break;case 469: _LL3EB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88A;(_tmp88A.YY3).tag=7;(_tmp88A.YY3).f1=Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct
_dyneither_ptr*_tmp88B=_cycalloc(sizeof(*_tmp88B));_tmp88B[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp88B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88A;});
break;case 470: _LL3EC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88C;(_tmp88C.YY3).tag=7;(_tmp88C.YY3).f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88C;});
break;case 471: _LL3ED: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88D;(_tmp88D.YY3).tag=7;(_tmp88D.YY3).f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp88D;});
break;case 472: _LL3EE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88E;(_tmp88E.YY3).tag=7;(_tmp88E.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_CompoundLit_e_struct*_tmp88F=_cycalloc(sizeof(*_tmp88F));_tmp88F[0]=({
struct Cyc_Absyn_CompoundLit_e_struct _tmp890;_tmp890.tag=27;_tmp890.f1=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp890.f2=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp890;});_tmp88F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp88E;});break;case 473: _LL3EF: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp891;(_tmp891.YY3).tag=7;(_tmp891.YY3).f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp892=_cycalloc(sizeof(*_tmp892));
_tmp892[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp893;_tmp893.tag=27;_tmp893.f1=
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
_tmp893.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp893;});_tmp892;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp891;});
break;case 474: _LL3F0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp894;(_tmp894.YY3).tag=7;(_tmp894.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnknownId_e_struct*_tmp895=_cycalloc(sizeof(*_tmp895));_tmp895[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp896;_tmp896.tag=2;_tmp896.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp896;});_tmp895;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp894;});break;case 475: _LL3F1: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 476: _LL3F2: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp897;(_tmp897.YY3).tag=7;(
_tmp897.YY3).f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp897;});
break;case 477: _LL3F3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 478: _LL3F4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp898;(_tmp898.YY3).tag=7;(_tmp898.YY3).f1=Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp898;});
break;case 479: _LL3F5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp899;(_tmp899.YY3).tag=7;(_tmp899.YY3).f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp899;});
break;case 480: _LL3F6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp89A;(_tmp89A.YY3).tag=7;(_tmp89A.YY3).f1=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89A;});
break;case 481: _LL3F7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp89B;(_tmp89B.YY3).tag=7;(_tmp89B.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Aggregate_e_struct*_tmp89C=_cycalloc(sizeof(*_tmp89C));_tmp89C[0]=({
struct Cyc_Absyn_Aggregate_e_struct _tmp89D;_tmp89D.tag=30;_tmp89D.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp89D.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp89D.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp89D.f4=0;_tmp89D;});
_tmp89C;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp89B;});break;case 482: _LL3F8: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp89E;(_tmp89E.YY3).tag=7;(_tmp89E.YY3).f1=Cyc_Absyn_stmt_exp(
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89E;});
break;case 483: _LL3F9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp89F;(_tmp89F.YY5).tag=9;(_tmp89F.YY5).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp89F;});break;case 484: _LL3FA: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp8A0;(_tmp8A0.YY5).tag=9;(_tmp8A0.YY5).f1=({struct Cyc_List_List*
_tmp8A1=_cycalloc(sizeof(*_tmp8A1));_tmp8A1->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8A1->tl=0;_tmp8A1;});
_tmp8A0;});break;case 485: _LL3FB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A2;(_tmp8A2.YY5).tag=9;(_tmp8A2.YY5).f1=({struct Cyc_List_List*_tmp8A3=
_cycalloc(sizeof(*_tmp8A3));_tmp8A3->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8A3->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8A3;});_tmp8A2;});
break;case 486: _LL3FC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A4;(_tmp8A4.YY3).tag=7;(_tmp8A4.YY3).f1=Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A4;});
break;case 487: _LL3FD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A5;(_tmp8A5.YY3).tag=7;(_tmp8A5.YY3).f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A5;});
break;case 488: _LL3FE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A6;(_tmp8A6.YY3).tag=7;(_tmp8A6.YY3).f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A6;});
break;case 489: _LL3FF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A7;(_tmp8A7.YY3).tag=7;(_tmp8A7.YY3).f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A7;});
break;case 490: _LL400: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A8;(_tmp8A8.QualId_tok).tag=4;(_tmp8A8.QualId_tok).f1=({struct _tuple1*
_tmp8A9=_cycalloc(sizeof(*_tmp8A9));_tmp8A9->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp8AB;(_tmp8AB.Rel_n).tag=1;(_tmp8AB.Rel_n).f1=0;
_tmp8AB;});_tmp8A9->f2=({struct _dyneither_ptr*_tmp8AA=_cycalloc(sizeof(*_tmp8AA));
_tmp8AA[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp8AA;});_tmp8A9;});_tmp8A8;});break;case 491: _LL401: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 492: _LL402: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp8AC;(_tmp8AC.QualId_tok).tag=
4;(_tmp8AC.QualId_tok).f1=({struct _tuple1*_tmp8AD=_cycalloc(sizeof(*_tmp8AD));
_tmp8AD->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp8AF;(
_tmp8AF.Rel_n).tag=1;(_tmp8AF.Rel_n).f1=0;_tmp8AF;});_tmp8AD->f2=({struct
_dyneither_ptr*_tmp8AE=_cycalloc(sizeof(*_tmp8AE));_tmp8AE[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8AE;});_tmp8AD;});
_tmp8AC;});break;case 493: _LL403: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 494: _LL404: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 495: _LL405: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 496: _LL406: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 497: _LL407:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 498:
_LL408: break;case 499: _LL409: yylex_buf->lex_curr_pos -=1;break;default: _LL40A:
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(500,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(142,yyn - 138)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
6016) && Cyc_yycheck[_check_known_subscript_notnull(6017,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(6017,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(142,yyn - 138)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1008,yystate)];if(yyn > - 32768  && yyn < 6016){int
sze=0;struct _dyneither_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 280 / 
sizeof(char*);++ x){if(Cyc_yycheck[_check_known_subscript_notnull(6017,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(280,x)])+ 15,count ++);}msg=({unsigned int _tmp8B0=(
unsigned int)(sze + 15);char*_tmp8B1=(char*)_region_malloc(yyregion,_check_times(
sizeof(char),_tmp8B0 + 1));struct _dyneither_ptr _tmp8B3=_tag_dyneither(_tmp8B1,
sizeof(char),_tmp8B0 + 1);{unsigned int _tmp8B2=_tmp8B0;unsigned int i;for(i=0;i < 
_tmp8B2;i ++){_tmp8B1[i]='\000';}_tmp8B1[_tmp8B2]=(char)0;}_tmp8B3;});Cyc_strcpy(
msg,({const char*_tmp8B4="parse error";_tag_dyneither(_tmp8B4,sizeof(char),12);}));
if(count < 5){count=0;for(x=yyn < 0?- yyn: 0;x < 280 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(6017,x + yyn)]== x){Cyc_strcat(msg,(struct
_dyneither_ptr)(count == 0?(struct _dyneither_ptr)({const char*_tmp8B5=", expecting `";
_tag_dyneither(_tmp8B5,sizeof(char),14);}):(struct _dyneither_ptr)({const char*
_tmp8B6=" or `";_tag_dyneither(_tmp8B6,sizeof(char),6);})));Cyc_strcat(msg,(
struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(280,x)]);Cyc_strcat(
msg,({const char*_tmp8B7="'";_tag_dyneither(_tmp8B7,sizeof(char),2);}));++ count;}}}
Cyc_yyerror((struct _dyneither_ptr)msg);}else{Cyc_yyerror(({const char*_tmp8B8="parse error";
_tag_dyneither(_tmp8B8,sizeof(char),12);}));}}goto yyerrlab1;yyerrlab1: if(
yyerrstatus == 3){if(Cyc_yychar == 0){int _tmp8B9=1;_npop_handler(0);return _tmp8B9;}
Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(
yyssp_offset == 0){int _tmp8BA=1;_npop_handler(0);return _tmp8BA;}-- yyvsp_offset;
yystate=(int)yyss[_check_known_subscript_notnull(10000,-- yyssp_offset)];--
yylsp_offset;yyerrhandle: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1008,
yystate)];if(yyn == - 32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6016) || 
Cyc_yycheck[_check_known_subscript_notnull(6017,yyn)]!= 1)goto yyerrdefault;yyn=(
int)Cyc_yytable[_check_known_subscript_notnull(6017,yyn)];if(yyn < 0){if(yyn == -
32768)goto yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 
1007){int _tmp8BB=0;_npop_handler(0);return _tmp8BB;}yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto yynewstate;};
_pop_region(yyregion);}void Cyc_yyprint(int i,union Cyc_YYSTYPE_union v){union Cyc_YYSTYPE_union
_tmp8C5=v;struct _tuple11 _tmp8C6;int _tmp8C7;char _tmp8C8;struct _dyneither_ptr
_tmp8C9;struct _tuple1*_tmp8CA;struct _tuple1 _tmp8CB;union Cyc_Absyn_Nmspace_union
_tmp8CC;struct _dyneither_ptr*_tmp8CD;_LL40D: if((_tmp8C5.Int_tok).tag != 0)goto
_LL40F;_tmp8C6=(_tmp8C5.Int_tok).f1;_tmp8C7=_tmp8C6.f2;_LL40E:({struct Cyc_Int_pa_struct
_tmp8D0;_tmp8D0.tag=1;_tmp8D0.f1=(unsigned long)_tmp8C7;{void*_tmp8CE[1]={&
_tmp8D0};Cyc_fprintf(Cyc_stderr,({const char*_tmp8CF="%d";_tag_dyneither(_tmp8CF,
sizeof(char),3);}),_tag_dyneither(_tmp8CE,sizeof(void*),1));}});goto _LL40C;
_LL40F: if((_tmp8C5.Char_tok).tag != 1)goto _LL411;_tmp8C8=(_tmp8C5.Char_tok).f1;
_LL410:({struct Cyc_Int_pa_struct _tmp8D3;_tmp8D3.tag=1;_tmp8D3.f1=(unsigned long)((
int)_tmp8C8);{void*_tmp8D1[1]={& _tmp8D3};Cyc_fprintf(Cyc_stderr,({const char*
_tmp8D2="%c";_tag_dyneither(_tmp8D2,sizeof(char),3);}),_tag_dyneither(_tmp8D1,
sizeof(void*),1));}});goto _LL40C;_LL411: if((_tmp8C5.String_tok).tag != 2)goto
_LL413;_tmp8C9=(_tmp8C5.String_tok).f1;_LL412:({struct Cyc_String_pa_struct
_tmp8D6;_tmp8D6.tag=0;_tmp8D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp8C9);{void*_tmp8D4[1]={& _tmp8D6};Cyc_fprintf(Cyc_stderr,({const char*_tmp8D5="\"%s\"";
_tag_dyneither(_tmp8D5,sizeof(char),5);}),_tag_dyneither(_tmp8D4,sizeof(void*),1));}});
goto _LL40C;_LL413: if((_tmp8C5.QualId_tok).tag != 4)goto _LL415;_tmp8CA=(_tmp8C5.QualId_tok).f1;
_tmp8CB=*_tmp8CA;_tmp8CC=_tmp8CB.f1;_tmp8CD=_tmp8CB.f2;_LL414: {struct Cyc_List_List*
_tmp8D7=0;{union Cyc_Absyn_Nmspace_union _tmp8D8=_tmp8CC;struct Cyc_List_List*
_tmp8D9;struct Cyc_List_List*_tmp8DA;_LL418: if((_tmp8D8.Rel_n).tag != 1)goto _LL41A;
_tmp8D9=(_tmp8D8.Rel_n).f1;_LL419: _tmp8D7=_tmp8D9;goto _LL417;_LL41A: if((_tmp8D8.Abs_n).tag
!= 2)goto _LL41C;_tmp8DA=(_tmp8D8.Abs_n).f1;_LL41B: _tmp8D7=_tmp8DA;goto _LL417;
_LL41C: if((_tmp8D8.Loc_n).tag != 0)goto _LL417;_LL41D: goto _LL417;_LL417:;}for(0;
_tmp8D7 != 0;_tmp8D7=_tmp8D7->tl){({struct Cyc_String_pa_struct _tmp8DD;_tmp8DD.tag=
0;_tmp8DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)_tmp8D7->hd));{void*_tmp8DB[1]={& _tmp8DD};Cyc_fprintf(Cyc_stderr,({
const char*_tmp8DC="%s::";_tag_dyneither(_tmp8DC,sizeof(char),5);}),
_tag_dyneither(_tmp8DB,sizeof(void*),1));}});}({struct Cyc_String_pa_struct
_tmp8E0;_tmp8E0.tag=0;_tmp8E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp8CD);{void*_tmp8DE[1]={& _tmp8E0};Cyc_fprintf(Cyc_stderr,({const char*_tmp8DF="%s::";
_tag_dyneither(_tmp8DF,sizeof(char),5);}),_tag_dyneither(_tmp8DE,sizeof(void*),1));}});
goto _LL40C;}_LL415:;_LL416:({void*_tmp8E1=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp8E2="?";_tag_dyneither(_tmp8E2,sizeof(char),2);}),_tag_dyneither(_tmp8E1,
sizeof(void*),0));});goto _LL40C;_LL40C:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(
f));return Cyc_Parse_parse_result;}
