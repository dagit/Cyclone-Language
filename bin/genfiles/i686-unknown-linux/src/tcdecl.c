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
struct _dyneither_ptr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_NewRegion{
struct _DynRegionHandle*dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[16];
extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dyneither_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,
struct _dyneither_ptr);extern char Cyc_Position_Nocontext[14];void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple0*,struct _tuple0*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);void*Cyc_Absyn_force_kb(void*kb);struct _dyneither_ptr Cyc_Absyn_attribute2string(
void*);extern char Cyc_Tcdecl_Incompatible[17];struct Cyc_Tcdecl_Xdatatypefielddecl{
struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};void Cyc_Tcdecl_merr(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);struct _tuple3{void*f1;int f2;};struct _tuple3 Cyc_Tcdecl_merge_scope(
void*s0,void*s1,struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg,int allow_externC_extern_merge);struct Cyc_Absyn_Aggrdecl*
Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Datatypedecl*
Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Enumdecl*
Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Typedefdecl*
Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);void*Cyc_Tcdecl_merge_binding(
void*d0,void*d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct
Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*
d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg);struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct
Cyc_List_List*f,int*res,struct _dyneither_ptr*v,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr*msg);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple4{void*f1;void*f2;};struct _tuple4*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple4*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(struct
_dyneither_ptr,struct _dyneither_ptr);struct Cyc_Set_Set;extern char Cyc_Set_Absent[
11];struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
le;int allow_valueof;};void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,
void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);int Cyc_Tcutil_same_atts(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(void*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*);struct _dyneither_ptr Cyc_Absynpp_scope2string(void*sc);char Cyc_Tcdecl_Incompatible[
17]="\000\000\000\000Incompatible\000";void Cyc_Tcdecl_merr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
if(msg1 == 0)(int)_throw((void*)Cyc_Tcdecl_Incompatible);{struct _dyneither_ptr
fmt2=(struct _dyneither_ptr)Cyc_strconcat(({const char*_tmp6="%s ";_tag_dyneither(
_tmp6,sizeof(char),4);}),(struct _dyneither_ptr)fmt);struct _dyneither_ptr ap2=({
unsigned int _tmp0=_get_dyneither_size(ap,sizeof(void*))+ 1;void**_tmp1=(void**)
_cycalloc(_check_times(sizeof(void*),_tmp0));struct _dyneither_ptr _tmp5=
_tag_dyneither(_tmp1,sizeof(void*),_tmp0);{unsigned int _tmp2=_tmp0;unsigned int i;
for(i=0;i < _tmp2;i ++){_tmp1[i]=i == 0?(void*)({struct Cyc_String_pa_struct*_tmp3=
_cycalloc(sizeof(*_tmp3));_tmp3[0]=({struct Cyc_String_pa_struct _tmp4;_tmp4.tag=0;
_tmp4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*msg1);_tmp4;});_tmp3;}):*((
void**)_check_dyneither_subscript(ap,sizeof(void*),(int)(i - 1)));}}_tmp5;});Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt2,ap2)));}}static void Cyc_Tcdecl_merge_scope_err(void*s0,void*s1,struct
_dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){({struct Cyc_String_pa_struct _tmpC;_tmpC.tag=0;_tmpC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_scope2string(s0));{
struct Cyc_String_pa_struct _tmpB;_tmpB.tag=0;_tmpB.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_scope2string(s1));{struct Cyc_String_pa_struct
_tmpA;_tmpA.tag=0;_tmpA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);{
struct Cyc_String_pa_struct _tmp9;_tmp9.tag=0;_tmp9.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);{void*_tmp7[4]={& _tmp9,& _tmpA,& _tmpB,& _tmpC};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp8="%s %s is %s whereas expected scope is %s";
_tag_dyneither(_tmp8,sizeof(char),41);}),_tag_dyneither(_tmp7,sizeof(void*),4));}}}}});}
struct _tuple3 Cyc_Tcdecl_merge_scope(void*s0,void*s1,struct _dyneither_ptr t,struct
_dyneither_ptr v,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg,int
externCmerge){{struct _tuple4 _tmpE=({struct _tuple4 _tmpD;_tmpD.f1=s0;_tmpD.f2=s1;
_tmpD;});void*_tmpF;void*_tmp10;void*_tmp11;void*_tmp12;void*_tmp13;void*_tmp14;
void*_tmp15;void*_tmp16;void*_tmp17;void*_tmp18;void*_tmp19;void*_tmp1A;void*
_tmp1B;void*_tmp1C;void*_tmp1D;void*_tmp1E;void*_tmp1F;void*_tmp20;_LL1: _tmpF=
_tmpE.f1;if((int)_tmpF != 4)goto _LL3;_tmp10=_tmpE.f2;if((int)_tmp10 != 4)goto _LL3;
_LL2: goto _LL0;_LL3: _tmp11=_tmpE.f1;if((int)_tmp11 != 4)goto _LL5;_tmp12=_tmpE.f2;
if((int)_tmp12 != 3)goto _LL5;_LL4: goto _LL6;_LL5: _tmp13=_tmpE.f1;if((int)_tmp13 != 
3)goto _LL7;_tmp14=_tmpE.f2;if((int)_tmp14 != 4)goto _LL7;_LL6: if(externCmerge)goto
_LL0;goto _LL8;_LL7: _tmp15=_tmpE.f1;if((int)_tmp15 != 4)goto _LL9;_LL8: goto _LLA;
_LL9: _tmp16=_tmpE.f2;if((int)_tmp16 != 4)goto _LLB;_LLA: Cyc_Tcdecl_merge_scope_err(
s0,s1,t,v,loc,msg);return({struct _tuple3 _tmp21;_tmp21.f1=s1;_tmp21.f2=0;_tmp21;});
_LLB: _tmp17=_tmpE.f2;if((int)_tmp17 != 3)goto _LLD;_LLC: s1=s0;goto _LL0;_LLD: _tmp18=
_tmpE.f1;if((int)_tmp18 != 3)goto _LLF;_LLE: goto _LL0;_LLF: _tmp19=_tmpE.f1;if((int)
_tmp19 != 0)goto _LL11;_tmp1A=_tmpE.f2;if((int)_tmp1A != 0)goto _LL11;_LL10: goto
_LL12;_LL11: _tmp1B=_tmpE.f1;if((int)_tmp1B != 2)goto _LL13;_tmp1C=_tmpE.f2;if((int)
_tmp1C != 2)goto _LL13;_LL12: goto _LL14;_LL13: _tmp1D=_tmpE.f1;if((int)_tmp1D != 1)
goto _LL15;_tmp1E=_tmpE.f2;if((int)_tmp1E != 1)goto _LL15;_LL14: goto _LL0;_LL15:
_tmp1F=_tmpE.f1;if((int)_tmp1F != 5)goto _LL17;_tmp20=_tmpE.f2;if((int)_tmp20 != 5)
goto _LL17;_LL16: goto _LL0;_LL17:;_LL18: Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,
msg);return({struct _tuple3 _tmp22;_tmp22.f1=s1;_tmp22.f2=0;_tmp22;});_LL0:;}
return({struct _tuple3 _tmp23;_tmp23.f1=s1;_tmp23.f2=1;_tmp23;});}static int Cyc_Tcdecl_check_type(
void*t0,void*t1){return Cyc_Tcutil_unify(t0,t1);}static unsigned int Cyc_Tcdecl_get_uint_const_value(
struct Cyc_Absyn_Exp*e){void*_tmp24=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp25;
int _tmp26;_LL1A: if(*((int*)_tmp24)!= 0)goto _LL1C;_tmp25=((struct Cyc_Absyn_Const_e_struct*)
_tmp24)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp24)->f1).Int_c).tag != 2)
goto _LL1C;_tmp26=(_tmp25.Int_c).f2;_LL1B: return(unsigned int)_tmp26;_LL1C:;_LL1D:(
int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp27=_cycalloc(
sizeof(*_tmp27));_tmp27[0]=({struct Cyc_Core_Invalid_argument_struct _tmp28;_tmp28.tag=
Cyc_Core_Invalid_argument;_tmp28.f1=({const char*_tmp29="Tcdecl::get_uint_const_value";
_tag_dyneither(_tmp29,sizeof(char),29);});_tmp28;});_tmp27;}));_LL19:;}inline
static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,
struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr*msg){if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
tvs0)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1)){({struct Cyc_String_pa_struct
_tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);{
struct Cyc_String_pa_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);{void*_tmp2A[2]={& _tmp2C,& _tmp2D};Cyc_Tcdecl_merr(loc,
msg,({const char*_tmp2B="%s %s has a different number of type parameters";
_tag_dyneither(_tmp2B,sizeof(char),48);}),_tag_dyneither(_tmp2A,sizeof(void*),2));}}});
return 0;}{struct Cyc_List_List*_tmp2E=tvs0;struct Cyc_List_List*_tmp2F=tvs1;for(0;
_tmp2E != 0;(_tmp2E=_tmp2E->tl,_tmp2F=_tmp2F->tl)){void*_tmp30=Cyc_Absyn_force_kb((
void*)((struct Cyc_Absyn_Tvar*)_tmp2E->hd)->kind);void*_tmp31=Cyc_Absyn_force_kb((
void*)((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp2F))->hd)->kind);
if(_tmp30 == _tmp31)continue;({struct Cyc_String_pa_struct _tmp38;_tmp38.tag=0;
_tmp38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp31));{struct Cyc_String_pa_struct _tmp37;_tmp37.tag=0;_tmp37.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp2E->hd)->name);{
struct Cyc_String_pa_struct _tmp36;_tmp36.tag=0;_tmp36.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp30));{struct Cyc_String_pa_struct
_tmp35;_tmp35.tag=0;_tmp35.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);{
struct Cyc_String_pa_struct _tmp34;_tmp34.tag=0;_tmp34.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);{void*_tmp32[5]={& _tmp34,& _tmp35,& _tmp36,& _tmp37,& _tmp38};
Cyc_Tcdecl_merr(loc,msg,({const char*_tmp33="%s %s has a different kind (%s) for type parameter %s (%s)";
_tag_dyneither(_tmp33,sizeof(char),59);}),_tag_dyneither(_tmp32,sizeof(void*),5));}}}}}});
return 0;}return 1;}}static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,
struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg){if(!Cyc_Tcutil_same_atts(atts0,atts1)){({struct Cyc_String_pa_struct
_tmp3C;_tmp3C.tag=0;_tmp3C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);{
struct Cyc_String_pa_struct _tmp3B;_tmp3B.tag=0;_tmp3B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);{void*_tmp39[2]={& _tmp3B,& _tmp3C};Cyc_Tcdecl_merr(loc,
msg,({const char*_tmp3A="%s %s has different attributes";_tag_dyneither(_tmp3A,
sizeof(char),31);}),_tag_dyneither(_tmp39,sizeof(void*),2));}}});return 0;}return
1;}struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(
struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){struct Cyc_List_List*inst=0;
for(0;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){inst=({struct Cyc_List_List*_tmp3D=
_cycalloc(sizeof(*_tmp3D));_tmp3D->hd=({struct _tuple5*_tmp3E=_cycalloc(sizeof(*
_tmp3E));_tmp3E->f1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
tvs1))->hd;_tmp3E->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp3F=_cycalloc(
sizeof(*_tmp3F));_tmp3F[0]=({struct Cyc_Absyn_VarType_struct _tmp40;_tmp40.tag=1;
_tmp40.f1=(struct Cyc_Absyn_Tvar*)tvs0->hd;_tmp40;});_tmp3F;});_tmp3E;});_tmp3D->tl=
inst;_tmp3D;});}return inst;}struct _tuple6{struct Cyc_Absyn_AggrdeclImpl*f1;struct
Cyc_Absyn_AggrdeclImpl*f2;};struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(
struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg){struct _dyneither_ptr _tmp41=Cyc_Absynpp_qvar2string(
d0->name);int _tmp42=1;if(!((void*)d0->kind == (void*)d1->kind))return 0;if(!Cyc_Tcdecl_check_tvs(
d0->tvs,d1->tvs,({const char*_tmp43="type";_tag_dyneither(_tmp43,sizeof(char),5);}),
_tmp41,loc,msg))return 0;{void*_tmp46;int _tmp47;struct _tuple3 _tmp45=Cyc_Tcdecl_merge_scope((
void*)d0->sc,(void*)d1->sc,({const char*_tmp44="type";_tag_dyneither(_tmp44,
sizeof(char),5);}),_tmp41,loc,msg,1);_tmp46=_tmp45.f1;_tmp47=_tmp45.f2;if(!
_tmp47)_tmp42=0;if(!Cyc_Tcdecl_check_atts(d0->attributes,d1->attributes,({const
char*_tmp48="type";_tag_dyneither(_tmp48,sizeof(char),5);}),_tmp41,loc,msg))
_tmp42=0;{struct Cyc_Absyn_Aggrdecl*d2;{struct _tuple6 _tmp4A=({struct _tuple6 _tmp49;
_tmp49.f1=d0->impl;_tmp49.f2=d1->impl;_tmp49;});struct Cyc_Absyn_AggrdeclImpl*
_tmp4B;struct Cyc_Absyn_AggrdeclImpl*_tmp4C;struct Cyc_Absyn_AggrdeclImpl*_tmp4D;
struct Cyc_Absyn_AggrdeclImpl _tmp4E;struct Cyc_List_List*_tmp4F;struct Cyc_List_List*
_tmp50;struct Cyc_List_List*_tmp51;int _tmp52;struct Cyc_Absyn_AggrdeclImpl*_tmp53;
struct Cyc_Absyn_AggrdeclImpl _tmp54;struct Cyc_List_List*_tmp55;struct Cyc_List_List*
_tmp56;struct Cyc_List_List*_tmp57;int _tmp58;_LL1F: _tmp4B=_tmp4A.f2;if(_tmp4B != 0)
goto _LL21;_LL20: d2=d0;goto _LL1E;_LL21: _tmp4C=_tmp4A.f1;if(_tmp4C != 0)goto _LL23;
_LL22: d2=d1;goto _LL1E;_LL23: _tmp4D=_tmp4A.f1;if(_tmp4D == 0)goto _LL1E;_tmp4E=*
_tmp4D;_tmp4F=_tmp4E.exist_vars;_tmp50=_tmp4E.rgn_po;_tmp51=_tmp4E.fields;_tmp52=
_tmp4E.tagged;_tmp53=_tmp4A.f2;if(_tmp53 == 0)goto _LL1E;_tmp54=*_tmp53;_tmp55=
_tmp54.exist_vars;_tmp56=_tmp54.rgn_po;_tmp57=_tmp54.fields;_tmp58=_tmp54.tagged;
_LL24: if(!Cyc_Tcdecl_check_tvs(_tmp4F,_tmp55,({const char*_tmp59="type";
_tag_dyneither(_tmp59,sizeof(char),5);}),_tmp41,loc,msg))return 0;{struct Cyc_List_List*
_tmp5A=Cyc_Tcdecl_build_tvs_map(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(d0->tvs,_tmp4F),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d1->tvs,_tmp55));
for(0;_tmp50 != 0  && _tmp56 != 0;(_tmp50=_tmp50->tl,_tmp56=_tmp56->tl)){Cyc_Tcdecl_check_type((*((
struct _tuple4*)_tmp50->hd)).f1,(*((struct _tuple4*)_tmp56->hd)).f1);Cyc_Tcdecl_check_type((*((
struct _tuple4*)_tmp50->hd)).f2,(*((struct _tuple4*)_tmp56->hd)).f2);}for(0;_tmp51
!= 0  && _tmp57 != 0;(_tmp51=_tmp51->tl,_tmp57=_tmp57->tl)){struct Cyc_Absyn_Aggrfield
_tmp5C;struct _dyneither_ptr*_tmp5D;struct Cyc_Absyn_Tqual _tmp5E;void*_tmp5F;
struct Cyc_Absyn_Exp*_tmp60;struct Cyc_List_List*_tmp61;struct Cyc_Absyn_Aggrfield*
_tmp5B=(struct Cyc_Absyn_Aggrfield*)_tmp51->hd;_tmp5C=*_tmp5B;_tmp5D=_tmp5C.name;
_tmp5E=_tmp5C.tq;_tmp5F=(void*)_tmp5C.type;_tmp60=_tmp5C.width;_tmp61=_tmp5C.attributes;{
struct Cyc_Absyn_Aggrfield _tmp63;struct _dyneither_ptr*_tmp64;struct Cyc_Absyn_Tqual
_tmp65;void*_tmp66;struct Cyc_Absyn_Exp*_tmp67;struct Cyc_List_List*_tmp68;struct
Cyc_Absyn_Aggrfield*_tmp62=(struct Cyc_Absyn_Aggrfield*)_tmp57->hd;_tmp63=*_tmp62;
_tmp64=_tmp63.name;_tmp65=_tmp63.tq;_tmp66=(void*)_tmp63.type;_tmp67=_tmp63.width;
_tmp68=_tmp63.attributes;if(Cyc_strptrcmp(_tmp5D,_tmp64)!= 0){({struct Cyc_String_pa_struct
_tmp6E;_tmp6E.tag=0;_tmp6E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp64);{struct Cyc_String_pa_struct _tmp6D;_tmp6D.tag=0;_tmp6D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp5D);{struct Cyc_String_pa_struct _tmp6C;
_tmp6C.tag=0;_tmp6C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp41);{
struct Cyc_String_pa_struct _tmp6B;_tmp6B.tag=0;_tmp6B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)({const char*_tmp6F="type";_tag_dyneither(_tmp6F,sizeof(char),
5);}));{void*_tmp69[4]={& _tmp6B,& _tmp6C,& _tmp6D,& _tmp6E};Cyc_Tcdecl_merr(loc,msg,({
const char*_tmp6A="%s %s : field name mismatch %s != %s";_tag_dyneither(_tmp6A,
sizeof(char),37);}),_tag_dyneither(_tmp69,sizeof(void*),4));}}}}});return 0;}if(!
Cyc_Tcutil_same_atts(_tmp61,_tmp68)){({struct Cyc_String_pa_struct _tmp74;_tmp74.tag=
0;_tmp74.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp5D);{struct Cyc_String_pa_struct
_tmp73;_tmp73.tag=0;_tmp73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp41);{struct Cyc_String_pa_struct _tmp72;_tmp72.tag=0;_tmp72.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)({const char*_tmp75="type";_tag_dyneither(
_tmp75,sizeof(char),5);}));{void*_tmp70[3]={& _tmp72,& _tmp73,& _tmp74};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp71="%s %s : attribute mismatch on field %s";
_tag_dyneither(_tmp71,sizeof(char),39);}),_tag_dyneither(_tmp70,sizeof(void*),3));}}}});
_tmp42=0;}if(!Cyc_Tcutil_equal_tqual(_tmp5E,_tmp65)){({struct Cyc_String_pa_struct
_tmp7A;_tmp7A.tag=0;_tmp7A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp5D);{struct Cyc_String_pa_struct _tmp79;_tmp79.tag=0;_tmp79.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp41);{struct Cyc_String_pa_struct _tmp78;
_tmp78.tag=0;_tmp78.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)({const char*
_tmp7B="type";_tag_dyneither(_tmp7B,sizeof(char),5);}));{void*_tmp76[3]={& _tmp78,&
_tmp79,& _tmp7A};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp77="%s %s : qualifier mismatch on field %s";
_tag_dyneither(_tmp77,sizeof(char),39);}),_tag_dyneither(_tmp76,sizeof(void*),3));}}}});
_tmp42=0;}if(((_tmp60 != 0  && _tmp67 != 0) && Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)_tmp60)!= Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)
_tmp67) || _tmp60 == 0  && _tmp67 != 0) || _tmp60 != 0  && _tmp67 == 0){({struct Cyc_String_pa_struct
_tmp80;_tmp80.tag=0;_tmp80.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp5D);{struct Cyc_String_pa_struct _tmp7F;_tmp7F.tag=0;_tmp7F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp41);{struct Cyc_String_pa_struct _tmp7E;
_tmp7E.tag=0;_tmp7E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)({const char*
_tmp81="type";_tag_dyneither(_tmp81,sizeof(char),5);}));{void*_tmp7C[3]={& _tmp7E,&
_tmp7F,& _tmp80};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp7D="%s %s : bitfield mismatch on field %s";
_tag_dyneither(_tmp7D,sizeof(char),38);}),_tag_dyneither(_tmp7C,sizeof(void*),3));}}}});
_tmp42=0;}{void*subst_t1=Cyc_Tcutil_substitute(_tmp5A,_tmp66);if(!Cyc_Tcdecl_check_type(
_tmp5F,subst_t1)){({struct Cyc_String_pa_struct _tmp87;_tmp87.tag=0;_tmp87.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_t1));{
struct Cyc_String_pa_struct _tmp86;_tmp86.tag=0;_tmp86.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp5F));{struct Cyc_String_pa_struct
_tmp85;_tmp85.tag=0;_tmp85.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp5D);{struct Cyc_String_pa_struct _tmp84;_tmp84.tag=0;_tmp84.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp41);{void*_tmp82[4]={& _tmp84,& _tmp85,&
_tmp86,& _tmp87};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp83="type %s : type mismatch on field %s: %s != %s";
_tag_dyneither(_tmp83,sizeof(char),46);}),_tag_dyneither(_tmp82,sizeof(void*),4));}}}}});
Cyc_Tcutil_explain_failure();_tmp42=0;}}}}if(_tmp51 != 0){({struct Cyc_String_pa_struct
_tmp8B;_tmp8B.tag=0;_tmp8B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Aggrfield*)_tmp51->hd)->name);{struct Cyc_String_pa_struct _tmp8A;
_tmp8A.tag=0;_tmp8A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp41);{
void*_tmp88[2]={& _tmp8A,& _tmp8B};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp89="type %s is missing field %s";
_tag_dyneither(_tmp89,sizeof(char),28);}),_tag_dyneither(_tmp88,sizeof(void*),2));}}});
_tmp42=0;}if(_tmp57 != 0){({struct Cyc_String_pa_struct _tmp8F;_tmp8F.tag=0;_tmp8F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp57->hd)->name);{
struct Cyc_String_pa_struct _tmp8E;_tmp8E.tag=0;_tmp8E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp41);{void*_tmp8C[2]={& _tmp8E,& _tmp8F};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp8D="type %s has extra field %s";_tag_dyneither(_tmp8D,
sizeof(char),27);}),_tag_dyneither(_tmp8C,sizeof(void*),2));}}});_tmp42=0;}if(
_tmp52 != _tmp58){({struct Cyc_String_pa_struct _tmp92;_tmp92.tag=0;_tmp92.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp41);{void*_tmp90[1]={& _tmp92};
Cyc_Tcdecl_merr(loc,msg,({const char*_tmp91="%s : previous declaration disagrees with @tagged qualifier";
_tag_dyneither(_tmp91,sizeof(char),59);}),_tag_dyneither(_tmp90,sizeof(void*),1));}});
_tmp42=0;}d2=d0;goto _LL1E;}_LL1E:;}if(!_tmp42)return 0;if(_tmp46 == (void*)d2->sc)
return(struct Cyc_Absyn_Aggrdecl*)d2;else{d2=({struct Cyc_Absyn_Aggrdecl*_tmp93=
_cycalloc(sizeof(*_tmp93));_tmp93[0]=*d2;_tmp93;});(void*)(d2->sc=(void*)_tmp46);
return(struct Cyc_Absyn_Aggrdecl*)d2;}}}}inline static struct _dyneither_ptr Cyc_Tcdecl_is_x2string(
int is_x){return is_x?({const char*_tmp94="@extensible datatype";_tag_dyneither(
_tmp94,sizeof(char),21);}):({const char*_tmp95="datatype";_tag_dyneither(_tmp95,
sizeof(char),9);});}struct _tuple7{struct Cyc_Absyn_Tqual f1;void*f2;};static struct
Cyc_Absyn_Datatypefield*Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*
f0,struct Cyc_Absyn_Datatypefield*f1,struct Cyc_List_List*inst,struct
_dyneither_ptr t,struct _dyneither_ptr v,struct _dyneither_ptr*msg){struct Cyc_Position_Segment*
loc=f1->loc;if(Cyc_strptrcmp((*f0->name).f2,(*f1->name).f2)!= 0){({struct Cyc_String_pa_struct
_tmp9B;_tmp9B.tag=0;_tmp9B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f0->name).f2);{
struct Cyc_String_pa_struct _tmp9A;_tmp9A.tag=0;_tmp9A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*f1->name).f2);{struct Cyc_String_pa_struct _tmp99;_tmp99.tag=
0;_tmp99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);{struct Cyc_String_pa_struct
_tmp98;_tmp98.tag=0;_tmp98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);{
void*_tmp96[4]={& _tmp98,& _tmp99,& _tmp9A,& _tmp9B};Cyc_Tcdecl_merr(loc,msg,({const
char*_tmp97="%s %s: field name mismatch %s != %s";_tag_dyneither(_tmp97,sizeof(
char),36);}),_tag_dyneither(_tmp96,sizeof(void*),4));}}}}});return 0;}{struct
_dyneither_ptr _tmp9C=*(*f0->name).f2;void*_tmpA2;int _tmpA3;struct _tuple3 _tmpA1=
Cyc_Tcdecl_merge_scope((void*)f0->sc,(void*)f1->sc,(struct _dyneither_ptr)({
struct Cyc_String_pa_struct _tmpA0;_tmpA0.tag=0;_tmpA0.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)v);{struct Cyc_String_pa_struct _tmp9F;_tmp9F.tag=0;_tmp9F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)t);{void*_tmp9D[2]={& _tmp9F,& _tmpA0};
Cyc_aprintf(({const char*_tmp9E="in %s %s, field";_tag_dyneither(_tmp9E,sizeof(
char),16);}),_tag_dyneither(_tmp9D,sizeof(void*),2));}}}),_tmp9C,loc,msg,0);
_tmpA2=_tmpA1.f1;_tmpA3=_tmpA1.f2;{struct Cyc_List_List*_tmpA4=f0->typs;struct Cyc_List_List*
_tmpA5=f1->typs;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA4)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA5)){({struct Cyc_String_pa_struct
_tmpAA;_tmpAA.tag=0;_tmpAA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp9C);{struct Cyc_String_pa_struct _tmpA9;_tmpA9.tag=0;_tmpA9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)v);{struct Cyc_String_pa_struct _tmpA8;
_tmpA8.tag=0;_tmpA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);{void*
_tmpA6[3]={& _tmpA8,& _tmpA9,& _tmpAA};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpA7="%s %s, field %s: parameter number mismatch";
_tag_dyneither(_tmpA7,sizeof(char),43);}),_tag_dyneither(_tmpA6,sizeof(void*),3));}}}});
_tmpA3=0;}for(0;_tmpA4 != 0;(_tmpA4=_tmpA4->tl,_tmpA5=_tmpA5->tl)){if(!Cyc_Tcutil_equal_tqual((*((
struct _tuple7*)_tmpA4->hd)).f1,(*((struct _tuple7*)((struct Cyc_List_List*)
_check_null(_tmpA5))->hd)).f1)){({struct Cyc_String_pa_struct _tmpAF;_tmpAF.tag=0;
_tmpAF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9C);{struct Cyc_String_pa_struct
_tmpAE;_tmpAE.tag=0;_tmpAE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);{
struct Cyc_String_pa_struct _tmpAD;_tmpAD.tag=0;_tmpAD.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);{void*_tmpAB[3]={& _tmpAD,& _tmpAE,& _tmpAF};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmpAC="%s %s, field %s: parameter qualifier";_tag_dyneither(
_tmpAC,sizeof(char),37);}),_tag_dyneither(_tmpAB,sizeof(void*),3));}}}});_tmpA3=
0;}{void*subst_t1=Cyc_Tcutil_substitute(inst,(*((struct _tuple7*)_tmpA5->hd)).f2);
if(!Cyc_Tcdecl_check_type((*((struct _tuple7*)_tmpA4->hd)).f2,subst_t1)){({struct
Cyc_String_pa_struct _tmpB6;_tmpB6.tag=0;_tmpB6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(subst_t1));{struct Cyc_String_pa_struct
_tmpB5;_tmpB5.tag=0;_tmpB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((*((
struct _tuple7*)_tmpA4->hd)).f2));{struct Cyc_String_pa_struct _tmpB4;_tmpB4.tag=0;
_tmpB4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9C);{struct Cyc_String_pa_struct
_tmpB3;_tmpB3.tag=0;_tmpB3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);{
struct Cyc_String_pa_struct _tmpB2;_tmpB2.tag=0;_tmpB2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);{void*_tmpB0[5]={& _tmpB2,& _tmpB3,& _tmpB4,& _tmpB5,& _tmpB6};
Cyc_Tcdecl_merr(loc,msg,({const char*_tmpB1="%s %s, field %s: parameter type mismatch %s != %s";
_tag_dyneither(_tmpB1,sizeof(char),50);}),_tag_dyneither(_tmpB0,sizeof(void*),5));}}}}}});
Cyc_Tcutil_explain_failure();_tmpA3=0;}}}if(!_tmpA3)return 0;if((void*)f0->sc != 
_tmpA2){struct Cyc_Absyn_Datatypefield*_tmpB7=({struct Cyc_Absyn_Datatypefield*
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=*f0;_tmpB8;});(void*)(((struct Cyc_Absyn_Datatypefield*)
_check_null(_tmpB7))->sc=(void*)_tmpA2);return _tmpB7;}else{return(struct Cyc_Absyn_Datatypefield*)
f0;}}}}static struct _tuple7*Cyc_Tcdecl_substitute_datatypefield_f2(struct Cyc_List_List*
inst,struct _tuple7*x){struct _tuple7 _tmpBA;struct Cyc_Absyn_Tqual _tmpBB;void*
_tmpBC;struct _tuple7*_tmpB9=x;_tmpBA=*_tmpB9;_tmpBB=_tmpBA.f1;_tmpBC=_tmpBA.f2;
return({struct _tuple7*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->f1=_tmpBB;_tmpBD->f2=
Cyc_Tcutil_substitute(inst,_tmpBC);_tmpBD;});}static struct Cyc_Absyn_Datatypefield*
Cyc_Tcdecl_substitute_datatypefield(struct Cyc_List_List*inst1,struct Cyc_Absyn_Datatypefield*
f1){struct Cyc_Absyn_Datatypefield*_tmpBE=({struct Cyc_Absyn_Datatypefield*_tmpBF=
_cycalloc(sizeof(*_tmpBF));_tmpBF[0]=*f1;_tmpBF;});_tmpBE->typs=((struct Cyc_List_List*(*)(
struct _tuple7*(*f)(struct Cyc_List_List*,struct _tuple7*),struct Cyc_List_List*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcdecl_substitute_datatypefield_f2,
inst1,f1->typs);return _tmpBE;}static struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(
struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct
Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct
_dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){struct Cyc_List_List**f2sp=({struct Cyc_List_List**_tmpC8=
_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=0;_tmpC8;});struct Cyc_List_List**_tmpC0=
f2sp;int cmp=- 1;for(0;f0s != 0  && f1s != 0;f1s=f1s->tl){while(f0s != 0  && (cmp=Cyc_Absyn_qvar_cmp(((
struct Cyc_Absyn_Datatypefield*)f0s->hd)->name,((struct Cyc_Absyn_Datatypefield*)
f1s->hd)->name))< 0){struct Cyc_List_List*_tmpC1=({struct Cyc_List_List*_tmpC2=
_cycalloc(sizeof(*_tmpC2));_tmpC2->hd=(struct Cyc_Absyn_Datatypefield*)f0s->hd;
_tmpC2->tl=0;_tmpC2;});*_tmpC0=_tmpC1;_tmpC0=&((struct Cyc_List_List*)_check_null(
_tmpC1))->tl;f0s=f0s->tl;}if(f0s == 0  || cmp > 0){*incl=0;{struct Cyc_List_List*
_tmpC3=({struct Cyc_List_List*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->hd=Cyc_Tcdecl_substitute_datatypefield(
inst,(struct Cyc_Absyn_Datatypefield*)f1s->hd);_tmpC4->tl=0;_tmpC4;});*_tmpC0=
_tmpC3;_tmpC0=&((struct Cyc_List_List*)_check_null(_tmpC3))->tl;}}else{struct Cyc_Absyn_Datatypefield*
_tmpC5=Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)f0s->hd,(
struct Cyc_Absyn_Datatypefield*)f1s->hd,inst,t,v,msg);if(_tmpC5 != 0){if(_tmpC5 != (
struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)f0s->hd))*incl=0;{
struct Cyc_List_List*_tmpC6=({struct Cyc_List_List*_tmpC7=_cycalloc(sizeof(*_tmpC7));
_tmpC7->hd=(struct Cyc_Absyn_Datatypefield*)_tmpC5;_tmpC7->tl=0;_tmpC7;});*_tmpC0=
_tmpC6;_tmpC0=&((struct Cyc_List_List*)_check_null(_tmpC6))->tl;}}else{*res=0;}
f0s=f0s->tl;}}if(f1s != 0){*incl=0;*_tmpC0=f1s;}else{*_tmpC0=f0s;}return*f2sp;}
struct _tuple8{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};static struct
_tuple8 Cyc_Tcdecl_split(struct Cyc_List_List*f){if(f == 0)return({struct _tuple8
_tmpC9;_tmpC9.f1=0;_tmpC9.f2=0;_tmpC9;});if(f->tl == 0)return({struct _tuple8
_tmpCA;_tmpCA.f1=f;_tmpCA.f2=0;_tmpCA;});{struct Cyc_List_List*_tmpCC;struct Cyc_List_List*
_tmpCD;struct _tuple8 _tmpCB=Cyc_Tcdecl_split(((struct Cyc_List_List*)_check_null(f->tl))->tl);
_tmpCC=_tmpCB.f1;_tmpCD=_tmpCB.f2;return({struct _tuple8 _tmpCE;_tmpCE.f1=({struct
Cyc_List_List*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->hd=(void*)((void*)f->hd);
_tmpD0->tl=_tmpCC;_tmpD0;});_tmpCE.f2=({struct Cyc_List_List*_tmpCF=_cycalloc(
sizeof(*_tmpCF));_tmpCF->hd=(void*)((void*)((struct Cyc_List_List*)_check_null(f->tl))->hd);
_tmpCF->tl=_tmpCD;_tmpCF;});_tmpCE;});}}struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(
struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg){struct Cyc_List_List*_tmpD2;struct Cyc_List_List*
_tmpD3;struct _tuple8 _tmpD1=((struct _tuple8(*)(struct Cyc_List_List*f))Cyc_Tcdecl_split)(
f);_tmpD2=_tmpD1.f1;_tmpD3=_tmpD1.f2;if(_tmpD2 != 0  && _tmpD2->tl != 0)_tmpD2=Cyc_Tcdecl_sort_xdatatype_fields(
_tmpD2,res,v,loc,msg);if(_tmpD3 != 0  && _tmpD3->tl != 0)_tmpD3=Cyc_Tcdecl_sort_xdatatype_fields(
_tmpD3,res,v,loc,msg);return Cyc_Tcdecl_merge_xdatatype_fields(_tmpD2,_tmpD3,0,0,
0,res,({int*_tmpD4=_cycalloc_atomic(sizeof(*_tmpD4));_tmpD4[0]=1;_tmpD4;}),({
const char*_tmpD5="@extensible datatype";_tag_dyneither(_tmpD5,sizeof(char),21);}),*
v,loc,msg);}struct _tuple9{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};struct
Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*
d0,struct Cyc_Absyn_Datatypedecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){struct _dyneither_ptr _tmpD6=Cyc_Absynpp_qvar2string(d0->name);
struct _dyneither_ptr t=({const char*_tmpFE="datatype";_tag_dyneither(_tmpFE,
sizeof(char),9);});int _tmpD7=1;if(d0->is_extensible != d1->is_extensible){({
struct Cyc_String_pa_struct _tmpDC;_tmpDC.tag=0;_tmpDC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(d1->is_extensible));{struct Cyc_String_pa_struct
_tmpDB;_tmpDB.tag=0;_tmpDB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(
d0->is_extensible));{struct Cyc_String_pa_struct _tmpDA;_tmpDA.tag=0;_tmpDA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD6);{void*_tmpD8[3]={& _tmpDA,&
_tmpDB,& _tmpDC};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpD9="expected %s to be a %s instead of a %s";
_tag_dyneither(_tmpD9,sizeof(char),39);}),_tag_dyneither(_tmpD8,sizeof(void*),3));}}}});
_tmpD7=0;}else{t=Cyc_Tcdecl_is_x2string(d0->is_extensible);}if(d0->is_flat != d1->is_flat){
if(d0->is_flat)({void*_tmpDD=0;Cyc_Tcdecl_merr(loc,msg,({const char*_tmpDE="expected __attribute__((flat))";
_tag_dyneither(_tmpDE,sizeof(char),31);}),_tag_dyneither(_tmpDD,sizeof(void*),0));});
else{({void*_tmpDF=0;Cyc_Tcdecl_merr(loc,msg,({const char*_tmpE0="did not expect __attribute__((flat))";
_tag_dyneither(_tmpE0,sizeof(char),37);}),_tag_dyneither(_tmpDF,sizeof(void*),0));});}}
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,t,_tmpD6,loc,msg))return 0;{void*_tmpE2;
int _tmpE3;struct _tuple3 _tmpE1=Cyc_Tcdecl_merge_scope((void*)d0->sc,(void*)d1->sc,
t,_tmpD6,loc,msg,0);_tmpE2=_tmpE1.f1;_tmpE3=_tmpE1.f2;if(!_tmpE3)_tmpD7=0;{
struct Cyc_Absyn_Datatypedecl*d2;{struct _tuple9 _tmpE5=({struct _tuple9 _tmpE4;
_tmpE4.f1=d0->fields;_tmpE4.f2=d1->fields;_tmpE4;});struct Cyc_Core_Opt*_tmpE6;
struct Cyc_Core_Opt*_tmpE7;struct Cyc_Core_Opt*_tmpE8;struct Cyc_Core_Opt _tmpE9;
struct Cyc_List_List*_tmpEA;struct Cyc_Core_Opt*_tmpEB;struct Cyc_Core_Opt _tmpEC;
struct Cyc_List_List*_tmpED;_LL26: _tmpE6=_tmpE5.f2;if(_tmpE6 != 0)goto _LL28;_LL27:
d2=d0;goto _LL25;_LL28: _tmpE7=_tmpE5.f1;if(_tmpE7 != 0)goto _LL2A;_LL29: d2=d1;goto
_LL25;_LL2A: _tmpE8=_tmpE5.f1;if(_tmpE8 == 0)goto _LL25;_tmpE9=*_tmpE8;_tmpEA=(
struct Cyc_List_List*)_tmpE9.v;_tmpEB=_tmpE5.f2;if(_tmpEB == 0)goto _LL25;_tmpEC=*
_tmpEB;_tmpED=(struct Cyc_List_List*)_tmpEC.v;_LL2B: {struct Cyc_List_List*_tmpEE=
Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);if(d0->is_extensible){int _tmpEF=1;
struct Cyc_List_List*_tmpF0=Cyc_Tcdecl_merge_xdatatype_fields(_tmpEA,_tmpED,
_tmpEE,d0->tvs,d1->tvs,& _tmpD7,& _tmpEF,t,_tmpD6,loc,msg);if(_tmpEF)d2=d0;else{d2=({
struct Cyc_Absyn_Datatypedecl*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1[0]=*d0;
_tmpF1;});(void*)(d2->sc=(void*)_tmpE2);d2->fields=({struct Cyc_Core_Opt*_tmpF2=
_cycalloc(sizeof(*_tmpF2));_tmpF2->v=_tmpF0;_tmpF2;});}}else{for(0;_tmpEA != 0
 && _tmpED != 0;(_tmpEA=_tmpEA->tl,_tmpED=_tmpED->tl)){Cyc_Tcdecl_merge_datatypefield((
struct Cyc_Absyn_Datatypefield*)_tmpEA->hd,(struct Cyc_Absyn_Datatypefield*)_tmpED->hd,
_tmpEE,t,_tmpD6,msg);}if(_tmpEA != 0){({struct Cyc_String_pa_struct _tmpF7;_tmpF7.tag=
0;_tmpF7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)((
struct Cyc_List_List*)_check_null(_tmpED))->hd)->name).f2);{struct Cyc_String_pa_struct
_tmpF6;_tmpF6.tag=0;_tmpF6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmpD6);{struct Cyc_String_pa_struct _tmpF5;_tmpF5.tag=0;_tmpF5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)t);{void*_tmpF3[3]={& _tmpF5,& _tmpF6,&
_tmpF7};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpF4="%s %s has extra field %s";
_tag_dyneither(_tmpF4,sizeof(char),25);}),_tag_dyneither(_tmpF3,sizeof(void*),3));}}}});
_tmpD7=0;}if(_tmpED != 0){({struct Cyc_String_pa_struct _tmpFC;_tmpFC.tag=0;_tmpFC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)
_tmpED->hd)->name).f2);{struct Cyc_String_pa_struct _tmpFB;_tmpFB.tag=0;_tmpFB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD6);{struct Cyc_String_pa_struct
_tmpFA;_tmpFA.tag=0;_tmpFA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);{
void*_tmpF8[3]={& _tmpFA,& _tmpFB,& _tmpFC};Cyc_Tcdecl_merr(loc,msg,({const char*
_tmpF9="%s %s is missing field %s";_tag_dyneither(_tmpF9,sizeof(char),26);}),
_tag_dyneither(_tmpF8,sizeof(void*),3));}}}});_tmpD7=0;}d2=d0;}goto _LL25;}_LL25:;}
if(!_tmpD7)return 0;if(_tmpE2 == (void*)d2->sc)return(struct Cyc_Absyn_Datatypedecl*)
d2;else{d2=({struct Cyc_Absyn_Datatypedecl*_tmpFD=_cycalloc(sizeof(*_tmpFD));
_tmpFD[0]=*d2;_tmpFD;});(void*)(d2->sc=(void*)_tmpE2);return(struct Cyc_Absyn_Datatypedecl*)
d2;}}}}struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*
d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){struct _dyneither_ptr _tmpFF=Cyc_Absynpp_qvar2string(d0->name);
int _tmp100=1;void*_tmp103;int _tmp104;struct _tuple3 _tmp102=Cyc_Tcdecl_merge_scope((
void*)d0->sc,(void*)d1->sc,({const char*_tmp101="enum";_tag_dyneither(_tmp101,
sizeof(char),5);}),_tmpFF,loc,msg,1);_tmp103=_tmp102.f1;_tmp104=_tmp102.f2;if(!
_tmp104)_tmp100=0;{struct Cyc_Absyn_Enumdecl*d2;{struct _tuple9 _tmp106=({struct
_tuple9 _tmp105;_tmp105.f1=d0->fields;_tmp105.f2=d1->fields;_tmp105;});struct Cyc_Core_Opt*
_tmp107;struct Cyc_Core_Opt*_tmp108;struct Cyc_Core_Opt*_tmp109;struct Cyc_Core_Opt
_tmp10A;struct Cyc_List_List*_tmp10B;struct Cyc_Core_Opt*_tmp10C;struct Cyc_Core_Opt
_tmp10D;struct Cyc_List_List*_tmp10E;_LL2D: _tmp107=_tmp106.f2;if(_tmp107 != 0)goto
_LL2F;_LL2E: d2=d0;goto _LL2C;_LL2F: _tmp108=_tmp106.f1;if(_tmp108 != 0)goto _LL31;
_LL30: d2=d1;goto _LL2C;_LL31: _tmp109=_tmp106.f1;if(_tmp109 == 0)goto _LL2C;_tmp10A=*
_tmp109;_tmp10B=(struct Cyc_List_List*)_tmp10A.v;_tmp10C=_tmp106.f2;if(_tmp10C == 
0)goto _LL2C;_tmp10D=*_tmp10C;_tmp10E=(struct Cyc_List_List*)_tmp10D.v;_LL32: for(0;
_tmp10B != 0  && _tmp10E != 0;(_tmp10B=_tmp10B->tl,_tmp10E=_tmp10E->tl)){struct Cyc_Absyn_Enumfield
_tmp110;struct _tuple0*_tmp111;struct _tuple0 _tmp112;struct _dyneither_ptr*_tmp113;
struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Position_Segment*_tmp115;struct Cyc_Absyn_Enumfield*
_tmp10F=(struct Cyc_Absyn_Enumfield*)_tmp10B->hd;_tmp110=*_tmp10F;_tmp111=_tmp110.name;
_tmp112=*_tmp111;_tmp113=_tmp112.f2;_tmp114=_tmp110.tag;_tmp115=_tmp110.loc;{
struct Cyc_Absyn_Enumfield _tmp117;struct _tuple0*_tmp118;struct _tuple0 _tmp119;
struct _dyneither_ptr*_tmp11A;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Position_Segment*
_tmp11C;struct Cyc_Absyn_Enumfield*_tmp116=(struct Cyc_Absyn_Enumfield*)_tmp10E->hd;
_tmp117=*_tmp116;_tmp118=_tmp117.name;_tmp119=*_tmp118;_tmp11A=_tmp119.f2;
_tmp11B=_tmp117.tag;_tmp11C=_tmp117.loc;if(Cyc_strptrcmp(_tmp113,_tmp11A)!= 0){({
struct Cyc_String_pa_struct _tmp121;_tmp121.tag=0;_tmp121.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp11A);{struct Cyc_String_pa_struct _tmp120;_tmp120.tag=0;
_tmp120.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp113);{struct Cyc_String_pa_struct
_tmp11F;_tmp11F.tag=0;_tmp11F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmpFF);{void*_tmp11D[3]={& _tmp11F,& _tmp120,& _tmp121};Cyc_Tcdecl_merr(loc,msg,({
const char*_tmp11E="enum %s: field name mismatch %s != %s";_tag_dyneither(_tmp11E,
sizeof(char),38);}),_tag_dyneither(_tmp11D,sizeof(void*),3));}}}});_tmp100=0;}
if(Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmp114))!= 
Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmp11B))){({
struct Cyc_String_pa_struct _tmp125;_tmp125.tag=0;_tmp125.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp11A);{struct Cyc_String_pa_struct _tmp124;_tmp124.tag=0;
_tmp124.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFF);{void*_tmp122[2]={&
_tmp124,& _tmp125};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp123="enum %s, field %s, value mismatch";
_tag_dyneither(_tmp123,sizeof(char),34);}),_tag_dyneither(_tmp122,sizeof(void*),
2));}}});_tmp100=0;}}}d2=d0;goto _LL2C;_LL2C:;}if(!_tmp100)return 0;if((void*)d2->sc
== _tmp103)return(struct Cyc_Absyn_Enumdecl*)d2;else{d2=({struct Cyc_Absyn_Enumdecl*
_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126[0]=*d2;_tmp126;});(void*)(d2->sc=(
void*)_tmp103);return(struct Cyc_Absyn_Enumdecl*)d2;}}}static struct _tuple3 Cyc_Tcdecl_check_var_or_fn_decl(
void*sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*atts0,void*sc1,
void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*atts1,struct _dyneither_ptr t,
struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){
int _tmp127=1;void*_tmp129;int _tmp12A;struct _tuple3 _tmp128=Cyc_Tcdecl_merge_scope(
sc0,sc1,t,v,loc,msg,0);_tmp129=_tmp128.f1;_tmp12A=_tmp128.f2;if(!_tmp12A)_tmp127=
0;if(!Cyc_Tcdecl_check_type(t0,t1)){({struct Cyc_String_pa_struct _tmp130;_tmp130.tag=
0;_tmp130.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t0));{struct Cyc_String_pa_struct _tmp12F;_tmp12F.tag=0;_tmp12F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{struct Cyc_String_pa_struct
_tmp12E;_tmp12E.tag=0;_tmp12E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);{
struct Cyc_String_pa_struct _tmp12D;_tmp12D.tag=0;_tmp12D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);{void*_tmp12B[4]={& _tmp12D,& _tmp12E,& _tmp12F,& _tmp130};
Cyc_Tcdecl_merr(loc,msg,({const char*_tmp12C="%s %s has type \n%s\n instead of \n%s\n";
_tag_dyneither(_tmp12C,sizeof(char),36);}),_tag_dyneither(_tmp12B,sizeof(void*),
4));}}}}});Cyc_Tcutil_explain_failure();_tmp127=0;}if(!Cyc_Tcutil_equal_tqual(
tq0,tq1)){({struct Cyc_String_pa_struct _tmp134;_tmp134.tag=0;_tmp134.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)v);{struct Cyc_String_pa_struct _tmp133;
_tmp133.tag=0;_tmp133.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);{void*
_tmp131[2]={& _tmp133,& _tmp134};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp132="%s %s has different type qualifiers";
_tag_dyneither(_tmp132,sizeof(char),36);}),_tag_dyneither(_tmp131,sizeof(void*),
2));}}});_tmp127=0;}if(!Cyc_Tcutil_same_atts(atts0,atts1)){({struct Cyc_String_pa_struct
_tmp138;_tmp138.tag=0;_tmp138.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);{
struct Cyc_String_pa_struct _tmp137;_tmp137.tag=0;_tmp137.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);{void*_tmp135[2]={& _tmp137,& _tmp138};Cyc_Tcdecl_merr(loc,
msg,({const char*_tmp136="%s %s has different attributes";_tag_dyneither(_tmp136,
sizeof(char),31);}),_tag_dyneither(_tmp135,sizeof(void*),2));}}});({void*_tmp139=
0;Cyc_fprintf(Cyc_stderr,({const char*_tmp13A="previous attributes: ";
_tag_dyneither(_tmp13A,sizeof(char),22);}),_tag_dyneither(_tmp139,sizeof(void*),
0));});for(0;atts0 != 0;atts0=atts0->tl){({struct Cyc_String_pa_struct _tmp13D;
_tmp13D.tag=0;_tmp13D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)atts0->hd));{void*_tmp13B[1]={& _tmp13D};Cyc_fprintf(Cyc_stderr,({const char*
_tmp13C="%s ";_tag_dyneither(_tmp13C,sizeof(char),4);}),_tag_dyneither(_tmp13B,
sizeof(void*),1));}});}({void*_tmp13E=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp13F="\ncurrent attributes: ";_tag_dyneither(_tmp13F,sizeof(char),22);}),
_tag_dyneither(_tmp13E,sizeof(void*),0));});for(0;atts1 != 0;atts1=atts1->tl){({
struct Cyc_String_pa_struct _tmp142;_tmp142.tag=0;_tmp142.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts1->hd));{void*_tmp140[
1]={& _tmp142};Cyc_fprintf(Cyc_stderr,({const char*_tmp141="%s ";_tag_dyneither(
_tmp141,sizeof(char),4);}),_tag_dyneither(_tmp140,sizeof(void*),1));}});}({void*
_tmp143=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp144="\n";_tag_dyneither(_tmp144,
sizeof(char),2);}),_tag_dyneither(_tmp143,sizeof(void*),0));});_tmp127=0;}return({
struct _tuple3 _tmp145;_tmp145.f1=_tmp129;_tmp145.f2=_tmp127;_tmp145;});}struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){struct _dyneither_ptr
_tmp146=Cyc_Absynpp_qvar2string(d0->name);void*_tmp149;int _tmp14A;struct _tuple3
_tmp148=Cyc_Tcdecl_check_var_or_fn_decl((void*)d0->sc,(void*)d0->type,d0->tq,d0->attributes,(
void*)d1->sc,(void*)d1->type,d1->tq,d1->attributes,({const char*_tmp147="variable";
_tag_dyneither(_tmp147,sizeof(char),9);}),_tmp146,loc,msg);_tmp149=_tmp148.f1;
_tmp14A=_tmp148.f2;if(!_tmp14A)return 0;if((void*)d0->sc == _tmp149)return(struct
Cyc_Absyn_Vardecl*)d0;else{struct Cyc_Absyn_Vardecl*_tmp14B=({struct Cyc_Absyn_Vardecl*
_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C[0]=*d0;_tmp14C;});(void*)(((struct
Cyc_Absyn_Vardecl*)_check_null(_tmp14B))->sc=(void*)_tmp149);return _tmp14B;}}
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*
d0,struct Cyc_Absyn_Typedefdecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){struct _dyneither_ptr _tmp14D=Cyc_Absynpp_qvar2string(d0->name);
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,({const char*_tmp14E="typedef";
_tag_dyneither(_tmp14E,sizeof(char),8);}),_tmp14D,loc,msg))return 0;{struct Cyc_List_List*
_tmp14F=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);if(d0->defn != 0  && d1->defn != 
0){void*subst_defn1=Cyc_Tcutil_substitute(_tmp14F,(void*)((struct Cyc_Core_Opt*)
_check_null(d1->defn))->v);if(!Cyc_Tcdecl_check_type((void*)((struct Cyc_Core_Opt*)
_check_null(d0->defn))->v,subst_defn1)){({struct Cyc_String_pa_struct _tmp154;
_tmp154.tag=0;_tmp154.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(d0->defn))->v));{struct Cyc_String_pa_struct
_tmp153;_tmp153.tag=0;_tmp153.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(subst_defn1));{struct Cyc_String_pa_struct _tmp152;_tmp152.tag=
0;_tmp152.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp14D);{void*_tmp150[
3]={& _tmp152,& _tmp153,& _tmp154};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp151="typedef %s does not refer to the same type: %s != %s";
_tag_dyneither(_tmp151,sizeof(char),53);}),_tag_dyneither(_tmp150,sizeof(void*),
3));}}}});return 0;}}return(struct Cyc_Absyn_Typedefdecl*)d0;}}void*Cyc_Tcdecl_merge_binding(
void*b0,void*b1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){struct
_tuple4 _tmp156=({struct _tuple4 _tmp155;_tmp155.f1=b0;_tmp155.f2=b1;_tmp155;});
void*_tmp157;void*_tmp158;void*_tmp159;struct Cyc_Absyn_Vardecl*_tmp15A;void*
_tmp15B;struct Cyc_Absyn_Vardecl*_tmp15C;void*_tmp15D;struct Cyc_Absyn_Vardecl*
_tmp15E;void*_tmp15F;struct Cyc_Absyn_Fndecl*_tmp160;void*_tmp161;void*_tmp162;
struct Cyc_Absyn_Fndecl*_tmp163;void*_tmp164;struct Cyc_Absyn_Fndecl*_tmp165;void*
_tmp166;struct Cyc_Absyn_Vardecl*_tmp167;_LL34: _tmp157=_tmp156.f1;if((int)_tmp157
!= 0)goto _LL36;_tmp158=_tmp156.f2;if((int)_tmp158 != 0)goto _LL36;_LL35: return(
void*)0;_LL36: _tmp159=_tmp156.f1;if(_tmp159 <= (void*)1)goto _LL38;if(*((int*)
_tmp159)!= 0)goto _LL38;_tmp15A=((struct Cyc_Absyn_Global_b_struct*)_tmp159)->f1;
_tmp15B=_tmp156.f2;if(_tmp15B <= (void*)1)goto _LL38;if(*((int*)_tmp15B)!= 0)goto
_LL38;_tmp15C=((struct Cyc_Absyn_Global_b_struct*)_tmp15B)->f1;_LL37: {struct Cyc_Absyn_Vardecl*
_tmp168=Cyc_Tcdecl_merge_vardecl(_tmp15A,_tmp15C,loc,msg);if(_tmp168 == 0)return(
void*)0;if(_tmp168 == (struct Cyc_Absyn_Vardecl*)_tmp15A)return b0;if(_tmp168 == (
struct Cyc_Absyn_Vardecl*)_tmp15C)return b1;return(void*)({struct Cyc_Absyn_Global_b_struct*
_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169[0]=({struct Cyc_Absyn_Global_b_struct
_tmp16A;_tmp16A.tag=0;_tmp16A.f1=(struct Cyc_Absyn_Vardecl*)_tmp168;_tmp16A;});
_tmp169;});}_LL38: _tmp15D=_tmp156.f1;if(_tmp15D <= (void*)1)goto _LL3A;if(*((int*)
_tmp15D)!= 0)goto _LL3A;_tmp15E=((struct Cyc_Absyn_Global_b_struct*)_tmp15D)->f1;
_tmp15F=_tmp156.f2;if(_tmp15F <= (void*)1)goto _LL3A;if(*((int*)_tmp15F)!= 1)goto
_LL3A;_tmp160=((struct Cyc_Absyn_Funname_b_struct*)_tmp15F)->f1;_LL39: {int
_tmp16D;struct _tuple3 _tmp16C=Cyc_Tcdecl_check_var_or_fn_decl((void*)_tmp15E->sc,(
void*)_tmp15E->type,_tmp15E->tq,_tmp15E->attributes,(void*)_tmp160->sc,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp160->cached_typ))->v,Cyc_Absyn_empty_tqual(0),
_tmp160->attributes,({const char*_tmp16B="function";_tag_dyneither(_tmp16B,
sizeof(char),9);}),Cyc_Absynpp_qvar2string(_tmp15E->name),loc,msg);_tmp16D=
_tmp16C.f2;return _tmp16D?b1:(void*)0;}_LL3A: _tmp161=_tmp156.f1;if(_tmp161 <= (
void*)1)goto _LL3C;if(*((int*)_tmp161)!= 1)goto _LL3C;_tmp162=_tmp156.f2;if(
_tmp162 <= (void*)1)goto _LL3C;if(*((int*)_tmp162)!= 1)goto _LL3C;_tmp163=((struct
Cyc_Absyn_Funname_b_struct*)_tmp162)->f1;_LL3B:({struct Cyc_String_pa_struct
_tmp170;_tmp170.tag=0;_tmp170.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp163->name));{void*_tmp16E[1]={& _tmp170};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp16F="redefinition of function %s";_tag_dyneither(_tmp16F,
sizeof(char),28);}),_tag_dyneither(_tmp16E,sizeof(void*),1));}});return(void*)0;
_LL3C: _tmp164=_tmp156.f1;if(_tmp164 <= (void*)1)goto _LL3E;if(*((int*)_tmp164)!= 1)
goto _LL3E;_tmp165=((struct Cyc_Absyn_Funname_b_struct*)_tmp164)->f1;_tmp166=
_tmp156.f2;if(_tmp166 <= (void*)1)goto _LL3E;if(*((int*)_tmp166)!= 0)goto _LL3E;
_tmp167=((struct Cyc_Absyn_Global_b_struct*)_tmp166)->f1;_LL3D: {int _tmp173;
struct _tuple3 _tmp172=Cyc_Tcdecl_check_var_or_fn_decl((void*)_tmp165->sc,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp165->cached_typ))->v,Cyc_Absyn_empty_tqual(0),
_tmp165->attributes,(void*)_tmp167->sc,(void*)_tmp167->type,_tmp167->tq,_tmp167->attributes,({
const char*_tmp171="variable";_tag_dyneither(_tmp171,sizeof(char),9);}),Cyc_Absynpp_qvar2string(
_tmp165->name),loc,msg);_tmp173=_tmp172.f2;return _tmp173?b0:(void*)0;}_LL3E:;
_LL3F:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp174=
_cycalloc(sizeof(*_tmp174));_tmp174[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp175;_tmp175.tag=Cyc_Core_Invalid_argument;_tmp175.f1=({const char*_tmp176="Tcdecl::merge_binding";
_tag_dyneither(_tmp176,sizeof(char),22);});_tmp175;});_tmp174;}));_LL33:;}struct
Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*
d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){struct Cyc_Tcdecl_Xdatatypefielddecl _tmp178;struct Cyc_Absyn_Datatypedecl*
_tmp179;struct Cyc_Absyn_Datatypefield*_tmp17A;struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp177=d0;_tmp178=*_tmp177;_tmp179=_tmp178.base;_tmp17A=_tmp178.field;{struct
Cyc_Tcdecl_Xdatatypefielddecl _tmp17C;struct Cyc_Absyn_Datatypedecl*_tmp17D;struct
Cyc_Absyn_Datatypefield*_tmp17E;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp17B=d1;
_tmp17C=*_tmp17B;_tmp17D=_tmp17C.base;_tmp17E=_tmp17C.field;{struct
_dyneither_ptr _tmp17F=Cyc_Absynpp_qvar2string(_tmp17A->name);if(Cyc_Absyn_qvar_cmp(
_tmp179->name,_tmp17D->name)!= 0){({struct Cyc_String_pa_struct _tmp184;_tmp184.tag=
0;_tmp184.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp17F);{struct Cyc_String_pa_struct
_tmp183;_tmp183.tag=0;_tmp183.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp17D->name));{struct Cyc_String_pa_struct _tmp182;
_tmp182.tag=0;_tmp182.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp179->name));{void*_tmp180[3]={& _tmp182,& _tmp183,& _tmp184};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp181="@extensible datatype %s and %s have both a field named %s";
_tag_dyneither(_tmp181,sizeof(char),58);}),_tag_dyneither(_tmp180,sizeof(void*),
3));}}}});return 0;}if(!Cyc_Tcdecl_check_tvs(_tmp179->tvs,_tmp17D->tvs,({const
char*_tmp185="@extensible datatype";_tag_dyneither(_tmp185,sizeof(char),21);}),
Cyc_Absynpp_qvar2string(_tmp179->name),loc,msg))return 0;{struct Cyc_List_List*
_tmp186=Cyc_Tcdecl_build_tvs_map(_tmp179->tvs,_tmp17D->tvs);struct Cyc_Absyn_Datatypefield*
_tmp187=Cyc_Tcdecl_merge_datatypefield(_tmp17A,_tmp17E,_tmp186,({const char*
_tmp189="@extensible datatype field";_tag_dyneither(_tmp189,sizeof(char),27);}),
_tmp17F,msg);if(_tmp187 == 0)return 0;if(_tmp187 == (struct Cyc_Absyn_Datatypefield*)
_tmp17A)return(struct Cyc_Tcdecl_Xdatatypefielddecl*)d0;return({struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188->base=_tmp179;_tmp188->field=(struct
Cyc_Absyn_Datatypefield*)_tmp187;_tmp188;});}}}}
