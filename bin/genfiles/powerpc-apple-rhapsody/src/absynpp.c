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
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
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
curr_namespace;};extern int Cyc_Absynpp_print_scopes;void Cyc_Absynpp_set_params(
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
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
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
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;struct Cyc_Absynpp_Params;
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs){Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;Cyc_Absynpp_qvar_to_Cids=
fs->qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;Cyc_Absynpp_to_VC=
fs->to_VC;Cyc_Absynpp_decls_first=fs->decls_first;Cyc_Absynpp_rewrite_temp_tvars=
fs->rewrite_temp_tvars;Cyc_Absynpp_print_all_tvars=fs->print_all_tvars;Cyc_Absynpp_print_all_kinds=
fs->print_all_kinds;Cyc_Absynpp_print_all_effects=fs->print_all_effects;Cyc_Absynpp_print_using_stmts=
fs->print_using_stmts;Cyc_Absynpp_print_externC_stmts=fs->print_externC_stmts;
Cyc_Absynpp_print_full_evars=fs->print_full_evars;Cyc_Absynpp_print_zeroterm=fs->print_zeroterm;
Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;Cyc_Absynpp_use_curr_namespace=
fs->use_curr_namespace;Cyc_Absynpp_curr_namespace=fs->curr_namespace;}struct Cyc_Absynpp_Params
Cyc_Absynpp_cyc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params
Cyc_Absynpp_cyci_params_r={1,0,0,0,0,1,0,0,1,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params
Cyc_Absynpp_c_params_r={1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0};struct Cyc_Absynpp_Params
Cyc_Absynpp_tc_params_r={0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0};static void Cyc_Absynpp_curr_namespace_add(
struct _dyneither_ptr*v);static void Cyc_Absynpp_curr_namespace_add(struct
_dyneither_ptr*v){struct Cyc_List_List*_tmp4E6;Cyc_Absynpp_curr_namespace=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_Absynpp_curr_namespace,((_tmp4E6=_cycalloc(sizeof(*_tmp4E6)),((_tmp4E6->hd=v,((
_tmp4E6->tl=0,_tmp4E6)))))));}static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**
l);static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){if(((struct Cyc_List_List*)
_check_null(*l))->tl == 0)*l=0;else{Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)
_check_null(*l))->tl);}}static void Cyc_Absynpp_curr_namespace_drop();static void
Cyc_Absynpp_curr_namespace_drop(){((void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(&
Cyc_Absynpp_curr_namespace);}struct _dyneither_ptr Cyc_Absynpp_char_escape(char c);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c){switch(c){case '\a': _LL0: {
const char*_tmp4E7;return(_tmp4E7="\\a",_tag_dyneither(_tmp4E7,sizeof(char),3));}
case '\b': _LL1: {const char*_tmp4E8;return(_tmp4E8="\\b",_tag_dyneither(_tmp4E8,
sizeof(char),3));}case '\f': _LL2: {const char*_tmp4E9;return(_tmp4E9="\\f",
_tag_dyneither(_tmp4E9,sizeof(char),3));}case '\n': _LL3: {const char*_tmp4EA;
return(_tmp4EA="\\n",_tag_dyneither(_tmp4EA,sizeof(char),3));}case '\r': _LL4: {
const char*_tmp4EB;return(_tmp4EB="\\r",_tag_dyneither(_tmp4EB,sizeof(char),3));}
case '\t': _LL5: {const char*_tmp4EC;return(_tmp4EC="\\t",_tag_dyneither(_tmp4EC,
sizeof(char),3));}case '\v': _LL6: {const char*_tmp4ED;return(_tmp4ED="\\v",
_tag_dyneither(_tmp4ED,sizeof(char),3));}case '\\': _LL7: {const char*_tmp4EE;
return(_tmp4EE="\\\\",_tag_dyneither(_tmp4EE,sizeof(char),3));}case '"': _LL8: {
const char*_tmp4EF;return(_tmp4EF="\"",_tag_dyneither(_tmp4EF,sizeof(char),2));}
case '\'': _LL9: {const char*_tmp4F0;return(_tmp4F0="\\'",_tag_dyneither(_tmp4F0,
sizeof(char),3));}default: _LLA: if(c >= ' '  && c <= '~'){struct _dyneither_ptr _tmpC=
Cyc_Core_new_string(2);{char _tmp4F3;char _tmp4F2;struct _dyneither_ptr _tmp4F1;(
_tmp4F1=_dyneither_ptr_plus(_tmpC,sizeof(char),0),((_tmp4F2=*((char*)
_check_dyneither_subscript(_tmp4F1,sizeof(char),0)),((_tmp4F3=c,((
_get_dyneither_size(_tmp4F1,sizeof(char))== 1  && (_tmp4F2 == '\000'  && _tmp4F3 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp4F1.curr)=_tmp4F3)))))));}return(struct
_dyneither_ptr)_tmpC;}else{struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);int
j=0;{char _tmp4F6;char _tmp4F5;struct _dyneither_ptr _tmp4F4;(_tmp4F4=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp4F5=*((char*)
_check_dyneither_subscript(_tmp4F4,sizeof(char),0)),((_tmp4F6='\\',((
_get_dyneither_size(_tmp4F4,sizeof(char))== 1  && (_tmp4F5 == '\000'  && _tmp4F6 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp4F4.curr)=_tmp4F6)))))));}{char _tmp4F9;char
_tmp4F8;struct _dyneither_ptr _tmp4F7;(_tmp4F7=_dyneither_ptr_plus(_tmp10,sizeof(
char),j ++),((_tmp4F8=*((char*)_check_dyneither_subscript(_tmp4F7,sizeof(char),0)),((
_tmp4F9=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp4F7,
sizeof(char))== 1  && (_tmp4F8 == '\000'  && _tmp4F9 != '\000')?_throw_arraybounds():
1,*((char*)_tmp4F7.curr)=_tmp4F9)))))));}{char _tmp4FC;char _tmp4FB;struct
_dyneither_ptr _tmp4FA;(_tmp4FA=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp4FB=*((char*)_check_dyneither_subscript(_tmp4FA,sizeof(char),0)),((_tmp4FC=(
char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp4FA,sizeof(char))== 1  && (
_tmp4FB == '\000'  && _tmp4FC != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4FA.curr)=
_tmp4FC)))))));}{char _tmp4FF;char _tmp4FE;struct _dyneither_ptr _tmp4FD;(_tmp4FD=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp4FE=*((char*)
_check_dyneither_subscript(_tmp4FD,sizeof(char),0)),((_tmp4FF=(char)('0' + (c & 7)),((
_get_dyneither_size(_tmp4FD,sizeof(char))== 1  && (_tmp4FE == '\000'  && _tmp4FF != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp4FD.curr)=_tmp4FF)))))));}return(struct
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
_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:{char _tmp502;char _tmp501;struct
_dyneither_ptr _tmp500;(_tmp500=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp501=*((
char*)_check_dyneither_subscript(_tmp500,sizeof(char),0)),((_tmp502='\\',((
_get_dyneither_size(_tmp500,sizeof(char))== 1  && (_tmp501 == '\000'  && _tmp502 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp500.curr)=_tmp502)))))));}{char _tmp505;char
_tmp504;struct _dyneither_ptr _tmp503;(_tmp503=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp504=*((char*)_check_dyneither_subscript(_tmp503,sizeof(char),0)),((
_tmp505='a',((_get_dyneither_size(_tmp503,sizeof(char))== 1  && (_tmp504 == '\000'
 && _tmp505 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp503.curr)=_tmp505)))))));}
goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:{char _tmp508;char _tmp507;struct
_dyneither_ptr _tmp506;(_tmp506=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp507=*((
char*)_check_dyneither_subscript(_tmp506,sizeof(char),0)),((_tmp508='\\',((
_get_dyneither_size(_tmp506,sizeof(char))== 1  && (_tmp507 == '\000'  && _tmp508 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp506.curr)=_tmp508)))))));}{char _tmp50B;char
_tmp50A;struct _dyneither_ptr _tmp509;(_tmp509=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp50A=*((char*)_check_dyneither_subscript(_tmp509,sizeof(char),0)),((
_tmp50B='b',((_get_dyneither_size(_tmp509,sizeof(char))== 1  && (_tmp50A == '\000'
 && _tmp50B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp509.curr)=_tmp50B)))))));}
goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:{char _tmp50E;char _tmp50D;struct
_dyneither_ptr _tmp50C;(_tmp50C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp50D=*((
char*)_check_dyneither_subscript(_tmp50C,sizeof(char),0)),((_tmp50E='\\',((
_get_dyneither_size(_tmp50C,sizeof(char))== 1  && (_tmp50D == '\000'  && _tmp50E != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp50C.curr)=_tmp50E)))))));}{char _tmp511;char
_tmp510;struct _dyneither_ptr _tmp50F;(_tmp50F=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp510=*((char*)_check_dyneither_subscript(_tmp50F,sizeof(char),0)),((
_tmp511='f',((_get_dyneither_size(_tmp50F,sizeof(char))== 1  && (_tmp510 == '\000'
 && _tmp511 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp50F.curr)=_tmp511)))))));}
goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:{char _tmp514;char _tmp513;struct
_dyneither_ptr _tmp512;(_tmp512=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp513=*((
char*)_check_dyneither_subscript(_tmp512,sizeof(char),0)),((_tmp514='\\',((
_get_dyneither_size(_tmp512,sizeof(char))== 1  && (_tmp513 == '\000'  && _tmp514 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp512.curr)=_tmp514)))))));}{char _tmp517;char
_tmp516;struct _dyneither_ptr _tmp515;(_tmp515=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp516=*((char*)_check_dyneither_subscript(_tmp515,sizeof(char),0)),((
_tmp517='n',((_get_dyneither_size(_tmp515,sizeof(char))== 1  && (_tmp516 == '\000'
 && _tmp517 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp515.curr)=_tmp517)))))));}
goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:{char _tmp51A;char _tmp519;struct
_dyneither_ptr _tmp518;(_tmp518=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp519=*((
char*)_check_dyneither_subscript(_tmp518,sizeof(char),0)),((_tmp51A='\\',((
_get_dyneither_size(_tmp518,sizeof(char))== 1  && (_tmp519 == '\000'  && _tmp51A != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp518.curr)=_tmp51A)))))));}{char _tmp51D;char
_tmp51C;struct _dyneither_ptr _tmp51B;(_tmp51B=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp51C=*((char*)_check_dyneither_subscript(_tmp51B,sizeof(char),0)),((
_tmp51D='r',((_get_dyneither_size(_tmp51B,sizeof(char))== 1  && (_tmp51C == '\000'
 && _tmp51D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp51B.curr)=_tmp51D)))))));}
goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:{char _tmp520;char _tmp51F;struct
_dyneither_ptr _tmp51E;(_tmp51E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp51F=*((
char*)_check_dyneither_subscript(_tmp51E,sizeof(char),0)),((_tmp520='\\',((
_get_dyneither_size(_tmp51E,sizeof(char))== 1  && (_tmp51F == '\000'  && _tmp520 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp51E.curr)=_tmp520)))))));}{char _tmp523;char
_tmp522;struct _dyneither_ptr _tmp521;(_tmp521=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp522=*((char*)_check_dyneither_subscript(_tmp521,sizeof(char),0)),((
_tmp523='t',((_get_dyneither_size(_tmp521,sizeof(char))== 1  && (_tmp522 == '\000'
 && _tmp523 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp521.curr)=_tmp523)))))));}
goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:{char _tmp526;char _tmp525;struct
_dyneither_ptr _tmp524;(_tmp524=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp525=*((
char*)_check_dyneither_subscript(_tmp524,sizeof(char),0)),((_tmp526='\\',((
_get_dyneither_size(_tmp524,sizeof(char))== 1  && (_tmp525 == '\000'  && _tmp526 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp524.curr)=_tmp526)))))));}{char _tmp529;char
_tmp528;struct _dyneither_ptr _tmp527;(_tmp527=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp528=*((char*)_check_dyneither_subscript(_tmp527,sizeof(char),0)),((
_tmp529='v',((_get_dyneither_size(_tmp527,sizeof(char))== 1  && (_tmp528 == '\000'
 && _tmp529 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp527.curr)=_tmp529)))))));}
goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:{char _tmp52C;char _tmp52B;struct
_dyneither_ptr _tmp52A;(_tmp52A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp52B=*((
char*)_check_dyneither_subscript(_tmp52A,sizeof(char),0)),((_tmp52C='\\',((
_get_dyneither_size(_tmp52A,sizeof(char))== 1  && (_tmp52B == '\000'  && _tmp52C != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp52A.curr)=_tmp52C)))))));}{char _tmp52F;char
_tmp52E;struct _dyneither_ptr _tmp52D;(_tmp52D=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp52E=*((char*)_check_dyneither_subscript(_tmp52D,sizeof(char),0)),((
_tmp52F='\\',((_get_dyneither_size(_tmp52D,sizeof(char))== 1  && (_tmp52E == '\000'
 && _tmp52F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp52D.curr)=_tmp52F)))))));}
goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:{char _tmp532;char _tmp531;struct
_dyneither_ptr _tmp530;(_tmp530=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp531=*((
char*)_check_dyneither_subscript(_tmp530,sizeof(char),0)),((_tmp532='\\',((
_get_dyneither_size(_tmp530,sizeof(char))== 1  && (_tmp531 == '\000'  && _tmp532 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp530.curr)=_tmp532)))))));}{char _tmp535;char
_tmp534;struct _dyneither_ptr _tmp533;(_tmp533=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp534=*((char*)_check_dyneither_subscript(_tmp533,sizeof(char),0)),((
_tmp535='"',((_get_dyneither_size(_tmp533,sizeof(char))== 1  && (_tmp534 == '\000'
 && _tmp535 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp533.curr)=_tmp535)))))));}
goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp538;char _tmp537;
struct _dyneither_ptr _tmp536;(_tmp536=_dyneither_ptr_plus(t,sizeof(char),j ++),((
_tmp537=*((char*)_check_dyneither_subscript(_tmp536,sizeof(char),0)),((_tmp538=
_tmp1E,((_get_dyneither_size(_tmp536,sizeof(char))== 1  && (_tmp537 == '\000'  && 
_tmp538 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp536.curr)=_tmp538)))))));}
else{{char _tmp53B;char _tmp53A;struct _dyneither_ptr _tmp539;(_tmp539=
_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp53A=*((char*)
_check_dyneither_subscript(_tmp539,sizeof(char),0)),((_tmp53B='\\',((
_get_dyneither_size(_tmp539,sizeof(char))== 1  && (_tmp53A == '\000'  && _tmp53B != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp539.curr)=_tmp53B)))))));}{char _tmp53E;char
_tmp53D;struct _dyneither_ptr _tmp53C;(_tmp53C=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp53D=*((char*)_check_dyneither_subscript(_tmp53C,sizeof(char),0)),((
_tmp53E=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dyneither_size(_tmp53C,sizeof(char))
== 1  && (_tmp53D == '\000'  && _tmp53E != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp53C.curr)=_tmp53E)))))));}{char _tmp541;char _tmp540;struct _dyneither_ptr
_tmp53F;(_tmp53F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp540=*((char*)
_check_dyneither_subscript(_tmp53F,sizeof(char),0)),((_tmp541=(char)('0' + (
_tmp1E >> 3 & 7)),((_get_dyneither_size(_tmp53F,sizeof(char))== 1  && (_tmp540 == '\000'
 && _tmp541 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp53F.curr)=_tmp541)))))));}{
char _tmp544;char _tmp543;struct _dyneither_ptr _tmp542;(_tmp542=_dyneither_ptr_plus(
t,sizeof(char),j ++),((_tmp543=*((char*)_check_dyneither_subscript(_tmp542,
sizeof(char),0)),((_tmp544=(char)('0' + (_tmp1E & 7)),((_get_dyneither_size(
_tmp542,sizeof(char))== 1  && (_tmp543 == '\000'  && _tmp544 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp542.curr)=_tmp544)))))));}}goto _LL21;_LL21:;}}
return(struct _dyneither_ptr)t;}}}}static char _tmp64[9]="restrict";static struct
_dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char
_tmp65[9]="volatile";static struct _dyneither_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dyneither_ptr
Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string;static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq);struct Cyc_PP_Doc*
Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=0;if(tq.q_restrict){
struct Cyc_List_List*_tmp545;l=((_tmp545=_cycalloc(sizeof(*_tmp545)),((_tmp545->hd=
Cyc_Absynpp_restrict_sp,((_tmp545->tl=l,_tmp545))))));}if(tq.q_volatile){struct
Cyc_List_List*_tmp546;l=((_tmp546=_cycalloc(sizeof(*_tmp546)),((_tmp546->hd=Cyc_Absynpp_volatile_sp,((
_tmp546->tl=l,_tmp546))))));}if(tq.print_const){struct Cyc_List_List*_tmp547;l=((
_tmp547=_cycalloc(sizeof(*_tmp547)),((_tmp547->hd=Cyc_Absynpp_const_sp,((_tmp547->tl=
l,_tmp547))))));}{const char*_tmp54A;const char*_tmp549;const char*_tmp548;return
Cyc_PP_egroup(((_tmp548="",_tag_dyneither(_tmp548,sizeof(char),1))),((_tmp549=" ",
_tag_dyneither(_tmp549,sizeof(char),2))),((_tmp54A=" ",_tag_dyneither(_tmp54A,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));}}
struct _dyneither_ptr Cyc_Absynpp_kind2string(void*k);struct _dyneither_ptr Cyc_Absynpp_kind2string(
void*k){void*_tmp6D=k;_LL37: if((int)_tmp6D != 0)goto _LL39;_LL38: {const char*
_tmp54B;return(_tmp54B="A",_tag_dyneither(_tmp54B,sizeof(char),2));}_LL39: if((
int)_tmp6D != 1)goto _LL3B;_LL3A: {const char*_tmp54C;return(_tmp54C="M",
_tag_dyneither(_tmp54C,sizeof(char),2));}_LL3B: if((int)_tmp6D != 2)goto _LL3D;
_LL3C: {const char*_tmp54D;return(_tmp54D="B",_tag_dyneither(_tmp54D,sizeof(char),
2));}_LL3D: if((int)_tmp6D != 3)goto _LL3F;_LL3E: {const char*_tmp54E;return(_tmp54E="R",
_tag_dyneither(_tmp54E,sizeof(char),2));}_LL3F: if((int)_tmp6D != 4)goto _LL41;
_LL40: {const char*_tmp54F;return(_tmp54F="UR",_tag_dyneither(_tmp54F,sizeof(char),
3));}_LL41: if((int)_tmp6D != 5)goto _LL43;_LL42: {const char*_tmp550;return(_tmp550="TR",
_tag_dyneither(_tmp550,sizeof(char),3));}_LL43: if((int)_tmp6D != 6)goto _LL45;
_LL44: {const char*_tmp551;return(_tmp551="E",_tag_dyneither(_tmp551,sizeof(char),
2));}_LL45: if((int)_tmp6D != 7)goto _LL36;_LL46: {const char*_tmp552;return(_tmp552="I",
_tag_dyneither(_tmp552,sizeof(char),2));}_LL36:;}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(
void*k);struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(void*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(
k));}struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c);struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp76=Cyc_Absyn_compress_kb(
c);void*_tmp77;void*_tmp78;_LL48: if(*((int*)_tmp76)!= 0)goto _LL4A;_tmp77=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp76)->f1;_LL49: return Cyc_Absynpp_kind2string(
_tmp77);_LL4A: if(*((int*)_tmp76)!= 1)goto _LL4C;_LL4B: if(Cyc_PP_tex_output){const
char*_tmp553;return(_tmp553="{?}",_tag_dyneither(_tmp553,sizeof(char),4));}else{
const char*_tmp554;return(_tmp554="?",_tag_dyneither(_tmp554,sizeof(char),2));}
_LL4C: if(*((int*)_tmp76)!= 2)goto _LL47;_tmp78=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp76)->f2;_LL4D: return Cyc_Absynpp_kind2string(_tmp78);_LL47:;}struct Cyc_PP_Doc*
Cyc_Absynpp_kindbound2doc(void*c);struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(
void*c){void*_tmp7B=Cyc_Absyn_compress_kb(c);void*_tmp7C;void*_tmp7D;_LL4F: if(*((
int*)_tmp7B)!= 0)goto _LL51;_tmp7C=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp7B)->f1;
_LL50: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7C));_LL51: if(*((int*)_tmp7B)
!= 1)goto _LL53;_LL52: if(Cyc_PP_tex_output){const char*_tmp555;return Cyc_PP_text_width(((
_tmp555="{?}",_tag_dyneither(_tmp555,sizeof(char),4))),1);}else{const char*
_tmp556;return Cyc_PP_text(((_tmp556="?",_tag_dyneither(_tmp556,sizeof(char),2))));}
_LL53: if(*((int*)_tmp7B)!= 2)goto _LL4E;_tmp7D=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp7B)->f2;_LL54: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7D));_LL4E:;}
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts);struct Cyc_PP_Doc*
Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){const char*_tmp559;const char*_tmp558;
const char*_tmp557;return Cyc_PP_egroup(((_tmp557="<",_tag_dyneither(_tmp557,
sizeof(char),2))),((_tmp558=">",_tag_dyneither(_tmp558,sizeof(char),2))),((
_tmp559=",",_tag_dyneither(_tmp559,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv);struct Cyc_PP_Doc*
Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){void*_tmp83=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmp84;void*_tmp85;void*_tmp86;_LL56: if(*((int*)_tmp83)!= 1)
goto _LL58;_LL57: goto _LL59;_LL58: if(*((int*)_tmp83)!= 0)goto _LL5A;_tmp84=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp83)->f1;if((int)_tmp84 != 2)goto _LL5A;_LL59:
return Cyc_PP_textptr(tv->name);_LL5A: if(*((int*)_tmp83)!= 2)goto _LL5C;_tmp85=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp83)->f2;_LL5B: _tmp86=_tmp85;goto
_LL5D;_LL5C: if(*((int*)_tmp83)!= 0)goto _LL55;_tmp86=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp83)->f1;_LL5D: {const char*_tmp55C;struct Cyc_PP_Doc*_tmp55B[3];return(_tmp55B[
2]=Cyc_Absynpp_kind2doc(_tmp86),((_tmp55B[1]=Cyc_PP_text(((_tmp55C="::",
_tag_dyneither(_tmp55C,sizeof(char),3)))),((_tmp55B[0]=Cyc_PP_textptr(tv->name),
Cyc_PP_cat(_tag_dyneither(_tmp55B,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL55:;}
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs);struct Cyc_PP_Doc*
Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){const char*_tmp55F;const char*
_tmp55E;const char*_tmp55D;return Cyc_PP_egroup(((_tmp55D="<",_tag_dyneither(
_tmp55D,sizeof(char),2))),((_tmp55E=">",_tag_dyneither(_tmp55E,sizeof(char),2))),((
_tmp55F=",",_tag_dyneither(_tmp55F,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_ktvar2doc,tvs));}static struct _dyneither_ptr*Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar*tv);static struct _dyneither_ptr*Cyc_Absynpp_get_name(struct
Cyc_Absyn_Tvar*tv){return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct
Cyc_List_List*tvs);struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*
tvs){if(Cyc_Absynpp_print_all_kinds)return Cyc_Absynpp_ktvars2doc(tvs);{const char*
_tmp562;const char*_tmp561;const char*_tmp560;return Cyc_PP_egroup(((_tmp560="<",
_tag_dyneither(_tmp560,sizeof(char),2))),((_tmp561=">",_tag_dyneither(_tmp561,
sizeof(char),2))),((_tmp562=",",_tag_dyneither(_tmp562,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
tvs)));}}struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(
struct _tuple10*t);struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple10*t){return
Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(
struct Cyc_List_List*ts);struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*
ts){const char*_tmp565;const char*_tmp564;const char*_tmp563;return Cyc_PP_group(((
_tmp563="(",_tag_dyneither(_tmp563,sizeof(char),2))),((_tmp564=")",
_tag_dyneither(_tmp564,sizeof(char),2))),((_tmp565=",",_tag_dyneither(_tmp565,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple10*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}struct Cyc_PP_Doc*
Cyc_Absynpp_noncallatt2doc(void*att);struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(
void*att){void*_tmp92=att;_LL5F: if((int)_tmp92 != 0)goto _LL61;_LL60: goto _LL62;
_LL61: if((int)_tmp92 != 1)goto _LL63;_LL62: goto _LL64;_LL63: if((int)_tmp92 != 2)goto
_LL65;_LL64: return Cyc_PP_nil_doc();_LL65:;_LL66: return Cyc_PP_text(Cyc_Absyn_attribute2string(
att));_LL5E:;}struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts);
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){for(0;atts != 
0;atts=atts->tl){void*_tmp93=(void*)atts->hd;_LL68: if((int)_tmp93 != 0)goto _LL6A;
_LL69: {const char*_tmp566;return Cyc_PP_text(((_tmp566=" _stdcall ",
_tag_dyneither(_tmp566,sizeof(char),11))));}_LL6A: if((int)_tmp93 != 1)goto _LL6C;
_LL6B: {const char*_tmp567;return Cyc_PP_text(((_tmp567=" _cdecl ",_tag_dyneither(
_tmp567,sizeof(char),9))));}_LL6C: if((int)_tmp93 != 2)goto _LL6E;_LL6D: {const char*
_tmp568;return Cyc_PP_text(((_tmp568=" _fastcall ",_tag_dyneither(_tmp568,sizeof(
char),12))));}_LL6E:;_LL6F: goto _LL67;_LL67:;}return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*
Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts);struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(
struct Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;
atts2 != 0;atts2=atts2->tl){void*_tmp97=(void*)atts2->hd;_LL71: if((int)_tmp97 != 0)
goto _LL73;_LL72: goto _LL74;_LL73: if((int)_tmp97 != 1)goto _LL75;_LL74: goto _LL76;
_LL75: if((int)_tmp97 != 2)goto _LL77;_LL76: goto _LL70;_LL77:;_LL78: hasatt=1;goto
_LL70;_LL70:;}}if(!hasatt)return Cyc_PP_nil_doc();{const char*_tmp573;const char*
_tmp572;const char*_tmp571;const char*_tmp570;const char*_tmp56F;struct Cyc_PP_Doc*
_tmp56E[3];return(_tmp56E[2]=Cyc_PP_text(((_tmp573=")",_tag_dyneither(_tmp573,
sizeof(char),2)))),((_tmp56E[1]=Cyc_PP_group(((_tmp572="",_tag_dyneither(_tmp572,
sizeof(char),1))),((_tmp571="",_tag_dyneither(_tmp571,sizeof(char),1))),((
_tmp570=" ",_tag_dyneither(_tmp570,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,
atts)),((_tmp56E[0]=Cyc_PP_text(((_tmp56F=" __declspec(",_tag_dyneither(_tmp56F,
sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp56E,sizeof(struct Cyc_PP_Doc*),
3)))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a);struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(
void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts);struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*
atts){if(atts == 0)return Cyc_PP_nil_doc();{void*_tmp9E=Cyc_Cyclone_c_compiler;
_LL7A: if((int)_tmp9E != 1)goto _LL7C;_LL7B: return Cyc_Absynpp_noncallconv2doc(atts);
_LL7C: if((int)_tmp9E != 0)goto _LL79;_LL7D: {const char*_tmp57C;const char*_tmp57B;
const char*_tmp57A;const char*_tmp579;struct Cyc_PP_Doc*_tmp578[2];return(_tmp578[1]=
Cyc_PP_group(((_tmp57C="((",_tag_dyneither(_tmp57C,sizeof(char),3))),((_tmp57B="))",
_tag_dyneither(_tmp57B,sizeof(char),3))),((_tmp57A=",",_tag_dyneither(_tmp57A,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp578[0]=Cyc_PP_text(((
_tmp579=" __attribute__",_tag_dyneither(_tmp579,sizeof(char),15)))),Cyc_PP_cat(
_tag_dyneither(_tmp578,sizeof(struct Cyc_PP_Doc*),2)))));}_LL79:;}}int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List*tms);int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{void*_tmpA4=(void*)tms->hd;_LL7F: if(*((int*)_tmpA4)!= 2)goto
_LL81;_LL80: return 1;_LL81: if(*((int*)_tmpA4)!= 5)goto _LL83;_LL82: {void*_tmpA5=
Cyc_Cyclone_c_compiler;_LL86: if((int)_tmpA5 != 0)goto _LL88;_LL87: return 0;_LL88:;
_LL89: return Cyc_Absynpp_next_is_pointer(tms->tl);_LL85:;}_LL83:;_LL84: return 0;
_LL7E:;}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);static struct Cyc_PP_Doc*
Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*Cyc_Absynpp_question();
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output){const char*_tmp57D;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp57D="{?}",_tag_dyneither(_tmp57D,sizeof(char),4))),1);}
else{const char*_tmp57E;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp57E="?",_tag_dyneither(_tmp57E,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_lb();static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){if(Cyc_PP_tex_output){const char*
_tmp57F;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp57F="{\\lb}",
_tag_dyneither(_tmp57F,sizeof(char),6))),1);}else{const char*_tmp580;Cyc_Absynpp_cache_lb=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp580="{",_tag_dyneither(_tmp580,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}static struct
Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb();static
struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){if(
Cyc_PP_tex_output){const char*_tmp581;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp581="{\\rb}",_tag_dyneither(_tmp581,sizeof(char),6))),1);}else{const char*
_tmp582;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp582="}",
_tag_dyneither(_tmp582,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;static
struct Cyc_PP_Doc*Cyc_Absynpp_dollar();static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){if(Cyc_PP_tex_output){const char*
_tmp583;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp583="\\$",
_tag_dyneither(_tmp583,sizeof(char),3))),1);}else{const char*_tmp584;Cyc_Absynpp_cache_dollar=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp584="$",_tag_dyneither(_tmp584,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}struct
Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss);struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss){struct Cyc_PP_Doc*_tmp585[3];return(_tmp585[2]=Cyc_Absynpp_rb(),((_tmp585[1]=
Cyc_PP_seq(sep,ss),((_tmp585[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(
_tmp585,sizeof(struct Cyc_PP_Doc*),3)))))));}static void Cyc_Absynpp_print_tms(
struct Cyc_List_List*tms);static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){void*_tmpAF=(void*)tms->hd;void*_tmpB0;struct Cyc_List_List*
_tmpB1;_LL8B: if(*((int*)_tmpAF)!= 0)goto _LL8D;_LL8C:{const char*_tmp588;void*
_tmp587;(_tmp587=0,Cyc_fprintf(Cyc_stderr,((_tmp588="Carray_mod ",_tag_dyneither(
_tmp588,sizeof(char),12))),_tag_dyneither(_tmp587,sizeof(void*),0)));}goto _LL8A;
_LL8D: if(*((int*)_tmpAF)!= 1)goto _LL8F;_LL8E:{const char*_tmp58B;void*_tmp58A;(
_tmp58A=0,Cyc_fprintf(Cyc_stderr,((_tmp58B="ConstArray_mod ",_tag_dyneither(
_tmp58B,sizeof(char),16))),_tag_dyneither(_tmp58A,sizeof(void*),0)));}goto _LL8A;
_LL8F: if(*((int*)_tmpAF)!= 3)goto _LL91;_tmpB0=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpAF)->f1;if(*((int*)_tmpB0)!= 1)goto _LL91;_tmpB1=((struct Cyc_Absyn_WithTypes_struct*)
_tmpB0)->f1;_LL90:{const char*_tmp58E;void*_tmp58D;(_tmp58D=0,Cyc_fprintf(Cyc_stderr,((
_tmp58E="Function_mod(",_tag_dyneither(_tmp58E,sizeof(char),14))),_tag_dyneither(
_tmp58D,sizeof(void*),0)));}for(0;_tmpB1 != 0;_tmpB1=_tmpB1->tl){struct Cyc_Core_Opt*
_tmpB8=(*((struct _tuple1*)_tmpB1->hd)).f1;if(_tmpB8 == 0){const char*_tmp591;void*
_tmp590;(_tmp590=0,Cyc_fprintf(Cyc_stderr,((_tmp591="?",_tag_dyneither(_tmp591,
sizeof(char),2))),_tag_dyneither(_tmp590,sizeof(void*),0)));}else{void*_tmp592;(
_tmp592=0,Cyc_fprintf(Cyc_stderr,*((struct _dyneither_ptr*)_tmpB8->v),
_tag_dyneither(_tmp592,sizeof(void*),0)));}if(_tmpB1->tl != 0){const char*_tmp595;
void*_tmp594;(_tmp594=0,Cyc_fprintf(Cyc_stderr,((_tmp595=",",_tag_dyneither(
_tmp595,sizeof(char),2))),_tag_dyneither(_tmp594,sizeof(void*),0)));}}{const char*
_tmp598;void*_tmp597;(_tmp597=0,Cyc_fprintf(Cyc_stderr,((_tmp598=") ",
_tag_dyneither(_tmp598,sizeof(char),3))),_tag_dyneither(_tmp597,sizeof(void*),0)));}
goto _LL8A;_LL91: if(*((int*)_tmpAF)!= 3)goto _LL93;_LL92:{const char*_tmp59B;void*
_tmp59A;(_tmp59A=0,Cyc_fprintf(Cyc_stderr,((_tmp59B="Function_mod()",
_tag_dyneither(_tmp59B,sizeof(char),15))),_tag_dyneither(_tmp59A,sizeof(void*),0)));}
goto _LL8A;_LL93: if(*((int*)_tmpAF)!= 5)goto _LL95;_LL94:{const char*_tmp59E;void*
_tmp59D;(_tmp59D=0,Cyc_fprintf(Cyc_stderr,((_tmp59E="Attributes_mod ",
_tag_dyneither(_tmp59E,sizeof(char),16))),_tag_dyneither(_tmp59D,sizeof(void*),0)));}
goto _LL8A;_LL95: if(*((int*)_tmpAF)!= 4)goto _LL97;_LL96:{const char*_tmp5A1;void*
_tmp5A0;(_tmp5A0=0,Cyc_fprintf(Cyc_stderr,((_tmp5A1="TypeParams_mod ",
_tag_dyneither(_tmp5A1,sizeof(char),16))),_tag_dyneither(_tmp5A0,sizeof(void*),0)));}
goto _LL8A;_LL97: if(*((int*)_tmpAF)!= 2)goto _LL8A;_LL98:{const char*_tmp5A4;void*
_tmp5A3;(_tmp5A3=0,Cyc_fprintf(Cyc_stderr,((_tmp5A4="Pointer_mod ",
_tag_dyneither(_tmp5A4,sizeof(char),13))),_tag_dyneither(_tmp5A3,sizeof(void*),0)));}
goto _LL8A;_LL8A:;}{const char*_tmp5A7;void*_tmp5A6;(_tmp5A6=0,Cyc_fprintf(Cyc_stderr,((
_tmp5A7="\n",_tag_dyneither(_tmp5A7,sizeof(char),2))),_tag_dyneither(_tmp5A6,
sizeof(void*),0)));}}struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct
Cyc_PP_Doc*d,struct Cyc_List_List*tms);struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int
is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);const char*_tmp5AC;const
char*_tmp5AB;struct Cyc_PP_Doc*_tmp5AA[3];struct Cyc_PP_Doc*p_rest=(_tmp5AA[2]=Cyc_PP_text(((
_tmp5AC=")",_tag_dyneither(_tmp5AC,sizeof(char),2)))),((_tmp5AA[1]=rest,((
_tmp5AA[0]=Cyc_PP_text(((_tmp5AB="(",_tag_dyneither(_tmp5AB,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp5AA,sizeof(struct Cyc_PP_Doc*),3)))))));void*_tmpCA=(
void*)tms->hd;struct Cyc_Absyn_Conref*_tmpCB;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Conref*
_tmpCD;void*_tmpCE;struct Cyc_List_List*_tmpCF;struct Cyc_List_List*_tmpD0;struct
Cyc_Position_Segment*_tmpD1;int _tmpD2;struct Cyc_Absyn_PtrAtts _tmpD3;void*_tmpD4;
struct Cyc_Absyn_Conref*_tmpD5;struct Cyc_Absyn_Conref*_tmpD6;struct Cyc_Absyn_Conref*
_tmpD7;struct Cyc_Absyn_Tqual _tmpD8;_LL9A: if(*((int*)_tmpCA)!= 0)goto _LL9C;_tmpCB=((
struct Cyc_Absyn_Carray_mod_struct*)_tmpCA)->f1;_LL9B: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{const char*_tmp5B1;const char*_tmp5B0;struct Cyc_PP_Doc*
_tmp5AF[2];return(_tmp5AF[1]=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpCB)?Cyc_PP_text(((_tmp5B0="[]ZEROTERM ",_tag_dyneither(_tmp5B0,sizeof(char),
12)))): Cyc_PP_text(((_tmp5B1="[]",_tag_dyneither(_tmp5B1,sizeof(char),3)))),((
_tmp5AF[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp5AF,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL9C: if(*((int*)_tmpCA)!= 1)goto _LL9E;_tmpCC=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpCA)->f1;_tmpCD=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCA)->f2;_LL9D:
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{const char*_tmp5B8;const char*
_tmp5B7;const char*_tmp5B6;struct Cyc_PP_Doc*_tmp5B5[4];return(_tmp5B5[3]=((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpCD)?Cyc_PP_text(((
_tmp5B7="]ZEROTERM ",_tag_dyneither(_tmp5B7,sizeof(char),11)))): Cyc_PP_text(((
_tmp5B8="]",_tag_dyneither(_tmp5B8,sizeof(char),2)))),((_tmp5B5[2]=Cyc_Absynpp_exp2doc(
_tmpCC),((_tmp5B5[1]=Cyc_PP_text(((_tmp5B6="[",_tag_dyneither(_tmp5B6,sizeof(
char),2)))),((_tmp5B5[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp5B5,sizeof(struct Cyc_PP_Doc*),
4)))))))));}_LL9E: if(*((int*)_tmpCA)!= 3)goto _LLA0;_tmpCE=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpCA)->f1;_LL9F: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{void*
_tmpE0=_tmpCE;struct Cyc_List_List*_tmpE1;int _tmpE2;struct Cyc_Absyn_VarargInfo*
_tmpE3;struct Cyc_Core_Opt*_tmpE4;struct Cyc_List_List*_tmpE5;struct Cyc_List_List*
_tmpE6;struct Cyc_Position_Segment*_tmpE7;_LLA7: if(*((int*)_tmpE0)!= 1)goto _LLA9;
_tmpE1=((struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f1;_tmpE2=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE0)->f2;_tmpE3=((struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f3;_tmpE4=((
struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f4;_tmpE5=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE0)->f5;_LLA8: {struct Cyc_PP_Doc*_tmp5B9[2];return(_tmp5B9[1]=Cyc_Absynpp_funargs2doc(
_tmpE1,_tmpE2,_tmpE3,_tmpE4,_tmpE5),((_tmp5B9[0]=rest,Cyc_PP_cat(_tag_dyneither(
_tmp5B9,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA9: if(*((int*)_tmpE0)!= 0)goto _LLA6;
_tmpE6=((struct Cyc_Absyn_NoTypes_struct*)_tmpE0)->f1;_tmpE7=((struct Cyc_Absyn_NoTypes_struct*)
_tmpE0)->f2;_LLAA: {const char*_tmp5C0;const char*_tmp5BF;const char*_tmp5BE;struct
Cyc_PP_Doc*_tmp5BD[2];return(_tmp5BD[1]=Cyc_PP_group(((_tmp5C0="(",
_tag_dyneither(_tmp5C0,sizeof(char),2))),((_tmp5BF=")",_tag_dyneither(_tmp5BF,
sizeof(char),2))),((_tmp5BE=",",_tag_dyneither(_tmp5BE,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,_tmpE6)),((_tmp5BD[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp5BD,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA6:;}_LLA0: if(*((int*)
_tmpCA)!= 5)goto _LLA2;_tmpCF=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpCA)->f2;
_LLA1: {void*_tmpED=Cyc_Cyclone_c_compiler;_LLAC: if((int)_tmpED != 0)goto _LLAE;
_LLAD: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{struct Cyc_PP_Doc*
_tmp5C1[2];return(_tmp5C1[1]=Cyc_Absynpp_atts2doc(_tmpCF),((_tmp5C1[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp5C1,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAE: if((int)_tmpED != 
1)goto _LLAB;_LLAF: if(Cyc_Absynpp_next_is_pointer(tms->tl)){struct Cyc_PP_Doc*
_tmp5C2[2];return(_tmp5C2[1]=rest,((_tmp5C2[0]=Cyc_Absynpp_callconv2doc(_tmpCF),
Cyc_PP_cat(_tag_dyneither(_tmp5C2,sizeof(struct Cyc_PP_Doc*),2)))));}return rest;
_LLAB:;}_LLA2: if(*((int*)_tmpCA)!= 4)goto _LLA4;_tmpD0=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpCA)->f1;_tmpD1=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCA)->f2;_tmpD2=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCA)->f3;_LLA3: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;if(_tmpD2){struct Cyc_PP_Doc*_tmp5C3[2];return(_tmp5C3[1]=
Cyc_Absynpp_ktvars2doc(_tmpD0),((_tmp5C3[0]=rest,Cyc_PP_cat(_tag_dyneither(
_tmp5C3,sizeof(struct Cyc_PP_Doc*),2)))));}else{struct Cyc_PP_Doc*_tmp5C4[2];
return(_tmp5C4[1]=Cyc_Absynpp_tvars2doc(_tmpD0),((_tmp5C4[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp5C4,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA4: if(*((int*)_tmpCA)
!= 2)goto _LL99;_tmpD3=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCA)->f1;_tmpD4=(
void*)_tmpD3.rgn;_tmpD5=_tmpD3.nullable;_tmpD6=_tmpD3.bounds;_tmpD7=_tmpD3.zero_term;
_tmpD8=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCA)->f2;_LLA5: {struct Cyc_PP_Doc*
ptr;{void*_tmpF2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmpD6);struct Cyc_Absyn_Exp*
_tmpF3;_LLB1: if((int)_tmpF2 != 0)goto _LLB3;_LLB2: ptr=Cyc_Absynpp_question();goto
_LLB0;_LLB3: if(_tmpF2 <= (void*)1)goto _LLB0;if(*((int*)_tmpF2)!= 0)goto _LLB0;
_tmpF3=((struct Cyc_Absyn_Upper_b_struct*)_tmpF2)->f1;_LLB4:{const char*_tmp5C6;
const char*_tmp5C5;ptr=Cyc_PP_text(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmpD5)?(_tmp5C6="*",_tag_dyneither(_tmp5C6,sizeof(char),2)):((_tmp5C5="@",
_tag_dyneither(_tmp5C5,sizeof(char),2))));}{unsigned int _tmpF7;int _tmpF8;struct
_tuple4 _tmpF6=Cyc_Evexp_eval_const_uint_exp(_tmpF3);_tmpF7=_tmpF6.f1;_tmpF8=
_tmpF6.f2;if(!_tmpF8  || _tmpF7 != 1){struct Cyc_PP_Doc*_tmp5C7[4];ptr=((_tmp5C7[3]=
Cyc_Absynpp_rb(),((_tmp5C7[2]=Cyc_Absynpp_exp2doc(_tmpF3),((_tmp5C7[1]=Cyc_Absynpp_lb(),((
_tmp5C7[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp5C7,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB0;}_LLB0:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpD7)){const char*_tmp5CA;
struct Cyc_PP_Doc*_tmp5C9[2];ptr=((_tmp5C9[1]=Cyc_PP_text(((_tmp5CA="ZEROTERM ",
_tag_dyneither(_tmp5CA,sizeof(char),10)))),((_tmp5C9[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp5C9,sizeof(struct Cyc_PP_Doc*),2))))));}else{if(is_char_ptr
 && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpD7)){
const char*_tmp5CD;struct Cyc_PP_Doc*_tmp5CC[2];ptr=((_tmp5CC[1]=Cyc_PP_text(((
_tmp5CD="NOZEROTERM ",_tag_dyneither(_tmp5CD,sizeof(char),12)))),((_tmp5CC[0]=
ptr,Cyc_PP_cat(_tag_dyneither(_tmp5CC,sizeof(struct Cyc_PP_Doc*),2))))));}}}{void*
_tmpFE=Cyc_Tcutil_compress(_tmpD4);_LLB6: if((int)_tmpFE != 2)goto _LLB8;_LLB7: goto
_LLB5;_LLB8:;_LLB9: {const char*_tmp5D0;struct Cyc_PP_Doc*_tmp5CF[3];ptr=((_tmp5CF[
2]=Cyc_PP_text(((_tmp5D0=" ",_tag_dyneither(_tmp5D0,sizeof(char),2)))),((_tmp5CF[
1]=Cyc_Absynpp_typ2doc(_tmpD4),((_tmp5CF[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp5CF,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}{struct Cyc_PP_Doc*_tmp5D1[2];
ptr=((_tmp5D1[1]=Cyc_Absynpp_tqual2doc(_tmpD8),((_tmp5D1[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp5D1,sizeof(struct Cyc_PP_Doc*),2))))));}{struct Cyc_PP_Doc*
_tmp5D2[2];return(_tmp5D2[1]=rest,((_tmp5D2[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp5D2,sizeof(struct Cyc_PP_Doc*),2)))));}}_LL99:;}}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){void*_tmp106=Cyc_Tcutil_compress(
t);_LLBB: if((int)_tmp106 != 2)goto _LLBD;_LLBC: {const char*_tmp5D3;return Cyc_PP_text(((
_tmp5D3="`H",_tag_dyneither(_tmp5D3,sizeof(char),3))));}_LLBD: if((int)_tmp106 != 
3)goto _LLBF;_LLBE: {const char*_tmp5D4;return Cyc_PP_text(((_tmp5D4="`U",
_tag_dyneither(_tmp5D4,sizeof(char),3))));}_LLBF:;_LLC0: return Cyc_Absynpp_ntyp2doc(
t);_LLBA:;}static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct
Cyc_List_List**effects,void*t);static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**
rgions,struct Cyc_List_List**effects,void*t){void*_tmp109=Cyc_Tcutil_compress(t);
void*_tmp10A;struct Cyc_List_List*_tmp10B;_LLC2: if(_tmp109 <= (void*)4)goto _LLC6;
if(*((int*)_tmp109)!= 19)goto _LLC4;_tmp10A=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp109)->f1;_LLC3:{struct Cyc_List_List*_tmp5D5;*rgions=((_tmp5D5=_cycalloc(
sizeof(*_tmp5D5)),((_tmp5D5->hd=Cyc_Absynpp_rgn2doc(_tmp10A),((_tmp5D5->tl=*
rgions,_tmp5D5))))));}goto _LLC1;_LLC4: if(*((int*)_tmp109)!= 20)goto _LLC6;_tmp10B=((
struct Cyc_Absyn_JoinEff_struct*)_tmp109)->f1;_LLC5: for(0;_tmp10B != 0;_tmp10B=
_tmp10B->tl){Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp10B->hd);}goto
_LLC1;_LLC6:;_LLC7:{struct Cyc_List_List*_tmp5D6;*effects=((_tmp5D6=_cycalloc(
sizeof(*_tmp5D6)),((_tmp5D6->hd=Cyc_Absynpp_typ2doc(t),((_tmp5D6->tl=*effects,
_tmp5D6))))));}goto _LLC1;_LLC1:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t);
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct Cyc_List_List*rgions=0;struct
Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(& rgions,& effects,t);rgions=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);effects=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);if(
rgions == 0  && effects != 0){const char*_tmp5D9;const char*_tmp5D8;const char*_tmp5D7;
return Cyc_PP_group(((_tmp5D7="",_tag_dyneither(_tmp5D7,sizeof(char),1))),((
_tmp5D8="",_tag_dyneither(_tmp5D8,sizeof(char),1))),((_tmp5D9="+",_tag_dyneither(
_tmp5D9,sizeof(char),2))),effects);}else{const char*_tmp5DA;struct Cyc_PP_Doc*
_tmp111=Cyc_Absynpp_group_braces(((_tmp5DA=",",_tag_dyneither(_tmp5DA,sizeof(
char),2))),rgions);struct Cyc_List_List*_tmp5DE;const char*_tmp5DD;const char*
_tmp5DC;const char*_tmp5DB;return Cyc_PP_group(((_tmp5DB="",_tag_dyneither(_tmp5DB,
sizeof(char),1))),((_tmp5DC="",_tag_dyneither(_tmp5DC,sizeof(char),1))),((
_tmp5DD="+",_tag_dyneither(_tmp5DD,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((
_tmp5DE=_cycalloc(sizeof(*_tmp5DE)),((_tmp5DE->hd=_tmp111,((_tmp5DE->tl=0,
_tmp5DE))))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(void*k);struct Cyc_PP_Doc*
Cyc_Absynpp_aggr_kind2doc(void*k){void*_tmp117=k;_LLC9: if((int)_tmp117 != 0)goto
_LLCB;_LLCA: {const char*_tmp5DF;return Cyc_PP_text(((_tmp5DF="struct ",
_tag_dyneither(_tmp5DF,sizeof(char),8))));}_LLCB: if((int)_tmp117 != 1)goto _LLC8;
_LLCC: {const char*_tmp5E0;return Cyc_PP_text(((_tmp5E0="union ",_tag_dyneither(
_tmp5E0,sizeof(char),7))));}_LLC8:;}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*s;{void*_tmp11A=t;
struct Cyc_Core_Opt*_tmp11B;struct Cyc_Core_Opt*_tmp11C;int _tmp11D;struct Cyc_Core_Opt*
_tmp11E;struct Cyc_Absyn_Tvar*_tmp11F;struct Cyc_Absyn_TunionInfo _tmp120;union Cyc_Absyn_TunionInfoU_union
_tmp121;struct Cyc_List_List*_tmp122;struct Cyc_Core_Opt*_tmp123;struct Cyc_Absyn_TunionFieldInfo
_tmp124;union Cyc_Absyn_TunionFieldInfoU_union _tmp125;struct Cyc_List_List*_tmp126;
void*_tmp127;void*_tmp128;int _tmp129;struct Cyc_List_List*_tmp12A;struct Cyc_Absyn_AggrInfo
_tmp12B;union Cyc_Absyn_AggrInfoU_union _tmp12C;struct Cyc_List_List*_tmp12D;void*
_tmp12E;struct Cyc_List_List*_tmp12F;struct Cyc_List_List*_tmp130;struct _tuple0*
_tmp131;struct Cyc_Absyn_Exp*_tmp132;struct _tuple0*_tmp133;struct Cyc_List_List*
_tmp134;struct Cyc_Absyn_Typedefdecl*_tmp135;void*_tmp136;void*_tmp137;void*
_tmp138;void*_tmp139;void*_tmp13A;_LLCE: if(_tmp11A <= (void*)4)goto _LLD4;if(*((
int*)_tmp11A)!= 7)goto _LLD0;_LLCF: {const char*_tmp5E1;return Cyc_PP_text(((
_tmp5E1="[[[array]]]",_tag_dyneither(_tmp5E1,sizeof(char),12))));}_LLD0: if(*((
int*)_tmp11A)!= 8)goto _LLD2;_LLD1: return Cyc_PP_nil_doc();_LLD2: if(*((int*)
_tmp11A)!= 4)goto _LLD4;_LLD3: return Cyc_PP_nil_doc();_LLD4: if((int)_tmp11A != 0)
goto _LLD6;_LLD5:{const char*_tmp5E2;s=Cyc_PP_text(((_tmp5E2="void",_tag_dyneither(
_tmp5E2,sizeof(char),5))));}goto _LLCD;_LLD6: if(_tmp11A <= (void*)4)goto _LLE0;if(*((
int*)_tmp11A)!= 0)goto _LLD8;_tmp11B=((struct Cyc_Absyn_Evar_struct*)_tmp11A)->f1;
_tmp11C=((struct Cyc_Absyn_Evar_struct*)_tmp11A)->f2;_tmp11D=((struct Cyc_Absyn_Evar_struct*)
_tmp11A)->f3;_tmp11E=((struct Cyc_Absyn_Evar_struct*)_tmp11A)->f4;_LLD7: if(
_tmp11C != 0)return Cyc_Absynpp_ntyp2doc((void*)_tmp11C->v);else{const char*_tmp5F0;
struct Cyc_Int_pa_struct _tmp5EF;void*_tmp5EE[1];const char*_tmp5ED;const char*
_tmp5EC;const char*_tmp5EB;struct Cyc_PP_Doc*_tmp5EA[6];s=((_tmp5EA[5]=_tmp11B == 0?
Cyc_Absynpp_question(): Cyc_Absynpp_kind2doc((void*)_tmp11B->v),((_tmp5EA[4]=Cyc_PP_text(((
_tmp5F0=")::",_tag_dyneither(_tmp5F0,sizeof(char),4)))),((_tmp5EA[3]=(!Cyc_Absynpp_print_full_evars
 || _tmp11E == 0)?Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)
_tmp11E->v),((_tmp5EA[2]=Cyc_PP_text((struct _dyneither_ptr)((_tmp5EF.tag=1,((
_tmp5EF.f1=(unsigned long)_tmp11D,((_tmp5EE[0]=& _tmp5EF,Cyc_aprintf(((_tmp5ED="%d",
_tag_dyneither(_tmp5ED,sizeof(char),3))),_tag_dyneither(_tmp5EE,sizeof(void*),1))))))))),((
_tmp5EA[1]=Cyc_PP_text(((_tmp5EC="(",_tag_dyneither(_tmp5EC,sizeof(char),2)))),((
_tmp5EA[0]=Cyc_PP_text(((_tmp5EB="%",_tag_dyneither(_tmp5EB,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp5EA,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
goto _LLCD;_LLD8: if(*((int*)_tmp11A)!= 1)goto _LLDA;_tmp11F=((struct Cyc_Absyn_VarType_struct*)
_tmp11A)->f1;_LLD9: s=Cyc_PP_textptr(_tmp11F->name);if(Cyc_Absynpp_print_all_kinds){
const char*_tmp5F3;struct Cyc_PP_Doc*_tmp5F2[3];s=((_tmp5F2[2]=Cyc_Absynpp_kindbound2doc((
void*)_tmp11F->kind),((_tmp5F2[1]=Cyc_PP_text(((_tmp5F3="::",_tag_dyneither(
_tmp5F3,sizeof(char),3)))),((_tmp5F2[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp5F2,
sizeof(struct Cyc_PP_Doc*),3))))))));}if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(
_tmp11F)){const char*_tmp5F8;const char*_tmp5F7;struct Cyc_PP_Doc*_tmp5F6[3];s=((
_tmp5F6[2]=Cyc_PP_text(((_tmp5F8=" */",_tag_dyneither(_tmp5F8,sizeof(char),4)))),((
_tmp5F6[1]=s,((_tmp5F6[0]=Cyc_PP_text(((_tmp5F7="_ /* ",_tag_dyneither(_tmp5F7,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp5F6,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLDA: if(*((int*)_tmp11A)!= 2)goto _LLDC;_tmp120=((struct Cyc_Absyn_TunionType_struct*)
_tmp11A)->f1;_tmp121=_tmp120.tunion_info;_tmp122=_tmp120.targs;_tmp123=_tmp120.rgn;
_LLDB:{union Cyc_Absyn_TunionInfoU_union _tmp149=_tmp121;struct Cyc_Absyn_UnknownTunionInfo
_tmp14A;struct _tuple0*_tmp14B;int _tmp14C;struct Cyc_Absyn_Tuniondecl**_tmp14D;
struct Cyc_Absyn_Tuniondecl*_tmp14E;struct Cyc_Absyn_Tuniondecl _tmp14F;struct
_tuple0*_tmp150;int _tmp151;_LL103: if((_tmp149.UnknownTunion).tag != 0)goto _LL105;
_tmp14A=(_tmp149.UnknownTunion).f1;_tmp14B=_tmp14A.name;_tmp14C=_tmp14A.is_xtunion;
_LL104: _tmp150=_tmp14B;_tmp151=_tmp14C;goto _LL106;_LL105: if((_tmp149.KnownTunion).tag
!= 1)goto _LL102;_tmp14D=(_tmp149.KnownTunion).f1;_tmp14E=*_tmp14D;_tmp14F=*
_tmp14E;_tmp150=_tmp14F.name;_tmp151=_tmp14F.is_xtunion;_LL106: {const char*
_tmp5FA;const char*_tmp5F9;struct Cyc_PP_Doc*_tmp152=Cyc_PP_text(_tmp151?(_tmp5FA="xtunion ",
_tag_dyneither(_tmp5FA,sizeof(char),9)):((_tmp5F9="tunion ",_tag_dyneither(
_tmp5F9,sizeof(char),8))));void*r=(unsigned int)_tmp123?(void*)_tmp123->v:(void*)
2;{void*_tmp153=Cyc_Tcutil_compress(r);_LL108: if((int)_tmp153 != 2)goto _LL10A;
_LL109:{struct Cyc_PP_Doc*_tmp5FB[3];s=((_tmp5FB[2]=Cyc_Absynpp_tps2doc(_tmp122),((
_tmp5FB[1]=Cyc_Absynpp_qvar2doc(_tmp150),((_tmp5FB[0]=_tmp152,Cyc_PP_cat(
_tag_dyneither(_tmp5FB,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL107;_LL10A:;
_LL10B:{const char*_tmp5FE;struct Cyc_PP_Doc*_tmp5FD[5];s=((_tmp5FD[4]=Cyc_Absynpp_tps2doc(
_tmp122),((_tmp5FD[3]=Cyc_Absynpp_qvar2doc(_tmp150),((_tmp5FD[2]=Cyc_PP_text(((
_tmp5FE=" ",_tag_dyneither(_tmp5FE,sizeof(char),2)))),((_tmp5FD[1]=Cyc_Absynpp_typ2doc(
r),((_tmp5FD[0]=_tmp152,Cyc_PP_cat(_tag_dyneither(_tmp5FD,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL107;_LL107:;}goto _LL102;}_LL102:;}goto _LLCD;_LLDC: if(*((
int*)_tmp11A)!= 3)goto _LLDE;_tmp124=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp11A)->f1;_tmp125=_tmp124.field_info;_tmp126=_tmp124.targs;_LLDD:{union Cyc_Absyn_TunionFieldInfoU_union
_tmp159=_tmp125;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp15A;struct _tuple0*
_tmp15B;struct _tuple0*_tmp15C;int _tmp15D;struct Cyc_Absyn_Tuniondecl*_tmp15E;
struct Cyc_Absyn_Tuniondecl _tmp15F;struct _tuple0*_tmp160;int _tmp161;struct Cyc_Absyn_Tunionfield*
_tmp162;struct Cyc_Absyn_Tunionfield _tmp163;struct _tuple0*_tmp164;_LL10D: if((
_tmp159.UnknownTunionfield).tag != 0)goto _LL10F;_tmp15A=(_tmp159.UnknownTunionfield).f1;
_tmp15B=_tmp15A.tunion_name;_tmp15C=_tmp15A.field_name;_tmp15D=_tmp15A.is_xtunion;
_LL10E: _tmp160=_tmp15B;_tmp161=_tmp15D;_tmp164=_tmp15C;goto _LL110;_LL10F: if((
_tmp159.KnownTunionfield).tag != 1)goto _LL10C;_tmp15E=(_tmp159.KnownTunionfield).f1;
_tmp15F=*_tmp15E;_tmp160=_tmp15F.name;_tmp161=_tmp15F.is_xtunion;_tmp162=(
_tmp159.KnownTunionfield).f2;_tmp163=*_tmp162;_tmp164=_tmp163.name;_LL110: {
const char*_tmp600;const char*_tmp5FF;struct Cyc_PP_Doc*_tmp165=Cyc_PP_text(_tmp161?(
_tmp600="xtunion ",_tag_dyneither(_tmp600,sizeof(char),9)):((_tmp5FF="tunion ",
_tag_dyneither(_tmp5FF,sizeof(char),8))));{const char*_tmp603;struct Cyc_PP_Doc*
_tmp602[4];s=((_tmp602[3]=Cyc_Absynpp_qvar2doc(_tmp164),((_tmp602[2]=Cyc_PP_text(((
_tmp603=".",_tag_dyneither(_tmp603,sizeof(char),2)))),((_tmp602[1]=Cyc_Absynpp_qvar2doc(
_tmp160),((_tmp602[0]=_tmp165,Cyc_PP_cat(_tag_dyneither(_tmp602,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LL10C;}_LL10C:;}goto _LLCD;_LLDE: if(*((int*)_tmp11A)!= 5)goto
_LLE0;_tmp127=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp11A)->f1;_tmp128=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp11A)->f2;_LLDF: {struct
_dyneither_ptr sns;struct _dyneither_ptr ts;{void*_tmp16A=_tmp127;_LL112: if((int)
_tmp16A != 2)goto _LL114;_LL113: goto _LL115;_LL114: if((int)_tmp16A != 0)goto _LL116;
_LL115:{const char*_tmp604;sns=((_tmp604="",_tag_dyneither(_tmp604,sizeof(char),1)));}
goto _LL111;_LL116: if((int)_tmp16A != 1)goto _LL111;_LL117:{const char*_tmp605;sns=((
_tmp605="unsigned ",_tag_dyneither(_tmp605,sizeof(char),10)));}goto _LL111;_LL111:;}{
void*_tmp16D=_tmp128;_LL119: if((int)_tmp16D != 0)goto _LL11B;_LL11A:{void*_tmp16E=
_tmp127;_LL124: if((int)_tmp16E != 2)goto _LL126;_LL125:{const char*_tmp606;sns=((
_tmp606="",_tag_dyneither(_tmp606,sizeof(char),1)));}goto _LL123;_LL126: if((int)
_tmp16E != 0)goto _LL128;_LL127:{const char*_tmp607;sns=((_tmp607="signed ",
_tag_dyneither(_tmp607,sizeof(char),8)));}goto _LL123;_LL128: if((int)_tmp16E != 1)
goto _LL123;_LL129:{const char*_tmp608;sns=((_tmp608="unsigned ",_tag_dyneither(
_tmp608,sizeof(char),10)));}goto _LL123;_LL123:;}{const char*_tmp609;ts=((_tmp609="char",
_tag_dyneither(_tmp609,sizeof(char),5)));}goto _LL118;_LL11B: if((int)_tmp16D != 1)
goto _LL11D;_LL11C:{const char*_tmp60A;ts=((_tmp60A="short",_tag_dyneither(_tmp60A,
sizeof(char),6)));}goto _LL118;_LL11D: if((int)_tmp16D != 2)goto _LL11F;_LL11E:{
const char*_tmp60B;ts=((_tmp60B="int",_tag_dyneither(_tmp60B,sizeof(char),4)));}
goto _LL118;_LL11F: if((int)_tmp16D != 3)goto _LL121;_LL120:{const char*_tmp60C;ts=((
_tmp60C="long",_tag_dyneither(_tmp60C,sizeof(char),5)));}goto _LL118;_LL121: if((
int)_tmp16D != 4)goto _LL118;_LL122:{void*_tmp176=Cyc_Cyclone_c_compiler;_LL12B:
if((int)_tmp176 != 0)goto _LL12D;_LL12C:{const char*_tmp60D;ts=((_tmp60D="long long",
_tag_dyneither(_tmp60D,sizeof(char),10)));}goto _LL12A;_LL12D: if((int)_tmp176 != 1)
goto _LL12A;_LL12E:{const char*_tmp60E;ts=((_tmp60E="__int64",_tag_dyneither(
_tmp60E,sizeof(char),8)));}goto _LL12A;_LL12A:;}goto _LL118;_LL118:;}{const char*
_tmp613;void*_tmp612[2];struct Cyc_String_pa_struct _tmp611;struct Cyc_String_pa_struct
_tmp610;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp610.tag=0,((_tmp610.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)ts),((_tmp611.tag=0,((_tmp611.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)sns),((_tmp612[0]=& _tmp611,((_tmp612[1]=&
_tmp610,Cyc_aprintf(((_tmp613="%s%s",_tag_dyneither(_tmp613,sizeof(char),5))),
_tag_dyneither(_tmp612,sizeof(void*),2)))))))))))))));}goto _LLCD;}_LLE0: if((int)
_tmp11A != 1)goto _LLE2;_LLE1:{const char*_tmp614;s=Cyc_PP_text(((_tmp614="float",
_tag_dyneither(_tmp614,sizeof(char),6))));}goto _LLCD;_LLE2: if(_tmp11A <= (void*)4)
goto _LLF8;if(*((int*)_tmp11A)!= 6)goto _LLE4;_tmp129=((struct Cyc_Absyn_DoubleType_struct*)
_tmp11A)->f1;_LLE3: if(_tmp129){const char*_tmp615;s=Cyc_PP_text(((_tmp615="long double",
_tag_dyneither(_tmp615,sizeof(char),12))));}else{const char*_tmp616;s=Cyc_PP_text(((
_tmp616="double",_tag_dyneither(_tmp616,sizeof(char),7))));}goto _LLCD;_LLE4: if(*((
int*)_tmp11A)!= 9)goto _LLE6;_tmp12A=((struct Cyc_Absyn_TupleType_struct*)_tmp11A)->f1;
_LLE5:{struct Cyc_PP_Doc*_tmp617[2];s=((_tmp617[1]=Cyc_Absynpp_args2doc(_tmp12A),((
_tmp617[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp617,sizeof(struct
Cyc_PP_Doc*),2))))));}goto _LLCD;_LLE6: if(*((int*)_tmp11A)!= 10)goto _LLE8;_tmp12B=((
struct Cyc_Absyn_AggrType_struct*)_tmp11A)->f1;_tmp12C=_tmp12B.aggr_info;_tmp12D=
_tmp12B.targs;_LLE7: {void*_tmp182;struct _tuple0*_tmp183;struct _tuple3 _tmp181=
Cyc_Absyn_aggr_kinded_name(_tmp12C);_tmp182=_tmp181.f1;_tmp183=_tmp181.f2;{
struct Cyc_PP_Doc*_tmp618[3];s=((_tmp618[2]=Cyc_Absynpp_tps2doc(_tmp12D),((
_tmp618[1]=Cyc_Absynpp_qvar2doc(_tmp183),((_tmp618[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp182),Cyc_PP_cat(_tag_dyneither(_tmp618,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;}_LLE8: if(*((int*)_tmp11A)!= 11)goto _LLEA;_tmp12E=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp11A)->f1;_tmp12F=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp11A)->f2;_LLE9:{
struct Cyc_PP_Doc*_tmp619[4];s=((_tmp619[3]=Cyc_Absynpp_rb(),((_tmp619[2]=Cyc_PP_nest(
2,Cyc_Absynpp_aggrfields2doc(_tmp12F)),((_tmp619[1]=Cyc_Absynpp_lb(),((_tmp619[0]=
Cyc_Absynpp_aggr_kind2doc(_tmp12E),Cyc_PP_cat(_tag_dyneither(_tmp619,sizeof(
struct Cyc_PP_Doc*),4))))))))));}goto _LLCD;_LLEA: if(*((int*)_tmp11A)!= 13)goto
_LLEC;_tmp130=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp11A)->f1;_LLEB:{const
char*_tmp61C;struct Cyc_PP_Doc*_tmp61B[4];s=((_tmp61B[3]=Cyc_Absynpp_rb(),((
_tmp61B[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp130)),((_tmp61B[1]=Cyc_Absynpp_lb(),((
_tmp61B[0]=Cyc_PP_text(((_tmp61C="enum ",_tag_dyneither(_tmp61C,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp61B,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LLCD;_LLEC: if(*((int*)_tmp11A)!= 12)goto _LLEE;_tmp131=((struct Cyc_Absyn_EnumType_struct*)
_tmp11A)->f1;_LLED:{const char*_tmp61F;struct Cyc_PP_Doc*_tmp61E[2];s=((_tmp61E[1]=
Cyc_Absynpp_qvar2doc(_tmp131),((_tmp61E[0]=Cyc_PP_text(((_tmp61F="enum ",
_tag_dyneither(_tmp61F,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp61E,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLCD;_LLEE: if(*((int*)_tmp11A)!= 17)goto
_LLF0;_tmp132=((struct Cyc_Absyn_ValueofType_struct*)_tmp11A)->f1;_LLEF:{const
char*_tmp624;const char*_tmp623;struct Cyc_PP_Doc*_tmp622[3];s=((_tmp622[2]=Cyc_PP_text(((
_tmp624=")",_tag_dyneither(_tmp624,sizeof(char),2)))),((_tmp622[1]=Cyc_Absynpp_exp2doc(
_tmp132),((_tmp622[0]=Cyc_PP_text(((_tmp623="valueof_t(",_tag_dyneither(_tmp623,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp622,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LLCD;_LLF0: if(*((int*)_tmp11A)!= 16)goto _LLF2;_tmp133=((struct
Cyc_Absyn_TypedefType_struct*)_tmp11A)->f1;_tmp134=((struct Cyc_Absyn_TypedefType_struct*)
_tmp11A)->f2;_tmp135=((struct Cyc_Absyn_TypedefType_struct*)_tmp11A)->f3;_LLF1:{
struct Cyc_PP_Doc*_tmp625[2];s=((_tmp625[1]=Cyc_Absynpp_tps2doc(_tmp134),((
_tmp625[0]=Cyc_Absynpp_qvar2doc(_tmp133),Cyc_PP_cat(_tag_dyneither(_tmp625,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLCD;_LLF2: if(*((int*)_tmp11A)!= 14)goto
_LLF4;_tmp136=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp11A)->f1;_LLF3:{
const char*_tmp62A;const char*_tmp629;struct Cyc_PP_Doc*_tmp628[3];s=((_tmp628[2]=
Cyc_PP_text(((_tmp62A=">",_tag_dyneither(_tmp62A,sizeof(char),2)))),((_tmp628[1]=
Cyc_Absynpp_rgn2doc(_tmp136),((_tmp628[0]=Cyc_PP_text(((_tmp629="region_t<",
_tag_dyneither(_tmp629,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp628,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCD;_LLF4: if(*((int*)_tmp11A)!= 15)
goto _LLF6;_tmp137=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp11A)->f1;
_tmp138=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp11A)->f2;_LLF5:{const
char*_tmp631;const char*_tmp630;const char*_tmp62F;struct Cyc_PP_Doc*_tmp62E[5];s=((
_tmp62E[4]=Cyc_PP_text(((_tmp631=">",_tag_dyneither(_tmp631,sizeof(char),2)))),((
_tmp62E[3]=Cyc_Absynpp_rgn2doc(_tmp138),((_tmp62E[2]=Cyc_PP_text(((_tmp630=",",
_tag_dyneither(_tmp630,sizeof(char),2)))),((_tmp62E[1]=Cyc_Absynpp_rgn2doc(
_tmp137),((_tmp62E[0]=Cyc_PP_text(((_tmp62F="dynregion_t<",_tag_dyneither(
_tmp62F,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp62E,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LLCD;_LLF6: if(*((int*)_tmp11A)!= 18)goto _LLF8;_tmp139=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp11A)->f1;_LLF7:{const char*_tmp636;const char*
_tmp635;struct Cyc_PP_Doc*_tmp634[3];s=((_tmp634[2]=Cyc_PP_text(((_tmp636=">",
_tag_dyneither(_tmp636,sizeof(char),2)))),((_tmp634[1]=Cyc_Absynpp_typ2doc(
_tmp139),((_tmp634[0]=Cyc_PP_text(((_tmp635="tag_t<",_tag_dyneither(_tmp635,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp634,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLF8: if((int)_tmp11A != 3)goto _LLFA;_LLF9: goto _LLFB;_LLFA: if((int)
_tmp11A != 2)goto _LLFC;_LLFB: s=Cyc_Absynpp_rgn2doc(t);goto _LLCD;_LLFC: if(_tmp11A
<= (void*)4)goto _LLFE;if(*((int*)_tmp11A)!= 21)goto _LLFE;_tmp13A=(void*)((struct
Cyc_Absyn_RgnsEff_struct*)_tmp11A)->f1;_LLFD:{const char*_tmp63B;const char*
_tmp63A;struct Cyc_PP_Doc*_tmp639[3];s=((_tmp639[2]=Cyc_PP_text(((_tmp63B=")",
_tag_dyneither(_tmp63B,sizeof(char),2)))),((_tmp639[1]=Cyc_Absynpp_typ2doc(
_tmp13A),((_tmp639[0]=Cyc_PP_text(((_tmp63A="regions(",_tag_dyneither(_tmp63A,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp639,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLFE: if(_tmp11A <= (void*)4)goto _LL100;if(*((int*)_tmp11A)!= 19)goto
_LL100;_LLFF: goto _LL101;_LL100: if(_tmp11A <= (void*)4)goto _LLCD;if(*((int*)
_tmp11A)!= 20)goto _LLCD;_LL101: s=Cyc_Absynpp_eff2doc(t);goto _LLCD;_LLCD:;}return
s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo);struct Cyc_PP_Doc*
Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _dyneither_ptr*)vo->v));}struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct
_tuple5*cmp);struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple5*cmp){struct
_tuple5 _tmp19C;void*_tmp19D;void*_tmp19E;struct _tuple5*_tmp19B=cmp;_tmp19C=*
_tmp19B;_tmp19D=_tmp19C.f1;_tmp19E=_tmp19C.f2;{const char*_tmp63E;struct Cyc_PP_Doc*
_tmp63D[3];return(_tmp63D[2]=Cyc_Absynpp_rgn2doc(_tmp19E),((_tmp63D[1]=Cyc_PP_text(((
_tmp63E=" > ",_tag_dyneither(_tmp63E,sizeof(char),4)))),((_tmp63D[0]=Cyc_Absynpp_rgn2doc(
_tmp19D),Cyc_PP_cat(_tag_dyneither(_tmp63D,sizeof(struct Cyc_PP_Doc*),3)))))));}}
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po);struct Cyc_PP_Doc*
Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){const char*_tmp641;const char*
_tmp640;const char*_tmp63F;return Cyc_PP_group(((_tmp63F="",_tag_dyneither(_tmp63F,
sizeof(char),1))),((_tmp640="",_tag_dyneither(_tmp640,sizeof(char),1))),((
_tmp641=",",_tag_dyneither(_tmp641,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,
po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple1*t);struct Cyc_PP_Doc*
Cyc_Absynpp_funarg2doc(struct _tuple1*t){struct Cyc_Core_Opt*_tmp642;struct Cyc_Core_Opt*
dopt=(*t).f1 == 0?0:((_tmp642=_cycalloc(sizeof(*_tmp642)),((_tmp642->v=Cyc_PP_text(*((
struct _dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null((*t).f1))->v)),_tmp642))));
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*
_tmp1A5=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_PP_Doc*
eff_doc;if(c_varargs){const char*_tmp645;struct Cyc_List_List*_tmp644;_tmp1A5=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1A5,((_tmp644=_cycalloc(sizeof(*_tmp644)),((_tmp644->hd=Cyc_PP_text(((
_tmp645="...",_tag_dyneither(_tmp645,sizeof(char),4)))),((_tmp644->tl=0,_tmp644)))))));}
else{if(cyc_varargs != 0){struct _tuple1*_tmp64E;const char*_tmp64D;const char*
_tmp64C;const char*_tmp64B;struct Cyc_PP_Doc*_tmp64A[3];struct Cyc_PP_Doc*_tmp1A8=(
_tmp64A[2]=Cyc_Absynpp_funarg2doc(((_tmp64E=_cycalloc(sizeof(*_tmp64E)),((
_tmp64E->f1=cyc_varargs->name,((_tmp64E->f2=cyc_varargs->tq,((_tmp64E->f3=(void*)
cyc_varargs->type,_tmp64E))))))))),((_tmp64A[1]=cyc_varargs->inject?Cyc_PP_text(((
_tmp64C=" inject ",_tag_dyneither(_tmp64C,sizeof(char),9)))): Cyc_PP_text(((
_tmp64D=" ",_tag_dyneither(_tmp64D,sizeof(char),2)))),((_tmp64A[0]=Cyc_PP_text(((
_tmp64B="...",_tag_dyneither(_tmp64B,sizeof(char),4)))),Cyc_PP_cat(
_tag_dyneither(_tmp64A,sizeof(struct Cyc_PP_Doc*),3)))))));struct Cyc_List_List*
_tmp64F;_tmp1A5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1A5,((_tmp64F=_cycalloc(sizeof(*_tmp64F)),((_tmp64F->hd=
_tmp1A8,((_tmp64F->tl=0,_tmp64F)))))));}}{const char*_tmp652;const char*_tmp651;
const char*_tmp650;struct Cyc_PP_Doc*_tmp1AF=Cyc_PP_group(((_tmp650="",
_tag_dyneither(_tmp650,sizeof(char),1))),((_tmp651="",_tag_dyneither(_tmp651,
sizeof(char),1))),((_tmp652=",",_tag_dyneither(_tmp652,sizeof(char),2))),_tmp1A5);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){const char*_tmp655;struct Cyc_PP_Doc*
_tmp654[3];_tmp1AF=((_tmp654[2]=Cyc_Absynpp_eff2doc((void*)effopt->v),((_tmp654[
1]=Cyc_PP_text(((_tmp655=";",_tag_dyneither(_tmp655,sizeof(char),2)))),((_tmp654[
0]=_tmp1AF,Cyc_PP_cat(_tag_dyneither(_tmp654,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){const char*_tmp658;struct Cyc_PP_Doc*_tmp657[3];_tmp1AF=((_tmp657[2]=
Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp657[1]=Cyc_PP_text(((_tmp658=":",
_tag_dyneither(_tmp658,sizeof(char),2)))),((_tmp657[0]=_tmp1AF,Cyc_PP_cat(
_tag_dyneither(_tmp657,sizeof(struct Cyc_PP_Doc*),3))))))));}{const char*_tmp65D;
const char*_tmp65C;struct Cyc_PP_Doc*_tmp65B[3];return(_tmp65B[2]=Cyc_PP_text(((
_tmp65D=")",_tag_dyneither(_tmp65D,sizeof(char),2)))),((_tmp65B[1]=_tmp1AF,((
_tmp65B[0]=Cyc_PP_text(((_tmp65C="(",_tag_dyneither(_tmp65C,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp65B,sizeof(struct Cyc_PP_Doc*),3)))))));}}}struct
_tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple6*arg);struct _tuple1*Cyc_Absynpp_arg_mk_opt(
struct _tuple6*arg){struct Cyc_Core_Opt*_tmp660;struct _tuple1*_tmp65F;return(
_tmp65F=_cycalloc(sizeof(*_tmp65F)),((_tmp65F->f1=((_tmp660=_cycalloc(sizeof(*
_tmp660)),((_tmp660->v=(*arg).f1,_tmp660)))),((_tmp65F->f2=(*arg).f2,((_tmp65F->f3=(*
arg).f3,_tmp65F)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct
_dyneither_ptr*v);struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){
return Cyc_PP_text(*v);}struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*
q);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){struct Cyc_List_List*
_tmp1BC=0;int match;{union Cyc_Absyn_Nmspace_union _tmp1BD=(*q).f1;struct Cyc_List_List*
_tmp1BE;struct Cyc_List_List*_tmp1BF;_LL130: if((_tmp1BD.Loc_n).tag != 0)goto _LL132;
_LL131: _tmp1BE=0;goto _LL133;_LL132: if((_tmp1BD.Rel_n).tag != 1)goto _LL134;_tmp1BE=(
_tmp1BD.Rel_n).f1;_LL133: match=0;_tmp1BC=_tmp1BE;goto _LL12F;_LL134: if((_tmp1BD.Abs_n).tag
!= 2)goto _LL12F;_tmp1BF=(_tmp1BD.Abs_n).f1;_LL135: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1BF,Cyc_Absynpp_curr_namespace);{
struct Cyc_List_List*_tmp661;_tmp1BC=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(
_tmp661=_cycalloc(sizeof(*_tmp661)),((_tmp661->hd=Cyc_Absynpp_cyc_stringptr,((
_tmp661->tl=_tmp1BF,_tmp661))))): _tmp1BF;}goto _LL12F;_LL12F:;}if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp663;struct Cyc_List_List*_tmp662;return(struct _dyneither_ptr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1BC,((_tmp662=_cycalloc(sizeof(*_tmp662)),((_tmp662->hd=(*q).f2,((_tmp662->tl=
0,_tmp662))))))),((_tmp663="_",_tag_dyneither(_tmp663,sizeof(char),2))));}else{
if(match)return*(*q).f2;else{const char*_tmp665;struct Cyc_List_List*_tmp664;
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1BC,((_tmp664=_cycalloc(sizeof(*
_tmp664)),((_tmp664->hd=(*q).f2,((_tmp664->tl=0,_tmp664))))))),((_tmp665="::",
_tag_dyneither(_tmp665,sizeof(char),3))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){return
Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp1C5=Cyc_Absynpp_qvar2string(q);if(Cyc_PP_tex_output){
const char*_tmp667;const char*_tmp666;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp666="\\textbf{",
_tag_dyneither(_tmp666,sizeof(char),9))),(struct _dyneither_ptr)_tmp1C5),((
_tmp667="}",_tag_dyneither(_tmp667,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp1C5));}else{return Cyc_PP_text(_tmp1C5);}}struct _dyneither_ptr
Cyc_Absynpp_typedef_name2string(struct _tuple0*v);struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(
struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace_union _tmp1C8=(*v).f1;
struct Cyc_List_List*_tmp1C9;struct Cyc_List_List*_tmp1CA;_LL137: if((_tmp1C8.Loc_n).tag
!= 0)goto _LL139;_LL138: goto _LL13A;_LL139: if((_tmp1C8.Rel_n).tag != 1)goto _LL13B;
_tmp1C9=(_tmp1C8.Rel_n).f1;if(_tmp1C9 != 0)goto _LL13B;_LL13A: return*(*v).f2;
_LL13B: if((_tmp1C8.Abs_n).tag != 2)goto _LL13D;_tmp1CA=(_tmp1C8.Abs_n).f1;_LL13C:
if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp1CA,Cyc_Absynpp_curr_namespace)
== 0)return*(*v).f2;else{goto _LL13E;}_LL13D:;_LL13E: {const char*_tmp668;return(
struct _dyneither_ptr)Cyc_strconcat(((_tmp668="/* bad namespace : */ ",
_tag_dyneither(_tmp668,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
v));}_LL136:;}else{return*(*v).f2;}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*
v){struct _dyneither_ptr _tmp1CC=Cyc_Absynpp_typedef_name2string(v);if(Cyc_PP_tex_output){
const char*_tmp66A;const char*_tmp669;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp669="\\textbf{",
_tag_dyneither(_tmp669,sizeof(char),9))),(struct _dyneither_ptr)_tmp1CC),((
_tmp66A="}",_tag_dyneither(_tmp66A,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp1CC));}else{return Cyc_PP_text(_tmp1CC);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e);
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){void*_tmp1CF=(void*)e->r;void*
_tmp1D0;struct Cyc_Absyn_Exp*_tmp1D1;struct Cyc_Absyn_Exp*_tmp1D2;_LL140: if(*((int*)
_tmp1CF)!= 0)goto _LL142;_LL141: goto _LL143;_LL142: if(*((int*)_tmp1CF)!= 1)goto
_LL144;_LL143: goto _LL145;_LL144: if(*((int*)_tmp1CF)!= 2)goto _LL146;_LL145: return
10000;_LL146: if(*((int*)_tmp1CF)!= 3)goto _LL148;_tmp1D0=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1CF)->f1;_LL147: {void*_tmp1D3=_tmp1D0;_LL191: if((int)_tmp1D3 != 0)goto _LL193;
_LL192: return 100;_LL193: if((int)_tmp1D3 != 1)goto _LL195;_LL194: return 110;_LL195:
if((int)_tmp1D3 != 2)goto _LL197;_LL196: return 100;_LL197: if((int)_tmp1D3 != 3)goto
_LL199;_LL198: goto _LL19A;_LL199: if((int)_tmp1D3 != 4)goto _LL19B;_LL19A: return 110;
_LL19B: if((int)_tmp1D3 != 5)goto _LL19D;_LL19C: goto _LL19E;_LL19D: if((int)_tmp1D3 != 
6)goto _LL19F;_LL19E: return 70;_LL19F: if((int)_tmp1D3 != 7)goto _LL1A1;_LL1A0: goto
_LL1A2;_LL1A1: if((int)_tmp1D3 != 8)goto _LL1A3;_LL1A2: goto _LL1A4;_LL1A3: if((int)
_tmp1D3 != 9)goto _LL1A5;_LL1A4: goto _LL1A6;_LL1A5: if((int)_tmp1D3 != 10)goto _LL1A7;
_LL1A6: return 80;_LL1A7: if((int)_tmp1D3 != 11)goto _LL1A9;_LL1A8: goto _LL1AA;_LL1A9:
if((int)_tmp1D3 != 12)goto _LL1AB;_LL1AA: return 130;_LL1AB: if((int)_tmp1D3 != 13)
goto _LL1AD;_LL1AC: return 60;_LL1AD: if((int)_tmp1D3 != 14)goto _LL1AF;_LL1AE: return
40;_LL1AF: if((int)_tmp1D3 != 15)goto _LL1B1;_LL1B0: return 50;_LL1B1: if((int)_tmp1D3
!= 16)goto _LL1B3;_LL1B2: return 90;_LL1B3: if((int)_tmp1D3 != 17)goto _LL1B5;_LL1B4:
return 80;_LL1B5: if((int)_tmp1D3 != 18)goto _LL1B7;_LL1B6: return 80;_LL1B7: if((int)
_tmp1D3 != 19)goto _LL190;_LL1B8: return 140;_LL190:;}_LL148: if(*((int*)_tmp1CF)!= 4)
goto _LL14A;_LL149: return 20;_LL14A: if(*((int*)_tmp1CF)!= 5)goto _LL14C;_LL14B:
return 130;_LL14C: if(*((int*)_tmp1CF)!= 6)goto _LL14E;_LL14D: return 30;_LL14E: if(*((
int*)_tmp1CF)!= 7)goto _LL150;_LL14F: return 35;_LL150: if(*((int*)_tmp1CF)!= 8)goto
_LL152;_LL151: return 30;_LL152: if(*((int*)_tmp1CF)!= 9)goto _LL154;_LL153: return 10;
_LL154: if(*((int*)_tmp1CF)!= 10)goto _LL156;_LL155: goto _LL157;_LL156: if(*((int*)
_tmp1CF)!= 11)goto _LL158;_LL157: return 140;_LL158: if(*((int*)_tmp1CF)!= 12)goto
_LL15A;_LL159: return 130;_LL15A: if(*((int*)_tmp1CF)!= 13)goto _LL15C;_tmp1D1=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1CF)->f1;_LL15B: return Cyc_Absynpp_exp_prec(
_tmp1D1);_LL15C: if(*((int*)_tmp1CF)!= 14)goto _LL15E;_tmp1D2=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1CF)->f1;_LL15D: return Cyc_Absynpp_exp_prec(_tmp1D2);_LL15E: if(*((int*)
_tmp1CF)!= 15)goto _LL160;_LL15F: return 120;_LL160: if(*((int*)_tmp1CF)!= 17)goto
_LL162;_LL161: return 15;_LL162: if(*((int*)_tmp1CF)!= 16)goto _LL164;_LL163: goto
_LL165;_LL164: if(*((int*)_tmp1CF)!= 18)goto _LL166;_LL165: goto _LL167;_LL166: if(*((
int*)_tmp1CF)!= 19)goto _LL168;_LL167: goto _LL169;_LL168: if(*((int*)_tmp1CF)!= 39)
goto _LL16A;_LL169: goto _LL16B;_LL16A: if(*((int*)_tmp1CF)!= 20)goto _LL16C;_LL16B:
goto _LL16D;_LL16C: if(*((int*)_tmp1CF)!= 21)goto _LL16E;_LL16D: goto _LL16F;_LL16E:
if(*((int*)_tmp1CF)!= 22)goto _LL170;_LL16F: return 130;_LL170: if(*((int*)_tmp1CF)
!= 23)goto _LL172;_LL171: goto _LL173;_LL172: if(*((int*)_tmp1CF)!= 24)goto _LL174;
_LL173: goto _LL175;_LL174: if(*((int*)_tmp1CF)!= 25)goto _LL176;_LL175: return 140;
_LL176: if(*((int*)_tmp1CF)!= 26)goto _LL178;_LL177: return 150;_LL178: if(*((int*)
_tmp1CF)!= 27)goto _LL17A;_LL179: goto _LL17B;_LL17A: if(*((int*)_tmp1CF)!= 28)goto
_LL17C;_LL17B: goto _LL17D;_LL17C: if(*((int*)_tmp1CF)!= 29)goto _LL17E;_LL17D: goto
_LL17F;_LL17E: if(*((int*)_tmp1CF)!= 30)goto _LL180;_LL17F: goto _LL181;_LL180: if(*((
int*)_tmp1CF)!= 31)goto _LL182;_LL181: goto _LL183;_LL182: if(*((int*)_tmp1CF)!= 32)
goto _LL184;_LL183: goto _LL185;_LL184: if(*((int*)_tmp1CF)!= 33)goto _LL186;_LL185:
goto _LL187;_LL186: if(*((int*)_tmp1CF)!= 34)goto _LL188;_LL187: goto _LL189;_LL188:
if(*((int*)_tmp1CF)!= 35)goto _LL18A;_LL189: goto _LL18B;_LL18A: if(*((int*)_tmp1CF)
!= 36)goto _LL18C;_LL18B: goto _LL18D;_LL18C: if(*((int*)_tmp1CF)!= 37)goto _LL18E;
_LL18D: return 140;_LL18E: if(*((int*)_tmp1CF)!= 38)goto _LL13F;_LL18F: return 10000;
_LL13F:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){int
myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*s;{void*_tmp1D4=(void*)e->r;
union Cyc_Absyn_Cnst_union _tmp1D5;struct _tuple0*_tmp1D6;struct _tuple0*_tmp1D7;
void*_tmp1D8;struct Cyc_List_List*_tmp1D9;struct Cyc_Absyn_Exp*_tmp1DA;struct Cyc_Core_Opt*
_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DD;void*_tmp1DE;
struct Cyc_Absyn_Exp*_tmp1DF;struct Cyc_Absyn_Exp*_tmp1E0;struct Cyc_Absyn_Exp*
_tmp1E1;struct Cyc_Absyn_Exp*_tmp1E2;struct Cyc_Absyn_Exp*_tmp1E3;struct Cyc_Absyn_Exp*
_tmp1E4;struct Cyc_Absyn_Exp*_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E6;struct Cyc_Absyn_Exp*
_tmp1E7;struct Cyc_Absyn_Exp*_tmp1E8;struct Cyc_List_List*_tmp1E9;struct Cyc_Absyn_Exp*
_tmp1EA;struct Cyc_List_List*_tmp1EB;struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_Absyn_Exp*
_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EE;void*_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F0;
struct Cyc_Absyn_Exp*_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_Absyn_Exp*
_tmp1F3;void*_tmp1F4;struct Cyc_Absyn_Exp*_tmp1F5;void*_tmp1F6;void*_tmp1F7;void*
_tmp1F8;struct _dyneither_ptr*_tmp1F9;void*_tmp1FA;void*_tmp1FB;unsigned int
_tmp1FC;struct Cyc_List_List*_tmp1FD;void*_tmp1FE;struct Cyc_Absyn_Exp*_tmp1FF;
struct Cyc_Absyn_Exp*_tmp200;struct _dyneither_ptr*_tmp201;struct Cyc_Absyn_Exp*
_tmp202;struct _dyneither_ptr*_tmp203;struct Cyc_Absyn_Exp*_tmp204;struct Cyc_Absyn_Exp*
_tmp205;struct Cyc_List_List*_tmp206;struct _tuple1*_tmp207;struct Cyc_List_List*
_tmp208;struct Cyc_List_List*_tmp209;struct Cyc_Absyn_Vardecl*_tmp20A;struct Cyc_Absyn_Exp*
_tmp20B;struct Cyc_Absyn_Exp*_tmp20C;struct _tuple0*_tmp20D;struct Cyc_List_List*
_tmp20E;struct Cyc_List_List*_tmp20F;struct Cyc_List_List*_tmp210;struct Cyc_List_List*
_tmp211;struct Cyc_Absyn_Tunionfield*_tmp212;struct _tuple0*_tmp213;struct _tuple0*
_tmp214;struct Cyc_Absyn_MallocInfo _tmp215;int _tmp216;struct Cyc_Absyn_Exp*_tmp217;
void**_tmp218;struct Cyc_Absyn_Exp*_tmp219;struct Cyc_Absyn_Exp*_tmp21A;struct Cyc_Absyn_Exp*
_tmp21B;struct Cyc_Core_Opt*_tmp21C;struct Cyc_List_List*_tmp21D;struct Cyc_Absyn_Stmt*
_tmp21E;_LL1BA: if(*((int*)_tmp1D4)!= 0)goto _LL1BC;_tmp1D5=((struct Cyc_Absyn_Const_e_struct*)
_tmp1D4)->f1;_LL1BB: s=Cyc_Absynpp_cnst2doc(_tmp1D5);goto _LL1B9;_LL1BC: if(*((int*)
_tmp1D4)!= 1)goto _LL1BE;_tmp1D6=((struct Cyc_Absyn_Var_e_struct*)_tmp1D4)->f1;
_LL1BD: _tmp1D7=_tmp1D6;goto _LL1BF;_LL1BE: if(*((int*)_tmp1D4)!= 2)goto _LL1C0;
_tmp1D7=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp1D4)->f1;_LL1BF: s=Cyc_Absynpp_qvar2doc(
_tmp1D7);goto _LL1B9;_LL1C0: if(*((int*)_tmp1D4)!= 3)goto _LL1C2;_tmp1D8=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp1D4)->f1;_tmp1D9=((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D4)->f2;_LL1C1: s=Cyc_Absynpp_primapp2doc(myprec,_tmp1D8,_tmp1D9);goto _LL1B9;
_LL1C2: if(*((int*)_tmp1D4)!= 4)goto _LL1C4;_tmp1DA=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1D4)->f1;_tmp1DB=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1D4)->f2;_tmp1DC=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp1D4)->f3;_LL1C3:{const char*_tmp66F;const
char*_tmp66E;struct Cyc_PP_Doc*_tmp66D[5];s=((_tmp66D[4]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1DC),((_tmp66D[3]=Cyc_PP_text(((_tmp66F="= ",_tag_dyneither(_tmp66F,
sizeof(char),3)))),((_tmp66D[2]=_tmp1DB == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((
void*)_tmp1DB->v),((_tmp66D[1]=Cyc_PP_text(((_tmp66E=" ",_tag_dyneither(_tmp66E,
sizeof(char),2)))),((_tmp66D[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DA),Cyc_PP_cat(
_tag_dyneither(_tmp66D,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1B9;
_LL1C4: if(*((int*)_tmp1D4)!= 5)goto _LL1C6;_tmp1DD=((struct Cyc_Absyn_Increment_e_struct*)
_tmp1D4)->f1;_tmp1DE=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp1D4)->f2;
_LL1C5: {struct Cyc_PP_Doc*_tmp222=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DD);{void*
_tmp223=_tmp1DE;_LL20D: if((int)_tmp223 != 0)goto _LL20F;_LL20E:{const char*_tmp672;
struct Cyc_PP_Doc*_tmp671[2];s=((_tmp671[1]=_tmp222,((_tmp671[0]=Cyc_PP_text(((
_tmp672="++",_tag_dyneither(_tmp672,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(
_tmp671,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL20C;_LL20F: if((int)_tmp223 != 2)
goto _LL211;_LL210:{const char*_tmp675;struct Cyc_PP_Doc*_tmp674[2];s=((_tmp674[1]=
_tmp222,((_tmp674[0]=Cyc_PP_text(((_tmp675="--",_tag_dyneither(_tmp675,sizeof(
char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp674,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL20C;_LL211: if((int)_tmp223 != 1)goto _LL213;_LL212:{const char*_tmp678;
struct Cyc_PP_Doc*_tmp677[2];s=((_tmp677[1]=Cyc_PP_text(((_tmp678="++",
_tag_dyneither(_tmp678,sizeof(char),3)))),((_tmp677[0]=_tmp222,Cyc_PP_cat(
_tag_dyneither(_tmp677,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL20C;_LL213: if((
int)_tmp223 != 3)goto _LL20C;_LL214:{const char*_tmp67B;struct Cyc_PP_Doc*_tmp67A[2];
s=((_tmp67A[1]=Cyc_PP_text(((_tmp67B="--",_tag_dyneither(_tmp67B,sizeof(char),3)))),((
_tmp67A[0]=_tmp222,Cyc_PP_cat(_tag_dyneither(_tmp67A,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL20C;_LL20C:;}goto _LL1B9;}_LL1C6: if(*((int*)_tmp1D4)!= 6)goto _LL1C8;
_tmp1DF=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1D4)->f1;_tmp1E0=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1D4)->f2;_tmp1E1=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1D4)->f3;_LL1C7:{
const char*_tmp680;const char*_tmp67F;struct Cyc_PP_Doc*_tmp67E[5];s=((_tmp67E[4]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E1),((_tmp67E[3]=Cyc_PP_text(((_tmp680=" : ",
_tag_dyneither(_tmp680,sizeof(char),4)))),((_tmp67E[2]=Cyc_Absynpp_exp2doc_prec(
0,_tmp1E0),((_tmp67E[1]=Cyc_PP_text(((_tmp67F=" ? ",_tag_dyneither(_tmp67F,
sizeof(char),4)))),((_tmp67E[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DF),Cyc_PP_cat(
_tag_dyneither(_tmp67E,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1B9;
_LL1C8: if(*((int*)_tmp1D4)!= 7)goto _LL1CA;_tmp1E2=((struct Cyc_Absyn_And_e_struct*)
_tmp1D4)->f1;_tmp1E3=((struct Cyc_Absyn_And_e_struct*)_tmp1D4)->f2;_LL1C9:{const
char*_tmp683;struct Cyc_PP_Doc*_tmp682[3];s=((_tmp682[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E3),((_tmp682[1]=Cyc_PP_text(((_tmp683=" && ",_tag_dyneither(_tmp683,
sizeof(char),5)))),((_tmp682[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E2),Cyc_PP_cat(
_tag_dyneither(_tmp682,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1CA:
if(*((int*)_tmp1D4)!= 8)goto _LL1CC;_tmp1E4=((struct Cyc_Absyn_Or_e_struct*)
_tmp1D4)->f1;_tmp1E5=((struct Cyc_Absyn_Or_e_struct*)_tmp1D4)->f2;_LL1CB:{const
char*_tmp686;struct Cyc_PP_Doc*_tmp685[3];s=((_tmp685[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E5),((_tmp685[1]=Cyc_PP_text(((_tmp686=" || ",_tag_dyneither(_tmp686,
sizeof(char),5)))),((_tmp685[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E4),Cyc_PP_cat(
_tag_dyneither(_tmp685,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1CC:
if(*((int*)_tmp1D4)!= 9)goto _LL1CE;_tmp1E6=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1D4)->f1;_tmp1E7=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1D4)->f2;_LL1CD:{
const char*_tmp68D;const char*_tmp68C;const char*_tmp68B;struct Cyc_PP_Doc*_tmp68A[5];
s=((_tmp68A[4]=Cyc_PP_text(((_tmp68D=")",_tag_dyneither(_tmp68D,sizeof(char),2)))),((
_tmp68A[3]=Cyc_Absynpp_exp2doc(_tmp1E7),((_tmp68A[2]=Cyc_PP_text(((_tmp68C=", ",
_tag_dyneither(_tmp68C,sizeof(char),3)))),((_tmp68A[1]=Cyc_Absynpp_exp2doc(
_tmp1E6),((_tmp68A[0]=Cyc_PP_text(((_tmp68B="(",_tag_dyneither(_tmp68B,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp68A,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1B9;_LL1CE: if(*((int*)_tmp1D4)!= 10)goto _LL1D0;_tmp1E8=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp1D4)->f1;_tmp1E9=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1D4)->f2;_LL1CF:{
const char*_tmp692;const char*_tmp691;struct Cyc_PP_Doc*_tmp690[4];s=((_tmp690[3]=
Cyc_PP_text(((_tmp692=")",_tag_dyneither(_tmp692,sizeof(char),2)))),((_tmp690[2]=
Cyc_Absynpp_exps2doc_prec(20,_tmp1E9),((_tmp690[1]=Cyc_PP_text(((_tmp691="(",
_tag_dyneither(_tmp691,sizeof(char),2)))),((_tmp690[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E8),Cyc_PP_cat(_tag_dyneither(_tmp690,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1B9;_LL1D0: if(*((int*)_tmp1D4)!= 11)goto _LL1D2;_tmp1EA=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1D4)->f1;_tmp1EB=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1D4)->f2;_LL1D1:{
const char*_tmp697;const char*_tmp696;struct Cyc_PP_Doc*_tmp695[4];s=((_tmp695[3]=
Cyc_PP_text(((_tmp697=")",_tag_dyneither(_tmp697,sizeof(char),2)))),((_tmp695[2]=
Cyc_Absynpp_exps2doc_prec(20,_tmp1EB),((_tmp695[1]=Cyc_PP_text(((_tmp696="(",
_tag_dyneither(_tmp696,sizeof(char),2)))),((_tmp695[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EA),Cyc_PP_cat(_tag_dyneither(_tmp695,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1B9;_LL1D2: if(*((int*)_tmp1D4)!= 12)goto _LL1D4;_tmp1EC=((struct Cyc_Absyn_Throw_e_struct*)
_tmp1D4)->f1;_LL1D3:{const char*_tmp69A;struct Cyc_PP_Doc*_tmp699[2];s=((_tmp699[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1EC),((_tmp699[0]=Cyc_PP_text(((_tmp69A="throw ",
_tag_dyneither(_tmp69A,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp699,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1B9;_LL1D4: if(*((int*)_tmp1D4)!= 13)
goto _LL1D6;_tmp1ED=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D4)->f1;_LL1D5:
s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1ED);goto _LL1B9;_LL1D6: if(*((int*)_tmp1D4)
!= 14)goto _LL1D8;_tmp1EE=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1D4)->f1;
_LL1D7: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1EE);goto _LL1B9;_LL1D8: if(*((int*)
_tmp1D4)!= 15)goto _LL1DA;_tmp1EF=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1D4)->f1;
_tmp1F0=((struct Cyc_Absyn_Cast_e_struct*)_tmp1D4)->f2;_LL1D9:{const char*_tmp69F;
const char*_tmp69E;struct Cyc_PP_Doc*_tmp69D[4];s=((_tmp69D[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F0),((_tmp69D[2]=Cyc_PP_text(((_tmp69F=")",_tag_dyneither(_tmp69F,
sizeof(char),2)))),((_tmp69D[1]=Cyc_Absynpp_typ2doc(_tmp1EF),((_tmp69D[0]=Cyc_PP_text(((
_tmp69E="(",_tag_dyneither(_tmp69E,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp69D,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1B9;_LL1DA: if(*((int*)
_tmp1D4)!= 16)goto _LL1DC;_tmp1F1=((struct Cyc_Absyn_Address_e_struct*)_tmp1D4)->f1;
_LL1DB:{const char*_tmp6A2;struct Cyc_PP_Doc*_tmp6A1[2];s=((_tmp6A1[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F1),((_tmp6A1[0]=Cyc_PP_text(((_tmp6A2="&",_tag_dyneither(_tmp6A2,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6A1,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1B9;_LL1DC: if(*((int*)_tmp1D4)!= 17)goto _LL1DE;_tmp1F2=((struct Cyc_Absyn_New_e_struct*)
_tmp1D4)->f1;_tmp1F3=((struct Cyc_Absyn_New_e_struct*)_tmp1D4)->f2;_LL1DD: if(
_tmp1F2 == 0){const char*_tmp6A5;struct Cyc_PP_Doc*_tmp6A4[2];s=((_tmp6A4[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F3),((_tmp6A4[0]=Cyc_PP_text(((_tmp6A5="new ",_tag_dyneither(_tmp6A5,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp6A4,sizeof(struct Cyc_PP_Doc*),2))))));}
else{const char*_tmp6AA;const char*_tmp6A9;struct Cyc_PP_Doc*_tmp6A8[4];s=((_tmp6A8[
3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F3),((_tmp6A8[2]=Cyc_PP_text(((_tmp6AA=") ",
_tag_dyneither(_tmp6AA,sizeof(char),3)))),((_tmp6A8[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp1F2),((_tmp6A8[0]=Cyc_PP_text(((_tmp6A9="rnew(",
_tag_dyneither(_tmp6A9,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp6A8,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1B9;_LL1DE: if(*((int*)_tmp1D4)!= 
18)goto _LL1E0;_tmp1F4=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp1D4)->f1;
_LL1DF:{const char*_tmp6AF;const char*_tmp6AE;struct Cyc_PP_Doc*_tmp6AD[3];s=((
_tmp6AD[2]=Cyc_PP_text(((_tmp6AF=")",_tag_dyneither(_tmp6AF,sizeof(char),2)))),((
_tmp6AD[1]=Cyc_Absynpp_typ2doc(_tmp1F4),((_tmp6AD[0]=Cyc_PP_text(((_tmp6AE="sizeof(",
_tag_dyneither(_tmp6AE,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp6AD,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1E0: if(*((int*)_tmp1D4)!= 19)
goto _LL1E2;_tmp1F5=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp1D4)->f1;_LL1E1:{
const char*_tmp6B4;const char*_tmp6B3;struct Cyc_PP_Doc*_tmp6B2[3];s=((_tmp6B2[2]=
Cyc_PP_text(((_tmp6B4=")",_tag_dyneither(_tmp6B4,sizeof(char),2)))),((_tmp6B2[1]=
Cyc_Absynpp_exp2doc(_tmp1F5),((_tmp6B2[0]=Cyc_PP_text(((_tmp6B3="sizeof(",
_tag_dyneither(_tmp6B3,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp6B2,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1E2: if(*((int*)_tmp1D4)!= 39)
goto _LL1E4;_tmp1F6=(void*)((struct Cyc_Absyn_Valueof_e_struct*)_tmp1D4)->f1;
_LL1E3:{const char*_tmp6B9;const char*_tmp6B8;struct Cyc_PP_Doc*_tmp6B7[3];s=((
_tmp6B7[2]=Cyc_PP_text(((_tmp6B9=")",_tag_dyneither(_tmp6B9,sizeof(char),2)))),((
_tmp6B7[1]=Cyc_Absynpp_typ2doc(_tmp1F6),((_tmp6B7[0]=Cyc_PP_text(((_tmp6B8="valueof(",
_tag_dyneither(_tmp6B8,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp6B7,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1E4: if(*((int*)_tmp1D4)!= 20)
goto _LL1E6;_tmp1F7=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D4)->f1;
_tmp1F8=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D4)->f2;if(*((int*)
_tmp1F8)!= 0)goto _LL1E6;_tmp1F9=((struct Cyc_Absyn_StructField_struct*)_tmp1F8)->f1;
_LL1E5:{const char*_tmp6C0;const char*_tmp6BF;const char*_tmp6BE;struct Cyc_PP_Doc*
_tmp6BD[5];s=((_tmp6BD[4]=Cyc_PP_text(((_tmp6C0=")",_tag_dyneither(_tmp6C0,
sizeof(char),2)))),((_tmp6BD[3]=Cyc_PP_textptr(_tmp1F9),((_tmp6BD[2]=Cyc_PP_text(((
_tmp6BF=",",_tag_dyneither(_tmp6BF,sizeof(char),2)))),((_tmp6BD[1]=Cyc_Absynpp_typ2doc(
_tmp1F7),((_tmp6BD[0]=Cyc_PP_text(((_tmp6BE="offsetof(",_tag_dyneither(_tmp6BE,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp6BD,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL1B9;_LL1E6: if(*((int*)_tmp1D4)!= 20)goto _LL1E8;_tmp1FA=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D4)->f1;_tmp1FB=(void*)((struct
Cyc_Absyn_Offsetof_e_struct*)_tmp1D4)->f2;if(*((int*)_tmp1FB)!= 1)goto _LL1E8;
_tmp1FC=((struct Cyc_Absyn_TupleIndex_struct*)_tmp1FB)->f1;_LL1E7:{const char*
_tmp6CE;struct Cyc_Int_pa_struct _tmp6CD;void*_tmp6CC[1];const char*_tmp6CB;const
char*_tmp6CA;const char*_tmp6C9;struct Cyc_PP_Doc*_tmp6C8[5];s=((_tmp6C8[4]=Cyc_PP_text(((
_tmp6CE=")",_tag_dyneither(_tmp6CE,sizeof(char),2)))),((_tmp6C8[3]=Cyc_PP_text((
struct _dyneither_ptr)((_tmp6CD.tag=1,((_tmp6CD.f1=(unsigned long)((int)_tmp1FC),((
_tmp6CC[0]=& _tmp6CD,Cyc_aprintf(((_tmp6CB="%d",_tag_dyneither(_tmp6CB,sizeof(
char),3))),_tag_dyneither(_tmp6CC,sizeof(void*),1))))))))),((_tmp6C8[2]=Cyc_PP_text(((
_tmp6CA=",",_tag_dyneither(_tmp6CA,sizeof(char),2)))),((_tmp6C8[1]=Cyc_Absynpp_typ2doc(
_tmp1FA),((_tmp6C8[0]=Cyc_PP_text(((_tmp6C9="offsetof(",_tag_dyneither(_tmp6C9,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp6C8,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL1B9;_LL1E8: if(*((int*)_tmp1D4)!= 21)goto _LL1EA;_tmp1FD=((
struct Cyc_Absyn_Gentyp_e_struct*)_tmp1D4)->f1;_tmp1FE=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)
_tmp1D4)->f2;_LL1E9:{const char*_tmp6D3;const char*_tmp6D2;struct Cyc_PP_Doc*
_tmp6D1[4];s=((_tmp6D1[3]=Cyc_PP_text(((_tmp6D3=")",_tag_dyneither(_tmp6D3,
sizeof(char),2)))),((_tmp6D1[2]=Cyc_Absynpp_typ2doc(_tmp1FE),((_tmp6D1[1]=Cyc_Absynpp_tvars2doc(
_tmp1FD),((_tmp6D1[0]=Cyc_PP_text(((_tmp6D2="__gen(",_tag_dyneither(_tmp6D2,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp6D1,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1B9;_LL1EA: if(*((int*)_tmp1D4)!= 22)goto _LL1EC;_tmp1FF=((struct Cyc_Absyn_Deref_e_struct*)
_tmp1D4)->f1;_LL1EB:{const char*_tmp6D6;struct Cyc_PP_Doc*_tmp6D5[2];s=((_tmp6D5[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1FF),((_tmp6D5[0]=Cyc_PP_text(((_tmp6D6="*",
_tag_dyneither(_tmp6D6,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6D5,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1B9;_LL1EC: if(*((int*)_tmp1D4)!= 23)
goto _LL1EE;_tmp200=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D4)->f1;_tmp201=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D4)->f2;_LL1ED:{const char*_tmp6D9;
struct Cyc_PP_Doc*_tmp6D8[3];s=((_tmp6D8[2]=Cyc_PP_textptr(_tmp201),((_tmp6D8[1]=
Cyc_PP_text(((_tmp6D9=".",_tag_dyneither(_tmp6D9,sizeof(char),2)))),((_tmp6D8[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp200),Cyc_PP_cat(_tag_dyneither(_tmp6D8,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1EE: if(*((int*)_tmp1D4)!= 24)
goto _LL1F0;_tmp202=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1D4)->f1;_tmp203=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1D4)->f2;_LL1EF:{const char*_tmp6DC;
struct Cyc_PP_Doc*_tmp6DB[3];s=((_tmp6DB[2]=Cyc_PP_textptr(_tmp203),((_tmp6DB[1]=
Cyc_PP_text(((_tmp6DC="->",_tag_dyneither(_tmp6DC,sizeof(char),3)))),((_tmp6DB[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp202),Cyc_PP_cat(_tag_dyneither(_tmp6DB,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1F0: if(*((int*)_tmp1D4)!= 25)
goto _LL1F2;_tmp204=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1D4)->f1;_tmp205=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp1D4)->f2;_LL1F1:{const char*_tmp6E1;const
char*_tmp6E0;struct Cyc_PP_Doc*_tmp6DF[4];s=((_tmp6DF[3]=Cyc_PP_text(((_tmp6E1="]",
_tag_dyneither(_tmp6E1,sizeof(char),2)))),((_tmp6DF[2]=Cyc_Absynpp_exp2doc(
_tmp205),((_tmp6DF[1]=Cyc_PP_text(((_tmp6E0="[",_tag_dyneither(_tmp6E0,sizeof(
char),2)))),((_tmp6DF[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp204),Cyc_PP_cat(
_tag_dyneither(_tmp6DF,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1B9;_LL1F2:
if(*((int*)_tmp1D4)!= 26)goto _LL1F4;_tmp206=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp1D4)->f1;_LL1F3:{const char*_tmp6E6;const char*_tmp6E5;struct Cyc_PP_Doc*
_tmp6E4[4];s=((_tmp6E4[3]=Cyc_PP_text(((_tmp6E6=")",_tag_dyneither(_tmp6E6,
sizeof(char),2)))),((_tmp6E4[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp206),((_tmp6E4[
1]=Cyc_PP_text(((_tmp6E5="(",_tag_dyneither(_tmp6E5,sizeof(char),2)))),((_tmp6E4[
0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp6E4,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LL1B9;_LL1F4: if(*((int*)_tmp1D4)!= 27)goto _LL1F6;_tmp207=((
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp1D4)->f1;_tmp208=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp1D4)->f2;_LL1F5:{const char*_tmp6ED;const char*_tmp6EC;const char*_tmp6EB;
struct Cyc_PP_Doc*_tmp6EA[4];s=((_tmp6EA[3]=Cyc_Absynpp_group_braces(((_tmp6ED=",",
_tag_dyneither(_tmp6ED,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp208)),((_tmp6EA[2]=Cyc_PP_text(((_tmp6EC=")",_tag_dyneither(_tmp6EC,sizeof(
char),2)))),((_tmp6EA[1]=Cyc_Absynpp_typ2doc((*_tmp207).f3),((_tmp6EA[0]=Cyc_PP_text(((
_tmp6EB="(",_tag_dyneither(_tmp6EB,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp6EA,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1B9;_LL1F6: if(*((int*)
_tmp1D4)!= 28)goto _LL1F8;_tmp209=((struct Cyc_Absyn_Array_e_struct*)_tmp1D4)->f1;
_LL1F7:{const char*_tmp6EE;s=Cyc_Absynpp_group_braces(((_tmp6EE=",",
_tag_dyneither(_tmp6EE,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp209));}goto _LL1B9;_LL1F8: if(*((int*)_tmp1D4)!= 29)goto _LL1FA;_tmp20A=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D4)->f1;_tmp20B=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1D4)->f2;_tmp20C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D4)->f3;
_LL1F9:{const char*_tmp6F5;const char*_tmp6F4;const char*_tmp6F3;struct Cyc_PP_Doc*
_tmp6F2[8];s=((_tmp6F2[7]=Cyc_Absynpp_rb(),((_tmp6F2[6]=Cyc_Absynpp_exp2doc(
_tmp20C),((_tmp6F2[5]=Cyc_PP_text(((_tmp6F5=" : ",_tag_dyneither(_tmp6F5,sizeof(
char),4)))),((_tmp6F2[4]=Cyc_Absynpp_exp2doc(_tmp20B),((_tmp6F2[3]=Cyc_PP_text(((
_tmp6F4=" < ",_tag_dyneither(_tmp6F4,sizeof(char),4)))),((_tmp6F2[2]=Cyc_PP_text(*(*
_tmp20A->name).f2),((_tmp6F2[1]=Cyc_PP_text(((_tmp6F3="for ",_tag_dyneither(
_tmp6F3,sizeof(char),5)))),((_tmp6F2[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp6F2,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}goto _LL1B9;
_LL1FA: if(*((int*)_tmp1D4)!= 30)goto _LL1FC;_tmp20D=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1D4)->f1;_tmp20E=((struct Cyc_Absyn_Struct_e_struct*)_tmp1D4)->f2;_tmp20F=((
struct Cyc_Absyn_Struct_e_struct*)_tmp1D4)->f3;_LL1FB: {struct Cyc_List_List*
_tmp275=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp20F);{const char*_tmp6FA;
struct Cyc_List_List*_tmp6F9;struct Cyc_PP_Doc*_tmp6F8[2];s=((_tmp6F8[1]=Cyc_Absynpp_group_braces(((
_tmp6FA=",",_tag_dyneither(_tmp6FA,sizeof(char),2))),_tmp20E != 0?(_tmp6F9=
_cycalloc(sizeof(*_tmp6F9)),((_tmp6F9->hd=Cyc_Absynpp_tps2doc(_tmp20E),((_tmp6F9->tl=
_tmp275,_tmp6F9))))): _tmp275),((_tmp6F8[0]=Cyc_Absynpp_qvar2doc(_tmp20D),Cyc_PP_cat(
_tag_dyneither(_tmp6F8,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1B9;}_LL1FC: if(*((
int*)_tmp1D4)!= 31)goto _LL1FE;_tmp210=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1D4)->f2;_LL1FD:{const char*_tmp6FB;s=Cyc_Absynpp_group_braces(((_tmp6FB=",",
_tag_dyneither(_tmp6FB,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp210));}goto _LL1B9;_LL1FE: if(*((int*)_tmp1D4)!= 32)goto _LL200;_tmp211=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp1D4)->f1;_tmp212=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp1D4)->f3;_LL1FF: if(_tmp211 == 0)s=Cyc_Absynpp_qvar2doc(_tmp212->name);else{
const char*_tmp702;const char*_tmp701;const char*_tmp700;struct Cyc_PP_Doc*_tmp6FF[2];
s=((_tmp6FF[1]=Cyc_PP_egroup(((_tmp702="(",_tag_dyneither(_tmp702,sizeof(char),2))),((
_tmp701=")",_tag_dyneither(_tmp701,sizeof(char),2))),((_tmp700=",",
_tag_dyneither(_tmp700,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,
_tmp211)),((_tmp6FF[0]=Cyc_Absynpp_qvar2doc(_tmp212->name),Cyc_PP_cat(
_tag_dyneither(_tmp6FF,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1B9;_LL200: if(*((
int*)_tmp1D4)!= 33)goto _LL202;_tmp213=((struct Cyc_Absyn_Enum_e_struct*)_tmp1D4)->f1;
_LL201: s=Cyc_Absynpp_qvar2doc(_tmp213);goto _LL1B9;_LL202: if(*((int*)_tmp1D4)!= 
34)goto _LL204;_tmp214=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp1D4)->f1;_LL203: s=
Cyc_Absynpp_qvar2doc(_tmp214);goto _LL1B9;_LL204: if(*((int*)_tmp1D4)!= 35)goto
_LL206;_tmp215=((struct Cyc_Absyn_Malloc_e_struct*)_tmp1D4)->f1;_tmp216=_tmp215.is_calloc;
_tmp217=_tmp215.rgn;_tmp218=_tmp215.elt_type;_tmp219=_tmp215.num_elts;_LL205: if(
_tmp216){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp218)),0);if(_tmp217 == 0){const char*_tmp709;const char*_tmp708;const char*
_tmp707;struct Cyc_PP_Doc*_tmp706[5];s=((_tmp706[4]=Cyc_PP_text(((_tmp709=")",
_tag_dyneither(_tmp709,sizeof(char),2)))),((_tmp706[3]=Cyc_Absynpp_exp2doc(st),((
_tmp706[2]=Cyc_PP_text(((_tmp708=",",_tag_dyneither(_tmp708,sizeof(char),2)))),((
_tmp706[1]=Cyc_Absynpp_exp2doc(_tmp219),((_tmp706[0]=Cyc_PP_text(((_tmp707="calloc(",
_tag_dyneither(_tmp707,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp706,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{const char*_tmp712;const char*
_tmp711;const char*_tmp710;const char*_tmp70F;struct Cyc_PP_Doc*_tmp70E[7];s=((
_tmp70E[6]=Cyc_PP_text(((_tmp712=")",_tag_dyneither(_tmp712,sizeof(char),2)))),((
_tmp70E[5]=Cyc_Absynpp_exp2doc(st),((_tmp70E[4]=Cyc_PP_text(((_tmp711=",",
_tag_dyneither(_tmp711,sizeof(char),2)))),((_tmp70E[3]=Cyc_Absynpp_exp2doc(
_tmp219),((_tmp70E[2]=Cyc_PP_text(((_tmp710=",",_tag_dyneither(_tmp710,sizeof(
char),2)))),((_tmp70E[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp217),((
_tmp70E[0]=Cyc_PP_text(((_tmp70F="rcalloc(",_tag_dyneither(_tmp70F,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp70E,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmp218 == 0)new_e=_tmp219;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp218,0),_tmp219,0);}if(_tmp217 == 0){const char*
_tmp717;const char*_tmp716;struct Cyc_PP_Doc*_tmp715[3];s=((_tmp715[2]=Cyc_PP_text(((
_tmp717=")",_tag_dyneither(_tmp717,sizeof(char),2)))),((_tmp715[1]=Cyc_Absynpp_exp2doc(
new_e),((_tmp715[0]=Cyc_PP_text(((_tmp716="malloc(",_tag_dyneither(_tmp716,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp715,sizeof(struct Cyc_PP_Doc*),3))))))));}
else{const char*_tmp71E;const char*_tmp71D;const char*_tmp71C;struct Cyc_PP_Doc*
_tmp71B[5];s=((_tmp71B[4]=Cyc_PP_text(((_tmp71E=")",_tag_dyneither(_tmp71E,
sizeof(char),2)))),((_tmp71B[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp71B[2]=Cyc_PP_text(((
_tmp71D=",",_tag_dyneither(_tmp71D,sizeof(char),2)))),((_tmp71B[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp217),((_tmp71B[0]=Cyc_PP_text(((_tmp71C="rmalloc(",
_tag_dyneither(_tmp71C,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp71B,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}}goto _LL1B9;_LL206: if(*((int*)_tmp1D4)
!= 36)goto _LL208;_tmp21A=((struct Cyc_Absyn_Swap_e_struct*)_tmp1D4)->f1;_tmp21B=((
struct Cyc_Absyn_Swap_e_struct*)_tmp1D4)->f2;_LL207:{const char*_tmp725;const char*
_tmp724;const char*_tmp723;struct Cyc_PP_Doc*_tmp722[5];s=((_tmp722[4]=Cyc_PP_text(((
_tmp725=")",_tag_dyneither(_tmp725,sizeof(char),2)))),((_tmp722[3]=Cyc_Absynpp_exp2doc(
_tmp21B),((_tmp722[2]=Cyc_PP_text(((_tmp724=",",_tag_dyneither(_tmp724,sizeof(
char),2)))),((_tmp722[1]=Cyc_Absynpp_exp2doc(_tmp21A),((_tmp722[0]=Cyc_PP_text(((
_tmp723="cycswap(",_tag_dyneither(_tmp723,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp722,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1B9;
_LL208: if(*((int*)_tmp1D4)!= 37)goto _LL20A;_tmp21C=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp1D4)->f1;_tmp21D=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1D4)->f2;
_LL209:{const char*_tmp726;s=Cyc_Absynpp_group_braces(((_tmp726=",",
_tag_dyneither(_tmp726,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp21D));}goto _LL1B9;_LL20A: if(*((int*)_tmp1D4)!= 38)goto _LL1B9;_tmp21E=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp1D4)->f1;_LL20B:{const char*_tmp72B;const
char*_tmp72A;struct Cyc_PP_Doc*_tmp729[7];s=((_tmp729[6]=Cyc_PP_text(((_tmp72B=")",
_tag_dyneither(_tmp72B,sizeof(char),2)))),((_tmp729[5]=Cyc_Absynpp_rb(),((
_tmp729[4]=Cyc_PP_blank_doc(),((_tmp729[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp21E)),((_tmp729[2]=Cyc_PP_blank_doc(),((_tmp729[1]=Cyc_Absynpp_lb(),((
_tmp729[0]=Cyc_PP_text(((_tmp72A="(",_tag_dyneither(_tmp72A,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp729,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
goto _LL1B9;_LL1B9:;}if(inprec >= myprec){const char*_tmp730;const char*_tmp72F;
struct Cyc_PP_Doc*_tmp72E[3];s=((_tmp72E[2]=Cyc_PP_text(((_tmp730=")",
_tag_dyneither(_tmp730,sizeof(char),2)))),((_tmp72E[1]=s,((_tmp72E[0]=Cyc_PP_text(((
_tmp72F="(",_tag_dyneither(_tmp72F,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp72E,sizeof(struct Cyc_PP_Doc*),3))))))));}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(
void*d);struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){void*_tmp299=d;
struct Cyc_Absyn_Exp*_tmp29A;struct _dyneither_ptr*_tmp29B;_LL216: if(*((int*)
_tmp299)!= 0)goto _LL218;_tmp29A=((struct Cyc_Absyn_ArrayElement_struct*)_tmp299)->f1;
_LL217: {const char*_tmp735;const char*_tmp734;struct Cyc_PP_Doc*_tmp733[3];return(
_tmp733[2]=Cyc_PP_text(((_tmp735="]",_tag_dyneither(_tmp735,sizeof(char),2)))),((
_tmp733[1]=Cyc_Absynpp_exp2doc(_tmp29A),((_tmp733[0]=Cyc_PP_text(((_tmp734=".[",
_tag_dyneither(_tmp734,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp733,
sizeof(struct Cyc_PP_Doc*),3)))))));}_LL218: if(*((int*)_tmp299)!= 1)goto _LL215;
_tmp29B=((struct Cyc_Absyn_FieldName_struct*)_tmp299)->f1;_LL219: {const char*
_tmp738;struct Cyc_PP_Doc*_tmp737[2];return(_tmp737[1]=Cyc_PP_textptr(_tmp29B),((
_tmp737[0]=Cyc_PP_text(((_tmp738=".",_tag_dyneither(_tmp738,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp737,sizeof(struct Cyc_PP_Doc*),2)))));}_LL215:;}
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple9*de);struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(
struct _tuple9*de){if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{const
char*_tmp73F;const char*_tmp73E;const char*_tmp73D;struct Cyc_PP_Doc*_tmp73C[2];
return(_tmp73C[1]=Cyc_Absynpp_exp2doc((*de).f2),((_tmp73C[0]=Cyc_PP_egroup(((
_tmp73F="",_tag_dyneither(_tmp73F,sizeof(char),1))),((_tmp73E="=",_tag_dyneither(
_tmp73E,sizeof(char),2))),((_tmp73D="=",_tag_dyneither(_tmp73D,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp73C,sizeof(
struct Cyc_PP_Doc*),2)))));}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es){const char*_tmp742;const char*_tmp741;const char*_tmp740;
return Cyc_PP_group(((_tmp740="",_tag_dyneither(_tmp740,sizeof(char),1))),((
_tmp741="",_tag_dyneither(_tmp741,sizeof(char),1))),((_tmp742=",",_tag_dyneither(
_tmp742,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst_union c);
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst_union c){union Cyc_Absyn_Cnst_union
_tmp2A8=c;void*_tmp2A9;char _tmp2AA;void*_tmp2AB;short _tmp2AC;void*_tmp2AD;int
_tmp2AE;void*_tmp2AF;int _tmp2B0;void*_tmp2B1;int _tmp2B2;void*_tmp2B3;long long
_tmp2B4;struct _dyneither_ptr _tmp2B5;struct _dyneither_ptr _tmp2B6;_LL21B: if((
_tmp2A8.Char_c).tag != 0)goto _LL21D;_tmp2A9=(_tmp2A8.Char_c).f1;_tmp2AA=(_tmp2A8.Char_c).f2;
_LL21C: {const char*_tmp746;void*_tmp745[1];struct Cyc_String_pa_struct _tmp744;
return Cyc_PP_text((struct _dyneither_ptr)((_tmp744.tag=0,((_tmp744.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp2AA)),((
_tmp745[0]=& _tmp744,Cyc_aprintf(((_tmp746="'%s'",_tag_dyneither(_tmp746,sizeof(
char),5))),_tag_dyneither(_tmp745,sizeof(void*),1)))))))));}_LL21D: if((_tmp2A8.Short_c).tag
!= 1)goto _LL21F;_tmp2AB=(_tmp2A8.Short_c).f1;_tmp2AC=(_tmp2A8.Short_c).f2;_LL21E: {
const char*_tmp74A;void*_tmp749[1];struct Cyc_Int_pa_struct _tmp748;return Cyc_PP_text((
struct _dyneither_ptr)((_tmp748.tag=1,((_tmp748.f1=(unsigned long)((int)_tmp2AC),((
_tmp749[0]=& _tmp748,Cyc_aprintf(((_tmp74A="%d",_tag_dyneither(_tmp74A,sizeof(
char),3))),_tag_dyneither(_tmp749,sizeof(void*),1)))))))));}_LL21F: if((_tmp2A8.Int_c).tag
!= 2)goto _LL221;_tmp2AD=(_tmp2A8.Int_c).f1;if((int)_tmp2AD != 2)goto _LL221;
_tmp2AE=(_tmp2A8.Int_c).f2;_LL220: _tmp2B0=_tmp2AE;goto _LL222;_LL221: if((_tmp2A8.Int_c).tag
!= 2)goto _LL223;_tmp2AF=(_tmp2A8.Int_c).f1;if((int)_tmp2AF != 0)goto _LL223;
_tmp2B0=(_tmp2A8.Int_c).f2;_LL222: {const char*_tmp74E;void*_tmp74D[1];struct Cyc_Int_pa_struct
_tmp74C;return Cyc_PP_text((struct _dyneither_ptr)((_tmp74C.tag=1,((_tmp74C.f1=(
unsigned long)_tmp2B0,((_tmp74D[0]=& _tmp74C,Cyc_aprintf(((_tmp74E="%d",
_tag_dyneither(_tmp74E,sizeof(char),3))),_tag_dyneither(_tmp74D,sizeof(void*),1)))))))));}
_LL223: if((_tmp2A8.Int_c).tag != 2)goto _LL225;_tmp2B1=(_tmp2A8.Int_c).f1;if((int)
_tmp2B1 != 1)goto _LL225;_tmp2B2=(_tmp2A8.Int_c).f2;_LL224: {const char*_tmp752;
void*_tmp751[1];struct Cyc_Int_pa_struct _tmp750;return Cyc_PP_text((struct
_dyneither_ptr)((_tmp750.tag=1,((_tmp750.f1=(unsigned int)_tmp2B2,((_tmp751[0]=&
_tmp750,Cyc_aprintf(((_tmp752="%u",_tag_dyneither(_tmp752,sizeof(char),3))),
_tag_dyneither(_tmp751,sizeof(void*),1)))))))));}_LL225: if((_tmp2A8.LongLong_c).tag
!= 3)goto _LL227;_tmp2B3=(_tmp2A8.LongLong_c).f1;_tmp2B4=(_tmp2A8.LongLong_c).f2;
_LL226: {const char*_tmp753;return Cyc_PP_text(((_tmp753="<<FIX LONG LONG CONSTANT>>",
_tag_dyneither(_tmp753,sizeof(char),27))));}_LL227: if((_tmp2A8.Float_c).tag != 4)
goto _LL229;_tmp2B5=(_tmp2A8.Float_c).f1;_LL228: return Cyc_PP_text(_tmp2B5);_LL229:
if((_tmp2A8.Null_c).tag != 6)goto _LL22B;_LL22A: {const char*_tmp754;return Cyc_PP_text(((
_tmp754="NULL",_tag_dyneither(_tmp754,sizeof(char),5))));}_LL22B: if((_tmp2A8.String_c).tag
!= 5)goto _LL21A;_tmp2B6=(_tmp2A8.String_c).f1;_LL22C: {const char*_tmp759;const
char*_tmp758;struct Cyc_PP_Doc*_tmp757[3];return(_tmp757[2]=Cyc_PP_text(((_tmp759="\"",
_tag_dyneither(_tmp759,sizeof(char),2)))),((_tmp757[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(
_tmp2B6)),((_tmp757[0]=Cyc_PP_text(((_tmp758="\"",_tag_dyneither(_tmp758,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp757,sizeof(struct Cyc_PP_Doc*),3)))))));}
_LL21A:;}struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*
es);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*
es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (void*)((void*)19)){if(es
== 0  || es->tl != 0){struct Cyc_Core_Failure_struct _tmp766;const char*_tmp765;void*
_tmp764[1];struct Cyc_String_pa_struct _tmp763;struct Cyc_Core_Failure_struct*
_tmp762;(int)_throw((void*)((_tmp762=_cycalloc(sizeof(*_tmp762)),((_tmp762[0]=((
_tmp766.tag=Cyc_Core_Failure,((_tmp766.f1=(struct _dyneither_ptr)((_tmp763.tag=0,((
_tmp763.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp764[0]=& _tmp763,Cyc_aprintf(((_tmp765="Absynpp::primapp2doc Numelts: %s with bad args",
_tag_dyneither(_tmp765,sizeof(char),47))),_tag_dyneither(_tmp764,sizeof(void*),1)))))))),
_tmp766)))),_tmp762)))));}{const char*_tmp76B;const char*_tmp76A;struct Cyc_PP_Doc*
_tmp769[3];return(_tmp769[2]=Cyc_PP_text(((_tmp76B=")",_tag_dyneither(_tmp76B,
sizeof(char),2)))),((_tmp769[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((
_tmp769[0]=Cyc_PP_text(((_tmp76A="numelts(",_tag_dyneither(_tmp76A,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp769,sizeof(struct Cyc_PP_Doc*),3)))))));}}
else{struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es);if(ds == 0){struct Cyc_Core_Failure_struct _tmp778;const char*_tmp777;
void*_tmp776[1];struct Cyc_String_pa_struct _tmp775;struct Cyc_Core_Failure_struct*
_tmp774;(int)_throw((void*)((_tmp774=_cycalloc(sizeof(*_tmp774)),((_tmp774[0]=((
_tmp778.tag=Cyc_Core_Failure,((_tmp778.f1=(struct _dyneither_ptr)((_tmp775.tag=0,((
_tmp775.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp776[0]=& _tmp775,Cyc_aprintf(((_tmp777="Absynpp::primapp2doc: %s with no args",
_tag_dyneither(_tmp777,sizeof(char),38))),_tag_dyneither(_tmp776,sizeof(void*),1)))))))),
_tmp778)))),_tmp774)))));}else{if(ds->tl == 0){const char*_tmp77B;struct Cyc_PP_Doc*
_tmp77A[3];return(_tmp77A[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp77A[1]=Cyc_PP_text(((
_tmp77B=" ",_tag_dyneither(_tmp77B,sizeof(char),2)))),((_tmp77A[0]=ps,Cyc_PP_cat(
_tag_dyneither(_tmp77A,sizeof(struct Cyc_PP_Doc*),3)))))));}else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0){struct Cyc_Core_Failure_struct _tmp788;const char*
_tmp787;void*_tmp786[1];struct Cyc_String_pa_struct _tmp785;struct Cyc_Core_Failure_struct*
_tmp784;(int)_throw((void*)((_tmp784=_cycalloc(sizeof(*_tmp784)),((_tmp784[0]=((
_tmp788.tag=Cyc_Core_Failure,((_tmp788.f1=(struct _dyneither_ptr)((_tmp785.tag=0,((
_tmp785.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp786[0]=& _tmp785,Cyc_aprintf(((_tmp787="Absynpp::primapp2doc: %s with more than 2 args",
_tag_dyneither(_tmp787,sizeof(char),47))),_tag_dyneither(_tmp786,sizeof(void*),1)))))))),
_tmp788)))),_tmp784)))));}else{const char*_tmp78D;const char*_tmp78C;struct Cyc_PP_Doc*
_tmp78B[5];return(_tmp78B[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd,((_tmp78B[3]=Cyc_PP_text(((_tmp78D=" ",_tag_dyneither(
_tmp78D,sizeof(char),2)))),((_tmp78B[2]=ps,((_tmp78B[1]=Cyc_PP_text(((_tmp78C=" ",
_tag_dyneither(_tmp78C,sizeof(char),2)))),((_tmp78B[0]=(struct Cyc_PP_Doc*)ds->hd,
Cyc_PP_cat(_tag_dyneither(_tmp78B,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
struct _dyneither_ptr Cyc_Absynpp_prim2str(void*p);struct _dyneither_ptr Cyc_Absynpp_prim2str(
void*p){void*_tmp2DF=p;_LL22E: if((int)_tmp2DF != 0)goto _LL230;_LL22F: {const char*
_tmp78E;return(_tmp78E="+",_tag_dyneither(_tmp78E,sizeof(char),2));}_LL230: if((
int)_tmp2DF != 1)goto _LL232;_LL231: {const char*_tmp78F;return(_tmp78F="*",
_tag_dyneither(_tmp78F,sizeof(char),2));}_LL232: if((int)_tmp2DF != 2)goto _LL234;
_LL233: {const char*_tmp790;return(_tmp790="-",_tag_dyneither(_tmp790,sizeof(char),
2));}_LL234: if((int)_tmp2DF != 3)goto _LL236;_LL235: {const char*_tmp791;return(
_tmp791="/",_tag_dyneither(_tmp791,sizeof(char),2));}_LL236: if((int)_tmp2DF != 4)
goto _LL238;_LL237: {const char*_tmp792;return(_tmp792="%",_tag_dyneither(_tmp792,
sizeof(char),2));}_LL238: if((int)_tmp2DF != 5)goto _LL23A;_LL239: {const char*
_tmp793;return(_tmp793="==",_tag_dyneither(_tmp793,sizeof(char),3));}_LL23A: if((
int)_tmp2DF != 6)goto _LL23C;_LL23B: {const char*_tmp794;return(_tmp794="!=",
_tag_dyneither(_tmp794,sizeof(char),3));}_LL23C: if((int)_tmp2DF != 7)goto _LL23E;
_LL23D: {const char*_tmp795;return(_tmp795=">",_tag_dyneither(_tmp795,sizeof(char),
2));}_LL23E: if((int)_tmp2DF != 8)goto _LL240;_LL23F: {const char*_tmp796;return(
_tmp796="<",_tag_dyneither(_tmp796,sizeof(char),2));}_LL240: if((int)_tmp2DF != 9)
goto _LL242;_LL241: {const char*_tmp797;return(_tmp797=">=",_tag_dyneither(_tmp797,
sizeof(char),3));}_LL242: if((int)_tmp2DF != 10)goto _LL244;_LL243: {const char*
_tmp798;return(_tmp798="<=",_tag_dyneither(_tmp798,sizeof(char),3));}_LL244: if((
int)_tmp2DF != 11)goto _LL246;_LL245: {const char*_tmp799;return(_tmp799="!",
_tag_dyneither(_tmp799,sizeof(char),2));}_LL246: if((int)_tmp2DF != 12)goto _LL248;
_LL247: {const char*_tmp79A;return(_tmp79A="~",_tag_dyneither(_tmp79A,sizeof(char),
2));}_LL248: if((int)_tmp2DF != 13)goto _LL24A;_LL249: {const char*_tmp79B;return(
_tmp79B="&",_tag_dyneither(_tmp79B,sizeof(char),2));}_LL24A: if((int)_tmp2DF != 14)
goto _LL24C;_LL24B: {const char*_tmp79C;return(_tmp79C="|",_tag_dyneither(_tmp79C,
sizeof(char),2));}_LL24C: if((int)_tmp2DF != 15)goto _LL24E;_LL24D: {const char*
_tmp79D;return(_tmp79D="^",_tag_dyneither(_tmp79D,sizeof(char),2));}_LL24E: if((
int)_tmp2DF != 16)goto _LL250;_LL24F: {const char*_tmp79E;return(_tmp79E="<<",
_tag_dyneither(_tmp79E,sizeof(char),3));}_LL250: if((int)_tmp2DF != 17)goto _LL252;
_LL251: {const char*_tmp79F;return(_tmp79F=">>",_tag_dyneither(_tmp79F,sizeof(
char),3));}_LL252: if((int)_tmp2DF != 18)goto _LL254;_LL253: {const char*_tmp7A0;
return(_tmp7A0=">>>",_tag_dyneither(_tmp7A0,sizeof(char),4));}_LL254: if((int)
_tmp2DF != 19)goto _LL22D;_LL255: {const char*_tmp7A1;return(_tmp7A1="numelts",
_tag_dyneither(_tmp7A1,sizeof(char),8));}_LL22D:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
void*p);struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*p){return Cyc_PP_text(Cyc_Absynpp_prim2str(
p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp2F4=(void*)s->r;_LL257: if(_tmp2F4 <= (void*)1)
goto _LL259;if(*((int*)_tmp2F4)!= 11)goto _LL259;_LL258: return 1;_LL259:;_LL25A:
return 0;_LL256:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st);
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st){struct Cyc_PP_Doc*
s;{void*_tmp2F5=(void*)st->r;struct Cyc_Absyn_Exp*_tmp2F6;struct Cyc_Absyn_Stmt*
_tmp2F7;struct Cyc_Absyn_Stmt*_tmp2F8;struct Cyc_Absyn_Exp*_tmp2F9;struct Cyc_Absyn_Exp*
_tmp2FA;struct Cyc_Absyn_Stmt*_tmp2FB;struct Cyc_Absyn_Stmt*_tmp2FC;struct _tuple2
_tmp2FD;struct Cyc_Absyn_Exp*_tmp2FE;struct Cyc_Absyn_Stmt*_tmp2FF;struct
_dyneither_ptr*_tmp300;struct Cyc_Absyn_Exp*_tmp301;struct _tuple2 _tmp302;struct
Cyc_Absyn_Exp*_tmp303;struct _tuple2 _tmp304;struct Cyc_Absyn_Exp*_tmp305;struct Cyc_Absyn_Stmt*
_tmp306;struct Cyc_Absyn_Exp*_tmp307;struct Cyc_List_List*_tmp308;struct Cyc_List_List*
_tmp309;struct Cyc_List_List*_tmp30A;struct Cyc_Absyn_Decl*_tmp30B;struct Cyc_Absyn_Stmt*
_tmp30C;struct _dyneither_ptr*_tmp30D;struct Cyc_Absyn_Stmt*_tmp30E;struct Cyc_Absyn_Stmt*
_tmp30F;struct _tuple2 _tmp310;struct Cyc_Absyn_Exp*_tmp311;struct Cyc_Absyn_Stmt*
_tmp312;struct Cyc_List_List*_tmp313;struct Cyc_Absyn_Exp*_tmp314;_LL25C: if((int)
_tmp2F5 != 0)goto _LL25E;_LL25D:{const char*_tmp7A2;s=Cyc_PP_text(((_tmp7A2=";",
_tag_dyneither(_tmp7A2,sizeof(char),2))));}goto _LL25B;_LL25E: if(_tmp2F5 <= (void*)
1)goto _LL260;if(*((int*)_tmp2F5)!= 0)goto _LL260;_tmp2F6=((struct Cyc_Absyn_Exp_s_struct*)
_tmp2F5)->f1;_LL25F:{const char*_tmp7A5;struct Cyc_PP_Doc*_tmp7A4[2];s=((_tmp7A4[1]=
Cyc_PP_text(((_tmp7A5=";",_tag_dyneither(_tmp7A5,sizeof(char),2)))),((_tmp7A4[0]=
Cyc_Absynpp_exp2doc(_tmp2F6),Cyc_PP_cat(_tag_dyneither(_tmp7A4,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL25B;_LL260: if(_tmp2F5 <= (void*)1)goto _LL262;if(*((int*)_tmp2F5)
!= 1)goto _LL262;_tmp2F7=((struct Cyc_Absyn_Seq_s_struct*)_tmp2F5)->f1;_tmp2F8=((
struct Cyc_Absyn_Seq_s_struct*)_tmp2F5)->f2;_LL261: if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp2F7)){struct Cyc_PP_Doc*_tmp7A8[5];struct Cyc_PP_Doc*
_tmp7A7[7];s=((_tmp7A7[6]=Cyc_Absynpp_is_declaration(_tmp2F8)?(_tmp7A8[4]=Cyc_PP_line_doc(),((
_tmp7A8[3]=Cyc_Absynpp_rb(),((_tmp7A8[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp2F8)),((_tmp7A8[1]=Cyc_PP_blank_doc(),((_tmp7A8[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp7A8,sizeof(struct Cyc_PP_Doc*),5))))))))))): Cyc_Absynpp_stmt2doc(
_tmp2F8),((_tmp7A7[5]=Cyc_PP_line_doc(),((_tmp7A7[4]=Cyc_Absynpp_rb(),((_tmp7A7[
3]=Cyc_PP_line_doc(),((_tmp7A7[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2F7)),((
_tmp7A7[1]=Cyc_PP_blank_doc(),((_tmp7A7[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp7A7,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{if(Cyc_Absynpp_is_declaration(
_tmp2F8)){struct Cyc_PP_Doc*_tmp7A9[7];s=((_tmp7A9[6]=Cyc_PP_line_doc(),((_tmp7A9[
5]=Cyc_Absynpp_rb(),((_tmp7A9[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2F8)),((
_tmp7A9[3]=Cyc_PP_blank_doc(),((_tmp7A9[2]=Cyc_Absynpp_lb(),((_tmp7A9[1]=Cyc_PP_line_doc(),((
_tmp7A9[0]=Cyc_Absynpp_stmt2doc(_tmp2F7),Cyc_PP_cat(_tag_dyneither(_tmp7A9,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{struct Cyc_List_List*_tmp7AD;
struct Cyc_List_List*_tmp7AC;const char*_tmp7AA;s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Stmt*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,((_tmp7AA="",_tag_dyneither(_tmp7AA,sizeof(char),1))),((
_tmp7AC=_cycalloc(sizeof(*_tmp7AC)),((_tmp7AC->hd=_tmp2F7,((_tmp7AC->tl=((
_tmp7AD=_cycalloc(sizeof(*_tmp7AD)),((_tmp7AD->hd=_tmp2F8,((_tmp7AD->tl=0,
_tmp7AD)))))),_tmp7AC)))))));}}}else{struct Cyc_List_List*_tmp7B1;struct Cyc_List_List*
_tmp7B0;const char*_tmp7AE;s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct
Cyc_Absyn_Stmt*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,((_tmp7AE="",_tag_dyneither(_tmp7AE,sizeof(char),1))),((
_tmp7B0=_cycalloc(sizeof(*_tmp7B0)),((_tmp7B0->hd=_tmp2F7,((_tmp7B0->tl=((
_tmp7B1=_cycalloc(sizeof(*_tmp7B1)),((_tmp7B1->hd=_tmp2F8,((_tmp7B1->tl=0,
_tmp7B1)))))),_tmp7B0)))))));}goto _LL25B;_LL262: if(_tmp2F5 <= (void*)1)goto _LL264;
if(*((int*)_tmp2F5)!= 2)goto _LL264;_tmp2F9=((struct Cyc_Absyn_Return_s_struct*)
_tmp2F5)->f1;_LL263: if(_tmp2F9 == 0){const char*_tmp7B2;s=Cyc_PP_text(((_tmp7B2="return;",
_tag_dyneither(_tmp7B2,sizeof(char),8))));}else{const char*_tmp7B7;const char*
_tmp7B6;struct Cyc_PP_Doc*_tmp7B5[3];s=((_tmp7B5[2]=Cyc_PP_text(((_tmp7B7=";",
_tag_dyneither(_tmp7B7,sizeof(char),2)))),((_tmp7B5[1]=_tmp2F9 == 0?Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2F9),((_tmp7B5[0]=Cyc_PP_text(((
_tmp7B6="return ",_tag_dyneither(_tmp7B6,sizeof(char),8)))),Cyc_PP_cat(
_tag_dyneither(_tmp7B5,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL25B;_LL264:
if(_tmp2F5 <= (void*)1)goto _LL266;if(*((int*)_tmp2F5)!= 3)goto _LL266;_tmp2FA=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2F5)->f1;_tmp2FB=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp2F5)->f2;_tmp2FC=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2F5)->f3;_LL265: {
int print_else;{void*_tmp325=(void*)_tmp2FC->r;_LL281: if((int)_tmp325 != 0)goto
_LL283;_LL282: print_else=0;goto _LL280;_LL283:;_LL284: print_else=1;goto _LL280;
_LL280:;}{struct Cyc_PP_Doc*_tmp7C6[6];const char*_tmp7C5;struct Cyc_PP_Doc*_tmp7C4[
2];struct Cyc_PP_Doc*_tmp7C3[2];const char*_tmp7C2;const char*_tmp7C1;struct Cyc_PP_Doc*
_tmp7C0[8];s=((_tmp7C0[7]=print_else?(_tmp7C6[5]=Cyc_Absynpp_rb(),((_tmp7C6[4]=
Cyc_PP_line_doc(),((_tmp7C6[3]=Cyc_PP_nest(2,((_tmp7C4[1]=Cyc_Absynpp_stmt2doc(
_tmp2FC),((_tmp7C4[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp7C4,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp7C6[2]=Cyc_Absynpp_lb(),((_tmp7C6[1]=
Cyc_PP_text(((_tmp7C5="else ",_tag_dyneither(_tmp7C5,sizeof(char),6)))),((
_tmp7C6[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp7C6,sizeof(struct Cyc_PP_Doc*),
6))))))))))))): Cyc_PP_nil_doc(),((_tmp7C0[6]=Cyc_Absynpp_rb(),((_tmp7C0[5]=Cyc_PP_line_doc(),((
_tmp7C0[4]=Cyc_PP_nest(2,((_tmp7C3[1]=Cyc_Absynpp_stmt2doc(_tmp2FB),((_tmp7C3[0]=
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp7C3,sizeof(struct Cyc_PP_Doc*),2))))))),((
_tmp7C0[3]=Cyc_Absynpp_lb(),((_tmp7C0[2]=Cyc_PP_text(((_tmp7C2=") ",
_tag_dyneither(_tmp7C2,sizeof(char),3)))),((_tmp7C0[1]=Cyc_Absynpp_exp2doc(
_tmp2FA),((_tmp7C0[0]=Cyc_PP_text(((_tmp7C1="if (",_tag_dyneither(_tmp7C1,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp7C0,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL25B;}_LL266: if(_tmp2F5 <= (void*)1)goto _LL268;if(*((int*)_tmp2F5)!= 4)goto
_LL268;_tmp2FD=((struct Cyc_Absyn_While_s_struct*)_tmp2F5)->f1;_tmp2FE=_tmp2FD.f1;
_tmp2FF=((struct Cyc_Absyn_While_s_struct*)_tmp2F5)->f2;_LL267:{struct Cyc_PP_Doc*
_tmp7CD[2];const char*_tmp7CC;const char*_tmp7CB;struct Cyc_PP_Doc*_tmp7CA[7];s=((
_tmp7CA[6]=Cyc_Absynpp_rb(),((_tmp7CA[5]=Cyc_PP_line_doc(),((_tmp7CA[4]=Cyc_PP_nest(
2,((_tmp7CD[1]=Cyc_Absynpp_stmt2doc(_tmp2FF),((_tmp7CD[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp7CD,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp7CA[3]=Cyc_Absynpp_lb(),((
_tmp7CA[2]=Cyc_PP_text(((_tmp7CC=") ",_tag_dyneither(_tmp7CC,sizeof(char),3)))),((
_tmp7CA[1]=Cyc_Absynpp_exp2doc(_tmp2FE),((_tmp7CA[0]=Cyc_PP_text(((_tmp7CB="while (",
_tag_dyneither(_tmp7CB,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7CA,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL25B;_LL268: if(_tmp2F5 <= (
void*)1)goto _LL26A;if(*((int*)_tmp2F5)!= 5)goto _LL26A;_LL269:{const char*_tmp7CE;
s=Cyc_PP_text(((_tmp7CE="break;",_tag_dyneither(_tmp7CE,sizeof(char),7))));}goto
_LL25B;_LL26A: if(_tmp2F5 <= (void*)1)goto _LL26C;if(*((int*)_tmp2F5)!= 6)goto
_LL26C;_LL26B:{const char*_tmp7CF;s=Cyc_PP_text(((_tmp7CF="continue;",
_tag_dyneither(_tmp7CF,sizeof(char),10))));}goto _LL25B;_LL26C: if(_tmp2F5 <= (void*)
1)goto _LL26E;if(*((int*)_tmp2F5)!= 7)goto _LL26E;_tmp300=((struct Cyc_Absyn_Goto_s_struct*)
_tmp2F5)->f1;_LL26D:{const char*_tmp7D3;void*_tmp7D2[1];struct Cyc_String_pa_struct
_tmp7D1;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp7D1.tag=0,((_tmp7D1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp300),((_tmp7D2[0]=& _tmp7D1,Cyc_aprintf(((
_tmp7D3="goto %s;",_tag_dyneither(_tmp7D3,sizeof(char),9))),_tag_dyneither(
_tmp7D2,sizeof(void*),1)))))))));}goto _LL25B;_LL26E: if(_tmp2F5 <= (void*)1)goto
_LL270;if(*((int*)_tmp2F5)!= 8)goto _LL270;_tmp301=((struct Cyc_Absyn_For_s_struct*)
_tmp2F5)->f1;_tmp302=((struct Cyc_Absyn_For_s_struct*)_tmp2F5)->f2;_tmp303=
_tmp302.f1;_tmp304=((struct Cyc_Absyn_For_s_struct*)_tmp2F5)->f3;_tmp305=_tmp304.f1;
_tmp306=((struct Cyc_Absyn_For_s_struct*)_tmp2F5)->f4;_LL26F:{struct Cyc_PP_Doc*
_tmp7DE[2];const char*_tmp7DD;const char*_tmp7DC;const char*_tmp7DB;const char*
_tmp7DA;struct Cyc_PP_Doc*_tmp7D9[11];s=((_tmp7D9[10]=Cyc_Absynpp_rb(),((_tmp7D9[
9]=Cyc_PP_line_doc(),((_tmp7D9[8]=Cyc_PP_nest(2,((_tmp7DE[1]=Cyc_Absynpp_stmt2doc(
_tmp306),((_tmp7DE[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp7DE,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp7D9[7]=Cyc_Absynpp_lb(),((_tmp7D9[6]=
Cyc_PP_text(((_tmp7DD=") ",_tag_dyneither(_tmp7DD,sizeof(char),3)))),((_tmp7D9[5]=
Cyc_Absynpp_exp2doc(_tmp305),((_tmp7D9[4]=Cyc_PP_text(((_tmp7DC="; ",
_tag_dyneither(_tmp7DC,sizeof(char),3)))),((_tmp7D9[3]=Cyc_Absynpp_exp2doc(
_tmp303),((_tmp7D9[2]=Cyc_PP_text(((_tmp7DB="; ",_tag_dyneither(_tmp7DB,sizeof(
char),3)))),((_tmp7D9[1]=Cyc_Absynpp_exp2doc(_tmp301),((_tmp7D9[0]=Cyc_PP_text(((
_tmp7DA="for(",_tag_dyneither(_tmp7DA,sizeof(char),5)))),Cyc_PP_cat(
_tag_dyneither(_tmp7D9,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL25B;_LL270: if(_tmp2F5 <= (void*)1)goto _LL272;if(*((int*)_tmp2F5)!= 9)goto
_LL272;_tmp307=((struct Cyc_Absyn_Switch_s_struct*)_tmp2F5)->f1;_tmp308=((struct
Cyc_Absyn_Switch_s_struct*)_tmp2F5)->f2;_LL271:{const char*_tmp7E3;const char*
_tmp7E2;struct Cyc_PP_Doc*_tmp7E1[8];s=((_tmp7E1[7]=Cyc_Absynpp_rb(),((_tmp7E1[6]=
Cyc_PP_line_doc(),((_tmp7E1[5]=Cyc_Absynpp_switchclauses2doc(_tmp308),((_tmp7E1[
4]=Cyc_PP_line_doc(),((_tmp7E1[3]=Cyc_Absynpp_lb(),((_tmp7E1[2]=Cyc_PP_text(((
_tmp7E3=") ",_tag_dyneither(_tmp7E3,sizeof(char),3)))),((_tmp7E1[1]=Cyc_Absynpp_exp2doc(
_tmp307),((_tmp7E1[0]=Cyc_PP_text(((_tmp7E2="switch (",_tag_dyneither(_tmp7E2,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7E1,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL25B;_LL272: if(_tmp2F5 <= (void*)1)goto _LL274;if(*((int*)_tmp2F5)!= 10)goto
_LL274;_tmp309=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp2F5)->f1;if(_tmp309 != 0)
goto _LL274;_LL273:{const char*_tmp7E4;s=Cyc_PP_text(((_tmp7E4="fallthru;",
_tag_dyneither(_tmp7E4,sizeof(char),10))));}goto _LL25B;_LL274: if(_tmp2F5 <= (void*)
1)goto _LL276;if(*((int*)_tmp2F5)!= 10)goto _LL276;_tmp30A=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp2F5)->f1;_LL275:{const char*_tmp7E9;const char*_tmp7E8;struct Cyc_PP_Doc*
_tmp7E7[3];s=((_tmp7E7[2]=Cyc_PP_text(((_tmp7E9=");",_tag_dyneither(_tmp7E9,
sizeof(char),3)))),((_tmp7E7[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp30A),((_tmp7E7[
0]=Cyc_PP_text(((_tmp7E8="fallthru(",_tag_dyneither(_tmp7E8,sizeof(char),10)))),
Cyc_PP_cat(_tag_dyneither(_tmp7E7,sizeof(struct Cyc_PP_Doc*),3))))))));}goto
_LL25B;_LL276: if(_tmp2F5 <= (void*)1)goto _LL278;if(*((int*)_tmp2F5)!= 11)goto
_LL278;_tmp30B=((struct Cyc_Absyn_Decl_s_struct*)_tmp2F5)->f1;_tmp30C=((struct Cyc_Absyn_Decl_s_struct*)
_tmp2F5)->f2;_LL277:{struct Cyc_PP_Doc*_tmp7EA[3];s=((_tmp7EA[2]=Cyc_Absynpp_stmt2doc(
_tmp30C),((_tmp7EA[1]=Cyc_PP_line_doc(),((_tmp7EA[0]=Cyc_Absynpp_decl2doc(
_tmp30B),Cyc_PP_cat(_tag_dyneither(_tmp7EA,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL25B;_LL278: if(_tmp2F5 <= (void*)1)goto _LL27A;if(*((int*)_tmp2F5)!= 12)goto
_LL27A;_tmp30D=((struct Cyc_Absyn_Label_s_struct*)_tmp2F5)->f1;_tmp30E=((struct
Cyc_Absyn_Label_s_struct*)_tmp2F5)->f2;_LL279: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(
_tmp30E)){const char*_tmp7ED;struct Cyc_PP_Doc*_tmp7EC[7];s=((_tmp7EC[6]=Cyc_Absynpp_rb(),((
_tmp7EC[5]=Cyc_PP_line_doc(),((_tmp7EC[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp30E)),((_tmp7EC[3]=Cyc_PP_line_doc(),((_tmp7EC[2]=Cyc_Absynpp_lb(),((_tmp7EC[
1]=Cyc_PP_text(((_tmp7ED=": ",_tag_dyneither(_tmp7ED,sizeof(char),3)))),((
_tmp7EC[0]=Cyc_PP_textptr(_tmp30D),Cyc_PP_cat(_tag_dyneither(_tmp7EC,sizeof(
struct Cyc_PP_Doc*),7))))))))))))))));}else{const char*_tmp7F0;struct Cyc_PP_Doc*
_tmp7EF[3];s=((_tmp7EF[2]=Cyc_Absynpp_stmt2doc(_tmp30E),((_tmp7EF[1]=Cyc_PP_text(((
_tmp7F0=": ",_tag_dyneither(_tmp7F0,sizeof(char),3)))),((_tmp7EF[0]=Cyc_PP_textptr(
_tmp30D),Cyc_PP_cat(_tag_dyneither(_tmp7EF,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL25B;_LL27A: if(_tmp2F5 <= (void*)1)goto _LL27C;if(*((int*)_tmp2F5)!= 13)goto
_LL27C;_tmp30F=((struct Cyc_Absyn_Do_s_struct*)_tmp2F5)->f1;_tmp310=((struct Cyc_Absyn_Do_s_struct*)
_tmp2F5)->f2;_tmp311=_tmp310.f1;_LL27B:{const char*_tmp7F7;const char*_tmp7F6;
const char*_tmp7F5;struct Cyc_PP_Doc*_tmp7F4[9];s=((_tmp7F4[8]=Cyc_PP_text(((
_tmp7F7=");",_tag_dyneither(_tmp7F7,sizeof(char),3)))),((_tmp7F4[7]=Cyc_Absynpp_exp2doc(
_tmp311),((_tmp7F4[6]=Cyc_PP_text(((_tmp7F6=" while (",_tag_dyneither(_tmp7F6,
sizeof(char),9)))),((_tmp7F4[5]=Cyc_Absynpp_rb(),((_tmp7F4[4]=Cyc_PP_line_doc(),((
_tmp7F4[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp30F)),((_tmp7F4[2]=Cyc_PP_line_doc(),((
_tmp7F4[1]=Cyc_Absynpp_lb(),((_tmp7F4[0]=Cyc_PP_text(((_tmp7F5="do ",
_tag_dyneither(_tmp7F5,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp7F4,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL25B;_LL27C: if(_tmp2F5 <= (
void*)1)goto _LL27E;if(*((int*)_tmp2F5)!= 14)goto _LL27E;_tmp312=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp2F5)->f1;_tmp313=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp2F5)->f2;_LL27D:{
const char*_tmp7FC;const char*_tmp7FB;struct Cyc_PP_Doc*_tmp7FA[12];s=((_tmp7FA[11]=
Cyc_Absynpp_rb(),((_tmp7FA[10]=Cyc_PP_line_doc(),((_tmp7FA[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp313)),((_tmp7FA[8]=Cyc_PP_line_doc(),((_tmp7FA[7]=Cyc_Absynpp_lb(),((_tmp7FA[
6]=Cyc_PP_text(((_tmp7FC=" catch ",_tag_dyneither(_tmp7FC,sizeof(char),8)))),((
_tmp7FA[5]=Cyc_Absynpp_rb(),((_tmp7FA[4]=Cyc_PP_line_doc(),((_tmp7FA[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp312)),((_tmp7FA[2]=Cyc_PP_line_doc(),((_tmp7FA[1]=Cyc_Absynpp_lb(),((
_tmp7FA[0]=Cyc_PP_text(((_tmp7FB="try ",_tag_dyneither(_tmp7FB,sizeof(char),5)))),
Cyc_PP_cat(_tag_dyneither(_tmp7FA,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
goto _LL25B;_LL27E: if(_tmp2F5 <= (void*)1)goto _LL25B;if(*((int*)_tmp2F5)!= 15)goto
_LL25B;_tmp314=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp2F5)->f1;_LL27F:{
const char*_tmp801;const char*_tmp800;struct Cyc_PP_Doc*_tmp7FF[3];s=((_tmp7FF[2]=
Cyc_PP_text(((_tmp801=");",_tag_dyneither(_tmp801,sizeof(char),3)))),((_tmp7FF[1]=
Cyc_Absynpp_exp2doc(_tmp314),((_tmp7FF[0]=Cyc_PP_text(((_tmp800="reset_region(",
_tag_dyneither(_tmp800,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp7FF,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL25B;_LL25B:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p);struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{void*_tmp352=(void*)p->r;void*_tmp353;
int _tmp354;char _tmp355;struct _dyneither_ptr _tmp356;struct Cyc_Absyn_Vardecl*
_tmp357;struct Cyc_Absyn_Pat*_tmp358;struct Cyc_Absyn_Pat _tmp359;void*_tmp35A;
struct Cyc_Absyn_Vardecl*_tmp35B;struct Cyc_Absyn_Pat*_tmp35C;struct Cyc_Absyn_Tvar*
_tmp35D;struct Cyc_Absyn_Vardecl*_tmp35E;struct Cyc_List_List*_tmp35F;int _tmp360;
struct Cyc_Absyn_Pat*_tmp361;struct Cyc_Absyn_Vardecl*_tmp362;struct Cyc_Absyn_Pat*
_tmp363;struct Cyc_Absyn_Pat _tmp364;void*_tmp365;struct Cyc_Absyn_Vardecl*_tmp366;
struct Cyc_Absyn_Pat*_tmp367;struct _tuple0*_tmp368;struct _tuple0*_tmp369;struct
Cyc_List_List*_tmp36A;int _tmp36B;struct Cyc_Absyn_AggrInfo _tmp36C;union Cyc_Absyn_AggrInfoU_union
_tmp36D;struct Cyc_List_List*_tmp36E;struct Cyc_List_List*_tmp36F;int _tmp370;
struct Cyc_Absyn_Enumfield*_tmp371;struct Cyc_Absyn_Enumfield*_tmp372;struct Cyc_Absyn_Tunionfield*
_tmp373;struct Cyc_List_List*_tmp374;struct Cyc_Absyn_Tunionfield*_tmp375;struct
Cyc_List_List*_tmp376;int _tmp377;struct Cyc_Absyn_Exp*_tmp378;_LL286: if((int)
_tmp352 != 0)goto _LL288;_LL287:{const char*_tmp802;s=Cyc_PP_text(((_tmp802="_",
_tag_dyneither(_tmp802,sizeof(char),2))));}goto _LL285;_LL288: if((int)_tmp352 != 1)
goto _LL28A;_LL289:{const char*_tmp803;s=Cyc_PP_text(((_tmp803="NULL",
_tag_dyneither(_tmp803,sizeof(char),5))));}goto _LL285;_LL28A: if(_tmp352 <= (void*)
2)goto _LL28C;if(*((int*)_tmp352)!= 7)goto _LL28C;_tmp353=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp352)->f1;_tmp354=((struct Cyc_Absyn_Int_p_struct*)_tmp352)->f2;_LL28B: if(
_tmp353 != (void*)((void*)1)){const char*_tmp807;void*_tmp806[1];struct Cyc_Int_pa_struct
_tmp805;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp805.tag=1,((_tmp805.f1=(
unsigned long)_tmp354,((_tmp806[0]=& _tmp805,Cyc_aprintf(((_tmp807="%d",
_tag_dyneither(_tmp807,sizeof(char),3))),_tag_dyneither(_tmp806,sizeof(void*),1)))))))));}
else{const char*_tmp80B;void*_tmp80A[1];struct Cyc_Int_pa_struct _tmp809;s=Cyc_PP_text((
struct _dyneither_ptr)((_tmp809.tag=1,((_tmp809.f1=(unsigned int)_tmp354,((
_tmp80A[0]=& _tmp809,Cyc_aprintf(((_tmp80B="%u",_tag_dyneither(_tmp80B,sizeof(
char),3))),_tag_dyneither(_tmp80A,sizeof(void*),1)))))))));}goto _LL285;_LL28C:
if(_tmp352 <= (void*)2)goto _LL28E;if(*((int*)_tmp352)!= 8)goto _LL28E;_tmp355=((
struct Cyc_Absyn_Char_p_struct*)_tmp352)->f1;_LL28D:{const char*_tmp80F;void*
_tmp80E[1];struct Cyc_String_pa_struct _tmp80D;s=Cyc_PP_text((struct _dyneither_ptr)((
_tmp80D.tag=0,((_tmp80D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(
_tmp355)),((_tmp80E[0]=& _tmp80D,Cyc_aprintf(((_tmp80F="'%s'",_tag_dyneither(
_tmp80F,sizeof(char),5))),_tag_dyneither(_tmp80E,sizeof(void*),1)))))))));}goto
_LL285;_LL28E: if(_tmp352 <= (void*)2)goto _LL290;if(*((int*)_tmp352)!= 9)goto
_LL290;_tmp356=((struct Cyc_Absyn_Float_p_struct*)_tmp352)->f1;_LL28F: s=Cyc_PP_text(
_tmp356);goto _LL285;_LL290: if(_tmp352 <= (void*)2)goto _LL292;if(*((int*)_tmp352)
!= 0)goto _LL292;_tmp357=((struct Cyc_Absyn_Var_p_struct*)_tmp352)->f1;_tmp358=((
struct Cyc_Absyn_Var_p_struct*)_tmp352)->f2;_tmp359=*_tmp358;_tmp35A=(void*)
_tmp359.r;if((int)_tmp35A != 0)goto _LL292;_LL291: s=Cyc_Absynpp_qvar2doc(_tmp357->name);
goto _LL285;_LL292: if(_tmp352 <= (void*)2)goto _LL294;if(*((int*)_tmp352)!= 0)goto
_LL294;_tmp35B=((struct Cyc_Absyn_Var_p_struct*)_tmp352)->f1;_tmp35C=((struct Cyc_Absyn_Var_p_struct*)
_tmp352)->f2;_LL293:{const char*_tmp812;struct Cyc_PP_Doc*_tmp811[3];s=((_tmp811[2]=
Cyc_Absynpp_pat2doc(_tmp35C),((_tmp811[1]=Cyc_PP_text(((_tmp812=" as ",
_tag_dyneither(_tmp812,sizeof(char),5)))),((_tmp811[0]=Cyc_Absynpp_qvar2doc(
_tmp35B->name),Cyc_PP_cat(_tag_dyneither(_tmp811,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL285;_LL294: if(_tmp352 <= (void*)2)goto _LL296;if(*((int*)_tmp352)!= 2)goto
_LL296;_tmp35D=((struct Cyc_Absyn_TagInt_p_struct*)_tmp352)->f1;_tmp35E=((struct
Cyc_Absyn_TagInt_p_struct*)_tmp352)->f2;_LL295:{const char*_tmp817;const char*
_tmp816;struct Cyc_PP_Doc*_tmp815[4];s=((_tmp815[3]=Cyc_PP_text(((_tmp817=">",
_tag_dyneither(_tmp817,sizeof(char),2)))),((_tmp815[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp35D)),((_tmp815[1]=Cyc_PP_text(((_tmp816="<",_tag_dyneither(_tmp816,sizeof(
char),2)))),((_tmp815[0]=Cyc_Absynpp_qvar2doc(_tmp35E->name),Cyc_PP_cat(
_tag_dyneither(_tmp815,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL285;_LL296:
if(_tmp352 <= (void*)2)goto _LL298;if(*((int*)_tmp352)!= 3)goto _LL298;_tmp35F=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp352)->f1;_tmp360=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp352)->f2;_LL297:{const char*_tmp820;const char*_tmp81F;const char*_tmp81E;const
char*_tmp81D;struct Cyc_PP_Doc*_tmp81C[4];s=((_tmp81C[3]=_tmp360?Cyc_PP_text(((
_tmp81F=", ...)",_tag_dyneither(_tmp81F,sizeof(char),7)))): Cyc_PP_text(((_tmp820=")",
_tag_dyneither(_tmp820,sizeof(char),2)))),((_tmp81C[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp81E=",",_tag_dyneither(_tmp81E,
sizeof(char),2))),_tmp35F),((_tmp81C[1]=Cyc_PP_text(((_tmp81D="(",_tag_dyneither(
_tmp81D,sizeof(char),2)))),((_tmp81C[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(
_tag_dyneither(_tmp81C,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL285;_LL298:
if(_tmp352 <= (void*)2)goto _LL29A;if(*((int*)_tmp352)!= 4)goto _LL29A;_tmp361=((
struct Cyc_Absyn_Pointer_p_struct*)_tmp352)->f1;_LL299:{const char*_tmp823;struct
Cyc_PP_Doc*_tmp822[2];s=((_tmp822[1]=Cyc_Absynpp_pat2doc(_tmp361),((_tmp822[0]=
Cyc_PP_text(((_tmp823="&",_tag_dyneither(_tmp823,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp822,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL285;_LL29A: if(
_tmp352 <= (void*)2)goto _LL29C;if(*((int*)_tmp352)!= 1)goto _LL29C;_tmp362=((
struct Cyc_Absyn_Reference_p_struct*)_tmp352)->f1;_tmp363=((struct Cyc_Absyn_Reference_p_struct*)
_tmp352)->f2;_tmp364=*_tmp363;_tmp365=(void*)_tmp364.r;if((int)_tmp365 != 0)goto
_LL29C;_LL29B:{const char*_tmp826;struct Cyc_PP_Doc*_tmp825[2];s=((_tmp825[1]=Cyc_Absynpp_qvar2doc(
_tmp362->name),((_tmp825[0]=Cyc_PP_text(((_tmp826="*",_tag_dyneither(_tmp826,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp825,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL285;_LL29C: if(_tmp352 <= (void*)2)goto _LL29E;if(*((int*)_tmp352)!= 1)goto
_LL29E;_tmp366=((struct Cyc_Absyn_Reference_p_struct*)_tmp352)->f1;_tmp367=((
struct Cyc_Absyn_Reference_p_struct*)_tmp352)->f2;_LL29D:{const char*_tmp82B;const
char*_tmp82A;struct Cyc_PP_Doc*_tmp829[4];s=((_tmp829[3]=Cyc_Absynpp_pat2doc(
_tmp367),((_tmp829[2]=Cyc_PP_text(((_tmp82B=" as ",_tag_dyneither(_tmp82B,
sizeof(char),5)))),((_tmp829[1]=Cyc_Absynpp_qvar2doc(_tmp366->name),((_tmp829[0]=
Cyc_PP_text(((_tmp82A="*",_tag_dyneither(_tmp82A,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp829,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL285;_LL29E:
if(_tmp352 <= (void*)2)goto _LL2A0;if(*((int*)_tmp352)!= 12)goto _LL2A0;_tmp368=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp352)->f1;_LL29F: s=Cyc_Absynpp_qvar2doc(
_tmp368);goto _LL285;_LL2A0: if(_tmp352 <= (void*)2)goto _LL2A2;if(*((int*)_tmp352)
!= 13)goto _LL2A2;_tmp369=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp352)->f1;
_tmp36A=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp352)->f2;_tmp36B=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp352)->f3;_LL2A1: {const char*_tmp82D;const char*_tmp82C;struct _dyneither_ptr
term=_tmp36B?(_tmp82D=", ...)",_tag_dyneither(_tmp82D,sizeof(char),7)):((_tmp82C=")",
_tag_dyneither(_tmp82C,sizeof(char),2)));{const char*_tmp832;const char*_tmp831;
struct Cyc_PP_Doc*_tmp830[2];s=((_tmp830[1]=Cyc_PP_group(((_tmp832="(",
_tag_dyneither(_tmp832,sizeof(char),2))),term,((_tmp831=",",_tag_dyneither(
_tmp831,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp36A)),((
_tmp830[0]=Cyc_Absynpp_qvar2doc(_tmp369),Cyc_PP_cat(_tag_dyneither(_tmp830,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL285;}_LL2A2: if(_tmp352 <= (void*)2)goto
_LL2A4;if(*((int*)_tmp352)!= 5)goto _LL2A4;_tmp36C=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp352)->f1;_tmp36D=_tmp36C.aggr_info;_tmp36E=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp352)->f2;_tmp36F=((struct Cyc_Absyn_Aggr_p_struct*)_tmp352)->f3;_tmp370=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp352)->f4;_LL2A3: {const char*_tmp834;const char*
_tmp833;struct _dyneither_ptr term=_tmp370?(_tmp834=", ...}",_tag_dyneither(
_tmp834,sizeof(char),7)):((_tmp833="}",_tag_dyneither(_tmp833,sizeof(char),2)));
struct _tuple0*_tmp39B;struct _tuple3 _tmp39A=Cyc_Absyn_aggr_kinded_name(_tmp36D);
_tmp39B=_tmp39A.f2;{const char*_tmp83F;const char*_tmp83E;const char*_tmp83D;const
char*_tmp83C;const char*_tmp83B;struct Cyc_PP_Doc*_tmp83A[4];s=((_tmp83A[3]=Cyc_PP_group(((
_tmp83F="",_tag_dyneither(_tmp83F,sizeof(char),1))),term,((_tmp83E=",",
_tag_dyneither(_tmp83E,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp36F)),((_tmp83A[2]=Cyc_PP_egroup(((_tmp83D="[",_tag_dyneither(_tmp83D,
sizeof(char),2))),((_tmp83C="]",_tag_dyneither(_tmp83C,sizeof(char),2))),((
_tmp83B=",",_tag_dyneither(_tmp83B,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp36E))),((_tmp83A[1]=
Cyc_Absynpp_lb(),((_tmp83A[0]=Cyc_Absynpp_qvar2doc(_tmp39B),Cyc_PP_cat(
_tag_dyneither(_tmp83A,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL285;}_LL2A4:
if(_tmp352 <= (void*)2)goto _LL2A6;if(*((int*)_tmp352)!= 10)goto _LL2A6;_tmp371=((
struct Cyc_Absyn_Enum_p_struct*)_tmp352)->f2;_LL2A5: _tmp372=_tmp371;goto _LL2A7;
_LL2A6: if(_tmp352 <= (void*)2)goto _LL2A8;if(*((int*)_tmp352)!= 11)goto _LL2A8;
_tmp372=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp352)->f2;_LL2A7: s=Cyc_Absynpp_qvar2doc(
_tmp372->name);goto _LL285;_LL2A8: if(_tmp352 <= (void*)2)goto _LL2AA;if(*((int*)
_tmp352)!= 6)goto _LL2AA;_tmp373=((struct Cyc_Absyn_Tunion_p_struct*)_tmp352)->f2;
_tmp374=((struct Cyc_Absyn_Tunion_p_struct*)_tmp352)->f3;if(_tmp374 != 0)goto
_LL2AA;_LL2A9: s=Cyc_Absynpp_qvar2doc(_tmp373->name);goto _LL285;_LL2AA: if(_tmp352
<= (void*)2)goto _LL2AC;if(*((int*)_tmp352)!= 6)goto _LL2AC;_tmp375=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp352)->f2;_tmp376=((struct Cyc_Absyn_Tunion_p_struct*)_tmp352)->f3;_tmp377=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp352)->f4;_LL2AB: {const char*_tmp841;const
char*_tmp840;struct _dyneither_ptr term=_tmp377?(_tmp841=", ...)",_tag_dyneither(
_tmp841,sizeof(char),7)):((_tmp840=")",_tag_dyneither(_tmp840,sizeof(char),2)));{
const char*_tmp846;const char*_tmp845;struct Cyc_PP_Doc*_tmp844[2];s=((_tmp844[1]=
Cyc_PP_egroup(((_tmp846="(",_tag_dyneither(_tmp846,sizeof(char),2))),term,((
_tmp845=",",_tag_dyneither(_tmp845,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp376)),((_tmp844[0]=Cyc_Absynpp_qvar2doc(_tmp375->name),
Cyc_PP_cat(_tag_dyneither(_tmp844,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL285;}
_LL2AC: if(_tmp352 <= (void*)2)goto _LL285;if(*((int*)_tmp352)!= 14)goto _LL285;
_tmp378=((struct Cyc_Absyn_Exp_p_struct*)_tmp352)->f1;_LL2AD: s=Cyc_Absynpp_exp2doc(
_tmp378);goto _LL285;_LL285:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple8*dp);struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*dp){const char*
_tmp84D;const char*_tmp84C;const char*_tmp84B;struct Cyc_PP_Doc*_tmp84A[2];return(
_tmp84A[1]=Cyc_Absynpp_pat2doc((*dp).f2),((_tmp84A[0]=Cyc_PP_egroup(((_tmp84D="",
_tag_dyneither(_tmp84D,sizeof(char),1))),((_tmp84C="=",_tag_dyneither(_tmp84C,
sizeof(char),2))),((_tmp84B="=",_tag_dyneither(_tmp84B,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp84A,sizeof(
struct Cyc_PP_Doc*),2)))));}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c);struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (void*)(c->pattern)->r == (
void*)((void*)0)){struct Cyc_PP_Doc*_tmp852[2];const char*_tmp851;struct Cyc_PP_Doc*
_tmp850[2];return(_tmp850[1]=Cyc_PP_nest(2,((_tmp852[1]=Cyc_Absynpp_stmt2doc(c->body),((
_tmp852[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp852,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp850[0]=Cyc_PP_text(((_tmp851="default: ",_tag_dyneither(_tmp851,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp850,sizeof(struct Cyc_PP_Doc*),
2)))));}else{if(c->where_clause == 0){struct Cyc_PP_Doc*_tmp859[2];const char*
_tmp858;const char*_tmp857;struct Cyc_PP_Doc*_tmp856[4];return(_tmp856[3]=Cyc_PP_nest(
2,((_tmp859[1]=Cyc_Absynpp_stmt2doc(c->body),((_tmp859[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp859,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp856[2]=Cyc_PP_text(((
_tmp858=": ",_tag_dyneither(_tmp858,sizeof(char),3)))),((_tmp856[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp856[0]=Cyc_PP_text(((_tmp857="case ",_tag_dyneither(_tmp857,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp856,sizeof(struct Cyc_PP_Doc*),4)))))))));}
else{struct Cyc_PP_Doc*_tmp862[2];const char*_tmp861;const char*_tmp860;const char*
_tmp85F;struct Cyc_PP_Doc*_tmp85E[6];return(_tmp85E[5]=Cyc_PP_nest(2,((_tmp862[1]=
Cyc_Absynpp_stmt2doc(c->body),((_tmp862[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp862,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp85E[4]=Cyc_PP_text(((
_tmp861=": ",_tag_dyneither(_tmp861,sizeof(char),3)))),((_tmp85E[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp85E[2]=Cyc_PP_text(((
_tmp860=" && ",_tag_dyneither(_tmp860,sizeof(char),5)))),((_tmp85E[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp85E[0]=Cyc_PP_text(((_tmp85F="case ",_tag_dyneither(_tmp85F,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp85E,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){const char*_tmp863;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((
_tmp863="",_tag_dyneither(_tmp863,sizeof(char),1))),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct
Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);else{
const char*_tmp866;struct Cyc_PP_Doc*_tmp865[3];return(_tmp865[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp865[1]=Cyc_PP_text(((_tmp866=" = ",
_tag_dyneither(_tmp866,sizeof(char),4)))),((_tmp865[0]=Cyc_Absynpp_qvar2doc(f->name),
Cyc_PP_cat(_tag_dyneither(_tmp865,sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){const char*_tmp867;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp867=",",_tag_dyneither(_tmp867,
sizeof(char),2))),fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*
v);static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return
Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct
Cyc_List_List*vds);static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*
vds){const char*_tmp868;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_id2doc,((_tmp868=",",_tag_dyneither(_tmp868,sizeof(char),2))),vds);}
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);struct Cyc_PP_Doc*
Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl
_tmp3BF;void*_tmp3C0;struct _tuple0*_tmp3C1;struct Cyc_Absyn_Tqual _tmp3C2;void*
_tmp3C3;struct Cyc_Absyn_Exp*_tmp3C4;struct Cyc_List_List*_tmp3C5;struct Cyc_Absyn_Vardecl*
_tmp3BE=vd;_tmp3BF=*_tmp3BE;_tmp3C0=(void*)_tmp3BF.sc;_tmp3C1=_tmp3BF.name;
_tmp3C2=_tmp3BF.tq;_tmp3C3=(void*)_tmp3BF.type;_tmp3C4=_tmp3BF.initializer;
_tmp3C5=_tmp3BF.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(
_tmp3C1);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp3C5);struct Cyc_PP_Doc*
beforenamedoc;{void*_tmp3C6=Cyc_Cyclone_c_compiler;_LL2AF: if((int)_tmp3C6 != 0)
goto _LL2B1;_LL2B0: beforenamedoc=attsdoc;goto _LL2AE;_LL2B1: if((int)_tmp3C6 != 1)
goto _LL2AE;_LL2B2:{void*_tmp3C7=Cyc_Tcutil_compress(_tmp3C3);struct Cyc_Absyn_FnInfo
_tmp3C8;struct Cyc_List_List*_tmp3C9;_LL2B4: if(_tmp3C7 <= (void*)4)goto _LL2B6;if(*((
int*)_tmp3C7)!= 8)goto _LL2B6;_tmp3C8=((struct Cyc_Absyn_FnType_struct*)_tmp3C7)->f1;
_tmp3C9=_tmp3C8.attributes;_LL2B5: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp3C9);
goto _LL2B3;_LL2B6:;_LL2B7: beforenamedoc=Cyc_PP_nil_doc();goto _LL2B3;_LL2B3:;}
goto _LL2AE;_LL2AE:;}{struct Cyc_PP_Doc*tmp_doc;{void*_tmp3CA=Cyc_Cyclone_c_compiler;
_LL2B9: if((int)_tmp3CA != 0)goto _LL2BB;_LL2BA: tmp_doc=Cyc_PP_nil_doc();goto _LL2B8;
_LL2BB: if((int)_tmp3CA != 1)goto _LL2B8;_LL2BC: tmp_doc=attsdoc;goto _LL2B8;_LL2B8:;}{
const char*_tmp875;struct Cyc_PP_Doc*_tmp874[2];const char*_tmp873;struct Cyc_Core_Opt*
_tmp872;struct Cyc_PP_Doc*_tmp871[2];struct Cyc_PP_Doc*_tmp870[5];s=((_tmp870[4]=
Cyc_PP_text(((_tmp875=";",_tag_dyneither(_tmp875,sizeof(char),2)))),((_tmp870[3]=
_tmp3C4 == 0?Cyc_PP_nil_doc():((_tmp874[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp3C4),((_tmp874[0]=Cyc_PP_text(((_tmp873=" = ",_tag_dyneither(_tmp873,sizeof(
char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp874,sizeof(struct Cyc_PP_Doc*),2)))))),((
_tmp870[2]=Cyc_Absynpp_tqtd2doc(_tmp3C2,_tmp3C3,((_tmp872=_cycalloc(sizeof(*
_tmp872)),((_tmp872->v=((_tmp871[1]=sn,((_tmp871[0]=beforenamedoc,Cyc_PP_cat(
_tag_dyneither(_tmp871,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp872))))),((_tmp870[
1]=Cyc_Absynpp_scope2doc(_tmp3C0),((_tmp870[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(
_tmp870,sizeof(struct Cyc_PP_Doc*),5))))))))))));}return s;}}}struct _tuple11{
struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(
struct _tuple11*x);struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple11*x){
struct _tuple0*_tmp3D2;struct _tuple11 _tmp3D1=*x;_tmp3D2=_tmp3D1.f2;return Cyc_Absynpp_qvar2doc(
_tmp3D2);}struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);struct
Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*
_tmp3D3=(void*)d->r;struct Cyc_Absyn_Fndecl*_tmp3D4;struct Cyc_Absyn_Aggrdecl*
_tmp3D5;struct Cyc_Absyn_Vardecl*_tmp3D6;struct Cyc_Absyn_Tvar*_tmp3D7;struct Cyc_Absyn_Vardecl*
_tmp3D8;int _tmp3D9;struct Cyc_Absyn_Exp*_tmp3DA;struct Cyc_Absyn_Exp*_tmp3DB;
struct Cyc_Absyn_Tvar*_tmp3DC;struct Cyc_Absyn_Vardecl*_tmp3DD;struct Cyc_Absyn_Tuniondecl*
_tmp3DE;struct Cyc_Absyn_Tuniondecl _tmp3DF;void*_tmp3E0;struct _tuple0*_tmp3E1;
struct Cyc_List_List*_tmp3E2;struct Cyc_Core_Opt*_tmp3E3;int _tmp3E4;int _tmp3E5;
struct Cyc_Absyn_Pat*_tmp3E6;struct Cyc_Absyn_Exp*_tmp3E7;struct Cyc_List_List*
_tmp3E8;struct Cyc_Absyn_Enumdecl*_tmp3E9;struct Cyc_Absyn_Enumdecl _tmp3EA;void*
_tmp3EB;struct _tuple0*_tmp3EC;struct Cyc_Core_Opt*_tmp3ED;struct Cyc_Absyn_Typedefdecl*
_tmp3EE;struct _dyneither_ptr*_tmp3EF;struct Cyc_List_List*_tmp3F0;struct _tuple0*
_tmp3F1;struct Cyc_List_List*_tmp3F2;struct Cyc_List_List*_tmp3F3;struct Cyc_List_List*
_tmp3F4;struct Cyc_List_List*_tmp3F5;_LL2BE: if(_tmp3D3 <= (void*)2)goto _LL2DA;if(*((
int*)_tmp3D3)!= 1)goto _LL2C0;_tmp3D4=((struct Cyc_Absyn_Fn_d_struct*)_tmp3D3)->f1;
_LL2BF: {struct Cyc_Absyn_FnType_struct _tmp87B;struct Cyc_Absyn_FnInfo _tmp87A;
struct Cyc_Absyn_FnType_struct*_tmp879;void*t=(void*)((_tmp879=_cycalloc(sizeof(*
_tmp879)),((_tmp879[0]=((_tmp87B.tag=8,((_tmp87B.f1=((_tmp87A.tvars=_tmp3D4->tvs,((
_tmp87A.effect=_tmp3D4->effect,((_tmp87A.ret_typ=(void*)((void*)_tmp3D4->ret_type),((
_tmp87A.args=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple6*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp3D4->args),((_tmp87A.c_varargs=
_tmp3D4->c_varargs,((_tmp87A.cyc_varargs=_tmp3D4->cyc_varargs,((_tmp87A.rgn_po=
_tmp3D4->rgn_po,((_tmp87A.attributes=0,_tmp87A)))))))))))))))),_tmp87B)))),
_tmp879))));struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp3D4->attributes);
struct Cyc_PP_Doc*inlinedoc;if(_tmp3D4->is_inline){void*_tmp3F6=Cyc_Cyclone_c_compiler;
_LL2DF: if((int)_tmp3F6 != 0)goto _LL2E1;_LL2E0:{const char*_tmp87C;inlinedoc=Cyc_PP_text(((
_tmp87C="inline ",_tag_dyneither(_tmp87C,sizeof(char),8))));}goto _LL2DE;_LL2E1:
if((int)_tmp3F6 != 1)goto _LL2DE;_LL2E2:{const char*_tmp87D;inlinedoc=Cyc_PP_text(((
_tmp87D="__inline ",_tag_dyneither(_tmp87D,sizeof(char),10))));}goto _LL2DE;
_LL2DE:;}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc((
void*)_tmp3D4->sc);struct Cyc_PP_Doc*beforenamedoc;{void*_tmp3F9=Cyc_Cyclone_c_compiler;
_LL2E4: if((int)_tmp3F9 != 0)goto _LL2E6;_LL2E5: beforenamedoc=attsdoc;goto _LL2E3;
_LL2E6: if((int)_tmp3F9 != 1)goto _LL2E3;_LL2E7: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp3D4->attributes);goto _LL2E3;_LL2E3:;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp3D4->name);struct Cyc_PP_Doc*_tmp880[2];struct Cyc_Core_Opt*_tmp87F;struct Cyc_PP_Doc*
tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,((_tmp87F=_cycalloc(
sizeof(*_tmp87F)),((_tmp87F->v=((_tmp880[1]=namedoc,((_tmp880[0]=beforenamedoc,
Cyc_PP_cat(_tag_dyneither(_tmp880,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp87F)))));
struct Cyc_PP_Doc*_tmp883[2];struct Cyc_PP_Doc*_tmp882[5];struct Cyc_PP_Doc*bodydoc=(
_tmp882[4]=Cyc_Absynpp_rb(),((_tmp882[3]=Cyc_PP_line_doc(),((_tmp882[2]=Cyc_PP_nest(
2,((_tmp883[1]=Cyc_Absynpp_stmt2doc(_tmp3D4->body),((_tmp883[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dyneither(_tmp883,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp882[1]=
Cyc_Absynpp_lb(),((_tmp882[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(
_tmp882,sizeof(struct Cyc_PP_Doc*),5)))))))))));{struct Cyc_PP_Doc*_tmp884[4];s=((
_tmp884[3]=bodydoc,((_tmp884[2]=tqtddoc,((_tmp884[1]=scopedoc,((_tmp884[0]=
inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp884,sizeof(struct Cyc_PP_Doc*),4))))))))));}{
void*_tmp3FB=Cyc_Cyclone_c_compiler;_LL2E9: if((int)_tmp3FB != 1)goto _LL2EB;_LL2EA:{
struct Cyc_PP_Doc*_tmp885[2];s=((_tmp885[1]=s,((_tmp885[0]=attsdoc,Cyc_PP_cat(
_tag_dyneither(_tmp885,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL2E8;_LL2EB:;
_LL2EC: goto _LL2E8;_LL2E8:;}goto _LL2BD;}}}_LL2C0: if(*((int*)_tmp3D3)!= 6)goto
_LL2C2;_tmp3D5=((struct Cyc_Absyn_Aggr_d_struct*)_tmp3D3)->f1;_LL2C1: if(_tmp3D5->impl
== 0){const char*_tmp888;struct Cyc_PP_Doc*_tmp887[5];s=((_tmp887[4]=Cyc_PP_text(((
_tmp888=";",_tag_dyneither(_tmp888,sizeof(char),2)))),((_tmp887[3]=Cyc_Absynpp_ktvars2doc(
_tmp3D5->tvs),((_tmp887[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3D5->name),((_tmp887[1]=
Cyc_Absynpp_aggr_kind2doc((void*)_tmp3D5->kind),((_tmp887[0]=Cyc_Absynpp_scope2doc((
void*)_tmp3D5->sc),Cyc_PP_cat(_tag_dyneither(_tmp887,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
else{const char*_tmp892;struct Cyc_PP_Doc*_tmp891[2];struct Cyc_PP_Doc*_tmp890[2];
const char*_tmp88F;struct Cyc_PP_Doc*_tmp88E[12];s=((_tmp88E[11]=Cyc_PP_text(((
_tmp892=";",_tag_dyneither(_tmp892,sizeof(char),2)))),((_tmp88E[10]=Cyc_Absynpp_atts2doc(
_tmp3D5->attributes),((_tmp88E[9]=Cyc_Absynpp_rb(),((_tmp88E[8]=Cyc_PP_line_doc(),((
_tmp88E[7]=Cyc_PP_nest(2,((_tmp891[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3D5->impl))->fields),((_tmp891[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp891,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp88E[6]=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3D5->impl))->rgn_po == 0?Cyc_PP_nil_doc():((
_tmp890[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp3D5->impl))->rgn_po),((_tmp890[0]=Cyc_PP_text(((_tmp88F=":",_tag_dyneither(
_tmp88F,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp890,sizeof(struct Cyc_PP_Doc*),
2)))))),((_tmp88E[5]=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3D5->impl))->exist_vars),((_tmp88E[4]=Cyc_Absynpp_lb(),((_tmp88E[
3]=Cyc_PP_blank_doc(),((_tmp88E[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3D5->name),((
_tmp88E[1]=Cyc_Absynpp_aggr_kind2doc((void*)_tmp3D5->kind),((_tmp88E[0]=Cyc_Absynpp_scope2doc((
void*)_tmp3D5->sc),Cyc_PP_cat(_tag_dyneither(_tmp88E,sizeof(struct Cyc_PP_Doc*),
12))))))))))))))))))))))))));}goto _LL2BD;_LL2C2: if(*((int*)_tmp3D3)!= 0)goto
_LL2C4;_tmp3D6=((struct Cyc_Absyn_Var_d_struct*)_tmp3D3)->f1;_LL2C3: s=Cyc_Absynpp_vardecl2doc(
_tmp3D6);goto _LL2BD;_LL2C4: if(*((int*)_tmp3D3)!= 4)goto _LL2C6;_tmp3D7=((struct
Cyc_Absyn_Region_d_struct*)_tmp3D3)->f1;_tmp3D8=((struct Cyc_Absyn_Region_d_struct*)
_tmp3D3)->f2;_tmp3D9=((struct Cyc_Absyn_Region_d_struct*)_tmp3D3)->f3;_tmp3DA=((
struct Cyc_Absyn_Region_d_struct*)_tmp3D3)->f4;_LL2C5:{const char*_tmp8A5;struct
Cyc_PP_Doc*_tmp8A4[3];const char*_tmp8A3;const char*_tmp8A2;const char*_tmp8A1;
const char*_tmp8A0;const char*_tmp89F;const char*_tmp89E;struct Cyc_PP_Doc*_tmp89D[8];
s=((_tmp89D[7]=Cyc_PP_text(((_tmp8A5=";",_tag_dyneither(_tmp8A5,sizeof(char),2)))),((
_tmp89D[6]=_tmp3DA != 0?(_tmp8A4[2]=Cyc_PP_text(((_tmp8A2=")",_tag_dyneither(
_tmp8A2,sizeof(char),2)))),((_tmp8A4[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp3DA),((_tmp8A4[0]=Cyc_PP_text(((_tmp8A3=" = open(",_tag_dyneither(_tmp8A3,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8A4,sizeof(struct Cyc_PP_Doc*),3))))))):
Cyc_PP_nil_doc(),((_tmp89D[5]=Cyc_Absynpp_qvar2doc(_tmp3D8->name),((_tmp89D[4]=
Cyc_PP_text(((_tmp8A1=">",_tag_dyneither(_tmp8A1,sizeof(char),2)))),((_tmp89D[3]=
Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp3D7)),((_tmp89D[2]=Cyc_PP_text(((_tmp8A0="<",
_tag_dyneither(_tmp8A0,sizeof(char),2)))),((_tmp89D[1]=_tmp3D9?Cyc_PP_nil_doc():
Cyc_PP_text(((_tmp89F="[resetable]",_tag_dyneither(_tmp89F,sizeof(char),12)))),((
_tmp89D[0]=Cyc_PP_text(((_tmp89E="region",_tag_dyneither(_tmp89E,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmp89D,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL2BD;_LL2C6: if(*((int*)_tmp3D3)!= 5)goto _LL2C8;_tmp3DB=((struct Cyc_Absyn_Alias_d_struct*)
_tmp3D3)->f1;_tmp3DC=((struct Cyc_Absyn_Alias_d_struct*)_tmp3D3)->f2;_tmp3DD=((
struct Cyc_Absyn_Alias_d_struct*)_tmp3D3)->f3;_LL2C7:{const char*_tmp8B0;const char*
_tmp8AF;const char*_tmp8AE;const char*_tmp8AD;const char*_tmp8AC;struct Cyc_PP_Doc*
_tmp8AB[8];s=((_tmp8AB[7]=Cyc_PP_text(((_tmp8B0=";",_tag_dyneither(_tmp8B0,
sizeof(char),2)))),((_tmp8AB[6]=Cyc_Absynpp_exp2doc(_tmp3DB),((_tmp8AB[5]=Cyc_PP_text(((
_tmp8AF=" = ",_tag_dyneither(_tmp8AF,sizeof(char),4)))),((_tmp8AB[4]=Cyc_Absynpp_qvar2doc(
_tmp3DD->name),((_tmp8AB[3]=Cyc_PP_text(((_tmp8AE="> ",_tag_dyneither(_tmp8AE,
sizeof(char),3)))),((_tmp8AB[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp3DC)),((
_tmp8AB[1]=Cyc_PP_text(((_tmp8AD=" <",_tag_dyneither(_tmp8AD,sizeof(char),3)))),((
_tmp8AB[0]=Cyc_PP_text(((_tmp8AC="alias",_tag_dyneither(_tmp8AC,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp8AB,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL2BD;_LL2C8: if(*((int*)_tmp3D3)!= 7)goto _LL2CA;_tmp3DE=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp3D3)->f1;_tmp3DF=*_tmp3DE;_tmp3E0=(void*)_tmp3DF.sc;_tmp3E1=_tmp3DF.name;
_tmp3E2=_tmp3DF.tvs;_tmp3E3=_tmp3DF.fields;_tmp3E4=_tmp3DF.is_xtunion;_tmp3E5=
_tmp3DF.is_flat;_LL2C9: if(_tmp3E3 == 0){const char*_tmp8B9;const char*_tmp8B8;const
char*_tmp8B7;const char*_tmp8B6;struct Cyc_PP_Doc*_tmp8B5[6];s=((_tmp8B5[5]=Cyc_PP_text(((
_tmp8B9=";",_tag_dyneither(_tmp8B9,sizeof(char),2)))),((_tmp8B5[4]=Cyc_Absynpp_ktvars2doc(
_tmp3E2),((_tmp8B5[3]=_tmp3E4?Cyc_Absynpp_qvar2bolddoc(_tmp3E1): Cyc_Absynpp_typedef_name2bolddoc(
_tmp3E1),((_tmp8B5[2]=_tmp3E5?Cyc_PP_text(((_tmp8B8="__attribute__((flat)) ",
_tag_dyneither(_tmp8B8,sizeof(char),23)))): Cyc_PP_blank_doc(),((_tmp8B5[1]=
_tmp3E4?Cyc_PP_text(((_tmp8B6="xtunion ",_tag_dyneither(_tmp8B6,sizeof(char),9)))):
Cyc_PP_text(((_tmp8B7="tunion ",_tag_dyneither(_tmp8B7,sizeof(char),8)))),((
_tmp8B5[0]=Cyc_Absynpp_scope2doc(_tmp3E0),Cyc_PP_cat(_tag_dyneither(_tmp8B5,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}else{const char*_tmp8C4;struct Cyc_PP_Doc*
_tmp8C3[2];const char*_tmp8C2;const char*_tmp8C1;const char*_tmp8C0;struct Cyc_PP_Doc*
_tmp8BF[11];s=((_tmp8BF[10]=Cyc_PP_text(((_tmp8C4=";",_tag_dyneither(_tmp8C4,
sizeof(char),2)))),((_tmp8BF[9]=Cyc_Absynpp_rb(),((_tmp8BF[8]=Cyc_PP_line_doc(),((
_tmp8BF[7]=Cyc_PP_nest(2,((_tmp8C3[1]=Cyc_Absynpp_tunionfields2doc((struct Cyc_List_List*)
_tmp3E3->v),((_tmp8C3[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8C3,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8BF[6]=Cyc_Absynpp_lb(),((_tmp8BF[5]=
Cyc_PP_blank_doc(),((_tmp8BF[4]=_tmp3E5?Cyc_PP_text(((_tmp8C2="__attribute__((flat)) ",
_tag_dyneither(_tmp8C2,sizeof(char),23)))): Cyc_PP_blank_doc(),((_tmp8BF[3]=Cyc_Absynpp_ktvars2doc(
_tmp3E2),((_tmp8BF[2]=_tmp3E4?Cyc_Absynpp_qvar2bolddoc(_tmp3E1): Cyc_Absynpp_typedef_name2bolddoc(
_tmp3E1),((_tmp8BF[1]=_tmp3E4?Cyc_PP_text(((_tmp8C0="xtunion ",_tag_dyneither(
_tmp8C0,sizeof(char),9)))): Cyc_PP_text(((_tmp8C1="tunion ",_tag_dyneither(
_tmp8C1,sizeof(char),8)))),((_tmp8BF[0]=Cyc_Absynpp_scope2doc(_tmp3E0),Cyc_PP_cat(
_tag_dyneither(_tmp8BF,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL2BD;_LL2CA: if(*((int*)_tmp3D3)!= 2)goto _LL2CC;_tmp3E6=((struct Cyc_Absyn_Let_d_struct*)
_tmp3D3)->f1;_tmp3E7=((struct Cyc_Absyn_Let_d_struct*)_tmp3D3)->f3;_LL2CB:{const
char*_tmp8CB;const char*_tmp8CA;const char*_tmp8C9;struct Cyc_PP_Doc*_tmp8C8[5];s=((
_tmp8C8[4]=Cyc_PP_text(((_tmp8CB=";",_tag_dyneither(_tmp8CB,sizeof(char),2)))),((
_tmp8C8[3]=Cyc_Absynpp_exp2doc(_tmp3E7),((_tmp8C8[2]=Cyc_PP_text(((_tmp8CA=" = ",
_tag_dyneither(_tmp8CA,sizeof(char),4)))),((_tmp8C8[1]=Cyc_Absynpp_pat2doc(
_tmp3E6),((_tmp8C8[0]=Cyc_PP_text(((_tmp8C9="let ",_tag_dyneither(_tmp8C9,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8C8,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL2BD;_LL2CC: if(*((int*)_tmp3D3)!= 3)goto _LL2CE;_tmp3E8=((struct Cyc_Absyn_Letv_d_struct*)
_tmp3D3)->f1;_LL2CD:{const char*_tmp8D0;const char*_tmp8CF;struct Cyc_PP_Doc*
_tmp8CE[3];s=((_tmp8CE[2]=Cyc_PP_text(((_tmp8D0=";",_tag_dyneither(_tmp8D0,
sizeof(char),2)))),((_tmp8CE[1]=Cyc_Absynpp_ids2doc(_tmp3E8),((_tmp8CE[0]=Cyc_PP_text(((
_tmp8CF="let ",_tag_dyneither(_tmp8CF,sizeof(char),5)))),Cyc_PP_cat(
_tag_dyneither(_tmp8CE,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL2BD;_LL2CE:
if(*((int*)_tmp3D3)!= 8)goto _LL2D0;_tmp3E9=((struct Cyc_Absyn_Enum_d_struct*)
_tmp3D3)->f1;_tmp3EA=*_tmp3E9;_tmp3EB=(void*)_tmp3EA.sc;_tmp3EC=_tmp3EA.name;
_tmp3ED=_tmp3EA.fields;_LL2CF: if(_tmp3ED == 0){const char*_tmp8D5;const char*
_tmp8D4;struct Cyc_PP_Doc*_tmp8D3[4];s=((_tmp8D3[3]=Cyc_PP_text(((_tmp8D5=";",
_tag_dyneither(_tmp8D5,sizeof(char),2)))),((_tmp8D3[2]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp3EC),((_tmp8D3[1]=Cyc_PP_text(((_tmp8D4="enum ",_tag_dyneither(_tmp8D4,
sizeof(char),6)))),((_tmp8D3[0]=Cyc_Absynpp_scope2doc(_tmp3EB),Cyc_PP_cat(
_tag_dyneither(_tmp8D3,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{const char*
_tmp8DC;struct Cyc_PP_Doc*_tmp8DB[2];const char*_tmp8DA;struct Cyc_PP_Doc*_tmp8D9[9];
s=((_tmp8D9[8]=Cyc_PP_text(((_tmp8DC=";",_tag_dyneither(_tmp8DC,sizeof(char),2)))),((
_tmp8D9[7]=Cyc_Absynpp_rb(),((_tmp8D9[6]=Cyc_PP_line_doc(),((_tmp8D9[5]=Cyc_PP_nest(
2,((_tmp8DB[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp3ED->v),((
_tmp8DB[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8DB,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp8D9[4]=Cyc_Absynpp_lb(),((_tmp8D9[3]=Cyc_PP_blank_doc(),((_tmp8D9[
2]=Cyc_Absynpp_qvar2bolddoc(_tmp3EC),((_tmp8D9[1]=Cyc_PP_text(((_tmp8DA="enum ",
_tag_dyneither(_tmp8DA,sizeof(char),6)))),((_tmp8D9[0]=Cyc_Absynpp_scope2doc(
_tmp3EB),Cyc_PP_cat(_tag_dyneither(_tmp8D9,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
goto _LL2BD;_LL2D0: if(*((int*)_tmp3D3)!= 9)goto _LL2D2;_tmp3EE=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp3D3)->f1;_LL2D1: {void*t;if(_tmp3EE->defn != 0)t=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3EE->defn))->v;else{t=Cyc_Absyn_new_evar(_tmp3EE->kind,0);}{
const char*_tmp8E6;struct Cyc_Core_Opt*_tmp8E5;struct Cyc_PP_Doc*_tmp8E4[2];const
char*_tmp8E3;struct Cyc_PP_Doc*_tmp8E2[4];s=((_tmp8E2[3]=Cyc_PP_text(((_tmp8E6=";",
_tag_dyneither(_tmp8E6,sizeof(char),2)))),((_tmp8E2[2]=Cyc_Absynpp_atts2doc(
_tmp3EE->atts),((_tmp8E2[1]=Cyc_Absynpp_tqtd2doc(_tmp3EE->tq,t,((_tmp8E5=
_cycalloc(sizeof(*_tmp8E5)),((_tmp8E5->v=((_tmp8E4[1]=Cyc_Absynpp_tvars2doc(
_tmp3EE->tvs),((_tmp8E4[0]=Cyc_Absynpp_typedef_name2bolddoc(_tmp3EE->name),Cyc_PP_cat(
_tag_dyneither(_tmp8E4,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp8E5))))),((_tmp8E2[
0]=Cyc_PP_text(((_tmp8E3="typedef ",_tag_dyneither(_tmp8E3,sizeof(char),9)))),
Cyc_PP_cat(_tag_dyneither(_tmp8E2,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL2BD;}_LL2D2: if(*((int*)_tmp3D3)!= 10)goto _LL2D4;_tmp3EF=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp3D3)->f1;_tmp3F0=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3D3)->f2;_LL2D3:
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp3EF);{const
char*_tmp8EB;const char*_tmp8EA;struct Cyc_PP_Doc*_tmp8E9[8];s=((_tmp8E9[7]=Cyc_Absynpp_rb(),((
_tmp8E9[6]=Cyc_PP_line_doc(),((_tmp8E9[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp8EB="",_tag_dyneither(_tmp8EB,sizeof(char),1))),
_tmp3F0),((_tmp8E9[4]=Cyc_PP_line_doc(),((_tmp8E9[3]=Cyc_Absynpp_lb(),((_tmp8E9[
2]=Cyc_PP_blank_doc(),((_tmp8E9[1]=Cyc_PP_textptr(_tmp3EF),((_tmp8E9[0]=Cyc_PP_text(((
_tmp8EA="namespace ",_tag_dyneither(_tmp8EA,sizeof(char),11)))),Cyc_PP_cat(
_tag_dyneither(_tmp8E9,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}if(Cyc_Absynpp_use_curr_namespace)
Cyc_Absynpp_curr_namespace_drop();goto _LL2BD;_LL2D4: if(*((int*)_tmp3D3)!= 11)
goto _LL2D6;_tmp3F1=((struct Cyc_Absyn_Using_d_struct*)_tmp3D3)->f1;_tmp3F2=((
struct Cyc_Absyn_Using_d_struct*)_tmp3D3)->f2;_LL2D5: if(Cyc_Absynpp_print_using_stmts){
const char*_tmp8F0;const char*_tmp8EF;struct Cyc_PP_Doc*_tmp8EE[8];s=((_tmp8EE[7]=
Cyc_Absynpp_rb(),((_tmp8EE[6]=Cyc_PP_line_doc(),((_tmp8EE[5]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmp8F0="",_tag_dyneither(_tmp8F0,
sizeof(char),1))),_tmp3F2),((_tmp8EE[4]=Cyc_PP_line_doc(),((_tmp8EE[3]=Cyc_Absynpp_lb(),((
_tmp8EE[2]=Cyc_PP_blank_doc(),((_tmp8EE[1]=Cyc_Absynpp_qvar2doc(_tmp3F1),((
_tmp8EE[0]=Cyc_PP_text(((_tmp8EF="using ",_tag_dyneither(_tmp8EF,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmp8EE,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
else{const char*_tmp8FB;const char*_tmp8FA;const char*_tmp8F9;const char*_tmp8F8;
const char*_tmp8F7;struct Cyc_PP_Doc*_tmp8F6[11];s=((_tmp8F6[10]=Cyc_PP_text(((
_tmp8FB=" */",_tag_dyneither(_tmp8FB,sizeof(char),4)))),((_tmp8F6[9]=Cyc_Absynpp_rb(),((
_tmp8F6[8]=Cyc_PP_text(((_tmp8FA="/* ",_tag_dyneither(_tmp8FA,sizeof(char),4)))),((
_tmp8F6[7]=Cyc_PP_line_doc(),((_tmp8F6[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp8F9="",_tag_dyneither(_tmp8F9,sizeof(char),1))),
_tmp3F2),((_tmp8F6[5]=Cyc_PP_line_doc(),((_tmp8F6[4]=Cyc_PP_text(((_tmp8F8=" */",
_tag_dyneither(_tmp8F8,sizeof(char),4)))),((_tmp8F6[3]=Cyc_Absynpp_lb(),((
_tmp8F6[2]=Cyc_PP_blank_doc(),((_tmp8F6[1]=Cyc_Absynpp_qvar2doc(_tmp3F1),((
_tmp8F6[0]=Cyc_PP_text(((_tmp8F7="/* using ",_tag_dyneither(_tmp8F7,sizeof(char),
10)))),Cyc_PP_cat(_tag_dyneither(_tmp8F6,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL2BD;_LL2D6: if(*((int*)_tmp3D3)!= 12)goto _LL2D8;_tmp3F3=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp3D3)->f1;_LL2D7: if(Cyc_Absynpp_print_externC_stmts){const char*_tmp900;const
char*_tmp8FF;struct Cyc_PP_Doc*_tmp8FE[6];s=((_tmp8FE[5]=Cyc_Absynpp_rb(),((
_tmp8FE[4]=Cyc_PP_line_doc(),((_tmp8FE[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp900="",_tag_dyneither(_tmp900,sizeof(char),1))),
_tmp3F3),((_tmp8FE[2]=Cyc_PP_line_doc(),((_tmp8FE[1]=Cyc_Absynpp_lb(),((_tmp8FE[
0]=Cyc_PP_text(((_tmp8FF="extern \"C\" ",_tag_dyneither(_tmp8FF,sizeof(char),12)))),
Cyc_PP_cat(_tag_dyneither(_tmp8FE,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmp90B;const char*_tmp90A;const char*_tmp909;const char*_tmp908;
const char*_tmp907;struct Cyc_PP_Doc*_tmp906[9];s=((_tmp906[8]=Cyc_PP_text(((
_tmp90B=" */",_tag_dyneither(_tmp90B,sizeof(char),4)))),((_tmp906[7]=Cyc_Absynpp_rb(),((
_tmp906[6]=Cyc_PP_text(((_tmp90A="/* ",_tag_dyneither(_tmp90A,sizeof(char),4)))),((
_tmp906[5]=Cyc_PP_line_doc(),((_tmp906[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp909="",_tag_dyneither(_tmp909,sizeof(char),1))),
_tmp3F3),((_tmp906[3]=Cyc_PP_line_doc(),((_tmp906[2]=Cyc_PP_text(((_tmp908=" */",
_tag_dyneither(_tmp908,sizeof(char),4)))),((_tmp906[1]=Cyc_Absynpp_lb(),((
_tmp906[0]=Cyc_PP_text(((_tmp907="/* extern \"C\" ",_tag_dyneither(_tmp907,
sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp906,sizeof(struct Cyc_PP_Doc*),
9))))))))))))))))))));}goto _LL2BD;_LL2D8: if(*((int*)_tmp3D3)!= 13)goto _LL2DA;
_tmp3F4=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp3D3)->f1;_tmp3F5=((struct
Cyc_Absyn_ExternCinclude_d_struct*)_tmp3D3)->f2;_LL2D9: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp3F5 != 0){const char*_tmp910;const char*_tmp90F;
struct Cyc_PP_Doc*_tmp90E[7];exs_doc=((_tmp90E[6]=Cyc_Absynpp_rb(),((_tmp90E[5]=
Cyc_PP_line_doc(),((_tmp90E[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple11*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_export2doc,((_tmp910=",",_tag_dyneither(_tmp910,sizeof(char),2))),
_tmp3F5),((_tmp90E[3]=Cyc_PP_line_doc(),((_tmp90E[2]=Cyc_Absynpp_lb(),((_tmp90E[
1]=Cyc_PP_text(((_tmp90F=" export ",_tag_dyneither(_tmp90F,sizeof(char),9)))),((
_tmp90E[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmp90E,sizeof(struct Cyc_PP_Doc*),
7))))))))))))))));}else{exs_doc=Cyc_Absynpp_rb();}{const char*_tmp915;const char*
_tmp914;struct Cyc_PP_Doc*_tmp913[6];s=((_tmp913[5]=exs_doc,((_tmp913[4]=Cyc_PP_line_doc(),((
_tmp913[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmp915="",_tag_dyneither(_tmp915,sizeof(char),1))),_tmp3F4),((_tmp913[2]=Cyc_PP_line_doc(),((
_tmp913[1]=Cyc_Absynpp_lb(),((_tmp913[0]=Cyc_PP_text(((_tmp914="extern \"C include\" ",
_tag_dyneither(_tmp914,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmp913,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}}else{const char*_tmp920;const char*
_tmp91F;const char*_tmp91E;const char*_tmp91D;const char*_tmp91C;struct Cyc_PP_Doc*
_tmp91B[9];s=((_tmp91B[8]=Cyc_PP_text(((_tmp920=" */",_tag_dyneither(_tmp920,
sizeof(char),4)))),((_tmp91B[7]=Cyc_Absynpp_rb(),((_tmp91B[6]=Cyc_PP_text(((
_tmp91F="/* ",_tag_dyneither(_tmp91F,sizeof(char),4)))),((_tmp91B[5]=Cyc_PP_line_doc(),((
_tmp91B[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmp91E="",_tag_dyneither(_tmp91E,sizeof(char),1))),_tmp3F4),((_tmp91B[3]=Cyc_PP_line_doc(),((
_tmp91B[2]=Cyc_PP_text(((_tmp91D=" */",_tag_dyneither(_tmp91D,sizeof(char),4)))),((
_tmp91B[1]=Cyc_Absynpp_lb(),((_tmp91B[0]=Cyc_PP_text(((_tmp91C="/* extern \"C include\" ",
_tag_dyneither(_tmp91C,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmp91B,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL2BD;_LL2DA: if((int)
_tmp3D3 != 0)goto _LL2DC;_LL2DB:{const char*_tmp923;struct Cyc_PP_Doc*_tmp922[2];s=((
_tmp922[1]=Cyc_Absynpp_lb(),((_tmp922[0]=Cyc_PP_text(((_tmp923="__cyclone_port_on__;",
_tag_dyneither(_tmp923,sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmp922,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL2BD;_LL2DC: if((int)_tmp3D3 != 1)goto
_LL2BD;_LL2DD:{const char*_tmp926;struct Cyc_PP_Doc*_tmp925[2];s=((_tmp925[1]=Cyc_Absynpp_lb(),((
_tmp925[0]=Cyc_PP_text(((_tmp926="__cyclone_port_off__;",_tag_dyneither(_tmp926,
sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmp925,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL2BD;_LL2BD:;}return s;}int Cyc_Absynpp_print_scopes=1;struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(void*sc);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*sc){
if(!Cyc_Absynpp_print_scopes)return Cyc_PP_nil_doc();{void*_tmp45D=sc;_LL2EE: if((
int)_tmp45D != 0)goto _LL2F0;_LL2EF: {const char*_tmp927;return Cyc_PP_text(((
_tmp927="static ",_tag_dyneither(_tmp927,sizeof(char),8))));}_LL2F0: if((int)
_tmp45D != 2)goto _LL2F2;_LL2F1: return Cyc_PP_nil_doc();_LL2F2: if((int)_tmp45D != 3)
goto _LL2F4;_LL2F3: {const char*_tmp928;return Cyc_PP_text(((_tmp928="extern ",
_tag_dyneither(_tmp928,sizeof(char),8))));}_LL2F4: if((int)_tmp45D != 4)goto _LL2F6;
_LL2F5: {const char*_tmp929;return Cyc_PP_text(((_tmp929="extern \"C\" ",
_tag_dyneither(_tmp929,sizeof(char),12))));}_LL2F6: if((int)_tmp45D != 1)goto
_LL2F8;_LL2F7: {const char*_tmp92A;return Cyc_PP_text(((_tmp92A="abstract ",
_tag_dyneither(_tmp92A,sizeof(char),10))));}_LL2F8: if((int)_tmp45D != 5)goto
_LL2ED;_LL2F9: {const char*_tmp92B;return Cyc_PP_text(((_tmp92B="register ",
_tag_dyneither(_tmp92B,sizeof(char),10))));}_LL2ED:;}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t);int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){void*_tmp463=t;struct
Cyc_Absyn_Tvar*_tmp464;struct Cyc_List_List*_tmp465;_LL2FB: if(_tmp463 <= (void*)4)
goto _LL2FF;if(*((int*)_tmp463)!= 1)goto _LL2FD;_tmp464=((struct Cyc_Absyn_VarType_struct*)
_tmp463)->f1;_LL2FC: return Cyc_Tcutil_is_temp_tvar(_tmp464);_LL2FD: if(*((int*)
_tmp463)!= 20)goto _LL2FF;_tmp465=((struct Cyc_Absyn_JoinEff_struct*)_tmp463)->f1;
_LL2FE: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp465);
_LL2FF:;_LL300: return 0;_LL2FA:;}int Cyc_Absynpp_is_anon_aggrtype(void*t);int Cyc_Absynpp_is_anon_aggrtype(
void*t){void*_tmp466=t;void**_tmp467;void*_tmp468;_LL302: if(_tmp466 <= (void*)4)
goto _LL308;if(*((int*)_tmp466)!= 11)goto _LL304;_LL303: return 1;_LL304: if(*((int*)
_tmp466)!= 13)goto _LL306;_LL305: return 1;_LL306: if(*((int*)_tmp466)!= 16)goto
_LL308;_tmp467=((struct Cyc_Absyn_TypedefType_struct*)_tmp466)->f4;if(_tmp467 == 0)
goto _LL308;_tmp468=*_tmp467;_LL307: return Cyc_Absynpp_is_anon_aggrtype(_tmp468);
_LL308:;_LL309: return 0;_LL301:;}static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms);static struct Cyc_List_List*
Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*
tms){if(tms != 0  && tms->tl != 0){struct _tuple5 _tmp92C;struct _tuple5 _tmp46A=(
_tmp92C.f1=(void*)tms->hd,((_tmp92C.f2=(void*)((struct Cyc_List_List*)_check_null(
tms->tl))->hd,_tmp92C)));void*_tmp46B;void*_tmp46C;_LL30B: _tmp46B=_tmp46A.f1;if(*((
int*)_tmp46B)!= 2)goto _LL30D;_tmp46C=_tmp46A.f2;if(*((int*)_tmp46C)!= 3)goto
_LL30D;_LL30C: {struct Cyc_List_List*_tmp92F;struct Cyc_List_List*_tmp92E;return(
_tmp92E=_region_malloc(r,sizeof(*_tmp92E)),((_tmp92E->hd=(void*)((void*)tms->hd),((
_tmp92E->tl=((_tmp92F=_region_malloc(r,sizeof(*_tmp92F)),((_tmp92F->hd=(void*)((
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd),((_tmp92F->tl=Cyc_Absynpp_bubble_attributes(
r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmp92F)))))),_tmp92E)))));}
_LL30D:;_LL30E: {struct Cyc_List_List*_tmp930;return(_tmp930=_region_malloc(r,
sizeof(*_tmp930)),((_tmp930->hd=(void*)atts,((_tmp930->tl=tms,_tmp930)))));}
_LL30A:;}else{struct Cyc_List_List*_tmp931;return(_tmp931=_region_malloc(r,
sizeof(*_tmp931)),((_tmp931->hd=(void*)atts,((_tmp931->tl=tms,_tmp931)))));}}
struct _tuple7 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,
void*t);struct _tuple7 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t){void*_tmp471=t;struct Cyc_Absyn_ArrayInfo _tmp472;void*_tmp473;struct
Cyc_Absyn_Tqual _tmp474;struct Cyc_Absyn_Exp*_tmp475;struct Cyc_Absyn_Conref*
_tmp476;struct Cyc_Position_Segment*_tmp477;struct Cyc_Absyn_PtrInfo _tmp478;void*
_tmp479;struct Cyc_Absyn_Tqual _tmp47A;struct Cyc_Absyn_PtrAtts _tmp47B;struct Cyc_Absyn_FnInfo
_tmp47C;struct Cyc_List_List*_tmp47D;struct Cyc_Core_Opt*_tmp47E;void*_tmp47F;
struct Cyc_List_List*_tmp480;int _tmp481;struct Cyc_Absyn_VarargInfo*_tmp482;struct
Cyc_List_List*_tmp483;struct Cyc_List_List*_tmp484;struct Cyc_Core_Opt*_tmp485;
struct Cyc_Core_Opt*_tmp486;int _tmp487;struct _tuple0*_tmp488;struct Cyc_List_List*
_tmp489;void**_tmp48A;_LL310: if(_tmp471 <= (void*)4)goto _LL31A;if(*((int*)_tmp471)
!= 7)goto _LL312;_tmp472=((struct Cyc_Absyn_ArrayType_struct*)_tmp471)->f1;_tmp473=(
void*)_tmp472.elt_type;_tmp474=_tmp472.tq;_tmp475=_tmp472.num_elts;_tmp476=
_tmp472.zero_term;_tmp477=_tmp472.zt_loc;_LL311: {struct Cyc_Absyn_Tqual _tmp48C;
void*_tmp48D;struct Cyc_List_List*_tmp48E;struct _tuple7 _tmp48B=Cyc_Absynpp_to_tms(
r,_tmp474,_tmp473);_tmp48C=_tmp48B.f1;_tmp48D=_tmp48B.f2;_tmp48E=_tmp48B.f3;{
void*tm;if(_tmp475 == 0){struct Cyc_Absyn_Carray_mod_struct _tmp934;struct Cyc_Absyn_Carray_mod_struct*
_tmp933;tm=(void*)((_tmp933=_region_malloc(r,sizeof(*_tmp933)),((_tmp933[0]=((
_tmp934.tag=0,((_tmp934.f1=_tmp476,((_tmp934.f2=_tmp477,_tmp934)))))),_tmp933))));}
else{struct Cyc_Absyn_ConstArray_mod_struct _tmp937;struct Cyc_Absyn_ConstArray_mod_struct*
_tmp936;tm=(void*)((_tmp936=_region_malloc(r,sizeof(*_tmp936)),((_tmp936[0]=((
_tmp937.tag=1,((_tmp937.f1=(struct Cyc_Absyn_Exp*)_tmp475,((_tmp937.f2=_tmp476,((
_tmp937.f3=_tmp477,_tmp937)))))))),_tmp936))));}{struct Cyc_List_List*_tmp93A;
struct _tuple7 _tmp939;return(_tmp939.f1=_tmp48C,((_tmp939.f2=_tmp48D,((_tmp939.f3=((
_tmp93A=_region_malloc(r,sizeof(*_tmp93A)),((_tmp93A->hd=(void*)tm,((_tmp93A->tl=
_tmp48E,_tmp93A)))))),_tmp939)))));}}}_LL312: if(*((int*)_tmp471)!= 4)goto _LL314;
_tmp478=((struct Cyc_Absyn_PointerType_struct*)_tmp471)->f1;_tmp479=(void*)
_tmp478.elt_typ;_tmp47A=_tmp478.elt_tq;_tmp47B=_tmp478.ptr_atts;_LL313: {struct
Cyc_Absyn_Tqual _tmp496;void*_tmp497;struct Cyc_List_List*_tmp498;struct _tuple7
_tmp495=Cyc_Absynpp_to_tms(r,_tmp47A,_tmp479);_tmp496=_tmp495.f1;_tmp497=_tmp495.f2;
_tmp498=_tmp495.f3;{struct Cyc_Absyn_Pointer_mod_struct*_tmp940;struct Cyc_Absyn_Pointer_mod_struct
_tmp93F;struct Cyc_List_List*_tmp93E;_tmp498=((_tmp93E=_region_malloc(r,sizeof(*
_tmp93E)),((_tmp93E->hd=(void*)((void*)((_tmp940=_region_malloc(r,sizeof(*
_tmp940)),((_tmp940[0]=((_tmp93F.tag=2,((_tmp93F.f1=_tmp47B,((_tmp93F.f2=tq,
_tmp93F)))))),_tmp940))))),((_tmp93E->tl=_tmp498,_tmp93E))))));}{struct _tuple7
_tmp941;return(_tmp941.f1=_tmp496,((_tmp941.f2=_tmp497,((_tmp941.f3=_tmp498,
_tmp941)))));}}_LL314: if(*((int*)_tmp471)!= 8)goto _LL316;_tmp47C=((struct Cyc_Absyn_FnType_struct*)
_tmp471)->f1;_tmp47D=_tmp47C.tvars;_tmp47E=_tmp47C.effect;_tmp47F=(void*)_tmp47C.ret_typ;
_tmp480=_tmp47C.args;_tmp481=_tmp47C.c_varargs;_tmp482=_tmp47C.cyc_varargs;
_tmp483=_tmp47C.rgn_po;_tmp484=_tmp47C.attributes;_LL315: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp47E == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp47E->v)){
_tmp47E=0;_tmp47D=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp47D);}{struct Cyc_Absyn_Tqual _tmp49E;void*_tmp49F;struct Cyc_List_List*_tmp4A0;
struct _tuple7 _tmp49D=Cyc_Absynpp_to_tms(r,Cyc_Absyn_empty_tqual(0),_tmp47F);
_tmp49E=_tmp49D.f1;_tmp49F=_tmp49D.f2;_tmp4A0=_tmp49D.f3;{struct Cyc_List_List*
tms=_tmp4A0;{void*_tmp4A1=Cyc_Cyclone_c_compiler;_LL31D: if((int)_tmp4A1 != 0)goto
_LL31F;_LL31E: if(_tmp484 != 0){struct Cyc_Absyn_Attributes_mod_struct _tmp944;
struct Cyc_Absyn_Attributes_mod_struct*_tmp943;tms=Cyc_Absynpp_bubble_attributes(
r,(void*)((_tmp943=_region_malloc(r,sizeof(*_tmp943)),((_tmp943[0]=((_tmp944.tag=
5,((_tmp944.f1=0,((_tmp944.f2=_tmp484,_tmp944)))))),_tmp943)))),tms);}{struct Cyc_Absyn_Function_mod_struct*
_tmp953;struct Cyc_Absyn_WithTypes_struct*_tmp952;struct Cyc_Absyn_WithTypes_struct
_tmp951;struct Cyc_Absyn_Function_mod_struct _tmp950;struct Cyc_List_List*_tmp94F;
tms=((_tmp94F=_region_malloc(r,sizeof(*_tmp94F)),((_tmp94F->hd=(void*)((void*)((
_tmp953=_region_malloc(r,sizeof(*_tmp953)),((_tmp953[0]=((_tmp950.tag=3,((
_tmp950.f1=(void*)((void*)((_tmp952=_region_malloc(r,sizeof(*_tmp952)),((_tmp952[
0]=((_tmp951.tag=1,((_tmp951.f1=_tmp480,((_tmp951.f2=_tmp481,((_tmp951.f3=
_tmp482,((_tmp951.f4=_tmp47E,((_tmp951.f5=_tmp483,_tmp951)))))))))))),_tmp952))))),
_tmp950)))),_tmp953))))),((_tmp94F->tl=tms,_tmp94F))))));}goto _LL31C;_LL31F: if((
int)_tmp4A1 != 1)goto _LL31C;_LL320:{struct Cyc_Absyn_Function_mod_struct*_tmp962;
struct Cyc_Absyn_WithTypes_struct*_tmp961;struct Cyc_Absyn_WithTypes_struct _tmp960;
struct Cyc_Absyn_Function_mod_struct _tmp95F;struct Cyc_List_List*_tmp95E;tms=((
_tmp95E=_region_malloc(r,sizeof(*_tmp95E)),((_tmp95E->hd=(void*)((void*)((
_tmp962=_region_malloc(r,sizeof(*_tmp962)),((_tmp962[0]=((_tmp95F.tag=3,((
_tmp95F.f1=(void*)((void*)((_tmp961=_region_malloc(r,sizeof(*_tmp961)),((_tmp961[
0]=((_tmp960.tag=1,((_tmp960.f1=_tmp480,((_tmp960.f2=_tmp481,((_tmp960.f3=
_tmp482,((_tmp960.f4=_tmp47E,((_tmp960.f5=_tmp483,_tmp960)))))))))))),_tmp961))))),
_tmp95F)))),_tmp962))))),((_tmp95E->tl=tms,_tmp95E))))));}for(0;_tmp484 != 0;
_tmp484=_tmp484->tl){void*_tmp4AE=(void*)_tmp484->hd;_LL322: if((int)_tmp4AE != 0)
goto _LL324;_LL323: goto _LL325;_LL324: if((int)_tmp4AE != 1)goto _LL326;_LL325: goto
_LL327;_LL326: if((int)_tmp4AE != 2)goto _LL328;_LL327:{struct Cyc_Absyn_Attributes_mod_struct*
_tmp96C;struct Cyc_List_List*_tmp96B;struct Cyc_Absyn_Attributes_mod_struct _tmp96A;
struct Cyc_List_List*_tmp969;tms=((_tmp969=_region_malloc(r,sizeof(*_tmp969)),((
_tmp969->hd=(void*)((void*)((_tmp96C=_region_malloc(r,sizeof(*_tmp96C)),((
_tmp96C[0]=((_tmp96A.tag=5,((_tmp96A.f1=0,((_tmp96A.f2=((_tmp96B=_cycalloc(
sizeof(*_tmp96B)),((_tmp96B->hd=(void*)((void*)_tmp484->hd),((_tmp96B->tl=0,
_tmp96B)))))),_tmp96A)))))),_tmp96C))))),((_tmp969->tl=tms,_tmp969))))));}goto
AfterAtts;_LL328:;_LL329: goto _LL321;_LL321:;}goto _LL31C;_LL31C:;}AfterAtts: if(
_tmp47D != 0){struct Cyc_Absyn_TypeParams_mod_struct*_tmp972;struct Cyc_Absyn_TypeParams_mod_struct
_tmp971;struct Cyc_List_List*_tmp970;tms=((_tmp970=_region_malloc(r,sizeof(*
_tmp970)),((_tmp970->hd=(void*)((void*)((_tmp972=_region_malloc(r,sizeof(*
_tmp972)),((_tmp972[0]=((_tmp971.tag=4,((_tmp971.f1=_tmp47D,((_tmp971.f2=0,((
_tmp971.f3=1,_tmp971)))))))),_tmp972))))),((_tmp970->tl=tms,_tmp970))))));}{
struct _tuple7 _tmp973;return(_tmp973.f1=_tmp49E,((_tmp973.f2=_tmp49F,((_tmp973.f3=
tms,_tmp973)))));}}}_LL316: if(*((int*)_tmp471)!= 0)goto _LL318;_tmp485=((struct
Cyc_Absyn_Evar_struct*)_tmp471)->f1;_tmp486=((struct Cyc_Absyn_Evar_struct*)
_tmp471)->f2;_tmp487=((struct Cyc_Absyn_Evar_struct*)_tmp471)->f3;_LL317: if(
_tmp486 == 0){struct _tuple7 _tmp974;return(_tmp974.f1=tq,((_tmp974.f2=t,((_tmp974.f3=
0,_tmp974)))));}else{return Cyc_Absynpp_to_tms(r,tq,(void*)_tmp486->v);}_LL318:
if(*((int*)_tmp471)!= 16)goto _LL31A;_tmp488=((struct Cyc_Absyn_TypedefType_struct*)
_tmp471)->f1;_tmp489=((struct Cyc_Absyn_TypedefType_struct*)_tmp471)->f2;_tmp48A=((
struct Cyc_Absyn_TypedefType_struct*)_tmp471)->f4;_LL319: if((_tmp48A == 0  || !Cyc_Absynpp_expand_typedefs)
 || Cyc_Absynpp_is_anon_aggrtype(*_tmp48A)){struct _tuple7 _tmp975;return(_tmp975.f1=
tq,((_tmp975.f2=t,((_tmp975.f3=0,_tmp975)))));}else{return Cyc_Absynpp_to_tms(r,
tq,*_tmp48A);}_LL31A:;_LL31B: {struct _tuple7 _tmp976;return(_tmp976.f1=tq,((
_tmp976.f2=t,((_tmp976.f3=0,_tmp976)))));}_LL30F:;}static int Cyc_Absynpp_is_char_ptr(
void*t);static int Cyc_Absynpp_is_char_ptr(void*t){void*_tmp4BA=t;struct Cyc_Core_Opt*
_tmp4BB;struct Cyc_Core_Opt _tmp4BC;void*_tmp4BD;struct Cyc_Absyn_PtrInfo _tmp4BE;
void*_tmp4BF;_LL32B: if(_tmp4BA <= (void*)4)goto _LL32F;if(*((int*)_tmp4BA)!= 0)
goto _LL32D;_tmp4BB=((struct Cyc_Absyn_Evar_struct*)_tmp4BA)->f2;if(_tmp4BB == 0)
goto _LL32D;_tmp4BC=*_tmp4BB;_tmp4BD=(void*)_tmp4BC.v;_LL32C: return Cyc_Absynpp_is_char_ptr(
_tmp4BD);_LL32D: if(*((int*)_tmp4BA)!= 4)goto _LL32F;_tmp4BE=((struct Cyc_Absyn_PointerType_struct*)
_tmp4BA)->f1;_tmp4BF=(void*)_tmp4BE.elt_typ;_LL32E: L: {void*_tmp4C0=_tmp4BF;
struct Cyc_Core_Opt*_tmp4C1;struct Cyc_Core_Opt _tmp4C2;void*_tmp4C3;void**_tmp4C4;
void*_tmp4C5;void*_tmp4C6;_LL332: if(_tmp4C0 <= (void*)4)goto _LL338;if(*((int*)
_tmp4C0)!= 0)goto _LL334;_tmp4C1=((struct Cyc_Absyn_Evar_struct*)_tmp4C0)->f2;if(
_tmp4C1 == 0)goto _LL334;_tmp4C2=*_tmp4C1;_tmp4C3=(void*)_tmp4C2.v;_LL333: _tmp4BF=
_tmp4C3;goto L;_LL334: if(*((int*)_tmp4C0)!= 16)goto _LL336;_tmp4C4=((struct Cyc_Absyn_TypedefType_struct*)
_tmp4C0)->f4;if(_tmp4C4 == 0)goto _LL336;_tmp4C5=*_tmp4C4;_LL335: _tmp4BF=_tmp4C5;
goto L;_LL336: if(*((int*)_tmp4C0)!= 5)goto _LL338;_tmp4C6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4C0)->f2;if((int)_tmp4C6 != 0)goto _LL338;_LL337: return 1;_LL338:;_LL339: return
0;_LL331:;}_LL32F:;_LL330: return 0;_LL32A:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){struct _RegionHandle
_tmp4C7=_new_region("temp");struct _RegionHandle*temp=& _tmp4C7;_push_region(temp);{
struct Cyc_Absyn_Tqual _tmp4C9;void*_tmp4CA;struct Cyc_List_List*_tmp4CB;struct
_tuple7 _tmp4C8=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp4C9=_tmp4C8.f1;_tmp4CA=
_tmp4C8.f2;_tmp4CB=_tmp4C8.f3;_tmp4CB=Cyc_List_imp_rev(_tmp4CB);if(_tmp4CB == 0
 && dopt == 0){struct Cyc_PP_Doc*_tmp977[2];struct Cyc_PP_Doc*_tmp4CD=(_tmp977[1]=
Cyc_Absynpp_ntyp2doc(_tmp4CA),((_tmp977[0]=Cyc_Absynpp_tqual2doc(_tmp4C9),Cyc_PP_cat(
_tag_dyneither(_tmp977,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return
_tmp4CD;}else{const char*_tmp97A;struct Cyc_PP_Doc*_tmp979[4];struct Cyc_PP_Doc*
_tmp4D0=(_tmp979[3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?
Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp4CB),((_tmp979[2]=Cyc_PP_text(((
_tmp97A=" ",_tag_dyneither(_tmp97A,sizeof(char),2)))),((_tmp979[1]=Cyc_Absynpp_ntyp2doc(
_tmp4CA),((_tmp979[0]=Cyc_Absynpp_tqual2doc(_tmp4C9),Cyc_PP_cat(_tag_dyneither(
_tmp979,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp4D0;}};
_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*
f);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){void*
_tmp4D1=Cyc_Cyclone_c_compiler;_LL33B: if((int)_tmp4D1 != 0)goto _LL33D;_LL33C: if(f->width
!= 0){const char*_tmp981;const char*_tmp980;struct Cyc_Core_Opt*_tmp97F;struct Cyc_PP_Doc*
_tmp97E[5];return(_tmp97E[4]=Cyc_PP_text(((_tmp981=";",_tag_dyneither(_tmp981,
sizeof(char),2)))),((_tmp97E[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmp97E[2]=
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmp97E[1]=
Cyc_PP_text(((_tmp980=":",_tag_dyneither(_tmp980,sizeof(char),2)))),((_tmp97E[0]=
Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,((_tmp97F=_cycalloc(sizeof(*_tmp97F)),((
_tmp97F->v=Cyc_PP_textptr(f->name),_tmp97F))))),Cyc_PP_cat(_tag_dyneither(
_tmp97E,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{const char*_tmp986;struct Cyc_Core_Opt*
_tmp985;struct Cyc_PP_Doc*_tmp984[3];return(_tmp984[2]=Cyc_PP_text(((_tmp986=";",
_tag_dyneither(_tmp986,sizeof(char),2)))),((_tmp984[1]=Cyc_Absynpp_atts2doc(f->attributes),((
_tmp984[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,((_tmp985=_cycalloc(sizeof(*
_tmp985)),((_tmp985->v=Cyc_PP_textptr(f->name),_tmp985))))),Cyc_PP_cat(
_tag_dyneither(_tmp984,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL33D: if((int)
_tmp4D1 != 1)goto _LL33A;_LL33E: if(f->width != 0){const char*_tmp98D;const char*
_tmp98C;struct Cyc_Core_Opt*_tmp98B;struct Cyc_PP_Doc*_tmp98A[5];return(_tmp98A[4]=
Cyc_PP_text(((_tmp98D=";",_tag_dyneither(_tmp98D,sizeof(char),2)))),((_tmp98A[3]=
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmp98A[2]=
Cyc_PP_text(((_tmp98C=":",_tag_dyneither(_tmp98C,sizeof(char),2)))),((_tmp98A[1]=
Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,((_tmp98B=_cycalloc(sizeof(*_tmp98B)),((
_tmp98B->v=Cyc_PP_textptr(f->name),_tmp98B))))),((_tmp98A[0]=Cyc_Absynpp_atts2doc(
f->attributes),Cyc_PP_cat(_tag_dyneither(_tmp98A,sizeof(struct Cyc_PP_Doc*),5)))))))))));}
else{const char*_tmp992;struct Cyc_Core_Opt*_tmp991;struct Cyc_PP_Doc*_tmp990[3];
return(_tmp990[2]=Cyc_PP_text(((_tmp992=";",_tag_dyneither(_tmp992,sizeof(char),
2)))),((_tmp990[1]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,((_tmp991=_cycalloc(
sizeof(*_tmp991)),((_tmp991->v=Cyc_PP_textptr(f->name),_tmp991))))),((_tmp990[0]=
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmp990,sizeof(
struct Cyc_PP_Doc*),3)))))));}_LL33A:;}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct
Cyc_List_List*fields){const char*_tmp993;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmp993="",_tag_dyneither(_tmp993,
sizeof(char),1))),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(struct
Cyc_Absyn_Tunionfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(struct Cyc_Absyn_Tunionfield*
f){struct Cyc_PP_Doc*_tmp994[3];return(_tmp994[2]=f->typs == 0?Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc(f->typs),((_tmp994[1]=Cyc_Absynpp_typedef_name2doc(f->name),((
_tmp994[0]=Cyc_Absynpp_scope2doc((void*)f->sc),Cyc_PP_cat(_tag_dyneither(_tmp994,
sizeof(struct Cyc_PP_Doc*),3)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(struct
Cyc_List_List*fields){const char*_tmp995;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Tunionfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_tunionfield2doc,((_tmp995=",",_tag_dyneither(_tmp995,
sizeof(char),2))),fields);}void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,
struct Cyc___cycFILE*f);void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,
struct Cyc___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((
struct Cyc_Absyn_Decl*)tdl->hd),72,f);{const char*_tmp998;void*_tmp997;(_tmp997=0,
Cyc_fprintf(f,((_tmp998="\n",_tag_dyneither(_tmp998,sizeof(char),2))),
_tag_dyneither(_tmp997,sizeof(void*),0)));}}}struct _dyneither_ptr Cyc_Absynpp_decllist2string(
struct Cyc_List_List*tdl);struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct
Cyc_List_List*tdl){const char*_tmp999;return Cyc_PP_string_of_doc(Cyc_PP_seql(((
_tmp999="",_tag_dyneither(_tmp999,sizeof(char),1))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_decl2doc,tdl)),72);}struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*e);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*
e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}struct _dyneither_ptr
Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s);struct _dyneither_ptr Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t);struct _dyneither_ptr Cyc_Absynpp_typ2string(
void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}struct
_dyneither_ptr Cyc_Absynpp_typ2cstring(void*t);struct _dyneither_ptr Cyc_Absynpp_typ2cstring(
void*t){int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;Cyc_Absynpp_add_cyc_prefix=0;{struct _dyneither_ptr s=
Cyc_Absynpp_typ2string(t);Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
old_add_cyc_prefix;return s;}}struct _dyneither_ptr Cyc_Absynpp_prim2string(void*p);
struct _dyneither_ptr Cyc_Absynpp_prim2string(void*p){return Cyc_PP_string_of_doc(
Cyc_Absynpp_prim2doc(p),72);}struct _dyneither_ptr Cyc_Absynpp_pat2string(struct
Cyc_Absyn_Pat*p);struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*
p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}struct _dyneither_ptr
Cyc_Absynpp_scope2string(void*sc);struct _dyneither_ptr Cyc_Absynpp_scope2string(
void*sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
