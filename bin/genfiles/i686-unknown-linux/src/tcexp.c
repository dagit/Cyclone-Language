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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);
extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[16];
extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*
x,int i);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*
r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern
struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct
Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,struct Cyc_Position_Segment*);
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);int Cyc_strcmp(struct _dyneither_ptr s1,struct
_dyneither_ptr s2);typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}
__value;}Cyc___mbstate_t;typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;
typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;
struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);struct
Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct
_RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct
Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
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
le;int allow_valueof;};struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*
Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void
Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*,void*);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);int
Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int
Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*
is_dyneither_ptr);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt
Cyc_Tcutil_trk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;
extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);struct _tuple4{struct Cyc_List_List*f1;struct
_RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*
Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);
void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);
void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);
int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(void*
t);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);int
Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple6{
int f1;void*f2;};struct _tuple6 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);struct _tuple7{unsigned int f1;int
f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2);int Cyc_Evexp_okay_szofarg(void*t);void Cyc_Tcstmt_tcStmt(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*);
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct _dyneither_ptr,struct Cyc_Position_Segment*);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);extern int Cyc_Tcexp_in_stmt_exp;
struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{
int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};struct Cyc_CfFlowInfo_UniquePlace{struct Cyc_CfFlowInfo_Place place;struct
Cyc_List_List*path;};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int
f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*
f2;};struct Cyc_CfFlowInfo_LessNumelts_struct{int tag;struct Cyc_Absyn_Vardecl*f1;}
;struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqNumelts_struct{
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
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){({void*_tmp0=0;
Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,
ap),_tag_dyneither(_tmp0,sizeof(void*),0));});if(topt == 0)return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct
_handler_cons _tmp1;_push_handler(& _tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))
_tmp3=1;if(!_tmp3){{struct _RegionHandle*_tmp4=Cyc_Tcenv_get_fnrgn(te);void*_tmp5=
Cyc_Tcenv_lookup_ordinary(_tmp4,te,e->loc,q);void*_tmp6;struct Cyc_Absyn_Enumdecl*
_tmp7;struct Cyc_Absyn_Enumfield*_tmp8;void*_tmp9;struct Cyc_Absyn_Enumfield*_tmpA;
struct Cyc_Absyn_Tuniondecl*_tmpB;struct Cyc_Absyn_Tunionfield*_tmpC;_LL1: if(*((
int*)_tmp5)!= 0)goto _LL3;_tmp6=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp5)->f1;
_LL2:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmpD=_cycalloc(
sizeof(*_tmpD));_tmpD[0]=({struct Cyc_Absyn_Var_e_struct _tmpE;_tmpE.tag=1;_tmpE.f1=
q;_tmpE.f2=(void*)_tmp6;_tmpE;});_tmpD;})));goto _LL0;_LL3: if(*((int*)_tmp5)!= 3)
goto _LL5;_tmp7=((struct Cyc_Tcenv_EnumRes_struct*)_tmp5)->f1;_tmp8=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp5)->f2;_LL4:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Enum_e_struct*
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Absyn_Enum_e_struct _tmp10;
_tmp10.tag=33;_tmp10.f1=q;_tmp10.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7;_tmp10.f3=(
struct Cyc_Absyn_Enumfield*)_tmp8;_tmp10;});_tmpF;})));goto _LL0;_LL5: if(*((int*)
_tmp5)!= 4)goto _LL7;_tmp9=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f1;
_tmpA=((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f2;_LL6:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp11=_cycalloc(sizeof(*_tmp11));
_tmp11[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp12;_tmp12.tag=34;_tmp12.f1=q;
_tmp12.f2=(void*)_tmp9;_tmp12.f3=(struct Cyc_Absyn_Enumfield*)_tmpA;_tmp12;});
_tmp11;})));goto _LL0;_LL7: if(*((int*)_tmp5)!= 2)goto _LL9;_tmpB=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp5)->f1;_tmpC=((struct Cyc_Tcenv_TunionRes_struct*)_tmp5)->f2;_LL8:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Tunion_e_struct*_tmp13=_cycalloc(sizeof(*_tmp13));
_tmp13[0]=({struct Cyc_Absyn_Tunion_e_struct _tmp14;_tmp14.tag=32;_tmp14.f1=0;
_tmp14.f2=_tmpB;_tmp14.f3=_tmpC;_tmp14;});_tmp13;})));goto _LL0;_LL9: if(*((int*)
_tmp5)!= 1)goto _LL0;_LLA:({struct Cyc_String_pa_struct _tmp17;_tmp17.tag=0;_tmp17.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q));{void*
_tmp15[1]={& _tmp17};Cyc_Tcutil_terr(e->loc,({const char*_tmp16="bad occurrence of type name %s";
_tag_dyneither(_tmp16,sizeof(char),31);}),_tag_dyneither(_tmp15,sizeof(void*),1));}});(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmp18=_cycalloc(
sizeof(*_tmp18));_tmp18[0]=({struct Cyc_Absyn_Var_e_struct _tmp19;_tmp19.tag=1;
_tmp19.f1=q;_tmp19.f2=(void*)((void*)0);_tmp19;});_tmp18;})));goto _LL0;_LL0:;};
_pop_handler();}else{void*_tmp2=(void*)_exn_thrown;void*_tmp1B=_tmp2;_LLC: if(
_tmp1B != Cyc_Dict_Absent)goto _LLE;_LLD:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*
_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=({struct Cyc_Absyn_Var_e_struct _tmp1D;
_tmp1D.tag=1;_tmp1D.f1=q;_tmp1D.f2=(void*)((void*)0);_tmp1D;});_tmp1C;})));goto
_LLB;_LLE:;_LLF:(void)_throw(_tmp1B);_LLB:;}}}struct _tuple8{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple8*Cyc_Tcexp_make_struct_arg(struct
Cyc_Absyn_Exp*e){return({struct _tuple8*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->f1=
0;_tmp1E->f2=e;_tmp1E;});}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*
_tmp1F=(void*)e1->r;struct _tuple1*_tmp20;_LL11: if(*((int*)_tmp1F)!= 2)goto _LL13;
_tmp20=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp1F)->f1;_LL12: {struct
_handler_cons _tmp21;_push_handler(& _tmp21);{int _tmp23=0;if(setjmp(_tmp21.handler))
_tmp23=1;if(!_tmp23){{struct _RegionHandle*_tmp24=Cyc_Tcenv_get_fnrgn(te);void*
_tmp25=Cyc_Tcenv_lookup_ordinary(_tmp24,te,e1->loc,_tmp20);void*_tmp26;struct Cyc_Absyn_Tuniondecl*
_tmp27;struct Cyc_Absyn_Tunionfield*_tmp28;struct Cyc_Absyn_Aggrdecl*_tmp29;_LL16:
if(*((int*)_tmp25)!= 0)goto _LL18;_tmp26=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmp25)->f1;_LL17:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp2B;_tmp2B.tag=11;_tmp2B.f1=e1;_tmp2B.f2=es;_tmp2B.f3=0;_tmp2B;});_tmp2A;})));
_npop_handler(0);return;_LL18: if(*((int*)_tmp25)!= 2)goto _LL1A;_tmp27=((struct
Cyc_Tcenv_TunionRes_struct*)_tmp25)->f1;_tmp28=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp25)->f2;_LL19: if(_tmp28->typs == 0)({struct Cyc_String_pa_struct _tmp2E;_tmp2E.tag=
0;_tmp2E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp28->name));{void*_tmp2C[1]={& _tmp2E};Cyc_Tcutil_terr(e->loc,({const char*
_tmp2D="%s is a constant, not a function";_tag_dyneither(_tmp2D,sizeof(char),33);}),
_tag_dyneither(_tmp2C,sizeof(void*),1));}});(void*)(e->r=(void*)((void*)({struct
Cyc_Absyn_Tunion_e_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F[0]=({struct
Cyc_Absyn_Tunion_e_struct _tmp30;_tmp30.tag=32;_tmp30.f1=es;_tmp30.f2=_tmp27;
_tmp30.f3=_tmp28;_tmp30;});_tmp2F;})));_npop_handler(0);return;_LL1A: if(*((int*)
_tmp25)!= 1)goto _LL1C;_tmp29=((struct Cyc_Tcenv_AggrRes_struct*)_tmp25)->f1;_LL1B: {
struct Cyc_List_List*_tmp31=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct
Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,
es);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*_tmp32=
_cycalloc(sizeof(*_tmp32));_tmp32[0]=({struct Cyc_Absyn_Struct_e_struct _tmp33;
_tmp33.tag=30;_tmp33.f1=_tmp29->name;_tmp33.f2=0;_tmp33.f3=_tmp31;_tmp33.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp29;_tmp33;});_tmp32;})));_npop_handler(0);return;}
_LL1C: if(*((int*)_tmp25)!= 4)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp25)
!= 3)goto _LL15;_LL1F:({struct Cyc_String_pa_struct _tmp36;_tmp36.tag=0;_tmp36.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp20));{
void*_tmp34[1]={& _tmp36};Cyc_Tcutil_terr(e->loc,({const char*_tmp35="%s is an enum constructor, not a function";
_tag_dyneither(_tmp35,sizeof(char),42);}),_tag_dyneither(_tmp34,sizeof(void*),1));}});
_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*_tmp22=(void*)
_exn_thrown;void*_tmp38=_tmp22;_LL21: if(_tmp38 != Cyc_Dict_Absent)goto _LL23;_LL22:(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp39=_cycalloc(
sizeof(*_tmp39));_tmp39[0]=({struct Cyc_Absyn_FnCall_e_struct _tmp3A;_tmp3A.tag=11;
_tmp3A.f1=e1;_tmp3A.f2=es;_tmp3A.f3=0;_tmp3A;});_tmp39;})));return;_LL23:;_LL24:(
void)_throw(_tmp38);_LL20:;}}}_LL13:;_LL14:(void*)(e->r=(void*)((void*)({struct
Cyc_Absyn_FnCall_e_struct*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B[0]=({struct
Cyc_Absyn_FnCall_e_struct _tmp3C;_tmp3C.tag=11;_tmp3C.f1=e1;_tmp3C.f2=es;_tmp3C.f3=
0;_tmp3C;});_tmp3B;})));return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des){if(topt == 0){(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_Absyn_Array_e_struct
_tmp3E;_tmp3E.tag=28;_tmp3E.f1=des;_tmp3E;});_tmp3D;})));return;}{void*t=*topt;
void*_tmp3F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp40;union Cyc_Absyn_AggrInfoU_union
_tmp41;struct Cyc_Absyn_ArrayInfo _tmp42;void*_tmp43;struct Cyc_Absyn_Tqual _tmp44;
_LL26: if(_tmp3F <= (void*)4)goto _LL2C;if(*((int*)_tmp3F)!= 10)goto _LL28;_tmp40=((
struct Cyc_Absyn_AggrType_struct*)_tmp3F)->f1;_tmp41=_tmp40.aggr_info;_LL27:{
union Cyc_Absyn_AggrInfoU_union _tmp45=_tmp41;struct Cyc_Absyn_Aggrdecl**_tmp46;
struct Cyc_Absyn_Aggrdecl*_tmp47;_LL2F: if((_tmp45.UnknownAggr).tag != 0)goto _LL31;
_LL30:({void*_tmp48=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp49="struct type not properly set";
_tag_dyneither(_tmp49,sizeof(char),29);}),_tag_dyneither(_tmp48,sizeof(void*),0));});
_LL31: if((_tmp45.KnownAggr).tag != 1)goto _LL2E;_tmp46=(_tmp45.KnownAggr).f1;
_tmp47=*_tmp46;_LL32:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp4B;_tmp4B.tag=30;_tmp4B.f1=_tmp47->name;_tmp4B.f2=0;_tmp4B.f3=des;_tmp4B.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp47;_tmp4B;});_tmp4A;})));_LL2E:;}goto _LL25;_LL28:
if(*((int*)_tmp3F)!= 7)goto _LL2A;_tmp42=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3F)->f1;_tmp43=(void*)_tmp42.elt_type;_tmp44=_tmp42.tq;_LL29:(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp4C=_cycalloc(sizeof(*_tmp4C));
_tmp4C[0]=({struct Cyc_Absyn_Array_e_struct _tmp4D;_tmp4D.tag=28;_tmp4D.f1=des;
_tmp4D;});_tmp4C;})));goto _LL25;_LL2A: if(*((int*)_tmp3F)!= 11)goto _LL2C;_LL2B:(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_AnonStruct_e_struct*_tmp4E=
_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=({struct Cyc_Absyn_AnonStruct_e_struct _tmp4F;
_tmp4F.tag=31;_tmp4F.f1=(void*)t;_tmp4F.f2=des;_tmp4F;});_tmp4E;})));goto _LL25;
_LL2C:;_LL2D:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp50=
_cycalloc(sizeof(*_tmp50));_tmp50[0]=({struct Cyc_Absyn_Array_e_struct _tmp51;
_tmp51.tag=28;_tmp51.f1=des;_tmp51;});_tmp50;})));goto _LL25;_LL25:;}}static void
Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e);static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(e);Cyc_Tcexp_tcExp(te,(
void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(te,e))({struct Cyc_String_pa_struct
_tmp55;_tmp55.tag=0;_tmp55.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmp54;_tmp54.tag=0;_tmp54.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
msg_part);{void*_tmp52[2]={& _tmp54,& _tmp55};Cyc_Tcutil_terr(e->loc,({const char*
_tmp53="test of %s has type %s instead of integral or * type";_tag_dyneither(
_tmp53,sizeof(char),53);}),_tag_dyneither(_tmp52,sizeof(void*),2));}}});}static
void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,union Cyc_Absyn_Cnst_union*c,struct Cyc_Absyn_Exp*e){void*t;{union Cyc_Absyn_Cnst_union
_tmp56=*((union Cyc_Absyn_Cnst_union*)_check_null(c));void*_tmp57;void*_tmp58;
void*_tmp59;void*_tmp5A;void*_tmp5B;void*_tmp5C;int _tmp5D;struct _dyneither_ptr
_tmp5E;_LL34: if((_tmp56.Char_c).tag != 0)goto _LL36;_tmp57=(_tmp56.Char_c).f1;if((
int)_tmp57 != 0)goto _LL36;_LL35: t=Cyc_Absyn_schar_typ;goto _LL33;_LL36: if((_tmp56.Char_c).tag
!= 0)goto _LL38;_tmp58=(_tmp56.Char_c).f1;if((int)_tmp58 != 1)goto _LL38;_LL37: t=
Cyc_Absyn_uchar_typ;goto _LL33;_LL38: if((_tmp56.Char_c).tag != 0)goto _LL3A;_tmp59=(
_tmp56.Char_c).f1;if((int)_tmp59 != 2)goto _LL3A;_LL39: t=Cyc_Absyn_char_typ;goto
_LL33;_LL3A: if((_tmp56.Short_c).tag != 1)goto _LL3C;_tmp5A=(_tmp56.Short_c).f1;
_LL3B: t=_tmp5A == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL33;
_LL3C: if((_tmp56.LongLong_c).tag != 3)goto _LL3E;_tmp5B=(_tmp56.LongLong_c).f1;
_LL3D: t=_tmp5B == (void*)1?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto
_LL33;_LL3E: if((_tmp56.Float_c).tag != 4)goto _LL40;_LL3F: t=Cyc_Absyn_float_typ;
goto _LL33;_LL40: if((_tmp56.Int_c).tag != 2)goto _LL42;_tmp5C=(_tmp56.Int_c).f1;
_tmp5D=(_tmp56.Int_c).f2;_LL41: if(topt == 0)t=_tmp5C == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;else{void*_tmp5F=Cyc_Tcutil_compress(*topt);void*_tmp60;void*
_tmp61;void*_tmp62;void*_tmp63;void*_tmp64;void*_tmp65;void*_tmp66;void*_tmp67;
void*_tmp68;_LL47: if(_tmp5F <= (void*)4)goto _LL53;if(*((int*)_tmp5F)!= 5)goto
_LL49;_tmp60=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F)->f1;_tmp61=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5F)->f2;if((int)_tmp61 != 0)goto _LL49;_LL48:{
void*_tmp69=_tmp60;_LL56: if((int)_tmp69 != 1)goto _LL58;_LL57: t=Cyc_Absyn_uchar_typ;
goto _LL55;_LL58: if((int)_tmp69 != 0)goto _LL5A;_LL59: t=Cyc_Absyn_schar_typ;goto
_LL55;_LL5A: if((int)_tmp69 != 2)goto _LL55;_LL5B: t=Cyc_Absyn_char_typ;goto _LL55;
_LL55:;}*c=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp6A;(_tmp6A.Char_c).tag=
0;(_tmp6A.Char_c).f1=(void*)_tmp60;(_tmp6A.Char_c).f2=(char)_tmp5D;_tmp6A;});
goto _LL46;_LL49: if(*((int*)_tmp5F)!= 5)goto _LL4B;_tmp62=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f1;_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F)->f2;if((int)
_tmp63 != 1)goto _LL4B;_LL4A: t=_tmp62 == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;*
c=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp6B;(_tmp6B.Short_c).tag=
1;(_tmp6B.Short_c).f1=(void*)_tmp62;(_tmp6B.Short_c).f2=(short)_tmp5D;_tmp6B;});
goto _LL46;_LL4B: if(*((int*)_tmp5F)!= 5)goto _LL4D;_tmp64=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f1;_tmp65=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F)->f2;if((int)
_tmp65 != 2)goto _LL4D;_LL4C: t=_tmp64 == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL4D: if(*((int*)_tmp5F)!= 5)goto _LL4F;_tmp66=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f1;_tmp67=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F)->f2;if((int)
_tmp67 != 3)goto _LL4F;_LL4E: t=_tmp66 == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL4F: if(*((int*)_tmp5F)!= 4)goto _LL51;if(!(_tmp5D == 0))goto _LL51;
_LL50:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp6C=
_cycalloc(sizeof(*_tmp6C));_tmp6C[0]=({struct Cyc_Absyn_Const_e_struct _tmp6D;
_tmp6D.tag=0;_tmp6D.f1=(union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp6E;(_tmp6E.Null_c).tag=6;_tmp6E;});_tmp6D;});_tmp6C;})));{struct Cyc_List_List*
_tmp6F=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70[0]=({struct Cyc_Absyn_PointerType_struct
_tmp71;_tmp71.tag=4;_tmp71.f1=({struct Cyc_Absyn_PtrInfo _tmp72;_tmp72.elt_typ=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*
_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->v=_tmp6F;_tmp75;}));_tmp72.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp72.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp73;_tmp73.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp74=_cycalloc(
sizeof(*_tmp74));_tmp74->v=_tmp6F;_tmp74;}));_tmp73.nullable=Cyc_Absyn_true_conref;
_tmp73.bounds=Cyc_Absyn_empty_conref();_tmp73.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp73.ptrloc=0;_tmp73;});_tmp72;});_tmp71;});_tmp70;});
goto _LL46;}_LL51: if(*((int*)_tmp5F)!= 18)goto _LL53;_tmp68=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp5F)->f1;_LL52: {struct Cyc_Absyn_ValueofType_struct*_tmp76=({struct Cyc_Absyn_ValueofType_struct*
_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D[0]=({struct Cyc_Absyn_ValueofType_struct
_tmp7E;_tmp7E.tag=17;_tmp7E.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp5D,0);_tmp7E;});
_tmp7D;});if(!Cyc_Tcutil_unify(_tmp68,(void*)_tmp76)){({struct Cyc_String_pa_struct
_tmp7A;_tmp7A.tag=0;_tmp7A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_tmp76));{struct Cyc_String_pa_struct _tmp79;_tmp79.tag=0;_tmp79.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp68));{void*
_tmp77[2]={& _tmp79,& _tmp7A};Cyc_Tcutil_terr(loc,({const char*_tmp78="expecting %s but found %s";
_tag_dyneither(_tmp78,sizeof(char),26);}),_tag_dyneither(_tmp77,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}t=(void*)({struct Cyc_Absyn_TagType_struct*_tmp7B=
_cycalloc(sizeof(*_tmp7B));_tmp7B[0]=({struct Cyc_Absyn_TagType_struct _tmp7C;
_tmp7C.tag=18;_tmp7C.f1=(void*)((void*)_tmp76);_tmp7C;});_tmp7B;});goto _LL46;}
_LL53:;_LL54: t=_tmp5C == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL46;
_LL46:;}goto _LL33;_LL42: if((_tmp56.String_c).tag != 5)goto _LL44;_tmp5E=(_tmp56.String_c).f1;
_LL43: {int len=(int)_get_dyneither_size(_tmp5E,sizeof(char));struct Cyc_Absyn_Exp*
elen=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp89;(_tmp89.Int_c).tag=2;(_tmp89.Int_c).f1=(void*)((void*)1);(_tmp89.Int_c).f2=
len;_tmp89;}),loc);elen->topt=({struct Cyc_Core_Opt*_tmp7F=_cycalloc(sizeof(*
_tmp7F));_tmp7F->v=(void*)Cyc_Absyn_uint_typ;_tmp7F;});t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Absyn_const_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp80=
_cycalloc(sizeof(*_tmp80));_tmp80[0]=({struct Cyc_Absyn_Upper_b_struct _tmp81;
_tmp81.tag=0;_tmp81.f1=elen;_tmp81;});_tmp80;}),Cyc_Absyn_true_conref);if(topt != 
0){void*_tmp82=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp83;struct
Cyc_Absyn_Tqual _tmp84;_LL5D: if(_tmp82 <= (void*)4)goto _LL61;if(*((int*)_tmp82)!= 
7)goto _LL5F;_tmp83=((struct Cyc_Absyn_ArrayType_struct*)_tmp82)->f1;_tmp84=_tmp83.tq;
_LL5E: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmp84,(struct Cyc_Absyn_Exp*)
elen,((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),0);_LL5F: if(*((int*)
_tmp82)!= 4)goto _LL61;_LL60: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp85=_cycalloc(sizeof(*_tmp85));
_tmp85->v=(void*)t;_tmp85;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);t=*
topt;}else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp87;_tmp87.tag=0;_tmp87.f1=elen;_tmp87;});
_tmp86;}),Cyc_Absyn_true_conref);if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp88=_cycalloc(sizeof(*_tmp88));
_tmp88->v=(void*)t;_tmp88;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);t=*
topt;}}goto _LL5C;_LL61:;_LL62: goto _LL5C;_LL5C:;}return t;}_LL44: if((_tmp56.Null_c).tag
!= 6)goto _LL33;_LL45: {struct Cyc_List_List*_tmp8A=Cyc_Tcenv_lookup_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));
_tmp8B[0]=({struct Cyc_Absyn_PointerType_struct _tmp8C;_tmp8C.tag=4;_tmp8C.f1=({
struct Cyc_Absyn_PtrInfo _tmp8D;_tmp8D.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp90=_cycalloc(sizeof(*
_tmp90));_tmp90->v=_tmp8A;_tmp90;}));_tmp8D.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp8D.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp8E;_tmp8E.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp8F=_cycalloc(
sizeof(*_tmp8F));_tmp8F->v=_tmp8A;_tmp8F;}));_tmp8E.nullable=Cyc_Absyn_true_conref;
_tmp8E.bounds=Cyc_Absyn_empty_conref();_tmp8E.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp8E.ptrloc=0;_tmp8E;});_tmp8D;});_tmp8C;});_tmp8B;});
goto _LL33;}_LL33:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q,void*b){void*_tmp91=b;struct Cyc_Absyn_Vardecl*
_tmp92;struct Cyc_Absyn_Fndecl*_tmp93;struct Cyc_Absyn_Vardecl*_tmp94;struct Cyc_Absyn_Vardecl*
_tmp95;struct Cyc_Absyn_Vardecl*_tmp96;_LL64: if((int)_tmp91 != 0)goto _LL66;_LL65:
return({struct Cyc_String_pa_struct _tmp99;_tmp99.tag=0;_tmp99.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q));{void*_tmp97[1]={&
_tmp99};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp98="undeclared identifier: %s";
_tag_dyneither(_tmp98,sizeof(char),26);}),_tag_dyneither(_tmp97,sizeof(void*),1));}});
_LL66: if(_tmp91 <= (void*)1)goto _LL68;if(*((int*)_tmp91)!= 0)goto _LL68;_tmp92=((
struct Cyc_Absyn_Global_b_struct*)_tmp91)->f1;_LL67:*q=*_tmp92->name;return(void*)
_tmp92->type;_LL68: if(_tmp91 <= (void*)1)goto _LL6A;if(*((int*)_tmp91)!= 1)goto
_LL6A;_tmp93=((struct Cyc_Absyn_Funname_b_struct*)_tmp91)->f1;_LL69:*q=*_tmp93->name;
return Cyc_Tcutil_fndecl2typ(_tmp93);_LL6A: if(_tmp91 <= (void*)1)goto _LL6C;if(*((
int*)_tmp91)!= 4)goto _LL6C;_tmp94=((struct Cyc_Absyn_Pat_b_struct*)_tmp91)->f1;
_LL6B: _tmp95=_tmp94;goto _LL6D;_LL6C: if(_tmp91 <= (void*)1)goto _LL6E;if(*((int*)
_tmp91)!= 3)goto _LL6E;_tmp95=((struct Cyc_Absyn_Local_b_struct*)_tmp91)->f1;_LL6D:
_tmp96=_tmp95;goto _LL6F;_LL6E: if(_tmp91 <= (void*)1)goto _LL63;if(*((int*)_tmp91)
!= 2)goto _LL63;_tmp96=((struct Cyc_Absyn_Param_b_struct*)_tmp91)->f1;_LL6F:(*q).f1=(
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp9A;(_tmp9A.Loc_n).tag=
0;_tmp9A;});return(void*)_tmp96->type;_LL63:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,
struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*)){struct
Cyc_List_List*desc_types;{void*_tmp9B=(void*)fmt->r;union Cyc_Absyn_Cnst_union
_tmp9C;struct _dyneither_ptr _tmp9D;struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_Exp
_tmp9F;void*_tmpA0;union Cyc_Absyn_Cnst_union _tmpA1;struct _dyneither_ptr _tmpA2;
_LL71: if(*((int*)_tmp9B)!= 0)goto _LL73;_tmp9C=((struct Cyc_Absyn_Const_e_struct*)
_tmp9B)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp9B)->f1).String_c).tag != 
5)goto _LL73;_tmp9D=(_tmp9C.String_c).f1;_LL72: _tmpA2=_tmp9D;goto _LL74;_LL73: if(*((
int*)_tmp9B)!= 15)goto _LL75;_tmp9E=((struct Cyc_Absyn_Cast_e_struct*)_tmp9B)->f2;
_tmp9F=*_tmp9E;_tmpA0=(void*)_tmp9F.r;if(*((int*)_tmpA0)!= 0)goto _LL75;_tmpA1=((
struct Cyc_Absyn_Const_e_struct*)_tmpA0)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpA0)->f1).String_c).tag != 5)goto _LL75;_tmpA2=(_tmpA1.String_c).f1;_LL74:
desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmpA2,fmt->loc);goto _LL70;
_LL75:;_LL76: if(opt_args != 0){struct Cyc_List_List*_tmpA3=(struct Cyc_List_List*)
opt_args->v;for(0;_tmpA3 != 0;_tmpA3=_tmpA3->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmpA3->hd);{struct _RegionHandle*_tmpA4=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpA4,(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)_tmpA3->hd)->topt))->v)
 && !Cyc_Tcutil_is_noalias_path(_tmpA4,(struct Cyc_Absyn_Exp*)_tmpA3->hd))({void*
_tmpA5=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpA3->hd)->loc,({const char*
_tmpA6="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmpA6,
sizeof(char),49);}),_tag_dyneither(_tmpA5,sizeof(void*),0));});}}}return;_LL70:;}
if(opt_args != 0){struct Cyc_List_List*_tmpA7=(struct Cyc_List_List*)opt_args->v;
for(0;desc_types != 0  && _tmpA7 != 0;(desc_types=desc_types->tl,_tmpA7=_tmpA7->tl)){
void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpA7->hd;
Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_String_pa_struct
_tmpAB;_tmpAB.tag=0;_tmpAB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmpAA;_tmpAA.tag=0;_tmpAA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmpA8[2]={& _tmpAA,& _tmpAB};Cyc_Tcutil_terr(e->loc,({const char*_tmpA9="descriptor has type \n%s\n but argument has type \n%s";
_tag_dyneither(_tmpA9,sizeof(char),51);}),_tag_dyneither(_tmpA8,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}{struct _RegionHandle*_tmpAC=Cyc_Tcenv_get_fnrgn(te);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmpAC,t) && !Cyc_Tcutil_is_noalias_path(
_tmpAC,e))({void*_tmpAD=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpA7->hd)->loc,({
const char*_tmpAE="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmpAE,sizeof(char),49);}),_tag_dyneither(_tmpAD,sizeof(void*),0));});}}
if(desc_types != 0)({void*_tmpAF=0;Cyc_Tcutil_terr(fmt->loc,({const char*_tmpB0="too few arguments";
_tag_dyneither(_tmpB0,sizeof(char),18);}),_tag_dyneither(_tmpAF,sizeof(void*),0));});
if(_tmpA7 != 0)({void*_tmpB1=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpA7->hd)->loc,({
const char*_tmpB2="too many arguments";_tag_dyneither(_tmpB2,sizeof(char),19);}),
_tag_dyneither(_tmpB1,sizeof(void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmpB3=p;_LL78: if((int)_tmpB3 != 0)goto _LL7A;_LL79:
goto _LL7B;_LL7A: if((int)_tmpB3 != 2)goto _LL7C;_LL7B: if(!Cyc_Tcutil_is_numeric(e))({
struct Cyc_String_pa_struct _tmpB6;_tmpB6.tag=0;_tmpB6.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpB4[1]={& _tmpB6};Cyc_Tcutil_terr(
loc,({const char*_tmpB5="expecting arithmetic type but found %s";_tag_dyneither(
_tmpB5,sizeof(char),39);}),_tag_dyneither(_tmpB4,sizeof(void*),1));}});return(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;_LL7C: if((int)_tmpB3 != 11)
goto _LL7E;_LL7D: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(
te,e))({struct Cyc_String_pa_struct _tmpB9;_tmpB9.tag=0;_tmpB9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpB7[1]={&
_tmpB9};Cyc_Tcutil_terr(loc,({const char*_tmpB8="expecting integral or * type but found %s";
_tag_dyneither(_tmpB8,sizeof(char),42);}),_tag_dyneither(_tmpB7,sizeof(void*),1));}});
return Cyc_Absyn_sint_typ;_LL7E: if((int)_tmpB3 != 12)goto _LL80;_LL7F: if(!Cyc_Tcutil_is_integral(
e))({struct Cyc_String_pa_struct _tmpBC;_tmpBC.tag=0;_tmpBC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmpBA[1]={&
_tmpBC};Cyc_Tcutil_terr(loc,({const char*_tmpBB="expecting integral type but found %s";
_tag_dyneither(_tmpBB,sizeof(char),37);}),_tag_dyneither(_tmpBA,sizeof(void*),1));}});
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;_LL80: if((int)_tmpB3
!= 19)goto _LL82;_LL81:{void*_tmpBD=t;struct Cyc_Absyn_PtrInfo _tmpBE;struct Cyc_Absyn_PtrAtts
_tmpBF;struct Cyc_Absyn_Conref*_tmpC0;_LL85: if(_tmpBD <= (void*)4)goto _LL89;if(*((
int*)_tmpBD)!= 7)goto _LL87;_LL86: goto _LL84;_LL87: if(*((int*)_tmpBD)!= 4)goto
_LL89;_tmpBE=((struct Cyc_Absyn_PointerType_struct*)_tmpBD)->f1;_tmpBF=_tmpBE.ptr_atts;
_tmpC0=_tmpBF.bounds;_LL88:{union Cyc_Absyn_Constraint_union _tmpC1=(Cyc_Absyn_compress_conref(
_tmpC0))->v;void*_tmpC2;void*_tmpC3;_LL8C: if((_tmpC1.Eq_constr).tag != 0)goto
_LL8E;_tmpC2=(_tmpC1.Eq_constr).f1;if((int)_tmpC2 != 0)goto _LL8E;_LL8D: goto _LL8F;
_LL8E: if((_tmpC1.Eq_constr).tag != 0)goto _LL90;_tmpC3=(_tmpC1.Eq_constr).f1;if(
_tmpC3 <= (void*)1)goto _LL90;if(*((int*)_tmpC3)!= 0)goto _LL90;_LL8F: goto _LL8B;
_LL90:;_LL91:({void*_tmpC4=0;Cyc_Tcutil_terr(loc,({const char*_tmpC5="can't apply numelts to pointer/array of abstract length";
_tag_dyneither(_tmpC5,sizeof(char),56);}),_tag_dyneither(_tmpC4,sizeof(void*),0));});
_LL8B:;}goto _LL84;_LL89:;_LL8A:({struct Cyc_String_pa_struct _tmpC8;_tmpC8.tag=0;
_tmpC8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{
void*_tmpC6[1]={& _tmpC8};Cyc_Tcutil_terr(loc,({const char*_tmpC7="numelts requires pointer or array type, not %s";
_tag_dyneither(_tmpC7,sizeof(char),47);}),_tag_dyneither(_tmpC6,sizeof(void*),1));}});
_LL84:;}return Cyc_Absyn_uint_typ;_LL82:;_LL83:({void*_tmpC9=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpCA="Non-unary primop";
_tag_dyneither(_tmpCA,sizeof(char),17);}),_tag_dyneither(_tmpC9,sizeof(void*),0));});
_LL77:;}static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({
struct Cyc_String_pa_struct _tmpCD;_tmpCD.tag=0;_tmpCD.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v));{void*_tmpCB[1]={& _tmpCD};Cyc_Tcutil_terr(e1->loc,({
const char*_tmpCC="type %s cannot be used here";_tag_dyneither(_tmpCC,sizeof(char),
28);}),_tag_dyneither(_tmpCB,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}if(!checker(e2)){({struct Cyc_String_pa_struct _tmpD0;_tmpD0.tag=0;_tmpD0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{void*_tmpCE[1]={& _tmpD0};Cyc_Tcutil_terr(
e2->loc,({const char*_tmpCF="type %s cannot be used here";_tag_dyneither(_tmpCF,
sizeof(char),28);}),_tag_dyneither(_tmpCE,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));}{void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);return Cyc_Tcutil_max_arithmetic_type(
t1,t2);}}static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);void*_tmpD1=t1;struct Cyc_Absyn_PtrInfo _tmpD2;void*
_tmpD3;struct Cyc_Absyn_Tqual _tmpD4;struct Cyc_Absyn_PtrAtts _tmpD5;void*_tmpD6;
struct Cyc_Absyn_Conref*_tmpD7;struct Cyc_Absyn_Conref*_tmpD8;struct Cyc_Absyn_Conref*
_tmpD9;_LL93: if(_tmpD1 <= (void*)4)goto _LL95;if(*((int*)_tmpD1)!= 4)goto _LL95;
_tmpD2=((struct Cyc_Absyn_PointerType_struct*)_tmpD1)->f1;_tmpD3=(void*)_tmpD2.elt_typ;
_tmpD4=_tmpD2.elt_tq;_tmpD5=_tmpD2.ptr_atts;_tmpD6=(void*)_tmpD5.rgn;_tmpD7=
_tmpD5.nullable;_tmpD8=_tmpD5.bounds;_tmpD9=_tmpD5.zero_term;_LL94: if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmpD3),(void*)1))({void*_tmpDA=0;Cyc_Tcutil_terr(e1->loc,({
const char*_tmpDB="can't perform arithmetic on abstract pointer type";
_tag_dyneither(_tmpDB,sizeof(char),50);}),_tag_dyneither(_tmpDA,sizeof(void*),0));});
if(Cyc_Tcutil_is_noalias_pointer(t1))({void*_tmpDC=0;Cyc_Tcutil_terr(e1->loc,({
const char*_tmpDD="can't perform arithmetic on non-aliasing pointer type";
_tag_dyneither(_tmpDD,sizeof(char),54);}),_tag_dyneither(_tmpDC,sizeof(void*),0));});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2))({struct Cyc_String_pa_struct _tmpE0;_tmpE0.tag=
0;_tmpE0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2));{void*_tmpDE[1]={& _tmpE0};Cyc_Tcutil_terr(e2->loc,({const char*_tmpDF="expecting int but found %s";
_tag_dyneither(_tmpDF,sizeof(char),27);}),_tag_dyneither(_tmpDE,sizeof(void*),1));}});
_tmpD8=Cyc_Absyn_compress_conref(_tmpD8);{union Cyc_Absyn_Constraint_union _tmpE1=
_tmpD8->v;void*_tmpE2;void*_tmpE3;struct Cyc_Absyn_Exp*_tmpE4;_LL98: if((_tmpE1.Eq_constr).tag
!= 0)goto _LL9A;_tmpE2=(_tmpE1.Eq_constr).f1;if((int)_tmpE2 != 0)goto _LL9A;_LL99:
return t1;_LL9A: if((_tmpE1.Eq_constr).tag != 0)goto _LL9C;_tmpE3=(_tmpE1.Eq_constr).f1;
if(_tmpE3 <= (void*)1)goto _LL9C;if(*((int*)_tmpE3)!= 0)goto _LL9C;_tmpE4=((struct
Cyc_Absyn_Upper_b_struct*)_tmpE3)->f1;_LL9B: if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpD9))({void*_tmpE5=0;Cyc_Tcutil_warn(e1->loc,({
const char*_tmpE6="pointer arithmetic on thin, zero-terminated pointer may be expensive.";
_tag_dyneither(_tmpE6,sizeof(char),70);}),_tag_dyneither(_tmpE5,sizeof(void*),0));});{
struct Cyc_Absyn_PointerType_struct*_tmpE7=({struct Cyc_Absyn_PointerType_struct*
_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8[0]=({struct Cyc_Absyn_PointerType_struct
_tmpE9;_tmpE9.tag=4;_tmpE9.f1=({struct Cyc_Absyn_PtrInfo _tmpEA;_tmpEA.elt_typ=(
void*)_tmpD3;_tmpEA.elt_tq=_tmpD4;_tmpEA.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpEB;_tmpEB.rgn=(void*)_tmpD6;_tmpEB.nullable=Cyc_Absyn_true_conref;_tmpEB.bounds=
Cyc_Absyn_bounds_dyneither_conref;_tmpEB.zero_term=_tmpD9;_tmpEB.ptrloc=0;_tmpEB;});
_tmpEA;});_tmpE9;});_tmpE8;});Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmpE7,(
void*)3);return(void*)_tmpE7;}_LL9C:;_LL9D: _tmpD8->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmpEC;(_tmpEC.Eq_constr).tag=0;(_tmpEC.Eq_constr).f1=(
void*)((void*)0);_tmpEC;});return t1;_LL97:;}_LL95:;_LL96: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LL92:;}static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;
void*t1_elt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){if(!Cyc_Tcutil_unify(t1,t2)){({struct Cyc_String_pa_struct _tmpF0;_tmpF0.tag=
0;_tmpF0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{struct Cyc_String_pa_struct
_tmpEF;_tmpEF.tag=0;_tmpEF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmpED[2]={& _tmpEF,&
_tmpF0};Cyc_Tcutil_terr(e1->loc,({const char*_tmpEE="pointer arithmetic on values of different types (%s != %s)";
_tag_dyneither(_tmpEE,sizeof(char),59);}),_tag_dyneither(_tmpED,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),(void*)1))({void*_tmpF1=0;Cyc_Tcutil_terr(e1->loc,({
const char*_tmpF2="can't perform arithmetic on abstract pointer type";
_tag_dyneither(_tmpF2,sizeof(char),50);}),_tag_dyneither(_tmpF1,sizeof(void*),0));});
if(Cyc_Tcutil_is_noalias_pointer(t1))({void*_tmpF3=0;Cyc_Tcutil_terr(e1->loc,({
const char*_tmpF4="can't perform arithmetic on non-aliasing pointer type";
_tag_dyneither(_tmpF4,sizeof(char),54);}),_tag_dyneither(_tmpF3,sizeof(void*),0));});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){({struct Cyc_String_pa_struct _tmpF8;_tmpF8.tag=
0;_tmpF8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2));{struct Cyc_String_pa_struct _tmpF7;_tmpF7.tag=0;_tmpF7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{void*_tmpF5[2]={&
_tmpF7,& _tmpF8};Cyc_Tcutil_terr(e2->loc,({const char*_tmpF6="expecting either %s or int but found %s";
_tag_dyneither(_tmpF6,sizeof(char),40);}),_tag_dyneither(_tmpF5,sizeof(void*),2));}}});
Cyc_Tcutil_explain_failure();}return t1;}}if(Cyc_Tcutil_is_pointer_type(t1))Cyc_Tcutil_unchecked_cast(
te,e1,Cyc_Absyn_sint_typ,(void*)3);if(Cyc_Tcutil_is_pointer_type(t2))Cyc_Tcutil_unchecked_cast(
te,e2,Cyc_Absyn_sint_typ,(void*)3);return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(
e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(e1_is_num  && e2_is_num)return
Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_typ_kind(t1)== (void*)2  || Cyc_Tcutil_unify(
t1,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,Cyc_Tcenv_lookup_opt_type_vars(
te)))){if(Cyc_Tcutil_unify(t1,t2))return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(
te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(te,e2,t1,(void*)3);return Cyc_Absyn_sint_typ;}
else{if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,
e1,t2,(void*)3);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_zero_to_null(te,t2,
e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))return Cyc_Absyn_sint_typ;else{goto
pointer_cmp;}}}}}else{pointer_cmp: {struct _tuple0 _tmpFA=({struct _tuple0 _tmpF9;
_tmpF9.f1=Cyc_Tcutil_compress(t1);_tmpF9.f2=Cyc_Tcutil_compress(t2);_tmpF9;});
void*_tmpFB;struct Cyc_Absyn_PtrInfo _tmpFC;void*_tmpFD;void*_tmpFE;struct Cyc_Absyn_PtrInfo
_tmpFF;void*_tmp100;_LL9F: _tmpFB=_tmpFA.f1;if(_tmpFB <= (void*)4)goto _LLA1;if(*((
int*)_tmpFB)!= 4)goto _LLA1;_tmpFC=((struct Cyc_Absyn_PointerType_struct*)_tmpFB)->f1;
_tmpFD=(void*)_tmpFC.elt_typ;_tmpFE=_tmpFA.f2;if(_tmpFE <= (void*)4)goto _LLA1;if(*((
int*)_tmpFE)!= 4)goto _LLA1;_tmpFF=((struct Cyc_Absyn_PointerType_struct*)_tmpFE)->f1;
_tmp100=(void*)_tmpFF.elt_typ;_LLA0: if(Cyc_Tcutil_unify(_tmpFD,_tmp100))return
Cyc_Absyn_sint_typ;goto _LL9E;_LLA1:;_LLA2: goto _LL9E;_LL9E:;}({struct Cyc_String_pa_struct
_tmp104;_tmp104.tag=0;_tmp104.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp103;_tmp103.tag=0;
_tmp103.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp101[2]={& _tmp103,& _tmp104};Cyc_Tcutil_terr(loc,({const char*
_tmp102="comparison not allowed between %s and %s";_tag_dyneither(_tmp102,
sizeof(char),41);}),_tag_dyneither(_tmp101,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}static void*Cyc_Tcexp_tcBinPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp105=p;_LLA4: if((int)_tmp105 != 
0)goto _LLA6;_LLA5: return Cyc_Tcexp_tcPlus(te,e1,e2);_LLA6: if((int)_tmp105 != 2)
goto _LLA8;_LLA7: return Cyc_Tcexp_tcMinus(te,e1,e2);_LLA8: if((int)_tmp105 != 1)goto
_LLAA;_LLA9: goto _LLAB;_LLAA: if((int)_tmp105 != 3)goto _LLAC;_LLAB: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LLAC: if((int)_tmp105 != 4)goto _LLAE;_LLAD: goto
_LLAF;_LLAE: if((int)_tmp105 != 13)goto _LLB0;_LLAF: goto _LLB1;_LLB0: if((int)_tmp105
!= 14)goto _LLB2;_LLB1: goto _LLB3;_LLB2: if((int)_tmp105 != 15)goto _LLB4;_LLB3: goto
_LLB5;_LLB4: if((int)_tmp105 != 16)goto _LLB6;_LLB5: goto _LLB7;_LLB6: if((int)_tmp105
!= 17)goto _LLB8;_LLB7: goto _LLB9;_LLB8: if((int)_tmp105 != 18)goto _LLBA;_LLB9:
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);_LLBA: if((int)
_tmp105 != 5)goto _LLBC;_LLBB: goto _LLBD;_LLBC: if((int)_tmp105 != 6)goto _LLBE;_LLBD:
goto _LLBF;_LLBE: if((int)_tmp105 != 7)goto _LLC0;_LLBF: goto _LLC1;_LLC0: if((int)
_tmp105 != 8)goto _LLC2;_LLC1: goto _LLC3;_LLC2: if((int)_tmp105 != 9)goto _LLC4;_LLC3:
goto _LLC5;_LLC4: if((int)_tmp105 != 10)goto _LLC6;_LLC5: return Cyc_Tcexp_tcAnyBinop(
te,loc,e1,e2);_LLC6:;_LLC7:({void*_tmp106=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp107="bad binary primop";
_tag_dyneither(_tmp107,sizeof(char),18);}),_tag_dyneither(_tmp106,sizeof(void*),
0));});_LLA3:;}static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_List_List*es){if(p == (void*)2  && ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(es)== 1)return Cyc_Tcexp_tcExp(te,topt,(struct
Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLC8: return({void*_tmp108=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp109="primitive operator has 0 arguments";_tag_dyneither(_tmp109,sizeof(char),
35);}),_tag_dyneither(_tmp108,sizeof(void*),0));});case 1: _LLC9: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLCA: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLCB: return({void*_tmp10A=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp10B="primitive operator has > 2 arguments";
_tag_dyneither(_tmp10B,sizeof(char),37);}),_tag_dyneither(_tmp10A,sizeof(void*),
0));});}return t;}}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmp10C=(void*)e->r;void*
_tmp10D;struct Cyc_Absyn_Vardecl*_tmp10E;void*_tmp10F;struct Cyc_Absyn_Vardecl*
_tmp110;void*_tmp111;struct Cyc_Absyn_Vardecl*_tmp112;void*_tmp113;struct Cyc_Absyn_Vardecl*
_tmp114;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*
_tmp117;struct _dyneither_ptr*_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct
_dyneither_ptr*_tmp11A;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11C;
struct Cyc_Absyn_Exp*_tmp11D;_LLCE: if(*((int*)_tmp10C)!= 1)goto _LLD0;_tmp10D=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10C)->f2;if(_tmp10D <= (void*)1)goto
_LLD0;if(*((int*)_tmp10D)!= 2)goto _LLD0;_tmp10E=((struct Cyc_Absyn_Param_b_struct*)
_tmp10D)->f1;_LLCF: _tmp110=_tmp10E;goto _LLD1;_LLD0: if(*((int*)_tmp10C)!= 1)goto
_LLD2;_tmp10F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10C)->f2;if(_tmp10F <= (
void*)1)goto _LLD2;if(*((int*)_tmp10F)!= 3)goto _LLD2;_tmp110=((struct Cyc_Absyn_Local_b_struct*)
_tmp10F)->f1;_LLD1: _tmp112=_tmp110;goto _LLD3;_LLD2: if(*((int*)_tmp10C)!= 1)goto
_LLD4;_tmp111=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10C)->f2;if(_tmp111 <= (
void*)1)goto _LLD4;if(*((int*)_tmp111)!= 4)goto _LLD4;_tmp112=((struct Cyc_Absyn_Pat_b_struct*)
_tmp111)->f1;_LLD3: _tmp114=_tmp112;goto _LLD5;_LLD4: if(*((int*)_tmp10C)!= 1)goto
_LLD6;_tmp113=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10C)->f2;if(_tmp113 <= (
void*)1)goto _LLD6;if(*((int*)_tmp113)!= 0)goto _LLD6;_tmp114=((struct Cyc_Absyn_Global_b_struct*)
_tmp113)->f1;_LLD5: if(!(_tmp114->tq).real_const)return;goto _LLCD;_LLD6: if(*((int*)
_tmp10C)!= 25)goto _LLD8;_tmp115=((struct Cyc_Absyn_Subscript_e_struct*)_tmp10C)->f1;
_tmp116=((struct Cyc_Absyn_Subscript_e_struct*)_tmp10C)->f2;_LLD7:{void*_tmp11E=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp115->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp11F;struct Cyc_Absyn_Tqual _tmp120;struct Cyc_Absyn_ArrayInfo
_tmp121;struct Cyc_Absyn_Tqual _tmp122;struct Cyc_List_List*_tmp123;_LLE5: if(
_tmp11E <= (void*)4)goto _LLEB;if(*((int*)_tmp11E)!= 4)goto _LLE7;_tmp11F=((struct
Cyc_Absyn_PointerType_struct*)_tmp11E)->f1;_tmp120=_tmp11F.elt_tq;_LLE6: _tmp122=
_tmp120;goto _LLE8;_LLE7: if(*((int*)_tmp11E)!= 7)goto _LLE9;_tmp121=((struct Cyc_Absyn_ArrayType_struct*)
_tmp11E)->f1;_tmp122=_tmp121.tq;_LLE8: if(!_tmp122.real_const)return;goto _LLE4;
_LLE9: if(*((int*)_tmp11E)!= 9)goto _LLEB;_tmp123=((struct Cyc_Absyn_TupleType_struct*)
_tmp11E)->f1;_LLEA: {unsigned int _tmp125;int _tmp126;struct _tuple7 _tmp124=Cyc_Evexp_eval_const_uint_exp(
_tmp116);_tmp125=_tmp124.f1;_tmp126=_tmp124.f2;if(!_tmp126){({void*_tmp127=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp128="tuple projection cannot use sizeof or offsetof";
_tag_dyneither(_tmp128,sizeof(char),47);}),_tag_dyneither(_tmp127,sizeof(void*),
0));});return;}{struct _handler_cons _tmp129;_push_handler(& _tmp129);{int _tmp12B=0;
if(setjmp(_tmp129.handler))_tmp12B=1;if(!_tmp12B){{struct _tuple9 _tmp12D;struct
Cyc_Absyn_Tqual _tmp12E;struct _tuple9*_tmp12C=((struct _tuple9*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp123,(int)_tmp125);_tmp12D=*_tmp12C;_tmp12E=_tmp12D.f1;
if(!_tmp12E.real_const){_npop_handler(0);return;}};_pop_handler();}else{void*
_tmp12A=(void*)_exn_thrown;void*_tmp130=_tmp12A;_LLEE: if(_tmp130 != Cyc_List_Nth)
goto _LLF0;_LLEF: return;_LLF0:;_LLF1:(void)_throw(_tmp130);_LLED:;}}}goto _LLE4;}
_LLEB:;_LLEC: goto _LLE4;_LLE4:;}goto _LLCD;_LLD8: if(*((int*)_tmp10C)!= 23)goto
_LLDA;_tmp117=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp10C)->f1;_tmp118=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp10C)->f2;_LLD9:{void*_tmp131=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp117->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp132;union Cyc_Absyn_AggrInfoU_union _tmp133;struct Cyc_Absyn_Aggrdecl**_tmp134;
struct Cyc_List_List*_tmp135;_LLF3: if(_tmp131 <= (void*)4)goto _LLF7;if(*((int*)
_tmp131)!= 10)goto _LLF5;_tmp132=((struct Cyc_Absyn_AggrType_struct*)_tmp131)->f1;
_tmp133=_tmp132.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp131)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LLF5;_tmp134=(_tmp133.KnownAggr).f1;_LLF4: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp134 == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp134,_tmp118);if(sf == 0  || !(sf->tq).real_const)return;goto _LLF2;}_LLF5: if(*((
int*)_tmp131)!= 11)goto _LLF7;_tmp135=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp131)->f2;_LLF6: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp135,
_tmp118);if(sf == 0  || !(sf->tq).real_const)return;goto _LLF2;}_LLF7:;_LLF8: goto
_LLF2;_LLF2:;}goto _LLCD;_LLDA: if(*((int*)_tmp10C)!= 24)goto _LLDC;_tmp119=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp10C)->f1;_tmp11A=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp10C)->f2;_LLDB:{void*_tmp136=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp119->topt))->v);struct Cyc_Absyn_PtrInfo _tmp137;void*_tmp138;
_LLFA: if(_tmp136 <= (void*)4)goto _LLFC;if(*((int*)_tmp136)!= 4)goto _LLFC;_tmp137=((
struct Cyc_Absyn_PointerType_struct*)_tmp136)->f1;_tmp138=(void*)_tmp137.elt_typ;
_LLFB:{void*_tmp139=Cyc_Tcutil_compress(_tmp138);struct Cyc_Absyn_AggrInfo _tmp13A;
union Cyc_Absyn_AggrInfoU_union _tmp13B;struct Cyc_Absyn_Aggrdecl**_tmp13C;struct
Cyc_List_List*_tmp13D;_LLFF: if(_tmp139 <= (void*)4)goto _LL103;if(*((int*)_tmp139)
!= 10)goto _LL101;_tmp13A=((struct Cyc_Absyn_AggrType_struct*)_tmp139)->f1;_tmp13B=
_tmp13A.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp139)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL101;_tmp13C=(_tmp13B.KnownAggr).f1;_LL100: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp13C == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp13C,_tmp11A);if(sf == 0  || !(sf->tq).real_const)return;goto _LLFE;}_LL101: if(*((
int*)_tmp139)!= 11)goto _LL103;_tmp13D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp139)->f2;_LL102: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(
_tmp13D,_tmp11A);if(sf == 0  || !(sf->tq).real_const)return;goto _LLFE;}_LL103:;
_LL104: goto _LLFE;_LLFE:;}goto _LLF9;_LLFC:;_LLFD: goto _LLF9;_LLF9:;}goto _LLCD;
_LLDC: if(*((int*)_tmp10C)!= 22)goto _LLDE;_tmp11B=((struct Cyc_Absyn_Deref_e_struct*)
_tmp10C)->f1;_LLDD:{void*_tmp13E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp11B->topt))->v);struct Cyc_Absyn_PtrInfo _tmp13F;struct Cyc_Absyn_Tqual
_tmp140;struct Cyc_Absyn_ArrayInfo _tmp141;struct Cyc_Absyn_Tqual _tmp142;_LL106: if(
_tmp13E <= (void*)4)goto _LL10A;if(*((int*)_tmp13E)!= 4)goto _LL108;_tmp13F=((
struct Cyc_Absyn_PointerType_struct*)_tmp13E)->f1;_tmp140=_tmp13F.elt_tq;_LL107:
_tmp142=_tmp140;goto _LL109;_LL108: if(*((int*)_tmp13E)!= 7)goto _LL10A;_tmp141=((
struct Cyc_Absyn_ArrayType_struct*)_tmp13E)->f1;_tmp142=_tmp141.tq;_LL109: if(!
_tmp142.real_const)return;goto _LL105;_LL10A:;_LL10B: goto _LL105;_LL105:;}goto
_LLCD;_LLDE: if(*((int*)_tmp10C)!= 13)goto _LLE0;_tmp11C=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp10C)->f1;_LLDF: _tmp11D=_tmp11C;goto _LLE1;_LLE0: if(*((int*)_tmp10C)!= 14)goto
_LLE2;_tmp11D=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp10C)->f1;_LLE1: Cyc_Tcexp_check_writable(
te,_tmp11D);return;_LLE2:;_LLE3: goto _LLCD;_LLCD:;}({struct Cyc_String_pa_struct
_tmp145;_tmp145.tag=0;_tmp145.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp143[1]={& _tmp145};Cyc_Tcutil_terr(e->loc,({
const char*_tmp144="attempt to write a const location: %s";_tag_dyneither(_tmp144,
sizeof(char),38);}),_tag_dyneither(_tmp143,sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,void*i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(e))return({
void*_tmp146=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp147="increment/decrement of non-lvalue";
_tag_dyneither(_tmp147,sizeof(char),34);}),_tag_dyneither(_tmp146,sizeof(void*),
0));});Cyc_Tcexp_check_writable(te,e);{void*t=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(e)){void*telt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == (void*)0  || i == (
void*)1)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(void*)1))({void*
_tmp148=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp149="can't perform arithmetic on abstract pointer type";
_tag_dyneither(_tmp149,sizeof(char),50);}),_tag_dyneither(_tmp148,sizeof(void*),
0));});if(Cyc_Tcutil_is_noalias_pointer(t))({void*_tmp14A=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp14B="can't perform arithmetic on non-aliasing pointer type";
_tag_dyneither(_tmp14B,sizeof(char),54);}),_tag_dyneither(_tmp14A,sizeof(void*),
0));});}else{({struct Cyc_String_pa_struct _tmp14E;_tmp14E.tag=0;_tmp14E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*
_tmp14C[1]={& _tmp14E};Cyc_Tcutil_terr(e->loc,({const char*_tmp14D="expecting arithmetic or ? type but found %s";
_tag_dyneither(_tmp14D,sizeof(char),44);}),_tag_dyneither(_tmp14C,sizeof(void*),
1));}});}}return t;}}static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3){Cyc_Tcexp_tcTest(te,e1,({const char*_tmp14F="conditional expression";
_tag_dyneither(_tmp14F,sizeof(char),23);}));Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(
te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(
te));struct Cyc_List_List _tmp150=({struct Cyc_List_List _tmp157;_tmp157.hd=e3;
_tmp157.tl=0;_tmp157;});struct Cyc_List_List _tmp151=({struct Cyc_List_List _tmp156;
_tmp156.hd=e2;_tmp156.tl=(struct Cyc_List_List*)& _tmp150;_tmp156;});if(!Cyc_Tcutil_coerce_list(
te,t,(struct Cyc_List_List*)& _tmp151)){({struct Cyc_String_pa_struct _tmp155;
_tmp155.tag=0;_tmp155.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v));{struct Cyc_String_pa_struct
_tmp154;_tmp154.tag=0;_tmp154.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));{
void*_tmp152[2]={& _tmp154,& _tmp155};Cyc_Tcutil_terr(loc,({const char*_tmp153="conditional clause types do not match: %s != %s";
_tag_dyneither(_tmp153,sizeof(char),48);}),_tag_dyneither(_tmp152,sizeof(void*),
2));}}});Cyc_Tcutil_explain_failure();}return t;}}static void*Cyc_Tcexp_tcAnd(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({const char*_tmp158="logical-and expression";
_tag_dyneither(_tmp158,sizeof(char),23);}));Cyc_Tcexp_tcTest(te,e2,({const char*
_tmp159="logical-and expression";_tag_dyneither(_tmp159,sizeof(char),23);}));
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
Cyc_Tcexp_tcTest(te,e1,({const char*_tmp15A="logical-or expression";
_tag_dyneither(_tmp15A,sizeof(char),22);}));Cyc_Tcexp_tcTest(te,e2,({const char*
_tmp15B="logical-or expression";_tag_dyneither(_tmp15B,sizeof(char),22);}));
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp15C=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(_tmp15C,te),0,e1);Cyc_Tcexp_tcExp(
te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),
e2);{void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{void*_tmp15D=Cyc_Tcutil_compress(
t1);_LL10D: if(_tmp15D <= (void*)4)goto _LL10F;if(*((int*)_tmp15D)!= 7)goto _LL10F;
_LL10E:({void*_tmp15E=0;Cyc_Tcutil_terr(loc,({const char*_tmp15F="cannot assign to an array";
_tag_dyneither(_tmp15F,sizeof(char),26);}),_tag_dyneither(_tmp15E,sizeof(void*),
0));});goto _LL10C;_LL10F:;_LL110: goto _LL10C;_LL10C:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),(void*)1))({void*_tmp160=0;Cyc_Tcutil_terr(loc,({const char*_tmp161="type is abstract (can't determine size).";
_tag_dyneither(_tmp161,sizeof(char),41);}),_tag_dyneither(_tmp160,sizeof(void*),
0));});if(!Cyc_Absyn_is_lvalue(e1))return({void*_tmp162=0;Cyc_Tcexp_expr_err(te,
loc,topt,({const char*_tmp163="assignment to non-lvalue";_tag_dyneither(_tmp163,
sizeof(char),25);}),_tag_dyneither(_tmp162,sizeof(void*),0));});Cyc_Tcexp_check_writable(
te,e1);if(po == 0){struct _RegionHandle*_tmp164=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp164,t2) && !Cyc_Tcutil_is_noalias_path(_tmp164,e2))({void*_tmp165=0;Cyc_Tcutil_terr(
e2->loc,({const char*_tmp166="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp166,sizeof(char),49);}),_tag_dyneither(_tmp165,sizeof(void*),
0));});if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){void*_tmp167=({struct Cyc_String_pa_struct
_tmp16B;_tmp16B.tag=0;_tmp16B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp16A;_tmp16A.tag=0;
_tmp16A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp168[2]={& _tmp16A,& _tmp16B};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp169="type mismatch: %s != %s";_tag_dyneither(_tmp169,sizeof(char),24);}),
_tag_dyneither(_tmp168,sizeof(void*),2));}}});Cyc_Tcutil_unify(t1,t2);Cyc_Tcutil_explain_failure();
return _tmp167;}}else{void*_tmp16C=(void*)po->v;void*_tmp16D=Cyc_Tcexp_tcBinPrimop(
te,loc,0,_tmp16C,e1,e2);if(!(Cyc_Tcutil_unify(_tmp16D,t1) || Cyc_Tcutil_coerceable(
_tmp16D) && Cyc_Tcutil_coerceable(t1))){void*_tmp16E=({struct Cyc_String_pa_struct
_tmp172;_tmp172.tag=0;_tmp172.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp171;_tmp171.tag=0;
_tmp171.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1));{void*_tmp16F[2]={& _tmp171,& _tmp172};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp170="Cannot use this operator in an assignment when the arguments have types %s and %s";
_tag_dyneither(_tmp170,sizeof(char),82);}),_tag_dyneither(_tmp16F,sizeof(void*),
2));}}});Cyc_Tcutil_unify(_tmp16D,t1);Cyc_Tcutil_explain_failure();return _tmp16E;}
return _tmp16D;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={6,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp173=Cyc_Tcutil_compress(
t1);void*_tmp174;void*_tmp175;_LL112: if((int)_tmp173 != 1)goto _LL114;_LL113: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ,(void*)1);t1=dbl_typ;goto _LL111;_LL114: if(_tmp173 <= (void*)4)goto
_LL118;if(*((int*)_tmp173)!= 5)goto _LL116;_tmp174=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp173)->f2;if((int)_tmp174 != 0)goto _LL116;_LL115: goto _LL117;_LL116: if(*((int*)
_tmp173)!= 5)goto _LL118;_tmp175=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp173)->f2;
if((int)_tmp175 != 1)goto _LL118;_LL117: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);t1=Cyc_Absyn_sint_typ;goto _LL111;_LL118:;_LL119: goto _LL111;_LL111:;}
for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp177;struct Cyc_List_List*
_tmp178;struct Cyc_Position_Segment*_tmp179;void*_tmp17A;struct Cyc_Absyn_Tunionfield
_tmp176=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_tmp177=_tmp176.name;_tmp178=
_tmp176.typs;_tmp179=_tmp176.loc;_tmp17A=(void*)_tmp176.sc;if(_tmp178 == 0  || 
_tmp178->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple9*)_tmp178->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)fields->hd);}}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp17C;struct Cyc_List_List*_tmp17D;struct Cyc_Position_Segment*
_tmp17E;void*_tmp17F;struct Cyc_Absyn_Tunionfield _tmp17B=*((struct Cyc_Absyn_Tunionfield*)
fields->hd);_tmp17C=_tmp17B.name;_tmp17D=_tmp17B.typs;_tmp17E=_tmp17B.loc;
_tmp17F=(void*)_tmp17B.sc;if(_tmp17D == 0  || _tmp17D->tl != 0)continue;{void*t2=
Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple9*)_tmp17D->hd)).f2);if(Cyc_Tcutil_coerce_arg(
te,e,t2))return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)
fields->hd);}}({struct Cyc_String_pa_struct _tmp183;_tmp183.tag=0;_tmp183.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{struct
Cyc_String_pa_struct _tmp182;_tmp182.tag=0;_tmp182.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu));{void*_tmp180[2]={& _tmp182,&
_tmp183};Cyc_Tcutil_terr(e->loc,({const char*_tmp181="can't find a field in %s to inject a value of type %s";
_tag_dyneither(_tmp181,sizeof(char),54);}),_tag_dyneither(_tmp180,sizeof(void*),
2));}}});return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){struct Cyc_List_List*
_tmp185=args;struct _RegionHandle*_tmp186=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*
_tmp187=Cyc_Tcenv_new_block(_tmp186,loc,te_orig);Cyc_Tcexp_tcExp(_tmp187,0,e);{
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp188=t;struct Cyc_Absyn_PtrInfo _tmp189;void*_tmp18A;struct Cyc_Absyn_Tqual
_tmp18B;struct Cyc_Absyn_PtrAtts _tmp18C;void*_tmp18D;struct Cyc_Absyn_Conref*
_tmp18E;struct Cyc_Absyn_Conref*_tmp18F;struct Cyc_Absyn_Conref*_tmp190;_LL11B: if(
_tmp188 <= (void*)4)goto _LL11D;if(*((int*)_tmp188)!= 4)goto _LL11D;_tmp189=((
struct Cyc_Absyn_PointerType_struct*)_tmp188)->f1;_tmp18A=(void*)_tmp189.elt_typ;
_tmp18B=_tmp189.elt_tq;_tmp18C=_tmp189.ptr_atts;_tmp18D=(void*)_tmp18C.rgn;
_tmp18E=_tmp18C.nullable;_tmp18F=_tmp18C.bounds;_tmp190=_tmp18C.zero_term;_LL11C:
Cyc_Tcenv_check_rgn_accessible(_tmp187,loc,_tmp18D);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp18F);{void*_tmp191=Cyc_Tcutil_compress(_tmp18A);struct Cyc_Absyn_FnInfo
_tmp192;struct Cyc_List_List*_tmp193;struct Cyc_Core_Opt*_tmp194;void*_tmp195;
struct Cyc_List_List*_tmp196;int _tmp197;struct Cyc_Absyn_VarargInfo*_tmp198;struct
Cyc_List_List*_tmp199;struct Cyc_List_List*_tmp19A;_LL120: if(_tmp191 <= (void*)4)
goto _LL122;if(*((int*)_tmp191)!= 8)goto _LL122;_tmp192=((struct Cyc_Absyn_FnType_struct*)
_tmp191)->f1;_tmp193=_tmp192.tvars;_tmp194=_tmp192.effect;_tmp195=(void*)_tmp192.ret_typ;
_tmp196=_tmp192.args;_tmp197=_tmp192.c_varargs;_tmp198=_tmp192.cyc_varargs;
_tmp199=_tmp192.rgn_po;_tmp19A=_tmp192.attributes;_LL121: if(topt != 0)Cyc_Tcutil_unify(
_tmp195,*topt);while(_tmp185 != 0  && _tmp196 != 0){struct Cyc_Absyn_Exp*e1=(struct
Cyc_Absyn_Exp*)_tmp185->hd;void*t2=(*((struct _tuple2*)_tmp196->hd)).f3;Cyc_Tcexp_tcExp(
_tmp187,(void**)& t2,e1);if(!Cyc_Tcutil_coerce_arg(_tmp187,e1,t2)){({struct Cyc_String_pa_struct
_tmp19E;_tmp19E.tag=0;_tmp19E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{struct Cyc_String_pa_struct _tmp19D;_tmp19D.tag=0;
_tmp19D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{void*_tmp19B[2]={&
_tmp19D,& _tmp19E};Cyc_Tcutil_terr(e1->loc,({const char*_tmp19C="actual argument has type \n\t%s\n but formal has type \n\t%s.";
_tag_dyneither(_tmp19C,sizeof(char),57);}),_tag_dyneither(_tmp19B,sizeof(void*),
2));}}});Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,
t2);Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp186,t2) && !Cyc_Tcutil_is_noalias_path(_tmp186,e1))({void*_tmp19F=0;Cyc_Tcutil_terr(
e1->loc,({const char*_tmp1A0="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp1A0,sizeof(char),49);}),_tag_dyneither(_tmp19F,sizeof(void*),
0));});_tmp185=_tmp185->tl;_tmp196=_tmp196->tl;}{int args_already_checked=0;{
struct Cyc_List_List*a=_tmp19A;for(0;a != 0;a=a->tl){void*_tmp1A1=(void*)a->hd;
void*_tmp1A2;int _tmp1A3;int _tmp1A4;_LL125: if(_tmp1A1 <= (void*)17)goto _LL127;if(*((
int*)_tmp1A1)!= 3)goto _LL127;_tmp1A2=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp1A1)->f1;_tmp1A3=((struct Cyc_Absyn_Format_att_struct*)_tmp1A1)->f2;_tmp1A4=((
struct Cyc_Absyn_Format_att_struct*)_tmp1A1)->f3;_LL126:{struct _handler_cons
_tmp1A5;_push_handler(& _tmp1A5);{int _tmp1A7=0;if(setjmp(_tmp1A5.handler))_tmp1A7=
1;if(!_tmp1A7){{struct Cyc_Absyn_Exp*_tmp1A8=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(args,_tmp1A3 - 1);struct Cyc_Core_Opt*fmt_args;if(_tmp1A4 == 
0)fmt_args=0;else{fmt_args=({struct Cyc_Core_Opt*_tmp1A9=_cycalloc(sizeof(*
_tmp1A9));_tmp1A9->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(
args,_tmp1A4 - 1);_tmp1A9;});}args_already_checked=1;{struct _RegionHandle*_tmp1AA=
Cyc_Tcenv_get_fnrgn(te_orig);void*_tmp1AB=_tmp1A2;_LL12A: if((int)_tmp1AB != 0)
goto _LL12C;_LL12B: Cyc_Tcexp_check_format_args(_tmp187,_tmp1A8,fmt_args,_tmp1AA,
Cyc_Formatstr_get_format_typs);goto _LL129;_LL12C: if((int)_tmp1AB != 1)goto _LL129;
_LL12D: Cyc_Tcexp_check_format_args(_tmp187,_tmp1A8,fmt_args,_tmp1AA,Cyc_Formatstr_get_scanf_typs);
goto _LL129;_LL129:;}};_pop_handler();}else{void*_tmp1A6=(void*)_exn_thrown;void*
_tmp1AD=_tmp1A6;_LL12F: if(_tmp1AD != Cyc_List_Nth)goto _LL131;_LL130:({void*
_tmp1AE=0;Cyc_Tcutil_terr(loc,({const char*_tmp1AF="bad format arguments";
_tag_dyneither(_tmp1AF,sizeof(char),21);}),_tag_dyneither(_tmp1AE,sizeof(void*),
0));});goto _LL12E;_LL131:;_LL132:(void)_throw(_tmp1AD);_LL12E:;}}}goto _LL124;
_LL127:;_LL128: goto _LL124;_LL124:;}}if(_tmp196 != 0)({void*_tmp1B0=0;Cyc_Tcutil_terr(
loc,({const char*_tmp1B1="too few arguments for function";_tag_dyneither(_tmp1B1,
sizeof(char),31);}),_tag_dyneither(_tmp1B0,sizeof(void*),0));});else{if((_tmp185
!= 0  || _tmp197) || _tmp198 != 0){if(_tmp197)for(0;_tmp185 != 0;_tmp185=_tmp185->tl){
Cyc_Tcexp_tcExp(_tmp187,0,(struct Cyc_Absyn_Exp*)_tmp185->hd);}else{if(_tmp198 == 
0)({void*_tmp1B2=0;Cyc_Tcutil_terr(loc,({const char*_tmp1B3="too many arguments for function";
_tag_dyneither(_tmp1B3,sizeof(char),32);}),_tag_dyneither(_tmp1B2,sizeof(void*),
0));});else{void*_tmp1B5;int _tmp1B6;struct Cyc_Absyn_VarargInfo _tmp1B4=*_tmp198;
_tmp1B5=(void*)_tmp1B4.type;_tmp1B6=_tmp1B4.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp1B7=({struct Cyc_Absyn_VarargCallInfo*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));
_tmp1D3->num_varargs=0;_tmp1D3->injectors=0;_tmp1D3->vai=(struct Cyc_Absyn_VarargInfo*)
_tmp198;_tmp1D3;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)_tmp1B7;
if(!_tmp1B6)for(0;_tmp185 != 0;_tmp185=_tmp185->tl){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp185->hd;++ _tmp1B7->num_varargs;Cyc_Tcexp_tcExp(_tmp187,(
void**)& _tmp1B5,e1);if(!Cyc_Tcutil_coerce_arg(_tmp187,e1,_tmp1B5)){({struct Cyc_String_pa_struct
_tmp1BB;_tmp1BB.tag=0;_tmp1BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));{
struct Cyc_String_pa_struct _tmp1BA;_tmp1BA.tag=0;_tmp1BA.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1B5));{void*_tmp1B8[2]={& _tmp1BA,&
_tmp1BB};Cyc_Tcutil_terr(loc,({const char*_tmp1B9="vararg requires type %s but argument has type %s";
_tag_dyneither(_tmp1B9,sizeof(char),49);}),_tag_dyneither(_tmp1B8,sizeof(void*),
2));}}});Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp186,_tmp1B5) && !Cyc_Tcutil_is_noalias_path(_tmp186,e1))({void*_tmp1BC=0;Cyc_Tcutil_terr(
e1->loc,({const char*_tmp1BD="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp1BD,sizeof(char),49);}),_tag_dyneither(_tmp1BC,sizeof(void*),
0));});}else{void*_tmp1BE=Cyc_Tcutil_compress(_tmp1B5);struct Cyc_Absyn_TunionInfo
_tmp1BF;union Cyc_Absyn_TunionInfoU_union _tmp1C0;struct Cyc_Absyn_Tuniondecl**
_tmp1C1;struct Cyc_Absyn_Tuniondecl*_tmp1C2;struct Cyc_List_List*_tmp1C3;struct Cyc_Core_Opt*
_tmp1C4;_LL134: if(_tmp1BE <= (void*)4)goto _LL136;if(*((int*)_tmp1BE)!= 2)goto
_LL136;_tmp1BF=((struct Cyc_Absyn_TunionType_struct*)_tmp1BE)->f1;_tmp1C0=_tmp1BF.tunion_info;
if((((((struct Cyc_Absyn_TunionType_struct*)_tmp1BE)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL136;_tmp1C1=(_tmp1C0.KnownTunion).f1;_tmp1C2=*_tmp1C1;_tmp1C3=
_tmp1BF.targs;_tmp1C4=_tmp1BF.rgn;_LL135: {struct Cyc_Absyn_Tuniondecl*_tmp1C5=*
Cyc_Tcenv_lookup_tuniondecl(_tmp187,loc,_tmp1C2->name);struct Cyc_List_List*
fields=0;if(_tmp1C5->fields == 0)({struct Cyc_String_pa_struct _tmp1C8;_tmp1C8.tag=
0;_tmp1C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp1B5));{void*_tmp1C6[1]={& _tmp1C8};Cyc_Tcutil_terr(loc,({const char*_tmp1C7="can't inject into %s";
_tag_dyneither(_tmp1C7,sizeof(char),21);}),_tag_dyneither(_tmp1C6,sizeof(void*),
1));}});else{fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1C5->fields))->v;}if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1C4))->v,Cyc_Tcenv_curr_rgn(_tmp187)))({void*_tmp1C9=0;Cyc_Tcutil_terr(
loc,({const char*_tmp1CA="bad region for injected varargs";_tag_dyneither(_tmp1CA,
sizeof(char),32);}),_tag_dyneither(_tmp1C9,sizeof(void*),0));});{struct
_RegionHandle*_tmp1CB=Cyc_Tcenv_get_fnrgn(_tmp187);{struct Cyc_List_List*_tmp1CC=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp1CB,_tmp1CB,_tmp1C5->tvs,_tmp1C3);
for(0;_tmp185 != 0;_tmp185=_tmp185->tl){++ _tmp1B7->num_varargs;{struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp185->hd;if(!args_already_checked){Cyc_Tcexp_tcExp(
_tmp187,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1CB,(void*)((struct
Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(_tmp1CB,
e1))({void*_tmp1CD=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp1CE="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp1CE,sizeof(char),49);}),_tag_dyneither(_tmp1CD,sizeof(void*),
0));});}{struct Cyc_Absyn_Tunionfield*_tmp1CF=Cyc_Tcexp_tcInjection(_tmp187,e1,
_tmp1B5,_tmp1CB,_tmp1CC,fields);if(_tmp1CF != 0)_tmp1B7->injectors=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1B7->injectors,({
struct Cyc_List_List*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->hd=(struct Cyc_Absyn_Tunionfield*)
_tmp1CF;_tmp1D0->tl=0;_tmp1D0;}));}}}}goto _LL133;}}_LL136:;_LL137:({void*_tmp1D1=
0;Cyc_Tcutil_terr(loc,({const char*_tmp1D2="bad inject vararg type";
_tag_dyneither(_tmp1D2,sizeof(char),23);}),_tag_dyneither(_tmp1D1,sizeof(void*),
0));});goto _LL133;_LL133:;}}}}}}Cyc_Tcenv_check_effect_accessible(_tmp187,loc,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp194))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp187,loc,_tmp199);return _tmp195;}_LL122:;_LL123: return({void*_tmp1D4=0;Cyc_Tcexp_expr_err(
_tmp187,loc,topt,({const char*_tmp1D5="expected pointer to function";
_tag_dyneither(_tmp1D5,sizeof(char),29);}),_tag_dyneither(_tmp1D4,sizeof(void*),
0));});_LL11F:;}_LL11D:;_LL11E: return({void*_tmp1D6=0;Cyc_Tcexp_expr_err(_tmp187,
loc,topt,({const char*_tmp1D7="expected pointer to function";_tag_dyneither(
_tmp1D7,sizeof(char),29);}),_tag_dyneither(_tmp1D6,sizeof(void*),0));});_LL11A:;}}
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,
e);if(!Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ))({struct Cyc_String_pa_struct
_tmp1DA;_tmp1DA.tag=0;_tmp1DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp1D8[1]={& _tmp1DA};Cyc_Tcutil_terr(loc,({const char*_tmp1D9="expected xtunion exn but found %s";
_tag_dyneither(_tmp1D9,sizeof(char),34);}),_tag_dyneither(_tmp1D8,sizeof(void*),
1));}});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}static void*
Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(
te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0));{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp1DB=t1;struct Cyc_Absyn_PtrInfo
_tmp1DC;void*_tmp1DD;struct Cyc_Absyn_Tqual _tmp1DE;struct Cyc_Absyn_PtrAtts _tmp1DF;
void*_tmp1E0;struct Cyc_Absyn_Conref*_tmp1E1;struct Cyc_Absyn_Conref*_tmp1E2;
struct Cyc_Absyn_Conref*_tmp1E3;_LL139: if(_tmp1DB <= (void*)4)goto _LL13B;if(*((int*)
_tmp1DB)!= 4)goto _LL13B;_tmp1DC=((struct Cyc_Absyn_PointerType_struct*)_tmp1DB)->f1;
_tmp1DD=(void*)_tmp1DC.elt_typ;_tmp1DE=_tmp1DC.elt_tq;_tmp1DF=_tmp1DC.ptr_atts;
_tmp1E0=(void*)_tmp1DF.rgn;_tmp1E1=_tmp1DF.nullable;_tmp1E2=_tmp1DF.bounds;
_tmp1E3=_tmp1DF.zero_term;_LL13A:{void*_tmp1E4=Cyc_Tcutil_compress(_tmp1DD);
struct Cyc_Absyn_FnInfo _tmp1E5;struct Cyc_List_List*_tmp1E6;struct Cyc_Core_Opt*
_tmp1E7;void*_tmp1E8;struct Cyc_List_List*_tmp1E9;int _tmp1EA;struct Cyc_Absyn_VarargInfo*
_tmp1EB;struct Cyc_List_List*_tmp1EC;struct Cyc_List_List*_tmp1ED;_LL13E: if(
_tmp1E4 <= (void*)4)goto _LL140;if(*((int*)_tmp1E4)!= 8)goto _LL140;_tmp1E5=((
struct Cyc_Absyn_FnType_struct*)_tmp1E4)->f1;_tmp1E6=_tmp1E5.tvars;_tmp1E7=
_tmp1E5.effect;_tmp1E8=(void*)_tmp1E5.ret_typ;_tmp1E9=_tmp1E5.args;_tmp1EA=
_tmp1E5.c_varargs;_tmp1EB=_tmp1E5.cyc_varargs;_tmp1EC=_tmp1E5.rgn_po;_tmp1ED=
_tmp1E5.attributes;_LL13F: {struct _RegionHandle*_tmp1EE=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*instantiation=0;for(0;ts != 0  && _tmp1E6 != 0;(ts=ts->tl,
_tmp1E6=_tmp1E6->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp1E6->hd);
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);
instantiation=({struct Cyc_List_List*_tmp1EF=_region_malloc(_tmp1EE,sizeof(*
_tmp1EF));_tmp1EF->hd=({struct _tuple5*_tmp1F0=_region_malloc(_tmp1EE,sizeof(*
_tmp1F0));_tmp1F0->f1=(struct Cyc_Absyn_Tvar*)_tmp1E6->hd;_tmp1F0->f2=(void*)ts->hd;
_tmp1F0;});_tmp1EF->tl=instantiation;_tmp1EF;});}if(ts != 0)return({void*_tmp1F1=
0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp1F2="too many type variables in instantiation";
_tag_dyneither(_tmp1F2,sizeof(char),41);}),_tag_dyneither(_tmp1F1,sizeof(void*),
0));});{void*new_fn_typ=Cyc_Tcutil_rsubstitute(_tmp1EE,instantiation,(void*)({
struct Cyc_Absyn_FnType_struct*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));_tmp1F7[0]=({
struct Cyc_Absyn_FnType_struct _tmp1F8;_tmp1F8.tag=8;_tmp1F8.f1=({struct Cyc_Absyn_FnInfo
_tmp1F9;_tmp1F9.tvars=_tmp1E6;_tmp1F9.effect=_tmp1E7;_tmp1F9.ret_typ=(void*)
_tmp1E8;_tmp1F9.args=_tmp1E9;_tmp1F9.c_varargs=_tmp1EA;_tmp1F9.cyc_varargs=
_tmp1EB;_tmp1F9.rgn_po=_tmp1EC;_tmp1F9.attributes=_tmp1ED;_tmp1F9;});_tmp1F8;});
_tmp1F7;}));return(void*)({struct Cyc_Absyn_PointerType_struct*_tmp1F3=_cycalloc(
sizeof(*_tmp1F3));_tmp1F3[0]=({struct Cyc_Absyn_PointerType_struct _tmp1F4;_tmp1F4.tag=
4;_tmp1F4.f1=({struct Cyc_Absyn_PtrInfo _tmp1F5;_tmp1F5.elt_typ=(void*)new_fn_typ;
_tmp1F5.elt_tq=_tmp1DE;_tmp1F5.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp1F6;_tmp1F6.rgn=(
void*)_tmp1E0;_tmp1F6.nullable=_tmp1E1;_tmp1F6.bounds=_tmp1E2;_tmp1F6.zero_term=
_tmp1E3;_tmp1F6.ptrloc=0;_tmp1F6;});_tmp1F5;});_tmp1F4;});_tmp1F3;});}}_LL140:;
_LL141: goto _LL13D;_LL13D:;}goto _LL138;_LL13B:;_LL13C: goto _LL138;_LL138:;}return({
struct Cyc_String_pa_struct _tmp1FC;_tmp1FC.tag=0;_tmp1FC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{void*_tmp1FA[1]={& _tmp1FC};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp1FB="expecting polymorphic type but found %s";
_tag_dyneither(_tmp1FB,sizeof(char),40);}),_tag_dyneither(_tmp1FA,sizeof(void*),
1));}});}}static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,void**c){Cyc_Tcutil_check_type(loc,
te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(
te,loc,t2,t))*((void**)_check_null(c))=(void*)1;else{void*crc=Cyc_Tcutil_castable(
te,loc,t2,t);if(crc != (void*)0)*((void**)_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(
te,t,e))*((void**)_check_null(c))=(void*)1;else{Cyc_Tcutil_unify(t2,t);{void*
_tmp1FD=({struct Cyc_String_pa_struct _tmp201;_tmp201.tag=0;_tmp201.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp200;_tmp200.tag=0;_tmp200.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp1FE[2]={& _tmp200,& _tmp201};Cyc_Tcexp_expr_err(
te,loc,(void**)& t,({const char*_tmp1FF="cannot cast %s to %s";_tag_dyneither(
_tmp1FF,sizeof(char),21);}),_tag_dyneither(_tmp1FE,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return _tmp1FD;}}}}{struct _tuple0 _tmp203=({struct _tuple0 _tmp202;_tmp202.f1=(void*)
e->r;_tmp202.f2=Cyc_Tcutil_compress(t);_tmp202;});void*_tmp204;struct Cyc_Absyn_MallocInfo
_tmp205;int _tmp206;void*_tmp207;struct Cyc_Absyn_PtrInfo _tmp208;struct Cyc_Absyn_PtrAtts
_tmp209;struct Cyc_Absyn_Conref*_tmp20A;struct Cyc_Absyn_Conref*_tmp20B;struct Cyc_Absyn_Conref*
_tmp20C;_LL143: _tmp204=_tmp203.f1;if(*((int*)_tmp204)!= 35)goto _LL145;_tmp205=((
struct Cyc_Absyn_Malloc_e_struct*)_tmp204)->f1;_tmp206=_tmp205.fat_result;_tmp207=
_tmp203.f2;if(_tmp207 <= (void*)4)goto _LL145;if(*((int*)_tmp207)!= 4)goto _LL145;
_tmp208=((struct Cyc_Absyn_PointerType_struct*)_tmp207)->f1;_tmp209=_tmp208.ptr_atts;
_tmp20A=_tmp209.nullable;_tmp20B=_tmp209.bounds;_tmp20C=_tmp209.zero_term;_LL144:
if((_tmp206  && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp20C)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp20A)){void*_tmp20D=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp20B);struct
Cyc_Absyn_Exp*_tmp20E;_LL148: if(_tmp20D <= (void*)1)goto _LL14A;if(*((int*)_tmp20D)
!= 0)goto _LL14A;_tmp20E=((struct Cyc_Absyn_Upper_b_struct*)_tmp20D)->f1;_LL149:
if((Cyc_Evexp_eval_const_uint_exp(_tmp20E)).f1 == 1)({void*_tmp20F=0;Cyc_Tcutil_warn(
loc,({const char*_tmp210="cast from ? pointer to * pointer will lose size information";
_tag_dyneither(_tmp210,sizeof(char),60);}),_tag_dyneither(_tmp20F,sizeof(void*),
0));});goto _LL147;_LL14A:;_LL14B: goto _LL147;_LL147:;}goto _LL142;_LL145:;_LL146:
goto _LL142;_LL142:;}return t;}}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp211=0;struct Cyc_Absyn_Tqual _tmp212=Cyc_Absyn_empty_tqual(0);if(topt
!= 0){void*_tmp213=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp214;
void*_tmp215;struct Cyc_Absyn_Tqual _tmp216;struct Cyc_Absyn_PtrAtts _tmp217;struct
Cyc_Absyn_Conref*_tmp218;_LL14D: if(_tmp213 <= (void*)4)goto _LL14F;if(*((int*)
_tmp213)!= 4)goto _LL14F;_tmp214=((struct Cyc_Absyn_PointerType_struct*)_tmp213)->f1;
_tmp215=(void*)_tmp214.elt_typ;_tmp216=_tmp214.elt_tq;_tmp217=_tmp214.ptr_atts;
_tmp218=_tmp217.zero_term;_LL14E: _tmp211=({void**_tmp219=_cycalloc(sizeof(*
_tmp219));_tmp219[0]=_tmp215;_tmp219;});_tmp212=_tmp216;goto _LL14C;_LL14F:;
_LL150: goto _LL14C;_LL14C:;}{struct _RegionHandle*_tmp21A=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_clear_notreadctxt(_tmp21A,te),_tmp211,e);if(Cyc_Tcutil_is_noalias_path(
_tmp21A,e))({void*_tmp21B=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp21C="Cannot take the address of an alias-free path";
_tag_dyneither(_tmp21C,sizeof(char),46);}),_tag_dyneither(_tmp21B,sizeof(void*),
0));});{void*_tmp21D=(void*)e->r;struct Cyc_Absyn_Exp*_tmp21E;struct Cyc_Absyn_Exp*
_tmp21F;_LL152: if(*((int*)_tmp21D)!= 25)goto _LL154;_tmp21E=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp21D)->f1;_tmp21F=((struct Cyc_Absyn_Subscript_e_struct*)_tmp21D)->f2;_LL153:{
void*_tmp220=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp21E->topt))->v);_LL157: if(_tmp220 <= (void*)4)goto _LL159;if(*((int*)_tmp220)
!= 9)goto _LL159;_LL158: goto _LL156;_LL159:;_LL15A:(void*)(e0->r=(void*)((void*)(
Cyc_Absyn_add_exp(_tmp21E,_tmp21F,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp21E,
_tmp21F);_LL156:;}goto _LL151;_LL154:;_LL155: goto _LL151;_LL151:;}{int _tmp222;void*
_tmp223;struct _tuple6 _tmp221=Cyc_Tcutil_addressof_props(te,e);_tmp222=_tmp221.f1;
_tmp223=_tmp221.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp222){
tq.print_const=1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp223,tq,Cyc_Absyn_false_conref);return t;}}}}}static
void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t){if(te->allow_valueof)return Cyc_Absyn_uint_typ;Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t))({
struct Cyc_String_pa_struct _tmp226;_tmp226.tag=0;_tmp226.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp224[1]={& _tmp226};Cyc_Tcutil_terr(
loc,({const char*_tmp225="sizeof applied to type %s, which has unknown size here";
_tag_dyneither(_tmp225,sizeof(char),55);}),_tag_dyneither(_tmp224,sizeof(void*),
1));}});if(topt != 0){void*_tmp227=Cyc_Tcutil_compress(*topt);void*_tmp228;_LL15C:
if(_tmp227 <= (void*)4)goto _LL15E;if(*((int*)_tmp227)!= 18)goto _LL15E;_tmp228=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp227)->f1;_LL15D: {struct Cyc_Absyn_Exp*
_tmp229=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct*_tmp22A=({
struct Cyc_Absyn_ValueofType_struct*_tmp22B=_cycalloc(sizeof(*_tmp22B));_tmp22B[0]=({
struct Cyc_Absyn_ValueofType_struct _tmp22C;_tmp22C.tag=17;_tmp22C.f1=_tmp229;
_tmp22C;});_tmp22B;});if(Cyc_Tcutil_unify(_tmp228,(void*)_tmp22A))return _tmp227;
goto _LL15B;}_LL15E:;_LL15F: goto _LL15B;_LL15B:;}return Cyc_Absyn_uint_typ;}int Cyc_Tcexp_structfield_has_name(
struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){return Cyc_strcmp((struct
_dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0;}static void*Cyc_Tcexp_tcOffsetof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*
_tmp22D=n;struct _dyneither_ptr*_tmp22E;unsigned int _tmp22F;_LL161: if(*((int*)
_tmp22D)!= 0)goto _LL163;_tmp22E=((struct Cyc_Absyn_StructField_struct*)_tmp22D)->f1;
_LL162: {int bad_type=1;{void*_tmp230=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp231;union Cyc_Absyn_AggrInfoU_union _tmp232;struct Cyc_Absyn_Aggrdecl**_tmp233;
struct Cyc_List_List*_tmp234;_LL166: if(_tmp230 <= (void*)4)goto _LL16A;if(*((int*)
_tmp230)!= 10)goto _LL168;_tmp231=((struct Cyc_Absyn_AggrType_struct*)_tmp230)->f1;
_tmp232=_tmp231.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp230)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL168;_tmp233=(_tmp232.KnownAggr).f1;_LL167: if((*_tmp233)->impl == 0)
goto _LL165;if(!((int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),
struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp22E,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp233)->impl))->fields))({
struct Cyc_String_pa_struct _tmp237;_tmp237.tag=0;_tmp237.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp22E);{void*_tmp235[1]={& _tmp237};Cyc_Tcutil_terr(loc,({
const char*_tmp236="no field of struct/union has name %s";_tag_dyneither(_tmp236,
sizeof(char),37);}),_tag_dyneither(_tmp235,sizeof(void*),1));}});bad_type=0;goto
_LL165;_LL168: if(*((int*)_tmp230)!= 11)goto _LL16A;_tmp234=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp230)->f2;_LL169: if(!((int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),
struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp22E,_tmp234))({struct Cyc_String_pa_struct _tmp23A;_tmp23A.tag=0;_tmp23A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp22E);{void*_tmp238[1]={& _tmp23A};
Cyc_Tcutil_terr(loc,({const char*_tmp239="no field of struct/union has name %s";
_tag_dyneither(_tmp239,sizeof(char),37);}),_tag_dyneither(_tmp238,sizeof(void*),
1));}});bad_type=0;goto _LL165;_LL16A:;_LL16B: goto _LL165;_LL165:;}if(bad_type)({
struct Cyc_String_pa_struct _tmp23D;_tmp23D.tag=0;_tmp23D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp23B[1]={& _tmp23D};Cyc_Tcutil_terr(
loc,({const char*_tmp23C="%s is not a known struct/union type";_tag_dyneither(
_tmp23C,sizeof(char),36);}),_tag_dyneither(_tmp23B,sizeof(void*),1));}});goto
_LL160;}_LL163: if(*((int*)_tmp22D)!= 1)goto _LL160;_tmp22F=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp22D)->f1;_LL164: {int bad_type=1;{void*_tmp23E=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_AggrInfo _tmp23F;union Cyc_Absyn_AggrInfoU_union _tmp240;struct Cyc_Absyn_Aggrdecl**
_tmp241;struct Cyc_List_List*_tmp242;struct Cyc_List_List*_tmp243;struct Cyc_Absyn_TunionFieldInfo
_tmp244;union Cyc_Absyn_TunionFieldInfoU_union _tmp245;struct Cyc_Absyn_Tunionfield*
_tmp246;_LL16D: if(_tmp23E <= (void*)4)goto _LL175;if(*((int*)_tmp23E)!= 10)goto
_LL16F;_tmp23F=((struct Cyc_Absyn_AggrType_struct*)_tmp23E)->f1;_tmp240=_tmp23F.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp23E)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL16F;_tmp241=(_tmp240.KnownAggr).f1;_LL16E: if((*_tmp241)->impl == 0)
goto _LL16C;_tmp242=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp241)->impl))->fields;
goto _LL170;_LL16F: if(*((int*)_tmp23E)!= 11)goto _LL171;_tmp242=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp23E)->f2;_LL170: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp242)
<= _tmp22F)({struct Cyc_Int_pa_struct _tmp24A;_tmp24A.tag=1;_tmp24A.f1=(
unsigned long)((int)_tmp22F);{struct Cyc_Int_pa_struct _tmp249;_tmp249.tag=1;
_tmp249.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp242);{void*_tmp247[2]={& _tmp249,& _tmp24A};Cyc_Tcutil_terr(loc,({const char*
_tmp248="struct/union has too few components: %d <= %d";_tag_dyneither(_tmp248,
sizeof(char),46);}),_tag_dyneither(_tmp247,sizeof(void*),2));}}});bad_type=0;
goto _LL16C;_LL171: if(*((int*)_tmp23E)!= 9)goto _LL173;_tmp243=((struct Cyc_Absyn_TupleType_struct*)
_tmp23E)->f1;_LL172: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp243)
<= _tmp22F)({struct Cyc_Int_pa_struct _tmp24E;_tmp24E.tag=1;_tmp24E.f1=(
unsigned long)((int)_tmp22F);{struct Cyc_Int_pa_struct _tmp24D;_tmp24D.tag=1;
_tmp24D.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp243);{void*_tmp24B[2]={& _tmp24D,& _tmp24E};Cyc_Tcutil_terr(loc,({const char*
_tmp24C="tuple has too few components: %d <= %d";_tag_dyneither(_tmp24C,sizeof(
char),39);}),_tag_dyneither(_tmp24B,sizeof(void*),2));}}});bad_type=0;goto _LL16C;
_LL173: if(*((int*)_tmp23E)!= 3)goto _LL175;_tmp244=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp23E)->f1;_tmp245=_tmp244.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp23E)->f1).field_info).KnownTunionfield).tag != 1)goto _LL175;_tmp246=(_tmp245.KnownTunionfield).f2;
_LL174: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp246->typs)< 
_tmp22F)({struct Cyc_Int_pa_struct _tmp252;_tmp252.tag=1;_tmp252.f1=(unsigned long)((
int)_tmp22F);{struct Cyc_Int_pa_struct _tmp251;_tmp251.tag=1;_tmp251.f1=(
unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp246->typs);{
void*_tmp24F[2]={& _tmp251,& _tmp252};Cyc_Tcutil_terr(loc,({const char*_tmp250="tunionfield has too few components: %d < %d";
_tag_dyneither(_tmp250,sizeof(char),44);}),_tag_dyneither(_tmp24F,sizeof(void*),
2));}}});bad_type=0;goto _LL16C;_LL175:;_LL176: goto _LL16C;_LL16C:;}if(bad_type)({
struct Cyc_String_pa_struct _tmp255;_tmp255.tag=0;_tmp255.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp253[1]={& _tmp255};Cyc_Tcutil_terr(
loc,({const char*_tmp254="%s is not a known type";_tag_dyneither(_tmp254,sizeof(
char),23);}),_tag_dyneither(_tmp253,sizeof(void*),1));}});goto _LL160;}_LL160:;}
return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*
te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){struct
_RegionHandle*_tmp256=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*_tmp257=
Cyc_Tcenv_clear_notreadctxt(_tmp256,te_orig);Cyc_Tcexp_tcExp(_tmp257,0,e);{void*
t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp258=t;struct Cyc_Absyn_PtrInfo _tmp259;void*_tmp25A;struct Cyc_Absyn_PtrAtts
_tmp25B;void*_tmp25C;struct Cyc_Absyn_Conref*_tmp25D;struct Cyc_Absyn_Conref*
_tmp25E;_LL178: if(_tmp258 <= (void*)4)goto _LL17A;if(*((int*)_tmp258)!= 4)goto
_LL17A;_tmp259=((struct Cyc_Absyn_PointerType_struct*)_tmp258)->f1;_tmp25A=(void*)
_tmp259.elt_typ;_tmp25B=_tmp259.ptr_atts;_tmp25C=(void*)_tmp25B.rgn;_tmp25D=
_tmp25B.bounds;_tmp25E=_tmp25B.zero_term;_LL179: Cyc_Tcenv_check_rgn_accessible(
_tmp257,loc,_tmp25C);Cyc_Tcutil_check_nonzero_bound(loc,_tmp25D);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp25A),(void*)1))({void*_tmp25F=0;Cyc_Tcutil_terr(loc,({
const char*_tmp260="can't dereference abstract pointer type";_tag_dyneither(
_tmp260,sizeof(char),40);}),_tag_dyneither(_tmp25F,sizeof(void*),0));});return
_tmp25A;_LL17A:;_LL17B: return({struct Cyc_String_pa_struct _tmp263;_tmp263.tag=0;
_tmp263.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{
void*_tmp261[1]={& _tmp263};Cyc_Tcexp_expr_err(_tmp257,loc,topt,({const char*
_tmp262="expecting * or @ type but found %s";_tag_dyneither(_tmp262,sizeof(char),
35);}),_tag_dyneither(_tmp261,sizeof(void*),1));}});_LL177:;}}static void*Cyc_Tcexp_tcAggrMember(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){Cyc_Tcexp_tcExpNoPromote(
te,0,e);{void*_tmp264=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo _tmp265;union Cyc_Absyn_AggrInfoU_union
_tmp266;struct Cyc_Absyn_Aggrdecl**_tmp267;struct Cyc_Absyn_Aggrdecl*_tmp268;
struct Cyc_List_List*_tmp269;void*_tmp26A;struct Cyc_List_List*_tmp26B;_LL17D: if(
_tmp264 <= (void*)4)goto _LL185;if(*((int*)_tmp264)!= 10)goto _LL17F;_tmp265=((
struct Cyc_Absyn_AggrType_struct*)_tmp264)->f1;_tmp266=_tmp265.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmp264)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL17F;_tmp267=(_tmp266.KnownAggr).f1;_tmp268=*_tmp267;_tmp269=_tmp265.targs;
_LL17E: {struct Cyc_Absyn_Aggrfield*_tmp26E=Cyc_Absyn_lookup_decl_field(_tmp268,f);
if(_tmp26E == 0)return({struct Cyc_String_pa_struct _tmp272;_tmp272.tag=0;_tmp272.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f);{struct Cyc_String_pa_struct
_tmp271;_tmp271.tag=0;_tmp271.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp268->name));{void*_tmp26F[2]={& _tmp271,& _tmp272};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp270="type %s has no %s field";_tag_dyneither(_tmp270,
sizeof(char),24);}),_tag_dyneither(_tmp26F,sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp268->impl))->exist_vars != 0)return({void*_tmp273=0;Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp274="must use pattern-matching to access fields of existential types";
_tag_dyneither(_tmp274,sizeof(char),64);}),_tag_dyneither(_tmp273,sizeof(void*),
0));});{void*t2;struct _RegionHandle*_tmp275=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
_tmp276=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp275,_tmp275,
_tmp268->tvs,_tmp269);t2=Cyc_Tcutil_rsubstitute(_tmp275,_tmp276,(void*)_tmp26E->type);
if(((void*)_tmp268->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t2))return({struct Cyc_String_pa_struct _tmp279;_tmp279.tag=0;_tmp279.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp277[1]={& _tmp279};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp278="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp278,sizeof(char),56);}),_tag_dyneither(_tmp277,sizeof(void*),
1));}});}return t2;}}_LL17F: if(*((int*)_tmp264)!= 11)goto _LL181;_tmp26A=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp264)->f1;_tmp26B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp264)->f2;_LL180: {struct Cyc_Absyn_Aggrfield*_tmp27A=Cyc_Absyn_lookup_field(
_tmp26B,f);if(_tmp27A == 0)return({struct Cyc_String_pa_struct _tmp27D;_tmp27D.tag=
0;_tmp27D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp27B[1]={&
_tmp27D};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp27C="type has no %s field";
_tag_dyneither(_tmp27C,sizeof(char),21);}),_tag_dyneither(_tmp27B,sizeof(void*),
1));}});if((_tmp26A == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp27A->type))return({struct Cyc_String_pa_struct _tmp280;_tmp280.tag=0;
_tmp280.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp27E[1]={&
_tmp280};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp27F="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp27F,sizeof(char),56);}),_tag_dyneither(_tmp27E,sizeof(void*),
1));}});return(void*)_tmp27A->type;}_LL181: if(*((int*)_tmp264)!= 7)goto _LL183;
if(!(Cyc_strcmp((struct _dyneither_ptr)*f,({const char*_tmp26C="size";
_tag_dyneither(_tmp26C,sizeof(char),5);}))== 0))goto _LL183;_LL182: goto _LL184;
_LL183: if(*((int*)_tmp264)!= 4)goto _LL185;if(!(Cyc_strcmp((struct _dyneither_ptr)*
f,({const char*_tmp26D="size";_tag_dyneither(_tmp26D,sizeof(char),5);}))== 0))
goto _LL185;_LL184:({struct Cyc_String_pa_struct _tmp283;_tmp283.tag=0;_tmp283.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{void*
_tmp281[1]={& _tmp283};Cyc_Tcutil_warn(e->loc,({const char*_tmp282="deprecated `.size' used here -- change to numelts(%s)";
_tag_dyneither(_tmp282,sizeof(char),54);}),_tag_dyneither(_tmp281,sizeof(void*),
1));}});(void*)(outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp284=_cycalloc(sizeof(*_tmp284));_tmp284[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp285;_tmp285.tag=3;_tmp285.f1=(void*)((void*)19);_tmp285.f2=({struct Cyc_List_List*
_tmp286=_cycalloc(sizeof(*_tmp286));_tmp286->hd=e;_tmp286->tl=0;_tmp286;});
_tmp285;});_tmp284;})));return Cyc_Absyn_uint_typ;_LL185:;_LL186: if(Cyc_strcmp((
struct _dyneither_ptr)*f,({const char*_tmp287="size";_tag_dyneither(_tmp287,
sizeof(char),5);}))== 0)return({struct Cyc_String_pa_struct _tmp28A;_tmp28A.tag=0;
_tmp28A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp288[1]={&
_tmp28A};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp289="expecting struct, union, or array, found %s";
_tag_dyneither(_tmp289,sizeof(char),44);}),_tag_dyneither(_tmp288,sizeof(void*),
1));}});else{return({struct Cyc_String_pa_struct _tmp28D;_tmp28D.tag=0;_tmp28D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp28B[1]={& _tmp28D};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp28C="expecting struct or union, found %s";
_tag_dyneither(_tmp28C,sizeof(char),36);}),_tag_dyneither(_tmp28B,sizeof(void*),
1));}});}_LL17C:;}}static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f){struct _RegionHandle*_tmp28E=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExp(
Cyc_Tcenv_clear_notreadctxt(_tmp28E,te),0,e);{void*_tmp28F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp290;void*_tmp291;struct Cyc_Absyn_PtrAtts _tmp292;void*_tmp293;struct Cyc_Absyn_Conref*
_tmp294;struct Cyc_Absyn_Conref*_tmp295;_LL188: if(_tmp28F <= (void*)4)goto _LL18A;
if(*((int*)_tmp28F)!= 4)goto _LL18A;_tmp290=((struct Cyc_Absyn_PointerType_struct*)
_tmp28F)->f1;_tmp291=(void*)_tmp290.elt_typ;_tmp292=_tmp290.ptr_atts;_tmp293=(
void*)_tmp292.rgn;_tmp294=_tmp292.bounds;_tmp295=_tmp292.zero_term;_LL189: Cyc_Tcutil_check_nonzero_bound(
loc,_tmp294);{void*_tmp296=Cyc_Tcutil_compress(_tmp291);struct Cyc_Absyn_AggrInfo
_tmp297;union Cyc_Absyn_AggrInfoU_union _tmp298;struct Cyc_Absyn_Aggrdecl**_tmp299;
struct Cyc_Absyn_Aggrdecl*_tmp29A;struct Cyc_List_List*_tmp29B;void*_tmp29C;struct
Cyc_List_List*_tmp29D;_LL18D: if(_tmp296 <= (void*)4)goto _LL191;if(*((int*)_tmp296)
!= 10)goto _LL18F;_tmp297=((struct Cyc_Absyn_AggrType_struct*)_tmp296)->f1;_tmp298=
_tmp297.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp296)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL18F;_tmp299=(_tmp298.KnownAggr).f1;_tmp29A=*_tmp299;_tmp29B=_tmp297.targs;
_LL18E: {struct Cyc_Absyn_Aggrfield*_tmp29E=Cyc_Absyn_lookup_decl_field(_tmp29A,f);
if(_tmp29E == 0)return({struct Cyc_String_pa_struct _tmp2A2;_tmp2A2.tag=0;_tmp2A2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f);{struct Cyc_String_pa_struct
_tmp2A1;_tmp2A1.tag=0;_tmp2A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp29A->name));{void*_tmp29F[2]={& _tmp2A1,& _tmp2A2};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2A0="type %s has no %s field";_tag_dyneither(_tmp2A0,
sizeof(char),24);}),_tag_dyneither(_tmp29F,sizeof(void*),2));}}});if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp29A->impl))->exist_vars != 0)return({void*_tmp2A3=0;Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2A4="must use pattern-matching to access fields of existential types";
_tag_dyneither(_tmp2A4,sizeof(char),64);}),_tag_dyneither(_tmp2A3,sizeof(void*),
0));});{void*t3;struct _RegionHandle*_tmp2A5=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
_tmp2A6=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp2A5,_tmp2A5,
_tmp29A->tvs,_tmp29B);t3=Cyc_Tcutil_rsubstitute(_tmp2A5,_tmp2A6,(void*)_tmp29E->type);}
if(((void*)_tmp29A->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t3))return({struct Cyc_String_pa_struct _tmp2A9;_tmp2A9.tag=0;_tmp2A9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp2A7[1]={& _tmp2A9};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp2A8="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp2A8,sizeof(char),56);}),_tag_dyneither(_tmp2A7,sizeof(void*),
1));}});return t3;}}_LL18F: if(*((int*)_tmp296)!= 11)goto _LL191;_tmp29C=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp296)->f1;_tmp29D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp296)->f2;_LL190: {struct Cyc_Absyn_Aggrfield*_tmp2AA=Cyc_Absyn_lookup_field(
_tmp29D,f);if(_tmp2AA == 0)return({struct Cyc_String_pa_struct _tmp2AD;_tmp2AD.tag=
0;_tmp2AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp2AB[1]={&
_tmp2AD};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2AC="type has no %s field";
_tag_dyneither(_tmp2AC,sizeof(char),21);}),_tag_dyneither(_tmp2AB,sizeof(void*),
1));}});if((_tmp29C == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp2AA->type))return({struct Cyc_String_pa_struct _tmp2B0;_tmp2B0.tag=0;
_tmp2B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp2AE[1]={&
_tmp2B0};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2AF="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp2AF,sizeof(char),56);}),_tag_dyneither(_tmp2AE,sizeof(void*),
1));}});return(void*)_tmp2AA->type;}_LL191:;_LL192: goto _LL18C;_LL18C:;}goto
_LL187;_LL18A:;_LL18B: goto _LL187;_LL187:;}return({struct Cyc_String_pa_struct
_tmp2B3;_tmp2B3.tag=0;_tmp2B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*_tmp2B1[1]={& _tmp2B3};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2B2="expecting struct or union pointer, found %s";
_tag_dyneither(_tmp2B2,sizeof(char),44);}),_tag_dyneither(_tmp2B1,sizeof(void*),
1));}});}static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){unsigned int _tmp2B5;int
_tmp2B6;struct _tuple7 _tmp2B4=Cyc_Evexp_eval_const_uint_exp(index);_tmp2B5=
_tmp2B4.f1;_tmp2B6=_tmp2B4.f2;if(!_tmp2B6)return({void*_tmp2B7=0;Cyc_Tcexp_expr_err(
te,loc,0,({const char*_tmp2B8="tuple projection cannot use sizeof or offsetof";
_tag_dyneither(_tmp2B8,sizeof(char),47);}),_tag_dyneither(_tmp2B7,sizeof(void*),
0));});{struct _handler_cons _tmp2B9;_push_handler(& _tmp2B9);{int _tmp2BB=0;if(
setjmp(_tmp2B9.handler))_tmp2BB=1;if(!_tmp2BB){{void*_tmp2BC=(*((struct _tuple9*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2B5)).f2;_npop_handler(0);
return _tmp2BC;};_pop_handler();}else{void*_tmp2BA=(void*)_exn_thrown;void*
_tmp2BE=_tmp2BA;_LL194: if(_tmp2BE != Cyc_List_Nth)goto _LL196;_LL195: return({
struct Cyc_Int_pa_struct _tmp2C2;_tmp2C2.tag=1;_tmp2C2.f1=(unsigned long)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(ts);{struct Cyc_Int_pa_struct _tmp2C1;
_tmp2C1.tag=1;_tmp2C1.f1=(unsigned long)((int)_tmp2B5);{void*_tmp2BF[2]={&
_tmp2C1,& _tmp2C2};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp2C0="index is %d but tuple has only %d fields";
_tag_dyneither(_tmp2C0,sizeof(char),41);}),_tag_dyneither(_tmp2BF,sizeof(void*),
2));}}});_LL196:;_LL197:(void)_throw(_tmp2BE);_LL193:;}}}}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp2C3=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp2C4=Cyc_Tcenv_clear_notreadctxt(_tmp2C3,
te_orig);Cyc_Tcexp_tcExp(_tmp2C4,0,e1);Cyc_Tcexp_tcExp(_tmp2C4,0,e2);{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2C4,e2))return({struct Cyc_String_pa_struct
_tmp2C7;_tmp2C7.tag=0;_tmp2C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));{void*_tmp2C5[1]={& _tmp2C7};Cyc_Tcexp_expr_err(
_tmp2C4,e2->loc,topt,({const char*_tmp2C6="expecting int subscript, found %s";
_tag_dyneither(_tmp2C6,sizeof(char),34);}),_tag_dyneither(_tmp2C5,sizeof(void*),
1));}});{void*_tmp2C8=t1;struct Cyc_Absyn_PtrInfo _tmp2C9;void*_tmp2CA;struct Cyc_Absyn_Tqual
_tmp2CB;struct Cyc_Absyn_PtrAtts _tmp2CC;void*_tmp2CD;struct Cyc_Absyn_Conref*
_tmp2CE;struct Cyc_Absyn_Conref*_tmp2CF;struct Cyc_List_List*_tmp2D0;_LL199: if(
_tmp2C8 <= (void*)4)goto _LL19D;if(*((int*)_tmp2C8)!= 4)goto _LL19B;_tmp2C9=((
struct Cyc_Absyn_PointerType_struct*)_tmp2C8)->f1;_tmp2CA=(void*)_tmp2C9.elt_typ;
_tmp2CB=_tmp2C9.elt_tq;_tmp2CC=_tmp2C9.ptr_atts;_tmp2CD=(void*)_tmp2CC.rgn;
_tmp2CE=_tmp2CC.bounds;_tmp2CF=_tmp2CC.zero_term;_LL19A: if(((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2CF)){int emit_warning=1;{union
Cyc_Absyn_Constraint_union _tmp2D1=(Cyc_Absyn_compress_conref(_tmp2CE))->v;void*
_tmp2D2;struct Cyc_Absyn_Exp*_tmp2D3;void*_tmp2D4;_LL1A0: if((_tmp2D1.Eq_constr).tag
!= 0)goto _LL1A2;_tmp2D2=(_tmp2D1.Eq_constr).f1;if(_tmp2D2 <= (void*)1)goto _LL1A2;
if(*((int*)_tmp2D2)!= 0)goto _LL1A2;_tmp2D3=((struct Cyc_Absyn_Upper_b_struct*)
_tmp2D2)->f1;_LL1A1: if(Cyc_Tcutil_is_const_exp(_tmp2C4,e2)){unsigned int _tmp2D6;
int _tmp2D7;struct _tuple7 _tmp2D5=Cyc_Evexp_eval_const_uint_exp(e2);_tmp2D6=
_tmp2D5.f1;_tmp2D7=_tmp2D5.f2;if(_tmp2D7){unsigned int _tmp2D9;int _tmp2DA;struct
_tuple7 _tmp2D8=Cyc_Evexp_eval_const_uint_exp(_tmp2D3);_tmp2D9=_tmp2D8.f1;_tmp2DA=
_tmp2D8.f2;if(_tmp2DA  && _tmp2D9 > _tmp2D6)emit_warning=0;}}goto _LL19F;_LL1A2: if((
_tmp2D1.Eq_constr).tag != 0)goto _LL1A4;_tmp2D4=(_tmp2D1.Eq_constr).f1;if((int)
_tmp2D4 != 0)goto _LL1A4;_LL1A3: emit_warning=0;goto _LL19F;_LL1A4:;_LL1A5: goto
_LL19F;_LL19F:;}if(emit_warning)({void*_tmp2DB=0;Cyc_Tcutil_warn(e2->loc,({const
char*_tmp2DC="subscript on thin, zero-terminated pointer could be expensive.";
_tag_dyneither(_tmp2DC,sizeof(char),63);}),_tag_dyneither(_tmp2DB,sizeof(void*),
0));});}else{if(Cyc_Tcutil_is_const_exp(_tmp2C4,e2)){unsigned int _tmp2DE;int
_tmp2DF;struct _tuple7 _tmp2DD=Cyc_Evexp_eval_const_uint_exp(e2);_tmp2DE=_tmp2DD.f1;
_tmp2DF=_tmp2DD.f2;if(_tmp2DF)Cyc_Tcutil_check_bound(loc,_tmp2DE,_tmp2CE);}else{
if(Cyc_Tcutil_is_bound_one(_tmp2CE) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp2CF))({void*_tmp2E0=0;Cyc_Tcutil_warn(e1->loc,({
const char*_tmp2E1="subscript applied to pointer to one object";_tag_dyneither(
_tmp2E1,sizeof(char),43);}),_tag_dyneither(_tmp2E0,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(
loc,_tmp2CE);}}Cyc_Tcenv_check_rgn_accessible(_tmp2C4,loc,_tmp2CD);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp2CA),(void*)1))({void*_tmp2E2=0;Cyc_Tcutil_terr(e1->loc,({
const char*_tmp2E3="can't subscript an abstract pointer";_tag_dyneither(_tmp2E3,
sizeof(char),36);}),_tag_dyneither(_tmp2E2,sizeof(void*),0));});return _tmp2CA;
_LL19B: if(*((int*)_tmp2C8)!= 9)goto _LL19D;_tmp2D0=((struct Cyc_Absyn_TupleType_struct*)
_tmp2C8)->f1;_LL19C: return Cyc_Tcexp_ithTupleType(_tmp2C4,loc,_tmp2D0,e2);_LL19D:;
_LL19E: return({struct Cyc_String_pa_struct _tmp2E6;_tmp2E6.tag=0;_tmp2E6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{void*
_tmp2E4[1]={& _tmp2E6};Cyc_Tcexp_expr_err(_tmp2C4,loc,topt,({const char*_tmp2E5="subscript applied to %s";
_tag_dyneither(_tmp2E5,sizeof(char),24);}),_tag_dyneither(_tmp2E4,sizeof(void*),
1));}});_LL198:;}}}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*
fields=0;if(topt != 0){void*_tmp2E7=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*
_tmp2E8;_LL1A7: if(_tmp2E7 <= (void*)4)goto _LL1A9;if(*((int*)_tmp2E7)!= 9)goto
_LL1A9;_tmp2E8=((struct Cyc_Absyn_TupleType_struct*)_tmp2E7)->f1;_LL1A8: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2E8)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1A6;for(0;es != 0;(es=es->tl,_tmp2E8=_tmp2E8->tl)){
void*_tmp2E9=(*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp2E8))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp2E9,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple9*)_tmp2E8->hd)).f2);fields=({
struct Cyc_List_List*_tmp2EA=_cycalloc(sizeof(*_tmp2EA));_tmp2EA->hd=({struct
_tuple9*_tmp2EB=_cycalloc(sizeof(*_tmp2EB));_tmp2EB->f1=(*((struct _tuple9*)
_tmp2E8->hd)).f1;_tmp2EB->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp2EB;});_tmp2EA->tl=fields;_tmp2EA;});}done=
1;goto _LL1A6;_LL1A9:;_LL1AA: goto _LL1A6;_LL1A6:;}if(!done)for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*
_tmp2EC=_cycalloc(sizeof(*_tmp2EC));_tmp2EC->hd=({struct _tuple9*_tmp2ED=
_cycalloc(sizeof(*_tmp2ED));_tmp2ED->f1=Cyc_Absyn_empty_tqual(0);_tmp2ED->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp2ED;});_tmp2EC->tl=fields;_tmp2EC;});}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp2EE=_cycalloc(sizeof(*_tmp2EE));_tmp2EE[0]=({struct Cyc_Absyn_TupleType_struct
_tmp2EF;_tmp2EF.tag=9;_tmp2EF.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(fields);_tmp2EF;});_tmp2EE;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*
t,struct Cyc_List_List*des){return({void*_tmp2F0=0;Cyc_Tcexp_expr_err(te,loc,topt,({
const char*_tmp2F1="tcCompoundLit";_tag_dyneither(_tmp2F1,sizeof(char),14);}),
_tag_dyneither(_tmp2F0,sizeof(void*),0));});}static void*Cyc_Tcexp_tcArray(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**elt_topt,int zero_term,
struct Cyc_List_List*des){void*res_t2;struct _RegionHandle*_tmp2F2=Cyc_Tcenv_get_fnrgn(
te);{int _tmp2F3=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple8*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp2F2,(struct Cyc_Absyn_Exp*(*)(
struct _tuple8*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Const_e_struct*
_tmp2F4=({struct Cyc_Absyn_Const_e_struct*_tmp2FC=_cycalloc(sizeof(*_tmp2FC));
_tmp2FC[0]=({struct Cyc_Absyn_Const_e_struct _tmp2FD;_tmp2FD.tag=0;_tmp2FD.f1=(
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp2FE;(_tmp2FE.Int_c).tag=
2;(_tmp2FE.Int_c).f1=(void*)((void*)1);(_tmp2FE.Int_c).f2=_tmp2F3;_tmp2FE;});
_tmp2FD;});_tmp2FC;});struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)
_tmp2F4,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp2F5=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp2F3 - 1);if(!Cyc_Tcutil_is_zero(
_tmp2F5))({void*_tmp2F6=0;Cyc_Tcutil_terr(_tmp2F5->loc,({const char*_tmp2F7="zero-terminated array doesn't end with zero.";
_tag_dyneither(_tmp2F7,sizeof(char),45);}),_tag_dyneither(_tmp2F6,sizeof(void*),
0));});}sz_exp->topt=({struct Cyc_Core_Opt*_tmp2F8=_cycalloc(sizeof(*_tmp2F8));
_tmp2F8->v=(void*)Cyc_Absyn_uint_typ;_tmp2F8;});res_t2=Cyc_Absyn_array_typ(res,
Cyc_Absyn_empty_tqual(0),(struct Cyc_Absyn_Exp*)sz_exp,zero_term?Cyc_Absyn_true_conref:
Cyc_Absyn_false_conref,0);{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(
te,res,es))({struct Cyc_String_pa_struct _tmp2FB;_tmp2FB.tag=0;_tmp2FB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(res));{void*_tmp2F9[
1]={& _tmp2FB};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,({const char*_tmp2FA="elements of array do not all have the same type (%s)";
_tag_dyneither(_tmp2FA,sizeof(char),53);}),_tag_dyneither(_tmp2F9,sizeof(void*),
1));}});}{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*
ds=(*((struct _tuple8*)des->hd)).f1;if(ds != 0){void*_tmp2FF=(void*)ds->hd;struct
Cyc_Absyn_Exp*_tmp300;_LL1AC: if(*((int*)_tmp2FF)!= 1)goto _LL1AE;_LL1AD:({void*
_tmp301=0;Cyc_Tcutil_terr(loc,({const char*_tmp302="only array index designators are supported";
_tag_dyneither(_tmp302,sizeof(char),43);}),_tag_dyneither(_tmp301,sizeof(void*),
0));});goto _LL1AB;_LL1AE: if(*((int*)_tmp2FF)!= 0)goto _LL1AB;_tmp300=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp2FF)->f1;_LL1AF: Cyc_Tcexp_tcExpInitializer(te,0,_tmp300);{unsigned int
_tmp304;int _tmp305;struct _tuple7 _tmp303=Cyc_Evexp_eval_const_uint_exp(_tmp300);
_tmp304=_tmp303.f1;_tmp305=_tmp303.f2;if(!_tmp305)({void*_tmp306=0;Cyc_Tcutil_terr(
_tmp300->loc,({const char*_tmp307="index designator cannot use sizeof or offsetof";
_tag_dyneither(_tmp307,sizeof(char),47);}),_tag_dyneither(_tmp306,sizeof(void*),
0));});else{if(_tmp304 != offset)({struct Cyc_Int_pa_struct _tmp30B;_tmp30B.tag=1;
_tmp30B.f1=(unsigned long)((int)_tmp304);{struct Cyc_Int_pa_struct _tmp30A;_tmp30A.tag=
1;_tmp30A.f1=(unsigned long)offset;{void*_tmp308[2]={& _tmp30A,& _tmp30B};Cyc_Tcutil_terr(
_tmp300->loc,({const char*_tmp309="expecting index designator %d but found %d";
_tag_dyneither(_tmp309,sizeof(char),43);}),_tag_dyneither(_tmp308,sizeof(void*),
2));}}});}goto _LL1AB;}_LL1AB:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp30C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1B1: if(_tmp30C <= (void*)4)goto _LL1B3;if(*((int*)
_tmp30C)!= 18)goto _LL1B3;_LL1B2: goto _LL1B0;_LL1B3:;_LL1B4: if(!Cyc_Tcutil_coerce_uint_typ(
te,bound))({struct Cyc_String_pa_struct _tmp30F;_tmp30F.tag=0;_tmp30F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v));{void*_tmp30D[1]={& _tmp30F};Cyc_Tcutil_terr(bound->loc,({
const char*_tmp30E="expecting unsigned int, found %s";_tag_dyneither(_tmp30E,
sizeof(char),33);}),_tag_dyneither(_tmp30D,sizeof(void*),1));}});_LL1B0:;}if(!(
vd->tq).real_const)({void*_tmp310=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp311="comprehension index variable is not declared const!";
_tag_dyneither(_tmp311,sizeof(char),52);}),_tag_dyneither(_tmp310,sizeof(void*),
0));});{struct _RegionHandle*_tmp312=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*
te2=Cyc_Tcenv_copy_tenv(_tmp312,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(
_tmp312,loc,te2);te2=Cyc_Tcenv_add_local_var(_tmp312,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*
_tmp313=te2;struct Cyc_Absyn_PtrInfo pinfo;void**_tmp314=0;struct Cyc_Absyn_Tqual*
_tmp315=0;struct Cyc_Absyn_Conref**_tmp316=0;if(topt != 0){void*_tmp317=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp318;struct Cyc_Absyn_ArrayInfo _tmp319;void*
_tmp31A;void**_tmp31B;struct Cyc_Absyn_Tqual _tmp31C;struct Cyc_Absyn_Tqual*_tmp31D;
struct Cyc_Absyn_Exp*_tmp31E;struct Cyc_Absyn_Conref*_tmp31F;struct Cyc_Absyn_Conref**
_tmp320;_LL1B6: if(_tmp317 <= (void*)4)goto _LL1BA;if(*((int*)_tmp317)!= 4)goto
_LL1B8;_tmp318=((struct Cyc_Absyn_PointerType_struct*)_tmp317)->f1;_LL1B7: pinfo=
_tmp318;_tmp314=(void**)((void**)((void*)& pinfo.elt_typ));_tmp315=(struct Cyc_Absyn_Tqual*)&
pinfo.elt_tq;_tmp316=(struct Cyc_Absyn_Conref**)&(pinfo.ptr_atts).zero_term;goto
_LL1B5;_LL1B8: if(*((int*)_tmp317)!= 7)goto _LL1BA;_tmp319=((struct Cyc_Absyn_ArrayType_struct*)
_tmp317)->f1;_tmp31A=(void*)_tmp319.elt_type;_tmp31B=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp317)->f1).elt_type;_tmp31C=_tmp319.tq;_tmp31D=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp317)->f1).tq;_tmp31E=_tmp319.num_elts;
_tmp31F=_tmp319.zero_term;_tmp320=(struct Cyc_Absyn_Conref**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp317)->f1).zero_term;_LL1B9: _tmp314=(void**)_tmp31B;_tmp315=(struct Cyc_Absyn_Tqual*)
_tmp31D;_tmp316=(struct Cyc_Absyn_Conref**)_tmp320;goto _LL1B5;_LL1BA:;_LL1BB: goto
_LL1B5;_LL1B5:;}{void*t=Cyc_Tcexp_tcExp(_tmp313,_tmp314,body);if(_tmp313->le == 0){
if(!Cyc_Tcutil_is_const_exp(_tmp313,bound))({void*_tmp321=0;Cyc_Tcutil_terr(
bound->loc,({const char*_tmp322="bound is not constant";_tag_dyneither(_tmp322,
sizeof(char),22);}),_tag_dyneither(_tmp321,sizeof(void*),0));});if(!Cyc_Tcutil_is_const_exp(
_tmp313,body))({void*_tmp323=0;Cyc_Tcutil_terr(bound->loc,({const char*_tmp324="body is not constant";
_tag_dyneither(_tmp324,sizeof(char),21);}),_tag_dyneither(_tmp323,sizeof(void*),
0));});}if(_tmp316 != 0  && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,*_tmp316)){struct Cyc_Absyn_Exp*_tmp325=Cyc_Absyn_uint_exp(1,0);_tmp325->topt=({
struct Cyc_Core_Opt*_tmp326=_cycalloc(sizeof(*_tmp326));_tmp326->v=(void*)Cyc_Absyn_uint_typ;
_tmp326;});bound=Cyc_Absyn_add_exp(bound,_tmp325,0);bound->topt=({struct Cyc_Core_Opt*
_tmp327=_cycalloc(sizeof(*_tmp327));_tmp327->v=(void*)Cyc_Absyn_uint_typ;_tmp327;});*
is_zero_term=1;}{void*_tmp328=Cyc_Absyn_array_typ(t,_tmp315 == 0?Cyc_Absyn_empty_tqual(
0):*_tmp315,(struct Cyc_Absyn_Exp*)bound,_tmp316 == 0?Cyc_Absyn_false_conref:*
_tmp316,0);return _tmp328;}}}}}struct _tuple10{struct Cyc_Absyn_Aggrfield*f1;struct
Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,
struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*
ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{
struct _handler_cons _tmp329;_push_handler(& _tmp329);{int _tmp32B=0;if(setjmp(
_tmp329.handler))_tmp32B=1;if(!_tmp32B){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;
_pop_handler();}else{void*_tmp32A=(void*)_exn_thrown;void*_tmp32D=_tmp32A;_LL1BD:
if(_tmp32D != Cyc_Dict_Absent)goto _LL1BF;_LL1BE:({struct Cyc_String_pa_struct
_tmp330;_tmp330.tag=0;_tmp330.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(*tn));{void*_tmp32E[1]={& _tmp330};Cyc_Tcutil_terr(loc,({
const char*_tmp32F="unbound struct name %s";_tag_dyneither(_tmp32F,sizeof(char),
23);}),_tag_dyneither(_tmp32E,sizeof(void*),1));}});return topt != 0?*topt:(void*)
0;_LL1BF:;_LL1C0:(void)_throw(_tmp32D);_LL1BC:;}}}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)
ad;*tn=ad->name;}if((void*)ad->kind == (void*)1)({void*_tmp331=0;Cyc_Tcutil_terr(
loc,({const char*_tmp332="expecting struct but found union";_tag_dyneither(
_tmp332,sizeof(char),33);}),_tag_dyneither(_tmp331,sizeof(void*),0));});if(ad->impl
== 0){({void*_tmp333=0;Cyc_Tcutil_terr(loc,({const char*_tmp334="can't build abstract struct";
_tag_dyneither(_tmp334,sizeof(char),28);}),_tag_dyneither(_tmp333,sizeof(void*),
0));});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{struct
_RegionHandle*_tmp335=Cyc_Tcenv_get_fnrgn(te);struct _tuple4 _tmp336=({struct
_tuple4 _tmp355;_tmp355.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp355.f2=_tmp335;
_tmp355;});struct Cyc_List_List*_tmp337=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp335,Cyc_Tcutil_r_make_inst_var,&
_tmp336,ad->tvs);struct Cyc_List_List*_tmp338=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp335,Cyc_Tcutil_r_make_inst_var,&
_tmp336,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp339=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp337);struct Cyc_List_List*_tmp33A=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp338);struct Cyc_List_List*_tmp33B=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp335,_tmp337,_tmp338);struct Cyc_Absyn_AggrType_struct*_tmp33C=({struct Cyc_Absyn_AggrType_struct*
_tmp350=_cycalloc(sizeof(*_tmp350));_tmp350[0]=({struct Cyc_Absyn_AggrType_struct
_tmp351;_tmp351.tag=10;_tmp351.f1=({struct Cyc_Absyn_AggrInfo _tmp352;_tmp352.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp353;(_tmp353.KnownAggr).tag=
1;(_tmp353.KnownAggr).f1=({struct Cyc_Absyn_Aggrdecl**_tmp354=_cycalloc(sizeof(*
_tmp354));_tmp354[0]=ad;_tmp354;});_tmp353;});_tmp352.targs=_tmp339;_tmp352;});
_tmp351;});_tmp350;});struct Cyc_List_List*_tmp33D=*ts;struct Cyc_List_List*
_tmp33E=_tmp33A;while(_tmp33D != 0  && _tmp33E != 0){Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),(void*)0,1,(void*)_tmp33D->hd);Cyc_Tcutil_unify((
void*)_tmp33D->hd,(void*)_tmp33E->hd);_tmp33D=_tmp33D->tl;_tmp33E=_tmp33E->tl;}
if(_tmp33D != 0)({void*_tmp33F=0;Cyc_Tcutil_terr(loc,({const char*_tmp340="too many explicit witness types";
_tag_dyneither(_tmp340,sizeof(char),32);}),_tag_dyneither(_tmp33F,sizeof(void*),
0));});*ts=_tmp33A;if(topt != 0)Cyc_Tcutil_unify((void*)_tmp33C,*topt);{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
_tmp335,loc,args,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){struct _tuple10 _tmp342;struct Cyc_Absyn_Aggrfield*
_tmp343;struct Cyc_Absyn_Exp*_tmp344;struct _tuple10*_tmp341=(struct _tuple10*)
fields->hd;_tmp342=*_tmp341;_tmp343=_tmp342.f1;_tmp344=_tmp342.f2;{void*_tmp345=
Cyc_Tcutil_rsubstitute(_tmp335,_tmp33B,(void*)_tmp343->type);Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp345,_tmp344);if(!Cyc_Tcutil_coerce_arg(te,_tmp344,_tmp345)){({
struct Cyc_String_pa_struct _tmp34B;_tmp34B.tag=0;_tmp34B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp344->topt))->v));{struct Cyc_String_pa_struct _tmp34A;_tmp34A.tag=
0;_tmp34A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp345));{struct Cyc_String_pa_struct _tmp349;_tmp349.tag=0;_tmp349.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));{struct Cyc_String_pa_struct
_tmp348;_tmp348.tag=0;_tmp348.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp343->name);{void*_tmp346[4]={& _tmp348,& _tmp349,& _tmp34A,& _tmp34B};Cyc_Tcutil_terr(
_tmp344->loc,({const char*_tmp347="field %s of struct %s expects type %s != %s";
_tag_dyneither(_tmp347,sizeof(char),44);}),_tag_dyneither(_tmp346,sizeof(void*),
4));}}}}});Cyc_Tcutil_explain_failure();}}}{struct Cyc_List_List*_tmp34C=0;{
struct Cyc_List_List*_tmp34D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;
for(0;_tmp34D != 0;_tmp34D=_tmp34D->tl){_tmp34C=({struct Cyc_List_List*_tmp34E=
_cycalloc(sizeof(*_tmp34E));_tmp34E->hd=({struct _tuple0*_tmp34F=_cycalloc(
sizeof(*_tmp34F));_tmp34F->f1=Cyc_Tcutil_rsubstitute(_tmp335,_tmp33B,(*((struct
_tuple0*)_tmp34D->hd)).f1);_tmp34F->f2=Cyc_Tcutil_rsubstitute(_tmp335,_tmp33B,(*((
struct _tuple0*)_tmp34D->hd)).f2);_tmp34F;});_tmp34E->tl=_tmp34C;_tmp34E;});}}
_tmp34C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp34C);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp34C);return(void*)_tmp33C;}}}}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){struct _RegionHandle*_tmp356=Cyc_Tcenv_get_fnrgn(
te);{void*_tmp357=Cyc_Tcutil_compress(ts);void*_tmp358;struct Cyc_List_List*
_tmp359;_LL1C2: if(_tmp357 <= (void*)4)goto _LL1C4;if(*((int*)_tmp357)!= 11)goto
_LL1C4;_tmp358=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp357)->f1;
_tmp359=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp357)->f2;_LL1C3: if(_tmp358 == (
void*)1)({void*_tmp35A=0;Cyc_Tcutil_terr(loc,({const char*_tmp35B="expecting struct but found union";
_tag_dyneither(_tmp35B,sizeof(char),33);}),_tag_dyneither(_tmp35A,sizeof(void*),
0));});{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*
rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(_tmp356,loc,args,_tmp359);for(0;
fields != 0;fields=fields->tl){struct _tuple10 _tmp35D;struct Cyc_Absyn_Aggrfield*
_tmp35E;struct Cyc_Absyn_Exp*_tmp35F;struct _tuple10*_tmp35C=(struct _tuple10*)
fields->hd;_tmp35D=*_tmp35C;_tmp35E=_tmp35D.f1;_tmp35F=_tmp35D.f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)((void**)((void*)& _tmp35E->type)),_tmp35F);if(!Cyc_Tcutil_coerce_arg(
te,_tmp35F,(void*)_tmp35E->type)){({struct Cyc_String_pa_struct _tmp364;_tmp364.tag=
0;_tmp364.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp35F->topt))->v));{struct Cyc_String_pa_struct
_tmp363;_tmp363.tag=0;_tmp363.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_tmp35E->type));{struct Cyc_String_pa_struct _tmp362;
_tmp362.tag=0;_tmp362.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp35E->name);{
void*_tmp360[3]={& _tmp362,& _tmp363,& _tmp364};Cyc_Tcutil_terr(_tmp35F->loc,({
const char*_tmp361="field %s of struct expects type %s != %s";_tag_dyneither(
_tmp361,sizeof(char),41);}),_tag_dyneither(_tmp360,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
goto _LL1C1;}_LL1C4:;_LL1C5:({void*_tmp365=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp366="tcAnonStruct: wrong type";
_tag_dyneither(_tmp366,sizeof(char),25);}),_tag_dyneither(_tmp365,sizeof(void*),
0));});_LL1C1:;}return ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
es,struct Cyc_Absyn_Tuniondecl*tud,struct Cyc_Absyn_Tunionfield*tuf){struct
_RegionHandle*_tmp367=Cyc_Tcenv_get_fnrgn(te);struct _tuple4 _tmp368=({struct
_tuple4 _tmp385;_tmp385.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp385.f2=_tmp367;
_tmp385;});struct Cyc_List_List*_tmp369=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp367,Cyc_Tcutil_r_make_inst_var,&
_tmp368,tud->tvs);struct Cyc_List_List*_tmp36A=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp369);void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp381=_cycalloc(sizeof(*_tmp381));_tmp381[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp382;_tmp382.tag=3;_tmp382.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp383;
_tmp383.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)({union Cyc_Absyn_TunionFieldInfoU_union
_tmp384;(_tmp384.KnownTunionfield).tag=1;(_tmp384.KnownTunionfield).f1=tud;(
_tmp384.KnownTunionfield).f2=tuf;_tmp384;});_tmp383.targs=_tmp36A;_tmp383;});
_tmp382;});_tmp381;});if(topt != 0){void*_tmp36B=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_TunionInfo _tmp36C;struct Cyc_List_List*_tmp36D;struct Cyc_Core_Opt*
_tmp36E;_LL1C7: if(_tmp36B <= (void*)4)goto _LL1CB;if(*((int*)_tmp36B)!= 3)goto
_LL1C9;_LL1C8: Cyc_Tcutil_unify(*topt,res);goto _LL1C6;_LL1C9: if(*((int*)_tmp36B)
!= 2)goto _LL1CB;_tmp36C=((struct Cyc_Absyn_TunionType_struct*)_tmp36B)->f1;
_tmp36D=_tmp36C.targs;_tmp36E=_tmp36C.rgn;_LL1CA:{struct Cyc_List_List*a=_tmp36A;
for(0;a != 0  && _tmp36D != 0;(a=a->tl,_tmp36D=_tmp36D->tl)){Cyc_Tcutil_unify((void*)
a->hd,(void*)_tmp36D->hd);}}if(tud->is_flat  || tuf->typs == 0  && es == 0){e->topt=({
struct Cyc_Core_Opt*_tmp36F=_cycalloc(sizeof(*_tmp36F));_tmp36F->v=(void*)res;
_tmp36F;});res=(void*)({struct Cyc_Absyn_TunionType_struct*_tmp370=_cycalloc(
sizeof(*_tmp370));_tmp370[0]=({struct Cyc_Absyn_TunionType_struct _tmp371;_tmp371.tag=
2;_tmp371.f1=({struct Cyc_Absyn_TunionInfo _tmp372;_tmp372.tunion_info=(union Cyc_Absyn_TunionInfoU_union)({
union Cyc_Absyn_TunionInfoU_union _tmp373;(_tmp373.KnownTunion).tag=1;(_tmp373.KnownTunion).f1=({
struct Cyc_Absyn_Tuniondecl**_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374[0]=tud;
_tmp374;});_tmp373;});_tmp372.targs=_tmp36A;_tmp372.rgn=_tmp36E;_tmp372;});
_tmp371;});_tmp370;});Cyc_Tcutil_unchecked_cast(te,e,res,(void*)1);}goto _LL1C6;
_LL1CB:;_LL1CC: goto _LL1C6;_LL1C6:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 
0  && ts != 0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
es->hd;void*t=Cyc_Tcutil_rsubstitute(_tmp367,_tmp369,(*((struct _tuple9*)ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({
struct Cyc_String_pa_struct _tmp379;_tmp379.tag=0;_tmp379.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)(e->topt == 0?(struct _dyneither_ptr)({const char*_tmp37A="?";
_tag_dyneither(_tmp37A,sizeof(char),2);}): Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v)));{struct Cyc_String_pa_struct _tmp378;
_tmp378.tag=0;_tmp378.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{struct Cyc_String_pa_struct _tmp377;_tmp377.tag=0;_tmp377.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));{void*
_tmp375[3]={& _tmp377,& _tmp378,& _tmp379};Cyc_Tcutil_terr(e->loc,({const char*
_tmp376="tunion constructor %s expects argument of type %s but this argument has type %s";
_tag_dyneither(_tmp376,sizeof(char),80);}),_tag_dyneither(_tmp375,sizeof(void*),
3));}}}});Cyc_Tcutil_explain_failure();}}if(es != 0)return({struct Cyc_String_pa_struct
_tmp37D;_tmp37D.tag=0;_tmp37D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name));{void*_tmp37B[1]={& _tmp37D};Cyc_Tcexp_expr_err(
te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,({const char*_tmp37C="too many arguments for tunion constructor %s";
_tag_dyneither(_tmp37C,sizeof(char),45);}),_tag_dyneither(_tmp37B,sizeof(void*),
1));}});if(ts != 0)return({struct Cyc_String_pa_struct _tmp380;_tmp380.tag=0;
_tmp380.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name));{void*_tmp37E[1]={& _tmp380};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp37F="too few arguments for tunion constructor %s";_tag_dyneither(
_tmp37F,sizeof(char),44);}),_tag_dyneither(_tmp37E,sizeof(void*),1));}});return
res;}}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp386=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp387;struct Cyc_Absyn_PtrAtts _tmp388;struct Cyc_Absyn_Conref*
_tmp389;struct Cyc_Absyn_ArrayInfo _tmp38A;void*_tmp38B;struct Cyc_List_List*
_tmp38C;struct Cyc_Absyn_AggrInfo _tmp38D;union Cyc_Absyn_AggrInfoU_union _tmp38E;
struct Cyc_List_List*_tmp38F;struct Cyc_List_List*_tmp390;_LL1CE: if((int)_tmp386 != 
0)goto _LL1D0;_LL1CF: return 1;_LL1D0: if(_tmp386 <= (void*)4)goto _LL1DC;if(*((int*)
_tmp386)!= 0)goto _LL1D2;_LL1D1: goto _LL1D3;_LL1D2: if(*((int*)_tmp386)!= 1)goto
_LL1D4;_LL1D3: goto _LL1D5;_LL1D4: if(*((int*)_tmp386)!= 2)goto _LL1D6;_LL1D5: goto
_LL1D7;_LL1D6: if(*((int*)_tmp386)!= 3)goto _LL1D8;_LL1D7: return 0;_LL1D8: if(*((int*)
_tmp386)!= 4)goto _LL1DA;_tmp387=((struct Cyc_Absyn_PointerType_struct*)_tmp386)->f1;
_tmp388=_tmp387.ptr_atts;_tmp389=_tmp388.nullable;_LL1D9: return((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp389);_LL1DA: if(*((int*)
_tmp386)!= 5)goto _LL1DC;_LL1DB: goto _LL1DD;_LL1DC: if((int)_tmp386 != 1)goto _LL1DE;
_LL1DD: goto _LL1DF;_LL1DE: if(_tmp386 <= (void*)4)goto _LL1F8;if(*((int*)_tmp386)!= 
6)goto _LL1E0;_LL1DF: return 1;_LL1E0: if(*((int*)_tmp386)!= 7)goto _LL1E2;_tmp38A=((
struct Cyc_Absyn_ArrayType_struct*)_tmp386)->f1;_tmp38B=(void*)_tmp38A.elt_type;
_LL1E1: return Cyc_Tcexp_zeroable_type(_tmp38B);_LL1E2: if(*((int*)_tmp386)!= 8)
goto _LL1E4;_LL1E3: return 0;_LL1E4: if(*((int*)_tmp386)!= 9)goto _LL1E6;_tmp38C=((
struct Cyc_Absyn_TupleType_struct*)_tmp386)->f1;_LL1E5: for(0;(unsigned int)
_tmp38C;_tmp38C=_tmp38C->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple9*)
_tmp38C->hd)).f2))return 0;}return 1;_LL1E6: if(*((int*)_tmp386)!= 10)goto _LL1E8;
_tmp38D=((struct Cyc_Absyn_AggrType_struct*)_tmp386)->f1;_tmp38E=_tmp38D.aggr_info;
_tmp38F=_tmp38D.targs;_LL1E7: {struct Cyc_Absyn_Aggrdecl*_tmp391=Cyc_Absyn_get_known_aggrdecl(
_tmp38E);if(_tmp391->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp391->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp392=_new_region("r");
struct _RegionHandle*r=& _tmp392;_push_region(r);{struct Cyc_List_List*_tmp393=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp391->tvs,_tmp38F);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp391->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp393,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp394=0;_npop_handler(0);
return _tmp394;}}}{int _tmp395=1;_npop_handler(0);return _tmp395;}};_pop_region(r);}}
_LL1E8: if(*((int*)_tmp386)!= 12)goto _LL1EA;_LL1E9: return 1;_LL1EA: if(*((int*)
_tmp386)!= 18)goto _LL1EC;_LL1EB: return 1;_LL1EC: if(*((int*)_tmp386)!= 11)goto
_LL1EE;_tmp390=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp386)->f2;_LL1ED: for(0;
_tmp390 != 0;_tmp390=_tmp390->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp390->hd)->type))return 0;}return 1;_LL1EE: if(*((int*)_tmp386)!= 13)goto _LL1F0;
_LL1EF: return 1;_LL1F0: if(*((int*)_tmp386)!= 16)goto _LL1F2;_LL1F1: return 0;_LL1F2:
if(*((int*)_tmp386)!= 15)goto _LL1F4;_LL1F3: return 0;_LL1F4: if(*((int*)_tmp386)!= 
14)goto _LL1F6;_LL1F5: return 0;_LL1F6: if(*((int*)_tmp386)!= 17)goto _LL1F8;_LL1F7:
goto _LL1F9;_LL1F8: if((int)_tmp386 != 2)goto _LL1FA;_LL1F9: goto _LL1FB;_LL1FA: if((
int)_tmp386 != 3)goto _LL1FC;_LL1FB: goto _LL1FD;_LL1FC: if(_tmp386 <= (void*)4)goto
_LL1FE;if(*((int*)_tmp386)!= 19)goto _LL1FE;_LL1FD: goto _LL1FF;_LL1FE: if(_tmp386 <= (
void*)4)goto _LL200;if(*((int*)_tmp386)!= 20)goto _LL200;_LL1FF: goto _LL201;_LL200:
if(_tmp386 <= (void*)4)goto _LL1CD;if(*((int*)_tmp386)!= 21)goto _LL1CD;_LL201:({
struct Cyc_String_pa_struct _tmp398;_tmp398.tag=0;_tmp398.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp396[1]={& _tmp398};((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp397="bad type `%s' in zeroable type";_tag_dyneither(_tmp397,sizeof(char),
31);}),_tag_dyneither(_tmp396,sizeof(void*),1));}});_LL1CD:;}static void Cyc_Tcexp_check_malloc_type(
int allow_zero,struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(
t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;if(topt != 0){void*_tmp399=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp39A;void*_tmp39B;_LL203:
if(_tmp399 <= (void*)4)goto _LL205;if(*((int*)_tmp399)!= 4)goto _LL205;_tmp39A=((
struct Cyc_Absyn_PointerType_struct*)_tmp399)->f1;_tmp39B=(void*)_tmp39A.elt_typ;
_LL204: Cyc_Tcutil_unify(_tmp39B,t);if(Cyc_Tcutil_bits_only(t) || allow_zero  && 
Cyc_Tcexp_zeroable_type(t))return;goto _LL202;_LL205:;_LL206: goto _LL202;_LL202:;}({
struct Cyc_String_pa_struct _tmp39F;_tmp39F.tag=0;_tmp39F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{struct Cyc_String_pa_struct
_tmp39E;_tmp39E.tag=0;_tmp39E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
allow_zero?(struct _dyneither_ptr)({const char*_tmp3A0="calloc";_tag_dyneither(
_tmp3A0,sizeof(char),7);}):(struct _dyneither_ptr)({const char*_tmp3A1="malloc";
_tag_dyneither(_tmp3A1,sizeof(char),7);})));{void*_tmp39C[2]={& _tmp39E,& _tmp39F};
Cyc_Tcutil_terr(loc,({const char*_tmp39D="%s cannot be used with type %s\n\t(type needs initialization)";
_tag_dyneither(_tmp39D,sizeof(char),60);}),_tag_dyneither(_tmp39C,sizeof(void*),
2));}}});}static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*ropt,void***t,struct Cyc_Absyn_Exp**e,int*
is_calloc,int*is_fat){void*rgn=(void*)2;if(ropt != 0){void*expected_type=(void*)({
struct Cyc_Absyn_RgnHandleType_struct*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));_tmp3A7[
0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp3A8;_tmp3A8.tag=14;_tmp3A8.f1=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(
te));_tmp3A8;});_tmp3A7;});void*handle_type=Cyc_Tcexp_tcExp(te,(void**)&
expected_type,(struct Cyc_Absyn_Exp*)ropt);void*_tmp3A2=Cyc_Tcutil_compress(
handle_type);void*_tmp3A3;_LL208: if(_tmp3A2 <= (void*)4)goto _LL20A;if(*((int*)
_tmp3A2)!= 14)goto _LL20A;_tmp3A3=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp3A2)->f1;_LL209: rgn=_tmp3A3;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto
_LL207;_LL20A:;_LL20B:({struct Cyc_String_pa_struct _tmp3A6;_tmp3A6.tag=0;_tmp3A6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type));{
void*_tmp3A4[1]={& _tmp3A6};Cyc_Tcutil_terr(ropt->loc,({const char*_tmp3A5="expecting region_t type but found %s";
_tag_dyneither(_tmp3A5,sizeof(char),37);}),_tag_dyneither(_tmp3A4,sizeof(void*),
1));}});goto _LL207;_LL207:;}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{
void*elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*is_calloc){if(*t == 0)({
void*_tmp3A9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp3AA="calloc with empty type";_tag_dyneither(_tmp3AA,sizeof(char),
23);}),_tag_dyneither(_tmp3A9,sizeof(void*),0));});elt_type=*((void**)
_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(
void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);num_elts=*e;
one_elt=0;}else{void*_tmp3AB=(void*)(*e)->r;void*_tmp3AC;void*_tmp3AD;struct Cyc_List_List*
_tmp3AE;struct Cyc_List_List _tmp3AF;struct Cyc_Absyn_Exp*_tmp3B0;struct Cyc_List_List*
_tmp3B1;struct Cyc_List_List _tmp3B2;struct Cyc_Absyn_Exp*_tmp3B3;struct Cyc_List_List*
_tmp3B4;_LL20D: if(*((int*)_tmp3AB)!= 18)goto _LL20F;_tmp3AC=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp3AB)->f1;_LL20E: elt_type=_tmp3AC;*t=(void**)({void**_tmp3B5=_cycalloc(
sizeof(*_tmp3B5));_tmp3B5[0]=elt_type;_tmp3B5;});num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL20C;
_LL20F: if(*((int*)_tmp3AB)!= 3)goto _LL211;_tmp3AD=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3AB)->f1;if((int)_tmp3AD != 1)goto _LL211;_tmp3AE=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3AB)->f2;if(_tmp3AE == 0)goto _LL211;_tmp3AF=*_tmp3AE;_tmp3B0=(struct Cyc_Absyn_Exp*)
_tmp3AF.hd;_tmp3B1=_tmp3AF.tl;if(_tmp3B1 == 0)goto _LL211;_tmp3B2=*_tmp3B1;_tmp3B3=(
struct Cyc_Absyn_Exp*)_tmp3B2.hd;_tmp3B4=_tmp3B2.tl;if(_tmp3B4 != 0)goto _LL211;
_LL210:{struct _tuple0 _tmp3B7=({struct _tuple0 _tmp3B6;_tmp3B6.f1=(void*)_tmp3B0->r;
_tmp3B6.f2=(void*)_tmp3B3->r;_tmp3B6;});void*_tmp3B8;void*_tmp3B9;void*_tmp3BA;
void*_tmp3BB;_LL214: _tmp3B8=_tmp3B7.f1;if(*((int*)_tmp3B8)!= 18)goto _LL216;
_tmp3B9=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3B8)->f1;_LL215: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3B9);elt_type=_tmp3B9;*t=(void**)({void**_tmp3BC=_cycalloc(
sizeof(*_tmp3BC));_tmp3BC[0]=elt_type;_tmp3BC;});num_elts=_tmp3B3;one_elt=0;goto
_LL213;_LL216: _tmp3BA=_tmp3B7.f2;if(*((int*)_tmp3BA)!= 18)goto _LL218;_tmp3BB=(
void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3BA)->f1;_LL217: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3BB);elt_type=_tmp3BB;*t=(void**)({void**_tmp3BD=_cycalloc(
sizeof(*_tmp3BD));_tmp3BD[0]=elt_type;_tmp3BD;});num_elts=_tmp3B0;one_elt=0;goto
_LL213;_LL218:;_LL219: goto No_sizeof;_LL213:;}goto _LL20C;_LL211:;_LL212: No_sizeof:
elt_type=Cyc_Absyn_char_typ;*t=(void**)({void**_tmp3BE=_cycalloc(sizeof(*_tmp3BE));
_tmp3BE[0]=elt_type;_tmp3BE;});num_elts=*e;one_elt=0;goto _LL20C;_LL20C:;}*e=
num_elts;*is_fat=!one_elt;{void*_tmp3BF=elt_type;struct Cyc_Absyn_AggrInfo _tmp3C0;
union Cyc_Absyn_AggrInfoU_union _tmp3C1;struct Cyc_Absyn_Aggrdecl**_tmp3C2;struct
Cyc_Absyn_Aggrdecl*_tmp3C3;_LL21B: if(_tmp3BF <= (void*)4)goto _LL21D;if(*((int*)
_tmp3BF)!= 10)goto _LL21D;_tmp3C0=((struct Cyc_Absyn_AggrType_struct*)_tmp3BF)->f1;
_tmp3C1=_tmp3C0.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp3BF)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL21D;_tmp3C2=(_tmp3C1.KnownAggr).f1;_tmp3C3=*_tmp3C2;_LL21C: if(
_tmp3C3->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3C3->impl))->exist_vars
!= 0)({void*_tmp3C4=0;Cyc_Tcutil_terr(loc,({const char*_tmp3C5="malloc with existential types not yet implemented";
_tag_dyneither(_tmp3C5,sizeof(char),50);}),_tag_dyneither(_tmp3C4,sizeof(void*),
0));});goto _LL21A;_LL21D:;_LL21E: goto _LL21A;_LL21A:;}{void*(*_tmp3C6)(void*t,
void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term)=Cyc_Absyn_at_typ;
struct Cyc_Absyn_Conref*_tmp3C7=Cyc_Absyn_false_conref;if(topt != 0){void*_tmp3C8=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp3C9;struct Cyc_Absyn_PtrAtts
_tmp3CA;struct Cyc_Absyn_Conref*_tmp3CB;struct Cyc_Absyn_Conref*_tmp3CC;struct Cyc_Absyn_Conref*
_tmp3CD;_LL220: if(_tmp3C8 <= (void*)4)goto _LL222;if(*((int*)_tmp3C8)!= 4)goto
_LL222;_tmp3C9=((struct Cyc_Absyn_PointerType_struct*)_tmp3C8)->f1;_tmp3CA=
_tmp3C9.ptr_atts;_tmp3CB=_tmp3CA.nullable;_tmp3CC=_tmp3CA.bounds;_tmp3CD=_tmp3CA.zero_term;
_LL221: _tmp3C7=_tmp3CD;if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3CB))_tmp3C6=Cyc_Absyn_star_typ;if(((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp3CD) && !(*is_calloc)){({void*_tmp3CE=0;Cyc_Tcutil_warn(
loc,({const char*_tmp3CF="converting malloc to calloc to ensure zero-termination";
_tag_dyneither(_tmp3CF,sizeof(char),55);}),_tag_dyneither(_tmp3CE,sizeof(void*),
0));});*is_calloc=1;}{void*_tmp3D0=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp3CC);struct Cyc_Absyn_Exp*_tmp3D1;_LL225: if((int)_tmp3D0 != 0)goto _LL227;
_LL226: goto _LL224;_LL227: if(_tmp3D0 <= (void*)1)goto _LL229;if(*((int*)_tmp3D0)!= 
0)goto _LL229;_tmp3D1=((struct Cyc_Absyn_Upper_b_struct*)_tmp3D0)->f1;if(!(!
one_elt))goto _LL229;_LL228: {int _tmp3D2=Cyc_Evexp_c_can_eval(num_elts);if(
_tmp3D2  && Cyc_Evexp_same_const_exp(_tmp3D1,num_elts)){*is_fat=0;return Cyc_Absyn_atb_typ(
elt_type,rgn,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_conref_val(_tmp3CC),_tmp3C7);}{
void*_tmp3D3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
num_elts->topt))->v);void*_tmp3D4;_LL22C: if(_tmp3D3 <= (void*)4)goto _LL22E;if(*((
int*)_tmp3D3)!= 18)goto _LL22E;_tmp3D4=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp3D3)->f1;_LL22D: {struct Cyc_Absyn_Exp*_tmp3D5=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,
Cyc_Absyn_valueof_exp(_tmp3D4,0),0,(void*)1,0);if(Cyc_Evexp_same_const_exp(
_tmp3D5,_tmp3D1)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_conref_val(_tmp3CC),_tmp3C7);}goto _LL22B;}_LL22E:;_LL22F: goto _LL22B;
_LL22B:;}goto _LL224;}_LL229:;_LL22A: goto _LL224;_LL224:;}goto _LL21F;_LL222:;
_LL223: goto _LL21F;_LL21F:;}if(!one_elt)_tmp3C6=Cyc_Absyn_dyneither_typ;return
_tmp3C6(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp3C7);}}}static void*Cyc_Tcexp_tcSwap(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((
void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp3D6=Cyc_Tcutil_compress(t1);_LL231: if(
_tmp3D6 <= (void*)4)goto _LL233;if(*((int*)_tmp3D6)!= 7)goto _LL233;_LL232:({void*
_tmp3D7=0;Cyc_Tcutil_terr(loc,({const char*_tmp3D8="cannot assign to an array";
_tag_dyneither(_tmp3D8,sizeof(char),26);}),_tag_dyneither(_tmp3D7,sizeof(void*),
0));});goto _LL230;_LL233:;_LL234: goto _LL230;_LL230:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(
t1,& ign_1))({void*_tmp3D9=0;Cyc_Tcutil_terr(loc,({const char*_tmp3DA="Swap not allowed for non-pointer or non-word-sized types.";
_tag_dyneither(_tmp3DA,sizeof(char),58);}),_tag_dyneither(_tmp3D9,sizeof(void*),
0));});if(!Cyc_Absyn_is_lvalue(e1))return({void*_tmp3DB=0;Cyc_Tcexp_expr_err(te,
e1->loc,topt,({const char*_tmp3DC="swap non-lvalue";_tag_dyneither(_tmp3DC,
sizeof(char),16);}),_tag_dyneither(_tmp3DB,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(
e2))return({void*_tmp3DD=0;Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*
_tmp3DE="swap non-lvalue";_tag_dyneither(_tmp3DE,sizeof(char),16);}),
_tag_dyneither(_tmp3DD,sizeof(void*),0));});{void*t_ign1=(void*)0;void*t_ign2=(
void*)0;int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2))
return({void*_tmp3DF=0;Cyc_Tcexp_expr_err(te,e1->loc,topt,({const char*_tmp3E0="swap value in zeroterm array";
_tag_dyneither(_tmp3E0,sizeof(char),29);}),_tag_dyneither(_tmp3DF,sizeof(void*),
0));});if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2))return({void*
_tmp3E1=0;Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*_tmp3E2="swap value in zeroterm array";
_tag_dyneither(_tmp3E2,sizeof(char),29);}),_tag_dyneither(_tmp3E1,sizeof(void*),
0));});Cyc_Tcexp_check_writable(te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(
t1,t2)){void*_tmp3E3=({struct Cyc_String_pa_struct _tmp3E7;_tmp3E7.tag=0;_tmp3E7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));{struct
Cyc_String_pa_struct _tmp3E6;_tmp3E6.tag=0;_tmp3E6.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));{void*_tmp3E4[2]={& _tmp3E6,&
_tmp3E7};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp3E5="type mismatch: %s != %s";
_tag_dyneither(_tmp3E5,sizeof(char),24);}),_tag_dyneither(_tmp3E4,sizeof(void*),
2));}}});return _tmp3E3;}return(void*)0;}}}}int Cyc_Tcexp_in_stmt_exp=0;static void*
Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Stmt*s){struct _RegionHandle*_tmp3E8=Cyc_Tcenv_get_fnrgn(te);{
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
_tmp3E8,te,s),s,1);Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp3E9=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp3EA;struct Cyc_Absyn_Stmt*_tmp3EB;struct Cyc_Absyn_Stmt*_tmp3EC;struct Cyc_Absyn_Decl*
_tmp3ED;struct Cyc_Absyn_Stmt*_tmp3EE;_LL236: if(_tmp3E9 <= (void*)1)goto _LL23C;if(*((
int*)_tmp3E9)!= 0)goto _LL238;_tmp3EA=((struct Cyc_Absyn_Exp_s_struct*)_tmp3E9)->f1;
_LL237: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3EA->topt))->v;_LL238:
if(*((int*)_tmp3E9)!= 1)goto _LL23A;_tmp3EB=((struct Cyc_Absyn_Seq_s_struct*)
_tmp3E9)->f1;_tmp3EC=((struct Cyc_Absyn_Seq_s_struct*)_tmp3E9)->f2;_LL239: s=
_tmp3EC;continue;_LL23A: if(*((int*)_tmp3E9)!= 11)goto _LL23C;_tmp3ED=((struct Cyc_Absyn_Decl_s_struct*)
_tmp3E9)->f1;_tmp3EE=((struct Cyc_Absyn_Decl_s_struct*)_tmp3E9)->f2;_LL23B: s=
_tmp3EE;continue;_LL23C:;_LL23D: return({void*_tmp3EF=0;Cyc_Tcexp_expr_err(te,loc,
topt,({const char*_tmp3F0="statement expression must end with expression";
_tag_dyneither(_tmp3F0,sizeof(char),46);}),_tag_dyneither(_tmp3EF,sizeof(void*),
0));});_LL235:;}}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1){void*rgn=(void*)2;if(rgn_handle != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp3F6=_cycalloc(sizeof(*_tmp3F6));_tmp3F6[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp3F7;_tmp3F7.tag=14;_tmp3F7.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp3F7;});_tmp3F6;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
rgn_handle);void*_tmp3F1=Cyc_Tcutil_compress(handle_type);void*_tmp3F2;_LL23F:
if(_tmp3F1 <= (void*)4)goto _LL241;if(*((int*)_tmp3F1)!= 14)goto _LL241;_tmp3F2=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp3F1)->f1;_LL240: rgn=_tmp3F2;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL23E;_LL241:;_LL242:({struct Cyc_String_pa_struct
_tmp3F5;_tmp3F5.tag=0;_tmp3F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type));{void*_tmp3F3[1]={& _tmp3F5};Cyc_Tcutil_terr(
rgn_handle->loc,({const char*_tmp3F4="expecting region_t type but found %s";
_tag_dyneither(_tmp3F4,sizeof(char),37);}),_tag_dyneither(_tmp3F3,sizeof(void*),
1));}});goto _LL23E;_LL23E:;}{void*_tmp3F8=(void*)e1->r;struct Cyc_Core_Opt*
_tmp3F9;struct Cyc_List_List*_tmp3FA;struct Cyc_List_List*_tmp3FB;union Cyc_Absyn_Cnst_union
_tmp3FC;struct _dyneither_ptr _tmp3FD;_LL244: if(*((int*)_tmp3F8)!= 29)goto _LL246;
_LL245: {void*_tmp3FE=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);void*_tmp3FF=Cyc_Tcutil_compress(
_tmp3FE);struct Cyc_Absyn_ArrayInfo _tmp400;void*_tmp401;struct Cyc_Absyn_Tqual
_tmp402;struct Cyc_Absyn_Exp*_tmp403;struct Cyc_Absyn_Conref*_tmp404;_LL24F: if(
_tmp3FF <= (void*)4)goto _LL251;if(*((int*)_tmp3FF)!= 7)goto _LL251;_tmp400=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3FF)->f1;_tmp401=(void*)_tmp400.elt_type;
_tmp402=_tmp400.tq;_tmp403=_tmp400.num_elts;_tmp404=_tmp400.zero_term;_LL250: {
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp403);void*b;{void*
_tmp405=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);
void*_tmp406;_LL254: if(_tmp405 <= (void*)4)goto _LL256;if(*((int*)_tmp405)!= 18)
goto _LL256;_tmp406=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp405)->f1;_LL255:
b=(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp407=_cycalloc(sizeof(*_tmp407));
_tmp407[0]=({struct Cyc_Absyn_Upper_b_struct _tmp408;_tmp408.tag=0;_tmp408.f1=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp406,0),0,(void*)1,0);_tmp408;});
_tmp407;});goto _LL253;_LL256:;_LL257: if(Cyc_Tcutil_is_const_exp(te,bnd))b=(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp409=_cycalloc(sizeof(*_tmp409));_tmp409[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp40A;_tmp40A.tag=0;_tmp40A.f1=bnd;_tmp40A;});
_tmp409;});else{b=(void*)0;}_LL253:;}{void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C[0]=({struct Cyc_Absyn_PointerType_struct
_tmp40D;_tmp40D.tag=4;_tmp40D.f1=({struct Cyc_Absyn_PtrInfo _tmp40E;_tmp40E.elt_typ=(
void*)_tmp401;_tmp40E.elt_tq=_tmp402;_tmp40E.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp40F;_tmp40F.rgn=(void*)rgn;_tmp40F.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp40F.bounds=Cyc_Absyn_new_conref(b);_tmp40F.zero_term=
_tmp404;_tmp40F.ptrloc=0;_tmp40F;});_tmp40E;});_tmp40D;});_tmp40C;});if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt)){e->topt=({struct Cyc_Core_Opt*_tmp40B=_cycalloc(sizeof(*_tmp40B));_tmp40B->v=(
void*)res_typ;_tmp40B;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*
topt;}}return res_typ;}}_LL251:;_LL252:({void*_tmp410=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp411="tcNew: comprehension returned non-array type";
_tag_dyneither(_tmp411,sizeof(char),45);}),_tag_dyneither(_tmp410,sizeof(void*),
0));});_LL24E:;}_LL246: if(*((int*)_tmp3F8)!= 37)goto _LL248;_tmp3F9=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp3F8)->f1;_tmp3FA=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp3F8)->f2;
_LL247:(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp412=
_cycalloc(sizeof(*_tmp412));_tmp412[0]=({struct Cyc_Absyn_Array_e_struct _tmp413;
_tmp413.tag=28;_tmp413.f1=_tmp3FA;_tmp413;});_tmp412;})));_tmp3FB=_tmp3FA;goto
_LL249;_LL248: if(*((int*)_tmp3F8)!= 28)goto _LL24A;_tmp3FB=((struct Cyc_Absyn_Array_e_struct*)
_tmp3F8)->f1;_LL249: {void**elt_typ_opt=0;int zero_term=0;if(topt != 0){void*
_tmp414=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp415;void*_tmp416;
void**_tmp417;struct Cyc_Absyn_Tqual _tmp418;struct Cyc_Absyn_PtrAtts _tmp419;struct
Cyc_Absyn_Conref*_tmp41A;_LL259: if(_tmp414 <= (void*)4)goto _LL25B;if(*((int*)
_tmp414)!= 4)goto _LL25B;_tmp415=((struct Cyc_Absyn_PointerType_struct*)_tmp414)->f1;
_tmp416=(void*)_tmp415.elt_typ;_tmp417=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp414)->f1).elt_typ;_tmp418=_tmp415.elt_tq;_tmp419=_tmp415.ptr_atts;_tmp41A=
_tmp419.zero_term;_LL25A: elt_typ_opt=(void**)_tmp417;zero_term=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp41A);goto _LL258;_LL25B:;
_LL25C: goto _LL258;_LL258:;}{void*_tmp41B=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp3FB);e1->topt=({struct Cyc_Core_Opt*_tmp41C=_cycalloc(
sizeof(*_tmp41C));_tmp41C->v=(void*)_tmp41B;_tmp41C;});{void*res_typ;{void*
_tmp41D=Cyc_Tcutil_compress(_tmp41B);struct Cyc_Absyn_ArrayInfo _tmp41E;void*
_tmp41F;struct Cyc_Absyn_Tqual _tmp420;struct Cyc_Absyn_Exp*_tmp421;struct Cyc_Absyn_Conref*
_tmp422;_LL25E: if(_tmp41D <= (void*)4)goto _LL260;if(*((int*)_tmp41D)!= 7)goto
_LL260;_tmp41E=((struct Cyc_Absyn_ArrayType_struct*)_tmp41D)->f1;_tmp41F=(void*)
_tmp41E.elt_type;_tmp420=_tmp41E.tq;_tmp421=_tmp41E.num_elts;_tmp422=_tmp41E.zero_term;
_LL25F: res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp423=_cycalloc(
sizeof(*_tmp423));_tmp423[0]=({struct Cyc_Absyn_PointerType_struct _tmp424;_tmp424.tag=
4;_tmp424.f1=({struct Cyc_Absyn_PtrInfo _tmp425;_tmp425.elt_typ=(void*)_tmp41F;
_tmp425.elt_tq=_tmp420;_tmp425.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp426;_tmp426.rgn=(
void*)rgn;_tmp426.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp426.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp427=_cycalloc(sizeof(*_tmp427));_tmp427[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp428;_tmp428.tag=0;_tmp428.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp421);
_tmp428;});_tmp427;}));_tmp426.zero_term=_tmp422;_tmp426.ptrloc=0;_tmp426;});
_tmp425;});_tmp424;});_tmp423;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ)
 && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){e->topt=({struct Cyc_Core_Opt*
_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429->v=(void*)res_typ;_tmp429;});Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}goto _LL25D;_LL260:;_LL261:({void*_tmp42A=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp42B="tcExpNoPromote on Array_e returned non-array type";_tag_dyneither(
_tmp42B,sizeof(char),50);}),_tag_dyneither(_tmp42A,sizeof(void*),0));});_LL25D:;}
return res_typ;}}}_LL24A: if(*((int*)_tmp3F8)!= 0)goto _LL24C;_tmp3FC=((struct Cyc_Absyn_Const_e_struct*)
_tmp3F8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp3F8)->f1).String_c).tag
!= 5)goto _LL24C;_tmp3FD=(_tmp3FC.String_c).f1;_LL24B: {void*_tmp42C=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);
void*_tmp42D=Cyc_Tcexp_tcExp(te,(void**)& _tmp42C,e1);return Cyc_Absyn_atb_typ(
_tmp42D,rgn,Cyc_Absyn_empty_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp42E=_cycalloc(sizeof(*_tmp42E));_tmp42E[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp42F;_tmp42F.tag=0;_tmp42F.f1=Cyc_Absyn_uint_exp(1,0);_tmp42F;});_tmp42E;}),
Cyc_Absyn_false_conref);}_LL24C:;_LL24D: {void**topt2=0;if(topt != 0){void*
_tmp430=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp431;void*_tmp432;
void**_tmp433;struct Cyc_Absyn_Tqual _tmp434;_LL263: if(_tmp430 <= (void*)4)goto
_LL267;if(*((int*)_tmp430)!= 4)goto _LL265;_tmp431=((struct Cyc_Absyn_PointerType_struct*)
_tmp430)->f1;_tmp432=(void*)_tmp431.elt_typ;_tmp433=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp430)->f1).elt_typ;_tmp434=_tmp431.elt_tq;_LL264: topt2=(void**)_tmp433;goto
_LL262;_LL265: if(*((int*)_tmp430)!= 2)goto _LL267;_LL266: topt2=({void**_tmp435=
_cycalloc(sizeof(*_tmp435));_tmp435[0]=*topt;_tmp435;});goto _LL262;_LL267:;
_LL268: goto _LL262;_LL262:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);void*res_typ=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp437=_cycalloc(sizeof(*_tmp437));
_tmp437[0]=({struct Cyc_Absyn_PointerType_struct _tmp438;_tmp438.tag=4;_tmp438.f1=({
struct Cyc_Absyn_PtrInfo _tmp439;_tmp439.elt_typ=(void*)telt;_tmp439.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmp439.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp43A;_tmp43A.rgn=(void*)rgn;
_tmp43A.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp43A.bounds=Cyc_Absyn_bounds_one_conref;_tmp43A.zero_term=Cyc_Absyn_false_conref;
_tmp43A.ptrloc=0;_tmp43A;});_tmp439;});_tmp438;});_tmp437;});if(topt != 0){if(!
Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt)){e->topt=({struct Cyc_Core_Opt*_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436->v=(
void*)res_typ;_tmp436;});Cyc_Tcutil_unchecked_cast(te,e,*topt,(void*)3);res_typ=*
topt;}}return res_typ;}}_LL243:;}}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,
void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp43B=t;struct Cyc_Absyn_ArrayInfo _tmp43C;void*_tmp43D;struct
Cyc_Absyn_Tqual _tmp43E;struct Cyc_Absyn_Exp*_tmp43F;struct Cyc_Absyn_Conref*
_tmp440;_LL26A: if(_tmp43B <= (void*)4)goto _LL26C;if(*((int*)_tmp43B)!= 7)goto
_LL26C;_tmp43C=((struct Cyc_Absyn_ArrayType_struct*)_tmp43B)->f1;_tmp43D=(void*)
_tmp43C.elt_type;_tmp43E=_tmp43C.tq;_tmp43F=_tmp43C.num_elts;_tmp440=_tmp43C.zero_term;
_LL26B: {void*_tmp442;struct _tuple6 _tmp441=Cyc_Tcutil_addressof_props(te,e);
_tmp442=_tmp441.f2;{void*_tmp443=_tmp43F == 0?(void*)((void*)0):(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp445;_tmp445.tag=0;_tmp445.f1=(struct Cyc_Absyn_Exp*)
_tmp43F;_tmp445;});_tmp444;});t=Cyc_Absyn_atb_typ(_tmp43D,_tmp442,_tmp43E,
_tmp443,_tmp440);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);
return t;}}_LL26C:;_LL26D: return t;_LL269:;}void*Cyc_Tcexp_tcExpInitializer(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(
te,topt,e);struct _RegionHandle*_tmp446=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp446,t) && !Cyc_Tcutil_is_noalias_path(_tmp446,e)){({struct Cyc_String_pa_struct
_tmp44B;_tmp44B.tag=0;_tmp44B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));{struct Cyc_String_pa_struct _tmp44A;_tmp44A.tag=0;
_tmp44A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{struct Cyc_String_pa_struct
_tmp449;_tmp449.tag=0;_tmp449.f1=(struct _dyneither_ptr)(topt == 0?({const char*
_tmp44C="NULL";_tag_dyneither(_tmp44C,sizeof(char),5);}):(struct _dyneither_ptr)
Cyc_Absynpp_typ2string(*topt));{void*_tmp447[3]={& _tmp449,& _tmp44A,& _tmp44B};Cyc_fprintf(
Cyc_stderr,({const char*_tmp448="topt=%s, e->topt->v=%s, e=%s\n";_tag_dyneither(
_tmp448,sizeof(char),30);}),_tag_dyneither(_tmp447,sizeof(void*),3));}}}});({
void*_tmp44D=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp44E="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp44E,sizeof(char),49);}),_tag_dyneither(_tmp44D,sizeof(void*),
0));});}{void*_tmp44F=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp450;_LL26F: if(*((
int*)_tmp44F)!= 28)goto _LL271;_LL270: goto _LL272;_LL271: if(*((int*)_tmp44F)!= 29)
goto _LL273;_LL272: goto _LL274;_LL273: if(*((int*)_tmp44F)!= 0)goto _LL275;_tmp450=((
struct Cyc_Absyn_Const_e_struct*)_tmp44F)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp44F)->f1).String_c).tag != 5)goto _LL275;_LL274: return t;_LL275:;_LL276: t=Cyc_Tcutil_compress(
t);{void*_tmp451=t;struct Cyc_Absyn_ArrayInfo _tmp452;void*_tmp453;struct Cyc_Absyn_Tqual
_tmp454;struct Cyc_Absyn_Exp*_tmp455;struct Cyc_Absyn_Conref*_tmp456;_LL278: if(
_tmp451 <= (void*)4)goto _LL27A;if(*((int*)_tmp451)!= 7)goto _LL27A;_tmp452=((
struct Cyc_Absyn_ArrayType_struct*)_tmp451)->f1;_tmp453=(void*)_tmp452.elt_type;
_tmp454=_tmp452.tq;_tmp455=_tmp452.num_elts;_tmp456=_tmp452.zero_term;_LL279: {
void*_tmp458;struct _tuple6 _tmp457=Cyc_Tcutil_addressof_props(te,e);_tmp458=
_tmp457.f2;{void*b=_tmp455 == 0?(void*)((void*)0):(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp459=_cycalloc(sizeof(*_tmp459));_tmp459[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp45A;_tmp45A.tag=0;_tmp45A.f1=(struct Cyc_Absyn_Exp*)_tmp455;_tmp45A;});
_tmp459;});t=Cyc_Absyn_atb_typ(_tmp453,_tmp458,_tmp454,b,_tmp456);Cyc_Tcutil_unchecked_cast(
te,e,t,(void*)3);return t;}}_LL27A:;_LL27B: return t;_LL277:;}_LL26E:;}}static void*
Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e){{void*_tmp45B=(void*)e->r;struct Cyc_Absyn_Exp*_tmp45C;_LL27D: if(*((int*)
_tmp45B)!= 13)goto _LL27F;_tmp45C=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp45B)->f1;_LL27E: Cyc_Tcexp_tcExpNoInst(te,topt,_tmp45C);(void*)(((struct Cyc_Core_Opt*)
_check_null(_tmp45C->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp45C->topt))->v,0));e->topt=_tmp45C->topt;goto
_LL27C;_LL27F:;_LL280: Cyc_Tcexp_tcExpNoInst(te,topt,e);(void*)(((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0));{void*_tmp45D=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp45E;void*
_tmp45F;struct Cyc_Absyn_Tqual _tmp460;struct Cyc_Absyn_PtrAtts _tmp461;void*_tmp462;
struct Cyc_Absyn_Conref*_tmp463;struct Cyc_Absyn_Conref*_tmp464;struct Cyc_Absyn_Conref*
_tmp465;_LL282: if(_tmp45D <= (void*)4)goto _LL284;if(*((int*)_tmp45D)!= 4)goto
_LL284;_tmp45E=((struct Cyc_Absyn_PointerType_struct*)_tmp45D)->f1;_tmp45F=(void*)
_tmp45E.elt_typ;_tmp460=_tmp45E.elt_tq;_tmp461=_tmp45E.ptr_atts;_tmp462=(void*)
_tmp461.rgn;_tmp463=_tmp461.nullable;_tmp464=_tmp461.bounds;_tmp465=_tmp461.zero_term;
_LL283:{void*_tmp466=Cyc_Tcutil_compress(_tmp45F);struct Cyc_Absyn_FnInfo _tmp467;
struct Cyc_List_List*_tmp468;struct Cyc_Core_Opt*_tmp469;void*_tmp46A;struct Cyc_List_List*
_tmp46B;int _tmp46C;struct Cyc_Absyn_VarargInfo*_tmp46D;struct Cyc_List_List*
_tmp46E;struct Cyc_List_List*_tmp46F;_LL287: if(_tmp466 <= (void*)4)goto _LL289;if(*((
int*)_tmp466)!= 8)goto _LL289;_tmp467=((struct Cyc_Absyn_FnType_struct*)_tmp466)->f1;
_tmp468=_tmp467.tvars;_tmp469=_tmp467.effect;_tmp46A=(void*)_tmp467.ret_typ;
_tmp46B=_tmp467.args;_tmp46C=_tmp467.c_varargs;_tmp46D=_tmp467.cyc_varargs;
_tmp46E=_tmp467.rgn_po;_tmp46F=_tmp467.attributes;_LL288: if(_tmp468 != 0){struct
_RegionHandle*_tmp470=Cyc_Tcenv_get_fnrgn(te);struct _tuple4 _tmp471=({struct
_tuple4 _tmp47E;_tmp47E.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp47E.f2=_tmp470;
_tmp47E;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp470,Cyc_Tcutil_r_make_inst_var,&
_tmp471,_tmp468);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(_tmp470,inst,(void*)({struct
Cyc_Absyn_FnType_struct*_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B[0]=({struct
Cyc_Absyn_FnType_struct _tmp47C;_tmp47C.tag=8;_tmp47C.f1=({struct Cyc_Absyn_FnInfo
_tmp47D;_tmp47D.tvars=0;_tmp47D.effect=_tmp469;_tmp47D.ret_typ=(void*)_tmp46A;
_tmp47D.args=_tmp46B;_tmp47D.c_varargs=_tmp46C;_tmp47D.cyc_varargs=_tmp46D;
_tmp47D.rgn_po=_tmp46E;_tmp47D.attributes=_tmp46F;_tmp47D;});_tmp47C;});_tmp47B;}));
struct Cyc_Absyn_PointerType_struct*_tmp472=({struct Cyc_Absyn_PointerType_struct*
_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477[0]=({struct Cyc_Absyn_PointerType_struct
_tmp478;_tmp478.tag=4;_tmp478.f1=({struct Cyc_Absyn_PtrInfo _tmp479;_tmp479.elt_typ=(
void*)ftyp;_tmp479.elt_tq=_tmp460;_tmp479.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp47A;_tmp47A.rgn=(void*)_tmp462;_tmp47A.nullable=_tmp463;_tmp47A.bounds=
_tmp464;_tmp47A.zero_term=_tmp465;_tmp47A.ptrloc=0;_tmp47A;});_tmp479;});_tmp478;});
_tmp477;});struct Cyc_Absyn_Exp*_tmp473=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Instantiate_e_struct*_tmp474=_cycalloc(sizeof(*_tmp474));
_tmp474[0]=({struct Cyc_Absyn_Instantiate_e_struct _tmp475;_tmp475.tag=14;_tmp475.f1=
_tmp473;_tmp475.f2=ts;_tmp475;});_tmp474;})));e->topt=({struct Cyc_Core_Opt*
_tmp476=_cycalloc(sizeof(*_tmp476));_tmp476->v=(void*)((void*)_tmp472);_tmp476;});}
goto _LL286;_LL289:;_LL28A: goto _LL286;_LL286:;}goto _LL281;_LL284:;_LL285: goto
_LL281;_LL281:;}goto _LL27C;_LL27C:;}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*loc=e->loc;void*
t;{void*_tmp47F=(void*)e->r;struct Cyc_Absyn_Exp*_tmp480;struct _tuple1*_tmp481;
struct Cyc_Absyn_Exp*_tmp482;struct Cyc_List_List*_tmp483;struct Cyc_Core_Opt*
_tmp484;struct Cyc_List_List*_tmp485;union Cyc_Absyn_Cnst_union _tmp486;union Cyc_Absyn_Cnst_union*
_tmp487;struct _tuple1*_tmp488;void*_tmp489;void*_tmp48A;struct Cyc_List_List*
_tmp48B;struct Cyc_Absyn_Exp*_tmp48C;void*_tmp48D;struct Cyc_Absyn_Exp*_tmp48E;
struct Cyc_Core_Opt*_tmp48F;struct Cyc_Absyn_Exp*_tmp490;struct Cyc_Absyn_Exp*
_tmp491;struct Cyc_Absyn_Exp*_tmp492;struct Cyc_Absyn_Exp*_tmp493;struct Cyc_Absyn_Exp*
_tmp494;struct Cyc_Absyn_Exp*_tmp495;struct Cyc_Absyn_Exp*_tmp496;struct Cyc_Absyn_Exp*
_tmp497;struct Cyc_Absyn_Exp*_tmp498;struct Cyc_Absyn_Exp*_tmp499;struct Cyc_Absyn_Exp*
_tmp49A;struct Cyc_List_List*_tmp49B;struct Cyc_Absyn_VarargCallInfo*_tmp49C;
struct Cyc_Absyn_VarargCallInfo**_tmp49D;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_Absyn_Exp*
_tmp49F;struct Cyc_List_List*_tmp4A0;void*_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A2;
void*_tmp4A3;void**_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A5;struct Cyc_Absyn_Exp*
_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A8;void*_tmp4A9;
void*_tmp4AA;void*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_Absyn_Exp*
_tmp4AD;struct _dyneither_ptr*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AF;struct
_dyneither_ptr*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B2;
struct Cyc_List_List*_tmp4B3;struct _tuple2*_tmp4B4;struct Cyc_List_List*_tmp4B5;
struct Cyc_List_List*_tmp4B6;struct Cyc_Absyn_Stmt*_tmp4B7;struct Cyc_Absyn_Vardecl*
_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BA;int _tmp4BB;int*
_tmp4BC;struct _tuple1*_tmp4BD;struct _tuple1**_tmp4BE;struct Cyc_List_List*_tmp4BF;
struct Cyc_List_List**_tmp4C0;struct Cyc_List_List*_tmp4C1;struct Cyc_Absyn_Aggrdecl*
_tmp4C2;struct Cyc_Absyn_Aggrdecl**_tmp4C3;void*_tmp4C4;struct Cyc_List_List*
_tmp4C5;struct Cyc_List_List*_tmp4C6;struct Cyc_Absyn_Tuniondecl*_tmp4C7;struct Cyc_Absyn_Tunionfield*
_tmp4C8;struct _tuple1*_tmp4C9;struct _tuple1**_tmp4CA;struct Cyc_Absyn_Enumdecl*
_tmp4CB;struct Cyc_Absyn_Enumfield*_tmp4CC;struct _tuple1*_tmp4CD;struct _tuple1**
_tmp4CE;void*_tmp4CF;struct Cyc_Absyn_Enumfield*_tmp4D0;struct Cyc_Absyn_MallocInfo
_tmp4D1;int _tmp4D2;int*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D4;void**_tmp4D5;void***
_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D7;struct Cyc_Absyn_Exp**_tmp4D8;int _tmp4D9;int*
_tmp4DA;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DC;void*_tmp4DD;
_LL28C: if(*((int*)_tmp47F)!= 13)goto _LL28E;_tmp480=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp47F)->f1;_LL28D: Cyc_Tcexp_tcExpNoInst(te,0,_tmp480);return;_LL28E: if(*((int*)
_tmp47F)!= 2)goto _LL290;_tmp481=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp47F)->f1;
_LL28F: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp481);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL290: if(*((int*)_tmp47F)!= 10)goto _LL292;_tmp482=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp47F)->f1;_tmp483=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp47F)->f2;_LL291:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp482,_tmp483);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL292: if(*((int*)_tmp47F)!= 37)goto _LL294;_tmp484=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp47F)->f1;_tmp485=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp47F)->f2;
_LL293: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp485);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL294: if(*((int*)_tmp47F)!= 0)goto _LL296;_tmp486=((struct Cyc_Absyn_Const_e_struct*)
_tmp47F)->f1;_tmp487=(union Cyc_Absyn_Cnst_union*)&((struct Cyc_Absyn_Const_e_struct*)
_tmp47F)->f1;_LL295: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst_union*)
_tmp487,e);goto _LL28B;_LL296: if(*((int*)_tmp47F)!= 1)goto _LL298;_tmp488=((struct
Cyc_Absyn_Var_e_struct*)_tmp47F)->f1;_tmp489=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp47F)->f2;_LL297: t=Cyc_Tcexp_tcVar(te,loc,_tmp488,_tmp489);goto _LL28B;_LL298:
if(*((int*)_tmp47F)!= 3)goto _LL29A;_tmp48A=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp47F)->f1;_tmp48B=((struct Cyc_Absyn_Primop_e_struct*)_tmp47F)->f2;_LL299: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp48A,_tmp48B);goto _LL28B;_LL29A: if(*((int*)
_tmp47F)!= 5)goto _LL29C;_tmp48C=((struct Cyc_Absyn_Increment_e_struct*)_tmp47F)->f1;
_tmp48D=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp47F)->f2;_LL29B: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp48C,_tmp48D);goto _LL28B;_LL29C: if(*((int*)_tmp47F)!= 4)goto
_LL29E;_tmp48E=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp47F)->f1;_tmp48F=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp47F)->f2;_tmp490=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp47F)->f3;_LL29D: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp48E,_tmp48F,_tmp490);
goto _LL28B;_LL29E: if(*((int*)_tmp47F)!= 6)goto _LL2A0;_tmp491=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp47F)->f1;_tmp492=((struct Cyc_Absyn_Conditional_e_struct*)_tmp47F)->f2;
_tmp493=((struct Cyc_Absyn_Conditional_e_struct*)_tmp47F)->f3;_LL29F: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp491,_tmp492,_tmp493);goto _LL28B;_LL2A0: if(*((int*)_tmp47F)!= 7)
goto _LL2A2;_tmp494=((struct Cyc_Absyn_And_e_struct*)_tmp47F)->f1;_tmp495=((struct
Cyc_Absyn_And_e_struct*)_tmp47F)->f2;_LL2A1: t=Cyc_Tcexp_tcAnd(te,loc,_tmp494,
_tmp495);goto _LL28B;_LL2A2: if(*((int*)_tmp47F)!= 8)goto _LL2A4;_tmp496=((struct
Cyc_Absyn_Or_e_struct*)_tmp47F)->f1;_tmp497=((struct Cyc_Absyn_Or_e_struct*)
_tmp47F)->f2;_LL2A3: t=Cyc_Tcexp_tcOr(te,loc,_tmp496,_tmp497);goto _LL28B;_LL2A4:
if(*((int*)_tmp47F)!= 9)goto _LL2A6;_tmp498=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp47F)->f1;_tmp499=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp47F)->f2;_LL2A5: t=
Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp498,_tmp499);goto _LL28B;_LL2A6: if(*((int*)
_tmp47F)!= 11)goto _LL2A8;_tmp49A=((struct Cyc_Absyn_FnCall_e_struct*)_tmp47F)->f1;
_tmp49B=((struct Cyc_Absyn_FnCall_e_struct*)_tmp47F)->f2;_tmp49C=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp47F)->f3;_tmp49D=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp47F)->f3;_LL2A7: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp49A,_tmp49B,_tmp49D);
goto _LL28B;_LL2A8: if(*((int*)_tmp47F)!= 12)goto _LL2AA;_tmp49E=((struct Cyc_Absyn_Throw_e_struct*)
_tmp47F)->f1;_LL2A9: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp49E);goto _LL28B;_LL2AA:
if(*((int*)_tmp47F)!= 14)goto _LL2AC;_tmp49F=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp47F)->f1;_tmp4A0=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp47F)->f2;_LL2AB:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp49F,_tmp4A0);goto _LL28B;_LL2AC: if(*((
int*)_tmp47F)!= 15)goto _LL2AE;_tmp4A1=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp47F)->f1;_tmp4A2=((struct Cyc_Absyn_Cast_e_struct*)_tmp47F)->f2;_tmp4A3=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp47F)->f4;_tmp4A4=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp47F)->f4);_LL2AD: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4A1,_tmp4A2,(void**)
_tmp4A4);goto _LL28B;_LL2AE: if(*((int*)_tmp47F)!= 16)goto _LL2B0;_tmp4A5=((struct
Cyc_Absyn_Address_e_struct*)_tmp47F)->f1;_LL2AF: t=Cyc_Tcexp_tcAddress(te,loc,e,
topt,_tmp4A5);goto _LL28B;_LL2B0: if(*((int*)_tmp47F)!= 17)goto _LL2B2;_tmp4A6=((
struct Cyc_Absyn_New_e_struct*)_tmp47F)->f1;_tmp4A7=((struct Cyc_Absyn_New_e_struct*)
_tmp47F)->f2;_LL2B1: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4A6,e,_tmp4A7);goto _LL28B;
_LL2B2: if(*((int*)_tmp47F)!= 19)goto _LL2B4;_tmp4A8=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp47F)->f1;_LL2B3: {void*_tmp4DE=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4A8);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4DE);goto _LL28B;}_LL2B4: if(*((int*)_tmp47F)!= 18)goto _LL2B6;
_tmp4A9=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp47F)->f1;_LL2B5: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4A9);goto _LL28B;_LL2B6: if(*((int*)_tmp47F)!= 20)goto _LL2B8;
_tmp4AA=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp47F)->f1;_tmp4AB=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp47F)->f2;_LL2B7: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp4AA,_tmp4AB);goto _LL28B;_LL2B8: if(*((int*)_tmp47F)!= 21)goto _LL2BA;
_LL2B9:({void*_tmp4DF=0;Cyc_Tcutil_terr(loc,({const char*_tmp4E0="gen() not in top-level initializer";
_tag_dyneither(_tmp4E0,sizeof(char),35);}),_tag_dyneither(_tmp4DF,sizeof(void*),
0));});return;_LL2BA: if(*((int*)_tmp47F)!= 22)goto _LL2BC;_tmp4AC=((struct Cyc_Absyn_Deref_e_struct*)
_tmp47F)->f1;_LL2BB: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4AC);goto _LL28B;_LL2BC:
if(*((int*)_tmp47F)!= 23)goto _LL2BE;_tmp4AD=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp47F)->f1;_tmp4AE=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp47F)->f2;_LL2BD:
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp4AD,_tmp4AE);goto _LL28B;_LL2BE: if(*((
int*)_tmp47F)!= 24)goto _LL2C0;_tmp4AF=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp47F)->f1;_tmp4B0=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp47F)->f2;_LL2BF: t=
Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp4AF,_tmp4B0);goto _LL28B;_LL2C0: if(*((int*)
_tmp47F)!= 25)goto _LL2C2;_tmp4B1=((struct Cyc_Absyn_Subscript_e_struct*)_tmp47F)->f1;
_tmp4B2=((struct Cyc_Absyn_Subscript_e_struct*)_tmp47F)->f2;_LL2C1: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp4B1,_tmp4B2);goto _LL28B;_LL2C2: if(*((int*)_tmp47F)!= 26)goto
_LL2C4;_tmp4B3=((struct Cyc_Absyn_Tuple_e_struct*)_tmp47F)->f1;_LL2C3: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp4B3);goto _LL28B;_LL2C4: if(*((int*)_tmp47F)!= 27)goto _LL2C6;
_tmp4B4=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp47F)->f1;_tmp4B5=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp47F)->f2;_LL2C5: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4B4,_tmp4B5);goto
_LL28B;_LL2C6: if(*((int*)_tmp47F)!= 28)goto _LL2C8;_tmp4B6=((struct Cyc_Absyn_Array_e_struct*)
_tmp47F)->f1;_LL2C7: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp4E1=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp4E2;void*_tmp4E3;void**
_tmp4E4;struct Cyc_Absyn_Conref*_tmp4E5;_LL2DD: if(_tmp4E1 <= (void*)4)goto _LL2DF;
if(*((int*)_tmp4E1)!= 7)goto _LL2DF;_tmp4E2=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4E1)->f1;_tmp4E3=(void*)_tmp4E2.elt_type;_tmp4E4=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp4E1)->f1).elt_type;_tmp4E5=_tmp4E2.zero_term;_LL2DE: elt_topt=(void**)_tmp4E4;
zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp4E5);
goto _LL2DC;_LL2DF:;_LL2E0: goto _LL2DC;_LL2DC:;}t=Cyc_Tcexp_tcArray(te,loc,
elt_topt,zero_term,_tmp4B6);goto _LL28B;}_LL2C8: if(*((int*)_tmp47F)!= 38)goto
_LL2CA;_tmp4B7=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp47F)->f1;_LL2C9: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp4B7);goto _LL28B;_LL2CA: if(*((int*)_tmp47F)!= 29)goto _LL2CC;
_tmp4B8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp47F)->f1;_tmp4B9=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp47F)->f2;_tmp4BA=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp47F)->f3;_tmp4BB=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp47F)->f4;
_tmp4BC=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp47F)->f4;_LL2CB: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4B8,_tmp4B9,_tmp4BA,_tmp4BC);goto
_LL28B;_LL2CC: if(*((int*)_tmp47F)!= 30)goto _LL2CE;_tmp4BD=((struct Cyc_Absyn_Struct_e_struct*)
_tmp47F)->f1;_tmp4BE=(struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp47F)->f1;_tmp4BF=((struct Cyc_Absyn_Struct_e_struct*)_tmp47F)->f2;_tmp4C0=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp47F)->f2;_tmp4C1=((
struct Cyc_Absyn_Struct_e_struct*)_tmp47F)->f3;_tmp4C2=((struct Cyc_Absyn_Struct_e_struct*)
_tmp47F)->f4;_tmp4C3=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp47F)->f4;_LL2CD: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp4BE,_tmp4C0,_tmp4C1,
_tmp4C3);goto _LL28B;_LL2CE: if(*((int*)_tmp47F)!= 31)goto _LL2D0;_tmp4C4=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp47F)->f1;_tmp4C5=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp47F)->f2;_LL2CF: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4C4,_tmp4C5);goto _LL28B;
_LL2D0: if(*((int*)_tmp47F)!= 32)goto _LL2D2;_tmp4C6=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp47F)->f1;_tmp4C7=((struct Cyc_Absyn_Tunion_e_struct*)_tmp47F)->f2;_tmp4C8=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp47F)->f3;_LL2D1: t=Cyc_Tcexp_tcTunion(te,loc,
topt,e,_tmp4C6,_tmp4C7,_tmp4C8);goto _LL28B;_LL2D2: if(*((int*)_tmp47F)!= 33)goto
_LL2D4;_tmp4C9=((struct Cyc_Absyn_Enum_e_struct*)_tmp47F)->f1;_tmp4CA=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp47F)->f1;_tmp4CB=((struct Cyc_Absyn_Enum_e_struct*)
_tmp47F)->f2;_tmp4CC=((struct Cyc_Absyn_Enum_e_struct*)_tmp47F)->f3;_LL2D3:*
_tmp4CA=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp4CC))->name;t=(void*)({
struct Cyc_Absyn_EnumType_struct*_tmp4E6=_cycalloc(sizeof(*_tmp4E6));_tmp4E6[0]=({
struct Cyc_Absyn_EnumType_struct _tmp4E7;_tmp4E7.tag=12;_tmp4E7.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp4CB))->name;_tmp4E7.f2=_tmp4CB;_tmp4E7;});_tmp4E6;});goto _LL28B;
_LL2D4: if(*((int*)_tmp47F)!= 34)goto _LL2D6;_tmp4CD=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp47F)->f1;_tmp4CE=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp47F)->f1;_tmp4CF=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp47F)->f2;
_tmp4D0=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp47F)->f3;_LL2D5:*_tmp4CE=((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp4D0))->name;t=_tmp4CF;goto _LL28B;
_LL2D6: if(*((int*)_tmp47F)!= 35)goto _LL2D8;_tmp4D1=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp47F)->f1;_tmp4D2=_tmp4D1.is_calloc;_tmp4D3=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp47F)->f1).is_calloc;_tmp4D4=_tmp4D1.rgn;_tmp4D5=_tmp4D1.elt_type;_tmp4D6=(
void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp47F)->f1).elt_type;_tmp4D7=
_tmp4D1.num_elts;_tmp4D8=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp47F)->f1).num_elts;_tmp4D9=_tmp4D1.fat_result;_tmp4DA=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp47F)->f1).fat_result;_LL2D7: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4D4,_tmp4D6,
_tmp4D8,_tmp4D3,_tmp4DA);goto _LL28B;_LL2D8: if(*((int*)_tmp47F)!= 36)goto _LL2DA;
_tmp4DB=((struct Cyc_Absyn_Swap_e_struct*)_tmp47F)->f1;_tmp4DC=((struct Cyc_Absyn_Swap_e_struct*)
_tmp47F)->f2;_LL2D9: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp4DB,_tmp4DC);goto _LL28B;
_LL2DA: if(*((int*)_tmp47F)!= 39)goto _LL28B;_tmp4DD=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp47F)->f1;_LL2DB: if(!te->allow_valueof)({void*_tmp4E8=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp4E9="valueof(-) can only occur within types";_tag_dyneither(
_tmp4E9,sizeof(char),39);}),_tag_dyneither(_tmp4E8,sizeof(void*),0));});t=Cyc_Absyn_sint_typ;
goto _LL28B;_LL28B:;}e->topt=({struct Cyc_Core_Opt*_tmp4EA=_cycalloc(sizeof(*
_tmp4EA));_tmp4EA->v=(void*)t;_tmp4EA;});}
