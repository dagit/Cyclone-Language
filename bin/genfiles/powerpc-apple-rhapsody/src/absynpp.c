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
void*v;};struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
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
char*tag;struct _dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*
x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2);int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2);extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*
f);struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*
Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();struct Cyc_PP_Doc*Cyc_PP_line_doc();
struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s);struct Cyc_PP_Doc*Cyc_PP_textptr(
struct _dyneither_ptr*p);struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,
int w);struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);struct Cyc_PP_Doc*Cyc_PP_cat(
struct _dyneither_ptr);struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct
Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),
struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_seql(
struct _dyneither_ptr sep,struct Cyc_List_List*l0);struct Cyc_PP_Doc*Cyc_PP_ppseql(
struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
struct Cyc_PP_Doc*Cyc_PP_group(struct _dyneither_ptr start,struct _dyneither_ptr stop,
struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_egroup(
struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,
struct Cyc_List_List*l);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;
struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];
struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dyneither_ptr source;
struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};extern char
Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;int is_flat;};
struct Cyc_Absyn_UnknownDatatype_struct{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
f1;};struct Cyc_Absyn_KnownDatatype_struct{int tag;struct Cyc_Absyn_Datatypedecl**
f1;};union Cyc_Absyn_DatatypeInfoU_union{struct Cyc_Absyn_UnknownDatatype_struct
UnknownDatatype;struct Cyc_Absyn_KnownDatatype_struct KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU_union datatype_info;struct Cyc_List_List*targs;struct
Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*
datatype_name;struct _tuple0*field_name;int is_extensible;};struct Cyc_Absyn_UnknownDatatypefield_struct{
int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo f1;};struct Cyc_Absyn_KnownDatatypefield_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};union
Cyc_Absyn_DatatypeFieldInfoU_union{struct Cyc_Absyn_UnknownDatatypefield_struct
UnknownDatatypefield;struct Cyc_Absyn_KnownDatatypefield_struct KnownDatatypefield;
};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
f1;};union Cyc_Absyn_AggrInfoU_union{struct Cyc_Absyn_UnknownAggr_struct
UnknownAggr;struct Cyc_Absyn_KnownAggr_struct KnownAggr;};struct Cyc_Absyn_AggrInfo{
union Cyc_Absyn_AggrInfoU_union aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;struct Cyc_Absyn_Conref*
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
_dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
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
f1;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{
struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*loc;void*annot;};
struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct
_tuple2{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
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
struct Cyc_Core_Opt*fields;int is_extensible;int is_flat;};struct Cyc_Absyn_Enumfield{
struct _tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};
struct Cyc_Absyn_Enumdecl{void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};
struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};
struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*
f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_struct{int tag;struct Cyc_Absyn_Tvar*
f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;
};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;
struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*
f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_bounds_one;struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple3{void*f1;
struct _tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU_union);
struct Cyc_Buffer_t;unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strptrcmp(
struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_str_sepstr(
struct Cyc_List_List*,struct _dyneither_ptr);struct _tuple4{unsigned int f1;int f2;};
struct _tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple5{void*f1;void*f2;};struct _tuple5*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple5*Cyc_Dict_rchoose(
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
le;int allow_valueof;};void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern int Cyc_Absynpp_print_for_cycdoc;void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);struct
_dyneither_ptr Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_typ2cstring(
void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(void*);struct _dyneither_ptr
Cyc_Absynpp_kindbound2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _dyneither_ptr
Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct _dyneither_ptr Cyc_Absynpp_prim2string(
void*p);struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);struct
_dyneither_ptr Cyc_Absynpp_scope2string(void*sc);int Cyc_Absynpp_is_anon_aggrtype(
void*t);extern struct _dyneither_ptr Cyc_Absynpp_cyc_string;extern struct
_dyneither_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Absynpp_prim2str(void*p);int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s);struct _tuple6{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple6*arg);struct
_tuple7{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};struct _tuple7
Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);extern
void*Cyc_Cyclone_c_compiler;struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*dp);struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List*cs);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
void*);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*);struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int
inprec,struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct
_tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst_union);struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
void*);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*
es);struct _tuple9{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_PP_Doc*
Cyc_Absynpp_de2doc(struct _tuple9*de);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*dopt);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct
Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd);static int Cyc_Absynpp_expand_typedefs;static int Cyc_Absynpp_qvar_to_Cids;
static char _tmp0[4]="Cyc";struct _dyneither_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,
_tmp0 + 4};struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
static int Cyc_Absynpp_add_cyc_prefix;static int Cyc_Absynpp_to_VC;static int Cyc_Absynpp_decls_first;
static int Cyc_Absynpp_rewrite_temp_tvars;static int Cyc_Absynpp_print_all_tvars;
static int Cyc_Absynpp_print_all_kinds;static int Cyc_Absynpp_print_all_effects;
static int Cyc_Absynpp_print_using_stmts;static int Cyc_Absynpp_print_externC_stmts;
static int Cyc_Absynpp_print_full_evars;static int Cyc_Absynpp_generate_line_directives;
static int Cyc_Absynpp_use_curr_namespace;static int Cyc_Absynpp_print_zeroterm;
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;int Cyc_Absynpp_print_for_cycdoc=
0;struct Cyc_Absynpp_Params;void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*
fs);void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){Cyc_Absynpp_expand_typedefs=
fs->expand_typedefs;Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
fs->add_cyc_prefix;Cyc_Absynpp_to_VC=fs->to_VC;Cyc_Absynpp_decls_first=fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars;Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;Cyc_Absynpp_print_all_effects=
fs->print_all_effects;Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;Cyc_Absynpp_print_externC_stmts=
fs->print_externC_stmts;Cyc_Absynpp_print_full_evars=fs->print_full_evars;Cyc_Absynpp_print_zeroterm=
fs->print_zeroterm;Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;Cyc_Absynpp_curr_namespace=
fs->curr_namespace;}struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,
1,0,0,0,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,0,
0,0,1,0,0,1,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,
0,1,0,0,0,0,0,0,0,0,0,0,0};struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,
0,0,0,0,0,0,0,1,1,0,1,0,0,0};static void Cyc_Absynpp_curr_namespace_add(struct
_dyneither_ptr*v);static void Cyc_Absynpp_curr_namespace_add(struct _dyneither_ptr*
v){struct Cyc_List_List*_tmp4E9;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((
_tmp4E9=_cycalloc(sizeof(*_tmp4E9)),((_tmp4E9->hd=v,((_tmp4E9->tl=0,_tmp4E9)))))));}
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l);static void Cyc_Absynpp_suppr_last(
struct Cyc_List_List**l){if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;
else{Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
static void Cyc_Absynpp_curr_namespace_drop();static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c);struct _dyneither_ptr Cyc_Absynpp_char_escape(
char c){switch(c){case '\a': _LL0: {const char*_tmp4EA;return(_tmp4EA="\\a",
_tag_dyneither(_tmp4EA,sizeof(char),3));}case '\b': _LL1: {const char*_tmp4EB;
return(_tmp4EB="\\b",_tag_dyneither(_tmp4EB,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp4EC;return(_tmp4EC="\\f",_tag_dyneither(_tmp4EC,sizeof(char),3));}
case '\n': _LL3: {const char*_tmp4ED;return(_tmp4ED="\\n",_tag_dyneither(_tmp4ED,
sizeof(char),3));}case '\r': _LL4: {const char*_tmp4EE;return(_tmp4EE="\\r",
_tag_dyneither(_tmp4EE,sizeof(char),3));}case '\t': _LL5: {const char*_tmp4EF;
return(_tmp4EF="\\t",_tag_dyneither(_tmp4EF,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp4F0;return(_tmp4F0="\\v",_tag_dyneither(_tmp4F0,sizeof(char),3));}
case '\\': _LL7: {const char*_tmp4F1;return(_tmp4F1="\\\\",_tag_dyneither(_tmp4F1,
sizeof(char),3));}case '"': _LL8: {const char*_tmp4F2;return(_tmp4F2="\"",
_tag_dyneither(_tmp4F2,sizeof(char),2));}case '\'': _LL9: {const char*_tmp4F3;
return(_tmp4F3="\\'",_tag_dyneither(_tmp4F3,sizeof(char),3));}default: _LLA: if(c
>= ' '  && c <= '~'){struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);{char _tmp4F6;
char _tmp4F5;struct _dyneither_ptr _tmp4F4;(_tmp4F4=_dyneither_ptr_plus(_tmpC,
sizeof(char),0),((_tmp4F5=*((char*)_check_dyneither_subscript(_tmp4F4,sizeof(
char),0)),((_tmp4F6=c,((_get_dyneither_size(_tmp4F4,sizeof(char))== 1  && (
_tmp4F5 == '\000'  && _tmp4F6 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4F4.curr)=
_tmp4F6)))))));}return(struct _dyneither_ptr)_tmpC;}else{struct _dyneither_ptr
_tmp10=Cyc_Core_new_string(5);int j=0;{char _tmp4F9;char _tmp4F8;struct
_dyneither_ptr _tmp4F7;(_tmp4F7=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp4F8=*((char*)_check_dyneither_subscript(_tmp4F7,sizeof(char),0)),((_tmp4F9='\\',((
_get_dyneither_size(_tmp4F7,sizeof(char))== 1  && (_tmp4F8 == '\000'  && _tmp4F9 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp4F7.curr)=_tmp4F9)))))));}{char _tmp4FC;char
_tmp4FB;struct _dyneither_ptr _tmp4FA;(_tmp4FA=_dyneither_ptr_plus(_tmp10,sizeof(
char),j ++),((_tmp4FB=*((char*)_check_dyneither_subscript(_tmp4FA,sizeof(char),0)),((
_tmp4FC=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp4FA,
sizeof(char))== 1  && (_tmp4FB == '\000'  && _tmp4FC != '\000')?_throw_arraybounds():
1,*((char*)_tmp4FA.curr)=_tmp4FC)))))));}{char _tmp4FF;char _tmp4FE;struct
_dyneither_ptr _tmp4FD;(_tmp4FD=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp4FE=*((char*)_check_dyneither_subscript(_tmp4FD,sizeof(char),0)),((_tmp4FF=(
char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp4FD,sizeof(char))== 1  && (
_tmp4FE == '\000'  && _tmp4FF != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4FD.curr)=
_tmp4FF)))))));}{char _tmp502;char _tmp501;struct _dyneither_ptr _tmp500;(_tmp500=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp501=*((char*)
_check_dyneither_subscript(_tmp500,sizeof(char),0)),((_tmp502=(char)('0' + (c & 7)),((
_get_dyneither_size(_tmp500,sizeof(char))== 1  && (_tmp501 == '\000'  && _tmp502 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp500.curr)=_tmp502)))))));}return(struct
_dyneither_ptr)_tmp10;}}}static int Cyc_Absynpp_special(struct _dyneither_ptr s);
static int Cyc_Absynpp_special(struct _dyneither_ptr s){int sz=(int)(
_get_dyneither_size(s,sizeof(char))- 1);{int i=0;for(0;i < sz;++ i){char c=*((const
char*)_check_dyneither_subscript(s,sizeof(char),i));if(((c <= ' '  || c >= '~') || 
c == '"') || c == '\\')return 1;}}return 0;}struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr s);struct _dyneither_ptr Cyc_Absynpp_string_escape(struct
_dyneither_ptr s){if(!Cyc_Absynpp_special(s))return s;{int n=(int)(
_get_dyneither_size(s,sizeof(char))- 1);if(n > 0  && *((const char*)
_check_dyneither_subscript(s,sizeof(char),n))== '\000')-- n;{int len=0;{int i=0;
for(0;i <= n;++ i){char _tmp1D=*((const char*)_check_dyneither_subscript(s,sizeof(
char),i));_LLD: if(_tmp1D != '\a')goto _LLF;_LLE: goto _LL10;_LLF: if(_tmp1D != '\b')
goto _LL11;_LL10: goto _LL12;_LL11: if(_tmp1D != '\f')goto _LL13;_LL12: goto _LL14;_LL13:
if(_tmp1D != '\n')goto _LL15;_LL14: goto _LL16;_LL15: if(_tmp1D != '\r')goto _LL17;
_LL16: goto _LL18;_LL17: if(_tmp1D != '\t')goto _LL19;_LL18: goto _LL1A;_LL19: if(_tmp1D
!= '\v')goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp1D != '\\')goto _LL1D;_LL1C: goto
_LL1E;_LL1D: if(_tmp1D != '"')goto _LL1F;_LL1E: len +=2;goto _LLC;_LL1F:;_LL20: if(
_tmp1D >= ' '  && _tmp1D <= '~')++ len;else{len +=4;}goto _LLC;_LLC:;}}{struct
_dyneither_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));int j=0;{int i=0;for(0;
i <= n;++ i){char _tmp1E=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:{char _tmp505;char _tmp504;struct
_dyneither_ptr _tmp503;(_tmp503=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp504=*((
char*)_check_dyneither_subscript(_tmp503,sizeof(char),0)),((_tmp505='\\',((
_get_dyneither_size(_tmp503,sizeof(char))== 1  && (_tmp504 == '\000'  && _tmp505 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp503.curr)=_tmp505)))))));}{char _tmp508;char
_tmp507;struct _dyneither_ptr _tmp506;(_tmp506=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp507=*((char*)_check_dyneither_subscript(_tmp506,sizeof(char),0)),((
_tmp508='a',((_get_dyneither_size(_tmp506,sizeof(char))== 1  && (_tmp507 == '\000'
 && _tmp508 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp506.curr)=_tmp508)))))));}
goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:{char _tmp50B;char _tmp50A;struct
_dyneither_ptr _tmp509;(_tmp509=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp50A=*((
char*)_check_dyneither_subscript(_tmp509,sizeof(char),0)),((_tmp50B='\\',((
_get_dyneither_size(_tmp509,sizeof(char))== 1  && (_tmp50A == '\000'  && _tmp50B != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp509.curr)=_tmp50B)))))));}{char _tmp50E;char
_tmp50D;struct _dyneither_ptr _tmp50C;(_tmp50C=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp50D=*((char*)_check_dyneither_subscript(_tmp50C,sizeof(char),0)),((
_tmp50E='b',((_get_dyneither_size(_tmp50C,sizeof(char))== 1  && (_tmp50D == '\000'
 && _tmp50E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp50C.curr)=_tmp50E)))))));}
goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:{char _tmp511;char _tmp510;struct
_dyneither_ptr _tmp50F;(_tmp50F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp510=*((
char*)_check_dyneither_subscript(_tmp50F,sizeof(char),0)),((_tmp511='\\',((
_get_dyneither_size(_tmp50F,sizeof(char))== 1  && (_tmp510 == '\000'  && _tmp511 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp50F.curr)=_tmp511)))))));}{char _tmp514;char
_tmp513;struct _dyneither_ptr _tmp512;(_tmp512=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp513=*((char*)_check_dyneither_subscript(_tmp512,sizeof(char),0)),((
_tmp514='f',((_get_dyneither_size(_tmp512,sizeof(char))== 1  && (_tmp513 == '\000'
 && _tmp514 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp512.curr)=_tmp514)))))));}
goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:{char _tmp517;char _tmp516;struct
_dyneither_ptr _tmp515;(_tmp515=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp516=*((
char*)_check_dyneither_subscript(_tmp515,sizeof(char),0)),((_tmp517='\\',((
_get_dyneither_size(_tmp515,sizeof(char))== 1  && (_tmp516 == '\000'  && _tmp517 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp515.curr)=_tmp517)))))));}{char _tmp51A;char
_tmp519;struct _dyneither_ptr _tmp518;(_tmp518=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp519=*((char*)_check_dyneither_subscript(_tmp518,sizeof(char),0)),((
_tmp51A='n',((_get_dyneither_size(_tmp518,sizeof(char))== 1  && (_tmp519 == '\000'
 && _tmp51A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp518.curr)=_tmp51A)))))));}
goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:{char _tmp51D;char _tmp51C;struct
_dyneither_ptr _tmp51B;(_tmp51B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp51C=*((
char*)_check_dyneither_subscript(_tmp51B,sizeof(char),0)),((_tmp51D='\\',((
_get_dyneither_size(_tmp51B,sizeof(char))== 1  && (_tmp51C == '\000'  && _tmp51D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp51B.curr)=_tmp51D)))))));}{char _tmp520;char
_tmp51F;struct _dyneither_ptr _tmp51E;(_tmp51E=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp51F=*((char*)_check_dyneither_subscript(_tmp51E,sizeof(char),0)),((
_tmp520='r',((_get_dyneither_size(_tmp51E,sizeof(char))== 1  && (_tmp51F == '\000'
 && _tmp520 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp51E.curr)=_tmp520)))))));}
goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:{char _tmp523;char _tmp522;struct
_dyneither_ptr _tmp521;(_tmp521=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp522=*((
char*)_check_dyneither_subscript(_tmp521,sizeof(char),0)),((_tmp523='\\',((
_get_dyneither_size(_tmp521,sizeof(char))== 1  && (_tmp522 == '\000'  && _tmp523 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp521.curr)=_tmp523)))))));}{char _tmp526;char
_tmp525;struct _dyneither_ptr _tmp524;(_tmp524=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp525=*((char*)_check_dyneither_subscript(_tmp524,sizeof(char),0)),((
_tmp526='t',((_get_dyneither_size(_tmp524,sizeof(char))== 1  && (_tmp525 == '\000'
 && _tmp526 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp524.curr)=_tmp526)))))));}
goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:{char _tmp529;char _tmp528;struct
_dyneither_ptr _tmp527;(_tmp527=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp528=*((
char*)_check_dyneither_subscript(_tmp527,sizeof(char),0)),((_tmp529='\\',((
_get_dyneither_size(_tmp527,sizeof(char))== 1  && (_tmp528 == '\000'  && _tmp529 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp527.curr)=_tmp529)))))));}{char _tmp52C;char
_tmp52B;struct _dyneither_ptr _tmp52A;(_tmp52A=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp52B=*((char*)_check_dyneither_subscript(_tmp52A,sizeof(char),0)),((
_tmp52C='v',((_get_dyneither_size(_tmp52A,sizeof(char))== 1  && (_tmp52B == '\000'
 && _tmp52C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp52A.curr)=_tmp52C)))))));}
goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:{char _tmp52F;char _tmp52E;struct
_dyneither_ptr _tmp52D;(_tmp52D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp52E=*((
char*)_check_dyneither_subscript(_tmp52D,sizeof(char),0)),((_tmp52F='\\',((
_get_dyneither_size(_tmp52D,sizeof(char))== 1  && (_tmp52E == '\000'  && _tmp52F != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp52D.curr)=_tmp52F)))))));}{char _tmp532;char
_tmp531;struct _dyneither_ptr _tmp530;(_tmp530=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp531=*((char*)_check_dyneither_subscript(_tmp530,sizeof(char),0)),((
_tmp532='\\',((_get_dyneither_size(_tmp530,sizeof(char))== 1  && (_tmp531 == '\000'
 && _tmp532 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp530.curr)=_tmp532)))))));}
goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:{char _tmp535;char _tmp534;struct
_dyneither_ptr _tmp533;(_tmp533=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp534=*((
char*)_check_dyneither_subscript(_tmp533,sizeof(char),0)),((_tmp535='\\',((
_get_dyneither_size(_tmp533,sizeof(char))== 1  && (_tmp534 == '\000'  && _tmp535 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp533.curr)=_tmp535)))))));}{char _tmp538;char
_tmp537;struct _dyneither_ptr _tmp536;(_tmp536=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp537=*((char*)_check_dyneither_subscript(_tmp536,sizeof(char),0)),((
_tmp538='"',((_get_dyneither_size(_tmp536,sizeof(char))== 1  && (_tmp537 == '\000'
 && _tmp538 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp536.curr)=_tmp538)))))));}
goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp53B;char _tmp53A;
struct _dyneither_ptr _tmp539;(_tmp539=_dyneither_ptr_plus(t,sizeof(char),j ++),((
_tmp53A=*((char*)_check_dyneither_subscript(_tmp539,sizeof(char),0)),((_tmp53B=
_tmp1E,((_get_dyneither_size(_tmp539,sizeof(char))== 1  && (_tmp53A == '\000'  && 
_tmp53B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp539.curr)=_tmp53B)))))));}
else{{char _tmp53E;char _tmp53D;struct _dyneither_ptr _tmp53C;(_tmp53C=
_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp53D=*((char*)
_check_dyneither_subscript(_tmp53C,sizeof(char),0)),((_tmp53E='\\',((
_get_dyneither_size(_tmp53C,sizeof(char))== 1  && (_tmp53D == '\000'  && _tmp53E != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp53C.curr)=_tmp53E)))))));}{char _tmp541;char
_tmp540;struct _dyneither_ptr _tmp53F;(_tmp53F=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp540=*((char*)_check_dyneither_subscript(_tmp53F,sizeof(char),0)),((
_tmp541=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dyneither_size(_tmp53F,sizeof(char))
== 1  && (_tmp540 == '\000'  && _tmp541 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp53F.curr)=_tmp541)))))));}{char _tmp544;char _tmp543;struct _dyneither_ptr
_tmp542;(_tmp542=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp543=*((char*)
_check_dyneither_subscript(_tmp542,sizeof(char),0)),((_tmp544=(char)('0' + (
_tmp1E >> 3 & 7)),((_get_dyneither_size(_tmp542,sizeof(char))== 1  && (_tmp543 == '\000'
 && _tmp544 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp542.curr)=_tmp544)))))));}{
char _tmp547;char _tmp546;struct _dyneither_ptr _tmp545;(_tmp545=_dyneither_ptr_plus(
t,sizeof(char),j ++),((_tmp546=*((char*)_check_dyneither_subscript(_tmp545,
sizeof(char),0)),((_tmp547=(char)('0' + (_tmp1E & 7)),((_get_dyneither_size(
_tmp545,sizeof(char))== 1  && (_tmp546 == '\000'  && _tmp547 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp545.curr)=_tmp547)))))));}}goto _LL21;_LL21:;}}
return(struct _dyneither_ptr)t;}}}}static char _tmp64[9]="restrict";static struct
_dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char
_tmp65[9]="volatile";static struct _dyneither_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dyneither_ptr
Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string;static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq);struct Cyc_PP_Doc*
Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=0;if(tq.q_restrict){
struct Cyc_List_List*_tmp548;l=((_tmp548=_cycalloc(sizeof(*_tmp548)),((_tmp548->hd=
Cyc_Absynpp_restrict_sp,((_tmp548->tl=l,_tmp548))))));}if(tq.q_volatile){struct
Cyc_List_List*_tmp549;l=((_tmp549=_cycalloc(sizeof(*_tmp549)),((_tmp549->hd=Cyc_Absynpp_volatile_sp,((
_tmp549->tl=l,_tmp549))))));}if(tq.print_const){struct Cyc_List_List*_tmp54A;l=((
_tmp54A=_cycalloc(sizeof(*_tmp54A)),((_tmp54A->hd=Cyc_Absynpp_const_sp,((_tmp54A->tl=
l,_tmp54A))))));}{const char*_tmp54D;const char*_tmp54C;const char*_tmp54B;return
Cyc_PP_egroup(((_tmp54B="",_tag_dyneither(_tmp54B,sizeof(char),1))),((_tmp54C=" ",
_tag_dyneither(_tmp54C,sizeof(char),2))),((_tmp54D=" ",_tag_dyneither(_tmp54D,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));}}
struct _dyneither_ptr Cyc_Absynpp_kind2string(void*k);struct _dyneither_ptr Cyc_Absynpp_kind2string(
void*k){void*_tmp6D=k;_LL37: if((int)_tmp6D != 0)goto _LL39;_LL38: {const char*
_tmp54E;return(_tmp54E="A",_tag_dyneither(_tmp54E,sizeof(char),2));}_LL39: if((
int)_tmp6D != 1)goto _LL3B;_LL3A: {const char*_tmp54F;return(_tmp54F="M",
_tag_dyneither(_tmp54F,sizeof(char),2));}_LL3B: if((int)_tmp6D != 2)goto _LL3D;
_LL3C: {const char*_tmp550;return(_tmp550="B",_tag_dyneither(_tmp550,sizeof(char),
2));}_LL3D: if((int)_tmp6D != 3)goto _LL3F;_LL3E: {const char*_tmp551;return(_tmp551="R",
_tag_dyneither(_tmp551,sizeof(char),2));}_LL3F: if((int)_tmp6D != 4)goto _LL41;
_LL40: {const char*_tmp552;return(_tmp552="UR",_tag_dyneither(_tmp552,sizeof(char),
3));}_LL41: if((int)_tmp6D != 5)goto _LL43;_LL42: {const char*_tmp553;return(_tmp553="TR",
_tag_dyneither(_tmp553,sizeof(char),3));}_LL43: if((int)_tmp6D != 6)goto _LL45;
_LL44: {const char*_tmp554;return(_tmp554="E",_tag_dyneither(_tmp554,sizeof(char),
2));}_LL45: if((int)_tmp6D != 7)goto _LL36;_LL46: {const char*_tmp555;return(_tmp555="I",
_tag_dyneither(_tmp555,sizeof(char),2));}_LL36:;}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(
void*k);struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(void*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(
k));}struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c);struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp76=Cyc_Absyn_compress_kb(
c);void*_tmp77;void*_tmp78;_LL48: if(*((int*)_tmp76)!= 0)goto _LL4A;_tmp77=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp76)->f1;_LL49: return Cyc_Absynpp_kind2string(
_tmp77);_LL4A: if(*((int*)_tmp76)!= 1)goto _LL4C;_LL4B: if(Cyc_PP_tex_output){const
char*_tmp556;return(_tmp556="{?}",_tag_dyneither(_tmp556,sizeof(char),4));}else{
const char*_tmp557;return(_tmp557="?",_tag_dyneither(_tmp557,sizeof(char),2));}
_LL4C: if(*((int*)_tmp76)!= 2)goto _LL47;_tmp78=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp76)->f2;_LL4D: return Cyc_Absynpp_kind2string(_tmp78);_LL47:;}struct Cyc_PP_Doc*
Cyc_Absynpp_kindbound2doc(void*c);struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(
void*c){void*_tmp7B=Cyc_Absyn_compress_kb(c);void*_tmp7C;void*_tmp7D;_LL4F: if(*((
int*)_tmp7B)!= 0)goto _LL51;_tmp7C=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp7B)->f1;
_LL50: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7C));_LL51: if(*((int*)_tmp7B)
!= 1)goto _LL53;_LL52: if(Cyc_PP_tex_output){const char*_tmp558;return Cyc_PP_text_width(((
_tmp558="{?}",_tag_dyneither(_tmp558,sizeof(char),4))),1);}else{const char*
_tmp559;return Cyc_PP_text(((_tmp559="?",_tag_dyneither(_tmp559,sizeof(char),2))));}
_LL53: if(*((int*)_tmp7B)!= 2)goto _LL4E;_tmp7D=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp7B)->f2;_LL54: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7D));_LL4E:;}
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts);struct Cyc_PP_Doc*
Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){const char*_tmp55C;const char*_tmp55B;
const char*_tmp55A;return Cyc_PP_egroup(((_tmp55A="<",_tag_dyneither(_tmp55A,
sizeof(char),2))),((_tmp55B=">",_tag_dyneither(_tmp55B,sizeof(char),2))),((
_tmp55C=",",_tag_dyneither(_tmp55C,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv);struct Cyc_PP_Doc*
Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){void*_tmp83=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmp84;void*_tmp85;void*_tmp86;_LL56: if(*((int*)_tmp83)!= 1)
goto _LL58;_LL57: goto _LL59;_LL58: if(*((int*)_tmp83)!= 0)goto _LL5A;_tmp84=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp83)->f1;if((int)_tmp84 != 2)goto _LL5A;_LL59:
return Cyc_PP_textptr(tv->name);_LL5A: if(*((int*)_tmp83)!= 2)goto _LL5C;_tmp85=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp83)->f2;_LL5B: _tmp86=_tmp85;goto
_LL5D;_LL5C: if(*((int*)_tmp83)!= 0)goto _LL55;_tmp86=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp83)->f1;_LL5D: {const char*_tmp55F;struct Cyc_PP_Doc*_tmp55E[3];return(_tmp55E[
2]=Cyc_Absynpp_kind2doc(_tmp86),((_tmp55E[1]=Cyc_PP_text(((_tmp55F="::",
_tag_dyneither(_tmp55F,sizeof(char),3)))),((_tmp55E[0]=Cyc_PP_textptr(tv->name),
Cyc_PP_cat(_tag_dyneither(_tmp55E,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL55:;}
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs);struct Cyc_PP_Doc*
Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){const char*_tmp562;const char*
_tmp561;const char*_tmp560;return Cyc_PP_egroup(((_tmp560="<",_tag_dyneither(
_tmp560,sizeof(char),2))),((_tmp561=">",_tag_dyneither(_tmp561,sizeof(char),2))),((
_tmp562=",",_tag_dyneither(_tmp562,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_ktvar2doc,tvs));}static struct _dyneither_ptr*Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar*tv);static struct _dyneither_ptr*Cyc_Absynpp_get_name(struct
Cyc_Absyn_Tvar*tv){return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct
Cyc_List_List*tvs);struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*
tvs){if(Cyc_Absynpp_print_all_kinds)return Cyc_Absynpp_ktvars2doc(tvs);{const char*
_tmp565;const char*_tmp564;const char*_tmp563;return Cyc_PP_egroup(((_tmp563="<",
_tag_dyneither(_tmp563,sizeof(char),2))),((_tmp564=">",_tag_dyneither(_tmp564,
sizeof(char),2))),((_tmp565=",",_tag_dyneither(_tmp565,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
tvs)));}}struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(
struct _tuple10*t);struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple10*t){return
Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(
struct Cyc_List_List*ts);struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*
ts){const char*_tmp568;const char*_tmp567;const char*_tmp566;return Cyc_PP_group(((
_tmp566="(",_tag_dyneither(_tmp566,sizeof(char),2))),((_tmp567=")",
_tag_dyneither(_tmp567,sizeof(char),2))),((_tmp568=",",_tag_dyneither(_tmp568,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple10*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}struct Cyc_PP_Doc*
Cyc_Absynpp_noncallatt2doc(void*att);struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(
void*att){void*_tmp92=att;_LL5F: if((int)_tmp92 != 0)goto _LL61;_LL60: goto _LL62;
_LL61: if((int)_tmp92 != 1)goto _LL63;_LL62: goto _LL64;_LL63: if((int)_tmp92 != 2)goto
_LL65;_LL64: return Cyc_PP_nil_doc();_LL65:;_LL66: return Cyc_PP_text(Cyc_Absyn_attribute2string(
att));_LL5E:;}struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts);
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){for(0;atts != 
0;atts=atts->tl){void*_tmp93=(void*)atts->hd;_LL68: if((int)_tmp93 != 0)goto _LL6A;
_LL69: {const char*_tmp569;return Cyc_PP_text(((_tmp569=" _stdcall ",
_tag_dyneither(_tmp569,sizeof(char),11))));}_LL6A: if((int)_tmp93 != 1)goto _LL6C;
_LL6B: {const char*_tmp56A;return Cyc_PP_text(((_tmp56A=" _cdecl ",_tag_dyneither(
_tmp56A,sizeof(char),9))));}_LL6C: if((int)_tmp93 != 2)goto _LL6E;_LL6D: {const char*
_tmp56B;return Cyc_PP_text(((_tmp56B=" _fastcall ",_tag_dyneither(_tmp56B,sizeof(
char),12))));}_LL6E:;_LL6F: goto _LL67;_LL67:;}return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*
Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts);struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(
struct Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;
atts2 != 0;atts2=atts2->tl){void*_tmp97=(void*)atts2->hd;_LL71: if((int)_tmp97 != 0)
goto _LL73;_LL72: goto _LL74;_LL73: if((int)_tmp97 != 1)goto _LL75;_LL74: goto _LL76;
_LL75: if((int)_tmp97 != 2)goto _LL77;_LL76: goto _LL70;_LL77:;_LL78: hasatt=1;goto
_LL70;_LL70:;}}if(!hasatt)return Cyc_PP_nil_doc();{const char*_tmp576;const char*
_tmp575;const char*_tmp574;const char*_tmp573;const char*_tmp572;struct Cyc_PP_Doc*
_tmp571[3];return(_tmp571[2]=Cyc_PP_text(((_tmp576=")",_tag_dyneither(_tmp576,
sizeof(char),2)))),((_tmp571[1]=Cyc_PP_group(((_tmp575="",_tag_dyneither(_tmp575,
sizeof(char),1))),((_tmp574="",_tag_dyneither(_tmp574,sizeof(char),1))),((
_tmp573=" ",_tag_dyneither(_tmp573,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,
atts)),((_tmp571[0]=Cyc_PP_text(((_tmp572=" __declspec(",_tag_dyneither(_tmp572,
sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp571,sizeof(struct Cyc_PP_Doc*),
3)))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a);struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(
void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts);struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*
atts){if(atts == 0)return Cyc_PP_nil_doc();{void*_tmp9E=Cyc_Cyclone_c_compiler;
_LL7A: if((int)_tmp9E != 1)goto _LL7C;_LL7B: return Cyc_Absynpp_noncallconv2doc(atts);
_LL7C: if((int)_tmp9E != 0)goto _LL79;_LL7D: {const char*_tmp57F;const char*_tmp57E;
const char*_tmp57D;const char*_tmp57C;struct Cyc_PP_Doc*_tmp57B[2];return(_tmp57B[1]=
Cyc_PP_group(((_tmp57F="((",_tag_dyneither(_tmp57F,sizeof(char),3))),((_tmp57E="))",
_tag_dyneither(_tmp57E,sizeof(char),3))),((_tmp57D=",",_tag_dyneither(_tmp57D,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp57B[0]=Cyc_PP_text(((
_tmp57C=" __attribute__",_tag_dyneither(_tmp57C,sizeof(char),15)))),Cyc_PP_cat(
_tag_dyneither(_tmp57B,sizeof(struct Cyc_PP_Doc*),2)))));}_LL79:;}}int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List*tms);int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{void*_tmpA4=(void*)tms->hd;_LL7F: if(*((int*)_tmpA4)!= 2)goto
_LL81;_LL80: return 1;_LL81: if(*((int*)_tmpA4)!= 5)goto _LL83;_LL82: {void*_tmpA5=
Cyc_Cyclone_c_compiler;_LL86: if((int)_tmpA5 != 0)goto _LL88;_LL87: return 0;_LL88:;
_LL89: return Cyc_Absynpp_next_is_pointer(tms->tl);_LL85:;}_LL83:;_LL84: return 0;
_LL7E:;}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);static struct Cyc_PP_Doc*
Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*Cyc_Absynpp_question();
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output){const char*_tmp580;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp580="{?}",_tag_dyneither(_tmp580,sizeof(char),4))),1);}
else{const char*_tmp581;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp581="?",_tag_dyneither(_tmp581,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_lb();static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){if(Cyc_PP_tex_output){const char*
_tmp582;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp582="{\\lb}",
_tag_dyneither(_tmp582,sizeof(char),6))),1);}else{const char*_tmp583;Cyc_Absynpp_cache_lb=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp583="{",_tag_dyneither(_tmp583,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}static struct
Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb();static
struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){if(
Cyc_PP_tex_output){const char*_tmp584;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp584="{\\rb}",_tag_dyneither(_tmp584,sizeof(char),6))),1);}else{const char*
_tmp585;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp585="}",
_tag_dyneither(_tmp585,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;static
struct Cyc_PP_Doc*Cyc_Absynpp_dollar();static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){if(Cyc_PP_tex_output){const char*
_tmp586;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp586="\\$",
_tag_dyneither(_tmp586,sizeof(char),3))),1);}else{const char*_tmp587;Cyc_Absynpp_cache_dollar=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp587="$",_tag_dyneither(_tmp587,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}struct
Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss);struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss){struct Cyc_PP_Doc*_tmp588[3];return(_tmp588[2]=Cyc_Absynpp_rb(),((_tmp588[1]=
Cyc_PP_seq(sep,ss),((_tmp588[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(
_tmp588,sizeof(struct Cyc_PP_Doc*),3)))))));}static void Cyc_Absynpp_print_tms(
struct Cyc_List_List*tms);static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){void*_tmpAF=(void*)tms->hd;void*_tmpB0;struct Cyc_List_List*
_tmpB1;_LL8B: if(*((int*)_tmpAF)!= 0)goto _LL8D;_LL8C:{const char*_tmp58B;void*
_tmp58A;(_tmp58A=0,Cyc_fprintf(Cyc_stderr,((_tmp58B="Carray_mod ",_tag_dyneither(
_tmp58B,sizeof(char),12))),_tag_dyneither(_tmp58A,sizeof(void*),0)));}goto _LL8A;
_LL8D: if(*((int*)_tmpAF)!= 1)goto _LL8F;_LL8E:{const char*_tmp58E;void*_tmp58D;(
_tmp58D=0,Cyc_fprintf(Cyc_stderr,((_tmp58E="ConstArray_mod ",_tag_dyneither(
_tmp58E,sizeof(char),16))),_tag_dyneither(_tmp58D,sizeof(void*),0)));}goto _LL8A;
_LL8F: if(*((int*)_tmpAF)!= 3)goto _LL91;_tmpB0=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpAF)->f1;if(*((int*)_tmpB0)!= 1)goto _LL91;_tmpB1=((struct Cyc_Absyn_WithTypes_struct*)
_tmpB0)->f1;_LL90:{const char*_tmp591;void*_tmp590;(_tmp590=0,Cyc_fprintf(Cyc_stderr,((
_tmp591="Function_mod(",_tag_dyneither(_tmp591,sizeof(char),14))),_tag_dyneither(
_tmp590,sizeof(void*),0)));}for(0;_tmpB1 != 0;_tmpB1=_tmpB1->tl){struct Cyc_Core_Opt*
_tmpB8=(*((struct _tuple1*)_tmpB1->hd)).f1;if(_tmpB8 == 0){const char*_tmp594;void*
_tmp593;(_tmp593=0,Cyc_fprintf(Cyc_stderr,((_tmp594="?",_tag_dyneither(_tmp594,
sizeof(char),2))),_tag_dyneither(_tmp593,sizeof(void*),0)));}else{void*_tmp595;(
_tmp595=0,Cyc_fprintf(Cyc_stderr,*((struct _dyneither_ptr*)_tmpB8->v),
_tag_dyneither(_tmp595,sizeof(void*),0)));}if(_tmpB1->tl != 0){const char*_tmp598;
void*_tmp597;(_tmp597=0,Cyc_fprintf(Cyc_stderr,((_tmp598=",",_tag_dyneither(
_tmp598,sizeof(char),2))),_tag_dyneither(_tmp597,sizeof(void*),0)));}}{const char*
_tmp59B;void*_tmp59A;(_tmp59A=0,Cyc_fprintf(Cyc_stderr,((_tmp59B=") ",
_tag_dyneither(_tmp59B,sizeof(char),3))),_tag_dyneither(_tmp59A,sizeof(void*),0)));}
goto _LL8A;_LL91: if(*((int*)_tmpAF)!= 3)goto _LL93;_LL92:{const char*_tmp59E;void*
_tmp59D;(_tmp59D=0,Cyc_fprintf(Cyc_stderr,((_tmp59E="Function_mod()",
_tag_dyneither(_tmp59E,sizeof(char),15))),_tag_dyneither(_tmp59D,sizeof(void*),0)));}
goto _LL8A;_LL93: if(*((int*)_tmpAF)!= 5)goto _LL95;_LL94:{const char*_tmp5A1;void*
_tmp5A0;(_tmp5A0=0,Cyc_fprintf(Cyc_stderr,((_tmp5A1="Attributes_mod ",
_tag_dyneither(_tmp5A1,sizeof(char),16))),_tag_dyneither(_tmp5A0,sizeof(void*),0)));}
goto _LL8A;_LL95: if(*((int*)_tmpAF)!= 4)goto _LL97;_LL96:{const char*_tmp5A4;void*
_tmp5A3;(_tmp5A3=0,Cyc_fprintf(Cyc_stderr,((_tmp5A4="TypeParams_mod ",
_tag_dyneither(_tmp5A4,sizeof(char),16))),_tag_dyneither(_tmp5A3,sizeof(void*),0)));}
goto _LL8A;_LL97: if(*((int*)_tmpAF)!= 2)goto _LL8A;_LL98:{const char*_tmp5A7;void*
_tmp5A6;(_tmp5A6=0,Cyc_fprintf(Cyc_stderr,((_tmp5A7="Pointer_mod ",
_tag_dyneither(_tmp5A7,sizeof(char),13))),_tag_dyneither(_tmp5A6,sizeof(void*),0)));}
goto _LL8A;_LL8A:;}{const char*_tmp5AA;void*_tmp5A9;(_tmp5A9=0,Cyc_fprintf(Cyc_stderr,((
_tmp5AA="\n",_tag_dyneither(_tmp5AA,sizeof(char),2))),_tag_dyneither(_tmp5A9,
sizeof(void*),0)));}}struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct
Cyc_PP_Doc*d,struct Cyc_List_List*tms);struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int
is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);const char*_tmp5AF;const
char*_tmp5AE;struct Cyc_PP_Doc*_tmp5AD[3];struct Cyc_PP_Doc*p_rest=(_tmp5AD[2]=Cyc_PP_text(((
_tmp5AF=")",_tag_dyneither(_tmp5AF,sizeof(char),2)))),((_tmp5AD[1]=rest,((
_tmp5AD[0]=Cyc_PP_text(((_tmp5AE="(",_tag_dyneither(_tmp5AE,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp5AD,sizeof(struct Cyc_PP_Doc*),3)))))));void*_tmpCA=(
void*)tms->hd;struct Cyc_Absyn_Conref*_tmpCB;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Conref*
_tmpCD;void*_tmpCE;struct Cyc_List_List*_tmpCF;struct Cyc_List_List*_tmpD0;struct
Cyc_Position_Segment*_tmpD1;int _tmpD2;struct Cyc_Absyn_PtrAtts _tmpD3;void*_tmpD4;
struct Cyc_Absyn_Conref*_tmpD5;struct Cyc_Absyn_Conref*_tmpD6;struct Cyc_Absyn_Conref*
_tmpD7;struct Cyc_Absyn_Tqual _tmpD8;_LL9A: if(*((int*)_tmpCA)!= 0)goto _LL9C;_tmpCB=((
struct Cyc_Absyn_Carray_mod_struct*)_tmpCA)->f1;_LL9B: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{const char*_tmp5B4;const char*_tmp5B3;struct Cyc_PP_Doc*
_tmp5B2[2];return(_tmp5B2[1]=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpCB)?Cyc_PP_text(((_tmp5B3="[]@zeroterm",_tag_dyneither(_tmp5B3,sizeof(char),
12)))): Cyc_PP_text(((_tmp5B4="[]",_tag_dyneither(_tmp5B4,sizeof(char),3)))),((
_tmp5B2[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp5B2,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL9C: if(*((int*)_tmpCA)!= 1)goto _LL9E;_tmpCC=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpCA)->f1;_tmpCD=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCA)->f2;_LL9D:
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{const char*_tmp5BB;const char*
_tmp5BA;const char*_tmp5B9;struct Cyc_PP_Doc*_tmp5B8[4];return(_tmp5B8[3]=((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpCD)?Cyc_PP_text(((
_tmp5BA="]@zeroterm",_tag_dyneither(_tmp5BA,sizeof(char),11)))): Cyc_PP_text(((
_tmp5BB="]",_tag_dyneither(_tmp5BB,sizeof(char),2)))),((_tmp5B8[2]=Cyc_Absynpp_exp2doc(
_tmpCC),((_tmp5B8[1]=Cyc_PP_text(((_tmp5B9="[",_tag_dyneither(_tmp5B9,sizeof(
char),2)))),((_tmp5B8[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp5B8,sizeof(struct Cyc_PP_Doc*),
4)))))))));}_LL9E: if(*((int*)_tmpCA)!= 3)goto _LLA0;_tmpCE=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpCA)->f1;_LL9F: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{void*
_tmpE0=_tmpCE;struct Cyc_List_List*_tmpE1;int _tmpE2;struct Cyc_Absyn_VarargInfo*
_tmpE3;struct Cyc_Core_Opt*_tmpE4;struct Cyc_List_List*_tmpE5;struct Cyc_List_List*
_tmpE6;struct Cyc_Position_Segment*_tmpE7;_LLA7: if(*((int*)_tmpE0)!= 1)goto _LLA9;
_tmpE1=((struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f1;_tmpE2=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE0)->f2;_tmpE3=((struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f3;_tmpE4=((
struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f4;_tmpE5=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE0)->f5;_LLA8: {struct Cyc_PP_Doc*_tmp5BC[2];return(_tmp5BC[1]=Cyc_Absynpp_funargs2doc(
_tmpE1,_tmpE2,_tmpE3,_tmpE4,_tmpE5),((_tmp5BC[0]=rest,Cyc_PP_cat(_tag_dyneither(
_tmp5BC,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA9: if(*((int*)_tmpE0)!= 0)goto _LLA6;
_tmpE6=((struct Cyc_Absyn_NoTypes_struct*)_tmpE0)->f1;_tmpE7=((struct Cyc_Absyn_NoTypes_struct*)
_tmpE0)->f2;_LLAA: {const char*_tmp5C3;const char*_tmp5C2;const char*_tmp5C1;struct
Cyc_PP_Doc*_tmp5C0[2];return(_tmp5C0[1]=Cyc_PP_group(((_tmp5C3="(",
_tag_dyneither(_tmp5C3,sizeof(char),2))),((_tmp5C2=")",_tag_dyneither(_tmp5C2,
sizeof(char),2))),((_tmp5C1=",",_tag_dyneither(_tmp5C1,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,_tmpE6)),((_tmp5C0[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp5C0,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA6:;}_LLA0: if(*((int*)
_tmpCA)!= 5)goto _LLA2;_tmpCF=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpCA)->f2;
_LLA1: {void*_tmpED=Cyc_Cyclone_c_compiler;_LLAC: if((int)_tmpED != 0)goto _LLAE;
_LLAD: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{struct Cyc_PP_Doc*
_tmp5C4[2];return(_tmp5C4[1]=Cyc_Absynpp_atts2doc(_tmpCF),((_tmp5C4[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp5C4,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAE: if((int)_tmpED != 
1)goto _LLAB;_LLAF: if(Cyc_Absynpp_next_is_pointer(tms->tl)){struct Cyc_PP_Doc*
_tmp5C5[2];return(_tmp5C5[1]=rest,((_tmp5C5[0]=Cyc_Absynpp_callconv2doc(_tmpCF),
Cyc_PP_cat(_tag_dyneither(_tmp5C5,sizeof(struct Cyc_PP_Doc*),2)))));}return rest;
_LLAB:;}_LLA2: if(*((int*)_tmpCA)!= 4)goto _LLA4;_tmpD0=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpCA)->f1;_tmpD1=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCA)->f2;_tmpD2=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCA)->f3;_LLA3: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;if(_tmpD2){struct Cyc_PP_Doc*_tmp5C6[2];return(_tmp5C6[1]=
Cyc_Absynpp_ktvars2doc(_tmpD0),((_tmp5C6[0]=rest,Cyc_PP_cat(_tag_dyneither(
_tmp5C6,sizeof(struct Cyc_PP_Doc*),2)))));}else{struct Cyc_PP_Doc*_tmp5C7[2];
return(_tmp5C7[1]=Cyc_Absynpp_tvars2doc(_tmpD0),((_tmp5C7[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp5C7,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA4: if(*((int*)_tmpCA)
!= 2)goto _LL99;_tmpD3=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCA)->f1;_tmpD4=(
void*)_tmpD3.rgn;_tmpD5=_tmpD3.nullable;_tmpD6=_tmpD3.bounds;_tmpD7=_tmpD3.zero_term;
_tmpD8=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCA)->f2;_LLA5: {struct Cyc_PP_Doc*
ptr;{void*_tmpF2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmpD6);struct Cyc_Absyn_Exp*
_tmpF3;_LLB1: if((int)_tmpF2 != 0)goto _LLB3;_LLB2: ptr=Cyc_Absynpp_question();goto
_LLB0;_LLB3: if(_tmpF2 <= (void*)1)goto _LLB0;if(*((int*)_tmpF2)!= 0)goto _LLB0;
_tmpF3=((struct Cyc_Absyn_Upper_b_struct*)_tmpF2)->f1;_LLB4:{const char*_tmp5C9;
const char*_tmp5C8;ptr=Cyc_PP_text(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmpD5)?(_tmp5C9="*",_tag_dyneither(_tmp5C9,sizeof(char),2)):((_tmp5C8="@",
_tag_dyneither(_tmp5C8,sizeof(char),2))));}{unsigned int _tmpF7;int _tmpF8;struct
_tuple4 _tmpF6=Cyc_Evexp_eval_const_uint_exp(_tmpF3);_tmpF7=_tmpF6.f1;_tmpF8=
_tmpF6.f2;if(!_tmpF8  || _tmpF7 != 1){struct Cyc_PP_Doc*_tmp5CA[4];ptr=((_tmp5CA[3]=
Cyc_Absynpp_rb(),((_tmp5CA[2]=Cyc_Absynpp_exp2doc(_tmpF3),((_tmp5CA[1]=Cyc_Absynpp_lb(),((
_tmp5CA[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp5CA,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB0;}_LLB0:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpD7)){const char*_tmp5CD;
struct Cyc_PP_Doc*_tmp5CC[2];ptr=((_tmp5CC[1]=Cyc_PP_text(((_tmp5CD="@zeroterm ",
_tag_dyneither(_tmp5CD,sizeof(char),11)))),((_tmp5CC[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp5CC,sizeof(struct Cyc_PP_Doc*),2))))));}else{if(is_char_ptr
 && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpD7)){
const char*_tmp5D0;struct Cyc_PP_Doc*_tmp5CF[2];ptr=((_tmp5CF[1]=Cyc_PP_text(((
_tmp5D0="@nozeroterm ",_tag_dyneither(_tmp5D0,sizeof(char),13)))),((_tmp5CF[0]=
ptr,Cyc_PP_cat(_tag_dyneither(_tmp5CF,sizeof(struct Cyc_PP_Doc*),2))))));}}}{void*
_tmpFE=Cyc_Tcutil_compress(_tmpD4);_LLB6: if((int)_tmpFE != 2)goto _LLB8;_LLB7: goto
_LLB5;_LLB8:;_LLB9: {const char*_tmp5D3;struct Cyc_PP_Doc*_tmp5D2[3];ptr=((_tmp5D2[
2]=Cyc_PP_text(((_tmp5D3=" ",_tag_dyneither(_tmp5D3,sizeof(char),2)))),((_tmp5D2[
1]=Cyc_Absynpp_typ2doc(_tmpD4),((_tmp5D2[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp5D2,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}{struct Cyc_PP_Doc*_tmp5D4[2];
ptr=((_tmp5D4[1]=Cyc_Absynpp_tqual2doc(_tmpD8),((_tmp5D4[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp5D4,sizeof(struct Cyc_PP_Doc*),2))))));}{struct Cyc_PP_Doc*
_tmp5D5[2];return(_tmp5D5[1]=rest,((_tmp5D5[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp5D5,sizeof(struct Cyc_PP_Doc*),2)))));}}_LL99:;}}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){void*_tmp106=Cyc_Tcutil_compress(
t);_LLBB: if((int)_tmp106 != 2)goto _LLBD;_LLBC: {const char*_tmp5D6;return Cyc_PP_text(((
_tmp5D6="`H",_tag_dyneither(_tmp5D6,sizeof(char),3))));}_LLBD: if((int)_tmp106 != 
3)goto _LLBF;_LLBE: {const char*_tmp5D7;return Cyc_PP_text(((_tmp5D7="`U",
_tag_dyneither(_tmp5D7,sizeof(char),3))));}_LLBF:;_LLC0: return Cyc_Absynpp_ntyp2doc(
t);_LLBA:;}static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct
Cyc_List_List**effects,void*t);static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**
rgions,struct Cyc_List_List**effects,void*t){void*_tmp109=Cyc_Tcutil_compress(t);
void*_tmp10A;struct Cyc_List_List*_tmp10B;_LLC2: if(_tmp109 <= (void*)4)goto _LLC6;
if(*((int*)_tmp109)!= 19)goto _LLC4;_tmp10A=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp109)->f1;_LLC3:{struct Cyc_List_List*_tmp5D8;*rgions=((_tmp5D8=_cycalloc(
sizeof(*_tmp5D8)),((_tmp5D8->hd=Cyc_Absynpp_rgn2doc(_tmp10A),((_tmp5D8->tl=*
rgions,_tmp5D8))))));}goto _LLC1;_LLC4: if(*((int*)_tmp109)!= 20)goto _LLC6;_tmp10B=((
struct Cyc_Absyn_JoinEff_struct*)_tmp109)->f1;_LLC5: for(0;_tmp10B != 0;_tmp10B=
_tmp10B->tl){Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp10B->hd);}goto
_LLC1;_LLC6:;_LLC7:{struct Cyc_List_List*_tmp5D9;*effects=((_tmp5D9=_cycalloc(
sizeof(*_tmp5D9)),((_tmp5D9->hd=Cyc_Absynpp_typ2doc(t),((_tmp5D9->tl=*effects,
_tmp5D9))))));}goto _LLC1;_LLC1:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t);
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct Cyc_List_List*rgions=0;struct
Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(& rgions,& effects,t);rgions=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);effects=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);if(
rgions == 0  && effects != 0){const char*_tmp5DC;const char*_tmp5DB;const char*_tmp5DA;
return Cyc_PP_group(((_tmp5DA="",_tag_dyneither(_tmp5DA,sizeof(char),1))),((
_tmp5DB="",_tag_dyneither(_tmp5DB,sizeof(char),1))),((_tmp5DC="+",_tag_dyneither(
_tmp5DC,sizeof(char),2))),effects);}else{const char*_tmp5DD;struct Cyc_PP_Doc*
_tmp111=Cyc_Absynpp_group_braces(((_tmp5DD=",",_tag_dyneither(_tmp5DD,sizeof(
char),2))),rgions);struct Cyc_List_List*_tmp5E1;const char*_tmp5E0;const char*
_tmp5DF;const char*_tmp5DE;return Cyc_PP_group(((_tmp5DE="",_tag_dyneither(_tmp5DE,
sizeof(char),1))),((_tmp5DF="",_tag_dyneither(_tmp5DF,sizeof(char),1))),((
_tmp5E0="+",_tag_dyneither(_tmp5E0,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((
_tmp5E1=_cycalloc(sizeof(*_tmp5E1)),((_tmp5E1->hd=_tmp111,((_tmp5E1->tl=0,
_tmp5E1))))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(void*k);struct Cyc_PP_Doc*
Cyc_Absynpp_aggr_kind2doc(void*k){void*_tmp117=k;_LLC9: if((int)_tmp117 != 0)goto
_LLCB;_LLCA: {const char*_tmp5E2;return Cyc_PP_text(((_tmp5E2="struct ",
_tag_dyneither(_tmp5E2,sizeof(char),8))));}_LLCB: if((int)_tmp117 != 1)goto _LLC8;
_LLCC: {const char*_tmp5E3;return Cyc_PP_text(((_tmp5E3="union ",_tag_dyneither(
_tmp5E3,sizeof(char),7))));}_LLC8:;}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*s;{void*_tmp11A=t;
struct Cyc_Core_Opt*_tmp11B;struct Cyc_Core_Opt*_tmp11C;int _tmp11D;struct Cyc_Core_Opt*
_tmp11E;struct Cyc_Absyn_Tvar*_tmp11F;struct Cyc_Absyn_DatatypeInfo _tmp120;union
Cyc_Absyn_DatatypeInfoU_union _tmp121;struct Cyc_List_List*_tmp122;struct Cyc_Core_Opt*
_tmp123;struct Cyc_Absyn_DatatypeFieldInfo _tmp124;union Cyc_Absyn_DatatypeFieldInfoU_union
_tmp125;struct Cyc_List_List*_tmp126;void*_tmp127;void*_tmp128;int _tmp129;struct
Cyc_List_List*_tmp12A;struct Cyc_Absyn_AggrInfo _tmp12B;union Cyc_Absyn_AggrInfoU_union
_tmp12C;struct Cyc_List_List*_tmp12D;void*_tmp12E;struct Cyc_List_List*_tmp12F;
struct Cyc_List_List*_tmp130;struct _tuple0*_tmp131;struct Cyc_Absyn_Exp*_tmp132;
struct _tuple0*_tmp133;struct Cyc_List_List*_tmp134;struct Cyc_Absyn_Typedefdecl*
_tmp135;void*_tmp136;void*_tmp137;void*_tmp138;void*_tmp139;void*_tmp13A;_LLCE:
if(_tmp11A <= (void*)4)goto _LLD4;if(*((int*)_tmp11A)!= 7)goto _LLD0;_LLCF: {const
char*_tmp5E4;return Cyc_PP_text(((_tmp5E4="[[[array]]]",_tag_dyneither(_tmp5E4,
sizeof(char),12))));}_LLD0: if(*((int*)_tmp11A)!= 8)goto _LLD2;_LLD1: return Cyc_PP_nil_doc();
_LLD2: if(*((int*)_tmp11A)!= 4)goto _LLD4;_LLD3: return Cyc_PP_nil_doc();_LLD4: if((
int)_tmp11A != 0)goto _LLD6;_LLD5:{const char*_tmp5E5;s=Cyc_PP_text(((_tmp5E5="void",
_tag_dyneither(_tmp5E5,sizeof(char),5))));}goto _LLCD;_LLD6: if(_tmp11A <= (void*)4)
goto _LLE0;if(*((int*)_tmp11A)!= 0)goto _LLD8;_tmp11B=((struct Cyc_Absyn_Evar_struct*)
_tmp11A)->f1;_tmp11C=((struct Cyc_Absyn_Evar_struct*)_tmp11A)->f2;_tmp11D=((
struct Cyc_Absyn_Evar_struct*)_tmp11A)->f3;_tmp11E=((struct Cyc_Absyn_Evar_struct*)
_tmp11A)->f4;_LLD7: if(_tmp11C != 0)return Cyc_Absynpp_ntyp2doc((void*)_tmp11C->v);
else{const char*_tmp5F5;struct Cyc_Int_pa_struct _tmp5F4;void*_tmp5F3[1];const char*
_tmp5F2;const char*_tmp5F1;const char*_tmp5F0;const char*_tmp5EF;struct Cyc_PP_Doc*
_tmp5EE[6];s=((_tmp5EE[5]=_tmp11B == 0?Cyc_Absynpp_question(): Cyc_Absynpp_kind2doc((
void*)_tmp11B->v),((_tmp5EE[4]=Cyc_PP_text(((_tmp5F5=")::",_tag_dyneither(
_tmp5F5,sizeof(char),4)))),((_tmp5EE[3]=(!Cyc_Absynpp_print_full_evars  || 
_tmp11E == 0)?Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)
_tmp11E->v),((_tmp5EE[2]=Cyc_PP_text((struct _dyneither_ptr)((_tmp5F4.tag=1,((
_tmp5F4.f1=(unsigned long)_tmp11D,((_tmp5F3[0]=& _tmp5F4,Cyc_aprintf(((_tmp5F2="%d",
_tag_dyneither(_tmp5F2,sizeof(char),3))),_tag_dyneither(_tmp5F3,sizeof(void*),1))))))))),((
_tmp5EE[1]=Cyc_PP_text(((_tmp5F1="(",_tag_dyneither(_tmp5F1,sizeof(char),2)))),((
_tmp5EE[0]=Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp5EF="\\%",
_tag_dyneither(_tmp5EF,sizeof(char),3)))): Cyc_PP_text(((_tmp5F0="%",
_tag_dyneither(_tmp5F0,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp5EE,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}goto _LLCD;_LLD8: if(*((int*)_tmp11A)!= 
1)goto _LLDA;_tmp11F=((struct Cyc_Absyn_VarType_struct*)_tmp11A)->f1;_LLD9: s=Cyc_PP_textptr(
_tmp11F->name);if(Cyc_Absynpp_print_all_kinds){const char*_tmp5F8;struct Cyc_PP_Doc*
_tmp5F7[3];s=((_tmp5F7[2]=Cyc_Absynpp_kindbound2doc((void*)_tmp11F->kind),((
_tmp5F7[1]=Cyc_PP_text(((_tmp5F8="::",_tag_dyneither(_tmp5F8,sizeof(char),3)))),((
_tmp5F7[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp5F7,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(_tmp11F)){const char*
_tmp5FD;const char*_tmp5FC;struct Cyc_PP_Doc*_tmp5FB[3];s=((_tmp5FB[2]=Cyc_PP_text(((
_tmp5FD=" */",_tag_dyneither(_tmp5FD,sizeof(char),4)))),((_tmp5FB[1]=s,((_tmp5FB[
0]=Cyc_PP_text(((_tmp5FC="_ /* ",_tag_dyneither(_tmp5FC,sizeof(char),6)))),Cyc_PP_cat(
_tag_dyneither(_tmp5FB,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCD;_LLDA: if(*((
int*)_tmp11A)!= 2)goto _LLDC;_tmp120=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp11A)->f1;_tmp121=_tmp120.datatype_info;_tmp122=_tmp120.targs;_tmp123=_tmp120.rgn;
_LLDB:{union Cyc_Absyn_DatatypeInfoU_union _tmp14A=_tmp121;struct Cyc_Absyn_UnknownDatatypeInfo
_tmp14B;struct _tuple0*_tmp14C;int _tmp14D;struct Cyc_Absyn_Datatypedecl**_tmp14E;
struct Cyc_Absyn_Datatypedecl*_tmp14F;struct Cyc_Absyn_Datatypedecl _tmp150;struct
_tuple0*_tmp151;int _tmp152;_LL103: if((_tmp14A.UnknownDatatype).tag != 0)goto
_LL105;_tmp14B=(_tmp14A.UnknownDatatype).f1;_tmp14C=_tmp14B.name;_tmp14D=_tmp14B.is_extensible;
_LL104: _tmp151=_tmp14C;_tmp152=_tmp14D;goto _LL106;_LL105: if((_tmp14A.KnownDatatype).tag
!= 1)goto _LL102;_tmp14E=(_tmp14A.KnownDatatype).f1;_tmp14F=*_tmp14E;_tmp150=*
_tmp14F;_tmp151=_tmp150.name;_tmp152=_tmp150.is_extensible;_LL106: {const char*
_tmp5FE;struct Cyc_PP_Doc*_tmp153=Cyc_PP_text(((_tmp5FE="datatype ",
_tag_dyneither(_tmp5FE,sizeof(char),10))));const char*_tmp5FF;struct Cyc_PP_Doc*
_tmp154=_tmp152?Cyc_PP_text(((_tmp5FF="@extensible ",_tag_dyneither(_tmp5FF,
sizeof(char),13)))): Cyc_PP_nil_doc();void*r=(unsigned int)_tmp123?(void*)_tmp123->v:(
void*)2;{void*_tmp155=Cyc_Tcutil_compress(r);_LL108: if((int)_tmp155 != 2)goto
_LL10A;_LL109:{struct Cyc_PP_Doc*_tmp600[4];s=((_tmp600[3]=Cyc_Absynpp_tps2doc(
_tmp122),((_tmp600[2]=Cyc_Absynpp_qvar2doc(_tmp151),((_tmp600[1]=_tmp153,((
_tmp600[0]=_tmp154,Cyc_PP_cat(_tag_dyneither(_tmp600,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL107;_LL10A:;_LL10B:{const char*_tmp603;struct Cyc_PP_Doc*_tmp602[6];s=((
_tmp602[5]=Cyc_Absynpp_tps2doc(_tmp122),((_tmp602[4]=Cyc_Absynpp_qvar2doc(
_tmp151),((_tmp602[3]=Cyc_PP_text(((_tmp603=" ",_tag_dyneither(_tmp603,sizeof(
char),2)))),((_tmp602[2]=Cyc_Absynpp_typ2doc(r),((_tmp602[1]=_tmp153,((_tmp602[0]=
_tmp154,Cyc_PP_cat(_tag_dyneither(_tmp602,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
goto _LL107;_LL107:;}goto _LL102;}_LL102:;}goto _LLCD;_LLDC: if(*((int*)_tmp11A)!= 3)
goto _LLDE;_tmp124=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp11A)->f1;
_tmp125=_tmp124.field_info;_tmp126=_tmp124.targs;_LLDD:{union Cyc_Absyn_DatatypeFieldInfoU_union
_tmp15B=_tmp125;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp15C;struct _tuple0*
_tmp15D;struct _tuple0*_tmp15E;int _tmp15F;struct Cyc_Absyn_Datatypedecl*_tmp160;
struct Cyc_Absyn_Datatypedecl _tmp161;struct _tuple0*_tmp162;int _tmp163;struct Cyc_Absyn_Datatypefield*
_tmp164;struct Cyc_Absyn_Datatypefield _tmp165;struct _tuple0*_tmp166;_LL10D: if((
_tmp15B.UnknownDatatypefield).tag != 0)goto _LL10F;_tmp15C=(_tmp15B.UnknownDatatypefield).f1;
_tmp15D=_tmp15C.datatype_name;_tmp15E=_tmp15C.field_name;_tmp15F=_tmp15C.is_extensible;
_LL10E: _tmp162=_tmp15D;_tmp163=_tmp15F;_tmp166=_tmp15E;goto _LL110;_LL10F: if((
_tmp15B.KnownDatatypefield).tag != 1)goto _LL10C;_tmp160=(_tmp15B.KnownDatatypefield).f1;
_tmp161=*_tmp160;_tmp162=_tmp161.name;_tmp163=_tmp161.is_extensible;_tmp164=(
_tmp15B.KnownDatatypefield).f2;_tmp165=*_tmp164;_tmp166=_tmp165.name;_LL110: {
const char*_tmp605;const char*_tmp604;struct Cyc_PP_Doc*_tmp167=Cyc_PP_text(_tmp163?(
_tmp605="@extensible datatype ",_tag_dyneither(_tmp605,sizeof(char),22)):((
_tmp604="datatype ",_tag_dyneither(_tmp604,sizeof(char),10))));{const char*
_tmp608;struct Cyc_PP_Doc*_tmp607[4];s=((_tmp607[3]=Cyc_Absynpp_qvar2doc(_tmp166),((
_tmp607[2]=Cyc_PP_text(((_tmp608=".",_tag_dyneither(_tmp608,sizeof(char),2)))),((
_tmp607[1]=Cyc_Absynpp_qvar2doc(_tmp162),((_tmp607[0]=_tmp167,Cyc_PP_cat(
_tag_dyneither(_tmp607,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL10C;}_LL10C:;}
goto _LLCD;_LLDE: if(*((int*)_tmp11A)!= 5)goto _LLE0;_tmp127=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp11A)->f1;_tmp128=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp11A)->f2;_LLDF: {
struct _dyneither_ptr sns;struct _dyneither_ptr ts;{void*_tmp16C=_tmp127;_LL112: if((
int)_tmp16C != 2)goto _LL114;_LL113: goto _LL115;_LL114: if((int)_tmp16C != 0)goto
_LL116;_LL115:{const char*_tmp609;sns=((_tmp609="",_tag_dyneither(_tmp609,sizeof(
char),1)));}goto _LL111;_LL116: if((int)_tmp16C != 1)goto _LL111;_LL117:{const char*
_tmp60A;sns=((_tmp60A="unsigned ",_tag_dyneither(_tmp60A,sizeof(char),10)));}
goto _LL111;_LL111:;}{void*_tmp16F=_tmp128;_LL119: if((int)_tmp16F != 0)goto _LL11B;
_LL11A:{void*_tmp170=_tmp127;_LL124: if((int)_tmp170 != 2)goto _LL126;_LL125:{const
char*_tmp60B;sns=((_tmp60B="",_tag_dyneither(_tmp60B,sizeof(char),1)));}goto
_LL123;_LL126: if((int)_tmp170 != 0)goto _LL128;_LL127:{const char*_tmp60C;sns=((
_tmp60C="signed ",_tag_dyneither(_tmp60C,sizeof(char),8)));}goto _LL123;_LL128:
if((int)_tmp170 != 1)goto _LL123;_LL129:{const char*_tmp60D;sns=((_tmp60D="unsigned ",
_tag_dyneither(_tmp60D,sizeof(char),10)));}goto _LL123;_LL123:;}{const char*
_tmp60E;ts=((_tmp60E="char",_tag_dyneither(_tmp60E,sizeof(char),5)));}goto _LL118;
_LL11B: if((int)_tmp16F != 1)goto _LL11D;_LL11C:{const char*_tmp60F;ts=((_tmp60F="short",
_tag_dyneither(_tmp60F,sizeof(char),6)));}goto _LL118;_LL11D: if((int)_tmp16F != 2)
goto _LL11F;_LL11E:{const char*_tmp610;ts=((_tmp610="int",_tag_dyneither(_tmp610,
sizeof(char),4)));}goto _LL118;_LL11F: if((int)_tmp16F != 3)goto _LL121;_LL120:{
const char*_tmp611;ts=((_tmp611="long",_tag_dyneither(_tmp611,sizeof(char),5)));}
goto _LL118;_LL121: if((int)_tmp16F != 4)goto _LL118;_LL122:{void*_tmp178=Cyc_Cyclone_c_compiler;
_LL12B: if((int)_tmp178 != 0)goto _LL12D;_LL12C:{const char*_tmp612;ts=((_tmp612="long long",
_tag_dyneither(_tmp612,sizeof(char),10)));}goto _LL12A;_LL12D: if((int)_tmp178 != 1)
goto _LL12A;_LL12E:{const char*_tmp613;ts=((_tmp613="__int64",_tag_dyneither(
_tmp613,sizeof(char),8)));}goto _LL12A;_LL12A:;}goto _LL118;_LL118:;}{const char*
_tmp618;void*_tmp617[2];struct Cyc_String_pa_struct _tmp616;struct Cyc_String_pa_struct
_tmp615;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp615.tag=0,((_tmp615.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)ts),((_tmp616.tag=0,((_tmp616.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)sns),((_tmp617[0]=& _tmp616,((_tmp617[1]=&
_tmp615,Cyc_aprintf(((_tmp618="%s%s",_tag_dyneither(_tmp618,sizeof(char),5))),
_tag_dyneither(_tmp617,sizeof(void*),2)))))))))))))));}goto _LLCD;}_LLE0: if((int)
_tmp11A != 1)goto _LLE2;_LLE1:{const char*_tmp619;s=Cyc_PP_text(((_tmp619="float",
_tag_dyneither(_tmp619,sizeof(char),6))));}goto _LLCD;_LLE2: if(_tmp11A <= (void*)4)
goto _LLF8;if(*((int*)_tmp11A)!= 6)goto _LLE4;_tmp129=((struct Cyc_Absyn_DoubleType_struct*)
_tmp11A)->f1;_LLE3: if(_tmp129){const char*_tmp61A;s=Cyc_PP_text(((_tmp61A="long double",
_tag_dyneither(_tmp61A,sizeof(char),12))));}else{const char*_tmp61B;s=Cyc_PP_text(((
_tmp61B="double",_tag_dyneither(_tmp61B,sizeof(char),7))));}goto _LLCD;_LLE4: if(*((
int*)_tmp11A)!= 9)goto _LLE6;_tmp12A=((struct Cyc_Absyn_TupleType_struct*)_tmp11A)->f1;
_LLE5:{struct Cyc_PP_Doc*_tmp61C[2];s=((_tmp61C[1]=Cyc_Absynpp_args2doc(_tmp12A),((
_tmp61C[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp61C,sizeof(struct
Cyc_PP_Doc*),2))))));}goto _LLCD;_LLE6: if(*((int*)_tmp11A)!= 10)goto _LLE8;_tmp12B=((
struct Cyc_Absyn_AggrType_struct*)_tmp11A)->f1;_tmp12C=_tmp12B.aggr_info;_tmp12D=
_tmp12B.targs;_LLE7: {void*_tmp184;struct _tuple0*_tmp185;struct _tuple3 _tmp183=
Cyc_Absyn_aggr_kinded_name(_tmp12C);_tmp184=_tmp183.f1;_tmp185=_tmp183.f2;{
struct Cyc_PP_Doc*_tmp61D[3];s=((_tmp61D[2]=Cyc_Absynpp_tps2doc(_tmp12D),((
_tmp61D[1]=Cyc_Absynpp_qvar2doc(_tmp185),((_tmp61D[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp184),Cyc_PP_cat(_tag_dyneither(_tmp61D,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;}_LLE8: if(*((int*)_tmp11A)!= 11)goto _LLEA;_tmp12E=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp11A)->f1;_tmp12F=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp11A)->f2;_LLE9:{
struct Cyc_PP_Doc*_tmp61E[4];s=((_tmp61E[3]=Cyc_Absynpp_rb(),((_tmp61E[2]=Cyc_PP_nest(
2,Cyc_Absynpp_aggrfields2doc(_tmp12F)),((_tmp61E[1]=Cyc_Absynpp_lb(),((_tmp61E[0]=
Cyc_Absynpp_aggr_kind2doc(_tmp12E),Cyc_PP_cat(_tag_dyneither(_tmp61E,sizeof(
struct Cyc_PP_Doc*),4))))))))));}goto _LLCD;_LLEA: if(*((int*)_tmp11A)!= 13)goto
_LLEC;_tmp130=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp11A)->f1;_LLEB:{const
char*_tmp621;struct Cyc_PP_Doc*_tmp620[4];s=((_tmp620[3]=Cyc_Absynpp_rb(),((
_tmp620[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp130)),((_tmp620[1]=Cyc_Absynpp_lb(),((
_tmp620[0]=Cyc_PP_text(((_tmp621="enum ",_tag_dyneither(_tmp621,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp620,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LLCD;_LLEC: if(*((int*)_tmp11A)!= 12)goto _LLEE;_tmp131=((struct Cyc_Absyn_EnumType_struct*)
_tmp11A)->f1;_LLED:{const char*_tmp624;struct Cyc_PP_Doc*_tmp623[2];s=((_tmp623[1]=
Cyc_Absynpp_qvar2doc(_tmp131),((_tmp623[0]=Cyc_PP_text(((_tmp624="enum ",
_tag_dyneither(_tmp624,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp623,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLCD;_LLEE: if(*((int*)_tmp11A)!= 17)goto
_LLF0;_tmp132=((struct Cyc_Absyn_ValueofType_struct*)_tmp11A)->f1;_LLEF:{const
char*_tmp629;const char*_tmp628;struct Cyc_PP_Doc*_tmp627[3];s=((_tmp627[2]=Cyc_PP_text(((
_tmp629=")",_tag_dyneither(_tmp629,sizeof(char),2)))),((_tmp627[1]=Cyc_Absynpp_exp2doc(
_tmp132),((_tmp627[0]=Cyc_PP_text(((_tmp628="valueof_t(",_tag_dyneither(_tmp628,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp627,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LLCD;_LLF0: if(*((int*)_tmp11A)!= 16)goto _LLF2;_tmp133=((struct
Cyc_Absyn_TypedefType_struct*)_tmp11A)->f1;_tmp134=((struct Cyc_Absyn_TypedefType_struct*)
_tmp11A)->f2;_tmp135=((struct Cyc_Absyn_TypedefType_struct*)_tmp11A)->f3;_LLF1:{
struct Cyc_PP_Doc*_tmp62A[2];s=((_tmp62A[1]=Cyc_Absynpp_tps2doc(_tmp134),((
_tmp62A[0]=Cyc_Absynpp_qvar2doc(_tmp133),Cyc_PP_cat(_tag_dyneither(_tmp62A,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLCD;_LLF2: if(*((int*)_tmp11A)!= 14)goto
_LLF4;_tmp136=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp11A)->f1;_LLF3:{
const char*_tmp62F;const char*_tmp62E;struct Cyc_PP_Doc*_tmp62D[3];s=((_tmp62D[2]=
Cyc_PP_text(((_tmp62F=">",_tag_dyneither(_tmp62F,sizeof(char),2)))),((_tmp62D[1]=
Cyc_Absynpp_rgn2doc(_tmp136),((_tmp62D[0]=Cyc_PP_text(((_tmp62E="region_t<",
_tag_dyneither(_tmp62E,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp62D,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCD;_LLF4: if(*((int*)_tmp11A)!= 15)
goto _LLF6;_tmp137=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp11A)->f1;
_tmp138=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp11A)->f2;_LLF5:{const
char*_tmp636;const char*_tmp635;const char*_tmp634;struct Cyc_PP_Doc*_tmp633[5];s=((
_tmp633[4]=Cyc_PP_text(((_tmp636=">",_tag_dyneither(_tmp636,sizeof(char),2)))),((
_tmp633[3]=Cyc_Absynpp_rgn2doc(_tmp138),((_tmp633[2]=Cyc_PP_text(((_tmp635=",",
_tag_dyneither(_tmp635,sizeof(char),2)))),((_tmp633[1]=Cyc_Absynpp_rgn2doc(
_tmp137),((_tmp633[0]=Cyc_PP_text(((_tmp634="dynregion_t<",_tag_dyneither(
_tmp634,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp633,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LLCD;_LLF6: if(*((int*)_tmp11A)!= 18)goto _LLF8;_tmp139=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp11A)->f1;_LLF7:{const char*_tmp63B;const char*
_tmp63A;struct Cyc_PP_Doc*_tmp639[3];s=((_tmp639[2]=Cyc_PP_text(((_tmp63B=">",
_tag_dyneither(_tmp63B,sizeof(char),2)))),((_tmp639[1]=Cyc_Absynpp_typ2doc(
_tmp139),((_tmp639[0]=Cyc_PP_text(((_tmp63A="tag_t<",_tag_dyneither(_tmp63A,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp639,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLF8: if((int)_tmp11A != 3)goto _LLFA;_LLF9: goto _LLFB;_LLFA: if((int)
_tmp11A != 2)goto _LLFC;_LLFB: s=Cyc_Absynpp_rgn2doc(t);goto _LLCD;_LLFC: if(_tmp11A
<= (void*)4)goto _LLFE;if(*((int*)_tmp11A)!= 21)goto _LLFE;_tmp13A=(void*)((struct
Cyc_Absyn_RgnsEff_struct*)_tmp11A)->f1;_LLFD:{const char*_tmp640;const char*
_tmp63F;struct Cyc_PP_Doc*_tmp63E[3];s=((_tmp63E[2]=Cyc_PP_text(((_tmp640=")",
_tag_dyneither(_tmp640,sizeof(char),2)))),((_tmp63E[1]=Cyc_Absynpp_typ2doc(
_tmp13A),((_tmp63E[0]=Cyc_PP_text(((_tmp63F="regions(",_tag_dyneither(_tmp63F,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp63E,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLFE: if(_tmp11A <= (void*)4)goto _LL100;if(*((int*)_tmp11A)!= 19)goto
_LL100;_LLFF: goto _LL101;_LL100: if(_tmp11A <= (void*)4)goto _LLCD;if(*((int*)
_tmp11A)!= 20)goto _LLCD;_LL101: s=Cyc_Absynpp_eff2doc(t);goto _LLCD;_LLCD:;}return
s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo);struct Cyc_PP_Doc*
Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _dyneither_ptr*)vo->v));}struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct
_tuple5*cmp);struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple5*cmp){struct
_tuple5 _tmp19E;void*_tmp19F;void*_tmp1A0;struct _tuple5*_tmp19D=cmp;_tmp19E=*
_tmp19D;_tmp19F=_tmp19E.f1;_tmp1A0=_tmp19E.f2;{const char*_tmp643;struct Cyc_PP_Doc*
_tmp642[3];return(_tmp642[2]=Cyc_Absynpp_rgn2doc(_tmp1A0),((_tmp642[1]=Cyc_PP_text(((
_tmp643=" > ",_tag_dyneither(_tmp643,sizeof(char),4)))),((_tmp642[0]=Cyc_Absynpp_rgn2doc(
_tmp19F),Cyc_PP_cat(_tag_dyneither(_tmp642,sizeof(struct Cyc_PP_Doc*),3)))))));}}
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po);struct Cyc_PP_Doc*
Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){const char*_tmp646;const char*
_tmp645;const char*_tmp644;return Cyc_PP_group(((_tmp644="",_tag_dyneither(_tmp644,
sizeof(char),1))),((_tmp645="",_tag_dyneither(_tmp645,sizeof(char),1))),((
_tmp646=",",_tag_dyneither(_tmp646,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,
po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple1*t);struct Cyc_PP_Doc*
Cyc_Absynpp_funarg2doc(struct _tuple1*t){struct Cyc_Core_Opt*_tmp647;struct Cyc_Core_Opt*
dopt=(*t).f1 == 0?0:((_tmp647=_cycalloc(sizeof(*_tmp647)),((_tmp647->v=Cyc_PP_text(*((
struct _dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null((*t).f1))->v)),_tmp647))));
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*
_tmp1A7=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_PP_Doc*
eff_doc;if(c_varargs){const char*_tmp64A;struct Cyc_List_List*_tmp649;_tmp1A7=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1A7,((_tmp649=_cycalloc(sizeof(*_tmp649)),((_tmp649->hd=Cyc_PP_text(((
_tmp64A="...",_tag_dyneither(_tmp64A,sizeof(char),4)))),((_tmp649->tl=0,_tmp649)))))));}
else{if(cyc_varargs != 0){struct _tuple1*_tmp653;const char*_tmp652;const char*
_tmp651;const char*_tmp650;struct Cyc_PP_Doc*_tmp64F[3];struct Cyc_PP_Doc*_tmp1AA=(
_tmp64F[2]=Cyc_Absynpp_funarg2doc(((_tmp653=_cycalloc(sizeof(*_tmp653)),((
_tmp653->f1=cyc_varargs->name,((_tmp653->f2=cyc_varargs->tq,((_tmp653->f3=(void*)
cyc_varargs->type,_tmp653))))))))),((_tmp64F[1]=cyc_varargs->inject?Cyc_PP_text(((
_tmp651=" inject ",_tag_dyneither(_tmp651,sizeof(char),9)))): Cyc_PP_text(((
_tmp652=" ",_tag_dyneither(_tmp652,sizeof(char),2)))),((_tmp64F[0]=Cyc_PP_text(((
_tmp650="...",_tag_dyneither(_tmp650,sizeof(char),4)))),Cyc_PP_cat(
_tag_dyneither(_tmp64F,sizeof(struct Cyc_PP_Doc*),3)))))));struct Cyc_List_List*
_tmp654;_tmp1A7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1A7,((_tmp654=_cycalloc(sizeof(*_tmp654)),((_tmp654->hd=
_tmp1AA,((_tmp654->tl=0,_tmp654)))))));}}{const char*_tmp657;const char*_tmp656;
const char*_tmp655;struct Cyc_PP_Doc*_tmp1B1=Cyc_PP_group(((_tmp655="",
_tag_dyneither(_tmp655,sizeof(char),1))),((_tmp656="",_tag_dyneither(_tmp656,
sizeof(char),1))),((_tmp657=",",_tag_dyneither(_tmp657,sizeof(char),2))),_tmp1A7);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){const char*_tmp65A;struct Cyc_PP_Doc*
_tmp659[3];_tmp1B1=((_tmp659[2]=Cyc_Absynpp_eff2doc((void*)effopt->v),((_tmp659[
1]=Cyc_PP_text(((_tmp65A=";",_tag_dyneither(_tmp65A,sizeof(char),2)))),((_tmp659[
0]=_tmp1B1,Cyc_PP_cat(_tag_dyneither(_tmp659,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){const char*_tmp65D;struct Cyc_PP_Doc*_tmp65C[3];_tmp1B1=((_tmp65C[2]=
Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp65C[1]=Cyc_PP_text(((_tmp65D=":",
_tag_dyneither(_tmp65D,sizeof(char),2)))),((_tmp65C[0]=_tmp1B1,Cyc_PP_cat(
_tag_dyneither(_tmp65C,sizeof(struct Cyc_PP_Doc*),3))))))));}{const char*_tmp662;
const char*_tmp661;struct Cyc_PP_Doc*_tmp660[3];return(_tmp660[2]=Cyc_PP_text(((
_tmp662=")",_tag_dyneither(_tmp662,sizeof(char),2)))),((_tmp660[1]=_tmp1B1,((
_tmp660[0]=Cyc_PP_text(((_tmp661="(",_tag_dyneither(_tmp661,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp660,sizeof(struct Cyc_PP_Doc*),3)))))));}}}struct
_tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple6*arg);struct _tuple1*Cyc_Absynpp_arg_mk_opt(
struct _tuple6*arg){struct Cyc_Core_Opt*_tmp665;struct _tuple1*_tmp664;return(
_tmp664=_cycalloc(sizeof(*_tmp664)),((_tmp664->f1=((_tmp665=_cycalloc(sizeof(*
_tmp665)),((_tmp665->v=(*arg).f1,_tmp665)))),((_tmp664->f2=(*arg).f2,((_tmp664->f3=(*
arg).f3,_tmp664)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct
_dyneither_ptr*v);struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){
return Cyc_PP_text(*v);}struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*
q);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){struct Cyc_List_List*
_tmp1BE=0;int match;{union Cyc_Absyn_Nmspace_union _tmp1BF=(*q).f1;struct Cyc_List_List*
_tmp1C0;struct Cyc_List_List*_tmp1C1;_LL130: if((_tmp1BF.Loc_n).tag != 0)goto _LL132;
_LL131: _tmp1C0=0;goto _LL133;_LL132: if((_tmp1BF.Rel_n).tag != 1)goto _LL134;_tmp1C0=(
_tmp1BF.Rel_n).f1;_LL133: match=0;_tmp1BE=_tmp1C0;goto _LL12F;_LL134: if((_tmp1BF.Abs_n).tag
!= 2)goto _LL12F;_tmp1C1=(_tmp1BF.Abs_n).f1;_LL135: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1C1,Cyc_Absynpp_curr_namespace);{
struct Cyc_List_List*_tmp666;_tmp1BE=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(
_tmp666=_cycalloc(sizeof(*_tmp666)),((_tmp666->hd=Cyc_Absynpp_cyc_stringptr,((
_tmp666->tl=_tmp1C1,_tmp666))))): _tmp1C1;}goto _LL12F;_LL12F:;}if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp668;struct Cyc_List_List*_tmp667;return(struct _dyneither_ptr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1BE,((_tmp667=_cycalloc(sizeof(*_tmp667)),((_tmp667->hd=(*q).f2,((_tmp667->tl=
0,_tmp667))))))),((_tmp668="_",_tag_dyneither(_tmp668,sizeof(char),2))));}else{
if(match)return*(*q).f2;else{const char*_tmp66A;struct Cyc_List_List*_tmp669;
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1BE,((_tmp669=_cycalloc(sizeof(*
_tmp669)),((_tmp669->hd=(*q).f2,((_tmp669->tl=0,_tmp669))))))),((_tmp66A="::",
_tag_dyneither(_tmp66A,sizeof(char),3))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){return
Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp1C7=Cyc_Absynpp_qvar2string(q);if(Cyc_PP_tex_output){
const char*_tmp66C;const char*_tmp66B;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp66B="\\textbf{",
_tag_dyneither(_tmp66B,sizeof(char),9))),(struct _dyneither_ptr)_tmp1C7),((
_tmp66C="}",_tag_dyneither(_tmp66C,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp1C7));}else{return Cyc_PP_text(_tmp1C7);}}struct _dyneither_ptr
Cyc_Absynpp_typedef_name2string(struct _tuple0*v);struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(
struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace_union _tmp1CA=(*v).f1;
struct Cyc_List_List*_tmp1CB;struct Cyc_List_List*_tmp1CC;_LL137: if((_tmp1CA.Loc_n).tag
!= 0)goto _LL139;_LL138: goto _LL13A;_LL139: if((_tmp1CA.Rel_n).tag != 1)goto _LL13B;
_tmp1CB=(_tmp1CA.Rel_n).f1;if(_tmp1CB != 0)goto _LL13B;_LL13A: return*(*v).f2;
_LL13B: if((_tmp1CA.Abs_n).tag != 2)goto _LL13D;_tmp1CC=(_tmp1CA.Abs_n).f1;_LL13C:
if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp1CC,Cyc_Absynpp_curr_namespace)
== 0)return*(*v).f2;else{goto _LL13E;}_LL13D:;_LL13E: {const char*_tmp66D;return(
struct _dyneither_ptr)Cyc_strconcat(((_tmp66D="/* bad namespace : */ ",
_tag_dyneither(_tmp66D,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
v));}_LL136:;}else{return*(*v).f2;}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*
v){struct _dyneither_ptr _tmp1CE=Cyc_Absynpp_typedef_name2string(v);if(Cyc_PP_tex_output){
const char*_tmp66F;const char*_tmp66E;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp66E="\\textbf{",
_tag_dyneither(_tmp66E,sizeof(char),9))),(struct _dyneither_ptr)_tmp1CE),((
_tmp66F="}",_tag_dyneither(_tmp66F,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp1CE));}else{return Cyc_PP_text(_tmp1CE);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e);
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){void*_tmp1D1=(void*)e->r;void*
_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D3;struct Cyc_Absyn_Exp*_tmp1D4;_LL140: if(*((int*)
_tmp1D1)!= 0)goto _LL142;_LL141: goto _LL143;_LL142: if(*((int*)_tmp1D1)!= 1)goto
_LL144;_LL143: goto _LL145;_LL144: if(*((int*)_tmp1D1)!= 2)goto _LL146;_LL145: return
10000;_LL146: if(*((int*)_tmp1D1)!= 3)goto _LL148;_tmp1D2=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D1)->f1;_LL147: {void*_tmp1D5=_tmp1D2;_LL191: if((int)_tmp1D5 != 0)goto _LL193;
_LL192: return 100;_LL193: if((int)_tmp1D5 != 1)goto _LL195;_LL194: return 110;_LL195:
if((int)_tmp1D5 != 2)goto _LL197;_LL196: return 100;_LL197: if((int)_tmp1D5 != 3)goto
_LL199;_LL198: goto _LL19A;_LL199: if((int)_tmp1D5 != 4)goto _LL19B;_LL19A: return 110;
_LL19B: if((int)_tmp1D5 != 5)goto _LL19D;_LL19C: goto _LL19E;_LL19D: if((int)_tmp1D5 != 
6)goto _LL19F;_LL19E: return 70;_LL19F: if((int)_tmp1D5 != 7)goto _LL1A1;_LL1A0: goto
_LL1A2;_LL1A1: if((int)_tmp1D5 != 8)goto _LL1A3;_LL1A2: goto _LL1A4;_LL1A3: if((int)
_tmp1D5 != 9)goto _LL1A5;_LL1A4: goto _LL1A6;_LL1A5: if((int)_tmp1D5 != 10)goto _LL1A7;
_LL1A6: return 80;_LL1A7: if((int)_tmp1D5 != 11)goto _LL1A9;_LL1A8: goto _LL1AA;_LL1A9:
if((int)_tmp1D5 != 12)goto _LL1AB;_LL1AA: return 130;_LL1AB: if((int)_tmp1D5 != 13)
goto _LL1AD;_LL1AC: return 60;_LL1AD: if((int)_tmp1D5 != 14)goto _LL1AF;_LL1AE: return
40;_LL1AF: if((int)_tmp1D5 != 15)goto _LL1B1;_LL1B0: return 50;_LL1B1: if((int)_tmp1D5
!= 16)goto _LL1B3;_LL1B2: return 90;_LL1B3: if((int)_tmp1D5 != 17)goto _LL1B5;_LL1B4:
return 80;_LL1B5: if((int)_tmp1D5 != 18)goto _LL1B7;_LL1B6: return 80;_LL1B7: if((int)
_tmp1D5 != 19)goto _LL190;_LL1B8: return 140;_LL190:;}_LL148: if(*((int*)_tmp1D1)!= 4)
goto _LL14A;_LL149: return 20;_LL14A: if(*((int*)_tmp1D1)!= 5)goto _LL14C;_LL14B:
return 130;_LL14C: if(*((int*)_tmp1D1)!= 6)goto _LL14E;_LL14D: return 30;_LL14E: if(*((
int*)_tmp1D1)!= 7)goto _LL150;_LL14F: return 35;_LL150: if(*((int*)_tmp1D1)!= 8)goto
_LL152;_LL151: return 30;_LL152: if(*((int*)_tmp1D1)!= 9)goto _LL154;_LL153: return 10;
_LL154: if(*((int*)_tmp1D1)!= 10)goto _LL156;_LL155: goto _LL157;_LL156: if(*((int*)
_tmp1D1)!= 11)goto _LL158;_LL157: return 140;_LL158: if(*((int*)_tmp1D1)!= 12)goto
_LL15A;_LL159: return 130;_LL15A: if(*((int*)_tmp1D1)!= 13)goto _LL15C;_tmp1D3=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D1)->f1;_LL15B: return Cyc_Absynpp_exp_prec(
_tmp1D3);_LL15C: if(*((int*)_tmp1D1)!= 14)goto _LL15E;_tmp1D4=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1D1)->f1;_LL15D: return Cyc_Absynpp_exp_prec(_tmp1D4);_LL15E: if(*((int*)
_tmp1D1)!= 15)goto _LL160;_LL15F: return 120;_LL160: if(*((int*)_tmp1D1)!= 17)goto
_LL162;_LL161: return 15;_LL162: if(*((int*)_tmp1D1)!= 16)goto _LL164;_LL163: goto
_LL165;_LL164: if(*((int*)_tmp1D1)!= 18)goto _LL166;_LL165: goto _LL167;_LL166: if(*((
int*)_tmp1D1)!= 19)goto _LL168;_LL167: goto _LL169;_LL168: if(*((int*)_tmp1D1)!= 39)
goto _LL16A;_LL169: goto _LL16B;_LL16A: if(*((int*)_tmp1D1)!= 20)goto _LL16C;_LL16B:
goto _LL16D;_LL16C: if(*((int*)_tmp1D1)!= 21)goto _LL16E;_LL16D: goto _LL16F;_LL16E:
if(*((int*)_tmp1D1)!= 22)goto _LL170;_LL16F: return 130;_LL170: if(*((int*)_tmp1D1)
!= 23)goto _LL172;_LL171: goto _LL173;_LL172: if(*((int*)_tmp1D1)!= 24)goto _LL174;
_LL173: goto _LL175;_LL174: if(*((int*)_tmp1D1)!= 25)goto _LL176;_LL175: return 140;
_LL176: if(*((int*)_tmp1D1)!= 26)goto _LL178;_LL177: return 150;_LL178: if(*((int*)
_tmp1D1)!= 27)goto _LL17A;_LL179: goto _LL17B;_LL17A: if(*((int*)_tmp1D1)!= 28)goto
_LL17C;_LL17B: goto _LL17D;_LL17C: if(*((int*)_tmp1D1)!= 29)goto _LL17E;_LL17D: goto
_LL17F;_LL17E: if(*((int*)_tmp1D1)!= 30)goto _LL180;_LL17F: goto _LL181;_LL180: if(*((
int*)_tmp1D1)!= 31)goto _LL182;_LL181: goto _LL183;_LL182: if(*((int*)_tmp1D1)!= 32)
goto _LL184;_LL183: goto _LL185;_LL184: if(*((int*)_tmp1D1)!= 33)goto _LL186;_LL185:
goto _LL187;_LL186: if(*((int*)_tmp1D1)!= 34)goto _LL188;_LL187: goto _LL189;_LL188:
if(*((int*)_tmp1D1)!= 35)goto _LL18A;_LL189: goto _LL18B;_LL18A: if(*((int*)_tmp1D1)
!= 36)goto _LL18C;_LL18B: goto _LL18D;_LL18C: if(*((int*)_tmp1D1)!= 37)goto _LL18E;
_LL18D: return 140;_LL18E: if(*((int*)_tmp1D1)!= 38)goto _LL13F;_LL18F: return 10000;
_LL13F:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){int
myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*s;{void*_tmp1D6=(void*)e->r;
union Cyc_Absyn_Cnst_union _tmp1D7;struct _tuple0*_tmp1D8;struct _tuple0*_tmp1D9;
void*_tmp1DA;struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DC;struct Cyc_Core_Opt*
_tmp1DD;struct Cyc_Absyn_Exp*_tmp1DE;struct Cyc_Absyn_Exp*_tmp1DF;void*_tmp1E0;
struct Cyc_Absyn_Exp*_tmp1E1;struct Cyc_Absyn_Exp*_tmp1E2;struct Cyc_Absyn_Exp*
_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E4;struct Cyc_Absyn_Exp*_tmp1E5;struct Cyc_Absyn_Exp*
_tmp1E6;struct Cyc_Absyn_Exp*_tmp1E7;struct Cyc_Absyn_Exp*_tmp1E8;struct Cyc_Absyn_Exp*
_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EA;struct Cyc_List_List*_tmp1EB;struct Cyc_Absyn_Exp*
_tmp1EC;struct Cyc_List_List*_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EE;struct Cyc_Absyn_Exp*
_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F0;void*_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F2;
struct Cyc_Absyn_Exp*_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F4;struct Cyc_Absyn_Exp*
_tmp1F5;void*_tmp1F6;struct Cyc_Absyn_Exp*_tmp1F7;void*_tmp1F8;void*_tmp1F9;void*
_tmp1FA;struct _dyneither_ptr*_tmp1FB;void*_tmp1FC;void*_tmp1FD;unsigned int
_tmp1FE;struct Cyc_List_List*_tmp1FF;void*_tmp200;struct Cyc_Absyn_Exp*_tmp201;
struct Cyc_Absyn_Exp*_tmp202;struct _dyneither_ptr*_tmp203;struct Cyc_Absyn_Exp*
_tmp204;struct _dyneither_ptr*_tmp205;struct Cyc_Absyn_Exp*_tmp206;struct Cyc_Absyn_Exp*
_tmp207;struct Cyc_List_List*_tmp208;struct _tuple1*_tmp209;struct Cyc_List_List*
_tmp20A;struct Cyc_List_List*_tmp20B;struct Cyc_Absyn_Vardecl*_tmp20C;struct Cyc_Absyn_Exp*
_tmp20D;struct Cyc_Absyn_Exp*_tmp20E;struct _tuple0*_tmp20F;struct Cyc_List_List*
_tmp210;struct Cyc_List_List*_tmp211;struct Cyc_List_List*_tmp212;struct Cyc_List_List*
_tmp213;struct Cyc_Absyn_Datatypefield*_tmp214;struct _tuple0*_tmp215;struct
_tuple0*_tmp216;struct Cyc_Absyn_MallocInfo _tmp217;int _tmp218;struct Cyc_Absyn_Exp*
_tmp219;void**_tmp21A;struct Cyc_Absyn_Exp*_tmp21B;struct Cyc_Absyn_Exp*_tmp21C;
struct Cyc_Absyn_Exp*_tmp21D;struct Cyc_Core_Opt*_tmp21E;struct Cyc_List_List*
_tmp21F;struct Cyc_Absyn_Stmt*_tmp220;_LL1BA: if(*((int*)_tmp1D6)!= 0)goto _LL1BC;
_tmp1D7=((struct Cyc_Absyn_Const_e_struct*)_tmp1D6)->f1;_LL1BB: s=Cyc_Absynpp_cnst2doc(
_tmp1D7);goto _LL1B9;_LL1BC: if(*((int*)_tmp1D6)!= 1)goto _LL1BE;_tmp1D8=((struct
Cyc_Absyn_Var_e_struct*)_tmp1D6)->f1;_LL1BD: _tmp1D9=_tmp1D8;goto _LL1BF;_LL1BE:
if(*((int*)_tmp1D6)!= 2)goto _LL1C0;_tmp1D9=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp1D6)->f1;_LL1BF: s=Cyc_Absynpp_qvar2doc(_tmp1D9);goto _LL1B9;_LL1C0: if(*((int*)
_tmp1D6)!= 3)goto _LL1C2;_tmp1DA=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D6)->f1;_tmp1DB=((struct Cyc_Absyn_Primop_e_struct*)_tmp1D6)->f2;_LL1C1: s=
Cyc_Absynpp_primapp2doc(myprec,_tmp1DA,_tmp1DB);goto _LL1B9;_LL1C2: if(*((int*)
_tmp1D6)!= 4)goto _LL1C4;_tmp1DC=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1D6)->f1;
_tmp1DD=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1D6)->f2;_tmp1DE=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1D6)->f3;_LL1C3:{const char*_tmp674;const char*_tmp673;struct Cyc_PP_Doc*
_tmp672[5];s=((_tmp672[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DE),((_tmp672[3]=
Cyc_PP_text(((_tmp674="= ",_tag_dyneither(_tmp674,sizeof(char),3)))),((_tmp672[2]=
_tmp1DD == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((void*)_tmp1DD->v),((_tmp672[1]=
Cyc_PP_text(((_tmp673=" ",_tag_dyneither(_tmp673,sizeof(char),2)))),((_tmp672[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DC),Cyc_PP_cat(_tag_dyneither(_tmp672,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1B9;_LL1C4: if(*((int*)_tmp1D6)!= 
5)goto _LL1C6;_tmp1DF=((struct Cyc_Absyn_Increment_e_struct*)_tmp1D6)->f1;_tmp1E0=(
void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp1D6)->f2;_LL1C5: {struct Cyc_PP_Doc*
_tmp224=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DF);{void*_tmp225=_tmp1E0;_LL20D:
if((int)_tmp225 != 0)goto _LL20F;_LL20E:{const char*_tmp677;struct Cyc_PP_Doc*
_tmp676[2];s=((_tmp676[1]=_tmp224,((_tmp676[0]=Cyc_PP_text(((_tmp677="++",
_tag_dyneither(_tmp677,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp676,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL20C;_LL20F: if((int)_tmp225 != 2)goto
_LL211;_LL210:{const char*_tmp67A;struct Cyc_PP_Doc*_tmp679[2];s=((_tmp679[1]=
_tmp224,((_tmp679[0]=Cyc_PP_text(((_tmp67A="--",_tag_dyneither(_tmp67A,sizeof(
char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp679,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL20C;_LL211: if((int)_tmp225 != 1)goto _LL213;_LL212:{const char*_tmp67D;
struct Cyc_PP_Doc*_tmp67C[2];s=((_tmp67C[1]=Cyc_PP_text(((_tmp67D="++",
_tag_dyneither(_tmp67D,sizeof(char),3)))),((_tmp67C[0]=_tmp224,Cyc_PP_cat(
_tag_dyneither(_tmp67C,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL20C;_LL213: if((
int)_tmp225 != 3)goto _LL20C;_LL214:{const char*_tmp680;struct Cyc_PP_Doc*_tmp67F[2];
s=((_tmp67F[1]=Cyc_PP_text(((_tmp680="--",_tag_dyneither(_tmp680,sizeof(char),3)))),((
_tmp67F[0]=_tmp224,Cyc_PP_cat(_tag_dyneither(_tmp67F,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL20C;_LL20C:;}goto _LL1B9;}_LL1C6: if(*((int*)_tmp1D6)!= 6)goto _LL1C8;
_tmp1E1=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1D6)->f1;_tmp1E2=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1D6)->f2;_tmp1E3=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1D6)->f3;_LL1C7:{
const char*_tmp685;const char*_tmp684;struct Cyc_PP_Doc*_tmp683[5];s=((_tmp683[4]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E3),((_tmp683[3]=Cyc_PP_text(((_tmp685=" : ",
_tag_dyneither(_tmp685,sizeof(char),4)))),((_tmp683[2]=Cyc_Absynpp_exp2doc_prec(
0,_tmp1E2),((_tmp683[1]=Cyc_PP_text(((_tmp684=" ? ",_tag_dyneither(_tmp684,
sizeof(char),4)))),((_tmp683[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E1),Cyc_PP_cat(
_tag_dyneither(_tmp683,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1B9;
_LL1C8: if(*((int*)_tmp1D6)!= 7)goto _LL1CA;_tmp1E4=((struct Cyc_Absyn_And_e_struct*)
_tmp1D6)->f1;_tmp1E5=((struct Cyc_Absyn_And_e_struct*)_tmp1D6)->f2;_LL1C9:{const
char*_tmp688;struct Cyc_PP_Doc*_tmp687[3];s=((_tmp687[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E5),((_tmp687[1]=Cyc_PP_text(((_tmp688=" && ",_tag_dyneither(_tmp688,
sizeof(char),5)))),((_tmp687[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E4),Cyc_PP_cat(
_tag_dyneither(_tmp687,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1CA:
if(*((int*)_tmp1D6)!= 8)goto _LL1CC;_tmp1E6=((struct Cyc_Absyn_Or_e_struct*)
_tmp1D6)->f1;_tmp1E7=((struct Cyc_Absyn_Or_e_struct*)_tmp1D6)->f2;_LL1CB:{const
char*_tmp68B;struct Cyc_PP_Doc*_tmp68A[3];s=((_tmp68A[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E7),((_tmp68A[1]=Cyc_PP_text(((_tmp68B=" || ",_tag_dyneither(_tmp68B,
sizeof(char),5)))),((_tmp68A[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E6),Cyc_PP_cat(
_tag_dyneither(_tmp68A,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1CC:
if(*((int*)_tmp1D6)!= 9)goto _LL1CE;_tmp1E8=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1D6)->f1;_tmp1E9=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1D6)->f2;_LL1CD:{
const char*_tmp692;const char*_tmp691;const char*_tmp690;struct Cyc_PP_Doc*_tmp68F[5];
s=((_tmp68F[4]=Cyc_PP_text(((_tmp692=")",_tag_dyneither(_tmp692,sizeof(char),2)))),((
_tmp68F[3]=Cyc_Absynpp_exp2doc(_tmp1E9),((_tmp68F[2]=Cyc_PP_text(((_tmp691=", ",
_tag_dyneither(_tmp691,sizeof(char),3)))),((_tmp68F[1]=Cyc_Absynpp_exp2doc(
_tmp1E8),((_tmp68F[0]=Cyc_PP_text(((_tmp690="(",_tag_dyneither(_tmp690,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp68F,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1B9;_LL1CE: if(*((int*)_tmp1D6)!= 10)goto _LL1D0;_tmp1EA=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp1D6)->f1;_tmp1EB=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1D6)->f2;_LL1CF:{
const char*_tmp697;const char*_tmp696;struct Cyc_PP_Doc*_tmp695[4];s=((_tmp695[3]=
Cyc_PP_text(((_tmp697=")",_tag_dyneither(_tmp697,sizeof(char),2)))),((_tmp695[2]=
Cyc_Absynpp_exps2doc_prec(20,_tmp1EB),((_tmp695[1]=Cyc_PP_text(((_tmp696="(",
_tag_dyneither(_tmp696,sizeof(char),2)))),((_tmp695[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EA),Cyc_PP_cat(_tag_dyneither(_tmp695,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1B9;_LL1D0: if(*((int*)_tmp1D6)!= 11)goto _LL1D2;_tmp1EC=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1D6)->f1;_tmp1ED=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1D6)->f2;_LL1D1:{
const char*_tmp69C;const char*_tmp69B;struct Cyc_PP_Doc*_tmp69A[4];s=((_tmp69A[3]=
Cyc_PP_text(((_tmp69C=")",_tag_dyneither(_tmp69C,sizeof(char),2)))),((_tmp69A[2]=
Cyc_Absynpp_exps2doc_prec(20,_tmp1ED),((_tmp69A[1]=Cyc_PP_text(((_tmp69B="(",
_tag_dyneither(_tmp69B,sizeof(char),2)))),((_tmp69A[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EC),Cyc_PP_cat(_tag_dyneither(_tmp69A,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1B9;_LL1D2: if(*((int*)_tmp1D6)!= 12)goto _LL1D4;_tmp1EE=((struct Cyc_Absyn_Throw_e_struct*)
_tmp1D6)->f1;_LL1D3:{const char*_tmp69F;struct Cyc_PP_Doc*_tmp69E[2];s=((_tmp69E[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1EE),((_tmp69E[0]=Cyc_PP_text(((_tmp69F="throw ",
_tag_dyneither(_tmp69F,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp69E,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1B9;_LL1D4: if(*((int*)_tmp1D6)!= 13)
goto _LL1D6;_tmp1EF=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D6)->f1;_LL1D5:
s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1EF);goto _LL1B9;_LL1D6: if(*((int*)_tmp1D6)
!= 14)goto _LL1D8;_tmp1F0=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1D6)->f1;
_LL1D7: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1F0);goto _LL1B9;_LL1D8: if(*((int*)
_tmp1D6)!= 15)goto _LL1DA;_tmp1F1=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1D6)->f1;
_tmp1F2=((struct Cyc_Absyn_Cast_e_struct*)_tmp1D6)->f2;_LL1D9:{const char*_tmp6A4;
const char*_tmp6A3;struct Cyc_PP_Doc*_tmp6A2[4];s=((_tmp6A2[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F2),((_tmp6A2[2]=Cyc_PP_text(((_tmp6A4=")",_tag_dyneither(_tmp6A4,
sizeof(char),2)))),((_tmp6A2[1]=Cyc_Absynpp_typ2doc(_tmp1F1),((_tmp6A2[0]=Cyc_PP_text(((
_tmp6A3="(",_tag_dyneither(_tmp6A3,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp6A2,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1B9;_LL1DA: if(*((int*)
_tmp1D6)!= 16)goto _LL1DC;_tmp1F3=((struct Cyc_Absyn_Address_e_struct*)_tmp1D6)->f1;
_LL1DB:{const char*_tmp6A7;struct Cyc_PP_Doc*_tmp6A6[2];s=((_tmp6A6[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F3),((_tmp6A6[0]=Cyc_PP_text(((_tmp6A7="&",_tag_dyneither(_tmp6A7,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6A6,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1B9;_LL1DC: if(*((int*)_tmp1D6)!= 17)goto _LL1DE;_tmp1F4=((struct Cyc_Absyn_New_e_struct*)
_tmp1D6)->f1;_tmp1F5=((struct Cyc_Absyn_New_e_struct*)_tmp1D6)->f2;_LL1DD: if(
_tmp1F4 == 0){const char*_tmp6AA;struct Cyc_PP_Doc*_tmp6A9[2];s=((_tmp6A9[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F5),((_tmp6A9[0]=Cyc_PP_text(((_tmp6AA="new ",_tag_dyneither(_tmp6AA,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp6A9,sizeof(struct Cyc_PP_Doc*),2))))));}
else{const char*_tmp6AF;const char*_tmp6AE;struct Cyc_PP_Doc*_tmp6AD[4];s=((_tmp6AD[
3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F5),((_tmp6AD[2]=Cyc_PP_text(((_tmp6AF=") ",
_tag_dyneither(_tmp6AF,sizeof(char),3)))),((_tmp6AD[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp1F4),((_tmp6AD[0]=Cyc_PP_text(((_tmp6AE="rnew(",
_tag_dyneither(_tmp6AE,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp6AD,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1B9;_LL1DE: if(*((int*)_tmp1D6)!= 
18)goto _LL1E0;_tmp1F6=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp1D6)->f1;
_LL1DF:{const char*_tmp6B4;const char*_tmp6B3;struct Cyc_PP_Doc*_tmp6B2[3];s=((
_tmp6B2[2]=Cyc_PP_text(((_tmp6B4=")",_tag_dyneither(_tmp6B4,sizeof(char),2)))),((
_tmp6B2[1]=Cyc_Absynpp_typ2doc(_tmp1F6),((_tmp6B2[0]=Cyc_PP_text(((_tmp6B3="sizeof(",
_tag_dyneither(_tmp6B3,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp6B2,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1E0: if(*((int*)_tmp1D6)!= 19)
goto _LL1E2;_tmp1F7=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp1D6)->f1;_LL1E1:{
const char*_tmp6B9;const char*_tmp6B8;struct Cyc_PP_Doc*_tmp6B7[3];s=((_tmp6B7[2]=
Cyc_PP_text(((_tmp6B9=")",_tag_dyneither(_tmp6B9,sizeof(char),2)))),((_tmp6B7[1]=
Cyc_Absynpp_exp2doc(_tmp1F7),((_tmp6B7[0]=Cyc_PP_text(((_tmp6B8="sizeof(",
_tag_dyneither(_tmp6B8,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp6B7,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1E2: if(*((int*)_tmp1D6)!= 39)
goto _LL1E4;_tmp1F8=(void*)((struct Cyc_Absyn_Valueof_e_struct*)_tmp1D6)->f1;
_LL1E3:{const char*_tmp6BE;const char*_tmp6BD;struct Cyc_PP_Doc*_tmp6BC[3];s=((
_tmp6BC[2]=Cyc_PP_text(((_tmp6BE=")",_tag_dyneither(_tmp6BE,sizeof(char),2)))),((
_tmp6BC[1]=Cyc_Absynpp_typ2doc(_tmp1F8),((_tmp6BC[0]=Cyc_PP_text(((_tmp6BD="valueof(",
_tag_dyneither(_tmp6BD,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp6BC,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1E4: if(*((int*)_tmp1D6)!= 20)
goto _LL1E6;_tmp1F9=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D6)->f1;
_tmp1FA=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D6)->f2;if(*((int*)
_tmp1FA)!= 0)goto _LL1E6;_tmp1FB=((struct Cyc_Absyn_StructField_struct*)_tmp1FA)->f1;
_LL1E5:{const char*_tmp6C5;const char*_tmp6C4;const char*_tmp6C3;struct Cyc_PP_Doc*
_tmp6C2[5];s=((_tmp6C2[4]=Cyc_PP_text(((_tmp6C5=")",_tag_dyneither(_tmp6C5,
sizeof(char),2)))),((_tmp6C2[3]=Cyc_PP_textptr(_tmp1FB),((_tmp6C2[2]=Cyc_PP_text(((
_tmp6C4=",",_tag_dyneither(_tmp6C4,sizeof(char),2)))),((_tmp6C2[1]=Cyc_Absynpp_typ2doc(
_tmp1F9),((_tmp6C2[0]=Cyc_PP_text(((_tmp6C3="offsetof(",_tag_dyneither(_tmp6C3,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp6C2,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL1B9;_LL1E6: if(*((int*)_tmp1D6)!= 20)goto _LL1E8;_tmp1FC=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D6)->f1;_tmp1FD=(void*)((struct
Cyc_Absyn_Offsetof_e_struct*)_tmp1D6)->f2;if(*((int*)_tmp1FD)!= 1)goto _LL1E8;
_tmp1FE=((struct Cyc_Absyn_TupleIndex_struct*)_tmp1FD)->f1;_LL1E7:{const char*
_tmp6D3;struct Cyc_Int_pa_struct _tmp6D2;void*_tmp6D1[1];const char*_tmp6D0;const
char*_tmp6CF;const char*_tmp6CE;struct Cyc_PP_Doc*_tmp6CD[5];s=((_tmp6CD[4]=Cyc_PP_text(((
_tmp6D3=")",_tag_dyneither(_tmp6D3,sizeof(char),2)))),((_tmp6CD[3]=Cyc_PP_text((
struct _dyneither_ptr)((_tmp6D2.tag=1,((_tmp6D2.f1=(unsigned long)((int)_tmp1FE),((
_tmp6D1[0]=& _tmp6D2,Cyc_aprintf(((_tmp6D0="%d",_tag_dyneither(_tmp6D0,sizeof(
char),3))),_tag_dyneither(_tmp6D1,sizeof(void*),1))))))))),((_tmp6CD[2]=Cyc_PP_text(((
_tmp6CF=",",_tag_dyneither(_tmp6CF,sizeof(char),2)))),((_tmp6CD[1]=Cyc_Absynpp_typ2doc(
_tmp1FC),((_tmp6CD[0]=Cyc_PP_text(((_tmp6CE="offsetof(",_tag_dyneither(_tmp6CE,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp6CD,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL1B9;_LL1E8: if(*((int*)_tmp1D6)!= 21)goto _LL1EA;_tmp1FF=((
struct Cyc_Absyn_Gentyp_e_struct*)_tmp1D6)->f1;_tmp200=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)
_tmp1D6)->f2;_LL1E9:{const char*_tmp6D8;const char*_tmp6D7;struct Cyc_PP_Doc*
_tmp6D6[4];s=((_tmp6D6[3]=Cyc_PP_text(((_tmp6D8=")",_tag_dyneither(_tmp6D8,
sizeof(char),2)))),((_tmp6D6[2]=Cyc_Absynpp_typ2doc(_tmp200),((_tmp6D6[1]=Cyc_Absynpp_tvars2doc(
_tmp1FF),((_tmp6D6[0]=Cyc_PP_text(((_tmp6D7="__gen(",_tag_dyneither(_tmp6D7,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp6D6,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1B9;_LL1EA: if(*((int*)_tmp1D6)!= 22)goto _LL1EC;_tmp201=((struct Cyc_Absyn_Deref_e_struct*)
_tmp1D6)->f1;_LL1EB:{const char*_tmp6DB;struct Cyc_PP_Doc*_tmp6DA[2];s=((_tmp6DA[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp201),((_tmp6DA[0]=Cyc_PP_text(((_tmp6DB="*",
_tag_dyneither(_tmp6DB,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6DA,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1B9;_LL1EC: if(*((int*)_tmp1D6)!= 23)
goto _LL1EE;_tmp202=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D6)->f1;_tmp203=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D6)->f2;_LL1ED:{const char*_tmp6DE;
struct Cyc_PP_Doc*_tmp6DD[3];s=((_tmp6DD[2]=Cyc_PP_textptr(_tmp203),((_tmp6DD[1]=
Cyc_PP_text(((_tmp6DE=".",_tag_dyneither(_tmp6DE,sizeof(char),2)))),((_tmp6DD[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp202),Cyc_PP_cat(_tag_dyneither(_tmp6DD,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1EE: if(*((int*)_tmp1D6)!= 24)
goto _LL1F0;_tmp204=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1D6)->f1;_tmp205=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1D6)->f2;_LL1EF:{const char*_tmp6E1;
struct Cyc_PP_Doc*_tmp6E0[3];s=((_tmp6E0[2]=Cyc_PP_textptr(_tmp205),((_tmp6E0[1]=
Cyc_PP_text(((_tmp6E1="->",_tag_dyneither(_tmp6E1,sizeof(char),3)))),((_tmp6E0[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp204),Cyc_PP_cat(_tag_dyneither(_tmp6E0,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1F0: if(*((int*)_tmp1D6)!= 25)
goto _LL1F2;_tmp206=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1D6)->f1;_tmp207=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp1D6)->f2;_LL1F1:{const char*_tmp6E6;const
char*_tmp6E5;struct Cyc_PP_Doc*_tmp6E4[4];s=((_tmp6E4[3]=Cyc_PP_text(((_tmp6E6="]",
_tag_dyneither(_tmp6E6,sizeof(char),2)))),((_tmp6E4[2]=Cyc_Absynpp_exp2doc(
_tmp207),((_tmp6E4[1]=Cyc_PP_text(((_tmp6E5="[",_tag_dyneither(_tmp6E5,sizeof(
char),2)))),((_tmp6E4[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp206),Cyc_PP_cat(
_tag_dyneither(_tmp6E4,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1B9;_LL1F2:
if(*((int*)_tmp1D6)!= 26)goto _LL1F4;_tmp208=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp1D6)->f1;_LL1F3:{const char*_tmp6EB;const char*_tmp6EA;struct Cyc_PP_Doc*
_tmp6E9[4];s=((_tmp6E9[3]=Cyc_PP_text(((_tmp6EB=")",_tag_dyneither(_tmp6EB,
sizeof(char),2)))),((_tmp6E9[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp208),((_tmp6E9[
1]=Cyc_PP_text(((_tmp6EA="(",_tag_dyneither(_tmp6EA,sizeof(char),2)))),((_tmp6E9[
0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp6E9,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LL1B9;_LL1F4: if(*((int*)_tmp1D6)!= 27)goto _LL1F6;_tmp209=((
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp1D6)->f1;_tmp20A=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp1D6)->f2;_LL1F5:{const char*_tmp6F2;const char*_tmp6F1;const char*_tmp6F0;
struct Cyc_PP_Doc*_tmp6EF[4];s=((_tmp6EF[3]=Cyc_Absynpp_group_braces(((_tmp6F2=",",
_tag_dyneither(_tmp6F2,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp20A)),((_tmp6EF[2]=Cyc_PP_text(((_tmp6F1=")",_tag_dyneither(_tmp6F1,sizeof(
char),2)))),((_tmp6EF[1]=Cyc_Absynpp_typ2doc((*_tmp209).f3),((_tmp6EF[0]=Cyc_PP_text(((
_tmp6F0="(",_tag_dyneither(_tmp6F0,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp6EF,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1B9;_LL1F6: if(*((int*)
_tmp1D6)!= 28)goto _LL1F8;_tmp20B=((struct Cyc_Absyn_Array_e_struct*)_tmp1D6)->f1;
_LL1F7:{const char*_tmp6F3;s=Cyc_Absynpp_group_braces(((_tmp6F3=",",
_tag_dyneither(_tmp6F3,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp20B));}goto _LL1B9;_LL1F8: if(*((int*)_tmp1D6)!= 29)goto _LL1FA;_tmp20C=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D6)->f1;_tmp20D=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1D6)->f2;_tmp20E=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D6)->f3;
_LL1F9:{const char*_tmp6FA;const char*_tmp6F9;const char*_tmp6F8;struct Cyc_PP_Doc*
_tmp6F7[8];s=((_tmp6F7[7]=Cyc_Absynpp_rb(),((_tmp6F7[6]=Cyc_Absynpp_exp2doc(
_tmp20E),((_tmp6F7[5]=Cyc_PP_text(((_tmp6FA=" : ",_tag_dyneither(_tmp6FA,sizeof(
char),4)))),((_tmp6F7[4]=Cyc_Absynpp_exp2doc(_tmp20D),((_tmp6F7[3]=Cyc_PP_text(((
_tmp6F9=" < ",_tag_dyneither(_tmp6F9,sizeof(char),4)))),((_tmp6F7[2]=Cyc_PP_text(*(*
_tmp20C->name).f2),((_tmp6F7[1]=Cyc_PP_text(((_tmp6F8="for ",_tag_dyneither(
_tmp6F8,sizeof(char),5)))),((_tmp6F7[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp6F7,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}goto _LL1B9;
_LL1FA: if(*((int*)_tmp1D6)!= 30)goto _LL1FC;_tmp20F=((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp1D6)->f1;_tmp210=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp1D6)->f2;_tmp211=((
struct Cyc_Absyn_Aggregate_e_struct*)_tmp1D6)->f3;_LL1FB: {struct Cyc_List_List*
_tmp277=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp211);{const char*_tmp6FF;
struct Cyc_List_List*_tmp6FE;struct Cyc_PP_Doc*_tmp6FD[2];s=((_tmp6FD[1]=Cyc_Absynpp_group_braces(((
_tmp6FF=",",_tag_dyneither(_tmp6FF,sizeof(char),2))),_tmp210 != 0?(_tmp6FE=
_cycalloc(sizeof(*_tmp6FE)),((_tmp6FE->hd=Cyc_Absynpp_tps2doc(_tmp210),((_tmp6FE->tl=
_tmp277,_tmp6FE))))): _tmp277),((_tmp6FD[0]=Cyc_Absynpp_qvar2doc(_tmp20F),Cyc_PP_cat(
_tag_dyneither(_tmp6FD,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1B9;}_LL1FC: if(*((
int*)_tmp1D6)!= 31)goto _LL1FE;_tmp212=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1D6)->f2;_LL1FD:{const char*_tmp700;s=Cyc_Absynpp_group_braces(((_tmp700=",",
_tag_dyneither(_tmp700,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp212));}goto _LL1B9;_LL1FE: if(*((int*)_tmp1D6)!= 32)goto _LL200;_tmp213=((
struct Cyc_Absyn_Datatype_e_struct*)_tmp1D6)->f1;_tmp214=((struct Cyc_Absyn_Datatype_e_struct*)
_tmp1D6)->f3;_LL1FF: if(_tmp213 == 0)s=Cyc_Absynpp_qvar2doc(_tmp214->name);else{
const char*_tmp707;const char*_tmp706;const char*_tmp705;struct Cyc_PP_Doc*_tmp704[2];
s=((_tmp704[1]=Cyc_PP_egroup(((_tmp707="(",_tag_dyneither(_tmp707,sizeof(char),2))),((
_tmp706=")",_tag_dyneither(_tmp706,sizeof(char),2))),((_tmp705=",",
_tag_dyneither(_tmp705,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,
_tmp213)),((_tmp704[0]=Cyc_Absynpp_qvar2doc(_tmp214->name),Cyc_PP_cat(
_tag_dyneither(_tmp704,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1B9;_LL200: if(*((
int*)_tmp1D6)!= 33)goto _LL202;_tmp215=((struct Cyc_Absyn_Enum_e_struct*)_tmp1D6)->f1;
_LL201: s=Cyc_Absynpp_qvar2doc(_tmp215);goto _LL1B9;_LL202: if(*((int*)_tmp1D6)!= 
34)goto _LL204;_tmp216=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp1D6)->f1;_LL203: s=
Cyc_Absynpp_qvar2doc(_tmp216);goto _LL1B9;_LL204: if(*((int*)_tmp1D6)!= 35)goto
_LL206;_tmp217=((struct Cyc_Absyn_Malloc_e_struct*)_tmp1D6)->f1;_tmp218=_tmp217.is_calloc;
_tmp219=_tmp217.rgn;_tmp21A=_tmp217.elt_type;_tmp21B=_tmp217.num_elts;_LL205: if(
_tmp218){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp21A)),0);if(_tmp219 == 0){const char*_tmp70E;const char*_tmp70D;const char*
_tmp70C;struct Cyc_PP_Doc*_tmp70B[5];s=((_tmp70B[4]=Cyc_PP_text(((_tmp70E=")",
_tag_dyneither(_tmp70E,sizeof(char),2)))),((_tmp70B[3]=Cyc_Absynpp_exp2doc(st),((
_tmp70B[2]=Cyc_PP_text(((_tmp70D=",",_tag_dyneither(_tmp70D,sizeof(char),2)))),((
_tmp70B[1]=Cyc_Absynpp_exp2doc(_tmp21B),((_tmp70B[0]=Cyc_PP_text(((_tmp70C="calloc(",
_tag_dyneither(_tmp70C,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp70B,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{const char*_tmp717;const char*
_tmp716;const char*_tmp715;const char*_tmp714;struct Cyc_PP_Doc*_tmp713[7];s=((
_tmp713[6]=Cyc_PP_text(((_tmp717=")",_tag_dyneither(_tmp717,sizeof(char),2)))),((
_tmp713[5]=Cyc_Absynpp_exp2doc(st),((_tmp713[4]=Cyc_PP_text(((_tmp716=",",
_tag_dyneither(_tmp716,sizeof(char),2)))),((_tmp713[3]=Cyc_Absynpp_exp2doc(
_tmp21B),((_tmp713[2]=Cyc_PP_text(((_tmp715=",",_tag_dyneither(_tmp715,sizeof(
char),2)))),((_tmp713[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp219),((
_tmp713[0]=Cyc_PP_text(((_tmp714="rcalloc(",_tag_dyneither(_tmp714,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp713,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmp21A == 0)new_e=_tmp21B;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp21A,0),_tmp21B,0);}if(_tmp219 == 0){const char*
_tmp71C;const char*_tmp71B;struct Cyc_PP_Doc*_tmp71A[3];s=((_tmp71A[2]=Cyc_PP_text(((
_tmp71C=")",_tag_dyneither(_tmp71C,sizeof(char),2)))),((_tmp71A[1]=Cyc_Absynpp_exp2doc(
new_e),((_tmp71A[0]=Cyc_PP_text(((_tmp71B="malloc(",_tag_dyneither(_tmp71B,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp71A,sizeof(struct Cyc_PP_Doc*),3))))))));}
else{const char*_tmp723;const char*_tmp722;const char*_tmp721;struct Cyc_PP_Doc*
_tmp720[5];s=((_tmp720[4]=Cyc_PP_text(((_tmp723=")",_tag_dyneither(_tmp723,
sizeof(char),2)))),((_tmp720[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp720[2]=Cyc_PP_text(((
_tmp722=",",_tag_dyneither(_tmp722,sizeof(char),2)))),((_tmp720[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp219),((_tmp720[0]=Cyc_PP_text(((_tmp721="rmalloc(",
_tag_dyneither(_tmp721,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp720,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}}goto _LL1B9;_LL206: if(*((int*)_tmp1D6)
!= 36)goto _LL208;_tmp21C=((struct Cyc_Absyn_Swap_e_struct*)_tmp1D6)->f1;_tmp21D=((
struct Cyc_Absyn_Swap_e_struct*)_tmp1D6)->f2;_LL207:{const char*_tmp72A;const char*
_tmp729;const char*_tmp728;struct Cyc_PP_Doc*_tmp727[5];s=((_tmp727[4]=Cyc_PP_text(((
_tmp72A=")",_tag_dyneither(_tmp72A,sizeof(char),2)))),((_tmp727[3]=Cyc_Absynpp_exp2doc(
_tmp21D),((_tmp727[2]=Cyc_PP_text(((_tmp729=",",_tag_dyneither(_tmp729,sizeof(
char),2)))),((_tmp727[1]=Cyc_Absynpp_exp2doc(_tmp21C),((_tmp727[0]=Cyc_PP_text(((
_tmp728="cycswap(",_tag_dyneither(_tmp728,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp727,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1B9;
_LL208: if(*((int*)_tmp1D6)!= 37)goto _LL20A;_tmp21E=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp1D6)->f1;_tmp21F=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1D6)->f2;
_LL209:{const char*_tmp72B;s=Cyc_Absynpp_group_braces(((_tmp72B=",",
_tag_dyneither(_tmp72B,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp21F));}goto _LL1B9;_LL20A: if(*((int*)_tmp1D6)!= 38)goto _LL1B9;_tmp220=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp1D6)->f1;_LL20B:{const char*_tmp730;const
char*_tmp72F;struct Cyc_PP_Doc*_tmp72E[7];s=((_tmp72E[6]=Cyc_PP_text(((_tmp730=")",
_tag_dyneither(_tmp730,sizeof(char),2)))),((_tmp72E[5]=Cyc_Absynpp_rb(),((
_tmp72E[4]=Cyc_PP_blank_doc(),((_tmp72E[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp220)),((_tmp72E[2]=Cyc_PP_blank_doc(),((_tmp72E[1]=Cyc_Absynpp_lb(),((
_tmp72E[0]=Cyc_PP_text(((_tmp72F="(",_tag_dyneither(_tmp72F,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp72E,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
goto _LL1B9;_LL1B9:;}if(inprec >= myprec){const char*_tmp735;const char*_tmp734;
struct Cyc_PP_Doc*_tmp733[3];s=((_tmp733[2]=Cyc_PP_text(((_tmp735=")",
_tag_dyneither(_tmp735,sizeof(char),2)))),((_tmp733[1]=s,((_tmp733[0]=Cyc_PP_text(((
_tmp734="(",_tag_dyneither(_tmp734,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp733,sizeof(struct Cyc_PP_Doc*),3))))))));}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(
void*d);struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){void*_tmp29B=d;
struct Cyc_Absyn_Exp*_tmp29C;struct _dyneither_ptr*_tmp29D;_LL216: if(*((int*)
_tmp29B)!= 0)goto _LL218;_tmp29C=((struct Cyc_Absyn_ArrayElement_struct*)_tmp29B)->f1;
_LL217: {const char*_tmp73A;const char*_tmp739;struct Cyc_PP_Doc*_tmp738[3];return(
_tmp738[2]=Cyc_PP_text(((_tmp73A="]",_tag_dyneither(_tmp73A,sizeof(char),2)))),((
_tmp738[1]=Cyc_Absynpp_exp2doc(_tmp29C),((_tmp738[0]=Cyc_PP_text(((_tmp739=".[",
_tag_dyneither(_tmp739,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp738,
sizeof(struct Cyc_PP_Doc*),3)))))));}_LL218: if(*((int*)_tmp29B)!= 1)goto _LL215;
_tmp29D=((struct Cyc_Absyn_FieldName_struct*)_tmp29B)->f1;_LL219: {const char*
_tmp73D;struct Cyc_PP_Doc*_tmp73C[2];return(_tmp73C[1]=Cyc_PP_textptr(_tmp29D),((
_tmp73C[0]=Cyc_PP_text(((_tmp73D=".",_tag_dyneither(_tmp73D,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp73C,sizeof(struct Cyc_PP_Doc*),2)))));}_LL215:;}
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple9*de);struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(
struct _tuple9*de){if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{const
char*_tmp744;const char*_tmp743;const char*_tmp742;struct Cyc_PP_Doc*_tmp741[2];
return(_tmp741[1]=Cyc_Absynpp_exp2doc((*de).f2),((_tmp741[0]=Cyc_PP_egroup(((
_tmp744="",_tag_dyneither(_tmp744,sizeof(char),1))),((_tmp743="=",_tag_dyneither(
_tmp743,sizeof(char),2))),((_tmp742="=",_tag_dyneither(_tmp742,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp741,sizeof(
struct Cyc_PP_Doc*),2)))));}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es){const char*_tmp747;const char*_tmp746;const char*_tmp745;
return Cyc_PP_group(((_tmp745="",_tag_dyneither(_tmp745,sizeof(char),1))),((
_tmp746="",_tag_dyneither(_tmp746,sizeof(char),1))),((_tmp747=",",_tag_dyneither(
_tmp747,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst_union c);
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst_union c){union Cyc_Absyn_Cnst_union
_tmp2AA=c;void*_tmp2AB;char _tmp2AC;void*_tmp2AD;short _tmp2AE;void*_tmp2AF;int
_tmp2B0;void*_tmp2B1;int _tmp2B2;void*_tmp2B3;int _tmp2B4;void*_tmp2B5;long long
_tmp2B6;struct _dyneither_ptr _tmp2B7;struct _dyneither_ptr _tmp2B8;_LL21B: if((
_tmp2AA.Char_c).tag != 0)goto _LL21D;_tmp2AB=(_tmp2AA.Char_c).f1;_tmp2AC=(_tmp2AA.Char_c).f2;
_LL21C: {const char*_tmp74B;void*_tmp74A[1];struct Cyc_String_pa_struct _tmp749;
return Cyc_PP_text((struct _dyneither_ptr)((_tmp749.tag=0,((_tmp749.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp2AC)),((
_tmp74A[0]=& _tmp749,Cyc_aprintf(((_tmp74B="'%s'",_tag_dyneither(_tmp74B,sizeof(
char),5))),_tag_dyneither(_tmp74A,sizeof(void*),1)))))))));}_LL21D: if((_tmp2AA.Short_c).tag
!= 1)goto _LL21F;_tmp2AD=(_tmp2AA.Short_c).f1;_tmp2AE=(_tmp2AA.Short_c).f2;_LL21E: {
const char*_tmp74F;void*_tmp74E[1];struct Cyc_Int_pa_struct _tmp74D;return Cyc_PP_text((
struct _dyneither_ptr)((_tmp74D.tag=1,((_tmp74D.f1=(unsigned long)((int)_tmp2AE),((
_tmp74E[0]=& _tmp74D,Cyc_aprintf(((_tmp74F="%d",_tag_dyneither(_tmp74F,sizeof(
char),3))),_tag_dyneither(_tmp74E,sizeof(void*),1)))))))));}_LL21F: if((_tmp2AA.Int_c).tag
!= 2)goto _LL221;_tmp2AF=(_tmp2AA.Int_c).f1;if((int)_tmp2AF != 2)goto _LL221;
_tmp2B0=(_tmp2AA.Int_c).f2;_LL220: _tmp2B2=_tmp2B0;goto _LL222;_LL221: if((_tmp2AA.Int_c).tag
!= 2)goto _LL223;_tmp2B1=(_tmp2AA.Int_c).f1;if((int)_tmp2B1 != 0)goto _LL223;
_tmp2B2=(_tmp2AA.Int_c).f2;_LL222: {const char*_tmp753;void*_tmp752[1];struct Cyc_Int_pa_struct
_tmp751;return Cyc_PP_text((struct _dyneither_ptr)((_tmp751.tag=1,((_tmp751.f1=(
unsigned long)_tmp2B2,((_tmp752[0]=& _tmp751,Cyc_aprintf(((_tmp753="%d",
_tag_dyneither(_tmp753,sizeof(char),3))),_tag_dyneither(_tmp752,sizeof(void*),1)))))))));}
_LL223: if((_tmp2AA.Int_c).tag != 2)goto _LL225;_tmp2B3=(_tmp2AA.Int_c).f1;if((int)
_tmp2B3 != 1)goto _LL225;_tmp2B4=(_tmp2AA.Int_c).f2;_LL224: {const char*_tmp757;
void*_tmp756[1];struct Cyc_Int_pa_struct _tmp755;return Cyc_PP_text((struct
_dyneither_ptr)((_tmp755.tag=1,((_tmp755.f1=(unsigned int)_tmp2B4,((_tmp756[0]=&
_tmp755,Cyc_aprintf(((_tmp757="%u",_tag_dyneither(_tmp757,sizeof(char),3))),
_tag_dyneither(_tmp756,sizeof(void*),1)))))))));}_LL225: if((_tmp2AA.LongLong_c).tag
!= 3)goto _LL227;_tmp2B5=(_tmp2AA.LongLong_c).f1;_tmp2B6=(_tmp2AA.LongLong_c).f2;
_LL226: {const char*_tmp758;return Cyc_PP_text(((_tmp758="<<FIX LONG LONG CONSTANT>>",
_tag_dyneither(_tmp758,sizeof(char),27))));}_LL227: if((_tmp2AA.Float_c).tag != 4)
goto _LL229;_tmp2B7=(_tmp2AA.Float_c).f1;_LL228: return Cyc_PP_text(_tmp2B7);_LL229:
if((_tmp2AA.Null_c).tag != 6)goto _LL22B;_LL22A: {const char*_tmp759;return Cyc_PP_text(((
_tmp759="NULL",_tag_dyneither(_tmp759,sizeof(char),5))));}_LL22B: if((_tmp2AA.String_c).tag
!= 5)goto _LL21A;_tmp2B8=(_tmp2AA.String_c).f1;_LL22C: {const char*_tmp75E;const
char*_tmp75D;struct Cyc_PP_Doc*_tmp75C[3];return(_tmp75C[2]=Cyc_PP_text(((_tmp75E="\"",
_tag_dyneither(_tmp75E,sizeof(char),2)))),((_tmp75C[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(
_tmp2B8)),((_tmp75C[0]=Cyc_PP_text(((_tmp75D="\"",_tag_dyneither(_tmp75D,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp75C,sizeof(struct Cyc_PP_Doc*),3)))))));}
_LL21A:;}struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*
es);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*
es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (void*)((void*)19)){if(es
== 0  || es->tl != 0){struct Cyc_Core_Failure_struct _tmp76B;const char*_tmp76A;void*
_tmp769[1];struct Cyc_String_pa_struct _tmp768;struct Cyc_Core_Failure_struct*
_tmp767;(int)_throw((void*)((_tmp767=_cycalloc(sizeof(*_tmp767)),((_tmp767[0]=((
_tmp76B.tag=Cyc_Core_Failure,((_tmp76B.f1=(struct _dyneither_ptr)((_tmp768.tag=0,((
_tmp768.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp769[0]=& _tmp768,Cyc_aprintf(((_tmp76A="Absynpp::primapp2doc Numelts: %s with bad args",
_tag_dyneither(_tmp76A,sizeof(char),47))),_tag_dyneither(_tmp769,sizeof(void*),1)))))))),
_tmp76B)))),_tmp767)))));}{const char*_tmp770;const char*_tmp76F;struct Cyc_PP_Doc*
_tmp76E[3];return(_tmp76E[2]=Cyc_PP_text(((_tmp770=")",_tag_dyneither(_tmp770,
sizeof(char),2)))),((_tmp76E[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((
_tmp76E[0]=Cyc_PP_text(((_tmp76F="numelts(",_tag_dyneither(_tmp76F,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp76E,sizeof(struct Cyc_PP_Doc*),3)))))));}}
else{struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es);if(ds == 0){struct Cyc_Core_Failure_struct _tmp77D;const char*_tmp77C;
void*_tmp77B[1];struct Cyc_String_pa_struct _tmp77A;struct Cyc_Core_Failure_struct*
_tmp779;(int)_throw((void*)((_tmp779=_cycalloc(sizeof(*_tmp779)),((_tmp779[0]=((
_tmp77D.tag=Cyc_Core_Failure,((_tmp77D.f1=(struct _dyneither_ptr)((_tmp77A.tag=0,((
_tmp77A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp77B[0]=& _tmp77A,Cyc_aprintf(((_tmp77C="Absynpp::primapp2doc: %s with no args",
_tag_dyneither(_tmp77C,sizeof(char),38))),_tag_dyneither(_tmp77B,sizeof(void*),1)))))))),
_tmp77D)))),_tmp779)))));}else{if(ds->tl == 0){const char*_tmp780;struct Cyc_PP_Doc*
_tmp77F[3];return(_tmp77F[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp77F[1]=Cyc_PP_text(((
_tmp780=" ",_tag_dyneither(_tmp780,sizeof(char),2)))),((_tmp77F[0]=ps,Cyc_PP_cat(
_tag_dyneither(_tmp77F,sizeof(struct Cyc_PP_Doc*),3)))))));}else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0){struct Cyc_Core_Failure_struct _tmp78D;const char*
_tmp78C;void*_tmp78B[1];struct Cyc_String_pa_struct _tmp78A;struct Cyc_Core_Failure_struct*
_tmp789;(int)_throw((void*)((_tmp789=_cycalloc(sizeof(*_tmp789)),((_tmp789[0]=((
_tmp78D.tag=Cyc_Core_Failure,((_tmp78D.f1=(struct _dyneither_ptr)((_tmp78A.tag=0,((
_tmp78A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp78B[0]=& _tmp78A,Cyc_aprintf(((_tmp78C="Absynpp::primapp2doc: %s with more than 2 args",
_tag_dyneither(_tmp78C,sizeof(char),47))),_tag_dyneither(_tmp78B,sizeof(void*),1)))))))),
_tmp78D)))),_tmp789)))));}else{const char*_tmp792;const char*_tmp791;struct Cyc_PP_Doc*
_tmp790[5];return(_tmp790[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd,((_tmp790[3]=Cyc_PP_text(((_tmp792=" ",_tag_dyneither(
_tmp792,sizeof(char),2)))),((_tmp790[2]=ps,((_tmp790[1]=Cyc_PP_text(((_tmp791=" ",
_tag_dyneither(_tmp791,sizeof(char),2)))),((_tmp790[0]=(struct Cyc_PP_Doc*)ds->hd,
Cyc_PP_cat(_tag_dyneither(_tmp790,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
struct _dyneither_ptr Cyc_Absynpp_prim2str(void*p);struct _dyneither_ptr Cyc_Absynpp_prim2str(
void*p){void*_tmp2E1=p;_LL22E: if((int)_tmp2E1 != 0)goto _LL230;_LL22F: {const char*
_tmp793;return(_tmp793="+",_tag_dyneither(_tmp793,sizeof(char),2));}_LL230: if((
int)_tmp2E1 != 1)goto _LL232;_LL231: {const char*_tmp794;return(_tmp794="*",
_tag_dyneither(_tmp794,sizeof(char),2));}_LL232: if((int)_tmp2E1 != 2)goto _LL234;
_LL233: {const char*_tmp795;return(_tmp795="-",_tag_dyneither(_tmp795,sizeof(char),
2));}_LL234: if((int)_tmp2E1 != 3)goto _LL236;_LL235: {const char*_tmp796;return(
_tmp796="/",_tag_dyneither(_tmp796,sizeof(char),2));}_LL236: if((int)_tmp2E1 != 4)
goto _LL238;_LL237: {const char*_tmp798;const char*_tmp797;return Cyc_Absynpp_print_for_cycdoc?(
_tmp798="\\%",_tag_dyneither(_tmp798,sizeof(char),3)):((_tmp797="%",
_tag_dyneither(_tmp797,sizeof(char),2)));}_LL238: if((int)_tmp2E1 != 5)goto _LL23A;
_LL239: {const char*_tmp799;return(_tmp799="==",_tag_dyneither(_tmp799,sizeof(
char),3));}_LL23A: if((int)_tmp2E1 != 6)goto _LL23C;_LL23B: {const char*_tmp79A;
return(_tmp79A="!=",_tag_dyneither(_tmp79A,sizeof(char),3));}_LL23C: if((int)
_tmp2E1 != 7)goto _LL23E;_LL23D: {const char*_tmp79B;return(_tmp79B=">",
_tag_dyneither(_tmp79B,sizeof(char),2));}_LL23E: if((int)_tmp2E1 != 8)goto _LL240;
_LL23F: {const char*_tmp79C;return(_tmp79C="<",_tag_dyneither(_tmp79C,sizeof(char),
2));}_LL240: if((int)_tmp2E1 != 9)goto _LL242;_LL241: {const char*_tmp79D;return(
_tmp79D=">=",_tag_dyneither(_tmp79D,sizeof(char),3));}_LL242: if((int)_tmp2E1 != 
10)goto _LL244;_LL243: {const char*_tmp79E;return(_tmp79E="<=",_tag_dyneither(
_tmp79E,sizeof(char),3));}_LL244: if((int)_tmp2E1 != 11)goto _LL246;_LL245: {const
char*_tmp79F;return(_tmp79F="!",_tag_dyneither(_tmp79F,sizeof(char),2));}_LL246:
if((int)_tmp2E1 != 12)goto _LL248;_LL247: {const char*_tmp7A0;return(_tmp7A0="~",
_tag_dyneither(_tmp7A0,sizeof(char),2));}_LL248: if((int)_tmp2E1 != 13)goto _LL24A;
_LL249: {const char*_tmp7A1;return(_tmp7A1="&",_tag_dyneither(_tmp7A1,sizeof(char),
2));}_LL24A: if((int)_tmp2E1 != 14)goto _LL24C;_LL24B: {const char*_tmp7A2;return(
_tmp7A2="|",_tag_dyneither(_tmp7A2,sizeof(char),2));}_LL24C: if((int)_tmp2E1 != 15)
goto _LL24E;_LL24D: {const char*_tmp7A3;return(_tmp7A3="^",_tag_dyneither(_tmp7A3,
sizeof(char),2));}_LL24E: if((int)_tmp2E1 != 16)goto _LL250;_LL24F: {const char*
_tmp7A4;return(_tmp7A4="<<",_tag_dyneither(_tmp7A4,sizeof(char),3));}_LL250: if((
int)_tmp2E1 != 17)goto _LL252;_LL251: {const char*_tmp7A5;return(_tmp7A5=">>",
_tag_dyneither(_tmp7A5,sizeof(char),3));}_LL252: if((int)_tmp2E1 != 18)goto _LL254;
_LL253: {const char*_tmp7A6;return(_tmp7A6=">>>",_tag_dyneither(_tmp7A6,sizeof(
char),4));}_LL254: if((int)_tmp2E1 != 19)goto _LL22D;_LL255: {const char*_tmp7A7;
return(_tmp7A7="numelts",_tag_dyneither(_tmp7A7,sizeof(char),8));}_LL22D:;}
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*p);struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
void*p){return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s);int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp2F7=(void*)s->r;_LL257: if(_tmp2F7 <= (void*)1)goto _LL259;if(*((int*)
_tmp2F7)!= 11)goto _LL259;_LL258: return 1;_LL259:;_LL25A: return 0;_LL256:;}struct
Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st){struct Cyc_PP_Doc*s;{void*_tmp2F8=(void*)st->r;struct Cyc_Absyn_Exp*
_tmp2F9;struct Cyc_Absyn_Stmt*_tmp2FA;struct Cyc_Absyn_Stmt*_tmp2FB;struct Cyc_Absyn_Exp*
_tmp2FC;struct Cyc_Absyn_Exp*_tmp2FD;struct Cyc_Absyn_Stmt*_tmp2FE;struct Cyc_Absyn_Stmt*
_tmp2FF;struct _tuple2 _tmp300;struct Cyc_Absyn_Exp*_tmp301;struct Cyc_Absyn_Stmt*
_tmp302;struct _dyneither_ptr*_tmp303;struct Cyc_Absyn_Exp*_tmp304;struct _tuple2
_tmp305;struct Cyc_Absyn_Exp*_tmp306;struct _tuple2 _tmp307;struct Cyc_Absyn_Exp*
_tmp308;struct Cyc_Absyn_Stmt*_tmp309;struct Cyc_Absyn_Exp*_tmp30A;struct Cyc_List_List*
_tmp30B;struct Cyc_List_List*_tmp30C;struct Cyc_List_List*_tmp30D;struct Cyc_Absyn_Decl*
_tmp30E;struct Cyc_Absyn_Stmt*_tmp30F;struct _dyneither_ptr*_tmp310;struct Cyc_Absyn_Stmt*
_tmp311;struct Cyc_Absyn_Stmt*_tmp312;struct _tuple2 _tmp313;struct Cyc_Absyn_Exp*
_tmp314;struct Cyc_Absyn_Stmt*_tmp315;struct Cyc_List_List*_tmp316;struct Cyc_Absyn_Exp*
_tmp317;_LL25C: if((int)_tmp2F8 != 0)goto _LL25E;_LL25D:{const char*_tmp7A8;s=Cyc_PP_text(((
_tmp7A8=";",_tag_dyneither(_tmp7A8,sizeof(char),2))));}goto _LL25B;_LL25E: if(
_tmp2F8 <= (void*)1)goto _LL260;if(*((int*)_tmp2F8)!= 0)goto _LL260;_tmp2F9=((
struct Cyc_Absyn_Exp_s_struct*)_tmp2F8)->f1;_LL25F:{const char*_tmp7AB;struct Cyc_PP_Doc*
_tmp7AA[2];s=((_tmp7AA[1]=Cyc_PP_text(((_tmp7AB=";",_tag_dyneither(_tmp7AB,
sizeof(char),2)))),((_tmp7AA[0]=Cyc_Absynpp_exp2doc(_tmp2F9),Cyc_PP_cat(
_tag_dyneither(_tmp7AA,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL25B;_LL260: if(
_tmp2F8 <= (void*)1)goto _LL262;if(*((int*)_tmp2F8)!= 1)goto _LL262;_tmp2FA=((
struct Cyc_Absyn_Seq_s_struct*)_tmp2F8)->f1;_tmp2FB=((struct Cyc_Absyn_Seq_s_struct*)
_tmp2F8)->f2;_LL261: if(Cyc_Absynpp_decls_first){if(Cyc_Absynpp_is_declaration(
_tmp2FA)){struct Cyc_PP_Doc*_tmp7AE[5];struct Cyc_PP_Doc*_tmp7AD[7];s=((_tmp7AD[6]=
Cyc_Absynpp_is_declaration(_tmp2FB)?(_tmp7AE[4]=Cyc_PP_line_doc(),((_tmp7AE[3]=
Cyc_Absynpp_rb(),((_tmp7AE[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2FB)),((
_tmp7AE[1]=Cyc_PP_blank_doc(),((_tmp7AE[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp7AE,sizeof(struct Cyc_PP_Doc*),5))))))))))): Cyc_Absynpp_stmt2doc(
_tmp2FB),((_tmp7AD[5]=Cyc_PP_line_doc(),((_tmp7AD[4]=Cyc_Absynpp_rb(),((_tmp7AD[
3]=Cyc_PP_line_doc(),((_tmp7AD[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2FA)),((
_tmp7AD[1]=Cyc_PP_blank_doc(),((_tmp7AD[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp7AD,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{if(Cyc_Absynpp_is_declaration(
_tmp2FB)){struct Cyc_PP_Doc*_tmp7AF[7];s=((_tmp7AF[6]=Cyc_PP_line_doc(),((_tmp7AF[
5]=Cyc_Absynpp_rb(),((_tmp7AF[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2FB)),((
_tmp7AF[3]=Cyc_PP_blank_doc(),((_tmp7AF[2]=Cyc_Absynpp_lb(),((_tmp7AF[1]=Cyc_PP_line_doc(),((
_tmp7AF[0]=Cyc_Absynpp_stmt2doc(_tmp2FA),Cyc_PP_cat(_tag_dyneither(_tmp7AF,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{struct Cyc_List_List*_tmp7B3;
struct Cyc_List_List*_tmp7B2;const char*_tmp7B0;s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Stmt*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,((_tmp7B0="",_tag_dyneither(_tmp7B0,sizeof(char),1))),((
_tmp7B2=_cycalloc(sizeof(*_tmp7B2)),((_tmp7B2->hd=_tmp2FA,((_tmp7B2->tl=((
_tmp7B3=_cycalloc(sizeof(*_tmp7B3)),((_tmp7B3->hd=_tmp2FB,((_tmp7B3->tl=0,
_tmp7B3)))))),_tmp7B2)))))));}}}else{struct Cyc_List_List*_tmp7B7;struct Cyc_List_List*
_tmp7B6;const char*_tmp7B4;s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct
Cyc_Absyn_Stmt*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,((_tmp7B4="",_tag_dyneither(_tmp7B4,sizeof(char),1))),((
_tmp7B6=_cycalloc(sizeof(*_tmp7B6)),((_tmp7B6->hd=_tmp2FA,((_tmp7B6->tl=((
_tmp7B7=_cycalloc(sizeof(*_tmp7B7)),((_tmp7B7->hd=_tmp2FB,((_tmp7B7->tl=0,
_tmp7B7)))))),_tmp7B6)))))));}goto _LL25B;_LL262: if(_tmp2F8 <= (void*)1)goto _LL264;
if(*((int*)_tmp2F8)!= 2)goto _LL264;_tmp2FC=((struct Cyc_Absyn_Return_s_struct*)
_tmp2F8)->f1;_LL263: if(_tmp2FC == 0){const char*_tmp7B8;s=Cyc_PP_text(((_tmp7B8="return;",
_tag_dyneither(_tmp7B8,sizeof(char),8))));}else{const char*_tmp7BD;const char*
_tmp7BC;struct Cyc_PP_Doc*_tmp7BB[3];s=((_tmp7BB[2]=Cyc_PP_text(((_tmp7BD=";",
_tag_dyneither(_tmp7BD,sizeof(char),2)))),((_tmp7BB[1]=_tmp2FC == 0?Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2FC),((_tmp7BB[0]=Cyc_PP_text(((
_tmp7BC="return ",_tag_dyneither(_tmp7BC,sizeof(char),8)))),Cyc_PP_cat(
_tag_dyneither(_tmp7BB,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL25B;_LL264:
if(_tmp2F8 <= (void*)1)goto _LL266;if(*((int*)_tmp2F8)!= 3)goto _LL266;_tmp2FD=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2F8)->f1;_tmp2FE=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp2F8)->f2;_tmp2FF=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2F8)->f3;_LL265: {
int print_else;{void*_tmp328=(void*)_tmp2FF->r;_LL281: if((int)_tmp328 != 0)goto
_LL283;_LL282: print_else=0;goto _LL280;_LL283:;_LL284: print_else=1;goto _LL280;
_LL280:;}{struct Cyc_PP_Doc*_tmp7CC[6];const char*_tmp7CB;struct Cyc_PP_Doc*_tmp7CA[
2];struct Cyc_PP_Doc*_tmp7C9[2];const char*_tmp7C8;const char*_tmp7C7;struct Cyc_PP_Doc*
_tmp7C6[8];s=((_tmp7C6[7]=print_else?(_tmp7CC[5]=Cyc_Absynpp_rb(),((_tmp7CC[4]=
Cyc_PP_line_doc(),((_tmp7CC[3]=Cyc_PP_nest(2,((_tmp7CA[1]=Cyc_Absynpp_stmt2doc(
_tmp2FF),((_tmp7CA[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp7CA,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp7CC[2]=Cyc_Absynpp_lb(),((_tmp7CC[1]=
Cyc_PP_text(((_tmp7CB="else ",_tag_dyneither(_tmp7CB,sizeof(char),6)))),((
_tmp7CC[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp7CC,sizeof(struct Cyc_PP_Doc*),
6))))))))))))): Cyc_PP_nil_doc(),((_tmp7C6[6]=Cyc_Absynpp_rb(),((_tmp7C6[5]=Cyc_PP_line_doc(),((
_tmp7C6[4]=Cyc_PP_nest(2,((_tmp7C9[1]=Cyc_Absynpp_stmt2doc(_tmp2FE),((_tmp7C9[0]=
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp7C9,sizeof(struct Cyc_PP_Doc*),2))))))),((
_tmp7C6[3]=Cyc_Absynpp_lb(),((_tmp7C6[2]=Cyc_PP_text(((_tmp7C8=") ",
_tag_dyneither(_tmp7C8,sizeof(char),3)))),((_tmp7C6[1]=Cyc_Absynpp_exp2doc(
_tmp2FD),((_tmp7C6[0]=Cyc_PP_text(((_tmp7C7="if (",_tag_dyneither(_tmp7C7,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp7C6,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL25B;}_LL266: if(_tmp2F8 <= (void*)1)goto _LL268;if(*((int*)_tmp2F8)!= 4)goto
_LL268;_tmp300=((struct Cyc_Absyn_While_s_struct*)_tmp2F8)->f1;_tmp301=_tmp300.f1;
_tmp302=((struct Cyc_Absyn_While_s_struct*)_tmp2F8)->f2;_LL267:{struct Cyc_PP_Doc*
_tmp7D3[2];const char*_tmp7D2;const char*_tmp7D1;struct Cyc_PP_Doc*_tmp7D0[7];s=((
_tmp7D0[6]=Cyc_Absynpp_rb(),((_tmp7D0[5]=Cyc_PP_line_doc(),((_tmp7D0[4]=Cyc_PP_nest(
2,((_tmp7D3[1]=Cyc_Absynpp_stmt2doc(_tmp302),((_tmp7D3[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp7D3,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp7D0[3]=Cyc_Absynpp_lb(),((
_tmp7D0[2]=Cyc_PP_text(((_tmp7D2=") ",_tag_dyneither(_tmp7D2,sizeof(char),3)))),((
_tmp7D0[1]=Cyc_Absynpp_exp2doc(_tmp301),((_tmp7D0[0]=Cyc_PP_text(((_tmp7D1="while (",
_tag_dyneither(_tmp7D1,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7D0,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL25B;_LL268: if(_tmp2F8 <= (
void*)1)goto _LL26A;if(*((int*)_tmp2F8)!= 5)goto _LL26A;_LL269:{const char*_tmp7D4;
s=Cyc_PP_text(((_tmp7D4="break;",_tag_dyneither(_tmp7D4,sizeof(char),7))));}goto
_LL25B;_LL26A: if(_tmp2F8 <= (void*)1)goto _LL26C;if(*((int*)_tmp2F8)!= 6)goto
_LL26C;_LL26B:{const char*_tmp7D5;s=Cyc_PP_text(((_tmp7D5="continue;",
_tag_dyneither(_tmp7D5,sizeof(char),10))));}goto _LL25B;_LL26C: if(_tmp2F8 <= (void*)
1)goto _LL26E;if(*((int*)_tmp2F8)!= 7)goto _LL26E;_tmp303=((struct Cyc_Absyn_Goto_s_struct*)
_tmp2F8)->f1;_LL26D:{const char*_tmp7D9;void*_tmp7D8[1];struct Cyc_String_pa_struct
_tmp7D7;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp7D7.tag=0,((_tmp7D7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp303),((_tmp7D8[0]=& _tmp7D7,Cyc_aprintf(((
_tmp7D9="goto %s;",_tag_dyneither(_tmp7D9,sizeof(char),9))),_tag_dyneither(
_tmp7D8,sizeof(void*),1)))))))));}goto _LL25B;_LL26E: if(_tmp2F8 <= (void*)1)goto
_LL270;if(*((int*)_tmp2F8)!= 8)goto _LL270;_tmp304=((struct Cyc_Absyn_For_s_struct*)
_tmp2F8)->f1;_tmp305=((struct Cyc_Absyn_For_s_struct*)_tmp2F8)->f2;_tmp306=
_tmp305.f1;_tmp307=((struct Cyc_Absyn_For_s_struct*)_tmp2F8)->f3;_tmp308=_tmp307.f1;
_tmp309=((struct Cyc_Absyn_For_s_struct*)_tmp2F8)->f4;_LL26F:{struct Cyc_PP_Doc*
_tmp7E4[2];const char*_tmp7E3;const char*_tmp7E2;const char*_tmp7E1;const char*
_tmp7E0;struct Cyc_PP_Doc*_tmp7DF[11];s=((_tmp7DF[10]=Cyc_Absynpp_rb(),((_tmp7DF[
9]=Cyc_PP_line_doc(),((_tmp7DF[8]=Cyc_PP_nest(2,((_tmp7E4[1]=Cyc_Absynpp_stmt2doc(
_tmp309),((_tmp7E4[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp7E4,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp7DF[7]=Cyc_Absynpp_lb(),((_tmp7DF[6]=
Cyc_PP_text(((_tmp7E3=") ",_tag_dyneither(_tmp7E3,sizeof(char),3)))),((_tmp7DF[5]=
Cyc_Absynpp_exp2doc(_tmp308),((_tmp7DF[4]=Cyc_PP_text(((_tmp7E2="; ",
_tag_dyneither(_tmp7E2,sizeof(char),3)))),((_tmp7DF[3]=Cyc_Absynpp_exp2doc(
_tmp306),((_tmp7DF[2]=Cyc_PP_text(((_tmp7E1="; ",_tag_dyneither(_tmp7E1,sizeof(
char),3)))),((_tmp7DF[1]=Cyc_Absynpp_exp2doc(_tmp304),((_tmp7DF[0]=Cyc_PP_text(((
_tmp7E0="for(",_tag_dyneither(_tmp7E0,sizeof(char),5)))),Cyc_PP_cat(
_tag_dyneither(_tmp7DF,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL25B;_LL270: if(_tmp2F8 <= (void*)1)goto _LL272;if(*((int*)_tmp2F8)!= 9)goto
_LL272;_tmp30A=((struct Cyc_Absyn_Switch_s_struct*)_tmp2F8)->f1;_tmp30B=((struct
Cyc_Absyn_Switch_s_struct*)_tmp2F8)->f2;_LL271:{const char*_tmp7E9;const char*
_tmp7E8;struct Cyc_PP_Doc*_tmp7E7[8];s=((_tmp7E7[7]=Cyc_Absynpp_rb(),((_tmp7E7[6]=
Cyc_PP_line_doc(),((_tmp7E7[5]=Cyc_Absynpp_switchclauses2doc(_tmp30B),((_tmp7E7[
4]=Cyc_PP_line_doc(),((_tmp7E7[3]=Cyc_Absynpp_lb(),((_tmp7E7[2]=Cyc_PP_text(((
_tmp7E9=") ",_tag_dyneither(_tmp7E9,sizeof(char),3)))),((_tmp7E7[1]=Cyc_Absynpp_exp2doc(
_tmp30A),((_tmp7E7[0]=Cyc_PP_text(((_tmp7E8="switch (",_tag_dyneither(_tmp7E8,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7E7,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL25B;_LL272: if(_tmp2F8 <= (void*)1)goto _LL274;if(*((int*)_tmp2F8)!= 10)goto
_LL274;_tmp30C=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp2F8)->f1;if(_tmp30C != 0)
goto _LL274;_LL273:{const char*_tmp7EA;s=Cyc_PP_text(((_tmp7EA="fallthru;",
_tag_dyneither(_tmp7EA,sizeof(char),10))));}goto _LL25B;_LL274: if(_tmp2F8 <= (void*)
1)goto _LL276;if(*((int*)_tmp2F8)!= 10)goto _LL276;_tmp30D=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp2F8)->f1;_LL275:{const char*_tmp7EF;const char*_tmp7EE;struct Cyc_PP_Doc*
_tmp7ED[3];s=((_tmp7ED[2]=Cyc_PP_text(((_tmp7EF=");",_tag_dyneither(_tmp7EF,
sizeof(char),3)))),((_tmp7ED[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp30D),((_tmp7ED[
0]=Cyc_PP_text(((_tmp7EE="fallthru(",_tag_dyneither(_tmp7EE,sizeof(char),10)))),
Cyc_PP_cat(_tag_dyneither(_tmp7ED,sizeof(struct Cyc_PP_Doc*),3))))))));}goto
_LL25B;_LL276: if(_tmp2F8 <= (void*)1)goto _LL278;if(*((int*)_tmp2F8)!= 11)goto
_LL278;_tmp30E=((struct Cyc_Absyn_Decl_s_struct*)_tmp2F8)->f1;_tmp30F=((struct Cyc_Absyn_Decl_s_struct*)
_tmp2F8)->f2;_LL277:{struct Cyc_PP_Doc*_tmp7F0[3];s=((_tmp7F0[2]=Cyc_Absynpp_stmt2doc(
_tmp30F),((_tmp7F0[1]=Cyc_PP_line_doc(),((_tmp7F0[0]=Cyc_Absynpp_decl2doc(
_tmp30E),Cyc_PP_cat(_tag_dyneither(_tmp7F0,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL25B;_LL278: if(_tmp2F8 <= (void*)1)goto _LL27A;if(*((int*)_tmp2F8)!= 12)goto
_LL27A;_tmp310=((struct Cyc_Absyn_Label_s_struct*)_tmp2F8)->f1;_tmp311=((struct
Cyc_Absyn_Label_s_struct*)_tmp2F8)->f2;_LL279: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(
_tmp311)){const char*_tmp7F3;struct Cyc_PP_Doc*_tmp7F2[7];s=((_tmp7F2[6]=Cyc_Absynpp_rb(),((
_tmp7F2[5]=Cyc_PP_line_doc(),((_tmp7F2[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp311)),((_tmp7F2[3]=Cyc_PP_line_doc(),((_tmp7F2[2]=Cyc_Absynpp_lb(),((_tmp7F2[
1]=Cyc_PP_text(((_tmp7F3=": ",_tag_dyneither(_tmp7F3,sizeof(char),3)))),((
_tmp7F2[0]=Cyc_PP_textptr(_tmp310),Cyc_PP_cat(_tag_dyneither(_tmp7F2,sizeof(
struct Cyc_PP_Doc*),7))))))))))))))));}else{const char*_tmp7F6;struct Cyc_PP_Doc*
_tmp7F5[3];s=((_tmp7F5[2]=Cyc_Absynpp_stmt2doc(_tmp311),((_tmp7F5[1]=Cyc_PP_text(((
_tmp7F6=": ",_tag_dyneither(_tmp7F6,sizeof(char),3)))),((_tmp7F5[0]=Cyc_PP_textptr(
_tmp310),Cyc_PP_cat(_tag_dyneither(_tmp7F5,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL25B;_LL27A: if(_tmp2F8 <= (void*)1)goto _LL27C;if(*((int*)_tmp2F8)!= 13)goto
_LL27C;_tmp312=((struct Cyc_Absyn_Do_s_struct*)_tmp2F8)->f1;_tmp313=((struct Cyc_Absyn_Do_s_struct*)
_tmp2F8)->f2;_tmp314=_tmp313.f1;_LL27B:{const char*_tmp7FD;const char*_tmp7FC;
const char*_tmp7FB;struct Cyc_PP_Doc*_tmp7FA[9];s=((_tmp7FA[8]=Cyc_PP_text(((
_tmp7FD=");",_tag_dyneither(_tmp7FD,sizeof(char),3)))),((_tmp7FA[7]=Cyc_Absynpp_exp2doc(
_tmp314),((_tmp7FA[6]=Cyc_PP_text(((_tmp7FC=" while (",_tag_dyneither(_tmp7FC,
sizeof(char),9)))),((_tmp7FA[5]=Cyc_Absynpp_rb(),((_tmp7FA[4]=Cyc_PP_line_doc(),((
_tmp7FA[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp312)),((_tmp7FA[2]=Cyc_PP_line_doc(),((
_tmp7FA[1]=Cyc_Absynpp_lb(),((_tmp7FA[0]=Cyc_PP_text(((_tmp7FB="do ",
_tag_dyneither(_tmp7FB,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp7FA,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL25B;_LL27C: if(_tmp2F8 <= (
void*)1)goto _LL27E;if(*((int*)_tmp2F8)!= 14)goto _LL27E;_tmp315=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp2F8)->f1;_tmp316=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp2F8)->f2;_LL27D:{
const char*_tmp802;const char*_tmp801;struct Cyc_PP_Doc*_tmp800[12];s=((_tmp800[11]=
Cyc_Absynpp_rb(),((_tmp800[10]=Cyc_PP_line_doc(),((_tmp800[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp316)),((_tmp800[8]=Cyc_PP_line_doc(),((_tmp800[7]=Cyc_Absynpp_lb(),((_tmp800[
6]=Cyc_PP_text(((_tmp802=" catch ",_tag_dyneither(_tmp802,sizeof(char),8)))),((
_tmp800[5]=Cyc_Absynpp_rb(),((_tmp800[4]=Cyc_PP_line_doc(),((_tmp800[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp315)),((_tmp800[2]=Cyc_PP_line_doc(),((_tmp800[1]=Cyc_Absynpp_lb(),((
_tmp800[0]=Cyc_PP_text(((_tmp801="try ",_tag_dyneither(_tmp801,sizeof(char),5)))),
Cyc_PP_cat(_tag_dyneither(_tmp800,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
goto _LL25B;_LL27E: if(_tmp2F8 <= (void*)1)goto _LL25B;if(*((int*)_tmp2F8)!= 15)goto
_LL25B;_tmp317=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp2F8)->f1;_LL27F:{
const char*_tmp807;const char*_tmp806;struct Cyc_PP_Doc*_tmp805[3];s=((_tmp805[2]=
Cyc_PP_text(((_tmp807=");",_tag_dyneither(_tmp807,sizeof(char),3)))),((_tmp805[1]=
Cyc_Absynpp_exp2doc(_tmp317),((_tmp805[0]=Cyc_PP_text(((_tmp806="reset_region(",
_tag_dyneither(_tmp806,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp805,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL25B;_LL25B:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p);struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{void*_tmp355=(void*)p->r;void*_tmp356;
int _tmp357;char _tmp358;struct _dyneither_ptr _tmp359;struct Cyc_Absyn_Vardecl*
_tmp35A;struct Cyc_Absyn_Pat*_tmp35B;struct Cyc_Absyn_Pat _tmp35C;void*_tmp35D;
struct Cyc_Absyn_Vardecl*_tmp35E;struct Cyc_Absyn_Pat*_tmp35F;struct Cyc_Absyn_Tvar*
_tmp360;struct Cyc_Absyn_Vardecl*_tmp361;struct Cyc_List_List*_tmp362;int _tmp363;
struct Cyc_Absyn_Pat*_tmp364;struct Cyc_Absyn_Vardecl*_tmp365;struct Cyc_Absyn_Pat*
_tmp366;struct Cyc_Absyn_Pat _tmp367;void*_tmp368;struct Cyc_Absyn_Vardecl*_tmp369;
struct Cyc_Absyn_Pat*_tmp36A;struct _tuple0*_tmp36B;struct _tuple0*_tmp36C;struct
Cyc_List_List*_tmp36D;int _tmp36E;struct Cyc_Absyn_AggrInfo _tmp36F;union Cyc_Absyn_AggrInfoU_union
_tmp370;struct Cyc_List_List*_tmp371;struct Cyc_List_List*_tmp372;int _tmp373;
struct Cyc_Absyn_Enumfield*_tmp374;struct Cyc_Absyn_Enumfield*_tmp375;struct Cyc_Absyn_Datatypefield*
_tmp376;struct Cyc_List_List*_tmp377;struct Cyc_Absyn_Datatypefield*_tmp378;struct
Cyc_List_List*_tmp379;int _tmp37A;struct Cyc_Absyn_Exp*_tmp37B;_LL286: if((int)
_tmp355 != 0)goto _LL288;_LL287:{const char*_tmp808;s=Cyc_PP_text(((_tmp808="_",
_tag_dyneither(_tmp808,sizeof(char),2))));}goto _LL285;_LL288: if((int)_tmp355 != 1)
goto _LL28A;_LL289:{const char*_tmp809;s=Cyc_PP_text(((_tmp809="NULL",
_tag_dyneither(_tmp809,sizeof(char),5))));}goto _LL285;_LL28A: if(_tmp355 <= (void*)
2)goto _LL28C;if(*((int*)_tmp355)!= 7)goto _LL28C;_tmp356=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp355)->f1;_tmp357=((struct Cyc_Absyn_Int_p_struct*)_tmp355)->f2;_LL28B: if(
_tmp356 != (void*)((void*)1)){const char*_tmp80D;void*_tmp80C[1];struct Cyc_Int_pa_struct
_tmp80B;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp80B.tag=1,((_tmp80B.f1=(
unsigned long)_tmp357,((_tmp80C[0]=& _tmp80B,Cyc_aprintf(((_tmp80D="%d",
_tag_dyneither(_tmp80D,sizeof(char),3))),_tag_dyneither(_tmp80C,sizeof(void*),1)))))))));}
else{const char*_tmp811;void*_tmp810[1];struct Cyc_Int_pa_struct _tmp80F;s=Cyc_PP_text((
struct _dyneither_ptr)((_tmp80F.tag=1,((_tmp80F.f1=(unsigned int)_tmp357,((
_tmp810[0]=& _tmp80F,Cyc_aprintf(((_tmp811="%u",_tag_dyneither(_tmp811,sizeof(
char),3))),_tag_dyneither(_tmp810,sizeof(void*),1)))))))));}goto _LL285;_LL28C:
if(_tmp355 <= (void*)2)goto _LL28E;if(*((int*)_tmp355)!= 8)goto _LL28E;_tmp358=((
struct Cyc_Absyn_Char_p_struct*)_tmp355)->f1;_LL28D:{const char*_tmp815;void*
_tmp814[1];struct Cyc_String_pa_struct _tmp813;s=Cyc_PP_text((struct _dyneither_ptr)((
_tmp813.tag=0,((_tmp813.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(
_tmp358)),((_tmp814[0]=& _tmp813,Cyc_aprintf(((_tmp815="'%s'",_tag_dyneither(
_tmp815,sizeof(char),5))),_tag_dyneither(_tmp814,sizeof(void*),1)))))))));}goto
_LL285;_LL28E: if(_tmp355 <= (void*)2)goto _LL290;if(*((int*)_tmp355)!= 9)goto
_LL290;_tmp359=((struct Cyc_Absyn_Float_p_struct*)_tmp355)->f1;_LL28F: s=Cyc_PP_text(
_tmp359);goto _LL285;_LL290: if(_tmp355 <= (void*)2)goto _LL292;if(*((int*)_tmp355)
!= 0)goto _LL292;_tmp35A=((struct Cyc_Absyn_Var_p_struct*)_tmp355)->f1;_tmp35B=((
struct Cyc_Absyn_Var_p_struct*)_tmp355)->f2;_tmp35C=*_tmp35B;_tmp35D=(void*)
_tmp35C.r;if((int)_tmp35D != 0)goto _LL292;_LL291: s=Cyc_Absynpp_qvar2doc(_tmp35A->name);
goto _LL285;_LL292: if(_tmp355 <= (void*)2)goto _LL294;if(*((int*)_tmp355)!= 0)goto
_LL294;_tmp35E=((struct Cyc_Absyn_Var_p_struct*)_tmp355)->f1;_tmp35F=((struct Cyc_Absyn_Var_p_struct*)
_tmp355)->f2;_LL293:{const char*_tmp818;struct Cyc_PP_Doc*_tmp817[3];s=((_tmp817[2]=
Cyc_Absynpp_pat2doc(_tmp35F),((_tmp817[1]=Cyc_PP_text(((_tmp818=" as ",
_tag_dyneither(_tmp818,sizeof(char),5)))),((_tmp817[0]=Cyc_Absynpp_qvar2doc(
_tmp35E->name),Cyc_PP_cat(_tag_dyneither(_tmp817,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL285;_LL294: if(_tmp355 <= (void*)2)goto _LL296;if(*((int*)_tmp355)!= 2)goto
_LL296;_tmp360=((struct Cyc_Absyn_TagInt_p_struct*)_tmp355)->f1;_tmp361=((struct
Cyc_Absyn_TagInt_p_struct*)_tmp355)->f2;_LL295:{const char*_tmp81D;const char*
_tmp81C;struct Cyc_PP_Doc*_tmp81B[4];s=((_tmp81B[3]=Cyc_PP_text(((_tmp81D=">",
_tag_dyneither(_tmp81D,sizeof(char),2)))),((_tmp81B[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp360)),((_tmp81B[1]=Cyc_PP_text(((_tmp81C="<",_tag_dyneither(_tmp81C,sizeof(
char),2)))),((_tmp81B[0]=Cyc_Absynpp_qvar2doc(_tmp361->name),Cyc_PP_cat(
_tag_dyneither(_tmp81B,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL285;_LL296:
if(_tmp355 <= (void*)2)goto _LL298;if(*((int*)_tmp355)!= 3)goto _LL298;_tmp362=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp355)->f1;_tmp363=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp355)->f2;_LL297:{const char*_tmp826;const char*_tmp825;const char*_tmp824;const
char*_tmp823;struct Cyc_PP_Doc*_tmp822[4];s=((_tmp822[3]=_tmp363?Cyc_PP_text(((
_tmp825=", ...)",_tag_dyneither(_tmp825,sizeof(char),7)))): Cyc_PP_text(((_tmp826=")",
_tag_dyneither(_tmp826,sizeof(char),2)))),((_tmp822[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp824=",",_tag_dyneither(_tmp824,
sizeof(char),2))),_tmp362),((_tmp822[1]=Cyc_PP_text(((_tmp823="(",_tag_dyneither(
_tmp823,sizeof(char),2)))),((_tmp822[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(
_tag_dyneither(_tmp822,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL285;_LL298:
if(_tmp355 <= (void*)2)goto _LL29A;if(*((int*)_tmp355)!= 4)goto _LL29A;_tmp364=((
struct Cyc_Absyn_Pointer_p_struct*)_tmp355)->f1;_LL299:{const char*_tmp829;struct
Cyc_PP_Doc*_tmp828[2];s=((_tmp828[1]=Cyc_Absynpp_pat2doc(_tmp364),((_tmp828[0]=
Cyc_PP_text(((_tmp829="&",_tag_dyneither(_tmp829,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp828,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL285;_LL29A: if(
_tmp355 <= (void*)2)goto _LL29C;if(*((int*)_tmp355)!= 1)goto _LL29C;_tmp365=((
struct Cyc_Absyn_Reference_p_struct*)_tmp355)->f1;_tmp366=((struct Cyc_Absyn_Reference_p_struct*)
_tmp355)->f2;_tmp367=*_tmp366;_tmp368=(void*)_tmp367.r;if((int)_tmp368 != 0)goto
_LL29C;_LL29B:{const char*_tmp82C;struct Cyc_PP_Doc*_tmp82B[2];s=((_tmp82B[1]=Cyc_Absynpp_qvar2doc(
_tmp365->name),((_tmp82B[0]=Cyc_PP_text(((_tmp82C="*",_tag_dyneither(_tmp82C,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp82B,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL285;_LL29C: if(_tmp355 <= (void*)2)goto _LL29E;if(*((int*)_tmp355)!= 1)goto
_LL29E;_tmp369=((struct Cyc_Absyn_Reference_p_struct*)_tmp355)->f1;_tmp36A=((
struct Cyc_Absyn_Reference_p_struct*)_tmp355)->f2;_LL29D:{const char*_tmp831;const
char*_tmp830;struct Cyc_PP_Doc*_tmp82F[4];s=((_tmp82F[3]=Cyc_Absynpp_pat2doc(
_tmp36A),((_tmp82F[2]=Cyc_PP_text(((_tmp831=" as ",_tag_dyneither(_tmp831,
sizeof(char),5)))),((_tmp82F[1]=Cyc_Absynpp_qvar2doc(_tmp369->name),((_tmp82F[0]=
Cyc_PP_text(((_tmp830="*",_tag_dyneither(_tmp830,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp82F,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL285;_LL29E:
if(_tmp355 <= (void*)2)goto _LL2A0;if(*((int*)_tmp355)!= 12)goto _LL2A0;_tmp36B=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp355)->f1;_LL29F: s=Cyc_Absynpp_qvar2doc(
_tmp36B);goto _LL285;_LL2A0: if(_tmp355 <= (void*)2)goto _LL2A2;if(*((int*)_tmp355)
!= 13)goto _LL2A2;_tmp36C=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp355)->f1;
_tmp36D=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp355)->f2;_tmp36E=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp355)->f3;_LL2A1: {const char*_tmp833;const char*_tmp832;struct _dyneither_ptr
term=_tmp36E?(_tmp833=", ...)",_tag_dyneither(_tmp833,sizeof(char),7)):((_tmp832=")",
_tag_dyneither(_tmp832,sizeof(char),2)));{const char*_tmp838;const char*_tmp837;
struct Cyc_PP_Doc*_tmp836[2];s=((_tmp836[1]=Cyc_PP_group(((_tmp838="(",
_tag_dyneither(_tmp838,sizeof(char),2))),term,((_tmp837=",",_tag_dyneither(
_tmp837,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp36D)),((
_tmp836[0]=Cyc_Absynpp_qvar2doc(_tmp36C),Cyc_PP_cat(_tag_dyneither(_tmp836,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL285;}_LL2A2: if(_tmp355 <= (void*)2)goto
_LL2A4;if(*((int*)_tmp355)!= 5)goto _LL2A4;_tmp36F=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp355)->f1;_tmp370=_tmp36F.aggr_info;_tmp371=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp355)->f2;_tmp372=((struct Cyc_Absyn_Aggr_p_struct*)_tmp355)->f3;_tmp373=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp355)->f4;_LL2A3: {const char*_tmp83A;const char*
_tmp839;struct _dyneither_ptr term=_tmp373?(_tmp83A=", ...}",_tag_dyneither(
_tmp83A,sizeof(char),7)):((_tmp839="}",_tag_dyneither(_tmp839,sizeof(char),2)));
struct _tuple0*_tmp39E;struct _tuple3 _tmp39D=Cyc_Absyn_aggr_kinded_name(_tmp370);
_tmp39E=_tmp39D.f2;{const char*_tmp845;const char*_tmp844;const char*_tmp843;const
char*_tmp842;const char*_tmp841;struct Cyc_PP_Doc*_tmp840[4];s=((_tmp840[3]=Cyc_PP_group(((
_tmp845="",_tag_dyneither(_tmp845,sizeof(char),1))),term,((_tmp844=",",
_tag_dyneither(_tmp844,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp372)),((_tmp840[2]=Cyc_PP_egroup(((_tmp843="[",_tag_dyneither(_tmp843,
sizeof(char),2))),((_tmp842="]",_tag_dyneither(_tmp842,sizeof(char),2))),((
_tmp841=",",_tag_dyneither(_tmp841,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp371))),((_tmp840[1]=
Cyc_Absynpp_lb(),((_tmp840[0]=Cyc_Absynpp_qvar2doc(_tmp39E),Cyc_PP_cat(
_tag_dyneither(_tmp840,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL285;}_LL2A4:
if(_tmp355 <= (void*)2)goto _LL2A6;if(*((int*)_tmp355)!= 10)goto _LL2A6;_tmp374=((
struct Cyc_Absyn_Enum_p_struct*)_tmp355)->f2;_LL2A5: _tmp375=_tmp374;goto _LL2A7;
_LL2A6: if(_tmp355 <= (void*)2)goto _LL2A8;if(*((int*)_tmp355)!= 11)goto _LL2A8;
_tmp375=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp355)->f2;_LL2A7: s=Cyc_Absynpp_qvar2doc(
_tmp375->name);goto _LL285;_LL2A8: if(_tmp355 <= (void*)2)goto _LL2AA;if(*((int*)
_tmp355)!= 6)goto _LL2AA;_tmp376=((struct Cyc_Absyn_Datatype_p_struct*)_tmp355)->f2;
_tmp377=((struct Cyc_Absyn_Datatype_p_struct*)_tmp355)->f3;if(_tmp377 != 0)goto
_LL2AA;_LL2A9: s=Cyc_Absynpp_qvar2doc(_tmp376->name);goto _LL285;_LL2AA: if(_tmp355
<= (void*)2)goto _LL2AC;if(*((int*)_tmp355)!= 6)goto _LL2AC;_tmp378=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp355)->f2;_tmp379=((struct Cyc_Absyn_Datatype_p_struct*)_tmp355)->f3;_tmp37A=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp355)->f4;_LL2AB: {const char*_tmp847;const
char*_tmp846;struct _dyneither_ptr term=_tmp37A?(_tmp847=", ...)",_tag_dyneither(
_tmp847,sizeof(char),7)):((_tmp846=")",_tag_dyneither(_tmp846,sizeof(char),2)));{
const char*_tmp84C;const char*_tmp84B;struct Cyc_PP_Doc*_tmp84A[2];s=((_tmp84A[1]=
Cyc_PP_egroup(((_tmp84C="(",_tag_dyneither(_tmp84C,sizeof(char),2))),term,((
_tmp84B=",",_tag_dyneither(_tmp84B,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp379)),((_tmp84A[0]=Cyc_Absynpp_qvar2doc(_tmp378->name),
Cyc_PP_cat(_tag_dyneither(_tmp84A,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL285;}
_LL2AC: if(_tmp355 <= (void*)2)goto _LL285;if(*((int*)_tmp355)!= 14)goto _LL285;
_tmp37B=((struct Cyc_Absyn_Exp_p_struct*)_tmp355)->f1;_LL2AD: s=Cyc_Absynpp_exp2doc(
_tmp37B);goto _LL285;_LL285:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple8*dp);struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*dp){const char*
_tmp853;const char*_tmp852;const char*_tmp851;struct Cyc_PP_Doc*_tmp850[2];return(
_tmp850[1]=Cyc_Absynpp_pat2doc((*dp).f2),((_tmp850[0]=Cyc_PP_egroup(((_tmp853="",
_tag_dyneither(_tmp853,sizeof(char),1))),((_tmp852="=",_tag_dyneither(_tmp852,
sizeof(char),2))),((_tmp851="=",_tag_dyneither(_tmp851,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp850,sizeof(
struct Cyc_PP_Doc*),2)))));}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c);struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (void*)(c->pattern)->r == (
void*)((void*)0)){struct Cyc_PP_Doc*_tmp858[2];const char*_tmp857;struct Cyc_PP_Doc*
_tmp856[2];return(_tmp856[1]=Cyc_PP_nest(2,((_tmp858[1]=Cyc_Absynpp_stmt2doc(c->body),((
_tmp858[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp858,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp856[0]=Cyc_PP_text(((_tmp857="default: ",_tag_dyneither(_tmp857,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp856,sizeof(struct Cyc_PP_Doc*),
2)))));}else{if(c->where_clause == 0){struct Cyc_PP_Doc*_tmp85F[2];const char*
_tmp85E;const char*_tmp85D;struct Cyc_PP_Doc*_tmp85C[4];return(_tmp85C[3]=Cyc_PP_nest(
2,((_tmp85F[1]=Cyc_Absynpp_stmt2doc(c->body),((_tmp85F[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp85F,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp85C[2]=Cyc_PP_text(((
_tmp85E=": ",_tag_dyneither(_tmp85E,sizeof(char),3)))),((_tmp85C[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp85C[0]=Cyc_PP_text(((_tmp85D="case ",_tag_dyneither(_tmp85D,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp85C,sizeof(struct Cyc_PP_Doc*),4)))))))));}
else{struct Cyc_PP_Doc*_tmp868[2];const char*_tmp867;const char*_tmp866;const char*
_tmp865;struct Cyc_PP_Doc*_tmp864[6];return(_tmp864[5]=Cyc_PP_nest(2,((_tmp868[1]=
Cyc_Absynpp_stmt2doc(c->body),((_tmp868[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp868,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp864[4]=Cyc_PP_text(((
_tmp867=": ",_tag_dyneither(_tmp867,sizeof(char),3)))),((_tmp864[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp864[2]=Cyc_PP_text(((
_tmp866=" && ",_tag_dyneither(_tmp866,sizeof(char),5)))),((_tmp864[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp864[0]=Cyc_PP_text(((_tmp865="case ",_tag_dyneither(_tmp865,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp864,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){const char*_tmp869;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((
_tmp869="",_tag_dyneither(_tmp869,sizeof(char),1))),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct
Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);else{
const char*_tmp86C;struct Cyc_PP_Doc*_tmp86B[3];return(_tmp86B[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp86B[1]=Cyc_PP_text(((_tmp86C=" = ",
_tag_dyneither(_tmp86C,sizeof(char),4)))),((_tmp86B[0]=Cyc_Absynpp_qvar2doc(f->name),
Cyc_PP_cat(_tag_dyneither(_tmp86B,sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){const char*_tmp86D;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp86D=",",_tag_dyneither(_tmp86D,
sizeof(char),2))),fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*
v);static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return
Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct
Cyc_List_List*vds);static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*
vds){const char*_tmp86E;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_id2doc,((_tmp86E=",",_tag_dyneither(_tmp86E,sizeof(char),2))),vds);}
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);struct Cyc_PP_Doc*
Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl
_tmp3C2;void*_tmp3C3;struct _tuple0*_tmp3C4;struct Cyc_Absyn_Tqual _tmp3C5;void*
_tmp3C6;struct Cyc_Absyn_Exp*_tmp3C7;struct Cyc_List_List*_tmp3C8;struct Cyc_Absyn_Vardecl*
_tmp3C1=vd;_tmp3C2=*_tmp3C1;_tmp3C3=(void*)_tmp3C2.sc;_tmp3C4=_tmp3C2.name;
_tmp3C5=_tmp3C2.tq;_tmp3C6=(void*)_tmp3C2.type;_tmp3C7=_tmp3C2.initializer;
_tmp3C8=_tmp3C2.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(
_tmp3C4);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp3C8);struct Cyc_PP_Doc*
beforenamedoc;{void*_tmp3C9=Cyc_Cyclone_c_compiler;_LL2AF: if((int)_tmp3C9 != 0)
goto _LL2B1;_LL2B0: beforenamedoc=attsdoc;goto _LL2AE;_LL2B1: if((int)_tmp3C9 != 1)
goto _LL2AE;_LL2B2:{void*_tmp3CA=Cyc_Tcutil_compress(_tmp3C6);struct Cyc_Absyn_FnInfo
_tmp3CB;struct Cyc_List_List*_tmp3CC;_LL2B4: if(_tmp3CA <= (void*)4)goto _LL2B6;if(*((
int*)_tmp3CA)!= 8)goto _LL2B6;_tmp3CB=((struct Cyc_Absyn_FnType_struct*)_tmp3CA)->f1;
_tmp3CC=_tmp3CB.attributes;_LL2B5: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp3CC);
goto _LL2B3;_LL2B6:;_LL2B7: beforenamedoc=Cyc_PP_nil_doc();goto _LL2B3;_LL2B3:;}
goto _LL2AE;_LL2AE:;}{struct Cyc_PP_Doc*tmp_doc;{void*_tmp3CD=Cyc_Cyclone_c_compiler;
_LL2B9: if((int)_tmp3CD != 0)goto _LL2BB;_LL2BA: tmp_doc=Cyc_PP_nil_doc();goto _LL2B8;
_LL2BB: if((int)_tmp3CD != 1)goto _LL2B8;_LL2BC: tmp_doc=attsdoc;goto _LL2B8;_LL2B8:;}{
const char*_tmp87B;struct Cyc_PP_Doc*_tmp87A[2];const char*_tmp879;struct Cyc_Core_Opt*
_tmp878;struct Cyc_PP_Doc*_tmp877[2];struct Cyc_PP_Doc*_tmp876[5];s=((_tmp876[4]=
Cyc_PP_text(((_tmp87B=";",_tag_dyneither(_tmp87B,sizeof(char),2)))),((_tmp876[3]=
_tmp3C7 == 0?Cyc_PP_nil_doc():((_tmp87A[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp3C7),((_tmp87A[0]=Cyc_PP_text(((_tmp879=" = ",_tag_dyneither(_tmp879,sizeof(
char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp87A,sizeof(struct Cyc_PP_Doc*),2)))))),((
_tmp876[2]=Cyc_Absynpp_tqtd2doc(_tmp3C5,_tmp3C6,((_tmp878=_cycalloc(sizeof(*
_tmp878)),((_tmp878->v=((_tmp877[1]=sn,((_tmp877[0]=beforenamedoc,Cyc_PP_cat(
_tag_dyneither(_tmp877,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp878))))),((_tmp876[
1]=Cyc_Absynpp_scope2doc(_tmp3C3),((_tmp876[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(
_tmp876,sizeof(struct Cyc_PP_Doc*),5))))))))))));}return s;}}}struct _tuple11{
struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(
struct _tuple11*x);struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple11*x){
struct _tuple0*_tmp3D5;struct _tuple11 _tmp3D4=*x;_tmp3D5=_tmp3D4.f2;return Cyc_Absynpp_qvar2doc(
_tmp3D5);}struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);struct
Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*
_tmp3D6=(void*)d->r;struct Cyc_Absyn_Fndecl*_tmp3D7;struct Cyc_Absyn_Aggrdecl*
_tmp3D8;struct Cyc_Absyn_Vardecl*_tmp3D9;struct Cyc_Absyn_Tvar*_tmp3DA;struct Cyc_Absyn_Vardecl*
_tmp3DB;int _tmp3DC;struct Cyc_Absyn_Exp*_tmp3DD;struct Cyc_Absyn_Exp*_tmp3DE;
struct Cyc_Absyn_Tvar*_tmp3DF;struct Cyc_Absyn_Vardecl*_tmp3E0;struct Cyc_Absyn_Datatypedecl*
_tmp3E1;struct Cyc_Absyn_Datatypedecl _tmp3E2;void*_tmp3E3;struct _tuple0*_tmp3E4;
struct Cyc_List_List*_tmp3E5;struct Cyc_Core_Opt*_tmp3E6;int _tmp3E7;int _tmp3E8;
struct Cyc_Absyn_Pat*_tmp3E9;struct Cyc_Absyn_Exp*_tmp3EA;struct Cyc_List_List*
_tmp3EB;struct Cyc_Absyn_Enumdecl*_tmp3EC;struct Cyc_Absyn_Enumdecl _tmp3ED;void*
_tmp3EE;struct _tuple0*_tmp3EF;struct Cyc_Core_Opt*_tmp3F0;struct Cyc_Absyn_Typedefdecl*
_tmp3F1;struct _dyneither_ptr*_tmp3F2;struct Cyc_List_List*_tmp3F3;struct _tuple0*
_tmp3F4;struct Cyc_List_List*_tmp3F5;struct Cyc_List_List*_tmp3F6;struct Cyc_List_List*
_tmp3F7;struct Cyc_List_List*_tmp3F8;_LL2BE: if(_tmp3D6 <= (void*)2)goto _LL2DA;if(*((
int*)_tmp3D6)!= 1)goto _LL2C0;_tmp3D7=((struct Cyc_Absyn_Fn_d_struct*)_tmp3D6)->f1;
_LL2BF: {struct Cyc_Absyn_FnType_struct _tmp881;struct Cyc_Absyn_FnInfo _tmp880;
struct Cyc_Absyn_FnType_struct*_tmp87F;void*t=(void*)((_tmp87F=_cycalloc(sizeof(*
_tmp87F)),((_tmp87F[0]=((_tmp881.tag=8,((_tmp881.f1=((_tmp880.tvars=_tmp3D7->tvs,((
_tmp880.effect=_tmp3D7->effect,((_tmp880.ret_typ=(void*)((void*)_tmp3D7->ret_type),((
_tmp880.args=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple6*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp3D7->args),((_tmp880.c_varargs=
_tmp3D7->c_varargs,((_tmp880.cyc_varargs=_tmp3D7->cyc_varargs,((_tmp880.rgn_po=
_tmp3D7->rgn_po,((_tmp880.attributes=0,_tmp880)))))))))))))))),_tmp881)))),
_tmp87F))));struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp3D7->attributes);
struct Cyc_PP_Doc*inlinedoc;if(_tmp3D7->is_inline){void*_tmp3F9=Cyc_Cyclone_c_compiler;
_LL2DF: if((int)_tmp3F9 != 0)goto _LL2E1;_LL2E0:{const char*_tmp882;inlinedoc=Cyc_PP_text(((
_tmp882="inline ",_tag_dyneither(_tmp882,sizeof(char),8))));}goto _LL2DE;_LL2E1:
if((int)_tmp3F9 != 1)goto _LL2DE;_LL2E2:{const char*_tmp883;inlinedoc=Cyc_PP_text(((
_tmp883="__inline ",_tag_dyneither(_tmp883,sizeof(char),10))));}goto _LL2DE;
_LL2DE:;}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc((
void*)_tmp3D7->sc);struct Cyc_PP_Doc*beforenamedoc;{void*_tmp3FC=Cyc_Cyclone_c_compiler;
_LL2E4: if((int)_tmp3FC != 0)goto _LL2E6;_LL2E5: beforenamedoc=attsdoc;goto _LL2E3;
_LL2E6: if((int)_tmp3FC != 1)goto _LL2E3;_LL2E7: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp3D7->attributes);goto _LL2E3;_LL2E3:;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp3D7->name);struct Cyc_PP_Doc*_tmp886[2];struct Cyc_Core_Opt*_tmp885;struct Cyc_PP_Doc*
tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,((_tmp885=_cycalloc(
sizeof(*_tmp885)),((_tmp885->v=((_tmp886[1]=namedoc,((_tmp886[0]=beforenamedoc,
Cyc_PP_cat(_tag_dyneither(_tmp886,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp885)))));
struct Cyc_PP_Doc*_tmp889[2];struct Cyc_PP_Doc*_tmp888[5];struct Cyc_PP_Doc*bodydoc=(
_tmp888[4]=Cyc_Absynpp_rb(),((_tmp888[3]=Cyc_PP_line_doc(),((_tmp888[2]=Cyc_PP_nest(
2,((_tmp889[1]=Cyc_Absynpp_stmt2doc(_tmp3D7->body),((_tmp889[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dyneither(_tmp889,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp888[1]=
Cyc_Absynpp_lb(),((_tmp888[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(
_tmp888,sizeof(struct Cyc_PP_Doc*),5)))))))))));{struct Cyc_PP_Doc*_tmp88A[4];s=((
_tmp88A[3]=bodydoc,((_tmp88A[2]=tqtddoc,((_tmp88A[1]=scopedoc,((_tmp88A[0]=
inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp88A,sizeof(struct Cyc_PP_Doc*),4))))))))));}{
void*_tmp3FE=Cyc_Cyclone_c_compiler;_LL2E9: if((int)_tmp3FE != 1)goto _LL2EB;_LL2EA:{
struct Cyc_PP_Doc*_tmp88B[2];s=((_tmp88B[1]=s,((_tmp88B[0]=attsdoc,Cyc_PP_cat(
_tag_dyneither(_tmp88B,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL2E8;_LL2EB:;
_LL2EC: goto _LL2E8;_LL2E8:;}goto _LL2BD;}}}_LL2C0: if(*((int*)_tmp3D6)!= 6)goto
_LL2C2;_tmp3D8=((struct Cyc_Absyn_Aggr_d_struct*)_tmp3D6)->f1;_LL2C1: if(_tmp3D8->impl
== 0){const char*_tmp88E;struct Cyc_PP_Doc*_tmp88D[5];s=((_tmp88D[4]=Cyc_PP_text(((
_tmp88E=";",_tag_dyneither(_tmp88E,sizeof(char),2)))),((_tmp88D[3]=Cyc_Absynpp_ktvars2doc(
_tmp3D8->tvs),((_tmp88D[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3D8->name),((_tmp88D[1]=
Cyc_Absynpp_aggr_kind2doc((void*)_tmp3D8->kind),((_tmp88D[0]=Cyc_Absynpp_scope2doc((
void*)_tmp3D8->sc),Cyc_PP_cat(_tag_dyneither(_tmp88D,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
else{const char*_tmp898;struct Cyc_PP_Doc*_tmp897[2];struct Cyc_PP_Doc*_tmp896[2];
const char*_tmp895;struct Cyc_PP_Doc*_tmp894[12];s=((_tmp894[11]=Cyc_PP_text(((
_tmp898=";",_tag_dyneither(_tmp898,sizeof(char),2)))),((_tmp894[10]=Cyc_Absynpp_atts2doc(
_tmp3D8->attributes),((_tmp894[9]=Cyc_Absynpp_rb(),((_tmp894[8]=Cyc_PP_line_doc(),((
_tmp894[7]=Cyc_PP_nest(2,((_tmp897[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3D8->impl))->fields),((_tmp897[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp897,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp894[6]=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3D8->impl))->rgn_po == 0?Cyc_PP_nil_doc():((
_tmp896[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp3D8->impl))->rgn_po),((_tmp896[0]=Cyc_PP_text(((_tmp895=":",_tag_dyneither(
_tmp895,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp896,sizeof(struct Cyc_PP_Doc*),
2)))))),((_tmp894[5]=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3D8->impl))->exist_vars),((_tmp894[4]=Cyc_Absynpp_lb(),((_tmp894[
3]=Cyc_PP_blank_doc(),((_tmp894[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3D8->name),((
_tmp894[1]=Cyc_Absynpp_aggr_kind2doc((void*)_tmp3D8->kind),((_tmp894[0]=Cyc_Absynpp_scope2doc((
void*)_tmp3D8->sc),Cyc_PP_cat(_tag_dyneither(_tmp894,sizeof(struct Cyc_PP_Doc*),
12))))))))))))))))))))))))));}goto _LL2BD;_LL2C2: if(*((int*)_tmp3D6)!= 0)goto
_LL2C4;_tmp3D9=((struct Cyc_Absyn_Var_d_struct*)_tmp3D6)->f1;_LL2C3: s=Cyc_Absynpp_vardecl2doc(
_tmp3D9);goto _LL2BD;_LL2C4: if(*((int*)_tmp3D6)!= 4)goto _LL2C6;_tmp3DA=((struct
Cyc_Absyn_Region_d_struct*)_tmp3D6)->f1;_tmp3DB=((struct Cyc_Absyn_Region_d_struct*)
_tmp3D6)->f2;_tmp3DC=((struct Cyc_Absyn_Region_d_struct*)_tmp3D6)->f3;_tmp3DD=((
struct Cyc_Absyn_Region_d_struct*)_tmp3D6)->f4;_LL2C5:{const char*_tmp8AB;struct
Cyc_PP_Doc*_tmp8AA[3];const char*_tmp8A9;const char*_tmp8A8;const char*_tmp8A7;
const char*_tmp8A6;const char*_tmp8A5;const char*_tmp8A4;struct Cyc_PP_Doc*_tmp8A3[8];
s=((_tmp8A3[7]=Cyc_PP_text(((_tmp8AB=";",_tag_dyneither(_tmp8AB,sizeof(char),2)))),((
_tmp8A3[6]=_tmp3DD != 0?(_tmp8AA[2]=Cyc_PP_text(((_tmp8A8=")",_tag_dyneither(
_tmp8A8,sizeof(char),2)))),((_tmp8AA[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp3DD),((_tmp8AA[0]=Cyc_PP_text(((_tmp8A9=" = open(",_tag_dyneither(_tmp8A9,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8AA,sizeof(struct Cyc_PP_Doc*),3))))))):
Cyc_PP_nil_doc(),((_tmp8A3[5]=_tmp3DC?Cyc_PP_text(((_tmp8A7=" @resetable",
_tag_dyneither(_tmp8A7,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp8A3[4]=Cyc_Absynpp_qvar2doc(
_tmp3DB->name),((_tmp8A3[3]=Cyc_PP_text(((_tmp8A6=">",_tag_dyneither(_tmp8A6,
sizeof(char),2)))),((_tmp8A3[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp3DA)),((
_tmp8A3[1]=Cyc_PP_text(((_tmp8A5="<",_tag_dyneither(_tmp8A5,sizeof(char),2)))),((
_tmp8A3[0]=Cyc_PP_text(((_tmp8A4="region",_tag_dyneither(_tmp8A4,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmp8A3,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL2BD;_LL2C6: if(*((int*)_tmp3D6)!= 5)goto _LL2C8;_tmp3DE=((struct Cyc_Absyn_Alias_d_struct*)
_tmp3D6)->f1;_tmp3DF=((struct Cyc_Absyn_Alias_d_struct*)_tmp3D6)->f2;_tmp3E0=((
struct Cyc_Absyn_Alias_d_struct*)_tmp3D6)->f3;_LL2C7:{const char*_tmp8B6;const char*
_tmp8B5;const char*_tmp8B4;const char*_tmp8B3;const char*_tmp8B2;struct Cyc_PP_Doc*
_tmp8B1[8];s=((_tmp8B1[7]=Cyc_PP_text(((_tmp8B6=";",_tag_dyneither(_tmp8B6,
sizeof(char),2)))),((_tmp8B1[6]=Cyc_Absynpp_exp2doc(_tmp3DE),((_tmp8B1[5]=Cyc_PP_text(((
_tmp8B5=" = ",_tag_dyneither(_tmp8B5,sizeof(char),4)))),((_tmp8B1[4]=Cyc_Absynpp_qvar2doc(
_tmp3E0->name),((_tmp8B1[3]=Cyc_PP_text(((_tmp8B4="> ",_tag_dyneither(_tmp8B4,
sizeof(char),3)))),((_tmp8B1[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp3DF)),((
_tmp8B1[1]=Cyc_PP_text(((_tmp8B3=" <",_tag_dyneither(_tmp8B3,sizeof(char),3)))),((
_tmp8B1[0]=Cyc_PP_text(((_tmp8B2="alias",_tag_dyneither(_tmp8B2,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp8B1,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL2BD;_LL2C8: if(*((int*)_tmp3D6)!= 7)goto _LL2CA;_tmp3E1=((struct Cyc_Absyn_Datatype_d_struct*)
_tmp3D6)->f1;_tmp3E2=*_tmp3E1;_tmp3E3=(void*)_tmp3E2.sc;_tmp3E4=_tmp3E2.name;
_tmp3E5=_tmp3E2.tvs;_tmp3E6=_tmp3E2.fields;_tmp3E7=_tmp3E2.is_extensible;_tmp3E8=
_tmp3E2.is_flat;_LL2C9: if(_tmp3E6 == 0){const char*_tmp8BF;const char*_tmp8BE;const
char*_tmp8BD;const char*_tmp8BC;struct Cyc_PP_Doc*_tmp8BB[7];s=((_tmp8BB[6]=Cyc_PP_text(((
_tmp8BF=";",_tag_dyneither(_tmp8BF,sizeof(char),2)))),((_tmp8BB[5]=Cyc_Absynpp_ktvars2doc(
_tmp3E5),((_tmp8BB[4]=_tmp3E7?Cyc_Absynpp_qvar2bolddoc(_tmp3E4): Cyc_Absynpp_typedef_name2bolddoc(
_tmp3E4),((_tmp8BB[3]=_tmp3E8?Cyc_PP_text(((_tmp8BE="__attribute__((flat)) ",
_tag_dyneither(_tmp8BE,sizeof(char),23)))): Cyc_PP_blank_doc(),((_tmp8BB[2]=Cyc_PP_text(((
_tmp8BD="datatype ",_tag_dyneither(_tmp8BD,sizeof(char),10)))),((_tmp8BB[1]=
_tmp3E7?Cyc_PP_text(((_tmp8BC="@extensible",_tag_dyneither(_tmp8BC,sizeof(char),
12)))): Cyc_PP_blank_doc(),((_tmp8BB[0]=Cyc_Absynpp_scope2doc(_tmp3E3),Cyc_PP_cat(
_tag_dyneither(_tmp8BB,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{const
char*_tmp8CA;struct Cyc_PP_Doc*_tmp8C9[2];const char*_tmp8C8;const char*_tmp8C7;
const char*_tmp8C6;struct Cyc_PP_Doc*_tmp8C5[12];s=((_tmp8C5[11]=Cyc_PP_text(((
_tmp8CA=";",_tag_dyneither(_tmp8CA,sizeof(char),2)))),((_tmp8C5[10]=Cyc_Absynpp_rb(),((
_tmp8C5[9]=Cyc_PP_line_doc(),((_tmp8C5[8]=Cyc_PP_nest(2,((_tmp8C9[1]=Cyc_Absynpp_datatypefields2doc((
struct Cyc_List_List*)_tmp3E6->v),((_tmp8C9[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp8C9,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8C5[7]=Cyc_Absynpp_lb(),((
_tmp8C5[6]=Cyc_PP_blank_doc(),((_tmp8C5[5]=Cyc_Absynpp_ktvars2doc(_tmp3E5),((
_tmp8C5[4]=_tmp3E7?Cyc_Absynpp_qvar2bolddoc(_tmp3E4): Cyc_Absynpp_typedef_name2bolddoc(
_tmp3E4),((_tmp8C5[3]=Cyc_PP_text(((_tmp8C8="datatype ",_tag_dyneither(_tmp8C8,
sizeof(char),10)))),((_tmp8C5[2]=_tmp3E8?Cyc_PP_text(((_tmp8C7="__attribute__((flat)) ",
_tag_dyneither(_tmp8C7,sizeof(char),23)))): Cyc_PP_blank_doc(),((_tmp8C5[1]=
_tmp3E7?Cyc_PP_text(((_tmp8C6="@extensible ",_tag_dyneither(_tmp8C6,sizeof(char),
13)))): Cyc_PP_blank_doc(),((_tmp8C5[0]=Cyc_Absynpp_scope2doc(_tmp3E3),Cyc_PP_cat(
_tag_dyneither(_tmp8C5,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
goto _LL2BD;_LL2CA: if(*((int*)_tmp3D6)!= 2)goto _LL2CC;_tmp3E9=((struct Cyc_Absyn_Let_d_struct*)
_tmp3D6)->f1;_tmp3EA=((struct Cyc_Absyn_Let_d_struct*)_tmp3D6)->f3;_LL2CB:{const
char*_tmp8D1;const char*_tmp8D0;const char*_tmp8CF;struct Cyc_PP_Doc*_tmp8CE[5];s=((
_tmp8CE[4]=Cyc_PP_text(((_tmp8D1=";",_tag_dyneither(_tmp8D1,sizeof(char),2)))),((
_tmp8CE[3]=Cyc_Absynpp_exp2doc(_tmp3EA),((_tmp8CE[2]=Cyc_PP_text(((_tmp8D0=" = ",
_tag_dyneither(_tmp8D0,sizeof(char),4)))),((_tmp8CE[1]=Cyc_Absynpp_pat2doc(
_tmp3E9),((_tmp8CE[0]=Cyc_PP_text(((_tmp8CF="let ",_tag_dyneither(_tmp8CF,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8CE,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL2BD;_LL2CC: if(*((int*)_tmp3D6)!= 3)goto _LL2CE;_tmp3EB=((struct Cyc_Absyn_Letv_d_struct*)
_tmp3D6)->f1;_LL2CD:{const char*_tmp8D6;const char*_tmp8D5;struct Cyc_PP_Doc*
_tmp8D4[3];s=((_tmp8D4[2]=Cyc_PP_text(((_tmp8D6=";",_tag_dyneither(_tmp8D6,
sizeof(char),2)))),((_tmp8D4[1]=Cyc_Absynpp_ids2doc(_tmp3EB),((_tmp8D4[0]=Cyc_PP_text(((
_tmp8D5="let ",_tag_dyneither(_tmp8D5,sizeof(char),5)))),Cyc_PP_cat(
_tag_dyneither(_tmp8D4,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL2BD;_LL2CE:
if(*((int*)_tmp3D6)!= 8)goto _LL2D0;_tmp3EC=((struct Cyc_Absyn_Enum_d_struct*)
_tmp3D6)->f1;_tmp3ED=*_tmp3EC;_tmp3EE=(void*)_tmp3ED.sc;_tmp3EF=_tmp3ED.name;
_tmp3F0=_tmp3ED.fields;_LL2CF: if(_tmp3F0 == 0){const char*_tmp8DB;const char*
_tmp8DA;struct Cyc_PP_Doc*_tmp8D9[4];s=((_tmp8D9[3]=Cyc_PP_text(((_tmp8DB=";",
_tag_dyneither(_tmp8DB,sizeof(char),2)))),((_tmp8D9[2]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp3EF),((_tmp8D9[1]=Cyc_PP_text(((_tmp8DA="enum ",_tag_dyneither(_tmp8DA,
sizeof(char),6)))),((_tmp8D9[0]=Cyc_Absynpp_scope2doc(_tmp3EE),Cyc_PP_cat(
_tag_dyneither(_tmp8D9,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{const char*
_tmp8E2;struct Cyc_PP_Doc*_tmp8E1[2];const char*_tmp8E0;struct Cyc_PP_Doc*_tmp8DF[9];
s=((_tmp8DF[8]=Cyc_PP_text(((_tmp8E2=";",_tag_dyneither(_tmp8E2,sizeof(char),2)))),((
_tmp8DF[7]=Cyc_Absynpp_rb(),((_tmp8DF[6]=Cyc_PP_line_doc(),((_tmp8DF[5]=Cyc_PP_nest(
2,((_tmp8E1[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp3F0->v),((
_tmp8E1[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8E1,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp8DF[4]=Cyc_Absynpp_lb(),((_tmp8DF[3]=Cyc_PP_blank_doc(),((_tmp8DF[
2]=Cyc_Absynpp_qvar2bolddoc(_tmp3EF),((_tmp8DF[1]=Cyc_PP_text(((_tmp8E0="enum ",
_tag_dyneither(_tmp8E0,sizeof(char),6)))),((_tmp8DF[0]=Cyc_Absynpp_scope2doc(
_tmp3EE),Cyc_PP_cat(_tag_dyneither(_tmp8DF,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
goto _LL2BD;_LL2D0: if(*((int*)_tmp3D6)!= 9)goto _LL2D2;_tmp3F1=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp3D6)->f1;_LL2D1: {void*t;if(_tmp3F1->defn != 0)t=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3F1->defn))->v;else{t=Cyc_Absyn_new_evar(_tmp3F1->kind,0);}{
const char*_tmp8EC;struct Cyc_Core_Opt*_tmp8EB;struct Cyc_PP_Doc*_tmp8EA[2];const
char*_tmp8E9;struct Cyc_PP_Doc*_tmp8E8[4];s=((_tmp8E8[3]=Cyc_PP_text(((_tmp8EC=";",
_tag_dyneither(_tmp8EC,sizeof(char),2)))),((_tmp8E8[2]=Cyc_Absynpp_atts2doc(
_tmp3F1->atts),((_tmp8E8[1]=Cyc_Absynpp_tqtd2doc(_tmp3F1->tq,t,((_tmp8EB=
_cycalloc(sizeof(*_tmp8EB)),((_tmp8EB->v=((_tmp8EA[1]=Cyc_Absynpp_tvars2doc(
_tmp3F1->tvs),((_tmp8EA[0]=Cyc_Absynpp_typedef_name2bolddoc(_tmp3F1->name),Cyc_PP_cat(
_tag_dyneither(_tmp8EA,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp8EB))))),((_tmp8E8[
0]=Cyc_PP_text(((_tmp8E9="typedef ",_tag_dyneither(_tmp8E9,sizeof(char),9)))),
Cyc_PP_cat(_tag_dyneither(_tmp8E8,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL2BD;}_LL2D2: if(*((int*)_tmp3D6)!= 10)goto _LL2D4;_tmp3F2=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp3D6)->f1;_tmp3F3=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3D6)->f2;_LL2D3:
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp3F2);{const
char*_tmp8F1;const char*_tmp8F0;struct Cyc_PP_Doc*_tmp8EF[8];s=((_tmp8EF[7]=Cyc_Absynpp_rb(),((
_tmp8EF[6]=Cyc_PP_line_doc(),((_tmp8EF[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp8F1="",_tag_dyneither(_tmp8F1,sizeof(char),1))),
_tmp3F3),((_tmp8EF[4]=Cyc_PP_line_doc(),((_tmp8EF[3]=Cyc_Absynpp_lb(),((_tmp8EF[
2]=Cyc_PP_blank_doc(),((_tmp8EF[1]=Cyc_PP_textptr(_tmp3F2),((_tmp8EF[0]=Cyc_PP_text(((
_tmp8F0="namespace ",_tag_dyneither(_tmp8F0,sizeof(char),11)))),Cyc_PP_cat(
_tag_dyneither(_tmp8EF,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}if(Cyc_Absynpp_use_curr_namespace)
Cyc_Absynpp_curr_namespace_drop();goto _LL2BD;_LL2D4: if(*((int*)_tmp3D6)!= 11)
goto _LL2D6;_tmp3F4=((struct Cyc_Absyn_Using_d_struct*)_tmp3D6)->f1;_tmp3F5=((
struct Cyc_Absyn_Using_d_struct*)_tmp3D6)->f2;_LL2D5: if(Cyc_Absynpp_print_using_stmts){
const char*_tmp8F6;const char*_tmp8F5;struct Cyc_PP_Doc*_tmp8F4[8];s=((_tmp8F4[7]=
Cyc_Absynpp_rb(),((_tmp8F4[6]=Cyc_PP_line_doc(),((_tmp8F4[5]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmp8F6="",_tag_dyneither(_tmp8F6,
sizeof(char),1))),_tmp3F5),((_tmp8F4[4]=Cyc_PP_line_doc(),((_tmp8F4[3]=Cyc_Absynpp_lb(),((
_tmp8F4[2]=Cyc_PP_blank_doc(),((_tmp8F4[1]=Cyc_Absynpp_qvar2doc(_tmp3F4),((
_tmp8F4[0]=Cyc_PP_text(((_tmp8F5="using ",_tag_dyneither(_tmp8F5,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmp8F4,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
else{const char*_tmp901;const char*_tmp900;const char*_tmp8FF;const char*_tmp8FE;
const char*_tmp8FD;struct Cyc_PP_Doc*_tmp8FC[11];s=((_tmp8FC[10]=Cyc_PP_text(((
_tmp901=" */",_tag_dyneither(_tmp901,sizeof(char),4)))),((_tmp8FC[9]=Cyc_Absynpp_rb(),((
_tmp8FC[8]=Cyc_PP_text(((_tmp900="/* ",_tag_dyneither(_tmp900,sizeof(char),4)))),((
_tmp8FC[7]=Cyc_PP_line_doc(),((_tmp8FC[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp8FF="",_tag_dyneither(_tmp8FF,sizeof(char),1))),
_tmp3F5),((_tmp8FC[5]=Cyc_PP_line_doc(),((_tmp8FC[4]=Cyc_PP_text(((_tmp8FE=" */",
_tag_dyneither(_tmp8FE,sizeof(char),4)))),((_tmp8FC[3]=Cyc_Absynpp_lb(),((
_tmp8FC[2]=Cyc_PP_blank_doc(),((_tmp8FC[1]=Cyc_Absynpp_qvar2doc(_tmp3F4),((
_tmp8FC[0]=Cyc_PP_text(((_tmp8FD="/* using ",_tag_dyneither(_tmp8FD,sizeof(char),
10)))),Cyc_PP_cat(_tag_dyneither(_tmp8FC,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL2BD;_LL2D6: if(*((int*)_tmp3D6)!= 12)goto _LL2D8;_tmp3F6=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp3D6)->f1;_LL2D7: if(Cyc_Absynpp_print_externC_stmts){const char*_tmp906;const
char*_tmp905;struct Cyc_PP_Doc*_tmp904[6];s=((_tmp904[5]=Cyc_Absynpp_rb(),((
_tmp904[4]=Cyc_PP_line_doc(),((_tmp904[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp906="",_tag_dyneither(_tmp906,sizeof(char),1))),
_tmp3F6),((_tmp904[2]=Cyc_PP_line_doc(),((_tmp904[1]=Cyc_Absynpp_lb(),((_tmp904[
0]=Cyc_PP_text(((_tmp905="extern \"C\" ",_tag_dyneither(_tmp905,sizeof(char),12)))),
Cyc_PP_cat(_tag_dyneither(_tmp904,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmp911;const char*_tmp910;const char*_tmp90F;const char*_tmp90E;
const char*_tmp90D;struct Cyc_PP_Doc*_tmp90C[9];s=((_tmp90C[8]=Cyc_PP_text(((
_tmp911=" */",_tag_dyneither(_tmp911,sizeof(char),4)))),((_tmp90C[7]=Cyc_Absynpp_rb(),((
_tmp90C[6]=Cyc_PP_text(((_tmp910="/* ",_tag_dyneither(_tmp910,sizeof(char),4)))),((
_tmp90C[5]=Cyc_PP_line_doc(),((_tmp90C[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp90F="",_tag_dyneither(_tmp90F,sizeof(char),1))),
_tmp3F6),((_tmp90C[3]=Cyc_PP_line_doc(),((_tmp90C[2]=Cyc_PP_text(((_tmp90E=" */",
_tag_dyneither(_tmp90E,sizeof(char),4)))),((_tmp90C[1]=Cyc_Absynpp_lb(),((
_tmp90C[0]=Cyc_PP_text(((_tmp90D="/* extern \"C\" ",_tag_dyneither(_tmp90D,
sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp90C,sizeof(struct Cyc_PP_Doc*),
9))))))))))))))))))));}goto _LL2BD;_LL2D8: if(*((int*)_tmp3D6)!= 13)goto _LL2DA;
_tmp3F7=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp3D6)->f1;_tmp3F8=((struct
Cyc_Absyn_ExternCinclude_d_struct*)_tmp3D6)->f2;_LL2D9: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp3F8 != 0){const char*_tmp916;const char*_tmp915;
struct Cyc_PP_Doc*_tmp914[7];exs_doc=((_tmp914[6]=Cyc_Absynpp_rb(),((_tmp914[5]=
Cyc_PP_line_doc(),((_tmp914[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple11*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_export2doc,((_tmp916=",",_tag_dyneither(_tmp916,sizeof(char),2))),
_tmp3F8),((_tmp914[3]=Cyc_PP_line_doc(),((_tmp914[2]=Cyc_Absynpp_lb(),((_tmp914[
1]=Cyc_PP_text(((_tmp915=" export ",_tag_dyneither(_tmp915,sizeof(char),9)))),((
_tmp914[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmp914,sizeof(struct Cyc_PP_Doc*),
7))))))))))))))));}else{exs_doc=Cyc_Absynpp_rb();}{const char*_tmp91B;const char*
_tmp91A;struct Cyc_PP_Doc*_tmp919[6];s=((_tmp919[5]=exs_doc,((_tmp919[4]=Cyc_PP_line_doc(),((
_tmp919[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmp91B="",_tag_dyneither(_tmp91B,sizeof(char),1))),_tmp3F7),((_tmp919[2]=Cyc_PP_line_doc(),((
_tmp919[1]=Cyc_Absynpp_lb(),((_tmp919[0]=Cyc_PP_text(((_tmp91A="extern \"C include\" ",
_tag_dyneither(_tmp91A,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmp919,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}}else{const char*_tmp926;const char*
_tmp925;const char*_tmp924;const char*_tmp923;const char*_tmp922;struct Cyc_PP_Doc*
_tmp921[9];s=((_tmp921[8]=Cyc_PP_text(((_tmp926=" */",_tag_dyneither(_tmp926,
sizeof(char),4)))),((_tmp921[7]=Cyc_Absynpp_rb(),((_tmp921[6]=Cyc_PP_text(((
_tmp925="/* ",_tag_dyneither(_tmp925,sizeof(char),4)))),((_tmp921[5]=Cyc_PP_line_doc(),((
_tmp921[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmp924="",_tag_dyneither(_tmp924,sizeof(char),1))),_tmp3F7),((_tmp921[3]=Cyc_PP_line_doc(),((
_tmp921[2]=Cyc_PP_text(((_tmp923=" */",_tag_dyneither(_tmp923,sizeof(char),4)))),((
_tmp921[1]=Cyc_Absynpp_lb(),((_tmp921[0]=Cyc_PP_text(((_tmp922="/* extern \"C include\" ",
_tag_dyneither(_tmp922,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmp921,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL2BD;_LL2DA: if((int)
_tmp3D6 != 0)goto _LL2DC;_LL2DB:{const char*_tmp929;struct Cyc_PP_Doc*_tmp928[2];s=((
_tmp928[1]=Cyc_Absynpp_lb(),((_tmp928[0]=Cyc_PP_text(((_tmp929="__cyclone_port_on__;",
_tag_dyneither(_tmp929,sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmp928,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL2BD;_LL2DC: if((int)_tmp3D6 != 1)goto
_LL2BD;_LL2DD:{const char*_tmp92C;struct Cyc_PP_Doc*_tmp92B[2];s=((_tmp92B[1]=Cyc_Absynpp_lb(),((
_tmp92B[0]=Cyc_PP_text(((_tmp92C="__cyclone_port_off__;",_tag_dyneither(_tmp92C,
sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmp92B,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL2BD;_LL2BD:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
void*sc);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*sc){if(Cyc_Absynpp_print_for_cycdoc)
return Cyc_PP_nil_doc();{void*_tmp460=sc;_LL2EE: if((int)_tmp460 != 0)goto _LL2F0;
_LL2EF: {const char*_tmp92D;return Cyc_PP_text(((_tmp92D="static ",_tag_dyneither(
_tmp92D,sizeof(char),8))));}_LL2F0: if((int)_tmp460 != 2)goto _LL2F2;_LL2F1: return
Cyc_PP_nil_doc();_LL2F2: if((int)_tmp460 != 3)goto _LL2F4;_LL2F3: {const char*
_tmp92E;return Cyc_PP_text(((_tmp92E="extern ",_tag_dyneither(_tmp92E,sizeof(char),
8))));}_LL2F4: if((int)_tmp460 != 4)goto _LL2F6;_LL2F5: {const char*_tmp92F;return
Cyc_PP_text(((_tmp92F="extern \"C\" ",_tag_dyneither(_tmp92F,sizeof(char),12))));}
_LL2F6: if((int)_tmp460 != 1)goto _LL2F8;_LL2F7: {const char*_tmp930;return Cyc_PP_text(((
_tmp930="abstract ",_tag_dyneither(_tmp930,sizeof(char),10))));}_LL2F8: if((int)
_tmp460 != 5)goto _LL2ED;_LL2F9: {const char*_tmp931;return Cyc_PP_text(((_tmp931="register ",
_tag_dyneither(_tmp931,sizeof(char),10))));}_LL2ED:;}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t);int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){void*_tmp466=t;struct
Cyc_Absyn_Tvar*_tmp467;struct Cyc_List_List*_tmp468;_LL2FB: if(_tmp466 <= (void*)4)
goto _LL2FF;if(*((int*)_tmp466)!= 1)goto _LL2FD;_tmp467=((struct Cyc_Absyn_VarType_struct*)
_tmp466)->f1;_LL2FC: return Cyc_Tcutil_is_temp_tvar(_tmp467);_LL2FD: if(*((int*)
_tmp466)!= 20)goto _LL2FF;_tmp468=((struct Cyc_Absyn_JoinEff_struct*)_tmp466)->f1;
_LL2FE: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp468);
_LL2FF:;_LL300: return 0;_LL2FA:;}int Cyc_Absynpp_is_anon_aggrtype(void*t);int Cyc_Absynpp_is_anon_aggrtype(
void*t){void*_tmp469=t;void**_tmp46A;void*_tmp46B;_LL302: if(_tmp469 <= (void*)4)
goto _LL308;if(*((int*)_tmp469)!= 11)goto _LL304;_LL303: return 1;_LL304: if(*((int*)
_tmp469)!= 13)goto _LL306;_LL305: return 1;_LL306: if(*((int*)_tmp469)!= 16)goto
_LL308;_tmp46A=((struct Cyc_Absyn_TypedefType_struct*)_tmp469)->f4;if(_tmp46A == 0)
goto _LL308;_tmp46B=*_tmp46A;_LL307: return Cyc_Absynpp_is_anon_aggrtype(_tmp46B);
_LL308:;_LL309: return 0;_LL301:;}static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms);static struct Cyc_List_List*
Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*
tms){if(tms != 0  && tms->tl != 0){struct _tuple5 _tmp932;struct _tuple5 _tmp46D=(
_tmp932.f1=(void*)tms->hd,((_tmp932.f2=(void*)((struct Cyc_List_List*)_check_null(
tms->tl))->hd,_tmp932)));void*_tmp46E;void*_tmp46F;_LL30B: _tmp46E=_tmp46D.f1;if(*((
int*)_tmp46E)!= 2)goto _LL30D;_tmp46F=_tmp46D.f2;if(*((int*)_tmp46F)!= 3)goto
_LL30D;_LL30C: {struct Cyc_List_List*_tmp935;struct Cyc_List_List*_tmp934;return(
_tmp934=_region_malloc(r,sizeof(*_tmp934)),((_tmp934->hd=(void*)((void*)tms->hd),((
_tmp934->tl=((_tmp935=_region_malloc(r,sizeof(*_tmp935)),((_tmp935->hd=(void*)((
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd),((_tmp935->tl=Cyc_Absynpp_bubble_attributes(
r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmp935)))))),_tmp934)))));}
_LL30D:;_LL30E: {struct Cyc_List_List*_tmp936;return(_tmp936=_region_malloc(r,
sizeof(*_tmp936)),((_tmp936->hd=(void*)atts,((_tmp936->tl=tms,_tmp936)))));}
_LL30A:;}else{struct Cyc_List_List*_tmp937;return(_tmp937=_region_malloc(r,
sizeof(*_tmp937)),((_tmp937->hd=(void*)atts,((_tmp937->tl=tms,_tmp937)))));}}
struct _tuple7 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,
void*t);struct _tuple7 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t){void*_tmp474=t;struct Cyc_Absyn_ArrayInfo _tmp475;void*_tmp476;struct
Cyc_Absyn_Tqual _tmp477;struct Cyc_Absyn_Exp*_tmp478;struct Cyc_Absyn_Conref*
_tmp479;struct Cyc_Position_Segment*_tmp47A;struct Cyc_Absyn_PtrInfo _tmp47B;void*
_tmp47C;struct Cyc_Absyn_Tqual _tmp47D;struct Cyc_Absyn_PtrAtts _tmp47E;struct Cyc_Absyn_FnInfo
_tmp47F;struct Cyc_List_List*_tmp480;struct Cyc_Core_Opt*_tmp481;void*_tmp482;
struct Cyc_List_List*_tmp483;int _tmp484;struct Cyc_Absyn_VarargInfo*_tmp485;struct
Cyc_List_List*_tmp486;struct Cyc_List_List*_tmp487;struct Cyc_Core_Opt*_tmp488;
struct Cyc_Core_Opt*_tmp489;int _tmp48A;struct _tuple0*_tmp48B;struct Cyc_List_List*
_tmp48C;void**_tmp48D;_LL310: if(_tmp474 <= (void*)4)goto _LL31A;if(*((int*)_tmp474)
!= 7)goto _LL312;_tmp475=((struct Cyc_Absyn_ArrayType_struct*)_tmp474)->f1;_tmp476=(
void*)_tmp475.elt_type;_tmp477=_tmp475.tq;_tmp478=_tmp475.num_elts;_tmp479=
_tmp475.zero_term;_tmp47A=_tmp475.zt_loc;_LL311: {struct Cyc_Absyn_Tqual _tmp48F;
void*_tmp490;struct Cyc_List_List*_tmp491;struct _tuple7 _tmp48E=Cyc_Absynpp_to_tms(
r,_tmp477,_tmp476);_tmp48F=_tmp48E.f1;_tmp490=_tmp48E.f2;_tmp491=_tmp48E.f3;{
void*tm;if(_tmp478 == 0){struct Cyc_Absyn_Carray_mod_struct _tmp93A;struct Cyc_Absyn_Carray_mod_struct*
_tmp939;tm=(void*)((_tmp939=_region_malloc(r,sizeof(*_tmp939)),((_tmp939[0]=((
_tmp93A.tag=0,((_tmp93A.f1=_tmp479,((_tmp93A.f2=_tmp47A,_tmp93A)))))),_tmp939))));}
else{struct Cyc_Absyn_ConstArray_mod_struct _tmp93D;struct Cyc_Absyn_ConstArray_mod_struct*
_tmp93C;tm=(void*)((_tmp93C=_region_malloc(r,sizeof(*_tmp93C)),((_tmp93C[0]=((
_tmp93D.tag=1,((_tmp93D.f1=(struct Cyc_Absyn_Exp*)_tmp478,((_tmp93D.f2=_tmp479,((
_tmp93D.f3=_tmp47A,_tmp93D)))))))),_tmp93C))));}{struct Cyc_List_List*_tmp940;
struct _tuple7 _tmp93F;return(_tmp93F.f1=_tmp48F,((_tmp93F.f2=_tmp490,((_tmp93F.f3=((
_tmp940=_region_malloc(r,sizeof(*_tmp940)),((_tmp940->hd=(void*)tm,((_tmp940->tl=
_tmp491,_tmp940)))))),_tmp93F)))));}}}_LL312: if(*((int*)_tmp474)!= 4)goto _LL314;
_tmp47B=((struct Cyc_Absyn_PointerType_struct*)_tmp474)->f1;_tmp47C=(void*)
_tmp47B.elt_typ;_tmp47D=_tmp47B.elt_tq;_tmp47E=_tmp47B.ptr_atts;_LL313: {struct
Cyc_Absyn_Tqual _tmp499;void*_tmp49A;struct Cyc_List_List*_tmp49B;struct _tuple7
_tmp498=Cyc_Absynpp_to_tms(r,_tmp47D,_tmp47C);_tmp499=_tmp498.f1;_tmp49A=_tmp498.f2;
_tmp49B=_tmp498.f3;{struct Cyc_Absyn_Pointer_mod_struct*_tmp946;struct Cyc_Absyn_Pointer_mod_struct
_tmp945;struct Cyc_List_List*_tmp944;_tmp49B=((_tmp944=_region_malloc(r,sizeof(*
_tmp944)),((_tmp944->hd=(void*)((void*)((_tmp946=_region_malloc(r,sizeof(*
_tmp946)),((_tmp946[0]=((_tmp945.tag=2,((_tmp945.f1=_tmp47E,((_tmp945.f2=tq,
_tmp945)))))),_tmp946))))),((_tmp944->tl=_tmp49B,_tmp944))))));}{struct _tuple7
_tmp947;return(_tmp947.f1=_tmp499,((_tmp947.f2=_tmp49A,((_tmp947.f3=_tmp49B,
_tmp947)))));}}_LL314: if(*((int*)_tmp474)!= 8)goto _LL316;_tmp47F=((struct Cyc_Absyn_FnType_struct*)
_tmp474)->f1;_tmp480=_tmp47F.tvars;_tmp481=_tmp47F.effect;_tmp482=(void*)_tmp47F.ret_typ;
_tmp483=_tmp47F.args;_tmp484=_tmp47F.c_varargs;_tmp485=_tmp47F.cyc_varargs;
_tmp486=_tmp47F.rgn_po;_tmp487=_tmp47F.attributes;_LL315: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp481 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp481->v)){
_tmp481=0;_tmp480=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp480);}{struct Cyc_Absyn_Tqual _tmp4A1;void*_tmp4A2;struct Cyc_List_List*_tmp4A3;
struct _tuple7 _tmp4A0=Cyc_Absynpp_to_tms(r,Cyc_Absyn_empty_tqual(0),_tmp482);
_tmp4A1=_tmp4A0.f1;_tmp4A2=_tmp4A0.f2;_tmp4A3=_tmp4A0.f3;{struct Cyc_List_List*
tms=_tmp4A3;{void*_tmp4A4=Cyc_Cyclone_c_compiler;_LL31D: if((int)_tmp4A4 != 0)goto
_LL31F;_LL31E: if(_tmp487 != 0){struct Cyc_Absyn_Attributes_mod_struct _tmp94A;
struct Cyc_Absyn_Attributes_mod_struct*_tmp949;tms=Cyc_Absynpp_bubble_attributes(
r,(void*)((_tmp949=_region_malloc(r,sizeof(*_tmp949)),((_tmp949[0]=((_tmp94A.tag=
5,((_tmp94A.f1=0,((_tmp94A.f2=_tmp487,_tmp94A)))))),_tmp949)))),tms);}{struct Cyc_Absyn_Function_mod_struct*
_tmp959;struct Cyc_Absyn_WithTypes_struct*_tmp958;struct Cyc_Absyn_WithTypes_struct
_tmp957;struct Cyc_Absyn_Function_mod_struct _tmp956;struct Cyc_List_List*_tmp955;
tms=((_tmp955=_region_malloc(r,sizeof(*_tmp955)),((_tmp955->hd=(void*)((void*)((
_tmp959=_region_malloc(r,sizeof(*_tmp959)),((_tmp959[0]=((_tmp956.tag=3,((
_tmp956.f1=(void*)((void*)((_tmp958=_region_malloc(r,sizeof(*_tmp958)),((_tmp958[
0]=((_tmp957.tag=1,((_tmp957.f1=_tmp483,((_tmp957.f2=_tmp484,((_tmp957.f3=
_tmp485,((_tmp957.f4=_tmp481,((_tmp957.f5=_tmp486,_tmp957)))))))))))),_tmp958))))),
_tmp956)))),_tmp959))))),((_tmp955->tl=tms,_tmp955))))));}goto _LL31C;_LL31F: if((
int)_tmp4A4 != 1)goto _LL31C;_LL320:{struct Cyc_Absyn_Function_mod_struct*_tmp968;
struct Cyc_Absyn_WithTypes_struct*_tmp967;struct Cyc_Absyn_WithTypes_struct _tmp966;
struct Cyc_Absyn_Function_mod_struct _tmp965;struct Cyc_List_List*_tmp964;tms=((
_tmp964=_region_malloc(r,sizeof(*_tmp964)),((_tmp964->hd=(void*)((void*)((
_tmp968=_region_malloc(r,sizeof(*_tmp968)),((_tmp968[0]=((_tmp965.tag=3,((
_tmp965.f1=(void*)((void*)((_tmp967=_region_malloc(r,sizeof(*_tmp967)),((_tmp967[
0]=((_tmp966.tag=1,((_tmp966.f1=_tmp483,((_tmp966.f2=_tmp484,((_tmp966.f3=
_tmp485,((_tmp966.f4=_tmp481,((_tmp966.f5=_tmp486,_tmp966)))))))))))),_tmp967))))),
_tmp965)))),_tmp968))))),((_tmp964->tl=tms,_tmp964))))));}for(0;_tmp487 != 0;
_tmp487=_tmp487->tl){void*_tmp4B1=(void*)_tmp487->hd;_LL322: if((int)_tmp4B1 != 0)
goto _LL324;_LL323: goto _LL325;_LL324: if((int)_tmp4B1 != 1)goto _LL326;_LL325: goto
_LL327;_LL326: if((int)_tmp4B1 != 2)goto _LL328;_LL327:{struct Cyc_Absyn_Attributes_mod_struct*
_tmp972;struct Cyc_List_List*_tmp971;struct Cyc_Absyn_Attributes_mod_struct _tmp970;
struct Cyc_List_List*_tmp96F;tms=((_tmp96F=_region_malloc(r,sizeof(*_tmp96F)),((
_tmp96F->hd=(void*)((void*)((_tmp972=_region_malloc(r,sizeof(*_tmp972)),((
_tmp972[0]=((_tmp970.tag=5,((_tmp970.f1=0,((_tmp970.f2=((_tmp971=_cycalloc(
sizeof(*_tmp971)),((_tmp971->hd=(void*)((void*)_tmp487->hd),((_tmp971->tl=0,
_tmp971)))))),_tmp970)))))),_tmp972))))),((_tmp96F->tl=tms,_tmp96F))))));}goto
AfterAtts;_LL328:;_LL329: goto _LL321;_LL321:;}goto _LL31C;_LL31C:;}AfterAtts: if(
_tmp480 != 0){struct Cyc_Absyn_TypeParams_mod_struct*_tmp978;struct Cyc_Absyn_TypeParams_mod_struct
_tmp977;struct Cyc_List_List*_tmp976;tms=((_tmp976=_region_malloc(r,sizeof(*
_tmp976)),((_tmp976->hd=(void*)((void*)((_tmp978=_region_malloc(r,sizeof(*
_tmp978)),((_tmp978[0]=((_tmp977.tag=4,((_tmp977.f1=_tmp480,((_tmp977.f2=0,((
_tmp977.f3=1,_tmp977)))))))),_tmp978))))),((_tmp976->tl=tms,_tmp976))))));}{
struct _tuple7 _tmp979;return(_tmp979.f1=_tmp4A1,((_tmp979.f2=_tmp4A2,((_tmp979.f3=
tms,_tmp979)))));}}}_LL316: if(*((int*)_tmp474)!= 0)goto _LL318;_tmp488=((struct
Cyc_Absyn_Evar_struct*)_tmp474)->f1;_tmp489=((struct Cyc_Absyn_Evar_struct*)
_tmp474)->f2;_tmp48A=((struct Cyc_Absyn_Evar_struct*)_tmp474)->f3;_LL317: if(
_tmp489 == 0){struct _tuple7 _tmp97A;return(_tmp97A.f1=tq,((_tmp97A.f2=t,((_tmp97A.f3=
0,_tmp97A)))));}else{return Cyc_Absynpp_to_tms(r,tq,(void*)_tmp489->v);}_LL318:
if(*((int*)_tmp474)!= 16)goto _LL31A;_tmp48B=((struct Cyc_Absyn_TypedefType_struct*)
_tmp474)->f1;_tmp48C=((struct Cyc_Absyn_TypedefType_struct*)_tmp474)->f2;_tmp48D=((
struct Cyc_Absyn_TypedefType_struct*)_tmp474)->f4;_LL319: if((_tmp48D == 0  || !Cyc_Absynpp_expand_typedefs)
 || Cyc_Absynpp_is_anon_aggrtype(*_tmp48D)){struct _tuple7 _tmp97B;return(_tmp97B.f1=
tq,((_tmp97B.f2=t,((_tmp97B.f3=0,_tmp97B)))));}else{return Cyc_Absynpp_to_tms(r,
tq,*_tmp48D);}_LL31A:;_LL31B: {struct _tuple7 _tmp97C;return(_tmp97C.f1=tq,((
_tmp97C.f2=t,((_tmp97C.f3=0,_tmp97C)))));}_LL30F:;}static int Cyc_Absynpp_is_char_ptr(
void*t);static int Cyc_Absynpp_is_char_ptr(void*t){void*_tmp4BD=t;struct Cyc_Core_Opt*
_tmp4BE;struct Cyc_Core_Opt _tmp4BF;void*_tmp4C0;struct Cyc_Absyn_PtrInfo _tmp4C1;
void*_tmp4C2;_LL32B: if(_tmp4BD <= (void*)4)goto _LL32F;if(*((int*)_tmp4BD)!= 0)
goto _LL32D;_tmp4BE=((struct Cyc_Absyn_Evar_struct*)_tmp4BD)->f2;if(_tmp4BE == 0)
goto _LL32D;_tmp4BF=*_tmp4BE;_tmp4C0=(void*)_tmp4BF.v;_LL32C: return Cyc_Absynpp_is_char_ptr(
_tmp4C0);_LL32D: if(*((int*)_tmp4BD)!= 4)goto _LL32F;_tmp4C1=((struct Cyc_Absyn_PointerType_struct*)
_tmp4BD)->f1;_tmp4C2=(void*)_tmp4C1.elt_typ;_LL32E: L: {void*_tmp4C3=_tmp4C2;
struct Cyc_Core_Opt*_tmp4C4;struct Cyc_Core_Opt _tmp4C5;void*_tmp4C6;void**_tmp4C7;
void*_tmp4C8;void*_tmp4C9;_LL332: if(_tmp4C3 <= (void*)4)goto _LL338;if(*((int*)
_tmp4C3)!= 0)goto _LL334;_tmp4C4=((struct Cyc_Absyn_Evar_struct*)_tmp4C3)->f2;if(
_tmp4C4 == 0)goto _LL334;_tmp4C5=*_tmp4C4;_tmp4C6=(void*)_tmp4C5.v;_LL333: _tmp4C2=
_tmp4C6;goto L;_LL334: if(*((int*)_tmp4C3)!= 16)goto _LL336;_tmp4C7=((struct Cyc_Absyn_TypedefType_struct*)
_tmp4C3)->f4;if(_tmp4C7 == 0)goto _LL336;_tmp4C8=*_tmp4C7;_LL335: _tmp4C2=_tmp4C8;
goto L;_LL336: if(*((int*)_tmp4C3)!= 5)goto _LL338;_tmp4C9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4C3)->f2;if((int)_tmp4C9 != 0)goto _LL338;_LL337: return 1;_LL338:;_LL339: return
0;_LL331:;}_LL32F:;_LL330: return 0;_LL32A:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){struct _RegionHandle
_tmp4CA=_new_region("temp");struct _RegionHandle*temp=& _tmp4CA;_push_region(temp);{
struct Cyc_Absyn_Tqual _tmp4CC;void*_tmp4CD;struct Cyc_List_List*_tmp4CE;struct
_tuple7 _tmp4CB=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp4CC=_tmp4CB.f1;_tmp4CD=
_tmp4CB.f2;_tmp4CE=_tmp4CB.f3;_tmp4CE=Cyc_List_imp_rev(_tmp4CE);if(_tmp4CE == 0
 && dopt == 0){struct Cyc_PP_Doc*_tmp97D[2];struct Cyc_PP_Doc*_tmp4D0=(_tmp97D[1]=
Cyc_Absynpp_ntyp2doc(_tmp4CD),((_tmp97D[0]=Cyc_Absynpp_tqual2doc(_tmp4CC),Cyc_PP_cat(
_tag_dyneither(_tmp97D,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return
_tmp4D0;}else{const char*_tmp980;struct Cyc_PP_Doc*_tmp97F[4];struct Cyc_PP_Doc*
_tmp4D3=(_tmp97F[3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?
Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp4CE),((_tmp97F[2]=Cyc_PP_text(((
_tmp980=" ",_tag_dyneither(_tmp980,sizeof(char),2)))),((_tmp97F[1]=Cyc_Absynpp_ntyp2doc(
_tmp4CD),((_tmp97F[0]=Cyc_Absynpp_tqual2doc(_tmp4CC),Cyc_PP_cat(_tag_dyneither(
_tmp97F,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp4D3;}};
_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*
f);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){void*
_tmp4D4=Cyc_Cyclone_c_compiler;_LL33B: if((int)_tmp4D4 != 0)goto _LL33D;_LL33C: if(f->width
!= 0){const char*_tmp987;const char*_tmp986;struct Cyc_Core_Opt*_tmp985;struct Cyc_PP_Doc*
_tmp984[5];return(_tmp984[4]=Cyc_PP_text(((_tmp987=";",_tag_dyneither(_tmp987,
sizeof(char),2)))),((_tmp984[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmp984[2]=
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmp984[1]=
Cyc_PP_text(((_tmp986=":",_tag_dyneither(_tmp986,sizeof(char),2)))),((_tmp984[0]=
Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,((_tmp985=_cycalloc(sizeof(*_tmp985)),((
_tmp985->v=Cyc_PP_textptr(f->name),_tmp985))))),Cyc_PP_cat(_tag_dyneither(
_tmp984,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{const char*_tmp98C;struct Cyc_Core_Opt*
_tmp98B;struct Cyc_PP_Doc*_tmp98A[3];return(_tmp98A[2]=Cyc_PP_text(((_tmp98C=";",
_tag_dyneither(_tmp98C,sizeof(char),2)))),((_tmp98A[1]=Cyc_Absynpp_atts2doc(f->attributes),((
_tmp98A[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,((_tmp98B=_cycalloc(sizeof(*
_tmp98B)),((_tmp98B->v=Cyc_PP_textptr(f->name),_tmp98B))))),Cyc_PP_cat(
_tag_dyneither(_tmp98A,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL33D: if((int)
_tmp4D4 != 1)goto _LL33A;_LL33E: if(f->width != 0){const char*_tmp993;const char*
_tmp992;struct Cyc_Core_Opt*_tmp991;struct Cyc_PP_Doc*_tmp990[5];return(_tmp990[4]=
Cyc_PP_text(((_tmp993=";",_tag_dyneither(_tmp993,sizeof(char),2)))),((_tmp990[3]=
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmp990[2]=
Cyc_PP_text(((_tmp992=":",_tag_dyneither(_tmp992,sizeof(char),2)))),((_tmp990[1]=
Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,((_tmp991=_cycalloc(sizeof(*_tmp991)),((
_tmp991->v=Cyc_PP_textptr(f->name),_tmp991))))),((_tmp990[0]=Cyc_Absynpp_atts2doc(
f->attributes),Cyc_PP_cat(_tag_dyneither(_tmp990,sizeof(struct Cyc_PP_Doc*),5)))))))))));}
else{const char*_tmp998;struct Cyc_Core_Opt*_tmp997;struct Cyc_PP_Doc*_tmp996[3];
return(_tmp996[2]=Cyc_PP_text(((_tmp998=";",_tag_dyneither(_tmp998,sizeof(char),
2)))),((_tmp996[1]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,((_tmp997=_cycalloc(
sizeof(*_tmp997)),((_tmp997->v=Cyc_PP_textptr(f->name),_tmp997))))),((_tmp996[0]=
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmp996,sizeof(
struct Cyc_PP_Doc*),3)))))));}_LL33A:;}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct
Cyc_List_List*fields){const char*_tmp999;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmp999="",_tag_dyneither(_tmp999,
sizeof(char),1))),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct
Cyc_Absyn_Datatypefield*f);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct
Cyc_Absyn_Datatypefield*f){struct Cyc_PP_Doc*_tmp99A[3];return(_tmp99A[2]=f->typs
== 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmp99A[1]=Cyc_Absynpp_typedef_name2doc(
f->name),((_tmp99A[0]=Cyc_Absynpp_scope2doc((void*)f->sc),Cyc_PP_cat(
_tag_dyneither(_tmp99A,sizeof(struct Cyc_PP_Doc*),3)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields){const char*_tmp99B;return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmp99B=",",
_tag_dyneither(_tmp99B,sizeof(char),2))),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{const char*_tmp99E;
void*_tmp99D;(_tmp99D=0,Cyc_fprintf(f,((_tmp99E="\n",_tag_dyneither(_tmp99E,
sizeof(char),2))),_tag_dyneither(_tmp99D,sizeof(void*),0)));}}}struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){const char*
_tmp99F;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmp99F="",_tag_dyneither(
_tmp99F,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),
72);}struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(
Cyc_Absynpp_exp2doc(e),72);}struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct
Cyc_Absyn_Stmt*s);struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*
s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s),72);}struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*t);struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){
return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}struct _dyneither_ptr Cyc_Absynpp_typ2cstring(
void*t);struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*t){int old_qvar_to_Cids=
Cyc_Absynpp_qvar_to_Cids;int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;Cyc_Absynpp_qvar_to_Cids=
1;Cyc_Absynpp_add_cyc_prefix=0;{struct _dyneither_ptr s=Cyc_Absynpp_typ2string(t);
Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
old_add_cyc_prefix;return s;}}struct _dyneither_ptr Cyc_Absynpp_prim2string(void*p);
struct _dyneither_ptr Cyc_Absynpp_prim2string(void*p){return Cyc_PP_string_of_doc(
Cyc_Absynpp_prim2doc(p),72);}struct _dyneither_ptr Cyc_Absynpp_pat2string(struct
Cyc_Absyn_Pat*p);struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*
p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}struct _dyneither_ptr
Cyc_Absynpp_scope2string(void*sc);struct _dyneither_ptr Cyc_Absynpp_scope2string(
void*sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
