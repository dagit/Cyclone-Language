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
Cyc_Position_Nocontext[14];struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct
_dyneither_ptr*f2;};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int
q_restrict;int real_const;struct Cyc_Position_Segment*loc;};struct
_union_Constraint_Eq_constr{int tag;void*val;};struct
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
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;struct Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct
_tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple2{void*f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct
_union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;}
;struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
f1;};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
struct Cyc_Absyn_IntType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple0*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_ValueofType_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};
struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct
Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Regparm_att_struct{
int tag;int f1;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
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
_union_Cnst_String_c String_c;};struct Cyc_Absyn_VarargCallInfo{int num_varargs;
struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;
unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;
void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_bounds_one;struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple9{void*f1;
struct _tuple0*f2;};struct _tuple9 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
struct Cyc_Buffer_t;unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strptrcmp(
struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_str_sepstr(
struct Cyc_List_List*,struct _dyneither_ptr);struct _tuple10{unsigned int f1;int f2;}
;struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple11{void*f1;void*f2;};struct
_tuple11*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct
_tuple11*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;
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
struct Cyc_Absyn_Stmt*s);struct _tuple12{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct _tuple7*Cyc_Absynpp_arg_mk_opt(struct _tuple12*arg);struct
_tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};struct _tuple13
Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);extern
void*Cyc_Cyclone_c_compiler;struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple14*dp);struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List*cs);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
void*);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*);struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int
inprec,struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct
_tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst);struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
void*);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*
es);struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_PP_Doc*
Cyc_Absynpp_de2doc(struct _tuple15*de);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
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
v){struct Cyc_List_List*_tmp503;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((
_tmp503=_cycalloc(sizeof(*_tmp503)),((_tmp503->hd=v,((_tmp503->tl=0,_tmp503)))))));}
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l);static void Cyc_Absynpp_suppr_last(
struct Cyc_List_List**l){if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;
else{Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
static void Cyc_Absynpp_curr_namespace_drop();static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c);struct _dyneither_ptr Cyc_Absynpp_char_escape(
char c){switch(c){case '\a': _LL0: {const char*_tmp504;return(_tmp504="\\a",
_tag_dyneither(_tmp504,sizeof(char),3));}case '\b': _LL1: {const char*_tmp505;
return(_tmp505="\\b",_tag_dyneither(_tmp505,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp506;return(_tmp506="\\f",_tag_dyneither(_tmp506,sizeof(char),3));}
case '\n': _LL3: {const char*_tmp507;return(_tmp507="\\n",_tag_dyneither(_tmp507,
sizeof(char),3));}case '\r': _LL4: {const char*_tmp508;return(_tmp508="\\r",
_tag_dyneither(_tmp508,sizeof(char),3));}case '\t': _LL5: {const char*_tmp509;
return(_tmp509="\\t",_tag_dyneither(_tmp509,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp50A;return(_tmp50A="\\v",_tag_dyneither(_tmp50A,sizeof(char),3));}
case '\\': _LL7: {const char*_tmp50B;return(_tmp50B="\\\\",_tag_dyneither(_tmp50B,
sizeof(char),3));}case '"': _LL8: {const char*_tmp50C;return(_tmp50C="\"",
_tag_dyneither(_tmp50C,sizeof(char),2));}case '\'': _LL9: {const char*_tmp50D;
return(_tmp50D="\\'",_tag_dyneither(_tmp50D,sizeof(char),3));}default: _LLA: if(c
>= ' '  && c <= '~'){struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);{char _tmp510;
char _tmp50F;struct _dyneither_ptr _tmp50E;(_tmp50E=_dyneither_ptr_plus(_tmpC,
sizeof(char),0),((_tmp50F=*((char*)_check_dyneither_subscript(_tmp50E,sizeof(
char),0)),((_tmp510=c,((_get_dyneither_size(_tmp50E,sizeof(char))== 1  && (
_tmp50F == '\000'  && _tmp510 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp50E.curr)=
_tmp510)))))));}return(struct _dyneither_ptr)_tmpC;}else{struct _dyneither_ptr
_tmp10=Cyc_Core_new_string(5);int j=0;{char _tmp513;char _tmp512;struct
_dyneither_ptr _tmp511;(_tmp511=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp512=*((char*)_check_dyneither_subscript(_tmp511,sizeof(char),0)),((_tmp513='\\',((
_get_dyneither_size(_tmp511,sizeof(char))== 1  && (_tmp512 == '\000'  && _tmp513 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp511.curr)=_tmp513)))))));}{char _tmp516;char
_tmp515;struct _dyneither_ptr _tmp514;(_tmp514=_dyneither_ptr_plus(_tmp10,sizeof(
char),j ++),((_tmp515=*((char*)_check_dyneither_subscript(_tmp514,sizeof(char),0)),((
_tmp516=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp514,
sizeof(char))== 1  && (_tmp515 == '\000'  && _tmp516 != '\000')?_throw_arraybounds():
1,*((char*)_tmp514.curr)=_tmp516)))))));}{char _tmp519;char _tmp518;struct
_dyneither_ptr _tmp517;(_tmp517=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp518=*((char*)_check_dyneither_subscript(_tmp517,sizeof(char),0)),((_tmp519=(
char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp517,sizeof(char))== 1  && (
_tmp518 == '\000'  && _tmp519 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp517.curr)=
_tmp519)))))));}{char _tmp51C;char _tmp51B;struct _dyneither_ptr _tmp51A;(_tmp51A=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp51B=*((char*)
_check_dyneither_subscript(_tmp51A,sizeof(char),0)),((_tmp51C=(char)('0' + (c & 7)),((
_get_dyneither_size(_tmp51A,sizeof(char))== 1  && (_tmp51B == '\000'  && _tmp51C != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp51A.curr)=_tmp51C)))))));}return(struct
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
_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:{char _tmp51F;char _tmp51E;struct
_dyneither_ptr _tmp51D;(_tmp51D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp51E=*((
char*)_check_dyneither_subscript(_tmp51D,sizeof(char),0)),((_tmp51F='\\',((
_get_dyneither_size(_tmp51D,sizeof(char))== 1  && (_tmp51E == '\000'  && _tmp51F != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp51D.curr)=_tmp51F)))))));}{char _tmp522;char
_tmp521;struct _dyneither_ptr _tmp520;(_tmp520=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp521=*((char*)_check_dyneither_subscript(_tmp520,sizeof(char),0)),((
_tmp522='a',((_get_dyneither_size(_tmp520,sizeof(char))== 1  && (_tmp521 == '\000'
 && _tmp522 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp520.curr)=_tmp522)))))));}
goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:{char _tmp525;char _tmp524;struct
_dyneither_ptr _tmp523;(_tmp523=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp524=*((
char*)_check_dyneither_subscript(_tmp523,sizeof(char),0)),((_tmp525='\\',((
_get_dyneither_size(_tmp523,sizeof(char))== 1  && (_tmp524 == '\000'  && _tmp525 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp523.curr)=_tmp525)))))));}{char _tmp528;char
_tmp527;struct _dyneither_ptr _tmp526;(_tmp526=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp527=*((char*)_check_dyneither_subscript(_tmp526,sizeof(char),0)),((
_tmp528='b',((_get_dyneither_size(_tmp526,sizeof(char))== 1  && (_tmp527 == '\000'
 && _tmp528 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp526.curr)=_tmp528)))))));}
goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:{char _tmp52B;char _tmp52A;struct
_dyneither_ptr _tmp529;(_tmp529=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp52A=*((
char*)_check_dyneither_subscript(_tmp529,sizeof(char),0)),((_tmp52B='\\',((
_get_dyneither_size(_tmp529,sizeof(char))== 1  && (_tmp52A == '\000'  && _tmp52B != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp529.curr)=_tmp52B)))))));}{char _tmp52E;char
_tmp52D;struct _dyneither_ptr _tmp52C;(_tmp52C=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp52D=*((char*)_check_dyneither_subscript(_tmp52C,sizeof(char),0)),((
_tmp52E='f',((_get_dyneither_size(_tmp52C,sizeof(char))== 1  && (_tmp52D == '\000'
 && _tmp52E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp52C.curr)=_tmp52E)))))));}
goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:{char _tmp531;char _tmp530;struct
_dyneither_ptr _tmp52F;(_tmp52F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp530=*((
char*)_check_dyneither_subscript(_tmp52F,sizeof(char),0)),((_tmp531='\\',((
_get_dyneither_size(_tmp52F,sizeof(char))== 1  && (_tmp530 == '\000'  && _tmp531 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp52F.curr)=_tmp531)))))));}{char _tmp534;char
_tmp533;struct _dyneither_ptr _tmp532;(_tmp532=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp533=*((char*)_check_dyneither_subscript(_tmp532,sizeof(char),0)),((
_tmp534='n',((_get_dyneither_size(_tmp532,sizeof(char))== 1  && (_tmp533 == '\000'
 && _tmp534 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp532.curr)=_tmp534)))))));}
goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:{char _tmp537;char _tmp536;struct
_dyneither_ptr _tmp535;(_tmp535=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp536=*((
char*)_check_dyneither_subscript(_tmp535,sizeof(char),0)),((_tmp537='\\',((
_get_dyneither_size(_tmp535,sizeof(char))== 1  && (_tmp536 == '\000'  && _tmp537 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp535.curr)=_tmp537)))))));}{char _tmp53A;char
_tmp539;struct _dyneither_ptr _tmp538;(_tmp538=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp539=*((char*)_check_dyneither_subscript(_tmp538,sizeof(char),0)),((
_tmp53A='r',((_get_dyneither_size(_tmp538,sizeof(char))== 1  && (_tmp539 == '\000'
 && _tmp53A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp538.curr)=_tmp53A)))))));}
goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:{char _tmp53D;char _tmp53C;struct
_dyneither_ptr _tmp53B;(_tmp53B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp53C=*((
char*)_check_dyneither_subscript(_tmp53B,sizeof(char),0)),((_tmp53D='\\',((
_get_dyneither_size(_tmp53B,sizeof(char))== 1  && (_tmp53C == '\000'  && _tmp53D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp53B.curr)=_tmp53D)))))));}{char _tmp540;char
_tmp53F;struct _dyneither_ptr _tmp53E;(_tmp53E=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp53F=*((char*)_check_dyneither_subscript(_tmp53E,sizeof(char),0)),((
_tmp540='t',((_get_dyneither_size(_tmp53E,sizeof(char))== 1  && (_tmp53F == '\000'
 && _tmp540 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp53E.curr)=_tmp540)))))));}
goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:{char _tmp543;char _tmp542;struct
_dyneither_ptr _tmp541;(_tmp541=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp542=*((
char*)_check_dyneither_subscript(_tmp541,sizeof(char),0)),((_tmp543='\\',((
_get_dyneither_size(_tmp541,sizeof(char))== 1  && (_tmp542 == '\000'  && _tmp543 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp541.curr)=_tmp543)))))));}{char _tmp546;char
_tmp545;struct _dyneither_ptr _tmp544;(_tmp544=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp545=*((char*)_check_dyneither_subscript(_tmp544,sizeof(char),0)),((
_tmp546='v',((_get_dyneither_size(_tmp544,sizeof(char))== 1  && (_tmp545 == '\000'
 && _tmp546 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp544.curr)=_tmp546)))))));}
goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:{char _tmp549;char _tmp548;struct
_dyneither_ptr _tmp547;(_tmp547=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp548=*((
char*)_check_dyneither_subscript(_tmp547,sizeof(char),0)),((_tmp549='\\',((
_get_dyneither_size(_tmp547,sizeof(char))== 1  && (_tmp548 == '\000'  && _tmp549 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp547.curr)=_tmp549)))))));}{char _tmp54C;char
_tmp54B;struct _dyneither_ptr _tmp54A;(_tmp54A=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp54B=*((char*)_check_dyneither_subscript(_tmp54A,sizeof(char),0)),((
_tmp54C='\\',((_get_dyneither_size(_tmp54A,sizeof(char))== 1  && (_tmp54B == '\000'
 && _tmp54C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp54A.curr)=_tmp54C)))))));}
goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:{char _tmp54F;char _tmp54E;struct
_dyneither_ptr _tmp54D;(_tmp54D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp54E=*((
char*)_check_dyneither_subscript(_tmp54D,sizeof(char),0)),((_tmp54F='\\',((
_get_dyneither_size(_tmp54D,sizeof(char))== 1  && (_tmp54E == '\000'  && _tmp54F != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp54D.curr)=_tmp54F)))))));}{char _tmp552;char
_tmp551;struct _dyneither_ptr _tmp550;(_tmp550=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp551=*((char*)_check_dyneither_subscript(_tmp550,sizeof(char),0)),((
_tmp552='"',((_get_dyneither_size(_tmp550,sizeof(char))== 1  && (_tmp551 == '\000'
 && _tmp552 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp550.curr)=_tmp552)))))));}
goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp555;char _tmp554;
struct _dyneither_ptr _tmp553;(_tmp553=_dyneither_ptr_plus(t,sizeof(char),j ++),((
_tmp554=*((char*)_check_dyneither_subscript(_tmp553,sizeof(char),0)),((_tmp555=
_tmp1E,((_get_dyneither_size(_tmp553,sizeof(char))== 1  && (_tmp554 == '\000'  && 
_tmp555 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp553.curr)=_tmp555)))))));}
else{{char _tmp558;char _tmp557;struct _dyneither_ptr _tmp556;(_tmp556=
_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp557=*((char*)
_check_dyneither_subscript(_tmp556,sizeof(char),0)),((_tmp558='\\',((
_get_dyneither_size(_tmp556,sizeof(char))== 1  && (_tmp557 == '\000'  && _tmp558 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp556.curr)=_tmp558)))))));}{char _tmp55B;char
_tmp55A;struct _dyneither_ptr _tmp559;(_tmp559=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp55A=*((char*)_check_dyneither_subscript(_tmp559,sizeof(char),0)),((
_tmp55B=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dyneither_size(_tmp559,sizeof(char))
== 1  && (_tmp55A == '\000'  && _tmp55B != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp559.curr)=_tmp55B)))))));}{char _tmp55E;char _tmp55D;struct _dyneither_ptr
_tmp55C;(_tmp55C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp55D=*((char*)
_check_dyneither_subscript(_tmp55C,sizeof(char),0)),((_tmp55E=(char)('0' + (
_tmp1E >> 3 & 7)),((_get_dyneither_size(_tmp55C,sizeof(char))== 1  && (_tmp55D == '\000'
 && _tmp55E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp55C.curr)=_tmp55E)))))));}{
char _tmp561;char _tmp560;struct _dyneither_ptr _tmp55F;(_tmp55F=_dyneither_ptr_plus(
t,sizeof(char),j ++),((_tmp560=*((char*)_check_dyneither_subscript(_tmp55F,
sizeof(char),0)),((_tmp561=(char)('0' + (_tmp1E & 7)),((_get_dyneither_size(
_tmp55F,sizeof(char))== 1  && (_tmp560 == '\000'  && _tmp561 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp55F.curr)=_tmp561)))))));}}goto _LL21;_LL21:;}}
return(struct _dyneither_ptr)t;}}}}static char _tmp64[9]="restrict";static struct
_dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char
_tmp65[9]="volatile";static struct _dyneither_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dyneither_ptr
Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string;static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq);struct Cyc_PP_Doc*
Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=0;if(tq.q_restrict){
struct Cyc_List_List*_tmp562;l=((_tmp562=_cycalloc(sizeof(*_tmp562)),((_tmp562->hd=
Cyc_Absynpp_restrict_sp,((_tmp562->tl=l,_tmp562))))));}if(tq.q_volatile){struct
Cyc_List_List*_tmp563;l=((_tmp563=_cycalloc(sizeof(*_tmp563)),((_tmp563->hd=Cyc_Absynpp_volatile_sp,((
_tmp563->tl=l,_tmp563))))));}if(tq.print_const){struct Cyc_List_List*_tmp564;l=((
_tmp564=_cycalloc(sizeof(*_tmp564)),((_tmp564->hd=Cyc_Absynpp_const_sp,((_tmp564->tl=
l,_tmp564))))));}{const char*_tmp567;const char*_tmp566;const char*_tmp565;return
Cyc_PP_egroup(((_tmp565="",_tag_dyneither(_tmp565,sizeof(char),1))),((_tmp566=" ",
_tag_dyneither(_tmp566,sizeof(char),2))),((_tmp567=" ",_tag_dyneither(_tmp567,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));}}
struct _dyneither_ptr Cyc_Absynpp_kind2string(void*k);struct _dyneither_ptr Cyc_Absynpp_kind2string(
void*k){void*_tmp6D=k;_LL37: if((int)_tmp6D != 0)goto _LL39;_LL38: {const char*
_tmp568;return(_tmp568="A",_tag_dyneither(_tmp568,sizeof(char),2));}_LL39: if((
int)_tmp6D != 1)goto _LL3B;_LL3A: {const char*_tmp569;return(_tmp569="M",
_tag_dyneither(_tmp569,sizeof(char),2));}_LL3B: if((int)_tmp6D != 2)goto _LL3D;
_LL3C: {const char*_tmp56A;return(_tmp56A="B",_tag_dyneither(_tmp56A,sizeof(char),
2));}_LL3D: if((int)_tmp6D != 3)goto _LL3F;_LL3E: {const char*_tmp56B;return(_tmp56B="R",
_tag_dyneither(_tmp56B,sizeof(char),2));}_LL3F: if((int)_tmp6D != 4)goto _LL41;
_LL40: {const char*_tmp56C;return(_tmp56C="UR",_tag_dyneither(_tmp56C,sizeof(char),
3));}_LL41: if((int)_tmp6D != 5)goto _LL43;_LL42: {const char*_tmp56D;return(_tmp56D="TR",
_tag_dyneither(_tmp56D,sizeof(char),3));}_LL43: if((int)_tmp6D != 6)goto _LL45;
_LL44: {const char*_tmp56E;return(_tmp56E="E",_tag_dyneither(_tmp56E,sizeof(char),
2));}_LL45: if((int)_tmp6D != 7)goto _LL36;_LL46: {const char*_tmp56F;return(_tmp56F="I",
_tag_dyneither(_tmp56F,sizeof(char),2));}_LL36:;}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(
void*k);struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(void*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(
k));}struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c);struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp76=Cyc_Absyn_compress_kb(
c);void*_tmp77;void*_tmp78;_LL48: if(*((int*)_tmp76)!= 0)goto _LL4A;_tmp77=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp76)->f1;_LL49: return Cyc_Absynpp_kind2string(
_tmp77);_LL4A: if(*((int*)_tmp76)!= 1)goto _LL4C;_LL4B: if(Cyc_PP_tex_output){const
char*_tmp570;return(_tmp570="{?}",_tag_dyneither(_tmp570,sizeof(char),4));}else{
const char*_tmp571;return(_tmp571="?",_tag_dyneither(_tmp571,sizeof(char),2));}
_LL4C: if(*((int*)_tmp76)!= 2)goto _LL47;_tmp78=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp76)->f2;_LL4D: return Cyc_Absynpp_kind2string(_tmp78);_LL47:;}struct Cyc_PP_Doc*
Cyc_Absynpp_kindbound2doc(void*c);struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(
void*c){void*_tmp7B=Cyc_Absyn_compress_kb(c);void*_tmp7C;void*_tmp7D;_LL4F: if(*((
int*)_tmp7B)!= 0)goto _LL51;_tmp7C=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp7B)->f1;
_LL50: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7C));_LL51: if(*((int*)_tmp7B)
!= 1)goto _LL53;_LL52: if(Cyc_PP_tex_output){const char*_tmp572;return Cyc_PP_text_width(((
_tmp572="{?}",_tag_dyneither(_tmp572,sizeof(char),4))),1);}else{const char*
_tmp573;return Cyc_PP_text(((_tmp573="?",_tag_dyneither(_tmp573,sizeof(char),2))));}
_LL53: if(*((int*)_tmp7B)!= 2)goto _LL4E;_tmp7D=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp7B)->f2;_LL54: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7D));_LL4E:;}
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts);struct Cyc_PP_Doc*
Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){const char*_tmp576;const char*_tmp575;
const char*_tmp574;return Cyc_PP_egroup(((_tmp574="<",_tag_dyneither(_tmp574,
sizeof(char),2))),((_tmp575=">",_tag_dyneither(_tmp575,sizeof(char),2))),((
_tmp576=",",_tag_dyneither(_tmp576,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv);struct Cyc_PP_Doc*
Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){void*_tmp83=Cyc_Absyn_compress_kb(
tv->kind);void*_tmp84;void*_tmp85;void*_tmp86;_LL56: if(*((int*)_tmp83)!= 1)goto
_LL58;_LL57: goto _LL59;_LL58: if(*((int*)_tmp83)!= 0)goto _LL5A;_tmp84=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp83)->f1;if((int)_tmp84 != 2)goto _LL5A;_LL59:
return Cyc_PP_textptr(tv->name);_LL5A: if(*((int*)_tmp83)!= 2)goto _LL5C;_tmp85=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp83)->f2;_LL5B: _tmp86=_tmp85;goto
_LL5D;_LL5C: if(*((int*)_tmp83)!= 0)goto _LL55;_tmp86=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp83)->f1;_LL5D: {const char*_tmp579;struct Cyc_PP_Doc*_tmp578[3];return(_tmp578[
2]=Cyc_Absynpp_kind2doc(_tmp86),((_tmp578[1]=Cyc_PP_text(((_tmp579="::",
_tag_dyneither(_tmp579,sizeof(char),3)))),((_tmp578[0]=Cyc_PP_textptr(tv->name),
Cyc_PP_cat(_tag_dyneither(_tmp578,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL55:;}
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs);struct Cyc_PP_Doc*
Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){const char*_tmp57C;const char*
_tmp57B;const char*_tmp57A;return Cyc_PP_egroup(((_tmp57A="<",_tag_dyneither(
_tmp57A,sizeof(char),2))),((_tmp57B=">",_tag_dyneither(_tmp57B,sizeof(char),2))),((
_tmp57C=",",_tag_dyneither(_tmp57C,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_ktvar2doc,tvs));}static struct _dyneither_ptr*Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar*tv);static struct _dyneither_ptr*Cyc_Absynpp_get_name(struct
Cyc_Absyn_Tvar*tv){return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct
Cyc_List_List*tvs);struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*
tvs){if(Cyc_Absynpp_print_all_kinds)return Cyc_Absynpp_ktvars2doc(tvs);{const char*
_tmp57F;const char*_tmp57E;const char*_tmp57D;return Cyc_PP_egroup(((_tmp57D="<",
_tag_dyneither(_tmp57D,sizeof(char),2))),((_tmp57E=">",_tag_dyneither(_tmp57E,
sizeof(char),2))),((_tmp57F=",",_tag_dyneither(_tmp57F,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
tvs)));}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(
struct _tuple16*t);struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple16*t){return
Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(
struct Cyc_List_List*ts);struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*
ts){const char*_tmp582;const char*_tmp581;const char*_tmp580;return Cyc_PP_group(((
_tmp580="(",_tag_dyneither(_tmp580,sizeof(char),2))),((_tmp581=")",
_tag_dyneither(_tmp581,sizeof(char),2))),((_tmp582=",",_tag_dyneither(_tmp582,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}struct Cyc_PP_Doc*
Cyc_Absynpp_noncallatt2doc(void*att);struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(
void*att){void*_tmp92=att;_LL5F: if((int)_tmp92 != 0)goto _LL61;_LL60: goto _LL62;
_LL61: if((int)_tmp92 != 1)goto _LL63;_LL62: goto _LL64;_LL63: if((int)_tmp92 != 2)goto
_LL65;_LL64: return Cyc_PP_nil_doc();_LL65:;_LL66: return Cyc_PP_text(Cyc_Absyn_attribute2string(
att));_LL5E:;}struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts);
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){for(0;atts != 
0;atts=atts->tl){void*_tmp93=(void*)atts->hd;_LL68: if((int)_tmp93 != 0)goto _LL6A;
_LL69: {const char*_tmp583;return Cyc_PP_text(((_tmp583=" _stdcall ",
_tag_dyneither(_tmp583,sizeof(char),11))));}_LL6A: if((int)_tmp93 != 1)goto _LL6C;
_LL6B: {const char*_tmp584;return Cyc_PP_text(((_tmp584=" _cdecl ",_tag_dyneither(
_tmp584,sizeof(char),9))));}_LL6C: if((int)_tmp93 != 2)goto _LL6E;_LL6D: {const char*
_tmp585;return Cyc_PP_text(((_tmp585=" _fastcall ",_tag_dyneither(_tmp585,sizeof(
char),12))));}_LL6E:;_LL6F: goto _LL67;_LL67:;}return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*
Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts);struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(
struct Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;
atts2 != 0;atts2=atts2->tl){void*_tmp97=(void*)atts2->hd;_LL71: if((int)_tmp97 != 0)
goto _LL73;_LL72: goto _LL74;_LL73: if((int)_tmp97 != 1)goto _LL75;_LL74: goto _LL76;
_LL75: if((int)_tmp97 != 2)goto _LL77;_LL76: goto _LL70;_LL77:;_LL78: hasatt=1;goto
_LL70;_LL70:;}}if(!hasatt)return Cyc_PP_nil_doc();{const char*_tmp590;const char*
_tmp58F;const char*_tmp58E;const char*_tmp58D;const char*_tmp58C;struct Cyc_PP_Doc*
_tmp58B[3];return(_tmp58B[2]=Cyc_PP_text(((_tmp590=")",_tag_dyneither(_tmp590,
sizeof(char),2)))),((_tmp58B[1]=Cyc_PP_group(((_tmp58F="",_tag_dyneither(_tmp58F,
sizeof(char),1))),((_tmp58E="",_tag_dyneither(_tmp58E,sizeof(char),1))),((
_tmp58D=" ",_tag_dyneither(_tmp58D,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,
atts)),((_tmp58B[0]=Cyc_PP_text(((_tmp58C=" __declspec(",_tag_dyneither(_tmp58C,
sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp58B,sizeof(struct Cyc_PP_Doc*),
3)))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a);struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(
void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts);struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*
atts){if(atts == 0)return Cyc_PP_nil_doc();{void*_tmp9E=Cyc_Cyclone_c_compiler;
_LL7A: if((int)_tmp9E != 1)goto _LL7C;_LL7B: return Cyc_Absynpp_noncallconv2doc(atts);
_LL7C: if((int)_tmp9E != 0)goto _LL79;_LL7D: {const char*_tmp599;const char*_tmp598;
const char*_tmp597;const char*_tmp596;struct Cyc_PP_Doc*_tmp595[2];return(_tmp595[1]=
Cyc_PP_group(((_tmp599="((",_tag_dyneither(_tmp599,sizeof(char),3))),((_tmp598="))",
_tag_dyneither(_tmp598,sizeof(char),3))),((_tmp597=",",_tag_dyneither(_tmp597,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp595[0]=Cyc_PP_text(((
_tmp596=" __attribute__",_tag_dyneither(_tmp596,sizeof(char),15)))),Cyc_PP_cat(
_tag_dyneither(_tmp595,sizeof(struct Cyc_PP_Doc*),2)))));}_LL79:;}}int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List*tms);int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{void*_tmpA4=(void*)tms->hd;_LL7F: if(*((int*)_tmpA4)!= 2)goto
_LL81;_LL80: return 1;_LL81: if(*((int*)_tmpA4)!= 5)goto _LL83;_LL82: {void*_tmpA5=
Cyc_Cyclone_c_compiler;_LL86: if((int)_tmpA5 != 0)goto _LL88;_LL87: return 0;_LL88:;
_LL89: return Cyc_Absynpp_next_is_pointer(tms->tl);_LL85:;}_LL83:;_LL84: return 0;
_LL7E:;}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);static struct Cyc_PP_Doc*
Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*Cyc_Absynpp_question();
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output){const char*_tmp59A;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp59A="{?}",_tag_dyneither(_tmp59A,sizeof(char),4))),1);}
else{const char*_tmp59B;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp59B="?",_tag_dyneither(_tmp59B,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_lb();static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){if(Cyc_PP_tex_output){const char*
_tmp59C;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp59C="{\\lb}",
_tag_dyneither(_tmp59C,sizeof(char),6))),1);}else{const char*_tmp59D;Cyc_Absynpp_cache_lb=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp59D="{",_tag_dyneither(_tmp59D,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}static struct
Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb();static
struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){if(
Cyc_PP_tex_output){const char*_tmp59E;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp59E="{\\rb}",_tag_dyneither(_tmp59E,sizeof(char),6))),1);}else{const char*
_tmp59F;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp59F="}",
_tag_dyneither(_tmp59F,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;static
struct Cyc_PP_Doc*Cyc_Absynpp_dollar();static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){if(Cyc_PP_tex_output){const char*
_tmp5A0;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp5A0="\\$",
_tag_dyneither(_tmp5A0,sizeof(char),3))),1);}else{const char*_tmp5A1;Cyc_Absynpp_cache_dollar=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp5A1="$",_tag_dyneither(_tmp5A1,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}struct
Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss);struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss){struct Cyc_PP_Doc*_tmp5A2[3];return(_tmp5A2[2]=Cyc_Absynpp_rb(),((_tmp5A2[1]=
Cyc_PP_seq(sep,ss),((_tmp5A2[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(
_tmp5A2,sizeof(struct Cyc_PP_Doc*),3)))))));}static void Cyc_Absynpp_print_tms(
struct Cyc_List_List*tms);static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){void*_tmpAF=(void*)tms->hd;void*_tmpB0;struct Cyc_List_List*
_tmpB1;_LL8B: if(*((int*)_tmpAF)!= 0)goto _LL8D;_LL8C:{const char*_tmp5A5;void*
_tmp5A4;(_tmp5A4=0,Cyc_fprintf(Cyc_stderr,((_tmp5A5="Carray_mod ",_tag_dyneither(
_tmp5A5,sizeof(char),12))),_tag_dyneither(_tmp5A4,sizeof(void*),0)));}goto _LL8A;
_LL8D: if(*((int*)_tmpAF)!= 1)goto _LL8F;_LL8E:{const char*_tmp5A8;void*_tmp5A7;(
_tmp5A7=0,Cyc_fprintf(Cyc_stderr,((_tmp5A8="ConstArray_mod ",_tag_dyneither(
_tmp5A8,sizeof(char),16))),_tag_dyneither(_tmp5A7,sizeof(void*),0)));}goto _LL8A;
_LL8F: if(*((int*)_tmpAF)!= 3)goto _LL91;_tmpB0=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpAF)->f1;if(*((int*)_tmpB0)!= 1)goto _LL91;_tmpB1=((struct Cyc_Absyn_WithTypes_struct*)
_tmpB0)->f1;_LL90:{const char*_tmp5AB;void*_tmp5AA;(_tmp5AA=0,Cyc_fprintf(Cyc_stderr,((
_tmp5AB="Function_mod(",_tag_dyneither(_tmp5AB,sizeof(char),14))),_tag_dyneither(
_tmp5AA,sizeof(void*),0)));}for(0;_tmpB1 != 0;_tmpB1=_tmpB1->tl){struct Cyc_Core_Opt*
_tmpB8=(*((struct _tuple7*)_tmpB1->hd)).f1;if(_tmpB8 == 0){const char*_tmp5AE;void*
_tmp5AD;(_tmp5AD=0,Cyc_fprintf(Cyc_stderr,((_tmp5AE="?",_tag_dyneither(_tmp5AE,
sizeof(char),2))),_tag_dyneither(_tmp5AD,sizeof(void*),0)));}else{void*_tmp5AF;(
_tmp5AF=0,Cyc_fprintf(Cyc_stderr,*((struct _dyneither_ptr*)_tmpB8->v),
_tag_dyneither(_tmp5AF,sizeof(void*),0)));}if(_tmpB1->tl != 0){const char*_tmp5B2;
void*_tmp5B1;(_tmp5B1=0,Cyc_fprintf(Cyc_stderr,((_tmp5B2=",",_tag_dyneither(
_tmp5B2,sizeof(char),2))),_tag_dyneither(_tmp5B1,sizeof(void*),0)));}}{const char*
_tmp5B5;void*_tmp5B4;(_tmp5B4=0,Cyc_fprintf(Cyc_stderr,((_tmp5B5=") ",
_tag_dyneither(_tmp5B5,sizeof(char),3))),_tag_dyneither(_tmp5B4,sizeof(void*),0)));}
goto _LL8A;_LL91: if(*((int*)_tmpAF)!= 3)goto _LL93;_LL92:{const char*_tmp5B8;void*
_tmp5B7;(_tmp5B7=0,Cyc_fprintf(Cyc_stderr,((_tmp5B8="Function_mod()",
_tag_dyneither(_tmp5B8,sizeof(char),15))),_tag_dyneither(_tmp5B7,sizeof(void*),0)));}
goto _LL8A;_LL93: if(*((int*)_tmpAF)!= 5)goto _LL95;_LL94:{const char*_tmp5BB;void*
_tmp5BA;(_tmp5BA=0,Cyc_fprintf(Cyc_stderr,((_tmp5BB="Attributes_mod ",
_tag_dyneither(_tmp5BB,sizeof(char),16))),_tag_dyneither(_tmp5BA,sizeof(void*),0)));}
goto _LL8A;_LL95: if(*((int*)_tmpAF)!= 4)goto _LL97;_LL96:{const char*_tmp5BE;void*
_tmp5BD;(_tmp5BD=0,Cyc_fprintf(Cyc_stderr,((_tmp5BE="TypeParams_mod ",
_tag_dyneither(_tmp5BE,sizeof(char),16))),_tag_dyneither(_tmp5BD,sizeof(void*),0)));}
goto _LL8A;_LL97: if(*((int*)_tmpAF)!= 2)goto _LL8A;_LL98:{const char*_tmp5C1;void*
_tmp5C0;(_tmp5C0=0,Cyc_fprintf(Cyc_stderr,((_tmp5C1="Pointer_mod ",
_tag_dyneither(_tmp5C1,sizeof(char),13))),_tag_dyneither(_tmp5C0,sizeof(void*),0)));}
goto _LL8A;_LL8A:;}{const char*_tmp5C4;void*_tmp5C3;(_tmp5C3=0,Cyc_fprintf(Cyc_stderr,((
_tmp5C4="\n",_tag_dyneither(_tmp5C4,sizeof(char),2))),_tag_dyneither(_tmp5C3,
sizeof(void*),0)));}}struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct
Cyc_PP_Doc*d,struct Cyc_List_List*tms);struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int
is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);const char*_tmp5C9;const
char*_tmp5C8;struct Cyc_PP_Doc*_tmp5C7[3];struct Cyc_PP_Doc*p_rest=(_tmp5C7[2]=Cyc_PP_text(((
_tmp5C9=")",_tag_dyneither(_tmp5C9,sizeof(char),2)))),((_tmp5C7[1]=rest,((
_tmp5C7[0]=Cyc_PP_text(((_tmp5C8="(",_tag_dyneither(_tmp5C8,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp5C7,sizeof(struct Cyc_PP_Doc*),3)))))));void*_tmpCA=(
void*)tms->hd;union Cyc_Absyn_Constraint*_tmpCB;struct Cyc_Absyn_Exp*_tmpCC;union
Cyc_Absyn_Constraint*_tmpCD;void*_tmpCE;struct Cyc_List_List*_tmpCF;struct Cyc_List_List*
_tmpD0;struct Cyc_Position_Segment*_tmpD1;int _tmpD2;struct Cyc_Absyn_PtrAtts _tmpD3;
void*_tmpD4;union Cyc_Absyn_Constraint*_tmpD5;union Cyc_Absyn_Constraint*_tmpD6;
union Cyc_Absyn_Constraint*_tmpD7;struct Cyc_Absyn_Tqual _tmpD8;_LL9A: if(*((int*)
_tmpCA)!= 0)goto _LL9C;_tmpCB=((struct Cyc_Absyn_Carray_mod_struct*)_tmpCA)->f1;
_LL9B: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{const char*_tmp5CE;
const char*_tmp5CD;struct Cyc_PP_Doc*_tmp5CC[2];return(_tmp5CC[1]=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCB)?Cyc_PP_text(((
_tmp5CD="[]@zeroterm",_tag_dyneither(_tmp5CD,sizeof(char),12)))): Cyc_PP_text(((
_tmp5CE="[]",_tag_dyneither(_tmp5CE,sizeof(char),3)))),((_tmp5CC[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp5CC,sizeof(struct Cyc_PP_Doc*),2)))));}_LL9C: if(*((int*)_tmpCA)
!= 1)goto _LL9E;_tmpCC=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCA)->f1;
_tmpCD=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCA)->f2;_LL9D: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{const char*_tmp5D5;const char*_tmp5D4;const char*_tmp5D3;
struct Cyc_PP_Doc*_tmp5D2[4];return(_tmp5D2[3]=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmpCD)?Cyc_PP_text(((_tmp5D4="]@zeroterm",
_tag_dyneither(_tmp5D4,sizeof(char),11)))): Cyc_PP_text(((_tmp5D5="]",
_tag_dyneither(_tmp5D5,sizeof(char),2)))),((_tmp5D2[2]=Cyc_Absynpp_exp2doc(
_tmpCC),((_tmp5D2[1]=Cyc_PP_text(((_tmp5D3="[",_tag_dyneither(_tmp5D3,sizeof(
char),2)))),((_tmp5D2[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp5D2,sizeof(struct Cyc_PP_Doc*),
4)))))))));}_LL9E: if(*((int*)_tmpCA)!= 3)goto _LLA0;_tmpCE=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpCA)->f1;_LL9F: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{void*
_tmpE0=_tmpCE;struct Cyc_List_List*_tmpE1;int _tmpE2;struct Cyc_Absyn_VarargInfo*
_tmpE3;struct Cyc_Core_Opt*_tmpE4;struct Cyc_List_List*_tmpE5;struct Cyc_List_List*
_tmpE6;struct Cyc_Position_Segment*_tmpE7;_LLA7: if(*((int*)_tmpE0)!= 1)goto _LLA9;
_tmpE1=((struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f1;_tmpE2=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE0)->f2;_tmpE3=((struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f3;_tmpE4=((
struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f4;_tmpE5=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE0)->f5;_LLA8: {struct Cyc_PP_Doc*_tmp5D6[2];return(_tmp5D6[1]=Cyc_Absynpp_funargs2doc(
_tmpE1,_tmpE2,_tmpE3,_tmpE4,_tmpE5),((_tmp5D6[0]=rest,Cyc_PP_cat(_tag_dyneither(
_tmp5D6,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA9: if(*((int*)_tmpE0)!= 0)goto _LLA6;
_tmpE6=((struct Cyc_Absyn_NoTypes_struct*)_tmpE0)->f1;_tmpE7=((struct Cyc_Absyn_NoTypes_struct*)
_tmpE0)->f2;_LLAA: {const char*_tmp5DD;const char*_tmp5DC;const char*_tmp5DB;struct
Cyc_PP_Doc*_tmp5DA[2];return(_tmp5DA[1]=Cyc_PP_group(((_tmp5DD="(",
_tag_dyneither(_tmp5DD,sizeof(char),2))),((_tmp5DC=")",_tag_dyneither(_tmp5DC,
sizeof(char),2))),((_tmp5DB=",",_tag_dyneither(_tmp5DB,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,_tmpE6)),((_tmp5DA[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp5DA,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA6:;}_LLA0: if(*((int*)
_tmpCA)!= 5)goto _LLA2;_tmpCF=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpCA)->f2;
_LLA1: {void*_tmpED=Cyc_Cyclone_c_compiler;_LLAC: if((int)_tmpED != 0)goto _LLAE;
_LLAD: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{struct Cyc_PP_Doc*
_tmp5DE[2];return(_tmp5DE[1]=Cyc_Absynpp_atts2doc(_tmpCF),((_tmp5DE[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp5DE,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAE: if((int)_tmpED != 
1)goto _LLAB;_LLAF: if(Cyc_Absynpp_next_is_pointer(tms->tl)){struct Cyc_PP_Doc*
_tmp5DF[2];return(_tmp5DF[1]=rest,((_tmp5DF[0]=Cyc_Absynpp_callconv2doc(_tmpCF),
Cyc_PP_cat(_tag_dyneither(_tmp5DF,sizeof(struct Cyc_PP_Doc*),2)))));}return rest;
_LLAB:;}_LLA2: if(*((int*)_tmpCA)!= 4)goto _LLA4;_tmpD0=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpCA)->f1;_tmpD1=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCA)->f2;_tmpD2=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCA)->f3;_LLA3: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;if(_tmpD2){struct Cyc_PP_Doc*_tmp5E0[2];return(_tmp5E0[1]=
Cyc_Absynpp_ktvars2doc(_tmpD0),((_tmp5E0[0]=rest,Cyc_PP_cat(_tag_dyneither(
_tmp5E0,sizeof(struct Cyc_PP_Doc*),2)))));}else{struct Cyc_PP_Doc*_tmp5E1[2];
return(_tmp5E1[1]=Cyc_Absynpp_tvars2doc(_tmpD0),((_tmp5E1[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp5E1,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA4: if(*((int*)_tmpCA)
!= 2)goto _LL99;_tmpD3=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCA)->f1;_tmpD4=
_tmpD3.rgn;_tmpD5=_tmpD3.nullable;_tmpD6=_tmpD3.bounds;_tmpD7=_tmpD3.zero_term;
_tmpD8=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCA)->f2;_LLA5: {struct Cyc_PP_Doc*
ptr;{void*_tmpF2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmpD6);struct Cyc_Absyn_Exp*
_tmpF3;_LLB1: if((int)_tmpF2 != 0)goto _LLB3;_LLB2: ptr=Cyc_Absynpp_question();goto
_LLB0;_LLB3: if(_tmpF2 <= (void*)1)goto _LLB0;if(*((int*)_tmpF2)!= 0)goto _LLB0;
_tmpF3=((struct Cyc_Absyn_Upper_b_struct*)_tmpF2)->f1;_LLB4:{const char*_tmp5E3;
const char*_tmp5E2;ptr=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
1,_tmpD5)?(_tmp5E3="*",_tag_dyneither(_tmp5E3,sizeof(char),2)):((_tmp5E2="@",
_tag_dyneither(_tmp5E2,sizeof(char),2))));}{unsigned int _tmpF7;int _tmpF8;struct
_tuple10 _tmpF6=Cyc_Evexp_eval_const_uint_exp(_tmpF3);_tmpF7=_tmpF6.f1;_tmpF8=
_tmpF6.f2;if(!_tmpF8  || _tmpF7 != 1){struct Cyc_PP_Doc*_tmp5E4[4];ptr=((_tmp5E4[3]=
Cyc_Absynpp_rb(),((_tmp5E4[2]=Cyc_Absynpp_exp2doc(_tmpF3),((_tmp5E4[1]=Cyc_Absynpp_lb(),((
_tmp5E4[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp5E4,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB0;}_LLB0:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpD7)){const char*
_tmp5E7;struct Cyc_PP_Doc*_tmp5E6[2];ptr=((_tmp5E6[1]=Cyc_PP_text(((_tmp5E7="@zeroterm ",
_tag_dyneither(_tmp5E7,sizeof(char),11)))),((_tmp5E6[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp5E6,sizeof(struct Cyc_PP_Doc*),2))))));}else{if(is_char_ptr
 && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpD7)){
const char*_tmp5EA;struct Cyc_PP_Doc*_tmp5E9[2];ptr=((_tmp5E9[1]=Cyc_PP_text(((
_tmp5EA="@nozeroterm ",_tag_dyneither(_tmp5EA,sizeof(char),13)))),((_tmp5E9[0]=
ptr,Cyc_PP_cat(_tag_dyneither(_tmp5E9,sizeof(struct Cyc_PP_Doc*),2))))));}}}{void*
_tmpFE=Cyc_Tcutil_compress(_tmpD4);_LLB6: if((int)_tmpFE != 2)goto _LLB8;_LLB7: goto
_LLB5;_LLB8:;_LLB9: {const char*_tmp5ED;struct Cyc_PP_Doc*_tmp5EC[3];ptr=((_tmp5EC[
2]=Cyc_PP_text(((_tmp5ED=" ",_tag_dyneither(_tmp5ED,sizeof(char),2)))),((_tmp5EC[
1]=Cyc_Absynpp_typ2doc(_tmpD4),((_tmp5EC[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp5EC,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}{struct Cyc_PP_Doc*_tmp5EE[2];
ptr=((_tmp5EE[1]=Cyc_Absynpp_tqual2doc(_tmpD8),((_tmp5EE[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp5EE,sizeof(struct Cyc_PP_Doc*),2))))));}{struct Cyc_PP_Doc*
_tmp5EF[2];return(_tmp5EF[1]=rest,((_tmp5EF[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp5EF,sizeof(struct Cyc_PP_Doc*),2)))));}}_LL99:;}}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){void*_tmp106=Cyc_Tcutil_compress(
t);_LLBB: if((int)_tmp106 != 2)goto _LLBD;_LLBC: {const char*_tmp5F0;return Cyc_PP_text(((
_tmp5F0="`H",_tag_dyneither(_tmp5F0,sizeof(char),3))));}_LLBD: if((int)_tmp106 != 
3)goto _LLBF;_LLBE: {const char*_tmp5F1;return Cyc_PP_text(((_tmp5F1="`U",
_tag_dyneither(_tmp5F1,sizeof(char),3))));}_LLBF:;_LLC0: return Cyc_Absynpp_ntyp2doc(
t);_LLBA:;}static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct
Cyc_List_List**effects,void*t);static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**
rgions,struct Cyc_List_List**effects,void*t){void*_tmp109=Cyc_Tcutil_compress(t);
void*_tmp10A;struct Cyc_List_List*_tmp10B;_LLC2: if(_tmp109 <= (void*)4)goto _LLC6;
if(*((int*)_tmp109)!= 19)goto _LLC4;_tmp10A=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp109)->f1;_LLC3:{struct Cyc_List_List*_tmp5F2;*rgions=((_tmp5F2=_cycalloc(
sizeof(*_tmp5F2)),((_tmp5F2->hd=Cyc_Absynpp_rgn2doc(_tmp10A),((_tmp5F2->tl=*
rgions,_tmp5F2))))));}goto _LLC1;_LLC4: if(*((int*)_tmp109)!= 20)goto _LLC6;_tmp10B=((
struct Cyc_Absyn_JoinEff_struct*)_tmp109)->f1;_LLC5: for(0;_tmp10B != 0;_tmp10B=
_tmp10B->tl){Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp10B->hd);}goto
_LLC1;_LLC6:;_LLC7:{struct Cyc_List_List*_tmp5F3;*effects=((_tmp5F3=_cycalloc(
sizeof(*_tmp5F3)),((_tmp5F3->hd=Cyc_Absynpp_typ2doc(t),((_tmp5F3->tl=*effects,
_tmp5F3))))));}goto _LLC1;_LLC1:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t);
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct Cyc_List_List*rgions=0;struct
Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(& rgions,& effects,t);rgions=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);effects=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);if(
rgions == 0  && effects != 0){const char*_tmp5F6;const char*_tmp5F5;const char*_tmp5F4;
return Cyc_PP_group(((_tmp5F4="",_tag_dyneither(_tmp5F4,sizeof(char),1))),((
_tmp5F5="",_tag_dyneither(_tmp5F5,sizeof(char),1))),((_tmp5F6="+",_tag_dyneither(
_tmp5F6,sizeof(char),2))),effects);}else{const char*_tmp5F7;struct Cyc_PP_Doc*
_tmp111=Cyc_Absynpp_group_braces(((_tmp5F7=",",_tag_dyneither(_tmp5F7,sizeof(
char),2))),rgions);struct Cyc_List_List*_tmp5FB;const char*_tmp5FA;const char*
_tmp5F9;const char*_tmp5F8;return Cyc_PP_group(((_tmp5F8="",_tag_dyneither(_tmp5F8,
sizeof(char),1))),((_tmp5F9="",_tag_dyneither(_tmp5F9,sizeof(char),1))),((
_tmp5FA="+",_tag_dyneither(_tmp5FA,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((
_tmp5FB=_cycalloc(sizeof(*_tmp5FB)),((_tmp5FB->hd=_tmp111,((_tmp5FB->tl=0,
_tmp5FB))))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(void*k);struct Cyc_PP_Doc*
Cyc_Absynpp_aggr_kind2doc(void*k){void*_tmp117=k;_LLC9: if((int)_tmp117 != 0)goto
_LLCB;_LLCA: {const char*_tmp5FC;return Cyc_PP_text(((_tmp5FC="struct ",
_tag_dyneither(_tmp5FC,sizeof(char),8))));}_LLCB: if((int)_tmp117 != 1)goto _LLC8;
_LLCC: {const char*_tmp5FD;return Cyc_PP_text(((_tmp5FD="union ",_tag_dyneither(
_tmp5FD,sizeof(char),7))));}_LLC8:;}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*s;{void*_tmp11A=t;
struct Cyc_Core_Opt*_tmp11B;struct Cyc_Core_Opt*_tmp11C;int _tmp11D;struct Cyc_Core_Opt*
_tmp11E;struct Cyc_Absyn_Tvar*_tmp11F;struct Cyc_Absyn_DatatypeInfo _tmp120;union
Cyc_Absyn_DatatypeInfoU _tmp121;struct Cyc_List_List*_tmp122;struct Cyc_Core_Opt*
_tmp123;struct Cyc_Absyn_DatatypeFieldInfo _tmp124;union Cyc_Absyn_DatatypeFieldInfoU
_tmp125;struct Cyc_List_List*_tmp126;void*_tmp127;void*_tmp128;int _tmp129;struct
Cyc_List_List*_tmp12A;struct Cyc_Absyn_AggrInfo _tmp12B;union Cyc_Absyn_AggrInfoU
_tmp12C;struct Cyc_List_List*_tmp12D;void*_tmp12E;struct Cyc_List_List*_tmp12F;
struct Cyc_List_List*_tmp130;struct _tuple0*_tmp131;struct Cyc_Absyn_Exp*_tmp132;
struct _tuple0*_tmp133;struct Cyc_List_List*_tmp134;struct Cyc_Absyn_Typedefdecl*
_tmp135;void*_tmp136;void*_tmp137;void*_tmp138;void*_tmp139;void*_tmp13A;_LLCE:
if(_tmp11A <= (void*)4)goto _LLD4;if(*((int*)_tmp11A)!= 7)goto _LLD0;_LLCF: {const
char*_tmp5FE;return Cyc_PP_text(((_tmp5FE="[[[array]]]",_tag_dyneither(_tmp5FE,
sizeof(char),12))));}_LLD0: if(*((int*)_tmp11A)!= 8)goto _LLD2;_LLD1: return Cyc_PP_nil_doc();
_LLD2: if(*((int*)_tmp11A)!= 4)goto _LLD4;_LLD3: return Cyc_PP_nil_doc();_LLD4: if((
int)_tmp11A != 0)goto _LLD6;_LLD5:{const char*_tmp5FF;s=Cyc_PP_text(((_tmp5FF="void",
_tag_dyneither(_tmp5FF,sizeof(char),5))));}goto _LLCD;_LLD6: if(_tmp11A <= (void*)4)
goto _LLE0;if(*((int*)_tmp11A)!= 0)goto _LLD8;_tmp11B=((struct Cyc_Absyn_Evar_struct*)
_tmp11A)->f1;_tmp11C=((struct Cyc_Absyn_Evar_struct*)_tmp11A)->f2;_tmp11D=((
struct Cyc_Absyn_Evar_struct*)_tmp11A)->f3;_tmp11E=((struct Cyc_Absyn_Evar_struct*)
_tmp11A)->f4;_LLD7: if(_tmp11C != 0)return Cyc_Absynpp_ntyp2doc((void*)_tmp11C->v);
else{const char*_tmp60F;struct Cyc_Int_pa_struct _tmp60E;void*_tmp60D[1];const char*
_tmp60C;const char*_tmp60B;const char*_tmp60A;const char*_tmp609;struct Cyc_PP_Doc*
_tmp608[6];s=((_tmp608[5]=_tmp11B == 0?Cyc_Absynpp_question(): Cyc_Absynpp_kind2doc((
void*)_tmp11B->v),((_tmp608[4]=Cyc_PP_text(((_tmp60F=")::",_tag_dyneither(
_tmp60F,sizeof(char),4)))),((_tmp608[3]=(!Cyc_Absynpp_print_full_evars  || 
_tmp11E == 0)?Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)
_tmp11E->v),((_tmp608[2]=Cyc_PP_text((struct _dyneither_ptr)((_tmp60E.tag=1,((
_tmp60E.f1=(unsigned long)_tmp11D,((_tmp60D[0]=& _tmp60E,Cyc_aprintf(((_tmp60C="%d",
_tag_dyneither(_tmp60C,sizeof(char),3))),_tag_dyneither(_tmp60D,sizeof(void*),1))))))))),((
_tmp608[1]=Cyc_PP_text(((_tmp60B="(",_tag_dyneither(_tmp60B,sizeof(char),2)))),((
_tmp608[0]=Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp609="\\%",
_tag_dyneither(_tmp609,sizeof(char),3)))): Cyc_PP_text(((_tmp60A="%",
_tag_dyneither(_tmp60A,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp608,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}goto _LLCD;_LLD8: if(*((int*)_tmp11A)!= 
1)goto _LLDA;_tmp11F=((struct Cyc_Absyn_VarType_struct*)_tmp11A)->f1;_LLD9: s=Cyc_PP_textptr(
_tmp11F->name);if(Cyc_Absynpp_print_all_kinds){const char*_tmp612;struct Cyc_PP_Doc*
_tmp611[3];s=((_tmp611[2]=Cyc_Absynpp_kindbound2doc(_tmp11F->kind),((_tmp611[1]=
Cyc_PP_text(((_tmp612="::",_tag_dyneither(_tmp612,sizeof(char),3)))),((_tmp611[0]=
s,Cyc_PP_cat(_tag_dyneither(_tmp611,sizeof(struct Cyc_PP_Doc*),3))))))));}if(Cyc_Absynpp_rewrite_temp_tvars
 && Cyc_Tcutil_is_temp_tvar(_tmp11F)){const char*_tmp617;const char*_tmp616;struct
Cyc_PP_Doc*_tmp615[3];s=((_tmp615[2]=Cyc_PP_text(((_tmp617=" */",_tag_dyneither(
_tmp617,sizeof(char),4)))),((_tmp615[1]=s,((_tmp615[0]=Cyc_PP_text(((_tmp616="_ /* ",
_tag_dyneither(_tmp616,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp615,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCD;_LLDA: if(*((int*)_tmp11A)!= 2)
goto _LLDC;_tmp120=((struct Cyc_Absyn_DatatypeType_struct*)_tmp11A)->f1;_tmp121=
_tmp120.datatype_info;_tmp122=_tmp120.targs;_tmp123=_tmp120.rgn;_LLDB:{union Cyc_Absyn_DatatypeInfoU
_tmp14A=_tmp121;struct Cyc_Absyn_UnknownDatatypeInfo _tmp14B;struct _tuple0*_tmp14C;
int _tmp14D;struct Cyc_Absyn_Datatypedecl**_tmp14E;struct Cyc_Absyn_Datatypedecl*
_tmp14F;struct Cyc_Absyn_Datatypedecl _tmp150;struct _tuple0*_tmp151;int _tmp152;
_LL103: if((_tmp14A.UnknownDatatype).tag != 1)goto _LL105;_tmp14B=(struct Cyc_Absyn_UnknownDatatypeInfo)(
_tmp14A.UnknownDatatype).val;_tmp14C=_tmp14B.name;_tmp14D=_tmp14B.is_extensible;
_LL104: _tmp151=_tmp14C;_tmp152=_tmp14D;goto _LL106;_LL105: if((_tmp14A.KnownDatatype).tag
!= 2)goto _LL102;_tmp14E=(struct Cyc_Absyn_Datatypedecl**)(_tmp14A.KnownDatatype).val;
_tmp14F=*_tmp14E;_tmp150=*_tmp14F;_tmp151=_tmp150.name;_tmp152=_tmp150.is_extensible;
_LL106: {const char*_tmp618;struct Cyc_PP_Doc*_tmp153=Cyc_PP_text(((_tmp618="datatype ",
_tag_dyneither(_tmp618,sizeof(char),10))));const char*_tmp619;struct Cyc_PP_Doc*
_tmp154=_tmp152?Cyc_PP_text(((_tmp619="@extensible ",_tag_dyneither(_tmp619,
sizeof(char),13)))): Cyc_PP_nil_doc();void*r=(unsigned int)_tmp123?(void*)_tmp123->v:(
void*)2;{void*_tmp155=Cyc_Tcutil_compress(r);_LL108: if((int)_tmp155 != 2)goto
_LL10A;_LL109:{struct Cyc_PP_Doc*_tmp61A[4];s=((_tmp61A[3]=Cyc_Absynpp_tps2doc(
_tmp122),((_tmp61A[2]=Cyc_Absynpp_qvar2doc(_tmp151),((_tmp61A[1]=_tmp153,((
_tmp61A[0]=_tmp154,Cyc_PP_cat(_tag_dyneither(_tmp61A,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL107;_LL10A:;_LL10B:{const char*_tmp61D;struct Cyc_PP_Doc*_tmp61C[6];s=((
_tmp61C[5]=Cyc_Absynpp_tps2doc(_tmp122),((_tmp61C[4]=Cyc_Absynpp_qvar2doc(
_tmp151),((_tmp61C[3]=Cyc_PP_text(((_tmp61D=" ",_tag_dyneither(_tmp61D,sizeof(
char),2)))),((_tmp61C[2]=Cyc_Absynpp_typ2doc(r),((_tmp61C[1]=_tmp153,((_tmp61C[0]=
_tmp154,Cyc_PP_cat(_tag_dyneither(_tmp61C,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
goto _LL107;_LL107:;}goto _LL102;}_LL102:;}goto _LLCD;_LLDC: if(*((int*)_tmp11A)!= 3)
goto _LLDE;_tmp124=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp11A)->f1;
_tmp125=_tmp124.field_info;_tmp126=_tmp124.targs;_LLDD:{union Cyc_Absyn_DatatypeFieldInfoU
_tmp15B=_tmp125;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp15C;struct _tuple0*
_tmp15D;struct _tuple0*_tmp15E;int _tmp15F;struct _tuple1 _tmp160;struct Cyc_Absyn_Datatypedecl*
_tmp161;struct Cyc_Absyn_Datatypedecl _tmp162;struct _tuple0*_tmp163;int _tmp164;
struct Cyc_Absyn_Datatypefield*_tmp165;struct Cyc_Absyn_Datatypefield _tmp166;
struct _tuple0*_tmp167;_LL10D: if((_tmp15B.UnknownDatatypefield).tag != 1)goto
_LL10F;_tmp15C=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp15B.UnknownDatatypefield).val;
_tmp15D=_tmp15C.datatype_name;_tmp15E=_tmp15C.field_name;_tmp15F=_tmp15C.is_extensible;
_LL10E: _tmp163=_tmp15D;_tmp164=_tmp15F;_tmp167=_tmp15E;goto _LL110;_LL10F: if((
_tmp15B.KnownDatatypefield).tag != 2)goto _LL10C;_tmp160=(struct _tuple1)(_tmp15B.KnownDatatypefield).val;
_tmp161=_tmp160.f1;_tmp162=*_tmp161;_tmp163=_tmp162.name;_tmp164=_tmp162.is_extensible;
_tmp165=_tmp160.f2;_tmp166=*_tmp165;_tmp167=_tmp166.name;_LL110: {const char*
_tmp61F;const char*_tmp61E;struct Cyc_PP_Doc*_tmp168=Cyc_PP_text(_tmp164?(_tmp61F="@extensible datatype ",
_tag_dyneither(_tmp61F,sizeof(char),22)):((_tmp61E="datatype ",_tag_dyneither(
_tmp61E,sizeof(char),10))));{const char*_tmp622;struct Cyc_PP_Doc*_tmp621[4];s=((
_tmp621[3]=Cyc_Absynpp_qvar2doc(_tmp167),((_tmp621[2]=Cyc_PP_text(((_tmp622=".",
_tag_dyneither(_tmp622,sizeof(char),2)))),((_tmp621[1]=Cyc_Absynpp_qvar2doc(
_tmp163),((_tmp621[0]=_tmp168,Cyc_PP_cat(_tag_dyneither(_tmp621,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LL10C;}_LL10C:;}goto _LLCD;_LLDE: if(*((int*)_tmp11A)!= 5)goto
_LLE0;_tmp127=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp11A)->f1;_tmp128=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp11A)->f2;_LLDF: {struct
_dyneither_ptr sns;struct _dyneither_ptr ts;{void*_tmp16D=_tmp127;_LL112: if((int)
_tmp16D != 2)goto _LL114;_LL113: goto _LL115;_LL114: if((int)_tmp16D != 0)goto _LL116;
_LL115:{const char*_tmp623;sns=((_tmp623="",_tag_dyneither(_tmp623,sizeof(char),1)));}
goto _LL111;_LL116: if((int)_tmp16D != 1)goto _LL111;_LL117:{const char*_tmp624;sns=((
_tmp624="unsigned ",_tag_dyneither(_tmp624,sizeof(char),10)));}goto _LL111;_LL111:;}{
void*_tmp170=_tmp128;_LL119: if((int)_tmp170 != 0)goto _LL11B;_LL11A:{void*_tmp171=
_tmp127;_LL124: if((int)_tmp171 != 2)goto _LL126;_LL125:{const char*_tmp625;sns=((
_tmp625="",_tag_dyneither(_tmp625,sizeof(char),1)));}goto _LL123;_LL126: if((int)
_tmp171 != 0)goto _LL128;_LL127:{const char*_tmp626;sns=((_tmp626="signed ",
_tag_dyneither(_tmp626,sizeof(char),8)));}goto _LL123;_LL128: if((int)_tmp171 != 1)
goto _LL123;_LL129:{const char*_tmp627;sns=((_tmp627="unsigned ",_tag_dyneither(
_tmp627,sizeof(char),10)));}goto _LL123;_LL123:;}{const char*_tmp628;ts=((_tmp628="char",
_tag_dyneither(_tmp628,sizeof(char),5)));}goto _LL118;_LL11B: if((int)_tmp170 != 1)
goto _LL11D;_LL11C:{const char*_tmp629;ts=((_tmp629="short",_tag_dyneither(_tmp629,
sizeof(char),6)));}goto _LL118;_LL11D: if((int)_tmp170 != 2)goto _LL11F;_LL11E:{
const char*_tmp62A;ts=((_tmp62A="int",_tag_dyneither(_tmp62A,sizeof(char),4)));}
goto _LL118;_LL11F: if((int)_tmp170 != 3)goto _LL121;_LL120:{const char*_tmp62B;ts=((
_tmp62B="long",_tag_dyneither(_tmp62B,sizeof(char),5)));}goto _LL118;_LL121: if((
int)_tmp170 != 4)goto _LL118;_LL122:{void*_tmp179=Cyc_Cyclone_c_compiler;_LL12B:
if((int)_tmp179 != 0)goto _LL12D;_LL12C:{const char*_tmp62C;ts=((_tmp62C="long long",
_tag_dyneither(_tmp62C,sizeof(char),10)));}goto _LL12A;_LL12D: if((int)_tmp179 != 1)
goto _LL12A;_LL12E:{const char*_tmp62D;ts=((_tmp62D="__int64",_tag_dyneither(
_tmp62D,sizeof(char),8)));}goto _LL12A;_LL12A:;}goto _LL118;_LL118:;}{const char*
_tmp632;void*_tmp631[2];struct Cyc_String_pa_struct _tmp630;struct Cyc_String_pa_struct
_tmp62F;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp62F.tag=0,((_tmp62F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)ts),((_tmp630.tag=0,((_tmp630.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)sns),((_tmp631[0]=& _tmp630,((_tmp631[1]=&
_tmp62F,Cyc_aprintf(((_tmp632="%s%s",_tag_dyneither(_tmp632,sizeof(char),5))),
_tag_dyneither(_tmp631,sizeof(void*),2)))))))))))))));}goto _LLCD;}_LLE0: if((int)
_tmp11A != 1)goto _LLE2;_LLE1:{const char*_tmp633;s=Cyc_PP_text(((_tmp633="float",
_tag_dyneither(_tmp633,sizeof(char),6))));}goto _LLCD;_LLE2: if(_tmp11A <= (void*)4)
goto _LLF8;if(*((int*)_tmp11A)!= 6)goto _LLE4;_tmp129=((struct Cyc_Absyn_DoubleType_struct*)
_tmp11A)->f1;_LLE3: if(_tmp129){const char*_tmp634;s=Cyc_PP_text(((_tmp634="long double",
_tag_dyneither(_tmp634,sizeof(char),12))));}else{const char*_tmp635;s=Cyc_PP_text(((
_tmp635="double",_tag_dyneither(_tmp635,sizeof(char),7))));}goto _LLCD;_LLE4: if(*((
int*)_tmp11A)!= 9)goto _LLE6;_tmp12A=((struct Cyc_Absyn_TupleType_struct*)_tmp11A)->f1;
_LLE5:{struct Cyc_PP_Doc*_tmp636[2];s=((_tmp636[1]=Cyc_Absynpp_args2doc(_tmp12A),((
_tmp636[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp636,sizeof(struct
Cyc_PP_Doc*),2))))));}goto _LLCD;_LLE6: if(*((int*)_tmp11A)!= 10)goto _LLE8;_tmp12B=((
struct Cyc_Absyn_AggrType_struct*)_tmp11A)->f1;_tmp12C=_tmp12B.aggr_info;_tmp12D=
_tmp12B.targs;_LLE7: {void*_tmp185;struct _tuple0*_tmp186;struct _tuple9 _tmp184=
Cyc_Absyn_aggr_kinded_name(_tmp12C);_tmp185=_tmp184.f1;_tmp186=_tmp184.f2;{
struct Cyc_PP_Doc*_tmp637[3];s=((_tmp637[2]=Cyc_Absynpp_tps2doc(_tmp12D),((
_tmp637[1]=Cyc_Absynpp_qvar2doc(_tmp186),((_tmp637[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp185),Cyc_PP_cat(_tag_dyneither(_tmp637,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;}_LLE8: if(*((int*)_tmp11A)!= 11)goto _LLEA;_tmp12E=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp11A)->f1;_tmp12F=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp11A)->f2;_LLE9:{
struct Cyc_PP_Doc*_tmp638[4];s=((_tmp638[3]=Cyc_Absynpp_rb(),((_tmp638[2]=Cyc_PP_nest(
2,Cyc_Absynpp_aggrfields2doc(_tmp12F)),((_tmp638[1]=Cyc_Absynpp_lb(),((_tmp638[0]=
Cyc_Absynpp_aggr_kind2doc(_tmp12E),Cyc_PP_cat(_tag_dyneither(_tmp638,sizeof(
struct Cyc_PP_Doc*),4))))))))));}goto _LLCD;_LLEA: if(*((int*)_tmp11A)!= 13)goto
_LLEC;_tmp130=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp11A)->f1;_LLEB:{const
char*_tmp63B;struct Cyc_PP_Doc*_tmp63A[4];s=((_tmp63A[3]=Cyc_Absynpp_rb(),((
_tmp63A[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp130)),((_tmp63A[1]=Cyc_Absynpp_lb(),((
_tmp63A[0]=Cyc_PP_text(((_tmp63B="enum ",_tag_dyneither(_tmp63B,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp63A,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LLCD;_LLEC: if(*((int*)_tmp11A)!= 12)goto _LLEE;_tmp131=((struct Cyc_Absyn_EnumType_struct*)
_tmp11A)->f1;_LLED:{const char*_tmp63E;struct Cyc_PP_Doc*_tmp63D[2];s=((_tmp63D[1]=
Cyc_Absynpp_qvar2doc(_tmp131),((_tmp63D[0]=Cyc_PP_text(((_tmp63E="enum ",
_tag_dyneither(_tmp63E,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp63D,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLCD;_LLEE: if(*((int*)_tmp11A)!= 17)goto
_LLF0;_tmp132=((struct Cyc_Absyn_ValueofType_struct*)_tmp11A)->f1;_LLEF:{const
char*_tmp643;const char*_tmp642;struct Cyc_PP_Doc*_tmp641[3];s=((_tmp641[2]=Cyc_PP_text(((
_tmp643=")",_tag_dyneither(_tmp643,sizeof(char),2)))),((_tmp641[1]=Cyc_Absynpp_exp2doc(
_tmp132),((_tmp641[0]=Cyc_PP_text(((_tmp642="valueof_t(",_tag_dyneither(_tmp642,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp641,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LLCD;_LLF0: if(*((int*)_tmp11A)!= 16)goto _LLF2;_tmp133=((struct
Cyc_Absyn_TypedefType_struct*)_tmp11A)->f1;_tmp134=((struct Cyc_Absyn_TypedefType_struct*)
_tmp11A)->f2;_tmp135=((struct Cyc_Absyn_TypedefType_struct*)_tmp11A)->f3;_LLF1:{
struct Cyc_PP_Doc*_tmp644[2];s=((_tmp644[1]=Cyc_Absynpp_tps2doc(_tmp134),((
_tmp644[0]=Cyc_Absynpp_qvar2doc(_tmp133),Cyc_PP_cat(_tag_dyneither(_tmp644,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLCD;_LLF2: if(*((int*)_tmp11A)!= 14)goto
_LLF4;_tmp136=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp11A)->f1;_LLF3:{
const char*_tmp649;const char*_tmp648;struct Cyc_PP_Doc*_tmp647[3];s=((_tmp647[2]=
Cyc_PP_text(((_tmp649=">",_tag_dyneither(_tmp649,sizeof(char),2)))),((_tmp647[1]=
Cyc_Absynpp_rgn2doc(_tmp136),((_tmp647[0]=Cyc_PP_text(((_tmp648="region_t<",
_tag_dyneither(_tmp648,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp647,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCD;_LLF4: if(*((int*)_tmp11A)!= 15)
goto _LLF6;_tmp137=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp11A)->f1;
_tmp138=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp11A)->f2;_LLF5:{const
char*_tmp650;const char*_tmp64F;const char*_tmp64E;struct Cyc_PP_Doc*_tmp64D[5];s=((
_tmp64D[4]=Cyc_PP_text(((_tmp650=">",_tag_dyneither(_tmp650,sizeof(char),2)))),((
_tmp64D[3]=Cyc_Absynpp_rgn2doc(_tmp138),((_tmp64D[2]=Cyc_PP_text(((_tmp64F=",",
_tag_dyneither(_tmp64F,sizeof(char),2)))),((_tmp64D[1]=Cyc_Absynpp_rgn2doc(
_tmp137),((_tmp64D[0]=Cyc_PP_text(((_tmp64E="dynregion_t<",_tag_dyneither(
_tmp64E,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp64D,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LLCD;_LLF6: if(*((int*)_tmp11A)!= 18)goto _LLF8;_tmp139=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp11A)->f1;_LLF7:{const char*_tmp655;const char*
_tmp654;struct Cyc_PP_Doc*_tmp653[3];s=((_tmp653[2]=Cyc_PP_text(((_tmp655=">",
_tag_dyneither(_tmp655,sizeof(char),2)))),((_tmp653[1]=Cyc_Absynpp_typ2doc(
_tmp139),((_tmp653[0]=Cyc_PP_text(((_tmp654="tag_t<",_tag_dyneither(_tmp654,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp653,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLF8: if((int)_tmp11A != 3)goto _LLFA;_LLF9: goto _LLFB;_LLFA: if((int)
_tmp11A != 2)goto _LLFC;_LLFB: s=Cyc_Absynpp_rgn2doc(t);goto _LLCD;_LLFC: if(_tmp11A
<= (void*)4)goto _LLFE;if(*((int*)_tmp11A)!= 21)goto _LLFE;_tmp13A=(void*)((struct
Cyc_Absyn_RgnsEff_struct*)_tmp11A)->f1;_LLFD:{const char*_tmp65A;const char*
_tmp659;struct Cyc_PP_Doc*_tmp658[3];s=((_tmp658[2]=Cyc_PP_text(((_tmp65A=")",
_tag_dyneither(_tmp65A,sizeof(char),2)))),((_tmp658[1]=Cyc_Absynpp_typ2doc(
_tmp13A),((_tmp658[0]=Cyc_PP_text(((_tmp659="regions(",_tag_dyneither(_tmp659,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp658,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLFE: if(_tmp11A <= (void*)4)goto _LL100;if(*((int*)_tmp11A)!= 19)goto
_LL100;_LLFF: goto _LL101;_LL100: if(_tmp11A <= (void*)4)goto _LLCD;if(*((int*)
_tmp11A)!= 20)goto _LLCD;_LL101: s=Cyc_Absynpp_eff2doc(t);goto _LLCD;_LLCD:;}return
s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo);struct Cyc_PP_Doc*
Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _dyneither_ptr*)vo->v));}struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct
_tuple11*cmp);struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple11*cmp){
struct _tuple11 _tmp19F;void*_tmp1A0;void*_tmp1A1;struct _tuple11*_tmp19E=cmp;
_tmp19F=*_tmp19E;_tmp1A0=_tmp19F.f1;_tmp1A1=_tmp19F.f2;{const char*_tmp65D;struct
Cyc_PP_Doc*_tmp65C[3];return(_tmp65C[2]=Cyc_Absynpp_rgn2doc(_tmp1A1),((_tmp65C[1]=
Cyc_PP_text(((_tmp65D=" > ",_tag_dyneither(_tmp65D,sizeof(char),4)))),((_tmp65C[
0]=Cyc_Absynpp_rgn2doc(_tmp1A0),Cyc_PP_cat(_tag_dyneither(_tmp65C,sizeof(struct
Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*
po);struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){const char*
_tmp660;const char*_tmp65F;const char*_tmp65E;return Cyc_PP_group(((_tmp65E="",
_tag_dyneither(_tmp65E,sizeof(char),1))),((_tmp65F="",_tag_dyneither(_tmp65F,
sizeof(char),1))),((_tmp660=",",_tag_dyneither(_tmp660,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple11*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(
struct _tuple7*t);struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple7*t){struct
Cyc_Core_Opt*_tmp661;struct Cyc_Core_Opt*dopt=(*t).f1 == 0?0:((_tmp661=_cycalloc(
sizeof(*_tmp661)),((_tmp661->v=Cyc_PP_text(*((struct _dyneither_ptr*)((struct Cyc_Core_Opt*)
_check_null((*t).f1))->v)),_tmp661))));return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,
dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp1A8=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs){const char*_tmp664;struct Cyc_List_List*
_tmp663;_tmp1A8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1A8,((_tmp663=_cycalloc(sizeof(*_tmp663)),((_tmp663->hd=
Cyc_PP_text(((_tmp664="...",_tag_dyneither(_tmp664,sizeof(char),4)))),((_tmp663->tl=
0,_tmp663)))))));}else{if(cyc_varargs != 0){struct _tuple7*_tmp66D;const char*
_tmp66C;const char*_tmp66B;const char*_tmp66A;struct Cyc_PP_Doc*_tmp669[3];struct
Cyc_PP_Doc*_tmp1AB=(_tmp669[2]=Cyc_Absynpp_funarg2doc(((_tmp66D=_cycalloc(
sizeof(*_tmp66D)),((_tmp66D->f1=cyc_varargs->name,((_tmp66D->f2=cyc_varargs->tq,((
_tmp66D->f3=cyc_varargs->type,_tmp66D))))))))),((_tmp669[1]=cyc_varargs->inject?
Cyc_PP_text(((_tmp66B=" inject ",_tag_dyneither(_tmp66B,sizeof(char),9)))): Cyc_PP_text(((
_tmp66C=" ",_tag_dyneither(_tmp66C,sizeof(char),2)))),((_tmp669[0]=Cyc_PP_text(((
_tmp66A="...",_tag_dyneither(_tmp66A,sizeof(char),4)))),Cyc_PP_cat(
_tag_dyneither(_tmp669,sizeof(struct Cyc_PP_Doc*),3)))))));struct Cyc_List_List*
_tmp66E;_tmp1A8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1A8,((_tmp66E=_cycalloc(sizeof(*_tmp66E)),((_tmp66E->hd=
_tmp1AB,((_tmp66E->tl=0,_tmp66E)))))));}}{const char*_tmp671;const char*_tmp670;
const char*_tmp66F;struct Cyc_PP_Doc*_tmp1B2=Cyc_PP_group(((_tmp66F="",
_tag_dyneither(_tmp66F,sizeof(char),1))),((_tmp670="",_tag_dyneither(_tmp670,
sizeof(char),1))),((_tmp671=",",_tag_dyneither(_tmp671,sizeof(char),2))),_tmp1A8);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){const char*_tmp674;struct Cyc_PP_Doc*
_tmp673[3];_tmp1B2=((_tmp673[2]=Cyc_Absynpp_eff2doc((void*)effopt->v),((_tmp673[
1]=Cyc_PP_text(((_tmp674=";",_tag_dyneither(_tmp674,sizeof(char),2)))),((_tmp673[
0]=_tmp1B2,Cyc_PP_cat(_tag_dyneither(_tmp673,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){const char*_tmp677;struct Cyc_PP_Doc*_tmp676[3];_tmp1B2=((_tmp676[2]=
Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp676[1]=Cyc_PP_text(((_tmp677=":",
_tag_dyneither(_tmp677,sizeof(char),2)))),((_tmp676[0]=_tmp1B2,Cyc_PP_cat(
_tag_dyneither(_tmp676,sizeof(struct Cyc_PP_Doc*),3))))))));}{const char*_tmp67C;
const char*_tmp67B;struct Cyc_PP_Doc*_tmp67A[3];return(_tmp67A[2]=Cyc_PP_text(((
_tmp67C=")",_tag_dyneither(_tmp67C,sizeof(char),2)))),((_tmp67A[1]=_tmp1B2,((
_tmp67A[0]=Cyc_PP_text(((_tmp67B="(",_tag_dyneither(_tmp67B,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp67A,sizeof(struct Cyc_PP_Doc*),3)))))));}}}struct
_tuple7*Cyc_Absynpp_arg_mk_opt(struct _tuple12*arg);struct _tuple7*Cyc_Absynpp_arg_mk_opt(
struct _tuple12*arg){struct Cyc_Core_Opt*_tmp67F;struct _tuple7*_tmp67E;return(
_tmp67E=_cycalloc(sizeof(*_tmp67E)),((_tmp67E->f1=((_tmp67F=_cycalloc(sizeof(*
_tmp67F)),((_tmp67F->v=(*arg).f1,_tmp67F)))),((_tmp67E->f2=(*arg).f2,((_tmp67E->f3=(*
arg).f3,_tmp67E)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct
_dyneither_ptr*v);struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){
return Cyc_PP_text(*v);}struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*
q);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){struct Cyc_List_List*
_tmp1BF=0;int match;{union Cyc_Absyn_Nmspace _tmp1C0=(*q).f1;int _tmp1C1;struct Cyc_List_List*
_tmp1C2;struct Cyc_List_List*_tmp1C3;_LL130: if((_tmp1C0.Loc_n).tag != 3)goto _LL132;
_tmp1C1=(int)(_tmp1C0.Loc_n).val;_LL131: _tmp1C2=0;goto _LL133;_LL132: if((_tmp1C0.Rel_n).tag
!= 1)goto _LL134;_tmp1C2=(struct Cyc_List_List*)(_tmp1C0.Rel_n).val;_LL133: match=0;
_tmp1BF=_tmp1C2;goto _LL12F;_LL134: if((_tmp1C0.Abs_n).tag != 2)goto _LL12F;_tmp1C3=(
struct Cyc_List_List*)(_tmp1C0.Abs_n).val;_LL135: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1C3,Cyc_Absynpp_curr_namespace);{
struct Cyc_List_List*_tmp680;_tmp1BF=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(
_tmp680=_cycalloc(sizeof(*_tmp680)),((_tmp680->hd=Cyc_Absynpp_cyc_stringptr,((
_tmp680->tl=_tmp1C3,_tmp680))))): _tmp1C3;}goto _LL12F;_LL12F:;}if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp682;struct Cyc_List_List*_tmp681;return(struct _dyneither_ptr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1BF,((_tmp681=_cycalloc(sizeof(*_tmp681)),((_tmp681->hd=(*q).f2,((_tmp681->tl=
0,_tmp681))))))),((_tmp682="_",_tag_dyneither(_tmp682,sizeof(char),2))));}else{
if(match)return*(*q).f2;else{const char*_tmp684;struct Cyc_List_List*_tmp683;
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1BF,((_tmp683=_cycalloc(sizeof(*
_tmp683)),((_tmp683->hd=(*q).f2,((_tmp683->tl=0,_tmp683))))))),((_tmp684="::",
_tag_dyneither(_tmp684,sizeof(char),3))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){return
Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp1C9=Cyc_Absynpp_qvar2string(q);if(Cyc_PP_tex_output){
const char*_tmp686;const char*_tmp685;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp685="\\textbf{",
_tag_dyneither(_tmp685,sizeof(char),9))),(struct _dyneither_ptr)_tmp1C9),((
_tmp686="}",_tag_dyneither(_tmp686,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp1C9));}else{return Cyc_PP_text(_tmp1C9);}}struct _dyneither_ptr
Cyc_Absynpp_typedef_name2string(struct _tuple0*v);struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(
struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace _tmp1CC=(*v).f1;int
_tmp1CD;struct Cyc_List_List*_tmp1CE;struct Cyc_List_List*_tmp1CF;_LL137: if((
_tmp1CC.Loc_n).tag != 3)goto _LL139;_tmp1CD=(int)(_tmp1CC.Loc_n).val;_LL138: goto
_LL13A;_LL139: if((_tmp1CC.Rel_n).tag != 1)goto _LL13B;_tmp1CE=(struct Cyc_List_List*)(
_tmp1CC.Rel_n).val;if(_tmp1CE != 0)goto _LL13B;_LL13A: return*(*v).f2;_LL13B: if((
_tmp1CC.Abs_n).tag != 2)goto _LL13D;_tmp1CF=(struct Cyc_List_List*)(_tmp1CC.Abs_n).val;
_LL13C: if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct
Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,
_tmp1CF,Cyc_Absynpp_curr_namespace)== 0)return*(*v).f2;else{goto _LL13E;}_LL13D:;
_LL13E: {const char*_tmp687;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp687="/* bad namespace : */ ",
_tag_dyneither(_tmp687,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
v));}_LL136:;}else{return*(*v).f2;}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*
v){struct _dyneither_ptr _tmp1D1=Cyc_Absynpp_typedef_name2string(v);if(Cyc_PP_tex_output){
const char*_tmp689;const char*_tmp688;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp688="\\textbf{",
_tag_dyneither(_tmp688,sizeof(char),9))),(struct _dyneither_ptr)_tmp1D1),((
_tmp689="}",_tag_dyneither(_tmp689,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp1D1));}else{return Cyc_PP_text(_tmp1D1);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e);
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){void*_tmp1D4=e->r;void*_tmp1D5;
struct Cyc_Absyn_Exp*_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D7;_LL140: if(*((int*)
_tmp1D4)!= 0)goto _LL142;_LL141: goto _LL143;_LL142: if(*((int*)_tmp1D4)!= 1)goto
_LL144;_LL143: goto _LL145;_LL144: if(*((int*)_tmp1D4)!= 2)goto _LL146;_LL145: return
10000;_LL146: if(*((int*)_tmp1D4)!= 3)goto _LL148;_tmp1D5=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D4)->f1;_LL147: {void*_tmp1D8=_tmp1D5;_LL193: if((int)_tmp1D8 != 0)goto _LL195;
_LL194: return 100;_LL195: if((int)_tmp1D8 != 1)goto _LL197;_LL196: return 110;_LL197:
if((int)_tmp1D8 != 2)goto _LL199;_LL198: return 100;_LL199: if((int)_tmp1D8 != 3)goto
_LL19B;_LL19A: goto _LL19C;_LL19B: if((int)_tmp1D8 != 4)goto _LL19D;_LL19C: return 110;
_LL19D: if((int)_tmp1D8 != 5)goto _LL19F;_LL19E: goto _LL1A0;_LL19F: if((int)_tmp1D8 != 
6)goto _LL1A1;_LL1A0: return 70;_LL1A1: if((int)_tmp1D8 != 7)goto _LL1A3;_LL1A2: goto
_LL1A4;_LL1A3: if((int)_tmp1D8 != 8)goto _LL1A5;_LL1A4: goto _LL1A6;_LL1A5: if((int)
_tmp1D8 != 9)goto _LL1A7;_LL1A6: goto _LL1A8;_LL1A7: if((int)_tmp1D8 != 10)goto _LL1A9;
_LL1A8: return 80;_LL1A9: if((int)_tmp1D8 != 11)goto _LL1AB;_LL1AA: goto _LL1AC;_LL1AB:
if((int)_tmp1D8 != 12)goto _LL1AD;_LL1AC: return 130;_LL1AD: if((int)_tmp1D8 != 13)
goto _LL1AF;_LL1AE: return 60;_LL1AF: if((int)_tmp1D8 != 14)goto _LL1B1;_LL1B0: return
40;_LL1B1: if((int)_tmp1D8 != 15)goto _LL1B3;_LL1B2: return 50;_LL1B3: if((int)_tmp1D8
!= 16)goto _LL1B5;_LL1B4: return 90;_LL1B5: if((int)_tmp1D8 != 17)goto _LL1B7;_LL1B6:
return 80;_LL1B7: if((int)_tmp1D8 != 18)goto _LL1B9;_LL1B8: return 80;_LL1B9: if((int)
_tmp1D8 != 19)goto _LL192;_LL1BA: return 140;_LL192:;}_LL148: if(*((int*)_tmp1D4)!= 4)
goto _LL14A;_LL149: return 20;_LL14A: if(*((int*)_tmp1D4)!= 5)goto _LL14C;_LL14B:
return 130;_LL14C: if(*((int*)_tmp1D4)!= 6)goto _LL14E;_LL14D: return 30;_LL14E: if(*((
int*)_tmp1D4)!= 7)goto _LL150;_LL14F: return 35;_LL150: if(*((int*)_tmp1D4)!= 8)goto
_LL152;_LL151: return 30;_LL152: if(*((int*)_tmp1D4)!= 9)goto _LL154;_LL153: return 10;
_LL154: if(*((int*)_tmp1D4)!= 10)goto _LL156;_LL155: goto _LL157;_LL156: if(*((int*)
_tmp1D4)!= 11)goto _LL158;_LL157: return 140;_LL158: if(*((int*)_tmp1D4)!= 12)goto
_LL15A;_LL159: return 130;_LL15A: if(*((int*)_tmp1D4)!= 13)goto _LL15C;_tmp1D6=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D4)->f1;_LL15B: return Cyc_Absynpp_exp_prec(
_tmp1D6);_LL15C: if(*((int*)_tmp1D4)!= 14)goto _LL15E;_tmp1D7=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1D4)->f1;_LL15D: return Cyc_Absynpp_exp_prec(_tmp1D7);_LL15E: if(*((int*)
_tmp1D4)!= 15)goto _LL160;_LL15F: return 120;_LL160: if(*((int*)_tmp1D4)!= 17)goto
_LL162;_LL161: return 15;_LL162: if(*((int*)_tmp1D4)!= 16)goto _LL164;_LL163: goto
_LL165;_LL164: if(*((int*)_tmp1D4)!= 18)goto _LL166;_LL165: goto _LL167;_LL166: if(*((
int*)_tmp1D4)!= 19)goto _LL168;_LL167: goto _LL169;_LL168: if(*((int*)_tmp1D4)!= 40)
goto _LL16A;_LL169: goto _LL16B;_LL16A: if(*((int*)_tmp1D4)!= 39)goto _LL16C;_LL16B:
goto _LL16D;_LL16C: if(*((int*)_tmp1D4)!= 20)goto _LL16E;_LL16D: goto _LL16F;_LL16E:
if(*((int*)_tmp1D4)!= 21)goto _LL170;_LL16F: goto _LL171;_LL170: if(*((int*)_tmp1D4)
!= 22)goto _LL172;_LL171: return 130;_LL172: if(*((int*)_tmp1D4)!= 23)goto _LL174;
_LL173: goto _LL175;_LL174: if(*((int*)_tmp1D4)!= 24)goto _LL176;_LL175: goto _LL177;
_LL176: if(*((int*)_tmp1D4)!= 25)goto _LL178;_LL177: return 140;_LL178: if(*((int*)
_tmp1D4)!= 26)goto _LL17A;_LL179: return 150;_LL17A: if(*((int*)_tmp1D4)!= 27)goto
_LL17C;_LL17B: goto _LL17D;_LL17C: if(*((int*)_tmp1D4)!= 28)goto _LL17E;_LL17D: goto
_LL17F;_LL17E: if(*((int*)_tmp1D4)!= 29)goto _LL180;_LL17F: goto _LL181;_LL180: if(*((
int*)_tmp1D4)!= 30)goto _LL182;_LL181: goto _LL183;_LL182: if(*((int*)_tmp1D4)!= 31)
goto _LL184;_LL183: goto _LL185;_LL184: if(*((int*)_tmp1D4)!= 32)goto _LL186;_LL185:
goto _LL187;_LL186: if(*((int*)_tmp1D4)!= 33)goto _LL188;_LL187: goto _LL189;_LL188:
if(*((int*)_tmp1D4)!= 34)goto _LL18A;_LL189: goto _LL18B;_LL18A: if(*((int*)_tmp1D4)
!= 35)goto _LL18C;_LL18B: goto _LL18D;_LL18C: if(*((int*)_tmp1D4)!= 36)goto _LL18E;
_LL18D: goto _LL18F;_LL18E: if(*((int*)_tmp1D4)!= 37)goto _LL190;_LL18F: return 140;
_LL190: if(*((int*)_tmp1D4)!= 38)goto _LL13F;_LL191: return 10000;_LL13F:;}struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(
struct Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int
inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp1D9=e->r;union Cyc_Absyn_Cnst _tmp1DA;struct _tuple0*_tmp1DB;struct
_tuple0*_tmp1DC;void*_tmp1DD;struct Cyc_List_List*_tmp1DE;struct Cyc_Absyn_Exp*
_tmp1DF;struct Cyc_Core_Opt*_tmp1E0;struct Cyc_Absyn_Exp*_tmp1E1;struct Cyc_Absyn_Exp*
_tmp1E2;void*_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E4;struct Cyc_Absyn_Exp*_tmp1E5;
struct Cyc_Absyn_Exp*_tmp1E6;struct Cyc_Absyn_Exp*_tmp1E7;struct Cyc_Absyn_Exp*
_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EA;struct Cyc_Absyn_Exp*
_tmp1EB;struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1ED;struct Cyc_List_List*
_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;struct Cyc_List_List*_tmp1F0;struct Cyc_Absyn_Exp*
_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_Absyn_Exp*_tmp1F3;void*_tmp1F4;
struct Cyc_Absyn_Exp*_tmp1F5;struct Cyc_Absyn_Exp*_tmp1F6;struct Cyc_Absyn_Exp*
_tmp1F7;struct Cyc_Absyn_Exp*_tmp1F8;void*_tmp1F9;struct Cyc_Absyn_Exp*_tmp1FA;
void*_tmp1FB;struct Cyc_Absyn_Exp*_tmp1FC;struct _dyneither_ptr*_tmp1FD;void*
_tmp1FE;void*_tmp1FF;struct _dyneither_ptr*_tmp200;void*_tmp201;void*_tmp202;
unsigned int _tmp203;struct Cyc_List_List*_tmp204;void*_tmp205;struct Cyc_Absyn_Exp*
_tmp206;struct Cyc_Absyn_Exp*_tmp207;struct _dyneither_ptr*_tmp208;struct Cyc_Absyn_Exp*
_tmp209;struct _dyneither_ptr*_tmp20A;struct Cyc_Absyn_Exp*_tmp20B;struct Cyc_Absyn_Exp*
_tmp20C;struct Cyc_List_List*_tmp20D;struct _tuple7*_tmp20E;struct Cyc_List_List*
_tmp20F;struct Cyc_List_List*_tmp210;struct Cyc_Absyn_Vardecl*_tmp211;struct Cyc_Absyn_Exp*
_tmp212;struct Cyc_Absyn_Exp*_tmp213;struct _tuple0*_tmp214;struct Cyc_List_List*
_tmp215;struct Cyc_List_List*_tmp216;struct Cyc_List_List*_tmp217;struct Cyc_List_List*
_tmp218;struct Cyc_Absyn_Datatypefield*_tmp219;struct _tuple0*_tmp21A;struct
_tuple0*_tmp21B;struct Cyc_Absyn_MallocInfo _tmp21C;int _tmp21D;struct Cyc_Absyn_Exp*
_tmp21E;void**_tmp21F;struct Cyc_Absyn_Exp*_tmp220;struct Cyc_Absyn_Exp*_tmp221;
struct Cyc_Absyn_Exp*_tmp222;struct Cyc_Core_Opt*_tmp223;struct Cyc_List_List*
_tmp224;struct Cyc_Absyn_Stmt*_tmp225;_LL1BC: if(*((int*)_tmp1D9)!= 0)goto _LL1BE;
_tmp1DA=((struct Cyc_Absyn_Const_e_struct*)_tmp1D9)->f1;_LL1BD: s=Cyc_Absynpp_cnst2doc(
_tmp1DA);goto _LL1BB;_LL1BE: if(*((int*)_tmp1D9)!= 1)goto _LL1C0;_tmp1DB=((struct
Cyc_Absyn_Var_e_struct*)_tmp1D9)->f1;_LL1BF: _tmp1DC=_tmp1DB;goto _LL1C1;_LL1C0:
if(*((int*)_tmp1D9)!= 2)goto _LL1C2;_tmp1DC=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp1D9)->f1;_LL1C1: s=Cyc_Absynpp_qvar2doc(_tmp1DC);goto _LL1BB;_LL1C2: if(*((int*)
_tmp1D9)!= 3)goto _LL1C4;_tmp1DD=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D9)->f1;_tmp1DE=((struct Cyc_Absyn_Primop_e_struct*)_tmp1D9)->f2;_LL1C3: s=
Cyc_Absynpp_primapp2doc(myprec,_tmp1DD,_tmp1DE);goto _LL1BB;_LL1C4: if(*((int*)
_tmp1D9)!= 4)goto _LL1C6;_tmp1DF=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1D9)->f1;
_tmp1E0=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1D9)->f2;_tmp1E1=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1D9)->f3;_LL1C5:{const char*_tmp68E;const char*_tmp68D;struct Cyc_PP_Doc*
_tmp68C[5];s=((_tmp68C[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E1),((_tmp68C[3]=
Cyc_PP_text(((_tmp68E="= ",_tag_dyneither(_tmp68E,sizeof(char),3)))),((_tmp68C[2]=
_tmp1E0 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((void*)_tmp1E0->v),((_tmp68C[1]=
Cyc_PP_text(((_tmp68D=" ",_tag_dyneither(_tmp68D,sizeof(char),2)))),((_tmp68C[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DF),Cyc_PP_cat(_tag_dyneither(_tmp68C,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1BB;_LL1C6: if(*((int*)_tmp1D9)!= 
5)goto _LL1C8;_tmp1E2=((struct Cyc_Absyn_Increment_e_struct*)_tmp1D9)->f1;_tmp1E3=(
void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp1D9)->f2;_LL1C7: {struct Cyc_PP_Doc*
_tmp229=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E2);{void*_tmp22A=_tmp1E3;_LL211:
if((int)_tmp22A != 0)goto _LL213;_LL212:{const char*_tmp691;struct Cyc_PP_Doc*
_tmp690[2];s=((_tmp690[1]=_tmp229,((_tmp690[0]=Cyc_PP_text(((_tmp691="++",
_tag_dyneither(_tmp691,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp690,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL210;_LL213: if((int)_tmp22A != 2)goto
_LL215;_LL214:{const char*_tmp694;struct Cyc_PP_Doc*_tmp693[2];s=((_tmp693[1]=
_tmp229,((_tmp693[0]=Cyc_PP_text(((_tmp694="--",_tag_dyneither(_tmp694,sizeof(
char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp693,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL210;_LL215: if((int)_tmp22A != 1)goto _LL217;_LL216:{const char*_tmp697;
struct Cyc_PP_Doc*_tmp696[2];s=((_tmp696[1]=Cyc_PP_text(((_tmp697="++",
_tag_dyneither(_tmp697,sizeof(char),3)))),((_tmp696[0]=_tmp229,Cyc_PP_cat(
_tag_dyneither(_tmp696,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL210;_LL217: if((
int)_tmp22A != 3)goto _LL210;_LL218:{const char*_tmp69A;struct Cyc_PP_Doc*_tmp699[2];
s=((_tmp699[1]=Cyc_PP_text(((_tmp69A="--",_tag_dyneither(_tmp69A,sizeof(char),3)))),((
_tmp699[0]=_tmp229,Cyc_PP_cat(_tag_dyneither(_tmp699,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL210;_LL210:;}goto _LL1BB;}_LL1C8: if(*((int*)_tmp1D9)!= 6)goto _LL1CA;
_tmp1E4=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1D9)->f1;_tmp1E5=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1D9)->f2;_tmp1E6=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1D9)->f3;_LL1C9:{
const char*_tmp69F;const char*_tmp69E;struct Cyc_PP_Doc*_tmp69D[5];s=((_tmp69D[4]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E6),((_tmp69D[3]=Cyc_PP_text(((_tmp69F=" : ",
_tag_dyneither(_tmp69F,sizeof(char),4)))),((_tmp69D[2]=Cyc_Absynpp_exp2doc_prec(
0,_tmp1E5),((_tmp69D[1]=Cyc_PP_text(((_tmp69E=" ? ",_tag_dyneither(_tmp69E,
sizeof(char),4)))),((_tmp69D[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E4),Cyc_PP_cat(
_tag_dyneither(_tmp69D,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1BB;
_LL1CA: if(*((int*)_tmp1D9)!= 7)goto _LL1CC;_tmp1E7=((struct Cyc_Absyn_And_e_struct*)
_tmp1D9)->f1;_tmp1E8=((struct Cyc_Absyn_And_e_struct*)_tmp1D9)->f2;_LL1CB:{const
char*_tmp6A2;struct Cyc_PP_Doc*_tmp6A1[3];s=((_tmp6A1[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E8),((_tmp6A1[1]=Cyc_PP_text(((_tmp6A2=" && ",_tag_dyneither(_tmp6A2,
sizeof(char),5)))),((_tmp6A1[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E7),Cyc_PP_cat(
_tag_dyneither(_tmp6A1,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1BB;_LL1CC:
if(*((int*)_tmp1D9)!= 8)goto _LL1CE;_tmp1E9=((struct Cyc_Absyn_Or_e_struct*)
_tmp1D9)->f1;_tmp1EA=((struct Cyc_Absyn_Or_e_struct*)_tmp1D9)->f2;_LL1CD:{const
char*_tmp6A5;struct Cyc_PP_Doc*_tmp6A4[3];s=((_tmp6A4[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EA),((_tmp6A4[1]=Cyc_PP_text(((_tmp6A5=" || ",_tag_dyneither(_tmp6A5,
sizeof(char),5)))),((_tmp6A4[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E9),Cyc_PP_cat(
_tag_dyneither(_tmp6A4,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1BB;_LL1CE:
if(*((int*)_tmp1D9)!= 9)goto _LL1D0;_tmp1EB=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1D9)->f1;_tmp1EC=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1D9)->f2;_LL1CF:{
const char*_tmp6AC;const char*_tmp6AB;const char*_tmp6AA;struct Cyc_PP_Doc*_tmp6A9[5];
s=((_tmp6A9[4]=Cyc_PP_text(((_tmp6AC=")",_tag_dyneither(_tmp6AC,sizeof(char),2)))),((
_tmp6A9[3]=Cyc_Absynpp_exp2doc(_tmp1EC),((_tmp6A9[2]=Cyc_PP_text(((_tmp6AB=", ",
_tag_dyneither(_tmp6AB,sizeof(char),3)))),((_tmp6A9[1]=Cyc_Absynpp_exp2doc(
_tmp1EB),((_tmp6A9[0]=Cyc_PP_text(((_tmp6AA="(",_tag_dyneither(_tmp6AA,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6A9,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1BB;_LL1D0: if(*((int*)_tmp1D9)!= 10)goto _LL1D2;_tmp1ED=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp1D9)->f1;_tmp1EE=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1D9)->f2;_LL1D1:{
const char*_tmp6B1;const char*_tmp6B0;struct Cyc_PP_Doc*_tmp6AF[4];s=((_tmp6AF[3]=
Cyc_PP_text(((_tmp6B1=")",_tag_dyneither(_tmp6B1,sizeof(char),2)))),((_tmp6AF[2]=
Cyc_Absynpp_exps2doc_prec(20,_tmp1EE),((_tmp6AF[1]=Cyc_PP_text(((_tmp6B0="(",
_tag_dyneither(_tmp6B0,sizeof(char),2)))),((_tmp6AF[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1ED),Cyc_PP_cat(_tag_dyneither(_tmp6AF,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1BB;_LL1D2: if(*((int*)_tmp1D9)!= 11)goto _LL1D4;_tmp1EF=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1D9)->f1;_tmp1F0=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1D9)->f2;_LL1D3:{
const char*_tmp6B6;const char*_tmp6B5;struct Cyc_PP_Doc*_tmp6B4[4];s=((_tmp6B4[3]=
Cyc_PP_text(((_tmp6B6=")",_tag_dyneither(_tmp6B6,sizeof(char),2)))),((_tmp6B4[2]=
Cyc_Absynpp_exps2doc_prec(20,_tmp1F0),((_tmp6B4[1]=Cyc_PP_text(((_tmp6B5="(",
_tag_dyneither(_tmp6B5,sizeof(char),2)))),((_tmp6B4[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EF),Cyc_PP_cat(_tag_dyneither(_tmp6B4,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1BB;_LL1D4: if(*((int*)_tmp1D9)!= 12)goto _LL1D6;_tmp1F1=((struct Cyc_Absyn_Throw_e_struct*)
_tmp1D9)->f1;_LL1D5:{const char*_tmp6B9;struct Cyc_PP_Doc*_tmp6B8[2];s=((_tmp6B8[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F1),((_tmp6B8[0]=Cyc_PP_text(((_tmp6B9="throw ",
_tag_dyneither(_tmp6B9,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp6B8,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1BB;_LL1D6: if(*((int*)_tmp1D9)!= 13)
goto _LL1D8;_tmp1F2=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D9)->f1;_LL1D7:
s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1F2);goto _LL1BB;_LL1D8: if(*((int*)_tmp1D9)
!= 14)goto _LL1DA;_tmp1F3=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1D9)->f1;
_LL1D9: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1F3);goto _LL1BB;_LL1DA: if(*((int*)
_tmp1D9)!= 15)goto _LL1DC;_tmp1F4=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1D9)->f1;
_tmp1F5=((struct Cyc_Absyn_Cast_e_struct*)_tmp1D9)->f2;_LL1DB:{const char*_tmp6BE;
const char*_tmp6BD;struct Cyc_PP_Doc*_tmp6BC[4];s=((_tmp6BC[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F5),((_tmp6BC[2]=Cyc_PP_text(((_tmp6BE=")",_tag_dyneither(_tmp6BE,
sizeof(char),2)))),((_tmp6BC[1]=Cyc_Absynpp_typ2doc(_tmp1F4),((_tmp6BC[0]=Cyc_PP_text(((
_tmp6BD="(",_tag_dyneither(_tmp6BD,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp6BC,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1BB;_LL1DC: if(*((int*)
_tmp1D9)!= 16)goto _LL1DE;_tmp1F6=((struct Cyc_Absyn_Address_e_struct*)_tmp1D9)->f1;
_LL1DD:{const char*_tmp6C1;struct Cyc_PP_Doc*_tmp6C0[2];s=((_tmp6C0[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F6),((_tmp6C0[0]=Cyc_PP_text(((_tmp6C1="&",_tag_dyneither(_tmp6C1,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6C0,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1BB;_LL1DE: if(*((int*)_tmp1D9)!= 17)goto _LL1E0;_tmp1F7=((struct Cyc_Absyn_New_e_struct*)
_tmp1D9)->f1;_tmp1F8=((struct Cyc_Absyn_New_e_struct*)_tmp1D9)->f2;_LL1DF: if(
_tmp1F7 == 0){const char*_tmp6C4;struct Cyc_PP_Doc*_tmp6C3[2];s=((_tmp6C3[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F8),((_tmp6C3[0]=Cyc_PP_text(((_tmp6C4="new ",_tag_dyneither(_tmp6C4,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp6C3,sizeof(struct Cyc_PP_Doc*),2))))));}
else{const char*_tmp6C9;const char*_tmp6C8;struct Cyc_PP_Doc*_tmp6C7[4];s=((_tmp6C7[
3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F8),((_tmp6C7[2]=Cyc_PP_text(((_tmp6C9=") ",
_tag_dyneither(_tmp6C9,sizeof(char),3)))),((_tmp6C7[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp1F7),((_tmp6C7[0]=Cyc_PP_text(((_tmp6C8="rnew(",
_tag_dyneither(_tmp6C8,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp6C7,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1BB;_LL1E0: if(*((int*)_tmp1D9)!= 
18)goto _LL1E2;_tmp1F9=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp1D9)->f1;
_LL1E1:{const char*_tmp6CE;const char*_tmp6CD;struct Cyc_PP_Doc*_tmp6CC[3];s=((
_tmp6CC[2]=Cyc_PP_text(((_tmp6CE=")",_tag_dyneither(_tmp6CE,sizeof(char),2)))),((
_tmp6CC[1]=Cyc_Absynpp_typ2doc(_tmp1F9),((_tmp6CC[0]=Cyc_PP_text(((_tmp6CD="sizeof(",
_tag_dyneither(_tmp6CD,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp6CC,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1BB;_LL1E2: if(*((int*)_tmp1D9)!= 19)
goto _LL1E4;_tmp1FA=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp1D9)->f1;_LL1E3:{
const char*_tmp6D3;const char*_tmp6D2;struct Cyc_PP_Doc*_tmp6D1[3];s=((_tmp6D1[2]=
Cyc_PP_text(((_tmp6D3=")",_tag_dyneither(_tmp6D3,sizeof(char),2)))),((_tmp6D1[1]=
Cyc_Absynpp_exp2doc(_tmp1FA),((_tmp6D1[0]=Cyc_PP_text(((_tmp6D2="sizeof(",
_tag_dyneither(_tmp6D2,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp6D1,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1BB;_LL1E4: if(*((int*)_tmp1D9)!= 40)
goto _LL1E6;_tmp1FB=(void*)((struct Cyc_Absyn_Valueof_e_struct*)_tmp1D9)->f1;
_LL1E5:{const char*_tmp6D8;const char*_tmp6D7;struct Cyc_PP_Doc*_tmp6D6[3];s=((
_tmp6D6[2]=Cyc_PP_text(((_tmp6D8=")",_tag_dyneither(_tmp6D8,sizeof(char),2)))),((
_tmp6D6[1]=Cyc_Absynpp_typ2doc(_tmp1FB),((_tmp6D6[0]=Cyc_PP_text(((_tmp6D7="valueof(",
_tag_dyneither(_tmp6D7,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp6D6,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1BB;_LL1E6: if(*((int*)_tmp1D9)!= 39)
goto _LL1E8;_tmp1FC=((struct Cyc_Absyn_Tagcheck_e_struct*)_tmp1D9)->f1;_tmp1FD=((
struct Cyc_Absyn_Tagcheck_e_struct*)_tmp1D9)->f2;_LL1E7:{const char*_tmp6DF;const
char*_tmp6DE;const char*_tmp6DD;struct Cyc_PP_Doc*_tmp6DC[5];s=((_tmp6DC[4]=Cyc_PP_text(((
_tmp6DF=")",_tag_dyneither(_tmp6DF,sizeof(char),2)))),((_tmp6DC[3]=Cyc_PP_textptr(
_tmp1FD),((_tmp6DC[2]=Cyc_PP_text(((_tmp6DE=".",_tag_dyneither(_tmp6DE,sizeof(
char),2)))),((_tmp6DC[1]=Cyc_Absynpp_exp2doc(_tmp1FC),((_tmp6DC[0]=Cyc_PP_text(((
_tmp6DD="tagcheck(",_tag_dyneither(_tmp6DD,sizeof(char),10)))),Cyc_PP_cat(
_tag_dyneither(_tmp6DC,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1BB;
_LL1E8: if(*((int*)_tmp1D9)!= 20)goto _LL1EA;_tmp1FE=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1D9)->f1;_tmp1FF=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D9)->f2;
if(*((int*)_tmp1FF)!= 0)goto _LL1EA;_tmp200=((struct Cyc_Absyn_StructField_struct*)
_tmp1FF)->f1;_LL1E9:{const char*_tmp6E6;const char*_tmp6E5;const char*_tmp6E4;
struct Cyc_PP_Doc*_tmp6E3[5];s=((_tmp6E3[4]=Cyc_PP_text(((_tmp6E6=")",
_tag_dyneither(_tmp6E6,sizeof(char),2)))),((_tmp6E3[3]=Cyc_PP_textptr(_tmp200),((
_tmp6E3[2]=Cyc_PP_text(((_tmp6E5=",",_tag_dyneither(_tmp6E5,sizeof(char),2)))),((
_tmp6E3[1]=Cyc_Absynpp_typ2doc(_tmp1FE),((_tmp6E3[0]=Cyc_PP_text(((_tmp6E4="offsetof(",
_tag_dyneither(_tmp6E4,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp6E3,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1BB;_LL1EA: if(*((int*)_tmp1D9)!= 
20)goto _LL1EC;_tmp201=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D9)->f1;
_tmp202=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D9)->f2;if(*((int*)
_tmp202)!= 1)goto _LL1EC;_tmp203=((struct Cyc_Absyn_TupleIndex_struct*)_tmp202)->f1;
_LL1EB:{const char*_tmp6F4;struct Cyc_Int_pa_struct _tmp6F3;void*_tmp6F2[1];const
char*_tmp6F1;const char*_tmp6F0;const char*_tmp6EF;struct Cyc_PP_Doc*_tmp6EE[5];s=((
_tmp6EE[4]=Cyc_PP_text(((_tmp6F4=")",_tag_dyneither(_tmp6F4,sizeof(char),2)))),((
_tmp6EE[3]=Cyc_PP_text((struct _dyneither_ptr)((_tmp6F3.tag=1,((_tmp6F3.f1=(
unsigned long)((int)_tmp203),((_tmp6F2[0]=& _tmp6F3,Cyc_aprintf(((_tmp6F1="%d",
_tag_dyneither(_tmp6F1,sizeof(char),3))),_tag_dyneither(_tmp6F2,sizeof(void*),1))))))))),((
_tmp6EE[2]=Cyc_PP_text(((_tmp6F0=",",_tag_dyneither(_tmp6F0,sizeof(char),2)))),((
_tmp6EE[1]=Cyc_Absynpp_typ2doc(_tmp201),((_tmp6EE[0]=Cyc_PP_text(((_tmp6EF="offsetof(",
_tag_dyneither(_tmp6EF,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp6EE,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1BB;_LL1EC: if(*((int*)_tmp1D9)!= 
21)goto _LL1EE;_tmp204=((struct Cyc_Absyn_Gentyp_e_struct*)_tmp1D9)->f1;_tmp205=(
void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp1D9)->f2;_LL1ED:{const char*_tmp6F9;
const char*_tmp6F8;struct Cyc_PP_Doc*_tmp6F7[4];s=((_tmp6F7[3]=Cyc_PP_text(((
_tmp6F9=")",_tag_dyneither(_tmp6F9,sizeof(char),2)))),((_tmp6F7[2]=Cyc_Absynpp_typ2doc(
_tmp205),((_tmp6F7[1]=Cyc_Absynpp_tvars2doc(_tmp204),((_tmp6F7[0]=Cyc_PP_text(((
_tmp6F8="__gen(",_tag_dyneither(_tmp6F8,sizeof(char),7)))),Cyc_PP_cat(
_tag_dyneither(_tmp6F7,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1BB;_LL1EE:
if(*((int*)_tmp1D9)!= 22)goto _LL1F0;_tmp206=((struct Cyc_Absyn_Deref_e_struct*)
_tmp1D9)->f1;_LL1EF:{const char*_tmp6FC;struct Cyc_PP_Doc*_tmp6FB[2];s=((_tmp6FB[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp206),((_tmp6FB[0]=Cyc_PP_text(((_tmp6FC="*",
_tag_dyneither(_tmp6FC,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6FB,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1BB;_LL1F0: if(*((int*)_tmp1D9)!= 23)
goto _LL1F2;_tmp207=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D9)->f1;_tmp208=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D9)->f2;_LL1F1:{const char*_tmp6FF;
struct Cyc_PP_Doc*_tmp6FE[3];s=((_tmp6FE[2]=Cyc_PP_textptr(_tmp208),((_tmp6FE[1]=
Cyc_PP_text(((_tmp6FF=".",_tag_dyneither(_tmp6FF,sizeof(char),2)))),((_tmp6FE[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp207),Cyc_PP_cat(_tag_dyneither(_tmp6FE,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1BB;_LL1F2: if(*((int*)_tmp1D9)!= 24)
goto _LL1F4;_tmp209=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1D9)->f1;_tmp20A=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1D9)->f2;_LL1F3:{const char*_tmp702;
struct Cyc_PP_Doc*_tmp701[3];s=((_tmp701[2]=Cyc_PP_textptr(_tmp20A),((_tmp701[1]=
Cyc_PP_text(((_tmp702="->",_tag_dyneither(_tmp702,sizeof(char),3)))),((_tmp701[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp209),Cyc_PP_cat(_tag_dyneither(_tmp701,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1BB;_LL1F4: if(*((int*)_tmp1D9)!= 25)
goto _LL1F6;_tmp20B=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1D9)->f1;_tmp20C=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp1D9)->f2;_LL1F5:{const char*_tmp707;const
char*_tmp706;struct Cyc_PP_Doc*_tmp705[4];s=((_tmp705[3]=Cyc_PP_text(((_tmp707="]",
_tag_dyneither(_tmp707,sizeof(char),2)))),((_tmp705[2]=Cyc_Absynpp_exp2doc(
_tmp20C),((_tmp705[1]=Cyc_PP_text(((_tmp706="[",_tag_dyneither(_tmp706,sizeof(
char),2)))),((_tmp705[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp20B),Cyc_PP_cat(
_tag_dyneither(_tmp705,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1BB;_LL1F6:
if(*((int*)_tmp1D9)!= 26)goto _LL1F8;_tmp20D=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp1D9)->f1;_LL1F7:{const char*_tmp70C;const char*_tmp70B;struct Cyc_PP_Doc*
_tmp70A[4];s=((_tmp70A[3]=Cyc_PP_text(((_tmp70C=")",_tag_dyneither(_tmp70C,
sizeof(char),2)))),((_tmp70A[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp20D),((_tmp70A[
1]=Cyc_PP_text(((_tmp70B="(",_tag_dyneither(_tmp70B,sizeof(char),2)))),((_tmp70A[
0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp70A,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LL1BB;_LL1F8: if(*((int*)_tmp1D9)!= 27)goto _LL1FA;_tmp20E=((
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp1D9)->f1;_tmp20F=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp1D9)->f2;_LL1F9:{const char*_tmp713;const char*_tmp712;const char*_tmp711;
struct Cyc_PP_Doc*_tmp710[4];s=((_tmp710[3]=Cyc_Absynpp_group_braces(((_tmp713=",",
_tag_dyneither(_tmp713,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp20F)),((_tmp710[2]=Cyc_PP_text(((_tmp712=")",_tag_dyneither(_tmp712,sizeof(
char),2)))),((_tmp710[1]=Cyc_Absynpp_typ2doc((*_tmp20E).f3),((_tmp710[0]=Cyc_PP_text(((
_tmp711="(",_tag_dyneither(_tmp711,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp710,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1BB;_LL1FA: if(*((int*)
_tmp1D9)!= 28)goto _LL1FC;_tmp210=((struct Cyc_Absyn_Array_e_struct*)_tmp1D9)->f1;
_LL1FB:{const char*_tmp714;s=Cyc_Absynpp_group_braces(((_tmp714=",",
_tag_dyneither(_tmp714,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp210));}goto _LL1BB;_LL1FC: if(*((int*)_tmp1D9)!= 29)goto _LL1FE;_tmp211=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D9)->f1;_tmp212=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1D9)->f2;_tmp213=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D9)->f3;
_LL1FD:{const char*_tmp71B;const char*_tmp71A;const char*_tmp719;struct Cyc_PP_Doc*
_tmp718[8];s=((_tmp718[7]=Cyc_Absynpp_rb(),((_tmp718[6]=Cyc_Absynpp_exp2doc(
_tmp213),((_tmp718[5]=Cyc_PP_text(((_tmp71B=" : ",_tag_dyneither(_tmp71B,sizeof(
char),4)))),((_tmp718[4]=Cyc_Absynpp_exp2doc(_tmp212),((_tmp718[3]=Cyc_PP_text(((
_tmp71A=" < ",_tag_dyneither(_tmp71A,sizeof(char),4)))),((_tmp718[2]=Cyc_PP_text(*(*
_tmp211->name).f2),((_tmp718[1]=Cyc_PP_text(((_tmp719="for ",_tag_dyneither(
_tmp719,sizeof(char),5)))),((_tmp718[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp718,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}goto _LL1BB;
_LL1FE: if(*((int*)_tmp1D9)!= 30)goto _LL200;_tmp214=((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp1D9)->f1;_tmp215=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp1D9)->f2;_tmp216=((
struct Cyc_Absyn_Aggregate_e_struct*)_tmp1D9)->f3;_LL1FF: {struct Cyc_List_List*
_tmp280=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp216);{const char*_tmp720;
struct Cyc_List_List*_tmp71F;struct Cyc_PP_Doc*_tmp71E[2];s=((_tmp71E[1]=Cyc_Absynpp_group_braces(((
_tmp720=",",_tag_dyneither(_tmp720,sizeof(char),2))),_tmp215 != 0?(_tmp71F=
_cycalloc(sizeof(*_tmp71F)),((_tmp71F->hd=Cyc_Absynpp_tps2doc(_tmp215),((_tmp71F->tl=
_tmp280,_tmp71F))))): _tmp280),((_tmp71E[0]=Cyc_Absynpp_qvar2doc(_tmp214),Cyc_PP_cat(
_tag_dyneither(_tmp71E,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1BB;}_LL200: if(*((
int*)_tmp1D9)!= 31)goto _LL202;_tmp217=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1D9)->f2;_LL201:{const char*_tmp721;s=Cyc_Absynpp_group_braces(((_tmp721=",",
_tag_dyneither(_tmp721,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp217));}goto _LL1BB;_LL202: if(*((int*)_tmp1D9)!= 32)goto _LL204;_tmp218=((
struct Cyc_Absyn_Datatype_e_struct*)_tmp1D9)->f1;_tmp219=((struct Cyc_Absyn_Datatype_e_struct*)
_tmp1D9)->f3;_LL203: if(_tmp218 == 0)s=Cyc_Absynpp_qvar2doc(_tmp219->name);else{
const char*_tmp728;const char*_tmp727;const char*_tmp726;struct Cyc_PP_Doc*_tmp725[2];
s=((_tmp725[1]=Cyc_PP_egroup(((_tmp728="(",_tag_dyneither(_tmp728,sizeof(char),2))),((
_tmp727=")",_tag_dyneither(_tmp727,sizeof(char),2))),((_tmp726=",",
_tag_dyneither(_tmp726,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,
_tmp218)),((_tmp725[0]=Cyc_Absynpp_qvar2doc(_tmp219->name),Cyc_PP_cat(
_tag_dyneither(_tmp725,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1BB;_LL204: if(*((
int*)_tmp1D9)!= 33)goto _LL206;_tmp21A=((struct Cyc_Absyn_Enum_e_struct*)_tmp1D9)->f1;
_LL205: s=Cyc_Absynpp_qvar2doc(_tmp21A);goto _LL1BB;_LL206: if(*((int*)_tmp1D9)!= 
34)goto _LL208;_tmp21B=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp1D9)->f1;_LL207: s=
Cyc_Absynpp_qvar2doc(_tmp21B);goto _LL1BB;_LL208: if(*((int*)_tmp1D9)!= 35)goto
_LL20A;_tmp21C=((struct Cyc_Absyn_Malloc_e_struct*)_tmp1D9)->f1;_tmp21D=_tmp21C.is_calloc;
_tmp21E=_tmp21C.rgn;_tmp21F=_tmp21C.elt_type;_tmp220=_tmp21C.num_elts;_LL209: if(
_tmp21D){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp21F)),0);if(_tmp21E == 0){const char*_tmp72F;const char*_tmp72E;const char*
_tmp72D;struct Cyc_PP_Doc*_tmp72C[5];s=((_tmp72C[4]=Cyc_PP_text(((_tmp72F=")",
_tag_dyneither(_tmp72F,sizeof(char),2)))),((_tmp72C[3]=Cyc_Absynpp_exp2doc(st),((
_tmp72C[2]=Cyc_PP_text(((_tmp72E=",",_tag_dyneither(_tmp72E,sizeof(char),2)))),((
_tmp72C[1]=Cyc_Absynpp_exp2doc(_tmp220),((_tmp72C[0]=Cyc_PP_text(((_tmp72D="calloc(",
_tag_dyneither(_tmp72D,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp72C,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{const char*_tmp738;const char*
_tmp737;const char*_tmp736;const char*_tmp735;struct Cyc_PP_Doc*_tmp734[7];s=((
_tmp734[6]=Cyc_PP_text(((_tmp738=")",_tag_dyneither(_tmp738,sizeof(char),2)))),((
_tmp734[5]=Cyc_Absynpp_exp2doc(st),((_tmp734[4]=Cyc_PP_text(((_tmp737=",",
_tag_dyneither(_tmp737,sizeof(char),2)))),((_tmp734[3]=Cyc_Absynpp_exp2doc(
_tmp220),((_tmp734[2]=Cyc_PP_text(((_tmp736=",",_tag_dyneither(_tmp736,sizeof(
char),2)))),((_tmp734[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp21E),((
_tmp734[0]=Cyc_PP_text(((_tmp735="rcalloc(",_tag_dyneither(_tmp735,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp734,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmp21F == 0)new_e=_tmp220;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp21F,0),_tmp220,0);}if(_tmp21E == 0){const char*
_tmp73D;const char*_tmp73C;struct Cyc_PP_Doc*_tmp73B[3];s=((_tmp73B[2]=Cyc_PP_text(((
_tmp73D=")",_tag_dyneither(_tmp73D,sizeof(char),2)))),((_tmp73B[1]=Cyc_Absynpp_exp2doc(
new_e),((_tmp73B[0]=Cyc_PP_text(((_tmp73C="malloc(",_tag_dyneither(_tmp73C,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp73B,sizeof(struct Cyc_PP_Doc*),3))))))));}
else{const char*_tmp744;const char*_tmp743;const char*_tmp742;struct Cyc_PP_Doc*
_tmp741[5];s=((_tmp741[4]=Cyc_PP_text(((_tmp744=")",_tag_dyneither(_tmp744,
sizeof(char),2)))),((_tmp741[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp741[2]=Cyc_PP_text(((
_tmp743=",",_tag_dyneither(_tmp743,sizeof(char),2)))),((_tmp741[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp21E),((_tmp741[0]=Cyc_PP_text(((_tmp742="rmalloc(",
_tag_dyneither(_tmp742,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp741,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}}goto _LL1BB;_LL20A: if(*((int*)_tmp1D9)
!= 36)goto _LL20C;_tmp221=((struct Cyc_Absyn_Swap_e_struct*)_tmp1D9)->f1;_tmp222=((
struct Cyc_Absyn_Swap_e_struct*)_tmp1D9)->f2;_LL20B:{const char*_tmp74B;const char*
_tmp74A;const char*_tmp749;struct Cyc_PP_Doc*_tmp748[5];s=((_tmp748[4]=Cyc_PP_text(((
_tmp74B=")",_tag_dyneither(_tmp74B,sizeof(char),2)))),((_tmp748[3]=Cyc_Absynpp_exp2doc(
_tmp222),((_tmp748[2]=Cyc_PP_text(((_tmp74A=",",_tag_dyneither(_tmp74A,sizeof(
char),2)))),((_tmp748[1]=Cyc_Absynpp_exp2doc(_tmp221),((_tmp748[0]=Cyc_PP_text(((
_tmp749="cycswap(",_tag_dyneither(_tmp749,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp748,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1BB;
_LL20C: if(*((int*)_tmp1D9)!= 37)goto _LL20E;_tmp223=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp1D9)->f1;_tmp224=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1D9)->f2;
_LL20D:{const char*_tmp74C;s=Cyc_Absynpp_group_braces(((_tmp74C=",",
_tag_dyneither(_tmp74C,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp224));}goto _LL1BB;_LL20E: if(*((int*)_tmp1D9)!= 38)goto _LL1BB;_tmp225=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp1D9)->f1;_LL20F:{const char*_tmp751;const
char*_tmp750;struct Cyc_PP_Doc*_tmp74F[7];s=((_tmp74F[6]=Cyc_PP_text(((_tmp751=")",
_tag_dyneither(_tmp751,sizeof(char),2)))),((_tmp74F[5]=Cyc_Absynpp_rb(),((
_tmp74F[4]=Cyc_PP_blank_doc(),((_tmp74F[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp225)),((_tmp74F[2]=Cyc_PP_blank_doc(),((_tmp74F[1]=Cyc_Absynpp_lb(),((
_tmp74F[0]=Cyc_PP_text(((_tmp750="(",_tag_dyneither(_tmp750,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp74F,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
goto _LL1BB;_LL1BB:;}if(inprec >= myprec){const char*_tmp756;const char*_tmp755;
struct Cyc_PP_Doc*_tmp754[3];s=((_tmp754[2]=Cyc_PP_text(((_tmp756=")",
_tag_dyneither(_tmp756,sizeof(char),2)))),((_tmp754[1]=s,((_tmp754[0]=Cyc_PP_text(((
_tmp755="(",_tag_dyneither(_tmp755,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp754,sizeof(struct Cyc_PP_Doc*),3))))))));}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(
void*d);struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){void*_tmp2A4=d;
struct Cyc_Absyn_Exp*_tmp2A5;struct _dyneither_ptr*_tmp2A6;_LL21A: if(*((int*)
_tmp2A4)!= 0)goto _LL21C;_tmp2A5=((struct Cyc_Absyn_ArrayElement_struct*)_tmp2A4)->f1;
_LL21B: {const char*_tmp75B;const char*_tmp75A;struct Cyc_PP_Doc*_tmp759[3];return(
_tmp759[2]=Cyc_PP_text(((_tmp75B="]",_tag_dyneither(_tmp75B,sizeof(char),2)))),((
_tmp759[1]=Cyc_Absynpp_exp2doc(_tmp2A5),((_tmp759[0]=Cyc_PP_text(((_tmp75A=".[",
_tag_dyneither(_tmp75A,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp759,
sizeof(struct Cyc_PP_Doc*),3)))))));}_LL21C: if(*((int*)_tmp2A4)!= 1)goto _LL219;
_tmp2A6=((struct Cyc_Absyn_FieldName_struct*)_tmp2A4)->f1;_LL21D: {const char*
_tmp75E;struct Cyc_PP_Doc*_tmp75D[2];return(_tmp75D[1]=Cyc_PP_textptr(_tmp2A6),((
_tmp75D[0]=Cyc_PP_text(((_tmp75E=".",_tag_dyneither(_tmp75E,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp75D,sizeof(struct Cyc_PP_Doc*),2)))));}_LL219:;}
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple15*de);struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(
struct _tuple15*de){if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
const char*_tmp765;const char*_tmp764;const char*_tmp763;struct Cyc_PP_Doc*_tmp762[2];
return(_tmp762[1]=Cyc_Absynpp_exp2doc((*de).f2),((_tmp762[0]=Cyc_PP_egroup(((
_tmp765="",_tag_dyneither(_tmp765,sizeof(char),1))),((_tmp764="=",_tag_dyneither(
_tmp764,sizeof(char),2))),((_tmp763="=",_tag_dyneither(_tmp763,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp762,sizeof(
struct Cyc_PP_Doc*),2)))));}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es){const char*_tmp768;const char*_tmp767;const char*_tmp766;
return Cyc_PP_group(((_tmp766="",_tag_dyneither(_tmp766,sizeof(char),1))),((
_tmp767="",_tag_dyneither(_tmp767,sizeof(char),1))),((_tmp768=",",_tag_dyneither(
_tmp768,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){union Cyc_Absyn_Cnst _tmp2B3=
c;struct _tuple3 _tmp2B4;void*_tmp2B5;char _tmp2B6;struct _tuple4 _tmp2B7;void*
_tmp2B8;short _tmp2B9;struct _tuple5 _tmp2BA;void*_tmp2BB;int _tmp2BC;struct _tuple5
_tmp2BD;void*_tmp2BE;int _tmp2BF;struct _tuple5 _tmp2C0;void*_tmp2C1;int _tmp2C2;
struct _tuple6 _tmp2C3;void*_tmp2C4;long long _tmp2C5;struct _dyneither_ptr _tmp2C6;
int _tmp2C7;struct _dyneither_ptr _tmp2C8;_LL21F: if((_tmp2B3.Char_c).tag != 2)goto
_LL221;_tmp2B4=(struct _tuple3)(_tmp2B3.Char_c).val;_tmp2B5=_tmp2B4.f1;_tmp2B6=
_tmp2B4.f2;_LL220: {const char*_tmp76C;void*_tmp76B[1];struct Cyc_String_pa_struct
_tmp76A;return Cyc_PP_text((struct _dyneither_ptr)((_tmp76A.tag=0,((_tmp76A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp2B6)),((
_tmp76B[0]=& _tmp76A,Cyc_aprintf(((_tmp76C="'%s'",_tag_dyneither(_tmp76C,sizeof(
char),5))),_tag_dyneither(_tmp76B,sizeof(void*),1)))))))));}_LL221: if((_tmp2B3.Short_c).tag
!= 3)goto _LL223;_tmp2B7=(struct _tuple4)(_tmp2B3.Short_c).val;_tmp2B8=_tmp2B7.f1;
_tmp2B9=_tmp2B7.f2;_LL222: {const char*_tmp770;void*_tmp76F[1];struct Cyc_Int_pa_struct
_tmp76E;return Cyc_PP_text((struct _dyneither_ptr)((_tmp76E.tag=1,((_tmp76E.f1=(
unsigned long)((int)_tmp2B9),((_tmp76F[0]=& _tmp76E,Cyc_aprintf(((_tmp770="%d",
_tag_dyneither(_tmp770,sizeof(char),3))),_tag_dyneither(_tmp76F,sizeof(void*),1)))))))));}
_LL223: if((_tmp2B3.Int_c).tag != 4)goto _LL225;_tmp2BA=(struct _tuple5)(_tmp2B3.Int_c).val;
_tmp2BB=_tmp2BA.f1;if((int)_tmp2BB != 2)goto _LL225;_tmp2BC=_tmp2BA.f2;_LL224:
_tmp2BF=_tmp2BC;goto _LL226;_LL225: if((_tmp2B3.Int_c).tag != 4)goto _LL227;_tmp2BD=(
struct _tuple5)(_tmp2B3.Int_c).val;_tmp2BE=_tmp2BD.f1;if((int)_tmp2BE != 0)goto
_LL227;_tmp2BF=_tmp2BD.f2;_LL226: {const char*_tmp774;void*_tmp773[1];struct Cyc_Int_pa_struct
_tmp772;return Cyc_PP_text((struct _dyneither_ptr)((_tmp772.tag=1,((_tmp772.f1=(
unsigned long)_tmp2BF,((_tmp773[0]=& _tmp772,Cyc_aprintf(((_tmp774="%d",
_tag_dyneither(_tmp774,sizeof(char),3))),_tag_dyneither(_tmp773,sizeof(void*),1)))))))));}
_LL227: if((_tmp2B3.Int_c).tag != 4)goto _LL229;_tmp2C0=(struct _tuple5)(_tmp2B3.Int_c).val;
_tmp2C1=_tmp2C0.f1;if((int)_tmp2C1 != 1)goto _LL229;_tmp2C2=_tmp2C0.f2;_LL228: {
const char*_tmp778;void*_tmp777[1];struct Cyc_Int_pa_struct _tmp776;return Cyc_PP_text((
struct _dyneither_ptr)((_tmp776.tag=1,((_tmp776.f1=(unsigned int)_tmp2C2,((
_tmp777[0]=& _tmp776,Cyc_aprintf(((_tmp778="%u",_tag_dyneither(_tmp778,sizeof(
char),3))),_tag_dyneither(_tmp777,sizeof(void*),1)))))))));}_LL229: if((_tmp2B3.LongLong_c).tag
!= 5)goto _LL22B;_tmp2C3=(struct _tuple6)(_tmp2B3.LongLong_c).val;_tmp2C4=_tmp2C3.f1;
_tmp2C5=_tmp2C3.f2;_LL22A: {const char*_tmp779;return Cyc_PP_text(((_tmp779="<<FIX LONG LONG CONSTANT>>",
_tag_dyneither(_tmp779,sizeof(char),27))));}_LL22B: if((_tmp2B3.Float_c).tag != 6)
goto _LL22D;_tmp2C6=(struct _dyneither_ptr)(_tmp2B3.Float_c).val;_LL22C: return Cyc_PP_text(
_tmp2C6);_LL22D: if((_tmp2B3.Null_c).tag != 1)goto _LL22F;_tmp2C7=(int)(_tmp2B3.Null_c).val;
_LL22E: {const char*_tmp77A;return Cyc_PP_text(((_tmp77A="NULL",_tag_dyneither(
_tmp77A,sizeof(char),5))));}_LL22F: if((_tmp2B3.String_c).tag != 7)goto _LL21E;
_tmp2C8=(struct _dyneither_ptr)(_tmp2B3.String_c).val;_LL230: {const char*_tmp77F;
const char*_tmp77E;struct Cyc_PP_Doc*_tmp77D[3];return(_tmp77D[2]=Cyc_PP_text(((
_tmp77F="\"",_tag_dyneither(_tmp77F,sizeof(char),2)))),((_tmp77D[1]=Cyc_PP_text(
Cyc_Absynpp_string_escape(_tmp2C8)),((_tmp77D[0]=Cyc_PP_text(((_tmp77E="\"",
_tag_dyneither(_tmp77E,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp77D,
sizeof(struct Cyc_PP_Doc*),3)))))));}_LL21E:;}struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(
int inprec,void*p,struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(
int inprec,void*p,struct Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(
p);if(p == (void*)((void*)19)){if(es == 0  || es->tl != 0){struct Cyc_Core_Failure_struct
_tmp78C;const char*_tmp78B;void*_tmp78A[1];struct Cyc_String_pa_struct _tmp789;
struct Cyc_Core_Failure_struct*_tmp788;(int)_throw((void*)((_tmp788=_cycalloc(
sizeof(*_tmp788)),((_tmp788[0]=((_tmp78C.tag=Cyc_Core_Failure,((_tmp78C.f1=(
struct _dyneither_ptr)((_tmp789.tag=0,((_tmp789.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp78A[0]=& _tmp789,Cyc_aprintf(((
_tmp78B="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp78B,
sizeof(char),47))),_tag_dyneither(_tmp78A,sizeof(void*),1)))))))),_tmp78C)))),
_tmp788)))));}{const char*_tmp791;const char*_tmp790;struct Cyc_PP_Doc*_tmp78F[3];
return(_tmp78F[2]=Cyc_PP_text(((_tmp791=")",_tag_dyneither(_tmp791,sizeof(char),
2)))),((_tmp78F[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp78F[0]=
Cyc_PP_text(((_tmp790="numelts(",_tag_dyneither(_tmp790,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp78F,sizeof(struct Cyc_PP_Doc*),3)))))));}}else{struct Cyc_List_List*
ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){struct Cyc_Core_Failure_struct _tmp79E;const char*_tmp79D;void*_tmp79C[1];
struct Cyc_String_pa_struct _tmp79B;struct Cyc_Core_Failure_struct*_tmp79A;(int)
_throw((void*)((_tmp79A=_cycalloc(sizeof(*_tmp79A)),((_tmp79A[0]=((_tmp79E.tag=
Cyc_Core_Failure,((_tmp79E.f1=(struct _dyneither_ptr)((_tmp79B.tag=0,((_tmp79B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((
_tmp79C[0]=& _tmp79B,Cyc_aprintf(((_tmp79D="Absynpp::primapp2doc: %s with no args",
_tag_dyneither(_tmp79D,sizeof(char),38))),_tag_dyneither(_tmp79C,sizeof(void*),1)))))))),
_tmp79E)))),_tmp79A)))));}else{if(ds->tl == 0){const char*_tmp7A1;struct Cyc_PP_Doc*
_tmp7A0[3];return(_tmp7A0[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp7A0[1]=Cyc_PP_text(((
_tmp7A1=" ",_tag_dyneither(_tmp7A1,sizeof(char),2)))),((_tmp7A0[0]=ps,Cyc_PP_cat(
_tag_dyneither(_tmp7A0,sizeof(struct Cyc_PP_Doc*),3)))))));}else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0){struct Cyc_Core_Failure_struct _tmp7AE;const char*
_tmp7AD;void*_tmp7AC[1];struct Cyc_String_pa_struct _tmp7AB;struct Cyc_Core_Failure_struct*
_tmp7AA;(int)_throw((void*)((_tmp7AA=_cycalloc(sizeof(*_tmp7AA)),((_tmp7AA[0]=((
_tmp7AE.tag=Cyc_Core_Failure,((_tmp7AE.f1=(struct _dyneither_ptr)((_tmp7AB.tag=0,((
_tmp7AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp7AC[0]=& _tmp7AB,Cyc_aprintf(((_tmp7AD="Absynpp::primapp2doc: %s with more than 2 args",
_tag_dyneither(_tmp7AD,sizeof(char),47))),_tag_dyneither(_tmp7AC,sizeof(void*),1)))))))),
_tmp7AE)))),_tmp7AA)))));}else{const char*_tmp7B3;const char*_tmp7B2;struct Cyc_PP_Doc*
_tmp7B1[5];return(_tmp7B1[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd,((_tmp7B1[3]=Cyc_PP_text(((_tmp7B3=" ",_tag_dyneither(
_tmp7B3,sizeof(char),2)))),((_tmp7B1[2]=ps,((_tmp7B1[1]=Cyc_PP_text(((_tmp7B2=" ",
_tag_dyneither(_tmp7B2,sizeof(char),2)))),((_tmp7B1[0]=(struct Cyc_PP_Doc*)ds->hd,
Cyc_PP_cat(_tag_dyneither(_tmp7B1,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
struct _dyneither_ptr Cyc_Absynpp_prim2str(void*p);struct _dyneither_ptr Cyc_Absynpp_prim2str(
void*p){void*_tmp2F1=p;_LL232: if((int)_tmp2F1 != 0)goto _LL234;_LL233: {const char*
_tmp7B4;return(_tmp7B4="+",_tag_dyneither(_tmp7B4,sizeof(char),2));}_LL234: if((
int)_tmp2F1 != 1)goto _LL236;_LL235: {const char*_tmp7B5;return(_tmp7B5="*",
_tag_dyneither(_tmp7B5,sizeof(char),2));}_LL236: if((int)_tmp2F1 != 2)goto _LL238;
_LL237: {const char*_tmp7B6;return(_tmp7B6="-",_tag_dyneither(_tmp7B6,sizeof(char),
2));}_LL238: if((int)_tmp2F1 != 3)goto _LL23A;_LL239: {const char*_tmp7B7;return(
_tmp7B7="/",_tag_dyneither(_tmp7B7,sizeof(char),2));}_LL23A: if((int)_tmp2F1 != 4)
goto _LL23C;_LL23B: {const char*_tmp7B9;const char*_tmp7B8;return Cyc_Absynpp_print_for_cycdoc?(
_tmp7B9="\\%",_tag_dyneither(_tmp7B9,sizeof(char),3)):((_tmp7B8="%",
_tag_dyneither(_tmp7B8,sizeof(char),2)));}_LL23C: if((int)_tmp2F1 != 5)goto _LL23E;
_LL23D: {const char*_tmp7BA;return(_tmp7BA="==",_tag_dyneither(_tmp7BA,sizeof(
char),3));}_LL23E: if((int)_tmp2F1 != 6)goto _LL240;_LL23F: {const char*_tmp7BB;
return(_tmp7BB="!=",_tag_dyneither(_tmp7BB,sizeof(char),3));}_LL240: if((int)
_tmp2F1 != 7)goto _LL242;_LL241: {const char*_tmp7BC;return(_tmp7BC=">",
_tag_dyneither(_tmp7BC,sizeof(char),2));}_LL242: if((int)_tmp2F1 != 8)goto _LL244;
_LL243: {const char*_tmp7BD;return(_tmp7BD="<",_tag_dyneither(_tmp7BD,sizeof(char),
2));}_LL244: if((int)_tmp2F1 != 9)goto _LL246;_LL245: {const char*_tmp7BE;return(
_tmp7BE=">=",_tag_dyneither(_tmp7BE,sizeof(char),3));}_LL246: if((int)_tmp2F1 != 
10)goto _LL248;_LL247: {const char*_tmp7BF;return(_tmp7BF="<=",_tag_dyneither(
_tmp7BF,sizeof(char),3));}_LL248: if((int)_tmp2F1 != 11)goto _LL24A;_LL249: {const
char*_tmp7C0;return(_tmp7C0="!",_tag_dyneither(_tmp7C0,sizeof(char),2));}_LL24A:
if((int)_tmp2F1 != 12)goto _LL24C;_LL24B: {const char*_tmp7C1;return(_tmp7C1="~",
_tag_dyneither(_tmp7C1,sizeof(char),2));}_LL24C: if((int)_tmp2F1 != 13)goto _LL24E;
_LL24D: {const char*_tmp7C2;return(_tmp7C2="&",_tag_dyneither(_tmp7C2,sizeof(char),
2));}_LL24E: if((int)_tmp2F1 != 14)goto _LL250;_LL24F: {const char*_tmp7C3;return(
_tmp7C3="|",_tag_dyneither(_tmp7C3,sizeof(char),2));}_LL250: if((int)_tmp2F1 != 15)
goto _LL252;_LL251: {const char*_tmp7C4;return(_tmp7C4="^",_tag_dyneither(_tmp7C4,
sizeof(char),2));}_LL252: if((int)_tmp2F1 != 16)goto _LL254;_LL253: {const char*
_tmp7C5;return(_tmp7C5="<<",_tag_dyneither(_tmp7C5,sizeof(char),3));}_LL254: if((
int)_tmp2F1 != 17)goto _LL256;_LL255: {const char*_tmp7C6;return(_tmp7C6=">>",
_tag_dyneither(_tmp7C6,sizeof(char),3));}_LL256: if((int)_tmp2F1 != 18)goto _LL258;
_LL257: {const char*_tmp7C7;return(_tmp7C7=">>>",_tag_dyneither(_tmp7C7,sizeof(
char),4));}_LL258: if((int)_tmp2F1 != 19)goto _LL231;_LL259: {const char*_tmp7C8;
return(_tmp7C8="numelts",_tag_dyneither(_tmp7C8,sizeof(char),8));}_LL231:;}
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*p);struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
void*p){return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s);int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp307=s->r;_LL25B: if(_tmp307 <= (void*)1)goto _LL25D;if(*((int*)_tmp307)!= 
11)goto _LL25D;_LL25C: return 1;_LL25D:;_LL25E: return 0;_LL25A:;}struct Cyc_PP_Doc*
Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st){struct Cyc_PP_Doc*s;{void*_tmp308=st->r;struct Cyc_Absyn_Exp*
_tmp309;struct Cyc_Absyn_Stmt*_tmp30A;struct Cyc_Absyn_Stmt*_tmp30B;struct Cyc_Absyn_Exp*
_tmp30C;struct Cyc_Absyn_Exp*_tmp30D;struct Cyc_Absyn_Stmt*_tmp30E;struct Cyc_Absyn_Stmt*
_tmp30F;struct _tuple8 _tmp310;struct Cyc_Absyn_Exp*_tmp311;struct Cyc_Absyn_Stmt*
_tmp312;struct _dyneither_ptr*_tmp313;struct Cyc_Absyn_Exp*_tmp314;struct _tuple8
_tmp315;struct Cyc_Absyn_Exp*_tmp316;struct _tuple8 _tmp317;struct Cyc_Absyn_Exp*
_tmp318;struct Cyc_Absyn_Stmt*_tmp319;struct Cyc_Absyn_Exp*_tmp31A;struct Cyc_List_List*
_tmp31B;struct Cyc_List_List*_tmp31C;struct Cyc_List_List*_tmp31D;struct Cyc_Absyn_Decl*
_tmp31E;struct Cyc_Absyn_Stmt*_tmp31F;struct _dyneither_ptr*_tmp320;struct Cyc_Absyn_Stmt*
_tmp321;struct Cyc_Absyn_Stmt*_tmp322;struct _tuple8 _tmp323;struct Cyc_Absyn_Exp*
_tmp324;struct Cyc_Absyn_Stmt*_tmp325;struct Cyc_List_List*_tmp326;struct Cyc_Absyn_Exp*
_tmp327;_LL260: if((int)_tmp308 != 0)goto _LL262;_LL261:{const char*_tmp7C9;s=Cyc_PP_text(((
_tmp7C9=";",_tag_dyneither(_tmp7C9,sizeof(char),2))));}goto _LL25F;_LL262: if(
_tmp308 <= (void*)1)goto _LL264;if(*((int*)_tmp308)!= 0)goto _LL264;_tmp309=((
struct Cyc_Absyn_Exp_s_struct*)_tmp308)->f1;_LL263:{const char*_tmp7CC;struct Cyc_PP_Doc*
_tmp7CB[2];s=((_tmp7CB[1]=Cyc_PP_text(((_tmp7CC=";",_tag_dyneither(_tmp7CC,
sizeof(char),2)))),((_tmp7CB[0]=Cyc_Absynpp_exp2doc(_tmp309),Cyc_PP_cat(
_tag_dyneither(_tmp7CB,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL25F;_LL264: if(
_tmp308 <= (void*)1)goto _LL266;if(*((int*)_tmp308)!= 1)goto _LL266;_tmp30A=((
struct Cyc_Absyn_Seq_s_struct*)_tmp308)->f1;_tmp30B=((struct Cyc_Absyn_Seq_s_struct*)
_tmp308)->f2;_LL265: if(Cyc_Absynpp_decls_first){if(Cyc_Absynpp_is_declaration(
_tmp30A)){struct Cyc_PP_Doc*_tmp7CF[5];struct Cyc_PP_Doc*_tmp7CE[7];s=((_tmp7CE[6]=
Cyc_Absynpp_is_declaration(_tmp30B)?(_tmp7CF[4]=Cyc_PP_line_doc(),((_tmp7CF[3]=
Cyc_Absynpp_rb(),((_tmp7CF[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp30B)),((
_tmp7CF[1]=Cyc_PP_blank_doc(),((_tmp7CF[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp7CF,sizeof(struct Cyc_PP_Doc*),5))))))))))): Cyc_Absynpp_stmt2doc(
_tmp30B),((_tmp7CE[5]=Cyc_PP_line_doc(),((_tmp7CE[4]=Cyc_Absynpp_rb(),((_tmp7CE[
3]=Cyc_PP_line_doc(),((_tmp7CE[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp30A)),((
_tmp7CE[1]=Cyc_PP_blank_doc(),((_tmp7CE[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp7CE,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{if(Cyc_Absynpp_is_declaration(
_tmp30B)){struct Cyc_PP_Doc*_tmp7D0[7];s=((_tmp7D0[6]=Cyc_PP_line_doc(),((_tmp7D0[
5]=Cyc_Absynpp_rb(),((_tmp7D0[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp30B)),((
_tmp7D0[3]=Cyc_PP_blank_doc(),((_tmp7D0[2]=Cyc_Absynpp_lb(),((_tmp7D0[1]=Cyc_PP_line_doc(),((
_tmp7D0[0]=Cyc_Absynpp_stmt2doc(_tmp30A),Cyc_PP_cat(_tag_dyneither(_tmp7D0,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{struct Cyc_List_List*_tmp7D4;
struct Cyc_List_List*_tmp7D3;const char*_tmp7D1;s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Stmt*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,((_tmp7D1="",_tag_dyneither(_tmp7D1,sizeof(char),1))),((
_tmp7D3=_cycalloc(sizeof(*_tmp7D3)),((_tmp7D3->hd=_tmp30A,((_tmp7D3->tl=((
_tmp7D4=_cycalloc(sizeof(*_tmp7D4)),((_tmp7D4->hd=_tmp30B,((_tmp7D4->tl=0,
_tmp7D4)))))),_tmp7D3)))))));}}}else{struct Cyc_List_List*_tmp7D8;struct Cyc_List_List*
_tmp7D7;const char*_tmp7D5;s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct
Cyc_Absyn_Stmt*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,((_tmp7D5="",_tag_dyneither(_tmp7D5,sizeof(char),1))),((
_tmp7D7=_cycalloc(sizeof(*_tmp7D7)),((_tmp7D7->hd=_tmp30A,((_tmp7D7->tl=((
_tmp7D8=_cycalloc(sizeof(*_tmp7D8)),((_tmp7D8->hd=_tmp30B,((_tmp7D8->tl=0,
_tmp7D8)))))),_tmp7D7)))))));}goto _LL25F;_LL266: if(_tmp308 <= (void*)1)goto _LL268;
if(*((int*)_tmp308)!= 2)goto _LL268;_tmp30C=((struct Cyc_Absyn_Return_s_struct*)
_tmp308)->f1;_LL267: if(_tmp30C == 0){const char*_tmp7D9;s=Cyc_PP_text(((_tmp7D9="return;",
_tag_dyneither(_tmp7D9,sizeof(char),8))));}else{const char*_tmp7DE;const char*
_tmp7DD;struct Cyc_PP_Doc*_tmp7DC[3];s=((_tmp7DC[2]=Cyc_PP_text(((_tmp7DE=";",
_tag_dyneither(_tmp7DE,sizeof(char),2)))),((_tmp7DC[1]=_tmp30C == 0?Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp30C),((_tmp7DC[0]=Cyc_PP_text(((
_tmp7DD="return ",_tag_dyneither(_tmp7DD,sizeof(char),8)))),Cyc_PP_cat(
_tag_dyneither(_tmp7DC,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL25F;_LL268:
if(_tmp308 <= (void*)1)goto _LL26A;if(*((int*)_tmp308)!= 3)goto _LL26A;_tmp30D=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp308)->f1;_tmp30E=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp308)->f2;_tmp30F=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp308)->f3;_LL269: {
int print_else;{void*_tmp338=_tmp30F->r;_LL285: if((int)_tmp338 != 0)goto _LL287;
_LL286: print_else=0;goto _LL284;_LL287:;_LL288: print_else=1;goto _LL284;_LL284:;}{
struct Cyc_PP_Doc*_tmp7ED[6];const char*_tmp7EC;struct Cyc_PP_Doc*_tmp7EB[2];struct
Cyc_PP_Doc*_tmp7EA[2];const char*_tmp7E9;const char*_tmp7E8;struct Cyc_PP_Doc*
_tmp7E7[8];s=((_tmp7E7[7]=print_else?(_tmp7ED[5]=Cyc_Absynpp_rb(),((_tmp7ED[4]=
Cyc_PP_line_doc(),((_tmp7ED[3]=Cyc_PP_nest(2,((_tmp7EB[1]=Cyc_Absynpp_stmt2doc(
_tmp30F),((_tmp7EB[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp7EB,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp7ED[2]=Cyc_Absynpp_lb(),((_tmp7ED[1]=
Cyc_PP_text(((_tmp7EC="else ",_tag_dyneither(_tmp7EC,sizeof(char),6)))),((
_tmp7ED[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp7ED,sizeof(struct Cyc_PP_Doc*),
6))))))))))))): Cyc_PP_nil_doc(),((_tmp7E7[6]=Cyc_Absynpp_rb(),((_tmp7E7[5]=Cyc_PP_line_doc(),((
_tmp7E7[4]=Cyc_PP_nest(2,((_tmp7EA[1]=Cyc_Absynpp_stmt2doc(_tmp30E),((_tmp7EA[0]=
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp7EA,sizeof(struct Cyc_PP_Doc*),2))))))),((
_tmp7E7[3]=Cyc_Absynpp_lb(),((_tmp7E7[2]=Cyc_PP_text(((_tmp7E9=") ",
_tag_dyneither(_tmp7E9,sizeof(char),3)))),((_tmp7E7[1]=Cyc_Absynpp_exp2doc(
_tmp30D),((_tmp7E7[0]=Cyc_PP_text(((_tmp7E8="if (",_tag_dyneither(_tmp7E8,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp7E7,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL25F;}_LL26A: if(_tmp308 <= (void*)1)goto _LL26C;if(*((int*)_tmp308)!= 4)goto
_LL26C;_tmp310=((struct Cyc_Absyn_While_s_struct*)_tmp308)->f1;_tmp311=_tmp310.f1;
_tmp312=((struct Cyc_Absyn_While_s_struct*)_tmp308)->f2;_LL26B:{struct Cyc_PP_Doc*
_tmp7F4[2];const char*_tmp7F3;const char*_tmp7F2;struct Cyc_PP_Doc*_tmp7F1[7];s=((
_tmp7F1[6]=Cyc_Absynpp_rb(),((_tmp7F1[5]=Cyc_PP_line_doc(),((_tmp7F1[4]=Cyc_PP_nest(
2,((_tmp7F4[1]=Cyc_Absynpp_stmt2doc(_tmp312),((_tmp7F4[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp7F4,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp7F1[3]=Cyc_Absynpp_lb(),((
_tmp7F1[2]=Cyc_PP_text(((_tmp7F3=") ",_tag_dyneither(_tmp7F3,sizeof(char),3)))),((
_tmp7F1[1]=Cyc_Absynpp_exp2doc(_tmp311),((_tmp7F1[0]=Cyc_PP_text(((_tmp7F2="while (",
_tag_dyneither(_tmp7F2,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7F1,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL25F;_LL26C: if(_tmp308 <= (
void*)1)goto _LL26E;if(*((int*)_tmp308)!= 5)goto _LL26E;_LL26D:{const char*_tmp7F5;
s=Cyc_PP_text(((_tmp7F5="break;",_tag_dyneither(_tmp7F5,sizeof(char),7))));}goto
_LL25F;_LL26E: if(_tmp308 <= (void*)1)goto _LL270;if(*((int*)_tmp308)!= 6)goto
_LL270;_LL26F:{const char*_tmp7F6;s=Cyc_PP_text(((_tmp7F6="continue;",
_tag_dyneither(_tmp7F6,sizeof(char),10))));}goto _LL25F;_LL270: if(_tmp308 <= (void*)
1)goto _LL272;if(*((int*)_tmp308)!= 7)goto _LL272;_tmp313=((struct Cyc_Absyn_Goto_s_struct*)
_tmp308)->f1;_LL271:{const char*_tmp7FA;void*_tmp7F9[1];struct Cyc_String_pa_struct
_tmp7F8;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp7F8.tag=0,((_tmp7F8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp313),((_tmp7F9[0]=& _tmp7F8,Cyc_aprintf(((
_tmp7FA="goto %s;",_tag_dyneither(_tmp7FA,sizeof(char),9))),_tag_dyneither(
_tmp7F9,sizeof(void*),1)))))))));}goto _LL25F;_LL272: if(_tmp308 <= (void*)1)goto
_LL274;if(*((int*)_tmp308)!= 8)goto _LL274;_tmp314=((struct Cyc_Absyn_For_s_struct*)
_tmp308)->f1;_tmp315=((struct Cyc_Absyn_For_s_struct*)_tmp308)->f2;_tmp316=
_tmp315.f1;_tmp317=((struct Cyc_Absyn_For_s_struct*)_tmp308)->f3;_tmp318=_tmp317.f1;
_tmp319=((struct Cyc_Absyn_For_s_struct*)_tmp308)->f4;_LL273:{struct Cyc_PP_Doc*
_tmp805[2];const char*_tmp804;const char*_tmp803;const char*_tmp802;const char*
_tmp801;struct Cyc_PP_Doc*_tmp800[11];s=((_tmp800[10]=Cyc_Absynpp_rb(),((_tmp800[
9]=Cyc_PP_line_doc(),((_tmp800[8]=Cyc_PP_nest(2,((_tmp805[1]=Cyc_Absynpp_stmt2doc(
_tmp319),((_tmp805[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp805,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp800[7]=Cyc_Absynpp_lb(),((_tmp800[6]=
Cyc_PP_text(((_tmp804=") ",_tag_dyneither(_tmp804,sizeof(char),3)))),((_tmp800[5]=
Cyc_Absynpp_exp2doc(_tmp318),((_tmp800[4]=Cyc_PP_text(((_tmp803="; ",
_tag_dyneither(_tmp803,sizeof(char),3)))),((_tmp800[3]=Cyc_Absynpp_exp2doc(
_tmp316),((_tmp800[2]=Cyc_PP_text(((_tmp802="; ",_tag_dyneither(_tmp802,sizeof(
char),3)))),((_tmp800[1]=Cyc_Absynpp_exp2doc(_tmp314),((_tmp800[0]=Cyc_PP_text(((
_tmp801="for(",_tag_dyneither(_tmp801,sizeof(char),5)))),Cyc_PP_cat(
_tag_dyneither(_tmp800,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL25F;_LL274: if(_tmp308 <= (void*)1)goto _LL276;if(*((int*)_tmp308)!= 9)goto
_LL276;_tmp31A=((struct Cyc_Absyn_Switch_s_struct*)_tmp308)->f1;_tmp31B=((struct
Cyc_Absyn_Switch_s_struct*)_tmp308)->f2;_LL275:{const char*_tmp80A;const char*
_tmp809;struct Cyc_PP_Doc*_tmp808[8];s=((_tmp808[7]=Cyc_Absynpp_rb(),((_tmp808[6]=
Cyc_PP_line_doc(),((_tmp808[5]=Cyc_Absynpp_switchclauses2doc(_tmp31B),((_tmp808[
4]=Cyc_PP_line_doc(),((_tmp808[3]=Cyc_Absynpp_lb(),((_tmp808[2]=Cyc_PP_text(((
_tmp80A=") ",_tag_dyneither(_tmp80A,sizeof(char),3)))),((_tmp808[1]=Cyc_Absynpp_exp2doc(
_tmp31A),((_tmp808[0]=Cyc_PP_text(((_tmp809="switch (",_tag_dyneither(_tmp809,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp808,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL25F;_LL276: if(_tmp308 <= (void*)1)goto _LL278;if(*((int*)_tmp308)!= 10)goto
_LL278;_tmp31C=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp308)->f1;if(_tmp31C != 0)
goto _LL278;_LL277:{const char*_tmp80B;s=Cyc_PP_text(((_tmp80B="fallthru;",
_tag_dyneither(_tmp80B,sizeof(char),10))));}goto _LL25F;_LL278: if(_tmp308 <= (void*)
1)goto _LL27A;if(*((int*)_tmp308)!= 10)goto _LL27A;_tmp31D=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp308)->f1;_LL279:{const char*_tmp810;const char*_tmp80F;struct Cyc_PP_Doc*
_tmp80E[3];s=((_tmp80E[2]=Cyc_PP_text(((_tmp810=");",_tag_dyneither(_tmp810,
sizeof(char),3)))),((_tmp80E[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp31D),((_tmp80E[
0]=Cyc_PP_text(((_tmp80F="fallthru(",_tag_dyneither(_tmp80F,sizeof(char),10)))),
Cyc_PP_cat(_tag_dyneither(_tmp80E,sizeof(struct Cyc_PP_Doc*),3))))))));}goto
_LL25F;_LL27A: if(_tmp308 <= (void*)1)goto _LL27C;if(*((int*)_tmp308)!= 11)goto
_LL27C;_tmp31E=((struct Cyc_Absyn_Decl_s_struct*)_tmp308)->f1;_tmp31F=((struct Cyc_Absyn_Decl_s_struct*)
_tmp308)->f2;_LL27B:{struct Cyc_PP_Doc*_tmp811[3];s=((_tmp811[2]=Cyc_Absynpp_stmt2doc(
_tmp31F),((_tmp811[1]=Cyc_PP_line_doc(),((_tmp811[0]=Cyc_Absynpp_decl2doc(
_tmp31E),Cyc_PP_cat(_tag_dyneither(_tmp811,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL25F;_LL27C: if(_tmp308 <= (void*)1)goto _LL27E;if(*((int*)_tmp308)!= 12)goto
_LL27E;_tmp320=((struct Cyc_Absyn_Label_s_struct*)_tmp308)->f1;_tmp321=((struct
Cyc_Absyn_Label_s_struct*)_tmp308)->f2;_LL27D: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(
_tmp321)){const char*_tmp814;struct Cyc_PP_Doc*_tmp813[7];s=((_tmp813[6]=Cyc_Absynpp_rb(),((
_tmp813[5]=Cyc_PP_line_doc(),((_tmp813[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp321)),((_tmp813[3]=Cyc_PP_line_doc(),((_tmp813[2]=Cyc_Absynpp_lb(),((_tmp813[
1]=Cyc_PP_text(((_tmp814=": ",_tag_dyneither(_tmp814,sizeof(char),3)))),((
_tmp813[0]=Cyc_PP_textptr(_tmp320),Cyc_PP_cat(_tag_dyneither(_tmp813,sizeof(
struct Cyc_PP_Doc*),7))))))))))))))));}else{const char*_tmp817;struct Cyc_PP_Doc*
_tmp816[3];s=((_tmp816[2]=Cyc_Absynpp_stmt2doc(_tmp321),((_tmp816[1]=Cyc_PP_text(((
_tmp817=": ",_tag_dyneither(_tmp817,sizeof(char),3)))),((_tmp816[0]=Cyc_PP_textptr(
_tmp320),Cyc_PP_cat(_tag_dyneither(_tmp816,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL25F;_LL27E: if(_tmp308 <= (void*)1)goto _LL280;if(*((int*)_tmp308)!= 13)goto
_LL280;_tmp322=((struct Cyc_Absyn_Do_s_struct*)_tmp308)->f1;_tmp323=((struct Cyc_Absyn_Do_s_struct*)
_tmp308)->f2;_tmp324=_tmp323.f1;_LL27F:{const char*_tmp81E;const char*_tmp81D;
const char*_tmp81C;struct Cyc_PP_Doc*_tmp81B[9];s=((_tmp81B[8]=Cyc_PP_text(((
_tmp81E=");",_tag_dyneither(_tmp81E,sizeof(char),3)))),((_tmp81B[7]=Cyc_Absynpp_exp2doc(
_tmp324),((_tmp81B[6]=Cyc_PP_text(((_tmp81D=" while (",_tag_dyneither(_tmp81D,
sizeof(char),9)))),((_tmp81B[5]=Cyc_Absynpp_rb(),((_tmp81B[4]=Cyc_PP_line_doc(),((
_tmp81B[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp322)),((_tmp81B[2]=Cyc_PP_line_doc(),((
_tmp81B[1]=Cyc_Absynpp_lb(),((_tmp81B[0]=Cyc_PP_text(((_tmp81C="do ",
_tag_dyneither(_tmp81C,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp81B,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL25F;_LL280: if(_tmp308 <= (
void*)1)goto _LL282;if(*((int*)_tmp308)!= 14)goto _LL282;_tmp325=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp308)->f1;_tmp326=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp308)->f2;_LL281:{
const char*_tmp823;const char*_tmp822;struct Cyc_PP_Doc*_tmp821[12];s=((_tmp821[11]=
Cyc_Absynpp_rb(),((_tmp821[10]=Cyc_PP_line_doc(),((_tmp821[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp326)),((_tmp821[8]=Cyc_PP_line_doc(),((_tmp821[7]=Cyc_Absynpp_lb(),((_tmp821[
6]=Cyc_PP_text(((_tmp823=" catch ",_tag_dyneither(_tmp823,sizeof(char),8)))),((
_tmp821[5]=Cyc_Absynpp_rb(),((_tmp821[4]=Cyc_PP_line_doc(),((_tmp821[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp325)),((_tmp821[2]=Cyc_PP_line_doc(),((_tmp821[1]=Cyc_Absynpp_lb(),((
_tmp821[0]=Cyc_PP_text(((_tmp822="try ",_tag_dyneither(_tmp822,sizeof(char),5)))),
Cyc_PP_cat(_tag_dyneither(_tmp821,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
goto _LL25F;_LL282: if(_tmp308 <= (void*)1)goto _LL25F;if(*((int*)_tmp308)!= 15)goto
_LL25F;_tmp327=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp308)->f1;_LL283:{
const char*_tmp828;const char*_tmp827;struct Cyc_PP_Doc*_tmp826[3];s=((_tmp826[2]=
Cyc_PP_text(((_tmp828=");",_tag_dyneither(_tmp828,sizeof(char),3)))),((_tmp826[1]=
Cyc_Absynpp_exp2doc(_tmp327),((_tmp826[0]=Cyc_PP_text(((_tmp827="reset_region(",
_tag_dyneither(_tmp827,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp826,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL25F;_LL25F:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p);struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{void*_tmp365=p->r;void*_tmp366;int
_tmp367;char _tmp368;struct _dyneither_ptr _tmp369;struct Cyc_Absyn_Vardecl*_tmp36A;
struct Cyc_Absyn_Pat*_tmp36B;struct Cyc_Absyn_Pat _tmp36C;void*_tmp36D;struct Cyc_Absyn_Vardecl*
_tmp36E;struct Cyc_Absyn_Pat*_tmp36F;struct Cyc_Absyn_Tvar*_tmp370;struct Cyc_Absyn_Vardecl*
_tmp371;struct Cyc_List_List*_tmp372;int _tmp373;struct Cyc_Absyn_Pat*_tmp374;
struct Cyc_Absyn_Vardecl*_tmp375;struct Cyc_Absyn_Pat*_tmp376;struct Cyc_Absyn_Pat
_tmp377;void*_tmp378;struct Cyc_Absyn_Vardecl*_tmp379;struct Cyc_Absyn_Pat*_tmp37A;
struct _tuple0*_tmp37B;struct _tuple0*_tmp37C;struct Cyc_List_List*_tmp37D;int
_tmp37E;struct Cyc_Absyn_AggrInfo*_tmp37F;struct Cyc_Absyn_AggrInfo _tmp380;union
Cyc_Absyn_AggrInfoU _tmp381;struct Cyc_List_List*_tmp382;struct Cyc_List_List*
_tmp383;int _tmp384;struct Cyc_Absyn_AggrInfo*_tmp385;struct Cyc_List_List*_tmp386;
struct Cyc_List_List*_tmp387;int _tmp388;struct Cyc_Absyn_Enumfield*_tmp389;struct
Cyc_Absyn_Enumfield*_tmp38A;struct Cyc_Absyn_Datatypefield*_tmp38B;struct Cyc_List_List*
_tmp38C;struct Cyc_Absyn_Datatypefield*_tmp38D;struct Cyc_List_List*_tmp38E;int
_tmp38F;struct Cyc_Absyn_Exp*_tmp390;_LL28A: if((int)_tmp365 != 0)goto _LL28C;_LL28B:{
const char*_tmp829;s=Cyc_PP_text(((_tmp829="_",_tag_dyneither(_tmp829,sizeof(char),
2))));}goto _LL289;_LL28C: if((int)_tmp365 != 1)goto _LL28E;_LL28D:{const char*
_tmp82A;s=Cyc_PP_text(((_tmp82A="NULL",_tag_dyneither(_tmp82A,sizeof(char),5))));}
goto _LL289;_LL28E: if(_tmp365 <= (void*)2)goto _LL290;if(*((int*)_tmp365)!= 7)goto
_LL290;_tmp366=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp365)->f1;_tmp367=((
struct Cyc_Absyn_Int_p_struct*)_tmp365)->f2;_LL28F: if(_tmp366 != (void*)((void*)1)){
const char*_tmp82E;void*_tmp82D[1];struct Cyc_Int_pa_struct _tmp82C;s=Cyc_PP_text((
struct _dyneither_ptr)((_tmp82C.tag=1,((_tmp82C.f1=(unsigned long)_tmp367,((
_tmp82D[0]=& _tmp82C,Cyc_aprintf(((_tmp82E="%d",_tag_dyneither(_tmp82E,sizeof(
char),3))),_tag_dyneither(_tmp82D,sizeof(void*),1)))))))));}else{const char*
_tmp832;void*_tmp831[1];struct Cyc_Int_pa_struct _tmp830;s=Cyc_PP_text((struct
_dyneither_ptr)((_tmp830.tag=1,((_tmp830.f1=(unsigned int)_tmp367,((_tmp831[0]=&
_tmp830,Cyc_aprintf(((_tmp832="%u",_tag_dyneither(_tmp832,sizeof(char),3))),
_tag_dyneither(_tmp831,sizeof(void*),1)))))))));}goto _LL289;_LL290: if(_tmp365 <= (
void*)2)goto _LL292;if(*((int*)_tmp365)!= 8)goto _LL292;_tmp368=((struct Cyc_Absyn_Char_p_struct*)
_tmp365)->f1;_LL291:{const char*_tmp836;void*_tmp835[1];struct Cyc_String_pa_struct
_tmp834;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp834.tag=0,((_tmp834.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp368)),((
_tmp835[0]=& _tmp834,Cyc_aprintf(((_tmp836="'%s'",_tag_dyneither(_tmp836,sizeof(
char),5))),_tag_dyneither(_tmp835,sizeof(void*),1)))))))));}goto _LL289;_LL292:
if(_tmp365 <= (void*)2)goto _LL294;if(*((int*)_tmp365)!= 9)goto _LL294;_tmp369=((
struct Cyc_Absyn_Float_p_struct*)_tmp365)->f1;_LL293: s=Cyc_PP_text(_tmp369);goto
_LL289;_LL294: if(_tmp365 <= (void*)2)goto _LL296;if(*((int*)_tmp365)!= 0)goto
_LL296;_tmp36A=((struct Cyc_Absyn_Var_p_struct*)_tmp365)->f1;_tmp36B=((struct Cyc_Absyn_Var_p_struct*)
_tmp365)->f2;_tmp36C=*_tmp36B;_tmp36D=_tmp36C.r;if((int)_tmp36D != 0)goto _LL296;
_LL295: s=Cyc_Absynpp_qvar2doc(_tmp36A->name);goto _LL289;_LL296: if(_tmp365 <= (
void*)2)goto _LL298;if(*((int*)_tmp365)!= 0)goto _LL298;_tmp36E=((struct Cyc_Absyn_Var_p_struct*)
_tmp365)->f1;_tmp36F=((struct Cyc_Absyn_Var_p_struct*)_tmp365)->f2;_LL297:{const
char*_tmp839;struct Cyc_PP_Doc*_tmp838[3];s=((_tmp838[2]=Cyc_Absynpp_pat2doc(
_tmp36F),((_tmp838[1]=Cyc_PP_text(((_tmp839=" as ",_tag_dyneither(_tmp839,
sizeof(char),5)))),((_tmp838[0]=Cyc_Absynpp_qvar2doc(_tmp36E->name),Cyc_PP_cat(
_tag_dyneither(_tmp838,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL289;_LL298:
if(_tmp365 <= (void*)2)goto _LL29A;if(*((int*)_tmp365)!= 2)goto _LL29A;_tmp370=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp365)->f1;_tmp371=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp365)->f2;_LL299:{const char*_tmp83E;const char*_tmp83D;struct Cyc_PP_Doc*
_tmp83C[4];s=((_tmp83C[3]=Cyc_PP_text(((_tmp83E=">",_tag_dyneither(_tmp83E,
sizeof(char),2)))),((_tmp83C[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp370)),((
_tmp83C[1]=Cyc_PP_text(((_tmp83D="<",_tag_dyneither(_tmp83D,sizeof(char),2)))),((
_tmp83C[0]=Cyc_Absynpp_qvar2doc(_tmp371->name),Cyc_PP_cat(_tag_dyneither(_tmp83C,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL289;_LL29A: if(_tmp365 <= (void*)2)
goto _LL29C;if(*((int*)_tmp365)!= 3)goto _LL29C;_tmp372=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp365)->f1;_tmp373=((struct Cyc_Absyn_Tuple_p_struct*)_tmp365)->f2;_LL29B:{
const char*_tmp847;const char*_tmp846;const char*_tmp845;const char*_tmp844;struct
Cyc_PP_Doc*_tmp843[4];s=((_tmp843[3]=_tmp373?Cyc_PP_text(((_tmp846=", ...)",
_tag_dyneither(_tmp846,sizeof(char),7)))): Cyc_PP_text(((_tmp847=")",
_tag_dyneither(_tmp847,sizeof(char),2)))),((_tmp843[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp845=",",_tag_dyneither(_tmp845,
sizeof(char),2))),_tmp372),((_tmp843[1]=Cyc_PP_text(((_tmp844="(",_tag_dyneither(
_tmp844,sizeof(char),2)))),((_tmp843[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(
_tag_dyneither(_tmp843,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL289;_LL29C:
if(_tmp365 <= (void*)2)goto _LL29E;if(*((int*)_tmp365)!= 4)goto _LL29E;_tmp374=((
struct Cyc_Absyn_Pointer_p_struct*)_tmp365)->f1;_LL29D:{const char*_tmp84A;struct
Cyc_PP_Doc*_tmp849[2];s=((_tmp849[1]=Cyc_Absynpp_pat2doc(_tmp374),((_tmp849[0]=
Cyc_PP_text(((_tmp84A="&",_tag_dyneither(_tmp84A,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp849,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL289;_LL29E: if(
_tmp365 <= (void*)2)goto _LL2A0;if(*((int*)_tmp365)!= 1)goto _LL2A0;_tmp375=((
struct Cyc_Absyn_Reference_p_struct*)_tmp365)->f1;_tmp376=((struct Cyc_Absyn_Reference_p_struct*)
_tmp365)->f2;_tmp377=*_tmp376;_tmp378=_tmp377.r;if((int)_tmp378 != 0)goto _LL2A0;
_LL29F:{const char*_tmp84D;struct Cyc_PP_Doc*_tmp84C[2];s=((_tmp84C[1]=Cyc_Absynpp_qvar2doc(
_tmp375->name),((_tmp84C[0]=Cyc_PP_text(((_tmp84D="*",_tag_dyneither(_tmp84D,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp84C,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL289;_LL2A0: if(_tmp365 <= (void*)2)goto _LL2A2;if(*((int*)_tmp365)!= 1)goto
_LL2A2;_tmp379=((struct Cyc_Absyn_Reference_p_struct*)_tmp365)->f1;_tmp37A=((
struct Cyc_Absyn_Reference_p_struct*)_tmp365)->f2;_LL2A1:{const char*_tmp852;const
char*_tmp851;struct Cyc_PP_Doc*_tmp850[4];s=((_tmp850[3]=Cyc_Absynpp_pat2doc(
_tmp37A),((_tmp850[2]=Cyc_PP_text(((_tmp852=" as ",_tag_dyneither(_tmp852,
sizeof(char),5)))),((_tmp850[1]=Cyc_Absynpp_qvar2doc(_tmp379->name),((_tmp850[0]=
Cyc_PP_text(((_tmp851="*",_tag_dyneither(_tmp851,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp850,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL289;_LL2A2:
if(_tmp365 <= (void*)2)goto _LL2A4;if(*((int*)_tmp365)!= 12)goto _LL2A4;_tmp37B=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp365)->f1;_LL2A3: s=Cyc_Absynpp_qvar2doc(
_tmp37B);goto _LL289;_LL2A4: if(_tmp365 <= (void*)2)goto _LL2A6;if(*((int*)_tmp365)
!= 13)goto _LL2A6;_tmp37C=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp365)->f1;
_tmp37D=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp365)->f2;_tmp37E=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp365)->f3;_LL2A5: {const char*_tmp854;const char*_tmp853;struct _dyneither_ptr
term=_tmp37E?(_tmp854=", ...)",_tag_dyneither(_tmp854,sizeof(char),7)):((_tmp853=")",
_tag_dyneither(_tmp853,sizeof(char),2)));{const char*_tmp859;const char*_tmp858;
struct Cyc_PP_Doc*_tmp857[2];s=((_tmp857[1]=Cyc_PP_group(((_tmp859="(",
_tag_dyneither(_tmp859,sizeof(char),2))),term,((_tmp858=",",_tag_dyneither(
_tmp858,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp37D)),((
_tmp857[0]=Cyc_Absynpp_qvar2doc(_tmp37C),Cyc_PP_cat(_tag_dyneither(_tmp857,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL289;}_LL2A6: if(_tmp365 <= (void*)2)goto
_LL2A8;if(*((int*)_tmp365)!= 5)goto _LL2A8;_tmp37F=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp365)->f1;if(_tmp37F == 0)goto _LL2A8;_tmp380=*_tmp37F;_tmp381=_tmp380.aggr_info;
_tmp382=((struct Cyc_Absyn_Aggr_p_struct*)_tmp365)->f2;_tmp383=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp365)->f3;_tmp384=((struct Cyc_Absyn_Aggr_p_struct*)_tmp365)->f4;_LL2A7: {
const char*_tmp85B;const char*_tmp85A;struct _dyneither_ptr term=_tmp384?(_tmp85B=", ...}",
_tag_dyneither(_tmp85B,sizeof(char),7)):((_tmp85A="}",_tag_dyneither(_tmp85A,
sizeof(char),2)));struct _tuple0*_tmp3B3;struct _tuple9 _tmp3B2=Cyc_Absyn_aggr_kinded_name(
_tmp381);_tmp3B3=_tmp3B2.f2;{const char*_tmp866;const char*_tmp865;const char*
_tmp864;const char*_tmp863;const char*_tmp862;struct Cyc_PP_Doc*_tmp861[4];s=((
_tmp861[3]=Cyc_PP_group(((_tmp866="",_tag_dyneither(_tmp866,sizeof(char),1))),
term,((_tmp865=",",_tag_dyneither(_tmp865,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp383)),((_tmp861[2]=Cyc_PP_egroup(((_tmp864="[",_tag_dyneither(_tmp864,
sizeof(char),2))),((_tmp863="]",_tag_dyneither(_tmp863,sizeof(char),2))),((
_tmp862=",",_tag_dyneither(_tmp862,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp382))),((_tmp861[1]=
Cyc_Absynpp_lb(),((_tmp861[0]=Cyc_Absynpp_qvar2doc(_tmp3B3),Cyc_PP_cat(
_tag_dyneither(_tmp861,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL289;}_LL2A8:
if(_tmp365 <= (void*)2)goto _LL2AA;if(*((int*)_tmp365)!= 5)goto _LL2AA;_tmp385=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp365)->f1;if(_tmp385 != 0)goto _LL2AA;_tmp386=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp365)->f2;_tmp387=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp365)->f3;_tmp388=((struct Cyc_Absyn_Aggr_p_struct*)_tmp365)->f4;_LL2A9: {
const char*_tmp868;const char*_tmp867;struct _dyneither_ptr term=_tmp388?(_tmp868=", ...}",
_tag_dyneither(_tmp868,sizeof(char),7)):((_tmp867="}",_tag_dyneither(_tmp867,
sizeof(char),2)));{const char*_tmp873;const char*_tmp872;const char*_tmp871;const
char*_tmp870;const char*_tmp86F;struct Cyc_PP_Doc*_tmp86E[3];s=((_tmp86E[2]=Cyc_PP_group(((
_tmp873="",_tag_dyneither(_tmp873,sizeof(char),1))),term,((_tmp872=",",
_tag_dyneither(_tmp872,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp387)),((_tmp86E[1]=Cyc_PP_egroup(((_tmp871="[",_tag_dyneither(_tmp871,
sizeof(char),2))),((_tmp870="]",_tag_dyneither(_tmp870,sizeof(char),2))),((
_tmp86F=",",_tag_dyneither(_tmp86F,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp386))),((_tmp86E[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp86E,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL289;}_LL2AA: if(_tmp365 <= (void*)2)goto _LL2AC;if(*((int*)_tmp365)!= 10)
goto _LL2AC;_tmp389=((struct Cyc_Absyn_Enum_p_struct*)_tmp365)->f2;_LL2AB: _tmp38A=
_tmp389;goto _LL2AD;_LL2AC: if(_tmp365 <= (void*)2)goto _LL2AE;if(*((int*)_tmp365)!= 
11)goto _LL2AE;_tmp38A=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp365)->f2;_LL2AD: s=
Cyc_Absynpp_qvar2doc(_tmp38A->name);goto _LL289;_LL2AE: if(_tmp365 <= (void*)2)goto
_LL2B0;if(*((int*)_tmp365)!= 6)goto _LL2B0;_tmp38B=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp365)->f2;_tmp38C=((struct Cyc_Absyn_Datatype_p_struct*)_tmp365)->f3;if(
_tmp38C != 0)goto _LL2B0;_LL2AF: s=Cyc_Absynpp_qvar2doc(_tmp38B->name);goto _LL289;
_LL2B0: if(_tmp365 <= (void*)2)goto _LL2B2;if(*((int*)_tmp365)!= 6)goto _LL2B2;
_tmp38D=((struct Cyc_Absyn_Datatype_p_struct*)_tmp365)->f2;_tmp38E=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp365)->f3;_tmp38F=((struct Cyc_Absyn_Datatype_p_struct*)_tmp365)->f4;_LL2B1: {
const char*_tmp875;const char*_tmp874;struct _dyneither_ptr term=_tmp38F?(_tmp875=", ...)",
_tag_dyneither(_tmp875,sizeof(char),7)):((_tmp874=")",_tag_dyneither(_tmp874,
sizeof(char),2)));{const char*_tmp87A;const char*_tmp879;struct Cyc_PP_Doc*_tmp878[
2];s=((_tmp878[1]=Cyc_PP_egroup(((_tmp87A="(",_tag_dyneither(_tmp87A,sizeof(char),
2))),term,((_tmp879=",",_tag_dyneither(_tmp879,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp38E)),((_tmp878[0]=Cyc_Absynpp_qvar2doc(_tmp38D->name),
Cyc_PP_cat(_tag_dyneither(_tmp878,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL289;}
_LL2B2: if(_tmp365 <= (void*)2)goto _LL289;if(*((int*)_tmp365)!= 14)goto _LL289;
_tmp390=((struct Cyc_Absyn_Exp_p_struct*)_tmp365)->f1;_LL2B3: s=Cyc_Absynpp_exp2doc(
_tmp390);goto _LL289;_LL289:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple14*dp);struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple14*dp){const char*
_tmp881;const char*_tmp880;const char*_tmp87F;struct Cyc_PP_Doc*_tmp87E[2];return(
_tmp87E[1]=Cyc_Absynpp_pat2doc((*dp).f2),((_tmp87E[0]=Cyc_PP_egroup(((_tmp881="",
_tag_dyneither(_tmp881,sizeof(char),1))),((_tmp880="=",_tag_dyneither(_tmp880,
sizeof(char),2))),((_tmp87F="=",_tag_dyneither(_tmp87F,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp87E,sizeof(
struct Cyc_PP_Doc*),2)))));}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c);struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (c->pattern)->r == (void*)((
void*)0)){struct Cyc_PP_Doc*_tmp886[2];const char*_tmp885;struct Cyc_PP_Doc*_tmp884[
2];return(_tmp884[1]=Cyc_PP_nest(2,((_tmp886[1]=Cyc_Absynpp_stmt2doc(c->body),((
_tmp886[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp886,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp884[0]=Cyc_PP_text(((_tmp885="default: ",_tag_dyneither(_tmp885,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp884,sizeof(struct Cyc_PP_Doc*),
2)))));}else{if(c->where_clause == 0){struct Cyc_PP_Doc*_tmp88D[2];const char*
_tmp88C;const char*_tmp88B;struct Cyc_PP_Doc*_tmp88A[4];return(_tmp88A[3]=Cyc_PP_nest(
2,((_tmp88D[1]=Cyc_Absynpp_stmt2doc(c->body),((_tmp88D[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp88D,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp88A[2]=Cyc_PP_text(((
_tmp88C=": ",_tag_dyneither(_tmp88C,sizeof(char),3)))),((_tmp88A[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp88A[0]=Cyc_PP_text(((_tmp88B="case ",_tag_dyneither(_tmp88B,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp88A,sizeof(struct Cyc_PP_Doc*),4)))))))));}
else{struct Cyc_PP_Doc*_tmp896[2];const char*_tmp895;const char*_tmp894;const char*
_tmp893;struct Cyc_PP_Doc*_tmp892[6];return(_tmp892[5]=Cyc_PP_nest(2,((_tmp896[1]=
Cyc_Absynpp_stmt2doc(c->body),((_tmp896[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp896,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp892[4]=Cyc_PP_text(((
_tmp895=": ",_tag_dyneither(_tmp895,sizeof(char),3)))),((_tmp892[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp892[2]=Cyc_PP_text(((
_tmp894=" && ",_tag_dyneither(_tmp894,sizeof(char),5)))),((_tmp892[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp892[0]=Cyc_PP_text(((_tmp893="case ",_tag_dyneither(_tmp893,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp892,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){const char*_tmp897;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((
_tmp897="",_tag_dyneither(_tmp897,sizeof(char),1))),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct
Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);else{
const char*_tmp89A;struct Cyc_PP_Doc*_tmp899[3];return(_tmp899[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp899[1]=Cyc_PP_text(((_tmp89A=" = ",
_tag_dyneither(_tmp89A,sizeof(char),4)))),((_tmp899[0]=Cyc_Absynpp_qvar2doc(f->name),
Cyc_PP_cat(_tag_dyneither(_tmp899,sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){const char*_tmp89B;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp89B=",",_tag_dyneither(_tmp89B,
sizeof(char),2))),fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*
v);static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return
Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct
Cyc_List_List*vds);static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*
vds){const char*_tmp89C;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_id2doc,((_tmp89C=",",_tag_dyneither(_tmp89C,sizeof(char),2))),vds);}
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);struct Cyc_PP_Doc*
Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl
_tmp3DF;void*_tmp3E0;struct _tuple0*_tmp3E1;struct Cyc_Absyn_Tqual _tmp3E2;void*
_tmp3E3;struct Cyc_Absyn_Exp*_tmp3E4;struct Cyc_List_List*_tmp3E5;struct Cyc_Absyn_Vardecl*
_tmp3DE=vd;_tmp3DF=*_tmp3DE;_tmp3E0=_tmp3DF.sc;_tmp3E1=_tmp3DF.name;_tmp3E2=
_tmp3DF.tq;_tmp3E3=_tmp3DF.type;_tmp3E4=_tmp3DF.initializer;_tmp3E5=_tmp3DF.attributes;{
struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp3E1);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp3E5);struct Cyc_PP_Doc*
beforenamedoc;{void*_tmp3E6=Cyc_Cyclone_c_compiler;_LL2B5: if((int)_tmp3E6 != 0)
goto _LL2B7;_LL2B6: beforenamedoc=attsdoc;goto _LL2B4;_LL2B7: if((int)_tmp3E6 != 1)
goto _LL2B4;_LL2B8:{void*_tmp3E7=Cyc_Tcutil_compress(_tmp3E3);struct Cyc_Absyn_FnInfo
_tmp3E8;struct Cyc_List_List*_tmp3E9;_LL2BA: if(_tmp3E7 <= (void*)4)goto _LL2BC;if(*((
int*)_tmp3E7)!= 8)goto _LL2BC;_tmp3E8=((struct Cyc_Absyn_FnType_struct*)_tmp3E7)->f1;
_tmp3E9=_tmp3E8.attributes;_LL2BB: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp3E9);
goto _LL2B9;_LL2BC:;_LL2BD: beforenamedoc=Cyc_PP_nil_doc();goto _LL2B9;_LL2B9:;}
goto _LL2B4;_LL2B4:;}{struct Cyc_PP_Doc*tmp_doc;{void*_tmp3EA=Cyc_Cyclone_c_compiler;
_LL2BF: if((int)_tmp3EA != 0)goto _LL2C1;_LL2C0: tmp_doc=Cyc_PP_nil_doc();goto _LL2BE;
_LL2C1: if((int)_tmp3EA != 1)goto _LL2BE;_LL2C2: tmp_doc=attsdoc;goto _LL2BE;_LL2BE:;}{
const char*_tmp8A9;struct Cyc_PP_Doc*_tmp8A8[2];const char*_tmp8A7;struct Cyc_Core_Opt*
_tmp8A6;struct Cyc_PP_Doc*_tmp8A5[2];struct Cyc_PP_Doc*_tmp8A4[5];s=((_tmp8A4[4]=
Cyc_PP_text(((_tmp8A9=";",_tag_dyneither(_tmp8A9,sizeof(char),2)))),((_tmp8A4[3]=
_tmp3E4 == 0?Cyc_PP_nil_doc():((_tmp8A8[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp3E4),((_tmp8A8[0]=Cyc_PP_text(((_tmp8A7=" = ",_tag_dyneither(_tmp8A7,sizeof(
char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp8A8,sizeof(struct Cyc_PP_Doc*),2)))))),((
_tmp8A4[2]=Cyc_Absynpp_tqtd2doc(_tmp3E2,_tmp3E3,((_tmp8A6=_cycalloc(sizeof(*
_tmp8A6)),((_tmp8A6->v=((_tmp8A5[1]=sn,((_tmp8A5[0]=beforenamedoc,Cyc_PP_cat(
_tag_dyneither(_tmp8A5,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp8A6))))),((_tmp8A4[
1]=Cyc_Absynpp_scope2doc(_tmp3E0),((_tmp8A4[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(
_tmp8A4,sizeof(struct Cyc_PP_Doc*),5))))))))))));}return s;}}}struct _tuple17{
struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(
struct _tuple17*x);struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple0*_tmp3F2;struct _tuple17 _tmp3F1=*x;_tmp3F2=_tmp3F1.f2;return Cyc_Absynpp_qvar2doc(
_tmp3F2);}struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);struct
Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*
_tmp3F3=d->r;struct Cyc_Absyn_Fndecl*_tmp3F4;struct Cyc_Absyn_Aggrdecl*_tmp3F5;
struct Cyc_Absyn_Vardecl*_tmp3F6;struct Cyc_Absyn_Tvar*_tmp3F7;struct Cyc_Absyn_Vardecl*
_tmp3F8;int _tmp3F9;struct Cyc_Absyn_Exp*_tmp3FA;struct Cyc_Absyn_Exp*_tmp3FB;
struct Cyc_Absyn_Tvar*_tmp3FC;struct Cyc_Absyn_Vardecl*_tmp3FD;struct Cyc_Absyn_Datatypedecl*
_tmp3FE;struct Cyc_Absyn_Datatypedecl _tmp3FF;void*_tmp400;struct _tuple0*_tmp401;
struct Cyc_List_List*_tmp402;struct Cyc_Core_Opt*_tmp403;int _tmp404;struct Cyc_Absyn_Pat*
_tmp405;struct Cyc_Absyn_Exp*_tmp406;struct Cyc_List_List*_tmp407;struct Cyc_Absyn_Enumdecl*
_tmp408;struct Cyc_Absyn_Enumdecl _tmp409;void*_tmp40A;struct _tuple0*_tmp40B;
struct Cyc_Core_Opt*_tmp40C;struct Cyc_Absyn_Typedefdecl*_tmp40D;struct
_dyneither_ptr*_tmp40E;struct Cyc_List_List*_tmp40F;struct _tuple0*_tmp410;struct
Cyc_List_List*_tmp411;struct Cyc_List_List*_tmp412;struct Cyc_List_List*_tmp413;
struct Cyc_List_List*_tmp414;_LL2C4: if(_tmp3F3 <= (void*)2)goto _LL2E0;if(*((int*)
_tmp3F3)!= 1)goto _LL2C6;_tmp3F4=((struct Cyc_Absyn_Fn_d_struct*)_tmp3F3)->f1;
_LL2C5: {struct Cyc_Absyn_FnType_struct _tmp8AF;struct Cyc_Absyn_FnInfo _tmp8AE;
struct Cyc_Absyn_FnType_struct*_tmp8AD;void*t=(void*)((_tmp8AD=_cycalloc(sizeof(*
_tmp8AD)),((_tmp8AD[0]=((_tmp8AF.tag=8,((_tmp8AF.f1=((_tmp8AE.tvars=_tmp3F4->tvs,((
_tmp8AE.effect=_tmp3F4->effect,((_tmp8AE.ret_typ=_tmp3F4->ret_type,((_tmp8AE.args=((
struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple12*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp3F4->args),((_tmp8AE.c_varargs=
_tmp3F4->c_varargs,((_tmp8AE.cyc_varargs=_tmp3F4->cyc_varargs,((_tmp8AE.rgn_po=
_tmp3F4->rgn_po,((_tmp8AE.attributes=0,_tmp8AE)))))))))))))))),_tmp8AF)))),
_tmp8AD))));struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp3F4->attributes);
struct Cyc_PP_Doc*inlinedoc;if(_tmp3F4->is_inline){void*_tmp415=Cyc_Cyclone_c_compiler;
_LL2E5: if((int)_tmp415 != 0)goto _LL2E7;_LL2E6:{const char*_tmp8B0;inlinedoc=Cyc_PP_text(((
_tmp8B0="inline ",_tag_dyneither(_tmp8B0,sizeof(char),8))));}goto _LL2E4;_LL2E7:
if((int)_tmp415 != 1)goto _LL2E4;_LL2E8:{const char*_tmp8B1;inlinedoc=Cyc_PP_text(((
_tmp8B1="__inline ",_tag_dyneither(_tmp8B1,sizeof(char),10))));}goto _LL2E4;
_LL2E4:;}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(
_tmp3F4->sc);struct Cyc_PP_Doc*beforenamedoc;{void*_tmp418=Cyc_Cyclone_c_compiler;
_LL2EA: if((int)_tmp418 != 0)goto _LL2EC;_LL2EB: beforenamedoc=attsdoc;goto _LL2E9;
_LL2EC: if((int)_tmp418 != 1)goto _LL2E9;_LL2ED: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp3F4->attributes);goto _LL2E9;_LL2E9:;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp3F4->name);struct Cyc_PP_Doc*_tmp8B4[2];struct Cyc_Core_Opt*_tmp8B3;struct Cyc_PP_Doc*
tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,((_tmp8B3=_cycalloc(
sizeof(*_tmp8B3)),((_tmp8B3->v=((_tmp8B4[1]=namedoc,((_tmp8B4[0]=beforenamedoc,
Cyc_PP_cat(_tag_dyneither(_tmp8B4,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp8B3)))));
struct Cyc_PP_Doc*_tmp8B7[2];struct Cyc_PP_Doc*_tmp8B6[5];struct Cyc_PP_Doc*bodydoc=(
_tmp8B6[4]=Cyc_Absynpp_rb(),((_tmp8B6[3]=Cyc_PP_line_doc(),((_tmp8B6[2]=Cyc_PP_nest(
2,((_tmp8B7[1]=Cyc_Absynpp_stmt2doc(_tmp3F4->body),((_tmp8B7[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dyneither(_tmp8B7,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8B6[1]=
Cyc_Absynpp_lb(),((_tmp8B6[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(
_tmp8B6,sizeof(struct Cyc_PP_Doc*),5)))))))))));{struct Cyc_PP_Doc*_tmp8B8[4];s=((
_tmp8B8[3]=bodydoc,((_tmp8B8[2]=tqtddoc,((_tmp8B8[1]=scopedoc,((_tmp8B8[0]=
inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp8B8,sizeof(struct Cyc_PP_Doc*),4))))))))));}{
void*_tmp41A=Cyc_Cyclone_c_compiler;_LL2EF: if((int)_tmp41A != 1)goto _LL2F1;_LL2F0:{
struct Cyc_PP_Doc*_tmp8B9[2];s=((_tmp8B9[1]=s,((_tmp8B9[0]=attsdoc,Cyc_PP_cat(
_tag_dyneither(_tmp8B9,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL2EE;_LL2F1:;
_LL2F2: goto _LL2EE;_LL2EE:;}goto _LL2C3;}}}_LL2C6: if(*((int*)_tmp3F3)!= 6)goto
_LL2C8;_tmp3F5=((struct Cyc_Absyn_Aggr_d_struct*)_tmp3F3)->f1;_LL2C7: if(_tmp3F5->impl
== 0){const char*_tmp8BC;struct Cyc_PP_Doc*_tmp8BB[5];s=((_tmp8BB[4]=Cyc_PP_text(((
_tmp8BC=";",_tag_dyneither(_tmp8BC,sizeof(char),2)))),((_tmp8BB[3]=Cyc_Absynpp_ktvars2doc(
_tmp3F5->tvs),((_tmp8BB[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3F5->name),((_tmp8BB[1]=
Cyc_Absynpp_aggr_kind2doc(_tmp3F5->kind),((_tmp8BB[0]=Cyc_Absynpp_scope2doc(
_tmp3F5->sc),Cyc_PP_cat(_tag_dyneither(_tmp8BB,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
else{const char*_tmp8C6;struct Cyc_PP_Doc*_tmp8C5[2];struct Cyc_PP_Doc*_tmp8C4[2];
const char*_tmp8C3;struct Cyc_PP_Doc*_tmp8C2[12];s=((_tmp8C2[11]=Cyc_PP_text(((
_tmp8C6=";",_tag_dyneither(_tmp8C6,sizeof(char),2)))),((_tmp8C2[10]=Cyc_Absynpp_atts2doc(
_tmp3F5->attributes),((_tmp8C2[9]=Cyc_Absynpp_rb(),((_tmp8C2[8]=Cyc_PP_line_doc(),((
_tmp8C2[7]=Cyc_PP_nest(2,((_tmp8C5[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3F5->impl))->fields),((_tmp8C5[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp8C5,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8C2[6]=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3F5->impl))->rgn_po == 0?Cyc_PP_nil_doc():((
_tmp8C4[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp3F5->impl))->rgn_po),((_tmp8C4[0]=Cyc_PP_text(((_tmp8C3=":",_tag_dyneither(
_tmp8C3,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8C4,sizeof(struct Cyc_PP_Doc*),
2)))))),((_tmp8C2[5]=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3F5->impl))->exist_vars),((_tmp8C2[4]=Cyc_Absynpp_lb(),((_tmp8C2[
3]=Cyc_PP_blank_doc(),((_tmp8C2[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3F5->name),((
_tmp8C2[1]=Cyc_Absynpp_aggr_kind2doc(_tmp3F5->kind),((_tmp8C2[0]=Cyc_Absynpp_scope2doc(
_tmp3F5->sc),Cyc_PP_cat(_tag_dyneither(_tmp8C2,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
goto _LL2C3;_LL2C8: if(*((int*)_tmp3F3)!= 0)goto _LL2CA;_tmp3F6=((struct Cyc_Absyn_Var_d_struct*)
_tmp3F3)->f1;_LL2C9: s=Cyc_Absynpp_vardecl2doc(_tmp3F6);goto _LL2C3;_LL2CA: if(*((
int*)_tmp3F3)!= 4)goto _LL2CC;_tmp3F7=((struct Cyc_Absyn_Region_d_struct*)_tmp3F3)->f1;
_tmp3F8=((struct Cyc_Absyn_Region_d_struct*)_tmp3F3)->f2;_tmp3F9=((struct Cyc_Absyn_Region_d_struct*)
_tmp3F3)->f3;_tmp3FA=((struct Cyc_Absyn_Region_d_struct*)_tmp3F3)->f4;_LL2CB:{
const char*_tmp8D9;struct Cyc_PP_Doc*_tmp8D8[3];const char*_tmp8D7;const char*
_tmp8D6;const char*_tmp8D5;const char*_tmp8D4;const char*_tmp8D3;const char*_tmp8D2;
struct Cyc_PP_Doc*_tmp8D1[8];s=((_tmp8D1[7]=Cyc_PP_text(((_tmp8D9=";",
_tag_dyneither(_tmp8D9,sizeof(char),2)))),((_tmp8D1[6]=_tmp3FA != 0?(_tmp8D8[2]=
Cyc_PP_text(((_tmp8D6=")",_tag_dyneither(_tmp8D6,sizeof(char),2)))),((_tmp8D8[1]=
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp3FA),((_tmp8D8[0]=Cyc_PP_text(((
_tmp8D7=" = open(",_tag_dyneither(_tmp8D7,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp8D8,sizeof(struct Cyc_PP_Doc*),3))))))): Cyc_PP_nil_doc(),((
_tmp8D1[5]=_tmp3F9?Cyc_PP_text(((_tmp8D5=" @resetable",_tag_dyneither(_tmp8D5,
sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp8D1[4]=Cyc_Absynpp_qvar2doc(_tmp3F8->name),((
_tmp8D1[3]=Cyc_PP_text(((_tmp8D4=">",_tag_dyneither(_tmp8D4,sizeof(char),2)))),((
_tmp8D1[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp3F7)),((_tmp8D1[1]=Cyc_PP_text(((
_tmp8D3="<",_tag_dyneither(_tmp8D3,sizeof(char),2)))),((_tmp8D1[0]=Cyc_PP_text(((
_tmp8D2="region",_tag_dyneither(_tmp8D2,sizeof(char),7)))),Cyc_PP_cat(
_tag_dyneither(_tmp8D1,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}goto _LL2C3;
_LL2CC: if(*((int*)_tmp3F3)!= 5)goto _LL2CE;_tmp3FB=((struct Cyc_Absyn_Alias_d_struct*)
_tmp3F3)->f1;_tmp3FC=((struct Cyc_Absyn_Alias_d_struct*)_tmp3F3)->f2;_tmp3FD=((
struct Cyc_Absyn_Alias_d_struct*)_tmp3F3)->f3;_LL2CD:{const char*_tmp8E4;const char*
_tmp8E3;const char*_tmp8E2;const char*_tmp8E1;const char*_tmp8E0;struct Cyc_PP_Doc*
_tmp8DF[8];s=((_tmp8DF[7]=Cyc_PP_text(((_tmp8E4=";",_tag_dyneither(_tmp8E4,
sizeof(char),2)))),((_tmp8DF[6]=Cyc_Absynpp_exp2doc(_tmp3FB),((_tmp8DF[5]=Cyc_PP_text(((
_tmp8E3=" = ",_tag_dyneither(_tmp8E3,sizeof(char),4)))),((_tmp8DF[4]=Cyc_Absynpp_qvar2doc(
_tmp3FD->name),((_tmp8DF[3]=Cyc_PP_text(((_tmp8E2="> ",_tag_dyneither(_tmp8E2,
sizeof(char),3)))),((_tmp8DF[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp3FC)),((
_tmp8DF[1]=Cyc_PP_text(((_tmp8E1=" <",_tag_dyneither(_tmp8E1,sizeof(char),3)))),((
_tmp8DF[0]=Cyc_PP_text(((_tmp8E0="alias",_tag_dyneither(_tmp8E0,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp8DF,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL2C3;_LL2CE: if(*((int*)_tmp3F3)!= 7)goto _LL2D0;_tmp3FE=((struct Cyc_Absyn_Datatype_d_struct*)
_tmp3F3)->f1;_tmp3FF=*_tmp3FE;_tmp400=_tmp3FF.sc;_tmp401=_tmp3FF.name;_tmp402=
_tmp3FF.tvs;_tmp403=_tmp3FF.fields;_tmp404=_tmp3FF.is_extensible;_LL2CF: if(
_tmp403 == 0){const char*_tmp8EB;const char*_tmp8EA;const char*_tmp8E9;struct Cyc_PP_Doc*
_tmp8E8[6];s=((_tmp8E8[5]=Cyc_PP_text(((_tmp8EB=";",_tag_dyneither(_tmp8EB,
sizeof(char),2)))),((_tmp8E8[4]=Cyc_Absynpp_ktvars2doc(_tmp402),((_tmp8E8[3]=
_tmp404?Cyc_Absynpp_qvar2bolddoc(_tmp401): Cyc_Absynpp_typedef_name2bolddoc(
_tmp401),((_tmp8E8[2]=Cyc_PP_text(((_tmp8EA="datatype ",_tag_dyneither(_tmp8EA,
sizeof(char),10)))),((_tmp8E8[1]=_tmp404?Cyc_PP_text(((_tmp8E9="@extensible",
_tag_dyneither(_tmp8E9,sizeof(char),12)))): Cyc_PP_blank_doc(),((_tmp8E8[0]=Cyc_Absynpp_scope2doc(
_tmp400),Cyc_PP_cat(_tag_dyneither(_tmp8E8,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmp8F4;struct Cyc_PP_Doc*_tmp8F3[2];const char*_tmp8F2;const char*
_tmp8F1;struct Cyc_PP_Doc*_tmp8F0[11];s=((_tmp8F0[10]=Cyc_PP_text(((_tmp8F4=";",
_tag_dyneither(_tmp8F4,sizeof(char),2)))),((_tmp8F0[9]=Cyc_Absynpp_rb(),((
_tmp8F0[8]=Cyc_PP_line_doc(),((_tmp8F0[7]=Cyc_PP_nest(2,((_tmp8F3[1]=Cyc_Absynpp_datatypefields2doc((
struct Cyc_List_List*)_tmp403->v),((_tmp8F3[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp8F3,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8F0[6]=Cyc_Absynpp_lb(),((
_tmp8F0[5]=Cyc_PP_blank_doc(),((_tmp8F0[4]=Cyc_Absynpp_ktvars2doc(_tmp402),((
_tmp8F0[3]=_tmp404?Cyc_Absynpp_qvar2bolddoc(_tmp401): Cyc_Absynpp_typedef_name2bolddoc(
_tmp401),((_tmp8F0[2]=Cyc_PP_text(((_tmp8F2="datatype ",_tag_dyneither(_tmp8F2,
sizeof(char),10)))),((_tmp8F0[1]=_tmp404?Cyc_PP_text(((_tmp8F1="@extensible ",
_tag_dyneither(_tmp8F1,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp8F0[0]=Cyc_Absynpp_scope2doc(
_tmp400),Cyc_PP_cat(_tag_dyneither(_tmp8F0,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL2C3;_LL2D0: if(*((int*)_tmp3F3)!= 2)goto _LL2D2;_tmp405=((struct Cyc_Absyn_Let_d_struct*)
_tmp3F3)->f1;_tmp406=((struct Cyc_Absyn_Let_d_struct*)_tmp3F3)->f3;_LL2D1:{const
char*_tmp8FB;const char*_tmp8FA;const char*_tmp8F9;struct Cyc_PP_Doc*_tmp8F8[5];s=((
_tmp8F8[4]=Cyc_PP_text(((_tmp8FB=";",_tag_dyneither(_tmp8FB,sizeof(char),2)))),((
_tmp8F8[3]=Cyc_Absynpp_exp2doc(_tmp406),((_tmp8F8[2]=Cyc_PP_text(((_tmp8FA=" = ",
_tag_dyneither(_tmp8FA,sizeof(char),4)))),((_tmp8F8[1]=Cyc_Absynpp_pat2doc(
_tmp405),((_tmp8F8[0]=Cyc_PP_text(((_tmp8F9="let ",_tag_dyneither(_tmp8F9,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8F8,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL2C3;_LL2D2: if(*((int*)_tmp3F3)!= 3)goto _LL2D4;_tmp407=((struct Cyc_Absyn_Letv_d_struct*)
_tmp3F3)->f1;_LL2D3:{const char*_tmp900;const char*_tmp8FF;struct Cyc_PP_Doc*
_tmp8FE[3];s=((_tmp8FE[2]=Cyc_PP_text(((_tmp900=";",_tag_dyneither(_tmp900,
sizeof(char),2)))),((_tmp8FE[1]=Cyc_Absynpp_ids2doc(_tmp407),((_tmp8FE[0]=Cyc_PP_text(((
_tmp8FF="let ",_tag_dyneither(_tmp8FF,sizeof(char),5)))),Cyc_PP_cat(
_tag_dyneither(_tmp8FE,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL2C3;_LL2D4:
if(*((int*)_tmp3F3)!= 8)goto _LL2D6;_tmp408=((struct Cyc_Absyn_Enum_d_struct*)
_tmp3F3)->f1;_tmp409=*_tmp408;_tmp40A=_tmp409.sc;_tmp40B=_tmp409.name;_tmp40C=
_tmp409.fields;_LL2D5: if(_tmp40C == 0){const char*_tmp905;const char*_tmp904;struct
Cyc_PP_Doc*_tmp903[4];s=((_tmp903[3]=Cyc_PP_text(((_tmp905=";",_tag_dyneither(
_tmp905,sizeof(char),2)))),((_tmp903[2]=Cyc_Absynpp_typedef_name2bolddoc(_tmp40B),((
_tmp903[1]=Cyc_PP_text(((_tmp904="enum ",_tag_dyneither(_tmp904,sizeof(char),6)))),((
_tmp903[0]=Cyc_Absynpp_scope2doc(_tmp40A),Cyc_PP_cat(_tag_dyneither(_tmp903,
sizeof(struct Cyc_PP_Doc*),4))))))))));}else{const char*_tmp90C;struct Cyc_PP_Doc*
_tmp90B[2];const char*_tmp90A;struct Cyc_PP_Doc*_tmp909[9];s=((_tmp909[8]=Cyc_PP_text(((
_tmp90C=";",_tag_dyneither(_tmp90C,sizeof(char),2)))),((_tmp909[7]=Cyc_Absynpp_rb(),((
_tmp909[6]=Cyc_PP_line_doc(),((_tmp909[5]=Cyc_PP_nest(2,((_tmp90B[1]=Cyc_Absynpp_enumfields2doc((
struct Cyc_List_List*)_tmp40C->v),((_tmp90B[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp90B,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp909[4]=Cyc_Absynpp_lb(),((
_tmp909[3]=Cyc_PP_blank_doc(),((_tmp909[2]=Cyc_Absynpp_qvar2bolddoc(_tmp40B),((
_tmp909[1]=Cyc_PP_text(((_tmp90A="enum ",_tag_dyneither(_tmp90A,sizeof(char),6)))),((
_tmp909[0]=Cyc_Absynpp_scope2doc(_tmp40A),Cyc_PP_cat(_tag_dyneither(_tmp909,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL2C3;_LL2D6: if(*((int*)
_tmp3F3)!= 9)goto _LL2D8;_tmp40D=((struct Cyc_Absyn_Typedef_d_struct*)_tmp3F3)->f1;
_LL2D7: {void*t;if(_tmp40D->defn != 0)t=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp40D->defn))->v;else{t=Cyc_Absyn_new_evar(_tmp40D->kind,0);}{const char*
_tmp916;struct Cyc_Core_Opt*_tmp915;struct Cyc_PP_Doc*_tmp914[2];const char*_tmp913;
struct Cyc_PP_Doc*_tmp912[4];s=((_tmp912[3]=Cyc_PP_text(((_tmp916=";",
_tag_dyneither(_tmp916,sizeof(char),2)))),((_tmp912[2]=Cyc_Absynpp_atts2doc(
_tmp40D->atts),((_tmp912[1]=Cyc_Absynpp_tqtd2doc(_tmp40D->tq,t,((_tmp915=
_cycalloc(sizeof(*_tmp915)),((_tmp915->v=((_tmp914[1]=Cyc_Absynpp_tvars2doc(
_tmp40D->tvs),((_tmp914[0]=Cyc_Absynpp_typedef_name2bolddoc(_tmp40D->name),Cyc_PP_cat(
_tag_dyneither(_tmp914,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp915))))),((_tmp912[
0]=Cyc_PP_text(((_tmp913="typedef ",_tag_dyneither(_tmp913,sizeof(char),9)))),
Cyc_PP_cat(_tag_dyneither(_tmp912,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL2C3;}_LL2D8: if(*((int*)_tmp3F3)!= 10)goto _LL2DA;_tmp40E=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp3F3)->f1;_tmp40F=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3F3)->f2;_LL2D9:
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp40E);{const
char*_tmp91B;const char*_tmp91A;struct Cyc_PP_Doc*_tmp919[8];s=((_tmp919[7]=Cyc_Absynpp_rb(),((
_tmp919[6]=Cyc_PP_line_doc(),((_tmp919[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp91B="",_tag_dyneither(_tmp91B,sizeof(char),1))),
_tmp40F),((_tmp919[4]=Cyc_PP_line_doc(),((_tmp919[3]=Cyc_Absynpp_lb(),((_tmp919[
2]=Cyc_PP_blank_doc(),((_tmp919[1]=Cyc_PP_textptr(_tmp40E),((_tmp919[0]=Cyc_PP_text(((
_tmp91A="namespace ",_tag_dyneither(_tmp91A,sizeof(char),11)))),Cyc_PP_cat(
_tag_dyneither(_tmp919,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}if(Cyc_Absynpp_use_curr_namespace)
Cyc_Absynpp_curr_namespace_drop();goto _LL2C3;_LL2DA: if(*((int*)_tmp3F3)!= 11)
goto _LL2DC;_tmp410=((struct Cyc_Absyn_Using_d_struct*)_tmp3F3)->f1;_tmp411=((
struct Cyc_Absyn_Using_d_struct*)_tmp3F3)->f2;_LL2DB: if(Cyc_Absynpp_print_using_stmts){
const char*_tmp920;const char*_tmp91F;struct Cyc_PP_Doc*_tmp91E[8];s=((_tmp91E[7]=
Cyc_Absynpp_rb(),((_tmp91E[6]=Cyc_PP_line_doc(),((_tmp91E[5]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmp920="",_tag_dyneither(_tmp920,
sizeof(char),1))),_tmp411),((_tmp91E[4]=Cyc_PP_line_doc(),((_tmp91E[3]=Cyc_Absynpp_lb(),((
_tmp91E[2]=Cyc_PP_blank_doc(),((_tmp91E[1]=Cyc_Absynpp_qvar2doc(_tmp410),((
_tmp91E[0]=Cyc_PP_text(((_tmp91F="using ",_tag_dyneither(_tmp91F,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmp91E,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
else{const char*_tmp92B;const char*_tmp92A;const char*_tmp929;const char*_tmp928;
const char*_tmp927;struct Cyc_PP_Doc*_tmp926[11];s=((_tmp926[10]=Cyc_PP_text(((
_tmp92B=" */",_tag_dyneither(_tmp92B,sizeof(char),4)))),((_tmp926[9]=Cyc_Absynpp_rb(),((
_tmp926[8]=Cyc_PP_text(((_tmp92A="/* ",_tag_dyneither(_tmp92A,sizeof(char),4)))),((
_tmp926[7]=Cyc_PP_line_doc(),((_tmp926[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp929="",_tag_dyneither(_tmp929,sizeof(char),1))),
_tmp411),((_tmp926[5]=Cyc_PP_line_doc(),((_tmp926[4]=Cyc_PP_text(((_tmp928=" */",
_tag_dyneither(_tmp928,sizeof(char),4)))),((_tmp926[3]=Cyc_Absynpp_lb(),((
_tmp926[2]=Cyc_PP_blank_doc(),((_tmp926[1]=Cyc_Absynpp_qvar2doc(_tmp410),((
_tmp926[0]=Cyc_PP_text(((_tmp927="/* using ",_tag_dyneither(_tmp927,sizeof(char),
10)))),Cyc_PP_cat(_tag_dyneither(_tmp926,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL2C3;_LL2DC: if(*((int*)_tmp3F3)!= 12)goto _LL2DE;_tmp412=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp3F3)->f1;_LL2DD: if(Cyc_Absynpp_print_externC_stmts){const char*_tmp930;const
char*_tmp92F;struct Cyc_PP_Doc*_tmp92E[6];s=((_tmp92E[5]=Cyc_Absynpp_rb(),((
_tmp92E[4]=Cyc_PP_line_doc(),((_tmp92E[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp930="",_tag_dyneither(_tmp930,sizeof(char),1))),
_tmp412),((_tmp92E[2]=Cyc_PP_line_doc(),((_tmp92E[1]=Cyc_Absynpp_lb(),((_tmp92E[
0]=Cyc_PP_text(((_tmp92F="extern \"C\" ",_tag_dyneither(_tmp92F,sizeof(char),12)))),
Cyc_PP_cat(_tag_dyneither(_tmp92E,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmp93B;const char*_tmp93A;const char*_tmp939;const char*_tmp938;
const char*_tmp937;struct Cyc_PP_Doc*_tmp936[9];s=((_tmp936[8]=Cyc_PP_text(((
_tmp93B=" */",_tag_dyneither(_tmp93B,sizeof(char),4)))),((_tmp936[7]=Cyc_Absynpp_rb(),((
_tmp936[6]=Cyc_PP_text(((_tmp93A="/* ",_tag_dyneither(_tmp93A,sizeof(char),4)))),((
_tmp936[5]=Cyc_PP_line_doc(),((_tmp936[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp939="",_tag_dyneither(_tmp939,sizeof(char),1))),
_tmp412),((_tmp936[3]=Cyc_PP_line_doc(),((_tmp936[2]=Cyc_PP_text(((_tmp938=" */",
_tag_dyneither(_tmp938,sizeof(char),4)))),((_tmp936[1]=Cyc_Absynpp_lb(),((
_tmp936[0]=Cyc_PP_text(((_tmp937="/* extern \"C\" ",_tag_dyneither(_tmp937,
sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp936,sizeof(struct Cyc_PP_Doc*),
9))))))))))))))))))));}goto _LL2C3;_LL2DE: if(*((int*)_tmp3F3)!= 13)goto _LL2E0;
_tmp413=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp3F3)->f1;_tmp414=((struct
Cyc_Absyn_ExternCinclude_d_struct*)_tmp3F3)->f2;_LL2DF: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp414 != 0){const char*_tmp940;const char*_tmp93F;
struct Cyc_PP_Doc*_tmp93E[7];exs_doc=((_tmp93E[6]=Cyc_Absynpp_rb(),((_tmp93E[5]=
Cyc_PP_line_doc(),((_tmp93E[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_export2doc,((_tmp940=",",_tag_dyneither(_tmp940,sizeof(char),2))),
_tmp414),((_tmp93E[3]=Cyc_PP_line_doc(),((_tmp93E[2]=Cyc_Absynpp_lb(),((_tmp93E[
1]=Cyc_PP_text(((_tmp93F=" export ",_tag_dyneither(_tmp93F,sizeof(char),9)))),((
_tmp93E[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmp93E,sizeof(struct Cyc_PP_Doc*),
7))))))))))))))));}else{exs_doc=Cyc_Absynpp_rb();}{const char*_tmp945;const char*
_tmp944;struct Cyc_PP_Doc*_tmp943[6];s=((_tmp943[5]=exs_doc,((_tmp943[4]=Cyc_PP_line_doc(),((
_tmp943[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmp945="",_tag_dyneither(_tmp945,sizeof(char),1))),_tmp413),((_tmp943[2]=Cyc_PP_line_doc(),((
_tmp943[1]=Cyc_Absynpp_lb(),((_tmp943[0]=Cyc_PP_text(((_tmp944="extern \"C include\" ",
_tag_dyneither(_tmp944,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmp943,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}}else{const char*_tmp950;const char*
_tmp94F;const char*_tmp94E;const char*_tmp94D;const char*_tmp94C;struct Cyc_PP_Doc*
_tmp94B[9];s=((_tmp94B[8]=Cyc_PP_text(((_tmp950=" */",_tag_dyneither(_tmp950,
sizeof(char),4)))),((_tmp94B[7]=Cyc_Absynpp_rb(),((_tmp94B[6]=Cyc_PP_text(((
_tmp94F="/* ",_tag_dyneither(_tmp94F,sizeof(char),4)))),((_tmp94B[5]=Cyc_PP_line_doc(),((
_tmp94B[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmp94E="",_tag_dyneither(_tmp94E,sizeof(char),1))),_tmp413),((_tmp94B[3]=Cyc_PP_line_doc(),((
_tmp94B[2]=Cyc_PP_text(((_tmp94D=" */",_tag_dyneither(_tmp94D,sizeof(char),4)))),((
_tmp94B[1]=Cyc_Absynpp_lb(),((_tmp94B[0]=Cyc_PP_text(((_tmp94C="/* extern \"C include\" ",
_tag_dyneither(_tmp94C,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmp94B,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL2C3;_LL2E0: if((int)
_tmp3F3 != 0)goto _LL2E2;_LL2E1:{const char*_tmp953;struct Cyc_PP_Doc*_tmp952[2];s=((
_tmp952[1]=Cyc_Absynpp_lb(),((_tmp952[0]=Cyc_PP_text(((_tmp953="__cyclone_port_on__;",
_tag_dyneither(_tmp953,sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmp952,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL2C3;_LL2E2: if((int)_tmp3F3 != 1)goto
_LL2C3;_LL2E3:{const char*_tmp956;struct Cyc_PP_Doc*_tmp955[2];s=((_tmp955[1]=Cyc_Absynpp_lb(),((
_tmp955[0]=Cyc_PP_text(((_tmp956="__cyclone_port_off__;",_tag_dyneither(_tmp956,
sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmp955,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL2C3;_LL2C3:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
void*sc);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*sc){if(Cyc_Absynpp_print_for_cycdoc)
return Cyc_PP_nil_doc();{void*_tmp47A=sc;_LL2F4: if((int)_tmp47A != 0)goto _LL2F6;
_LL2F5: {const char*_tmp957;return Cyc_PP_text(((_tmp957="static ",_tag_dyneither(
_tmp957,sizeof(char),8))));}_LL2F6: if((int)_tmp47A != 2)goto _LL2F8;_LL2F7: return
Cyc_PP_nil_doc();_LL2F8: if((int)_tmp47A != 3)goto _LL2FA;_LL2F9: {const char*
_tmp958;return Cyc_PP_text(((_tmp958="extern ",_tag_dyneither(_tmp958,sizeof(char),
8))));}_LL2FA: if((int)_tmp47A != 4)goto _LL2FC;_LL2FB: {const char*_tmp959;return
Cyc_PP_text(((_tmp959="extern \"C\" ",_tag_dyneither(_tmp959,sizeof(char),12))));}
_LL2FC: if((int)_tmp47A != 1)goto _LL2FE;_LL2FD: {const char*_tmp95A;return Cyc_PP_text(((
_tmp95A="abstract ",_tag_dyneither(_tmp95A,sizeof(char),10))));}_LL2FE: if((int)
_tmp47A != 5)goto _LL2F3;_LL2FF: {const char*_tmp95B;return Cyc_PP_text(((_tmp95B="register ",
_tag_dyneither(_tmp95B,sizeof(char),10))));}_LL2F3:;}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t);int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){void*_tmp480=t;struct
Cyc_Absyn_Tvar*_tmp481;struct Cyc_List_List*_tmp482;_LL301: if(_tmp480 <= (void*)4)
goto _LL305;if(*((int*)_tmp480)!= 1)goto _LL303;_tmp481=((struct Cyc_Absyn_VarType_struct*)
_tmp480)->f1;_LL302: return Cyc_Tcutil_is_temp_tvar(_tmp481);_LL303: if(*((int*)
_tmp480)!= 20)goto _LL305;_tmp482=((struct Cyc_Absyn_JoinEff_struct*)_tmp480)->f1;
_LL304: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp482);
_LL305:;_LL306: return 0;_LL300:;}int Cyc_Absynpp_is_anon_aggrtype(void*t);int Cyc_Absynpp_is_anon_aggrtype(
void*t){void*_tmp483=t;void**_tmp484;void*_tmp485;_LL308: if(_tmp483 <= (void*)4)
goto _LL30E;if(*((int*)_tmp483)!= 11)goto _LL30A;_LL309: return 1;_LL30A: if(*((int*)
_tmp483)!= 13)goto _LL30C;_LL30B: return 1;_LL30C: if(*((int*)_tmp483)!= 16)goto
_LL30E;_tmp484=((struct Cyc_Absyn_TypedefType_struct*)_tmp483)->f4;if(_tmp484 == 0)
goto _LL30E;_tmp485=*_tmp484;_LL30D: return Cyc_Absynpp_is_anon_aggrtype(_tmp485);
_LL30E:;_LL30F: return 0;_LL307:;}static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms);static struct Cyc_List_List*
Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*
tms){if(tms != 0  && tms->tl != 0){struct _tuple11 _tmp95C;struct _tuple11 _tmp487=(
_tmp95C.f1=(void*)tms->hd,((_tmp95C.f2=(void*)((struct Cyc_List_List*)_check_null(
tms->tl))->hd,_tmp95C)));void*_tmp488;void*_tmp489;_LL311: _tmp488=_tmp487.f1;if(*((
int*)_tmp488)!= 2)goto _LL313;_tmp489=_tmp487.f2;if(*((int*)_tmp489)!= 3)goto
_LL313;_LL312: {struct Cyc_List_List*_tmp95F;struct Cyc_List_List*_tmp95E;return(
_tmp95E=_region_malloc(r,sizeof(*_tmp95E)),((_tmp95E->hd=(void*)((void*)tms->hd),((
_tmp95E->tl=((_tmp95F=_region_malloc(r,sizeof(*_tmp95F)),((_tmp95F->hd=(void*)((
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd),((_tmp95F->tl=Cyc_Absynpp_bubble_attributes(
r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmp95F)))))),_tmp95E)))));}
_LL313:;_LL314: {struct Cyc_List_List*_tmp960;return(_tmp960=_region_malloc(r,
sizeof(*_tmp960)),((_tmp960->hd=(void*)atts,((_tmp960->tl=tms,_tmp960)))));}
_LL310:;}else{struct Cyc_List_List*_tmp961;return(_tmp961=_region_malloc(r,
sizeof(*_tmp961)),((_tmp961->hd=(void*)atts,((_tmp961->tl=tms,_tmp961)))));}}
struct _tuple13 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,
void*t);struct _tuple13 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t){void*_tmp48E=t;struct Cyc_Absyn_ArrayInfo _tmp48F;void*_tmp490;struct
Cyc_Absyn_Tqual _tmp491;struct Cyc_Absyn_Exp*_tmp492;union Cyc_Absyn_Constraint*
_tmp493;struct Cyc_Position_Segment*_tmp494;struct Cyc_Absyn_PtrInfo _tmp495;void*
_tmp496;struct Cyc_Absyn_Tqual _tmp497;struct Cyc_Absyn_PtrAtts _tmp498;struct Cyc_Absyn_FnInfo
_tmp499;struct Cyc_List_List*_tmp49A;struct Cyc_Core_Opt*_tmp49B;void*_tmp49C;
struct Cyc_List_List*_tmp49D;int _tmp49E;struct Cyc_Absyn_VarargInfo*_tmp49F;struct
Cyc_List_List*_tmp4A0;struct Cyc_List_List*_tmp4A1;struct Cyc_Core_Opt*_tmp4A2;
struct Cyc_Core_Opt*_tmp4A3;int _tmp4A4;struct _tuple0*_tmp4A5;struct Cyc_List_List*
_tmp4A6;void**_tmp4A7;_LL316: if(_tmp48E <= (void*)4)goto _LL320;if(*((int*)_tmp48E)
!= 7)goto _LL318;_tmp48F=((struct Cyc_Absyn_ArrayType_struct*)_tmp48E)->f1;_tmp490=
_tmp48F.elt_type;_tmp491=_tmp48F.tq;_tmp492=_tmp48F.num_elts;_tmp493=_tmp48F.zero_term;
_tmp494=_tmp48F.zt_loc;_LL317: {struct Cyc_Absyn_Tqual _tmp4A9;void*_tmp4AA;struct
Cyc_List_List*_tmp4AB;struct _tuple13 _tmp4A8=Cyc_Absynpp_to_tms(r,_tmp491,_tmp490);
_tmp4A9=_tmp4A8.f1;_tmp4AA=_tmp4A8.f2;_tmp4AB=_tmp4A8.f3;{void*tm;if(_tmp492 == 0){
struct Cyc_Absyn_Carray_mod_struct _tmp964;struct Cyc_Absyn_Carray_mod_struct*
_tmp963;tm=(void*)((_tmp963=_region_malloc(r,sizeof(*_tmp963)),((_tmp963[0]=((
_tmp964.tag=0,((_tmp964.f1=_tmp493,((_tmp964.f2=_tmp494,_tmp964)))))),_tmp963))));}
else{struct Cyc_Absyn_ConstArray_mod_struct _tmp967;struct Cyc_Absyn_ConstArray_mod_struct*
_tmp966;tm=(void*)((_tmp966=_region_malloc(r,sizeof(*_tmp966)),((_tmp966[0]=((
_tmp967.tag=1,((_tmp967.f1=(struct Cyc_Absyn_Exp*)_tmp492,((_tmp967.f2=_tmp493,((
_tmp967.f3=_tmp494,_tmp967)))))))),_tmp966))));}{struct Cyc_List_List*_tmp96A;
struct _tuple13 _tmp969;return(_tmp969.f1=_tmp4A9,((_tmp969.f2=_tmp4AA,((_tmp969.f3=((
_tmp96A=_region_malloc(r,sizeof(*_tmp96A)),((_tmp96A->hd=(void*)tm,((_tmp96A->tl=
_tmp4AB,_tmp96A)))))),_tmp969)))));}}}_LL318: if(*((int*)_tmp48E)!= 4)goto _LL31A;
_tmp495=((struct Cyc_Absyn_PointerType_struct*)_tmp48E)->f1;_tmp496=_tmp495.elt_typ;
_tmp497=_tmp495.elt_tq;_tmp498=_tmp495.ptr_atts;_LL319: {struct Cyc_Absyn_Tqual
_tmp4B3;void*_tmp4B4;struct Cyc_List_List*_tmp4B5;struct _tuple13 _tmp4B2=Cyc_Absynpp_to_tms(
r,_tmp497,_tmp496);_tmp4B3=_tmp4B2.f1;_tmp4B4=_tmp4B2.f2;_tmp4B5=_tmp4B2.f3;{
struct Cyc_Absyn_Pointer_mod_struct*_tmp970;struct Cyc_Absyn_Pointer_mod_struct
_tmp96F;struct Cyc_List_List*_tmp96E;_tmp4B5=((_tmp96E=_region_malloc(r,sizeof(*
_tmp96E)),((_tmp96E->hd=(void*)((void*)((_tmp970=_region_malloc(r,sizeof(*
_tmp970)),((_tmp970[0]=((_tmp96F.tag=2,((_tmp96F.f1=_tmp498,((_tmp96F.f2=tq,
_tmp96F)))))),_tmp970))))),((_tmp96E->tl=_tmp4B5,_tmp96E))))));}{struct _tuple13
_tmp971;return(_tmp971.f1=_tmp4B3,((_tmp971.f2=_tmp4B4,((_tmp971.f3=_tmp4B5,
_tmp971)))));}}_LL31A: if(*((int*)_tmp48E)!= 8)goto _LL31C;_tmp499=((struct Cyc_Absyn_FnType_struct*)
_tmp48E)->f1;_tmp49A=_tmp499.tvars;_tmp49B=_tmp499.effect;_tmp49C=_tmp499.ret_typ;
_tmp49D=_tmp499.args;_tmp49E=_tmp499.c_varargs;_tmp49F=_tmp499.cyc_varargs;
_tmp4A0=_tmp499.rgn_po;_tmp4A1=_tmp499.attributes;_LL31B: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp49B == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp49B->v)){
_tmp49B=0;_tmp49A=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp49A);}{struct Cyc_Absyn_Tqual _tmp4BB;void*_tmp4BC;struct Cyc_List_List*_tmp4BD;
struct _tuple13 _tmp4BA=Cyc_Absynpp_to_tms(r,Cyc_Absyn_empty_tqual(0),_tmp49C);
_tmp4BB=_tmp4BA.f1;_tmp4BC=_tmp4BA.f2;_tmp4BD=_tmp4BA.f3;{struct Cyc_List_List*
tms=_tmp4BD;{void*_tmp4BE=Cyc_Cyclone_c_compiler;_LL323: if((int)_tmp4BE != 0)goto
_LL325;_LL324: if(_tmp4A1 != 0){struct Cyc_Absyn_Attributes_mod_struct _tmp974;
struct Cyc_Absyn_Attributes_mod_struct*_tmp973;tms=Cyc_Absynpp_bubble_attributes(
r,(void*)((_tmp973=_region_malloc(r,sizeof(*_tmp973)),((_tmp973[0]=((_tmp974.tag=
5,((_tmp974.f1=0,((_tmp974.f2=_tmp4A1,_tmp974)))))),_tmp973)))),tms);}{struct Cyc_Absyn_Function_mod_struct*
_tmp983;struct Cyc_Absyn_WithTypes_struct*_tmp982;struct Cyc_Absyn_WithTypes_struct
_tmp981;struct Cyc_Absyn_Function_mod_struct _tmp980;struct Cyc_List_List*_tmp97F;
tms=((_tmp97F=_region_malloc(r,sizeof(*_tmp97F)),((_tmp97F->hd=(void*)((void*)((
_tmp983=_region_malloc(r,sizeof(*_tmp983)),((_tmp983[0]=((_tmp980.tag=3,((
_tmp980.f1=(void*)((void*)((_tmp982=_region_malloc(r,sizeof(*_tmp982)),((_tmp982[
0]=((_tmp981.tag=1,((_tmp981.f1=_tmp49D,((_tmp981.f2=_tmp49E,((_tmp981.f3=
_tmp49F,((_tmp981.f4=_tmp49B,((_tmp981.f5=_tmp4A0,_tmp981)))))))))))),_tmp982))))),
_tmp980)))),_tmp983))))),((_tmp97F->tl=tms,_tmp97F))))));}goto _LL322;_LL325: if((
int)_tmp4BE != 1)goto _LL322;_LL326:{struct Cyc_Absyn_Function_mod_struct*_tmp992;
struct Cyc_Absyn_WithTypes_struct*_tmp991;struct Cyc_Absyn_WithTypes_struct _tmp990;
struct Cyc_Absyn_Function_mod_struct _tmp98F;struct Cyc_List_List*_tmp98E;tms=((
_tmp98E=_region_malloc(r,sizeof(*_tmp98E)),((_tmp98E->hd=(void*)((void*)((
_tmp992=_region_malloc(r,sizeof(*_tmp992)),((_tmp992[0]=((_tmp98F.tag=3,((
_tmp98F.f1=(void*)((void*)((_tmp991=_region_malloc(r,sizeof(*_tmp991)),((_tmp991[
0]=((_tmp990.tag=1,((_tmp990.f1=_tmp49D,((_tmp990.f2=_tmp49E,((_tmp990.f3=
_tmp49F,((_tmp990.f4=_tmp49B,((_tmp990.f5=_tmp4A0,_tmp990)))))))))))),_tmp991))))),
_tmp98F)))),_tmp992))))),((_tmp98E->tl=tms,_tmp98E))))));}for(0;_tmp4A1 != 0;
_tmp4A1=_tmp4A1->tl){void*_tmp4CB=(void*)_tmp4A1->hd;_LL328: if((int)_tmp4CB != 0)
goto _LL32A;_LL329: goto _LL32B;_LL32A: if((int)_tmp4CB != 1)goto _LL32C;_LL32B: goto
_LL32D;_LL32C: if((int)_tmp4CB != 2)goto _LL32E;_LL32D:{struct Cyc_Absyn_Attributes_mod_struct*
_tmp99C;struct Cyc_List_List*_tmp99B;struct Cyc_Absyn_Attributes_mod_struct _tmp99A;
struct Cyc_List_List*_tmp999;tms=((_tmp999=_region_malloc(r,sizeof(*_tmp999)),((
_tmp999->hd=(void*)((void*)((_tmp99C=_region_malloc(r,sizeof(*_tmp99C)),((
_tmp99C[0]=((_tmp99A.tag=5,((_tmp99A.f1=0,((_tmp99A.f2=((_tmp99B=_cycalloc(
sizeof(*_tmp99B)),((_tmp99B->hd=(void*)((void*)_tmp4A1->hd),((_tmp99B->tl=0,
_tmp99B)))))),_tmp99A)))))),_tmp99C))))),((_tmp999->tl=tms,_tmp999))))));}goto
AfterAtts;_LL32E:;_LL32F: goto _LL327;_LL327:;}goto _LL322;_LL322:;}AfterAtts: if(
_tmp49A != 0){struct Cyc_Absyn_TypeParams_mod_struct*_tmp9A2;struct Cyc_Absyn_TypeParams_mod_struct
_tmp9A1;struct Cyc_List_List*_tmp9A0;tms=((_tmp9A0=_region_malloc(r,sizeof(*
_tmp9A0)),((_tmp9A0->hd=(void*)((void*)((_tmp9A2=_region_malloc(r,sizeof(*
_tmp9A2)),((_tmp9A2[0]=((_tmp9A1.tag=4,((_tmp9A1.f1=_tmp49A,((_tmp9A1.f2=0,((
_tmp9A1.f3=1,_tmp9A1)))))))),_tmp9A2))))),((_tmp9A0->tl=tms,_tmp9A0))))));}{
struct _tuple13 _tmp9A3;return(_tmp9A3.f1=_tmp4BB,((_tmp9A3.f2=_tmp4BC,((_tmp9A3.f3=
tms,_tmp9A3)))));}}}_LL31C: if(*((int*)_tmp48E)!= 0)goto _LL31E;_tmp4A2=((struct
Cyc_Absyn_Evar_struct*)_tmp48E)->f1;_tmp4A3=((struct Cyc_Absyn_Evar_struct*)
_tmp48E)->f2;_tmp4A4=((struct Cyc_Absyn_Evar_struct*)_tmp48E)->f3;_LL31D: if(
_tmp4A3 == 0){struct _tuple13 _tmp9A4;return(_tmp9A4.f1=tq,((_tmp9A4.f2=t,((_tmp9A4.f3=
0,_tmp9A4)))));}else{return Cyc_Absynpp_to_tms(r,tq,(void*)_tmp4A3->v);}_LL31E:
if(*((int*)_tmp48E)!= 16)goto _LL320;_tmp4A5=((struct Cyc_Absyn_TypedefType_struct*)
_tmp48E)->f1;_tmp4A6=((struct Cyc_Absyn_TypedefType_struct*)_tmp48E)->f2;_tmp4A7=((
struct Cyc_Absyn_TypedefType_struct*)_tmp48E)->f4;_LL31F: if((_tmp4A7 == 0  || !Cyc_Absynpp_expand_typedefs)
 || Cyc_Absynpp_is_anon_aggrtype(*_tmp4A7)){struct _tuple13 _tmp9A5;return(_tmp9A5.f1=
tq,((_tmp9A5.f2=t,((_tmp9A5.f3=0,_tmp9A5)))));}else{return Cyc_Absynpp_to_tms(r,
tq,*_tmp4A7);}_LL320:;_LL321: {struct _tuple13 _tmp9A6;return(_tmp9A6.f1=tq,((
_tmp9A6.f2=t,((_tmp9A6.f3=0,_tmp9A6)))));}_LL315:;}static int Cyc_Absynpp_is_char_ptr(
void*t);static int Cyc_Absynpp_is_char_ptr(void*t){void*_tmp4D7=t;struct Cyc_Core_Opt*
_tmp4D8;struct Cyc_Core_Opt _tmp4D9;void*_tmp4DA;struct Cyc_Absyn_PtrInfo _tmp4DB;
void*_tmp4DC;_LL331: if(_tmp4D7 <= (void*)4)goto _LL335;if(*((int*)_tmp4D7)!= 0)
goto _LL333;_tmp4D8=((struct Cyc_Absyn_Evar_struct*)_tmp4D7)->f2;if(_tmp4D8 == 0)
goto _LL333;_tmp4D9=*_tmp4D8;_tmp4DA=(void*)_tmp4D9.v;_LL332: return Cyc_Absynpp_is_char_ptr(
_tmp4DA);_LL333: if(*((int*)_tmp4D7)!= 4)goto _LL335;_tmp4DB=((struct Cyc_Absyn_PointerType_struct*)
_tmp4D7)->f1;_tmp4DC=_tmp4DB.elt_typ;_LL334: L: {void*_tmp4DD=_tmp4DC;struct Cyc_Core_Opt*
_tmp4DE;struct Cyc_Core_Opt _tmp4DF;void*_tmp4E0;void**_tmp4E1;void*_tmp4E2;void*
_tmp4E3;_LL338: if(_tmp4DD <= (void*)4)goto _LL33E;if(*((int*)_tmp4DD)!= 0)goto
_LL33A;_tmp4DE=((struct Cyc_Absyn_Evar_struct*)_tmp4DD)->f2;if(_tmp4DE == 0)goto
_LL33A;_tmp4DF=*_tmp4DE;_tmp4E0=(void*)_tmp4DF.v;_LL339: _tmp4DC=_tmp4E0;goto L;
_LL33A: if(*((int*)_tmp4DD)!= 16)goto _LL33C;_tmp4E1=((struct Cyc_Absyn_TypedefType_struct*)
_tmp4DD)->f4;if(_tmp4E1 == 0)goto _LL33C;_tmp4E2=*_tmp4E1;_LL33B: _tmp4DC=_tmp4E2;
goto L;_LL33C: if(*((int*)_tmp4DD)!= 5)goto _LL33E;_tmp4E3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4DD)->f2;if((int)_tmp4E3 != 0)goto _LL33E;_LL33D: return 1;_LL33E:;_LL33F: return
0;_LL337:;}_LL335:;_LL336: return 0;_LL330:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){struct _RegionHandle
_tmp4E4=_new_region("temp");struct _RegionHandle*temp=& _tmp4E4;_push_region(temp);{
struct Cyc_Absyn_Tqual _tmp4E6;void*_tmp4E7;struct Cyc_List_List*_tmp4E8;struct
_tuple13 _tmp4E5=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp4E6=_tmp4E5.f1;_tmp4E7=
_tmp4E5.f2;_tmp4E8=_tmp4E5.f3;_tmp4E8=Cyc_List_imp_rev(_tmp4E8);if(_tmp4E8 == 0
 && dopt == 0){struct Cyc_PP_Doc*_tmp9A7[2];struct Cyc_PP_Doc*_tmp4EA=(_tmp9A7[1]=
Cyc_Absynpp_ntyp2doc(_tmp4E7),((_tmp9A7[0]=Cyc_Absynpp_tqual2doc(_tmp4E6),Cyc_PP_cat(
_tag_dyneither(_tmp9A7,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return
_tmp4EA;}else{const char*_tmp9AA;struct Cyc_PP_Doc*_tmp9A9[4];struct Cyc_PP_Doc*
_tmp4ED=(_tmp9A9[3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?
Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp4E8),((_tmp9A9[2]=Cyc_PP_text(((
_tmp9AA=" ",_tag_dyneither(_tmp9AA,sizeof(char),2)))),((_tmp9A9[1]=Cyc_Absynpp_ntyp2doc(
_tmp4E7),((_tmp9A9[0]=Cyc_Absynpp_tqual2doc(_tmp4E6),Cyc_PP_cat(_tag_dyneither(
_tmp9A9,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp4ED;}};
_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*
f);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){void*
_tmp4EE=Cyc_Cyclone_c_compiler;_LL341: if((int)_tmp4EE != 0)goto _LL343;_LL342: if(f->width
!= 0){const char*_tmp9B1;const char*_tmp9B0;struct Cyc_Core_Opt*_tmp9AF;struct Cyc_PP_Doc*
_tmp9AE[5];return(_tmp9AE[4]=Cyc_PP_text(((_tmp9B1=";",_tag_dyneither(_tmp9B1,
sizeof(char),2)))),((_tmp9AE[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmp9AE[2]=
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmp9AE[1]=
Cyc_PP_text(((_tmp9B0=":",_tag_dyneither(_tmp9B0,sizeof(char),2)))),((_tmp9AE[0]=
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmp9AF=_cycalloc(sizeof(*_tmp9AF)),((
_tmp9AF->v=Cyc_PP_textptr(f->name),_tmp9AF))))),Cyc_PP_cat(_tag_dyneither(
_tmp9AE,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{const char*_tmp9B6;struct Cyc_Core_Opt*
_tmp9B5;struct Cyc_PP_Doc*_tmp9B4[3];return(_tmp9B4[2]=Cyc_PP_text(((_tmp9B6=";",
_tag_dyneither(_tmp9B6,sizeof(char),2)))),((_tmp9B4[1]=Cyc_Absynpp_atts2doc(f->attributes),((
_tmp9B4[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmp9B5=_cycalloc(sizeof(*
_tmp9B5)),((_tmp9B5->v=Cyc_PP_textptr(f->name),_tmp9B5))))),Cyc_PP_cat(
_tag_dyneither(_tmp9B4,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL343: if((int)
_tmp4EE != 1)goto _LL340;_LL344: if(f->width != 0){const char*_tmp9BD;const char*
_tmp9BC;struct Cyc_Core_Opt*_tmp9BB;struct Cyc_PP_Doc*_tmp9BA[5];return(_tmp9BA[4]=
Cyc_PP_text(((_tmp9BD=";",_tag_dyneither(_tmp9BD,sizeof(char),2)))),((_tmp9BA[3]=
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmp9BA[2]=
Cyc_PP_text(((_tmp9BC=":",_tag_dyneither(_tmp9BC,sizeof(char),2)))),((_tmp9BA[1]=
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmp9BB=_cycalloc(sizeof(*_tmp9BB)),((
_tmp9BB->v=Cyc_PP_textptr(f->name),_tmp9BB))))),((_tmp9BA[0]=Cyc_Absynpp_atts2doc(
f->attributes),Cyc_PP_cat(_tag_dyneither(_tmp9BA,sizeof(struct Cyc_PP_Doc*),5)))))))))));}
else{const char*_tmp9C2;struct Cyc_Core_Opt*_tmp9C1;struct Cyc_PP_Doc*_tmp9C0[3];
return(_tmp9C0[2]=Cyc_PP_text(((_tmp9C2=";",_tag_dyneither(_tmp9C2,sizeof(char),
2)))),((_tmp9C0[1]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmp9C1=_cycalloc(
sizeof(*_tmp9C1)),((_tmp9C1->v=Cyc_PP_textptr(f->name),_tmp9C1))))),((_tmp9C0[0]=
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmp9C0,sizeof(
struct Cyc_PP_Doc*),3)))))));}_LL340:;}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct
Cyc_List_List*fields){const char*_tmp9C3;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmp9C3="",_tag_dyneither(_tmp9C3,
sizeof(char),1))),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct
Cyc_Absyn_Datatypefield*f);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct
Cyc_Absyn_Datatypefield*f){struct Cyc_PP_Doc*_tmp9C4[3];return(_tmp9C4[2]=f->typs
== 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmp9C4[1]=Cyc_Absynpp_typedef_name2doc(
f->name),((_tmp9C4[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(
_tmp9C4,sizeof(struct Cyc_PP_Doc*),3)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields){const char*_tmp9C5;return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmp9C5=",",
_tag_dyneither(_tmp9C5,sizeof(char),2))),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{const char*_tmp9C8;
void*_tmp9C7;(_tmp9C7=0,Cyc_fprintf(f,((_tmp9C8="\n",_tag_dyneither(_tmp9C8,
sizeof(char),2))),_tag_dyneither(_tmp9C7,sizeof(void*),0)));}}}struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){const char*
_tmp9C9;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmp9C9="",_tag_dyneither(
_tmp9C9,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
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
