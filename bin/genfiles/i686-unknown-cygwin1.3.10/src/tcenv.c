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
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct
Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;int(*next)(void*env,
void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct
Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict
Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));int Cyc_Dict_is_empty(
struct Cyc_Dict_Dict d);int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct
_RegionHandle*,struct Cyc_Dict_Dict,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*
Cyc_Set_empty(int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rempty(struct
_RegionHandle*r,int(*cmp)(void*,void*));int Cyc_Set_member(struct Cyc_Set_Set*s,
void*elt);extern char Cyc_Set_Absent[11];struct Cyc_SlowDict_Dict;extern char Cyc_SlowDict_Present[
12];extern char Cyc_SlowDict_Absent[11];struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dyneither_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};
extern char Cyc_Position_Nocontext[14];int Cyc_strptrcmp(struct _dyneither_ptr*s1,
struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;
extern void*Cyc_Absyn_empty_effect;extern struct _tuple1*Cyc_Absyn_exn_name;extern
struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;void*Cyc_Absyn_dyneither_typ(void*t,
void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
le;int allow_valueof;};void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(struct _RegionHandle*);struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(
struct _RegionHandle*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Fenv*old_fenv,
struct Cyc_Absyn_Fndecl*new_fn);struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _dyneither_ptr*,struct
Cyc_List_List*);void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Tuniondecl***Cyc_Tcenv_lookup_xtuniondecl(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_return_typ(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct
Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_goto(struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);
struct _tuple4{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_Tcenv_CList*
f3;};struct _tuple4*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Switch_clause***);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*
loc);void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);void
Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*rgn);void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*rgn);void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void*eff);int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,
void*r1,void*r2);void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*
te);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void
Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_subset_effect(
int may_constrain_evars,void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int
constrain,void*r,void*e);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,
struct Cyc_List_List*);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);char Cyc_Tcenv_Env_error[14]="\000\000\000\000Env_error\000";
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){({struct Cyc_String_pa_struct
_tmp2;_tmp2.tag=0;_tmp2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);{
void*_tmp0[1]={& _tmp2};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="Internal error in tcenv: %s\n";
_tag_dyneither(_tmp1,sizeof(char),29);}),_tag_dyneither(_tmp0,sizeof(void*),1));}});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)Cyc_Tcenv_Env_error);}
struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;struct Cyc_Tcenv_CtrlEnv{struct
_RegionHandle*ctrl_rgn;void*continue_stmt;void*break_stmt;struct _tuple4*
fallthru_clause;void*next_stmt;int try_depth;};struct Cyc_Tcenv_SharedFenv{struct
_RegionHandle*frgn;void*return_typ;struct Cyc_Dict_Dict seen_labels;struct Cyc_Dict_Dict
needed_labels;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*
delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*
b;struct Cyc_Tcenv_Bindings*tl;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*
shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;
struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*
ctrl_env;void*capability;void*curr_rgn;int in_notreadctxt;};char Cyc_Tcenv_NoBinding[
14]="\000\000\000\000NoBinding\000";void*Cyc_Tcenv_lookup_binding(struct Cyc_Tcenv_Bindings*
bs,struct _dyneither_ptr*v){for(0;(unsigned int)bs;bs=bs->tl){if(Cyc_strptrcmp(v,
bs->v)== 0)return(void*)bs->b;}(int)_throw((void*)Cyc_Tcenv_NoBinding);}struct
Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*r){return({struct Cyc_Tcenv_Genv*
_tmp3=_region_malloc(r,sizeof(*_tmp3));_tmp3->grgn=r;_tmp3->namespaces=((struct
Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct
_dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp);_tmp3->aggrdecls=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp);_tmp3->tuniondecls=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);
_tmp3->enumdecls=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp3->typedefs=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp3->ordinaries=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp3->availables=0;
_tmp3;});}struct _tuple5{void*f1;int f2;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(
struct _RegionHandle*r){Cyc_Tcutil_empty_var_set=({struct Cyc_Core_Opt*_tmp4=
_cycalloc(sizeof(*_tmp4));_tmp4->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp4;});{
struct Cyc_Tcenv_Genv*_tmp5=Cyc_Tcenv_empty_genv(r);_tmp5->tuniondecls=((struct
Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Tuniondecl**
v))Cyc_Dict_insert)(_tmp5->tuniondecls,(*Cyc_Absyn_exn_name).f2,({struct Cyc_Absyn_Tuniondecl**
_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=Cyc_Absyn_exn_tud;_tmp6;}));{struct Cyc_List_List*
_tmp7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;
for(0;_tmp7 != 0;_tmp7=_tmp7->tl){_tmp5->ordinaries=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple5*v))Cyc_Dict_insert)(
_tmp5->ordinaries,(*((struct Cyc_Absyn_Tunionfield*)_tmp7->hd)->name).f2,({struct
_tuple5*_tmp8=_region_malloc(r,sizeof(*_tmp8));_tmp8->f1=(void*)({struct Cyc_Tcenv_TunionRes_struct*
_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9[0]=({struct Cyc_Tcenv_TunionRes_struct _tmpA;
_tmpA.tag=2;_tmpA.f1=Cyc_Absyn_exn_tud;_tmpA.f2=(struct Cyc_Absyn_Tunionfield*)
_tmp7->hd;_tmpA;});_tmp9;});_tmp8->f2=1;_tmp8;}));}}{struct Cyc_Dict_Dict ae=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),
0,_tmp5);return({struct Cyc_Tcenv_Tenv*_tmpB=_region_malloc(r,sizeof(*_tmpB));
_tmpB->ns=0;_tmpB->ae=ae;_tmpB->le=0;_tmpB->allow_valueof=0;_tmpB;});}}}static
struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){return((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,Cyc_Tcenv_resolve_namespace(te,loc,n,ns));}static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(
struct Cyc_List_List*ns){if(ns == 0)return((struct Cyc_List_List*(*)(struct
_dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmpC="outer_namespace";
_tag_dyneither(_tmpC,sizeof(char),16);}));return((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}static int Cyc_Tcenv_same_namespace(
struct Cyc_List_List*n1,struct Cyc_List_List*n2){if(n1 == n2)return 1;for(0;n1 != 0;
n1=n1->tl){if(n2 == 0)return 0;if(Cyc_strptrcmp((struct _dyneither_ptr*)n1->hd,(
struct _dyneither_ptr*)n2->hd)!= 0)return 0;n2=n2->tl;}if(n2 != 0)return 0;return 1;}
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct
_dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss){for(0;nss != 0;
nss=nss->tl){if(!Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)nss->hd)){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)nss->hd);struct
_handler_cons _tmpD;_push_handler(& _tmpD);{int _tmpF=0;if(setjmp(_tmpD.handler))
_tmpF=1;if(!_tmpF){lookup(ge2,v);({struct Cyc_String_pa_struct _tmp12;_tmp12.tag=0;
_tmp12.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);{void*_tmp10[1]={&
_tmp12};Cyc_Tcutil_terr(loc,({const char*_tmp11="%s is ambiguous";_tag_dyneither(
_tmp11,sizeof(char),16);}),_tag_dyneither(_tmp10,sizeof(void*),1));}});;
_pop_handler();}else{void*_tmpE=(void*)_exn_thrown;void*_tmp14=_tmpE;_LL1: if(
_tmp14 != Cyc_Dict_Absent)goto _LL3;_LL2: goto _LL0;_LL3:;_LL4:(void)_throw(_tmp14);
_LL0:;}}}}return;}static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v){struct Cyc_List_List*cns=te->ns;while(1){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);{struct _handler_cons _tmp15;
_push_handler(& _tmp15);{int _tmp17=0;if(setjmp(_tmp15.handler))_tmp17=1;if(!
_tmp17){{void*result=lookup(ge,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp18=result;_npop_handler(0);return _tmp18;}};_pop_handler();}else{void*
_tmp16=(void*)_exn_thrown;void*_tmp1A=_tmp16;_LL6: if(_tmp1A != Cyc_Dict_Absent)
goto _LL8;_LL7: goto _LL5;_LL8:;_LL9:(void)_throw(_tmp1A);_LL5:;}}}{struct Cyc_List_List*
_tmp1B=ge->availables;for(0;_tmp1B != 0;_tmp1B=_tmp1B->tl){struct Cyc_Tcenv_Genv*
ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,(struct Cyc_List_List*)_tmp1B->hd);struct _handler_cons _tmp1C;_push_handler(&
_tmp1C);{int _tmp1E=0;if(setjmp(_tmp1C.handler))_tmp1E=1;if(!_tmp1E){{void*result=
lookup(ge2,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)
_tmp1B->hd,_tmp1B->tl);{void*_tmp1F=result;_npop_handler(0);return _tmp1F;}};
_pop_handler();}else{void*_tmp1D=(void*)_exn_thrown;void*_tmp21=_tmp1D;_LLB: if(
_tmp21 != Cyc_Dict_Absent)goto _LLD;_LLC: goto _LLA;_LLD:;_LLE:(void)_throw(_tmp21);
_LLA:;}}}}if(cns == 0)(int)_throw((void*)Cyc_Dict_Absent);cns=Cyc_Tcenv_outer_namespace(
cns);}}static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,
struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp22=ge->ordinaries;struct _tuple5*
ans=((struct _tuple5*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp22,v);(*ans).f2=1;return(*ans).f1;}static void*Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp24;union Cyc_Absyn_Nmspace_union _tmp25;struct _dyneither_ptr*_tmp26;
struct _tuple1*_tmp23=q;_tmp24=*_tmp23;_tmp25=_tmp24.f1;_tmp26=_tmp24.f2;{union
Cyc_Absyn_Nmspace_union _tmp27=_tmp25;struct Cyc_List_List*_tmp28;struct Cyc_List_List*
_tmp29;struct Cyc_List_List _tmp2A;struct _dyneither_ptr*_tmp2B;struct Cyc_List_List*
_tmp2C;struct Cyc_List_List*_tmp2D;_LL10: if((_tmp27.Loc_n).tag != 0)goto _LL12;
_LL11: goto _LL13;_LL12: if((_tmp27.Rel_n).tag != 1)goto _LL14;_tmp28=(_tmp27.Rel_n).f1;
if(_tmp28 != 0)goto _LL14;_LL13: return Cyc_Tcenv_scoped_lookup(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,
_tmp26);_LL14: if((_tmp27.Rel_n).tag != 1)goto _LL16;_tmp29=(_tmp27.Rel_n).f1;if(
_tmp29 == 0)goto _LL16;_tmp2A=*_tmp29;_tmp2B=(struct _dyneither_ptr*)_tmp2A.hd;
_tmp2C=_tmp2A.tl;_LL15: {struct Cyc_Tcenv_Genv*_tmp2E=Cyc_Tcenv_lookup_namespace(
te,loc,_tmp2B,_tmp2C);return Cyc_Tcenv_lookup_ordinary_global_f(_tmp2E,_tmp26);}
_LL16: if((_tmp27.Abs_n).tag != 2)goto _LLF;_tmp2D=(_tmp27.Abs_n).f1;_LL17: return
Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp2D),_tmp26);_LLF:;}}struct
Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){struct Cyc_List_List*_tmp2F=te->ns;
struct _RegionHandle _tmp30=_new_region("temp");struct _RegionHandle*temp=& _tmp30;
_push_region(temp);{struct Cyc_List_List*_tmp31=0;while(1){struct Cyc_Tcenv_Genv*
_tmp32=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,_tmp2F);struct Cyc_List_List*_tmp33=_tmp32->availables;
struct Cyc_Set_Set*_tmp34=_tmp32->namespaces;{struct Cyc_List_List*_tmp35=_tmp33;
for(0;_tmp35 != 0;_tmp35=_tmp35->tl){struct Cyc_Set_Set*_tmp36=(((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)
_tmp35->hd))->namespaces;if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_member)(_tmp36,n))_tmp31=({struct Cyc_List_List*_tmp37=
_region_malloc(temp,sizeof(*_tmp37));_tmp37->hd=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2F,({struct Cyc_List_List*
_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->hd=n;_tmp38->tl=ns;_tmp38;}));_tmp37->tl=
_tmp31;_tmp37;});}}if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
_tmp34,n))_tmp31=({struct Cyc_List_List*_tmp39=_region_malloc(temp,sizeof(*_tmp39));
_tmp39->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(_tmp2F,({struct Cyc_List_List*_tmp3A=_cycalloc(sizeof(*_tmp3A));
_tmp3A->hd=n;_tmp3A->tl=ns;_tmp3A;}));_tmp39->tl=_tmp31;_tmp39;});if(_tmp31 != 0){
if(_tmp31->tl != 0)({struct Cyc_String_pa_struct _tmp3D;_tmp3D.tag=0;_tmp3D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*n);{void*_tmp3B[1]={& _tmp3D};Cyc_Tcutil_terr(
loc,({const char*_tmp3C="%s is ambiguous";_tag_dyneither(_tmp3C,sizeof(char),16);}),
_tag_dyneither(_tmp3B,sizeof(void*),1));}});{struct Cyc_List_List*_tmp3E=(struct
Cyc_List_List*)_tmp31->hd;_npop_handler(0);return _tmp3E;}}if(_tmp2F == 0)(int)
_throw((void*)Cyc_Dict_Absent);_tmp2F=Cyc_Tcenv_outer_namespace(_tmp2F);}};
_pop_region(temp);}static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp3F=ge->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*
k))Cyc_Dict_lookup)(_tmp3F,v);}struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp41;union Cyc_Absyn_Nmspace_union _tmp42;struct _dyneither_ptr*_tmp43;
struct _tuple1*_tmp40=q;_tmp41=*_tmp40;_tmp42=_tmp41.f1;_tmp43=_tmp41.f2;{union
Cyc_Absyn_Nmspace_union _tmp44=_tmp42;struct Cyc_List_List*_tmp45;struct Cyc_List_List*
_tmp46;struct Cyc_List_List*_tmp47;struct Cyc_List_List _tmp48;struct _dyneither_ptr*
_tmp49;struct Cyc_List_List*_tmp4A;_LL19: if((_tmp44.Loc_n).tag != 0)goto _LL1B;
_LL1A: goto _LL1C;_LL1B: if((_tmp44.Rel_n).tag != 1)goto _LL1D;_tmp45=(_tmp44.Rel_n).f1;
if(_tmp45 != 0)goto _LL1D;_LL1C: return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,
Cyc_Tcenv_lookup_aggrdecl_f,_tmp43);_LL1D: if((_tmp44.Abs_n).tag != 2)goto _LL1F;
_tmp46=(_tmp44.Abs_n).f1;_LL1E: {struct Cyc_Dict_Dict _tmp4B=(((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp46))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*
k))Cyc_Dict_lookup)(_tmp4B,_tmp43);}_LL1F: if((_tmp44.Rel_n).tag != 1)goto _LL18;
_tmp47=(_tmp44.Rel_n).f1;if(_tmp47 == 0)goto _LL18;_tmp48=*_tmp47;_tmp49=(struct
_dyneither_ptr*)_tmp48.hd;_tmp4A=_tmp48.tl;_LL20: {struct Cyc_Dict_Dict _tmp4C=(
Cyc_Tcenv_lookup_namespace(te,loc,_tmp49,_tmp4A))->aggrdecls;return((struct Cyc_Absyn_Aggrdecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4C,_tmp43);}
_LL18:;}}static struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl_f(struct
Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp4D=ge->tuniondecls;
return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4D,v);}struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp4F;union Cyc_Absyn_Nmspace_union _tmp50;struct _dyneither_ptr*_tmp51;
struct _tuple1*_tmp4E=q;_tmp4F=*_tmp4E;_tmp50=_tmp4F.f1;_tmp51=_tmp4F.f2;{union
Cyc_Absyn_Nmspace_union _tmp52=_tmp50;struct Cyc_List_List*_tmp53;struct Cyc_List_List*
_tmp54;struct Cyc_List_List _tmp55;struct _dyneither_ptr*_tmp56;struct Cyc_List_List*
_tmp57;struct Cyc_List_List*_tmp58;_LL22: if((_tmp52.Loc_n).tag != 0)goto _LL24;
_LL23: goto _LL25;_LL24: if((_tmp52.Rel_n).tag != 1)goto _LL26;_tmp53=(_tmp52.Rel_n).f1;
if(_tmp53 != 0)goto _LL26;_LL25: return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Tuniondecl**(*lookup)(struct
Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_tuniondecl_f,_tmp51);_LL26: if((_tmp52.Rel_n).tag != 1)
goto _LL28;_tmp54=(_tmp52.Rel_n).f1;if(_tmp54 == 0)goto _LL28;_tmp55=*_tmp54;_tmp56=(
struct _dyneither_ptr*)_tmp55.hd;_tmp57=_tmp55.tl;_LL27: {struct Cyc_Dict_Dict
_tmp59=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp56,_tmp57))->tuniondecls;return((
struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp59,_tmp51);}_LL28: if((_tmp52.Abs_n).tag != 2)goto _LL21;_tmp58=(_tmp52.Abs_n).f1;
_LL29: {struct Cyc_Dict_Dict _tmp5A=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp58))->tuniondecls;return((
struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp5A,_tmp51);}_LL21:;}}static struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_xtuniondecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){return((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->tuniondecls,v);}
struct Cyc_Absyn_Tuniondecl***Cyc_Tcenv_lookup_xtuniondecl(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp5C;union Cyc_Absyn_Nmspace_union _tmp5D;struct _dyneither_ptr*_tmp5E;
struct _tuple1*_tmp5B=q;_tmp5C=*_tmp5B;_tmp5D=_tmp5C.f1;_tmp5E=_tmp5C.f2;{union
Cyc_Absyn_Nmspace_union _tmp5F=_tmp5D;struct Cyc_List_List*_tmp60;struct Cyc_List_List*
_tmp61;struct Cyc_List_List _tmp62;struct _dyneither_ptr*_tmp63;struct Cyc_List_List*
_tmp64;struct Cyc_List_List*_tmp65;_LL2B: if((_tmp5F.Rel_n).tag != 1)goto _LL2D;
_tmp60=(_tmp5F.Rel_n).f1;if(_tmp60 != 0)goto _LL2D;_LL2C: {struct _handler_cons
_tmp66;_push_handler(& _tmp66);{int _tmp68=0;if(setjmp(_tmp66.handler))_tmp68=1;
if(!_tmp68){{struct Cyc_Absyn_Tuniondecl***_tmp6A=({struct Cyc_Absyn_Tuniondecl***
_tmp69=_region_malloc(r,sizeof(*_tmp69));_tmp69[0]=((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Tuniondecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_xtuniondecl_f,_tmp5E);_tmp69;});_npop_handler(0);return
_tmp6A;};_pop_handler();}else{void*_tmp67=(void*)_exn_thrown;void*_tmp6C=_tmp67;
_LL34: if(_tmp6C != Cyc_Dict_Absent)goto _LL36;_LL35: return 0;_LL36:;_LL37:(void)
_throw(_tmp6C);_LL33:;}}}_LL2D: if((_tmp5F.Loc_n).tag != 0)goto _LL2F;_LL2E:({void*
_tmp6D=0;Cyc_Tcutil_terr(loc,({const char*_tmp6E="lookup_xtuniondecl: impossible";
_tag_dyneither(_tmp6E,sizeof(char),31);}),_tag_dyneither(_tmp6D,sizeof(void*),0));});
return 0;_LL2F: if((_tmp5F.Rel_n).tag != 1)goto _LL31;_tmp61=(_tmp5F.Rel_n).f1;if(
_tmp61 == 0)goto _LL31;_tmp62=*_tmp61;_tmp63=(struct _dyneither_ptr*)_tmp62.hd;
_tmp64=_tmp62.tl;_LL30: {struct Cyc_Tcenv_Genv*ge;{struct _handler_cons _tmp6F;
_push_handler(& _tmp6F);{int _tmp71=0;if(setjmp(_tmp6F.handler))_tmp71=1;if(!
_tmp71){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp63,_tmp64);;_pop_handler();}
else{void*_tmp70=(void*)_exn_thrown;void*_tmp73=_tmp70;_LL39: if(_tmp73 != Cyc_Dict_Absent)
goto _LL3B;_LL3A:({void*_tmp74=0;Cyc_Tcutil_terr(loc,({const char*_tmp75="bad qualified name for xtunion";
_tag_dyneither(_tmp75,sizeof(char),31);}),_tag_dyneither(_tmp74,sizeof(void*),0));});(
int)_throw((void*)Cyc_Dict_Absent);_LL3B:;_LL3C:(void)_throw(_tmp73);_LL38:;}}}{
struct Cyc_Dict_Dict _tmp76=ge->tuniondecls;return({struct Cyc_Absyn_Tuniondecl***
_tmp77=_region_malloc(r,sizeof(*_tmp77));_tmp77[0]=((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp76,_tmp5E);
_tmp77;});}}_LL31: if((_tmp5F.Abs_n).tag != 2)goto _LL2A;_tmp65=(_tmp5F.Abs_n).f1;
_LL32: {struct Cyc_Dict_Dict _tmp78=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp65))->tuniondecls;return({
struct Cyc_Absyn_Tuniondecl***_tmp79=_region_malloc(r,sizeof(*_tmp79));_tmp79[0]=((
struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp78,_tmp5E);_tmp79;});}_LL2A:;}}static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp7A=ge->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*
k))Cyc_Dict_lookup)(_tmp7A,v);}struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp7C;union Cyc_Absyn_Nmspace_union _tmp7D;struct _dyneither_ptr*_tmp7E;
struct _tuple1*_tmp7B=q;_tmp7C=*_tmp7B;_tmp7D=_tmp7C.f1;_tmp7E=_tmp7C.f2;{union
Cyc_Absyn_Nmspace_union _tmp7F=_tmp7D;struct Cyc_List_List*_tmp80;struct Cyc_List_List*
_tmp81;struct Cyc_List_List _tmp82;struct _dyneither_ptr*_tmp83;struct Cyc_List_List*
_tmp84;struct Cyc_List_List*_tmp85;_LL3E: if((_tmp7F.Loc_n).tag != 0)goto _LL40;
_LL3F: goto _LL41;_LL40: if((_tmp7F.Rel_n).tag != 1)goto _LL42;_tmp80=(_tmp7F.Rel_n).f1;
if(_tmp80 != 0)goto _LL42;_LL41: return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,
Cyc_Tcenv_lookup_enumdecl_f,_tmp7E);_LL42: if((_tmp7F.Rel_n).tag != 1)goto _LL44;
_tmp81=(_tmp7F.Rel_n).f1;if(_tmp81 == 0)goto _LL44;_tmp82=*_tmp81;_tmp83=(struct
_dyneither_ptr*)_tmp82.hd;_tmp84=_tmp82.tl;_LL43: {struct Cyc_Dict_Dict _tmp86=(
Cyc_Tcenv_lookup_namespace(te,loc,_tmp83,_tmp84))->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp86,_tmp7E);}
_LL44: if((_tmp7F.Abs_n).tag != 2)goto _LL3D;_tmp85=(_tmp7F.Abs_n).f1;_LL45: {
struct Cyc_Dict_Dict _tmp87=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp85))->enumdecls;return((
struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp87,_tmp7E);}_LL3D:;}}static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp88=ge->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(_tmp88,v);}struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp8A;union Cyc_Absyn_Nmspace_union _tmp8B;struct _dyneither_ptr*_tmp8C;
struct _tuple1*_tmp89=q;_tmp8A=*_tmp89;_tmp8B=_tmp8A.f1;_tmp8C=_tmp8A.f2;{union
Cyc_Absyn_Nmspace_union _tmp8D=_tmp8B;struct Cyc_List_List*_tmp8E;struct Cyc_List_List*
_tmp8F;struct Cyc_List_List _tmp90;struct _dyneither_ptr*_tmp91;struct Cyc_List_List*
_tmp92;struct Cyc_List_List*_tmp93;_LL47: if((_tmp8D.Loc_n).tag != 0)goto _LL49;
_LL48: goto _LL4A;_LL49: if((_tmp8D.Rel_n).tag != 1)goto _LL4B;_tmp8E=(_tmp8D.Rel_n).f1;
if(_tmp8E != 0)goto _LL4B;_LL4A: return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct
Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp8C);_LL4B: if((_tmp8D.Rel_n).tag != 1)
goto _LL4D;_tmp8F=(_tmp8D.Rel_n).f1;if(_tmp8F == 0)goto _LL4D;_tmp90=*_tmp8F;_tmp91=(
struct _dyneither_ptr*)_tmp90.hd;_tmp92=_tmp90.tl;_LL4C: {struct Cyc_Dict_Dict
_tmp94=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp91,_tmp92))->typedefs;return((
struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp94,_tmp8C);}_LL4D: if((_tmp8D.Abs_n).tag != 2)goto _LL46;_tmp93=(_tmp8D.Abs_n).f1;
_LL4E: {struct Cyc_Dict_Dict _tmp95=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp93))->typedefs;return((
struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp95,_tmp8C);}_LL46:;}}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*
te,struct _dyneither_ptr err_msg){struct Cyc_Tcenv_Fenv*_tmp96=te->le;if((struct Cyc_Tcenv_Tenv*)
te == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);return(
struct Cyc_Tcenv_Fenv*)_check_null(_tmp96);}static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(
struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){if(te->le
== 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp97="put_fenv";
_tag_dyneither(_tmp97,sizeof(char),9);}));return({struct Cyc_Tcenv_Tenv*_tmp98=
_region_malloc(l,sizeof(*_tmp98));_tmp98->ns=te->ns;_tmp98->ae=te->ae;_tmp98->le=(
struct Cyc_Tcenv_Fenv*)fe;_tmp98->allow_valueof=te->allow_valueof;_tmp98;});}
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct
Cyc_Tcenv_Tenv*te){return({struct Cyc_Tcenv_Tenv*_tmp99=_region_malloc(l,sizeof(*
_tmp99));_tmp99->ns=te->ns;_tmp99->ae=te->ae;_tmp99->le=0;_tmp99->allow_valueof=
te->allow_valueof;_tmp99;});}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmp9B;
struct Cyc_List_List*_tmp9C;struct Cyc_RgnOrder_RgnPO*_tmp9D;struct Cyc_Tcenv_Bindings*
_tmp9E;struct Cyc_Absyn_Stmt*_tmp9F;struct Cyc_Tcenv_CtrlEnv*_tmpA0;void*_tmpA1;
void*_tmpA2;int _tmpA3;struct Cyc_Tcenv_Fenv _tmp9A=*f;_tmp9B=_tmp9A.shared;_tmp9C=
_tmp9A.type_vars;_tmp9D=_tmp9A.region_order;_tmp9E=_tmp9A.locals;_tmp9F=_tmp9A.encloser;
_tmpA0=_tmp9A.ctrl_env;_tmpA1=(void*)_tmp9A.capability;_tmpA2=(void*)_tmp9A.curr_rgn;
_tmpA3=_tmp9A.in_notreadctxt;return({struct Cyc_Tcenv_Fenv*_tmpA4=_region_malloc(
l,sizeof(*_tmpA4));_tmpA4->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp9B;_tmpA4->type_vars=(
struct Cyc_List_List*)_tmp9C;_tmpA4->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp9D;_tmpA4->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp9E);_tmpA4->encloser=(struct Cyc_Absyn_Stmt*)_tmp9F;_tmpA4->ctrl_env=(struct
Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmpA0);_tmpA4->capability=(void*)((
void*)_tmpA1);_tmpA4->curr_rgn=(void*)((void*)_tmpA2);_tmpA4->in_notreadctxt=(
int)_tmpA3;_tmpA4;});}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmpA6;
struct Cyc_List_List*_tmpA7;struct Cyc_RgnOrder_RgnPO*_tmpA8;struct Cyc_Tcenv_Bindings*
_tmpA9;struct Cyc_Absyn_Stmt*_tmpAA;struct Cyc_Tcenv_CtrlEnv*_tmpAB;void*_tmpAC;
void*_tmpAD;int _tmpAE;struct Cyc_Tcenv_Fenv _tmpA5=*f;_tmpA6=_tmpA5.shared;_tmpA7=
_tmpA5.type_vars;_tmpA8=_tmpA5.region_order;_tmpA9=_tmpA5.locals;_tmpAA=_tmpA5.encloser;
_tmpAB=_tmpA5.ctrl_env;_tmpAC=(void*)_tmpA5.capability;_tmpAD=(void*)_tmpA5.curr_rgn;
_tmpAE=_tmpA5.in_notreadctxt;{struct _RegionHandle*_tmpB0;void*_tmpB1;void*_tmpB2;
struct _tuple4*_tmpB3;void*_tmpB4;int _tmpB5;struct Cyc_Tcenv_CtrlEnv _tmpAF=*_tmpAB;
_tmpB0=_tmpAF.ctrl_rgn;_tmpB1=(void*)_tmpAF.continue_stmt;_tmpB2=(void*)_tmpAF.break_stmt;
_tmpB3=_tmpAF.fallthru_clause;_tmpB4=(void*)_tmpAF.next_stmt;_tmpB5=_tmpAF.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmpB6=({struct Cyc_Tcenv_CtrlEnv*_tmpB8=_region_malloc(l,
sizeof(*_tmpB8));_tmpB8->ctrl_rgn=_tmpB0;_tmpB8->continue_stmt=(void*)_tmpB1;
_tmpB8->break_stmt=(void*)_tmpB2;_tmpB8->fallthru_clause=_tmpB3;_tmpB8->next_stmt=(
void*)_tmpB4;_tmpB8->try_depth=_tmpB5;_tmpB8;});return({struct Cyc_Tcenv_Fenv*
_tmpB7=_region_malloc(l,sizeof(*_tmpB7));_tmpB7->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpA6;_tmpB7->type_vars=(struct Cyc_List_List*)_tmpA7;_tmpB7->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpA8;_tmpB7->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmpA9);_tmpB7->encloser=(struct Cyc_Absyn_Stmt*)_tmpAA;
_tmpB7->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpB6;_tmpB7->capability=(void*)((
void*)_tmpAC);_tmpB7->curr_rgn=(void*)((void*)_tmpAD);_tmpB7->in_notreadctxt=(
int)_tmpAE;_tmpB7;});}}}void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv _tmpBB;struct Cyc_Tcenv_SharedFenv*_tmpBC;struct Cyc_Tcenv_Fenv*
_tmpBA=Cyc_Tcenv_get_fenv(te,({const char*_tmpB9="return_typ";_tag_dyneither(
_tmpB9,sizeof(char),11);}));_tmpBB=*_tmpBA;_tmpBC=_tmpBB.shared;{void*_tmpBE;
struct Cyc_Tcenv_SharedFenv _tmpBD=*_tmpBC;_tmpBE=(void*)_tmpBD.return_typ;return
_tmpBE;}}struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpBF=te->le;if(te->le == 0)return 0;{struct Cyc_List_List*
_tmpC1;struct Cyc_Tcenv_Fenv _tmpC0=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpBF));
_tmpC1=_tmpC0.type_vars;return _tmpC1;}}struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te){return({struct Cyc_Core_Opt*_tmpC2=_cycalloc(sizeof(*
_tmpC2));_tmpC2->v=Cyc_Tcenv_lookup_type_vars(te);_tmpC2;});}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){struct Cyc_Tcenv_Fenv _tmpC5;
struct Cyc_Tcenv_SharedFenv*_tmpC6;struct Cyc_List_List*_tmpC7;struct Cyc_RgnOrder_RgnPO*
_tmpC8;struct Cyc_Tcenv_Bindings*_tmpC9;struct Cyc_Absyn_Stmt*_tmpCA;struct Cyc_Tcenv_CtrlEnv*
_tmpCB;void*_tmpCC;void*_tmpCD;int _tmpCE;struct Cyc_Tcenv_Fenv*_tmpC4=Cyc_Tcenv_get_fenv(
te,({const char*_tmpC3="add_type_vars";_tag_dyneither(_tmpC3,sizeof(char),14);}));
_tmpC5=*_tmpC4;_tmpC6=_tmpC5.shared;_tmpC7=_tmpC5.type_vars;_tmpC8=_tmpC5.region_order;
_tmpC9=_tmpC5.locals;_tmpCA=_tmpC5.encloser;_tmpCB=_tmpC5.ctrl_env;_tmpCC=(void*)
_tmpC5.capability;_tmpCD=(void*)_tmpC5.curr_rgn;_tmpCE=_tmpC5.in_notreadctxt;Cyc_Tcutil_add_tvar_identities(
tvs);{struct Cyc_List_List*_tmpCF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpC7);Cyc_Tcutil_check_unique_tvars(
loc,_tmpCF);{struct Cyc_Tcenv_Fenv*_tmpD0=({struct Cyc_Tcenv_Fenv*_tmpD1=
_region_malloc(r,sizeof(*_tmpD1));_tmpD1->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpC6;_tmpD1->type_vars=(struct Cyc_List_List*)_tmpCF;_tmpD1->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpC8;_tmpD1->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmpC9);_tmpD1->encloser=(struct Cyc_Absyn_Stmt*)_tmpCA;
_tmpD1->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmpCB);
_tmpD1->capability=(void*)((void*)_tmpCC);_tmpD1->curr_rgn=(void*)((void*)_tmpCD);
_tmpD1->in_notreadctxt=(int)_tmpCE;_tmpD1;});return Cyc_Tcenv_put_fenv(r,te,
_tmpD0);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct
Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpD2=te->le;if(_tmpD2 == 0)return Cyc_Tcenv_put_emptyfenv(
r,te);else{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,(struct Cyc_Tcenv_Fenv*)
_tmpD2);return Cyc_Tcenv_put_fenv(r,te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Vardecl*vd){struct _dyneither_ptr*_tmpD3=(*vd->name).f2;{union Cyc_Absyn_Nmspace_union
_tmpD4=(*vd->name).f1;_LL50: if((_tmpD4.Loc_n).tag != 0)goto _LL52;_LL51: goto _LL4F;
_LL52:;_LL53:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpD5=
_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Core_Impossible_struct _tmpD6;
_tmpD6.tag=Cyc_Core_Impossible;_tmpD6.f1=({const char*_tmpD7="add_local_var: called with Rel_n";
_tag_dyneither(_tmpD7,sizeof(char),33);});_tmpD6;});_tmpD5;}));_LL4F:;}{struct
Cyc_Tcenv_Fenv _tmpDA;struct Cyc_Tcenv_SharedFenv*_tmpDB;struct Cyc_List_List*
_tmpDC;struct Cyc_RgnOrder_RgnPO*_tmpDD;struct Cyc_Tcenv_Bindings*_tmpDE;struct Cyc_Absyn_Stmt*
_tmpDF;struct Cyc_Tcenv_CtrlEnv*_tmpE0;void*_tmpE1;void*_tmpE2;int _tmpE3;struct
Cyc_Tcenv_Fenv*_tmpD9=Cyc_Tcenv_get_fenv(te,({const char*_tmpD8="add_local_var";
_tag_dyneither(_tmpD8,sizeof(char),14);}));_tmpDA=*_tmpD9;_tmpDB=_tmpDA.shared;
_tmpDC=_tmpDA.type_vars;_tmpDD=_tmpDA.region_order;_tmpDE=_tmpDA.locals;_tmpDF=
_tmpDA.encloser;_tmpE0=_tmpDA.ctrl_env;_tmpE1=(void*)_tmpDA.capability;_tmpE2=(
void*)_tmpDA.curr_rgn;_tmpE3=_tmpDA.in_notreadctxt;{struct Cyc_Tcenv_Bindings*
_tmpE4=({struct Cyc_Tcenv_Bindings*_tmpE7=_region_malloc(r,sizeof(*_tmpE7));
_tmpE7->v=_tmpD3;_tmpE7->b=(void*)((void*)({struct Cyc_Absyn_Local_b_struct*
_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8[0]=({struct Cyc_Absyn_Local_b_struct
_tmpE9;_tmpE9.tag=3;_tmpE9.f1=vd;_tmpE9;});_tmpE8;}));_tmpE7->tl=(struct Cyc_Tcenv_Bindings*)
_tmpDE;_tmpE7;});struct Cyc_Tcenv_Fenv*_tmpE5=({struct Cyc_Tcenv_Fenv*_tmpE6=
_region_malloc(r,sizeof(*_tmpE6));_tmpE6->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpDB;_tmpE6->type_vars=(struct Cyc_List_List*)_tmpDC;_tmpE6->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpDD;_tmpE6->locals=(struct Cyc_Tcenv_Bindings*)
_tmpE4;_tmpE6->encloser=(struct Cyc_Absyn_Stmt*)_tmpDF;_tmpE6->ctrl_env=(struct
Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmpE0);_tmpE6->capability=(void*)((
void*)_tmpE1);_tmpE6->curr_rgn=(void*)((void*)_tmpE2);_tmpE6->in_notreadctxt=(
int)_tmpE3;_tmpE6;});return Cyc_Tcenv_put_fenv(r,te,_tmpE5);}}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpEA=te->le;if(_tmpEA == 0)return Cyc_Tcenv_put_emptyfenv(r,
te);{struct Cyc_Tcenv_SharedFenv*_tmpEC;struct Cyc_List_List*_tmpED;struct Cyc_RgnOrder_RgnPO*
_tmpEE;struct Cyc_Tcenv_Bindings*_tmpEF;struct Cyc_Absyn_Stmt*_tmpF0;struct Cyc_Tcenv_CtrlEnv*
_tmpF1;void*_tmpF2;void*_tmpF3;int _tmpF4;struct Cyc_Tcenv_Fenv _tmpEB=*_tmpEA;
_tmpEC=_tmpEB.shared;_tmpED=_tmpEB.type_vars;_tmpEE=_tmpEB.region_order;_tmpEF=
_tmpEB.locals;_tmpF0=_tmpEB.encloser;_tmpF1=_tmpEB.ctrl_env;_tmpF2=(void*)_tmpEB.capability;
_tmpF3=(void*)_tmpEB.curr_rgn;_tmpF4=_tmpEB.in_notreadctxt;{struct Cyc_Tcenv_Fenv*
_tmpF5=({struct Cyc_Tcenv_Fenv*_tmpF6=_region_malloc(r,sizeof(*_tmpF6));_tmpF6->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmpEC;_tmpF6->type_vars=(struct Cyc_List_List*)
_tmpED;_tmpF6->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpEE;_tmpF6->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmpEF);_tmpF6->encloser=(
struct Cyc_Absyn_Stmt*)_tmpF0;_tmpF6->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct
Cyc_Tcenv_CtrlEnv*)_tmpF1);_tmpF6->capability=(void*)((void*)_tmpF2);_tmpF6->curr_rgn=(
void*)((void*)_tmpF3);_tmpF6->in_notreadctxt=(int)1;_tmpF6;});return Cyc_Tcenv_put_fenv(
r,te,_tmpF5);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpF7=te->le;if(
_tmpF7 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*
_tmpF9;struct Cyc_List_List*_tmpFA;struct Cyc_RgnOrder_RgnPO*_tmpFB;struct Cyc_Tcenv_Bindings*
_tmpFC;struct Cyc_Absyn_Stmt*_tmpFD;struct Cyc_Tcenv_CtrlEnv*_tmpFE;void*_tmpFF;
void*_tmp100;int _tmp101;struct Cyc_Tcenv_Fenv _tmpF8=*_tmpF7;_tmpF9=_tmpF8.shared;
_tmpFA=_tmpF8.type_vars;_tmpFB=_tmpF8.region_order;_tmpFC=_tmpF8.locals;_tmpFD=
_tmpF8.encloser;_tmpFE=_tmpF8.ctrl_env;_tmpFF=(void*)_tmpF8.capability;_tmp100=(
void*)_tmpF8.curr_rgn;_tmp101=_tmpF8.in_notreadctxt;{struct Cyc_Tcenv_Fenv*
_tmp102=({struct Cyc_Tcenv_Fenv*_tmp103=_region_malloc(r,sizeof(*_tmp103));
_tmp103->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpF9;_tmp103->type_vars=(struct
Cyc_List_List*)_tmpFA;_tmp103->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpFB;
_tmp103->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmpFC);
_tmp103->encloser=(struct Cyc_Absyn_Stmt*)_tmpFD;_tmp103->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmpFE);_tmp103->capability=(void*)((void*)_tmpFF);
_tmp103->curr_rgn=(void*)((void*)_tmp100);_tmp103->in_notreadctxt=(int)0;_tmp103;});
return Cyc_Tcenv_put_fenv(r,te,_tmp102);}}}int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv*_tmp104=te->le;if(_tmp104 == 0)return 0;{struct Cyc_Tcenv_Fenv
_tmp106;int _tmp107;struct Cyc_Tcenv_Fenv*_tmp105=(struct Cyc_Tcenv_Fenv*)_tmp104;
_tmp106=*_tmp105;_tmp107=_tmp106.in_notreadctxt;return _tmp107;}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){struct _dyneither_ptr*_tmp108=(*
vd->name).f2;struct Cyc_Tcenv_Fenv _tmp10B;struct Cyc_Tcenv_SharedFenv*_tmp10C;
struct Cyc_List_List*_tmp10D;struct Cyc_RgnOrder_RgnPO*_tmp10E;struct Cyc_Tcenv_Bindings*
_tmp10F;struct Cyc_Absyn_Stmt*_tmp110;struct Cyc_Tcenv_CtrlEnv*_tmp111;void*
_tmp112;void*_tmp113;int _tmp114;struct Cyc_Tcenv_Fenv*_tmp10A=Cyc_Tcenv_get_fenv(
te,({const char*_tmp109="add_pat_var";_tag_dyneither(_tmp109,sizeof(char),12);}));
_tmp10B=*_tmp10A;_tmp10C=_tmp10B.shared;_tmp10D=_tmp10B.type_vars;_tmp10E=
_tmp10B.region_order;_tmp10F=_tmp10B.locals;_tmp110=_tmp10B.encloser;_tmp111=
_tmp10B.ctrl_env;_tmp112=(void*)_tmp10B.capability;_tmp113=(void*)_tmp10B.curr_rgn;
_tmp114=_tmp10B.in_notreadctxt;{struct Cyc_Tcenv_Bindings*_tmp115=({struct Cyc_Tcenv_Bindings*
_tmp118=_region_malloc(r,sizeof(*_tmp118));_tmp118->v=_tmp108;_tmp118->b=(void*)((
void*)({struct Cyc_Absyn_Pat_b_struct*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119[
0]=({struct Cyc_Absyn_Pat_b_struct _tmp11A;_tmp11A.tag=4;_tmp11A.f1=vd;_tmp11A;});
_tmp119;}));_tmp118->tl=(struct Cyc_Tcenv_Bindings*)_tmp10F;_tmp118;});struct Cyc_Tcenv_Fenv*
_tmp116=({struct Cyc_Tcenv_Fenv*_tmp117=_region_malloc(r,sizeof(*_tmp117));
_tmp117->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp10C;_tmp117->type_vars=(struct
Cyc_List_List*)_tmp10D;_tmp117->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp10E;
_tmp117->locals=(struct Cyc_Tcenv_Bindings*)_tmp115;_tmp117->encloser=(struct Cyc_Absyn_Stmt*)
_tmp110;_tmp117->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp111);_tmp117->capability=(void*)((void*)_tmp112);_tmp117->curr_rgn=(void*)((
void*)_tmp113);_tmp117->in_notreadctxt=(int)_tmp114;_tmp117;});return Cyc_Tcenv_put_fenv(
r,te,_tmp116);}}void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct Cyc_Tcenv_Fenv*_tmp11B=
te->le;struct _tuple1 _tmp11D;union Cyc_Absyn_Nmspace_union _tmp11E;struct
_dyneither_ptr*_tmp11F;struct _tuple1*_tmp11C=q;_tmp11D=*_tmp11C;_tmp11E=_tmp11D.f1;
_tmp11F=_tmp11D.f2;{union Cyc_Absyn_Nmspace_union _tmp120=_tmp11E;struct Cyc_List_List*
_tmp121;_LL55: if((_tmp120.Loc_n).tag != 0)goto _LL57;_LL56: if(_tmp11B == 0)(int)
_throw((void*)Cyc_Dict_Absent);goto _LL58;_LL57: if((_tmp120.Rel_n).tag != 1)goto
_LL59;_tmp121=(_tmp120.Rel_n).f1;if(_tmp121 != 0)goto _LL59;if(!(_tmp11B != 0))goto
_LL59;_LL58: {struct Cyc_Tcenv_Fenv _tmp123;struct Cyc_Tcenv_Bindings*_tmp124;
struct Cyc_Tcenv_Fenv*_tmp122=(struct Cyc_Tcenv_Fenv*)_tmp11B;_tmp123=*_tmp122;
_tmp124=_tmp123.locals;{struct _handler_cons _tmp125;_push_handler(& _tmp125);{int
_tmp127=0;if(setjmp(_tmp125.handler))_tmp127=1;if(!_tmp127){{void*_tmp12A=(void*)({
struct Cyc_Tcenv_VarRes_struct*_tmp128=_region_malloc(r,sizeof(*_tmp128));_tmp128[
0]=({struct Cyc_Tcenv_VarRes_struct _tmp129;_tmp129.tag=0;_tmp129.f1=(void*)Cyc_Tcenv_lookup_binding(
_tmp124,_tmp11F);_tmp129;});_tmp128;});_npop_handler(0);return _tmp12A;};
_pop_handler();}else{void*_tmp126=(void*)_exn_thrown;void*_tmp12C=_tmp126;_LL5C:;
_LL5D: return(void*)Cyc_Tcenv_lookup_ordinary_global(te,loc,q);_LL5E:;_LL5F:(void)
_throw(_tmp12C);_LL5B:;}}}}_LL59:;_LL5A: {struct _handler_cons _tmp12D;
_push_handler(& _tmp12D);{int _tmp12F=0;if(setjmp(_tmp12D.handler))_tmp12F=1;if(!
_tmp12F){{void*_tmp130=(void*)Cyc_Tcenv_lookup_ordinary_global(te,loc,q);
_npop_handler(0);return _tmp130;};_pop_handler();}else{void*_tmp12E=(void*)
_exn_thrown;void*_tmp132=_tmp12E;_LL61: if(_tmp132 != Cyc_Dict_Absent)goto _LL63;
_LL62: return(void*)({struct Cyc_Tcenv_VarRes_struct*_tmp133=_region_malloc(r,
sizeof(*_tmp133));_tmp133[0]=({struct Cyc_Tcenv_VarRes_struct _tmp134;_tmp134.tag=
0;_tmp134.f1=(void*)((void*)0);_tmp134;});_tmp133;});_LL63:;_LL64:(void)_throw(
_tmp132);_LL60:;}}}_LL54:;}}void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv
_tmp137;struct Cyc_Tcenv_CtrlEnv*_tmp138;struct Cyc_Tcenv_Fenv*_tmp136=Cyc_Tcenv_get_fenv(
te,({const char*_tmp135="process_continue";_tag_dyneither(_tmp135,sizeof(char),17);}));
_tmp137=*_tmp136;_tmp138=_tmp137.ctrl_env;{void*_tmp139=(void*)_tmp138->continue_stmt;
struct Cyc_Absyn_Stmt*_tmp13A;_LL66: if(_tmp139 <= (void*)3)goto _LL68;if(*((int*)
_tmp139)!= 0)goto _LL68;_tmp13A=((struct Cyc_Tcenv_Stmt_j_struct*)_tmp139)->f1;
_LL67: _tmp13A->non_local_preds=({struct Cyc_List_List*_tmp13B=_cycalloc(sizeof(*
_tmp13B));_tmp13B->hd=s;_tmp13B->tl=_tmp13A->non_local_preds;_tmp13B;});*sopt=(
struct Cyc_Absyn_Stmt*)_tmp13A;return;_LL68: if((int)_tmp139 != 0)goto _LL6A;_LL69:({
void*_tmp13C=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp13D="continue not in a loop";
_tag_dyneither(_tmp13D,sizeof(char),23);}),_tag_dyneither(_tmp13C,sizeof(void*),
0));});return;_LL6A: if((int)_tmp139 != 1)goto _LL6C;_LL6B: goto _LL6D;_LL6C: if((int)
_tmp139 != 2)goto _LL65;_LL6D:((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({
const char*_tmp13E="bad continue destination";_tag_dyneither(_tmp13E,sizeof(char),
25);}));_LL65:;}}void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv _tmp141;struct Cyc_Tcenv_CtrlEnv*
_tmp142;struct Cyc_Tcenv_SharedFenv*_tmp143;struct Cyc_Tcenv_Fenv*_tmp140=Cyc_Tcenv_get_fenv(
te,({const char*_tmp13F="process_break";_tag_dyneither(_tmp13F,sizeof(char),14);}));
_tmp141=*_tmp140;_tmp142=_tmp141.ctrl_env;_tmp143=_tmp141.shared;{void*_tmp144=(
void*)_tmp142->break_stmt;struct Cyc_Absyn_Stmt*_tmp145;_LL6F: if(_tmp144 <= (void*)
3)goto _LL71;if(*((int*)_tmp144)!= 0)goto _LL71;_tmp145=((struct Cyc_Tcenv_Stmt_j_struct*)
_tmp144)->f1;_LL70: _tmp145->non_local_preds=({struct Cyc_List_List*_tmp146=
_cycalloc(sizeof(*_tmp146));_tmp146->hd=s;_tmp146->tl=_tmp145->non_local_preds;
_tmp146;});*sopt=(struct Cyc_Absyn_Stmt*)_tmp145;return;_LL71: if((int)_tmp144 != 0)
goto _LL73;_LL72:({void*_tmp147=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp148="break not in a loop or switch";
_tag_dyneither(_tmp148,sizeof(char),30);}),_tag_dyneither(_tmp147,sizeof(void*),
0));});return;_LL73: if((int)_tmp144 != 2)goto _LL75;_LL74: if((void*)_tmp143->return_typ
!= (void*)((void*)0))({void*_tmp149=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp14A="break causes function not to return a value";
_tag_dyneither(_tmp14A,sizeof(char),44);}),_tag_dyneither(_tmp149,sizeof(void*),
0));});return;_LL75: if((int)_tmp144 != 1)goto _LL6E;_LL76:({void*_tmp14B=0;Cyc_Tcutil_terr(
s->loc,({const char*_tmp14C="break causes outer switch clause to implicitly fallthru";
_tag_dyneither(_tmp14C,sizeof(char),56);}),_tag_dyneither(_tmp14B,sizeof(void*),
0));});return;_LL6E:;}}void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv
_tmp14F;struct Cyc_Tcenv_SharedFenv*_tmp150;struct Cyc_Tcenv_Fenv*_tmp14E=Cyc_Tcenv_get_fenv(
te,({const char*_tmp14D="process_goto";_tag_dyneither(_tmp14D,sizeof(char),13);}));
_tmp14F=*_tmp14E;_tmp150=_tmp14F.shared;{struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp150->seen_labels,
l);if(sopt2 == 0){struct Cyc_Dict_Dict _tmp151=_tmp150->needed_labels;struct Cyc_List_List**
slopt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))
Cyc_Dict_lookup_opt)(_tmp151,l);struct _RegionHandle*frgn=_tmp150->frgn;if(slopt
== 0)slopt=({struct Cyc_List_List**_tmp152=_region_malloc(frgn,sizeof(*_tmp152));
_tmp152[0]=0;_tmp152;});{struct Cyc_List_List*new_needed=({struct Cyc_List_List*
_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->hd=s;_tmp153->tl=*((struct Cyc_List_List**)
_check_null(slopt));_tmp153;});_tmp150->needed_labels=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(
_tmp151,l,new_needed);}}else{struct Cyc_Absyn_Stmt*s=*sopt2;s->non_local_preds=({
struct Cyc_List_List*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->hd=s;_tmp154->tl=
s->non_local_preds;_tmp154;});*sopt=(struct Cyc_Absyn_Stmt*)s;}}}struct _tuple4*
Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct
Cyc_Absyn_Switch_clause***clauseopt){struct Cyc_Tcenv_Fenv _tmp157;struct Cyc_Tcenv_CtrlEnv*
_tmp158;struct Cyc_Tcenv_Fenv*_tmp156=Cyc_Tcenv_get_fenv(te,({const char*_tmp155="process_fallthru";
_tag_dyneither(_tmp155,sizeof(char),17);}));_tmp157=*_tmp156;_tmp158=_tmp157.ctrl_env;{
struct _tuple4*_tmp159=(struct _tuple4*)_tmp158->fallthru_clause;if(_tmp159 != 0){(((*
_tmp159).f1)->body)->non_local_preds=({struct Cyc_List_List*_tmp15A=_cycalloc(
sizeof(*_tmp15A));_tmp15A->hd=s;_tmp15A->tl=(((*_tmp159).f1)->body)->non_local_preds;
_tmp15A;});*clauseopt=(struct Cyc_Absyn_Switch_clause**)({struct Cyc_Absyn_Switch_clause**
_tmp15B=_cycalloc(sizeof(*_tmp15B));_tmp15B[0]=(*_tmp159).f1;_tmp15B;});}return
_tmp159;}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,
struct Cyc_Absyn_Switch_clause*clause){struct Cyc_Tcenv_Fenv _tmp15E;struct Cyc_Tcenv_SharedFenv*
_tmp15F;struct Cyc_List_List*_tmp160;struct Cyc_RgnOrder_RgnPO*_tmp161;struct Cyc_Tcenv_Bindings*
_tmp162;struct Cyc_Absyn_Stmt*_tmp163;struct Cyc_Tcenv_CtrlEnv*_tmp164;void*
_tmp165;void*_tmp166;int _tmp167;struct Cyc_Tcenv_Fenv*_tmp15D=Cyc_Tcenv_get_fenv(
te,({const char*_tmp15C="set_fallthru";_tag_dyneither(_tmp15C,sizeof(char),13);}));
_tmp15E=*_tmp15D;_tmp15F=_tmp15E.shared;_tmp160=_tmp15E.type_vars;_tmp161=
_tmp15E.region_order;_tmp162=_tmp15E.locals;_tmp163=_tmp15E.encloser;_tmp164=
_tmp15E.ctrl_env;_tmp165=(void*)_tmp15E.capability;_tmp166=(void*)_tmp15E.curr_rgn;
_tmp167=_tmp15E.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv _tmp169;struct
_RegionHandle*_tmp16A;void*_tmp16B;void*_tmp16C;struct _tuple4*_tmp16D;void*
_tmp16E;int _tmp16F;struct Cyc_Tcenv_CtrlEnv*_tmp168=_tmp164;_tmp169=*_tmp168;
_tmp16A=_tmp169.ctrl_rgn;_tmp16B=(void*)_tmp169.continue_stmt;_tmp16C=(void*)
_tmp169.break_stmt;_tmp16D=_tmp169.fallthru_clause;_tmp16E=(void*)_tmp169.next_stmt;
_tmp16F=_tmp169.try_depth;{struct Cyc_List_List*ft_typ=0;for(0;vds != 0;vds=vds->tl){
ft_typ=({struct Cyc_List_List*_tmp170=_region_malloc(_tmp16A,sizeof(*_tmp170));
_tmp170->hd=(void*)((void*)((struct Cyc_Absyn_Vardecl*)vds->hd)->type);_tmp170->tl=
ft_typ;_tmp170;});}{struct Cyc_Tcenv_CList*_tmp171=(struct Cyc_Tcenv_CList*)Cyc_List_imp_rev(
ft_typ);struct Cyc_Tcenv_CtrlEnv*_tmp172=({struct Cyc_Tcenv_CtrlEnv*_tmp175=
_region_malloc(r,sizeof(*_tmp175));_tmp175->ctrl_rgn=_tmp16A;_tmp175->continue_stmt=(
void*)_tmp16B;_tmp175->break_stmt=(void*)_tmp16C;_tmp175->fallthru_clause=({
struct _tuple4*_tmp176=_region_malloc(_tmp16A,sizeof(*_tmp176));_tmp176->f1=
clause;_tmp176->f2=new_tvs;_tmp176->f3=_tmp171;_tmp176;});_tmp175->next_stmt=(
void*)_tmp16E;_tmp175->try_depth=_tmp16F;_tmp175;});struct Cyc_Tcenv_Fenv*_tmp173=({
struct Cyc_Tcenv_Fenv*_tmp174=_region_malloc(r,sizeof(*_tmp174));_tmp174->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmp15F;_tmp174->type_vars=(struct Cyc_List_List*)
_tmp160;_tmp174->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp161;_tmp174->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmp162);_tmp174->encloser=(
struct Cyc_Absyn_Stmt*)_tmp163;_tmp174->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)
_tmp172;_tmp174->capability=(void*)((void*)_tmp165);_tmp174->curr_rgn=(void*)((
void*)_tmp166);_tmp174->in_notreadctxt=(int)_tmp167;_tmp174;});return Cyc_Tcenv_put_fenv(
r,te,_tmp173);}}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(
r,Cyc_Tcenv_get_fenv(te,({const char*_tmp17A="clear_fallthru";_tag_dyneither(
_tmp17A,sizeof(char),15);})));struct Cyc_Tcenv_Fenv _tmp178;struct Cyc_Tcenv_CtrlEnv*
_tmp179;struct Cyc_Tcenv_Fenv*_tmp177=fe;_tmp178=*_tmp177;_tmp179=_tmp178.ctrl_env;
_tmp179->fallthru_clause=0;return Cyc_Tcenv_put_fenv(r,te,fe);}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
continue_dest){struct Cyc_Tcenv_Fenv _tmp17D;struct Cyc_Tcenv_SharedFenv*_tmp17E;
struct Cyc_List_List*_tmp17F;struct Cyc_RgnOrder_RgnPO*_tmp180;struct Cyc_Tcenv_Bindings*
_tmp181;struct Cyc_Absyn_Stmt*_tmp182;struct Cyc_Tcenv_CtrlEnv*_tmp183;void*
_tmp184;void*_tmp185;int _tmp186;struct Cyc_Tcenv_Fenv*_tmp17C=Cyc_Tcenv_get_fenv(
te,({const char*_tmp17B="set_in_loop";_tag_dyneither(_tmp17B,sizeof(char),12);}));
_tmp17D=*_tmp17C;_tmp17E=_tmp17D.shared;_tmp17F=_tmp17D.type_vars;_tmp180=
_tmp17D.region_order;_tmp181=_tmp17D.locals;_tmp182=_tmp17D.encloser;_tmp183=
_tmp17D.ctrl_env;_tmp184=(void*)_tmp17D.capability;_tmp185=(void*)_tmp17D.curr_rgn;
_tmp186=_tmp17D.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv*new_cenv=({struct Cyc_Tcenv_CtrlEnv*
_tmp188=_region_malloc(r,sizeof(*_tmp188));_tmp188->ctrl_rgn=r;_tmp188->continue_stmt=(
void*)((void*)({struct Cyc_Tcenv_Stmt_j_struct*_tmp18B=_region_malloc(r,sizeof(*
_tmp18B));_tmp18B[0]=({struct Cyc_Tcenv_Stmt_j_struct _tmp18C;_tmp18C.tag=0;
_tmp18C.f1=continue_dest;_tmp18C;});_tmp18B;}));_tmp188->break_stmt=(void*)((
void*)((void*)_tmp183->next_stmt));_tmp188->next_stmt=(void*)((void*)({struct Cyc_Tcenv_Stmt_j_struct*
_tmp189=_region_malloc(r,sizeof(*_tmp189));_tmp189[0]=({struct Cyc_Tcenv_Stmt_j_struct
_tmp18A;_tmp18A.tag=0;_tmp18A.f1=continue_dest;_tmp18A;});_tmp189;}));_tmp188->fallthru_clause=(
struct _tuple4*)_tmp183->fallthru_clause;_tmp188->try_depth=_tmp183->try_depth;
_tmp188;});struct Cyc_Tcenv_Fenv*new_fenv=({struct Cyc_Tcenv_Fenv*_tmp187=
_region_malloc(r,sizeof(*_tmp187));_tmp187->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp17E;_tmp187->type_vars=(struct Cyc_List_List*)_tmp17F;_tmp187->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp180;_tmp187->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp181);_tmp187->encloser=(struct Cyc_Absyn_Stmt*)
_tmp182;_tmp187->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv;_tmp187->capability=(
void*)((void*)_tmp184);_tmp187->curr_rgn=(void*)((void*)_tmp185);_tmp187->in_notreadctxt=(
int)_tmp186;_tmp187;});return Cyc_Tcenv_put_fenv(r,te,new_fenv);}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_enter_try(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp190="enter_try";
_tag_dyneither(_tmp190,sizeof(char),10);})));struct Cyc_Tcenv_Fenv _tmp18E;struct
Cyc_Tcenv_CtrlEnv*_tmp18F;struct Cyc_Tcenv_Fenv*_tmp18D=fe;_tmp18E=*_tmp18D;
_tmp18F=_tmp18E.ctrl_env;++ _tmp18F->try_depth;return Cyc_Tcenv_put_fenv(r,te,fe);}
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp193;
struct Cyc_Tcenv_CtrlEnv*_tmp194;struct Cyc_Tcenv_Fenv*_tmp192=Cyc_Tcenv_get_fenv(
te,({const char*_tmp191="get_try_depth";_tag_dyneither(_tmp191,sizeof(char),14);}));
_tmp193=*_tmp192;_tmp194=_tmp193.ctrl_env;return _tmp194->try_depth;}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,({const char*_tmp198="set_in_switch";
_tag_dyneither(_tmp198,sizeof(char),14);})));struct Cyc_Tcenv_Fenv _tmp196;struct
Cyc_Tcenv_CtrlEnv*_tmp197;struct Cyc_Tcenv_Fenv*_tmp195=fe;_tmp196=*_tmp195;
_tmp197=_tmp196.ctrl_env;(void*)(_tmp197->break_stmt=(void*)((void*)_tmp197->next_stmt));(
void*)(_tmp197->next_stmt=(void*)((void*)1));return Cyc_Tcenv_put_fenv(r,te,fe);}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,void*j){struct Cyc_Tcenv_SharedFenv*_tmp19B;struct Cyc_List_List*_tmp19C;struct
Cyc_RgnOrder_RgnPO*_tmp19D;struct Cyc_Tcenv_Bindings*_tmp19E;struct Cyc_Absyn_Stmt*
_tmp19F;struct Cyc_Tcenv_CtrlEnv*_tmp1A0;void*_tmp1A1;void*_tmp1A2;int _tmp1A3;
struct Cyc_Tcenv_Fenv _tmp19A=*Cyc_Tcenv_get_fenv(te,({const char*_tmp199="set_next";
_tag_dyneither(_tmp199,sizeof(char),9);}));_tmp19B=_tmp19A.shared;_tmp19C=
_tmp19A.type_vars;_tmp19D=_tmp19A.region_order;_tmp19E=_tmp19A.locals;_tmp19F=
_tmp19A.encloser;_tmp1A0=_tmp19A.ctrl_env;_tmp1A1=(void*)_tmp19A.capability;
_tmp1A2=(void*)_tmp19A.curr_rgn;_tmp1A3=_tmp19A.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv*
new_cenv=({struct Cyc_Tcenv_CtrlEnv*_tmp1A5=_region_malloc(r,sizeof(*_tmp1A5));
_tmp1A5->ctrl_rgn=r;_tmp1A5->continue_stmt=(void*)((void*)((void*)_tmp1A0->continue_stmt));
_tmp1A5->break_stmt=(void*)((void*)((void*)_tmp1A0->break_stmt));_tmp1A5->next_stmt=(
void*)j;_tmp1A5->fallthru_clause=(struct _tuple4*)_tmp1A0->fallthru_clause;
_tmp1A5->try_depth=_tmp1A0->try_depth;_tmp1A5;});struct Cyc_Tcenv_Fenv*new_fenv=({
struct Cyc_Tcenv_Fenv*_tmp1A4=_region_malloc(r,sizeof(*_tmp1A4));_tmp1A4->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmp19B;_tmp1A4->type_vars=(struct Cyc_List_List*)
_tmp19C;_tmp1A4->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp19D;_tmp1A4->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmp19E);_tmp1A4->encloser=(
struct Cyc_Absyn_Stmt*)_tmp19F;_tmp1A4->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)
new_cenv;_tmp1A4->capability=(void*)((void*)_tmp1A1);_tmp1A4->curr_rgn=(void*)((
void*)_tmp1A2);_tmp1A4->in_notreadctxt=(int)_tmp1A3;_tmp1A4;});return Cyc_Tcenv_put_fenv(
r,te,new_fenv);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*
te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Tcenv_Fenv _tmp1A8;
struct Cyc_Tcenv_SharedFenv*_tmp1A9;struct Cyc_Tcenv_Fenv*_tmp1A7=Cyc_Tcenv_get_fenv(
te,({const char*_tmp1A6="add_label";_tag_dyneither(_tmp1A6,sizeof(char),10);}));
_tmp1A8=*_tmp1A7;_tmp1A9=_tmp1A8.shared;{struct Cyc_Dict_Dict needed=_tmp1A9->needed_labels;
struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(needed,v);struct _RegionHandle*frgn=
_tmp1A9->frgn;if(sl_opt != 0){_tmp1A9->needed_labels=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete)(
frgn,needed,v);{struct Cyc_List_List*_tmp1AA=*sl_opt;s->non_local_preds=_tmp1AA;
for(0;_tmp1AA != 0;_tmp1AA=_tmp1AA->tl){void*_tmp1AB=(void*)((struct Cyc_Absyn_Stmt*)
_tmp1AA->hd)->r;struct Cyc_Absyn_Stmt*_tmp1AC;struct Cyc_Absyn_Stmt**_tmp1AD;_LL78:
if(_tmp1AB <= (void*)1)goto _LL7A;if(*((int*)_tmp1AB)!= 7)goto _LL7A;_tmp1AC=((
struct Cyc_Absyn_Goto_s_struct*)_tmp1AB)->f2;_tmp1AD=(struct Cyc_Absyn_Stmt**)&((
struct Cyc_Absyn_Goto_s_struct*)_tmp1AB)->f2;_LL79:*_tmp1AD=(struct Cyc_Absyn_Stmt*)
s;goto _LL77;_LL7A:;_LL7B:((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({
const char*_tmp1AE="Tcenv: add_label backpatching of non-goto";_tag_dyneither(
_tmp1AE,sizeof(char),42);}));goto _LL77;_LL77:;}}}if(((int(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp1A9->seen_labels,v))({struct Cyc_String_pa_struct
_tmp1B1;_tmp1B1.tag=0;_tmp1B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v);{
void*_tmp1AF[1]={& _tmp1B1};Cyc_Tcutil_terr(s->loc,({const char*_tmp1B0="Repeated label: %s";
_tag_dyneither(_tmp1B0,sizeof(char),19);}),_tag_dyneither(_tmp1AF,sizeof(void*),
1));}});_tmp1A9->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp1A9->seen_labels,
v,s);return te;}}int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te){struct
Cyc_Tcenv_Fenv _tmp1B4;struct Cyc_Tcenv_SharedFenv*_tmp1B5;struct Cyc_Tcenv_Fenv*
_tmp1B3=Cyc_Tcenv_get_fenv(te,({const char*_tmp1B2="all_labels_resolved";
_tag_dyneither(_tmp1B2,sizeof(char),20);}));_tmp1B4=*_tmp1B3;_tmp1B5=_tmp1B4.shared;
return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp1B5->needed_labels);}
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv
_tmp1B8;struct Cyc_Absyn_Stmt*_tmp1B9;struct Cyc_Tcenv_Fenv*_tmp1B7=Cyc_Tcenv_get_fenv(
te,({const char*_tmp1B6="get_encloser";_tag_dyneither(_tmp1B6,sizeof(char),13);}));
_tmp1B8=*_tmp1B7;_tmp1B9=_tmp1B8.encloser;return _tmp1B9;}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){if(te->le
== 0)return({struct Cyc_Tcenv_Tenv*_tmp1BA=_region_malloc(r2,sizeof(*_tmp1BA));
_tmp1BA->ns=te->ns;_tmp1BA->ae=te->ae;_tmp1BA->le=0;_tmp1BA->allow_valueof=1;
_tmp1BA;});{struct Cyc_Tcenv_SharedFenv*_tmp1BC;struct Cyc_List_List*_tmp1BD;
struct Cyc_RgnOrder_RgnPO*_tmp1BE;struct Cyc_Tcenv_Bindings*_tmp1BF;struct Cyc_Absyn_Stmt*
_tmp1C0;struct Cyc_Tcenv_CtrlEnv*_tmp1C1;void*_tmp1C2;void*_tmp1C3;int _tmp1C4;
struct Cyc_Tcenv_Fenv _tmp1BB=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));
_tmp1BC=_tmp1BB.shared;_tmp1BD=_tmp1BB.type_vars;_tmp1BE=_tmp1BB.region_order;
_tmp1BF=_tmp1BB.locals;_tmp1C0=_tmp1BB.encloser;_tmp1C1=_tmp1BB.ctrl_env;_tmp1C2=(
void*)_tmp1BB.capability;_tmp1C3=(void*)_tmp1BB.curr_rgn;_tmp1C4=_tmp1BB.in_notreadctxt;{
struct Cyc_Tcenv_Fenv*_tmp1C5=({struct Cyc_Tcenv_Fenv*_tmp1C7=_region_malloc(r2,
sizeof(*_tmp1C7));_tmp1C7->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1BC;_tmp1C7->type_vars=(
struct Cyc_List_List*)_tmp1BD;_tmp1C7->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp1BE;_tmp1C7->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp1BF);_tmp1C7->encloser=(struct Cyc_Absyn_Stmt*)_tmp1C0;_tmp1C7->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp1C1);_tmp1C7->capability=(
void*)((void*)_tmp1C2);_tmp1C7->curr_rgn=(void*)((void*)_tmp1C3);_tmp1C7->in_notreadctxt=(
int)_tmp1C4;_tmp1C7;});return({struct Cyc_Tcenv_Tenv*_tmp1C6=_region_malloc(r2,
sizeof(*_tmp1C6));_tmp1C6->ns=te->ns;_tmp1C6->ae=te->ae;_tmp1C6->le=_tmp1C5;
_tmp1C6->allow_valueof=1;_tmp1C6;});}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){struct Cyc_Tcenv_SharedFenv*
_tmp1CA;struct Cyc_List_List*_tmp1CB;struct Cyc_RgnOrder_RgnPO*_tmp1CC;struct Cyc_Tcenv_Bindings*
_tmp1CD;struct Cyc_Absyn_Stmt*_tmp1CE;struct Cyc_Tcenv_CtrlEnv*_tmp1CF;void*
_tmp1D0;void*_tmp1D1;int _tmp1D2;struct Cyc_Tcenv_Fenv _tmp1C9=*Cyc_Tcenv_get_fenv(
te,({const char*_tmp1C8="set_encloser";_tag_dyneither(_tmp1C8,sizeof(char),13);}));
_tmp1CA=_tmp1C9.shared;_tmp1CB=_tmp1C9.type_vars;_tmp1CC=_tmp1C9.region_order;
_tmp1CD=_tmp1C9.locals;_tmp1CE=_tmp1C9.encloser;_tmp1CF=_tmp1C9.ctrl_env;_tmp1D0=(
void*)_tmp1C9.capability;_tmp1D1=(void*)_tmp1C9.curr_rgn;_tmp1D2=_tmp1C9.in_notreadctxt;{
struct Cyc_Tcenv_Fenv*_tmp1D3=({struct Cyc_Tcenv_Fenv*_tmp1D4=_region_malloc(r,
sizeof(*_tmp1D4));_tmp1D4->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1CA;_tmp1D4->type_vars=(
struct Cyc_List_List*)_tmp1CB;_tmp1D4->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp1CC;_tmp1D4->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp1CD);_tmp1D4->encloser=(struct Cyc_Absyn_Stmt*)s;_tmp1D4->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmp1CF);_tmp1D4->capability=(void*)((void*)_tmp1D0);
_tmp1D4->curr_rgn=(void*)((void*)_tmp1D1);_tmp1D4->in_notreadctxt=(int)_tmp1D2;
_tmp1D4;});return Cyc_Tcenv_put_fenv(r,te,_tmp1D3);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
struct Cyc_Absyn_Tvar*tv;{void*_tmp1D5=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*
_tmp1D6;_LL7D: if(_tmp1D5 <= (void*)4)goto _LL7F;if(*((int*)_tmp1D5)!= 1)goto _LL7F;
_tmp1D6=((struct Cyc_Absyn_VarType_struct*)_tmp1D5)->f1;_LL7E: tv=_tmp1D6;goto
_LL7C;_LL7F:;_LL80: tv=((struct Cyc_Absyn_Tvar*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({
const char*_tmp1D7="bad add region";_tag_dyneither(_tmp1D7,sizeof(char),15);}));
goto _LL7C;_LL7C:;}{struct Cyc_Tcenv_SharedFenv*_tmp1DA;struct Cyc_List_List*
_tmp1DB;struct Cyc_RgnOrder_RgnPO*_tmp1DC;struct Cyc_Tcenv_Bindings*_tmp1DD;struct
Cyc_Absyn_Stmt*_tmp1DE;struct Cyc_Tcenv_CtrlEnv*_tmp1DF;void*_tmp1E0;void*_tmp1E1;
int _tmp1E2;struct Cyc_Tcenv_Fenv _tmp1D9=*Cyc_Tcenv_get_fenv(te,({const char*
_tmp1D8="add_region";_tag_dyneither(_tmp1D8,sizeof(char),11);}));_tmp1DA=_tmp1D9.shared;
_tmp1DB=_tmp1D9.type_vars;_tmp1DC=_tmp1D9.region_order;_tmp1DD=_tmp1D9.locals;
_tmp1DE=_tmp1D9.encloser;_tmp1DF=_tmp1D9.ctrl_env;_tmp1E0=(void*)_tmp1D9.capability;
_tmp1E1=(void*)_tmp1D9.curr_rgn;_tmp1E2=_tmp1D9.in_notreadctxt;_tmp1DC=Cyc_RgnOrder_add_youngest(
_tmp1DA->frgn,_tmp1DC,tv,resetable,opened);_tmp1E0=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1E4;_tmp1E4.tag=20;_tmp1E4.f1=({struct Cyc_List_List*_tmp1E5=_cycalloc(
sizeof(*_tmp1E5));_tmp1E5->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1E8;_tmp1E8.tag=19;_tmp1E8.f1=(void*)rgn;_tmp1E8;});_tmp1E7;}));_tmp1E5->tl=({
struct Cyc_List_List*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));_tmp1E6->hd=(void*)
_tmp1E0;_tmp1E6->tl=0;_tmp1E6;});_tmp1E5;});_tmp1E4;});_tmp1E3;});{struct Cyc_Tcenv_Fenv*
_tmp1E9=({struct Cyc_Tcenv_Fenv*_tmp1EA=_region_malloc(r,sizeof(*_tmp1EA));
_tmp1EA->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1DA;_tmp1EA->type_vars=(struct
Cyc_List_List*)_tmp1DB;_tmp1EA->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1DC;
_tmp1EA->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmp1DD);
_tmp1EA->encloser=(struct Cyc_Absyn_Stmt*)_tmp1DE;_tmp1EA->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmp1DF);_tmp1EA->capability=(void*)((void*)_tmp1E0);
_tmp1EA->curr_rgn=(void*)((void*)_tmp1E1);_tmp1EA->in_notreadctxt=(int)_tmp1E2;
_tmp1EA;});return Cyc_Tcenv_put_fenv(r,te,_tmp1E9);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Tvar*block_rgn){struct Cyc_Tcenv_SharedFenv*_tmp1ED;struct Cyc_List_List*
_tmp1EE;struct Cyc_RgnOrder_RgnPO*_tmp1EF;struct Cyc_Tcenv_Bindings*_tmp1F0;struct
Cyc_Absyn_Stmt*_tmp1F1;struct Cyc_Tcenv_CtrlEnv*_tmp1F2;void*_tmp1F3;void*_tmp1F4;
int _tmp1F5;struct Cyc_Tcenv_Fenv _tmp1EC=*Cyc_Tcenv_get_fenv(te,({const char*
_tmp1EB="new_named_block";_tag_dyneither(_tmp1EB,sizeof(char),16);}));_tmp1ED=
_tmp1EC.shared;_tmp1EE=_tmp1EC.type_vars;_tmp1EF=_tmp1EC.region_order;_tmp1F0=
_tmp1EC.locals;_tmp1F1=_tmp1EC.encloser;_tmp1F2=_tmp1EC.ctrl_env;_tmp1F3=(void*)
_tmp1EC.capability;_tmp1F4=(void*)_tmp1EC.curr_rgn;_tmp1F5=_tmp1EC.in_notreadctxt;{
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,({
const char*_tmp201="new_block";_tag_dyneither(_tmp201,sizeof(char),10);})));void*
block_typ=(void*)({struct Cyc_Absyn_VarType_struct*_tmp1FF=_cycalloc(sizeof(*
_tmp1FF));_tmp1FF[0]=({struct Cyc_Absyn_VarType_struct _tmp200;_tmp200.tag=1;
_tmp200.f1=block_rgn;_tmp200;});_tmp1FF;});_tmp1EE=({struct Cyc_List_List*_tmp1F6=
_cycalloc(sizeof(*_tmp1F6));_tmp1F6->hd=block_rgn;_tmp1F6->tl=_tmp1EE;_tmp1F6;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp1EE);_tmp1EF=Cyc_RgnOrder_add_youngest(
_tmp1ED->frgn,_tmp1EF,block_rgn,0,0);_tmp1F3=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp1F7=_cycalloc(sizeof(*_tmp1F7));_tmp1F7[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp1F8;_tmp1F8.tag=20;_tmp1F8.f1=({struct Cyc_List_List*_tmp1F9=_cycalloc(
sizeof(*_tmp1F9));_tmp1F9->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1FB=_cycalloc(sizeof(*_tmp1FB));_tmp1FB[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1FC;_tmp1FC.tag=19;_tmp1FC.f1=(void*)block_typ;_tmp1FC;});_tmp1FB;}));
_tmp1F9->tl=({struct Cyc_List_List*_tmp1FA=_cycalloc(sizeof(*_tmp1FA));_tmp1FA->hd=(
void*)_tmp1F3;_tmp1FA->tl=0;_tmp1FA;});_tmp1F9;});_tmp1F8;});_tmp1F7;});_tmp1F4=
block_typ;{struct Cyc_Tcenv_Fenv*_tmp1FD=({struct Cyc_Tcenv_Fenv*_tmp1FE=
_region_malloc(r,sizeof(*_tmp1FE));_tmp1FE->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp1ED;_tmp1FE->type_vars=(struct Cyc_List_List*)_tmp1EE;_tmp1FE->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp1EF;_tmp1FE->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp1F0);_tmp1FE->encloser=(struct Cyc_Absyn_Stmt*)
_tmp1F1;_tmp1FE->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp1F2);_tmp1FE->capability=(void*)((void*)_tmp1F3);_tmp1FE->curr_rgn=(void*)((
void*)_tmp1F4);_tmp1FE->in_notreadctxt=(int)_tmp1F5;_tmp1FE;});return Cyc_Tcenv_put_fenv(
r,te,_tmp1FD);}}}static struct Cyc_Absyn_Eq_kb_struct Cyc_Tcenv_rgn_kb={0,(void*)((
void*)3)};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,struct
Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te){struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((
void*)& Cyc_Tcenv_rgn_kb);Cyc_Tcutil_add_tvar_identity(t);return Cyc_Tcenv_new_named_block(
r,loc,te,t);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*
loc){struct Cyc_Tcenv_SharedFenv*_tmp205;struct Cyc_List_List*_tmp206;struct Cyc_RgnOrder_RgnPO*
_tmp207;struct Cyc_Tcenv_Bindings*_tmp208;struct Cyc_Absyn_Stmt*_tmp209;struct Cyc_Tcenv_CtrlEnv*
_tmp20A;void*_tmp20B;void*_tmp20C;int _tmp20D;struct Cyc_Tcenv_Fenv _tmp204=*Cyc_Tcenv_get_fenv(
te,({const char*_tmp203="new_outlives_constraints";_tag_dyneither(_tmp203,sizeof(
char),25);}));_tmp205=_tmp204.shared;_tmp206=_tmp204.type_vars;_tmp207=_tmp204.region_order;
_tmp208=_tmp204.locals;_tmp209=_tmp204.encloser;_tmp20A=_tmp204.ctrl_env;_tmp20B=(
void*)_tmp204.capability;_tmp20C=(void*)_tmp204.curr_rgn;_tmp20D=_tmp204.in_notreadctxt;
for(0;cs != 0;cs=cs->tl){_tmp207=Cyc_RgnOrder_add_outlives_constraint(_tmp205->frgn,
_tmp207,(*((struct _tuple0*)cs->hd)).f1,(*((struct _tuple0*)cs->hd)).f2,loc);}{
struct Cyc_Tcenv_Fenv*_tmp20E=({struct Cyc_Tcenv_Fenv*_tmp20F=_region_malloc(r,
sizeof(*_tmp20F));_tmp20F->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp205;_tmp20F->type_vars=(
struct Cyc_List_List*)_tmp206;_tmp20F->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp207;_tmp20F->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp208);_tmp20F->encloser=(struct Cyc_Absyn_Stmt*)_tmp209;_tmp20F->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp20A);_tmp20F->capability=(
void*)((void*)_tmp20B);_tmp20F->curr_rgn=(void*)((void*)_tmp20C);_tmp20F->in_notreadctxt=(
int)_tmp20D;_tmp20F;});return Cyc_Tcenv_put_fenv(r,te,_tmp20E);}}void*Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp210=te->le;if(_tmp210 == 0)
return(void*)2;{struct Cyc_Tcenv_Fenv*fe=(struct Cyc_Tcenv_Fenv*)_tmp210;struct Cyc_Tcenv_Fenv
_tmp212;void*_tmp213;struct Cyc_Tcenv_Fenv*_tmp211=fe;_tmp212=*_tmp211;_tmp213=(
void*)_tmp212.curr_rgn;return _tmp213;}}void Cyc_Tcenv_check_rgn_accessible(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*rgn){struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_get_fenv(te,({const char*_tmp21D="check_rgn_accessible";
_tag_dyneither(_tmp21D,sizeof(char),21);}));struct Cyc_Tcenv_Fenv _tmp215;void*
_tmp216;struct Cyc_RgnOrder_RgnPO*_tmp217;struct Cyc_Tcenv_Fenv*_tmp214=fe;_tmp215=*
_tmp214;_tmp216=(void*)_tmp215.capability;_tmp217=_tmp215.region_order;if(Cyc_Tcutil_region_in_effect(
0,rgn,_tmp216) || Cyc_Tcutil_region_in_effect(1,rgn,_tmp216))return;if(Cyc_RgnOrder_eff_outlives_eff(
_tmp217,(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp218=_cycalloc(sizeof(*
_tmp218));_tmp218[0]=({struct Cyc_Absyn_AccessEff_struct _tmp219;_tmp219.tag=19;
_tmp219.f1=(void*)rgn;_tmp219;});_tmp218;}),_tmp216))return;({struct Cyc_String_pa_struct
_tmp21C;_tmp21C.tag=0;_tmp21C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(rgn));{void*_tmp21A[1]={& _tmp21C};Cyc_Tcutil_terr(loc,({
const char*_tmp21B="Expression accesses unavailable region %s";_tag_dyneither(
_tmp21B,sizeof(char),42);}),_tag_dyneither(_tmp21A,sizeof(void*),1));}});}void
Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*rgn){Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{struct Cyc_Tcenv_Fenv
_tmp220;struct Cyc_RgnOrder_RgnPO*_tmp221;struct Cyc_Tcenv_Fenv*_tmp21F=Cyc_Tcenv_get_fenv(
te,({const char*_tmp21E="check_rgn_resetable";_tag_dyneither(_tmp21E,sizeof(char),
20);}));_tmp220=*_tmp21F;_tmp221=_tmp220.region_order;{void*_tmp222=Cyc_Tcutil_compress(
rgn);struct Cyc_Absyn_Tvar*_tmp223;_LL82: if(_tmp222 <= (void*)4)goto _LL84;if(*((
int*)_tmp222)!= 1)goto _LL84;_tmp223=((struct Cyc_Absyn_VarType_struct*)_tmp222)->f1;
_LL83: if(!Cyc_RgnOrder_is_region_resetable(_tmp221,_tmp223))({struct Cyc_String_pa_struct
_tmp226;_tmp226.tag=0;_tmp226.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(rgn));{void*_tmp224[1]={& _tmp226};Cyc_Tcutil_terr(loc,({
const char*_tmp225="Region %s is not resetable";_tag_dyneither(_tmp225,sizeof(
char),27);}),_tag_dyneither(_tmp224,sizeof(void*),1));}});return;_LL84:;_LL85:({
void*_tmp227=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp228="check_rgn_resetable";_tag_dyneither(_tmp228,sizeof(char),20);}),
_tag_dyneither(_tmp227,sizeof(void*),0));});_LL81:;}}}int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){struct Cyc_Tcenv_Fenv*_tmp229=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);rt_b=Cyc_Tcutil_compress(rt_b);if(_tmp229 == 0)
return rt_a == (void*)3  || rt_a == (void*)2  && rt_b != (void*)3;{struct Cyc_Tcenv_Fenv*
fe=(struct Cyc_Tcenv_Fenv*)_tmp229;struct Cyc_Tcenv_Fenv _tmp22B;struct Cyc_RgnOrder_RgnPO*
_tmp22C;struct Cyc_Tcenv_Fenv*_tmp22A=fe;_tmp22B=*_tmp22A;_tmp22C=_tmp22B.region_order;{
int _tmp22D=Cyc_RgnOrder_effect_outlives(_tmp22C,(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp22E=_cycalloc(sizeof(*_tmp22E));_tmp22E[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp22F;_tmp22F.tag=19;_tmp22F.f1=(void*)rt_a;_tmp22F;});_tmp22E;}),rt_b);return
_tmp22D;}}}struct _tuple6{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;struct Cyc_Position_Segment*
f4;};void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*eff){struct Cyc_Tcenv_Fenv _tmp232;void*_tmp233;struct Cyc_RgnOrder_RgnPO*
_tmp234;struct Cyc_Tcenv_SharedFenv*_tmp235;struct Cyc_Tcenv_Fenv*_tmp231=Cyc_Tcenv_get_fenv(
te,({const char*_tmp230="check_effect_accessible";_tag_dyneither(_tmp230,sizeof(
char),24);}));_tmp232=*_tmp231;_tmp233=(void*)_tmp232.capability;_tmp234=_tmp232.region_order;
_tmp235=_tmp232.shared;if(Cyc_Tcutil_subset_effect(0,eff,_tmp233))return;if(Cyc_RgnOrder_eff_outlives_eff(
_tmp234,eff,_tmp233))return;{struct _RegionHandle*frgn=_tmp235->frgn;_tmp235->delayed_effect_checks=({
struct Cyc_List_List*_tmp236=_region_malloc(frgn,sizeof(*_tmp236));_tmp236->hd=({
struct _tuple6*_tmp237=_region_malloc(frgn,sizeof(*_tmp237));_tmp237->f1=_tmp233;
_tmp237->f2=eff;_tmp237->f3=_tmp234;_tmp237->f4=loc;_tmp237;});_tmp236->tl=
_tmp235->delayed_effect_checks;_tmp236;});}}void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp23A;struct Cyc_Tcenv_SharedFenv*
_tmp23B;struct Cyc_Tcenv_Fenv*_tmp239=Cyc_Tcenv_get_fenv(te,({const char*_tmp238="check_delayed_effects";
_tag_dyneither(_tmp238,sizeof(char),22);}));_tmp23A=*_tmp239;_tmp23B=_tmp23A.shared;{
struct Cyc_List_List*_tmp23C=_tmp23B->delayed_effect_checks;for(0;_tmp23C != 0;
_tmp23C=_tmp23C->tl){struct _tuple6 _tmp23E;void*_tmp23F;void*_tmp240;struct Cyc_RgnOrder_RgnPO*
_tmp241;struct Cyc_Position_Segment*_tmp242;struct _tuple6*_tmp23D=(struct _tuple6*)
_tmp23C->hd;_tmp23E=*_tmp23D;_tmp23F=_tmp23E.f1;_tmp240=_tmp23E.f2;_tmp241=
_tmp23E.f3;_tmp242=_tmp23E.f4;if(Cyc_Tcutil_subset_effect(1,_tmp240,_tmp23F))
continue;if(Cyc_RgnOrder_eff_outlives_eff(_tmp241,_tmp240,_tmp23F))continue;({
struct Cyc_String_pa_struct _tmp246;_tmp246.tag=0;_tmp246.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp240));{struct Cyc_String_pa_struct
_tmp245;_tmp245.tag=0;_tmp245.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp23F));{void*_tmp243[2]={& _tmp245,& _tmp246};Cyc_Tcutil_terr(
_tmp242,({const char*_tmp244="Capability \n%s\ndoes not cover function's effect\n%s";
_tag_dyneither(_tmp244,sizeof(char),51);}),_tag_dyneither(_tmp243,sizeof(void*),
2));}}});}}}struct _tuple7{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;
struct Cyc_Position_Segment*f3;};void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po){struct Cyc_Tcenv_Fenv*
_tmp247=te->le;if(_tmp247 == 0){for(0;po != 0;po=po->tl){if(!Cyc_Tcutil_subset_effect(
1,(*((struct _tuple0*)po->hd)).f1,Cyc_Absyn_empty_effect) || !Cyc_Tcutil_subset_effect(
1,(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp248=_cycalloc(sizeof(*_tmp248));
_tmp248[0]=({struct Cyc_Absyn_AccessEff_struct _tmp249;_tmp249.tag=19;_tmp249.f1=(
void*)(*((struct _tuple0*)po->hd)).f2;_tmp249;});_tmp248;}),Cyc_Absyn_empty_effect))({
void*_tmp24A=0;Cyc_Tcutil_terr(loc,({const char*_tmp24B="the required region ordering is not satisfied here";
_tag_dyneither(_tmp24B,sizeof(char),51);}),_tag_dyneither(_tmp24A,sizeof(void*),
0));});}return;}{struct Cyc_Tcenv_Fenv _tmp24D;struct Cyc_RgnOrder_RgnPO*_tmp24E;
struct Cyc_Tcenv_SharedFenv*_tmp24F;struct Cyc_Tcenv_Fenv*_tmp24C=(struct Cyc_Tcenv_Fenv*)
_tmp247;_tmp24D=*_tmp24C;_tmp24E=_tmp24D.region_order;_tmp24F=_tmp24D.shared;if(
!Cyc_RgnOrder_satisfies_constraints(_tmp24E,po,(void*)2,0))_tmp24F->delayed_constraint_checks=({
struct Cyc_List_List*_tmp250=_region_malloc(_tmp24F->frgn,sizeof(*_tmp250));
_tmp250->hd=({struct _tuple7*_tmp251=_region_malloc(_tmp24F->frgn,sizeof(*_tmp251));
_tmp251->f1=_tmp24E;_tmp251->f2=po;_tmp251->f3=loc;_tmp251;});_tmp250->tl=
_tmp24F->delayed_constraint_checks;_tmp250;});}}void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp254;struct Cyc_Tcenv_SharedFenv*
_tmp255;struct Cyc_Tcenv_Fenv*_tmp253=Cyc_Tcenv_get_fenv(te,({const char*_tmp252="check_delayed_constraints";
_tag_dyneither(_tmp252,sizeof(char),26);}));_tmp254=*_tmp253;_tmp255=_tmp254.shared;{
struct Cyc_List_List*_tmp256=_tmp255->delayed_constraint_checks;for(0;_tmp256 != 0;
_tmp256=_tmp256->tl){struct _tuple7 _tmp258;struct Cyc_RgnOrder_RgnPO*_tmp259;
struct Cyc_List_List*_tmp25A;struct Cyc_Position_Segment*_tmp25B;struct _tuple7*
_tmp257=(struct _tuple7*)_tmp256->hd;_tmp258=*_tmp257;_tmp259=_tmp258.f1;_tmp25A=
_tmp258.f2;_tmp25B=_tmp258.f3;if(!Cyc_RgnOrder_satisfies_constraints(_tmp259,
_tmp25A,(void*)2,1))({void*_tmp25C=0;Cyc_Tcutil_terr(_tmp25B,({const char*_tmp25D="the required region ordering is not satisfied here";
_tag_dyneither(_tmp25D,sizeof(char),51);}),_tag_dyneither(_tmp25C,sizeof(void*),
0));});}}}struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;
};struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Fndecl*fd){struct Cyc_Tcenv_Bindings*locals=0;struct Cyc_Absyn_Tvar*
rgn0=({struct Cyc_Absyn_Tvar*_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C->name=({
struct _dyneither_ptr*_tmp28D=_cycalloc(sizeof(*_tmp28D));_tmp28D[0]=(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp290;_tmp290.tag=0;_tmp290.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);{void*_tmp28E[1]={&
_tmp290};Cyc_aprintf(({const char*_tmp28F="`%s";_tag_dyneither(_tmp28F,sizeof(
char),4);}),_tag_dyneither(_tmp28E,sizeof(void*),1));}});_tmp28D;});_tmp28C->identity=
Cyc_Tcutil_new_tvar_id();_tmp28C->kind=(void*)((void*)& Cyc_Tcenv_rgn_kb);_tmp28C;});
struct Cyc_List_List*_tmp25E=({struct Cyc_List_List*_tmp28B=_cycalloc(sizeof(*
_tmp28B));_tmp28B->hd=rgn0;_tmp28B->tl=fd->tvs;_tmp28B;});Cyc_Tcutil_check_unique_tvars(
loc,_tmp25E);{struct Cyc_RgnOrder_RgnPO*_tmp25F=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,
fd->rgn_po,(void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,rgn0,loc);
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_struct*_tmp289=_cycalloc(sizeof(*
_tmp289));_tmp289[0]=({struct Cyc_Absyn_VarType_struct _tmp28A;_tmp28A.tag=1;
_tmp28A.f1=rgn0;_tmp28A;});_tmp289;});struct Cyc_Core_Opt*param_rgn_opt=({struct
Cyc_Core_Opt*_tmp288=_cycalloc(sizeof(*_tmp288));_tmp288->v=(void*)param_rgn;
_tmp288;});struct Cyc_List_List*_tmp260=0;{struct Cyc_List_List*_tmp261=fd->args;
for(0;_tmp261 != 0;_tmp261=_tmp261->tl){struct Cyc_Absyn_Vardecl*_tmp262=({struct
Cyc_Absyn_Vardecl*_tmp268=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp268[
0]=({struct Cyc_Absyn_Vardecl _tmp269;_tmp269.sc=(void*)((void*)2);_tmp269.name=({
struct _tuple1*_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A->f1=(union Cyc_Absyn_Nmspace_union)((
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp26B;(_tmp26B.Loc_n).tag=
0;_tmp26B;}));_tmp26A->f2=(*((struct _tuple8*)_tmp261->hd)).f1;_tmp26A;});_tmp269.tq=(*((
struct _tuple8*)_tmp261->hd)).f2;_tmp269.type=(void*)(*((struct _tuple8*)_tmp261->hd)).f3;
_tmp269.initializer=0;_tmp269.rgn=param_rgn_opt;_tmp269.attributes=0;_tmp269.escapes=
0;_tmp269;});_tmp268;});_tmp260=({struct Cyc_List_List*_tmp263=_cycalloc(sizeof(
struct Cyc_List_List)* 1);_tmp263[0]=({struct Cyc_List_List _tmp264;_tmp264.hd=
_tmp262;_tmp264.tl=_tmp260;_tmp264;});_tmp263;});locals=({struct Cyc_Tcenv_Bindings*
_tmp265=_region_malloc(r,sizeof(*_tmp265));_tmp265->v=(*((struct _tuple8*)_tmp261->hd)).f1;
_tmp265->b=(void*)((void*)({struct Cyc_Absyn_Param_b_struct*_tmp266=_cycalloc(
sizeof(*_tmp266));_tmp266[0]=({struct Cyc_Absyn_Param_b_struct _tmp267;_tmp267.tag=
2;_tmp267.f1=_tmp262;_tmp267;});_tmp266;}));_tmp265->tl=locals;_tmp265;});}}if(
fd->cyc_varargs != 0){struct Cyc_Core_Opt*_tmp26D;struct Cyc_Absyn_Tqual _tmp26E;
void*_tmp26F;int _tmp270;struct Cyc_Absyn_VarargInfo _tmp26C=*((struct Cyc_Absyn_VarargInfo*)
_check_null(fd->cyc_varargs));_tmp26D=_tmp26C.name;_tmp26E=_tmp26C.tq;_tmp26F=(
void*)_tmp26C.type;_tmp270=_tmp26C.inject;if(_tmp26D != 0){void*_tmp271=Cyc_Absyn_dyneither_typ(
_tmp26F,param_rgn,_tmp26E,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl*
_tmp272=({struct Cyc_Absyn_Vardecl*_tmp277=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1);_tmp277[0]=({struct Cyc_Absyn_Vardecl _tmp278;_tmp278.sc=(void*)((void*)2);
_tmp278.name=({struct _tuple1*_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279->f1=(
union Cyc_Absyn_Nmspace_union)((union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp27A;(_tmp27A.Loc_n).tag=0;_tmp27A;}));_tmp279->f2=(struct _dyneither_ptr*)
_tmp26D->v;_tmp279;});_tmp278.tq=Cyc_Absyn_empty_tqual(0);_tmp278.type=(void*)
_tmp271;_tmp278.initializer=0;_tmp278.rgn=param_rgn_opt;_tmp278.attributes=0;
_tmp278.escapes=0;_tmp278;});_tmp277;});_tmp260=({struct Cyc_List_List*_tmp273=
_cycalloc(sizeof(*_tmp273));_tmp273->hd=_tmp272;_tmp273->tl=_tmp260;_tmp273;});
locals=({struct Cyc_Tcenv_Bindings*_tmp274=_region_malloc(r,sizeof(*_tmp274));
_tmp274->v=(struct _dyneither_ptr*)_tmp26D->v;_tmp274->b=(void*)((void*)({struct
Cyc_Absyn_Param_b_struct*_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275[0]=({struct
Cyc_Absyn_Param_b_struct _tmp276;_tmp276.tag=2;_tmp276.f1=_tmp272;_tmp276;});
_tmp275;}));_tmp274->tl=locals;_tmp274;});}else{({void*_tmp27B=0;Cyc_Tcutil_terr(
loc,({const char*_tmp27C="missing name for varargs";_tag_dyneither(_tmp27C,
sizeof(char),25);}),_tag_dyneither(_tmp27B,sizeof(void*),0));});}}fd->param_vardecls=({
struct Cyc_Core_Opt*_tmp27D=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1);_tmp27D[0]=({
struct Cyc_Core_Opt _tmp27E;_tmp27E.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp260);_tmp27E;});_tmp27D;});return({struct Cyc_Tcenv_Fenv*
_tmp27F=_region_malloc(r,sizeof(*_tmp27F));_tmp27F->shared=(struct Cyc_Tcenv_SharedFenv*)({
struct Cyc_Tcenv_SharedFenv*_tmp287=_region_malloc(r,sizeof(*_tmp287));_tmp287->frgn=
r;_tmp287->return_typ=(void*)((void*)fd->ret_type);_tmp287->seen_labels=((struct
Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct
_dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp287->needed_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp);_tmp287->delayed_effect_checks=
0;_tmp287->delayed_constraint_checks=0;_tmp287;});_tmp27F->type_vars=(struct Cyc_List_List*)
_tmp25E;_tmp27F->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp25F;_tmp27F->locals=(
struct Cyc_Tcenv_Bindings*)locals;_tmp27F->encloser=(struct Cyc_Absyn_Stmt*)fd->body;
_tmp27F->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*_tmp286=
_region_malloc(r,sizeof(*_tmp286));_tmp286->ctrl_rgn=r;_tmp286->continue_stmt=(
void*)((void*)0);_tmp286->break_stmt=(void*)((void*)0);_tmp286->fallthru_clause=
0;_tmp286->next_stmt=(void*)((void*)2);_tmp286->try_depth=0;_tmp286;});_tmp27F->capability=(
void*)((void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp280=_cycalloc(sizeof(*
_tmp280));_tmp280[0]=({struct Cyc_Absyn_JoinEff_struct _tmp281;_tmp281.tag=20;
_tmp281.f1=({struct Cyc_List_List*_tmp282=_cycalloc(sizeof(*_tmp282));_tmp282->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp284=_cycalloc(sizeof(*
_tmp284));_tmp284[0]=({struct Cyc_Absyn_AccessEff_struct _tmp285;_tmp285.tag=19;
_tmp285.f1=(void*)param_rgn;_tmp285;});_tmp284;}));_tmp282->tl=({struct Cyc_List_List*
_tmp283=_cycalloc(sizeof(*_tmp283));_tmp283->hd=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(fd->effect))->v);_tmp283->tl=0;_tmp283;});_tmp282;});_tmp281;});
_tmp280;})));_tmp27F->curr_rgn=(void*)((void*)param_rgn);_tmp27F->in_notreadctxt=(
int)0;_tmp27F;});}}struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){struct Cyc_Tcenv_Fenv
_tmp292;struct Cyc_Tcenv_Bindings*_tmp293;struct Cyc_RgnOrder_RgnPO*_tmp294;struct
Cyc_List_List*_tmp295;struct Cyc_Tcenv_SharedFenv*_tmp296;struct Cyc_Tcenv_Fenv*
_tmp291=old_fenv;_tmp292=*_tmp291;_tmp293=_tmp292.locals;_tmp294=_tmp292.region_order;
_tmp295=_tmp292.type_vars;_tmp296=_tmp292.shared;{struct _RegionHandle*_tmp297=
_tmp296->frgn;struct Cyc_Tcenv_Bindings*_tmp298=(struct Cyc_Tcenv_Bindings*)
_tmp293;struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmp2C9=_cycalloc(
sizeof(*_tmp2C9));_tmp2C9->name=({struct _dyneither_ptr*_tmp2CA=_cycalloc(sizeof(*
_tmp2CA));_tmp2CA[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp2CD;
_tmp2CD.tag=0;_tmp2CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);{
void*_tmp2CB[1]={& _tmp2CD};Cyc_aprintf(({const char*_tmp2CC="`%s";_tag_dyneither(
_tmp2CC,sizeof(char),4);}),_tag_dyneither(_tmp2CB,sizeof(void*),1));}});_tmp2CA;});
_tmp2C9->identity=Cyc_Tcutil_new_tvar_id();_tmp2C9->kind=(void*)((void*)& Cyc_Tcenv_rgn_kb);
_tmp2C9;});{struct Cyc_List_List*_tmp299=fd->tvs;for(0;_tmp299 != 0;_tmp299=
_tmp299->tl){if(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp299->hd)== (void*)
3)_tmp294=Cyc_RgnOrder_add_youngest(_tmp297,_tmp294,(struct Cyc_Absyn_Tvar*)
_tmp299->hd,0,0);else{if(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp299->hd)
== (void*)4  || Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp299->hd)== (void*)
5)({void*_tmp29A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp29B="non-intuitionistic tvar in nested_fenv";_tag_dyneither(
_tmp29B,sizeof(char),39);}),_tag_dyneither(_tmp29A,sizeof(void*),0));});}}}
_tmp294=Cyc_RgnOrder_add_youngest(_tmp297,_tmp294,rgn0,0,0);{struct Cyc_List_List*
_tmp29C=({struct Cyc_List_List*_tmp2C8=_cycalloc(sizeof(*_tmp2C8));_tmp2C8->hd=
rgn0;_tmp2C8->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(fd->tvs,_tmp295);_tmp2C8;});Cyc_Tcutil_check_unique_tvars(
loc,_tmp29C);{void*param_rgn=(void*)({struct Cyc_Absyn_VarType_struct*_tmp2C6=
_cycalloc(sizeof(*_tmp2C6));_tmp2C6[0]=({struct Cyc_Absyn_VarType_struct _tmp2C7;
_tmp2C7.tag=1;_tmp2C7.f1=rgn0;_tmp2C7;});_tmp2C6;});struct Cyc_Core_Opt*
param_rgn_opt=({struct Cyc_Core_Opt*_tmp2C5=_cycalloc(sizeof(*_tmp2C5));_tmp2C5->v=(
void*)param_rgn;_tmp2C5;});struct Cyc_List_List*_tmp29D=0;{struct Cyc_List_List*
_tmp29E=fd->args;for(0;_tmp29E != 0;_tmp29E=_tmp29E->tl){struct Cyc_Absyn_Vardecl*
_tmp29F=({struct Cyc_Absyn_Vardecl*_tmp2A5=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1);_tmp2A5[0]=({struct Cyc_Absyn_Vardecl _tmp2A6;_tmp2A6.sc=(void*)((void*)2);
_tmp2A6.name=({struct _tuple1*_tmp2A7=_cycalloc(sizeof(*_tmp2A7));_tmp2A7->f1=(
union Cyc_Absyn_Nmspace_union)((union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp2A8;(_tmp2A8.Loc_n).tag=0;_tmp2A8;}));_tmp2A7->f2=(*((struct _tuple8*)_tmp29E->hd)).f1;
_tmp2A7;});_tmp2A6.tq=(*((struct _tuple8*)_tmp29E->hd)).f2;_tmp2A6.type=(void*)(*((
struct _tuple8*)_tmp29E->hd)).f3;_tmp2A6.initializer=0;_tmp2A6.rgn=param_rgn_opt;
_tmp2A6.attributes=0;_tmp2A6.escapes=0;_tmp2A6;});_tmp2A5;});_tmp29D=({struct Cyc_List_List*
_tmp2A0=_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp2A0[0]=({struct Cyc_List_List
_tmp2A1;_tmp2A1.hd=_tmp29F;_tmp2A1.tl=_tmp29D;_tmp2A1;});_tmp2A0;});_tmp298=(
struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp2A2=_region_malloc(
_tmp297,sizeof(*_tmp2A2));_tmp2A2->v=(*((struct _tuple8*)_tmp29E->hd)).f1;_tmp2A2->b=(
void*)((void*)({struct Cyc_Absyn_Param_b_struct*_tmp2A3=_cycalloc(sizeof(*_tmp2A3));
_tmp2A3[0]=({struct Cyc_Absyn_Param_b_struct _tmp2A4;_tmp2A4.tag=2;_tmp2A4.f1=
_tmp29F;_tmp2A4;});_tmp2A3;}));_tmp2A2->tl=_tmp298;_tmp2A2;});}}if(fd->cyc_varargs
!= 0){struct Cyc_Core_Opt*_tmp2AA;struct Cyc_Absyn_Tqual _tmp2AB;void*_tmp2AC;int
_tmp2AD;struct Cyc_Absyn_VarargInfo _tmp2A9=*((struct Cyc_Absyn_VarargInfo*)
_check_null(fd->cyc_varargs));_tmp2AA=_tmp2A9.name;_tmp2AB=_tmp2A9.tq;_tmp2AC=(
void*)_tmp2A9.type;_tmp2AD=_tmp2A9.inject;if(_tmp2AA != 0){void*_tmp2AE=Cyc_Absyn_dyneither_typ(
_tmp2AC,param_rgn,_tmp2AB,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl*
_tmp2AF=({struct Cyc_Absyn_Vardecl*_tmp2B4=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1);_tmp2B4[0]=({struct Cyc_Absyn_Vardecl _tmp2B5;_tmp2B5.sc=(void*)((void*)2);
_tmp2B5.name=({struct _tuple1*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6->f1=(
union Cyc_Absyn_Nmspace_union)((union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp2B7;(_tmp2B7.Loc_n).tag=0;_tmp2B7;}));_tmp2B6->f2=(struct _dyneither_ptr*)
_tmp2AA->v;_tmp2B6;});_tmp2B5.tq=Cyc_Absyn_empty_tqual(0);_tmp2B5.type=(void*)
_tmp2AE;_tmp2B5.initializer=0;_tmp2B5.rgn=param_rgn_opt;_tmp2B5.attributes=0;
_tmp2B5.escapes=0;_tmp2B5;});_tmp2B4;});_tmp29D=({struct Cyc_List_List*_tmp2B0=
_cycalloc(sizeof(*_tmp2B0));_tmp2B0->hd=_tmp2AF;_tmp2B0->tl=_tmp29D;_tmp2B0;});
_tmp298=(struct Cyc_Tcenv_Bindings*)({struct Cyc_Tcenv_Bindings*_tmp2B1=
_region_malloc(_tmp297,sizeof(*_tmp2B1));_tmp2B1->v=(struct _dyneither_ptr*)
_tmp2AA->v;_tmp2B1->b=(void*)((void*)({struct Cyc_Absyn_Param_b_struct*_tmp2B2=
_cycalloc(sizeof(*_tmp2B2));_tmp2B2[0]=({struct Cyc_Absyn_Param_b_struct _tmp2B3;
_tmp2B3.tag=2;_tmp2B3.f1=_tmp2AF;_tmp2B3;});_tmp2B2;}));_tmp2B1->tl=_tmp298;
_tmp2B1;});}else{({void*_tmp2B8=0;Cyc_Tcutil_terr(loc,({const char*_tmp2B9="missing name for varargs";
_tag_dyneither(_tmp2B9,sizeof(char),25);}),_tag_dyneither(_tmp2B8,sizeof(void*),
0));});}}fd->param_vardecls=({struct Cyc_Core_Opt*_tmp2BA=_cycalloc(sizeof(struct
Cyc_Core_Opt)* 1);_tmp2BA[0]=({struct Cyc_Core_Opt _tmp2BB;_tmp2BB.v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp29D);_tmp2BB;});_tmp2BA;});return(
struct Cyc_Tcenv_Fenv*)({struct Cyc_Tcenv_Fenv*_tmp2BC=_region_malloc(_tmp297,
sizeof(*_tmp2BC));_tmp2BC->shared=(struct Cyc_Tcenv_SharedFenv*)({struct Cyc_Tcenv_SharedFenv*
_tmp2C4=_region_malloc(_tmp297,sizeof(*_tmp2C4));_tmp2C4->frgn=_tmp297;_tmp2C4->return_typ=(
void*)((void*)fd->ret_type);_tmp2C4->seen_labels=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
_tmp297,Cyc_strptrcmp);_tmp2C4->needed_labels=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
_tmp297,Cyc_strptrcmp);_tmp2C4->delayed_effect_checks=0;_tmp2C4->delayed_constraint_checks=
0;_tmp2C4;});_tmp2BC->type_vars=(struct Cyc_List_List*)_tmp29C;_tmp2BC->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp294;_tmp2BC->locals=(struct Cyc_Tcenv_Bindings*)
_tmp298;_tmp2BC->encloser=(struct Cyc_Absyn_Stmt*)fd->body;_tmp2BC->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)({struct Cyc_Tcenv_CtrlEnv*
_tmp2C3=_region_malloc(_tmp297,sizeof(*_tmp2C3));_tmp2C3->ctrl_rgn=_tmp297;
_tmp2C3->continue_stmt=(void*)((void*)0);_tmp2C3->break_stmt=(void*)((void*)0);
_tmp2C3->fallthru_clause=0;_tmp2C3->next_stmt=(void*)((void*)2);_tmp2C3->try_depth=
0;_tmp2C3;}));_tmp2BC->capability=(void*)((void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp2BD=_cycalloc(sizeof(*_tmp2BD));_tmp2BD[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp2BE;_tmp2BE.tag=20;_tmp2BE.f1=({struct Cyc_List_List*_tmp2BF=_cycalloc(
sizeof(*_tmp2BF));_tmp2BF->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp2C1=_cycalloc(sizeof(*_tmp2C1));_tmp2C1[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp2C2;_tmp2C2.tag=19;_tmp2C2.f1=(void*)param_rgn;_tmp2C2;});_tmp2C1;}));
_tmp2BF->tl=({struct Cyc_List_List*_tmp2C0=_cycalloc(sizeof(*_tmp2C0));_tmp2C0->hd=(
void*)((void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v);_tmp2C0->tl=0;
_tmp2C0;});_tmp2BF;});_tmp2BE;});_tmp2BD;})));_tmp2BC->curr_rgn=(void*)((void*)
param_rgn);_tmp2BC->in_notreadctxt=(int)0;_tmp2BC;});}}}}
