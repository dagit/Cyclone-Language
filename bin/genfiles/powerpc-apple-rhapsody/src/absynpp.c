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
_dyneither_ptr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Subscript_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Struct_e_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*
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
_dyneither_ptr*v){struct Cyc_List_List*_tmp4E7;Cyc_Absynpp_curr_namespace=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_Absynpp_curr_namespace,((_tmp4E7=_cycalloc(sizeof(*_tmp4E7)),((_tmp4E7->hd=v,((
_tmp4E7->tl=0,_tmp4E7)))))));}static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**
l);static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){if(((struct Cyc_List_List*)
_check_null(*l))->tl == 0)*l=0;else{Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)
_check_null(*l))->tl);}}static void Cyc_Absynpp_curr_namespace_drop();static void
Cyc_Absynpp_curr_namespace_drop(){((void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(&
Cyc_Absynpp_curr_namespace);}struct _dyneither_ptr Cyc_Absynpp_char_escape(char c);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c){switch(c){case '\a': _LL0: {
const char*_tmp4E8;return(_tmp4E8="\\a",_tag_dyneither(_tmp4E8,sizeof(char),3));}
case '\b': _LL1: {const char*_tmp4E9;return(_tmp4E9="\\b",_tag_dyneither(_tmp4E9,
sizeof(char),3));}case '\f': _LL2: {const char*_tmp4EA;return(_tmp4EA="\\f",
_tag_dyneither(_tmp4EA,sizeof(char),3));}case '\n': _LL3: {const char*_tmp4EB;
return(_tmp4EB="\\n",_tag_dyneither(_tmp4EB,sizeof(char),3));}case '\r': _LL4: {
const char*_tmp4EC;return(_tmp4EC="\\r",_tag_dyneither(_tmp4EC,sizeof(char),3));}
case '\t': _LL5: {const char*_tmp4ED;return(_tmp4ED="\\t",_tag_dyneither(_tmp4ED,
sizeof(char),3));}case '\v': _LL6: {const char*_tmp4EE;return(_tmp4EE="\\v",
_tag_dyneither(_tmp4EE,sizeof(char),3));}case '\\': _LL7: {const char*_tmp4EF;
return(_tmp4EF="\\\\",_tag_dyneither(_tmp4EF,sizeof(char),3));}case '"': _LL8: {
const char*_tmp4F0;return(_tmp4F0="\"",_tag_dyneither(_tmp4F0,sizeof(char),2));}
case '\'': _LL9: {const char*_tmp4F1;return(_tmp4F1="\\'",_tag_dyneither(_tmp4F1,
sizeof(char),3));}default: _LLA: if(c >= ' '  && c <= '~'){struct _dyneither_ptr _tmpC=
Cyc_Core_new_string(2);{char _tmp4F4;char _tmp4F3;struct _dyneither_ptr _tmp4F2;(
_tmp4F2=_dyneither_ptr_plus(_tmpC,sizeof(char),0),((_tmp4F3=*((char*)
_check_dyneither_subscript(_tmp4F2,sizeof(char),0)),((_tmp4F4=c,((
_get_dyneither_size(_tmp4F2,sizeof(char))== 1  && (_tmp4F3 == '\000'  && _tmp4F4 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp4F2.curr)=_tmp4F4)))))));}return(struct
_dyneither_ptr)_tmpC;}else{struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);int
j=0;{char _tmp4F7;char _tmp4F6;struct _dyneither_ptr _tmp4F5;(_tmp4F5=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp4F6=*((char*)
_check_dyneither_subscript(_tmp4F5,sizeof(char),0)),((_tmp4F7='\\',((
_get_dyneither_size(_tmp4F5,sizeof(char))== 1  && (_tmp4F6 == '\000'  && _tmp4F7 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp4F5.curr)=_tmp4F7)))))));}{char _tmp4FA;char
_tmp4F9;struct _dyneither_ptr _tmp4F8;(_tmp4F8=_dyneither_ptr_plus(_tmp10,sizeof(
char),j ++),((_tmp4F9=*((char*)_check_dyneither_subscript(_tmp4F8,sizeof(char),0)),((
_tmp4FA=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp4F8,
sizeof(char))== 1  && (_tmp4F9 == '\000'  && _tmp4FA != '\000')?_throw_arraybounds():
1,*((char*)_tmp4F8.curr)=_tmp4FA)))))));}{char _tmp4FD;char _tmp4FC;struct
_dyneither_ptr _tmp4FB;(_tmp4FB=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp4FC=*((char*)_check_dyneither_subscript(_tmp4FB,sizeof(char),0)),((_tmp4FD=(
char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp4FB,sizeof(char))== 1  && (
_tmp4FC == '\000'  && _tmp4FD != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4FB.curr)=
_tmp4FD)))))));}{char _tmp500;char _tmp4FF;struct _dyneither_ptr _tmp4FE;(_tmp4FE=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp4FF=*((char*)
_check_dyneither_subscript(_tmp4FE,sizeof(char),0)),((_tmp500=(char)('0' + (c & 7)),((
_get_dyneither_size(_tmp4FE,sizeof(char))== 1  && (_tmp4FF == '\000'  && _tmp500 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp4FE.curr)=_tmp500)))))));}return(struct
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
_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:{char _tmp503;char _tmp502;struct
_dyneither_ptr _tmp501;(_tmp501=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp502=*((
char*)_check_dyneither_subscript(_tmp501,sizeof(char),0)),((_tmp503='\\',((
_get_dyneither_size(_tmp501,sizeof(char))== 1  && (_tmp502 == '\000'  && _tmp503 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp501.curr)=_tmp503)))))));}{char _tmp506;char
_tmp505;struct _dyneither_ptr _tmp504;(_tmp504=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp505=*((char*)_check_dyneither_subscript(_tmp504,sizeof(char),0)),((
_tmp506='a',((_get_dyneither_size(_tmp504,sizeof(char))== 1  && (_tmp505 == '\000'
 && _tmp506 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp504.curr)=_tmp506)))))));}
goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:{char _tmp509;char _tmp508;struct
_dyneither_ptr _tmp507;(_tmp507=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp508=*((
char*)_check_dyneither_subscript(_tmp507,sizeof(char),0)),((_tmp509='\\',((
_get_dyneither_size(_tmp507,sizeof(char))== 1  && (_tmp508 == '\000'  && _tmp509 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp507.curr)=_tmp509)))))));}{char _tmp50C;char
_tmp50B;struct _dyneither_ptr _tmp50A;(_tmp50A=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp50B=*((char*)_check_dyneither_subscript(_tmp50A,sizeof(char),0)),((
_tmp50C='b',((_get_dyneither_size(_tmp50A,sizeof(char))== 1  && (_tmp50B == '\000'
 && _tmp50C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp50A.curr)=_tmp50C)))))));}
goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:{char _tmp50F;char _tmp50E;struct
_dyneither_ptr _tmp50D;(_tmp50D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp50E=*((
char*)_check_dyneither_subscript(_tmp50D,sizeof(char),0)),((_tmp50F='\\',((
_get_dyneither_size(_tmp50D,sizeof(char))== 1  && (_tmp50E == '\000'  && _tmp50F != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp50D.curr)=_tmp50F)))))));}{char _tmp512;char
_tmp511;struct _dyneither_ptr _tmp510;(_tmp510=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp511=*((char*)_check_dyneither_subscript(_tmp510,sizeof(char),0)),((
_tmp512='f',((_get_dyneither_size(_tmp510,sizeof(char))== 1  && (_tmp511 == '\000'
 && _tmp512 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp510.curr)=_tmp512)))))));}
goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:{char _tmp515;char _tmp514;struct
_dyneither_ptr _tmp513;(_tmp513=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp514=*((
char*)_check_dyneither_subscript(_tmp513,sizeof(char),0)),((_tmp515='\\',((
_get_dyneither_size(_tmp513,sizeof(char))== 1  && (_tmp514 == '\000'  && _tmp515 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp513.curr)=_tmp515)))))));}{char _tmp518;char
_tmp517;struct _dyneither_ptr _tmp516;(_tmp516=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp517=*((char*)_check_dyneither_subscript(_tmp516,sizeof(char),0)),((
_tmp518='n',((_get_dyneither_size(_tmp516,sizeof(char))== 1  && (_tmp517 == '\000'
 && _tmp518 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp516.curr)=_tmp518)))))));}
goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:{char _tmp51B;char _tmp51A;struct
_dyneither_ptr _tmp519;(_tmp519=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp51A=*((
char*)_check_dyneither_subscript(_tmp519,sizeof(char),0)),((_tmp51B='\\',((
_get_dyneither_size(_tmp519,sizeof(char))== 1  && (_tmp51A == '\000'  && _tmp51B != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp519.curr)=_tmp51B)))))));}{char _tmp51E;char
_tmp51D;struct _dyneither_ptr _tmp51C;(_tmp51C=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp51D=*((char*)_check_dyneither_subscript(_tmp51C,sizeof(char),0)),((
_tmp51E='r',((_get_dyneither_size(_tmp51C,sizeof(char))== 1  && (_tmp51D == '\000'
 && _tmp51E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp51C.curr)=_tmp51E)))))));}
goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:{char _tmp521;char _tmp520;struct
_dyneither_ptr _tmp51F;(_tmp51F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp520=*((
char*)_check_dyneither_subscript(_tmp51F,sizeof(char),0)),((_tmp521='\\',((
_get_dyneither_size(_tmp51F,sizeof(char))== 1  && (_tmp520 == '\000'  && _tmp521 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp51F.curr)=_tmp521)))))));}{char _tmp524;char
_tmp523;struct _dyneither_ptr _tmp522;(_tmp522=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp523=*((char*)_check_dyneither_subscript(_tmp522,sizeof(char),0)),((
_tmp524='t',((_get_dyneither_size(_tmp522,sizeof(char))== 1  && (_tmp523 == '\000'
 && _tmp524 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp522.curr)=_tmp524)))))));}
goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:{char _tmp527;char _tmp526;struct
_dyneither_ptr _tmp525;(_tmp525=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp526=*((
char*)_check_dyneither_subscript(_tmp525,sizeof(char),0)),((_tmp527='\\',((
_get_dyneither_size(_tmp525,sizeof(char))== 1  && (_tmp526 == '\000'  && _tmp527 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp525.curr)=_tmp527)))))));}{char _tmp52A;char
_tmp529;struct _dyneither_ptr _tmp528;(_tmp528=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp529=*((char*)_check_dyneither_subscript(_tmp528,sizeof(char),0)),((
_tmp52A='v',((_get_dyneither_size(_tmp528,sizeof(char))== 1  && (_tmp529 == '\000'
 && _tmp52A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp528.curr)=_tmp52A)))))));}
goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:{char _tmp52D;char _tmp52C;struct
_dyneither_ptr _tmp52B;(_tmp52B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp52C=*((
char*)_check_dyneither_subscript(_tmp52B,sizeof(char),0)),((_tmp52D='\\',((
_get_dyneither_size(_tmp52B,sizeof(char))== 1  && (_tmp52C == '\000'  && _tmp52D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp52B.curr)=_tmp52D)))))));}{char _tmp530;char
_tmp52F;struct _dyneither_ptr _tmp52E;(_tmp52E=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp52F=*((char*)_check_dyneither_subscript(_tmp52E,sizeof(char),0)),((
_tmp530='\\',((_get_dyneither_size(_tmp52E,sizeof(char))== 1  && (_tmp52F == '\000'
 && _tmp530 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp52E.curr)=_tmp530)))))));}
goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:{char _tmp533;char _tmp532;struct
_dyneither_ptr _tmp531;(_tmp531=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp532=*((
char*)_check_dyneither_subscript(_tmp531,sizeof(char),0)),((_tmp533='\\',((
_get_dyneither_size(_tmp531,sizeof(char))== 1  && (_tmp532 == '\000'  && _tmp533 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp531.curr)=_tmp533)))))));}{char _tmp536;char
_tmp535;struct _dyneither_ptr _tmp534;(_tmp534=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp535=*((char*)_check_dyneither_subscript(_tmp534,sizeof(char),0)),((
_tmp536='"',((_get_dyneither_size(_tmp534,sizeof(char))== 1  && (_tmp535 == '\000'
 && _tmp536 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp534.curr)=_tmp536)))))));}
goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp539;char _tmp538;
struct _dyneither_ptr _tmp537;(_tmp537=_dyneither_ptr_plus(t,sizeof(char),j ++),((
_tmp538=*((char*)_check_dyneither_subscript(_tmp537,sizeof(char),0)),((_tmp539=
_tmp1E,((_get_dyneither_size(_tmp537,sizeof(char))== 1  && (_tmp538 == '\000'  && 
_tmp539 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp537.curr)=_tmp539)))))));}
else{{char _tmp53C;char _tmp53B;struct _dyneither_ptr _tmp53A;(_tmp53A=
_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp53B=*((char*)
_check_dyneither_subscript(_tmp53A,sizeof(char),0)),((_tmp53C='\\',((
_get_dyneither_size(_tmp53A,sizeof(char))== 1  && (_tmp53B == '\000'  && _tmp53C != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp53A.curr)=_tmp53C)))))));}{char _tmp53F;char
_tmp53E;struct _dyneither_ptr _tmp53D;(_tmp53D=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp53E=*((char*)_check_dyneither_subscript(_tmp53D,sizeof(char),0)),((
_tmp53F=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dyneither_size(_tmp53D,sizeof(char))
== 1  && (_tmp53E == '\000'  && _tmp53F != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp53D.curr)=_tmp53F)))))));}{char _tmp542;char _tmp541;struct _dyneither_ptr
_tmp540;(_tmp540=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp541=*((char*)
_check_dyneither_subscript(_tmp540,sizeof(char),0)),((_tmp542=(char)('0' + (
_tmp1E >> 3 & 7)),((_get_dyneither_size(_tmp540,sizeof(char))== 1  && (_tmp541 == '\000'
 && _tmp542 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp540.curr)=_tmp542)))))));}{
char _tmp545;char _tmp544;struct _dyneither_ptr _tmp543;(_tmp543=_dyneither_ptr_plus(
t,sizeof(char),j ++),((_tmp544=*((char*)_check_dyneither_subscript(_tmp543,
sizeof(char),0)),((_tmp545=(char)('0' + (_tmp1E & 7)),((_get_dyneither_size(
_tmp543,sizeof(char))== 1  && (_tmp544 == '\000'  && _tmp545 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp543.curr)=_tmp545)))))));}}goto _LL21;_LL21:;}}
return(struct _dyneither_ptr)t;}}}}static char _tmp64[9]="restrict";static struct
_dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char
_tmp65[9]="volatile";static struct _dyneither_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dyneither_ptr
Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string;static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq);struct Cyc_PP_Doc*
Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=0;if(tq.q_restrict){
struct Cyc_List_List*_tmp546;l=((_tmp546=_cycalloc(sizeof(*_tmp546)),((_tmp546->hd=
Cyc_Absynpp_restrict_sp,((_tmp546->tl=l,_tmp546))))));}if(tq.q_volatile){struct
Cyc_List_List*_tmp547;l=((_tmp547=_cycalloc(sizeof(*_tmp547)),((_tmp547->hd=Cyc_Absynpp_volatile_sp,((
_tmp547->tl=l,_tmp547))))));}if(tq.print_const){struct Cyc_List_List*_tmp548;l=((
_tmp548=_cycalloc(sizeof(*_tmp548)),((_tmp548->hd=Cyc_Absynpp_const_sp,((_tmp548->tl=
l,_tmp548))))));}{const char*_tmp54B;const char*_tmp54A;const char*_tmp549;return
Cyc_PP_egroup(((_tmp549="",_tag_dyneither(_tmp549,sizeof(char),1))),((_tmp54A=" ",
_tag_dyneither(_tmp54A,sizeof(char),2))),((_tmp54B=" ",_tag_dyneither(_tmp54B,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));}}
struct _dyneither_ptr Cyc_Absynpp_kind2string(void*k);struct _dyneither_ptr Cyc_Absynpp_kind2string(
void*k){void*_tmp6D=k;_LL37: if((int)_tmp6D != 0)goto _LL39;_LL38: {const char*
_tmp54C;return(_tmp54C="A",_tag_dyneither(_tmp54C,sizeof(char),2));}_LL39: if((
int)_tmp6D != 1)goto _LL3B;_LL3A: {const char*_tmp54D;return(_tmp54D="M",
_tag_dyneither(_tmp54D,sizeof(char),2));}_LL3B: if((int)_tmp6D != 2)goto _LL3D;
_LL3C: {const char*_tmp54E;return(_tmp54E="B",_tag_dyneither(_tmp54E,sizeof(char),
2));}_LL3D: if((int)_tmp6D != 3)goto _LL3F;_LL3E: {const char*_tmp54F;return(_tmp54F="R",
_tag_dyneither(_tmp54F,sizeof(char),2));}_LL3F: if((int)_tmp6D != 4)goto _LL41;
_LL40: {const char*_tmp550;return(_tmp550="UR",_tag_dyneither(_tmp550,sizeof(char),
3));}_LL41: if((int)_tmp6D != 5)goto _LL43;_LL42: {const char*_tmp551;return(_tmp551="TR",
_tag_dyneither(_tmp551,sizeof(char),3));}_LL43: if((int)_tmp6D != 6)goto _LL45;
_LL44: {const char*_tmp552;return(_tmp552="E",_tag_dyneither(_tmp552,sizeof(char),
2));}_LL45: if((int)_tmp6D != 7)goto _LL36;_LL46: {const char*_tmp553;return(_tmp553="I",
_tag_dyneither(_tmp553,sizeof(char),2));}_LL36:;}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(
void*k);struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(void*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(
k));}struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c);struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp76=Cyc_Absyn_compress_kb(
c);void*_tmp77;void*_tmp78;_LL48: if(*((int*)_tmp76)!= 0)goto _LL4A;_tmp77=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp76)->f1;_LL49: return Cyc_Absynpp_kind2string(
_tmp77);_LL4A: if(*((int*)_tmp76)!= 1)goto _LL4C;_LL4B: if(Cyc_PP_tex_output){const
char*_tmp554;return(_tmp554="{?}",_tag_dyneither(_tmp554,sizeof(char),4));}else{
const char*_tmp555;return(_tmp555="?",_tag_dyneither(_tmp555,sizeof(char),2));}
_LL4C: if(*((int*)_tmp76)!= 2)goto _LL47;_tmp78=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp76)->f2;_LL4D: return Cyc_Absynpp_kind2string(_tmp78);_LL47:;}struct Cyc_PP_Doc*
Cyc_Absynpp_kindbound2doc(void*c);struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(
void*c){void*_tmp7B=Cyc_Absyn_compress_kb(c);void*_tmp7C;void*_tmp7D;_LL4F: if(*((
int*)_tmp7B)!= 0)goto _LL51;_tmp7C=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp7B)->f1;
_LL50: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7C));_LL51: if(*((int*)_tmp7B)
!= 1)goto _LL53;_LL52: if(Cyc_PP_tex_output){const char*_tmp556;return Cyc_PP_text_width(((
_tmp556="{?}",_tag_dyneither(_tmp556,sizeof(char),4))),1);}else{const char*
_tmp557;return Cyc_PP_text(((_tmp557="?",_tag_dyneither(_tmp557,sizeof(char),2))));}
_LL53: if(*((int*)_tmp7B)!= 2)goto _LL4E;_tmp7D=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp7B)->f2;_LL54: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp7D));_LL4E:;}
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts);struct Cyc_PP_Doc*
Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){const char*_tmp55A;const char*_tmp559;
const char*_tmp558;return Cyc_PP_egroup(((_tmp558="<",_tag_dyneither(_tmp558,
sizeof(char),2))),((_tmp559=">",_tag_dyneither(_tmp559,sizeof(char),2))),((
_tmp55A=",",_tag_dyneither(_tmp55A,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv);struct Cyc_PP_Doc*
Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){void*_tmp83=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmp84;void*_tmp85;void*_tmp86;_LL56: if(*((int*)_tmp83)!= 1)
goto _LL58;_LL57: goto _LL59;_LL58: if(*((int*)_tmp83)!= 0)goto _LL5A;_tmp84=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp83)->f1;if((int)_tmp84 != 2)goto _LL5A;_LL59:
return Cyc_PP_textptr(tv->name);_LL5A: if(*((int*)_tmp83)!= 2)goto _LL5C;_tmp85=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp83)->f2;_LL5B: _tmp86=_tmp85;goto
_LL5D;_LL5C: if(*((int*)_tmp83)!= 0)goto _LL55;_tmp86=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp83)->f1;_LL5D: {const char*_tmp55D;struct Cyc_PP_Doc*_tmp55C[3];return(_tmp55C[
2]=Cyc_Absynpp_kind2doc(_tmp86),((_tmp55C[1]=Cyc_PP_text(((_tmp55D="::",
_tag_dyneither(_tmp55D,sizeof(char),3)))),((_tmp55C[0]=Cyc_PP_textptr(tv->name),
Cyc_PP_cat(_tag_dyneither(_tmp55C,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL55:;}
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs);struct Cyc_PP_Doc*
Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){const char*_tmp560;const char*
_tmp55F;const char*_tmp55E;return Cyc_PP_egroup(((_tmp55E="<",_tag_dyneither(
_tmp55E,sizeof(char),2))),((_tmp55F=">",_tag_dyneither(_tmp55F,sizeof(char),2))),((
_tmp560=",",_tag_dyneither(_tmp560,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_ktvar2doc,tvs));}static struct _dyneither_ptr*Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar*tv);static struct _dyneither_ptr*Cyc_Absynpp_get_name(struct
Cyc_Absyn_Tvar*tv){return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct
Cyc_List_List*tvs);struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*
tvs){if(Cyc_Absynpp_print_all_kinds)return Cyc_Absynpp_ktvars2doc(tvs);{const char*
_tmp563;const char*_tmp562;const char*_tmp561;return Cyc_PP_egroup(((_tmp561="<",
_tag_dyneither(_tmp561,sizeof(char),2))),((_tmp562=">",_tag_dyneither(_tmp562,
sizeof(char),2))),((_tmp563=",",_tag_dyneither(_tmp563,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
tvs)));}}struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(
struct _tuple10*t);struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple10*t){return
Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(
struct Cyc_List_List*ts);struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*
ts){const char*_tmp566;const char*_tmp565;const char*_tmp564;return Cyc_PP_group(((
_tmp564="(",_tag_dyneither(_tmp564,sizeof(char),2))),((_tmp565=")",
_tag_dyneither(_tmp565,sizeof(char),2))),((_tmp566=",",_tag_dyneither(_tmp566,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple10*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}struct Cyc_PP_Doc*
Cyc_Absynpp_noncallatt2doc(void*att);struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(
void*att){void*_tmp92=att;_LL5F: if((int)_tmp92 != 0)goto _LL61;_LL60: goto _LL62;
_LL61: if((int)_tmp92 != 1)goto _LL63;_LL62: goto _LL64;_LL63: if((int)_tmp92 != 2)goto
_LL65;_LL64: return Cyc_PP_nil_doc();_LL65:;_LL66: return Cyc_PP_text(Cyc_Absyn_attribute2string(
att));_LL5E:;}struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts);
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){for(0;atts != 
0;atts=atts->tl){void*_tmp93=(void*)atts->hd;_LL68: if((int)_tmp93 != 0)goto _LL6A;
_LL69: {const char*_tmp567;return Cyc_PP_text(((_tmp567=" _stdcall ",
_tag_dyneither(_tmp567,sizeof(char),11))));}_LL6A: if((int)_tmp93 != 1)goto _LL6C;
_LL6B: {const char*_tmp568;return Cyc_PP_text(((_tmp568=" _cdecl ",_tag_dyneither(
_tmp568,sizeof(char),9))));}_LL6C: if((int)_tmp93 != 2)goto _LL6E;_LL6D: {const char*
_tmp569;return Cyc_PP_text(((_tmp569=" _fastcall ",_tag_dyneither(_tmp569,sizeof(
char),12))));}_LL6E:;_LL6F: goto _LL67;_LL67:;}return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*
Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts);struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(
struct Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;
atts2 != 0;atts2=atts2->tl){void*_tmp97=(void*)atts2->hd;_LL71: if((int)_tmp97 != 0)
goto _LL73;_LL72: goto _LL74;_LL73: if((int)_tmp97 != 1)goto _LL75;_LL74: goto _LL76;
_LL75: if((int)_tmp97 != 2)goto _LL77;_LL76: goto _LL70;_LL77:;_LL78: hasatt=1;goto
_LL70;_LL70:;}}if(!hasatt)return Cyc_PP_nil_doc();{const char*_tmp574;const char*
_tmp573;const char*_tmp572;const char*_tmp571;const char*_tmp570;struct Cyc_PP_Doc*
_tmp56F[3];return(_tmp56F[2]=Cyc_PP_text(((_tmp574=")",_tag_dyneither(_tmp574,
sizeof(char),2)))),((_tmp56F[1]=Cyc_PP_group(((_tmp573="",_tag_dyneither(_tmp573,
sizeof(char),1))),((_tmp572="",_tag_dyneither(_tmp572,sizeof(char),1))),((
_tmp571=" ",_tag_dyneither(_tmp571,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,
atts)),((_tmp56F[0]=Cyc_PP_text(((_tmp570=" __declspec(",_tag_dyneither(_tmp570,
sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp56F,sizeof(struct Cyc_PP_Doc*),
3)))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a);struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(
void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts);struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*
atts){if(atts == 0)return Cyc_PP_nil_doc();{void*_tmp9E=Cyc_Cyclone_c_compiler;
_LL7A: if((int)_tmp9E != 1)goto _LL7C;_LL7B: return Cyc_Absynpp_noncallconv2doc(atts);
_LL7C: if((int)_tmp9E != 0)goto _LL79;_LL7D: {const char*_tmp57D;const char*_tmp57C;
const char*_tmp57B;const char*_tmp57A;struct Cyc_PP_Doc*_tmp579[2];return(_tmp579[1]=
Cyc_PP_group(((_tmp57D="((",_tag_dyneither(_tmp57D,sizeof(char),3))),((_tmp57C="))",
_tag_dyneither(_tmp57C,sizeof(char),3))),((_tmp57B=",",_tag_dyneither(_tmp57B,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp579[0]=Cyc_PP_text(((
_tmp57A=" __attribute__",_tag_dyneither(_tmp57A,sizeof(char),15)))),Cyc_PP_cat(
_tag_dyneither(_tmp579,sizeof(struct Cyc_PP_Doc*),2)))));}_LL79:;}}int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List*tms);int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{void*_tmpA4=(void*)tms->hd;_LL7F: if(*((int*)_tmpA4)!= 2)goto
_LL81;_LL80: return 1;_LL81: if(*((int*)_tmpA4)!= 5)goto _LL83;_LL82: {void*_tmpA5=
Cyc_Cyclone_c_compiler;_LL86: if((int)_tmpA5 != 0)goto _LL88;_LL87: return 0;_LL88:;
_LL89: return Cyc_Absynpp_next_is_pointer(tms->tl);_LL85:;}_LL83:;_LL84: return 0;
_LL7E:;}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);static struct Cyc_PP_Doc*
Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*Cyc_Absynpp_question();
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output){const char*_tmp57E;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp57E="{?}",_tag_dyneither(_tmp57E,sizeof(char),4))),1);}
else{const char*_tmp57F;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp57F="?",_tag_dyneither(_tmp57F,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_lb();static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){if(Cyc_PP_tex_output){const char*
_tmp580;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp580="{\\lb}",
_tag_dyneither(_tmp580,sizeof(char),6))),1);}else{const char*_tmp581;Cyc_Absynpp_cache_lb=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp581="{",_tag_dyneither(_tmp581,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}static struct
Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb();static
struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){if(
Cyc_PP_tex_output){const char*_tmp582;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp582="{\\rb}",_tag_dyneither(_tmp582,sizeof(char),6))),1);}else{const char*
_tmp583;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp583="}",
_tag_dyneither(_tmp583,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;static
struct Cyc_PP_Doc*Cyc_Absynpp_dollar();static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){if(Cyc_PP_tex_output){const char*
_tmp584;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp584="\\$",
_tag_dyneither(_tmp584,sizeof(char),3))),1);}else{const char*_tmp585;Cyc_Absynpp_cache_dollar=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp585="$",_tag_dyneither(_tmp585,sizeof(char),
2))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}struct
Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss);struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*
ss){struct Cyc_PP_Doc*_tmp586[3];return(_tmp586[2]=Cyc_Absynpp_rb(),((_tmp586[1]=
Cyc_PP_seq(sep,ss),((_tmp586[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(
_tmp586,sizeof(struct Cyc_PP_Doc*),3)))))));}static void Cyc_Absynpp_print_tms(
struct Cyc_List_List*tms);static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){void*_tmpAF=(void*)tms->hd;void*_tmpB0;struct Cyc_List_List*
_tmpB1;_LL8B: if(*((int*)_tmpAF)!= 0)goto _LL8D;_LL8C:{const char*_tmp589;void*
_tmp588;(_tmp588=0,Cyc_fprintf(Cyc_stderr,((_tmp589="Carray_mod ",_tag_dyneither(
_tmp589,sizeof(char),12))),_tag_dyneither(_tmp588,sizeof(void*),0)));}goto _LL8A;
_LL8D: if(*((int*)_tmpAF)!= 1)goto _LL8F;_LL8E:{const char*_tmp58C;void*_tmp58B;(
_tmp58B=0,Cyc_fprintf(Cyc_stderr,((_tmp58C="ConstArray_mod ",_tag_dyneither(
_tmp58C,sizeof(char),16))),_tag_dyneither(_tmp58B,sizeof(void*),0)));}goto _LL8A;
_LL8F: if(*((int*)_tmpAF)!= 3)goto _LL91;_tmpB0=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpAF)->f1;if(*((int*)_tmpB0)!= 1)goto _LL91;_tmpB1=((struct Cyc_Absyn_WithTypes_struct*)
_tmpB0)->f1;_LL90:{const char*_tmp58F;void*_tmp58E;(_tmp58E=0,Cyc_fprintf(Cyc_stderr,((
_tmp58F="Function_mod(",_tag_dyneither(_tmp58F,sizeof(char),14))),_tag_dyneither(
_tmp58E,sizeof(void*),0)));}for(0;_tmpB1 != 0;_tmpB1=_tmpB1->tl){struct Cyc_Core_Opt*
_tmpB8=(*((struct _tuple1*)_tmpB1->hd)).f1;if(_tmpB8 == 0){const char*_tmp592;void*
_tmp591;(_tmp591=0,Cyc_fprintf(Cyc_stderr,((_tmp592="?",_tag_dyneither(_tmp592,
sizeof(char),2))),_tag_dyneither(_tmp591,sizeof(void*),0)));}else{void*_tmp593;(
_tmp593=0,Cyc_fprintf(Cyc_stderr,*((struct _dyneither_ptr*)_tmpB8->v),
_tag_dyneither(_tmp593,sizeof(void*),0)));}if(_tmpB1->tl != 0){const char*_tmp596;
void*_tmp595;(_tmp595=0,Cyc_fprintf(Cyc_stderr,((_tmp596=",",_tag_dyneither(
_tmp596,sizeof(char),2))),_tag_dyneither(_tmp595,sizeof(void*),0)));}}{const char*
_tmp599;void*_tmp598;(_tmp598=0,Cyc_fprintf(Cyc_stderr,((_tmp599=") ",
_tag_dyneither(_tmp599,sizeof(char),3))),_tag_dyneither(_tmp598,sizeof(void*),0)));}
goto _LL8A;_LL91: if(*((int*)_tmpAF)!= 3)goto _LL93;_LL92:{const char*_tmp59C;void*
_tmp59B;(_tmp59B=0,Cyc_fprintf(Cyc_stderr,((_tmp59C="Function_mod()",
_tag_dyneither(_tmp59C,sizeof(char),15))),_tag_dyneither(_tmp59B,sizeof(void*),0)));}
goto _LL8A;_LL93: if(*((int*)_tmpAF)!= 5)goto _LL95;_LL94:{const char*_tmp59F;void*
_tmp59E;(_tmp59E=0,Cyc_fprintf(Cyc_stderr,((_tmp59F="Attributes_mod ",
_tag_dyneither(_tmp59F,sizeof(char),16))),_tag_dyneither(_tmp59E,sizeof(void*),0)));}
goto _LL8A;_LL95: if(*((int*)_tmpAF)!= 4)goto _LL97;_LL96:{const char*_tmp5A2;void*
_tmp5A1;(_tmp5A1=0,Cyc_fprintf(Cyc_stderr,((_tmp5A2="TypeParams_mod ",
_tag_dyneither(_tmp5A2,sizeof(char),16))),_tag_dyneither(_tmp5A1,sizeof(void*),0)));}
goto _LL8A;_LL97: if(*((int*)_tmpAF)!= 2)goto _LL8A;_LL98:{const char*_tmp5A5;void*
_tmp5A4;(_tmp5A4=0,Cyc_fprintf(Cyc_stderr,((_tmp5A5="Pointer_mod ",
_tag_dyneither(_tmp5A5,sizeof(char),13))),_tag_dyneither(_tmp5A4,sizeof(void*),0)));}
goto _LL8A;_LL8A:;}{const char*_tmp5A8;void*_tmp5A7;(_tmp5A7=0,Cyc_fprintf(Cyc_stderr,((
_tmp5A8="\n",_tag_dyneither(_tmp5A8,sizeof(char),2))),_tag_dyneither(_tmp5A7,
sizeof(void*),0)));}}struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct
Cyc_PP_Doc*d,struct Cyc_List_List*tms);struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int
is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);const char*_tmp5AD;const
char*_tmp5AC;struct Cyc_PP_Doc*_tmp5AB[3];struct Cyc_PP_Doc*p_rest=(_tmp5AB[2]=Cyc_PP_text(((
_tmp5AD=")",_tag_dyneither(_tmp5AD,sizeof(char),2)))),((_tmp5AB[1]=rest,((
_tmp5AB[0]=Cyc_PP_text(((_tmp5AC="(",_tag_dyneither(_tmp5AC,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp5AB,sizeof(struct Cyc_PP_Doc*),3)))))));void*_tmpCA=(
void*)tms->hd;struct Cyc_Absyn_Conref*_tmpCB;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Conref*
_tmpCD;void*_tmpCE;struct Cyc_List_List*_tmpCF;struct Cyc_List_List*_tmpD0;struct
Cyc_Position_Segment*_tmpD1;int _tmpD2;struct Cyc_Absyn_PtrAtts _tmpD3;void*_tmpD4;
struct Cyc_Absyn_Conref*_tmpD5;struct Cyc_Absyn_Conref*_tmpD6;struct Cyc_Absyn_Conref*
_tmpD7;struct Cyc_Absyn_Tqual _tmpD8;_LL9A: if(*((int*)_tmpCA)!= 0)goto _LL9C;_tmpCB=((
struct Cyc_Absyn_Carray_mod_struct*)_tmpCA)->f1;_LL9B: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{const char*_tmp5B2;const char*_tmp5B1;struct Cyc_PP_Doc*
_tmp5B0[2];return(_tmp5B0[1]=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpCB)?Cyc_PP_text(((_tmp5B1="[]@zeroterm",_tag_dyneither(_tmp5B1,sizeof(char),
12)))): Cyc_PP_text(((_tmp5B2="[]",_tag_dyneither(_tmp5B2,sizeof(char),3)))),((
_tmp5B0[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp5B0,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL9C: if(*((int*)_tmpCA)!= 1)goto _LL9E;_tmpCC=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpCA)->f1;_tmpCD=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCA)->f2;_LL9D:
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{const char*_tmp5B9;const char*
_tmp5B8;const char*_tmp5B7;struct Cyc_PP_Doc*_tmp5B6[4];return(_tmp5B6[3]=((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpCD)?Cyc_PP_text(((
_tmp5B8="]@zeroterm",_tag_dyneither(_tmp5B8,sizeof(char),11)))): Cyc_PP_text(((
_tmp5B9="]",_tag_dyneither(_tmp5B9,sizeof(char),2)))),((_tmp5B6[2]=Cyc_Absynpp_exp2doc(
_tmpCC),((_tmp5B6[1]=Cyc_PP_text(((_tmp5B7="[",_tag_dyneither(_tmp5B7,sizeof(
char),2)))),((_tmp5B6[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp5B6,sizeof(struct Cyc_PP_Doc*),
4)))))))));}_LL9E: if(*((int*)_tmpCA)!= 3)goto _LLA0;_tmpCE=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpCA)->f1;_LL9F: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{void*
_tmpE0=_tmpCE;struct Cyc_List_List*_tmpE1;int _tmpE2;struct Cyc_Absyn_VarargInfo*
_tmpE3;struct Cyc_Core_Opt*_tmpE4;struct Cyc_List_List*_tmpE5;struct Cyc_List_List*
_tmpE6;struct Cyc_Position_Segment*_tmpE7;_LLA7: if(*((int*)_tmpE0)!= 1)goto _LLA9;
_tmpE1=((struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f1;_tmpE2=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE0)->f2;_tmpE3=((struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f3;_tmpE4=((
struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f4;_tmpE5=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE0)->f5;_LLA8: {struct Cyc_PP_Doc*_tmp5BA[2];return(_tmp5BA[1]=Cyc_Absynpp_funargs2doc(
_tmpE1,_tmpE2,_tmpE3,_tmpE4,_tmpE5),((_tmp5BA[0]=rest,Cyc_PP_cat(_tag_dyneither(
_tmp5BA,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA9: if(*((int*)_tmpE0)!= 0)goto _LLA6;
_tmpE6=((struct Cyc_Absyn_NoTypes_struct*)_tmpE0)->f1;_tmpE7=((struct Cyc_Absyn_NoTypes_struct*)
_tmpE0)->f2;_LLAA: {const char*_tmp5C1;const char*_tmp5C0;const char*_tmp5BF;struct
Cyc_PP_Doc*_tmp5BE[2];return(_tmp5BE[1]=Cyc_PP_group(((_tmp5C1="(",
_tag_dyneither(_tmp5C1,sizeof(char),2))),((_tmp5C0=")",_tag_dyneither(_tmp5C0,
sizeof(char),2))),((_tmp5BF=",",_tag_dyneither(_tmp5BF,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,_tmpE6)),((_tmp5BE[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp5BE,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA6:;}_LLA0: if(*((int*)
_tmpCA)!= 5)goto _LLA2;_tmpCF=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpCA)->f2;
_LLA1: {void*_tmpED=Cyc_Cyclone_c_compiler;_LLAC: if((int)_tmpED != 0)goto _LLAE;
_LLAD: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{struct Cyc_PP_Doc*
_tmp5C2[2];return(_tmp5C2[1]=Cyc_Absynpp_atts2doc(_tmpCF),((_tmp5C2[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp5C2,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAE: if((int)_tmpED != 
1)goto _LLAB;_LLAF: if(Cyc_Absynpp_next_is_pointer(tms->tl)){struct Cyc_PP_Doc*
_tmp5C3[2];return(_tmp5C3[1]=rest,((_tmp5C3[0]=Cyc_Absynpp_callconv2doc(_tmpCF),
Cyc_PP_cat(_tag_dyneither(_tmp5C3,sizeof(struct Cyc_PP_Doc*),2)))));}return rest;
_LLAB:;}_LLA2: if(*((int*)_tmpCA)!= 4)goto _LLA4;_tmpD0=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpCA)->f1;_tmpD1=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCA)->f2;_tmpD2=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCA)->f3;_LLA3: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;if(_tmpD2){struct Cyc_PP_Doc*_tmp5C4[2];return(_tmp5C4[1]=
Cyc_Absynpp_ktvars2doc(_tmpD0),((_tmp5C4[0]=rest,Cyc_PP_cat(_tag_dyneither(
_tmp5C4,sizeof(struct Cyc_PP_Doc*),2)))));}else{struct Cyc_PP_Doc*_tmp5C5[2];
return(_tmp5C5[1]=Cyc_Absynpp_tvars2doc(_tmpD0),((_tmp5C5[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp5C5,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA4: if(*((int*)_tmpCA)
!= 2)goto _LL99;_tmpD3=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCA)->f1;_tmpD4=(
void*)_tmpD3.rgn;_tmpD5=_tmpD3.nullable;_tmpD6=_tmpD3.bounds;_tmpD7=_tmpD3.zero_term;
_tmpD8=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCA)->f2;_LLA5: {struct Cyc_PP_Doc*
ptr;{void*_tmpF2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmpD6);struct Cyc_Absyn_Exp*
_tmpF3;_LLB1: if((int)_tmpF2 != 0)goto _LLB3;_LLB2: ptr=Cyc_Absynpp_question();goto
_LLB0;_LLB3: if(_tmpF2 <= (void*)1)goto _LLB0;if(*((int*)_tmpF2)!= 0)goto _LLB0;
_tmpF3=((struct Cyc_Absyn_Upper_b_struct*)_tmpF2)->f1;_LLB4:{const char*_tmp5C7;
const char*_tmp5C6;ptr=Cyc_PP_text(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmpD5)?(_tmp5C7="*",_tag_dyneither(_tmp5C7,sizeof(char),2)):((_tmp5C6="@",
_tag_dyneither(_tmp5C6,sizeof(char),2))));}{unsigned int _tmpF7;int _tmpF8;struct
_tuple4 _tmpF6=Cyc_Evexp_eval_const_uint_exp(_tmpF3);_tmpF7=_tmpF6.f1;_tmpF8=
_tmpF6.f2;if(!_tmpF8  || _tmpF7 != 1){struct Cyc_PP_Doc*_tmp5C8[4];ptr=((_tmp5C8[3]=
Cyc_Absynpp_rb(),((_tmp5C8[2]=Cyc_Absynpp_exp2doc(_tmpF3),((_tmp5C8[1]=Cyc_Absynpp_lb(),((
_tmp5C8[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp5C8,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB0;}_LLB0:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpD7)){const char*_tmp5CB;
struct Cyc_PP_Doc*_tmp5CA[2];ptr=((_tmp5CA[1]=Cyc_PP_text(((_tmp5CB="@zeroterm ",
_tag_dyneither(_tmp5CB,sizeof(char),11)))),((_tmp5CA[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp5CA,sizeof(struct Cyc_PP_Doc*),2))))));}else{if(is_char_ptr
 && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpD7)){
const char*_tmp5CE;struct Cyc_PP_Doc*_tmp5CD[2];ptr=((_tmp5CD[1]=Cyc_PP_text(((
_tmp5CE="@nozeroterm ",_tag_dyneither(_tmp5CE,sizeof(char),13)))),((_tmp5CD[0]=
ptr,Cyc_PP_cat(_tag_dyneither(_tmp5CD,sizeof(struct Cyc_PP_Doc*),2))))));}}}{void*
_tmpFE=Cyc_Tcutil_compress(_tmpD4);_LLB6: if((int)_tmpFE != 2)goto _LLB8;_LLB7: goto
_LLB5;_LLB8:;_LLB9: {const char*_tmp5D1;struct Cyc_PP_Doc*_tmp5D0[3];ptr=((_tmp5D0[
2]=Cyc_PP_text(((_tmp5D1=" ",_tag_dyneither(_tmp5D1,sizeof(char),2)))),((_tmp5D0[
1]=Cyc_Absynpp_typ2doc(_tmpD4),((_tmp5D0[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp5D0,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}{struct Cyc_PP_Doc*_tmp5D2[2];
ptr=((_tmp5D2[1]=Cyc_Absynpp_tqual2doc(_tmpD8),((_tmp5D2[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp5D2,sizeof(struct Cyc_PP_Doc*),2))))));}{struct Cyc_PP_Doc*
_tmp5D3[2];return(_tmp5D3[1]=rest,((_tmp5D3[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp5D3,sizeof(struct Cyc_PP_Doc*),2)))));}}_LL99:;}}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){void*_tmp106=Cyc_Tcutil_compress(
t);_LLBB: if((int)_tmp106 != 2)goto _LLBD;_LLBC: {const char*_tmp5D4;return Cyc_PP_text(((
_tmp5D4="`H",_tag_dyneither(_tmp5D4,sizeof(char),3))));}_LLBD: if((int)_tmp106 != 
3)goto _LLBF;_LLBE: {const char*_tmp5D5;return Cyc_PP_text(((_tmp5D5="`U",
_tag_dyneither(_tmp5D5,sizeof(char),3))));}_LLBF:;_LLC0: return Cyc_Absynpp_ntyp2doc(
t);_LLBA:;}static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct
Cyc_List_List**effects,void*t);static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**
rgions,struct Cyc_List_List**effects,void*t){void*_tmp109=Cyc_Tcutil_compress(t);
void*_tmp10A;struct Cyc_List_List*_tmp10B;_LLC2: if(_tmp109 <= (void*)4)goto _LLC6;
if(*((int*)_tmp109)!= 19)goto _LLC4;_tmp10A=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp109)->f1;_LLC3:{struct Cyc_List_List*_tmp5D6;*rgions=((_tmp5D6=_cycalloc(
sizeof(*_tmp5D6)),((_tmp5D6->hd=Cyc_Absynpp_rgn2doc(_tmp10A),((_tmp5D6->tl=*
rgions,_tmp5D6))))));}goto _LLC1;_LLC4: if(*((int*)_tmp109)!= 20)goto _LLC6;_tmp10B=((
struct Cyc_Absyn_JoinEff_struct*)_tmp109)->f1;_LLC5: for(0;_tmp10B != 0;_tmp10B=
_tmp10B->tl){Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp10B->hd);}goto
_LLC1;_LLC6:;_LLC7:{struct Cyc_List_List*_tmp5D7;*effects=((_tmp5D7=_cycalloc(
sizeof(*_tmp5D7)),((_tmp5D7->hd=Cyc_Absynpp_typ2doc(t),((_tmp5D7->tl=*effects,
_tmp5D7))))));}goto _LLC1;_LLC1:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t);
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct Cyc_List_List*rgions=0;struct
Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(& rgions,& effects,t);rgions=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);effects=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);if(
rgions == 0  && effects != 0){const char*_tmp5DA;const char*_tmp5D9;const char*_tmp5D8;
return Cyc_PP_group(((_tmp5D8="",_tag_dyneither(_tmp5D8,sizeof(char),1))),((
_tmp5D9="",_tag_dyneither(_tmp5D9,sizeof(char),1))),((_tmp5DA="+",_tag_dyneither(
_tmp5DA,sizeof(char),2))),effects);}else{const char*_tmp5DB;struct Cyc_PP_Doc*
_tmp111=Cyc_Absynpp_group_braces(((_tmp5DB=",",_tag_dyneither(_tmp5DB,sizeof(
char),2))),rgions);struct Cyc_List_List*_tmp5DF;const char*_tmp5DE;const char*
_tmp5DD;const char*_tmp5DC;return Cyc_PP_group(((_tmp5DC="",_tag_dyneither(_tmp5DC,
sizeof(char),1))),((_tmp5DD="",_tag_dyneither(_tmp5DD,sizeof(char),1))),((
_tmp5DE="+",_tag_dyneither(_tmp5DE,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((
_tmp5DF=_cycalloc(sizeof(*_tmp5DF)),((_tmp5DF->hd=_tmp111,((_tmp5DF->tl=0,
_tmp5DF))))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(void*k);struct Cyc_PP_Doc*
Cyc_Absynpp_aggr_kind2doc(void*k){void*_tmp117=k;_LLC9: if((int)_tmp117 != 0)goto
_LLCB;_LLCA: {const char*_tmp5E0;return Cyc_PP_text(((_tmp5E0="struct ",
_tag_dyneither(_tmp5E0,sizeof(char),8))));}_LLCB: if((int)_tmp117 != 1)goto _LLC8;
_LLCC: {const char*_tmp5E1;return Cyc_PP_text(((_tmp5E1="union ",_tag_dyneither(
_tmp5E1,sizeof(char),7))));}_LLC8:;}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
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
char*_tmp5E2;return Cyc_PP_text(((_tmp5E2="[[[array]]]",_tag_dyneither(_tmp5E2,
sizeof(char),12))));}_LLD0: if(*((int*)_tmp11A)!= 8)goto _LLD2;_LLD1: return Cyc_PP_nil_doc();
_LLD2: if(*((int*)_tmp11A)!= 4)goto _LLD4;_LLD3: return Cyc_PP_nil_doc();_LLD4: if((
int)_tmp11A != 0)goto _LLD6;_LLD5:{const char*_tmp5E3;s=Cyc_PP_text(((_tmp5E3="void",
_tag_dyneither(_tmp5E3,sizeof(char),5))));}goto _LLCD;_LLD6: if(_tmp11A <= (void*)4)
goto _LLE0;if(*((int*)_tmp11A)!= 0)goto _LLD8;_tmp11B=((struct Cyc_Absyn_Evar_struct*)
_tmp11A)->f1;_tmp11C=((struct Cyc_Absyn_Evar_struct*)_tmp11A)->f2;_tmp11D=((
struct Cyc_Absyn_Evar_struct*)_tmp11A)->f3;_tmp11E=((struct Cyc_Absyn_Evar_struct*)
_tmp11A)->f4;_LLD7: if(_tmp11C != 0)return Cyc_Absynpp_ntyp2doc((void*)_tmp11C->v);
else{const char*_tmp5F1;struct Cyc_Int_pa_struct _tmp5F0;void*_tmp5EF[1];const char*
_tmp5EE;const char*_tmp5ED;const char*_tmp5EC;struct Cyc_PP_Doc*_tmp5EB[6];s=((
_tmp5EB[5]=_tmp11B == 0?Cyc_Absynpp_question(): Cyc_Absynpp_kind2doc((void*)
_tmp11B->v),((_tmp5EB[4]=Cyc_PP_text(((_tmp5F1=")::",_tag_dyneither(_tmp5F1,
sizeof(char),4)))),((_tmp5EB[3]=(!Cyc_Absynpp_print_full_evars  || _tmp11E == 0)?
Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp11E->v),((
_tmp5EB[2]=Cyc_PP_text((struct _dyneither_ptr)((_tmp5F0.tag=1,((_tmp5F0.f1=(
unsigned long)_tmp11D,((_tmp5EF[0]=& _tmp5F0,Cyc_aprintf(((_tmp5EE="%d",
_tag_dyneither(_tmp5EE,sizeof(char),3))),_tag_dyneither(_tmp5EF,sizeof(void*),1))))))))),((
_tmp5EB[1]=Cyc_PP_text(((_tmp5ED="(",_tag_dyneither(_tmp5ED,sizeof(char),2)))),((
_tmp5EB[0]=Cyc_PP_text(((_tmp5EC="%",_tag_dyneither(_tmp5EC,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp5EB,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
goto _LLCD;_LLD8: if(*((int*)_tmp11A)!= 1)goto _LLDA;_tmp11F=((struct Cyc_Absyn_VarType_struct*)
_tmp11A)->f1;_LLD9: s=Cyc_PP_textptr(_tmp11F->name);if(Cyc_Absynpp_print_all_kinds){
const char*_tmp5F4;struct Cyc_PP_Doc*_tmp5F3[3];s=((_tmp5F3[2]=Cyc_Absynpp_kindbound2doc((
void*)_tmp11F->kind),((_tmp5F3[1]=Cyc_PP_text(((_tmp5F4="::",_tag_dyneither(
_tmp5F4,sizeof(char),3)))),((_tmp5F3[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp5F3,
sizeof(struct Cyc_PP_Doc*),3))))))));}if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(
_tmp11F)){const char*_tmp5F9;const char*_tmp5F8;struct Cyc_PP_Doc*_tmp5F7[3];s=((
_tmp5F7[2]=Cyc_PP_text(((_tmp5F9=" */",_tag_dyneither(_tmp5F9,sizeof(char),4)))),((
_tmp5F7[1]=s,((_tmp5F7[0]=Cyc_PP_text(((_tmp5F8="_ /* ",_tag_dyneither(_tmp5F8,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp5F7,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLDA: if(*((int*)_tmp11A)!= 2)goto _LLDC;_tmp120=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp11A)->f1;_tmp121=_tmp120.datatype_info;_tmp122=_tmp120.targs;_tmp123=_tmp120.rgn;
_LLDB:{union Cyc_Absyn_DatatypeInfoU_union _tmp149=_tmp121;struct Cyc_Absyn_UnknownDatatypeInfo
_tmp14A;struct _tuple0*_tmp14B;int _tmp14C;struct Cyc_Absyn_Datatypedecl**_tmp14D;
struct Cyc_Absyn_Datatypedecl*_tmp14E;struct Cyc_Absyn_Datatypedecl _tmp14F;struct
_tuple0*_tmp150;int _tmp151;_LL103: if((_tmp149.UnknownDatatype).tag != 0)goto
_LL105;_tmp14A=(_tmp149.UnknownDatatype).f1;_tmp14B=_tmp14A.name;_tmp14C=_tmp14A.is_extensible;
_LL104: _tmp150=_tmp14B;_tmp151=_tmp14C;goto _LL106;_LL105: if((_tmp149.KnownDatatype).tag
!= 1)goto _LL102;_tmp14D=(_tmp149.KnownDatatype).f1;_tmp14E=*_tmp14D;_tmp14F=*
_tmp14E;_tmp150=_tmp14F.name;_tmp151=_tmp14F.is_extensible;_LL106: {const char*
_tmp5FA;struct Cyc_PP_Doc*_tmp152=Cyc_PP_text(((_tmp5FA="datatype ",
_tag_dyneither(_tmp5FA,sizeof(char),10))));const char*_tmp5FB;struct Cyc_PP_Doc*
_tmp153=_tmp151?Cyc_PP_text(((_tmp5FB="@extensible ",_tag_dyneither(_tmp5FB,
sizeof(char),13)))): Cyc_PP_nil_doc();void*r=(unsigned int)_tmp123?(void*)_tmp123->v:(
void*)2;{void*_tmp154=Cyc_Tcutil_compress(r);_LL108: if((int)_tmp154 != 2)goto
_LL10A;_LL109:{struct Cyc_PP_Doc*_tmp5FC[4];s=((_tmp5FC[3]=Cyc_Absynpp_tps2doc(
_tmp122),((_tmp5FC[2]=Cyc_Absynpp_qvar2doc(_tmp150),((_tmp5FC[1]=_tmp152,((
_tmp5FC[0]=_tmp153,Cyc_PP_cat(_tag_dyneither(_tmp5FC,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL107;_LL10A:;_LL10B:{const char*_tmp5FF;struct Cyc_PP_Doc*_tmp5FE[6];s=((
_tmp5FE[5]=Cyc_Absynpp_tps2doc(_tmp122),((_tmp5FE[4]=Cyc_Absynpp_qvar2doc(
_tmp150),((_tmp5FE[3]=Cyc_PP_text(((_tmp5FF=" ",_tag_dyneither(_tmp5FF,sizeof(
char),2)))),((_tmp5FE[2]=Cyc_Absynpp_typ2doc(r),((_tmp5FE[1]=_tmp152,((_tmp5FE[0]=
_tmp153,Cyc_PP_cat(_tag_dyneither(_tmp5FE,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
goto _LL107;_LL107:;}goto _LL102;}_LL102:;}goto _LLCD;_LLDC: if(*((int*)_tmp11A)!= 3)
goto _LLDE;_tmp124=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp11A)->f1;
_tmp125=_tmp124.field_info;_tmp126=_tmp124.targs;_LLDD:{union Cyc_Absyn_DatatypeFieldInfoU_union
_tmp15A=_tmp125;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp15B;struct _tuple0*
_tmp15C;struct _tuple0*_tmp15D;int _tmp15E;struct Cyc_Absyn_Datatypedecl*_tmp15F;
struct Cyc_Absyn_Datatypedecl _tmp160;struct _tuple0*_tmp161;int _tmp162;struct Cyc_Absyn_Datatypefield*
_tmp163;struct Cyc_Absyn_Datatypefield _tmp164;struct _tuple0*_tmp165;_LL10D: if((
_tmp15A.UnknownDatatypefield).tag != 0)goto _LL10F;_tmp15B=(_tmp15A.UnknownDatatypefield).f1;
_tmp15C=_tmp15B.datatype_name;_tmp15D=_tmp15B.field_name;_tmp15E=_tmp15B.is_extensible;
_LL10E: _tmp161=_tmp15C;_tmp162=_tmp15E;_tmp165=_tmp15D;goto _LL110;_LL10F: if((
_tmp15A.KnownDatatypefield).tag != 1)goto _LL10C;_tmp15F=(_tmp15A.KnownDatatypefield).f1;
_tmp160=*_tmp15F;_tmp161=_tmp160.name;_tmp162=_tmp160.is_extensible;_tmp163=(
_tmp15A.KnownDatatypefield).f2;_tmp164=*_tmp163;_tmp165=_tmp164.name;_LL110: {
const char*_tmp601;const char*_tmp600;struct Cyc_PP_Doc*_tmp166=Cyc_PP_text(_tmp162?(
_tmp601="@extensible datatype ",_tag_dyneither(_tmp601,sizeof(char),22)):((
_tmp600="datatype ",_tag_dyneither(_tmp600,sizeof(char),10))));{const char*
_tmp604;struct Cyc_PP_Doc*_tmp603[4];s=((_tmp603[3]=Cyc_Absynpp_qvar2doc(_tmp165),((
_tmp603[2]=Cyc_PP_text(((_tmp604=".",_tag_dyneither(_tmp604,sizeof(char),2)))),((
_tmp603[1]=Cyc_Absynpp_qvar2doc(_tmp161),((_tmp603[0]=_tmp166,Cyc_PP_cat(
_tag_dyneither(_tmp603,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL10C;}_LL10C:;}
goto _LLCD;_LLDE: if(*((int*)_tmp11A)!= 5)goto _LLE0;_tmp127=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp11A)->f1;_tmp128=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp11A)->f2;_LLDF: {
struct _dyneither_ptr sns;struct _dyneither_ptr ts;{void*_tmp16B=_tmp127;_LL112: if((
int)_tmp16B != 2)goto _LL114;_LL113: goto _LL115;_LL114: if((int)_tmp16B != 0)goto
_LL116;_LL115:{const char*_tmp605;sns=((_tmp605="",_tag_dyneither(_tmp605,sizeof(
char),1)));}goto _LL111;_LL116: if((int)_tmp16B != 1)goto _LL111;_LL117:{const char*
_tmp606;sns=((_tmp606="unsigned ",_tag_dyneither(_tmp606,sizeof(char),10)));}
goto _LL111;_LL111:;}{void*_tmp16E=_tmp128;_LL119: if((int)_tmp16E != 0)goto _LL11B;
_LL11A:{void*_tmp16F=_tmp127;_LL124: if((int)_tmp16F != 2)goto _LL126;_LL125:{const
char*_tmp607;sns=((_tmp607="",_tag_dyneither(_tmp607,sizeof(char),1)));}goto
_LL123;_LL126: if((int)_tmp16F != 0)goto _LL128;_LL127:{const char*_tmp608;sns=((
_tmp608="signed ",_tag_dyneither(_tmp608,sizeof(char),8)));}goto _LL123;_LL128:
if((int)_tmp16F != 1)goto _LL123;_LL129:{const char*_tmp609;sns=((_tmp609="unsigned ",
_tag_dyneither(_tmp609,sizeof(char),10)));}goto _LL123;_LL123:;}{const char*
_tmp60A;ts=((_tmp60A="char",_tag_dyneither(_tmp60A,sizeof(char),5)));}goto _LL118;
_LL11B: if((int)_tmp16E != 1)goto _LL11D;_LL11C:{const char*_tmp60B;ts=((_tmp60B="short",
_tag_dyneither(_tmp60B,sizeof(char),6)));}goto _LL118;_LL11D: if((int)_tmp16E != 2)
goto _LL11F;_LL11E:{const char*_tmp60C;ts=((_tmp60C="int",_tag_dyneither(_tmp60C,
sizeof(char),4)));}goto _LL118;_LL11F: if((int)_tmp16E != 3)goto _LL121;_LL120:{
const char*_tmp60D;ts=((_tmp60D="long",_tag_dyneither(_tmp60D,sizeof(char),5)));}
goto _LL118;_LL121: if((int)_tmp16E != 4)goto _LL118;_LL122:{void*_tmp177=Cyc_Cyclone_c_compiler;
_LL12B: if((int)_tmp177 != 0)goto _LL12D;_LL12C:{const char*_tmp60E;ts=((_tmp60E="long long",
_tag_dyneither(_tmp60E,sizeof(char),10)));}goto _LL12A;_LL12D: if((int)_tmp177 != 1)
goto _LL12A;_LL12E:{const char*_tmp60F;ts=((_tmp60F="__int64",_tag_dyneither(
_tmp60F,sizeof(char),8)));}goto _LL12A;_LL12A:;}goto _LL118;_LL118:;}{const char*
_tmp614;void*_tmp613[2];struct Cyc_String_pa_struct _tmp612;struct Cyc_String_pa_struct
_tmp611;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp611.tag=0,((_tmp611.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)ts),((_tmp612.tag=0,((_tmp612.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)sns),((_tmp613[0]=& _tmp612,((_tmp613[1]=&
_tmp611,Cyc_aprintf(((_tmp614="%s%s",_tag_dyneither(_tmp614,sizeof(char),5))),
_tag_dyneither(_tmp613,sizeof(void*),2)))))))))))))));}goto _LLCD;}_LLE0: if((int)
_tmp11A != 1)goto _LLE2;_LLE1:{const char*_tmp615;s=Cyc_PP_text(((_tmp615="float",
_tag_dyneither(_tmp615,sizeof(char),6))));}goto _LLCD;_LLE2: if(_tmp11A <= (void*)4)
goto _LLF8;if(*((int*)_tmp11A)!= 6)goto _LLE4;_tmp129=((struct Cyc_Absyn_DoubleType_struct*)
_tmp11A)->f1;_LLE3: if(_tmp129){const char*_tmp616;s=Cyc_PP_text(((_tmp616="long double",
_tag_dyneither(_tmp616,sizeof(char),12))));}else{const char*_tmp617;s=Cyc_PP_text(((
_tmp617="double",_tag_dyneither(_tmp617,sizeof(char),7))));}goto _LLCD;_LLE4: if(*((
int*)_tmp11A)!= 9)goto _LLE6;_tmp12A=((struct Cyc_Absyn_TupleType_struct*)_tmp11A)->f1;
_LLE5:{struct Cyc_PP_Doc*_tmp618[2];s=((_tmp618[1]=Cyc_Absynpp_args2doc(_tmp12A),((
_tmp618[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp618,sizeof(struct
Cyc_PP_Doc*),2))))));}goto _LLCD;_LLE6: if(*((int*)_tmp11A)!= 10)goto _LLE8;_tmp12B=((
struct Cyc_Absyn_AggrType_struct*)_tmp11A)->f1;_tmp12C=_tmp12B.aggr_info;_tmp12D=
_tmp12B.targs;_LLE7: {void*_tmp183;struct _tuple0*_tmp184;struct _tuple3 _tmp182=
Cyc_Absyn_aggr_kinded_name(_tmp12C);_tmp183=_tmp182.f1;_tmp184=_tmp182.f2;{
struct Cyc_PP_Doc*_tmp619[3];s=((_tmp619[2]=Cyc_Absynpp_tps2doc(_tmp12D),((
_tmp619[1]=Cyc_Absynpp_qvar2doc(_tmp184),((_tmp619[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp183),Cyc_PP_cat(_tag_dyneither(_tmp619,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;}_LLE8: if(*((int*)_tmp11A)!= 11)goto _LLEA;_tmp12E=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp11A)->f1;_tmp12F=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp11A)->f2;_LLE9:{
struct Cyc_PP_Doc*_tmp61A[4];s=((_tmp61A[3]=Cyc_Absynpp_rb(),((_tmp61A[2]=Cyc_PP_nest(
2,Cyc_Absynpp_aggrfields2doc(_tmp12F)),((_tmp61A[1]=Cyc_Absynpp_lb(),((_tmp61A[0]=
Cyc_Absynpp_aggr_kind2doc(_tmp12E),Cyc_PP_cat(_tag_dyneither(_tmp61A,sizeof(
struct Cyc_PP_Doc*),4))))))))));}goto _LLCD;_LLEA: if(*((int*)_tmp11A)!= 13)goto
_LLEC;_tmp130=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp11A)->f1;_LLEB:{const
char*_tmp61D;struct Cyc_PP_Doc*_tmp61C[4];s=((_tmp61C[3]=Cyc_Absynpp_rb(),((
_tmp61C[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp130)),((_tmp61C[1]=Cyc_Absynpp_lb(),((
_tmp61C[0]=Cyc_PP_text(((_tmp61D="enum ",_tag_dyneither(_tmp61D,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp61C,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LLCD;_LLEC: if(*((int*)_tmp11A)!= 12)goto _LLEE;_tmp131=((struct Cyc_Absyn_EnumType_struct*)
_tmp11A)->f1;_LLED:{const char*_tmp620;struct Cyc_PP_Doc*_tmp61F[2];s=((_tmp61F[1]=
Cyc_Absynpp_qvar2doc(_tmp131),((_tmp61F[0]=Cyc_PP_text(((_tmp620="enum ",
_tag_dyneither(_tmp620,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp61F,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLCD;_LLEE: if(*((int*)_tmp11A)!= 17)goto
_LLF0;_tmp132=((struct Cyc_Absyn_ValueofType_struct*)_tmp11A)->f1;_LLEF:{const
char*_tmp625;const char*_tmp624;struct Cyc_PP_Doc*_tmp623[3];s=((_tmp623[2]=Cyc_PP_text(((
_tmp625=")",_tag_dyneither(_tmp625,sizeof(char),2)))),((_tmp623[1]=Cyc_Absynpp_exp2doc(
_tmp132),((_tmp623[0]=Cyc_PP_text(((_tmp624="valueof_t(",_tag_dyneither(_tmp624,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp623,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LLCD;_LLF0: if(*((int*)_tmp11A)!= 16)goto _LLF2;_tmp133=((struct
Cyc_Absyn_TypedefType_struct*)_tmp11A)->f1;_tmp134=((struct Cyc_Absyn_TypedefType_struct*)
_tmp11A)->f2;_tmp135=((struct Cyc_Absyn_TypedefType_struct*)_tmp11A)->f3;_LLF1:{
struct Cyc_PP_Doc*_tmp626[2];s=((_tmp626[1]=Cyc_Absynpp_tps2doc(_tmp134),((
_tmp626[0]=Cyc_Absynpp_qvar2doc(_tmp133),Cyc_PP_cat(_tag_dyneither(_tmp626,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLCD;_LLF2: if(*((int*)_tmp11A)!= 14)goto
_LLF4;_tmp136=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp11A)->f1;_LLF3:{
const char*_tmp62B;const char*_tmp62A;struct Cyc_PP_Doc*_tmp629[3];s=((_tmp629[2]=
Cyc_PP_text(((_tmp62B=">",_tag_dyneither(_tmp62B,sizeof(char),2)))),((_tmp629[1]=
Cyc_Absynpp_rgn2doc(_tmp136),((_tmp629[0]=Cyc_PP_text(((_tmp62A="region_t<",
_tag_dyneither(_tmp62A,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp629,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCD;_LLF4: if(*((int*)_tmp11A)!= 15)
goto _LLF6;_tmp137=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp11A)->f1;
_tmp138=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp11A)->f2;_LLF5:{const
char*_tmp632;const char*_tmp631;const char*_tmp630;struct Cyc_PP_Doc*_tmp62F[5];s=((
_tmp62F[4]=Cyc_PP_text(((_tmp632=">",_tag_dyneither(_tmp632,sizeof(char),2)))),((
_tmp62F[3]=Cyc_Absynpp_rgn2doc(_tmp138),((_tmp62F[2]=Cyc_PP_text(((_tmp631=",",
_tag_dyneither(_tmp631,sizeof(char),2)))),((_tmp62F[1]=Cyc_Absynpp_rgn2doc(
_tmp137),((_tmp62F[0]=Cyc_PP_text(((_tmp630="dynregion_t<",_tag_dyneither(
_tmp630,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp62F,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LLCD;_LLF6: if(*((int*)_tmp11A)!= 18)goto _LLF8;_tmp139=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp11A)->f1;_LLF7:{const char*_tmp637;const char*
_tmp636;struct Cyc_PP_Doc*_tmp635[3];s=((_tmp635[2]=Cyc_PP_text(((_tmp637=">",
_tag_dyneither(_tmp637,sizeof(char),2)))),((_tmp635[1]=Cyc_Absynpp_typ2doc(
_tmp139),((_tmp635[0]=Cyc_PP_text(((_tmp636="tag_t<",_tag_dyneither(_tmp636,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp635,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLF8: if((int)_tmp11A != 3)goto _LLFA;_LLF9: goto _LLFB;_LLFA: if((int)
_tmp11A != 2)goto _LLFC;_LLFB: s=Cyc_Absynpp_rgn2doc(t);goto _LLCD;_LLFC: if(_tmp11A
<= (void*)4)goto _LLFE;if(*((int*)_tmp11A)!= 21)goto _LLFE;_tmp13A=(void*)((struct
Cyc_Absyn_RgnsEff_struct*)_tmp11A)->f1;_LLFD:{const char*_tmp63C;const char*
_tmp63B;struct Cyc_PP_Doc*_tmp63A[3];s=((_tmp63A[2]=Cyc_PP_text(((_tmp63C=")",
_tag_dyneither(_tmp63C,sizeof(char),2)))),((_tmp63A[1]=Cyc_Absynpp_typ2doc(
_tmp13A),((_tmp63A[0]=Cyc_PP_text(((_tmp63B="regions(",_tag_dyneither(_tmp63B,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp63A,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLFE: if(_tmp11A <= (void*)4)goto _LL100;if(*((int*)_tmp11A)!= 19)goto
_LL100;_LLFF: goto _LL101;_LL100: if(_tmp11A <= (void*)4)goto _LLCD;if(*((int*)
_tmp11A)!= 20)goto _LLCD;_LL101: s=Cyc_Absynpp_eff2doc(t);goto _LLCD;_LLCD:;}return
s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo);struct Cyc_PP_Doc*
Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _dyneither_ptr*)vo->v));}struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct
_tuple5*cmp);struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple5*cmp){struct
_tuple5 _tmp19D;void*_tmp19E;void*_tmp19F;struct _tuple5*_tmp19C=cmp;_tmp19D=*
_tmp19C;_tmp19E=_tmp19D.f1;_tmp19F=_tmp19D.f2;{const char*_tmp63F;struct Cyc_PP_Doc*
_tmp63E[3];return(_tmp63E[2]=Cyc_Absynpp_rgn2doc(_tmp19F),((_tmp63E[1]=Cyc_PP_text(((
_tmp63F=" > ",_tag_dyneither(_tmp63F,sizeof(char),4)))),((_tmp63E[0]=Cyc_Absynpp_rgn2doc(
_tmp19E),Cyc_PP_cat(_tag_dyneither(_tmp63E,sizeof(struct Cyc_PP_Doc*),3)))))));}}
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po);struct Cyc_PP_Doc*
Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){const char*_tmp642;const char*
_tmp641;const char*_tmp640;return Cyc_PP_group(((_tmp640="",_tag_dyneither(_tmp640,
sizeof(char),1))),((_tmp641="",_tag_dyneither(_tmp641,sizeof(char),1))),((
_tmp642=",",_tag_dyneither(_tmp642,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,
po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple1*t);struct Cyc_PP_Doc*
Cyc_Absynpp_funarg2doc(struct _tuple1*t){struct Cyc_Core_Opt*_tmp643;struct Cyc_Core_Opt*
dopt=(*t).f1 == 0?0:((_tmp643=_cycalloc(sizeof(*_tmp643)),((_tmp643->v=Cyc_PP_text(*((
struct _dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null((*t).f1))->v)),_tmp643))));
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*
_tmp1A6=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_PP_Doc*
eff_doc;if(c_varargs){const char*_tmp646;struct Cyc_List_List*_tmp645;_tmp1A6=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1A6,((_tmp645=_cycalloc(sizeof(*_tmp645)),((_tmp645->hd=Cyc_PP_text(((
_tmp646="...",_tag_dyneither(_tmp646,sizeof(char),4)))),((_tmp645->tl=0,_tmp645)))))));}
else{if(cyc_varargs != 0){struct _tuple1*_tmp64F;const char*_tmp64E;const char*
_tmp64D;const char*_tmp64C;struct Cyc_PP_Doc*_tmp64B[3];struct Cyc_PP_Doc*_tmp1A9=(
_tmp64B[2]=Cyc_Absynpp_funarg2doc(((_tmp64F=_cycalloc(sizeof(*_tmp64F)),((
_tmp64F->f1=cyc_varargs->name,((_tmp64F->f2=cyc_varargs->tq,((_tmp64F->f3=(void*)
cyc_varargs->type,_tmp64F))))))))),((_tmp64B[1]=cyc_varargs->inject?Cyc_PP_text(((
_tmp64D=" inject ",_tag_dyneither(_tmp64D,sizeof(char),9)))): Cyc_PP_text(((
_tmp64E=" ",_tag_dyneither(_tmp64E,sizeof(char),2)))),((_tmp64B[0]=Cyc_PP_text(((
_tmp64C="...",_tag_dyneither(_tmp64C,sizeof(char),4)))),Cyc_PP_cat(
_tag_dyneither(_tmp64B,sizeof(struct Cyc_PP_Doc*),3)))))));struct Cyc_List_List*
_tmp650;_tmp1A6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1A6,((_tmp650=_cycalloc(sizeof(*_tmp650)),((_tmp650->hd=
_tmp1A9,((_tmp650->tl=0,_tmp650)))))));}}{const char*_tmp653;const char*_tmp652;
const char*_tmp651;struct Cyc_PP_Doc*_tmp1B0=Cyc_PP_group(((_tmp651="",
_tag_dyneither(_tmp651,sizeof(char),1))),((_tmp652="",_tag_dyneither(_tmp652,
sizeof(char),1))),((_tmp653=",",_tag_dyneither(_tmp653,sizeof(char),2))),_tmp1A6);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){const char*_tmp656;struct Cyc_PP_Doc*
_tmp655[3];_tmp1B0=((_tmp655[2]=Cyc_Absynpp_eff2doc((void*)effopt->v),((_tmp655[
1]=Cyc_PP_text(((_tmp656=";",_tag_dyneither(_tmp656,sizeof(char),2)))),((_tmp655[
0]=_tmp1B0,Cyc_PP_cat(_tag_dyneither(_tmp655,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){const char*_tmp659;struct Cyc_PP_Doc*_tmp658[3];_tmp1B0=((_tmp658[2]=
Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp658[1]=Cyc_PP_text(((_tmp659=":",
_tag_dyneither(_tmp659,sizeof(char),2)))),((_tmp658[0]=_tmp1B0,Cyc_PP_cat(
_tag_dyneither(_tmp658,sizeof(struct Cyc_PP_Doc*),3))))))));}{const char*_tmp65E;
const char*_tmp65D;struct Cyc_PP_Doc*_tmp65C[3];return(_tmp65C[2]=Cyc_PP_text(((
_tmp65E=")",_tag_dyneither(_tmp65E,sizeof(char),2)))),((_tmp65C[1]=_tmp1B0,((
_tmp65C[0]=Cyc_PP_text(((_tmp65D="(",_tag_dyneither(_tmp65D,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp65C,sizeof(struct Cyc_PP_Doc*),3)))))));}}}struct
_tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple6*arg);struct _tuple1*Cyc_Absynpp_arg_mk_opt(
struct _tuple6*arg){struct Cyc_Core_Opt*_tmp661;struct _tuple1*_tmp660;return(
_tmp660=_cycalloc(sizeof(*_tmp660)),((_tmp660->f1=((_tmp661=_cycalloc(sizeof(*
_tmp661)),((_tmp661->v=(*arg).f1,_tmp661)))),((_tmp660->f2=(*arg).f2,((_tmp660->f3=(*
arg).f3,_tmp660)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct
_dyneither_ptr*v);struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){
return Cyc_PP_text(*v);}struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*
q);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){struct Cyc_List_List*
_tmp1BD=0;int match;{union Cyc_Absyn_Nmspace_union _tmp1BE=(*q).f1;struct Cyc_List_List*
_tmp1BF;struct Cyc_List_List*_tmp1C0;_LL130: if((_tmp1BE.Loc_n).tag != 0)goto _LL132;
_LL131: _tmp1BF=0;goto _LL133;_LL132: if((_tmp1BE.Rel_n).tag != 1)goto _LL134;_tmp1BF=(
_tmp1BE.Rel_n).f1;_LL133: match=0;_tmp1BD=_tmp1BF;goto _LL12F;_LL134: if((_tmp1BE.Abs_n).tag
!= 2)goto _LL12F;_tmp1C0=(_tmp1BE.Abs_n).f1;_LL135: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp1C0,Cyc_Absynpp_curr_namespace);{
struct Cyc_List_List*_tmp662;_tmp1BD=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(
_tmp662=_cycalloc(sizeof(*_tmp662)),((_tmp662->hd=Cyc_Absynpp_cyc_stringptr,((
_tmp662->tl=_tmp1C0,_tmp662))))): _tmp1C0;}goto _LL12F;_LL12F:;}if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp664;struct Cyc_List_List*_tmp663;return(struct _dyneither_ptr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1BD,((_tmp663=_cycalloc(sizeof(*_tmp663)),((_tmp663->hd=(*q).f2,((_tmp663->tl=
0,_tmp663))))))),((_tmp664="_",_tag_dyneither(_tmp664,sizeof(char),2))));}else{
if(match)return*(*q).f2;else{const char*_tmp666;struct Cyc_List_List*_tmp665;
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1BD,((_tmp665=_cycalloc(sizeof(*
_tmp665)),((_tmp665->hd=(*q).f2,((_tmp665->tl=0,_tmp665))))))),((_tmp666="::",
_tag_dyneither(_tmp666,sizeof(char),3))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){return
Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp1C6=Cyc_Absynpp_qvar2string(q);if(Cyc_PP_tex_output){
const char*_tmp668;const char*_tmp667;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp667="\\textbf{",
_tag_dyneither(_tmp667,sizeof(char),9))),(struct _dyneither_ptr)_tmp1C6),((
_tmp668="}",_tag_dyneither(_tmp668,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp1C6));}else{return Cyc_PP_text(_tmp1C6);}}struct _dyneither_ptr
Cyc_Absynpp_typedef_name2string(struct _tuple0*v);struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(
struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace_union _tmp1C9=(*v).f1;
struct Cyc_List_List*_tmp1CA;struct Cyc_List_List*_tmp1CB;_LL137: if((_tmp1C9.Loc_n).tag
!= 0)goto _LL139;_LL138: goto _LL13A;_LL139: if((_tmp1C9.Rel_n).tag != 1)goto _LL13B;
_tmp1CA=(_tmp1C9.Rel_n).f1;if(_tmp1CA != 0)goto _LL13B;_LL13A: return*(*v).f2;
_LL13B: if((_tmp1C9.Abs_n).tag != 2)goto _LL13D;_tmp1CB=(_tmp1C9.Abs_n).f1;_LL13C:
if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp1CB,Cyc_Absynpp_curr_namespace)
== 0)return*(*v).f2;else{goto _LL13E;}_LL13D:;_LL13E: {const char*_tmp669;return(
struct _dyneither_ptr)Cyc_strconcat(((_tmp669="/* bad namespace : */ ",
_tag_dyneither(_tmp669,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
v));}_LL136:;}else{return*(*v).f2;}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*
v){struct _dyneither_ptr _tmp1CD=Cyc_Absynpp_typedef_name2string(v);if(Cyc_PP_tex_output){
const char*_tmp66B;const char*_tmp66A;return Cyc_PP_text_width((struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp66A="\\textbf{",
_tag_dyneither(_tmp66A,sizeof(char),9))),(struct _dyneither_ptr)_tmp1CD),((
_tmp66B="}",_tag_dyneither(_tmp66B,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp1CD));}else{return Cyc_PP_text(_tmp1CD);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e);
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){void*_tmp1D0=(void*)e->r;void*
_tmp1D1;struct Cyc_Absyn_Exp*_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D3;_LL140: if(*((int*)
_tmp1D0)!= 0)goto _LL142;_LL141: goto _LL143;_LL142: if(*((int*)_tmp1D0)!= 1)goto
_LL144;_LL143: goto _LL145;_LL144: if(*((int*)_tmp1D0)!= 2)goto _LL146;_LL145: return
10000;_LL146: if(*((int*)_tmp1D0)!= 3)goto _LL148;_tmp1D1=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D0)->f1;_LL147: {void*_tmp1D4=_tmp1D1;_LL191: if((int)_tmp1D4 != 0)goto _LL193;
_LL192: return 100;_LL193: if((int)_tmp1D4 != 1)goto _LL195;_LL194: return 110;_LL195:
if((int)_tmp1D4 != 2)goto _LL197;_LL196: return 100;_LL197: if((int)_tmp1D4 != 3)goto
_LL199;_LL198: goto _LL19A;_LL199: if((int)_tmp1D4 != 4)goto _LL19B;_LL19A: return 110;
_LL19B: if((int)_tmp1D4 != 5)goto _LL19D;_LL19C: goto _LL19E;_LL19D: if((int)_tmp1D4 != 
6)goto _LL19F;_LL19E: return 70;_LL19F: if((int)_tmp1D4 != 7)goto _LL1A1;_LL1A0: goto
_LL1A2;_LL1A1: if((int)_tmp1D4 != 8)goto _LL1A3;_LL1A2: goto _LL1A4;_LL1A3: if((int)
_tmp1D4 != 9)goto _LL1A5;_LL1A4: goto _LL1A6;_LL1A5: if((int)_tmp1D4 != 10)goto _LL1A7;
_LL1A6: return 80;_LL1A7: if((int)_tmp1D4 != 11)goto _LL1A9;_LL1A8: goto _LL1AA;_LL1A9:
if((int)_tmp1D4 != 12)goto _LL1AB;_LL1AA: return 130;_LL1AB: if((int)_tmp1D4 != 13)
goto _LL1AD;_LL1AC: return 60;_LL1AD: if((int)_tmp1D4 != 14)goto _LL1AF;_LL1AE: return
40;_LL1AF: if((int)_tmp1D4 != 15)goto _LL1B1;_LL1B0: return 50;_LL1B1: if((int)_tmp1D4
!= 16)goto _LL1B3;_LL1B2: return 90;_LL1B3: if((int)_tmp1D4 != 17)goto _LL1B5;_LL1B4:
return 80;_LL1B5: if((int)_tmp1D4 != 18)goto _LL1B7;_LL1B6: return 80;_LL1B7: if((int)
_tmp1D4 != 19)goto _LL190;_LL1B8: return 140;_LL190:;}_LL148: if(*((int*)_tmp1D0)!= 4)
goto _LL14A;_LL149: return 20;_LL14A: if(*((int*)_tmp1D0)!= 5)goto _LL14C;_LL14B:
return 130;_LL14C: if(*((int*)_tmp1D0)!= 6)goto _LL14E;_LL14D: return 30;_LL14E: if(*((
int*)_tmp1D0)!= 7)goto _LL150;_LL14F: return 35;_LL150: if(*((int*)_tmp1D0)!= 8)goto
_LL152;_LL151: return 30;_LL152: if(*((int*)_tmp1D0)!= 9)goto _LL154;_LL153: return 10;
_LL154: if(*((int*)_tmp1D0)!= 10)goto _LL156;_LL155: goto _LL157;_LL156: if(*((int*)
_tmp1D0)!= 11)goto _LL158;_LL157: return 140;_LL158: if(*((int*)_tmp1D0)!= 12)goto
_LL15A;_LL159: return 130;_LL15A: if(*((int*)_tmp1D0)!= 13)goto _LL15C;_tmp1D2=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D0)->f1;_LL15B: return Cyc_Absynpp_exp_prec(
_tmp1D2);_LL15C: if(*((int*)_tmp1D0)!= 14)goto _LL15E;_tmp1D3=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1D0)->f1;_LL15D: return Cyc_Absynpp_exp_prec(_tmp1D3);_LL15E: if(*((int*)
_tmp1D0)!= 15)goto _LL160;_LL15F: return 120;_LL160: if(*((int*)_tmp1D0)!= 17)goto
_LL162;_LL161: return 15;_LL162: if(*((int*)_tmp1D0)!= 16)goto _LL164;_LL163: goto
_LL165;_LL164: if(*((int*)_tmp1D0)!= 18)goto _LL166;_LL165: goto _LL167;_LL166: if(*((
int*)_tmp1D0)!= 19)goto _LL168;_LL167: goto _LL169;_LL168: if(*((int*)_tmp1D0)!= 39)
goto _LL16A;_LL169: goto _LL16B;_LL16A: if(*((int*)_tmp1D0)!= 20)goto _LL16C;_LL16B:
goto _LL16D;_LL16C: if(*((int*)_tmp1D0)!= 21)goto _LL16E;_LL16D: goto _LL16F;_LL16E:
if(*((int*)_tmp1D0)!= 22)goto _LL170;_LL16F: return 130;_LL170: if(*((int*)_tmp1D0)
!= 23)goto _LL172;_LL171: goto _LL173;_LL172: if(*((int*)_tmp1D0)!= 24)goto _LL174;
_LL173: goto _LL175;_LL174: if(*((int*)_tmp1D0)!= 25)goto _LL176;_LL175: return 140;
_LL176: if(*((int*)_tmp1D0)!= 26)goto _LL178;_LL177: return 150;_LL178: if(*((int*)
_tmp1D0)!= 27)goto _LL17A;_LL179: goto _LL17B;_LL17A: if(*((int*)_tmp1D0)!= 28)goto
_LL17C;_LL17B: goto _LL17D;_LL17C: if(*((int*)_tmp1D0)!= 29)goto _LL17E;_LL17D: goto
_LL17F;_LL17E: if(*((int*)_tmp1D0)!= 30)goto _LL180;_LL17F: goto _LL181;_LL180: if(*((
int*)_tmp1D0)!= 31)goto _LL182;_LL181: goto _LL183;_LL182: if(*((int*)_tmp1D0)!= 32)
goto _LL184;_LL183: goto _LL185;_LL184: if(*((int*)_tmp1D0)!= 33)goto _LL186;_LL185:
goto _LL187;_LL186: if(*((int*)_tmp1D0)!= 34)goto _LL188;_LL187: goto _LL189;_LL188:
if(*((int*)_tmp1D0)!= 35)goto _LL18A;_LL189: goto _LL18B;_LL18A: if(*((int*)_tmp1D0)
!= 36)goto _LL18C;_LL18B: goto _LL18D;_LL18C: if(*((int*)_tmp1D0)!= 37)goto _LL18E;
_LL18D: return 140;_LL18E: if(*((int*)_tmp1D0)!= 38)goto _LL13F;_LL18F: return 10000;
_LL13F:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){int
myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*s;{void*_tmp1D5=(void*)e->r;
union Cyc_Absyn_Cnst_union _tmp1D6;struct _tuple0*_tmp1D7;struct _tuple0*_tmp1D8;
void*_tmp1D9;struct Cyc_List_List*_tmp1DA;struct Cyc_Absyn_Exp*_tmp1DB;struct Cyc_Core_Opt*
_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DD;struct Cyc_Absyn_Exp*_tmp1DE;void*_tmp1DF;
struct Cyc_Absyn_Exp*_tmp1E0;struct Cyc_Absyn_Exp*_tmp1E1;struct Cyc_Absyn_Exp*
_tmp1E2;struct Cyc_Absyn_Exp*_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E4;struct Cyc_Absyn_Exp*
_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E6;struct Cyc_Absyn_Exp*_tmp1E7;struct Cyc_Absyn_Exp*
_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_List_List*_tmp1EA;struct Cyc_Absyn_Exp*
_tmp1EB;struct Cyc_List_List*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1ED;struct Cyc_Absyn_Exp*
_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;void*_tmp1F0;struct Cyc_Absyn_Exp*_tmp1F1;
struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_Absyn_Exp*_tmp1F3;struct Cyc_Absyn_Exp*
_tmp1F4;void*_tmp1F5;struct Cyc_Absyn_Exp*_tmp1F6;void*_tmp1F7;void*_tmp1F8;void*
_tmp1F9;struct _dyneither_ptr*_tmp1FA;void*_tmp1FB;void*_tmp1FC;unsigned int
_tmp1FD;struct Cyc_List_List*_tmp1FE;void*_tmp1FF;struct Cyc_Absyn_Exp*_tmp200;
struct Cyc_Absyn_Exp*_tmp201;struct _dyneither_ptr*_tmp202;struct Cyc_Absyn_Exp*
_tmp203;struct _dyneither_ptr*_tmp204;struct Cyc_Absyn_Exp*_tmp205;struct Cyc_Absyn_Exp*
_tmp206;struct Cyc_List_List*_tmp207;struct _tuple1*_tmp208;struct Cyc_List_List*
_tmp209;struct Cyc_List_List*_tmp20A;struct Cyc_Absyn_Vardecl*_tmp20B;struct Cyc_Absyn_Exp*
_tmp20C;struct Cyc_Absyn_Exp*_tmp20D;struct _tuple0*_tmp20E;struct Cyc_List_List*
_tmp20F;struct Cyc_List_List*_tmp210;struct Cyc_List_List*_tmp211;struct Cyc_List_List*
_tmp212;struct Cyc_Absyn_Datatypefield*_tmp213;struct _tuple0*_tmp214;struct
_tuple0*_tmp215;struct Cyc_Absyn_MallocInfo _tmp216;int _tmp217;struct Cyc_Absyn_Exp*
_tmp218;void**_tmp219;struct Cyc_Absyn_Exp*_tmp21A;struct Cyc_Absyn_Exp*_tmp21B;
struct Cyc_Absyn_Exp*_tmp21C;struct Cyc_Core_Opt*_tmp21D;struct Cyc_List_List*
_tmp21E;struct Cyc_Absyn_Stmt*_tmp21F;_LL1BA: if(*((int*)_tmp1D5)!= 0)goto _LL1BC;
_tmp1D6=((struct Cyc_Absyn_Const_e_struct*)_tmp1D5)->f1;_LL1BB: s=Cyc_Absynpp_cnst2doc(
_tmp1D6);goto _LL1B9;_LL1BC: if(*((int*)_tmp1D5)!= 1)goto _LL1BE;_tmp1D7=((struct
Cyc_Absyn_Var_e_struct*)_tmp1D5)->f1;_LL1BD: _tmp1D8=_tmp1D7;goto _LL1BF;_LL1BE:
if(*((int*)_tmp1D5)!= 2)goto _LL1C0;_tmp1D8=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp1D5)->f1;_LL1BF: s=Cyc_Absynpp_qvar2doc(_tmp1D8);goto _LL1B9;_LL1C0: if(*((int*)
_tmp1D5)!= 3)goto _LL1C2;_tmp1D9=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D5)->f1;_tmp1DA=((struct Cyc_Absyn_Primop_e_struct*)_tmp1D5)->f2;_LL1C1: s=
Cyc_Absynpp_primapp2doc(myprec,_tmp1D9,_tmp1DA);goto _LL1B9;_LL1C2: if(*((int*)
_tmp1D5)!= 4)goto _LL1C4;_tmp1DB=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1D5)->f1;
_tmp1DC=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1D5)->f2;_tmp1DD=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1D5)->f3;_LL1C3:{const char*_tmp670;const char*_tmp66F;struct Cyc_PP_Doc*
_tmp66E[5];s=((_tmp66E[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DD),((_tmp66E[3]=
Cyc_PP_text(((_tmp670="= ",_tag_dyneither(_tmp670,sizeof(char),3)))),((_tmp66E[2]=
_tmp1DC == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((void*)_tmp1DC->v),((_tmp66E[1]=
Cyc_PP_text(((_tmp66F=" ",_tag_dyneither(_tmp66F,sizeof(char),2)))),((_tmp66E[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DB),Cyc_PP_cat(_tag_dyneither(_tmp66E,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1B9;_LL1C4: if(*((int*)_tmp1D5)!= 
5)goto _LL1C6;_tmp1DE=((struct Cyc_Absyn_Increment_e_struct*)_tmp1D5)->f1;_tmp1DF=(
void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp1D5)->f2;_LL1C5: {struct Cyc_PP_Doc*
_tmp223=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1DE);{void*_tmp224=_tmp1DF;_LL20D:
if((int)_tmp224 != 0)goto _LL20F;_LL20E:{const char*_tmp673;struct Cyc_PP_Doc*
_tmp672[2];s=((_tmp672[1]=_tmp223,((_tmp672[0]=Cyc_PP_text(((_tmp673="++",
_tag_dyneither(_tmp673,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp672,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL20C;_LL20F: if((int)_tmp224 != 2)goto
_LL211;_LL210:{const char*_tmp676;struct Cyc_PP_Doc*_tmp675[2];s=((_tmp675[1]=
_tmp223,((_tmp675[0]=Cyc_PP_text(((_tmp676="--",_tag_dyneither(_tmp676,sizeof(
char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp675,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL20C;_LL211: if((int)_tmp224 != 1)goto _LL213;_LL212:{const char*_tmp679;
struct Cyc_PP_Doc*_tmp678[2];s=((_tmp678[1]=Cyc_PP_text(((_tmp679="++",
_tag_dyneither(_tmp679,sizeof(char),3)))),((_tmp678[0]=_tmp223,Cyc_PP_cat(
_tag_dyneither(_tmp678,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL20C;_LL213: if((
int)_tmp224 != 3)goto _LL20C;_LL214:{const char*_tmp67C;struct Cyc_PP_Doc*_tmp67B[2];
s=((_tmp67B[1]=Cyc_PP_text(((_tmp67C="--",_tag_dyneither(_tmp67C,sizeof(char),3)))),((
_tmp67B[0]=_tmp223,Cyc_PP_cat(_tag_dyneither(_tmp67B,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL20C;_LL20C:;}goto _LL1B9;}_LL1C6: if(*((int*)_tmp1D5)!= 6)goto _LL1C8;
_tmp1E0=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1D5)->f1;_tmp1E1=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1D5)->f2;_tmp1E2=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1D5)->f3;_LL1C7:{
const char*_tmp681;const char*_tmp680;struct Cyc_PP_Doc*_tmp67F[5];s=((_tmp67F[4]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E2),((_tmp67F[3]=Cyc_PP_text(((_tmp681=" : ",
_tag_dyneither(_tmp681,sizeof(char),4)))),((_tmp67F[2]=Cyc_Absynpp_exp2doc_prec(
0,_tmp1E1),((_tmp67F[1]=Cyc_PP_text(((_tmp680=" ? ",_tag_dyneither(_tmp680,
sizeof(char),4)))),((_tmp67F[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E0),Cyc_PP_cat(
_tag_dyneither(_tmp67F,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1B9;
_LL1C8: if(*((int*)_tmp1D5)!= 7)goto _LL1CA;_tmp1E3=((struct Cyc_Absyn_And_e_struct*)
_tmp1D5)->f1;_tmp1E4=((struct Cyc_Absyn_And_e_struct*)_tmp1D5)->f2;_LL1C9:{const
char*_tmp684;struct Cyc_PP_Doc*_tmp683[3];s=((_tmp683[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E4),((_tmp683[1]=Cyc_PP_text(((_tmp684=" && ",_tag_dyneither(_tmp684,
sizeof(char),5)))),((_tmp683[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E3),Cyc_PP_cat(
_tag_dyneither(_tmp683,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1CA:
if(*((int*)_tmp1D5)!= 8)goto _LL1CC;_tmp1E5=((struct Cyc_Absyn_Or_e_struct*)
_tmp1D5)->f1;_tmp1E6=((struct Cyc_Absyn_Or_e_struct*)_tmp1D5)->f2;_LL1CB:{const
char*_tmp687;struct Cyc_PP_Doc*_tmp686[3];s=((_tmp686[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E6),((_tmp686[1]=Cyc_PP_text(((_tmp687=" || ",_tag_dyneither(_tmp687,
sizeof(char),5)))),((_tmp686[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E5),Cyc_PP_cat(
_tag_dyneither(_tmp686,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1CC:
if(*((int*)_tmp1D5)!= 9)goto _LL1CE;_tmp1E7=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1D5)->f1;_tmp1E8=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1D5)->f2;_LL1CD:{
const char*_tmp68E;const char*_tmp68D;const char*_tmp68C;struct Cyc_PP_Doc*_tmp68B[5];
s=((_tmp68B[4]=Cyc_PP_text(((_tmp68E=")",_tag_dyneither(_tmp68E,sizeof(char),2)))),((
_tmp68B[3]=Cyc_Absynpp_exp2doc(_tmp1E8),((_tmp68B[2]=Cyc_PP_text(((_tmp68D=", ",
_tag_dyneither(_tmp68D,sizeof(char),3)))),((_tmp68B[1]=Cyc_Absynpp_exp2doc(
_tmp1E7),((_tmp68B[0]=Cyc_PP_text(((_tmp68C="(",_tag_dyneither(_tmp68C,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp68B,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1B9;_LL1CE: if(*((int*)_tmp1D5)!= 10)goto _LL1D0;_tmp1E9=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp1D5)->f1;_tmp1EA=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1D5)->f2;_LL1CF:{
const char*_tmp693;const char*_tmp692;struct Cyc_PP_Doc*_tmp691[4];s=((_tmp691[3]=
Cyc_PP_text(((_tmp693=")",_tag_dyneither(_tmp693,sizeof(char),2)))),((_tmp691[2]=
Cyc_Absynpp_exps2doc_prec(20,_tmp1EA),((_tmp691[1]=Cyc_PP_text(((_tmp692="(",
_tag_dyneither(_tmp692,sizeof(char),2)))),((_tmp691[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E9),Cyc_PP_cat(_tag_dyneither(_tmp691,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1B9;_LL1D0: if(*((int*)_tmp1D5)!= 11)goto _LL1D2;_tmp1EB=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1D5)->f1;_tmp1EC=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1D5)->f2;_LL1D1:{
const char*_tmp698;const char*_tmp697;struct Cyc_PP_Doc*_tmp696[4];s=((_tmp696[3]=
Cyc_PP_text(((_tmp698=")",_tag_dyneither(_tmp698,sizeof(char),2)))),((_tmp696[2]=
Cyc_Absynpp_exps2doc_prec(20,_tmp1EC),((_tmp696[1]=Cyc_PP_text(((_tmp697="(",
_tag_dyneither(_tmp697,sizeof(char),2)))),((_tmp696[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EB),Cyc_PP_cat(_tag_dyneither(_tmp696,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1B9;_LL1D2: if(*((int*)_tmp1D5)!= 12)goto _LL1D4;_tmp1ED=((struct Cyc_Absyn_Throw_e_struct*)
_tmp1D5)->f1;_LL1D3:{const char*_tmp69B;struct Cyc_PP_Doc*_tmp69A[2];s=((_tmp69A[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1ED),((_tmp69A[0]=Cyc_PP_text(((_tmp69B="throw ",
_tag_dyneither(_tmp69B,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp69A,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1B9;_LL1D4: if(*((int*)_tmp1D5)!= 13)
goto _LL1D6;_tmp1EE=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D5)->f1;_LL1D5:
s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1EE);goto _LL1B9;_LL1D6: if(*((int*)_tmp1D5)
!= 14)goto _LL1D8;_tmp1EF=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1D5)->f1;
_LL1D7: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1EF);goto _LL1B9;_LL1D8: if(*((int*)
_tmp1D5)!= 15)goto _LL1DA;_tmp1F0=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp1D5)->f1;
_tmp1F1=((struct Cyc_Absyn_Cast_e_struct*)_tmp1D5)->f2;_LL1D9:{const char*_tmp6A0;
const char*_tmp69F;struct Cyc_PP_Doc*_tmp69E[4];s=((_tmp69E[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F1),((_tmp69E[2]=Cyc_PP_text(((_tmp6A0=")",_tag_dyneither(_tmp6A0,
sizeof(char),2)))),((_tmp69E[1]=Cyc_Absynpp_typ2doc(_tmp1F0),((_tmp69E[0]=Cyc_PP_text(((
_tmp69F="(",_tag_dyneither(_tmp69F,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp69E,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1B9;_LL1DA: if(*((int*)
_tmp1D5)!= 16)goto _LL1DC;_tmp1F2=((struct Cyc_Absyn_Address_e_struct*)_tmp1D5)->f1;
_LL1DB:{const char*_tmp6A3;struct Cyc_PP_Doc*_tmp6A2[2];s=((_tmp6A2[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F2),((_tmp6A2[0]=Cyc_PP_text(((_tmp6A3="&",_tag_dyneither(_tmp6A3,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6A2,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1B9;_LL1DC: if(*((int*)_tmp1D5)!= 17)goto _LL1DE;_tmp1F3=((struct Cyc_Absyn_New_e_struct*)
_tmp1D5)->f1;_tmp1F4=((struct Cyc_Absyn_New_e_struct*)_tmp1D5)->f2;_LL1DD: if(
_tmp1F3 == 0){const char*_tmp6A6;struct Cyc_PP_Doc*_tmp6A5[2];s=((_tmp6A5[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F4),((_tmp6A5[0]=Cyc_PP_text(((_tmp6A6="new ",_tag_dyneither(_tmp6A6,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp6A5,sizeof(struct Cyc_PP_Doc*),2))))));}
else{const char*_tmp6AB;const char*_tmp6AA;struct Cyc_PP_Doc*_tmp6A9[4];s=((_tmp6A9[
3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F4),((_tmp6A9[2]=Cyc_PP_text(((_tmp6AB=") ",
_tag_dyneither(_tmp6AB,sizeof(char),3)))),((_tmp6A9[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp1F3),((_tmp6A9[0]=Cyc_PP_text(((_tmp6AA="rnew(",
_tag_dyneither(_tmp6AA,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp6A9,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1B9;_LL1DE: if(*((int*)_tmp1D5)!= 
18)goto _LL1E0;_tmp1F5=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp1D5)->f1;
_LL1DF:{const char*_tmp6B0;const char*_tmp6AF;struct Cyc_PP_Doc*_tmp6AE[3];s=((
_tmp6AE[2]=Cyc_PP_text(((_tmp6B0=")",_tag_dyneither(_tmp6B0,sizeof(char),2)))),((
_tmp6AE[1]=Cyc_Absynpp_typ2doc(_tmp1F5),((_tmp6AE[0]=Cyc_PP_text(((_tmp6AF="sizeof(",
_tag_dyneither(_tmp6AF,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp6AE,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1E0: if(*((int*)_tmp1D5)!= 19)
goto _LL1E2;_tmp1F6=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp1D5)->f1;_LL1E1:{
const char*_tmp6B5;const char*_tmp6B4;struct Cyc_PP_Doc*_tmp6B3[3];s=((_tmp6B3[2]=
Cyc_PP_text(((_tmp6B5=")",_tag_dyneither(_tmp6B5,sizeof(char),2)))),((_tmp6B3[1]=
Cyc_Absynpp_exp2doc(_tmp1F6),((_tmp6B3[0]=Cyc_PP_text(((_tmp6B4="sizeof(",
_tag_dyneither(_tmp6B4,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp6B3,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1E2: if(*((int*)_tmp1D5)!= 39)
goto _LL1E4;_tmp1F7=(void*)((struct Cyc_Absyn_Valueof_e_struct*)_tmp1D5)->f1;
_LL1E3:{const char*_tmp6BA;const char*_tmp6B9;struct Cyc_PP_Doc*_tmp6B8[3];s=((
_tmp6B8[2]=Cyc_PP_text(((_tmp6BA=")",_tag_dyneither(_tmp6BA,sizeof(char),2)))),((
_tmp6B8[1]=Cyc_Absynpp_typ2doc(_tmp1F7),((_tmp6B8[0]=Cyc_PP_text(((_tmp6B9="valueof(",
_tag_dyneither(_tmp6B9,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp6B8,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1E4: if(*((int*)_tmp1D5)!= 20)
goto _LL1E6;_tmp1F8=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D5)->f1;
_tmp1F9=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D5)->f2;if(*((int*)
_tmp1F9)!= 0)goto _LL1E6;_tmp1FA=((struct Cyc_Absyn_StructField_struct*)_tmp1F9)->f1;
_LL1E5:{const char*_tmp6C1;const char*_tmp6C0;const char*_tmp6BF;struct Cyc_PP_Doc*
_tmp6BE[5];s=((_tmp6BE[4]=Cyc_PP_text(((_tmp6C1=")",_tag_dyneither(_tmp6C1,
sizeof(char),2)))),((_tmp6BE[3]=Cyc_PP_textptr(_tmp1FA),((_tmp6BE[2]=Cyc_PP_text(((
_tmp6C0=",",_tag_dyneither(_tmp6C0,sizeof(char),2)))),((_tmp6BE[1]=Cyc_Absynpp_typ2doc(
_tmp1F8),((_tmp6BE[0]=Cyc_PP_text(((_tmp6BF="offsetof(",_tag_dyneither(_tmp6BF,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp6BE,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL1B9;_LL1E6: if(*((int*)_tmp1D5)!= 20)goto _LL1E8;_tmp1FB=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1D5)->f1;_tmp1FC=(void*)((struct
Cyc_Absyn_Offsetof_e_struct*)_tmp1D5)->f2;if(*((int*)_tmp1FC)!= 1)goto _LL1E8;
_tmp1FD=((struct Cyc_Absyn_TupleIndex_struct*)_tmp1FC)->f1;_LL1E7:{const char*
_tmp6CF;struct Cyc_Int_pa_struct _tmp6CE;void*_tmp6CD[1];const char*_tmp6CC;const
char*_tmp6CB;const char*_tmp6CA;struct Cyc_PP_Doc*_tmp6C9[5];s=((_tmp6C9[4]=Cyc_PP_text(((
_tmp6CF=")",_tag_dyneither(_tmp6CF,sizeof(char),2)))),((_tmp6C9[3]=Cyc_PP_text((
struct _dyneither_ptr)((_tmp6CE.tag=1,((_tmp6CE.f1=(unsigned long)((int)_tmp1FD),((
_tmp6CD[0]=& _tmp6CE,Cyc_aprintf(((_tmp6CC="%d",_tag_dyneither(_tmp6CC,sizeof(
char),3))),_tag_dyneither(_tmp6CD,sizeof(void*),1))))))))),((_tmp6C9[2]=Cyc_PP_text(((
_tmp6CB=",",_tag_dyneither(_tmp6CB,sizeof(char),2)))),((_tmp6C9[1]=Cyc_Absynpp_typ2doc(
_tmp1FB),((_tmp6C9[0]=Cyc_PP_text(((_tmp6CA="offsetof(",_tag_dyneither(_tmp6CA,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp6C9,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL1B9;_LL1E8: if(*((int*)_tmp1D5)!= 21)goto _LL1EA;_tmp1FE=((
struct Cyc_Absyn_Gentyp_e_struct*)_tmp1D5)->f1;_tmp1FF=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)
_tmp1D5)->f2;_LL1E9:{const char*_tmp6D4;const char*_tmp6D3;struct Cyc_PP_Doc*
_tmp6D2[4];s=((_tmp6D2[3]=Cyc_PP_text(((_tmp6D4=")",_tag_dyneither(_tmp6D4,
sizeof(char),2)))),((_tmp6D2[2]=Cyc_Absynpp_typ2doc(_tmp1FF),((_tmp6D2[1]=Cyc_Absynpp_tvars2doc(
_tmp1FE),((_tmp6D2[0]=Cyc_PP_text(((_tmp6D3="__gen(",_tag_dyneither(_tmp6D3,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp6D2,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1B9;_LL1EA: if(*((int*)_tmp1D5)!= 22)goto _LL1EC;_tmp200=((struct Cyc_Absyn_Deref_e_struct*)
_tmp1D5)->f1;_LL1EB:{const char*_tmp6D7;struct Cyc_PP_Doc*_tmp6D6[2];s=((_tmp6D6[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp200),((_tmp6D6[0]=Cyc_PP_text(((_tmp6D7="*",
_tag_dyneither(_tmp6D7,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6D6,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1B9;_LL1EC: if(*((int*)_tmp1D5)!= 23)
goto _LL1EE;_tmp201=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D5)->f1;_tmp202=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp1D5)->f2;_LL1ED:{const char*_tmp6DA;
struct Cyc_PP_Doc*_tmp6D9[3];s=((_tmp6D9[2]=Cyc_PP_textptr(_tmp202),((_tmp6D9[1]=
Cyc_PP_text(((_tmp6DA=".",_tag_dyneither(_tmp6DA,sizeof(char),2)))),((_tmp6D9[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp201),Cyc_PP_cat(_tag_dyneither(_tmp6D9,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1EE: if(*((int*)_tmp1D5)!= 24)
goto _LL1F0;_tmp203=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1D5)->f1;_tmp204=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1D5)->f2;_LL1EF:{const char*_tmp6DD;
struct Cyc_PP_Doc*_tmp6DC[3];s=((_tmp6DC[2]=Cyc_PP_textptr(_tmp204),((_tmp6DC[1]=
Cyc_PP_text(((_tmp6DD="->",_tag_dyneither(_tmp6DD,sizeof(char),3)))),((_tmp6DC[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp203),Cyc_PP_cat(_tag_dyneither(_tmp6DC,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1B9;_LL1F0: if(*((int*)_tmp1D5)!= 25)
goto _LL1F2;_tmp205=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1D5)->f1;_tmp206=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp1D5)->f2;_LL1F1:{const char*_tmp6E2;const
char*_tmp6E1;struct Cyc_PP_Doc*_tmp6E0[4];s=((_tmp6E0[3]=Cyc_PP_text(((_tmp6E2="]",
_tag_dyneither(_tmp6E2,sizeof(char),2)))),((_tmp6E0[2]=Cyc_Absynpp_exp2doc(
_tmp206),((_tmp6E0[1]=Cyc_PP_text(((_tmp6E1="[",_tag_dyneither(_tmp6E1,sizeof(
char),2)))),((_tmp6E0[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp205),Cyc_PP_cat(
_tag_dyneither(_tmp6E0,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1B9;_LL1F2:
if(*((int*)_tmp1D5)!= 26)goto _LL1F4;_tmp207=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp1D5)->f1;_LL1F3:{const char*_tmp6E7;const char*_tmp6E6;struct Cyc_PP_Doc*
_tmp6E5[4];s=((_tmp6E5[3]=Cyc_PP_text(((_tmp6E7=")",_tag_dyneither(_tmp6E7,
sizeof(char),2)))),((_tmp6E5[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp207),((_tmp6E5[
1]=Cyc_PP_text(((_tmp6E6="(",_tag_dyneither(_tmp6E6,sizeof(char),2)))),((_tmp6E5[
0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp6E5,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LL1B9;_LL1F4: if(*((int*)_tmp1D5)!= 27)goto _LL1F6;_tmp208=((
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp1D5)->f1;_tmp209=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp1D5)->f2;_LL1F5:{const char*_tmp6EE;const char*_tmp6ED;const char*_tmp6EC;
struct Cyc_PP_Doc*_tmp6EB[4];s=((_tmp6EB[3]=Cyc_Absynpp_group_braces(((_tmp6EE=",",
_tag_dyneither(_tmp6EE,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp209)),((_tmp6EB[2]=Cyc_PP_text(((_tmp6ED=")",_tag_dyneither(_tmp6ED,sizeof(
char),2)))),((_tmp6EB[1]=Cyc_Absynpp_typ2doc((*_tmp208).f3),((_tmp6EB[0]=Cyc_PP_text(((
_tmp6EC="(",_tag_dyneither(_tmp6EC,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp6EB,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1B9;_LL1F6: if(*((int*)
_tmp1D5)!= 28)goto _LL1F8;_tmp20A=((struct Cyc_Absyn_Array_e_struct*)_tmp1D5)->f1;
_LL1F7:{const char*_tmp6EF;s=Cyc_Absynpp_group_braces(((_tmp6EF=",",
_tag_dyneither(_tmp6EF,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp20A));}goto _LL1B9;_LL1F8: if(*((int*)_tmp1D5)!= 29)goto _LL1FA;_tmp20B=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D5)->f1;_tmp20C=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1D5)->f2;_tmp20D=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1D5)->f3;
_LL1F9:{const char*_tmp6F6;const char*_tmp6F5;const char*_tmp6F4;struct Cyc_PP_Doc*
_tmp6F3[8];s=((_tmp6F3[7]=Cyc_Absynpp_rb(),((_tmp6F3[6]=Cyc_Absynpp_exp2doc(
_tmp20D),((_tmp6F3[5]=Cyc_PP_text(((_tmp6F6=" : ",_tag_dyneither(_tmp6F6,sizeof(
char),4)))),((_tmp6F3[4]=Cyc_Absynpp_exp2doc(_tmp20C),((_tmp6F3[3]=Cyc_PP_text(((
_tmp6F5=" < ",_tag_dyneither(_tmp6F5,sizeof(char),4)))),((_tmp6F3[2]=Cyc_PP_text(*(*
_tmp20B->name).f2),((_tmp6F3[1]=Cyc_PP_text(((_tmp6F4="for ",_tag_dyneither(
_tmp6F4,sizeof(char),5)))),((_tmp6F3[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp6F3,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}goto _LL1B9;
_LL1FA: if(*((int*)_tmp1D5)!= 30)goto _LL1FC;_tmp20E=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1D5)->f1;_tmp20F=((struct Cyc_Absyn_Struct_e_struct*)_tmp1D5)->f2;_tmp210=((
struct Cyc_Absyn_Struct_e_struct*)_tmp1D5)->f3;_LL1FB: {struct Cyc_List_List*
_tmp276=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp210);{const char*_tmp6FB;
struct Cyc_List_List*_tmp6FA;struct Cyc_PP_Doc*_tmp6F9[2];s=((_tmp6F9[1]=Cyc_Absynpp_group_braces(((
_tmp6FB=",",_tag_dyneither(_tmp6FB,sizeof(char),2))),_tmp20F != 0?(_tmp6FA=
_cycalloc(sizeof(*_tmp6FA)),((_tmp6FA->hd=Cyc_Absynpp_tps2doc(_tmp20F),((_tmp6FA->tl=
_tmp276,_tmp6FA))))): _tmp276),((_tmp6F9[0]=Cyc_Absynpp_qvar2doc(_tmp20E),Cyc_PP_cat(
_tag_dyneither(_tmp6F9,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1B9;}_LL1FC: if(*((
int*)_tmp1D5)!= 31)goto _LL1FE;_tmp211=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1D5)->f2;_LL1FD:{const char*_tmp6FC;s=Cyc_Absynpp_group_braces(((_tmp6FC=",",
_tag_dyneither(_tmp6FC,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp211));}goto _LL1B9;_LL1FE: if(*((int*)_tmp1D5)!= 32)goto _LL200;_tmp212=((
struct Cyc_Absyn_Datatype_e_struct*)_tmp1D5)->f1;_tmp213=((struct Cyc_Absyn_Datatype_e_struct*)
_tmp1D5)->f3;_LL1FF: if(_tmp212 == 0)s=Cyc_Absynpp_qvar2doc(_tmp213->name);else{
const char*_tmp703;const char*_tmp702;const char*_tmp701;struct Cyc_PP_Doc*_tmp700[2];
s=((_tmp700[1]=Cyc_PP_egroup(((_tmp703="(",_tag_dyneither(_tmp703,sizeof(char),2))),((
_tmp702=")",_tag_dyneither(_tmp702,sizeof(char),2))),((_tmp701=",",
_tag_dyneither(_tmp701,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,
_tmp212)),((_tmp700[0]=Cyc_Absynpp_qvar2doc(_tmp213->name),Cyc_PP_cat(
_tag_dyneither(_tmp700,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1B9;_LL200: if(*((
int*)_tmp1D5)!= 33)goto _LL202;_tmp214=((struct Cyc_Absyn_Enum_e_struct*)_tmp1D5)->f1;
_LL201: s=Cyc_Absynpp_qvar2doc(_tmp214);goto _LL1B9;_LL202: if(*((int*)_tmp1D5)!= 
34)goto _LL204;_tmp215=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp1D5)->f1;_LL203: s=
Cyc_Absynpp_qvar2doc(_tmp215);goto _LL1B9;_LL204: if(*((int*)_tmp1D5)!= 35)goto
_LL206;_tmp216=((struct Cyc_Absyn_Malloc_e_struct*)_tmp1D5)->f1;_tmp217=_tmp216.is_calloc;
_tmp218=_tmp216.rgn;_tmp219=_tmp216.elt_type;_tmp21A=_tmp216.num_elts;_LL205: if(
_tmp217){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp219)),0);if(_tmp218 == 0){const char*_tmp70A;const char*_tmp709;const char*
_tmp708;struct Cyc_PP_Doc*_tmp707[5];s=((_tmp707[4]=Cyc_PP_text(((_tmp70A=")",
_tag_dyneither(_tmp70A,sizeof(char),2)))),((_tmp707[3]=Cyc_Absynpp_exp2doc(st),((
_tmp707[2]=Cyc_PP_text(((_tmp709=",",_tag_dyneither(_tmp709,sizeof(char),2)))),((
_tmp707[1]=Cyc_Absynpp_exp2doc(_tmp21A),((_tmp707[0]=Cyc_PP_text(((_tmp708="calloc(",
_tag_dyneither(_tmp708,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp707,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{const char*_tmp713;const char*
_tmp712;const char*_tmp711;const char*_tmp710;struct Cyc_PP_Doc*_tmp70F[7];s=((
_tmp70F[6]=Cyc_PP_text(((_tmp713=")",_tag_dyneither(_tmp713,sizeof(char),2)))),((
_tmp70F[5]=Cyc_Absynpp_exp2doc(st),((_tmp70F[4]=Cyc_PP_text(((_tmp712=",",
_tag_dyneither(_tmp712,sizeof(char),2)))),((_tmp70F[3]=Cyc_Absynpp_exp2doc(
_tmp21A),((_tmp70F[2]=Cyc_PP_text(((_tmp711=",",_tag_dyneither(_tmp711,sizeof(
char),2)))),((_tmp70F[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp218),((
_tmp70F[0]=Cyc_PP_text(((_tmp710="rcalloc(",_tag_dyneither(_tmp710,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp70F,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmp219 == 0)new_e=_tmp21A;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp219,0),_tmp21A,0);}if(_tmp218 == 0){const char*
_tmp718;const char*_tmp717;struct Cyc_PP_Doc*_tmp716[3];s=((_tmp716[2]=Cyc_PP_text(((
_tmp718=")",_tag_dyneither(_tmp718,sizeof(char),2)))),((_tmp716[1]=Cyc_Absynpp_exp2doc(
new_e),((_tmp716[0]=Cyc_PP_text(((_tmp717="malloc(",_tag_dyneither(_tmp717,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp716,sizeof(struct Cyc_PP_Doc*),3))))))));}
else{const char*_tmp71F;const char*_tmp71E;const char*_tmp71D;struct Cyc_PP_Doc*
_tmp71C[5];s=((_tmp71C[4]=Cyc_PP_text(((_tmp71F=")",_tag_dyneither(_tmp71F,
sizeof(char),2)))),((_tmp71C[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp71C[2]=Cyc_PP_text(((
_tmp71E=",",_tag_dyneither(_tmp71E,sizeof(char),2)))),((_tmp71C[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp218),((_tmp71C[0]=Cyc_PP_text(((_tmp71D="rmalloc(",
_tag_dyneither(_tmp71D,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp71C,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}}goto _LL1B9;_LL206: if(*((int*)_tmp1D5)
!= 36)goto _LL208;_tmp21B=((struct Cyc_Absyn_Swap_e_struct*)_tmp1D5)->f1;_tmp21C=((
struct Cyc_Absyn_Swap_e_struct*)_tmp1D5)->f2;_LL207:{const char*_tmp726;const char*
_tmp725;const char*_tmp724;struct Cyc_PP_Doc*_tmp723[5];s=((_tmp723[4]=Cyc_PP_text(((
_tmp726=")",_tag_dyneither(_tmp726,sizeof(char),2)))),((_tmp723[3]=Cyc_Absynpp_exp2doc(
_tmp21C),((_tmp723[2]=Cyc_PP_text(((_tmp725=",",_tag_dyneither(_tmp725,sizeof(
char),2)))),((_tmp723[1]=Cyc_Absynpp_exp2doc(_tmp21B),((_tmp723[0]=Cyc_PP_text(((
_tmp724="cycswap(",_tag_dyneither(_tmp724,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp723,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1B9;
_LL208: if(*((int*)_tmp1D5)!= 37)goto _LL20A;_tmp21D=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp1D5)->f1;_tmp21E=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1D5)->f2;
_LL209:{const char*_tmp727;s=Cyc_Absynpp_group_braces(((_tmp727=",",
_tag_dyneither(_tmp727,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp21E));}goto _LL1B9;_LL20A: if(*((int*)_tmp1D5)!= 38)goto _LL1B9;_tmp21F=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp1D5)->f1;_LL20B:{const char*_tmp72C;const
char*_tmp72B;struct Cyc_PP_Doc*_tmp72A[7];s=((_tmp72A[6]=Cyc_PP_text(((_tmp72C=")",
_tag_dyneither(_tmp72C,sizeof(char),2)))),((_tmp72A[5]=Cyc_Absynpp_rb(),((
_tmp72A[4]=Cyc_PP_blank_doc(),((_tmp72A[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp21F)),((_tmp72A[2]=Cyc_PP_blank_doc(),((_tmp72A[1]=Cyc_Absynpp_lb(),((
_tmp72A[0]=Cyc_PP_text(((_tmp72B="(",_tag_dyneither(_tmp72B,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp72A,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
goto _LL1B9;_LL1B9:;}if(inprec >= myprec){const char*_tmp731;const char*_tmp730;
struct Cyc_PP_Doc*_tmp72F[3];s=((_tmp72F[2]=Cyc_PP_text(((_tmp731=")",
_tag_dyneither(_tmp731,sizeof(char),2)))),((_tmp72F[1]=s,((_tmp72F[0]=Cyc_PP_text(((
_tmp730="(",_tag_dyneither(_tmp730,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp72F,sizeof(struct Cyc_PP_Doc*),3))))))));}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(
void*d);struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){void*_tmp29A=d;
struct Cyc_Absyn_Exp*_tmp29B;struct _dyneither_ptr*_tmp29C;_LL216: if(*((int*)
_tmp29A)!= 0)goto _LL218;_tmp29B=((struct Cyc_Absyn_ArrayElement_struct*)_tmp29A)->f1;
_LL217: {const char*_tmp736;const char*_tmp735;struct Cyc_PP_Doc*_tmp734[3];return(
_tmp734[2]=Cyc_PP_text(((_tmp736="]",_tag_dyneither(_tmp736,sizeof(char),2)))),((
_tmp734[1]=Cyc_Absynpp_exp2doc(_tmp29B),((_tmp734[0]=Cyc_PP_text(((_tmp735=".[",
_tag_dyneither(_tmp735,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp734,
sizeof(struct Cyc_PP_Doc*),3)))))));}_LL218: if(*((int*)_tmp29A)!= 1)goto _LL215;
_tmp29C=((struct Cyc_Absyn_FieldName_struct*)_tmp29A)->f1;_LL219: {const char*
_tmp739;struct Cyc_PP_Doc*_tmp738[2];return(_tmp738[1]=Cyc_PP_textptr(_tmp29C),((
_tmp738[0]=Cyc_PP_text(((_tmp739=".",_tag_dyneither(_tmp739,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp738,sizeof(struct Cyc_PP_Doc*),2)))));}_LL215:;}
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple9*de);struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(
struct _tuple9*de){if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{const
char*_tmp740;const char*_tmp73F;const char*_tmp73E;struct Cyc_PP_Doc*_tmp73D[2];
return(_tmp73D[1]=Cyc_Absynpp_exp2doc((*de).f2),((_tmp73D[0]=Cyc_PP_egroup(((
_tmp740="",_tag_dyneither(_tmp740,sizeof(char),1))),((_tmp73F="=",_tag_dyneither(
_tmp73F,sizeof(char),2))),((_tmp73E="=",_tag_dyneither(_tmp73E,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp73D,sizeof(
struct Cyc_PP_Doc*),2)))));}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es){const char*_tmp743;const char*_tmp742;const char*_tmp741;
return Cyc_PP_group(((_tmp741="",_tag_dyneither(_tmp741,sizeof(char),1))),((
_tmp742="",_tag_dyneither(_tmp742,sizeof(char),1))),((_tmp743=",",_tag_dyneither(
_tmp743,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst_union c);
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst_union c){union Cyc_Absyn_Cnst_union
_tmp2A9=c;void*_tmp2AA;char _tmp2AB;void*_tmp2AC;short _tmp2AD;void*_tmp2AE;int
_tmp2AF;void*_tmp2B0;int _tmp2B1;void*_tmp2B2;int _tmp2B3;void*_tmp2B4;long long
_tmp2B5;struct _dyneither_ptr _tmp2B6;struct _dyneither_ptr _tmp2B7;_LL21B: if((
_tmp2A9.Char_c).tag != 0)goto _LL21D;_tmp2AA=(_tmp2A9.Char_c).f1;_tmp2AB=(_tmp2A9.Char_c).f2;
_LL21C: {const char*_tmp747;void*_tmp746[1];struct Cyc_String_pa_struct _tmp745;
return Cyc_PP_text((struct _dyneither_ptr)((_tmp745.tag=0,((_tmp745.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp2AB)),((
_tmp746[0]=& _tmp745,Cyc_aprintf(((_tmp747="'%s'",_tag_dyneither(_tmp747,sizeof(
char),5))),_tag_dyneither(_tmp746,sizeof(void*),1)))))))));}_LL21D: if((_tmp2A9.Short_c).tag
!= 1)goto _LL21F;_tmp2AC=(_tmp2A9.Short_c).f1;_tmp2AD=(_tmp2A9.Short_c).f2;_LL21E: {
const char*_tmp74B;void*_tmp74A[1];struct Cyc_Int_pa_struct _tmp749;return Cyc_PP_text((
struct _dyneither_ptr)((_tmp749.tag=1,((_tmp749.f1=(unsigned long)((int)_tmp2AD),((
_tmp74A[0]=& _tmp749,Cyc_aprintf(((_tmp74B="%d",_tag_dyneither(_tmp74B,sizeof(
char),3))),_tag_dyneither(_tmp74A,sizeof(void*),1)))))))));}_LL21F: if((_tmp2A9.Int_c).tag
!= 2)goto _LL221;_tmp2AE=(_tmp2A9.Int_c).f1;if((int)_tmp2AE != 2)goto _LL221;
_tmp2AF=(_tmp2A9.Int_c).f2;_LL220: _tmp2B1=_tmp2AF;goto _LL222;_LL221: if((_tmp2A9.Int_c).tag
!= 2)goto _LL223;_tmp2B0=(_tmp2A9.Int_c).f1;if((int)_tmp2B0 != 0)goto _LL223;
_tmp2B1=(_tmp2A9.Int_c).f2;_LL222: {const char*_tmp74F;void*_tmp74E[1];struct Cyc_Int_pa_struct
_tmp74D;return Cyc_PP_text((struct _dyneither_ptr)((_tmp74D.tag=1,((_tmp74D.f1=(
unsigned long)_tmp2B1,((_tmp74E[0]=& _tmp74D,Cyc_aprintf(((_tmp74F="%d",
_tag_dyneither(_tmp74F,sizeof(char),3))),_tag_dyneither(_tmp74E,sizeof(void*),1)))))))));}
_LL223: if((_tmp2A9.Int_c).tag != 2)goto _LL225;_tmp2B2=(_tmp2A9.Int_c).f1;if((int)
_tmp2B2 != 1)goto _LL225;_tmp2B3=(_tmp2A9.Int_c).f2;_LL224: {const char*_tmp753;
void*_tmp752[1];struct Cyc_Int_pa_struct _tmp751;return Cyc_PP_text((struct
_dyneither_ptr)((_tmp751.tag=1,((_tmp751.f1=(unsigned int)_tmp2B3,((_tmp752[0]=&
_tmp751,Cyc_aprintf(((_tmp753="%u",_tag_dyneither(_tmp753,sizeof(char),3))),
_tag_dyneither(_tmp752,sizeof(void*),1)))))))));}_LL225: if((_tmp2A9.LongLong_c).tag
!= 3)goto _LL227;_tmp2B4=(_tmp2A9.LongLong_c).f1;_tmp2B5=(_tmp2A9.LongLong_c).f2;
_LL226: {const char*_tmp754;return Cyc_PP_text(((_tmp754="<<FIX LONG LONG CONSTANT>>",
_tag_dyneither(_tmp754,sizeof(char),27))));}_LL227: if((_tmp2A9.Float_c).tag != 4)
goto _LL229;_tmp2B6=(_tmp2A9.Float_c).f1;_LL228: return Cyc_PP_text(_tmp2B6);_LL229:
if((_tmp2A9.Null_c).tag != 6)goto _LL22B;_LL22A: {const char*_tmp755;return Cyc_PP_text(((
_tmp755="NULL",_tag_dyneither(_tmp755,sizeof(char),5))));}_LL22B: if((_tmp2A9.String_c).tag
!= 5)goto _LL21A;_tmp2B7=(_tmp2A9.String_c).f1;_LL22C: {const char*_tmp75A;const
char*_tmp759;struct Cyc_PP_Doc*_tmp758[3];return(_tmp758[2]=Cyc_PP_text(((_tmp75A="\"",
_tag_dyneither(_tmp75A,sizeof(char),2)))),((_tmp758[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(
_tmp2B7)),((_tmp758[0]=Cyc_PP_text(((_tmp759="\"",_tag_dyneither(_tmp759,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp758,sizeof(struct Cyc_PP_Doc*),3)))))));}
_LL21A:;}struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*
es);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*
es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (void*)((void*)19)){if(es
== 0  || es->tl != 0){struct Cyc_Core_Failure_struct _tmp767;const char*_tmp766;void*
_tmp765[1];struct Cyc_String_pa_struct _tmp764;struct Cyc_Core_Failure_struct*
_tmp763;(int)_throw((void*)((_tmp763=_cycalloc(sizeof(*_tmp763)),((_tmp763[0]=((
_tmp767.tag=Cyc_Core_Failure,((_tmp767.f1=(struct _dyneither_ptr)((_tmp764.tag=0,((
_tmp764.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp765[0]=& _tmp764,Cyc_aprintf(((_tmp766="Absynpp::primapp2doc Numelts: %s with bad args",
_tag_dyneither(_tmp766,sizeof(char),47))),_tag_dyneither(_tmp765,sizeof(void*),1)))))))),
_tmp767)))),_tmp763)))));}{const char*_tmp76C;const char*_tmp76B;struct Cyc_PP_Doc*
_tmp76A[3];return(_tmp76A[2]=Cyc_PP_text(((_tmp76C=")",_tag_dyneither(_tmp76C,
sizeof(char),2)))),((_tmp76A[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((
_tmp76A[0]=Cyc_PP_text(((_tmp76B="numelts(",_tag_dyneither(_tmp76B,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp76A,sizeof(struct Cyc_PP_Doc*),3)))))));}}
else{struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es);if(ds == 0){struct Cyc_Core_Failure_struct _tmp779;const char*_tmp778;
void*_tmp777[1];struct Cyc_String_pa_struct _tmp776;struct Cyc_Core_Failure_struct*
_tmp775;(int)_throw((void*)((_tmp775=_cycalloc(sizeof(*_tmp775)),((_tmp775[0]=((
_tmp779.tag=Cyc_Core_Failure,((_tmp779.f1=(struct _dyneither_ptr)((_tmp776.tag=0,((
_tmp776.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp777[0]=& _tmp776,Cyc_aprintf(((_tmp778="Absynpp::primapp2doc: %s with no args",
_tag_dyneither(_tmp778,sizeof(char),38))),_tag_dyneither(_tmp777,sizeof(void*),1)))))))),
_tmp779)))),_tmp775)))));}else{if(ds->tl == 0){const char*_tmp77C;struct Cyc_PP_Doc*
_tmp77B[3];return(_tmp77B[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp77B[1]=Cyc_PP_text(((
_tmp77C=" ",_tag_dyneither(_tmp77C,sizeof(char),2)))),((_tmp77B[0]=ps,Cyc_PP_cat(
_tag_dyneither(_tmp77B,sizeof(struct Cyc_PP_Doc*),3)))))));}else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0){struct Cyc_Core_Failure_struct _tmp789;const char*
_tmp788;void*_tmp787[1];struct Cyc_String_pa_struct _tmp786;struct Cyc_Core_Failure_struct*
_tmp785;(int)_throw((void*)((_tmp785=_cycalloc(sizeof(*_tmp785)),((_tmp785[0]=((
_tmp789.tag=Cyc_Core_Failure,((_tmp789.f1=(struct _dyneither_ptr)((_tmp786.tag=0,((
_tmp786.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp787[0]=& _tmp786,Cyc_aprintf(((_tmp788="Absynpp::primapp2doc: %s with more than 2 args",
_tag_dyneither(_tmp788,sizeof(char),47))),_tag_dyneither(_tmp787,sizeof(void*),1)))))))),
_tmp789)))),_tmp785)))));}else{const char*_tmp78E;const char*_tmp78D;struct Cyc_PP_Doc*
_tmp78C[5];return(_tmp78C[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd,((_tmp78C[3]=Cyc_PP_text(((_tmp78E=" ",_tag_dyneither(
_tmp78E,sizeof(char),2)))),((_tmp78C[2]=ps,((_tmp78C[1]=Cyc_PP_text(((_tmp78D=" ",
_tag_dyneither(_tmp78D,sizeof(char),2)))),((_tmp78C[0]=(struct Cyc_PP_Doc*)ds->hd,
Cyc_PP_cat(_tag_dyneither(_tmp78C,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
struct _dyneither_ptr Cyc_Absynpp_prim2str(void*p);struct _dyneither_ptr Cyc_Absynpp_prim2str(
void*p){void*_tmp2E0=p;_LL22E: if((int)_tmp2E0 != 0)goto _LL230;_LL22F: {const char*
_tmp78F;return(_tmp78F="+",_tag_dyneither(_tmp78F,sizeof(char),2));}_LL230: if((
int)_tmp2E0 != 1)goto _LL232;_LL231: {const char*_tmp790;return(_tmp790="*",
_tag_dyneither(_tmp790,sizeof(char),2));}_LL232: if((int)_tmp2E0 != 2)goto _LL234;
_LL233: {const char*_tmp791;return(_tmp791="-",_tag_dyneither(_tmp791,sizeof(char),
2));}_LL234: if((int)_tmp2E0 != 3)goto _LL236;_LL235: {const char*_tmp792;return(
_tmp792="/",_tag_dyneither(_tmp792,sizeof(char),2));}_LL236: if((int)_tmp2E0 != 4)
goto _LL238;_LL237: {const char*_tmp793;return(_tmp793="%",_tag_dyneither(_tmp793,
sizeof(char),2));}_LL238: if((int)_tmp2E0 != 5)goto _LL23A;_LL239: {const char*
_tmp794;return(_tmp794="==",_tag_dyneither(_tmp794,sizeof(char),3));}_LL23A: if((
int)_tmp2E0 != 6)goto _LL23C;_LL23B: {const char*_tmp795;return(_tmp795="!=",
_tag_dyneither(_tmp795,sizeof(char),3));}_LL23C: if((int)_tmp2E0 != 7)goto _LL23E;
_LL23D: {const char*_tmp796;return(_tmp796=">",_tag_dyneither(_tmp796,sizeof(char),
2));}_LL23E: if((int)_tmp2E0 != 8)goto _LL240;_LL23F: {const char*_tmp797;return(
_tmp797="<",_tag_dyneither(_tmp797,sizeof(char),2));}_LL240: if((int)_tmp2E0 != 9)
goto _LL242;_LL241: {const char*_tmp798;return(_tmp798=">=",_tag_dyneither(_tmp798,
sizeof(char),3));}_LL242: if((int)_tmp2E0 != 10)goto _LL244;_LL243: {const char*
_tmp799;return(_tmp799="<=",_tag_dyneither(_tmp799,sizeof(char),3));}_LL244: if((
int)_tmp2E0 != 11)goto _LL246;_LL245: {const char*_tmp79A;return(_tmp79A="!",
_tag_dyneither(_tmp79A,sizeof(char),2));}_LL246: if((int)_tmp2E0 != 12)goto _LL248;
_LL247: {const char*_tmp79B;return(_tmp79B="~",_tag_dyneither(_tmp79B,sizeof(char),
2));}_LL248: if((int)_tmp2E0 != 13)goto _LL24A;_LL249: {const char*_tmp79C;return(
_tmp79C="&",_tag_dyneither(_tmp79C,sizeof(char),2));}_LL24A: if((int)_tmp2E0 != 14)
goto _LL24C;_LL24B: {const char*_tmp79D;return(_tmp79D="|",_tag_dyneither(_tmp79D,
sizeof(char),2));}_LL24C: if((int)_tmp2E0 != 15)goto _LL24E;_LL24D: {const char*
_tmp79E;return(_tmp79E="^",_tag_dyneither(_tmp79E,sizeof(char),2));}_LL24E: if((
int)_tmp2E0 != 16)goto _LL250;_LL24F: {const char*_tmp79F;return(_tmp79F="<<",
_tag_dyneither(_tmp79F,sizeof(char),3));}_LL250: if((int)_tmp2E0 != 17)goto _LL252;
_LL251: {const char*_tmp7A0;return(_tmp7A0=">>",_tag_dyneither(_tmp7A0,sizeof(
char),3));}_LL252: if((int)_tmp2E0 != 18)goto _LL254;_LL253: {const char*_tmp7A1;
return(_tmp7A1=">>>",_tag_dyneither(_tmp7A1,sizeof(char),4));}_LL254: if((int)
_tmp2E0 != 19)goto _LL22D;_LL255: {const char*_tmp7A2;return(_tmp7A2="numelts",
_tag_dyneither(_tmp7A2,sizeof(char),8));}_LL22D:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
void*p);struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*p){return Cyc_PP_text(Cyc_Absynpp_prim2str(
p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp2F5=(void*)s->r;_LL257: if(_tmp2F5 <= (void*)1)
goto _LL259;if(*((int*)_tmp2F5)!= 11)goto _LL259;_LL258: return 1;_LL259:;_LL25A:
return 0;_LL256:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st);
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st){struct Cyc_PP_Doc*
s;{void*_tmp2F6=(void*)st->r;struct Cyc_Absyn_Exp*_tmp2F7;struct Cyc_Absyn_Stmt*
_tmp2F8;struct Cyc_Absyn_Stmt*_tmp2F9;struct Cyc_Absyn_Exp*_tmp2FA;struct Cyc_Absyn_Exp*
_tmp2FB;struct Cyc_Absyn_Stmt*_tmp2FC;struct Cyc_Absyn_Stmt*_tmp2FD;struct _tuple2
_tmp2FE;struct Cyc_Absyn_Exp*_tmp2FF;struct Cyc_Absyn_Stmt*_tmp300;struct
_dyneither_ptr*_tmp301;struct Cyc_Absyn_Exp*_tmp302;struct _tuple2 _tmp303;struct
Cyc_Absyn_Exp*_tmp304;struct _tuple2 _tmp305;struct Cyc_Absyn_Exp*_tmp306;struct Cyc_Absyn_Stmt*
_tmp307;struct Cyc_Absyn_Exp*_tmp308;struct Cyc_List_List*_tmp309;struct Cyc_List_List*
_tmp30A;struct Cyc_List_List*_tmp30B;struct Cyc_Absyn_Decl*_tmp30C;struct Cyc_Absyn_Stmt*
_tmp30D;struct _dyneither_ptr*_tmp30E;struct Cyc_Absyn_Stmt*_tmp30F;struct Cyc_Absyn_Stmt*
_tmp310;struct _tuple2 _tmp311;struct Cyc_Absyn_Exp*_tmp312;struct Cyc_Absyn_Stmt*
_tmp313;struct Cyc_List_List*_tmp314;struct Cyc_Absyn_Exp*_tmp315;_LL25C: if((int)
_tmp2F6 != 0)goto _LL25E;_LL25D:{const char*_tmp7A3;s=Cyc_PP_text(((_tmp7A3=";",
_tag_dyneither(_tmp7A3,sizeof(char),2))));}goto _LL25B;_LL25E: if(_tmp2F6 <= (void*)
1)goto _LL260;if(*((int*)_tmp2F6)!= 0)goto _LL260;_tmp2F7=((struct Cyc_Absyn_Exp_s_struct*)
_tmp2F6)->f1;_LL25F:{const char*_tmp7A6;struct Cyc_PP_Doc*_tmp7A5[2];s=((_tmp7A5[1]=
Cyc_PP_text(((_tmp7A6=";",_tag_dyneither(_tmp7A6,sizeof(char),2)))),((_tmp7A5[0]=
Cyc_Absynpp_exp2doc(_tmp2F7),Cyc_PP_cat(_tag_dyneither(_tmp7A5,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL25B;_LL260: if(_tmp2F6 <= (void*)1)goto _LL262;if(*((int*)_tmp2F6)
!= 1)goto _LL262;_tmp2F8=((struct Cyc_Absyn_Seq_s_struct*)_tmp2F6)->f1;_tmp2F9=((
struct Cyc_Absyn_Seq_s_struct*)_tmp2F6)->f2;_LL261: if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp2F8)){struct Cyc_PP_Doc*_tmp7A9[5];struct Cyc_PP_Doc*
_tmp7A8[7];s=((_tmp7A8[6]=Cyc_Absynpp_is_declaration(_tmp2F9)?(_tmp7A9[4]=Cyc_PP_line_doc(),((
_tmp7A9[3]=Cyc_Absynpp_rb(),((_tmp7A9[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp2F9)),((_tmp7A9[1]=Cyc_PP_blank_doc(),((_tmp7A9[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp7A9,sizeof(struct Cyc_PP_Doc*),5))))))))))): Cyc_Absynpp_stmt2doc(
_tmp2F9),((_tmp7A8[5]=Cyc_PP_line_doc(),((_tmp7A8[4]=Cyc_Absynpp_rb(),((_tmp7A8[
3]=Cyc_PP_line_doc(),((_tmp7A8[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2F8)),((
_tmp7A8[1]=Cyc_PP_blank_doc(),((_tmp7A8[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dyneither(_tmp7A8,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{if(Cyc_Absynpp_is_declaration(
_tmp2F9)){struct Cyc_PP_Doc*_tmp7AA[7];s=((_tmp7AA[6]=Cyc_PP_line_doc(),((_tmp7AA[
5]=Cyc_Absynpp_rb(),((_tmp7AA[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2F9)),((
_tmp7AA[3]=Cyc_PP_blank_doc(),((_tmp7AA[2]=Cyc_Absynpp_lb(),((_tmp7AA[1]=Cyc_PP_line_doc(),((
_tmp7AA[0]=Cyc_Absynpp_stmt2doc(_tmp2F8),Cyc_PP_cat(_tag_dyneither(_tmp7AA,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{struct Cyc_List_List*_tmp7AE;
struct Cyc_List_List*_tmp7AD;const char*_tmp7AB;s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Stmt*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,((_tmp7AB="",_tag_dyneither(_tmp7AB,sizeof(char),1))),((
_tmp7AD=_cycalloc(sizeof(*_tmp7AD)),((_tmp7AD->hd=_tmp2F8,((_tmp7AD->tl=((
_tmp7AE=_cycalloc(sizeof(*_tmp7AE)),((_tmp7AE->hd=_tmp2F9,((_tmp7AE->tl=0,
_tmp7AE)))))),_tmp7AD)))))));}}}else{struct Cyc_List_List*_tmp7B2;struct Cyc_List_List*
_tmp7B1;const char*_tmp7AF;s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct
Cyc_Absyn_Stmt*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,((_tmp7AF="",_tag_dyneither(_tmp7AF,sizeof(char),1))),((
_tmp7B1=_cycalloc(sizeof(*_tmp7B1)),((_tmp7B1->hd=_tmp2F8,((_tmp7B1->tl=((
_tmp7B2=_cycalloc(sizeof(*_tmp7B2)),((_tmp7B2->hd=_tmp2F9,((_tmp7B2->tl=0,
_tmp7B2)))))),_tmp7B1)))))));}goto _LL25B;_LL262: if(_tmp2F6 <= (void*)1)goto _LL264;
if(*((int*)_tmp2F6)!= 2)goto _LL264;_tmp2FA=((struct Cyc_Absyn_Return_s_struct*)
_tmp2F6)->f1;_LL263: if(_tmp2FA == 0){const char*_tmp7B3;s=Cyc_PP_text(((_tmp7B3="return;",
_tag_dyneither(_tmp7B3,sizeof(char),8))));}else{const char*_tmp7B8;const char*
_tmp7B7;struct Cyc_PP_Doc*_tmp7B6[3];s=((_tmp7B6[2]=Cyc_PP_text(((_tmp7B8=";",
_tag_dyneither(_tmp7B8,sizeof(char),2)))),((_tmp7B6[1]=_tmp2FA == 0?Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2FA),((_tmp7B6[0]=Cyc_PP_text(((
_tmp7B7="return ",_tag_dyneither(_tmp7B7,sizeof(char),8)))),Cyc_PP_cat(
_tag_dyneither(_tmp7B6,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL25B;_LL264:
if(_tmp2F6 <= (void*)1)goto _LL266;if(*((int*)_tmp2F6)!= 3)goto _LL266;_tmp2FB=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2F6)->f1;_tmp2FC=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp2F6)->f2;_tmp2FD=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2F6)->f3;_LL265: {
int print_else;{void*_tmp326=(void*)_tmp2FD->r;_LL281: if((int)_tmp326 != 0)goto
_LL283;_LL282: print_else=0;goto _LL280;_LL283:;_LL284: print_else=1;goto _LL280;
_LL280:;}{struct Cyc_PP_Doc*_tmp7C7[6];const char*_tmp7C6;struct Cyc_PP_Doc*_tmp7C5[
2];struct Cyc_PP_Doc*_tmp7C4[2];const char*_tmp7C3;const char*_tmp7C2;struct Cyc_PP_Doc*
_tmp7C1[8];s=((_tmp7C1[7]=print_else?(_tmp7C7[5]=Cyc_Absynpp_rb(),((_tmp7C7[4]=
Cyc_PP_line_doc(),((_tmp7C7[3]=Cyc_PP_nest(2,((_tmp7C5[1]=Cyc_Absynpp_stmt2doc(
_tmp2FD),((_tmp7C5[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp7C5,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp7C7[2]=Cyc_Absynpp_lb(),((_tmp7C7[1]=
Cyc_PP_text(((_tmp7C6="else ",_tag_dyneither(_tmp7C6,sizeof(char),6)))),((
_tmp7C7[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp7C7,sizeof(struct Cyc_PP_Doc*),
6))))))))))))): Cyc_PP_nil_doc(),((_tmp7C1[6]=Cyc_Absynpp_rb(),((_tmp7C1[5]=Cyc_PP_line_doc(),((
_tmp7C1[4]=Cyc_PP_nest(2,((_tmp7C4[1]=Cyc_Absynpp_stmt2doc(_tmp2FC),((_tmp7C4[0]=
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp7C4,sizeof(struct Cyc_PP_Doc*),2))))))),((
_tmp7C1[3]=Cyc_Absynpp_lb(),((_tmp7C1[2]=Cyc_PP_text(((_tmp7C3=") ",
_tag_dyneither(_tmp7C3,sizeof(char),3)))),((_tmp7C1[1]=Cyc_Absynpp_exp2doc(
_tmp2FB),((_tmp7C1[0]=Cyc_PP_text(((_tmp7C2="if (",_tag_dyneither(_tmp7C2,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp7C1,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL25B;}_LL266: if(_tmp2F6 <= (void*)1)goto _LL268;if(*((int*)_tmp2F6)!= 4)goto
_LL268;_tmp2FE=((struct Cyc_Absyn_While_s_struct*)_tmp2F6)->f1;_tmp2FF=_tmp2FE.f1;
_tmp300=((struct Cyc_Absyn_While_s_struct*)_tmp2F6)->f2;_LL267:{struct Cyc_PP_Doc*
_tmp7CE[2];const char*_tmp7CD;const char*_tmp7CC;struct Cyc_PP_Doc*_tmp7CB[7];s=((
_tmp7CB[6]=Cyc_Absynpp_rb(),((_tmp7CB[5]=Cyc_PP_line_doc(),((_tmp7CB[4]=Cyc_PP_nest(
2,((_tmp7CE[1]=Cyc_Absynpp_stmt2doc(_tmp300),((_tmp7CE[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp7CE,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp7CB[3]=Cyc_Absynpp_lb(),((
_tmp7CB[2]=Cyc_PP_text(((_tmp7CD=") ",_tag_dyneither(_tmp7CD,sizeof(char),3)))),((
_tmp7CB[1]=Cyc_Absynpp_exp2doc(_tmp2FF),((_tmp7CB[0]=Cyc_PP_text(((_tmp7CC="while (",
_tag_dyneither(_tmp7CC,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7CB,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL25B;_LL268: if(_tmp2F6 <= (
void*)1)goto _LL26A;if(*((int*)_tmp2F6)!= 5)goto _LL26A;_LL269:{const char*_tmp7CF;
s=Cyc_PP_text(((_tmp7CF="break;",_tag_dyneither(_tmp7CF,sizeof(char),7))));}goto
_LL25B;_LL26A: if(_tmp2F6 <= (void*)1)goto _LL26C;if(*((int*)_tmp2F6)!= 6)goto
_LL26C;_LL26B:{const char*_tmp7D0;s=Cyc_PP_text(((_tmp7D0="continue;",
_tag_dyneither(_tmp7D0,sizeof(char),10))));}goto _LL25B;_LL26C: if(_tmp2F6 <= (void*)
1)goto _LL26E;if(*((int*)_tmp2F6)!= 7)goto _LL26E;_tmp301=((struct Cyc_Absyn_Goto_s_struct*)
_tmp2F6)->f1;_LL26D:{const char*_tmp7D4;void*_tmp7D3[1];struct Cyc_String_pa_struct
_tmp7D2;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp7D2.tag=0,((_tmp7D2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp301),((_tmp7D3[0]=& _tmp7D2,Cyc_aprintf(((
_tmp7D4="goto %s;",_tag_dyneither(_tmp7D4,sizeof(char),9))),_tag_dyneither(
_tmp7D3,sizeof(void*),1)))))))));}goto _LL25B;_LL26E: if(_tmp2F6 <= (void*)1)goto
_LL270;if(*((int*)_tmp2F6)!= 8)goto _LL270;_tmp302=((struct Cyc_Absyn_For_s_struct*)
_tmp2F6)->f1;_tmp303=((struct Cyc_Absyn_For_s_struct*)_tmp2F6)->f2;_tmp304=
_tmp303.f1;_tmp305=((struct Cyc_Absyn_For_s_struct*)_tmp2F6)->f3;_tmp306=_tmp305.f1;
_tmp307=((struct Cyc_Absyn_For_s_struct*)_tmp2F6)->f4;_LL26F:{struct Cyc_PP_Doc*
_tmp7DF[2];const char*_tmp7DE;const char*_tmp7DD;const char*_tmp7DC;const char*
_tmp7DB;struct Cyc_PP_Doc*_tmp7DA[11];s=((_tmp7DA[10]=Cyc_Absynpp_rb(),((_tmp7DA[
9]=Cyc_PP_line_doc(),((_tmp7DA[8]=Cyc_PP_nest(2,((_tmp7DF[1]=Cyc_Absynpp_stmt2doc(
_tmp307),((_tmp7DF[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp7DF,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp7DA[7]=Cyc_Absynpp_lb(),((_tmp7DA[6]=
Cyc_PP_text(((_tmp7DE=") ",_tag_dyneither(_tmp7DE,sizeof(char),3)))),((_tmp7DA[5]=
Cyc_Absynpp_exp2doc(_tmp306),((_tmp7DA[4]=Cyc_PP_text(((_tmp7DD="; ",
_tag_dyneither(_tmp7DD,sizeof(char),3)))),((_tmp7DA[3]=Cyc_Absynpp_exp2doc(
_tmp304),((_tmp7DA[2]=Cyc_PP_text(((_tmp7DC="; ",_tag_dyneither(_tmp7DC,sizeof(
char),3)))),((_tmp7DA[1]=Cyc_Absynpp_exp2doc(_tmp302),((_tmp7DA[0]=Cyc_PP_text(((
_tmp7DB="for(",_tag_dyneither(_tmp7DB,sizeof(char),5)))),Cyc_PP_cat(
_tag_dyneither(_tmp7DA,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL25B;_LL270: if(_tmp2F6 <= (void*)1)goto _LL272;if(*((int*)_tmp2F6)!= 9)goto
_LL272;_tmp308=((struct Cyc_Absyn_Switch_s_struct*)_tmp2F6)->f1;_tmp309=((struct
Cyc_Absyn_Switch_s_struct*)_tmp2F6)->f2;_LL271:{const char*_tmp7E4;const char*
_tmp7E3;struct Cyc_PP_Doc*_tmp7E2[8];s=((_tmp7E2[7]=Cyc_Absynpp_rb(),((_tmp7E2[6]=
Cyc_PP_line_doc(),((_tmp7E2[5]=Cyc_Absynpp_switchclauses2doc(_tmp309),((_tmp7E2[
4]=Cyc_PP_line_doc(),((_tmp7E2[3]=Cyc_Absynpp_lb(),((_tmp7E2[2]=Cyc_PP_text(((
_tmp7E4=") ",_tag_dyneither(_tmp7E4,sizeof(char),3)))),((_tmp7E2[1]=Cyc_Absynpp_exp2doc(
_tmp308),((_tmp7E2[0]=Cyc_PP_text(((_tmp7E3="switch (",_tag_dyneither(_tmp7E3,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7E2,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL25B;_LL272: if(_tmp2F6 <= (void*)1)goto _LL274;if(*((int*)_tmp2F6)!= 10)goto
_LL274;_tmp30A=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp2F6)->f1;if(_tmp30A != 0)
goto _LL274;_LL273:{const char*_tmp7E5;s=Cyc_PP_text(((_tmp7E5="fallthru;",
_tag_dyneither(_tmp7E5,sizeof(char),10))));}goto _LL25B;_LL274: if(_tmp2F6 <= (void*)
1)goto _LL276;if(*((int*)_tmp2F6)!= 10)goto _LL276;_tmp30B=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp2F6)->f1;_LL275:{const char*_tmp7EA;const char*_tmp7E9;struct Cyc_PP_Doc*
_tmp7E8[3];s=((_tmp7E8[2]=Cyc_PP_text(((_tmp7EA=");",_tag_dyneither(_tmp7EA,
sizeof(char),3)))),((_tmp7E8[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp30B),((_tmp7E8[
0]=Cyc_PP_text(((_tmp7E9="fallthru(",_tag_dyneither(_tmp7E9,sizeof(char),10)))),
Cyc_PP_cat(_tag_dyneither(_tmp7E8,sizeof(struct Cyc_PP_Doc*),3))))))));}goto
_LL25B;_LL276: if(_tmp2F6 <= (void*)1)goto _LL278;if(*((int*)_tmp2F6)!= 11)goto
_LL278;_tmp30C=((struct Cyc_Absyn_Decl_s_struct*)_tmp2F6)->f1;_tmp30D=((struct Cyc_Absyn_Decl_s_struct*)
_tmp2F6)->f2;_LL277:{struct Cyc_PP_Doc*_tmp7EB[3];s=((_tmp7EB[2]=Cyc_Absynpp_stmt2doc(
_tmp30D),((_tmp7EB[1]=Cyc_PP_line_doc(),((_tmp7EB[0]=Cyc_Absynpp_decl2doc(
_tmp30C),Cyc_PP_cat(_tag_dyneither(_tmp7EB,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL25B;_LL278: if(_tmp2F6 <= (void*)1)goto _LL27A;if(*((int*)_tmp2F6)!= 12)goto
_LL27A;_tmp30E=((struct Cyc_Absyn_Label_s_struct*)_tmp2F6)->f1;_tmp30F=((struct
Cyc_Absyn_Label_s_struct*)_tmp2F6)->f2;_LL279: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(
_tmp30F)){const char*_tmp7EE;struct Cyc_PP_Doc*_tmp7ED[7];s=((_tmp7ED[6]=Cyc_Absynpp_rb(),((
_tmp7ED[5]=Cyc_PP_line_doc(),((_tmp7ED[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp30F)),((_tmp7ED[3]=Cyc_PP_line_doc(),((_tmp7ED[2]=Cyc_Absynpp_lb(),((_tmp7ED[
1]=Cyc_PP_text(((_tmp7EE=": ",_tag_dyneither(_tmp7EE,sizeof(char),3)))),((
_tmp7ED[0]=Cyc_PP_textptr(_tmp30E),Cyc_PP_cat(_tag_dyneither(_tmp7ED,sizeof(
struct Cyc_PP_Doc*),7))))))))))))))));}else{const char*_tmp7F1;struct Cyc_PP_Doc*
_tmp7F0[3];s=((_tmp7F0[2]=Cyc_Absynpp_stmt2doc(_tmp30F),((_tmp7F0[1]=Cyc_PP_text(((
_tmp7F1=": ",_tag_dyneither(_tmp7F1,sizeof(char),3)))),((_tmp7F0[0]=Cyc_PP_textptr(
_tmp30E),Cyc_PP_cat(_tag_dyneither(_tmp7F0,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL25B;_LL27A: if(_tmp2F6 <= (void*)1)goto _LL27C;if(*((int*)_tmp2F6)!= 13)goto
_LL27C;_tmp310=((struct Cyc_Absyn_Do_s_struct*)_tmp2F6)->f1;_tmp311=((struct Cyc_Absyn_Do_s_struct*)
_tmp2F6)->f2;_tmp312=_tmp311.f1;_LL27B:{const char*_tmp7F8;const char*_tmp7F7;
const char*_tmp7F6;struct Cyc_PP_Doc*_tmp7F5[9];s=((_tmp7F5[8]=Cyc_PP_text(((
_tmp7F8=");",_tag_dyneither(_tmp7F8,sizeof(char),3)))),((_tmp7F5[7]=Cyc_Absynpp_exp2doc(
_tmp312),((_tmp7F5[6]=Cyc_PP_text(((_tmp7F7=" while (",_tag_dyneither(_tmp7F7,
sizeof(char),9)))),((_tmp7F5[5]=Cyc_Absynpp_rb(),((_tmp7F5[4]=Cyc_PP_line_doc(),((
_tmp7F5[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp310)),((_tmp7F5[2]=Cyc_PP_line_doc(),((
_tmp7F5[1]=Cyc_Absynpp_lb(),((_tmp7F5[0]=Cyc_PP_text(((_tmp7F6="do ",
_tag_dyneither(_tmp7F6,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp7F5,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL25B;_LL27C: if(_tmp2F6 <= (
void*)1)goto _LL27E;if(*((int*)_tmp2F6)!= 14)goto _LL27E;_tmp313=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp2F6)->f1;_tmp314=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp2F6)->f2;_LL27D:{
const char*_tmp7FD;const char*_tmp7FC;struct Cyc_PP_Doc*_tmp7FB[12];s=((_tmp7FB[11]=
Cyc_Absynpp_rb(),((_tmp7FB[10]=Cyc_PP_line_doc(),((_tmp7FB[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp314)),((_tmp7FB[8]=Cyc_PP_line_doc(),((_tmp7FB[7]=Cyc_Absynpp_lb(),((_tmp7FB[
6]=Cyc_PP_text(((_tmp7FD=" catch ",_tag_dyneither(_tmp7FD,sizeof(char),8)))),((
_tmp7FB[5]=Cyc_Absynpp_rb(),((_tmp7FB[4]=Cyc_PP_line_doc(),((_tmp7FB[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp313)),((_tmp7FB[2]=Cyc_PP_line_doc(),((_tmp7FB[1]=Cyc_Absynpp_lb(),((
_tmp7FB[0]=Cyc_PP_text(((_tmp7FC="try ",_tag_dyneither(_tmp7FC,sizeof(char),5)))),
Cyc_PP_cat(_tag_dyneither(_tmp7FB,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
goto _LL25B;_LL27E: if(_tmp2F6 <= (void*)1)goto _LL25B;if(*((int*)_tmp2F6)!= 15)goto
_LL25B;_tmp315=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp2F6)->f1;_LL27F:{
const char*_tmp802;const char*_tmp801;struct Cyc_PP_Doc*_tmp800[3];s=((_tmp800[2]=
Cyc_PP_text(((_tmp802=");",_tag_dyneither(_tmp802,sizeof(char),3)))),((_tmp800[1]=
Cyc_Absynpp_exp2doc(_tmp315),((_tmp800[0]=Cyc_PP_text(((_tmp801="reset_region(",
_tag_dyneither(_tmp801,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp800,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL25B;_LL25B:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p);struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{void*_tmp353=(void*)p->r;void*_tmp354;
int _tmp355;char _tmp356;struct _dyneither_ptr _tmp357;struct Cyc_Absyn_Vardecl*
_tmp358;struct Cyc_Absyn_Pat*_tmp359;struct Cyc_Absyn_Pat _tmp35A;void*_tmp35B;
struct Cyc_Absyn_Vardecl*_tmp35C;struct Cyc_Absyn_Pat*_tmp35D;struct Cyc_Absyn_Tvar*
_tmp35E;struct Cyc_Absyn_Vardecl*_tmp35F;struct Cyc_List_List*_tmp360;int _tmp361;
struct Cyc_Absyn_Pat*_tmp362;struct Cyc_Absyn_Vardecl*_tmp363;struct Cyc_Absyn_Pat*
_tmp364;struct Cyc_Absyn_Pat _tmp365;void*_tmp366;struct Cyc_Absyn_Vardecl*_tmp367;
struct Cyc_Absyn_Pat*_tmp368;struct _tuple0*_tmp369;struct _tuple0*_tmp36A;struct
Cyc_List_List*_tmp36B;int _tmp36C;struct Cyc_Absyn_AggrInfo _tmp36D;union Cyc_Absyn_AggrInfoU_union
_tmp36E;struct Cyc_List_List*_tmp36F;struct Cyc_List_List*_tmp370;int _tmp371;
struct Cyc_Absyn_Enumfield*_tmp372;struct Cyc_Absyn_Enumfield*_tmp373;struct Cyc_Absyn_Datatypefield*
_tmp374;struct Cyc_List_List*_tmp375;struct Cyc_Absyn_Datatypefield*_tmp376;struct
Cyc_List_List*_tmp377;int _tmp378;struct Cyc_Absyn_Exp*_tmp379;_LL286: if((int)
_tmp353 != 0)goto _LL288;_LL287:{const char*_tmp803;s=Cyc_PP_text(((_tmp803="_",
_tag_dyneither(_tmp803,sizeof(char),2))));}goto _LL285;_LL288: if((int)_tmp353 != 1)
goto _LL28A;_LL289:{const char*_tmp804;s=Cyc_PP_text(((_tmp804="NULL",
_tag_dyneither(_tmp804,sizeof(char),5))));}goto _LL285;_LL28A: if(_tmp353 <= (void*)
2)goto _LL28C;if(*((int*)_tmp353)!= 7)goto _LL28C;_tmp354=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp353)->f1;_tmp355=((struct Cyc_Absyn_Int_p_struct*)_tmp353)->f2;_LL28B: if(
_tmp354 != (void*)((void*)1)){const char*_tmp808;void*_tmp807[1];struct Cyc_Int_pa_struct
_tmp806;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp806.tag=1,((_tmp806.f1=(
unsigned long)_tmp355,((_tmp807[0]=& _tmp806,Cyc_aprintf(((_tmp808="%d",
_tag_dyneither(_tmp808,sizeof(char),3))),_tag_dyneither(_tmp807,sizeof(void*),1)))))))));}
else{const char*_tmp80C;void*_tmp80B[1];struct Cyc_Int_pa_struct _tmp80A;s=Cyc_PP_text((
struct _dyneither_ptr)((_tmp80A.tag=1,((_tmp80A.f1=(unsigned int)_tmp355,((
_tmp80B[0]=& _tmp80A,Cyc_aprintf(((_tmp80C="%u",_tag_dyneither(_tmp80C,sizeof(
char),3))),_tag_dyneither(_tmp80B,sizeof(void*),1)))))))));}goto _LL285;_LL28C:
if(_tmp353 <= (void*)2)goto _LL28E;if(*((int*)_tmp353)!= 8)goto _LL28E;_tmp356=((
struct Cyc_Absyn_Char_p_struct*)_tmp353)->f1;_LL28D:{const char*_tmp810;void*
_tmp80F[1];struct Cyc_String_pa_struct _tmp80E;s=Cyc_PP_text((struct _dyneither_ptr)((
_tmp80E.tag=0,((_tmp80E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(
_tmp356)),((_tmp80F[0]=& _tmp80E,Cyc_aprintf(((_tmp810="'%s'",_tag_dyneither(
_tmp810,sizeof(char),5))),_tag_dyneither(_tmp80F,sizeof(void*),1)))))))));}goto
_LL285;_LL28E: if(_tmp353 <= (void*)2)goto _LL290;if(*((int*)_tmp353)!= 9)goto
_LL290;_tmp357=((struct Cyc_Absyn_Float_p_struct*)_tmp353)->f1;_LL28F: s=Cyc_PP_text(
_tmp357);goto _LL285;_LL290: if(_tmp353 <= (void*)2)goto _LL292;if(*((int*)_tmp353)
!= 0)goto _LL292;_tmp358=((struct Cyc_Absyn_Var_p_struct*)_tmp353)->f1;_tmp359=((
struct Cyc_Absyn_Var_p_struct*)_tmp353)->f2;_tmp35A=*_tmp359;_tmp35B=(void*)
_tmp35A.r;if((int)_tmp35B != 0)goto _LL292;_LL291: s=Cyc_Absynpp_qvar2doc(_tmp358->name);
goto _LL285;_LL292: if(_tmp353 <= (void*)2)goto _LL294;if(*((int*)_tmp353)!= 0)goto
_LL294;_tmp35C=((struct Cyc_Absyn_Var_p_struct*)_tmp353)->f1;_tmp35D=((struct Cyc_Absyn_Var_p_struct*)
_tmp353)->f2;_LL293:{const char*_tmp813;struct Cyc_PP_Doc*_tmp812[3];s=((_tmp812[2]=
Cyc_Absynpp_pat2doc(_tmp35D),((_tmp812[1]=Cyc_PP_text(((_tmp813=" as ",
_tag_dyneither(_tmp813,sizeof(char),5)))),((_tmp812[0]=Cyc_Absynpp_qvar2doc(
_tmp35C->name),Cyc_PP_cat(_tag_dyneither(_tmp812,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL285;_LL294: if(_tmp353 <= (void*)2)goto _LL296;if(*((int*)_tmp353)!= 2)goto
_LL296;_tmp35E=((struct Cyc_Absyn_TagInt_p_struct*)_tmp353)->f1;_tmp35F=((struct
Cyc_Absyn_TagInt_p_struct*)_tmp353)->f2;_LL295:{const char*_tmp818;const char*
_tmp817;struct Cyc_PP_Doc*_tmp816[4];s=((_tmp816[3]=Cyc_PP_text(((_tmp818=">",
_tag_dyneither(_tmp818,sizeof(char),2)))),((_tmp816[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp35E)),((_tmp816[1]=Cyc_PP_text(((_tmp817="<",_tag_dyneither(_tmp817,sizeof(
char),2)))),((_tmp816[0]=Cyc_Absynpp_qvar2doc(_tmp35F->name),Cyc_PP_cat(
_tag_dyneither(_tmp816,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL285;_LL296:
if(_tmp353 <= (void*)2)goto _LL298;if(*((int*)_tmp353)!= 3)goto _LL298;_tmp360=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp353)->f1;_tmp361=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp353)->f2;_LL297:{const char*_tmp821;const char*_tmp820;const char*_tmp81F;const
char*_tmp81E;struct Cyc_PP_Doc*_tmp81D[4];s=((_tmp81D[3]=_tmp361?Cyc_PP_text(((
_tmp820=", ...)",_tag_dyneither(_tmp820,sizeof(char),7)))): Cyc_PP_text(((_tmp821=")",
_tag_dyneither(_tmp821,sizeof(char),2)))),((_tmp81D[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp81F=",",_tag_dyneither(_tmp81F,
sizeof(char),2))),_tmp360),((_tmp81D[1]=Cyc_PP_text(((_tmp81E="(",_tag_dyneither(
_tmp81E,sizeof(char),2)))),((_tmp81D[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(
_tag_dyneither(_tmp81D,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL285;_LL298:
if(_tmp353 <= (void*)2)goto _LL29A;if(*((int*)_tmp353)!= 4)goto _LL29A;_tmp362=((
struct Cyc_Absyn_Pointer_p_struct*)_tmp353)->f1;_LL299:{const char*_tmp824;struct
Cyc_PP_Doc*_tmp823[2];s=((_tmp823[1]=Cyc_Absynpp_pat2doc(_tmp362),((_tmp823[0]=
Cyc_PP_text(((_tmp824="&",_tag_dyneither(_tmp824,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp823,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL285;_LL29A: if(
_tmp353 <= (void*)2)goto _LL29C;if(*((int*)_tmp353)!= 1)goto _LL29C;_tmp363=((
struct Cyc_Absyn_Reference_p_struct*)_tmp353)->f1;_tmp364=((struct Cyc_Absyn_Reference_p_struct*)
_tmp353)->f2;_tmp365=*_tmp364;_tmp366=(void*)_tmp365.r;if((int)_tmp366 != 0)goto
_LL29C;_LL29B:{const char*_tmp827;struct Cyc_PP_Doc*_tmp826[2];s=((_tmp826[1]=Cyc_Absynpp_qvar2doc(
_tmp363->name),((_tmp826[0]=Cyc_PP_text(((_tmp827="*",_tag_dyneither(_tmp827,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp826,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL285;_LL29C: if(_tmp353 <= (void*)2)goto _LL29E;if(*((int*)_tmp353)!= 1)goto
_LL29E;_tmp367=((struct Cyc_Absyn_Reference_p_struct*)_tmp353)->f1;_tmp368=((
struct Cyc_Absyn_Reference_p_struct*)_tmp353)->f2;_LL29D:{const char*_tmp82C;const
char*_tmp82B;struct Cyc_PP_Doc*_tmp82A[4];s=((_tmp82A[3]=Cyc_Absynpp_pat2doc(
_tmp368),((_tmp82A[2]=Cyc_PP_text(((_tmp82C=" as ",_tag_dyneither(_tmp82C,
sizeof(char),5)))),((_tmp82A[1]=Cyc_Absynpp_qvar2doc(_tmp367->name),((_tmp82A[0]=
Cyc_PP_text(((_tmp82B="*",_tag_dyneither(_tmp82B,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp82A,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL285;_LL29E:
if(_tmp353 <= (void*)2)goto _LL2A0;if(*((int*)_tmp353)!= 12)goto _LL2A0;_tmp369=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp353)->f1;_LL29F: s=Cyc_Absynpp_qvar2doc(
_tmp369);goto _LL285;_LL2A0: if(_tmp353 <= (void*)2)goto _LL2A2;if(*((int*)_tmp353)
!= 13)goto _LL2A2;_tmp36A=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp353)->f1;
_tmp36B=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp353)->f2;_tmp36C=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp353)->f3;_LL2A1: {const char*_tmp82E;const char*_tmp82D;struct _dyneither_ptr
term=_tmp36C?(_tmp82E=", ...)",_tag_dyneither(_tmp82E,sizeof(char),7)):((_tmp82D=")",
_tag_dyneither(_tmp82D,sizeof(char),2)));{const char*_tmp833;const char*_tmp832;
struct Cyc_PP_Doc*_tmp831[2];s=((_tmp831[1]=Cyc_PP_group(((_tmp833="(",
_tag_dyneither(_tmp833,sizeof(char),2))),term,((_tmp832=",",_tag_dyneither(
_tmp832,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp36B)),((
_tmp831[0]=Cyc_Absynpp_qvar2doc(_tmp36A),Cyc_PP_cat(_tag_dyneither(_tmp831,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL285;}_LL2A2: if(_tmp353 <= (void*)2)goto
_LL2A4;if(*((int*)_tmp353)!= 5)goto _LL2A4;_tmp36D=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp353)->f1;_tmp36E=_tmp36D.aggr_info;_tmp36F=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp353)->f2;_tmp370=((struct Cyc_Absyn_Aggr_p_struct*)_tmp353)->f3;_tmp371=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp353)->f4;_LL2A3: {const char*_tmp835;const char*
_tmp834;struct _dyneither_ptr term=_tmp371?(_tmp835=", ...}",_tag_dyneither(
_tmp835,sizeof(char),7)):((_tmp834="}",_tag_dyneither(_tmp834,sizeof(char),2)));
struct _tuple0*_tmp39C;struct _tuple3 _tmp39B=Cyc_Absyn_aggr_kinded_name(_tmp36E);
_tmp39C=_tmp39B.f2;{const char*_tmp840;const char*_tmp83F;const char*_tmp83E;const
char*_tmp83D;const char*_tmp83C;struct Cyc_PP_Doc*_tmp83B[4];s=((_tmp83B[3]=Cyc_PP_group(((
_tmp840="",_tag_dyneither(_tmp840,sizeof(char),1))),term,((_tmp83F=",",
_tag_dyneither(_tmp83F,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp370)),((_tmp83B[2]=Cyc_PP_egroup(((_tmp83E="[",_tag_dyneither(_tmp83E,
sizeof(char),2))),((_tmp83D="]",_tag_dyneither(_tmp83D,sizeof(char),2))),((
_tmp83C=",",_tag_dyneither(_tmp83C,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp36F))),((_tmp83B[1]=
Cyc_Absynpp_lb(),((_tmp83B[0]=Cyc_Absynpp_qvar2doc(_tmp39C),Cyc_PP_cat(
_tag_dyneither(_tmp83B,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL285;}_LL2A4:
if(_tmp353 <= (void*)2)goto _LL2A6;if(*((int*)_tmp353)!= 10)goto _LL2A6;_tmp372=((
struct Cyc_Absyn_Enum_p_struct*)_tmp353)->f2;_LL2A5: _tmp373=_tmp372;goto _LL2A7;
_LL2A6: if(_tmp353 <= (void*)2)goto _LL2A8;if(*((int*)_tmp353)!= 11)goto _LL2A8;
_tmp373=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp353)->f2;_LL2A7: s=Cyc_Absynpp_qvar2doc(
_tmp373->name);goto _LL285;_LL2A8: if(_tmp353 <= (void*)2)goto _LL2AA;if(*((int*)
_tmp353)!= 6)goto _LL2AA;_tmp374=((struct Cyc_Absyn_Datatype_p_struct*)_tmp353)->f2;
_tmp375=((struct Cyc_Absyn_Datatype_p_struct*)_tmp353)->f3;if(_tmp375 != 0)goto
_LL2AA;_LL2A9: s=Cyc_Absynpp_qvar2doc(_tmp374->name);goto _LL285;_LL2AA: if(_tmp353
<= (void*)2)goto _LL2AC;if(*((int*)_tmp353)!= 6)goto _LL2AC;_tmp376=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp353)->f2;_tmp377=((struct Cyc_Absyn_Datatype_p_struct*)_tmp353)->f3;_tmp378=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp353)->f4;_LL2AB: {const char*_tmp842;const
char*_tmp841;struct _dyneither_ptr term=_tmp378?(_tmp842=", ...)",_tag_dyneither(
_tmp842,sizeof(char),7)):((_tmp841=")",_tag_dyneither(_tmp841,sizeof(char),2)));{
const char*_tmp847;const char*_tmp846;struct Cyc_PP_Doc*_tmp845[2];s=((_tmp845[1]=
Cyc_PP_egroup(((_tmp847="(",_tag_dyneither(_tmp847,sizeof(char),2))),term,((
_tmp846=",",_tag_dyneither(_tmp846,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp377)),((_tmp845[0]=Cyc_Absynpp_qvar2doc(_tmp376->name),
Cyc_PP_cat(_tag_dyneither(_tmp845,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL285;}
_LL2AC: if(_tmp353 <= (void*)2)goto _LL285;if(*((int*)_tmp353)!= 14)goto _LL285;
_tmp379=((struct Cyc_Absyn_Exp_p_struct*)_tmp353)->f1;_LL2AD: s=Cyc_Absynpp_exp2doc(
_tmp379);goto _LL285;_LL285:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple8*dp);struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*dp){const char*
_tmp84E;const char*_tmp84D;const char*_tmp84C;struct Cyc_PP_Doc*_tmp84B[2];return(
_tmp84B[1]=Cyc_Absynpp_pat2doc((*dp).f2),((_tmp84B[0]=Cyc_PP_egroup(((_tmp84E="",
_tag_dyneither(_tmp84E,sizeof(char),1))),((_tmp84D="=",_tag_dyneither(_tmp84D,
sizeof(char),2))),((_tmp84C="=",_tag_dyneither(_tmp84C,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp84B,sizeof(
struct Cyc_PP_Doc*),2)))));}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c);struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (void*)(c->pattern)->r == (
void*)((void*)0)){struct Cyc_PP_Doc*_tmp853[2];const char*_tmp852;struct Cyc_PP_Doc*
_tmp851[2];return(_tmp851[1]=Cyc_PP_nest(2,((_tmp853[1]=Cyc_Absynpp_stmt2doc(c->body),((
_tmp853[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp853,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp851[0]=Cyc_PP_text(((_tmp852="default: ",_tag_dyneither(_tmp852,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp851,sizeof(struct Cyc_PP_Doc*),
2)))));}else{if(c->where_clause == 0){struct Cyc_PP_Doc*_tmp85A[2];const char*
_tmp859;const char*_tmp858;struct Cyc_PP_Doc*_tmp857[4];return(_tmp857[3]=Cyc_PP_nest(
2,((_tmp85A[1]=Cyc_Absynpp_stmt2doc(c->body),((_tmp85A[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp85A,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp857[2]=Cyc_PP_text(((
_tmp859=": ",_tag_dyneither(_tmp859,sizeof(char),3)))),((_tmp857[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp857[0]=Cyc_PP_text(((_tmp858="case ",_tag_dyneither(_tmp858,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp857,sizeof(struct Cyc_PP_Doc*),4)))))))));}
else{struct Cyc_PP_Doc*_tmp863[2];const char*_tmp862;const char*_tmp861;const char*
_tmp860;struct Cyc_PP_Doc*_tmp85F[6];return(_tmp85F[5]=Cyc_PP_nest(2,((_tmp863[1]=
Cyc_Absynpp_stmt2doc(c->body),((_tmp863[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp863,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp85F[4]=Cyc_PP_text(((
_tmp862=": ",_tag_dyneither(_tmp862,sizeof(char),3)))),((_tmp85F[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp85F[2]=Cyc_PP_text(((
_tmp861=" && ",_tag_dyneither(_tmp861,sizeof(char),5)))),((_tmp85F[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp85F[0]=Cyc_PP_text(((_tmp860="case ",_tag_dyneither(_tmp860,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp85F,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){const char*_tmp864;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((
_tmp864="",_tag_dyneither(_tmp864,sizeof(char),1))),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct
Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);else{
const char*_tmp867;struct Cyc_PP_Doc*_tmp866[3];return(_tmp866[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp866[1]=Cyc_PP_text(((_tmp867=" = ",
_tag_dyneither(_tmp867,sizeof(char),4)))),((_tmp866[0]=Cyc_Absynpp_qvar2doc(f->name),
Cyc_PP_cat(_tag_dyneither(_tmp866,sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){const char*_tmp868;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp868=",",_tag_dyneither(_tmp868,
sizeof(char),2))),fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*
v);static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return
Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct
Cyc_List_List*vds);static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*
vds){const char*_tmp869;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_id2doc,((_tmp869=",",_tag_dyneither(_tmp869,sizeof(char),2))),vds);}
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);struct Cyc_PP_Doc*
Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl
_tmp3C0;void*_tmp3C1;struct _tuple0*_tmp3C2;struct Cyc_Absyn_Tqual _tmp3C3;void*
_tmp3C4;struct Cyc_Absyn_Exp*_tmp3C5;struct Cyc_List_List*_tmp3C6;struct Cyc_Absyn_Vardecl*
_tmp3BF=vd;_tmp3C0=*_tmp3BF;_tmp3C1=(void*)_tmp3C0.sc;_tmp3C2=_tmp3C0.name;
_tmp3C3=_tmp3C0.tq;_tmp3C4=(void*)_tmp3C0.type;_tmp3C5=_tmp3C0.initializer;
_tmp3C6=_tmp3C0.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(
_tmp3C2);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp3C6);struct Cyc_PP_Doc*
beforenamedoc;{void*_tmp3C7=Cyc_Cyclone_c_compiler;_LL2AF: if((int)_tmp3C7 != 0)
goto _LL2B1;_LL2B0: beforenamedoc=attsdoc;goto _LL2AE;_LL2B1: if((int)_tmp3C7 != 1)
goto _LL2AE;_LL2B2:{void*_tmp3C8=Cyc_Tcutil_compress(_tmp3C4);struct Cyc_Absyn_FnInfo
_tmp3C9;struct Cyc_List_List*_tmp3CA;_LL2B4: if(_tmp3C8 <= (void*)4)goto _LL2B6;if(*((
int*)_tmp3C8)!= 8)goto _LL2B6;_tmp3C9=((struct Cyc_Absyn_FnType_struct*)_tmp3C8)->f1;
_tmp3CA=_tmp3C9.attributes;_LL2B5: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp3CA);
goto _LL2B3;_LL2B6:;_LL2B7: beforenamedoc=Cyc_PP_nil_doc();goto _LL2B3;_LL2B3:;}
goto _LL2AE;_LL2AE:;}{struct Cyc_PP_Doc*tmp_doc;{void*_tmp3CB=Cyc_Cyclone_c_compiler;
_LL2B9: if((int)_tmp3CB != 0)goto _LL2BB;_LL2BA: tmp_doc=Cyc_PP_nil_doc();goto _LL2B8;
_LL2BB: if((int)_tmp3CB != 1)goto _LL2B8;_LL2BC: tmp_doc=attsdoc;goto _LL2B8;_LL2B8:;}{
const char*_tmp876;struct Cyc_PP_Doc*_tmp875[2];const char*_tmp874;struct Cyc_Core_Opt*
_tmp873;struct Cyc_PP_Doc*_tmp872[2];struct Cyc_PP_Doc*_tmp871[5];s=((_tmp871[4]=
Cyc_PP_text(((_tmp876=";",_tag_dyneither(_tmp876,sizeof(char),2)))),((_tmp871[3]=
_tmp3C5 == 0?Cyc_PP_nil_doc():((_tmp875[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp3C5),((_tmp875[0]=Cyc_PP_text(((_tmp874=" = ",_tag_dyneither(_tmp874,sizeof(
char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp875,sizeof(struct Cyc_PP_Doc*),2)))))),((
_tmp871[2]=Cyc_Absynpp_tqtd2doc(_tmp3C3,_tmp3C4,((_tmp873=_cycalloc(sizeof(*
_tmp873)),((_tmp873->v=((_tmp872[1]=sn,((_tmp872[0]=beforenamedoc,Cyc_PP_cat(
_tag_dyneither(_tmp872,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp873))))),((_tmp871[
1]=Cyc_Absynpp_scope2doc(_tmp3C1),((_tmp871[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(
_tmp871,sizeof(struct Cyc_PP_Doc*),5))))))))))));}return s;}}}struct _tuple11{
struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(
struct _tuple11*x);struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple11*x){
struct _tuple0*_tmp3D3;struct _tuple11 _tmp3D2=*x;_tmp3D3=_tmp3D2.f2;return Cyc_Absynpp_qvar2doc(
_tmp3D3);}struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);struct
Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*
_tmp3D4=(void*)d->r;struct Cyc_Absyn_Fndecl*_tmp3D5;struct Cyc_Absyn_Aggrdecl*
_tmp3D6;struct Cyc_Absyn_Vardecl*_tmp3D7;struct Cyc_Absyn_Tvar*_tmp3D8;struct Cyc_Absyn_Vardecl*
_tmp3D9;int _tmp3DA;struct Cyc_Absyn_Exp*_tmp3DB;struct Cyc_Absyn_Exp*_tmp3DC;
struct Cyc_Absyn_Tvar*_tmp3DD;struct Cyc_Absyn_Vardecl*_tmp3DE;struct Cyc_Absyn_Datatypedecl*
_tmp3DF;struct Cyc_Absyn_Datatypedecl _tmp3E0;void*_tmp3E1;struct _tuple0*_tmp3E2;
struct Cyc_List_List*_tmp3E3;struct Cyc_Core_Opt*_tmp3E4;int _tmp3E5;int _tmp3E6;
struct Cyc_Absyn_Pat*_tmp3E7;struct Cyc_Absyn_Exp*_tmp3E8;struct Cyc_List_List*
_tmp3E9;struct Cyc_Absyn_Enumdecl*_tmp3EA;struct Cyc_Absyn_Enumdecl _tmp3EB;void*
_tmp3EC;struct _tuple0*_tmp3ED;struct Cyc_Core_Opt*_tmp3EE;struct Cyc_Absyn_Typedefdecl*
_tmp3EF;struct _dyneither_ptr*_tmp3F0;struct Cyc_List_List*_tmp3F1;struct _tuple0*
_tmp3F2;struct Cyc_List_List*_tmp3F3;struct Cyc_List_List*_tmp3F4;struct Cyc_List_List*
_tmp3F5;struct Cyc_List_List*_tmp3F6;_LL2BE: if(_tmp3D4 <= (void*)2)goto _LL2DA;if(*((
int*)_tmp3D4)!= 1)goto _LL2C0;_tmp3D5=((struct Cyc_Absyn_Fn_d_struct*)_tmp3D4)->f1;
_LL2BF: {struct Cyc_Absyn_FnType_struct _tmp87C;struct Cyc_Absyn_FnInfo _tmp87B;
struct Cyc_Absyn_FnType_struct*_tmp87A;void*t=(void*)((_tmp87A=_cycalloc(sizeof(*
_tmp87A)),((_tmp87A[0]=((_tmp87C.tag=8,((_tmp87C.f1=((_tmp87B.tvars=_tmp3D5->tvs,((
_tmp87B.effect=_tmp3D5->effect,((_tmp87B.ret_typ=(void*)((void*)_tmp3D5->ret_type),((
_tmp87B.args=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple6*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp3D5->args),((_tmp87B.c_varargs=
_tmp3D5->c_varargs,((_tmp87B.cyc_varargs=_tmp3D5->cyc_varargs,((_tmp87B.rgn_po=
_tmp3D5->rgn_po,((_tmp87B.attributes=0,_tmp87B)))))))))))))))),_tmp87C)))),
_tmp87A))));struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp3D5->attributes);
struct Cyc_PP_Doc*inlinedoc;if(_tmp3D5->is_inline){void*_tmp3F7=Cyc_Cyclone_c_compiler;
_LL2DF: if((int)_tmp3F7 != 0)goto _LL2E1;_LL2E0:{const char*_tmp87D;inlinedoc=Cyc_PP_text(((
_tmp87D="inline ",_tag_dyneither(_tmp87D,sizeof(char),8))));}goto _LL2DE;_LL2E1:
if((int)_tmp3F7 != 1)goto _LL2DE;_LL2E2:{const char*_tmp87E;inlinedoc=Cyc_PP_text(((
_tmp87E="__inline ",_tag_dyneither(_tmp87E,sizeof(char),10))));}goto _LL2DE;
_LL2DE:;}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc((
void*)_tmp3D5->sc);struct Cyc_PP_Doc*beforenamedoc;{void*_tmp3FA=Cyc_Cyclone_c_compiler;
_LL2E4: if((int)_tmp3FA != 0)goto _LL2E6;_LL2E5: beforenamedoc=attsdoc;goto _LL2E3;
_LL2E6: if((int)_tmp3FA != 1)goto _LL2E3;_LL2E7: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp3D5->attributes);goto _LL2E3;_LL2E3:;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp3D5->name);struct Cyc_PP_Doc*_tmp881[2];struct Cyc_Core_Opt*_tmp880;struct Cyc_PP_Doc*
tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,((_tmp880=_cycalloc(
sizeof(*_tmp880)),((_tmp880->v=((_tmp881[1]=namedoc,((_tmp881[0]=beforenamedoc,
Cyc_PP_cat(_tag_dyneither(_tmp881,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp880)))));
struct Cyc_PP_Doc*_tmp884[2];struct Cyc_PP_Doc*_tmp883[5];struct Cyc_PP_Doc*bodydoc=(
_tmp883[4]=Cyc_Absynpp_rb(),((_tmp883[3]=Cyc_PP_line_doc(),((_tmp883[2]=Cyc_PP_nest(
2,((_tmp884[1]=Cyc_Absynpp_stmt2doc(_tmp3D5->body),((_tmp884[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dyneither(_tmp884,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp883[1]=
Cyc_Absynpp_lb(),((_tmp883[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(
_tmp883,sizeof(struct Cyc_PP_Doc*),5)))))))))));{struct Cyc_PP_Doc*_tmp885[4];s=((
_tmp885[3]=bodydoc,((_tmp885[2]=tqtddoc,((_tmp885[1]=scopedoc,((_tmp885[0]=
inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp885,sizeof(struct Cyc_PP_Doc*),4))))))))));}{
void*_tmp3FC=Cyc_Cyclone_c_compiler;_LL2E9: if((int)_tmp3FC != 1)goto _LL2EB;_LL2EA:{
struct Cyc_PP_Doc*_tmp886[2];s=((_tmp886[1]=s,((_tmp886[0]=attsdoc,Cyc_PP_cat(
_tag_dyneither(_tmp886,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL2E8;_LL2EB:;
_LL2EC: goto _LL2E8;_LL2E8:;}goto _LL2BD;}}}_LL2C0: if(*((int*)_tmp3D4)!= 6)goto
_LL2C2;_tmp3D6=((struct Cyc_Absyn_Aggr_d_struct*)_tmp3D4)->f1;_LL2C1: if(_tmp3D6->impl
== 0){const char*_tmp889;struct Cyc_PP_Doc*_tmp888[5];s=((_tmp888[4]=Cyc_PP_text(((
_tmp889=";",_tag_dyneither(_tmp889,sizeof(char),2)))),((_tmp888[3]=Cyc_Absynpp_ktvars2doc(
_tmp3D6->tvs),((_tmp888[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3D6->name),((_tmp888[1]=
Cyc_Absynpp_aggr_kind2doc((void*)_tmp3D6->kind),((_tmp888[0]=Cyc_Absynpp_scope2doc((
void*)_tmp3D6->sc),Cyc_PP_cat(_tag_dyneither(_tmp888,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
else{const char*_tmp893;struct Cyc_PP_Doc*_tmp892[2];struct Cyc_PP_Doc*_tmp891[2];
const char*_tmp890;struct Cyc_PP_Doc*_tmp88F[12];s=((_tmp88F[11]=Cyc_PP_text(((
_tmp893=";",_tag_dyneither(_tmp893,sizeof(char),2)))),((_tmp88F[10]=Cyc_Absynpp_atts2doc(
_tmp3D6->attributes),((_tmp88F[9]=Cyc_Absynpp_rb(),((_tmp88F[8]=Cyc_PP_line_doc(),((
_tmp88F[7]=Cyc_PP_nest(2,((_tmp892[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3D6->impl))->fields),((_tmp892[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp892,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp88F[6]=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3D6->impl))->rgn_po == 0?Cyc_PP_nil_doc():((
_tmp891[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp3D6->impl))->rgn_po),((_tmp891[0]=Cyc_PP_text(((_tmp890=":",_tag_dyneither(
_tmp890,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp891,sizeof(struct Cyc_PP_Doc*),
2)))))),((_tmp88F[5]=Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3D6->impl))->exist_vars),((_tmp88F[4]=Cyc_Absynpp_lb(),((_tmp88F[
3]=Cyc_PP_blank_doc(),((_tmp88F[2]=Cyc_Absynpp_qvar2bolddoc(_tmp3D6->name),((
_tmp88F[1]=Cyc_Absynpp_aggr_kind2doc((void*)_tmp3D6->kind),((_tmp88F[0]=Cyc_Absynpp_scope2doc((
void*)_tmp3D6->sc),Cyc_PP_cat(_tag_dyneither(_tmp88F,sizeof(struct Cyc_PP_Doc*),
12))))))))))))))))))))))))));}goto _LL2BD;_LL2C2: if(*((int*)_tmp3D4)!= 0)goto
_LL2C4;_tmp3D7=((struct Cyc_Absyn_Var_d_struct*)_tmp3D4)->f1;_LL2C3: s=Cyc_Absynpp_vardecl2doc(
_tmp3D7);goto _LL2BD;_LL2C4: if(*((int*)_tmp3D4)!= 4)goto _LL2C6;_tmp3D8=((struct
Cyc_Absyn_Region_d_struct*)_tmp3D4)->f1;_tmp3D9=((struct Cyc_Absyn_Region_d_struct*)
_tmp3D4)->f2;_tmp3DA=((struct Cyc_Absyn_Region_d_struct*)_tmp3D4)->f3;_tmp3DB=((
struct Cyc_Absyn_Region_d_struct*)_tmp3D4)->f4;_LL2C5:{const char*_tmp8A6;struct
Cyc_PP_Doc*_tmp8A5[3];const char*_tmp8A4;const char*_tmp8A3;const char*_tmp8A2;
const char*_tmp8A1;const char*_tmp8A0;const char*_tmp89F;struct Cyc_PP_Doc*_tmp89E[8];
s=((_tmp89E[7]=Cyc_PP_text(((_tmp8A6=";",_tag_dyneither(_tmp8A6,sizeof(char),2)))),((
_tmp89E[6]=_tmp3DB != 0?(_tmp8A5[2]=Cyc_PP_text(((_tmp8A3=")",_tag_dyneither(
_tmp8A3,sizeof(char),2)))),((_tmp8A5[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp3DB),((_tmp8A5[0]=Cyc_PP_text(((_tmp8A4=" = open(",_tag_dyneither(_tmp8A4,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8A5,sizeof(struct Cyc_PP_Doc*),3))))))):
Cyc_PP_nil_doc(),((_tmp89E[5]=_tmp3DA?Cyc_PP_text(((_tmp8A2=" @resetable",
_tag_dyneither(_tmp8A2,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp89E[4]=Cyc_Absynpp_qvar2doc(
_tmp3D9->name),((_tmp89E[3]=Cyc_PP_text(((_tmp8A1=">",_tag_dyneither(_tmp8A1,
sizeof(char),2)))),((_tmp89E[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp3D8)),((
_tmp89E[1]=Cyc_PP_text(((_tmp8A0="<",_tag_dyneither(_tmp8A0,sizeof(char),2)))),((
_tmp89E[0]=Cyc_PP_text(((_tmp89F="region",_tag_dyneither(_tmp89F,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmp89E,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL2BD;_LL2C6: if(*((int*)_tmp3D4)!= 5)goto _LL2C8;_tmp3DC=((struct Cyc_Absyn_Alias_d_struct*)
_tmp3D4)->f1;_tmp3DD=((struct Cyc_Absyn_Alias_d_struct*)_tmp3D4)->f2;_tmp3DE=((
struct Cyc_Absyn_Alias_d_struct*)_tmp3D4)->f3;_LL2C7:{const char*_tmp8B1;const char*
_tmp8B0;const char*_tmp8AF;const char*_tmp8AE;const char*_tmp8AD;struct Cyc_PP_Doc*
_tmp8AC[8];s=((_tmp8AC[7]=Cyc_PP_text(((_tmp8B1=";",_tag_dyneither(_tmp8B1,
sizeof(char),2)))),((_tmp8AC[6]=Cyc_Absynpp_exp2doc(_tmp3DC),((_tmp8AC[5]=Cyc_PP_text(((
_tmp8B0=" = ",_tag_dyneither(_tmp8B0,sizeof(char),4)))),((_tmp8AC[4]=Cyc_Absynpp_qvar2doc(
_tmp3DE->name),((_tmp8AC[3]=Cyc_PP_text(((_tmp8AF="> ",_tag_dyneither(_tmp8AF,
sizeof(char),3)))),((_tmp8AC[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp3DD)),((
_tmp8AC[1]=Cyc_PP_text(((_tmp8AE=" <",_tag_dyneither(_tmp8AE,sizeof(char),3)))),((
_tmp8AC[0]=Cyc_PP_text(((_tmp8AD="alias",_tag_dyneither(_tmp8AD,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp8AC,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL2BD;_LL2C8: if(*((int*)_tmp3D4)!= 7)goto _LL2CA;_tmp3DF=((struct Cyc_Absyn_Datatype_d_struct*)
_tmp3D4)->f1;_tmp3E0=*_tmp3DF;_tmp3E1=(void*)_tmp3E0.sc;_tmp3E2=_tmp3E0.name;
_tmp3E3=_tmp3E0.tvs;_tmp3E4=_tmp3E0.fields;_tmp3E5=_tmp3E0.is_extensible;_tmp3E6=
_tmp3E0.is_flat;_LL2C9: if(_tmp3E4 == 0){const char*_tmp8BA;const char*_tmp8B9;const
char*_tmp8B8;const char*_tmp8B7;struct Cyc_PP_Doc*_tmp8B6[7];s=((_tmp8B6[6]=Cyc_PP_text(((
_tmp8BA=";",_tag_dyneither(_tmp8BA,sizeof(char),2)))),((_tmp8B6[5]=Cyc_Absynpp_ktvars2doc(
_tmp3E3),((_tmp8B6[4]=_tmp3E5?Cyc_Absynpp_qvar2bolddoc(_tmp3E2): Cyc_Absynpp_typedef_name2bolddoc(
_tmp3E2),((_tmp8B6[3]=_tmp3E6?Cyc_PP_text(((_tmp8B9="__attribute__((flat)) ",
_tag_dyneither(_tmp8B9,sizeof(char),23)))): Cyc_PP_blank_doc(),((_tmp8B6[2]=Cyc_PP_text(((
_tmp8B8="datatype ",_tag_dyneither(_tmp8B8,sizeof(char),10)))),((_tmp8B6[1]=
_tmp3E5?Cyc_PP_text(((_tmp8B7="@extensible",_tag_dyneither(_tmp8B7,sizeof(char),
12)))): Cyc_PP_blank_doc(),((_tmp8B6[0]=Cyc_Absynpp_scope2doc(_tmp3E1),Cyc_PP_cat(
_tag_dyneither(_tmp8B6,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{const
char*_tmp8C5;struct Cyc_PP_Doc*_tmp8C4[2];const char*_tmp8C3;const char*_tmp8C2;
const char*_tmp8C1;struct Cyc_PP_Doc*_tmp8C0[12];s=((_tmp8C0[11]=Cyc_PP_text(((
_tmp8C5=";",_tag_dyneither(_tmp8C5,sizeof(char),2)))),((_tmp8C0[10]=Cyc_Absynpp_rb(),((
_tmp8C0[9]=Cyc_PP_line_doc(),((_tmp8C0[8]=Cyc_PP_nest(2,((_tmp8C4[1]=Cyc_Absynpp_datatypefields2doc((
struct Cyc_List_List*)_tmp3E4->v),((_tmp8C4[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmp8C4,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8C0[7]=Cyc_Absynpp_lb(),((
_tmp8C0[6]=Cyc_PP_blank_doc(),((_tmp8C0[5]=Cyc_Absynpp_ktvars2doc(_tmp3E3),((
_tmp8C0[4]=_tmp3E5?Cyc_Absynpp_qvar2bolddoc(_tmp3E2): Cyc_Absynpp_typedef_name2bolddoc(
_tmp3E2),((_tmp8C0[3]=Cyc_PP_text(((_tmp8C3="datatype ",_tag_dyneither(_tmp8C3,
sizeof(char),10)))),((_tmp8C0[2]=_tmp3E6?Cyc_PP_text(((_tmp8C2="__attribute__((flat)) ",
_tag_dyneither(_tmp8C2,sizeof(char),23)))): Cyc_PP_blank_doc(),((_tmp8C0[1]=
_tmp3E5?Cyc_PP_text(((_tmp8C1="@extensible ",_tag_dyneither(_tmp8C1,sizeof(char),
13)))): Cyc_PP_blank_doc(),((_tmp8C0[0]=Cyc_Absynpp_scope2doc(_tmp3E1),Cyc_PP_cat(
_tag_dyneither(_tmp8C0,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
goto _LL2BD;_LL2CA: if(*((int*)_tmp3D4)!= 2)goto _LL2CC;_tmp3E7=((struct Cyc_Absyn_Let_d_struct*)
_tmp3D4)->f1;_tmp3E8=((struct Cyc_Absyn_Let_d_struct*)_tmp3D4)->f3;_LL2CB:{const
char*_tmp8CC;const char*_tmp8CB;const char*_tmp8CA;struct Cyc_PP_Doc*_tmp8C9[5];s=((
_tmp8C9[4]=Cyc_PP_text(((_tmp8CC=";",_tag_dyneither(_tmp8CC,sizeof(char),2)))),((
_tmp8C9[3]=Cyc_Absynpp_exp2doc(_tmp3E8),((_tmp8C9[2]=Cyc_PP_text(((_tmp8CB=" = ",
_tag_dyneither(_tmp8CB,sizeof(char),4)))),((_tmp8C9[1]=Cyc_Absynpp_pat2doc(
_tmp3E7),((_tmp8C9[0]=Cyc_PP_text(((_tmp8CA="let ",_tag_dyneither(_tmp8CA,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8C9,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL2BD;_LL2CC: if(*((int*)_tmp3D4)!= 3)goto _LL2CE;_tmp3E9=((struct Cyc_Absyn_Letv_d_struct*)
_tmp3D4)->f1;_LL2CD:{const char*_tmp8D1;const char*_tmp8D0;struct Cyc_PP_Doc*
_tmp8CF[3];s=((_tmp8CF[2]=Cyc_PP_text(((_tmp8D1=";",_tag_dyneither(_tmp8D1,
sizeof(char),2)))),((_tmp8CF[1]=Cyc_Absynpp_ids2doc(_tmp3E9),((_tmp8CF[0]=Cyc_PP_text(((
_tmp8D0="let ",_tag_dyneither(_tmp8D0,sizeof(char),5)))),Cyc_PP_cat(
_tag_dyneither(_tmp8CF,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL2BD;_LL2CE:
if(*((int*)_tmp3D4)!= 8)goto _LL2D0;_tmp3EA=((struct Cyc_Absyn_Enum_d_struct*)
_tmp3D4)->f1;_tmp3EB=*_tmp3EA;_tmp3EC=(void*)_tmp3EB.sc;_tmp3ED=_tmp3EB.name;
_tmp3EE=_tmp3EB.fields;_LL2CF: if(_tmp3EE == 0){const char*_tmp8D6;const char*
_tmp8D5;struct Cyc_PP_Doc*_tmp8D4[4];s=((_tmp8D4[3]=Cyc_PP_text(((_tmp8D6=";",
_tag_dyneither(_tmp8D6,sizeof(char),2)))),((_tmp8D4[2]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp3ED),((_tmp8D4[1]=Cyc_PP_text(((_tmp8D5="enum ",_tag_dyneither(_tmp8D5,
sizeof(char),6)))),((_tmp8D4[0]=Cyc_Absynpp_scope2doc(_tmp3EC),Cyc_PP_cat(
_tag_dyneither(_tmp8D4,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{const char*
_tmp8DD;struct Cyc_PP_Doc*_tmp8DC[2];const char*_tmp8DB;struct Cyc_PP_Doc*_tmp8DA[9];
s=((_tmp8DA[8]=Cyc_PP_text(((_tmp8DD=";",_tag_dyneither(_tmp8DD,sizeof(char),2)))),((
_tmp8DA[7]=Cyc_Absynpp_rb(),((_tmp8DA[6]=Cyc_PP_line_doc(),((_tmp8DA[5]=Cyc_PP_nest(
2,((_tmp8DC[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp3EE->v),((
_tmp8DC[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8DC,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp8DA[4]=Cyc_Absynpp_lb(),((_tmp8DA[3]=Cyc_PP_blank_doc(),((_tmp8DA[
2]=Cyc_Absynpp_qvar2bolddoc(_tmp3ED),((_tmp8DA[1]=Cyc_PP_text(((_tmp8DB="enum ",
_tag_dyneither(_tmp8DB,sizeof(char),6)))),((_tmp8DA[0]=Cyc_Absynpp_scope2doc(
_tmp3EC),Cyc_PP_cat(_tag_dyneither(_tmp8DA,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
goto _LL2BD;_LL2D0: if(*((int*)_tmp3D4)!= 9)goto _LL2D2;_tmp3EF=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp3D4)->f1;_LL2D1: {void*t;if(_tmp3EF->defn != 0)t=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3EF->defn))->v;else{t=Cyc_Absyn_new_evar(_tmp3EF->kind,0);}{
const char*_tmp8E7;struct Cyc_Core_Opt*_tmp8E6;struct Cyc_PP_Doc*_tmp8E5[2];const
char*_tmp8E4;struct Cyc_PP_Doc*_tmp8E3[4];s=((_tmp8E3[3]=Cyc_PP_text(((_tmp8E7=";",
_tag_dyneither(_tmp8E7,sizeof(char),2)))),((_tmp8E3[2]=Cyc_Absynpp_atts2doc(
_tmp3EF->atts),((_tmp8E3[1]=Cyc_Absynpp_tqtd2doc(_tmp3EF->tq,t,((_tmp8E6=
_cycalloc(sizeof(*_tmp8E6)),((_tmp8E6->v=((_tmp8E5[1]=Cyc_Absynpp_tvars2doc(
_tmp3EF->tvs),((_tmp8E5[0]=Cyc_Absynpp_typedef_name2bolddoc(_tmp3EF->name),Cyc_PP_cat(
_tag_dyneither(_tmp8E5,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp8E6))))),((_tmp8E3[
0]=Cyc_PP_text(((_tmp8E4="typedef ",_tag_dyneither(_tmp8E4,sizeof(char),9)))),
Cyc_PP_cat(_tag_dyneither(_tmp8E3,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL2BD;}_LL2D2: if(*((int*)_tmp3D4)!= 10)goto _LL2D4;_tmp3F0=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp3D4)->f1;_tmp3F1=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3D4)->f2;_LL2D3:
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp3F0);{const
char*_tmp8EC;const char*_tmp8EB;struct Cyc_PP_Doc*_tmp8EA[8];s=((_tmp8EA[7]=Cyc_Absynpp_rb(),((
_tmp8EA[6]=Cyc_PP_line_doc(),((_tmp8EA[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp8EC="",_tag_dyneither(_tmp8EC,sizeof(char),1))),
_tmp3F1),((_tmp8EA[4]=Cyc_PP_line_doc(),((_tmp8EA[3]=Cyc_Absynpp_lb(),((_tmp8EA[
2]=Cyc_PP_blank_doc(),((_tmp8EA[1]=Cyc_PP_textptr(_tmp3F0),((_tmp8EA[0]=Cyc_PP_text(((
_tmp8EB="namespace ",_tag_dyneither(_tmp8EB,sizeof(char),11)))),Cyc_PP_cat(
_tag_dyneither(_tmp8EA,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}if(Cyc_Absynpp_use_curr_namespace)
Cyc_Absynpp_curr_namespace_drop();goto _LL2BD;_LL2D4: if(*((int*)_tmp3D4)!= 11)
goto _LL2D6;_tmp3F2=((struct Cyc_Absyn_Using_d_struct*)_tmp3D4)->f1;_tmp3F3=((
struct Cyc_Absyn_Using_d_struct*)_tmp3D4)->f2;_LL2D5: if(Cyc_Absynpp_print_using_stmts){
const char*_tmp8F1;const char*_tmp8F0;struct Cyc_PP_Doc*_tmp8EF[8];s=((_tmp8EF[7]=
Cyc_Absynpp_rb(),((_tmp8EF[6]=Cyc_PP_line_doc(),((_tmp8EF[5]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmp8F1="",_tag_dyneither(_tmp8F1,
sizeof(char),1))),_tmp3F3),((_tmp8EF[4]=Cyc_PP_line_doc(),((_tmp8EF[3]=Cyc_Absynpp_lb(),((
_tmp8EF[2]=Cyc_PP_blank_doc(),((_tmp8EF[1]=Cyc_Absynpp_qvar2doc(_tmp3F2),((
_tmp8EF[0]=Cyc_PP_text(((_tmp8F0="using ",_tag_dyneither(_tmp8F0,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmp8EF,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
else{const char*_tmp8FC;const char*_tmp8FB;const char*_tmp8FA;const char*_tmp8F9;
const char*_tmp8F8;struct Cyc_PP_Doc*_tmp8F7[11];s=((_tmp8F7[10]=Cyc_PP_text(((
_tmp8FC=" */",_tag_dyneither(_tmp8FC,sizeof(char),4)))),((_tmp8F7[9]=Cyc_Absynpp_rb(),((
_tmp8F7[8]=Cyc_PP_text(((_tmp8FB="/* ",_tag_dyneither(_tmp8FB,sizeof(char),4)))),((
_tmp8F7[7]=Cyc_PP_line_doc(),((_tmp8F7[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp8FA="",_tag_dyneither(_tmp8FA,sizeof(char),1))),
_tmp3F3),((_tmp8F7[5]=Cyc_PP_line_doc(),((_tmp8F7[4]=Cyc_PP_text(((_tmp8F9=" */",
_tag_dyneither(_tmp8F9,sizeof(char),4)))),((_tmp8F7[3]=Cyc_Absynpp_lb(),((
_tmp8F7[2]=Cyc_PP_blank_doc(),((_tmp8F7[1]=Cyc_Absynpp_qvar2doc(_tmp3F2),((
_tmp8F7[0]=Cyc_PP_text(((_tmp8F8="/* using ",_tag_dyneither(_tmp8F8,sizeof(char),
10)))),Cyc_PP_cat(_tag_dyneither(_tmp8F7,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL2BD;_LL2D6: if(*((int*)_tmp3D4)!= 12)goto _LL2D8;_tmp3F4=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp3D4)->f1;_LL2D7: if(Cyc_Absynpp_print_externC_stmts){const char*_tmp901;const
char*_tmp900;struct Cyc_PP_Doc*_tmp8FF[6];s=((_tmp8FF[5]=Cyc_Absynpp_rb(),((
_tmp8FF[4]=Cyc_PP_line_doc(),((_tmp8FF[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp901="",_tag_dyneither(_tmp901,sizeof(char),1))),
_tmp3F4),((_tmp8FF[2]=Cyc_PP_line_doc(),((_tmp8FF[1]=Cyc_Absynpp_lb(),((_tmp8FF[
0]=Cyc_PP_text(((_tmp900="extern \"C\" ",_tag_dyneither(_tmp900,sizeof(char),12)))),
Cyc_PP_cat(_tag_dyneither(_tmp8FF,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmp90C;const char*_tmp90B;const char*_tmp90A;const char*_tmp909;
const char*_tmp908;struct Cyc_PP_Doc*_tmp907[9];s=((_tmp907[8]=Cyc_PP_text(((
_tmp90C=" */",_tag_dyneither(_tmp90C,sizeof(char),4)))),((_tmp907[7]=Cyc_Absynpp_rb(),((
_tmp907[6]=Cyc_PP_text(((_tmp90B="/* ",_tag_dyneither(_tmp90B,sizeof(char),4)))),((
_tmp907[5]=Cyc_PP_line_doc(),((_tmp907[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp90A="",_tag_dyneither(_tmp90A,sizeof(char),1))),
_tmp3F4),((_tmp907[3]=Cyc_PP_line_doc(),((_tmp907[2]=Cyc_PP_text(((_tmp909=" */",
_tag_dyneither(_tmp909,sizeof(char),4)))),((_tmp907[1]=Cyc_Absynpp_lb(),((
_tmp907[0]=Cyc_PP_text(((_tmp908="/* extern \"C\" ",_tag_dyneither(_tmp908,
sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp907,sizeof(struct Cyc_PP_Doc*),
9))))))))))))))))))));}goto _LL2BD;_LL2D8: if(*((int*)_tmp3D4)!= 13)goto _LL2DA;
_tmp3F5=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp3D4)->f1;_tmp3F6=((struct
Cyc_Absyn_ExternCinclude_d_struct*)_tmp3D4)->f2;_LL2D9: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp3F6 != 0){const char*_tmp911;const char*_tmp910;
struct Cyc_PP_Doc*_tmp90F[7];exs_doc=((_tmp90F[6]=Cyc_Absynpp_rb(),((_tmp90F[5]=
Cyc_PP_line_doc(),((_tmp90F[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple11*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_export2doc,((_tmp911=",",_tag_dyneither(_tmp911,sizeof(char),2))),
_tmp3F6),((_tmp90F[3]=Cyc_PP_line_doc(),((_tmp90F[2]=Cyc_Absynpp_lb(),((_tmp90F[
1]=Cyc_PP_text(((_tmp910=" export ",_tag_dyneither(_tmp910,sizeof(char),9)))),((
_tmp90F[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmp90F,sizeof(struct Cyc_PP_Doc*),
7))))))))))))))));}else{exs_doc=Cyc_Absynpp_rb();}{const char*_tmp916;const char*
_tmp915;struct Cyc_PP_Doc*_tmp914[6];s=((_tmp914[5]=exs_doc,((_tmp914[4]=Cyc_PP_line_doc(),((
_tmp914[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmp916="",_tag_dyneither(_tmp916,sizeof(char),1))),_tmp3F5),((_tmp914[2]=Cyc_PP_line_doc(),((
_tmp914[1]=Cyc_Absynpp_lb(),((_tmp914[0]=Cyc_PP_text(((_tmp915="extern \"C include\" ",
_tag_dyneither(_tmp915,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmp914,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}}else{const char*_tmp921;const char*
_tmp920;const char*_tmp91F;const char*_tmp91E;const char*_tmp91D;struct Cyc_PP_Doc*
_tmp91C[9];s=((_tmp91C[8]=Cyc_PP_text(((_tmp921=" */",_tag_dyneither(_tmp921,
sizeof(char),4)))),((_tmp91C[7]=Cyc_Absynpp_rb(),((_tmp91C[6]=Cyc_PP_text(((
_tmp920="/* ",_tag_dyneither(_tmp920,sizeof(char),4)))),((_tmp91C[5]=Cyc_PP_line_doc(),((
_tmp91C[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmp91F="",_tag_dyneither(_tmp91F,sizeof(char),1))),_tmp3F5),((_tmp91C[3]=Cyc_PP_line_doc(),((
_tmp91C[2]=Cyc_PP_text(((_tmp91E=" */",_tag_dyneither(_tmp91E,sizeof(char),4)))),((
_tmp91C[1]=Cyc_Absynpp_lb(),((_tmp91C[0]=Cyc_PP_text(((_tmp91D="/* extern \"C include\" ",
_tag_dyneither(_tmp91D,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmp91C,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL2BD;_LL2DA: if((int)
_tmp3D4 != 0)goto _LL2DC;_LL2DB:{const char*_tmp924;struct Cyc_PP_Doc*_tmp923[2];s=((
_tmp923[1]=Cyc_Absynpp_lb(),((_tmp923[0]=Cyc_PP_text(((_tmp924="__cyclone_port_on__;",
_tag_dyneither(_tmp924,sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmp923,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL2BD;_LL2DC: if((int)_tmp3D4 != 1)goto
_LL2BD;_LL2DD:{const char*_tmp927;struct Cyc_PP_Doc*_tmp926[2];s=((_tmp926[1]=Cyc_Absynpp_lb(),((
_tmp926[0]=Cyc_PP_text(((_tmp927="__cyclone_port_off__;",_tag_dyneither(_tmp927,
sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmp926,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL2BD;_LL2BD:;}return s;}int Cyc_Absynpp_print_scopes=1;struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(void*sc);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*sc){
if(!Cyc_Absynpp_print_scopes)return Cyc_PP_nil_doc();{void*_tmp45E=sc;_LL2EE: if((
int)_tmp45E != 0)goto _LL2F0;_LL2EF: {const char*_tmp928;return Cyc_PP_text(((
_tmp928="static ",_tag_dyneither(_tmp928,sizeof(char),8))));}_LL2F0: if((int)
_tmp45E != 2)goto _LL2F2;_LL2F1: return Cyc_PP_nil_doc();_LL2F2: if((int)_tmp45E != 3)
goto _LL2F4;_LL2F3: {const char*_tmp929;return Cyc_PP_text(((_tmp929="extern ",
_tag_dyneither(_tmp929,sizeof(char),8))));}_LL2F4: if((int)_tmp45E != 4)goto _LL2F6;
_LL2F5: {const char*_tmp92A;return Cyc_PP_text(((_tmp92A="extern \"C\" ",
_tag_dyneither(_tmp92A,sizeof(char),12))));}_LL2F6: if((int)_tmp45E != 1)goto
_LL2F8;_LL2F7: {const char*_tmp92B;return Cyc_PP_text(((_tmp92B="abstract ",
_tag_dyneither(_tmp92B,sizeof(char),10))));}_LL2F8: if((int)_tmp45E != 5)goto
_LL2ED;_LL2F9: {const char*_tmp92C;return Cyc_PP_text(((_tmp92C="register ",
_tag_dyneither(_tmp92C,sizeof(char),10))));}_LL2ED:;}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t);int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){void*_tmp464=t;struct
Cyc_Absyn_Tvar*_tmp465;struct Cyc_List_List*_tmp466;_LL2FB: if(_tmp464 <= (void*)4)
goto _LL2FF;if(*((int*)_tmp464)!= 1)goto _LL2FD;_tmp465=((struct Cyc_Absyn_VarType_struct*)
_tmp464)->f1;_LL2FC: return Cyc_Tcutil_is_temp_tvar(_tmp465);_LL2FD: if(*((int*)
_tmp464)!= 20)goto _LL2FF;_tmp466=((struct Cyc_Absyn_JoinEff_struct*)_tmp464)->f1;
_LL2FE: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp466);
_LL2FF:;_LL300: return 0;_LL2FA:;}int Cyc_Absynpp_is_anon_aggrtype(void*t);int Cyc_Absynpp_is_anon_aggrtype(
void*t){void*_tmp467=t;void**_tmp468;void*_tmp469;_LL302: if(_tmp467 <= (void*)4)
goto _LL308;if(*((int*)_tmp467)!= 11)goto _LL304;_LL303: return 1;_LL304: if(*((int*)
_tmp467)!= 13)goto _LL306;_LL305: return 1;_LL306: if(*((int*)_tmp467)!= 16)goto
_LL308;_tmp468=((struct Cyc_Absyn_TypedefType_struct*)_tmp467)->f4;if(_tmp468 == 0)
goto _LL308;_tmp469=*_tmp468;_LL307: return Cyc_Absynpp_is_anon_aggrtype(_tmp469);
_LL308:;_LL309: return 0;_LL301:;}static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms);static struct Cyc_List_List*
Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*
tms){if(tms != 0  && tms->tl != 0){struct _tuple5 _tmp92D;struct _tuple5 _tmp46B=(
_tmp92D.f1=(void*)tms->hd,((_tmp92D.f2=(void*)((struct Cyc_List_List*)_check_null(
tms->tl))->hd,_tmp92D)));void*_tmp46C;void*_tmp46D;_LL30B: _tmp46C=_tmp46B.f1;if(*((
int*)_tmp46C)!= 2)goto _LL30D;_tmp46D=_tmp46B.f2;if(*((int*)_tmp46D)!= 3)goto
_LL30D;_LL30C: {struct Cyc_List_List*_tmp930;struct Cyc_List_List*_tmp92F;return(
_tmp92F=_region_malloc(r,sizeof(*_tmp92F)),((_tmp92F->hd=(void*)((void*)tms->hd),((
_tmp92F->tl=((_tmp930=_region_malloc(r,sizeof(*_tmp930)),((_tmp930->hd=(void*)((
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd),((_tmp930->tl=Cyc_Absynpp_bubble_attributes(
r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmp930)))))),_tmp92F)))));}
_LL30D:;_LL30E: {struct Cyc_List_List*_tmp931;return(_tmp931=_region_malloc(r,
sizeof(*_tmp931)),((_tmp931->hd=(void*)atts,((_tmp931->tl=tms,_tmp931)))));}
_LL30A:;}else{struct Cyc_List_List*_tmp932;return(_tmp932=_region_malloc(r,
sizeof(*_tmp932)),((_tmp932->hd=(void*)atts,((_tmp932->tl=tms,_tmp932)))));}}
struct _tuple7 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,
void*t);struct _tuple7 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t){void*_tmp472=t;struct Cyc_Absyn_ArrayInfo _tmp473;void*_tmp474;struct
Cyc_Absyn_Tqual _tmp475;struct Cyc_Absyn_Exp*_tmp476;struct Cyc_Absyn_Conref*
_tmp477;struct Cyc_Position_Segment*_tmp478;struct Cyc_Absyn_PtrInfo _tmp479;void*
_tmp47A;struct Cyc_Absyn_Tqual _tmp47B;struct Cyc_Absyn_PtrAtts _tmp47C;struct Cyc_Absyn_FnInfo
_tmp47D;struct Cyc_List_List*_tmp47E;struct Cyc_Core_Opt*_tmp47F;void*_tmp480;
struct Cyc_List_List*_tmp481;int _tmp482;struct Cyc_Absyn_VarargInfo*_tmp483;struct
Cyc_List_List*_tmp484;struct Cyc_List_List*_tmp485;struct Cyc_Core_Opt*_tmp486;
struct Cyc_Core_Opt*_tmp487;int _tmp488;struct _tuple0*_tmp489;struct Cyc_List_List*
_tmp48A;void**_tmp48B;_LL310: if(_tmp472 <= (void*)4)goto _LL31A;if(*((int*)_tmp472)
!= 7)goto _LL312;_tmp473=((struct Cyc_Absyn_ArrayType_struct*)_tmp472)->f1;_tmp474=(
void*)_tmp473.elt_type;_tmp475=_tmp473.tq;_tmp476=_tmp473.num_elts;_tmp477=
_tmp473.zero_term;_tmp478=_tmp473.zt_loc;_LL311: {struct Cyc_Absyn_Tqual _tmp48D;
void*_tmp48E;struct Cyc_List_List*_tmp48F;struct _tuple7 _tmp48C=Cyc_Absynpp_to_tms(
r,_tmp475,_tmp474);_tmp48D=_tmp48C.f1;_tmp48E=_tmp48C.f2;_tmp48F=_tmp48C.f3;{
void*tm;if(_tmp476 == 0){struct Cyc_Absyn_Carray_mod_struct _tmp935;struct Cyc_Absyn_Carray_mod_struct*
_tmp934;tm=(void*)((_tmp934=_region_malloc(r,sizeof(*_tmp934)),((_tmp934[0]=((
_tmp935.tag=0,((_tmp935.f1=_tmp477,((_tmp935.f2=_tmp478,_tmp935)))))),_tmp934))));}
else{struct Cyc_Absyn_ConstArray_mod_struct _tmp938;struct Cyc_Absyn_ConstArray_mod_struct*
_tmp937;tm=(void*)((_tmp937=_region_malloc(r,sizeof(*_tmp937)),((_tmp937[0]=((
_tmp938.tag=1,((_tmp938.f1=(struct Cyc_Absyn_Exp*)_tmp476,((_tmp938.f2=_tmp477,((
_tmp938.f3=_tmp478,_tmp938)))))))),_tmp937))));}{struct Cyc_List_List*_tmp93B;
struct _tuple7 _tmp93A;return(_tmp93A.f1=_tmp48D,((_tmp93A.f2=_tmp48E,((_tmp93A.f3=((
_tmp93B=_region_malloc(r,sizeof(*_tmp93B)),((_tmp93B->hd=(void*)tm,((_tmp93B->tl=
_tmp48F,_tmp93B)))))),_tmp93A)))));}}}_LL312: if(*((int*)_tmp472)!= 4)goto _LL314;
_tmp479=((struct Cyc_Absyn_PointerType_struct*)_tmp472)->f1;_tmp47A=(void*)
_tmp479.elt_typ;_tmp47B=_tmp479.elt_tq;_tmp47C=_tmp479.ptr_atts;_LL313: {struct
Cyc_Absyn_Tqual _tmp497;void*_tmp498;struct Cyc_List_List*_tmp499;struct _tuple7
_tmp496=Cyc_Absynpp_to_tms(r,_tmp47B,_tmp47A);_tmp497=_tmp496.f1;_tmp498=_tmp496.f2;
_tmp499=_tmp496.f3;{struct Cyc_Absyn_Pointer_mod_struct*_tmp941;struct Cyc_Absyn_Pointer_mod_struct
_tmp940;struct Cyc_List_List*_tmp93F;_tmp499=((_tmp93F=_region_malloc(r,sizeof(*
_tmp93F)),((_tmp93F->hd=(void*)((void*)((_tmp941=_region_malloc(r,sizeof(*
_tmp941)),((_tmp941[0]=((_tmp940.tag=2,((_tmp940.f1=_tmp47C,((_tmp940.f2=tq,
_tmp940)))))),_tmp941))))),((_tmp93F->tl=_tmp499,_tmp93F))))));}{struct _tuple7
_tmp942;return(_tmp942.f1=_tmp497,((_tmp942.f2=_tmp498,((_tmp942.f3=_tmp499,
_tmp942)))));}}_LL314: if(*((int*)_tmp472)!= 8)goto _LL316;_tmp47D=((struct Cyc_Absyn_FnType_struct*)
_tmp472)->f1;_tmp47E=_tmp47D.tvars;_tmp47F=_tmp47D.effect;_tmp480=(void*)_tmp47D.ret_typ;
_tmp481=_tmp47D.args;_tmp482=_tmp47D.c_varargs;_tmp483=_tmp47D.cyc_varargs;
_tmp484=_tmp47D.rgn_po;_tmp485=_tmp47D.attributes;_LL315: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp47F == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp47F->v)){
_tmp47F=0;_tmp47E=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp47E);}{struct Cyc_Absyn_Tqual _tmp49F;void*_tmp4A0;struct Cyc_List_List*_tmp4A1;
struct _tuple7 _tmp49E=Cyc_Absynpp_to_tms(r,Cyc_Absyn_empty_tqual(0),_tmp480);
_tmp49F=_tmp49E.f1;_tmp4A0=_tmp49E.f2;_tmp4A1=_tmp49E.f3;{struct Cyc_List_List*
tms=_tmp4A1;{void*_tmp4A2=Cyc_Cyclone_c_compiler;_LL31D: if((int)_tmp4A2 != 0)goto
_LL31F;_LL31E: if(_tmp485 != 0){struct Cyc_Absyn_Attributes_mod_struct _tmp945;
struct Cyc_Absyn_Attributes_mod_struct*_tmp944;tms=Cyc_Absynpp_bubble_attributes(
r,(void*)((_tmp944=_region_malloc(r,sizeof(*_tmp944)),((_tmp944[0]=((_tmp945.tag=
5,((_tmp945.f1=0,((_tmp945.f2=_tmp485,_tmp945)))))),_tmp944)))),tms);}{struct Cyc_Absyn_Function_mod_struct*
_tmp954;struct Cyc_Absyn_WithTypes_struct*_tmp953;struct Cyc_Absyn_WithTypes_struct
_tmp952;struct Cyc_Absyn_Function_mod_struct _tmp951;struct Cyc_List_List*_tmp950;
tms=((_tmp950=_region_malloc(r,sizeof(*_tmp950)),((_tmp950->hd=(void*)((void*)((
_tmp954=_region_malloc(r,sizeof(*_tmp954)),((_tmp954[0]=((_tmp951.tag=3,((
_tmp951.f1=(void*)((void*)((_tmp953=_region_malloc(r,sizeof(*_tmp953)),((_tmp953[
0]=((_tmp952.tag=1,((_tmp952.f1=_tmp481,((_tmp952.f2=_tmp482,((_tmp952.f3=
_tmp483,((_tmp952.f4=_tmp47F,((_tmp952.f5=_tmp484,_tmp952)))))))))))),_tmp953))))),
_tmp951)))),_tmp954))))),((_tmp950->tl=tms,_tmp950))))));}goto _LL31C;_LL31F: if((
int)_tmp4A2 != 1)goto _LL31C;_LL320:{struct Cyc_Absyn_Function_mod_struct*_tmp963;
struct Cyc_Absyn_WithTypes_struct*_tmp962;struct Cyc_Absyn_WithTypes_struct _tmp961;
struct Cyc_Absyn_Function_mod_struct _tmp960;struct Cyc_List_List*_tmp95F;tms=((
_tmp95F=_region_malloc(r,sizeof(*_tmp95F)),((_tmp95F->hd=(void*)((void*)((
_tmp963=_region_malloc(r,sizeof(*_tmp963)),((_tmp963[0]=((_tmp960.tag=3,((
_tmp960.f1=(void*)((void*)((_tmp962=_region_malloc(r,sizeof(*_tmp962)),((_tmp962[
0]=((_tmp961.tag=1,((_tmp961.f1=_tmp481,((_tmp961.f2=_tmp482,((_tmp961.f3=
_tmp483,((_tmp961.f4=_tmp47F,((_tmp961.f5=_tmp484,_tmp961)))))))))))),_tmp962))))),
_tmp960)))),_tmp963))))),((_tmp95F->tl=tms,_tmp95F))))));}for(0;_tmp485 != 0;
_tmp485=_tmp485->tl){void*_tmp4AF=(void*)_tmp485->hd;_LL322: if((int)_tmp4AF != 0)
goto _LL324;_LL323: goto _LL325;_LL324: if((int)_tmp4AF != 1)goto _LL326;_LL325: goto
_LL327;_LL326: if((int)_tmp4AF != 2)goto _LL328;_LL327:{struct Cyc_Absyn_Attributes_mod_struct*
_tmp96D;struct Cyc_List_List*_tmp96C;struct Cyc_Absyn_Attributes_mod_struct _tmp96B;
struct Cyc_List_List*_tmp96A;tms=((_tmp96A=_region_malloc(r,sizeof(*_tmp96A)),((
_tmp96A->hd=(void*)((void*)((_tmp96D=_region_malloc(r,sizeof(*_tmp96D)),((
_tmp96D[0]=((_tmp96B.tag=5,((_tmp96B.f1=0,((_tmp96B.f2=((_tmp96C=_cycalloc(
sizeof(*_tmp96C)),((_tmp96C->hd=(void*)((void*)_tmp485->hd),((_tmp96C->tl=0,
_tmp96C)))))),_tmp96B)))))),_tmp96D))))),((_tmp96A->tl=tms,_tmp96A))))));}goto
AfterAtts;_LL328:;_LL329: goto _LL321;_LL321:;}goto _LL31C;_LL31C:;}AfterAtts: if(
_tmp47E != 0){struct Cyc_Absyn_TypeParams_mod_struct*_tmp973;struct Cyc_Absyn_TypeParams_mod_struct
_tmp972;struct Cyc_List_List*_tmp971;tms=((_tmp971=_region_malloc(r,sizeof(*
_tmp971)),((_tmp971->hd=(void*)((void*)((_tmp973=_region_malloc(r,sizeof(*
_tmp973)),((_tmp973[0]=((_tmp972.tag=4,((_tmp972.f1=_tmp47E,((_tmp972.f2=0,((
_tmp972.f3=1,_tmp972)))))))),_tmp973))))),((_tmp971->tl=tms,_tmp971))))));}{
struct _tuple7 _tmp974;return(_tmp974.f1=_tmp49F,((_tmp974.f2=_tmp4A0,((_tmp974.f3=
tms,_tmp974)))));}}}_LL316: if(*((int*)_tmp472)!= 0)goto _LL318;_tmp486=((struct
Cyc_Absyn_Evar_struct*)_tmp472)->f1;_tmp487=((struct Cyc_Absyn_Evar_struct*)
_tmp472)->f2;_tmp488=((struct Cyc_Absyn_Evar_struct*)_tmp472)->f3;_LL317: if(
_tmp487 == 0){struct _tuple7 _tmp975;return(_tmp975.f1=tq,((_tmp975.f2=t,((_tmp975.f3=
0,_tmp975)))));}else{return Cyc_Absynpp_to_tms(r,tq,(void*)_tmp487->v);}_LL318:
if(*((int*)_tmp472)!= 16)goto _LL31A;_tmp489=((struct Cyc_Absyn_TypedefType_struct*)
_tmp472)->f1;_tmp48A=((struct Cyc_Absyn_TypedefType_struct*)_tmp472)->f2;_tmp48B=((
struct Cyc_Absyn_TypedefType_struct*)_tmp472)->f4;_LL319: if((_tmp48B == 0  || !Cyc_Absynpp_expand_typedefs)
 || Cyc_Absynpp_is_anon_aggrtype(*_tmp48B)){struct _tuple7 _tmp976;return(_tmp976.f1=
tq,((_tmp976.f2=t,((_tmp976.f3=0,_tmp976)))));}else{return Cyc_Absynpp_to_tms(r,
tq,*_tmp48B);}_LL31A:;_LL31B: {struct _tuple7 _tmp977;return(_tmp977.f1=tq,((
_tmp977.f2=t,((_tmp977.f3=0,_tmp977)))));}_LL30F:;}static int Cyc_Absynpp_is_char_ptr(
void*t);static int Cyc_Absynpp_is_char_ptr(void*t){void*_tmp4BB=t;struct Cyc_Core_Opt*
_tmp4BC;struct Cyc_Core_Opt _tmp4BD;void*_tmp4BE;struct Cyc_Absyn_PtrInfo _tmp4BF;
void*_tmp4C0;_LL32B: if(_tmp4BB <= (void*)4)goto _LL32F;if(*((int*)_tmp4BB)!= 0)
goto _LL32D;_tmp4BC=((struct Cyc_Absyn_Evar_struct*)_tmp4BB)->f2;if(_tmp4BC == 0)
goto _LL32D;_tmp4BD=*_tmp4BC;_tmp4BE=(void*)_tmp4BD.v;_LL32C: return Cyc_Absynpp_is_char_ptr(
_tmp4BE);_LL32D: if(*((int*)_tmp4BB)!= 4)goto _LL32F;_tmp4BF=((struct Cyc_Absyn_PointerType_struct*)
_tmp4BB)->f1;_tmp4C0=(void*)_tmp4BF.elt_typ;_LL32E: L: {void*_tmp4C1=_tmp4C0;
struct Cyc_Core_Opt*_tmp4C2;struct Cyc_Core_Opt _tmp4C3;void*_tmp4C4;void**_tmp4C5;
void*_tmp4C6;void*_tmp4C7;_LL332: if(_tmp4C1 <= (void*)4)goto _LL338;if(*((int*)
_tmp4C1)!= 0)goto _LL334;_tmp4C2=((struct Cyc_Absyn_Evar_struct*)_tmp4C1)->f2;if(
_tmp4C2 == 0)goto _LL334;_tmp4C3=*_tmp4C2;_tmp4C4=(void*)_tmp4C3.v;_LL333: _tmp4C0=
_tmp4C4;goto L;_LL334: if(*((int*)_tmp4C1)!= 16)goto _LL336;_tmp4C5=((struct Cyc_Absyn_TypedefType_struct*)
_tmp4C1)->f4;if(_tmp4C5 == 0)goto _LL336;_tmp4C6=*_tmp4C5;_LL335: _tmp4C0=_tmp4C6;
goto L;_LL336: if(*((int*)_tmp4C1)!= 5)goto _LL338;_tmp4C7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4C1)->f2;if((int)_tmp4C7 != 0)goto _LL338;_LL337: return 1;_LL338:;_LL339: return
0;_LL331:;}_LL32F:;_LL330: return 0;_LL32A:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){struct _RegionHandle
_tmp4C8=_new_region("temp");struct _RegionHandle*temp=& _tmp4C8;_push_region(temp);{
struct Cyc_Absyn_Tqual _tmp4CA;void*_tmp4CB;struct Cyc_List_List*_tmp4CC;struct
_tuple7 _tmp4C9=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp4CA=_tmp4C9.f1;_tmp4CB=
_tmp4C9.f2;_tmp4CC=_tmp4C9.f3;_tmp4CC=Cyc_List_imp_rev(_tmp4CC);if(_tmp4CC == 0
 && dopt == 0){struct Cyc_PP_Doc*_tmp978[2];struct Cyc_PP_Doc*_tmp4CE=(_tmp978[1]=
Cyc_Absynpp_ntyp2doc(_tmp4CB),((_tmp978[0]=Cyc_Absynpp_tqual2doc(_tmp4CA),Cyc_PP_cat(
_tag_dyneither(_tmp978,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return
_tmp4CE;}else{const char*_tmp97B;struct Cyc_PP_Doc*_tmp97A[4];struct Cyc_PP_Doc*
_tmp4D1=(_tmp97A[3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?
Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp4CC),((_tmp97A[2]=Cyc_PP_text(((
_tmp97B=" ",_tag_dyneither(_tmp97B,sizeof(char),2)))),((_tmp97A[1]=Cyc_Absynpp_ntyp2doc(
_tmp4CB),((_tmp97A[0]=Cyc_Absynpp_tqual2doc(_tmp4CA),Cyc_PP_cat(_tag_dyneither(
_tmp97A,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp4D1;}};
_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*
f);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){void*
_tmp4D2=Cyc_Cyclone_c_compiler;_LL33B: if((int)_tmp4D2 != 0)goto _LL33D;_LL33C: if(f->width
!= 0){const char*_tmp982;const char*_tmp981;struct Cyc_Core_Opt*_tmp980;struct Cyc_PP_Doc*
_tmp97F[5];return(_tmp97F[4]=Cyc_PP_text(((_tmp982=";",_tag_dyneither(_tmp982,
sizeof(char),2)))),((_tmp97F[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmp97F[2]=
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmp97F[1]=
Cyc_PP_text(((_tmp981=":",_tag_dyneither(_tmp981,sizeof(char),2)))),((_tmp97F[0]=
Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,((_tmp980=_cycalloc(sizeof(*_tmp980)),((
_tmp980->v=Cyc_PP_textptr(f->name),_tmp980))))),Cyc_PP_cat(_tag_dyneither(
_tmp97F,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{const char*_tmp987;struct Cyc_Core_Opt*
_tmp986;struct Cyc_PP_Doc*_tmp985[3];return(_tmp985[2]=Cyc_PP_text(((_tmp987=";",
_tag_dyneither(_tmp987,sizeof(char),2)))),((_tmp985[1]=Cyc_Absynpp_atts2doc(f->attributes),((
_tmp985[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,((_tmp986=_cycalloc(sizeof(*
_tmp986)),((_tmp986->v=Cyc_PP_textptr(f->name),_tmp986))))),Cyc_PP_cat(
_tag_dyneither(_tmp985,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL33D: if((int)
_tmp4D2 != 1)goto _LL33A;_LL33E: if(f->width != 0){const char*_tmp98E;const char*
_tmp98D;struct Cyc_Core_Opt*_tmp98C;struct Cyc_PP_Doc*_tmp98B[5];return(_tmp98B[4]=
Cyc_PP_text(((_tmp98E=";",_tag_dyneither(_tmp98E,sizeof(char),2)))),((_tmp98B[3]=
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmp98B[2]=
Cyc_PP_text(((_tmp98D=":",_tag_dyneither(_tmp98D,sizeof(char),2)))),((_tmp98B[1]=
Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,((_tmp98C=_cycalloc(sizeof(*_tmp98C)),((
_tmp98C->v=Cyc_PP_textptr(f->name),_tmp98C))))),((_tmp98B[0]=Cyc_Absynpp_atts2doc(
f->attributes),Cyc_PP_cat(_tag_dyneither(_tmp98B,sizeof(struct Cyc_PP_Doc*),5)))))))))));}
else{const char*_tmp993;struct Cyc_Core_Opt*_tmp992;struct Cyc_PP_Doc*_tmp991[3];
return(_tmp991[2]=Cyc_PP_text(((_tmp993=";",_tag_dyneither(_tmp993,sizeof(char),
2)))),((_tmp991[1]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,((_tmp992=_cycalloc(
sizeof(*_tmp992)),((_tmp992->v=Cyc_PP_textptr(f->name),_tmp992))))),((_tmp991[0]=
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmp991,sizeof(
struct Cyc_PP_Doc*),3)))))));}_LL33A:;}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct
Cyc_List_List*fields){const char*_tmp994;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))
Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmp994="",_tag_dyneither(_tmp994,
sizeof(char),1))),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct
Cyc_Absyn_Datatypefield*f);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct
Cyc_Absyn_Datatypefield*f){struct Cyc_PP_Doc*_tmp995[3];return(_tmp995[2]=f->typs
== 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmp995[1]=Cyc_Absynpp_typedef_name2doc(
f->name),((_tmp995[0]=Cyc_Absynpp_scope2doc((void*)f->sc),Cyc_PP_cat(
_tag_dyneither(_tmp995,sizeof(struct Cyc_PP_Doc*),3)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields){const char*_tmp996;return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmp996=",",
_tag_dyneither(_tmp996,sizeof(char),2))),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{const char*_tmp999;
void*_tmp998;(_tmp998=0,Cyc_fprintf(f,((_tmp999="\n",_tag_dyneither(_tmp999,
sizeof(char),2))),_tag_dyneither(_tmp998,sizeof(void*),0)));}}}struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){const char*
_tmp99A;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmp99A="",_tag_dyneither(
_tmp99A,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
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
