#ifndef _SETJMP_H_
#define _SETJMP_H_
#ifndef _jmp_buf_def_
#define _jmp_buf_def_
typedef int jmp_buf[192];
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
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct Cyc_Typerep_Int_struct{int tag;int f1;unsigned int f2;};struct Cyc_Typerep_Float_struct{
int tag;};struct Cyc_Typerep_Double_struct{int tag;};struct Cyc_Typerep_ThinPtr_struct{
int tag;unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;}
;struct _tuple0{unsigned int f1;struct _dyneither_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{
int tag;struct _dyneither_ptr*f1;unsigned int f2;struct _dyneither_ptr f3;};struct
_tuple1{unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;
unsigned int f1;struct _dyneither_ptr f2;};struct _tuple2{unsigned int f1;struct
_dyneither_ptr f2;};struct Cyc_Typerep_TUnion_struct{int tag;struct _dyneither_ptr f1;
struct _dyneither_ptr f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;unsigned int f3;struct
_dyneither_ptr f4;};struct _tuple3{struct _dyneither_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};void Cyc_Typerep_print_typestruct(
void*rep);void Cyc_Typerep_xmlize_typestruct(void*rep);struct _tuple4{int f1;void*
f2;};struct _tuple4 Cyc_Typerep_get_unionbranch(unsigned int tag,struct
_dyneither_ptr l);struct _dyneither_ptr Cyc_Typerep_get_tagname(unsigned int tag,
struct _dyneither_ptr l);struct _dyneither_ptr Cyc_Typerep_get_tagname2(unsigned int
tag,struct _dyneither_ptr l);struct _tuple4 Cyc_Typerep_get_xtunionbranch(struct
_dyneither_ptr tag,struct _dyneither_ptr l);unsigned int Cyc_Typerep_size_type(void*
rep);struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};int Cyc_printf(struct _dyneither_ptr,struct
_dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;
};extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct _tuple5{void*f1;void*f2;};struct _tuple5*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple5*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);int Cyc_strcmp(struct _dyneither_ptr s1,struct
_dyneither_ptr s2);struct _tuple4 Cyc_Typerep_tuple_tl(void*typ);static void _tmp192(
int*pos1,struct _dyneither_ptr*_tmp3,unsigned int*_tmp191,unsigned int*_tmp190,
struct _tuple1***_tmp18E){for(*_tmp191=0;*_tmp191 < *_tmp190;(*_tmp191)++){struct
_tuple1*_tmp18C;(*_tmp18E)[*_tmp191]=((_tmp18C=_cycalloc(sizeof(*_tmp18C)),((
_tmp18C->f1=(*(*((struct _tuple1**)_check_dyneither_subscript(*_tmp3,sizeof(
struct _tuple1*),(int)(*_tmp191 + 1))))).f1 - *pos1,((_tmp18C->f2=(*(*((struct
_tuple1**)_check_dyneither_subscript(*_tmp3,sizeof(struct _tuple1*),(int)(*
_tmp191 + 1))))).f2,_tmp18C))))));}}struct _tuple4 Cyc_Typerep_tuple_tl(void*typ){
void*_tmp0=typ;unsigned int _tmp2;struct _dyneither_ptr _tmp3;_LL1: {struct Cyc_Typerep_Tuple_struct*
_tmp1=(struct Cyc_Typerep_Tuple_struct*)_tmp0;if(_tmp1->tag != 6)goto _LL3;else{
_tmp2=_tmp1->f1;_tmp3=_tmp1->f2;}}_LL2: if(_get_dyneither_size(_tmp3,sizeof(
struct _tuple1*))< 2)goto _LL4;{int pos1=(int)(*(*((struct _tuple1**)
_check_dyneither_subscript(_tmp3,sizeof(struct _tuple1*),1)))).f1;unsigned int
_tmp191;unsigned int _tmp190;struct _dyneither_ptr _tmp18F;struct _tuple1**_tmp18E;
unsigned int _tmp18D;struct _dyneither_ptr _tmp4=(_tmp18D=_get_dyneither_size(_tmp3,
sizeof(struct _tuple1*))- 1,((_tmp18E=(struct _tuple1**)_cycalloc(_check_times(
sizeof(struct _tuple1*),_tmp18D)),((_tmp18F=_tag_dyneither(_tmp18E,sizeof(struct
_tuple1*),_tmp18D),((((_tmp190=_tmp18D,_tmp192(& pos1,& _tmp3,& _tmp191,& _tmp190,&
_tmp18E))),_tmp18F)))))));struct Cyc_Typerep_Tuple_struct*_tmp198;struct Cyc_Typerep_Tuple_struct
_tmp197;struct _tuple4 _tmp196;return(_tmp196.f1=pos1,((_tmp196.f2=(void*)((
_tmp198=_cycalloc(sizeof(*_tmp198)),((_tmp198[0]=((_tmp197.tag=6,((_tmp197.f1=
_tmp2 - pos1,((_tmp197.f2=_tmp4,_tmp197)))))),_tmp198)))),_tmp196)));}_LL3:;_LL4: {
struct Cyc_Core_Failure_struct _tmp19E;const char*_tmp19D;struct Cyc_Core_Failure_struct*
_tmp19C;(int)_throw((void*)((_tmp19C=_cycalloc(sizeof(*_tmp19C)),((_tmp19C[0]=((
_tmp19E.tag=Cyc_Core_Failure,((_tmp19E.f1=((_tmp19D="tuple_tl: expected tuple of size >= 2",
_tag_dyneither(_tmp19D,sizeof(char),38))),_tmp19E)))),_tmp19C)))));}_LL0:;}
struct _tuple4 Cyc_Typerep_get_unionbranch(unsigned int tag,struct _dyneither_ptr l);
struct _tuple4 Cyc_Typerep_get_unionbranch(unsigned int tag,struct _dyneither_ptr l){{
int i=0;for(0;i < _get_dyneither_size(l,sizeof(struct _tuple0*));++ i){unsigned int
_tmp11;void*_tmp12;struct _tuple0 _tmp10=*((struct _tuple0**)l.curr)[i];_tmp11=
_tmp10.f1;_tmp12=_tmp10.f3;if(_tmp11 == tag)return Cyc_Typerep_tuple_tl(_tmp12);}}{
struct Cyc_Core_Failure_struct _tmp1A4;const char*_tmp1A3;struct Cyc_Core_Failure_struct*
_tmp1A2;(int)_throw((void*)((_tmp1A2=_cycalloc(sizeof(*_tmp1A2)),((_tmp1A2[0]=((
_tmp1A4.tag=Cyc_Core_Failure,((_tmp1A4.f1=((_tmp1A3="Could not find tag in TUnion",
_tag_dyneither(_tmp1A3,sizeof(char),29))),_tmp1A4)))),_tmp1A2)))));}}struct
_dyneither_ptr Cyc_Typerep_get_tagname(unsigned int tag,struct _dyneither_ptr l);
struct _dyneither_ptr Cyc_Typerep_get_tagname(unsigned int tag,struct _dyneither_ptr
l){{int i=0;for(0;i < _get_dyneither_size(l,sizeof(struct _tuple2*));++ i){
unsigned int _tmp17;struct _dyneither_ptr _tmp18;struct _tuple2 _tmp16=*((struct
_tuple2**)l.curr)[i];_tmp17=_tmp16.f1;_tmp18=_tmp16.f2;if(_tmp17 == tag)return
_tmp18;}}{struct Cyc_Core_Failure_struct _tmp1AA;const char*_tmp1A9;struct Cyc_Core_Failure_struct*
_tmp1A8;(int)_throw((void*)((_tmp1A8=_cycalloc(sizeof(*_tmp1A8)),((_tmp1A8[0]=((
_tmp1AA.tag=Cyc_Core_Failure,((_tmp1AA.f1=((_tmp1A9="Could not find name in TUnion",
_tag_dyneither(_tmp1A9,sizeof(char),30))),_tmp1AA)))),_tmp1A8)))));}}struct
_dyneither_ptr Cyc_Typerep_get_tagname2(unsigned int tag,struct _dyneither_ptr l);
struct _dyneither_ptr Cyc_Typerep_get_tagname2(unsigned int tag,struct
_dyneither_ptr l){{int i=0;for(0;i < _get_dyneither_size(l,sizeof(struct _tuple0*));
++ i){unsigned int _tmp1D;struct _dyneither_ptr _tmp1E;struct _tuple0 _tmp1C=*((struct
_tuple0**)l.curr)[i];_tmp1D=_tmp1C.f1;_tmp1E=_tmp1C.f2;if(_tmp1D == tag)return
_tmp1E;}}{struct Cyc_Core_Failure_struct _tmp1B0;const char*_tmp1AF;struct Cyc_Core_Failure_struct*
_tmp1AE;(int)_throw((void*)((_tmp1AE=_cycalloc(sizeof(*_tmp1AE)),((_tmp1AE[0]=((
_tmp1B0.tag=Cyc_Core_Failure,((_tmp1B0.f1=((_tmp1AF="Could not find name in TUnion",
_tag_dyneither(_tmp1AF,sizeof(char),30))),_tmp1B0)))),_tmp1AE)))));}}struct
_tuple4 Cyc_Typerep_get_xtunionbranch(struct _dyneither_ptr tag,struct
_dyneither_ptr l);struct _tuple4 Cyc_Typerep_get_xtunionbranch(struct _dyneither_ptr
tag,struct _dyneither_ptr l){{int i=0;for(0;i < _get_dyneither_size(l,sizeof(struct
_tuple3*));++ i){struct _dyneither_ptr _tmp23;void*_tmp24;struct _tuple3 _tmp22=*((
struct _tuple3**)l.curr)[i];_tmp23=_tmp22.f1;_tmp24=_tmp22.f2;if(Cyc_strcmp((
struct _dyneither_ptr)_tmp23,(struct _dyneither_ptr)tag)== 0)return Cyc_Typerep_tuple_tl(
_tmp24);}}{struct Cyc_Core_Failure_struct _tmp1B6;const char*_tmp1B5;struct Cyc_Core_Failure_struct*
_tmp1B4;(int)_throw((void*)((_tmp1B4=_cycalloc(sizeof(*_tmp1B4)),((_tmp1B4[0]=((
_tmp1B6.tag=Cyc_Core_Failure,((_tmp1B6.f1=((_tmp1B5="Could not find tag in XTUnion",
_tag_dyneither(_tmp1B5,sizeof(char),30))),_tmp1B6)))),_tmp1B4)))));}}
unsigned int Cyc_Typerep_size_type(void*rep);unsigned int Cyc_Typerep_size_type(
void*rep){void*_tmp28=rep;int _tmp2A;unsigned int _tmp2B;unsigned int _tmp31;
unsigned int _tmp33;unsigned int _tmp35;int _tmp39;int _tmp3B;struct _dyneither_ptr
_tmp3C;_LL6: {struct Cyc_Typerep_Int_struct*_tmp29=(struct Cyc_Typerep_Int_struct*)
_tmp28;if(_tmp29->tag != 0)goto _LL8;else{_tmp2A=_tmp29->f1;_tmp2B=_tmp29->f2;}}
_LL7: return _tmp2B >> 3;_LL8: {struct Cyc_Typerep_Float_struct*_tmp2C=(struct Cyc_Typerep_Float_struct*)
_tmp28;if(_tmp2C->tag != 1)goto _LLA;}_LL9: return sizeof(float);_LLA: {struct Cyc_Typerep_Double_struct*
_tmp2D=(struct Cyc_Typerep_Double_struct*)_tmp28;if(_tmp2D->tag != 2)goto _LLC;}
_LLB: return sizeof(double);_LLC: {struct Cyc_Typerep_ThinPtr_struct*_tmp2E=(struct
Cyc_Typerep_ThinPtr_struct*)_tmp28;if(_tmp2E->tag != 3)goto _LLE;}_LLD: return
sizeof(void*);_LLE: {struct Cyc_Typerep_FatPtr_struct*_tmp2F=(struct Cyc_Typerep_FatPtr_struct*)
_tmp28;if(_tmp2F->tag != 4)goto _LL10;}_LLF: return sizeof(struct _dyneither_ptr);
_LL10: {struct Cyc_Typerep_Struct_struct*_tmp30=(struct Cyc_Typerep_Struct_struct*)
_tmp28;if(_tmp30->tag != 5)goto _LL12;else{_tmp31=_tmp30->f2;}}_LL11: return _tmp31;
_LL12: {struct Cyc_Typerep_Tuple_struct*_tmp32=(struct Cyc_Typerep_Tuple_struct*)
_tmp28;if(_tmp32->tag != 6)goto _LL14;else{_tmp33=_tmp32->f1;}}_LL13: return _tmp33;
_LL14: {struct Cyc_Typerep_TUnionField_struct*_tmp34=(struct Cyc_Typerep_TUnionField_struct*)
_tmp28;if(_tmp34->tag != 8)goto _LL16;else{_tmp35=_tmp34->f3;}}_LL15: return _tmp35;
_LL16: {struct Cyc_Typerep_TUnion_struct*_tmp36=(struct Cyc_Typerep_TUnion_struct*)
_tmp28;if(_tmp36->tag != 7)goto _LL18;}_LL17: return sizeof(void*);_LL18: {struct Cyc_Typerep_XTUnion_struct*
_tmp37=(struct Cyc_Typerep_XTUnion_struct*)_tmp28;if(_tmp37->tag != 9)goto _LL1A;}
_LL19: return sizeof(void*);_LL1A: {struct Cyc_Typerep_Union_struct*_tmp38=(struct
Cyc_Typerep_Union_struct*)_tmp28;if(_tmp38->tag != 10)goto _LL1C;else{_tmp39=
_tmp38->f2;}}_LL1B: return(unsigned int)_tmp39;_LL1C: {struct Cyc_Typerep_Enum_struct*
_tmp3A=(struct Cyc_Typerep_Enum_struct*)_tmp28;if(_tmp3A->tag != 11)goto _LL5;else{
_tmp3B=_tmp3A->f2;_tmp3C=_tmp3A->f3;}}_LL1D: return(unsigned int)_tmp3B;_LL5:;}
struct _dyneither_ptr Cyc_Typerep_anon_or_name(struct _dyneither_ptr*name);struct
_dyneither_ptr Cyc_Typerep_anon_or_name(struct _dyneither_ptr*name){const char*
_tmp1B7;return name == 0?(_tmp1B7="(anon)",_tag_dyneither(_tmp1B7,sizeof(char),7)):*
name;}void Cyc_Typerep_print_typestruct(void*rep);void Cyc_Typerep_print_typestruct(
void*rep){void*_tmp3E=rep;int _tmp40;unsigned int _tmp41;unsigned int _tmp45;void*
_tmp46;void*_tmp48;struct _dyneither_ptr*_tmp4A;unsigned int _tmp4B;struct
_dyneither_ptr _tmp4C;unsigned int _tmp4E;struct _dyneither_ptr _tmp4F;struct
_dyneither_ptr _tmp51;struct _dyneither_ptr _tmp52;unsigned int _tmp53;struct
_dyneither_ptr _tmp54;struct _dyneither_ptr _tmp56;struct _dyneither_ptr _tmp57;
struct _dyneither_ptr _tmp58;struct _dyneither_ptr _tmp5A;struct _dyneither_ptr _tmp5B;
struct _dyneither_ptr*_tmp5D;int _tmp5E;struct _dyneither_ptr _tmp5F;struct
_dyneither_ptr*_tmp61;int _tmp62;struct _dyneither_ptr _tmp63;_LL1F: {struct Cyc_Typerep_Int_struct*
_tmp3F=(struct Cyc_Typerep_Int_struct*)_tmp3E;if(_tmp3F->tag != 0)goto _LL21;else{
_tmp40=_tmp3F->f1;_tmp41=_tmp3F->f2;}}_LL20:{const char*_tmp1C0;const char*_tmp1BF;
const char*_tmp1BE;void*_tmp1BD[2];struct Cyc_String_pa_struct _tmp1BC;struct Cyc_Int_pa_struct
_tmp1BB;(_tmp1BB.tag=1,((_tmp1BB.f1=(unsigned long)((int)_tmp41),((_tmp1BC.tag=0,((
_tmp1BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(_tmp40?(struct
_dyneither_ptr)((_tmp1BF="signed",_tag_dyneither(_tmp1BF,sizeof(char),7))):(
struct _dyneither_ptr)((_tmp1C0="unsigned",_tag_dyneither(_tmp1C0,sizeof(char),9))))),((
_tmp1BD[0]=& _tmp1BC,((_tmp1BD[1]=& _tmp1BB,Cyc_printf(((_tmp1BE="Int(%s,%d)",
_tag_dyneither(_tmp1BE,sizeof(char),11))),_tag_dyneither(_tmp1BD,sizeof(void*),2)))))))))))));}
goto _LL1E;_LL21: {struct Cyc_Typerep_Float_struct*_tmp42=(struct Cyc_Typerep_Float_struct*)
_tmp3E;if(_tmp42->tag != 1)goto _LL23;}_LL22:{const char*_tmp1C3;void*_tmp1C2;(
_tmp1C2=0,Cyc_printf(((_tmp1C3="Float",_tag_dyneither(_tmp1C3,sizeof(char),6))),
_tag_dyneither(_tmp1C2,sizeof(void*),0)));}goto _LL1E;_LL23: {struct Cyc_Typerep_Double_struct*
_tmp43=(struct Cyc_Typerep_Double_struct*)_tmp3E;if(_tmp43->tag != 2)goto _LL25;}
_LL24:{const char*_tmp1C6;void*_tmp1C5;(_tmp1C5=0,Cyc_printf(((_tmp1C6="Double",
_tag_dyneither(_tmp1C6,sizeof(char),7))),_tag_dyneither(_tmp1C5,sizeof(void*),0)));}
goto _LL1E;_LL25: {struct Cyc_Typerep_ThinPtr_struct*_tmp44=(struct Cyc_Typerep_ThinPtr_struct*)
_tmp3E;if(_tmp44->tag != 3)goto _LL27;else{_tmp45=_tmp44->f1;_tmp46=(void*)_tmp44->f2;}}
_LL26:{const char*_tmp1CA;void*_tmp1C9[1];struct Cyc_Int_pa_struct _tmp1C8;(_tmp1C8.tag=
1,((_tmp1C8.f1=(unsigned long)((int)_tmp45),((_tmp1C9[0]=& _tmp1C8,Cyc_printf(((
_tmp1CA="ThinPtr(%d,",_tag_dyneither(_tmp1CA,sizeof(char),12))),_tag_dyneither(
_tmp1C9,sizeof(void*),1)))))));}Cyc_Typerep_print_typestruct(_tmp46);{const char*
_tmp1CD;void*_tmp1CC;(_tmp1CC=0,Cyc_printf(((_tmp1CD=")",_tag_dyneither(_tmp1CD,
sizeof(char),2))),_tag_dyneither(_tmp1CC,sizeof(void*),0)));}goto _LL1E;_LL27: {
struct Cyc_Typerep_FatPtr_struct*_tmp47=(struct Cyc_Typerep_FatPtr_struct*)_tmp3E;
if(_tmp47->tag != 4)goto _LL29;else{_tmp48=(void*)_tmp47->f1;}}_LL28:{const char*
_tmp1D0;void*_tmp1CF;(_tmp1CF=0,Cyc_printf(((_tmp1D0="FatPtr(",_tag_dyneither(
_tmp1D0,sizeof(char),8))),_tag_dyneither(_tmp1CF,sizeof(void*),0)));}Cyc_Typerep_print_typestruct(
_tmp48);{const char*_tmp1D3;void*_tmp1D2;(_tmp1D2=0,Cyc_printf(((_tmp1D3=")",
_tag_dyneither(_tmp1D3,sizeof(char),2))),_tag_dyneither(_tmp1D2,sizeof(void*),0)));}
goto _LL1E;_LL29: {struct Cyc_Typerep_Struct_struct*_tmp49=(struct Cyc_Typerep_Struct_struct*)
_tmp3E;if(_tmp49->tag != 5)goto _LL2B;else{_tmp4A=_tmp49->f1;_tmp4B=_tmp49->f2;
_tmp4C=_tmp49->f3;}}_LL2A:{const char*_tmp1D8;void*_tmp1D7[2];struct Cyc_String_pa_struct
_tmp1D6;struct Cyc_Int_pa_struct _tmp1D5;(_tmp1D5.tag=1,((_tmp1D5.f1=(
unsigned long)((int)_tmp4B),((_tmp1D6.tag=0,((_tmp1D6.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Typerep_anon_or_name(_tmp4A)),((_tmp1D7[0]=& _tmp1D6,((
_tmp1D7[1]=& _tmp1D5,Cyc_printf(((_tmp1D8="Struct(%s,%d,[",_tag_dyneither(_tmp1D8,
sizeof(char),15))),_tag_dyneither(_tmp1D7,sizeof(void*),2)))))))))))));}{int i=0;
for(0;i < _get_dyneither_size(_tmp4C,sizeof(struct _tuple0*));++ i){unsigned int
_tmp7C;struct _dyneither_ptr _tmp7D;void*_tmp7E;struct _tuple0 _tmp7B=*((struct
_tuple0**)_tmp4C.curr)[i];_tmp7C=_tmp7B.f1;_tmp7D=_tmp7B.f2;_tmp7E=_tmp7B.f3;{
const char*_tmp1DD;void*_tmp1DC[2];struct Cyc_Int_pa_struct _tmp1DB;struct Cyc_String_pa_struct
_tmp1DA;(_tmp1DA.tag=0,((_tmp1DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp7D),((_tmp1DB.tag=1,((_tmp1DB.f1=(unsigned long)((int)_tmp7C),((_tmp1DC[0]=&
_tmp1DB,((_tmp1DC[1]=& _tmp1DA,Cyc_printf(((_tmp1DD="$(%d,%s,",_tag_dyneither(
_tmp1DD,sizeof(char),9))),_tag_dyneither(_tmp1DC,sizeof(void*),2)))))))))))));}
Cyc_Typerep_print_typestruct(_tmp7E);{const char*_tmp1E1;void*_tmp1E0[1];struct
Cyc_Int_pa_struct _tmp1DF;(_tmp1DF.tag=1,((_tmp1DF.f1=(unsigned long)((int)(i != 
_get_dyneither_size(_tmp4C,sizeof(struct _tuple0*))- 1?',':' ')),((_tmp1E0[0]=&
_tmp1DF,Cyc_printf(((_tmp1E1=")%c",_tag_dyneither(_tmp1E1,sizeof(char),4))),
_tag_dyneither(_tmp1E0,sizeof(void*),1)))))));}}}{const char*_tmp1E4;void*_tmp1E3;(
_tmp1E3=0,Cyc_printf(((_tmp1E4=" ]",_tag_dyneither(_tmp1E4,sizeof(char),3))),
_tag_dyneither(_tmp1E3,sizeof(void*),0)));}goto _LL1E;_LL2B: {struct Cyc_Typerep_Tuple_struct*
_tmp4D=(struct Cyc_Typerep_Tuple_struct*)_tmp3E;if(_tmp4D->tag != 6)goto _LL2D;
else{_tmp4E=_tmp4D->f1;_tmp4F=_tmp4D->f2;}}_LL2C:{const char*_tmp1E8;void*_tmp1E7[
1];struct Cyc_Int_pa_struct _tmp1E6;(_tmp1E6.tag=1,((_tmp1E6.f1=(unsigned long)((
int)_tmp4E),((_tmp1E7[0]=& _tmp1E6,Cyc_printf(((_tmp1E8="Tuple(%d,[",
_tag_dyneither(_tmp1E8,sizeof(char),11))),_tag_dyneither(_tmp1E7,sizeof(void*),1)))))));}{
int i=0;for(0;i < _get_dyneither_size(_tmp4F,sizeof(struct _tuple1*));++ i){
unsigned int _tmp8C;void*_tmp8D;struct _tuple1 _tmp8B=*((struct _tuple1**)_tmp4F.curr)[
i];_tmp8C=_tmp8B.f1;_tmp8D=_tmp8B.f2;{const char*_tmp1EC;void*_tmp1EB[1];struct
Cyc_Int_pa_struct _tmp1EA;(_tmp1EA.tag=1,((_tmp1EA.f1=(unsigned long)((int)_tmp8C),((
_tmp1EB[0]=& _tmp1EA,Cyc_printf(((_tmp1EC="$(%d,",_tag_dyneither(_tmp1EC,sizeof(
char),6))),_tag_dyneither(_tmp1EB,sizeof(void*),1)))))));}Cyc_Typerep_print_typestruct(
_tmp8D);{const char*_tmp1F0;void*_tmp1EF[1];struct Cyc_Int_pa_struct _tmp1EE;(
_tmp1EE.tag=1,((_tmp1EE.f1=(unsigned long)((int)(i != _get_dyneither_size(_tmp4F,
sizeof(struct _tuple1*))- 1?',':' ')),((_tmp1EF[0]=& _tmp1EE,Cyc_printf(((_tmp1F0=")%c",
_tag_dyneither(_tmp1F0,sizeof(char),4))),_tag_dyneither(_tmp1EF,sizeof(void*),1)))))));}}}{
const char*_tmp1F3;void*_tmp1F2;(_tmp1F2=0,Cyc_printf(((_tmp1F3=" ]",
_tag_dyneither(_tmp1F3,sizeof(char),3))),_tag_dyneither(_tmp1F2,sizeof(void*),0)));}
goto _LL1E;_LL2D: {struct Cyc_Typerep_TUnionField_struct*_tmp50=(struct Cyc_Typerep_TUnionField_struct*)
_tmp3E;if(_tmp50->tag != 8)goto _LL2F;else{_tmp51=_tmp50->f1;_tmp52=_tmp50->f2;
_tmp53=_tmp50->f3;_tmp54=_tmp50->f4;}}_LL2E:{const char*_tmp1F9;void*_tmp1F8[3];
struct Cyc_String_pa_struct _tmp1F7;struct Cyc_String_pa_struct _tmp1F6;struct Cyc_Int_pa_struct
_tmp1F5;(_tmp1F5.tag=1,((_tmp1F5.f1=(unsigned long)((int)_tmp53),((_tmp1F6.tag=0,((
_tmp1F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp52),((_tmp1F7.tag=0,((
_tmp1F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp51),((_tmp1F8[0]=&
_tmp1F7,((_tmp1F8[1]=& _tmp1F6,((_tmp1F8[2]=& _tmp1F5,Cyc_printf(((_tmp1F9="TUnionField(%s,%s,%d,[",
_tag_dyneither(_tmp1F9,sizeof(char),23))),_tag_dyneither(_tmp1F8,sizeof(void*),3)))))))))))))))))));}{
int i=0;for(0;i < _get_dyneither_size(_tmp54,sizeof(struct _tuple1*));++ i){
unsigned int _tmp9C;void*_tmp9D;struct _tuple1 _tmp9B=*((struct _tuple1**)_tmp54.curr)[
i];_tmp9C=_tmp9B.f1;_tmp9D=_tmp9B.f2;{const char*_tmp1FD;void*_tmp1FC[1];struct
Cyc_Int_pa_struct _tmp1FB;(_tmp1FB.tag=1,((_tmp1FB.f1=(unsigned long)((int)_tmp9C),((
_tmp1FC[0]=& _tmp1FB,Cyc_printf(((_tmp1FD="$(%d,",_tag_dyneither(_tmp1FD,sizeof(
char),6))),_tag_dyneither(_tmp1FC,sizeof(void*),1)))))));}Cyc_Typerep_print_typestruct(
_tmp9D);{const char*_tmp201;void*_tmp200[1];struct Cyc_Int_pa_struct _tmp1FF;(
_tmp1FF.tag=1,((_tmp1FF.f1=(unsigned long)((int)(i != _get_dyneither_size(_tmp54,
sizeof(struct _tuple1*))- 1?',':' ')),((_tmp200[0]=& _tmp1FF,Cyc_printf(((_tmp201=")%c",
_tag_dyneither(_tmp201,sizeof(char),4))),_tag_dyneither(_tmp200,sizeof(void*),1)))))));}}}{
const char*_tmp204;void*_tmp203;(_tmp203=0,Cyc_printf(((_tmp204=" ]",
_tag_dyneither(_tmp204,sizeof(char),3))),_tag_dyneither(_tmp203,sizeof(void*),0)));}
goto _LL1E;_LL2F: {struct Cyc_Typerep_TUnion_struct*_tmp55=(struct Cyc_Typerep_TUnion_struct*)
_tmp3E;if(_tmp55->tag != 7)goto _LL31;else{_tmp56=_tmp55->f1;_tmp57=_tmp55->f2;
_tmp58=_tmp55->f3;}}_LL30:{const char*_tmp208;void*_tmp207[1];struct Cyc_String_pa_struct
_tmp206;(_tmp206.tag=0,((_tmp206.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp56),((_tmp207[0]=& _tmp206,Cyc_printf(((_tmp208="TUnion(%s,[",_tag_dyneither(
_tmp208,sizeof(char),12))),_tag_dyneither(_tmp207,sizeof(void*),1)))))));}{int i=
0;for(0;i < _get_dyneither_size(_tmp57,sizeof(struct _tuple2*));++ i){unsigned int
_tmpAA;struct _dyneither_ptr _tmpAB;struct _tuple2 _tmpA9=*((struct _tuple2**)_tmp57.curr)[
i];_tmpAA=_tmpA9.f1;_tmpAB=_tmpA9.f2;{const char*_tmp20E;void*_tmp20D[3];struct
Cyc_Int_pa_struct _tmp20C;struct Cyc_String_pa_struct _tmp20B;struct Cyc_Int_pa_struct
_tmp20A;(_tmp20A.tag=1,((_tmp20A.f1=(unsigned long)((int)(i != 
_get_dyneither_size(_tmp57,sizeof(struct _tuple2*))- 1?',':' ')),((_tmp20B.tag=0,((
_tmp20B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpAB),((_tmp20C.tag=1,((
_tmp20C.f1=(unsigned long)((int)_tmpAA),((_tmp20D[0]=& _tmp20C,((_tmp20D[1]=&
_tmp20B,((_tmp20D[2]=& _tmp20A,Cyc_printf(((_tmp20E="$(%d,%s)%c",_tag_dyneither(
_tmp20E,sizeof(char),11))),_tag_dyneither(_tmp20D,sizeof(void*),3)))))))))))))))))));}}}{
const char*_tmp211;void*_tmp210;(_tmp210=0,Cyc_printf(((_tmp211="],[",
_tag_dyneither(_tmp211,sizeof(char),4))),_tag_dyneither(_tmp210,sizeof(void*),0)));}{
int i=0;for(0;i < _get_dyneither_size(_tmp58,sizeof(struct _tuple0*));++ i){
unsigned int _tmpB4;struct _dyneither_ptr _tmpB5;void*_tmpB6;struct _tuple0 _tmpB3=*((
struct _tuple0**)_tmp58.curr)[i];_tmpB4=_tmpB3.f1;_tmpB5=_tmpB3.f2;_tmpB6=_tmpB3.f3;{
const char*_tmp216;void*_tmp215[2];struct Cyc_Int_pa_struct _tmp214;struct Cyc_String_pa_struct
_tmp213;(_tmp213.tag=0,((_tmp213.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmpB5),((_tmp214.tag=1,((_tmp214.f1=(unsigned long)((int)_tmpB4),((_tmp215[0]=&
_tmp214,((_tmp215[1]=& _tmp213,Cyc_printf(((_tmp216="$(%d,%s,",_tag_dyneither(
_tmp216,sizeof(char),9))),_tag_dyneither(_tmp215,sizeof(void*),2)))))))))))));}
Cyc_Typerep_print_typestruct(_tmpB6);{const char*_tmp21A;void*_tmp219[1];struct
Cyc_Int_pa_struct _tmp218;(_tmp218.tag=1,((_tmp218.f1=(unsigned long)((int)(i != 
_get_dyneither_size(_tmp58,sizeof(struct _tuple0*))- 1?',':' ')),((_tmp219[0]=&
_tmp218,Cyc_printf(((_tmp21A=")%c",_tag_dyneither(_tmp21A,sizeof(char),4))),
_tag_dyneither(_tmp219,sizeof(void*),1)))))));}}}{const char*_tmp21D;void*_tmp21C;(
_tmp21C=0,Cyc_printf(((_tmp21D="] )",_tag_dyneither(_tmp21D,sizeof(char),4))),
_tag_dyneither(_tmp21C,sizeof(void*),0)));}goto _LL1E;_LL31: {struct Cyc_Typerep_XTUnion_struct*
_tmp59=(struct Cyc_Typerep_XTUnion_struct*)_tmp3E;if(_tmp59->tag != 9)goto _LL33;
else{_tmp5A=_tmp59->f1;_tmp5B=_tmp59->f2;}}_LL32:{const char*_tmp221;void*_tmp220[
1];struct Cyc_String_pa_struct _tmp21F;(_tmp21F.tag=0,((_tmp21F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp5A),((_tmp220[0]=& _tmp21F,Cyc_printf(((
_tmp221="XTUnion(%s,",_tag_dyneither(_tmp221,sizeof(char),12))),_tag_dyneither(
_tmp220,sizeof(void*),1)))))));}{int i=0;for(0;i < _get_dyneither_size(_tmp5B,
sizeof(struct _tuple3*));++ i){struct _dyneither_ptr _tmpC4;void*_tmpC5;struct
_tuple3 _tmpC3=*((struct _tuple3**)_tmp5B.curr)[i];_tmpC4=_tmpC3.f1;_tmpC5=_tmpC3.f2;{
const char*_tmp225;void*_tmp224[1];struct Cyc_String_pa_struct _tmp223;(_tmp223.tag=
0,((_tmp223.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(
_tmpC4,sizeof(char),4)),((_tmp224[0]=& _tmp223,Cyc_printf(((_tmp225="$(%s,",
_tag_dyneither(_tmp225,sizeof(char),6))),_tag_dyneither(_tmp224,sizeof(void*),1)))))));}
Cyc_Typerep_print_typestruct(_tmpC5);{const char*_tmp229;void*_tmp228[1];struct
Cyc_Int_pa_struct _tmp227;(_tmp227.tag=1,((_tmp227.f1=(unsigned long)((int)(i != 
_get_dyneither_size(_tmp5B,sizeof(struct _tuple3*))- 1?',':' ')),((_tmp228[0]=&
_tmp227,Cyc_printf(((_tmp229=")%c",_tag_dyneither(_tmp229,sizeof(char),4))),
_tag_dyneither(_tmp228,sizeof(void*),1)))))));}}}{const char*_tmp22C;void*_tmp22B;(
_tmp22B=0,Cyc_printf(((_tmp22C=" )",_tag_dyneither(_tmp22C,sizeof(char),3))),
_tag_dyneither(_tmp22B,sizeof(void*),0)));}goto _LL1E;_LL33: {struct Cyc_Typerep_Union_struct*
_tmp5C=(struct Cyc_Typerep_Union_struct*)_tmp3E;if(_tmp5C->tag != 10)goto _LL35;
else{_tmp5D=_tmp5C->f1;_tmp5E=_tmp5C->f2;_tmp5F=_tmp5C->f3;}}_LL34:{const char*
_tmp231;void*_tmp230[2];struct Cyc_String_pa_struct _tmp22F;struct Cyc_Int_pa_struct
_tmp22E;(_tmp22E.tag=1,((_tmp22E.f1=(unsigned long)_tmp5E,((_tmp22F.tag=0,((
_tmp22F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Typerep_anon_or_name(
_tmp5D)),((_tmp230[0]=& _tmp22F,((_tmp230[1]=& _tmp22E,Cyc_printf(((_tmp231="Union(%s,%d,",
_tag_dyneither(_tmp231,sizeof(char),13))),_tag_dyneither(_tmp230,sizeof(void*),2)))))))))))));}{
int i=0;for(0;i < _get_dyneither_size(_tmp5F,sizeof(struct _tuple3*));++ i){struct
_dyneither_ptr _tmpD3;void*_tmpD4;struct _tuple3 _tmpD2=*((struct _tuple3**)_tmp5F.curr)[
i];_tmpD3=_tmpD2.f1;_tmpD4=_tmpD2.f2;{const char*_tmp235;void*_tmp234[1];struct
Cyc_String_pa_struct _tmp233;(_tmp233.tag=0,((_tmp233.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmpD3),((_tmp234[0]=& _tmp233,Cyc_printf(((_tmp235="$(%s,",
_tag_dyneither(_tmp235,sizeof(char),6))),_tag_dyneither(_tmp234,sizeof(void*),1)))))));}
Cyc_Typerep_print_typestruct(_tmpD4);{const char*_tmp239;void*_tmp238[1];struct
Cyc_Int_pa_struct _tmp237;(_tmp237.tag=1,((_tmp237.f1=(unsigned long)((int)(i != 
_get_dyneither_size(_tmp5F,sizeof(struct _tuple3*))- 1?',':' ')),((_tmp238[0]=&
_tmp237,Cyc_printf(((_tmp239=")%c",_tag_dyneither(_tmp239,sizeof(char),4))),
_tag_dyneither(_tmp238,sizeof(void*),1)))))));}}}{const char*_tmp23C;void*_tmp23B;(
_tmp23B=0,Cyc_printf(((_tmp23C=")",_tag_dyneither(_tmp23C,sizeof(char),2))),
_tag_dyneither(_tmp23B,sizeof(void*),0)));}goto _LL1E;_LL35: {struct Cyc_Typerep_Enum_struct*
_tmp60=(struct Cyc_Typerep_Enum_struct*)_tmp3E;if(_tmp60->tag != 11)goto _LL1E;
else{_tmp61=_tmp60->f1;_tmp62=_tmp60->f2;_tmp63=_tmp60->f3;}}_LL36:{const char*
_tmp241;void*_tmp240[2];struct Cyc_String_pa_struct _tmp23F;struct Cyc_Int_pa_struct
_tmp23E;(_tmp23E.tag=1,((_tmp23E.f1=(unsigned long)_tmp62,((_tmp23F.tag=0,((
_tmp23F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Typerep_anon_or_name(
_tmp61)),((_tmp240[0]=& _tmp23F,((_tmp240[1]=& _tmp23E,Cyc_printf(((_tmp241="Enum(%s,%d,",
_tag_dyneither(_tmp241,sizeof(char),12))),_tag_dyneither(_tmp240,sizeof(void*),2)))))))))))));}{
int i=0;for(0;i < _get_dyneither_size(_tmp63,sizeof(struct _tuple2*));++ i){
unsigned int _tmpE2;struct _dyneither_ptr _tmpE3;struct _tuple2 _tmpE1=*((struct
_tuple2**)_tmp63.curr)[i];_tmpE2=_tmpE1.f1;_tmpE3=_tmpE1.f2;{const char*_tmp247;
void*_tmp246[3];struct Cyc_Int_pa_struct _tmp245;struct Cyc_String_pa_struct _tmp244;
struct Cyc_Int_pa_struct _tmp243;(_tmp243.tag=1,((_tmp243.f1=(unsigned long)((int)(
i != _get_dyneither_size(_tmp63,sizeof(struct _tuple2*))- 1?',':' ')),((_tmp244.tag=
0,((_tmp244.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE3),((_tmp245.tag=
1,((_tmp245.f1=(unsigned long)((int)_tmpE2),((_tmp246[0]=& _tmp245,((_tmp246[1]=&
_tmp244,((_tmp246[2]=& _tmp243,Cyc_printf(((_tmp247="$(%d,%s)%c",_tag_dyneither(
_tmp247,sizeof(char),11))),_tag_dyneither(_tmp246,sizeof(void*),3)))))))))))))))))));}}}{
const char*_tmp24A;void*_tmp249;(_tmp249=0,Cyc_printf(((_tmp24A=")",
_tag_dyneither(_tmp24A,sizeof(char),2))),_tag_dyneither(_tmp249,sizeof(void*),0)));}
goto _LL1E;_LL1E:;}void Cyc_Typerep_xmlize_typestruct(void*rep);void Cyc_Typerep_xmlize_typestruct(
void*rep){void*_tmpEB=rep;int _tmpED;unsigned int _tmpEE;unsigned int _tmpF2;void*
_tmpF3;void*_tmpF5;struct _dyneither_ptr*_tmpF7;unsigned int _tmpF8;struct
_dyneither_ptr _tmpF9;unsigned int _tmpFB;struct _dyneither_ptr _tmpFC;struct
_dyneither_ptr _tmpFE;struct _dyneither_ptr _tmpFF;unsigned int _tmp100;struct
_dyneither_ptr _tmp101;struct _dyneither_ptr _tmp103;struct _dyneither_ptr _tmp104;
struct _dyneither_ptr _tmp105;struct _dyneither_ptr _tmp107;struct _dyneither_ptr
_tmp108;struct _dyneither_ptr*_tmp10A;int _tmp10B;struct _dyneither_ptr _tmp10C;
struct _dyneither_ptr*_tmp10E;int _tmp10F;struct _dyneither_ptr _tmp110;_LL38: {
struct Cyc_Typerep_Int_struct*_tmpEC=(struct Cyc_Typerep_Int_struct*)_tmpEB;if(
_tmpEC->tag != 0)goto _LL3A;else{_tmpED=_tmpEC->f1;_tmpEE=_tmpEC->f2;}}_LL39:{
const char*_tmp24F;void*_tmp24E[2];struct Cyc_Int_pa_struct _tmp24D;struct Cyc_Int_pa_struct
_tmp24C;(_tmp24C.tag=1,((_tmp24C.f1=(unsigned long)((int)_tmpEE),((_tmp24D.tag=1,((
_tmp24D.f1=(unsigned long)_tmpED,((_tmp24E[0]=& _tmp24D,((_tmp24E[1]=& _tmp24C,Cyc_printf(((
_tmp24F="<Int sn=%d sz=\"%d\"/>",_tag_dyneither(_tmp24F,sizeof(char),21))),
_tag_dyneither(_tmp24E,sizeof(void*),2)))))))))))));}goto _LL37;_LL3A: {struct Cyc_Typerep_Float_struct*
_tmpEF=(struct Cyc_Typerep_Float_struct*)_tmpEB;if(_tmpEF->tag != 1)goto _LL3C;}
_LL3B:{const char*_tmp252;void*_tmp251;(_tmp251=0,Cyc_printf(((_tmp252="<Float/>",
_tag_dyneither(_tmp252,sizeof(char),9))),_tag_dyneither(_tmp251,sizeof(void*),0)));}
goto _LL37;_LL3C: {struct Cyc_Typerep_Double_struct*_tmpF0=(struct Cyc_Typerep_Double_struct*)
_tmpEB;if(_tmpF0->tag != 2)goto _LL3E;}_LL3D:{const char*_tmp255;void*_tmp254;(
_tmp254=0,Cyc_printf(((_tmp255="<Double/>",_tag_dyneither(_tmp255,sizeof(char),
10))),_tag_dyneither(_tmp254,sizeof(void*),0)));}goto _LL37;_LL3E: {struct Cyc_Typerep_ThinPtr_struct*
_tmpF1=(struct Cyc_Typerep_ThinPtr_struct*)_tmpEB;if(_tmpF1->tag != 3)goto _LL40;
else{_tmpF2=_tmpF1->f1;_tmpF3=(void*)_tmpF1->f2;}}_LL3F:{const char*_tmp259;void*
_tmp258[1];struct Cyc_Int_pa_struct _tmp257;(_tmp257.tag=1,((_tmp257.f1=(
unsigned long)((int)_tmpF2),((_tmp258[0]=& _tmp257,Cyc_printf(((_tmp259="<ThinPtr sz=\"%d\">",
_tag_dyneither(_tmp259,sizeof(char),18))),_tag_dyneither(_tmp258,sizeof(void*),1)))))));}
Cyc_Typerep_xmlize_typestruct(_tmpF3);{const char*_tmp25C;void*_tmp25B;(_tmp25B=0,
Cyc_printf(((_tmp25C="</ThinPtr",_tag_dyneither(_tmp25C,sizeof(char),10))),
_tag_dyneither(_tmp25B,sizeof(void*),0)));}goto _LL37;_LL40: {struct Cyc_Typerep_FatPtr_struct*
_tmpF4=(struct Cyc_Typerep_FatPtr_struct*)_tmpEB;if(_tmpF4->tag != 4)goto _LL42;
else{_tmpF5=(void*)_tmpF4->f1;}}_LL41:{const char*_tmp25F;void*_tmp25E;(_tmp25E=0,
Cyc_printf(((_tmp25F="<FatPtr>",_tag_dyneither(_tmp25F,sizeof(char),9))),
_tag_dyneither(_tmp25E,sizeof(void*),0)));}Cyc_Typerep_xmlize_typestruct(_tmpF5);{
const char*_tmp262;void*_tmp261;(_tmp261=0,Cyc_printf(((_tmp262="</FatPtr>",
_tag_dyneither(_tmp262,sizeof(char),10))),_tag_dyneither(_tmp261,sizeof(void*),0)));}
goto _LL37;_LL42: {struct Cyc_Typerep_Struct_struct*_tmpF6=(struct Cyc_Typerep_Struct_struct*)
_tmpEB;if(_tmpF6->tag != 5)goto _LL44;else{_tmpF7=_tmpF6->f1;_tmpF8=_tmpF6->f2;
_tmpF9=_tmpF6->f3;}}_LL43:{const char*_tmp267;void*_tmp266[2];struct Cyc_String_pa_struct
_tmp265;struct Cyc_Int_pa_struct _tmp264;(_tmp264.tag=1,((_tmp264.f1=(
unsigned long)((int)_tmpF8),((_tmp265.tag=0,((_tmp265.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Typerep_anon_or_name(_tmpF7)),((_tmp266[0]=& _tmp265,((
_tmp266[1]=& _tmp264,Cyc_printf(((_tmp267="<Struct name=\"%s\" sz=\"%d\">",
_tag_dyneither(_tmp267,sizeof(char),27))),_tag_dyneither(_tmp266,sizeof(void*),2)))))))))))));}{
int i=0;for(0;i < _get_dyneither_size(_tmpF9,sizeof(struct _tuple0*));++ i){
unsigned int _tmp127;struct _dyneither_ptr _tmp128;void*_tmp129;struct _tuple0
_tmp126=*((struct _tuple0**)_tmpF9.curr)[i];_tmp127=_tmp126.f1;_tmp128=_tmp126.f2;
_tmp129=_tmp126.f3;{const char*_tmp26C;void*_tmp26B[2];struct Cyc_Int_pa_struct
_tmp26A;struct Cyc_String_pa_struct _tmp269;(_tmp269.tag=0,((_tmp269.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp128),((_tmp26A.tag=1,((_tmp26A.f1=(
unsigned long)((int)_tmp127),((_tmp26B[0]=& _tmp26A,((_tmp26B[1]=& _tmp269,Cyc_printf(((
_tmp26C="<Component ofs=\"%d\" nm=\"%s\">",_tag_dyneither(_tmp26C,sizeof(char),
29))),_tag_dyneither(_tmp26B,sizeof(void*),2)))))))))))));}Cyc_Typerep_xmlize_typestruct(
_tmp129);{const char*_tmp26F;void*_tmp26E;(_tmp26E=0,Cyc_printf(((_tmp26F="</Component>",
_tag_dyneither(_tmp26F,sizeof(char),13))),_tag_dyneither(_tmp26E,sizeof(void*),0)));}}}{
const char*_tmp272;void*_tmp271;(_tmp271=0,Cyc_printf(((_tmp272="</Struct>",
_tag_dyneither(_tmp272,sizeof(char),10))),_tag_dyneither(_tmp271,sizeof(void*),0)));}
goto _LL37;_LL44: {struct Cyc_Typerep_Tuple_struct*_tmpFA=(struct Cyc_Typerep_Tuple_struct*)
_tmpEB;if(_tmpFA->tag != 6)goto _LL46;else{_tmpFB=_tmpFA->f1;_tmpFC=_tmpFA->f2;}}
_LL45:{const char*_tmp276;void*_tmp275[1];struct Cyc_Int_pa_struct _tmp274;(_tmp274.tag=
1,((_tmp274.f1=(unsigned long)((int)_tmpFB),((_tmp275[0]=& _tmp274,Cyc_printf(((
_tmp276="<Tuple sz=\"%d\">",_tag_dyneither(_tmp276,sizeof(char),16))),
_tag_dyneither(_tmp275,sizeof(void*),1)))))));}{int i=0;for(0;i < 
_get_dyneither_size(_tmpFC,sizeof(struct _tuple1*));++ i){unsigned int _tmp136;void*
_tmp137;struct _tuple1 _tmp135=*((struct _tuple1**)_tmpFC.curr)[i];_tmp136=_tmp135.f1;
_tmp137=_tmp135.f2;{const char*_tmp27A;void*_tmp279[1];struct Cyc_Int_pa_struct
_tmp278;(_tmp278.tag=1,((_tmp278.f1=(unsigned long)((int)_tmp136),((_tmp279[0]=&
_tmp278,Cyc_printf(((_tmp27A="<Component ofs=\"%d\">",_tag_dyneither(_tmp27A,
sizeof(char),21))),_tag_dyneither(_tmp279,sizeof(void*),1)))))));}Cyc_Typerep_xmlize_typestruct(
_tmp137);{const char*_tmp27D;void*_tmp27C;(_tmp27C=0,Cyc_printf(((_tmp27D="</Component>",
_tag_dyneither(_tmp27D,sizeof(char),13))),_tag_dyneither(_tmp27C,sizeof(void*),0)));}}}{
const char*_tmp280;void*_tmp27F;(_tmp27F=0,Cyc_printf(((_tmp280="</Tuple>",
_tag_dyneither(_tmp280,sizeof(char),9))),_tag_dyneither(_tmp27F,sizeof(void*),0)));}
goto _LL37;_LL46: {struct Cyc_Typerep_TUnionField_struct*_tmpFD=(struct Cyc_Typerep_TUnionField_struct*)
_tmpEB;if(_tmpFD->tag != 8)goto _LL48;else{_tmpFE=_tmpFD->f1;_tmpFF=_tmpFD->f2;
_tmp100=_tmpFD->f3;_tmp101=_tmpFD->f4;}}_LL47:{const char*_tmp286;void*_tmp285[3];
struct Cyc_String_pa_struct _tmp284;struct Cyc_String_pa_struct _tmp283;struct Cyc_Int_pa_struct
_tmp282;(_tmp282.tag=1,((_tmp282.f1=(unsigned long)((int)_tmp100),((_tmp283.tag=
0,((_tmp283.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFF),((_tmp284.tag=
0,((_tmp284.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFE),((_tmp285[0]=&
_tmp284,((_tmp285[1]=& _tmp283,((_tmp285[2]=& _tmp282,Cyc_printf(((_tmp286="<TUnionField tname=\"%s\" fname=\"%s\" sz=\"%d\">",
_tag_dyneither(_tmp286,sizeof(char),44))),_tag_dyneither(_tmp285,sizeof(void*),3)))))))))))))))))));}{
int i=0;for(0;i < _get_dyneither_size(_tmp101,sizeof(struct _tuple1*));++ i){
unsigned int _tmp145;void*_tmp146;struct _tuple1 _tmp144=*((struct _tuple1**)_tmp101.curr)[
i];_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;{const char*_tmp28A;void*_tmp289[1];
struct Cyc_Int_pa_struct _tmp288;(_tmp288.tag=1,((_tmp288.f1=(unsigned long)((int)
_tmp145),((_tmp289[0]=& _tmp288,Cyc_printf(((_tmp28A="<Component ofs=\"%d\">",
_tag_dyneither(_tmp28A,sizeof(char),21))),_tag_dyneither(_tmp289,sizeof(void*),1)))))));}
Cyc_Typerep_xmlize_typestruct(_tmp146);{const char*_tmp28D;void*_tmp28C;(_tmp28C=
0,Cyc_printf(((_tmp28D="</Component>",_tag_dyneither(_tmp28D,sizeof(char),13))),
_tag_dyneither(_tmp28C,sizeof(void*),0)));}}}{const char*_tmp290;void*_tmp28F;(
_tmp28F=0,Cyc_printf(((_tmp290="</TUnionField>",_tag_dyneither(_tmp290,sizeof(
char),15))),_tag_dyneither(_tmp28F,sizeof(void*),0)));}goto _LL37;_LL48: {struct
Cyc_Typerep_TUnion_struct*_tmp102=(struct Cyc_Typerep_TUnion_struct*)_tmpEB;if(
_tmp102->tag != 7)goto _LL4A;else{_tmp103=_tmp102->f1;_tmp104=_tmp102->f2;_tmp105=
_tmp102->f3;}}_LL49:{const char*_tmp294;void*_tmp293[1];struct Cyc_String_pa_struct
_tmp292;(_tmp292.tag=0,((_tmp292.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp103),((_tmp293[0]=& _tmp292,Cyc_printf(((_tmp294="<TUnion name=\"%s\">",
_tag_dyneither(_tmp294,sizeof(char),19))),_tag_dyneither(_tmp293,sizeof(void*),1)))))));}{
int i=0;for(0;i < _get_dyneither_size(_tmp104,sizeof(struct _tuple2*));++ i){
unsigned int _tmp152;struct _dyneither_ptr _tmp153;struct _tuple2 _tmp151=*((struct
_tuple2**)_tmp104.curr)[i];_tmp152=_tmp151.f1;_tmp153=_tmp151.f2;{const char*
_tmp299;void*_tmp298[2];struct Cyc_Int_pa_struct _tmp297;struct Cyc_String_pa_struct
_tmp296;(_tmp296.tag=0,((_tmp296.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp153),((_tmp297.tag=1,((_tmp297.f1=(unsigned long)((int)_tmp152),((_tmp298[0]=&
_tmp297,((_tmp298[1]=& _tmp296,Cyc_printf(((_tmp299="<Tag tag=\"%d\" nm=\"%s\"/>",
_tag_dyneither(_tmp299,sizeof(char),24))),_tag_dyneither(_tmp298,sizeof(void*),2)))))))))))));}}}{
int i=0;for(0;i < _get_dyneither_size(_tmp105,sizeof(struct _tuple0*));++ i){
unsigned int _tmp159;struct _dyneither_ptr _tmp15A;void*_tmp15B;struct _tuple0
_tmp158=*((struct _tuple0**)_tmp105.curr)[i];_tmp159=_tmp158.f1;_tmp15A=_tmp158.f2;
_tmp15B=_tmp158.f3;{const char*_tmp29E;void*_tmp29D[2];struct Cyc_Int_pa_struct
_tmp29C;struct Cyc_String_pa_struct _tmp29B;(_tmp29B.tag=0,((_tmp29B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp15A),((_tmp29C.tag=1,((_tmp29C.f1=(
unsigned long)((int)_tmp159),((_tmp29D[0]=& _tmp29C,((_tmp29D[1]=& _tmp29B,Cyc_printf(((
_tmp29E="<Tag tag=\"%d\" nm=\"%s\">",_tag_dyneither(_tmp29E,sizeof(char),23))),
_tag_dyneither(_tmp29D,sizeof(void*),2)))))))))))));}Cyc_Typerep_xmlize_typestruct(
_tmp15B);{const char*_tmp2A1;void*_tmp2A0;(_tmp2A0=0,Cyc_printf(((_tmp2A1="</Tag>",
_tag_dyneither(_tmp2A1,sizeof(char),7))),_tag_dyneither(_tmp2A0,sizeof(void*),0)));}}}{
const char*_tmp2A4;void*_tmp2A3;(_tmp2A3=0,Cyc_printf(((_tmp2A4="</Tunion>",
_tag_dyneither(_tmp2A4,sizeof(char),10))),_tag_dyneither(_tmp2A3,sizeof(void*),0)));}
goto _LL37;_LL4A: {struct Cyc_Typerep_XTUnion_struct*_tmp106=(struct Cyc_Typerep_XTUnion_struct*)
_tmpEB;if(_tmp106->tag != 9)goto _LL4C;else{_tmp107=_tmp106->f1;_tmp108=_tmp106->f2;}}
_LL4B:{const char*_tmp2A8;void*_tmp2A7[1];struct Cyc_String_pa_struct _tmp2A6;(
_tmp2A6.tag=0,((_tmp2A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp107),((
_tmp2A7[0]=& _tmp2A6,Cyc_printf(((_tmp2A8="<XTUnion name=\"%s\">",_tag_dyneither(
_tmp2A8,sizeof(char),20))),_tag_dyneither(_tmp2A7,sizeof(void*),1)))))));}{int i=
0;for(0;i < _get_dyneither_size(_tmp108,sizeof(struct _tuple3*));++ i){struct
_dyneither_ptr _tmp168;void*_tmp169;struct _tuple3 _tmp167=*((struct _tuple3**)
_tmp108.curr)[i];_tmp168=_tmp167.f1;_tmp169=_tmp167.f2;{const char*_tmp2AC;void*
_tmp2AB[1];struct Cyc_String_pa_struct _tmp2AA;(_tmp2AA.tag=0,((_tmp2AA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_tmp168,sizeof(char),4)),((
_tmp2AB[0]=& _tmp2AA,Cyc_printf(((_tmp2AC="<Tag tag=\"%s\">",_tag_dyneither(
_tmp2AC,sizeof(char),15))),_tag_dyneither(_tmp2AB,sizeof(void*),1)))))));}Cyc_Typerep_xmlize_typestruct(
_tmp169);{const char*_tmp2AF;void*_tmp2AE;(_tmp2AE=0,Cyc_printf(((_tmp2AF="</Tag>",
_tag_dyneither(_tmp2AF,sizeof(char),7))),_tag_dyneither(_tmp2AE,sizeof(void*),0)));}}}{
const char*_tmp2B2;void*_tmp2B1;(_tmp2B1=0,Cyc_printf(((_tmp2B2="</XTunion>",
_tag_dyneither(_tmp2B2,sizeof(char),11))),_tag_dyneither(_tmp2B1,sizeof(void*),0)));}
goto _LL37;_LL4C: {struct Cyc_Typerep_Union_struct*_tmp109=(struct Cyc_Typerep_Union_struct*)
_tmpEB;if(_tmp109->tag != 10)goto _LL4E;else{_tmp10A=_tmp109->f1;_tmp10B=_tmp109->f2;
_tmp10C=_tmp109->f3;}}_LL4D:{const char*_tmp2B7;void*_tmp2B6[2];struct Cyc_String_pa_struct
_tmp2B5;struct Cyc_Int_pa_struct _tmp2B4;(_tmp2B4.tag=1,((_tmp2B4.f1=(
unsigned long)_tmp10B,((_tmp2B5.tag=0,((_tmp2B5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Typerep_anon_or_name(_tmp10A)),((_tmp2B6[0]=& _tmp2B5,((
_tmp2B6[1]=& _tmp2B4,Cyc_printf(((_tmp2B7="<Union name=\"%s\" sz=\"%d\">",
_tag_dyneither(_tmp2B7,sizeof(char),26))),_tag_dyneither(_tmp2B6,sizeof(void*),2)))))))))))));}{
int i=0;for(0;i < _get_dyneither_size(_tmp10C,sizeof(struct _tuple3*));++ i){struct
_dyneither_ptr _tmp176;void*_tmp177;struct _tuple3 _tmp175=*((struct _tuple3**)
_tmp10C.curr)[i];_tmp176=_tmp175.f1;_tmp177=_tmp175.f2;{const char*_tmp2BB;void*
_tmp2BA[1];struct Cyc_String_pa_struct _tmp2B9;(_tmp2B9.tag=0,((_tmp2B9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp176),((_tmp2BA[0]=& _tmp2B9,Cyc_printf(((
_tmp2BB="<Case nm=\"%s\">",_tag_dyneither(_tmp2BB,sizeof(char),15))),
_tag_dyneither(_tmp2BA,sizeof(void*),1)))))));}Cyc_Typerep_xmlize_typestruct(
_tmp177);{const char*_tmp2BE;void*_tmp2BD;(_tmp2BD=0,Cyc_printf(((_tmp2BE="</Case>",
_tag_dyneither(_tmp2BE,sizeof(char),8))),_tag_dyneither(_tmp2BD,sizeof(void*),0)));}}}{
const char*_tmp2C1;void*_tmp2C0;(_tmp2C0=0,Cyc_printf(((_tmp2C1="</Union>",
_tag_dyneither(_tmp2C1,sizeof(char),9))),_tag_dyneither(_tmp2C0,sizeof(void*),0)));}
goto _LL37;_LL4E: {struct Cyc_Typerep_Enum_struct*_tmp10D=(struct Cyc_Typerep_Enum_struct*)
_tmpEB;if(_tmp10D->tag != 11)goto _LL37;else{_tmp10E=_tmp10D->f1;_tmp10F=_tmp10D->f2;
_tmp110=_tmp10D->f3;}}_LL4F:{const char*_tmp2C6;void*_tmp2C5[2];struct Cyc_String_pa_struct
_tmp2C4;struct Cyc_Int_pa_struct _tmp2C3;(_tmp2C3.tag=1,((_tmp2C3.f1=(
unsigned long)_tmp10F,((_tmp2C4.tag=0,((_tmp2C4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Typerep_anon_or_name(_tmp10E)),((_tmp2C5[0]=& _tmp2C4,((
_tmp2C5[1]=& _tmp2C3,Cyc_printf(((_tmp2C6="<Enum name=\"%s\" szb=\"%d\">",
_tag_dyneither(_tmp2C6,sizeof(char),26))),_tag_dyneither(_tmp2C5,sizeof(void*),2)))))))))))));}{
int i=0;for(0;i < _get_dyneither_size(_tmp110,sizeof(struct _tuple2*));++ i){
unsigned int _tmp184;struct _dyneither_ptr _tmp185;struct _tuple2 _tmp183=*((struct
_tuple2**)_tmp110.curr)[i];_tmp184=_tmp183.f1;_tmp185=_tmp183.f2;{const char*
_tmp2CB;void*_tmp2CA[2];struct Cyc_Int_pa_struct _tmp2C9;struct Cyc_String_pa_struct
_tmp2C8;(_tmp2C8.tag=0,((_tmp2C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp185),((_tmp2C9.tag=1,((_tmp2C9.f1=(unsigned long)((int)_tmp184),((_tmp2CA[0]=&
_tmp2C9,((_tmp2CA[1]=& _tmp2C8,Cyc_printf(((_tmp2CB="<Val val=\"%d\">%s</Val>",
_tag_dyneither(_tmp2CB,sizeof(char),23))),_tag_dyneither(_tmp2CA,sizeof(void*),2)))))))))))));}}}{
const char*_tmp2CE;void*_tmp2CD;(_tmp2CD=0,Cyc_printf(((_tmp2CE="</Enum>",
_tag_dyneither(_tmp2CE,sizeof(char),8))),_tag_dyneither(_tmp2CD,sizeof(void*),0)));}
goto _LL37;_LL37:;}
