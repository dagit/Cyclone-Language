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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);int
Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[16];
extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct
_dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*,int(*cmp)(void*,void*),
struct Cyc_List_List*a,struct Cyc_List_List*b);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_rzip(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc___cycFILE;extern struct
Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
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
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,
void**ans);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict
d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,
void*);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Hashtable_Table;
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*
cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*
t,void*key,void*val);void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,
void*key);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];
int Cyc_Position_error_p();struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
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
Cyc_Absyn_UnknownTunionInfo{struct _tuple1*name;int is_xtunion;int is_flat;};struct
Cyc_Absyn_UnknownTunion_struct{int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};
struct Cyc_Absyn_KnownTunion_struct{int tag;struct Cyc_Absyn_Tuniondecl**f1;};union
Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct UnknownTunion;
struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{union
Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
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
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Region_s_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*
f4;struct Cyc_Absyn_Stmt*f5;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Alias_s_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;struct Cyc_Absyn_Stmt*f4;};
struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*loc;struct Cyc_List_List*
non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
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
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_aggr_type(void*t);struct
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
le;int allow_valueof;};void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_path(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);int Cyc_Tcutil_is_noreturn(
void*);struct _tuple4{unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessNumelts_struct{int tag;struct Cyc_Absyn_Vardecl*f1;}
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
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct _dyneither_ptr f1;};int Cyc_CfFlowInfo_update_place_set(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,
struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict
consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct
Cyc_CfFlowInfo_Place*);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(
struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(union
Cyc_CfFlowInfo_FlowInfo_union f1,union Cyc_CfFlowInfo_FlowInfo_union f2);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct
Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,
struct Cyc_List_List*);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_join_flow(struct
Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo_union,
union Cyc_CfFlowInfo_FlowInfo_union,int);struct _tuple5{union Cyc_CfFlowInfo_FlowInfo_union
f1;void*f2;};struct _tuple5 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*all_changed,struct _tuple5 pr1,struct _tuple5 pr2,int);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,
union Cyc_CfFlowInfo_FlowInfo_union,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Dict_Dict,
struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union f,void*rgn);
struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_readthrough_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);struct _tuple6{struct Cyc_CfFlowInfo_ConsumeInfo f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};struct _tuple6 Cyc_CfFlowInfo_save_consume_info(struct Cyc_CfFlowInfo_FlowEnv*,
union Cyc_CfFlowInfo_FlowInfo_union f,int clear);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo_union f,struct
Cyc_CfFlowInfo_ConsumeInfo c);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*
enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(struct
Cyc_List_List*ds);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_all_effects: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt*encloser;int visited;};static char Cyc_NewControlFlow_CFAnnot[
12]="\000\000\000\000CFAnnot\000";struct Cyc_NewControlFlow_CFAnnot_struct{char*
tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){struct Cyc_NewControlFlow_CFAnnot_struct
_tmp5BB;struct Cyc_NewControlFlow_CFStmtAnnot _tmp5BA;struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp5B9;(void*)(enclosee->annot=(void*)((void*)((_tmp5B9=_cycalloc(sizeof(*
_tmp5B9)),((_tmp5B9[0]=((_tmp5BB.tag=Cyc_NewControlFlow_CFAnnot,((_tmp5BB.f1=((
_tmp5BA.encloser=encloser,((_tmp5BA.visited=0,_tmp5BA)))),_tmp5BB)))),_tmp5B9))))));}
struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*
fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo_union
tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;struct Cyc_List_List*
param_roots;struct Cyc_Hashtable_Table*flow_table;};static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,
struct Cyc_Absyn_Stmt*);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Decl*);
struct _tuple7{union Cyc_CfFlowInfo_FlowInfo_union f1;union Cyc_CfFlowInfo_AbsLVal_union
f2;};static struct _tuple7 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);
struct _tuple8{union Cyc_CfFlowInfo_FlowInfo_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};static struct _tuple8 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);static struct Cyc_List_List*
Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_Place*p,void*t);static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt*s);static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt*s){void*_tmp3=(void*)s->annot;struct Cyc_NewControlFlow_CFStmtAnnot
_tmp4;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;_LL1: if(*((void**)_tmp3)!= Cyc_NewControlFlow_CFAnnot)
goto _LL3;_tmp4=((struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_tmp5=(
struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_struct*)
_tmp3)->f1;_LL2: return _tmp5;_LL3:;_LL4: {struct Cyc_Core_Impossible_struct _tmp5C1;
const char*_tmp5C0;struct Cyc_Core_Impossible_struct*_tmp5BF;(int)_throw((void*)((
_tmp5BF=_cycalloc(sizeof(*_tmp5BF)),((_tmp5BF[0]=((_tmp5C1.tag=Cyc_Core_Impossible,((
_tmp5C1.f1=((_tmp5C0="ControlFlow -- wrong stmt annotation",_tag_dyneither(
_tmp5C0,sizeof(char),37))),_tmp5C1)))),_tmp5BF)))));}_LL0:;}static union Cyc_CfFlowInfo_FlowInfo_union*
Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*
s);static union Cyc_CfFlowInfo_FlowInfo_union*Cyc_NewControlFlow_get_stmt_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union**
sflow=((union Cyc_CfFlowInfo_FlowInfo_union**(*)(struct Cyc_Hashtable_Table*t,
struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);if(sflow
== 0){union Cyc_CfFlowInfo_FlowInfo_union _tmp5C4;union Cyc_CfFlowInfo_FlowInfo_union*
_tmp5C3;union Cyc_CfFlowInfo_FlowInfo_union*res=(_tmp5C3=_region_malloc(env->r,
sizeof(*_tmp5C3)),((_tmp5C3[0]=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp5C4.BottomFL).tag=
0,_tmp5C4)),_tmp5C3)));((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*
key,union Cyc_CfFlowInfo_FlowInfo_union*val))Cyc_Hashtable_insert)(env->flow_table,
s,res);return res;}return*sflow;}struct _tuple9{struct Cyc_NewControlFlow_CFStmtAnnot*
f1;union Cyc_CfFlowInfo_FlowInfo_union*f2;};static struct _tuple9 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Stmt*s);static struct _tuple9 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo_union*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,
s);*_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC,1);
_tmpB->visited=env->iteration_num;{struct _tuple9 _tmp5C5;return(_tmp5C5.f1=_tmpB,((
_tmp5C5.f2=_tmpC,_tmp5C5)));}}static int Cyc_NewControlFlow_get_field_index_fs(
struct Cyc_List_List*fs,struct _dyneither_ptr*f);static int Cyc_NewControlFlow_get_field_index_fs(
struct Cyc_List_List*fs,struct _dyneither_ptr*f){int n=0;for(0;(unsigned int)fs;fs=
fs->tl){struct _dyneither_ptr*_tmpE=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmpE,f)== 0)return n;n=n + 1;}{struct Cyc_Core_Impossible_struct
_tmp5D2;const char*_tmp5D1;void*_tmp5D0[1];struct Cyc_String_pa_struct _tmp5CF;
struct Cyc_Core_Impossible_struct*_tmp5CE;(int)_throw((void*)((_tmp5CE=_cycalloc(
sizeof(*_tmp5CE)),((_tmp5CE[0]=((_tmp5D2.tag=Cyc_Core_Impossible,((_tmp5D2.f1=(
struct _dyneither_ptr)((_tmp5CF.tag=0,((_tmp5CF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp5D0[0]=& _tmp5CF,Cyc_aprintf(((_tmp5D1="get_field_index_fs failed: %s",
_tag_dyneither(_tmp5D1,sizeof(char),30))),_tag_dyneither(_tmp5D0,sizeof(void*),1)))))))),
_tmp5D2)))),_tmp5CE)))));}}static int Cyc_NewControlFlow_get_field_index(void*t,
struct _dyneither_ptr*f);static int Cyc_NewControlFlow_get_field_index(void*t,
struct _dyneither_ptr*f){void*_tmp14=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp15;union Cyc_Absyn_AggrInfoU_union _tmp16;struct Cyc_List_List*_tmp17;_LL6: if(
_tmp14 <= (void*)4)goto _LLA;if(*((int*)_tmp14)!= 10)goto _LL8;_tmp15=((struct Cyc_Absyn_AggrType_struct*)
_tmp14)->f1;_tmp16=_tmp15.aggr_info;_LL7: {struct Cyc_List_List*_tmp18=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp16))->impl))->fields;
_tmp17=_tmp18;goto _LL9;}_LL8: if(*((int*)_tmp14)!= 11)goto _LLA;_tmp17=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp14)->f2;_LL9: return Cyc_NewControlFlow_get_field_index_fs(_tmp17,f);_LLA:;
_LLB: {struct Cyc_Core_Impossible_struct _tmp5DF;const char*_tmp5DE;void*_tmp5DD[1];
struct Cyc_String_pa_struct _tmp5DC;struct Cyc_Core_Impossible_struct*_tmp5DB;(int)
_throw((void*)((_tmp5DB=_cycalloc(sizeof(*_tmp5DB)),((_tmp5DB[0]=((_tmp5DF.tag=
Cyc_Core_Impossible,((_tmp5DF.f1=(struct _dyneither_ptr)((_tmp5DC.tag=0,((_tmp5DC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp5DD[
0]=& _tmp5DC,Cyc_aprintf(((_tmp5DE="get_field_index failed: %s",_tag_dyneither(
_tmp5DE,sizeof(char),27))),_tag_dyneither(_tmp5DD,sizeof(void*),1)))))))),
_tmp5DF)))),_tmp5DB)))));}_LL5:;}static void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union new_flow);
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,
union Cyc_CfFlowInfo_FlowInfo_union new_flow){if(env->in_try){env->tryflow=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,new_flow,env->tryflow,1);{struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1F;struct Cyc_Dict_Dict _tmp20;struct Cyc_List_List*_tmp21;struct _tuple6 _tmp1E=
Cyc_CfFlowInfo_save_consume_info(env->fenv,env->tryflow,0);_tmp1F=_tmp1E.f1;
_tmp20=_tmp1F.consumed;_tmp21=_tmp1F.may_consume;{struct Cyc_CfFlowInfo_ConsumeInfo
_tmp5E0;env->tryflow=Cyc_CfFlowInfo_restore_consume_info(env->tryflow,((_tmp5E0.consumed=
_tmp20,((_tmp5E0.may_consume=0,_tmp5E0)))));}}}}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo_union
flow);static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo_union flow){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp23=Cyc_NewControlFlow_get_stmt_annot(s);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp24=Cyc_NewControlFlow_get_stmt_flow(env,s);union Cyc_CfFlowInfo_FlowInfo_union
_tmp25=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,flow,*_tmp24,1);if(!
Cyc_CfFlowInfo_flow_lessthan_approx(_tmp25,*_tmp24)){*_tmp24=_tmp25;if(_tmp23->visited
== env->iteration_num)env->iterate_again=1;}}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*vds,void*leafval,struct Cyc_Position_Segment*loc);
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_List_List*vds,void*
leafval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo_union _tmp26=
inflow;struct Cyc_Dict_Dict _tmp27;struct Cyc_List_List*_tmp28;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp29;_LLD: if((_tmp26.BottomFL).tag != 0)goto _LLF;_LLE: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp5E1;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp5E1.BottomFL).tag=0,
_tmp5E1));}_LLF: if((_tmp26.ReachableFL).tag != 1)goto _LLC;_tmp27=(_tmp26.ReachableFL).f1;
_tmp28=(_tmp26.ReachableFL).f2;_tmp29=(_tmp26.ReachableFL).f3;_LL10: for(0;vds != 
0;vds=vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct _tmp5E4;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp5E3;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp2B=(_tmp5E3=_region_malloc(fenv->r,
sizeof(*_tmp5E3)),((_tmp5E3[0]=((_tmp5E4.tag=0,((_tmp5E4.f1=(struct Cyc_Absyn_Vardecl*)
vds->hd,_tmp5E4)))),_tmp5E3)));_tmp27=Cyc_Dict_insert(_tmp27,(void*)_tmp2B,Cyc_CfFlowInfo_typ_to_absrval(
fenv,(void*)((struct Cyc_Absyn_Vardecl*)vds->hd)->type,leafval));}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5E5;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp5E5.ReachableFL).tag=1,(((
_tmp5E5.ReachableFL).f1=_tmp27,(((_tmp5E5.ReachableFL).f2=_tmp28,(((_tmp5E5.ReachableFL).f3=
_tmp29,_tmp5E5))))))));}_LLC:;}struct _tuple10{struct Cyc_CfFlowInfo_Place*f1;
struct Cyc_Position_Segment*f2;};static void Cyc_NewControlFlow_remove_vars(struct
Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Dict_Dict*
out_unique_places,struct Cyc_Dict_Dict old_unique_places);static void Cyc_NewControlFlow_remove_vars(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union outflow,
struct Cyc_Dict_Dict*out_unique_places,struct Cyc_Dict_Dict old_unique_places){
struct Cyc_CfFlowInfo_ConsumeInfo _tmp30;struct Cyc_Dict_Dict _tmp31;struct _tuple6
_tmp2F=Cyc_CfFlowInfo_save_consume_info(fenv,outflow,0);_tmp30=_tmp2F.f1;_tmp31=
_tmp30.consumed;{struct _RegionHandle _tmp32=_new_region("tmp");struct
_RegionHandle*tmp=& _tmp32;_push_region(tmp);{struct Cyc_Iter_Iter _tmp33=((struct
Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
tmp,*((struct Cyc_Dict_Dict*)_check_null(out_unique_places)));struct _tuple10
_tmp34=*((struct _tuple10*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
tmp,*out_unique_places);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple10*))Cyc_Iter_next)(
_tmp33,& _tmp34)){struct Cyc_CfFlowInfo_Place*_tmp35=_tmp34.f1;if(!((int(*)(struct
Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp31,_tmp35)){
const char*_tmp5E9;void*_tmp5E8[1];struct Cyc_String_pa_struct _tmp5E7;(_tmp5E7.tag=
0,((_tmp5E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_CfFlowInfo_place_err_string(
_tmp35)),((_tmp5E8[0]=& _tmp5E7,Cyc_Tcutil_terr(_tmp34.f2,((_tmp5E9="Failed to consume unique variable %s",
_tag_dyneither(_tmp5E9,sizeof(char),37))),_tag_dyneither(_tmp5E8,sizeof(void*),1)))))));}}};
_pop_region(tmp);}*out_unique_places=old_unique_places;}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo_union inflow,void*r);static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo_union inflow,void*r){union Cyc_CfFlowInfo_FlowInfo_union
_tmp39=inflow;struct Cyc_Dict_Dict _tmp3A;struct Cyc_List_List*_tmp3B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3C;_LL12: if((_tmp39.BottomFL).tag != 0)goto _LL14;_LL13: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp5EA;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp5EA.BottomFL).tag=0,
_tmp5EA));}_LL14: if((_tmp39.ReachableFL).tag != 1)goto _LL11;_tmp3A=(_tmp39.ReachableFL).f1;
_tmp3B=(_tmp39.ReachableFL).f2;_tmp3C=(_tmp39.ReachableFL).f3;_LL15: if(Cyc_CfFlowInfo_initlevel(
_tmp3A,r)!= (void*)2){const char*_tmp5ED;void*_tmp5EC;(_tmp5EC=0,Cyc_Tcutil_terr(
loc,((_tmp5ED="expression may not be fully initialized",_tag_dyneither(_tmp5ED,
sizeof(char),40))),_tag_dyneither(_tmp5EC,sizeof(void*),0)));}{struct Cyc_Dict_Dict
_tmp40=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp3A,env->all_changed,r);if(
_tmp3A.t == _tmp40.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union _tmp5EE;
union Cyc_CfFlowInfo_FlowInfo_union _tmp41=((_tmp5EE.ReachableFL).tag=1,(((_tmp5EE.ReachableFL).f1=
_tmp40,(((_tmp5EE.ReachableFL).f2=_tmp3B,(((_tmp5EE.ReachableFL).f3=_tmp3C,
_tmp5EE)))))));Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp41);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp41;}}_LL11:;}struct
_tuple11{union Cyc_CfFlowInfo_FlowInfo_union f1;struct Cyc_List_List*f2;};static
struct _tuple11 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_List_List*es,int arg1_unconsumed);static struct _tuple11 Cyc_NewControlFlow_anal_unordered_Rexps(
struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*es,int arg1_unconsumed){if(es == 0){struct _tuple11
_tmp5EF;return(_tmp5EF.f1=inflow,((_tmp5EF.f2=0,_tmp5EF)));}if(es->tl == 0){union
Cyc_CfFlowInfo_FlowInfo_union _tmp45;void*_tmp46;struct _tuple5 _tmp44=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp45=_tmp44.f1;_tmp46=_tmp44.f2;{
struct Cyc_List_List*_tmp5F2;struct _tuple11 _tmp5F1;return(_tmp5F1.f1=_tmp45,((
_tmp5F1.f2=((_tmp5F2=_region_malloc(rgn,sizeof(*_tmp5F2)),((_tmp5F2->hd=(void*)
_tmp46,((_tmp5F2->tl=0,_tmp5F2)))))),_tmp5F1)));}}{struct Cyc_Dict_Dict*
outer_all_changed=env->all_changed;struct Cyc_Dict_Dict this_all_changed;union Cyc_CfFlowInfo_FlowInfo_union
old_inflow;union Cyc_CfFlowInfo_FlowInfo_union outflow;struct Cyc_List_List*rvals;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp4A;union Cyc_CfFlowInfo_FlowInfo_union _tmp4B;
struct _tuple6 _tmp49=Cyc_CfFlowInfo_save_consume_info(env->fenv,inflow,1);_tmp4A=
_tmp49.f1;_tmp4B=_tmp49.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp5F3;struct Cyc_CfFlowInfo_ConsumeInfo
outflow_consume=(_tmp5F3.consumed=(env->fenv)->mt_place_set,((_tmp5F3.may_consume=
0,_tmp5F3)));int init_consume=0;do{this_all_changed=(env->fenv)->mt_place_set;
_tmp4B=Cyc_CfFlowInfo_restore_consume_info(_tmp4B,_tmp4A);{struct Cyc_Dict_Dict*
_tmp5F4;env->all_changed=((_tmp5F4=_region_malloc(env->r,sizeof(*_tmp5F4)),((
_tmp5F4[0]=(env->fenv)->mt_place_set,_tmp5F4))));}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp4E;void*_tmp4F;struct _tuple5 _tmp4D=Cyc_NewControlFlow_anal_Rexp(env,_tmp4B,(
struct Cyc_Absyn_Exp*)es->hd);_tmp4E=_tmp4D.f1;_tmp4F=_tmp4D.f2;outflow=_tmp4E;{
struct Cyc_List_List*_tmp5F5;rvals=((_tmp5F5=_region_malloc(rgn,sizeof(*_tmp5F5)),((
_tmp5F5->hd=(void*)_tmp4F,((_tmp5F5->tl=0,_tmp5F5))))));}this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);if(
arg1_unconsumed){union Cyc_CfFlowInfo_FlowInfo_union _tmp51=outflow;struct Cyc_Dict_Dict
_tmp52;struct Cyc_List_List*_tmp53;struct Cyc_CfFlowInfo_ConsumeInfo _tmp54;_LL17:
if((_tmp51.BottomFL).tag != 0)goto _LL19;_LL18: goto _LL16;_LL19: if((_tmp51.ReachableFL).tag
!= 1)goto _LL16;_tmp52=(_tmp51.ReachableFL).f1;_tmp53=(_tmp51.ReachableFL).f2;
_tmp54=(_tmp51.ReachableFL).f3;_LL1A: {struct Cyc_CfFlowInfo_ConsumeInfo _tmp5F8;
union Cyc_CfFlowInfo_FlowInfo_union _tmp5F7;outflow=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp5F7.ReachableFL).tag=1,(((_tmp5F7.ReachableFL).f1=_tmp52,(((_tmp5F7.ReachableFL).f2=
_tmp53,(((_tmp5F7.ReachableFL).f3=((_tmp5F8.consumed=_tmp54.consumed,((_tmp5F8.may_consume=
_tmp4A.may_consume,_tmp5F8)))),_tmp5F7))))))));}_LL16:;}{struct Cyc_List_List*es2=
es->tl;for(0;es2 != 0;es2=es2->tl){{struct Cyc_Dict_Dict*_tmp5F9;env->all_changed=((
_tmp5F9=_region_malloc(env->r,sizeof(*_tmp5F9)),((_tmp5F9[0]=(env->fenv)->mt_place_set,
_tmp5F9))));}{union Cyc_CfFlowInfo_FlowInfo_union _tmp59;void*_tmp5A;struct _tuple5
_tmp58=Cyc_NewControlFlow_anal_Rexp(env,_tmp4B,(struct Cyc_Absyn_Exp*)es2->hd);
_tmp59=_tmp58.f1;_tmp5A=_tmp58.f2;{struct Cyc_List_List*_tmp5FA;rvals=((_tmp5FA=
_region_malloc(rgn,sizeof(*_tmp5FA)),((_tmp5FA->hd=(void*)_tmp5A,((_tmp5FA->tl=
rvals,_tmp5FA))))));}outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp59,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);}}}{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp5D;struct _tuple6 _tmp5C=Cyc_CfFlowInfo_save_consume_info(
env->fenv,outflow,0);_tmp5D=_tmp5C.f1;if(init_consume)old_inflow=Cyc_CfFlowInfo_restore_consume_info(
_tmp4B,outflow_consume);else{old_inflow=_tmp4B;}init_consume=1;outflow_consume=
_tmp5D;_tmp4B=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,_tmp4B,
outflow,1);}}}while(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp4B,old_inflow));if(
outer_all_changed == 0)env->all_changed=0;else{struct Cyc_Dict_Dict*_tmp5FB;env->all_changed=((
_tmp5FB=_region_malloc(env->r,sizeof(*_tmp5FB)),((_tmp5FB[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0),_tmp5FB))));}Cyc_NewControlFlow_update_tryflow(
env,outflow);{struct _tuple11 _tmp5FC;return(_tmp5FC.f1=outflow,((_tmp5FC.f2=Cyc_List_imp_rev(
rvals),_tmp5FC)));}}}}static struct _tuple5 Cyc_NewControlFlow_anal_use_ints(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct
Cyc_List_List*es,int arg1_unconsumed);static struct _tuple5 Cyc_NewControlFlow_anal_use_ints(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_List_List*es,int arg1_unconsumed){struct _RegionHandle _tmp61=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp61;_push_region(rgn);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp63;struct Cyc_List_List*_tmp64;struct _tuple11 _tmp62=Cyc_NewControlFlow_anal_unordered_Rexps(
rgn,env,inflow,es,arg1_unconsumed);_tmp63=_tmp62.f1;_tmp64=_tmp62.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp65=_tmp63;struct Cyc_Dict_Dict _tmp66;_LL1C: if((_tmp65.ReachableFL).tag != 1)
goto _LL1E;_tmp66=(_tmp65.ReachableFL).f1;_LL1D: for(0;_tmp64 != 0;(_tmp64=_tmp64->tl,
es=((struct Cyc_List_List*)_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(
_tmp66,(void*)_tmp64->hd)== (void*)0){const char*_tmp5FF;void*_tmp5FE;(_tmp5FE=0,
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((
_tmp5FF="expression may not be initialized",_tag_dyneither(_tmp5FF,sizeof(char),
34))),_tag_dyneither(_tmp5FE,sizeof(void*),0)));}}goto _LL1B;_LL1E: if((_tmp65.BottomFL).tag
!= 0)goto _LL1B;_LL1F: goto _LL1B;_LL1B:;}{struct _tuple5 _tmp600;struct _tuple5 _tmp6A=(
_tmp600.f1=_tmp63,((_tmp600.f2=(void*)(env->fenv)->unknown_all,_tmp600)));
_npop_handler(0);return _tmp6A;}};_pop_region(rgn);}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Absyn_Exp*e,void*il);
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Absyn_Exp*e,void*il){union Cyc_CfFlowInfo_FlowInfo_union _tmp6B=
outflow;struct Cyc_Dict_Dict _tmp6C;struct Cyc_List_List*_tmp6D;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp6E;_LL21: if((_tmp6B.BottomFL).tag != 0)goto _LL23;_LL22: return outflow;_LL23:
if((_tmp6B.ReachableFL).tag != 1)goto _LL20;_tmp6C=(_tmp6B.ReachableFL).f1;_tmp6D=(
_tmp6B.ReachableFL).f2;_tmp6E=(_tmp6B.ReachableFL).f3;_LL24: {union Cyc_CfFlowInfo_AbsLVal_union
_tmp6F=(Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*
_tmp70;_LL26: if((_tmp6F.UnknownL).tag != 1)goto _LL28;_LL27: return outflow;_LL28:
if((_tmp6F.PlaceL).tag != 0)goto _LL25;_tmp70=(_tmp6F.PlaceL).f1;_LL29: {void*
nzval=il == (void*)2?(void*)1:(void*)2;union Cyc_CfFlowInfo_FlowInfo_union _tmp601;
union Cyc_CfFlowInfo_FlowInfo_union _tmp71=((_tmp601.ReachableFL).tag=1,(((_tmp601.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp6C,env->all_changed,_tmp70,
nzval),(((_tmp601.ReachableFL).f2=_tmp6D,(((_tmp601.ReachableFL).f3=_tmp6E,
_tmp601)))))));return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp71;}_LL25:;}_LL20:;}
static struct _tuple8 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Absyn_Exp*e,void*il);static struct _tuple8 Cyc_NewControlFlow_splitzero(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Absyn_Exp*e,void*il){union
Cyc_CfFlowInfo_FlowInfo_union _tmp73=outflow;struct Cyc_Dict_Dict _tmp74;struct Cyc_List_List*
_tmp75;struct Cyc_CfFlowInfo_ConsumeInfo _tmp76;_LL2B: if((_tmp73.BottomFL).tag != 0)
goto _LL2D;_LL2C: {struct _tuple8 _tmp602;return(_tmp602.f1=outflow,((_tmp602.f2=
outflow,_tmp602)));}_LL2D: if((_tmp73.ReachableFL).tag != 1)goto _LL2A;_tmp74=(
_tmp73.ReachableFL).f1;_tmp75=(_tmp73.ReachableFL).f2;_tmp76=(_tmp73.ReachableFL).f3;
_LL2E: {union Cyc_CfFlowInfo_AbsLVal_union _tmp78=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp79;_LL30: if((_tmp78.UnknownL).tag
!= 1)goto _LL32;_LL31: {struct _tuple8 _tmp603;return(_tmp603.f1=outflow,((_tmp603.f2=
outflow,_tmp603)));}_LL32: if((_tmp78.PlaceL).tag != 0)goto _LL2F;_tmp79=(_tmp78.PlaceL).f1;
_LL33: {void*nzval=il == (void*)2?(void*)1:(void*)2;union Cyc_CfFlowInfo_FlowInfo_union
_tmp608;union Cyc_CfFlowInfo_FlowInfo_union _tmp607;struct _tuple8 _tmp606;return(
_tmp606.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp608.ReachableFL).tag=1,(((
_tmp608.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp74,env->all_changed,
_tmp79,nzval),(((_tmp608.ReachableFL).f2=_tmp75,(((_tmp608.ReachableFL).f3=
_tmp76,_tmp608)))))))),((_tmp606.f2=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp607.ReachableFL).tag=1,(((_tmp607.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp74,env->all_changed,_tmp79,(void*)0),(((_tmp607.ReachableFL).f2=
_tmp75,(((_tmp607.ReachableFL).f3=_tmp76,_tmp607)))))))),_tmp606)));}_LL2F:;}
_LL2A:;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_if_tagcmp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union flow,
struct Cyc_Absyn_Exp*e1,void*r1,void*p,void*r2);static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_if_tagcmp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
flow,struct Cyc_Absyn_Exp*e1,void*r1,void*p,void*r2){union Cyc_CfFlowInfo_FlowInfo_union
_tmp7E=flow;struct Cyc_Dict_Dict _tmp7F;struct Cyc_List_List*_tmp80;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp81;_LL35: if((_tmp7E.BottomFL).tag != 0)goto _LL37;_LL36: return flow;_LL37: if((
_tmp7E.ReachableFL).tag != 1)goto _LL34;_tmp7F=(_tmp7E.ReachableFL).f1;_tmp80=(
_tmp7E.ReachableFL).f2;_tmp81=(_tmp7E.ReachableFL).f3;_LL38: {void*_tmp82=r2;
struct Cyc_List_List*_tmp83;_LL3A: if(_tmp82 <= (void*)3)goto _LL3C;if(*((int*)
_tmp82)!= 3)goto _LL3C;_tmp83=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp82)->f1;
_LL3B: {union Cyc_CfFlowInfo_AbsLVal_union _tmp84=(Cyc_NewControlFlow_anal_Lexp(
env,flow,e1)).f2;struct Cyc_CfFlowInfo_Place*_tmp85;_LL3F: if((_tmp84.UnknownL).tag
!= 1)goto _LL41;_LL40: return flow;_LL41: if((_tmp84.PlaceL).tag != 0)goto _LL3E;
_tmp85=(_tmp84.PlaceL).f1;_LL42: {struct Cyc_List_List*new_cl;{void*_tmp86=r1;
struct Cyc_List_List*_tmp87;void*_tmp88;_LL44: if(_tmp86 <= (void*)3)goto _LL48;if(*((
int*)_tmp86)!= 3)goto _LL46;_tmp87=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp86)->f1;
_LL45: new_cl=_tmp87;goto _LL43;_LL46: if(*((int*)_tmp86)!= 0)goto _LL48;_tmp88=(
void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp86)->f1;if((int)_tmp88 != 2)
goto _LL48;_LL47: goto _LL49;_LL48: if((int)_tmp86 != 0)goto _LL4A;_LL49: goto _LL4B;
_LL4A: if((int)_tmp86 != 1)goto _LL4C;_LL4B: new_cl=0;goto _LL43;_LL4C:;_LL4D: return
flow;_LL43:;}for(0;_tmp83 != 0;_tmp83=_tmp83->tl){void*new_cmp;{struct _tuple0
_tmp609;struct _tuple0 _tmp8A=(_tmp609.f1=p,((_tmp609.f2=(void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp83->hd)->cmp,_tmp609)));void*_tmp8B;void*_tmp8C;void*_tmp8D;void*_tmp8E;
_LL4F: _tmp8B=_tmp8A.f1;if((int)_tmp8B != 8)goto _LL51;_LL50: goto _LL52;_LL51: _tmp8C=
_tmp8A.f2;if((int)_tmp8C != 8)goto _LL53;_LL52: new_cmp=(void*)8;goto _LL4E;_LL53:
_tmp8D=_tmp8A.f1;if((int)_tmp8D != 5)goto _LL55;_tmp8E=_tmp8A.f2;if((int)_tmp8E != 
5)goto _LL55;_LL54: new_cmp=(void*)5;goto _LL4E;_LL55:;_LL56: new_cmp=(void*)10;goto
_LL4E;_LL4E:;}{struct Cyc_CfFlowInfo_TagCmp*_tmp60C;struct Cyc_List_List*_tmp60B;
new_cl=((_tmp60B=_region_malloc(env->r,sizeof(*_tmp60B)),((_tmp60B->hd=((_tmp60C=
_region_malloc(env->r,sizeof(*_tmp60C)),((_tmp60C->cmp=(void*)new_cmp,((_tmp60C->bd=(
void*)((void*)((struct Cyc_CfFlowInfo_TagCmp*)_tmp83->hd)->bd),_tmp60C)))))),((
_tmp60B->tl=new_cl,_tmp60B))))));}}{struct Cyc_CfFlowInfo_TagCmps_struct*_tmp612;
struct Cyc_CfFlowInfo_TagCmps_struct _tmp611;union Cyc_CfFlowInfo_FlowInfo_union
_tmp610;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp610.ReachableFL).tag=1,(((
_tmp610.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(env->fenv,e1->loc,_tmp7F,env->all_changed,
_tmp85,(void*)((_tmp612=_region_malloc(env->r,sizeof(*_tmp612)),((_tmp612[0]=((
_tmp611.tag=3,((_tmp611.f1=new_cl,_tmp611)))),_tmp612))))),(((_tmp610.ReachableFL).f2=
_tmp80,(((_tmp610.ReachableFL).f3=_tmp81,_tmp610))))))));}}_LL3E:;}_LL3C:;_LL3D:
return flow;_LL39:;}_LL34:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct Cyc_NewControlFlow_mt_unknownz_v={
Cyc_CfFlowInfo_UnknownZ,0};static struct _tuple5 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_Absyn_Exp*e,void*r);static struct
_tuple5 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union f,struct
Cyc_Absyn_Exp*e,void*r){void*_tmp96=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp97;void*_tmp98;struct Cyc_Absyn_PtrAtts
_tmp99;struct Cyc_Absyn_Conref*_tmp9A;struct Cyc_Absyn_Conref*_tmp9B;_LL58: if(
_tmp96 <= (void*)4)goto _LL5A;if(*((int*)_tmp96)!= 4)goto _LL5A;_tmp97=((struct Cyc_Absyn_PointerType_struct*)
_tmp96)->f1;_tmp98=(void*)_tmp97.elt_typ;_tmp99=_tmp97.ptr_atts;_tmp9A=_tmp99.bounds;
_tmp9B=_tmp99.zero_term;_LL59: {union Cyc_CfFlowInfo_FlowInfo_union _tmp9C=f;
struct Cyc_Dict_Dict _tmp9D;struct Cyc_List_List*_tmp9E;_LL5D: if((_tmp9C.BottomFL).tag
!= 0)goto _LL5F;_LL5E: {struct _tuple5 _tmp613;return(_tmp613.f1=f,((_tmp613.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp98,(void*)(env->fenv)->unknown_all),_tmp613)));}_LL5F: if((_tmp9C.ReachableFL).tag
!= 1)goto _LL5C;_tmp9D=(_tmp9C.ReachableFL).f1;_tmp9E=(_tmp9C.ReachableFL).f2;
_LL60: if(Cyc_Tcutil_is_bound_one(_tmp9A)){void*_tmpA0=r;struct Cyc_CfFlowInfo_Place*
_tmpA1;void*_tmpA2;_LL62: if((int)_tmpA0 != 1)goto _LL64;_LL63: goto _LL65;_LL64: if((
int)_tmpA0 != 2)goto _LL66;_LL65:{void*_tmpA3=(void*)e->annot;struct Cyc_List_List*
_tmpA4;_LL6F: if(*((void**)_tmpA3)!= Cyc_CfFlowInfo_NotZero)goto _LL71;_tmpA4=((
struct Cyc_CfFlowInfo_NotZero_struct*)_tmpA3)->f1;_LL70: if(!Cyc_CfFlowInfo_same_relns(
_tmp9E,_tmpA4))goto _LL72;goto _LL6E;_LL71:;_LL72:{void*_tmpA5=(void*)e->r;_LL74:
if(*((int*)_tmpA5)!= 25)goto _LL76;_LL75:{struct Cyc_CfFlowInfo_NotZero_struct
_tmp616;struct Cyc_CfFlowInfo_NotZero_struct*_tmp615;(void*)(e->annot=(void*)((
void*)((_tmp615=_cycalloc(sizeof(*_tmp615)),((_tmp615[0]=((_tmp616.tag=Cyc_CfFlowInfo_NotZero,((
_tmp616.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9E),_tmp616)))),
_tmp615))))));}goto _LL73;_LL76:;_LL77:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_notzero_v));
goto _LL73;_LL73:;}goto _LL6E;_LL6E:;}goto _LL61;_LL66: if(_tmpA0 <= (void*)3)goto
_LL68;if(*((int*)_tmpA0)!= 2)goto _LL68;_tmpA1=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpA0)->f1;_LL67:{void*_tmpA8=(void*)e->annot;struct Cyc_List_List*_tmpA9;_LL79:
if(*((void**)_tmpA8)!= Cyc_CfFlowInfo_NotZero)goto _LL7B;_tmpA9=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpA8)->f1;_LL7A: if(!Cyc_CfFlowInfo_same_relns(_tmp9E,_tmpA9))goto _LL7C;goto
_LL78;_LL7B:;_LL7C:{void*_tmpAA=(void*)e->r;_LL7E: if(*((int*)_tmpAA)!= 25)goto
_LL80;_LL7F:{struct Cyc_CfFlowInfo_NotZero_struct _tmp619;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp618;(void*)(e->annot=(void*)((void*)((_tmp618=_cycalloc(sizeof(*_tmp618)),((
_tmp618[0]=((_tmp619.tag=Cyc_CfFlowInfo_NotZero,((_tmp619.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp9E),_tmp619)))),_tmp618))))));}goto _LL7D;_LL80:;_LL81:(
void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_notzero_v));goto _LL7D;
_LL7D:;}goto _LL78;_LL78:;}{struct _tuple5 _tmp61A;return(_tmp61A.f1=f,((_tmp61A.f2=
Cyc_CfFlowInfo_lookup_place(_tmp9D,_tmpA1),_tmp61A)));}_LL68: if((int)_tmpA0 != 0)
goto _LL6A;_LL69:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));{union
Cyc_CfFlowInfo_FlowInfo_union _tmp61D;struct _tuple5 _tmp61C;return(_tmp61C.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp61D.BottomFL).tag=0,_tmp61D)),((
_tmp61C.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp98,(void*)(env->fenv)->unknown_all),
_tmp61C)));}_LL6A: if(_tmpA0 <= (void*)3)goto _LL6C;if(*((int*)_tmpA0)!= 0)goto
_LL6C;_tmpA2=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpA0)->f1;_LL6B: f=
Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpA2);goto _LL6D;_LL6C:;_LL6D:{void*
_tmpB0=(void*)e->r;_LL83: if(*((int*)_tmpB0)!= 25)goto _LL85;_LL84:{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp620;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp61F;(void*)(e->annot=(void*)((
void*)((_tmp61F=_cycalloc(sizeof(*_tmp61F)),((_tmp61F[0]=((_tmp620.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp620.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9E),_tmp620)))),
_tmp61F))))));}goto _LL82;_LL85:;_LL86:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_unknownz_v));
goto _LL82;_LL82:;}goto _LL61;_LL61:;}else{void*_tmpB3=(void*)e->annot;struct Cyc_List_List*
_tmpB4;_LL88: if(*((void**)_tmpB3)!= Cyc_CfFlowInfo_UnknownZ)goto _LL8A;_tmpB4=((
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmpB3)->f1;_LL89: if(!Cyc_CfFlowInfo_same_relns(
_tmp9E,_tmpB4))goto _LL8B;goto _LL87;_LL8A:;_LL8B:{void*_tmpB5=(void*)e->r;_LL8D:
if(*((int*)_tmpB5)!= 25)goto _LL8F;_LL8E:{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp623;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp622;(void*)(e->annot=(void*)((
void*)((_tmp622=_cycalloc(sizeof(*_tmp622)),((_tmp622[0]=((_tmp623.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp623.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9E),_tmp623)))),
_tmp622))))));}goto _LL8C;_LL8F:;_LL90:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_unknownz_v));
goto _LL8C;_LL8C:;}goto _LL87;_LL87:;}{void*_tmpB8=Cyc_CfFlowInfo_initlevel(_tmp9D,
r);_LL92: if((int)_tmpB8 != 0)goto _LL94;_LL93:{const char*_tmp626;void*_tmp625;(
_tmp625=0,Cyc_Tcutil_terr(e->loc,((_tmp626="dereference of possibly uninitialized pointer",
_tag_dyneither(_tmp626,sizeof(char),46))),_tag_dyneither(_tmp625,sizeof(void*),0)));}
goto _LL95;_LL94: if((int)_tmpB8 != 2)goto _LL96;_LL95: {struct _tuple5 _tmp627;return(
_tmp627.f1=f,((_tmp627.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp98,(void*)(
env->fenv)->unknown_all),_tmp627)));}_LL96: if((int)_tmpB8 != 1)goto _LL91;_LL97: {
struct _tuple5 _tmp628;return(_tmp628.f1=f,((_tmp628.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp98,(void*)(env->fenv)->unknown_none),_tmp628)));}_LL91:;}_LL5C:;}
_LL5A:;_LL5B: {struct Cyc_Core_Impossible_struct _tmp62E;const char*_tmp62D;struct
Cyc_Core_Impossible_struct*_tmp62C;(int)_throw((void*)((_tmp62C=_cycalloc(
sizeof(*_tmp62C)),((_tmp62C[0]=((_tmp62E.tag=Cyc_Core_Impossible,((_tmp62E.f1=((
_tmp62D="right deref of non-pointer-type",_tag_dyneither(_tmp62D,sizeof(char),32))),
_tmp62E)))),_tmp62C)))));}_LL57:;}static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(
struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2);static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(
struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){void*_tmpC0=(void*)e1->r;void*_tmpC1;struct Cyc_Absyn_Vardecl*
_tmpC2;void*_tmpC3;struct Cyc_Absyn_Vardecl*_tmpC4;void*_tmpC5;struct Cyc_Absyn_Vardecl*
_tmpC6;void*_tmpC7;struct Cyc_Absyn_Vardecl*_tmpC8;_LL99: if(*((int*)_tmpC0)!= 1)
goto _LL9B;_tmpC1=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpC0)->f2;if(_tmpC1 <= (
void*)1)goto _LL9B;if(*((int*)_tmpC1)!= 4)goto _LL9B;_tmpC2=((struct Cyc_Absyn_Pat_b_struct*)
_tmpC1)->f1;_LL9A: _tmpC4=_tmpC2;goto _LL9C;_LL9B: if(*((int*)_tmpC0)!= 1)goto _LL9D;
_tmpC3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpC0)->f2;if(_tmpC3 <= (void*)1)
goto _LL9D;if(*((int*)_tmpC3)!= 3)goto _LL9D;_tmpC4=((struct Cyc_Absyn_Local_b_struct*)
_tmpC3)->f1;_LL9C: _tmpC6=_tmpC4;goto _LL9E;_LL9D: if(*((int*)_tmpC0)!= 1)goto _LL9F;
_tmpC5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpC0)->f2;if(_tmpC5 <= (void*)1)
goto _LL9F;if(*((int*)_tmpC5)!= 2)goto _LL9F;_tmpC6=((struct Cyc_Absyn_Param_b_struct*)
_tmpC5)->f1;_LL9E: _tmpC8=_tmpC6;goto _LLA0;_LL9F: if(*((int*)_tmpC0)!= 1)goto _LLA1;
_tmpC7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpC0)->f2;if(_tmpC7 <= (void*)1)
goto _LLA1;if(*((int*)_tmpC7)!= 0)goto _LLA1;_tmpC8=((struct Cyc_Absyn_Global_b_struct*)
_tmpC7)->f1;_LLA0: if(!_tmpC8->escapes){void*_tmpC9=(void*)e2->r;void*_tmpCA;
struct Cyc_Absyn_Vardecl*_tmpCB;void*_tmpCC;struct Cyc_Absyn_Vardecl*_tmpCD;void*
_tmpCE;struct Cyc_Absyn_Vardecl*_tmpCF;void*_tmpD0;struct Cyc_Absyn_Vardecl*_tmpD1;
_LLA4: if(*((int*)_tmpC9)!= 1)goto _LLA6;_tmpCA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC9)->f2;if(_tmpCA <= (void*)1)goto _LLA6;if(*((int*)_tmpCA)!= 4)goto _LLA6;
_tmpCB=((struct Cyc_Absyn_Pat_b_struct*)_tmpCA)->f1;_LLA5: _tmpCD=_tmpCB;goto _LLA7;
_LLA6: if(*((int*)_tmpC9)!= 1)goto _LLA8;_tmpCC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC9)->f2;if(_tmpCC <= (void*)1)goto _LLA8;if(*((int*)_tmpCC)!= 3)goto _LLA8;
_tmpCD=((struct Cyc_Absyn_Local_b_struct*)_tmpCC)->f1;_LLA7: _tmpCF=_tmpCD;goto
_LLA9;_LLA8: if(*((int*)_tmpC9)!= 1)goto _LLAA;_tmpCE=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC9)->f2;if(_tmpCE <= (void*)1)goto _LLAA;if(*((int*)_tmpCE)!= 2)goto _LLAA;
_tmpCF=((struct Cyc_Absyn_Param_b_struct*)_tmpCE)->f1;_LLA9: _tmpD1=_tmpCF;goto
_LLAB;_LLAA: if(*((int*)_tmpC9)!= 1)goto _LLAC;_tmpD0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC9)->f2;if(_tmpD0 <= (void*)1)goto _LLAC;if(*((int*)_tmpD0)!= 0)goto _LLAC;
_tmpD1=((struct Cyc_Absyn_Global_b_struct*)_tmpD0)->f1;_LLAB: if(!_tmpD1->escapes){
int found=0;{struct Cyc_List_List*_tmpD2=relns;for(0;_tmpD2 != 0;_tmpD2=_tmpD2->tl){
struct Cyc_CfFlowInfo_Reln*_tmpD3=(struct Cyc_CfFlowInfo_Reln*)_tmpD2->hd;if(
_tmpD3->vd == _tmpD1){union Cyc_CfFlowInfo_RelnOp_union _tmpD4=_tmpD3->rop;struct
Cyc_Absyn_Vardecl*_tmpD5;_LLAF: if((_tmpD4.LessNumelts).tag != 2)goto _LLB1;_tmpD5=(
_tmpD4.LessNumelts).f1;if(!(_tmpD5 == _tmpC8))goto _LLB1;_LLB0: return relns;_LLB1:;
_LLB2: continue;_LLAE:;}}}if(!found){struct Cyc_CfFlowInfo_Reln*_tmp634;union Cyc_CfFlowInfo_RelnOp_union
_tmp633;struct Cyc_List_List*_tmp632;return(_tmp632=_region_malloc(rgn,sizeof(*
_tmp632)),((_tmp632->hd=((_tmp634=_region_malloc(rgn,sizeof(*_tmp634)),((_tmp634->vd=
_tmpD1,((_tmp634->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((_tmp633.LessNumelts).tag=
2,(((_tmp633.LessNumelts).f1=_tmpC8,_tmp633)))),_tmp634)))))),((_tmp632->tl=
relns,_tmp632)))));}}return relns;_LLAC:;_LLAD: return relns;_LLA3:;}else{return
relns;}_LLA1:;_LLA2: return relns;_LL98:;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_may_consume_place(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_CfFlowInfo_Place*place,void*t,struct
Cyc_List_List**ps);static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_may_consume_place(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,
struct Cyc_CfFlowInfo_Place*place,void*t,struct Cyc_List_List**ps){struct Cyc_CfFlowInfo_FlowEnv*
_tmpD9=env->fenv;struct Cyc_List_List*_tmpDA=Cyc_NewControlFlow_noalias_ptrs_rec(
env,place,t);if(ps != 0)*ps=_tmpDA;if(_tmpDA == 0)return cinfo;cinfo.may_consume=((
struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(
_tmpD9->r,Cyc_CfFlowInfo_place_cmp,_tmpDA,cinfo.may_consume);return cinfo;}struct
_tuple12{union Cyc_CfFlowInfo_AbsLVal_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_may_consume_exp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_may_consume_exp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_noalias_path(e)){struct Cyc_CfFlowInfo_FlowEnv*
_tmpDB=env->fenv;union Cyc_CfFlowInfo_AbsLVal_union _tmpDD;struct _tuple7 _tmpDC=Cyc_NewControlFlow_anal_Lexp(
env,inflow,e);_tmpDD=_tmpDC.f2;{struct _tuple12 _tmp635;struct _tuple12 _tmpDF=(
_tmp635.f1=_tmpDD,((_tmp635.f2=inflow,_tmp635)));union Cyc_CfFlowInfo_AbsLVal_union
_tmpE0;struct Cyc_CfFlowInfo_Place*_tmpE1;union Cyc_CfFlowInfo_FlowInfo_union
_tmpE2;struct Cyc_Dict_Dict _tmpE3;struct Cyc_List_List*_tmpE4;struct Cyc_CfFlowInfo_ConsumeInfo
_tmpE5;_LLB4: _tmpE0=_tmpDF.f1;if(((_tmpDF.f1).PlaceL).tag != 0)goto _LLB6;_tmpE1=(
_tmpE0.PlaceL).f1;_tmpE2=_tmpDF.f2;if(((_tmpDF.f2).ReachableFL).tag != 1)goto
_LLB6;_tmpE3=(_tmpE2.ReachableFL).f1;_tmpE4=(_tmpE2.ReachableFL).f2;_tmpE5=(
_tmpE2.ReachableFL).f3;_LLB5: {struct Cyc_CfFlowInfo_ConsumeInfo _tmpE6=Cyc_NewControlFlow_may_consume_place(
env,_tmpE5,_tmpE1,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0);union
Cyc_CfFlowInfo_FlowInfo_union _tmp636;return(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp636.ReachableFL).tag=1,(((_tmp636.ReachableFL).f1=_tmpE3,(((_tmp636.ReachableFL).f2=
_tmpE4,(((_tmp636.ReachableFL).f3=_tmpE6,_tmp636))))))));}_LLB6:;_LLB7:{const
char*_tmp63A;void*_tmp639[1];struct Cyc_String_pa_struct _tmp638;(_tmp638.tag=0,((
_tmp638.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmp639[0]=& _tmp638,Cyc_fprintf(Cyc_stderr,((_tmp63A="Oops---no location for noalias_path |%s|\n",
_tag_dyneither(_tmp63A,sizeof(char),42))),_tag_dyneither(_tmp639,sizeof(void*),1)))))));}
return inflow;_LLB3:;}}return inflow;}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_consume_place(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo
cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Position_Segment*loc);static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_consume_place(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*
p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Position_Segment*
loc){if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(
cinfo.consumed,p)){struct Cyc_CfFlowInfo_Place _tmpEC;void*_tmpED;struct Cyc_List_List*
_tmpEE;struct Cyc_CfFlowInfo_Place*_tmpEB=p;_tmpEC=*_tmpEB;_tmpED=(void*)_tmpEC.root;
_tmpEE=_tmpEC.fields;{void*rval=(void*)0;if((Cyc_Dict_lookup_bool(outdict,_tmpED,&
rval) && Cyc_CfFlowInfo_initlevel(outdict,rval)!= (void*)0) && rval != (void*)0){
void*_tmpEF=_tmpED;struct Cyc_Absyn_Vardecl*_tmpF0;_LLB9: if(*((int*)_tmpEF)!= 0)
goto _LLBB;_tmpF0=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmpEF)->f1;_LLBA: {
struct _dyneither_ptr _tmpF1=Cyc_Absynpp_qvar2string(_tmpF0->name);if(_tmpEE == 0){
const char*_tmp63E;void*_tmp63D[1];struct Cyc_String_pa_struct _tmp63C;(_tmp63C.tag=
0,((_tmp63C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpF1),((_tmp63D[0]=&
_tmp63C,Cyc_Tcutil_warn(loc,((_tmp63E="may clobber unique pointer %s",
_tag_dyneither(_tmp63E,sizeof(char),30))),_tag_dyneither(_tmp63D,sizeof(void*),1)))))));}
else{const char*_tmp642;void*_tmp641[1];struct Cyc_String_pa_struct _tmp640;(
_tmp640.tag=0,((_tmp640.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpF1),((
_tmp641[0]=& _tmp640,Cyc_Tcutil_warn(loc,((_tmp642="may clobber unique pointer contained in %s",
_tag_dyneither(_tmp642,sizeof(char),43))),_tag_dyneither(_tmp641,sizeof(void*),1)))))));}
goto _LLB8;}_LLBB:;_LLBC: {const char*_tmp645;void*_tmp644;(_tmp644=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp645="warning locations not for VarRoots",
_tag_dyneither(_tmp645,sizeof(char),35))),_tag_dyneither(_tmp644,sizeof(void*),0)));}
_LLB8:;}}}if(env->all_changed == 0)cinfo.consumed=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_rdelete)((
env->fenv)->r,cinfo.consumed,p);return cinfo;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_consume_assignment(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict
outdict,struct Cyc_Absyn_Exp*e);static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_consume_assignment(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo
cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_noalias_path(
e)){struct Cyc_CfFlowInfo_FlowEnv*_tmpFA=env->fenv;struct Cyc_List_List*_tmpFB=0;
cinfo=Cyc_NewControlFlow_may_consume_place(env,cinfo,p,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,(struct Cyc_List_List**)& _tmpFB);while(_tmpFB != 0){cinfo=
Cyc_NewControlFlow_consume_place(env,(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)
_check_null(_tmpFB))->hd,cinfo,outdict,e->loc);_tmpFB=((struct Cyc_List_List*)
_check_null(_tmpFB))->tl;}}return cinfo;}static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_aux(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*
ts);static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_aux(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*ts){struct Cyc_List_List*l=0;
struct Cyc_CfFlowInfo_Place _tmpFD;void*_tmpFE;struct Cyc_List_List*_tmpFF;struct
Cyc_CfFlowInfo_Place*_tmpFC=p;_tmpFD=*_tmpFC;_tmpFE=(void*)_tmpFD.root;_tmpFF=
_tmpFD.fields;{int fld=0;for(0;ts != 0;(fld ++,ts=ts->tl)){void*_tmp100=(void*)ts->hd;
if(Cyc_Tcutil_is_noalias_pointer(_tmp100)){int _tmp646[1];struct Cyc_List_List*
_tmp101=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_rappend)(env->r,_tmpFF,((_tmp646[0]=fld,((struct
Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,
_tag_dyneither(_tmp646,sizeof(int),1)))));struct Cyc_CfFlowInfo_Place*_tmp649;
struct Cyc_List_List*_tmp648;l=((struct Cyc_List_List*(*)(struct _RegionHandle*,int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,((
_tmp648=_region_malloc(env->r,sizeof(*_tmp648)),((_tmp648->hd=((_tmp649=
_region_malloc(env->r,sizeof(*_tmp649)),((_tmp649->root=(void*)_tmpFE,((_tmp649->fields=
_tmp101,_tmp649)))))),((_tmp648->tl=0,_tmp648)))))),l);}else{if(Cyc_Absyn_is_aggr_type(
_tmp100)){int _tmp64A[1];struct Cyc_List_List*_tmp105=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
env->r,_tmpFF,((_tmp64A[0]=fld,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(_tmp64A,sizeof(int),
1)))));struct Cyc_CfFlowInfo_Place*_tmp64B;struct Cyc_CfFlowInfo_Place*_tmp106=(
_tmp64B=_region_malloc(env->r,sizeof(*_tmp64B)),((_tmp64B->root=(void*)_tmpFE,((
_tmp64B->fields=_tmp105,_tmp64B)))));l=((struct Cyc_List_List*(*)(struct
_RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,
l,Cyc_NewControlFlow_noalias_ptrs_rec(env,_tmp106,_tmp100));}}}}return l;}struct
_tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);static
struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,void*t){if(Cyc_Tcutil_is_noalias_pointer(t)){
struct Cyc_List_List*_tmp64C;return(_tmp64C=_region_malloc(env->r,sizeof(*_tmp64C)),((
_tmp64C->hd=p,((_tmp64C->tl=0,_tmp64C)))));}{void*_tmp10A=Cyc_Tcutil_compress(t);
struct Cyc_List_List*_tmp10B;struct Cyc_List_List*_tmp10C;struct Cyc_Absyn_AggrInfo
_tmp10D;union Cyc_Absyn_AggrInfoU_union _tmp10E;struct Cyc_Absyn_Aggrdecl**_tmp10F;
struct Cyc_List_List*_tmp110;struct Cyc_Absyn_AggrInfo _tmp111;union Cyc_Absyn_AggrInfoU_union
_tmp112;struct Cyc_Absyn_TunionFieldInfo _tmp113;union Cyc_Absyn_TunionFieldInfoU_union
_tmp114;struct Cyc_List_List*_tmp115;_LLBE: if(_tmp10A <= (void*)4)goto _LLCA;if(*((
int*)_tmp10A)!= 9)goto _LLC0;_tmp10B=((struct Cyc_Absyn_TupleType_struct*)_tmp10A)->f1;
_LLBF: {struct Cyc_List_List*_tmp116=0;while(_tmp10B != 0){{struct Cyc_List_List*
_tmp64D;_tmp116=((_tmp64D=_region_malloc(env->r,sizeof(*_tmp64D)),((_tmp64D->hd=(
void*)(*((struct _tuple13*)_tmp10B->hd)).f2,((_tmp64D->tl=_tmp116,_tmp64D))))));}
_tmp10B=_tmp10B->tl;}_tmp116=Cyc_List_imp_rev(_tmp116);return Cyc_NewControlFlow_noalias_ptrs_aux(
env,p,_tmp116);}_LLC0: if(*((int*)_tmp10A)!= 11)goto _LLC2;_tmp10C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp10A)->f2;_LLC1: {struct Cyc_List_List*_tmp118=0;while(_tmp10C != 0){{struct Cyc_List_List*
_tmp64E;_tmp118=((_tmp64E=_region_malloc(env->r,sizeof(*_tmp64E)),((_tmp64E->hd=(
void*)((void*)((struct Cyc_Absyn_Aggrfield*)_tmp10C->hd)->type),((_tmp64E->tl=
_tmp118,_tmp64E))))));}_tmp10C=_tmp10C->tl;}_tmp118=Cyc_List_imp_rev(_tmp118);
return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp118);}_LLC2: if(*((int*)
_tmp10A)!= 10)goto _LLC4;_tmp10D=((struct Cyc_Absyn_AggrType_struct*)_tmp10A)->f1;
_tmp10E=_tmp10D.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp10A)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LLC4;_tmp10F=(_tmp10E.KnownAggr).f1;_tmp110=_tmp10D.targs;_LLC3: if((*
_tmp10F)->impl == 0)return 0;else{struct Cyc_List_List*_tmp11A=0;{struct
_RegionHandle _tmp11B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp11B;
_push_region(rgn);{struct Cyc_List_List*_tmp11C=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmp10F)->tvs,_tmp110);struct Cyc_List_List*_tmp11D=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp10F)->impl))->fields;while(
_tmp11D != 0){{struct Cyc_List_List*_tmp64F;_tmp11A=((_tmp64F=_region_malloc(env->r,
sizeof(*_tmp64F)),((_tmp64F->hd=(void*)Cyc_Tcutil_rsubstitute(rgn,_tmp11C,(void*)((
struct Cyc_Absyn_Aggrfield*)_tmp11D->hd)->type),((_tmp64F->tl=_tmp11A,_tmp64F))))));}
_tmp11D=_tmp11D->tl;}};_pop_region(rgn);}_tmp11A=Cyc_List_imp_rev(_tmp11A);
return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp11A);}_LLC4: if(*((int*)
_tmp10A)!= 10)goto _LLC6;_tmp111=((struct Cyc_Absyn_AggrType_struct*)_tmp10A)->f1;
_tmp112=_tmp111.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp10A)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LLC6;_LLC5: {const char*_tmp652;void*_tmp651;(_tmp651=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp652="got unknown aggr in noalias_ptrs_rec",
_tag_dyneither(_tmp652,sizeof(char),37))),_tag_dyneither(_tmp651,sizeof(void*),0)));}
_LLC6: if(*((int*)_tmp10A)!= 2)goto _LLC8;_LLC7: if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
t)){struct Cyc_List_List*_tmp653;return(_tmp653=_region_malloc(env->r,sizeof(*
_tmp653)),((_tmp653->hd=p,((_tmp653->tl=0,_tmp653)))));}else{return 0;}_LLC8: if(*((
int*)_tmp10A)!= 3)goto _LLCA;_tmp113=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp10A)->f1;_tmp114=_tmp113.field_info;_tmp115=_tmp113.targs;_LLC9: {union Cyc_Absyn_TunionFieldInfoU_union
_tmp122=_tmp114;struct Cyc_Absyn_Tuniondecl*_tmp123;struct Cyc_Absyn_Tunionfield*
_tmp124;_LLCD: if((_tmp122.UnknownTunionfield).tag != 0)goto _LLCF;_LLCE: {const
char*_tmp656;void*_tmp655;(_tmp655=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp656="got unknown tunion field in noalias_ptrs_rec",
_tag_dyneither(_tmp656,sizeof(char),45))),_tag_dyneither(_tmp655,sizeof(void*),0)));}
_LLCF: if((_tmp122.KnownTunionfield).tag != 1)goto _LLCC;_tmp123=(_tmp122.KnownTunionfield).f1;
_tmp124=(_tmp122.KnownTunionfield).f2;_LLD0: {struct Cyc_List_List*_tmp127=0;{
struct _RegionHandle _tmp128=_new_region("rgn");struct _RegionHandle*rgn=& _tmp128;
_push_region(rgn);{struct Cyc_List_List*_tmp129=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp123->tvs,_tmp115);struct Cyc_List_List*_tmp12A=
_tmp124->typs;while(_tmp12A != 0){{struct Cyc_List_List*_tmp657;_tmp127=((_tmp657=
_region_malloc(env->r,sizeof(*_tmp657)),((_tmp657->hd=(void*)Cyc_Tcutil_rsubstitute(
rgn,_tmp129,(*((struct _tuple13*)_tmp12A->hd)).f2),((_tmp657->tl=_tmp127,_tmp657))))));}
_tmp12A=_tmp12A->tl;}};_pop_region(rgn);}_tmp127=Cyc_List_imp_rev(_tmp127);
return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp127);}_LLCC:;}_LLCA:;_LLCB:
return 0;_LLBD:;}}static struct _tuple5 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal_union lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_Position_Segment*loc);static struct _tuple5 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal_union lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_Position_Segment*loc){outflow=Cyc_CfFlowInfo_consume_unique_rvals(
loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp12C=outflow;struct Cyc_Dict_Dict
_tmp12D;struct Cyc_List_List*_tmp12E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp12F;
_LLD2: if((_tmp12C.BottomFL).tag != 0)goto _LLD4;_LLD3: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp65A;struct _tuple5 _tmp659;return(_tmp659.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp65A.BottomFL).tag=0,_tmp65A)),((_tmp659.f2=rval,_tmp659)));}_LLD4: if((
_tmp12C.ReachableFL).tag != 1)goto _LLD1;_tmp12D=(_tmp12C.ReachableFL).f1;_tmp12E=(
_tmp12C.ReachableFL).f2;_tmp12F=(_tmp12C.ReachableFL).f3;_LLD5: {union Cyc_CfFlowInfo_AbsLVal_union
_tmp132=lval;struct Cyc_CfFlowInfo_Place*_tmp133;_LLD7: if((_tmp132.PlaceL).tag != 
0)goto _LLD9;_tmp133=(_tmp132.PlaceL).f1;_LLD8: _tmp12F=Cyc_NewControlFlow_consume_assignment(
env,_tmp133,_tmp12F,_tmp12D,lexp);_tmp12D=Cyc_CfFlowInfo_assign_place(fenv,loc,
_tmp12D,env->all_changed,_tmp133,rval);_tmp12E=Cyc_CfFlowInfo_reln_assign_exp(
fenv->r,_tmp12E,lexp,rexp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp65B;outflow=(
union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp65B.ReachableFL).tag=1,(((_tmp65B.ReachableFL).f1=
_tmp12D,(((_tmp65B.ReachableFL).f2=_tmp12E,(((_tmp65B.ReachableFL).f3=_tmp12F,
_tmp65B))))))));}Cyc_NewControlFlow_update_tryflow(env,outflow);{struct _tuple5
_tmp65C;return(_tmp65C.f1=outflow,((_tmp65C.f2=rval,_tmp65C)));}_LLD9: if((
_tmp132.UnknownL).tag != 1)goto _LLD6;_LLDA: {struct _tuple5 _tmp65D;return(_tmp65D.f1=
Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp65D.f2=rval,
_tmp65D)));}_LLD6:;}_LLD1:;}}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e);static void
_tmp6E9(unsigned int*_tmp6E8,unsigned int*_tmp6E7,void***_tmp6E5,struct Cyc_List_List**
_tmp2B1){for(*_tmp6E8=0;*_tmp6E8 < *_tmp6E7;(*_tmp6E8)++){void*_tmp6E3;(*_tmp6E5)[*
_tmp6E8]=((_tmp6E3=(void*)((struct Cyc_List_List*)_check_null(*_tmp2B1))->hd,((*
_tmp2B1=(*_tmp2B1)->tl,_tmp6E3))));}}static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp137=env->fenv;struct Cyc_Dict_Dict
d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo cinfo;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp138=inflow;struct Cyc_Dict_Dict _tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp13B;_LLDC: if((_tmp138.BottomFL).tag != 0)goto _LLDE;_LLDD: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp660;struct _tuple5 _tmp65F;return(_tmp65F.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp660.BottomFL).tag=0,_tmp660)),((_tmp65F.f2=(void*)_tmp137->unknown_all,
_tmp65F)));}_LLDE: if((_tmp138.ReachableFL).tag != 1)goto _LLDB;_tmp139=(_tmp138.ReachableFL).f1;
_tmp13A=(_tmp138.ReachableFL).f2;_tmp13B=(_tmp138.ReachableFL).f3;_LLDF: d=
_tmp139;relns=_tmp13A;cinfo=_tmp13B;_LLDB:;}{void*_tmp13E=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp13F;void*_tmp140;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Exp*_tmp142;
struct Cyc_Absyn_Exp*_tmp143;union Cyc_Absyn_Cnst_union _tmp144;union Cyc_Absyn_Cnst_union
_tmp145;int _tmp146;union Cyc_Absyn_Cnst_union _tmp147;void*_tmp148;struct Cyc_List_List*
_tmp149;void*_tmp14A;void*_tmp14B;struct Cyc_Absyn_Vardecl*_tmp14C;void*_tmp14D;
struct Cyc_Absyn_Vardecl*_tmp14E;void*_tmp14F;struct Cyc_Absyn_Vardecl*_tmp150;
void*_tmp151;struct Cyc_List_List*_tmp152;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Exp*
_tmp154;struct Cyc_Core_Opt*_tmp155;struct Cyc_Core_Opt _tmp156;struct Cyc_Absyn_Exp*
_tmp157;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Core_Opt*_tmp159;struct Cyc_Absyn_Exp*
_tmp15A;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15C;struct Cyc_Absyn_Exp*
_tmp15D;struct Cyc_Absyn_Exp*_tmp15E;struct Cyc_List_List*_tmp15F;struct Cyc_Absyn_MallocInfo
_tmp160;int _tmp161;struct Cyc_Absyn_Exp*_tmp162;void**_tmp163;struct Cyc_Absyn_Exp*
_tmp164;int _tmp165;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Exp*_tmp167;
struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*_tmp169;struct Cyc_Absyn_Exp*
_tmp16A;struct Cyc_Absyn_Exp*_tmp16B;struct Cyc_Absyn_Exp*_tmp16C;struct
_dyneither_ptr*_tmp16D;struct Cyc_Absyn_Exp*_tmp16E;struct _dyneither_ptr*_tmp16F;
struct Cyc_Absyn_Exp*_tmp170;struct Cyc_Absyn_Exp*_tmp171;struct Cyc_Absyn_Exp*
_tmp172;struct Cyc_Absyn_Exp*_tmp173;struct Cyc_Absyn_Exp*_tmp174;struct Cyc_Absyn_Exp*
_tmp175;struct Cyc_Absyn_Exp*_tmp176;struct Cyc_Absyn_Exp*_tmp177;struct Cyc_Absyn_Exp*
_tmp178;struct Cyc_List_List*_tmp179;struct Cyc_Absyn_Tuniondecl*_tmp17A;struct Cyc_List_List*
_tmp17B;struct Cyc_List_List*_tmp17C;struct Cyc_List_List*_tmp17D;struct Cyc_Absyn_Aggrdecl*
_tmp17E;struct Cyc_Absyn_Aggrdecl _tmp17F;struct Cyc_Absyn_AggrdeclImpl*_tmp180;
struct Cyc_Absyn_AggrdeclImpl _tmp181;struct Cyc_List_List*_tmp182;struct Cyc_List_List*
_tmp183;struct Cyc_Absyn_Vardecl*_tmp184;struct Cyc_Absyn_Exp*_tmp185;struct Cyc_Absyn_Exp*
_tmp186;int _tmp187;struct Cyc_Absyn_Stmt*_tmp188;void*_tmp189;_LLE1: if(*((int*)
_tmp13E)!= 15)goto _LLE3;_tmp13F=((struct Cyc_Absyn_Cast_e_struct*)_tmp13E)->f2;
_tmp140=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp13E)->f4;if((int)_tmp140 != 2)
goto _LLE3;_LLE2: {union Cyc_CfFlowInfo_FlowInfo_union _tmp18B;void*_tmp18C;struct
_tuple5 _tmp18A=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp13F);_tmp18B=_tmp18A.f1;
_tmp18C=_tmp18A.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp18E;void*_tmp18F;
struct _tuple5 _tmp18D=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp18B,_tmp13F,
_tmp18C);_tmp18E=_tmp18D.f1;_tmp18F=_tmp18D.f2;{struct _tuple5 _tmp661;return(
_tmp661.f1=_tmp18E,((_tmp661.f2=_tmp18C,_tmp661)));}}}_LLE3: if(*((int*)_tmp13E)
!= 15)goto _LLE5;_tmp141=((struct Cyc_Absyn_Cast_e_struct*)_tmp13E)->f2;_LLE4:
_tmp142=_tmp141;goto _LLE6;_LLE5: if(*((int*)_tmp13E)!= 13)goto _LLE7;_tmp142=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp13E)->f1;_LLE6: _tmp143=_tmp142;goto
_LLE8;_LLE7: if(*((int*)_tmp13E)!= 14)goto _LLE9;_tmp143=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp13E)->f1;_LLE8: return Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp143);_LLE9:
if(*((int*)_tmp13E)!= 0)goto _LLEB;_tmp144=((struct Cyc_Absyn_Const_e_struct*)
_tmp13E)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp13E)->f1).Null_c).tag != 
6)goto _LLEB;_LLEA: goto _LLEC;_LLEB: if(*((int*)_tmp13E)!= 0)goto _LLED;_tmp145=((
struct Cyc_Absyn_Const_e_struct*)_tmp13E)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp13E)->f1).Int_c).tag != 2)goto _LLED;_tmp146=(_tmp145.Int_c).f2;if(_tmp146 != 0)
goto _LLED;_LLEC: {struct _tuple5 _tmp662;return(_tmp662.f1=inflow,((_tmp662.f2=(
void*)0,_tmp662)));}_LLED: if(*((int*)_tmp13E)!= 0)goto _LLEF;_tmp147=((struct Cyc_Absyn_Const_e_struct*)
_tmp13E)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp13E)->f1).Int_c).tag != 2)
goto _LLEF;_LLEE: goto _LLF0;_LLEF: if(*((int*)_tmp13E)!= 1)goto _LLF1;_tmp148=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp13E)->f2;if(_tmp148 <= (void*)1)goto _LLF1;if(*((
int*)_tmp148)!= 1)goto _LLF1;_LLF0: {struct _tuple5 _tmp663;return(_tmp663.f1=
inflow,((_tmp663.f2=(void*)1,_tmp663)));}_LLF1: if(*((int*)_tmp13E)!= 32)goto
_LLF3;_tmp149=((struct Cyc_Absyn_Tunion_e_struct*)_tmp13E)->f1;if(_tmp149 != 0)
goto _LLF3;_LLF2: goto _LLF4;_LLF3: if(*((int*)_tmp13E)!= 0)goto _LLF5;_LLF4: goto
_LLF6;_LLF5: if(*((int*)_tmp13E)!= 19)goto _LLF7;_LLF6: goto _LLF8;_LLF7: if(*((int*)
_tmp13E)!= 18)goto _LLF9;_LLF8: goto _LLFA;_LLF9: if(*((int*)_tmp13E)!= 20)goto _LLFB;
_LLFA: goto _LLFC;_LLFB: if(*((int*)_tmp13E)!= 21)goto _LLFD;_LLFC: goto _LLFE;_LLFD:
if(*((int*)_tmp13E)!= 34)goto _LLFF;_LLFE: goto _LL100;_LLFF: if(*((int*)_tmp13E)!= 
33)goto _LL101;_LL100: {struct _tuple5 _tmp664;return(_tmp664.f1=inflow,((_tmp664.f2=(
void*)_tmp137->unknown_all,_tmp664)));}_LL101: if(*((int*)_tmp13E)!= 1)goto _LL103;
_tmp14A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp13E)->f2;if(_tmp14A <= (void*)
1)goto _LL103;if(*((int*)_tmp14A)!= 0)goto _LL103;_LL102: {struct _tuple5 _tmp665;
return(_tmp665.f1=inflow,((_tmp665.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp137,(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp137->unknown_all),
_tmp665)));}_LL103: if(*((int*)_tmp13E)!= 1)goto _LL105;_tmp14B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13E)->f2;if(_tmp14B <= (void*)1)goto _LL105;if(*((int*)_tmp14B)!= 2)goto _LL105;
_tmp14C=((struct Cyc_Absyn_Param_b_struct*)_tmp14B)->f1;_LL104: _tmp14E=_tmp14C;
goto _LL106;_LL105: if(*((int*)_tmp13E)!= 1)goto _LL107;_tmp14D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13E)->f2;if(_tmp14D <= (void*)1)goto _LL107;if(*((int*)_tmp14D)!= 3)goto _LL107;
_tmp14E=((struct Cyc_Absyn_Local_b_struct*)_tmp14D)->f1;_LL106: _tmp150=_tmp14E;
goto _LL108;_LL107: if(*((int*)_tmp13E)!= 1)goto _LL109;_tmp14F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13E)->f2;if(_tmp14F <= (void*)1)goto _LL109;if(*((int*)_tmp14F)!= 4)goto _LL109;
_tmp150=((struct Cyc_Absyn_Pat_b_struct*)_tmp14F)->f1;_LL108: inflow=Cyc_NewControlFlow_may_consume_exp(
env,inflow,e);{struct Cyc_CfFlowInfo_VarRoot_struct*_tmp66B;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp66A;struct _tuple5 _tmp669;return(_tmp669.f1=inflow,((_tmp669.f2=Cyc_Dict_lookup(
d,(void*)((_tmp66B=_region_malloc(env->r,sizeof(*_tmp66B)),((_tmp66B[0]=((
_tmp66A.tag=0,((_tmp66A.f1=_tmp150,_tmp66A)))),_tmp66B))))),_tmp669)));}_LL109:
if(*((int*)_tmp13E)!= 3)goto _LL10B;_tmp151=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp13E)->f1;_tmp152=((struct Cyc_Absyn_Primop_e_struct*)_tmp13E)->f2;_LL10A: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp199;void*_tmp19A;struct _tuple5 _tmp198=Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmp152,0);_tmp199=_tmp198.f1;_tmp19A=_tmp198.f2;{void*_tmp19B=
_tmp151;_LL14A: if((int)_tmp19B != 0)goto _LL14C;_LL14B: goto _LL14D;_LL14C: if((int)
_tmp19B != 2)goto _LL14E;_LL14D: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp152))->hd)->loc,_tmp199);goto _LL149;_LL14E:;
_LL14F: _tmp199=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp152))->hd)->loc,_tmp199);goto _LL149;_LL149:;}{
struct _tuple5 _tmp66C;return(_tmp66C.f1=_tmp199,((_tmp66C.f2=_tmp19A,_tmp66C)));}}
_LL10B: if(*((int*)_tmp13E)!= 5)goto _LL10D;_tmp153=((struct Cyc_Absyn_Increment_e_struct*)
_tmp13E)->f1;_LL10C: {struct Cyc_List_List _tmp66D;struct Cyc_List_List _tmp19D=(
_tmp66D.hd=_tmp153,((_tmp66D.tl=0,_tmp66D)));union Cyc_CfFlowInfo_FlowInfo_union
_tmp19F;struct _tuple5 _tmp19E=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct
Cyc_List_List*)& _tmp19D,0);_tmp19F=_tmp19E.f1;Cyc_CfFlowInfo_check_unique_rvals(
_tmp153->loc,_tmp19F);{union Cyc_CfFlowInfo_AbsLVal_union _tmp1A1;struct _tuple7
_tmp1A0=Cyc_NewControlFlow_anal_Lexp(env,_tmp19F,_tmp153);_tmp1A1=_tmp1A0.f2;{
struct _tuple12 _tmp66E;struct _tuple12 _tmp1A3=(_tmp66E.f1=_tmp1A1,((_tmp66E.f2=
_tmp19F,_tmp66E)));union Cyc_CfFlowInfo_AbsLVal_union _tmp1A4;struct Cyc_CfFlowInfo_Place*
_tmp1A5;union Cyc_CfFlowInfo_FlowInfo_union _tmp1A6;struct Cyc_Dict_Dict _tmp1A7;
struct Cyc_List_List*_tmp1A8;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1A9;_LL151:
_tmp1A4=_tmp1A3.f1;if(((_tmp1A3.f1).PlaceL).tag != 0)goto _LL153;_tmp1A5=(_tmp1A4.PlaceL).f1;
_tmp1A6=_tmp1A3.f2;if(((_tmp1A3.f2).ReachableFL).tag != 1)goto _LL153;_tmp1A7=(
_tmp1A6.ReachableFL).f1;_tmp1A8=(_tmp1A6.ReachableFL).f2;_tmp1A9=(_tmp1A6.ReachableFL).f3;
_LL152: _tmp1A8=Cyc_CfFlowInfo_reln_kill_exp(_tmp137->r,_tmp1A8,_tmp153);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp66F;_tmp19F=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp66F.ReachableFL).tag=1,(((_tmp66F.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
_tmp137,_tmp153->loc,_tmp1A7,env->all_changed,_tmp1A5,(void*)_tmp137->unknown_all),(((
_tmp66F.ReachableFL).f2=_tmp1A8,(((_tmp66F.ReachableFL).f3=_tmp1A9,_tmp66F))))))));}
goto _LL150;_LL153:;_LL154: goto _LL150;_LL150:;}{struct _tuple5 _tmp670;return(
_tmp670.f1=_tmp19F,((_tmp670.f2=(void*)_tmp137->unknown_all,_tmp670)));}}}_LL10D:
if(*((int*)_tmp13E)!= 4)goto _LL10F;_tmp154=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp13E)->f1;_tmp155=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13E)->f2;if(
_tmp155 == 0)goto _LL10F;_tmp156=*_tmp155;_tmp157=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp13E)->f3;_LL10E: {struct Cyc_List_List _tmp671;struct Cyc_List_List _tmp1AD=(
_tmp671.hd=_tmp157,((_tmp671.tl=0,_tmp671)));struct Cyc_List_List _tmp672;struct
Cyc_List_List _tmp1AE=(_tmp672.hd=_tmp154,((_tmp672.tl=(struct Cyc_List_List*)&
_tmp1AD,_tmp672)));union Cyc_CfFlowInfo_FlowInfo_union _tmp1B0;struct _tuple5
_tmp1AF=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct Cyc_List_List*)&
_tmp1AE,1);_tmp1B0=_tmp1AF.f1;{union Cyc_CfFlowInfo_AbsLVal_union _tmp1B2;struct
_tuple7 _tmp1B1=Cyc_NewControlFlow_anal_Lexp(env,_tmp1B0,_tmp154);_tmp1B2=_tmp1B1.f2;
_tmp1B0=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp1B0);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1B3=_tmp1B0;struct Cyc_Dict_Dict _tmp1B4;struct Cyc_List_List*_tmp1B5;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1B6;_LL156: if((_tmp1B3.ReachableFL).tag != 1)goto _LL158;_tmp1B4=(_tmp1B3.ReachableFL).f1;
_tmp1B5=(_tmp1B3.ReachableFL).f2;_tmp1B6=(_tmp1B3.ReachableFL).f3;_LL157:{union
Cyc_CfFlowInfo_AbsLVal_union _tmp1B7=_tmp1B2;struct Cyc_CfFlowInfo_Place*_tmp1B8;
_LL15B: if((_tmp1B7.PlaceL).tag != 0)goto _LL15D;_tmp1B8=(_tmp1B7.PlaceL).f1;_LL15C:
_tmp1B6=Cyc_NewControlFlow_consume_assignment(env,_tmp1B8,_tmp1B6,_tmp1B4,
_tmp154);_tmp1B5=Cyc_CfFlowInfo_reln_kill_exp(_tmp137->r,_tmp1B5,_tmp154);
_tmp1B4=Cyc_CfFlowInfo_assign_place(_tmp137,_tmp154->loc,_tmp1B4,env->all_changed,
_tmp1B8,(void*)_tmp137->unknown_all);{union Cyc_CfFlowInfo_FlowInfo_union _tmp673;
_tmp1B0=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp673.ReachableFL).tag=1,(((
_tmp673.ReachableFL).f1=_tmp1B4,(((_tmp673.ReachableFL).f2=_tmp1B5,(((_tmp673.ReachableFL).f3=
_tmp1B6,_tmp673))))))));}goto _LL15A;_LL15D: if((_tmp1B7.UnknownL).tag != 1)goto
_LL15A;_LL15E: goto _LL15A;_LL15A:;}goto _LL155;_LL158:;_LL159: goto _LL155;_LL155:;}{
struct _tuple5 _tmp674;return(_tmp674.f1=_tmp1B0,((_tmp674.f2=(void*)_tmp137->unknown_all,
_tmp674)));}}}_LL10F: if(*((int*)_tmp13E)!= 4)goto _LL111;_tmp158=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp13E)->f1;_tmp159=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13E)->f2;if(
_tmp159 != 0)goto _LL111;_tmp15A=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13E)->f3;
_LL110: {struct Cyc_Dict_Dict*_tmp1BD=env->all_changed;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1BF;union Cyc_CfFlowInfo_AbsLVal_union _tmp1C0;struct _tuple7 _tmp1BE=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmp158);_tmp1BF=_tmp1BE.f1;_tmp1C0=_tmp1BE.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1C2;void*_tmp1C3;struct _tuple5 _tmp1C1=Cyc_NewControlFlow_anal_Rexp(env,
_tmp1BF,_tmp15A);_tmp1C2=_tmp1C1.f1;_tmp1C3=_tmp1C1.f2;{struct _tuple5 _tmp1C4=Cyc_NewControlFlow_do_assign(
_tmp137,env,_tmp1C2,_tmp158,_tmp1C0,_tmp15A,_tmp1C3,e->loc);struct Cyc_Dict_Dict*
_tmp1C5=env->all_changed;env->all_changed=_tmp1BD;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1C7;void*_tmp1C8;struct _tuple5 _tmp1C6=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp15A);_tmp1C7=_tmp1C6.f1;_tmp1C8=_tmp1C6.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1CA;union Cyc_CfFlowInfo_AbsLVal_union _tmp1CB;struct _tuple7 _tmp1C9=Cyc_NewControlFlow_anal_Lexp(
env,_tmp1C7,_tmp158);_tmp1CA=_tmp1C9.f1;_tmp1CB=_tmp1C9.f2;{struct _tuple5 _tmp1CC=
Cyc_NewControlFlow_do_assign(_tmp137,env,_tmp1CA,_tmp158,_tmp1CB,_tmp15A,_tmp1C8,
e->loc);if(_tmp1BD == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*((struct Cyc_Dict_Dict*)
_check_null(_tmp1C5)),*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);}{
struct _tuple5 _tmp1CD=Cyc_CfFlowInfo_join_flow_and_rval(_tmp137,_tmp1BD,_tmp1C4,
_tmp1CC,1);return _tmp1CD;}}}}}}}_LL111: if(*((int*)_tmp13E)!= 9)goto _LL113;
_tmp15B=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp13E)->f1;_tmp15C=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp13E)->f2;_LL112: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1CF;void*_tmp1D0;
struct _tuple5 _tmp1CE=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp15B);_tmp1CF=
_tmp1CE.f1;_tmp1D0=_tmp1CE.f2;_tmp1CF=Cyc_CfFlowInfo_drop_unique_rvals(_tmp15B->loc,
_tmp1CF);return Cyc_NewControlFlow_anal_Rexp(env,_tmp1CF,_tmp15C);}_LL113: if(*((
int*)_tmp13E)!= 12)goto _LL115;_tmp15D=((struct Cyc_Absyn_Throw_e_struct*)_tmp13E)->f1;
_LL114: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1D2;void*_tmp1D3;struct _tuple5
_tmp1D1=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp15D);_tmp1D2=_tmp1D1.f1;
_tmp1D3=_tmp1D1.f2;_tmp1D2=Cyc_CfFlowInfo_consume_unique_rvals(_tmp15D->loc,
_tmp1D2);Cyc_NewControlFlow_use_Rval(env,_tmp15D->loc,_tmp1D2,_tmp1D3);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp677;struct _tuple5 _tmp676;return(_tmp676.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp677.BottomFL).tag=0,_tmp677)),((_tmp676.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp137,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp137->unknown_all),
_tmp676)));}}_LL115: if(*((int*)_tmp13E)!= 11)goto _LL117;_tmp15E=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp13E)->f1;_tmp15F=((struct Cyc_Absyn_FnCall_e_struct*)_tmp13E)->f2;_LL116: {
struct _RegionHandle _tmp1D6=_new_region("temp");struct _RegionHandle*temp=& _tmp1D6;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1D9;struct Cyc_List_List*
_tmp1DA;struct Cyc_List_List*_tmp678;struct _tuple11 _tmp1D8=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((_tmp678=_region_malloc(temp,sizeof(*_tmp678)),((_tmp678->hd=
_tmp15E,((_tmp678->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rcopy)(temp,_tmp15F),_tmp678)))))),1);_tmp1D9=_tmp1D8.f1;_tmp1DA=
_tmp1D8.f2;_tmp1D9=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp1D9);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1DB=Cyc_NewControlFlow_use_Rval(env,_tmp15E->loc,_tmp1D9,(void*)((struct Cyc_List_List*)
_check_null(_tmp1DA))->hd);_tmp1DA=_tmp1DA->tl;{struct Cyc_List_List*init_params=
0;{void*_tmp1DC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp15E->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1DD;void*_tmp1DE;_LL160: if(
_tmp1DC <= (void*)4)goto _LL162;if(*((int*)_tmp1DC)!= 4)goto _LL162;_tmp1DD=((
struct Cyc_Absyn_PointerType_struct*)_tmp1DC)->f1;_tmp1DE=(void*)_tmp1DD.elt_typ;
_LL161:{void*_tmp1DF=Cyc_Tcutil_compress(_tmp1DE);struct Cyc_Absyn_FnInfo _tmp1E0;
struct Cyc_List_List*_tmp1E1;_LL165: if(_tmp1DF <= (void*)4)goto _LL167;if(*((int*)
_tmp1DF)!= 8)goto _LL167;_tmp1E0=((struct Cyc_Absyn_FnType_struct*)_tmp1DF)->f1;
_tmp1E1=_tmp1E0.attributes;_LL166: for(0;_tmp1E1 != 0;_tmp1E1=_tmp1E1->tl){void*
_tmp1E2=(void*)_tmp1E1->hd;int _tmp1E3;_LL16A: if(_tmp1E2 <= (void*)17)goto _LL16C;
if(*((int*)_tmp1E2)!= 4)goto _LL16C;_tmp1E3=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1E2)->f1;_LL16B:{struct Cyc_List_List*_tmp679;init_params=((_tmp679=
_region_malloc(temp,sizeof(*_tmp679)),((_tmp679->hd=(void*)_tmp1E3,((_tmp679->tl=
init_params,_tmp679))))));}goto _LL169;_LL16C:;_LL16D: goto _LL169;_LL169:;}goto
_LL164;_LL167:;_LL168: {const char*_tmp67C;void*_tmp67B;(_tmp67B=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp67C="anal_Rexp: bad function type",
_tag_dyneither(_tmp67C,sizeof(char),29))),_tag_dyneither(_tmp67B,sizeof(void*),0)));}
_LL164:;}goto _LL15F;_LL162:;_LL163: {const char*_tmp67F;void*_tmp67E;(_tmp67E=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp67F="anal_Rexp: bad function type",_tag_dyneither(_tmp67F,sizeof(char),29))),
_tag_dyneither(_tmp67E,sizeof(void*),0)));}_LL15F:;}{int i=1;for(0;_tmp1DA != 0;(((
_tmp1DA=_tmp1DA->tl,_tmp15F=((struct Cyc_List_List*)_check_null(_tmp15F))->tl)),
++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
_tmp1DB=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp15F))->hd)->loc,_tmp1DB,(void*)_tmp1DA->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1E9=_tmp1D9;struct Cyc_Dict_Dict _tmp1EA;_LL16F: if((_tmp1E9.BottomFL).tag != 0)
goto _LL171;_LL170: goto _LL16E;_LL171: if((_tmp1E9.ReachableFL).tag != 1)goto _LL16E;
_tmp1EA=(_tmp1E9.ReachableFL).f1;_LL172: if(Cyc_CfFlowInfo_initlevel(_tmp1EA,(
void*)_tmp1DA->hd)== (void*)0){const char*_tmp682;void*_tmp681;(_tmp681=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp15F))->hd)->loc,((
_tmp682="expression may not be initialized",_tag_dyneither(_tmp682,sizeof(char),
34))),_tag_dyneither(_tmp681,sizeof(void*),0)));}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1ED=_tmp1DB;struct Cyc_Dict_Dict _tmp1EE;struct Cyc_List_List*_tmp1EF;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1F0;_LL174: if((_tmp1ED.BottomFL).tag != 0)goto _LL176;_LL175: goto _LL173;_LL176:
if((_tmp1ED.ReachableFL).tag != 1)goto _LL173;_tmp1EE=(_tmp1ED.ReachableFL).f1;
_tmp1EF=(_tmp1ED.ReachableFL).f2;_tmp1F0=(_tmp1ED.ReachableFL).f3;_LL177: {
struct Cyc_Dict_Dict _tmp1F1=Cyc_CfFlowInfo_escape_deref(_tmp137,_tmp1EE,env->all_changed,(
void*)_tmp1DA->hd);{void*_tmp1F2=(void*)_tmp1DA->hd;struct Cyc_CfFlowInfo_Place*
_tmp1F3;_LL179: if(_tmp1F2 <= (void*)3)goto _LL17B;if(*((int*)_tmp1F2)!= 2)goto
_LL17B;_tmp1F3=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1F2)->f1;_LL17A:{
void*_tmp1F4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp15F))->hd)->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1F5;void*_tmp1F6;_LL17E: if(_tmp1F4 <= (void*)4)goto
_LL180;if(*((int*)_tmp1F4)!= 4)goto _LL180;_tmp1F5=((struct Cyc_Absyn_PointerType_struct*)
_tmp1F4)->f1;_tmp1F6=(void*)_tmp1F5.elt_typ;_LL17F: _tmp1F1=Cyc_CfFlowInfo_assign_place(
_tmp137,((struct Cyc_Absyn_Exp*)_tmp15F->hd)->loc,_tmp1F1,env->all_changed,
_tmp1F3,Cyc_CfFlowInfo_typ_to_absrval(_tmp137,_tmp1F6,(void*)_tmp137->esc_all));
goto _LL17D;_LL180:;_LL181: {const char*_tmp685;void*_tmp684;(_tmp684=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp685="anal_Rexp:bad type for initialized arg",
_tag_dyneither(_tmp685,sizeof(char),39))),_tag_dyneither(_tmp684,sizeof(void*),0)));}
_LL17D:;}goto _LL178;_LL17B:;_LL17C: goto _LL178;_LL178:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp686;_tmp1DB=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp686.ReachableFL).tag=
1,(((_tmp686.ReachableFL).f1=_tmp1F1,(((_tmp686.ReachableFL).f2=_tmp1EF,(((
_tmp686.ReachableFL).f3=_tmp1F0,_tmp686))))))));}goto _LL173;}_LL173:;}goto _LL16E;
_LL16E:;}}}if(Cyc_Tcutil_is_noreturn((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp15E->topt))->v)){union Cyc_CfFlowInfo_FlowInfo_union _tmp689;struct _tuple5
_tmp688;struct _tuple5 _tmp1FC=(_tmp688.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp689.BottomFL).tag=0,_tmp689)),((_tmp688.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp137,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp137->unknown_all),
_tmp688)));_npop_handler(0);return _tmp1FC;}else{struct _tuple5 _tmp68A;struct
_tuple5 _tmp1FE=(_tmp68A.f1=_tmp1DB,((_tmp68A.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp137,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp137->unknown_all),
_tmp68A)));_npop_handler(0);return _tmp1FE;}}}};_pop_region(temp);}_LL117: if(*((
int*)_tmp13E)!= 35)goto _LL119;_tmp160=((struct Cyc_Absyn_Malloc_e_struct*)_tmp13E)->f1;
_tmp161=_tmp160.is_calloc;_tmp162=_tmp160.rgn;_tmp163=_tmp160.elt_type;_tmp164=
_tmp160.num_elts;_tmp165=_tmp160.fat_result;_LL118: {struct Cyc_CfFlowInfo_MallocPt_struct
_tmp68D;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp68C;void*root=(void*)((_tmp68C=
_region_malloc(_tmp137->r,sizeof(*_tmp68C)),((_tmp68C[0]=((_tmp68D.tag=1,((
_tmp68D.f1=_tmp164,((_tmp68D.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v),_tmp68D)))))),_tmp68C))));struct Cyc_CfFlowInfo_Place*_tmp68E;
struct Cyc_CfFlowInfo_Place*place=(_tmp68E=_region_malloc(_tmp137->r,sizeof(*
_tmp68E)),((_tmp68E->root=(void*)root,((_tmp68E->fields=0,_tmp68E)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp691;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp690;void*rval=(void*)((_tmp690=
_region_malloc(_tmp137->r,sizeof(*_tmp690)),((_tmp690[0]=((_tmp691.tag=2,((
_tmp691.f1=place,_tmp691)))),_tmp690))));void*place_val=_tmp165?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(
_tmp137,*((void**)_check_null(_tmp163)),(void*)_tmp137->unknown_none);union Cyc_CfFlowInfo_FlowInfo_union
outflow;Cyc_CfFlowInfo_update_place_set(env->all_changed,place,0);if(_tmp162 != 0){
struct _RegionHandle _tmp1FF=_new_region("temp");struct _RegionHandle*temp=& _tmp1FF;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp202;struct Cyc_List_List*
_tmp203;struct Cyc_Absyn_Exp*_tmp692[2];struct _tuple11 _tmp201=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((_tmp692[1]=_tmp164,((_tmp692[0]=(struct Cyc_Absyn_Exp*)_tmp162,((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
temp,_tag_dyneither(_tmp692,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);_tmp202=
_tmp201.f1;_tmp203=_tmp201.f2;outflow=_tmp202;};_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo_union
_tmp205;struct _tuple5 _tmp204=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp164);
_tmp205=_tmp204.f1;outflow=_tmp205;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp164->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp206=outflow;struct
Cyc_Dict_Dict _tmp207;struct Cyc_List_List*_tmp208;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp209;_LL183: if((_tmp206.BottomFL).tag != 0)goto _LL185;_LL184: {struct _tuple5
_tmp693;return(_tmp693.f1=outflow,((_tmp693.f2=rval,_tmp693)));}_LL185: if((
_tmp206.ReachableFL).tag != 1)goto _LL182;_tmp207=(_tmp206.ReachableFL).f1;_tmp208=(
_tmp206.ReachableFL).f2;_tmp209=(_tmp206.ReachableFL).f3;_LL186: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp696;struct _tuple5 _tmp695;return(_tmp695.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp696.ReachableFL).tag=1,(((_tmp696.ReachableFL).f1=Cyc_Dict_insert(_tmp207,
root,place_val),(((_tmp696.ReachableFL).f2=_tmp208,(((_tmp696.ReachableFL).f3=
_tmp209,_tmp696)))))))),((_tmp695.f2=rval,_tmp695)));}_LL182:;}}_LL119: if(*((int*)
_tmp13E)!= 36)goto _LL11B;_tmp166=((struct Cyc_Absyn_Swap_e_struct*)_tmp13E)->f1;
_tmp167=((struct Cyc_Absyn_Swap_e_struct*)_tmp13E)->f2;_LL11A: {void*left_rval;
void*right_rval;union Cyc_CfFlowInfo_FlowInfo_union outflow;{struct _RegionHandle
_tmp212=_new_region("temp");struct _RegionHandle*temp=& _tmp212;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp215;struct Cyc_List_List*_tmp216;struct Cyc_Absyn_Exp*
_tmp697[2];struct _tuple11 _tmp214=Cyc_NewControlFlow_anal_unordered_Rexps(temp,
env,inflow,((_tmp697[1]=_tmp167,((_tmp697[0]=_tmp166,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(
_tmp697,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);_tmp215=_tmp214.f1;_tmp216=
_tmp214.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp216))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp216->tl))->hd;outflow=
_tmp215;};_pop_region(temp);}Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
outflow);{union Cyc_CfFlowInfo_AbsLVal_union _tmp218;struct _tuple7 _tmp217=Cyc_NewControlFlow_anal_Lexp(
env,outflow,_tmp166);_tmp218=_tmp217.f2;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp21A;struct _tuple7 _tmp219=Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmp167);
_tmp21A=_tmp219.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp21B=outflow;struct Cyc_Dict_Dict
_tmp21C;struct Cyc_List_List*_tmp21D;struct Cyc_CfFlowInfo_ConsumeInfo _tmp21E;
_LL188: if((_tmp21B.ReachableFL).tag != 1)goto _LL18A;_tmp21C=(_tmp21B.ReachableFL).f1;
_tmp21D=(_tmp21B.ReachableFL).f2;_tmp21E=(_tmp21B.ReachableFL).f3;_LL189:{union
Cyc_CfFlowInfo_AbsLVal_union _tmp21F=_tmp218;struct Cyc_CfFlowInfo_Place*_tmp220;
_LL18D: if((_tmp21F.PlaceL).tag != 0)goto _LL18F;_tmp220=(_tmp21F.PlaceL).f1;_LL18E:
_tmp21C=Cyc_CfFlowInfo_assign_place(_tmp137,_tmp166->loc,_tmp21C,env->all_changed,
_tmp220,right_rval);goto _LL18C;_LL18F: if((_tmp21F.UnknownL).tag != 1)goto _LL18C;
_LL190: goto _LL18C;_LL18C:;}{union Cyc_CfFlowInfo_AbsLVal_union _tmp221=_tmp21A;
struct Cyc_CfFlowInfo_Place*_tmp222;_LL192: if((_tmp221.PlaceL).tag != 0)goto _LL194;
_tmp222=(_tmp221.PlaceL).f1;_LL193: _tmp21C=Cyc_CfFlowInfo_assign_place(_tmp137,
_tmp167->loc,_tmp21C,env->all_changed,_tmp222,left_rval);goto _LL191;_LL194: if((
_tmp221.UnknownL).tag != 1)goto _LL191;_LL195: goto _LL191;_LL191:;}_tmp21D=Cyc_CfFlowInfo_reln_kill_exp(
_tmp137->r,_tmp21D,_tmp166);_tmp21D=Cyc_CfFlowInfo_reln_kill_exp(_tmp137->r,
_tmp21D,_tmp167);{union Cyc_CfFlowInfo_FlowInfo_union _tmp698;outflow=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp698.ReachableFL).tag=1,(((_tmp698.ReachableFL).f1=_tmp21C,(((_tmp698.ReachableFL).f2=
_tmp21D,(((_tmp698.ReachableFL).f3=_tmp21E,_tmp698))))))));}goto _LL187;_LL18A:;
_LL18B: goto _LL187;_LL187:;}{struct _tuple5 _tmp699;return(_tmp699.f1=outflow,((
_tmp699.f2=(void*)_tmp137->unknown_all,_tmp699)));}}}}_LL11B: if(*((int*)_tmp13E)
!= 17)goto _LL11D;_tmp168=((struct Cyc_Absyn_New_e_struct*)_tmp13E)->f1;_tmp169=((
struct Cyc_Absyn_New_e_struct*)_tmp13E)->f2;_LL11C: {struct Cyc_CfFlowInfo_MallocPt_struct
_tmp69C;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp69B;void*root=(void*)((_tmp69B=
_region_malloc(_tmp137->r,sizeof(*_tmp69B)),((_tmp69B[0]=((_tmp69C.tag=1,((
_tmp69C.f1=_tmp169,((_tmp69C.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v),_tmp69C)))))),_tmp69B))));struct Cyc_CfFlowInfo_Place*_tmp69D;
struct Cyc_CfFlowInfo_Place*place=(_tmp69D=_region_malloc(_tmp137->r,sizeof(*
_tmp69D)),((_tmp69D->root=(void*)root,((_tmp69D->fields=0,_tmp69D)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp6A0;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp69F;void*rval=(void*)((_tmp69F=
_region_malloc(_tmp137->r,sizeof(*_tmp69F)),((_tmp69F[0]=((_tmp6A0.tag=2,((
_tmp6A0.f1=place,_tmp6A0)))),_tmp69F))));Cyc_CfFlowInfo_update_place_set(env->all_changed,
place,0);{union Cyc_CfFlowInfo_FlowInfo_union outflow;void*place_val;if(_tmp168 != 
0){struct _RegionHandle _tmp225=_new_region("temp");struct _RegionHandle*temp=&
_tmp225;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp228;struct Cyc_List_List*
_tmp229;struct Cyc_Absyn_Exp*_tmp6A1[2];struct _tuple11 _tmp227=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((_tmp6A1[1]=_tmp169,((_tmp6A1[0]=(struct Cyc_Absyn_Exp*)_tmp168,((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
temp,_tag_dyneither(_tmp6A1,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);_tmp228=
_tmp227.f1;_tmp229=_tmp227.f2;outflow=_tmp228;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp229))->tl))->hd;};_pop_region(
temp);}else{union Cyc_CfFlowInfo_FlowInfo_union _tmp22B;void*_tmp22C;struct _tuple5
_tmp22A=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp169);_tmp22B=_tmp22A.f1;
_tmp22C=_tmp22A.f2;outflow=_tmp22B;place_val=_tmp22C;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp169->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp22D=outflow;struct
Cyc_Dict_Dict _tmp22E;struct Cyc_List_List*_tmp22F;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp230;_LL197: if((_tmp22D.BottomFL).tag != 0)goto _LL199;_LL198: {struct _tuple5
_tmp6A2;return(_tmp6A2.f1=outflow,((_tmp6A2.f2=rval,_tmp6A2)));}_LL199: if((
_tmp22D.ReachableFL).tag != 1)goto _LL196;_tmp22E=(_tmp22D.ReachableFL).f1;_tmp22F=(
_tmp22D.ReachableFL).f2;_tmp230=(_tmp22D.ReachableFL).f3;_LL19A: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp6A5;struct _tuple5 _tmp6A4;return(_tmp6A4.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp6A5.ReachableFL).tag=1,(((_tmp6A5.ReachableFL).f1=Cyc_Dict_insert(_tmp22E,
root,place_val),(((_tmp6A5.ReachableFL).f2=_tmp22F,(((_tmp6A5.ReachableFL).f3=
_tmp230,_tmp6A5)))))))),((_tmp6A4.f2=rval,_tmp6A4)));}_LL196:;}}}_LL11D: if(*((
int*)_tmp13E)!= 16)goto _LL11F;_tmp16A=((struct Cyc_Absyn_Address_e_struct*)
_tmp13E)->f1;_LL11E: {union Cyc_CfFlowInfo_FlowInfo_union _tmp23A;struct _tuple5
_tmp239=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp16A);_tmp23A=_tmp239.f1;{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp23C;struct _tuple6 _tmp23B=Cyc_CfFlowInfo_save_consume_info(
env->fenv,_tmp23A,0);_tmp23C=_tmp23B.f1;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp23E;union Cyc_CfFlowInfo_AbsLVal_union _tmp23F;struct _tuple7 _tmp23D=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmp16A);_tmp23E=_tmp23D.f1;_tmp23F=_tmp23D.f2;_tmp23E=Cyc_CfFlowInfo_restore_consume_info(
_tmp23E,_tmp23C);{union Cyc_CfFlowInfo_AbsLVal_union _tmp240=_tmp23F;struct Cyc_CfFlowInfo_Place*
_tmp241;_LL19C: if((_tmp240.UnknownL).tag != 1)goto _LL19E;_LL19D: {struct _tuple5
_tmp6A6;return(_tmp6A6.f1=_tmp23E,((_tmp6A6.f2=(void*)1,_tmp6A6)));}_LL19E: if((
_tmp240.PlaceL).tag != 0)goto _LL19B;_tmp241=(_tmp240.PlaceL).f1;_LL19F: {struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp6AC;struct Cyc_CfFlowInfo_AddressOf_struct
_tmp6AB;struct _tuple5 _tmp6AA;return(_tmp6AA.f1=_tmp23E,((_tmp6AA.f2=(void*)((
_tmp6AC=_region_malloc(env->r,sizeof(*_tmp6AC)),((_tmp6AC[0]=((_tmp6AB.tag=2,((
_tmp6AB.f1=_tmp241,_tmp6AB)))),_tmp6AC)))),_tmp6AA)));}_LL19B:;}}}}_LL11F: if(*((
int*)_tmp13E)!= 22)goto _LL121;_tmp16B=((struct Cyc_Absyn_Deref_e_struct*)_tmp13E)->f1;
_LL120: {union Cyc_CfFlowInfo_FlowInfo_union _tmp247;void*_tmp248;struct _tuple5
_tmp246=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp16B);_tmp247=_tmp246.f1;
_tmp248=_tmp246.f2;_tmp247=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp247);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp247,_tmp16B,_tmp248);}
_LL121: if(*((int*)_tmp13E)!= 23)goto _LL123;_tmp16C=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp13E)->f1;_tmp16D=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp13E)->f2;_LL122: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp24A;void*_tmp24B;struct _tuple5 _tmp249=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp16C);_tmp24A=_tmp249.f1;_tmp24B=_tmp249.f2;_tmp24A=Cyc_CfFlowInfo_drop_unique_rvals(
e->loc,_tmp24A);_tmp24A=Cyc_NewControlFlow_may_consume_exp(env,_tmp24A,e);if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp16C->topt))->v)){struct _tuple5
_tmp6AD;return(_tmp6AD.f1=_tmp24A,((_tmp6AD.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp137,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp137->unknown_all),
_tmp6AD)));}{void*_tmp24D=_tmp24B;struct _dyneither_ptr _tmp24E;_LL1A1: if(_tmp24D
<= (void*)3)goto _LL1A3;if(*((int*)_tmp24D)!= 4)goto _LL1A3;_tmp24E=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp24D)->f1;_LL1A2: {int _tmp24F=Cyc_NewControlFlow_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp16C->topt))->v,_tmp16D);struct _tuple5 _tmp6AE;
return(_tmp6AE.f1=_tmp24A,((_tmp6AE.f2=*((void**)_check_dyneither_subscript(
_tmp24E,sizeof(void*),_tmp24F)),_tmp6AE)));}_LL1A3:;_LL1A4: {struct Cyc_Core_Impossible_struct
_tmp6BB;const char*_tmp6BA;void*_tmp6B9[1];struct Cyc_String_pa_struct _tmp6B8;
struct Cyc_Core_Impossible_struct*_tmp6B7;(int)_throw((void*)((_tmp6B7=_cycalloc(
sizeof(*_tmp6B7)),((_tmp6B7[0]=((_tmp6BB.tag=Cyc_Core_Impossible,((_tmp6BB.f1=(
struct _dyneither_ptr)((_tmp6B8.tag=0,((_tmp6B8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp6B9[0]=& _tmp6B8,Cyc_aprintf(((
_tmp6BA="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp6BA,sizeof(char),26))),
_tag_dyneither(_tmp6B9,sizeof(void*),1)))))))),_tmp6BB)))),_tmp6B7)))));}_LL1A0:;}}
_LL123: if(*((int*)_tmp13E)!= 24)goto _LL125;_tmp16E=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp13E)->f1;_tmp16F=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp13E)->f2;_LL124: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp257;void*_tmp258;struct _tuple5 _tmp256=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp16E);_tmp257=_tmp256.f1;_tmp258=_tmp256.f2;_tmp257=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp257);{union Cyc_CfFlowInfo_FlowInfo_union _tmp25A;void*_tmp25B;struct
_tuple5 _tmp259=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp257,_tmp16E,_tmp258);
_tmp25A=_tmp259.f1;_tmp25B=_tmp259.f2;{void*_tmp25C=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp16E->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp25D;void*_tmp25E;_LL1A6: if(_tmp25C <= (void*)4)goto _LL1A8;if(*((int*)_tmp25C)
!= 4)goto _LL1A8;_tmp25D=((struct Cyc_Absyn_PointerType_struct*)_tmp25C)->f1;
_tmp25E=(void*)_tmp25D.elt_typ;_LL1A7: if(Cyc_Absyn_is_union_type(_tmp25E)){
struct _tuple5 _tmp6BC;return(_tmp6BC.f1=_tmp25A,((_tmp6BC.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp137,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp137->unknown_all),
_tmp6BC)));}{void*_tmp260=_tmp25B;struct _dyneither_ptr _tmp261;_LL1AB: if(_tmp260
<= (void*)3)goto _LL1AD;if(*((int*)_tmp260)!= 4)goto _LL1AD;_tmp261=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp260)->f1;_LL1AC: {int _tmp262=Cyc_NewControlFlow_get_field_index(_tmp25E,
_tmp16F);struct _tuple5 _tmp6BD;return(_tmp6BD.f1=_tmp25A,((_tmp6BD.f2=*((void**)
_check_dyneither_subscript(_tmp261,sizeof(void*),_tmp262)),_tmp6BD)));}_LL1AD:;
_LL1AE: {struct Cyc_Core_Impossible_struct _tmp6C3;const char*_tmp6C2;struct Cyc_Core_Impossible_struct*
_tmp6C1;(int)_throw((void*)((_tmp6C1=_cycalloc(sizeof(*_tmp6C1)),((_tmp6C1[0]=((
_tmp6C3.tag=Cyc_Core_Impossible,((_tmp6C3.f1=((_tmp6C2="anal_Rexp: AggrArrow",
_tag_dyneither(_tmp6C2,sizeof(char),21))),_tmp6C3)))),_tmp6C1)))));}_LL1AA:;}
_LL1A8:;_LL1A9: {struct Cyc_Core_Impossible_struct _tmp6C9;const char*_tmp6C8;
struct Cyc_Core_Impossible_struct*_tmp6C7;(int)_throw((void*)((_tmp6C7=_cycalloc(
sizeof(*_tmp6C7)),((_tmp6C7[0]=((_tmp6C9.tag=Cyc_Core_Impossible,((_tmp6C9.f1=((
_tmp6C8="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp6C8,sizeof(char),25))),
_tmp6C9)))),_tmp6C7)))));}_LL1A5:;}}}_LL125: if(*((int*)_tmp13E)!= 6)goto _LL127;
_tmp170=((struct Cyc_Absyn_Conditional_e_struct*)_tmp13E)->f1;_tmp171=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp13E)->f2;_tmp172=((struct Cyc_Absyn_Conditional_e_struct*)_tmp13E)->f3;_LL126: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp26B;union Cyc_CfFlowInfo_FlowInfo_union
_tmp26C;struct _tuple8 _tmp26A=Cyc_NewControlFlow_anal_test(env,inflow,_tmp170);
_tmp26B=_tmp26A.f1;_tmp26C=_tmp26A.f2;_tmp26B=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp170->loc,_tmp26B);_tmp26C=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp170->loc,
_tmp26C);{struct _tuple5 _tmp26D=Cyc_NewControlFlow_anal_Rexp(env,_tmp26B,_tmp171);
struct _tuple5 _tmp26E=Cyc_NewControlFlow_anal_Rexp(env,_tmp26C,_tmp172);return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp137,env->all_changed,_tmp26D,_tmp26E,1);}}_LL127: if(*((int*)_tmp13E)!= 7)
goto _LL129;_tmp173=((struct Cyc_Absyn_And_e_struct*)_tmp13E)->f1;_tmp174=((struct
Cyc_Absyn_And_e_struct*)_tmp13E)->f2;_LL128: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp270;union Cyc_CfFlowInfo_FlowInfo_union _tmp271;struct _tuple8 _tmp26F=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp173);_tmp270=_tmp26F.f1;_tmp271=_tmp26F.f2;_tmp270=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp173->loc,_tmp270);_tmp271=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp173->loc,
_tmp271);{union Cyc_CfFlowInfo_FlowInfo_union _tmp273;void*_tmp274;struct _tuple5
_tmp272=Cyc_NewControlFlow_anal_Rexp(env,_tmp270,_tmp174);_tmp273=_tmp272.f1;
_tmp274=_tmp272.f2;_tmp273=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp174->loc,
_tmp273);{struct _tuple5 _tmp6CA;struct _tuple5 _tmp275=(_tmp6CA.f1=_tmp273,((
_tmp6CA.f2=_tmp274,_tmp6CA)));struct _tuple5 _tmp6CB;struct _tuple5 _tmp276=(_tmp6CB.f1=
_tmp271,((_tmp6CB.f2=(void*)((void*)0),_tmp6CB)));return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp137,env->all_changed,_tmp275,_tmp276,0);}}}_LL129: if(*((int*)_tmp13E)!= 8)
goto _LL12B;_tmp175=((struct Cyc_Absyn_Or_e_struct*)_tmp13E)->f1;_tmp176=((struct
Cyc_Absyn_Or_e_struct*)_tmp13E)->f2;_LL12A: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp27A;union Cyc_CfFlowInfo_FlowInfo_union _tmp27B;struct _tuple8 _tmp279=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp175);_tmp27A=_tmp279.f1;_tmp27B=_tmp279.f2;_tmp27A=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp175->loc,_tmp27A);_tmp27B=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp175->loc,
_tmp27B);{union Cyc_CfFlowInfo_FlowInfo_union _tmp27D;void*_tmp27E;struct _tuple5
_tmp27C=Cyc_NewControlFlow_anal_Rexp(env,_tmp27B,_tmp176);_tmp27D=_tmp27C.f1;
_tmp27E=_tmp27C.f2;_tmp27D=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp176->loc,
_tmp27D);{struct _tuple5 _tmp6CC;struct _tuple5 _tmp27F=(_tmp6CC.f1=_tmp27D,((
_tmp6CC.f2=_tmp27E,_tmp6CC)));struct _tuple5 _tmp6CD;struct _tuple5 _tmp280=(_tmp6CD.f1=
_tmp27A,((_tmp6CD.f2=(void*)((void*)1),_tmp6CD)));return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp137,env->all_changed,_tmp27F,_tmp280,0);}}}_LL12B: if(*((int*)_tmp13E)!= 25)
goto _LL12D;_tmp177=((struct Cyc_Absyn_Subscript_e_struct*)_tmp13E)->f1;_tmp178=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp13E)->f2;_LL12C: {struct _RegionHandle
_tmp283=_new_region("temp");struct _RegionHandle*temp=& _tmp283;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp286;struct Cyc_List_List*_tmp287;struct Cyc_Absyn_Exp*
_tmp6CE[2];struct _tuple11 _tmp285=Cyc_NewControlFlow_anal_unordered_Rexps(temp,
env,inflow,((_tmp6CE[1]=_tmp178,((_tmp6CE[0]=_tmp177,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(
_tmp6CE,sizeof(struct Cyc_Absyn_Exp*),2)))))),1);_tmp286=_tmp285.f1;_tmp287=
_tmp285.f2;_tmp286=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp178->loc,_tmp286);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp288=_tmp286;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp289=_tmp286;struct Cyc_Dict_Dict _tmp28A;struct Cyc_List_List*_tmp28B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp28C;_LL1B0: if((_tmp289.ReachableFL).tag != 1)goto _LL1B2;_tmp28A=(_tmp289.ReachableFL).f1;
_tmp28B=(_tmp289.ReachableFL).f2;_tmp28C=(_tmp289.ReachableFL).f3;_LL1B1: if(Cyc_CfFlowInfo_initlevel(
_tmp28A,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp287))->tl))->hd)== (void*)0){const char*_tmp6D1;void*_tmp6D0;(
_tmp6D0=0,Cyc_Tcutil_terr(_tmp178->loc,((_tmp6D1="expression may not be initialized",
_tag_dyneither(_tmp6D1,sizeof(char),34))),_tag_dyneither(_tmp6D0,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp28F=Cyc_NewControlFlow_add_subscript_reln(_tmp137->r,
_tmp28B,_tmp177,_tmp178);if(_tmp28B != _tmp28F){union Cyc_CfFlowInfo_FlowInfo_union
_tmp6D2;_tmp288=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp6D2.ReachableFL).tag=
1,(((_tmp6D2.ReachableFL).f1=_tmp28A,(((_tmp6D2.ReachableFL).f2=_tmp28F,(((
_tmp6D2.ReachableFL).f3=_tmp28C,_tmp6D2))))))));}goto _LL1AF;}_LL1B2:;_LL1B3: goto
_LL1AF;_LL1AF:;}{void*_tmp291=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp177->topt))->v);struct Cyc_List_List*_tmp292;struct Cyc_Absyn_PtrInfo
_tmp293;struct Cyc_Absyn_PtrAtts _tmp294;struct Cyc_Absyn_Conref*_tmp295;_LL1B5: if(
_tmp291 <= (void*)4)goto _LL1B9;if(*((int*)_tmp291)!= 9)goto _LL1B7;_tmp292=((
struct Cyc_Absyn_TupleType_struct*)_tmp291)->f1;_LL1B6: {void*_tmp296=(void*)((
struct Cyc_List_List*)_check_null(_tmp287))->hd;struct _dyneither_ptr _tmp297;
_LL1BC: if(_tmp296 <= (void*)3)goto _LL1BE;if(*((int*)_tmp296)!= 4)goto _LL1BE;
_tmp297=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp296)->f1;_LL1BD: _tmp288=Cyc_NewControlFlow_may_consume_exp(
env,_tmp288,e);{unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp178)).f1;struct
_tuple5 _tmp6D3;struct _tuple5 _tmp299=(_tmp6D3.f1=_tmp288,((_tmp6D3.f2=*((void**)
_check_dyneither_subscript(_tmp297,sizeof(void*),(int)i)),_tmp6D3)));
_npop_handler(0);return _tmp299;}_LL1BE:;_LL1BF: {struct Cyc_Core_Impossible_struct
_tmp6D9;const char*_tmp6D8;struct Cyc_Core_Impossible_struct*_tmp6D7;(int)_throw((
void*)((_tmp6D7=_cycalloc(sizeof(*_tmp6D7)),((_tmp6D7[0]=((_tmp6D9.tag=Cyc_Core_Impossible,((
_tmp6D9.f1=((_tmp6D8="anal_Rexp: Subscript",_tag_dyneither(_tmp6D8,sizeof(char),
21))),_tmp6D9)))),_tmp6D7)))));}_LL1BB:;}_LL1B7: if(*((int*)_tmp291)!= 4)goto
_LL1B9;_tmp293=((struct Cyc_Absyn_PointerType_struct*)_tmp291)->f1;_tmp294=
_tmp293.ptr_atts;_tmp295=_tmp294.bounds;_LL1B8: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp29E;void*_tmp29F;struct _tuple5 _tmp29D=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp286,_tmp177,(void*)((struct Cyc_List_List*)_check_null(_tmp287))->hd);
_tmp29E=_tmp29D.f1;_tmp29F=_tmp29D.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp2A0=
_tmp29E;_LL1C1: if((_tmp2A0.BottomFL).tag != 0)goto _LL1C3;_LL1C2: {struct _tuple5
_tmp6DA;struct _tuple5 _tmp2A2=(_tmp6DA.f1=_tmp29E,((_tmp6DA.f2=_tmp29F,_tmp6DA)));
_npop_handler(0);return _tmp2A2;}_LL1C3:;_LL1C4: {struct _tuple5 _tmp6DB;struct
_tuple5 _tmp2A4=(_tmp6DB.f1=_tmp288,((_tmp6DB.f2=_tmp29F,_tmp6DB)));_npop_handler(
0);return _tmp2A4;}_LL1C0:;}}_LL1B9:;_LL1BA: {struct Cyc_Core_Impossible_struct
_tmp6E1;const char*_tmp6E0;struct Cyc_Core_Impossible_struct*_tmp6DF;(int)_throw((
void*)((_tmp6DF=_cycalloc(sizeof(*_tmp6DF)),((_tmp6DF[0]=((_tmp6E1.tag=Cyc_Core_Impossible,((
_tmp6E1.f1=((_tmp6E0="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp6E0,
sizeof(char),33))),_tmp6E1)))),_tmp6DF)))));}_LL1B4:;}}};_pop_region(temp);}
_LL12D: if(*((int*)_tmp13E)!= 32)goto _LL12F;_tmp179=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp13E)->f1;_tmp17A=((struct Cyc_Absyn_Tunion_e_struct*)_tmp13E)->f2;_LL12E: if(
_tmp17A->is_flat){struct _RegionHandle _tmp2A8=_new_region("temp");struct
_RegionHandle*temp=& _tmp2A8;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2AA;struct Cyc_List_List*_tmp2AB;struct _tuple11 _tmp2A9=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp179,0);_tmp2AA=_tmp2A9.f1;_tmp2AB=_tmp2A9.f2;_tmp2AA=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2AA);for(0;(unsigned int)_tmp179;(_tmp179=_tmp179->tl,_tmp2AB=_tmp2AB->tl)){
_tmp2AA=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)_tmp179->hd)->loc,
_tmp2AA,(void*)((struct Cyc_List_List*)_check_null(_tmp2AB))->hd);}{struct _tuple5
_tmp6E2;struct _tuple5 _tmp2AD=(_tmp6E2.f1=_tmp2AA,((_tmp6E2.f2=(void*)_tmp137->unknown_all,
_tmp6E2)));_npop_handler(0);return _tmp2AD;}};_pop_region(temp);}_tmp17B=_tmp179;
goto _LL130;_LL12F: if(*((int*)_tmp13E)!= 26)goto _LL131;_tmp17B=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp13E)->f1;_LL130: {struct _RegionHandle _tmp2AE=_new_region("temp");struct
_RegionHandle*temp=& _tmp2AE;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2B0;struct Cyc_List_List*_tmp2B1;struct _tuple11 _tmp2AF=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp17B,0);_tmp2B0=_tmp2AF.f1;_tmp2B1=_tmp2AF.f2;_tmp2B0=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2B0);{unsigned int _tmp6E8;unsigned int _tmp6E7;struct _dyneither_ptr
_tmp6E6;void**_tmp6E5;unsigned int _tmp6E4;struct _dyneither_ptr aggrdict=(_tmp6E4=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp17B),((_tmp6E5=(
void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp6E4)),((_tmp6E6=
_tag_dyneither(_tmp6E5,sizeof(void*),_tmp6E4),((((_tmp6E7=_tmp6E4,_tmp6E9(&
_tmp6E8,& _tmp6E7,& _tmp6E5,& _tmp2B1))),_tmp6E6)))))));struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp6EF;struct Cyc_CfFlowInfo_Aggregate_struct _tmp6EE;struct _tuple5 _tmp6ED;struct
_tuple5 _tmp2B5=(_tmp6ED.f1=_tmp2B0,((_tmp6ED.f2=(void*)((_tmp6EF=_region_malloc(
env->r,sizeof(*_tmp6EF)),((_tmp6EF[0]=((_tmp6EE.tag=4,((_tmp6EE.f1=aggrdict,
_tmp6EE)))),_tmp6EF)))),_tmp6ED)));_npop_handler(0);return _tmp2B5;}};_pop_region(
temp);}_LL131: if(*((int*)_tmp13E)!= 31)goto _LL133;_tmp17C=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp13E)->f2;_LL132: {struct Cyc_List_List*fs;{void*_tmp2BB=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_List_List*
_tmp2BC;_LL1C6: if(_tmp2BB <= (void*)4)goto _LL1C8;if(*((int*)_tmp2BB)!= 11)goto
_LL1C8;_tmp2BC=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2BB)->f2;_LL1C7: fs=
_tmp2BC;goto _LL1C5;_LL1C8:;_LL1C9: {struct Cyc_Core_Impossible_struct _tmp6F5;
const char*_tmp6F4;struct Cyc_Core_Impossible_struct*_tmp6F3;(int)_throw((void*)((
_tmp6F3=_cycalloc(sizeof(*_tmp6F3)),((_tmp6F3[0]=((_tmp6F5.tag=Cyc_Core_Impossible,((
_tmp6F5.f1=((_tmp6F4="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp6F4,
sizeof(char),35))),_tmp6F5)))),_tmp6F3)))));}_LL1C5:;}_tmp17D=_tmp17C;_tmp182=fs;
goto _LL134;}_LL133: if(*((int*)_tmp13E)!= 30)goto _LL135;_tmp17D=((struct Cyc_Absyn_Struct_e_struct*)
_tmp13E)->f3;_tmp17E=((struct Cyc_Absyn_Struct_e_struct*)_tmp13E)->f4;if(_tmp17E
== 0)goto _LL135;_tmp17F=*_tmp17E;_tmp180=_tmp17F.impl;if(_tmp180 == 0)goto _LL135;
_tmp181=*_tmp180;_tmp182=_tmp181.fields;_LL134: {struct _RegionHandle _tmp2C0=
_new_region("temp");struct _RegionHandle*temp=& _tmp2C0;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp2C2;struct Cyc_List_List*_tmp2C3;struct _tuple11
_tmp2C1=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple14*),struct Cyc_List_List*
x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple14*))Cyc_Core_snd,
_tmp17D),0);_tmp2C2=_tmp2C1.f1;_tmp2C3=_tmp2C1.f2;_tmp2C2=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2C2);{struct _dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(
_tmp137,_tmp182,(void*)_tmp137->unknown_all);{int i=0;for(0;_tmp2C3 != 0;(((
_tmp2C3=_tmp2C3->tl,_tmp17D=_tmp17D->tl)),++ i)){struct Cyc_List_List*ds=(*((
struct _tuple14*)((struct Cyc_List_List*)_check_null(_tmp17D))->hd)).f1;for(0;ds != 
0;ds=ds->tl){void*_tmp2C4=(void*)ds->hd;struct _dyneither_ptr*_tmp2C5;_LL1CB: if(*((
int*)_tmp2C4)!= 0)goto _LL1CD;_LL1CC: {struct Cyc_Core_Impossible_struct _tmp6FB;
const char*_tmp6FA;struct Cyc_Core_Impossible_struct*_tmp6F9;(int)_throw((void*)((
_tmp6F9=_cycalloc(sizeof(*_tmp6F9)),((_tmp6F9[0]=((_tmp6FB.tag=Cyc_Core_Impossible,((
_tmp6FB.f1=((_tmp6FA="anal_Rexp:Struct_e",_tag_dyneither(_tmp6FA,sizeof(char),19))),
_tmp6FB)))),_tmp6F9)))));}_LL1CD: if(*((int*)_tmp2C4)!= 1)goto _LL1CA;_tmp2C5=((
struct Cyc_Absyn_FieldName_struct*)_tmp2C4)->f1;_LL1CE: {int _tmp2C9=Cyc_NewControlFlow_get_field_index_fs(
_tmp182,_tmp2C5);*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),
_tmp2C9))=(void*)_tmp2C3->hd;}_LL1CA:;}}}{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp701;struct Cyc_CfFlowInfo_Aggregate_struct _tmp700;struct _tuple5 _tmp6FF;struct
_tuple5 _tmp2CD=(_tmp6FF.f1=_tmp2C2,((_tmp6FF.f2=(void*)((_tmp701=_region_malloc(
env->r,sizeof(*_tmp701)),((_tmp701[0]=((_tmp700.tag=4,((_tmp700.f1=aggrdict,
_tmp700)))),_tmp701)))),_tmp6FF)));_npop_handler(0);return _tmp2CD;}}};
_pop_region(temp);}_LL135: if(*((int*)_tmp13E)!= 30)goto _LL137;_LL136: {struct Cyc_Core_Impossible_struct
_tmp707;const char*_tmp706;struct Cyc_Core_Impossible_struct*_tmp705;(int)_throw((
void*)((_tmp705=_cycalloc(sizeof(*_tmp705)),((_tmp705[0]=((_tmp707.tag=Cyc_Core_Impossible,((
_tmp707.f1=((_tmp706="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp706,
sizeof(char),31))),_tmp707)))),_tmp705)))));}_LL137: if(*((int*)_tmp13E)!= 28)
goto _LL139;_tmp183=((struct Cyc_Absyn_Array_e_struct*)_tmp13E)->f1;_LL138: {
struct _RegionHandle _tmp2D1=_new_region("temp");struct _RegionHandle*temp=& _tmp2D1;
_push_region(temp);{struct Cyc_List_List*_tmp2D2=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple14*),struct Cyc_List_List*x))
Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple14*))Cyc_Core_snd,
_tmp183);union Cyc_CfFlowInfo_FlowInfo_union _tmp2D4;struct Cyc_List_List*_tmp2D5;
struct _tuple11 _tmp2D3=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
_tmp2D2,0);_tmp2D4=_tmp2D3.f1;_tmp2D5=_tmp2D3.f2;_tmp2D4=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2D4);for(0;_tmp2D5 != 0;(_tmp2D5=_tmp2D5->tl,_tmp2D2=_tmp2D2->tl)){
_tmp2D4=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2D2))->hd)->loc,_tmp2D4,(void*)_tmp2D5->hd);}{struct _tuple5
_tmp708;struct _tuple5 _tmp2D7=(_tmp708.f1=_tmp2D4,((_tmp708.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp137,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp137->unknown_all),
_tmp708)));_npop_handler(0);return _tmp2D7;}};_pop_region(temp);}_LL139: if(*((int*)
_tmp13E)!= 29)goto _LL13B;_tmp184=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp13E)->f1;_tmp185=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp13E)->f2;
_tmp186=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp13E)->f3;_tmp187=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp13E)->f4;_LL13A: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp2D9;void*_tmp2DA;struct _tuple5 _tmp2D8=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp185);_tmp2D9=_tmp2D8.f1;_tmp2DA=_tmp2D8.f2;_tmp2D9=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp185->loc,_tmp2D9);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2DB=_tmp2D9;struct
Cyc_Dict_Dict _tmp2DC;struct Cyc_List_List*_tmp2DD;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2DE;_LL1D0: if((_tmp2DB.BottomFL).tag != 0)goto _LL1D2;_LL1D1: {struct _tuple5
_tmp709;return(_tmp709.f1=_tmp2D9,((_tmp709.f2=(void*)_tmp137->unknown_all,
_tmp709)));}_LL1D2: if((_tmp2DB.ReachableFL).tag != 1)goto _LL1CF;_tmp2DC=(_tmp2DB.ReachableFL).f1;
_tmp2DD=(_tmp2DB.ReachableFL).f2;_tmp2DE=(_tmp2DB.ReachableFL).f3;_LL1D3: if(Cyc_CfFlowInfo_initlevel(
_tmp2DC,_tmp2DA)== (void*)0){const char*_tmp70C;void*_tmp70B;(_tmp70B=0,Cyc_Tcutil_terr(
_tmp185->loc,((_tmp70C="expression may not be initialized",_tag_dyneither(
_tmp70C,sizeof(char),34))),_tag_dyneither(_tmp70B,sizeof(void*),0)));}{struct Cyc_List_List*
new_relns=_tmp2DD;comp_loop: {void*_tmp2E2=(void*)_tmp185->r;struct Cyc_Absyn_Exp*
_tmp2E3;void*_tmp2E4;struct Cyc_Absyn_Vardecl*_tmp2E5;void*_tmp2E6;struct Cyc_Absyn_Vardecl*
_tmp2E7;void*_tmp2E8;struct Cyc_Absyn_Vardecl*_tmp2E9;void*_tmp2EA;struct Cyc_Absyn_Vardecl*
_tmp2EB;union Cyc_Absyn_Cnst_union _tmp2EC;int _tmp2ED;void*_tmp2EE;struct Cyc_List_List*
_tmp2EF;struct Cyc_List_List _tmp2F0;struct Cyc_Absyn_Exp*_tmp2F1;_LL1D5: if(*((int*)
_tmp2E2)!= 15)goto _LL1D7;_tmp2E3=((struct Cyc_Absyn_Cast_e_struct*)_tmp2E2)->f2;
_LL1D6: _tmp185=_tmp2E3;goto comp_loop;_LL1D7: if(*((int*)_tmp2E2)!= 1)goto _LL1D9;
_tmp2E4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E2)->f2;if(_tmp2E4 <= (void*)
1)goto _LL1D9;if(*((int*)_tmp2E4)!= 0)goto _LL1D9;_tmp2E5=((struct Cyc_Absyn_Global_b_struct*)
_tmp2E4)->f1;if(!(!_tmp2E5->escapes))goto _LL1D9;_LL1D8: _tmp2E7=_tmp2E5;goto
_LL1DA;_LL1D9: if(*((int*)_tmp2E2)!= 1)goto _LL1DB;_tmp2E6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E2)->f2;if(_tmp2E6 <= (void*)1)goto _LL1DB;if(*((int*)_tmp2E6)!= 3)goto _LL1DB;
_tmp2E7=((struct Cyc_Absyn_Local_b_struct*)_tmp2E6)->f1;if(!(!_tmp2E7->escapes))
goto _LL1DB;_LL1DA: _tmp2E9=_tmp2E7;goto _LL1DC;_LL1DB: if(*((int*)_tmp2E2)!= 1)goto
_LL1DD;_tmp2E8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E2)->f2;if(_tmp2E8 <= (
void*)1)goto _LL1DD;if(*((int*)_tmp2E8)!= 4)goto _LL1DD;_tmp2E9=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2E8)->f1;if(!(!_tmp2E9->escapes))goto _LL1DD;_LL1DC: _tmp2EB=_tmp2E9;goto
_LL1DE;_LL1DD: if(*((int*)_tmp2E2)!= 1)goto _LL1DF;_tmp2EA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E2)->f2;if(_tmp2EA <= (void*)1)goto _LL1DF;if(*((int*)_tmp2EA)!= 2)goto _LL1DF;
_tmp2EB=((struct Cyc_Absyn_Param_b_struct*)_tmp2EA)->f1;if(!(!_tmp2EB->escapes))
goto _LL1DF;_LL1DE:{struct Cyc_CfFlowInfo_Reln*_tmp712;union Cyc_CfFlowInfo_RelnOp_union
_tmp711;struct Cyc_List_List*_tmp710;new_relns=((_tmp710=_region_malloc(env->r,
sizeof(*_tmp710)),((_tmp710->hd=((_tmp712=_region_malloc(env->r,sizeof(*_tmp712)),((
_tmp712->vd=_tmp184,((_tmp712->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((_tmp711.LessVar).tag=
1,(((_tmp711.LessVar).f1=_tmp2EB,_tmp711)))),_tmp712)))))),((_tmp710->tl=_tmp2DD,
_tmp710))))));}goto _LL1D4;_LL1DF: if(*((int*)_tmp2E2)!= 0)goto _LL1E1;_tmp2EC=((
struct Cyc_Absyn_Const_e_struct*)_tmp2E2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp2E2)->f1).Int_c).tag != 2)goto _LL1E1;_tmp2ED=(_tmp2EC.Int_c).f2;_LL1E0:{
struct Cyc_CfFlowInfo_Reln*_tmp718;union Cyc_CfFlowInfo_RelnOp_union _tmp717;struct
Cyc_List_List*_tmp716;new_relns=((_tmp716=_region_malloc(env->r,sizeof(*_tmp716)),((
_tmp716->hd=((_tmp718=_region_malloc(env->r,sizeof(*_tmp718)),((_tmp718->vd=
_tmp184,((_tmp718->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((_tmp717.LessConst).tag=
3,(((_tmp717.LessConst).f1=(unsigned int)_tmp2ED,_tmp717)))),_tmp718)))))),((
_tmp716->tl=_tmp2DD,_tmp716))))));}goto _LL1D4;_LL1E1: if(*((int*)_tmp2E2)!= 3)
goto _LL1E3;_tmp2EE=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2E2)->f1;
_tmp2EF=((struct Cyc_Absyn_Primop_e_struct*)_tmp2E2)->f2;if(_tmp2EF == 0)goto
_LL1E3;_tmp2F0=*_tmp2EF;_tmp2F1=(struct Cyc_Absyn_Exp*)_tmp2F0.hd;_LL1E2:{void*
_tmp2F8=(void*)_tmp2F1->r;void*_tmp2F9;struct Cyc_Absyn_Vardecl*_tmp2FA;void*
_tmp2FB;struct Cyc_Absyn_Vardecl*_tmp2FC;void*_tmp2FD;struct Cyc_Absyn_Vardecl*
_tmp2FE;void*_tmp2FF;struct Cyc_Absyn_Vardecl*_tmp300;_LL1E6: if(*((int*)_tmp2F8)
!= 1)goto _LL1E8;_tmp2F9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2F8)->f2;if(
_tmp2F9 <= (void*)1)goto _LL1E8;if(*((int*)_tmp2F9)!= 0)goto _LL1E8;_tmp2FA=((
struct Cyc_Absyn_Global_b_struct*)_tmp2F9)->f1;if(!(!_tmp2FA->escapes))goto _LL1E8;
_LL1E7: _tmp2FC=_tmp2FA;goto _LL1E9;_LL1E8: if(*((int*)_tmp2F8)!= 1)goto _LL1EA;
_tmp2FB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2F8)->f2;if(_tmp2FB <= (void*)
1)goto _LL1EA;if(*((int*)_tmp2FB)!= 3)goto _LL1EA;_tmp2FC=((struct Cyc_Absyn_Local_b_struct*)
_tmp2FB)->f1;if(!(!_tmp2FC->escapes))goto _LL1EA;_LL1E9: _tmp2FE=_tmp2FC;goto
_LL1EB;_LL1EA: if(*((int*)_tmp2F8)!= 1)goto _LL1EC;_tmp2FD=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2F8)->f2;if(_tmp2FD <= (void*)1)goto _LL1EC;if(*((int*)_tmp2FD)!= 4)goto _LL1EC;
_tmp2FE=((struct Cyc_Absyn_Pat_b_struct*)_tmp2FD)->f1;if(!(!_tmp2FE->escapes))
goto _LL1EC;_LL1EB: _tmp300=_tmp2FE;goto _LL1ED;_LL1EC: if(*((int*)_tmp2F8)!= 1)goto
_LL1EE;_tmp2FF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2F8)->f2;if(_tmp2FF <= (
void*)1)goto _LL1EE;if(*((int*)_tmp2FF)!= 2)goto _LL1EE;_tmp300=((struct Cyc_Absyn_Param_b_struct*)
_tmp2FF)->f1;if(!(!_tmp300->escapes))goto _LL1EE;_LL1ED:{struct Cyc_CfFlowInfo_Reln*
_tmp71E;union Cyc_CfFlowInfo_RelnOp_union _tmp71D;struct Cyc_List_List*_tmp71C;
new_relns=((_tmp71C=_region_malloc(env->r,sizeof(*_tmp71C)),((_tmp71C->hd=((
_tmp71E=_region_malloc(env->r,sizeof(*_tmp71E)),((_tmp71E->vd=_tmp184,((_tmp71E->rop=(
union Cyc_CfFlowInfo_RelnOp_union)(((_tmp71D.LessNumelts).tag=2,(((_tmp71D.LessNumelts).f1=
_tmp300,_tmp71D)))),_tmp71E)))))),((_tmp71C->tl=_tmp2DD,_tmp71C))))));}goto
_LL1E5;_LL1EE:;_LL1EF: goto _LL1E5;_LL1E5:;}goto _LL1D4;_LL1E3:;_LL1E4: goto _LL1D4;
_LL1D4:;}if(_tmp2DD != new_relns){union Cyc_CfFlowInfo_FlowInfo_union _tmp71F;
_tmp2D9=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp71F.ReachableFL).tag=1,(((
_tmp71F.ReachableFL).f1=_tmp2DC,(((_tmp71F.ReachableFL).f2=new_relns,(((_tmp71F.ReachableFL).f3=
_tmp2DE,_tmp71F))))))));}{void*_tmp305=_tmp2DA;_LL1F1: if((int)_tmp305 != 0)goto
_LL1F3;_LL1F2: {struct _tuple5 _tmp720;return(_tmp720.f1=_tmp2D9,((_tmp720.f2=(
void*)_tmp137->unknown_all,_tmp720)));}_LL1F3: if((int)_tmp305 != 2)goto _LL1F5;
_LL1F4: goto _LL1F6;_LL1F5: if((int)_tmp305 != 1)goto _LL1F7;_LL1F6: goto _LL1F8;_LL1F7:
if(_tmp305 <= (void*)3)goto _LL1F9;if(*((int*)_tmp305)!= 2)goto _LL1F9;_LL1F8: {
struct Cyc_List_List _tmp721;struct Cyc_List_List _tmp307=(_tmp721.hd=_tmp184,((
_tmp721.tl=0,_tmp721)));_tmp2D9=Cyc_NewControlFlow_add_vars(_tmp137,_tmp2D9,(
struct Cyc_List_List*)& _tmp307,(void*)_tmp137->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp309;void*_tmp30A;struct _tuple5 _tmp308=Cyc_NewControlFlow_anal_Rexp(env,
_tmp2D9,_tmp186);_tmp309=_tmp308.f1;_tmp30A=_tmp308.f2;_tmp309=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp186->loc,_tmp309);{union Cyc_CfFlowInfo_FlowInfo_union _tmp30B=_tmp309;struct
Cyc_Dict_Dict _tmp30C;struct Cyc_CfFlowInfo_ConsumeInfo _tmp30D;_LL1FC: if((_tmp30B.BottomFL).tag
!= 0)goto _LL1FE;_LL1FD: {struct _tuple5 _tmp722;return(_tmp722.f1=_tmp309,((
_tmp722.f2=(void*)_tmp137->unknown_all,_tmp722)));}_LL1FE: if((_tmp30B.ReachableFL).tag
!= 1)goto _LL1FB;_tmp30C=(_tmp30B.ReachableFL).f1;_tmp30D=(_tmp30B.ReachableFL).f3;
_LL1FF: if(Cyc_CfFlowInfo_initlevel(_tmp30C,_tmp30A)!= (void*)2){{const char*
_tmp725;void*_tmp724;(_tmp724=0,Cyc_Tcutil_terr(_tmp185->loc,((_tmp725="expression may not be initialized",
_tag_dyneither(_tmp725,sizeof(char),34))),_tag_dyneither(_tmp724,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp728;struct _tuple5 _tmp727;return(_tmp727.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp728.BottomFL).tag=0,_tmp728)),((
_tmp727.f2=(void*)_tmp137->unknown_all,_tmp727)));}}_LL1FB:;}_tmp2D9=_tmp309;
goto _LL1FA;}}_LL1F9:;_LL1FA: while(1){struct Cyc_List_List _tmp729;struct Cyc_List_List
_tmp314=(_tmp729.hd=_tmp184,((_tmp729.tl=0,_tmp729)));_tmp2D9=Cyc_NewControlFlow_add_vars(
_tmp137,_tmp2D9,(struct Cyc_List_List*)& _tmp314,(void*)_tmp137->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp316;void*_tmp317;struct _tuple5 _tmp315=Cyc_NewControlFlow_anal_Rexp(
env,_tmp2D9,_tmp186);_tmp316=_tmp315.f1;_tmp317=_tmp315.f2;_tmp316=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp186->loc,_tmp316);{union Cyc_CfFlowInfo_FlowInfo_union _tmp318=_tmp316;struct
Cyc_Dict_Dict _tmp319;struct Cyc_CfFlowInfo_ConsumeInfo _tmp31A;_LL201: if((_tmp318.BottomFL).tag
!= 0)goto _LL203;_LL202: goto _LL200;_LL203: if((_tmp318.ReachableFL).tag != 1)goto
_LL200;_tmp319=(_tmp318.ReachableFL).f1;_tmp31A=(_tmp318.ReachableFL).f3;_LL204:
if(Cyc_CfFlowInfo_initlevel(_tmp319,_tmp317)!= (void*)2){{const char*_tmp72C;void*
_tmp72B;(_tmp72B=0,Cyc_Tcutil_terr(_tmp185->loc,((_tmp72C="expression may not be initialized",
_tag_dyneither(_tmp72C,sizeof(char),34))),_tag_dyneither(_tmp72B,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp72F;struct _tuple5 _tmp72E;return(_tmp72E.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp72F.BottomFL).tag=0,_tmp72F)),((
_tmp72E.f2=(void*)_tmp137->unknown_all,_tmp72E)));}}_LL200:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp31F=Cyc_CfFlowInfo_join_flow(_tmp137,env->all_changed,_tmp2D9,_tmp316,1);if(
Cyc_CfFlowInfo_flow_lessthan_approx(_tmp31F,_tmp2D9))break;_tmp2D9=_tmp31F;}}}{
struct _tuple5 _tmp730;return(_tmp730.f1=_tmp2D9,((_tmp730.f2=(void*)_tmp137->unknown_all,
_tmp730)));}_LL1F0:;}}_LL1CF:;}}_LL13B: if(*((int*)_tmp13E)!= 38)goto _LL13D;
_tmp188=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp13E)->f1;_LL13C: while(1){union
Cyc_CfFlowInfo_FlowInfo_union*_tmp323;struct _tuple9 _tmp322=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,_tmp188);_tmp323=_tmp322.f2;inflow=*_tmp323;{void*_tmp324=(void*)
_tmp188->r;struct Cyc_Absyn_Stmt*_tmp325;struct Cyc_Absyn_Stmt*_tmp326;struct Cyc_Absyn_Decl*
_tmp327;struct Cyc_Absyn_Stmt*_tmp328;struct Cyc_Absyn_Exp*_tmp329;_LL206: if(
_tmp324 <= (void*)1)goto _LL20C;if(*((int*)_tmp324)!= 1)goto _LL208;_tmp325=((
struct Cyc_Absyn_Seq_s_struct*)_tmp324)->f1;_tmp326=((struct Cyc_Absyn_Seq_s_struct*)
_tmp324)->f2;_LL207: inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp325);
_tmp188=_tmp326;goto _LL205;_LL208: if(*((int*)_tmp324)!= 11)goto _LL20A;_tmp327=((
struct Cyc_Absyn_Decl_s_struct*)_tmp324)->f1;_tmp328=((struct Cyc_Absyn_Decl_s_struct*)
_tmp324)->f2;_LL209: inflow=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp327);
_tmp188=_tmp328;goto _LL205;_LL20A: if(*((int*)_tmp324)!= 0)goto _LL20C;_tmp329=((
struct Cyc_Absyn_Exp_s_struct*)_tmp324)->f1;_LL20B: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp329);_LL20C:;_LL20D: {struct Cyc_Core_Impossible_struct _tmp736;
const char*_tmp735;struct Cyc_Core_Impossible_struct*_tmp734;(int)_throw((void*)((
_tmp734=_cycalloc(sizeof(*_tmp734)),((_tmp734[0]=((_tmp736.tag=Cyc_Core_Impossible,((
_tmp736.f1=((_tmp735="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp735,
sizeof(char),33))),_tmp736)))),_tmp734)))));}_LL205:;}}_LL13D: if(*((int*)_tmp13E)
!= 1)goto _LL13F;_tmp189=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp13E)->f2;if((
int)_tmp189 != 0)goto _LL13F;_LL13E: goto _LL140;_LL13F: if(*((int*)_tmp13E)!= 2)goto
_LL141;_LL140: goto _LL142;_LL141: if(*((int*)_tmp13E)!= 10)goto _LL143;_LL142: goto
_LL144;_LL143: if(*((int*)_tmp13E)!= 37)goto _LL145;_LL144: goto _LL146;_LL145: if(*((
int*)_tmp13E)!= 27)goto _LL147;_LL146: goto _LL148;_LL147: if(*((int*)_tmp13E)!= 39)
goto _LLE0;_LL148: {struct Cyc_Core_Impossible_struct _tmp73C;const char*_tmp73B;
struct Cyc_Core_Impossible_struct*_tmp73A;(int)_throw((void*)((_tmp73A=_cycalloc(
sizeof(*_tmp73A)),((_tmp73A[0]=((_tmp73C.tag=Cyc_Core_Impossible,((_tmp73C.f1=((
_tmp73B="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp73B,sizeof(char),31))),
_tmp73C)))),_tmp73A)))));}_LLE0:;}}static struct _tuple7 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo_union f,void*r,struct Cyc_List_List*
flds);static struct _tuple7 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo_union
f,void*r,struct Cyc_List_List*flds){struct Cyc_CfFlowInfo_FlowEnv*_tmp330=env->fenv;
void*_tmp331=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp332;void*_tmp333;struct Cyc_Absyn_PtrAtts _tmp334;
struct Cyc_Absyn_Conref*_tmp335;struct Cyc_Absyn_Conref*_tmp336;_LL20F: if(_tmp331
<= (void*)4)goto _LL211;if(*((int*)_tmp331)!= 4)goto _LL211;_tmp332=((struct Cyc_Absyn_PointerType_struct*)
_tmp331)->f1;_tmp333=(void*)_tmp332.elt_typ;_tmp334=_tmp332.ptr_atts;_tmp335=
_tmp334.bounds;_tmp336=_tmp334.zero_term;_LL210: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp337=f;struct Cyc_Dict_Dict _tmp338;struct Cyc_List_List*_tmp339;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp33A;_LL214: if((_tmp337.BottomFL).tag != 0)goto _LL216;_LL215: {union Cyc_CfFlowInfo_AbsLVal_union
_tmp73F;struct _tuple7 _tmp73E;return(_tmp73E.f1=f,((_tmp73E.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp73F.UnknownL).tag=1,_tmp73F)),_tmp73E)));}_LL216: if((_tmp337.ReachableFL).tag
!= 1)goto _LL213;_tmp338=(_tmp337.ReachableFL).f1;_tmp339=(_tmp337.ReachableFL).f2;
_tmp33A=(_tmp337.ReachableFL).f3;_LL217: if(Cyc_Tcutil_is_bound_one(_tmp335)){
void*_tmp33D=r;struct Cyc_CfFlowInfo_Place*_tmp33E;struct Cyc_CfFlowInfo_Place
_tmp33F;void*_tmp340;struct Cyc_List_List*_tmp341;void*_tmp342;_LL219: if((int)
_tmp33D != 1)goto _LL21B;_LL21A: goto _LL21C;_LL21B: if((int)_tmp33D != 2)goto _LL21D;
_LL21C:{struct Cyc_CfFlowInfo_NotZero_struct _tmp742;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp741;(void*)(e->annot=(void*)((void*)((_tmp741=_cycalloc(sizeof(*_tmp741)),((
_tmp741[0]=((_tmp742.tag=Cyc_CfFlowInfo_NotZero,((_tmp742.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp339),_tmp742)))),_tmp741))))));}goto _LL218;_LL21D: if(
_tmp33D <= (void*)3)goto _LL21F;if(*((int*)_tmp33D)!= 2)goto _LL21F;_tmp33E=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp33D)->f1;_tmp33F=*_tmp33E;_tmp340=(
void*)_tmp33F.root;_tmp341=_tmp33F.fields;_LL21E:{struct Cyc_CfFlowInfo_NotZero_struct
_tmp745;struct Cyc_CfFlowInfo_NotZero_struct*_tmp744;(void*)(e->annot=(void*)((
void*)((_tmp744=_cycalloc(sizeof(*_tmp744)),((_tmp744[0]=((_tmp745.tag=Cyc_CfFlowInfo_NotZero,((
_tmp745.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp339),_tmp745)))),
_tmp744))))));}{union Cyc_CfFlowInfo_AbsLVal_union _tmp74B;struct Cyc_CfFlowInfo_Place*
_tmp74A;struct _tuple7 _tmp749;return(_tmp749.f1=f,((_tmp749.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp74B.PlaceL).tag=0,(((_tmp74B.PlaceL).f1=((_tmp74A=_region_malloc(_tmp330->r,
sizeof(*_tmp74A)),((_tmp74A->root=(void*)_tmp340,((_tmp74A->fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp330->r,_tmp341,flds),_tmp74A)))))),_tmp74B)))),_tmp749)));}_LL21F: if((int)
_tmp33D != 0)goto _LL221;_LL220:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));{
union Cyc_CfFlowInfo_FlowInfo_union _tmp750;union Cyc_CfFlowInfo_AbsLVal_union
_tmp74F;struct _tuple7 _tmp74E;return(_tmp74E.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp750.BottomFL).tag=0,_tmp750)),((_tmp74E.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp74F.UnknownL).tag=1,_tmp74F)),_tmp74E)));}_LL221: if(_tmp33D <= (void*)3)goto
_LL223;if(*((int*)_tmp33D)!= 0)goto _LL223;_tmp342=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp33D)->f1;_LL222: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp342);goto
_LL224;_LL223:;_LL224: {struct Cyc_CfFlowInfo_UnknownZ_struct _tmp753;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp752;(void*)(e->annot=(void*)((void*)((_tmp752=_cycalloc(sizeof(*_tmp752)),((
_tmp752[0]=((_tmp753.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp753.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp339),_tmp753)))),_tmp752))))));}_LL218:;}else{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp756;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp755;(void*)(e->annot=(void*)((
void*)((_tmp755=_cycalloc(sizeof(*_tmp755)),((_tmp755[0]=((_tmp756.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp756.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp339),_tmp756)))),
_tmp755))))));}if(Cyc_CfFlowInfo_initlevel(_tmp338,r)== (void*)0){const char*
_tmp759;void*_tmp758;(_tmp758=0,Cyc_Tcutil_terr(e->loc,((_tmp759="dereference of possibly uninitialized pointer",
_tag_dyneither(_tmp759,sizeof(char),46))),_tag_dyneither(_tmp758,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_AbsLVal_union _tmp75C;struct _tuple7 _tmp75B;return(_tmp75B.f1=
f,((_tmp75B.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((_tmp75C.UnknownL).tag=1,
_tmp75C)),_tmp75B)));}_LL213:;}_LL211:;_LL212: {struct Cyc_Core_Impossible_struct
_tmp762;const char*_tmp761;struct Cyc_Core_Impossible_struct*_tmp760;(int)_throw((
void*)((_tmp760=_cycalloc(sizeof(*_tmp760)),((_tmp760[0]=((_tmp762.tag=Cyc_Core_Impossible,((
_tmp762.f1=((_tmp761="left deref of non-pointer-type",_tag_dyneither(_tmp761,
sizeof(char),31))),_tmp762)))),_tmp760)))));}_LL20E:;}static struct _tuple7 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds);static struct _tuple7 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*
_tmp358=env->fenv;{union Cyc_CfFlowInfo_FlowInfo_union _tmp359=inflow;struct Cyc_Dict_Dict
_tmp35A;struct Cyc_List_List*_tmp35B;_LL226: if((_tmp359.BottomFL).tag != 0)goto
_LL228;_LL227: {union Cyc_CfFlowInfo_FlowInfo_union _tmp767;union Cyc_CfFlowInfo_AbsLVal_union
_tmp766;struct _tuple7 _tmp765;return(_tmp765.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp767.BottomFL).tag=0,_tmp767)),((_tmp765.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp766.UnknownL).tag=1,_tmp766)),_tmp765)));}_LL228: if((_tmp359.ReachableFL).tag
!= 1)goto _LL225;_tmp35A=(_tmp359.ReachableFL).f1;_tmp35B=(_tmp359.ReachableFL).f2;
_LL229: d=_tmp35A;_LL225:;}{void*_tmp35F=(void*)e->r;struct Cyc_Absyn_Exp*_tmp360;
struct Cyc_Absyn_Exp*_tmp361;struct Cyc_Absyn_Exp*_tmp362;void*_tmp363;struct Cyc_Absyn_Vardecl*
_tmp364;void*_tmp365;struct Cyc_Absyn_Vardecl*_tmp366;void*_tmp367;struct Cyc_Absyn_Vardecl*
_tmp368;void*_tmp369;struct Cyc_Absyn_Vardecl*_tmp36A;struct Cyc_Absyn_Exp*_tmp36B;
struct _dyneither_ptr*_tmp36C;struct Cyc_Absyn_Exp*_tmp36D;struct Cyc_Absyn_Exp*
_tmp36E;struct Cyc_Absyn_Exp*_tmp36F;struct Cyc_Absyn_Exp*_tmp370;struct
_dyneither_ptr*_tmp371;_LL22B: if(*((int*)_tmp35F)!= 15)goto _LL22D;_tmp360=((
struct Cyc_Absyn_Cast_e_struct*)_tmp35F)->f2;_LL22C: _tmp361=_tmp360;goto _LL22E;
_LL22D: if(*((int*)_tmp35F)!= 13)goto _LL22F;_tmp361=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp35F)->f1;_LL22E: _tmp362=_tmp361;goto _LL230;_LL22F: if(*((int*)_tmp35F)!= 14)
goto _LL231;_tmp362=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp35F)->f1;_LL230:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp362,flds);_LL231: if(*((int*)
_tmp35F)!= 1)goto _LL233;_tmp363=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp35F)->f2;
if(_tmp363 <= (void*)1)goto _LL233;if(*((int*)_tmp363)!= 2)goto _LL233;_tmp364=((
struct Cyc_Absyn_Param_b_struct*)_tmp363)->f1;_LL232: _tmp366=_tmp364;goto _LL234;
_LL233: if(*((int*)_tmp35F)!= 1)goto _LL235;_tmp365=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp35F)->f2;if(_tmp365 <= (void*)1)goto _LL235;if(*((int*)_tmp365)!= 3)goto _LL235;
_tmp366=((struct Cyc_Absyn_Local_b_struct*)_tmp365)->f1;_LL234: _tmp368=_tmp366;
goto _LL236;_LL235: if(*((int*)_tmp35F)!= 1)goto _LL237;_tmp367=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp35F)->f2;if(_tmp367 <= (void*)1)goto _LL237;if(*((int*)_tmp367)!= 4)goto _LL237;
_tmp368=((struct Cyc_Absyn_Pat_b_struct*)_tmp367)->f1;_LL236: {union Cyc_CfFlowInfo_AbsLVal_union
_tmp776;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp775;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp774;struct Cyc_CfFlowInfo_Place*_tmp773;struct _tuple7 _tmp772;return(_tmp772.f1=
inflow,((_tmp772.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((_tmp776.PlaceL).tag=0,(((
_tmp776.PlaceL).f1=((_tmp773=_region_malloc(env->r,sizeof(*_tmp773)),((_tmp773->root=(
void*)((void*)((_tmp775=_region_malloc(env->r,sizeof(*_tmp775)),((_tmp775[0]=((
_tmp774.tag=0,((_tmp774.f1=_tmp368,_tmp774)))),_tmp775))))),((_tmp773->fields=
flds,_tmp773)))))),_tmp776)))),_tmp772)));}_LL237: if(*((int*)_tmp35F)!= 1)goto
_LL239;_tmp369=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp35F)->f2;if(_tmp369 <= (
void*)1)goto _LL239;if(*((int*)_tmp369)!= 0)goto _LL239;_tmp36A=((struct Cyc_Absyn_Global_b_struct*)
_tmp369)->f1;_LL238: {union Cyc_CfFlowInfo_AbsLVal_union _tmp779;struct _tuple7
_tmp778;return(_tmp778.f1=inflow,((_tmp778.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp779.UnknownL).tag=1,_tmp779)),_tmp778)));}_LL239: if(*((int*)_tmp35F)!= 24)
goto _LL23B;_tmp36B=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp35F)->f1;_tmp36C=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp35F)->f2;_LL23A:{void*_tmp379=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp36B->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp37A;void*_tmp37B;_LL244: if(_tmp379 <= (void*)4)goto _LL246;if(*((int*)_tmp379)
!= 4)goto _LL246;_tmp37A=((struct Cyc_Absyn_PointerType_struct*)_tmp379)->f1;
_tmp37B=(void*)_tmp37A.elt_typ;_LL245: if(!Cyc_Absyn_is_union_type(_tmp37B)){
struct Cyc_List_List*_tmp77A;flds=((_tmp77A=_region_malloc(env->r,sizeof(*_tmp77A)),((
_tmp77A->hd=(void*)Cyc_NewControlFlow_get_field_index(_tmp37B,_tmp36C),((_tmp77A->tl=
flds,_tmp77A))))));}goto _LL243;_LL246:;_LL247: {struct Cyc_Core_Impossible_struct
_tmp780;const char*_tmp77F;struct Cyc_Core_Impossible_struct*_tmp77E;(int)_throw((
void*)((_tmp77E=_cycalloc(sizeof(*_tmp77E)),((_tmp77E[0]=((_tmp780.tag=Cyc_Core_Impossible,((
_tmp780.f1=((_tmp77F="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp77F,sizeof(
char),25))),_tmp780)))),_tmp77E)))));}_LL243:;}_tmp36D=_tmp36B;goto _LL23C;_LL23B:
if(*((int*)_tmp35F)!= 22)goto _LL23D;_tmp36D=((struct Cyc_Absyn_Deref_e_struct*)
_tmp35F)->f1;_LL23C: {union Cyc_CfFlowInfo_FlowInfo_union _tmp381;void*_tmp382;
struct _tuple5 _tmp380=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp36D);_tmp381=
_tmp380.f1;_tmp382=_tmp380.f2;_tmp381=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp381);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp36D,_tmp381,_tmp382,
flds);}_LL23D: if(*((int*)_tmp35F)!= 25)goto _LL23F;_tmp36E=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp35F)->f1;_tmp36F=((struct Cyc_Absyn_Subscript_e_struct*)_tmp35F)->f2;_LL23E: {
void*_tmp383=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp36E->topt))->v);struct Cyc_Absyn_PtrInfo _tmp384;struct Cyc_Absyn_PtrAtts
_tmp385;struct Cyc_Absyn_Conref*_tmp386;_LL249: if(_tmp383 <= (void*)4)goto _LL24D;
if(*((int*)_tmp383)!= 9)goto _LL24B;_LL24A: {unsigned int _tmp387=(Cyc_Evexp_eval_const_uint_exp(
_tmp36F)).f1;struct Cyc_List_List*_tmp781;return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp36E,((_tmp781=_region_malloc(env->r,sizeof(*_tmp781)),((_tmp781->hd=(
void*)((int)_tmp387),((_tmp781->tl=flds,_tmp781)))))));}_LL24B: if(*((int*)
_tmp383)!= 4)goto _LL24D;_tmp384=((struct Cyc_Absyn_PointerType_struct*)_tmp383)->f1;
_tmp385=_tmp384.ptr_atts;_tmp386=_tmp385.bounds;_LL24C: {struct _RegionHandle
_tmp389=_new_region("temp");struct _RegionHandle*temp=& _tmp389;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp38C;struct Cyc_List_List*_tmp38D;struct Cyc_Absyn_Exp*
_tmp782[2];struct _tuple11 _tmp38B=Cyc_NewControlFlow_anal_unordered_Rexps(temp,
env,inflow,((_tmp782[1]=_tmp36F,((_tmp782[0]=_tmp36E,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(
_tmp782,sizeof(struct Cyc_Absyn_Exp*),2)))))),1);_tmp38C=_tmp38B.f1;_tmp38D=
_tmp38B.f2;_tmp38C=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp36F->loc,_tmp38C);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp38E=_tmp38C;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp38F=_tmp38C;struct Cyc_Dict_Dict _tmp390;struct Cyc_List_List*_tmp391;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp392;_LL250: if((_tmp38F.ReachableFL).tag != 1)goto _LL252;_tmp390=(_tmp38F.ReachableFL).f1;
_tmp391=(_tmp38F.ReachableFL).f2;_tmp392=(_tmp38F.ReachableFL).f3;_LL251: if(Cyc_CfFlowInfo_initlevel(
_tmp390,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp38D))->tl))->hd)== (void*)0){const char*_tmp785;void*_tmp784;(
_tmp784=0,Cyc_Tcutil_terr(_tmp36F->loc,((_tmp785="expression may not be initialized",
_tag_dyneither(_tmp785,sizeof(char),34))),_tag_dyneither(_tmp784,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp395=Cyc_NewControlFlow_add_subscript_reln(_tmp358->r,
_tmp391,_tmp36E,_tmp36F);if(_tmp391 != _tmp395){union Cyc_CfFlowInfo_FlowInfo_union
_tmp786;_tmp38E=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp786.ReachableFL).tag=
1,(((_tmp786.ReachableFL).f1=_tmp390,(((_tmp786.ReachableFL).f2=_tmp395,(((
_tmp786.ReachableFL).f3=_tmp392,_tmp786))))))));}goto _LL24F;}_LL252:;_LL253: goto
_LL24F;_LL24F:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp398;union Cyc_CfFlowInfo_AbsLVal_union
_tmp399;struct _tuple7 _tmp397=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp36E,
_tmp38C,(void*)((struct Cyc_List_List*)_check_null(_tmp38D))->hd,flds);_tmp398=
_tmp397.f1;_tmp399=_tmp397.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp39A=_tmp398;
_LL255: if((_tmp39A.BottomFL).tag != 0)goto _LL257;_LL256: {struct _tuple7 _tmp787;
struct _tuple7 _tmp39C=(_tmp787.f1=_tmp398,((_tmp787.f2=_tmp399,_tmp787)));
_npop_handler(0);return _tmp39C;}_LL257:;_LL258: {struct _tuple7 _tmp788;struct
_tuple7 _tmp39E=(_tmp788.f1=_tmp38E,((_tmp788.f2=_tmp399,_tmp788)));_npop_handler(
0);return _tmp39E;}_LL254:;}}}};_pop_region(temp);}_LL24D:;_LL24E: {struct Cyc_Core_Impossible_struct
_tmp78E;const char*_tmp78D;struct Cyc_Core_Impossible_struct*_tmp78C;(int)_throw((
void*)((_tmp78C=_cycalloc(sizeof(*_tmp78C)),((_tmp78C[0]=((_tmp78E.tag=Cyc_Core_Impossible,((
_tmp78E.f1=((_tmp78D="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp78D,
sizeof(char),33))),_tmp78E)))),_tmp78C)))));}_LL248:;}_LL23F: if(*((int*)_tmp35F)
!= 23)goto _LL241;_tmp370=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp35F)->f1;
_tmp371=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp35F)->f2;_LL240: if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp370->topt))->v)){union Cyc_CfFlowInfo_AbsLVal_union
_tmp791;struct _tuple7 _tmp790;return(_tmp790.f1=inflow,((_tmp790.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp791.UnknownL).tag=1,_tmp791)),_tmp790)));}{struct Cyc_List_List*_tmp792;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp370,((_tmp792=
_region_malloc(env->r,sizeof(*_tmp792)),((_tmp792->hd=(void*)Cyc_NewControlFlow_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp370->topt))->v,_tmp371),((_tmp792->tl=
flds,_tmp792)))))));}_LL241:;_LL242: {union Cyc_CfFlowInfo_FlowInfo_union _tmp797;
union Cyc_CfFlowInfo_AbsLVal_union _tmp796;struct _tuple7 _tmp795;return(_tmp795.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp797.BottomFL).tag=0,_tmp797)),((
_tmp795.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((_tmp796.UnknownL).tag=1,_tmp796)),
_tmp795)));}_LL22A:;}}static struct _tuple7 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e);static struct
_tuple7 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A9;union Cyc_CfFlowInfo_AbsLVal_union _tmp3AA;struct _tuple7 _tmp3A8=Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);_tmp3A9=_tmp3A8.f1;_tmp3AA=_tmp3A8.f2;{struct _tuple7 _tmp798;
return(_tmp798.f1=_tmp3A9,((_tmp798.f2=_tmp3AA,_tmp798)));}}static struct _tuple8
Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_Absyn_Exp*e);static struct _tuple8 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp3AC=env->fenv;{void*
_tmp3AD=(void*)e->r;struct Cyc_Absyn_Exp*_tmp3AE;struct Cyc_Absyn_Exp*_tmp3AF;
struct Cyc_Absyn_Exp*_tmp3B0;struct Cyc_Absyn_Exp*_tmp3B1;struct Cyc_Absyn_Exp*
_tmp3B2;struct Cyc_Absyn_Exp*_tmp3B3;struct Cyc_Absyn_Exp*_tmp3B4;struct Cyc_Absyn_Exp*
_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B6;void*_tmp3B7;struct Cyc_List_List*_tmp3B8;
struct Cyc_List_List _tmp3B9;struct Cyc_Absyn_Exp*_tmp3BA;struct Cyc_List_List*
_tmp3BB;void*_tmp3BC;struct Cyc_List_List*_tmp3BD;_LL25A: if(*((int*)_tmp3AD)!= 6)
goto _LL25C;_tmp3AE=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3AD)->f1;_tmp3AF=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp3AD)->f2;_tmp3B0=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp3AD)->f3;_LL25B: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3BF;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3C0;struct _tuple8 _tmp3BE=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3AE);
_tmp3BF=_tmp3BE.f1;_tmp3C0=_tmp3BE.f2;_tmp3BF=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3AE->loc,_tmp3BF);_tmp3C0=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3AE->loc,
_tmp3C0);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3C2;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3C3;struct _tuple8 _tmp3C1=Cyc_NewControlFlow_anal_test(env,_tmp3BF,_tmp3AF);
_tmp3C2=_tmp3C1.f1;_tmp3C3=_tmp3C1.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp3C5;
union Cyc_CfFlowInfo_FlowInfo_union _tmp3C6;struct _tuple8 _tmp3C4=Cyc_NewControlFlow_anal_test(
env,_tmp3C0,_tmp3B0);_tmp3C5=_tmp3C4.f1;_tmp3C6=_tmp3C4.f2;{struct _tuple8 _tmp799;
return(_tmp799.f1=Cyc_CfFlowInfo_join_flow(_tmp3AC,env->all_changed,_tmp3C2,
_tmp3C5,1),((_tmp799.f2=Cyc_CfFlowInfo_join_flow(_tmp3AC,env->all_changed,
_tmp3C3,_tmp3C6,1),_tmp799)));}}}}_LL25C: if(*((int*)_tmp3AD)!= 7)goto _LL25E;
_tmp3B1=((struct Cyc_Absyn_And_e_struct*)_tmp3AD)->f1;_tmp3B2=((struct Cyc_Absyn_And_e_struct*)
_tmp3AD)->f2;_LL25D: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3C9;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3CA;struct _tuple8 _tmp3C8=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3B1);
_tmp3C9=_tmp3C8.f1;_tmp3CA=_tmp3C8.f2;_tmp3C9=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3B1->loc,_tmp3C9);_tmp3CA=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3B1->loc,
_tmp3CA);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3CC;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3CD;struct _tuple8 _tmp3CB=Cyc_NewControlFlow_anal_test(env,_tmp3C9,_tmp3B2);
_tmp3CC=_tmp3CB.f1;_tmp3CD=_tmp3CB.f2;_tmp3CC=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3B2->loc,_tmp3CC);_tmp3CD=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3B2->loc,
_tmp3CD);{struct _tuple8 _tmp79A;return(_tmp79A.f1=_tmp3CC,((_tmp79A.f2=Cyc_CfFlowInfo_join_flow(
_tmp3AC,env->all_changed,_tmp3CA,_tmp3CD,0),_tmp79A)));}}}_LL25E: if(*((int*)
_tmp3AD)!= 8)goto _LL260;_tmp3B3=((struct Cyc_Absyn_Or_e_struct*)_tmp3AD)->f1;
_tmp3B4=((struct Cyc_Absyn_Or_e_struct*)_tmp3AD)->f2;_LL25F: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D0;union Cyc_CfFlowInfo_FlowInfo_union _tmp3D1;struct _tuple8 _tmp3CF=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3B3);_tmp3D0=_tmp3CF.f1;_tmp3D1=_tmp3CF.f2;_tmp3D0=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3B3->loc,_tmp3D0);_tmp3D1=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3B3->loc,
_tmp3D1);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3D3;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D4;struct _tuple8 _tmp3D2=Cyc_NewControlFlow_anal_test(env,_tmp3D1,_tmp3B4);
_tmp3D3=_tmp3D2.f1;_tmp3D4=_tmp3D2.f2;_tmp3D3=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3B4->loc,_tmp3D3);_tmp3D4=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3B4->loc,
_tmp3D4);{struct _tuple8 _tmp79B;return(_tmp79B.f1=Cyc_CfFlowInfo_join_flow(
_tmp3AC,env->all_changed,_tmp3D0,_tmp3D3,0),((_tmp79B.f2=_tmp3D4,_tmp79B)));}}}
_LL260: if(*((int*)_tmp3AD)!= 9)goto _LL262;_tmp3B5=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp3AD)->f1;_tmp3B6=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp3AD)->f2;_LL261: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp3D7;void*_tmp3D8;struct _tuple5 _tmp3D6=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp3B5);_tmp3D7=_tmp3D6.f1;_tmp3D8=_tmp3D6.f2;_tmp3D7=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp3B5->loc,_tmp3D7);return Cyc_NewControlFlow_anal_test(env,_tmp3D7,_tmp3B6);}
_LL262: if(*((int*)_tmp3AD)!= 3)goto _LL264;_tmp3B7=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3AD)->f1;if((int)_tmp3B7 != 11)goto _LL264;_tmp3B8=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3AD)->f2;if(_tmp3B8 == 0)goto _LL264;_tmp3B9=*_tmp3B8;_tmp3BA=(struct Cyc_Absyn_Exp*)
_tmp3B9.hd;_tmp3BB=_tmp3B9.tl;if(_tmp3BB != 0)goto _LL264;_LL263: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3DA;union Cyc_CfFlowInfo_FlowInfo_union _tmp3DB;struct _tuple8 _tmp3D9=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3BA);_tmp3DA=_tmp3D9.f1;_tmp3DB=_tmp3D9.f2;{struct _tuple8 _tmp79C;
return(_tmp79C.f1=_tmp3DB,((_tmp79C.f2=_tmp3DA,_tmp79C)));}}_LL264: if(*((int*)
_tmp3AD)!= 3)goto _LL266;_tmp3BC=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3AD)->f1;_tmp3BD=((struct Cyc_Absyn_Primop_e_struct*)_tmp3AD)->f2;_LL265: {
void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo_union f;{struct _RegionHandle _tmp3DD=
_new_region("temp");struct _RegionHandle*temp=& _tmp3DD;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp3DF;struct Cyc_List_List*_tmp3E0;struct _tuple11
_tmp3DE=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp3BD,0);
_tmp3DF=_tmp3DE.f1;_tmp3E0=_tmp3DE.f2;r1=(void*)((struct Cyc_List_List*)
_check_null(_tmp3E0))->hd;r2=(void*)((struct Cyc_List_List*)_check_null(_tmp3E0->tl))->hd;
f=_tmp3DF;};_pop_region(temp);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp3E1=f;
struct Cyc_Dict_Dict _tmp3E2;struct Cyc_List_List*_tmp3E3;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3E4;_LL269: if((_tmp3E1.BottomFL).tag != 0)goto _LL26B;_LL26A: {struct _tuple8
_tmp79D;return(_tmp79D.f1=f,((_tmp79D.f2=f,_tmp79D)));}_LL26B: if((_tmp3E1.ReachableFL).tag
!= 1)goto _LL268;_tmp3E2=(_tmp3E1.ReachableFL).f1;_tmp3E3=(_tmp3E1.ReachableFL).f2;
_tmp3E4=(_tmp3E1.ReachableFL).f3;_LL26C: {struct Cyc_Absyn_Exp*_tmp3E6=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3BD))->hd;struct Cyc_Absyn_Exp*_tmp3E7=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3BD->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
_tmp3E2,r1)== (void*)0){const char*_tmp7A0;void*_tmp79F;(_tmp79F=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp3BD->hd)->loc,((_tmp7A0="expression may not be initialized",
_tag_dyneither(_tmp7A0,sizeof(char),34))),_tag_dyneither(_tmp79F,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(_tmp3E2,r2)== (void*)0){const char*_tmp7A3;void*
_tmp7A2;(_tmp7A2=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3BD->tl))->hd)->loc,((_tmp7A3="expression may not be initialized",
_tag_dyneither(_tmp7A3,sizeof(char),34))),_tag_dyneither(_tmp7A2,sizeof(void*),0)));}
if(_tmp3BC == (void*)5  || _tmp3BC == (void*)6){struct _tuple0 _tmp7A4;struct _tuple0
_tmp3ED=(_tmp7A4.f1=r1,((_tmp7A4.f2=r2,_tmp7A4)));void*_tmp3EE;void*_tmp3EF;void*
_tmp3F0;void*_tmp3F1;void*_tmp3F2;void*_tmp3F3;void*_tmp3F4;void*_tmp3F5;void*
_tmp3F6;void*_tmp3F7;void*_tmp3F8;void*_tmp3F9;void*_tmp3FA;void*_tmp3FB;void*
_tmp3FC;void*_tmp3FD;void*_tmp3FE;void*_tmp3FF;void*_tmp400;void*_tmp401;_LL26E:
_tmp3EE=_tmp3ED.f1;if(_tmp3EE <= (void*)3)goto _LL270;if(*((int*)_tmp3EE)!= 0)goto
_LL270;_tmp3EF=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3EE)->f1;
_tmp3F0=_tmp3ED.f2;if((int)_tmp3F0 != 0)goto _LL270;_LL26F: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp403;union Cyc_CfFlowInfo_FlowInfo_union _tmp404;struct _tuple8 _tmp402=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp3E6,_tmp3EF);_tmp403=_tmp402.f1;_tmp404=_tmp402.f2;{void*_tmp405=
_tmp3BC;_LL283: if((int)_tmp405 != 5)goto _LL285;_LL284: {struct _tuple8 _tmp7A5;
return(_tmp7A5.f1=_tmp404,((_tmp7A5.f2=_tmp403,_tmp7A5)));}_LL285: if((int)
_tmp405 != 6)goto _LL287;_LL286: {struct _tuple8 _tmp7A6;return(_tmp7A6.f1=_tmp403,((
_tmp7A6.f2=_tmp404,_tmp7A6)));}_LL287:;_LL288: {struct Cyc_Core_Impossible_struct
_tmp7AC;const char*_tmp7AB;struct Cyc_Core_Impossible_struct*_tmp7AA;(int)_throw((
void*)((_tmp7AA=_cycalloc(sizeof(*_tmp7AA)),((_tmp7AA[0]=((_tmp7AC.tag=Cyc_Core_Impossible,((
_tmp7AC.f1=((_tmp7AB="anal_test, zero-split",_tag_dyneither(_tmp7AB,sizeof(char),
22))),_tmp7AC)))),_tmp7AA)))));}_LL282:;}}_LL270: _tmp3F1=_tmp3ED.f1;if((int)
_tmp3F1 != 0)goto _LL272;_tmp3F2=_tmp3ED.f2;if(_tmp3F2 <= (void*)3)goto _LL272;if(*((
int*)_tmp3F2)!= 0)goto _LL272;_tmp3F3=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp3F2)->f1;_LL271: {union Cyc_CfFlowInfo_FlowInfo_union _tmp40C;union Cyc_CfFlowInfo_FlowInfo_union
_tmp40D;struct _tuple8 _tmp40B=Cyc_NewControlFlow_splitzero(env,f,f,_tmp3E7,
_tmp3F3);_tmp40C=_tmp40B.f1;_tmp40D=_tmp40B.f2;{void*_tmp40E=_tmp3BC;_LL28A: if((
int)_tmp40E != 5)goto _LL28C;_LL28B: {struct _tuple8 _tmp7AD;return(_tmp7AD.f1=
_tmp40D,((_tmp7AD.f2=_tmp40C,_tmp7AD)));}_LL28C: if((int)_tmp40E != 6)goto _LL28E;
_LL28D: {struct _tuple8 _tmp7AE;return(_tmp7AE.f1=_tmp40C,((_tmp7AE.f2=_tmp40D,
_tmp7AE)));}_LL28E:;_LL28F: {struct Cyc_Core_Impossible_struct _tmp7B4;const char*
_tmp7B3;struct Cyc_Core_Impossible_struct*_tmp7B2;(int)_throw((void*)((_tmp7B2=
_cycalloc(sizeof(*_tmp7B2)),((_tmp7B2[0]=((_tmp7B4.tag=Cyc_Core_Impossible,((
_tmp7B4.f1=((_tmp7B3="anal_test, zero-split",_tag_dyneither(_tmp7B3,sizeof(char),
22))),_tmp7B4)))),_tmp7B2)))));}_LL289:;}}_LL272: _tmp3F4=_tmp3ED.f1;if((int)
_tmp3F4 != 0)goto _LL274;_tmp3F5=_tmp3ED.f2;if((int)_tmp3F5 != 0)goto _LL274;_LL273:
if(_tmp3BC == (void*)5){union Cyc_CfFlowInfo_FlowInfo_union _tmp7B7;struct _tuple8
_tmp7B6;return(_tmp7B6.f1=f,((_tmp7B6.f2=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7B7.BottomFL).tag=0,_tmp7B7)),_tmp7B6)));}else{union Cyc_CfFlowInfo_FlowInfo_union
_tmp7BA;struct _tuple8 _tmp7B9;return(_tmp7B9.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7BA.BottomFL).tag=0,_tmp7BA)),((_tmp7B9.f2=f,_tmp7B9)));}_LL274: _tmp3F6=
_tmp3ED.f1;if((int)_tmp3F6 != 0)goto _LL276;_tmp3F7=_tmp3ED.f2;if((int)_tmp3F7 != 1)
goto _LL276;_LL275: goto _LL277;_LL276: _tmp3F8=_tmp3ED.f1;if((int)_tmp3F8 != 0)goto
_LL278;_tmp3F9=_tmp3ED.f2;if((int)_tmp3F9 != 2)goto _LL278;_LL277: goto _LL279;
_LL278: _tmp3FA=_tmp3ED.f1;if((int)_tmp3FA != 0)goto _LL27A;_tmp3FB=_tmp3ED.f2;if(
_tmp3FB <= (void*)3)goto _LL27A;if(*((int*)_tmp3FB)!= 2)goto _LL27A;_LL279: goto
_LL27B;_LL27A: _tmp3FC=_tmp3ED.f1;if((int)_tmp3FC != 1)goto _LL27C;_tmp3FD=_tmp3ED.f2;
if((int)_tmp3FD != 0)goto _LL27C;_LL27B: goto _LL27D;_LL27C: _tmp3FE=_tmp3ED.f1;if((
int)_tmp3FE != 2)goto _LL27E;_tmp3FF=_tmp3ED.f2;if((int)_tmp3FF != 0)goto _LL27E;
_LL27D: goto _LL27F;_LL27E: _tmp400=_tmp3ED.f1;if(_tmp400 <= (void*)3)goto _LL280;if(*((
int*)_tmp400)!= 2)goto _LL280;_tmp401=_tmp3ED.f2;if((int)_tmp401 != 0)goto _LL280;
_LL27F: if(_tmp3BC == (void*)6){union Cyc_CfFlowInfo_FlowInfo_union _tmp7BD;struct
_tuple8 _tmp7BC;return(_tmp7BC.f1=f,((_tmp7BC.f2=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7BD.BottomFL).tag=0,_tmp7BD)),_tmp7BC)));}else{union Cyc_CfFlowInfo_FlowInfo_union
_tmp7C0;struct _tuple8 _tmp7BF;return(_tmp7BF.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7C0.BottomFL).tag=0,_tmp7C0)),((_tmp7BF.f2=f,_tmp7BF)));}_LL280:;_LL281: goto
_LL26D;_LL26D:;}{struct _tuple0 _tmp7C1;struct _tuple0 _tmp41D=(_tmp7C1.f1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3E6->topt))->v),((_tmp7C1.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3E7->topt))->v),_tmp7C1)));void*
_tmp41E;void*_tmp41F;void*_tmp420;void*_tmp421;void*_tmp422;void*_tmp423;_LL291:
_tmp41E=_tmp41D.f1;if(_tmp41E <= (void*)4)goto _LL293;if(*((int*)_tmp41E)!= 5)goto
_LL293;_tmp41F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp41E)->f1;if((int)
_tmp41F != 1)goto _LL293;_LL292: goto _LL294;_LL293: _tmp420=_tmp41D.f2;if(_tmp420 <= (
void*)4)goto _LL295;if(*((int*)_tmp420)!= 5)goto _LL295;_tmp421=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp420)->f1;if((int)_tmp421 != 1)goto _LL295;_LL294: goto _LL296;_LL295: _tmp422=
_tmp41D.f1;if(_tmp422 <= (void*)4)goto _LL297;if(*((int*)_tmp422)!= 18)goto _LL297;
_LL296: goto _LL298;_LL297: _tmp423=_tmp41D.f2;if(_tmp423 <= (void*)4)goto _LL299;if(*((
int*)_tmp423)!= 18)goto _LL299;_LL298: goto _LL290;_LL299:;_LL29A: {struct _tuple8
_tmp7C2;return(_tmp7C2.f1=f,((_tmp7C2.f2=f,_tmp7C2)));}_LL290:;}{void*_tmp425=
_tmp3BC;_LL29C: if((int)_tmp425 != 5)goto _LL29E;_LL29D: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp426=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3E6,r1,(void*)5,r2);_tmp426=Cyc_NewControlFlow_if_tagcmp(
env,_tmp426,_tmp3E7,r2,(void*)5,r1);{struct _tuple8 _tmp7C3;return(_tmp7C3.f1=
_tmp426,((_tmp7C3.f2=f,_tmp7C3)));}}_LL29E: if((int)_tmp425 != 6)goto _LL2A0;_LL29F: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp428=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3E6,r1,(void*)5,r2);_tmp428=Cyc_NewControlFlow_if_tagcmp(env,_tmp428,_tmp3E7,
r2,(void*)5,r1);{struct _tuple8 _tmp7C4;return(_tmp7C4.f1=f,((_tmp7C4.f2=_tmp428,
_tmp7C4)));}}_LL2A0: if((int)_tmp425 != 7)goto _LL2A2;_LL2A1: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp42A=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3E7,r2,(void*)8,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp42B=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3E6,r1,(void*)10,r2);struct
_tuple8 _tmp7C5;return(_tmp7C5.f1=_tmp42A,((_tmp7C5.f2=_tmp42B,_tmp7C5)));}_LL2A2:
if((int)_tmp425 != 9)goto _LL2A4;_LL2A3: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp42D=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3E7,r2,(void*)10,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp42E=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3E6,r1,(void*)8,r2);struct _tuple8
_tmp7C6;return(_tmp7C6.f1=_tmp42D,((_tmp7C6.f2=_tmp42E,_tmp7C6)));}_LL2A4: if((
int)_tmp425 != 8)goto _LL2A6;_LL2A5: {union Cyc_CfFlowInfo_FlowInfo_union _tmp430=
Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3E6,r1,(void*)8,r2);union Cyc_CfFlowInfo_FlowInfo_union
_tmp431=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3E7,r2,(void*)10,r1);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp432=_tmp430;struct Cyc_Dict_Dict _tmp433;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp434;_LL2AB: if((_tmp432.BottomFL).tag != 0)goto _LL2AD;_LL2AC: {struct Cyc_Core_Impossible_struct
_tmp7CC;const char*_tmp7CB;struct Cyc_Core_Impossible_struct*_tmp7CA;(int)_throw((
void*)((_tmp7CA=_cycalloc(sizeof(*_tmp7CA)),((_tmp7CA[0]=((_tmp7CC.tag=Cyc_Core_Impossible,((
_tmp7CC.f1=((_tmp7CB="anal_test, Lt",_tag_dyneither(_tmp7CB,sizeof(char),14))),
_tmp7CC)))),_tmp7CA)))));}_LL2AD: if((_tmp432.ReachableFL).tag != 1)goto _LL2AA;
_tmp433=(_tmp432.ReachableFL).f1;_tmp434=(_tmp432.ReachableFL).f3;_LL2AE: _tmp3E2=
_tmp433;_tmp3E4=_tmp434;_LL2AA:;}{void*_tmp438=(void*)_tmp3E6->r;void*_tmp439;
struct Cyc_Absyn_Vardecl*_tmp43A;void*_tmp43B;struct Cyc_Absyn_Vardecl*_tmp43C;
void*_tmp43D;struct Cyc_Absyn_Vardecl*_tmp43E;void*_tmp43F;struct Cyc_Absyn_Vardecl*
_tmp440;_LL2B0: if(*((int*)_tmp438)!= 1)goto _LL2B2;_tmp439=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp438)->f2;if(_tmp439 <= (void*)1)goto _LL2B2;if(*((int*)_tmp439)!= 0)goto _LL2B2;
_tmp43A=((struct Cyc_Absyn_Global_b_struct*)_tmp439)->f1;if(!(!_tmp43A->escapes))
goto _LL2B2;_LL2B1: _tmp43C=_tmp43A;goto _LL2B3;_LL2B2: if(*((int*)_tmp438)!= 1)goto
_LL2B4;_tmp43B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp438)->f2;if(_tmp43B <= (
void*)1)goto _LL2B4;if(*((int*)_tmp43B)!= 3)goto _LL2B4;_tmp43C=((struct Cyc_Absyn_Local_b_struct*)
_tmp43B)->f1;if(!(!_tmp43C->escapes))goto _LL2B4;_LL2B3: _tmp43E=_tmp43C;goto
_LL2B5;_LL2B4: if(*((int*)_tmp438)!= 1)goto _LL2B6;_tmp43D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp438)->f2;if(_tmp43D <= (void*)1)goto _LL2B6;if(*((int*)_tmp43D)!= 4)goto _LL2B6;
_tmp43E=((struct Cyc_Absyn_Pat_b_struct*)_tmp43D)->f1;if(!(!_tmp43E->escapes))
goto _LL2B6;_LL2B5: _tmp440=_tmp43E;goto _LL2B7;_LL2B6: if(*((int*)_tmp438)!= 1)goto
_LL2B8;_tmp43F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp438)->f2;if(_tmp43F <= (
void*)1)goto _LL2B8;if(*((int*)_tmp43F)!= 2)goto _LL2B8;_tmp440=((struct Cyc_Absyn_Param_b_struct*)
_tmp43F)->f1;if(!(!_tmp440->escapes))goto _LL2B8;_LL2B7: {void*_tmp441=(void*)
_tmp3E7->r;void*_tmp442;struct Cyc_Absyn_Vardecl*_tmp443;void*_tmp444;struct Cyc_Absyn_Vardecl*
_tmp445;void*_tmp446;struct Cyc_Absyn_Vardecl*_tmp447;void*_tmp448;struct Cyc_Absyn_Vardecl*
_tmp449;union Cyc_Absyn_Cnst_union _tmp44A;int _tmp44B;void*_tmp44C;struct Cyc_List_List*
_tmp44D;struct Cyc_List_List _tmp44E;struct Cyc_Absyn_Exp*_tmp44F;_LL2BB: if(*((int*)
_tmp441)!= 1)goto _LL2BD;_tmp442=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp441)->f2;
if(_tmp442 <= (void*)1)goto _LL2BD;if(*((int*)_tmp442)!= 0)goto _LL2BD;_tmp443=((
struct Cyc_Absyn_Global_b_struct*)_tmp442)->f1;if(!(!_tmp443->escapes))goto _LL2BD;
_LL2BC: _tmp445=_tmp443;goto _LL2BE;_LL2BD: if(*((int*)_tmp441)!= 1)goto _LL2BF;
_tmp444=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp441)->f2;if(_tmp444 <= (void*)
1)goto _LL2BF;if(*((int*)_tmp444)!= 3)goto _LL2BF;_tmp445=((struct Cyc_Absyn_Local_b_struct*)
_tmp444)->f1;if(!(!_tmp445->escapes))goto _LL2BF;_LL2BE: _tmp447=_tmp445;goto
_LL2C0;_LL2BF: if(*((int*)_tmp441)!= 1)goto _LL2C1;_tmp446=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp441)->f2;if(_tmp446 <= (void*)1)goto _LL2C1;if(*((int*)_tmp446)!= 4)goto _LL2C1;
_tmp447=((struct Cyc_Absyn_Pat_b_struct*)_tmp446)->f1;if(!(!_tmp447->escapes))
goto _LL2C1;_LL2C0: _tmp449=_tmp447;goto _LL2C2;_LL2C1: if(*((int*)_tmp441)!= 1)goto
_LL2C3;_tmp448=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp441)->f2;if(_tmp448 <= (
void*)1)goto _LL2C3;if(*((int*)_tmp448)!= 2)goto _LL2C3;_tmp449=((struct Cyc_Absyn_Param_b_struct*)
_tmp448)->f1;if(!(!_tmp449->escapes))goto _LL2C3;_LL2C2: {struct _RegionHandle*
_tmp450=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp7D2;union Cyc_CfFlowInfo_RelnOp_union
_tmp7D1;struct Cyc_List_List*_tmp7D0;struct Cyc_List_List*_tmp451=(_tmp7D0=
_region_malloc(_tmp450,sizeof(*_tmp7D0)),((_tmp7D0->hd=((_tmp7D2=_region_malloc(
_tmp450,sizeof(*_tmp7D2)),((_tmp7D2->vd=_tmp440,((_tmp7D2->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((
_tmp7D1.LessVar).tag=1,(((_tmp7D1.LessVar).f1=_tmp449,_tmp7D1)))),_tmp7D2)))))),((
_tmp7D0->tl=_tmp3E3,_tmp7D0)))));union Cyc_CfFlowInfo_FlowInfo_union _tmp7D5;
struct _tuple8 _tmp7D4;return(_tmp7D4.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7D5.ReachableFL).tag=1,(((_tmp7D5.ReachableFL).f1=_tmp3E2,(((_tmp7D5.ReachableFL).f2=
_tmp451,(((_tmp7D5.ReachableFL).f3=_tmp3E4,_tmp7D5)))))))),((_tmp7D4.f2=_tmp431,
_tmp7D4)));}_LL2C3: if(*((int*)_tmp441)!= 0)goto _LL2C5;_tmp44A=((struct Cyc_Absyn_Const_e_struct*)
_tmp441)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp441)->f1).Int_c).tag != 2)
goto _LL2C5;_tmp44B=(_tmp44A.Int_c).f2;_LL2C4: {struct _RegionHandle*_tmp457=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp7DB;union Cyc_CfFlowInfo_RelnOp_union _tmp7DA;struct
Cyc_List_List*_tmp7D9;struct Cyc_List_List*_tmp458=(_tmp7D9=_region_malloc(
_tmp457,sizeof(*_tmp7D9)),((_tmp7D9->hd=((_tmp7DB=_region_malloc(_tmp457,sizeof(*
_tmp7DB)),((_tmp7DB->vd=_tmp440,((_tmp7DB->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((
_tmp7DA.LessConst).tag=3,(((_tmp7DA.LessConst).f1=(unsigned int)_tmp44B,_tmp7DA)))),
_tmp7DB)))))),((_tmp7D9->tl=_tmp3E3,_tmp7D9)))));union Cyc_CfFlowInfo_FlowInfo_union
_tmp7DE;struct _tuple8 _tmp7DD;return(_tmp7DD.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7DE.ReachableFL).tag=1,(((_tmp7DE.ReachableFL).f1=_tmp3E2,(((_tmp7DE.ReachableFL).f2=
_tmp458,(((_tmp7DE.ReachableFL).f3=_tmp3E4,_tmp7DE)))))))),((_tmp7DD.f2=_tmp431,
_tmp7DD)));}_LL2C5: if(*((int*)_tmp441)!= 3)goto _LL2C7;_tmp44C=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp441)->f1;_tmp44D=((struct Cyc_Absyn_Primop_e_struct*)_tmp441)->f2;if(_tmp44D
== 0)goto _LL2C7;_tmp44E=*_tmp44D;_tmp44F=(struct Cyc_Absyn_Exp*)_tmp44E.hd;_LL2C6: {
void*_tmp45E=(void*)_tmp44F->r;void*_tmp45F;struct Cyc_Absyn_Vardecl*_tmp460;void*
_tmp461;struct Cyc_Absyn_Vardecl*_tmp462;void*_tmp463;struct Cyc_Absyn_Vardecl*
_tmp464;void*_tmp465;struct Cyc_Absyn_Vardecl*_tmp466;_LL2CA: if(*((int*)_tmp45E)
!= 1)goto _LL2CC;_tmp45F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp45E)->f2;if(
_tmp45F <= (void*)1)goto _LL2CC;if(*((int*)_tmp45F)!= 0)goto _LL2CC;_tmp460=((
struct Cyc_Absyn_Global_b_struct*)_tmp45F)->f1;if(!(!_tmp460->escapes))goto _LL2CC;
_LL2CB: _tmp462=_tmp460;goto _LL2CD;_LL2CC: if(*((int*)_tmp45E)!= 1)goto _LL2CE;
_tmp461=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp45E)->f2;if(_tmp461 <= (void*)
1)goto _LL2CE;if(*((int*)_tmp461)!= 3)goto _LL2CE;_tmp462=((struct Cyc_Absyn_Local_b_struct*)
_tmp461)->f1;if(!(!_tmp462->escapes))goto _LL2CE;_LL2CD: _tmp464=_tmp462;goto
_LL2CF;_LL2CE: if(*((int*)_tmp45E)!= 1)goto _LL2D0;_tmp463=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp45E)->f2;if(_tmp463 <= (void*)1)goto _LL2D0;if(*((int*)_tmp463)!= 4)goto _LL2D0;
_tmp464=((struct Cyc_Absyn_Pat_b_struct*)_tmp463)->f1;if(!(!_tmp464->escapes))
goto _LL2D0;_LL2CF: _tmp466=_tmp464;goto _LL2D1;_LL2D0: if(*((int*)_tmp45E)!= 1)goto
_LL2D2;_tmp465=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp45E)->f2;if(_tmp465 <= (
void*)1)goto _LL2D2;if(*((int*)_tmp465)!= 2)goto _LL2D2;_tmp466=((struct Cyc_Absyn_Param_b_struct*)
_tmp465)->f1;if(!(!_tmp466->escapes))goto _LL2D2;_LL2D1: {struct _RegionHandle*
_tmp467=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp7E4;union Cyc_CfFlowInfo_RelnOp_union
_tmp7E3;struct Cyc_List_List*_tmp7E2;struct Cyc_List_List*_tmp468=(_tmp7E2=
_region_malloc(_tmp467,sizeof(*_tmp7E2)),((_tmp7E2->hd=((_tmp7E4=_region_malloc(
_tmp467,sizeof(*_tmp7E4)),((_tmp7E4->vd=_tmp440,((_tmp7E4->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((
_tmp7E3.LessNumelts).tag=2,(((_tmp7E3.LessNumelts).f1=_tmp466,_tmp7E3)))),
_tmp7E4)))))),((_tmp7E2->tl=_tmp3E3,_tmp7E2)))));union Cyc_CfFlowInfo_FlowInfo_union
_tmp7E7;struct _tuple8 _tmp7E6;return(_tmp7E6.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7E7.ReachableFL).tag=1,(((_tmp7E7.ReachableFL).f1=_tmp3E2,(((_tmp7E7.ReachableFL).f2=
_tmp468,(((_tmp7E7.ReachableFL).f3=_tmp3E4,_tmp7E7)))))))),((_tmp7E6.f2=_tmp431,
_tmp7E6)));}_LL2D2:;_LL2D3: {struct _tuple8 _tmp7E8;return(_tmp7E8.f1=_tmp430,((
_tmp7E8.f2=_tmp431,_tmp7E8)));}_LL2C9:;}_LL2C7:;_LL2C8: {struct _tuple8 _tmp7E9;
return(_tmp7E9.f1=_tmp430,((_tmp7E9.f2=_tmp431,_tmp7E9)));}_LL2BA:;}_LL2B8:;
_LL2B9: {struct _tuple8 _tmp7EA;return(_tmp7EA.f1=_tmp430,((_tmp7EA.f2=_tmp431,
_tmp7EA)));}_LL2AF:;}}_LL2A6: if((int)_tmp425 != 10)goto _LL2A8;_LL2A7: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp471=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3E6,r1,(void*)10,r2);union Cyc_CfFlowInfo_FlowInfo_union
_tmp472=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3E7,r1,(void*)8,r1);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp473=_tmp471;struct Cyc_Dict_Dict _tmp474;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp475;_LL2D5: if((_tmp473.BottomFL).tag != 0)goto _LL2D7;_LL2D6: {struct Cyc_Core_Impossible_struct
_tmp7F0;const char*_tmp7EF;struct Cyc_Core_Impossible_struct*_tmp7EE;(int)_throw((
void*)((_tmp7EE=_cycalloc(sizeof(*_tmp7EE)),((_tmp7EE[0]=((_tmp7F0.tag=Cyc_Core_Impossible,((
_tmp7F0.f1=((_tmp7EF="anal_test, Lte",_tag_dyneither(_tmp7EF,sizeof(char),15))),
_tmp7F0)))),_tmp7EE)))));}_LL2D7: if((_tmp473.ReachableFL).tag != 1)goto _LL2D4;
_tmp474=(_tmp473.ReachableFL).f1;_tmp475=(_tmp473.ReachableFL).f3;_LL2D8: _tmp3E2=
_tmp474;_tmp3E4=_tmp475;_LL2D4:;}{void*_tmp479=(void*)_tmp3E6->r;void*_tmp47A;
struct Cyc_Absyn_Vardecl*_tmp47B;void*_tmp47C;struct Cyc_Absyn_Vardecl*_tmp47D;
void*_tmp47E;struct Cyc_Absyn_Vardecl*_tmp47F;void*_tmp480;struct Cyc_Absyn_Vardecl*
_tmp481;_LL2DA: if(*((int*)_tmp479)!= 1)goto _LL2DC;_tmp47A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp479)->f2;if(_tmp47A <= (void*)1)goto _LL2DC;if(*((int*)_tmp47A)!= 0)goto _LL2DC;
_tmp47B=((struct Cyc_Absyn_Global_b_struct*)_tmp47A)->f1;if(!(!_tmp47B->escapes))
goto _LL2DC;_LL2DB: _tmp47D=_tmp47B;goto _LL2DD;_LL2DC: if(*((int*)_tmp479)!= 1)goto
_LL2DE;_tmp47C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp479)->f2;if(_tmp47C <= (
void*)1)goto _LL2DE;if(*((int*)_tmp47C)!= 3)goto _LL2DE;_tmp47D=((struct Cyc_Absyn_Local_b_struct*)
_tmp47C)->f1;if(!(!_tmp47D->escapes))goto _LL2DE;_LL2DD: _tmp47F=_tmp47D;goto
_LL2DF;_LL2DE: if(*((int*)_tmp479)!= 1)goto _LL2E0;_tmp47E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp479)->f2;if(_tmp47E <= (void*)1)goto _LL2E0;if(*((int*)_tmp47E)!= 4)goto _LL2E0;
_tmp47F=((struct Cyc_Absyn_Pat_b_struct*)_tmp47E)->f1;if(!(!_tmp47F->escapes))
goto _LL2E0;_LL2DF: _tmp481=_tmp47F;goto _LL2E1;_LL2E0: if(*((int*)_tmp479)!= 1)goto
_LL2E2;_tmp480=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp479)->f2;if(_tmp480 <= (
void*)1)goto _LL2E2;if(*((int*)_tmp480)!= 2)goto _LL2E2;_tmp481=((struct Cyc_Absyn_Param_b_struct*)
_tmp480)->f1;if(!(!_tmp481->escapes))goto _LL2E2;_LL2E1: {void*_tmp482=(void*)
_tmp3E7->r;void*_tmp483;struct Cyc_List_List*_tmp484;struct Cyc_List_List _tmp485;
struct Cyc_Absyn_Exp*_tmp486;_LL2E5: if(*((int*)_tmp482)!= 3)goto _LL2E7;_tmp483=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp482)->f1;_tmp484=((struct Cyc_Absyn_Primop_e_struct*)
_tmp482)->f2;if(_tmp484 == 0)goto _LL2E7;_tmp485=*_tmp484;_tmp486=(struct Cyc_Absyn_Exp*)
_tmp485.hd;_LL2E6: {void*_tmp487=(void*)_tmp486->r;void*_tmp488;struct Cyc_Absyn_Vardecl*
_tmp489;void*_tmp48A;struct Cyc_Absyn_Vardecl*_tmp48B;void*_tmp48C;struct Cyc_Absyn_Vardecl*
_tmp48D;void*_tmp48E;struct Cyc_Absyn_Vardecl*_tmp48F;_LL2EA: if(*((int*)_tmp487)
!= 1)goto _LL2EC;_tmp488=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp487)->f2;if(
_tmp488 <= (void*)1)goto _LL2EC;if(*((int*)_tmp488)!= 0)goto _LL2EC;_tmp489=((
struct Cyc_Absyn_Global_b_struct*)_tmp488)->f1;if(!(!_tmp489->escapes))goto _LL2EC;
_LL2EB: _tmp48B=_tmp489;goto _LL2ED;_LL2EC: if(*((int*)_tmp487)!= 1)goto _LL2EE;
_tmp48A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp487)->f2;if(_tmp48A <= (void*)
1)goto _LL2EE;if(*((int*)_tmp48A)!= 3)goto _LL2EE;_tmp48B=((struct Cyc_Absyn_Local_b_struct*)
_tmp48A)->f1;if(!(!_tmp48B->escapes))goto _LL2EE;_LL2ED: _tmp48D=_tmp48B;goto
_LL2EF;_LL2EE: if(*((int*)_tmp487)!= 1)goto _LL2F0;_tmp48C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp487)->f2;if(_tmp48C <= (void*)1)goto _LL2F0;if(*((int*)_tmp48C)!= 4)goto _LL2F0;
_tmp48D=((struct Cyc_Absyn_Pat_b_struct*)_tmp48C)->f1;if(!(!_tmp48D->escapes))
goto _LL2F0;_LL2EF: _tmp48F=_tmp48D;goto _LL2F1;_LL2F0: if(*((int*)_tmp487)!= 1)goto
_LL2F2;_tmp48E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp487)->f2;if(_tmp48E <= (
void*)1)goto _LL2F2;if(*((int*)_tmp48E)!= 2)goto _LL2F2;_tmp48F=((struct Cyc_Absyn_Param_b_struct*)
_tmp48E)->f1;if(!(!_tmp48F->escapes))goto _LL2F2;_LL2F1: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp490=env->fenv;struct Cyc_CfFlowInfo_Reln*_tmp7F6;union Cyc_CfFlowInfo_RelnOp_union
_tmp7F5;struct Cyc_List_List*_tmp7F4;struct Cyc_List_List*_tmp491=(_tmp7F4=
_region_malloc(_tmp490->r,sizeof(*_tmp7F4)),((_tmp7F4->hd=((_tmp7F6=
_region_malloc(_tmp490->r,sizeof(*_tmp7F6)),((_tmp7F6->vd=_tmp481,((_tmp7F6->rop=(
union Cyc_CfFlowInfo_RelnOp_union)(((_tmp7F5.LessEqNumelts).tag=4,(((_tmp7F5.LessEqNumelts).f1=
_tmp48F,_tmp7F5)))),_tmp7F6)))))),((_tmp7F4->tl=_tmp3E3,_tmp7F4)))));union Cyc_CfFlowInfo_FlowInfo_union
_tmp7F9;struct _tuple8 _tmp7F8;return(_tmp7F8.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7F9.ReachableFL).tag=1,(((_tmp7F9.ReachableFL).f1=_tmp3E2,(((_tmp7F9.ReachableFL).f2=
_tmp491,(((_tmp7F9.ReachableFL).f3=_tmp3E4,_tmp7F9)))))))),((_tmp7F8.f2=_tmp472,
_tmp7F8)));}_LL2F2:;_LL2F3: {struct _tuple8 _tmp7FA;return(_tmp7FA.f1=_tmp471,((
_tmp7FA.f2=_tmp472,_tmp7FA)));}_LL2E9:;}_LL2E7:;_LL2E8: {struct _tuple8 _tmp7FB;
return(_tmp7FB.f1=_tmp471,((_tmp7FB.f2=_tmp472,_tmp7FB)));}_LL2E4:;}_LL2E2:;
_LL2E3: {struct _tuple8 _tmp7FC;return(_tmp7FC.f1=_tmp471,((_tmp7FC.f2=_tmp472,
_tmp7FC)));}_LL2D9:;}}_LL2A8:;_LL2A9: {struct _tuple8 _tmp7FD;return(_tmp7FD.f1=f,((
_tmp7FD.f2=f,_tmp7FD)));}_LL29B:;}}_LL268:;}}_LL266:;_LL267: goto _LL259;_LL259:;}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp49C;void*_tmp49D;struct _tuple5 _tmp49B=Cyc_NewControlFlow_anal_Rexp(
env,inflow,e);_tmp49C=_tmp49B.f1;_tmp49D=_tmp49B.f2;_tmp49C=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp49C);{union Cyc_CfFlowInfo_FlowInfo_union _tmp49E=_tmp49C;struct Cyc_Dict_Dict
_tmp49F;_LL2F5: if((_tmp49E.BottomFL).tag != 0)goto _LL2F7;_LL2F6: {struct _tuple8
_tmp7FE;return(_tmp7FE.f1=_tmp49C,((_tmp7FE.f2=_tmp49C,_tmp7FE)));}_LL2F7: if((
_tmp49E.ReachableFL).tag != 1)goto _LL2F4;_tmp49F=(_tmp49E.ReachableFL).f1;_LL2F8: {
void*_tmp4A1=_tmp49D;void*_tmp4A2;void*_tmp4A3;void*_tmp4A4;_LL2FA: if((int)
_tmp4A1 != 0)goto _LL2FC;_LL2FB: {union Cyc_CfFlowInfo_FlowInfo_union _tmp801;struct
_tuple8 _tmp800;return(_tmp800.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp801.BottomFL).tag=
0,_tmp801)),((_tmp800.f2=_tmp49C,_tmp800)));}_LL2FC: if((int)_tmp4A1 != 2)goto
_LL2FE;_LL2FD: goto _LL2FF;_LL2FE: if((int)_tmp4A1 != 1)goto _LL300;_LL2FF: goto _LL301;
_LL300: if(_tmp4A1 <= (void*)3)goto _LL302;if(*((int*)_tmp4A1)!= 2)goto _LL302;
_LL301: {union Cyc_CfFlowInfo_FlowInfo_union _tmp804;struct _tuple8 _tmp803;return(
_tmp803.f1=_tmp49C,((_tmp803.f2=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp804.BottomFL).tag=
0,_tmp804)),_tmp803)));}_LL302: if(_tmp4A1 <= (void*)3)goto _LL304;if(*((int*)
_tmp4A1)!= 0)goto _LL304;_tmp4A2=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4A1)->f1;if((int)_tmp4A2 != 0)goto _LL304;_LL303: goto _LL305;_LL304: if(_tmp4A1
<= (void*)3)goto _LL306;if(*((int*)_tmp4A1)!= 1)goto _LL306;_tmp4A3=(void*)((
struct Cyc_CfFlowInfo_Esc_struct*)_tmp4A1)->f1;if((int)_tmp4A3 != 0)goto _LL306;
_LL305:{const char*_tmp807;void*_tmp806;(_tmp806=0,Cyc_Tcutil_terr(e->loc,((
_tmp807="expression may not be initialized",_tag_dyneither(_tmp807,sizeof(char),
34))),_tag_dyneither(_tmp806,sizeof(void*),0)));}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp80C;union Cyc_CfFlowInfo_FlowInfo_union _tmp80B;struct _tuple8 _tmp80A;return(
_tmp80A.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp80C.BottomFL).tag=0,
_tmp80C)),((_tmp80A.f2=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp80B.BottomFL).tag=
0,_tmp80B)),_tmp80A)));}_LL306: if(_tmp4A1 <= (void*)3)goto _LL308;if(*((int*)
_tmp4A1)!= 0)goto _LL308;_tmp4A4=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4A1)->f1;_LL307: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp49C,e,
_tmp4A4);_LL308: if(_tmp4A1 <= (void*)3)goto _LL30A;if(*((int*)_tmp4A1)!= 1)goto
_LL30A;_LL309: {struct _tuple8 _tmp80D;return(_tmp80D.f1=_tmp49C,((_tmp80D.f2=
_tmp49C,_tmp80D)));}_LL30A: if(_tmp4A1 <= (void*)3)goto _LL30C;if(*((int*)_tmp4A1)
!= 3)goto _LL30C;_LL30B: {struct _tuple8 _tmp80E;return(_tmp80E.f1=_tmp49C,((
_tmp80E.f2=_tmp49C,_tmp80E)));}_LL30C: if(_tmp4A1 <= (void*)3)goto _LL2F9;if(*((int*)
_tmp4A1)!= 4)goto _LL2F9;_LL30D: {struct Cyc_Core_Impossible_struct _tmp814;const
char*_tmp813;struct Cyc_Core_Impossible_struct*_tmp812;(int)_throw((void*)((
_tmp812=_cycalloc(sizeof(*_tmp812)),((_tmp812[0]=((_tmp814.tag=Cyc_Core_Impossible,((
_tmp814.f1=((_tmp813="anal_test",_tag_dyneither(_tmp813,sizeof(char),10))),
_tmp814)))),_tmp812)))));}_LL2F9:;}_LL2F4:;}}}static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
flow);static void Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*
loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union flow){
union Cyc_CfFlowInfo_FlowInfo_union _tmp4B3=flow;struct Cyc_Dict_Dict _tmp4B4;_LL30F:
if((_tmp4B3.BottomFL).tag != 0)goto _LL311;_LL310: return;_LL311: if((_tmp4B3.ReachableFL).tag
!= 1)goto _LL30E;_tmp4B4=(_tmp4B3.ReachableFL).f1;_LL312:{struct Cyc_List_List*
_tmp4B5=env->param_roots;for(0;_tmp4B5 != 0;_tmp4B5=_tmp4B5->tl){if(Cyc_CfFlowInfo_initlevel(
_tmp4B4,Cyc_CfFlowInfo_lookup_place(_tmp4B4,(struct Cyc_CfFlowInfo_Place*)_tmp4B5->hd))
!= (void*)2){const char*_tmp817;void*_tmp816;(_tmp816=0,Cyc_Tcutil_terr(loc,((
_tmp817="function may not initialize all the parameters with attribute 'initializes'",
_tag_dyneither(_tmp817,sizeof(char),76))),_tag_dyneither(_tmp816,sizeof(void*),0)));}}}
return;_LL30E:;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_List_List*scs);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp4B8=env->fenv;for(0;
scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp4BA;struct Cyc_Core_Opt*
_tmp4BB;struct Cyc_Absyn_Exp*_tmp4BC;struct Cyc_Absyn_Stmt*_tmp4BD;struct Cyc_Position_Segment*
_tmp4BE;struct Cyc_Absyn_Switch_clause*_tmp4B9=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp4BA=*_tmp4B9;_tmp4BB=_tmp4BA.pat_vars;_tmp4BC=_tmp4BA.where_clause;
_tmp4BD=_tmp4BA.body;_tmp4BE=_tmp4BA.loc;{union Cyc_CfFlowInfo_FlowInfo_union
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp4B8,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp4BB))->v,(void*)_tmp4B8->unknown_all,_tmp4BE);
union Cyc_CfFlowInfo_FlowInfo_union clause_outflow;if(_tmp4BC != 0){struct Cyc_Absyn_Exp*
wexp=(struct Cyc_Absyn_Exp*)_tmp4BC;union Cyc_CfFlowInfo_FlowInfo_union _tmp4C0;
union Cyc_CfFlowInfo_FlowInfo_union _tmp4C1;struct _tuple8 _tmp4BF=Cyc_NewControlFlow_anal_test(
env,clause_inflow,wexp);_tmp4C0=_tmp4BF.f1;_tmp4C1=_tmp4BF.f2;_tmp4C0=Cyc_CfFlowInfo_readthrough_unique_rvals(
wexp->loc,_tmp4C0);_tmp4C1=Cyc_CfFlowInfo_readthrough_unique_rvals(wexp->loc,
_tmp4C1);inflow=_tmp4C1;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp4C0,
_tmp4BD);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,
_tmp4BD);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp4C2=clause_outflow;_LL314: if((
_tmp4C2.BottomFL).tag != 0)goto _LL316;_LL315: goto _LL313;_LL316:;_LL317: if(scs->tl
== 0)return clause_outflow;else{if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(
scs->tl))->hd)->pat_vars))->v != 0){const char*_tmp81A;void*_tmp819;(_tmp819=0,Cyc_Tcutil_terr(
_tmp4BD->loc,((_tmp81A="switch clause may implicitly fallthru",_tag_dyneither(
_tmp81A,sizeof(char),38))),_tag_dyneither(_tmp819,sizeof(void*),0)));}else{const
char*_tmp81D;void*_tmp81C;(_tmp81C=0,Cyc_Tcutil_warn(_tmp4BD->loc,((_tmp81D="switch clause may implicitly fallthru",
_tag_dyneither(_tmp81D,sizeof(char),38))),_tag_dyneither(_tmp81C,sizeof(void*),0)));}
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->body,clause_outflow);}goto _LL313;_LL313:;}}}{union
Cyc_CfFlowInfo_FlowInfo_union _tmp81E;return(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp81E.BottomFL).tag=0,_tmp81E));}}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Stmt*s);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union outflow;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp4C9;union Cyc_CfFlowInfo_FlowInfo_union*_tmp4CA;struct _tuple9 _tmp4C8=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp4C9=_tmp4C8.f1;_tmp4CA=_tmp4C8.f2;inflow=*_tmp4CA;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp4CB=env->fenv;{void*_tmp4CC=(void*)s->r;struct Cyc_Absyn_Exp*_tmp4CD;struct
Cyc_Absyn_Exp*_tmp4CE;struct Cyc_Absyn_Exp*_tmp4CF;struct Cyc_Absyn_Stmt*_tmp4D0;
struct Cyc_Absyn_Stmt*_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D2;struct Cyc_Absyn_Stmt*
_tmp4D3;struct Cyc_Absyn_Stmt*_tmp4D4;struct _tuple3 _tmp4D5;struct Cyc_Absyn_Exp*
_tmp4D6;struct Cyc_Absyn_Stmt*_tmp4D7;struct Cyc_Absyn_Stmt*_tmp4D8;struct Cyc_Absyn_Stmt*
_tmp4D9;struct _tuple3 _tmp4DA;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Stmt*
_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DD;struct _tuple3 _tmp4DE;struct Cyc_Absyn_Exp*
_tmp4DF;struct Cyc_Absyn_Stmt*_tmp4E0;struct _tuple3 _tmp4E1;struct Cyc_Absyn_Exp*
_tmp4E2;struct Cyc_Absyn_Stmt*_tmp4E3;struct Cyc_Absyn_Stmt*_tmp4E4;struct Cyc_Absyn_Stmt*
_tmp4E5;struct Cyc_List_List*_tmp4E6;struct Cyc_Absyn_Switch_clause**_tmp4E7;
struct Cyc_Absyn_Switch_clause*_tmp4E8;struct Cyc_Absyn_Stmt*_tmp4E9;struct Cyc_Absyn_Stmt*
_tmp4EA;struct Cyc_Absyn_Stmt*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EC;struct Cyc_List_List*
_tmp4ED;struct Cyc_Absyn_Stmt*_tmp4EE;struct Cyc_List_List*_tmp4EF;struct Cyc_Absyn_Decl*
_tmp4F0;struct Cyc_Absyn_Stmt*_tmp4F1;struct Cyc_Absyn_Stmt*_tmp4F2;struct Cyc_Absyn_Tvar*
_tmp4F3;struct Cyc_Absyn_Vardecl*_tmp4F4;int _tmp4F5;struct Cyc_Absyn_Exp*_tmp4F6;
struct Cyc_Absyn_Stmt*_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F8;struct Cyc_Absyn_Exp*
_tmp4F9;struct Cyc_Absyn_Tvar*_tmp4FA;struct Cyc_Absyn_Vardecl*_tmp4FB;struct Cyc_Absyn_Stmt*
_tmp4FC;_LL319: if((int)_tmp4CC != 0)goto _LL31B;_LL31A: return inflow;_LL31B: if(
_tmp4CC <= (void*)1)goto _LL343;if(*((int*)_tmp4CC)!= 2)goto _LL31D;_tmp4CD=((
struct Cyc_Absyn_Return_s_struct*)_tmp4CC)->f1;if(_tmp4CD != 0)goto _LL31D;_LL31C:
if(env->noreturn){const char*_tmp821;void*_tmp820;(_tmp820=0,Cyc_Tcutil_terr(s->loc,((
_tmp821="`noreturn' function might return",_tag_dyneither(_tmp821,sizeof(char),
33))),_tag_dyneither(_tmp820,sizeof(void*),0)));}Cyc_NewControlFlow_check_init_params(
s->loc,env,inflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp822;return(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp822.BottomFL).tag=0,_tmp822));}_LL31D: if(*((int*)_tmp4CC)!= 2)goto _LL31F;
_tmp4CE=((struct Cyc_Absyn_Return_s_struct*)_tmp4CC)->f1;_LL31E: if(env->noreturn){
const char*_tmp825;void*_tmp824;(_tmp824=0,Cyc_Tcutil_terr(s->loc,((_tmp825="`noreturn' function might return",
_tag_dyneither(_tmp825,sizeof(char),33))),_tag_dyneither(_tmp824,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp503;void*_tmp504;struct _tuple5 _tmp502=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp4CE));_tmp503=_tmp502.f1;
_tmp504=_tmp502.f2;_tmp503=Cyc_CfFlowInfo_consume_unique_rvals(_tmp4CE->loc,
_tmp503);_tmp503=Cyc_NewControlFlow_use_Rval(env,_tmp4CE->loc,_tmp503,_tmp504);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp503);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp826;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp826.BottomFL).tag=0,
_tmp826));}}_LL31F: if(*((int*)_tmp4CC)!= 0)goto _LL321;_tmp4CF=((struct Cyc_Absyn_Exp_s_struct*)
_tmp4CC)->f1;_LL320: outflow=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp4CF)).f1;
goto _LL318;_LL321: if(*((int*)_tmp4CC)!= 1)goto _LL323;_tmp4D0=((struct Cyc_Absyn_Seq_s_struct*)
_tmp4CC)->f1;_tmp4D1=((struct Cyc_Absyn_Seq_s_struct*)_tmp4CC)->f2;_LL322: outflow=
Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4D0),
_tmp4D1);goto _LL318;_LL323: if(*((int*)_tmp4CC)!= 3)goto _LL325;_tmp4D2=((struct
Cyc_Absyn_IfThenElse_s_struct*)_tmp4CC)->f1;_tmp4D3=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp4CC)->f2;_tmp4D4=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4CC)->f3;_LL324: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp507;union Cyc_CfFlowInfo_FlowInfo_union
_tmp508;struct _tuple8 _tmp506=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4D2);
_tmp507=_tmp506.f1;_tmp508=_tmp506.f2;_tmp507=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4D2->loc,_tmp507);_tmp508=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4D2->loc,
_tmp508);outflow=Cyc_CfFlowInfo_join_flow(_tmp4CB,env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp507,_tmp4D3),Cyc_NewControlFlow_anal_stmt(env,_tmp508,_tmp4D4),1);goto
_LL318;}_LL325: if(*((int*)_tmp4CC)!= 4)goto _LL327;_tmp4D5=((struct Cyc_Absyn_While_s_struct*)
_tmp4CC)->f1;_tmp4D6=_tmp4D5.f1;_tmp4D7=_tmp4D5.f2;_tmp4D8=((struct Cyc_Absyn_While_s_struct*)
_tmp4CC)->f2;_LL326: {union Cyc_CfFlowInfo_FlowInfo_union*_tmp50A;struct _tuple9
_tmp509=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp4D7);_tmp50A=_tmp509.f2;{
union Cyc_CfFlowInfo_FlowInfo_union _tmp50B=*_tmp50A;union Cyc_CfFlowInfo_FlowInfo_union
_tmp50D;union Cyc_CfFlowInfo_FlowInfo_union _tmp50E;struct _tuple8 _tmp50C=Cyc_NewControlFlow_anal_test(
env,_tmp50B,_tmp4D6);_tmp50D=_tmp50C.f1;_tmp50E=_tmp50C.f2;_tmp50D=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4D6->loc,_tmp50D);_tmp50E=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4D6->loc,
_tmp50E);{union Cyc_CfFlowInfo_FlowInfo_union _tmp50F=Cyc_NewControlFlow_anal_stmt(
env,_tmp50D,_tmp4D8);Cyc_NewControlFlow_update_flow(env,_tmp4D7,_tmp50F);outflow=
_tmp50E;goto _LL318;}}}_LL327: if(*((int*)_tmp4CC)!= 13)goto _LL329;_tmp4D9=((
struct Cyc_Absyn_Do_s_struct*)_tmp4CC)->f1;_tmp4DA=((struct Cyc_Absyn_Do_s_struct*)
_tmp4CC)->f2;_tmp4DB=_tmp4DA.f1;_tmp4DC=_tmp4DA.f2;_LL328: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp510=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4D9);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp512;struct _tuple9 _tmp511=Cyc_NewControlFlow_pre_stmt_check(env,_tmp510,
_tmp4DC);_tmp512=_tmp511.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp513=*_tmp512;
union Cyc_CfFlowInfo_FlowInfo_union _tmp515;union Cyc_CfFlowInfo_FlowInfo_union
_tmp516;struct _tuple8 _tmp514=Cyc_NewControlFlow_anal_test(env,_tmp513,_tmp4DB);
_tmp515=_tmp514.f1;_tmp516=_tmp514.f2;Cyc_NewControlFlow_update_flow(env,_tmp4D9,
_tmp515);outflow=_tmp516;goto _LL318;}}_LL329: if(*((int*)_tmp4CC)!= 8)goto _LL32B;
_tmp4DD=((struct Cyc_Absyn_For_s_struct*)_tmp4CC)->f1;_tmp4DE=((struct Cyc_Absyn_For_s_struct*)
_tmp4CC)->f2;_tmp4DF=_tmp4DE.f1;_tmp4E0=_tmp4DE.f2;_tmp4E1=((struct Cyc_Absyn_For_s_struct*)
_tmp4CC)->f3;_tmp4E2=_tmp4E1.f1;_tmp4E3=_tmp4E1.f2;_tmp4E4=((struct Cyc_Absyn_For_s_struct*)
_tmp4CC)->f4;_LL32A: {union Cyc_CfFlowInfo_FlowInfo_union _tmp517=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4DD)).f1;_tmp517=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4DD->loc,
_tmp517);{union Cyc_CfFlowInfo_FlowInfo_union*_tmp519;struct _tuple9 _tmp518=Cyc_NewControlFlow_pre_stmt_check(
env,_tmp517,_tmp4E0);_tmp519=_tmp518.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp51A=*_tmp519;union Cyc_CfFlowInfo_FlowInfo_union _tmp51C;union Cyc_CfFlowInfo_FlowInfo_union
_tmp51D;struct _tuple8 _tmp51B=Cyc_NewControlFlow_anal_test(env,_tmp51A,_tmp4DF);
_tmp51C=_tmp51B.f1;_tmp51D=_tmp51B.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp51E=
Cyc_NewControlFlow_anal_stmt(env,_tmp51C,_tmp4E4);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp520;struct _tuple9 _tmp51F=Cyc_NewControlFlow_pre_stmt_check(env,_tmp51E,
_tmp4E3);_tmp520=_tmp51F.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp521=*_tmp520;
union Cyc_CfFlowInfo_FlowInfo_union _tmp522=(Cyc_NewControlFlow_anal_Rexp(env,
_tmp521,_tmp4E2)).f1;_tmp522=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4E2->loc,
_tmp522);Cyc_NewControlFlow_update_flow(env,_tmp4E0,_tmp522);outflow=_tmp51D;
goto _LL318;}}}}}_LL32B: if(*((int*)_tmp4CC)!= 5)goto _LL32D;_tmp4E5=((struct Cyc_Absyn_Break_s_struct*)
_tmp4CC)->f1;if(_tmp4E5 != 0)goto _LL32D;_LL32C: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp827;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp827.BottomFL).tag=0,
_tmp827));}_LL32D: if(*((int*)_tmp4CC)!= 10)goto _LL32F;_tmp4E6=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp4CC)->f1;_tmp4E7=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp4CC)->f2;if(
_tmp4E7 == 0)goto _LL32F;_tmp4E8=*_tmp4E7;_LL32E: {struct _RegionHandle _tmp524=
_new_region("temp");struct _RegionHandle*temp=& _tmp524;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp526;struct Cyc_List_List*_tmp527;struct _tuple11
_tmp525=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp4E6,0);
_tmp526=_tmp525.f1;_tmp527=_tmp525.f2;for(0;_tmp527 != 0;(_tmp527=_tmp527->tl,
_tmp4E6=_tmp4E6->tl)){_tmp526=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4E6))->hd)->loc,_tmp526,(void*)_tmp527->hd);}
_tmp526=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,_tmp526);_tmp526=Cyc_NewControlFlow_add_vars(
_tmp4CB,_tmp526,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4E8->pat_vars))->v,(
void*)_tmp4CB->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp4E8->body,_tmp526);{union Cyc_CfFlowInfo_FlowInfo_union _tmp828;union Cyc_CfFlowInfo_FlowInfo_union
_tmp529=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp828.BottomFL).tag=0,_tmp828));
_npop_handler(0);return _tmp529;}};_pop_region(temp);}_LL32F: if(*((int*)_tmp4CC)
!= 5)goto _LL331;_tmp4E9=((struct Cyc_Absyn_Break_s_struct*)_tmp4CC)->f1;_LL330:
_tmp4EA=_tmp4E9;goto _LL332;_LL331: if(*((int*)_tmp4CC)!= 6)goto _LL333;_tmp4EA=((
struct Cyc_Absyn_Continue_s_struct*)_tmp4CC)->f1;_LL332: _tmp4EB=_tmp4EA;goto
_LL334;_LL333: if(*((int*)_tmp4CC)!= 7)goto _LL335;_tmp4EB=((struct Cyc_Absyn_Goto_s_struct*)
_tmp4CC)->f2;_LL334: if(env->iteration_num == 1){struct Cyc_Absyn_Stmt*_tmp52A=
_tmp4C9->encloser;struct Cyc_Absyn_Stmt*_tmp52B=(Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)_check_null(_tmp4EB)))->encloser;while(_tmp52B != _tmp52A){
struct Cyc_Absyn_Stmt*_tmp52C=(Cyc_NewControlFlow_get_stmt_annot(_tmp52A))->encloser;
if(_tmp52C == _tmp52A){{const char*_tmp82B;void*_tmp82A;(_tmp82A=0,Cyc_Tcutil_terr(
s->loc,((_tmp82B="goto enters local scope or exception handler",_tag_dyneither(
_tmp82B,sizeof(char),45))),_tag_dyneither(_tmp82A,sizeof(void*),0)));}break;}
_tmp52A=_tmp52C;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_check_null(_tmp4EB),inflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp82C;return(
union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp82C.BottomFL).tag=0,_tmp82C));}_LL335:
if(*((int*)_tmp4CC)!= 9)goto _LL337;_tmp4EC=((struct Cyc_Absyn_Switch_s_struct*)
_tmp4CC)->f1;_tmp4ED=((struct Cyc_Absyn_Switch_s_struct*)_tmp4CC)->f2;_LL336: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp531;void*_tmp532;struct _tuple5 _tmp530=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4EC);_tmp531=_tmp530.f1;_tmp532=_tmp530.f2;_tmp531=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4EC->loc,_tmp531);_tmp531=Cyc_NewControlFlow_use_Rval(env,_tmp4EC->loc,
_tmp531,_tmp532);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp531,_tmp4ED);goto
_LL318;}_LL337: if(*((int*)_tmp4CC)!= 14)goto _LL339;_tmp4EE=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp4CC)->f1;_tmp4EF=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4CC)->f2;_LL338: {
int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo_union old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo_union s1_outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4EE);union Cyc_CfFlowInfo_FlowInfo_union
scs_inflow=env->tryflow;env->in_try=old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(
env,scs_inflow);{union Cyc_CfFlowInfo_FlowInfo_union scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp4EF);{union Cyc_CfFlowInfo_FlowInfo_union _tmp533=scs_outflow;
_LL346: if((_tmp533.BottomFL).tag != 0)goto _LL348;_LL347: goto _LL345;_LL348:;_LL349: {
const char*_tmp82F;void*_tmp82E;(_tmp82E=0,Cyc_Tcutil_terr(s->loc,((_tmp82F="last catch clause may implicitly fallthru",
_tag_dyneither(_tmp82F,sizeof(char),42))),_tag_dyneither(_tmp82E,sizeof(void*),0)));}
_LL345:;}outflow=s1_outflow;goto _LL318;}}}_LL339: if(*((int*)_tmp4CC)!= 11)goto
_LL33B;_tmp4F0=((struct Cyc_Absyn_Decl_s_struct*)_tmp4CC)->f1;_tmp4F1=((struct Cyc_Absyn_Decl_s_struct*)
_tmp4CC)->f2;_LL33A: outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp4F0),_tmp4F1);goto _LL318;_LL33B: if(*((int*)_tmp4CC)!= 12)goto
_LL33D;_tmp4F2=((struct Cyc_Absyn_Label_s_struct*)_tmp4CC)->f2;_LL33C: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4F2);goto _LL318;_LL33D: if(*((int*)_tmp4CC)!= 15)goto _LL33F;
_tmp4F3=((struct Cyc_Absyn_Region_s_struct*)_tmp4CC)->f1;_tmp4F4=((struct Cyc_Absyn_Region_s_struct*)
_tmp4CC)->f2;_tmp4F5=((struct Cyc_Absyn_Region_s_struct*)_tmp4CC)->f3;_tmp4F6=((
struct Cyc_Absyn_Region_s_struct*)_tmp4CC)->f4;_tmp4F7=((struct Cyc_Absyn_Region_s_struct*)
_tmp4CC)->f5;_LL33E: if(_tmp4F6 != 0){struct Cyc_Absyn_Exp*_tmp536=(struct Cyc_Absyn_Exp*)
_tmp4F6;union Cyc_CfFlowInfo_FlowInfo_union _tmp538;void*_tmp539;struct _tuple5
_tmp537=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp536);_tmp538=_tmp537.f1;
_tmp539=_tmp537.f2;_tmp538=Cyc_CfFlowInfo_consume_unique_rvals(_tmp536->loc,
_tmp538);inflow=Cyc_NewControlFlow_use_Rval(env,_tmp536->loc,_tmp538,_tmp539);}{
struct Cyc_List_List _tmp830;struct Cyc_List_List _tmp53A=(_tmp830.hd=_tmp4F4,((
_tmp830.tl=0,_tmp830)));inflow=Cyc_NewControlFlow_add_vars(_tmp4CB,inflow,(
struct Cyc_List_List*)& _tmp53A,(void*)_tmp4CB->unknown_all,_tmp4F7->loc);outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4F7);goto _LL318;}_LL33F: if(*((int*)
_tmp4CC)!= 16)goto _LL341;_tmp4F8=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp4CC)->f1;
_LL340: {union Cyc_CfFlowInfo_FlowInfo_union _tmp53D;void*_tmp53E;struct _tuple5
_tmp53C=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp4F8);_tmp53D=_tmp53C.f1;
_tmp53E=_tmp53C.f2;_tmp53D=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4F8->loc,
_tmp53D);{union Cyc_CfFlowInfo_FlowInfo_union _tmp53F=Cyc_NewControlFlow_use_Rval(
env,_tmp4F8->loc,_tmp53D,_tmp53E);{void*_tmp540=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4F8->topt))->v);void*_tmp541;_LL34B: if(
_tmp540 <= (void*)4)goto _LL34D;if(*((int*)_tmp540)!= 14)goto _LL34D;_tmp541=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp540)->f1;_LL34C: outflow=Cyc_CfFlowInfo_kill_flow_region(
_tmp4CB,_tmp53D,_tmp541);goto _LL34A;_LL34D:;_LL34E: {struct Cyc_Core_Impossible_struct
_tmp836;const char*_tmp835;struct Cyc_Core_Impossible_struct*_tmp834;(int)_throw((
void*)((_tmp834=_cycalloc(sizeof(*_tmp834)),((_tmp834[0]=((_tmp836.tag=Cyc_Core_Impossible,((
_tmp836.f1=((_tmp835="anal_stmt -- reset_region",_tag_dyneither(_tmp835,sizeof(
char),26))),_tmp836)))),_tmp834)))));}_LL34A:;}goto _LL318;}}_LL341: if(*((int*)
_tmp4CC)!= 17)goto _LL343;_tmp4F9=((struct Cyc_Absyn_Alias_s_struct*)_tmp4CC)->f1;
_tmp4FA=((struct Cyc_Absyn_Alias_s_struct*)_tmp4CC)->f2;_tmp4FB=((struct Cyc_Absyn_Alias_s_struct*)
_tmp4CC)->f3;_tmp4FC=((struct Cyc_Absyn_Alias_s_struct*)_tmp4CC)->f4;_LL342: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp546;void*_tmp547;struct _tuple5 _tmp545=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4F9);_tmp546=_tmp545.f1;_tmp547=_tmp545.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
_tmp549;struct Cyc_List_List*_tmp54A;struct _tuple6 _tmp548=Cyc_CfFlowInfo_save_consume_info(
_tmp4CB,_tmp546,0);_tmp549=_tmp548.f1;_tmp54A=_tmp549.may_consume;_tmp546=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4F9->loc,_tmp546);_tmp546=Cyc_NewControlFlow_use_Rval(env,_tmp4F9->loc,
_tmp546,_tmp547);{struct Cyc_List_List _tmp837;struct Cyc_List_List _tmp54B=(_tmp837.hd=
_tmp4FB,((_tmp837.tl=0,_tmp837)));_tmp546=Cyc_NewControlFlow_add_vars(_tmp4CB,
_tmp546,(struct Cyc_List_List*)& _tmp54B,(void*)_tmp4CB->unknown_all,s->loc);
outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp546,_tmp4FC);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp54C=outflow;struct Cyc_Dict_Dict _tmp54D;struct Cyc_List_List*_tmp54E;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp54F;_LL350: if((_tmp54C.BottomFL).tag != 0)goto _LL352;_LL351: goto _LL34F;_LL352:
if((_tmp54C.ReachableFL).tag != 1)goto _LL34F;_tmp54D=(_tmp54C.ReachableFL).f1;
_tmp54E=(_tmp54C.ReachableFL).f2;_tmp54F=(_tmp54C.ReachableFL).f3;_LL353: while(
_tmp54A != 0){struct Cyc_Dict_Dict _tmp550=_tmp54F.consumed;_tmp54F.consumed=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))
Cyc_Dict_rdelete)(_tmp4CB->r,_tmp54F.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp54A->hd);if((_tmp54F.consumed).t != _tmp550.t);_tmp54A=_tmp54A->tl;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp838;outflow=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp838.ReachableFL).tag=
1,(((_tmp838.ReachableFL).f1=_tmp54D,(((_tmp838.ReachableFL).f2=_tmp54E,(((
_tmp838.ReachableFL).f3=_tmp54F,_tmp838))))))));}goto _LL34F;_LL34F:;}goto _LL318;}}}
_LL343:;_LL344: {struct Cyc_Core_Impossible_struct _tmp83E;const char*_tmp83D;
struct Cyc_Core_Impossible_struct*_tmp83C;(int)_throw((void*)((_tmp83C=_cycalloc(
sizeof(*_tmp83C)),((_tmp83C[0]=((_tmp83E.tag=Cyc_Core_Impossible,((_tmp83E.f1=((
_tmp83D="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(
_tmp83D,sizeof(char),56))),_tmp83E)))),_tmp83C)))));}_LL318:;}outflow=Cyc_CfFlowInfo_drop_unique_rvals(
s->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp556=outflow;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp557;_LL355: if((_tmp556.ReachableFL).tag != 1)goto _LL357;_tmp557=(_tmp556.ReachableFL).f3;
_LL356: goto _LL354;_LL357:;_LL358: goto _LL354;_LL354:;}return outflow;}}static void
Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_Absyn_Fndecl*fd);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Decl*decl);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Decl*decl){void*_tmp558=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp559;struct Cyc_Core_Opt*_tmp55A;struct Cyc_Core_Opt _tmp55B;struct Cyc_List_List*
_tmp55C;struct Cyc_Absyn_Exp*_tmp55D;struct Cyc_List_List*_tmp55E;struct Cyc_Absyn_Fndecl*
_tmp55F;_LL35A: if(_tmp558 <= (void*)2)goto _LL362;if(*((int*)_tmp558)!= 0)goto
_LL35C;_tmp559=((struct Cyc_Absyn_Var_d_struct*)_tmp558)->f1;_LL35B: {struct Cyc_List_List
_tmp83F;struct Cyc_List_List _tmp560=(_tmp83F.hd=_tmp559,((_tmp83F.tl=0,_tmp83F)));
inflow=Cyc_NewControlFlow_add_vars(env->fenv,inflow,(struct Cyc_List_List*)&
_tmp560,(void*)(env->fenv)->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*_tmp561=
_tmp559->initializer;if(_tmp561 == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp563;void*_tmp564;struct _tuple5 _tmp562=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_tmp561);_tmp563=_tmp562.f1;_tmp564=_tmp562.f2;
_tmp563=Cyc_CfFlowInfo_consume_unique_rvals(_tmp561->loc,_tmp563);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp565=_tmp563;struct Cyc_Dict_Dict _tmp566;struct Cyc_List_List*_tmp567;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp568;_LL365: if((_tmp565.BottomFL).tag != 0)goto _LL367;_LL366: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp840;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp840.BottomFL).tag=0,
_tmp840));}_LL367: if((_tmp565.ReachableFL).tag != 1)goto _LL364;_tmp566=(_tmp565.ReachableFL).f1;
_tmp567=(_tmp565.ReachableFL).f2;_tmp568=(_tmp565.ReachableFL).f3;_LL368:{struct
Cyc_CfFlowInfo_VarRoot_struct*_tmp846;struct Cyc_CfFlowInfo_VarRoot_struct _tmp845;
struct Cyc_CfFlowInfo_Place*_tmp844;_tmp566=Cyc_CfFlowInfo_assign_place(env->fenv,
decl->loc,_tmp566,env->all_changed,((_tmp844=_region_malloc(env->r,sizeof(*
_tmp844)),((_tmp844->root=(void*)((void*)((_tmp846=_region_malloc(env->r,sizeof(*
_tmp846)),((_tmp846[0]=((_tmp845.tag=0,((_tmp845.f1=_tmp559,_tmp845)))),_tmp846))))),((
_tmp844->fields=0,_tmp844)))))),_tmp564);}_tmp567=Cyc_CfFlowInfo_reln_assign_var(
env->r,_tmp567,_tmp559,(struct Cyc_Absyn_Exp*)_check_null(_tmp559->initializer));{
union Cyc_CfFlowInfo_FlowInfo_union _tmp847;union Cyc_CfFlowInfo_FlowInfo_union
_tmp56D=((_tmp847.ReachableFL).tag=1,(((_tmp847.ReachableFL).f1=_tmp566,(((
_tmp847.ReachableFL).f2=_tmp567,(((_tmp847.ReachableFL).f3=_tmp568,_tmp847)))))));
Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp56D);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp56D;}_LL364:;}}}}_LL35C:
if(*((int*)_tmp558)!= 2)goto _LL35E;_tmp55A=((struct Cyc_Absyn_Let_d_struct*)
_tmp558)->f2;if(_tmp55A == 0)goto _LL35E;_tmp55B=*_tmp55A;_tmp55C=(struct Cyc_List_List*)
_tmp55B.v;_tmp55D=((struct Cyc_Absyn_Let_d_struct*)_tmp558)->f3;_LL35D: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp571;void*_tmp572;struct _tuple5 _tmp570=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp55D);_tmp571=_tmp570.f1;_tmp572=_tmp570.f2;_tmp571=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp55D->loc,_tmp571);_tmp571=Cyc_NewControlFlow_use_Rval(env,_tmp55D->loc,
_tmp571,_tmp572);return Cyc_NewControlFlow_add_vars(env->fenv,_tmp571,_tmp55C,(
void*)(env->fenv)->unknown_all,decl->loc);}_LL35E: if(*((int*)_tmp558)!= 3)goto
_LL360;_tmp55E=((struct Cyc_Absyn_Letv_d_struct*)_tmp558)->f1;_LL35F: return Cyc_NewControlFlow_add_vars(
env->fenv,inflow,_tmp55E,(void*)(env->fenv)->unknown_none,decl->loc);_LL360: if(*((
int*)_tmp558)!= 1)goto _LL362;_tmp55F=((struct Cyc_Absyn_Fn_d_struct*)_tmp558)->f1;
_LL361: Cyc_NewControlFlow_check_nested_fun(env->fenv,inflow,_tmp55F);{void*
_tmp573=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp55F->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp574=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp55F->fn_vardecl);
struct Cyc_List_List*_tmp848;return Cyc_NewControlFlow_add_vars(env->fenv,inflow,((
_tmp848=_region_malloc(env->r,sizeof(*_tmp848)),((_tmp848->hd=_tmp574,((_tmp848->tl=
0,_tmp848)))))),(void*)(env->fenv)->unknown_all,decl->loc);}_LL362:;_LL363: {
struct Cyc_Core_Impossible_struct _tmp84E;const char*_tmp84D;struct Cyc_Core_Impossible_struct*
_tmp84C;(int)_throw((void*)((_tmp84C=_cycalloc(sizeof(*_tmp84C)),((_tmp84C[0]=((
_tmp84E.tag=Cyc_Core_Impossible,((_tmp84E.f1=((_tmp84D="anal_decl: unexpected decl variant",
_tag_dyneither(_tmp84D,sizeof(char),35))),_tmp84E)))),_tmp84C)))));}_LL359:;}
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd);static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd){struct _RegionHandle _tmp579=_new_region("frgn");struct
_RegionHandle*frgn=& _tmp579;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*
fenv=Cyc_CfFlowInfo_new_flow_env(frgn);struct Cyc_CfFlowInfo_ConsumeInfo _tmp851;
union Cyc_CfFlowInfo_FlowInfo_union _tmp850;Cyc_NewControlFlow_check_nested_fun(
fenv,(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp850.ReachableFL).tag=1,(((
_tmp850.ReachableFL).f1=fenv->mt_flowdict,(((_tmp850.ReachableFL).f2=0,(((
_tmp850.ReachableFL).f3=((_tmp851.consumed=fenv->mt_place_set,((_tmp851.may_consume=
0,_tmp851)))),_tmp850)))))))),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(
void*s);static int Cyc_NewControlFlow_hash_ptr(void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Fndecl*fd);static void Cyc_NewControlFlow_check_nested_fun(struct
Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle*_tmp57C=fenv->r;struct Cyc_Position_Segment*_tmp57D=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(void*)fenv->unknown_all,_tmp57D);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo_union _tmp57E=inflow;struct Cyc_Dict_Dict
_tmp57F;struct Cyc_List_List*_tmp580;struct Cyc_CfFlowInfo_ConsumeInfo _tmp581;
_LL36A: if((_tmp57E.BottomFL).tag != 0)goto _LL36C;_LL36B: {const char*_tmp854;void*
_tmp853;(_tmp853=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp854="check_fun",_tag_dyneither(_tmp854,sizeof(char),10))),_tag_dyneither(
_tmp853,sizeof(void*),0)));}_LL36C: if((_tmp57E.ReachableFL).tag != 1)goto _LL369;
_tmp57F=(_tmp57E.ReachableFL).f1;_tmp580=(_tmp57E.ReachableFL).f2;_tmp581=(
_tmp57E.ReachableFL).f3;_LL36D: {struct Cyc_List_List*atts;{void*_tmp584=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo
_tmp585;struct Cyc_List_List*_tmp586;_LL36F: if(_tmp584 <= (void*)4)goto _LL371;if(*((
int*)_tmp584)!= 8)goto _LL371;_tmp585=((struct Cyc_Absyn_FnType_struct*)_tmp584)->f1;
_tmp586=_tmp585.attributes;_LL370: atts=_tmp586;goto _LL36E;_LL371:;_LL372: {const
char*_tmp857;void*_tmp856;(_tmp856=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp857="check_fun: non-function type cached with fndecl_t",
_tag_dyneither(_tmp857,sizeof(char),50))),_tag_dyneither(_tmp856,sizeof(void*),0)));}
_LL36E:;}for(0;atts != 0;atts=atts->tl){void*_tmp589=(void*)atts->hd;int _tmp58A;
_LL374: if(_tmp589 <= (void*)17)goto _LL376;if(*((int*)_tmp589)!= 4)goto _LL376;
_tmp58A=((struct Cyc_Absyn_Initializes_att_struct*)_tmp589)->f1;_LL375: {
unsigned int j=(unsigned int)_tmp58A;if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){{
const char*_tmp85A;void*_tmp859;(_tmp859=0,Cyc_Tcutil_terr(_tmp57D,((_tmp85A="initializes attribute exceeds number of parameters",
_tag_dyneither(_tmp85A,sizeof(char),51))),_tag_dyneither(_tmp859,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp58D=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp58E=Cyc_Tcutil_compress((
void*)_tmp58D->type);struct Cyc_Absyn_PtrInfo _tmp58F;void*_tmp590;struct Cyc_Absyn_PtrAtts
_tmp591;struct Cyc_Absyn_Conref*_tmp592;struct Cyc_Absyn_Conref*_tmp593;struct Cyc_Absyn_Conref*
_tmp594;_LL379: if(_tmp58E <= (void*)4)goto _LL37B;if(*((int*)_tmp58E)!= 4)goto
_LL37B;_tmp58F=((struct Cyc_Absyn_PointerType_struct*)_tmp58E)->f1;_tmp590=(void*)
_tmp58F.elt_typ;_tmp591=_tmp58F.ptr_atts;_tmp592=_tmp591.nullable;_tmp593=
_tmp591.bounds;_tmp594=_tmp591.zero_term;_LL37A: if(((int(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_val)(_tmp592)){const char*_tmp85D;void*_tmp85C;(_tmp85C=0,Cyc_Tcutil_terr(
_tmp57D,((_tmp85D="initializes attribute not allowed on nullable pointers",
_tag_dyneither(_tmp85D,sizeof(char),55))),_tag_dyneither(_tmp85C,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_bound_one(_tmp593)){const char*_tmp860;void*_tmp85F;(_tmp85F=0,
Cyc_Tcutil_terr(_tmp57D,((_tmp860="initializes attribute allowed only on pointers of size 1",
_tag_dyneither(_tmp860,sizeof(char),57))),_tag_dyneither(_tmp85F,sizeof(void*),0)));}
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp594)){
const char*_tmp863;void*_tmp862;(_tmp862=0,Cyc_Tcutil_terr(_tmp57D,((_tmp863="initializes attribute allowed only on pointers to non-zero-terminated arrays",
_tag_dyneither(_tmp863,sizeof(char),77))),_tag_dyneither(_tmp862,sizeof(void*),0)));}{
struct Cyc_CfFlowInfo_InitParam_struct _tmp866;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp865;struct Cyc_CfFlowInfo_InitParam_struct*_tmp59B=(_tmp865=_region_malloc(
_tmp57C,sizeof(*_tmp865)),((_tmp865[0]=((_tmp866.tag=2,((_tmp866.f1=(int)j,((
_tmp866.f2=(void*)_tmp590,_tmp866)))))),_tmp865)));struct Cyc_CfFlowInfo_Place*
_tmp867;struct Cyc_CfFlowInfo_Place*_tmp59C=(_tmp867=_region_malloc(_tmp57C,
sizeof(*_tmp867)),((_tmp867->root=(void*)((void*)_tmp59B),((_tmp867->fields=0,
_tmp867)))));_tmp57F=Cyc_Dict_insert(_tmp57F,(void*)_tmp59B,Cyc_CfFlowInfo_typ_to_absrval(
fenv,_tmp590,(void*)fenv->esc_none));{struct Cyc_CfFlowInfo_AddressOf_struct
_tmp86D;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp86C;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp86A;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp869;_tmp57F=Cyc_Dict_insert(
_tmp57F,(void*)((_tmp869=_region_malloc(_tmp57C,sizeof(*_tmp869)),((_tmp869[0]=((
_tmp86A.tag=0,((_tmp86A.f1=_tmp58D,_tmp86A)))),_tmp869)))),(void*)((_tmp86C=
_region_malloc(_tmp57C,sizeof(*_tmp86C)),((_tmp86C[0]=((_tmp86D.tag=2,((_tmp86D.f1=
_tmp59C,_tmp86D)))),_tmp86C)))));}{struct Cyc_List_List*_tmp86E;param_roots=((
_tmp86E=_region_malloc(_tmp57C,sizeof(*_tmp86E)),((_tmp86E->hd=_tmp59C,((_tmp86E->tl=
param_roots,_tmp86E))))));}goto _LL378;}_LL37B:;_LL37C: {const char*_tmp871;void*
_tmp870;(_tmp870=0,Cyc_Tcutil_terr(_tmp57D,((_tmp871="initializes attribute on non-pointer",
_tag_dyneither(_tmp871,sizeof(char),37))),_tag_dyneither(_tmp870,sizeof(void*),0)));}
_LL378:;}goto _LL373;}}_LL376:;_LL377: goto _LL373;_LL373:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp872;inflow=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp872.ReachableFL).tag=1,(((
_tmp872.ReachableFL).f1=_tmp57F,(((_tmp872.ReachableFL).f2=_tmp580,(((_tmp872.ReachableFL).f3=
_tmp581,_tmp872))))))));}}_LL369:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp57C,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*_tmp873;struct
Cyc_NewControlFlow_AnalEnv*env=(_tmp873=_region_malloc(_tmp57C,sizeof(*_tmp873)),((
_tmp873->r=_tmp57C,((_tmp873->fenv=fenv,((_tmp873->iterate_again=1,((_tmp873->iteration_num=
0,((_tmp873->in_try=0,((_tmp873->tryflow=inflow,((_tmp873->all_changed=0,((
_tmp873->noreturn=noreturn,((_tmp873->param_roots=param_roots,((_tmp873->flow_table=
flow_table,_tmp873)))))))))))))))))))));union Cyc_CfFlowInfo_FlowInfo_union
outflow=inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5A8=outflow;_LL37E: if((_tmp5A8.BottomFL).tag != 0)goto _LL380;_LL37F: goto
_LL37D;_LL380:;_LL381: Cyc_NewControlFlow_check_init_params(_tmp57D,env,outflow);
if(noreturn){const char*_tmp876;void*_tmp875;(_tmp875=0,Cyc_Tcutil_terr(_tmp57D,((
_tmp876="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp876,
sizeof(char),46))),_tag_dyneither(_tmp875,sizeof(void*),0)));}else{void*_tmp5AB=
Cyc_Tcutil_compress((void*)fd->ret_type);_LL383: if((int)_tmp5AB != 0)goto _LL385;
_LL384: goto _LL382;_LL385: if(_tmp5AB <= (void*)4)goto _LL387;if(*((int*)_tmp5AB)!= 
6)goto _LL387;_LL386: goto _LL388;_LL387: if((int)_tmp5AB != 1)goto _LL389;_LL388: goto
_LL38A;_LL389: if(_tmp5AB <= (void*)4)goto _LL38B;if(*((int*)_tmp5AB)!= 5)goto
_LL38B;_LL38A:{const char*_tmp879;void*_tmp878;(_tmp878=0,Cyc_Tcutil_warn(_tmp57D,((
_tmp879="function may not return a value",_tag_dyneither(_tmp879,sizeof(char),32))),
_tag_dyneither(_tmp878,sizeof(void*),0)));}goto _LL382;_LL38B:;_LL38C:{const char*
_tmp87C;void*_tmp87B;(_tmp87B=0,Cyc_Tcutil_terr(_tmp57D,((_tmp87C="function may not return a value",
_tag_dyneither(_tmp87C,sizeof(char),32))),_tag_dyneither(_tmp87B,sizeof(void*),0)));}
goto _LL382;_LL382:;}goto _LL37D;_LL37D:;}}}}void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds);void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){void*_tmp5B1=(void*)((struct Cyc_Absyn_Decl*)ds->hd)->r;
struct Cyc_Absyn_Fndecl*_tmp5B2;struct Cyc_List_List*_tmp5B3;struct Cyc_List_List*
_tmp5B4;struct Cyc_List_List*_tmp5B5;_LL38E: if(_tmp5B1 <= (void*)2)goto _LL398;if(*((
int*)_tmp5B1)!= 1)goto _LL390;_tmp5B2=((struct Cyc_Absyn_Fn_d_struct*)_tmp5B1)->f1;
_LL38F: Cyc_NewControlFlow_check_fun(_tmp5B2);goto _LL38D;_LL390: if(*((int*)
_tmp5B1)!= 10)goto _LL392;_tmp5B3=((struct Cyc_Absyn_ExternC_d_struct*)_tmp5B1)->f1;
_LL391: _tmp5B4=_tmp5B3;goto _LL393;_LL392: if(*((int*)_tmp5B1)!= 9)goto _LL394;
_tmp5B4=((struct Cyc_Absyn_Using_d_struct*)_tmp5B1)->f2;_LL393: _tmp5B5=_tmp5B4;
goto _LL395;_LL394: if(*((int*)_tmp5B1)!= 8)goto _LL396;_tmp5B5=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp5B1)->f2;_LL395: Cyc_NewControlFlow_cf_check(_tmp5B5);goto _LL38D;_LL396: if(*((
int*)_tmp5B1)!= 11)goto _LL398;_LL397: goto _LL38D;_LL398:;_LL399: goto _LL38D;_LL38D:;}}
