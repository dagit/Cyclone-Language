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
  if (!_cus_arr.base) _throw_null();
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
  if (!_cus_arr.base) _throw_null(); \
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
 void exit(int);struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct
Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[16];
extern char Cyc_Core_Free_Region[16];struct _dyneither_ptr Cstring_to_string(char*);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[
18];extern char Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;int(*next)(void*env,
void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(
struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);
extern char Cyc_Set_Absent[11];struct Cyc_Typerep_Int_struct{int tag;int f1;
unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;void*f2;
};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple0{unsigned int f1;
struct _dyneither_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{int tag;struct
_dyneither_ptr*f1;unsigned int f2;struct _dyneither_ptr f3;};struct _tuple1{
unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;
struct _dyneither_ptr f2;};struct _tuple2{unsigned int f1;struct _dyneither_ptr f2;};
struct Cyc_Typerep_TUnion_struct{int tag;struct _dyneither_ptr f1;struct
_dyneither_ptr f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;unsigned int f3;struct
_dyneither_ptr f4;};struct _tuple3{struct _dyneither_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};struct _tuple4{int
f1;void*f2;};struct _tuple4 Cyc_Typerep_get_unionbranch(unsigned int tag,struct
_dyneither_ptr l);struct _dyneither_ptr Cyc_Typerep_get_tagname(unsigned int tag,
struct _dyneither_ptr l);struct _dyneither_ptr Cyc_Typerep_get_tagname2(unsigned int
tag,struct _dyneither_ptr l);struct _tuple4 Cyc_Typerep_get_xtunionbranch(struct
_dyneither_ptr tag,struct _dyneither_ptr l);unsigned int Cyc_Typerep_size_type(void*
rep);struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;extern struct Cyc___cycFILE*
Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct
Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;
long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};int Cyc_fgetc(struct Cyc___cycFILE*);int Cyc_fprintf(
struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);int Cyc_fputc(int,
struct Cyc___cycFILE*);unsigned long Cyc_fread(struct _dyneither_ptr,unsigned long,
unsigned long,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;
};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};int Cyc_strcmp(
struct _dyneither_ptr s1,struct _dyneither_ptr s2);int isprint(int);struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict
Cyc_Dict_empty(int(*cmp)(void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict d,
void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict d,void*k);struct _tuple5{void*f1;void*f2;};struct _tuple5*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple5*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);void Cyc_Marshal_fprint_type(void*rep,
struct Cyc___cycFILE*fp,void*val);void Cyc_Marshal_print_type(void*rep,void*val);
void Cyc_Marshal_write_type(void*rep,struct Cyc___cycFILE*fp,void*val);struct
_tuple6{struct Cyc_Dict_Dict f1;int f2;};struct _tuple7{struct _dyneither_ptr f1;int f2;
};struct _tuple6 Cyc_Marshal_empty_addr_index();struct _tuple6 Cyc_Marshal_write_type_base(
void*rep,struct _tuple6 env,struct Cyc___cycFILE*fp,void*val);void*Cyc_Marshal_rread_type(
struct _RegionHandle*r,void*rep,struct Cyc___cycFILE*fp);void*Cyc_Marshal_read_type(
void*rep,struct Cyc___cycFILE*fp);void*Cyc_Marshal_rcopy_type(struct _RegionHandle*,
void*rep,void*val);void*Cyc_Marshal_copy_type(void*rep,void*val);int Cyc_Marshal_leq_type(
void*rep,void*x,void*y);int Cyc_Marshal_eq_type(void*rep,void*x,void*y);int Cyc_Marshal_cmp_type(
void*rep,void*x,void*y);void*unsafe_cast(void*x);static int Cyc_Marshal_uint_cmp(
unsigned int a,unsigned int b);static int Cyc_Marshal_uint_cmp(unsigned int a,
unsigned int b){if(a == b)return 0;else{if(a < b)return 1;else{return - 1;}}}static
struct Cyc_Set_Set*Cyc_Marshal_empty_addr_set();static struct Cyc_Set_Set*Cyc_Marshal_empty_addr_set(){
return((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,unsigned int elt))Cyc_Set_insert)(((
struct Cyc_Set_Set*(*)(int(*cmp)(unsigned int,unsigned int)))Cyc_Set_empty)(Cyc_Marshal_uint_cmp),
0);}struct _tuple8{int f1;struct Cyc_Set_Set*f2;};static struct _tuple8*Cyc_Marshal_member_insert(
struct Cyc_Set_Set*set,unsigned int val);static struct _tuple8*Cyc_Marshal_member_insert(
struct Cyc_Set_Set*set,unsigned int val){struct _tuple8*_tmp2A2;return(_tmp2A2=
_cycalloc(sizeof(*_tmp2A2)),((_tmp2A2->f1=((int(*)(struct Cyc_Set_Set*s,
unsigned int elt))Cyc_Set_member)(set,val),((_tmp2A2->f2=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,unsigned int elt))Cyc_Set_insert)(set,val),_tmp2A2)))));}
static int Cyc_Marshal_index_member(struct _tuple6 idx,unsigned int m);inline static
int Cyc_Marshal_index_member(struct _tuple6 idx,unsigned int m){int _tmp2;struct
_tuple6 _tmp1=idx;_tmp2=_tmp1.f2;return m < _tmp2;}struct _tuple6 Cyc_Marshal_empty_addr_index();
struct _tuple6 Cyc_Marshal_empty_addr_index(){struct _tuple6 _tmp2A3;return(_tmp2A3.f1=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,unsigned int k,int v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(int(*cmp)(unsigned int,unsigned int)))Cyc_Dict_empty)(Cyc_Marshal_uint_cmp),
0,0),((_tmp2A3.f2=1,_tmp2A3)));}static unsigned int Cyc_Marshal_index_lookup(
struct _tuple6 index,unsigned int val);static unsigned int Cyc_Marshal_index_lookup(
struct _tuple6 index,unsigned int val){struct Cyc_Dict_Dict _tmp5;int _tmp6;struct
_tuple6 _tmp4=index;_tmp5=_tmp4.f1;_tmp6=_tmp4.f2;{int*_tmp7=((int*(*)(struct Cyc_Dict_Dict
d,unsigned int k))Cyc_Dict_lookup_opt)(_tmp5,val);if(_tmp7 != 0)return(
unsigned int)*_tmp7;else{return(unsigned int)_tmp6;}}}static struct _tuple6 Cyc_Marshal_index_insert(
struct _tuple6 index,unsigned int val);static struct _tuple6 Cyc_Marshal_index_insert(
struct _tuple6 index,unsigned int val){struct Cyc_Dict_Dict _tmp9;int _tmpA;struct
_tuple6 _tmp8=index;_tmp9=_tmp8.f1;_tmpA=_tmp8.f2;{struct _tuple6 _tmp2A4;return(
_tmp2A4.f1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,unsigned int k,int v))Cyc_Dict_insert)(
_tmp9,val,_tmpA),((_tmp2A4.f2=_tmpA + 1,_tmp2A4)));}}static struct _tuple7 Cyc_Marshal_empty_addr_table();
static void _tmp2A9(unsigned int*_tmp2A8,unsigned int*_tmp2A7,unsigned int**
_tmp2A6){for(*_tmp2A8=0;*_tmp2A8 < *_tmp2A7;(*_tmp2A8)++){(*_tmp2A6)[*_tmp2A8]=0;}}
static struct _tuple7 Cyc_Marshal_empty_addr_table(){unsigned int _tmp2AE;
unsigned int*_tmp2AD;unsigned int _tmp2AC;unsigned int _tmp2AB;struct _tuple7
_tmp2AA;return(_tmp2AA.f1=_tag_dyneither(((_tmp2AE=(unsigned int)1,((_tmp2AD=(
unsigned int*)_cycalloc_atomic(_check_times(sizeof(unsigned int),_tmp2AE)),((((
_tmp2AC=_tmp2AE,_tmp2A9(& _tmp2AB,& _tmp2AC,& _tmp2AD))),_tmp2AD)))))),sizeof(
unsigned int),(unsigned int)1),((_tmp2AA.f2=1,_tmp2AA)));}static int Cyc_Marshal_table_member(
struct _tuple7 table,unsigned int m);inline static int Cyc_Marshal_table_member(struct
_tuple7 table,unsigned int m){int _tmp11;struct _tuple7 _tmp10=table;_tmp11=_tmp10.f2;
return m < _tmp11;}static struct _tuple7 Cyc_Marshal_table_insert(struct _tuple7 tbl,
unsigned int i);static void _tmp2B4(unsigned int*_tmp2B3,unsigned int*_tmp2B2,
unsigned int**_tmp2B0,int*_tmp14,struct _dyneither_ptr*_tmp13){for(*_tmp2B3=0;*
_tmp2B3 < *_tmp2B2;(*_tmp2B3)++){(*_tmp2B0)[*_tmp2B3]=*_tmp2B3 < *_tmp14?*((
unsigned int*)_check_dyneither_subscript(*_tmp13,sizeof(unsigned int),(int)*
_tmp2B3)): 0;}}static struct _tuple7 Cyc_Marshal_table_insert(struct _tuple7 tbl,
unsigned int i){struct _dyneither_ptr _tmp13;int _tmp14;struct _tuple7 _tmp12=tbl;
_tmp13=_tmp12.f1;_tmp14=_tmp12.f2;if(_get_dyneither_size(_tmp13,sizeof(
unsigned int))== _tmp14){unsigned int _tmp2B3;unsigned int _tmp2B2;struct
_dyneither_ptr _tmp2B1;unsigned int*_tmp2B0;unsigned int _tmp2AF;_tmp13=((_tmp2AF=(
unsigned int)(2 * _tmp14),((_tmp2B0=(unsigned int*)_cycalloc_atomic(_check_times(
sizeof(unsigned int),_tmp2AF)),((_tmp2B1=_tag_dyneither(_tmp2B0,sizeof(
unsigned int),_tmp2AF),((((_tmp2B2=_tmp2AF,_tmp2B4(& _tmp2B3,& _tmp2B2,& _tmp2B0,&
_tmp14,& _tmp13))),_tmp2B1))))))));}*((unsigned int*)_check_dyneither_subscript(
_tmp13,sizeof(unsigned int),_tmp14))=i;{struct _tuple7 _tmp2B5;return(_tmp2B5.f1=
_tmp13,((_tmp2B5.f2=_tmp14 + 1,_tmp2B5)));}}static unsigned int Cyc_Marshal_table_lookup(
struct _tuple7 tbl,int i);static unsigned int Cyc_Marshal_table_lookup(struct _tuple7
tbl,int i){return*((unsigned int*)_check_dyneither_subscript(tbl.f1,sizeof(
unsigned int),i));}static void Cyc_Marshal_fprint_ref(struct Cyc___cycFILE*fp,
unsigned int ref);static void Cyc_Marshal_fprint_ref(struct Cyc___cycFILE*fp,
unsigned int ref){if(ref == 0){const char*_tmp2B8;void*_tmp2B7;(_tmp2B7=0,Cyc_fprintf(
fp,((_tmp2B8="NULL",_tag_dyneither(_tmp2B8,sizeof(char),5))),_tag_dyneither(
_tmp2B7,sizeof(void*),0)));}else{const char*_tmp2BC;void*_tmp2BB[1];struct Cyc_Int_pa_struct
_tmp2BA;(_tmp2BA.tag=1,((_tmp2BA.f1=ref,((_tmp2BB[0]=& _tmp2BA,Cyc_fprintf(fp,((
_tmp2BC="(#%x)",_tag_dyneither(_tmp2BC,sizeof(char),6))),_tag_dyneither(_tmp2BB,
sizeof(void*),1)))))));}}struct _tuple9{unsigned int f1;unsigned int f2;
unsigned int f3;};struct Cyc_Set_Set*Cyc_Marshal_fprint_type_base(void*rep,struct
Cyc_Set_Set*env,struct Cyc___cycFILE*fp,void*val);struct Cyc_Set_Set*Cyc_Marshal_fprint_type_base(
void*rep,struct Cyc_Set_Set*env,struct Cyc___cycFILE*fp,void*val){{void*_tmp1F=rep;
int _tmp20;unsigned int _tmp21;unsigned int _tmp22;void*_tmp23;void*_tmp24;struct
_dyneither_ptr*_tmp25;struct _dyneither_ptr _tmp26;struct _dyneither_ptr _tmp27;
struct _dyneither_ptr _tmp28;struct _dyneither_ptr _tmp29;struct _dyneither_ptr _tmp2A;
struct _dyneither_ptr _tmp2B;struct _dyneither_ptr _tmp2C;struct _dyneither_ptr _tmp2D;
struct _dyneither_ptr _tmp2E;struct _dyneither_ptr _tmp2F;struct _dyneither_ptr*
_tmp30;struct _dyneither_ptr _tmp31;int _tmp32;struct _dyneither_ptr _tmp33;_LL1: if(
_tmp1F <= (void*)2)goto _LL3;if(*((int*)_tmp1F)!= 0)goto _LL3;_tmp20=((struct Cyc_Typerep_Int_struct*)
_tmp1F)->f1;_tmp21=((struct Cyc_Typerep_Int_struct*)_tmp1F)->f2;_LL2: switch(
_tmp21){case 8: _LL19: {char*x=((char*(*)(void*x))unsafe_cast)(val);if(isprint((
int)*x)){const char*_tmp2C0;void*_tmp2BF[1];struct Cyc_Int_pa_struct _tmp2BE;(
_tmp2BE.tag=1,((_tmp2BE.f1=(unsigned long)((int)*x),((_tmp2BF[0]=& _tmp2BE,Cyc_fprintf(
fp,((_tmp2C0="'%c'",_tag_dyneither(_tmp2C0,sizeof(char),5))),_tag_dyneither(
_tmp2BF,sizeof(void*),1)))))));}else{const char*_tmp2C4;void*_tmp2C3[1];struct Cyc_Int_pa_struct
_tmp2C2;(_tmp2C2.tag=1,((_tmp2C2.f1=(unsigned long)((int)*x),((_tmp2C3[0]=&
_tmp2C2,Cyc_fprintf(fp,((_tmp2C4="'\\%d'",_tag_dyneither(_tmp2C4,sizeof(char),6))),
_tag_dyneither(_tmp2C3,sizeof(void*),1)))))));}break;}case 16: _LL1A: {short*x=((
short*(*)(void*x))unsafe_cast)(val);{const char*_tmp2CA;const char*_tmp2C9;void*
_tmp2C8[1];struct Cyc_Int_pa_struct _tmp2C7;(_tmp2C7.tag=1,((_tmp2C7.f1=(
unsigned long)((int)*x),((_tmp2C8[0]=& _tmp2C7,Cyc_fprintf(fp,_tmp20?(_tmp2C9="%d",
_tag_dyneither(_tmp2C9,sizeof(char),3)):((_tmp2CA="%u",_tag_dyneither(_tmp2CA,
sizeof(char),3))),_tag_dyneither(_tmp2C8,sizeof(void*),1)))))));}break;}case 32:
_LL1B: {int*x=((int*(*)(void*x))unsafe_cast)(val);{const char*_tmp2D0;const char*
_tmp2CF;void*_tmp2CE[1];struct Cyc_Int_pa_struct _tmp2CD;(_tmp2CD.tag=1,((_tmp2CD.f1=(
unsigned long)*x,((_tmp2CE[0]=& _tmp2CD,Cyc_fprintf(fp,_tmp20?(_tmp2CF="%d",
_tag_dyneither(_tmp2CF,sizeof(char),3)):((_tmp2D0="%u",_tag_dyneither(_tmp2D0,
sizeof(char),3))),_tag_dyneither(_tmp2CE,sizeof(void*),1)))))));}break;}default:
_LL1C:{const char*_tmp2D4;void*_tmp2D3[1];struct Cyc_Int_pa_struct _tmp2D2;(_tmp2D2.tag=
1,((_tmp2D2.f1=(unsigned long)((int)_tmp21),((_tmp2D3[0]=& _tmp2D2,Cyc_fprintf(
Cyc_stderr,((_tmp2D4="fprint_type_base: illegal int size %d bits\n",
_tag_dyneither(_tmp2D4,sizeof(char),44))),_tag_dyneither(_tmp2D3,sizeof(void*),1)))))));}
exit(1);}goto _LL0;_LL3: if((int)_tmp1F != 0)goto _LL5;_LL4: {float*x=((float*(*)(
void*x))unsafe_cast)(val);{const char*_tmp2D8;void*_tmp2D7[1];struct Cyc_Double_pa_struct
_tmp2D6;(_tmp2D6.tag=2,((_tmp2D6.f1=(double)*x,((_tmp2D7[0]=& _tmp2D6,Cyc_fprintf(
fp,((_tmp2D8="%f",_tag_dyneither(_tmp2D8,sizeof(char),3))),_tag_dyneither(
_tmp2D7,sizeof(void*),1)))))));}goto _LL0;}_LL5: if((int)_tmp1F != 1)goto _LL7;_LL6: {
double*x=((double*(*)(void*x))unsafe_cast)(val);{const char*_tmp2DC;void*_tmp2DB[
1];struct Cyc_Double_pa_struct _tmp2DA;(_tmp2DA.tag=2,((_tmp2DA.f1=*x,((_tmp2DB[0]=&
_tmp2DA,Cyc_fprintf(fp,((_tmp2DC="%f",_tag_dyneither(_tmp2DC,sizeof(char),3))),
_tag_dyneither(_tmp2DB,sizeof(void*),1)))))));}goto _LL0;}_LL7: if(_tmp1F <= (void*)
2)goto _LL9;if(*((int*)_tmp1F)!= 1)goto _LL9;_tmp22=((struct Cyc_Typerep_ThinPtr_struct*)
_tmp1F)->f1;_tmp23=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp1F)->f2;_LL8: {
int*x=((int*(*)(void*x))unsafe_cast)(val);int y=*((int*)_check_null(x));struct
_tuple8 _tmp4C;int _tmp4D;struct Cyc_Set_Set*_tmp4E;struct _tuple8*_tmp4B=Cyc_Marshal_member_insert(
env,(unsigned int)y);_tmp4C=*_tmp4B;_tmp4D=_tmp4C.f1;_tmp4E=_tmp4C.f2;env=_tmp4E;
if(!_tmp4D){{const char*_tmp2E0;void*_tmp2DF[1];struct Cyc_Int_pa_struct _tmp2DE;(
_tmp2DE.tag=1,((_tmp2DE.f1=(unsigned int)y,((_tmp2DF[0]=& _tmp2DE,Cyc_fprintf(fp,((
_tmp2E0="{(@%x) ",_tag_dyneither(_tmp2E0,sizeof(char),8))),_tag_dyneither(
_tmp2DF,sizeof(void*),1)))))));}if(y != 0){int typ_szb=(int)Cyc_Typerep_size_type(
_tmp23);int i=0;for(0;i < _tmp22;++ i){env=Cyc_Marshal_fprint_type_base(_tmp23,env,
fp,((void*(*)(int x))unsafe_cast)(y));if(i != _tmp22 - 1){const char*_tmp2E3;void*
_tmp2E2;(_tmp2E2=0,Cyc_fprintf(fp,((_tmp2E3=", ",_tag_dyneither(_tmp2E3,sizeof(
char),3))),_tag_dyneither(_tmp2E2,sizeof(void*),0)));}y +=typ_szb;}}else{const
char*_tmp2E6;void*_tmp2E5;(_tmp2E5=0,Cyc_fprintf(fp,((_tmp2E6="NULL",
_tag_dyneither(_tmp2E6,sizeof(char),5))),_tag_dyneither(_tmp2E5,sizeof(void*),0)));}{
const char*_tmp2E9;void*_tmp2E8;(_tmp2E8=0,Cyc_fprintf(fp,((_tmp2E9=" }",
_tag_dyneither(_tmp2E9,sizeof(char),3))),_tag_dyneither(_tmp2E8,sizeof(void*),0)));}}
else{Cyc_Marshal_fprint_ref(fp,(unsigned int)y);}goto _LL0;}_LL9: if(_tmp1F <= (
void*)2)goto _LLB;if(*((int*)_tmp1F)!= 2)goto _LLB;_tmp24=(void*)((struct Cyc_Typerep_FatPtr_struct*)
_tmp1F)->f1;_LLA: {struct _tuple9*x=((struct _tuple9*(*)(void*x))unsafe_cast)(val);
unsigned int typ_szb=Cyc_Typerep_size_type(_tmp24);unsigned int base=(*x).f1;
unsigned int curr=(*x).f2;unsigned int last_plus_one=(*x).f3;struct _tuple8 _tmp59;
int _tmp5A;struct Cyc_Set_Set*_tmp5B;struct _tuple8*_tmp58=Cyc_Marshal_member_insert(
env,base);_tmp59=*_tmp58;_tmp5A=_tmp59.f1;_tmp5B=_tmp59.f2;env=_tmp5B;if(!_tmp5A){{
const char*_tmp2ED;void*_tmp2EC[1];struct Cyc_Int_pa_struct _tmp2EB;(_tmp2EB.tag=1,((
_tmp2EB.f1=base,((_tmp2EC[0]=& _tmp2EB,Cyc_fprintf(fp,((_tmp2ED="[(@%x) ",
_tag_dyneither(_tmp2ED,sizeof(char),8))),_tag_dyneither(_tmp2EC,sizeof(void*),1)))))));}{
unsigned int i=base;for(0;i < last_plus_one;i +=typ_szb){env=Cyc_Marshal_fprint_type_base(
_tmp24,env,fp,((void*(*)(unsigned int x))unsafe_cast)(i));if(i + typ_szb < 
last_plus_one){const char*_tmp2F0;void*_tmp2EF;(_tmp2EF=0,Cyc_fprintf(fp,((
_tmp2F0=", ",_tag_dyneither(_tmp2F0,sizeof(char),3))),_tag_dyneither(_tmp2EF,
sizeof(void*),0)));}}}{const char*_tmp2F3;void*_tmp2F2;(_tmp2F2=0,Cyc_fprintf(fp,((
_tmp2F3=" ]",_tag_dyneither(_tmp2F3,sizeof(char),3))),_tag_dyneither(_tmp2F2,
sizeof(void*),0)));}}else{Cyc_Marshal_fprint_ref(fp,base);}goto _LL0;}_LLB: if(
_tmp1F <= (void*)2)goto _LLD;if(*((int*)_tmp1F)!= 3)goto _LLD;_tmp25=((struct Cyc_Typerep_Struct_struct*)
_tmp1F)->f1;_tmp26=((struct Cyc_Typerep_Struct_struct*)_tmp1F)->f3;_LLC: if(_tmp25
== 0){const char*_tmp2F6;void*_tmp2F5;(_tmp2F5=0,Cyc_fprintf(fp,((_tmp2F6="{ ",
_tag_dyneither(_tmp2F6,sizeof(char),3))),_tag_dyneither(_tmp2F5,sizeof(void*),0)));}
else{const char*_tmp2FA;void*_tmp2F9[1];struct Cyc_String_pa_struct _tmp2F8;(
_tmp2F8.tag=0,((_tmp2F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp25),((
_tmp2F9[0]=& _tmp2F8,Cyc_fprintf(fp,((_tmp2FA="%s{ ",_tag_dyneither(_tmp2FA,
sizeof(char),5))),_tag_dyneither(_tmp2F9,sizeof(void*),1)))))));}{int x=((int(*)(
void*x))unsafe_cast)(val);{int i=0;for(0;i < _get_dyneither_size(_tmp26,sizeof(
struct _tuple0*));++ i){unsigned int _tmp69;struct _dyneither_ptr _tmp6A;void*_tmp6B;
struct _tuple0 _tmp68=*(*((struct _tuple0**)_check_dyneither_subscript(_tmp26,
sizeof(struct _tuple0*),i)));_tmp69=_tmp68.f1;_tmp6A=_tmp68.f2;_tmp6B=_tmp68.f3;{
const char*_tmp2FE;void*_tmp2FD[1];struct Cyc_String_pa_struct _tmp2FC;(_tmp2FC.tag=
0,((_tmp2FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp6A),((_tmp2FD[0]=&
_tmp2FC,Cyc_fprintf(fp,((_tmp2FE="%s=",_tag_dyneither(_tmp2FE,sizeof(char),4))),
_tag_dyneither(_tmp2FD,sizeof(void*),1)))))));}{int x_ofs=(int)(x + _tmp69);env=
Cyc_Marshal_fprint_type_base(_tmp6B,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));
if(i != _get_dyneither_size(_tmp26,sizeof(struct _tuple0*))- 1){const char*_tmp301;
void*_tmp300;(_tmp300=0,Cyc_fprintf(fp,((_tmp301=", ",_tag_dyneither(_tmp301,
sizeof(char),3))),_tag_dyneither(_tmp300,sizeof(void*),0)));}}}}{const char*
_tmp304;void*_tmp303;(_tmp303=0,Cyc_fprintf(fp,((_tmp304=" }",_tag_dyneither(
_tmp304,sizeof(char),3))),_tag_dyneither(_tmp303,sizeof(void*),0)));}goto _LL0;}
_LLD: if(_tmp1F <= (void*)2)goto _LLF;if(*((int*)_tmp1F)!= 4)goto _LLF;_tmp27=((
struct Cyc_Typerep_Tuple_struct*)_tmp1F)->f2;_LLE:{const char*_tmp307;void*_tmp306;(
_tmp306=0,Cyc_fprintf(fp,((_tmp307="$( ",_tag_dyneither(_tmp307,sizeof(char),4))),
_tag_dyneither(_tmp306,sizeof(void*),0)));}{int x=((int(*)(void*x))unsafe_cast)(
val);{int i=0;for(0;i < _get_dyneither_size(_tmp27,sizeof(struct _tuple1*));++ i){
unsigned int _tmp76;void*_tmp77;struct _tuple1 _tmp75=*(*((struct _tuple1**)
_check_dyneither_subscript(_tmp27,sizeof(struct _tuple1*),i)));_tmp76=_tmp75.f1;
_tmp77=_tmp75.f2;{int x_ofs=(int)(x + _tmp76);env=Cyc_Marshal_fprint_type_base(
_tmp77,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));if(i != _get_dyneither_size(
_tmp27,sizeof(struct _tuple1*))- 1){const char*_tmp30A;void*_tmp309;(_tmp309=0,Cyc_fprintf(
fp,((_tmp30A=", ",_tag_dyneither(_tmp30A,sizeof(char),3))),_tag_dyneither(
_tmp309,sizeof(void*),0)));}}}}{const char*_tmp30D;void*_tmp30C;(_tmp30C=0,Cyc_fprintf(
fp,((_tmp30D=" )",_tag_dyneither(_tmp30D,sizeof(char),3))),_tag_dyneither(
_tmp30C,sizeof(void*),0)));}goto _LL0;}_LLF: if(_tmp1F <= (void*)2)goto _LL11;if(*((
int*)_tmp1F)!= 6)goto _LL11;_tmp28=((struct Cyc_Typerep_TUnionField_struct*)_tmp1F)->f1;
_tmp29=((struct Cyc_Typerep_TUnionField_struct*)_tmp1F)->f2;_tmp2A=((struct Cyc_Typerep_TUnionField_struct*)
_tmp1F)->f4;_LL10:{const char*_tmp312;void*_tmp311[2];struct Cyc_String_pa_struct
_tmp310;struct Cyc_String_pa_struct _tmp30F;(_tmp30F.tag=0,((_tmp30F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp29),((_tmp310.tag=0,((_tmp310.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp28),((_tmp311[0]=& _tmp310,((
_tmp311[1]=& _tmp30F,Cyc_fprintf(fp,((_tmp312="%s.%s(",_tag_dyneither(_tmp312,
sizeof(char),7))),_tag_dyneither(_tmp311,sizeof(void*),2)))))))))))));}{int x=((
int(*)(void*x))unsafe_cast)(val);{int i=1;for(0;i < _get_dyneither_size(_tmp2A,
sizeof(struct _tuple1*));++ i){unsigned int _tmp81;void*_tmp82;struct _tuple1 _tmp80=*(*((
struct _tuple1**)_check_dyneither_subscript(_tmp2A,sizeof(struct _tuple1*),i)));
_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;{int x_ofs=(int)(x + _tmp81);env=Cyc_Marshal_fprint_type_base(
_tmp82,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));if(i != _get_dyneither_size(
_tmp2A,sizeof(struct _tuple1*))- 1){const char*_tmp315;void*_tmp314;(_tmp314=0,Cyc_fprintf(
fp,((_tmp315=", ",_tag_dyneither(_tmp315,sizeof(char),3))),_tag_dyneither(
_tmp314,sizeof(void*),0)));}}}}{const char*_tmp318;void*_tmp317;(_tmp317=0,Cyc_fprintf(
fp,((_tmp318=" )",_tag_dyneither(_tmp318,sizeof(char),3))),_tag_dyneither(
_tmp317,sizeof(void*),0)));}goto _LL0;}_LL11: if(_tmp1F <= (void*)2)goto _LL13;if(*((
int*)_tmp1F)!= 5)goto _LL13;_tmp2B=((struct Cyc_Typerep_TUnion_struct*)_tmp1F)->f1;
_tmp2C=((struct Cyc_Typerep_TUnion_struct*)_tmp1F)->f2;_tmp2D=((struct Cyc_Typerep_TUnion_struct*)
_tmp1F)->f3;_LL12: {unsigned int*tagp=((unsigned int*(*)(void*x))unsafe_cast)(
val);if(*tagp < 1024){const char*_tmp31C;void*_tmp31B[1];struct Cyc_String_pa_struct
_tmp31A;(_tmp31A.tag=0,((_tmp31A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Typerep_get_tagname(*tagp,_tmp2C)),((_tmp31B[0]=& _tmp31A,Cyc_fprintf(fp,((
_tmp31C="%s",_tag_dyneither(_tmp31C,sizeof(char),3))),_tag_dyneither(_tmp31B,
sizeof(void*),1)))))));}else{struct _tuple8 _tmp8B;int _tmp8C;struct Cyc_Set_Set*
_tmp8D;struct _tuple8*_tmp8A=Cyc_Marshal_member_insert(env,*tagp);_tmp8B=*_tmp8A;
_tmp8C=_tmp8B.f1;_tmp8D=_tmp8B.f2;env=_tmp8D;if(!_tmp8C){{const char*_tmp320;void*
_tmp31F[1];struct Cyc_Int_pa_struct _tmp31E;(_tmp31E.tag=1,((_tmp31E.f1=*tagp,((
_tmp31F[0]=& _tmp31E,Cyc_fprintf(fp,((_tmp320="(@%x)",_tag_dyneither(_tmp320,
sizeof(char),6))),_tag_dyneither(_tmp31F,sizeof(void*),1)))))));}{unsigned int*x=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*tagp);unsigned int tag=*x;int _tmp92;
void*_tmp93;struct _tuple4 _tmp91=Cyc_Typerep_get_unionbranch(tag,_tmp2D);_tmp92=
_tmp91.f1;_tmp93=_tmp91.f2;{void*xoff=((void*(*)(unsigned int x))unsafe_cast)(*
tagp + _tmp92);{const char*_tmp324;void*_tmp323[1];struct Cyc_String_pa_struct
_tmp322;(_tmp322.tag=0,((_tmp322.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Typerep_get_tagname2(tag,_tmp2D)),((_tmp323[0]=& _tmp322,Cyc_fprintf(fp,((
_tmp324="&%s[",_tag_dyneither(_tmp324,sizeof(char),5))),_tag_dyneither(_tmp323,
sizeof(void*),1)))))));}env=Cyc_Marshal_fprint_type_base(_tmp93,env,fp,xoff);{
const char*_tmp327;void*_tmp326;(_tmp326=0,Cyc_fprintf(fp,((_tmp327="]",
_tag_dyneither(_tmp327,sizeof(char),2))),_tag_dyneither(_tmp326,sizeof(void*),0)));}}}}
else{Cyc_Marshal_fprint_ref(fp,*tagp);}}goto _LL0;}_LL13: if(_tmp1F <= (void*)2)
goto _LL15;if(*((int*)_tmp1F)!= 7)goto _LL15;_tmp2E=((struct Cyc_Typerep_XTUnion_struct*)
_tmp1F)->f1;_tmp2F=((struct Cyc_Typerep_XTUnion_struct*)_tmp1F)->f2;_LL14: {
unsigned int*xtunionp=((unsigned int*(*)(void*x))unsafe_cast)(val);struct _tuple8
_tmp9A;int _tmp9B;struct Cyc_Set_Set*_tmp9C;struct _tuple8*_tmp99=Cyc_Marshal_member_insert(
env,*xtunionp);_tmp9A=*_tmp99;_tmp9B=_tmp9A.f1;_tmp9C=_tmp9A.f2;env=_tmp9C;if(!
_tmp9B){{const char*_tmp32B;void*_tmp32A[1];struct Cyc_Int_pa_struct _tmp329;(
_tmp329.tag=1,((_tmp329.f1=*xtunionp,((_tmp32A[0]=& _tmp329,Cyc_fprintf(fp,((
_tmp32B="(@%x)",_tag_dyneither(_tmp32B,sizeof(char),6))),_tag_dyneither(_tmp32A,
sizeof(void*),1)))))));}{unsigned int*xtstructp=((unsigned int*(*)(unsigned int x))
unsafe_cast)(*xtunionp);if(*xtstructp != 0){char*xtnamec=((char*(*)(unsigned int x))
unsafe_cast)(*xtstructp + 4);struct _dyneither_ptr xtname=(struct _dyneither_ptr)
Cstring_to_string(xtnamec);{const char*_tmp32F;void*_tmp32E[1];struct Cyc_String_pa_struct
_tmp32D;(_tmp32D.tag=0,((_tmp32D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
xtname),((_tmp32E[0]=& _tmp32D,Cyc_fprintf(fp,((_tmp32F="&%s[( ",_tag_dyneither(
_tmp32F,sizeof(char),7))),_tag_dyneither(_tmp32E,sizeof(void*),1)))))));}{int
_tmpA4;void*_tmpA5;struct _tuple4 _tmpA3=Cyc_Typerep_get_xtunionbranch(xtname,
_tmp2F);_tmpA4=_tmpA3.f1;_tmpA5=_tmpA3.f2;{void*xoff=((void*(*)(unsigned int x))
unsafe_cast)(*xtunionp + _tmpA4);env=Cyc_Marshal_fprint_type_base(_tmpA5,env,fp,
xoff);{const char*_tmp332;void*_tmp331;(_tmp331=0,Cyc_fprintf(fp,((_tmp332=" )]",
_tag_dyneither(_tmp332,sizeof(char),4))),_tag_dyneither(_tmp331,sizeof(void*),0)));}}}}
else{char*xtnamec=((char*(*)(unsigned int x))unsafe_cast)(*xtunionp + 4);struct
_dyneither_ptr xtname=(struct _dyneither_ptr)Cstring_to_string(xtnamec);const char*
_tmp336;void*_tmp335[1];struct Cyc_String_pa_struct _tmp334;(_tmp334.tag=0,((
_tmp334.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)xtname),((_tmp335[0]=&
_tmp334,Cyc_fprintf(fp,((_tmp336="%s",_tag_dyneither(_tmp336,sizeof(char),3))),
_tag_dyneither(_tmp335,sizeof(void*),1)))))));}}}else{Cyc_Marshal_fprint_ref(fp,*
xtunionp);}goto _LL0;}_LL15: if(_tmp1F <= (void*)2)goto _LL17;if(*((int*)_tmp1F)!= 8)
goto _LL17;_tmp30=((struct Cyc_Typerep_Union_struct*)_tmp1F)->f1;_tmp31=((struct
Cyc_Typerep_Union_struct*)_tmp1F)->f3;_LL16: if(_tmp30 == 0){const char*_tmp339;
void*_tmp338;(_tmp338=0,Cyc_fprintf(fp,((_tmp339="{",_tag_dyneither(_tmp339,
sizeof(char),2))),_tag_dyneither(_tmp338,sizeof(void*),0)));}else{const char*
_tmp33D;void*_tmp33C[1];struct Cyc_String_pa_struct _tmp33B;(_tmp33B.tag=0,((
_tmp33B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp30),((_tmp33C[0]=&
_tmp33B,Cyc_fprintf(fp,((_tmp33D="%s{",_tag_dyneither(_tmp33D,sizeof(char),4))),
_tag_dyneither(_tmp33C,sizeof(void*),1)))))));}{int i=0;for(0;i < 
_get_dyneither_size(_tmp31,sizeof(struct _tuple3*));++ i){struct _dyneither_ptr
_tmpB1;void*_tmpB2;struct _tuple3 _tmpB0=*(*((struct _tuple3**)
_check_dyneither_subscript(_tmp31,sizeof(struct _tuple3*),i)));_tmpB1=_tmpB0.f1;
_tmpB2=_tmpB0.f2;{const char*_tmp341;void*_tmp340[1];struct Cyc_String_pa_struct
_tmp33F;(_tmp33F.tag=0,((_tmp33F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmpB1),((_tmp340[0]=& _tmp33F,Cyc_fprintf(fp,((_tmp341="%s=",_tag_dyneither(
_tmp341,sizeof(char),4))),_tag_dyneither(_tmp340,sizeof(void*),1)))))));}Cyc_Marshal_fprint_type_base(
_tmpB2,env,fp,val);if(i < _get_dyneither_size(_tmp31,sizeof(struct _tuple3*))- 1){
const char*_tmp344;void*_tmp343;(_tmp343=0,Cyc_fprintf(fp,((_tmp344=" | ",
_tag_dyneither(_tmp344,sizeof(char),4))),_tag_dyneither(_tmp343,sizeof(void*),0)));}}}{
const char*_tmp347;void*_tmp346;(_tmp346=0,Cyc_fprintf(fp,((_tmp347="}",
_tag_dyneither(_tmp347,sizeof(char),2))),_tag_dyneither(_tmp346,sizeof(void*),0)));}
goto _LL0;_LL17: if(_tmp1F <= (void*)2)goto _LL0;if(*((int*)_tmp1F)!= 9)goto _LL0;
_tmp32=((struct Cyc_Typerep_Enum_struct*)_tmp1F)->f2;_tmp33=((struct Cyc_Typerep_Enum_struct*)
_tmp1F)->f3;_LL18: {unsigned int*tagp=((unsigned int*(*)(void*x))unsafe_cast)(
val);{const char*_tmp34B;void*_tmp34A[1];struct Cyc_String_pa_struct _tmp349;(
_tmp349.tag=0,((_tmp349.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Typerep_get_tagname(*
tagp,_tmp33)),((_tmp34A[0]=& _tmp349,Cyc_fprintf(fp,((_tmp34B="%s",_tag_dyneither(
_tmp34B,sizeof(char),3))),_tag_dyneither(_tmp34A,sizeof(void*),1)))))));}goto
_LL0;}_LL0:;}return env;}void Cyc_Marshal_fprint_type(void*ts,struct Cyc___cycFILE*
fp,void*val);void Cyc_Marshal_fprint_type(void*ts,struct Cyc___cycFILE*fp,void*val){
struct _handler_cons _tmpBD;_push_handler(& _tmpBD);{int _tmpBF=0;if(setjmp(_tmpBD.handler))
_tmpBF=1;if(!_tmpBF){Cyc_Marshal_fprint_type_base(ts,Cyc_Marshal_empty_addr_set(),
fp,val);;_pop_handler();}else{void*_tmpBE=(void*)_exn_thrown;void*_tmpC1=_tmpBE;
struct _dyneither_ptr _tmpC2;_LL1F: if(*((void**)_tmpC1)!= Cyc_Core_Failure)goto
_LL21;_tmpC2=((struct Cyc_Core_Failure_struct*)_tmpC1)->f1;_LL20:{const char*
_tmp34F;void*_tmp34E[1];struct Cyc_String_pa_struct _tmp34D;(_tmp34D.tag=0,((
_tmp34D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpC2),((_tmp34E[0]=&
_tmp34D,Cyc_fprintf(Cyc_stderr,((_tmp34F="Core::Failure(%s)\n",_tag_dyneither(
_tmp34F,sizeof(char),19))),_tag_dyneither(_tmp34E,sizeof(void*),1)))))));}{
struct Cyc_Core_Failure_struct _tmp352;struct Cyc_Core_Failure_struct*_tmp351;(int)
_throw((void*)((_tmp351=_cycalloc(sizeof(*_tmp351)),((_tmp351[0]=((_tmp352.tag=
Cyc_Core_Failure,((_tmp352.f1=_tmpC2,_tmp352)))),_tmp351)))));}_LL21:;_LL22:(
void)_throw(_tmpC1);_LL1E:;}}}void Cyc_Marshal_print_type(void*ts,void*val);void
Cyc_Marshal_print_type(void*ts,void*val){Cyc_Marshal_fprint_type(ts,Cyc_stdout,
val);}void Cyc_Marshal_cycputchar(char x,struct Cyc___cycFILE*fp);void Cyc_Marshal_cycputchar(
char x,struct Cyc___cycFILE*fp){struct Cyc_Core_Failure_struct _tmp358;const char*
_tmp357;struct Cyc_Core_Failure_struct*_tmp356;(int)_throw((void*)((_tmp356=
_cycalloc(sizeof(*_tmp356)),((_tmp356[0]=((_tmp358.tag=Cyc_Core_Failure,((
_tmp358.f1=((_tmp357="Write failure",_tag_dyneither(_tmp357,sizeof(char),14))),
_tmp358)))),_tmp356)))));}void Cyc_Marshal_cycputshort(short x,struct Cyc___cycFILE*
fp);void Cyc_Marshal_cycputshort(short x,struct Cyc___cycFILE*fp){struct Cyc_Core_Failure_struct
_tmp35E;const char*_tmp35D;struct Cyc_Core_Failure_struct*_tmp35C;(int)_throw((
void*)((_tmp35C=_cycalloc(sizeof(*_tmp35C)),((_tmp35C[0]=((_tmp35E.tag=Cyc_Core_Failure,((
_tmp35E.f1=((_tmp35D="Write failure",_tag_dyneither(_tmp35D,sizeof(char),14))),
_tmp35E)))),_tmp35C)))));}void Cyc_Marshal_cycputint(int x,struct Cyc___cycFILE*fp);
void Cyc_Marshal_cycputint(int x,struct Cyc___cycFILE*fp){struct Cyc_Core_Failure_struct
_tmp364;const char*_tmp363;struct Cyc_Core_Failure_struct*_tmp362;(int)_throw((
void*)((_tmp362=_cycalloc(sizeof(*_tmp362)),((_tmp362[0]=((_tmp364.tag=Cyc_Core_Failure,((
_tmp364.f1=((_tmp363="Write failure",_tag_dyneither(_tmp363,sizeof(char),14))),
_tmp364)))),_tmp362)))));}void Cyc_Marshal_cycputfloat(float x,struct Cyc___cycFILE*
fp);void Cyc_Marshal_cycputfloat(float x,struct Cyc___cycFILE*fp){struct Cyc_Core_Failure_struct
_tmp36A;const char*_tmp369;struct Cyc_Core_Failure_struct*_tmp368;(int)_throw((
void*)((_tmp368=_cycalloc(sizeof(*_tmp368)),((_tmp368[0]=((_tmp36A.tag=Cyc_Core_Failure,((
_tmp36A.f1=((_tmp369="Write failure",_tag_dyneither(_tmp369,sizeof(char),14))),
_tmp36A)))),_tmp368)))));}void Cyc_Marshal_cycputdouble(double x,struct Cyc___cycFILE*
fp);void Cyc_Marshal_cycputdouble(double x,struct Cyc___cycFILE*fp){struct Cyc_Core_Failure_struct
_tmp370;const char*_tmp36F;struct Cyc_Core_Failure_struct*_tmp36E;(int)_throw((
void*)((_tmp36E=_cycalloc(sizeof(*_tmp36E)),((_tmp36E[0]=((_tmp370.tag=Cyc_Core_Failure,((
_tmp370.f1=((_tmp36F="Write failure",_tag_dyneither(_tmp36F,sizeof(char),14))),
_tmp370)))),_tmp36E)))));}void Cyc_Marshal_cycputvarint(unsigned int i,struct Cyc___cycFILE*
fp);void Cyc_Marshal_cycputvarint(unsigned int i,struct Cyc___cycFILE*fp){int width=
0;unsigned int mask=-256;while((int)mask){if((int)(i & mask))++ width;mask <<=8;}Cyc_fputc((
int)(width << 6 | i & 63),fp);i >>=6;{int j=0;for(0;j < width;++ j){Cyc_fputc((int)(i & 
255),fp);i >>=8;}}}char Cyc_Marshal_cycgetchar(struct Cyc___cycFILE*fp);static void
_tmp375(unsigned int*_tmp374,unsigned int*_tmp373,char**_tmp372){for(*_tmp374=0;*
_tmp374 < *_tmp373;(*_tmp374)++){(*_tmp372)[*_tmp374]='\000';}}char Cyc_Marshal_cycgetchar(
struct Cyc___cycFILE*fp){struct _RegionHandle _tmpD7=_new_region("r");struct
_RegionHandle*r=& _tmpD7;_push_region(r);{unsigned int _tmp374;unsigned int _tmp373;
char*_tmp372;unsigned int _tmp371;struct _dyneither_ptr x=_tag_dyneither(((_tmp371=
sizeof(char),((_tmp372=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp371)),((((
_tmp373=_tmp371,_tmp375(& _tmp374,& _tmp373,& _tmp372))),_tmp372)))))),sizeof(char),
sizeof(char));if(Cyc_fread(x,sizeof(char),1,fp)< 1){struct Cyc_Core_Failure_struct
_tmp37B;const char*_tmp37A;struct Cyc_Core_Failure_struct*_tmp379;(int)_throw((
void*)((_tmp379=_cycalloc(sizeof(*_tmp379)),((_tmp379[0]=((_tmp37B.tag=Cyc_Core_Failure,((
_tmp37B.f1=((_tmp37A="Read failure",_tag_dyneither(_tmp37A,sizeof(char),13))),
_tmp37B)))),_tmp379)))));}{char*xx=((char*(*)(char*x))unsafe_cast)((char*)
_check_null(_untag_dyneither_ptr(x,sizeof(char),1)));char _tmpDB=*xx;
_npop_handler(0);return _tmpDB;}};_pop_region(r);}short Cyc_Marshal_cycgetshort(
struct Cyc___cycFILE*fp);static void _tmp380(unsigned int*_tmp37F,unsigned int*
_tmp37E,char**_tmp37D){for(*_tmp37F=0;*_tmp37F < *_tmp37E;(*_tmp37F)++){(*_tmp37D)[*
_tmp37F]='\000';}}short Cyc_Marshal_cycgetshort(struct Cyc___cycFILE*fp){struct
_RegionHandle _tmpDF=_new_region("r");struct _RegionHandle*r=& _tmpDF;_push_region(
r);{unsigned int _tmp37F;unsigned int _tmp37E;char*_tmp37D;unsigned int _tmp37C;
struct _dyneither_ptr x=_tag_dyneither(((_tmp37C=sizeof(short),((_tmp37D=(char*)
_region_malloc(r,_check_times(sizeof(char),_tmp37C)),((((_tmp37E=_tmp37C,_tmp380(&
_tmp37F,& _tmp37E,& _tmp37D))),_tmp37D)))))),sizeof(char),sizeof(short));if(Cyc_fread(
x,sizeof(short),1,fp)< 1){struct Cyc_Core_Failure_struct _tmp386;const char*_tmp385;
struct Cyc_Core_Failure_struct*_tmp384;(int)_throw((void*)((_tmp384=_cycalloc(
sizeof(*_tmp384)),((_tmp384[0]=((_tmp386.tag=Cyc_Core_Failure,((_tmp386.f1=((
_tmp385="Read failure",_tag_dyneither(_tmp385,sizeof(char),13))),_tmp386)))),
_tmp384)))));}{short*xx=((short*(*)(char*x))unsafe_cast)((char*)_check_null(
_untag_dyneither_ptr(x,sizeof(char),1)));short _tmpE3=*xx;_npop_handler(0);return
_tmpE3;}};_pop_region(r);}int Cyc_Marshal_cycgetint(struct Cyc___cycFILE*fp);
static void _tmp38B(unsigned int*_tmp38A,unsigned int*_tmp389,char**_tmp388){for(*
_tmp38A=0;*_tmp38A < *_tmp389;(*_tmp38A)++){(*_tmp388)[*_tmp38A]='\000';}}int Cyc_Marshal_cycgetint(
struct Cyc___cycFILE*fp){struct _RegionHandle _tmpE7=_new_region("r");struct
_RegionHandle*r=& _tmpE7;_push_region(r);{unsigned int _tmp38A;unsigned int _tmp389;
char*_tmp388;unsigned int _tmp387;struct _dyneither_ptr x=_tag_dyneither(((_tmp387=
sizeof(int),((_tmp388=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp387)),((((
_tmp389=_tmp387,_tmp38B(& _tmp38A,& _tmp389,& _tmp388))),_tmp388)))))),sizeof(char),
sizeof(int));if(Cyc_fread(x,sizeof(int),1,fp)< 1){struct Cyc_Core_Failure_struct
_tmp391;const char*_tmp390;struct Cyc_Core_Failure_struct*_tmp38F;(int)_throw((
void*)((_tmp38F=_cycalloc(sizeof(*_tmp38F)),((_tmp38F[0]=((_tmp391.tag=Cyc_Core_Failure,((
_tmp391.f1=((_tmp390="Read failure",_tag_dyneither(_tmp390,sizeof(char),13))),
_tmp391)))),_tmp38F)))));}{int*xx=((int*(*)(char*x))unsafe_cast)((char*)
_check_null(_untag_dyneither_ptr(x,sizeof(char),1)));int _tmpEB=*xx;_npop_handler(
0);return _tmpEB;}};_pop_region(r);}float Cyc_Marshal_cycgetfloat(struct Cyc___cycFILE*
fp);static void _tmp396(unsigned int*_tmp395,unsigned int*_tmp394,char**_tmp393){
for(*_tmp395=0;*_tmp395 < *_tmp394;(*_tmp395)++){(*_tmp393)[*_tmp395]='\000';}}
float Cyc_Marshal_cycgetfloat(struct Cyc___cycFILE*fp){struct _RegionHandle _tmpEF=
_new_region("r");struct _RegionHandle*r=& _tmpEF;_push_region(r);{unsigned int
_tmp395;unsigned int _tmp394;char*_tmp393;unsigned int _tmp392;struct
_dyneither_ptr x=_tag_dyneither(((_tmp392=sizeof(float),((_tmp393=(char*)
_region_malloc(r,_check_times(sizeof(char),_tmp392)),((((_tmp394=_tmp392,_tmp396(&
_tmp395,& _tmp394,& _tmp393))),_tmp393)))))),sizeof(char),sizeof(float));if(Cyc_fread(
x,sizeof(float),1,fp)< 1){struct Cyc_Core_Failure_struct _tmp39C;const char*_tmp39B;
struct Cyc_Core_Failure_struct*_tmp39A;(int)_throw((void*)((_tmp39A=_cycalloc(
sizeof(*_tmp39A)),((_tmp39A[0]=((_tmp39C.tag=Cyc_Core_Failure,((_tmp39C.f1=((
_tmp39B="Read failure",_tag_dyneither(_tmp39B,sizeof(char),13))),_tmp39C)))),
_tmp39A)))));}{float*xx=((float*(*)(char*x))unsafe_cast)((char*)_check_null(
_untag_dyneither_ptr(x,sizeof(char),1)));float _tmpF3=*xx;_npop_handler(0);return
_tmpF3;}};_pop_region(r);}double Cyc_Marshal_cycgetdouble(struct Cyc___cycFILE*fp);
static void _tmp3A1(unsigned int*_tmp3A0,unsigned int*_tmp39F,char**_tmp39E){for(*
_tmp3A0=0;*_tmp3A0 < *_tmp39F;(*_tmp3A0)++){(*_tmp39E)[*_tmp3A0]='\000';}}double
Cyc_Marshal_cycgetdouble(struct Cyc___cycFILE*fp){struct _RegionHandle _tmpF7=
_new_region("r");struct _RegionHandle*r=& _tmpF7;_push_region(r);{unsigned int
_tmp3A0;unsigned int _tmp39F;char*_tmp39E;unsigned int _tmp39D;struct
_dyneither_ptr x=_tag_dyneither(((_tmp39D=sizeof(double),((_tmp39E=(char*)
_region_malloc(r,_check_times(sizeof(char),_tmp39D)),((((_tmp39F=_tmp39D,_tmp3A1(&
_tmp3A0,& _tmp39F,& _tmp39E))),_tmp39E)))))),sizeof(char),sizeof(double));if(Cyc_fread(
x,sizeof(double),1,fp)< 1){struct Cyc_Core_Failure_struct _tmp3A7;const char*
_tmp3A6;struct Cyc_Core_Failure_struct*_tmp3A5;(int)_throw((void*)((_tmp3A5=
_cycalloc(sizeof(*_tmp3A5)),((_tmp3A5[0]=((_tmp3A7.tag=Cyc_Core_Failure,((
_tmp3A7.f1=((_tmp3A6="Read failure",_tag_dyneither(_tmp3A6,sizeof(char),13))),
_tmp3A7)))),_tmp3A5)))));}{double*xx=((double*(*)(char*x))unsafe_cast)((char*)
_check_null(_untag_dyneither_ptr(x,sizeof(char),1)));double _tmpFB=*xx;
_npop_handler(0);return _tmpFB;}};_pop_region(r);}unsigned int Cyc_Marshal_cycgetvarint(
struct Cyc___cycFILE*fp);unsigned int Cyc_Marshal_cycgetvarint(struct Cyc___cycFILE*
fp){int i=0;int j=(int)Cyc_Marshal_cycgetchar(fp);int width=(j & 192)>> 6;int pos=6;i
+=j & 63;{int k=0;for(0;k < width;++ k){j=(int)Cyc_Marshal_cycgetchar(fp);i |=j << pos;
pos +=8;}}return(unsigned int)i;}struct _tuple6 Cyc_Marshal_write_type_base(void*
rep,struct _tuple6 env,struct Cyc___cycFILE*fp,void*val);static void _tmp3BF(
unsigned int*_tmp3BE,unsigned int*_tmp3BD,struct _tuple1***_tmp3BB){for(*_tmp3BE=
0;*_tmp3BE < *_tmp3BD;(*_tmp3BE)++){struct Cyc_Typerep_Int_struct*_tmp3B9;struct
Cyc_Typerep_Int_struct _tmp3B8;struct _tuple1*_tmp3B7;(*_tmp3BB)[*_tmp3BE]=((
_tmp3B7=_cycalloc(sizeof(*_tmp3B7)),((_tmp3B7->f1=*_tmp3BE,((_tmp3B7->f2=(void*)((
_tmp3B9=_cycalloc_atomic(sizeof(*_tmp3B9)),((_tmp3B9[0]=((_tmp3B8.tag=0,((
_tmp3B8.f1=0,((_tmp3B8.f2=8,_tmp3B8)))))),_tmp3B9)))),_tmp3B7))))));}}struct
_tuple6 Cyc_Marshal_write_type_base(void*rep,struct _tuple6 env,struct Cyc___cycFILE*
fp,void*val){{void*_tmpFF=rep;unsigned int _tmp100;unsigned int _tmp101;void*
_tmp102;void*_tmp103;struct _dyneither_ptr _tmp104;struct _dyneither_ptr _tmp105;
struct _dyneither_ptr _tmp106;struct _dyneither_ptr _tmp107;struct _dyneither_ptr
_tmp108;struct _dyneither_ptr _tmp109;int _tmp10A;int _tmp10B;_LL24: if(_tmpFF <= (
void*)2)goto _LL26;if(*((int*)_tmpFF)!= 0)goto _LL26;_tmp100=((struct Cyc_Typerep_Int_struct*)
_tmpFF)->f2;_LL25: switch(_tmp100){case 8: _LL3C: {char*x=((char*(*)(void*x))
unsafe_cast)(val);Cyc_Marshal_cycputchar(*x,fp);break;}case 16: _LL3D: {short*x=((
short*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputshort(*x,fp);break;}case 32:
_LL3E: {int*x=((int*(*)(void*x))unsafe_cast)(val);Cyc_Marshal_cycputint(*x,fp);
break;}default: _LL3F:{const char*_tmp3AB;void*_tmp3AA[1];struct Cyc_Int_pa_struct
_tmp3A9;(_tmp3A9.tag=1,((_tmp3A9.f1=(unsigned long)((int)_tmp100),((_tmp3AA[0]=&
_tmp3A9,Cyc_fprintf(Cyc_stderr,((_tmp3AB="write_type_base: illegal int size %d bits\n",
_tag_dyneither(_tmp3AB,sizeof(char),43))),_tag_dyneither(_tmp3AA,sizeof(void*),1)))))));}
exit(1);}goto _LL23;_LL26: if((int)_tmpFF != 0)goto _LL28;_LL27: {float*x=((float*(*)(
void*x))unsafe_cast)(val);Cyc_Marshal_cycputfloat(*x,fp);goto _LL23;}_LL28: if((
int)_tmpFF != 1)goto _LL2A;_LL29: {double*x=((double*(*)(void*x))unsafe_cast)(val);
Cyc_Marshal_cycputdouble(*x,fp);goto _LL23;}_LL2A: if(_tmpFF <= (void*)2)goto _LL2C;
if(*((int*)_tmpFF)!= 1)goto _LL2C;_tmp101=((struct Cyc_Typerep_ThinPtr_struct*)
_tmpFF)->f1;_tmp102=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmpFF)->f2;_LL2B: {
int*x=((int*(*)(void*x))unsafe_cast)(val);int y=*((int*)_check_null(x));
unsigned int _tmp10F=Cyc_Marshal_index_lookup(env,(unsigned int)y);Cyc_Marshal_cycputvarint(
_tmp10F,fp);if(!Cyc_Marshal_index_member(env,_tmp10F)){env=Cyc_Marshal_index_insert(
env,(unsigned int)y);{int typ_szb=(int)Cyc_Typerep_size_type(_tmp102);int i=0;for(
0;i < _tmp101;++ i){env=Cyc_Marshal_write_type_base(_tmp102,env,fp,((void*(*)(int x))
unsafe_cast)(y));y +=typ_szb;}}}goto _LL23;}_LL2C: if(_tmpFF <= (void*)2)goto _LL2E;
if(*((int*)_tmpFF)!= 2)goto _LL2E;_tmp103=(void*)((struct Cyc_Typerep_FatPtr_struct*)
_tmpFF)->f1;_LL2D: {struct _tuple9*x=((struct _tuple9*(*)(void*x))unsafe_cast)(val);
unsigned int typ_szb=Cyc_Typerep_size_type(_tmp103);unsigned int base=(*x).f1;
unsigned int curr=(*x).f2;unsigned int last_plus_one=(*x).f3;unsigned int pos=(curr
- base)/ typ_szb;unsigned int sz=(last_plus_one - base)/ typ_szb;unsigned int
_tmp110=Cyc_Marshal_index_lookup(env,base);Cyc_Marshal_cycputvarint(_tmp110,fp);
Cyc_Marshal_cycputvarint(pos,fp);Cyc_Marshal_cycputvarint(sz,fp);if(!Cyc_Marshal_index_member(
env,_tmp110)){env=Cyc_Marshal_index_insert(env,base);{unsigned int i=base;for(0;i
< last_plus_one;i +=typ_szb){env=Cyc_Marshal_write_type_base(_tmp103,env,fp,((
void*(*)(unsigned int x))unsafe_cast)(i));}}}goto _LL23;}_LL2E: if(_tmpFF <= (void*)
2)goto _LL30;if(*((int*)_tmpFF)!= 3)goto _LL30;_tmp104=((struct Cyc_Typerep_Struct_struct*)
_tmpFF)->f3;_LL2F: {int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < 
_get_dyneither_size(_tmp104,sizeof(struct _tuple0*));++ i){unsigned int _tmp112;
void*_tmp113;struct _tuple0 _tmp111=*(*((struct _tuple0**)
_check_dyneither_subscript(_tmp104,sizeof(struct _tuple0*),i)));_tmp112=_tmp111.f1;
_tmp113=_tmp111.f3;{int x_ofs=(int)(x + _tmp112);env=Cyc_Marshal_write_type_base(
_tmp113,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL23;}_LL30: if(
_tmpFF <= (void*)2)goto _LL32;if(*((int*)_tmpFF)!= 4)goto _LL32;_tmp105=((struct Cyc_Typerep_Tuple_struct*)
_tmpFF)->f2;_LL31: {int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < 
_get_dyneither_size(_tmp105,sizeof(struct _tuple1*));++ i){unsigned int _tmp115;
void*_tmp116;struct _tuple1 _tmp114=*(*((struct _tuple1**)
_check_dyneither_subscript(_tmp105,sizeof(struct _tuple1*),i)));_tmp115=_tmp114.f1;
_tmp116=_tmp114.f2;{int x_ofs=(int)(x + _tmp115);env=Cyc_Marshal_write_type_base(
_tmp116,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL23;}_LL32: if(
_tmpFF <= (void*)2)goto _LL34;if(*((int*)_tmpFF)!= 6)goto _LL34;_tmp106=((struct Cyc_Typerep_TUnionField_struct*)
_tmpFF)->f4;_LL33: {int x=((int(*)(void*x))unsafe_cast)(val);{int i=0;for(0;i < 
_get_dyneither_size(_tmp106,sizeof(struct _tuple1*));++ i){unsigned int _tmp118;
void*_tmp119;struct _tuple1 _tmp117=*(*((struct _tuple1**)
_check_dyneither_subscript(_tmp106,sizeof(struct _tuple1*),i)));_tmp118=_tmp117.f1;
_tmp119=_tmp117.f2;{int x_ofs=(int)(x + _tmp118);env=Cyc_Marshal_write_type_base(
_tmp119,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL23;}_LL34: if(
_tmpFF <= (void*)2)goto _LL36;if(*((int*)_tmpFF)!= 5)goto _LL36;_tmp107=((struct Cyc_Typerep_TUnion_struct*)
_tmpFF)->f2;_tmp108=((struct Cyc_Typerep_TUnion_struct*)_tmpFF)->f3;_LL35: {
unsigned int*tagp=((unsigned int*(*)(void*x))unsafe_cast)(val);if(*tagp < 1024)
Cyc_Marshal_cycputvarint(*tagp,fp);else{unsigned int _tmp11A=Cyc_Marshal_index_lookup(
env,*tagp);Cyc_Marshal_cycputvarint(_tmp11A + 1024,fp);if(!Cyc_Marshal_index_member(
env,_tmp11A)){env=Cyc_Marshal_index_insert(env,*tagp);{unsigned int*x=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*tagp);unsigned int tag=*x;Cyc_Marshal_cycputvarint(
tag,fp);{int _tmp11C;void*_tmp11D;struct _tuple4 _tmp11B=Cyc_Typerep_get_unionbranch(
tag,_tmp108);_tmp11C=_tmp11B.f1;_tmp11D=_tmp11B.f2;{void*xoff=((void*(*)(
unsigned int x))unsafe_cast)(*tagp + _tmp11C);env=Cyc_Marshal_write_type_base(
_tmp11D,env,fp,xoff);}}}}}goto _LL23;}_LL36: if(_tmpFF <= (void*)2)goto _LL38;if(*((
int*)_tmpFF)!= 7)goto _LL38;_tmp109=((struct Cyc_Typerep_XTUnion_struct*)_tmpFF)->f2;
_LL37: {unsigned int*xtunionp=((unsigned int*(*)(void*x))unsafe_cast)(val);
unsigned int _tmp11E=Cyc_Marshal_index_lookup(env,*xtunionp);Cyc_Marshal_cycputvarint(
_tmp11E,fp);if(!Cyc_Marshal_index_member(env,_tmp11E)){env=Cyc_Marshal_index_insert(
env,*xtunionp);{unsigned int*xtstructp=((unsigned int*(*)(unsigned int x))
unsafe_cast)(*xtunionp);if(*xtstructp != 0){Cyc_Marshal_cycputchar('\001',fp);{
char*xtnamec=((char*(*)(unsigned int x))unsafe_cast)(*xtstructp + 4);struct
_dyneither_ptr xtname=(struct _dyneither_ptr)Cstring_to_string(xtnamec);{const char*
_tmp3AF;void*_tmp3AE[1];struct Cyc_String_pa_struct _tmp3AD;(_tmp3AD.tag=0,((
_tmp3AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)xtname),((_tmp3AE[0]=&
_tmp3AD,Cyc_fprintf(fp,((_tmp3AF="%s",_tag_dyneither(_tmp3AF,sizeof(char),3))),
_tag_dyneither(_tmp3AE,sizeof(void*),1)))))));}Cyc_Marshal_cycputchar('\000',fp);{
int _tmp123;void*_tmp124;struct _tuple4 _tmp122=Cyc_Typerep_get_xtunionbranch(
xtname,_tmp109);_tmp123=_tmp122.f1;_tmp124=_tmp122.f2;{void*xoff=((void*(*)(
unsigned int x))unsafe_cast)(*xtunionp + _tmp123);env=Cyc_Marshal_write_type_base(
_tmp124,env,fp,xoff);}}}}else{Cyc_Marshal_cycputchar('\000',fp);{char*xtnamec=((
char*(*)(unsigned int x))unsafe_cast)(*xtunionp + 4);struct _dyneither_ptr xtname=(
struct _dyneither_ptr)Cstring_to_string(xtnamec);{const char*_tmp3B3;void*_tmp3B2[
1];struct Cyc_String_pa_struct _tmp3B1;(_tmp3B1.tag=0,((_tmp3B1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)xtname),((_tmp3B2[0]=& _tmp3B1,Cyc_fprintf(
fp,((_tmp3B3="%s",_tag_dyneither(_tmp3B3,sizeof(char),3))),_tag_dyneither(
_tmp3B2,sizeof(void*),1)))))));}Cyc_Marshal_cycputchar('\000',fp);}}}}goto _LL23;}
_LL38: if(_tmpFF <= (void*)2)goto _LL3A;if(*((int*)_tmpFF)!= 8)goto _LL3A;_tmp10A=((
struct Cyc_Typerep_Union_struct*)_tmpFF)->f2;_LL39:{struct Cyc_Typerep_Tuple_struct
_tmp3CC;unsigned int _tmp3CB;unsigned int _tmp3CA;struct _dyneither_ptr _tmp3C9;
struct _tuple1**_tmp3C8;unsigned int _tmp3C7;struct Cyc_Typerep_Tuple_struct*
_tmp3C6;env=Cyc_Marshal_write_type_base((void*)((_tmp3C6=_cycalloc(sizeof(*
_tmp3C6)),((_tmp3C6[0]=((_tmp3CC.tag=4,((_tmp3CC.f1=(unsigned int)_tmp10A,((
_tmp3CC.f2=((_tmp3C7=(unsigned int)_tmp10A,((_tmp3C8=(struct _tuple1**)_cycalloc(
_check_times(sizeof(struct _tuple1*),_tmp3C7)),((_tmp3C9=_tag_dyneither(_tmp3C8,
sizeof(struct _tuple1*),_tmp3C7),((((_tmp3CA=_tmp3C7,_tmp3BF(& _tmp3CB,& _tmp3CA,&
_tmp3C8))),_tmp3C9)))))))),_tmp3CC)))))),_tmp3C6)))),env,fp,val);}goto _LL23;
_LL3A: if(_tmpFF <= (void*)2)goto _LL23;if(*((int*)_tmpFF)!= 9)goto _LL23;_tmp10B=((
struct Cyc_Typerep_Enum_struct*)_tmpFF)->f2;_LL3B: {struct Cyc_Typerep_Int_struct
_tmp3CF;struct Cyc_Typerep_Int_struct*_tmp3CE;env=Cyc_Marshal_write_type_base((
void*)((_tmp3CE=_cycalloc_atomic(sizeof(*_tmp3CE)),((_tmp3CE[0]=((_tmp3CF.tag=0,((
_tmp3CF.f1=0,((_tmp3CF.f2=(unsigned int)(_tmp10B << 3),_tmp3CF)))))),_tmp3CE)))),
env,fp,val);}_LL23:;}return env;}void Cyc_Marshal_write_type(void*ts,struct Cyc___cycFILE*
fp,void*val);void Cyc_Marshal_write_type(void*ts,struct Cyc___cycFILE*fp,void*val){
struct _handler_cons _tmp133;_push_handler(& _tmp133);{int _tmp135=0;if(setjmp(
_tmp133.handler))_tmp135=1;if(!_tmp135){Cyc_Marshal_write_type_base(ts,Cyc_Marshal_empty_addr_index(),
fp,val);;_pop_handler();}else{void*_tmp134=(void*)_exn_thrown;void*_tmp137=
_tmp134;struct _dyneither_ptr _tmp138;_LL42: if(*((void**)_tmp137)!= Cyc_Core_Failure)
goto _LL44;_tmp138=((struct Cyc_Core_Failure_struct*)_tmp137)->f1;_LL43:{const char*
_tmp3D3;void*_tmp3D2[1];struct Cyc_String_pa_struct _tmp3D1;(_tmp3D1.tag=0,((
_tmp3D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp138),((_tmp3D2[0]=&
_tmp3D1,Cyc_fprintf(Cyc_stderr,((_tmp3D3="Core::Failure(%s)\n",_tag_dyneither(
_tmp3D3,sizeof(char),19))),_tag_dyneither(_tmp3D2,sizeof(void*),1)))))));}{
struct Cyc_Core_Failure_struct _tmp3D6;struct Cyc_Core_Failure_struct*_tmp3D5;(int)
_throw((void*)((_tmp3D5=_cycalloc(sizeof(*_tmp3D5)),((_tmp3D5[0]=((_tmp3D6.tag=
Cyc_Core_Failure,((_tmp3D6.f1=_tmp138,_tmp3D6)))),_tmp3D5)))));}_LL44:;_LL45:(
void)_throw(_tmp137);_LL41:;}}}void Cyc_Marshal_fscanntstring(struct Cyc___cycFILE*
fp,struct _dyneither_ptr s);void Cyc_Marshal_fscanntstring(struct Cyc___cycFILE*fp,
struct _dyneither_ptr s){int c;int i=0;while((c=Cyc_fgetc(fp))!= '\000'){if(c == - 1){
struct Cyc_Core_Failure_struct _tmp3DC;const char*_tmp3DB;struct Cyc_Core_Failure_struct*
_tmp3DA;(int)_throw((void*)((_tmp3DA=_cycalloc(sizeof(*_tmp3DA)),((_tmp3DA[0]=((
_tmp3DC.tag=Cyc_Core_Failure,((_tmp3DC.f1=((_tmp3DB="Read error",_tag_dyneither(
_tmp3DB,sizeof(char),11))),_tmp3DC)))),_tmp3DA)))));}{char _tmp3DF;char _tmp3DE;
struct _dyneither_ptr _tmp3DD;(_tmp3DD=_dyneither_ptr_plus(s,sizeof(char),i),((
_tmp3DE=*((char*)_check_dyneither_subscript(_tmp3DD,sizeof(char),0)),((_tmp3DF=(
char)c,((_get_dyneither_size(_tmp3DD,sizeof(char))== 1  && (_tmp3DE == '\000'  && 
_tmp3DF != '\000')?_throw_arraybounds(): 1,*((char*)_tmp3DD.curr)=_tmp3DF)))))));}
++ i;}{char _tmp3E2;char _tmp3E1;struct _dyneither_ptr _tmp3E0;(_tmp3E0=
_dyneither_ptr_plus(s,sizeof(char),i),((_tmp3E1=*((char*)
_check_dyneither_subscript(_tmp3E0,sizeof(char),0)),((_tmp3E2='\000',((
_get_dyneither_size(_tmp3E0,sizeof(char))== 1  && (_tmp3E1 == '\000'  && _tmp3E2 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp3E0.curr)=_tmp3E2)))))));}}struct _tuple7 Cyc_Marshal_rread_type_base(
struct _RegionHandle*r,void*rep,struct _tuple7 env,struct Cyc___cycFILE*fp,void*
result);static void _tmp3F3(unsigned int*_tmp3F2,unsigned int*_tmp3F1,char**
_tmp3EF){for(*_tmp3F2=0;*_tmp3F2 < *_tmp3F1;(*_tmp3F2)++){(*_tmp3EF)[*_tmp3F2]='\000';}}
static void _tmp3F9(unsigned int*_tmp3F8,unsigned int*_tmp3F7,char**_tmp3F5){for(*
_tmp3F8=0;*_tmp3F8 < *_tmp3F7;(*_tmp3F8)++){(*_tmp3F5)[*_tmp3F8]='\000';}}static
void _tmp3FF(unsigned int*_tmp3FE,unsigned int*_tmp3FD,char**_tmp3FB){for(*
_tmp3FE=0;*_tmp3FE < *_tmp3FD;(*_tmp3FE)++){(*_tmp3FB)[*_tmp3FE]='\000';}}static
void _tmp404(unsigned int*_tmp403,unsigned int*_tmp402,char**_tmp401){for(*
_tmp403=0;*_tmp403 < *_tmp402;(*_tmp403)++){(*_tmp401)[*_tmp403]='\000';}}static
void _tmp40F(unsigned int*_tmp40E,unsigned int*_tmp40D,char**_tmp40B){for(*
_tmp40E=0;*_tmp40E < *_tmp40D;(*_tmp40E)++){(*_tmp40B)[*_tmp40E]='\000';}}static
void _tmp41B(unsigned int*_tmp41A,unsigned int*_tmp419,struct _tuple1***_tmp417){
for(*_tmp41A=0;*_tmp41A < *_tmp419;(*_tmp41A)++){struct Cyc_Typerep_Int_struct*
_tmp415;struct Cyc_Typerep_Int_struct _tmp414;struct _tuple1*_tmp413;(*_tmp417)[*
_tmp41A]=((_tmp413=_cycalloc(sizeof(*_tmp413)),((_tmp413->f1=*_tmp41A,((_tmp413->f2=(
void*)((_tmp415=_cycalloc_atomic(sizeof(*_tmp415)),((_tmp415[0]=((_tmp414.tag=0,((
_tmp414.f1=0,((_tmp414.f2=8,_tmp414)))))),_tmp415)))),_tmp413))))));}}struct
_tuple7 Cyc_Marshal_rread_type_base(struct _RegionHandle*r,void*rep,struct _tuple7
env,struct Cyc___cycFILE*fp,void*result){{void*_tmp147=rep;unsigned int _tmp148;
unsigned int _tmp149;void*_tmp14A;void*_tmp14B;struct _dyneither_ptr _tmp14C;struct
_dyneither_ptr _tmp14D;struct _dyneither_ptr _tmp14E;struct _dyneither_ptr _tmp14F;
struct _dyneither_ptr _tmp150;struct _dyneither_ptr _tmp151;int _tmp152;int _tmp153;
_LL47: if(_tmp147 <= (void*)2)goto _LL49;if(*((int*)_tmp147)!= 0)goto _LL49;_tmp148=((
struct Cyc_Typerep_Int_struct*)_tmp147)->f2;_LL48: switch(_tmp148){case 8: _LL5F: {
char*x=((char*(*)(void*x))unsafe_cast)(result);{char _tmp3E7;char _tmp3E6;struct
_dyneither_ptr _tmp3E5;char*_tmp3E4;(_tmp3E5=((_tmp3E4=x,_tag_dyneither(_tmp3E4,
sizeof(char),_get_zero_arr_size_char(_tmp3E4,1)))),((_tmp3E6=*((char*)
_check_dyneither_subscript(_tmp3E5,sizeof(char),0)),((_tmp3E7=Cyc_Marshal_cycgetchar(
fp),((_get_dyneither_size(_tmp3E5,sizeof(char))== 1  && (_tmp3E6 == '\000'  && 
_tmp3E7 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp3E5.curr)=_tmp3E7)))))));}
break;}case 16: _LL60: {short*x=((short*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_cycgetshort(
fp);break;}case 32: _LL61: {int*x=((int*(*)(void*x))unsafe_cast)(result);*x=Cyc_Marshal_cycgetint(
fp);break;}default: _LL62: {struct Cyc_Core_Failure_struct _tmp3ED;const char*
_tmp3EC;struct Cyc_Core_Failure_struct*_tmp3EB;(int)_throw((void*)((_tmp3EB=
_cycalloc(sizeof(*_tmp3EB)),((_tmp3EB[0]=((_tmp3ED.tag=Cyc_Core_Failure,((
_tmp3ED.f1=((_tmp3EC="read_type_base: illegal int size bits",_tag_dyneither(
_tmp3EC,sizeof(char),38))),_tmp3ED)))),_tmp3EB)))));}}goto _LL46;_LL49: if((int)
_tmp147 != 0)goto _LL4B;_LL4A: {float*y=((float*(*)(void*x))unsafe_cast)(result);*
y=Cyc_Marshal_cycgetfloat(fp);goto _LL46;}_LL4B: if((int)_tmp147 != 1)goto _LL4D;
_LL4C: {double*y=((double*(*)(void*x))unsafe_cast)(result);*y=Cyc_Marshal_cycgetdouble(
fp);goto _LL46;}_LL4D: if(_tmp147 <= (void*)2)goto _LL4F;if(*((int*)_tmp147)!= 1)
goto _LL4F;_tmp149=((struct Cyc_Typerep_ThinPtr_struct*)_tmp147)->f1;_tmp14A=(void*)((
struct Cyc_Typerep_ThinPtr_struct*)_tmp147)->f2;_LL4E: {int mem=(int)Cyc_Marshal_cycgetvarint(
fp);if(!Cyc_Marshal_table_member(env,(unsigned int)mem)){int typ_szb=(int)Cyc_Typerep_size_type(
_tmp14A);unsigned int _tmp3F2;unsigned int _tmp3F1;struct _dyneither_ptr _tmp3F0;
char*_tmp3EF;unsigned int _tmp3EE;struct _dyneither_ptr buf=(_tmp3EE=_tmp149 * 
typ_szb,((_tmp3EF=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp3EE + 1)),((
_tmp3F0=_tag_dyneither(_tmp3EF,sizeof(char),_tmp3EE + 1),((((_tmp3F1=_tmp3EE,((
_tmp3F3(& _tmp3F2,& _tmp3F1,& _tmp3EF),_tmp3EF[_tmp3F1]=(char)0)))),_tmp3F0)))))));
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_dyneither_ptr(buf,
sizeof(char),1 + 1)));unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(
result);*x=bufptr;env=Cyc_Marshal_table_insert(env,bufptr);{int y=(int)bufptr;int
i=0;for(0;i < _tmp149;++ i){env=Cyc_Marshal_rread_type_base(r,_tmp14A,env,fp,((
void*(*)(int x))unsafe_cast)(y));y +=typ_szb;}}}else{unsigned int*x=((unsigned int*(*)(
void*x))unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(env,mem);}goto _LL46;}
_LL4F: if(_tmp147 <= (void*)2)goto _LL51;if(*((int*)_tmp147)!= 2)goto _LL51;_tmp14B=(
void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp147)->f1;_LL50: {int mem=(int)Cyc_Marshal_cycgetvarint(
fp);int pos=(int)Cyc_Marshal_cycgetvarint(fp);int sz=(int)Cyc_Marshal_cycgetvarint(
fp);int typ_szb=(int)Cyc_Typerep_size_type(_tmp14B);if(!Cyc_Marshal_table_member(
env,(unsigned int)mem)){unsigned int _tmp3F8;unsigned int _tmp3F7;struct
_dyneither_ptr _tmp3F6;char*_tmp3F5;unsigned int _tmp3F4;struct _dyneither_ptr buf=(
_tmp3F4=(unsigned int)(sz * typ_szb),((_tmp3F5=(char*)_cycalloc_atomic(
_check_times(sizeof(char),_tmp3F4 + 1)),((_tmp3F6=_tag_dyneither(_tmp3F5,sizeof(
char),_tmp3F4 + 1),((((_tmp3F7=_tmp3F4,((_tmp3F9(& _tmp3F8,& _tmp3F7,& _tmp3F5),
_tmp3F5[_tmp3F7]=(char)0)))),_tmp3F6)))))));unsigned int bufptr=(unsigned int)((
char*)_check_null(_untag_dyneither_ptr(buf,sizeof(char),1 + 1)));struct _tuple9*x=((
struct _tuple9*(*)(void*x))unsafe_cast)(result);(*x).f1=bufptr;(*x).f2=bufptr + 
pos * typ_szb;(*x).f3=bufptr + sz * typ_szb;env=Cyc_Marshal_table_insert(env,bufptr);{
int y=(int)bufptr;int i=0;for(0;i < sz;++ i){env=Cyc_Marshal_rread_type_base(r,
_tmp14B,env,fp,((void*(*)(int x))unsafe_cast)(y));y +=typ_szb;}}}else{struct
_tuple9*x=((struct _tuple9*(*)(void*x))unsafe_cast)(result);(*x).f1=Cyc_Marshal_table_lookup(
env,mem);(*x).f2=(*x).f1 + typ_szb * pos;(*x).f3=(*x).f1 + typ_szb * sz;}goto _LL46;}
_LL51: if(_tmp147 <= (void*)2)goto _LL53;if(*((int*)_tmp147)!= 3)goto _LL53;_tmp14C=((
struct Cyc_Typerep_Struct_struct*)_tmp147)->f3;_LL52: {unsigned int x=((
unsigned int(*)(void*x))unsafe_cast)(result);{int i=0;for(0;i < 
_get_dyneither_size(_tmp14C,sizeof(struct _tuple0*));++ i){unsigned int _tmp164;
void*_tmp165;struct _tuple0 _tmp163=*(*((struct _tuple0**)
_check_dyneither_subscript(_tmp14C,sizeof(struct _tuple0*),i)));_tmp164=_tmp163.f1;
_tmp165=_tmp163.f3;{int x_ofs=(int)(x + _tmp164);env=Cyc_Marshal_rread_type_base(r,
_tmp165,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL46;}_LL53: if(
_tmp147 <= (void*)2)goto _LL55;if(*((int*)_tmp147)!= 4)goto _LL55;_tmp14D=((struct
Cyc_Typerep_Tuple_struct*)_tmp147)->f2;_LL54: {unsigned int x=((unsigned int(*)(
void*x))unsafe_cast)(result);{int i=0;for(0;i < _get_dyneither_size(_tmp14D,
sizeof(struct _tuple1*));++ i){unsigned int _tmp167;void*_tmp168;struct _tuple1
_tmp166=*(*((struct _tuple1**)_check_dyneither_subscript(_tmp14D,sizeof(struct
_tuple1*),i)));_tmp167=_tmp166.f1;_tmp168=_tmp166.f2;{int x_ofs=(int)(x + _tmp167);
env=Cyc_Marshal_rread_type_base(r,_tmp168,env,fp,((void*(*)(int x))unsafe_cast)(
x_ofs));}}}goto _LL46;}_LL55: if(_tmp147 <= (void*)2)goto _LL57;if(*((int*)_tmp147)
!= 6)goto _LL57;_tmp14E=((struct Cyc_Typerep_TUnionField_struct*)_tmp147)->f4;
_LL56: {unsigned int x=((unsigned int(*)(void*x))unsafe_cast)(result);{int i=0;
for(0;i < _get_dyneither_size(_tmp14E,sizeof(struct _tuple1*));++ i){unsigned int
_tmp16A;void*_tmp16B;struct _tuple1 _tmp169=*(*((struct _tuple1**)
_check_dyneither_subscript(_tmp14E,sizeof(struct _tuple1*),i)));_tmp16A=_tmp169.f1;
_tmp16B=_tmp169.f2;{int x_ofs=(int)(x + _tmp16A);env=Cyc_Marshal_rread_type_base(r,
_tmp16B,env,fp,((void*(*)(int x))unsafe_cast)(x_ofs));}}}goto _LL46;}_LL57: if(
_tmp147 <= (void*)2)goto _LL59;if(*((int*)_tmp147)!= 5)goto _LL59;_tmp14F=((struct
Cyc_Typerep_TUnion_struct*)_tmp147)->f2;_tmp150=((struct Cyc_Typerep_TUnion_struct*)
_tmp147)->f3;_LL58: {int tagmem=(int)Cyc_Marshal_cycgetvarint(fp);if(tagmem < 1024){
int tag=tagmem;unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=(
unsigned int)tag;}else{int mem=tagmem - 1024;if(!Cyc_Marshal_table_member(env,(
unsigned int)mem)){int tag=(int)Cyc_Marshal_cycgetvarint(fp);int _tmp16D;void*
_tmp16E;struct _tuple4 _tmp16C=Cyc_Typerep_get_unionbranch((unsigned int)tag,
_tmp150);_tmp16D=_tmp16C.f1;_tmp16E=_tmp16C.f2;{unsigned int _tmp3FE;unsigned int
_tmp3FD;struct _dyneither_ptr _tmp3FC;char*_tmp3FB;unsigned int _tmp3FA;struct
_dyneither_ptr buf=(_tmp3FA=_tmp16D + Cyc_Typerep_size_type(_tmp16E),((_tmp3FB=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp3FA + 1)),((_tmp3FC=
_tag_dyneither(_tmp3FB,sizeof(char),_tmp3FA + 1),((((_tmp3FD=_tmp3FA,((_tmp3FF(&
_tmp3FE,& _tmp3FD,& _tmp3FB),_tmp3FB[_tmp3FD]=(char)0)))),_tmp3FC)))))));
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_dyneither_ptr(buf,
sizeof(char),1 + 1)));env=Cyc_Marshal_table_insert(env,bufptr);{unsigned int*x=((
unsigned int*(*)(void*x))unsafe_cast)(result);*x=bufptr;{unsigned int*tagptr=((
unsigned int*(*)(unsigned int x))unsafe_cast)(bufptr);*tagptr=(unsigned int)tag;
env=Cyc_Marshal_rread_type_base(r,_tmp16E,env,fp,((void*(*)(unsigned int x))
unsafe_cast)(bufptr + _tmp16D));}}}}else{unsigned int*x=((unsigned int*(*)(void*x))
unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(env,mem);}}goto _LL46;}_LL59: if(
_tmp147 <= (void*)2)goto _LL5B;if(*((int*)_tmp147)!= 7)goto _LL5B;_tmp151=((struct
Cyc_Typerep_XTUnion_struct*)_tmp147)->f2;_LL5A: {int mem=(int)Cyc_Marshal_cycgetvarint(
fp);if(!Cyc_Marshal_table_member(env,(unsigned int)mem)){char nonnull=(char)Cyc_Marshal_cycgetchar(
fp);char*_tmp409;unsigned int _tmp408;char*_tmp407;unsigned int _tmp406;
unsigned int _tmp405;struct _dyneither_ptr s=(_tmp409=((_tmp408=(unsigned int)100,((
_tmp407=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp408 + 1)),((((
_tmp406=_tmp408,((_tmp404(& _tmp405,& _tmp406,& _tmp407),_tmp407[_tmp406]=(char)0)))),
_tmp407)))))),_tag_dyneither(_tmp409,sizeof(char),_get_zero_arr_size_char(
_tmp409,(unsigned int)100 + 1)));struct _dyneither_ptr xtname=_dyneither_ptr_plus(s,
sizeof(char),4);Cyc_Marshal_fscanntstring(fp,xtname);if((int)nonnull){int _tmp174;
void*_tmp175;struct _tuple4 _tmp173=Cyc_Typerep_get_xtunionbranch((struct
_dyneither_ptr)xtname,_tmp151);_tmp174=_tmp173.f1;_tmp175=_tmp173.f2;{
unsigned int _tmp40E;unsigned int _tmp40D;struct _dyneither_ptr _tmp40C;char*_tmp40B;
unsigned int _tmp40A;struct _dyneither_ptr buf=(_tmp40A=_tmp174 + Cyc_Typerep_size_type(
_tmp175),((_tmp40B=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp40A + 1)),((
_tmp40C=_tag_dyneither(_tmp40B,sizeof(char),_tmp40A + 1),((((_tmp40D=_tmp40A,((
_tmp40F(& _tmp40E,& _tmp40D,& _tmp40B),_tmp40B[_tmp40D]=(char)0)))),_tmp40C)))))));
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_dyneither_ptr(buf,
sizeof(char),1 + 1)));env=Cyc_Marshal_table_insert(env,bufptr);{unsigned int*x=((
unsigned int*(*)(void*x))unsafe_cast)(result);*x=bufptr;{unsigned int*tagptr=((
unsigned int*(*)(unsigned int x))unsafe_cast)(bufptr);*tagptr=(unsigned int)((
char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1 + 1)));env=Cyc_Marshal_rread_type_base(
r,_tmp175,env,fp,((void*(*)(unsigned int x))unsafe_cast)(bufptr + _tmp174));}}}}
else{unsigned int*x=((unsigned int*(*)(void*x))unsafe_cast)(result);*x=(
unsigned int)((char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1 + 1)));env=
Cyc_Marshal_table_insert(env,*x);}}else{unsigned int*x=((unsigned int*(*)(void*x))
unsafe_cast)(result);*x=Cyc_Marshal_table_lookup(env,mem);}goto _LL46;}_LL5B: if(
_tmp147 <= (void*)2)goto _LL5D;if(*((int*)_tmp147)!= 8)goto _LL5D;_tmp152=((struct
Cyc_Typerep_Union_struct*)_tmp147)->f2;_LL5C:{struct Cyc_Typerep_Tuple_struct
_tmp428;unsigned int _tmp427;unsigned int _tmp426;struct _dyneither_ptr _tmp425;
struct _tuple1**_tmp424;unsigned int _tmp423;struct Cyc_Typerep_Tuple_struct*
_tmp422;env=Cyc_Marshal_rread_type_base(r,(void*)((_tmp422=_cycalloc(sizeof(*
_tmp422)),((_tmp422[0]=((_tmp428.tag=4,((_tmp428.f1=(unsigned int)_tmp152,((
_tmp428.f2=((_tmp423=(unsigned int)_tmp152,((_tmp424=(struct _tuple1**)_cycalloc(
_check_times(sizeof(struct _tuple1*),_tmp423)),((_tmp425=_tag_dyneither(_tmp424,
sizeof(struct _tuple1*),_tmp423),((((_tmp426=_tmp423,_tmp41B(& _tmp427,& _tmp426,&
_tmp424))),_tmp425)))))))),_tmp428)))))),_tmp422)))),env,fp,result);}goto _LL46;
_LL5D: if(_tmp147 <= (void*)2)goto _LL46;if(*((int*)_tmp147)!= 9)goto _LL46;_tmp153=((
struct Cyc_Typerep_Enum_struct*)_tmp147)->f2;_LL5E: {struct Cyc_Typerep_Int_struct
_tmp42B;struct Cyc_Typerep_Int_struct*_tmp42A;env=Cyc_Marshal_rread_type_base(r,(
void*)((_tmp42A=_cycalloc_atomic(sizeof(*_tmp42A)),((_tmp42A[0]=((_tmp42B.tag=0,((
_tmp42B.f1=0,((_tmp42B.f2=(unsigned int)(_tmp153 << 3),_tmp42B)))))),_tmp42A)))),
env,fp,result);}_LL46:;}return env;}void*Cyc_Marshal_rread_type(struct
_RegionHandle*r,void*ts,struct Cyc___cycFILE*fp);static void _tmp431(unsigned int*
_tmp430,unsigned int*_tmp42F,char**_tmp42D){for(*_tmp430=0;*_tmp430 < *_tmp42F;(*
_tmp430)++){(*_tmp42D)[*_tmp430]='\000';}}void*Cyc_Marshal_rread_type(struct
_RegionHandle*r,void*ts,struct Cyc___cycFILE*fp){struct _handler_cons _tmp189;
_push_handler(& _tmp189);{int _tmp18B=0;if(setjmp(_tmp189.handler))_tmp18B=1;if(!
_tmp18B){{unsigned int _tmp430;unsigned int _tmp42F;struct _dyneither_ptr _tmp42E;
char*_tmp42D;unsigned int _tmp42C;struct _dyneither_ptr buf=(_tmp42C=Cyc_Typerep_size_type(
ts),((_tmp42D=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp42C + 1)),((
_tmp42E=_tag_dyneither(_tmp42D,sizeof(char),_tmp42C + 1),((((_tmp42F=_tmp42C,((
_tmp431(& _tmp430,& _tmp42F,& _tmp42D),_tmp42D[_tmp42F]=(char)0)))),_tmp42E)))))));
void*result=((void*(*)(char*x))unsafe_cast)((char*)_check_null(
_untag_dyneither_ptr(buf,sizeof(char),1 + 1)));Cyc_Marshal_rread_type_base(r,ts,
Cyc_Marshal_empty_addr_table(),fp,result);{void*_tmp18C=result;_npop_handler(0);
return _tmp18C;}};_pop_handler();}else{void*_tmp18A=(void*)_exn_thrown;void*
_tmp192=_tmp18A;struct _dyneither_ptr _tmp193;_LL65: if(*((void**)_tmp192)!= Cyc_Core_Failure)
goto _LL67;_tmp193=((struct Cyc_Core_Failure_struct*)_tmp192)->f1;_LL66:{const char*
_tmp435;void*_tmp434[1];struct Cyc_String_pa_struct _tmp433;(_tmp433.tag=0,((
_tmp433.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp193),((_tmp434[0]=&
_tmp433,Cyc_fprintf(Cyc_stderr,((_tmp435="Core::Failure(%s)\n",_tag_dyneither(
_tmp435,sizeof(char),19))),_tag_dyneither(_tmp434,sizeof(void*),1)))))));}{
struct Cyc_Core_Failure_struct _tmp438;struct Cyc_Core_Failure_struct*_tmp437;(int)
_throw((void*)((_tmp437=_cycalloc(sizeof(*_tmp437)),((_tmp437[0]=((_tmp438.tag=
Cyc_Core_Failure,((_tmp438.f1=_tmp193,_tmp438)))),_tmp437)))));}_LL67:;_LL68:(
void)_throw(_tmp192);_LL64:;}}}void*Cyc_Marshal_read_type(void*ts,struct Cyc___cycFILE*
fp);void*Cyc_Marshal_read_type(void*ts,struct Cyc___cycFILE*fp){return Cyc_Marshal_rread_type(
Cyc_Core_heap_region,ts,fp);}static struct Cyc_Dict_Dict Cyc_Marshal_empty_addr_map();
static struct Cyc_Dict_Dict Cyc_Marshal_empty_addr_map(){return((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,unsigned int k,unsigned int v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(
int(*cmp)(unsigned int,unsigned int)))Cyc_Dict_empty)(Cyc_Marshal_uint_cmp),0,0);}
struct Cyc_Dict_Dict Cyc_Marshal_rcopy_type_base(struct _RegionHandle*r,void*rep,
struct Cyc_Dict_Dict env,void*val,void*result);static void _tmp443(unsigned int*
_tmp442,unsigned int*_tmp441,char**_tmp43F){for(*_tmp442=0;*_tmp442 < *_tmp441;(*
_tmp442)++){(*_tmp43F)[*_tmp442]='\000';}}static void _tmp44A(unsigned int*_tmp449,
unsigned int*_tmp448,char**_tmp446){for(*_tmp449=0;*_tmp449 < *_tmp448;(*_tmp449)
++){(*_tmp446)[*_tmp449]='\000';}}static void _tmp451(unsigned int*_tmp450,
unsigned int*_tmp44F,char**_tmp44D){for(*_tmp450=0;*_tmp450 < *_tmp44F;(*_tmp450)
++){(*_tmp44D)[*_tmp450]='\000';}}static void _tmp457(unsigned int*_tmp456,
unsigned int*_tmp455,char**_tmp453){for(*_tmp456=0;*_tmp456 < *_tmp455;(*_tmp456)
++){(*_tmp453)[*_tmp456]='\000';}}static void _tmp463(unsigned int*_tmp462,
unsigned int*_tmp461,struct _tuple1***_tmp45F){for(*_tmp462=0;*_tmp462 < *_tmp461;(*
_tmp462)++){struct Cyc_Typerep_Int_struct*_tmp45D;struct Cyc_Typerep_Int_struct
_tmp45C;struct _tuple1*_tmp45B;(*_tmp45F)[*_tmp462]=((_tmp45B=_cycalloc(sizeof(*
_tmp45B)),((_tmp45B->f1=*_tmp462,((_tmp45B->f2=(void*)((_tmp45D=_cycalloc_atomic(
sizeof(*_tmp45D)),((_tmp45D[0]=((_tmp45C.tag=0,((_tmp45C.f1=0,((_tmp45C.f2=8,
_tmp45C)))))),_tmp45D)))),_tmp45B))))));}}struct Cyc_Dict_Dict Cyc_Marshal_rcopy_type_base(
struct _RegionHandle*r,void*rep,struct Cyc_Dict_Dict env,void*val,void*result){{
void*_tmp199=rep;unsigned int _tmp19A;unsigned int _tmp19B;unsigned int _tmp19C;
unsigned int _tmp19D;unsigned int _tmp19E;void*_tmp19F;void*_tmp1A0;struct
_dyneither_ptr _tmp1A1;struct _dyneither_ptr _tmp1A2;struct _dyneither_ptr _tmp1A3;
struct _dyneither_ptr _tmp1A4;struct _dyneither_ptr _tmp1A5;struct _dyneither_ptr
_tmp1A6;int _tmp1A7;int _tmp1A8;_LL6A: if(_tmp199 <= (void*)2)goto _LL72;if(*((int*)
_tmp199)!= 0)goto _LL6C;_tmp19A=((struct Cyc_Typerep_Int_struct*)_tmp199)->f2;if(
_tmp19A != 8)goto _LL6C;_LL6B: {char*val0=((char*(*)(void*x))unsafe_cast)(val);
char*result0=((char*(*)(void*x))unsafe_cast)(result);{char _tmp43D;char _tmp43C;
struct _dyneither_ptr _tmp43B;char*_tmp43A;(_tmp43B=((_tmp43A=result0,
_tag_dyneither(_tmp43A,sizeof(char),_get_zero_arr_size_char(_tmp43A,1)))),((
_tmp43C=*((char*)_check_dyneither_subscript(_tmp43B,sizeof(char),0)),((_tmp43D=*
val0,((_get_dyneither_size(_tmp43B,sizeof(char))== 1  && (_tmp43C == '\000'  && 
_tmp43D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp43B.curr)=_tmp43D)))))));}
goto _LL69;}_LL6C: if(*((int*)_tmp199)!= 0)goto _LL6E;_tmp19B=((struct Cyc_Typerep_Int_struct*)
_tmp199)->f2;if(_tmp19B != 16)goto _LL6E;_LL6D: {short*val0=((short*(*)(void*x))
unsafe_cast)(val);short*result0=((short*(*)(void*x))unsafe_cast)(result);*
result0=*val0;goto _LL69;}_LL6E: if(*((int*)_tmp199)!= 0)goto _LL70;_tmp19C=((
struct Cyc_Typerep_Int_struct*)_tmp199)->f2;if(_tmp19C != 32)goto _LL70;_LL6F: {int*
val0=((int*(*)(void*x))unsafe_cast)(val);int*result0=((int*(*)(void*x))
unsafe_cast)(result);*result0=*val0;goto _LL69;}_LL70: if(*((int*)_tmp199)!= 0)
goto _LL72;_tmp19D=((struct Cyc_Typerep_Int_struct*)_tmp199)->f2;if(_tmp19D != 64)
goto _LL72;_LL71: {long long*val0=((long long*(*)(void*x))unsafe_cast)(val);
long long*result0=((long long*(*)(void*x))unsafe_cast)(result);*result0=*val0;
goto _LL69;}_LL72: if((int)_tmp199 != 0)goto _LL74;_LL73: {float*val0=((float*(*)(
void*x))unsafe_cast)(val);float*result0=((float*(*)(void*x))unsafe_cast)(result);*
result0=*val0;goto _LL69;}_LL74: if((int)_tmp199 != 1)goto _LL76;_LL75: {double*val0=((
double*(*)(void*x))unsafe_cast)(val);double*result0=((double*(*)(void*x))
unsafe_cast)(result);*result0=*val0;goto _LL69;}_LL76: if(_tmp199 <= (void*)2)goto
_LL88;if(*((int*)_tmp199)!= 1)goto _LL78;_tmp19E=((struct Cyc_Typerep_ThinPtr_struct*)
_tmp199)->f1;_tmp19F=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp199)->f2;
_LL77: {unsigned int*val0=((unsigned int*(*)(void*x))unsafe_cast)(val);
unsigned int*result0=((unsigned int*(*)(void*x))unsafe_cast)(result);int addr=(
int)*val0;if(((int(*)(struct Cyc_Dict_Dict d,unsigned int k))Cyc_Dict_member)(env,(
unsigned int)addr))*result0=((unsigned int(*)(struct Cyc_Dict_Dict d,unsigned int k))
Cyc_Dict_lookup)(env,(unsigned int)addr);else{int typ_szb=(int)Cyc_Typerep_size_type(
_tmp19F);unsigned int _tmp442;unsigned int _tmp441;struct _dyneither_ptr _tmp440;
char*_tmp43F;unsigned int _tmp43E;struct _dyneither_ptr buf=(_tmp43E=_tmp19E * 
typ_szb,((_tmp43F=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp43E + 1)),((
_tmp440=_tag_dyneither(_tmp43F,sizeof(char),_tmp43E + 1),((((_tmp441=_tmp43E,((
_tmp443(& _tmp442,& _tmp441,& _tmp43F),_tmp43F[_tmp441]=(char)0)))),_tmp440)))))));
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_dyneither_ptr(buf,
sizeof(char),1 + 1)));*result0=bufptr;env=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,unsigned int k,unsigned int v))Cyc_Dict_insert)(env,(unsigned int)addr,bufptr);{
int x=addr;int y=(int)bufptr;int i=0;for(0;i < _tmp19E;++ i){env=Cyc_Marshal_rcopy_type_base(
r,_tmp19F,env,((void*(*)(int x))unsafe_cast)(x),((void*(*)(int x))unsafe_cast)(y));
x +=typ_szb;y +=typ_szb;}}}goto _LL69;}_LL78: if(*((int*)_tmp199)!= 2)goto _LL7A;
_tmp1A0=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp199)->f1;_LL79: {struct
_tuple9*val0=((struct _tuple9*(*)(void*x))unsafe_cast)(val);struct _tuple9*result0=((
struct _tuple9*(*)(void*x))unsafe_cast)(result);unsigned int _tmp1B2;unsigned int
_tmp1B3;unsigned int _tmp1B4;struct _tuple9 _tmp1B1=*val0;_tmp1B2=_tmp1B1.f1;
_tmp1B3=_tmp1B1.f2;_tmp1B4=_tmp1B1.f3;{int typ_szb=(int)Cyc_Typerep_size_type(
_tmp1A0);int size=(int)((_tmp1B4 - _tmp1B2)/ typ_szb);int pos=(int)((_tmp1B3 - 
_tmp1B2)/ typ_szb);if(((int(*)(struct Cyc_Dict_Dict d,unsigned int k))Cyc_Dict_member)(
env,_tmp1B2)){unsigned int _tmp1B5=((unsigned int(*)(struct Cyc_Dict_Dict d,
unsigned int k))Cyc_Dict_lookup)(env,_tmp1B2);struct _tuple9 _tmp444;*result0=((
_tmp444.f1=_tmp1B5,((_tmp444.f2=_tmp1B5 + pos * typ_szb,((_tmp444.f3=_tmp1B5 + size
* typ_szb,_tmp444))))));}else{unsigned int _tmp449;unsigned int _tmp448;struct
_dyneither_ptr _tmp447;char*_tmp446;unsigned int _tmp445;struct _dyneither_ptr buf=(
_tmp445=_tmp1B4 - _tmp1B2,((_tmp446=(char*)_region_malloc(r,_check_times(sizeof(
char),_tmp445 + 1)),((_tmp447=_tag_dyneither(_tmp446,sizeof(char),_tmp445 + 1),((((
_tmp448=_tmp445,((_tmp44A(& _tmp449,& _tmp448,& _tmp446),_tmp446[_tmp448]=(char)0)))),
_tmp447)))))));unsigned int bufptr=(unsigned int)((char*)_check_null(
_untag_dyneither_ptr(buf,sizeof(char),1 + 1)));{struct _tuple9 _tmp44B;*result0=((
_tmp44B.f1=bufptr,((_tmp44B.f2=bufptr + pos * typ_szb,((_tmp44B.f3=bufptr + size * 
typ_szb,_tmp44B))))));}env=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
unsigned int k,unsigned int v))Cyc_Dict_insert)(env,_tmp1B2,bufptr);{int y=(int)
_tmp1B2;int z=(int)bufptr;int i=0;for(0;i < size;++ i){env=Cyc_Marshal_rcopy_type_base(
r,_tmp1A0,env,((void*(*)(int x))unsafe_cast)(y),((void*(*)(int x))unsafe_cast)(z));
y +=typ_szb;z +=typ_szb;}}}goto _LL69;}}_LL7A: if(*((int*)_tmp199)!= 3)goto _LL7C;
_tmp1A1=((struct Cyc_Typerep_Struct_struct*)_tmp199)->f3;_LL7B: {int x=((int(*)(
void*x))unsafe_cast)(val);int y=((int(*)(void*x))unsafe_cast)(result);{int i=0;
for(0;i < _get_dyneither_size(_tmp1A1,sizeof(struct _tuple0*));++ i){unsigned int
_tmp1BD;void*_tmp1BE;struct _tuple0 _tmp1BC=*(*((struct _tuple0**)
_check_dyneither_subscript(_tmp1A1,sizeof(struct _tuple0*),i)));_tmp1BD=_tmp1BC.f1;
_tmp1BE=_tmp1BC.f3;{int x_ofs=(int)(x + _tmp1BD);int y_ofs=(int)(y + _tmp1BD);env=
Cyc_Marshal_rcopy_type_base(r,_tmp1BE,env,((void*(*)(int x))unsafe_cast)(x_ofs),((
void*(*)(int x))unsafe_cast)(y_ofs));}}}goto _LL69;}_LL7C: if(*((int*)_tmp199)!= 4)
goto _LL7E;_tmp1A2=((struct Cyc_Typerep_Tuple_struct*)_tmp199)->f2;_LL7D: {int x=((
int(*)(void*x))unsafe_cast)(val);int y=((int(*)(void*x))unsafe_cast)(result);{int
i=0;for(0;i < _get_dyneither_size(_tmp1A2,sizeof(struct _tuple1*));++ i){
unsigned int _tmp1C0;void*_tmp1C1;struct _tuple1 _tmp1BF=*(*((struct _tuple1**)
_check_dyneither_subscript(_tmp1A2,sizeof(struct _tuple1*),i)));_tmp1C0=_tmp1BF.f1;
_tmp1C1=_tmp1BF.f2;{int x_ofs=(int)(x + _tmp1C0);int y_ofs=(int)(y + _tmp1C0);env=
Cyc_Marshal_rcopy_type_base(r,_tmp1C1,env,((void*(*)(int x))unsafe_cast)(x_ofs),((
void*(*)(int x))unsafe_cast)(y_ofs));}}}goto _LL69;}_LL7E: if(*((int*)_tmp199)!= 6)
goto _LL80;_tmp1A3=((struct Cyc_Typerep_TUnionField_struct*)_tmp199)->f4;_LL7F: {
int x=((int(*)(void*x))unsafe_cast)(val);int y=((int(*)(void*x))unsafe_cast)(
result);{int i=0;for(0;i < _get_dyneither_size(_tmp1A3,sizeof(struct _tuple1*));++ i){
unsigned int _tmp1C3;void*_tmp1C4;struct _tuple1 _tmp1C2=*(*((struct _tuple1**)
_check_dyneither_subscript(_tmp1A3,sizeof(struct _tuple1*),i)));_tmp1C3=_tmp1C2.f1;
_tmp1C4=_tmp1C2.f2;{int x_ofs=(int)(x + _tmp1C3);int y_ofs=(int)(y + _tmp1C3);env=
Cyc_Marshal_rcopy_type_base(r,_tmp1C4,env,((void*(*)(int x))unsafe_cast)(x_ofs),((
void*(*)(int x))unsafe_cast)(y_ofs));}}}goto _LL69;}_LL80: if(*((int*)_tmp199)!= 5)
goto _LL82;_tmp1A4=((struct Cyc_Typerep_TUnion_struct*)_tmp199)->f2;_tmp1A5=((
struct Cyc_Typerep_TUnion_struct*)_tmp199)->f3;_LL81: {unsigned int*val0=((
unsigned int*(*)(void*x))unsafe_cast)(val);unsigned int*result0=((unsigned int*(*)(
void*x))unsafe_cast)(result);if(*val0 < 1024)*result0=*val0;else{unsigned int addr=*
val0;if(((int(*)(struct Cyc_Dict_Dict d,unsigned int k))Cyc_Dict_member)(env,addr))*
result0=((unsigned int(*)(struct Cyc_Dict_Dict d,unsigned int k))Cyc_Dict_lookup)(
env,addr);else{unsigned int*tufp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*
val0);unsigned int tag=*tufp;int _tmp1C6;void*_tmp1C7;struct _tuple4 _tmp1C5=Cyc_Typerep_get_unionbranch(
tag,_tmp1A5);_tmp1C6=_tmp1C5.f1;_tmp1C7=_tmp1C5.f2;{unsigned int _tmp450;
unsigned int _tmp44F;struct _dyneither_ptr _tmp44E;char*_tmp44D;unsigned int _tmp44C;
struct _dyneither_ptr buf=(_tmp44C=_tmp1C6 + Cyc_Typerep_size_type(_tmp1C7),((
_tmp44D=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp44C + 1)),((_tmp44E=
_tag_dyneither(_tmp44D,sizeof(char),_tmp44C + 1),((((_tmp44F=_tmp44C,((_tmp451(&
_tmp450,& _tmp44F,& _tmp44D),_tmp44D[_tmp44F]=(char)0)))),_tmp44E)))))));
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_dyneither_ptr(buf,
sizeof(char),1 + 1)));env=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
unsigned int k,unsigned int v))Cyc_Dict_insert)(env,addr,bufptr);*result0=bufptr;{
unsigned int*newtagp=((unsigned int*(*)(unsigned int x))unsafe_cast)(bufptr);*
newtagp=tag;env=Cyc_Marshal_rcopy_type_base(r,_tmp1C7,env,((void*(*)(
unsigned int x))unsafe_cast)(addr + _tmp1C6),((void*(*)(unsigned int x))unsafe_cast)(
bufptr + _tmp1C6));}}}}goto _LL69;}_LL82: if(*((int*)_tmp199)!= 7)goto _LL84;_tmp1A6=((
struct Cyc_Typerep_XTUnion_struct*)_tmp199)->f2;_LL83: {unsigned int*val0=((
unsigned int*(*)(void*x))unsafe_cast)(val);unsigned int*result0=((unsigned int*(*)(
void*x))unsafe_cast)(result);unsigned int addr=*val0;if(((int(*)(struct Cyc_Dict_Dict
d,unsigned int k))Cyc_Dict_member)(env,addr))*result0=((unsigned int(*)(struct Cyc_Dict_Dict
d,unsigned int k))Cyc_Dict_lookup)(env,addr);else{unsigned int*xtstructp=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*val0);if(*xtstructp != 0){
unsigned int xtnamep=*xtstructp;char*xtnamec=((char*(*)(unsigned int x))
unsafe_cast)(xtnamep + 4);struct _dyneither_ptr xtname=(struct _dyneither_ptr)
Cstring_to_string(xtnamec);int _tmp1CD;void*_tmp1CE;struct _tuple4 _tmp1CC=Cyc_Typerep_get_xtunionbranch(
xtname,_tmp1A6);_tmp1CD=_tmp1CC.f1;_tmp1CE=_tmp1CC.f2;{unsigned int _tmp456;
unsigned int _tmp455;struct _dyneither_ptr _tmp454;char*_tmp453;unsigned int _tmp452;
struct _dyneither_ptr buf=(_tmp452=_tmp1CD + Cyc_Typerep_size_type(_tmp1CE),((
_tmp453=(char*)_region_malloc(r,_check_times(sizeof(char),_tmp452 + 1)),((_tmp454=
_tag_dyneither(_tmp453,sizeof(char),_tmp452 + 1),((((_tmp455=_tmp452,((_tmp457(&
_tmp456,& _tmp455,& _tmp453),_tmp453[_tmp455]=(char)0)))),_tmp454)))))));
unsigned int bufptr=(unsigned int)((char*)_check_null(_untag_dyneither_ptr(buf,
sizeof(char),1 + 1)));*result0=bufptr;{unsigned int*tagptr=((unsigned int*(*)(
unsigned int x))unsafe_cast)(bufptr);*tagptr=xtnamep;{void*xoff=((void*(*)(
unsigned int x))unsafe_cast)(*val0 + _tmp1CD);env=Cyc_Marshal_rcopy_type_base(r,
_tmp1CE,env,xoff,((void*(*)(unsigned int x))unsafe_cast)(bufptr + _tmp1CD));env=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,unsigned int k,unsigned int v))Cyc_Dict_insert)(
env,addr,bufptr);}}}}else{*result0=*val0;}}goto _LL69;}_LL84: if(*((int*)_tmp199)
!= 8)goto _LL86;_tmp1A7=((struct Cyc_Typerep_Union_struct*)_tmp199)->f2;_LL85:{
struct Cyc_Typerep_Tuple_struct _tmp470;unsigned int _tmp46F;unsigned int _tmp46E;
struct _dyneither_ptr _tmp46D;struct _tuple1**_tmp46C;unsigned int _tmp46B;struct Cyc_Typerep_Tuple_struct*
_tmp46A;env=Cyc_Marshal_rcopy_type_base(r,(void*)((_tmp46A=_cycalloc(sizeof(*
_tmp46A)),((_tmp46A[0]=((_tmp470.tag=4,((_tmp470.f1=(unsigned int)_tmp1A7,((
_tmp470.f2=((_tmp46B=(unsigned int)_tmp1A7,((_tmp46C=(struct _tuple1**)_cycalloc(
_check_times(sizeof(struct _tuple1*),_tmp46B)),((_tmp46D=_tag_dyneither(_tmp46C,
sizeof(struct _tuple1*),_tmp46B),((((_tmp46E=_tmp46B,_tmp463(& _tmp46F,& _tmp46E,&
_tmp46C))),_tmp46D)))))))),_tmp470)))))),_tmp46A)))),env,val,result);}goto _LL69;
_LL86: if(*((int*)_tmp199)!= 9)goto _LL88;_tmp1A8=((struct Cyc_Typerep_Enum_struct*)
_tmp199)->f2;_LL87:{struct Cyc_Typerep_Int_struct _tmp473;struct Cyc_Typerep_Int_struct*
_tmp472;env=Cyc_Marshal_rcopy_type_base(r,(void*)((_tmp472=_cycalloc_atomic(
sizeof(*_tmp472)),((_tmp472[0]=((_tmp473.tag=0,((_tmp473.f1=0,((_tmp473.f2=(
unsigned int)(_tmp1A8 << 3),_tmp473)))))),_tmp472)))),env,val,result);}goto _LL69;
_LL88:;_LL89: {struct Cyc_Core_Failure_struct _tmp479;const char*_tmp478;struct Cyc_Core_Failure_struct*
_tmp477;(int)_throw((void*)((_tmp477=_cycalloc(sizeof(*_tmp477)),((_tmp477[0]=((
_tmp479.tag=Cyc_Core_Failure,((_tmp479.f1=((_tmp478="Unhandled case in copy_type_base",
_tag_dyneither(_tmp478,sizeof(char),33))),_tmp479)))),_tmp477)))));}_LL69:;}
return env;}void*Cyc_Marshal_rcopy_type(struct _RegionHandle*r,void*ts,void*val);
static void _tmp47F(unsigned int*_tmp47E,unsigned int*_tmp47D,char**_tmp47B){for(*
_tmp47E=0;*_tmp47E < *_tmp47D;(*_tmp47E)++){(*_tmp47B)[*_tmp47E]='\000';}}void*
Cyc_Marshal_rcopy_type(struct _RegionHandle*r,void*ts,void*val){struct
_handler_cons _tmp1E1;_push_handler(& _tmp1E1);{int _tmp1E3=0;if(setjmp(_tmp1E1.handler))
_tmp1E3=1;if(!_tmp1E3){{unsigned int _tmp47E;unsigned int _tmp47D;struct
_dyneither_ptr _tmp47C;char*_tmp47B;unsigned int _tmp47A;struct _dyneither_ptr buf=(
_tmp47A=Cyc_Typerep_size_type(ts),((_tmp47B=(char*)_region_malloc(r,_check_times(
sizeof(char),_tmp47A + 1)),((_tmp47C=_tag_dyneither(_tmp47B,sizeof(char),_tmp47A + 
1),((((_tmp47D=_tmp47A,((_tmp47F(& _tmp47E,& _tmp47D,& _tmp47B),_tmp47B[_tmp47D]=(
char)0)))),_tmp47C)))))));void*result=((void*(*)(char*x))unsafe_cast)((char*)
_check_null(_untag_dyneither_ptr(buf,sizeof(char),1 + 1)));Cyc_Marshal_rcopy_type_base(
r,ts,Cyc_Marshal_empty_addr_map(),val,result);{void*_tmp1E4=result;_npop_handler(
0);return _tmp1E4;}};_pop_handler();}else{void*_tmp1E2=(void*)_exn_thrown;void*
_tmp1EA=_tmp1E2;struct _dyneither_ptr _tmp1EB;_LL8B: if(*((void**)_tmp1EA)!= Cyc_Core_Failure)
goto _LL8D;_tmp1EB=((struct Cyc_Core_Failure_struct*)_tmp1EA)->f1;_LL8C:{const char*
_tmp483;void*_tmp482[1];struct Cyc_String_pa_struct _tmp481;(_tmp481.tag=0,((
_tmp481.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1EB),((_tmp482[0]=&
_tmp481,Cyc_fprintf(Cyc_stderr,((_tmp483="Core::Failure(%s)\n",_tag_dyneither(
_tmp483,sizeof(char),19))),_tag_dyneither(_tmp482,sizeof(void*),1)))))));}{
struct Cyc_Core_Failure_struct _tmp486;struct Cyc_Core_Failure_struct*_tmp485;(int)
_throw((void*)((_tmp485=_cycalloc(sizeof(*_tmp485)),((_tmp485[0]=((_tmp486.tag=
Cyc_Core_Failure,((_tmp486.f1=_tmp1EB,_tmp486)))),_tmp485)))));}_LL8D:;_LL8E:(
void)_throw(_tmp1EA);_LL8A:;}}}void*Cyc_Marshal_copy_type(void*ts,void*val);void*
Cyc_Marshal_copy_type(void*ts,void*val){return Cyc_Marshal_rcopy_type(Cyc_Core_heap_region,
ts,val);}void*Cyc_Marshal_order_lex(void*ord1,void*ord2);void*Cyc_Marshal_order_lex(
void*ord1,void*ord2){if(ord1 != (void*)1)return ord1;else{return ord2;}}struct
_tuple10{unsigned int f1;unsigned int f2;};static int Cyc_Marshal_addrp_cmp(struct
_tuple10*addrp1,struct _tuple10*addrp2);static int Cyc_Marshal_addrp_cmp(struct
_tuple10*addrp1,struct _tuple10*addrp2){if((*addrp1).f1 < (*addrp2).f1)return 1;
else{if((*addrp1).f1 == (*addrp2).f1)return Cyc_Marshal_uint_cmp((*addrp1).f2,(*
addrp2).f2);else{return - 1;}}}struct Cyc_Dict_Dict Cyc_Marshal_empty_eq_dict();
struct Cyc_Dict_Dict Cyc_Marshal_empty_eq_dict(){struct _tuple10*_tmp487;return((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple10*k,void*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple10*,struct _tuple10*)))Cyc_Dict_empty)(
Cyc_Marshal_addrp_cmp),((_tmp487=_cycalloc_atomic(sizeof(*_tmp487)),((_tmp487->f1=
0,((_tmp487->f2=0,_tmp487)))))),(void*)1);}struct _tuple11{void*f1;struct Cyc_Dict_Dict
f2;};struct _tuple11 Cyc_Marshal_return_update(struct _tuple10*pair,struct _tuple11
ordenv);struct _tuple11 Cyc_Marshal_return_update(struct _tuple10*pair,struct
_tuple11 ordenv){void*_tmp1F3;struct Cyc_Dict_Dict _tmp1F4;struct _tuple11 _tmp1F2=
ordenv;_tmp1F3=_tmp1F2.f1;_tmp1F4=_tmp1F2.f2;_tmp1F4=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple10*k,void*v))Cyc_Dict_insert)(_tmp1F4,pair,
_tmp1F3);{struct _tuple11 _tmp488;return(_tmp488.f1=_tmp1F3,((_tmp488.f2=_tmp1F4,
_tmp488)));}}struct _tuple11 Cyc_Marshal_leq_eq_type(void*rep,struct Cyc_Dict_Dict
env,void*x,void*y);static void _tmp4BE(unsigned int*_tmp4BD,unsigned int*_tmp4BC,
struct _tuple1***_tmp4BA){for(*_tmp4BD=0;*_tmp4BD < *_tmp4BC;(*_tmp4BD)++){struct
Cyc_Typerep_Int_struct*_tmp4B8;struct Cyc_Typerep_Int_struct _tmp4B7;struct _tuple1*
_tmp4B6;(*_tmp4BA)[*_tmp4BD]=((_tmp4B6=_cycalloc(sizeof(*_tmp4B6)),((_tmp4B6->f1=*
_tmp4BD,((_tmp4B6->f2=(void*)((_tmp4B8=_cycalloc_atomic(sizeof(*_tmp4B8)),((
_tmp4B8[0]=((_tmp4B7.tag=0,((_tmp4B7.f1=0,((_tmp4B7.f2=8,_tmp4B7)))))),_tmp4B8)))),
_tmp4B6))))));}}struct _tuple11 Cyc_Marshal_leq_eq_type(void*rep,struct Cyc_Dict_Dict
env,void*x,void*y){if(x == y){struct _tuple11 _tmp489;return(_tmp489.f1=(void*)1,((
_tmp489.f2=env,_tmp489)));}{void*_tmp1F7=rep;int _tmp1F8;unsigned int _tmp1F9;int
_tmp1FA;unsigned int _tmp1FB;int _tmp1FC;unsigned int _tmp1FD;int _tmp1FE;
unsigned int _tmp1FF;int _tmp200;unsigned int _tmp201;int _tmp202;unsigned int
_tmp203;int _tmp204;unsigned int _tmp205;int _tmp206;unsigned int _tmp207;
unsigned int _tmp208;void*_tmp209;void*_tmp20A;struct _dyneither_ptr _tmp20B;struct
_dyneither_ptr _tmp20C;struct _dyneither_ptr _tmp20D;struct _dyneither_ptr _tmp20E;
struct _dyneither_ptr _tmp20F;struct _dyneither_ptr _tmp210;int _tmp211;struct
_dyneither_ptr _tmp212;int _tmp213;_LL90: if(_tmp1F7 <= (void*)2)goto _LLA0;if(*((int*)
_tmp1F7)!= 0)goto _LL92;_tmp1F8=((struct Cyc_Typerep_Int_struct*)_tmp1F7)->f1;if(
_tmp1F8 != 0)goto _LL92;_tmp1F9=((struct Cyc_Typerep_Int_struct*)_tmp1F7)->f2;if(
_tmp1F9 != 8)goto _LL92;_LL91: {unsigned char*x0=((unsigned char*(*)(void*x))
unsafe_cast)(x);unsigned char*y0=((unsigned char*(*)(void*x))unsafe_cast)(y);
struct _tuple11 _tmp48A;return(_tmp48A.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(
void*)0),((_tmp48A.f2=env,_tmp48A)));}_LL92: if(*((int*)_tmp1F7)!= 0)goto _LL94;
_tmp1FA=((struct Cyc_Typerep_Int_struct*)_tmp1F7)->f1;if(_tmp1FA != 1)goto _LL94;
_tmp1FB=((struct Cyc_Typerep_Int_struct*)_tmp1F7)->f2;if(_tmp1FB != 8)goto _LL94;
_LL93: {signed char*x0=((signed char*(*)(void*x))unsafe_cast)(x);signed char*y0=((
signed char*(*)(void*x))unsafe_cast)(y);struct _tuple11 _tmp48B;return(_tmp48B.f1=*
x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0),((_tmp48B.f2=env,_tmp48B)));}
_LL94: if(*((int*)_tmp1F7)!= 0)goto _LL96;_tmp1FC=((struct Cyc_Typerep_Int_struct*)
_tmp1F7)->f1;if(_tmp1FC != 1)goto _LL96;_tmp1FD=((struct Cyc_Typerep_Int_struct*)
_tmp1F7)->f2;if(_tmp1FD != 16)goto _LL96;_LL95: {short*x0=((short*(*)(void*x))
unsafe_cast)(x);short*y0=((short*(*)(void*x))unsafe_cast)(y);struct _tuple11
_tmp48C;return(_tmp48C.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0),((
_tmp48C.f2=env,_tmp48C)));}_LL96: if(*((int*)_tmp1F7)!= 0)goto _LL98;_tmp1FE=((
struct Cyc_Typerep_Int_struct*)_tmp1F7)->f1;if(_tmp1FE != 0)goto _LL98;_tmp1FF=((
struct Cyc_Typerep_Int_struct*)_tmp1F7)->f2;if(_tmp1FF != 16)goto _LL98;_LL97: {
unsigned short*x0=((unsigned short*(*)(void*x))unsafe_cast)(x);unsigned short*y0=((
unsigned short*(*)(void*x))unsafe_cast)(y);struct _tuple11 _tmp48D;return(_tmp48D.f1=*
x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0),((_tmp48D.f2=env,_tmp48D)));}
_LL98: if(*((int*)_tmp1F7)!= 0)goto _LL9A;_tmp200=((struct Cyc_Typerep_Int_struct*)
_tmp1F7)->f1;if(_tmp200 != 1)goto _LL9A;_tmp201=((struct Cyc_Typerep_Int_struct*)
_tmp1F7)->f2;if(_tmp201 != 32)goto _LL9A;_LL99: {int*x0=((int*(*)(void*x))
unsafe_cast)(x);int*y0=((int*(*)(void*x))unsafe_cast)(y);struct _tuple11 _tmp48E;
return(_tmp48E.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0),((_tmp48E.f2=
env,_tmp48E)));}_LL9A: if(*((int*)_tmp1F7)!= 0)goto _LL9C;_tmp202=((struct Cyc_Typerep_Int_struct*)
_tmp1F7)->f1;if(_tmp202 != 0)goto _LL9C;_tmp203=((struct Cyc_Typerep_Int_struct*)
_tmp1F7)->f2;if(_tmp203 != 32)goto _LL9C;_LL9B: {unsigned int*x0=((unsigned int*(*)(
void*x))unsafe_cast)(x);unsigned int*y0=((unsigned int*(*)(void*x))unsafe_cast)(
y);struct _tuple11 _tmp48F;return(_tmp48F.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)
2:(void*)0),((_tmp48F.f2=env,_tmp48F)));}_LL9C: if(*((int*)_tmp1F7)!= 0)goto _LL9E;
_tmp204=((struct Cyc_Typerep_Int_struct*)_tmp1F7)->f1;if(_tmp204 != 1)goto _LL9E;
_tmp205=((struct Cyc_Typerep_Int_struct*)_tmp1F7)->f2;if(_tmp205 != 64)goto _LL9E;
_LL9D: {long long*x0=((long long*(*)(void*x))unsafe_cast)(x);long long*y0=((
long long*(*)(void*x))unsafe_cast)(y);struct _tuple11 _tmp490;return(_tmp490.f1=*
x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0),((_tmp490.f2=env,_tmp490)));}
_LL9E: if(*((int*)_tmp1F7)!= 0)goto _LLA0;_tmp206=((struct Cyc_Typerep_Int_struct*)
_tmp1F7)->f1;if(_tmp206 != 0)goto _LLA0;_tmp207=((struct Cyc_Typerep_Int_struct*)
_tmp1F7)->f2;if(_tmp207 != 64)goto _LLA0;_LL9F: {unsigned long long*x0=((
unsigned long long*(*)(void*x))unsafe_cast)(x);unsigned long long*y0=((
unsigned long long*(*)(void*x))unsafe_cast)(y);struct _tuple11 _tmp491;return(
_tmp491.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0),((_tmp491.f2=env,
_tmp491)));}_LLA0: if((int)_tmp1F7 != 0)goto _LLA2;_LLA1: {float*x0=((float*(*)(
void*x))unsafe_cast)(x);float*y0=((float*(*)(void*x))unsafe_cast)(y);struct
_tuple11 _tmp492;return(_tmp492.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)
0),((_tmp492.f2=env,_tmp492)));}_LLA2: if((int)_tmp1F7 != 1)goto _LLA4;_LLA3: {
double*x0=((double*(*)(void*x))unsafe_cast)(x);double*y0=((double*(*)(void*x))
unsafe_cast)(y);struct _tuple11 _tmp493;return(_tmp493.f1=*x0 == *y0?(void*)1:(*x0
<= *y0?(void*)2:(void*)0),((_tmp493.f2=env,_tmp493)));}_LLA4: if(_tmp1F7 <= (void*)
2)goto _LLB6;if(*((int*)_tmp1F7)!= 1)goto _LLA6;_tmp208=((struct Cyc_Typerep_ThinPtr_struct*)
_tmp1F7)->f1;_tmp209=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp1F7)->f2;
_LLA5: {unsigned int*x0=((unsigned int*(*)(void*x))unsafe_cast)(x);unsigned int*
y0=((unsigned int*(*)(void*x))unsafe_cast)(y);unsigned int addrx=*x0;unsigned int
addry=*y0;struct _tuple10*_tmp494;struct _tuple10*_tmp21E=(_tmp494=
_cycalloc_atomic(sizeof(*_tmp494)),((_tmp494->f1=addrx,((_tmp494->f2=addry,
_tmp494)))));if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple10*k))Cyc_Dict_member)(
env,_tmp21E)){struct _tuple11 _tmp495;return(_tmp495.f1=((void*(*)(struct Cyc_Dict_Dict
d,struct _tuple10*k))Cyc_Dict_lookup)(env,_tmp21E),((_tmp495.f2=env,_tmp495)));}
else{if(addrx == 0){struct _tuple11 _tmp496;return(_tmp496.f1=(void*)2,((_tmp496.f2=
env,_tmp496)));}else{if(addry == 0){struct _tuple11 _tmp497;return(_tmp497.f1=(void*)
0,((_tmp497.f2=env,_tmp497)));}else{env=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _tuple10*k,void*v))Cyc_Dict_insert)(env,_tmp21E,(void*)1);{int typ_szb=(
int)Cyc_Typerep_size_type(_tmp209);int xa=(int)addrx;int ya=(int)addry;void*ord=(
void*)1;{int i=0;for(0;i < _tmp208;++ i){void*_tmp223;struct Cyc_Dict_Dict _tmp224;
struct _tuple11 _tmp222=Cyc_Marshal_leq_eq_type(_tmp209,env,((void*(*)(int x))
unsafe_cast)(xa),((void*(*)(int x))unsafe_cast)(ya));_tmp223=_tmp222.f1;_tmp224=
_tmp222.f2;ord=Cyc_Marshal_order_lex(ord,_tmp223);env=_tmp224;xa +=typ_szb;ya +=
typ_szb;}}{struct _tuple11 _tmp498;return Cyc_Marshal_return_update(_tmp21E,((
_tmp498.f1=ord,((_tmp498.f2=env,_tmp498)))));}}}}}}_LLA6: if(*((int*)_tmp1F7)!= 2)
goto _LLA8;_tmp20A=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp1F7)->f1;_LLA7: {
struct _tuple9*x0=((struct _tuple9*(*)(void*x))unsafe_cast)(x);struct _tuple9*y0=((
struct _tuple9*(*)(void*x))unsafe_cast)(y);unsigned int _tmp228;unsigned int
_tmp229;unsigned int _tmp22A;struct _tuple9 _tmp227=*x0;_tmp228=_tmp227.f1;_tmp229=
_tmp227.f2;_tmp22A=_tmp227.f3;{unsigned int _tmp22C;unsigned int _tmp22D;
unsigned int _tmp22E;struct _tuple9 _tmp22B=*y0;_tmp22C=_tmp22B.f1;_tmp22D=_tmp22B.f2;
_tmp22E=_tmp22B.f3;{int typ_szb=(int)Cyc_Typerep_size_type(_tmp20A);int xpos=(int)((
_tmp229 - _tmp228)/ typ_szb);int ypos=(int)((_tmp22D - _tmp22C)/ typ_szb);if(xpos > 
ypos){struct _tuple11 _tmp499;return(_tmp499.f1=(void*)0,((_tmp499.f2=env,_tmp499)));}{
int xsize=(int)((_tmp22A - _tmp228)/ typ_szb);int ysize=(int)((_tmp22E - _tmp22C)/ 
typ_szb);if(xsize > ysize){struct _tuple11 _tmp49A;return(_tmp49A.f1=(void*)0,((
_tmp49A.f2=env,_tmp49A)));}{struct _tuple10*_tmp49B;struct _tuple10*_tmp231=(
_tmp49B=_cycalloc_atomic(sizeof(*_tmp49B)),((_tmp49B->f1=_tmp228,((_tmp49B->f2=
_tmp22C,_tmp49B)))));if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple10*k))Cyc_Dict_member)(
env,_tmp231)){struct _tuple11 _tmp49C;return(_tmp49C.f1=((void*(*)(struct Cyc_Dict_Dict
d,struct _tuple10*k))Cyc_Dict_lookup)(env,_tmp231),((_tmp49C.f2=env,_tmp49C)));}
else{if(_tmp228 == 0){struct _tuple11 _tmp49D;return(_tmp49D.f1=(void*)2,((_tmp49D.f2=
env,_tmp49D)));}else{if(_tmp22C == 0){struct _tuple11 _tmp49E;return(_tmp49E.f1=(
void*)0,((_tmp49E.f2=env,_tmp49E)));}else{env=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _tuple10*k,void*v))Cyc_Dict_insert)(env,_tmp231,(void*)1);{int xb=(int)
_tmp228;int yb=(int)_tmp22C;void*ord=(void*)1;{int i=0;for(0;i < xsize;++ i){void*
_tmp236;struct Cyc_Dict_Dict _tmp237;struct _tuple11 _tmp235=Cyc_Marshal_leq_eq_type(
_tmp20A,env,((void*(*)(int x))unsafe_cast)(xb),((void*(*)(int x))unsafe_cast)(yb));
_tmp236=_tmp235.f1;_tmp237=_tmp235.f2;ord=Cyc_Marshal_order_lex(ord,_tmp236);env=
_tmp237;xb +=typ_szb;yb +=typ_szb;}}if(ord == (void*)1  && xsize < ysize)ord=(void*)2;{
struct _tuple11 _tmp49F;return Cyc_Marshal_return_update(_tmp231,((_tmp49F.f1=ord,((
_tmp49F.f2=env,_tmp49F)))));}}}}}}}}}}_LLA8: if(*((int*)_tmp1F7)!= 4)goto _LLAA;
_tmp20B=((struct Cyc_Typerep_Tuple_struct*)_tmp1F7)->f2;_LLA9: {int x0=((int(*)(
void*x))unsafe_cast)(x);int y0=((int(*)(void*x))unsafe_cast)(y);void*ord=(void*)1;{
int i=0;for(0;i < _get_dyneither_size(_tmp20B,sizeof(struct _tuple1*));++ i){
unsigned int _tmp23B;void*_tmp23C;struct _tuple1 _tmp23A=*(*((struct _tuple1**)
_check_dyneither_subscript(_tmp20B,sizeof(struct _tuple1*),i)));_tmp23B=_tmp23A.f1;
_tmp23C=_tmp23A.f2;{int x_ofs=(int)(x0 + _tmp23B);int y_ofs=(int)(y0 + _tmp23B);void*
_tmp23E;struct Cyc_Dict_Dict _tmp23F;struct _tuple11 _tmp23D=Cyc_Marshal_leq_eq_type(
_tmp23C,env,((void*(*)(int x))unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(
y_ofs));_tmp23E=_tmp23D.f1;_tmp23F=_tmp23D.f2;ord=Cyc_Marshal_order_lex(ord,
_tmp23E);env=_tmp23F;}}}{struct _tuple11 _tmp4A0;return(_tmp4A0.f1=ord,((_tmp4A0.f2=
env,_tmp4A0)));}}_LLAA: if(*((int*)_tmp1F7)!= 3)goto _LLAC;_tmp20C=((struct Cyc_Typerep_Struct_struct*)
_tmp1F7)->f3;_LLAB: {int x0=((int(*)(void*x))unsafe_cast)(x);int y0=((int(*)(void*
x))unsafe_cast)(y);void*ord=(void*)1;{int i=0;for(0;i < _get_dyneither_size(
_tmp20C,sizeof(struct _tuple0*));++ i){unsigned int _tmp242;void*_tmp243;struct
_tuple0 _tmp241=*(*((struct _tuple0**)_check_dyneither_subscript(_tmp20C,sizeof(
struct _tuple0*),i)));_tmp242=_tmp241.f1;_tmp243=_tmp241.f3;{int x_ofs=(int)(x0 + 
_tmp242);int y_ofs=(int)(y0 + _tmp242);void*_tmp245;struct Cyc_Dict_Dict _tmp246;
struct _tuple11 _tmp244=Cyc_Marshal_leq_eq_type(_tmp243,env,((void*(*)(int x))
unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));_tmp245=_tmp244.f1;
_tmp246=_tmp244.f2;ord=Cyc_Marshal_order_lex(ord,_tmp245);env=_tmp246;}}}{struct
_tuple11 _tmp4A1;return(_tmp4A1.f1=ord,((_tmp4A1.f2=env,_tmp4A1)));}}_LLAC: if(*((
int*)_tmp1F7)!= 6)goto _LLAE;_tmp20D=((struct Cyc_Typerep_TUnionField_struct*)
_tmp1F7)->f4;_LLAD: {int x0=((int(*)(void*x))unsafe_cast)(x);int y0=((int(*)(void*
x))unsafe_cast)(y);void*ord=(void*)1;{int i=1;for(0;i < _get_dyneither_size(
_tmp20D,sizeof(struct _tuple1*));++ i){unsigned int _tmp249;void*_tmp24A;struct
_tuple1 _tmp248=*(*((struct _tuple1**)_check_dyneither_subscript(_tmp20D,sizeof(
struct _tuple1*),i)));_tmp249=_tmp248.f1;_tmp24A=_tmp248.f2;{int x_ofs=(int)(x0 + 
_tmp249);int y_ofs=(int)(y0 + _tmp249);void*_tmp24C;struct Cyc_Dict_Dict _tmp24D;
struct _tuple11 _tmp24B=Cyc_Marshal_leq_eq_type(_tmp24A,env,((void*(*)(int x))
unsafe_cast)(x_ofs),((void*(*)(int x))unsafe_cast)(y_ofs));_tmp24C=_tmp24B.f1;
_tmp24D=_tmp24B.f2;ord=Cyc_Marshal_order_lex(ord,_tmp24C);env=_tmp24D;}}}{struct
_tuple11 _tmp4A2;return(_tmp4A2.f1=ord,((_tmp4A2.f2=env,_tmp4A2)));}}_LLAE: if(*((
int*)_tmp1F7)!= 5)goto _LLB0;_tmp20E=((struct Cyc_Typerep_TUnion_struct*)_tmp1F7)->f2;
_tmp20F=((struct Cyc_Typerep_TUnion_struct*)_tmp1F7)->f3;_LLAF: {unsigned int*x0=((
unsigned int*(*)(void*x))unsafe_cast)(x);unsigned int*y0=((unsigned int*(*)(void*
x))unsafe_cast)(y);if(*x0 < 1024  && *y0 < 1024){struct _tuple11 _tmp4A3;return(
_tmp4A3.f1=*x0 == *y0?(void*)1:(*x0 <= *y0?(void*)2:(void*)0),((_tmp4A3.f2=env,
_tmp4A3)));}else{if(*x0 < 1024){struct _tuple11 _tmp4A4;return(_tmp4A4.f1=(void*)2,((
_tmp4A4.f2=env,_tmp4A4)));}else{if(*y0 < 1024){struct _tuple11 _tmp4A5;return(
_tmp4A5.f1=(void*)0,((_tmp4A5.f2=env,_tmp4A5)));}else{struct _tuple10*_tmp4A6;
struct _tuple10*_tmp252=(_tmp4A6=_cycalloc_atomic(sizeof(*_tmp4A6)),((_tmp4A6->f1=*
x0,((_tmp4A6->f2=*y0,_tmp4A6)))));if(((int(*)(struct Cyc_Dict_Dict d,struct
_tuple10*k))Cyc_Dict_member)(env,_tmp252)){struct _tuple11 _tmp4A7;return(_tmp4A7.f1=((
void*(*)(struct Cyc_Dict_Dict d,struct _tuple10*k))Cyc_Dict_lookup)(env,_tmp252),((
_tmp4A7.f2=env,_tmp4A7)));}else{env=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _tuple10*k,void*v))Cyc_Dict_insert)(env,_tmp252,(void*)1);{unsigned int*
xtagp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*x0);unsigned int*ytagp=((
unsigned int*(*)(unsigned int x))unsafe_cast)(*y0);if(*xtagp != *ytagp){struct
_tuple11 _tmp4A8;return Cyc_Marshal_return_update(_tmp252,((_tmp4A8.f1=*xtagp == *
ytagp?(void*)1:(*xtagp <= *ytagp?(void*)2:(void*)0),((_tmp4A8.f2=env,_tmp4A8)))));}
else{int _tmp256;void*_tmp257;struct _tuple4 _tmp255=Cyc_Typerep_get_unionbranch(*
xtagp,_tmp20F);_tmp256=_tmp255.f1;_tmp257=_tmp255.f2;{void*xoff=((void*(*)(
unsigned int x))unsafe_cast)(*x0 + _tmp256);void*yoff=((void*(*)(unsigned int x))
unsafe_cast)(*y0 + _tmp256);return Cyc_Marshal_return_update(_tmp252,Cyc_Marshal_leq_eq_type(
_tmp257,env,xoff,yoff));}}}}}}}}_LLB0: if(*((int*)_tmp1F7)!= 7)goto _LLB2;_tmp210=((
struct Cyc_Typerep_XTUnion_struct*)_tmp1F7)->f2;_LLB1: {unsigned int*x0=((
unsigned int*(*)(void*x))unsafe_cast)(x);unsigned int*y0=((unsigned int*(*)(void*
x))unsafe_cast)(y);struct _tuple10*_tmp4A9;struct _tuple10*_tmp259=(_tmp4A9=
_cycalloc_atomic(sizeof(*_tmp4A9)),((_tmp4A9->f1=*x0,((_tmp4A9->f2=*y0,_tmp4A9)))));
if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple10*k))Cyc_Dict_member)(env,_tmp259)){
struct _tuple11 _tmp4AA;return(_tmp4AA.f1=((void*(*)(struct Cyc_Dict_Dict d,struct
_tuple10*k))Cyc_Dict_lookup)(env,_tmp259),((_tmp4AA.f2=env,_tmp4AA)));}else{{
struct _tuple10*_tmp4AB;env=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple10*k,void*v))Cyc_Dict_insert)(env,((_tmp4AB=_cycalloc_atomic(sizeof(*
_tmp4AB)),((_tmp4AB->f1=*x0,((_tmp4AB->f2=*y0,_tmp4AB)))))),(void*)1);}{
unsigned int*xp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*x0);unsigned int*
yp=((unsigned int*(*)(unsigned int x))unsafe_cast)(*y0);if(*xp == 0  && *yp == 0){
char*xnamec=((char*(*)(unsigned int x))unsafe_cast)(*x0 + 4);char*ynamec=((char*(*)(
unsigned int x))unsafe_cast)(*y0 + 4);struct _dyneither_ptr xname=(struct
_dyneither_ptr)Cstring_to_string(xnamec);struct _dyneither_ptr yname=(struct
_dyneither_ptr)Cstring_to_string(ynamec);int cmp=Cyc_strcmp((struct _dyneither_ptr)
xname,(struct _dyneither_ptr)yname);if(cmp < 0){struct _tuple11 _tmp4AC;return Cyc_Marshal_return_update(
_tmp259,((_tmp4AC.f1=(void*)0,((_tmp4AC.f2=env,_tmp4AC)))));}else{if(cmp == 0){
struct _tuple11 _tmp4AD;return Cyc_Marshal_return_update(_tmp259,((_tmp4AD.f1=(void*)
1,((_tmp4AD.f2=env,_tmp4AD)))));}else{struct _tuple11 _tmp4AE;return Cyc_Marshal_return_update(
_tmp259,((_tmp4AE.f1=(void*)2,((_tmp4AE.f2=env,_tmp4AE)))));}}}else{if(*xp == 0){
struct _tuple11 _tmp4AF;return Cyc_Marshal_return_update(_tmp259,((_tmp4AF.f1=(void*)
2,((_tmp4AF.f2=env,_tmp4AF)))));}else{if(*yp == 0){struct _tuple11 _tmp4B0;return
Cyc_Marshal_return_update(_tmp259,((_tmp4B0.f1=(void*)0,((_tmp4B0.f2=env,_tmp4B0)))));}
else{char*xnamec=((char*(*)(unsigned int x))unsafe_cast)(*xp + 4);char*ynamec=((
char*(*)(unsigned int x))unsafe_cast)(*yp + 4);struct _dyneither_ptr xname=(struct
_dyneither_ptr)Cstring_to_string(xnamec);struct _dyneither_ptr yname=(struct
_dyneither_ptr)Cstring_to_string(ynamec);int cmp=Cyc_strcmp((struct _dyneither_ptr)
xname,(struct _dyneither_ptr)yname);if(cmp < 0){struct _tuple11 _tmp4B1;return Cyc_Marshal_return_update(
_tmp259,((_tmp4B1.f1=(void*)0,((_tmp4B1.f2=env,_tmp4B1)))));}else{if(cmp == 0){
int _tmp263;void*_tmp264;struct _tuple4 _tmp262=Cyc_Typerep_get_xtunionbranch(xname,
_tmp210);_tmp263=_tmp262.f1;_tmp264=_tmp262.f2;{void*xoff=((void*(*)(
unsigned int x))unsafe_cast)(*x0 + _tmp263);void*yoff=((void*(*)(unsigned int x))
unsafe_cast)(*y0 + _tmp263);return Cyc_Marshal_return_update(_tmp259,Cyc_Marshal_leq_eq_type(
_tmp264,env,xoff,yoff));}}else{struct _tuple11 _tmp4B2;return Cyc_Marshal_return_update(
_tmp259,((_tmp4B2.f1=(void*)2,((_tmp4B2.f2=env,_tmp4B2)))));}}}}}}}}_LLB2: if(*((
int*)_tmp1F7)!= 8)goto _LLB4;_tmp211=((struct Cyc_Typerep_Union_struct*)_tmp1F7)->f2;
_tmp212=((struct Cyc_Typerep_Union_struct*)_tmp1F7)->f3;_LLB3: {struct Cyc_Typerep_Tuple_struct
_tmp4CB;unsigned int _tmp4CA;unsigned int _tmp4C9;struct _dyneither_ptr _tmp4C8;
struct _tuple1**_tmp4C7;unsigned int _tmp4C6;struct Cyc_Typerep_Tuple_struct*
_tmp4C5;return Cyc_Marshal_leq_eq_type((void*)((_tmp4C5=_cycalloc(sizeof(*_tmp4C5)),((
_tmp4C5[0]=((_tmp4CB.tag=4,((_tmp4CB.f1=(unsigned int)_tmp211,((_tmp4CB.f2=((
_tmp4C6=(unsigned int)_tmp211,((_tmp4C7=(struct _tuple1**)_cycalloc(_check_times(
sizeof(struct _tuple1*),_tmp4C6)),((_tmp4C8=_tag_dyneither(_tmp4C7,sizeof(struct
_tuple1*),_tmp4C6),((((_tmp4C9=_tmp4C6,_tmp4BE(& _tmp4CA,& _tmp4C9,& _tmp4C7))),
_tmp4C8)))))))),_tmp4CB)))))),_tmp4C5)))),env,x,y);}_LLB4: if(*((int*)_tmp1F7)!= 
9)goto _LLB6;_tmp213=((struct Cyc_Typerep_Enum_struct*)_tmp1F7)->f2;_LLB5: {struct
Cyc_Typerep_Int_struct _tmp4CE;struct Cyc_Typerep_Int_struct*_tmp4CD;return Cyc_Marshal_leq_eq_type((
void*)((_tmp4CD=_cycalloc_atomic(sizeof(*_tmp4CD)),((_tmp4CD[0]=((_tmp4CE.tag=0,((
_tmp4CE.f1=0,((_tmp4CE.f2=(unsigned int)(_tmp213 << 3),_tmp4CE)))))),_tmp4CD)))),
env,x,y);}_LLB6:;_LLB7: {struct Cyc_Core_Failure_struct _tmp4D4;const char*_tmp4D3;
struct Cyc_Core_Failure_struct*_tmp4D2;(int)_throw((void*)((_tmp4D2=_cycalloc(
sizeof(*_tmp4D2)),((_tmp4D2[0]=((_tmp4D4.tag=Cyc_Core_Failure,((_tmp4D4.f1=((
_tmp4D3="leq_eq_type case unmatched",_tag_dyneither(_tmp4D3,sizeof(char),27))),
_tmp4D4)))),_tmp4D2)))));}_LL8F:;}}int Cyc_Marshal_eq_type(void*ts,void*x,void*y);
int Cyc_Marshal_eq_type(void*ts,void*x,void*y){struct _handler_cons _tmp275;
_push_handler(& _tmp275);{int _tmp277=0;if(setjmp(_tmp275.handler))_tmp277=1;if(!
_tmp277){{void*_tmp279;struct _tuple11 _tmp278=Cyc_Marshal_leq_eq_type(ts,Cyc_Marshal_empty_eq_dict(),
x,y);_tmp279=_tmp278.f1;{int _tmp27A=_tmp279 == (void*)1;_npop_handler(0);return
_tmp27A;}};_pop_handler();}else{void*_tmp276=(void*)_exn_thrown;void*_tmp27C=
_tmp276;struct _dyneither_ptr _tmp27D;_LLB9: if(*((void**)_tmp27C)!= Cyc_Core_Failure)
goto _LLBB;_tmp27D=((struct Cyc_Core_Failure_struct*)_tmp27C)->f1;_LLBA:{const char*
_tmp4D8;void*_tmp4D7[1];struct Cyc_String_pa_struct _tmp4D6;(_tmp4D6.tag=0,((
_tmp4D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp27D),((_tmp4D7[0]=&
_tmp4D6,Cyc_fprintf(Cyc_stderr,((_tmp4D8="Core::Failure(%s)\n",_tag_dyneither(
_tmp4D8,sizeof(char),19))),_tag_dyneither(_tmp4D7,sizeof(void*),1)))))));}{
struct Cyc_Core_Failure_struct _tmp4DB;struct Cyc_Core_Failure_struct*_tmp4DA;(int)
_throw((void*)((_tmp4DA=_cycalloc(sizeof(*_tmp4DA)),((_tmp4DA[0]=((_tmp4DB.tag=
Cyc_Core_Failure,((_tmp4DB.f1=_tmp27D,_tmp4DB)))),_tmp4DA)))));}_LLBB:;_LLBC:(
void)_throw(_tmp27C);_LLB8:;}}}int Cyc_Marshal_leq_type(void*ts,void*x,void*y);
int Cyc_Marshal_leq_type(void*ts,void*x,void*y){struct _handler_cons _tmp283;
_push_handler(& _tmp283);{int _tmp285=0;if(setjmp(_tmp283.handler))_tmp285=1;if(!
_tmp285){{void*_tmp287;struct _tuple11 _tmp286=Cyc_Marshal_leq_eq_type(ts,Cyc_Marshal_empty_eq_dict(),
x,y);_tmp287=_tmp286.f1;{int _tmp288=_tmp287 != (void*)0;_npop_handler(0);return
_tmp288;}};_pop_handler();}else{void*_tmp284=(void*)_exn_thrown;void*_tmp28A=
_tmp284;struct _dyneither_ptr _tmp28B;_LLBE: if(*((void**)_tmp28A)!= Cyc_Core_Failure)
goto _LLC0;_tmp28B=((struct Cyc_Core_Failure_struct*)_tmp28A)->f1;_LLBF:{const char*
_tmp4DF;void*_tmp4DE[1];struct Cyc_String_pa_struct _tmp4DD;(_tmp4DD.tag=0,((
_tmp4DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp28B),((_tmp4DE[0]=&
_tmp4DD,Cyc_fprintf(Cyc_stderr,((_tmp4DF="Core::Failure(%s)\n",_tag_dyneither(
_tmp4DF,sizeof(char),19))),_tag_dyneither(_tmp4DE,sizeof(void*),1)))))));}{
struct Cyc_Core_Failure_struct _tmp4E2;struct Cyc_Core_Failure_struct*_tmp4E1;(int)
_throw((void*)((_tmp4E1=_cycalloc(sizeof(*_tmp4E1)),((_tmp4E1[0]=((_tmp4E2.tag=
Cyc_Core_Failure,((_tmp4E2.f1=_tmp28B,_tmp4E2)))),_tmp4E1)))));}_LLC0:;_LLC1:(
void)_throw(_tmp28A);_LLBD:;}}}int Cyc_Marshal_cmp_type(void*ts,void*x,void*y);
int Cyc_Marshal_cmp_type(void*ts,void*x,void*y){struct _handler_cons _tmp291;
_push_handler(& _tmp291);{int _tmp293=0;if(setjmp(_tmp291.handler))_tmp293=1;if(!
_tmp293){{void*_tmp295;struct _tuple11 _tmp294=Cyc_Marshal_leq_eq_type(ts,Cyc_Marshal_empty_eq_dict(),
x,y);_tmp295=_tmp294.f1;{void*_tmp296=_tmp295;_LLC3: if((int)_tmp296 != 0)goto
_LLC5;_LLC4: {int _tmp297=- 1;_npop_handler(0);return _tmp297;}_LLC5: if((int)
_tmp296 != 1)goto _LLC7;_LLC6: {int _tmp298=0;_npop_handler(0);return _tmp298;}_LLC7:
if((int)_tmp296 != 2)goto _LLC2;_LLC8: {int _tmp299=1;_npop_handler(0);return
_tmp299;}_LLC2:;}};_pop_handler();}else{void*_tmp292=(void*)_exn_thrown;void*
_tmp29B=_tmp292;struct _dyneither_ptr _tmp29C;_LLCA: if(*((void**)_tmp29B)!= Cyc_Core_Failure)
goto _LLCC;_tmp29C=((struct Cyc_Core_Failure_struct*)_tmp29B)->f1;_LLCB:{const char*
_tmp4E6;void*_tmp4E5[1];struct Cyc_String_pa_struct _tmp4E4;(_tmp4E4.tag=0,((
_tmp4E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp29C),((_tmp4E5[0]=&
_tmp4E4,Cyc_fprintf(Cyc_stderr,((_tmp4E6="Core::Failure(%s)\n",_tag_dyneither(
_tmp4E6,sizeof(char),19))),_tag_dyneither(_tmp4E5,sizeof(void*),1)))))));}{
struct Cyc_Core_Failure_struct _tmp4E9;struct Cyc_Core_Failure_struct*_tmp4E8;(int)
_throw((void*)((_tmp4E8=_cycalloc(sizeof(*_tmp4E8)),((_tmp4E8[0]=((_tmp4E9.tag=
Cyc_Core_Failure,((_tmp4E9.f1=_tmp29C,_tmp4E9)))),_tmp4E8)))));}_LLCC:;_LLCD:(
void)_throw(_tmp29B);_LLC9:;}}}
