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
void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),
struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[8];int Cyc_List_list_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*
Cyc_Position_segment_join(struct Cyc_Position_Segment*,struct Cyc_Position_Segment*);
struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];
struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple0{union
Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
loc;};struct _union_Constraint_Eq_constr{int tag;void*val;};struct
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
struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;
struct _tuple0*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);union
Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,
struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU
field_info;struct Cyc_List_List*targs;};struct _tuple2{void*f1;struct _tuple0*f2;
struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2
val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};
union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct
_union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
void*,struct _tuple0*,struct Cyc_Core_Opt*);union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(
struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU
aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;
struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_Evar_struct{int tag;
struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};struct
Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{
int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;void*f1;
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
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;union
Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct
_tuple3{void*f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};
struct _tuple4{void*f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4
val;};struct _tuple5{void*f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5
val;};struct _tuple6{void*f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;
struct _tuple6 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};
struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;union
Cyc_Absyn_Cnst Cyc_Absyn_Char_c(void*,char);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(
void*,short);union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(void*,int);union Cyc_Absyn_Cnst
Cyc_Absyn_LongLong_c(void*,long long);union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(
struct _dyneither_ptr);union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr);
struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;
struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{int tag;
struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;unsigned int
f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**
elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;};struct
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
int tag;struct Cyc_List_List*f1;};struct _tuple7{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple7*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
_tuple8{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple8 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple8 f2;struct _tuple8 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple8 f2;};struct Cyc_Absyn_TryCatch_s_struct{
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
struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{void*sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct
_tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;void*ret_type;
struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;
struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};
struct Cyc_Absyn_Datatypedecl{void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;
struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct
_tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};char Cyc_Absyn_EmptyAnnot[15]="\000\000\000\000EmptyAnnot\000";
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*
x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();union Cyc_Absyn_Constraint*
Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_val(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);extern union
Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_force_kb(
void*kb);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);void*Cyc_Absyn_int_typ(void*,void*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_float_typ;
void*Cyc_Absyn_double_typ(int);extern void*Cyc_Absyn_empty_effect;extern struct
_tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
extern void*Cyc_Absyn_exn_typ;extern struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar;void*Cyc_Absyn_string_typ(
void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_file_typ();
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,
union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_star_typ(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple0*name);void*Cyc_Absyn_unionq_typ(struct
_tuple0*name);void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name);void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct
Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*,struct Cyc_List_List*es,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct
Cyc_Absyn_Exp*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _dyneither_ptr*lab,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,
struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(
struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int
tagged);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple0*n,
struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(void*
s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct
Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_datatype_decl(void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_extensible,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);void*Cyc_Absyn_pointer_expand(void*,int
fresh_evar);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple9*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*,int);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
int Cyc_Absyn_fntype_att(void*a);struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
struct _tuple10{void*f1;struct _tuple0*f2;};struct _tuple10 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_nontagged_union_type(
void*);int Cyc_Absyn_is_aggr_type(void*t);extern int Cyc_Absyn_porting_c_code;
extern int Cyc_Absyn_no_regions;struct Cyc_Typerep_Int_struct{int tag;int f1;
unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;void*f2;
};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple11{unsigned int f1;
struct _dyneither_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{int tag;struct
_dyneither_ptr*f1;unsigned int f2;struct _dyneither_ptr f3;};struct _tuple12{
unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;
struct _dyneither_ptr f2;};struct _tuple13{unsigned int f1;struct _dyneither_ptr f2;};
struct Cyc_Typerep_TUnion_struct{int tag;struct _dyneither_ptr f1;struct
_dyneither_ptr f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;unsigned int f3;struct
_dyneither_ptr f4;};struct _tuple14{struct _dyneither_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
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
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Iter_Iter{void*env;int(*next)(void*
env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct _tuple15{void*f1;void*f2;};struct _tuple15*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple15*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
_dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);void*Cyc_Tcutil_kind_to_bound(
void*k);struct _tuple16{struct Cyc_Dict_Dict f1;int f2;};struct _tuple17{struct
_dyneither_ptr f1;int f2;};extern int Cyc_Cyclone_tovc_r;extern void*Cyc_Cyclone_c_compiler;
int Cyc_Cyclone_tovc_r=0;void*Cyc_Cyclone_c_compiler=(void*)0;static int Cyc_Absyn_strlist_cmp(
struct Cyc_List_List*ss1,struct Cyc_List_List*ss2);static int Cyc_Absyn_strlist_cmp(
struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){return((int(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_strptrcmp,ss1,ss2);}int Cyc_Absyn_varlist_cmp(struct
Cyc_List_List*vs1,struct Cyc_List_List*vs2);int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*
vs1,struct Cyc_List_List*vs2){if((int)vs1 == (int)vs2)return 0;return Cyc_Absyn_strlist_cmp(
vs1,vs2);}struct _tuple18{union Cyc_Absyn_Nmspace f1;union Cyc_Absyn_Nmspace f2;};int
Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2);int Cyc_Absyn_qvar_cmp(
struct _tuple0*q1,struct _tuple0*q2){union Cyc_Absyn_Nmspace _tmp0=(*q1).f1;union Cyc_Absyn_Nmspace
_tmp1=(*q2).f1;{struct _tuple18 _tmp1B1;struct _tuple18 _tmp3=(_tmp1B1.f1=_tmp0,((
_tmp1B1.f2=_tmp1,_tmp1B1)));union Cyc_Absyn_Nmspace _tmp4;int _tmp5;union Cyc_Absyn_Nmspace
_tmp6;int _tmp7;union Cyc_Absyn_Nmspace _tmp8;struct Cyc_List_List*_tmp9;union Cyc_Absyn_Nmspace
_tmpA;struct Cyc_List_List*_tmpB;union Cyc_Absyn_Nmspace _tmpC;struct Cyc_List_List*
_tmpD;union Cyc_Absyn_Nmspace _tmpE;struct Cyc_List_List*_tmpF;union Cyc_Absyn_Nmspace
_tmp10;int _tmp11;union Cyc_Absyn_Nmspace _tmp12;int _tmp13;union Cyc_Absyn_Nmspace
_tmp14;struct Cyc_List_List*_tmp15;union Cyc_Absyn_Nmspace _tmp16;struct Cyc_List_List*
_tmp17;_LL1: _tmp4=_tmp3.f1;if((_tmp4.Loc_n).tag != 3)goto _LL3;_tmp5=(int)(_tmp4.Loc_n).val;
_tmp6=_tmp3.f2;if((_tmp6.Loc_n).tag != 3)goto _LL3;_tmp7=(int)(_tmp6.Loc_n).val;
_LL2: goto _LL0;_LL3: _tmp8=_tmp3.f1;if((_tmp8.Rel_n).tag != 1)goto _LL5;_tmp9=(
struct Cyc_List_List*)(_tmp8.Rel_n).val;_tmpA=_tmp3.f2;if((_tmpA.Rel_n).tag != 1)
goto _LL5;_tmpB=(struct Cyc_List_List*)(_tmpA.Rel_n).val;_LL4: _tmpD=_tmp9;_tmpF=
_tmpB;goto _LL6;_LL5: _tmpC=_tmp3.f1;if((_tmpC.Abs_n).tag != 2)goto _LL7;_tmpD=(
struct Cyc_List_List*)(_tmpC.Abs_n).val;_tmpE=_tmp3.f2;if((_tmpE.Abs_n).tag != 2)
goto _LL7;_tmpF=(struct Cyc_List_List*)(_tmpE.Abs_n).val;_LL6: {int i=Cyc_Absyn_strlist_cmp(
_tmpD,_tmpF);if(i != 0)return i;goto _LL0;}_LL7: _tmp10=_tmp3.f1;if((_tmp10.Loc_n).tag
!= 3)goto _LL9;_tmp11=(int)(_tmp10.Loc_n).val;_LL8: return - 1;_LL9: _tmp12=_tmp3.f2;
if((_tmp12.Loc_n).tag != 3)goto _LLB;_tmp13=(int)(_tmp12.Loc_n).val;_LLA: return 1;
_LLB: _tmp14=_tmp3.f1;if((_tmp14.Rel_n).tag != 1)goto _LLD;_tmp15=(struct Cyc_List_List*)(
_tmp14.Rel_n).val;_LLC: return - 1;_LLD: _tmp16=_tmp3.f2;if((_tmp16.Rel_n).tag != 1)
goto _LL0;_tmp17=(struct Cyc_List_List*)(_tmp16.Rel_n).val;_LLE: return 1;_LL0:;}
return Cyc_strptrcmp((*q1).f2,(*q2).f2);}int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*
tv1,struct Cyc_Absyn_Tvar*tv2);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,
struct Cyc_Absyn_Tvar*tv2){int i=Cyc_strptrcmp(tv1->name,tv2->name);if(i != 0)
return i;return tv1->identity - tv2->identity;}union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n={.Loc_n={
3,0}};union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*x);union Cyc_Absyn_Nmspace
Cyc_Absyn_Abs_n(struct Cyc_List_List*x){union Cyc_Absyn_Nmspace _tmp1B2;return((
_tmp1B2.Abs_n).val=x,(((_tmp1B2.Abs_n).tag=2,_tmp1B2)));}union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*x);union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(
struct Cyc_List_List*x){union Cyc_Absyn_Nmspace _tmp1B3;return((_tmp1B3.Rel_n).val=
x,(((_tmp1B3.Rel_n).tag=1,_tmp1B3)));}union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={
1,0}};int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv);int Cyc_Absyn_is_qvar_qualified(
struct _tuple0*qv){union Cyc_Absyn_Nmspace _tmp1A=(*qv).f1;struct Cyc_List_List*
_tmp1B;struct Cyc_List_List*_tmp1C;int _tmp1D;_LL10: if((_tmp1A.Rel_n).tag != 1)goto
_LL12;_tmp1B=(struct Cyc_List_List*)(_tmp1A.Rel_n).val;if(_tmp1B != 0)goto _LL12;
_LL11: goto _LL13;_LL12: if((_tmp1A.Abs_n).tag != 2)goto _LL14;_tmp1C=(struct Cyc_List_List*)(
_tmp1A.Abs_n).val;if(_tmp1C != 0)goto _LL14;_LL13: goto _LL15;_LL14: if((_tmp1A.Loc_n).tag
!= 3)goto _LL16;_tmp1D=(int)(_tmp1A.Loc_n).val;_LL15: return 0;_LL16:;_LL17: return 1;
_LLF:;}static int Cyc_Absyn_new_type_counter=0;void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*env);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*
env){struct Cyc_Absyn_Evar_struct _tmp1B6;struct Cyc_Absyn_Evar_struct*_tmp1B5;
return(void*)((_tmp1B5=_cycalloc(sizeof(*_tmp1B5)),((_tmp1B5[0]=((_tmp1B6.tag=0,((
_tmp1B6.f1=k,((_tmp1B6.f2=0,((_tmp1B6.f3=Cyc_Absyn_new_type_counter ++,((_tmp1B6.f4=
env,_tmp1B6)))))))))),_tmp1B5))));}static struct Cyc_Core_Opt Cyc_Absyn_mk={(void*)((
void*)1)};void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(
struct Cyc_Core_Opt*tenv){return Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Absyn_mk,
tenv);}struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,
struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual
x,struct Cyc_Absyn_Tqual y){struct Cyc_Absyn_Tqual _tmp1B7;return(_tmp1B7.print_const=
x.print_const  || y.print_const,((_tmp1B7.q_volatile=x.q_volatile  || y.q_volatile,((
_tmp1B7.q_restrict=x.q_restrict  || y.q_restrict,((_tmp1B7.real_const=x.real_const
 || y.real_const,((_tmp1B7.loc=Cyc_Position_segment_join(x.loc,y.loc),_tmp1B7)))))))));}
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Tqual _tmp1B8;return(_tmp1B8.print_const=0,((_tmp1B8.q_volatile=0,((
_tmp1B8.q_restrict=0,((_tmp1B8.real_const=0,((_tmp1B8.loc=loc,_tmp1B8)))))))));}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Tqual _tmp1B9;return(_tmp1B9.print_const=1,((_tmp1B9.q_volatile=0,((
_tmp1B9.q_restrict=0,((_tmp1B9.real_const=1,((_tmp1B9.loc=loc,_tmp1B9)))))))));}
static struct Cyc_Absyn_Const_e_struct Cyc_Absyn_one_b_raw={0,{.Int_c={4,{(void*)1,
1}}}};struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={0,(void*)& Cyc_Absyn_one_b_raw,
0,(void*)Cyc_Absyn_EmptyAnnot};struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=&
Cyc_Absyn_exp_unsigned_one_v;static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={
0,& Cyc_Absyn_exp_unsigned_one_v};void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo
udi);union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo
udi){union Cyc_Absyn_DatatypeInfoU _tmp1BA;return((_tmp1BA.UnknownDatatype).val=
udi,(((_tmp1BA.UnknownDatatype).tag=1,_tmp1BA)));}union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d);extern union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){union Cyc_Absyn_DatatypeInfoU
_tmp1BB;return((_tmp1BB.KnownDatatype).val=d,(((_tmp1BB.KnownDatatype).tag=2,
_tmp1BB)));}union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(
struct Cyc_Absyn_UnknownDatatypeFieldInfo s);union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(
struct Cyc_Absyn_UnknownDatatypeFieldInfo s){union Cyc_Absyn_DatatypeFieldInfoU
_tmp1BC;return((_tmp1BC.UnknownDatatypefield).val=s,(((_tmp1BC.UnknownDatatypefield).tag=
1,_tmp1BC)));}union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(
struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df);union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*
df){struct _tuple1 _tmp1BF;union Cyc_Absyn_DatatypeFieldInfoU _tmp1BE;return((
_tmp1BE.KnownDatatypefield).val=((_tmp1BF.f1=dd,((_tmp1BF.f2=df,_tmp1BF)))),(((
_tmp1BE.KnownDatatypefield).tag=2,_tmp1BE)));}union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
void*ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged);union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
void*ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){struct _tuple2 _tmp1C2;union Cyc_Absyn_AggrInfoU
_tmp1C1;return((_tmp1C1.UnknownAggr).val=((_tmp1C2.f1=ak,((_tmp1C2.f2=n,((
_tmp1C2.f3=tagged,_tmp1C2)))))),(((_tmp1C1.UnknownAggr).tag=1,_tmp1C1)));}union
Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad);union Cyc_Absyn_AggrInfoU
Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){union Cyc_Absyn_AggrInfoU
_tmp1C3;return((_tmp1C3.KnownAggr).val=ad,(((_tmp1C3.KnownAggr).tag=2,_tmp1C3)));}
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);union Cyc_Absyn_Constraint*
Cyc_Absyn_new_conref(void*x){union Cyc_Absyn_Constraint*_tmp1C4;return(_tmp1C4=
_cycalloc(sizeof(*_tmp1C4)),(((_tmp1C4->Eq_constr).val=(void*)x,(((_tmp1C4->Eq_constr).tag=
1,_tmp1C4)))));}union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();union Cyc_Absyn_Constraint*
Cyc_Absyn_empty_conref(){union Cyc_Absyn_Constraint*_tmp1C5;return(_tmp1C5=
_cycalloc_atomic(sizeof(*_tmp1C5)),(((_tmp1C5->No_constr).val=0,(((_tmp1C5->No_constr).tag=
3,_tmp1C5)))));}static union Cyc_Absyn_Constraint Cyc_Absyn_true_conref_v={.Eq_constr={
1,(void*)1}};union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_false_conref_v={.Eq_constr={1,(void*)0}};
union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_one_conref_v={.Eq_constr={1,(
void*)((void*)& Cyc_Absyn_one_bt)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref=&
Cyc_Absyn_bounds_one_conref_v;static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_dyneither_conref_v={.Eq_constr={
1,(void*)((void*)0)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref=&
Cyc_Absyn_bounds_dyneither_conref_v;union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(
union Cyc_Absyn_Constraint*x);union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(
union Cyc_Absyn_Constraint*x){union Cyc_Absyn_Constraint*_tmp2F=x;union Cyc_Absyn_Constraint
_tmp30;int _tmp31;union Cyc_Absyn_Constraint _tmp32;void*_tmp33;union Cyc_Absyn_Constraint
_tmp34;union Cyc_Absyn_Constraint*_tmp35;_LL19: _tmp30=*_tmp2F;if((_tmp30.No_constr).tag
!= 3)goto _LL1B;_tmp31=(int)(_tmp30.No_constr).val;_LL1A: goto _LL1C;_LL1B: _tmp32=*
_tmp2F;if((_tmp32.Eq_constr).tag != 1)goto _LL1D;_tmp33=(void*)(_tmp32.Eq_constr).val;
_LL1C: return x;_LL1D: _tmp34=*_tmp2F;if((_tmp34.Forward_constr).tag != 2)goto _LL18;
_tmp35=(union Cyc_Absyn_Constraint*)(_tmp34.Forward_constr).val;_LL1E: {union Cyc_Absyn_Constraint*
_tmp36=Cyc_Absyn_compress_conref(_tmp35);{struct _union_Constraint_Forward_constr*
_tmp1C6;(_tmp1C6=& x->Forward_constr,((_tmp1C6->tag=2,_tmp1C6->val=_tmp36)));}
return _tmp36;}_LL18:;}void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x){union Cyc_Absyn_Constraint*
_tmp38=Cyc_Absyn_compress_conref(x);union Cyc_Absyn_Constraint _tmp39;void*_tmp3A;
_LL20: _tmp39=*_tmp38;if((_tmp39.Eq_constr).tag != 1)goto _LL22;_tmp3A=(void*)(
_tmp39.Eq_constr).val;_LL21: return _tmp3A;_LL22:;_LL23: {const char*_tmp1C9;void*
_tmp1C8;(_tmp1C8=0,Cyc_Tcutil_impos(((_tmp1C9="conref_val",_tag_dyneither(
_tmp1C9,sizeof(char),11))),_tag_dyneither(_tmp1C8,sizeof(void*),0)));}_LL1F:;}
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x){union Cyc_Absyn_Constraint*_tmp3D=Cyc_Absyn_compress_conref(
x);union Cyc_Absyn_Constraint _tmp3E;void*_tmp3F;_LL25: _tmp3E=*_tmp3D;if((_tmp3E.Eq_constr).tag
!= 1)goto _LL27;_tmp3F=(void*)(_tmp3E.Eq_constr).val;_LL26: return _tmp3F;_LL27:;
_LL28: return y;_LL24:;}void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x){x=Cyc_Absyn_compress_conref(
x);{union Cyc_Absyn_Constraint*_tmp40=x;union Cyc_Absyn_Constraint _tmp41;void*
_tmp42;_LL2A: _tmp41=*_tmp40;if((_tmp41.Eq_constr).tag != 1)goto _LL2C;_tmp42=(void*)(
_tmp41.Eq_constr).val;_LL2B: return _tmp42;_LL2C:;_LL2D:{struct
_union_Constraint_Eq_constr*_tmp1CA;(_tmp1CA=& x->Eq_constr,((_tmp1CA->tag=1,
_tmp1CA->val=y)));}return y;_LL29:;}}void*Cyc_Absyn_compress_kb(void*k);void*Cyc_Absyn_compress_kb(
void*k){void*_tmp44=k;struct Cyc_Core_Opt*_tmp45;struct Cyc_Core_Opt*_tmp46;struct
Cyc_Core_Opt*_tmp47;struct Cyc_Core_Opt _tmp48;void*_tmp49;void**_tmp4A;struct Cyc_Core_Opt*
_tmp4B;struct Cyc_Core_Opt _tmp4C;void*_tmp4D;void**_tmp4E;_LL2F: if(*((int*)_tmp44)
!= 0)goto _LL31;_LL30: goto _LL32;_LL31: if(*((int*)_tmp44)!= 1)goto _LL33;_tmp45=((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp44)->f1;if(_tmp45 != 0)goto _LL33;_LL32:
goto _LL34;_LL33: if(*((int*)_tmp44)!= 2)goto _LL35;_tmp46=((struct Cyc_Absyn_Less_kb_struct*)
_tmp44)->f1;if(_tmp46 != 0)goto _LL35;_LL34: return k;_LL35: if(*((int*)_tmp44)!= 1)
goto _LL37;_tmp47=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp44)->f1;if(_tmp47 == 0)
goto _LL37;_tmp48=*_tmp47;_tmp49=(void*)_tmp48.v;_tmp4A=(void**)&(*((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp44)->f1).v;_LL36: _tmp4E=_tmp4A;goto _LL38;_LL37: if(*((int*)_tmp44)!= 2)goto
_LL2E;_tmp4B=((struct Cyc_Absyn_Less_kb_struct*)_tmp44)->f1;if(_tmp4B == 0)goto
_LL2E;_tmp4C=*_tmp4B;_tmp4D=(void*)_tmp4C.v;_tmp4E=(void**)&(*((struct Cyc_Absyn_Less_kb_struct*)
_tmp44)->f1).v;_LL38:*_tmp4E=Cyc_Absyn_compress_kb(*_tmp4E);return*_tmp4E;_LL2E:;}
void*Cyc_Absyn_force_kb(void*kb);void*Cyc_Absyn_force_kb(void*kb){void*_tmp4F=
Cyc_Absyn_compress_kb(kb);void*_tmp50;struct Cyc_Core_Opt*_tmp51;struct Cyc_Core_Opt**
_tmp52;struct Cyc_Core_Opt*_tmp53;struct Cyc_Core_Opt**_tmp54;void*_tmp55;_LL3A:
if(*((int*)_tmp4F)!= 0)goto _LL3C;_tmp50=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp4F)->f1;_LL3B: return _tmp50;_LL3C: if(*((int*)_tmp4F)!= 1)goto _LL3E;_tmp51=((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp4F)->f1;_tmp52=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp4F)->f1;_LL3D: _tmp54=_tmp52;_tmp55=(void*)
2;goto _LL3F;_LL3E: if(*((int*)_tmp4F)!= 2)goto _LL39;_tmp53=((struct Cyc_Absyn_Less_kb_struct*)
_tmp4F)->f1;_tmp54=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp4F)->f1;_tmp55=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4F)->f2;_LL3F:{
struct Cyc_Core_Opt*_tmp1CB;*_tmp54=((_tmp1CB=_cycalloc(sizeof(*_tmp1CB)),((
_tmp1CB->v=(void*)Cyc_Tcutil_kind_to_bound(_tmp55),_tmp1CB))));}return _tmp55;
_LL39:;}static struct Cyc_Absyn_IntType_struct Cyc_Absyn_char_tt={5,(void*)((void*)
2),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_uchar_tt={5,(
void*)((void*)1),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ushort_tt={
5,(void*)((void*)1),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_uint_tt={
5,(void*)((void*)1),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ulong_tt={
5,(void*)((void*)1),(void*)((void*)3)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ulonglong_tt={
5,(void*)((void*)1),(void*)((void*)4)};void*Cyc_Absyn_char_typ=(void*)& Cyc_Absyn_char_tt;
void*Cyc_Absyn_uchar_typ=(void*)& Cyc_Absyn_uchar_tt;void*Cyc_Absyn_ushort_typ=(
void*)& Cyc_Absyn_ushort_tt;void*Cyc_Absyn_uint_typ=(void*)& Cyc_Absyn_uint_tt;
void*Cyc_Absyn_ulong_typ=(void*)& Cyc_Absyn_ulong_tt;void*Cyc_Absyn_ulonglong_typ=(
void*)& Cyc_Absyn_ulonglong_tt;static struct Cyc_Absyn_IntType_struct Cyc_Absyn_schar_tt={
5,(void*)((void*)0),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_sshort_tt={
5,(void*)((void*)0),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_sint_tt={
5,(void*)((void*)0),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_slong_tt={
5,(void*)((void*)0),(void*)((void*)3)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_slonglong_tt={
5,(void*)((void*)0),(void*)((void*)4)};void*Cyc_Absyn_schar_typ=(void*)& Cyc_Absyn_schar_tt;
void*Cyc_Absyn_sshort_typ=(void*)& Cyc_Absyn_sshort_tt;void*Cyc_Absyn_sint_typ=(
void*)& Cyc_Absyn_sint_tt;void*Cyc_Absyn_slong_typ=(void*)& Cyc_Absyn_slong_tt;
void*Cyc_Absyn_slonglong_typ=(void*)& Cyc_Absyn_slonglong_tt;static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nshort_tt={5,(void*)((void*)2),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nint_tt={5,(void*)((void*)2),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nlong_tt={5,(void*)((void*)2),(void*)((void*)3)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nlonglong_tt={5,(void*)((void*)2),(void*)((void*)4)};void*Cyc_Absyn_nshort_typ=(
void*)& Cyc_Absyn_nshort_tt;void*Cyc_Absyn_nint_typ=(void*)& Cyc_Absyn_nint_tt;
void*Cyc_Absyn_nlong_typ=(void*)& Cyc_Absyn_nlong_tt;void*Cyc_Absyn_nlonglong_typ=(
void*)& Cyc_Absyn_nlonglong_tt;void*Cyc_Absyn_int_typ(void*sn,void*sz);void*Cyc_Absyn_int_typ(
void*sn,void*sz){void*_tmp66=sn;_LL41: if((int)_tmp66 != 0)goto _LL43;_LL42: {void*
_tmp67=sz;_LL48: if((int)_tmp67 != 0)goto _LL4A;_LL49: return Cyc_Absyn_schar_typ;
_LL4A: if((int)_tmp67 != 1)goto _LL4C;_LL4B: return Cyc_Absyn_sshort_typ;_LL4C: if((
int)_tmp67 != 2)goto _LL4E;_LL4D: return Cyc_Absyn_sint_typ;_LL4E: if((int)_tmp67 != 3)
goto _LL50;_LL4F: return Cyc_Absyn_slong_typ;_LL50: if((int)_tmp67 != 4)goto _LL47;
_LL51: return Cyc_Absyn_slonglong_typ;_LL47:;}_LL43: if((int)_tmp66 != 1)goto _LL45;
_LL44: {void*_tmp68=sz;_LL53: if((int)_tmp68 != 0)goto _LL55;_LL54: return Cyc_Absyn_uchar_typ;
_LL55: if((int)_tmp68 != 1)goto _LL57;_LL56: return Cyc_Absyn_ushort_typ;_LL57: if((
int)_tmp68 != 2)goto _LL59;_LL58: return Cyc_Absyn_uint_typ;_LL59: if((int)_tmp68 != 3)
goto _LL5B;_LL5A: return Cyc_Absyn_ulong_typ;_LL5B: if((int)_tmp68 != 4)goto _LL52;
_LL5C: return Cyc_Absyn_ulonglong_typ;_LL52:;}_LL45: if((int)_tmp66 != 2)goto _LL40;
_LL46: {void*_tmp69=sz;_LL5E: if((int)_tmp69 != 0)goto _LL60;_LL5F: return Cyc_Absyn_char_typ;
_LL60: if((int)_tmp69 != 1)goto _LL62;_LL61: return Cyc_Absyn_nshort_typ;_LL62: if((
int)_tmp69 != 2)goto _LL64;_LL63: return Cyc_Absyn_nint_typ;_LL64: if((int)_tmp69 != 3)
goto _LL66;_LL65: return Cyc_Absyn_nlong_typ;_LL66: if((int)_tmp69 != 4)goto _LL5D;
_LL67: return Cyc_Absyn_nlonglong_typ;_LL5D:;}_LL40:;}void*Cyc_Absyn_float_typ=(
void*)1;void*Cyc_Absyn_double_typ(int b);void*Cyc_Absyn_double_typ(int b){static
struct Cyc_Absyn_DoubleType_struct dt={6,1};static struct Cyc_Absyn_DoubleType_struct
df={6,0};return(void*)(b?& dt:& df);}static char _tmp6C[4]="exn";static struct
_dyneither_ptr Cyc_Absyn_exn_str={_tmp6C,_tmp6C,_tmp6C + 4};static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={
2,0}},& Cyc_Absyn_exn_str};struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;
static char _tmp6D[15]="Null_Exception";static struct _dyneither_ptr Cyc_Absyn_Null_Exception_str={
_tmp6D,_tmp6D,_tmp6D + 15};static struct _tuple0 Cyc_Absyn_Null_Exception_pr={{.Abs_n={
2,0}},& Cyc_Absyn_Null_Exception_str};struct _tuple0*Cyc_Absyn_Null_Exception_name=&
Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Null_Exception_tuf_v={&
Cyc_Absyn_Null_Exception_pr,0,0,(void*)3};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Null_Exception_tuf=&
Cyc_Absyn_Null_Exception_tuf_v;static char _tmp6E[13]="Array_bounds";static struct
_dyneither_ptr Cyc_Absyn_Array_bounds_str={_tmp6E,_tmp6E,_tmp6E + 13};static struct
_tuple0 Cyc_Absyn_Array_bounds_pr={{.Abs_n={2,0}},& Cyc_Absyn_Array_bounds_str};
struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static
struct Cyc_Absyn_Datatypefield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,
0,0,(void*)3};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Array_bounds_tuf=& Cyc_Absyn_Array_bounds_tuf_v;
static char _tmp6F[16]="Match_Exception";static struct _dyneither_ptr Cyc_Absyn_Match_Exception_str={
_tmp6F,_tmp6F,_tmp6F + 16};static struct _tuple0 Cyc_Absyn_Match_Exception_pr={{.Abs_n={
2,0}},& Cyc_Absyn_Match_Exception_str};struct _tuple0*Cyc_Absyn_Match_Exception_name=&
Cyc_Absyn_Match_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Match_Exception_tuf_v={&
Cyc_Absyn_Match_Exception_pr,0,0,(void*)3};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Match_Exception_tuf=&
Cyc_Absyn_Match_Exception_tuf_v;static char _tmp70[10]="Bad_alloc";static struct
_dyneither_ptr Cyc_Absyn_Bad_alloc_str={_tmp70,_tmp70,_tmp70 + 10};static struct
_tuple0 Cyc_Absyn_Bad_alloc_pr={{.Abs_n={2,0}},& Cyc_Absyn_Bad_alloc_str};struct
_tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Datatypefield
Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,0,0,(void*)3};struct Cyc_Absyn_Datatypefield*
Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;static struct Cyc_List_List Cyc_Absyn_exn_l0={(
void*)& Cyc_Absyn_Null_Exception_tuf_v,0};static struct Cyc_List_List Cyc_Absyn_exn_l1={(
void*)& Cyc_Absyn_Array_bounds_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l0};
static struct Cyc_List_List Cyc_Absyn_exn_l2={(void*)& Cyc_Absyn_Match_Exception_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l1};static struct Cyc_List_List Cyc_Absyn_exn_l3={(
void*)& Cyc_Absyn_Bad_alloc_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l2};static
struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(void*)((struct Cyc_List_List*)& Cyc_Absyn_exn_l3)};
static struct Cyc_Absyn_Datatypedecl Cyc_Absyn_exn_tud_v={(void*)3,& Cyc_Absyn_exn_name_v,
0,(struct Cyc_Core_Opt*)& Cyc_Absyn_exn_ol,1};struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud=&
Cyc_Absyn_exn_tud_v;static struct Cyc_Core_Opt Cyc_Absyn_heap_opt={(void*)((void*)2)};
static struct Cyc_Absyn_DatatypeType_struct Cyc_Absyn_exn_typ_tt={2,{{.KnownDatatype={
2,& Cyc_Absyn_exn_tud}},0,(struct Cyc_Core_Opt*)& Cyc_Absyn_heap_opt}};void*Cyc_Absyn_exn_typ=(
void*)& Cyc_Absyn_exn_typ_tt;static char _tmp72[9]="PrintArg";static struct
_dyneither_ptr Cyc_Absyn_printarg_str={_tmp72,_tmp72,_tmp72 + 9};static char _tmp73[
9]="ScanfArg";static struct _dyneither_ptr Cyc_Absyn_scanfarg_str={_tmp73,_tmp73,
_tmp73 + 9};static struct _tuple0 Cyc_Absyn_datatype_print_arg_qvar_p={{.Abs_n={2,0}},&
Cyc_Absyn_printarg_str};static struct _tuple0 Cyc_Absyn_datatype_scanf_arg_qvar_p={{.Abs_n={
2,0}},& Cyc_Absyn_scanfarg_str};struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar=&
Cyc_Absyn_datatype_print_arg_qvar_p;struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar=&
Cyc_Absyn_datatype_scanf_arg_qvar_p;static void**Cyc_Absyn_string_t_opt=0;void*
Cyc_Absyn_string_typ(void*rgn);void*Cyc_Absyn_string_typ(void*rgn){if(rgn != (
void*)2)return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_empty_tqual(0),(
void*)0,Cyc_Absyn_true_conref);if(Cyc_Absyn_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_empty_tqual(0),(void*)0,Cyc_Absyn_true_conref);
void**_tmp1CC;Cyc_Absyn_string_t_opt=((_tmp1CC=_cycalloc(sizeof(*_tmp1CC)),((
_tmp1CC[0]=t,_tmp1CC))));}return*((void**)_check_null(Cyc_Absyn_string_t_opt));}
static void**Cyc_Absyn_const_string_t_opt=0;void*Cyc_Absyn_const_string_typ(void*
rgn);void*Cyc_Absyn_const_string_typ(void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);
if(Cyc_Absyn_const_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);void**_tmp1CD;
Cyc_Absyn_const_string_t_opt=((_tmp1CD=_cycalloc(sizeof(*_tmp1CD)),((_tmp1CD[0]=
t,_tmp1CD))));}return*((void**)_check_null(Cyc_Absyn_const_string_t_opt));}void*
Cyc_Absyn_starb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zeroterm);void*Cyc_Absyn_starb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,
union Cyc_Absyn_Constraint*zeroterm){struct Cyc_Absyn_PointerType_struct _tmp1D7;
struct Cyc_Absyn_PtrAtts _tmp1D6;struct Cyc_Absyn_PtrInfo _tmp1D5;struct Cyc_Absyn_PointerType_struct*
_tmp1D4;return(void*)((_tmp1D4=_cycalloc(sizeof(*_tmp1D4)),((_tmp1D4[0]=((
_tmp1D7.tag=4,((_tmp1D7.f1=((_tmp1D5.elt_typ=t,((_tmp1D5.elt_tq=tq,((_tmp1D5.ptr_atts=((
_tmp1D6.rgn=r,((_tmp1D6.nullable=Cyc_Absyn_true_conref,((_tmp1D6.bounds=Cyc_Absyn_new_conref(
b),((_tmp1D6.zero_term=zeroterm,((_tmp1D6.ptrloc=0,_tmp1D6)))))))))),_tmp1D5)))))),
_tmp1D7)))),_tmp1D4))));}void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual
tq,void*b,union Cyc_Absyn_Constraint*zeroterm);void*Cyc_Absyn_atb_typ(void*t,void*
r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zeroterm){struct Cyc_Absyn_PointerType_struct
_tmp1E1;struct Cyc_Absyn_PtrAtts _tmp1E0;struct Cyc_Absyn_PtrInfo _tmp1DF;struct Cyc_Absyn_PointerType_struct*
_tmp1DE;return(void*)((_tmp1DE=_cycalloc(sizeof(*_tmp1DE)),((_tmp1DE[0]=((
_tmp1E1.tag=4,((_tmp1E1.f1=((_tmp1DF.elt_typ=t,((_tmp1DF.elt_tq=tq,((_tmp1DF.ptr_atts=((
_tmp1E0.rgn=r,((_tmp1E0.nullable=Cyc_Absyn_false_conref,((_tmp1E0.bounds=Cyc_Absyn_new_conref(
b),((_tmp1E0.zero_term=zeroterm,((_tmp1E0.ptrloc=0,_tmp1E0)))))))))),_tmp1DF)))))),
_tmp1E1)))),_tmp1DE))));}void*Cyc_Absyn_star_typ(void*t,void*r,struct Cyc_Absyn_Tqual
tq,union Cyc_Absyn_Constraint*zeroterm);void*Cyc_Absyn_star_typ(void*t,void*r,
struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){struct Cyc_Absyn_PointerType_struct
_tmp1EB;struct Cyc_Absyn_PtrAtts _tmp1EA;struct Cyc_Absyn_PtrInfo _tmp1E9;struct Cyc_Absyn_PointerType_struct*
_tmp1E8;return(void*)((_tmp1E8=_cycalloc(sizeof(*_tmp1E8)),((_tmp1E8[0]=((
_tmp1EB.tag=4,((_tmp1EB.f1=((_tmp1E9.elt_typ=t,((_tmp1E9.elt_tq=tq,((_tmp1E9.ptr_atts=((
_tmp1EA.rgn=r,((_tmp1EA.nullable=Cyc_Absyn_true_conref,((_tmp1EA.bounds=Cyc_Absyn_bounds_one_conref,((
_tmp1EA.zero_term=zeroterm,((_tmp1EA.ptrloc=0,_tmp1EA)))))))))),_tmp1E9)))))),
_tmp1EB)))),_tmp1E8))));}void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){struct Cyc_Absyn_PointerType_struct
_tmp1F5;struct Cyc_Absyn_PtrAtts _tmp1F4;struct Cyc_Absyn_PtrInfo _tmp1F3;struct Cyc_Absyn_PointerType_struct*
_tmp1F2;return(void*)((_tmp1F2=_cycalloc(sizeof(*_tmp1F2)),((_tmp1F2[0]=((
_tmp1F5.tag=4,((_tmp1F5.f1=((_tmp1F3.elt_typ=t,((_tmp1F3.elt_tq=tq,((_tmp1F3.ptr_atts=((
_tmp1F4.rgn=(void*)2,((_tmp1F4.nullable=Cyc_Absyn_true_conref,((_tmp1F4.bounds=
Cyc_Absyn_bounds_one_conref,((_tmp1F4.zero_term=Cyc_Absyn_false_conref,((_tmp1F4.ptrloc=
0,_tmp1F4)))))))))),_tmp1F3)))))),_tmp1F5)))),_tmp1F2))));}void*Cyc_Absyn_at_typ(
void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm);void*
Cyc_Absyn_at_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zeroterm){struct Cyc_Absyn_PointerType_struct _tmp1FF;struct Cyc_Absyn_PtrAtts
_tmp1FE;struct Cyc_Absyn_PtrInfo _tmp1FD;struct Cyc_Absyn_PointerType_struct*
_tmp1FC;return(void*)((_tmp1FC=_cycalloc(sizeof(*_tmp1FC)),((_tmp1FC[0]=((
_tmp1FF.tag=4,((_tmp1FF.f1=((_tmp1FD.elt_typ=t,((_tmp1FD.elt_tq=tq,((_tmp1FD.ptr_atts=((
_tmp1FE.rgn=r,((_tmp1FE.nullable=Cyc_Absyn_false_conref,((_tmp1FE.bounds=Cyc_Absyn_bounds_one_conref,((
_tmp1FE.zero_term=zeroterm,((_tmp1FE.ptrloc=0,_tmp1FE)))))))))),_tmp1FD)))))),
_tmp1FF)))),_tmp1FC))));}void*Cyc_Absyn_dyneither_typ(void*t,void*r,struct Cyc_Absyn_Tqual
tq,union Cyc_Absyn_Constraint*zeroterm);void*Cyc_Absyn_dyneither_typ(void*t,void*
r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){struct Cyc_Absyn_PointerType_struct
_tmp209;struct Cyc_Absyn_PtrAtts _tmp208;struct Cyc_Absyn_PtrInfo _tmp207;struct Cyc_Absyn_PointerType_struct*
_tmp206;return(void*)((_tmp206=_cycalloc(sizeof(*_tmp206)),((_tmp206[0]=((
_tmp209.tag=4,((_tmp209.f1=((_tmp207.elt_typ=t,((_tmp207.elt_tq=tq,((_tmp207.ptr_atts=((
_tmp208.rgn=r,((_tmp208.nullable=Cyc_Absyn_true_conref,((_tmp208.bounds=Cyc_Absyn_bounds_dyneither_conref,((
_tmp208.zero_term=zeroterm,((_tmp208.ptrloc=0,_tmp208)))))))))),_tmp207)))))),
_tmp209)))),_tmp206))));}void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*
ztloc);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*ztloc){
struct Cyc_Absyn_ArrayType_struct _tmp20F;struct Cyc_Absyn_ArrayInfo _tmp20E;struct
Cyc_Absyn_ArrayType_struct*_tmp20D;return(void*)((_tmp20D=_cycalloc(sizeof(*
_tmp20D)),((_tmp20D[0]=((_tmp20F.tag=7,((_tmp20F.f1=((_tmp20E.elt_type=elt_type,((
_tmp20E.tq=tq,((_tmp20E.num_elts=num_elts,((_tmp20E.zero_term=zero_term,((
_tmp20E.zt_loc=ztloc,_tmp20E)))))))))),_tmp20F)))),_tmp20D))));}static char _tmp98[
8]="__sFILE";void*Cyc_Absyn_file_typ();void*Cyc_Absyn_file_typ(){static void**
file_t_opt=0;static struct _dyneither_ptr sf_str={_tmp98,_tmp98,_tmp98 + 8};static
struct _dyneither_ptr*sf=& sf_str;if(file_t_opt == 0){struct _tuple0*_tmp210;struct
_tuple0*file_t_name=(_tmp210=_cycalloc(sizeof(*_tmp210)),((_tmp210->f1=Cyc_Absyn_Abs_n(
0),((_tmp210->f2=sf,_tmp210)))));struct Cyc_Absyn_Aggrdecl*_tmp211;struct Cyc_Absyn_Aggrdecl*
sd=(_tmp211=_cycalloc(sizeof(*_tmp211)),((_tmp211->kind=(void*)0,((_tmp211->sc=(
void*)1,((_tmp211->name=file_t_name,((_tmp211->tvs=0,((_tmp211->impl=0,((_tmp211->attributes=
0,_tmp211)))))))))))));struct Cyc_Absyn_AggrType_struct _tmp21B;struct Cyc_Absyn_Aggrdecl**
_tmp21A;struct Cyc_Absyn_AggrInfo _tmp219;struct Cyc_Absyn_AggrType_struct*_tmp218;
void*file_struct_typ=(void*)((_tmp218=_cycalloc(sizeof(*_tmp218)),((_tmp218[0]=((
_tmp21B.tag=10,((_tmp21B.f1=((_tmp219.aggr_info=Cyc_Absyn_KnownAggr(((_tmp21A=
_cycalloc(sizeof(*_tmp21A)),((_tmp21A[0]=sd,_tmp21A))))),((_tmp219.targs=0,
_tmp219)))),_tmp21B)))),_tmp218))));void**_tmp21C;file_t_opt=((_tmp21C=_cycalloc(
sizeof(*_tmp21C)),((_tmp21C[0]=Cyc_Absyn_at_typ(file_struct_typ,(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref),_tmp21C))));}return*file_t_opt;}void*Cyc_Absyn_void_star_typ();
void*Cyc_Absyn_void_star_typ(){static void**void_star_t_opt=0;if(void_star_t_opt
== 0){void**_tmp21D;void_star_t_opt=((_tmp21D=_cycalloc(sizeof(*_tmp21D)),((
_tmp21D[0]=Cyc_Absyn_star_typ((void*)0,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),
_tmp21D))));}return*void_star_t_opt;}static struct Cyc_Absyn_JoinEff_struct Cyc_Absyn_empty_eff={
20,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;void*Cyc_Absyn_aggr_typ(
void*k,struct _dyneither_ptr*name);void*Cyc_Absyn_aggr_typ(void*k,struct
_dyneither_ptr*name){struct Cyc_Absyn_AggrType_struct _tmp227;struct _tuple0*
_tmp226;struct Cyc_Absyn_AggrInfo _tmp225;struct Cyc_Absyn_AggrType_struct*_tmp224;
return(void*)((_tmp224=_cycalloc(sizeof(*_tmp224)),((_tmp224[0]=((_tmp227.tag=10,((
_tmp227.f1=((_tmp225.aggr_info=Cyc_Absyn_UnknownAggr(k,((_tmp226=_cycalloc(
sizeof(*_tmp226)),((_tmp226->f1=Cyc_Absyn_rel_ns_null,((_tmp226->f2=name,_tmp226)))))),
0),((_tmp225.targs=0,_tmp225)))),_tmp227)))),_tmp224))));}void*Cyc_Absyn_strct(
struct _dyneither_ptr*name);void*Cyc_Absyn_strct(struct _dyneither_ptr*name){
return Cyc_Absyn_aggr_typ((void*)0,name);}void*Cyc_Absyn_union_typ(struct
_dyneither_ptr*name);void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name){return
Cyc_Absyn_aggr_typ((void*)1,name);}void*Cyc_Absyn_strctq(struct _tuple0*name);
void*Cyc_Absyn_strctq(struct _tuple0*name){struct Cyc_Absyn_AggrType_struct _tmp22D;
struct Cyc_Absyn_AggrInfo _tmp22C;struct Cyc_Absyn_AggrType_struct*_tmp22B;return(
void*)((_tmp22B=_cycalloc(sizeof(*_tmp22B)),((_tmp22B[0]=((_tmp22D.tag=10,((
_tmp22D.f1=((_tmp22C.aggr_info=Cyc_Absyn_UnknownAggr((void*)0,name,0),((_tmp22C.targs=
0,_tmp22C)))),_tmp22D)))),_tmp22B))));}void*Cyc_Absyn_unionq_typ(struct _tuple0*
name);void*Cyc_Absyn_unionq_typ(struct _tuple0*name){struct Cyc_Absyn_AggrType_struct
_tmp233;struct Cyc_Absyn_AggrInfo _tmp232;struct Cyc_Absyn_AggrType_struct*_tmp231;
return(void*)((_tmp231=_cycalloc(sizeof(*_tmp231)),((_tmp231[0]=((_tmp233.tag=10,((
_tmp233.f1=((_tmp232.aggr_info=Cyc_Absyn_UnknownAggr((void*)1,name,0),((_tmp232.targs=
0,_tmp232)))),_tmp233)))),_tmp231))));}union Cyc_Absyn_Cnst Cyc_Absyn_Null_c={.Null_c={
1,0}};union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(void*sn,char c);union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(
void*sn,char c){struct _tuple3 _tmp236;union Cyc_Absyn_Cnst _tmp235;return((_tmp235.Char_c).val=((
_tmp236.f1=sn,((_tmp236.f2=c,_tmp236)))),(((_tmp235.Char_c).tag=2,_tmp235)));}
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(void*sn,short s);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(
void*sn,short s){struct _tuple4 _tmp239;union Cyc_Absyn_Cnst _tmp238;return((_tmp238.Short_c).val=((
_tmp239.f1=sn,((_tmp239.f2=s,_tmp239)))),(((_tmp238.Short_c).tag=3,_tmp238)));}
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(void*sn,int i);union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(
void*sn,int i){struct _tuple5 _tmp23C;union Cyc_Absyn_Cnst _tmp23B;return((_tmp23B.Int_c).val=((
_tmp23C.f1=sn,((_tmp23C.f2=i,_tmp23C)))),(((_tmp23B.Int_c).tag=4,_tmp23B)));}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(void*sn,long long l);union Cyc_Absyn_Cnst
Cyc_Absyn_LongLong_c(void*sn,long long l){struct _tuple6 _tmp23F;union Cyc_Absyn_Cnst
_tmp23E;return((_tmp23E.LongLong_c).val=((_tmp23F.f1=sn,((_tmp23F.f2=l,_tmp23F)))),(((
_tmp23E.LongLong_c).tag=5,_tmp23E)));}union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(
struct _dyneither_ptr s);union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr
s){union Cyc_Absyn_Cnst _tmp240;return((_tmp240.Float_c).val=s,(((_tmp240.Float_c).tag=
6,_tmp240)));}union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr s);union
Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr s){union Cyc_Absyn_Cnst
_tmp241;return((_tmp241.String_c).val=s,(((_tmp241.String_c).tag=7,_tmp241)));}
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Exp*_tmp242;return(_tmp242=_cycalloc(sizeof(*_tmp242)),((_tmp242->topt=
0,((_tmp242->r=r,((_tmp242->loc=loc,((_tmp242->annot=(void*)Cyc_Absyn_EmptyAnnot,
_tmp242)))))))));}struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*
rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*
Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_New_e_struct _tmp245;struct Cyc_Absyn_New_e_struct*_tmp244;
return Cyc_Absyn_new_exp((void*)((_tmp244=_cycalloc(sizeof(*_tmp244)),((_tmp244[0]=((
_tmp245.tag=17,((_tmp245.f1=rgn_handle,((_tmp245.f2=e,_tmp245)))))),_tmp244)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_tmp246;return(
_tmp246=_cycalloc(sizeof(*_tmp246)),((_tmp246[0]=*e,_tmp246)));}struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Const_e_struct _tmp249;struct Cyc_Absyn_Const_e_struct*
_tmp248;return Cyc_Absyn_new_exp((void*)((_tmp248=_cycalloc(sizeof(*_tmp248)),((
_tmp248[0]=((_tmp249.tag=0,((_tmp249.f1=c,_tmp249)))),_tmp248)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Const_e_struct
_tmp24C;struct Cyc_Absyn_Const_e_struct*_tmp24B;return Cyc_Absyn_new_exp((void*)((
_tmp24B=_cycalloc(sizeof(*_tmp24B)),((_tmp24B[0]=((_tmp24C.tag=0,((_tmp24C.f1=
Cyc_Absyn_Null_c,_tmp24C)))),_tmp24B)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
void*s,int i,struct Cyc_Position_Segment*seg);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
void*s,int i,struct Cyc_Position_Segment*seg){return Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(
s,i),seg);}struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_int_exp((void*)0,i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int i,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int i,struct Cyc_Position_Segment*loc){return Cyc_Absyn_int_exp((void*)1,(
int)i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*
loc){return Cyc_Absyn_bool_exp(1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct
Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(0,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp(Cyc_Absyn_Char_c((
void*)2,c),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct
_dyneither_ptr f,struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(
f),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct
_dyneither_ptr s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp(Cyc_Absyn_String_c(
s),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Var_e_struct _tmp24F;struct Cyc_Absyn_Var_e_struct*_tmp24E;
return Cyc_Absyn_new_exp((void*)((_tmp24E=_cycalloc(sizeof(*_tmp24E)),((_tmp24E[0]=((
_tmp24F.tag=1,((_tmp24F.f1=q,((_tmp24F.f2=(void*)((void*)0),_tmp24F)))))),
_tmp24E)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct
_tuple0*q,void*b,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Var_e_struct
_tmp252;struct Cyc_Absyn_Var_e_struct*_tmp251;return Cyc_Absyn_new_exp((void*)((
_tmp251=_cycalloc(sizeof(*_tmp251)),((_tmp251[0]=((_tmp252.tag=1,((_tmp252.f1=q,((
_tmp252.f2=(void*)b,_tmp252)))))),_tmp251)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(
struct _tuple0*q,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(
struct _tuple0*q,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_UnknownId_e_struct
_tmp255;struct Cyc_Absyn_UnknownId_e_struct*_tmp254;return Cyc_Absyn_new_exp((void*)((
_tmp254=_cycalloc(sizeof(*_tmp254)),((_tmp254[0]=((_tmp255.tag=2,((_tmp255.f1=q,
_tmp255)))),_tmp254)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*p,
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(
void*p,struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Primop_e_struct
_tmp258;struct Cyc_Absyn_Primop_e_struct*_tmp257;return Cyc_Absyn_new_exp((void*)((
_tmp257=_cycalloc(sizeof(*_tmp257)),((_tmp257[0]=((_tmp258.tag=3,((_tmp258.f1=(
void*)p,((_tmp258.f2=es,_tmp258)))))),_tmp257)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp(void*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
struct Cyc_List_List*_tmp259;return Cyc_Absyn_primop_exp(p,((_tmp259=_cycalloc(
sizeof(*_tmp259)),((_tmp259->hd=e,((_tmp259->tl=0,_tmp259)))))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_prim2_exp(void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(void*p,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){struct
Cyc_List_List*_tmp25C;struct Cyc_List_List*_tmp25B;return Cyc_Absyn_primop_exp(p,((
_tmp25B=_cycalloc(sizeof(*_tmp25B)),((_tmp25B->hd=e1,((_tmp25B->tl=((_tmp25C=
_cycalloc(sizeof(*_tmp25C)),((_tmp25C->hd=e2,((_tmp25C->tl=0,_tmp25C)))))),
_tmp25B)))))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)0,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_prim2_exp((void*)1,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)3,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_prim2_exp((void*)5,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_prim2_exp((void*)6,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_prim2_exp((void*)7,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_prim2_exp((void*)8,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_prim2_exp((void*)9,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_prim2_exp((void*)10,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_AssignOp_e_struct
_tmp25F;struct Cyc_Absyn_AssignOp_e_struct*_tmp25E;return Cyc_Absyn_new_exp((void*)((
_tmp25E=_cycalloc(sizeof(*_tmp25E)),((_tmp25E[0]=((_tmp25F.tag=4,((_tmp25F.f1=e1,((
_tmp25F.f2=popt,((_tmp25F.f3=e2,_tmp25F)))))))),_tmp25E)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,void*i,struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*
e,void*i,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Increment_e_struct
_tmp262;struct Cyc_Absyn_Increment_e_struct*_tmp261;return Cyc_Absyn_new_exp((void*)((
_tmp261=_cycalloc(sizeof(*_tmp261)),((_tmp261[0]=((_tmp262.tag=5,((_tmp262.f1=e,((
_tmp262.f2=(void*)i,_tmp262)))))),_tmp261)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(
e,(void*)1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(e,(
void*)0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(e,(
void*)2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(
e,(void*)3,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Conditional_e_struct
_tmp265;struct Cyc_Absyn_Conditional_e_struct*_tmp264;return Cyc_Absyn_new_exp((
void*)((_tmp264=_cycalloc(sizeof(*_tmp264)),((_tmp264[0]=((_tmp265.tag=6,((
_tmp265.f1=e1,((_tmp265.f2=e2,((_tmp265.f3=e3,_tmp265)))))))),_tmp264)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){struct
Cyc_Absyn_And_e_struct _tmp268;struct Cyc_Absyn_And_e_struct*_tmp267;return Cyc_Absyn_new_exp((
void*)((_tmp267=_cycalloc(sizeof(*_tmp267)),((_tmp267[0]=((_tmp268.tag=7,((
_tmp268.f1=e1,((_tmp268.f2=e2,_tmp268)))))),_tmp267)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Or_e_struct _tmp26B;struct Cyc_Absyn_Or_e_struct*
_tmp26A;return Cyc_Absyn_new_exp((void*)((_tmp26A=_cycalloc(sizeof(*_tmp26A)),((
_tmp26A[0]=((_tmp26B.tag=8,((_tmp26B.f1=e1,((_tmp26B.f2=e2,_tmp26B)))))),_tmp26A)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){struct
Cyc_Absyn_SeqExp_e_struct _tmp26E;struct Cyc_Absyn_SeqExp_e_struct*_tmp26D;return
Cyc_Absyn_new_exp((void*)((_tmp26D=_cycalloc(sizeof(*_tmp26D)),((_tmp26D[0]=((
_tmp26E.tag=9,((_tmp26E.f1=e1,((_tmp26E.f2=e2,_tmp26E)))))),_tmp26D)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_UnknownCall_e_struct _tmp271;struct Cyc_Absyn_UnknownCall_e_struct*
_tmp270;return Cyc_Absyn_new_exp((void*)((_tmp270=_cycalloc(sizeof(*_tmp270)),((
_tmp270[0]=((_tmp271.tag=10,((_tmp271.f1=e,((_tmp271.f2=es,_tmp271)))))),_tmp270)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_FnCall_e_struct _tmp274;struct Cyc_Absyn_FnCall_e_struct*_tmp273;
return Cyc_Absyn_new_exp((void*)((_tmp273=_cycalloc(sizeof(*_tmp273)),((_tmp273[0]=((
_tmp274.tag=11,((_tmp274.f1=e,((_tmp274.f2=es,((_tmp274.f3=0,_tmp274)))))))),
_tmp273)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_NoInstantiate_e_struct
_tmp277;struct Cyc_Absyn_NoInstantiate_e_struct*_tmp276;return Cyc_Absyn_new_exp((
void*)((_tmp276=_cycalloc(sizeof(*_tmp276)),((_tmp276[0]=((_tmp277.tag=13,((
_tmp277.f1=e,_tmp277)))),_tmp276)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
ts,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Instantiate_e_struct _tmp27A;
struct Cyc_Absyn_Instantiate_e_struct*_tmp279;return Cyc_Absyn_new_exp((void*)((
_tmp279=_cycalloc(sizeof(*_tmp279)),((_tmp279[0]=((_tmp27A.tag=14,((_tmp27A.f1=e,((
_tmp27A.f2=ts,_tmp27A)))))),_tmp279)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*t,struct Cyc_Absyn_Exp*e,int user_cast,void*c,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,
void*c,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Cast_e_struct _tmp27D;
struct Cyc_Absyn_Cast_e_struct*_tmp27C;return Cyc_Absyn_new_exp((void*)((_tmp27C=
_cycalloc(sizeof(*_tmp27C)),((_tmp27C[0]=((_tmp27D.tag=15,((_tmp27D.f1=(void*)t,((
_tmp27D.f2=e,((_tmp27D.f3=user_cast,((_tmp27D.f4=(void*)c,_tmp27D)))))))))),
_tmp27C)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Throw_e_struct
_tmp280;struct Cyc_Absyn_Throw_e_struct*_tmp27F;return Cyc_Absyn_new_exp((void*)((
_tmp27F=_cycalloc(sizeof(*_tmp27F)),((_tmp27F[0]=((_tmp280.tag=12,((_tmp280.f1=e,
_tmp280)))),_tmp27F)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Address_e_struct
_tmp283;struct Cyc_Absyn_Address_e_struct*_tmp282;return Cyc_Absyn_new_exp((void*)((
_tmp282=_cycalloc(sizeof(*_tmp282)),((_tmp282[0]=((_tmp283.tag=16,((_tmp283.f1=e,
_tmp283)))),_tmp282)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*
t,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Sizeoftyp_e_struct _tmp286;
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp285;return Cyc_Absyn_new_exp((void*)((
_tmp285=_cycalloc(sizeof(*_tmp285)),((_tmp285[0]=((_tmp286.tag=18,((_tmp286.f1=(
void*)t,_tmp286)))),_tmp285)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Sizeofexp_e_struct
_tmp289;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp288;return Cyc_Absyn_new_exp((void*)((
_tmp288=_cycalloc(sizeof(*_tmp288)),((_tmp288[0]=((_tmp289.tag=19,((_tmp289.f1=e,
_tmp289)))),_tmp288)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,
void*of,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(
void*t,void*of,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Offsetof_e_struct
_tmp28C;struct Cyc_Absyn_Offsetof_e_struct*_tmp28B;return Cyc_Absyn_new_exp((void*)((
_tmp28B=_cycalloc(sizeof(*_tmp28B)),((_tmp28B[0]=((_tmp28C.tag=20,((_tmp28C.f1=(
void*)t,((_tmp28C.f2=(void*)of,_tmp28C)))))),_tmp28B)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_gentyp_exp(struct Cyc_List_List*tvs,void*t,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*tvs,void*t,
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Gentyp_e_struct _tmp28F;struct Cyc_Absyn_Gentyp_e_struct*
_tmp28E;return Cyc_Absyn_new_exp((void*)((_tmp28E=_cycalloc(sizeof(*_tmp28E)),((
_tmp28E[0]=((_tmp28F.tag=21,((_tmp28F.f1=tvs,((_tmp28F.f2=(void*)t,_tmp28F)))))),
_tmp28E)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Deref_e_struct
_tmp292;struct Cyc_Absyn_Deref_e_struct*_tmp291;return Cyc_Absyn_new_exp((void*)((
_tmp291=_cycalloc(sizeof(*_tmp291)),((_tmp291[0]=((_tmp292.tag=22,((_tmp292.f1=e,
_tmp292)))),_tmp291)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*n,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_AggrMember_e_struct
_tmp295;struct Cyc_Absyn_AggrMember_e_struct*_tmp294;return Cyc_Absyn_new_exp((
void*)((_tmp294=_cycalloc(sizeof(*_tmp294)),((_tmp294[0]=((_tmp295.tag=23,((
_tmp295.f1=e,((_tmp295.f2=n,((_tmp295.f3=0,((_tmp295.f4=0,_tmp295)))))))))),
_tmp294)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr*n,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_AggrArrow_e_struct _tmp298;struct Cyc_Absyn_AggrArrow_e_struct*
_tmp297;return Cyc_Absyn_new_exp((void*)((_tmp297=_cycalloc(sizeof(*_tmp297)),((
_tmp297[0]=((_tmp298.tag=24,((_tmp298.f1=e,((_tmp298.f2=n,((_tmp298.f3=0,((
_tmp298.f4=0,_tmp298)))))))))),_tmp297)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Subscript_e_struct _tmp29B;
struct Cyc_Absyn_Subscript_e_struct*_tmp29A;return Cyc_Absyn_new_exp((void*)((
_tmp29A=_cycalloc(sizeof(*_tmp29A)),((_tmp29A[0]=((_tmp29B.tag=25,((_tmp29B.f1=
e1,((_tmp29B.f2=e2,_tmp29B)))))),_tmp29A)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Tuple_e_struct
_tmp29E;struct Cyc_Absyn_Tuple_e_struct*_tmp29D;return Cyc_Absyn_new_exp((void*)((
_tmp29D=_cycalloc(sizeof(*_tmp29D)),((_tmp29D[0]=((_tmp29E.tag=26,((_tmp29E.f1=
es,_tmp29E)))),_tmp29D)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct
Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_StmtExp_e_struct
_tmp2A1;struct Cyc_Absyn_StmtExp_e_struct*_tmp2A0;return Cyc_Absyn_new_exp((void*)((
_tmp2A0=_cycalloc(sizeof(*_tmp2A0)),((_tmp2A0[0]=((_tmp2A1.tag=38,((_tmp2A1.f1=s,
_tmp2A1)))),_tmp2A0)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*
loc){return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_valueof_exp(void*t,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*
Cyc_Absyn_valueof_exp(void*t,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Valueof_e_struct
_tmp2A4;struct Cyc_Absyn_Valueof_e_struct*_tmp2A3;return Cyc_Absyn_new_exp((void*)((
_tmp2A3=_cycalloc(sizeof(*_tmp2A3)),((_tmp2A3[0]=((_tmp2A4.tag=40,((_tmp2A4.f1=(
void*)t,_tmp2A4)))),_tmp2A3)))),loc);}struct _tuple19{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_List_List*dles=
0;for(0;es != 0;es=es->tl){struct _tuple19*_tmp2A7;struct Cyc_List_List*_tmp2A6;
dles=((_tmp2A6=_cycalloc(sizeof(*_tmp2A6)),((_tmp2A6->hd=((_tmp2A7=_cycalloc(
sizeof(*_tmp2A7)),((_tmp2A7->f1=0,((_tmp2A7->f2=(struct Cyc_Absyn_Exp*)es->hd,
_tmp2A7)))))),((_tmp2A6->tl=dles,_tmp2A6))))));}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);{struct Cyc_Absyn_Array_e_struct
_tmp2AA;struct Cyc_Absyn_Array_e_struct*_tmp2A9;return Cyc_Absyn_new_exp((void*)((
_tmp2A9=_cycalloc(sizeof(*_tmp2A9)),((_tmp2A9[0]=((_tmp2AA.tag=28,((_tmp2AA.f1=
dles,_tmp2AA)))),_tmp2A9)))),loc);}}struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*
dles,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp2AD;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp2AC;return Cyc_Absyn_new_exp((
void*)((_tmp2AC=_cycalloc(sizeof(*_tmp2AC)),((_tmp2AC[0]=((_tmp2AD.tag=37,((
_tmp2AD.f1=n,((_tmp2AD.f2=dles,_tmp2AD)))))),_tmp2AC)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Stmt*
_tmp2AE;return(_tmp2AE=_cycalloc(sizeof(*_tmp2AE)),((_tmp2AE->r=s,((_tmp2AE->loc=
loc,((_tmp2AE->non_local_preds=0,((_tmp2AE->try_depth=0,((_tmp2AE->annot=(void*)
Cyc_Absyn_EmptyAnnot,_tmp2AE)))))))))));}struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)0,loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Exp_s_struct _tmp2B1;struct Cyc_Absyn_Exp_s_struct*_tmp2B0;
return Cyc_Absyn_new_stmt((void*)((_tmp2B0=_cycalloc(sizeof(*_tmp2B0)),((_tmp2B0[
0]=((_tmp2B1.tag=0,((_tmp2B1.f1=e,_tmp2B1)))),_tmp2B0)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,struct Cyc_Position_Segment*
loc){if(ss == 0)return Cyc_Absyn_skip_stmt(loc);else{if(ss->tl == 0)return(struct
Cyc_Absyn_Stmt*)ss->hd;else{return Cyc_Absyn_seq_stmt((struct Cyc_Absyn_Stmt*)ss->hd,
Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Return_s_struct
_tmp2B4;struct Cyc_Absyn_Return_s_struct*_tmp2B3;return Cyc_Absyn_new_stmt((void*)((
_tmp2B3=_cycalloc(sizeof(*_tmp2B3)),((_tmp2B3[0]=((_tmp2B4.tag=2,((_tmp2B4.f1=e,
_tmp2B4)))),_tmp2B3)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc){struct
Cyc_Absyn_IfThenElse_s_struct _tmp2B7;struct Cyc_Absyn_IfThenElse_s_struct*_tmp2B6;
return Cyc_Absyn_new_stmt((void*)((_tmp2B6=_cycalloc(sizeof(*_tmp2B6)),((_tmp2B6[
0]=((_tmp2B7.tag=3,((_tmp2B7.f1=e,((_tmp2B7.f2=s1,((_tmp2B7.f3=s2,_tmp2B7)))))))),
_tmp2B6)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_While_s_struct _tmp2BD;struct _tuple8 _tmp2BC;struct Cyc_Absyn_While_s_struct*
_tmp2BB;return Cyc_Absyn_new_stmt((void*)((_tmp2BB=_cycalloc(sizeof(*_tmp2BB)),((
_tmp2BB[0]=((_tmp2BD.tag=4,((_tmp2BD.f1=((_tmp2BC.f1=e,((_tmp2BC.f2=Cyc_Absyn_skip_stmt(
e->loc),_tmp2BC)))),((_tmp2BD.f2=s,_tmp2BD)))))),_tmp2BB)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_break_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Break_s_struct _tmp2C0;struct Cyc_Absyn_Break_s_struct*
_tmp2BF;return Cyc_Absyn_new_stmt((void*)((_tmp2BF=_cycalloc(sizeof(*_tmp2BF)),((
_tmp2BF[0]=((_tmp2C0.tag=5,((_tmp2C0.f1=0,_tmp2C0)))),_tmp2BF)))),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Continue_s_struct
_tmp2C3;struct Cyc_Absyn_Continue_s_struct*_tmp2C2;return Cyc_Absyn_new_stmt((void*)((
_tmp2C2=_cycalloc(sizeof(*_tmp2C2)),((_tmp2C2[0]=((_tmp2C3.tag=6,((_tmp2C3.f1=0,
_tmp2C3)))),_tmp2C2)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct
Cyc_Position_Segment*loc){struct Cyc_Absyn_For_s_struct _tmp2CC;struct _tuple8
_tmp2CB;struct _tuple8 _tmp2CA;struct Cyc_Absyn_For_s_struct*_tmp2C9;return Cyc_Absyn_new_stmt((
void*)((_tmp2C9=_cycalloc(sizeof(*_tmp2C9)),((_tmp2C9[0]=((_tmp2CC.tag=8,((
_tmp2CC.f1=e1,((_tmp2CC.f2=((_tmp2CA.f1=e2,((_tmp2CA.f2=Cyc_Absyn_skip_stmt(e3->loc),
_tmp2CA)))),((_tmp2CC.f3=((_tmp2CB.f1=e3,((_tmp2CB.f2=Cyc_Absyn_skip_stmt(e3->loc),
_tmp2CB)))),((_tmp2CC.f4=s,_tmp2CC)))))))))),_tmp2C9)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Switch_s_struct _tmp2CF;
struct Cyc_Absyn_Switch_s_struct*_tmp2CE;return Cyc_Absyn_new_stmt((void*)((
_tmp2CE=_cycalloc(sizeof(*_tmp2CE)),((_tmp2CE[0]=((_tmp2CF.tag=9,((_tmp2CF.f1=e,((
_tmp2CF.f2=scs,_tmp2CF)))))),_tmp2CE)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc){struct _tuple15 _tmp2D0;struct _tuple15 _tmp10D=(
_tmp2D0.f1=s1->r,((_tmp2D0.f2=s2->r,_tmp2D0)));void*_tmp10E;void*_tmp10F;_LL69:
_tmp10E=_tmp10D.f1;if((int)_tmp10E != 0)goto _LL6B;_LL6A: return s2;_LL6B: _tmp10F=
_tmp10D.f2;if((int)_tmp10F != 0)goto _LL6D;_LL6C: return s1;_LL6D:;_LL6E: {struct Cyc_Absyn_Seq_s_struct
_tmp2D3;struct Cyc_Absyn_Seq_s_struct*_tmp2D2;return Cyc_Absyn_new_stmt((void*)((
_tmp2D2=_cycalloc(sizeof(*_tmp2D2)),((_tmp2D2[0]=((_tmp2D3.tag=1,((_tmp2D3.f1=s1,((
_tmp2D3.f2=s2,_tmp2D3)))))),_tmp2D2)))),loc);}_LL68:;}struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Fallthru_s_struct
_tmp2D6;struct Cyc_Absyn_Fallthru_s_struct*_tmp2D5;return Cyc_Absyn_new_stmt((void*)((
_tmp2D5=_cycalloc(sizeof(*_tmp2D5)),((_tmp2D5[0]=((_tmp2D6.tag=10,((_tmp2D6.f1=
el,((_tmp2D6.f2=0,_tmp2D6)))))),_tmp2D5)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl_s_struct _tmp2D9;struct Cyc_Absyn_Decl_s_struct*
_tmp2D8;return Cyc_Absyn_new_stmt((void*)((_tmp2D8=_cycalloc(sizeof(*_tmp2D8)),((
_tmp2D8[0]=((_tmp2D9.tag=11,((_tmp2D9.f1=d,((_tmp2D9.f2=s,_tmp2D9)))))),_tmp2D8)))),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct
Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Var_d_struct
_tmp2DC;struct Cyc_Absyn_Var_d_struct*_tmp2DB;struct Cyc_Absyn_Decl*d=Cyc_Absyn_new_decl((
void*)((_tmp2DB=_cycalloc(sizeof(*_tmp2DB)),((_tmp2DB[0]=((_tmp2DC.tag=0,((
_tmp2DC.f1=Cyc_Absyn_new_vardecl(x,t,init),_tmp2DC)))),_tmp2DB)))),loc);struct
Cyc_Absyn_Decl_s_struct _tmp2DF;struct Cyc_Absyn_Decl_s_struct*_tmp2DE;return Cyc_Absyn_new_stmt((
void*)((_tmp2DE=_cycalloc(sizeof(*_tmp2DE)),((_tmp2DE[0]=((_tmp2DF.tag=11,((
_tmp2DF.f1=d,((_tmp2DF.f2=s,_tmp2DF)))))),_tmp2DE)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Label_s_struct _tmp2E2;struct
Cyc_Absyn_Label_s_struct*_tmp2E1;return Cyc_Absyn_new_stmt((void*)((_tmp2E1=
_cycalloc(sizeof(*_tmp2E1)),((_tmp2E1[0]=((_tmp2E2.tag=12,((_tmp2E2.f1=v,((
_tmp2E2.f2=s,_tmp2E2)))))),_tmp2E1)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Do_s_struct _tmp2E8;struct
_tuple8 _tmp2E7;struct Cyc_Absyn_Do_s_struct*_tmp2E6;return Cyc_Absyn_new_stmt((
void*)((_tmp2E6=_cycalloc(sizeof(*_tmp2E6)),((_tmp2E6[0]=((_tmp2E8.tag=13,((
_tmp2E8.f1=s,((_tmp2E8.f2=((_tmp2E7.f1=e,((_tmp2E7.f2=Cyc_Absyn_skip_stmt(e->loc),
_tmp2E7)))),_tmp2E8)))))),_tmp2E6)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_TryCatch_s_struct _tmp2EB;
struct Cyc_Absyn_TryCatch_s_struct*_tmp2EA;return Cyc_Absyn_new_stmt((void*)((
_tmp2EA=_cycalloc(sizeof(*_tmp2EA)),((_tmp2EA[0]=((_tmp2EB.tag=14,((_tmp2EB.f1=s,((
_tmp2EB.f2=scs,_tmp2EB)))))),_tmp2EA)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _dyneither_ptr*lab,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Goto_s_struct _tmp2EE;struct Cyc_Absyn_Goto_s_struct*_tmp2ED;
return Cyc_Absyn_new_stmt((void*)((_tmp2ED=_cycalloc(sizeof(*_tmp2ED)),((_tmp2ED[
0]=((_tmp2EE.tag=7,((_tmp2EE.f1=lab,((_tmp2EE.f2=0,_tmp2EE)))))),_tmp2ED)))),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s){struct Cyc_Absyn_Pat*_tmp2EF;return(_tmp2EF=
_cycalloc(sizeof(*_tmp2EF)),((_tmp2EF->r=p,((_tmp2EF->topt=0,((_tmp2EF->loc=s,
_tmp2EF)))))));}struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_p_struct
_tmp2F2;struct Cyc_Absyn_Exp_p_struct*_tmp2F1;return Cyc_Absyn_new_pat((void*)((
_tmp2F1=_cycalloc(sizeof(*_tmp2F1)),((_tmp2F1[0]=((_tmp2F2.tag=14,((_tmp2F2.f1=e,
_tmp2F2)))),_tmp2F1)))),e->loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl*_tmp2F3;return(_tmp2F3=
_cycalloc(sizeof(*_tmp2F3)),((_tmp2F3->r=r,((_tmp2F3->loc=loc,_tmp2F3)))));}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct
Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Let_d_struct
_tmp2F6;struct Cyc_Absyn_Let_d_struct*_tmp2F5;return Cyc_Absyn_new_decl((void*)((
_tmp2F5=_cycalloc(sizeof(*_tmp2F5)),((_tmp2F5[0]=((_tmp2F6.tag=2,((_tmp2F6.f1=p,((
_tmp2F6.f2=0,((_tmp2F6.f3=e,_tmp2F6)))))))),_tmp2F5)))),loc);}struct Cyc_Absyn_Decl*
Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Letv_d_struct _tmp2F9;struct Cyc_Absyn_Letv_d_struct*_tmp2F8;
return Cyc_Absyn_new_decl((void*)((_tmp2F8=_cycalloc(sizeof(*_tmp2F8)),((_tmp2F8[
0]=((_tmp2F9.tag=3,((_tmp2F9.f1=vds,_tmp2F9)))),_tmp2F8)))),loc);}struct Cyc_Absyn_Decl*
Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,int
resetable,struct Cyc_Absyn_Exp*eo,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,int
resetable,struct Cyc_Absyn_Exp*eo,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Region_d_struct
_tmp2FC;struct Cyc_Absyn_Region_d_struct*_tmp2FB;return Cyc_Absyn_new_decl((void*)((
_tmp2FB=_cycalloc(sizeof(*_tmp2FB)),((_tmp2FB[0]=((_tmp2FC.tag=4,((_tmp2FC.f1=tv,((
_tmp2FC.f2=vd,((_tmp2FC.f3=resetable,((_tmp2FC.f4=eo,_tmp2FC)))))))))),_tmp2FB)))),
loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Tvar*
tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Alias_d_struct _tmp2FF;struct
Cyc_Absyn_Alias_d_struct*_tmp2FE;return Cyc_Absyn_new_decl((void*)((_tmp2FE=
_cycalloc(sizeof(*_tmp2FE)),((_tmp2FE[0]=((_tmp2FF.tag=5,((_tmp2FF.f1=e,((
_tmp2FF.f2=tv,((_tmp2FF.f3=vd,_tmp2FF)))))))),_tmp2FE)))),loc);}struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){struct Cyc_Absyn_Vardecl*
_tmp300;return(_tmp300=_cycalloc(sizeof(*_tmp300)),((_tmp300->sc=(void*)2,((
_tmp300->name=x,((_tmp300->tq=Cyc_Absyn_empty_tqual(0),((_tmp300->type=t,((
_tmp300->initializer=init,((_tmp300->rgn=0,((_tmp300->attributes=0,((_tmp300->escapes=
0,_tmp300)))))))))))))))));}struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){struct Cyc_Absyn_Vardecl*_tmp301;
return(_tmp301=_cycalloc(sizeof(*_tmp301)),((_tmp301->sc=(void*)0,((_tmp301->name=
x,((_tmp301->tq=Cyc_Absyn_empty_tqual(0),((_tmp301->type=t,((_tmp301->initializer=
init,((_tmp301->rgn=0,((_tmp301->attributes=0,((_tmp301->escapes=0,_tmp301)))))))))))))))));}
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,
struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);struct Cyc_Absyn_AggrdeclImpl*
Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct
Cyc_List_List*fs,int tagged){struct Cyc_Absyn_AggrdeclImpl*_tmp302;return(_tmp302=
_cycalloc(sizeof(*_tmp302)),((_tmp302->exist_vars=exists,((_tmp302->rgn_po=po,((
_tmp302->fields=fs,((_tmp302->tagged=tagged,_tmp302)))))))));}struct Cyc_Absyn_Decl*
Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct
Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Aggr_d_struct _tmp308;struct Cyc_Absyn_Aggrdecl*_tmp307;
struct Cyc_Absyn_Aggr_d_struct*_tmp306;return Cyc_Absyn_new_decl((void*)((_tmp306=
_cycalloc(sizeof(*_tmp306)),((_tmp306[0]=((_tmp308.tag=6,((_tmp308.f1=((_tmp307=
_cycalloc(sizeof(*_tmp307)),((_tmp307->kind=k,((_tmp307->sc=s,((_tmp307->name=n,((
_tmp307->tvs=ts,((_tmp307->impl=i,((_tmp307->attributes=atts,_tmp307)))))))))))))),
_tmp308)))),_tmp306)))),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(void*s,
struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((
void*)0,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(void*s,
struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((
void*)1,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(void*s,
struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int
is_extensible,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Datatype_d_struct
_tmp30E;struct Cyc_Absyn_Datatypedecl*_tmp30D;struct Cyc_Absyn_Datatype_d_struct*
_tmp30C;return Cyc_Absyn_new_decl((void*)((_tmp30C=_cycalloc(sizeof(*_tmp30C)),((
_tmp30C[0]=((_tmp30E.tag=7,((_tmp30E.f1=((_tmp30D=_cycalloc(sizeof(*_tmp30D)),((
_tmp30D->sc=s,((_tmp30D->name=n,((_tmp30D->tvs=ts,((_tmp30D->fields=fs,((_tmp30D->is_extensible=
is_extensible,_tmp30D)))))))))))),_tmp30E)))),_tmp30C)))),loc);}static struct
_tuple7*Cyc_Absyn_expand_arg(struct _tuple7*a);static struct _tuple7*Cyc_Absyn_expand_arg(
struct _tuple7*a){struct _tuple7*_tmp30F;return(_tmp30F=_cycalloc(sizeof(*_tmp30F)),((
_tmp30F->f1=(*a).f1,((_tmp30F->f2=(*a).f2,((_tmp30F->f3=Cyc_Absyn_pointer_expand((*
a).f3,1),_tmp30F)))))));}void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,
struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*args,int c_varargs,
struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*
atts);void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*
eff_typ,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts){struct Cyc_Absyn_FnType_struct
_tmp315;struct Cyc_Absyn_FnInfo _tmp314;struct Cyc_Absyn_FnType_struct*_tmp313;
return(void*)((_tmp313=_cycalloc(sizeof(*_tmp313)),((_tmp313[0]=((_tmp315.tag=8,((
_tmp315.f1=((_tmp314.tvars=tvs,((_tmp314.ret_typ=Cyc_Absyn_pointer_expand(
ret_typ,0),((_tmp314.effect=eff_typ,((_tmp314.args=((struct Cyc_List_List*(*)(
struct _tuple7*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absyn_expand_arg,
args),((_tmp314.c_varargs=c_varargs,((_tmp314.cyc_varargs=cyc_varargs,((_tmp314.rgn_po=
rgn_po,((_tmp314.attributes=atts,_tmp314)))))))))))))))),_tmp315)))),_tmp313))));}
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar);void*Cyc_Absyn_pointer_expand(
void*t,int fresh_evar){void*_tmp13C=Cyc_Tcutil_compress(t);_LL70: if(_tmp13C <= (
void*)4)goto _LL72;if(*((int*)_tmp13C)!= 8)goto _LL72;_LL71: {struct Cyc_Core_Opt*
_tmp316;return Cyc_Absyn_at_typ(t,fresh_evar?Cyc_Absyn_new_evar(((_tmp316=
_cycalloc(sizeof(*_tmp316)),((_tmp316->v=(void*)((void*)3),_tmp316)))),0):(void*)
2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);}_LL72:;_LL73: return t;_LL6F:;}
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*
e){void*_tmp13E=e->r;void*_tmp13F;void*_tmp140;struct Cyc_Absyn_Vardecl*_tmp141;
void*_tmp142;struct Cyc_Absyn_Vardecl*_tmp143;struct Cyc_Absyn_Exp*_tmp144;struct
Cyc_Absyn_Exp*_tmp145;struct Cyc_Absyn_Exp*_tmp146;_LL75: if(*((int*)_tmp13E)!= 1)
goto _LL77;_tmp13F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp13E)->f2;if(_tmp13F
<= (void*)1)goto _LL77;if(*((int*)_tmp13F)!= 1)goto _LL77;_LL76: return 0;_LL77: if(*((
int*)_tmp13E)!= 1)goto _LL79;_tmp140=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13E)->f2;if(_tmp140 <= (void*)1)goto _LL79;if(*((int*)_tmp140)!= 0)goto _LL79;
_tmp141=((struct Cyc_Absyn_Global_b_struct*)_tmp140)->f1;_LL78: _tmp143=_tmp141;
goto _LL7A;_LL79: if(*((int*)_tmp13E)!= 1)goto _LL7B;_tmp142=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13E)->f2;if(_tmp142 <= (void*)1)goto _LL7B;if(*((int*)_tmp142)!= 3)goto _LL7B;
_tmp143=((struct Cyc_Absyn_Local_b_struct*)_tmp142)->f1;_LL7A: {void*_tmp147=Cyc_Tcutil_compress(
_tmp143->type);_LL8C: if(_tmp147 <= (void*)4)goto _LL8E;if(*((int*)_tmp147)!= 7)
goto _LL8E;_LL8D: return 0;_LL8E:;_LL8F: return 1;_LL8B:;}_LL7B: if(*((int*)_tmp13E)!= 
1)goto _LL7D;_LL7C: goto _LL7E;_LL7D: if(*((int*)_tmp13E)!= 24)goto _LL7F;_LL7E: goto
_LL80;_LL7F: if(*((int*)_tmp13E)!= 22)goto _LL81;_LL80: goto _LL82;_LL81: if(*((int*)
_tmp13E)!= 25)goto _LL83;_LL82: return 1;_LL83: if(*((int*)_tmp13E)!= 23)goto _LL85;
_tmp144=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp13E)->f1;_LL84: return Cyc_Absyn_is_lvalue(
_tmp144);_LL85: if(*((int*)_tmp13E)!= 14)goto _LL87;_tmp145=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp13E)->f1;_LL86: return Cyc_Absyn_is_lvalue(_tmp145);_LL87: if(*((int*)_tmp13E)
!= 13)goto _LL89;_tmp146=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp13E)->f1;
_LL88: return Cyc_Absyn_is_lvalue(_tmp146);_LL89:;_LL8A: return 0;_LL74:;}struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _dyneither_ptr*v);struct
Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct
_dyneither_ptr*v){{struct Cyc_List_List*_tmp148=fields;for(0;_tmp148 != 0;_tmp148=
_tmp148->tl){if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)_tmp148->hd)->name,v)
== 0)return(struct Cyc_Absyn_Aggrfield*)((struct Cyc_Absyn_Aggrfield*)_tmp148->hd);}}
return 0;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*
ad,struct _dyneither_ptr*v);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*v){return ad->impl == 0?0: Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple9*
Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i);struct _tuple9*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*ts,int i){for(0;i != 0;-- i){if(ts == 0)return 0;ts=ts->tl;}if(ts
== 0)return 0;return(struct _tuple9*)((struct _tuple9*)ts->hd);}struct
_dyneither_ptr Cyc_Absyn_attribute2string(void*a);struct _dyneither_ptr Cyc_Absyn_attribute2string(
void*a){void*_tmp149=a;int _tmp14A;int _tmp14B;struct _dyneither_ptr _tmp14C;void*
_tmp14D;int _tmp14E;int _tmp14F;void*_tmp150;int _tmp151;int _tmp152;int _tmp153;
struct _dyneither_ptr _tmp154;_LL91: if(_tmp149 <= (void*)17)goto _LL93;if(*((int*)
_tmp149)!= 0)goto _LL93;_tmp14A=((struct Cyc_Absyn_Regparm_att_struct*)_tmp149)->f1;
_LL92: {const char*_tmp31A;void*_tmp319[1];struct Cyc_Int_pa_struct _tmp318;return(
struct _dyneither_ptr)((_tmp318.tag=1,((_tmp318.f1=(unsigned long)_tmp14A,((
_tmp319[0]=& _tmp318,Cyc_aprintf(((_tmp31A="regparm(%d)",_tag_dyneither(_tmp31A,
sizeof(char),12))),_tag_dyneither(_tmp319,sizeof(void*),1))))))));}_LL93: if((int)
_tmp149 != 0)goto _LL95;_LL94: {const char*_tmp31B;return(_tmp31B="stdcall",
_tag_dyneither(_tmp31B,sizeof(char),8));}_LL95: if((int)_tmp149 != 1)goto _LL97;
_LL96: {const char*_tmp31C;return(_tmp31C="cdecl",_tag_dyneither(_tmp31C,sizeof(
char),6));}_LL97: if((int)_tmp149 != 2)goto _LL99;_LL98: {const char*_tmp31D;return(
_tmp31D="fastcall",_tag_dyneither(_tmp31D,sizeof(char),9));}_LL99: if((int)
_tmp149 != 3)goto _LL9B;_LL9A: {const char*_tmp31E;return(_tmp31E="noreturn",
_tag_dyneither(_tmp31E,sizeof(char),9));}_LL9B: if((int)_tmp149 != 4)goto _LL9D;
_LL9C: {const char*_tmp31F;return(_tmp31F="const",_tag_dyneither(_tmp31F,sizeof(
char),6));}_LL9D: if(_tmp149 <= (void*)17)goto _LL9F;if(*((int*)_tmp149)!= 1)goto
_LL9F;_tmp14B=((struct Cyc_Absyn_Aligned_att_struct*)_tmp149)->f1;_LL9E: if(
_tmp14B == - 1){const char*_tmp320;return(_tmp320="aligned",_tag_dyneither(_tmp320,
sizeof(char),8));}else{void*_tmp15E=Cyc_Cyclone_c_compiler;_LLC2: if((int)_tmp15E
!= 0)goto _LLC4;_LLC3: {const char*_tmp324;void*_tmp323[1];struct Cyc_Int_pa_struct
_tmp322;return(struct _dyneither_ptr)((_tmp322.tag=1,((_tmp322.f1=(unsigned long)
_tmp14B,((_tmp323[0]=& _tmp322,Cyc_aprintf(((_tmp324="aligned(%d)",_tag_dyneither(
_tmp324,sizeof(char),12))),_tag_dyneither(_tmp323,sizeof(void*),1))))))));}_LLC4:
if((int)_tmp15E != 1)goto _LLC1;_LLC5: {const char*_tmp328;void*_tmp327[1];struct
Cyc_Int_pa_struct _tmp326;return(struct _dyneither_ptr)((_tmp326.tag=1,((_tmp326.f1=(
unsigned long)_tmp14B,((_tmp327[0]=& _tmp326,Cyc_aprintf(((_tmp328="align(%d)",
_tag_dyneither(_tmp328,sizeof(char),10))),_tag_dyneither(_tmp327,sizeof(void*),1))))))));}
_LLC1:;}_LL9F: if((int)_tmp149 != 5)goto _LLA1;_LLA0: {const char*_tmp329;return(
_tmp329="packed",_tag_dyneither(_tmp329,sizeof(char),7));}_LLA1: if(_tmp149 <= (
void*)17)goto _LLA3;if(*((int*)_tmp149)!= 2)goto _LLA3;_tmp14C=((struct Cyc_Absyn_Section_att_struct*)
_tmp149)->f1;_LLA2: {const char*_tmp32D;void*_tmp32C[1];struct Cyc_String_pa_struct
_tmp32B;return(struct _dyneither_ptr)((_tmp32B.tag=0,((_tmp32B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp14C),((_tmp32C[0]=& _tmp32B,Cyc_aprintf(((
_tmp32D="section(\"%s\")",_tag_dyneither(_tmp32D,sizeof(char),14))),
_tag_dyneither(_tmp32C,sizeof(void*),1))))))));}_LLA3: if((int)_tmp149 != 6)goto
_LLA5;_LLA4: {const char*_tmp32E;return(_tmp32E="nocommon",_tag_dyneither(_tmp32E,
sizeof(char),9));}_LLA5: if((int)_tmp149 != 7)goto _LLA7;_LLA6: {const char*_tmp32F;
return(_tmp32F="shared",_tag_dyneither(_tmp32F,sizeof(char),7));}_LLA7: if((int)
_tmp149 != 8)goto _LLA9;_LLA8: {const char*_tmp330;return(_tmp330="unused",
_tag_dyneither(_tmp330,sizeof(char),7));}_LLA9: if((int)_tmp149 != 9)goto _LLAB;
_LLAA: {const char*_tmp331;return(_tmp331="weak",_tag_dyneither(_tmp331,sizeof(
char),5));}_LLAB: if((int)_tmp149 != 10)goto _LLAD;_LLAC: {const char*_tmp332;return(
_tmp332="dllimport",_tag_dyneither(_tmp332,sizeof(char),10));}_LLAD: if((int)
_tmp149 != 11)goto _LLAF;_LLAE: {const char*_tmp333;return(_tmp333="dllexport",
_tag_dyneither(_tmp333,sizeof(char),10));}_LLAF: if((int)_tmp149 != 12)goto _LLB1;
_LLB0: {const char*_tmp334;return(_tmp334="no_instrument_function",_tag_dyneither(
_tmp334,sizeof(char),23));}_LLB1: if((int)_tmp149 != 13)goto _LLB3;_LLB2: {const
char*_tmp335;return(_tmp335="constructor",_tag_dyneither(_tmp335,sizeof(char),12));}
_LLB3: if((int)_tmp149 != 14)goto _LLB5;_LLB4: {const char*_tmp336;return(_tmp336="destructor",
_tag_dyneither(_tmp336,sizeof(char),11));}_LLB5: if((int)_tmp149 != 15)goto _LLB7;
_LLB6: {const char*_tmp337;return(_tmp337="no_check_memory_usage",_tag_dyneither(
_tmp337,sizeof(char),22));}_LLB7: if(_tmp149 <= (void*)17)goto _LLBD;if(*((int*)
_tmp149)!= 3)goto _LLB9;_tmp14D=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp149)->f1;if((int)_tmp14D != 0)goto _LLB9;_tmp14E=((struct Cyc_Absyn_Format_att_struct*)
_tmp149)->f2;_tmp14F=((struct Cyc_Absyn_Format_att_struct*)_tmp149)->f3;_LLB8: {
const char*_tmp33C;void*_tmp33B[2];struct Cyc_Int_pa_struct _tmp33A;struct Cyc_Int_pa_struct
_tmp339;return(struct _dyneither_ptr)((_tmp339.tag=1,((_tmp339.f1=(unsigned int)
_tmp14F,((_tmp33A.tag=1,((_tmp33A.f1=(unsigned int)_tmp14E,((_tmp33B[0]=& _tmp33A,((
_tmp33B[1]=& _tmp339,Cyc_aprintf(((_tmp33C="format(printf,%u,%u)",_tag_dyneither(
_tmp33C,sizeof(char),21))),_tag_dyneither(_tmp33B,sizeof(void*),2))))))))))))));}
_LLB9: if(*((int*)_tmp149)!= 3)goto _LLBB;_tmp150=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp149)->f1;if((int)_tmp150 != 1)goto _LLBB;_tmp151=((struct Cyc_Absyn_Format_att_struct*)
_tmp149)->f2;_tmp152=((struct Cyc_Absyn_Format_att_struct*)_tmp149)->f3;_LLBA: {
const char*_tmp341;void*_tmp340[2];struct Cyc_Int_pa_struct _tmp33F;struct Cyc_Int_pa_struct
_tmp33E;return(struct _dyneither_ptr)((_tmp33E.tag=1,((_tmp33E.f1=(unsigned int)
_tmp152,((_tmp33F.tag=1,((_tmp33F.f1=(unsigned int)_tmp151,((_tmp340[0]=& _tmp33F,((
_tmp340[1]=& _tmp33E,Cyc_aprintf(((_tmp341="format(scanf,%u,%u)",_tag_dyneither(
_tmp341,sizeof(char),20))),_tag_dyneither(_tmp340,sizeof(void*),2))))))))))))));}
_LLBB: if(*((int*)_tmp149)!= 4)goto _LLBD;_tmp153=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp149)->f1;_LLBC: {const char*_tmp345;void*_tmp344[1];struct Cyc_Int_pa_struct
_tmp343;return(struct _dyneither_ptr)((_tmp343.tag=1,((_tmp343.f1=(unsigned long)
_tmp153,((_tmp344[0]=& _tmp343,Cyc_aprintf(((_tmp345="initializes(%d)",
_tag_dyneither(_tmp345,sizeof(char),16))),_tag_dyneither(_tmp344,sizeof(void*),1))))))));}
_LLBD: if((int)_tmp149 != 16)goto _LLBF;_LLBE: {const char*_tmp346;return(_tmp346="pure",
_tag_dyneither(_tmp346,sizeof(char),5));}_LLBF: if(_tmp149 <= (void*)17)goto _LL90;
if(*((int*)_tmp149)!= 5)goto _LL90;_tmp154=((struct Cyc_Absyn_Mode_att_struct*)
_tmp149)->f1;_LLC0: {const char*_tmp34A;void*_tmp349[1];struct Cyc_String_pa_struct
_tmp348;return(struct _dyneither_ptr)((_tmp348.tag=0,((_tmp348.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp154),((_tmp349[0]=& _tmp348,Cyc_aprintf(((
_tmp34A="__mode__(\"%s\")",_tag_dyneither(_tmp34A,sizeof(char),15))),
_tag_dyneither(_tmp349,sizeof(void*),1))))))));}_LL90:;}int Cyc_Absyn_fntype_att(
void*a);int Cyc_Absyn_fntype_att(void*a){void*_tmp182=a;_LLC7: if(_tmp182 <= (void*)
17)goto _LLC9;if(*((int*)_tmp182)!= 0)goto _LLC9;_LLC8: goto _LLCA;_LLC9: if((int)
_tmp182 != 2)goto _LLCB;_LLCA: goto _LLCC;_LLCB: if((int)_tmp182 != 0)goto _LLCD;_LLCC:
goto _LLCE;_LLCD: if((int)_tmp182 != 1)goto _LLCF;_LLCE: goto _LLD0;_LLCF: if((int)
_tmp182 != 3)goto _LLD1;_LLD0: goto _LLD2;_LLD1: if((int)_tmp182 != 16)goto _LLD3;_LLD2:
goto _LLD4;_LLD3: if(_tmp182 <= (void*)17)goto _LLD5;if(*((int*)_tmp182)!= 3)goto
_LLD5;_LLD4: goto _LLD6;_LLD5: if((int)_tmp182 != 4)goto _LLD7;_LLD6: return 1;_LLD7:
if(_tmp182 <= (void*)17)goto _LLD9;if(*((int*)_tmp182)!= 4)goto _LLD9;_LLD8: return 1;
_LLD9:;_LLDA: return 0;_LLC6:;}static char _tmp183[3]="f0";static struct
_dyneither_ptr Cyc_Absyn_f0={_tmp183,_tmp183,_tmp183 + 3};static struct
_dyneither_ptr*Cyc_Absyn_field_names_v[1]={& Cyc_Absyn_f0};static struct
_dyneither_ptr Cyc_Absyn_field_names={(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(
void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct
_dyneither_ptr**)Cyc_Absyn_field_names_v + 1)};struct _dyneither_ptr*Cyc_Absyn_fieldname(
int i);static void _tmp358(unsigned int*fsz,unsigned int*_tmp357,unsigned int*
_tmp356,struct _dyneither_ptr***_tmp354){for(*_tmp357=0;*_tmp357 < *_tmp356;(*
_tmp357)++){struct Cyc_Int_pa_struct _tmp352;void*_tmp351[1];const char*_tmp350;
struct _dyneither_ptr*_tmp34F;(*_tmp354)[*_tmp357]=*_tmp357 < *fsz?*((struct
_dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct
_dyneither_ptr*),(int)*_tmp357)):((_tmp34F=_cycalloc(sizeof(*_tmp34F)),((_tmp34F[
0]=(struct _dyneither_ptr)((_tmp352.tag=1,((_tmp352.f1=(unsigned long)((int)*
_tmp357),((_tmp351[0]=& _tmp352,Cyc_aprintf(((_tmp350="f%d",_tag_dyneither(
_tmp350,sizeof(char),4))),_tag_dyneither(_tmp351,sizeof(void*),1)))))))),_tmp34F))));}}
struct _dyneither_ptr*Cyc_Absyn_fieldname(int i){unsigned int fsz=
_get_dyneither_size(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*));if(i >= 
fsz){unsigned int _tmp357;unsigned int _tmp356;struct _dyneither_ptr _tmp355;struct
_dyneither_ptr**_tmp354;unsigned int _tmp353;Cyc_Absyn_field_names=((_tmp353=(
unsigned int)(i + 1),((_tmp354=(struct _dyneither_ptr**)_cycalloc(_check_times(
sizeof(struct _dyneither_ptr*),_tmp353)),((_tmp355=_tag_dyneither(_tmp354,sizeof(
struct _dyneither_ptr*),_tmp353),((((_tmp356=_tmp353,_tmp358(& fsz,& _tmp357,&
_tmp356,& _tmp354))),_tmp355))))))));}return*((struct _dyneither_ptr**)
_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),i));}
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU info);struct
_tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU info){union Cyc_Absyn_AggrInfoU
_tmp18C=info;struct _tuple2 _tmp18D;void*_tmp18E;struct _tuple0*_tmp18F;struct Cyc_Absyn_Aggrdecl**
_tmp190;struct Cyc_Absyn_Aggrdecl*_tmp191;struct Cyc_Absyn_Aggrdecl _tmp192;void*
_tmp193;struct _tuple0*_tmp194;_LLDC: if((_tmp18C.UnknownAggr).tag != 1)goto _LLDE;
_tmp18D=(struct _tuple2)(_tmp18C.UnknownAggr).val;_tmp18E=_tmp18D.f1;_tmp18F=
_tmp18D.f2;_LLDD: {struct _tuple10 _tmp359;return(_tmp359.f1=_tmp18E,((_tmp359.f2=
_tmp18F,_tmp359)));}_LLDE: if((_tmp18C.KnownAggr).tag != 2)goto _LLDB;_tmp190=(
struct Cyc_Absyn_Aggrdecl**)(_tmp18C.KnownAggr).val;_tmp191=*_tmp190;_tmp192=*
_tmp191;_tmp193=_tmp192.kind;_tmp194=_tmp192.name;_LLDF: {struct _tuple10 _tmp35A;
return(_tmp35A.f1=_tmp193,((_tmp35A.f2=_tmp194,_tmp35A)));}_LLDB:;}struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info){union Cyc_Absyn_AggrInfoU
_tmp197=info;struct _tuple2 _tmp198;struct Cyc_Absyn_Aggrdecl**_tmp199;struct Cyc_Absyn_Aggrdecl*
_tmp19A;_LLE1: if((_tmp197.UnknownAggr).tag != 1)goto _LLE3;_tmp198=(struct _tuple2)(
_tmp197.UnknownAggr).val;_LLE2: {const char*_tmp35D;void*_tmp35C;(_tmp35C=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp35D="unchecked aggrdecl",
_tag_dyneither(_tmp35D,sizeof(char),19))),_tag_dyneither(_tmp35C,sizeof(void*),0)));}
_LLE3: if((_tmp197.KnownAggr).tag != 2)goto _LLE0;_tmp199=(struct Cyc_Absyn_Aggrdecl**)(
_tmp197.KnownAggr).val;_tmp19A=*_tmp199;_LLE4: return _tmp19A;_LLE0:;}int Cyc_Absyn_is_union_type(
void*t);int Cyc_Absyn_is_union_type(void*t){void*_tmp19D=Cyc_Tcutil_compress(t);
void*_tmp19E;struct Cyc_Absyn_AggrInfo _tmp19F;union Cyc_Absyn_AggrInfoU _tmp1A0;
_LLE6: if(_tmp19D <= (void*)4)goto _LLEA;if(*((int*)_tmp19D)!= 11)goto _LLE8;_tmp19E=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp19D)->f1;if((int)_tmp19E != 1)
goto _LLE8;_LLE7: return 1;_LLE8: if(*((int*)_tmp19D)!= 10)goto _LLEA;_tmp19F=((
struct Cyc_Absyn_AggrType_struct*)_tmp19D)->f1;_tmp1A0=_tmp19F.aggr_info;_LLE9:
return(Cyc_Absyn_aggr_kinded_name(_tmp1A0)).f1 == (void*)1;_LLEA:;_LLEB: return 0;
_LLE5:;}int Cyc_Absyn_is_nontagged_union_type(void*t);int Cyc_Absyn_is_nontagged_union_type(
void*t){void*_tmp1A1=Cyc_Tcutil_compress(t);void*_tmp1A2;struct Cyc_Absyn_AggrInfo
_tmp1A3;union Cyc_Absyn_AggrInfoU _tmp1A4;_LLED: if(_tmp1A1 <= (void*)4)goto _LLF1;
if(*((int*)_tmp1A1)!= 11)goto _LLEF;_tmp1A2=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1A1)->f1;if((int)_tmp1A2 != 1)goto _LLEF;_LLEE: return 1;_LLEF: if(*((int*)
_tmp1A1)!= 10)goto _LLF1;_tmp1A3=((struct Cyc_Absyn_AggrType_struct*)_tmp1A1)->f1;
_tmp1A4=_tmp1A3.aggr_info;_LLF0: {union Cyc_Absyn_AggrInfoU _tmp1A5=_tmp1A4;struct
Cyc_Absyn_Aggrdecl**_tmp1A6;struct Cyc_Absyn_Aggrdecl*_tmp1A7;struct _tuple2
_tmp1A8;void*_tmp1A9;struct Cyc_Core_Opt*_tmp1AA;struct _tuple2 _tmp1AB;void*
_tmp1AC;struct Cyc_Core_Opt*_tmp1AD;struct Cyc_Core_Opt _tmp1AE;int _tmp1AF;_LLF4:
if((_tmp1A5.KnownAggr).tag != 2)goto _LLF6;_tmp1A6=(struct Cyc_Absyn_Aggrdecl**)(
_tmp1A5.KnownAggr).val;_tmp1A7=*_tmp1A6;_LLF5: return _tmp1A7->kind == (void*)1  && ((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1A7->impl))->tagged == 0;_LLF6: if((
_tmp1A5.UnknownAggr).tag != 1)goto _LLF8;_tmp1A8=(struct _tuple2)(_tmp1A5.UnknownAggr).val;
_tmp1A9=_tmp1A8.f1;_tmp1AA=_tmp1A8.f3;if(_tmp1AA != 0)goto _LLF8;_LLF7: return
_tmp1A9 == (void*)1;_LLF8: if((_tmp1A5.UnknownAggr).tag != 1)goto _LLF3;_tmp1AB=(
struct _tuple2)(_tmp1A5.UnknownAggr).val;_tmp1AC=_tmp1AB.f1;_tmp1AD=_tmp1AB.f3;
if(_tmp1AD == 0)goto _LLF3;_tmp1AE=*_tmp1AD;_tmp1AF=(int)_tmp1AE.v;_LLF9: return
_tmp1AC == (void*)1  && !_tmp1AF;_LLF3:;}_LLF1:;_LLF2: return 0;_LLEC:;}int Cyc_Absyn_is_aggr_type(
void*t);int Cyc_Absyn_is_aggr_type(void*t){void*_tmp1B0=Cyc_Tcutil_compress(t);
_LLFB: if(_tmp1B0 <= (void*)4)goto _LL105;if(*((int*)_tmp1B0)!= 2)goto _LLFD;_LLFC:
goto _LLFE;_LLFD: if(*((int*)_tmp1B0)!= 3)goto _LLFF;_LLFE: goto _LL100;_LLFF: if(*((
int*)_tmp1B0)!= 9)goto _LL101;_LL100: goto _LL102;_LL101: if(*((int*)_tmp1B0)!= 10)
goto _LL103;_LL102: goto _LL104;_LL103: if(*((int*)_tmp1B0)!= 11)goto _LL105;_LL104:
return 1;_LL105:;_LL106: return 0;_LLFA:;}int Cyc_Absyn_porting_c_code=0;int Cyc_Absyn_no_regions=
0;
