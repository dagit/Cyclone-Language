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
void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_dyneither_ptr);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter_c(void(*f)(void*,void*),
void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*
x);extern char Cyc_List_Nth[8];int Cyc_strcmp(struct _dyneither_ptr s1,struct
_dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*
s2);struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_List_List*
Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Error{
struct _dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];extern int Cyc_Position_use_gcc_style_location;
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
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
Cyc_Absyn_UnknownTunionInfo{struct _tuple0*name;int is_xtunion;int is_flat;};struct
Cyc_Absyn_UnknownTunion_struct{int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};
struct Cyc_Absyn_KnownTunion_struct{int tag;struct Cyc_Absyn_Tuniondecl**f1;};union
Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct UnknownTunion;
struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{union
Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*tunion_name;struct
_tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int
tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};union Cyc_Absyn_TunionFieldInfoU_union{
struct Cyc_Absyn_UnknownTunionfield_struct UnknownTunionfield;struct Cyc_Absyn_KnownTunionfield_struct
KnownTunionfield;};struct Cyc_Absyn_TunionFieldInfo{union Cyc_Absyn_TunionFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
_dyneither_ptr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Subscript_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Struct_e_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tunion_e_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*
f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;
};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};
struct _tuple2{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple0*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple0*,struct _tuple0*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*
Cyc_Absyn_sint_typ;void*Cyc_Absyn_string_typ(void*rgn);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);struct
Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
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
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dyneither_ptr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(
void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
struct _tuple3{void*f1;void*f2;};struct _tuple3*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple3*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
_dyneither_ptr ap);extern char Cyc_Tcdecl_Incompatible[17];struct Cyc_Tcdecl_Xtunionfielddecl{
struct Cyc_Absyn_Tuniondecl*base;struct Cyc_Absyn_Tunionfield*field;};struct Cyc_Tcgenrep_RepInfo;
struct Cyc_Position_Segment{int start;int end;};struct Cyc_Port_Edit{struct Cyc_Position_Segment*
loc;struct _dyneither_ptr old_string;struct _dyneither_ptr new_string;};int Cyc_Port_cmp_edit(
struct Cyc_Port_Edit*e1,struct Cyc_Port_Edit*e2);int Cyc_Port_cmp_edit(struct Cyc_Port_Edit*
e1,struct Cyc_Port_Edit*e2){if((unsigned int)e1->loc  && (unsigned int)e2->loc)
return((struct Cyc_Position_Segment*)_check_null(e1->loc))->start - ((struct Cyc_Position_Segment*)
_check_null(e2->loc))->start;else{if(e1->loc == e2->loc)return 0;else{if((
unsigned int)e1->loc)return 1;else{return - 1;}}}}struct Cyc_Port_Cvar{int id;void**
eq;struct Cyc_List_List*uppers;struct Cyc_List_List*lowers;};struct Cyc_Port_Cfield{
void*qual;struct _dyneither_ptr*f;void*type;};struct Cyc_Port_RgnVar_ct_struct{int
tag;struct _dyneither_ptr*f1;};struct Cyc_Port_Ptr_ct_struct{int tag;void*f1;void*
f2;void*f3;void*f4;void*f5;};struct Cyc_Port_Array_ct_struct{int tag;void*f1;void*
f2;void*f3;};struct _tuple4{struct Cyc_Absyn_Aggrdecl*f1;struct Cyc_List_List*f2;};
struct Cyc_Port_KnownAggr_ct_struct{int tag;struct _tuple4*f1;};struct Cyc_Port_UnknownAggr_ct_struct{
int tag;struct Cyc_List_List*f1;void**f2;};struct Cyc_Port_Fn_ct_struct{int tag;void*
f1;struct Cyc_List_List*f2;};struct Cyc_Port_Var_ct_struct{int tag;struct Cyc_Port_Cvar*
f1;};static struct _dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*
ts);static struct _dyneither_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*
ts);static struct _dyneither_ptr Cyc_Port_ctype2string(int deep,void*t);static struct
_dyneither_ptr Cyc_Port_ctype2string(int deep,void*t){void*_tmp0=t;struct
_dyneither_ptr*_tmp1;void*_tmp2;void*_tmp3;void*_tmp4;void*_tmp5;void*_tmp6;void*
_tmp7;void*_tmp8;void*_tmp9;struct _tuple4*_tmpA;struct _tuple4 _tmpB;struct Cyc_Absyn_Aggrdecl*
_tmpC;struct Cyc_List_List*_tmpD;struct Cyc_List_List*_tmpE;void**_tmpF;void*
_tmp10;struct Cyc_List_List*_tmp11;void*_tmp12;struct Cyc_List_List*_tmp13;struct
Cyc_Port_Cvar*_tmp14;_LL1: if((int)_tmp0 != 0)goto _LL3;_LL2: {const char*_tmp3F3;
return(_tmp3F3="const",_tag_dyneither(_tmp3F3,sizeof(char),6));}_LL3: if((int)
_tmp0 != 1)goto _LL5;_LL4: {const char*_tmp3F4;return(_tmp3F4="notconst",
_tag_dyneither(_tmp3F4,sizeof(char),9));}_LL5: if((int)_tmp0 != 2)goto _LL7;_LL6: {
const char*_tmp3F5;return(_tmp3F5="thin",_tag_dyneither(_tmp3F5,sizeof(char),5));}
_LL7: if((int)_tmp0 != 3)goto _LL9;_LL8: {const char*_tmp3F6;return(_tmp3F6="fat",
_tag_dyneither(_tmp3F6,sizeof(char),4));}_LL9: if((int)_tmp0 != 4)goto _LLB;_LLA: {
const char*_tmp3F7;return(_tmp3F7="void",_tag_dyneither(_tmp3F7,sizeof(char),5));}
_LLB: if((int)_tmp0 != 5)goto _LLD;_LLC: {const char*_tmp3F8;return(_tmp3F8="zero",
_tag_dyneither(_tmp3F8,sizeof(char),5));}_LLD: if((int)_tmp0 != 6)goto _LLF;_LLE: {
const char*_tmp3F9;return(_tmp3F9="arith",_tag_dyneither(_tmp3F9,sizeof(char),6));}
_LLF: if((int)_tmp0 != 7)goto _LL11;_LL10: {const char*_tmp3FA;return(_tmp3FA="heap",
_tag_dyneither(_tmp3FA,sizeof(char),5));}_LL11: if((int)_tmp0 != 8)goto _LL13;_LL12: {
const char*_tmp3FB;return(_tmp3FB="ZT",_tag_dyneither(_tmp3FB,sizeof(char),3));}
_LL13: if((int)_tmp0 != 9)goto _LL15;_LL14: {const char*_tmp3FC;return(_tmp3FC="NZT",
_tag_dyneither(_tmp3FC,sizeof(char),4));}_LL15: if(_tmp0 <= (void*)10)goto _LL17;
if(*((int*)_tmp0)!= 0)goto _LL17;_tmp1=((struct Cyc_Port_RgnVar_ct_struct*)_tmp0)->f1;
_LL16: {const char*_tmp400;void*_tmp3FF[1];struct Cyc_String_pa_struct _tmp3FE;
return(struct _dyneither_ptr)((_tmp3FE.tag=0,((_tmp3FE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp1),((_tmp3FF[0]=& _tmp3FE,Cyc_aprintf(((_tmp400="%s",
_tag_dyneither(_tmp400,sizeof(char),3))),_tag_dyneither(_tmp3FF,sizeof(void*),1))))))));}
_LL17: if(_tmp0 <= (void*)10)goto _LL19;if(*((int*)_tmp0)!= 1)goto _LL19;_tmp2=(void*)((
struct Cyc_Port_Ptr_ct_struct*)_tmp0)->f1;_tmp3=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp0)->f2;_tmp4=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp0)->f3;_tmp5=(void*)((
struct Cyc_Port_Ptr_ct_struct*)_tmp0)->f4;_tmp6=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp0)->f5;_LL18: {const char*_tmp408;void*_tmp407[5];struct Cyc_String_pa_struct
_tmp406;struct Cyc_String_pa_struct _tmp405;struct Cyc_String_pa_struct _tmp404;
struct Cyc_String_pa_struct _tmp403;struct Cyc_String_pa_struct _tmp402;return(
struct _dyneither_ptr)((_tmp402.tag=0,((_tmp402.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp6)),((_tmp403.tag=0,((_tmp403.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp5)),((
_tmp404.tag=0,((_tmp404.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(
deep,_tmp4)),((_tmp405.tag=0,((_tmp405.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp3)),((_tmp406.tag=0,((_tmp406.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp2)),((
_tmp407[0]=& _tmp406,((_tmp407[1]=& _tmp405,((_tmp407[2]=& _tmp404,((_tmp407[3]=&
_tmp403,((_tmp407[4]=& _tmp402,Cyc_aprintf(((_tmp408="ptr(%s,%s,%s,%s,%s)",
_tag_dyneither(_tmp408,sizeof(char),20))),_tag_dyneither(_tmp407,sizeof(void*),5))))))))))))))))))))))))))))))));}
_LL19: if(_tmp0 <= (void*)10)goto _LL1B;if(*((int*)_tmp0)!= 2)goto _LL1B;_tmp7=(void*)((
struct Cyc_Port_Array_ct_struct*)_tmp0)->f1;_tmp8=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmp0)->f2;_tmp9=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp0)->f3;_LL1A: {
const char*_tmp40E;void*_tmp40D[3];struct Cyc_String_pa_struct _tmp40C;struct Cyc_String_pa_struct
_tmp40B;struct Cyc_String_pa_struct _tmp40A;return(struct _dyneither_ptr)((_tmp40A.tag=
0,((_tmp40A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(
deep,_tmp9)),((_tmp40B.tag=0,((_tmp40B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp8)),((_tmp40C.tag=0,((_tmp40C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp7)),((
_tmp40D[0]=& _tmp40C,((_tmp40D[1]=& _tmp40B,((_tmp40D[2]=& _tmp40A,Cyc_aprintf(((
_tmp40E="array(%s,%s,%s)",_tag_dyneither(_tmp40E,sizeof(char),16))),
_tag_dyneither(_tmp40D,sizeof(void*),3))))))))))))))))))));}_LL1B: if(_tmp0 <= (
void*)10)goto _LL1D;if(*((int*)_tmp0)!= 3)goto _LL1D;_tmpA=((struct Cyc_Port_KnownAggr_ct_struct*)
_tmp0)->f1;_tmpB=*_tmpA;_tmpC=_tmpB.f1;_tmpD=_tmpB.f2;_LL1C: {const char*_tmp410;
const char*_tmp40F;struct _dyneither_ptr s=(void*)_tmpC->kind == (void*)0?(_tmp410="struct",
_tag_dyneither(_tmp410,sizeof(char),7)):((_tmp40F="union",_tag_dyneither(_tmp40F,
sizeof(char),6)));if(!deep){const char*_tmp415;void*_tmp414[2];struct Cyc_String_pa_struct
_tmp413;struct Cyc_String_pa_struct _tmp412;return(struct _dyneither_ptr)((_tmp412.tag=
0,((_tmp412.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpC->name)),((_tmp413.tag=0,((_tmp413.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp414[0]=& _tmp413,((_tmp414[1]=& _tmp412,Cyc_aprintf(((
_tmp415="%s %s",_tag_dyneither(_tmp415,sizeof(char),6))),_tag_dyneither(_tmp414,
sizeof(void*),2))))))))))))));}else{const char*_tmp41B;void*_tmp41A[3];struct Cyc_String_pa_struct
_tmp419;struct Cyc_String_pa_struct _tmp418;struct Cyc_String_pa_struct _tmp417;
return(struct _dyneither_ptr)((_tmp417.tag=0,((_tmp417.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Port_cfields2string(0,_tmpD)),((_tmp418.tag=0,((_tmp418.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC->name)),((
_tmp419.tag=0,((_tmp419.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp41A[0]=& _tmp419,((_tmp41A[1]=& _tmp418,((_tmp41A[2]=& _tmp417,Cyc_aprintf(((
_tmp41B="%s %s {%s}",_tag_dyneither(_tmp41B,sizeof(char),11))),_tag_dyneither(
_tmp41A,sizeof(void*),3))))))))))))))))))));}}_LL1D: if(_tmp0 <= (void*)10)goto
_LL1F;if(*((int*)_tmp0)!= 4)goto _LL1F;_tmpE=((struct Cyc_Port_UnknownAggr_ct_struct*)
_tmp0)->f1;_tmpF=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmp0)->f2;if(_tmpF == 0)
goto _LL1F;_tmp10=*_tmpF;_LL1E: return Cyc_Port_ctype2string(deep,_tmp10);_LL1F: if(
_tmp0 <= (void*)10)goto _LL21;if(*((int*)_tmp0)!= 4)goto _LL21;_tmp11=((struct Cyc_Port_UnknownAggr_ct_struct*)
_tmp0)->f1;_LL20: {const char*_tmp41F;void*_tmp41E[1];struct Cyc_String_pa_struct
_tmp41D;return(struct _dyneither_ptr)((_tmp41D.tag=0,((_tmp41D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_cfields2string(deep,_tmp11)),((
_tmp41E[0]=& _tmp41D,Cyc_aprintf(((_tmp41F="aggr {%s}",_tag_dyneither(_tmp41F,
sizeof(char),10))),_tag_dyneither(_tmp41E,sizeof(void*),1))))))));}_LL21: if(
_tmp0 <= (void*)10)goto _LL23;if(*((int*)_tmp0)!= 5)goto _LL23;_tmp12=(void*)((
struct Cyc_Port_Fn_ct_struct*)_tmp0)->f1;_tmp13=((struct Cyc_Port_Fn_ct_struct*)
_tmp0)->f2;_LL22: {const char*_tmp424;void*_tmp423[2];struct Cyc_String_pa_struct
_tmp422;struct Cyc_String_pa_struct _tmp421;return(struct _dyneither_ptr)((_tmp421.tag=
0,((_tmp421.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(
deep,_tmp12)),((_tmp422.tag=0,((_tmp422.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Port_ctypes2string(deep,_tmp13)),((_tmp423[0]=& _tmp422,((
_tmp423[1]=& _tmp421,Cyc_aprintf(((_tmp424="fn(%s)->%s",_tag_dyneither(_tmp424,
sizeof(char),11))),_tag_dyneither(_tmp423,sizeof(void*),2))))))))))))));}_LL23:
if(_tmp0 <= (void*)10)goto _LL0;if(*((int*)_tmp0)!= 6)goto _LL0;_tmp14=((struct Cyc_Port_Var_ct_struct*)
_tmp0)->f1;_LL24: if((unsigned int)_tmp14->eq)return Cyc_Port_ctype2string(deep,*((
void**)_check_null(_tmp14->eq)));else{if(!deep  || _tmp14->uppers == 0  && _tmp14->lowers
== 0){const char*_tmp428;void*_tmp427[1];struct Cyc_Int_pa_struct _tmp426;return(
struct _dyneither_ptr)((_tmp426.tag=1,((_tmp426.f1=(unsigned long)_tmp14->id,((
_tmp427[0]=& _tmp426,Cyc_aprintf(((_tmp428="var(%d)",_tag_dyneither(_tmp428,
sizeof(char),8))),_tag_dyneither(_tmp427,sizeof(void*),1))))))));}else{const char*
_tmp42E;void*_tmp42D[3];struct Cyc_String_pa_struct _tmp42C;struct Cyc_Int_pa_struct
_tmp42B;struct Cyc_String_pa_struct _tmp42A;return(struct _dyneither_ptr)((_tmp42A.tag=
0,((_tmp42A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctypes2string(
0,_tmp14->uppers)),((_tmp42B.tag=1,((_tmp42B.f1=(unsigned long)_tmp14->id,((
_tmp42C.tag=0,((_tmp42C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctypes2string(
0,_tmp14->lowers)),((_tmp42D[0]=& _tmp42C,((_tmp42D[1]=& _tmp42B,((_tmp42D[2]=&
_tmp42A,Cyc_aprintf(((_tmp42E="var([%s]<=%d<=[%s])",_tag_dyneither(_tmp42E,
sizeof(char),20))),_tag_dyneither(_tmp42D,sizeof(void*),3))))))))))))))))))));}}
_LL0:;}static struct _dyneither_ptr*Cyc_Port_ctype2stringptr(int deep,void*t);
static struct _dyneither_ptr*Cyc_Port_ctype2stringptr(int deep,void*t){struct
_dyneither_ptr*_tmp42F;return(_tmp42F=_cycalloc(sizeof(*_tmp42F)),((_tmp42F[0]=
Cyc_Port_ctype2string(deep,t),_tmp42F)));}struct Cyc_List_List*Cyc_Port_sep(
struct _dyneither_ptr s,struct Cyc_List_List*xs);struct Cyc_List_List*Cyc_Port_sep(
struct _dyneither_ptr s,struct Cyc_List_List*xs){struct _dyneither_ptr*_tmp430;
struct _dyneither_ptr*_tmp49=(_tmp430=_cycalloc(sizeof(*_tmp430)),((_tmp430[0]=s,
_tmp430)));if(xs == 0)return xs;{struct Cyc_List_List*_tmp4A=xs;struct Cyc_List_List*
_tmp4B=xs->tl;for(0;_tmp4B != 0;(_tmp4A=_tmp4B,_tmp4B=_tmp4B->tl)){struct Cyc_List_List*
_tmp431;((struct Cyc_List_List*)_check_null(_tmp4A))->tl=((_tmp431=_cycalloc(
sizeof(*_tmp431)),((_tmp431->hd=_tmp49,((_tmp431->tl=_tmp4B,_tmp431))))));}
return xs;}}static struct _dyneither_ptr*Cyc_Port_cfield2stringptr(int deep,struct
Cyc_Port_Cfield*f);static struct _dyneither_ptr*Cyc_Port_cfield2stringptr(int deep,
struct Cyc_Port_Cfield*f){const char*_tmp437;void*_tmp436[3];struct Cyc_String_pa_struct
_tmp435;struct Cyc_String_pa_struct _tmp434;struct Cyc_String_pa_struct _tmp433;
struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp433.tag=0,((_tmp433.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,(void*)f->type)),((
_tmp434.tag=0,((_tmp434.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f->f),((
_tmp435.tag=0,((_tmp435.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(
deep,(void*)f->qual)),((_tmp436[0]=& _tmp435,((_tmp436[1]=& _tmp434,((_tmp436[2]=&
_tmp433,Cyc_aprintf(((_tmp437="%s %s: %s",_tag_dyneither(_tmp437,sizeof(char),10))),
_tag_dyneither(_tmp436,sizeof(void*),3))))))))))))))))))));struct _dyneither_ptr*
_tmp438;return(_tmp438=_cycalloc(sizeof(*_tmp438)),((_tmp438[0]=s,_tmp438)));}
static struct _dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts);
static struct _dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts){
const char*_tmp439;return(struct _dyneither_ptr)Cyc_strconcat_l(Cyc_Port_sep(((
_tmp439=",",_tag_dyneither(_tmp439,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Port_ctype2stringptr,deep,ts)));}static struct _dyneither_ptr Cyc_Port_cfields2string(
int deep,struct Cyc_List_List*fs);static struct _dyneither_ptr Cyc_Port_cfields2string(
int deep,struct Cyc_List_List*fs){const char*_tmp43A;return(struct _dyneither_ptr)
Cyc_strconcat_l(Cyc_Port_sep(((_tmp43A=";",_tag_dyneither(_tmp43A,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(int,struct Cyc_Port_Cfield*),
int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_cfield2stringptr,deep,fs)));}
static void*Cyc_Port_notconst_ct();static void*Cyc_Port_notconst_ct(){return(void*)
1;}static void*Cyc_Port_const_ct();static void*Cyc_Port_const_ct(){return(void*)0;}
static void*Cyc_Port_thin_ct();static void*Cyc_Port_thin_ct(){return(void*)2;}
static void*Cyc_Port_fat_ct();static void*Cyc_Port_fat_ct(){return(void*)3;}static
void*Cyc_Port_void_ct();static void*Cyc_Port_void_ct(){return(void*)4;}static void*
Cyc_Port_zero_ct();static void*Cyc_Port_zero_ct(){return(void*)5;}static void*Cyc_Port_arith_ct();
static void*Cyc_Port_arith_ct(){return(void*)6;}static void*Cyc_Port_heap_ct();
static void*Cyc_Port_heap_ct(){return(void*)7;}static void*Cyc_Port_zterm_ct();
static void*Cyc_Port_zterm_ct(){return(void*)8;}static void*Cyc_Port_nozterm_ct();
static void*Cyc_Port_nozterm_ct(){return(void*)9;}static void*Cyc_Port_rgnvar_ct(
struct _dyneither_ptr*n);static void*Cyc_Port_rgnvar_ct(struct _dyneither_ptr*n){
struct Cyc_Port_RgnVar_ct_struct _tmp43D;struct Cyc_Port_RgnVar_ct_struct*_tmp43C;
return(void*)((_tmp43C=_cycalloc(sizeof(*_tmp43C)),((_tmp43C[0]=((_tmp43D.tag=0,((
_tmp43D.f1=n,_tmp43D)))),_tmp43C))));}static void*Cyc_Port_unknown_aggr_ct(struct
Cyc_List_List*fs);static void*Cyc_Port_unknown_aggr_ct(struct Cyc_List_List*fs){
struct Cyc_Port_UnknownAggr_ct_struct _tmp440;struct Cyc_Port_UnknownAggr_ct_struct*
_tmp43F;return(void*)((_tmp43F=_cycalloc(sizeof(*_tmp43F)),((_tmp43F[0]=((
_tmp440.tag=4,((_tmp440.f1=fs,((_tmp440.f2=0,_tmp440)))))),_tmp43F))));}static
void*Cyc_Port_known_aggr_ct(struct _tuple4*p);static void*Cyc_Port_known_aggr_ct(
struct _tuple4*p){struct Cyc_Port_KnownAggr_ct_struct _tmp443;struct Cyc_Port_KnownAggr_ct_struct*
_tmp442;return(void*)((_tmp442=_cycalloc(sizeof(*_tmp442)),((_tmp442[0]=((
_tmp443.tag=3,((_tmp443.f1=p,_tmp443)))),_tmp442))));}static void*Cyc_Port_ptr_ct(
void*elt,void*qual,void*ptr_kind,void*r,void*zt);static void*Cyc_Port_ptr_ct(void*
elt,void*qual,void*ptr_kind,void*r,void*zt){struct Cyc_Port_Ptr_ct_struct _tmp446;
struct Cyc_Port_Ptr_ct_struct*_tmp445;return(void*)((_tmp445=_cycalloc(sizeof(*
_tmp445)),((_tmp445[0]=((_tmp446.tag=1,((_tmp446.f1=(void*)elt,((_tmp446.f2=(
void*)qual,((_tmp446.f3=(void*)ptr_kind,((_tmp446.f4=(void*)r,((_tmp446.f5=(void*)
zt,_tmp446)))))))))))),_tmp445))));}static void*Cyc_Port_array_ct(void*elt,void*
qual,void*zt);static void*Cyc_Port_array_ct(void*elt,void*qual,void*zt){struct Cyc_Port_Array_ct_struct
_tmp449;struct Cyc_Port_Array_ct_struct*_tmp448;return(void*)((_tmp448=_cycalloc(
sizeof(*_tmp448)),((_tmp448[0]=((_tmp449.tag=2,((_tmp449.f1=(void*)elt,((_tmp449.f2=(
void*)qual,((_tmp449.f3=(void*)zt,_tmp449)))))))),_tmp448))));}static void*Cyc_Port_fn_ct(
void*return_type,struct Cyc_List_List*args);static void*Cyc_Port_fn_ct(void*
return_type,struct Cyc_List_List*args){struct Cyc_Port_Fn_ct_struct _tmp44C;struct
Cyc_Port_Fn_ct_struct*_tmp44B;return(void*)((_tmp44B=_cycalloc(sizeof(*_tmp44B)),((
_tmp44B[0]=((_tmp44C.tag=5,((_tmp44C.f1=(void*)return_type,((_tmp44C.f2=args,
_tmp44C)))))),_tmp44B))));}static void*Cyc_Port_var();static void*Cyc_Port_var(){
static int counter=0;struct Cyc_Port_Var_ct_struct _tmp452;struct Cyc_Port_Cvar*
_tmp451;struct Cyc_Port_Var_ct_struct*_tmp450;return(void*)((_tmp450=_cycalloc(
sizeof(*_tmp450)),((_tmp450[0]=((_tmp452.tag=6,((_tmp452.f1=((_tmp451=_cycalloc(
sizeof(*_tmp451)),((_tmp451->id=counter ++,((_tmp451->eq=0,((_tmp451->uppers=0,((
_tmp451->lowers=0,_tmp451)))))))))),_tmp452)))),_tmp450))));}static void*Cyc_Port_new_var(
void*x);static void*Cyc_Port_new_var(void*x){return Cyc_Port_var();}static struct
_dyneither_ptr*Cyc_Port_new_region_var();static struct _dyneither_ptr*Cyc_Port_new_region_var(){
static int counter=0;const char*_tmp456;void*_tmp455[1];struct Cyc_Int_pa_struct
_tmp454;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp454.tag=1,((_tmp454.f1=(
unsigned long)counter ++,((_tmp455[0]=& _tmp454,Cyc_aprintf(((_tmp456="`R%d",
_tag_dyneither(_tmp456,sizeof(char),5))),_tag_dyneither(_tmp455,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp457;return(_tmp457=_cycalloc(sizeof(*_tmp457)),((
_tmp457[0]=s,_tmp457)));}static int Cyc_Port_unifies(void*t1,void*t2);static void*
Cyc_Port_compress_ct(void*t);static void*Cyc_Port_compress_ct(void*t){void*_tmp69=
t;struct Cyc_Port_Cvar*_tmp6A;struct Cyc_Port_Cvar _tmp6B;void**_tmp6C;void***
_tmp6D;struct Cyc_List_List*_tmp6E;struct Cyc_List_List*_tmp6F;void**_tmp70;_LL26:
if(_tmp69 <= (void*)10)goto _LL2A;if(*((int*)_tmp69)!= 6)goto _LL28;_tmp6A=((struct
Cyc_Port_Var_ct_struct*)_tmp69)->f1;_tmp6B=*_tmp6A;_tmp6C=_tmp6B.eq;_tmp6D=(void***)&(*((
struct Cyc_Port_Var_ct_struct*)_tmp69)->f1).eq;_tmp6E=_tmp6B.uppers;_tmp6F=_tmp6B.lowers;
_LL27: {void**_tmp71=*_tmp6D;if((unsigned int)_tmp71){void*r=Cyc_Port_compress_ct(*
_tmp71);if(*_tmp71 != r){void**_tmp458;*_tmp6D=((_tmp458=_cycalloc(sizeof(*
_tmp458)),((_tmp458[0]=r,_tmp458))));}return r;}for(0;_tmp6F != 0;_tmp6F=_tmp6F->tl){
void*_tmp73=(void*)_tmp6F->hd;_LL2D: if((int)_tmp73 != 0)goto _LL2F;_LL2E: goto _LL30;
_LL2F: if((int)_tmp73 != 9)goto _LL31;_LL30: goto _LL32;_LL31: if((int)_tmp73 != 4)goto
_LL33;_LL32: goto _LL34;_LL33: if(_tmp73 <= (void*)10)goto _LL37;if(*((int*)_tmp73)!= 
3)goto _LL35;_LL34: goto _LL36;_LL35: if(*((int*)_tmp73)!= 5)goto _LL37;_LL36:{void**
_tmp459;*_tmp6D=((_tmp459=_cycalloc(sizeof(*_tmp459)),((_tmp459[0]=(void*)_tmp6F->hd,
_tmp459))));}return(void*)_tmp6F->hd;_LL37:;_LL38: goto _LL2C;_LL2C:;}for(0;_tmp6E
!= 0;_tmp6E=_tmp6E->tl){void*_tmp75=(void*)_tmp6E->hd;_LL3A: if((int)_tmp75 != 1)
goto _LL3C;_LL3B: goto _LL3D;_LL3C: if((int)_tmp75 != 8)goto _LL3E;_LL3D: goto _LL3F;
_LL3E: if((int)_tmp75 != 5)goto _LL40;_LL3F: goto _LL41;_LL40: if(_tmp75 <= (void*)10)
goto _LL44;if(*((int*)_tmp75)!= 3)goto _LL42;_LL41: goto _LL43;_LL42: if(*((int*)
_tmp75)!= 5)goto _LL44;_LL43:{void**_tmp45A;*_tmp6D=((_tmp45A=_cycalloc(sizeof(*
_tmp45A)),((_tmp45A[0]=(void*)_tmp6E->hd,_tmp45A))));}return(void*)_tmp6E->hd;
_LL44:;_LL45: goto _LL39;_LL39:;}return t;}_LL28: if(*((int*)_tmp69)!= 4)goto _LL2A;
_tmp70=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmp69)->f2;_LL29: if((
unsigned int)_tmp70)return Cyc_Port_compress_ct(*_tmp70);else{return t;}_LL2A:;
_LL2B: return t;_LL25:;}static void*Cyc_Port_inst(struct Cyc_Dict_Dict*instenv,void*
t);static void*Cyc_Port_inst(struct Cyc_Dict_Dict*instenv,void*t){void*_tmp77=Cyc_Port_compress_ct(
t);struct _dyneither_ptr*_tmp78;void*_tmp79;void*_tmp7A;void*_tmp7B;void*_tmp7C;
void*_tmp7D;void*_tmp7E;void*_tmp7F;void*_tmp80;void*_tmp81;struct Cyc_List_List*
_tmp82;_LL47: if((int)_tmp77 != 0)goto _LL49;_LL48: goto _LL4A;_LL49: if((int)_tmp77 != 
1)goto _LL4B;_LL4A: goto _LL4C;_LL4B: if((int)_tmp77 != 2)goto _LL4D;_LL4C: goto _LL4E;
_LL4D: if((int)_tmp77 != 3)goto _LL4F;_LL4E: goto _LL50;_LL4F: if((int)_tmp77 != 4)goto
_LL51;_LL50: goto _LL52;_LL51: if((int)_tmp77 != 5)goto _LL53;_LL52: goto _LL54;_LL53:
if((int)_tmp77 != 6)goto _LL55;_LL54: goto _LL56;_LL55: if((int)_tmp77 != 8)goto _LL57;
_LL56: goto _LL58;_LL57: if((int)_tmp77 != 9)goto _LL59;_LL58: goto _LL5A;_LL59: if(
_tmp77 <= (void*)10)goto _LL5F;if(*((int*)_tmp77)!= 4)goto _LL5B;_LL5A: goto _LL5C;
_LL5B: if(*((int*)_tmp77)!= 3)goto _LL5D;_LL5C: goto _LL5E;_LL5D: if(*((int*)_tmp77)
!= 6)goto _LL5F;_LL5E: goto _LL60;_LL5F: if((int)_tmp77 != 7)goto _LL61;_LL60: return t;
_LL61: if(_tmp77 <= (void*)10)goto _LL63;if(*((int*)_tmp77)!= 0)goto _LL63;_tmp78=((
struct Cyc_Port_RgnVar_ct_struct*)_tmp77)->f1;_LL62: if(!((int(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k))Cyc_Dict_member)(*instenv,_tmp78))*instenv=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*instenv,
_tmp78,Cyc_Port_var());return((void*(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(*instenv,_tmp78);_LL63: if(_tmp77 <= (void*)10)
goto _LL65;if(*((int*)_tmp77)!= 1)goto _LL65;_tmp79=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp77)->f1;_tmp7A=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp77)->f2;_tmp7B=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp77)->f3;_tmp7C=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp77)->f4;_tmp7D=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp77)->f5;_LL64: {
void*_tmp85;void*_tmp86;struct _tuple3 _tmp45B;struct _tuple3 _tmp84=(_tmp45B.f1=Cyc_Port_inst(
instenv,_tmp79),((_tmp45B.f2=Cyc_Port_inst(instenv,_tmp7C),_tmp45B)));_tmp85=
_tmp84.f1;_tmp86=_tmp84.f2;if(_tmp85 == _tmp79  && _tmp86 == _tmp7C)return t;{struct
Cyc_Port_Ptr_ct_struct _tmp45E;struct Cyc_Port_Ptr_ct_struct*_tmp45D;return(void*)((
_tmp45D=_cycalloc(sizeof(*_tmp45D)),((_tmp45D[0]=((_tmp45E.tag=1,((_tmp45E.f1=(
void*)_tmp85,((_tmp45E.f2=(void*)_tmp7A,((_tmp45E.f3=(void*)_tmp7B,((_tmp45E.f4=(
void*)_tmp86,((_tmp45E.f5=(void*)_tmp7D,_tmp45E)))))))))))),_tmp45D))));}}_LL65:
if(_tmp77 <= (void*)10)goto _LL67;if(*((int*)_tmp77)!= 2)goto _LL67;_tmp7E=(void*)((
struct Cyc_Port_Array_ct_struct*)_tmp77)->f1;_tmp7F=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmp77)->f2;_tmp80=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp77)->f3;_LL66: {
void*_tmp89=Cyc_Port_inst(instenv,_tmp7E);if(_tmp89 == _tmp7E)return t;{struct Cyc_Port_Array_ct_struct
_tmp461;struct Cyc_Port_Array_ct_struct*_tmp460;return(void*)((_tmp460=_cycalloc(
sizeof(*_tmp460)),((_tmp460[0]=((_tmp461.tag=2,((_tmp461.f1=(void*)_tmp89,((
_tmp461.f2=(void*)_tmp7F,((_tmp461.f3=(void*)_tmp80,_tmp461)))))))),_tmp460))));}}
_LL67: if(_tmp77 <= (void*)10)goto _LL46;if(*((int*)_tmp77)!= 5)goto _LL46;_tmp81=(
void*)((struct Cyc_Port_Fn_ct_struct*)_tmp77)->f1;_tmp82=((struct Cyc_Port_Fn_ct_struct*)
_tmp77)->f2;_LL68: {struct Cyc_Port_Fn_ct_struct _tmp464;struct Cyc_Port_Fn_ct_struct*
_tmp463;return(void*)((_tmp463=_cycalloc(sizeof(*_tmp463)),((_tmp463[0]=((
_tmp464.tag=5,((_tmp464.f1=(void*)Cyc_Port_inst(instenv,_tmp81),((_tmp464.f2=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Dict_Dict*,void*),struct Cyc_Dict_Dict*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_inst,instenv,_tmp82),_tmp464)))))),
_tmp463))));}_LL46:;}void*Cyc_Port_instantiate(void*t);void*Cyc_Port_instantiate(
void*t){struct Cyc_Dict_Dict*_tmp465;return Cyc_Port_inst(((_tmp465=_cycalloc(
sizeof(*_tmp465)),((_tmp465[0]=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),_tmp465)))),
t);}struct Cyc_List_List*Cyc_Port_filter_self(void*t,struct Cyc_List_List*ts);
struct Cyc_List_List*Cyc_Port_filter_self(void*t,struct Cyc_List_List*ts){int found=
0;{struct Cyc_List_List*_tmp8F=ts;for(0;(unsigned int)_tmp8F;_tmp8F=_tmp8F->tl){
void*_tmp90=Cyc_Port_compress_ct((void*)_tmp8F->hd);if(t == _tmp90)found=1;}}if(!
found)return ts;{struct Cyc_List_List*res=0;for(0;ts != 0;ts=ts->tl){if(t == Cyc_Port_compress_ct((
void*)ts->hd))continue;{struct Cyc_List_List*_tmp466;res=((_tmp466=_cycalloc(
sizeof(*_tmp466)),((_tmp466->hd=(void*)((void*)ts->hd),((_tmp466->tl=res,_tmp466))))));}}
return res;}}void Cyc_Port_generalize(int is_rgn,void*t);void Cyc_Port_generalize(
int is_rgn,void*t){t=Cyc_Port_compress_ct(t);{void*_tmp92=t;struct Cyc_Port_Cvar*
_tmp93;void*_tmp94;void*_tmp95;void*_tmp96;void*_tmp97;void*_tmp98;void*_tmp99;
void*_tmp9A;void*_tmp9B;void*_tmp9C;struct Cyc_List_List*_tmp9D;_LL6A: if(_tmp92 <= (
void*)10)goto _LL6C;if(*((int*)_tmp92)!= 6)goto _LL6C;_tmp93=((struct Cyc_Port_Var_ct_struct*)
_tmp92)->f1;_LL6B: _tmp93->uppers=Cyc_Port_filter_self(t,_tmp93->uppers);_tmp93->lowers=
Cyc_Port_filter_self(t,_tmp93->lowers);if(is_rgn){if(_tmp93->uppers == 0  && 
_tmp93->lowers == 0){Cyc_Port_unifies(t,Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var()));
return;}if((unsigned int)_tmp93->uppers){Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)
_check_null(_tmp93->uppers))->hd);Cyc_Port_generalize(1,t);}else{Cyc_Port_unifies(
t,(void*)((struct Cyc_List_List*)_check_null(_tmp93->lowers))->hd);Cyc_Port_generalize(
1,t);}}return;_LL6C: if((int)_tmp92 != 0)goto _LL6E;_LL6D: goto _LL6F;_LL6E: if((int)
_tmp92 != 1)goto _LL70;_LL6F: goto _LL71;_LL70: if((int)_tmp92 != 2)goto _LL72;_LL71:
goto _LL73;_LL72: if((int)_tmp92 != 3)goto _LL74;_LL73: goto _LL75;_LL74: if((int)
_tmp92 != 4)goto _LL76;_LL75: goto _LL77;_LL76: if((int)_tmp92 != 5)goto _LL78;_LL77:
goto _LL79;_LL78: if((int)_tmp92 != 6)goto _LL7A;_LL79: goto _LL7B;_LL7A: if(_tmp92 <= (
void*)10)goto _LL80;if(*((int*)_tmp92)!= 4)goto _LL7C;_LL7B: goto _LL7D;_LL7C: if(*((
int*)_tmp92)!= 3)goto _LL7E;_LL7D: goto _LL7F;_LL7E: if(*((int*)_tmp92)!= 0)goto
_LL80;_LL7F: goto _LL81;_LL80: if((int)_tmp92 != 9)goto _LL82;_LL81: goto _LL83;_LL82:
if((int)_tmp92 != 8)goto _LL84;_LL83: goto _LL85;_LL84: if((int)_tmp92 != 7)goto _LL86;
_LL85: return;_LL86: if(_tmp92 <= (void*)10)goto _LL88;if(*((int*)_tmp92)!= 1)goto
_LL88;_tmp94=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp92)->f1;_tmp95=(void*)((
struct Cyc_Port_Ptr_ct_struct*)_tmp92)->f2;_tmp96=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp92)->f3;_tmp97=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp92)->f4;_tmp98=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp92)->f5;_LL87: Cyc_Port_generalize(0,
_tmp94);Cyc_Port_generalize(1,_tmp97);goto _LL69;_LL88: if(_tmp92 <= (void*)10)goto
_LL8A;if(*((int*)_tmp92)!= 2)goto _LL8A;_tmp99=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmp92)->f1;_tmp9A=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp92)->f2;_tmp9B=(
void*)((struct Cyc_Port_Array_ct_struct*)_tmp92)->f3;_LL89: Cyc_Port_generalize(0,
_tmp99);Cyc_Port_generalize(0,_tmp9A);goto _LL69;_LL8A: if(_tmp92 <= (void*)10)goto
_LL69;if(*((int*)_tmp92)!= 5)goto _LL69;_tmp9C=(void*)((struct Cyc_Port_Fn_ct_struct*)
_tmp92)->f1;_tmp9D=((struct Cyc_Port_Fn_ct_struct*)_tmp92)->f2;_LL8B: Cyc_Port_generalize(
0,_tmp9C);((void(*)(void(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Port_generalize,0,_tmp9D);goto _LL69;_LL69:;}}static int Cyc_Port_occurs(void*v,
void*t);static int Cyc_Port_occurs(void*v,void*t){t=Cyc_Port_compress_ct(t);if(t == 
v)return 1;{void*_tmp9E=t;void*_tmp9F;void*_tmpA0;void*_tmpA1;void*_tmpA2;void*
_tmpA3;void*_tmpA4;void*_tmpA5;void*_tmpA6;void*_tmpA7;struct Cyc_List_List*
_tmpA8;struct _tuple4*_tmpA9;struct _tuple4 _tmpAA;struct Cyc_List_List*_tmpAB;
struct Cyc_List_List*_tmpAC;_LL8D: if(_tmp9E <= (void*)10)goto _LL97;if(*((int*)
_tmp9E)!= 1)goto _LL8F;_tmp9F=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp9E)->f1;
_tmpA0=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp9E)->f2;_tmpA1=(void*)((struct
Cyc_Port_Ptr_ct_struct*)_tmp9E)->f3;_tmpA2=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp9E)->f4;_tmpA3=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp9E)->f5;_LL8E:
return(((Cyc_Port_occurs(v,_tmp9F) || Cyc_Port_occurs(v,_tmpA0)) || Cyc_Port_occurs(
v,_tmpA1)) || Cyc_Port_occurs(v,_tmpA2)) || Cyc_Port_occurs(v,_tmpA3);_LL8F: if(*((
int*)_tmp9E)!= 2)goto _LL91;_tmpA4=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmp9E)->f1;_tmpA5=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp9E)->f2;_tmpA6=(
void*)((struct Cyc_Port_Array_ct_struct*)_tmp9E)->f3;_LL90: return(Cyc_Port_occurs(
v,_tmpA4) || Cyc_Port_occurs(v,_tmpA5)) || Cyc_Port_occurs(v,_tmpA6);_LL91: if(*((
int*)_tmp9E)!= 5)goto _LL93;_tmpA7=(void*)((struct Cyc_Port_Fn_ct_struct*)_tmp9E)->f1;
_tmpA8=((struct Cyc_Port_Fn_ct_struct*)_tmp9E)->f2;_LL92: if(Cyc_Port_occurs(v,
_tmpA7))return 1;for(0;(unsigned int)_tmpA8;_tmpA8=_tmpA8->tl){if(Cyc_Port_occurs(
v,(void*)_tmpA8->hd))return 1;}return 0;_LL93: if(*((int*)_tmp9E)!= 3)goto _LL95;
_tmpA9=((struct Cyc_Port_KnownAggr_ct_struct*)_tmp9E)->f1;_tmpAA=*_tmpA9;_tmpAB=
_tmpAA.f2;_LL94: return 0;_LL95: if(*((int*)_tmp9E)!= 4)goto _LL97;_tmpAC=((struct
Cyc_Port_UnknownAggr_ct_struct*)_tmp9E)->f1;_LL96: for(0;(unsigned int)_tmpAC;
_tmpAC=_tmpAC->tl){if(Cyc_Port_occurs(v,(void*)((struct Cyc_Port_Cfield*)_tmpAC->hd)->qual)
 || Cyc_Port_occurs(v,(void*)((struct Cyc_Port_Cfield*)_tmpAC->hd)->type))return 1;}
