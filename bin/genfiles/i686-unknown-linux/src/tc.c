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
extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void Cyc_List_iter_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_List_List*Cyc_List_filter_c(
int(*f)(void*,void*),void*env,struct Cyc_List_List*x);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dyneither_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};
extern char Cyc_Position_Nocontext[14];struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct
_dyneither_ptr*f2;};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int
q_restrict;int real_const;struct Cyc_Position_Segment*loc;};struct
_union_Constraint_Eq_constr{int tag;void*val;};struct
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
struct _tuple0*,struct _tuple0*);struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
void*Cyc_Absyn_string_typ(void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);typedef struct{int __count;
union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{
long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;
struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*
Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*
Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];int Cyc_Dict_is_empty(
struct Cyc_Dict_Dict d);int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict d);struct _tuple9{void*f1;void*f2;};struct _tuple9*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple9*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(
struct _RegionHandle*,int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
le;int allow_valueof;};struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*);struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_new_fenv(struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*,struct Cyc_List_List*);struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple0*);int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*
te);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void
Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int
Cyc_Tcutil_is_function_type(void*t);void*Cyc_Tcutil_kind_to_bound(void*k);void
Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*
k,int allow_evars,void*);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,
struct Cyc_List_List*);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*,void*t);void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_supports_default(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple9*eq;int isTrue;
};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part);void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Stmt*s,int new_block);struct _tuple10{unsigned int f1;int f2;};
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);void Cyc_Tc_tc(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*
ds);struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Enumdecl*);extern char Cyc_Tcdecl_Incompatible[17];struct Cyc_Tcdecl_Xdatatypefielddecl{
struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};struct
Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct
Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*
d0,struct Cyc_Absyn_Datatypedecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg);struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*
d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg);void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(
struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Tcgenrep_RepInfo;struct Cyc_Dict_Dict Cyc_Tcgenrep_empty_typerep_dict();
struct _tuple11{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Exp*
f3;};struct _tuple11 Cyc_Tcgenrep_tcGenrep(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*
ge,struct Cyc_Position_Segment*loc,void*type,struct Cyc_Dict_Dict dict);static char
_tmp0[1]="";static struct _dyneither_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};
static struct _dyneither_ptr*Cyc_Tc_tc_msg=(struct _dyneither_ptr*)& Cyc_Tc_tc_msg_c;
struct _tuple12{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};static int
Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){for(0;exports
!= 0;exports=exports->tl){struct _tuple12*_tmp1=(struct _tuple12*)exports->hd;if(
Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0){(*_tmp1).f3=1;return 1;}}return 0;}static
void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int ignore_init,struct Cyc_List_List**
exports){struct Cyc_Absyn_Vardecl _tmp3;void*_tmp4;struct _tuple0*_tmp5;struct
_tuple0 _tmp6;union Cyc_Absyn_Nmspace _tmp7;struct _dyneither_ptr*_tmp8;void*_tmp9;
struct Cyc_Absyn_Exp*_tmpA;struct Cyc_List_List*_tmpB;struct Cyc_Absyn_Vardecl*
_tmp2=vd;_tmp3=*_tmp2;_tmp4=_tmp3.sc;_tmp5=_tmp3.name;_tmp6=*_tmp5;_tmp7=_tmp6.f1;
_tmp8=_tmp6.f2;_tmp9=_tmp3.type;_tmpA=_tmp3.initializer;_tmpB=_tmp3.attributes;{
union Cyc_Absyn_Nmspace _tmpC=_tmp7;struct Cyc_List_List*_tmpD;struct Cyc_List_List*
_tmpE;_LL1: if((_tmpC.Rel_n).tag != 1)goto _LL3;_tmpD=(struct Cyc_List_List*)(_tmpC.Rel_n).val;
if(_tmpD != 0)goto _LL3;_LL2: goto _LL4;_LL3: if((_tmpC.Abs_n).tag != 2)goto _LL5;_tmpE=(
struct Cyc_List_List*)(_tmpC.Abs_n).val;if(_tmpE != 0)goto _LL5;_LL4: goto _LL0;_LL5:;
_LL6:({struct Cyc_String_pa_struct _tmp11;_tmp11.tag=0;_tmp11.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));{void*
_tmpF[1]={& _tmp11};Cyc_Tcutil_terr(loc,({const char*_tmp10="qualified variable declarations are not implemented (%s)";
_tag_dyneither(_tmp10,sizeof(char),57);}),_tag_dyneither(_tmpF,sizeof(void*),1));}});
return;_LL0:;}(*vd->name).f1=Cyc_Absyn_Abs_n(te->ns);{void*_tmp12=Cyc_Tcutil_compress(
_tmp9);struct Cyc_Absyn_ArrayInfo _tmp13;void*_tmp14;struct Cyc_Absyn_Tqual _tmp15;
struct Cyc_Absyn_Exp*_tmp16;union Cyc_Absyn_Constraint*_tmp17;struct Cyc_Position_Segment*
_tmp18;_LL8: if(_tmp12 <= (void*)4)goto _LLA;if(*((int*)_tmp12)!= 7)goto _LLA;_tmp13=((
struct Cyc_Absyn_ArrayType_struct*)_tmp12)->f1;_tmp14=_tmp13.elt_type;_tmp15=
_tmp13.tq;_tmp16=_tmp13.num_elts;if(_tmp16 != 0)goto _LLA;_tmp17=_tmp13.zero_term;
_tmp18=_tmp13.zt_loc;if(!(_tmpA != 0))goto _LLA;_LL9:{void*_tmp19=_tmpA->r;union
Cyc_Absyn_Cnst _tmp1A;struct _dyneither_ptr _tmp1B;struct Cyc_Absyn_Exp*_tmp1C;
struct Cyc_List_List*_tmp1D;struct Cyc_List_List*_tmp1E;_LLD: if(*((int*)_tmp19)!= 
0)goto _LLF;_tmp1A=((struct Cyc_Absyn_Const_e_struct*)_tmp19)->f1;if((_tmp1A.String_c).tag
!= 7)goto _LLF;_tmp1B=(struct _dyneither_ptr)(_tmp1A.String_c).val;_LLE: _tmp9=(vd->type=
Cyc_Absyn_array_typ(_tmp14,_tmp15,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
_get_dyneither_size(_tmp1B,sizeof(char)),0),_tmp17,_tmp18));goto _LLC;_LLF: if(*((
int*)_tmp19)!= 29)goto _LL11;_tmp1C=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp19)->f2;_LL10: _tmp9=(vd->type=Cyc_Absyn_array_typ(_tmp14,_tmp15,(struct Cyc_Absyn_Exp*)
_tmp1C,_tmp17,_tmp18));goto _LLC;_LL11: if(*((int*)_tmp19)!= 37)goto _LL13;_tmp1D=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp19)->f2;_LL12: _tmp1E=_tmp1D;goto
_LL14;_LL13: if(*((int*)_tmp19)!= 28)goto _LL15;_tmp1E=((struct Cyc_Absyn_Array_e_struct*)
_tmp19)->f1;_LL14: _tmp9=(vd->type=Cyc_Absyn_array_typ(_tmp14,_tmp15,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp1E),0),_tmp17,_tmp18));goto _LLC;_LL15:;_LL16: goto _LLC;_LLC:;}goto _LL7;_LLA:;
_LLB: goto _LL7;_LL7:;}Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp9);(vd->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,_tmp9);{void*
_tmp1F=Cyc_Tcutil_compress(_tmp9);_LL18: if(_tmp1F <= (void*)4)goto _LL1A;if(*((int*)
_tmp1F)!= 7)goto _LL1A;_LL19: vd->escapes=0;goto _LL17;_LL1A:;_LL1B: vd->escapes=1;
goto _LL17;_LL17:;}if(Cyc_Tcutil_is_function_type(_tmp9))_tmpB=Cyc_Tcutil_transfer_fn_type_atts(
_tmp9,_tmpB);if(_tmp4 == (void*)3  || _tmp4 == (void*)4){if(_tmpA != 0)({void*_tmp20=
0;Cyc_Tcutil_terr(loc,({const char*_tmp21="extern declaration should not have initializer";
_tag_dyneither(_tmp21,sizeof(char),47);}),_tag_dyneither(_tmp20,sizeof(void*),0));});}
else{if(!Cyc_Tcutil_is_function_type(_tmp9)){for(0;_tmpB != 0;_tmpB=_tmpB->tl){
void*_tmp22=(void*)_tmpB->hd;_LL1D: if(_tmp22 <= (void*)17)goto _LL21;if(*((int*)
_tmp22)!= 1)goto _LL1F;_LL1E: goto _LL20;_LL1F: if(*((int*)_tmp22)!= 2)goto _LL21;
_LL20: goto _LL22;_LL21: if((int)_tmp22 != 6)goto _LL23;_LL22: goto _LL24;_LL23: if((int)
_tmp22 != 7)goto _LL25;_LL24: goto _LL26;_LL25: if((int)_tmp22 != 8)goto _LL27;_LL26:
goto _LL28;_LL27: if((int)_tmp22 != 9)goto _LL29;_LL28: goto _LL2A;_LL29: if((int)
_tmp22 != 10)goto _LL2B;_LL2A: goto _LL2C;_LL2B: if((int)_tmp22 != 11)goto _LL2D;_LL2C:
continue;_LL2D:;_LL2E:({struct Cyc_String_pa_struct _tmp26;_tmp26.tag=0;_tmp26.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));{
struct Cyc_String_pa_struct _tmp25;_tmp25.tag=0;_tmp25.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmpB->hd));{void*_tmp23[
2]={& _tmp25,& _tmp26};Cyc_Tcutil_terr(loc,({const char*_tmp24="bad attribute %s for variable %s";
_tag_dyneither(_tmp24,sizeof(char),33);}),_tag_dyneither(_tmp23,sizeof(void*),2));}}});
goto _LL1C;_LL1C:;}if(_tmpA == 0  || ignore_init){if(check_var_init  && !Cyc_Tcutil_supports_default(
_tmp9))({struct Cyc_String_pa_struct _tmp2A;_tmp2A.tag=0;_tmp2A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp9));{struct Cyc_String_pa_struct
_tmp29;_tmp29.tag=0;_tmp29.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
vd->name));{void*_tmp27[2]={& _tmp29,& _tmp2A};Cyc_Tcutil_terr(loc,({const char*
_tmp28="initializer required for variable %s of type %s";_tag_dyneither(_tmp28,
sizeof(char),48);}),_tag_dyneither(_tmp27,sizeof(void*),2));}}});}else{struct Cyc_Absyn_Exp*
_tmp2B=(struct Cyc_Absyn_Exp*)_tmpA;void*_tmp2C=Cyc_Tcexp_tcExpInitializer(te,(
void**)& _tmp9,_tmp2B);if(!Cyc_Tcutil_coerce_assign(te,_tmp2B,_tmp9)){({struct Cyc_String_pa_struct
_tmp31;_tmp31.tag=0;_tmp31.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp2C));{struct Cyc_String_pa_struct _tmp30;_tmp30.tag=0;_tmp30.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp9));{struct Cyc_String_pa_struct
_tmp2F;_tmp2F.tag=0;_tmp2F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
vd->name));{void*_tmp2D[3]={& _tmp2F,& _tmp30,& _tmp31};Cyc_Tcutil_terr(loc,({const
char*_tmp2E="%s declared with type \n%s\n but initialized with type \n%s";
_tag_dyneither(_tmp2E,sizeof(char),57);}),_tag_dyneither(_tmp2D,sizeof(void*),3));}}}});
Cyc_Tcutil_explain_failure();}if(!Cyc_Tcutil_is_const_exp(te,_tmp2B))({void*
_tmp32=0;Cyc_Tcutil_terr(loc,({const char*_tmp33="initializer is not a constant expression";
_tag_dyneither(_tmp33,sizeof(char),41);}),_tag_dyneither(_tmp32,sizeof(void*),0));});}}
else{for(0;_tmpB != 0;_tmpB=_tmpB->tl){void*_tmp34=(void*)_tmpB->hd;_LL30: if(
_tmp34 <= (void*)17)goto _LL32;if(*((int*)_tmp34)!= 0)goto _LL32;_LL31: goto _LL33;
_LL32: if((int)_tmp34 != 0)goto _LL34;_LL33: goto _LL35;_LL34: if((int)_tmp34 != 1)goto
_LL36;_LL35: goto _LL37;_LL36: if((int)_tmp34 != 2)goto _LL38;_LL37: goto _LL39;_LL38:
if((int)_tmp34 != 3)goto _LL3A;_LL39: goto _LL3B;_LL3A: if(_tmp34 <= (void*)17)goto
_LL3E;if(*((int*)_tmp34)!= 3)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if(*((int*)_tmp34)
!= 4)goto _LL3E;_LL3D: goto _LL3F;_LL3E: if((int)_tmp34 != 16)goto _LL40;_LL3F: goto
_LL41;_LL40: if((int)_tmp34 != 4)goto _LL42;_LL41:({void*_tmp35=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp36="tcVardecl: fn type atts in function var decl";
_tag_dyneither(_tmp36,sizeof(char),45);}),_tag_dyneither(_tmp35,sizeof(void*),0));});
_LL42: if(_tmp34 <= (void*)17)goto _LL44;if(*((int*)_tmp34)!= 1)goto _LL44;_LL43:
goto _LL45;_LL44: if((int)_tmp34 != 5)goto _LL46;_LL45:({struct Cyc_String_pa_struct
_tmp39;_tmp39.tag=0;_tmp39.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmpB->hd));{void*_tmp37[1]={& _tmp39};Cyc_Tcutil_terr(loc,({const char*
_tmp38="bad attribute %s in function declaration";_tag_dyneither(_tmp38,sizeof(
char),41);}),_tag_dyneither(_tmp37,sizeof(void*),1));}});goto _LL2F;_LL46:;_LL47:
continue;_LL2F:;}}}{struct _handler_cons _tmp3A;_push_handler(& _tmp3A);{int _tmp3C=
0;if(setjmp(_tmp3A.handler))_tmp3C=1;if(!_tmp3C){{struct _tuple5*_tmp3D=((struct
_tuple5*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,
_tmp8);void*_tmp3E=(*_tmp3D).f1;void*_tmp3F;_LL49: if(*((int*)_tmp3E)!= 0)goto
_LL4B;_tmp3F=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp3E)->f1;_LL4A: {struct
Cyc_Absyn_Global_b_struct*_tmp40=({struct Cyc_Absyn_Global_b_struct*_tmp45=
_cycalloc(sizeof(*_tmp45));_tmp45[0]=({struct Cyc_Absyn_Global_b_struct _tmp46;
_tmp46.tag=0;_tmp46.f1=vd;_tmp46;});_tmp45;});void*_tmp41=Cyc_Tcdecl_merge_binding(
_tmp3F,(void*)_tmp40,loc,Cyc_Tc_tc_msg);if(_tmp41 == (void*)0){_npop_handler(0);
return;}if(_tmp41 == _tmp3F  && (*_tmp3D).f2){_npop_handler(0);return;}if(exports
== 0  || Cyc_Tc_export_member(vd->name,*exports))ge->ordinaries=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple5*v))Cyc_Dict_insert)(ge->ordinaries,
_tmp8,(struct _tuple5*)({struct _tuple5*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->f1=(
void*)({struct Cyc_Tcenv_VarRes_struct*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43[0]=({
struct Cyc_Tcenv_VarRes_struct _tmp44;_tmp44.tag=0;_tmp44.f1=(void*)_tmp41;_tmp44;});
_tmp43;});_tmp42->f2=1;_tmp42;}));_npop_handler(0);return;}_LL4B: if(*((int*)
_tmp3E)!= 1)goto _LL4D;_LL4C:({void*_tmp47=0;Cyc_Tcutil_warn(loc,({const char*
_tmp48="variable declaration shadows previous struct declaration";_tag_dyneither(
_tmp48,sizeof(char),57);}),_tag_dyneither(_tmp47,sizeof(void*),0));});goto _LL48;
_LL4D: if(*((int*)_tmp3E)!= 2)goto _LL4F;_LL4E:({void*_tmp49=0;Cyc_Tcutil_warn(loc,({
const char*_tmp4A="variable declaration shadows previous datatype constructor";
_tag_dyneither(_tmp4A,sizeof(char),59);}),_tag_dyneither(_tmp49,sizeof(void*),0));});
goto _LL48;_LL4F: if(*((int*)_tmp3E)!= 4)goto _LL51;_LL50: goto _LL52;_LL51: if(*((int*)
_tmp3E)!= 3)goto _LL48;_LL52:({void*_tmp4B=0;Cyc_Tcutil_warn(loc,({const char*
_tmp4C="variable declaration shadows previous enum tag";_tag_dyneither(_tmp4C,
sizeof(char),47);}),_tag_dyneither(_tmp4B,sizeof(void*),0));});goto _LL48;_LL48:;};
_pop_handler();}else{void*_tmp3B=(void*)_exn_thrown;void*_tmp4E=_tmp3B;_LL54: if(
_tmp4E != Cyc_Dict_Absent)goto _LL56;_LL55: goto _LL53;_LL56:;_LL57:(void)_throw(
_tmp4E);_LL53:;}}}if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports))ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple5*
v))Cyc_Dict_insert)(ge->ordinaries,_tmp8,(struct _tuple5*)({struct _tuple5*_tmp4F=
_cycalloc(sizeof(*_tmp4F));_tmp4F->f1=(void*)({struct Cyc_Tcenv_VarRes_struct*
_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50[0]=({struct Cyc_Tcenv_VarRes_struct
_tmp51;_tmp51.tag=0;_tmp51.f1=(void*)((void*)({struct Cyc_Absyn_Global_b_struct*
_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52[0]=({struct Cyc_Absyn_Global_b_struct
_tmp53;_tmp53.tag=0;_tmp53.f1=vd;_tmp53;});_tmp52;}));_tmp51;});_tmp50;});_tmp4F->f2=
0;_tmp4F;}));}static int Cyc_Tc_is_main(struct _tuple0*n){struct _tuple0 _tmp55;union
Cyc_Absyn_Nmspace _tmp56;struct _dyneither_ptr*_tmp57;struct _tuple0*_tmp54=n;
_tmp55=*_tmp54;_tmp56=_tmp55.f1;_tmp57=_tmp55.f2;{union Cyc_Absyn_Nmspace _tmp58=
_tmp56;struct Cyc_List_List*_tmp59;_LL59: if((_tmp58.Abs_n).tag != 2)goto _LL5B;
_tmp59=(struct Cyc_List_List*)(_tmp58.Abs_n).val;if(_tmp59 != 0)goto _LL5B;_LL5A:
return Cyc_strcmp((struct _dyneither_ptr)*_tmp57,({const char*_tmp5A="main";
_tag_dyneither(_tmp5A,sizeof(char),5);}))== 0;_LL5B:;_LL5C: return 0;_LL58:;}}
struct _tuple13{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static
void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Fndecl*fd,int ignore_body,struct Cyc_List_List**exports){
struct _dyneither_ptr*v=(*fd->name).f2;if(fd->sc == (void*)4  && !ignore_body)({
void*_tmp5B=0;Cyc_Tcutil_terr(loc,({const char*_tmp5C="extern \"C\" functions cannot be implemented in Cyclone";
_tag_dyneither(_tmp5C,sizeof(char),54);}),_tag_dyneither(_tmp5B,sizeof(void*),0));});{
union Cyc_Absyn_Nmspace _tmp5D=(*fd->name).f1;struct Cyc_List_List*_tmp5E;struct Cyc_List_List*
_tmp5F;_LL5E: if((_tmp5D.Rel_n).tag != 1)goto _LL60;_tmp5E=(struct Cyc_List_List*)(
_tmp5D.Rel_n).val;if(_tmp5E != 0)goto _LL60;_LL5F: goto _LL5D;_LL60: if((_tmp5D.Abs_n).tag
!= 2)goto _LL62;_tmp5F=(struct Cyc_List_List*)(_tmp5D.Abs_n).val;_LL61:({void*
_tmp60=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp61="tc: Abs_n in tcFndecl";_tag_dyneither(_tmp61,sizeof(char),22);}),
_tag_dyneither(_tmp60,sizeof(void*),0));});_LL62:;_LL63:({struct Cyc_String_pa_struct
_tmp64;_tmp64.tag=0;_tmp64.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
fd->name));{void*_tmp62[1]={& _tmp64};Cyc_Tcutil_terr(loc,({const char*_tmp63="qualified function declarations are not implemented (%s)";
_tag_dyneither(_tmp63,sizeof(char),57);}),_tag_dyneither(_tmp62,sizeof(void*),1));}});
return;_LL5D:;}(*fd->name).f1=Cyc_Absyn_Abs_n(te->ns);Cyc_Tcutil_check_fndecl_valid_type(
loc,te,fd);{void*t=Cyc_Tcutil_fndecl2typ(fd);fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(
t,fd->attributes);{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=
atts->tl){void*_tmp65=(void*)atts->hd;_LL65: if((int)_tmp65 != 5)goto _LL67;_LL66:
goto _LL68;_LL67: if(_tmp65 <= (void*)17)goto _LL69;if(*((int*)_tmp65)!= 1)goto _LL69;
_LL68:({struct Cyc_String_pa_struct _tmp68;_tmp68.tag=0;_tmp68.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));{
void*_tmp66[1]={& _tmp68};Cyc_Tcutil_terr(loc,({const char*_tmp67="bad attribute %s for function";
_tag_dyneither(_tmp67,sizeof(char),30);}),_tag_dyneither(_tmp66,sizeof(void*),1));}});
goto _LL64;_LL69:;_LL6A: goto _LL64;_LL64:;}}{struct _handler_cons _tmp69;
_push_handler(& _tmp69);{int _tmp6B=0;if(setjmp(_tmp69.handler))_tmp6B=1;if(!
_tmp6B){{struct _tuple5*_tmp6C=((struct _tuple5*(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,v);void*_tmp6D=(*_tmp6C).f1;
void*_tmp6E;_LL6C: if(*((int*)_tmp6D)!= 0)goto _LL6E;_tmp6E=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmp6D)->f1;_LL6D: {struct Cyc_Absyn_Funname_b_struct*_tmp6F=({struct Cyc_Absyn_Funname_b_struct*
_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=({struct Cyc_Absyn_Funname_b_struct
_tmp75;_tmp75.tag=1;_tmp75.f1=fd;_tmp75;});_tmp74;});void*_tmp70=Cyc_Tcdecl_merge_binding(
_tmp6E,(void*)_tmp6F,loc,Cyc_Tc_tc_msg);if(_tmp70 == (void*)0)goto _LL6B;if(_tmp70
== _tmp6E  && (*_tmp6C).f2)goto _LL6B;if(exports == 0  || Cyc_Tc_export_member(fd->name,*
exports))ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct _tuple5*v))Cyc_Dict_insert)(ge->ordinaries,v,(struct
_tuple5*)({struct _tuple5*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->f1=(void*)({
struct Cyc_Tcenv_VarRes_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72[0]=({
struct Cyc_Tcenv_VarRes_struct _tmp73;_tmp73.tag=0;_tmp73.f1=(void*)_tmp70;_tmp73;});
_tmp72;});_tmp71->f2=1;_tmp71;}));goto _LL6B;}_LL6E: if(*((int*)_tmp6D)!= 1)goto
_LL70;_LL6F:({void*_tmp76=0;Cyc_Tcutil_warn(loc,({const char*_tmp77="function declaration shadows previous type declaration";
_tag_dyneither(_tmp77,sizeof(char),55);}),_tag_dyneither(_tmp76,sizeof(void*),0));});
goto _LL6B;_LL70: if(*((int*)_tmp6D)!= 2)goto _LL72;_LL71:({void*_tmp78=0;Cyc_Tcutil_warn(
loc,({const char*_tmp79="function declaration shadows previous datatype constructor";
_tag_dyneither(_tmp79,sizeof(char),59);}),_tag_dyneither(_tmp78,sizeof(void*),0));});
goto _LL6B;_LL72: if(*((int*)_tmp6D)!= 4)goto _LL74;_LL73: goto _LL75;_LL74: if(*((int*)
_tmp6D)!= 3)goto _LL6B;_LL75:({void*_tmp7A=0;Cyc_Tcutil_warn(loc,({const char*
_tmp7B="function declaration shadows previous enum tag";_tag_dyneither(_tmp7B,
sizeof(char),47);}),_tag_dyneither(_tmp7A,sizeof(void*),0));});goto _LL6B;_LL6B:;};
_pop_handler();}else{void*_tmp6A=(void*)_exn_thrown;void*_tmp7D=_tmp6A;_LL77: if(
_tmp7D != Cyc_Dict_Absent)goto _LL79;_LL78: if(exports == 0  || Cyc_Tc_export_member(
fd->name,*exports))ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct _tuple5*v))Cyc_Dict_insert)(ge->ordinaries,v,(
struct _tuple5*)({struct _tuple5*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->f1=(
void*)({struct Cyc_Tcenv_VarRes_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F[0]=({
struct Cyc_Tcenv_VarRes_struct _tmp80;_tmp80.tag=0;_tmp80.f1=(void*)((void*)({
struct Cyc_Absyn_Funname_b_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[0]=({
struct Cyc_Absyn_Funname_b_struct _tmp82;_tmp82.tag=1;_tmp82.f1=fd;_tmp82;});
_tmp81;}));_tmp80;});_tmp7F;});_tmp7E->f2=0;_tmp7E;}));goto _LL76;_LL79:;_LL7A:(
void)_throw(_tmp7D);_LL76:;}}}if(ignore_body)return;{struct _RegionHandle _tmp83=
_new_region("fnrgn");struct _RegionHandle*fnrgn=& _tmp83;_push_region(fnrgn);{
struct Cyc_Tcenv_Fenv*_tmp84=Cyc_Tcenv_new_fenv(fnrgn,loc,fd);struct Cyc_Tcenv_Tenv*
_tmp85=({struct Cyc_Tcenv_Tenv*_tmp88=_region_malloc(fnrgn,sizeof(*_tmp88));
_tmp88->ns=te->ns;_tmp88->ae=te->ae;_tmp88->le=(struct Cyc_Tcenv_Fenv*)_tmp84;
_tmp88->allow_valueof=0;_tmp88;});Cyc_Tcstmt_tcStmt(_tmp85,fd->body,0);Cyc_Tcenv_check_delayed_effects(
_tmp85);Cyc_Tcenv_check_delayed_constraints(_tmp85);if(!Cyc_Tcenv_all_labels_resolved(
_tmp85))({void*_tmp86=0;Cyc_Tcutil_terr(loc,({const char*_tmp87="function has goto statements to undefined labels";
_tag_dyneither(_tmp87,sizeof(char),49);}),_tag_dyneither(_tmp86,sizeof(void*),0));});}
if(Cyc_Tc_is_main(fd->name)){{void*_tmp89=Cyc_Tcutil_compress(fd->ret_type);void*
_tmp8A;_LL7C: if((int)_tmp89 != 0)goto _LL7E;_LL7D:({void*_tmp8B=0;Cyc_Tcutil_warn(
loc,({const char*_tmp8C="main declared with return type void";_tag_dyneither(
_tmp8C,sizeof(char),36);}),_tag_dyneither(_tmp8B,sizeof(void*),0));});goto _LL7B;
_LL7E: if(_tmp89 <= (void*)4)goto _LL80;if(*((int*)_tmp89)!= 5)goto _LL80;_tmp8A=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp89)->f2;_LL7F: goto _LL7B;_LL80:;_LL81:({
struct Cyc_String_pa_struct _tmp8F;_tmp8F.tag=0;_tmp8F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(fd->ret_type));{void*_tmp8D[1]={&
_tmp8F};Cyc_Tcutil_terr(loc,({const char*_tmp8E="main declared with return type %s instead of int or void";
_tag_dyneither(_tmp8E,sizeof(char),57);}),_tag_dyneither(_tmp8D,sizeof(void*),1));}});
goto _LL7B;_LL7B:;}if(fd->c_varargs  || fd->cyc_varargs != 0)({void*_tmp90=0;Cyc_Tcutil_terr(
loc,({const char*_tmp91="main declared with varargs";_tag_dyneither(_tmp91,
sizeof(char),27);}),_tag_dyneither(_tmp90,sizeof(void*),0));});{struct Cyc_List_List*
_tmp92=fd->args;if(_tmp92 != 0){struct _tuple13 _tmp94;void*_tmp95;struct _tuple13*
_tmp93=(struct _tuple13*)_tmp92->hd;_tmp94=*_tmp93;_tmp95=_tmp94.f3;{void*_tmp96=
Cyc_Tcutil_compress(_tmp95);void*_tmp97;_LL83: if(_tmp96 <= (void*)4)goto _LL85;if(*((
int*)_tmp96)!= 5)goto _LL85;_tmp97=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp96)->f2;_LL84: goto _LL82;_LL85:;_LL86:({struct Cyc_String_pa_struct _tmp9A;
_tmp9A.tag=0;_tmp9A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp95));{void*_tmp98[1]={& _tmp9A};Cyc_Tcutil_terr(loc,({const char*_tmp99="main declared with first argument of type %s instead of int";
_tag_dyneither(_tmp99,sizeof(char),60);}),_tag_dyneither(_tmp98,sizeof(void*),1));}});
goto _LL82;_LL82:;}_tmp92=_tmp92->tl;if(_tmp92 != 0){struct _tuple13 _tmp9C;void*
_tmp9D;struct _tuple13*_tmp9B=(struct _tuple13*)_tmp92->hd;_tmp9C=*_tmp9B;_tmp9D=
_tmp9C.f3;_tmp92=_tmp92->tl;if(_tmp92 != 0)({void*_tmp9E=0;Cyc_Tcutil_terr(loc,({
const char*_tmp9F="main declared with too many arguments";_tag_dyneither(_tmp9F,
sizeof(char),38);}),_tag_dyneither(_tmp9E,sizeof(void*),0));});{struct Cyc_Core_Opt*
tvs=({struct Cyc_Core_Opt*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->v=fd->tvs;
_tmpAB;});if(((!Cyc_Tcutil_unify(_tmp9D,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->v=(
void*)((void*)3);_tmpA0;}),tvs)),Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpA1=
_cycalloc(sizeof(*_tmpA1));_tmpA1->v=(void*)((void*)3);_tmpA1;}),tvs),Cyc_Absyn_empty_tqual(
0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())) && !Cyc_Tcutil_unify(
_tmp9D,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->v=(void*)((void*)3);
_tmpA2;}),tvs)),Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpA3=_cycalloc(sizeof(*
_tmpA3));_tmpA3->v=(void*)((void*)3);_tmpA3;}),tvs),Cyc_Absyn_empty_tqual(0),((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !Cyc_Tcutil_unify(
_tmp9D,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(({struct
Cyc_Core_Opt*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->v=(void*)((void*)3);
_tmpA4;}),tvs)),Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpA5=_cycalloc(sizeof(*
_tmpA5));_tmpA5->v=(void*)((void*)3);_tmpA5;}),tvs),Cyc_Absyn_const_tqual(0),((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !Cyc_Tcutil_unify(
_tmp9D,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->v=(void*)((void*)3);
_tmpA6;}),tvs)),Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpA7=_cycalloc(sizeof(*
_tmpA7));_tmpA7->v=(void*)((void*)3);_tmpA7;}),tvs),Cyc_Absyn_const_tqual(0),((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())))({struct Cyc_String_pa_struct
_tmpAA;_tmpAA.tag=0;_tmpAA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp9D));{void*_tmpA8[1]={& _tmpAA};Cyc_Tcutil_terr(loc,({const char*_tmpA9="second argument of main has type %s instead of char??";
_tag_dyneither(_tmpA9,sizeof(char),54);}),_tag_dyneither(_tmpA8,sizeof(void*),1));}});}}}}};
_pop_region(fnrgn);}}}static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*
td){struct _dyneither_ptr*v=(*td->name).f2;{union Cyc_Absyn_Nmspace _tmpAC=(*td->name).f1;
struct Cyc_List_List*_tmpAD;struct Cyc_List_List*_tmpAE;_LL88: if((_tmpAC.Rel_n).tag
!= 1)goto _LL8A;_tmpAD=(struct Cyc_List_List*)(_tmpAC.Rel_n).val;if(_tmpAD != 0)
goto _LL8A;_LL89: goto _LL8B;_LL8A: if((_tmpAC.Abs_n).tag != 2)goto _LL8C;_tmpAE=(
struct Cyc_List_List*)(_tmpAC.Abs_n).val;if(_tmpAE != 0)goto _LL8C;_LL8B: goto _LL87;
_LL8C:;_LL8D:({struct Cyc_String_pa_struct _tmpB1;_tmpB1.tag=0;_tmpB1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(td->name));{void*
_tmpAF[1]={& _tmpB1};Cyc_Tcutil_terr(loc,({const char*_tmpB0="qualified typedef declarations are not implemented (%s)";
_tag_dyneither(_tmpB0,sizeof(char),56);}),_tag_dyneither(_tmpAF,sizeof(void*),1));}});
return;_LL87:;}if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(
ge->typedefs,v)){({struct Cyc_String_pa_struct _tmpB4;_tmpB4.tag=0;_tmpB4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*v);{void*_tmpB2[1]={& _tmpB4};Cyc_Tcutil_terr(
loc,({const char*_tmpB3="redeclaration of typedef %s";_tag_dyneither(_tmpB3,
sizeof(char),28);}),_tag_dyneither(_tmpB2,sizeof(void*),1));}});return;}(*td->name).f1=
Cyc_Absyn_Abs_n(te->ns);Cyc_Tcutil_check_unique_tvars(loc,td->tvs);Cyc_Tcutil_add_tvar_identities(
td->tvs);if(td->defn != 0){Cyc_Tcutil_check_type(loc,te,td->tvs,(void*)0,0,(void*)((
struct Cyc_Core_Opt*)_check_null(td->defn))->v);(td->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(td->tq).print_const,(void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);}{
struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){void*_tmpB5=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)tvs->hd)->kind);struct Cyc_Core_Opt*_tmpB6;struct Cyc_Core_Opt**
_tmpB7;struct Cyc_Core_Opt*_tmpB8;struct Cyc_Core_Opt**_tmpB9;void*_tmpBA;_LL8F:
if(*((int*)_tmpB5)!= 1)goto _LL91;_tmpB6=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB5)->f1;_tmpB7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB5)->f1;_LL90: if(td->defn != 0)({struct Cyc_String_pa_struct _tmpBD;_tmpBD.tag=
0;_tmpBD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
tvs->hd)->name);{void*_tmpBB[1]={& _tmpBD};Cyc_Tcutil_warn(loc,({const char*_tmpBC="type variable %s is not used in typedef definition";
_tag_dyneither(_tmpBC,sizeof(char),51);}),_tag_dyneither(_tmpBB,sizeof(void*),1));}});*
_tmpB7=({struct Cyc_Core_Opt*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->v=(void*)
Cyc_Tcutil_kind_to_bound((void*)2);_tmpBE;});goto _LL8E;_LL91: if(*((int*)_tmpB5)
!= 2)goto _LL93;_tmpB8=((struct Cyc_Absyn_Less_kb_struct*)_tmpB5)->f1;_tmpB9=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpB5)->f1;_tmpBA=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmpB5)->f2;_LL92:*_tmpB9=({struct Cyc_Core_Opt*
_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->v=(void*)Cyc_Tcutil_kind_to_bound(
_tmpBA);_tmpBF;});goto _LL8E;_LL93:;_LL94: continue;_LL8E:;}}ge->typedefs=((struct
Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*
v))Cyc_Dict_insert)(ge->typedefs,v,td);}static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){struct _RegionHandle
_tmpC0=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpC0;
_push_region(uprev_rgn);for(0;rpo != 0;rpo=rpo->tl){struct _tuple9 _tmpC2;void*
_tmpC3;void*_tmpC4;struct _tuple9*_tmpC1=(struct _tuple9*)rpo->hd;_tmpC2=*_tmpC1;
_tmpC3=_tmpC2.f1;_tmpC4=_tmpC2.f2;Cyc_Tcutil_check_type(loc,te,tvs,(void*)6,0,
_tmpC3);Cyc_Tcutil_check_type(loc,te,tvs,(void*)5,0,_tmpC4);}{struct Cyc_List_List*
prev_fields=0;struct Cyc_List_List*_tmpC5=fields;for(0;_tmpC5 != 0;_tmpC5=_tmpC5->tl){
struct Cyc_Absyn_Aggrfield _tmpC7;struct _dyneither_ptr*_tmpC8;struct Cyc_Absyn_Tqual
_tmpC9;void*_tmpCA;struct Cyc_Absyn_Exp*_tmpCB;struct Cyc_List_List*_tmpCC;struct
Cyc_Absyn_Aggrfield*_tmpC6=(struct Cyc_Absyn_Aggrfield*)_tmpC5->hd;_tmpC7=*_tmpC6;
_tmpC8=_tmpC7.name;_tmpC9=_tmpC7.tq;_tmpCA=_tmpC7.type;_tmpCB=_tmpC7.width;
_tmpCC=_tmpC7.attributes;if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct
_dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,
prev_fields,_tmpC8))({struct Cyc_String_pa_struct _tmpCF;_tmpCF.tag=0;_tmpCF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpC8);{void*_tmpCD[1]={& _tmpCF};
Cyc_Tcutil_terr(loc,({const char*_tmpCE="duplicate field %s";_tag_dyneither(
_tmpCE,sizeof(char),19);}),_tag_dyneither(_tmpCD,sizeof(void*),1));}});if(Cyc_strcmp((
struct _dyneither_ptr)*_tmpC8,({const char*_tmpD0="";_tag_dyneither(_tmpD0,sizeof(
char),1);}))!= 0)prev_fields=({struct Cyc_List_List*_tmpD1=_region_malloc(
uprev_rgn,sizeof(*_tmpD1));_tmpD1->hd=_tmpC8;_tmpD1->tl=prev_fields;_tmpD1;});
Cyc_Tcutil_check_type(loc,te,tvs,(void*)1,0,_tmpCA);(((struct Cyc_Absyn_Aggrfield*)
_tmpC5->hd)->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct
Cyc_Absyn_Aggrfield*)_tmpC5->hd)->tq).print_const,_tmpCA);Cyc_Tcutil_check_bitfield(
loc,te,_tmpCA,_tmpCB,_tmpC8);}};_pop_region(uprev_rgn);}struct _tuple14{struct Cyc_Absyn_AggrdeclImpl*
f1;struct Cyc_Absyn_Aggrdecl***f2;};void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl*
ad){struct _dyneither_ptr*_tmpD2=(*ad->name).f2;{struct Cyc_List_List*atts=ad->attributes;
for(0;atts != 0;atts=atts->tl){void*_tmpD3=(void*)atts->hd;_LL96: if((int)_tmpD3 != 
5)goto _LL98;_LL97: goto _LL99;_LL98: if(_tmpD3 <= (void*)17)goto _LL9A;if(*((int*)
_tmpD3)!= 1)goto _LL9A;_LL99: continue;_LL9A:;_LL9B:({struct Cyc_String_pa_struct
_tmpD7;_tmpD7.tag=0;_tmpD7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpD2);{struct Cyc_String_pa_struct _tmpD6;_tmpD6.tag=0;_tmpD6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));{
void*_tmpD4[2]={& _tmpD6,& _tmpD7};Cyc_Tcutil_terr(loc,({const char*_tmpD5="bad attribute %s in  %s definition";
_tag_dyneither(_tmpD5,sizeof(char),35);}),_tag_dyneither(_tmpD4,sizeof(void*),2));}}});
goto _LL95;_LL95:;}}{struct Cyc_List_List*_tmpD8=ad->tvs;{struct Cyc_List_List*tvs2=
_tmpD8;for(0;tvs2 != 0;tvs2=tvs2->tl){void*_tmpD9=Cyc_Absyn_compress_kb(((struct
Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*_tmpDA;struct Cyc_Core_Opt**
_tmpDB;struct Cyc_Core_Opt*_tmpDC;struct Cyc_Core_Opt**_tmpDD;void*_tmpDE;struct
Cyc_Core_Opt*_tmpDF;struct Cyc_Core_Opt**_tmpE0;void*_tmpE1;void*_tmpE2;_LL9D: if(*((
int*)_tmpD9)!= 1)goto _LL9F;_tmpDA=((struct Cyc_Absyn_Unknown_kb_struct*)_tmpD9)->f1;
_tmpDB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmpD9)->f1;
_LL9E: _tmpDD=_tmpDB;goto _LLA0;_LL9F: if(*((int*)_tmpD9)!= 2)goto _LLA1;_tmpDC=((
struct Cyc_Absyn_Less_kb_struct*)_tmpD9)->f1;_tmpDD=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmpD9)->f1;_tmpDE=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpD9)->f2;if((int)_tmpDE != 1)goto _LLA1;_LLA0: _tmpE0=_tmpDD;goto _LLA2;_LLA1: if(*((
int*)_tmpD9)!= 2)goto _LLA3;_tmpDF=((struct Cyc_Absyn_Less_kb_struct*)_tmpD9)->f1;
_tmpE0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpD9)->f1;
_tmpE1=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpD9)->f2;if((int)_tmpE1 != 0)
goto _LLA3;_LLA2:*_tmpE0=({struct Cyc_Core_Opt*_tmpE3=_cycalloc(sizeof(*_tmpE3));
_tmpE3->v=(void*)Cyc_Tcutil_kind_to_bound((void*)2);_tmpE3;});continue;_LLA3: if(*((
int*)_tmpD9)!= 0)goto _LLA5;_tmpE2=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpD9)->f1;
if((int)_tmpE2 != 1)goto _LLA5;_LLA4:({struct Cyc_String_pa_struct _tmpE7;_tmpE7.tag=
0;_tmpE7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
tvs2->hd)->name);{struct Cyc_String_pa_struct _tmpE6;_tmpE6.tag=0;_tmpE6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpD2);{void*_tmpE4[2]={& _tmpE6,&
_tmpE7};Cyc_Tcutil_terr(loc,({const char*_tmpE5="type %s attempts to abstract type variable %s of kind M";
_tag_dyneither(_tmpE5,sizeof(char),56);}),_tag_dyneither(_tmpE4,sizeof(void*),2));}}});
continue;_LLA5:;_LLA6: continue;_LL9C:;}}{union Cyc_Absyn_Nmspace _tmpE8=(*ad->name).f1;
struct Cyc_List_List*_tmpE9;struct Cyc_List_List*_tmpEA;_LLA8: if((_tmpE8.Rel_n).tag
!= 1)goto _LLAA;_tmpE9=(struct Cyc_List_List*)(_tmpE8.Rel_n).val;if(_tmpE9 != 0)
goto _LLAA;_LLA9: goto _LLAB;_LLAA: if((_tmpE8.Abs_n).tag != 2)goto _LLAC;_tmpEA=(
struct Cyc_List_List*)(_tmpE8.Abs_n).val;if(_tmpEA != 0)goto _LLAC;_LLAB: goto _LLA7;
_LLAC:;_LLAD:({struct Cyc_String_pa_struct _tmpED;_tmpED.tag=0;_tmpED.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(ad->name));{void*
_tmpEB[1]={& _tmpED};Cyc_Tcutil_terr(loc,({const char*_tmpEC="qualified struct declarations are not implemented (%s)";
_tag_dyneither(_tmpEC,sizeof(char),55);}),_tag_dyneither(_tmpEB,sizeof(void*),1));}});
return;_LLA7:;}(*ad->name).f1=Cyc_Absyn_Abs_n(te->ns);Cyc_Tcutil_check_unique_tvars(
loc,ad->tvs);Cyc_Tcutil_add_tvar_identities(ad->tvs);{struct _tuple14 _tmpEF=({
struct _tuple14 _tmpEE;_tmpEE.f1=ad->impl;_tmpEE.f2=((struct Cyc_Absyn_Aggrdecl***(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->aggrdecls,
_tmpD2);_tmpEE;});struct Cyc_Absyn_AggrdeclImpl*_tmpF0;struct Cyc_Absyn_Aggrdecl***
_tmpF1;struct Cyc_Absyn_AggrdeclImpl*_tmpF2;struct Cyc_Absyn_AggrdeclImpl _tmpF3;
struct Cyc_List_List*_tmpF4;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*_tmpF6;
int _tmpF7;struct Cyc_Absyn_Aggrdecl***_tmpF8;struct Cyc_Absyn_AggrdeclImpl*_tmpF9;
struct Cyc_Absyn_AggrdeclImpl _tmpFA;struct Cyc_List_List*_tmpFB;struct Cyc_List_List*
_tmpFC;struct Cyc_List_List*_tmpFD;int _tmpFE;struct Cyc_Absyn_Aggrdecl***_tmpFF;
struct Cyc_Absyn_Aggrdecl**_tmp100;struct Cyc_Absyn_AggrdeclImpl*_tmp101;struct Cyc_Absyn_Aggrdecl***
_tmp102;struct Cyc_Absyn_Aggrdecl**_tmp103;_LLAF: _tmpF0=_tmpEF.f1;if(_tmpF0 != 0)
goto _LLB1;_tmpF1=_tmpEF.f2;if(_tmpF1 != 0)goto _LLB1;_LLB0: ge->aggrdecls=((struct
Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**
v))Cyc_Dict_insert)(ge->aggrdecls,_tmpD2,({struct Cyc_Absyn_Aggrdecl**_tmp104=
_cycalloc(sizeof(*_tmp104));_tmp104[0]=ad;_tmp104;}));goto _LLAE;_LLB1: _tmpF2=
_tmpEF.f1;if(_tmpF2 == 0)goto _LLB3;_tmpF3=*_tmpF2;_tmpF4=_tmpF3.exist_vars;_tmpF5=
_tmpF3.rgn_po;_tmpF6=_tmpF3.fields;_tmpF7=_tmpF3.tagged;_tmpF8=_tmpEF.f2;if(
_tmpF8 != 0)goto _LLB3;_LLB2: {struct Cyc_Absyn_Aggrdecl**_tmp105=({struct Cyc_Absyn_Aggrdecl**
_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C[0]=({struct Cyc_Absyn_Aggrdecl*
_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->kind=ad->kind;_tmp11D->sc=(void*)3;
_tmp11D->name=ad->name;_tmp11D->tvs=_tmpD8;_tmp11D->impl=0;_tmp11D->attributes=
ad->attributes;_tmp11D;});_tmp11C;});ge->aggrdecls=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(
ge->aggrdecls,_tmpD2,_tmp105);Cyc_Tcutil_check_unique_tvars(loc,_tmpF4);Cyc_Tcutil_add_tvar_identities(
_tmpF4);{struct Cyc_List_List*tvs2=_tmpF4;for(0;tvs2 != 0;tvs2=tvs2->tl){void*
_tmp106=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*
_tmp107;struct Cyc_Core_Opt**_tmp108;struct Cyc_Core_Opt*_tmp109;struct Cyc_Core_Opt**
_tmp10A;void*_tmp10B;struct Cyc_Core_Opt*_tmp10C;struct Cyc_Core_Opt**_tmp10D;void*
_tmp10E;void*_tmp10F;_LLB8: if(*((int*)_tmp106)!= 1)goto _LLBA;_tmp107=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp106)->f1;_tmp108=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp106)->f1;_LLB9: _tmp10A=_tmp108;goto _LLBB;_LLBA: if(*((int*)_tmp106)!= 2)goto
_LLBC;_tmp109=((struct Cyc_Absyn_Less_kb_struct*)_tmp106)->f1;_tmp10A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp106)->f1;_tmp10B=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp106)->f2;if((int)_tmp10B != 1)goto _LLBC;_LLBB: _tmp10D=_tmp10A;goto _LLBD;_LLBC:
if(*((int*)_tmp106)!= 2)goto _LLBE;_tmp10C=((struct Cyc_Absyn_Less_kb_struct*)
_tmp106)->f1;_tmp10D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp106)->f1;_tmp10E=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp106)->f2;if((
int)_tmp10E != 0)goto _LLBE;_LLBD:*_tmp10D=({struct Cyc_Core_Opt*_tmp110=_cycalloc(
sizeof(*_tmp110));_tmp110->v=(void*)Cyc_Tcutil_kind_to_bound((void*)2);_tmp110;});
continue;_LLBE: if(*((int*)_tmp106)!= 0)goto _LLC0;_tmp10F=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp106)->f1;if((int)_tmp10F != 1)goto _LLC0;_LLBF:({struct Cyc_String_pa_struct
_tmp114;_tmp114.tag=0;_tmp114.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name);{struct Cyc_String_pa_struct _tmp113;
_tmp113.tag=0;_tmp113.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpD2);{
void*_tmp111[2]={& _tmp113,& _tmp114};Cyc_Tcutil_terr(loc,({const char*_tmp112="type %s attempts to abstract type variable %s of kind M";
_tag_dyneither(_tmp112,sizeof(char),56);}),_tag_dyneither(_tmp111,sizeof(void*),
2));}}});continue;_LLC0:;_LLC1: continue;_LLB7:;}}if(_tmpF7  && ad->kind == (void*)
0)({void*_tmp115=0;Cyc_Tcutil_terr(loc,({const char*_tmp116="@tagged qualifier is only allowed on union declarations";
_tag_dyneither(_tmp116,sizeof(char),56);}),_tag_dyneither(_tmp115,sizeof(void*),
0));});Cyc_Tc_tcAggrImpl(te,ge,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmpD8,_tmpF4),_tmpF5,_tmpF6);if(ad->kind
== (void*)1  && !_tmpF7){struct Cyc_List_List*f=_tmpF6;for(0;f != 0;f=f->tl){if(!
Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type))({struct Cyc_String_pa_struct
_tmp11B;_tmp11B.tag=0;_tmp11B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type));{struct Cyc_String_pa_struct
_tmp11A;_tmp11A.tag=0;_tmp11A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpD2);{struct Cyc_String_pa_struct _tmp119;_tmp119.tag=0;_tmp119.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name);{
void*_tmp117[3]={& _tmp119,& _tmp11A,& _tmp11B};Cyc_Tcutil_warn(loc,({const char*
_tmp118="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read";
_tag_dyneither(_tmp118,sizeof(char),98);}),_tag_dyneither(_tmp117,sizeof(void*),
3));}}}});}}*_tmp105=ad;goto _LLAE;}_LLB3: _tmpF9=_tmpEF.f1;if(_tmpF9 == 0)goto
_LLB5;_tmpFA=*_tmpF9;_tmpFB=_tmpFA.exist_vars;_tmpFC=_tmpFA.rgn_po;_tmpFD=_tmpFA.fields;
_tmpFE=_tmpFA.tagged;_tmpFF=_tmpEF.f2;if(_tmpFF == 0)goto _LLB5;_tmp100=*_tmpFF;
_LLB4: if(ad->kind != (*_tmp100)->kind)({void*_tmp11E=0;Cyc_Tcutil_terr(loc,({
const char*_tmp11F="cannot reuse struct names for unions and vice-versa";
_tag_dyneither(_tmp11F,sizeof(char),52);}),_tag_dyneither(_tmp11E,sizeof(void*),
0));});{struct Cyc_Absyn_Aggrdecl*_tmp120=*_tmp100;*_tmp100=({struct Cyc_Absyn_Aggrdecl*
_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->kind=ad->kind;_tmp121->sc=(void*)3;
_tmp121->name=ad->name;_tmp121->tvs=_tmpD8;_tmp121->impl=0;_tmp121->attributes=
ad->attributes;_tmp121;});Cyc_Tcutil_check_unique_tvars(loc,_tmpFB);Cyc_Tcutil_add_tvar_identities(
_tmpFB);if(_tmpFE  && ad->kind == (void*)0)({void*_tmp122=0;Cyc_Tcutil_terr(loc,({
const char*_tmp123="@tagged qualifier is only allowed on union declarations";
_tag_dyneither(_tmp123,sizeof(char),56);}),_tag_dyneither(_tmp122,sizeof(void*),
0));});Cyc_Tc_tcAggrImpl(te,ge,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmpD8,_tmpFB),_tmpFC,_tmpFD);*_tmp100=
_tmp120;_tmp103=_tmp100;goto _LLB6;}_LLB5: _tmp101=_tmpEF.f1;if(_tmp101 != 0)goto
_LLAE;_tmp102=_tmpEF.f2;if(_tmp102 == 0)goto _LLAE;_tmp103=*_tmp102;_LLB6: {struct
Cyc_Absyn_Aggrdecl*_tmp124=Cyc_Tcdecl_merge_aggrdecl(*_tmp103,ad,loc,Cyc_Tc_tc_msg);
if(_tmp124 == 0)return;else{*_tmp103=(struct Cyc_Absyn_Aggrdecl*)_tmp124;ad=(
struct Cyc_Absyn_Aggrdecl*)_tmp124;goto _LLAE;}}_LLAE:;}ge->ordinaries=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple5*v))Cyc_Dict_insert)(ge->ordinaries,
_tmpD2,(struct _tuple5*)({struct _tuple5*_tmp125=_cycalloc(sizeof(*_tmp125));
_tmp125->f1=(void*)({struct Cyc_Tcenv_AggrRes_struct*_tmp126=_cycalloc(sizeof(*
_tmp126));_tmp126[0]=({struct Cyc_Tcenv_AggrRes_struct _tmp127;_tmp127.tag=1;
_tmp127.f1=ad;_tmp127;});_tmp126;});_tmp125->f2=1;_tmp125;}));}}struct _tuple15{
struct Cyc_Absyn_Tqual f1;void*f2;};static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*
fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){{struct Cyc_List_List*
_tmp128=fields;for(0;_tmp128 != 0;_tmp128=_tmp128->tl){struct Cyc_Absyn_Datatypefield*
_tmp129=(struct Cyc_Absyn_Datatypefield*)_tmp128->hd;{struct Cyc_List_List*typs=
_tmp129->typs;for(0;typs != 0;typs=typs->tl){Cyc_Tcutil_check_type(_tmp129->loc,
te,tvs,(void*)1,0,(*((struct _tuple15*)typs->hd)).f2);{struct _RegionHandle*
_tmp12A=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp12A,(*((
struct _tuple15*)typs->hd)).f2))({struct Cyc_String_pa_struct _tmp12D;_tmp12D.tag=0;
_tmp12D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp129->name));{void*_tmp12B[1]={& _tmp12D};Cyc_Tcutil_terr(_tmp129->loc,({const
char*_tmp12C="noalias pointers in datatypes are not allowed (%s)";_tag_dyneither(
_tmp12C,sizeof(char),51);}),_tag_dyneither(_tmp12B,sizeof(void*),1));}});((*((
struct _tuple15*)typs->hd)).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
_tmp129->loc,((*((struct _tuple15*)typs->hd)).f1).print_const,(*((struct _tuple15*)
typs->hd)).f2);}}}{union Cyc_Absyn_Nmspace _tmp12E=(*_tmp129->name).f1;struct Cyc_List_List*
_tmp12F;struct Cyc_List_List*_tmp130;struct Cyc_List_List*_tmp131;int _tmp132;_LLC3:
if((_tmp12E.Rel_n).tag != 1)goto _LLC5;_tmp12F=(struct Cyc_List_List*)(_tmp12E.Rel_n).val;
if(_tmp12F != 0)goto _LLC5;_LLC4: if(is_extensible)(*_tmp129->name).f1=Cyc_Absyn_Abs_n(
te->ns);else{(*_tmp129->name).f1=(*name).f1;}goto _LLC2;_LLC5: if((_tmp12E.Rel_n).tag
!= 1)goto _LLC7;_tmp130=(struct Cyc_List_List*)(_tmp12E.Rel_n).val;_LLC6:({struct
Cyc_String_pa_struct _tmp135;_tmp135.tag=0;_tmp135.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp129->name));{void*_tmp133[1]={&
_tmp135};Cyc_Tcutil_terr(_tmp129->loc,({const char*_tmp134="qualified datatypefield declarations are not allowed (%s)";
_tag_dyneither(_tmp134,sizeof(char),58);}),_tag_dyneither(_tmp133,sizeof(void*),
1));}});goto _LLC2;_LLC7: if((_tmp12E.Abs_n).tag != 2)goto _LLC9;_tmp131=(struct Cyc_List_List*)(
_tmp12E.Abs_n).val;_LLC8: goto _LLC2;_LLC9: if((_tmp12E.Loc_n).tag != 3)goto _LLC2;
_tmp132=(int)(_tmp12E.Loc_n).val;_LLCA:({void*_tmp136=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp137="tcDatatypeFields: Loc_n";
_tag_dyneither(_tmp137,sizeof(char),24);}),_tag_dyneither(_tmp136,sizeof(void*),
0));});_LLC2:;}}}{struct Cyc_List_List*fields2;if(is_extensible){int _tmp138=1;
struct Cyc_List_List*_tmp139=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp138,(*
name).f2,loc,Cyc_Tc_tc_msg);if(_tmp138)fields2=_tmp139;else{fields2=0;}}else{
struct _RegionHandle _tmp13A=_new_region("uprev_rgn");struct _RegionHandle*
uprev_rgn=& _tmp13A;_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;{
struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Datatypefield*
_tmp13B=(struct Cyc_Absyn_Datatypefield*)fs->hd;if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp13B->name).f2))({
struct Cyc_String_pa_struct _tmp13F;_tmp13F.tag=0;_tmp13F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)obj);{struct Cyc_String_pa_struct _tmp13E;_tmp13E.tag=0;
_tmp13E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp13B->name).f2);{
void*_tmp13C[2]={& _tmp13E,& _tmp13F};Cyc_Tcutil_terr(_tmp13B->loc,({const char*
_tmp13D="duplicate field name %s in %s";_tag_dyneither(_tmp13D,sizeof(char),30);}),
_tag_dyneither(_tmp13C,sizeof(void*),2));}}});else{prev_fields=({struct Cyc_List_List*
_tmp140=_region_malloc(uprev_rgn,sizeof(*_tmp140));_tmp140->hd=(*_tmp13B->name).f2;
_tmp140->tl=prev_fields;_tmp140;});}if(_tmp13B->sc != (void*)2){({struct Cyc_String_pa_struct
_tmp143;_tmp143.tag=0;_tmp143.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*
_tmp13B->name).f2);{void*_tmp141[1]={& _tmp143};Cyc_Tcutil_warn(loc,({const char*
_tmp142="ignoring scope of field %s";_tag_dyneither(_tmp142,sizeof(char),27);}),
_tag_dyneither(_tmp141,sizeof(void*),1));}});_tmp13B->sc=(void*)2;}}}fields2=
fields;};_pop_region(uprev_rgn);}{struct Cyc_List_List*_tmp144=fields;for(0;
_tmp144 != 0;_tmp144=_tmp144->tl){struct Cyc_Absyn_Datatypefield*_tmp145=(struct
Cyc_Absyn_Datatypefield*)_tmp144->hd;ge->ordinaries=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple5*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmp145->name).f2,(struct _tuple5*)({struct _tuple5*_tmp146=_cycalloc(sizeof(*
_tmp146));_tmp146->f1=(void*)({struct Cyc_Tcenv_DatatypeRes_struct*_tmp147=
_cycalloc(sizeof(*_tmp147));_tmp147[0]=({struct Cyc_Tcenv_DatatypeRes_struct
_tmp148;_tmp148.tag=2;_tmp148.f1=tudres;_tmp148.f2=_tmp145;_tmp148;});_tmp147;});
_tmp146->f2=1;_tmp146;}));}}return fields2;}}struct _tuple16{struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Datatypedecl***f2;};void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Datatypedecl*
tud){struct _dyneither_ptr*v=(*tud->name).f2;struct _dyneither_ptr obj=tud->is_extensible?({
const char*_tmp193="@extensible datatype";_tag_dyneither(_tmp193,sizeof(char),21);}):({
const char*_tmp194="datatype";_tag_dyneither(_tmp194,sizeof(char),9);});struct Cyc_List_List*
tvs=tud->tvs;{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){void*
_tmp149=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*
_tmp14A;struct Cyc_Core_Opt**_tmp14B;struct Cyc_Core_Opt*_tmp14C;struct Cyc_Core_Opt**
_tmp14D;void*_tmp14E;struct Cyc_Core_Opt*_tmp14F;struct Cyc_Core_Opt**_tmp150;void*
_tmp151;struct Cyc_Core_Opt*_tmp152;struct Cyc_Core_Opt**_tmp153;void*_tmp154;void*
_tmp155;struct Cyc_Core_Opt*_tmp156;struct Cyc_Core_Opt**_tmp157;void*_tmp158;void*
_tmp159;void*_tmp15A;_LLCC: if(*((int*)_tmp149)!= 1)goto _LLCE;_tmp14A=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp149)->f1;_tmp14B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp149)->f1;_LLCD: _tmp14D=_tmp14B;goto _LLCF;_LLCE: if(*((int*)_tmp149)!= 2)goto
_LLD0;_tmp14C=((struct Cyc_Absyn_Less_kb_struct*)_tmp149)->f1;_tmp14D=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp149)->f1;_tmp14E=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp149)->f2;if((int)_tmp14E != 1)goto _LLD0;_LLCF: _tmp150=_tmp14D;goto _LLD1;_LLD0:
if(*((int*)_tmp149)!= 2)goto _LLD2;_tmp14F=((struct Cyc_Absyn_Less_kb_struct*)
_tmp149)->f1;_tmp150=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp149)->f1;_tmp151=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp149)->f2;if((
int)_tmp151 != 0)goto _LLD2;_LLD1:*_tmp150=({struct Cyc_Core_Opt*_tmp15B=_cycalloc(
sizeof(*_tmp15B));_tmp15B->v=(void*)Cyc_Tcutil_kind_to_bound((void*)2);_tmp15B;});
goto _LLCB;_LLD2: if(*((int*)_tmp149)!= 2)goto _LLD4;_tmp152=((struct Cyc_Absyn_Less_kb_struct*)
_tmp149)->f1;_tmp153=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp149)->f1;_tmp154=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp149)->f2;if((
int)_tmp154 != 5)goto _LLD4;_LLD3:*_tmp153=({struct Cyc_Core_Opt*_tmp15C=_cycalloc(
sizeof(*_tmp15C));_tmp15C->v=(void*)Cyc_Tcutil_kind_to_bound((void*)3);_tmp15C;});
goto _LLCB;_LLD4: if(*((int*)_tmp149)!= 0)goto _LLD6;_tmp155=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp149)->f1;if((int)_tmp155 != 5)goto _LLD6;_LLD5:({struct Cyc_String_pa_struct
_tmp161;_tmp161.tag=0;_tmp161.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name);{struct Cyc_String_pa_struct _tmp160;
_tmp160.tag=0;_tmp160.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);{
struct Cyc_String_pa_struct _tmp15F;_tmp15F.tag=0;_tmp15F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)obj);{void*_tmp15D[3]={& _tmp15F,& _tmp160,& _tmp161};Cyc_Tcutil_terr(
loc,({const char*_tmp15E="%s %s attempts to abstract type variable %s of kind TR";
_tag_dyneither(_tmp15E,sizeof(char),55);}),_tag_dyneither(_tmp15D,sizeof(void*),
3));}}}});goto _LLCB;_LLD6: if(*((int*)_tmp149)!= 2)goto _LLD8;_tmp156=((struct Cyc_Absyn_Less_kb_struct*)
_tmp149)->f1;_tmp157=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp149)->f1;_tmp158=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp149)->f2;_LLD7:
goto _LLD9;_LLD8: if(*((int*)_tmp149)!= 0)goto _LLDA;_tmp159=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp149)->f1;if((int)_tmp159 != 4)goto _LLDA;_LLD9:({struct Cyc_String_pa_struct
_tmp166;_tmp166.tag=0;_tmp166.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name);{struct Cyc_String_pa_struct _tmp165;
_tmp165.tag=0;_tmp165.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);{
struct Cyc_String_pa_struct _tmp164;_tmp164.tag=0;_tmp164.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)obj);{void*_tmp162[3]={& _tmp164,& _tmp165,& _tmp166};Cyc_Tcutil_terr(
loc,({const char*_tmp163="%s %s attempts to abstract type variable %s of kind UR";
_tag_dyneither(_tmp163,sizeof(char),55);}),_tag_dyneither(_tmp162,sizeof(void*),
3));}}}});goto _LLCB;_LLDA: if(*((int*)_tmp149)!= 0)goto _LLDC;_tmp15A=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp149)->f1;if((int)_tmp15A != 1)goto _LLDC;_LLDB:({
struct Cyc_String_pa_struct _tmp16B;_tmp16B.tag=0;_tmp16B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);{struct Cyc_String_pa_struct
_tmp16A;_tmp16A.tag=0;_tmp16A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);{
struct Cyc_String_pa_struct _tmp169;_tmp169.tag=0;_tmp169.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)obj);{void*_tmp167[3]={& _tmp169,& _tmp16A,& _tmp16B};Cyc_Tcutil_terr(
loc,({const char*_tmp168="%s %s attempts to abstract type variable %s of kind M";
_tag_dyneither(_tmp168,sizeof(char),54);}),_tag_dyneither(_tmp167,sizeof(void*),
3));}}}});goto _LLCB;_LLDC:;_LLDD: goto _LLCB;_LLCB:;}}Cyc_Tcutil_check_unique_tvars(
loc,tvs);Cyc_Tcutil_add_tvar_identities(tvs);{struct _RegionHandle _tmp16C=
_new_region("temp");struct _RegionHandle*temp=& _tmp16C;_push_region(temp);{struct
Cyc_Absyn_Datatypedecl***tud_opt;{struct _handler_cons _tmp16D;_push_handler(&
_tmp16D);{int _tmp16F=0;if(setjmp(_tmp16D.handler))_tmp16F=1;if(!_tmp16F){tud_opt=
Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);if(tud_opt == 0  && !tud->is_extensible)(
int)_throw((void*)Cyc_Dict_Absent);if(tud_opt != 0)tud->name=(*(*tud_opt))->name;
else{(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns);};_pop_handler();}else{void*_tmp16E=(
void*)_exn_thrown;void*_tmp171=_tmp16E;_LLDF: if(_tmp171 != Cyc_Dict_Absent)goto
_LLE1;_LLE0:{union Cyc_Absyn_Nmspace _tmp172=(*tud->name).f1;struct Cyc_List_List*
_tmp173;struct Cyc_List_List*_tmp174;_LLE4: if((_tmp172.Rel_n).tag != 1)goto _LLE6;
_tmp173=(struct Cyc_List_List*)(_tmp172.Rel_n).val;if(_tmp173 != 0)goto _LLE6;_LLE5:(*
tud->name).f1=Cyc_Absyn_Abs_n(te->ns);goto _LLE3;_LLE6: if((_tmp172.Abs_n).tag != 2)
goto _LLE8;_tmp174=(struct Cyc_List_List*)(_tmp172.Abs_n).val;_LLE7: goto _LLE9;
_LLE8:;_LLE9:({struct Cyc_String_pa_struct _tmp177;_tmp177.tag=0;_tmp177.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tud->name));{
void*_tmp175[1]={& _tmp177};Cyc_Tcutil_terr(loc,({const char*_tmp176="qualified (non-extensible) datatype declarations are not implemented (%s)";
_tag_dyneither(_tmp176,sizeof(char),74);}),_tag_dyneither(_tmp175,sizeof(void*),
1));}});_npop_handler(0);return;_LLE3:;}{struct Cyc_Absyn_Datatypedecl***_tmp178=((
struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))
Cyc_Dict_lookup_opt)(ge->datatypedecls,v);tud_opt=(unsigned int)_tmp178?({struct
Cyc_Absyn_Datatypedecl***_tmp179=_region_malloc(temp,sizeof(*_tmp179));_tmp179[0]=*
_tmp178;_tmp179;}): 0;goto _LLDE;}_LLE1:;_LLE2:(void)_throw(_tmp171);_LLDE:;}}}{
struct _tuple16 _tmp17B=({struct _tuple16 _tmp17A;_tmp17A.f1=tud->fields;_tmp17A.f2=
tud_opt;_tmp17A;});struct Cyc_Core_Opt*_tmp17C;struct Cyc_Absyn_Datatypedecl***
_tmp17D;struct Cyc_Core_Opt*_tmp17E;struct Cyc_Core_Opt _tmp17F;struct Cyc_List_List*
_tmp180;struct Cyc_List_List**_tmp181;struct Cyc_Absyn_Datatypedecl***_tmp182;
struct Cyc_Core_Opt*_tmp183;struct Cyc_Core_Opt _tmp184;struct Cyc_List_List*_tmp185;
struct Cyc_List_List**_tmp186;struct Cyc_Absyn_Datatypedecl***_tmp187;struct Cyc_Absyn_Datatypedecl**
_tmp188;struct Cyc_Core_Opt*_tmp189;struct Cyc_Absyn_Datatypedecl***_tmp18A;struct
Cyc_Absyn_Datatypedecl**_tmp18B;_LLEB: _tmp17C=_tmp17B.f1;if(_tmp17C != 0)goto
_LLED;_tmp17D=_tmp17B.f2;if(_tmp17D != 0)goto _LLED;_LLEC: ge->datatypedecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**
v))Cyc_Dict_insert)(ge->datatypedecls,v,({struct Cyc_Absyn_Datatypedecl**_tmp18C=
_cycalloc(sizeof(*_tmp18C));_tmp18C[0]=tud;_tmp18C;}));goto _LLEA;_LLED: _tmp17E=
_tmp17B.f1;if(_tmp17E == 0)goto _LLEF;_tmp17F=*_tmp17E;_tmp180=(struct Cyc_List_List*)
_tmp17F.v;_tmp181=(struct Cyc_List_List**)&(*_tmp17B.f1).v;_tmp182=_tmp17B.f2;if(
_tmp182 != 0)goto _LLEF;_LLEE: {struct Cyc_Absyn_Datatypedecl**_tmp18D=({struct Cyc_Absyn_Datatypedecl**
_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E[0]=({struct Cyc_Absyn_Datatypedecl*
_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F->sc=(void*)3;_tmp18F->name=tud->name;
_tmp18F->tvs=tvs;_tmp18F->fields=0;_tmp18F->is_extensible=tud->is_extensible;
_tmp18F;});_tmp18E;});ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,
v,_tmp18D);*_tmp181=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*
_tmp181,tvs,tud);*_tmp18D=tud;goto _LLEA;}_LLEF: _tmp183=_tmp17B.f1;if(_tmp183 == 0)
goto _LLF1;_tmp184=*_tmp183;_tmp185=(struct Cyc_List_List*)_tmp184.v;_tmp186=(
struct Cyc_List_List**)&(*_tmp17B.f1).v;_tmp187=_tmp17B.f2;if(_tmp187 == 0)goto
_LLF1;_tmp188=*_tmp187;_LLF0: {struct Cyc_Absyn_Datatypedecl*_tmp190=*_tmp188;if((
!tud->is_extensible  && (unsigned int)_tmp190) && _tmp190->is_extensible)tud->is_extensible=
1;*_tmp188=({struct Cyc_Absyn_Datatypedecl*_tmp191=_cycalloc(sizeof(*_tmp191));
_tmp191->sc=(void*)3;_tmp191->name=tud->name;_tmp191->tvs=tvs;_tmp191->fields=0;
_tmp191->is_extensible=tud->is_extensible;_tmp191;});*_tmp186=Cyc_Tc_tcDatatypeFields(
te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp186,tvs,tud);*_tmp188=_tmp190;
_tmp18B=_tmp188;goto _LLF2;}_LLF1: _tmp189=_tmp17B.f1;if(_tmp189 != 0)goto _LLEA;
_tmp18A=_tmp17B.f2;if(_tmp18A == 0)goto _LLEA;_tmp18B=*_tmp18A;_LLF2: {struct Cyc_Absyn_Datatypedecl*
_tmp192=Cyc_Tcdecl_merge_datatypedecl(*_tmp18B,tud,loc,Cyc_Tc_tc_msg);if(_tmp192
== 0){_npop_handler(0);return;}else{*_tmp18B=(struct Cyc_Absyn_Datatypedecl*)
_tmp192;goto _LLEA;}}_LLEA:;}};_pop_region(temp);}}void Cyc_Tc_tcEnumdecl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct
Cyc_Absyn_Enumdecl*ed){struct _dyneither_ptr*v=(*ed->name).f2;{union Cyc_Absyn_Nmspace
_tmp195=(*ed->name).f1;struct Cyc_List_List*_tmp196;struct Cyc_List_List*_tmp197;
_LLF4: if((_tmp195.Rel_n).tag != 1)goto _LLF6;_tmp196=(struct Cyc_List_List*)(
_tmp195.Rel_n).val;if(_tmp196 != 0)goto _LLF6;_LLF5: goto _LLF7;_LLF6: if((_tmp195.Abs_n).tag
!= 2)goto _LLF8;_tmp197=(struct Cyc_List_List*)(_tmp195.Abs_n).val;if(_tmp197 != 0)
goto _LLF8;_LLF7: goto _LLF3;_LLF8:;_LLF9:({struct Cyc_String_pa_struct _tmp19A;
_tmp19A.tag=0;_tmp19A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
ed->name));{void*_tmp198[1]={& _tmp19A};Cyc_Tcutil_terr(loc,({const char*_tmp199="qualified enum declarations are not implemented (%s)";
_tag_dyneither(_tmp199,sizeof(char),53);}),_tag_dyneither(_tmp198,sizeof(void*),
1));}});return;_LLF3:;}(*ed->name).f1=Cyc_Absyn_Abs_n(te->ns);if(ed->fields != 0){
struct _RegionHandle _tmp19B=_new_region("uprev_rgn");struct _RegionHandle*
uprev_rgn=& _tmp19B;_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp19C=(struct Cyc_Absyn_Enumfield*)fs->hd;if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp19C->name).f2))({
struct Cyc_String_pa_struct _tmp19F;_tmp19F.tag=0;_tmp19F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp19C->name).f2);{void*_tmp19D[1]={& _tmp19F};Cyc_Tcutil_terr(
_tmp19C->loc,({const char*_tmp19E="duplicate field name %s";_tag_dyneither(
_tmp19E,sizeof(char),24);}),_tag_dyneither(_tmp19D,sizeof(void*),1));}});else{
prev_fields=({struct Cyc_List_List*_tmp1A0=_region_malloc(uprev_rgn,sizeof(*
_tmp1A0));_tmp1A0->hd=(*_tmp19C->name).f2;_tmp1A0->tl=prev_fields;_tmp1A0;});}
if(_tmp19C->tag == 0)_tmp19C->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp19C->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp19C->tag)))({struct Cyc_String_pa_struct _tmp1A4;_tmp1A4.tag=0;
_tmp1A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp19C->name).f2);{
struct Cyc_String_pa_struct _tmp1A3;_tmp1A3.tag=0;_tmp1A3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*v);{void*_tmp1A1[2]={& _tmp1A3,& _tmp1A4};Cyc_Tcutil_terr(
loc,({const char*_tmp1A2="enum %s, field %s: expression is not constant";
_tag_dyneither(_tmp1A2,sizeof(char),46);}),_tag_dyneither(_tmp1A1,sizeof(void*),
2));}}});}{unsigned int _tmp1A6;int _tmp1A7;struct _tuple10 _tmp1A5=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp19C->tag));_tmp1A6=_tmp1A5.f1;_tmp1A7=
_tmp1A5.f2;if(!_tmp1A7)({void*_tmp1A8=0;Cyc_Tcutil_terr(loc,({const char*_tmp1A9="Cyclone enum tags cannot use sizeof or offsetof";
_tag_dyneither(_tmp1A9,sizeof(char),48);}),_tag_dyneither(_tmp1A8,sizeof(void*),
0));});tag_count=_tmp1A6 + 1;(*_tmp19C->name).f1=Cyc_Absyn_Abs_n(te->ns);}}};
_pop_region(uprev_rgn);}{struct _handler_cons _tmp1AA;_push_handler(& _tmp1AA);{int
_tmp1AC=0;if(setjmp(_tmp1AA.handler))_tmp1AC=1;if(!_tmp1AC){{struct Cyc_Absyn_Enumdecl**
_tmp1AD=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->enumdecls,v);struct Cyc_Absyn_Enumdecl*
_tmp1AE=Cyc_Tcdecl_merge_enumdecl(*_tmp1AD,ed,loc,Cyc_Tc_tc_msg);if(_tmp1AE == 0){
_npop_handler(0);return;}*_tmp1AD=(struct Cyc_Absyn_Enumdecl*)_tmp1AE;};
_pop_handler();}else{void*_tmp1AB=(void*)_exn_thrown;void*_tmp1B0=_tmp1AB;_LLFB:
if(_tmp1B0 != Cyc_Dict_Absent)goto _LLFD;_LLFC: {struct Cyc_Absyn_Enumdecl**_tmp1B1=({
struct Cyc_Absyn_Enumdecl**_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2[0]=ed;
_tmp1B2;});ge->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,v,
_tmp1B1);goto _LLFA;}_LLFD:;_LLFE:(void)_throw(_tmp1B0);_LLFA:;}}}if(ed->fields != 
0){struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp1B3=(struct Cyc_Absyn_Enumfield*)fs->hd;ge->ordinaries=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple5*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmp1B3->name).f2,(struct _tuple5*)({struct _tuple5*_tmp1B4=_cycalloc(sizeof(*
_tmp1B4));_tmp1B4->f1=(void*)({struct Cyc_Tcenv_EnumRes_struct*_tmp1B5=_cycalloc(
sizeof(*_tmp1B5));_tmp1B5[0]=({struct Cyc_Tcenv_EnumRes_struct _tmp1B6;_tmp1B6.tag=
3;_tmp1B6.f1=ed;_tmp1B6.f2=_tmp1B3;_tmp1B6;});_tmp1B5;});_tmp1B4->f2=1;_tmp1B4;}));}}}
static int Cyc_Tc_okay_externC(struct Cyc_Position_Segment*loc,void*sc){void*
_tmp1B7=sc;_LL100: if((int)_tmp1B7 != 0)goto _LL102;_LL101:({void*_tmp1B8=0;Cyc_Tcutil_warn(
loc,({const char*_tmp1B9="static declaration nested within extern \"C\"";
_tag_dyneither(_tmp1B9,sizeof(char),44);}),_tag_dyneither(_tmp1B8,sizeof(void*),
0));});return 0;_LL102: if((int)_tmp1B7 != 1)goto _LL104;_LL103:({void*_tmp1BA=0;Cyc_Tcutil_warn(
loc,({const char*_tmp1BB="abstract declaration nested within extern \"C\"";
_tag_dyneither(_tmp1BB,sizeof(char),46);}),_tag_dyneither(_tmp1BA,sizeof(void*),
0));});return 0;_LL104: if((int)_tmp1B7 != 2)goto _LL106;_LL105: goto _LL107;_LL106:
if((int)_tmp1B7 != 5)goto _LL108;_LL107: goto _LL109;_LL108: if((int)_tmp1B7 != 3)goto
_LL10A;_LL109: return 1;_LL10A: if((int)_tmp1B7 != 4)goto _LLFF;_LL10B:({void*_tmp1BC=
0;Cyc_Tcutil_warn(loc,({const char*_tmp1BD="nested extern \"C\" declaration";
_tag_dyneither(_tmp1BD,sizeof(char),30);}),_tag_dyneither(_tmp1BC,sizeof(void*),
0));});return 1;_LLFF:;}static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*
te,struct _tuple12*exp){struct Cyc_Position_Segment*_tmp1BF;struct _tuple0*_tmp1C0;
struct _tuple12 _tmp1BE=*exp;_tmp1BF=_tmp1BE.f1;_tmp1C0=_tmp1BE.f2;{struct _tuple0
_tmp1C2;union Cyc_Absyn_Nmspace _tmp1C3;struct _dyneither_ptr*_tmp1C4;struct _tuple0*
_tmp1C1=_tmp1C0;_tmp1C2=*_tmp1C1;_tmp1C3=_tmp1C2.f1;_tmp1C4=_tmp1C2.f2;{union Cyc_Absyn_Nmspace
_tmp1C5=_tmp1C3;struct Cyc_List_List*_tmp1C6;struct Cyc_List_List*_tmp1C7;_LL10D:
if((_tmp1C5.Rel_n).tag != 1)goto _LL10F;_tmp1C6=(struct Cyc_List_List*)(_tmp1C5.Rel_n).val;
if(_tmp1C6 != 0)goto _LL10F;_LL10E: goto _LL110;_LL10F: if((_tmp1C5.Abs_n).tag != 2)
goto _LL111;_tmp1C7=(struct Cyc_List_List*)(_tmp1C5.Abs_n).val;if(_tmp1C7 != 0)goto
_LL111;_LL110: goto _LL10C;_LL111:;_LL112:({struct Cyc_String_pa_struct _tmp1CA;
_tmp1CA.tag=0;_tmp1CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp1C0));{void*_tmp1C8[1]={& _tmp1CA};Cyc_Tcutil_terr(_tmp1BF,({const char*
_tmp1C9="qualified export variables are not implemented (%s)";_tag_dyneither(
_tmp1C9,sizeof(char),52);}),_tag_dyneither(_tmp1C8,sizeof(void*),1));}});return;
_LL10C:;}(*_tmp1C0).f1=Cyc_Absyn_Abs_n(te->ns);}}static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int
in_externCinclude,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**
exports){struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_List_List*last=
0;struct Cyc_Dict_Dict dict=Cyc_Tcgenrep_empty_typerep_dict();struct Cyc_List_List*
_tmp1CB=ds0;for(0;_tmp1CB != 0;(last=_tmp1CB,_tmp1CB=_tmp1CB->tl)){struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)_tmp1CB->hd;struct Cyc_Position_Segment*loc=d->loc;void*
_tmp1CC=d->r;struct Cyc_Absyn_Vardecl*_tmp1CD;struct Cyc_Absyn_Fndecl*_tmp1CE;
struct Cyc_Absyn_Typedefdecl*_tmp1CF;struct Cyc_Absyn_Aggrdecl*_tmp1D0;struct Cyc_Absyn_Datatypedecl*
_tmp1D1;struct Cyc_Absyn_Enumdecl*_tmp1D2;struct _dyneither_ptr*_tmp1D3;struct Cyc_List_List*
_tmp1D4;struct _tuple0*_tmp1D5;struct _tuple0 _tmp1D6;union Cyc_Absyn_Nmspace _tmp1D7;
struct _dyneither_ptr*_tmp1D8;struct Cyc_List_List*_tmp1D9;struct Cyc_List_List*
_tmp1DA;struct Cyc_List_List*_tmp1DB;struct Cyc_List_List*_tmp1DC;_LL114: if(
_tmp1CC <= (void*)2)goto _LL128;if(*((int*)_tmp1CC)!= 2)goto _LL116;_LL115: goto
_LL117;_LL116: if(*((int*)_tmp1CC)!= 3)goto _LL118;_LL117:({void*_tmp1DD=0;Cyc_Tcutil_terr(
loc,({const char*_tmp1DE="top level let-declarations are not implemented";
_tag_dyneither(_tmp1DE,sizeof(char),47);}),_tag_dyneither(_tmp1DD,sizeof(void*),
0));});goto _LL113;_LL118: if(*((int*)_tmp1CC)!= 4)goto _LL11A;_LL119:({void*
_tmp1DF=0;Cyc_Tcutil_terr(loc,({const char*_tmp1E0="top level region declarations are not implemented";
_tag_dyneither(_tmp1E0,sizeof(char),50);}),_tag_dyneither(_tmp1DF,sizeof(void*),
0));});goto _LL113;_LL11A: if(*((int*)_tmp1CC)!= 5)goto _LL11C;_LL11B:({void*
_tmp1E1=0;Cyc_Tcutil_terr(loc,({const char*_tmp1E2="top level alias declarations are not implemented";
_tag_dyneither(_tmp1E2,sizeof(char),49);}),_tag_dyneither(_tmp1E1,sizeof(void*),
0));});goto _LL113;_LL11C: if(*((int*)_tmp1CC)!= 0)goto _LL11E;_tmp1CD=((struct Cyc_Absyn_Var_d_struct*)
_tmp1CC)->f1;_LL11D: if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1CD->sc))
_tmp1CD->sc=(void*)4;if(_tmp1CD->initializer != 0){void*_tmp1E3=((struct Cyc_Absyn_Exp*)
_check_null(_tmp1CD->initializer))->r;void*_tmp1E4;_LL135: if(*((int*)_tmp1E3)!= 
21)goto _LL137;_tmp1E4=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp1E3)->f2;
_LL136: {struct Cyc_Dict_Dict _tmp1E6;struct Cyc_List_List*_tmp1E7;struct Cyc_Absyn_Exp*
_tmp1E8;struct _tuple11 _tmp1E5=Cyc_Tcgenrep_tcGenrep(te,ge,loc,_tmp1E4,dict);
_tmp1E6=_tmp1E5.f1;_tmp1E7=_tmp1E5.f2;_tmp1E8=_tmp1E5.f3;dict=_tmp1E6;Cyc_Tc_tc_decls(
te,_tmp1E7,in_externC,in_externCinclude,check_var_init,grgn,exports);_tmp1CD->initializer=(
struct Cyc_Absyn_Exp*)_tmp1E8;Cyc_Tc_tcVardecl(te,ge,loc,_tmp1CD,check_var_init,
in_externCinclude,exports);if(_tmp1E7 != 0){if(last != 0){((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1E7,
_tmp1CB);last->tl=_tmp1E7;}else{struct Cyc_List_List tmp=({struct Cyc_List_List
_tmp1E9;_tmp1E9.hd=(struct Cyc_Absyn_Decl*)_tmp1CB->hd;_tmp1E9.tl=_tmp1CB->tl;
_tmp1E9;});_tmp1CB->hd=(void*)((struct Cyc_Absyn_Decl*)_tmp1E7->hd);_tmp1CB->tl=
_tmp1E7->tl;_tmp1E7->hd=(void*)((struct Cyc_Absyn_Decl*)tmp.hd);_tmp1E7->tl=tmp.tl;((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp1CB,_tmp1E7);}}continue;}_LL137:;_LL138: goto _LL134;_LL134:;}Cyc_Tc_tcVardecl(
te,ge,loc,_tmp1CD,check_var_init,in_externCinclude,exports);goto _LL113;_LL11E:
if(*((int*)_tmp1CC)!= 1)goto _LL120;_tmp1CE=((struct Cyc_Absyn_Fn_d_struct*)
_tmp1CC)->f1;_LL11F: if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1CE->sc))
_tmp1CE->sc=(void*)4;Cyc_Tc_tcFndecl(te,ge,loc,_tmp1CE,in_externCinclude,exports);
goto _LL113;_LL120: if(*((int*)_tmp1CC)!= 9)goto _LL122;_tmp1CF=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp1CC)->f1;_LL121: Cyc_Tc_tcTypedefdecl(te,ge,loc,_tmp1CF);goto _LL113;_LL122:
if(*((int*)_tmp1CC)!= 6)goto _LL124;_tmp1D0=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp1CC)->f1;_LL123: if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1D0->sc))
_tmp1D0->sc=(void*)4;Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp1D0);goto _LL113;_LL124: if(*((
int*)_tmp1CC)!= 7)goto _LL126;_tmp1D1=((struct Cyc_Absyn_Datatype_d_struct*)
_tmp1CC)->f1;_LL125: if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1D1->sc))
_tmp1D1->sc=(void*)4;Cyc_Tc_tcDatatypedecl(te,ge,loc,_tmp1D1);goto _LL113;_LL126:
if(*((int*)_tmp1CC)!= 8)goto _LL128;_tmp1D2=((struct Cyc_Absyn_Enum_d_struct*)
_tmp1CC)->f1;_LL127: if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1D2->sc))
_tmp1D2->sc=(void*)4;Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp1D2);goto _LL113;_LL128: if((
int)_tmp1CC != 0)goto _LL12A;_LL129:({void*_tmp1EA=0;Cyc_Tcutil_warn(d->loc,({
const char*_tmp1EB="spurious __cyclone_port_on__";_tag_dyneither(_tmp1EB,sizeof(
char),29);}),_tag_dyneither(_tmp1EA,sizeof(void*),0));});goto _LL113;_LL12A: if((
int)_tmp1CC != 1)goto _LL12C;_LL12B: goto _LL113;_LL12C: if(_tmp1CC <= (void*)2)goto
_LL12E;if(*((int*)_tmp1CC)!= 10)goto _LL12E;_tmp1D3=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp1CC)->f1;_tmp1D4=((struct Cyc_Absyn_Namespace_d_struct*)_tmp1CC)->f2;_LL12D: {
struct Cyc_List_List*_tmp1EC=te->ns;struct Cyc_List_List*_tmp1ED=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1EC,({struct
Cyc_List_List*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE->hd=_tmp1D3;_tmp1EE->tl=
0;_tmp1EE;}));if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
ge->namespaces,_tmp1D3)){ge->namespaces=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(
grgn,ge->namespaces,_tmp1D3);te->ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp1ED,
Cyc_Tcenv_empty_genv(grgn));}te->ns=_tmp1ED;Cyc_Tc_tc_decls(te,_tmp1D4,
in_externC,in_externCinclude,check_var_init,grgn,exports);te->ns=_tmp1EC;goto
_LL113;}_LL12E: if(_tmp1CC <= (void*)2)goto _LL130;if(*((int*)_tmp1CC)!= 11)goto
_LL130;_tmp1D5=((struct Cyc_Absyn_Using_d_struct*)_tmp1CC)->f1;_tmp1D6=*_tmp1D5;
_tmp1D7=_tmp1D6.f1;_tmp1D8=_tmp1D6.f2;_tmp1D9=((struct Cyc_Absyn_Using_d_struct*)
_tmp1CC)->f2;_LL12F: {struct _dyneither_ptr*first;struct Cyc_List_List*rest;{union
Cyc_Absyn_Nmspace _tmp1EF=_tmp1D7;int _tmp1F0;struct Cyc_List_List*_tmp1F1;struct
Cyc_List_List*_tmp1F2;struct Cyc_List_List*_tmp1F3;struct Cyc_List_List _tmp1F4;
struct _dyneither_ptr*_tmp1F5;struct Cyc_List_List*_tmp1F6;struct Cyc_List_List*
_tmp1F7;struct Cyc_List_List _tmp1F8;struct _dyneither_ptr*_tmp1F9;struct Cyc_List_List*
_tmp1FA;_LL13A: if((_tmp1EF.Loc_n).tag != 3)goto _LL13C;_tmp1F0=(int)(_tmp1EF.Loc_n).val;
_LL13B: goto _LL13D;_LL13C: if((_tmp1EF.Rel_n).tag != 1)goto _LL13E;_tmp1F1=(struct
Cyc_List_List*)(_tmp1EF.Rel_n).val;if(_tmp1F1 != 0)goto _LL13E;_LL13D: goto _LL13F;
_LL13E: if((_tmp1EF.Abs_n).tag != 2)goto _LL140;_tmp1F2=(struct Cyc_List_List*)(
_tmp1EF.Abs_n).val;if(_tmp1F2 != 0)goto _LL140;_LL13F: first=_tmp1D8;rest=0;goto
_LL139;_LL140: if((_tmp1EF.Rel_n).tag != 1)goto _LL142;_tmp1F3=(struct Cyc_List_List*)(
_tmp1EF.Rel_n).val;if(_tmp1F3 == 0)goto _LL142;_tmp1F4=*_tmp1F3;_tmp1F5=(struct
_dyneither_ptr*)_tmp1F4.hd;_tmp1F6=_tmp1F4.tl;_LL141: _tmp1F9=_tmp1F5;_tmp1FA=
_tmp1F6;goto _LL143;_LL142: if((_tmp1EF.Abs_n).tag != 2)goto _LL139;_tmp1F7=(struct
Cyc_List_List*)(_tmp1EF.Abs_n).val;if(_tmp1F7 == 0)goto _LL139;_tmp1F8=*_tmp1F7;
_tmp1F9=(struct _dyneither_ptr*)_tmp1F8.hd;_tmp1FA=_tmp1F8.tl;_LL143: first=
_tmp1F9;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1FA,({struct Cyc_List_List*_tmp1FB=_cycalloc(sizeof(*
_tmp1FB));_tmp1FB->hd=_tmp1D8;_tmp1FB->tl=0;_tmp1FB;}));goto _LL139;_LL139:;}{
struct Cyc_List_List*_tmp1FC=Cyc_Tcenv_resolve_namespace(te,loc,first,rest);ge->availables=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));
_tmp1FD->hd=_tmp1FC;_tmp1FD->tl=ge->availables;_tmp1FD;});Cyc_Tc_tc_decls(te,
_tmp1D9,in_externC,in_externCinclude,check_var_init,grgn,exports);ge->availables=((
struct Cyc_List_List*)_check_null(ge->availables))->tl;goto _LL113;}}_LL130: if(
_tmp1CC <= (void*)2)goto _LL132;if(*((int*)_tmp1CC)!= 12)goto _LL132;_tmp1DA=((
struct Cyc_Absyn_ExternC_d_struct*)_tmp1CC)->f1;_LL131: Cyc_Tc_tc_decls(te,_tmp1DA,
1,in_externCinclude,check_var_init,grgn,exports);goto _LL113;_LL132: if(_tmp1CC <= (
void*)2)goto _LL113;if(*((int*)_tmp1CC)!= 13)goto _LL113;_tmp1DB=((struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp1CC)->f1;_tmp1DC=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp1CC)->f2;
_LL133:((void(*)(void(*f)(struct Cyc_Tcenv_Tenv*,struct _tuple12*),struct Cyc_Tcenv_Tenv*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tc_resolve_export_namespace,te,
_tmp1DC);{struct Cyc_List_List*newexs=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1DC,(unsigned int)exports?*exports:
0);Cyc_Tc_tc_decls(te,_tmp1DB,1,1,check_var_init,grgn,(struct Cyc_List_List**)&
newexs);for(0;_tmp1DC != 0;_tmp1DC=_tmp1DC->tl){struct _tuple12*_tmp1FE=(struct
_tuple12*)_tmp1DC->hd;if(!(*_tmp1FE).f3)({struct Cyc_String_pa_struct _tmp201;
_tmp201.tag=0;_tmp201.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((*
_tmp1FE).f2));{void*_tmp1FF[1]={& _tmp201};Cyc_Tcutil_warn((*_tmp1FE).f1,({const
char*_tmp200="%s is exported but not defined";_tag_dyneither(_tmp200,sizeof(char),
31);}),_tag_dyneither(_tmp1FF,sizeof(void*),1));}});}goto _LL113;}_LL113:;}}void
Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*te,int check_var_init,struct
Cyc_List_List*ds){Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);Cyc_Tc_tc_decls(
te,ds,0,0,check_var_init,g,0);}struct Cyc_Tc_TreeshakeEnv{struct _RegionHandle*rgn;
int in_cinclude;struct Cyc_Dict_Dict nsdict;};static int Cyc_Tc_vardecl_needed(struct
Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);static struct Cyc_List_List*Cyc_Tc_treeshake_f(
struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){return((struct Cyc_List_List*(*)(
int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*
env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}
struct _tuple17{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};static int Cyc_Tc_vardecl_needed(
struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){void*_tmp202=d->r;struct
Cyc_Absyn_Vardecl*_tmp203;struct Cyc_List_List*_tmp204;struct Cyc_List_List**
_tmp205;struct Cyc_List_List*_tmp206;struct Cyc_List_List**_tmp207;struct Cyc_List_List*
_tmp208;struct Cyc_List_List**_tmp209;struct Cyc_List_List*_tmp20A;struct Cyc_List_List**
_tmp20B;_LL145: if(_tmp202 <= (void*)2)goto _LL14F;if(*((int*)_tmp202)!= 0)goto
_LL147;_tmp203=((struct Cyc_Absyn_Var_d_struct*)_tmp202)->f1;_LL146: if(env->in_cinclude
 || _tmp203->sc != (void*)3  && _tmp203->sc != (void*)4)return 1;{struct _tuple0
_tmp20D;union Cyc_Absyn_Nmspace _tmp20E;struct _dyneither_ptr*_tmp20F;struct _tuple0*
_tmp20C=_tmp203->name;_tmp20D=*_tmp20C;_tmp20E=_tmp20D.f1;_tmp20F=_tmp20D.f2;{
struct Cyc_List_List*ns;{union Cyc_Absyn_Nmspace _tmp210=_tmp20E;int _tmp211;struct
Cyc_List_List*_tmp212;struct Cyc_List_List*_tmp213;_LL152: if((_tmp210.Loc_n).tag
!= 3)goto _LL154;_tmp211=(int)(_tmp210.Loc_n).val;_LL153: ns=0;goto _LL151;_LL154:
if((_tmp210.Rel_n).tag != 1)goto _LL156;_tmp212=(struct Cyc_List_List*)(_tmp210.Rel_n).val;
_LL155: ns=_tmp212;goto _LL151;_LL156: if((_tmp210.Abs_n).tag != 2)goto _LL151;
_tmp213=(struct Cyc_List_List*)(_tmp210.Abs_n).val;_LL157: ns=_tmp213;goto _LL151;
_LL151:;}{struct _tuple17*_tmp214=((struct _tuple17*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(env->nsdict,ns);struct Cyc_Tcenv_Genv*
_tmp215=(*_tmp214).f1;int _tmp216=(*((struct _tuple5*(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp215->ordinaries,_tmp20F)).f2;if(!
_tmp216)(*_tmp214).f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*
s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(env->rgn,(*_tmp214).f2,_tmp20F);
return _tmp216;}}}_LL147: if(*((int*)_tmp202)!= 12)goto _LL149;_tmp204=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp202)->f1;_tmp205=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_struct*)
_tmp202)->f1;_LL148: _tmp207=_tmp205;goto _LL14A;_LL149: if(*((int*)_tmp202)!= 11)
goto _LL14B;_tmp206=((struct Cyc_Absyn_Using_d_struct*)_tmp202)->f2;_tmp207=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_struct*)_tmp202)->f2;_LL14A:
_tmp209=_tmp207;goto _LL14C;_LL14B: if(*((int*)_tmp202)!= 10)goto _LL14D;_tmp208=((
struct Cyc_Absyn_Namespace_d_struct*)_tmp202)->f2;_tmp209=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Namespace_d_struct*)_tmp202)->f2;_LL14C:*_tmp209=Cyc_Tc_treeshake_f(
env,*_tmp209);return 1;_LL14D: if(*((int*)_tmp202)!= 13)goto _LL14F;_tmp20A=((
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp202)->f1;_tmp20B=(struct Cyc_List_List**)&((
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp202)->f1;_LL14E: {int in_cinclude=
env->in_cinclude;env->in_cinclude=1;*_tmp20B=Cyc_Tc_treeshake_f(env,*_tmp20B);
env->in_cinclude=in_cinclude;return 1;}_LL14F:;_LL150: return 1;_LL144:;}static int
Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,void*y){
return !((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
set,x);}static struct _tuple17*Cyc_Tc_treeshake_make_env_f(struct _RegionHandle*rgn,
struct Cyc_Tcenv_Genv*ge){return({struct _tuple17*_tmp217=_region_malloc(rgn,
sizeof(*_tmp217));_tmp217->f1=ge;_tmp217->f2=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(
rgn,Cyc_strptrcmp);_tmp217;});}struct _tuple18{struct Cyc_List_List*f1;struct
_tuple17*f2;};struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*ds){struct _RegionHandle _tmp218=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp218;_push_region(rgn);{struct Cyc_Tc_TreeshakeEnv _tmp219=({
struct Cyc_Tc_TreeshakeEnv _tmp224;_tmp224.rgn=rgn;_tmp224.in_cinclude=0;_tmp224.nsdict=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct _tuple17*(*f)(struct
_RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict
d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae);_tmp224;});
struct Cyc_List_List*_tmp21A=Cyc_Tc_treeshake_f(& _tmp219,ds);if(((int(*)(struct
Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp219.nsdict)){struct Cyc_List_List*_tmp21B=
_tmp21A;_npop_handler(0);return _tmp21B;}{struct Cyc_Iter_Iter _tmp21C=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,
_tmp219.nsdict);struct _tuple18 _tmp21D=*((struct _tuple18*(*)(struct _RegionHandle*
r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(rgn,_tmp219.nsdict);while(((int(*)(
struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(_tmp21C,& _tmp21D)){struct
_tuple17*_tmp21F;struct _tuple17 _tmp220;struct Cyc_Tcenv_Genv*_tmp221;struct Cyc_Set_Set*
_tmp222;struct _tuple18 _tmp21E=_tmp21D;_tmp21F=_tmp21E.f2;_tmp220=*_tmp21F;
_tmp221=_tmp220.f1;_tmp222=_tmp220.f2;_tmp221->ordinaries=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*f)(struct Cyc_Set_Set*,struct _dyneither_ptr*,struct
_tuple5*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(
_tmp221->grgn,(int(*)(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,struct _tuple5*
y))Cyc_Tc_treeshake_remove_f,_tmp222,_tmp221->ordinaries);}{struct Cyc_List_List*
_tmp223=_tmp21A;_npop_handler(0);return _tmp223;}}};_pop_region(rgn);}
