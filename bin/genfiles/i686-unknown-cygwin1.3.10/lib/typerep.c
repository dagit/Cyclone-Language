#ifndef _SETJMP_H_
#define _SETJMP_H_
#ifndef _jmp_buf_def_
#define _jmp_buf_def_
typedef int jmp_buf[52];
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
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_Typerep_Int_struct{
int tag;int f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;
unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct
_tuple0{unsigned int f1;struct _dyneither_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{
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
_dyneither_ptr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];
struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];extern char
Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple5{void*f1;void*f2;};struct _tuple5*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple5*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);int Cyc_strcmp(struct _dyneither_ptr s1,
struct _dyneither_ptr s2);struct _tuple4 Cyc_Typerep_tuple_tl(void*typ){void*_tmp0=
typ;unsigned int _tmp1;struct _dyneither_ptr _tmp2;_LL1: if(_tmp0 <= (void*)2)goto
_LL3;if(*((int*)_tmp0)!= 4)goto _LL3;_tmp1=((struct Cyc_Typerep_Tuple_struct*)
_tmp0)->f1;_tmp2=((struct Cyc_Typerep_Tuple_struct*)_tmp0)->f2;_LL2: if(
_get_dyneither_size(_tmp2,sizeof(struct _tuple1*))< 2)goto _LL4;{int pos1=(int)(*(*((
struct _tuple1**)_check_dyneither_subscript(_tmp2,sizeof(struct _tuple1*),1)))).f1;
struct _dyneither_ptr _tmp3=({unsigned int _tmp7=_get_dyneither_size(_tmp2,sizeof(
struct _tuple1*))- 1;struct _tuple1**_tmp8=(struct _tuple1**)_cycalloc(_check_times(
sizeof(struct _tuple1*),_tmp7));struct _dyneither_ptr _tmpB=_tag_dyneither(_tmp8,
sizeof(struct _tuple1*),_tmp7);{unsigned int _tmp9=_tmp7;unsigned int i;for(i=0;i < 
_tmp9;i ++){_tmp8[i]=({struct _tuple1*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->f1=(*(*((
struct _tuple1**)_check_dyneither_subscript(_tmp2,sizeof(struct _tuple1*),(int)(i + 
1))))).f1 - pos1;_tmpA->f2=(*(*((struct _tuple1**)_check_dyneither_subscript(_tmp2,
sizeof(struct _tuple1*),(int)(i + 1))))).f2;_tmpA;});}}_tmpB;});return({struct
_tuple4 _tmp4;_tmp4.f1=pos1;_tmp4.f2=(void*)({struct Cyc_Typerep_Tuple_struct*
_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=({struct Cyc_Typerep_Tuple_struct _tmp6;
_tmp6.tag=4;_tmp6.f1=_tmp1 - pos1;_tmp6.f2=_tmp3;_tmp6;});_tmp5;});_tmp4;});}_LL3:;
_LL4:(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpC=_cycalloc(sizeof(*
_tmpC));_tmpC[0]=({struct Cyc_Core_Failure_struct _tmpD;_tmpD.tag=Cyc_Core_Failure;
_tmpD.f1=({const char*_tmpE="tuple_tl: expected tuple of size >= 2";
_tag_dyneither(_tmpE,sizeof(char),38);});_tmpD;});_tmpC;}));_LL0:;}struct _tuple4
Cyc_Typerep_get_unionbranch(unsigned int tag,struct _dyneither_ptr l){{int i=0;for(0;
i < _get_dyneither_size(l,sizeof(struct _tuple0*));++ i){unsigned int _tmp10;void*
_tmp11;struct _tuple0 _tmpF=*(*((struct _tuple0**)_check_dyneither_subscript(l,
sizeof(struct _tuple0*),i)));_tmp10=_tmpF.f1;_tmp11=_tmpF.f3;if(_tmp10 == tag)
return Cyc_Typerep_tuple_tl(_tmp11);}}(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12[0]=({struct Cyc_Core_Failure_struct
_tmp13;_tmp13.tag=Cyc_Core_Failure;_tmp13.f1=({const char*_tmp14="Could not find tag in TUnion";
_tag_dyneither(_tmp14,sizeof(char),29);});_tmp13;});_tmp12;}));}struct
_dyneither_ptr Cyc_Typerep_get_tagname(unsigned int tag,struct _dyneither_ptr l){{
int i=0;for(0;i < _get_dyneither_size(l,sizeof(struct _tuple2*));++ i){unsigned int
_tmp16;struct _dyneither_ptr _tmp17;struct _tuple2 _tmp15=*(*((struct _tuple2**)
_check_dyneither_subscript(l,sizeof(struct _tuple2*),i)));_tmp16=_tmp15.f1;_tmp17=
_tmp15.f2;if(_tmp16 == tag)return _tmp17;}}(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18[0]=({struct Cyc_Core_Failure_struct
_tmp19;_tmp19.tag=Cyc_Core_Failure;_tmp19.f1=({const char*_tmp1A="Could not find name in TUnion";
_tag_dyneither(_tmp1A,sizeof(char),30);});_tmp19;});_tmp18;}));}struct
_dyneither_ptr Cyc_Typerep_get_tagname2(unsigned int tag,struct _dyneither_ptr l){{
int i=0;for(0;i < _get_dyneither_size(l,sizeof(struct _tuple0*));++ i){unsigned int
_tmp1C;struct _dyneither_ptr _tmp1D;struct _tuple0 _tmp1B=*(*((struct _tuple0**)
_check_dyneither_subscript(l,sizeof(struct _tuple0*),i)));_tmp1C=_tmp1B.f1;_tmp1D=
_tmp1B.f2;if(_tmp1C == tag)return _tmp1D;}}(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=({struct Cyc_Core_Failure_struct
_tmp1F;_tmp1F.tag=Cyc_Core_Failure;_tmp1F.f1=({const char*_tmp20="Could not find name in TUnion";
_tag_dyneither(_tmp20,sizeof(char),30);});_tmp1F;});_tmp1E;}));}struct _tuple4 Cyc_Typerep_get_xtunionbranch(
struct _dyneither_ptr tag,struct _dyneither_ptr l){{int i=0;for(0;i < 
_get_dyneither_size(l,sizeof(struct _tuple3*));++ i){struct _dyneither_ptr _tmp22;
void*_tmp23;struct _tuple3 _tmp21=*(*((struct _tuple3**)_check_dyneither_subscript(
l,sizeof(struct _tuple3*),i)));_tmp22=_tmp21.f1;_tmp23=_tmp21.f2;if(Cyc_strcmp((
struct _dyneither_ptr)_tmp22,(struct _dyneither_ptr)tag)== 0)return Cyc_Typerep_tuple_tl(
_tmp23);}}(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp24=_cycalloc(
sizeof(*_tmp24));_tmp24[0]=({struct Cyc_Core_Failure_struct _tmp25;_tmp25.tag=Cyc_Core_Failure;
_tmp25.f1=({const char*_tmp26="Could not find tag in XTUnion";_tag_dyneither(
_tmp26,sizeof(char),30);});_tmp25;});_tmp24;}));}unsigned int Cyc_Typerep_size_type(
void*rep){void*_tmp27=rep;int _tmp28;unsigned int _tmp29;unsigned int _tmp2A;
unsigned int _tmp2B;unsigned int _tmp2C;int _tmp2D;int _tmp2E;struct _dyneither_ptr
_tmp2F;_LL6: if(_tmp27 <= (void*)2)goto _LL8;if(*((int*)_tmp27)!= 0)goto _LL8;_tmp28=((
struct Cyc_Typerep_Int_struct*)_tmp27)->f1;_tmp29=((struct Cyc_Typerep_Int_struct*)
_tmp27)->f2;_LL7: return _tmp29 >> 3;_LL8: if((int)_tmp27 != 0)goto _LLA;_LL9: return
sizeof(float);_LLA: if((int)_tmp27 != 1)goto _LLC;_LLB: return sizeof(double);_LLC:
if(_tmp27 <= (void*)2)goto _LLE;if(*((int*)_tmp27)!= 1)goto _LLE;_LLD: return sizeof(
void*);_LLE: if(_tmp27 <= (void*)2)goto _LL10;if(*((int*)_tmp27)!= 2)goto _LL10;_LLF:
return sizeof(struct _dyneither_ptr);_LL10: if(_tmp27 <= (void*)2)goto _LL12;if(*((
int*)_tmp27)!= 3)goto _LL12;_tmp2A=((struct Cyc_Typerep_Struct_struct*)_tmp27)->f2;
_LL11: return _tmp2A;_LL12: if(_tmp27 <= (void*)2)goto _LL14;if(*((int*)_tmp27)!= 4)
goto _LL14;_tmp2B=((struct Cyc_Typerep_Tuple_struct*)_tmp27)->f1;_LL13: return
_tmp2B;_LL14: if(_tmp27 <= (void*)2)goto _LL16;if(*((int*)_tmp27)!= 6)goto _LL16;
_tmp2C=((struct Cyc_Typerep_TUnionField_struct*)_tmp27)->f3;_LL15: return _tmp2C;
_LL16: if(_tmp27 <= (void*)2)goto _LL18;if(*((int*)_tmp27)!= 5)goto _LL18;_LL17:
return sizeof(void*);_LL18: if(_tmp27 <= (void*)2)goto _LL1A;if(*((int*)_tmp27)!= 7)
goto _LL1A;_LL19: return sizeof(void*);_LL1A: if(_tmp27 <= (void*)2)goto _LL1C;if(*((
int*)_tmp27)!= 8)goto _LL1C;_tmp2D=((struct Cyc_Typerep_Union_struct*)_tmp27)->f2;
_LL1B: return(unsigned int)_tmp2D;_LL1C: if(_tmp27 <= (void*)2)goto _LL5;if(*((int*)
_tmp27)!= 9)goto _LL5;_tmp2E=((struct Cyc_Typerep_Enum_struct*)_tmp27)->f2;_tmp2F=((
struct Cyc_Typerep_Enum_struct*)_tmp27)->f3;_LL1D: return(unsigned int)_tmp2E;_LL5:;}
struct _dyneither_ptr Cyc_Typerep_anon_or_name(struct _dyneither_ptr*name){return
name == 0?({const char*_tmp30="(anon)";_tag_dyneither(_tmp30,sizeof(char),7);}):*
name;}void Cyc_Typerep_print_typestruct(void*rep){void*_tmp31=rep;int _tmp32;
unsigned int _tmp33;unsigned int _tmp34;void*_tmp35;void*_tmp36;struct
_dyneither_ptr*_tmp37;unsigned int _tmp38;struct _dyneither_ptr _tmp39;unsigned int
_tmp3A;struct _dyneither_ptr _tmp3B;struct _dyneither_ptr _tmp3C;struct
_dyneither_ptr _tmp3D;unsigned int _tmp3E;struct _dyneither_ptr _tmp3F;struct
_dyneither_ptr _tmp40;struct _dyneither_ptr _tmp41;struct _dyneither_ptr _tmp42;
struct _dyneither_ptr _tmp43;struct _dyneither_ptr _tmp44;struct _dyneither_ptr*
_tmp45;int _tmp46;struct _dyneither_ptr _tmp47;struct _dyneither_ptr*_tmp48;int
_tmp49;struct _dyneither_ptr _tmp4A;_LL1F: if(_tmp31 <= (void*)2)goto _LL21;if(*((int*)
_tmp31)!= 0)goto _LL21;_tmp32=((struct Cyc_Typerep_Int_struct*)_tmp31)->f1;_tmp33=((
struct Cyc_Typerep_Int_struct*)_tmp31)->f2;_LL20:({struct Cyc_Int_pa_struct _tmp4E;
_tmp4E.tag=1;_tmp4E.f1=(unsigned long)((int)_tmp33);{struct Cyc_String_pa_struct
_tmp4D;_tmp4D.tag=0;_tmp4D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
_tmp32?(struct _dyneither_ptr)({const char*_tmp4F="signed";_tag_dyneither(_tmp4F,
sizeof(char),7);}):(struct _dyneither_ptr)({const char*_tmp50="unsigned";
_tag_dyneither(_tmp50,sizeof(char),9);})));{void*_tmp4B[2]={& _tmp4D,& _tmp4E};Cyc_printf(({
const char*_tmp4C="Int(%s,%d)";_tag_dyneither(_tmp4C,sizeof(char),11);}),
_tag_dyneither(_tmp4B,sizeof(void*),2));}}});goto _LL1E;_LL21: if((int)_tmp31 != 0)
goto _LL23;_LL22:({void*_tmp51=0;Cyc_printf(({const char*_tmp52="Float";
_tag_dyneither(_tmp52,sizeof(char),6);}),_tag_dyneither(_tmp51,sizeof(void*),0));});
goto _LL1E;_LL23: if((int)_tmp31 != 1)goto _LL25;_LL24:({void*_tmp53=0;Cyc_printf(({
const char*_tmp54="Double";_tag_dyneither(_tmp54,sizeof(char),7);}),
_tag_dyneither(_tmp53,sizeof(void*),0));});goto _LL1E;_LL25: if(_tmp31 <= (void*)2)
goto _LL27;if(*((int*)_tmp31)!= 1)goto _LL27;_tmp34=((struct Cyc_Typerep_ThinPtr_struct*)
_tmp31)->f1;_tmp35=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp31)->f2;_LL26:({
struct Cyc_Int_pa_struct _tmp57;_tmp57.tag=1;_tmp57.f1=(unsigned long)((int)_tmp34);{
void*_tmp55[1]={& _tmp57};Cyc_printf(({const char*_tmp56="ThinPtr(%d,";
_tag_dyneither(_tmp56,sizeof(char),12);}),_tag_dyneither(_tmp55,sizeof(void*),1));}});
Cyc_Typerep_print_typestruct(_tmp35);({void*_tmp58=0;Cyc_printf(({const char*
_tmp59=")";_tag_dyneither(_tmp59,sizeof(char),2);}),_tag_dyneither(_tmp58,
sizeof(void*),0));});goto _LL1E;_LL27: if(_tmp31 <= (void*)2)goto _LL29;if(*((int*)
_tmp31)!= 2)goto _LL29;_tmp36=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp31)->f1;
_LL28:({void*_tmp5A=0;Cyc_printf(({const char*_tmp5B="FatPtr(";_tag_dyneither(
_tmp5B,sizeof(char),8);}),_tag_dyneither(_tmp5A,sizeof(void*),0));});Cyc_Typerep_print_typestruct(
_tmp36);({void*_tmp5C=0;Cyc_printf(({const char*_tmp5D=")";_tag_dyneither(_tmp5D,
sizeof(char),2);}),_tag_dyneither(_tmp5C,sizeof(void*),0));});goto _LL1E;_LL29:
if(_tmp31 <= (void*)2)goto _LL2B;if(*((int*)_tmp31)!= 3)goto _LL2B;_tmp37=((struct
Cyc_Typerep_Struct_struct*)_tmp31)->f1;_tmp38=((struct Cyc_Typerep_Struct_struct*)
_tmp31)->f2;_tmp39=((struct Cyc_Typerep_Struct_struct*)_tmp31)->f3;_LL2A:({struct
Cyc_Int_pa_struct _tmp61;_tmp61.tag=1;_tmp61.f1=(unsigned long)((int)_tmp38);{
struct Cyc_String_pa_struct _tmp60;_tmp60.tag=0;_tmp60.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Typerep_anon_or_name(_tmp37));{void*_tmp5E[2]={& _tmp60,&
_tmp61};Cyc_printf(({const char*_tmp5F="Struct(%s,%d,[";_tag_dyneither(_tmp5F,
sizeof(char),15);}),_tag_dyneither(_tmp5E,sizeof(void*),2));}}});{int i=0;for(0;i
< _get_dyneither_size(_tmp39,sizeof(struct _tuple0*));++ i){unsigned int _tmp63;
struct _dyneither_ptr _tmp64;void*_tmp65;struct _tuple0 _tmp62=*(*((struct _tuple0**)
_check_dyneither_subscript(_tmp39,sizeof(struct _tuple0*),i)));_tmp63=_tmp62.f1;
_tmp64=_tmp62.f2;_tmp65=_tmp62.f3;({struct Cyc_String_pa_struct _tmp69;_tmp69.tag=
0;_tmp69.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp64);{struct Cyc_Int_pa_struct
_tmp68;_tmp68.tag=1;_tmp68.f1=(unsigned long)((int)_tmp63);{void*_tmp66[2]={&
_tmp68,& _tmp69};Cyc_printf(({const char*_tmp67="$(%d,%s,";_tag_dyneither(_tmp67,
sizeof(char),9);}),_tag_dyneither(_tmp66,sizeof(void*),2));}}});Cyc_Typerep_print_typestruct(
_tmp65);({struct Cyc_Int_pa_struct _tmp6C;_tmp6C.tag=1;_tmp6C.f1=(unsigned long)((
int)(i != _get_dyneither_size(_tmp39,sizeof(struct _tuple0*))- 1?',':' '));{void*
_tmp6A[1]={& _tmp6C};Cyc_printf(({const char*_tmp6B=")%c";_tag_dyneither(_tmp6B,
sizeof(char),4);}),_tag_dyneither(_tmp6A,sizeof(void*),1));}});}}({void*_tmp6D=0;
Cyc_printf(({const char*_tmp6E=" ]";_tag_dyneither(_tmp6E,sizeof(char),3);}),
_tag_dyneither(_tmp6D,sizeof(void*),0));});goto _LL1E;_LL2B: if(_tmp31 <= (void*)2)
goto _LL2D;if(*((int*)_tmp31)!= 4)goto _LL2D;_tmp3A=((struct Cyc_Typerep_Tuple_struct*)
_tmp31)->f1;_tmp3B=((struct Cyc_Typerep_Tuple_struct*)_tmp31)->f2;_LL2C:({struct
Cyc_Int_pa_struct _tmp71;_tmp71.tag=1;_tmp71.f1=(unsigned long)((int)_tmp3A);{
void*_tmp6F[1]={& _tmp71};Cyc_printf(({const char*_tmp70="Tuple(%d,[";
_tag_dyneither(_tmp70,sizeof(char),11);}),_tag_dyneither(_tmp6F,sizeof(void*),1));}});{
int i=0;for(0;i < _get_dyneither_size(_tmp3B,sizeof(struct _tuple1*));++ i){
unsigned int _tmp73;void*_tmp74;struct _tuple1 _tmp72=*(*((struct _tuple1**)
_check_dyneither_subscript(_tmp3B,sizeof(struct _tuple1*),i)));_tmp73=_tmp72.f1;
_tmp74=_tmp72.f2;({struct Cyc_Int_pa_struct _tmp77;_tmp77.tag=1;_tmp77.f1=(
unsigned long)((int)_tmp73);{void*_tmp75[1]={& _tmp77};Cyc_printf(({const char*
_tmp76="$(%d,";_tag_dyneither(_tmp76,sizeof(char),6);}),_tag_dyneither(_tmp75,
sizeof(void*),1));}});Cyc_Typerep_print_typestruct(_tmp74);({struct Cyc_Int_pa_struct
_tmp7A;_tmp7A.tag=1;_tmp7A.f1=(unsigned long)((int)(i != _get_dyneither_size(
_tmp3B,sizeof(struct _tuple1*))- 1?',':' '));{void*_tmp78[1]={& _tmp7A};Cyc_printf(({
const char*_tmp79=")%c";_tag_dyneither(_tmp79,sizeof(char),4);}),_tag_dyneither(
_tmp78,sizeof(void*),1));}});}}({void*_tmp7B=0;Cyc_printf(({const char*_tmp7C=" ]";
_tag_dyneither(_tmp7C,sizeof(char),3);}),_tag_dyneither(_tmp7B,sizeof(void*),0));});
goto _LL1E;_LL2D: if(_tmp31 <= (void*)2)goto _LL2F;if(*((int*)_tmp31)!= 6)goto _LL2F;
_tmp3C=((struct Cyc_Typerep_TUnionField_struct*)_tmp31)->f1;_tmp3D=((struct Cyc_Typerep_TUnionField_struct*)
_tmp31)->f2;_tmp3E=((struct Cyc_Typerep_TUnionField_struct*)_tmp31)->f3;_tmp3F=((
struct Cyc_Typerep_TUnionField_struct*)_tmp31)->f4;_LL2E:({struct Cyc_Int_pa_struct
_tmp81;_tmp81.tag=1;_tmp81.f1=(unsigned long)((int)_tmp3E);{struct Cyc_String_pa_struct
_tmp80;_tmp80.tag=0;_tmp80.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp3D);{struct Cyc_String_pa_struct _tmp7F;_tmp7F.tag=0;_tmp7F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp3C);{void*_tmp7D[3]={& _tmp7F,& _tmp80,&
_tmp81};Cyc_printf(({const char*_tmp7E="TUnionField(%s,%s,%d,[";_tag_dyneither(
_tmp7E,sizeof(char),23);}),_tag_dyneither(_tmp7D,sizeof(void*),3));}}}});{int i=0;
for(0;i < _get_dyneither_size(_tmp3F,sizeof(struct _tuple1*));++ i){unsigned int
_tmp83;void*_tmp84;struct _tuple1 _tmp82=*(*((struct _tuple1**)
_check_dyneither_subscript(_tmp3F,sizeof(struct _tuple1*),i)));_tmp83=_tmp82.f1;
_tmp84=_tmp82.f2;({struct Cyc_Int_pa_struct _tmp87;_tmp87.tag=1;_tmp87.f1=(
unsigned long)((int)_tmp83);{void*_tmp85[1]={& _tmp87};Cyc_printf(({const char*
_tmp86="$(%d,";_tag_dyneither(_tmp86,sizeof(char),6);}),_tag_dyneither(_tmp85,
sizeof(void*),1));}});Cyc_Typerep_print_typestruct(_tmp84);({struct Cyc_Int_pa_struct
_tmp8A;_tmp8A.tag=1;_tmp8A.f1=(unsigned long)((int)(i != _get_dyneither_size(
_tmp3F,sizeof(struct _tuple1*))- 1?',':' '));{void*_tmp88[1]={& _tmp8A};Cyc_printf(({
const char*_tmp89=")%c";_tag_dyneither(_tmp89,sizeof(char),4);}),_tag_dyneither(
_tmp88,sizeof(void*),1));}});}}({void*_tmp8B=0;Cyc_printf(({const char*_tmp8C=" ]";
_tag_dyneither(_tmp8C,sizeof(char),3);}),_tag_dyneither(_tmp8B,sizeof(void*),0));});
goto _LL1E;_LL2F: if(_tmp31 <= (void*)2)goto _LL31;if(*((int*)_tmp31)!= 5)goto _LL31;
_tmp40=((struct Cyc_Typerep_TUnion_struct*)_tmp31)->f1;_tmp41=((struct Cyc_Typerep_TUnion_struct*)
_tmp31)->f2;_tmp42=((struct Cyc_Typerep_TUnion_struct*)_tmp31)->f3;_LL30:({struct
Cyc_String_pa_struct _tmp8F;_tmp8F.tag=0;_tmp8F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp40);{void*_tmp8D[1]={& _tmp8F};Cyc_printf(({const char*_tmp8E="TUnion(%s,[";
_tag_dyneither(_tmp8E,sizeof(char),12);}),_tag_dyneither(_tmp8D,sizeof(void*),1));}});{
int i=0;for(0;i < _get_dyneither_size(_tmp41,sizeof(struct _tuple2*));++ i){
unsigned int _tmp91;struct _dyneither_ptr _tmp92;struct _tuple2 _tmp90=*(*((struct
_tuple2**)_check_dyneither_subscript(_tmp41,sizeof(struct _tuple2*),i)));_tmp91=
_tmp90.f1;_tmp92=_tmp90.f2;({struct Cyc_Int_pa_struct _tmp97;_tmp97.tag=1;_tmp97.f1=(
unsigned long)((int)(i != _get_dyneither_size(_tmp41,sizeof(struct _tuple2*))- 1?',':' '));{
struct Cyc_String_pa_struct _tmp96;_tmp96.tag=0;_tmp96.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp92);{struct Cyc_Int_pa_struct _tmp95;_tmp95.tag=1;_tmp95.f1=(
unsigned long)((int)_tmp91);{void*_tmp93[3]={& _tmp95,& _tmp96,& _tmp97};Cyc_printf(({
const char*_tmp94="$(%d,%s)%c";_tag_dyneither(_tmp94,sizeof(char),11);}),
_tag_dyneither(_tmp93,sizeof(void*),3));}}}});}}({void*_tmp98=0;Cyc_printf(({
const char*_tmp99="],[";_tag_dyneither(_tmp99,sizeof(char),4);}),_tag_dyneither(
_tmp98,sizeof(void*),0));});{int i=0;for(0;i < _get_dyneither_size(_tmp42,sizeof(
struct _tuple0*));++ i){unsigned int _tmp9B;struct _dyneither_ptr _tmp9C;void*_tmp9D;
struct _tuple0 _tmp9A=*(*((struct _tuple0**)_check_dyneither_subscript(_tmp42,
sizeof(struct _tuple0*),i)));_tmp9B=_tmp9A.f1;_tmp9C=_tmp9A.f2;_tmp9D=_tmp9A.f3;({
struct Cyc_String_pa_struct _tmpA1;_tmpA1.tag=0;_tmpA1.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp9C);{struct Cyc_Int_pa_struct _tmpA0;_tmpA0.tag=1;_tmpA0.f1=(
unsigned long)((int)_tmp9B);{void*_tmp9E[2]={& _tmpA0,& _tmpA1};Cyc_printf(({const
char*_tmp9F="$(%d,%s,";_tag_dyneither(_tmp9F,sizeof(char),9);}),_tag_dyneither(
_tmp9E,sizeof(void*),2));}}});Cyc_Typerep_print_typestruct(_tmp9D);({struct Cyc_Int_pa_struct
_tmpA4;_tmpA4.tag=1;_tmpA4.f1=(unsigned long)((int)(i != _get_dyneither_size(
_tmp42,sizeof(struct _tuple0*))- 1?',':' '));{void*_tmpA2[1]={& _tmpA4};Cyc_printf(({
const char*_tmpA3=")%c";_tag_dyneither(_tmpA3,sizeof(char),4);}),_tag_dyneither(
_tmpA2,sizeof(void*),1));}});}}({void*_tmpA5=0;Cyc_printf(({const char*_tmpA6="] )";
_tag_dyneither(_tmpA6,sizeof(char),4);}),_tag_dyneither(_tmpA5,sizeof(void*),0));});
goto _LL1E;_LL31: if(_tmp31 <= (void*)2)goto _LL33;if(*((int*)_tmp31)!= 7)goto _LL33;
_tmp43=((struct Cyc_Typerep_XTUnion_struct*)_tmp31)->f1;_tmp44=((struct Cyc_Typerep_XTUnion_struct*)
_tmp31)->f2;_LL32:({struct Cyc_String_pa_struct _tmpA9;_tmpA9.tag=0;_tmpA9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp43);{void*_tmpA7[1]={& _tmpA9};
Cyc_printf(({const char*_tmpA8="XTUnion(%s,";_tag_dyneither(_tmpA8,sizeof(char),
12);}),_tag_dyneither(_tmpA7,sizeof(void*),1));}});{int i=0;for(0;i < 
_get_dyneither_size(_tmp44,sizeof(struct _tuple3*));++ i){struct _dyneither_ptr
_tmpAB;void*_tmpAC;struct _tuple3 _tmpAA=*(*((struct _tuple3**)
_check_dyneither_subscript(_tmp44,sizeof(struct _tuple3*),i)));_tmpAB=_tmpAA.f1;
_tmpAC=_tmpAA.f2;({struct Cyc_String_pa_struct _tmpAF;_tmpAF.tag=0;_tmpAF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_tmpAB,sizeof(
char),4));{void*_tmpAD[1]={& _tmpAF};Cyc_printf(({const char*_tmpAE="$(%s,";
_tag_dyneither(_tmpAE,sizeof(char),6);}),_tag_dyneither(_tmpAD,sizeof(void*),1));}});
Cyc_Typerep_print_typestruct(_tmpAC);({struct Cyc_Int_pa_struct _tmpB2;_tmpB2.tag=
1;_tmpB2.f1=(unsigned long)((int)(i != _get_dyneither_size(_tmp44,sizeof(struct
_tuple3*))- 1?',':' '));{void*_tmpB0[1]={& _tmpB2};Cyc_printf(({const char*_tmpB1=")%c";
_tag_dyneither(_tmpB1,sizeof(char),4);}),_tag_dyneither(_tmpB0,sizeof(void*),1));}});}}({
void*_tmpB3=0;Cyc_printf(({const char*_tmpB4=" )";_tag_dyneither(_tmpB4,sizeof(
char),3);}),_tag_dyneither(_tmpB3,sizeof(void*),0));});goto _LL1E;_LL33: if(_tmp31
<= (void*)2)goto _LL35;if(*((int*)_tmp31)!= 8)goto _LL35;_tmp45=((struct Cyc_Typerep_Union_struct*)
_tmp31)->f1;_tmp46=((struct Cyc_Typerep_Union_struct*)_tmp31)->f2;_tmp47=((struct
Cyc_Typerep_Union_struct*)_tmp31)->f3;_LL34:({struct Cyc_Int_pa_struct _tmpB8;
_tmpB8.tag=1;_tmpB8.f1=(unsigned long)_tmp46;{struct Cyc_String_pa_struct _tmpB7;
_tmpB7.tag=0;_tmpB7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Typerep_anon_or_name(
_tmp45));{void*_tmpB5[2]={& _tmpB7,& _tmpB8};Cyc_printf(({const char*_tmpB6="Union(%s,%d,";
_tag_dyneither(_tmpB6,sizeof(char),13);}),_tag_dyneither(_tmpB5,sizeof(void*),2));}}});{
int i=0;for(0;i < _get_dyneither_size(_tmp47,sizeof(struct _tuple3*));++ i){struct
_dyneither_ptr _tmpBA;void*_tmpBB;struct _tuple3 _tmpB9=*(*((struct _tuple3**)
_check_dyneither_subscript(_tmp47,sizeof(struct _tuple3*),i)));_tmpBA=_tmpB9.f1;
_tmpBB=_tmpB9.f2;({struct Cyc_String_pa_struct _tmpBE;_tmpBE.tag=0;_tmpBE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBA);{void*_tmpBC[1]={& _tmpBE};
Cyc_printf(({const char*_tmpBD="$(%s,";_tag_dyneither(_tmpBD,sizeof(char),6);}),
_tag_dyneither(_tmpBC,sizeof(void*),1));}});Cyc_Typerep_print_typestruct(_tmpBB);({
struct Cyc_Int_pa_struct _tmpC1;_tmpC1.tag=1;_tmpC1.f1=(unsigned long)((int)(i != 
_get_dyneither_size(_tmp47,sizeof(struct _tuple3*))- 1?',':' '));{void*_tmpBF[1]={&
_tmpC1};Cyc_printf(({const char*_tmpC0=")%c";_tag_dyneither(_tmpC0,sizeof(char),4);}),
_tag_dyneither(_tmpBF,sizeof(void*),1));}});}}({void*_tmpC2=0;Cyc_printf(({const
char*_tmpC3=")";_tag_dyneither(_tmpC3,sizeof(char),2);}),_tag_dyneither(_tmpC2,
sizeof(void*),0));});goto _LL1E;_LL35: if(_tmp31 <= (void*)2)goto _LL1E;if(*((int*)
_tmp31)!= 9)goto _LL1E;_tmp48=((struct Cyc_Typerep_Enum_struct*)_tmp31)->f1;_tmp49=((
struct Cyc_Typerep_Enum_struct*)_tmp31)->f2;_tmp4A=((struct Cyc_Typerep_Enum_struct*)
_tmp31)->f3;_LL36:({struct Cyc_Int_pa_struct _tmpC7;_tmpC7.tag=1;_tmpC7.f1=(
unsigned long)_tmp49;{struct Cyc_String_pa_struct _tmpC6;_tmpC6.tag=0;_tmpC6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Typerep_anon_or_name(_tmp48));{
void*_tmpC4[2]={& _tmpC6,& _tmpC7};Cyc_printf(({const char*_tmpC5="Enum(%s,%d,";
_tag_dyneither(_tmpC5,sizeof(char),12);}),_tag_dyneither(_tmpC4,sizeof(void*),2));}}});{
int i=0;for(0;i < _get_dyneither_size(_tmp4A,sizeof(struct _tuple2*));++ i){
unsigned int _tmpC9;struct _dyneither_ptr _tmpCA;struct _tuple2 _tmpC8=*(*((struct
_tuple2**)_check_dyneither_subscript(_tmp4A,sizeof(struct _tuple2*),i)));_tmpC9=
_tmpC8.f1;_tmpCA=_tmpC8.f2;({struct Cyc_Int_pa_struct _tmpCF;_tmpCF.tag=1;_tmpCF.f1=(
unsigned long)((int)(i != _get_dyneither_size(_tmp4A,sizeof(struct _tuple2*))- 1?',':' '));{
struct Cyc_String_pa_struct _tmpCE;_tmpCE.tag=0;_tmpCE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmpCA);{struct Cyc_Int_pa_struct _tmpCD;_tmpCD.tag=1;_tmpCD.f1=(
unsigned long)((int)_tmpC9);{void*_tmpCB[3]={& _tmpCD,& _tmpCE,& _tmpCF};Cyc_printf(({
const char*_tmpCC="$(%d,%s)%c";_tag_dyneither(_tmpCC,sizeof(char),11);}),
_tag_dyneither(_tmpCB,sizeof(void*),3));}}}});}}({void*_tmpD0=0;Cyc_printf(({
const char*_tmpD1=")";_tag_dyneither(_tmpD1,sizeof(char),2);}),_tag_dyneither(
_tmpD0,sizeof(void*),0));});goto _LL1E;_LL1E:;}void Cyc_Typerep_xmlize_typestruct(
void*rep){void*_tmpD2=rep;int _tmpD3;unsigned int _tmpD4;unsigned int _tmpD5;void*
_tmpD6;void*_tmpD7;struct _dyneither_ptr*_tmpD8;unsigned int _tmpD9;struct
_dyneither_ptr _tmpDA;unsigned int _tmpDB;struct _dyneither_ptr _tmpDC;struct
_dyneither_ptr _tmpDD;struct _dyneither_ptr _tmpDE;unsigned int _tmpDF;struct
_dyneither_ptr _tmpE0;struct _dyneither_ptr _tmpE1;struct _dyneither_ptr _tmpE2;
struct _dyneither_ptr _tmpE3;struct _dyneither_ptr _tmpE4;struct _dyneither_ptr _tmpE5;
struct _dyneither_ptr*_tmpE6;int _tmpE7;struct _dyneither_ptr _tmpE8;struct
_dyneither_ptr*_tmpE9;int _tmpEA;struct _dyneither_ptr _tmpEB;_LL38: if(_tmpD2 <= (
void*)2)goto _LL3A;if(*((int*)_tmpD2)!= 0)goto _LL3A;_tmpD3=((struct Cyc_Typerep_Int_struct*)
_tmpD2)->f1;_tmpD4=((struct Cyc_Typerep_Int_struct*)_tmpD2)->f2;_LL39:({struct Cyc_Int_pa_struct
_tmpEF;_tmpEF.tag=1;_tmpEF.f1=(unsigned long)((int)_tmpD4);{struct Cyc_Int_pa_struct
_tmpEE;_tmpEE.tag=1;_tmpEE.f1=(unsigned long)_tmpD3;{void*_tmpEC[2]={& _tmpEE,&
_tmpEF};Cyc_printf(({const char*_tmpED="<Int sn=%d sz=\"%d\"/>";_tag_dyneither(
_tmpED,sizeof(char),21);}),_tag_dyneither(_tmpEC,sizeof(void*),2));}}});goto
_LL37;_LL3A: if((int)_tmpD2 != 0)goto _LL3C;_LL3B:({void*_tmpF0=0;Cyc_printf(({
const char*_tmpF1="<Float/>";_tag_dyneither(_tmpF1,sizeof(char),9);}),
_tag_dyneither(_tmpF0,sizeof(void*),0));});goto _LL37;_LL3C: if((int)_tmpD2 != 1)
goto _LL3E;_LL3D:({void*_tmpF2=0;Cyc_printf(({const char*_tmpF3="<Double/>";
_tag_dyneither(_tmpF3,sizeof(char),10);}),_tag_dyneither(_tmpF2,sizeof(void*),0));});
goto _LL37;_LL3E: if(_tmpD2 <= (void*)2)goto _LL40;if(*((int*)_tmpD2)!= 1)goto _LL40;
_tmpD5=((struct Cyc_Typerep_ThinPtr_struct*)_tmpD2)->f1;_tmpD6=(void*)((struct Cyc_Typerep_ThinPtr_struct*)
_tmpD2)->f2;_LL3F:({struct Cyc_Int_pa_struct _tmpF6;_tmpF6.tag=1;_tmpF6.f1=(
unsigned long)((int)_tmpD5);{void*_tmpF4[1]={& _tmpF6};Cyc_printf(({const char*
_tmpF5="<ThinPtr sz=\"%d\">";_tag_dyneither(_tmpF5,sizeof(char),18);}),
_tag_dyneither(_tmpF4,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(_tmpD6);({
void*_tmpF7=0;Cyc_printf(({const char*_tmpF8="</ThinPtr";_tag_dyneither(_tmpF8,
sizeof(char),10);}),_tag_dyneither(_tmpF7,sizeof(void*),0));});goto _LL37;_LL40:
if(_tmpD2 <= (void*)2)goto _LL42;if(*((int*)_tmpD2)!= 2)goto _LL42;_tmpD7=(void*)((
struct Cyc_Typerep_FatPtr_struct*)_tmpD2)->f1;_LL41:({void*_tmpF9=0;Cyc_printf(({
const char*_tmpFA="<FatPtr>";_tag_dyneither(_tmpFA,sizeof(char),9);}),
_tag_dyneither(_tmpF9,sizeof(void*),0));});Cyc_Typerep_xmlize_typestruct(_tmpD7);({
void*_tmpFB=0;Cyc_printf(({const char*_tmpFC="</FatPtr>";_tag_dyneither(_tmpFC,
sizeof(char),10);}),_tag_dyneither(_tmpFB,sizeof(void*),0));});goto _LL37;_LL42:
if(_tmpD2 <= (void*)2)goto _LL44;if(*((int*)_tmpD2)!= 3)goto _LL44;_tmpD8=((struct
Cyc_Typerep_Struct_struct*)_tmpD2)->f1;_tmpD9=((struct Cyc_Typerep_Struct_struct*)
_tmpD2)->f2;_tmpDA=((struct Cyc_Typerep_Struct_struct*)_tmpD2)->f3;_LL43:({struct
Cyc_Int_pa_struct _tmp100;_tmp100.tag=1;_tmp100.f1=(unsigned long)((int)_tmpD9);{
struct Cyc_String_pa_struct _tmpFF;_tmpFF.tag=0;_tmpFF.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Typerep_anon_or_name(_tmpD8));{void*_tmpFD[2]={& _tmpFF,&
_tmp100};Cyc_printf(({const char*_tmpFE="<Struct name=\"%s\" sz=\"%d\">";
_tag_dyneither(_tmpFE,sizeof(char),27);}),_tag_dyneither(_tmpFD,sizeof(void*),2));}}});{
int i=0;for(0;i < _get_dyneither_size(_tmpDA,sizeof(struct _tuple0*));++ i){
unsigned int _tmp102;struct _dyneither_ptr _tmp103;void*_tmp104;struct _tuple0
_tmp101=*(*((struct _tuple0**)_check_dyneither_subscript(_tmpDA,sizeof(struct
_tuple0*),i)));_tmp102=_tmp101.f1;_tmp103=_tmp101.f2;_tmp104=_tmp101.f3;({struct
Cyc_String_pa_struct _tmp108;_tmp108.tag=0;_tmp108.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp103);{struct Cyc_Int_pa_struct _tmp107;_tmp107.tag=1;
_tmp107.f1=(unsigned long)((int)_tmp102);{void*_tmp105[2]={& _tmp107,& _tmp108};
Cyc_printf(({const char*_tmp106="<Component ofs=\"%d\" nm=\"%s\">";_tag_dyneither(
_tmp106,sizeof(char),29);}),_tag_dyneither(_tmp105,sizeof(void*),2));}}});Cyc_Typerep_xmlize_typestruct(
_tmp104);({void*_tmp109=0;Cyc_printf(({const char*_tmp10A="</Component>";
_tag_dyneither(_tmp10A,sizeof(char),13);}),_tag_dyneither(_tmp109,sizeof(void*),
0));});}}({void*_tmp10B=0;Cyc_printf(({const char*_tmp10C="</Struct>";
_tag_dyneither(_tmp10C,sizeof(char),10);}),_tag_dyneither(_tmp10B,sizeof(void*),
0));});goto _LL37;_LL44: if(_tmpD2 <= (void*)2)goto _LL46;if(*((int*)_tmpD2)!= 4)
goto _LL46;_tmpDB=((struct Cyc_Typerep_Tuple_struct*)_tmpD2)->f1;_tmpDC=((struct
Cyc_Typerep_Tuple_struct*)_tmpD2)->f2;_LL45:({struct Cyc_Int_pa_struct _tmp10F;
_tmp10F.tag=1;_tmp10F.f1=(unsigned long)((int)_tmpDB);{void*_tmp10D[1]={& _tmp10F};
Cyc_printf(({const char*_tmp10E="<Tuple sz=\"%d\">";_tag_dyneither(_tmp10E,
sizeof(char),16);}),_tag_dyneither(_tmp10D,sizeof(void*),1));}});{int i=0;for(0;i
< _get_dyneither_size(_tmpDC,sizeof(struct _tuple1*));++ i){unsigned int _tmp111;
void*_tmp112;struct _tuple1 _tmp110=*(*((struct _tuple1**)
_check_dyneither_subscript(_tmpDC,sizeof(struct _tuple1*),i)));_tmp111=_tmp110.f1;
_tmp112=_tmp110.f2;({struct Cyc_Int_pa_struct _tmp115;_tmp115.tag=1;_tmp115.f1=(
unsigned long)((int)_tmp111);{void*_tmp113[1]={& _tmp115};Cyc_printf(({const char*
_tmp114="<Component ofs=\"%d\">";_tag_dyneither(_tmp114,sizeof(char),21);}),
_tag_dyneither(_tmp113,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(
_tmp112);({void*_tmp116=0;Cyc_printf(({const char*_tmp117="</Component>";
_tag_dyneither(_tmp117,sizeof(char),13);}),_tag_dyneither(_tmp116,sizeof(void*),
0));});}}({void*_tmp118=0;Cyc_printf(({const char*_tmp119="</Tuple>";
_tag_dyneither(_tmp119,sizeof(char),9);}),_tag_dyneither(_tmp118,sizeof(void*),0));});
goto _LL37;_LL46: if(_tmpD2 <= (void*)2)goto _LL48;if(*((int*)_tmpD2)!= 6)goto _LL48;
_tmpDD=((struct Cyc_Typerep_TUnionField_struct*)_tmpD2)->f1;_tmpDE=((struct Cyc_Typerep_TUnionField_struct*)
_tmpD2)->f2;_tmpDF=((struct Cyc_Typerep_TUnionField_struct*)_tmpD2)->f3;_tmpE0=((
struct Cyc_Typerep_TUnionField_struct*)_tmpD2)->f4;_LL47:({struct Cyc_Int_pa_struct
_tmp11E;_tmp11E.tag=1;_tmp11E.f1=(unsigned long)((int)_tmpDF);{struct Cyc_String_pa_struct
_tmp11D;_tmp11D.tag=0;_tmp11D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmpDE);{struct Cyc_String_pa_struct _tmp11C;_tmp11C.tag=0;_tmp11C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmpDD);{void*_tmp11A[3]={& _tmp11C,&
_tmp11D,& _tmp11E};Cyc_printf(({const char*_tmp11B="<TUnionField tname=\"%s\" fname=\"%s\" sz=\"%d\">";
_tag_dyneither(_tmp11B,sizeof(char),44);}),_tag_dyneither(_tmp11A,sizeof(void*),
3));}}}});{int i=0;for(0;i < _get_dyneither_size(_tmpE0,sizeof(struct _tuple1*));++
i){unsigned int _tmp120;void*_tmp121;struct _tuple1 _tmp11F=*(*((struct _tuple1**)
_check_dyneither_subscript(_tmpE0,sizeof(struct _tuple1*),i)));_tmp120=_tmp11F.f1;
_tmp121=_tmp11F.f2;({struct Cyc_Int_pa_struct _tmp124;_tmp124.tag=1;_tmp124.f1=(
unsigned long)((int)_tmp120);{void*_tmp122[1]={& _tmp124};Cyc_printf(({const char*
_tmp123="<Component ofs=\"%d\">";_tag_dyneither(_tmp123,sizeof(char),21);}),
_tag_dyneither(_tmp122,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(
_tmp121);({void*_tmp125=0;Cyc_printf(({const char*_tmp126="</Component>";
_tag_dyneither(_tmp126,sizeof(char),13);}),_tag_dyneither(_tmp125,sizeof(void*),
0));});}}({void*_tmp127=0;Cyc_printf(({const char*_tmp128="</TUnionField>";
_tag_dyneither(_tmp128,sizeof(char),15);}),_tag_dyneither(_tmp127,sizeof(void*),
0));});goto _LL37;_LL48: if(_tmpD2 <= (void*)2)goto _LL4A;if(*((int*)_tmpD2)!= 5)
goto _LL4A;_tmpE1=((struct Cyc_Typerep_TUnion_struct*)_tmpD2)->f1;_tmpE2=((struct
Cyc_Typerep_TUnion_struct*)_tmpD2)->f2;_tmpE3=((struct Cyc_Typerep_TUnion_struct*)
_tmpD2)->f3;_LL49:({struct Cyc_String_pa_struct _tmp12B;_tmp12B.tag=0;_tmp12B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE1);{void*_tmp129[1]={& _tmp12B};
Cyc_printf(({const char*_tmp12A="<TUnion name=\"%s\">";_tag_dyneither(_tmp12A,
sizeof(char),19);}),_tag_dyneither(_tmp129,sizeof(void*),1));}});{int i=0;for(0;i
< _get_dyneither_size(_tmpE2,sizeof(struct _tuple2*));++ i){unsigned int _tmp12D;
struct _dyneither_ptr _tmp12E;struct _tuple2 _tmp12C=*(*((struct _tuple2**)
_check_dyneither_subscript(_tmpE2,sizeof(struct _tuple2*),i)));_tmp12D=_tmp12C.f1;
_tmp12E=_tmp12C.f2;({struct Cyc_String_pa_struct _tmp132;_tmp132.tag=0;_tmp132.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp12E);{struct Cyc_Int_pa_struct
_tmp131;_tmp131.tag=1;_tmp131.f1=(unsigned long)((int)_tmp12D);{void*_tmp12F[2]={&
_tmp131,& _tmp132};Cyc_printf(({const char*_tmp130="<Tag tag=\"%d\" nm=\"%s\"/>";
_tag_dyneither(_tmp130,sizeof(char),24);}),_tag_dyneither(_tmp12F,sizeof(void*),
2));}}});}}{int i=0;for(0;i < _get_dyneither_size(_tmpE3,sizeof(struct _tuple0*));
++ i){unsigned int _tmp134;struct _dyneither_ptr _tmp135;void*_tmp136;struct _tuple0
_tmp133=*(*((struct _tuple0**)_check_dyneither_subscript(_tmpE3,sizeof(struct
_tuple0*),i)));_tmp134=_tmp133.f1;_tmp135=_tmp133.f2;_tmp136=_tmp133.f3;({struct
Cyc_String_pa_struct _tmp13A;_tmp13A.tag=0;_tmp13A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp135);{struct Cyc_Int_pa_struct _tmp139;_tmp139.tag=1;
_tmp139.f1=(unsigned long)((int)_tmp134);{void*_tmp137[2]={& _tmp139,& _tmp13A};
Cyc_printf(({const char*_tmp138="<Tag tag=\"%d\" nm=\"%s\">";_tag_dyneither(
_tmp138,sizeof(char),23);}),_tag_dyneither(_tmp137,sizeof(void*),2));}}});Cyc_Typerep_xmlize_typestruct(
_tmp136);({void*_tmp13B=0;Cyc_printf(({const char*_tmp13C="</Tag>";_tag_dyneither(
_tmp13C,sizeof(char),7);}),_tag_dyneither(_tmp13B,sizeof(void*),0));});}}({void*
_tmp13D=0;Cyc_printf(({const char*_tmp13E="</Tunion>";_tag_dyneither(_tmp13E,
sizeof(char),10);}),_tag_dyneither(_tmp13D,sizeof(void*),0));});goto _LL37;_LL4A:
if(_tmpD2 <= (void*)2)goto _LL4C;if(*((int*)_tmpD2)!= 7)goto _LL4C;_tmpE4=((struct
Cyc_Typerep_XTUnion_struct*)_tmpD2)->f1;_tmpE5=((struct Cyc_Typerep_XTUnion_struct*)
_tmpD2)->f2;_LL4B:({struct Cyc_String_pa_struct _tmp141;_tmp141.tag=0;_tmp141.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE4);{void*_tmp13F[1]={& _tmp141};
Cyc_printf(({const char*_tmp140="<XTUnion name=\"%s\">";_tag_dyneither(_tmp140,
sizeof(char),20);}),_tag_dyneither(_tmp13F,sizeof(void*),1));}});{int i=0;for(0;i
< _get_dyneither_size(_tmpE5,sizeof(struct _tuple3*));++ i){struct _dyneither_ptr
_tmp143;void*_tmp144;struct _tuple3 _tmp142=*(*((struct _tuple3**)
_check_dyneither_subscript(_tmpE5,sizeof(struct _tuple3*),i)));_tmp143=_tmp142.f1;
_tmp144=_tmp142.f2;({struct Cyc_String_pa_struct _tmp147;_tmp147.tag=0;_tmp147.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_tmp143,sizeof(
char),4));{void*_tmp145[1]={& _tmp147};Cyc_printf(({const char*_tmp146="<Tag tag=\"%s\">";
_tag_dyneither(_tmp146,sizeof(char),15);}),_tag_dyneither(_tmp145,sizeof(void*),
1));}});Cyc_Typerep_xmlize_typestruct(_tmp144);({void*_tmp148=0;Cyc_printf(({
const char*_tmp149="</Tag>";_tag_dyneither(_tmp149,sizeof(char),7);}),
_tag_dyneither(_tmp148,sizeof(void*),0));});}}({void*_tmp14A=0;Cyc_printf(({
const char*_tmp14B="</XTunion>";_tag_dyneither(_tmp14B,sizeof(char),11);}),
_tag_dyneither(_tmp14A,sizeof(void*),0));});goto _LL37;_LL4C: if(_tmpD2 <= (void*)2)
goto _LL4E;if(*((int*)_tmpD2)!= 8)goto _LL4E;_tmpE6=((struct Cyc_Typerep_Union_struct*)
_tmpD2)->f1;_tmpE7=((struct Cyc_Typerep_Union_struct*)_tmpD2)->f2;_tmpE8=((struct
Cyc_Typerep_Union_struct*)_tmpD2)->f3;_LL4D:({struct Cyc_Int_pa_struct _tmp14F;
_tmp14F.tag=1;_tmp14F.f1=(unsigned long)_tmpE7;{struct Cyc_String_pa_struct
_tmp14E;_tmp14E.tag=0;_tmp14E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Typerep_anon_or_name(_tmpE6));{void*_tmp14C[2]={& _tmp14E,& _tmp14F};Cyc_printf(({
const char*_tmp14D="<Union name=\"%s\" sz=\"%d\">";_tag_dyneither(_tmp14D,sizeof(
char),26);}),_tag_dyneither(_tmp14C,sizeof(void*),2));}}});{int i=0;for(0;i < 
_get_dyneither_size(_tmpE8,sizeof(struct _tuple3*));++ i){struct _dyneither_ptr
_tmp151;void*_tmp152;struct _tuple3 _tmp150=*(*((struct _tuple3**)
_check_dyneither_subscript(_tmpE8,sizeof(struct _tuple3*),i)));_tmp151=_tmp150.f1;
_tmp152=_tmp150.f2;({struct Cyc_String_pa_struct _tmp155;_tmp155.tag=0;_tmp155.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp151);{void*_tmp153[1]={& _tmp155};
Cyc_printf(({const char*_tmp154="<Case nm=\"%s\">";_tag_dyneither(_tmp154,sizeof(
char),15);}),_tag_dyneither(_tmp153,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(
_tmp152);({void*_tmp156=0;Cyc_printf(({const char*_tmp157="</Case>";
_tag_dyneither(_tmp157,sizeof(char),8);}),_tag_dyneither(_tmp156,sizeof(void*),0));});}}({
void*_tmp158=0;Cyc_printf(({const char*_tmp159="</Union>";_tag_dyneither(_tmp159,
sizeof(char),9);}),_tag_dyneither(_tmp158,sizeof(void*),0));});goto _LL37;_LL4E:
if(_tmpD2 <= (void*)2)goto _LL37;if(*((int*)_tmpD2)!= 9)goto _LL37;_tmpE9=((struct
Cyc_Typerep_Enum_struct*)_tmpD2)->f1;_tmpEA=((struct Cyc_Typerep_Enum_struct*)
_tmpD2)->f2;_tmpEB=((struct Cyc_Typerep_Enum_struct*)_tmpD2)->f3;_LL4F:({struct
Cyc_Int_pa_struct _tmp15D;_tmp15D.tag=1;_tmp15D.f1=(unsigned long)_tmpEA;{struct
Cyc_String_pa_struct _tmp15C;_tmp15C.tag=0;_tmp15C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Typerep_anon_or_name(_tmpE9));{void*_tmp15A[2]={&
_tmp15C,& _tmp15D};Cyc_printf(({const char*_tmp15B="<Enum name=\"%s\" szb=\"%d\">";
_tag_dyneither(_tmp15B,sizeof(char),26);}),_tag_dyneither(_tmp15A,sizeof(void*),
2));}}});{int i=0;for(0;i < _get_dyneither_size(_tmpEB,sizeof(struct _tuple2*));++ i){
unsigned int _tmp15F;struct _dyneither_ptr _tmp160;struct _tuple2 _tmp15E=*(*((struct
_tuple2**)_check_dyneither_subscript(_tmpEB,sizeof(struct _tuple2*),i)));_tmp15F=
_tmp15E.f1;_tmp160=_tmp15E.f2;({struct Cyc_String_pa_struct _tmp164;_tmp164.tag=0;
_tmp164.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp160);{struct Cyc_Int_pa_struct
_tmp163;_tmp163.tag=1;_tmp163.f1=(unsigned long)((int)_tmp15F);{void*_tmp161[2]={&
_tmp163,& _tmp164};Cyc_printf(({const char*_tmp162="<Val val=\"%d\">%s</Val>";
_tag_dyneither(_tmp162,sizeof(char),23);}),_tag_dyneither(_tmp161,sizeof(void*),
2));}}});}}({void*_tmp165=0;Cyc_printf(({const char*_tmp166="</Enum>";
_tag_dyneither(_tmp166,sizeof(char),8);}),_tag_dyneither(_tmp165,sizeof(void*),0));});
goto _LL37;_LL37:;}
