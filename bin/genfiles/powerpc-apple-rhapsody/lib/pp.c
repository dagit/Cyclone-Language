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
 struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct Cyc_Core_Opt{
void*v;};struct _dyneither_ptr Cyc_Core_new_string(unsigned int);void*Cyc_Core_identity(
void*);int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc___cycFILE;
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
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];extern char Cyc_List_Nth[8];unsigned long Cyc_strlen(struct _dyneither_ptr s);int
Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);struct _dyneither_ptr
Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);struct Cyc_Hashtable_Table;
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*
hash)(void*));void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*
val);void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);int Cyc_Hashtable_hash_stringptr(
struct _dyneither_ptr*p);struct Cyc_Fn_Function{void*(*f)(void*,void*);void*env;};
struct Cyc_Fn_Function*Cyc_Fn_make_fn(void*(*f)(void*,void*),void*x);struct Cyc_Fn_Function*
Cyc_Fn_fp2fn(void*(*f)(void*));void*Cyc_Fn_apply(struct Cyc_Fn_Function*f,void*x);
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);struct
_dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);struct _tuple0{struct
_dyneither_ptr f1;struct Cyc_List_List*f2;};struct _tuple0*Cyc_PP_string_and_links(
struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();
struct Cyc_PP_Doc*Cyc_PP_line_doc();struct Cyc_PP_Doc*Cyc_PP_oline_doc();struct Cyc_PP_Doc*
Cyc_PP_text(struct _dyneither_ptr s);struct Cyc_PP_Doc*Cyc_PP_textptr(struct
_dyneither_ptr*p);struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int w);
struct Cyc_PP_Doc*Cyc_PP_hyperlink(struct _dyneither_ptr shrt,struct _dyneither_ptr
full);struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);struct Cyc_PP_Doc*Cyc_PP_cat(
struct _dyneither_ptr);struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*doclist);
struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _dyneither_ptr docs);struct Cyc_PP_Doc*Cyc_PP_doc_union(
struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2);struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*
d);struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l);
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr
sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,
struct Cyc_List_List*l0);struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(
void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_group(
struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,
struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_groupl(struct _dyneither_ptr start,
struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);struct
Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr stop,
struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_Xarray_Xarray{struct
_RegionHandle*r;struct _dyneither_ptr elmts;int num_elmts;};int Cyc_Xarray_length(
struct Cyc_Xarray_Xarray*);void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);void
Cyc_Xarray_set(struct Cyc_Xarray_Xarray*,int,void*);struct Cyc_Xarray_Xarray*Cyc_Xarray_create(
int,void*);void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);struct Cyc_PP_Single_struct{
int tag;void*f1;};struct Cyc_PP_Append_struct{int tag;void*f1;void*f2;};void*Cyc_PP_append(
void*a1,void*a2);void*Cyc_PP_append(void*a1,void*a2){if(a1 == (void*)((void*)0))
return a2;else{if(a2 == (void*)((void*)0))return a1;else{struct Cyc_PP_Append_struct
_tmpA3;struct Cyc_PP_Append_struct*_tmpA2;return(void*)((_tmpA2=_cycalloc(sizeof(*
_tmpA2)),((_tmpA2[0]=((_tmpA3.tag=1,((_tmpA3.f1=(void*)a1,((_tmpA3.f2=(void*)a2,
_tmpA3)))))),_tmpA2))));}}}struct Cyc_List_List*Cyc_PP_list_of_alist_f(void*y,
struct Cyc_List_List*l);struct Cyc_List_List*Cyc_PP_list_of_alist_f(void*y,struct
Cyc_List_List*l){void*_tmp2=y;void*_tmp3;void*_tmp4;void*_tmp5;_LL1: if((int)
_tmp2 != 0)goto _LL3;_LL2: return l;_LL3: if(_tmp2 <= (void*)1)goto _LL5;if(*((int*)
_tmp2)!= 0)goto _LL5;_tmp3=(void*)((struct Cyc_PP_Single_struct*)_tmp2)->f1;_LL4: {
struct Cyc_List_List*_tmpA4;return(_tmpA4=_cycalloc(sizeof(*_tmpA4)),((_tmpA4->hd=(
void*)_tmp3,((_tmpA4->tl=l,_tmpA4)))));}_LL5: if(_tmp2 <= (void*)1)goto _LL0;if(*((
int*)_tmp2)!= 1)goto _LL0;_tmp4=(void*)((struct Cyc_PP_Append_struct*)_tmp2)->f1;
_tmp5=(void*)((struct Cyc_PP_Append_struct*)_tmp2)->f2;_LL6: return Cyc_PP_list_of_alist_f(
_tmp4,Cyc_PP_list_of_alist_f(_tmp5,l));_LL0:;}struct Cyc_List_List*Cyc_PP_list_of_alist(
void*x);struct Cyc_List_List*Cyc_PP_list_of_alist(void*x){return Cyc_PP_list_of_alist_f(
x,0);}struct Cyc_PP_Ppstate{int ci;int cc;int cl;int pw;int epw;};struct Cyc_PP_Out{int
newcc;int newcl;void*ppout;void*links;};struct Cyc_PP_Doc{int mwo;int mw;struct Cyc_Fn_Function*
f;};static void Cyc_PP_dump_out(struct Cyc___cycFILE*f,void*al);static void Cyc_PP_dump_out(
struct Cyc___cycFILE*f,void*al){struct Cyc_Xarray_Xarray*_tmp7=Cyc_Xarray_create(
16,al);Cyc_Xarray_add(_tmp7,al);{int last=0;while(last >= 0){void*_tmp8=Cyc_Xarray_get(
_tmp7,last);struct _dyneither_ptr*_tmp9;void*_tmpA;void*_tmpB;_LL8: if((int)_tmp8
!= 0)goto _LLA;_LL9: -- last;goto _LL7;_LLA: if(_tmp8 <= (void*)1)goto _LLC;if(*((int*)
_tmp8)!= 0)goto _LLC;_tmp9=(struct _dyneither_ptr*)((struct Cyc_PP_Single_struct*)
_tmp8)->f1;_LLB: -- last;{const char*_tmpA8;void*_tmpA7[1];struct Cyc_String_pa_struct
_tmpA6;(_tmpA6.tag=0,((_tmpA6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp9),((_tmpA7[0]=& _tmpA6,Cyc_fprintf(f,((_tmpA8="%s",_tag_dyneither(_tmpA8,
sizeof(char),3))),_tag_dyneither(_tmpA7,sizeof(void*),1)))))));}goto _LL7;_LLC:
if(_tmp8 <= (void*)1)goto _LL7;if(*((int*)_tmp8)!= 1)goto _LL7;_tmpA=(void*)((
struct Cyc_PP_Append_struct*)_tmp8)->f1;_tmpB=(void*)((struct Cyc_PP_Append_struct*)
_tmp8)->f2;_LLD: Cyc_Xarray_set(_tmp7,last,_tmpB);if(last == Cyc_Xarray_length(
_tmp7)- 1)Cyc_Xarray_add(_tmp7,_tmpA);else{Cyc_Xarray_set(_tmp7,last + 1,_tmpA);}
++ last;goto _LL7;_LL7:;}}}void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct
Cyc___cycFILE*f);void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*
f){struct Cyc_PP_Ppstate*_tmpA9;struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct
Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,((_tmpA9=
_cycalloc_atomic(sizeof(*_tmpA9)),((_tmpA9->ci=0,((_tmpA9->cc=0,((_tmpA9->cl=1,((
_tmpA9->pw=w,((_tmpA9->epw=w,_tmpA9)))))))))))));Cyc_PP_dump_out(f,(void*)o->ppout);}
struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);struct
_dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w){struct Cyc_PP_Ppstate*
_tmpAA;struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct
Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,((_tmpAA=_cycalloc_atomic(sizeof(*_tmpAA)),((
_tmpAA->ci=0,((_tmpAA->cc=0,((_tmpAA->cl=1,((_tmpAA->pw=w,((_tmpAA->epw=w,_tmpAA)))))))))))));
const char*_tmpAB;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(
void*x))Cyc_PP_list_of_alist)((void*)o->ppout),((_tmpAB="",_tag_dyneither(_tmpAB,
sizeof(char),1))));}struct _tuple0*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int
w);struct _tuple0*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w){struct Cyc_PP_Ppstate*
_tmpAC;struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct
Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,((_tmpAC=_cycalloc_atomic(sizeof(*_tmpAC)),((
_tmpAC->ci=0,((_tmpAC->cc=0,((_tmpAC->cl=1,((_tmpAC->pw=w,((_tmpAC->epw=w,_tmpAC)))))))))))));
const char*_tmpAF;struct _tuple0*_tmpAE;return(_tmpAE=_cycalloc(sizeof(*_tmpAE)),((
_tmpAE->f1=(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(void*x))
Cyc_PP_list_of_alist)((void*)o->ppout),((_tmpAF="",_tag_dyneither(_tmpAF,sizeof(
char),1)))),((_tmpAE->f2=((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)((
void*)o->links),_tmpAE)))));}static struct Cyc_Core_Opt*Cyc_PP_bhashtbl=0;int Cyc_PP_tex_output=
0;struct _dyneither_ptr Cyc_PP_nlblanks(int i);struct _dyneither_ptr Cyc_PP_nlblanks(
int i){if(Cyc_PP_bhashtbl == 0){struct Cyc_Core_Opt*_tmpB0;Cyc_PP_bhashtbl=((_tmpB0=
_cycalloc(sizeof(*_tmpB0)),((_tmpB0->v=((struct Cyc_Hashtable_Table*(*)(int sz,int(*
cmp)(int,int),int(*hash)(int)))Cyc_Hashtable_create)(61,Cyc_Core_intcmp,(int(*)(
int))Cyc_Core_identity),_tmpB0))));}if(i < 0){struct Cyc_Core_Failure_struct _tmpB6;
const char*_tmpB5;struct Cyc_Core_Failure_struct*_tmpB4;(int)_throw((void*)((
_tmpB4=_cycalloc(sizeof(*_tmpB4)),((_tmpB4[0]=((_tmpB6.tag=Cyc_Core_Failure,((
_tmpB6.f1=((_tmpB5="nlblanks",_tag_dyneither(_tmpB5,sizeof(char),9))),_tmpB6)))),
_tmpB4)))));}{struct _handler_cons _tmp19;_push_handler(& _tmp19);{int _tmp1B=0;if(
setjmp(_tmp19.handler))_tmp1B=1;if(!_tmp1B){{struct _dyneither_ptr _tmp1C=*((
struct _dyneither_ptr*(*)(struct Cyc_Hashtable_Table*t,int key))Cyc_Hashtable_lookup)((
struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,
i);_npop_handler(0);return _tmp1C;};_pop_handler();}else{void*_tmp1A=(void*)
_exn_thrown;void*_tmp1E=_tmp1A;_LLF: if(_tmp1E != Cyc_Core_Not_found)goto _LL11;
_LL10: if(!Cyc_PP_tex_output){int _tmp1F=i / 8;int _tmp20=i % 8;int _tmp21=(2 + _tmp1F)
+ _tmp20;struct _dyneither_ptr nlb=Cyc_Core_new_string((unsigned int)_tmp21);{char
_tmpB9;char _tmpB8;struct _dyneither_ptr _tmpB7;(_tmpB7=_dyneither_ptr_plus(nlb,
sizeof(char),0),((_tmpB8=*((char*)_check_dyneither_subscript(_tmpB7,sizeof(char),
0)),((_tmpB9='\n',((_get_dyneither_size(_tmpB7,sizeof(char))== 1  && (_tmpB8 == '\000'
 && _tmpB9 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpB7.curr)=_tmpB9)))))));}{
int j=0;for(0;j < _tmp1F;++ j){char _tmpBC;char _tmpBB;struct _dyneither_ptr _tmpBA;(
_tmpBA=_dyneither_ptr_plus(nlb,sizeof(char),j + 1),((_tmpBB=*((char*)
_check_dyneither_subscript(_tmpBA,sizeof(char),0)),((_tmpBC='\t',((
_get_dyneither_size(_tmpBA,sizeof(char))== 1  && (_tmpBB == '\000'  && _tmpBC != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpBA.curr)=_tmpBC)))))));}}{int j=0;for(0;j < 
_tmp20;++ j){char _tmpBF;char _tmpBE;struct _dyneither_ptr _tmpBD;(_tmpBD=
_dyneither_ptr_plus(nlb,sizeof(char),(j + 1)+ _tmp1F),((_tmpBE=*((char*)
_check_dyneither_subscript(_tmpBD,sizeof(char),0)),((_tmpBF=' ',((
_get_dyneither_size(_tmpBD,sizeof(char))== 1  && (_tmpBE == '\000'  && _tmpBF != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpBD.curr)=_tmpBF)))))));}}{struct
_dyneither_ptr*_tmpC0;((void(*)(struct Cyc_Hashtable_Table*t,int key,struct
_dyneither_ptr*val))Cyc_Hashtable_insert)((struct Cyc_Hashtable_Table*)((struct
Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,i,((_tmpC0=_cycalloc(sizeof(
struct _dyneither_ptr)* 1),((_tmpC0[0]=(struct _dyneither_ptr)nlb,_tmpC0)))));}
return(struct _dyneither_ptr)nlb;}else{int _tmp2C=3 + i;struct _dyneither_ptr nlb=Cyc_Core_new_string((
unsigned int)(_tmp2C + 1));{char _tmpC3;char _tmpC2;struct _dyneither_ptr _tmpC1;(
_tmpC1=_dyneither_ptr_plus(nlb,sizeof(char),0),((_tmpC2=*((char*)
_check_dyneither_subscript(_tmpC1,sizeof(char),0)),((_tmpC3='\\',((
_get_dyneither_size(_tmpC1,sizeof(char))== 1  && (_tmpC2 == '\000'  && _tmpC3 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpC1.curr)=_tmpC3)))))));}{char _tmpC6;char
_tmpC5;struct _dyneither_ptr _tmpC4;(_tmpC4=_dyneither_ptr_plus(nlb,sizeof(char),1),((
_tmpC5=*((char*)_check_dyneither_subscript(_tmpC4,sizeof(char),0)),((_tmpC6='\\',((
_get_dyneither_size(_tmpC4,sizeof(char))== 1  && (_tmpC5 == '\000'  && _tmpC6 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpC4.curr)=_tmpC6)))))));}{char _tmpC9;char
_tmpC8;struct _dyneither_ptr _tmpC7;(_tmpC7=_dyneither_ptr_plus(nlb,sizeof(char),2),((
_tmpC8=*((char*)_check_dyneither_subscript(_tmpC7,sizeof(char),0)),((_tmpC9='\n',((
_get_dyneither_size(_tmpC7,sizeof(char))== 1  && (_tmpC8 == '\000'  && _tmpC9 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpC7.curr)=_tmpC9)))))));}{int j=3;for(0;j < 
_tmp2C;++ j){char _tmpCC;char _tmpCB;struct _dyneither_ptr _tmpCA;(_tmpCA=
_dyneither_ptr_plus(nlb,sizeof(char),j),((_tmpCB=*((char*)
_check_dyneither_subscript(_tmpCA,sizeof(char),0)),((_tmpCC='~',((
_get_dyneither_size(_tmpCA,sizeof(char))== 1  && (_tmpCB == '\000'  && _tmpCC != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpCA.curr)=_tmpCC)))))));}}{struct
_dyneither_ptr*_tmpCD;((void(*)(struct Cyc_Hashtable_Table*t,int key,struct
_dyneither_ptr*val))Cyc_Hashtable_insert)((struct Cyc_Hashtable_Table*)((struct
Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,i,((_tmpCD=_cycalloc(sizeof(
struct _dyneither_ptr)* 1),((_tmpCD[0]=(struct _dyneither_ptr)nlb,_tmpCD)))));}
return(struct _dyneither_ptr)nlb;}_LL11:;_LL12:(void)_throw(_tmp1E);_LLE:;}}}}
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl=0;int Cyc_PP_infinity=9999999;struct
_tuple1{int f1;struct _dyneither_ptr f2;};static struct Cyc_PP_Out*Cyc_PP_text_doc_f(
struct _tuple1*clo,struct Cyc_PP_Ppstate*st);static struct Cyc_PP_Out*Cyc_PP_text_doc_f(
struct _tuple1*clo,struct Cyc_PP_Ppstate*st){struct _tuple1 _tmp3B;int _tmp3C;struct
_dyneither_ptr _tmp3D;struct _tuple1*_tmp3A=clo;_tmp3B=*_tmp3A;_tmp3C=_tmp3B.f1;
_tmp3D=_tmp3B.f2;{struct Cyc_PP_Single_struct*_tmpD7;struct _dyneither_ptr*_tmpD6;
struct Cyc_PP_Single_struct _tmpD5;struct Cyc_PP_Out*_tmpD4;return(_tmpD4=_cycalloc(
sizeof(*_tmpD4)),((_tmpD4->newcc=st->cc + _tmp3C,((_tmpD4->newcl=st->cl,((_tmpD4->ppout=(
void*)((void*)((_tmpD7=_cycalloc(sizeof(*_tmpD7)),((_tmpD7[0]=((_tmpD5.tag=0,((
_tmpD5.f1=(void*)((_tmpD6=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmpD6[0]=
_tmp3D,_tmpD6)))),_tmpD5)))),_tmpD7))))),((_tmpD4->links=(void*)((void*)0),
_tmpD4)))))))));}}static struct Cyc_PP_Doc*Cyc_PP_text_doc(struct _dyneither_ptr s);
static struct Cyc_PP_Doc*Cyc_PP_text_doc(struct _dyneither_ptr s){int slen=(int)Cyc_strlen((
struct _dyneither_ptr)s);struct _tuple1*_tmpDA;struct Cyc_PP_Doc*_tmpD9;return(
_tmpD9=_cycalloc(sizeof(*_tmpD9)),((_tmpD9->mwo=slen,((_tmpD9->mw=Cyc_PP_infinity,((
_tmpD9->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple1*,
struct Cyc_PP_Ppstate*),struct _tuple1*x))Cyc_Fn_make_fn)(Cyc_PP_text_doc_f,((
_tmpDA=_cycalloc(sizeof(*_tmpDA)),((_tmpDA->f1=slen,((_tmpDA->f2=s,_tmpDA))))))),
_tmpD9)))))));}struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s);struct Cyc_PP_Doc*
Cyc_PP_text(struct _dyneither_ptr s){struct Cyc_Hashtable_Table*t;if(Cyc_PP_str_hashtbl
== 0){t=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(
101,Cyc_zstrptrcmp,Cyc_Hashtable_hash_stringptr);{struct Cyc_Core_Opt*_tmpDB;Cyc_PP_str_hashtbl=((
_tmpDB=_cycalloc(sizeof(*_tmpDB)),((_tmpDB->v=t,_tmpDB))));}}else{t=(struct Cyc_Hashtable_Table*)((
struct Cyc_Core_Opt*)_check_null(Cyc_PP_str_hashtbl))->v;}{struct _handler_cons
_tmp45;_push_handler(& _tmp45);{int _tmp47=0;if(setjmp(_tmp45.handler))_tmp47=1;
if(!_tmp47){{struct _dyneither_ptr*_tmpDC;struct Cyc_PP_Doc*_tmp49=((struct Cyc_PP_Doc*(*)(
struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,((
_tmpDC=_cycalloc(sizeof(*_tmpDC)),((_tmpDC[0]=s,_tmpDC)))));_npop_handler(0);
return _tmp49;};_pop_handler();}else{void*_tmp46=(void*)_exn_thrown;void*_tmp4B=
_tmp46;_LL14: if(_tmp4B != Cyc_Core_Not_found)goto _LL16;_LL15: {struct Cyc_PP_Doc*d=
Cyc_PP_text_doc(s);{struct _dyneither_ptr*_tmpDD;((void(*)(struct Cyc_Hashtable_Table*
t,struct _dyneither_ptr*key,struct Cyc_PP_Doc*val))Cyc_Hashtable_insert)(t,((
_tmpDD=_cycalloc(sizeof(*_tmpDD)),((_tmpDD[0]=s,_tmpDD)))),d);}return d;}_LL16:;
_LL17:(void)_throw(_tmp4B);_LL13:;}}}}struct Cyc_PP_Doc*Cyc_PP_textptr(struct
_dyneither_ptr*s);struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*s){return
Cyc_PP_text(*s);}static struct Cyc_PP_Doc*Cyc_PP_text_width_doc(struct
_dyneither_ptr s,int slen);static struct Cyc_PP_Doc*Cyc_PP_text_width_doc(struct
_dyneither_ptr s,int slen){struct _tuple1*_tmpE0;struct Cyc_PP_Doc*_tmpDF;return(
_tmpDF=_cycalloc(sizeof(*_tmpDF)),((_tmpDF->mwo=slen,((_tmpDF->mw=Cyc_PP_infinity,((
_tmpDF->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple1*,
struct Cyc_PP_Ppstate*),struct _tuple1*x))Cyc_Fn_make_fn)(Cyc_PP_text_doc_f,((
_tmpE0=_cycalloc(sizeof(*_tmpE0)),((_tmpE0->f1=slen,((_tmpE0->f2=s,_tmpE0))))))),
_tmpDF)))))));}static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl2=0;struct Cyc_PP_Doc*
Cyc_PP_text_width(struct _dyneither_ptr s,int slen);struct Cyc_PP_Doc*Cyc_PP_text_width(
struct _dyneither_ptr s,int slen){struct Cyc_Hashtable_Table*t;if(Cyc_PP_str_hashtbl2
== 0){t=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(
101,Cyc_zstrptrcmp,Cyc_Hashtable_hash_stringptr);{struct Cyc_Core_Opt*_tmpE1;Cyc_PP_str_hashtbl2=((
_tmpE1=_cycalloc(sizeof(*_tmpE1)),((_tmpE1->v=t,_tmpE1))));}}else{t=(struct Cyc_Hashtable_Table*)((
struct Cyc_Core_Opt*)_check_null(Cyc_PP_str_hashtbl2))->v;}{struct _handler_cons
_tmp50;_push_handler(& _tmp50);{int _tmp52=0;if(setjmp(_tmp50.handler))_tmp52=1;
if(!_tmp52){{struct _dyneither_ptr*_tmpE2;struct Cyc_PP_Doc*_tmp54=((struct Cyc_PP_Doc*(*)(
struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,((
_tmpE2=_cycalloc(sizeof(*_tmpE2)),((_tmpE2[0]=s,_tmpE2)))));_npop_handler(0);
return _tmp54;};_pop_handler();}else{void*_tmp51=(void*)_exn_thrown;void*_tmp56=
_tmp51;_LL19: if(_tmp56 != Cyc_Core_Not_found)goto _LL1B;_LL1A: {struct Cyc_PP_Doc*d=
Cyc_PP_text_width_doc(s,slen);{struct _dyneither_ptr*_tmpE3;((void(*)(struct Cyc_Hashtable_Table*
t,struct _dyneither_ptr*key,struct Cyc_PP_Doc*val))Cyc_Hashtable_insert)(t,((
_tmpE3=_cycalloc(sizeof(*_tmpE3)),((_tmpE3[0]=s,_tmpE3)))),d);}return d;}_LL1B:;
_LL1C:(void)_throw(_tmp56);_LL18:;}}}}struct Cyc_Core_Opt*Cyc_PP_nil_doc_opt=0;
struct Cyc_Core_Opt*Cyc_PP_blank_doc_opt=0;struct Cyc_Core_Opt*Cyc_PP_line_doc_opt=
0;struct Cyc_PP_Doc*Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_nil_doc(){if(Cyc_PP_nil_doc_opt
== 0){const char*_tmpE6;struct Cyc_Core_Opt*_tmpE5;Cyc_PP_nil_doc_opt=((_tmpE5=
_cycalloc(sizeof(*_tmpE5)),((_tmpE5->v=Cyc_PP_text(((_tmpE6="",_tag_dyneither(
_tmpE6,sizeof(char),1)))),_tmpE5))));}return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)
_check_null(Cyc_PP_nil_doc_opt))->v;}struct Cyc_PP_Doc*Cyc_PP_blank_doc();struct
Cyc_PP_Doc*Cyc_PP_blank_doc(){if(Cyc_PP_blank_doc_opt == 0){const char*_tmpE9;
struct Cyc_Core_Opt*_tmpE8;Cyc_PP_blank_doc_opt=((_tmpE8=_cycalloc(sizeof(*_tmpE8)),((
_tmpE8->v=Cyc_PP_text(((_tmpE9="",_tag_dyneither(_tmpE9,sizeof(char),1)))),
_tmpE8))));}return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_blank_doc_opt))->v;}
struct _tuple2{int f1;struct _dyneither_ptr f2;struct _dyneither_ptr f3;};struct
_tuple3{int f1;int f2;int f3;struct _dyneither_ptr f4;};static struct Cyc_PP_Out*Cyc_PP_hyperlink_f(
struct _tuple2*clo,struct Cyc_PP_Ppstate*st);static struct Cyc_PP_Out*Cyc_PP_hyperlink_f(
struct _tuple2*clo,struct Cyc_PP_Ppstate*st){struct _tuple2 _tmp5D;int _tmp5E;struct
_dyneither_ptr _tmp5F;struct _dyneither_ptr _tmp60;struct _tuple2*_tmp5C=clo;_tmp5D=*
_tmp5C;_tmp5E=_tmp5D.f1;_tmp5F=_tmp5D.f2;_tmp60=_tmp5D.f3;{struct Cyc_PP_Single_struct*
_tmpFC;struct _dyneither_ptr*_tmpFB;struct Cyc_PP_Single_struct _tmpFA;struct Cyc_PP_Single_struct*
_tmpF9;struct _tuple3*_tmpF8;struct Cyc_PP_Single_struct _tmpF7;struct Cyc_PP_Out*
_tmpF6;return(_tmpF6=_cycalloc(sizeof(*_tmpF6)),((_tmpF6->newcc=st->cc + _tmp5E,((
_tmpF6->newcl=st->cl,((_tmpF6->ppout=(void*)((void*)((_tmpFC=_cycalloc(sizeof(*
_tmpFC)),((_tmpFC[0]=((_tmpFA.tag=0,((_tmpFA.f1=(void*)((_tmpFB=_cycalloc(
sizeof(struct _dyneither_ptr)* 1),((_tmpFB[0]=_tmp5F,_tmpFB)))),_tmpFA)))),_tmpFC))))),((
_tmpF6->links=(void*)((void*)((_tmpF9=_cycalloc(sizeof(*_tmpF9)),((_tmpF9[0]=((
_tmpF7.tag=0,((_tmpF7.f1=(void*)((_tmpF8=_cycalloc(sizeof(*_tmpF8)),((_tmpF8->f1=
st->cl,((_tmpF8->f2=st->cc,((_tmpF8->f3=_tmp5E,((_tmpF8->f4=_tmp60,_tmpF8)))))))))),
_tmpF7)))),_tmpF9))))),_tmpF6)))))))));}}struct Cyc_PP_Doc*Cyc_PP_hyperlink(
struct _dyneither_ptr shrt,struct _dyneither_ptr full);struct Cyc_PP_Doc*Cyc_PP_hyperlink(
struct _dyneither_ptr shrt,struct _dyneither_ptr full){int slen=(int)Cyc_strlen((
struct _dyneither_ptr)shrt);struct _tuple2*_tmpFF;struct Cyc_PP_Doc*_tmpFE;return(
_tmpFE=_cycalloc(sizeof(*_tmpFE)),((_tmpFE->mwo=slen,((_tmpFE->mw=Cyc_PP_infinity,((
_tmpFE->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple2*,
struct Cyc_PP_Ppstate*),struct _tuple2*x))Cyc_Fn_make_fn)(Cyc_PP_hyperlink_f,((
_tmpFF=_cycalloc(sizeof(*_tmpFF)),((_tmpFF->f1=slen,((_tmpFF->f2=shrt,((_tmpFF->f3=
full,_tmpFF))))))))),_tmpFE)))))));}static struct Cyc_PP_Out*Cyc_PP_line_f(struct
Cyc_PP_Ppstate*st);static struct Cyc_PP_Out*Cyc_PP_line_f(struct Cyc_PP_Ppstate*st){
struct Cyc_PP_Single_struct*_tmp109;struct _dyneither_ptr*_tmp108;struct Cyc_PP_Single_struct
_tmp107;struct Cyc_PP_Out*_tmp106;return(_tmp106=_cycalloc(sizeof(*_tmp106)),((
_tmp106->newcc=st->ci,((_tmp106->newcl=st->cl + 1,((_tmp106->ppout=(void*)((void*)((
_tmp109=_cycalloc(sizeof(*_tmp109)),((_tmp109[0]=((_tmp107.tag=0,((_tmp107.f1=(
void*)((_tmp108=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp108[0]=Cyc_PP_nlblanks(
st->ci),_tmp108)))),_tmp107)))),_tmp109))))),((_tmp106->links=(void*)((void*)0),
_tmp106)))))))));}struct Cyc_PP_Doc*Cyc_PP_line_doc();struct Cyc_PP_Doc*Cyc_PP_line_doc(){
if(Cyc_PP_line_doc_opt == 0){struct Cyc_PP_Doc*_tmp10C;struct Cyc_Core_Opt*_tmp10B;
Cyc_PP_line_doc_opt=((_tmp10B=_cycalloc(sizeof(*_tmp10B)),((_tmp10B->v=((_tmp10C=
_cycalloc(sizeof(*_tmp10C)),((_tmp10C->mwo=0,((_tmp10C->mw=0,((_tmp10C->f=((
struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_PP_Ppstate*)))Cyc_Fn_fp2fn)(
Cyc_PP_line_f),_tmp10C)))))))),_tmp10B))));}return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)
_check_null(Cyc_PP_line_doc_opt))->v;}struct _tuple4{int f1;struct Cyc_PP_Doc*f2;};
static struct Cyc_PP_Out*Cyc_PP_nest_f(struct _tuple4*clo,struct Cyc_PP_Ppstate*st);
static struct Cyc_PP_Out*Cyc_PP_nest_f(struct _tuple4*clo,struct Cyc_PP_Ppstate*st){
struct _tuple4 _tmp71;int _tmp72;struct Cyc_PP_Doc*_tmp73;struct _tuple4*_tmp70=clo;
_tmp71=*_tmp70;_tmp72=_tmp71.f1;_tmp73=_tmp71.f2;{struct Cyc_PP_Ppstate*_tmp10D;
struct Cyc_PP_Ppstate*st2=(_tmp10D=_cycalloc_atomic(sizeof(*_tmp10D)),((_tmp10D->ci=
st->ci + _tmp72,((_tmp10D->cc=st->cc,((_tmp10D->cl=st->cl,((_tmp10D->pw=st->pw,((
_tmp10D->epw=st->epw,_tmp10D)))))))))));return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*
f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp73->f,st2);}}struct Cyc_PP_Doc*Cyc_PP_nest(
int k,struct Cyc_PP_Doc*d);struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d){
struct _tuple4*_tmp110;struct Cyc_PP_Doc*_tmp10F;return(_tmp10F=_cycalloc(sizeof(*
_tmp10F)),((_tmp10F->mwo=d->mwo,((_tmp10F->mw=d->mw,((_tmp10F->f=((struct Cyc_Fn_Function*(*)(
struct Cyc_PP_Out*(*f)(struct _tuple4*,struct Cyc_PP_Ppstate*),struct _tuple4*x))Cyc_Fn_make_fn)(
Cyc_PP_nest_f,((_tmp110=_cycalloc(sizeof(*_tmp110)),((_tmp110->f1=k,((_tmp110->f2=
d,_tmp110))))))),_tmp10F)))))));}int Cyc_PP_min(int x,int y);int Cyc_PP_min(int x,int
y){if(x <= y)return x;else{return y;}}int Cyc_PP_max(int x,int y);int Cyc_PP_max(int x,
int y){if(x >= y)return x;else{return y;}}struct _tuple5{struct Cyc_PP_Doc*f1;struct
Cyc_PP_Doc*f2;};static struct Cyc_PP_Out*Cyc_PP_concat_f(struct _tuple5*clo,struct
Cyc_PP_Ppstate*st);static struct Cyc_PP_Out*Cyc_PP_concat_f(struct _tuple5*clo,
struct Cyc_PP_Ppstate*st){struct _tuple5 _tmp78;struct Cyc_PP_Doc*_tmp79;struct Cyc_PP_Doc*
_tmp7A;struct _tuple5*_tmp77=clo;_tmp78=*_tmp77;_tmp79=_tmp78.f1;_tmp7A=_tmp78.f2;{
int epw2=Cyc_PP_max(st->pw - _tmp7A->mw,st->epw - _tmp79->mwo);struct Cyc_PP_Ppstate*
_tmp111;struct Cyc_PP_Ppstate*st2=(_tmp111=_cycalloc_atomic(sizeof(*_tmp111)),((
_tmp111->ci=st->ci,((_tmp111->cc=st->cc,((_tmp111->cl=st->cl,((_tmp111->pw=st->pw,((
_tmp111->epw=epw2,_tmp111)))))))))));struct Cyc_PP_Out*o1=((struct Cyc_PP_Out*(*)(
struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp79->f,st2);
struct Cyc_PP_Ppstate*_tmp112;struct Cyc_PP_Ppstate*st3=(_tmp112=_cycalloc_atomic(
sizeof(*_tmp112)),((_tmp112->ci=st->ci,((_tmp112->cc=o1->newcc,((_tmp112->cl=o1->newcl,((
_tmp112->pw=st->pw,((_tmp112->epw=epw2,_tmp112)))))))))));struct Cyc_PP_Out*o2=((
struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(
_tmp7A->f,st3);struct Cyc_PP_Out*_tmp113;struct Cyc_PP_Out*o3=(_tmp113=_cycalloc(
sizeof(*_tmp113)),((_tmp113->newcc=o2->newcc,((_tmp113->newcl=o2->newcl,((
_tmp113->ppout=(void*)((void*(*)(void*a1,void*a2))Cyc_PP_append)((void*)o1->ppout,(
void*)o2->ppout),((_tmp113->links=(void*)((void*(*)(void*a1,void*a2))Cyc_PP_append)((
void*)o1->links,(void*)o2->links),_tmp113)))))))));return o3;}}static struct Cyc_PP_Doc*
Cyc_PP_concat(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2);static struct Cyc_PP_Doc*
Cyc_PP_concat(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2){struct _tuple5*_tmp116;
struct Cyc_PP_Doc*_tmp115;return(_tmp115=_cycalloc(sizeof(*_tmp115)),((_tmp115->mwo=
Cyc_PP_min(d1->mw,d1->mwo + d2->mwo),((_tmp115->mw=Cyc_PP_min(d1->mw,d1->mwo + d2->mw),((
_tmp115->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple5*,
struct Cyc_PP_Ppstate*),struct _tuple5*x))Cyc_Fn_make_fn)(Cyc_PP_concat_f,((
_tmp116=_cycalloc(sizeof(*_tmp116)),((_tmp116->f1=d1,((_tmp116->f2=d2,_tmp116))))))),
_tmp115)))))));}struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr l);struct Cyc_PP_Doc*
Cyc_PP_cat(struct _dyneither_ptr l){struct Cyc_PP_Doc*_tmp80=Cyc_PP_nil_doc();{int i=(
int)(_get_dyneither_size(l,sizeof(struct Cyc_PP_Doc*))- 1);for(0;i >= 0;-- i){
_tmp80=Cyc_PP_concat(*((struct Cyc_PP_Doc**)_check_dyneither_subscript(l,sizeof(
struct Cyc_PP_Doc*),i)),_tmp80);}}return _tmp80;}static struct Cyc_PP_Out*Cyc_PP_long_cats_f(
struct Cyc_List_List*ds0,struct Cyc_PP_Ppstate*st);static struct Cyc_PP_Out*Cyc_PP_long_cats_f(
struct Cyc_List_List*ds0,struct Cyc_PP_Ppstate*st){struct Cyc_List_List*os=0;{
struct Cyc_List_List*_tmp81=ds0;for(0;_tmp81 != 0;_tmp81=_tmp81->tl){struct Cyc_PP_Doc*
d=(struct Cyc_PP_Doc*)_tmp81->hd;struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct
Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st);{struct Cyc_PP_Ppstate*
_tmp117;st=((_tmp117=_cycalloc_atomic(sizeof(*_tmp117)),((_tmp117->ci=st->ci,((
_tmp117->cc=o->newcc,((_tmp117->cl=o->newcl,((_tmp117->pw=st->pw,((_tmp117->epw=
st->epw - d->mwo,_tmp117))))))))))));}{struct Cyc_List_List*_tmp118;os=((_tmp118=
_cycalloc(sizeof(*_tmp118)),((_tmp118->hd=o,((_tmp118->tl=os,_tmp118))))));}}}{
int newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)_check_null(os))->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;void*s=(void*)0;void*links=(void*)0;
for(0;os != 0;os=os->tl){s=((void*(*)(void*a1,void*a2))Cyc_PP_append)((void*)((
struct Cyc_PP_Out*)os->hd)->ppout,s);links=((void*(*)(void*a1,void*a2))Cyc_PP_append)((
void*)((struct Cyc_PP_Out*)os->hd)->links,links);}{struct Cyc_PP_Out*_tmp119;
return(_tmp119=_cycalloc(sizeof(*_tmp119)),((_tmp119->newcc=newcc,((_tmp119->newcl=
newcl,((_tmp119->ppout=(void*)s,((_tmp119->links=(void*)links,_tmp119)))))))));}}}
static struct Cyc_PP_Doc*Cyc_PP_long_cats(struct Cyc_List_List*doclist);static
struct Cyc_PP_Doc*Cyc_PP_long_cats(struct Cyc_List_List*doclist){struct Cyc_List_List*
orig=doclist;struct Cyc_PP_Doc*d=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(doclist))->hd;doclist=doclist->tl;{int mw=d->mw;int mwo=d->mw;{struct
Cyc_List_List*_tmp85=doclist;for(0;_tmp85 != 0;_tmp85=_tmp85->tl){int mw2=Cyc_PP_min(
mw,mwo + ((struct Cyc_PP_Doc*)_tmp85->hd)->mwo);int mwo2=Cyc_PP_min(mw,mwo + ((
struct Cyc_PP_Doc*)_tmp85->hd)->mw);mw=mw2;mwo=mwo2;}}{struct Cyc_PP_Doc*_tmp11A;
return(_tmp11A=_cycalloc(sizeof(*_tmp11A)),((_tmp11A->mwo=mw,((_tmp11A->mw=mwo,((
_tmp11A->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_List_List*,
struct Cyc_PP_Ppstate*),struct Cyc_List_List*x))Cyc_Fn_make_fn)(Cyc_PP_long_cats_f,
orig),_tmp11A)))))));}}}struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*doclist);
struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*doclist){if(doclist == 0)return
Cyc_PP_nil_doc();else{if(doclist->tl == 0)return(struct Cyc_PP_Doc*)doclist->hd;
else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(doclist)> 30)return Cyc_PP_long_cats(
doclist);else{return Cyc_PP_concat((struct Cyc_PP_Doc*)doclist->hd,Cyc_PP_cats(
doclist->tl));}}}}static struct Cyc_PP_Out*Cyc_PP_cats_arr_f(struct _dyneither_ptr*
docs_ptr,struct Cyc_PP_Ppstate*st);static struct Cyc_PP_Out*Cyc_PP_cats_arr_f(
struct _dyneither_ptr*docs_ptr,struct Cyc_PP_Ppstate*st){struct Cyc_List_List*os=0;
struct _dyneither_ptr docs=*docs_ptr;int sz=(int)_get_dyneither_size(docs,sizeof(
struct Cyc_PP_Doc*));{int i=0;for(0;i < sz;++ i){struct Cyc_PP_Doc*d=*((struct Cyc_PP_Doc**)
_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),i));struct Cyc_PP_Out*o=((
struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(
d->f,st);{struct Cyc_PP_Ppstate*_tmp11B;st=((_tmp11B=_cycalloc_atomic(sizeof(*
_tmp11B)),((_tmp11B->ci=st->ci,((_tmp11B->cc=o->newcc,((_tmp11B->cl=o->newcl,((
_tmp11B->pw=st->pw,((_tmp11B->epw=st->epw - d->mwo,_tmp11B))))))))))));}{struct
Cyc_List_List*_tmp11C;os=((_tmp11C=_cycalloc(sizeof(*_tmp11C)),((_tmp11C->hd=o,((
_tmp11C->tl=os,_tmp11C))))));}}}{int newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)
_check_null(os))->hd)->newcc;int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;void*s=(
void*)0;void*links=(void*)0;for(0;os != 0;os=os->tl){s=((void*(*)(void*a1,void*a2))
Cyc_PP_append)((void*)((struct Cyc_PP_Out*)os->hd)->ppout,s);links=((void*(*)(
void*a1,void*a2))Cyc_PP_append)((void*)((struct Cyc_PP_Out*)os->hd)->links,links);}{
struct Cyc_PP_Out*_tmp11D;return(_tmp11D=_cycalloc(sizeof(*_tmp11D)),((_tmp11D->newcc=
newcc,((_tmp11D->newcl=newcl,((_tmp11D->ppout=(void*)s,((_tmp11D->links=(void*)
links,_tmp11D)))))))));}}}struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _dyneither_ptr
docs);struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _dyneither_ptr docs){int sz=(int)
_get_dyneither_size(docs,sizeof(struct Cyc_PP_Doc*));if(sz == 0){struct Cyc_Core_Failure_struct
_tmp123;const char*_tmp122;struct Cyc_Core_Failure_struct*_tmp121;(int)_throw((
void*)((_tmp121=_cycalloc(sizeof(*_tmp121)),((_tmp121[0]=((_tmp123.tag=Cyc_Core_Failure,((
_tmp123.f1=((_tmp122="cats_arr -- size zero array",_tag_dyneither(_tmp122,
sizeof(char),28))),_tmp123)))),_tmp121)))));}{struct Cyc_PP_Doc*d=*((struct Cyc_PP_Doc**)
_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),0));int mw=d->mw;int mwo=
d->mw;{int i=1;for(0;i < sz;++ i){int mw2=Cyc_PP_min(mw,mwo + (*((struct Cyc_PP_Doc**)
_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),i)))->mwo);int mwo2=Cyc_PP_min(
mw,mwo + (*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),
i)))->mw);mw=mw2;mwo=mwo2;}}{struct _dyneither_ptr*_tmp126;struct Cyc_PP_Doc*
_tmp125;return(_tmp125=_cycalloc(sizeof(*_tmp125)),((_tmp125->mwo=mw,((_tmp125->mw=
mwo,((_tmp125->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct
_dyneither_ptr*,struct Cyc_PP_Ppstate*),struct _dyneither_ptr*x))Cyc_Fn_make_fn)(
Cyc_PP_cats_arr_f,((_tmp126=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp126[
0]=docs,_tmp126))))),_tmp125)))))));}}}static struct Cyc_PP_Out*Cyc_PP_doc_union_f(
struct _tuple5*clo,struct Cyc_PP_Ppstate*st);static struct Cyc_PP_Out*Cyc_PP_doc_union_f(
struct _tuple5*clo,struct Cyc_PP_Ppstate*st){struct _tuple5 _tmp90;struct Cyc_PP_Doc*
_tmp91;struct Cyc_PP_Doc*_tmp92;struct _tuple5*_tmp8F=clo;_tmp90=*_tmp8F;_tmp91=
_tmp90.f1;_tmp92=_tmp90.f2;{int dfits=st->cc + _tmp91->mwo <= st->epw  || st->cc + 
_tmp91->mw <= st->pw;if(dfits)return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*
f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp91->f,st);else{return((struct Cyc_PP_Out*(*)(
struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp92->f,st);}}}
struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*d,struct Cyc_PP_Doc*d2);struct
Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*d,struct Cyc_PP_Doc*d2){struct
_tuple5*_tmp129;struct Cyc_PP_Doc*_tmp128;return(_tmp128=_cycalloc(sizeof(*
_tmp128)),((_tmp128->mwo=Cyc_PP_min(d->mwo,d2->mwo),((_tmp128->mw=Cyc_PP_min(d->mw,
d2->mw),((_tmp128->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct
_tuple5*,struct Cyc_PP_Ppstate*),struct _tuple5*x))Cyc_Fn_make_fn)(Cyc_PP_doc_union_f,((
_tmp129=_cycalloc(sizeof(*_tmp129)),((_tmp129->f1=d,((_tmp129->f2=d2,_tmp129))))))),
_tmp128)))))));}struct Cyc_PP_Doc*Cyc_PP_oline_doc();struct Cyc_PP_Doc*Cyc_PP_oline_doc(){
return Cyc_PP_doc_union(Cyc_PP_nil_doc(),Cyc_PP_line_doc());}static struct Cyc_PP_Out*
Cyc_PP_tab_f(struct Cyc_PP_Doc*d,struct Cyc_PP_Ppstate*st);static struct Cyc_PP_Out*
Cyc_PP_tab_f(struct Cyc_PP_Doc*d,struct Cyc_PP_Ppstate*st){struct Cyc_PP_Ppstate*
_tmp12A;struct Cyc_PP_Ppstate*st2=(_tmp12A=_cycalloc_atomic(sizeof(*_tmp12A)),((
_tmp12A->ci=st->cc,((_tmp12A->cc=st->cc,((_tmp12A->cl=st->cl,((_tmp12A->pw=st->pw,((
_tmp12A->epw=st->epw,_tmp12A)))))))))));return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*
f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st2);}struct Cyc_PP_Doc*Cyc_PP_tab(
struct Cyc_PP_Doc*d);struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*d){struct Cyc_PP_Doc*
_tmp12B;struct Cyc_PP_Doc*d2=(_tmp12B=_cycalloc(sizeof(*_tmp12B)),((_tmp12B->mwo=
d->mwo,((_tmp12B->mw=d->mw,((_tmp12B->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*
f)(struct Cyc_PP_Doc*,struct Cyc_PP_Ppstate*),struct Cyc_PP_Doc*x))Cyc_Fn_make_fn)(
Cyc_PP_tab_f,d),_tmp12B)))))));return d2;}static struct Cyc_PP_Doc*Cyc_PP_ppseq_f(
struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
static struct Cyc_PP_Doc*Cyc_PP_ppseq_f(struct Cyc_PP_Doc*(*pp)(void*),struct
_dyneither_ptr sep,struct Cyc_List_List*l){if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return pp((void*)l->hd);else{struct Cyc_PP_Doc*_tmp12C[4];return(
_tmp12C[3]=Cyc_PP_ppseq_f(pp,sep,l->tl),((_tmp12C[2]=Cyc_PP_oline_doc(),((
_tmp12C[1]=Cyc_PP_text(sep),((_tmp12C[0]=pp((void*)l->hd),Cyc_PP_cat(
_tag_dyneither(_tmp12C,sizeof(struct Cyc_PP_Doc*),4)))))))));}}}struct Cyc_PP_Doc*
Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*
l);struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct
_dyneither_ptr sep,struct Cyc_List_List*l){return Cyc_PP_tab(Cyc_PP_ppseq_f(pp,sep,
l));}struct Cyc_PP_Doc*Cyc_PP_seq_f(struct _dyneither_ptr sep,struct Cyc_List_List*l);
struct Cyc_PP_Doc*Cyc_PP_seq_f(struct _dyneither_ptr sep,struct Cyc_List_List*l){if(
l == 0)return Cyc_PP_nil_doc();else{if(l->tl == 0)return(struct Cyc_PP_Doc*)l->hd;
else{struct Cyc_PP_Doc*sep2=Cyc_PP_text(sep);struct Cyc_PP_Doc*oline=Cyc_PP_oline_doc();
struct Cyc_List_List*_tmp98=l;while(_tmp98->tl != 0){struct Cyc_List_List*_tmp99=
_tmp98->tl;{struct Cyc_List_List*_tmp12F;struct Cyc_List_List*_tmp12E;_tmp98->tl=((
_tmp12E=_cycalloc(sizeof(*_tmp12E)),((_tmp12E->hd=sep2,((_tmp12E->tl=((_tmp12F=
_cycalloc(sizeof(*_tmp12F)),((_tmp12F->hd=oline,((_tmp12F->tl=_tmp99,_tmp12F)))))),
_tmp12E))))));}_tmp98=_tmp99;}return Cyc_PP_cats(l);}}}struct Cyc_PP_Doc*Cyc_PP_seq(
struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_seq(
struct _dyneither_ptr sep,struct Cyc_List_List*l){return Cyc_PP_tab(Cyc_PP_seq_f(sep,
l));}struct Cyc_PP_Doc*Cyc_PP_ppseql_f(struct Cyc_PP_Doc*(*pp)(void*),struct
_dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_ppseql_f(struct
Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){if(l == 0)
return Cyc_PP_nil_doc();else{if(l->tl == 0)return pp((void*)l->hd);else{struct Cyc_PP_Doc*
_tmp130[4];return(_tmp130[3]=Cyc_PP_ppseql_f(pp,sep,l->tl),((_tmp130[2]=Cyc_PP_line_doc(),((
_tmp130[1]=Cyc_PP_text(sep),((_tmp130[0]=pp((void*)l->hd),Cyc_PP_cat(
_tag_dyneither(_tmp130,sizeof(struct Cyc_PP_Doc*),4)))))))));}}}struct Cyc_PP_Doc*
Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*
l);struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct
_dyneither_ptr sep,struct Cyc_List_List*l){return Cyc_PP_tab(Cyc_PP_ppseql_f(pp,sep,
l));}static struct Cyc_PP_Doc*Cyc_PP_seql_f(struct _dyneither_ptr sep,struct Cyc_List_List*
l);static struct Cyc_PP_Doc*Cyc_PP_seql_f(struct _dyneither_ptr sep,struct Cyc_List_List*
l){if(l == 0)return Cyc_PP_nil_doc();else{if(l->tl == 0)return(struct Cyc_PP_Doc*)l->hd;
else{struct Cyc_PP_Doc*_tmp131[4];return(_tmp131[3]=Cyc_PP_seql_f(sep,l->tl),((
_tmp131[2]=Cyc_PP_line_doc(),((_tmp131[1]=Cyc_PP_text(sep),((_tmp131[0]=(struct
Cyc_PP_Doc*)l->hd,Cyc_PP_cat(_tag_dyneither(_tmp131,sizeof(struct Cyc_PP_Doc*),4)))))))));}}}
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,struct Cyc_List_List*l0);
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,struct Cyc_List_List*l0){
return Cyc_PP_tab(Cyc_PP_seql_f(sep,l0));}struct Cyc_PP_Doc*Cyc_PP_group(struct
_dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*
ss);struct Cyc_PP_Doc*Cyc_PP_group(struct _dyneither_ptr start,struct _dyneither_ptr
stop,struct _dyneither_ptr sep,struct Cyc_List_List*ss){struct Cyc_PP_Doc*_tmp132[3];
return(_tmp132[2]=Cyc_PP_text(stop),((_tmp132[1]=Cyc_PP_seq(sep,ss),((_tmp132[0]=
Cyc_PP_text(start),Cyc_PP_cat(_tag_dyneither(_tmp132,sizeof(struct Cyc_PP_Doc*),3)))))));}
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr
stop,struct _dyneither_ptr sep,struct Cyc_List_List*ss);struct Cyc_PP_Doc*Cyc_PP_egroup(
struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,
struct Cyc_List_List*ss){if(ss == 0)return Cyc_PP_nil_doc();else{struct Cyc_PP_Doc*
_tmp133[3];return(_tmp133[2]=Cyc_PP_text(stop),((_tmp133[1]=Cyc_PP_seq(sep,ss),((
_tmp133[0]=Cyc_PP_text(start),Cyc_PP_cat(_tag_dyneither(_tmp133,sizeof(struct Cyc_PP_Doc*),
3)))))));}}struct Cyc_PP_Doc*Cyc_PP_groupl(struct _dyneither_ptr start,struct
_dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*ss);struct Cyc_PP_Doc*
Cyc_PP_groupl(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct
_dyneither_ptr sep,struct Cyc_List_List*ss){struct Cyc_PP_Doc*_tmp134[3];return(
_tmp134[2]=Cyc_PP_text(stop),((_tmp134[1]=Cyc_PP_seql(sep,ss),((_tmp134[0]=Cyc_PP_text(
start),Cyc_PP_cat(_tag_dyneither(_tmp134,sizeof(struct Cyc_PP_Doc*),3)))))));}