return 0;_LL97:;_LL98: return 0;_LL8C:;}}char Cyc_Port_Unify_ct[13]="\000\000\000\000Unify_ct\000";
static int Cyc_Port_leq(void*t1,void*t2);static void Cyc_Port_unify_cts(struct Cyc_List_List*
t1,struct Cyc_List_List*t2);static struct Cyc_List_List*Cyc_Port_merge_fields(
struct Cyc_List_List*fs1,struct Cyc_List_List*fs2,int allow_subset);static void Cyc_Port_unify_ct(
void*t1,void*t2);static void Cyc_Port_unify_ct(void*t1,void*t2){t1=Cyc_Port_compress_ct(
t1);t2=Cyc_Port_compress_ct(t2);if(t1 == t2)return;{struct _tuple3 _tmp467;struct
_tuple3 _tmpAE=(_tmp467.f1=t1,((_tmp467.f2=t2,_tmp467)));void*_tmpAF;struct Cyc_Port_Cvar*
_tmpB0;void*_tmpB1;struct Cyc_Port_Cvar*_tmpB2;void*_tmpB3;void*_tmpB4;void*
_tmpB5;void*_tmpB6;void*_tmpB7;void*_tmpB8;void*_tmpB9;void*_tmpBA;void*_tmpBB;
void*_tmpBC;void*_tmpBD;void*_tmpBE;void*_tmpBF;struct _dyneither_ptr*_tmpC0;
struct _dyneither_ptr _tmpC1;void*_tmpC2;struct _dyneither_ptr*_tmpC3;struct
_dyneither_ptr _tmpC4;void*_tmpC5;void*_tmpC6;void*_tmpC7;void*_tmpC8;void*_tmpC9;
void*_tmpCA;void*_tmpCB;void*_tmpCC;void*_tmpCD;void*_tmpCE;struct Cyc_List_List*
_tmpCF;void*_tmpD0;void*_tmpD1;struct Cyc_List_List*_tmpD2;void*_tmpD3;struct
_tuple4*_tmpD4;void*_tmpD5;struct _tuple4*_tmpD6;void*_tmpD7;struct Cyc_List_List*
_tmpD8;void**_tmpD9;void***_tmpDA;void*_tmpDB;struct Cyc_List_List*_tmpDC;void**
_tmpDD;void***_tmpDE;void*_tmpDF;struct Cyc_List_List*_tmpE0;void**_tmpE1;void***
_tmpE2;void*_tmpE3;struct _tuple4*_tmpE4;struct _tuple4 _tmpE5;struct Cyc_Absyn_Aggrdecl*
_tmpE6;struct Cyc_List_List*_tmpE7;void*_tmpE8;struct _tuple4*_tmpE9;struct _tuple4
_tmpEA;struct Cyc_Absyn_Aggrdecl*_tmpEB;struct Cyc_List_List*_tmpEC;void*_tmpED;
struct Cyc_List_List*_tmpEE;void**_tmpEF;void***_tmpF0;_LL9A: _tmpAF=_tmpAE.f1;if(
_tmpAF <= (void*)10)goto _LL9C;if(*((int*)_tmpAF)!= 6)goto _LL9C;_tmpB0=((struct Cyc_Port_Var_ct_struct*)
_tmpAF)->f1;_LL9B: if(!Cyc_Port_occurs(t1,t2)){{struct Cyc_List_List*_tmpF1=Cyc_Port_filter_self(
t1,_tmpB0->uppers);for(0;(unsigned int)_tmpF1;_tmpF1=_tmpF1->tl){if(!Cyc_Port_leq(
t2,(void*)_tmpF1->hd))(int)_throw((void*)Cyc_Port_Unify_ct);}}{struct Cyc_List_List*
_tmpF2=Cyc_Port_filter_self(t1,_tmpB0->lowers);for(0;(unsigned int)_tmpF2;_tmpF2=
_tmpF2->tl){if(!Cyc_Port_leq((void*)_tmpF2->hd,t2))(int)_throw((void*)Cyc_Port_Unify_ct);}}{
void**_tmp468;_tmpB0->eq=((_tmp468=_cycalloc(sizeof(*_tmp468)),((_tmp468[0]=t2,
_tmp468))));}return;}else{(int)_throw((void*)Cyc_Port_Unify_ct);}_LL9C: _tmpB1=
_tmpAE.f2;if(_tmpB1 <= (void*)10)goto _LL9E;if(*((int*)_tmpB1)!= 6)goto _LL9E;
_tmpB2=((struct Cyc_Port_Var_ct_struct*)_tmpB1)->f1;_LL9D: Cyc_Port_unify_ct(t2,t1);
return;_LL9E: _tmpB3=_tmpAE.f1;if(_tmpB3 <= (void*)10)goto _LLA0;if(*((int*)_tmpB3)
!= 1)goto _LLA0;_tmpB4=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmpB3)->f1;_tmpB5=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmpB3)->f2;_tmpB6=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmpB3)->f3;_tmpB7=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmpB3)->f4;_tmpB8=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmpB3)->f5;_tmpB9=_tmpAE.f2;if(_tmpB9 <= (
void*)10)goto _LLA0;if(*((int*)_tmpB9)!= 1)goto _LLA0;_tmpBA=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmpB9)->f1;_tmpBB=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmpB9)->f2;_tmpBC=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmpB9)->f3;_tmpBD=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmpB9)->f4;_tmpBE=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmpB9)->f5;_LL9F: Cyc_Port_unify_ct(
_tmpB4,_tmpBA);Cyc_Port_unify_ct(_tmpB5,_tmpBB);Cyc_Port_unify_ct(_tmpB6,_tmpBC);
Cyc_Port_unify_ct(_tmpB7,_tmpBD);Cyc_Port_unify_ct(_tmpB8,_tmpBE);return;_LLA0:
_tmpBF=_tmpAE.f1;if(_tmpBF <= (void*)10)goto _LLA2;if(*((int*)_tmpBF)!= 0)goto
_LLA2;_tmpC0=((struct Cyc_Port_RgnVar_ct_struct*)_tmpBF)->f1;_tmpC1=*_tmpC0;
_tmpC2=_tmpAE.f2;if(_tmpC2 <= (void*)10)goto _LLA2;if(*((int*)_tmpC2)!= 0)goto
_LLA2;_tmpC3=((struct Cyc_Port_RgnVar_ct_struct*)_tmpC2)->f1;_tmpC4=*_tmpC3;_LLA1:
if(Cyc_strcmp((struct _dyneither_ptr)_tmpC1,(struct _dyneither_ptr)_tmpC4)!= 0)(
int)_throw((void*)Cyc_Port_Unify_ct);return;_LLA2: _tmpC5=_tmpAE.f1;if(_tmpC5 <= (
void*)10)goto _LLA4;if(*((int*)_tmpC5)!= 2)goto _LLA4;_tmpC6=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmpC5)->f1;_tmpC7=(void*)((struct Cyc_Port_Array_ct_struct*)_tmpC5)->f2;_tmpC8=(
void*)((struct Cyc_Port_Array_ct_struct*)_tmpC5)->f3;_tmpC9=_tmpAE.f2;if(_tmpC9 <= (
void*)10)goto _LLA4;if(*((int*)_tmpC9)!= 2)goto _LLA4;_tmpCA=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmpC9)->f1;_tmpCB=(void*)((struct Cyc_Port_Array_ct_struct*)_tmpC9)->f2;_tmpCC=(
void*)((struct Cyc_Port_Array_ct_struct*)_tmpC9)->f3;_LLA3: Cyc_Port_unify_ct(
_tmpC6,_tmpCA);Cyc_Port_unify_ct(_tmpC7,_tmpCB);Cyc_Port_unify_ct(_tmpC8,_tmpCC);
return;_LLA4: _tmpCD=_tmpAE.f1;if(_tmpCD <= (void*)10)goto _LLA6;if(*((int*)_tmpCD)
!= 5)goto _LLA6;_tmpCE=(void*)((struct Cyc_Port_Fn_ct_struct*)_tmpCD)->f1;_tmpCF=((
struct Cyc_Port_Fn_ct_struct*)_tmpCD)->f2;_tmpD0=_tmpAE.f2;if(_tmpD0 <= (void*)10)
goto _LLA6;if(*((int*)_tmpD0)!= 5)goto _LLA6;_tmpD1=(void*)((struct Cyc_Port_Fn_ct_struct*)
_tmpD0)->f1;_tmpD2=((struct Cyc_Port_Fn_ct_struct*)_tmpD0)->f2;_LLA5: Cyc_Port_unify_ct(
_tmpCE,_tmpD1);Cyc_Port_unify_cts(_tmpCF,_tmpD2);return;_LLA6: _tmpD3=_tmpAE.f1;
if(_tmpD3 <= (void*)10)goto _LLA8;if(*((int*)_tmpD3)!= 3)goto _LLA8;_tmpD4=((struct
Cyc_Port_KnownAggr_ct_struct*)_tmpD3)->f1;_tmpD5=_tmpAE.f2;if(_tmpD5 <= (void*)10)
goto _LLA8;if(*((int*)_tmpD5)!= 3)goto _LLA8;_tmpD6=((struct Cyc_Port_KnownAggr_ct_struct*)
_tmpD5)->f1;_LLA7: if(_tmpD4 == _tmpD6)return;else{(int)_throw((void*)Cyc_Port_Unify_ct);}
_LLA8: _tmpD7=_tmpAE.f1;if(_tmpD7 <= (void*)10)goto _LLAA;if(*((int*)_tmpD7)!= 4)
goto _LLAA;_tmpD8=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmpD7)->f1;_tmpD9=((
struct Cyc_Port_UnknownAggr_ct_struct*)_tmpD7)->f2;_tmpDA=(void***)&((struct Cyc_Port_UnknownAggr_ct_struct*)
_tmpD7)->f2;_tmpDB=_tmpAE.f2;if(_tmpDB <= (void*)10)goto _LLAA;if(*((int*)_tmpDB)
!= 4)goto _LLAA;_tmpDC=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmpDB)->f1;_tmpDD=((
struct Cyc_Port_UnknownAggr_ct_struct*)_tmpDB)->f2;_tmpDE=(void***)&((struct Cyc_Port_UnknownAggr_ct_struct*)
_tmpDB)->f2;_LLA9: {void*_tmpF4=Cyc_Port_unknown_aggr_ct(Cyc_Port_merge_fields(
_tmpD8,_tmpDC,1));{void**_tmp469;*_tmpDA=(*_tmpDE=((_tmp469=_cycalloc(sizeof(*
_tmp469)),((_tmp469[0]=_tmpF4,_tmp469)))));}return;}_LLAA: _tmpDF=_tmpAE.f1;if(
_tmpDF <= (void*)10)goto _LLAC;if(*((int*)_tmpDF)!= 4)goto _LLAC;_tmpE0=((struct Cyc_Port_UnknownAggr_ct_struct*)
_tmpDF)->f1;_tmpE1=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmpDF)->f2;_tmpE2=(
void***)&((struct Cyc_Port_UnknownAggr_ct_struct*)_tmpDF)->f2;_tmpE3=_tmpAE.f2;
if(_tmpE3 <= (void*)10)goto _LLAC;if(*((int*)_tmpE3)!= 3)goto _LLAC;_tmpE4=((struct
Cyc_Port_KnownAggr_ct_struct*)_tmpE3)->f1;_tmpE5=*_tmpE4;_tmpE6=_tmpE5.f1;_tmpE7=
_tmpE5.f2;_LLAB: Cyc_Port_merge_fields(_tmpE7,_tmpE0,0);{void**_tmp46A;*_tmpE2=((
_tmp46A=_cycalloc(sizeof(*_tmp46A)),((_tmp46A[0]=t2,_tmp46A))));}return;_LLAC:
_tmpE8=_tmpAE.f1;if(_tmpE8 <= (void*)10)goto _LLAE;if(*((int*)_tmpE8)!= 3)goto
_LLAE;_tmpE9=((struct Cyc_Port_KnownAggr_ct_struct*)_tmpE8)->f1;_tmpEA=*_tmpE9;
_tmpEB=_tmpEA.f1;_tmpEC=_tmpEA.f2;_tmpED=_tmpAE.f2;if(_tmpED <= (void*)10)goto
_LLAE;if(*((int*)_tmpED)!= 4)goto _LLAE;_tmpEE=((struct Cyc_Port_UnknownAggr_ct_struct*)
_tmpED)->f1;_tmpEF=((struct Cyc_Port_UnknownAggr_ct_struct*)_tmpED)->f2;_tmpF0=(
void***)&((struct Cyc_Port_UnknownAggr_ct_struct*)_tmpED)->f2;_LLAD: Cyc_Port_merge_fields(
_tmpEC,_tmpEE,0);{void**_tmp46B;*_tmpF0=((_tmp46B=_cycalloc(sizeof(*_tmp46B)),((
_tmp46B[0]=t1,_tmp46B))));}return;_LLAE:;_LLAF:(int)_throw((void*)Cyc_Port_Unify_ct);
_LL99:;}}static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*
t2);static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Port_unify_ct((void*)t1->hd,(
void*)t2->hd);}if(t1 != 0  || t2 != 0)(int)_throw((void*)Cyc_Port_Unify_ct);}static
struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct Cyc_List_List*
fs2,int allow_f1_subset_f2);static struct Cyc_List_List*Cyc_Port_merge_fields(
struct Cyc_List_List*fs1,struct Cyc_List_List*fs2,int allow_f1_subset_f2){struct Cyc_List_List*
common=0;{struct Cyc_List_List*_tmpF8=fs2;for(0;(unsigned int)_tmpF8;_tmpF8=
_tmpF8->tl){struct Cyc_Port_Cfield*_tmpF9=(struct Cyc_Port_Cfield*)_tmpF8->hd;int
found=0;{struct Cyc_List_List*_tmpFA=fs1;for(0;(unsigned int)_tmpFA;_tmpFA=_tmpFA->tl){
struct Cyc_Port_Cfield*_tmpFB=(struct Cyc_Port_Cfield*)_tmpFA->hd;if(Cyc_strptrcmp(
_tmpFB->f,_tmpF9->f)== 0){{struct Cyc_List_List*_tmp46C;common=((_tmp46C=
_cycalloc(sizeof(*_tmp46C)),((_tmp46C->hd=_tmpFB,((_tmp46C->tl=common,_tmp46C))))));}
Cyc_Port_unify_ct((void*)_tmpFB->qual,(void*)_tmpF9->qual);Cyc_Port_unify_ct((
void*)_tmpFB->type,(void*)_tmpF9->type);found=1;break;}}}if(!found){if(
allow_f1_subset_f2){struct Cyc_List_List*_tmp46D;common=((_tmp46D=_cycalloc(
sizeof(*_tmp46D)),((_tmp46D->hd=_tmpF9,((_tmp46D->tl=common,_tmp46D))))));}else{(
int)_throw((void*)Cyc_Port_Unify_ct);}}}}{struct Cyc_List_List*_tmpFE=fs1;for(0;(
unsigned int)_tmpFE;_tmpFE=_tmpFE->tl){struct Cyc_Port_Cfield*_tmpFF=(struct Cyc_Port_Cfield*)
_tmpFE->hd;int found=0;{struct Cyc_List_List*_tmp100=fs2;for(0;(unsigned int)
_tmp100;_tmp100=_tmp100->tl){struct Cyc_Port_Cfield*_tmp101=(struct Cyc_Port_Cfield*)
_tmp100->hd;if(Cyc_strptrcmp(_tmpFF->f,_tmp101->f))found=1;}}if(!found){struct
Cyc_List_List*_tmp46E;common=((_tmp46E=_cycalloc(sizeof(*_tmp46E)),((_tmp46E->hd=
_tmpFF,((_tmp46E->tl=common,_tmp46E))))));}}}return common;}static int Cyc_Port_unifies(
void*t1,void*t2);static int Cyc_Port_unifies(void*t1,void*t2){{struct _handler_cons
_tmp103;_push_handler(& _tmp103);{int _tmp105=0;if(setjmp(_tmp103.handler))_tmp105=
1;if(!_tmp105){Cyc_Port_unify_ct(t1,t2);;_pop_handler();}else{void*_tmp104=(void*)
_exn_thrown;void*_tmp107=_tmp104;_LLB1: if(_tmp107 != Cyc_Port_Unify_ct)goto _LLB3;
_LLB2: return 0;_LLB3:;_LLB4:(void)_throw(_tmp107);_LLB0:;}}}return 1;}struct
_tuple5{void*f1;void*f2;void*f3;void*f4;void*f5;};static struct Cyc_List_List*Cyc_Port_insert_upper(
void*v,void*t,struct Cyc_List_List**uppers);static struct Cyc_List_List*Cyc_Port_insert_upper(
void*v,void*t,struct Cyc_List_List**uppers){t=Cyc_Port_compress_ct(t);{void*
_tmp108=t;_LLB6: if((int)_tmp108 != 1)goto _LLB8;_LLB7: goto _LLB9;_LLB8: if((int)
_tmp108 != 8)goto _LLBA;_LLB9: goto _LLBB;_LLBA: if((int)_tmp108 != 5)goto _LLBC;_LLBB:
goto _LLBD;_LLBC: if((int)_tmp108 != 2)goto _LLBE;_LLBD: goto _LLBF;_LLBE: if((int)
_tmp108 != 3)goto _LLC0;_LLBF: goto _LLC1;_LLC0: if(_tmp108 <= (void*)10)goto _LLC6;if(*((
int*)_tmp108)!= 2)goto _LLC2;_LLC1: goto _LLC3;_LLC2: if(*((int*)_tmp108)!= 3)goto
_LLC4;_LLC3: goto _LLC5;_LLC4: if(*((int*)_tmp108)!= 5)goto _LLC6;_LLC5: goto _LLC7;
_LLC6: if((int)_tmp108 != 7)goto _LLC8;_LLC7:*uppers=0;Cyc_Port_unifies(v,t);return*
uppers;_LLC8: if((int)_tmp108 != 4)goto _LLCA;_LLC9: goto _LLCB;_LLCA: if((int)_tmp108
!= 0)goto _LLCC;_LLCB: goto _LLCD;_LLCC: if((int)_tmp108 != 9)goto _LLCE;_LLCD: return*
uppers;_LLCE:;_LLCF: goto _LLB5;_LLB5:;}{struct Cyc_List_List*_tmp109=*uppers;for(0;(
unsigned int)_tmp109;_tmp109=_tmp109->tl){void*_tmp10A=Cyc_Port_compress_ct((
void*)_tmp109->hd);if(t == _tmp10A)return*uppers;{struct _tuple3 _tmp46F;struct
_tuple3 _tmp10C=(_tmp46F.f1=t,((_tmp46F.f2=_tmp10A,_tmp46F)));void*_tmp10D;void*
_tmp10E;void*_tmp10F;void*_tmp110;void*_tmp111;void*_tmp112;void*_tmp113;void*
_tmp114;void*_tmp115;void*_tmp116;void*_tmp117;void*_tmp118;void*_tmp119;void*
_tmp11A;void*_tmp11B;void*_tmp11C;void*_tmp11D;void*_tmp11E;_LLD1: _tmp10D=
_tmp10C.f1;if((int)_tmp10D != 6)goto _LLD3;_tmp10E=_tmp10C.f2;if(_tmp10E <= (void*)
10)goto _LLD3;if(*((int*)_tmp10E)!= 1)goto _LLD3;_LLD2: goto _LLD4;_LLD3: _tmp10F=
_tmp10C.f1;if((int)_tmp10F != 6)goto _LLD5;_tmp110=_tmp10C.f2;if((int)_tmp110 != 5)
goto _LLD5;_LLD4: goto _LLD6;_LLD5: _tmp111=_tmp10C.f1;if((int)_tmp111 != 6)goto _LLD7;
_tmp112=_tmp10C.f2;if(_tmp112 <= (void*)10)goto _LLD7;if(*((int*)_tmp112)!= 2)goto
_LLD7;_LLD6: return*uppers;_LLD7: _tmp113=_tmp10C.f1;if(_tmp113 <= (void*)10)goto
_LLD9;if(*((int*)_tmp113)!= 1)goto _LLD9;_tmp114=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp113)->f1;_tmp115=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp113)->f2;_tmp116=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp113)->f3;_tmp117=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp113)->f4;_tmp118=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp113)->f5;_tmp119=
_tmp10C.f2;if(_tmp119 <= (void*)10)goto _LLD9;if(*((int*)_tmp119)!= 1)goto _LLD9;
_tmp11A=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp119)->f1;_tmp11B=(void*)((
struct Cyc_Port_Ptr_ct_struct*)_tmp119)->f2;_tmp11C=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp119)->f3;_tmp11D=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp119)->f4;_tmp11E=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp119)->f5;_LLD8: {void*_tmp121;void*
_tmp122;void*_tmp123;void*_tmp124;void*_tmp125;struct _tuple5 _tmp470;struct
_tuple5 _tmp120=(_tmp470.f1=Cyc_Port_var(),((_tmp470.f2=Cyc_Port_var(),((_tmp470.f3=
Cyc_Port_var(),((_tmp470.f4=Cyc_Port_var(),((_tmp470.f5=Cyc_Port_var(),_tmp470)))))))));
_tmp121=_tmp120.f1;_tmp122=_tmp120.f2;_tmp123=_tmp120.f3;_tmp124=_tmp120.f4;
_tmp125=_tmp120.f5;{struct Cyc_Port_Ptr_ct_struct _tmp473;struct Cyc_Port_Ptr_ct_struct*
_tmp472;struct Cyc_Port_Ptr_ct_struct*_tmp126=(_tmp472=_cycalloc(sizeof(*_tmp472)),((
_tmp472[0]=((_tmp473.tag=1,((_tmp473.f1=(void*)_tmp121,((_tmp473.f2=(void*)
_tmp122,((_tmp473.f3=(void*)_tmp123,((_tmp473.f4=(void*)_tmp124,((_tmp473.f5=(
void*)_tmp125,_tmp473)))))))))))),_tmp472)));Cyc_Port_leq(_tmp121,_tmp114);Cyc_Port_leq(
_tmp121,_tmp11A);Cyc_Port_leq(_tmp122,_tmp115);Cyc_Port_leq(_tmp122,_tmp11B);Cyc_Port_leq(
_tmp123,_tmp116);Cyc_Port_leq(_tmp123,_tmp11B);Cyc_Port_leq(_tmp124,_tmp117);Cyc_Port_leq(
_tmp124,_tmp11D);Cyc_Port_leq(_tmp125,_tmp118);Cyc_Port_leq(_tmp125,_tmp11E);(
void*)(_tmp109->hd=(void*)((void*)_tmp126));return*uppers;}}_LLD9:;_LLDA: goto
_LLD0;_LLD0:;}}}{struct Cyc_List_List*_tmp474;return(_tmp474=_cycalloc(sizeof(*
_tmp474)),((_tmp474->hd=(void*)t,((_tmp474->tl=*uppers,_tmp474)))));}}static
struct Cyc_List_List*Cyc_Port_insert_lower(void*v,void*t,struct Cyc_List_List**
lowers);static struct Cyc_List_List*Cyc_Port_insert_lower(void*v,void*t,struct Cyc_List_List**
lowers){t=Cyc_Port_compress_ct(t);{void*_tmp12A=t;_LLDC: if((int)_tmp12A != 0)goto
_LLDE;_LLDD: goto _LLDF;_LLDE: if((int)_tmp12A != 8)goto _LLE0;_LLDF: goto _LLE1;_LLE0:
if((int)_tmp12A != 2)goto _LLE2;_LLE1: goto _LLE3;_LLE2: if((int)_tmp12A != 3)goto
_LLE4;_LLE3: goto _LLE5;_LLE4: if((int)_tmp12A != 4)goto _LLE6;_LLE5: goto _LLE7;_LLE6:
if(_tmp12A <= (void*)10)goto _LLEC;if(*((int*)_tmp12A)!= 3)goto _LLE8;_LLE7: goto
_LLE9;_LLE8: if(*((int*)_tmp12A)!= 5)goto _LLEA;_LLE9: goto _LLEB;_LLEA: if(*((int*)
_tmp12A)!= 0)goto _LLEC;_LLEB:*lowers=0;Cyc_Port_unifies(v,t);return*lowers;_LLEC:
if((int)_tmp12A != 7)goto _LLEE;_LLED: goto _LLEF;_LLEE: if((int)_tmp12A != 1)goto
_LLF0;_LLEF: goto _LLF1;_LLF0: if((int)_tmp12A != 9)goto _LLF2;_LLF1: return*lowers;
_LLF2:;_LLF3: goto _LLDB;_LLDB:;}{struct Cyc_List_List*_tmp12B=*lowers;for(0;(
unsigned int)_tmp12B;_tmp12B=_tmp12B->tl){void*_tmp12C=Cyc_Port_compress_ct((
void*)_tmp12B->hd);if(t == _tmp12C)return*lowers;{struct _tuple3 _tmp475;struct
_tuple3 _tmp12E=(_tmp475.f1=t,((_tmp475.f2=_tmp12C,_tmp475)));void*_tmp12F;void*
_tmp130;void*_tmp131;void*_tmp132;void*_tmp133;void*_tmp134;void*_tmp135;void*
_tmp136;void*_tmp137;void*_tmp138;void*_tmp139;void*_tmp13A;void*_tmp13B;void*
_tmp13C;void*_tmp13D;void*_tmp13E;void*_tmp13F;void*_tmp140;void*_tmp141;void*
_tmp142;void*_tmp143;_LLF5: _tmp12F=_tmp12E.f2;if((int)_tmp12F != 4)goto _LLF7;
_LLF6: goto _LLF8;_LLF7: _tmp130=_tmp12E.f1;if((int)_tmp130 != 5)goto _LLF9;_tmp131=
_tmp12E.f2;if((int)_tmp131 != 6)goto _LLF9;_LLF8: goto _LLFA;_LLF9: _tmp132=_tmp12E.f1;
if((int)_tmp132 != 5)goto _LLFB;_tmp133=_tmp12E.f2;if(_tmp133 <= (void*)10)goto
_LLFB;if(*((int*)_tmp133)!= 1)goto _LLFB;_LLFA: goto _LLFC;_LLFB: _tmp134=_tmp12E.f1;
if(_tmp134 <= (void*)10)goto _LLFD;if(*((int*)_tmp134)!= 1)goto _LLFD;_tmp135=
_tmp12E.f2;if((int)_tmp135 != 6)goto _LLFD;_LLFC: goto _LLFE;_LLFD: _tmp136=_tmp12E.f1;
if(_tmp136 <= (void*)10)goto _LLFF;if(*((int*)_tmp136)!= 2)goto _LLFF;_tmp137=
_tmp12E.f2;if((int)_tmp137 != 6)goto _LLFF;_LLFE: return*lowers;_LLFF: _tmp138=
_tmp12E.f1;if(_tmp138 <= (void*)10)goto _LL101;if(*((int*)_tmp138)!= 1)goto _LL101;
_tmp139=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp138)->f1;_tmp13A=(void*)((
struct Cyc_Port_Ptr_ct_struct*)_tmp138)->f2;_tmp13B=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp138)->f3;_tmp13C=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp138)->f4;_tmp13D=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp138)->f5;_tmp13E=_tmp12E.f2;if(_tmp13E
<= (void*)10)goto _LL101;if(*((int*)_tmp13E)!= 1)goto _LL101;_tmp13F=(void*)((
struct Cyc_Port_Ptr_ct_struct*)_tmp13E)->f1;_tmp140=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp13E)->f2;_tmp141=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp13E)->f3;_tmp142=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp13E)->f4;_tmp143=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp13E)->f5;_LL100: {void*_tmp146;void*_tmp147;void*_tmp148;void*_tmp149;void*
_tmp14A;struct _tuple5 _tmp476;struct _tuple5 _tmp145=(_tmp476.f1=Cyc_Port_var(),((
_tmp476.f2=Cyc_Port_var(),((_tmp476.f3=Cyc_Port_var(),((_tmp476.f4=Cyc_Port_var(),((
_tmp476.f5=Cyc_Port_var(),_tmp476)))))))));_tmp146=_tmp145.f1;_tmp147=_tmp145.f2;
_tmp148=_tmp145.f3;_tmp149=_tmp145.f4;_tmp14A=_tmp145.f5;{struct Cyc_Port_Ptr_ct_struct
_tmp479;struct Cyc_Port_Ptr_ct_struct*_tmp478;struct Cyc_Port_Ptr_ct_struct*
_tmp14B=(_tmp478=_cycalloc(sizeof(*_tmp478)),((_tmp478[0]=((_tmp479.tag=1,((
_tmp479.f1=(void*)_tmp146,((_tmp479.f2=(void*)_tmp147,((_tmp479.f3=(void*)
_tmp148,((_tmp479.f4=(void*)_tmp149,((_tmp479.f5=(void*)_tmp14A,_tmp479)))))))))))),
_tmp478)));Cyc_Port_leq(_tmp139,_tmp146);Cyc_Port_leq(_tmp13F,_tmp146);Cyc_Port_leq(
_tmp13A,_tmp147);Cyc_Port_leq(_tmp140,_tmp147);Cyc_Port_leq(_tmp13B,_tmp148);Cyc_Port_leq(
_tmp140,_tmp148);Cyc_Port_leq(_tmp13C,_tmp149);Cyc_Port_leq(_tmp142,_tmp149);Cyc_Port_leq(
_tmp13D,_tmp14A);Cyc_Port_leq(_tmp143,_tmp14A);(void*)(_tmp12B->hd=(void*)((void*)
_tmp14B));return*lowers;}}_LL101:;_LL102: goto _LLF4;_LLF4:;}}}{struct Cyc_List_List*
_tmp47A;return(_tmp47A=_cycalloc(sizeof(*_tmp47A)),((_tmp47A->hd=(void*)t,((
_tmp47A->tl=*lowers,_tmp47A)))));}}static int Cyc_Port_leq(void*t1,void*t2);static
int Cyc_Port_leq(void*t1,void*t2){if(t1 == t2)return 1;t1=Cyc_Port_compress_ct(t1);
t2=Cyc_Port_compress_ct(t2);{struct _tuple3 _tmp47B;struct _tuple3 _tmp150=(_tmp47B.f1=
t1,((_tmp47B.f2=t2,_tmp47B)));void*_tmp151;void*_tmp152;struct _dyneither_ptr*
_tmp153;struct _dyneither_ptr _tmp154;void*_tmp155;struct _dyneither_ptr*_tmp156;
struct _dyneither_ptr _tmp157;void*_tmp158;struct _dyneither_ptr*_tmp159;struct
_dyneither_ptr _tmp15A;void*_tmp15B;void*_tmp15C;void*_tmp15D;void*_tmp15E;void*
_tmp15F;void*_tmp160;void*_tmp161;void*_tmp162;void*_tmp163;void*_tmp164;void*
_tmp165;void*_tmp166;void*_tmp167;void*_tmp168;void*_tmp169;void*_tmp16A;void*
_tmp16B;void*_tmp16C;void*_tmp16D;void*_tmp16E;void*_tmp16F;void*_tmp170;void*
_tmp171;void*_tmp172;void*_tmp173;void*_tmp174;void*_tmp175;void*_tmp176;void*
_tmp177;void*_tmp178;void*_tmp179;void*_tmp17A;void*_tmp17B;void*_tmp17C;void*
_tmp17D;void*_tmp17E;void*_tmp17F;void*_tmp180;void*_tmp181;void*_tmp182;void*
_tmp183;void*_tmp184;void*_tmp185;void*_tmp186;void*_tmp187;void*_tmp188;void*
_tmp189;void*_tmp18A;void*_tmp18B;void*_tmp18C;void*_tmp18D;void*_tmp18E;void*
_tmp18F;void*_tmp190;void*_tmp191;void*_tmp192;void*_tmp193;void*_tmp194;struct
Cyc_Port_Cvar*_tmp195;void*_tmp196;struct Cyc_Port_Cvar*_tmp197;void*_tmp198;
struct Cyc_Port_Cvar*_tmp199;void*_tmp19A;struct Cyc_Port_Cvar*_tmp19B;_LL104:
_tmp151=_tmp150.f1;if((int)_tmp151 != 7)goto _LL106;_LL105: return 1;_LL106: _tmp152=
_tmp150.f1;if(_tmp152 <= (void*)10)goto _LL108;if(*((int*)_tmp152)!= 0)goto _LL108;
_tmp153=((struct Cyc_Port_RgnVar_ct_struct*)_tmp152)->f1;_tmp154=*_tmp153;_tmp155=
_tmp150.f2;if(_tmp155 <= (void*)10)goto _LL108;if(*((int*)_tmp155)!= 0)goto _LL108;
_tmp156=((struct Cyc_Port_RgnVar_ct_struct*)_tmp155)->f1;_tmp157=*_tmp156;_LL107:
return Cyc_strcmp((struct _dyneither_ptr)_tmp154,(struct _dyneither_ptr)_tmp157)== 
0;_LL108: _tmp158=_tmp150.f1;if(_tmp158 <= (void*)10)goto _LL10A;if(*((int*)_tmp158)
!= 0)goto _LL10A;_tmp159=((struct Cyc_Port_RgnVar_ct_struct*)_tmp158)->f1;_tmp15A=*
_tmp159;_tmp15B=_tmp150.f2;if((int)_tmp15B != 7)goto _LL10A;_LL109: return 0;_LL10A:
_tmp15C=_tmp150.f1;if((int)_tmp15C != 1)goto _LL10C;_tmp15D=_tmp150.f2;if((int)
_tmp15D != 0)goto _LL10C;_LL10B: return 1;_LL10C: _tmp15E=_tmp150.f1;if((int)_tmp15E
!= 0)goto _LL10E;_tmp15F=_tmp150.f2;if((int)_tmp15F != 1)goto _LL10E;_LL10D: return 0;
_LL10E: _tmp160=_tmp150.f1;if((int)_tmp160 != 9)goto _LL110;_tmp161=_tmp150.f2;if((
int)_tmp161 != 8)goto _LL110;_LL10F: return 0;_LL110: _tmp162=_tmp150.f1;if((int)
_tmp162 != 8)goto _LL112;_tmp163=_tmp150.f2;if((int)_tmp163 != 9)goto _LL112;_LL111:
return 1;_LL112: _tmp164=_tmp150.f1;if(_tmp164 <= (void*)10)goto _LL114;if(*((int*)
_tmp164)!= 6)goto _LL114;_tmp165=_tmp150.f2;if((int)_tmp165 != 0)goto _LL114;_LL113:
return 1;_LL114: _tmp166=_tmp150.f1;if(_tmp166 <= (void*)10)goto _LL116;if(*((int*)
_tmp166)!= 6)goto _LL116;_tmp167=_tmp150.f2;if((int)_tmp167 != 4)goto _LL116;_LL115:
return 1;_LL116: _tmp168=_tmp150.f1;if((int)_tmp168 != 4)goto _LL118;_LL117: return 0;
_LL118: _tmp169=_tmp150.f1;if((int)_tmp169 != 5)goto _LL11A;_tmp16A=_tmp150.f2;if((
int)_tmp16A != 6)goto _LL11A;_LL119: return 1;_LL11A: _tmp16B=_tmp150.f1;if((int)
_tmp16B != 5)goto _LL11C;_tmp16C=_tmp150.f2;if(_tmp16C <= (void*)10)goto _LL11C;if(*((
int*)_tmp16C)!= 1)goto _LL11C;_LL11B: return 1;_LL11C: _tmp16D=_tmp150.f1;if((int)
_tmp16D != 5)goto _LL11E;_tmp16E=_tmp150.f2;if((int)_tmp16E != 4)goto _LL11E;_LL11D:
return 1;_LL11E: _tmp16F=_tmp150.f1;if(_tmp16F <= (void*)10)goto _LL120;if(*((int*)
_tmp16F)!= 1)goto _LL120;_tmp170=_tmp150.f2;if((int)_tmp170 != 6)goto _LL120;_LL11F:
return 1;_LL120: _tmp171=_tmp150.f1;if(_tmp171 <= (void*)10)goto _LL122;if(*((int*)
_tmp171)!= 1)goto _LL122;_tmp172=_tmp150.f2;if((int)_tmp172 != 4)goto _LL122;_LL121:
return 1;_LL122: _tmp173=_tmp150.f1;if(_tmp173 <= (void*)10)goto _LL124;if(*((int*)
_tmp173)!= 2)goto _LL124;_tmp174=_tmp150.f2;if((int)_tmp174 != 6)goto _LL124;_LL123:
return 1;_LL124: _tmp175=_tmp150.f1;if(_tmp175 <= (void*)10)goto _LL126;if(*((int*)
_tmp175)!= 2)goto _LL126;_tmp176=_tmp150.f2;if((int)_tmp176 != 4)goto _LL126;_LL125:
return 1;_LL126: _tmp177=_tmp150.f1;if(_tmp177 <= (void*)10)goto _LL128;if(*((int*)
_tmp177)!= 1)goto _LL128;_tmp178=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp177)->f1;
_tmp179=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp177)->f2;_tmp17A=(void*)((
struct Cyc_Port_Ptr_ct_struct*)_tmp177)->f3;_tmp17B=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp177)->f4;_tmp17C=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp177)->f5;_tmp17D=
_tmp150.f2;if(_tmp17D <= (void*)10)goto _LL128;if(*((int*)_tmp17D)!= 1)goto _LL128;
_tmp17E=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp17D)->f1;_tmp17F=(void*)((
struct Cyc_Port_Ptr_ct_struct*)_tmp17D)->f2;_tmp180=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp17D)->f3;_tmp181=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp17D)->f4;_tmp182=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp17D)->f5;_LL127: return(((Cyc_Port_leq(
_tmp178,_tmp17E) && Cyc_Port_leq(_tmp179,_tmp17F)) && Cyc_Port_unifies(_tmp17A,
_tmp180)) && Cyc_Port_leq(_tmp17B,_tmp181)) && Cyc_Port_leq(_tmp17C,_tmp182);
_LL128: _tmp183=_tmp150.f1;if(_tmp183 <= (void*)10)goto _LL12A;if(*((int*)_tmp183)
!= 2)goto _LL12A;_tmp184=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp183)->f1;
_tmp185=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp183)->f2;_tmp186=(void*)((
struct Cyc_Port_Array_ct_struct*)_tmp183)->f3;_tmp187=_tmp150.f2;if(_tmp187 <= (
void*)10)goto _LL12A;if(*((int*)_tmp187)!= 2)goto _LL12A;_tmp188=(void*)((struct
Cyc_Port_Array_ct_struct*)_tmp187)->f1;_tmp189=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmp187)->f2;_tmp18A=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp187)->f3;
_LL129: return(Cyc_Port_leq(_tmp184,_tmp188) && Cyc_Port_leq(_tmp185,_tmp189))
 && Cyc_Port_leq(_tmp186,_tmp18A);_LL12A: _tmp18B=_tmp150.f1;if(_tmp18B <= (void*)
10)goto _LL12C;if(*((int*)_tmp18B)!= 2)goto _LL12C;_tmp18C=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmp18B)->f1;_tmp18D=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp18B)->f2;
_tmp18E=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp18B)->f3;_tmp18F=_tmp150.f2;
if(_tmp18F <= (void*)10)goto _LL12C;if(*((int*)_tmp18F)!= 1)goto _LL12C;_tmp190=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp18F)->f1;_tmp191=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp18F)->f2;_tmp192=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp18F)->f3;_tmp193=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp18F)->f5;_LL12B: return((Cyc_Port_leq(
_tmp18C,_tmp190) && Cyc_Port_leq(_tmp18D,_tmp191)) && Cyc_Port_unifies((void*)3,
_tmp192)) && Cyc_Port_leq(_tmp18E,_tmp193);_LL12C: _tmp194=_tmp150.f1;if(_tmp194
<= (void*)10)goto _LL12E;if(*((int*)_tmp194)!= 6)goto _LL12E;_tmp195=((struct Cyc_Port_Var_ct_struct*)
_tmp194)->f1;_tmp196=_tmp150.f2;if(_tmp196 <= (void*)10)goto _LL12E;if(*((int*)
_tmp196)!= 6)goto _LL12E;_tmp197=((struct Cyc_Port_Var_ct_struct*)_tmp196)->f1;
_LL12D: _tmp195->uppers=Cyc_Port_filter_self(t1,_tmp195->uppers);_tmp197->lowers=
Cyc_Port_filter_self(t2,_tmp197->lowers);_tmp195->uppers=Cyc_Port_insert_upper(
t1,t2,& _tmp195->uppers);_tmp197->lowers=Cyc_Port_insert_lower(t2,t1,& _tmp197->lowers);
return 1;_LL12E: _tmp198=_tmp150.f1;if(_tmp198 <= (void*)10)goto _LL130;if(*((int*)
_tmp198)!= 6)goto _LL130;_tmp199=((struct Cyc_Port_Var_ct_struct*)_tmp198)->f1;
_LL12F: _tmp199->uppers=Cyc_Port_filter_self(t1,_tmp199->uppers);_tmp199->uppers=
Cyc_Port_insert_upper(t1,t2,& _tmp199->uppers);return 1;_LL130: _tmp19A=_tmp150.f2;
if(_tmp19A <= (void*)10)goto _LL132;if(*((int*)_tmp19A)!= 6)goto _LL132;_tmp19B=((
struct Cyc_Port_Var_ct_struct*)_tmp19A)->f1;_LL131: _tmp19B->lowers=Cyc_Port_filter_self(
t2,_tmp19B->lowers);_tmp19B->lowers=Cyc_Port_insert_lower(t2,t1,& _tmp19B->lowers);
return 1;_LL132:;_LL133: return Cyc_Port_unifies(t1,t2);_LL103:;}}struct Cyc_Port_GlobalCenv{
struct Cyc_Dict_Dict typedef_dict;struct Cyc_Dict_Dict struct_dict;struct Cyc_Dict_Dict
union_dict;void*return_type;struct Cyc_List_List*qualifier_edits;struct Cyc_List_List*
pointer_edits;struct Cyc_List_List*zeroterm_edits;struct Cyc_List_List*edits;int
porting;};struct Cyc_Port_Cenv{struct Cyc_Port_GlobalCenv*gcenv;struct Cyc_Dict_Dict
var_dict;void*cpos;};static struct Cyc_Port_Cenv*Cyc_Port_empty_cenv();static
struct Cyc_Port_Cenv*Cyc_Port_empty_cenv(){struct Cyc_Port_GlobalCenv*_tmp47C;
struct Cyc_Port_GlobalCenv*g=(_tmp47C=_cycalloc(sizeof(*_tmp47C)),((_tmp47C->typedef_dict=((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp),((_tmp47C->struct_dict=((struct Cyc_Dict_Dict(*)(int(*cmp)(
struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),((_tmp47C->union_dict=((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp),((_tmp47C->qualifier_edits=0,((_tmp47C->pointer_edits=0,((
_tmp47C->zeroterm_edits=0,((_tmp47C->edits=0,((_tmp47C->porting=0,((_tmp47C->return_type=(
void*)Cyc_Port_void_ct(),_tmp47C)))))))))))))))))));struct Cyc_Port_Cenv*_tmp47D;
return(_tmp47D=_cycalloc(sizeof(*_tmp47D)),((_tmp47D->gcenv=g,((_tmp47D->cpos=(
void*)((void*)3),((_tmp47D->var_dict=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct
_tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),_tmp47D)))))));}
static int Cyc_Port_in_fn_arg(struct Cyc_Port_Cenv*env);static int Cyc_Port_in_fn_arg(
struct Cyc_Port_Cenv*env){return(void*)env->cpos == (void*)1;}static int Cyc_Port_in_fn_res(
struct Cyc_Port_Cenv*env);static int Cyc_Port_in_fn_res(struct Cyc_Port_Cenv*env){
return(void*)env->cpos == (void*)0;}static int Cyc_Port_in_toplevel(struct Cyc_Port_Cenv*
env);static int Cyc_Port_in_toplevel(struct Cyc_Port_Cenv*env){return(void*)env->cpos
== (void*)3;}static void*Cyc_Port_lookup_return_type(struct Cyc_Port_Cenv*env);
static void*Cyc_Port_lookup_return_type(struct Cyc_Port_Cenv*env){return(void*)(
env->gcenv)->return_type;}static void*Cyc_Port_lookup_typedef(struct Cyc_Port_Cenv*
env,struct _tuple0*n);static void*Cyc_Port_lookup_typedef(struct Cyc_Port_Cenv*env,
struct _tuple0*n){struct _handler_cons _tmp19E;_push_handler(& _tmp19E);{int _tmp1A0=
0;if(setjmp(_tmp19E.handler))_tmp1A0=1;if(!_tmp1A0){{void*_tmp1A2;struct _tuple3
_tmp1A1=*((struct _tuple3*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((
env->gcenv)->typedef_dict,n);_tmp1A2=_tmp1A1.f1;{void*_tmp1A3=_tmp1A2;
_npop_handler(0);return _tmp1A3;}};_pop_handler();}else{void*_tmp19F=(void*)
_exn_thrown;void*_tmp1A5=_tmp19F;_LL135: if(_tmp1A5 != Cyc_Dict_Absent)goto _LL137;
_LL136: return Cyc_Absyn_sint_typ;_LL137:;_LL138:(void)_throw(_tmp1A5);_LL134:;}}}
static void*Cyc_Port_lookup_typedef_ctype(struct Cyc_Port_Cenv*env,struct _tuple0*n);
static void*Cyc_Port_lookup_typedef_ctype(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct _handler_cons _tmp1A6;_push_handler(& _tmp1A6);{int _tmp1A8=0;if(setjmp(
_tmp1A6.handler))_tmp1A8=1;if(!_tmp1A8){{void*_tmp1AA;struct _tuple3 _tmp1A9=*((
struct _tuple3*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,
n);_tmp1AA=_tmp1A9.f2;{void*_tmp1AB=_tmp1AA;_npop_handler(0);return _tmp1AB;}};
_pop_handler();}else{void*_tmp1A7=(void*)_exn_thrown;void*_tmp1AD=_tmp1A7;_LL13A:
if(_tmp1AD != Cyc_Dict_Absent)goto _LL13C;_LL13B: return Cyc_Port_var();_LL13C:;
_LL13D:(void)_throw(_tmp1AD);_LL139:;}}}static struct _tuple4*Cyc_Port_lookup_struct_decl(
struct Cyc_Port_Cenv*env,struct _tuple0*n);static struct _tuple4*Cyc_Port_lookup_struct_decl(
struct Cyc_Port_Cenv*env,struct _tuple0*n){struct _tuple4**_tmp1AE=((struct _tuple4**(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->struct_dict,
n);if((unsigned int)_tmp1AE)return*_tmp1AE;else{struct Cyc_Absyn_Aggrdecl*_tmp47E;
struct Cyc_Absyn_Aggrdecl*_tmp1AF=(_tmp47E=_cycalloc(sizeof(*_tmp47E)),((_tmp47E->kind=(
void*)((void*)0),((_tmp47E->sc=(void*)((void*)2),((_tmp47E->name=n,((_tmp47E->tvs=
0,((_tmp47E->impl=0,((_tmp47E->attributes=0,_tmp47E)))))))))))));struct _tuple4*
_tmp47F;struct _tuple4*p=(_tmp47F=_cycalloc(sizeof(*_tmp47F)),((_tmp47F->f1=
_tmp1AF,((_tmp47F->f2=0,_tmp47F)))));(env->gcenv)->struct_dict=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple4*v))Cyc_Dict_insert)((env->gcenv)->struct_dict,
n,p);return p;}}static struct _tuple4*Cyc_Port_lookup_union_decl(struct Cyc_Port_Cenv*
env,struct _tuple0*n);static struct _tuple4*Cyc_Port_lookup_union_decl(struct Cyc_Port_Cenv*
env,struct _tuple0*n){struct _tuple4**_tmp1B2=((struct _tuple4**(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->union_dict,n);if((
unsigned int)_tmp1B2)return*_tmp1B2;else{struct Cyc_Absyn_Aggrdecl*_tmp480;struct
Cyc_Absyn_Aggrdecl*_tmp1B3=(_tmp480=_cycalloc(sizeof(*_tmp480)),((_tmp480->kind=(
void*)((void*)1),((_tmp480->sc=(void*)((void*)2),((_tmp480->name=n,((_tmp480->tvs=
0,((_tmp480->impl=0,((_tmp480->attributes=0,_tmp480)))))))))))));struct _tuple4*
_tmp481;struct _tuple4*p=(_tmp481=_cycalloc(sizeof(*_tmp481)),((_tmp481->f1=
_tmp1B3,((_tmp481->f2=0,_tmp481)))));(env->gcenv)->union_dict=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple4*v))Cyc_Dict_insert)((env->gcenv)->union_dict,
n,p);return p;}}struct _tuple6{void*f1;struct _tuple3*f2;};static struct _tuple3 Cyc_Port_lookup_var(
struct Cyc_Port_Cenv*env,struct _tuple0*x);static struct _tuple3 Cyc_Port_lookup_var(
struct Cyc_Port_Cenv*env,struct _tuple0*x){struct _handler_cons _tmp1B6;
_push_handler(& _tmp1B6);{int _tmp1B8=0;if(setjmp(_tmp1B6.handler))_tmp1B8=1;if(!
_tmp1B8){{struct _tuple3*_tmp1BA;struct _tuple6 _tmp1B9=*((struct _tuple6*(*)(struct
Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);_tmp1BA=
_tmp1B9.f2;{struct _tuple3 _tmp1BB=*_tmp1BA;_npop_handler(0);return _tmp1BB;}};
_pop_handler();}else{void*_tmp1B7=(void*)_exn_thrown;void*_tmp1BD=_tmp1B7;_LL13F:
if(_tmp1BD != Cyc_Dict_Absent)goto _LL141;_LL140: {struct _tuple3 _tmp482;return(
_tmp482.f1=Cyc_Port_var(),((_tmp482.f2=Cyc_Port_var(),_tmp482)));}_LL141:;_LL142:(
void)_throw(_tmp1BD);_LL13E:;}}}static struct _tuple6*Cyc_Port_lookup_full_var(
struct Cyc_Port_Cenv*env,struct _tuple0*x);static struct _tuple6*Cyc_Port_lookup_full_var(
struct Cyc_Port_Cenv*env,struct _tuple0*x){return((struct _tuple6*(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);}static int Cyc_Port_declared_var(
struct Cyc_Port_Cenv*env,struct _tuple0*x);static int Cyc_Port_declared_var(struct
Cyc_Port_Cenv*env,struct _tuple0*x){return((int(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k))Cyc_Dict_member)(env->var_dict,x);}static int Cyc_Port_isfn(struct Cyc_Port_Cenv*
env,struct _tuple0*x);static int Cyc_Port_isfn(struct Cyc_Port_Cenv*env,struct
_tuple0*x){struct _handler_cons _tmp1BF;_push_handler(& _tmp1BF);{int _tmp1C1=0;if(
setjmp(_tmp1BF.handler))_tmp1C1=1;if(!_tmp1C1){{void*_tmp1C3;struct _tuple6
_tmp1C2=*((struct _tuple6*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(
env->var_dict,x);_tmp1C3=_tmp1C2.f1;LOOP: {void*_tmp1C4=_tmp1C3;struct _tuple0*
_tmp1C5;_LL144: if(_tmp1C4 <= (void*)4)goto _LL148;if(*((int*)_tmp1C4)!= 16)goto
_LL146;_tmp1C5=((struct Cyc_Absyn_TypedefType_struct*)_tmp1C4)->f1;_LL145: _tmp1C3=
Cyc_Port_lookup_typedef(env,_tmp1C5);goto LOOP;_LL146: if(*((int*)_tmp1C4)!= 8)
goto _LL148;_LL147: {int _tmp1C6=1;_npop_handler(0);return _tmp1C6;}_LL148:;_LL149: {
int _tmp1C7=0;_npop_handler(0);return _tmp1C7;}_LL143:;}};_pop_handler();}else{
void*_tmp1C0=(void*)_exn_thrown;void*_tmp1C9=_tmp1C0;_LL14B: if(_tmp1C9 != Cyc_Dict_Absent)
goto _LL14D;_LL14C: return 0;_LL14D:;_LL14E:(void)_throw(_tmp1C9);_LL14A:;}}}static
int Cyc_Port_isarray(struct Cyc_Port_Cenv*env,struct _tuple0*x);static int Cyc_Port_isarray(
struct Cyc_Port_Cenv*env,struct _tuple0*x){struct _handler_cons _tmp1CA;
_push_handler(& _tmp1CA);{int _tmp1CC=0;if(setjmp(_tmp1CA.handler))_tmp1CC=1;if(!
_tmp1CC){{void*_tmp1CE;struct _tuple6 _tmp1CD=*((struct _tuple6*(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);_tmp1CE=_tmp1CD.f1;LOOP: {
void*_tmp1CF=_tmp1CE;struct _tuple0*_tmp1D0;_LL150: if(_tmp1CF <= (void*)4)goto
_LL154;if(*((int*)_tmp1CF)!= 16)goto _LL152;_tmp1D0=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1CF)->f1;_LL151: _tmp1CE=Cyc_Port_lookup_typedef(env,_tmp1D0);goto LOOP;_LL152:
if(*((int*)_tmp1CF)!= 7)goto _LL154;_LL153: {int _tmp1D1=1;_npop_handler(0);return
_tmp1D1;}_LL154:;_LL155: {int _tmp1D2=0;_npop_handler(0);return _tmp1D2;}_LL14F:;}};
_pop_handler();}else{void*_tmp1CB=(void*)_exn_thrown;void*_tmp1D4=_tmp1CB;_LL157:
if(_tmp1D4 != Cyc_Dict_Absent)goto _LL159;_LL158: return 0;_LL159:;_LL15A:(void)
_throw(_tmp1D4);_LL156:;}}}static struct Cyc_Port_Cenv*Cyc_Port_set_cpos(struct Cyc_Port_Cenv*
env,void*cpos);static struct Cyc_Port_Cenv*Cyc_Port_set_cpos(struct Cyc_Port_Cenv*
env,void*cpos){struct Cyc_Port_Cenv*_tmp483;return(_tmp483=_cycalloc(sizeof(*
_tmp483)),((_tmp483->gcenv=env->gcenv,((_tmp483->var_dict=env->var_dict,((
_tmp483->cpos=(void*)cpos,_tmp483)))))));}static void Cyc_Port_add_return_type(
struct Cyc_Port_Cenv*env,void*t);static void Cyc_Port_add_return_type(struct Cyc_Port_Cenv*
env,void*t){(void*)((env->gcenv)->return_type=(void*)t);}static struct Cyc_Port_Cenv*
Cyc_Port_add_var(struct Cyc_Port_Cenv*env,struct _tuple0*x,void*t,void*qual,void*
ctype);static struct Cyc_Port_Cenv*Cyc_Port_add_var(struct Cyc_Port_Cenv*env,struct
_tuple0*x,void*t,void*qual,void*ctype){struct _tuple6*_tmp489;struct _tuple3*
_tmp488;struct Cyc_Port_Cenv*_tmp487;return(_tmp487=_cycalloc(sizeof(*_tmp487)),((
_tmp487->gcenv=env->gcenv,((_tmp487->var_dict=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _tuple0*k,struct _tuple6*v))Cyc_Dict_insert)(env->var_dict,x,((_tmp489=
_cycalloc(sizeof(*_tmp489)),((_tmp489->f1=t,((_tmp489->f2=((_tmp488=_cycalloc(
sizeof(*_tmp488)),((_tmp488->f1=qual,((_tmp488->f2=ctype,_tmp488)))))),_tmp489))))))),((
_tmp487->cpos=(void*)((void*)env->cpos),_tmp487)))))));}static void Cyc_Port_add_typedef(
struct Cyc_Port_Cenv*env,struct _tuple0*n,void*t,void*ct);static void Cyc_Port_add_typedef(
struct Cyc_Port_Cenv*env,struct _tuple0*n,void*t,void*ct){struct _tuple3*_tmp48A;(
env->gcenv)->typedef_dict=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple0*k,struct _tuple3*v))Cyc_Dict_insert)((env->gcenv)->typedef_dict,n,((
_tmp48A=_cycalloc(sizeof(*_tmp48A)),((_tmp48A->f1=t,((_tmp48A->f2=ct,_tmp48A)))))));}
struct _tuple7{void*f1;void*f2;struct Cyc_Position_Segment*f3;};static void Cyc_Port_register_const_cvar(
struct Cyc_Port_Cenv*env,void*new_qual,void*orig_qual,struct Cyc_Position_Segment*
loc);static void Cyc_Port_register_const_cvar(struct Cyc_Port_Cenv*env,void*
new_qual,void*orig_qual,struct Cyc_Position_Segment*loc){struct _tuple7*_tmp48D;
struct Cyc_List_List*_tmp48C;(env->gcenv)->qualifier_edits=((_tmp48C=_cycalloc(
sizeof(*_tmp48C)),((_tmp48C->hd=((_tmp48D=_cycalloc(sizeof(*_tmp48D)),((_tmp48D->f1=
new_qual,((_tmp48D->f2=orig_qual,((_tmp48D->f3=loc,_tmp48D)))))))),((_tmp48C->tl=(
env->gcenv)->qualifier_edits,_tmp48C))))));}static void Cyc_Port_register_ptr_cvars(
struct Cyc_Port_Cenv*env,void*new_ptr,void*orig_ptr,struct Cyc_Position_Segment*
loc);static void Cyc_Port_register_ptr_cvars(struct Cyc_Port_Cenv*env,void*new_ptr,
void*orig_ptr,struct Cyc_Position_Segment*loc){struct _tuple7*_tmp490;struct Cyc_List_List*
_tmp48F;(env->gcenv)->pointer_edits=((_tmp48F=_cycalloc(sizeof(*_tmp48F)),((
_tmp48F->hd=((_tmp490=_cycalloc(sizeof(*_tmp490)),((_tmp490->f1=new_ptr,((
_tmp490->f2=orig_ptr,((_tmp490->f3=loc,_tmp490)))))))),((_tmp48F->tl=(env->gcenv)->pointer_edits,
_tmp48F))))));}static void Cyc_Port_register_zeroterm_cvars(struct Cyc_Port_Cenv*
env,void*new_zt,void*orig_zt,struct Cyc_Position_Segment*loc);static void Cyc_Port_register_zeroterm_cvars(
struct Cyc_Port_Cenv*env,void*new_zt,void*orig_zt,struct Cyc_Position_Segment*loc){
struct _tuple7*_tmp493;struct Cyc_List_List*_tmp492;(env->gcenv)->zeroterm_edits=((
_tmp492=_cycalloc(sizeof(*_tmp492)),((_tmp492->hd=((_tmp493=_cycalloc(sizeof(*
_tmp493)),((_tmp493->f1=new_zt,((_tmp493->f2=orig_zt,((_tmp493->f3=loc,_tmp493)))))))),((
_tmp492->tl=(env->gcenv)->zeroterm_edits,_tmp492))))));}static void*Cyc_Port_main_type();
static void*Cyc_Port_main_type(){struct _tuple1*_tmp494;struct _tuple1*arg1=(
_tmp494=_cycalloc(sizeof(*_tmp494)),((_tmp494->f1=0,((_tmp494->f2=Cyc_Absyn_empty_tqual(
0),((_tmp494->f3=Cyc_Absyn_sint_typ,_tmp494)))))));struct _tuple1*_tmp495;struct
_tuple1*arg2=(_tmp495=_cycalloc(sizeof(*_tmp495)),((_tmp495->f1=0,((_tmp495->f2=
Cyc_Absyn_empty_tqual(0),((_tmp495->f3=Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(
Cyc_Absyn_wildtyp(0)),Cyc_Absyn_wildtyp(0),Cyc_Absyn_empty_tqual(0),((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)()),_tmp495)))))));struct Cyc_Absyn_FnType_struct _tmp49F;
struct _tuple1*_tmp49E[2];struct Cyc_Absyn_FnInfo _tmp49D;struct Cyc_Absyn_FnType_struct*
_tmp49C;return(void*)((_tmp49C=_cycalloc(sizeof(*_tmp49C)),((_tmp49C[0]=((
_tmp49F.tag=8,((_tmp49F.f1=((_tmp49D.tvars=0,((_tmp49D.effect=0,((_tmp49D.ret_typ=(
void*)Cyc_Absyn_sint_typ,((_tmp49D.args=((_tmp49E[1]=arg2,((_tmp49E[0]=arg1,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp49E,sizeof(struct _tuple1*),2)))))),((_tmp49D.c_varargs=0,((_tmp49D.cyc_varargs=
0,((_tmp49D.rgn_po=0,((_tmp49D.attributes=0,_tmp49D)))))))))))))))),_tmp49F)))),
_tmp49C))));}static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t);
static struct Cyc_Port_Cenv*Cyc_Port_initial_cenv();static struct Cyc_Port_Cenv*Cyc_Port_initial_cenv(){
struct Cyc_Port_Cenv*_tmp1E6=Cyc_Port_empty_cenv();{union Cyc_Absyn_Nmspace_union
_tmp4A2;struct _tuple0*_tmp4A1;_tmp1E6=Cyc_Port_add_var(_tmp1E6,((_tmp4A1=
_cycalloc(sizeof(*_tmp4A1)),((_tmp4A1->f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp4A2.Loc_n).tag=0,_tmp4A2)),((_tmp4A1->f2=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"main",sizeof(char),5),_tmp4A1)))))),Cyc_Port_main_type(),
Cyc_Port_const_ct(),Cyc_Port_type_to_ctype(_tmp1E6,Cyc_Port_main_type()));}
return _tmp1E6;}static void Cyc_Port_region_counts(struct Cyc_Dict_Dict*cts,void*t);
static void Cyc_Port_region_counts(struct Cyc_Dict_Dict*cts,void*t){void*_tmp1EA=
Cyc_Port_compress_ct(t);struct _dyneither_ptr*_tmp1EB;void*_tmp1EC;void*_tmp1ED;
void*_tmp1EE;void*_tmp1EF;void*_tmp1F0;void*_tmp1F1;void*_tmp1F2;void*_tmp1F3;
void*_tmp1F4;struct Cyc_List_List*_tmp1F5;_LL15C: if((int)_tmp1EA != 0)goto _LL15E;
_LL15D: goto _LL15F;_LL15E: if((int)_tmp1EA != 1)goto _LL160;_LL15F: goto _LL161;_LL160:
if((int)_tmp1EA != 2)goto _LL162;_LL161: goto _LL163;_LL162: if((int)_tmp1EA != 3)goto
_LL164;_LL163: goto _LL165;_LL164: if((int)_tmp1EA != 4)goto _LL166;_LL165: goto _LL167;
_LL166: if((int)_tmp1EA != 5)goto _LL168;_LL167: goto _LL169;_LL168: if((int)_tmp1EA != 
6)goto _LL16A;_LL169: goto _LL16B;_LL16A: if(_tmp1EA <= (void*)10)goto _LL170;if(*((
int*)_tmp1EA)!= 4)goto _LL16C;_LL16B: goto _LL16D;_LL16C: if(*((int*)_tmp1EA)!= 3)
goto _LL16E;_LL16D: goto _LL16F;_LL16E: if(*((int*)_tmp1EA)!= 6)goto _LL170;_LL16F:
goto _LL171;_LL170: if((int)_tmp1EA != 8)goto _LL172;_LL171: goto _LL173;_LL172: if((
int)_tmp1EA != 9)goto _LL174;_LL173: goto _LL175;_LL174: if((int)_tmp1EA != 7)goto
_LL176;_LL175: return;_LL176: if(_tmp1EA <= (void*)10)goto _LL178;if(*((int*)_tmp1EA)
!= 0)goto _LL178;_tmp1EB=((struct Cyc_Port_RgnVar_ct_struct*)_tmp1EA)->f1;_LL177:
if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*cts,
_tmp1EB)){int*_tmp4A3;*cts=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,int*v))Cyc_Dict_insert)(*cts,_tmp1EB,(int*)((_tmp4A3=
_cycalloc_atomic(sizeof(*_tmp4A3)),((_tmp4A3[0]=0,_tmp4A3)))));}{int*_tmp1F7=((
int*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*cts,
_tmp1EB);*_tmp1F7=*_tmp1F7 + 1;return;}_LL178: if(_tmp1EA <= (void*)10)goto _LL17A;
if(*((int*)_tmp1EA)!= 1)goto _LL17A;_tmp1EC=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp1EA)->f1;_tmp1ED=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp1EA)->f2;_tmp1EE=(
void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp1EA)->f3;_tmp1EF=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp1EA)->f4;_tmp1F0=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp1EA)->f5;_LL179:
Cyc_Port_region_counts(cts,_tmp1EC);Cyc_Port_region_counts(cts,_tmp1EF);return;
_LL17A: if(_tmp1EA <= (void*)10)goto _LL17C;if(*((int*)_tmp1EA)!= 2)goto _LL17C;
_tmp1F1=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp1EA)->f1;_tmp1F2=(void*)((
struct Cyc_Port_Array_ct_struct*)_tmp1EA)->f2;_tmp1F3=(void*)((struct Cyc_Port_Array_ct_struct*)
_tmp1EA)->f3;_LL17B: Cyc_Port_region_counts(cts,_tmp1F1);return;_LL17C: if(_tmp1EA
<= (void*)10)goto _LL15B;if(*((int*)_tmp1EA)!= 5)goto _LL15B;_tmp1F4=(void*)((
struct Cyc_Port_Fn_ct_struct*)_tmp1EA)->f1;_tmp1F5=((struct Cyc_Port_Fn_ct_struct*)
_tmp1EA)->f2;_LL17D: Cyc_Port_region_counts(cts,_tmp1F4);for(0;(unsigned int)
_tmp1F5;_tmp1F5=_tmp1F5->tl){Cyc_Port_region_counts(cts,(void*)_tmp1F5->hd);}
return;_LL15B:;}static void Cyc_Port_register_rgns(struct Cyc_Port_Cenv*env,struct
Cyc_Dict_Dict counts,int fn_res,void*t,void*c);static void Cyc_Port_register_rgns(
struct Cyc_Port_Cenv*env,struct Cyc_Dict_Dict counts,int fn_res,void*t,void*c){c=Cyc_Port_compress_ct(
c);{struct _tuple3 _tmp4A4;struct _tuple3 _tmp1F9=(_tmp4A4.f1=t,((_tmp4A4.f2=c,
_tmp4A4)));void*_tmp1FA;struct Cyc_Absyn_PtrInfo _tmp1FB;void*_tmp1FC;struct Cyc_Absyn_PtrAtts
_tmp1FD;void*_tmp1FE;struct Cyc_Absyn_PtrLoc*_tmp1FF;void*_tmp200;void*_tmp201;
void*_tmp202;void*_tmp203;struct Cyc_Absyn_ArrayInfo _tmp204;void*_tmp205;void*
_tmp206;void*_tmp207;void*_tmp208;struct Cyc_Absyn_FnInfo _tmp209;void*_tmp20A;
struct Cyc_List_List*_tmp20B;void*_tmp20C;void*_tmp20D;struct Cyc_List_List*
_tmp20E;_LL17F: _tmp1FA=_tmp1F9.f1;if(_tmp1FA <= (void*)4)goto _LL181;if(*((int*)
_tmp1FA)!= 4)goto _LL181;_tmp1FB=((struct Cyc_Absyn_PointerType_struct*)_tmp1FA)->f1;
_tmp1FC=(void*)_tmp1FB.elt_typ;_tmp1FD=_tmp1FB.ptr_atts;_tmp1FE=(void*)_tmp1FD.rgn;
_tmp1FF=_tmp1FD.ptrloc;_tmp200=_tmp1F9.f2;if(_tmp200 <= (void*)10)goto _LL181;if(*((
int*)_tmp200)!= 1)goto _LL181;_tmp201=(void*)((struct Cyc_Port_Ptr_ct_struct*)
_tmp200)->f1;_tmp202=(void*)((struct Cyc_Port_Ptr_ct_struct*)_tmp200)->f4;_LL180:
Cyc_Port_register_rgns(env,counts,fn_res,_tmp1FC,_tmp201);{struct Cyc_Position_Segment*
_tmp20F=(unsigned int)_tmp1FF?_tmp1FF->rgn_loc: 0;_tmp202=Cyc_Port_compress_ct(
_tmp202);{struct _tuple3 _tmp4A5;struct _tuple3 _tmp211=(_tmp4A5.f1=_tmp1FE,((
_tmp4A5.f2=_tmp202,_tmp4A5)));void*_tmp212;void*_tmp213;void*_tmp214;void*
_tmp215;struct _dyneither_ptr*_tmp216;_LL188: _tmp212=_tmp211.f1;if(_tmp212 <= (
void*)4)goto _LL18A;if(*((int*)_tmp212)!= 0)goto _LL18A;_tmp213=_tmp211.f2;if((int)
_tmp213 != 7)goto _LL18A;if(!(!fn_res))goto _LL18A;_LL189:{struct Cyc_Port_Edit*
_tmp4AE;const char*_tmp4AD;const char*_tmp4AC;struct Cyc_List_List*_tmp4AB;(env->gcenv)->edits=((
_tmp4AB=_cycalloc(sizeof(*_tmp4AB)),((_tmp4AB->hd=((_tmp4AE=_cycalloc(sizeof(*
_tmp4AE)),((_tmp4AE->loc=_tmp20F,((_tmp4AE->old_string=((_tmp4AC="`H ",
_tag_dyneither(_tmp4AC,sizeof(char),4))),((_tmp4AE->new_string=((_tmp4AD="",
_tag_dyneither(_tmp4AD,sizeof(char),1))),_tmp4AE)))))))),((_tmp4AB->tl=(env->gcenv)->edits,
_tmp4AB))))));}goto _LL187;_LL18A: _tmp214=_tmp211.f1;if(_tmp214 <= (void*)4)goto
_LL18C;if(*((int*)_tmp214)!= 0)goto _LL18C;_tmp215=_tmp211.f2;if(_tmp215 <= (void*)
10)goto _LL18C;if(*((int*)_tmp215)!= 0)goto _LL18C;_tmp216=((struct Cyc_Port_RgnVar_ct_struct*)
_tmp215)->f1;_LL18B: {int _tmp21B=*((int*(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(counts,_tmp216);if(_tmp21B > 1){struct Cyc_Port_Edit*
_tmp4BE;const char*_tmp4BD;const char*_tmp4BC;void*_tmp4BB[1];struct Cyc_String_pa_struct
_tmp4BA;struct Cyc_List_List*_tmp4B9;(env->gcenv)->edits=((_tmp4B9=_cycalloc(
sizeof(*_tmp4B9)),((_tmp4B9->hd=((_tmp4BE=_cycalloc(sizeof(*_tmp4BE)),((_tmp4BE->loc=
_tmp20F,((_tmp4BE->old_string=(struct _dyneither_ptr)((_tmp4BA.tag=0,((_tmp4BA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp216),((_tmp4BB[0]=& _tmp4BA,Cyc_aprintf(((
_tmp4BC="%s ",_tag_dyneither(_tmp4BC,sizeof(char),4))),_tag_dyneither(_tmp4BB,
sizeof(void*),1)))))))),((_tmp4BE->new_string=((_tmp4BD="",_tag_dyneither(
_tmp4BD,sizeof(char),1))),_tmp4BE)))))))),((_tmp4B9->tl=(env->gcenv)->edits,
_tmp4B9))))));}goto _LL187;}_LL18C:;_LL18D: goto _LL187;_LL187:;}goto _LL17E;}_LL181:
_tmp203=_tmp1F9.f1;if(_tmp203 <= (void*)4)goto _LL183;if(*((int*)_tmp203)!= 7)goto
_LL183;_tmp204=((struct Cyc_Absyn_ArrayType_struct*)_tmp203)->f1;_tmp205=(void*)
_tmp204.elt_type;_tmp206=_tmp1F9.f2;if(_tmp206 <= (void*)10)goto _LL183;if(*((int*)
_tmp206)!= 2)goto _LL183;_tmp207=(void*)((struct Cyc_Port_Array_ct_struct*)_tmp206)->f1;
_LL182: Cyc_Port_register_rgns(env,counts,fn_res,_tmp205,_tmp207);goto _LL17E;
_LL183: _tmp208=_tmp1F9.f1;if(_tmp208 <= (void*)4)goto _LL185;if(*((int*)_tmp208)!= 
8)goto _LL185;_tmp209=((struct Cyc_Absyn_FnType_struct*)_tmp208)->f1;_tmp20A=(void*)
_tmp209.ret_typ;_tmp20B=_tmp209.args;_tmp20C=_tmp1F9.f2;if(_tmp20C <= (void*)10)
goto _LL185;if(*((int*)_tmp20C)!= 5)goto _LL185;_tmp20D=(void*)((struct Cyc_Port_Fn_ct_struct*)
_tmp20C)->f1;_tmp20E=((struct Cyc_Port_Fn_ct_struct*)_tmp20C)->f2;_LL184: Cyc_Port_register_rgns(
env,counts,1,_tmp20A,_tmp20D);for(0;_tmp20B != 0  && _tmp20E != 0;(_tmp20B=_tmp20B->tl,
_tmp20E=_tmp20E->tl)){void*_tmp223;struct _tuple1 _tmp222=*((struct _tuple1*)
_tmp20B->hd);_tmp223=_tmp222.f3;{void*_tmp224=(void*)_tmp20E->hd;Cyc_Port_register_rgns(
env,counts,0,_tmp223,_tmp224);}}goto _LL17E;_LL185:;_LL186: goto _LL17E;_LL17E:;}}
static void*Cyc_Port_gen_exp(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e);
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s);
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct
Cyc_Absyn_Decl*d);static int Cyc_Port_is_char(struct Cyc_Port_Cenv*env,void*t);
static int Cyc_Port_is_char(struct Cyc_Port_Cenv*env,void*t){void*_tmp225=t;struct
_tuple0*_tmp226;void*_tmp227;_LL18F: if(_tmp225 <= (void*)4)goto _LL193;if(*((int*)
_tmp225)!= 16)goto _LL191;_tmp226=((struct Cyc_Absyn_TypedefType_struct*)_tmp225)->f1;
_LL190:{union Cyc_Absyn_Nmspace_union _tmp4BF;(*_tmp226).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp4BF.Loc_n).tag=0,_tmp4BF));}return Cyc_Port_is_char(env,Cyc_Port_lookup_typedef(
env,_tmp226));_LL191: if(*((int*)_tmp225)!= 5)goto _LL193;_tmp227=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp225)->f2;_LL192: return 1;_LL193:;_LL194: return 0;
_LL18E:;}static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t);
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t){void*_tmp229=t;
struct _tuple0*_tmp22A;struct Cyc_Absyn_PtrInfo _tmp22B;void*_tmp22C;struct Cyc_Absyn_Tqual
_tmp22D;struct Cyc_Absyn_PtrAtts _tmp22E;void*_tmp22F;struct Cyc_Absyn_Conref*
_tmp230;struct Cyc_Absyn_Conref*_tmp231;struct Cyc_Absyn_Conref*_tmp232;struct Cyc_Absyn_PtrLoc*
_tmp233;struct Cyc_Absyn_ArrayInfo _tmp234;void*_tmp235;struct Cyc_Absyn_Tqual
_tmp236;struct Cyc_Absyn_Conref*_tmp237;struct Cyc_Position_Segment*_tmp238;struct
Cyc_Absyn_FnInfo _tmp239;void*_tmp23A;struct Cyc_List_List*_tmp23B;struct Cyc_Absyn_AggrInfo
_tmp23C;union Cyc_Absyn_AggrInfoU_union _tmp23D;struct Cyc_List_List*_tmp23E;_LL196:
if(_tmp229 <= (void*)4)goto _LL198;if(*((int*)_tmp229)!= 16)goto _LL198;_tmp22A=((
struct Cyc_Absyn_TypedefType_struct*)_tmp229)->f1;_LL197:{union Cyc_Absyn_Nmspace_union
_tmp4C0;(*_tmp22A).f1=(union Cyc_Absyn_Nmspace_union)(((_tmp4C0.Loc_n).tag=0,
_tmp4C0));}return Cyc_Port_lookup_typedef_ctype(env,_tmp22A);_LL198: if((int)
_tmp229 != 0)goto _LL19A;_LL199: return Cyc_Port_void_ct();_LL19A: if(_tmp229 <= (void*)
4)goto _LL19E;if(*((int*)_tmp229)!= 4)goto _LL19C;_tmp22B=((struct Cyc_Absyn_PointerType_struct*)
_tmp229)->f1;_tmp22C=(void*)_tmp22B.elt_typ;_tmp22D=_tmp22B.elt_tq;_tmp22E=
_tmp22B.ptr_atts;_tmp22F=(void*)_tmp22E.rgn;_tmp230=_tmp22E.nullable;_tmp231=
_tmp22E.bounds;_tmp232=_tmp22E.zero_term;_tmp233=_tmp22E.ptrloc;_LL19B: {struct
Cyc_Position_Segment*_tmp240=(unsigned int)_tmp233?_tmp233->ptr_loc: 0;struct Cyc_Position_Segment*
_tmp241=(unsigned int)_tmp233?_tmp233->rgn_loc: 0;struct Cyc_Position_Segment*
_tmp242=(unsigned int)_tmp233?_tmp233->zt_loc: 0;void*_tmp243=Cyc_Port_type_to_ctype(
env,_tmp22C);void*new_rgn;{void*_tmp244=_tmp22F;struct Cyc_Absyn_Tvar*_tmp245;
struct Cyc_Absyn_Tvar _tmp246;struct _dyneither_ptr*_tmp247;_LL1AF: if((int)_tmp244
!= 2)goto _LL1B1;_LL1B0: new_rgn=Cyc_Port_heap_ct();goto _LL1AE;_LL1B1: if(_tmp244 <= (
void*)4)goto _LL1B5;if(*((int*)_tmp244)!= 1)goto _LL1B3;_tmp245=((struct Cyc_Absyn_VarType_struct*)
_tmp244)->f1;_tmp246=*_tmp245;_tmp247=_tmp246.name;_LL1B2: new_rgn=Cyc_Port_rgnvar_ct(
_tmp247);goto _LL1AE;_LL1B3: if(*((int*)_tmp244)!= 0)goto _LL1B5;_LL1B4: if(Cyc_Port_in_fn_arg(
env))new_rgn=Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var());else{if(Cyc_Port_in_fn_res(
env) || Cyc_Port_in_toplevel(env))new_rgn=Cyc_Port_heap_ct();else{new_rgn=Cyc_Port_var();}}
goto _LL1AE;_LL1B5:;_LL1B6: new_rgn=Cyc_Port_heap_ct();goto _LL1AE;_LL1AE:;}{int
_tmp248=Cyc_Absyn_conref_val(_tmp231)== (void*)0;int orig_zt;{union Cyc_Absyn_Constraint_union
_tmp249=_tmp232->v;_LL1B8: if((_tmp249.No_constr).tag != 2)goto _LL1BA;_LL1B9:
orig_zt=Cyc_Port_is_char(env,_tmp22C);goto _LL1B7;_LL1BA:;_LL1BB: orig_zt=((int(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp232);goto _LL1B7;_LL1B7:;}if((
env->gcenv)->porting){void*_tmp24A=Cyc_Port_var();Cyc_Port_register_const_cvar(
env,_tmp24A,_tmp22D.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),
_tmp22D.loc);if(_tmp22D.print_const)Cyc_Port_unify_ct(_tmp24A,Cyc_Port_const_ct());{
void*_tmp24B=Cyc_Port_var();Cyc_Port_register_ptr_cvars(env,_tmp24B,_tmp248?Cyc_Port_fat_ct():
Cyc_Port_thin_ct(),_tmp240);if(_tmp248)Cyc_Port_unify_ct(_tmp24B,Cyc_Port_fat_ct());{
void*_tmp24C=Cyc_Port_var();Cyc_Port_register_zeroterm_cvars(env,_tmp24C,orig_zt?
Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct(),_tmp242);{union Cyc_Absyn_Constraint_union
_tmp24D=_tmp232->v;_LL1BD: if((_tmp24D.No_constr).tag != 2)goto _LL1BF;_LL1BE: goto
_LL1BC;_LL1BF:;_LL1C0: Cyc_Port_unify_ct(_tmp24C,((int(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_val)(_tmp232)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());
goto _LL1BC;_LL1BC:;}return Cyc_Port_ptr_ct(_tmp243,_tmp24A,_tmp24B,new_rgn,
_tmp24C);}}}else{return Cyc_Port_ptr_ct(_tmp243,_tmp22D.print_const?Cyc_Port_const_ct():
Cyc_Port_notconst_ct(),_tmp248?Cyc_Port_fat_ct(): Cyc_Port_thin_ct(),new_rgn,
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());}}}_LL19C: if(*((int*)_tmp229)
!= 5)goto _LL19E;_LL19D: goto _LL19F;_LL19E: if((int)_tmp229 != 1)goto _LL1A0;_LL19F:
goto _LL1A1;_LL1A0: if(_tmp229 <= (void*)4)goto _LL1AC;if(*((int*)_tmp229)!= 6)goto
_LL1A2;_LL1A1: return Cyc_Port_arith_ct();_LL1A2: if(*((int*)_tmp229)!= 7)goto
_LL1A4;_tmp234=((struct Cyc_Absyn_ArrayType_struct*)_tmp229)->f1;_tmp235=(void*)
_tmp234.elt_type;_tmp236=_tmp234.tq;_tmp237=_tmp234.zero_term;_tmp238=_tmp234.zt_loc;
_LL1A3: {void*_tmp24E=Cyc_Port_type_to_ctype(env,_tmp235);int orig_zt;{union Cyc_Absyn_Constraint_union
_tmp24F=_tmp237->v;_LL1C2: if((_tmp24F.No_constr).tag != 2)goto _LL1C4;_LL1C3:
orig_zt=0;goto _LL1C1;_LL1C4:;_LL1C5: orig_zt=((int(*)(struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_val)(_tmp237);goto _LL1C1;_LL1C1:;}if((env->gcenv)->porting){
void*_tmp250=Cyc_Port_var();Cyc_Port_register_const_cvar(env,_tmp250,_tmp236.print_const?
Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),_tmp236.loc);{void*_tmp251=Cyc_Port_var();
Cyc_Port_register_zeroterm_cvars(env,_tmp251,orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct(),
_tmp238);{union Cyc_Absyn_Constraint_union _tmp252=_tmp237->v;_LL1C7: if((_tmp252.No_constr).tag
!= 2)goto _LL1C9;_LL1C8: goto _LL1C6;_LL1C9:;_LL1CA: Cyc_Port_unify_ct(_tmp251,((int(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp237)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());
goto _LL1C6;_LL1C6:;}return Cyc_Port_array_ct(_tmp24E,_tmp250,_tmp251);}}else{
return Cyc_Port_array_ct(_tmp24E,_tmp236.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());}}_LL1A4: if(*((int*)_tmp229)
!= 8)goto _LL1A6;_tmp239=((struct Cyc_Absyn_FnType_struct*)_tmp229)->f1;_tmp23A=(
void*)_tmp239.ret_typ;_tmp23B=_tmp239.args;_LL1A5: {void*_tmp253=Cyc_Port_type_to_ctype(
Cyc_Port_set_cpos(env,(void*)0),_tmp23A);struct Cyc_Port_Cenv*_tmp254=Cyc_Port_set_cpos(
env,(void*)1);struct Cyc_List_List*cargs=0;for(0;_tmp23B != 0;_tmp23B=_tmp23B->tl){
struct Cyc_Absyn_Tqual _tmp256;void*_tmp257;struct _tuple1 _tmp255=*((struct _tuple1*)
_tmp23B->hd);_tmp256=_tmp255.f2;_tmp257=_tmp255.f3;{struct Cyc_List_List*_tmp4C1;
cargs=((_tmp4C1=_cycalloc(sizeof(*_tmp4C1)),((_tmp4C1->hd=(void*)Cyc_Port_type_to_ctype(
_tmp254,_tmp257),((_tmp4C1->tl=cargs,_tmp4C1))))));}}return Cyc_Port_fn_ct(
_tmp253,Cyc_List_imp_rev(cargs));}_LL1A6: if(*((int*)_tmp229)!= 10)goto _LL1A8;
_tmp23C=((struct Cyc_Absyn_AggrType_struct*)_tmp229)->f1;_tmp23D=_tmp23C.aggr_info;
_LL1A7: {union Cyc_Absyn_AggrInfoU_union _tmp259=_tmp23D;void*_tmp25A;struct
_tuple0*_tmp25B;void*_tmp25C;struct _tuple0*_tmp25D;struct Cyc_Absyn_Aggrdecl**
_tmp25E;_LL1CC: if((_tmp259.UnknownAggr).tag != 0)goto _LL1CE;_tmp25A=(_tmp259.UnknownAggr).f1;
if((int)_tmp25A != 0)goto _LL1CE;_tmp25B=(_tmp259.UnknownAggr).f2;_LL1CD:{union Cyc_Absyn_Nmspace_union
_tmp4C2;(*_tmp25B).f1=(union Cyc_Absyn_Nmspace_union)(((_tmp4C2.Loc_n).tag=0,
_tmp4C2));}{struct _tuple4*_tmp260=Cyc_Port_lookup_struct_decl(env,_tmp25B);
return Cyc_Port_known_aggr_ct(_tmp260);}_LL1CE: if((_tmp259.UnknownAggr).tag != 0)
goto _LL1D0;_tmp25C=(_tmp259.UnknownAggr).f1;if((int)_tmp25C != 1)goto _LL1D0;
_tmp25D=(_tmp259.UnknownAggr).f2;_LL1CF:{union Cyc_Absyn_Nmspace_union _tmp4C3;(*
_tmp25D).f1=(union Cyc_Absyn_Nmspace_union)(((_tmp4C3.Loc_n).tag=0,_tmp4C3));}{
struct _tuple4*_tmp262=Cyc_Port_lookup_union_decl(env,_tmp25D);return Cyc_Port_known_aggr_ct(
_tmp262);}_LL1D0: if((_tmp259.KnownAggr).tag != 1)goto _LL1CB;_tmp25E=(_tmp259.KnownAggr).f1;
_LL1D1: {struct Cyc_Absyn_Aggrdecl*_tmp263=*_tmp25E;void*_tmp264=(void*)_tmp263->kind;
_LL1D3: if((int)_tmp264 != 0)goto _LL1D5;_LL1D4: {struct _tuple4*_tmp265=Cyc_Port_lookup_struct_decl(
env,_tmp263->name);return Cyc_Port_known_aggr_ct(_tmp265);}_LL1D5: if((int)_tmp264
!= 1)goto _LL1D2;_LL1D6: {struct _tuple4*_tmp266=Cyc_Port_lookup_union_decl(env,
_tmp263->name);return Cyc_Port_known_aggr_ct(_tmp266);}_LL1D2:;}_LL1CB:;}_LL1A8:
if(*((int*)_tmp229)!= 12)goto _LL1AA;_LL1A9: return Cyc_Port_arith_ct();_LL1AA: if(*((
int*)_tmp229)!= 13)goto _LL1AC;_tmp23E=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp229)->f1;_LL1AB: for(0;(unsigned int)_tmp23E;_tmp23E=_tmp23E->tl){{union Cyc_Absyn_Nmspace_union
_tmp4C4;(*((struct Cyc_Absyn_Enumfield*)_tmp23E->hd)->name).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp4C4.Loc_n).tag=0,_tmp4C4));}env=Cyc_Port_add_var(env,((struct Cyc_Absyn_Enumfield*)
_tmp23E->hd)->name,Cyc_Absyn_sint_typ,Cyc_Port_const_ct(),Cyc_Port_arith_ct());}
return Cyc_Port_arith_ct();_LL1AC:;_LL1AD: return Cyc_Port_arith_ct();_LL195:;}
static void*Cyc_Port_gen_primop(struct Cyc_Port_Cenv*env,void*p,struct Cyc_List_List*
args);static void*Cyc_Port_gen_primop(struct Cyc_Port_Cenv*env,void*p,struct Cyc_List_List*
args){void*_tmp268=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)
_check_null(args))->hd);struct Cyc_List_List*_tmp269=args->tl;void*_tmp26A=p;
_LL1D8: if((int)_tmp26A != 0)goto _LL1DA;_LL1D9: {void*_tmp26B=Cyc_Port_compress_ct((
void*)((struct Cyc_List_List*)_check_null(_tmp269))->hd);if(Cyc_Port_leq(_tmp268,
Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),
Cyc_Port_var()))){Cyc_Port_leq(_tmp26B,Cyc_Port_arith_ct());return _tmp268;}else{
if(Cyc_Port_leq(_tmp26B,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_var(),Cyc_Port_var()))){Cyc_Port_leq(_tmp268,Cyc_Port_arith_ct());
return _tmp26B;}else{Cyc_Port_leq(_tmp268,Cyc_Port_arith_ct());Cyc_Port_leq(
_tmp26B,Cyc_Port_arith_ct());return Cyc_Port_arith_ct();}}}_LL1DA: if((int)_tmp26A
!= 2)goto _LL1DC;_LL1DB: if(_tmp269 == 0){Cyc_Port_leq(_tmp268,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();}else{void*_tmp26C=Cyc_Port_compress_ct((void*)_tmp269->hd);
if(Cyc_Port_leq(_tmp268,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_var(),Cyc_Port_var()))){if(Cyc_Port_leq(_tmp26C,Cyc_Port_ptr_ct(Cyc_Port_var(),
Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var())))return Cyc_Port_arith_ct();
else{Cyc_Port_leq(_tmp26C,Cyc_Port_arith_ct());return _tmp268;}}else{Cyc_Port_leq(
_tmp268,Cyc_Port_arith_ct());Cyc_Port_leq(_tmp268,Cyc_Port_arith_ct());return Cyc_Port_arith_ct();}}
_LL1DC: if((int)_tmp26A != 1)goto _LL1DE;_LL1DD: goto _LL1DF;_LL1DE: if((int)_tmp26A != 
3)goto _LL1E0;_LL1DF: goto _LL1E1;_LL1E0: if((int)_tmp26A != 4)goto _LL1E2;_LL1E1: goto
_LL1E3;_LL1E2: if((int)_tmp26A != 5)goto _LL1E4;_LL1E3: goto _LL1E5;_LL1E4: if((int)
_tmp26A != 6)goto _LL1E6;_LL1E5: goto _LL1E7;_LL1E6: if((int)_tmp26A != 8)goto _LL1E8;
_LL1E7: goto _LL1E9;_LL1E8: if((int)_tmp26A != 7)goto _LL1EA;_LL1E9: goto _LL1EB;_LL1EA:
if((int)_tmp26A != 10)goto _LL1EC;_LL1EB: goto _LL1ED;_LL1EC: if((int)_tmp26A != 9)
goto _LL1EE;_LL1ED: goto _LL1EF;_LL1EE: if((int)_tmp26A != 13)goto _LL1F0;_LL1EF: goto
_LL1F1;_LL1F0: if((int)_tmp26A != 14)goto _LL1F2;_LL1F1: goto _LL1F3;_LL1F2: if((int)
_tmp26A != 15)goto _LL1F4;_LL1F3: goto _LL1F5;_LL1F4: if((int)_tmp26A != 16)goto _LL1F6;
_LL1F5: goto _LL1F7;_LL1F6: if((int)_tmp26A != 17)goto _LL1F8;_LL1F7: goto _LL1F9;
_LL1F8: if((int)_tmp26A != 18)goto _LL1FA;_LL1F9: Cyc_Port_leq(_tmp268,Cyc_Port_arith_ct());
Cyc_Port_leq((void*)((struct Cyc_List_List*)_check_null(_tmp269))->hd,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();_LL1FA: if((int)_tmp26A != 11)goto _LL1FC;_LL1FB: goto
_LL1FD;_LL1FC: if((int)_tmp26A != 12)goto _LL1FE;_LL1FD: Cyc_Port_leq(_tmp268,Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();_LL1FE:;_LL1FF: {const char*_tmp4C7;void*_tmp4C6;(
_tmp4C6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp4C7=".size primop",_tag_dyneither(_tmp4C7,sizeof(char),13))),_tag_dyneither(
_tmp4C6,sizeof(void*),0)));}_LL1D7:;}static struct _tuple3 Cyc_Port_gen_lhs(struct
Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e);static struct _tuple3 Cyc_Port_gen_lhs(
struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){void*_tmp26F=(void*)e->r;struct
_tuple0*_tmp270;struct _tuple0*_tmp271;struct Cyc_Absyn_Exp*_tmp272;struct Cyc_Absyn_Exp*
_tmp273;struct Cyc_Absyn_Exp*_tmp274;struct Cyc_Absyn_Exp*_tmp275;struct
_dyneither_ptr*_tmp276;struct Cyc_Absyn_Exp*_tmp277;struct _dyneither_ptr*_tmp278;
_LL201: if(*((int*)_tmp26F)!= 1)goto _LL203;_tmp270=((struct Cyc_Absyn_Var_e_struct*)
_tmp26F)->f1;_LL202: _tmp271=_tmp270;goto _LL204;_LL203: if(*((int*)_tmp26F)!= 2)
goto _LL205;_tmp271=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp26F)->f1;_LL204:{
union Cyc_Absyn_Nmspace_union _tmp4C8;(*_tmp271).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp4C8.Loc_n).tag=0,_tmp4C8));}return Cyc_Port_lookup_var(env,_tmp271);_LL205:
if(*((int*)_tmp26F)!= 25)goto _LL207;_tmp272=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp26F)->f1;_tmp273=((struct Cyc_Absyn_Subscript_e_struct*)_tmp26F)->f2;_LL206: {
void*_tmp27A=Cyc_Port_var();void*_tmp27B=Cyc_Port_var();void*_tmp27C=Cyc_Port_gen_exp(
env,_tmp272);Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp273),Cyc_Port_arith_ct());{
void*_tmp27D=Cyc_Port_ptr_ct(_tmp27A,_tmp27B,Cyc_Port_fat_ct(),Cyc_Port_var(),
Cyc_Port_var());Cyc_Port_leq(_tmp27C,_tmp27D);{struct _tuple3 _tmp4C9;return(
_tmp4C9.f1=_tmp27B,((_tmp4C9.f2=_tmp27A,_tmp4C9)));}}}_LL207: if(*((int*)_tmp26F)
!= 22)goto _LL209;_tmp274=((struct Cyc_Absyn_Deref_e_struct*)_tmp26F)->f1;_LL208: {
void*_tmp27F=Cyc_Port_var();void*_tmp280=Cyc_Port_var();void*_tmp281=Cyc_Port_ptr_ct(
_tmp27F,_tmp280,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var());Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp274),_tmp281);{struct _tuple3 _tmp4CA;return(_tmp4CA.f1=_tmp280,((_tmp4CA.f2=
_tmp27F,_tmp4CA)));}}_LL209: if(*((int*)_tmp26F)!= 23)goto _LL20B;_tmp275=((struct
Cyc_Absyn_AggrMember_e_struct*)_tmp26F)->f1;_tmp276=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp26F)->f2;_LL20A: {void*_tmp283=Cyc_Port_var();void*_tmp284=Cyc_Port_var();
void*_tmp285=Cyc_Port_gen_exp(env,_tmp275);{struct Cyc_Port_Cfield*_tmp4CD;struct
Cyc_Port_Cfield*_tmp4CC[1];Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp275),Cyc_Port_unknown_aggr_ct(((
_tmp4CC[0]=((_tmp4CD=_cycalloc(sizeof(*_tmp4CD)),((_tmp4CD->qual=(void*)_tmp284,((
_tmp4CD->f=_tmp276,((_tmp4CD->type=(void*)_tmp283,_tmp4CD)))))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4CC,sizeof(struct Cyc_Port_Cfield*),
1))))));}{struct _tuple3 _tmp4CE;return(_tmp4CE.f1=_tmp284,((_tmp4CE.f2=_tmp283,
_tmp4CE)));}}_LL20B: if(*((int*)_tmp26F)!= 24)goto _LL20D;_tmp277=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp26F)->f1;_tmp278=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp26F)->f2;_LL20C: {
void*_tmp289=Cyc_Port_var();void*_tmp28A=Cyc_Port_var();void*_tmp28B=Cyc_Port_gen_exp(
env,_tmp277);{struct Cyc_Port_Cfield*_tmp4D1;struct Cyc_Port_Cfield*_tmp4D0[1];Cyc_Port_leq(
Cyc_Port_gen_exp(env,_tmp277),Cyc_Port_ptr_ct(Cyc_Port_unknown_aggr_ct(((_tmp4D0[
0]=((_tmp4D1=_cycalloc(sizeof(*_tmp4D1)),((_tmp4D1->qual=(void*)_tmp28A,((
_tmp4D1->f=_tmp278,((_tmp4D1->type=(void*)_tmp289,_tmp4D1)))))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4D0,sizeof(struct Cyc_Port_Cfield*),
1))))),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var()));}{struct
_tuple3 _tmp4D2;return(_tmp4D2.f1=_tmp28A,((_tmp4D2.f2=_tmp289,_tmp4D2)));}}
_LL20D:;_LL20E: {struct Cyc_String_pa_struct _tmp4DA;void*_tmp4D9[1];const char*
_tmp4D8;void*_tmp4D7;(_tmp4D7=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp4DA.tag=0,((
_tmp4DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmp4D9[0]=& _tmp4DA,Cyc_aprintf(((_tmp4D8="gen_lhs: %s",_tag_dyneither(_tmp4D8,
sizeof(char),12))),_tag_dyneither(_tmp4D9,sizeof(void*),1)))))))),_tag_dyneither(
_tmp4D7,sizeof(void*),0)));}_LL200:;}static void*Cyc_Port_gen_exp(struct Cyc_Port_Cenv*
env,struct Cyc_Absyn_Exp*e);static void*Cyc_Port_gen_exp(struct Cyc_Port_Cenv*env,
struct Cyc_Absyn_Exp*e){void*_tmp293=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp294;
union Cyc_Absyn_Cnst_union _tmp295;union Cyc_Absyn_Cnst_union _tmp296;union Cyc_Absyn_Cnst_union
_tmp297;union Cyc_Absyn_Cnst_union _tmp298;int _tmp299;union Cyc_Absyn_Cnst_union
_tmp29A;union Cyc_Absyn_Cnst_union _tmp29B;union Cyc_Absyn_Cnst_union _tmp29C;struct
_tuple0*_tmp29D;struct _tuple0*_tmp29E;void*_tmp29F;struct Cyc_List_List*_tmp2A0;
struct Cyc_Absyn_Exp*_tmp2A1;struct Cyc_Core_Opt*_tmp2A2;struct Cyc_Absyn_Exp*
_tmp2A3;struct Cyc_Absyn_Exp*_tmp2A4;void*_tmp2A5;struct Cyc_Absyn_Exp*_tmp2A6;
struct Cyc_Absyn_Exp*_tmp2A7;struct Cyc_Absyn_Exp*_tmp2A8;struct Cyc_Absyn_Exp*
_tmp2A9;struct Cyc_Absyn_Exp*_tmp2AA;struct Cyc_Absyn_Exp*_tmp2AB;struct Cyc_Absyn_Exp*
_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AD;struct Cyc_Absyn_Exp*_tmp2AE;struct Cyc_Absyn_Exp*
_tmp2AF;struct Cyc_List_List*_tmp2B0;struct Cyc_Absyn_Exp*_tmp2B1;struct Cyc_List_List*
_tmp2B2;struct Cyc_Absyn_Exp*_tmp2B3;void*_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B5;
struct Cyc_Absyn_Exp*_tmp2B6;struct Cyc_Absyn_Exp*_tmp2B7;struct Cyc_Absyn_Exp*
_tmp2B8;struct Cyc_Absyn_Exp*_tmp2B9;struct Cyc_Absyn_Exp*_tmp2BA;struct
_dyneither_ptr*_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BC;struct _dyneither_ptr*_tmp2BD;
struct Cyc_Absyn_MallocInfo _tmp2BE;void**_tmp2BF;struct Cyc_Absyn_Exp*_tmp2C0;
struct Cyc_Absyn_Exp*_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C2;struct Cyc_Absyn_Stmt*
_tmp2C3;_LL210: if(*((int*)_tmp293)!= 0)goto _LL212;_tmp294=((struct Cyc_Absyn_Const_e_struct*)
_tmp293)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp293)->f1).Char_c).tag != 
0)goto _LL212;_LL211: goto _LL213;_LL212: if(*((int*)_tmp293)!= 0)goto _LL214;_tmp295=((
struct Cyc_Absyn_Const_e_struct*)_tmp293)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp293)->f1).Short_c).tag != 1)goto _LL214;_LL213: goto _LL215;_LL214: if(*((int*)
_tmp293)!= 0)goto _LL216;_tmp296=((struct Cyc_Absyn_Const_e_struct*)_tmp293)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp293)->f1).LongLong_c).tag != 3)goto
_LL216;_LL215: goto _LL217;_LL216: if(*((int*)_tmp293)!= 18)goto _LL218;_LL217: goto
_LL219;_LL218: if(*((int*)_tmp293)!= 19)goto _LL21A;_LL219: goto _LL21B;_LL21A: if(*((
int*)_tmp293)!= 20)goto _LL21C;_LL21B: goto _LL21D;_LL21C: if(*((int*)_tmp293)!= 33)
goto _LL21E;_LL21D: goto _LL21F;_LL21E: if(*((int*)_tmp293)!= 34)goto _LL220;_LL21F:
goto _LL221;_LL220: if(*((int*)_tmp293)!= 0)goto _LL222;_tmp297=((struct Cyc_Absyn_Const_e_struct*)
_tmp293)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp293)->f1).Float_c).tag != 
4)goto _LL222;_LL221: return Cyc_Port_arith_ct();_LL222: if(*((int*)_tmp293)!= 0)
goto _LL224;_tmp298=((struct Cyc_Absyn_Const_e_struct*)_tmp293)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmp293)->f1).Int_c).tag != 2)goto _LL224;_tmp299=(
_tmp298.Int_c).f2;if(_tmp299 != 0)goto _LL224;_LL223: return Cyc_Port_zero_ct();
_LL224: if(*((int*)_tmp293)!= 0)goto _LL226;_tmp29A=((struct Cyc_Absyn_Const_e_struct*)
_tmp293)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp293)->f1).Int_c).tag != 2)
goto _LL226;_LL225: return Cyc_Port_arith_ct();_LL226: if(*((int*)_tmp293)!= 0)goto
_LL228;_tmp29B=((struct Cyc_Absyn_Const_e_struct*)_tmp293)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp293)->f1).String_c).tag != 5)goto _LL228;_LL227: return Cyc_Port_ptr_ct(Cyc_Port_arith_ct(),
Cyc_Port_const_ct(),Cyc_Port_fat_ct(),Cyc_Port_heap_ct(),Cyc_Port_zterm_ct());
_LL228: if(*((int*)_tmp293)!= 0)goto _LL22A;_tmp29C=((struct Cyc_Absyn_Const_e_struct*)
_tmp293)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp293)->f1).Null_c).tag != 
6)goto _LL22A;_LL229: return Cyc_Port_zero_ct();_LL22A: if(*((int*)_tmp293)!= 1)goto
_LL22C;_tmp29D=((struct Cyc_Absyn_Var_e_struct*)_tmp293)->f1;_LL22B: _tmp29E=
_tmp29D;goto _LL22D;_LL22C: if(*((int*)_tmp293)!= 2)goto _LL22E;_tmp29E=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp293)->f1;_LL22D:{union Cyc_Absyn_Nmspace_union _tmp4DB;(*_tmp29E).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp4DB.Loc_n).tag=0,_tmp4DB));}{void*_tmp2C6;struct _tuple3 _tmp2C5=Cyc_Port_lookup_var(
env,_tmp29E);_tmp2C6=_tmp2C5.f2;if(Cyc_Port_isfn(env,_tmp29E)){_tmp2C6=Cyc_Port_instantiate(
_tmp2C6);return Cyc_Port_ptr_ct(_tmp2C6,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_heap_ct(),
Cyc_Port_nozterm_ct());}else{if(Cyc_Port_isarray(env,_tmp29E)){void*_tmp2C7=Cyc_Port_var();
void*_tmp2C8=Cyc_Port_var();void*_tmp2C9=Cyc_Port_var();void*_tmp2CA=Cyc_Port_array_ct(
_tmp2C7,_tmp2C8,_tmp2C9);Cyc_Port_unifies(_tmp2C6,_tmp2CA);{void*_tmp2CB=Cyc_Port_ptr_ct(
_tmp2C7,_tmp2C8,Cyc_Port_fat_ct(),Cyc_Port_var(),_tmp2C9);return _tmp2CB;}}else{
return _tmp2C6;}}}_LL22E: if(*((int*)_tmp293)!= 3)goto _LL230;_tmp29F=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp293)->f1;_tmp2A0=((struct Cyc_Absyn_Primop_e_struct*)
_tmp293)->f2;_LL22F: return Cyc_Port_gen_primop(env,_tmp29F,((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp,env,_tmp2A0));_LL230: if(*((
int*)_tmp293)!= 4)goto _LL232;_tmp2A1=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp293)->f1;_tmp2A2=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp293)->f2;_tmp2A3=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp293)->f3;_LL231: {void*_tmp2CD;void*
_tmp2CE;struct _tuple3 _tmp2CC=Cyc_Port_gen_lhs(env,_tmp2A1);_tmp2CD=_tmp2CC.f1;
_tmp2CE=_tmp2CC.f2;Cyc_Port_unifies(_tmp2CD,Cyc_Port_notconst_ct());{void*
_tmp2CF=Cyc_Port_gen_exp(env,_tmp2A3);if((unsigned int)_tmp2A2){struct Cyc_List_List
_tmp4DC;struct Cyc_List_List x2=(_tmp4DC.hd=(void*)_tmp2CF,((_tmp4DC.tl=0,_tmp4DC)));
struct Cyc_List_List _tmp4DD;struct Cyc_List_List x1=(_tmp4DD.hd=(void*)_tmp2CE,((
_tmp4DD.tl=(struct Cyc_List_List*)& x2,_tmp4DD)));void*_tmp2D0=Cyc_Port_gen_primop(
env,(void*)_tmp2A2->v,(struct Cyc_List_List*)& x1);Cyc_Port_leq(_tmp2D0,_tmp2CE);
return _tmp2CE;}else{Cyc_Port_leq(_tmp2CF,_tmp2CE);return _tmp2CE;}}}_LL232: if(*((
int*)_tmp293)!= 5)goto _LL234;_tmp2A4=((struct Cyc_Absyn_Increment_e_struct*)
_tmp293)->f1;_tmp2A5=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp293)->f2;
_LL233: {void*_tmp2D4;void*_tmp2D5;struct _tuple3 _tmp2D3=Cyc_Port_gen_lhs(env,
_tmp2A4);_tmp2D4=_tmp2D3.f1;_tmp2D5=_tmp2D3.f2;Cyc_Port_unifies(_tmp2D4,Cyc_Port_notconst_ct());
Cyc_Port_leq(_tmp2D5,Cyc_Port_ptr_ct(Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_fat_ct(),
Cyc_Port_var(),Cyc_Port_var()));Cyc_Port_leq(_tmp2D5,Cyc_Port_arith_ct());return
_tmp2D5;}_LL234: if(*((int*)_tmp293)!= 6)goto _LL236;_tmp2A6=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp293)->f1;_tmp2A7=((struct Cyc_Absyn_Conditional_e_struct*)_tmp293)->f2;
_tmp2A8=((struct Cyc_Absyn_Conditional_e_struct*)_tmp293)->f3;_LL235: {void*
_tmp2D6=Cyc_Port_var();Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp2A6),Cyc_Port_arith_ct());
Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp2A7),_tmp2D6);Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp2A8),_tmp2D6);return _tmp2D6;}_LL236: if(*((int*)_tmp293)!= 7)goto _LL238;
_tmp2A9=((struct Cyc_Absyn_And_e_struct*)_tmp293)->f1;_tmp2AA=((struct Cyc_Absyn_And_e_struct*)
_tmp293)->f2;_LL237: _tmp2AB=_tmp2A9;_tmp2AC=_tmp2AA;goto _LL239;_LL238: if(*((int*)
_tmp293)!= 8)goto _LL23A;_tmp2AB=((struct Cyc_Absyn_Or_e_struct*)_tmp293)->f1;
_tmp2AC=((struct Cyc_Absyn_Or_e_struct*)_tmp293)->f2;_LL239: Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp2AB),Cyc_Port_arith_ct());Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp2AC),Cyc_Port_arith_ct());
return Cyc_Port_arith_ct();_LL23A: if(*((int*)_tmp293)!= 9)goto _LL23C;_tmp2AD=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp293)->f1;_tmp2AE=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp293)->f2;_LL23B: Cyc_Port_gen_exp(env,_tmp2AD);return Cyc_Port_gen_exp(env,
_tmp2AE);_LL23C: if(*((int*)_tmp293)!= 10)goto _LL23E;_tmp2AF=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp293)->f1;_tmp2B0=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp293)->f2;_LL23D:
_tmp2B1=_tmp2AF;_tmp2B2=_tmp2B0;goto _LL23F;_LL23E: if(*((int*)_tmp293)!= 11)goto
_LL240;_tmp2B1=((struct Cyc_Absyn_FnCall_e_struct*)_tmp293)->f1;_tmp2B2=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp293)->f2;_LL23F: {void*_tmp2D7=Cyc_Port_var();
void*_tmp2D8=Cyc_Port_gen_exp(env,_tmp2B1);struct Cyc_List_List*_tmp2D9=((struct
Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp,env,_tmp2B2);struct
Cyc_List_List*_tmp2DA=Cyc_List_map(Cyc_Port_new_var,_tmp2D9);Cyc_Port_unifies(
_tmp2D8,Cyc_Port_ptr_ct(Cyc_Port_fn_ct(_tmp2D7,_tmp2DA),Cyc_Port_var(),Cyc_Port_var(),
Cyc_Port_var(),Cyc_Port_nozterm_ct()));for(0;_tmp2D9 != 0;(_tmp2D9=_tmp2D9->tl,
_tmp2DA=_tmp2DA->tl)){Cyc_Port_leq((void*)_tmp2D9->hd,(void*)((struct Cyc_List_List*)
_check_null(_tmp2DA))->hd);}return _tmp2D7;}_LL240: if(*((int*)_tmp293)!= 12)goto
_LL242;_LL241: {const char*_tmp4E0;void*_tmp4DF;(_tmp4DF=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4E0="throw",
_tag_dyneither(_tmp4E0,sizeof(char),6))),_tag_dyneither(_tmp4DF,sizeof(void*),0)));}
_LL242: if(*((int*)_tmp293)!= 13)goto _LL244;_tmp2B3=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp293)->f1;_LL243: return Cyc_Port_gen_exp(env,_tmp2B3);_LL244: if(*((int*)
_tmp293)!= 14)goto _LL246;_LL245: {const char*_tmp4E3;void*_tmp4E2;(_tmp4E2=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp4E3="instantiate",_tag_dyneither(_tmp4E3,sizeof(char),12))),_tag_dyneither(
_tmp4E2,sizeof(void*),0)));}_LL246: if(*((int*)_tmp293)!= 15)goto _LL248;_tmp2B4=(
void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp293)->f1;_tmp2B5=((struct Cyc_Absyn_Cast_e_struct*)
_tmp293)->f2;_LL247: {void*_tmp2DF=Cyc_Port_type_to_ctype(env,_tmp2B4);void*
_tmp2E0=Cyc_Port_gen_exp(env,_tmp2B5);Cyc_Port_leq(_tmp2DF,_tmp2E0);return
_tmp2E0;}_LL248: if(*((int*)_tmp293)!= 16)goto _LL24A;_tmp2B6=((struct Cyc_Absyn_Address_e_struct*)
_tmp293)->f1;_LL249: {void*_tmp2E2;void*_tmp2E3;struct _tuple3 _tmp2E1=Cyc_Port_gen_lhs(
env,_tmp2B6);_tmp2E2=_tmp2E1.f1;_tmp2E3=_tmp2E1.f2;return Cyc_Port_ptr_ct(_tmp2E3,
_tmp2E2,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var());}_LL24A: if(*((int*)_tmp293)
!= 25)goto _LL24C;_tmp2B7=((struct Cyc_Absyn_Subscript_e_struct*)_tmp293)->f1;
_tmp2B8=((struct Cyc_Absyn_Subscript_e_struct*)_tmp293)->f2;_LL24B: {void*_tmp2E4=
Cyc_Port_var();Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp2B8),Cyc_Port_arith_ct());{
void*_tmp2E5=Cyc_Port_gen_exp(env,_tmp2B7);Cyc_Port_leq(_tmp2E5,Cyc_Port_ptr_ct(
_tmp2E4,Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_var(),Cyc_Port_var()));return
_tmp2E4;}}_LL24C: if(*((int*)_tmp293)!= 22)goto _LL24E;_tmp2B9=((struct Cyc_Absyn_Deref_e_struct*)
_tmp293)->f1;_LL24D: {void*_tmp2E6=Cyc_Port_var();Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp2B9),Cyc_Port_ptr_ct(_tmp2E6,Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),
Cyc_Port_var()));return _tmp2E6;}_LL24E: if(*((int*)_tmp293)!= 23)goto _LL250;
_tmp2BA=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp293)->f1;_tmp2BB=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp293)->f2;_LL24F: {void*_tmp2E7=Cyc_Port_var();void*_tmp2E8=Cyc_Port_gen_exp(
env,_tmp2BA);{struct Cyc_Port_Cfield*_tmp4E6;struct Cyc_Port_Cfield*_tmp4E5[1];Cyc_Port_leq(
Cyc_Port_gen_exp(env,_tmp2BA),Cyc_Port_unknown_aggr_ct(((_tmp4E5[0]=((_tmp4E6=
_cycalloc(sizeof(*_tmp4E6)),((_tmp4E6->qual=(void*)Cyc_Port_var(),((_tmp4E6->f=
_tmp2BB,((_tmp4E6->type=(void*)_tmp2E7,_tmp4E6)))))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4E5,sizeof(struct Cyc_Port_Cfield*),
1))))));}return _tmp2E7;}_LL250: if(*((int*)_tmp293)!= 24)goto _LL252;_tmp2BC=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp293)->f1;_tmp2BD=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp293)->f2;_LL251: {void*_tmp2EB=Cyc_Port_var();void*_tmp2EC=Cyc_Port_gen_exp(
env,_tmp2BC);{struct Cyc_Port_Cfield*_tmp4E9;struct Cyc_Port_Cfield*_tmp4E8[1];Cyc_Port_leq(
Cyc_Port_gen_exp(env,_tmp2BC),Cyc_Port_ptr_ct(Cyc_Port_unknown_aggr_ct(((_tmp4E8[
0]=((_tmp4E9=_cycalloc(sizeof(*_tmp4E9)),((_tmp4E9->qual=(void*)Cyc_Port_var(),((
_tmp4E9->f=_tmp2BD,((_tmp4E9->type=(void*)_tmp2EB,_tmp4E9)))))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4E8,sizeof(struct Cyc_Port_Cfield*),
1))))),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var(),Cyc_Port_var()));}return
_tmp2EB;}_LL252: if(*((int*)_tmp293)!= 35)goto _LL254;_tmp2BE=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp293)->f1;_tmp2BF=_tmp2BE.elt_type;_tmp2C0=_tmp2BE.num_elts;_LL253: Cyc_Port_leq(
Cyc_Port_gen_exp(env,_tmp2C0),Cyc_Port_arith_ct());{void*_tmp2EF=(unsigned int)
_tmp2BF?Cyc_Port_type_to_ctype(env,*_tmp2BF): Cyc_Port_var();return Cyc_Port_ptr_ct(
_tmp2EF,Cyc_Port_var(),Cyc_Port_fat_ct(),Cyc_Port_heap_ct(),Cyc_Port_var());}
_LL254: if(*((int*)_tmp293)!= 36)goto _LL256;_tmp2C1=((struct Cyc_Absyn_Swap_e_struct*)
_tmp293)->f1;_tmp2C2=((struct Cyc_Absyn_Swap_e_struct*)_tmp293)->f2;_LL255: {
const char*_tmp4EC;void*_tmp4EB;(_tmp4EB=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4EC="Swap_e",_tag_dyneither(
_tmp4EC,sizeof(char),7))),_tag_dyneither(_tmp4EB,sizeof(void*),0)));}_LL256: if(*((
int*)_tmp293)!= 21)goto _LL258;_LL257: {const char*_tmp4EF;void*_tmp4EE;(_tmp4EE=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp4EF="Gen",_tag_dyneither(_tmp4EF,sizeof(char),4))),_tag_dyneither(_tmp4EE,
sizeof(void*),0)));}_LL258: if(*((int*)_tmp293)!= 17)goto _LL25A;_LL259: {const
char*_tmp4F2;void*_tmp4F1;(_tmp4F1=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4F2="New_e",_tag_dyneither(_tmp4F2,
sizeof(char),6))),_tag_dyneither(_tmp4F1,sizeof(void*),0)));}_LL25A: if(*((int*)
_tmp293)!= 26)goto _LL25C;_LL25B: {const char*_tmp4F5;void*_tmp4F4;(_tmp4F4=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp4F5="Tuple_e",_tag_dyneither(_tmp4F5,sizeof(char),8))),_tag_dyneither(
_tmp4F4,sizeof(void*),0)));}_LL25C: if(*((int*)_tmp293)!= 27)goto _LL25E;_LL25D: {
const char*_tmp4F8;void*_tmp4F7;(_tmp4F7=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4F8="CompoundLit_e",
_tag_dyneither(_tmp4F8,sizeof(char),14))),_tag_dyneither(_tmp4F7,sizeof(void*),0)));}
_LL25E: if(*((int*)_tmp293)!= 28)goto _LL260;_LL25F: {const char*_tmp4FB;void*
_tmp4FA;(_tmp4FA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp4FB="Array_e",_tag_dyneither(_tmp4FB,sizeof(char),8))),_tag_dyneither(
_tmp4FA,sizeof(void*),0)));}_LL260: if(*((int*)_tmp293)!= 29)goto _LL262;_LL261: {
const char*_tmp4FE;void*_tmp4FD;(_tmp4FD=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4FE="Comprehension_e",
_tag_dyneither(_tmp4FE,sizeof(char),16))),_tag_dyneither(_tmp4FD,sizeof(void*),0)));}
_LL262: if(*((int*)_tmp293)!= 30)goto _LL264;_LL263: {const char*_tmp501;void*
_tmp500;(_tmp500=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp501="Struct_e",_tag_dyneither(_tmp501,sizeof(char),9))),_tag_dyneither(
_tmp500,sizeof(void*),0)));}_LL264: if(*((int*)_tmp293)!= 31)goto _LL266;_LL265: {
const char*_tmp504;void*_tmp503;(_tmp503=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp504="AnonStruct_e",
_tag_dyneither(_tmp504,sizeof(char),13))),_tag_dyneither(_tmp503,sizeof(void*),0)));}
_LL266: if(*((int*)_tmp293)!= 32)goto _LL268;_LL267: {const char*_tmp507;void*
_tmp506;(_tmp506=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp507="Tunion_e",_tag_dyneither(_tmp507,sizeof(char),9))),_tag_dyneither(
_tmp506,sizeof(void*),0)));}_LL268: if(*((int*)_tmp293)!= 37)goto _LL26A;_LL269: {
const char*_tmp50A;void*_tmp509;(_tmp509=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp50A="UnresolvedMem_e",
_tag_dyneither(_tmp50A,sizeof(char),16))),_tag_dyneither(_tmp509,sizeof(void*),0)));}
_LL26A: if(*((int*)_tmp293)!= 38)goto _LL26C;_tmp2C3=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp293)->f1;_LL26B: {const char*_tmp50D;void*_tmp50C;(_tmp50C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp50D="StmtExp_e",
_tag_dyneither(_tmp50D,sizeof(char),10))),_tag_dyneither(_tmp50C,sizeof(void*),0)));}
_LL26C: if(*((int*)_tmp293)!= 39)goto _LL20F;_LL26D: {const char*_tmp510;void*
_tmp50F;(_tmp50F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp510="Valueof_e",_tag_dyneither(_tmp510,sizeof(char),10))),_tag_dyneither(
_tmp50F,sizeof(void*),0)));}_LL20F:;}static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*
env,struct Cyc_Absyn_Stmt*s);static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,
struct Cyc_Absyn_Stmt*s){void*_tmp30A=(void*)s->r;struct Cyc_Absyn_Exp*_tmp30B;
struct Cyc_Absyn_Stmt*_tmp30C;struct Cyc_Absyn_Stmt*_tmp30D;struct Cyc_Absyn_Exp*
_tmp30E;struct Cyc_Absyn_Exp*_tmp30F;struct Cyc_Absyn_Stmt*_tmp310;struct Cyc_Absyn_Stmt*
_tmp311;struct _tuple2 _tmp312;struct Cyc_Absyn_Exp*_tmp313;struct Cyc_Absyn_Stmt*
_tmp314;struct Cyc_Absyn_Exp*_tmp315;struct _tuple2 _tmp316;struct Cyc_Absyn_Exp*
_tmp317;struct _tuple2 _tmp318;struct Cyc_Absyn_Exp*_tmp319;struct Cyc_Absyn_Stmt*
_tmp31A;struct Cyc_Absyn_Exp*_tmp31B;struct Cyc_List_List*_tmp31C;struct Cyc_Absyn_Decl*
_tmp31D;struct Cyc_Absyn_Stmt*_tmp31E;struct Cyc_Absyn_Stmt*_tmp31F;struct Cyc_Absyn_Stmt*
_tmp320;struct _tuple2 _tmp321;struct Cyc_Absyn_Exp*_tmp322;_LL26F: if((int)_tmp30A
!= 0)goto _LL271;_LL270: return;_LL271: if(_tmp30A <= (void*)1)goto _LL273;if(*((int*)
_tmp30A)!= 0)goto _LL273;_tmp30B=((struct Cyc_Absyn_Exp_s_struct*)_tmp30A)->f1;
_LL272: Cyc_Port_gen_exp(env,_tmp30B);return;_LL273: if(_tmp30A <= (void*)1)goto
_LL275;if(*((int*)_tmp30A)!= 1)goto _LL275;_tmp30C=((struct Cyc_Absyn_Seq_s_struct*)
_tmp30A)->f1;_tmp30D=((struct Cyc_Absyn_Seq_s_struct*)_tmp30A)->f2;_LL274: Cyc_Port_gen_stmt(
env,_tmp30C);Cyc_Port_gen_stmt(env,_tmp30D);return;_LL275: if(_tmp30A <= (void*)1)
goto _LL277;if(*((int*)_tmp30A)!= 2)goto _LL277;_tmp30E=((struct Cyc_Absyn_Return_s_struct*)
_tmp30A)->f1;_LL276: {void*_tmp323=Cyc_Port_lookup_return_type(env);void*_tmp324=(
unsigned int)_tmp30E?Cyc_Port_gen_exp(env,(struct Cyc_Absyn_Exp*)_tmp30E): Cyc_Port_void_ct();
Cyc_Port_leq(_tmp324,_tmp323);return;}_LL277: if(_tmp30A <= (void*)1)goto _LL279;
if(*((int*)_tmp30A)!= 3)goto _LL279;_tmp30F=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp30A)->f1;_tmp310=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp30A)->f2;_tmp311=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp30A)->f3;_LL278: Cyc_Port_leq(Cyc_Port_gen_exp(
env,_tmp30F),Cyc_Port_arith_ct());Cyc_Port_gen_stmt(env,_tmp310);Cyc_Port_gen_stmt(
env,_tmp311);return;_LL279: if(_tmp30A <= (void*)1)goto _LL27B;if(*((int*)_tmp30A)
!= 4)goto _LL27B;_tmp312=((struct Cyc_Absyn_While_s_struct*)_tmp30A)->f1;_tmp313=
_tmp312.f1;_tmp314=((struct Cyc_Absyn_While_s_struct*)_tmp30A)->f2;_LL27A: Cyc_Port_leq(
Cyc_Port_gen_exp(env,_tmp313),Cyc_Port_arith_ct());Cyc_Port_gen_stmt(env,_tmp314);
return;_LL27B: if(_tmp30A <= (void*)1)goto _LL27D;if(*((int*)_tmp30A)!= 5)goto
_LL27D;_LL27C: goto _LL27E;_LL27D: if(_tmp30A <= (void*)1)goto _LL27F;if(*((int*)
_tmp30A)!= 6)goto _LL27F;_LL27E: goto _LL280;_LL27F: if(_tmp30A <= (void*)1)goto
_LL281;if(*((int*)_tmp30A)!= 7)goto _LL281;_LL280: return;_LL281: if(_tmp30A <= (
void*)1)goto _LL283;if(*((int*)_tmp30A)!= 8)goto _LL283;_tmp315=((struct Cyc_Absyn_For_s_struct*)
_tmp30A)->f1;_tmp316=((struct Cyc_Absyn_For_s_struct*)_tmp30A)->f2;_tmp317=
_tmp316.f1;_tmp318=((struct Cyc_Absyn_For_s_struct*)_tmp30A)->f3;_tmp319=_tmp318.f1;
_tmp31A=((struct Cyc_Absyn_For_s_struct*)_tmp30A)->f4;_LL282: Cyc_Port_gen_exp(env,
_tmp315);Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp317),Cyc_Port_arith_ct());Cyc_Port_gen_exp(
env,_tmp319);Cyc_Port_gen_stmt(env,_tmp31A);return;_LL283: if(_tmp30A <= (void*)1)
goto _LL285;if(*((int*)_tmp30A)!= 9)goto _LL285;_tmp31B=((struct Cyc_Absyn_Switch_s_struct*)
_tmp30A)->f1;_tmp31C=((struct Cyc_Absyn_Switch_s_struct*)_tmp30A)->f2;_LL284: Cyc_Port_leq(
Cyc_Port_gen_exp(env,_tmp31B),Cyc_Port_arith_ct());for(0;(unsigned int)_tmp31C;
_tmp31C=_tmp31C->tl){Cyc_Port_gen_stmt(env,((struct Cyc_Absyn_Switch_clause*)
_tmp31C->hd)->body);}return;_LL285: if(_tmp30A <= (void*)1)goto _LL287;if(*((int*)
_tmp30A)!= 10)goto _LL287;_LL286: {const char*_tmp513;void*_tmp512;(_tmp512=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp513="fallthru",_tag_dyneither(_tmp513,sizeof(char),9))),_tag_dyneither(
_tmp512,sizeof(void*),0)));}_LL287: if(_tmp30A <= (void*)1)goto _LL289;if(*((int*)
_tmp30A)!= 11)goto _LL289;_tmp31D=((struct Cyc_Absyn_Decl_s_struct*)_tmp30A)->f1;
_tmp31E=((struct Cyc_Absyn_Decl_s_struct*)_tmp30A)->f2;_LL288: env=Cyc_Port_gen_localdecl(
env,_tmp31D);Cyc_Port_gen_stmt(env,_tmp31E);return;_LL289: if(_tmp30A <= (void*)1)
goto _LL28B;if(*((int*)_tmp30A)!= 12)goto _LL28B;_tmp31F=((struct Cyc_Absyn_Label_s_struct*)
_tmp30A)->f2;_LL28A: Cyc_Port_gen_stmt(env,_tmp31F);return;_LL28B: if(_tmp30A <= (
void*)1)goto _LL28D;if(*((int*)_tmp30A)!= 13)goto _LL28D;_tmp320=((struct Cyc_Absyn_Do_s_struct*)
_tmp30A)->f1;_tmp321=((struct Cyc_Absyn_Do_s_struct*)_tmp30A)->f2;_tmp322=_tmp321.f1;
_LL28C: Cyc_Port_gen_stmt(env,_tmp320);Cyc_Port_leq(Cyc_Port_gen_exp(env,_tmp322),
Cyc_Port_arith_ct());return;_LL28D: if(_tmp30A <= (void*)1)goto _LL28F;if(*((int*)
_tmp30A)!= 14)goto _LL28F;_LL28E: {const char*_tmp516;void*_tmp515;(_tmp515=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp516="try/catch",_tag_dyneither(_tmp516,sizeof(char),10))),_tag_dyneither(
_tmp515,sizeof(void*),0)));}_LL28F: if(_tmp30A <= (void*)1)goto _LL291;if(*((int*)
_tmp30A)!= 15)goto _LL291;_LL290: {const char*_tmp519;void*_tmp518;(_tmp518=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp519="region",_tag_dyneither(_tmp519,sizeof(char),7))),_tag_dyneither(_tmp518,
sizeof(void*),0)));}_LL291: if(_tmp30A <= (void*)1)goto _LL293;if(*((int*)_tmp30A)
!= 16)goto _LL293;_LL292: {const char*_tmp51C;void*_tmp51B;(_tmp51B=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp51C="reset region",
_tag_dyneither(_tmp51C,sizeof(char),13))),_tag_dyneither(_tmp51B,sizeof(void*),0)));}
_LL293: if(_tmp30A <= (void*)1)goto _LL26E;if(*((int*)_tmp30A)!= 17)goto _LL26E;
_LL294: {const char*_tmp51F;void*_tmp51E;(_tmp51E=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp51F="alias",_tag_dyneither(
_tmp51F,sizeof(char),6))),_tag_dyneither(_tmp51E,sizeof(void*),0)));}_LL26E:;}
struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Port_gen_initializer(
struct Cyc_Port_Cenv*env,void*t,struct Cyc_Absyn_Exp*e);static void*Cyc_Port_gen_initializer(
struct Cyc_Port_Cenv*env,void*t,struct Cyc_Absyn_Exp*e){void*_tmp32F=(void*)e->r;
struct Cyc_List_List*_tmp330;struct Cyc_List_List*_tmp331;struct Cyc_List_List*
_tmp332;union Cyc_Absyn_Cnst_union _tmp333;struct _dyneither_ptr _tmp334;_LL296: if(*((
int*)_tmp32F)!= 37)goto _LL298;_tmp330=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp32F)->f2;_LL297: _tmp331=_tmp330;goto _LL299;_LL298: if(*((int*)_tmp32F)!= 28)
goto _LL29A;_tmp331=((struct Cyc_Absyn_Array_e_struct*)_tmp32F)->f1;_LL299: _tmp332=
_tmp331;goto _LL29B;_LL29A: if(*((int*)_tmp32F)!= 27)goto _LL29C;_tmp332=((struct
Cyc_Absyn_CompoundLit_e_struct*)_tmp32F)->f2;_LL29B: LOOP: {void*_tmp335=t;struct
_tuple0*_tmp336;struct Cyc_Absyn_ArrayInfo _tmp337;void*_tmp338;struct Cyc_Absyn_Conref*
_tmp339;struct Cyc_Position_Segment*_tmp33A;struct Cyc_Absyn_AggrInfo _tmp33B;union
Cyc_Absyn_AggrInfoU_union _tmp33C;void*_tmp33D;struct _tuple0*_tmp33E;struct Cyc_Absyn_AggrInfo
_tmp33F;union Cyc_Absyn_AggrInfoU_union _tmp340;struct Cyc_Absyn_Aggrdecl**_tmp341;
struct Cyc_Absyn_Aggrdecl*_tmp342;_LL2A1: if(_tmp335 <= (void*)4)goto _LL2A9;if(*((
int*)_tmp335)!= 16)goto _LL2A3;_tmp336=((struct Cyc_Absyn_TypedefType_struct*)
_tmp335)->f1;_LL2A2:{union Cyc_Absyn_Nmspace_union _tmp520;(*_tmp336).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp520.Loc_n).tag=0,_tmp520));}t=Cyc_Port_lookup_typedef(env,_tmp336);goto LOOP;
_LL2A3: if(*((int*)_tmp335)!= 7)goto _LL2A5;_tmp337=((struct Cyc_Absyn_ArrayType_struct*)
_tmp335)->f1;_tmp338=(void*)_tmp337.elt_type;_tmp339=_tmp337.zero_term;_tmp33A=
_tmp337.zt_loc;_LL2A4: {void*_tmp344=Cyc_Port_var();void*_tmp345=Cyc_Port_var();
void*_tmp346=Cyc_Port_var();void*_tmp347=Cyc_Port_type_to_ctype(env,_tmp338);
for(0;_tmp332 != 0;_tmp332=_tmp332->tl){struct Cyc_List_List*_tmp349;struct Cyc_Absyn_Exp*
_tmp34A;struct _tuple8 _tmp348=*((struct _tuple8*)_tmp332->hd);_tmp349=_tmp348.f1;
_tmp34A=_tmp348.f2;if((unsigned int)_tmp349){const char*_tmp523;void*_tmp522;(
_tmp522=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp523="designators in initializers",_tag_dyneither(_tmp523,sizeof(char),28))),
_tag_dyneither(_tmp522,sizeof(void*),0)));}{void*_tmp34D=Cyc_Port_gen_initializer(
env,_tmp338,_tmp34A);Cyc_Port_leq(_tmp34D,_tmp344);}}return Cyc_Port_array_ct(
_tmp344,_tmp345,_tmp346);}_LL2A5: if(*((int*)_tmp335)!= 10)goto _LL2A7;_tmp33B=((
struct Cyc_Absyn_AggrType_struct*)_tmp335)->f1;_tmp33C=_tmp33B.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmp335)->f1).aggr_info).UnknownAggr).tag != 0)
goto _LL2A7;_tmp33D=(_tmp33C.UnknownAggr).f1;if((int)_tmp33D != 0)goto _LL2A7;
_tmp33E=(_tmp33C.UnknownAggr).f2;_LL2A6:{union Cyc_Absyn_Nmspace_union _tmp524;(*
_tmp33E).f1=(union Cyc_Absyn_Nmspace_union)(((_tmp524.Loc_n).tag=0,_tmp524));}{
struct Cyc_Absyn_Aggrdecl*_tmp350;struct _tuple4 _tmp34F=*Cyc_Port_lookup_struct_decl(
env,_tmp33E);_tmp350=_tmp34F.f1;if((struct Cyc_Absyn_Aggrdecl*)_tmp350 == 0){const
char*_tmp527;void*_tmp526;(_tmp526=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp527="struct is not yet defined",
_tag_dyneither(_tmp527,sizeof(char),26))),_tag_dyneither(_tmp526,sizeof(void*),0)));}
_tmp342=_tmp350;goto _LL2A8;}_LL2A7: if(*((int*)_tmp335)!= 10)goto _LL2A9;_tmp33F=((
struct Cyc_Absyn_AggrType_struct*)_tmp335)->f1;_tmp340=_tmp33F.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmp335)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL2A9;_tmp341=(_tmp340.KnownAggr).f1;_tmp342=*_tmp341;_LL2A8: {struct Cyc_List_List*
_tmp353=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp342->impl))->fields;for(
0;_tmp332 != 0;_tmp332=_tmp332->tl){struct Cyc_List_List*_tmp355;struct Cyc_Absyn_Exp*
_tmp356;struct _tuple8 _tmp354=*((struct _tuple8*)_tmp332->hd);_tmp355=_tmp354.f1;
_tmp356=_tmp354.f2;if((unsigned int)_tmp355){const char*_tmp52A;void*_tmp529;(
_tmp529=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp52A="designators in initializers",_tag_dyneither(_tmp52A,sizeof(char),28))),
_tag_dyneither(_tmp529,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrfield*_tmp359=(
struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp353))->hd;
_tmp353=_tmp353->tl;{void*_tmp35A=Cyc_Port_gen_initializer(env,(void*)_tmp359->type,
_tmp356);;}}}return Cyc_Port_type_to_ctype(env,t);}_LL2A9:;_LL2AA: {const char*
_tmp52D;void*_tmp52C;(_tmp52C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp52D="bad type for aggregate initializer",
_tag_dyneither(_tmp52D,sizeof(char),35))),_tag_dyneither(_tmp52C,sizeof(void*),0)));}
_LL2A0:;}_LL29C: if(*((int*)_tmp32F)!= 0)goto _LL29E;_tmp333=((struct Cyc_Absyn_Const_e_struct*)
_tmp32F)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp32F)->f1).String_c).tag
!= 5)goto _LL29E;_tmp334=(_tmp333.String_c).f1;_LL29D: LOOP2: {void*_tmp35D=t;
struct _tuple0*_tmp35E;_LL2AC: if(_tmp35D <= (void*)4)goto _LL2B0;if(*((int*)_tmp35D)
!= 16)goto _LL2AE;_tmp35E=((struct Cyc_Absyn_TypedefType_struct*)_tmp35D)->f1;
_LL2AD:{union Cyc_Absyn_Nmspace_union _tmp52E;(*_tmp35E).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp52E.Loc_n).tag=0,_tmp52E));}t=Cyc_Port_lookup_typedef(env,_tmp35E);goto LOOP2;
_LL2AE: if(*((int*)_tmp35D)!= 7)goto _LL2B0;_LL2AF: return Cyc_Port_array_ct(Cyc_Port_arith_ct(),
Cyc_Port_const_ct(),Cyc_Port_zterm_ct());_LL2B0:;_LL2B1: return Cyc_Port_gen_exp(
env,e);_LL2AB:;}_LL29E:;_LL29F: return Cyc_Port_gen_exp(env,e);_LL295:;}static
struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*
d);static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,
struct Cyc_Absyn_Decl*d){void*_tmp360=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp361;
_LL2B3: if(_tmp360 <= (void*)2)goto _LL2B5;if(*((int*)_tmp360)!= 0)goto _LL2B5;
_tmp361=((struct Cyc_Absyn_Var_d_struct*)_tmp360)->f1;_LL2B4: {void*_tmp362=Cyc_Port_var();
void*q;if((env->gcenv)->porting){q=Cyc_Port_var();Cyc_Port_register_const_cvar(
env,q,(_tmp361->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(
_tmp361->tq).loc);}else{q=(_tmp361->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}{
union Cyc_Absyn_Nmspace_union _tmp52F;(*_tmp361->name).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp52F.Loc_n).tag=0,_tmp52F));}env=Cyc_Port_add_var(env,_tmp361->name,(void*)
_tmp361->type,q,_tmp362);Cyc_Port_unifies(_tmp362,Cyc_Port_type_to_ctype(env,(
void*)_tmp361->type));if((unsigned int)_tmp361->initializer){struct Cyc_Absyn_Exp*
e=(struct Cyc_Absyn_Exp*)_check_null(_tmp361->initializer);void*t2=Cyc_Port_gen_initializer(
env,(void*)_tmp361->type,e);Cyc_Port_leq(t2,_tmp362);}return env;}_LL2B5:;_LL2B6: {
const char*_tmp532;void*_tmp531;(_tmp531=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp532="non-local decl that isn't a variable",
_tag_dyneither(_tmp532,sizeof(char),37))),_tag_dyneither(_tmp531,sizeof(void*),0)));}
_LL2B2:;}struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;}
;static struct _tuple1*Cyc_Port_make_targ(struct _tuple9*arg);static struct _tuple1*
Cyc_Port_make_targ(struct _tuple9*arg){struct _dyneither_ptr*_tmp367;struct Cyc_Absyn_Tqual
_tmp368;void*_tmp369;struct _tuple9 _tmp366=*arg;_tmp367=_tmp366.f1;_tmp368=
_tmp366.f2;_tmp369=_tmp366.f3;{struct _tuple1*_tmp533;return(_tmp533=_cycalloc(
sizeof(*_tmp533)),((_tmp533->f1=0,((_tmp533->f2=_tmp368,((_tmp533->f3=_tmp369,
_tmp533)))))));}}static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*
env,struct Cyc_List_List*ds);struct _tuple10{struct _dyneither_ptr*f1;void*f2;void*
f3;void*f4;};static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecl(struct Cyc_Port_Cenv*
env,struct Cyc_Absyn_Decl*d);static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecl(
struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d){void*_tmp36B=(void*)d->r;struct
Cyc_Absyn_Vardecl*_tmp36C;struct Cyc_Absyn_Fndecl*_tmp36D;struct Cyc_Absyn_Typedefdecl*
_tmp36E;struct Cyc_Absyn_Aggrdecl*_tmp36F;struct Cyc_Absyn_Enumdecl*_tmp370;_LL2B8:
if((int)_tmp36B != 0)goto _LL2BA;_LL2B9:(env->gcenv)->porting=1;return env;_LL2BA:
if((int)_tmp36B != 1)goto _LL2BC;_LL2BB:(env->gcenv)->porting=1;return env;_LL2BC:
if(_tmp36B <= (void*)2)goto _LL2C6;if(*((int*)_tmp36B)!= 0)goto _LL2BE;_tmp36C=((
struct Cyc_Absyn_Var_d_struct*)_tmp36B)->f1;_LL2BD:{union Cyc_Absyn_Nmspace_union
_tmp534;(*_tmp36C->name).f1=(union Cyc_Absyn_Nmspace_union)(((_tmp534.Loc_n).tag=
0,_tmp534));}if(Cyc_Port_declared_var(env,_tmp36C->name)){void*_tmp373;void*
_tmp374;struct _tuple3 _tmp372=Cyc_Port_lookup_var(env,_tmp36C->name);_tmp373=
_tmp372.f1;_tmp374=_tmp372.f2;{void*q2;if((env->gcenv)->porting  && !Cyc_Port_isfn(
env,_tmp36C->name)){q2=Cyc_Port_var();Cyc_Port_register_const_cvar(env,q2,(
_tmp36C->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(_tmp36C->tq).loc);}
else{q2=(_tmp36C->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
Cyc_Port_unifies(_tmp373,q2);Cyc_Port_unifies(_tmp374,Cyc_Port_type_to_ctype(env,(
void*)_tmp36C->type));if((unsigned int)_tmp36C->initializer){struct Cyc_Absyn_Exp*
e=(struct Cyc_Absyn_Exp*)_check_null(_tmp36C->initializer);Cyc_Port_leq(Cyc_Port_gen_initializer(
env,(void*)_tmp36C->type,e),_tmp374);}return env;}}else{return Cyc_Port_gen_localdecl(
env,d);}_LL2BE: if(*((int*)_tmp36B)!= 1)goto _LL2C0;_tmp36D=((struct Cyc_Absyn_Fn_d_struct*)
_tmp36B)->f1;_LL2BF:{union Cyc_Absyn_Nmspace_union _tmp535;(*_tmp36D->name).f1=(
union Cyc_Absyn_Nmspace_union)(((_tmp535.Loc_n).tag=0,_tmp535));}{struct _tuple6*
predeclared=0;if(Cyc_Port_declared_var(env,_tmp36D->name))predeclared=(struct
_tuple6*)Cyc_Port_lookup_full_var(env,_tmp36D->name);{void*_tmp376=(void*)
_tmp36D->ret_type;struct Cyc_List_List*_tmp377=_tmp36D->args;struct Cyc_List_List*
_tmp378=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Port_make_targ,_tmp377);struct Cyc_Absyn_FnType_struct
_tmp53B;struct Cyc_Absyn_FnInfo _tmp53A;struct Cyc_Absyn_FnType_struct*_tmp539;
struct Cyc_Absyn_FnType_struct*_tmp379=(_tmp539=_cycalloc(sizeof(*_tmp539)),((
_tmp539[0]=((_tmp53B.tag=8,((_tmp53B.f1=((_tmp53A.tvars=0,((_tmp53A.effect=0,((
_tmp53A.ret_typ=(void*)_tmp376,((_tmp53A.args=_tmp378,((_tmp53A.c_varargs=0,((
_tmp53A.cyc_varargs=0,((_tmp53A.rgn_po=0,((_tmp53A.attributes=0,_tmp53A)))))))))))))))),
_tmp53B)))),_tmp539)));struct Cyc_Port_Cenv*_tmp37A=Cyc_Port_set_cpos(env,(void*)
2);void*_tmp37B=Cyc_Port_type_to_ctype(_tmp37A,_tmp376);struct Cyc_List_List*
c_args=0;struct Cyc_List_List*c_arg_types=0;{struct Cyc_List_List*_tmp37C=_tmp377;
for(0;(unsigned int)_tmp37C;_tmp37C=_tmp37C->tl){struct _dyneither_ptr*_tmp37E;
struct Cyc_Absyn_Tqual _tmp37F;void*_tmp380;struct _tuple9 _tmp37D=*((struct _tuple9*)
_tmp37C->hd);_tmp37E=_tmp37D.f1;_tmp37F=_tmp37D.f2;_tmp380=_tmp37D.f3;{void*
_tmp381=Cyc_Port_type_to_ctype(_tmp37A,_tmp380);void*tqv;if((env->gcenv)->porting){
tqv=Cyc_Port_var();Cyc_Port_register_const_cvar(env,tqv,_tmp37F.print_const?Cyc_Port_const_ct():
Cyc_Port_notconst_ct(),_tmp37F.loc);}else{tqv=_tmp37F.print_const?Cyc_Port_const_ct():
Cyc_Port_notconst_ct();}{struct _tuple10*_tmp53E;struct Cyc_List_List*_tmp53D;
c_args=((_tmp53D=_cycalloc(sizeof(*_tmp53D)),((_tmp53D->hd=((_tmp53E=_cycalloc(
sizeof(*_tmp53E)),((_tmp53E->f1=_tmp37E,((_tmp53E->f2=_tmp380,((_tmp53E->f3=tqv,((
_tmp53E->f4=_tmp381,_tmp53E)))))))))),((_tmp53D->tl=c_args,_tmp53D))))));}{
struct Cyc_List_List*_tmp53F;c_arg_types=((_tmp53F=_cycalloc(sizeof(*_tmp53F)),((
_tmp53F->hd=(void*)_tmp381,((_tmp53F->tl=c_arg_types,_tmp53F))))));}}}}c_args=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(c_args);
c_arg_types=Cyc_List_imp_rev(c_arg_types);{void*_tmp385=Cyc_Port_fn_ct(_tmp37B,
c_arg_types);{union Cyc_Absyn_Nmspace_union _tmp540;(*_tmp36D->name).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp540.Loc_n).tag=0,_tmp540));}_tmp37A=Cyc_Port_add_var(_tmp37A,_tmp36D->name,(
void*)_tmp379,Cyc_Port_const_ct(),_tmp385);Cyc_Port_add_return_type(_tmp37A,
_tmp37B);{struct Cyc_List_List*_tmp387=c_args;for(0;(unsigned int)_tmp387;_tmp387=
_tmp387->tl){struct _dyneither_ptr*_tmp389;void*_tmp38A;void*_tmp38B;void*_tmp38C;
struct _tuple10 _tmp388=*((struct _tuple10*)_tmp387->hd);_tmp389=_tmp388.f1;_tmp38A=
_tmp388.f2;_tmp38B=_tmp388.f3;_tmp38C=_tmp388.f4;{union Cyc_Absyn_Nmspace_union
_tmp543;struct _tuple0*_tmp542;_tmp37A=Cyc_Port_add_var(_tmp37A,((_tmp542=
_cycalloc(sizeof(*_tmp542)),((_tmp542->f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp543.Loc_n).tag=0,_tmp543)),((_tmp542->f2=_tmp389,_tmp542)))))),_tmp38A,
_tmp38B,_tmp38C);}}}Cyc_Port_gen_stmt(_tmp37A,_tmp36D->body);Cyc_Port_generalize(
0,_tmp385);{struct Cyc_Dict_Dict counts=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);Cyc_Port_region_counts(&
counts,_tmp385);Cyc_Port_register_rgns(env,counts,1,(void*)_tmp379,_tmp385);env=
Cyc_Port_add_var(_tmp37A,_tmp36D->name,(void*)_tmp379,Cyc_Port_const_ct(),
_tmp385);if((unsigned int)predeclared){void*_tmp390;struct _tuple3*_tmp391;struct
_tuple3 _tmp392;void*_tmp393;void*_tmp394;struct _tuple6 _tmp38F=*predeclared;
_tmp390=_tmp38F.f1;_tmp391=_tmp38F.f2;_tmp392=*_tmp391;_tmp393=_tmp392.f1;
_tmp394=_tmp392.f2;Cyc_Port_unifies(_tmp393,Cyc_Port_const_ct());Cyc_Port_unifies(
_tmp394,Cyc_Port_instantiate(_tmp385));Cyc_Port_register_rgns(env,counts,1,
_tmp390,_tmp385);}return env;}}}}_LL2C0: if(*((int*)_tmp36B)!= 7)goto _LL2C2;
_tmp36E=((struct Cyc_Absyn_Typedef_d_struct*)_tmp36B)->f1;_LL2C1: {void*_tmp398=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp36E->defn))->v;void*_tmp399=Cyc_Port_type_to_ctype(
env,_tmp398);{union Cyc_Absyn_Nmspace_union _tmp544;(*_tmp36E->name).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp544.Loc_n).tag=0,_tmp544));}Cyc_Port_add_typedef(env,_tmp36E->name,_tmp398,
_tmp399);return env;}_LL2C2: if(*((int*)_tmp36B)!= 4)goto _LL2C4;_tmp36F=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp36B)->f1;_LL2C3: {struct _tuple0*_tmp39B=_tmp36F->name;{
union Cyc_Absyn_Nmspace_union _tmp545;(*_tmp36F->name).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp545.Loc_n).tag=0,_tmp545));}{struct _tuple4*previous;struct Cyc_List_List*curr=
0;{void*_tmp39D=(void*)_tmp36F->kind;_LL2C9: if((int)_tmp39D != 0)goto _LL2CB;
_LL2CA: previous=Cyc_Port_lookup_struct_decl(env,_tmp39B);goto _LL2C8;_LL2CB: if((
int)_tmp39D != 1)goto _LL2C8;_LL2CC: previous=Cyc_Port_lookup_union_decl(env,
_tmp39B);goto _LL2C8;_LL2C8:;}if((unsigned int)_tmp36F->impl){struct Cyc_List_List*
cf=(*previous).f2;{struct Cyc_List_List*_tmp39E=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp36F->impl))->fields;for(0;(unsigned int)_tmp39E;_tmp39E=_tmp39E->tl){
struct Cyc_Absyn_Aggrfield*_tmp39F=(struct Cyc_Absyn_Aggrfield*)_tmp39E->hd;void*
qv;if((env->gcenv)->porting){qv=Cyc_Port_var();Cyc_Port_register_const_cvar(env,
qv,(_tmp39F->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct(),(_tmp39F->tq).loc);}
else{qv=(_tmp39F->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}{
void*_tmp3A0=Cyc_Port_type_to_ctype(env,(void*)_tmp39F->type);if(cf != 0){void*
_tmp3A2;void*_tmp3A3;struct Cyc_Port_Cfield _tmp3A1=*((struct Cyc_Port_Cfield*)cf->hd);
_tmp3A2=(void*)_tmp3A1.qual;_tmp3A3=(void*)_tmp3A1.type;cf=cf->tl;Cyc_Port_unifies(
qv,_tmp3A2);Cyc_Port_unifies(_tmp3A0,_tmp3A3);}{struct Cyc_Port_Cfield*_tmp548;
struct Cyc_List_List*_tmp547;curr=((_tmp547=_cycalloc(sizeof(*_tmp547)),((_tmp547->hd=((
_tmp548=_cycalloc(sizeof(*_tmp548)),((_tmp548->qual=(void*)qv,((_tmp548->f=
_tmp39F->name,((_tmp548->type=(void*)_tmp3A0,_tmp548)))))))),((_tmp547->tl=curr,
_tmp547))))));}}}}curr=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
curr);if((*previous).f2 == 0)(*previous).f2=curr;}return env;}}_LL2C4: if(*((int*)
_tmp36B)!= 6)goto _LL2C6;_tmp370=((struct Cyc_Absyn_Enum_d_struct*)_tmp36B)->f1;
_LL2C5:{union Cyc_Absyn_Nmspace_union _tmp549;(*_tmp370->name).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp549.Loc_n).tag=0,_tmp549));}if((unsigned int)_tmp370->fields){struct Cyc_List_List*
_tmp3A7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp370->fields))->v;
for(0;(unsigned int)_tmp3A7;_tmp3A7=_tmp3A7->tl){{union Cyc_Absyn_Nmspace_union
_tmp54A;(*((struct Cyc_Absyn_Enumfield*)_tmp3A7->hd)->name).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp54A.Loc_n).tag=0,_tmp54A));}{struct Cyc_Absyn_EnumType_struct _tmp54D;struct
Cyc_Absyn_EnumType_struct*_tmp54C;env=Cyc_Port_add_var(env,((struct Cyc_Absyn_Enumfield*)
_tmp3A7->hd)->name,(void*)((_tmp54C=_cycalloc(sizeof(*_tmp54C)),((_tmp54C[0]=((
_tmp54D.tag=12,((_tmp54D.f1=_tmp370->name,((_tmp54D.f2=(struct Cyc_Absyn_Enumdecl*)
_tmp370,_tmp54D)))))),_tmp54C)))),Cyc_Port_const_ct(),Cyc_Port_arith_ct());}}}
return env;_LL2C6:;_LL2C7: if((env->gcenv)->porting){const char*_tmp550;void*
_tmp54F;(_tmp54F=0,Cyc_fprintf(Cyc_stderr,((_tmp550="Warning: Cyclone declaration found in code to be ported -- skipping.",
_tag_dyneither(_tmp550,sizeof(char),69))),_tag_dyneither(_tmp54F,sizeof(void*),0)));}
return env;_LL2B7:;}static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*
env,struct Cyc_List_List*ds);static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(
struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds){for(0;(unsigned int)ds;ds=ds->tl){
env=Cyc_Port_gen_topdecl(env,(struct Cyc_Absyn_Decl*)ds->hd);}return env;}static
struct Cyc_List_List*Cyc_Port_gen_edits(struct Cyc_List_List*ds);static struct Cyc_List_List*
Cyc_Port_gen_edits(struct Cyc_List_List*ds){struct Cyc_Port_Cenv*env=Cyc_Port_gen_topdecls(
Cyc_Port_initial_cenv(),ds);struct Cyc_List_List*_tmp3AD=(env->gcenv)->pointer_edits;
struct Cyc_List_List*_tmp3AE=(env->gcenv)->qualifier_edits;struct Cyc_List_List*
_tmp3AF=(env->gcenv)->zeroterm_edits;struct Cyc_List_List*_tmp3B0=(env->gcenv)->edits;{
struct Cyc_List_List*_tmp3B1=_tmp3AD;for(0;(unsigned int)_tmp3B1;_tmp3B1=_tmp3B1->tl){
void*_tmp3B3;void*_tmp3B4;struct Cyc_Position_Segment*_tmp3B5;struct _tuple7
_tmp3B2=*((struct _tuple7*)_tmp3B1->hd);_tmp3B3=_tmp3B2.f1;_tmp3B4=_tmp3B2.f2;
_tmp3B5=_tmp3B2.f3;Cyc_Port_unifies(_tmp3B3,_tmp3B4);}}{struct Cyc_List_List*
_tmp3B6=_tmp3AE;for(0;(unsigned int)_tmp3B6;_tmp3B6=_tmp3B6->tl){void*_tmp3B8;
void*_tmp3B9;struct Cyc_Position_Segment*_tmp3BA;struct _tuple7 _tmp3B7=*((struct
_tuple7*)_tmp3B6->hd);_tmp3B8=_tmp3B7.f1;_tmp3B9=_tmp3B7.f2;_tmp3BA=_tmp3B7.f3;
Cyc_Port_unifies(_tmp3B8,_tmp3B9);}}{struct Cyc_List_List*_tmp3BB=_tmp3AF;for(0;(
unsigned int)_tmp3BB;_tmp3BB=_tmp3BB->tl){void*_tmp3BD;void*_tmp3BE;struct Cyc_Position_Segment*
_tmp3BF;struct _tuple7 _tmp3BC=*((struct _tuple7*)_tmp3BB->hd);_tmp3BD=_tmp3BC.f1;
_tmp3BE=_tmp3BC.f2;_tmp3BF=_tmp3BC.f3;Cyc_Port_unifies(_tmp3BD,_tmp3BE);}}for(0;(
unsigned int)_tmp3AD;_tmp3AD=_tmp3AD->tl){void*_tmp3C1;void*_tmp3C2;struct Cyc_Position_Segment*
_tmp3C3;struct _tuple7 _tmp3C0=*((struct _tuple7*)_tmp3AD->hd);_tmp3C1=_tmp3C0.f1;
_tmp3C2=_tmp3C0.f2;_tmp3C3=_tmp3C0.f3;if(!Cyc_Port_unifies(_tmp3C1,_tmp3C2) && (
unsigned int)_tmp3C3){void*_tmp3C4=Cyc_Port_compress_ct(_tmp3C2);_LL2CE: if((int)
_tmp3C4 != 2)goto _LL2D0;_LL2CF:{struct Cyc_Port_Edit*_tmp559;const char*_tmp558;
const char*_tmp557;struct Cyc_List_List*_tmp556;_tmp3B0=((_tmp556=_cycalloc(
sizeof(*_tmp556)),((_tmp556->hd=((_tmp559=_cycalloc(sizeof(*_tmp559)),((_tmp559->loc=
_tmp3C3,((_tmp559->old_string=((_tmp557="?",_tag_dyneither(_tmp557,sizeof(char),
2))),((_tmp559->new_string=((_tmp558="*",_tag_dyneither(_tmp558,sizeof(char),2))),
_tmp559)))))))),((_tmp556->tl=_tmp3B0,_tmp556))))));}goto _LL2CD;_LL2D0: if((int)
_tmp3C4 != 3)goto _LL2D2;_LL2D1:{struct Cyc_Port_Edit*_tmp562;const char*_tmp561;
const char*_tmp560;struct Cyc_List_List*_tmp55F;_tmp3B0=((_tmp55F=_cycalloc(
sizeof(*_tmp55F)),((_tmp55F->hd=((_tmp562=_cycalloc(sizeof(*_tmp562)),((_tmp562->loc=
_tmp3C3,((_tmp562->old_string=((_tmp560="*",_tag_dyneither(_tmp560,sizeof(char),
2))),((_tmp562->new_string=((_tmp561="?",_tag_dyneither(_tmp561,sizeof(char),2))),
_tmp562)))))))),((_tmp55F->tl=_tmp3B0,_tmp55F))))));}goto _LL2CD;_LL2D2:;_LL2D3:
goto _LL2CD;_LL2CD:;}}for(0;(unsigned int)_tmp3AE;_tmp3AE=_tmp3AE->tl){void*
_tmp3CE;void*_tmp3CF;struct Cyc_Position_Segment*_tmp3D0;struct _tuple7 _tmp3CD=*((
struct _tuple7*)_tmp3AE->hd);_tmp3CE=_tmp3CD.f1;_tmp3CF=_tmp3CD.f2;_tmp3D0=
_tmp3CD.f3;if(!Cyc_Port_unifies(_tmp3CE,_tmp3CF) && (unsigned int)_tmp3D0){void*
_tmp3D1=Cyc_Port_compress_ct(_tmp3CF);_LL2D5: if((int)_tmp3D1 != 1)goto _LL2D7;
_LL2D6:{struct Cyc_Port_Edit*_tmp56B;const char*_tmp56A;const char*_tmp569;struct
Cyc_List_List*_tmp568;_tmp3B0=((_tmp568=_cycalloc(sizeof(*_tmp568)),((_tmp568->hd=((
_tmp56B=_cycalloc(sizeof(*_tmp56B)),((_tmp56B->loc=_tmp3D0,((_tmp56B->old_string=((
_tmp569="const ",_tag_dyneither(_tmp569,sizeof(char),7))),((_tmp56B->new_string=((
_tmp56A="",_tag_dyneither(_tmp56A,sizeof(char),1))),_tmp56B)))))))),((_tmp568->tl=
_tmp3B0,_tmp568))))));}goto _LL2D4;_LL2D7: if((int)_tmp3D1 != 0)goto _LL2D9;_LL2D8:{
struct Cyc_Port_Edit*_tmp574;const char*_tmp573;const char*_tmp572;struct Cyc_List_List*
_tmp571;_tmp3B0=((_tmp571=_cycalloc(sizeof(*_tmp571)),((_tmp571->hd=((_tmp574=
_cycalloc(sizeof(*_tmp574)),((_tmp574->loc=_tmp3D0,((_tmp574->old_string=((
_tmp572="",_tag_dyneither(_tmp572,sizeof(char),1))),((_tmp574->new_string=((
_tmp573="const ",_tag_dyneither(_tmp573,sizeof(char),7))),_tmp574)))))))),((
_tmp571->tl=_tmp3B0,_tmp571))))));}goto _LL2D4;_LL2D9:;_LL2DA: goto _LL2D4;_LL2D4:;}}
for(0;(unsigned int)_tmp3AF;_tmp3AF=_tmp3AF->tl){void*_tmp3DB;void*_tmp3DC;
struct Cyc_Position_Segment*_tmp3DD;struct _tuple7 _tmp3DA=*((struct _tuple7*)
_tmp3AF->hd);_tmp3DB=_tmp3DA.f1;_tmp3DC=_tmp3DA.f2;_tmp3DD=_tmp3DA.f3;if(!Cyc_Port_unifies(
_tmp3DB,_tmp3DC) && (unsigned int)_tmp3DD){void*_tmp3DE=Cyc_Port_compress_ct(
_tmp3DC);_LL2DC: if((int)_tmp3DE != 8)goto _LL2DE;_LL2DD:{struct Cyc_Port_Edit*
_tmp57D;const char*_tmp57C;const char*_tmp57B;struct Cyc_List_List*_tmp57A;_tmp3B0=((
_tmp57A=_cycalloc(sizeof(*_tmp57A)),((_tmp57A->hd=((_tmp57D=_cycalloc(sizeof(*
_tmp57D)),((_tmp57D->loc=_tmp3DD,((_tmp57D->old_string=((_tmp57B="NOZEROTERM ",
_tag_dyneither(_tmp57B,sizeof(char),12))),((_tmp57D->new_string=((_tmp57C="",
_tag_dyneither(_tmp57C,sizeof(char),1))),_tmp57D)))))))),((_tmp57A->tl=_tmp3B0,
_tmp57A))))));}goto _LL2DB;_LL2DE: if((int)_tmp3DE != 9)goto _LL2E0;_LL2DF:{struct
Cyc_Port_Edit*_tmp586;const char*_tmp585;const char*_tmp584;struct Cyc_List_List*
_tmp583;_tmp3B0=((_tmp583=_cycalloc(sizeof(*_tmp583)),((_tmp583->hd=((_tmp586=
_cycalloc(sizeof(*_tmp586)),((_tmp586->loc=_tmp3DD,((_tmp586->old_string=((
_tmp584="ZEROTERM ",_tag_dyneither(_tmp584,sizeof(char),10))),((_tmp586->new_string=((
_tmp585="",_tag_dyneither(_tmp585,sizeof(char),1))),_tmp586)))))))),((_tmp583->tl=
_tmp3B0,_tmp583))))));}goto _LL2DB;_LL2E0:;_LL2E1: goto _LL2DB;_LL2DB:;}}_tmp3B0=((
struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_Port_Edit*,struct Cyc_Port_Edit*),
struct Cyc_List_List*x))Cyc_List_merge_sort)(Cyc_Port_cmp_edit,_tmp3B0);while((
unsigned int)_tmp3B0  && ((struct Cyc_Position_Segment*)_check_null(((struct Cyc_Port_Edit*)
_tmp3B0->hd)->loc))->start == 0){_tmp3B0=_tmp3B0->tl;}return _tmp3B0;}static struct
Cyc_Position_Segment*Cyc_Port_get_seg(struct Cyc_Port_Edit*e);static struct Cyc_Position_Segment*
Cyc_Port_get_seg(struct Cyc_Port_Edit*e){return e->loc;}void Cyc_Port_port(struct
Cyc_List_List*ds);void Cyc_Port_port(struct Cyc_List_List*ds){struct Cyc_List_List*
_tmp3E7=Cyc_Port_gen_edits(ds);struct Cyc_List_List*_tmp3E8=((struct Cyc_List_List*(*)(
struct Cyc_Position_Segment*(*f)(struct Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Port_get_seg,_tmp3E7);Cyc_Position_use_gcc_style_location=0;{struct Cyc_List_List*
_tmp3E9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Position_strings_of_segments(
_tmp3E8));for(0;(unsigned int)_tmp3E7;(_tmp3E7=_tmp3E7->tl,_tmp3E9=_tmp3E9->tl)){
struct Cyc_Position_Segment*_tmp3EB;struct _dyneither_ptr _tmp3EC;struct
_dyneither_ptr _tmp3ED;struct Cyc_Port_Edit _tmp3EA=*((struct Cyc_Port_Edit*)_tmp3E7->hd);
_tmp3EB=_tmp3EA.loc;_tmp3EC=_tmp3EA.old_string;_tmp3ED=_tmp3EA.new_string;{
struct _dyneither_ptr sloc=(struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct
Cyc_List_List*)_check_null(_tmp3E9))->hd);const char*_tmp58C;void*_tmp58B[3];
struct Cyc_String_pa_struct _tmp58A;struct Cyc_String_pa_struct _tmp589;struct Cyc_String_pa_struct
_tmp588;(_tmp588.tag=0,((_tmp588.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp3ED),((_tmp589.tag=0,((_tmp589.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp3EC),((_tmp58A.tag=0,((_tmp58A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)sloc),((_tmp58B[0]=& _tmp58A,((_tmp58B[1]=& _tmp589,((_tmp58B[
2]=& _tmp588,Cyc_printf(((_tmp58C="%s: insert `%s' for `%s'\n",_tag_dyneither(
_tmp58C,sizeof(char),26))),_tag_dyneither(_tmp58B,sizeof(void*),3)))))))))))))))))));}}}}
