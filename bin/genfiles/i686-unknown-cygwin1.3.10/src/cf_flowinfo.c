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
void*v;};int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_rlist(struct
_RegionHandle*,struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);
void*Cyc_List_hd(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_tl(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void Cyc_List_iter_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rappend(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct
Cyc_List_List*Cyc_List_delete_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,
void*x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char
Cyc_Set_Absent[11];struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct
_dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_rempty(struct
_RegionHandle*,int(*cmp)(void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict d,
void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct
Cyc_Dict_Dict d,void*k,void**ans);void Cyc_Dict_iter(void(*f)(void*,void*),struct
Cyc_Dict_Dict d);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct
Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,
void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct Cyc_Dict_Dict
Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2);int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),
struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct _tuple0{void*f1;void*f2;};
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);struct
Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);
struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;
int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
int Cyc_Position_segment_equals(struct Cyc_Position_Segment*,struct Cyc_Position_Segment*);
struct _dyneither_ptr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);
struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);int Cyc_Absyn_is_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
_dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_typecmp(void*,void*);int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{
int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct
Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessNumelts_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};struct Cyc_CfFlowInfo_LessEqNumelts_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};union Cyc_CfFlowInfo_RelnOp_union{struct Cyc_CfFlowInfo_EqualConst_struct
EqualConst;struct Cyc_CfFlowInfo_LessVar_struct LessVar;struct Cyc_CfFlowInfo_LessNumelts_struct
LessNumelts;struct Cyc_CfFlowInfo_LessConst_struct LessConst;struct Cyc_CfFlowInfo_LessEqNumelts_struct
LessEqNumelts;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp_union
rop;};struct Cyc_CfFlowInfo_TagCmp{void*cmp;void*bd;};char Cyc_CfFlowInfo_HasTagCmps[
15]="\000\000\000\000HasTagCmps\000";struct Cyc_CfFlowInfo_HasTagCmps_struct{char*
tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_IsZero[11]="\000\000\000\000IsZero\000";
char Cyc_CfFlowInfo_NotZero[12]="\000\000\000\000NotZero\000";struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[13]="\000\000\000\000UnknownZ\000";
struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};struct Cyc_List_List*
Cyc_CfFlowInfo_copy_tagcmps(struct _RegionHandle*,struct Cyc_List_List*);struct Cyc_CfFlowInfo_PlaceL_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UnknownL_struct{int
tag;};union Cyc_CfFlowInfo_AbsLVal_union{struct Cyc_CfFlowInfo_PlaceL_struct PlaceL;
struct Cyc_CfFlowInfo_UnknownL_struct UnknownL;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct
_dyneither_ptr f1;};int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int
disjoint);int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict
s2);int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict
s2);struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict consumed;struct Cyc_List_List*
may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(struct
_RegionHandle*,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2);int Cyc_CfFlowInfo_consume_approx(struct Cyc_CfFlowInfo_ConsumeInfo c1,struct
Cyc_CfFlowInfo_ConsumeInfo c2);struct Cyc_CfFlowInfo_BottomFL_struct{int tag;};
struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;struct Cyc_List_List*
f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r);int Cyc_CfFlowInfo_root_cmp(void*,void*);int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(
struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(union
Cyc_CfFlowInfo_FlowInfo_union f1,union Cyc_CfFlowInfo_FlowInfo_union f2);void Cyc_CfFlowInfo_print_absrval(
void*rval);void Cyc_CfFlowInfo_print_initlevel(void*il);void Cyc_CfFlowInfo_print_root(
void*root);void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_place_set(
struct Cyc_Dict_Dict p);void Cyc_CfFlowInfo_print_place_list(struct Cyc_List_List*p);
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);void Cyc_CfFlowInfo_print_flow(
union Cyc_CfFlowInfo_FlowInfo_union f);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct
Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,
struct Cyc_List_List*);void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_join_flow(struct
Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo_union,
union Cyc_CfFlowInfo_FlowInfo_union,int);struct _tuple4{union Cyc_CfFlowInfo_FlowInfo_union
f1;void*f2;};struct _tuple4 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*all_changed,struct _tuple4 pr1,struct _tuple4 pr2,int);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,
union Cyc_CfFlowInfo_FlowInfo_union,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Dict_Dict,
struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union f,void*rgn);
struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_consume_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);void Cyc_CfFlowInfo_check_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo_union f);struct _tuple5{struct Cyc_CfFlowInfo_ConsumeInfo
f1;union Cyc_CfFlowInfo_FlowInfo_union f2;};struct _tuple5 Cyc_CfFlowInfo_save_consume_info(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo_union f,int clear);
union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo_union
f,struct Cyc_CfFlowInfo_ConsumeInfo c);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*
p1,struct Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,(union Cyc_Absyn_Cnst_union)((
struct Cyc_Absyn_Null_c_struct){6})};static struct Cyc_Absyn_Exp dummy_exp={0,(void*)((
void*)& dummy_rawexp),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};return({struct Cyc_CfFlowInfo_FlowEnv*
_tmp0=_region_malloc(r,sizeof(*_tmp0));_tmp0->r=r;_tmp0->unknown_none=(void*)((
void*)({struct Cyc_CfFlowInfo_UnknownR_struct*_tmpE=_region_malloc(r,sizeof(*
_tmpE));_tmpE[0]=({struct Cyc_CfFlowInfo_UnknownR_struct _tmpF;_tmpF.tag=0;_tmpF.f1=(
void*)((void*)0);_tmpF;});_tmpE;}));_tmp0->unknown_this=(void*)((void*)({struct
Cyc_CfFlowInfo_UnknownR_struct*_tmpC=_region_malloc(r,sizeof(*_tmpC));_tmpC[0]=({
struct Cyc_CfFlowInfo_UnknownR_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(void*)((void*)1);
_tmpD;});_tmpC;}));_tmp0->unknown_all=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownR_struct*
_tmpA=_region_malloc(r,sizeof(*_tmpA));_tmpA[0]=({struct Cyc_CfFlowInfo_UnknownR_struct
_tmpB;_tmpB.tag=0;_tmpB.f1=(void*)((void*)2);_tmpB;});_tmpA;}));_tmp0->esc_none=(
void*)((void*)({struct Cyc_CfFlowInfo_Esc_struct*_tmp8=_region_malloc(r,sizeof(*
_tmp8));_tmp8[0]=({struct Cyc_CfFlowInfo_Esc_struct _tmp9;_tmp9.tag=1;_tmp9.f1=(
void*)((void*)0);_tmp9;});_tmp8;}));_tmp0->esc_this=(void*)((void*)({struct Cyc_CfFlowInfo_Esc_struct*
_tmp6=_region_malloc(r,sizeof(*_tmp6));_tmp6[0]=({struct Cyc_CfFlowInfo_Esc_struct
_tmp7;_tmp7.tag=1;_tmp7.f1=(void*)((void*)1);_tmp7;});_tmp6;}));_tmp0->esc_all=(
void*)((void*)({struct Cyc_CfFlowInfo_Esc_struct*_tmp4=_region_malloc(r,sizeof(*
_tmp4));_tmp4[0]=({struct Cyc_CfFlowInfo_Esc_struct _tmp5;_tmp5.tag=1;_tmp5.f1=(
void*)((void*)2);_tmp5;});_tmp4;}));_tmp0->mt_flowdict=Cyc_Dict_rempty(r,Cyc_CfFlowInfo_root_cmp);
_tmp0->mt_place_set=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp);
_tmp0->dummy_place=({struct Cyc_CfFlowInfo_Place*_tmp1=_region_malloc(r,sizeof(*
_tmp1));_tmp1->root=(void*)((void*)({struct Cyc_CfFlowInfo_MallocPt_struct*_tmp2=
_region_malloc(r,sizeof(*_tmp2));_tmp2[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp3;_tmp3.tag=1;_tmp3.f1=& dummy_exp;_tmp3.f2=(void*)((void*)0);_tmp3;});_tmp2;}));
_tmp1->fields=0;_tmp1;});_tmp0;});}int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc){if(set != 0){
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(*
set,place))return 1;else{*set=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k,struct Cyc_Position_Segment*v))Cyc_Dict_insert)(*set,
place,loc);}}return 0;}struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct
Cyc_CfFlowInfo_Place*place){void*_tmp13;struct Cyc_CfFlowInfo_Place _tmp12=*place;
_tmp13=(void*)_tmp12.root;{void*_tmp14=_tmp13;struct Cyc_Absyn_Vardecl*_tmp15;
_LL1: if(*((int*)_tmp14)!= 0)goto _LL3;_tmp15=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp14)->f1;_LL2: return Cyc_Absynpp_qvar2string(_tmp15->name);_LL3:;_LL4:({void*
_tmp16=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp17="error locations not for VarRoots";_tag_dyneither(_tmp17,
sizeof(char),33);}),_tag_dyneither(_tmp16,sizeof(void*),0));});_LL0:;}}static
void Cyc_CfFlowInfo_unique_err(struct Cyc_CfFlowInfo_Place*place,struct
_dyneither_ptr err_msg1,struct _dyneither_ptr err_msg2,struct Cyc_Position_Segment*
consumed_loc,struct Cyc_Position_Segment*loc){struct _dyneither_ptr _tmp18=Cyc_CfFlowInfo_place_err_string(
place);if(!Cyc_Position_segment_equals(consumed_loc,loc)){struct _dyneither_ptr
_tmp19=Cyc_Position_string_of_segment(consumed_loc);({struct Cyc_String_pa_struct
_tmp1C;_tmp1C.tag=0;_tmp1C.f1=(struct _dyneither_ptr)_tmp19;{struct Cyc_String_pa_struct
_tmp1B;_tmp1B.tag=0;_tmp1B.f1=(struct _dyneither_ptr)_tmp18;{void*_tmp1A[2]={&
_tmp1B,& _tmp1C};Cyc_Tcutil_terr(loc,err_msg1,_tag_dyneither(_tmp1A,sizeof(void*),
2));}}});}else{({struct Cyc_String_pa_struct _tmp1E;_tmp1E.tag=0;_tmp1E.f1=(struct
_dyneither_ptr)_tmp18;{void*_tmp1D[1]={& _tmp1E};Cyc_Tcutil_terr(loc,err_msg2,
_tag_dyneither(_tmp1D,sizeof(void*),1));}});}}static struct Cyc_Position_Segment*
Cyc_CfFlowInfo_combine_consume_f(int isErr,struct Cyc_CfFlowInfo_Place*place,
struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*loc2){if(isErr)Cyc_CfFlowInfo_unique_err(
place,({const char*_tmp1F="May consume non-aliased pointer %s more than once (cf. %s)";
_tag_dyneither(_tmp1F,sizeof(char),59);}),({const char*_tmp20="May consume non-aliased pointer %s more than once";
_tag_dyneither(_tmp20,sizeof(char),50);}),loc1,loc2);return loc1;}struct Cyc_Dict_Dict
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int
disjoint){struct Cyc_Dict_Dict _tmp21=((struct Cyc_Dict_Dict(*)(struct Cyc_Position_Segment*(*
f)(int,struct Cyc_CfFlowInfo_Place*,struct Cyc_Position_Segment*,struct Cyc_Position_Segment*),
int env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_combine_consume_f,
disjoint,s1,s2);return _tmp21;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*
p);struct _tuple6{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*f2;};
int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
if((int)s1.t == (int)s2.t)return 1;{struct _handler_cons _tmp22;_push_handler(&
_tmp22);{int _tmp24=0;if(setjmp(_tmp22.handler))_tmp24=1;if(!_tmp24){{struct
_RegionHandle _tmp25=_new_region("temp");struct _RegionHandle*temp=& _tmp25;
_push_region(temp);{struct Cyc_Iter_Iter _tmp26=((struct Cyc_Iter_Iter(*)(struct
_RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(temp,s1);struct
_tuple6 _tmp27=*((struct _tuple6*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict d))
Cyc_Dict_rchoose)(temp,s1);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple6*))
Cyc_Iter_next)(_tmp26,& _tmp27)){struct Cyc_CfFlowInfo_Place*_tmp28=_tmp27.f1;if(!((
int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(s2,
_tmp28)){int _tmp29=0;_npop_handler(1);return _tmp29;}}{int _tmp2A=1;_npop_handler(
1);return _tmp2A;}};_pop_region(temp);};_pop_handler();}else{void*_tmp23=(void*)
_exn_thrown;void*_tmp2C=_tmp23;_LL6:;_LL7: return 1;_LL8:;_LL9:(void)_throw(_tmp2C);
_LL5:;}}}}int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict
s2){return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(
s2,s1);}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)r1 == (int)r2)return 0;{
struct _tuple0 _tmp2E=({struct _tuple0 _tmp2D;_tmp2D.f1=r1;_tmp2D.f2=r2;_tmp2D;});
void*_tmp2F;struct Cyc_Absyn_Vardecl*_tmp30;void*_tmp31;struct Cyc_Absyn_Vardecl*
_tmp32;void*_tmp33;void*_tmp34;void*_tmp35;struct Cyc_Absyn_Exp*_tmp36;void*
_tmp37;struct Cyc_Absyn_Exp*_tmp38;void*_tmp39;void*_tmp3A;void*_tmp3B;int _tmp3C;
void*_tmp3D;int _tmp3E;_LLB: _tmp2F=_tmp2E.f1;if(*((int*)_tmp2F)!= 0)goto _LLD;
_tmp30=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp2F)->f1;_tmp31=_tmp2E.f2;if(*((
int*)_tmp31)!= 0)goto _LLD;_tmp32=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp31)->f1;
_LLC: return(int)_tmp30 - (int)_tmp32;_LLD: _tmp33=_tmp2E.f1;if(*((int*)_tmp33)!= 0)
goto _LLF;_LLE: return - 1;_LLF: _tmp34=_tmp2E.f2;if(*((int*)_tmp34)!= 0)goto _LL11;
_LL10: return 1;_LL11: _tmp35=_tmp2E.f1;if(*((int*)_tmp35)!= 1)goto _LL13;_tmp36=((
struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp35)->f1;_tmp37=_tmp2E.f2;if(*((int*)
_tmp37)!= 1)goto _LL13;_tmp38=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp37)->f1;
_LL12: return(int)_tmp36 - (int)_tmp38;_LL13: _tmp39=_tmp2E.f1;if(*((int*)_tmp39)!= 
1)goto _LL15;_LL14: return - 1;_LL15: _tmp3A=_tmp2E.f2;if(*((int*)_tmp3A)!= 1)goto
_LL17;_LL16: return 1;_LL17: _tmp3B=_tmp2E.f1;if(*((int*)_tmp3B)!= 2)goto _LLA;
_tmp3C=((struct Cyc_CfFlowInfo_InitParam_struct*)_tmp3B)->f1;_tmp3D=_tmp2E.f2;if(*((
int*)_tmp3D)!= 2)goto _LLA;_tmp3E=((struct Cyc_CfFlowInfo_InitParam_struct*)_tmp3D)->f1;
_LL18: return _tmp3C - _tmp3E;_LLA:;}}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*
p1,struct Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)p2)return 0;{void*_tmp40;
struct Cyc_List_List*_tmp41;struct Cyc_CfFlowInfo_Place _tmp3F=*p1;_tmp40=(void*)
_tmp3F.root;_tmp41=_tmp3F.fields;{void*_tmp43;struct Cyc_List_List*_tmp44;struct
Cyc_CfFlowInfo_Place _tmp42=*p2;_tmp43=(void*)_tmp42.root;_tmp44=_tmp42.fields;{
int i=Cyc_CfFlowInfo_root_cmp(_tmp40,_tmp43);if(i != 0)return i;return((int(*)(int(*
cmp)(int,int),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Core_intcmp,_tmp41,_tmp44);}}}}static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(
struct Cyc_CfFlowInfo_Place*p){struct Cyc_List_List*sl=0;void*_tmp46;struct Cyc_List_List*
_tmp47;struct Cyc_CfFlowInfo_Place _tmp45=*p;_tmp46=(void*)_tmp45.root;_tmp47=
_tmp45.fields;{void*_tmp48=_tmp46;struct Cyc_Absyn_Vardecl*_tmp49;struct Cyc_Absyn_Exp*
_tmp4A;int _tmp4B;_LL1A: if(*((int*)_tmp48)!= 0)goto _LL1C;_tmp49=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp48)->f1;_LL1B: sl=({struct Cyc_List_List*_tmp4C=_cycalloc(sizeof(*_tmp4C));
_tmp4C->hd=({struct _dyneither_ptr*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D[0]=({
struct Cyc_String_pa_struct _tmp50;_tmp50.tag=0;_tmp50.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp49->name).f2);{void*_tmp4E[1]={& _tmp50};Cyc_aprintf(({
const char*_tmp4F="%s";_tag_dyneither(_tmp4F,sizeof(char),3);}),_tag_dyneither(
_tmp4E,sizeof(void*),1));}});_tmp4D;});_tmp4C->tl=sl;_tmp4C;});goto _LL19;_LL1C:
if(*((int*)_tmp48)!= 1)goto _LL1E;_tmp4A=((struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp48)->f1;_LL1D: sl=({struct Cyc_List_List*_tmp51=_cycalloc(sizeof(*_tmp51));
_tmp51->hd=({struct _dyneither_ptr*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52[0]=({
struct Cyc_Int_pa_struct _tmp55;_tmp55.tag=1;_tmp55.f1=(unsigned long)((int)_tmp4A);{
void*_tmp53[1]={& _tmp55};Cyc_aprintf(({const char*_tmp54="mpt%d";_tag_dyneither(
_tmp54,sizeof(char),6);}),_tag_dyneither(_tmp53,sizeof(void*),1));}});_tmp52;});
_tmp51->tl=sl;_tmp51;});goto _LL19;_LL1E: if(*((int*)_tmp48)!= 2)goto _LL19;_tmp4B=((
struct Cyc_CfFlowInfo_InitParam_struct*)_tmp48)->f1;_LL1F: sl=({struct Cyc_List_List*
_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->hd=({struct _dyneither_ptr*_tmp57=
_cycalloc(sizeof(*_tmp57));_tmp57[0]=({struct Cyc_Int_pa_struct _tmp5A;_tmp5A.tag=
1;_tmp5A.f1=(unsigned long)_tmp4B;{void*_tmp58[1]={& _tmp5A};Cyc_aprintf(({const
char*_tmp59="param%d";_tag_dyneither(_tmp59,sizeof(char),8);}),_tag_dyneither(
_tmp58,sizeof(void*),1));}});_tmp57;});_tmp56->tl=sl;_tmp56;});goto _LL19;_LL19:;}
for(0;_tmp47 != 0;_tmp47=_tmp47->tl){sl=({struct Cyc_List_List*_tmp5B=_cycalloc(
sizeof(*_tmp5B));_tmp5B->hd=({struct _dyneither_ptr*_tmp5C=_cycalloc(sizeof(*
_tmp5C));_tmp5C[0]=({struct Cyc_Int_pa_struct _tmp5F;_tmp5F.tag=1;_tmp5F.f1=(
unsigned long)((int)_tmp47->hd);{void*_tmp5D[1]={& _tmp5F};Cyc_aprintf(({const
char*_tmp5E="%d";_tag_dyneither(_tmp5E,sizeof(char),3);}),_tag_dyneither(_tmp5D,
sizeof(void*),1));}});_tmp5C;});_tmp5B->tl=sl;_tmp5B;});}{struct _dyneither_ptr*
_tmp60=({struct _dyneither_ptr*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65[0]=({
struct Cyc_String_pa_struct _tmp68;_tmp68.tag=0;_tmp68.f1=(struct _dyneither_ptr)({
const char*_tmp69="";_tag_dyneither(_tmp69,sizeof(char),1);});{void*_tmp66[1]={&
_tmp68};Cyc_aprintf(({const char*_tmp67="%s";_tag_dyneither(_tmp67,sizeof(char),3);}),
_tag_dyneither(_tmp66,sizeof(void*),1));}});_tmp65;});for(0;sl != 0;sl=sl->tl){*
_tmp60=({struct Cyc_String_pa_struct _tmp64;_tmp64.tag=0;_tmp64.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp60);{struct Cyc_String_pa_struct _tmp63;
_tmp63.tag=0;_tmp63.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)sl->hd));{void*_tmp61[2]={& _tmp63,& _tmp64};Cyc_aprintf(({const
char*_tmp62="%s.%s";_tag_dyneither(_tmp62,sizeof(char),6);}),_tag_dyneither(
_tmp61,sizeof(void*),2));}}});}return _tmp60;}}static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,void*t,void*leafval);struct
_dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_List_List*fs,void*leafval){unsigned int sz=(unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(fs);struct _dyneither_ptr d=({unsigned int
_tmp6E=sz;void**_tmp6F=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),
_tmp6E));struct _dyneither_ptr _tmp71=_tag_dyneither(_tmp6F,sizeof(void*),_tmp6E);{
unsigned int _tmp70=_tmp6E;unsigned int i;for(i=0;i < _tmp70;i ++){_tmp6F[i]=(void*)
fenv->unknown_all;}}_tmp71;});{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){struct Cyc_Absyn_Aggrfield
_tmp6B;struct _dyneither_ptr*_tmp6C;void*_tmp6D;struct Cyc_Absyn_Aggrfield*_tmp6A=(
struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;_tmp6B=*
_tmp6A;_tmp6C=_tmp6B.name;_tmp6D=(void*)_tmp6B.type;if(_get_dyneither_size(*
_tmp6C,sizeof(char))!= 1)*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=
Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,_tmp6D,leafval);}}return d;}struct _tuple7{
struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,void*t,void*leafval){if(Cyc_Absyn_is_union_type(
t))return(void*)fenv->unknown_all;{void*_tmp72=Cyc_Tcutil_compress(t);struct Cyc_Absyn_TunionFieldInfo
_tmp73;union Cyc_Absyn_TunionFieldInfoU_union _tmp74;struct Cyc_Absyn_Tunionfield*
_tmp75;struct Cyc_List_List*_tmp76;struct Cyc_Absyn_AggrInfo _tmp77;union Cyc_Absyn_AggrInfoU_union
_tmp78;void*_tmp79;struct Cyc_List_List*_tmp7A;struct Cyc_Absyn_ArrayInfo _tmp7B;
void*_tmp7C;struct Cyc_Absyn_Conref*_tmp7D;void*_tmp7E;struct Cyc_Absyn_PtrInfo
_tmp7F;struct Cyc_Absyn_PtrAtts _tmp80;struct Cyc_Absyn_Conref*_tmp81;_LL21: if(
_tmp72 <= (void*)4)goto _LL2F;if(*((int*)_tmp72)!= 3)goto _LL23;_tmp73=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp72)->f1;_tmp74=_tmp73.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp72)->f1).field_info).KnownTunionfield).tag != 1)goto _LL23;_tmp75=(_tmp74.KnownTunionfield).f2;
_LL22: if(_tmp75->typs == 0)return leafval;_tmp76=_tmp75->typs;goto _LL24;_LL23: if(*((
int*)_tmp72)!= 9)goto _LL25;_tmp76=((struct Cyc_Absyn_TupleType_struct*)_tmp72)->f1;
_LL24: {unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp76);struct _dyneither_ptr d=({unsigned int _tmp84=sz;void**_tmp85=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp84));struct _dyneither_ptr
_tmp87=_tag_dyneither(_tmp85,sizeof(void*),_tmp84);{unsigned int _tmp86=_tmp84;
unsigned int i;for(i=0;i < _tmp86;i ++){_tmp85[i]=(void*)fenv->unknown_all;}}_tmp87;});{
int i=0;for(0;i < sz;++ i){*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=
Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,(*((struct _tuple7*)((struct Cyc_List_List*)
_check_null(_tmp76))->hd)).f2,leafval);_tmp76=_tmp76->tl;}}return(void*)({struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp82=_region_malloc(fenv->r,sizeof(*_tmp82));
_tmp82[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp83;_tmp83.tag=4;_tmp83.f1=d;
_tmp83;});_tmp82;});}_LL25: if(*((int*)_tmp72)!= 10)goto _LL27;_tmp77=((struct Cyc_Absyn_AggrType_struct*)
_tmp72)->f1;_tmp78=_tmp77.aggr_info;_LL26: {struct Cyc_Absyn_Aggrdecl*_tmp88=Cyc_Absyn_get_known_aggrdecl(
_tmp78);if(_tmp88->impl == 0)goto _LL20;_tmp7A=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp88->impl))->fields;goto _LL28;}_LL27: if(*((int*)_tmp72)!= 11)goto
_LL29;_tmp79=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp72)->f1;if((int)
_tmp79 != 0)goto _LL29;_tmp7A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp72)->f2;
_LL28: return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp89=_region_malloc(
fenv->r,sizeof(*_tmp89));_tmp89[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp8A;
_tmp8A.tag=4;_tmp8A.f1=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmp7A,leafval);
_tmp8A;});_tmp89;});_LL29: if(*((int*)_tmp72)!= 7)goto _LL2B;_tmp7B=((struct Cyc_Absyn_ArrayType_struct*)
_tmp72)->f1;_tmp7C=(void*)_tmp7B.elt_type;_tmp7D=_tmp7B.zero_term;if(!((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp7D))goto _LL2B;_LL2A:
return allow_zeroterm  && Cyc_Tcutil_bits_only(_tmp7C)?(void*)fenv->unknown_all:
leafval;_LL2B: if(*((int*)_tmp72)!= 18)goto _LL2D;_tmp7E=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp72)->f1;_LL2C: {void*_tmp8B=leafval;void*_tmp8C;void*_tmp8D;_LL32: if(_tmp8B
<= (void*)3)goto _LL36;if(*((int*)_tmp8B)!= 0)goto _LL34;_tmp8C=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp8B)->f1;if((int)_tmp8C != 2)goto _LL34;_LL33: goto _LL35;_LL34: if(*((int*)_tmp8B)
!= 1)goto _LL36;_tmp8D=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp8B)->f1;if((
int)_tmp8D != 2)goto _LL36;_LL35: goto _LL37;_LL36: if((int)_tmp8B != 0)goto _LL38;
_LL37: goto _LL39;_LL38: if((int)_tmp8B != 1)goto _LL3A;_LL39: return(void*)({struct
Cyc_CfFlowInfo_TagCmps_struct*_tmp8E=_region_malloc(fenv->r,sizeof(*_tmp8E));
_tmp8E[0]=({struct Cyc_CfFlowInfo_TagCmps_struct _tmp8F;_tmp8F.tag=3;_tmp8F.f1=({
struct Cyc_CfFlowInfo_TagCmp*_tmp90[1];_tmp90[0]=({struct Cyc_CfFlowInfo_TagCmp*
_tmp91=_region_malloc(fenv->r,sizeof(*_tmp91));_tmp91->cmp=(void*)((void*)5);
_tmp91->bd=(void*)_tmp7E;_tmp91;});((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(fenv->r,_tag_dyneither(_tmp90,sizeof(
struct Cyc_CfFlowInfo_TagCmp*),1));});_tmp8F;});_tmp8E;});_LL3A:;_LL3B: return
leafval;_LL31:;}_LL2D: if(*((int*)_tmp72)!= 4)goto _LL2F;_tmp7F=((struct Cyc_Absyn_PointerType_struct*)
_tmp72)->f1;_tmp80=_tmp7F.ptr_atts;_tmp81=_tmp80.nullable;if(!(!((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp81)))goto _LL2F;_LL2E:{void*
_tmp92=leafval;void*_tmp93;void*_tmp94;_LL3D: if(_tmp92 <= (void*)3)goto _LL41;if(*((
int*)_tmp92)!= 0)goto _LL3F;_tmp93=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp92)->f1;if((int)_tmp93 != 1)goto _LL3F;_LL3E: return(void*)2;_LL3F: if(*((int*)
_tmp92)!= 0)goto _LL41;_tmp94=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp92)->f1;if((int)_tmp94 != 2)goto _LL41;_LL40: return(void*)1;_LL41:;_LL42: goto
_LL3C;_LL3C:;}goto _LL20;_LL2F:;_LL30: goto _LL20;_LL20:;}return Cyc_Tcutil_bits_only(
t)?(void*)fenv->unknown_all: leafval;}void*Cyc_CfFlowInfo_typ_to_absrval(struct
Cyc_CfFlowInfo_FlowEnv*fenv,void*t,void*leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,1,t,leafval);}static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Dict_Dict set){struct _RegionHandle _tmp95=_new_region("r");struct
_RegionHandle*r=& _tmp95;_push_region(r);{struct _tuple6 _tmp96=({struct _tuple6
_tmpA1;_tmpA1.f1=place;_tmpA1.f2=0;_tmpA1;});struct Cyc_Iter_Iter _tmp97=((struct
Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
r,set);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple6*))Cyc_Iter_next)(_tmp97,&
_tmp96)){struct Cyc_CfFlowInfo_Place*_tmp98=_tmp96.f1;void*_tmp9A;struct Cyc_List_List*
_tmp9B;struct Cyc_CfFlowInfo_Place _tmp99=*place;_tmp9A=(void*)_tmp99.root;_tmp9B=
_tmp99.fields;{void*_tmp9D;struct Cyc_List_List*_tmp9E;struct Cyc_CfFlowInfo_Place
_tmp9C=*_tmp98;_tmp9D=(void*)_tmp9C.root;_tmp9E=_tmp9C.fields;if(Cyc_CfFlowInfo_root_cmp(
_tmp9A,_tmp9D)!= 0)continue;for(0;_tmp9B != 0  && _tmp9E != 0;(_tmp9B=_tmp9B->tl,
_tmp9E=_tmp9E->tl)){if((int)_tmp9B->hd != (int)_tmp9E->hd)break;}if(_tmp9B == 0){
int _tmp9F=1;_npop_handler(0);return _tmp9F;}}}{int _tmpA0=0;_npop_handler(0);
return _tmpA0;}};_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*
rgn;struct Cyc_List_List*places;};static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*
pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place))pile->places=({struct
Cyc_List_List*_tmpA2=_region_malloc(pile->rgn,sizeof(*_tmpA2));_tmpA2->hd=place;
_tmpA2->tl=pile->places;_tmpA2;});}static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*
pile,void*r){void*_tmpA3=r;struct Cyc_CfFlowInfo_Place*_tmpA4;struct
_dyneither_ptr _tmpA5;_LL44: if(_tmpA3 <= (void*)3)goto _LL48;if(*((int*)_tmpA3)!= 2)
goto _LL46;_tmpA4=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpA3)->f1;_LL45: Cyc_CfFlowInfo_add_place(
pile,_tmpA4);return;_LL46: if(*((int*)_tmpA3)!= 4)goto _LL48;_tmpA5=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpA3)->f1;_LL47:{int i=0;for(0;i < _get_dyneither_size(_tmpA5,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,*((void**)_check_dyneither_subscript(_tmpA5,
sizeof(void*),i)));}}return;_LL48:;_LL49: return;_LL43:;}static void*Cyc_CfFlowInfo_insert_place_inner(
struct _RegionHandle*r,void*new_val,void*old_val){void*_tmpA6=old_val;struct
_dyneither_ptr _tmpA7;_LL4B: if(_tmpA6 <= (void*)3)goto _LL4D;if(*((int*)_tmpA6)!= 4)
goto _LL4D;_tmpA7=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpA6)->f1;_LL4C: {
struct _dyneither_ptr d2=({unsigned int _tmpAA=_get_dyneither_size(_tmpA7,sizeof(
void*));void**_tmpAB=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmpAA));
struct _dyneither_ptr _tmpAD=_tag_dyneither(_tmpAB,sizeof(void*),_tmpAA);{
unsigned int _tmpAC=_tmpAA;unsigned int i;for(i=0;i < _tmpAC;i ++){_tmpAB[i]=Cyc_CfFlowInfo_insert_place_inner(
r,new_val,*((void**)_check_dyneither_subscript(_tmpA7,sizeof(void*),(int)i)));}}
_tmpAD;});return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmpA8=
_region_malloc(r,sizeof(*_tmpA8));_tmpA8[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpA9;_tmpA9.tag=4;_tmpA9.f1=d2;_tmpA9;});_tmpA8;});}_LL4D:;_LL4E: return new_val;
_LL4A:;}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){void*_tmpAE=*((void**)
_check_dyneither_subscript(d,sizeof(void*),n));if(_tmpAE == rval)return d;{struct
_dyneither_ptr res=({unsigned int _tmpAF=_get_dyneither_size(d,sizeof(void*));void**
_tmpB0=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmpAF));struct
_dyneither_ptr _tmpB2=_tag_dyneither(_tmpB0,sizeof(void*),_tmpAF);{unsigned int
_tmpB1=_tmpAF;unsigned int i;for(i=0;i < _tmpB1;i ++){_tmpB0[i]=*((void**)
_check_dyneither_subscript(d,sizeof(void*),(int)i));}}_tmpB2;});*((void**)
_check_dyneither_subscript(res,sizeof(void*),n))=rval;return res;}}struct _tuple8{
struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 
0)return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{struct _tuple8
_tmpB4=({struct _tuple8 _tmpB3;_tmpB3.f1=fs;_tmpB3.f2=old_val;_tmpB3;});struct Cyc_List_List*
_tmpB5;struct Cyc_List_List _tmpB6;int _tmpB7;struct Cyc_List_List*_tmpB8;void*
_tmpB9;struct _dyneither_ptr _tmpBA;_LL50: _tmpB5=_tmpB4.f1;if(_tmpB5 == 0)goto _LL52;
_tmpB6=*_tmpB5;_tmpB7=(int)_tmpB6.hd;_tmpB8=_tmpB6.tl;_tmpB9=_tmpB4.f2;if(_tmpB9
<= (void*)3)goto _LL52;if(*((int*)_tmpB9)!= 4)goto _LL52;_tmpBA=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpB9)->f1;_LL51: {void*_tmpBB=Cyc_CfFlowInfo_insert_place_outer(r,_tmpB8,*((
void**)_check_dyneither_subscript(_tmpBA,sizeof(void*),_tmpB7)),new_val);struct
_dyneither_ptr _tmpBC=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmpBA,_tmpB7,_tmpBB);if(
_tmpBC.curr == _tmpBA.curr)return old_val;else{return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpBD=_region_malloc(r,sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpBE;_tmpBE.tag=4;_tmpBE.f1=_tmpBC;_tmpBE;});_tmpBD;});}}_LL52:;_LL53:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));
_tmpBF[0]=({struct Cyc_Core_Impossible_struct _tmpC0;_tmpC0.tag=Cyc_Core_Impossible;
_tmpC0.f1=({const char*_tmpC1="bad insert place";_tag_dyneither(_tmpC1,sizeof(
char),17);});_tmpC0;});_tmpBF;}));_LL4F:;}}static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*
all_changed,struct Cyc_Dict_Dict d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*
_tmpC2=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;Cyc_CfFlowInfo_update_place_set(
all_changed,_tmpC2,0);{void*oldval;void*newval;{struct _handler_cons _tmpC3;
_push_handler(& _tmpC3);{int _tmpC5=0;if(setjmp(_tmpC3.handler))_tmpC5=1;if(!
_tmpC5){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmpC2);;_pop_handler();}else{void*
_tmpC4=(void*)_exn_thrown;void*_tmpC7=_tmpC4;_LL55: if(_tmpC7 != Cyc_Dict_Absent)
goto _LL57;_LL56: continue;_LL57:;_LL58:(void)_throw(_tmpC7);_LL54:;}}}{void*
_tmpC8=Cyc_CfFlowInfo_initlevel(d,oldval);_LL5A: if((int)_tmpC8 != 2)goto _LL5C;
_LL5B: newval=(void*)fenv->esc_all;goto _LL59;_LL5C: if((int)_tmpC8 != 1)goto _LL5E;
_LL5D: newval=(void*)fenv->esc_this;goto _LL59;_LL5E: if((int)_tmpC8 != 0)goto _LL59;
_LL5F: newval=(void*)fenv->esc_none;goto _LL59;_LL59:;}Cyc_CfFlowInfo_add_places(
pile,oldval);{void*_tmpCA;struct Cyc_List_List*_tmpCB;struct Cyc_CfFlowInfo_Place
_tmpC9=*_tmpC2;_tmpCA=(void*)_tmpC9.root;_tmpCB=_tmpC9.fields;d=Cyc_Dict_insert(
d,_tmpCA,Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmpCB,Cyc_Dict_lookup(d,
_tmpCA),newval));}}}return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct
_RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};static void*Cyc_CfFlowInfo_initlevel_approx(
void*r){void*_tmpCC=r;void*_tmpCD;void*_tmpCE;_LL61: if(_tmpCC <= (void*)3)goto
_LL65;if(*((int*)_tmpCC)!= 0)goto _LL63;_tmpCD=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpCC)->f1;_LL62: return _tmpCD;_LL63: if(*((int*)_tmpCC)!= 1)goto _LL65;_tmpCE=(
void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmpCC)->f1;_LL64: return _tmpCE;_LL65:
if((int)_tmpCC != 0)goto _LL67;_LL66: goto _LL68;_LL67: if((int)_tmpCC != 1)goto _LL69;
_LL68: return(void*)2;_LL69: if((int)_tmpCC != 2)goto _LL6B;_LL6A: return(void*)1;
_LL6B: if(_tmpCC <= (void*)3)goto _LL6D;if(*((int*)_tmpCC)!= 3)goto _LL6D;_LL6C:
return(void*)2;_LL6D:;_LL6E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({struct Cyc_Core_Impossible_struct
_tmpD0;_tmpD0.tag=Cyc_Core_Impossible;_tmpD0.f1=({const char*_tmpD1="initlevel_approx";
_tag_dyneither(_tmpD1,sizeof(char),17);});_tmpD0;});_tmpCF;}));_LL60:;}static
void*Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,
void*acc){void*this_ans;if(acc == (void*)0)return(void*)0;{void*_tmpD2=r;struct
_dyneither_ptr _tmpD3;struct Cyc_CfFlowInfo_Place*_tmpD4;_LL70: if(_tmpD2 <= (void*)
3)goto _LL74;if(*((int*)_tmpD2)!= 4)goto _LL72;_tmpD3=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpD2)->f1;_LL71: {unsigned int _tmpD5=_get_dyneither_size(_tmpD3,sizeof(void*));
this_ans=(void*)2;{int i=0;for(0;i < _tmpD5;++ i){this_ans=Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmpD3,sizeof(void*),i)),this_ans);}}
goto _LL6F;}_LL72: if(*((int*)_tmpD2)!= 2)goto _LL74;_tmpD4=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpD2)->f1;_LL73: if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct
Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(
Cyc_CfFlowInfo_place_cmp,env->seen,_tmpD4))this_ans=(void*)2;else{env->seen=({
struct Cyc_List_List*_tmpD6=_region_malloc(env->e,sizeof(*_tmpD6));_tmpD6->hd=
_tmpD4;_tmpD6->tl=env->seen;_tmpD6;});this_ans=Cyc_CfFlowInfo_initlevel_rec(env,
Cyc_CfFlowInfo_lookup_place(env->d,_tmpD4),(void*)2);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == (void*)0)this_ans=(void*)1;}goto _LL6F;
_LL74:;_LL75: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL6F:;}if(this_ans == (
void*)0)return(void*)0;if(this_ans == (void*)1  || acc == (void*)1)return(void*)1;
return(void*)2;}void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle _tmpD7=_new_region("env_rgn");struct _RegionHandle*env_rgn=&
_tmpD7;_push_region(env_rgn);{struct Cyc_CfFlowInfo_InitlevelEnv _tmpD8=({struct
Cyc_CfFlowInfo_InitlevelEnv _tmpDA;_tmpDA.e=env_rgn;_tmpDA.d=d;_tmpDA.seen=0;
_tmpDA;});void*_tmpD9=Cyc_CfFlowInfo_initlevel_rec(& _tmpD8,r,(void*)2);
_npop_handler(0);return _tmpD9;};_pop_region(env_rgn);}struct _tuple9{void*f1;int
f2;};void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place){void*_tmpDC;struct Cyc_List_List*_tmpDD;struct Cyc_CfFlowInfo_Place _tmpDB=*
place;_tmpDC=(void*)_tmpDB.root;_tmpDD=_tmpDB.fields;{void*_tmpDE=Cyc_Dict_lookup(
d,_tmpDC);for(0;_tmpDD != 0;_tmpDD=_tmpDD->tl){struct _tuple9 _tmpE0=({struct
_tuple9 _tmpDF;_tmpDF.f1=_tmpDE;_tmpDF.f2=(int)_tmpDD->hd;_tmpDF;});void*_tmpE1;
struct _dyneither_ptr _tmpE2;int _tmpE3;_LL77: _tmpE1=_tmpE0.f1;if(_tmpE1 <= (void*)3)
goto _LL79;if(*((int*)_tmpE1)!= 4)goto _LL79;_tmpE2=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpE1)->f1;_tmpE3=_tmpE0.f2;_LL78: _tmpDE=*((void**)_check_dyneither_subscript(
_tmpE2,sizeof(void*),_tmpE3));goto _LL76;_LL79:;_LL7A:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=({struct
Cyc_Core_Impossible_struct _tmpE5;_tmpE5.tag=Cyc_Core_Impossible;_tmpE5.f1=({
const char*_tmpE6="bad lookup_place";_tag_dyneither(_tmpE6,sizeof(char),17);});
_tmpE5;});_tmpE4;}));_LL76:;}return _tmpDE;}}static int Cyc_CfFlowInfo_is_rval_unescaped(
void*rval){void*_tmpE7=rval;struct _dyneither_ptr _tmpE8;_LL7C: if(_tmpE7 <= (void*)
3)goto _LL80;if(*((int*)_tmpE7)!= 1)goto _LL7E;_LL7D: return 0;_LL7E: if(*((int*)
_tmpE7)!= 4)goto _LL80;_tmpE8=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpE7)->f1;
_LL7F: {unsigned int sz=_get_dyneither_size(_tmpE8,sizeof(void*));{int i=0;for(0;i
< sz;++ i){if(!Cyc_CfFlowInfo_is_rval_unescaped(*((void**)
_check_dyneither_subscript(_tmpE8,sizeof(void*),i))))return 0;}}return 1;}_LL80:;
_LL81: return 1;_LL7B:;}int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct
Cyc_CfFlowInfo_Place*place){return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(
d,place));}struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){struct
_RegionHandle _tmpE9=_new_region("rgn");struct _RegionHandle*rgn=& _tmpE9;
_push_region(rgn);{struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*
_tmpEB=_region_malloc(rgn,sizeof(*_tmpEB));_tmpEB->rgn=rgn;_tmpEB->places=0;
_tmpEB;});Cyc_CfFlowInfo_add_places(pile,r);{struct Cyc_Dict_Dict _tmpEA=Cyc_CfFlowInfo_escape_these(
fenv,pile,all_changed,d);_npop_handler(0);return _tmpEA;}};_pop_region(rgn);}
struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict d;struct Cyc_Position_Segment*loc;};static void*Cyc_CfFlowInfo_assign_place_inner(
struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){struct _tuple0 _tmpED=({
struct _tuple0 _tmpEC;_tmpEC.f1=oldval;_tmpEC.f2=newval;_tmpEC;});void*_tmpEE;void*
_tmpEF;struct Cyc_CfFlowInfo_Place*_tmpF0;void*_tmpF1;void*_tmpF2;struct
_dyneither_ptr _tmpF3;void*_tmpF4;struct _dyneither_ptr _tmpF5;void*_tmpF6;void*
_tmpF7;_LL83: _tmpEE=_tmpED.f1;if(_tmpEE <= (void*)3)goto _LL85;if(*((int*)_tmpEE)
!= 1)goto _LL85;_tmpEF=_tmpED.f2;if(_tmpEF <= (void*)3)goto _LL85;if(*((int*)_tmpEF)
!= 2)goto _LL85;_tmpF0=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpEF)->f1;_LL84:
Cyc_CfFlowInfo_add_place(env->pile,_tmpF0);goto _LL86;_LL85: _tmpF1=_tmpED.f1;if(
_tmpF1 <= (void*)3)goto _LL87;if(*((int*)_tmpF1)!= 1)goto _LL87;_LL86: if(Cyc_CfFlowInfo_initlevel(
env->d,newval)!= (void*)2)({void*_tmpF8=0;Cyc_Tcutil_terr(env->loc,({const char*
_tmpF9="assignment puts possibly-uninitialized data in an escaped location";
_tag_dyneither(_tmpF9,sizeof(char),67);}),_tag_dyneither(_tmpF8,sizeof(void*),0));});
return(void*)(env->fenv)->esc_all;_LL87: _tmpF2=_tmpED.f1;if(_tmpF2 <= (void*)3)
goto _LL89;if(*((int*)_tmpF2)!= 4)goto _LL89;_tmpF3=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpF2)->f1;_tmpF4=_tmpED.f2;if(_tmpF4 <= (void*)3)goto _LL89;if(*((int*)_tmpF4)!= 
4)goto _LL89;_tmpF5=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpF4)->f1;_LL88: {
struct _dyneither_ptr new_d=({unsigned int _tmpFC=_get_dyneither_size(_tmpF3,
sizeof(void*));void**_tmpFD=(void**)_region_malloc((env->fenv)->r,_check_times(
sizeof(void*),_tmpFC));struct _dyneither_ptr _tmpFF=_tag_dyneither(_tmpFD,sizeof(
void*),_tmpFC);{unsigned int _tmpFE=_tmpFC;unsigned int i;for(i=0;i < _tmpFE;i ++){
_tmpFD[i]=Cyc_CfFlowInfo_assign_place_inner(env,*((void**)
_check_dyneither_subscript(_tmpF3,sizeof(void*),(int)i)),*((void**)
_check_dyneither_subscript(_tmpF5,sizeof(void*),(int)i)));}}_tmpFF;});int change=
0;{int i=0;for(0;i < _get_dyneither_size(_tmpF3,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmpF3,sizeof(void*),i))){change=1;break;}}}if(!
change)return oldval;change=0;{int i=0;for(0;i < _get_dyneither_size(_tmpF3,sizeof(
void*));++ i){if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((
void**)_check_dyneither_subscript(_tmpF5,sizeof(void*),i))){change=1;break;}}}
if(!change)return newval;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpFA=_region_malloc((env->fenv)->r,sizeof(*_tmpFA));_tmpFA[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpFB;_tmpFB.tag=4;_tmpFB.f1=new_d;_tmpFB;});_tmpFA;});}_LL89: _tmpF6=_tmpED.f2;
if(_tmpF6 <= (void*)3)goto _LL8B;if(*((int*)_tmpF6)!= 1)goto _LL8B;_tmpF7=(void*)((
struct Cyc_CfFlowInfo_Esc_struct*)_tmpF6)->f1;_LL8A: {void*_tmp100=_tmpF7;_LL8E:
if((int)_tmp100 != 0)goto _LL90;_LL8F: return(void*)(env->fenv)->unknown_none;_LL90:
if((int)_tmp100 != 1)goto _LL92;_LL91: return(void*)(env->fenv)->unknown_this;_LL92:
if((int)_tmp100 != 2)goto _LL8D;_LL93: return(void*)(env->fenv)->unknown_all;_LL8D:;}
_LL8B:;_LL8C: return newval;_LL82:;}static void*Cyc_CfFlowInfo_assign_place_outer(
struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,void*oldval,void*
newval){if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple8 _tmp102=({struct _tuple8 _tmp101;_tmp101.f1=fs;_tmp101.f2=oldval;
_tmp101;});struct Cyc_List_List*_tmp103;struct Cyc_List_List _tmp104;int _tmp105;
struct Cyc_List_List*_tmp106;void*_tmp107;struct _dyneither_ptr _tmp108;_LL95:
_tmp103=_tmp102.f1;if(_tmp103 == 0)goto _LL97;_tmp104=*_tmp103;_tmp105=(int)
_tmp104.hd;_tmp106=_tmp104.tl;_tmp107=_tmp102.f2;if(_tmp107 <= (void*)3)goto _LL97;
if(*((int*)_tmp107)!= 4)goto _LL97;_tmp108=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp107)->f1;_LL96: {void*_tmp109=Cyc_CfFlowInfo_assign_place_outer(env,_tmp106,*((
void**)_check_dyneither_subscript(_tmp108,sizeof(void*),_tmp105)),newval);struct
_dyneither_ptr _tmp10A=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp108,
_tmp105,_tmp109);if(_tmp10A.curr == _tmp108.curr)return oldval;else{return(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp10B=_region_malloc((env->fenv)->r,
sizeof(*_tmp10B));_tmp10B[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp10C;
_tmp10C.tag=4;_tmp10C.f1=_tmp10A;_tmp10C;});_tmp10B;});}}_LL97:;_LL98:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp10D=_cycalloc(sizeof(*
_tmp10D));_tmp10D[0]=({struct Cyc_Core_Impossible_struct _tmp10E;_tmp10E.tag=Cyc_Core_Impossible;
_tmp10E.f1=({const char*_tmp10F="bad assign place";_tag_dyneither(_tmp10F,sizeof(
char),17);});_tmp10E;});_tmp10D;}));_LL94:;}}struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){Cyc_CfFlowInfo_update_place_set(
all_changed,place,0);{struct _RegionHandle _tmp110=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp110;_push_region(rgn);{struct Cyc_CfFlowInfo_Place _tmp112;
void*_tmp113;struct Cyc_List_List*_tmp114;struct Cyc_CfFlowInfo_Place*_tmp111=
place;_tmp112=*_tmp111;_tmp113=(void*)_tmp112.root;_tmp114=_tmp112.fields;{
struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv _tmp116;
_tmp116.fenv=fenv;_tmp116.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp117=
_region_malloc(rgn,sizeof(*_tmp117));_tmp117->rgn=rgn;_tmp117->places=0;_tmp117;});
_tmp116.d=d;_tmp116.loc=loc;_tmp116;});void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmp114,Cyc_Dict_lookup(d,_tmp113),r);struct Cyc_Dict_Dict _tmp115=Cyc_CfFlowInfo_escape_these(
fenv,env.pile,all_changed,Cyc_Dict_insert(d,_tmp113,newval));_npop_handler(0);
return _tmp115;}};_pop_region(rgn);}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*
fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict
d2;};enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One
 = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv
joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*
curr_place;struct Cyc_List_List**last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged
changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,
void*r2);static struct Cyc_List_List*Cyc_CfFlowInfo_join_tag_cmps(struct
_RegionHandle*r,struct Cyc_List_List*l1,struct Cyc_List_List*l2){if(l1 == l2)return
l1;{struct Cyc_List_List*_tmp118=0;for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp11A;void*_tmp11B;void*_tmp11C;struct Cyc_CfFlowInfo_TagCmp*_tmp119=(struct Cyc_CfFlowInfo_TagCmp*)
l2->hd;_tmp11A=*_tmp119;_tmp11B=(void*)_tmp11A.cmp;_tmp11C=(void*)_tmp11A.bd;{
int found=0;void*joined_cmp=(void*)10;{struct Cyc_List_List*_tmp11D=l1;for(0;
_tmp11D != 0;_tmp11D=_tmp11D->tl){struct Cyc_CfFlowInfo_TagCmp _tmp11F;void*_tmp120;
void*_tmp121;struct Cyc_CfFlowInfo_TagCmp*_tmp11E=(struct Cyc_CfFlowInfo_TagCmp*)
_tmp11D->hd;_tmp11F=*_tmp11E;_tmp120=(void*)_tmp11F.cmp;_tmp121=(void*)_tmp11F.bd;
if(Cyc_Tcutil_typecmp(_tmp11C,_tmp121)== 0){found=1;if(_tmp120 == _tmp11B){
joined_cmp=_tmp120;break;}}}}if(found)_tmp118=({struct Cyc_List_List*_tmp122=
_region_malloc(r,sizeof(*_tmp122));_tmp122->hd=({struct Cyc_CfFlowInfo_TagCmp*
_tmp123=_region_malloc(r,sizeof(*_tmp123));_tmp123->cmp=(void*)joined_cmp;
_tmp123->bd=(void*)_tmp11C;_tmp123;});_tmp122->tl=_tmp118;_tmp122;});}}return
_tmp118;}}static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*
env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{struct _RegionHandle*_tmp124=(
env->fenv)->r;{struct _tuple0 _tmp126=({struct _tuple0 _tmp125;_tmp125.f1=r1;_tmp125.f2=
r2;_tmp125;});void*_tmp127;struct Cyc_CfFlowInfo_Place*_tmp128;void*_tmp129;
struct Cyc_CfFlowInfo_Place*_tmp12A;void*_tmp12B;struct Cyc_CfFlowInfo_Place*
_tmp12C;void*_tmp12D;struct Cyc_CfFlowInfo_Place*_tmp12E;void*_tmp12F;void*
_tmp130;void*_tmp131;void*_tmp132;void*_tmp133;struct _dyneither_ptr _tmp134;void*
_tmp135;struct _dyneither_ptr _tmp136;void*_tmp137;struct Cyc_List_List*_tmp138;
void*_tmp139;struct Cyc_List_List*_tmp13A;void*_tmp13B;void*_tmp13C;_LL9A: _tmp127=
_tmp126.f1;if(_tmp127 <= (void*)3)goto _LL9C;if(*((int*)_tmp127)!= 2)goto _LL9C;
_tmp128=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp127)->f1;_tmp129=_tmp126.f2;
if(_tmp129 <= (void*)3)goto _LL9C;if(*((int*)_tmp129)!= 2)goto _LL9C;_tmp12A=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp129)->f1;_LL9B: if(Cyc_CfFlowInfo_place_cmp(
_tmp128,_tmp12A)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmp128);Cyc_CfFlowInfo_add_place(
env->pile,_tmp12A);goto _LL99;_LL9C: _tmp12B=_tmp126.f1;if(_tmp12B <= (void*)3)goto
_LL9E;if(*((int*)_tmp12B)!= 2)goto _LL9E;_tmp12C=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp12B)->f1;_LL9D: Cyc_CfFlowInfo_add_place(env->pile,_tmp12C);goto _LL99;_LL9E:
_tmp12D=_tmp126.f2;if(_tmp12D <= (void*)3)goto _LLA0;if(*((int*)_tmp12D)!= 2)goto
_LLA0;_tmp12E=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp12D)->f1;_LL9F: Cyc_CfFlowInfo_add_place(
env->pile,_tmp12E);goto _LL99;_LLA0: _tmp12F=_tmp126.f1;if((int)_tmp12F != 1)goto
_LLA2;_tmp130=_tmp126.f2;if((int)_tmp130 != 2)goto _LLA2;_LLA1: goto _LLA3;_LLA2:
_tmp131=_tmp126.f1;if((int)_tmp131 != 2)goto _LLA4;_tmp132=_tmp126.f2;if((int)
_tmp132 != 1)goto _LLA4;_LLA3: return(void*)2;_LLA4: _tmp133=_tmp126.f1;if(_tmp133 <= (
void*)3)goto _LLA6;if(*((int*)_tmp133)!= 4)goto _LLA6;_tmp134=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp133)->f1;_tmp135=_tmp126.f2;if(_tmp135 <= (void*)3)goto _LLA6;if(*((int*)
_tmp135)!= 4)goto _LLA6;_tmp136=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp135)->f1;
_LLA5: {struct _dyneither_ptr new_d=({unsigned int _tmp13F=_get_dyneither_size(
_tmp134,sizeof(void*));void**_tmp140=(void**)_region_malloc(_tmp124,_check_times(
sizeof(void*),_tmp13F));struct _dyneither_ptr _tmp142=_tag_dyneither(_tmp140,
sizeof(void*),_tmp13F);{unsigned int _tmp141=_tmp13F;unsigned int i;for(i=0;i < 
_tmp141;i ++){_tmp140[i]=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,
void*r2))Cyc_CfFlowInfo_join_absRval)(env,0,*((void**)_check_dyneither_subscript(
_tmp134,sizeof(void*),(int)i)),*((void**)_check_dyneither_subscript(_tmp136,
sizeof(void*),(int)i)));}}_tmp142;});int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp134,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp134,sizeof(void*),i))){change=1;break;}}}if(!
change)return r1;change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp134,sizeof(
void*));++ i){if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((
void**)_check_dyneither_subscript(_tmp136,sizeof(void*),i))){change=1;break;}}}
if(!change)return r2;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp13D=
_region_malloc(_tmp124,sizeof(*_tmp13D));_tmp13D[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp13E;_tmp13E.tag=4;_tmp13E.f1=new_d;_tmp13E;});_tmp13D;});}_LLA6: _tmp137=
_tmp126.f1;if(_tmp137 <= (void*)3)goto _LLA8;if(*((int*)_tmp137)!= 3)goto _LLA8;
_tmp138=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp137)->f1;_tmp139=_tmp126.f2;
if(_tmp139 <= (void*)3)goto _LLA8;if(*((int*)_tmp139)!= 3)goto _LLA8;_tmp13A=((
struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp139)->f1;_LLA7: {struct Cyc_List_List*
_tmp143=Cyc_CfFlowInfo_join_tag_cmps(_tmp124,_tmp138,_tmp13A);if(_tmp143 == 
_tmp138)return r1;return(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*_tmp144=
_region_malloc(_tmp124,sizeof(*_tmp144));_tmp144[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmp145;_tmp145.tag=3;_tmp145.f1=_tmp143;_tmp145;});_tmp144;});}_LLA8: _tmp13B=
_tmp126.f1;if(_tmp13B <= (void*)3)goto _LLAA;if(*((int*)_tmp13B)!= 3)goto _LLAA;
_LLA9: return r2;_LLAA: _tmp13C=_tmp126.f2;if(_tmp13C <= (void*)3)goto _LLAC;if(*((
int*)_tmp13C)!= 3)goto _LLAC;_LLAB: return r1;_LLAC:;_LLAD: goto _LL99;_LL99:;}{void*
il1=Cyc_CfFlowInfo_initlevel(env->d1,r1);void*il2=Cyc_CfFlowInfo_initlevel(env->d2,
r2);struct _tuple0 _tmp147=({struct _tuple0 _tmp146;_tmp146.f1=r1;_tmp146.f2=r2;
_tmp146;});void*_tmp148;void*_tmp149;_LLAF: _tmp148=_tmp147.f1;if(_tmp148 <= (void*)
3)goto _LLB1;if(*((int*)_tmp148)!= 1)goto _LLB1;_LLB0: goto _LLB2;_LLB1: _tmp149=
_tmp147.f2;if(_tmp149 <= (void*)3)goto _LLB3;if(*((int*)_tmp149)!= 1)goto _LLB3;
_LLB2: {struct _tuple0 _tmp14B=({struct _tuple0 _tmp14A;_tmp14A.f1=il1;_tmp14A.f2=
il2;_tmp14A;});void*_tmp14C;void*_tmp14D;void*_tmp14E;void*_tmp14F;_LLB6: _tmp14C=
_tmp14B.f2;if((int)_tmp14C != 0)goto _LLB8;_LLB7: goto _LLB9;_LLB8: _tmp14D=_tmp14B.f1;
if((int)_tmp14D != 0)goto _LLBA;_LLB9: return(void*)(env->fenv)->esc_none;_LLBA:
_tmp14E=_tmp14B.f2;if((int)_tmp14E != 1)goto _LLBC;_LLBB: goto _LLBD;_LLBC: _tmp14F=
_tmp14B.f1;if((int)_tmp14F != 1)goto _LLBE;_LLBD: return(void*)(env->fenv)->esc_this;
_LLBE:;_LLBF: return(void*)(env->fenv)->esc_all;_LLB5:;}_LLB3:;_LLB4: {struct
_tuple0 _tmp151=({struct _tuple0 _tmp150;_tmp150.f1=il1;_tmp150.f2=il2;_tmp150;});
void*_tmp152;void*_tmp153;void*_tmp154;void*_tmp155;_LLC1: _tmp152=_tmp151.f2;if((
int)_tmp152 != 0)goto _LLC3;_LLC2: goto _LLC4;_LLC3: _tmp153=_tmp151.f1;if((int)
_tmp153 != 0)goto _LLC5;_LLC4: return(void*)(env->fenv)->unknown_none;_LLC5: _tmp154=
_tmp151.f2;if((int)_tmp154 != 1)goto _LLC7;_LLC6: goto _LLC8;_LLC7: _tmp155=_tmp151.f1;
if((int)_tmp155 != 1)goto _LLC9;_LLC8: return(void*)(env->fenv)->unknown_this;_LLC9:;
_LLCA: return(void*)(env->fenv)->unknown_all;_LLC0:;}_LLAE:;}}}struct _tuple10{
union Cyc_CfFlowInfo_RelnOp_union f1;union Cyc_CfFlowInfo_RelnOp_union f2;};static
int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp_union r1,union Cyc_CfFlowInfo_RelnOp_union
r2){struct _tuple10 _tmp157=({struct _tuple10 _tmp156;_tmp156.f1=r1;_tmp156.f2=r2;
_tmp156;});union Cyc_CfFlowInfo_RelnOp_union _tmp158;unsigned int _tmp159;union Cyc_CfFlowInfo_RelnOp_union
_tmp15A;unsigned int _tmp15B;union Cyc_CfFlowInfo_RelnOp_union _tmp15C;struct Cyc_Absyn_Vardecl*
_tmp15D;union Cyc_CfFlowInfo_RelnOp_union _tmp15E;struct Cyc_Absyn_Vardecl*_tmp15F;
union Cyc_CfFlowInfo_RelnOp_union _tmp160;struct Cyc_Absyn_Vardecl*_tmp161;union Cyc_CfFlowInfo_RelnOp_union
_tmp162;struct Cyc_Absyn_Vardecl*_tmp163;union Cyc_CfFlowInfo_RelnOp_union _tmp164;
unsigned int _tmp165;union Cyc_CfFlowInfo_RelnOp_union _tmp166;unsigned int _tmp167;
union Cyc_CfFlowInfo_RelnOp_union _tmp168;struct Cyc_Absyn_Vardecl*_tmp169;union Cyc_CfFlowInfo_RelnOp_union
_tmp16A;struct Cyc_Absyn_Vardecl*_tmp16B;_LLCC: _tmp158=_tmp157.f1;if(((_tmp157.f1).EqualConst).tag
!= 0)goto _LLCE;_tmp159=(_tmp158.EqualConst).f1;_tmp15A=_tmp157.f2;if(((_tmp157.f2).EqualConst).tag
!= 0)goto _LLCE;_tmp15B=(_tmp15A.EqualConst).f1;_LLCD: return _tmp159 == _tmp15B;
_LLCE: _tmp15C=_tmp157.f1;if(((_tmp157.f1).LessVar).tag != 1)goto _LLD0;_tmp15D=(
_tmp15C.LessVar).f1;_tmp15E=_tmp157.f2;if(((_tmp157.f2).LessVar).tag != 1)goto
_LLD0;_tmp15F=(_tmp15E.LessVar).f1;_LLCF: return _tmp15D == _tmp15F;_LLD0: _tmp160=
_tmp157.f1;if(((_tmp157.f1).LessNumelts).tag != 2)goto _LLD2;_tmp161=(_tmp160.LessNumelts).f1;
_tmp162=_tmp157.f2;if(((_tmp157.f2).LessNumelts).tag != 2)goto _LLD2;_tmp163=(
_tmp162.LessNumelts).f1;_LLD1: return _tmp161 == _tmp163;_LLD2: _tmp164=_tmp157.f1;
if(((_tmp157.f1).LessConst).tag != 3)goto _LLD4;_tmp165=(_tmp164.LessConst).f1;
_tmp166=_tmp157.f2;if(((_tmp157.f2).LessConst).tag != 3)goto _LLD4;_tmp167=(
_tmp166.LessConst).f1;_LLD3: return _tmp165 == _tmp167;_LLD4: _tmp168=_tmp157.f1;if(((
_tmp157.f1).LessEqNumelts).tag != 4)goto _LLD6;_tmp169=(_tmp168.LessEqNumelts).f1;
_tmp16A=_tmp157.f2;if(((_tmp157.f2).LessEqNumelts).tag != 4)goto _LLD6;_tmp16B=(
_tmp16A.LessEqNumelts).f1;_LLD5: return _tmp169 == _tmp16B;_LLD6:;_LLD7: return 0;
_LLCB:;}static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct _RegionHandle*
r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){if(r1s == r2s)return r1s;{
struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*_tmp16C=r1s;for(0;
_tmp16C != 0;_tmp16C=_tmp16C->tl){struct Cyc_CfFlowInfo_Reln*_tmp16D=(struct Cyc_CfFlowInfo_Reln*)
_tmp16C->hd;int found=0;{struct Cyc_List_List*_tmp16E=r2s;for(0;_tmp16E != 0;
_tmp16E=_tmp16E->tl){struct Cyc_CfFlowInfo_Reln*_tmp16F=(struct Cyc_CfFlowInfo_Reln*)
_tmp16E->hd;if(_tmp16D == _tmp16F  || _tmp16D->vd == _tmp16F->vd  && Cyc_CfFlowInfo_same_relop(
_tmp16D->rop,_tmp16F->rop)){res=({struct Cyc_List_List*_tmp170=_region_malloc(r,
sizeof(*_tmp170));_tmp170->hd=_tmp16D;_tmp170->tl=res;_tmp170;});found=1;break;}}}
if(!found)diff=1;}}if(!diff)res=r1s;return res;}}static void Cyc_CfFlowInfo_remove_f(
struct Cyc_List_List**l,struct Cyc_CfFlowInfo_Place*place){struct _handler_cons
_tmp171;_push_handler(& _tmp171);{int _tmp173=0;if(setjmp(_tmp171.handler))_tmp173=
1;if(!_tmp173){*l=((struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_delete_cmp)(Cyc_CfFlowInfo_place_cmp,*l,place);;_pop_handler();}else{
void*_tmp172=(void*)_exn_thrown;void*_tmp175=_tmp172;_LLD9: if(_tmp175 != Cyc_Core_Not_found)
goto _LLDB;_LLDA: goto _LLD8;_LLDB:;_LLDC:(void)_throw(_tmp175);_LLD8:;}}}void Cyc_CfFlowInfo_print_place_list(
struct Cyc_List_List*x){int _tmp176=1;while(x != 0){if(!_tmp176){({void*_tmp177=0;
Cyc_fprintf(Cyc_stderr,({const char*_tmp178=", ";_tag_dyneither(_tmp178,sizeof(
char),3);}),_tag_dyneither(_tmp177,sizeof(void*),0));});_tmp176=0;}Cyc_CfFlowInfo_print_place((
struct Cyc_CfFlowInfo_Place*)x->hd);x=x->tl;}({void*_tmp179=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp17A="\n";_tag_dyneither(_tmp17A,sizeof(char),2);}),_tag_dyneither(
_tmp179,sizeof(void*),0));});}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_or_consume(
struct _RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*mc=c2.may_consume;{struct _RegionHandle _tmp17B=
_new_region("temp");struct _RegionHandle*temp=& _tmp17B;_push_region(temp);{struct
Cyc_List_List*_tmp17C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rcopy)(temp,c1.may_consume);((void(*)(void(*f)(struct Cyc_List_List**,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List**env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_CfFlowInfo_remove_f,& _tmp17C,c2.may_consume);while(_tmp17C != 0){mc=({struct
Cyc_List_List*_tmp17D=_region_malloc(r,sizeof(*_tmp17D));_tmp17D->hd=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmp17C))->hd;_tmp17D->tl=mc;_tmp17D;});_tmp17C=((
struct Cyc_List_List*)_check_null(_tmp17C))->tl;}};_pop_region(temp);}return({
struct Cyc_CfFlowInfo_ConsumeInfo _tmp17E;_tmp17E.consumed=Cyc_CfFlowInfo_union_place_set(
c1.consumed,c2.consumed,0);_tmp17E.may_consume=mc;_tmp17E;});}struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_CfFlowInfo_and_consume(struct _RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo
c1,struct Cyc_CfFlowInfo_ConsumeInfo c2){struct Cyc_List_List*_tmp17F=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,c1.may_consume,c2.may_consume);return({struct Cyc_CfFlowInfo_ConsumeInfo _tmp180;
_tmp180.consumed=Cyc_CfFlowInfo_union_place_set(c1.consumed,c2.consumed,1);
_tmp180.may_consume=_tmp17F;_tmp180;});}struct _tuple11{union Cyc_CfFlowInfo_FlowInfo_union
f1;union Cyc_CfFlowInfo_FlowInfo_union f2;};union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_join_flow(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo_union
f1,union Cyc_CfFlowInfo_FlowInfo_union f2,int or_consumed){struct _tuple11 _tmp182=({
struct _tuple11 _tmp181;_tmp181.f1=f1;_tmp181.f2=f2;_tmp181;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp183;union Cyc_CfFlowInfo_FlowInfo_union _tmp184;union Cyc_CfFlowInfo_FlowInfo_union
_tmp185;struct Cyc_Dict_Dict _tmp186;struct Cyc_List_List*_tmp187;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp188;union Cyc_CfFlowInfo_FlowInfo_union _tmp189;struct Cyc_Dict_Dict _tmp18A;
struct Cyc_List_List*_tmp18B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp18C;_LLDE:
_tmp183=_tmp182.f1;if(((_tmp182.f1).BottomFL).tag != 0)goto _LLE0;_LLDF: return f2;
_LLE0: _tmp184=_tmp182.f2;if(((_tmp182.f2).BottomFL).tag != 0)goto _LLE2;_LLE1:
return f1;_LLE2: _tmp185=_tmp182.f1;if(((_tmp182.f1).ReachableFL).tag != 1)goto
_LLDD;_tmp186=(_tmp185.ReachableFL).f1;_tmp187=(_tmp185.ReachableFL).f2;_tmp188=(
_tmp185.ReachableFL).f3;_tmp189=_tmp182.f2;if(((_tmp182.f2).ReachableFL).tag != 1)
goto _LLDD;_tmp18A=(_tmp189.ReachableFL).f1;_tmp18B=(_tmp189.ReachableFL).f2;
_tmp18C=(_tmp189.ReachableFL).f3;_LLE3: if(((_tmp186.t == _tmp18A.t  && _tmp187 == 
_tmp18B) && _tmp188.may_consume == _tmp18C.may_consume) && (_tmp188.consumed).t == (
_tmp18C.consumed).t)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))
return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle _tmp18D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp18D;
_push_region(rgn);{struct Cyc_CfFlowInfo_JoinEnv _tmp18E=({struct Cyc_CfFlowInfo_JoinEnv
_tmp194;_tmp194.fenv=fenv;_tmp194.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp195=
_region_malloc(rgn,sizeof(*_tmp195));_tmp195->rgn=rgn;_tmp195->places=0;_tmp195;});
_tmp194.d1=_tmp186;_tmp194.d2=_tmp18A;_tmp194;});struct Cyc_Dict_Dict _tmp18F=((
struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),
struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))
Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_absRval,& _tmp18E,_tmp186,_tmp18A);
struct Cyc_List_List*_tmp190=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp187,_tmp18B);
struct Cyc_CfFlowInfo_ConsumeInfo _tmp191=or_consumed?Cyc_CfFlowInfo_or_consume(
fenv->r,_tmp188,_tmp18C): Cyc_CfFlowInfo_and_consume(fenv->r,_tmp188,_tmp18C);
union Cyc_CfFlowInfo_FlowInfo_union _tmp193=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp192;(_tmp192.ReachableFL).tag=1;(_tmp192.ReachableFL).f1=
Cyc_CfFlowInfo_escape_these(fenv,_tmp18E.pile,all_changed,_tmp18F);(_tmp192.ReachableFL).f2=
_tmp190;(_tmp192.ReachableFL).f3=_tmp191;_tmp192;});_npop_handler(0);return
_tmp193;};_pop_region(rgn);}_LLDD:;}struct _tuple12{union Cyc_CfFlowInfo_FlowInfo_union
f1;union Cyc_CfFlowInfo_FlowInfo_union f2;union Cyc_CfFlowInfo_FlowInfo_union f3;};
struct _tuple4 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict*all_changed,struct _tuple4 pr1,struct _tuple4 pr2,int or_consumed){
union Cyc_CfFlowInfo_FlowInfo_union _tmp197;void*_tmp198;struct _tuple4 _tmp196=pr1;
_tmp197=_tmp196.f1;_tmp198=_tmp196.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp19A;
void*_tmp19B;struct _tuple4 _tmp199=pr2;_tmp19A=_tmp199.f1;_tmp19B=_tmp199.f2;{
union Cyc_CfFlowInfo_FlowInfo_union outflow=Cyc_CfFlowInfo_join_flow(fenv,
all_changed,_tmp197,_tmp19A,or_consumed);struct _tuple12 _tmp19D=({struct _tuple12
_tmp19C;_tmp19C.f1=_tmp197;_tmp19C.f2=_tmp19A;_tmp19C.f3=outflow;_tmp19C;});
union Cyc_CfFlowInfo_FlowInfo_union _tmp19E;union Cyc_CfFlowInfo_FlowInfo_union
_tmp19F;union Cyc_CfFlowInfo_FlowInfo_union _tmp1A0;struct Cyc_Dict_Dict _tmp1A1;
union Cyc_CfFlowInfo_FlowInfo_union _tmp1A2;struct Cyc_Dict_Dict _tmp1A3;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1A4;struct Cyc_Dict_Dict _tmp1A5;struct Cyc_List_List*_tmp1A6;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1A7;_LLE5: _tmp19E=_tmp19D.f1;if(((_tmp19D.f1).BottomFL).tag != 0)goto _LLE7;
_LLE6: return({struct _tuple4 _tmp1A8;_tmp1A8.f1=outflow;_tmp1A8.f2=_tmp19B;_tmp1A8;});
_LLE7: _tmp19F=_tmp19D.f2;if(((_tmp19D.f2).BottomFL).tag != 0)goto _LLE9;_LLE8:
return({struct _tuple4 _tmp1A9;_tmp1A9.f1=outflow;_tmp1A9.f2=_tmp198;_tmp1A9;});
_LLE9: _tmp1A0=_tmp19D.f1;if(((_tmp19D.f1).ReachableFL).tag != 1)goto _LLEB;_tmp1A1=(
_tmp1A0.ReachableFL).f1;_tmp1A2=_tmp19D.f2;if(((_tmp19D.f2).ReachableFL).tag != 1)
goto _LLEB;_tmp1A3=(_tmp1A2.ReachableFL).f1;_tmp1A4=_tmp19D.f3;if(((_tmp19D.f3).ReachableFL).tag
!= 1)goto _LLEB;_tmp1A5=(_tmp1A4.ReachableFL).f1;_tmp1A6=(_tmp1A4.ReachableFL).f2;
_tmp1A7=(_tmp1A4.ReachableFL).f3;_LLEA: if(((int(*)(int ignore,void*r1,void*r2))
Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp198,_tmp19B))return({struct _tuple4
_tmp1AA;_tmp1AA.f1=outflow;_tmp1AA.f2=_tmp19B;_tmp1AA;});if(((int(*)(int ignore,
void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp19B,_tmp198))
return({struct _tuple4 _tmp1AB;_tmp1AB.f1=outflow;_tmp1AB.f2=_tmp198;_tmp1AB;});{
struct _RegionHandle _tmp1AC=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1AC;
_push_region(rgn);{struct Cyc_CfFlowInfo_JoinEnv _tmp1AD=({struct Cyc_CfFlowInfo_JoinEnv
_tmp1B2;_tmp1B2.fenv=fenv;_tmp1B2.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp1B3=
_region_malloc(rgn,sizeof(*_tmp1B3));_tmp1B3->rgn=rgn;_tmp1B3->places=0;_tmp1B3;});
_tmp1B2.d1=_tmp1A1;_tmp1B2.d2=_tmp1A3;_tmp1B2;});void*_tmp1AE=((void*(*)(struct
Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(&
_tmp1AD,0,_tmp198,_tmp19B);struct _tuple4 _tmp1B1=({struct _tuple4 _tmp1AF;_tmp1AF.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp1B0;(
_tmp1B0.ReachableFL).tag=1;(_tmp1B0.ReachableFL).f1=Cyc_CfFlowInfo_escape_these(
fenv,_tmp1AD.pile,all_changed,_tmp1A5);(_tmp1B0.ReachableFL).f2=_tmp1A6;(_tmp1B0.ReachableFL).f3=
_tmp1A7;_tmp1B0;});_tmp1AF.f2=_tmp1AE;_tmp1AF;});_npop_handler(0);return _tmp1B1;};
_pop_region(rgn);}_LLEB:;_LLEC:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4[0]=({struct Cyc_Core_Impossible_struct
_tmp1B5;_tmp1B5.tag=Cyc_Core_Impossible;_tmp1B5.f1=({const char*_tmp1B6="join_flow_and_rval: BottomFL outflow";
_tag_dyneither(_tmp1B6,sizeof(char),37);});_tmp1B5;});_tmp1B4;}));_LLE4:;}}}
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,int,
void*,void*);static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*
env,void*r1,void*r2){int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,
env->curr_place);int changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,
env->curr_place);if(changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
if(changed1){if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))
return r1;env->changed=Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(
env->curr_place,env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{struct
_tuple0 _tmp1B8=({struct _tuple0 _tmp1B7;_tmp1B7.f1=r1;_tmp1B7.f2=r2;_tmp1B7;});
void*_tmp1B9;struct _dyneither_ptr _tmp1BA;void*_tmp1BB;struct _dyneither_ptr
_tmp1BC;_LLEE: _tmp1B9=_tmp1B8.f1;if(_tmp1B9 <= (void*)3)goto _LLF0;if(*((int*)
_tmp1B9)!= 4)goto _LLF0;_tmp1BA=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1B9)->f1;
_tmp1BB=_tmp1B8.f2;if(_tmp1BB <= (void*)3)goto _LLF0;if(*((int*)_tmp1BB)!= 4)goto
_LLF0;_tmp1BC=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1BB)->f1;_LLEF: {
struct _dyneither_ptr new_d=({unsigned int _tmp1BF=_get_dyneither_size(_tmp1BA,
sizeof(void*));void**_tmp1C0=(void**)_region_malloc(((env->joinenv).fenv)->r,
_check_times(sizeof(void*),_tmp1BF));struct _dyneither_ptr _tmp1C2=_tag_dyneither(
_tmp1C0,sizeof(void*),_tmp1BF);{unsigned int _tmp1C1=_tmp1BF;unsigned int i;for(i=
0;i < _tmp1C1;i ++){_tmp1C0[i]=Cyc_CfFlowInfo_after_absRval_d(env,(int)i,*((void**)
_check_dyneither_subscript(_tmp1BA,sizeof(void*),(int)i)),*((void**)
_check_dyneither_subscript(_tmp1BC,sizeof(void*),(int)i)));}}_tmp1C2;});int
change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp1BA,sizeof(void*));++ i){if(*((
void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp1BA,sizeof(void*),i))){change=1;break;}}}if(!
change)return r1;change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp1BA,sizeof(
void*));++ i){if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((
void**)_check_dyneither_subscript(_tmp1BC,sizeof(void*),i))){change=1;break;}}}
if(!change)return r2;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1BD=
_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp1BD));_tmp1BD[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp1BE;_tmp1BE.tag=4;_tmp1BE.f1=new_d;_tmp1BE;});_tmp1BD;});}_LLF0:;_LLF1:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1C3=_cycalloc(sizeof(*
_tmp1C3));_tmp1C3[0]=({struct Cyc_Core_Impossible_struct _tmp1C4;_tmp1C4.tag=Cyc_Core_Impossible;
_tmp1C4.f1=({const char*_tmp1C5="after_absRval -- non-aggregates!";_tag_dyneither(
_tmp1C5,sizeof(char),33);});_tmp1C4;});_tmp1C3;}));_LLED:;}}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp1C6=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged
_tmp1C7=env->changed;*env->last_field_cell=({struct Cyc_List_List*_tmp1C8=
_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp1C8));_tmp1C8->hd=(void*)key;
_tmp1C8->tl=0;_tmp1C8;});env->last_field_cell=(struct Cyc_List_List**)&((struct
Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{void*_tmp1C9=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp1C6;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp1C7;return _tmp1C9;}}}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;*
env->curr_place=({struct Cyc_CfFlowInfo_Place _tmp1CA;_tmp1CA.root=(void*)root;
_tmp1CA.fields=0;_tmp1CA;});env->last_field_cell=(struct Cyc_List_List**)&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo_union f1,union
Cyc_CfFlowInfo_FlowInfo_union f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){
struct _tuple11 _tmp1CC=({struct _tuple11 _tmp1CB;_tmp1CB.f1=f1;_tmp1CB.f2=f2;
_tmp1CB;});union Cyc_CfFlowInfo_FlowInfo_union _tmp1CD;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1CE;union Cyc_CfFlowInfo_FlowInfo_union _tmp1CF;struct Cyc_Dict_Dict _tmp1D0;
struct Cyc_List_List*_tmp1D1;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1D2;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1D3;struct Cyc_Dict_Dict _tmp1D4;struct Cyc_List_List*_tmp1D5;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1D6;_LLF3: _tmp1CD=_tmp1CC.f1;if(((_tmp1CC.f1).BottomFL).tag != 0)goto _LLF5;
_LLF4: goto _LLF6;_LLF5: _tmp1CE=_tmp1CC.f2;if(((_tmp1CC.f2).BottomFL).tag != 0)goto
_LLF7;_LLF6: return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1D7;(_tmp1D7.BottomFL).tag=0;_tmp1D7;});_LLF7: _tmp1CF=_tmp1CC.f1;if(((
_tmp1CC.f1).ReachableFL).tag != 1)goto _LLF2;_tmp1D0=(_tmp1CF.ReachableFL).f1;
_tmp1D1=(_tmp1CF.ReachableFL).f2;_tmp1D2=(_tmp1CF.ReachableFL).f3;_tmp1D3=
_tmp1CC.f2;if(((_tmp1CC.f2).ReachableFL).tag != 1)goto _LLF2;_tmp1D4=(_tmp1D3.ReachableFL).f1;
_tmp1D5=(_tmp1D3.ReachableFL).f2;_tmp1D6=(_tmp1D3.ReachableFL).f3;_LLF8: if(((
_tmp1D0.t == _tmp1D4.t  && _tmp1D1 == _tmp1D5) && _tmp1D2.may_consume == _tmp1D6.may_consume)
 && (_tmp1D2.consumed).t == (_tmp1D6.consumed).t)return f1;{struct _RegionHandle
_tmp1D8=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1D8;_push_region(rgn);{
struct Cyc_CfFlowInfo_Place*_tmp1D9=fenv->dummy_place;struct Cyc_CfFlowInfo_AfterEnv
_tmp1DA=({struct Cyc_CfFlowInfo_AfterEnv _tmp1E0;_tmp1E0.joinenv=({struct Cyc_CfFlowInfo_JoinEnv
_tmp1E1;_tmp1E1.fenv=fenv;_tmp1E1.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp1E2=
_region_malloc(rgn,sizeof(*_tmp1E2));_tmp1E2->rgn=rgn;_tmp1E2->places=0;_tmp1E2;});
_tmp1E1.d1=_tmp1D0;_tmp1E1.d2=_tmp1D4;_tmp1E1;});_tmp1E0.chg1=chg1;_tmp1E0.chg2=
chg2;_tmp1E0.curr_place=_tmp1D9;_tmp1E0.last_field_cell=(struct Cyc_List_List**)&
_tmp1D9->fields;_tmp1E0.changed=Cyc_CfFlowInfo_Neither;_tmp1E0;});struct Cyc_Dict_Dict
_tmp1DB=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,
void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,& _tmp1DA,_tmp1D0,_tmp1D4);
struct Cyc_List_List*_tmp1DC=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp1D1,_tmp1D5);
struct Cyc_CfFlowInfo_ConsumeInfo _tmp1DD=Cyc_CfFlowInfo_and_consume(fenv->r,
_tmp1D2,_tmp1D6);union Cyc_CfFlowInfo_FlowInfo_union _tmp1DF=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1DE;(_tmp1DE.ReachableFL).tag=1;(_tmp1DE.ReachableFL).f1=
Cyc_CfFlowInfo_escape_these(fenv,(_tmp1DA.joinenv).pile,all_changed,_tmp1DB);(
_tmp1DE.ReachableFL).f2=_tmp1DC;(_tmp1DE.ReachableFL).f3=_tmp1DD;_tmp1DE;});
_npop_handler(0);return _tmp1DF;};_pop_region(rgn);}_LLF2:;}static int Cyc_CfFlowInfo_tag_cmps_lessthan_approx(
struct Cyc_List_List*l1,struct Cyc_List_List*l2){for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp1E4;void*_tmp1E5;void*_tmp1E6;struct Cyc_CfFlowInfo_TagCmp*_tmp1E3=(struct Cyc_CfFlowInfo_TagCmp*)
l2->hd;_tmp1E4=*_tmp1E3;_tmp1E5=(void*)_tmp1E4.cmp;_tmp1E6=(void*)_tmp1E4.bd;{
struct Cyc_List_List*_tmp1E7=l1;for(0;_tmp1E7 != 0;_tmp1E7=_tmp1E7->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp1E9;void*_tmp1EA;void*_tmp1EB;struct Cyc_CfFlowInfo_TagCmp*_tmp1E8=(struct Cyc_CfFlowInfo_TagCmp*)
_tmp1E7->hd;_tmp1E9=*_tmp1E8;_tmp1EA=(void*)_tmp1E9.cmp;_tmp1EB=(void*)_tmp1E9.bd;
if(_tmp1EA == _tmp1E5  && Cyc_Tcutil_typecmp(_tmp1EB,_tmp1E6)== 0)break;}if(
_tmp1E7 == 0)return 0;}}return 1;}static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp1ED=({struct
_tuple0 _tmp1EC;_tmp1EC.f1=r1;_tmp1EC.f2=r2;_tmp1EC;});void*_tmp1EE;struct Cyc_CfFlowInfo_Place*
_tmp1EF;void*_tmp1F0;struct Cyc_CfFlowInfo_Place*_tmp1F1;void*_tmp1F2;void*
_tmp1F3;void*_tmp1F4;struct _dyneither_ptr _tmp1F5;void*_tmp1F6;struct
_dyneither_ptr _tmp1F7;void*_tmp1F8;void*_tmp1F9;void*_tmp1FA;void*_tmp1FB;void*
_tmp1FC;void*_tmp1FD;void*_tmp1FE;struct Cyc_List_List*_tmp1FF;void*_tmp200;
struct Cyc_List_List*_tmp201;void*_tmp202;_LLFA: _tmp1EE=_tmp1ED.f1;if(_tmp1EE <= (
void*)3)goto _LLFC;if(*((int*)_tmp1EE)!= 2)goto _LLFC;_tmp1EF=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp1EE)->f1;_tmp1F0=_tmp1ED.f2;if(_tmp1F0 <= (void*)3)goto _LLFC;if(*((int*)
_tmp1F0)!= 2)goto _LLFC;_tmp1F1=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1F0)->f1;
_LLFB: return Cyc_CfFlowInfo_place_cmp(_tmp1EF,_tmp1F1)== 0;_LLFC: _tmp1F2=_tmp1ED.f1;
if(_tmp1F2 <= (void*)3)goto _LLFE;if(*((int*)_tmp1F2)!= 2)goto _LLFE;_LLFD: goto
_LLFF;_LLFE: _tmp1F3=_tmp1ED.f2;if(_tmp1F3 <= (void*)3)goto _LL100;if(*((int*)
_tmp1F3)!= 2)goto _LL100;_LLFF: return 0;_LL100: _tmp1F4=_tmp1ED.f1;if(_tmp1F4 <= (
void*)3)goto _LL102;if(*((int*)_tmp1F4)!= 4)goto _LL102;_tmp1F5=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1F4)->f1;_tmp1F6=_tmp1ED.f2;if(_tmp1F6 <= (void*)3)goto _LL102;if(*((int*)
_tmp1F6)!= 4)goto _LL102;_tmp1F7=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1F6)->f1;
_LL101: if(_tmp1F5.curr == _tmp1F7.curr)return 1;{int i=0;for(0;i < 
_get_dyneither_size(_tmp1F5,sizeof(void*));++ i){if(!((int(*)(int ignore,void*r1,
void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,*((void**)
_check_dyneither_subscript(_tmp1F5,sizeof(void*),i)),*((void**)
_check_dyneither_subscript(_tmp1F7,sizeof(void*),i))))return 0;}}return 1;_LL102:
_tmp1F8=_tmp1ED.f2;if((int)_tmp1F8 != 2)goto _LL104;_LL103: return r1 == (void*)1;
_LL104: _tmp1F9=_tmp1ED.f2;if((int)_tmp1F9 != 0)goto _LL106;_LL105: goto _LL107;
_LL106: _tmp1FA=_tmp1ED.f2;if((int)_tmp1FA != 1)goto _LL108;_LL107: return 0;_LL108:
_tmp1FB=_tmp1ED.f1;if(_tmp1FB <= (void*)3)goto _LL10A;if(*((int*)_tmp1FB)!= 1)goto
_LL10A;_tmp1FC=_tmp1ED.f2;if(_tmp1FC <= (void*)3)goto _LL10A;if(*((int*)_tmp1FC)!= 
1)goto _LL10A;_LL109: goto _LLF9;_LL10A: _tmp1FD=_tmp1ED.f1;if(_tmp1FD <= (void*)3)
goto _LL10C;if(*((int*)_tmp1FD)!= 1)goto _LL10C;_LL10B: return 0;_LL10C: _tmp1FE=
_tmp1ED.f1;if(_tmp1FE <= (void*)3)goto _LL10E;if(*((int*)_tmp1FE)!= 3)goto _LL10E;
_tmp1FF=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp1FE)->f1;_tmp200=_tmp1ED.f2;
if(_tmp200 <= (void*)3)goto _LL10E;if(*((int*)_tmp200)!= 3)goto _LL10E;_tmp201=((
struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp200)->f1;_LL10D: return Cyc_CfFlowInfo_tag_cmps_lessthan_approx(
_tmp1FF,_tmp201);_LL10E: _tmp202=_tmp1ED.f2;if(_tmp202 <= (void*)3)goto _LL110;if(*((
int*)_tmp202)!= 3)goto _LL110;_LL10F: return 0;_LL110:;_LL111: goto _LLF9;_LLF9:;}{
struct _tuple0 _tmp204=({struct _tuple0 _tmp203;_tmp203.f1=Cyc_CfFlowInfo_initlevel_approx(
r1);_tmp203.f2=Cyc_CfFlowInfo_initlevel_approx(r2);_tmp203;});void*_tmp205;void*
_tmp206;void*_tmp207;void*_tmp208;void*_tmp209;void*_tmp20A;_LL113: _tmp205=
_tmp204.f1;if((int)_tmp205 != 2)goto _LL115;_tmp206=_tmp204.f2;if((int)_tmp206 != 2)
goto _LL115;_LL114: return 1;_LL115: _tmp207=_tmp204.f2;if((int)_tmp207 != 0)goto
_LL117;_LL116: return 1;_LL117: _tmp208=_tmp204.f1;if((int)_tmp208 != 0)goto _LL119;
_LL118: return 0;_LL119: _tmp209=_tmp204.f2;if((int)_tmp209 != 1)goto _LL11B;_LL11A:
return 1;_LL11B: _tmp20A=_tmp204.f1;if((int)_tmp20A != 1)goto _LL112;_LL11C: return 0;
_LL112:;}}static int Cyc_CfFlowInfo_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*
r1s){if(r1s == r2s)return 1;for(0;r1s != 0;r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*
_tmp20B=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;int found=0;{struct Cyc_List_List*
_tmp20C=r2s;for(0;_tmp20C != 0;_tmp20C=_tmp20C->tl){struct Cyc_CfFlowInfo_Reln*
_tmp20D=(struct Cyc_CfFlowInfo_Reln*)_tmp20C->hd;if(_tmp20B == _tmp20D  || _tmp20B->vd
== _tmp20D->vd  && Cyc_CfFlowInfo_same_relop(_tmp20B->rop,_tmp20D->rop)){found=1;
break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_consume_approx(struct Cyc_CfFlowInfo_ConsumeInfo
c1,struct Cyc_CfFlowInfo_ConsumeInfo c2){if(c1.may_consume != c2.may_consume){
struct Cyc_List_List*_tmp20E=c1.may_consume;struct Cyc_List_List*_tmp20F=c2.may_consume;
for(0;_tmp20F != 0  && _tmp20E != 0;_tmp20F=_tmp20F->tl){if((unsigned int)_tmp20E == (
unsigned int)_tmp20F)goto compare_consumed_set;{int _tmp210=Cyc_CfFlowInfo_place_cmp((
struct Cyc_CfFlowInfo_Place*)_tmp20E->hd,(struct Cyc_CfFlowInfo_Place*)_tmp20F->hd);
if(_tmp210 == 0)_tmp20E=_tmp20E->tl;}}if(_tmp20E != 0)return 0;}
compare_consumed_set: return Cyc_CfFlowInfo_place_set_subset(c1.consumed,c2.consumed);}
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo_union f1,
union Cyc_CfFlowInfo_FlowInfo_union f2){struct _tuple11 _tmp212=({struct _tuple11
_tmp211;_tmp211.f1=f1;_tmp211.f2=f2;_tmp211;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp213;union Cyc_CfFlowInfo_FlowInfo_union _tmp214;union Cyc_CfFlowInfo_FlowInfo_union
_tmp215;struct Cyc_Dict_Dict _tmp216;struct Cyc_List_List*_tmp217;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp218;union Cyc_CfFlowInfo_FlowInfo_union _tmp219;struct Cyc_Dict_Dict _tmp21A;
struct Cyc_List_List*_tmp21B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp21C;_LL11E:
_tmp213=_tmp212.f1;if(((_tmp212.f1).BottomFL).tag != 0)goto _LL120;_LL11F: return 1;
_LL120: _tmp214=_tmp212.f2;if(((_tmp212.f2).BottomFL).tag != 0)goto _LL122;_LL121:
return 0;_LL122: _tmp215=_tmp212.f1;if(((_tmp212.f1).ReachableFL).tag != 1)goto
_LL11D;_tmp216=(_tmp215.ReachableFL).f1;_tmp217=(_tmp215.ReachableFL).f2;_tmp218=(
_tmp215.ReachableFL).f3;_tmp219=_tmp212.f2;if(((_tmp212.f2).ReachableFL).tag != 1)
goto _LL11D;_tmp21A=(_tmp219.ReachableFL).f1;_tmp21B=(_tmp219.ReachableFL).f2;
_tmp21C=(_tmp219.ReachableFL).f3;_LL123: if(((_tmp216.t == _tmp21A.t  && _tmp217 == 
_tmp21B) && _tmp218.may_consume == _tmp21C.may_consume) && (_tmp218.consumed).t == (
_tmp21C.consumed).t)return 1;return(Cyc_Dict_forall_intersect(Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp216,_tmp21A) && Cyc_CfFlowInfo_relns_approx(_tmp217,_tmp21B)) && Cyc_CfFlowInfo_consume_approx(
_tmp218,_tmp21C);_LL11D:;}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(
struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){struct
Cyc_List_List*p;int found=0;for(p=rs;!found  && p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*
_tmp21D=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp21D->vd == v){found=1;break;}{
union Cyc_CfFlowInfo_RelnOp_union _tmp21E=_tmp21D->rop;struct Cyc_Absyn_Vardecl*
_tmp21F;struct Cyc_Absyn_Vardecl*_tmp220;struct Cyc_Absyn_Vardecl*_tmp221;_LL125:
if((_tmp21E.LessVar).tag != 1)goto _LL127;_tmp21F=(_tmp21E.LessVar).f1;_LL126:
_tmp220=_tmp21F;goto _LL128;_LL127: if((_tmp21E.LessNumelts).tag != 2)goto _LL129;
_tmp220=(_tmp21E.LessNumelts).f1;_LL128: _tmp221=_tmp220;goto _LL12A;_LL129: if((
_tmp21E.LessEqNumelts).tag != 4)goto _LL12B;_tmp221=(_tmp21E.LessEqNumelts).f1;
_LL12A: if(v == _tmp221)found=1;goto _LL124;_LL12B:;_LL12C: goto _LL124;_LL124:;}}if(
!found)return rs;{struct Cyc_List_List*_tmp222=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*
_tmp223=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp223->vd != v){{union Cyc_CfFlowInfo_RelnOp_union
_tmp224=_tmp223->rop;struct Cyc_Absyn_Vardecl*_tmp225;struct Cyc_Absyn_Vardecl*
_tmp226;struct Cyc_Absyn_Vardecl*_tmp227;_LL12E: if((_tmp224.LessVar).tag != 1)goto
_LL130;_tmp225=(_tmp224.LessVar).f1;_LL12F: _tmp226=_tmp225;goto _LL131;_LL130: if((
_tmp224.LessNumelts).tag != 2)goto _LL132;_tmp226=(_tmp224.LessNumelts).f1;_LL131:
_tmp227=_tmp226;goto _LL133;_LL132: if((_tmp224.LessEqNumelts).tag != 4)goto _LL134;
_tmp227=(_tmp224.LessEqNumelts).f1;_LL133: if(v == _tmp227)continue;goto _LL12D;
_LL134:;_LL135: goto _LL12D;_LL12D:;}_tmp222=({struct Cyc_List_List*_tmp228=
_region_malloc(rgn,sizeof(*_tmp228));_tmp228->hd=_tmp223;_tmp228->tl=_tmp222;
_tmp228;});}}return _tmp222;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*
_tmp229=(void*)e->r;void*_tmp22A;struct Cyc_Absyn_Vardecl*_tmp22B;void*_tmp22C;
struct Cyc_Absyn_Vardecl*_tmp22D;void*_tmp22E;struct Cyc_Absyn_Vardecl*_tmp22F;
void*_tmp230;struct Cyc_Absyn_Vardecl*_tmp231;_LL137: if(*((int*)_tmp229)!= 1)goto
_LL139;_tmp22A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp229)->f2;if(_tmp22A <= (
void*)1)goto _LL139;if(*((int*)_tmp22A)!= 0)goto _LL139;_tmp22B=((struct Cyc_Absyn_Global_b_struct*)
_tmp22A)->f1;_LL138: _tmp22D=_tmp22B;goto _LL13A;_LL139: if(*((int*)_tmp229)!= 1)
goto _LL13B;_tmp22C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp229)->f2;if(
_tmp22C <= (void*)1)goto _LL13B;if(*((int*)_tmp22C)!= 2)goto _LL13B;_tmp22D=((
struct Cyc_Absyn_Param_b_struct*)_tmp22C)->f1;_LL13A: _tmp22F=_tmp22D;goto _LL13C;
_LL13B: if(*((int*)_tmp229)!= 1)goto _LL13D;_tmp22E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp229)->f2;if(_tmp22E <= (void*)1)goto _LL13D;if(*((int*)_tmp22E)!= 3)goto _LL13D;
_tmp22F=((struct Cyc_Absyn_Local_b_struct*)_tmp22E)->f1;_LL13C: _tmp231=_tmp22F;
goto _LL13E;_LL13D: if(*((int*)_tmp229)!= 1)goto _LL13F;_tmp230=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp229)->f2;if(_tmp230 <= (void*)1)goto _LL13F;if(*((int*)_tmp230)!= 4)goto _LL13F;
_tmp231=((struct Cyc_Absyn_Pat_b_struct*)_tmp230)->f1;_LL13E: if(!_tmp231->escapes)
return Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp231);goto _LL136;_LL13F:;_LL140: goto
_LL136;_LL136:;}return r;}struct Cyc_CfFlowInfo_TagCmp*Cyc_CfFlowInfo_copy_tagcmp(
struct _RegionHandle*r2,struct Cyc_CfFlowInfo_TagCmp*t){return({struct Cyc_CfFlowInfo_TagCmp*
_tmp232=_region_malloc(r2,sizeof(*_tmp232));_tmp232[0]=*t;_tmp232;});}struct Cyc_List_List*
Cyc_CfFlowInfo_copy_tagcmps(struct _RegionHandle*r2,struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_CfFlowInfo_TagCmp*(*
f)(struct _RegionHandle*,struct Cyc_CfFlowInfo_TagCmp*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_CfFlowInfo_copy_tagcmp,r2,ts);}
struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(struct _RegionHandle*r2,struct
Cyc_CfFlowInfo_Reln*r){return({struct Cyc_CfFlowInfo_Reln*_tmp233=_region_malloc(
r2,sizeof(*_tmp233));_tmp233[0]=*r;_tmp233;});}struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(
struct _RegionHandle*r2,struct Cyc_List_List*relns){return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*f)(struct _RegionHandle*,struct
Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*
r1,struct Cyc_List_List*r2){for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){struct
Cyc_CfFlowInfo_Reln*_tmp234=(struct Cyc_CfFlowInfo_Reln*)r1->hd;struct Cyc_CfFlowInfo_Reln*
_tmp235=(struct Cyc_CfFlowInfo_Reln*)r2->hd;if(_tmp234->vd != _tmp235->vd  || !Cyc_CfFlowInfo_same_relop(
_tmp234->rop,_tmp235->rop))return 0;}if(r1 != 0  || r2 != 0)return 0;else{return 1;}}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,
struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)
return r;r=Cyc_CfFlowInfo_reln_kill_var(rgn,r,v);{void*_tmp236=(void*)e->r;struct
Cyc_Absyn_MallocInfo _tmp237;struct Cyc_Absyn_Exp*_tmp238;int _tmp239;_LL142: if(*((
int*)_tmp236)!= 35)goto _LL144;_tmp237=((struct Cyc_Absyn_Malloc_e_struct*)_tmp236)->f1;
_tmp238=_tmp237.num_elts;_tmp239=_tmp237.fat_result;if(_tmp239 != 1)goto _LL144;
_LL143: malloc_loop: {void*_tmp23A=(void*)_tmp238->r;struct Cyc_Absyn_Exp*_tmp23B;
void*_tmp23C;struct Cyc_Absyn_Vardecl*_tmp23D;void*_tmp23E;struct Cyc_Absyn_Vardecl*
_tmp23F;void*_tmp240;struct Cyc_Absyn_Vardecl*_tmp241;void*_tmp242;struct Cyc_Absyn_Vardecl*
_tmp243;_LL147: if(*((int*)_tmp23A)!= 15)goto _LL149;_tmp23B=((struct Cyc_Absyn_Cast_e_struct*)
_tmp23A)->f2;_LL148: _tmp238=_tmp23B;goto malloc_loop;_LL149: if(*((int*)_tmp23A)!= 
1)goto _LL14B;_tmp23C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp23A)->f2;if(
_tmp23C <= (void*)1)goto _LL14B;if(*((int*)_tmp23C)!= 4)goto _LL14B;_tmp23D=((
struct Cyc_Absyn_Pat_b_struct*)_tmp23C)->f1;_LL14A: _tmp23F=_tmp23D;goto _LL14C;
_LL14B: if(*((int*)_tmp23A)!= 1)goto _LL14D;_tmp23E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23A)->f2;if(_tmp23E <= (void*)1)goto _LL14D;if(*((int*)_tmp23E)!= 3)goto _LL14D;
_tmp23F=((struct Cyc_Absyn_Local_b_struct*)_tmp23E)->f1;_LL14C: _tmp241=_tmp23F;
goto _LL14E;_LL14D: if(*((int*)_tmp23A)!= 1)goto _LL14F;_tmp240=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23A)->f2;if(_tmp240 <= (void*)1)goto _LL14F;if(*((int*)_tmp240)!= 2)goto _LL14F;
_tmp241=((struct Cyc_Absyn_Param_b_struct*)_tmp240)->f1;_LL14E: _tmp243=_tmp241;
goto _LL150;_LL14F: if(*((int*)_tmp23A)!= 1)goto _LL151;_tmp242=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp23A)->f2;if(_tmp242 <= (void*)1)goto _LL151;if(*((int*)_tmp242)!= 0)goto _LL151;
_tmp243=((struct Cyc_Absyn_Global_b_struct*)_tmp242)->f1;_LL150: if(_tmp243->escapes)
return r;return({struct Cyc_List_List*_tmp244=_region_malloc(rgn,sizeof(*_tmp244));
_tmp244->hd=({struct Cyc_CfFlowInfo_Reln*_tmp245=_region_malloc(rgn,sizeof(*
_tmp245));_tmp245->vd=_tmp243;_tmp245->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp246;(_tmp246.LessEqNumelts).tag=4;(_tmp246.LessEqNumelts).f1=
v;_tmp246;});_tmp245;});_tmp244->tl=r;_tmp244;});_LL151:;_LL152: return r;_LL146:;}
_LL144:;_LL145: goto _LL141;_LL141:;}{void*_tmp247=Cyc_Tcutil_compress((void*)v->type);
_LL154: if(_tmp247 <= (void*)4)goto _LL156;if(*((int*)_tmp247)!= 5)goto _LL156;
_LL155: goto _LL153;_LL156:;_LL157: return r;_LL153:;}loop: {void*_tmp248=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp249;union Cyc_Absyn_Cnst_union _tmp24A;int _tmp24B;void*
_tmp24C;struct Cyc_List_List*_tmp24D;struct Cyc_List_List _tmp24E;struct Cyc_List_List*
_tmp24F;struct Cyc_List_List _tmp250;struct Cyc_Absyn_Exp*_tmp251;void*_tmp252;
struct Cyc_List_List*_tmp253;struct Cyc_List_List _tmp254;struct Cyc_Absyn_Exp*
_tmp255;_LL159: if(*((int*)_tmp248)!= 15)goto _LL15B;_tmp249=((struct Cyc_Absyn_Cast_e_struct*)
_tmp248)->f2;_LL15A: e=_tmp249;goto loop;_LL15B: if(*((int*)_tmp248)!= 0)goto _LL15D;
_tmp24A=((struct Cyc_Absyn_Const_e_struct*)_tmp248)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp248)->f1).Int_c).tag != 2)goto _LL15D;_tmp24B=(_tmp24A.Int_c).f2;_LL15C: return({
struct Cyc_List_List*_tmp256=_region_malloc(rgn,sizeof(*_tmp256));_tmp256->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp257=_region_malloc(rgn,sizeof(*_tmp257));_tmp257->vd=
v;_tmp257->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp258;(_tmp258.EqualConst).tag=0;(_tmp258.EqualConst).f1=(unsigned int)_tmp24B;
_tmp258;});_tmp257;});_tmp256->tl=r;_tmp256;});_LL15D: if(*((int*)_tmp248)!= 3)
goto _LL15F;_tmp24C=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp248)->f1;if((
int)_tmp24C != 4)goto _LL15F;_tmp24D=((struct Cyc_Absyn_Primop_e_struct*)_tmp248)->f2;
if(_tmp24D == 0)goto _LL15F;_tmp24E=*_tmp24D;_tmp24F=_tmp24E.tl;if(_tmp24F == 0)
goto _LL15F;_tmp250=*_tmp24F;_tmp251=(struct Cyc_Absyn_Exp*)_tmp250.hd;_LL15E:{
void*_tmp259=(void*)_tmp251->r;void*_tmp25A;struct Cyc_List_List*_tmp25B;struct
Cyc_List_List _tmp25C;struct Cyc_Absyn_Exp*_tmp25D;_LL164: if(*((int*)_tmp259)!= 3)
goto _LL166;_tmp25A=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp259)->f1;
_tmp25B=((struct Cyc_Absyn_Primop_e_struct*)_tmp259)->f2;if(_tmp25B == 0)goto
_LL166;_tmp25C=*_tmp25B;_tmp25D=(struct Cyc_Absyn_Exp*)_tmp25C.hd;_LL165:{void*
_tmp25E=(void*)_tmp25D->r;void*_tmp25F;struct Cyc_Absyn_Vardecl*_tmp260;void*
_tmp261;struct Cyc_Absyn_Vardecl*_tmp262;void*_tmp263;struct Cyc_Absyn_Vardecl*
_tmp264;void*_tmp265;struct Cyc_Absyn_Vardecl*_tmp266;_LL169: if(*((int*)_tmp25E)
!= 1)goto _LL16B;_tmp25F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp25E)->f2;if(
_tmp25F <= (void*)1)goto _LL16B;if(*((int*)_tmp25F)!= 0)goto _LL16B;_tmp260=((
struct Cyc_Absyn_Global_b_struct*)_tmp25F)->f1;_LL16A: _tmp262=_tmp260;goto _LL16C;
_LL16B: if(*((int*)_tmp25E)!= 1)goto _LL16D;_tmp261=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp25E)->f2;if(_tmp261 <= (void*)1)goto _LL16D;if(*((int*)_tmp261)!= 3)goto _LL16D;
_tmp262=((struct Cyc_Absyn_Local_b_struct*)_tmp261)->f1;_LL16C: _tmp264=_tmp262;
goto _LL16E;_LL16D: if(*((int*)_tmp25E)!= 1)goto _LL16F;_tmp263=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp25E)->f2;if(_tmp263 <= (void*)1)goto _LL16F;if(*((int*)_tmp263)!= 2)goto _LL16F;
_tmp264=((struct Cyc_Absyn_Param_b_struct*)_tmp263)->f1;_LL16E: _tmp266=_tmp264;
goto _LL170;_LL16F: if(*((int*)_tmp25E)!= 1)goto _LL171;_tmp265=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp25E)->f2;if(_tmp265 <= (void*)1)goto _LL171;if(*((int*)_tmp265)!= 4)goto _LL171;
_tmp266=((struct Cyc_Absyn_Pat_b_struct*)_tmp265)->f1;_LL170: if(_tmp266->escapes)
return r;return({struct Cyc_List_List*_tmp267=_region_malloc(rgn,sizeof(*_tmp267));
_tmp267->hd=({struct Cyc_CfFlowInfo_Reln*_tmp268=_region_malloc(rgn,sizeof(*
_tmp268));_tmp268->vd=v;_tmp268->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union
Cyc_CfFlowInfo_RelnOp_union _tmp269;(_tmp269.LessNumelts).tag=2;(_tmp269.LessNumelts).f1=
_tmp266;_tmp269;});_tmp268;});_tmp267->tl=r;_tmp267;});_LL171:;_LL172: goto _LL168;
_LL168:;}goto _LL163;_LL166:;_LL167: goto _LL163;_LL163:;}goto _LL158;_LL15F: if(*((
int*)_tmp248)!= 3)goto _LL161;_tmp252=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp248)->f1;_tmp253=((struct Cyc_Absyn_Primop_e_struct*)_tmp248)->f2;if(_tmp253
== 0)goto _LL161;_tmp254=*_tmp253;_tmp255=(struct Cyc_Absyn_Exp*)_tmp254.hd;_LL160:{
void*_tmp26A=(void*)_tmp255->r;void*_tmp26B;struct Cyc_Absyn_Vardecl*_tmp26C;void*
_tmp26D;struct Cyc_Absyn_Vardecl*_tmp26E;void*_tmp26F;struct Cyc_Absyn_Vardecl*
_tmp270;void*_tmp271;struct Cyc_Absyn_Vardecl*_tmp272;_LL174: if(*((int*)_tmp26A)
!= 1)goto _LL176;_tmp26B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp26A)->f2;if(
_tmp26B <= (void*)1)goto _LL176;if(*((int*)_tmp26B)!= 0)goto _LL176;_tmp26C=((
struct Cyc_Absyn_Global_b_struct*)_tmp26B)->f1;_LL175: _tmp26E=_tmp26C;goto _LL177;
_LL176: if(*((int*)_tmp26A)!= 1)goto _LL178;_tmp26D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26A)->f2;if(_tmp26D <= (void*)1)goto _LL178;if(*((int*)_tmp26D)!= 3)goto _LL178;
_tmp26E=((struct Cyc_Absyn_Local_b_struct*)_tmp26D)->f1;_LL177: _tmp270=_tmp26E;
goto _LL179;_LL178: if(*((int*)_tmp26A)!= 1)goto _LL17A;_tmp26F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26A)->f2;if(_tmp26F <= (void*)1)goto _LL17A;if(*((int*)_tmp26F)!= 2)goto _LL17A;
_tmp270=((struct Cyc_Absyn_Param_b_struct*)_tmp26F)->f1;_LL179: _tmp272=_tmp270;
goto _LL17B;_LL17A: if(*((int*)_tmp26A)!= 1)goto _LL17C;_tmp271=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26A)->f2;if(_tmp271 <= (void*)1)goto _LL17C;if(*((int*)_tmp271)!= 4)goto _LL17C;
_tmp272=((struct Cyc_Absyn_Pat_b_struct*)_tmp271)->f1;_LL17B: if(_tmp272->escapes)
return r;return({struct Cyc_List_List*_tmp273=_region_malloc(rgn,sizeof(*_tmp273));
_tmp273->hd=({struct Cyc_CfFlowInfo_Reln*_tmp274=_region_malloc(rgn,sizeof(*
_tmp274));_tmp274->vd=v;_tmp274->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union
Cyc_CfFlowInfo_RelnOp_union _tmp275;(_tmp275.LessEqNumelts).tag=4;(_tmp275.LessEqNumelts).f1=
_tmp272;_tmp275;});_tmp274;});_tmp273->tl=r;_tmp273;});_LL17C:;_LL17D: goto _LL173;
_LL173:;}goto _LL158;_LL161:;_LL162: goto _LL158;_LL158:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*_tmp276=(void*)e1->r;void*
_tmp277;struct Cyc_Absyn_Vardecl*_tmp278;void*_tmp279;struct Cyc_Absyn_Vardecl*
_tmp27A;void*_tmp27B;struct Cyc_Absyn_Vardecl*_tmp27C;void*_tmp27D;struct Cyc_Absyn_Vardecl*
_tmp27E;_LL17F: if(*((int*)_tmp276)!= 1)goto _LL181;_tmp277=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp276)->f2;if(_tmp277 <= (void*)1)goto _LL181;if(*((int*)_tmp277)!= 0)goto _LL181;
_tmp278=((struct Cyc_Absyn_Global_b_struct*)_tmp277)->f1;_LL180: _tmp27A=_tmp278;
goto _LL182;_LL181: if(*((int*)_tmp276)!= 1)goto _LL183;_tmp279=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp276)->f2;if(_tmp279 <= (void*)1)goto _LL183;if(*((int*)_tmp279)!= 2)goto _LL183;
_tmp27A=((struct Cyc_Absyn_Param_b_struct*)_tmp279)->f1;_LL182: _tmp27C=_tmp27A;
goto _LL184;_LL183: if(*((int*)_tmp276)!= 1)goto _LL185;_tmp27B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp276)->f2;if(_tmp27B <= (void*)1)goto _LL185;if(*((int*)_tmp27B)!= 3)goto _LL185;
_tmp27C=((struct Cyc_Absyn_Local_b_struct*)_tmp27B)->f1;_LL184: _tmp27E=_tmp27C;
goto _LL186;_LL185: if(*((int*)_tmp276)!= 1)goto _LL187;_tmp27D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp276)->f2;if(_tmp27D <= (void*)1)goto _LL187;if(*((int*)_tmp27D)!= 4)goto _LL187;
_tmp27E=((struct Cyc_Absyn_Pat_b_struct*)_tmp27D)->f1;_LL186: if(!_tmp27E->escapes)
return Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp27E,e2);goto _LL17E;_LL187:;_LL188:
goto _LL17E;_LL17E:;}return r;}static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*
r){({struct Cyc_String_pa_struct _tmp281;_tmp281.tag=0;_tmp281.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((r->vd)->name));{
void*_tmp27F[1]={& _tmp281};Cyc_fprintf(Cyc_stderr,({const char*_tmp280="%s";
_tag_dyneither(_tmp280,sizeof(char),3);}),_tag_dyneither(_tmp27F,sizeof(void*),1));}});{
union Cyc_CfFlowInfo_RelnOp_union _tmp282=r->rop;unsigned int _tmp283;struct Cyc_Absyn_Vardecl*
_tmp284;struct Cyc_Absyn_Vardecl*_tmp285;unsigned int _tmp286;struct Cyc_Absyn_Vardecl*
_tmp287;_LL18A: if((_tmp282.EqualConst).tag != 0)goto _LL18C;_tmp283=(_tmp282.EqualConst).f1;
_LL18B:({struct Cyc_Int_pa_struct _tmp28A;_tmp28A.tag=1;_tmp28A.f1=(unsigned long)((
int)_tmp283);{void*_tmp288[1]={& _tmp28A};Cyc_fprintf(Cyc_stderr,({const char*
_tmp289="==%d";_tag_dyneither(_tmp289,sizeof(char),5);}),_tag_dyneither(_tmp288,
sizeof(void*),1));}});goto _LL189;_LL18C: if((_tmp282.LessVar).tag != 1)goto _LL18E;
_tmp284=(_tmp282.LessVar).f1;_LL18D:({struct Cyc_String_pa_struct _tmp28D;_tmp28D.tag=
0;_tmp28D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp284->name));{void*_tmp28B[1]={& _tmp28D};Cyc_fprintf(Cyc_stderr,({const char*
_tmp28C="<%s";_tag_dyneither(_tmp28C,sizeof(char),4);}),_tag_dyneither(_tmp28B,
sizeof(void*),1));}});goto _LL189;_LL18E: if((_tmp282.LessNumelts).tag != 2)goto
_LL190;_tmp285=(_tmp282.LessNumelts).f1;_LL18F:({struct Cyc_String_pa_struct
_tmp290;_tmp290.tag=0;_tmp290.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp285->name));{void*_tmp28E[1]={& _tmp290};Cyc_fprintf(
Cyc_stderr,({const char*_tmp28F="<numelts(%s)";_tag_dyneither(_tmp28F,sizeof(char),
13);}),_tag_dyneither(_tmp28E,sizeof(void*),1));}});goto _LL189;_LL190: if((
_tmp282.LessConst).tag != 3)goto _LL192;_tmp286=(_tmp282.LessConst).f1;_LL191:({
struct Cyc_Int_pa_struct _tmp293;_tmp293.tag=1;_tmp293.f1=(unsigned long)((int)
_tmp286);{void*_tmp291[1]={& _tmp293};Cyc_fprintf(Cyc_stderr,({const char*_tmp292="<%d";
_tag_dyneither(_tmp292,sizeof(char),4);}),_tag_dyneither(_tmp291,sizeof(void*),1));}});
goto _LL189;_LL192: if((_tmp282.LessEqNumelts).tag != 4)goto _LL189;_tmp287=(_tmp282.LessEqNumelts).f1;
_LL193:({struct Cyc_String_pa_struct _tmp296;_tmp296.tag=0;_tmp296.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp287->name));{
void*_tmp294[1]={& _tmp296};Cyc_fprintf(Cyc_stderr,({const char*_tmp295="<=numelts(%s)";
_tag_dyneither(_tmp295,sizeof(char),14);}),_tag_dyneither(_tmp294,sizeof(void*),
1));}});goto _LL189;_LL189:;}}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*
r){for(0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)r->hd);
if(r->tl != 0)({void*_tmp297=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp298=",";
_tag_dyneither(_tmp298,sizeof(char),2);}),_tag_dyneither(_tmp297,sizeof(void*),0));});}}
void Cyc_CfFlowInfo_print_initlevel(void*il){void*_tmp299=il;_LL195: if((int)
_tmp299 != 0)goto _LL197;_LL196:({void*_tmp29A=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmp29B="uninitialized";_tag_dyneither(_tmp29B,sizeof(char),14);}),
_tag_dyneither(_tmp29A,sizeof(void*),0));});goto _LL194;_LL197: if((int)_tmp299 != 
1)goto _LL199;_LL198:({void*_tmp29C=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp29D="this-initialized";
_tag_dyneither(_tmp29D,sizeof(char),17);}),_tag_dyneither(_tmp29C,sizeof(void*),
0));});goto _LL194;_LL199: if((int)_tmp299 != 2)goto _LL194;_LL19A:({void*_tmp29E=0;
Cyc_fprintf(Cyc_stderr,({const char*_tmp29F="all-initialized";_tag_dyneither(
_tmp29F,sizeof(char),16);}),_tag_dyneither(_tmp29E,sizeof(void*),0));});goto
_LL194;_LL194:;}void Cyc_CfFlowInfo_print_root(void*root){void*_tmp2A0=root;
struct Cyc_Absyn_Vardecl*_tmp2A1;struct Cyc_Absyn_Exp*_tmp2A2;void*_tmp2A3;_LL19C:
if(*((int*)_tmp2A0)!= 0)goto _LL19E;_tmp2A1=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp2A0)->f1;_LL19D:({struct Cyc_String_pa_struct _tmp2A6;_tmp2A6.tag=0;_tmp2A6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2A1->name));{
void*_tmp2A4[1]={& _tmp2A6};Cyc_fprintf(Cyc_stderr,({const char*_tmp2A5="Root(%s)";
_tag_dyneither(_tmp2A5,sizeof(char),9);}),_tag_dyneither(_tmp2A4,sizeof(void*),1));}});
goto _LL19B;_LL19E: if(*((int*)_tmp2A0)!= 1)goto _LL1A0;_tmp2A2=((struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp2A0)->f1;_tmp2A3=(void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp2A0)->f2;
_LL19F:({struct Cyc_String_pa_struct _tmp2AA;_tmp2AA.tag=0;_tmp2AA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2A3));{struct
Cyc_String_pa_struct _tmp2A9;_tmp2A9.tag=0;_tmp2A9.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp2A2));{void*_tmp2A7[2]={& _tmp2A9,&
_tmp2AA};Cyc_fprintf(Cyc_stderr,({const char*_tmp2A8="MallocPt(%s,%s)";
_tag_dyneither(_tmp2A8,sizeof(char),16);}),_tag_dyneither(_tmp2A7,sizeof(void*),
2));}}});goto _LL19B;_LL1A0: if(*((int*)_tmp2A0)!= 2)goto _LL19B;_LL1A1:({void*
_tmp2AB=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2AC="InitParam(_,_)";
_tag_dyneither(_tmp2AC,sizeof(char),15);}),_tag_dyneither(_tmp2AB,sizeof(void*),
0));});goto _LL19B;_LL19B:;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*
p){Cyc_CfFlowInfo_print_root((void*)p->root);{struct Cyc_List_List*_tmp2AD=p->fields;
while(_tmp2AD != 0){({struct Cyc_Int_pa_struct _tmp2B0;_tmp2B0.tag=1;_tmp2B0.f1=(
unsigned long)((int)_tmp2AD->hd);{void*_tmp2AE[1]={& _tmp2B0};Cyc_fprintf(Cyc_stderr,({
const char*_tmp2AF=".%d";_tag_dyneither(_tmp2AF,sizeof(char),4);}),_tag_dyneither(
_tmp2AE,sizeof(void*),1));}});_tmp2AD=_tmp2AD->tl;}({void*_tmp2B1=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp2B2=" ";_tag_dyneither(_tmp2B2,sizeof(char),2);}),
_tag_dyneither(_tmp2B1,sizeof(void*),0));});}}void Cyc_CfFlowInfo_print_place_set(
struct Cyc_Dict_Dict p){struct _RegionHandle _tmp2B3=_new_region("r");struct
_RegionHandle*r=& _tmp2B3;_push_region(r);{struct _handler_cons _tmp2B4;
_push_handler(& _tmp2B4);{int _tmp2B6=0;if(setjmp(_tmp2B4.handler))_tmp2B6=1;if(!
_tmp2B6){{struct Cyc_Iter_Iter _tmp2B7=((struct Cyc_Iter_Iter(*)(struct
_RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);struct _tuple6
_tmp2B8=*((struct _tuple6*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
r,p);int first=1;({void*_tmp2B9=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2BA="{ ";
_tag_dyneither(_tmp2BA,sizeof(char),3);}),_tag_dyneither(_tmp2B9,sizeof(void*),0));});
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple6*))Cyc_Iter_next)(_tmp2B7,&
_tmp2B8)){struct Cyc_CfFlowInfo_Place*_tmp2BB=_tmp2B8.f1;if(!first){({void*
_tmp2BC=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2BD=", ";_tag_dyneither(_tmp2BD,
sizeof(char),3);}),_tag_dyneither(_tmp2BC,sizeof(void*),0));});first=0;}Cyc_CfFlowInfo_print_place(
_tmp2BB);}({void*_tmp2BE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2BF="}\n";
_tag_dyneither(_tmp2BF,sizeof(char),3);}),_tag_dyneither(_tmp2BE,sizeof(void*),0));});};
_pop_handler();}else{void*_tmp2B5=(void*)_exn_thrown;void*_tmp2C1=_tmp2B5;_LL1A3:;
_LL1A4:({void*_tmp2C2=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2C3="{ }\n";
_tag_dyneither(_tmp2C3,sizeof(char),5);}),_tag_dyneither(_tmp2C2,sizeof(void*),0));});
_npop_handler(0);return;_LL1A5:;_LL1A6:(void)_throw(_tmp2C1);_LL1A2:;}}};
_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(void*rval){void*_tmp2C4=rval;
void*_tmp2C5;void*_tmp2C6;struct Cyc_CfFlowInfo_Place*_tmp2C7;struct
_dyneither_ptr _tmp2C8;_LL1A8: if((int)_tmp2C4 != 0)goto _LL1AA;_LL1A9:({void*
_tmp2C9=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2CA="Zero";_tag_dyneither(
_tmp2CA,sizeof(char),5);}),_tag_dyneither(_tmp2C9,sizeof(void*),0));});goto
_LL1A7;_LL1AA: if((int)_tmp2C4 != 1)goto _LL1AC;_LL1AB:({void*_tmp2CB=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp2CC="NotZeroAll";_tag_dyneither(_tmp2CC,sizeof(char),
11);}),_tag_dyneither(_tmp2CB,sizeof(void*),0));});goto _LL1A7;_LL1AC: if((int)
_tmp2C4 != 2)goto _LL1AE;_LL1AD:({void*_tmp2CD=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmp2CE="NotZeroThis";_tag_dyneither(_tmp2CE,sizeof(char),12);}),
_tag_dyneither(_tmp2CD,sizeof(void*),0));});goto _LL1A7;_LL1AE: if(_tmp2C4 <= (void*)
3)goto _LL1B0;if(*((int*)_tmp2C4)!= 0)goto _LL1B0;_tmp2C5=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp2C4)->f1;_LL1AF:({void*_tmp2CF=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2D0="Unknown(";
_tag_dyneither(_tmp2D0,sizeof(char),9);}),_tag_dyneither(_tmp2CF,sizeof(void*),0));});
Cyc_CfFlowInfo_print_initlevel(_tmp2C5);({void*_tmp2D1=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp2D2=")";_tag_dyneither(_tmp2D2,sizeof(char),2);}),_tag_dyneither(
_tmp2D1,sizeof(void*),0));});goto _LL1A7;_LL1B0: if(_tmp2C4 <= (void*)3)goto _LL1B2;
if(*((int*)_tmp2C4)!= 1)goto _LL1B2;_tmp2C6=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp2C4)->f1;_LL1B1:({void*_tmp2D3=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2D4="Esc(";
_tag_dyneither(_tmp2D4,sizeof(char),5);}),_tag_dyneither(_tmp2D3,sizeof(void*),0));});
Cyc_CfFlowInfo_print_initlevel(_tmp2C6);({void*_tmp2D5=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp2D6=")";_tag_dyneither(_tmp2D6,sizeof(char),2);}),_tag_dyneither(
_tmp2D5,sizeof(void*),0));});goto _LL1A7;_LL1B2: if(_tmp2C4 <= (void*)3)goto _LL1B4;
if(*((int*)_tmp2C4)!= 2)goto _LL1B4;_tmp2C7=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp2C4)->f1;_LL1B3:({void*_tmp2D7=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2D8="AddrOf(";
_tag_dyneither(_tmp2D8,sizeof(char),8);}),_tag_dyneither(_tmp2D7,sizeof(void*),0));});
Cyc_CfFlowInfo_print_place(_tmp2C7);({void*_tmp2D9=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp2DA=")";_tag_dyneither(_tmp2DA,sizeof(char),2);}),_tag_dyneither(
_tmp2D9,sizeof(void*),0));});goto _LL1A7;_LL1B4: if(_tmp2C4 <= (void*)3)goto _LL1B6;
if(*((int*)_tmp2C4)!= 3)goto _LL1B6;_LL1B5:({void*_tmp2DB=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp2DC="TagCmps(?)";_tag_dyneither(_tmp2DC,sizeof(char),11);}),
_tag_dyneither(_tmp2DB,sizeof(void*),0));});goto _LL1A7;_LL1B6: if(_tmp2C4 <= (void*)
3)goto _LL1A7;if(*((int*)_tmp2C4)!= 4)goto _LL1A7;_tmp2C8=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2C4)->f1;_LL1B7:({void*_tmp2DD=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2DE="Aggregate{";
_tag_dyneither(_tmp2DE,sizeof(char),11);}),_tag_dyneither(_tmp2DD,sizeof(void*),
0));});{int i=0;for(0;i < _get_dyneither_size(_tmp2C8,sizeof(void*));++ i){Cyc_CfFlowInfo_print_absrval(*((
void**)_check_dyneither_subscript(_tmp2C8,sizeof(void*),i)));if(i + 1 < 
_get_dyneither_size(_tmp2C8,sizeof(void*)))({void*_tmp2DF=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp2E0=",";_tag_dyneither(_tmp2E0,sizeof(char),2);}),_tag_dyneither(
_tmp2DF,sizeof(void*),0));});}}({void*_tmp2E1=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmp2E2="}";_tag_dyneither(_tmp2E2,sizeof(char),2);}),_tag_dyneither(
_tmp2E1,sizeof(void*),0));});goto _LL1A7;_LL1A7:;}static void Cyc_CfFlowInfo_print_flow_mapping(
void*root,void*rval){({void*_tmp2E3=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2E4="    ";
_tag_dyneither(_tmp2E4,sizeof(char),5);}),_tag_dyneither(_tmp2E3,sizeof(void*),0));});
Cyc_CfFlowInfo_print_root(root);({void*_tmp2E5=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmp2E6=" --> ";_tag_dyneither(_tmp2E6,sizeof(char),6);}),_tag_dyneither(
_tmp2E5,sizeof(void*),0));});Cyc_CfFlowInfo_print_absrval(rval);({void*_tmp2E7=0;
Cyc_fprintf(Cyc_stderr,({const char*_tmp2E8="\n";_tag_dyneither(_tmp2E8,sizeof(
char),2);}),_tag_dyneither(_tmp2E7,sizeof(void*),0));});}void Cyc_CfFlowInfo_print_flowdict(
struct Cyc_Dict_Dict d){Cyc_Dict_iter(Cyc_CfFlowInfo_print_flow_mapping,d);}void
Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo_union f){union Cyc_CfFlowInfo_FlowInfo_union
_tmp2E9=f;struct Cyc_Dict_Dict _tmp2EA;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2EB;
_LL1B9: if((_tmp2E9.BottomFL).tag != 0)goto _LL1BB;_LL1BA:({void*_tmp2EC=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp2ED="  BottomFL\n";_tag_dyneither(_tmp2ED,sizeof(char),
12);}),_tag_dyneither(_tmp2EC,sizeof(void*),0));});goto _LL1B8;_LL1BB: if((_tmp2E9.ReachableFL).tag
!= 1)goto _LL1B8;_tmp2EA=(_tmp2E9.ReachableFL).f1;_tmp2EB=(_tmp2E9.ReachableFL).f3;
_LL1BC:({void*_tmp2EE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2EF="  ReachableFL:\n";
_tag_dyneither(_tmp2EF,sizeof(char),16);}),_tag_dyneither(_tmp2EE,sizeof(void*),
0));});Cyc_Dict_iter(Cyc_CfFlowInfo_print_flow_mapping,_tmp2EA);({void*_tmp2F0=0;
Cyc_fprintf(Cyc_stderr,({const char*_tmp2F1="\n    consumed: ";_tag_dyneither(
_tmp2F1,sizeof(char),16);}),_tag_dyneither(_tmp2F0,sizeof(void*),0));});Cyc_CfFlowInfo_print_place_set(
_tmp2EB.consumed);({void*_tmp2F2=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2F3="\n    may_consume: ";
_tag_dyneither(_tmp2F3,sizeof(char),19);}),_tag_dyneither(_tmp2F2,sizeof(void*),
0));});Cyc_CfFlowInfo_print_place_list(_tmp2EB.may_consume);({void*_tmp2F4=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp2F5="\n";_tag_dyneither(_tmp2F5,sizeof(char),2);}),
_tag_dyneither(_tmp2F4,sizeof(void*),0));});goto _LL1B8;_LL1B8:;}int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t){void*_tmp2F6=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp2F7;struct Cyc_Absyn_TunionInfo _tmp2F8;struct Cyc_List_List*_tmp2F9;struct Cyc_Core_Opt*
_tmp2FA;struct Cyc_List_List*_tmp2FB;struct Cyc_Absyn_AggrInfo _tmp2FC;struct Cyc_List_List*
_tmp2FD;struct Cyc_Absyn_TunionFieldInfo _tmp2FE;struct Cyc_List_List*_tmp2FF;
struct Cyc_List_List*_tmp300;struct Cyc_Absyn_PtrInfo _tmp301;void*_tmp302;struct
Cyc_Absyn_PtrAtts _tmp303;void*_tmp304;struct Cyc_List_List*_tmp305;struct Cyc_List_List*
_tmp306;struct Cyc_Absyn_ArrayInfo _tmp307;void*_tmp308;void*_tmp309;void*_tmp30A;
void*_tmp30B;void*_tmp30C;void*_tmp30D;_LL1BE: if((int)_tmp2F6 != 0)goto _LL1C0;
_LL1BF: goto _LL1C1;_LL1C0: if(_tmp2F6 <= (void*)4)goto _LL1C2;if(*((int*)_tmp2F6)!= 
5)goto _LL1C2;_LL1C1: goto _LL1C3;_LL1C2: if((int)_tmp2F6 != 1)goto _LL1C4;_LL1C3: goto
_LL1C5;_LL1C4: if(_tmp2F6 <= (void*)4)goto _LL1CC;if(*((int*)_tmp2F6)!= 6)goto
_LL1C6;_LL1C5: goto _LL1C7;_LL1C6: if(*((int*)_tmp2F6)!= 12)goto _LL1C8;_LL1C7: goto
_LL1C9;_LL1C8: if(*((int*)_tmp2F6)!= 13)goto _LL1CA;_LL1C9: goto _LL1CB;_LL1CA: if(*((
int*)_tmp2F6)!= 18)goto _LL1CC;_LL1CB: goto _LL1CD;_LL1CC: if((int)_tmp2F6 != 2)goto
_LL1CE;_LL1CD: goto _LL1CF;_LL1CE: if(_tmp2F6 <= (void*)4)goto _LL1D0;if(*((int*)
_tmp2F6)!= 0)goto _LL1D0;_LL1CF: return 0;_LL1D0: if((int)_tmp2F6 != 3)goto _LL1D2;
_LL1D1: {void*_tmp30E=rgn;_LL1F3: if((int)_tmp30E != 0)goto _LL1F5;_LL1F4: return 1;
_LL1F5:;_LL1F6: return 0;_LL1F2:;}_LL1D2: if(_tmp2F6 <= (void*)4)goto _LL1D4;if(*((
int*)_tmp2F6)!= 1)goto _LL1D4;_tmp2F7=((struct Cyc_Absyn_VarType_struct*)_tmp2F6)->f1;
_LL1D3: {void*_tmp30F=rgn;struct Cyc_Absyn_Tvar*_tmp310;_LL1F8: if((int)_tmp30F != 
0)goto _LL1FA;_LL1F9: return 0;_LL1FA: if(_tmp30F <= (void*)1)goto _LL1F7;if(*((int*)
_tmp30F)!= 0)goto _LL1F7;_tmp310=((struct Cyc_CfFlowInfo_Region_k_struct*)_tmp30F)->f1;
_LL1FB: return Cyc_Absyn_tvar_cmp(_tmp2F7,_tmp310)== 0;_LL1F7:;}_LL1D4: if(_tmp2F6
<= (void*)4)goto _LL1D6;if(*((int*)_tmp2F6)!= 2)goto _LL1D6;_tmp2F8=((struct Cyc_Absyn_TunionType_struct*)
_tmp2F6)->f1;_tmp2F9=_tmp2F8.targs;_tmp2FA=_tmp2F8.rgn;_LL1D5: if((unsigned int)
_tmp2FA  && Cyc_CfFlowInfo_contains_region(rgn,(void*)_tmp2FA->v))return 1;_tmp2FB=
_tmp2F9;goto _LL1D7;_LL1D6: if(_tmp2F6 <= (void*)4)goto _LL1D8;if(*((int*)_tmp2F6)!= 
16)goto _LL1D8;_tmp2FB=((struct Cyc_Absyn_TypedefType_struct*)_tmp2F6)->f2;_LL1D7:
_tmp2FD=_tmp2FB;goto _LL1D9;_LL1D8: if(_tmp2F6 <= (void*)4)goto _LL1DA;if(*((int*)
_tmp2F6)!= 10)goto _LL1DA;_tmp2FC=((struct Cyc_Absyn_AggrType_struct*)_tmp2F6)->f1;
_tmp2FD=_tmp2FC.targs;_LL1D9: _tmp2FF=_tmp2FD;goto _LL1DB;_LL1DA: if(_tmp2F6 <= (
void*)4)goto _LL1DC;if(*((int*)_tmp2F6)!= 3)goto _LL1DC;_tmp2FE=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2F6)->f1;_tmp2FF=_tmp2FE.targs;_LL1DB: _tmp300=_tmp2FF;goto _LL1DD;_LL1DC: if(
_tmp2F6 <= (void*)4)goto _LL1DE;if(*((int*)_tmp2F6)!= 20)goto _LL1DE;_tmp300=((
struct Cyc_Absyn_JoinEff_struct*)_tmp2F6)->f1;_LL1DD: return Cyc_List_exists_c(Cyc_CfFlowInfo_contains_region,
rgn,_tmp300);_LL1DE: if(_tmp2F6 <= (void*)4)goto _LL1E0;if(*((int*)_tmp2F6)!= 4)
goto _LL1E0;_tmp301=((struct Cyc_Absyn_PointerType_struct*)_tmp2F6)->f1;_tmp302=(
void*)_tmp301.elt_typ;_tmp303=_tmp301.ptr_atts;_tmp304=(void*)_tmp303.rgn;_LL1DF:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp304) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp302);_LL1E0: if(_tmp2F6 <= (void*)4)goto _LL1E2;if(*((int*)_tmp2F6)!= 8)
goto _LL1E2;_LL1E1: return 0;_LL1E2: if(_tmp2F6 <= (void*)4)goto _LL1E4;if(*((int*)
_tmp2F6)!= 9)goto _LL1E4;_tmp305=((struct Cyc_Absyn_TupleType_struct*)_tmp2F6)->f1;
_LL1E3: for(0;_tmp305 != 0;_tmp305=_tmp305->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(*((struct _tuple7*)_tmp305->hd)).f2))return 1;}return 0;_LL1E4: if(_tmp2F6 <= (
void*)4)goto _LL1E6;if(*((int*)_tmp2F6)!= 11)goto _LL1E6;_tmp306=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2F6)->f2;_LL1E5: for(0;_tmp306 != 0;_tmp306=_tmp306->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp306->hd)->type))return 1;}return 0;
_LL1E6: if(_tmp2F6 <= (void*)4)goto _LL1E8;if(*((int*)_tmp2F6)!= 7)goto _LL1E8;
_tmp307=((struct Cyc_Absyn_ArrayType_struct*)_tmp2F6)->f1;_tmp308=(void*)_tmp307.elt_type;
_LL1E7: _tmp309=_tmp308;goto _LL1E9;_LL1E8: if(_tmp2F6 <= (void*)4)goto _LL1EA;if(*((
int*)_tmp2F6)!= 19)goto _LL1EA;_tmp309=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp2F6)->f1;_LL1E9: _tmp30A=_tmp309;goto _LL1EB;_LL1EA: if(_tmp2F6 <= (void*)4)goto
_LL1EC;if(*((int*)_tmp2F6)!= 21)goto _LL1EC;_tmp30A=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp2F6)->f1;_LL1EB: return Cyc_CfFlowInfo_contains_region(rgn,_tmp30A);_LL1EC: if(
_tmp2F6 <= (void*)4)goto _LL1EE;if(*((int*)_tmp2F6)!= 15)goto _LL1EE;_tmp30B=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp2F6)->f1;_tmp30C=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp2F6)->f2;_LL1ED: return Cyc_CfFlowInfo_contains_region(rgn,_tmp30B) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp30C);_LL1EE: if(_tmp2F6 <= (void*)4)goto _LL1F0;if(*((int*)_tmp2F6)!= 14)
goto _LL1F0;_tmp30D=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2F6)->f1;
_LL1EF: return 0;_LL1F0: if(_tmp2F6 <= (void*)4)goto _LL1BD;if(*((int*)_tmp2F6)!= 17)
goto _LL1BD;_LL1F1: return 0;_LL1BD:;}struct _tuple13{struct Cyc_CfFlowInfo_FlowEnv*
f1;struct Cyc_Dict_Dict f2;void*f3;};static void Cyc_CfFlowInfo_kill_root(struct
_tuple13*env,void*root,void*rval){struct _tuple13 _tmp312;struct Cyc_CfFlowInfo_FlowEnv*
_tmp313;struct Cyc_Dict_Dict _tmp314;struct Cyc_Dict_Dict*_tmp315;void*_tmp316;
struct _tuple13*_tmp311=env;_tmp312=*_tmp311;_tmp313=_tmp312.f1;_tmp314=_tmp312.f2;
_tmp315=(struct Cyc_Dict_Dict*)&(*_tmp311).f2;_tmp316=_tmp312.f3;{void*_tmp317=
root;struct Cyc_Absyn_Vardecl*_tmp318;void*_tmp319;_LL1FD: if(*((int*)_tmp317)!= 0)
goto _LL1FF;_tmp318=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp317)->f1;_LL1FE:
if(Cyc_CfFlowInfo_contains_region(_tmp316,(void*)_tmp318->type))rval=Cyc_CfFlowInfo_typ_to_absrval(
_tmp313,(void*)_tmp318->type,(void*)_tmp313->unknown_none);*_tmp315=Cyc_Dict_insert(*
_tmp315,root,rval);goto _LL1FC;_LL1FF: if(*((int*)_tmp317)!= 1)goto _LL201;_tmp319=(
void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp317)->f2;_LL200: if(!Cyc_CfFlowInfo_contains_region(
_tmp316,_tmp319))*_tmp315=Cyc_Dict_insert(*_tmp315,root,rval);goto _LL1FC;_LL201:
if(*((int*)_tmp317)!= 2)goto _LL1FC;_LL202: goto _LL1FC;_LL1FC:;}}static struct Cyc_Dict_Dict
Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict
fd,void*rgn){void*_tmp31A=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp31B;
_LL204: if(_tmp31A <= (void*)4)goto _LL206;if(*((int*)_tmp31A)!= 1)goto _LL206;
_tmp31B=((struct Cyc_Absyn_VarType_struct*)_tmp31A)->f1;_LL205: {struct Cyc_CfFlowInfo_Region_k_struct
v=({struct Cyc_CfFlowInfo_Region_k_struct _tmp31D;_tmp31D.tag=0;_tmp31D.f1=_tmp31B;
_tmp31D;});void*v2=(void*)& v;struct _tuple13 env=({struct _tuple13 _tmp31C;_tmp31C.f1=
fenv;_tmp31C.f2=fenv->mt_flowdict;_tmp31C.f3=v2;_tmp31C;});((void(*)(void(*f)(
struct _tuple13*,void*,void*),struct _tuple13*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f2;}_LL206: if((int)_tmp31A != 3)goto
_LL208;_LL207: {struct _tuple13 env=({struct _tuple13 _tmp31E;_tmp31E.f1=fenv;
_tmp31E.f2=fenv->mt_flowdict;_tmp31E.f3=(void*)0;_tmp31E;});((void(*)(void(*f)(
struct _tuple13*,void*,void*),struct _tuple13*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f2;}_LL208:;_LL209:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp31F=_cycalloc(sizeof(*_tmp31F));_tmp31F[0]=({
struct Cyc_Core_Impossible_struct _tmp320;_tmp320.tag=Cyc_Core_Impossible;_tmp320.f1=({
const char*_tmp321="kill_flowdict_region";_tag_dyneither(_tmp321,sizeof(char),21);});
_tmp320;});_tmp31F;}));_LL203:;}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn){return 0;}union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union
f,void*rgn){union Cyc_CfFlowInfo_FlowInfo_union _tmp322=f;struct Cyc_Dict_Dict
_tmp323;struct Cyc_List_List*_tmp324;struct Cyc_CfFlowInfo_ConsumeInfo _tmp325;
_LL20B: if((_tmp322.BottomFL).tag != 0)goto _LL20D;_LL20C: return f;_LL20D: if((
_tmp322.ReachableFL).tag != 1)goto _LL20A;_tmp323=(_tmp322.ReachableFL).f1;_tmp324=(
_tmp322.ReachableFL).f2;_tmp325=(_tmp322.ReachableFL).f3;_LL20E: {struct Cyc_Dict_Dict
_tmp326=Cyc_CfFlowInfo_kill_flowdict_region(fenv,_tmp323,rgn);struct Cyc_List_List*
_tmp327=Cyc_CfFlowInfo_kill_relns_region(_tmp324,rgn);return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp328;(_tmp328.ReachableFL).tag=1;(_tmp328.ReachableFL).f1=
_tmp326;(_tmp328.ReachableFL).f2=_tmp327;(_tmp328.ReachableFL).f3=_tmp325;
_tmp328;});}_LL20A:;}struct _tuple14{struct Cyc_Dict_Dict*f1;struct Cyc_Dict_Dict f2;
struct Cyc_Position_Segment*f3;};static void Cyc_CfFlowInfo_consume_f(struct
_tuple14*env,struct Cyc_CfFlowInfo_Place*place){struct _tuple14 _tmp32A;struct Cyc_Dict_Dict*
_tmp32B;struct Cyc_Dict_Dict _tmp32C;struct Cyc_Position_Segment*_tmp32D;struct
_tuple14*_tmp329=env;_tmp32A=*_tmp329;_tmp32B=_tmp32A.f1;_tmp32C=_tmp32A.f2;
_tmp32D=_tmp32A.f3;{void*rval=(void*)0;if(Cyc_Dict_lookup_bool(_tmp32C,(void*)
place->root,& rval) && rval == (void*)0)return;if(Cyc_CfFlowInfo_update_place_set((
struct Cyc_Dict_Dict*)_tmp32B,place,_tmp32D)){struct Cyc_Position_Segment*_tmp32E=((
struct Cyc_Position_Segment*(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))
Cyc_Dict_lookup)(*_tmp32B,place);Cyc_CfFlowInfo_unique_err(place,({const char*
_tmp32F="May consume non-aliased pointer %s more than once (cf. %s)";
_tag_dyneither(_tmp32F,sizeof(char),59);}),({const char*_tmp330="May consume non-aliased pointer %s more than once";
_tag_dyneither(_tmp330,sizeof(char),50);}),_tmp32E,_tmp32D);}}}union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f){union Cyc_CfFlowInfo_FlowInfo_union _tmp331=f;struct Cyc_Dict_Dict _tmp332;struct
Cyc_List_List*_tmp333;struct Cyc_CfFlowInfo_ConsumeInfo _tmp334;_LL210: if((_tmp331.BottomFL).tag
!= 0)goto _LL212;_LL211: return f;_LL212: if((_tmp331.ReachableFL).tag != 1)goto
_LL20F;_tmp332=(_tmp331.ReachableFL).f1;_tmp333=(_tmp331.ReachableFL).f2;_tmp334=(
_tmp331.ReachableFL).f3;_LL213: if(_tmp334.may_consume == 0)return f;{struct Cyc_Dict_Dict
_tmp335=_tmp334.consumed;struct _tuple14 _tmp336=({struct _tuple14 _tmp339;_tmp339.f1=&
_tmp335;_tmp339.f2=_tmp332;_tmp339.f3=loc;_tmp339;});((void(*)(void(*f)(struct
_tuple14*,struct Cyc_CfFlowInfo_Place*),struct _tuple14*env,struct Cyc_List_List*x))
Cyc_List_iter_c)(Cyc_CfFlowInfo_consume_f,& _tmp336,_tmp334.may_consume);return(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp337;(
_tmp337.ReachableFL).tag=1;(_tmp337.ReachableFL).f1=_tmp332;(_tmp337.ReachableFL).f2=
_tmp333;(_tmp337.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp338;
_tmp338.consumed=_tmp335;_tmp338.may_consume=0;_tmp338;});_tmp337;});}_LL20F:;}
void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f){union Cyc_CfFlowInfo_FlowInfo_union _tmp33A=f;struct Cyc_Dict_Dict _tmp33B;struct
Cyc_List_List*_tmp33C;struct Cyc_CfFlowInfo_ConsumeInfo _tmp33D;_LL215: if((_tmp33A.BottomFL).tag
!= 0)goto _LL217;_LL216: return;_LL217: if((_tmp33A.ReachableFL).tag != 1)goto _LL214;
_tmp33B=(_tmp33A.ReachableFL).f1;_tmp33C=(_tmp33A.ReachableFL).f2;_tmp33D=(
_tmp33A.ReachableFL).f3;_LL218: {struct Cyc_List_List*_tmp33E=_tmp33D.may_consume;
while(_tmp33E != 0){if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))
Cyc_Dict_member)(_tmp33D.consumed,(struct Cyc_CfFlowInfo_Place*)_tmp33E->hd)){
struct Cyc_Position_Segment*_tmp33F=((struct Cyc_Position_Segment*(*)(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_lookup)(_tmp33D.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp33E->hd);Cyc_CfFlowInfo_unique_err((struct Cyc_CfFlowInfo_Place*)_tmp33E->hd,({
const char*_tmp340="Read through possibly consumed unique pointer %s (cf. %s)";
_tag_dyneither(_tmp340,sizeof(char),58);}),({const char*_tmp341="Read through possibly consumed unique pointer %s";
_tag_dyneither(_tmp341,sizeof(char),49);}),_tmp33F,loc);break;}_tmp33E=_tmp33E->tl;}
return;}_LL214:;}union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_drop_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f){union Cyc_CfFlowInfo_FlowInfo_union
_tmp342=f;struct Cyc_Dict_Dict _tmp343;struct Cyc_List_List*_tmp344;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp345;_LL21A: if((_tmp342.BottomFL).tag != 0)goto _LL21C;_LL21B: return f;_LL21C:
if((_tmp342.ReachableFL).tag != 1)goto _LL219;_tmp343=(_tmp342.ReachableFL).f1;
_tmp344=(_tmp342.ReachableFL).f2;_tmp345=(_tmp342.ReachableFL).f3;_LL21D: _tmp345.may_consume=
0;return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp346;(_tmp346.ReachableFL).tag=1;(_tmp346.ReachableFL).f1=_tmp343;(_tmp346.ReachableFL).f2=
_tmp344;(_tmp346.ReachableFL).f3=_tmp345;_tmp346;});_LL219:;}union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f){Cyc_CfFlowInfo_check_unique_rvals(loc,f);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp347=f;struct Cyc_Dict_Dict _tmp348;struct Cyc_List_List*_tmp349;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp34A;_LL21F: if((_tmp347.BottomFL).tag != 0)goto _LL221;_LL220: return f;_LL221:
if((_tmp347.ReachableFL).tag != 1)goto _LL21E;_tmp348=(_tmp347.ReachableFL).f1;
_tmp349=(_tmp347.ReachableFL).f2;_tmp34A=(_tmp347.ReachableFL).f3;_LL222: _tmp34A.may_consume=
0;return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp34B;(_tmp34B.ReachableFL).tag=1;(_tmp34B.ReachableFL).f1=_tmp348;(_tmp34B.ReachableFL).f2=
_tmp349;(_tmp34B.ReachableFL).f3=_tmp34A;_tmp34B;});_LL21E:;}}struct _tuple5 Cyc_CfFlowInfo_save_consume_info(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union f,int clear){
union Cyc_CfFlowInfo_FlowInfo_union _tmp34C=f;struct Cyc_Dict_Dict _tmp34D;struct Cyc_List_List*
_tmp34E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp34F;_LL224: if((_tmp34C.BottomFL).tag
!= 0)goto _LL226;_LL225: return({struct _tuple5 _tmp350;_tmp350.f1=({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp351;_tmp351.consumed=fenv->mt_place_set;_tmp351.may_consume=0;_tmp351;});
_tmp350.f2=f;_tmp350;});_LL226: if((_tmp34C.ReachableFL).tag != 1)goto _LL223;
_tmp34D=(_tmp34C.ReachableFL).f1;_tmp34E=(_tmp34C.ReachableFL).f2;_tmp34F=(
_tmp34C.ReachableFL).f3;_LL227: if(clear)return({struct _tuple5 _tmp352;_tmp352.f1=
_tmp34F;_tmp352.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp353;(_tmp353.ReachableFL).tag=1;(_tmp353.ReachableFL).f1=_tmp34D;(_tmp353.ReachableFL).f2=
_tmp34E;(_tmp353.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp354;
_tmp354.consumed=fenv->mt_place_set;_tmp354.may_consume=0;_tmp354;});_tmp353;});
_tmp352;});else{return({struct _tuple5 _tmp355;_tmp355.f1=_tmp34F;_tmp355.f2=f;
_tmp355;});}_LL223:;}union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_restore_consume_info(
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_CfFlowInfo_ConsumeInfo c){union Cyc_CfFlowInfo_FlowInfo_union
_tmp356=f;struct Cyc_Dict_Dict _tmp357;struct Cyc_List_List*_tmp358;_LL229: if((
_tmp356.BottomFL).tag != 0)goto _LL22B;_LL22A: return f;_LL22B: if((_tmp356.ReachableFL).tag
!= 1)goto _LL228;_tmp357=(_tmp356.ReachableFL).f1;_tmp358=(_tmp356.ReachableFL).f2;
_LL22C: return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp359;(_tmp359.ReachableFL).tag=1;(_tmp359.ReachableFL).f1=_tmp357;(_tmp359.ReachableFL).f2=
_tmp358;(_tmp359.ReachableFL).f3=c;_tmp359;});_LL228:;}
