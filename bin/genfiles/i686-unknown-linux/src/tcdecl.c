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
struct Cyc_Position_Error*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
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
struct _tuple0*,struct _tuple0*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);void*Cyc_Absyn_force_kb(void*kb);struct _dyneither_ptr Cyc_Absyn_attribute2string(
void*);extern char Cyc_Tcdecl_Incompatible[17];struct Cyc_Tcdecl_Xdatatypefielddecl{
struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};void Cyc_Tcdecl_merr(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);struct _tuple5 Cyc_Tcdecl_merge_scope(void*s0,void*s1,
struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr*msg,int allow_externC_extern_merge);struct Cyc_Absyn_Aggrdecl*
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
12];extern char Cyc_Dict_Absent[11];struct _tuple9{void*f1;void*f2;};struct _tuple9*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple9*Cyc_Dict_rchoose(
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
struct _tuple5 Cyc_Tcdecl_merge_scope(void*s0,void*s1,struct _dyneither_ptr t,struct
_dyneither_ptr v,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg,int
externCmerge){{struct _tuple9 _tmpE=({struct _tuple9 _tmpD;_tmpD.f1=s0;_tmpD.f2=s1;
_tmpD;});void*_tmpF;void*_tmp10;void*_tmp11;void*_tmp12;void*_tmp13;void*_tmp14;
void*_tmp15;void*_tmp16;void*_tmp17;void*_tmp18;void*_tmp19;void*_tmp1A;void*
_tmp1B;void*_tmp1C;void*_tmp1D;void*_tmp1E;void*_tmp1F;void*_tmp20;_LL1: _tmpF=
_tmpE.f1;if((int)_tmpF != 4)goto _LL3;_tmp10=_tmpE.f2;if((int)_tmp10 != 4)goto _LL3;
_LL2: goto _LL0;_LL3: _tmp11=_tmpE.f1;if((int)_tmp11 != 4)goto _LL5;_tmp12=_tmpE.f2;
if((int)_tmp12 != 3)goto _LL5;_LL4: goto _LL6;_LL5: _tmp13=_tmpE.f1;if((int)_tmp13 != 
3)goto _LL7;_tmp14=_tmpE.f2;if((int)_tmp14 != 4)goto _LL7;_LL6: if(externCmerge)goto
_LL0;goto _LL8;_LL7: _tmp15=_tmpE.f1;if((int)_tmp15 != 4)goto _LL9;_LL8: goto _LLA;
_LL9: _tmp16=_tmpE.f2;if((int)_tmp16 != 4)goto _LLB;_LLA: Cyc_Tcdecl_merge_scope_err(
s0,s1,t,v,loc,msg);return({struct _tuple5 _tmp21;_tmp21.f1=s1;_tmp21.f2=0;_tmp21;});
_LLB: _tmp17=_tmpE.f2;if((int)_tmp17 != 3)goto _LLD;_LLC: s1=s0;goto _LL0;_LLD: _tmp18=
_tmpE.f1;if((int)_tmp18 != 3)goto _LLF;_LLE: goto _LL0;_LLF: _tmp19=_tmpE.f1;if((int)
_tmp19 != 0)goto _LL11;_tmp1A=_tmpE.f2;if((int)_tmp1A != 0)goto _LL11;_LL10: goto
_LL12;_LL11: _tmp1B=_tmpE.f1;if((int)_tmp1B != 2)goto _LL13;_tmp1C=_tmpE.f2;if((int)
_tmp1C != 2)goto _LL13;_LL12: goto _LL14;_LL13: _tmp1D=_tmpE.f1;if((int)_tmp1D != 1)
goto _LL15;_tmp1E=_tmpE.f2;if((int)_tmp1E != 1)goto _LL15;_LL14: goto _LL0;_LL15:
_tmp1F=_tmpE.f1;if((int)_tmp1F != 5)goto _LL17;_tmp20=_tmpE.f2;if((int)_tmp20 != 5)
goto _LL17;_LL16: goto _LL0;_LL17:;_LL18: Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,
msg);return({struct _tuple5 _tmp22;_tmp22.f1=s1;_tmp22.f2=0;_tmp22;});_LL0:;}
return({struct _tuple5 _tmp23;_tmp23.f1=s1;_tmp23.f2=1;_tmp23;});}static int Cyc_Tcdecl_check_type(
void*t0,void*t1){return Cyc_Tcutil_unify(t0,t1);}static unsigned int Cyc_Tcdecl_get_uint_const_value(
struct Cyc_Absyn_Exp*e){void*_tmp24=e->r;union Cyc_Absyn_Cnst _tmp25;struct _tuple5
_tmp26;int _tmp27;_LL1A: if(*((int*)_tmp24)!= 0)goto _LL1C;_tmp25=((struct Cyc_Absyn_Const_e_struct*)
_tmp24)->f1;if((_tmp25.Int_c).tag != 4)goto _LL1C;_tmp26=(struct _tuple5)(_tmp25.Int_c).val;
_tmp27=_tmp26.f2;_LL1B: return(unsigned int)_tmp27;_LL1C:;_LL1D:(int)_throw((void*)({
struct Cyc_Core_Invalid_argument_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28[
0]=({struct Cyc_Core_Invalid_argument_struct _tmp29;_tmp29.tag=Cyc_Core_Invalid_argument;
_tmp29.f1=({const char*_tmp2A="Tcdecl::get_uint_const_value";_tag_dyneither(
_tmp2A,sizeof(char),29);});_tmp29;});_tmp28;}));_LL19:;}inline static int Cyc_Tcdecl_check_tvs(
struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,struct _dyneither_ptr t,struct
_dyneither_ptr v,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs0)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(tvs1)){({struct Cyc_String_pa_struct _tmp2E;_tmp2E.tag=0;
_tmp2E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);{struct Cyc_String_pa_struct
_tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);{
void*_tmp2B[2]={& _tmp2D,& _tmp2E};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp2C="%s %s has a different number of type parameters";
_tag_dyneither(_tmp2C,sizeof(char),48);}),_tag_dyneither(_tmp2B,sizeof(void*),2));}}});
return 0;}{struct Cyc_List_List*_tmp2F=tvs0;struct Cyc_List_List*_tmp30=tvs1;for(0;
_tmp2F != 0;(_tmp2F=_tmp2F->tl,_tmp30=_tmp30->tl)){void*_tmp31=Cyc_Absyn_force_kb(((
struct Cyc_Absyn_Tvar*)_tmp2F->hd)->kind);void*_tmp32=Cyc_Absyn_force_kb(((struct
Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp30))->hd)->kind);if(
_tmp31 == _tmp32)continue;({struct Cyc_String_pa_struct _tmp39;_tmp39.tag=0;_tmp39.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp32));{
struct Cyc_String_pa_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp2F->hd)->name);{struct Cyc_String_pa_struct
_tmp37;_tmp37.tag=0;_tmp37.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp31));{struct Cyc_String_pa_struct _tmp36;_tmp36.tag=0;_tmp36.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)v);{struct Cyc_String_pa_struct _tmp35;
_tmp35.tag=0;_tmp35.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);{void*
_tmp33[5]={& _tmp35,& _tmp36,& _tmp37,& _tmp38,& _tmp39};Cyc_Tcdecl_merr(loc,msg,({
const char*_tmp34="%s %s has a different kind (%s) for type parameter %s (%s)";
_tag_dyneither(_tmp34,sizeof(char),59);}),_tag_dyneither(_tmp33,sizeof(void*),5));}}}}}});
return 0;}return 1;}}static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,
struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg){if(!Cyc_Tcutil_same_atts(atts0,atts1)){({struct Cyc_String_pa_struct
_tmp3D;_tmp3D.tag=0;_tmp3D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);{
struct Cyc_String_pa_struct _tmp3C;_tmp3C.tag=0;_tmp3C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);{void*_tmp3A[2]={& _tmp3C,& _tmp3D};Cyc_Tcdecl_merr(loc,
msg,({const char*_tmp3B="%s %s has different attributes";_tag_dyneither(_tmp3B,
sizeof(char),31);}),_tag_dyneither(_tmp3A,sizeof(void*),2));}}});return 0;}return
1;}struct _tuple10{struct Cyc_Absyn_Tvar*f1;void*f2;};static struct Cyc_List_List*
Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){
struct Cyc_List_List*inst=0;for(0;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){inst=({
struct Cyc_List_List*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E->hd=({struct
_tuple10*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->f1=(struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(tvs1))->hd;_tmp3F->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40[0]=({struct Cyc_Absyn_VarType_struct
_tmp41;_tmp41.tag=1;_tmp41.f1=(struct Cyc_Absyn_Tvar*)tvs0->hd;_tmp41;});_tmp40;});
_tmp3F;});_tmp3E->tl=inst;_tmp3E;});}return inst;}struct _tuple11{struct Cyc_Absyn_AggrdeclImpl*
f1;struct Cyc_Absyn_AggrdeclImpl*f2;};struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(
struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg){struct _dyneither_ptr _tmp42=Cyc_Absynpp_qvar2string(
d0->name);int _tmp43=1;if(!(d0->kind == d1->kind))return 0;if(!Cyc_Tcdecl_check_tvs(
d0->tvs,d1->tvs,({const char*_tmp44="type";_tag_dyneither(_tmp44,sizeof(char),5);}),
_tmp42,loc,msg))return 0;{void*_tmp47;int _tmp48;struct _tuple5 _tmp46=Cyc_Tcdecl_merge_scope(
d0->sc,d1->sc,({const char*_tmp45="type";_tag_dyneither(_tmp45,sizeof(char),5);}),
_tmp42,loc,msg,1);_tmp47=_tmp46.f1;_tmp48=_tmp46.f2;if(!_tmp48)_tmp43=0;if(!Cyc_Tcdecl_check_atts(
d0->attributes,d1->attributes,({const char*_tmp49="type";_tag_dyneither(_tmp49,
sizeof(char),5);}),_tmp42,loc,msg))_tmp43=0;{struct Cyc_Absyn_Aggrdecl*d2;{struct
_tuple11 _tmp4B=({struct _tuple11 _tmp4A;_tmp4A.f1=d0->impl;_tmp4A.f2=d1->impl;
_tmp4A;});struct Cyc_Absyn_AggrdeclImpl*_tmp4C;struct Cyc_Absyn_AggrdeclImpl*
_tmp4D;struct Cyc_Absyn_AggrdeclImpl*_tmp4E;struct Cyc_Absyn_AggrdeclImpl _tmp4F;
struct Cyc_List_List*_tmp50;struct Cyc_List_List*_tmp51;struct Cyc_List_List*_tmp52;
int _tmp53;struct Cyc_Absyn_AggrdeclImpl*_tmp54;struct Cyc_Absyn_AggrdeclImpl _tmp55;
struct Cyc_List_List*_tmp56;struct Cyc_List_List*_tmp57;struct Cyc_List_List*_tmp58;
int _tmp59;_LL1F: _tmp4C=_tmp4B.f2;if(_tmp4C != 0)goto _LL21;_LL20: d2=d0;goto _LL1E;
_LL21: _tmp4D=_tmp4B.f1;if(_tmp4D != 0)goto _LL23;_LL22: d2=d1;goto _LL1E;_LL23:
_tmp4E=_tmp4B.f1;if(_tmp4E == 0)goto _LL1E;_tmp4F=*_tmp4E;_tmp50=_tmp4F.exist_vars;
_tmp51=_tmp4F.rgn_po;_tmp52=_tmp4F.fields;_tmp53=_tmp4F.tagged;_tmp54=_tmp4B.f2;
if(_tmp54 == 0)goto _LL1E;_tmp55=*_tmp54;_tmp56=_tmp55.exist_vars;_tmp57=_tmp55.rgn_po;
_tmp58=_tmp55.fields;_tmp59=_tmp55.tagged;_LL24: if(!Cyc_Tcdecl_check_tvs(_tmp50,
_tmp56,({const char*_tmp5A="type";_tag_dyneither(_tmp5A,sizeof(char),5);}),_tmp42,
loc,msg))return 0;{struct Cyc_List_List*_tmp5B=Cyc_Tcdecl_build_tvs_map(((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
d0->tvs,_tmp50),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(d1->tvs,_tmp56));for(0;_tmp51 != 0  && _tmp57 != 0;(_tmp51=
_tmp51->tl,_tmp57=_tmp57->tl)){Cyc_Tcdecl_check_type((*((struct _tuple9*)_tmp51->hd)).f1,(*((
struct _tuple9*)_tmp57->hd)).f1);Cyc_Tcdecl_check_type((*((struct _tuple9*)_tmp51->hd)).f2,(*((
struct _tuple9*)_tmp57->hd)).f2);}for(0;_tmp52 != 0  && _tmp58 != 0;(_tmp52=_tmp52->tl,
_tmp58=_tmp58->tl)){struct Cyc_Absyn_Aggrfield _tmp5D;struct _dyneither_ptr*_tmp5E;
struct Cyc_Absyn_Tqual _tmp5F;void*_tmp60;struct Cyc_Absyn_Exp*_tmp61;struct Cyc_List_List*
_tmp62;struct Cyc_Absyn_Aggrfield*_tmp5C=(struct Cyc_Absyn_Aggrfield*)_tmp52->hd;
_tmp5D=*_tmp5C;_tmp5E=_tmp5D.name;_tmp5F=_tmp5D.tq;_tmp60=_tmp5D.type;_tmp61=
_tmp5D.width;_tmp62=_tmp5D.attributes;{struct Cyc_Absyn_Aggrfield _tmp64;struct
_dyneither_ptr*_tmp65;struct Cyc_Absyn_Tqual _tmp66;void*_tmp67;struct Cyc_Absyn_Exp*
_tmp68;struct Cyc_List_List*_tmp69;struct Cyc_Absyn_Aggrfield*_tmp63=(struct Cyc_Absyn_Aggrfield*)
_tmp58->hd;_tmp64=*_tmp63;_tmp65=_tmp64.name;_tmp66=_tmp64.tq;_tmp67=_tmp64.type;
_tmp68=_tmp64.width;_tmp69=_tmp64.attributes;if(Cyc_strptrcmp(_tmp5E,_tmp65)!= 0){({
struct Cyc_String_pa_struct _tmp6F;_tmp6F.tag=0;_tmp6F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp65);{struct Cyc_String_pa_struct _tmp6E;_tmp6E.tag=0;
_tmp6E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp5E);{struct Cyc_String_pa_struct
_tmp6D;_tmp6D.tag=0;_tmp6D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp42);{struct Cyc_String_pa_struct _tmp6C;_tmp6C.tag=0;_tmp6C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)({const char*_tmp70="type";_tag_dyneither(
_tmp70,sizeof(char),5);}));{void*_tmp6A[4]={& _tmp6C,& _tmp6D,& _tmp6E,& _tmp6F};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp6B="%s %s : field name mismatch %s != %s";_tag_dyneither(
_tmp6B,sizeof(char),37);}),_tag_dyneither(_tmp6A,sizeof(void*),4));}}}}});return
0;}if(!Cyc_Tcutil_same_atts(_tmp62,_tmp69)){({struct Cyc_String_pa_struct _tmp75;
_tmp75.tag=0;_tmp75.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp5E);{
struct Cyc_String_pa_struct _tmp74;_tmp74.tag=0;_tmp74.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp42);{struct Cyc_String_pa_struct _tmp73;_tmp73.tag=0;
_tmp73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)({const char*_tmp76="type";
_tag_dyneither(_tmp76,sizeof(char),5);}));{void*_tmp71[3]={& _tmp73,& _tmp74,&
_tmp75};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp72="%s %s : attribute mismatch on field %s";
_tag_dyneither(_tmp72,sizeof(char),39);}),_tag_dyneither(_tmp71,sizeof(void*),3));}}}});
_tmp43=0;}if(!Cyc_Tcutil_equal_tqual(_tmp5F,_tmp66)){({struct Cyc_String_pa_struct
_tmp7B;_tmp7B.tag=0;_tmp7B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp5E);{struct Cyc_String_pa_struct _tmp7A;_tmp7A.tag=0;_tmp7A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp42);{struct Cyc_String_pa_struct _tmp79;
_tmp79.tag=0;_tmp79.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)({const char*
_tmp7C="type";_tag_dyneither(_tmp7C,sizeof(char),5);}));{void*_tmp77[3]={& _tmp79,&
_tmp7A,& _tmp7B};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp78="%s %s : qualifier mismatch on field %s";
_tag_dyneither(_tmp78,sizeof(char),39);}),_tag_dyneither(_tmp77,sizeof(void*),3));}}}});
_tmp43=0;}if(((_tmp61 != 0  && _tmp68 != 0) && Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)_tmp61)!= Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)
_tmp68) || _tmp61 == 0  && _tmp68 != 0) || _tmp61 != 0  && _tmp68 == 0){({struct Cyc_String_pa_struct
_tmp81;_tmp81.tag=0;_tmp81.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp5E);{struct Cyc_String_pa_struct _tmp80;_tmp80.tag=0;_tmp80.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp42);{struct Cyc_String_pa_struct _tmp7F;
_tmp7F.tag=0;_tmp7F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)({const char*
_tmp82="type";_tag_dyneither(_tmp82,sizeof(char),5);}));{void*_tmp7D[3]={& _tmp7F,&
_tmp80,& _tmp81};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp7E="%s %s : bitfield mismatch on field %s";
_tag_dyneither(_tmp7E,sizeof(char),38);}),_tag_dyneither(_tmp7D,sizeof(void*),3));}}}});
_tmp43=0;}{void*subst_t1=Cyc_Tcutil_substitute(_tmp5B,_tmp67);if(!Cyc_Tcdecl_check_type(
_tmp60,subst_t1)){({struct Cyc_String_pa_struct _tmp88;_tmp88.tag=0;_tmp88.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_t1));{
struct Cyc_String_pa_struct _tmp87;_tmp87.tag=0;_tmp87.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp60));{struct Cyc_String_pa_struct
_tmp86;_tmp86.tag=0;_tmp86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp5E);{struct Cyc_String_pa_struct _tmp85;_tmp85.tag=0;_tmp85.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp42);{void*_tmp83[4]={& _tmp85,& _tmp86,&
_tmp87,& _tmp88};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp84="type %s : type mismatch on field %s: %s != %s";
_tag_dyneither(_tmp84,sizeof(char),46);}),_tag_dyneither(_tmp83,sizeof(void*),4));}}}}});
Cyc_Tcutil_explain_failure();_tmp43=0;}}}}if(_tmp52 != 0){({struct Cyc_String_pa_struct
_tmp8C;_tmp8C.tag=0;_tmp8C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Aggrfield*)_tmp52->hd)->name);{struct Cyc_String_pa_struct _tmp8B;
_tmp8B.tag=0;_tmp8B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp42);{
void*_tmp89[2]={& _tmp8B,& _tmp8C};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp8A="type %s is missing field %s";
_tag_dyneither(_tmp8A,sizeof(char),28);}),_tag_dyneither(_tmp89,sizeof(void*),2));}}});
_tmp43=0;}if(_tmp58 != 0){({struct Cyc_String_pa_struct _tmp90;_tmp90.tag=0;_tmp90.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp58->hd)->name);{
struct Cyc_String_pa_struct _tmp8F;_tmp8F.tag=0;_tmp8F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp42);{void*_tmp8D[2]={& _tmp8F,& _tmp90};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp8E="type %s has extra field %s";_tag_dyneither(_tmp8E,
sizeof(char),27);}),_tag_dyneither(_tmp8D,sizeof(void*),2));}}});_tmp43=0;}if(
_tmp53 != _tmp59){({struct Cyc_String_pa_struct _tmp93;_tmp93.tag=0;_tmp93.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp42);{void*_tmp91[1]={& _tmp93};
Cyc_Tcdecl_merr(loc,msg,({const char*_tmp92="%s : previous declaration disagrees with @tagged qualifier";
_tag_dyneither(_tmp92,sizeof(char),59);}),_tag_dyneither(_tmp91,sizeof(void*),1));}});
_tmp43=0;}d2=d0;goto _LL1E;}_LL1E:;}if(!_tmp43)return 0;if(_tmp47 == d2->sc)return(
struct Cyc_Absyn_Aggrdecl*)d2;else{d2=({struct Cyc_Absyn_Aggrdecl*_tmp94=_cycalloc(
sizeof(*_tmp94));_tmp94[0]=*d2;_tmp94;});d2->sc=_tmp47;return(struct Cyc_Absyn_Aggrdecl*)
d2;}}}}inline static struct _dyneither_ptr Cyc_Tcdecl_is_x2string(int is_x){return
is_x?({const char*_tmp95="@extensible datatype";_tag_dyneither(_tmp95,sizeof(char),
21);}):({const char*_tmp96="datatype";_tag_dyneither(_tmp96,sizeof(char),9);});}
struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};static struct Cyc_Absyn_Datatypefield*
Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*f0,struct Cyc_Absyn_Datatypefield*
f1,struct Cyc_List_List*inst,struct _dyneither_ptr t,struct _dyneither_ptr v,struct
_dyneither_ptr*msg){struct Cyc_Position_Segment*loc=f1->loc;if(Cyc_strptrcmp((*f0->name).f2,(*
f1->name).f2)!= 0){({struct Cyc_String_pa_struct _tmp9C;_tmp9C.tag=0;_tmp9C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*f0->name).f2);{struct Cyc_String_pa_struct
_tmp9B;_tmp9B.tag=0;_tmp9B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f1->name).f2);{
struct Cyc_String_pa_struct _tmp9A;_tmp9A.tag=0;_tmp9A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)v);{struct Cyc_String_pa_struct _tmp99;_tmp99.tag=0;_tmp99.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)t);{void*_tmp97[4]={& _tmp99,& _tmp9A,&
_tmp9B,& _tmp9C};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp98="%s %s: field name mismatch %s != %s";
_tag_dyneither(_tmp98,sizeof(char),36);}),_tag_dyneither(_tmp97,sizeof(void*),4));}}}}});
return 0;}{struct _dyneither_ptr _tmp9D=*(*f0->name).f2;void*_tmpA3;int _tmpA4;
struct _tuple5 _tmpA2=Cyc_Tcdecl_merge_scope(f0->sc,f1->sc,(struct _dyneither_ptr)({
struct Cyc_String_pa_struct _tmpA1;_tmpA1.tag=0;_tmpA1.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)v);{struct Cyc_String_pa_struct _tmpA0;_tmpA0.tag=0;_tmpA0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)t);{void*_tmp9E[2]={& _tmpA0,& _tmpA1};
Cyc_aprintf(({const char*_tmp9F="in %s %s, field";_tag_dyneither(_tmp9F,sizeof(
char),16);}),_tag_dyneither(_tmp9E,sizeof(void*),2));}}}),_tmp9D,loc,msg,0);
_tmpA3=_tmpA2.f1;_tmpA4=_tmpA2.f2;{struct Cyc_List_List*_tmpA5=f0->typs;struct Cyc_List_List*
_tmpA6=f1->typs;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA5)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA6)){({struct Cyc_String_pa_struct
_tmpAB;_tmpAB.tag=0;_tmpAB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp9D);{struct Cyc_String_pa_struct _tmpAA;_tmpAA.tag=0;_tmpAA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)v);{struct Cyc_String_pa_struct _tmpA9;
_tmpA9.tag=0;_tmpA9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);{void*
_tmpA7[3]={& _tmpA9,& _tmpAA,& _tmpAB};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpA8="%s %s, field %s: parameter number mismatch";
_tag_dyneither(_tmpA8,sizeof(char),43);}),_tag_dyneither(_tmpA7,sizeof(void*),3));}}}});
_tmpA4=0;}for(0;_tmpA5 != 0;(_tmpA5=_tmpA5->tl,_tmpA6=_tmpA6->tl)){if(!Cyc_Tcutil_equal_tqual((*((
struct _tuple12*)_tmpA5->hd)).f1,(*((struct _tuple12*)((struct Cyc_List_List*)
_check_null(_tmpA6))->hd)).f1)){({struct Cyc_String_pa_struct _tmpB0;_tmpB0.tag=0;
_tmpB0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9D);{struct Cyc_String_pa_struct
_tmpAF;_tmpAF.tag=0;_tmpAF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);{
struct Cyc_String_pa_struct _tmpAE;_tmpAE.tag=0;_tmpAE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);{void*_tmpAC[3]={& _tmpAE,& _tmpAF,& _tmpB0};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmpAD="%s %s, field %s: parameter qualifier";_tag_dyneither(
_tmpAD,sizeof(char),37);}),_tag_dyneither(_tmpAC,sizeof(void*),3));}}}});_tmpA4=
0;}{void*subst_t1=Cyc_Tcutil_substitute(inst,(*((struct _tuple12*)_tmpA6->hd)).f2);
if(!Cyc_Tcdecl_check_type((*((struct _tuple12*)_tmpA5->hd)).f2,subst_t1)){({
struct Cyc_String_pa_struct _tmpB7;_tmpB7.tag=0;_tmpB7.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_t1));{struct Cyc_String_pa_struct
_tmpB6;_tmpB6.tag=0;_tmpB6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((*((
struct _tuple12*)_tmpA5->hd)).f2));{struct Cyc_String_pa_struct _tmpB5;_tmpB5.tag=0;
_tmpB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9D);{struct Cyc_String_pa_struct
_tmpB4;_tmpB4.tag=0;_tmpB4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);{
struct Cyc_String_pa_struct _tmpB3;_tmpB3.tag=0;_tmpB3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);{void*_tmpB1[5]={& _tmpB3,& _tmpB4,& _tmpB5,& _tmpB6,& _tmpB7};
Cyc_Tcdecl_merr(loc,msg,({const char*_tmpB2="%s %s, field %s: parameter type mismatch %s != %s";
_tag_dyneither(_tmpB2,sizeof(char),50);}),_tag_dyneither(_tmpB1,sizeof(void*),5));}}}}}});
Cyc_Tcutil_explain_failure();_tmpA4=0;}}}if(!_tmpA4)return 0;if(f0->sc != _tmpA3){
struct Cyc_Absyn_Datatypefield*_tmpB8=({struct Cyc_Absyn_Datatypefield*_tmpB9=
_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=*f0;_tmpB9;});((struct Cyc_Absyn_Datatypefield*)
_check_null(_tmpB8))->sc=_tmpA3;return _tmpB8;}else{return(struct Cyc_Absyn_Datatypefield*)
f0;}}}}static struct _tuple12*Cyc_Tcdecl_substitute_datatypefield_f2(struct Cyc_List_List*
inst,struct _tuple12*x){struct _tuple12 _tmpBB;struct Cyc_Absyn_Tqual _tmpBC;void*
_tmpBD;struct _tuple12*_tmpBA=x;_tmpBB=*_tmpBA;_tmpBC=_tmpBB.f1;_tmpBD=_tmpBB.f2;
return({struct _tuple12*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->f1=_tmpBC;
_tmpBE->f2=Cyc_Tcutil_substitute(inst,_tmpBD);_tmpBE;});}static struct Cyc_Absyn_Datatypefield*
Cyc_Tcdecl_substitute_datatypefield(struct Cyc_List_List*inst1,struct Cyc_Absyn_Datatypefield*
f1){struct Cyc_Absyn_Datatypefield*_tmpBF=({struct Cyc_Absyn_Datatypefield*_tmpC0=
_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=*f1;_tmpC0;});_tmpBF->typs=((struct Cyc_List_List*(*)(
struct _tuple12*(*f)(struct Cyc_List_List*,struct _tuple12*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcdecl_substitute_datatypefield_f2,
inst1,f1->typs);return _tmpBF;}static struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(
struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct
Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct
_dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){struct Cyc_List_List**f2sp=({struct Cyc_List_List**_tmpC9=
_cycalloc(sizeof(*_tmpC9));_tmpC9[0]=0;_tmpC9;});struct Cyc_List_List**_tmpC1=
f2sp;int cmp=- 1;for(0;f0s != 0  && f1s != 0;f1s=f1s->tl){while(f0s != 0  && (cmp=Cyc_Absyn_qvar_cmp(((
struct Cyc_Absyn_Datatypefield*)f0s->hd)->name,((struct Cyc_Absyn_Datatypefield*)
f1s->hd)->name))< 0){struct Cyc_List_List*_tmpC2=({struct Cyc_List_List*_tmpC3=
_cycalloc(sizeof(*_tmpC3));_tmpC3->hd=(struct Cyc_Absyn_Datatypefield*)f0s->hd;
_tmpC3->tl=0;_tmpC3;});*_tmpC1=_tmpC2;_tmpC1=&((struct Cyc_List_List*)_check_null(
_tmpC2))->tl;f0s=f0s->tl;}if(f0s == 0  || cmp > 0){*incl=0;{struct Cyc_List_List*
_tmpC4=({struct Cyc_List_List*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->hd=Cyc_Tcdecl_substitute_datatypefield(
inst,(struct Cyc_Absyn_Datatypefield*)f1s->hd);_tmpC5->tl=0;_tmpC5;});*_tmpC1=
_tmpC4;_tmpC1=&((struct Cyc_List_List*)_check_null(_tmpC4))->tl;}}else{struct Cyc_Absyn_Datatypefield*
_tmpC6=Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)f0s->hd,(
struct Cyc_Absyn_Datatypefield*)f1s->hd,inst,t,v,msg);if(_tmpC6 != 0){if(_tmpC6 != (
struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)f0s->hd))*incl=0;{
struct Cyc_List_List*_tmpC7=({struct Cyc_List_List*_tmpC8=_cycalloc(sizeof(*_tmpC8));
_tmpC8->hd=(struct Cyc_Absyn_Datatypefield*)_tmpC6;_tmpC8->tl=0;_tmpC8;});*_tmpC1=
_tmpC7;_tmpC1=&((struct Cyc_List_List*)_check_null(_tmpC7))->tl;}}else{*res=0;}
f0s=f0s->tl;}}if(f1s != 0){*incl=0;*_tmpC1=f1s;}else{*_tmpC1=f0s;}return*f2sp;}
struct _tuple13{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};static struct
_tuple13 Cyc_Tcdecl_split(struct Cyc_List_List*f){if(f == 0)return({struct _tuple13
_tmpCA;_tmpCA.f1=0;_tmpCA.f2=0;_tmpCA;});if(f->tl == 0)return({struct _tuple13
_tmpCB;_tmpCB.f1=f;_tmpCB.f2=0;_tmpCB;});{struct Cyc_List_List*_tmpCD;struct Cyc_List_List*
_tmpCE;struct _tuple13 _tmpCC=Cyc_Tcdecl_split(((struct Cyc_List_List*)_check_null(
f->tl))->tl);_tmpCD=_tmpCC.f1;_tmpCE=_tmpCC.f2;return({struct _tuple13 _tmpCF;
_tmpCF.f1=({struct Cyc_List_List*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->hd=(
void*)((void*)f->hd);_tmpD1->tl=_tmpCD;_tmpD1;});_tmpCF.f2=({struct Cyc_List_List*
_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->hd=(void*)((void*)((struct Cyc_List_List*)
_check_null(f->tl))->hd);_tmpD0->tl=_tmpCE;_tmpD0;});_tmpCF;});}}struct Cyc_List_List*
Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct
_dyneither_ptr*v,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){struct
Cyc_List_List*_tmpD3;struct Cyc_List_List*_tmpD4;struct _tuple13 _tmpD2=((struct
_tuple13(*)(struct Cyc_List_List*f))Cyc_Tcdecl_split)(f);_tmpD3=_tmpD2.f1;_tmpD4=
_tmpD2.f2;if(_tmpD3 != 0  && _tmpD3->tl != 0)_tmpD3=Cyc_Tcdecl_sort_xdatatype_fields(
_tmpD3,res,v,loc,msg);if(_tmpD4 != 0  && _tmpD4->tl != 0)_tmpD4=Cyc_Tcdecl_sort_xdatatype_fields(
_tmpD4,res,v,loc,msg);return Cyc_Tcdecl_merge_xdatatype_fields(_tmpD3,_tmpD4,0,0,
0,res,({int*_tmpD5=_cycalloc_atomic(sizeof(*_tmpD5));_tmpD5[0]=1;_tmpD5;}),({
const char*_tmpD6="@extensible datatype";_tag_dyneither(_tmpD6,sizeof(char),21);}),*
v,loc,msg);}struct _tuple14{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};struct
Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*
d0,struct Cyc_Absyn_Datatypedecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){struct _dyneither_ptr _tmpD7=Cyc_Absynpp_qvar2string(d0->name);
struct _dyneither_ptr t=({const char*_tmpFB="datatype";_tag_dyneither(_tmpFB,
sizeof(char),9);});int _tmpD8=1;if(d0->is_extensible != d1->is_extensible){({
struct Cyc_String_pa_struct _tmpDD;_tmpDD.tag=0;_tmpDD.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(d1->is_extensible));{struct Cyc_String_pa_struct
_tmpDC;_tmpDC.tag=0;_tmpDC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(
d0->is_extensible));{struct Cyc_String_pa_struct _tmpDB;_tmpDB.tag=0;_tmpDB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD7);{void*_tmpD9[3]={& _tmpDB,&
_tmpDC,& _tmpDD};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpDA="expected %s to be a %s instead of a %s";
_tag_dyneither(_tmpDA,sizeof(char),39);}),_tag_dyneither(_tmpD9,sizeof(void*),3));}}}});
_tmpD8=0;}else{t=Cyc_Tcdecl_is_x2string(d0->is_extensible);}if(!Cyc_Tcdecl_check_tvs(
d0->tvs,d1->tvs,t,_tmpD7,loc,msg))return 0;{void*_tmpDF;int _tmpE0;struct _tuple5
_tmpDE=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,t,_tmpD7,loc,msg,0);_tmpDF=_tmpDE.f1;
_tmpE0=_tmpDE.f2;if(!_tmpE0)_tmpD8=0;{struct Cyc_Absyn_Datatypedecl*d2;{struct
_tuple14 _tmpE2=({struct _tuple14 _tmpE1;_tmpE1.f1=d0->fields;_tmpE1.f2=d1->fields;
_tmpE1;});struct Cyc_Core_Opt*_tmpE3;struct Cyc_Core_Opt*_tmpE4;struct Cyc_Core_Opt*
_tmpE5;struct Cyc_Core_Opt _tmpE6;struct Cyc_List_List*_tmpE7;struct Cyc_Core_Opt*
_tmpE8;struct Cyc_Core_Opt _tmpE9;struct Cyc_List_List*_tmpEA;_LL26: _tmpE3=_tmpE2.f2;
if(_tmpE3 != 0)goto _LL28;_LL27: d2=d0;goto _LL25;_LL28: _tmpE4=_tmpE2.f1;if(_tmpE4 != 
0)goto _LL2A;_LL29: d2=d1;goto _LL25;_LL2A: _tmpE5=_tmpE2.f1;if(_tmpE5 == 0)goto _LL25;
_tmpE6=*_tmpE5;_tmpE7=(struct Cyc_List_List*)_tmpE6.v;_tmpE8=_tmpE2.f2;if(_tmpE8
== 0)goto _LL25;_tmpE9=*_tmpE8;_tmpEA=(struct Cyc_List_List*)_tmpE9.v;_LL2B: {
struct Cyc_List_List*_tmpEB=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);if(d0->is_extensible){
int _tmpEC=1;struct Cyc_List_List*_tmpED=Cyc_Tcdecl_merge_xdatatype_fields(_tmpE7,
_tmpEA,_tmpEB,d0->tvs,d1->tvs,& _tmpD8,& _tmpEC,t,_tmpD7,loc,msg);if(_tmpEC)d2=d0;
else{d2=({struct Cyc_Absyn_Datatypedecl*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE[
0]=*d0;_tmpEE;});d2->sc=_tmpDF;d2->fields=({struct Cyc_Core_Opt*_tmpEF=_cycalloc(
sizeof(*_tmpEF));_tmpEF->v=_tmpED;_tmpEF;});}}else{for(0;_tmpE7 != 0  && _tmpEA != 
0;(_tmpE7=_tmpE7->tl,_tmpEA=_tmpEA->tl)){Cyc_Tcdecl_merge_datatypefield((struct
Cyc_Absyn_Datatypefield*)_tmpE7->hd,(struct Cyc_Absyn_Datatypefield*)_tmpEA->hd,
_tmpEB,t,_tmpD7,msg);}if(_tmpE7 != 0){({struct Cyc_String_pa_struct _tmpF4;_tmpF4.tag=
0;_tmpF4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)((
struct Cyc_List_List*)_check_null(_tmpEA))->hd)->name).f2);{struct Cyc_String_pa_struct
_tmpF3;_tmpF3.tag=0;_tmpF3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmpD7);{struct Cyc_String_pa_struct _tmpF2;_tmpF2.tag=0;_tmpF2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)t);{void*_tmpF0[3]={& _tmpF2,& _tmpF3,&
_tmpF4};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpF1="%s %s has extra field %s";
_tag_dyneither(_tmpF1,sizeof(char),25);}),_tag_dyneither(_tmpF0,sizeof(void*),3));}}}});
_tmpD8=0;}if(_tmpEA != 0){({struct Cyc_String_pa_struct _tmpF9;_tmpF9.tag=0;_tmpF9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)
_tmpEA->hd)->name).f2);{struct Cyc_String_pa_struct _tmpF8;_tmpF8.tag=0;_tmpF8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD7);{struct Cyc_String_pa_struct
_tmpF7;_tmpF7.tag=0;_tmpF7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);{
void*_tmpF5[3]={& _tmpF7,& _tmpF8,& _tmpF9};Cyc_Tcdecl_merr(loc,msg,({const char*
_tmpF6="%s %s is missing field %s";_tag_dyneither(_tmpF6,sizeof(char),26);}),
_tag_dyneither(_tmpF5,sizeof(void*),3));}}}});_tmpD8=0;}d2=d0;}goto _LL25;}_LL25:;}
if(!_tmpD8)return 0;if(_tmpDF == d2->sc)return(struct Cyc_Absyn_Datatypedecl*)d2;
else{d2=({struct Cyc_Absyn_Datatypedecl*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA[
0]=*d2;_tmpFA;});d2->sc=_tmpDF;return(struct Cyc_Absyn_Datatypedecl*)d2;}}}}
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,
struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*
msg){struct _dyneither_ptr _tmpFC=Cyc_Absynpp_qvar2string(d0->name);int _tmpFD=1;
void*_tmp100;int _tmp101;struct _tuple5 _tmpFF=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,({
const char*_tmpFE="enum";_tag_dyneither(_tmpFE,sizeof(char),5);}),_tmpFC,loc,msg,
1);_tmp100=_tmpFF.f1;_tmp101=_tmpFF.f2;if(!_tmp101)_tmpFD=0;{struct Cyc_Absyn_Enumdecl*
d2;{struct _tuple14 _tmp103=({struct _tuple14 _tmp102;_tmp102.f1=d0->fields;_tmp102.f2=
d1->fields;_tmp102;});struct Cyc_Core_Opt*_tmp104;struct Cyc_Core_Opt*_tmp105;
struct Cyc_Core_Opt*_tmp106;struct Cyc_Core_Opt _tmp107;struct Cyc_List_List*_tmp108;
struct Cyc_Core_Opt*_tmp109;struct Cyc_Core_Opt _tmp10A;struct Cyc_List_List*_tmp10B;
_LL2D: _tmp104=_tmp103.f2;if(_tmp104 != 0)goto _LL2F;_LL2E: d2=d0;goto _LL2C;_LL2F:
_tmp105=_tmp103.f1;if(_tmp105 != 0)goto _LL31;_LL30: d2=d1;goto _LL2C;_LL31: _tmp106=
_tmp103.f1;if(_tmp106 == 0)goto _LL2C;_tmp107=*_tmp106;_tmp108=(struct Cyc_List_List*)
_tmp107.v;_tmp109=_tmp103.f2;if(_tmp109 == 0)goto _LL2C;_tmp10A=*_tmp109;_tmp10B=(
struct Cyc_List_List*)_tmp10A.v;_LL32: for(0;_tmp108 != 0  && _tmp10B != 0;(_tmp108=
_tmp108->tl,_tmp10B=_tmp10B->tl)){struct Cyc_Absyn_Enumfield _tmp10D;struct _tuple0*
_tmp10E;struct _tuple0 _tmp10F;struct _dyneither_ptr*_tmp110;struct Cyc_Absyn_Exp*
_tmp111;struct Cyc_Position_Segment*_tmp112;struct Cyc_Absyn_Enumfield*_tmp10C=(
struct Cyc_Absyn_Enumfield*)_tmp108->hd;_tmp10D=*_tmp10C;_tmp10E=_tmp10D.name;
_tmp10F=*_tmp10E;_tmp110=_tmp10F.f2;_tmp111=_tmp10D.tag;_tmp112=_tmp10D.loc;{
struct Cyc_Absyn_Enumfield _tmp114;struct _tuple0*_tmp115;struct _tuple0 _tmp116;
struct _dyneither_ptr*_tmp117;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Position_Segment*
_tmp119;struct Cyc_Absyn_Enumfield*_tmp113=(struct Cyc_Absyn_Enumfield*)_tmp10B->hd;
_tmp114=*_tmp113;_tmp115=_tmp114.name;_tmp116=*_tmp115;_tmp117=_tmp116.f2;
_tmp118=_tmp114.tag;_tmp119=_tmp114.loc;if(Cyc_strptrcmp(_tmp110,_tmp117)!= 0){({
struct Cyc_String_pa_struct _tmp11E;_tmp11E.tag=0;_tmp11E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp117);{struct Cyc_String_pa_struct _tmp11D;_tmp11D.tag=0;
_tmp11D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp110);{struct Cyc_String_pa_struct
_tmp11C;_tmp11C.tag=0;_tmp11C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmpFC);{void*_tmp11A[3]={& _tmp11C,& _tmp11D,& _tmp11E};Cyc_Tcdecl_merr(loc,msg,({
const char*_tmp11B="enum %s: field name mismatch %s != %s";_tag_dyneither(_tmp11B,
sizeof(char),38);}),_tag_dyneither(_tmp11A,sizeof(void*),3));}}}});_tmpFD=0;}if(
Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmp111))!= 
Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmp118))){({
struct Cyc_String_pa_struct _tmp122;_tmp122.tag=0;_tmp122.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp117);{struct Cyc_String_pa_struct _tmp121;_tmp121.tag=0;
_tmp121.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFC);{void*_tmp11F[2]={&
_tmp121,& _tmp122};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp120="enum %s, field %s, value mismatch";
_tag_dyneither(_tmp120,sizeof(char),34);}),_tag_dyneither(_tmp11F,sizeof(void*),
2));}}});_tmpFD=0;}}}d2=d0;goto _LL2C;_LL2C:;}if(!_tmpFD)return 0;if(d2->sc == 
_tmp100)return(struct Cyc_Absyn_Enumdecl*)d2;else{d2=({struct Cyc_Absyn_Enumdecl*
_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123[0]=*d2;_tmp123;});d2->sc=_tmp100;
return(struct Cyc_Absyn_Enumdecl*)d2;}}}static struct _tuple5 Cyc_Tcdecl_check_var_or_fn_decl(
void*sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*atts0,void*sc1,
void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*atts1,struct _dyneither_ptr t,
struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){
int _tmp124=1;void*_tmp126;int _tmp127;struct _tuple5 _tmp125=Cyc_Tcdecl_merge_scope(
sc0,sc1,t,v,loc,msg,0);_tmp126=_tmp125.f1;_tmp127=_tmp125.f2;if(!_tmp127)_tmp124=
0;if(!Cyc_Tcdecl_check_type(t0,t1)){({struct Cyc_String_pa_struct _tmp12D;_tmp12D.tag=
0;_tmp12D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t0));{struct Cyc_String_pa_struct _tmp12C;_tmp12C.tag=0;_tmp12C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{struct Cyc_String_pa_struct
_tmp12B;_tmp12B.tag=0;_tmp12B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);{
struct Cyc_String_pa_struct _tmp12A;_tmp12A.tag=0;_tmp12A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);{void*_tmp128[4]={& _tmp12A,& _tmp12B,& _tmp12C,& _tmp12D};
Cyc_Tcdecl_merr(loc,msg,({const char*_tmp129="%s %s has type \n%s\n instead of \n%s\n";
_tag_dyneither(_tmp129,sizeof(char),36);}),_tag_dyneither(_tmp128,sizeof(void*),
4));}}}}});Cyc_Tcutil_explain_failure();_tmp124=0;}if(!Cyc_Tcutil_equal_tqual(
tq0,tq1)){({struct Cyc_String_pa_struct _tmp131;_tmp131.tag=0;_tmp131.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)v);{struct Cyc_String_pa_struct _tmp130;
_tmp130.tag=0;_tmp130.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);{void*
_tmp12E[2]={& _tmp130,& _tmp131};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp12F="%s %s has different type qualifiers";
_tag_dyneither(_tmp12F,sizeof(char),36);}),_tag_dyneither(_tmp12E,sizeof(void*),
2));}}});_tmp124=0;}if(!Cyc_Tcutil_same_atts(atts0,atts1)){({struct Cyc_String_pa_struct
_tmp135;_tmp135.tag=0;_tmp135.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);{
struct Cyc_String_pa_struct _tmp134;_tmp134.tag=0;_tmp134.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);{void*_tmp132[2]={& _tmp134,& _tmp135};Cyc_Tcdecl_merr(loc,
msg,({const char*_tmp133="%s %s has different attributes";_tag_dyneither(_tmp133,
sizeof(char),31);}),_tag_dyneither(_tmp132,sizeof(void*),2));}}});({void*_tmp136=
0;Cyc_fprintf(Cyc_stderr,({const char*_tmp137="previous attributes: ";
_tag_dyneither(_tmp137,sizeof(char),22);}),_tag_dyneither(_tmp136,sizeof(void*),
0));});for(0;atts0 != 0;atts0=atts0->tl){({struct Cyc_String_pa_struct _tmp13A;
_tmp13A.tag=0;_tmp13A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)atts0->hd));{void*_tmp138[1]={& _tmp13A};Cyc_fprintf(Cyc_stderr,({const char*
_tmp139="%s ";_tag_dyneither(_tmp139,sizeof(char),4);}),_tag_dyneither(_tmp138,
sizeof(void*),1));}});}({void*_tmp13B=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp13C="\ncurrent attributes: ";_tag_dyneither(_tmp13C,sizeof(char),22);}),
_tag_dyneither(_tmp13B,sizeof(void*),0));});for(0;atts1 != 0;atts1=atts1->tl){({
struct Cyc_String_pa_struct _tmp13F;_tmp13F.tag=0;_tmp13F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts1->hd));{void*_tmp13D[
1]={& _tmp13F};Cyc_fprintf(Cyc_stderr,({const char*_tmp13E="%s ";_tag_dyneither(
_tmp13E,sizeof(char),4);}),_tag_dyneither(_tmp13D,sizeof(void*),1));}});}({void*
_tmp140=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp141="\n";_tag_dyneither(_tmp141,
sizeof(char),2);}),_tag_dyneither(_tmp140,sizeof(void*),0));});_tmp124=0;}return({
struct _tuple5 _tmp142;_tmp142.f1=_tmp126;_tmp142.f2=_tmp124;_tmp142;});}struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){struct _dyneither_ptr
_tmp143=Cyc_Absynpp_qvar2string(d0->name);void*_tmp146;int _tmp147;struct _tuple5
_tmp145=Cyc_Tcdecl_check_var_or_fn_decl(d0->sc,d0->type,d0->tq,d0->attributes,d1->sc,
d1->type,d1->tq,d1->attributes,({const char*_tmp144="variable";_tag_dyneither(
_tmp144,sizeof(char),9);}),_tmp143,loc,msg);_tmp146=_tmp145.f1;_tmp147=_tmp145.f2;
if(!_tmp147)return 0;if(d0->sc == _tmp146)return(struct Cyc_Absyn_Vardecl*)d0;else{
struct Cyc_Absyn_Vardecl*_tmp148=({struct Cyc_Absyn_Vardecl*_tmp149=_cycalloc(
sizeof(*_tmp149));_tmp149[0]=*d0;_tmp149;});((struct Cyc_Absyn_Vardecl*)
_check_null(_tmp148))->sc=_tmp146;return _tmp148;}}struct Cyc_Absyn_Typedefdecl*
Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){struct _dyneither_ptr
_tmp14A=Cyc_Absynpp_qvar2string(d0->name);if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,({
const char*_tmp14B="typedef";_tag_dyneither(_tmp14B,sizeof(char),8);}),_tmp14A,
loc,msg))return 0;{struct Cyc_List_List*_tmp14C=Cyc_Tcdecl_build_tvs_map(d0->tvs,
d1->tvs);if(d0->defn != 0  && d1->defn != 0){void*subst_defn1=Cyc_Tcutil_substitute(
_tmp14C,(void*)((struct Cyc_Core_Opt*)_check_null(d1->defn))->v);if(!Cyc_Tcdecl_check_type((
void*)((struct Cyc_Core_Opt*)_check_null(d0->defn))->v,subst_defn1)){({struct Cyc_String_pa_struct
_tmp151;_tmp151.tag=0;_tmp151.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(d0->defn))->v));{
struct Cyc_String_pa_struct _tmp150;_tmp150.tag=0;_tmp150.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_defn1));{struct Cyc_String_pa_struct
_tmp14F;_tmp14F.tag=0;_tmp14F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp14A);{void*_tmp14D[3]={& _tmp14F,& _tmp150,& _tmp151};Cyc_Tcdecl_merr(loc,msg,({
const char*_tmp14E="typedef %s does not refer to the same type: %s != %s";
_tag_dyneither(_tmp14E,sizeof(char),53);}),_tag_dyneither(_tmp14D,sizeof(void*),
3));}}}});return 0;}}return(struct Cyc_Absyn_Typedefdecl*)d0;}}void*Cyc_Tcdecl_merge_binding(
void*b0,void*b1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){struct
_tuple9 _tmp153=({struct _tuple9 _tmp152;_tmp152.f1=b0;_tmp152.f2=b1;_tmp152;});
void*_tmp154;void*_tmp155;void*_tmp156;struct Cyc_Absyn_Vardecl*_tmp157;void*
_tmp158;struct Cyc_Absyn_Vardecl*_tmp159;void*_tmp15A;struct Cyc_Absyn_Vardecl*
_tmp15B;void*_tmp15C;struct Cyc_Absyn_Fndecl*_tmp15D;void*_tmp15E;void*_tmp15F;
struct Cyc_Absyn_Fndecl*_tmp160;void*_tmp161;struct Cyc_Absyn_Fndecl*_tmp162;void*
_tmp163;struct Cyc_Absyn_Vardecl*_tmp164;_LL34: _tmp154=_tmp153.f1;if((int)_tmp154
!= 0)goto _LL36;_tmp155=_tmp153.f2;if((int)_tmp155 != 0)goto _LL36;_LL35: return(
void*)0;_LL36: _tmp156=_tmp153.f1;if(_tmp156 <= (void*)1)goto _LL38;if(*((int*)
_tmp156)!= 0)goto _LL38;_tmp157=((struct Cyc_Absyn_Global_b_struct*)_tmp156)->f1;
_tmp158=_tmp153.f2;if(_tmp158 <= (void*)1)goto _LL38;if(*((int*)_tmp158)!= 0)goto
_LL38;_tmp159=((struct Cyc_Absyn_Global_b_struct*)_tmp158)->f1;_LL37: {struct Cyc_Absyn_Vardecl*
_tmp165=Cyc_Tcdecl_merge_vardecl(_tmp157,_tmp159,loc,msg);if(_tmp165 == 0)return(
void*)0;if(_tmp165 == (struct Cyc_Absyn_Vardecl*)_tmp157)return b0;if(_tmp165 == (
struct Cyc_Absyn_Vardecl*)_tmp159)return b1;return(void*)({struct Cyc_Absyn_Global_b_struct*
_tmp166=_cycalloc(sizeof(*_tmp166));_tmp166[0]=({struct Cyc_Absyn_Global_b_struct
_tmp167;_tmp167.tag=0;_tmp167.f1=(struct Cyc_Absyn_Vardecl*)_tmp165;_tmp167;});
_tmp166;});}_LL38: _tmp15A=_tmp153.f1;if(_tmp15A <= (void*)1)goto _LL3A;if(*((int*)
_tmp15A)!= 0)goto _LL3A;_tmp15B=((struct Cyc_Absyn_Global_b_struct*)_tmp15A)->f1;
_tmp15C=_tmp153.f2;if(_tmp15C <= (void*)1)goto _LL3A;if(*((int*)_tmp15C)!= 1)goto
_LL3A;_tmp15D=((struct Cyc_Absyn_Funname_b_struct*)_tmp15C)->f1;_LL39: {int
_tmp16A;struct _tuple5 _tmp169=Cyc_Tcdecl_check_var_or_fn_decl(_tmp15B->sc,_tmp15B->type,
_tmp15B->tq,_tmp15B->attributes,_tmp15D->sc,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp15D->cached_typ))->v,Cyc_Absyn_empty_tqual(0),_tmp15D->attributes,({
const char*_tmp168="function";_tag_dyneither(_tmp168,sizeof(char),9);}),Cyc_Absynpp_qvar2string(
_tmp15B->name),loc,msg);_tmp16A=_tmp169.f2;return _tmp16A?b1:(void*)0;}_LL3A:
_tmp15E=_tmp153.f1;if(_tmp15E <= (void*)1)goto _LL3C;if(*((int*)_tmp15E)!= 1)goto
_LL3C;_tmp15F=_tmp153.f2;if(_tmp15F <= (void*)1)goto _LL3C;if(*((int*)_tmp15F)!= 1)
goto _LL3C;_tmp160=((struct Cyc_Absyn_Funname_b_struct*)_tmp15F)->f1;_LL3B:({
struct Cyc_String_pa_struct _tmp16D;_tmp16D.tag=0;_tmp16D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp160->name));{void*_tmp16B[1]={&
_tmp16D};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp16C="redefinition of function %s";
_tag_dyneither(_tmp16C,sizeof(char),28);}),_tag_dyneither(_tmp16B,sizeof(void*),
1));}});return(void*)0;_LL3C: _tmp161=_tmp153.f1;if(_tmp161 <= (void*)1)goto _LL3E;
if(*((int*)_tmp161)!= 1)goto _LL3E;_tmp162=((struct Cyc_Absyn_Funname_b_struct*)
_tmp161)->f1;_tmp163=_tmp153.f2;if(_tmp163 <= (void*)1)goto _LL3E;if(*((int*)
_tmp163)!= 0)goto _LL3E;_tmp164=((struct Cyc_Absyn_Global_b_struct*)_tmp163)->f1;
_LL3D: {int _tmp170;struct _tuple5 _tmp16F=Cyc_Tcdecl_check_var_or_fn_decl(_tmp162->sc,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp162->cached_typ))->v,Cyc_Absyn_empty_tqual(
0),_tmp162->attributes,_tmp164->sc,_tmp164->type,_tmp164->tq,_tmp164->attributes,({
const char*_tmp16E="variable";_tag_dyneither(_tmp16E,sizeof(char),9);}),Cyc_Absynpp_qvar2string(
_tmp162->name),loc,msg);_tmp170=_tmp16F.f2;return _tmp170?b0:(void*)0;}_LL3E:;
_LL3F:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp171=
_cycalloc(sizeof(*_tmp171));_tmp171[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp172;_tmp172.tag=Cyc_Core_Invalid_argument;_tmp172.f1=({const char*_tmp173="Tcdecl::merge_binding";
_tag_dyneither(_tmp173,sizeof(char),22);});_tmp172;});_tmp171;}));_LL33:;}struct
Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*
d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){struct Cyc_Tcdecl_Xdatatypefielddecl _tmp175;struct Cyc_Absyn_Datatypedecl*
_tmp176;struct Cyc_Absyn_Datatypefield*_tmp177;struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp174=d0;_tmp175=*_tmp174;_tmp176=_tmp175.base;_tmp177=_tmp175.field;{struct
Cyc_Tcdecl_Xdatatypefielddecl _tmp179;struct Cyc_Absyn_Datatypedecl*_tmp17A;struct
Cyc_Absyn_Datatypefield*_tmp17B;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp178=d1;
_tmp179=*_tmp178;_tmp17A=_tmp179.base;_tmp17B=_tmp179.field;{struct
_dyneither_ptr _tmp17C=Cyc_Absynpp_qvar2string(_tmp177->name);if(Cyc_Absyn_qvar_cmp(
_tmp176->name,_tmp17A->name)!= 0){({struct Cyc_String_pa_struct _tmp181;_tmp181.tag=
0;_tmp181.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp17C);{struct Cyc_String_pa_struct
_tmp180;_tmp180.tag=0;_tmp180.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp17A->name));{struct Cyc_String_pa_struct _tmp17F;
_tmp17F.tag=0;_tmp17F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp176->name));{void*_tmp17D[3]={& _tmp17F,& _tmp180,& _tmp181};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp17E="@extensible datatype %s and %s have both a field named %s";
_tag_dyneither(_tmp17E,sizeof(char),58);}),_tag_dyneither(_tmp17D,sizeof(void*),
3));}}}});return 0;}if(!Cyc_Tcdecl_check_tvs(_tmp176->tvs,_tmp17A->tvs,({const
char*_tmp182="@extensible datatype";_tag_dyneither(_tmp182,sizeof(char),21);}),
Cyc_Absynpp_qvar2string(_tmp176->name),loc,msg))return 0;{struct Cyc_List_List*
_tmp183=Cyc_Tcdecl_build_tvs_map(_tmp176->tvs,_tmp17A->tvs);struct Cyc_Absyn_Datatypefield*
_tmp184=Cyc_Tcdecl_merge_datatypefield(_tmp177,_tmp17B,_tmp183,({const char*
_tmp186="@extensible datatype field";_tag_dyneither(_tmp186,sizeof(char),27);}),
_tmp17C,msg);if(_tmp184 == 0)return 0;if(_tmp184 == (struct Cyc_Absyn_Datatypefield*)
_tmp177)return(struct Cyc_Tcdecl_Xdatatypefielddecl*)d0;return({struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185->base=_tmp176;_tmp185->field=(struct
Cyc_Absyn_Datatypefield*)_tmp184;_tmp185;});}}}}
