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
void*v;};void*Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];int isalnum(int);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(
struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*
x);void*Cyc_List_fold_right_c(void*(*f)(void*,void*,void*),void*,struct Cyc_List_List*
x,void*accum);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*
x);extern char Cyc_List_Nth[8];int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_zip3(struct Cyc_List_List*x,struct Cyc_List_List*y,
struct Cyc_List_List*z);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_tabulate(
int n,void*(*f)(int));struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct
Cyc_List_List*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{long
__pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;
struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dyneither_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,
void*));struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict d,void*k);void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict
d);struct _tuple1{void*f1;void*f2;};struct _tuple1*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple1*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_delete(struct
Cyc_Dict_Dict,void*);unsigned int Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(
struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_strdup(struct
_dyneither_ptr src);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*Cyc_Hashtable_create(
int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(struct
Cyc_Hashtable_Table*t,void*key,void*val);void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*
t,void*key);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(int,int);struct Cyc_Position_Error{
struct _dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Typerep_Int_struct{
int tag;int f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;
unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct
_tuple2{unsigned int f1;struct _dyneither_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{
int tag;struct _dyneither_ptr*f1;unsigned int f2;struct _dyneither_ptr f3;};struct
_tuple3{unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;
unsigned int f1;struct _dyneither_ptr f2;};struct _tuple4{unsigned int f1;struct
_dyneither_ptr f2;};struct Cyc_Typerep_TUnion_struct{int tag;struct _dyneither_ptr f1;
struct _dyneither_ptr f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;unsigned int f3;struct
_dyneither_ptr f4;};struct _tuple5{struct _dyneither_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);struct Cyc_timespec{long tv_sec;long tv_nsec;};struct Cyc_tm{int tm_sec;int
tm_min;int tm_hour;int tm_mday;int tm_mon;int tm_year;int tm_wday;int tm_yday;int
tm_isdst;long tm_gmtoff;const char*tm_zone;};struct Cyc_itimerspec{struct Cyc_timespec
it_interval;struct Cyc_timespec it_value;};struct _union_Nmspace_Rel_n{int tag;
struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple6{union Cyc_Absyn_Nmspace f1;struct
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
struct _tuple6*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple6*datatype_name;
struct _tuple6*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple7{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple7 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct
Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct
Cyc_List_List*targs;};struct _tuple8{void*f1;struct _tuple6*f2;struct Cyc_Core_Opt*
f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple8 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);
struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*
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
;struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple6*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple6*
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
_tuple9{void*f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple9 val;};
struct _tuple10{void*f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple10
val;};struct _tuple11{void*f1;int f2;};struct _union_Cnst_Int_c{int tag;struct
_tuple11 val;};struct _tuple12{void*f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple12 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr
val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(void*,int);
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr);struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*
rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple6*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple6*f1;};struct
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
int tag;struct Cyc_List_List*f1;};struct _tuple13{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple13*f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple6*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple6*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple6*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
_tuple14{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple14 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple14 f2;struct _tuple14 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple14 f2;};struct Cyc_Absyn_TryCatch_s_struct{
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
struct _tuple6*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple6*f1;
struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{void*sc;struct _tuple6*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct
_tuple6*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;void*ret_type;
struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;
struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple6*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple6*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};
struct Cyc_Absyn_Datatypedecl{void*sc;struct _tuple6*name;struct Cyc_List_List*tvs;
struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct
_tuple6*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple6*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple6*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple6*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple6*,struct _tuple6*);void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*
x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_star_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknownid_exp(struct _tuple6*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(
void*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(
struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple6*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*);
struct _tuple15{unsigned int f1;int f2;};struct _tuple15 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_RgnOrder_RgnPO;
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
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;int allow_valueof;};void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,
void*);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcgenrep_RepInfo{struct Cyc_List_List*
decls;struct Cyc_Absyn_Exp*exp;struct Cyc_List_List*dependencies;struct Cyc_Core_Opt*
fwd_decl;int emitted;int is_extern;};struct Cyc_Dict_Dict Cyc_Tcgenrep_empty_typerep_dict(){
return((struct Cyc_Dict_Dict(*)(int(*cmp)(void*,void*)))Cyc_Dict_empty)(Cyc_Tcutil_typecmp);}
void Cyc_Tcgenrep_print_dict_entry(void*type,struct Cyc_Tcgenrep_RepInfo*info){({
struct Cyc_Int_pa_struct _tmp4;_tmp4.tag=1;_tmp4.f1=(unsigned long)info->emitted;{
struct Cyc_Int_pa_struct _tmp3;_tmp3.tag=1;_tmp3.f1=(unsigned int)info;{struct Cyc_String_pa_struct
_tmp2;_tmp2.tag=0;_tmp2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
type));{void*_tmp0[3]={& _tmp2,& _tmp3,& _tmp4};Cyc_printf(({const char*_tmp1="(%s,%x:%d,";
_tag_dyneither(_tmp1,sizeof(char),11);}),_tag_dyneither(_tmp0,sizeof(void*),3));}}}});{
struct Cyc_List_List*p=info->dependencies;for(0;p != 0;p=p->tl){({struct Cyc_Int_pa_struct
_tmp7;_tmp7.tag=1;_tmp7.f1=(unsigned int)((struct Cyc_Tcgenrep_RepInfo*)p->hd);{
void*_tmp5[1]={& _tmp7};Cyc_printf(({const char*_tmp6="%x";_tag_dyneither(_tmp6,
sizeof(char),3);}),_tag_dyneither(_tmp5,sizeof(void*),1));}});if(p->tl != 0)({
void*_tmp8=0;Cyc_printf(({const char*_tmp9=",";_tag_dyneither(_tmp9,sizeof(char),
2);}),_tag_dyneither(_tmp8,sizeof(void*),0));});}}({void*_tmpA=0;Cyc_printf(({
const char*_tmpB=")\n";_tag_dyneither(_tmpB,sizeof(char),3);}),_tag_dyneither(
_tmpA,sizeof(void*),0));});}void Cyc_Tcgenrep_print_typerep_dict(struct Cyc_Dict_Dict
dict){((void(*)(void(*f)(void*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Dict_Dict d))
Cyc_Dict_iter)(Cyc_Tcgenrep_print_dict_entry,dict);}static int Cyc_Tcgenrep_rephash(
struct Cyc_Tcgenrep_RepInfo*ri){return(int)ri;}static int Cyc_Tcgenrep_repcmp(
struct Cyc_Tcgenrep_RepInfo*r1,struct Cyc_Tcgenrep_RepInfo*r2){unsigned int r1p;
unsigned int r2p;r1p=(unsigned int)r1;r2p=(unsigned int)r2;if(r1 < r2)return - 1;
else{if(r1 == r2)return 0;else{return 1;}}}static struct Cyc_Absyn_Datatypefield*Cyc_Tcgenrep_getField(
struct Cyc_Absyn_Datatypedecl*td,struct _tuple6*fieldname){if(td->fields == 0)({
void*_tmpC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpD="Could not find field in datatypedecl";_tag_dyneither(_tmpD,
sizeof(char),37);}),_tag_dyneither(_tmpC,sizeof(void*),0));});else{struct Cyc_List_List*
l=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;for(0;
l != 0;l=l->tl){if(!Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)l->hd)->name,
fieldname))return(struct Cyc_Absyn_Datatypefield*)l->hd;}}({void*_tmpE=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpF="Could not find field in datatypedecl";_tag_dyneither(_tmpF,sizeof(char),
37);}),_tag_dyneither(_tmpE,sizeof(void*),0));});}static char _tmp10[8]="Typerep";
static struct _dyneither_ptr Cyc_Tcgenrep_typerep_nm={_tmp10,_tmp10,_tmp10 + 8};
static struct Cyc_List_List Cyc_Tcgenrep_l2={(void*)& Cyc_Tcgenrep_typerep_nm,0};
static struct Cyc_List_List*Cyc_Tcgenrep_dfsvisit(struct Cyc_Tcgenrep_RepInfo*ri,
struct Cyc_Hashtable_Table*visited){struct _handler_cons _tmp11;_push_handler(&
_tmp11);{int _tmp13=0;if(setjmp(_tmp11.handler))_tmp13=1;if(!_tmp13){((int(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Tcgenrep_RepInfo*key))Cyc_Hashtable_lookup)(
visited,ri);{struct Cyc_List_List*_tmp14=0;_npop_handler(0);return _tmp14;};
_pop_handler();}else{void*_tmp12=(void*)_exn_thrown;void*_tmp16=_tmp12;_LL1: if(
_tmp16 != Cyc_Core_Not_found)goto _LL3;_LL2:((void(*)(struct Cyc_Hashtable_Table*t,
struct Cyc_Tcgenrep_RepInfo*key,int val))Cyc_Hashtable_insert)(visited,ri,1);{
struct Cyc_List_List*ds=0;{struct Cyc_List_List*l=ri->dependencies;for(0;l != 0;l=l->tl){
ds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
ds,Cyc_Tcgenrep_dfsvisit((struct Cyc_Tcgenrep_RepInfo*)l->hd,visited));}}if(ri->emitted
== 0){if(ri->fwd_decl != 0)ds=({struct Cyc_List_List*_tmp17=_cycalloc(sizeof(*
_tmp17));_tmp17->hd=({struct Cyc_Absyn_Decl*_tmp18[1];_tmp18[0]=(struct Cyc_Absyn_Decl*)((
struct Cyc_Core_Opt*)_check_null(ri->fwd_decl))->v;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp18,sizeof(struct Cyc_Absyn_Decl*),
1));});_tmp17->tl=ds;_tmp17;});{struct Cyc_List_List*_tmp19=({struct Cyc_List_List*
_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->hd=ri->decls;_tmp1A->tl=0;_tmp1A;});ri->emitted=
1;return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(ds,_tmp19);}}else{return ds;}}_LL3:;_LL4:(void)_throw(_tmp16);
_LL0:;}}}static struct Cyc_List_List*Cyc_Tcgenrep_dfs(struct Cyc_Tcgenrep_RepInfo*
ri){struct Cyc_Hashtable_Table*tab=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(
struct Cyc_Tcgenrep_RepInfo*,struct Cyc_Tcgenrep_RepInfo*),int(*hash)(struct Cyc_Tcgenrep_RepInfo*)))
Cyc_Hashtable_create)(53,Cyc_Tcgenrep_repcmp,Cyc_Tcgenrep_rephash);struct Cyc_List_List*
ds=Cyc_Tcgenrep_dfsvisit(ri,tab);return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_flatten)(ds);}static char _tmp1B[11]="Typestruct";static struct
_dyneither_ptr Cyc_Tcgenrep_typestruct_str={_tmp1B,_tmp1B,_tmp1B + 11};static char
_tmp1C[4]="Var";static struct _dyneither_ptr Cyc_Tcgenrep_var_str={_tmp1C,_tmp1C,
_tmp1C + 4};static char _tmp1D[4]="Int";static struct _dyneither_ptr Cyc_Tcgenrep_int_str={
_tmp1D,_tmp1D,_tmp1D + 4};static char _tmp1E[6]="Float";static struct _dyneither_ptr
Cyc_Tcgenrep_float_str={_tmp1E,_tmp1E,_tmp1E + 6};static char _tmp1F[7]="Double";
static struct _dyneither_ptr Cyc_Tcgenrep_double_str={_tmp1F,_tmp1F,_tmp1F + 7};
static char _tmp20[8]="ThinPtr";static struct _dyneither_ptr Cyc_Tcgenrep_thinptr_str={
_tmp20,_tmp20,_tmp20 + 8};static char _tmp21[7]="FatPtr";static struct _dyneither_ptr
Cyc_Tcgenrep_fatptr_str={_tmp21,_tmp21,_tmp21 + 7};static char _tmp22[6]="Tuple";
static struct _dyneither_ptr Cyc_Tcgenrep_tuple_str={_tmp22,_tmp22,_tmp22 + 6};
static char _tmp23[12]="TUnionField";static struct _dyneither_ptr Cyc_Tcgenrep_datatypefield_str={
_tmp23,_tmp23,_tmp23 + 12};static char _tmp24[7]="Struct";static struct
_dyneither_ptr Cyc_Tcgenrep_struct_str={_tmp24,_tmp24,_tmp24 + 7};static char _tmp25[
7]="TUnion";static struct _dyneither_ptr Cyc_Tcgenrep_datatype_str={_tmp25,_tmp25,
_tmp25 + 7};static char _tmp26[8]="XTUnion";static struct _dyneither_ptr Cyc_Tcgenrep_xdatatype_str={
_tmp26,_tmp26,_tmp26 + 8};static char _tmp27[6]="Union";static struct _dyneither_ptr
Cyc_Tcgenrep_union_str={_tmp27,_tmp27,_tmp27 + 6};static char _tmp28[5]="Enum";
static struct _dyneither_ptr Cyc_Tcgenrep_enum_str={_tmp28,_tmp28,_tmp28 + 5};static
char _tmp29[7]="name_t";static struct _dyneither_ptr Cyc_Tcgenrep_name_t_str={_tmp29,
_tmp29,_tmp29 + 7};static struct _tuple6*Cyc_Tcgenrep_typerep_name(struct
_dyneither_ptr*name){return({struct _tuple6*_tmp2A=_cycalloc(sizeof(*_tmp2A));
_tmp2A->f1=Cyc_Absyn_Abs_n((struct Cyc_List_List*)& Cyc_Tcgenrep_l2);_tmp2A->f2=
name;_tmp2A;});}static int Cyc_Tcgenrep_gen_id_counter=0;static struct
_dyneither_ptr*Cyc_Tcgenrep_new_gen_id(struct _dyneither_ptr name){return({struct
_dyneither_ptr*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B[0]=(struct _dyneither_ptr)({
struct Cyc_Int_pa_struct _tmp2F;_tmp2F.tag=1;_tmp2F.f1=(unsigned long)Cyc_Tcgenrep_gen_id_counter
++;{struct Cyc_String_pa_struct _tmp2E;_tmp2E.tag=0;_tmp2E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)name);{void*_tmp2C[2]={& _tmp2E,& _tmp2F};
Cyc_aprintf(({const char*_tmp2D="_gen%s_%d";_tag_dyneither(_tmp2D,sizeof(char),10);}),
_tag_dyneither(_tmp2C,sizeof(void*),2));}}});_tmp2B;});}static void Cyc_Tcgenrep_print_params(
struct Cyc_List_List*l){({void*_tmp30=0;Cyc_printf(({const char*_tmp31="<";
_tag_dyneither(_tmp31,sizeof(char),2);}),_tag_dyneither(_tmp30,sizeof(void*),0));});{
struct Cyc_List_List*p=l;for(0;p != 0;p=p->tl){({struct Cyc_Int_pa_struct _tmp35;
_tmp35.tag=1;_tmp35.f1=(unsigned long)((int)(p->tl != 0?',':' '));{struct Cyc_String_pa_struct
_tmp34;_tmp34.tag=0;_tmp34.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)p->hd));{void*_tmp32[2]={& _tmp34,& _tmp35};Cyc_printf(({const char*_tmp33="%s%c";
_tag_dyneither(_tmp33,sizeof(char),5);}),_tag_dyneither(_tmp32,sizeof(void*),2));}}});}}({
void*_tmp36=0;Cyc_printf(({const char*_tmp37=">\n";_tag_dyneither(_tmp37,sizeof(
char),3);}),_tag_dyneither(_tmp36,sizeof(void*),0));});}static void Cyc_Tcgenrep_print_tvars(
struct Cyc_List_List*l){({void*_tmp38=0;Cyc_printf(({const char*_tmp39="<";
_tag_dyneither(_tmp39,sizeof(char),2);}),_tag_dyneither(_tmp38,sizeof(void*),0));});{
struct Cyc_List_List*p=l;for(0;p != 0;p=p->tl){({struct Cyc_Int_pa_struct _tmp3D;
_tmp3D.tag=1;_tmp3D.f1=(unsigned long)((int)(p->tl != 0?',':' '));{struct Cyc_String_pa_struct
_tmp3C;_tmp3C.tag=0;_tmp3C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Tvar*)p->hd)->name);{void*_tmp3A[2]={& _tmp3C,& _tmp3D};Cyc_printf(({
const char*_tmp3B="%s%c";_tag_dyneither(_tmp3B,sizeof(char),5);}),_tag_dyneither(
_tmp3A,sizeof(void*),2));}}});}}({void*_tmp3E=0;Cyc_printf(({const char*_tmp3F=">\n";
_tag_dyneither(_tmp3F,sizeof(char),3);}),_tag_dyneither(_tmp3E,sizeof(void*),0));});}
static struct _tuple6*Cyc_Tcgenrep_toplevel_name(struct _dyneither_ptr*name){return({
struct _tuple6*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->f1=Cyc_Absyn_Rel_n(0);
_tmp40->f2=(struct _dyneither_ptr*)name;_tmp40;});}static struct _tuple6*Cyc_Tcgenrep_relative_name(
struct Cyc_List_List*pathl,struct _dyneither_ptr*name){return({struct _tuple6*
_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->f1=Cyc_Absyn_Rel_n(pathl);_tmp41->f2=(
struct _dyneither_ptr*)name;_tmp41;});}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_decl(
struct _dyneither_ptr*name,void*type,struct Cyc_Absyn_Exp*init,struct Cyc_Position_Segment*
seg){struct _tuple6*qvar=Cyc_Tcgenrep_toplevel_name(name);struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(qvar,type,init);vd->sc=(void*)0;{struct Cyc_Absyn_Var_d_struct*
r1=({struct Cyc_Absyn_Var_d_struct*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42[0]=({
struct Cyc_Absyn_Var_d_struct _tmp43;_tmp43.tag=0;_tmp43.f1=vd;_tmp43;});_tmp42;});
void*r2=(void*)r1;return Cyc_Absyn_new_decl(r2,seg);}}static struct Cyc_Absyn_Decl*
Cyc_Tcgenrep_gen_vardecl(struct _tuple6*name,void*type,struct Cyc_Absyn_Exp*init,
void*sc,struct Cyc_Position_Segment*seg){struct _tuple6*topname=Cyc_Tcgenrep_toplevel_name((*
name).f2);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(topname,type,init);vd->sc=
sc;{struct Cyc_Absyn_Var_d_struct*r1=({struct Cyc_Absyn_Var_d_struct*_tmp44=
_cycalloc(sizeof(*_tmp44));_tmp44[0]=({struct Cyc_Absyn_Var_d_struct _tmp45;_tmp45.tag=
0;_tmp45.f1=vd;_tmp45;});_tmp44;});void*r2=(void*)r1;return Cyc_Absyn_new_decl(r2,
seg);}}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_string(struct _dyneither_ptr s,
struct Cyc_Position_Segment*seg){return Cyc_Absyn_const_exp(Cyc_Absyn_String_c(s),
seg);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_string_cls(struct Cyc_Position_Segment*
seg,struct _dyneither_ptr*s){return Cyc_Absyn_const_exp(Cyc_Absyn_String_c(*s),seg);}
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_qvar_string_cls(struct Cyc_Position_Segment*
seg,struct _tuple6*s){return Cyc_Absyn_const_exp(Cyc_Absyn_String_c(*(*s).f2),seg);}
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_int(int i,struct Cyc_Position_Segment*
seg){return Cyc_Absyn_const_exp(Cyc_Absyn_Int_c((void*)0,i),seg);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_cnst_int_cls(struct Cyc_Position_Segment*seg,int i){return Cyc_Absyn_const_exp(
Cyc_Absyn_Int_c((void*)0,i),seg);}static int Cyc_Tcgenrep_size_of2int(void*sz){
void*_tmp46=sz;_LL6: if((int)_tmp46 != 0)goto _LL8;_LL7: return 8;_LL8: if((int)_tmp46
!= 1)goto _LLA;_LL9: return 16;_LLA: if((int)_tmp46 != 2)goto _LLC;_LLB: return 32;_LLC:
if((int)_tmp46 != 3)goto _LLE;_LLD: return 32;_LLE: if((int)_tmp46 != 4)goto _LL5;_LLF:
return 64;_LL5:;}static void*Cyc_Tcgenrep_datatype_typ(struct _tuple6*name){return(
void*)({struct Cyc_Absyn_DatatypeType_struct*_tmp47=_cycalloc(sizeof(*_tmp47));
_tmp47[0]=({struct Cyc_Absyn_DatatypeType_struct _tmp48;_tmp48.tag=2;_tmp48.f1=({
struct Cyc_Absyn_DatatypeInfo _tmp49;_tmp49.datatype_info=Cyc_Absyn_UnknownDatatype(({
struct Cyc_Absyn_UnknownDatatypeInfo _tmp4B;_tmp4B.name=name;_tmp4B.is_extensible=
0;_tmp4B;}));_tmp49.targs=0;_tmp49.rgn=({struct Cyc_Core_Opt*_tmp4A=_cycalloc(
sizeof(*_tmp4A));_tmp4A->v=(void*)((void*)2);_tmp4A;});_tmp49;});_tmp48;});
_tmp47;});}static void*Cyc_Tcgenrep_datatypefield_typ(struct _tuple6*name,struct
_tuple6*fieldname){return(void*)({struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C[0]=({struct Cyc_Absyn_DatatypeFieldType_struct
_tmp4D;_tmp4D.tag=3;_tmp4D.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp4E;_tmp4E.field_info=
Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp4F;
_tmp4F.datatype_name=name;_tmp4F.field_name=fieldname;_tmp4F.is_extensible=0;
_tmp4F;}));_tmp4E.targs=0;_tmp4E;});_tmp4D;});_tmp4C;});}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_call_exp(struct _tuple6*name,struct Cyc_List_List*args,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_unknowncall_exp(Cyc_Absyn_unknownid_exp(name,loc),args,loc);}
static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_datatype_constructor_decl(struct _tuple6*
datatypename,struct _tuple6*fieldname,struct _tuple6*varname,struct Cyc_List_List*
args,void*sc,struct Cyc_Position_Segment*loc){void*_tmp50=Cyc_Tcgenrep_datatypefield_typ(
datatypename,fieldname);struct Cyc_Absyn_Exp*_tmp51=Cyc_Tcgenrep_call_exp(
fieldname,args,loc);struct Cyc_Absyn_Decl*_tmp52=Cyc_Tcgenrep_gen_vardecl(varname,
_tmp50,(struct Cyc_Absyn_Exp*)_tmp51,sc,loc);return _tmp52;}struct _tuple16{struct
Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_Tcgenrep_get_second(struct _tuple16*
pair){return(*pair).f2;}struct _tuple17{struct _dyneither_ptr*f1;struct Cyc_Absyn_Exp*
f2;};static struct _dyneither_ptr*Cyc_Tcgenrep_get_first(struct _tuple17*pair){
return(*pair).f1;}static char _tmp53[5]="list";static struct _dyneither_ptr Cyc_Tcgenrep_list_str={
_tmp53,_tmp53,_tmp53 + 5};static char _tmp54[5]="List";static struct _dyneither_ptr
Cyc_Tcgenrep_List_str={_tmp54,_tmp54,_tmp54 + 5};struct _tuple18{struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_tuple2_exp_cls(
struct Cyc_Position_Segment*loc,struct _tuple18*es){struct _tuple18 _tmp56;struct Cyc_Absyn_Exp*
_tmp57;struct Cyc_Absyn_Exp*_tmp58;struct _tuple18*_tmp55=es;_tmp56=*_tmp55;_tmp57=
_tmp56.f1;_tmp58=_tmp56.f2;return Cyc_Absyn_tuple_exp(({struct Cyc_Absyn_Exp*
_tmp59[2];_tmp59[1]=_tmp58;_tmp59[0]=_tmp57;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp59,sizeof(struct Cyc_Absyn_Exp*),
2));}),loc);}struct _tuple19{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct
Cyc_Absyn_Exp*f3;};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_tuple3_exp_cls(struct
Cyc_Position_Segment*loc,struct _tuple19*es){struct _tuple19 _tmp5B;struct Cyc_Absyn_Exp*
_tmp5C;struct Cyc_Absyn_Exp*_tmp5D;struct Cyc_Absyn_Exp*_tmp5E;struct _tuple19*
_tmp5A=es;_tmp5B=*_tmp5A;_tmp5C=_tmp5B.f1;_tmp5D=_tmp5B.f2;_tmp5E=_tmp5B.f3;
return Cyc_Absyn_tuple_exp(({struct Cyc_Absyn_Exp*_tmp5F[3];_tmp5F[2]=_tmp5E;
_tmp5F[1]=_tmp5D;_tmp5F[0]=_tmp5C;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp5F,sizeof(struct Cyc_Absyn_Exp*),3));}),loc);}
static char _tmp60[5]="NULL";static struct _dyneither_ptr Cyc_Tcgenrep_null_str={
_tmp60,_tmp60,_tmp60 + 5};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_list_exp(struct
Cyc_List_List*l,struct Cyc_Position_Segment*loc){if(l == 0)return Cyc_Absyn_null_exp(
loc);return Cyc_Tcgenrep_call_exp(Cyc_Tcgenrep_toplevel_name(& Cyc_Tcgenrep_list_str),
l,loc);}struct _tuple20{void*f1;struct Cyc_Position_Segment*f2;};static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_make_offsetof_exp(struct _tuple20*typeloc,int index){struct _tuple20
_tmp62;void*_tmp63;struct Cyc_Position_Segment*_tmp64;struct _tuple20*_tmp61=
typeloc;_tmp62=*_tmp61;_tmp63=_tmp62.f1;_tmp64=_tmp62.f2;return Cyc_Absyn_offsetof_exp(
_tmp63,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp65=_cycalloc_atomic(
sizeof(*_tmp65));_tmp65[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp66;_tmp66.tag=
1;_tmp66.f1=(unsigned int)index;_tmp66;});_tmp65;}),_tmp64);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_get_and_cast_ri_exp(struct Cyc_Position_Segment*loc,struct Cyc_Tcgenrep_RepInfo*
info){return Cyc_Absyn_cast_exp(Cyc_Tcgenrep_datatype_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str)),(struct Cyc_Absyn_Exp*)_check_null(info->exp),1,(
void*)0,loc);}static struct _tuple17*Cyc_Tcgenrep_gen_id_for_exp(struct Cyc_Absyn_Exp*
e){return({struct _tuple17*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->f1=Cyc_Tcgenrep_new_gen_id(({
const char*_tmp68="tuple";_tag_dyneither(_tmp68,sizeof(char),6);}));_tmp67->f2=e;
_tmp67;});}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_decl_cls(struct _tuple20*
env,struct _tuple17*name_exp){struct _tuple20 _tmp6A;void*_tmp6B;struct Cyc_Position_Segment*
_tmp6C;struct _tuple20*_tmp69=env;_tmp6A=*_tmp69;_tmp6B=_tmp6A.f1;_tmp6C=_tmp6A.f2;{
struct _tuple17 _tmp6E;struct _dyneither_ptr*_tmp6F;struct Cyc_Absyn_Exp*_tmp70;
struct _tuple17*_tmp6D=name_exp;_tmp6E=*_tmp6D;_tmp6F=_tmp6E.f1;_tmp70=_tmp6E.f2;
return Cyc_Tcgenrep_gen_decl(_tmp6F,_tmp6B,(struct Cyc_Absyn_Exp*)_tmp70,_tmp6C);}}
struct _tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tuple21*Cyc_Tcgenrep_null_designator_exp(struct Cyc_Absyn_Exp*e){return({struct
_tuple21*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->f1=0;_tmp71->f2=e;_tmp71;});}
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_arr_init_exp(struct Cyc_List_List*l,struct
Cyc_Position_Segment*loc){struct Cyc_List_List*_tmp72=((struct Cyc_List_List*(*)(
struct _tuple21*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_null_designator_exp,l);return Cyc_Absyn_unresolvedmem_exp(0,_tmp72,
loc);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_address_exp_cls(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_address_exp(e,loc);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_unknownid_exp_cls(struct Cyc_Position_Segment*loc,struct _tuple6*e){
return Cyc_Absyn_unknownid_exp(e,loc);}static int Cyc_Tcgenrep_has_bitfield(struct
Cyc_Absyn_Aggrfield*sf){return sf->width != 0;}static int Cyc_Tcgenrep_add_bitfield_sizes(
int total,struct Cyc_Absyn_Aggrfield*sf){unsigned int _tmp74;int _tmp75;struct
_tuple15 _tmp73=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
sf->width));_tmp74=_tmp73.f1;_tmp75=_tmp73.f2;if(!_tmp75)({void*_tmp76=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp77="add_bitfield_sizes: sizeof or offsetof in bitfield size";_tag_dyneither(
_tmp77,sizeof(char),56);}),_tag_dyneither(_tmp76,sizeof(void*),0));});return(int)(
_tmp74 + total);}static void*Cyc_Tcgenrep_select_structfield_type(struct Cyc_Absyn_Aggrfield*
sf){if(Cyc_strcmp((struct _dyneither_ptr)*sf->name,({const char*_tmp78="";
_tag_dyneither(_tmp78,sizeof(char),1);}))== 0)({void*_tmp79=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7A="gen(): anonymous (padding) structfield not handled yet";
_tag_dyneither(_tmp7A,sizeof(char),55);}),_tag_dyneither(_tmp79,sizeof(void*),0));});
if(Cyc_Tcgenrep_has_bitfield(sf))({void*_tmp7B=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7C="gen(): mixed struct bitfields and union bitfields not handled";
_tag_dyneither(_tmp7C,sizeof(char),62);}),_tag_dyneither(_tmp7B,sizeof(void*),0));});
return sf->type;}struct _tuple22{struct _dyneither_ptr*f1;void*f2;};static struct
_tuple22*Cyc_Tcgenrep_select_structfield_nmtype(struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*sf->name,({const char*_tmp7D="";
_tag_dyneither(_tmp7D,sizeof(char),1);}))== 0)({void*_tmp7E=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7F="gen(): anonymous (padding) structfield not handled yet";
_tag_dyneither(_tmp7F,sizeof(char),55);}),_tag_dyneither(_tmp7E,sizeof(void*),0));});
if(Cyc_Tcgenrep_has_bitfield(sf))({void*_tmp80=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp81="gen(): mixed struct bitfields and union bitfields not handled";
_tag_dyneither(_tmp81,sizeof(char),62);}),_tag_dyneither(_tmp80,sizeof(void*),0));});
return({struct _tuple22*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->f1=sf->name;
_tmp82->f2=sf->type;_tmp82;});}struct _tuple23{int f1;struct _tuple6*f2;};struct
_tuple23*Cyc_Tcgenrep_select_enumfield_tagnm(struct Cyc_Absyn_Enumfield*ef){if(ef->tag
== 0)({void*_tmp83=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp84="Enum tag exp should be filled in by now";
_tag_dyneither(_tmp84,sizeof(char),40);}),_tag_dyneither(_tmp83,sizeof(void*),0));});
return({struct _tuple23*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->f1=(int)(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(ef->tag))).f1;_tmp85->f2=ef->name;_tmp85;});}
static struct Cyc_Dict_Dict Cyc_Tcgenrep_update_info(struct Cyc_Dict_Dict dict,void*
type,struct Cyc_List_List*decls,struct Cyc_Absyn_Exp*exp,struct Cyc_List_List*
dependencies,struct Cyc_Core_Opt*fwd_decl){struct Cyc_Tcgenrep_RepInfo**_tmp86=((
struct Cyc_Tcgenrep_RepInfo**(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup_opt)(
dict,type);if(_tmp86 != 0){if((*_tmp86)->decls != 0){Cyc_Tcgenrep_print_typerep_dict(
dict);({void*_tmp87=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp88="Updating non-forward declaration";
_tag_dyneither(_tmp88,sizeof(char),33);}),_tag_dyneither(_tmp87,sizeof(void*),0));});}(*
_tmp86)->decls=decls;(*_tmp86)->exp=exp;(*_tmp86)->dependencies=dependencies;
return dict;}else{return((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,
struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(dict,type,({struct Cyc_Tcgenrep_RepInfo*
_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->decls=decls;_tmp89->exp=exp;_tmp89->dependencies=
dependencies;_tmp89->fwd_decl=fwd_decl;_tmp89->emitted=0;_tmp89->is_extern=0;
_tmp89;}));}}static struct Cyc_Dict_Dict Cyc_Tcgenrep_make_fwd_decl_info(struct Cyc_Dict_Dict
dict,void*type,struct Cyc_Absyn_Exp*exp,struct Cyc_Core_Opt*fwd_decl,int is_extern){
struct Cyc_Tcgenrep_RepInfo**_tmp8A=((struct Cyc_Tcgenrep_RepInfo**(*)(struct Cyc_Dict_Dict
d,void*k))Cyc_Dict_lookup_opt)(dict,type);if(_tmp8A != 0){({struct Cyc_String_pa_struct
_tmp8D;_tmp8D.tag=0;_tmp8D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
type));{void*_tmp8B[1]={& _tmp8D};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp8C="Repinfo for fwd declaration of %s already exists";
_tag_dyneither(_tmp8C,sizeof(char),49);}),_tag_dyneither(_tmp8B,sizeof(void*),1));}});
return dict;}else{return((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,
struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(dict,type,({struct Cyc_Tcgenrep_RepInfo*
_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->decls=0;_tmp8E->exp=exp;_tmp8E->dependencies=
0;_tmp8E->fwd_decl=fwd_decl;_tmp8E->emitted=0;_tmp8E->is_extern=is_extern;_tmp8E;}));}}
static struct Cyc_Absyn_Tqual Cyc_Tcgenrep_tq_none={0,0,0,0,0};static struct _tuple16*
Cyc_Tcgenrep_tqual_type(struct Cyc_Absyn_Tqual*tq,void*type){return({struct
_tuple16*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->f1=*((struct Cyc_Absyn_Tqual*)
_check_null(tq));_tmp8F->f2=type;_tmp8F;});}static void*Cyc_Tcgenrep_tuple_typ(
struct Cyc_List_List*types){struct Cyc_List_List*_tmp90=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Absyn_Tqual*,void*),struct Cyc_Absyn_Tqual*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tqual_type,(struct Cyc_Absyn_Tqual*)&
Cyc_Tcgenrep_tq_none,types);struct Cyc_Absyn_TupleType_struct*tuple_type_base=({
struct Cyc_Absyn_TupleType_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({
struct Cyc_Absyn_TupleType_struct _tmp92;_tmp92.tag=9;_tmp92.f1=_tmp90;_tmp92;});
_tmp91;});void*tuple_type=(void*)tuple_type_base;return tuple_type;}static struct
_tuple21*Cyc_Tcgenrep_array_decls(void*type,struct Cyc_List_List*exps,struct Cyc_Position_Segment*
loc){struct Cyc_List_List*_tmp93=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_gen_id_for_exp,
exps);struct Cyc_List_List*_tmp94=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*
f)(struct _tuple20*,struct _tuple17*),struct _tuple20*env,struct Cyc_List_List*x))
Cyc_List_map_c)(Cyc_Tcgenrep_gen_decl_cls,({struct _tuple20*_tmpA2=_cycalloc(
sizeof(*_tmpA2));_tmpA2->f1=type;_tmpA2->f2=loc;_tmpA2;}),_tmp93);struct Cyc_List_List*
_tmp95=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _tuple17*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_first,_tmp93);struct Cyc_List_List*
_tmp96=((struct Cyc_List_List*(*)(struct _tuple6*(*f)(struct _dyneither_ptr*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_toplevel_name,_tmp95);struct
Cyc_List_List*_tmp97=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct
Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_unknownid_exp_cls,loc,_tmp96);struct Cyc_List_List*
_tmp98=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct Cyc_Absyn_Exp*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_address_exp_cls,loc,_tmp97);struct Cyc_Absyn_Exp*_tmp99=Cyc_Tcgenrep_arr_init_exp(
_tmp98,loc);struct _dyneither_ptr*_tmp9A=Cyc_Tcgenrep_new_gen_id(({const char*
_tmpA1="arr";_tag_dyneither(_tmpA1,sizeof(char),4);}));void*_tmp9B=Cyc_Absyn_at_typ(
type,(void*)2,Cyc_Tcgenrep_tq_none,Cyc_Absyn_false_conref);void*_tmp9C=Cyc_Absyn_array_typ(
_tmp9B,Cyc_Tcgenrep_tq_none,0,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Decl*
_tmp9D=Cyc_Tcgenrep_gen_decl(_tmp9A,_tmp9C,(struct Cyc_Absyn_Exp*)_tmp99,loc);
struct Cyc_Absyn_Exp*_tmp9E=Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(
_tmp9A),loc);return({struct _tuple21*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp94,({struct Cyc_Absyn_Decl*_tmpA0[1];_tmpA0[0]=_tmp9D;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA0,sizeof(struct Cyc_Absyn_Decl*),
1));}));_tmp9F->f2=_tmp9E;_tmp9F;});}static void*Cyc_Tcgenrep_check_datatypefield_and_get_type(
struct Cyc_Absyn_Datatypefield*tuf){return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3[0]=({struct Cyc_Absyn_TupleType_struct
_tmpA4;_tmpA4.tag=9;_tmpA4.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(({struct _tuple16*_tmpA5[1];_tmpA5[0]=({
struct _tuple16*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->f1=Cyc_Tcgenrep_tq_none;
_tmpA6->f2=Cyc_Absyn_uint_typ;_tmpA6;});((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA5,sizeof(struct _tuple16*),1));}),
tuf->typs);_tmpA4;});_tmpA3;});}static struct _tuple6*Cyc_Tcgenrep_check_datatypefield_and_get_name(
struct Cyc_Absyn_Datatypefield*tuf){return tuf->name;}struct _tuple24{struct _tuple6*
f1;void*f2;};static struct _tuple24*Cyc_Tcgenrep_check_datatypefield_and_get_nmtype(
struct Cyc_Absyn_Datatypefield*tuf){return({struct _tuple24*_tmpA7=_cycalloc(
sizeof(*_tmpA7));_tmpA7->f1=tuf->name;_tmpA7->f2=(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=({struct Cyc_Absyn_TupleType_struct
_tmpA9;_tmpA9.tag=9;_tmpA9.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(({struct _tuple16*_tmpAA[1];_tmpAA[0]=({
struct _tuple16*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->f1=Cyc_Tcgenrep_tq_none;
_tmpAB->f2=Cyc_Absyn_uint_typ;_tmpAB;});((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAA,sizeof(struct _tuple16*),1));}),
tuf->typs);_tmpA9;});_tmpA8;});_tmpA7;});}static struct _tuple22*Cyc_Tcgenrep_check_xdatatypefield_and_get_name_type(
struct Cyc_Absyn_Datatypefield*tuf){return({struct _tuple22*_tmpAC=_cycalloc(
sizeof(*_tmpAC));_tmpAC->f1=(*tuf->name).f2;_tmpAC->f2=(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({struct Cyc_Absyn_TupleType_struct
_tmpAE;_tmpAE.tag=9;_tmpAE.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(({struct _tuple16*_tmpAF[1];_tmpAF[0]=({
struct _tuple16*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->f1=Cyc_Tcgenrep_tq_none;
_tmpB0->f2=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Tcgenrep_tq_none,
Cyc_Absyn_true_conref);_tmpB0;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpAF,sizeof(struct _tuple16*),1));}),tuf->typs);
_tmpAE;});_tmpAD;});_tmpAC;});}static int Cyc_Tcgenrep_filter_empty_datatypefield(
struct Cyc_Absyn_Datatypefield*tuf){return tuf->typs != 0;}static int Cyc_Tcgenrep_filter_nonempty_datatypefield(
struct Cyc_Absyn_Datatypefield*tuf){return tuf->typs == 0;}static struct
_dyneither_ptr*Cyc_Tcgenrep_get_datatypefield_name(struct Cyc_Absyn_Datatypefield*
tuf){return(*tuf->name).f2;}struct Cyc_Absyn_Aggrfield*Cyc_Tcgenrep_substitute_structfield_type(
struct Cyc_List_List*subst,struct Cyc_Absyn_Aggrfield*sf){return({struct Cyc_Absyn_Aggrfield*
_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->name=sf->name;_tmpB1->tq=sf->tq;_tmpB1->type=
Cyc_Tcutil_substitute(subst,sf->type);_tmpB1->width=sf->width;_tmpB1->attributes=
sf->attributes;_tmpB1;});}struct _tuple16*Cyc_Tcgenrep_substitute_tqual_type(
struct Cyc_List_List*subst,struct _tuple16*pair){struct _tuple16 _tmpB3;struct Cyc_Absyn_Tqual
_tmpB4;void*_tmpB5;struct _tuple16*_tmpB2=pair;_tmpB3=*_tmpB2;_tmpB4=_tmpB3.f1;
_tmpB5=_tmpB3.f2;return({struct _tuple16*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->f1=
_tmpB4;_tmpB6->f2=Cyc_Tcutil_substitute(subst,_tmpB5);_tmpB6;});}struct Cyc_Absyn_Datatypefield*
Cyc_Tcgenrep_substitute_datatypefield_type(struct Cyc_List_List*subst,struct Cyc_Absyn_Datatypefield*
tf){return({struct Cyc_Absyn_Datatypefield*_tmpB7=_cycalloc(sizeof(*_tmpB7));
_tmpB7->name=tf->name;_tmpB7->typs=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(
struct Cyc_List_List*,struct _tuple16*),struct Cyc_List_List*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_substitute_tqual_type,subst,tf->typs);_tmpB7->loc=
tf->loc;_tmpB7->sc=tf->sc;_tmpB7;});}void*Cyc_Tcgenrep_monomorphize_type(void*
type){void*_tmpB8=Cyc_Tcutil_compress(type);struct Cyc_Absyn_AggrInfo _tmpB9;union
Cyc_Absyn_AggrInfoU _tmpBA;struct Cyc_List_List*_tmpBB;struct Cyc_Absyn_DatatypeInfo
_tmpBC;union Cyc_Absyn_DatatypeInfoU _tmpBD;struct Cyc_Absyn_Datatypedecl**_tmpBE;
struct Cyc_Absyn_Datatypedecl*_tmpBF;struct Cyc_List_List*_tmpC0;struct Cyc_Core_Opt*
_tmpC1;_LL11: if(_tmpB8 <= (void*)4)goto _LL15;if(*((int*)_tmpB8)!= 10)goto _LL13;
_tmpB9=((struct Cyc_Absyn_AggrType_struct*)_tmpB8)->f1;_tmpBA=_tmpB9.aggr_info;
_tmpBB=_tmpB9.targs;_LL12: {struct Cyc_Absyn_Aggrdecl*_tmpC2=Cyc_Absyn_get_known_aggrdecl(
_tmpBA);struct Cyc_List_List*_tmpC3=_tmpC2->tvs;if(Cyc_List_length(_tmpBB)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC3))({struct Cyc_Int_pa_struct
_tmpC7;_tmpC7.tag=1;_tmpC7.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpC3);{struct Cyc_Int_pa_struct _tmpC6;_tmpC6.tag=1;_tmpC6.f1=(unsigned long)Cyc_List_length(
_tmpBB);{void*_tmpC4[2]={& _tmpC6,& _tmpC7};((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpC5="gen(): number of params %d differs from number of tyvars %d";
_tag_dyneither(_tmpC5,sizeof(char),60);}),_tag_dyneither(_tmpC4,sizeof(void*),2));}}});{
struct Cyc_List_List*_tmpC8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(_tmpC3,_tmpBB);struct Cyc_List_List*fields=0;
if(_tmpC2->impl != 0)fields=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_substitute_structfield_type,
_tmpC8,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC2->impl))->fields);{
struct Cyc_Absyn_AggrdeclImpl*_tmpC9=_tmpC2->impl == 0?0:({struct Cyc_Absyn_AggrdeclImpl*
_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->exist_vars=0;_tmpCF->rgn_po=0;_tmpCF->fields=
fields;_tmpCF->tagged=0;_tmpCF;});struct Cyc_Absyn_Aggrdecl*ad2=({struct Cyc_Absyn_Aggrdecl*
_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->kind=_tmpC2->kind;_tmpCE->sc=_tmpC2->sc;
_tmpCE->name=_tmpC2->name;_tmpCE->tvs=0;_tmpCE->impl=_tmpC9;_tmpCE->attributes=
_tmpC2->attributes;_tmpCE;});return(void*)({struct Cyc_Absyn_AggrType_struct*
_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_Absyn_AggrType_struct
_tmpCB;_tmpCB.tag=10;_tmpCB.f1=({struct Cyc_Absyn_AggrInfo _tmpCC;_tmpCC.aggr_info=
Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpCD=_cycalloc(sizeof(*_tmpCD));
_tmpCD[0]=ad2;_tmpCD;}));_tmpCC.targs=0;_tmpCC;});_tmpCB;});_tmpCA;});}}}_LL13:
if(*((int*)_tmpB8)!= 2)goto _LL15;_tmpBC=((struct Cyc_Absyn_DatatypeType_struct*)
_tmpB8)->f1;_tmpBD=_tmpBC.datatype_info;if((_tmpBD.KnownDatatype).tag != 2)goto
_LL15;_tmpBE=(struct Cyc_Absyn_Datatypedecl**)(_tmpBD.KnownDatatype).val;_tmpBF=*
_tmpBE;_tmpC0=_tmpBC.targs;_tmpC1=_tmpBC.rgn;_LL14: {struct Cyc_List_List*_tmpD0=
_tmpBF->tvs;if(Cyc_List_length(_tmpC0)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpD0))({struct Cyc_Int_pa_struct _tmpD4;_tmpD4.tag=1;_tmpD4.f1=(unsigned long)((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD0);{struct Cyc_Int_pa_struct
_tmpD3;_tmpD3.tag=1;_tmpD3.f1=(unsigned long)Cyc_List_length(_tmpC0);{void*
_tmpD1[2]={& _tmpD3,& _tmpD4};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpD2="gen(): number of params %d differs from number of tyvars %d";
_tag_dyneither(_tmpD2,sizeof(char),60);}),_tag_dyneither(_tmpD1,sizeof(void*),2));}}});{
struct Cyc_List_List*_tmpD5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(_tmpD0,_tmpC0);struct Cyc_Core_Opt*fields=0;
if(_tmpBF->fields != 0)fields=({struct Cyc_Core_Opt*_tmpD6=_cycalloc(sizeof(*
_tmpD6));_tmpD6->v=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Datatypefield*(*f)(
struct Cyc_List_List*,struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_substitute_datatypefield_type,
_tmpD5,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpBF->fields))->v);
_tmpD6;});{struct Cyc_Absyn_Datatypedecl*_tmpD7=({struct Cyc_Absyn_Datatypedecl*
_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->sc=_tmpBF->sc;_tmpDE->name=_tmpBF->name;
_tmpDE->tvs=0;_tmpDE->fields=fields;_tmpDE->is_extensible=_tmpBF->is_extensible;
_tmpDE;});struct Cyc_Absyn_DatatypeType_struct*_tmpD8=({struct Cyc_Absyn_DatatypeType_struct*
_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA[0]=({struct Cyc_Absyn_DatatypeType_struct
_tmpDB;_tmpDB.tag=2;_tmpDB.f1=({struct Cyc_Absyn_DatatypeInfo _tmpDC;_tmpDC.datatype_info=
Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_tmpDD=_cycalloc(sizeof(*
_tmpDD));_tmpDD[0]=_tmpD7;_tmpDD;}));_tmpDC.targs=_tmpC0;_tmpDC.rgn=_tmpC1;
_tmpDC;});_tmpDB;});_tmpDA;});struct Cyc_Position_Segment*_tmpD9=Cyc_Position_segment_of_abs(
0,0);return(void*)_tmpD8;}}}_LL15:;_LL16: return type;_LL10:;}struct _dyneither_ptr
Cyc_Tcgenrep_make_type_cstring(void*t){struct _dyneither_ptr s=Cyc_strdup((struct
_dyneither_ptr)Cyc_Absynpp_typ2cstring(t));{int i=0;for(0;i < Cyc_strlen((struct
_dyneither_ptr)s);++ i){if(*((char*)_check_dyneither_subscript(s,sizeof(char),i))
== ' ')({struct _dyneither_ptr _tmpDF=_dyneither_ptr_plus(s,sizeof(char),i);char
_tmpE0=*((char*)_check_dyneither_subscript(_tmpDF,sizeof(char),0));char _tmpE1='_';
if(_get_dyneither_size(_tmpDF,sizeof(char))== 1  && (_tmpE0 == '\000'  && _tmpE1 != '\000'))
_throw_arraybounds();*((char*)_tmpDF.curr)=_tmpE1;});else{if(!isalnum((int)*((
char*)_check_dyneither_subscript(s,sizeof(char),i))) && *((char*)
_check_dyneither_subscript(s,sizeof(char),i))!= '_')({struct _dyneither_ptr _tmpE2=
_dyneither_ptr_plus(s,sizeof(char),i);char _tmpE3=*((char*)
_check_dyneither_subscript(_tmpE2,sizeof(char),0));char _tmpE4=(char)('0' + *((
char*)_check_dyneither_subscript(s,sizeof(char),i))% 10);if(_get_dyneither_size(
_tmpE2,sizeof(char))== 1  && (_tmpE3 == '\000'  && _tmpE4 != '\000'))
_throw_arraybounds();*((char*)_tmpE2.curr)=_tmpE4;});}}}return(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)s,({const char*_tmpE5="_rep";
_tag_dyneither(_tmpE5,sizeof(char),5);}));}struct _tuple25{struct Cyc_Dict_Dict f1;
struct Cyc_Tcgenrep_RepInfo*f2;};static struct _tuple25*Cyc_Tcgenrep_lookupRep(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*loc,
void*type);struct _tuple26{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct
_tuple27{struct Cyc_Tcenv_Tenv*f1;struct Cyc_Position_Segment*f2;};static struct
_tuple26*Cyc_Tcgenrep_lookupRepsCls(struct _tuple27*env,void*type,struct _tuple26*
carry){struct _tuple25 _tmpE7;struct Cyc_Dict_Dict _tmpE8;struct Cyc_Tcgenrep_RepInfo*
_tmpE9;struct _tuple25*_tmpE6=Cyc_Tcgenrep_lookupRep((*env).f1,(*carry).f1,(*env).f2,
type);_tmpE7=*_tmpE6;_tmpE8=_tmpE7.f1;_tmpE9=_tmpE7.f2;return({struct _tuple26*
_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->f1=_tmpE8;_tmpEA->f2=({struct Cyc_List_List*
_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->hd=_tmpE9;_tmpEB->tl=(*carry).f2;
_tmpEB;});_tmpEA;});}static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepTuple(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*loc,struct
_tuple6*varname,void*sc,void*type,struct Cyc_List_List*types){struct Cyc_Absyn_Exp*
_tmpEC=Cyc_Absyn_sizeoftyp_exp(type,loc);struct _tuple27*_tmpED=({struct _tuple27*
_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->f1=te;_tmp102->f2=loc;_tmp102;});
struct _tuple26*_tmpEE=({struct _tuple26*_tmp101=_cycalloc(sizeof(*_tmp101));
_tmp101->f1=dict;_tmp101->f2=0;_tmp101;});struct _tuple26 _tmpF0;struct Cyc_Dict_Dict
_tmpF1;struct Cyc_List_List*_tmpF2;struct _tuple26*_tmpEF=((struct _tuple26*(*)(
struct _tuple26*(*f)(struct _tuple27*,void*,struct _tuple26*),struct _tuple27*,
struct Cyc_List_List*x,struct _tuple26*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmpED,types,_tmpEE);_tmpF0=*_tmpEF;_tmpF1=_tmpF0.f1;_tmpF2=_tmpF0.f2;dict=
_tmpF1;{struct Cyc_List_List*_tmpF3=((struct Cyc_List_List*(*)(int n,int(*f)(int)))
Cyc_List_tabulate)(Cyc_List_length(types),(int(*)(int))Cyc_Core_identity);struct
Cyc_List_List*_tmpF4=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct
_tuple20*,int),struct _tuple20*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,({
struct _tuple20*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100->f1=type;_tmp100->f2=
loc;_tmp100;}),_tmpF3);struct Cyc_List_List*_tmpF5=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,
loc,_tmpF2);struct Cyc_List_List*_tmpF6=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple18*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmpF4,_tmpF5));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*_tmpFF[2];_tmpFF[1]=
Cyc_Tcgenrep_datatype_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmpFF[0]=Cyc_Absyn_uint_typ;Cyc_List_list(_tag_dyneither(_tmpFF,sizeof(void*),2));}));
struct _tuple21 _tmpF8;struct Cyc_List_List*_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;
struct _tuple21*_tmpF7=Cyc_Tcgenrep_array_decls(tuple_type,_tmpF6,loc);_tmpF8=*
_tmpF7;_tmpF9=_tmpF8.f1;_tmpFA=_tmpF8.f2;{struct Cyc_Absyn_Decl*_tmpFB=Cyc_Tcgenrep_datatype_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tuple_str),varname,({struct Cyc_Absyn_Exp*_tmpFE[2];_tmpFE[1]=_tmpFA;
_tmpFE[0]=_tmpEC;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpFE,sizeof(struct Cyc_Absyn_Exp*),2));}),sc,loc);struct Cyc_List_List*
_tmpFC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmpF9,({struct Cyc_Absyn_Decl*_tmpFD[1];_tmpFD[0]=_tmpFB;((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpFD,sizeof(struct Cyc_Absyn_Decl*),1));}));return Cyc_Tcgenrep_update_info(dict,
type,_tmpFC,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmpF2,0);}}}static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepDatatypefield(
struct _tuple6*tname,struct _tuple6*fname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict
dict,struct Cyc_Position_Segment*loc,struct _tuple6*varname,void*sc,void*type,
struct Cyc_List_List*types){struct Cyc_Absyn_Exp*_tmp103=Cyc_Absyn_sizeoftyp_exp(
type,loc);struct _tuple27*_tmp104=({struct _tuple27*_tmp11B=_cycalloc(sizeof(*
_tmp11B));_tmp11B->f1=te;_tmp11B->f2=loc;_tmp11B;});struct _tuple26*_tmp105=({
struct _tuple26*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->f1=dict;_tmp11A->f2=
0;_tmp11A;});struct _tuple26 _tmp107;struct Cyc_Dict_Dict _tmp108;struct Cyc_List_List*
_tmp109;struct _tuple26*_tmp106=((struct _tuple26*(*)(struct _tuple26*(*f)(struct
_tuple27*,void*,struct _tuple26*),struct _tuple27*,struct Cyc_List_List*x,struct
_tuple26*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmp104,types,
_tmp105);_tmp107=*_tmp106;_tmp108=_tmp107.f1;_tmp109=_tmp107.f2;dict=_tmp108;{
struct Cyc_List_List*_tmp10A=((struct Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(
Cyc_List_length(types),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*
_tmp10B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple20*,int),
struct _tuple20*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,({
struct _tuple20*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->f1=type;_tmp119->f2=
loc;_tmp119;}),_tmp10A);struct Cyc_List_List*_tmp10C=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,
loc,_tmp109);struct Cyc_List_List*_tmp10D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple18*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp10B,_tmp10C));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*_tmp118[2];
_tmp118[1]=Cyc_Tcgenrep_datatype_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp118[0]=Cyc_Absyn_uint_typ;Cyc_List_list(_tag_dyneither(_tmp118,sizeof(void*),
2));}));struct Cyc_Absyn_Exp*_tmp10E=Cyc_Tcgenrep_cnst_string(*(*tname).f2,loc);
struct Cyc_Absyn_Exp*_tmp10F=Cyc_Tcgenrep_cnst_string(*(*fname).f2,loc);struct
_tuple21 _tmp111;struct Cyc_List_List*_tmp112;struct Cyc_Absyn_Exp*_tmp113;struct
_tuple21*_tmp110=Cyc_Tcgenrep_array_decls(tuple_type,_tmp10D,loc);_tmp111=*
_tmp110;_tmp112=_tmp111.f1;_tmp113=_tmp111.f2;{struct Cyc_Absyn_Decl*_tmp114=Cyc_Tcgenrep_datatype_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_datatypefield_str),varname,({struct Cyc_Absyn_Exp*_tmp117[4];_tmp117[
3]=_tmp113;_tmp117[2]=_tmp103;_tmp117[1]=_tmp10F;_tmp117[0]=_tmp10E;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp117,sizeof(struct Cyc_Absyn_Exp*),
4));}),sc,loc);struct Cyc_List_List*_tmp115=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp112,({struct Cyc_Absyn_Decl*
_tmp116[1];_tmp116[0]=_tmp114;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp116,sizeof(struct Cyc_Absyn_Decl*),1));}));
return Cyc_Tcgenrep_update_info(dict,type,_tmp115,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp109,0);}}}static struct Cyc_Dict_Dict
Cyc_Tcgenrep_buildRepStruct(struct Cyc_Core_Opt*sname,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*loc,struct _tuple6*varname,
void*sc,void*type,struct Cyc_List_List*nmtypes){struct Cyc_Absyn_Exp*_tmp11C=Cyc_Absyn_sizeoftyp_exp(
type,loc);struct _tuple27*_tmp11D=({struct _tuple27*_tmp13A=_cycalloc(sizeof(*
_tmp13A));_tmp13A->f1=te;_tmp13A->f2=loc;_tmp13A;});struct _tuple26*_tmp11E=({
struct _tuple26*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->f1=dict;_tmp139->f2=
0;_tmp139;});struct Cyc_List_List*_tmp120;struct Cyc_List_List*_tmp121;struct
_tuple0 _tmp11F=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(nmtypes);
_tmp120=_tmp11F.f1;_tmp121=_tmp11F.f2;{struct _tuple26 _tmp123;struct Cyc_Dict_Dict
_tmp124;struct Cyc_List_List*_tmp125;struct _tuple26*_tmp122=((struct _tuple26*(*)(
struct _tuple26*(*f)(struct _tuple27*,void*,struct _tuple26*),struct _tuple27*,
struct Cyc_List_List*x,struct _tuple26*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp11D,_tmp121,_tmp11E);_tmp123=*_tmp122;_tmp124=_tmp123.f1;_tmp125=_tmp123.f2;
dict=_tmp124;{struct Cyc_List_List*_tmp126=((struct Cyc_List_List*(*)(int n,int(*f)(
int)))Cyc_List_tabulate)(Cyc_List_length(_tmp121),(int(*)(int))Cyc_Core_identity);
struct Cyc_List_List*_tmp127=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct _tuple20*,int),struct _tuple20*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_make_offsetof_exp,({struct _tuple20*_tmp138=_cycalloc(sizeof(*
_tmp138));_tmp138->f1=type;_tmp138->f2=loc;_tmp138;}),_tmp126);struct Cyc_List_List*
_tmp128=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _dyneither_ptr*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_cnst_string_cls,loc,_tmp120);struct Cyc_List_List*_tmp129=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,
loc,_tmp125);struct Cyc_List_List*_tmp12A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple19*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple3_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*
z))Cyc_List_zip3)(_tmp127,_tmp128,_tmp129));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({
void*_tmp137[3];_tmp137[2]=Cyc_Tcgenrep_datatype_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str));_tmp137[1]=Cyc_Absyn_const_string_typ((void*)2);
_tmp137[0]=Cyc_Absyn_uint_typ;Cyc_List_list(_tag_dyneither(_tmp137,sizeof(void*),
3));}));struct _tuple21 _tmp12C;struct Cyc_List_List*_tmp12D;struct Cyc_Absyn_Exp*
_tmp12E;struct _tuple21*_tmp12B=Cyc_Tcgenrep_array_decls(tuple_type,_tmp12A,loc);
_tmp12C=*_tmp12B;_tmp12D=_tmp12C.f1;_tmp12E=_tmp12C.f2;{struct Cyc_Absyn_Exp*name;
if(sname == 0)name=Cyc_Absyn_null_exp(loc);else{struct _dyneither_ptr*_tmp12F=Cyc_Tcgenrep_new_gen_id(({
const char*_tmp132="name";_tag_dyneither(_tmp132,sizeof(char),5);}));struct Cyc_Absyn_Decl*
_tmp130=Cyc_Tcgenrep_gen_decl(_tmp12F,Cyc_Absyn_const_string_typ((void*)2),(
struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*((struct _tuple6*)sname->v)).f2,
loc),loc);_tmp12D=({struct Cyc_List_List*_tmp131=_cycalloc(sizeof(*_tmp131));
_tmp131->hd=_tmp130;_tmp131->tl=_tmp12D;_tmp131;});name=Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(_tmp12F),loc),loc);}{struct
Cyc_Absyn_Decl*_tmp133=Cyc_Tcgenrep_datatype_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_struct_str),
varname,({struct Cyc_Absyn_Exp*_tmp136[3];_tmp136[2]=_tmp12E;_tmp136[1]=_tmp11C;
_tmp136[0]=name;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp136,sizeof(struct Cyc_Absyn_Exp*),3));}),sc,loc);struct Cyc_List_List*
_tmp134=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp12D,({struct Cyc_Absyn_Decl*_tmp135[1];_tmp135[0]=
_tmp133;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp135,sizeof(struct Cyc_Absyn_Decl*),1));}));return Cyc_Tcgenrep_update_info(
dict,type,_tmp134,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp125,0);}}}}}static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepUnion(
struct Cyc_Core_Opt*uname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct
Cyc_Position_Segment*loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*
nmtypes){struct Cyc_Absyn_Exp*_tmp13B=Cyc_Absyn_sizeoftyp_exp(type,loc);struct
_tuple27*_tmp13C=({struct _tuple27*_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->f1=
te;_tmp156->f2=loc;_tmp156;});struct _tuple26*_tmp13D=({struct _tuple26*_tmp155=
_cycalloc(sizeof(*_tmp155));_tmp155->f1=dict;_tmp155->f2=0;_tmp155;});struct Cyc_List_List*
_tmp13F;struct Cyc_List_List*_tmp140;struct _tuple0 _tmp13E=((struct _tuple0(*)(
struct Cyc_List_List*x))Cyc_List_split)(nmtypes);_tmp13F=_tmp13E.f1;_tmp140=
_tmp13E.f2;{struct _tuple26 _tmp142;struct Cyc_Dict_Dict _tmp143;struct Cyc_List_List*
_tmp144;struct _tuple26*_tmp141=((struct _tuple26*(*)(struct _tuple26*(*f)(struct
_tuple27*,void*,struct _tuple26*),struct _tuple27*,struct Cyc_List_List*x,struct
_tuple26*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmp13C,
_tmp140,_tmp13D);_tmp142=*_tmp141;_tmp143=_tmp142.f1;_tmp144=_tmp142.f2;dict=
_tmp143;{struct Cyc_List_List*_tmp145=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _dyneither_ptr*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,loc,
_tmp13F);struct Cyc_List_List*_tmp146=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp144);struct Cyc_List_List*_tmp147=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple18*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp145,_tmp146));void*_tmp148=Cyc_Tcgenrep_tuple_typ(({void*_tmp154[2];_tmp154[
1]=Cyc_Tcgenrep_datatype_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp154[0]=Cyc_Absyn_const_string_typ((void*)2);Cyc_List_list(_tag_dyneither(
_tmp154,sizeof(void*),2));}));struct _tuple21 _tmp14A;struct Cyc_List_List*_tmp14B;
struct Cyc_Absyn_Exp*_tmp14C;struct _tuple21*_tmp149=Cyc_Tcgenrep_array_decls(
_tmp148,_tmp147,loc);_tmp14A=*_tmp149;_tmp14B=_tmp14A.f1;_tmp14C=_tmp14A.f2;{
struct Cyc_Absyn_Exp*name;if(uname == 0)name=Cyc_Absyn_null_exp(loc);else{struct
_dyneither_ptr*_tmp14D=Cyc_Tcgenrep_new_gen_id(({const char*_tmp150="name";
_tag_dyneither(_tmp150,sizeof(char),5);}));struct Cyc_Absyn_Decl*_tmp14E=Cyc_Tcgenrep_gen_decl(
_tmp14D,Cyc_Absyn_const_string_typ((void*)2),(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*((
struct _tuple6*)uname->v)).f2,loc),loc);_tmp14B=({struct Cyc_List_List*_tmp14F=
_cycalloc(sizeof(*_tmp14F));_tmp14F->hd=_tmp14E;_tmp14F->tl=_tmp14B;_tmp14F;});
name=Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(
_tmp14D),loc),loc);}{struct Cyc_Absyn_Decl*_tmp151=Cyc_Tcgenrep_datatype_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_union_str),varname,({struct Cyc_Absyn_Exp*_tmp153[3];_tmp153[2]=
_tmp14C;_tmp153[1]=_tmp13B;_tmp153[0]=name;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp153,sizeof(struct Cyc_Absyn_Exp*),
3));}),sc,loc);return Cyc_Tcgenrep_update_info(dict,type,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp14B,({
struct Cyc_Absyn_Decl*_tmp152[1];_tmp152[0]=_tmp151;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp152,sizeof(struct Cyc_Absyn_Decl*),
1));})),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp144,0);}}}}}static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepEnum(
struct _tuple6**ename,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*
loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*tagnms){struct
Cyc_Absyn_Exp*_tmp157=Cyc_Absyn_sizeoftyp_exp(type,loc);struct Cyc_List_List*
_tmp159;struct Cyc_List_List*_tmp15A;struct _tuple0 _tmp158=((struct _tuple0(*)(
struct Cyc_List_List*x))Cyc_List_split)(tagnms);_tmp159=_tmp158.f1;_tmp15A=
_tmp158.f2;{struct Cyc_List_List*_tmp15B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
_tmp15A);struct Cyc_List_List*_tmp15C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp159);struct Cyc_List_List*
_tmp15D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple18*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(_tmp15C,_tmp15B));void*_tmp15E=Cyc_Tcgenrep_tuple_typ(({
void*_tmp16A[2];_tmp16A[1]=Cyc_Absyn_const_string_typ((void*)2);_tmp16A[0]=Cyc_Absyn_uint_typ;
Cyc_List_list(_tag_dyneither(_tmp16A,sizeof(void*),2));}));struct _tuple21 _tmp160;
struct Cyc_List_List*_tmp161;struct Cyc_Absyn_Exp*_tmp162;struct _tuple21*_tmp15F=
Cyc_Tcgenrep_array_decls(_tmp15E,_tmp15D,loc);_tmp160=*_tmp15F;_tmp161=_tmp160.f1;
_tmp162=_tmp160.f2;{struct Cyc_Absyn_Exp*name;if(ename == 0)name=Cyc_Absyn_null_exp(
loc);else{struct _dyneither_ptr*_tmp163=Cyc_Tcgenrep_new_gen_id(({const char*
_tmp166="name";_tag_dyneither(_tmp166,sizeof(char),5);}));struct Cyc_Absyn_Decl*
_tmp164=Cyc_Tcgenrep_gen_decl(_tmp163,Cyc_Absyn_const_string_typ((void*)2),(
struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*(*ename)).f2,loc),loc);_tmp161=({
struct Cyc_List_List*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165->hd=_tmp164;
_tmp165->tl=_tmp161;_tmp165;});name=Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
Cyc_Tcgenrep_toplevel_name(_tmp163),loc),loc);}{struct Cyc_Absyn_Decl*_tmp167=Cyc_Tcgenrep_datatype_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_enum_str),varname,({struct Cyc_Absyn_Exp*_tmp169[3];_tmp169[2]=
_tmp162;_tmp169[1]=_tmp157;_tmp169[0]=name;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp169,sizeof(struct Cyc_Absyn_Exp*),
3));}),sc,loc);return Cyc_Tcgenrep_update_info(dict,type,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp161,({
struct Cyc_Absyn_Decl*_tmp168[1];_tmp168[0]=_tmp167;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp168,sizeof(struct Cyc_Absyn_Decl*),
1));})),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),0,0);}}}}static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepDatatype(
struct _tuple6*tname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*
loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*tonames,struct
Cyc_List_List*nmtypes){struct Cyc_List_List*_tmp16B=((struct Cyc_List_List*(*)(int
n,int(*f)(int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
tonames),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmp16C=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp16B);
struct Cyc_List_List*_tmp16D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
tonames);struct Cyc_List_List*_tmp16E=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp16C,_tmp16D);struct Cyc_List_List*
_tmp16F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple18*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp16E);void*_tmp170=Cyc_Tcgenrep_tuple_typ(({
void*_tmp18F[2];_tmp18F[1]=Cyc_Absyn_const_string_typ((void*)2);_tmp18F[0]=Cyc_Absyn_uint_typ;
Cyc_List_list(_tag_dyneither(_tmp18F,sizeof(void*),2));}));struct _tuple21 _tmp172;
struct Cyc_List_List*_tmp173;struct Cyc_Absyn_Exp*_tmp174;struct _tuple21*_tmp171=
Cyc_Tcgenrep_array_decls(_tmp170,_tmp16F,loc);_tmp172=*_tmp171;_tmp173=_tmp172.f1;
_tmp174=_tmp172.f2;{struct _tuple27*_tmp175=({struct _tuple27*_tmp18E=_cycalloc(
sizeof(*_tmp18E));_tmp18E->f1=te;_tmp18E->f2=loc;_tmp18E;});struct _tuple26*
_tmp176=({struct _tuple26*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->f1=dict;
_tmp18D->f2=0;_tmp18D;});struct Cyc_List_List*_tmp178;struct Cyc_List_List*_tmp179;
struct _tuple0 _tmp177=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
nmtypes);_tmp178=_tmp177.f1;_tmp179=_tmp177.f2;{struct _tuple26 _tmp17B;struct Cyc_Dict_Dict
_tmp17C;struct Cyc_List_List*_tmp17D;struct _tuple26*_tmp17A=((struct _tuple26*(*)(
struct _tuple26*(*f)(struct _tuple27*,void*,struct _tuple26*),struct _tuple27*,
struct Cyc_List_List*x,struct _tuple26*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp175,_tmp179,_tmp176);_tmp17B=*_tmp17A;_tmp17C=_tmp17B.f1;_tmp17D=_tmp17B.f2;
dict=_tmp17C;{struct Cyc_List_List*_tmp17E=((struct Cyc_List_List*(*)(int n,int(*f)(
int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp17D),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmp17F=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp17E);
struct Cyc_List_List*_tmp180=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
_tmp178);struct Cyc_List_List*_tmp181=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp17D);struct Cyc_List_List*_tmp182=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y,struct Cyc_List_List*z))Cyc_List_zip3)(_tmp17F,_tmp180,
_tmp181);struct Cyc_List_List*_tmp183=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple19*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple3_exp_cls,loc,_tmp182);
void*tuple_type2=Cyc_Tcgenrep_tuple_typ(({void*_tmp18C[3];_tmp18C[2]=Cyc_Tcgenrep_datatype_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));_tmp18C[1]=Cyc_Absyn_const_string_typ((
void*)2);_tmp18C[0]=Cyc_Absyn_uint_typ;Cyc_List_list(_tag_dyneither(_tmp18C,
sizeof(void*),3));}));struct _tuple21 _tmp185;struct Cyc_List_List*_tmp186;struct
Cyc_Absyn_Exp*_tmp187;struct _tuple21*_tmp184=Cyc_Tcgenrep_array_decls(
tuple_type2,_tmp183,loc);_tmp185=*_tmp184;_tmp186=_tmp185.f1;_tmp187=_tmp185.f2;{
struct Cyc_Absyn_Exp*_tmp188=Cyc_Tcgenrep_cnst_string(*(*tname).f2,loc);struct Cyc_Absyn_Decl*
_tmp189=Cyc_Tcgenrep_datatype_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_datatype_str),varname,({struct Cyc_Absyn_Exp*
_tmp18B[3];_tmp18B[2]=_tmp187;_tmp18B[1]=_tmp174;_tmp18B[0]=_tmp188;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp18B,sizeof(struct Cyc_Absyn_Exp*),
3));}),sc,loc);return Cyc_Tcgenrep_update_info(dict,type,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp173,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp186,({struct Cyc_Absyn_Decl*_tmp18A[1];_tmp18A[0]=_tmp189;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp18A,sizeof(struct Cyc_Absyn_Decl*),
1));}))),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp17D,0);return dict;}}}}}static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepXDatatype(
struct _tuple6*xname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*
loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*nametypes){
struct Cyc_List_List*_tmp191;struct Cyc_List_List*_tmp192;struct _tuple0 _tmp190=((
struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(nametypes);_tmp191=
_tmp190.f1;_tmp192=_tmp190.f2;{struct _tuple27*_tmp193=({struct _tuple27*_tmp1A8=
_cycalloc(sizeof(*_tmp1A8));_tmp1A8->f1=te;_tmp1A8->f2=loc;_tmp1A8;});struct
_tuple26*_tmp194=({struct _tuple26*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7->f1=
dict;_tmp1A7->f2=0;_tmp1A7;});struct _tuple26 _tmp196;struct Cyc_Dict_Dict _tmp197;
struct Cyc_List_List*_tmp198;struct _tuple26*_tmp195=((struct _tuple26*(*)(struct
_tuple26*(*f)(struct _tuple27*,void*,struct _tuple26*),struct _tuple27*,struct Cyc_List_List*
x,struct _tuple26*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp193,_tmp192,_tmp194);_tmp196=*_tmp195;_tmp197=_tmp196.f1;_tmp198=_tmp196.f2;
dict=_tmp197;{struct Cyc_List_List*_tmp199=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _dyneither_ptr*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,loc,
_tmp191);struct Cyc_List_List*_tmp19A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp198);struct Cyc_List_List*_tmp19B=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp199,_tmp19A);struct Cyc_List_List*
_tmp19C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple18*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp19B);void*name_type=Cyc_Absyn_dyneither_typ(
Cyc_Absyn_char_typ,(void*)2,({struct Cyc_Absyn_Tqual _tmp1A6;_tmp1A6.print_const=1;
_tmp1A6.q_volatile=0;_tmp1A6.q_restrict=0;_tmp1A6.real_const=1;_tmp1A6.loc=0;
_tmp1A6;}),Cyc_Absyn_true_conref);void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*
_tmp1A5[2];_tmp1A5[1]=Cyc_Tcgenrep_datatype_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp1A5[0]=name_type;Cyc_List_list(_tag_dyneither(_tmp1A5,sizeof(void*),2));}));
struct _tuple21 _tmp19E;struct Cyc_List_List*_tmp19F;struct Cyc_Absyn_Exp*_tmp1A0;
struct _tuple21*_tmp19D=Cyc_Tcgenrep_array_decls(tuple_type,_tmp19C,loc);_tmp19E=*
_tmp19D;_tmp19F=_tmp19E.f1;_tmp1A0=_tmp19E.f2;{struct Cyc_Absyn_Exp*_tmp1A1=Cyc_Tcgenrep_cnst_string(*(*
xname).f2,loc);struct Cyc_Absyn_Decl*_tmp1A2=Cyc_Tcgenrep_datatype_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_xdatatype_str),varname,({struct Cyc_Absyn_Exp*_tmp1A4[2];_tmp1A4[1]=
_tmp1A0;_tmp1A4[0]=_tmp1A1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp1A4,sizeof(struct Cyc_Absyn_Exp*),2));}),sc,loc);return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp19F,({struct Cyc_Absyn_Decl*_tmp1A3[1];_tmp1A3[0]=
_tmp1A2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp1A3,sizeof(struct Cyc_Absyn_Decl*),1));})),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp198,0);
return dict;}}}}static struct _tuple25*Cyc_Tcgenrep_lookupRep(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*loc,void*type){struct Cyc_Tcgenrep_RepInfo**
info=((struct Cyc_Tcgenrep_RepInfo**(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup_opt)(
dict,type);if(info != 0){if((*info)->is_extern)dict=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict,void*))Cyc_Dict_delete)(dict,type);else{struct _tuple25*
_tmp1A9=({struct _tuple25*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA->f1=dict;
_tmp1AA->f2=*info;_tmp1AA;});return _tmp1A9;}}{void*_tmp1AB=Cyc_Tcutil_compress(
Cyc_Tcgenrep_monomorphize_type(type));void*_tmp1AC;void*_tmp1AD;int _tmp1AE;
struct Cyc_Absyn_PtrInfo _tmp1AF;struct Cyc_Absyn_ArrayInfo _tmp1B0;void*_tmp1B1;
struct Cyc_Absyn_Tqual _tmp1B2;struct Cyc_Absyn_Exp*_tmp1B3;union Cyc_Absyn_Constraint*
_tmp1B4;struct Cyc_List_List*_tmp1B5;struct _tuple6*_tmp1B6;struct Cyc_List_List*
_tmp1B7;struct Cyc_Absyn_Typedefdecl*_tmp1B8;void**_tmp1B9;struct Cyc_Absyn_Tvar*
_tmp1BA;struct Cyc_Absyn_FnInfo _tmp1BB;struct Cyc_Absyn_DatatypeInfo _tmp1BC;union
Cyc_Absyn_DatatypeInfoU _tmp1BD;struct Cyc_Absyn_Datatypedecl**_tmp1BE;struct Cyc_Absyn_Datatypedecl*
_tmp1BF;struct Cyc_List_List*_tmp1C0;struct Cyc_Core_Opt*_tmp1C1;struct Cyc_Absyn_DatatypeFieldInfo
_tmp1C2;struct Cyc_Absyn_AggrInfo _tmp1C3;union Cyc_Absyn_AggrInfoU _tmp1C4;struct
Cyc_List_List*_tmp1C5;void*_tmp1C6;struct Cyc_List_List*_tmp1C7;void*_tmp1C8;
struct Cyc_List_List*_tmp1C9;struct Cyc_Absyn_Enumdecl*_tmp1CA;struct Cyc_List_List*
_tmp1CB;_LL18: if(_tmp1AB <= (void*)4)goto _LL1A;if(*((int*)_tmp1AB)!= 5)goto _LL1A;
_tmp1AC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1AB)->f1;_tmp1AD=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp1AB)->f2;_LL19: {struct _tuple6*_tmp1CC=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(({const char*_tmp1D0="rep";_tag_dyneither(_tmp1D0,sizeof(
char),4);})));struct Cyc_Absyn_Decl*_tmp1CD=Cyc_Tcgenrep_datatype_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_int_str),_tmp1CC,({struct Cyc_Absyn_Exp*_tmp1CF[2];_tmp1CF[1]=Cyc_Tcgenrep_cnst_int(
Cyc_Tcgenrep_size_of2int(_tmp1AD),loc);_tmp1CF[0]=Cyc_Tcgenrep_cnst_int(_tmp1AC
== (void*)0?1: 0,loc);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp1CF,sizeof(struct Cyc_Absyn_Exp*),2));}),(void*)0,loc);dict=
Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*_tmp1CE[1];_tmp1CE[0]=
_tmp1CD;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp1CE,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp1CC,loc),loc),0,0);goto _LL17;}
_LL1A: if((int)_tmp1AB != 1)goto _LL1C;_LL1B: dict=Cyc_Tcgenrep_update_info(dict,
type,0,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_float_str),loc),0,0);goto _LL17;_LL1C: if(_tmp1AB <= (void*)4)goto
_LL26;if(*((int*)_tmp1AB)!= 6)goto _LL1E;_tmp1AE=((struct Cyc_Absyn_DoubleType_struct*)
_tmp1AB)->f1;_LL1D: dict=Cyc_Tcgenrep_update_info(dict,type,0,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_double_str),loc),
0,0);goto _LL17;_LL1E: if(*((int*)_tmp1AB)!= 4)goto _LL20;_tmp1AF=((struct Cyc_Absyn_PointerType_struct*)
_tmp1AB)->f1;_LL1F:{void*_tmp1D1=Cyc_Absyn_conref_val((_tmp1AF.ptr_atts).bounds);
struct Cyc_Absyn_Exp*_tmp1D2;_LL51: if(_tmp1D1 <= (void*)1)goto _LL53;if(*((int*)
_tmp1D1)!= 0)goto _LL53;_tmp1D2=((struct Cyc_Absyn_Upper_b_struct*)_tmp1D1)->f1;
_LL52: {struct _tuple6*_tmp1D3=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({
const char*_tmp1DE="rep";_tag_dyneither(_tmp1DE,sizeof(char),4);})));struct Cyc_Core_Opt*
_tmp1D4=({struct Cyc_Core_Opt*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD->v=Cyc_Tcgenrep_gen_vardecl(
_tmp1D3,Cyc_Tcgenrep_datatypefield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_thinptr_str)),0,(void*)3,loc);_tmp1DD;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1D3,loc),loc),_tmp1D4,0);{struct _tuple25 _tmp1D6;
struct Cyc_Dict_Dict _tmp1D7;struct Cyc_Tcgenrep_RepInfo*_tmp1D8;struct _tuple25*
_tmp1D5=Cyc_Tcgenrep_lookupRep(te,dict,loc,_tmp1AF.elt_typ);_tmp1D6=*_tmp1D5;
_tmp1D7=_tmp1D6.f1;_tmp1D8=_tmp1D6.f2;dict=_tmp1D7;{struct Cyc_Absyn_Decl*_tmp1D9=
Cyc_Tcgenrep_datatype_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_thinptr_str),_tmp1D3,({struct Cyc_Absyn_Exp*
_tmp1DC[2];_tmp1DC[1]=(struct Cyc_Absyn_Exp*)_check_null(_tmp1D8->exp);_tmp1DC[0]=
_tmp1D2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp1DC,sizeof(struct Cyc_Absyn_Exp*),2));}),(void*)0,loc);dict=
Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*_tmp1DA[1];_tmp1DA[0]=
_tmp1D9;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp1DA,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp1D3,loc),loc),({struct Cyc_Tcgenrep_RepInfo*
_tmp1DB[1];_tmp1DB[0]=_tmp1D8;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp1DB,sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),
0);goto _LL50;}}}_LL53: if((int)_tmp1D1 != 0)goto _LL50;_LL54: {struct _tuple6*
_tmp1DF=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({const char*_tmp1EA="rep";
_tag_dyneither(_tmp1EA,sizeof(char),4);})));struct Cyc_Core_Opt*_tmp1E0=({struct
Cyc_Core_Opt*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9->v=Cyc_Tcgenrep_gen_vardecl(
_tmp1DF,Cyc_Tcgenrep_datatypefield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_fatptr_str)),0,(void*)3,loc);_tmp1E9;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1DF,loc),loc),_tmp1E0,0);{struct _tuple25 _tmp1E2;
struct Cyc_Dict_Dict _tmp1E3;struct Cyc_Tcgenrep_RepInfo*_tmp1E4;struct _tuple25*
_tmp1E1=Cyc_Tcgenrep_lookupRep(te,dict,loc,_tmp1AF.elt_typ);_tmp1E2=*_tmp1E1;
_tmp1E3=_tmp1E2.f1;_tmp1E4=_tmp1E2.f2;dict=_tmp1E3;{struct Cyc_Absyn_Decl*_tmp1E5=
Cyc_Tcgenrep_datatype_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_fatptr_str),_tmp1DF,({struct Cyc_Absyn_Exp*
_tmp1E8[1];_tmp1E8[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp1E4->exp);((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1E8,
sizeof(struct Cyc_Absyn_Exp*),1));}),(void*)0,loc);dict=Cyc_Tcgenrep_update_info(
dict,type,({struct Cyc_Absyn_Decl*_tmp1E6[1];_tmp1E6[0]=_tmp1E5;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1E6,sizeof(struct Cyc_Absyn_Decl*),
1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1DF,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp1E7[1];_tmp1E7[0]=_tmp1E4;((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp1E7,sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LL50;}}}_LL50:;}goto
_LL17;_LL20: if(*((int*)_tmp1AB)!= 7)goto _LL22;_tmp1B0=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1AB)->f1;_tmp1B1=_tmp1B0.elt_type;_tmp1B2=_tmp1B0.tq;_tmp1B3=_tmp1B0.num_elts;
_tmp1B4=_tmp1B0.zero_term;_LL21: if(_tmp1B3 == 0)({void*_tmp1EB=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1EC="tcgenrep:At this point, array bounds must be constants";
_tag_dyneither(_tmp1EC,sizeof(char),55);}),_tag_dyneither(_tmp1EB,sizeof(void*),
0));});{struct _tuple25 _tmp1EE;struct Cyc_Dict_Dict _tmp1EF;struct Cyc_Tcgenrep_RepInfo*
_tmp1F0;struct _tuple25*_tmp1ED=Cyc_Tcgenrep_lookupRep(te,dict,loc,_tmp1B1);
_tmp1EE=*_tmp1ED;_tmp1EF=_tmp1EE.f1;_tmp1F0=_tmp1EE.f2;dict=_tmp1EF;{struct
_tuple6*_tmp1F1=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({const char*
_tmp1F6="rep";_tag_dyneither(_tmp1F6,sizeof(char),4);})));struct Cyc_Absyn_Decl*
_tmp1F2=Cyc_Tcgenrep_datatype_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_thinptr_str),_tmp1F1,({struct Cyc_Absyn_Exp*
_tmp1F5[2];_tmp1F5[1]=(struct Cyc_Absyn_Exp*)_check_null(_tmp1F0->exp);_tmp1F5[0]=(
struct Cyc_Absyn_Exp*)_tmp1B3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp1F5,sizeof(struct Cyc_Absyn_Exp*),2));}),(void*)0,loc);dict=
Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*_tmp1F3[1];_tmp1F3[0]=
_tmp1F2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp1F3,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp1F1,loc),loc),({struct Cyc_Tcgenrep_RepInfo*
_tmp1F4[1];_tmp1F4[0]=_tmp1F0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp1F4,sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),
0);goto _LL17;}}_LL22: if(*((int*)_tmp1AB)!= 9)goto _LL24;_tmp1B5=((struct Cyc_Absyn_TupleType_struct*)
_tmp1AB)->f1;_LL23: {struct _tuple6*_tmp1F7=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({
const char*_tmp1FB="rep";_tag_dyneither(_tmp1FB,sizeof(char),4);})));struct Cyc_Core_Opt*
_tmp1F8=({struct Cyc_Core_Opt*_tmp1FA=_cycalloc(sizeof(*_tmp1FA));_tmp1FA->v=Cyc_Tcgenrep_gen_vardecl(
_tmp1F7,Cyc_Tcgenrep_datatypefield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tuple_str)),0,(void*)3,loc);_tmp1FA;});
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1F7,loc),loc),_tmp1F8,0);{struct Cyc_List_List*
_tmp1F9=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple16*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcgenrep_get_second,_tmp1B5);dict=Cyc_Tcgenrep_buildRepTuple(
te,dict,loc,_tmp1F7,(void*)0,type,_tmp1F9);goto _LL17;}}_LL24: if(*((int*)_tmp1AB)
!= 16)goto _LL26;_tmp1B6=((struct Cyc_Absyn_TypedefType_struct*)_tmp1AB)->f1;
_tmp1B7=((struct Cyc_Absyn_TypedefType_struct*)_tmp1AB)->f2;_tmp1B8=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1AB)->f3;_tmp1B9=((struct Cyc_Absyn_TypedefType_struct*)_tmp1AB)->f4;_LL25:
if(_tmp1B9 == 0)({void*_tmp1FC=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1FD="gen(): can't gen for abstract typedef";
_tag_dyneither(_tmp1FD,sizeof(char),38);}),_tag_dyneither(_tmp1FC,sizeof(void*),
0));});{struct _tuple25 _tmp1FF;struct Cyc_Dict_Dict _tmp200;struct Cyc_Tcgenrep_RepInfo*
_tmp201;struct _tuple25*_tmp1FE=Cyc_Tcgenrep_lookupRep(te,dict,loc,*_tmp1B9);
_tmp1FF=*_tmp1FE;_tmp200=_tmp1FF.f1;_tmp201=_tmp1FF.f2;dict=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(
_tmp200,type,_tmp201);goto _LL17;}_LL26: if((int)_tmp1AB != 0)goto _LL28;_LL27:({
void*_tmp202=0;Cyc_Tcutil_terr(loc,({const char*_tmp203="found void in gen() expression";
_tag_dyneither(_tmp203,sizeof(char),31);}),_tag_dyneither(_tmp202,sizeof(void*),
0));});goto _LL17;_LL28: if(_tmp1AB <= (void*)4)goto _LL32;if(*((int*)_tmp1AB)!= 0)
goto _LL2A;_LL29:({void*_tmp204=0;Cyc_Tcutil_terr(loc,({const char*_tmp205="found evar in gen() expression";
_tag_dyneither(_tmp205,sizeof(char),31);}),_tag_dyneither(_tmp204,sizeof(void*),
0));});goto _LL17;_LL2A: if(*((int*)_tmp1AB)!= 1)goto _LL2C;_tmp1BA=((struct Cyc_Absyn_VarType_struct*)
_tmp1AB)->f1;_LL2B:({void*_tmp206=0;Cyc_Tcutil_terr(loc,({const char*_tmp207="found tyvar in gen() expression";
_tag_dyneither(_tmp207,sizeof(char),32);}),_tag_dyneither(_tmp206,sizeof(void*),
0));});goto _LL17;_LL2C: if(*((int*)_tmp1AB)!= 8)goto _LL2E;_tmp1BB=((struct Cyc_Absyn_FnType_struct*)
_tmp1AB)->f1;_LL2D:({void*_tmp208=0;Cyc_Tcutil_terr(loc,({const char*_tmp209="found function type in gen() expression";
_tag_dyneither(_tmp209,sizeof(char),40);}),_tag_dyneither(_tmp208,sizeof(void*),
0));});goto _LL17;_LL2E: if(*((int*)_tmp1AB)!= 14)goto _LL30;_LL2F: goto _LL31;_LL30:
if(*((int*)_tmp1AB)!= 15)goto _LL32;_LL31: goto _LL33;_LL32: if((int)_tmp1AB != 3)
goto _LL34;_LL33: goto _LL35;_LL34: if((int)_tmp1AB != 2)goto _LL36;_LL35: goto _LL37;
_LL36: if(_tmp1AB <= (void*)4)goto _LL38;if(*((int*)_tmp1AB)!= 19)goto _LL38;_LL37:
goto _LL39;_LL38: if(_tmp1AB <= (void*)4)goto _LL3A;if(*((int*)_tmp1AB)!= 20)goto
_LL3A;_LL39: goto _LL3B;_LL3A: if(_tmp1AB <= (void*)4)goto _LL3C;if(*((int*)_tmp1AB)
!= 21)goto _LL3C;_LL3B:({void*_tmp20A=0;Cyc_Tcutil_terr(loc,({const char*_tmp20B="gen(): unhandled region, handle or effect type";
_tag_dyneither(_tmp20B,sizeof(char),47);}),_tag_dyneither(_tmp20A,sizeof(void*),
0));});goto _LL17;_LL3C: if(_tmp1AB <= (void*)4)goto _LL3E;if(*((int*)_tmp1AB)!= 2)
goto _LL3E;_tmp1BC=((struct Cyc_Absyn_DatatypeType_struct*)_tmp1AB)->f1;_tmp1BD=
_tmp1BC.datatype_info;if((_tmp1BD.KnownDatatype).tag != 2)goto _LL3E;_tmp1BE=(
struct Cyc_Absyn_Datatypedecl**)(_tmp1BD.KnownDatatype).val;_tmp1BF=*_tmp1BE;
_tmp1C0=_tmp1BC.targs;_tmp1C1=_tmp1BC.rgn;_LL3D: if(_tmp1BF->tvs != 0)({void*
_tmp20C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp20D="gen(): datatype type variables not handled yet";
_tag_dyneither(_tmp20D,sizeof(char),47);}),_tag_dyneither(_tmp20C,sizeof(void*),
0));});if(_tmp1BF->fields == 0){struct _tuple6*_tmp20E=Cyc_Tcgenrep_toplevel_name(({
struct _dyneither_ptr*_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp211;}));struct Cyc_Core_Opt*_tmp20F=({struct Cyc_Core_Opt*_tmp210=
_cycalloc(sizeof(*_tmp210));_tmp210->v=Cyc_Tcgenrep_gen_vardecl(_tmp20E,Cyc_Tcgenrep_datatypefield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_datatype_str)),0,(void*)3,loc);_tmp210;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp20E,loc),loc),_tmp20F,1);goto _LL17;}if(!_tmp1BF->is_extensible){struct
_tuple6*_tmp212=Cyc_Tcgenrep_toplevel_name(({struct _dyneither_ptr*_tmp21A=
_cycalloc(sizeof(*_tmp21A));_tmp21A[0]=Cyc_Tcgenrep_make_type_cstring(type);
_tmp21A;}));struct Cyc_Core_Opt*_tmp213=({struct Cyc_Core_Opt*_tmp219=_cycalloc(
sizeof(*_tmp219));_tmp219->v=Cyc_Tcgenrep_gen_vardecl(_tmp212,Cyc_Tcgenrep_datatypefield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_datatype_str)),0,(void*)3,loc);_tmp219;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp212,loc),loc),_tmp213,0);{struct Cyc_List_List*_tmp214=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1BF->fields))->v;struct Cyc_List_List*_tmp215=((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*
x))Cyc_List_filter)(Cyc_Tcgenrep_filter_empty_datatypefield,_tmp214);struct Cyc_List_List*
_tmp216=((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Datatypefield*),struct
Cyc_List_List*x))Cyc_List_filter)(Cyc_Tcgenrep_filter_nonempty_datatypefield,
_tmp214);struct Cyc_List_List*_tmp217=((struct Cyc_List_List*(*)(struct _tuple24*(*
f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_datatypefield_and_get_nmtype,
_tmp215);struct Cyc_List_List*_tmp218=((struct Cyc_List_List*(*)(struct _tuple6*(*f)(
struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_datatypefield_and_get_name,
_tmp216);dict=Cyc_Tcgenrep_buildRepDatatype(_tmp1BF->name,te,dict,loc,_tmp212,(
void*)2,type,_tmp218,_tmp217);}}else{struct _tuple6*_tmp21B=Cyc_Tcgenrep_toplevel_name(({
struct _dyneither_ptr*_tmp221=_cycalloc(sizeof(*_tmp221));_tmp221[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp221;}));struct Cyc_Core_Opt*_tmp21C=({struct Cyc_Core_Opt*_tmp220=
_cycalloc(sizeof(*_tmp220));_tmp220->v=Cyc_Tcgenrep_gen_vardecl(_tmp21B,Cyc_Tcgenrep_datatypefield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_xdatatype_str)),0,(void*)3,loc);_tmp220;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp21B,loc),loc),_tmp21C,0);{struct Cyc_List_List*_tmp21D=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1BF->fields))->v;struct Cyc_List_List*_tmp21E=((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*
x))Cyc_List_filter)(Cyc_Tcgenrep_filter_empty_datatypefield,_tmp21D);struct Cyc_List_List*
_tmp21F=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(struct Cyc_Absyn_Datatypefield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_xdatatypefield_and_get_name_type,
_tmp21E);dict=Cyc_Tcgenrep_buildRepXDatatype(_tmp1BF->name,te,dict,loc,_tmp21B,(
void*)2,type,_tmp21F);}}goto _LL17;_LL3E: if(_tmp1AB <= (void*)4)goto _LL40;if(*((
int*)_tmp1AB)!= 2)goto _LL40;_LL3F:({void*_tmp222=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp223="gen(): datatype must be resolved by now";
_tag_dyneither(_tmp223,sizeof(char),40);}),_tag_dyneither(_tmp222,sizeof(void*),
0));});_LL40: if(_tmp1AB <= (void*)4)goto _LL42;if(*((int*)_tmp1AB)!= 3)goto _LL42;
_tmp1C2=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp1AB)->f1;_LL41: if(
_tmp1C2.targs != 0)({void*_tmp224=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp225="gen(): datatypefield type parameters not handled yet";
_tag_dyneither(_tmp225,sizeof(char),53);}),_tag_dyneither(_tmp224,sizeof(void*),
0));});{union Cyc_Absyn_DatatypeFieldInfoU _tmp226=_tmp1C2.field_info;struct
_tuple7 _tmp227;struct Cyc_Absyn_Datatypedecl*_tmp228;struct Cyc_Absyn_Datatypefield*
_tmp229;_LL56: if((_tmp226.KnownDatatypefield).tag != 2)goto _LL58;_tmp227=(struct
_tuple7)(_tmp226.KnownDatatypefield).val;_tmp228=_tmp227.f1;_tmp229=_tmp227.f2;
_LL57: {struct Cyc_List_List*_tmp22A=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_second,_tmp229->typs);
struct Cyc_List_List*_tmp22B=({struct Cyc_List_List*_tmp22E=_cycalloc(sizeof(*
_tmp22E));_tmp22E->hd=(void*)Cyc_Absyn_uint_typ;_tmp22E->tl=_tmp22A;_tmp22E;});
struct _tuple6*_tmp22C=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({const
char*_tmp22D="rep";_tag_dyneither(_tmp22D,sizeof(char),4);})));dict=Cyc_Tcgenrep_buildRepDatatypefield(
_tmp228->name,_tmp229->name,te,dict,loc,_tmp22C,(void*)0,type,_tmp22B);goto _LL55;}
_LL58:;_LL59:({void*_tmp22F=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp230="gen(): datatypefield must be known at this point";
_tag_dyneither(_tmp230,sizeof(char),49);}),_tag_dyneither(_tmp22F,sizeof(void*),
0));});_LL55:;}goto _LL17;_LL42: if(_tmp1AB <= (void*)4)goto _LL44;if(*((int*)
_tmp1AB)!= 10)goto _LL44;_tmp1C3=((struct Cyc_Absyn_AggrType_struct*)_tmp1AB)->f1;
_tmp1C4=_tmp1C3.aggr_info;_tmp1C5=_tmp1C3.targs;_LL43: {struct Cyc_Absyn_Aggrdecl*
_tmp231=Cyc_Absyn_get_known_aggrdecl(_tmp1C4);if(_tmp231->impl != 0  && ((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp231->impl))->exist_vars != 0)({void*
_tmp232=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp233="gen(): existential type variables not yet supported";
_tag_dyneither(_tmp233,sizeof(char),52);}),_tag_dyneither(_tmp232,sizeof(void*),
0));});if(_tmp231->kind == (void*)0){struct Cyc_Absyn_Aggrdecl*_tmp234=_tmp231;if(
_tmp234->impl == 0){struct _tuple6*_tmp235=Cyc_Tcgenrep_toplevel_name(({struct
_dyneither_ptr*_tmp238=_cycalloc(sizeof(*_tmp238));_tmp238[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp238;}));struct Cyc_Core_Opt*_tmp236=({struct Cyc_Core_Opt*_tmp237=
_cycalloc(sizeof(*_tmp237));_tmp237->v=Cyc_Tcgenrep_gen_vardecl(_tmp235,Cyc_Tcgenrep_datatypefield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_struct_str)),0,(void*)3,loc);_tmp237;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp235,loc),loc),_tmp236,1);goto _LL17;}if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp234->impl))->fields != 0  && ((int(*)(int(*pred)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp234->impl))->fields)){int _tmp239=((int(*)(int(*f)(int,struct Cyc_Absyn_Aggrfield*),
int accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,
0,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp234->impl))->fields);int
_tmp23A=_tmp239 / 8 + (_tmp239 % 8 == 0?0: 1);struct Cyc_List_List*chars=0;{int i=0;
for(0;i < _tmp23A;++ i){chars=({struct Cyc_List_List*_tmp23B=_cycalloc(sizeof(*
_tmp23B));_tmp23B->hd=({struct _tuple16*_tmp23C=_cycalloc(sizeof(*_tmp23C));
_tmp23C->f1=Cyc_Tcgenrep_tq_none;_tmp23C->f2=Cyc_Absyn_char_typ;_tmp23C;});
_tmp23B->tl=chars;_tmp23B;});}}{void*base_type=(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp241=_cycalloc(sizeof(*_tmp241));_tmp241[0]=({struct Cyc_Absyn_TupleType_struct
_tmp242;_tmp242.tag=9;_tmp242.f1=chars;_tmp242;});_tmp241;});struct _tuple25
_tmp23E;struct Cyc_Dict_Dict _tmp23F;struct Cyc_Tcgenrep_RepInfo*_tmp240;struct
_tuple25*_tmp23D=Cyc_Tcgenrep_lookupRep(te,dict,loc,base_type);_tmp23E=*_tmp23D;
_tmp23F=_tmp23E.f1;_tmp240=_tmp23E.f2;dict=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(_tmp23F,type,_tmp240);}}
else{struct _tuple6*_tmp243=Cyc_Tcgenrep_toplevel_name(({struct _dyneither_ptr*
_tmp248=_cycalloc(sizeof(*_tmp248));_tmp248[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp248;}));struct Cyc_Core_Opt*_tmp244=({struct Cyc_Core_Opt*_tmp247=
_cycalloc(sizeof(*_tmp247));_tmp247->v=Cyc_Tcgenrep_gen_vardecl(_tmp243,Cyc_Tcgenrep_datatypefield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_struct_str)),0,(void*)3,loc);_tmp247;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp243,loc),loc),_tmp244,0);{struct Cyc_List_List*_tmp245=((struct Cyc_List_List*(*)(
struct _tuple22*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp234->impl))->fields);dict=Cyc_Tcgenrep_buildRepStruct(({struct
Cyc_Core_Opt*_tmp246=_cycalloc(sizeof(*_tmp246));_tmp246->v=_tmp234->name;
_tmp246;}),te,dict,loc,_tmp243,(void*)2,type,_tmp245);}}}else{struct Cyc_Absyn_Aggrdecl*
_tmp249=_tmp231;if(_tmp249->tvs != 0)({void*_tmp24A=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp24B="gen: unions with parameters not yet supported";
_tag_dyneither(_tmp24B,sizeof(char),46);}),_tag_dyneither(_tmp24A,sizeof(void*),
0));});if(_tmp249->impl == 0){struct _tuple6*_tmp24C=Cyc_Tcgenrep_toplevel_name(({
struct _dyneither_ptr*_tmp24F=_cycalloc(sizeof(*_tmp24F));_tmp24F[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp24F;}));struct Cyc_Core_Opt*_tmp24D=({struct Cyc_Core_Opt*_tmp24E=
_cycalloc(sizeof(*_tmp24E));_tmp24E->v=Cyc_Tcgenrep_gen_vardecl(_tmp24C,Cyc_Tcgenrep_datatypefield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_union_str)),0,(void*)3,loc);_tmp24E;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp24C,loc),loc),_tmp24D,1);goto _LL17;}{struct Cyc_List_List*_tmp250=((struct Cyc_List_List*(*)(
struct _tuple22*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp249->impl))->fields);struct _tuple6*_tmp251=Cyc_Tcgenrep_toplevel_name(({
struct _dyneither_ptr*_tmp253=_cycalloc(sizeof(*_tmp253));_tmp253[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp253;}));dict=Cyc_Tcgenrep_buildRepUnion(({struct Cyc_Core_Opt*_tmp252=
_cycalloc(sizeof(*_tmp252));_tmp252->v=_tmp249->name;_tmp252;}),te,dict,loc,
_tmp251,(void*)2,type,_tmp250);}}goto _LL17;}_LL44: if(_tmp1AB <= (void*)4)goto
_LL46;if(*((int*)_tmp1AB)!= 11)goto _LL46;_tmp1C6=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1AB)->f1;if((int)_tmp1C6 != 0)goto _LL46;_tmp1C7=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1AB)->f2;_LL45: if(_tmp1C7 != 0  && ((int(*)(int(*pred)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,_tmp1C7)){int
_tmp254=((int(*)(int(*f)(int,struct Cyc_Absyn_Aggrfield*),int accum,struct Cyc_List_List*
x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,0,_tmp1C7);int _tmp255=
_tmp254 / 8 + (_tmp254 % 8 == 0?0: 1);void*base_type=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,
Cyc_Tcgenrep_tq_none,(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_int(_tmp255,loc),
Cyc_Absyn_true_conref,0);struct _tuple25 _tmp257;struct Cyc_Dict_Dict _tmp258;struct
Cyc_Tcgenrep_RepInfo*_tmp259;struct _tuple25*_tmp256=Cyc_Tcgenrep_lookupRep(te,
dict,loc,base_type);_tmp257=*_tmp256;_tmp258=_tmp257.f1;_tmp259=_tmp257.f2;dict=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Tcgenrep_RepInfo*v))
Cyc_Dict_insert)(_tmp258,type,_tmp259);}else{struct Cyc_List_List*_tmp25A=((
struct Cyc_List_List*(*)(struct _tuple22*(*f)(struct Cyc_Absyn_Aggrfield*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_nmtype,_tmp1C7);
struct _tuple6*_tmp25B=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({const
char*_tmp25C="rep";_tag_dyneither(_tmp25C,sizeof(char),4);})));dict=Cyc_Tcgenrep_buildRepStruct(
0,te,dict,loc,_tmp25B,(void*)0,type,_tmp25A);}goto _LL17;_LL46: if(_tmp1AB <= (void*)
4)goto _LL48;if(*((int*)_tmp1AB)!= 11)goto _LL48;_tmp1C8=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1AB)->f1;if((int)_tmp1C8 != 1)goto _LL48;_tmp1C9=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1AB)->f2;_LL47: {struct Cyc_List_List*_tmp25D=((struct Cyc_List_List*(*)(
struct _tuple22*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,_tmp1C9);struct _tuple6*_tmp25E=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(({const char*_tmp25F="rep";_tag_dyneither(_tmp25F,sizeof(
char),4);})));dict=Cyc_Tcgenrep_buildRepUnion(0,te,dict,loc,_tmp25E,(void*)0,
type,_tmp25D);goto _LL17;}_LL48: if(_tmp1AB <= (void*)4)goto _LL4A;if(*((int*)
_tmp1AB)!= 12)goto _LL4A;_tmp1CA=((struct Cyc_Absyn_EnumType_struct*)_tmp1AB)->f2;
_LL49: if(_tmp1CA == 0)({void*_tmp260=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp261="gen(): enum declaration must be present by now";
_tag_dyneither(_tmp261,sizeof(char),47);}),_tag_dyneither(_tmp260,sizeof(void*),
0));});{struct Cyc_Absyn_Enumdecl _tmp262=*_tmp1CA;if(_tmp262.fields == 0){struct
_tuple6*_tmp263=Cyc_Tcgenrep_toplevel_name(({struct _dyneither_ptr*_tmp266=
_cycalloc(sizeof(*_tmp266));_tmp266[0]=Cyc_Tcgenrep_make_type_cstring(type);
_tmp266;}));struct Cyc_Core_Opt*_tmp264=({struct Cyc_Core_Opt*_tmp265=_cycalloc(
sizeof(*_tmp265));_tmp265->v=Cyc_Tcgenrep_gen_vardecl(_tmp263,Cyc_Tcgenrep_datatypefield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_enum_str)),0,(void*)3,loc);_tmp265;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp263,loc),loc),_tmp264,1);goto _LL17;}{struct Cyc_List_List*_tmp267=((struct Cyc_List_List*(*)(
struct _tuple23*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_enumfield_tagnm,(struct Cyc_List_List*)(_tmp262.fields)->v);
struct _tuple6*_tmp268=Cyc_Tcgenrep_toplevel_name(({struct _dyneither_ptr*_tmp269=
_cycalloc(sizeof(*_tmp269));_tmp269[0]=Cyc_Tcgenrep_make_type_cstring(type);
_tmp269;}));dict=Cyc_Tcgenrep_buildRepEnum((struct _tuple6**)& _tmp262.name,te,
dict,loc,_tmp268,(void*)2,type,_tmp267);goto _LL17;}}_LL4A: if(_tmp1AB <= (void*)4)
goto _LL4C;if(*((int*)_tmp1AB)!= 13)goto _LL4C;_tmp1CB=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp1AB)->f1;_LL4B: {struct Cyc_List_List*_tmp26A=((struct Cyc_List_List*(*)(
struct _tuple23*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_enumfield_tagnm,_tmp1CB);struct _tuple6*_tmp26B=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(({const char*_tmp26C="rep";_tag_dyneither(_tmp26C,sizeof(
char),4);})));dict=Cyc_Tcgenrep_buildRepEnum(0,te,dict,loc,_tmp26B,(void*)0,type,
_tmp26A);goto _LL17;}_LL4C: if(_tmp1AB <= (void*)4)goto _LL4E;if(*((int*)_tmp1AB)!= 
18)goto _LL4E;_LL4D:({void*_tmp26D=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp26E="gen() for tag_t<-> not yet supported";
_tag_dyneither(_tmp26E,sizeof(char),37);}),_tag_dyneither(_tmp26D,sizeof(void*),
0));});_LL4E: if(_tmp1AB <= (void*)4)goto _LL17;if(*((int*)_tmp1AB)!= 17)goto _LL17;
_LL4F:({void*_tmp26F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp270="gen() for valueof_t<-> not yet supported";
_tag_dyneither(_tmp270,sizeof(char),41);}),_tag_dyneither(_tmp26F,sizeof(void*),
0));});_LL17:;}return({struct _tuple25*_tmp271=_cycalloc(sizeof(*_tmp271));
_tmp271->f1=dict;_tmp271->f2=((struct Cyc_Tcgenrep_RepInfo*(*)(struct Cyc_Dict_Dict
d,void*k))Cyc_Dict_lookup)(dict,type);_tmp271;});}static int Cyc_Tcgenrep_not_emitted_filter(
struct Cyc_Tcgenrep_RepInfo*ri){return ri->emitted == 0;}static void Cyc_Tcgenrep_mark_emitted(
struct Cyc_Tcgenrep_RepInfo*ri){ri->emitted=1;}struct _tuple28{struct Cyc_Dict_Dict
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple28 Cyc_Tcgenrep_tcGenrep(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
void*type,struct Cyc_Dict_Dict dict){Cyc_Tcutil_check_valid_toplevel_type(loc,te,
type);Cyc_Tcutil_check_valid_toplevel_type(loc,te,type);{struct _tuple25 _tmp273;
struct Cyc_Dict_Dict _tmp274;struct Cyc_Tcgenrep_RepInfo*_tmp275;struct _tuple25*
_tmp272=Cyc_Tcgenrep_lookupRep(te,dict,loc,type);_tmp273=*_tmp272;_tmp274=
_tmp273.f1;_tmp275=_tmp273.f2;{struct Cyc_List_List*_tmp276=Cyc_Tcgenrep_dfs(
_tmp275);return({struct _tuple28 _tmp277;_tmp277.f1=_tmp274;_tmp277.f2=_tmp276;
_tmp277.f3=(struct Cyc_Absyn_Exp*)_check_null(_tmp275->exp);_tmp277;});}}}
