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
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
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
f1;};struct Cyc_Absyn_Region_d_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
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
struct Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern
struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_force_kb(
void*kb);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);void*Cyc_Absyn_int_typ(void*,void*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_float_typ;
void*Cyc_Absyn_double_typ(int);extern void*Cyc_Absyn_empty_effect;extern struct
_tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;
extern void*Cyc_Absyn_exn_typ;extern struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar;
extern struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar;void*Cyc_Absyn_string_typ(
void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_file_typ();
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct
Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_star_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _dyneither_ptr*name);void*
Cyc_Absyn_strctq(struct _tuple0*name);void*Cyc_Absyn_unionq_typ(struct _tuple0*
name);void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name);void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct
Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_bool_exp(int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);
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
struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs);struct
Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Core_Opt*fs,int is_xtunion,int is_flat,struct Cyc_Position_Segment*loc);
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,
void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);void*Cyc_Absyn_pointer_expand(
void*,int fresh_evar);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
struct _tuple3{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple3*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*,int);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
int Cyc_Absyn_fntype_att(void*a);struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
struct _tuple4{void*f1;struct _tuple0*f2;};struct _tuple4 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU_union);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_aggr_type(
void*t);void Cyc_Absyn_print_decls(struct Cyc_List_List*);extern int Cyc_Absyn_porting_c_code;
extern int Cyc_Absyn_no_regions;struct Cyc_Typerep_Int_struct{int tag;int f1;
unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;void*f2;
};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple5{unsigned int f1;
struct _dyneither_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{int tag;struct
_dyneither_ptr*f1;unsigned int f2;struct _dyneither_ptr f3;};struct _tuple6{
unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;
struct _dyneither_ptr f2;};struct _tuple7{unsigned int f1;struct _dyneither_ptr f2;};
struct Cyc_Typerep_TUnion_struct{int tag;struct _dyneither_ptr f1;struct
_dyneither_ptr f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;unsigned int f3;struct
_dyneither_ptr f4;};struct _tuple8{struct _dyneither_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);extern void*Cyc_decls_rep;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc___cycFILE;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dyneither_ptr f1;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char
Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);
struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern
char Cyc_Dict_Absent[11];struct _tuple9{void*f1;void*f2;};struct _tuple9*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple9*Cyc_Dict_rchoose(
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
_dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);void*Cyc_Tcutil_kind_to_bound(
void*k);void Cyc_Marshal_print_type(void*rep,void*val);struct _tuple10{struct Cyc_Dict_Dict
f1;int f2;};struct _tuple11{struct _dyneither_ptr f1;int f2;};extern int Cyc_Cyclone_tovc_r;
extern void*Cyc_Cyclone_c_compiler;int Cyc_Cyclone_tovc_r=0;void*Cyc_Cyclone_c_compiler=(
void*)0;static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*
ss2);static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*
ss2){return((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,
ss1,ss2);}int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct Cyc_List_List*
vs2);int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct Cyc_List_List*vs2){
if((int)vs1 == (int)vs2)return 0;return Cyc_Absyn_strlist_cmp(vs1,vs2);}struct
_tuple12{union Cyc_Absyn_Nmspace_union f1;union Cyc_Absyn_Nmspace_union f2;};int Cyc_Absyn_qvar_cmp(
struct _tuple0*q1,struct _tuple0*q2);int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct
_tuple0*q2){union Cyc_Absyn_Nmspace_union _tmp0=(*q1).f1;union Cyc_Absyn_Nmspace_union
_tmp1=(*q2).f1;{struct _tuple12 _tmp4A8;struct _tuple12 _tmp3=(_tmp4A8.f1=_tmp0,((
_tmp4A8.f2=_tmp1,_tmp4A8)));union Cyc_Absyn_Nmspace_union _tmp4;union Cyc_Absyn_Nmspace_union
_tmp5;union Cyc_Absyn_Nmspace_union _tmp6;struct Cyc_List_List*_tmp7;union Cyc_Absyn_Nmspace_union
_tmp8;struct Cyc_List_List*_tmp9;union Cyc_Absyn_Nmspace_union _tmpA;struct Cyc_List_List*
_tmpB;union Cyc_Absyn_Nmspace_union _tmpC;struct Cyc_List_List*_tmpD;union Cyc_Absyn_Nmspace_union
_tmpE;union Cyc_Absyn_Nmspace_union _tmpF;union Cyc_Absyn_Nmspace_union _tmp10;union
Cyc_Absyn_Nmspace_union _tmp11;_LL1: _tmp4=_tmp3.f1;if(((_tmp3.f1).Loc_n).tag != 0)
goto _LL3;_tmp5=_tmp3.f2;if(((_tmp3.f2).Loc_n).tag != 0)goto _LL3;_LL2: goto _LL0;
_LL3: _tmp6=_tmp3.f1;if(((_tmp3.f1).Rel_n).tag != 1)goto _LL5;_tmp7=(_tmp6.Rel_n).f1;
_tmp8=_tmp3.f2;if(((_tmp3.f2).Rel_n).tag != 1)goto _LL5;_tmp9=(_tmp8.Rel_n).f1;
_LL4: _tmpB=_tmp7;_tmpD=_tmp9;goto _LL6;_LL5: _tmpA=_tmp3.f1;if(((_tmp3.f1).Abs_n).tag
!= 2)goto _LL7;_tmpB=(_tmpA.Abs_n).f1;_tmpC=_tmp3.f2;if(((_tmp3.f2).Abs_n).tag != 
2)goto _LL7;_tmpD=(_tmpC.Abs_n).f1;_LL6: {int i=Cyc_Absyn_strlist_cmp(_tmpB,_tmpD);
if(i != 0)return i;goto _LL0;}_LL7: _tmpE=_tmp3.f1;if(((_tmp3.f1).Loc_n).tag != 0)
goto _LL9;_LL8: return - 1;_LL9: _tmpF=_tmp3.f2;if(((_tmp3.f2).Loc_n).tag != 0)goto
_LLB;_LLA: return 1;_LLB: _tmp10=_tmp3.f1;if(((_tmp3.f1).Rel_n).tag != 1)goto _LLD;
_LLC: return - 1;_LLD: _tmp11=_tmp3.f2;if(((_tmp3.f2).Rel_n).tag != 1)goto _LL0;_LLE:
return 1;_LL0:;}return Cyc_strptrcmp((*q1).f2,(*q2).f2);}int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2);int Cyc_Absyn_tvar_cmp(struct
Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){int i=Cyc_strptrcmp(tv1->name,tv2->name);
if(i != 0)return i;return tv1->identity - tv2->identity;}union Cyc_Absyn_Nmspace_union
Cyc_Absyn_rel_ns_null=(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Rel_n_struct){
1,0});int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv);int Cyc_Absyn_is_qvar_qualified(
struct _tuple0*qv){union Cyc_Absyn_Nmspace_union _tmp13=(*qv).f1;struct Cyc_List_List*
_tmp14;struct Cyc_List_List*_tmp15;_LL10: if((_tmp13.Rel_n).tag != 1)goto _LL12;
_tmp14=(_tmp13.Rel_n).f1;if(_tmp14 != 0)goto _LL12;_LL11: goto _LL13;_LL12: if((
_tmp13.Abs_n).tag != 2)goto _LL14;_tmp15=(_tmp13.Abs_n).f1;if(_tmp15 != 0)goto _LL14;
_LL13: goto _LL15;_LL14: if((_tmp13.Loc_n).tag != 0)goto _LL16;_LL15: return 0;_LL16:;
_LL17: return 1;_LLF:;}static int Cyc_Absyn_new_type_counter=0;void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*env){struct Cyc_Absyn_Evar_struct _tmp4AB;struct Cyc_Absyn_Evar_struct*
_tmp4AA;return(void*)((_tmp4AA=_cycalloc(sizeof(*_tmp4AA)),((_tmp4AA[0]=((
_tmp4AB.tag=0,((_tmp4AB.f1=k,((_tmp4AB.f2=0,((_tmp4AB.f3=Cyc_Absyn_new_type_counter
++,((_tmp4AB.f4=env,_tmp4AB)))))))))),_tmp4AA))));}static struct Cyc_Core_Opt Cyc_Absyn_mk={(
void*)((void*)1)};void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(
struct Cyc_Core_Opt*tenv){return Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Absyn_mk,
tenv);}struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,
struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual
x,struct Cyc_Absyn_Tqual y){struct Cyc_Absyn_Tqual _tmp4AC;return(_tmp4AC.print_const=
x.print_const  || y.print_const,((_tmp4AC.q_volatile=x.q_volatile  || y.q_volatile,((
_tmp4AC.q_restrict=x.q_restrict  || y.q_restrict,((_tmp4AC.real_const=x.real_const
 || y.real_const,((_tmp4AC.loc=Cyc_Position_segment_join(x.loc,y.loc),_tmp4AC)))))))));}
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Tqual _tmp4AD;return(_tmp4AD.print_const=0,((_tmp4AD.q_volatile=0,((
_tmp4AD.q_restrict=0,((_tmp4AD.real_const=0,((_tmp4AD.loc=loc,_tmp4AD)))))))));}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Tqual _tmp4AE;return(_tmp4AE.print_const=1,((_tmp4AE.q_volatile=0,((
_tmp4AE.q_restrict=0,((_tmp4AE.real_const=1,((_tmp4AE.loc=loc,_tmp4AE)))))))));}
static struct Cyc_Absyn_Const_e_struct Cyc_Absyn_one_b_raw={0,(union Cyc_Absyn_Cnst_union)((
struct Cyc_Absyn_Int_c_struct){2,(void*)((void*)1),1})};struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={
0,(void*)((void*)& Cyc_Absyn_one_b_raw),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=& Cyc_Absyn_exp_unsigned_one_v;
static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={0,& Cyc_Absyn_exp_unsigned_one_v};
void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(
void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x){union Cyc_Absyn_Constraint_union
_tmp4B1;struct Cyc_Absyn_Conref*_tmp4B0;return(_tmp4B0=_cycalloc(sizeof(*_tmp4B0)),((
_tmp4B0->v=(union Cyc_Absyn_Constraint_union)(((_tmp4B1.Eq_constr).tag=0,(((
_tmp4B1.Eq_constr).f1=(void*)x,_tmp4B1)))),_tmp4B0)));}struct Cyc_Absyn_Conref*
Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref(){union
Cyc_Absyn_Constraint_union _tmp4B4;struct Cyc_Absyn_Conref*_tmp4B3;return(_tmp4B3=
_cycalloc(sizeof(*_tmp4B3)),((_tmp4B3->v=(union Cyc_Absyn_Constraint_union)(((
_tmp4B4.No_constr).tag=2,_tmp4B4)),_tmp4B3)));}static struct Cyc_Absyn_Conref Cyc_Absyn_true_conref_v={(
union Cyc_Absyn_Constraint_union)((struct Cyc_Absyn_Eq_constr_struct){0,(void*)1})};
struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v;static
struct Cyc_Absyn_Conref Cyc_Absyn_false_conref_v={(union Cyc_Absyn_Constraint_union)((
struct Cyc_Absyn_Eq_constr_struct){0,(void*)0})};struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref=&
Cyc_Absyn_false_conref_v;static struct Cyc_Absyn_Conref Cyc_Absyn_bounds_one_conref_v={(
union Cyc_Absyn_Constraint_union)((struct Cyc_Absyn_Eq_constr_struct){0,(void*)((
void*)& Cyc_Absyn_one_bt)})};struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref=&
Cyc_Absyn_bounds_one_conref_v;static struct Cyc_Absyn_Conref Cyc_Absyn_bounds_dyneither_conref_v={(
union Cyc_Absyn_Constraint_union)((struct Cyc_Absyn_Eq_constr_struct){0,(void*)((
void*)0)})};struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dyneither_conref=& Cyc_Absyn_bounds_dyneither_conref_v;
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x){union
Cyc_Absyn_Constraint_union _tmp26=x->v;struct Cyc_Absyn_Conref*_tmp27;_LL19: if((
_tmp26.No_constr).tag != 2)goto _LL1B;_LL1A: goto _LL1C;_LL1B: if((_tmp26.Eq_constr).tag
!= 0)goto _LL1D;_LL1C: return x;_LL1D: if((_tmp26.Forward_constr).tag != 1)goto _LL18;
_tmp27=(_tmp26.Forward_constr).f1;_LL1E: {struct Cyc_Absyn_Conref*_tmp28=Cyc_Absyn_compress_conref(
_tmp27);{union Cyc_Absyn_Constraint_union _tmp4B5;x->v=(union Cyc_Absyn_Constraint_union)(((
_tmp4B5.Forward_constr).tag=1,(((_tmp4B5.Forward_constr).f1=_tmp28,_tmp4B5))));}
return _tmp28;}_LL18:;}void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);void*
Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x){union Cyc_Absyn_Constraint_union
_tmp2A=(Cyc_Absyn_compress_conref(x))->v;void*_tmp2B;_LL20: if((_tmp2A.Eq_constr).tag
!= 0)goto _LL22;_tmp2B=(_tmp2A.Eq_constr).f1;_LL21: return _tmp2B;_LL22:;_LL23: {
const char*_tmp4B8;void*_tmp4B7;(_tmp4B7=0,Cyc_Tcutil_impos(((_tmp4B8="conref_val",
_tag_dyneither(_tmp4B8,sizeof(char),11))),_tag_dyneither(_tmp4B7,sizeof(void*),0)));}
_LL1F:;}void*Cyc_Absyn_conref_def(void*y,struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(
void*y,struct Cyc_Absyn_Conref*x){union Cyc_Absyn_Constraint_union _tmp2E=(Cyc_Absyn_compress_conref(
x))->v;void*_tmp2F;_LL25: if((_tmp2E.Eq_constr).tag != 0)goto _LL27;_tmp2F=(_tmp2E.Eq_constr).f1;
_LL26: return _tmp2F;_LL27:;_LL28: return y;_LL24:;}void*Cyc_Absyn_compress_kb(void*
k);void*Cyc_Absyn_compress_kb(void*k){void*_tmp30=k;struct Cyc_Core_Opt*_tmp31;
struct Cyc_Core_Opt*_tmp32;struct Cyc_Core_Opt*_tmp33;struct Cyc_Core_Opt _tmp34;
void*_tmp35;void**_tmp36;struct Cyc_Core_Opt*_tmp37;struct Cyc_Core_Opt _tmp38;void*
_tmp39;void**_tmp3A;_LL2A: if(*((int*)_tmp30)!= 0)goto _LL2C;_LL2B: goto _LL2D;_LL2C:
if(*((int*)_tmp30)!= 1)goto _LL2E;_tmp31=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp30)->f1;if(_tmp31 != 0)goto _LL2E;_LL2D: goto _LL2F;_LL2E: if(*((int*)_tmp30)!= 2)
goto _LL30;_tmp32=((struct Cyc_Absyn_Less_kb_struct*)_tmp30)->f1;if(_tmp32 != 0)
goto _LL30;_LL2F: return k;_LL30: if(*((int*)_tmp30)!= 1)goto _LL32;_tmp33=((struct
Cyc_Absyn_Unknown_kb_struct*)_tmp30)->f1;if(_tmp33 == 0)goto _LL32;_tmp34=*_tmp33;
_tmp35=(void*)_tmp34.v;_tmp36=(void**)&(*((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp30)->f1).v;_LL31: _tmp3A=_tmp36;goto _LL33;_LL32: if(*((int*)_tmp30)!= 2)goto
_LL29;_tmp37=((struct Cyc_Absyn_Less_kb_struct*)_tmp30)->f1;if(_tmp37 == 0)goto
_LL29;_tmp38=*_tmp37;_tmp39=(void*)_tmp38.v;_tmp3A=(void**)&(*((struct Cyc_Absyn_Less_kb_struct*)
_tmp30)->f1).v;_LL33:*_tmp3A=Cyc_Absyn_compress_kb(*_tmp3A);return*_tmp3A;_LL29:;}
void*Cyc_Absyn_force_kb(void*kb);void*Cyc_Absyn_force_kb(void*kb){void*_tmp3B=
Cyc_Absyn_compress_kb(kb);void*_tmp3C;struct Cyc_Core_Opt*_tmp3D;struct Cyc_Core_Opt**
_tmp3E;struct Cyc_Core_Opt*_tmp3F;struct Cyc_Core_Opt**_tmp40;void*_tmp41;_LL35:
if(*((int*)_tmp3B)!= 0)goto _LL37;_tmp3C=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp3B)->f1;_LL36: return _tmp3C;_LL37: if(*((int*)_tmp3B)!= 1)goto _LL39;_tmp3D=((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp3B)->f1;_tmp3E=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp3B)->f1;_LL38: _tmp40=_tmp3E;_tmp41=(void*)
2;goto _LL3A;_LL39: if(*((int*)_tmp3B)!= 2)goto _LL34;_tmp3F=((struct Cyc_Absyn_Less_kb_struct*)
_tmp3B)->f1;_tmp40=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp3B)->f1;_tmp41=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp3B)->f2;_LL3A:{
struct Cyc_Core_Opt*_tmp4B9;*_tmp40=((_tmp4B9=_cycalloc(sizeof(*_tmp4B9)),((
_tmp4B9->v=(void*)Cyc_Tcutil_kind_to_bound(_tmp41),_tmp4B9))));}return _tmp41;
_LL34:;}static struct Cyc_Absyn_IntType_struct Cyc_Absyn_char_tt={5,(void*)((void*)
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
void*sn,void*sz){void*_tmp52=sn;_LL3C: if((int)_tmp52 != 0)goto _LL3E;_LL3D: {void*
_tmp53=sz;_LL43: if((int)_tmp53 != 0)goto _LL45;_LL44: return Cyc_Absyn_schar_typ;
_LL45: if((int)_tmp53 != 1)goto _LL47;_LL46: return Cyc_Absyn_sshort_typ;_LL47: if((
int)_tmp53 != 2)goto _LL49;_LL48: return Cyc_Absyn_sint_typ;_LL49: if((int)_tmp53 != 3)
goto _LL4B;_LL4A: return Cyc_Absyn_slong_typ;_LL4B: if((int)_tmp53 != 4)goto _LL42;
_LL4C: return Cyc_Absyn_slonglong_typ;_LL42:;}_LL3E: if((int)_tmp52 != 1)goto _LL40;
_LL3F: {void*_tmp54=sz;_LL4E: if((int)_tmp54 != 0)goto _LL50;_LL4F: return Cyc_Absyn_uchar_typ;
_LL50: if((int)_tmp54 != 1)goto _LL52;_LL51: return Cyc_Absyn_ushort_typ;_LL52: if((
int)_tmp54 != 2)goto _LL54;_LL53: return Cyc_Absyn_uint_typ;_LL54: if((int)_tmp54 != 3)
goto _LL56;_LL55: return Cyc_Absyn_ulong_typ;_LL56: if((int)_tmp54 != 4)goto _LL4D;
_LL57: return Cyc_Absyn_ulonglong_typ;_LL4D:;}_LL40: if((int)_tmp52 != 2)goto _LL3B;
_LL41: {void*_tmp55=sz;_LL59: if((int)_tmp55 != 0)goto _LL5B;_LL5A: return Cyc_Absyn_char_typ;
_LL5B: if((int)_tmp55 != 1)goto _LL5D;_LL5C: return Cyc_Absyn_nshort_typ;_LL5D: if((
int)_tmp55 != 2)goto _LL5F;_LL5E: return Cyc_Absyn_nint_typ;_LL5F: if((int)_tmp55 != 3)
goto _LL61;_LL60: return Cyc_Absyn_nlong_typ;_LL61: if((int)_tmp55 != 4)goto _LL58;
_LL62: return Cyc_Absyn_nlonglong_typ;_LL58:;}_LL3B:;}void*Cyc_Absyn_float_typ=(
void*)1;void*Cyc_Absyn_double_typ(int b);void*Cyc_Absyn_double_typ(int b){static
struct Cyc_Absyn_DoubleType_struct dt={6,1};static struct Cyc_Absyn_DoubleType_struct
df={6,0};return(void*)(b?& dt:& df);}static char _tmp58[4]="exn";static struct
_dyneither_ptr Cyc_Absyn_exn_str={_tmp58,_tmp58,_tmp58 + 4};static struct _tuple0 Cyc_Absyn_exn_name_v={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp5A[15]="Null_Exception";
static struct _dyneither_ptr Cyc_Absyn_Null_Exception_str={_tmp5A,_tmp5A,_tmp5A + 15};
static struct _tuple0 Cyc_Absyn_Null_Exception_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_Null_Exception_str};struct _tuple0*
Cyc_Absyn_Null_Exception_name=& Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Tunionfield
Cyc_Absyn_Null_Exception_tuf_v={& Cyc_Absyn_Null_Exception_pr,0,0,(void*)((void*)
3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;
static char _tmp5C[13]="Array_bounds";static struct _dyneither_ptr Cyc_Absyn_Array_bounds_str={
_tmp5C,_tmp5C,_tmp5C + 13};static struct _tuple0 Cyc_Absyn_Array_bounds_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_Array_bounds_str};
struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static
struct Cyc_Absyn_Tunionfield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Array_bounds_tuf=&
Cyc_Absyn_Array_bounds_tuf_v;static char _tmp5E[16]="Match_Exception";static struct
_dyneither_ptr Cyc_Absyn_Match_Exception_str={_tmp5E,_tmp5E,_tmp5E + 16};static
struct _tuple0 Cyc_Absyn_Match_Exception_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_Match_Exception_str};struct
_tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;static
struct Cyc_Absyn_Tunionfield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Match_Exception_tuf=&
Cyc_Absyn_Match_Exception_tuf_v;static char _tmp60[10]="Bad_alloc";static struct
_dyneither_ptr Cyc_Absyn_Bad_alloc_str={_tmp60,_tmp60,_tmp60 + 10};static struct
_tuple0 Cyc_Absyn_Bad_alloc_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Abs_n_struct){
2,0}),& Cyc_Absyn_Bad_alloc_str};struct _tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;
static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;
static struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,
0};static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l0};static struct Cyc_List_List Cyc_Absyn_exn_l2={(
void*)& Cyc_Absyn_Match_Exception_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l2};static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(
void*)((struct Cyc_List_List*)& Cyc_Absyn_exn_l3)};static struct Cyc_Absyn_Tuniondecl
Cyc_Absyn_exn_tud_v={(void*)((void*)3),& Cyc_Absyn_exn_name_v,0,(struct Cyc_Core_Opt*)&
Cyc_Absyn_exn_ol,1,0};struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
static struct Cyc_Core_Opt Cyc_Absyn_heap_opt={(void*)((void*)2)};static struct Cyc_Absyn_TunionType_struct
Cyc_Absyn_exn_typ_tt={2,{(union Cyc_Absyn_TunionInfoU_union)((struct Cyc_Absyn_KnownTunion_struct){
1,& Cyc_Absyn_exn_tud}),0,(struct Cyc_Core_Opt*)& Cyc_Absyn_heap_opt}};void*Cyc_Absyn_exn_typ=(
void*)& Cyc_Absyn_exn_typ_tt;static char _tmp64[9]="PrintArg";static struct
_dyneither_ptr Cyc_Absyn_printarg_str={_tmp64,_tmp64,_tmp64 + 9};static char _tmp65[
9]="ScanfArg";static struct _dyneither_ptr Cyc_Absyn_scanfarg_str={_tmp65,_tmp65,
_tmp65 + 9};static struct _tuple0 Cyc_Absyn_tunion_print_arg_qvar_p={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_printarg_str};static struct _tuple0
Cyc_Absyn_tunion_scanf_arg_qvar_p={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Abs_n_struct){
2,0}),& Cyc_Absyn_scanfarg_str};struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar=&
Cyc_Absyn_tunion_print_arg_qvar_p;struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar=&
Cyc_Absyn_tunion_scanf_arg_qvar_p;static void**Cyc_Absyn_string_t_opt=0;void*Cyc_Absyn_string_typ(
void*rgn);void*Cyc_Absyn_string_typ(void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_empty_tqual(0),(void*)0,Cyc_Absyn_true_conref);
if(Cyc_Absyn_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Absyn_empty_tqual(0),(void*)0,Cyc_Absyn_true_conref);void**_tmp4BA;
Cyc_Absyn_string_t_opt=((_tmp4BA=_cycalloc(sizeof(*_tmp4BA)),((_tmp4BA[0]=t,
_tmp4BA))));}return*((void**)_check_null(Cyc_Absyn_string_t_opt));}static void**
Cyc_Absyn_const_string_t_opt=0;void*Cyc_Absyn_const_string_typ(void*rgn);void*
Cyc_Absyn_const_string_typ(void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);
if(Cyc_Absyn_const_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);void**_tmp4BB;
Cyc_Absyn_const_string_t_opt=((_tmp4BB=_cycalloc(sizeof(*_tmp4BB)),((_tmp4BB[0]=
t,_tmp4BB))));}return*((void**)_check_null(Cyc_Absyn_const_string_t_opt));}void*
Cyc_Absyn_starb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*
zeroterm);void*Cyc_Absyn_starb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,
struct Cyc_Absyn_Conref*zeroterm){struct Cyc_Absyn_PointerType_struct _tmp4C5;
struct Cyc_Absyn_PtrAtts _tmp4C4;struct Cyc_Absyn_PtrInfo _tmp4C3;struct Cyc_Absyn_PointerType_struct*
_tmp4C2;return(void*)((_tmp4C2=_cycalloc(sizeof(*_tmp4C2)),((_tmp4C2[0]=((
_tmp4C5.tag=4,((_tmp4C5.f1=((_tmp4C3.elt_typ=(void*)t,((_tmp4C3.elt_tq=tq,((
_tmp4C3.ptr_atts=((_tmp4C4.rgn=(void*)r,((_tmp4C4.nullable=Cyc_Absyn_true_conref,((
_tmp4C4.bounds=Cyc_Absyn_new_conref(b),((_tmp4C4.zero_term=zeroterm,((_tmp4C4.ptrloc=
0,_tmp4C4)))))))))),_tmp4C3)))))),_tmp4C5)))),_tmp4C2))));}void*Cyc_Absyn_atb_typ(
void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zeroterm);
void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*
zeroterm){struct Cyc_Absyn_PointerType_struct _tmp4CF;struct Cyc_Absyn_PtrAtts
_tmp4CE;struct Cyc_Absyn_PtrInfo _tmp4CD;struct Cyc_Absyn_PointerType_struct*
_tmp4CC;return(void*)((_tmp4CC=_cycalloc(sizeof(*_tmp4CC)),((_tmp4CC[0]=((
_tmp4CF.tag=4,((_tmp4CF.f1=((_tmp4CD.elt_typ=(void*)t,((_tmp4CD.elt_tq=tq,((
_tmp4CD.ptr_atts=((_tmp4CE.rgn=(void*)r,((_tmp4CE.nullable=Cyc_Absyn_false_conref,((
_tmp4CE.bounds=Cyc_Absyn_new_conref(b),((_tmp4CE.zero_term=zeroterm,((_tmp4CE.ptrloc=
0,_tmp4CE)))))))))),_tmp4CD)))))),_tmp4CF)))),_tmp4CC))));}void*Cyc_Absyn_star_typ(
void*t,void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm);void*Cyc_Absyn_star_typ(
void*t,void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){struct Cyc_Absyn_PointerType_struct
_tmp4D9;struct Cyc_Absyn_PtrAtts _tmp4D8;struct Cyc_Absyn_PtrInfo _tmp4D7;struct Cyc_Absyn_PointerType_struct*
_tmp4D6;return(void*)((_tmp4D6=_cycalloc(sizeof(*_tmp4D6)),((_tmp4D6[0]=((
_tmp4D9.tag=4,((_tmp4D9.f1=((_tmp4D7.elt_typ=(void*)t,((_tmp4D7.elt_tq=tq,((
_tmp4D7.ptr_atts=((_tmp4D8.rgn=(void*)r,((_tmp4D8.nullable=Cyc_Absyn_true_conref,((
_tmp4D8.bounds=Cyc_Absyn_bounds_one_conref,((_tmp4D8.zero_term=zeroterm,((
_tmp4D8.ptrloc=0,_tmp4D8)))))))))),_tmp4D7)))))),_tmp4D9)))),_tmp4D6))));}void*
Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_cstar_typ(
void*t,struct Cyc_Absyn_Tqual tq){struct Cyc_Absyn_PointerType_struct _tmp4E3;struct
Cyc_Absyn_PtrAtts _tmp4E2;struct Cyc_Absyn_PtrInfo _tmp4E1;struct Cyc_Absyn_PointerType_struct*
_tmp4E0;return(void*)((_tmp4E0=_cycalloc(sizeof(*_tmp4E0)),((_tmp4E0[0]=((
_tmp4E3.tag=4,((_tmp4E3.f1=((_tmp4E1.elt_typ=(void*)t,((_tmp4E1.elt_tq=tq,((
_tmp4E1.ptr_atts=((_tmp4E2.rgn=(void*)((void*)2),((_tmp4E2.nullable=Cyc_Absyn_true_conref,((
_tmp4E2.bounds=Cyc_Absyn_bounds_one_conref,((_tmp4E2.zero_term=Cyc_Absyn_false_conref,((
_tmp4E2.ptrloc=0,_tmp4E2)))))))))),_tmp4E1)))))),_tmp4E3)))),_tmp4E0))));}void*
Cyc_Absyn_at_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zeroterm);void*Cyc_Absyn_at_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zeroterm){struct Cyc_Absyn_PointerType_struct _tmp4ED;struct Cyc_Absyn_PtrAtts
_tmp4EC;struct Cyc_Absyn_PtrInfo _tmp4EB;struct Cyc_Absyn_PointerType_struct*
_tmp4EA;return(void*)((_tmp4EA=_cycalloc(sizeof(*_tmp4EA)),((_tmp4EA[0]=((
_tmp4ED.tag=4,((_tmp4ED.f1=((_tmp4EB.elt_typ=(void*)t,((_tmp4EB.elt_tq=tq,((
_tmp4EB.ptr_atts=((_tmp4EC.rgn=(void*)r,((_tmp4EC.nullable=Cyc_Absyn_false_conref,((
_tmp4EC.bounds=Cyc_Absyn_bounds_one_conref,((_tmp4EC.zero_term=zeroterm,((
_tmp4EC.ptrloc=0,_tmp4EC)))))))))),_tmp4EB)))))),_tmp4ED)))),_tmp4EA))));}void*
Cyc_Absyn_dyneither_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zeroterm);void*Cyc_Absyn_dyneither_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Conref*zeroterm){struct Cyc_Absyn_PointerType_struct _tmp4F7;
struct Cyc_Absyn_PtrAtts _tmp4F6;struct Cyc_Absyn_PtrInfo _tmp4F5;struct Cyc_Absyn_PointerType_struct*
_tmp4F4;return(void*)((_tmp4F4=_cycalloc(sizeof(*_tmp4F4)),((_tmp4F4[0]=((
_tmp4F7.tag=4,((_tmp4F7.f1=((_tmp4F5.elt_typ=(void*)t,((_tmp4F5.elt_tq=tq,((
_tmp4F5.ptr_atts=((_tmp4F6.rgn=(void*)r,((_tmp4F6.nullable=Cyc_Absyn_true_conref,((
_tmp4F6.bounds=Cyc_Absyn_bounds_dyneither_conref,((_tmp4F6.zero_term=zeroterm,((
_tmp4F6.ptrloc=0,_tmp4F6)))))))))),_tmp4F5)))))),_tmp4F7)))),_tmp4F4))));}void*
Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);void*
Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc){
struct Cyc_Absyn_ArrayType_struct _tmp4FD;struct Cyc_Absyn_ArrayInfo _tmp4FC;struct
Cyc_Absyn_ArrayType_struct*_tmp4FB;return(void*)((_tmp4FB=_cycalloc(sizeof(*
_tmp4FB)),((_tmp4FB[0]=((_tmp4FD.tag=7,((_tmp4FD.f1=((_tmp4FC.elt_type=(void*)
elt_type,((_tmp4FC.tq=tq,((_tmp4FC.num_elts=num_elts,((_tmp4FC.zero_term=
zero_term,((_tmp4FC.zt_loc=ztloc,_tmp4FC)))))))))),_tmp4FD)))),_tmp4FB))));}
static char _tmp8E[8]="__sFILE";void*Cyc_Absyn_file_typ();void*Cyc_Absyn_file_typ(){
static void**file_t_opt=0;static struct _dyneither_ptr sf_str={_tmp8E,_tmp8E,_tmp8E + 
8};static struct _dyneither_ptr*sf=& sf_str;if(file_t_opt == 0){union Cyc_Absyn_Nmspace_union
_tmp500;struct _tuple0*_tmp4FF;struct _tuple0*file_t_name=(_tmp4FF=_cycalloc(
sizeof(*_tmp4FF)),((_tmp4FF->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp500.Abs_n).tag=
2,(((_tmp500.Abs_n).f1=0,_tmp500)))),((_tmp4FF->f2=sf,_tmp4FF)))));struct Cyc_Absyn_Aggrdecl*
_tmp501;struct Cyc_Absyn_Aggrdecl*sd=(_tmp501=_cycalloc(sizeof(*_tmp501)),((
_tmp501->kind=(void*)((void*)0),((_tmp501->sc=(void*)((void*)1),((_tmp501->name=
file_t_name,((_tmp501->tvs=0,((_tmp501->impl=0,((_tmp501->attributes=0,_tmp501)))))))))))));
struct Cyc_Absyn_AggrType_struct _tmp510;union Cyc_Absyn_AggrInfoU_union _tmp50F;
struct Cyc_Absyn_Aggrdecl**_tmp50E;struct Cyc_Absyn_AggrInfo _tmp50D;struct Cyc_Absyn_AggrType_struct*
_tmp50C;void*file_struct_typ=(void*)((_tmp50C=_cycalloc(sizeof(*_tmp50C)),((
_tmp50C[0]=((_tmp510.tag=10,((_tmp510.f1=((_tmp50D.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((
_tmp50F.KnownAggr).tag=1,(((_tmp50F.KnownAggr).f1=((_tmp50E=_cycalloc(sizeof(*
_tmp50E)),((_tmp50E[0]=sd,_tmp50E)))),_tmp50F)))),((_tmp50D.targs=0,_tmp50D)))),
_tmp510)))),_tmp50C))));void**_tmp511;file_t_opt=((_tmp511=_cycalloc(sizeof(*
_tmp511)),((_tmp511[0]=Cyc_Absyn_at_typ(file_struct_typ,(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref),_tmp511))));}return*file_t_opt;}void*Cyc_Absyn_void_star_typ();
void*Cyc_Absyn_void_star_typ(){static void**void_star_t_opt=0;if(void_star_t_opt
== 0){void**_tmp512;void_star_t_opt=((_tmp512=_cycalloc(sizeof(*_tmp512)),((
_tmp512[0]=Cyc_Absyn_star_typ((void*)0,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),
_tmp512))));}return*void_star_t_opt;}static struct Cyc_Absyn_JoinEff_struct Cyc_Absyn_empty_eff={
20,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;void*Cyc_Absyn_aggr_typ(
void*k,struct _dyneither_ptr*name);void*Cyc_Absyn_aggr_typ(void*k,struct
_dyneither_ptr*name){struct Cyc_Absyn_AggrType_struct _tmp521;union Cyc_Absyn_AggrInfoU_union
_tmp520;struct _tuple0*_tmp51F;struct Cyc_Absyn_AggrInfo _tmp51E;struct Cyc_Absyn_AggrType_struct*
_tmp51D;return(void*)((_tmp51D=_cycalloc(sizeof(*_tmp51D)),((_tmp51D[0]=((
_tmp521.tag=10,((_tmp521.f1=((_tmp51E.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((
_tmp520.UnknownAggr).tag=0,(((_tmp520.UnknownAggr).f1=(void*)k,(((_tmp520.UnknownAggr).f2=((
_tmp51F=_cycalloc(sizeof(*_tmp51F)),((_tmp51F->f1=Cyc_Absyn_rel_ns_null,((
_tmp51F->f2=name,_tmp51F)))))),_tmp520)))))),((_tmp51E.targs=0,_tmp51E)))),
_tmp521)))),_tmp51D))));}void*Cyc_Absyn_strct(struct _dyneither_ptr*name);void*
Cyc_Absyn_strct(struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ((void*)0,
name);}void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name);void*Cyc_Absyn_union_typ(
struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ((void*)1,name);}void*Cyc_Absyn_strctq(
struct _tuple0*name);void*Cyc_Absyn_strctq(struct _tuple0*name){struct Cyc_Absyn_AggrType_struct
_tmp52B;union Cyc_Absyn_AggrInfoU_union _tmp52A;struct Cyc_Absyn_AggrInfo _tmp529;
struct Cyc_Absyn_AggrType_struct*_tmp528;return(void*)((_tmp528=_cycalloc(sizeof(*
_tmp528)),((_tmp528[0]=((_tmp52B.tag=10,((_tmp52B.f1=((_tmp529.aggr_info=(union
Cyc_Absyn_AggrInfoU_union)(((_tmp52A.UnknownAggr).tag=0,(((_tmp52A.UnknownAggr).f1=(
void*)((void*)0),(((_tmp52A.UnknownAggr).f2=name,_tmp52A)))))),((_tmp529.targs=0,
_tmp529)))),_tmp52B)))),_tmp528))));}void*Cyc_Absyn_unionq_typ(struct _tuple0*
name);void*Cyc_Absyn_unionq_typ(struct _tuple0*name){struct Cyc_Absyn_AggrType_struct
_tmp535;union Cyc_Absyn_AggrInfoU_union _tmp534;struct Cyc_Absyn_AggrInfo _tmp533;
struct Cyc_Absyn_AggrType_struct*_tmp532;return(void*)((_tmp532=_cycalloc(sizeof(*
_tmp532)),((_tmp532[0]=((_tmp535.tag=10,((_tmp535.f1=((_tmp533.aggr_info=(union
Cyc_Absyn_AggrInfoU_union)(((_tmp534.UnknownAggr).tag=0,(((_tmp534.UnknownAggr).f1=(
void*)((void*)1),(((_tmp534.UnknownAggr).f2=name,_tmp534)))))),((_tmp533.targs=0,
_tmp533)))),_tmp535)))),_tmp532))));}struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*
r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Exp*_tmp536;return(_tmp536=
_cycalloc(sizeof(*_tmp536)),((_tmp536->topt=0,((_tmp536->r=(void*)r,((_tmp536->loc=
loc,((_tmp536->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot),_tmp536)))))))));}
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct
Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_New_e_struct _tmp539;struct Cyc_Absyn_New_e_struct*_tmp538;return
Cyc_Absyn_new_exp((void*)((_tmp538=_cycalloc(sizeof(*_tmp538)),((_tmp538[0]=((
_tmp539.tag=17,((_tmp539.f1=rgn_handle,((_tmp539.f2=e,_tmp539)))))),_tmp538)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_tmp53A;return(
_tmp53A=_cycalloc(sizeof(*_tmp53A)),((_tmp53A[0]=*e,_tmp53A)));}struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union c,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union c,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Const_e_struct _tmp53D;struct Cyc_Absyn_Const_e_struct*
_tmp53C;return Cyc_Absyn_new_exp((void*)((_tmp53C=_cycalloc(sizeof(*_tmp53C)),((
_tmp53C[0]=((_tmp53D.tag=0,((_tmp53D.f1=c,_tmp53D)))),_tmp53C)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Const_e_struct
_tmp543;union Cyc_Absyn_Cnst_union _tmp542;struct Cyc_Absyn_Const_e_struct*_tmp541;
return Cyc_Absyn_new_exp((void*)((_tmp541=_cycalloc(sizeof(*_tmp541)),((_tmp541[0]=((
_tmp543.tag=0,((_tmp543.f1=(union Cyc_Absyn_Cnst_union)(((_tmp542.Null_c).tag=6,
_tmp542)),_tmp543)))),_tmp541)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
void*s,int i,struct Cyc_Position_Segment*seg);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
void*s,int i,struct Cyc_Position_Segment*seg){union Cyc_Absyn_Cnst_union _tmp544;
return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)(((_tmp544.Int_c).tag=2,(((
_tmp544.Int_c).f1=(void*)s,(((_tmp544.Int_c).f2=i,_tmp544)))))),seg);}struct Cyc_Absyn_Exp*
Cyc_Absyn_signed_int_exp(int i,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*
Cyc_Absyn_signed_int_exp(int i,struct Cyc_Position_Segment*loc){return Cyc_Absyn_int_exp((
void*)0,i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_int_exp((void*)1,(int)i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(
int b,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*
Cyc_Absyn_true_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(1,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*loc){return
Cyc_Absyn_bool_exp(0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*
loc){union Cyc_Absyn_Cnst_union _tmp545;return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)(((
_tmp545.Char_c).tag=0,(((_tmp545.Char_c).f1=(void*)((void*)2),(((_tmp545.Char_c).f2=
c,_tmp545)))))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct
_dyneither_ptr f,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dyneither_ptr f,struct Cyc_Position_Segment*loc){union Cyc_Absyn_Cnst_union
_tmp546;return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)(((_tmp546.Float_c).tag=
4,(((_tmp546.Float_c).f1=f,_tmp546)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*loc){union Cyc_Absyn_Cnst_union
_tmp547;return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)(((_tmp547.String_c).tag=
5,(((_tmp547.String_c).f1=s,_tmp547)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple0*q,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple0*q,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Var_e_struct
_tmp54A;struct Cyc_Absyn_Var_e_struct*_tmp549;return Cyc_Absyn_new_exp((void*)((
_tmp549=_cycalloc(sizeof(*_tmp549)),((_tmp549[0]=((_tmp54A.tag=1,((_tmp54A.f1=q,((
_tmp54A.f2=(void*)((void*)0),_tmp54A)))))),_tmp549)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Var_e_struct _tmp54D;struct Cyc_Absyn_Var_e_struct*_tmp54C;
return Cyc_Absyn_new_exp((void*)((_tmp54C=_cycalloc(sizeof(*_tmp54C)),((_tmp54C[0]=((
_tmp54D.tag=1,((_tmp54D.f1=q,((_tmp54D.f2=(void*)b,_tmp54D)))))),_tmp54C)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_UnknownId_e_struct _tmp550;struct Cyc_Absyn_UnknownId_e_struct*
_tmp54F;return Cyc_Absyn_new_exp((void*)((_tmp54F=_cycalloc(sizeof(*_tmp54F)),((
_tmp54F[0]=((_tmp550.tag=2,((_tmp550.f1=q,_tmp550)))),_tmp54F)))),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*p,struct Cyc_List_List*es,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*p,struct Cyc_List_List*es,
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Primop_e_struct _tmp553;struct Cyc_Absyn_Primop_e_struct*
_tmp552;return Cyc_Absyn_new_exp((void*)((_tmp552=_cycalloc(sizeof(*_tmp552)),((
_tmp552[0]=((_tmp553.tag=3,((_tmp553.f1=(void*)p,((_tmp553.f2=es,_tmp553)))))),
_tmp552)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*p,
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_List_List*
_tmp554;return Cyc_Absyn_primop_exp(p,((_tmp554=_cycalloc(sizeof(*_tmp554)),((
_tmp554->hd=e,((_tmp554->tl=0,_tmp554)))))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(void*p,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){struct Cyc_List_List*
_tmp557;struct Cyc_List_List*_tmp556;return Cyc_Absyn_primop_exp(p,((_tmp556=
_cycalloc(sizeof(*_tmp556)),((_tmp556->hd=e1,((_tmp556->tl=((_tmp557=_cycalloc(
sizeof(*_tmp557)),((_tmp557->hd=e2,((_tmp557->tl=0,_tmp557)))))),_tmp556)))))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_prim2_exp((void*)0,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
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
_tmp55A;struct Cyc_Absyn_AssignOp_e_struct*_tmp559;return Cyc_Absyn_new_exp((void*)((
_tmp559=_cycalloc(sizeof(*_tmp559)),((_tmp559[0]=((_tmp55A.tag=4,((_tmp55A.f1=e1,((
_tmp55A.f2=popt,((_tmp55A.f3=e2,_tmp55A)))))))),_tmp559)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,void*i,struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*
e,void*i,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Increment_e_struct
_tmp55D;struct Cyc_Absyn_Increment_e_struct*_tmp55C;return Cyc_Absyn_new_exp((void*)((
_tmp55C=_cycalloc(sizeof(*_tmp55C)),((_tmp55C[0]=((_tmp55D.tag=5,((_tmp55D.f1=e,((
_tmp55D.f2=(void*)i,_tmp55D)))))),_tmp55C)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
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
_tmp560;struct Cyc_Absyn_Conditional_e_struct*_tmp55F;return Cyc_Absyn_new_exp((
void*)((_tmp55F=_cycalloc(sizeof(*_tmp55F)),((_tmp55F[0]=((_tmp560.tag=6,((
_tmp560.f1=e1,((_tmp560.f2=e2,((_tmp560.f3=e3,_tmp560)))))))),_tmp55F)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){struct
Cyc_Absyn_And_e_struct _tmp563;struct Cyc_Absyn_And_e_struct*_tmp562;return Cyc_Absyn_new_exp((
void*)((_tmp562=_cycalloc(sizeof(*_tmp562)),((_tmp562[0]=((_tmp563.tag=7,((
_tmp563.f1=e1,((_tmp563.f2=e2,_tmp563)))))),_tmp562)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Or_e_struct _tmp566;struct Cyc_Absyn_Or_e_struct*
_tmp565;return Cyc_Absyn_new_exp((void*)((_tmp565=_cycalloc(sizeof(*_tmp565)),((
_tmp565[0]=((_tmp566.tag=8,((_tmp566.f1=e1,((_tmp566.f2=e2,_tmp566)))))),_tmp565)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){struct
Cyc_Absyn_SeqExp_e_struct _tmp569;struct Cyc_Absyn_SeqExp_e_struct*_tmp568;return
Cyc_Absyn_new_exp((void*)((_tmp568=_cycalloc(sizeof(*_tmp568)),((_tmp568[0]=((
_tmp569.tag=9,((_tmp569.f1=e1,((_tmp569.f2=e2,_tmp569)))))),_tmp568)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_UnknownCall_e_struct _tmp56C;struct Cyc_Absyn_UnknownCall_e_struct*
_tmp56B;return Cyc_Absyn_new_exp((void*)((_tmp56B=_cycalloc(sizeof(*_tmp56B)),((
_tmp56B[0]=((_tmp56C.tag=10,((_tmp56C.f1=e,((_tmp56C.f2=es,_tmp56C)))))),_tmp56B)))),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_FnCall_e_struct _tmp56F;struct Cyc_Absyn_FnCall_e_struct*_tmp56E;
return Cyc_Absyn_new_exp((void*)((_tmp56E=_cycalloc(sizeof(*_tmp56E)),((_tmp56E[0]=((
_tmp56F.tag=11,((_tmp56F.f1=e,((_tmp56F.f2=es,((_tmp56F.f3=0,_tmp56F)))))))),
_tmp56E)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_NoInstantiate_e_struct
_tmp572;struct Cyc_Absyn_NoInstantiate_e_struct*_tmp571;return Cyc_Absyn_new_exp((
void*)((_tmp571=_cycalloc(sizeof(*_tmp571)),((_tmp571[0]=((_tmp572.tag=13,((
_tmp572.f1=e,_tmp572)))),_tmp571)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
ts,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Instantiate_e_struct _tmp575;
struct Cyc_Absyn_Instantiate_e_struct*_tmp574;return Cyc_Absyn_new_exp((void*)((
_tmp574=_cycalloc(sizeof(*_tmp574)),((_tmp574[0]=((_tmp575.tag=14,((_tmp575.f1=e,((
_tmp575.f2=ts,_tmp575)))))),_tmp574)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*t,struct Cyc_Absyn_Exp*e,int user_cast,void*c,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,
void*c,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Cast_e_struct _tmp578;
struct Cyc_Absyn_Cast_e_struct*_tmp577;return Cyc_Absyn_new_exp((void*)((_tmp577=
_cycalloc(sizeof(*_tmp577)),((_tmp577[0]=((_tmp578.tag=15,((_tmp578.f1=(void*)t,((
_tmp578.f2=e,((_tmp578.f3=user_cast,((_tmp578.f4=(void*)c,_tmp578)))))))))),
_tmp577)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Throw_e_struct
_tmp57B;struct Cyc_Absyn_Throw_e_struct*_tmp57A;return Cyc_Absyn_new_exp((void*)((
_tmp57A=_cycalloc(sizeof(*_tmp57A)),((_tmp57A[0]=((_tmp57B.tag=12,((_tmp57B.f1=e,
_tmp57B)))),_tmp57A)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Address_e_struct
_tmp57E;struct Cyc_Absyn_Address_e_struct*_tmp57D;return Cyc_Absyn_new_exp((void*)((
_tmp57D=_cycalloc(sizeof(*_tmp57D)),((_tmp57D[0]=((_tmp57E.tag=16,((_tmp57E.f1=e,
_tmp57E)))),_tmp57D)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*
t,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Sizeoftyp_e_struct _tmp581;
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp580;return Cyc_Absyn_new_exp((void*)((
_tmp580=_cycalloc(sizeof(*_tmp580)),((_tmp580[0]=((_tmp581.tag=18,((_tmp581.f1=(
void*)t,_tmp581)))),_tmp580)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Sizeofexp_e_struct
_tmp584;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp583;return Cyc_Absyn_new_exp((void*)((
_tmp583=_cycalloc(sizeof(*_tmp583)),((_tmp583[0]=((_tmp584.tag=19,((_tmp584.f1=e,
_tmp584)))),_tmp583)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,
void*of,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(
void*t,void*of,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Offsetof_e_struct
_tmp587;struct Cyc_Absyn_Offsetof_e_struct*_tmp586;return Cyc_Absyn_new_exp((void*)((
_tmp586=_cycalloc(sizeof(*_tmp586)),((_tmp586[0]=((_tmp587.tag=20,((_tmp587.f1=(
void*)t,((_tmp587.f2=(void*)of,_tmp587)))))),_tmp586)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_gentyp_exp(struct Cyc_List_List*tvs,void*t,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*tvs,void*t,
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Gentyp_e_struct _tmp58A;struct Cyc_Absyn_Gentyp_e_struct*
_tmp589;return Cyc_Absyn_new_exp((void*)((_tmp589=_cycalloc(sizeof(*_tmp589)),((
_tmp589[0]=((_tmp58A.tag=21,((_tmp58A.f1=tvs,((_tmp58A.f2=(void*)t,_tmp58A)))))),
_tmp589)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Deref_e_struct
_tmp58D;struct Cyc_Absyn_Deref_e_struct*_tmp58C;return Cyc_Absyn_new_exp((void*)((
_tmp58C=_cycalloc(sizeof(*_tmp58C)),((_tmp58C[0]=((_tmp58D.tag=22,((_tmp58D.f1=e,
_tmp58D)))),_tmp58C)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*n,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_AggrMember_e_struct
_tmp590;struct Cyc_Absyn_AggrMember_e_struct*_tmp58F;return Cyc_Absyn_new_exp((
void*)((_tmp58F=_cycalloc(sizeof(*_tmp58F)),((_tmp58F[0]=((_tmp590.tag=23,((
_tmp590.f1=e,((_tmp590.f2=n,_tmp590)))))),_tmp58F)))),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*n,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_AggrArrow_e_struct
_tmp593;struct Cyc_Absyn_AggrArrow_e_struct*_tmp592;return Cyc_Absyn_new_exp((void*)((
_tmp592=_cycalloc(sizeof(*_tmp592)),((_tmp592[0]=((_tmp593.tag=24,((_tmp593.f1=e,((
_tmp593.f2=n,_tmp593)))))),_tmp592)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Subscript_e_struct _tmp596;
struct Cyc_Absyn_Subscript_e_struct*_tmp595;return Cyc_Absyn_new_exp((void*)((
_tmp595=_cycalloc(sizeof(*_tmp595)),((_tmp595[0]=((_tmp596.tag=25,((_tmp596.f1=
e1,((_tmp596.f2=e2,_tmp596)))))),_tmp595)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Tuple_e_struct
_tmp599;struct Cyc_Absyn_Tuple_e_struct*_tmp598;return Cyc_Absyn_new_exp((void*)((
_tmp598=_cycalloc(sizeof(*_tmp598)),((_tmp598[0]=((_tmp599.tag=26,((_tmp599.f1=
es,_tmp599)))),_tmp598)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct
Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_StmtExp_e_struct
_tmp59C;struct Cyc_Absyn_StmtExp_e_struct*_tmp59B;return Cyc_Absyn_new_exp((void*)((
_tmp59B=_cycalloc(sizeof(*_tmp59B)),((_tmp59B[0]=((_tmp59C.tag=38,((_tmp59C.f1=s,
_tmp59C)))),_tmp59B)))),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*
loc){return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_valueof_exp(void*t,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*
Cyc_Absyn_valueof_exp(void*t,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Valueof_e_struct
_tmp59F;struct Cyc_Absyn_Valueof_e_struct*_tmp59E;return Cyc_Absyn_new_exp((void*)((
_tmp59E=_cycalloc(sizeof(*_tmp59E)),((_tmp59E[0]=((_tmp59F.tag=39,((_tmp59F.f1=(
void*)t,_tmp59F)))),_tmp59E)))),loc);}struct _tuple13{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_List_List*dles=
0;for(0;es != 0;es=es->tl){struct _tuple13*_tmp5A2;struct Cyc_List_List*_tmp5A1;
dles=((_tmp5A1=_cycalloc(sizeof(*_tmp5A1)),((_tmp5A1->hd=((_tmp5A2=_cycalloc(
sizeof(*_tmp5A2)),((_tmp5A2->f1=0,((_tmp5A2->f2=(struct Cyc_Absyn_Exp*)es->hd,
_tmp5A2)))))),((_tmp5A1->tl=dles,_tmp5A1))))));}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);{struct Cyc_Absyn_Array_e_struct
_tmp5A5;struct Cyc_Absyn_Array_e_struct*_tmp5A4;return Cyc_Absyn_new_exp((void*)((
_tmp5A4=_cycalloc(sizeof(*_tmp5A4)),((_tmp5A4[0]=((_tmp5A5.tag=28,((_tmp5A5.f1=
dles,_tmp5A5)))),_tmp5A4)))),loc);}}struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*
dles,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp5A8;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp5A7;return Cyc_Absyn_new_exp((
void*)((_tmp5A7=_cycalloc(sizeof(*_tmp5A7)),((_tmp5A7[0]=((_tmp5A8.tag=37,((
_tmp5A8.f1=n,((_tmp5A8.f2=dles,_tmp5A8)))))),_tmp5A7)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Stmt*
_tmp5A9;return(_tmp5A9=_cycalloc(sizeof(*_tmp5A9)),((_tmp5A9->r=(void*)s,((
_tmp5A9->loc=loc,((_tmp5A9->non_local_preds=0,((_tmp5A9->try_depth=0,((_tmp5A9->annot=(
void*)((void*)Cyc_Absyn_EmptyAnnot),_tmp5A9)))))))))));}struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)0,loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Exp_s_struct _tmp5AC;struct Cyc_Absyn_Exp_s_struct*_tmp5AB;
return Cyc_Absyn_new_stmt((void*)((_tmp5AB=_cycalloc(sizeof(*_tmp5AB)),((_tmp5AB[
0]=((_tmp5AC.tag=0,((_tmp5AC.f1=e,_tmp5AC)))),_tmp5AB)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,struct Cyc_Position_Segment*
loc){if(ss == 0)return Cyc_Absyn_skip_stmt(loc);else{if(ss->tl == 0)return(struct
Cyc_Absyn_Stmt*)ss->hd;else{return Cyc_Absyn_seq_stmt((struct Cyc_Absyn_Stmt*)ss->hd,
Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Return_s_struct
_tmp5AF;struct Cyc_Absyn_Return_s_struct*_tmp5AE;return Cyc_Absyn_new_stmt((void*)((
_tmp5AE=_cycalloc(sizeof(*_tmp5AE)),((_tmp5AE[0]=((_tmp5AF.tag=2,((_tmp5AF.f1=e,
_tmp5AF)))),_tmp5AE)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc){struct
Cyc_Absyn_IfThenElse_s_struct _tmp5B2;struct Cyc_Absyn_IfThenElse_s_struct*_tmp5B1;
return Cyc_Absyn_new_stmt((void*)((_tmp5B1=_cycalloc(sizeof(*_tmp5B1)),((_tmp5B1[
0]=((_tmp5B2.tag=3,((_tmp5B2.f1=e,((_tmp5B2.f2=s1,((_tmp5B2.f3=s2,_tmp5B2)))))))),
_tmp5B1)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_While_s_struct _tmp5B8;struct _tuple2 _tmp5B7;struct Cyc_Absyn_While_s_struct*
_tmp5B6;return Cyc_Absyn_new_stmt((void*)((_tmp5B6=_cycalloc(sizeof(*_tmp5B6)),((
_tmp5B6[0]=((_tmp5B8.tag=4,((_tmp5B8.f1=((_tmp5B7.f1=e,((_tmp5B7.f2=Cyc_Absyn_skip_stmt(
e->loc),_tmp5B7)))),((_tmp5B8.f2=s,_tmp5B8)))))),_tmp5B6)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_break_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Break_s_struct _tmp5BB;struct Cyc_Absyn_Break_s_struct*
_tmp5BA;return Cyc_Absyn_new_stmt((void*)((_tmp5BA=_cycalloc(sizeof(*_tmp5BA)),((
_tmp5BA[0]=((_tmp5BB.tag=5,((_tmp5BB.f1=0,_tmp5BB)))),_tmp5BA)))),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Continue_s_struct
_tmp5BE;struct Cyc_Absyn_Continue_s_struct*_tmp5BD;return Cyc_Absyn_new_stmt((void*)((
_tmp5BD=_cycalloc(sizeof(*_tmp5BD)),((_tmp5BD[0]=((_tmp5BE.tag=6,((_tmp5BE.f1=0,
_tmp5BE)))),_tmp5BD)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct
Cyc_Position_Segment*loc){struct Cyc_Absyn_For_s_struct _tmp5C7;struct _tuple2
_tmp5C6;struct _tuple2 _tmp5C5;struct Cyc_Absyn_For_s_struct*_tmp5C4;return Cyc_Absyn_new_stmt((
void*)((_tmp5C4=_cycalloc(sizeof(*_tmp5C4)),((_tmp5C4[0]=((_tmp5C7.tag=8,((
_tmp5C7.f1=e1,((_tmp5C7.f2=((_tmp5C5.f1=e2,((_tmp5C5.f2=Cyc_Absyn_skip_stmt(e3->loc),
_tmp5C5)))),((_tmp5C7.f3=((_tmp5C6.f1=e3,((_tmp5C6.f2=Cyc_Absyn_skip_stmt(e3->loc),
_tmp5C6)))),((_tmp5C7.f4=s,_tmp5C7)))))))))),_tmp5C4)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Switch_s_struct _tmp5CA;
struct Cyc_Absyn_Switch_s_struct*_tmp5C9;return Cyc_Absyn_new_stmt((void*)((
_tmp5C9=_cycalloc(sizeof(*_tmp5C9)),((_tmp5C9[0]=((_tmp5CA.tag=9,((_tmp5CA.f1=e,((
_tmp5CA.f2=scs,_tmp5CA)))))),_tmp5C9)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc){struct _tuple9 _tmp5CB;struct _tuple9 _tmp101=(
_tmp5CB.f1=(void*)s1->r,((_tmp5CB.f2=(void*)s2->r,_tmp5CB)));void*_tmp102;void*
_tmp103;_LL64: _tmp102=_tmp101.f1;if((int)_tmp102 != 0)goto _LL66;_LL65: return s2;
_LL66: _tmp103=_tmp101.f2;if((int)_tmp103 != 0)goto _LL68;_LL67: return s1;_LL68:;
_LL69: {struct Cyc_Absyn_Seq_s_struct _tmp5CE;struct Cyc_Absyn_Seq_s_struct*_tmp5CD;
return Cyc_Absyn_new_stmt((void*)((_tmp5CD=_cycalloc(sizeof(*_tmp5CD)),((_tmp5CD[
0]=((_tmp5CE.tag=1,((_tmp5CE.f1=s1,((_tmp5CE.f2=s2,_tmp5CE)))))),_tmp5CD)))),loc);}
_LL63:;}struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Fallthru_s_struct
_tmp5D1;struct Cyc_Absyn_Fallthru_s_struct*_tmp5D0;return Cyc_Absyn_new_stmt((void*)((
_tmp5D0=_cycalloc(sizeof(*_tmp5D0)),((_tmp5D0[0]=((_tmp5D1.tag=10,((_tmp5D1.f1=
el,((_tmp5D1.f2=0,_tmp5D1)))))),_tmp5D0)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl_s_struct _tmp5D4;struct Cyc_Absyn_Decl_s_struct*
_tmp5D3;return Cyc_Absyn_new_stmt((void*)((_tmp5D3=_cycalloc(sizeof(*_tmp5D3)),((
_tmp5D3[0]=((_tmp5D4.tag=11,((_tmp5D4.f1=d,((_tmp5D4.f2=s,_tmp5D4)))))),_tmp5D3)))),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct
Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Var_d_struct
_tmp5D7;struct Cyc_Absyn_Var_d_struct*_tmp5D6;struct Cyc_Absyn_Decl*d=Cyc_Absyn_new_decl((
void*)((_tmp5D6=_cycalloc(sizeof(*_tmp5D6)),((_tmp5D6[0]=((_tmp5D7.tag=0,((
_tmp5D7.f1=Cyc_Absyn_new_vardecl(x,t,init),_tmp5D7)))),_tmp5D6)))),loc);struct
Cyc_Absyn_Decl_s_struct _tmp5DA;struct Cyc_Absyn_Decl_s_struct*_tmp5D9;return Cyc_Absyn_new_stmt((
void*)((_tmp5D9=_cycalloc(sizeof(*_tmp5D9)),((_tmp5D9[0]=((_tmp5DA.tag=11,((
_tmp5DA.f1=d,((_tmp5DA.f2=s,_tmp5DA)))))),_tmp5D9)))),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Label_s_struct _tmp5DD;struct
Cyc_Absyn_Label_s_struct*_tmp5DC;return Cyc_Absyn_new_stmt((void*)((_tmp5DC=
_cycalloc(sizeof(*_tmp5DC)),((_tmp5DC[0]=((_tmp5DD.tag=12,((_tmp5DD.f1=v,((
_tmp5DD.f2=s,_tmp5DD)))))),_tmp5DC)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Do_s_struct _tmp5E3;struct
_tuple2 _tmp5E2;struct Cyc_Absyn_Do_s_struct*_tmp5E1;return Cyc_Absyn_new_stmt((
void*)((_tmp5E1=_cycalloc(sizeof(*_tmp5E1)),((_tmp5E1[0]=((_tmp5E3.tag=13,((
_tmp5E3.f1=s,((_tmp5E3.f2=((_tmp5E2.f1=e,((_tmp5E2.f2=Cyc_Absyn_skip_stmt(e->loc),
_tmp5E2)))),_tmp5E3)))))),_tmp5E1)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_TryCatch_s_struct _tmp5E6;
struct Cyc_Absyn_TryCatch_s_struct*_tmp5E5;return Cyc_Absyn_new_stmt((void*)((
_tmp5E5=_cycalloc(sizeof(*_tmp5E5)),((_tmp5E5[0]=((_tmp5E6.tag=14,((_tmp5E6.f1=s,((
_tmp5E6.f2=scs,_tmp5E6)))))),_tmp5E5)))),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _dyneither_ptr*lab,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Goto_s_struct _tmp5E9;struct Cyc_Absyn_Goto_s_struct*_tmp5E8;
return Cyc_Absyn_new_stmt((void*)((_tmp5E8=_cycalloc(sizeof(*_tmp5E8)),((_tmp5E8[
0]=((_tmp5E9.tag=7,((_tmp5E9.f1=lab,((_tmp5E9.f2=0,_tmp5E9)))))),_tmp5E8)))),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s){struct Cyc_Absyn_Pat*_tmp5EA;return(_tmp5EA=
_cycalloc(sizeof(*_tmp5EA)),((_tmp5EA->r=(void*)p,((_tmp5EA->topt=0,((_tmp5EA->loc=
s,_tmp5EA)))))));}struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_p_struct
_tmp5ED;struct Cyc_Absyn_Exp_p_struct*_tmp5EC;return Cyc_Absyn_new_pat((void*)((
_tmp5EC=_cycalloc(sizeof(*_tmp5EC)),((_tmp5EC[0]=((_tmp5ED.tag=14,((_tmp5ED.f1=e,
_tmp5ED)))),_tmp5EC)))),e->loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,
struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl*_tmp5EE;return(_tmp5EE=
_cycalloc(sizeof(*_tmp5EE)),((_tmp5EE->r=(void*)r,((_tmp5EE->loc=loc,_tmp5EE)))));}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct
Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Let_d_struct
_tmp5F1;struct Cyc_Absyn_Let_d_struct*_tmp5F0;return Cyc_Absyn_new_decl((void*)((
_tmp5F0=_cycalloc(sizeof(*_tmp5F0)),((_tmp5F0[0]=((_tmp5F1.tag=2,((_tmp5F1.f1=p,((
_tmp5F1.f2=0,((_tmp5F1.f3=e,_tmp5F1)))))))),_tmp5F0)))),loc);}struct Cyc_Absyn_Decl*
Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Letv_d_struct _tmp5F4;struct Cyc_Absyn_Letv_d_struct*_tmp5F3;
return Cyc_Absyn_new_decl((void*)((_tmp5F3=_cycalloc(sizeof(*_tmp5F3)),((_tmp5F3[
0]=((_tmp5F4.tag=3,((_tmp5F4.f1=vds,_tmp5F4)))),_tmp5F3)))),loc);}struct Cyc_Absyn_Decl*
Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,int
resetable,struct Cyc_Absyn_Exp*eo,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,int
resetable,struct Cyc_Absyn_Exp*eo,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Region_d_struct
_tmp5F7;struct Cyc_Absyn_Region_d_struct*_tmp5F6;return Cyc_Absyn_new_decl((void*)((
_tmp5F6=_cycalloc(sizeof(*_tmp5F6)),((_tmp5F6[0]=((_tmp5F7.tag=4,((_tmp5F7.f1=tv,((
_tmp5F7.f2=vd,((_tmp5F7.f3=resetable,((_tmp5F7.f4=eo,_tmp5F7)))))))))),_tmp5F6)))),
loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Tvar*
tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Alias_d_struct _tmp5FA;struct
Cyc_Absyn_Alias_d_struct*_tmp5F9;return Cyc_Absyn_new_decl((void*)((_tmp5F9=
_cycalloc(sizeof(*_tmp5F9)),((_tmp5F9[0]=((_tmp5FA.tag=5,((_tmp5FA.f1=e,((
_tmp5FA.f2=tv,((_tmp5FA.f3=vd,_tmp5FA)))))))),_tmp5F9)))),loc);}struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){struct Cyc_Absyn_Vardecl*
_tmp5FB;return(_tmp5FB=_cycalloc(sizeof(*_tmp5FB)),((_tmp5FB->sc=(void*)((void*)
2),((_tmp5FB->name=x,((_tmp5FB->tq=Cyc_Absyn_empty_tqual(0),((_tmp5FB->type=(
void*)t,((_tmp5FB->initializer=init,((_tmp5FB->rgn=0,((_tmp5FB->attributes=0,((
_tmp5FB->escapes=0,_tmp5FB)))))))))))))))));}struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){struct Cyc_Absyn_Vardecl*_tmp5FC;
return(_tmp5FC=_cycalloc(sizeof(*_tmp5FC)),((_tmp5FC->sc=(void*)((void*)0),((
_tmp5FC->name=x,((_tmp5FC->tq=Cyc_Absyn_empty_tqual(0),((_tmp5FC->type=(void*)t,((
_tmp5FC->initializer=init,((_tmp5FC->rgn=0,((_tmp5FC->attributes=0,((_tmp5FC->escapes=
0,_tmp5FC)))))))))))))))));}struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(
struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs);struct
Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct
Cyc_List_List*po,struct Cyc_List_List*fs){struct Cyc_Absyn_AggrdeclImpl*_tmp5FD;
return(_tmp5FD=_cycalloc(sizeof(*_tmp5FD)),((_tmp5FD->exist_vars=exists,((
_tmp5FD->rgn_po=po,((_tmp5FD->fields=fs,_tmp5FD)))))));}struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(
void*k,void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct
Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Aggr_d_struct _tmp603;struct Cyc_Absyn_Aggrdecl*_tmp602;struct Cyc_Absyn_Aggr_d_struct*
_tmp601;return Cyc_Absyn_new_decl((void*)((_tmp601=_cycalloc(sizeof(*_tmp601)),((
_tmp601[0]=((_tmp603.tag=6,((_tmp603.f1=((_tmp602=_cycalloc(sizeof(*_tmp602)),((
_tmp602->kind=(void*)k,((_tmp602->sc=(void*)s,((_tmp602->name=n,((_tmp602->tvs=
ts,((_tmp602->impl=i,((_tmp602->attributes=atts,_tmp602)))))))))))))),_tmp603)))),
_tmp601)))),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(void*s,struct
_tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((
void*)0,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(void*s,
struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((
void*)1,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,
struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_xtunion,int
is_flat,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int
is_xtunion,int is_flat,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Tunion_d_struct
_tmp609;struct Cyc_Absyn_Tuniondecl*_tmp608;struct Cyc_Absyn_Tunion_d_struct*
_tmp607;return Cyc_Absyn_new_decl((void*)((_tmp607=_cycalloc(sizeof(*_tmp607)),((
_tmp607[0]=((_tmp609.tag=7,((_tmp609.f1=((_tmp608=_cycalloc(sizeof(*_tmp608)),((
_tmp608->sc=(void*)s,((_tmp608->name=n,((_tmp608->tvs=ts,((_tmp608->fields=fs,((
_tmp608->is_xtunion=is_xtunion,((_tmp608->is_flat=is_flat,_tmp608)))))))))))))),
_tmp609)))),_tmp607)))),loc);}static struct _tuple1*Cyc_Absyn_expand_arg(struct
_tuple1*a);static struct _tuple1*Cyc_Absyn_expand_arg(struct _tuple1*a){struct
_tuple1*_tmp60A;return(_tmp60A=_cycalloc(sizeof(*_tmp60A)),((_tmp60A->f1=(*a).f1,((
_tmp60A->f2=(*a).f2,((_tmp60A->f3=Cyc_Absyn_pointer_expand((*a).f3,1),_tmp60A)))))));}
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,
void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts){struct Cyc_Absyn_FnType_struct _tmp610;struct Cyc_Absyn_FnInfo
_tmp60F;struct Cyc_Absyn_FnType_struct*_tmp60E;return(void*)((_tmp60E=_cycalloc(
sizeof(*_tmp60E)),((_tmp60E[0]=((_tmp610.tag=8,((_tmp610.f1=((_tmp60F.tvars=tvs,((
_tmp60F.ret_typ=(void*)Cyc_Absyn_pointer_expand(ret_typ,0),((_tmp60F.effect=
eff_typ,((_tmp60F.args=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct
_tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absyn_expand_arg,args),((
_tmp60F.c_varargs=c_varargs,((_tmp60F.cyc_varargs=cyc_varargs,((_tmp60F.rgn_po=
rgn_po,((_tmp60F.attributes=atts,_tmp60F)))))))))))))))),_tmp610)))),_tmp60E))));}
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar);void*Cyc_Absyn_pointer_expand(
void*t,int fresh_evar){void*_tmp130=Cyc_Tcutil_compress(t);_LL6B: if(_tmp130 <= (
void*)4)goto _LL6D;if(*((int*)_tmp130)!= 8)goto _LL6D;_LL6C: {struct Cyc_Core_Opt*
_tmp611;return Cyc_Absyn_at_typ(t,fresh_evar?Cyc_Absyn_new_evar(((_tmp611=
_cycalloc(sizeof(*_tmp611)),((_tmp611->v=(void*)((void*)3),_tmp611)))),0):(void*)
2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);}_LL6D:;_LL6E: return t;_LL6A:;}
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*
e){void*_tmp132=(void*)e->r;void*_tmp133;void*_tmp134;struct Cyc_Absyn_Vardecl*
_tmp135;void*_tmp136;struct Cyc_Absyn_Vardecl*_tmp137;struct Cyc_Absyn_Exp*_tmp138;
struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp13A;_LL70: if(*((int*)_tmp132)
!= 1)goto _LL72;_tmp133=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp132)->f2;if(
_tmp133 <= (void*)1)goto _LL72;if(*((int*)_tmp133)!= 1)goto _LL72;_LL71: return 0;
_LL72: if(*((int*)_tmp132)!= 1)goto _LL74;_tmp134=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp132)->f2;if(_tmp134 <= (void*)1)goto _LL74;if(*((int*)_tmp134)!= 0)goto _LL74;
_tmp135=((struct Cyc_Absyn_Global_b_struct*)_tmp134)->f1;_LL73: _tmp137=_tmp135;
goto _LL75;_LL74: if(*((int*)_tmp132)!= 1)goto _LL76;_tmp136=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp132)->f2;if(_tmp136 <= (void*)1)goto _LL76;if(*((int*)_tmp136)!= 3)goto _LL76;
_tmp137=((struct Cyc_Absyn_Local_b_struct*)_tmp136)->f1;_LL75: {void*_tmp13B=Cyc_Tcutil_compress((
void*)_tmp137->type);_LL87: if(_tmp13B <= (void*)4)goto _LL89;if(*((int*)_tmp13B)!= 
7)goto _LL89;_LL88: return 0;_LL89:;_LL8A: return 1;_LL86:;}_LL76: if(*((int*)_tmp132)
!= 1)goto _LL78;_LL77: goto _LL79;_LL78: if(*((int*)_tmp132)!= 24)goto _LL7A;_LL79:
goto _LL7B;_LL7A: if(*((int*)_tmp132)!= 22)goto _LL7C;_LL7B: goto _LL7D;_LL7C: if(*((
int*)_tmp132)!= 25)goto _LL7E;_LL7D: return 1;_LL7E: if(*((int*)_tmp132)!= 23)goto
_LL80;_tmp138=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp132)->f1;_LL7F: return
Cyc_Absyn_is_lvalue(_tmp138);_LL80: if(*((int*)_tmp132)!= 14)goto _LL82;_tmp139=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmp132)->f1;_LL81: return Cyc_Absyn_is_lvalue(
_tmp139);_LL82: if(*((int*)_tmp132)!= 13)goto _LL84;_tmp13A=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp132)->f1;_LL83: return Cyc_Absyn_is_lvalue(_tmp13A);_LL84:;_LL85: return 0;_LL6F:;}
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,
struct _dyneither_ptr*v);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct
Cyc_List_List*fields,struct _dyneither_ptr*v){{struct Cyc_List_List*_tmp13C=fields;
for(0;_tmp13C != 0;_tmp13C=_tmp13C->tl){if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)
_tmp13C->hd)->name,v)== 0)return(struct Cyc_Absyn_Aggrfield*)((struct Cyc_Absyn_Aggrfield*)
_tmp13C->hd);}}return 0;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*v);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*v){
return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields,v);}struct _tuple3*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*ts,int i);struct _tuple3*Cyc_Absyn_lookup_tuple_field(struct
Cyc_List_List*ts,int i){for(0;i != 0;-- i){if(ts == 0)return 0;ts=ts->tl;}if(ts == 0)
return 0;return(struct _tuple3*)((struct _tuple3*)ts->hd);}struct _dyneither_ptr Cyc_Absyn_attribute2string(
void*a);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*a){void*_tmp13D=a;
int _tmp13E;int _tmp13F;struct _dyneither_ptr _tmp140;void*_tmp141;int _tmp142;int
_tmp143;void*_tmp144;int _tmp145;int _tmp146;int _tmp147;struct _dyneither_ptr
_tmp148;_LL8C: if(_tmp13D <= (void*)17)goto _LL8E;if(*((int*)_tmp13D)!= 0)goto _LL8E;
_tmp13E=((struct Cyc_Absyn_Regparm_att_struct*)_tmp13D)->f1;_LL8D: {const char*
_tmp615;void*_tmp614[1];struct Cyc_Int_pa_struct _tmp613;return(struct
_dyneither_ptr)((_tmp613.tag=1,((_tmp613.f1=(unsigned long)_tmp13E,((_tmp614[0]=&
_tmp613,Cyc_aprintf(((_tmp615="regparm(%d)",_tag_dyneither(_tmp615,sizeof(char),
12))),_tag_dyneither(_tmp614,sizeof(void*),1))))))));}_LL8E: if((int)_tmp13D != 0)
goto _LL90;_LL8F: {const char*_tmp616;return(_tmp616="stdcall",_tag_dyneither(
_tmp616,sizeof(char),8));}_LL90: if((int)_tmp13D != 1)goto _LL92;_LL91: {const char*
_tmp617;return(_tmp617="cdecl",_tag_dyneither(_tmp617,sizeof(char),6));}_LL92:
if((int)_tmp13D != 2)goto _LL94;_LL93: {const char*_tmp618;return(_tmp618="fastcall",
_tag_dyneither(_tmp618,sizeof(char),9));}_LL94: if((int)_tmp13D != 3)goto _LL96;
_LL95: {const char*_tmp619;return(_tmp619="noreturn",_tag_dyneither(_tmp619,
sizeof(char),9));}_LL96: if((int)_tmp13D != 4)goto _LL98;_LL97: {const char*_tmp61A;
return(_tmp61A="const",_tag_dyneither(_tmp61A,sizeof(char),6));}_LL98: if(_tmp13D
<= (void*)17)goto _LL9A;if(*((int*)_tmp13D)!= 1)goto _LL9A;_tmp13F=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp13D)->f1;_LL99: if(_tmp13F == - 1){const char*_tmp61B;return(_tmp61B="aligned",
_tag_dyneither(_tmp61B,sizeof(char),8));}else{void*_tmp152=Cyc_Cyclone_c_compiler;
_LLBD: if((int)_tmp152 != 0)goto _LLBF;_LLBE: {const char*_tmp61F;void*_tmp61E[1];
struct Cyc_Int_pa_struct _tmp61D;return(struct _dyneither_ptr)((_tmp61D.tag=1,((
_tmp61D.f1=(unsigned long)_tmp13F,((_tmp61E[0]=& _tmp61D,Cyc_aprintf(((_tmp61F="aligned(%d)",
_tag_dyneither(_tmp61F,sizeof(char),12))),_tag_dyneither(_tmp61E,sizeof(void*),1))))))));}
_LLBF: if((int)_tmp152 != 1)goto _LLBC;_LLC0: {const char*_tmp623;void*_tmp622[1];
struct Cyc_Int_pa_struct _tmp621;return(struct _dyneither_ptr)((_tmp621.tag=1,((
_tmp621.f1=(unsigned long)_tmp13F,((_tmp622[0]=& _tmp621,Cyc_aprintf(((_tmp623="align(%d)",
_tag_dyneither(_tmp623,sizeof(char),10))),_tag_dyneither(_tmp622,sizeof(void*),1))))))));}
_LLBC:;}_LL9A: if((int)_tmp13D != 5)goto _LL9C;_LL9B: {const char*_tmp624;return(
_tmp624="packed",_tag_dyneither(_tmp624,sizeof(char),7));}_LL9C: if(_tmp13D <= (
void*)17)goto _LL9E;if(*((int*)_tmp13D)!= 2)goto _LL9E;_tmp140=((struct Cyc_Absyn_Section_att_struct*)
_tmp13D)->f1;_LL9D: {const char*_tmp628;void*_tmp627[1];struct Cyc_String_pa_struct
_tmp626;return(struct _dyneither_ptr)((_tmp626.tag=0,((_tmp626.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp140),((_tmp627[0]=& _tmp626,Cyc_aprintf(((
_tmp628="section(\"%s\")",_tag_dyneither(_tmp628,sizeof(char),14))),
_tag_dyneither(_tmp627,sizeof(void*),1))))))));}_LL9E: if((int)_tmp13D != 6)goto
_LLA0;_LL9F: {const char*_tmp629;return(_tmp629="nocommon",_tag_dyneither(_tmp629,
sizeof(char),9));}_LLA0: if((int)_tmp13D != 7)goto _LLA2;_LLA1: {const char*_tmp62A;
return(_tmp62A="shared",_tag_dyneither(_tmp62A,sizeof(char),7));}_LLA2: if((int)
_tmp13D != 8)goto _LLA4;_LLA3: {const char*_tmp62B;return(_tmp62B="unused",
_tag_dyneither(_tmp62B,sizeof(char),7));}_LLA4: if((int)_tmp13D != 9)goto _LLA6;
_LLA5: {const char*_tmp62C;return(_tmp62C="weak",_tag_dyneither(_tmp62C,sizeof(
char),5));}_LLA6: if((int)_tmp13D != 10)goto _LLA8;_LLA7: {const char*_tmp62D;return(
_tmp62D="dllimport",_tag_dyneither(_tmp62D,sizeof(char),10));}_LLA8: if((int)
_tmp13D != 11)goto _LLAA;_LLA9: {const char*_tmp62E;return(_tmp62E="dllexport",
_tag_dyneither(_tmp62E,sizeof(char),10));}_LLAA: if((int)_tmp13D != 12)goto _LLAC;
_LLAB: {const char*_tmp62F;return(_tmp62F="no_instrument_function",_tag_dyneither(
_tmp62F,sizeof(char),23));}_LLAC: if((int)_tmp13D != 13)goto _LLAE;_LLAD: {const
char*_tmp630;return(_tmp630="constructor",_tag_dyneither(_tmp630,sizeof(char),12));}
_LLAE: if((int)_tmp13D != 14)goto _LLB0;_LLAF: {const char*_tmp631;return(_tmp631="destructor",
_tag_dyneither(_tmp631,sizeof(char),11));}_LLB0: if((int)_tmp13D != 15)goto _LLB2;
_LLB1: {const char*_tmp632;return(_tmp632="no_check_memory_usage",_tag_dyneither(
_tmp632,sizeof(char),22));}_LLB2: if(_tmp13D <= (void*)17)goto _LLB8;if(*((int*)
_tmp13D)!= 3)goto _LLB4;_tmp141=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp13D)->f1;if((int)_tmp141 != 0)goto _LLB4;_tmp142=((struct Cyc_Absyn_Format_att_struct*)
_tmp13D)->f2;_tmp143=((struct Cyc_Absyn_Format_att_struct*)_tmp13D)->f3;_LLB3: {
const char*_tmp637;void*_tmp636[2];struct Cyc_Int_pa_struct _tmp635;struct Cyc_Int_pa_struct
_tmp634;return(struct _dyneither_ptr)((_tmp634.tag=1,((_tmp634.f1=(unsigned int)
_tmp143,((_tmp635.tag=1,((_tmp635.f1=(unsigned int)_tmp142,((_tmp636[0]=& _tmp635,((
_tmp636[1]=& _tmp634,Cyc_aprintf(((_tmp637="format(printf,%u,%u)",_tag_dyneither(
_tmp637,sizeof(char),21))),_tag_dyneither(_tmp636,sizeof(void*),2))))))))))))));}
_LLB4: if(*((int*)_tmp13D)!= 3)goto _LLB6;_tmp144=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp13D)->f1;if((int)_tmp144 != 1)goto _LLB6;_tmp145=((struct Cyc_Absyn_Format_att_struct*)
_tmp13D)->f2;_tmp146=((struct Cyc_Absyn_Format_att_struct*)_tmp13D)->f3;_LLB5: {
const char*_tmp63C;void*_tmp63B[2];struct Cyc_Int_pa_struct _tmp63A;struct Cyc_Int_pa_struct
_tmp639;return(struct _dyneither_ptr)((_tmp639.tag=1,((_tmp639.f1=(unsigned int)
_tmp146,((_tmp63A.tag=1,((_tmp63A.f1=(unsigned int)_tmp145,((_tmp63B[0]=& _tmp63A,((
_tmp63B[1]=& _tmp639,Cyc_aprintf(((_tmp63C="format(scanf,%u,%u)",_tag_dyneither(
_tmp63C,sizeof(char),20))),_tag_dyneither(_tmp63B,sizeof(void*),2))))))))))))));}
_LLB6: if(*((int*)_tmp13D)!= 4)goto _LLB8;_tmp147=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp13D)->f1;_LLB7: {const char*_tmp640;void*_tmp63F[1];struct Cyc_Int_pa_struct
_tmp63E;return(struct _dyneither_ptr)((_tmp63E.tag=1,((_tmp63E.f1=(unsigned long)
_tmp147,((_tmp63F[0]=& _tmp63E,Cyc_aprintf(((_tmp640="initializes(%d)",
_tag_dyneither(_tmp640,sizeof(char),16))),_tag_dyneither(_tmp63F,sizeof(void*),1))))))));}
_LLB8: if((int)_tmp13D != 16)goto _LLBA;_LLB9: {const char*_tmp641;return(_tmp641="pure",
_tag_dyneither(_tmp641,sizeof(char),5));}_LLBA: if(_tmp13D <= (void*)17)goto _LL8B;
if(*((int*)_tmp13D)!= 5)goto _LL8B;_tmp148=((struct Cyc_Absyn_Mode_att_struct*)
_tmp13D)->f1;_LLBB: {const char*_tmp645;void*_tmp644[1];struct Cyc_String_pa_struct
_tmp643;return(struct _dyneither_ptr)((_tmp643.tag=0,((_tmp643.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp148),((_tmp644[0]=& _tmp643,Cyc_aprintf(((
_tmp645="__mode__(\"%s\")",_tag_dyneither(_tmp645,sizeof(char),15))),
_tag_dyneither(_tmp644,sizeof(void*),1))))))));}_LL8B:;}int Cyc_Absyn_fntype_att(
void*a);int Cyc_Absyn_fntype_att(void*a){void*_tmp176=a;_LLC2: if(_tmp176 <= (void*)
17)goto _LLC4;if(*((int*)_tmp176)!= 0)goto _LLC4;_LLC3: goto _LLC5;_LLC4: if((int)
_tmp176 != 2)goto _LLC6;_LLC5: goto _LLC7;_LLC6: if((int)_tmp176 != 0)goto _LLC8;_LLC7:
goto _LLC9;_LLC8: if((int)_tmp176 != 1)goto _LLCA;_LLC9: goto _LLCB;_LLCA: if((int)
_tmp176 != 3)goto _LLCC;_LLCB: goto _LLCD;_LLCC: if((int)_tmp176 != 16)goto _LLCE;_LLCD:
goto _LLCF;_LLCE: if(_tmp176 <= (void*)17)goto _LLD0;if(*((int*)_tmp176)!= 3)goto
_LLD0;_LLCF: goto _LLD1;_LLD0: if((int)_tmp176 != 4)goto _LLD2;_LLD1: return 1;_LLD2:
if(_tmp176 <= (void*)17)goto _LLD4;if(*((int*)_tmp176)!= 4)goto _LLD4;_LLD3: return 1;
_LLD4:;_LLD5: return 0;_LLC1:;}static char _tmp177[3]="f0";static struct
_dyneither_ptr Cyc_Absyn_f0={_tmp177,_tmp177,_tmp177 + 3};static struct
_dyneither_ptr*Cyc_Absyn_field_names_v[1]={& Cyc_Absyn_f0};static struct
_dyneither_ptr Cyc_Absyn_field_names={(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(
void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct
_dyneither_ptr**)Cyc_Absyn_field_names_v + 1)};struct _dyneither_ptr*Cyc_Absyn_fieldname(
int i);static void _tmp653(unsigned int*fsz,unsigned int*_tmp652,unsigned int*
_tmp651,struct _dyneither_ptr***_tmp64F){for(*_tmp652=0;*_tmp652 < *_tmp651;(*
_tmp652)++){struct Cyc_Int_pa_struct _tmp64D;void*_tmp64C[1];const char*_tmp64B;
struct _dyneither_ptr*_tmp64A;(*_tmp64F)[*_tmp652]=*_tmp652 < *fsz?*((struct
_dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct
_dyneither_ptr*),(int)*_tmp652)):((_tmp64A=_cycalloc(sizeof(*_tmp64A)),((_tmp64A[
0]=(struct _dyneither_ptr)((_tmp64D.tag=1,((_tmp64D.f1=(unsigned long)((int)*
_tmp652),((_tmp64C[0]=& _tmp64D,Cyc_aprintf(((_tmp64B="f%d",_tag_dyneither(
_tmp64B,sizeof(char),4))),_tag_dyneither(_tmp64C,sizeof(void*),1)))))))),_tmp64A))));}}
struct _dyneither_ptr*Cyc_Absyn_fieldname(int i){unsigned int fsz=
_get_dyneither_size(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*));if(i >= 
fsz){unsigned int _tmp652;unsigned int _tmp651;struct _dyneither_ptr _tmp650;struct
_dyneither_ptr**_tmp64F;unsigned int _tmp64E;Cyc_Absyn_field_names=((_tmp64E=(
unsigned int)(i + 1),((_tmp64F=(struct _dyneither_ptr**)_cycalloc(_check_times(
sizeof(struct _dyneither_ptr*),_tmp64E)),((_tmp650=_tag_dyneither(_tmp64F,sizeof(
struct _dyneither_ptr*),_tmp64E),((((_tmp651=_tmp64E,_tmp653(& fsz,& _tmp652,&
_tmp651,& _tmp64F))),_tmp650))))))));}return*((struct _dyneither_ptr**)
_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),i));}
struct _tuple4 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU_union info);
struct _tuple4 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU_union info){
union Cyc_Absyn_AggrInfoU_union _tmp180=info;void*_tmp181;struct _tuple0*_tmp182;
struct Cyc_Absyn_Aggrdecl**_tmp183;struct Cyc_Absyn_Aggrdecl*_tmp184;struct Cyc_Absyn_Aggrdecl
_tmp185;void*_tmp186;struct _tuple0*_tmp187;_LLD7: if((_tmp180.UnknownAggr).tag != 
0)goto _LLD9;_tmp181=(_tmp180.UnknownAggr).f1;_tmp182=(_tmp180.UnknownAggr).f2;
_LLD8: {struct _tuple4 _tmp654;return(_tmp654.f1=_tmp181,((_tmp654.f2=_tmp182,
_tmp654)));}_LLD9: if((_tmp180.KnownAggr).tag != 1)goto _LLD6;_tmp183=(_tmp180.KnownAggr).f1;
_tmp184=*_tmp183;_tmp185=*_tmp184;_tmp186=(void*)_tmp185.kind;_tmp187=_tmp185.name;
_LLDA: {struct _tuple4 _tmp655;return(_tmp655.f1=_tmp186,((_tmp655.f2=_tmp187,
_tmp655)));}_LLD6:;}struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union
Cyc_Absyn_AggrInfoU_union info);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info){union Cyc_Absyn_AggrInfoU_union _tmp18A=info;
void*_tmp18B;struct _tuple0*_tmp18C;struct Cyc_Absyn_Aggrdecl**_tmp18D;struct Cyc_Absyn_Aggrdecl*
_tmp18E;_LLDC: if((_tmp18A.UnknownAggr).tag != 0)goto _LLDE;_tmp18B=(_tmp18A.UnknownAggr).f1;
_tmp18C=(_tmp18A.UnknownAggr).f2;_LLDD: {const char*_tmp658;void*_tmp657;(_tmp657=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp658="unchecked aggrdecl",_tag_dyneither(_tmp658,sizeof(char),19))),
_tag_dyneither(_tmp657,sizeof(void*),0)));}_LLDE: if((_tmp18A.KnownAggr).tag != 1)
goto _LLDB;_tmp18D=(_tmp18A.KnownAggr).f1;_tmp18E=*_tmp18D;_LLDF: return _tmp18E;
_LLDB:;}int Cyc_Absyn_is_union_type(void*t);int Cyc_Absyn_is_union_type(void*t){
void*_tmp191=Cyc_Tcutil_compress(t);void*_tmp192;struct Cyc_Absyn_AggrInfo _tmp193;
union Cyc_Absyn_AggrInfoU_union _tmp194;_LLE1: if(_tmp191 <= (void*)4)goto _LLE5;if(*((
int*)_tmp191)!= 11)goto _LLE3;_tmp192=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp191)->f1;if((int)_tmp192 != 1)goto _LLE3;_LLE2: return 1;_LLE3: if(*((int*)
_tmp191)!= 10)goto _LLE5;_tmp193=((struct Cyc_Absyn_AggrType_struct*)_tmp191)->f1;
_tmp194=_tmp193.aggr_info;_LLE4: return(Cyc_Absyn_aggr_kinded_name(_tmp194)).f1 == (
void*)1;_LLE5:;_LLE6: return 0;_LLE0:;}int Cyc_Absyn_is_aggr_type(void*t);int Cyc_Absyn_is_aggr_type(
void*t){void*_tmp195=Cyc_Tcutil_compress(t);_LLE8: if(_tmp195 <= (void*)4)goto
_LLF2;if(*((int*)_tmp195)!= 2)goto _LLEA;_LLE9: goto _LLEB;_LLEA: if(*((int*)_tmp195)
!= 3)goto _LLEC;_LLEB: goto _LLED;_LLEC: if(*((int*)_tmp195)!= 9)goto _LLEE;_LLED:
goto _LLEF;_LLEE: if(*((int*)_tmp195)!= 10)goto _LLF0;_LLEF: goto _LLF1;_LLF0: if(*((
int*)_tmp195)!= 11)goto _LLF2;_LLF1: return 1;_LLF2:;_LLF3: return 0;_LLE7:;}void Cyc_Absyn_print_decls(
struct Cyc_List_List*decls);void Cyc_Absyn_print_decls(struct Cyc_List_List*decls){((
void(*)(void*rep,struct Cyc_List_List**val))Cyc_Marshal_print_type)(Cyc_decls_rep,&
decls);{const char*_tmp65B;void*_tmp65A;(_tmp65A=0,Cyc_printf(((_tmp65B="\n",
_tag_dyneither(_tmp65B,sizeof(char),2))),_tag_dyneither(_tmp65A,sizeof(void*),0)));}}
int Cyc_Absyn_porting_c_code=0;int Cyc_Absyn_no_regions=0;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_0;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Decl_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_800;static struct Cyc_Typerep_Int_struct
Cyc__genrep_24={0,0,32};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_148;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep;extern struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep;static char _tmp199[7]="Static";
static struct _tuple7 Cyc__gentuple_156={0,(struct _dyneither_ptr){_tmp199,_tmp199,
_tmp199 + 7}};static char _tmp19A[9]="Abstract";static struct _tuple7 Cyc__gentuple_157={
1,(struct _dyneither_ptr){_tmp19A,_tmp19A,_tmp19A + 9}};static char _tmp19B[7]="Public";
static struct _tuple7 Cyc__gentuple_158={2,(struct _dyneither_ptr){_tmp19B,_tmp19B,
_tmp19B + 7}};static char _tmp19C[7]="Extern";static struct _tuple7 Cyc__gentuple_159={
3,(struct _dyneither_ptr){_tmp19C,_tmp19C,_tmp19C + 7}};static char _tmp19D[8]="ExternC";
static struct _tuple7 Cyc__gentuple_160={4,(struct _dyneither_ptr){_tmp19D,_tmp19D,
_tmp19D + 8}};static char _tmp19E[9]="Register";static struct _tuple7 Cyc__gentuple_161={
5,(struct _dyneither_ptr){_tmp19E,_tmp19E,_tmp19E + 9}};static struct _tuple7*Cyc__genarr_162[
6]={& Cyc__gentuple_156,& Cyc__gentuple_157,& Cyc__gentuple_158,& Cyc__gentuple_159,&
Cyc__gentuple_160,& Cyc__gentuple_161};static struct _tuple5*Cyc__genarr_163[0]={};
static char _tmp1A0[6]="Scope";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep={
5,(struct _dyneither_ptr){_tmp1A0,_tmp1A0,_tmp1A0 + 6},{(void*)((struct _tuple7**)
Cyc__genarr_162),(void*)((struct _tuple7**)Cyc__genarr_162),(void*)((struct
_tuple7**)Cyc__genarr_162 + 6)},{(void*)((struct _tuple5**)Cyc__genarr_163),(void*)((
struct _tuple5**)Cyc__genarr_163),(void*)((struct _tuple5**)Cyc__genarr_163 + 0)}};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_11;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_12;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_18;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_19;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_var_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_13;extern struct Cyc_Typerep_FatPtr_struct
Cyc__genrep_14;static struct Cyc_Typerep_Int_struct Cyc__genrep_15={0,0,8};static
struct Cyc_Typerep_FatPtr_struct Cyc__genrep_14={2,(void*)((void*)& Cyc__genrep_15)};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_13={1,1,(void*)((void*)& Cyc__genrep_14)};
static char _tmp1A4[5]="List";static struct _dyneither_ptr Cyc__genname_23=(struct
_dyneither_ptr){_tmp1A4,_tmp1A4,_tmp1A4 + 5};static char _tmp1A5[3]="hd";static
struct _tuple5 Cyc__gentuple_20={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp1A5,_tmp1A5,_tmp1A5 + 3},(void*)& Cyc__genrep_13};static char
_tmp1A6[3]="tl";static struct _tuple5 Cyc__gentuple_21={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp1A6,_tmp1A6,_tmp1A6 + 3},(void*)& Cyc__genrep_19};static
struct _tuple5*Cyc__genarr_22[2]={& Cyc__gentuple_20,& Cyc__gentuple_21};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_var_t46H2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_23,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_22),(void*)((
struct _tuple5**)Cyc__genarr_22),(void*)((struct _tuple5**)Cyc__genarr_22 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_19={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_var_t46H2_rep)};
struct _tuple14{unsigned int f1;struct Cyc_List_List*f2;};static struct _tuple6 Cyc__gentuple_25={
offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_26={
offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_19};static struct _tuple6*Cyc__genarr_27[
2]={& Cyc__gentuple_25,& Cyc__gentuple_26};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_18={
4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_27),(void*)((
struct _tuple6**)Cyc__genarr_27),(void*)((struct _tuple6**)Cyc__genarr_27 + 2)}};
static char _tmp1AA[6]="Loc_n";static struct _tuple7 Cyc__gentuple_16={0,(struct
_dyneither_ptr){_tmp1AA,_tmp1AA,_tmp1AA + 6}};static struct _tuple7*Cyc__genarr_17[
1]={& Cyc__gentuple_16};static char _tmp1AB[6]="Rel_n";static struct _tuple5 Cyc__gentuple_28={
0,(struct _dyneither_ptr){_tmp1AB,_tmp1AB,_tmp1AB + 6},(void*)& Cyc__genrep_18};
static char _tmp1AC[6]="Abs_n";static struct _tuple5 Cyc__gentuple_29={1,(struct
_dyneither_ptr){_tmp1AC,_tmp1AC,_tmp1AC + 6},(void*)& Cyc__genrep_18};static struct
_tuple5*Cyc__genarr_30[2]={& Cyc__gentuple_28,& Cyc__gentuple_29};static char
_tmp1AE[8]="Nmspace";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep={5,(
struct _dyneither_ptr){_tmp1AE,_tmp1AE,_tmp1AE + 8},{(void*)((struct _tuple7**)Cyc__genarr_17),(
void*)((struct _tuple7**)Cyc__genarr_17),(void*)((struct _tuple7**)Cyc__genarr_17 + 
1)},{(void*)((struct _tuple5**)Cyc__genarr_30),(void*)((struct _tuple5**)Cyc__genarr_30),(
void*)((struct _tuple5**)Cyc__genarr_30 + 2)}};static struct _tuple6 Cyc__gentuple_31={
offsetof(struct _tuple0,f1),(void*)& Cyc_Absyn_nmspace_t_rep};static struct _tuple6
Cyc__gentuple_32={offsetof(struct _tuple0,f2),(void*)& Cyc__genrep_13};static
struct _tuple6*Cyc__genarr_33[2]={& Cyc__gentuple_31,& Cyc__gentuple_32};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_12={4,sizeof(struct _tuple0),{(void*)((
struct _tuple6**)Cyc__genarr_33),(void*)((struct _tuple6**)Cyc__genarr_33),(void*)((
struct _tuple6**)Cyc__genarr_33 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_11={
1,1,(void*)((void*)& Cyc__genrep_12)};extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_tqual_t_rep;
static struct Cyc_Typerep_Int_struct Cyc__genrep_10={0,1,32};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_2;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2={1,1,(void*)((void*)& Cyc_struct_Position_Segment_rep)};
static char _tmp1B3[6]="Tqual";static struct _dyneither_ptr Cyc__genname_155=(struct
_dyneither_ptr){_tmp1B3,_tmp1B3,_tmp1B3 + 6};static char _tmp1B4[12]="print_const";
static struct _tuple5 Cyc__gentuple_149={offsetof(struct Cyc_Absyn_Tqual,print_const),(
struct _dyneither_ptr){_tmp1B4,_tmp1B4,_tmp1B4 + 12},(void*)& Cyc__genrep_10};
static char _tmp1B5[11]="q_volatile";static struct _tuple5 Cyc__gentuple_150={
offsetof(struct Cyc_Absyn_Tqual,q_volatile),(struct _dyneither_ptr){_tmp1B5,
_tmp1B5,_tmp1B5 + 11},(void*)& Cyc__genrep_10};static char _tmp1B6[11]="q_restrict";
static struct _tuple5 Cyc__gentuple_151={offsetof(struct Cyc_Absyn_Tqual,q_restrict),(
struct _dyneither_ptr){_tmp1B6,_tmp1B6,_tmp1B6 + 11},(void*)& Cyc__genrep_10};
static char _tmp1B7[11]="real_const";static struct _tuple5 Cyc__gentuple_152={
offsetof(struct Cyc_Absyn_Tqual,real_const),(struct _dyneither_ptr){_tmp1B7,
_tmp1B7,_tmp1B7 + 11},(void*)& Cyc__genrep_10};static char _tmp1B8[4]="loc";static
struct _tuple5 Cyc__gentuple_153={offsetof(struct Cyc_Absyn_Tqual,loc),(struct
_dyneither_ptr){_tmp1B8,_tmp1B8,_tmp1B8 + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_154[5]={& Cyc__gentuple_149,& Cyc__gentuple_150,& Cyc__gentuple_151,&
Cyc__gentuple_152,& Cyc__gentuple_153};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tqual_t_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_155,sizeof(struct Cyc_Absyn_Tqual),{(void*)((
struct _tuple5**)Cyc__genarr_154),(void*)((struct _tuple5**)Cyc__genarr_154),(void*)((
struct _tuple5**)Cyc__genarr_154 + 5)}};extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1102;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1107;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kind_t2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep;static char _tmp1BA[8]="AnyKind";
static struct _tuple7 Cyc__gentuple_277={0,(struct _dyneither_ptr){_tmp1BA,_tmp1BA,
_tmp1BA + 8}};static char _tmp1BB[8]="MemKind";static struct _tuple7 Cyc__gentuple_278={
1,(struct _dyneither_ptr){_tmp1BB,_tmp1BB,_tmp1BB + 8}};static char _tmp1BC[8]="BoxKind";
static struct _tuple7 Cyc__gentuple_279={2,(struct _dyneither_ptr){_tmp1BC,_tmp1BC,
_tmp1BC + 8}};static char _tmp1BD[8]="RgnKind";static struct _tuple7 Cyc__gentuple_280={
3,(struct _dyneither_ptr){_tmp1BD,_tmp1BD,_tmp1BD + 8}};static char _tmp1BE[14]="UniqueRgnKind";
static struct _tuple7 Cyc__gentuple_281={4,(struct _dyneither_ptr){_tmp1BE,_tmp1BE,
_tmp1BE + 14}};static char _tmp1BF[11]="TopRgnKind";static struct _tuple7 Cyc__gentuple_282={
5,(struct _dyneither_ptr){_tmp1BF,_tmp1BF,_tmp1BF + 11}};static char _tmp1C0[8]="EffKind";
static struct _tuple7 Cyc__gentuple_283={6,(struct _dyneither_ptr){_tmp1C0,_tmp1C0,
_tmp1C0 + 8}};static char _tmp1C1[8]="IntKind";static struct _tuple7 Cyc__gentuple_284={
7,(struct _dyneither_ptr){_tmp1C1,_tmp1C1,_tmp1C1 + 8}};static struct _tuple7*Cyc__genarr_285[
8]={& Cyc__gentuple_277,& Cyc__gentuple_278,& Cyc__gentuple_279,& Cyc__gentuple_280,&
Cyc__gentuple_281,& Cyc__gentuple_282,& Cyc__gentuple_283,& Cyc__gentuple_284};
static struct _tuple5*Cyc__genarr_286[0]={};static char _tmp1C3[5]="Kind";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kind_t_rep={5,(struct _dyneither_ptr){_tmp1C3,_tmp1C3,_tmp1C3 + 5},{(
void*)((struct _tuple7**)Cyc__genarr_285),(void*)((struct _tuple7**)Cyc__genarr_285),(
void*)((struct _tuple7**)Cyc__genarr_285 + 8)},{(void*)((struct _tuple5**)Cyc__genarr_286),(
void*)((struct _tuple5**)Cyc__genarr_286),(void*)((struct _tuple5**)Cyc__genarr_286
+ 0)}};static char _tmp1C4[4]="Opt";static struct _dyneither_ptr Cyc__genname_1110=(
struct _dyneither_ptr){_tmp1C4,_tmp1C4,_tmp1C4 + 4};static char _tmp1C5[2]="v";
static struct _tuple5 Cyc__gentuple_1108={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp1C5,_tmp1C5,_tmp1C5 + 2},(void*)& Cyc_Absyn_kind_t_rep};static
struct _tuple5*Cyc__genarr_1109[1]={& Cyc__gentuple_1108};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kind_t2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_1110,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_1109),(void*)((
struct _tuple5**)Cyc__genarr_1109),(void*)((struct _tuple5**)Cyc__genarr_1109 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1107={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kind_t2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_110;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_type_t2_rep;static char _tmp1C8[4]="Opt";static struct
_dyneither_ptr Cyc__genname_1142=(struct _dyneither_ptr){_tmp1C8,_tmp1C8,_tmp1C8 + 
4};static char _tmp1C9[2]="v";static struct _tuple5 Cyc__gentuple_1140={offsetof(
struct Cyc_Core_Opt,v),(struct _dyneither_ptr){_tmp1C9,_tmp1C9,_tmp1C9 + 2},(void*)&
Cyc_Absyn_type_t_rep};static struct _tuple5*Cyc__genarr_1141[1]={& Cyc__gentuple_1140};
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_type_t2_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_1142,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_1141),(void*)((struct _tuple5**)Cyc__genarr_1141),(void*)((
struct _tuple5**)Cyc__genarr_1141 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_110={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_type_t2_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1103;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_273;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tvar_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_274;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kindbound_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_299;static struct _tuple6 Cyc__gentuple_300={offsetof(struct _tuple6,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_301={offsetof(struct
_tuple6,f2),(void*)& Cyc_Absyn_kind_t_rep};static struct _tuple6*Cyc__genarr_302[2]={&
Cyc__gentuple_300,& Cyc__gentuple_301};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_299={
4,sizeof(struct _tuple6),{(void*)((struct _tuple6**)Cyc__genarr_302),(void*)((
struct _tuple6**)Cyc__genarr_302),(void*)((struct _tuple6**)Cyc__genarr_302 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_295;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_287;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep;
static char _tmp1CD[4]="Opt";static struct _dyneither_ptr Cyc__genname_290=(struct
_dyneither_ptr){_tmp1CD,_tmp1CD,_tmp1CD + 4};static char _tmp1CE[2]="v";static
struct _tuple5 Cyc__gentuple_288={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp1CE,_tmp1CE,_tmp1CE + 2},(void*)& Cyc_Absyn_kindbound_t_rep};
static struct _tuple5*Cyc__genarr_289[1]={& Cyc__gentuple_288};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_290,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_289),(void*)((
struct _tuple5**)Cyc__genarr_289),(void*)((struct _tuple5**)Cyc__genarr_289 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_287={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep)};
struct _tuple15{unsigned int f1;struct Cyc_Core_Opt*f2;};static struct _tuple6 Cyc__gentuple_296={
offsetof(struct _tuple15,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_297={
offsetof(struct _tuple15,f2),(void*)& Cyc__genrep_287};static struct _tuple6*Cyc__genarr_298[
2]={& Cyc__gentuple_296,& Cyc__gentuple_297};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_295={4,sizeof(struct _tuple15),{(void*)((struct _tuple6**)Cyc__genarr_298),(
void*)((struct _tuple6**)Cyc__genarr_298),(void*)((struct _tuple6**)Cyc__genarr_298
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_276;struct _tuple16{
unsigned int f1;struct Cyc_Core_Opt*f2;void*f3;};static struct _tuple6 Cyc__gentuple_291={
offsetof(struct _tuple16,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_292={
offsetof(struct _tuple16,f2),(void*)& Cyc__genrep_287};static struct _tuple6 Cyc__gentuple_293={
offsetof(struct _tuple16,f3),(void*)& Cyc_Absyn_kind_t_rep};static struct _tuple6*
Cyc__genarr_294[3]={& Cyc__gentuple_291,& Cyc__gentuple_292,& Cyc__gentuple_293};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_276={4,sizeof(struct _tuple16),{(
void*)((struct _tuple6**)Cyc__genarr_294),(void*)((struct _tuple6**)Cyc__genarr_294),(
void*)((struct _tuple6**)Cyc__genarr_294 + 3)}};static struct _tuple7*Cyc__genarr_275[
0]={};static char _tmp1D3[6]="Eq_kb";static struct _tuple5 Cyc__gentuple_303={0,(
struct _dyneither_ptr){_tmp1D3,_tmp1D3,_tmp1D3 + 6},(void*)& Cyc__genrep_299};
static char _tmp1D4[11]="Unknown_kb";static struct _tuple5 Cyc__gentuple_304={1,(
struct _dyneither_ptr){_tmp1D4,_tmp1D4,_tmp1D4 + 11},(void*)& Cyc__genrep_295};
static char _tmp1D5[8]="Less_kb";static struct _tuple5 Cyc__gentuple_305={2,(struct
_dyneither_ptr){_tmp1D5,_tmp1D5,_tmp1D5 + 8},(void*)& Cyc__genrep_276};static
struct _tuple5*Cyc__genarr_306[3]={& Cyc__gentuple_303,& Cyc__gentuple_304,& Cyc__gentuple_305};
static char _tmp1D7[10]="KindBound";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kindbound_t_rep={
5,(struct _dyneither_ptr){_tmp1D7,_tmp1D7,_tmp1D7 + 10},{(void*)((struct _tuple7**)
Cyc__genarr_275),(void*)((struct _tuple7**)Cyc__genarr_275),(void*)((struct
_tuple7**)Cyc__genarr_275 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_306),(void*)((
struct _tuple5**)Cyc__genarr_306),(void*)((struct _tuple5**)Cyc__genarr_306 + 3)}};
static char _tmp1D8[5]="Tvar";static struct _dyneither_ptr Cyc__genname_311=(struct
_dyneither_ptr){_tmp1D8,_tmp1D8,_tmp1D8 + 5};static char _tmp1D9[5]="name";static
struct _tuple5 Cyc__gentuple_307={offsetof(struct Cyc_Absyn_Tvar,name),(struct
_dyneither_ptr){_tmp1D9,_tmp1D9,_tmp1D9 + 5},(void*)& Cyc__genrep_13};static char
_tmp1DA[9]="identity";static struct _tuple5 Cyc__gentuple_308={offsetof(struct Cyc_Absyn_Tvar,identity),(
struct _dyneither_ptr){_tmp1DA,_tmp1DA,_tmp1DA + 9},(void*)& Cyc__genrep_10};static
char _tmp1DB[5]="kind";static struct _tuple5 Cyc__gentuple_309={offsetof(struct Cyc_Absyn_Tvar,kind),(
struct _dyneither_ptr){_tmp1DB,_tmp1DB,_tmp1DB + 5},(void*)& Cyc_Absyn_kindbound_t_rep};
static struct _tuple5*Cyc__genarr_310[3]={& Cyc__gentuple_307,& Cyc__gentuple_308,&
Cyc__gentuple_309};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_311,sizeof(struct Cyc_Absyn_Tvar),{(void*)((
struct _tuple5**)Cyc__genarr_310),(void*)((struct _tuple5**)Cyc__genarr_310),(void*)((
struct _tuple5**)Cyc__genarr_310 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_274={
1,1,(void*)((void*)& Cyc_struct_Absyn_Tvar_rep)};static char _tmp1DE[5]="List";
static struct _dyneither_ptr Cyc__genname_315=(struct _dyneither_ptr){_tmp1DE,
_tmp1DE,_tmp1DE + 5};static char _tmp1DF[3]="hd";static struct _tuple5 Cyc__gentuple_312={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp1DF,_tmp1DF,_tmp1DF + 
3},(void*)& Cyc__genrep_274};static char _tmp1E0[3]="tl";static struct _tuple5 Cyc__gentuple_313={
offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){_tmp1E0,_tmp1E0,_tmp1E0 + 
3},(void*)& Cyc__genrep_273};static struct _tuple5*Cyc__genarr_314[2]={& Cyc__gentuple_312,&
Cyc__gentuple_313};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tvar_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_315,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_314),(void*)((struct _tuple5**)Cyc__genarr_314),(void*)((
struct _tuple5**)Cyc__genarr_314 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_273={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tvar_t46H2_rep)};static char
_tmp1E3[4]="Opt";static struct _dyneither_ptr Cyc__genname_1106=(struct
_dyneither_ptr){_tmp1E3,_tmp1E3,_tmp1E3 + 4};static char _tmp1E4[2]="v";static
struct _tuple5 Cyc__gentuple_1104={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp1E4,_tmp1E4,_tmp1E4 + 2},(void*)& Cyc__genrep_273};static
struct _tuple5*Cyc__genarr_1105[1]={& Cyc__gentuple_1104};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep={3,(struct _dyneither_ptr*)&
Cyc__genname_1106,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_1105),(
void*)((struct _tuple5**)Cyc__genarr_1105),(void*)((struct _tuple5**)Cyc__genarr_1105
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1103={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep)};struct _tuple17{
unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;int f4;struct Cyc_Core_Opt*
f5;};static struct _tuple6 Cyc__gentuple_1111={offsetof(struct _tuple17,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1112={offsetof(struct _tuple17,f2),(
void*)& Cyc__genrep_1107};static struct _tuple6 Cyc__gentuple_1113={offsetof(struct
_tuple17,f3),(void*)& Cyc__genrep_110};static struct _tuple6 Cyc__gentuple_1114={
offsetof(struct _tuple17,f4),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_1115={
offsetof(struct _tuple17,f5),(void*)& Cyc__genrep_1103};static struct _tuple6*Cyc__genarr_1116[
5]={& Cyc__gentuple_1111,& Cyc__gentuple_1112,& Cyc__gentuple_1113,& Cyc__gentuple_1114,&
Cyc__gentuple_1115};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1102={4,
sizeof(struct _tuple17),{(void*)((struct _tuple6**)Cyc__genarr_1116),(void*)((
struct _tuple6**)Cyc__genarr_1116),(void*)((struct _tuple6**)Cyc__genarr_1116 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1098;struct _tuple18{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;};static struct _tuple6 Cyc__gentuple_1099={offsetof(
struct _tuple18,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1100={
offsetof(struct _tuple18,f2),(void*)& Cyc__genrep_274};static struct _tuple6*Cyc__genarr_1101[
2]={& Cyc__gentuple_1099,& Cyc__gentuple_1100};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1098={4,sizeof(struct _tuple18),{(void*)((struct _tuple6**)Cyc__genarr_1101),(
void*)((struct _tuple6**)Cyc__genarr_1101),(void*)((struct _tuple6**)Cyc__genarr_1101
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1071;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1078;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep;static char _tmp1E9[18]="UnknownTunionInfo";
static struct _dyneither_ptr Cyc__genname_1083=(struct _dyneither_ptr){_tmp1E9,
_tmp1E9,_tmp1E9 + 18};static char _tmp1EA[5]="name";static struct _tuple5 Cyc__gentuple_1079={
offsetof(struct Cyc_Absyn_UnknownTunionInfo,name),(struct _dyneither_ptr){_tmp1EA,
_tmp1EA,_tmp1EA + 5},(void*)& Cyc__genrep_11};static char _tmp1EB[11]="is_xtunion";
static struct _tuple5 Cyc__gentuple_1080={offsetof(struct Cyc_Absyn_UnknownTunionInfo,is_xtunion),(
struct _dyneither_ptr){_tmp1EB,_tmp1EB,_tmp1EB + 11},(void*)& Cyc__genrep_10};
static char _tmp1EC[8]="is_flat";static struct _tuple5 Cyc__gentuple_1081={offsetof(
struct Cyc_Absyn_UnknownTunionInfo,is_flat),(struct _dyneither_ptr){_tmp1EC,
_tmp1EC,_tmp1EC + 8},(void*)& Cyc__genrep_10};static struct _tuple5*Cyc__genarr_1082[
3]={& Cyc__gentuple_1079,& Cyc__gentuple_1080,& Cyc__gentuple_1081};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep={3,(struct _dyneither_ptr*)& Cyc__genname_1083,
sizeof(struct Cyc_Absyn_UnknownTunionInfo),{(void*)((struct _tuple5**)Cyc__genarr_1082),(
void*)((struct _tuple5**)Cyc__genarr_1082),(void*)((struct _tuple5**)Cyc__genarr_1082
+ 3)}};struct _tuple19{unsigned int f1;struct Cyc_Absyn_UnknownTunionInfo f2;};
static struct _tuple6 Cyc__gentuple_1084={offsetof(struct _tuple19,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1085={offsetof(struct _tuple19,f2),(void*)& Cyc_struct_Absyn_UnknownTunionInfo_rep};
static struct _tuple6*Cyc__genarr_1086[2]={& Cyc__gentuple_1084,& Cyc__gentuple_1085};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1078={4,sizeof(struct _tuple19),{(
void*)((struct _tuple6**)Cyc__genarr_1086),(void*)((struct _tuple6**)Cyc__genarr_1086),(
void*)((struct _tuple6**)Cyc__genarr_1086 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1073;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1074;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_263;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Tuniondecl_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_264;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_265;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_246;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_247;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_248;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_249;static struct
_tuple6 Cyc__gentuple_250={offsetof(struct _tuple3,f1),(void*)& Cyc_Absyn_tqual_t_rep};
static struct _tuple6 Cyc__gentuple_251={offsetof(struct _tuple3,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_252[2]={& Cyc__gentuple_250,&
Cyc__gentuple_251};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_249={4,
sizeof(struct _tuple3),{(void*)((struct _tuple6**)Cyc__genarr_252),(void*)((struct
_tuple6**)Cyc__genarr_252),(void*)((struct _tuple6**)Cyc__genarr_252 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_248={1,1,(void*)((void*)& Cyc__genrep_249)};
static char _tmp1F1[5]="List";static struct _dyneither_ptr Cyc__genname_256=(struct
_dyneither_ptr){_tmp1F1,_tmp1F1,_tmp1F1 + 5};static char _tmp1F2[3]="hd";static
struct _tuple5 Cyc__gentuple_253={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp1F2,_tmp1F2,_tmp1F2 + 3},(void*)& Cyc__genrep_248};static char
_tmp1F3[3]="tl";static struct _tuple5 Cyc__gentuple_254={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp1F3,_tmp1F3,_tmp1F3 + 3},(void*)& Cyc__genrep_247};
static struct _tuple5*Cyc__genarr_255[2]={& Cyc__gentuple_253,& Cyc__gentuple_254};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_256,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_255),(void*)((struct _tuple5**)Cyc__genarr_255),(void*)((
struct _tuple5**)Cyc__genarr_255 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_247={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
static char _tmp1F6[12]="Tunionfield";static struct _dyneither_ptr Cyc__genname_262=(
struct _dyneither_ptr){_tmp1F6,_tmp1F6,_tmp1F6 + 12};static char _tmp1F7[5]="name";
static struct _tuple5 Cyc__gentuple_257={offsetof(struct Cyc_Absyn_Tunionfield,name),(
struct _dyneither_ptr){_tmp1F7,_tmp1F7,_tmp1F7 + 5},(void*)& Cyc__genrep_11};static
char _tmp1F8[5]="typs";static struct _tuple5 Cyc__gentuple_258={offsetof(struct Cyc_Absyn_Tunionfield,typs),(
struct _dyneither_ptr){_tmp1F8,_tmp1F8,_tmp1F8 + 5},(void*)& Cyc__genrep_247};
static char _tmp1F9[4]="loc";static struct _tuple5 Cyc__gentuple_259={offsetof(struct
Cyc_Absyn_Tunionfield,loc),(struct _dyneither_ptr){_tmp1F9,_tmp1F9,_tmp1F9 + 4},(
void*)& Cyc__genrep_2};static char _tmp1FA[3]="sc";static struct _tuple5 Cyc__gentuple_260={
offsetof(struct Cyc_Absyn_Tunionfield,sc),(struct _dyneither_ptr){_tmp1FA,_tmp1FA,
_tmp1FA + 3},(void*)& Cyc_Absyn_scope_t_rep};static struct _tuple5*Cyc__genarr_261[4]={&
Cyc__gentuple_257,& Cyc__gentuple_258,& Cyc__gentuple_259,& Cyc__gentuple_260};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_262,sizeof(struct Cyc_Absyn_Tunionfield),{(void*)((
struct _tuple5**)Cyc__genarr_261),(void*)((struct _tuple5**)Cyc__genarr_261),(void*)((
struct _tuple5**)Cyc__genarr_261 + 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_246={
1,1,(void*)((void*)& Cyc_struct_Absyn_Tunionfield_rep)};static char _tmp1FD[5]="List";
static struct _dyneither_ptr Cyc__genname_269=(struct _dyneither_ptr){_tmp1FD,
_tmp1FD,_tmp1FD + 5};static char _tmp1FE[3]="hd";static struct _tuple5 Cyc__gentuple_266={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp1FE,_tmp1FE,_tmp1FE + 
3},(void*)& Cyc__genrep_246};static char _tmp1FF[3]="tl";static struct _tuple5 Cyc__gentuple_267={
offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){_tmp1FF,_tmp1FF,_tmp1FF + 
3},(void*)& Cyc__genrep_265};static struct _tuple5*Cyc__genarr_268[2]={& Cyc__gentuple_266,&
Cyc__gentuple_267};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_269,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_268),(void*)((struct _tuple5**)Cyc__genarr_268),(void*)((
struct _tuple5**)Cyc__genarr_268 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_265={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep)};static
char _tmp202[4]="Opt";static struct _dyneither_ptr Cyc__genname_272=(struct
_dyneither_ptr){_tmp202,_tmp202,_tmp202 + 4};static char _tmp203[2]="v";static
struct _tuple5 Cyc__gentuple_270={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp203,_tmp203,_tmp203 + 2},(void*)& Cyc__genrep_265};static
struct _tuple5*Cyc__genarr_271[1]={& Cyc__gentuple_270};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_272,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_271),(void*)((struct _tuple5**)Cyc__genarr_271),(void*)((
struct _tuple5**)Cyc__genarr_271 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_264={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep)};
static char _tmp206[11]="Tuniondecl";static struct _dyneither_ptr Cyc__genname_323=(
struct _dyneither_ptr){_tmp206,_tmp206,_tmp206 + 11};static char _tmp207[3]="sc";
static struct _tuple5 Cyc__gentuple_316={offsetof(struct Cyc_Absyn_Tuniondecl,sc),(
struct _dyneither_ptr){_tmp207,_tmp207,_tmp207 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp208[5]="name";static struct _tuple5 Cyc__gentuple_317={offsetof(
struct Cyc_Absyn_Tuniondecl,name),(struct _dyneither_ptr){_tmp208,_tmp208,_tmp208 + 
5},(void*)& Cyc__genrep_11};static char _tmp209[4]="tvs";static struct _tuple5 Cyc__gentuple_318={
offsetof(struct Cyc_Absyn_Tuniondecl,tvs),(struct _dyneither_ptr){_tmp209,_tmp209,
_tmp209 + 4},(void*)& Cyc__genrep_273};static char _tmp20A[7]="fields";static struct
_tuple5 Cyc__gentuple_319={offsetof(struct Cyc_Absyn_Tuniondecl,fields),(struct
_dyneither_ptr){_tmp20A,_tmp20A,_tmp20A + 7},(void*)& Cyc__genrep_264};static char
_tmp20B[11]="is_xtunion";static struct _tuple5 Cyc__gentuple_320={offsetof(struct
Cyc_Absyn_Tuniondecl,is_xtunion),(struct _dyneither_ptr){_tmp20B,_tmp20B,_tmp20B + 
11},(void*)& Cyc__genrep_10};static char _tmp20C[8]="is_flat";static struct _tuple5
Cyc__gentuple_321={offsetof(struct Cyc_Absyn_Tuniondecl,is_flat),(struct
_dyneither_ptr){_tmp20C,_tmp20C,_tmp20C + 8},(void*)& Cyc__genrep_10};static struct
_tuple5*Cyc__genarr_322[6]={& Cyc__gentuple_316,& Cyc__gentuple_317,& Cyc__gentuple_318,&
Cyc__gentuple_319,& Cyc__gentuple_320,& Cyc__gentuple_321};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Tuniondecl_rep={3,(struct _dyneither_ptr*)& Cyc__genname_323,
sizeof(struct Cyc_Absyn_Tuniondecl),{(void*)((struct _tuple5**)Cyc__genarr_322),(
void*)((struct _tuple5**)Cyc__genarr_322),(void*)((struct _tuple5**)Cyc__genarr_322
+ 6)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_263={1,1,(void*)((void*)&
Cyc_struct_Absyn_Tuniondecl_rep)};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1074={
1,1,(void*)((void*)& Cyc__genrep_263)};struct _tuple20{unsigned int f1;struct Cyc_Absyn_Tuniondecl**
f2;};static struct _tuple6 Cyc__gentuple_1075={offsetof(struct _tuple20,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1076={offsetof(struct _tuple20,f2),(
void*)& Cyc__genrep_1074};static struct _tuple6*Cyc__genarr_1077[2]={& Cyc__gentuple_1075,&
Cyc__gentuple_1076};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1073={4,
sizeof(struct _tuple20),{(void*)((struct _tuple6**)Cyc__genarr_1077),(void*)((
struct _tuple6**)Cyc__genarr_1077),(void*)((struct _tuple6**)Cyc__genarr_1077 + 2)}};
static struct _tuple7*Cyc__genarr_1072[0]={};static char _tmp211[14]="UnknownTunion";
static struct _tuple5 Cyc__gentuple_1087={0,(struct _dyneither_ptr){_tmp211,_tmp211,
_tmp211 + 14},(void*)& Cyc__genrep_1078};static char _tmp212[12]="KnownTunion";
static struct _tuple5 Cyc__gentuple_1088={1,(struct _dyneither_ptr){_tmp212,_tmp212,
_tmp212 + 12},(void*)& Cyc__genrep_1073};static struct _tuple5*Cyc__genarr_1089[2]={&
Cyc__gentuple_1087,& Cyc__gentuple_1088};static char _tmp214[12]="TunionInfoU";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep={5,(struct
_dyneither_ptr){_tmp214,_tmp214,_tmp214 + 12},{(void*)((struct _tuple7**)Cyc__genarr_1072),(
void*)((struct _tuple7**)Cyc__genarr_1072),(void*)((struct _tuple7**)Cyc__genarr_1072
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1089),(void*)((struct _tuple5**)Cyc__genarr_1089),(
void*)((struct _tuple5**)Cyc__genarr_1089 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_121;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep;
static char _tmp215[5]="List";static struct _dyneither_ptr Cyc__genname_125=(struct
_dyneither_ptr){_tmp215,_tmp215,_tmp215 + 5};static char _tmp216[3]="hd";static
struct _tuple5 Cyc__gentuple_122={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp216,_tmp216,_tmp216 + 3},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp217[3]="tl";static struct _tuple5 Cyc__gentuple_123={offsetof(struct
Cyc_List_List,tl),(struct _dyneither_ptr){_tmp217,_tmp217,_tmp217 + 3},(void*)& Cyc__genrep_121};
static struct _tuple5*Cyc__genarr_124[2]={& Cyc__gentuple_122,& Cyc__gentuple_123};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_125,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_124),(void*)((struct _tuple5**)Cyc__genarr_124),(void*)((
struct _tuple5**)Cyc__genarr_124 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_121={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_type_t46H2_rep)};static char
_tmp21A[11]="TunionInfo";static struct _dyneither_ptr Cyc__genname_1094=(struct
_dyneither_ptr){_tmp21A,_tmp21A,_tmp21A + 11};static char _tmp21B[12]="tunion_info";
static struct _tuple5 Cyc__gentuple_1090={offsetof(struct Cyc_Absyn_TunionInfo,tunion_info),(
struct _dyneither_ptr){_tmp21B,_tmp21B,_tmp21B + 12},(void*)& Cyc_tunion_Absyn_TunionInfoU_rep};
static char _tmp21C[6]="targs";static struct _tuple5 Cyc__gentuple_1091={offsetof(
struct Cyc_Absyn_TunionInfo,targs),(struct _dyneither_ptr){_tmp21C,_tmp21C,_tmp21C
+ 6},(void*)& Cyc__genrep_121};static char _tmp21D[4]="rgn";static struct _tuple5 Cyc__gentuple_1092={
offsetof(struct Cyc_Absyn_TunionInfo,rgn),(struct _dyneither_ptr){_tmp21D,_tmp21D,
_tmp21D + 4},(void*)& Cyc__genrep_110};static struct _tuple5*Cyc__genarr_1093[3]={&
Cyc__gentuple_1090,& Cyc__gentuple_1091,& Cyc__gentuple_1092};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_info_t_rep={3,(struct _dyneither_ptr*)& Cyc__genname_1094,sizeof(
struct Cyc_Absyn_TunionInfo),{(void*)((struct _tuple5**)Cyc__genarr_1093),(void*)((
struct _tuple5**)Cyc__genarr_1093),(void*)((struct _tuple5**)Cyc__genarr_1093 + 3)}};
struct _tuple21{unsigned int f1;struct Cyc_Absyn_TunionInfo f2;};static struct _tuple6
Cyc__gentuple_1095={offsetof(struct _tuple21,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_1096={offsetof(struct _tuple21,f2),(void*)& Cyc_Absyn_tunion_info_t_rep};
static struct _tuple6*Cyc__genarr_1097[2]={& Cyc__gentuple_1095,& Cyc__gentuple_1096};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1071={4,sizeof(struct _tuple21),{(
void*)((struct _tuple6**)Cyc__genarr_1097),(void*)((struct _tuple6**)Cyc__genarr_1097),(
void*)((struct _tuple6**)Cyc__genarr_1097 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1045;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_field_info_t_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1052;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionFieldInfo_rep;static char _tmp220[23]="UnknownTunionFieldInfo";
static struct _dyneither_ptr Cyc__genname_1057=(struct _dyneither_ptr){_tmp220,
_tmp220,_tmp220 + 23};static char _tmp221[12]="tunion_name";static struct _tuple5 Cyc__gentuple_1053={
offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,tunion_name),(struct
_dyneither_ptr){_tmp221,_tmp221,_tmp221 + 12},(void*)& Cyc__genrep_11};static char
_tmp222[11]="field_name";static struct _tuple5 Cyc__gentuple_1054={offsetof(struct
Cyc_Absyn_UnknownTunionFieldInfo,field_name),(struct _dyneither_ptr){_tmp222,
_tmp222,_tmp222 + 11},(void*)& Cyc__genrep_11};static char _tmp223[11]="is_xtunion";
static struct _tuple5 Cyc__gentuple_1055={offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,is_xtunion),(
struct _dyneither_ptr){_tmp223,_tmp223,_tmp223 + 11},(void*)& Cyc__genrep_10};
static struct _tuple5*Cyc__genarr_1056[3]={& Cyc__gentuple_1053,& Cyc__gentuple_1054,&
Cyc__gentuple_1055};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_UnknownTunionFieldInfo_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_1057,sizeof(struct Cyc_Absyn_UnknownTunionFieldInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1056),(void*)((struct _tuple5**)Cyc__genarr_1056),(
void*)((struct _tuple5**)Cyc__genarr_1056 + 3)}};struct _tuple22{unsigned int f1;
struct Cyc_Absyn_UnknownTunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1058={
offsetof(struct _tuple22,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1059={
offsetof(struct _tuple22,f2),(void*)& Cyc_struct_Absyn_UnknownTunionFieldInfo_rep};
static struct _tuple6*Cyc__genarr_1060[2]={& Cyc__gentuple_1058,& Cyc__gentuple_1059};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1052={4,sizeof(struct _tuple22),{(
void*)((struct _tuple6**)Cyc__genarr_1060),(void*)((struct _tuple6**)Cyc__genarr_1060),(
void*)((struct _tuple6**)Cyc__genarr_1060 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1047;struct _tuple23{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;
struct Cyc_Absyn_Tunionfield*f3;};static struct _tuple6 Cyc__gentuple_1048={
offsetof(struct _tuple23,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1049={
offsetof(struct _tuple23,f2),(void*)((void*)& Cyc__genrep_263)};static struct
_tuple6 Cyc__gentuple_1050={offsetof(struct _tuple23,f3),(void*)& Cyc__genrep_246};
static struct _tuple6*Cyc__genarr_1051[3]={& Cyc__gentuple_1048,& Cyc__gentuple_1049,&
Cyc__gentuple_1050};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1047={4,
sizeof(struct _tuple23),{(void*)((struct _tuple6**)Cyc__genarr_1051),(void*)((
struct _tuple6**)Cyc__genarr_1051),(void*)((struct _tuple6**)Cyc__genarr_1051 + 3)}};
static struct _tuple7*Cyc__genarr_1046[0]={};static char _tmp227[19]="UnknownTunionfield";
static struct _tuple5 Cyc__gentuple_1061={0,(struct _dyneither_ptr){_tmp227,_tmp227,
_tmp227 + 19},(void*)& Cyc__genrep_1052};static char _tmp228[17]="KnownTunionfield";
static struct _tuple5 Cyc__gentuple_1062={1,(struct _dyneither_ptr){_tmp228,_tmp228,
_tmp228 + 17},(void*)& Cyc__genrep_1047};static struct _tuple5*Cyc__genarr_1063[2]={&
Cyc__gentuple_1061,& Cyc__gentuple_1062};static char _tmp22A[17]="TunionFieldInfoU";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep={5,(struct
_dyneither_ptr){_tmp22A,_tmp22A,_tmp22A + 17},{(void*)((struct _tuple7**)Cyc__genarr_1046),(
void*)((struct _tuple7**)Cyc__genarr_1046),(void*)((struct _tuple7**)Cyc__genarr_1046
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1063),(void*)((struct _tuple5**)Cyc__genarr_1063),(
void*)((struct _tuple5**)Cyc__genarr_1063 + 2)}};static char _tmp22B[16]="TunionFieldInfo";
static struct _dyneither_ptr Cyc__genname_1067=(struct _dyneither_ptr){_tmp22B,
_tmp22B,_tmp22B + 16};static char _tmp22C[11]="field_info";static struct _tuple5 Cyc__gentuple_1064={
offsetof(struct Cyc_Absyn_TunionFieldInfo,field_info),(struct _dyneither_ptr){
_tmp22C,_tmp22C,_tmp22C + 11},(void*)& Cyc_tunion_Absyn_TunionFieldInfoU_rep};
static char _tmp22D[6]="targs";static struct _tuple5 Cyc__gentuple_1065={offsetof(
struct Cyc_Absyn_TunionFieldInfo,targs),(struct _dyneither_ptr){_tmp22D,_tmp22D,
_tmp22D + 6},(void*)& Cyc__genrep_121};static struct _tuple5*Cyc__genarr_1066[2]={&
Cyc__gentuple_1064,& Cyc__gentuple_1065};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_field_info_t_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_1067,sizeof(struct Cyc_Absyn_TunionFieldInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1066),(void*)((struct _tuple5**)Cyc__genarr_1066),(
void*)((struct _tuple5**)Cyc__genarr_1066 + 2)}};struct _tuple24{unsigned int f1;
struct Cyc_Absyn_TunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1068={
offsetof(struct _tuple24,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1069={
offsetof(struct _tuple24,f2),(void*)& Cyc_Absyn_tunion_field_info_t_rep};static
struct _tuple6*Cyc__genarr_1070[2]={& Cyc__gentuple_1068,& Cyc__gentuple_1069};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1045={4,sizeof(struct _tuple24),{(
void*)((struct _tuple6**)Cyc__genarr_1070),(void*)((struct _tuple6**)Cyc__genarr_1070),(
void*)((struct _tuple6**)Cyc__genarr_1070 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1019;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep;
extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_atts_t_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_984;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0bool2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0bool2_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_85;struct _tuple25{unsigned int f1;
int f2;};static struct _tuple6 Cyc__gentuple_86={offsetof(struct _tuple25,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_87={offsetof(struct _tuple25,f2),(
void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_88[2]={& Cyc__gentuple_86,&
Cyc__gentuple_87};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_85={4,sizeof(
struct _tuple25),{(void*)((struct _tuple6**)Cyc__genarr_88),(void*)((struct _tuple6**)
Cyc__genarr_88),(void*)((struct _tuple6**)Cyc__genarr_88 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_987;struct _tuple26{unsigned int f1;struct Cyc_Absyn_Conref*f2;};static
struct _tuple6 Cyc__gentuple_988={offsetof(struct _tuple26,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_989={offsetof(struct _tuple26,f2),(void*)& Cyc__genrep_984};
static struct _tuple6*Cyc__genarr_990[2]={& Cyc__gentuple_988,& Cyc__gentuple_989};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_987={4,sizeof(struct _tuple26),{(
void*)((struct _tuple6**)Cyc__genarr_990),(void*)((struct _tuple6**)Cyc__genarr_990),(
void*)((struct _tuple6**)Cyc__genarr_990 + 2)}};static char _tmp232[10]="No_constr";
static struct _tuple7 Cyc__gentuple_985={0,(struct _dyneither_ptr){_tmp232,_tmp232,
_tmp232 + 10}};static struct _tuple7*Cyc__genarr_986[1]={& Cyc__gentuple_985};static
char _tmp233[10]="Eq_constr";static struct _tuple5 Cyc__gentuple_991={0,(struct
_dyneither_ptr){_tmp233,_tmp233,_tmp233 + 10},(void*)& Cyc__genrep_85};static char
_tmp234[15]="Forward_constr";static struct _tuple5 Cyc__gentuple_992={1,(struct
_dyneither_ptr){_tmp234,_tmp234,_tmp234 + 15},(void*)& Cyc__genrep_987};static
struct _tuple5*Cyc__genarr_993[2]={& Cyc__gentuple_991,& Cyc__gentuple_992};static
char _tmp236[11]="Constraint";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0bool2_rep={
5,(struct _dyneither_ptr){_tmp236,_tmp236,_tmp236 + 11},{(void*)((struct _tuple7**)
Cyc__genarr_986),(void*)((struct _tuple7**)Cyc__genarr_986),(void*)((struct
_tuple7**)Cyc__genarr_986 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_993),(void*)((
struct _tuple5**)Cyc__genarr_993),(void*)((struct _tuple5**)Cyc__genarr_993 + 2)}};
static char _tmp237[7]="Conref";static struct _dyneither_ptr Cyc__genname_996=(struct
_dyneither_ptr){_tmp237,_tmp237,_tmp237 + 7};static char _tmp238[2]="v";static
struct _tuple5 Cyc__gentuple_994={offsetof(struct Cyc_Absyn_Conref,v),(struct
_dyneither_ptr){_tmp238,_tmp238,_tmp238 + 2},(void*)& Cyc_tunion_Absyn_Constraint0bool2_rep};
static struct _tuple5*Cyc__genarr_995[1]={& Cyc__gentuple_994};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0bool2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_996,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple5**)Cyc__genarr_995),(void*)((
struct _tuple5**)Cyc__genarr_995),(void*)((struct _tuple5**)Cyc__genarr_995 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_984={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0bool2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1026;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep;static char _tmp23B[7]="Conref";static
struct _dyneither_ptr Cyc__genname_1029=(struct _dyneither_ptr){_tmp23B,_tmp23B,
_tmp23B + 7};static char _tmp23C[2]="v";static struct _tuple5 Cyc__gentuple_1027={
offsetof(struct Cyc_Absyn_Conref,v),(struct _dyneither_ptr){_tmp23C,_tmp23C,
_tmp23C + 2},(void*)& Cyc_tunion_Absyn_Constraint0bool2_rep};static struct _tuple5*
Cyc__genarr_1028[1]={& Cyc__gentuple_1027};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_1029,sizeof(struct Cyc_Absyn_Conref),{(
void*)((struct _tuple5**)Cyc__genarr_1028),(void*)((struct _tuple5**)Cyc__genarr_1028),(
void*)((struct _tuple5**)Cyc__genarr_1028 + 1)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1026={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1020;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_PtrLoc_rep;static char _tmp23F[7]="PtrLoc";static struct
_dyneither_ptr Cyc__genname_1025=(struct _dyneither_ptr){_tmp23F,_tmp23F,_tmp23F + 
7};static char _tmp240[8]="ptr_loc";static struct _tuple5 Cyc__gentuple_1021={
offsetof(struct Cyc_Absyn_PtrLoc,ptr_loc),(struct _dyneither_ptr){_tmp240,_tmp240,
_tmp240 + 8},(void*)& Cyc__genrep_2};static char _tmp241[8]="rgn_loc";static struct
_tuple5 Cyc__gentuple_1022={offsetof(struct Cyc_Absyn_PtrLoc,rgn_loc),(struct
_dyneither_ptr){_tmp241,_tmp241,_tmp241 + 8},(void*)& Cyc__genrep_2};static char
_tmp242[7]="zt_loc";static struct _tuple5 Cyc__gentuple_1023={offsetof(struct Cyc_Absyn_PtrLoc,zt_loc),(
struct _dyneither_ptr){_tmp242,_tmp242,_tmp242 + 7},(void*)& Cyc__genrep_2};static
struct _tuple5*Cyc__genarr_1024[3]={& Cyc__gentuple_1021,& Cyc__gentuple_1022,& Cyc__gentuple_1023};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_PtrLoc_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_1025,sizeof(struct Cyc_Absyn_PtrLoc),{(void*)((
struct _tuple5**)Cyc__genarr_1024),(void*)((struct _tuple5**)Cyc__genarr_1024),(
void*)((struct _tuple5**)Cyc__genarr_1024 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1020={1,1,(void*)((void*)& Cyc_struct_Absyn_PtrLoc_rep)};static char
_tmp245[8]="PtrAtts";static struct _dyneither_ptr Cyc__genname_1036=(struct
_dyneither_ptr){_tmp245,_tmp245,_tmp245 + 8};static char _tmp246[4]="rgn";static
struct _tuple5 Cyc__gentuple_1030={offsetof(struct Cyc_Absyn_PtrAtts,rgn),(struct
_dyneither_ptr){_tmp246,_tmp246,_tmp246 + 4},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp247[9]="nullable";static struct _tuple5 Cyc__gentuple_1031={offsetof(
struct Cyc_Absyn_PtrAtts,nullable),(struct _dyneither_ptr){_tmp247,_tmp247,_tmp247
+ 9},(void*)& Cyc__genrep_984};static char _tmp248[7]="bounds";static struct _tuple5
Cyc__gentuple_1032={offsetof(struct Cyc_Absyn_PtrAtts,bounds),(struct
_dyneither_ptr){_tmp248,_tmp248,_tmp248 + 7},(void*)& Cyc__genrep_1026};static char
_tmp249[10]="zero_term";static struct _tuple5 Cyc__gentuple_1033={offsetof(struct
Cyc_Absyn_PtrAtts,zero_term),(struct _dyneither_ptr){_tmp249,_tmp249,_tmp249 + 10},(
void*)& Cyc__genrep_984};static char _tmp24A[7]="ptrloc";static struct _tuple5 Cyc__gentuple_1034={
offsetof(struct Cyc_Absyn_PtrAtts,ptrloc),(struct _dyneither_ptr){_tmp24A,_tmp24A,
_tmp24A + 7},(void*)& Cyc__genrep_1020};static struct _tuple5*Cyc__genarr_1035[5]={&
Cyc__gentuple_1030,& Cyc__gentuple_1031,& Cyc__gentuple_1032,& Cyc__gentuple_1033,&
Cyc__gentuple_1034};struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_atts_t_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_1036,sizeof(struct Cyc_Absyn_PtrAtts),{(void*)((
struct _tuple5**)Cyc__genarr_1035),(void*)((struct _tuple5**)Cyc__genarr_1035),(
void*)((struct _tuple5**)Cyc__genarr_1035 + 5)}};static char _tmp24C[8]="PtrInfo";
static struct _dyneither_ptr Cyc__genname_1041=(struct _dyneither_ptr){_tmp24C,
_tmp24C,_tmp24C + 8};static char _tmp24D[8]="elt_typ";static struct _tuple5 Cyc__gentuple_1037={
offsetof(struct Cyc_Absyn_PtrInfo,elt_typ),(struct _dyneither_ptr){_tmp24D,_tmp24D,
_tmp24D + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp24E[7]="elt_tq";
static struct _tuple5 Cyc__gentuple_1038={offsetof(struct Cyc_Absyn_PtrInfo,elt_tq),(
struct _dyneither_ptr){_tmp24E,_tmp24E,_tmp24E + 7},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp24F[9]="ptr_atts";static struct _tuple5 Cyc__gentuple_1039={offsetof(
struct Cyc_Absyn_PtrInfo,ptr_atts),(struct _dyneither_ptr){_tmp24F,_tmp24F,_tmp24F
+ 9},(void*)& Cyc_Absyn_ptr_atts_t_rep};static struct _tuple5*Cyc__genarr_1040[3]={&
Cyc__gentuple_1037,& Cyc__gentuple_1038,& Cyc__gentuple_1039};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_ptr_info_t_rep={3,(struct _dyneither_ptr*)& Cyc__genname_1041,sizeof(
struct Cyc_Absyn_PtrInfo),{(void*)((struct _tuple5**)Cyc__genarr_1040),(void*)((
struct _tuple5**)Cyc__genarr_1040),(void*)((struct _tuple5**)Cyc__genarr_1040 + 3)}};
struct _tuple27{unsigned int f1;struct Cyc_Absyn_PtrInfo f2;};static struct _tuple6 Cyc__gentuple_1042={
offsetof(struct _tuple27,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1043={
offsetof(struct _tuple27,f2),(void*)& Cyc_Absyn_ptr_info_t_rep};static struct
_tuple6*Cyc__genarr_1044[2]={& Cyc__gentuple_1042,& Cyc__gentuple_1043};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1019={4,sizeof(struct _tuple27),{(void*)((
struct _tuple6**)Cyc__genarr_1044),(void*)((struct _tuple6**)Cyc__genarr_1044),(
void*)((struct _tuple6**)Cyc__genarr_1044 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1007;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep;
static char _tmp252[7]="Signed";static struct _tuple7 Cyc__gentuple_236={0,(struct
_dyneither_ptr){_tmp252,_tmp252,_tmp252 + 7}};static char _tmp253[9]="Unsigned";
static struct _tuple7 Cyc__gentuple_237={1,(struct _dyneither_ptr){_tmp253,_tmp253,
_tmp253 + 9}};static char _tmp254[5]="None";static struct _tuple7 Cyc__gentuple_238={2,(
struct _dyneither_ptr){_tmp254,_tmp254,_tmp254 + 5}};static struct _tuple7*Cyc__genarr_239[
3]={& Cyc__gentuple_236,& Cyc__gentuple_237,& Cyc__gentuple_238};static struct
_tuple5*Cyc__genarr_240[0]={};static char _tmp256[5]="Sign";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_sign_t_rep={5,(struct _dyneither_ptr){_tmp256,_tmp256,_tmp256 + 5},{(
void*)((struct _tuple7**)Cyc__genarr_239),(void*)((struct _tuple7**)Cyc__genarr_239),(
void*)((struct _tuple7**)Cyc__genarr_239 + 3)},{(void*)((struct _tuple5**)Cyc__genarr_240),(
void*)((struct _tuple5**)Cyc__genarr_240),(void*)((struct _tuple5**)Cyc__genarr_240
+ 0)}};extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep;static char
_tmp257[8]="Char_sz";static struct _tuple7 Cyc__gentuple_1008={0,(struct
_dyneither_ptr){_tmp257,_tmp257,_tmp257 + 8}};static char _tmp258[9]="Short_sz";
static struct _tuple7 Cyc__gentuple_1009={1,(struct _dyneither_ptr){_tmp258,_tmp258,
_tmp258 + 9}};static char _tmp259[7]="Int_sz";static struct _tuple7 Cyc__gentuple_1010={
2,(struct _dyneither_ptr){_tmp259,_tmp259,_tmp259 + 7}};static char _tmp25A[8]="Long_sz";
static struct _tuple7 Cyc__gentuple_1011={3,(struct _dyneither_ptr){_tmp25A,_tmp25A,
_tmp25A + 8}};static char _tmp25B[12]="LongLong_sz";static struct _tuple7 Cyc__gentuple_1012={
4,(struct _dyneither_ptr){_tmp25B,_tmp25B,_tmp25B + 12}};static struct _tuple7*Cyc__genarr_1013[
5]={& Cyc__gentuple_1008,& Cyc__gentuple_1009,& Cyc__gentuple_1010,& Cyc__gentuple_1011,&
Cyc__gentuple_1012};static struct _tuple5*Cyc__genarr_1014[0]={};static char _tmp25D[
8]="Size_of";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep={5,(struct
_dyneither_ptr){_tmp25D,_tmp25D,_tmp25D + 8},{(void*)((struct _tuple7**)Cyc__genarr_1013),(
void*)((struct _tuple7**)Cyc__genarr_1013),(void*)((struct _tuple7**)Cyc__genarr_1013
+ 5)},{(void*)((struct _tuple5**)Cyc__genarr_1014),(void*)((struct _tuple5**)Cyc__genarr_1014),(
void*)((struct _tuple5**)Cyc__genarr_1014 + 0)}};struct _tuple28{unsigned int f1;
void*f2;void*f3;};static struct _tuple6 Cyc__gentuple_1015={offsetof(struct _tuple28,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1016={offsetof(struct
_tuple28,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_1017={
offsetof(struct _tuple28,f3),(void*)& Cyc_Absyn_size_of_t_rep};static struct _tuple6*
Cyc__genarr_1018[3]={& Cyc__gentuple_1015,& Cyc__gentuple_1016,& Cyc__gentuple_1017};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1007={4,sizeof(struct _tuple28),{(
void*)((struct _tuple6**)Cyc__genarr_1018),(void*)((struct _tuple6**)Cyc__genarr_1018),(
void*)((struct _tuple6**)Cyc__genarr_1018 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_983;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_array_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_145;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Exp_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_exp_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_849;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_cnst_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_864;struct
_tuple29{unsigned int f1;void*f2;char f3;};static struct _tuple6 Cyc__gentuple_865={
offsetof(struct _tuple29,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_866={
offsetof(struct _tuple29,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_867={
offsetof(struct _tuple29,f3),(void*)((void*)& Cyc__genrep_15)};static struct _tuple6*
Cyc__genarr_868[3]={& Cyc__gentuple_865,& Cyc__gentuple_866,& Cyc__gentuple_867};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_864={4,sizeof(struct _tuple29),{(
void*)((struct _tuple6**)Cyc__genarr_868),(void*)((struct _tuple6**)Cyc__genarr_868),(
void*)((struct _tuple6**)Cyc__genarr_868 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_858;static struct Cyc_Typerep_Int_struct Cyc__genrep_859={0,1,16};
struct _tuple30{unsigned int f1;void*f2;short f3;};static struct _tuple6 Cyc__gentuple_860={
offsetof(struct _tuple30,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_861={
offsetof(struct _tuple30,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_862={
offsetof(struct _tuple30,f3),(void*)& Cyc__genrep_859};static struct _tuple6*Cyc__genarr_863[
3]={& Cyc__gentuple_860,& Cyc__gentuple_861,& Cyc__gentuple_862};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_858={4,sizeof(struct _tuple30),{(void*)((struct _tuple6**)Cyc__genarr_863),(
void*)((struct _tuple6**)Cyc__genarr_863),(void*)((struct _tuple6**)Cyc__genarr_863
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_235;struct _tuple31{
unsigned int f1;void*f2;int f3;};static struct _tuple6 Cyc__gentuple_241={offsetof(
struct _tuple31,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_242={
offsetof(struct _tuple31,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_243={
offsetof(struct _tuple31,f3),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_244[
3]={& Cyc__gentuple_241,& Cyc__gentuple_242,& Cyc__gentuple_243};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_235={4,sizeof(struct _tuple31),{(void*)((struct _tuple6**)Cyc__genarr_244),(
void*)((struct _tuple6**)Cyc__genarr_244),(void*)((struct _tuple6**)Cyc__genarr_244
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_852;static struct Cyc_Typerep_Int_struct
Cyc__genrep_853={0,1,64};struct _tuple32{unsigned int f1;void*f2;long long f3;};
static struct _tuple6 Cyc__gentuple_854={offsetof(struct _tuple32,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_855={offsetof(struct _tuple32,f2),(void*)& Cyc_Absyn_sign_t_rep};
static struct _tuple6 Cyc__gentuple_856={offsetof(struct _tuple32,f3),(void*)& Cyc__genrep_853};
static struct _tuple6*Cyc__genarr_857[3]={& Cyc__gentuple_854,& Cyc__gentuple_855,&
Cyc__gentuple_856};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_852={4,
sizeof(struct _tuple32),{(void*)((struct _tuple6**)Cyc__genarr_857),(void*)((
struct _tuple6**)Cyc__genarr_857),(void*)((struct _tuple6**)Cyc__genarr_857 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_81;static struct _tuple6 Cyc__gentuple_82={
offsetof(struct _tuple7,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_83={
offsetof(struct _tuple7,f2),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*
Cyc__genarr_84[2]={& Cyc__gentuple_82,& Cyc__gentuple_83};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_81={4,sizeof(struct _tuple7),{(void*)((struct _tuple6**)Cyc__genarr_84),(
void*)((struct _tuple6**)Cyc__genarr_84),(void*)((struct _tuple6**)Cyc__genarr_84 + 
2)}};static char _tmp266[7]="Null_c";static struct _tuple7 Cyc__gentuple_850={0,(
struct _dyneither_ptr){_tmp266,_tmp266,_tmp266 + 7}};static struct _tuple7*Cyc__genarr_851[
1]={& Cyc__gentuple_850};static char _tmp267[7]="Char_c";static struct _tuple5 Cyc__gentuple_869={
0,(struct _dyneither_ptr){_tmp267,_tmp267,_tmp267 + 7},(void*)& Cyc__genrep_864};
static char _tmp268[8]="Short_c";static struct _tuple5 Cyc__gentuple_870={1,(struct
_dyneither_ptr){_tmp268,_tmp268,_tmp268 + 8},(void*)& Cyc__genrep_858};static char
_tmp269[6]="Int_c";static struct _tuple5 Cyc__gentuple_871={2,(struct _dyneither_ptr){
_tmp269,_tmp269,_tmp269 + 6},(void*)& Cyc__genrep_235};static char _tmp26A[11]="LongLong_c";
static struct _tuple5 Cyc__gentuple_872={3,(struct _dyneither_ptr){_tmp26A,_tmp26A,
_tmp26A + 11},(void*)& Cyc__genrep_852};static char _tmp26B[8]="Float_c";static
struct _tuple5 Cyc__gentuple_873={4,(struct _dyneither_ptr){_tmp26B,_tmp26B,_tmp26B
+ 8},(void*)& Cyc__genrep_81};static char _tmp26C[9]="String_c";static struct _tuple5
Cyc__gentuple_874={5,(struct _dyneither_ptr){_tmp26C,_tmp26C,_tmp26C + 9},(void*)&
Cyc__genrep_81};static struct _tuple5*Cyc__genarr_875[6]={& Cyc__gentuple_869,& Cyc__gentuple_870,&
Cyc__gentuple_871,& Cyc__gentuple_872,& Cyc__gentuple_873,& Cyc__gentuple_874};
static char _tmp26E[5]="Cnst";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_cnst_t_rep={
5,(struct _dyneither_ptr){_tmp26E,_tmp26E,_tmp26E + 5},{(void*)((struct _tuple7**)
Cyc__genarr_851),(void*)((struct _tuple7**)Cyc__genarr_851),(void*)((struct
_tuple7**)Cyc__genarr_851 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_875),(void*)((
struct _tuple5**)Cyc__genarr_875),(void*)((struct _tuple5**)Cyc__genarr_875 + 6)}};
struct _tuple33{unsigned int f1;union Cyc_Absyn_Cnst_union f2;};static struct _tuple6
Cyc__gentuple_876={offsetof(struct _tuple33,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_877={offsetof(struct _tuple33,f2),(void*)& Cyc_Absyn_cnst_t_rep};
static struct _tuple6*Cyc__genarr_878[2]={& Cyc__gentuple_876,& Cyc__gentuple_877};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_849={4,sizeof(struct _tuple33),{(
void*)((struct _tuple6**)Cyc__genarr_878),(void*)((struct _tuple6**)Cyc__genarr_878),(
void*)((struct _tuple6**)Cyc__genarr_878 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_797;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_binding_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_804;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_805;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_808;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_809;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_810;struct _tuple34{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};static struct _tuple6 Cyc__gentuple_811={offsetof(struct _tuple34,f1),(
void*)& Cyc__genrep_13};static struct _tuple6 Cyc__gentuple_812={offsetof(struct
_tuple34,f2),(void*)& Cyc_Absyn_tqual_t_rep};static struct _tuple6 Cyc__gentuple_813={
offsetof(struct _tuple34,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_814[3]={& Cyc__gentuple_811,& Cyc__gentuple_812,& Cyc__gentuple_813};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_810={4,sizeof(struct _tuple34),{(
void*)((struct _tuple6**)Cyc__genarr_814),(void*)((struct _tuple6**)Cyc__genarr_814),(
void*)((struct _tuple6**)Cyc__genarr_814 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_809={1,1,(void*)((void*)& Cyc__genrep_810)};static char _tmp272[5]="List";
static struct _dyneither_ptr Cyc__genname_818=(struct _dyneither_ptr){_tmp272,
_tmp272,_tmp272 + 5};static char _tmp273[3]="hd";static struct _tuple5 Cyc__gentuple_815={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp273,_tmp273,_tmp273 + 
3},(void*)& Cyc__genrep_809};static char _tmp274[3]="tl";static struct _tuple5 Cyc__gentuple_816={
offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){_tmp274,_tmp274,_tmp274 + 
3},(void*)& Cyc__genrep_808};static struct _tuple5*Cyc__genarr_817[2]={& Cyc__gentuple_815,&
Cyc__gentuple_816};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_818,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_817),(void*)((struct _tuple5**)Cyc__genarr_817),(void*)((
struct _tuple5**)Cyc__genarr_817 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_808={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_807;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_657;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep;static
char _tmp277[4]="Opt";static struct _dyneither_ptr Cyc__genname_660=(struct
_dyneither_ptr){_tmp277,_tmp277,_tmp277 + 4};static char _tmp278[2]="v";static
struct _tuple5 Cyc__gentuple_658={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp278,_tmp278,_tmp278 + 2},(void*)& Cyc__genrep_13};static struct
_tuple5*Cyc__genarr_659[1]={& Cyc__gentuple_658};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_var_t2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_660,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_659),(void*)((
struct _tuple5**)Cyc__genarr_659),(void*)((struct _tuple5**)Cyc__genarr_659 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_657={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_var_t2_rep)};
static char _tmp27B[11]="VarargInfo";static struct _dyneither_ptr Cyc__genname_727=(
struct _dyneither_ptr){_tmp27B,_tmp27B,_tmp27B + 11};static char _tmp27C[5]="name";
static struct _tuple5 Cyc__gentuple_722={offsetof(struct Cyc_Absyn_VarargInfo,name),(
struct _dyneither_ptr){_tmp27C,_tmp27C,_tmp27C + 5},(void*)& Cyc__genrep_657};
static char _tmp27D[3]="tq";static struct _tuple5 Cyc__gentuple_723={offsetof(struct
Cyc_Absyn_VarargInfo,tq),(struct _dyneither_ptr){_tmp27D,_tmp27D,_tmp27D + 3},(
void*)& Cyc_Absyn_tqual_t_rep};static char _tmp27E[5]="type";static struct _tuple5 Cyc__gentuple_724={
offsetof(struct Cyc_Absyn_VarargInfo,type),(struct _dyneither_ptr){_tmp27E,_tmp27E,
_tmp27E + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp27F[7]="inject";
static struct _tuple5 Cyc__gentuple_725={offsetof(struct Cyc_Absyn_VarargInfo,inject),(
struct _dyneither_ptr){_tmp27F,_tmp27F,_tmp27F + 7},(void*)& Cyc__genrep_10};static
struct _tuple5*Cyc__genarr_726[4]={& Cyc__gentuple_722,& Cyc__gentuple_723,& Cyc__gentuple_724,&
Cyc__gentuple_725};struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_info_t_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_727,sizeof(struct Cyc_Absyn_VarargInfo),{(
void*)((struct _tuple5**)Cyc__genarr_726),(void*)((struct _tuple5**)Cyc__genarr_726),(
void*)((struct _tuple5**)Cyc__genarr_726 + 4)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_807={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_372;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_373;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_374;static struct
_tuple6 Cyc__gentuple_375={offsetof(struct _tuple9,f1),(void*)((void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple6 Cyc__gentuple_376={offsetof(struct _tuple9,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_377[2]={& Cyc__gentuple_375,&
Cyc__gentuple_376};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_374={4,
sizeof(struct _tuple9),{(void*)((struct _tuple6**)Cyc__genarr_377),(void*)((struct
_tuple6**)Cyc__genarr_377),(void*)((struct _tuple6**)Cyc__genarr_377 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_373={1,1,(void*)((void*)& Cyc__genrep_374)};
static char _tmp284[5]="List";static struct _dyneither_ptr Cyc__genname_381=(struct
_dyneither_ptr){_tmp284,_tmp284,_tmp284 + 5};static char _tmp285[3]="hd";static
struct _tuple5 Cyc__gentuple_378={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp285,_tmp285,_tmp285 + 3},(void*)& Cyc__genrep_373};static char
_tmp286[3]="tl";static struct _tuple5 Cyc__gentuple_379={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp286,_tmp286,_tmp286 + 3},(void*)& Cyc__genrep_372};
static struct _tuple5*Cyc__genarr_380[2]={& Cyc__gentuple_378,& Cyc__gentuple_379};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_381,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_380),(void*)((struct _tuple5**)Cyc__genarr_380),(void*)((
struct _tuple5**)Cyc__genarr_380 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_372={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_134;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_129;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_130;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_130={1,1,(void*)((
void*)& Cyc_struct_Absyn_Exp_rep)};struct _tuple35{unsigned int f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple6 Cyc__gentuple_926={offsetof(struct _tuple35,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_927={offsetof(struct _tuple35,f2),(
void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_928[2]={& Cyc__gentuple_926,&
Cyc__gentuple_927};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_129={4,
sizeof(struct _tuple35),{(void*)((struct _tuple6**)Cyc__genarr_928),(void*)((
struct _tuple6**)Cyc__genarr_928),(void*)((struct _tuple6**)Cyc__genarr_928 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_542;struct _tuple36{unsigned int
f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_543={
offsetof(struct _tuple36,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_544={
offsetof(struct _tuple36,f2),(void*)& Cyc__genrep_134};static struct _tuple6 Cyc__gentuple_545={
offsetof(struct _tuple36,f3),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_546[
3]={& Cyc__gentuple_543,& Cyc__gentuple_544,& Cyc__gentuple_545};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_542={4,sizeof(struct _tuple36),{(void*)((struct _tuple6**)Cyc__genarr_546),(
void*)((struct _tuple6**)Cyc__genarr_546),(void*)((struct _tuple6**)Cyc__genarr_546
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_538;static struct _tuple6
Cyc__gentuple_539={offsetof(struct _tuple35,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_540={offsetof(struct _tuple35,f2),(void*)& Cyc__genrep_145};
static struct _tuple6*Cyc__genarr_541[2]={& Cyc__gentuple_539,& Cyc__gentuple_540};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_538={4,sizeof(struct _tuple35),{(
void*)((struct _tuple6**)Cyc__genarr_541),(void*)((struct _tuple6**)Cyc__genarr_541),(
void*)((struct _tuple6**)Cyc__genarr_541 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_532;struct _tuple37{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Stmt*
f3;struct Cyc_Absyn_Stmt*f4;};static struct _tuple6 Cyc__gentuple_533={offsetof(
struct _tuple37,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_534={
offsetof(struct _tuple37,f2),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_535={
offsetof(struct _tuple37,f3),(void*)& Cyc__genrep_134};static struct _tuple6 Cyc__gentuple_536={
offsetof(struct _tuple37,f4),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_537[
4]={& Cyc__gentuple_533,& Cyc__gentuple_534,& Cyc__gentuple_535,& Cyc__gentuple_536};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_532={4,sizeof(struct _tuple37),{(
void*)((struct _tuple6**)Cyc__genarr_537),(void*)((struct _tuple6**)Cyc__genarr_537),(
void*)((struct _tuple6**)Cyc__genarr_537 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_527;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_476;static struct
_tuple6 Cyc__gentuple_477={offsetof(struct _tuple2,f1),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_478={offsetof(struct _tuple2,f2),(void*)& Cyc__genrep_134};
static struct _tuple6*Cyc__genarr_479[2]={& Cyc__gentuple_477,& Cyc__gentuple_478};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_476={4,sizeof(struct _tuple2),{(
void*)((struct _tuple6**)Cyc__genarr_479),(void*)((struct _tuple6**)Cyc__genarr_479),(
void*)((struct _tuple6**)Cyc__genarr_479 + 2)}};struct _tuple38{unsigned int f1;
struct _tuple2 f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_528={
offsetof(struct _tuple38,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_529={
offsetof(struct _tuple38,f2),(void*)& Cyc__genrep_476};static struct _tuple6 Cyc__gentuple_530={
offsetof(struct _tuple38,f3),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_531[
3]={& Cyc__gentuple_528,& Cyc__gentuple_529,& Cyc__gentuple_530};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_527={4,sizeof(struct _tuple38),{(void*)((struct _tuple6**)Cyc__genarr_531),(
void*)((struct _tuple6**)Cyc__genarr_531),(void*)((struct _tuple6**)Cyc__genarr_531
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_523;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_518;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_518={1,1,(void*)((
void*)& Cyc_struct_Absyn_Stmt_rep)};struct _tuple39{unsigned int f1;struct Cyc_Absyn_Stmt*
f2;};static struct _tuple6 Cyc__gentuple_524={offsetof(struct _tuple39,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_525={offsetof(struct _tuple39,f2),(
void*)& Cyc__genrep_518};static struct _tuple6*Cyc__genarr_526[2]={& Cyc__gentuple_524,&
Cyc__gentuple_525};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_523={4,
sizeof(struct _tuple39),{(void*)((struct _tuple6**)Cyc__genarr_526),(void*)((
struct _tuple6**)Cyc__genarr_526),(void*)((struct _tuple6**)Cyc__genarr_526 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_517;struct _tuple40{unsigned int
f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_519={
offsetof(struct _tuple40,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_520={
offsetof(struct _tuple40,f2),(void*)& Cyc__genrep_13};static struct _tuple6 Cyc__gentuple_521={
offsetof(struct _tuple40,f3),(void*)& Cyc__genrep_518};static struct _tuple6*Cyc__genarr_522[
3]={& Cyc__gentuple_519,& Cyc__gentuple_520,& Cyc__gentuple_521};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_517={4,sizeof(struct _tuple40),{(void*)((struct _tuple6**)Cyc__genarr_522),(
void*)((struct _tuple6**)Cyc__genarr_522),(void*)((struct _tuple6**)Cyc__genarr_522
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_510;struct _tuple41{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct _tuple2 f3;struct _tuple2 f4;struct Cyc_Absyn_Stmt*
f5;};static struct _tuple6 Cyc__gentuple_511={offsetof(struct _tuple41,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_512={offsetof(struct _tuple41,f2),(
void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_513={offsetof(struct
_tuple41,f3),(void*)& Cyc__genrep_476};static struct _tuple6 Cyc__gentuple_514={
offsetof(struct _tuple41,f4),(void*)& Cyc__genrep_476};static struct _tuple6 Cyc__gentuple_515={
offsetof(struct _tuple41,f5),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_516[
5]={& Cyc__gentuple_511,& Cyc__gentuple_512,& Cyc__gentuple_513,& Cyc__gentuple_514,&
Cyc__gentuple_515};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_510={4,
sizeof(struct _tuple41),{(void*)((struct _tuple6**)Cyc__genarr_516),(void*)((
struct _tuple6**)Cyc__genarr_516),(void*)((struct _tuple6**)Cyc__genarr_516 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_505;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_143;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_144;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Switch_clause_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_181;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_434;
struct _tuple42{unsigned int f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Pat*f3;
};static struct _tuple6 Cyc__gentuple_435={offsetof(struct _tuple42,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_436={offsetof(struct _tuple42,f2),(void*)& Cyc__genrep_148};
static struct _tuple6 Cyc__gentuple_437={offsetof(struct _tuple42,f3),(void*)& Cyc__genrep_181};
static struct _tuple6*Cyc__genarr_438[3]={& Cyc__gentuple_435,& Cyc__gentuple_436,&
Cyc__gentuple_437};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_434={4,
sizeof(struct _tuple42),{(void*)((struct _tuple6**)Cyc__genarr_438),(void*)((
struct _tuple6**)Cyc__genarr_438),(void*)((struct _tuple6**)Cyc__genarr_438 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_429;struct _tuple43{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};static struct _tuple6 Cyc__gentuple_430={
offsetof(struct _tuple43,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_431={
offsetof(struct _tuple43,f2),(void*)& Cyc__genrep_274};static struct _tuple6 Cyc__gentuple_432={
offsetof(struct _tuple43,f3),(void*)& Cyc__genrep_148};static struct _tuple6*Cyc__genarr_433[
3]={& Cyc__gentuple_430,& Cyc__gentuple_431,& Cyc__gentuple_432};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_429={4,sizeof(struct _tuple43),{(void*)((struct _tuple6**)Cyc__genarr_433),(
void*)((struct _tuple6**)Cyc__genarr_433),(void*)((struct _tuple6**)Cyc__genarr_433
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_424;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_186;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep;
static char _tmp296[5]="List";static struct _dyneither_ptr Cyc__genname_190=(struct
_dyneither_ptr){_tmp296,_tmp296,_tmp296 + 5};static char _tmp297[3]="hd";static
struct _tuple5 Cyc__gentuple_187={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp297,_tmp297,_tmp297 + 3},(void*)& Cyc__genrep_181};static char
_tmp298[3]="tl";static struct _tuple5 Cyc__gentuple_188={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp298,_tmp298,_tmp298 + 3},(void*)& Cyc__genrep_186};
static struct _tuple5*Cyc__genarr_189[2]={& Cyc__gentuple_187,& Cyc__gentuple_188};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_190,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_189),(void*)((struct _tuple5**)Cyc__genarr_189),(void*)((
struct _tuple5**)Cyc__genarr_189 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_186={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_pat_t46H2_rep)};struct _tuple44{
unsigned int f1;struct Cyc_List_List*f2;int f3;};static struct _tuple6 Cyc__gentuple_425={
offsetof(struct _tuple44,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_426={
offsetof(struct _tuple44,f2),(void*)& Cyc__genrep_186};static struct _tuple6 Cyc__gentuple_427={
offsetof(struct _tuple44,f3),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_428[
3]={& Cyc__gentuple_425,& Cyc__gentuple_426,& Cyc__gentuple_427};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_424={4,sizeof(struct _tuple44),{(void*)((struct _tuple6**)Cyc__genarr_428),(
void*)((struct _tuple6**)Cyc__genarr_428),(void*)((struct _tuple6**)Cyc__genarr_428
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_420;struct _tuple45{
unsigned int f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple6 Cyc__gentuple_421={
offsetof(struct _tuple45,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_422={
offsetof(struct _tuple45,f2),(void*)& Cyc__genrep_181};static struct _tuple6*Cyc__genarr_423[
2]={& Cyc__gentuple_421,& Cyc__gentuple_422};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_420={4,sizeof(struct _tuple45),{(void*)((struct _tuple6**)Cyc__genarr_423),(
void*)((struct _tuple6**)Cyc__genarr_423),(void*)((struct _tuple6**)Cyc__genarr_423
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_330;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_aggr_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_402;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_aggr_kind_t_rep;static char _tmp29D[8]="StructA";static struct _tuple7 Cyc__gentuple_387={
0,(struct _dyneither_ptr){_tmp29D,_tmp29D,_tmp29D + 8}};static char _tmp29E[7]="UnionA";
static struct _tuple7 Cyc__gentuple_388={1,(struct _dyneither_ptr){_tmp29E,_tmp29E,
_tmp29E + 7}};static struct _tuple7*Cyc__genarr_389[2]={& Cyc__gentuple_387,& Cyc__gentuple_388};
static struct _tuple5*Cyc__genarr_390[0]={};static char _tmp2A0[9]="AggrKind";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_aggr_kind_t_rep={5,(struct _dyneither_ptr){
_tmp2A0,_tmp2A0,_tmp2A0 + 9},{(void*)((struct _tuple7**)Cyc__genarr_389),(void*)((
struct _tuple7**)Cyc__genarr_389),(void*)((struct _tuple7**)Cyc__genarr_389 + 2)},{(
void*)((struct _tuple5**)Cyc__genarr_390),(void*)((struct _tuple5**)Cyc__genarr_390),(
void*)((struct _tuple5**)Cyc__genarr_390 + 0)}};struct _tuple46{unsigned int f1;void*
f2;struct _tuple0*f3;};static struct _tuple6 Cyc__gentuple_403={offsetof(struct
_tuple46,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_404={
offsetof(struct _tuple46,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};static struct
_tuple6 Cyc__gentuple_405={offsetof(struct _tuple46,f3),(void*)& Cyc__genrep_11};
static struct _tuple6*Cyc__genarr_406[3]={& Cyc__gentuple_403,& Cyc__gentuple_404,&
Cyc__gentuple_405};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_402={4,
sizeof(struct _tuple46),{(void*)((struct _tuple6**)Cyc__genarr_406),(void*)((
struct _tuple6**)Cyc__genarr_406),(void*)((struct _tuple6**)Cyc__genarr_406 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_355;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_356;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_357;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_358;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_359;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_360;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_62;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_attribute_t46H2_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_attribute_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_89;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep;static char
_tmp2A2[10]="Printf_ft";static struct _tuple7 Cyc__gentuple_90={0,(struct
_dyneither_ptr){_tmp2A2,_tmp2A2,_tmp2A2 + 10}};static char _tmp2A3[9]="Scanf_ft";
static struct _tuple7 Cyc__gentuple_91={1,(struct _dyneither_ptr){_tmp2A3,_tmp2A3,
_tmp2A3 + 9}};static struct _tuple7*Cyc__genarr_92[2]={& Cyc__gentuple_90,& Cyc__gentuple_91};
static struct _tuple5*Cyc__genarr_93[0]={};static char _tmp2A5[12]="Format_Type";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep={5,(struct
_dyneither_ptr){_tmp2A5,_tmp2A5,_tmp2A5 + 12},{(void*)((struct _tuple7**)Cyc__genarr_92),(
void*)((struct _tuple7**)Cyc__genarr_92),(void*)((struct _tuple7**)Cyc__genarr_92 + 
2)},{(void*)((struct _tuple5**)Cyc__genarr_93),(void*)((struct _tuple5**)Cyc__genarr_93),(
void*)((struct _tuple5**)Cyc__genarr_93 + 0)}};struct _tuple47{unsigned int f1;void*
f2;int f3;int f4;};static struct _tuple6 Cyc__gentuple_94={offsetof(struct _tuple47,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_95={offsetof(struct
_tuple47,f2),(void*)& Cyc_tunion_Absyn_Format_Type_rep};static struct _tuple6 Cyc__gentuple_96={
offsetof(struct _tuple47,f3),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_97={
offsetof(struct _tuple47,f4),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_98[
4]={& Cyc__gentuple_94,& Cyc__gentuple_95,& Cyc__gentuple_96,& Cyc__gentuple_97};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_89={4,sizeof(struct _tuple47),{(
void*)((struct _tuple6**)Cyc__genarr_98),(void*)((struct _tuple6**)Cyc__genarr_98),(
void*)((struct _tuple6**)Cyc__genarr_98 + 4)}};static char _tmp2A7[12]="Stdcall_att";
static struct _tuple7 Cyc__gentuple_63={0,(struct _dyneither_ptr){_tmp2A7,_tmp2A7,
_tmp2A7 + 12}};static char _tmp2A8[10]="Cdecl_att";static struct _tuple7 Cyc__gentuple_64={
1,(struct _dyneither_ptr){_tmp2A8,_tmp2A8,_tmp2A8 + 10}};static char _tmp2A9[13]="Fastcall_att";
static struct _tuple7 Cyc__gentuple_65={2,(struct _dyneither_ptr){_tmp2A9,_tmp2A9,
_tmp2A9 + 13}};static char _tmp2AA[13]="Noreturn_att";static struct _tuple7 Cyc__gentuple_66={
3,(struct _dyneither_ptr){_tmp2AA,_tmp2AA,_tmp2AA + 13}};static char _tmp2AB[10]="Const_att";
static struct _tuple7 Cyc__gentuple_67={4,(struct _dyneither_ptr){_tmp2AB,_tmp2AB,
_tmp2AB + 10}};static char _tmp2AC[11]="Packed_att";static struct _tuple7 Cyc__gentuple_68={
5,(struct _dyneither_ptr){_tmp2AC,_tmp2AC,_tmp2AC + 11}};static char _tmp2AD[13]="Nocommon_att";
static struct _tuple7 Cyc__gentuple_69={6,(struct _dyneither_ptr){_tmp2AD,_tmp2AD,
_tmp2AD + 13}};static char _tmp2AE[11]="Shared_att";static struct _tuple7 Cyc__gentuple_70={
7,(struct _dyneither_ptr){_tmp2AE,_tmp2AE,_tmp2AE + 11}};static char _tmp2AF[11]="Unused_att";
static struct _tuple7 Cyc__gentuple_71={8,(struct _dyneither_ptr){_tmp2AF,_tmp2AF,
_tmp2AF + 11}};static char _tmp2B0[9]="Weak_att";static struct _tuple7 Cyc__gentuple_72={
9,(struct _dyneither_ptr){_tmp2B0,_tmp2B0,_tmp2B0 + 9}};static char _tmp2B1[14]="Dllimport_att";
static struct _tuple7 Cyc__gentuple_73={10,(struct _dyneither_ptr){_tmp2B1,_tmp2B1,
_tmp2B1 + 14}};static char _tmp2B2[14]="Dllexport_att";static struct _tuple7 Cyc__gentuple_74={
11,(struct _dyneither_ptr){_tmp2B2,_tmp2B2,_tmp2B2 + 14}};static char _tmp2B3[27]="No_instrument_function_att";
static struct _tuple7 Cyc__gentuple_75={12,(struct _dyneither_ptr){_tmp2B3,_tmp2B3,
_tmp2B3 + 27}};static char _tmp2B4[16]="Constructor_att";static struct _tuple7 Cyc__gentuple_76={
13,(struct _dyneither_ptr){_tmp2B4,_tmp2B4,_tmp2B4 + 16}};static char _tmp2B5[15]="Destructor_att";
static struct _tuple7 Cyc__gentuple_77={14,(struct _dyneither_ptr){_tmp2B5,_tmp2B5,
_tmp2B5 + 15}};static char _tmp2B6[26]="No_check_memory_usage_att";static struct
_tuple7 Cyc__gentuple_78={15,(struct _dyneither_ptr){_tmp2B6,_tmp2B6,_tmp2B6 + 26}};
static char _tmp2B7[9]="Pure_att";static struct _tuple7 Cyc__gentuple_79={16,(struct
_dyneither_ptr){_tmp2B7,_tmp2B7,_tmp2B7 + 9}};static struct _tuple7*Cyc__genarr_80[
17]={& Cyc__gentuple_63,& Cyc__gentuple_64,& Cyc__gentuple_65,& Cyc__gentuple_66,&
Cyc__gentuple_67,& Cyc__gentuple_68,& Cyc__gentuple_69,& Cyc__gentuple_70,& Cyc__gentuple_71,&
Cyc__gentuple_72,& Cyc__gentuple_73,& Cyc__gentuple_74,& Cyc__gentuple_75,& Cyc__gentuple_76,&
Cyc__gentuple_77,& Cyc__gentuple_78,& Cyc__gentuple_79};static char _tmp2B8[12]="Regparm_att";
static struct _tuple5 Cyc__gentuple_99={0,(struct _dyneither_ptr){_tmp2B8,_tmp2B8,
_tmp2B8 + 12},(void*)& Cyc__genrep_85};static char _tmp2B9[12]="Aligned_att";static
struct _tuple5 Cyc__gentuple_100={1,(struct _dyneither_ptr){_tmp2B9,_tmp2B9,_tmp2B9
+ 12},(void*)& Cyc__genrep_85};static char _tmp2BA[12]="Section_att";static struct
_tuple5 Cyc__gentuple_101={2,(struct _dyneither_ptr){_tmp2BA,_tmp2BA,_tmp2BA + 12},(
void*)& Cyc__genrep_81};static char _tmp2BB[11]="Format_att";static struct _tuple5 Cyc__gentuple_102={
3,(struct _dyneither_ptr){_tmp2BB,_tmp2BB,_tmp2BB + 11},(void*)& Cyc__genrep_89};
static char _tmp2BC[16]="Initializes_att";static struct _tuple5 Cyc__gentuple_103={4,(
struct _dyneither_ptr){_tmp2BC,_tmp2BC,_tmp2BC + 16},(void*)& Cyc__genrep_85};
static char _tmp2BD[9]="Mode_att";static struct _tuple5 Cyc__gentuple_104={5,(struct
_dyneither_ptr){_tmp2BD,_tmp2BD,_tmp2BD + 9},(void*)& Cyc__genrep_81};static struct
_tuple5*Cyc__genarr_105[6]={& Cyc__gentuple_99,& Cyc__gentuple_100,& Cyc__gentuple_101,&
Cyc__gentuple_102,& Cyc__gentuple_103,& Cyc__gentuple_104};static char _tmp2BF[10]="Attribute";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_attribute_t_rep={5,(struct
_dyneither_ptr){_tmp2BF,_tmp2BF,_tmp2BF + 10},{(void*)((struct _tuple7**)Cyc__genarr_80),(
void*)((struct _tuple7**)Cyc__genarr_80),(void*)((struct _tuple7**)Cyc__genarr_80 + 
17)},{(void*)((struct _tuple5**)Cyc__genarr_105),(void*)((struct _tuple5**)Cyc__genarr_105),(
void*)((struct _tuple5**)Cyc__genarr_105 + 6)}};static char _tmp2C0[5]="List";static
struct _dyneither_ptr Cyc__genname_109=(struct _dyneither_ptr){_tmp2C0,_tmp2C0,
_tmp2C0 + 5};static char _tmp2C1[3]="hd";static struct _tuple5 Cyc__gentuple_106={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp2C1,_tmp2C1,_tmp2C1 + 
3},(void*)& Cyc_Absyn_attribute_t_rep};static char _tmp2C2[3]="tl";static struct
_tuple5 Cyc__gentuple_107={offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){
_tmp2C2,_tmp2C2,_tmp2C2 + 3},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_108[
2]={& Cyc__gentuple_106,& Cyc__gentuple_107};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_attribute_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_109,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_108),(void*)((struct _tuple5**)Cyc__genarr_108),(void*)((
struct _tuple5**)Cyc__genarr_108 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_62={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_attribute_t46H2_rep)};static char
_tmp2C5[10]="Aggrfield";static struct _dyneither_ptr Cyc__genname_367=(struct
_dyneither_ptr){_tmp2C5,_tmp2C5,_tmp2C5 + 10};static char _tmp2C6[5]="name";static
struct _tuple5 Cyc__gentuple_361={offsetof(struct Cyc_Absyn_Aggrfield,name),(struct
_dyneither_ptr){_tmp2C6,_tmp2C6,_tmp2C6 + 5},(void*)& Cyc__genrep_13};static char
_tmp2C7[3]="tq";static struct _tuple5 Cyc__gentuple_362={offsetof(struct Cyc_Absyn_Aggrfield,tq),(
struct _dyneither_ptr){_tmp2C7,_tmp2C7,_tmp2C7 + 3},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp2C8[5]="type";static struct _tuple5 Cyc__gentuple_363={offsetof(
struct Cyc_Absyn_Aggrfield,type),(struct _dyneither_ptr){_tmp2C8,_tmp2C8,_tmp2C8 + 
5},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp2C9[6]="width";static
struct _tuple5 Cyc__gentuple_364={offsetof(struct Cyc_Absyn_Aggrfield,width),(
struct _dyneither_ptr){_tmp2C9,_tmp2C9,_tmp2C9 + 6},(void*)& Cyc__genrep_145};
static char _tmp2CA[11]="attributes";static struct _tuple5 Cyc__gentuple_365={
offsetof(struct Cyc_Absyn_Aggrfield,attributes),(struct _dyneither_ptr){_tmp2CA,
_tmp2CA,_tmp2CA + 11},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_366[
5]={& Cyc__gentuple_361,& Cyc__gentuple_362,& Cyc__gentuple_363,& Cyc__gentuple_364,&
Cyc__gentuple_365};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_367,sizeof(struct Cyc_Absyn_Aggrfield),{(
void*)((struct _tuple5**)Cyc__genarr_366),(void*)((struct _tuple5**)Cyc__genarr_366),(
void*)((struct _tuple5**)Cyc__genarr_366 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_360={1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrfield_rep)};static char
_tmp2CD[5]="List";static struct _dyneither_ptr Cyc__genname_371=(struct
_dyneither_ptr){_tmp2CD,_tmp2CD,_tmp2CD + 5};static char _tmp2CE[3]="hd";static
struct _tuple5 Cyc__gentuple_368={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp2CE,_tmp2CE,_tmp2CE + 3},(void*)& Cyc__genrep_360};static char
_tmp2CF[3]="tl";static struct _tuple5 Cyc__gentuple_369={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp2CF,_tmp2CF,_tmp2CF + 3},(void*)& Cyc__genrep_359};
static struct _tuple5*Cyc__genarr_370[2]={& Cyc__gentuple_368,& Cyc__gentuple_369};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_371,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_370),(void*)((struct _tuple5**)Cyc__genarr_370),(void*)((
struct _tuple5**)Cyc__genarr_370 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_359={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep)};static char
_tmp2D2[13]="AggrdeclImpl";static struct _dyneither_ptr Cyc__genname_386=(struct
_dyneither_ptr){_tmp2D2,_tmp2D2,_tmp2D2 + 13};static char _tmp2D3[11]="exist_vars";
static struct _tuple5 Cyc__gentuple_382={offsetof(struct Cyc_Absyn_AggrdeclImpl,exist_vars),(
struct _dyneither_ptr){_tmp2D3,_tmp2D3,_tmp2D3 + 11},(void*)& Cyc__genrep_273};
static char _tmp2D4[7]="rgn_po";static struct _tuple5 Cyc__gentuple_383={offsetof(
struct Cyc_Absyn_AggrdeclImpl,rgn_po),(struct _dyneither_ptr){_tmp2D4,_tmp2D4,
_tmp2D4 + 7},(void*)& Cyc__genrep_372};static char _tmp2D5[7]="fields";static struct
_tuple5 Cyc__gentuple_384={offsetof(struct Cyc_Absyn_AggrdeclImpl,fields),(struct
_dyneither_ptr){_tmp2D5,_tmp2D5,_tmp2D5 + 7},(void*)& Cyc__genrep_359};static
struct _tuple5*Cyc__genarr_385[3]={& Cyc__gentuple_382,& Cyc__gentuple_383,& Cyc__gentuple_384};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_386,sizeof(struct Cyc_Absyn_AggrdeclImpl),{(void*)((
struct _tuple5**)Cyc__genarr_385),(void*)((struct _tuple5**)Cyc__genarr_385),(void*)((
struct _tuple5**)Cyc__genarr_385 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_358={
1,1,(void*)((void*)& Cyc_struct_Absyn_AggrdeclImpl_rep)};static char _tmp2D8[9]="Aggrdecl";
static struct _dyneither_ptr Cyc__genname_398=(struct _dyneither_ptr){_tmp2D8,
_tmp2D8,_tmp2D8 + 9};static char _tmp2D9[5]="kind";static struct _tuple5 Cyc__gentuple_391={
offsetof(struct Cyc_Absyn_Aggrdecl,kind),(struct _dyneither_ptr){_tmp2D9,_tmp2D9,
_tmp2D9 + 5},(void*)& Cyc_Absyn_aggr_kind_t_rep};static char _tmp2DA[3]="sc";static
struct _tuple5 Cyc__gentuple_392={offsetof(struct Cyc_Absyn_Aggrdecl,sc),(struct
_dyneither_ptr){_tmp2DA,_tmp2DA,_tmp2DA + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp2DB[5]="name";static struct _tuple5 Cyc__gentuple_393={offsetof(
struct Cyc_Absyn_Aggrdecl,name),(struct _dyneither_ptr){_tmp2DB,_tmp2DB,_tmp2DB + 5},(
void*)& Cyc__genrep_11};static char _tmp2DC[4]="tvs";static struct _tuple5 Cyc__gentuple_394={
offsetof(struct Cyc_Absyn_Aggrdecl,tvs),(struct _dyneither_ptr){_tmp2DC,_tmp2DC,
_tmp2DC + 4},(void*)& Cyc__genrep_273};static char _tmp2DD[5]="impl";static struct
_tuple5 Cyc__gentuple_395={offsetof(struct Cyc_Absyn_Aggrdecl,impl),(struct
_dyneither_ptr){_tmp2DD,_tmp2DD,_tmp2DD + 5},(void*)& Cyc__genrep_358};static char
_tmp2DE[11]="attributes";static struct _tuple5 Cyc__gentuple_396={offsetof(struct
Cyc_Absyn_Aggrdecl,attributes),(struct _dyneither_ptr){_tmp2DE,_tmp2DE,_tmp2DE + 
11},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_397[6]={& Cyc__gentuple_391,&
Cyc__gentuple_392,& Cyc__gentuple_393,& Cyc__gentuple_394,& Cyc__gentuple_395,& Cyc__gentuple_396};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_398,sizeof(struct Cyc_Absyn_Aggrdecl),{(void*)((
struct _tuple5**)Cyc__genarr_397),(void*)((struct _tuple5**)Cyc__genarr_397),(void*)((
struct _tuple5**)Cyc__genarr_397 + 6)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_357={
1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_356={1,1,(void*)((void*)& Cyc__genrep_357)};struct _tuple48{
unsigned int f1;struct Cyc_Absyn_Aggrdecl**f2;};static struct _tuple6 Cyc__gentuple_399={
offsetof(struct _tuple48,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_400={
offsetof(struct _tuple48,f2),(void*)& Cyc__genrep_356};static struct _tuple6*Cyc__genarr_401[
2]={& Cyc__gentuple_399,& Cyc__gentuple_400};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_355={4,sizeof(struct _tuple48),{(void*)((struct _tuple6**)Cyc__genarr_401),(
void*)((struct _tuple6**)Cyc__genarr_401),(void*)((struct _tuple6**)Cyc__genarr_401
+ 2)}};static struct _tuple7*Cyc__genarr_354[0]={};static char _tmp2E3[12]="UnknownAggr";
static struct _tuple5 Cyc__gentuple_407={0,(struct _dyneither_ptr){_tmp2E3,_tmp2E3,
_tmp2E3 + 12},(void*)& Cyc__genrep_402};static char _tmp2E4[10]="KnownAggr";static
struct _tuple5 Cyc__gentuple_408={1,(struct _dyneither_ptr){_tmp2E4,_tmp2E4,_tmp2E4
+ 10},(void*)& Cyc__genrep_355};static struct _tuple5*Cyc__genarr_409[2]={& Cyc__gentuple_407,&
Cyc__gentuple_408};static char _tmp2E6[10]="AggrInfoU";struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_AggrInfoU_rep={5,(struct _dyneither_ptr){_tmp2E6,_tmp2E6,_tmp2E6
+ 10},{(void*)((struct _tuple7**)Cyc__genarr_354),(void*)((struct _tuple7**)Cyc__genarr_354),(
void*)((struct _tuple7**)Cyc__genarr_354 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_409),(
void*)((struct _tuple5**)Cyc__genarr_409),(void*)((struct _tuple5**)Cyc__genarr_409
+ 2)}};static char _tmp2E7[9]="AggrInfo";static struct _dyneither_ptr Cyc__genname_413=(
struct _dyneither_ptr){_tmp2E7,_tmp2E7,_tmp2E7 + 9};static char _tmp2E8[10]="aggr_info";
static struct _tuple5 Cyc__gentuple_410={offsetof(struct Cyc_Absyn_AggrInfo,aggr_info),(
struct _dyneither_ptr){_tmp2E8,_tmp2E8,_tmp2E8 + 10},(void*)& Cyc_tunion_Absyn_AggrInfoU_rep};
static char _tmp2E9[6]="targs";static struct _tuple5 Cyc__gentuple_411={offsetof(
struct Cyc_Absyn_AggrInfo,targs),(struct _dyneither_ptr){_tmp2E9,_tmp2E9,_tmp2E9 + 
6},(void*)& Cyc__genrep_121};static struct _tuple5*Cyc__genarr_412[2]={& Cyc__gentuple_410,&
Cyc__gentuple_411};struct Cyc_Typerep_Struct_struct Cyc_Absyn_aggr_info_t_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_413,sizeof(struct Cyc_Absyn_AggrInfo),{(void*)((
struct _tuple5**)Cyc__genarr_412),(void*)((struct _tuple5**)Cyc__genarr_412),(void*)((
struct _tuple5**)Cyc__genarr_412 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_331;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_332;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_333;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_334;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_336;struct _tuple49{unsigned int f1;struct _dyneither_ptr*f2;};static
struct _tuple6 Cyc__gentuple_337={offsetof(struct _tuple49,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_338={offsetof(struct _tuple49,f2),(void*)& Cyc__genrep_13};
static struct _tuple6*Cyc__genarr_339[2]={& Cyc__gentuple_337,& Cyc__gentuple_338};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_336={4,sizeof(struct _tuple49),{(
void*)((struct _tuple6**)Cyc__genarr_339),(void*)((struct _tuple6**)Cyc__genarr_339),(
void*)((struct _tuple6**)Cyc__genarr_339 + 2)}};static struct _tuple7*Cyc__genarr_335[
0]={};static char _tmp2EC[13]="ArrayElement";static struct _tuple5 Cyc__gentuple_340={
0,(struct _dyneither_ptr){_tmp2EC,_tmp2EC,_tmp2EC + 13},(void*)& Cyc__genrep_129};
static char _tmp2ED[10]="FieldName";static struct _tuple5 Cyc__gentuple_341={1,(
struct _dyneither_ptr){_tmp2ED,_tmp2ED,_tmp2ED + 10},(void*)& Cyc__genrep_336};
static struct _tuple5*Cyc__genarr_342[2]={& Cyc__gentuple_340,& Cyc__gentuple_341};
static char _tmp2EF[11]="Designator";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep={
5,(struct _dyneither_ptr){_tmp2EF,_tmp2EF,_tmp2EF + 11},{(void*)((struct _tuple7**)
Cyc__genarr_335),(void*)((struct _tuple7**)Cyc__genarr_335),(void*)((struct
_tuple7**)Cyc__genarr_335 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_342),(void*)((
struct _tuple5**)Cyc__genarr_342),(void*)((struct _tuple5**)Cyc__genarr_342 + 2)}};
static char _tmp2F0[5]="List";static struct _dyneither_ptr Cyc__genname_346=(struct
_dyneither_ptr){_tmp2F0,_tmp2F0,_tmp2F0 + 5};static char _tmp2F1[3]="hd";static
struct _tuple5 Cyc__gentuple_343={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp2F1,_tmp2F1,_tmp2F1 + 3},(void*)& Cyc_Absyn_designator_t_rep};
static char _tmp2F2[3]="tl";static struct _tuple5 Cyc__gentuple_344={offsetof(struct
Cyc_List_List,tl),(struct _dyneither_ptr){_tmp2F2,_tmp2F2,_tmp2F2 + 3},(void*)& Cyc__genrep_334};
static struct _tuple5*Cyc__genarr_345[2]={& Cyc__gentuple_343,& Cyc__gentuple_344};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_346,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_345),(void*)((struct _tuple5**)Cyc__genarr_345),(void*)((
struct _tuple5**)Cyc__genarr_345 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_334={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_designator_t46H2_rep)};struct
_tuple50{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple6 Cyc__gentuple_347={
offsetof(struct _tuple50,f1),(void*)& Cyc__genrep_334};static struct _tuple6 Cyc__gentuple_348={
offsetof(struct _tuple50,f2),(void*)& Cyc__genrep_181};static struct _tuple6*Cyc__genarr_349[
2]={& Cyc__gentuple_347,& Cyc__gentuple_348};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_333={4,sizeof(struct _tuple50),{(void*)((struct _tuple6**)Cyc__genarr_349),(
void*)((struct _tuple6**)Cyc__genarr_349),(void*)((struct _tuple6**)Cyc__genarr_349
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_332={1,1,(void*)((void*)&
Cyc__genrep_333)};static char _tmp2F7[5]="List";static struct _dyneither_ptr Cyc__genname_353=(
struct _dyneither_ptr){_tmp2F7,_tmp2F7,_tmp2F7 + 5};static char _tmp2F8[3]="hd";
static struct _tuple5 Cyc__gentuple_350={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp2F8,_tmp2F8,_tmp2F8 + 3},(void*)& Cyc__genrep_332};static char
_tmp2F9[3]="tl";static struct _tuple5 Cyc__gentuple_351={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp2F9,_tmp2F9,_tmp2F9 + 3},(void*)& Cyc__genrep_331};
static struct _tuple5*Cyc__genarr_352[2]={& Cyc__gentuple_350,& Cyc__gentuple_351};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_353,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_352),(void*)((struct _tuple5**)Cyc__genarr_352),(void*)((
struct _tuple5**)Cyc__genarr_352 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_331={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep)};
struct _tuple51{unsigned int f1;struct Cyc_Absyn_AggrInfo f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;int f5;};static struct _tuple6 Cyc__gentuple_414={offsetof(
struct _tuple51,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_415={
offsetof(struct _tuple51,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6 Cyc__gentuple_416={offsetof(struct _tuple51,f3),(void*)& Cyc__genrep_273};
static struct _tuple6 Cyc__gentuple_417={offsetof(struct _tuple51,f4),(void*)& Cyc__genrep_331};
static struct _tuple6 Cyc__gentuple_418={offsetof(struct _tuple51,f5),(void*)& Cyc__genrep_10};
static struct _tuple6*Cyc__genarr_419[5]={& Cyc__gentuple_414,& Cyc__gentuple_415,&
Cyc__gentuple_416,& Cyc__gentuple_417,& Cyc__gentuple_418};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_330={4,sizeof(struct _tuple51),{(void*)((struct _tuple6**)Cyc__genarr_419),(
void*)((struct _tuple6**)Cyc__genarr_419),(void*)((struct _tuple6**)Cyc__genarr_419
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_245;struct _tuple52{
unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*f3;
struct Cyc_List_List*f4;int f5;};static struct _tuple6 Cyc__gentuple_324={offsetof(
struct _tuple52,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_325={
offsetof(struct _tuple52,f2),(void*)((void*)& Cyc__genrep_263)};static struct
_tuple6 Cyc__gentuple_326={offsetof(struct _tuple52,f3),(void*)& Cyc__genrep_246};
static struct _tuple6 Cyc__gentuple_327={offsetof(struct _tuple52,f4),(void*)& Cyc__genrep_186};
static struct _tuple6 Cyc__gentuple_328={offsetof(struct _tuple52,f5),(void*)& Cyc__genrep_10};
static struct _tuple6*Cyc__genarr_329[5]={& Cyc__gentuple_324,& Cyc__gentuple_325,&
Cyc__gentuple_326,& Cyc__gentuple_327,& Cyc__gentuple_328};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_245={4,sizeof(struct _tuple52),{(void*)((struct _tuple6**)Cyc__genarr_329),(
void*)((struct _tuple6**)Cyc__genarr_329),(void*)((struct _tuple6**)Cyc__genarr_329
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_231;struct _tuple53{
unsigned int f1;char f2;};static struct _tuple6 Cyc__gentuple_232={offsetof(struct
_tuple53,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_233={
offsetof(struct _tuple53,f2),(void*)((void*)& Cyc__genrep_15)};static struct _tuple6*
Cyc__genarr_234[2]={& Cyc__gentuple_232,& Cyc__gentuple_233};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_231={4,sizeof(struct _tuple53),{(void*)((struct _tuple6**)Cyc__genarr_234),(
void*)((struct _tuple6**)Cyc__genarr_234),(void*)((struct _tuple6**)Cyc__genarr_234
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_211;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_212;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_213;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_214;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_enumfield_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_201;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Enumfield_rep;static char _tmp2FF[10]="Enumfield";static struct
_dyneither_ptr Cyc__genname_206=(struct _dyneither_ptr){_tmp2FF,_tmp2FF,_tmp2FF + 
10};static char _tmp300[5]="name";static struct _tuple5 Cyc__gentuple_202={offsetof(
struct Cyc_Absyn_Enumfield,name),(struct _dyneither_ptr){_tmp300,_tmp300,_tmp300 + 
5},(void*)& Cyc__genrep_11};static char _tmp301[4]="tag";static struct _tuple5 Cyc__gentuple_203={
offsetof(struct Cyc_Absyn_Enumfield,tag),(struct _dyneither_ptr){_tmp301,_tmp301,
_tmp301 + 4},(void*)& Cyc__genrep_145};static char _tmp302[4]="loc";static struct
_tuple5 Cyc__gentuple_204={offsetof(struct Cyc_Absyn_Enumfield,loc),(struct
_dyneither_ptr){_tmp302,_tmp302,_tmp302 + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_205[3]={& Cyc__gentuple_202,& Cyc__gentuple_203,& Cyc__gentuple_204};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumfield_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_206,sizeof(struct Cyc_Absyn_Enumfield),{(void*)((
struct _tuple5**)Cyc__genarr_205),(void*)((struct _tuple5**)Cyc__genarr_205),(void*)((
struct _tuple5**)Cyc__genarr_205 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_201={
1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};static char _tmp305[5]="List";
static struct _dyneither_ptr Cyc__genname_218=(struct _dyneither_ptr){_tmp305,
_tmp305,_tmp305 + 5};static char _tmp306[3]="hd";static struct _tuple5 Cyc__gentuple_215={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp306,_tmp306,_tmp306 + 
3},(void*)& Cyc__genrep_201};static char _tmp307[3]="tl";static struct _tuple5 Cyc__gentuple_216={
offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){_tmp307,_tmp307,_tmp307 + 
3},(void*)& Cyc__genrep_214};static struct _tuple5*Cyc__genarr_217[2]={& Cyc__gentuple_215,&
Cyc__gentuple_216};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_enumfield_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_218,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_217),(void*)((struct _tuple5**)Cyc__genarr_217),(void*)((
struct _tuple5**)Cyc__genarr_217 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_214={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_enumfield_t46H2_rep)};static char
_tmp30A[4]="Opt";static struct _dyneither_ptr Cyc__genname_221=(struct
_dyneither_ptr){_tmp30A,_tmp30A,_tmp30A + 4};static char _tmp30B[2]="v";static
struct _tuple5 Cyc__gentuple_219={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp30B,_tmp30B,_tmp30B + 2},(void*)& Cyc__genrep_214};static
struct _tuple5*Cyc__genarr_220[1]={& Cyc__gentuple_219};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_221,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_220),(void*)((struct _tuple5**)Cyc__genarr_220),(void*)((
struct _tuple5**)Cyc__genarr_220 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_213={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep)};
static char _tmp30E[9]="Enumdecl";static struct _dyneither_ptr Cyc__genname_226=(
struct _dyneither_ptr){_tmp30E,_tmp30E,_tmp30E + 9};static char _tmp30F[3]="sc";
static struct _tuple5 Cyc__gentuple_222={offsetof(struct Cyc_Absyn_Enumdecl,sc),(
struct _dyneither_ptr){_tmp30F,_tmp30F,_tmp30F + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp310[5]="name";static struct _tuple5 Cyc__gentuple_223={offsetof(
struct Cyc_Absyn_Enumdecl,name),(struct _dyneither_ptr){_tmp310,_tmp310,_tmp310 + 5},(
void*)& Cyc__genrep_11};static char _tmp311[7]="fields";static struct _tuple5 Cyc__gentuple_224={
offsetof(struct Cyc_Absyn_Enumdecl,fields),(struct _dyneither_ptr){_tmp311,_tmp311,
_tmp311 + 7},(void*)& Cyc__genrep_213};static struct _tuple5*Cyc__genarr_225[3]={&
Cyc__gentuple_222,& Cyc__gentuple_223,& Cyc__gentuple_224};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Enumdecl_rep={3,(struct _dyneither_ptr*)& Cyc__genname_226,
sizeof(struct Cyc_Absyn_Enumdecl),{(void*)((struct _tuple5**)Cyc__genarr_225),(
void*)((struct _tuple5**)Cyc__genarr_225),(void*)((struct _tuple5**)Cyc__genarr_225
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_212={1,1,(void*)((void*)&
Cyc_struct_Absyn_Enumdecl_rep)};struct _tuple54{unsigned int f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};static struct _tuple6 Cyc__gentuple_227={
offsetof(struct _tuple54,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_228={
offsetof(struct _tuple54,f2),(void*)& Cyc__genrep_212};static struct _tuple6 Cyc__gentuple_229={
offsetof(struct _tuple54,f3),(void*)& Cyc__genrep_201};static struct _tuple6*Cyc__genarr_230[
3]={& Cyc__gentuple_227,& Cyc__gentuple_228,& Cyc__gentuple_229};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_211={4,sizeof(struct _tuple54),{(void*)((struct _tuple6**)Cyc__genarr_230),(
void*)((struct _tuple6**)Cyc__genarr_230),(void*)((struct _tuple6**)Cyc__genarr_230
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_200;struct _tuple55{
unsigned int f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};static struct _tuple6 Cyc__gentuple_207={
offsetof(struct _tuple55,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_208={
offsetof(struct _tuple55,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_209={offsetof(struct _tuple55,f3),(void*)& Cyc__genrep_201};
static struct _tuple6*Cyc__genarr_210[3]={& Cyc__gentuple_207,& Cyc__gentuple_208,&
Cyc__gentuple_209};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_200={4,
sizeof(struct _tuple55),{(void*)((struct _tuple6**)Cyc__genarr_210),(void*)((
struct _tuple6**)Cyc__genarr_210),(void*)((struct _tuple6**)Cyc__genarr_210 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_196;struct _tuple56{unsigned int
f1;struct _tuple0*f2;};static struct _tuple6 Cyc__gentuple_197={offsetof(struct
_tuple56,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_198={
offsetof(struct _tuple56,f2),(void*)& Cyc__genrep_11};static struct _tuple6*Cyc__genarr_199[
2]={& Cyc__gentuple_197,& Cyc__gentuple_198};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_196={4,sizeof(struct _tuple56),{(void*)((struct _tuple6**)Cyc__genarr_199),(
void*)((struct _tuple6**)Cyc__genarr_199),(void*)((struct _tuple6**)Cyc__genarr_199
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_185;struct _tuple57{
unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;int f4;};static struct
_tuple6 Cyc__gentuple_191={offsetof(struct _tuple57,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_192={offsetof(struct _tuple57,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_193={offsetof(struct _tuple57,f3),(void*)& Cyc__genrep_186};
static struct _tuple6 Cyc__gentuple_194={offsetof(struct _tuple57,f4),(void*)& Cyc__genrep_10};
static struct _tuple6*Cyc__genarr_195[4]={& Cyc__gentuple_191,& Cyc__gentuple_192,&
Cyc__gentuple_193,& Cyc__gentuple_194};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_185={
4,sizeof(struct _tuple57),{(void*)((struct _tuple6**)Cyc__genarr_195),(void*)((
struct _tuple6**)Cyc__genarr_195),(void*)((struct _tuple6**)Cyc__genarr_195 + 4)}};
static char _tmp318[7]="Wild_p";static struct _tuple7 Cyc__gentuple_182={0,(struct
_dyneither_ptr){_tmp318,_tmp318,_tmp318 + 7}};static char _tmp319[7]="Null_p";
static struct _tuple7 Cyc__gentuple_183={1,(struct _dyneither_ptr){_tmp319,_tmp319,
_tmp319 + 7}};static struct _tuple7*Cyc__genarr_184[2]={& Cyc__gentuple_182,& Cyc__gentuple_183};
static char _tmp31A[6]="Var_p";static struct _tuple5 Cyc__gentuple_439={0,(struct
_dyneither_ptr){_tmp31A,_tmp31A,_tmp31A + 6},(void*)& Cyc__genrep_434};static char
_tmp31B[12]="Reference_p";static struct _tuple5 Cyc__gentuple_440={1,(struct
_dyneither_ptr){_tmp31B,_tmp31B,_tmp31B + 12},(void*)& Cyc__genrep_434};static char
_tmp31C[9]="TagInt_p";static struct _tuple5 Cyc__gentuple_441={2,(struct
_dyneither_ptr){_tmp31C,_tmp31C,_tmp31C + 9},(void*)& Cyc__genrep_429};static char
_tmp31D[8]="Tuple_p";static struct _tuple5 Cyc__gentuple_442={3,(struct
_dyneither_ptr){_tmp31D,_tmp31D,_tmp31D + 8},(void*)& Cyc__genrep_424};static char
_tmp31E[10]="Pointer_p";static struct _tuple5 Cyc__gentuple_443={4,(struct
_dyneither_ptr){_tmp31E,_tmp31E,_tmp31E + 10},(void*)& Cyc__genrep_420};static char
_tmp31F[7]="Aggr_p";static struct _tuple5 Cyc__gentuple_444={5,(struct
_dyneither_ptr){_tmp31F,_tmp31F,_tmp31F + 7},(void*)& Cyc__genrep_330};static char
_tmp320[9]="Tunion_p";static struct _tuple5 Cyc__gentuple_445={6,(struct
_dyneither_ptr){_tmp320,_tmp320,_tmp320 + 9},(void*)& Cyc__genrep_245};static char
_tmp321[6]="Int_p";static struct _tuple5 Cyc__gentuple_446={7,(struct _dyneither_ptr){
_tmp321,_tmp321,_tmp321 + 6},(void*)& Cyc__genrep_235};static char _tmp322[7]="Char_p";
static struct _tuple5 Cyc__gentuple_447={8,(struct _dyneither_ptr){_tmp322,_tmp322,
_tmp322 + 7},(void*)& Cyc__genrep_231};static char _tmp323[8]="Float_p";static struct
_tuple5 Cyc__gentuple_448={9,(struct _dyneither_ptr){_tmp323,_tmp323,_tmp323 + 8},(
void*)& Cyc__genrep_81};static char _tmp324[7]="Enum_p";static struct _tuple5 Cyc__gentuple_449={
10,(struct _dyneither_ptr){_tmp324,_tmp324,_tmp324 + 7},(void*)& Cyc__genrep_211};
static char _tmp325[11]="AnonEnum_p";static struct _tuple5 Cyc__gentuple_450={11,(
struct _dyneither_ptr){_tmp325,_tmp325,_tmp325 + 11},(void*)& Cyc__genrep_200};
static char _tmp326[12]="UnknownId_p";static struct _tuple5 Cyc__gentuple_451={12,(
struct _dyneither_ptr){_tmp326,_tmp326,_tmp326 + 12},(void*)& Cyc__genrep_196};
static char _tmp327[14]="UnknownCall_p";static struct _tuple5 Cyc__gentuple_452={13,(
struct _dyneither_ptr){_tmp327,_tmp327,_tmp327 + 14},(void*)& Cyc__genrep_185};
static char _tmp328[6]="Exp_p";static struct _tuple5 Cyc__gentuple_453={14,(struct
_dyneither_ptr){_tmp328,_tmp328,_tmp328 + 6},(void*)& Cyc__genrep_129};static
struct _tuple5*Cyc__genarr_454[15]={& Cyc__gentuple_439,& Cyc__gentuple_440,& Cyc__gentuple_441,&
Cyc__gentuple_442,& Cyc__gentuple_443,& Cyc__gentuple_444,& Cyc__gentuple_445,& Cyc__gentuple_446,&
Cyc__gentuple_447,& Cyc__gentuple_448,& Cyc__gentuple_449,& Cyc__gentuple_450,& Cyc__gentuple_451,&
Cyc__gentuple_452,& Cyc__gentuple_453};static char _tmp32A[8]="Raw_pat";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep={5,(struct _dyneither_ptr){_tmp32A,_tmp32A,_tmp32A + 8},{(
void*)((struct _tuple7**)Cyc__genarr_184),(void*)((struct _tuple7**)Cyc__genarr_184),(
void*)((struct _tuple7**)Cyc__genarr_184 + 2)},{(void*)((struct _tuple5**)Cyc__genarr_454),(
void*)((struct _tuple5**)Cyc__genarr_454),(void*)((struct _tuple5**)Cyc__genarr_454
+ 15)}};static char _tmp32B[4]="Pat";static struct _dyneither_ptr Cyc__genname_459=(
struct _dyneither_ptr){_tmp32B,_tmp32B,_tmp32B + 4};static char _tmp32C[2]="r";
static struct _tuple5 Cyc__gentuple_455={offsetof(struct Cyc_Absyn_Pat,r),(struct
_dyneither_ptr){_tmp32C,_tmp32C,_tmp32C + 2},(void*)& Cyc_Absyn_raw_pat_t_rep};
static char _tmp32D[5]="topt";static struct _tuple5 Cyc__gentuple_456={offsetof(
struct Cyc_Absyn_Pat,topt),(struct _dyneither_ptr){_tmp32D,_tmp32D,_tmp32D + 5},(
void*)& Cyc__genrep_110};static char _tmp32E[4]="loc";static struct _tuple5 Cyc__gentuple_457={
offsetof(struct Cyc_Absyn_Pat,loc),(struct _dyneither_ptr){_tmp32E,_tmp32E,_tmp32E
+ 4},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_458[3]={& Cyc__gentuple_455,&
Cyc__gentuple_456,& Cyc__gentuple_457};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_459,sizeof(struct Cyc_Absyn_Pat),{(void*)((
struct _tuple5**)Cyc__genarr_458),(void*)((struct _tuple5**)Cyc__genarr_458),(void*)((
struct _tuple5**)Cyc__genarr_458 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_181={
1,1,(void*)((void*)& Cyc_struct_Absyn_Pat_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_146;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_147;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_vardecl_t46H2_rep;static char _tmp331[5]="List";static
struct _dyneither_ptr Cyc__genname_177=(struct _dyneither_ptr){_tmp331,_tmp331,
_tmp331 + 5};static char _tmp332[3]="hd";static struct _tuple5 Cyc__gentuple_174={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp332,_tmp332,_tmp332 + 
3},(void*)& Cyc__genrep_148};static char _tmp333[3]="tl";static struct _tuple5 Cyc__gentuple_175={
offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){_tmp333,_tmp333,_tmp333 + 
3},(void*)& Cyc__genrep_147};static struct _tuple5*Cyc__genarr_176[2]={& Cyc__gentuple_174,&
Cyc__gentuple_175};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_vardecl_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_177,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_176),(void*)((struct _tuple5**)Cyc__genarr_176),(void*)((
struct _tuple5**)Cyc__genarr_176 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_147={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_vardecl_t46H2_rep)};static char
_tmp336[4]="Opt";static struct _dyneither_ptr Cyc__genname_180=(struct
_dyneither_ptr){_tmp336,_tmp336,_tmp336 + 4};static char _tmp337[2]="v";static
struct _tuple5 Cyc__gentuple_178={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp337,_tmp337,_tmp337 + 2},(void*)& Cyc__genrep_147};static
struct _tuple5*Cyc__genarr_179[1]={& Cyc__gentuple_178};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_180,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_179),(void*)((struct _tuple5**)Cyc__genarr_179),(void*)((
struct _tuple5**)Cyc__genarr_179 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_146={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep)};
static char _tmp33A[14]="Switch_clause";static struct _dyneither_ptr Cyc__genname_466=(
struct _dyneither_ptr){_tmp33A,_tmp33A,_tmp33A + 14};static char _tmp33B[8]="pattern";
static struct _tuple5 Cyc__gentuple_460={offsetof(struct Cyc_Absyn_Switch_clause,pattern),(
struct _dyneither_ptr){_tmp33B,_tmp33B,_tmp33B + 8},(void*)& Cyc__genrep_181};
static char _tmp33C[9]="pat_vars";static struct _tuple5 Cyc__gentuple_461={offsetof(
struct Cyc_Absyn_Switch_clause,pat_vars),(struct _dyneither_ptr){_tmp33C,_tmp33C,
_tmp33C + 9},(void*)& Cyc__genrep_146};static char _tmp33D[13]="where_clause";static
struct _tuple5 Cyc__gentuple_462={offsetof(struct Cyc_Absyn_Switch_clause,where_clause),(
struct _dyneither_ptr){_tmp33D,_tmp33D,_tmp33D + 13},(void*)& Cyc__genrep_145};
static char _tmp33E[5]="body";static struct _tuple5 Cyc__gentuple_463={offsetof(
struct Cyc_Absyn_Switch_clause,body),(struct _dyneither_ptr){_tmp33E,_tmp33E,
_tmp33E + 5},(void*)& Cyc__genrep_134};static char _tmp33F[4]="loc";static struct
_tuple5 Cyc__gentuple_464={offsetof(struct Cyc_Absyn_Switch_clause,loc),(struct
_dyneither_ptr){_tmp33F,_tmp33F,_tmp33F + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_465[5]={& Cyc__gentuple_460,& Cyc__gentuple_461,& Cyc__gentuple_462,&
Cyc__gentuple_463,& Cyc__gentuple_464};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Switch_clause_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_466,sizeof(struct Cyc_Absyn_Switch_clause),{(
void*)((struct _tuple5**)Cyc__genarr_465),(void*)((struct _tuple5**)Cyc__genarr_465),(
void*)((struct _tuple5**)Cyc__genarr_465 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_144={1,1,(void*)((void*)& Cyc_struct_Absyn_Switch_clause_rep)};static
char _tmp342[5]="List";static struct _dyneither_ptr Cyc__genname_470=(struct
_dyneither_ptr){_tmp342,_tmp342,_tmp342 + 5};static char _tmp343[3]="hd";static
struct _tuple5 Cyc__gentuple_467={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp343,_tmp343,_tmp343 + 3},(void*)((void*)& Cyc__genrep_144)};
static char _tmp344[3]="tl";static struct _tuple5 Cyc__gentuple_468={offsetof(struct
Cyc_List_List,tl),(struct _dyneither_ptr){_tmp344,_tmp344,_tmp344 + 3},(void*)& Cyc__genrep_143};
static struct _tuple5*Cyc__genarr_469[2]={& Cyc__gentuple_467,& Cyc__gentuple_468};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_470,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_469),(void*)((struct _tuple5**)Cyc__genarr_469),(void*)((
struct _tuple5**)Cyc__genarr_469 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_143={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep)};struct
_tuple58{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_506={offsetof(struct _tuple58,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_507={offsetof(struct _tuple58,f2),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_508={offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_143};
static struct _tuple6*Cyc__genarr_509[3]={& Cyc__gentuple_506,& Cyc__gentuple_507,&
Cyc__gentuple_508};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_505={4,
sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_509),(void*)((
struct _tuple6**)Cyc__genarr_509),(void*)((struct _tuple6**)Cyc__genarr_509 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_494;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_496;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep;
static char _tmp348[5]="List";static struct _dyneither_ptr Cyc__genname_500=(struct
_dyneither_ptr){_tmp348,_tmp348,_tmp348 + 5};static char _tmp349[3]="hd";static
struct _tuple5 Cyc__gentuple_497={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp349,_tmp349,_tmp349 + 3},(void*)& Cyc__genrep_130};static char
_tmp34A[3]="tl";static struct _tuple5 Cyc__gentuple_498={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp34A,_tmp34A,_tmp34A + 3},(void*)& Cyc__genrep_496};
static struct _tuple5*Cyc__genarr_499[2]={& Cyc__gentuple_497,& Cyc__gentuple_498};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_500,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_499),(void*)((struct _tuple5**)Cyc__genarr_499),(void*)((
struct _tuple5**)Cyc__genarr_499 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_496={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_exp_t46H2_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_495;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_495={1,1,(void*)((
void*)& Cyc__genrep_144)};struct _tuple59{unsigned int f1;struct Cyc_List_List*f2;
struct Cyc_Absyn_Switch_clause**f3;};static struct _tuple6 Cyc__gentuple_501={
offsetof(struct _tuple59,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_502={
offsetof(struct _tuple59,f2),(void*)& Cyc__genrep_496};static struct _tuple6 Cyc__gentuple_503={
offsetof(struct _tuple59,f3),(void*)& Cyc__genrep_495};static struct _tuple6*Cyc__genarr_504[
3]={& Cyc__gentuple_501,& Cyc__gentuple_502,& Cyc__gentuple_503};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_494={4,sizeof(struct _tuple59),{(void*)((struct _tuple6**)Cyc__genarr_504),(
void*)((struct _tuple6**)Cyc__genarr_504),(void*)((struct _tuple6**)Cyc__genarr_504
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_489;struct _tuple60{
unsigned int f1;struct Cyc_Absyn_Decl*f2;struct Cyc_Absyn_Stmt*f3;};static struct
_tuple6 Cyc__gentuple_490={offsetof(struct _tuple60,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_491={offsetof(struct _tuple60,f2),(void*)& Cyc__genrep_1};
static struct _tuple6 Cyc__gentuple_492={offsetof(struct _tuple60,f3),(void*)& Cyc__genrep_134};
static struct _tuple6*Cyc__genarr_493[3]={& Cyc__gentuple_490,& Cyc__gentuple_491,&
Cyc__gentuple_492};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_489={4,
sizeof(struct _tuple60),{(void*)((struct _tuple6**)Cyc__genarr_493),(void*)((
struct _tuple6**)Cyc__genarr_493),(void*)((struct _tuple6**)Cyc__genarr_493 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_484;static struct _tuple6 Cyc__gentuple_485={
offsetof(struct _tuple40,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_486={
offsetof(struct _tuple40,f2),(void*)& Cyc__genrep_13};static struct _tuple6 Cyc__gentuple_487={
offsetof(struct _tuple40,f3),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_488[
3]={& Cyc__gentuple_485,& Cyc__gentuple_486,& Cyc__gentuple_487};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_484={4,sizeof(struct _tuple40),{(void*)((struct _tuple6**)Cyc__genarr_488),(
void*)((struct _tuple6**)Cyc__genarr_488),(void*)((struct _tuple6**)Cyc__genarr_488
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_475;struct _tuple61{
unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct _tuple2 f3;};static struct _tuple6 Cyc__gentuple_480={
offsetof(struct _tuple61,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_481={
offsetof(struct _tuple61,f2),(void*)& Cyc__genrep_134};static struct _tuple6 Cyc__gentuple_482={
offsetof(struct _tuple61,f3),(void*)& Cyc__genrep_476};static struct _tuple6*Cyc__genarr_483[
3]={& Cyc__gentuple_480,& Cyc__gentuple_481,& Cyc__gentuple_482};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_475={4,sizeof(struct _tuple61),{(void*)((struct _tuple6**)Cyc__genarr_483),(
void*)((struct _tuple6**)Cyc__genarr_483),(void*)((struct _tuple6**)Cyc__genarr_483
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_142;struct _tuple62{
unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;};static struct
_tuple6 Cyc__gentuple_471={offsetof(struct _tuple62,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_472={offsetof(struct _tuple62,f2),(void*)& Cyc__genrep_134};
static struct _tuple6 Cyc__gentuple_473={offsetof(struct _tuple62,f3),(void*)& Cyc__genrep_143};
static struct _tuple6*Cyc__genarr_474[3]={& Cyc__gentuple_471,& Cyc__gentuple_472,&
Cyc__gentuple_473};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_142={4,
sizeof(struct _tuple62),{(void*)((struct _tuple6**)Cyc__genarr_474),(void*)((
struct _tuple6**)Cyc__genarr_474),(void*)((struct _tuple6**)Cyc__genarr_474 + 3)}};
static char _tmp353[7]="Skip_s";static struct _tuple7 Cyc__gentuple_140={0,(struct
_dyneither_ptr){_tmp353,_tmp353,_tmp353 + 7}};static struct _tuple7*Cyc__genarr_141[
1]={& Cyc__gentuple_140};static char _tmp354[6]="Exp_s";static struct _tuple5 Cyc__gentuple_547={
0,(struct _dyneither_ptr){_tmp354,_tmp354,_tmp354 + 6},(void*)& Cyc__genrep_129};
static char _tmp355[6]="Seq_s";static struct _tuple5 Cyc__gentuple_548={1,(struct
_dyneither_ptr){_tmp355,_tmp355,_tmp355 + 6},(void*)& Cyc__genrep_542};static char
_tmp356[9]="Return_s";static struct _tuple5 Cyc__gentuple_549={2,(struct
_dyneither_ptr){_tmp356,_tmp356,_tmp356 + 9},(void*)& Cyc__genrep_538};static char
_tmp357[13]="IfThenElse_s";static struct _tuple5 Cyc__gentuple_550={3,(struct
_dyneither_ptr){_tmp357,_tmp357,_tmp357 + 13},(void*)& Cyc__genrep_532};static char
_tmp358[8]="While_s";static struct _tuple5 Cyc__gentuple_551={4,(struct
_dyneither_ptr){_tmp358,_tmp358,_tmp358 + 8},(void*)& Cyc__genrep_527};static char
_tmp359[8]="Break_s";static struct _tuple5 Cyc__gentuple_552={5,(struct
_dyneither_ptr){_tmp359,_tmp359,_tmp359 + 8},(void*)& Cyc__genrep_523};static char
_tmp35A[11]="Continue_s";static struct _tuple5 Cyc__gentuple_553={6,(struct
_dyneither_ptr){_tmp35A,_tmp35A,_tmp35A + 11},(void*)& Cyc__genrep_523};static char
_tmp35B[7]="Goto_s";static struct _tuple5 Cyc__gentuple_554={7,(struct
_dyneither_ptr){_tmp35B,_tmp35B,_tmp35B + 7},(void*)& Cyc__genrep_517};static char
_tmp35C[6]="For_s";static struct _tuple5 Cyc__gentuple_555={8,(struct _dyneither_ptr){
_tmp35C,_tmp35C,_tmp35C + 6},(void*)& Cyc__genrep_510};static char _tmp35D[9]="Switch_s";
static struct _tuple5 Cyc__gentuple_556={9,(struct _dyneither_ptr){_tmp35D,_tmp35D,
_tmp35D + 9},(void*)& Cyc__genrep_505};static char _tmp35E[11]="Fallthru_s";static
struct _tuple5 Cyc__gentuple_557={10,(struct _dyneither_ptr){_tmp35E,_tmp35E,
_tmp35E + 11},(void*)& Cyc__genrep_494};static char _tmp35F[7]="Decl_s";static struct
_tuple5 Cyc__gentuple_558={11,(struct _dyneither_ptr){_tmp35F,_tmp35F,_tmp35F + 7},(
void*)& Cyc__genrep_489};static char _tmp360[8]="Label_s";static struct _tuple5 Cyc__gentuple_559={
12,(struct _dyneither_ptr){_tmp360,_tmp360,_tmp360 + 8},(void*)& Cyc__genrep_484};
static char _tmp361[5]="Do_s";static struct _tuple5 Cyc__gentuple_560={13,(struct
_dyneither_ptr){_tmp361,_tmp361,_tmp361 + 5},(void*)& Cyc__genrep_475};static char
_tmp362[11]="TryCatch_s";static struct _tuple5 Cyc__gentuple_561={14,(struct
_dyneither_ptr){_tmp362,_tmp362,_tmp362 + 11},(void*)& Cyc__genrep_142};static char
_tmp363[14]="ResetRegion_s";static struct _tuple5 Cyc__gentuple_562={15,(struct
_dyneither_ptr){_tmp363,_tmp363,_tmp363 + 14},(void*)& Cyc__genrep_129};static
struct _tuple5*Cyc__genarr_563[16]={& Cyc__gentuple_547,& Cyc__gentuple_548,& Cyc__gentuple_549,&
Cyc__gentuple_550,& Cyc__gentuple_551,& Cyc__gentuple_552,& Cyc__gentuple_553,& Cyc__gentuple_554,&
Cyc__gentuple_555,& Cyc__gentuple_556,& Cyc__gentuple_557,& Cyc__gentuple_558,& Cyc__gentuple_559,&
Cyc__gentuple_560,& Cyc__gentuple_561,& Cyc__gentuple_562};static char _tmp365[9]="Raw_stmt";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep={5,(struct _dyneither_ptr){
_tmp365,_tmp365,_tmp365 + 9},{(void*)((struct _tuple7**)Cyc__genarr_141),(void*)((
struct _tuple7**)Cyc__genarr_141),(void*)((struct _tuple7**)Cyc__genarr_141 + 1)},{(
void*)((struct _tuple5**)Cyc__genarr_563),(void*)((struct _tuple5**)Cyc__genarr_563),(
void*)((struct _tuple5**)Cyc__genarr_563 + 16)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_135;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_stmt_t46H2_rep;
static char _tmp366[5]="List";static struct _dyneither_ptr Cyc__genname_139=(struct
_dyneither_ptr){_tmp366,_tmp366,_tmp366 + 5};static char _tmp367[3]="hd";static
struct _tuple5 Cyc__gentuple_136={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp367,_tmp367,_tmp367 + 3},(void*)& Cyc__genrep_134};static char
_tmp368[3]="tl";static struct _tuple5 Cyc__gentuple_137={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp368,_tmp368,_tmp368 + 3},(void*)& Cyc__genrep_135};
static struct _tuple5*Cyc__genarr_138[2]={& Cyc__gentuple_136,& Cyc__gentuple_137};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_stmt_t46H2_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_139,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_138),(void*)((struct _tuple5**)Cyc__genarr_138),(void*)((
struct _tuple5**)Cyc__genarr_138 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_135={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_stmt_t46H2_rep)};extern struct Cyc_Typerep_XTUnion_struct
Cyc_Absyn_absyn_annot_t_rep;static struct _tuple8*Cyc__genarr_131[0]={};static char
_tmp36C[11]="AbsynAnnot";struct Cyc_Typerep_XTUnion_struct Cyc_Absyn_absyn_annot_t_rep={
7,(struct _dyneither_ptr){_tmp36C,_tmp36C,_tmp36C + 11},{(void*)((struct _tuple8**)
Cyc__genarr_131),(void*)((struct _tuple8**)Cyc__genarr_131),(void*)((struct
_tuple8**)Cyc__genarr_131 + 0)}};static char _tmp36D[5]="Stmt";static struct
_dyneither_ptr Cyc__genname_570=(struct _dyneither_ptr){_tmp36D,_tmp36D,_tmp36D + 5};
static char _tmp36E[2]="r";static struct _tuple5 Cyc__gentuple_564={offsetof(struct
Cyc_Absyn_Stmt,r),(struct _dyneither_ptr){_tmp36E,_tmp36E,_tmp36E + 2},(void*)& Cyc_Absyn_raw_stmt_t_rep};
static char _tmp36F[4]="loc";static struct _tuple5 Cyc__gentuple_565={offsetof(struct
Cyc_Absyn_Stmt,loc),(struct _dyneither_ptr){_tmp36F,_tmp36F,_tmp36F + 4},(void*)&
Cyc__genrep_2};static char _tmp370[16]="non_local_preds";static struct _tuple5 Cyc__gentuple_566={
offsetof(struct Cyc_Absyn_Stmt,non_local_preds),(struct _dyneither_ptr){_tmp370,
_tmp370,_tmp370 + 16},(void*)& Cyc__genrep_135};static char _tmp371[10]="try_depth";
static struct _tuple5 Cyc__gentuple_567={offsetof(struct Cyc_Absyn_Stmt,try_depth),(
struct _dyneither_ptr){_tmp371,_tmp371,_tmp371 + 10},(void*)& Cyc__genrep_10};
static char _tmp372[6]="annot";static struct _tuple5 Cyc__gentuple_568={offsetof(
struct Cyc_Absyn_Stmt,annot),(struct _dyneither_ptr){_tmp372,_tmp372,_tmp372 + 6},(
void*)& Cyc_Absyn_absyn_annot_t_rep};static struct _tuple5*Cyc__genarr_569[5]={& Cyc__gentuple_564,&
Cyc__gentuple_565,& Cyc__gentuple_566,& Cyc__gentuple_567,& Cyc__gentuple_568};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Stmt_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_570,sizeof(struct Cyc_Absyn_Stmt),{(void*)((struct
_tuple5**)Cyc__genarr_569),(void*)((struct _tuple5**)Cyc__genarr_569),(void*)((
struct _tuple5**)Cyc__genarr_569 + 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_134={
1,1,(void*)((void*)& Cyc_struct_Absyn_Stmt_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_806;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_806={1,1,(void*)((
void*)& Cyc_struct_Absyn_Vardecl_rep)};static char _tmp376[7]="Fndecl";static struct
_dyneither_ptr Cyc__genname_835=(struct _dyneither_ptr){_tmp376,_tmp376,_tmp376 + 7};
static char _tmp377[3]="sc";static struct _tuple5 Cyc__gentuple_819={offsetof(struct
Cyc_Absyn_Fndecl,sc),(struct _dyneither_ptr){_tmp377,_tmp377,_tmp377 + 3},(void*)&
Cyc_Absyn_scope_t_rep};static char _tmp378[10]="is_inline";static struct _tuple5 Cyc__gentuple_820={
offsetof(struct Cyc_Absyn_Fndecl,is_inline),(struct _dyneither_ptr){_tmp378,
_tmp378,_tmp378 + 10},(void*)& Cyc__genrep_10};static char _tmp379[5]="name";static
struct _tuple5 Cyc__gentuple_821={offsetof(struct Cyc_Absyn_Fndecl,name),(struct
_dyneither_ptr){_tmp379,_tmp379,_tmp379 + 5},(void*)& Cyc__genrep_11};static char
_tmp37A[4]="tvs";static struct _tuple5 Cyc__gentuple_822={offsetof(struct Cyc_Absyn_Fndecl,tvs),(
struct _dyneither_ptr){_tmp37A,_tmp37A,_tmp37A + 4},(void*)& Cyc__genrep_273};
static char _tmp37B[7]="effect";static struct _tuple5 Cyc__gentuple_823={offsetof(
struct Cyc_Absyn_Fndecl,effect),(struct _dyneither_ptr){_tmp37B,_tmp37B,_tmp37B + 7},(
void*)& Cyc__genrep_110};static char _tmp37C[9]="ret_type";static struct _tuple5 Cyc__gentuple_824={
offsetof(struct Cyc_Absyn_Fndecl,ret_type),(struct _dyneither_ptr){_tmp37C,_tmp37C,
_tmp37C + 9},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp37D[5]="args";
static struct _tuple5 Cyc__gentuple_825={offsetof(struct Cyc_Absyn_Fndecl,args),(
struct _dyneither_ptr){_tmp37D,_tmp37D,_tmp37D + 5},(void*)& Cyc__genrep_808};
static char _tmp37E[10]="c_varargs";static struct _tuple5 Cyc__gentuple_826={
offsetof(struct Cyc_Absyn_Fndecl,c_varargs),(struct _dyneither_ptr){_tmp37E,
_tmp37E,_tmp37E + 10},(void*)& Cyc__genrep_10};static char _tmp37F[12]="cyc_varargs";
static struct _tuple5 Cyc__gentuple_827={offsetof(struct Cyc_Absyn_Fndecl,cyc_varargs),(
struct _dyneither_ptr){_tmp37F,_tmp37F,_tmp37F + 12},(void*)& Cyc__genrep_807};
static char _tmp380[7]="rgn_po";static struct _tuple5 Cyc__gentuple_828={offsetof(
struct Cyc_Absyn_Fndecl,rgn_po),(struct _dyneither_ptr){_tmp380,_tmp380,_tmp380 + 7},(
void*)& Cyc__genrep_372};static char _tmp381[5]="body";static struct _tuple5 Cyc__gentuple_829={
offsetof(struct Cyc_Absyn_Fndecl,body),(struct _dyneither_ptr){_tmp381,_tmp381,
_tmp381 + 5},(void*)& Cyc__genrep_134};static char _tmp382[11]="cached_typ";static
struct _tuple5 Cyc__gentuple_830={offsetof(struct Cyc_Absyn_Fndecl,cached_typ),(
struct _dyneither_ptr){_tmp382,_tmp382,_tmp382 + 11},(void*)& Cyc__genrep_110};
static char _tmp383[15]="param_vardecls";static struct _tuple5 Cyc__gentuple_831={
offsetof(struct Cyc_Absyn_Fndecl,param_vardecls),(struct _dyneither_ptr){_tmp383,
_tmp383,_tmp383 + 15},(void*)& Cyc__genrep_146};static char _tmp384[11]="fn_vardecl";
static struct _tuple5 Cyc__gentuple_832={offsetof(struct Cyc_Absyn_Fndecl,fn_vardecl),(
struct _dyneither_ptr){_tmp384,_tmp384,_tmp384 + 11},(void*)& Cyc__genrep_806};
static char _tmp385[11]="attributes";static struct _tuple5 Cyc__gentuple_833={
offsetof(struct Cyc_Absyn_Fndecl,attributes),(struct _dyneither_ptr){_tmp385,
_tmp385,_tmp385 + 11},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_834[
15]={& Cyc__gentuple_819,& Cyc__gentuple_820,& Cyc__gentuple_821,& Cyc__gentuple_822,&
Cyc__gentuple_823,& Cyc__gentuple_824,& Cyc__gentuple_825,& Cyc__gentuple_826,& Cyc__gentuple_827,&
Cyc__gentuple_828,& Cyc__gentuple_829,& Cyc__gentuple_830,& Cyc__gentuple_831,& Cyc__gentuple_832,&
Cyc__gentuple_833};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_835,sizeof(struct Cyc_Absyn_Fndecl),{(void*)((
struct _tuple5**)Cyc__genarr_834),(void*)((struct _tuple5**)Cyc__genarr_834),(void*)((
struct _tuple5**)Cyc__genarr_834 + 15)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_805={
1,1,(void*)((void*)& Cyc_struct_Absyn_Fndecl_rep)};struct _tuple63{unsigned int f1;
struct Cyc_Absyn_Fndecl*f2;};static struct _tuple6 Cyc__gentuple_836={offsetof(
struct _tuple63,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_837={
offsetof(struct _tuple63,f2),(void*)& Cyc__genrep_805};static struct _tuple6*Cyc__genarr_838[
2]={& Cyc__gentuple_836,& Cyc__gentuple_837};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_804={4,sizeof(struct _tuple63),{(void*)((struct _tuple6**)Cyc__genarr_838),(
void*)((struct _tuple6**)Cyc__genarr_838),(void*)((struct _tuple6**)Cyc__genarr_838
+ 2)}};static char _tmp389[13]="Unresolved_b";static struct _tuple7 Cyc__gentuple_798={
0,(struct _dyneither_ptr){_tmp389,_tmp389,_tmp389 + 13}};static struct _tuple7*Cyc__genarr_799[
1]={& Cyc__gentuple_798};static char _tmp38A[9]="Global_b";static struct _tuple5 Cyc__gentuple_839={
0,(struct _dyneither_ptr){_tmp38A,_tmp38A,_tmp38A + 9},(void*)& Cyc__genrep_800};
static char _tmp38B[10]="Funname_b";static struct _tuple5 Cyc__gentuple_840={1,(
struct _dyneither_ptr){_tmp38B,_tmp38B,_tmp38B + 10},(void*)& Cyc__genrep_804};
static char _tmp38C[8]="Param_b";static struct _tuple5 Cyc__gentuple_841={2,(struct
_dyneither_ptr){_tmp38C,_tmp38C,_tmp38C + 8},(void*)& Cyc__genrep_800};static char
_tmp38D[8]="Local_b";static struct _tuple5 Cyc__gentuple_842={3,(struct
_dyneither_ptr){_tmp38D,_tmp38D,_tmp38D + 8},(void*)& Cyc__genrep_800};static char
_tmp38E[6]="Pat_b";static struct _tuple5 Cyc__gentuple_843={4,(struct _dyneither_ptr){
_tmp38E,_tmp38E,_tmp38E + 6},(void*)& Cyc__genrep_800};static struct _tuple5*Cyc__genarr_844[
5]={& Cyc__gentuple_839,& Cyc__gentuple_840,& Cyc__gentuple_841,& Cyc__gentuple_842,&
Cyc__gentuple_843};static char _tmp390[8]="Binding";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_binding_t_rep={5,(struct _dyneither_ptr){_tmp390,_tmp390,_tmp390 + 8},{(
void*)((struct _tuple7**)Cyc__genarr_799),(void*)((struct _tuple7**)Cyc__genarr_799),(
void*)((struct _tuple7**)Cyc__genarr_799 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_844),(
void*)((struct _tuple5**)Cyc__genarr_844),(void*)((struct _tuple5**)Cyc__genarr_844
+ 5)}};struct _tuple64{unsigned int f1;struct _tuple0*f2;void*f3;};static struct
_tuple6 Cyc__gentuple_845={offsetof(struct _tuple64,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_846={offsetof(struct _tuple64,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_847={offsetof(struct _tuple64,f3),(void*)& Cyc_Absyn_binding_t_rep};
static struct _tuple6*Cyc__genarr_848[3]={& Cyc__gentuple_845,& Cyc__gentuple_846,&
Cyc__gentuple_847};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_797={4,
sizeof(struct _tuple64),{(void*)((struct _tuple6**)Cyc__genarr_848),(void*)((
struct _tuple6**)Cyc__genarr_848),(void*)((struct _tuple6**)Cyc__genarr_848 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_792;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_primop_t_rep;static char _tmp392[5]="Plus";static struct _tuple7 Cyc__gentuple_762={
0,(struct _dyneither_ptr){_tmp392,_tmp392,_tmp392 + 5}};static char _tmp393[6]="Times";
static struct _tuple7 Cyc__gentuple_763={1,(struct _dyneither_ptr){_tmp393,_tmp393,
_tmp393 + 6}};static char _tmp394[6]="Minus";static struct _tuple7 Cyc__gentuple_764={
2,(struct _dyneither_ptr){_tmp394,_tmp394,_tmp394 + 6}};static char _tmp395[4]="Div";
static struct _tuple7 Cyc__gentuple_765={3,(struct _dyneither_ptr){_tmp395,_tmp395,
_tmp395 + 4}};static char _tmp396[4]="Mod";static struct _tuple7 Cyc__gentuple_766={4,(
struct _dyneither_ptr){_tmp396,_tmp396,_tmp396 + 4}};static char _tmp397[3]="Eq";
static struct _tuple7 Cyc__gentuple_767={5,(struct _dyneither_ptr){_tmp397,_tmp397,
_tmp397 + 3}};static char _tmp398[4]="Neq";static struct _tuple7 Cyc__gentuple_768={6,(
struct _dyneither_ptr){_tmp398,_tmp398,_tmp398 + 4}};static char _tmp399[3]="Gt";
static struct _tuple7 Cyc__gentuple_769={7,(struct _dyneither_ptr){_tmp399,_tmp399,
_tmp399 + 3}};static char _tmp39A[3]="Lt";static struct _tuple7 Cyc__gentuple_770={8,(
struct _dyneither_ptr){_tmp39A,_tmp39A,_tmp39A + 3}};static char _tmp39B[4]="Gte";
static struct _tuple7 Cyc__gentuple_771={9,(struct _dyneither_ptr){_tmp39B,_tmp39B,
_tmp39B + 4}};static char _tmp39C[4]="Lte";static struct _tuple7 Cyc__gentuple_772={10,(
struct _dyneither_ptr){_tmp39C,_tmp39C,_tmp39C + 4}};static char _tmp39D[4]="Not";
static struct _tuple7 Cyc__gentuple_773={11,(struct _dyneither_ptr){_tmp39D,_tmp39D,
_tmp39D + 4}};static char _tmp39E[7]="Bitnot";static struct _tuple7 Cyc__gentuple_774={
12,(struct _dyneither_ptr){_tmp39E,_tmp39E,_tmp39E + 7}};static char _tmp39F[7]="Bitand";
static struct _tuple7 Cyc__gentuple_775={13,(struct _dyneither_ptr){_tmp39F,_tmp39F,
_tmp39F + 7}};static char _tmp3A0[6]="Bitor";static struct _tuple7 Cyc__gentuple_776={
14,(struct _dyneither_ptr){_tmp3A0,_tmp3A0,_tmp3A0 + 6}};static char _tmp3A1[7]="Bitxor";
static struct _tuple7 Cyc__gentuple_777={15,(struct _dyneither_ptr){_tmp3A1,_tmp3A1,
_tmp3A1 + 7}};static char _tmp3A2[10]="Bitlshift";static struct _tuple7 Cyc__gentuple_778={
16,(struct _dyneither_ptr){_tmp3A2,_tmp3A2,_tmp3A2 + 10}};static char _tmp3A3[11]="Bitlrshift";
static struct _tuple7 Cyc__gentuple_779={17,(struct _dyneither_ptr){_tmp3A3,_tmp3A3,
_tmp3A3 + 11}};static char _tmp3A4[11]="Bitarshift";static struct _tuple7 Cyc__gentuple_780={
18,(struct _dyneither_ptr){_tmp3A4,_tmp3A4,_tmp3A4 + 11}};static char _tmp3A5[8]="Numelts";
static struct _tuple7 Cyc__gentuple_781={19,(struct _dyneither_ptr){_tmp3A5,_tmp3A5,
_tmp3A5 + 8}};static struct _tuple7*Cyc__genarr_782[20]={& Cyc__gentuple_762,& Cyc__gentuple_763,&
Cyc__gentuple_764,& Cyc__gentuple_765,& Cyc__gentuple_766,& Cyc__gentuple_767,& Cyc__gentuple_768,&
Cyc__gentuple_769,& Cyc__gentuple_770,& Cyc__gentuple_771,& Cyc__gentuple_772,& Cyc__gentuple_773,&
Cyc__gentuple_774,& Cyc__gentuple_775,& Cyc__gentuple_776,& Cyc__gentuple_777,& Cyc__gentuple_778,&
Cyc__gentuple_779,& Cyc__gentuple_780,& Cyc__gentuple_781};static struct _tuple5*Cyc__genarr_783[
0]={};static char _tmp3A7[7]="Primop";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep={
5,(struct _dyneither_ptr){_tmp3A7,_tmp3A7,_tmp3A7 + 7},{(void*)((struct _tuple7**)
Cyc__genarr_782),(void*)((struct _tuple7**)Cyc__genarr_782),(void*)((struct
_tuple7**)Cyc__genarr_782 + 20)},{(void*)((struct _tuple5**)Cyc__genarr_783),(void*)((
struct _tuple5**)Cyc__genarr_783),(void*)((struct _tuple5**)Cyc__genarr_783 + 0)}};
struct _tuple65{unsigned int f1;void*f2;struct Cyc_List_List*f3;};static struct
_tuple6 Cyc__gentuple_793={offsetof(struct _tuple65,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_794={offsetof(struct _tuple65,f2),(void*)& Cyc_Absyn_primop_t_rep};
static struct _tuple6 Cyc__gentuple_795={offsetof(struct _tuple65,f3),(void*)& Cyc__genrep_496};
static struct _tuple6*Cyc__genarr_796[3]={& Cyc__gentuple_793,& Cyc__gentuple_794,&
Cyc__gentuple_795};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_792={4,
sizeof(struct _tuple65),{(void*)((struct _tuple6**)Cyc__genarr_796),(void*)((
struct _tuple6**)Cyc__genarr_796),(void*)((struct _tuple6**)Cyc__genarr_796 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_760;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_761;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep;
static char _tmp3A9[4]="Opt";static struct _dyneither_ptr Cyc__genname_786=(struct
_dyneither_ptr){_tmp3A9,_tmp3A9,_tmp3A9 + 4};static char _tmp3AA[2]="v";static
struct _tuple5 Cyc__gentuple_784={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp3AA,_tmp3AA,_tmp3AA + 2},(void*)& Cyc_Absyn_primop_t_rep};
static struct _tuple5*Cyc__genarr_785[1]={& Cyc__gentuple_784};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_primop_t2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_786,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_785),(void*)((
struct _tuple5**)Cyc__genarr_785),(void*)((struct _tuple5**)Cyc__genarr_785 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_761={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_primop_t2_rep)};
struct _tuple66{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Core_Opt*f3;
struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_787={offsetof(struct
_tuple66,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_788={
offsetof(struct _tuple66,f2),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_789={
offsetof(struct _tuple66,f3),(void*)& Cyc__genrep_761};static struct _tuple6 Cyc__gentuple_790={
offsetof(struct _tuple66,f4),(void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_791[
4]={& Cyc__gentuple_787,& Cyc__gentuple_788,& Cyc__gentuple_789,& Cyc__gentuple_790};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_760={4,sizeof(struct _tuple66),{(
void*)((struct _tuple6**)Cyc__genarr_791),(void*)((struct _tuple6**)Cyc__genarr_791),(
void*)((struct _tuple6**)Cyc__genarr_791 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_749;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep;
static char _tmp3AE[7]="PreInc";static struct _tuple7 Cyc__gentuple_750={0,(struct
_dyneither_ptr){_tmp3AE,_tmp3AE,_tmp3AE + 7}};static char _tmp3AF[8]="PostInc";
static struct _tuple7 Cyc__gentuple_751={1,(struct _dyneither_ptr){_tmp3AF,_tmp3AF,
_tmp3AF + 8}};static char _tmp3B0[7]="PreDec";static struct _tuple7 Cyc__gentuple_752={
2,(struct _dyneither_ptr){_tmp3B0,_tmp3B0,_tmp3B0 + 7}};static char _tmp3B1[8]="PostDec";
static struct _tuple7 Cyc__gentuple_753={3,(struct _dyneither_ptr){_tmp3B1,_tmp3B1,
_tmp3B1 + 8}};static struct _tuple7*Cyc__genarr_754[4]={& Cyc__gentuple_750,& Cyc__gentuple_751,&
Cyc__gentuple_752,& Cyc__gentuple_753};static struct _tuple5*Cyc__genarr_755[0]={};
static char _tmp3B3[12]="Incrementor";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep={
5,(struct _dyneither_ptr){_tmp3B3,_tmp3B3,_tmp3B3 + 12},{(void*)((struct _tuple7**)
Cyc__genarr_754),(void*)((struct _tuple7**)Cyc__genarr_754),(void*)((struct
_tuple7**)Cyc__genarr_754 + 4)},{(void*)((struct _tuple5**)Cyc__genarr_755),(void*)((
struct _tuple5**)Cyc__genarr_755),(void*)((struct _tuple5**)Cyc__genarr_755 + 0)}};
struct _tuple67{unsigned int f1;struct Cyc_Absyn_Exp*f2;void*f3;};static struct
_tuple6 Cyc__gentuple_756={offsetof(struct _tuple67,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_757={offsetof(struct _tuple67,f2),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_758={offsetof(struct _tuple67,f3),(void*)& Cyc_Absyn_incrementor_t_rep};
static struct _tuple6*Cyc__genarr_759[3]={& Cyc__gentuple_756,& Cyc__gentuple_757,&
Cyc__gentuple_758};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_749={4,
sizeof(struct _tuple67),{(void*)((struct _tuple6**)Cyc__genarr_759),(void*)((
struct _tuple6**)Cyc__genarr_759),(void*)((struct _tuple6**)Cyc__genarr_759 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_743;struct _tuple68{unsigned int
f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*f4;};static
struct _tuple6 Cyc__gentuple_744={offsetof(struct _tuple68,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_745={offsetof(struct _tuple68,f2),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_746={offsetof(struct _tuple68,f3),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_747={offsetof(struct _tuple68,f4),(void*)& Cyc__genrep_130};
static struct _tuple6*Cyc__genarr_748[4]={& Cyc__gentuple_744,& Cyc__gentuple_745,&
Cyc__gentuple_746,& Cyc__gentuple_747};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_743={
4,sizeof(struct _tuple68),{(void*)((struct _tuple6**)Cyc__genarr_748),(void*)((
struct _tuple6**)Cyc__genarr_748),(void*)((struct _tuple6**)Cyc__genarr_748 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_593;struct _tuple69{unsigned int
f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};static struct _tuple6 Cyc__gentuple_594={
offsetof(struct _tuple69,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_595={
offsetof(struct _tuple69,f2),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_596={
offsetof(struct _tuple69,f3),(void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_597[
3]={& Cyc__gentuple_594,& Cyc__gentuple_595,& Cyc__gentuple_596};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_593={4,sizeof(struct _tuple69),{(void*)((struct _tuple6**)Cyc__genarr_597),(
void*)((struct _tuple6**)Cyc__genarr_597),(void*)((struct _tuple6**)Cyc__genarr_597
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_738;static struct _tuple6
Cyc__gentuple_739={offsetof(struct _tuple58,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_740={offsetof(struct _tuple58,f2),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_741={offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_496};
static struct _tuple6*Cyc__genarr_742[3]={& Cyc__gentuple_739,& Cyc__gentuple_740,&
Cyc__gentuple_741};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_738={4,
sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_742),(void*)((
struct _tuple6**)Cyc__genarr_742),(void*)((struct _tuple6**)Cyc__genarr_742 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_719;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_720;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_call_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_721;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_721={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};static char
_tmp3B9[15]="VarargCallInfo";static struct _dyneither_ptr Cyc__genname_732=(struct
_dyneither_ptr){_tmp3B9,_tmp3B9,_tmp3B9 + 15};static char _tmp3BA[12]="num_varargs";
static struct _tuple5 Cyc__gentuple_728={offsetof(struct Cyc_Absyn_VarargCallInfo,num_varargs),(
struct _dyneither_ptr){_tmp3BA,_tmp3BA,_tmp3BA + 12},(void*)& Cyc__genrep_10};
static char _tmp3BB[10]="injectors";static struct _tuple5 Cyc__gentuple_729={
offsetof(struct Cyc_Absyn_VarargCallInfo,injectors),(struct _dyneither_ptr){
_tmp3BB,_tmp3BB,_tmp3BB + 10},(void*)& Cyc__genrep_265};static char _tmp3BC[4]="vai";
static struct _tuple5 Cyc__gentuple_730={offsetof(struct Cyc_Absyn_VarargCallInfo,vai),(
struct _dyneither_ptr){_tmp3BC,_tmp3BC,_tmp3BC + 4},(void*)& Cyc__genrep_721};
static struct _tuple5*Cyc__genarr_731[3]={& Cyc__gentuple_728,& Cyc__gentuple_729,&
Cyc__gentuple_730};struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_call_info_t_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_732,sizeof(struct Cyc_Absyn_VarargCallInfo),{(
void*)((struct _tuple5**)Cyc__genarr_731),(void*)((struct _tuple5**)Cyc__genarr_731),(
void*)((struct _tuple5**)Cyc__genarr_731 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_720={1,1,(void*)((void*)& Cyc_Absyn_vararg_call_info_t_rep)};struct
_tuple70{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_VarargCallInfo*
f4;};static struct _tuple6 Cyc__gentuple_733={offsetof(struct _tuple70,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_734={offsetof(struct _tuple70,f2),(
void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_735={offsetof(struct
_tuple70,f3),(void*)& Cyc__genrep_496};static struct _tuple6 Cyc__gentuple_736={
offsetof(struct _tuple70,f4),(void*)& Cyc__genrep_720};static struct _tuple6*Cyc__genarr_737[
4]={& Cyc__gentuple_733,& Cyc__gentuple_734,& Cyc__gentuple_735,& Cyc__gentuple_736};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_719={4,sizeof(struct _tuple70),{(
void*)((struct _tuple6**)Cyc__genarr_737),(void*)((struct _tuple6**)Cyc__genarr_737),(
void*)((struct _tuple6**)Cyc__genarr_737 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_714;static struct _tuple6 Cyc__gentuple_715={offsetof(struct _tuple58,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_716={offsetof(struct
_tuple58,f2),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_717={
offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_121};static struct _tuple6*Cyc__genarr_718[
3]={& Cyc__gentuple_715,& Cyc__gentuple_716,& Cyc__gentuple_717};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_714={4,sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_718),(
void*)((struct _tuple6**)Cyc__genarr_718),(void*)((struct _tuple6**)Cyc__genarr_718
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_701;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_coercion_t_rep;static char _tmp3C1[17]="Unknown_coercion";static struct
_tuple7 Cyc__gentuple_702={0,(struct _dyneither_ptr){_tmp3C1,_tmp3C1,_tmp3C1 + 17}};
static char _tmp3C2[12]="No_coercion";static struct _tuple7 Cyc__gentuple_703={1,(
struct _dyneither_ptr){_tmp3C2,_tmp3C2,_tmp3C2 + 12}};static char _tmp3C3[16]="NonNull_to_Null";
static struct _tuple7 Cyc__gentuple_704={2,(struct _dyneither_ptr){_tmp3C3,_tmp3C3,
_tmp3C3 + 16}};static char _tmp3C4[15]="Other_coercion";static struct _tuple7 Cyc__gentuple_705={
3,(struct _dyneither_ptr){_tmp3C4,_tmp3C4,_tmp3C4 + 15}};static struct _tuple7*Cyc__genarr_706[
4]={& Cyc__gentuple_702,& Cyc__gentuple_703,& Cyc__gentuple_704,& Cyc__gentuple_705};
static struct _tuple5*Cyc__genarr_707[0]={};static char _tmp3C6[9]="Coercion";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_coercion_t_rep={5,(struct _dyneither_ptr){
_tmp3C6,_tmp3C6,_tmp3C6 + 9},{(void*)((struct _tuple7**)Cyc__genarr_706),(void*)((
struct _tuple7**)Cyc__genarr_706),(void*)((struct _tuple7**)Cyc__genarr_706 + 4)},{(
void*)((struct _tuple5**)Cyc__genarr_707),(void*)((struct _tuple5**)Cyc__genarr_707),(
void*)((struct _tuple5**)Cyc__genarr_707 + 0)}};struct _tuple71{unsigned int f1;void*
f2;struct Cyc_Absyn_Exp*f3;int f4;void*f5;};static struct _tuple6 Cyc__gentuple_708={
offsetof(struct _tuple71,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_709={
offsetof(struct _tuple71,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_710={offsetof(struct _tuple71,f3),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_711={offsetof(struct _tuple71,f4),(void*)& Cyc__genrep_10};
static struct _tuple6 Cyc__gentuple_712={offsetof(struct _tuple71,f5),(void*)& Cyc_Absyn_coercion_t_rep};
static struct _tuple6*Cyc__genarr_713[5]={& Cyc__gentuple_708,& Cyc__gentuple_709,&
Cyc__gentuple_710,& Cyc__gentuple_711,& Cyc__gentuple_712};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_701={4,sizeof(struct _tuple71),{(void*)((struct _tuple6**)Cyc__genarr_713),(
void*)((struct _tuple6**)Cyc__genarr_713),(void*)((struct _tuple6**)Cyc__genarr_713
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_696;static struct _tuple6
Cyc__gentuple_697={offsetof(struct _tuple69,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_698={offsetof(struct _tuple69,f2),(void*)& Cyc__genrep_145};
static struct _tuple6 Cyc__gentuple_699={offsetof(struct _tuple69,f3),(void*)& Cyc__genrep_130};
static struct _tuple6*Cyc__genarr_700[3]={& Cyc__gentuple_697,& Cyc__gentuple_698,&
Cyc__gentuple_699};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_696={4,
sizeof(struct _tuple69),{(void*)((struct _tuple6**)Cyc__genarr_700),(void*)((
struct _tuple6**)Cyc__genarr_700),(void*)((struct _tuple6**)Cyc__genarr_700 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_116;static struct _tuple6 Cyc__gentuple_117={
offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_118={
offsetof(struct _tuple6,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_119[2]={& Cyc__gentuple_117,& Cyc__gentuple_118};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_116={4,sizeof(struct _tuple6),{(void*)((
struct _tuple6**)Cyc__genarr_119),(void*)((struct _tuple6**)Cyc__genarr_119),(void*)((
struct _tuple6**)Cyc__genarr_119 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_683;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_offsetof_field_t_rep;extern struct
Cyc_Typerep_Tuple_struct Cyc__genrep_685;struct _tuple72{unsigned int f1;
unsigned int f2;};static struct _tuple6 Cyc__gentuple_686={offsetof(struct _tuple72,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_687={offsetof(struct
_tuple72,f2),(void*)& Cyc__genrep_24};static struct _tuple6*Cyc__genarr_688[2]={&
Cyc__gentuple_686,& Cyc__gentuple_687};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_685={
4,sizeof(struct _tuple72),{(void*)((struct _tuple6**)Cyc__genarr_688),(void*)((
struct _tuple6**)Cyc__genarr_688),(void*)((struct _tuple6**)Cyc__genarr_688 + 2)}};
static struct _tuple7*Cyc__genarr_684[0]={};static char _tmp3CB[12]="StructField";
static struct _tuple5 Cyc__gentuple_689={0,(struct _dyneither_ptr){_tmp3CB,_tmp3CB,
_tmp3CB + 12},(void*)& Cyc__genrep_336};static char _tmp3CC[11]="TupleIndex";static
struct _tuple5 Cyc__gentuple_690={1,(struct _dyneither_ptr){_tmp3CC,_tmp3CC,_tmp3CC
+ 11},(void*)& Cyc__genrep_685};static struct _tuple5*Cyc__genarr_691[2]={& Cyc__gentuple_689,&
Cyc__gentuple_690};static char _tmp3CE[14]="OffsetofField";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_offsetof_field_t_rep={5,(struct _dyneither_ptr){_tmp3CE,_tmp3CE,_tmp3CE
+ 14},{(void*)((struct _tuple7**)Cyc__genarr_684),(void*)((struct _tuple7**)Cyc__genarr_684),(
void*)((struct _tuple7**)Cyc__genarr_684 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_691),(
void*)((struct _tuple5**)Cyc__genarr_691),(void*)((struct _tuple5**)Cyc__genarr_691
+ 2)}};static struct _tuple6 Cyc__gentuple_692={offsetof(struct _tuple28,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_693={offsetof(struct _tuple28,f2),(
void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_694={
offsetof(struct _tuple28,f3),(void*)& Cyc_Absyn_offsetof_field_t_rep};static struct
_tuple6*Cyc__genarr_695[3]={& Cyc__gentuple_692,& Cyc__gentuple_693,& Cyc__gentuple_694};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_683={4,sizeof(struct _tuple28),{(
void*)((struct _tuple6**)Cyc__genarr_695),(void*)((struct _tuple6**)Cyc__genarr_695),(
void*)((struct _tuple6**)Cyc__genarr_695 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_678;struct _tuple73{unsigned int f1;struct Cyc_List_List*f2;void*f3;};
static struct _tuple6 Cyc__gentuple_679={offsetof(struct _tuple73,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_680={offsetof(struct _tuple73,f2),(void*)& Cyc__genrep_273};
static struct _tuple6 Cyc__gentuple_681={offsetof(struct _tuple73,f3),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_682[3]={& Cyc__gentuple_679,&
Cyc__gentuple_680,& Cyc__gentuple_681};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_678={
4,sizeof(struct _tuple73),{(void*)((struct _tuple6**)Cyc__genarr_682),(void*)((
struct _tuple6**)Cyc__genarr_682),(void*)((struct _tuple6**)Cyc__genarr_682 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_673;struct _tuple74{unsigned int
f1;struct Cyc_Absyn_Exp*f2;struct _dyneither_ptr*f3;};static struct _tuple6 Cyc__gentuple_674={
offsetof(struct _tuple74,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_675={
offsetof(struct _tuple74,f2),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_676={
offsetof(struct _tuple74,f3),(void*)& Cyc__genrep_13};static struct _tuple6*Cyc__genarr_677[
3]={& Cyc__gentuple_674,& Cyc__gentuple_675,& Cyc__gentuple_676};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_673={4,sizeof(struct _tuple74),{(void*)((struct _tuple6**)Cyc__genarr_677),(
void*)((struct _tuple6**)Cyc__genarr_677),(void*)((struct _tuple6**)Cyc__genarr_677
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_669;static struct _tuple6
Cyc__gentuple_670={offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_671={offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_496};
static struct _tuple6*Cyc__genarr_672[2]={& Cyc__gentuple_670,& Cyc__gentuple_671};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_669={4,sizeof(struct _tuple14),{(
void*)((struct _tuple6**)Cyc__genarr_672),(void*)((struct _tuple6**)Cyc__genarr_672),(
void*)((struct _tuple6**)Cyc__genarr_672 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_654;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_655;extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_656;static struct _tuple6 Cyc__gentuple_661={
offsetof(struct _tuple1,f1),(void*)& Cyc__genrep_657};static struct _tuple6 Cyc__gentuple_662={
offsetof(struct _tuple1,f2),(void*)& Cyc_Absyn_tqual_t_rep};static struct _tuple6 Cyc__gentuple_663={
offsetof(struct _tuple1,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_664[3]={& Cyc__gentuple_661,& Cyc__gentuple_662,& Cyc__gentuple_663};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_656={4,sizeof(struct _tuple1),{(
void*)((struct _tuple6**)Cyc__genarr_664),(void*)((struct _tuple6**)Cyc__genarr_664),(
void*)((struct _tuple6**)Cyc__genarr_664 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_655={1,1,(void*)((void*)& Cyc__genrep_656)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_575;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_576;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_577;static struct _tuple6 Cyc__gentuple_578={offsetof(struct _tuple13,f1),(
void*)& Cyc__genrep_334};static struct _tuple6 Cyc__gentuple_579={offsetof(struct
_tuple13,f2),(void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_580[2]={&
Cyc__gentuple_578,& Cyc__gentuple_579};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_577={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_580),(void*)((
struct _tuple6**)Cyc__genarr_580),(void*)((struct _tuple6**)Cyc__genarr_580 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_576={1,1,(void*)((void*)& Cyc__genrep_577)};
static char _tmp3D7[5]="List";static struct _dyneither_ptr Cyc__genname_584=(struct
_dyneither_ptr){_tmp3D7,_tmp3D7,_tmp3D7 + 5};static char _tmp3D8[3]="hd";static
struct _tuple5 Cyc__gentuple_581={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp3D8,_tmp3D8,_tmp3D8 + 3},(void*)& Cyc__genrep_576};static char
_tmp3D9[3]="tl";static struct _tuple5 Cyc__gentuple_582={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp3D9,_tmp3D9,_tmp3D9 + 3},(void*)& Cyc__genrep_575};
static struct _tuple5*Cyc__genarr_583[2]={& Cyc__gentuple_581,& Cyc__gentuple_582};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_584,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_583),(void*)((struct _tuple5**)Cyc__genarr_583),(void*)((
struct _tuple5**)Cyc__genarr_583 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_575={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep)};
struct _tuple75{unsigned int f1;struct _tuple1*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_665={offsetof(struct _tuple75,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_666={offsetof(struct _tuple75,f2),(void*)& Cyc__genrep_655};
static struct _tuple6 Cyc__gentuple_667={offsetof(struct _tuple75,f3),(void*)& Cyc__genrep_575};
static struct _tuple6*Cyc__genarr_668[3]={& Cyc__gentuple_665,& Cyc__gentuple_666,&
Cyc__gentuple_667};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_654={4,
sizeof(struct _tuple75),{(void*)((struct _tuple6**)Cyc__genarr_668),(void*)((
struct _tuple6**)Cyc__genarr_668),(void*)((struct _tuple6**)Cyc__genarr_668 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_650;static struct _tuple6 Cyc__gentuple_651={
offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_652={
offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_575};static struct _tuple6*Cyc__genarr_653[
2]={& Cyc__gentuple_651,& Cyc__gentuple_652};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_650={4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_653),(
void*)((struct _tuple6**)Cyc__genarr_653),(void*)((struct _tuple6**)Cyc__genarr_653
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_643;struct _tuple76{
unsigned int f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*
f4;int f5;};static struct _tuple6 Cyc__gentuple_644={offsetof(struct _tuple76,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_645={offsetof(struct
_tuple76,f2),(void*)& Cyc__genrep_148};static struct _tuple6 Cyc__gentuple_646={
offsetof(struct _tuple76,f3),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_647={
offsetof(struct _tuple76,f4),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_648={
offsetof(struct _tuple76,f5),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_649[
5]={& Cyc__gentuple_644,& Cyc__gentuple_645,& Cyc__gentuple_646,& Cyc__gentuple_647,&
Cyc__gentuple_648};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_643={4,
sizeof(struct _tuple76),{(void*)((struct _tuple6**)Cyc__genarr_649),(void*)((
struct _tuple6**)Cyc__genarr_649),(void*)((struct _tuple6**)Cyc__genarr_649 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_635;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_636;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_636={1,1,(void*)((
void*)& Cyc_struct_Absyn_Aggrdecl_rep)};struct _tuple77{unsigned int f1;struct
_tuple0*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_Absyn_Aggrdecl*
f5;};static struct _tuple6 Cyc__gentuple_637={offsetof(struct _tuple77,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_638={offsetof(struct _tuple77,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_639={offsetof(struct
_tuple77,f3),(void*)& Cyc__genrep_121};static struct _tuple6 Cyc__gentuple_640={
offsetof(struct _tuple77,f4),(void*)& Cyc__genrep_575};static struct _tuple6 Cyc__gentuple_641={
offsetof(struct _tuple77,f5),(void*)& Cyc__genrep_636};static struct _tuple6*Cyc__genarr_642[
5]={& Cyc__gentuple_637,& Cyc__gentuple_638,& Cyc__gentuple_639,& Cyc__gentuple_640,&
Cyc__gentuple_641};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_635={4,
sizeof(struct _tuple77),{(void*)((struct _tuple6**)Cyc__genarr_642),(void*)((
struct _tuple6**)Cyc__genarr_642),(void*)((struct _tuple6**)Cyc__genarr_642 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_630;static struct _tuple6 Cyc__gentuple_631={
offsetof(struct _tuple65,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_632={
offsetof(struct _tuple65,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_633={offsetof(struct _tuple65,f3),(void*)& Cyc__genrep_575};
static struct _tuple6*Cyc__genarr_634[3]={& Cyc__gentuple_631,& Cyc__gentuple_632,&
Cyc__gentuple_633};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_630={4,
sizeof(struct _tuple65),{(void*)((struct _tuple6**)Cyc__genarr_634),(void*)((
struct _tuple6**)Cyc__genarr_634),(void*)((struct _tuple6**)Cyc__genarr_634 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_624;struct _tuple78{unsigned int
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Tuniondecl*f3;struct Cyc_Absyn_Tunionfield*
f4;};static struct _tuple6 Cyc__gentuple_625={offsetof(struct _tuple78,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_626={offsetof(struct _tuple78,f2),(
void*)& Cyc__genrep_496};static struct _tuple6 Cyc__gentuple_627={offsetof(struct
_tuple78,f3),(void*)((void*)& Cyc__genrep_263)};static struct _tuple6 Cyc__gentuple_628={
offsetof(struct _tuple78,f4),(void*)& Cyc__genrep_246};static struct _tuple6*Cyc__genarr_629[
4]={& Cyc__gentuple_625,& Cyc__gentuple_626,& Cyc__gentuple_627,& Cyc__gentuple_628};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_624={4,sizeof(struct _tuple78),{(
void*)((struct _tuple6**)Cyc__genarr_629),(void*)((struct _tuple6**)Cyc__genarr_629),(
void*)((struct _tuple6**)Cyc__genarr_629 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_617;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_618;static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_618={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_611;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_611={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};struct
_tuple79{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_619={offsetof(struct _tuple79,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_620={offsetof(struct _tuple79,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_621={offsetof(struct
_tuple79,f3),(void*)& Cyc__genrep_618};static struct _tuple6 Cyc__gentuple_622={
offsetof(struct _tuple79,f4),(void*)& Cyc__genrep_611};static struct _tuple6*Cyc__genarr_623[
4]={& Cyc__gentuple_619,& Cyc__gentuple_620,& Cyc__gentuple_621,& Cyc__gentuple_622};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_617={4,sizeof(struct _tuple79),{(
void*)((struct _tuple6**)Cyc__genarr_623),(void*)((struct _tuple6**)Cyc__genarr_623),(
void*)((struct _tuple6**)Cyc__genarr_623 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_610;struct _tuple80{unsigned int f1;struct _tuple0*f2;void*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_612={offsetof(struct _tuple80,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_613={offsetof(struct _tuple80,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_614={offsetof(struct
_tuple80,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_615={
offsetof(struct _tuple80,f4),(void*)& Cyc__genrep_611};static struct _tuple6*Cyc__genarr_616[
4]={& Cyc__gentuple_612,& Cyc__gentuple_613,& Cyc__gentuple_614,& Cyc__gentuple_615};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_610={4,sizeof(struct _tuple80),{(
void*)((struct _tuple6**)Cyc__genarr_616),(void*)((struct _tuple6**)Cyc__genarr_616),(
void*)((struct _tuple6**)Cyc__genarr_616 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_598;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_599;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_599={1,1,(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp3E8[11]="MallocInfo";
static struct _dyneither_ptr Cyc__genname_606=(struct _dyneither_ptr){_tmp3E8,
_tmp3E8,_tmp3E8 + 11};static char _tmp3E9[10]="is_calloc";static struct _tuple5 Cyc__gentuple_600={
offsetof(struct Cyc_Absyn_MallocInfo,is_calloc),(struct _dyneither_ptr){_tmp3E9,
_tmp3E9,_tmp3E9 + 10},(void*)& Cyc__genrep_10};static char _tmp3EA[4]="rgn";static
struct _tuple5 Cyc__gentuple_601={offsetof(struct Cyc_Absyn_MallocInfo,rgn),(struct
_dyneither_ptr){_tmp3EA,_tmp3EA,_tmp3EA + 4},(void*)& Cyc__genrep_145};static char
_tmp3EB[9]="elt_type";static struct _tuple5 Cyc__gentuple_602={offsetof(struct Cyc_Absyn_MallocInfo,elt_type),(
struct _dyneither_ptr){_tmp3EB,_tmp3EB,_tmp3EB + 9},(void*)& Cyc__genrep_599};
static char _tmp3EC[9]="num_elts";static struct _tuple5 Cyc__gentuple_603={offsetof(
struct Cyc_Absyn_MallocInfo,num_elts),(struct _dyneither_ptr){_tmp3EC,_tmp3EC,
_tmp3EC + 9},(void*)& Cyc__genrep_130};static char _tmp3ED[11]="fat_result";static
struct _tuple5 Cyc__gentuple_604={offsetof(struct Cyc_Absyn_MallocInfo,fat_result),(
struct _dyneither_ptr){_tmp3ED,_tmp3ED,_tmp3ED + 11},(void*)& Cyc__genrep_10};
static struct _tuple5*Cyc__genarr_605[5]={& Cyc__gentuple_600,& Cyc__gentuple_601,&
Cyc__gentuple_602,& Cyc__gentuple_603,& Cyc__gentuple_604};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_malloc_info_t_rep={3,(struct _dyneither_ptr*)& Cyc__genname_606,sizeof(
struct Cyc_Absyn_MallocInfo),{(void*)((struct _tuple5**)Cyc__genarr_605),(void*)((
struct _tuple5**)Cyc__genarr_605),(void*)((struct _tuple5**)Cyc__genarr_605 + 5)}};
struct _tuple81{unsigned int f1;struct Cyc_Absyn_MallocInfo f2;};static struct _tuple6
Cyc__gentuple_607={offsetof(struct _tuple81,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_608={offsetof(struct _tuple81,f2),(void*)& Cyc_Absyn_malloc_info_t_rep};
static struct _tuple6*Cyc__genarr_609[2]={& Cyc__gentuple_607,& Cyc__gentuple_608};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_598={4,sizeof(struct _tuple81),{(
void*)((struct _tuple6**)Cyc__genarr_609),(void*)((struct _tuple6**)Cyc__genarr_609),(
void*)((struct _tuple6**)Cyc__genarr_609 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_574;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_585;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep;
static char _tmp3F0[4]="Opt";static struct _dyneither_ptr Cyc__genname_588=(struct
_dyneither_ptr){_tmp3F0,_tmp3F0,_tmp3F0 + 4};static char _tmp3F1[2]="v";static
struct _tuple5 Cyc__gentuple_586={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp3F1,_tmp3F1,_tmp3F1 + 2},(void*)& Cyc__genrep_11};static struct
_tuple5*Cyc__genarr_587[1]={& Cyc__gentuple_586};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_588,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_587),(void*)((
struct _tuple5**)Cyc__genarr_587),(void*)((struct _tuple5**)Cyc__genarr_587 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_585={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep)};
struct _tuple82{unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_589={offsetof(struct _tuple82,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_590={offsetof(struct _tuple82,f2),(void*)& Cyc__genrep_585};
static struct _tuple6 Cyc__gentuple_591={offsetof(struct _tuple82,f3),(void*)& Cyc__genrep_575};
static struct _tuple6*Cyc__genarr_592[3]={& Cyc__gentuple_589,& Cyc__gentuple_590,&
Cyc__gentuple_591};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_574={4,
sizeof(struct _tuple82),{(void*)((struct _tuple6**)Cyc__genarr_592),(void*)((
struct _tuple6**)Cyc__genarr_592),(void*)((struct _tuple6**)Cyc__genarr_592 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_133;static struct _tuple6 Cyc__gentuple_571={
offsetof(struct _tuple39,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_572={
offsetof(struct _tuple39,f2),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_573[
2]={& Cyc__gentuple_571,& Cyc__gentuple_572};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_133={4,sizeof(struct _tuple39),{(void*)((struct _tuple6**)Cyc__genarr_573),(
void*)((struct _tuple6**)Cyc__genarr_573),(void*)((struct _tuple6**)Cyc__genarr_573
+ 2)}};static struct _tuple7*Cyc__genarr_132[0]={};static char _tmp3F6[8]="Const_e";
static struct _tuple5 Cyc__gentuple_879={0,(struct _dyneither_ptr){_tmp3F6,_tmp3F6,
_tmp3F6 + 8},(void*)& Cyc__genrep_849};static char _tmp3F7[6]="Var_e";static struct
_tuple5 Cyc__gentuple_880={1,(struct _dyneither_ptr){_tmp3F7,_tmp3F7,_tmp3F7 + 6},(
void*)& Cyc__genrep_797};static char _tmp3F8[12]="UnknownId_e";static struct _tuple5
Cyc__gentuple_881={2,(struct _dyneither_ptr){_tmp3F8,_tmp3F8,_tmp3F8 + 12},(void*)&
Cyc__genrep_196};static char _tmp3F9[9]="Primop_e";static struct _tuple5 Cyc__gentuple_882={
3,(struct _dyneither_ptr){_tmp3F9,_tmp3F9,_tmp3F9 + 9},(void*)& Cyc__genrep_792};
static char _tmp3FA[11]="AssignOp_e";static struct _tuple5 Cyc__gentuple_883={4,(
struct _dyneither_ptr){_tmp3FA,_tmp3FA,_tmp3FA + 11},(void*)& Cyc__genrep_760};
static char _tmp3FB[12]="Increment_e";static struct _tuple5 Cyc__gentuple_884={5,(
struct _dyneither_ptr){_tmp3FB,_tmp3FB,_tmp3FB + 12},(void*)& Cyc__genrep_749};
static char _tmp3FC[14]="Conditional_e";static struct _tuple5 Cyc__gentuple_885={6,(
struct _dyneither_ptr){_tmp3FC,_tmp3FC,_tmp3FC + 14},(void*)& Cyc__genrep_743};
static char _tmp3FD[6]="And_e";static struct _tuple5 Cyc__gentuple_886={7,(struct
_dyneither_ptr){_tmp3FD,_tmp3FD,_tmp3FD + 6},(void*)& Cyc__genrep_593};static char
_tmp3FE[5]="Or_e";static struct _tuple5 Cyc__gentuple_887={8,(struct _dyneither_ptr){
_tmp3FE,_tmp3FE,_tmp3FE + 5},(void*)& Cyc__genrep_593};static char _tmp3FF[9]="SeqExp_e";
static struct _tuple5 Cyc__gentuple_888={9,(struct _dyneither_ptr){_tmp3FF,_tmp3FF,
_tmp3FF + 9},(void*)& Cyc__genrep_593};static char _tmp400[14]="UnknownCall_e";
static struct _tuple5 Cyc__gentuple_889={10,(struct _dyneither_ptr){_tmp400,_tmp400,
_tmp400 + 14},(void*)& Cyc__genrep_738};static char _tmp401[9]="FnCall_e";static
struct _tuple5 Cyc__gentuple_890={11,(struct _dyneither_ptr){_tmp401,_tmp401,
_tmp401 + 9},(void*)& Cyc__genrep_719};static char _tmp402[8]="Throw_e";static struct
_tuple5 Cyc__gentuple_891={12,(struct _dyneither_ptr){_tmp402,_tmp402,_tmp402 + 8},(
void*)& Cyc__genrep_129};static char _tmp403[16]="NoInstantiate_e";static struct
_tuple5 Cyc__gentuple_892={13,(struct _dyneither_ptr){_tmp403,_tmp403,_tmp403 + 16},(
void*)& Cyc__genrep_129};static char _tmp404[14]="Instantiate_e";static struct
_tuple5 Cyc__gentuple_893={14,(struct _dyneither_ptr){_tmp404,_tmp404,_tmp404 + 14},(
void*)& Cyc__genrep_714};static char _tmp405[7]="Cast_e";static struct _tuple5 Cyc__gentuple_894={
15,(struct _dyneither_ptr){_tmp405,_tmp405,_tmp405 + 7},(void*)& Cyc__genrep_701};
static char _tmp406[10]="Address_e";static struct _tuple5 Cyc__gentuple_895={16,(
struct _dyneither_ptr){_tmp406,_tmp406,_tmp406 + 10},(void*)& Cyc__genrep_129};
static char _tmp407[6]="New_e";static struct _tuple5 Cyc__gentuple_896={17,(struct
_dyneither_ptr){_tmp407,_tmp407,_tmp407 + 6},(void*)& Cyc__genrep_696};static char
_tmp408[12]="Sizeoftyp_e";static struct _tuple5 Cyc__gentuple_897={18,(struct
_dyneither_ptr){_tmp408,_tmp408,_tmp408 + 12},(void*)& Cyc__genrep_116};static char
_tmp409[12]="Sizeofexp_e";static struct _tuple5 Cyc__gentuple_898={19,(struct
_dyneither_ptr){_tmp409,_tmp409,_tmp409 + 12},(void*)& Cyc__genrep_129};static char
_tmp40A[11]="Offsetof_e";static struct _tuple5 Cyc__gentuple_899={20,(struct
_dyneither_ptr){_tmp40A,_tmp40A,_tmp40A + 11},(void*)& Cyc__genrep_683};static char
_tmp40B[9]="Gentyp_e";static struct _tuple5 Cyc__gentuple_900={21,(struct
_dyneither_ptr){_tmp40B,_tmp40B,_tmp40B + 9},(void*)& Cyc__genrep_678};static char
_tmp40C[8]="Deref_e";static struct _tuple5 Cyc__gentuple_901={22,(struct
_dyneither_ptr){_tmp40C,_tmp40C,_tmp40C + 8},(void*)& Cyc__genrep_129};static char
_tmp40D[13]="AggrMember_e";static struct _tuple5 Cyc__gentuple_902={23,(struct
_dyneither_ptr){_tmp40D,_tmp40D,_tmp40D + 13},(void*)& Cyc__genrep_673};static char
_tmp40E[12]="AggrArrow_e";static struct _tuple5 Cyc__gentuple_903={24,(struct
_dyneither_ptr){_tmp40E,_tmp40E,_tmp40E + 12},(void*)& Cyc__genrep_673};static char
_tmp40F[12]="Subscript_e";static struct _tuple5 Cyc__gentuple_904={25,(struct
_dyneither_ptr){_tmp40F,_tmp40F,_tmp40F + 12},(void*)& Cyc__genrep_593};static char
_tmp410[8]="Tuple_e";static struct _tuple5 Cyc__gentuple_905={26,(struct
_dyneither_ptr){_tmp410,_tmp410,_tmp410 + 8},(void*)& Cyc__genrep_669};static char
_tmp411[14]="CompoundLit_e";static struct _tuple5 Cyc__gentuple_906={27,(struct
_dyneither_ptr){_tmp411,_tmp411,_tmp411 + 14},(void*)& Cyc__genrep_654};static char
_tmp412[8]="Array_e";static struct _tuple5 Cyc__gentuple_907={28,(struct
_dyneither_ptr){_tmp412,_tmp412,_tmp412 + 8},(void*)& Cyc__genrep_650};static char
_tmp413[16]="Comprehension_e";static struct _tuple5 Cyc__gentuple_908={29,(struct
_dyneither_ptr){_tmp413,_tmp413,_tmp413 + 16},(void*)& Cyc__genrep_643};static char
_tmp414[9]="Struct_e";static struct _tuple5 Cyc__gentuple_909={30,(struct
_dyneither_ptr){_tmp414,_tmp414,_tmp414 + 9},(void*)& Cyc__genrep_635};static char
_tmp415[13]="AnonStruct_e";static struct _tuple5 Cyc__gentuple_910={31,(struct
_dyneither_ptr){_tmp415,_tmp415,_tmp415 + 13},(void*)& Cyc__genrep_630};static char
_tmp416[9]="Tunion_e";static struct _tuple5 Cyc__gentuple_911={32,(struct
_dyneither_ptr){_tmp416,_tmp416,_tmp416 + 9},(void*)& Cyc__genrep_624};static char
_tmp417[7]="Enum_e";static struct _tuple5 Cyc__gentuple_912={33,(struct
_dyneither_ptr){_tmp417,_tmp417,_tmp417 + 7},(void*)& Cyc__genrep_617};static char
_tmp418[11]="AnonEnum_e";static struct _tuple5 Cyc__gentuple_913={34,(struct
_dyneither_ptr){_tmp418,_tmp418,_tmp418 + 11},(void*)& Cyc__genrep_610};static char
_tmp419[9]="Malloc_e";static struct _tuple5 Cyc__gentuple_914={35,(struct
_dyneither_ptr){_tmp419,_tmp419,_tmp419 + 9},(void*)& Cyc__genrep_598};static char
_tmp41A[7]="Swap_e";static struct _tuple5 Cyc__gentuple_915={36,(struct
_dyneither_ptr){_tmp41A,_tmp41A,_tmp41A + 7},(void*)& Cyc__genrep_593};static char
_tmp41B[16]="UnresolvedMem_e";static struct _tuple5 Cyc__gentuple_916={37,(struct
_dyneither_ptr){_tmp41B,_tmp41B,_tmp41B + 16},(void*)& Cyc__genrep_574};static char
_tmp41C[10]="StmtExp_e";static struct _tuple5 Cyc__gentuple_917={38,(struct
_dyneither_ptr){_tmp41C,_tmp41C,_tmp41C + 10},(void*)& Cyc__genrep_133};static char
_tmp41D[10]="Valueof_e";static struct _tuple5 Cyc__gentuple_918={39,(struct
_dyneither_ptr){_tmp41D,_tmp41D,_tmp41D + 10},(void*)& Cyc__genrep_116};static
struct _tuple5*Cyc__genarr_919[40]={& Cyc__gentuple_879,& Cyc__gentuple_880,& Cyc__gentuple_881,&
Cyc__gentuple_882,& Cyc__gentuple_883,& Cyc__gentuple_884,& Cyc__gentuple_885,& Cyc__gentuple_886,&
Cyc__gentuple_887,& Cyc__gentuple_888,& Cyc__gentuple_889,& Cyc__gentuple_890,& Cyc__gentuple_891,&
Cyc__gentuple_892,& Cyc__gentuple_893,& Cyc__gentuple_894,& Cyc__gentuple_895,& Cyc__gentuple_896,&
Cyc__gentuple_897,& Cyc__gentuple_898,& Cyc__gentuple_899,& Cyc__gentuple_900,& Cyc__gentuple_901,&
Cyc__gentuple_902,& Cyc__gentuple_903,& Cyc__gentuple_904,& Cyc__gentuple_905,& Cyc__gentuple_906,&
Cyc__gentuple_907,& Cyc__gentuple_908,& Cyc__gentuple_909,& Cyc__gentuple_910,& Cyc__gentuple_911,&
Cyc__gentuple_912,& Cyc__gentuple_913,& Cyc__gentuple_914,& Cyc__gentuple_915,& Cyc__gentuple_916,&
Cyc__gentuple_917,& Cyc__gentuple_918};static char _tmp41F[8]="Raw_exp";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_exp_t_rep={5,(struct _dyneither_ptr){_tmp41F,_tmp41F,_tmp41F + 8},{(
void*)((struct _tuple7**)Cyc__genarr_132),(void*)((struct _tuple7**)Cyc__genarr_132),(
void*)((struct _tuple7**)Cyc__genarr_132 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_919),(
void*)((struct _tuple5**)Cyc__genarr_919),(void*)((struct _tuple5**)Cyc__genarr_919
+ 40)}};static char _tmp420[4]="Exp";static struct _dyneither_ptr Cyc__genname_925=(
struct _dyneither_ptr){_tmp420,_tmp420,_tmp420 + 4};static char _tmp421[5]="topt";
static struct _tuple5 Cyc__gentuple_920={offsetof(struct Cyc_Absyn_Exp,topt),(struct
_dyneither_ptr){_tmp421,_tmp421,_tmp421 + 5},(void*)& Cyc__genrep_110};static char
_tmp422[2]="r";static struct _tuple5 Cyc__gentuple_921={offsetof(struct Cyc_Absyn_Exp,r),(
struct _dyneither_ptr){_tmp422,_tmp422,_tmp422 + 2},(void*)& Cyc_Absyn_raw_exp_t_rep};
static char _tmp423[4]="loc";static struct _tuple5 Cyc__gentuple_922={offsetof(struct
Cyc_Absyn_Exp,loc),(struct _dyneither_ptr){_tmp423,_tmp423,_tmp423 + 4},(void*)&
Cyc__genrep_2};static char _tmp424[6]="annot";static struct _tuple5 Cyc__gentuple_923={
offsetof(struct Cyc_Absyn_Exp,annot),(struct _dyneither_ptr){_tmp424,_tmp424,
_tmp424 + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};static struct _tuple5*Cyc__genarr_924[
4]={& Cyc__gentuple_920,& Cyc__gentuple_921,& Cyc__gentuple_922,& Cyc__gentuple_923};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Exp_rep={3,(struct _dyneither_ptr*)&
Cyc__genname_925,sizeof(struct Cyc_Absyn_Exp),{(void*)((struct _tuple5**)Cyc__genarr_924),(
void*)((struct _tuple5**)Cyc__genarr_924),(void*)((struct _tuple5**)Cyc__genarr_924
+ 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_145={1,1,(void*)((void*)&
Cyc_struct_Absyn_Exp_rep)};static char _tmp427[10]="ArrayInfo";static struct
_dyneither_ptr Cyc__genname_1003=(struct _dyneither_ptr){_tmp427,_tmp427,_tmp427 + 
10};static char _tmp428[9]="elt_type";static struct _tuple5 Cyc__gentuple_997={
offsetof(struct Cyc_Absyn_ArrayInfo,elt_type),(struct _dyneither_ptr){_tmp428,
_tmp428,_tmp428 + 9},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp429[3]="tq";
static struct _tuple5 Cyc__gentuple_998={offsetof(struct Cyc_Absyn_ArrayInfo,tq),(
struct _dyneither_ptr){_tmp429,_tmp429,_tmp429 + 3},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp42A[9]="num_elts";static struct _tuple5 Cyc__gentuple_999={offsetof(
struct Cyc_Absyn_ArrayInfo,num_elts),(struct _dyneither_ptr){_tmp42A,_tmp42A,
_tmp42A + 9},(void*)& Cyc__genrep_145};static char _tmp42B[10]="zero_term";static
struct _tuple5 Cyc__gentuple_1000={offsetof(struct Cyc_Absyn_ArrayInfo,zero_term),(
struct _dyneither_ptr){_tmp42B,_tmp42B,_tmp42B + 10},(void*)& Cyc__genrep_984};
static char _tmp42C[7]="zt_loc";static struct _tuple5 Cyc__gentuple_1001={offsetof(
struct Cyc_Absyn_ArrayInfo,zt_loc),(struct _dyneither_ptr){_tmp42C,_tmp42C,_tmp42C
+ 7},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_1002[5]={& Cyc__gentuple_997,&
Cyc__gentuple_998,& Cyc__gentuple_999,& Cyc__gentuple_1000,& Cyc__gentuple_1001};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_array_info_t_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_1003,sizeof(struct Cyc_Absyn_ArrayInfo),{(void*)((
struct _tuple5**)Cyc__genarr_1002),(void*)((struct _tuple5**)Cyc__genarr_1002),(
void*)((struct _tuple5**)Cyc__genarr_1002 + 5)}};struct _tuple83{unsigned int f1;
struct Cyc_Absyn_ArrayInfo f2;};static struct _tuple6 Cyc__gentuple_1004={offsetof(
struct _tuple83,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1005={
offsetof(struct _tuple83,f2),(void*)& Cyc_Absyn_array_info_t_rep};static struct
_tuple6*Cyc__genarr_1006[2]={& Cyc__gentuple_1004,& Cyc__gentuple_1005};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_983={4,sizeof(struct _tuple83),{(void*)((
struct _tuple6**)Cyc__genarr_1006),(void*)((struct _tuple6**)Cyc__genarr_1006),(
void*)((struct _tuple6**)Cyc__genarr_1006 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_964;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_fn_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_965;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep;
static char _tmp42F[5]="List";static struct _dyneither_ptr Cyc__genname_969=(struct
_dyneither_ptr){_tmp42F,_tmp42F,_tmp42F + 5};static char _tmp430[3]="hd";static
struct _tuple5 Cyc__gentuple_966={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp430,_tmp430,_tmp430 + 3},(void*)& Cyc__genrep_655};static char
_tmp431[3]="tl";static struct _tuple5 Cyc__gentuple_967={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp431,_tmp431,_tmp431 + 3},(void*)& Cyc__genrep_965};
static struct _tuple5*Cyc__genarr_968[2]={& Cyc__gentuple_966,& Cyc__gentuple_967};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_969,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_968),(void*)((struct _tuple5**)Cyc__genarr_968),(void*)((
struct _tuple5**)Cyc__genarr_968 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_965={
1,1,(void*)((void*)& Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep)};
static char _tmp434[7]="FnInfo";static struct _dyneither_ptr Cyc__genname_979=(struct
_dyneither_ptr){_tmp434,_tmp434,_tmp434 + 7};static char _tmp435[6]="tvars";static
struct _tuple5 Cyc__gentuple_970={offsetof(struct Cyc_Absyn_FnInfo,tvars),(struct
_dyneither_ptr){_tmp435,_tmp435,_tmp435 + 6},(void*)& Cyc__genrep_273};static char
_tmp436[7]="effect";static struct _tuple5 Cyc__gentuple_971={offsetof(struct Cyc_Absyn_FnInfo,effect),(
struct _dyneither_ptr){_tmp436,_tmp436,_tmp436 + 7},(void*)& Cyc__genrep_110};
static char _tmp437[8]="ret_typ";static struct _tuple5 Cyc__gentuple_972={offsetof(
struct Cyc_Absyn_FnInfo,ret_typ),(struct _dyneither_ptr){_tmp437,_tmp437,_tmp437 + 
8},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp438[5]="args";static
struct _tuple5 Cyc__gentuple_973={offsetof(struct Cyc_Absyn_FnInfo,args),(struct
_dyneither_ptr){_tmp438,_tmp438,_tmp438 + 5},(void*)& Cyc__genrep_965};static char
_tmp439[10]="c_varargs";static struct _tuple5 Cyc__gentuple_974={offsetof(struct Cyc_Absyn_FnInfo,c_varargs),(
struct _dyneither_ptr){_tmp439,_tmp439,_tmp439 + 10},(void*)& Cyc__genrep_10};
static char _tmp43A[12]="cyc_varargs";static struct _tuple5 Cyc__gentuple_975={
offsetof(struct Cyc_Absyn_FnInfo,cyc_varargs),(struct _dyneither_ptr){_tmp43A,
_tmp43A,_tmp43A + 12},(void*)& Cyc__genrep_807};static char _tmp43B[7]="rgn_po";
static struct _tuple5 Cyc__gentuple_976={offsetof(struct Cyc_Absyn_FnInfo,rgn_po),(
struct _dyneither_ptr){_tmp43B,_tmp43B,_tmp43B + 7},(void*)& Cyc__genrep_372};
static char _tmp43C[11]="attributes";static struct _tuple5 Cyc__gentuple_977={
offsetof(struct Cyc_Absyn_FnInfo,attributes),(struct _dyneither_ptr){_tmp43C,
_tmp43C,_tmp43C + 11},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_978[
8]={& Cyc__gentuple_970,& Cyc__gentuple_971,& Cyc__gentuple_972,& Cyc__gentuple_973,&
Cyc__gentuple_974,& Cyc__gentuple_975,& Cyc__gentuple_976,& Cyc__gentuple_977};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_fn_info_t_rep={3,(struct _dyneither_ptr*)&
Cyc__genname_979,sizeof(struct Cyc_Absyn_FnInfo),{(void*)((struct _tuple5**)Cyc__genarr_978),(
void*)((struct _tuple5**)Cyc__genarr_978),(void*)((struct _tuple5**)Cyc__genarr_978
+ 8)}};struct _tuple84{unsigned int f1;struct Cyc_Absyn_FnInfo f2;};static struct
_tuple6 Cyc__gentuple_980={offsetof(struct _tuple84,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_981={offsetof(struct _tuple84,f2),(void*)& Cyc_Absyn_fn_info_t_rep};
static struct _tuple6*Cyc__genarr_982[2]={& Cyc__gentuple_980,& Cyc__gentuple_981};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_964={4,sizeof(struct _tuple84),{(
void*)((struct _tuple6**)Cyc__genarr_982),(void*)((struct _tuple6**)Cyc__genarr_982),(
void*)((struct _tuple6**)Cyc__genarr_982 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_960;static struct _tuple6 Cyc__gentuple_961={offsetof(struct _tuple14,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_962={offsetof(struct
_tuple14,f2),(void*)& Cyc__genrep_247};static struct _tuple6*Cyc__genarr_963[2]={&
Cyc__gentuple_961,& Cyc__gentuple_962};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_960={
4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_963),(void*)((
struct _tuple6**)Cyc__genarr_963),(void*)((struct _tuple6**)Cyc__genarr_963 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_956;struct _tuple85{unsigned int
f1;struct Cyc_Absyn_AggrInfo f2;};static struct _tuple6 Cyc__gentuple_957={offsetof(
struct _tuple85,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_958={
offsetof(struct _tuple85,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6*Cyc__genarr_959[2]={& Cyc__gentuple_957,& Cyc__gentuple_958};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_956={4,sizeof(struct _tuple85),{(void*)((
struct _tuple6**)Cyc__genarr_959),(void*)((struct _tuple6**)Cyc__genarr_959),(void*)((
struct _tuple6**)Cyc__genarr_959 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_951;
static struct _tuple6 Cyc__gentuple_952={offsetof(struct _tuple65,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_953={offsetof(struct _tuple65,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};
static struct _tuple6 Cyc__gentuple_954={offsetof(struct _tuple65,f3),(void*)& Cyc__genrep_359};
static struct _tuple6*Cyc__genarr_955[3]={& Cyc__gentuple_952,& Cyc__gentuple_953,&
Cyc__gentuple_954};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_951={4,
sizeof(struct _tuple65),{(void*)((struct _tuple6**)Cyc__genarr_955),(void*)((
struct _tuple6**)Cyc__genarr_955),(void*)((struct _tuple6**)Cyc__genarr_955 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_946;struct _tuple86{unsigned int
f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;};static struct _tuple6 Cyc__gentuple_947={
offsetof(struct _tuple86,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_948={
offsetof(struct _tuple86,f2),(void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_949={
offsetof(struct _tuple86,f3),(void*)& Cyc__genrep_618};static struct _tuple6*Cyc__genarr_950[
3]={& Cyc__gentuple_947,& Cyc__gentuple_948,& Cyc__gentuple_949};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_946={4,sizeof(struct _tuple86),{(void*)((struct _tuple6**)Cyc__genarr_950),(
void*)((struct _tuple6**)Cyc__genarr_950),(void*)((struct _tuple6**)Cyc__genarr_950
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_942;static struct _tuple6
Cyc__gentuple_943={offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_944={offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_214};
static struct _tuple6*Cyc__genarr_945[2]={& Cyc__gentuple_943,& Cyc__gentuple_944};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_942={4,sizeof(struct _tuple14),{(
void*)((struct _tuple6**)Cyc__genarr_945),(void*)((struct _tuple6**)Cyc__genarr_945),(
void*)((struct _tuple6**)Cyc__genarr_945 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_937;static struct _tuple6 Cyc__gentuple_938={offsetof(struct _tuple28,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_939={offsetof(struct
_tuple28,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_940={
offsetof(struct _tuple28,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_941[3]={& Cyc__gentuple_938,& Cyc__gentuple_939,& Cyc__gentuple_940};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_937={4,sizeof(struct _tuple28),{(
void*)((struct _tuple6**)Cyc__genarr_941),(void*)((struct _tuple6**)Cyc__genarr_941),(
void*)((struct _tuple6**)Cyc__genarr_941 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_929;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_930;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Typedefdecl_rep;static char
_tmp445[12]="Typedefdecl";static struct _dyneither_ptr Cyc__genname_1150=(struct
_dyneither_ptr){_tmp445,_tmp445,_tmp445 + 12};static char _tmp446[5]="name";static
struct _tuple5 Cyc__gentuple_1143={offsetof(struct Cyc_Absyn_Typedefdecl,name),(
struct _dyneither_ptr){_tmp446,_tmp446,_tmp446 + 5},(void*)& Cyc__genrep_11};static
char _tmp447[3]="tq";static struct _tuple5 Cyc__gentuple_1144={offsetof(struct Cyc_Absyn_Typedefdecl,tq),(
struct _dyneither_ptr){_tmp447,_tmp447,_tmp447 + 3},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp448[4]="tvs";static struct _tuple5 Cyc__gentuple_1145={offsetof(
struct Cyc_Absyn_Typedefdecl,tvs),(struct _dyneither_ptr){_tmp448,_tmp448,_tmp448 + 
4},(void*)& Cyc__genrep_273};static char _tmp449[5]="kind";static struct _tuple5 Cyc__gentuple_1146={
offsetof(struct Cyc_Absyn_Typedefdecl,kind),(struct _dyneither_ptr){_tmp449,
_tmp449,_tmp449 + 5},(void*)& Cyc__genrep_1107};static char _tmp44A[5]="defn";static
struct _tuple5 Cyc__gentuple_1147={offsetof(struct Cyc_Absyn_Typedefdecl,defn),(
struct _dyneither_ptr){_tmp44A,_tmp44A,_tmp44A + 5},(void*)& Cyc__genrep_110};
static char _tmp44B[5]="atts";static struct _tuple5 Cyc__gentuple_1148={offsetof(
struct Cyc_Absyn_Typedefdecl,atts),(struct _dyneither_ptr){_tmp44B,_tmp44B,_tmp44B
+ 5},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_1149[6]={& Cyc__gentuple_1143,&
Cyc__gentuple_1144,& Cyc__gentuple_1145,& Cyc__gentuple_1146,& Cyc__gentuple_1147,&
Cyc__gentuple_1148};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Typedefdecl_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_1150,sizeof(struct Cyc_Absyn_Typedefdecl),{(
void*)((struct _tuple5**)Cyc__genarr_1149),(void*)((struct _tuple5**)Cyc__genarr_1149),(
void*)((struct _tuple5**)Cyc__genarr_1149 + 6)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_930={1,1,(void*)((void*)& Cyc_struct_Absyn_Typedefdecl_rep)};struct
_tuple87{unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Typedefdecl*
f4;void**f5;};static struct _tuple6 Cyc__gentuple_931={offsetof(struct _tuple87,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_932={offsetof(struct
_tuple87,f2),(void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_933={
offsetof(struct _tuple87,f3),(void*)& Cyc__genrep_121};static struct _tuple6 Cyc__gentuple_934={
offsetof(struct _tuple87,f4),(void*)& Cyc__genrep_930};static struct _tuple6 Cyc__gentuple_935={
offsetof(struct _tuple87,f5),(void*)& Cyc__genrep_599};static struct _tuple6*Cyc__genarr_936[
5]={& Cyc__gentuple_931,& Cyc__gentuple_932,& Cyc__gentuple_933,& Cyc__gentuple_934,&
Cyc__gentuple_935};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_929={4,
sizeof(struct _tuple87),{(void*)((struct _tuple6**)Cyc__genarr_936),(void*)((
struct _tuple6**)Cyc__genarr_936),(void*)((struct _tuple6**)Cyc__genarr_936 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_120;static struct _tuple6 Cyc__gentuple_126={
offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_127={
offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_121};static struct _tuple6*Cyc__genarr_128[
2]={& Cyc__gentuple_126,& Cyc__gentuple_127};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_120={4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_128),(
void*)((struct _tuple6**)Cyc__genarr_128),(void*)((struct _tuple6**)Cyc__genarr_128
+ 2)}};static char _tmp450[9]="VoidType";static struct _tuple7 Cyc__gentuple_111={0,(
struct _dyneither_ptr){_tmp450,_tmp450,_tmp450 + 9}};static char _tmp451[10]="FloatType";
static struct _tuple7 Cyc__gentuple_112={1,(struct _dyneither_ptr){_tmp451,_tmp451,
_tmp451 + 10}};static char _tmp452[8]="HeapRgn";static struct _tuple7 Cyc__gentuple_113={
2,(struct _dyneither_ptr){_tmp452,_tmp452,_tmp452 + 8}};static char _tmp453[10]="UniqueRgn";
static struct _tuple7 Cyc__gentuple_114={3,(struct _dyneither_ptr){_tmp453,_tmp453,
_tmp453 + 10}};static struct _tuple7*Cyc__genarr_115[4]={& Cyc__gentuple_111,& Cyc__gentuple_112,&
Cyc__gentuple_113,& Cyc__gentuple_114};static char _tmp454[5]="Evar";static struct
_tuple5 Cyc__gentuple_1117={0,(struct _dyneither_ptr){_tmp454,_tmp454,_tmp454 + 5},(
void*)& Cyc__genrep_1102};static char _tmp455[8]="VarType";static struct _tuple5 Cyc__gentuple_1118={
1,(struct _dyneither_ptr){_tmp455,_tmp455,_tmp455 + 8},(void*)& Cyc__genrep_1098};
static char _tmp456[11]="TunionType";static struct _tuple5 Cyc__gentuple_1119={2,(
struct _dyneither_ptr){_tmp456,_tmp456,_tmp456 + 11},(void*)& Cyc__genrep_1071};
static char _tmp457[16]="TunionFieldType";static struct _tuple5 Cyc__gentuple_1120={3,(
struct _dyneither_ptr){_tmp457,_tmp457,_tmp457 + 16},(void*)& Cyc__genrep_1045};
static char _tmp458[12]="PointerType";static struct _tuple5 Cyc__gentuple_1121={4,(
struct _dyneither_ptr){_tmp458,_tmp458,_tmp458 + 12},(void*)& Cyc__genrep_1019};
static char _tmp459[8]="IntType";static struct _tuple5 Cyc__gentuple_1122={5,(struct
_dyneither_ptr){_tmp459,_tmp459,_tmp459 + 8},(void*)& Cyc__genrep_1007};static char
_tmp45A[11]="DoubleType";static struct _tuple5 Cyc__gentuple_1123={6,(struct
_dyneither_ptr){_tmp45A,_tmp45A,_tmp45A + 11},(void*)& Cyc__genrep_85};static char
_tmp45B[10]="ArrayType";static struct _tuple5 Cyc__gentuple_1124={7,(struct
_dyneither_ptr){_tmp45B,_tmp45B,_tmp45B + 10},(void*)& Cyc__genrep_983};static char
_tmp45C[7]="FnType";static struct _tuple5 Cyc__gentuple_1125={8,(struct
_dyneither_ptr){_tmp45C,_tmp45C,_tmp45C + 7},(void*)& Cyc__genrep_964};static char
_tmp45D[10]="TupleType";static struct _tuple5 Cyc__gentuple_1126={9,(struct
_dyneither_ptr){_tmp45D,_tmp45D,_tmp45D + 10},(void*)& Cyc__genrep_960};static char
_tmp45E[9]="AggrType";static struct _tuple5 Cyc__gentuple_1127={10,(struct
_dyneither_ptr){_tmp45E,_tmp45E,_tmp45E + 9},(void*)& Cyc__genrep_956};static char
_tmp45F[13]="AnonAggrType";static struct _tuple5 Cyc__gentuple_1128={11,(struct
_dyneither_ptr){_tmp45F,_tmp45F,_tmp45F + 13},(void*)& Cyc__genrep_951};static char
_tmp460[9]="EnumType";static struct _tuple5 Cyc__gentuple_1129={12,(struct
_dyneither_ptr){_tmp460,_tmp460,_tmp460 + 9},(void*)& Cyc__genrep_946};static char
_tmp461[13]="AnonEnumType";static struct _tuple5 Cyc__gentuple_1130={13,(struct
_dyneither_ptr){_tmp461,_tmp461,_tmp461 + 13},(void*)& Cyc__genrep_942};static char
_tmp462[14]="RgnHandleType";static struct _tuple5 Cyc__gentuple_1131={14,(struct
_dyneither_ptr){_tmp462,_tmp462,_tmp462 + 14},(void*)& Cyc__genrep_116};static char
_tmp463[11]="DynRgnType";static struct _tuple5 Cyc__gentuple_1132={15,(struct
_dyneither_ptr){_tmp463,_tmp463,_tmp463 + 11},(void*)& Cyc__genrep_937};static char
_tmp464[12]="TypedefType";static struct _tuple5 Cyc__gentuple_1133={16,(struct
_dyneither_ptr){_tmp464,_tmp464,_tmp464 + 12},(void*)& Cyc__genrep_929};static char
_tmp465[12]="ValueofType";static struct _tuple5 Cyc__gentuple_1134={17,(struct
_dyneither_ptr){_tmp465,_tmp465,_tmp465 + 12},(void*)& Cyc__genrep_129};static char
_tmp466[8]="TagType";static struct _tuple5 Cyc__gentuple_1135={18,(struct
_dyneither_ptr){_tmp466,_tmp466,_tmp466 + 8},(void*)& Cyc__genrep_116};static char
_tmp467[10]="AccessEff";static struct _tuple5 Cyc__gentuple_1136={19,(struct
_dyneither_ptr){_tmp467,_tmp467,_tmp467 + 10},(void*)& Cyc__genrep_116};static char
_tmp468[8]="JoinEff";static struct _tuple5 Cyc__gentuple_1137={20,(struct
_dyneither_ptr){_tmp468,_tmp468,_tmp468 + 8},(void*)& Cyc__genrep_120};static char
_tmp469[8]="RgnsEff";static struct _tuple5 Cyc__gentuple_1138={21,(struct
_dyneither_ptr){_tmp469,_tmp469,_tmp469 + 8},(void*)& Cyc__genrep_116};static
struct _tuple5*Cyc__genarr_1139[22]={& Cyc__gentuple_1117,& Cyc__gentuple_1118,& Cyc__gentuple_1119,&
Cyc__gentuple_1120,& Cyc__gentuple_1121,& Cyc__gentuple_1122,& Cyc__gentuple_1123,&
Cyc__gentuple_1124,& Cyc__gentuple_1125,& Cyc__gentuple_1126,& Cyc__gentuple_1127,&
Cyc__gentuple_1128,& Cyc__gentuple_1129,& Cyc__gentuple_1130,& Cyc__gentuple_1131,&
Cyc__gentuple_1132,& Cyc__gentuple_1133,& Cyc__gentuple_1134,& Cyc__gentuple_1135,&
Cyc__gentuple_1136,& Cyc__gentuple_1137,& Cyc__gentuple_1138};static char _tmp46B[5]="Type";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep={5,(struct _dyneither_ptr){
_tmp46B,_tmp46B,_tmp46B + 5},{(void*)((struct _tuple7**)Cyc__genarr_115),(void*)((
struct _tuple7**)Cyc__genarr_115),(void*)((struct _tuple7**)Cyc__genarr_115 + 4)},{(
void*)((struct _tuple5**)Cyc__genarr_1139),(void*)((struct _tuple5**)Cyc__genarr_1139),(
void*)((struct _tuple5**)Cyc__genarr_1139 + 22)}};static char _tmp46C[8]="Vardecl";
static struct _dyneither_ptr Cyc__genname_173=(struct _dyneither_ptr){_tmp46C,
_tmp46C,_tmp46C + 8};static char _tmp46D[3]="sc";static struct _tuple5 Cyc__gentuple_164={
offsetof(struct Cyc_Absyn_Vardecl,sc),(struct _dyneither_ptr){_tmp46D,_tmp46D,
_tmp46D + 3},(void*)& Cyc_Absyn_scope_t_rep};static char _tmp46E[5]="name";static
struct _tuple5 Cyc__gentuple_165={offsetof(struct Cyc_Absyn_Vardecl,name),(struct
_dyneither_ptr){_tmp46E,_tmp46E,_tmp46E + 5},(void*)& Cyc__genrep_11};static char
_tmp46F[3]="tq";static struct _tuple5 Cyc__gentuple_166={offsetof(struct Cyc_Absyn_Vardecl,tq),(
struct _dyneither_ptr){_tmp46F,_tmp46F,_tmp46F + 3},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp470[5]="type";static struct _tuple5 Cyc__gentuple_167={offsetof(
struct Cyc_Absyn_Vardecl,type),(struct _dyneither_ptr){_tmp470,_tmp470,_tmp470 + 5},(
void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp471[12]="initializer";static
struct _tuple5 Cyc__gentuple_168={offsetof(struct Cyc_Absyn_Vardecl,initializer),(
struct _dyneither_ptr){_tmp471,_tmp471,_tmp471 + 12},(void*)& Cyc__genrep_145};
static char _tmp472[4]="rgn";static struct _tuple5 Cyc__gentuple_169={offsetof(struct
Cyc_Absyn_Vardecl,rgn),(struct _dyneither_ptr){_tmp472,_tmp472,_tmp472 + 4},(void*)&
Cyc__genrep_110};static char _tmp473[11]="attributes";static struct _tuple5 Cyc__gentuple_170={
offsetof(struct Cyc_Absyn_Vardecl,attributes),(struct _dyneither_ptr){_tmp473,
_tmp473,_tmp473 + 11},(void*)& Cyc__genrep_62};static char _tmp474[8]="escapes";
static struct _tuple5 Cyc__gentuple_171={offsetof(struct Cyc_Absyn_Vardecl,escapes),(
struct _dyneither_ptr){_tmp474,_tmp474,_tmp474 + 8},(void*)& Cyc__genrep_10};static
struct _tuple5*Cyc__genarr_172[8]={& Cyc__gentuple_164,& Cyc__gentuple_165,& Cyc__gentuple_166,&
Cyc__gentuple_167,& Cyc__gentuple_168,& Cyc__gentuple_169,& Cyc__gentuple_170,& Cyc__gentuple_171};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_173,sizeof(struct Cyc_Absyn_Vardecl),{(void*)((
struct _tuple5**)Cyc__genarr_172),(void*)((struct _tuple5**)Cyc__genarr_172),(void*)((
struct _tuple5**)Cyc__genarr_172 + 8)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_148={
1,1,(void*)((void*)& Cyc_struct_Absyn_Vardecl_rep)};struct _tuple88{unsigned int f1;
struct Cyc_Absyn_Vardecl*f2;};static struct _tuple6 Cyc__gentuple_801={offsetof(
struct _tuple88,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_802={
offsetof(struct _tuple88,f2),(void*)& Cyc__genrep_148};static struct _tuple6*Cyc__genarr_803[
2]={& Cyc__gentuple_801,& Cyc__gentuple_802};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_800={4,sizeof(struct _tuple88),{(void*)((struct _tuple6**)Cyc__genarr_803),(
void*)((struct _tuple6**)Cyc__genarr_803),(void*)((struct _tuple6**)Cyc__genarr_803
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1183;struct _tuple89{
unsigned int f1;struct Cyc_Absyn_Pat*f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*
f4;};static struct _tuple6 Cyc__gentuple_1184={offsetof(struct _tuple89,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1185={offsetof(struct _tuple89,f2),(
void*)& Cyc__genrep_181};static struct _tuple6 Cyc__gentuple_1186={offsetof(struct
_tuple89,f3),(void*)& Cyc__genrep_146};static struct _tuple6 Cyc__gentuple_1187={
offsetof(struct _tuple89,f4),(void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_1188[
4]={& Cyc__gentuple_1184,& Cyc__gentuple_1185,& Cyc__gentuple_1186,& Cyc__gentuple_1187};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1183={4,sizeof(struct _tuple89),{(
void*)((struct _tuple6**)Cyc__genarr_1188),(void*)((struct _tuple6**)Cyc__genarr_1188),(
void*)((struct _tuple6**)Cyc__genarr_1188 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1179;static struct _tuple6 Cyc__gentuple_1180={offsetof(struct _tuple14,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1181={offsetof(struct
_tuple14,f2),(void*)& Cyc__genrep_147};static struct _tuple6*Cyc__genarr_1182[2]={&
Cyc__gentuple_1180,& Cyc__gentuple_1181};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1179={
4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_1182),(void*)((
struct _tuple6**)Cyc__genarr_1182),(void*)((struct _tuple6**)Cyc__genarr_1182 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1172;struct _tuple90{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;int f4;struct Cyc_Absyn_Exp*
f5;};static struct _tuple6 Cyc__gentuple_1173={offsetof(struct _tuple90,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1174={offsetof(struct _tuple90,f2),(
void*)& Cyc__genrep_274};static struct _tuple6 Cyc__gentuple_1175={offsetof(struct
_tuple90,f3),(void*)& Cyc__genrep_148};static struct _tuple6 Cyc__gentuple_1176={
offsetof(struct _tuple90,f4),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_1177={
offsetof(struct _tuple90,f5),(void*)& Cyc__genrep_145};static struct _tuple6*Cyc__genarr_1178[
5]={& Cyc__gentuple_1173,& Cyc__gentuple_1174,& Cyc__gentuple_1175,& Cyc__gentuple_1176,&
Cyc__gentuple_1177};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1172={4,
sizeof(struct _tuple90),{(void*)((struct _tuple6**)Cyc__genarr_1178),(void*)((
struct _tuple6**)Cyc__genarr_1178),(void*)((struct _tuple6**)Cyc__genarr_1178 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1166;struct _tuple91{unsigned int
f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Tvar*f3;struct Cyc_Absyn_Vardecl*f4;};
static struct _tuple6 Cyc__gentuple_1167={offsetof(struct _tuple91,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1168={offsetof(struct _tuple91,f2),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_1169={offsetof(struct _tuple91,f3),(void*)& Cyc__genrep_274};
static struct _tuple6 Cyc__gentuple_1170={offsetof(struct _tuple91,f4),(void*)& Cyc__genrep_148};
static struct _tuple6*Cyc__genarr_1171[4]={& Cyc__gentuple_1167,& Cyc__gentuple_1168,&
Cyc__gentuple_1169,& Cyc__gentuple_1170};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1166={
4,sizeof(struct _tuple91),{(void*)((struct _tuple6**)Cyc__genarr_1171),(void*)((
struct _tuple6**)Cyc__genarr_1171),(void*)((struct _tuple6**)Cyc__genarr_1171 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1162;struct _tuple92{unsigned int
f1;struct Cyc_Absyn_Aggrdecl*f2;};static struct _tuple6 Cyc__gentuple_1163={
offsetof(struct _tuple92,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1164={
offsetof(struct _tuple92,f2),(void*)((void*)& Cyc__genrep_357)};static struct
_tuple6*Cyc__genarr_1165[2]={& Cyc__gentuple_1163,& Cyc__gentuple_1164};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1162={4,sizeof(struct _tuple92),{(void*)((
struct _tuple6**)Cyc__genarr_1165),(void*)((struct _tuple6**)Cyc__genarr_1165),(
void*)((struct _tuple6**)Cyc__genarr_1165 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1158;struct _tuple93{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;};
static struct _tuple6 Cyc__gentuple_1159={offsetof(struct _tuple93,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1160={offsetof(struct _tuple93,f2),(void*)((void*)&
Cyc__genrep_263)};static struct _tuple6*Cyc__genarr_1161[2]={& Cyc__gentuple_1159,&
Cyc__gentuple_1160};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1158={4,
sizeof(struct _tuple93),{(void*)((struct _tuple6**)Cyc__genarr_1161),(void*)((
struct _tuple6**)Cyc__genarr_1161),(void*)((struct _tuple6**)Cyc__genarr_1161 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1154;struct _tuple94{unsigned int
f1;struct Cyc_Absyn_Enumdecl*f2;};static struct _tuple6 Cyc__gentuple_1155={
offsetof(struct _tuple94,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1156={
offsetof(struct _tuple94,f2),(void*)& Cyc__genrep_212};static struct _tuple6*Cyc__genarr_1157[
2]={& Cyc__gentuple_1155,& Cyc__gentuple_1156};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1154={4,sizeof(struct _tuple94),{(void*)((struct _tuple6**)Cyc__genarr_1157),(
void*)((struct _tuple6**)Cyc__genarr_1157),(void*)((struct _tuple6**)Cyc__genarr_1157
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_60;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_61;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_61={1,1,(void*)((
void*)& Cyc_struct_Absyn_Typedefdecl_rep)};struct _tuple95{unsigned int f1;struct
Cyc_Absyn_Typedefdecl*f2;};static struct _tuple6 Cyc__gentuple_1151={offsetof(
struct _tuple95,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1152={
offsetof(struct _tuple95,f2),(void*)& Cyc__genrep_61};static struct _tuple6*Cyc__genarr_1153[
2]={& Cyc__gentuple_1151,& Cyc__gentuple_1152};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_60={4,sizeof(struct _tuple95),{(void*)((struct _tuple6**)Cyc__genarr_1153),(
void*)((struct _tuple6**)Cyc__genarr_1153),(void*)((struct _tuple6**)Cyc__genarr_1153
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_55;struct _tuple96{
unsigned int f1;struct _dyneither_ptr*f2;struct Cyc_List_List*f3;};static struct
_tuple6 Cyc__gentuple_56={offsetof(struct _tuple96,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_57={offsetof(struct _tuple96,f2),(void*)& Cyc__genrep_13};
static struct _tuple6 Cyc__gentuple_58={offsetof(struct _tuple96,f3),(void*)& Cyc__genrep_0};
static struct _tuple6*Cyc__genarr_59[3]={& Cyc__gentuple_56,& Cyc__gentuple_57,& Cyc__gentuple_58};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_55={4,sizeof(struct _tuple96),{(
void*)((struct _tuple6**)Cyc__genarr_59),(void*)((struct _tuple6**)Cyc__genarr_59),(
void*)((struct _tuple6**)Cyc__genarr_59 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_50;struct _tuple97{unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*
f3;};static struct _tuple6 Cyc__gentuple_51={offsetof(struct _tuple97,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_52={offsetof(struct _tuple97,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_53={offsetof(struct
_tuple97,f3),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_54[3]={& Cyc__gentuple_51,&
Cyc__gentuple_52,& Cyc__gentuple_53};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_50={
4,sizeof(struct _tuple97),{(void*)((struct _tuple6**)Cyc__genarr_54),(void*)((
struct _tuple6**)Cyc__genarr_54),(void*)((struct _tuple6**)Cyc__genarr_54 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_46;static struct _tuple6 Cyc__gentuple_47={
offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_48={
offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_49[
2]={& Cyc__gentuple_47,& Cyc__gentuple_48};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_46={
4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_49),(void*)((
struct _tuple6**)Cyc__genarr_49),(void*)((struct _tuple6**)Cyc__genarr_49 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_6;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_7;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_8;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_9;struct _tuple98{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int
f3;};static struct _tuple6 Cyc__gentuple_34={offsetof(struct _tuple98,f1),(void*)&
Cyc__genrep_2};static struct _tuple6 Cyc__gentuple_35={offsetof(struct _tuple98,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_36={offsetof(struct
_tuple98,f3),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_37[3]={& Cyc__gentuple_34,&
Cyc__gentuple_35,& Cyc__gentuple_36};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_9={
4,sizeof(struct _tuple98),{(void*)((struct _tuple6**)Cyc__genarr_37),(void*)((
struct _tuple6**)Cyc__genarr_37),(void*)((struct _tuple6**)Cyc__genarr_37 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_8={1,1,(void*)((void*)& Cyc__genrep_9)};
static char _tmp486[5]="List";static struct _dyneither_ptr Cyc__genname_41=(struct
_dyneither_ptr){_tmp486,_tmp486,_tmp486 + 5};static char _tmp487[3]="hd";static
struct _tuple5 Cyc__gentuple_38={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp487,_tmp487,_tmp487 + 3},(void*)& Cyc__genrep_8};static char
_tmp488[3]="tl";static struct _tuple5 Cyc__gentuple_39={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp488,_tmp488,_tmp488 + 3},(void*)& Cyc__genrep_7};static
struct _tuple5*Cyc__genarr_40[2]={& Cyc__gentuple_38,& Cyc__gentuple_39};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_41,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_40),(void*)((struct _tuple5**)Cyc__genarr_40),(void*)((
struct _tuple5**)Cyc__genarr_40 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_7={
1,1,(void*)((void*)& Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep)};
struct _tuple99{unsigned int f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_42={offsetof(struct _tuple99,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_43={offsetof(struct _tuple99,f2),(void*)& Cyc__genrep_0};
static struct _tuple6 Cyc__gentuple_44={offsetof(struct _tuple99,f3),(void*)& Cyc__genrep_7};
static struct _tuple6*Cyc__genarr_45[3]={& Cyc__gentuple_42,& Cyc__gentuple_43,& Cyc__gentuple_44};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_6={4,sizeof(struct _tuple99),{(
void*)((struct _tuple6**)Cyc__genarr_45),(void*)((struct _tuple6**)Cyc__genarr_45),(
void*)((struct _tuple6**)Cyc__genarr_45 + 3)}};static char _tmp48C[9]="Porton_d";
static struct _tuple7 Cyc__gentuple_3={0,(struct _dyneither_ptr){_tmp48C,_tmp48C,
_tmp48C + 9}};static char _tmp48D[10]="Portoff_d";static struct _tuple7 Cyc__gentuple_4={
1,(struct _dyneither_ptr){_tmp48D,_tmp48D,_tmp48D + 10}};static struct _tuple7*Cyc__genarr_5[
2]={& Cyc__gentuple_3,& Cyc__gentuple_4};static char _tmp48E[6]="Var_d";static struct
_tuple5 Cyc__gentuple_1189={0,(struct _dyneither_ptr){_tmp48E,_tmp48E,_tmp48E + 6},(
void*)& Cyc__genrep_800};static char _tmp48F[5]="Fn_d";static struct _tuple5 Cyc__gentuple_1190={
1,(struct _dyneither_ptr){_tmp48F,_tmp48F,_tmp48F + 5},(void*)& Cyc__genrep_804};
static char _tmp490[6]="Let_d";static struct _tuple5 Cyc__gentuple_1191={2,(struct
_dyneither_ptr){_tmp490,_tmp490,_tmp490 + 6},(void*)& Cyc__genrep_1183};static char
_tmp491[7]="Letv_d";static struct _tuple5 Cyc__gentuple_1192={3,(struct
_dyneither_ptr){_tmp491,_tmp491,_tmp491 + 7},(void*)& Cyc__genrep_1179};static char
_tmp492[9]="Region_d";static struct _tuple5 Cyc__gentuple_1193={4,(struct
_dyneither_ptr){_tmp492,_tmp492,_tmp492 + 9},(void*)& Cyc__genrep_1172};static char
_tmp493[8]="Alias_d";static struct _tuple5 Cyc__gentuple_1194={5,(struct
_dyneither_ptr){_tmp493,_tmp493,_tmp493 + 8},(void*)& Cyc__genrep_1166};static char
_tmp494[7]="Aggr_d";static struct _tuple5 Cyc__gentuple_1195={6,(struct
_dyneither_ptr){_tmp494,_tmp494,_tmp494 + 7},(void*)& Cyc__genrep_1162};static char
_tmp495[9]="Tunion_d";static struct _tuple5 Cyc__gentuple_1196={7,(struct
_dyneither_ptr){_tmp495,_tmp495,_tmp495 + 9},(void*)& Cyc__genrep_1158};static char
_tmp496[7]="Enum_d";static struct _tuple5 Cyc__gentuple_1197={8,(struct
_dyneither_ptr){_tmp496,_tmp496,_tmp496 + 7},(void*)& Cyc__genrep_1154};static char
_tmp497[10]="Typedef_d";static struct _tuple5 Cyc__gentuple_1198={9,(struct
_dyneither_ptr){_tmp497,_tmp497,_tmp497 + 10},(void*)& Cyc__genrep_60};static char
_tmp498[12]="Namespace_d";static struct _tuple5 Cyc__gentuple_1199={10,(struct
_dyneither_ptr){_tmp498,_tmp498,_tmp498 + 12},(void*)& Cyc__genrep_55};static char
_tmp499[8]="Using_d";static struct _tuple5 Cyc__gentuple_1200={11,(struct
_dyneither_ptr){_tmp499,_tmp499,_tmp499 + 8},(void*)& Cyc__genrep_50};static char
_tmp49A[10]="ExternC_d";static struct _tuple5 Cyc__gentuple_1201={12,(struct
_dyneither_ptr){_tmp49A,_tmp49A,_tmp49A + 10},(void*)& Cyc__genrep_46};static char
_tmp49B[17]="ExternCinclude_d";static struct _tuple5 Cyc__gentuple_1202={13,(struct
_dyneither_ptr){_tmp49B,_tmp49B,_tmp49B + 17},(void*)& Cyc__genrep_6};static struct
_tuple5*Cyc__genarr_1203[14]={& Cyc__gentuple_1189,& Cyc__gentuple_1190,& Cyc__gentuple_1191,&
Cyc__gentuple_1192,& Cyc__gentuple_1193,& Cyc__gentuple_1194,& Cyc__gentuple_1195,&
Cyc__gentuple_1196,& Cyc__gentuple_1197,& Cyc__gentuple_1198,& Cyc__gentuple_1199,&
Cyc__gentuple_1200,& Cyc__gentuple_1201,& Cyc__gentuple_1202};static char _tmp49D[9]="Raw_decl";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep={5,(struct _dyneither_ptr){
_tmp49D,_tmp49D,_tmp49D + 9},{(void*)((struct _tuple7**)Cyc__genarr_5),(void*)((
struct _tuple7**)Cyc__genarr_5),(void*)((struct _tuple7**)Cyc__genarr_5 + 2)},{(
void*)((struct _tuple5**)Cyc__genarr_1203),(void*)((struct _tuple5**)Cyc__genarr_1203),(
void*)((struct _tuple5**)Cyc__genarr_1203 + 14)}};static char _tmp49E[5]="Decl";
static struct _dyneither_ptr Cyc__genname_1207=(struct _dyneither_ptr){_tmp49E,
_tmp49E,_tmp49E + 5};static char _tmp49F[2]="r";static struct _tuple5 Cyc__gentuple_1204={
offsetof(struct Cyc_Absyn_Decl,r),(struct _dyneither_ptr){_tmp49F,_tmp49F,_tmp49F + 
2},(void*)& Cyc_Absyn_raw_decl_t_rep};static char _tmp4A0[4]="loc";static struct
_tuple5 Cyc__gentuple_1205={offsetof(struct Cyc_Absyn_Decl,loc),(struct
_dyneither_ptr){_tmp4A0,_tmp4A0,_tmp4A0 + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_1206[2]={& Cyc__gentuple_1204,& Cyc__gentuple_1205};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Decl_rep={3,(struct _dyneither_ptr*)& Cyc__genname_1207,sizeof(
struct Cyc_Absyn_Decl),{(void*)((struct _tuple5**)Cyc__genarr_1206),(void*)((
struct _tuple5**)Cyc__genarr_1206),(void*)((struct _tuple5**)Cyc__genarr_1206 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1={1,1,(void*)((void*)& Cyc_struct_Absyn_Decl_rep)};
static char _tmp4A3[5]="List";static struct _dyneither_ptr Cyc__genname_1211=(struct
_dyneither_ptr){_tmp4A3,_tmp4A3,_tmp4A3 + 5};static char _tmp4A4[3]="hd";static
struct _tuple5 Cyc__gentuple_1208={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp4A4,_tmp4A4,_tmp4A4 + 3},(void*)& Cyc__genrep_1};static char
_tmp4A5[3]="tl";static struct _tuple5 Cyc__gentuple_1209={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp4A5,_tmp4A5,_tmp4A5 + 3},(void*)& Cyc__genrep_0};static
struct _tuple5*Cyc__genarr_1210[2]={& Cyc__gentuple_1208,& Cyc__gentuple_1209};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_1211,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_1210),(void*)((struct _tuple5**)Cyc__genarr_1210),(
void*)((struct _tuple5**)Cyc__genarr_1210 + 2)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_0={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_decl_t46H2_rep)};
void*Cyc_decls_rep=(void*)& Cyc__genrep_0;
