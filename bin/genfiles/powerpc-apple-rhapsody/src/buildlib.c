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
 void exit(int);void*abort();struct Cyc_Core_NewRegion{struct _DynRegionHandle*
dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[16];
extern char Cyc_Core_Free_Region[16];struct _dyneither_ptr Cstring_to_string(char*);
struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;extern struct Cyc___cycFILE*
Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct
Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;
long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_fflush(struct
Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);int Cyc_fprintf(
struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);int Cyc_fputc(int,
struct Cyc___cycFILE*);int Cyc_fputs(const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
int remove(const char*);int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};extern char
Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{char*tag;struct _dyneither_ptr
f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*
refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int
lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int
lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct
_dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);char Cyc_Lexing_lexeme_char(
struct Cyc_Lexing_lexbuf*,int);int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*
cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);struct
Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(
struct Cyc_Set_Set*s);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char
Cyc_Set_Absent[11];struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,
struct Cyc_Set_Set*s);unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strptrcmp(
struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_strdup(struct
_dyneither_ptr src);struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,
unsigned long n);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*Cyc_Hashtable_create(
int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(struct
Cyc_Hashtable_Table*t,void*key,void*val);void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*
t,void*key);int Cyc_Hashtable_hash_stringptr(struct _dyneither_ptr*p);struct
_dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Filename_chop_extension(struct _dyneither_ptr);struct
_dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);struct _dyneither_ptr Cyc_Filename_basename(
struct _dyneither_ptr);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;
struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];void
Cyc_Position_reset_position(struct _dyneither_ptr);struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];
extern char Cyc_Arg_Bad[8];struct Cyc_Arg_Bad_struct{char*tag;struct _dyneither_ptr
f1;};extern char Cyc_Arg_Error[10];struct Cyc_Arg_Unit_spec_struct{int tag;void(*f1)();
};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct
Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct
_dyneither_ptr);};struct Cyc_Arg_Set_spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{
int tag;int*f1;};struct Cyc_Arg_String_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;void(*f1)(int);};struct
Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};void Cyc_Arg_usage(
struct Cyc_List_List*,struct _dyneither_ptr);void Cyc_Arg_parse(struct Cyc_List_List*
specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct
_dyneither_ptr args);struct Cyc_Buffer_t;struct Cyc_Buffer_t*Cyc_Buffer_create(
unsigned int n);struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);void
Cyc_Buffer_add_char(struct Cyc_Buffer_t*,char);void Cyc_Buffer_add_string(struct
Cyc_Buffer_t*,struct _dyneither_ptr);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct _tuple9{void*f1;
struct _tuple0*f2;};struct _tuple9 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f);struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct
_union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;
struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple10{
struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*
f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple10*val;};struct
_union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;void*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*val;};struct
_union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY11{
int tag;struct Cyc_Absyn_Pat*val;};struct _tuple11{struct Cyc_List_List*f1;int f2;};
struct _union_YYSTYPE_YY12{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY13{
int tag;struct Cyc_List_List*val;};struct _tuple12{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple12*val;};struct
_union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY16{
int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Absyn_Fndecl*
val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec*val;};struct _tuple13{
struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY20{int
tag;struct _tuple13*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY22{int tag;void*val;};struct _union_YYSTYPE_YY23{int
tag;void*val;};struct _union_YYSTYPE_YY24{int tag;void*val;};struct
_union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY26{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY29{int tag;struct Cyc_Declarator*val;};struct _union_YYSTYPE_YY30{
int tag;struct Cyc_Abstractdeclarator*val;};struct _union_YYSTYPE_YY31{int tag;int
val;};struct _union_YYSTYPE_YY32{int tag;void*val;};struct _union_YYSTYPE_YY33{int
tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct
Cyc_List_List*val;};struct _tuple14{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple14*val;
};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY37{int tag;struct _tuple7*val;};struct _union_YYSTYPE_YY38{int tag;
struct Cyc_List_List*val;};struct _tuple15{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*
f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int
tag;struct _tuple15*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;void*val;
};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct
_union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct
_union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{
int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};
struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct
_union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct
_union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok
Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_YY1
YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct
_union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;
struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9
YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct
_union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14
YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct
_union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19
YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct
_union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24
YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct
_union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29
YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct
_union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34
YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct
_union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39
YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct
_union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44
YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct
_union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49
YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct
_union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54
YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int
timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct _tuple16{
void*f1;void*f2;};struct _tuple16*Cyc_Dict_rchoose(struct _RegionHandle*r,struct
Cyc_Dict_Dict d);struct _tuple16*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict
d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,
struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn,struct Cyc_Position_Segment*loc);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,
struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);
struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*tl;};struct Cyc_Tcenv_VarRes_struct{
int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;
struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct
Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;int allow_valueof;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,
struct Cyc_List_List*ds);typedef struct{int quot;int rem;}Cyc_div_t;typedef struct{
long quot;long rem;}Cyc_ldiv_t;void*abort();void exit(int);int system(const char*);
struct Cyc_timespec{long tv_sec;int tv_nsec;};struct Cyc_stat{int st_dev;unsigned int
st_ino;unsigned short st_mode;unsigned short st_nlink;unsigned int st_uid;
unsigned int st_gid;int st_rdev;struct Cyc_timespec st_atimespec;struct Cyc_timespec
st_mtimespec;struct Cyc_timespec st_ctimespec;long long st_size;long long st_blocks;
unsigned int st_blksize;unsigned int st_flags;unsigned int st_gen;int st_lspare;
long long st_qspare[2];};int mkdir(const char*pathname,unsigned short mode);struct
Cyc_flock{long long l_start;long long l_len;int l_pid;short l_type;short l_whence;};
struct Cyc_Flock_struct{int tag;struct Cyc_flock*f1;};struct Cyc_Long_struct{int tag;
long f1;};int Cyc_open(const char*,int,struct _dyneither_ptr);struct Cyc_option{
struct _dyneither_ptr name;int has_arg;int*flag;int val;};int chdir(const char*);int
close(int);struct _dyneither_ptr Cyc_getcwd(struct _dyneither_ptr buf,unsigned long
size);int isspace(int);int toupper(int);void Cyc_Lex_lex_init(int
use_cyclone_keywords);extern char*Ccomp;static int Cyc_do_setjmp=0;struct Cyc___cycFILE*
Cyc_log_file=0;struct Cyc___cycFILE*Cyc_cstubs_file=0;struct Cyc___cycFILE*Cyc_cycstubs_file=
0;int Cyc_log(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);int Cyc_log(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap){if(Cyc_log_file == 0){{const char*
_tmp32A;void*_tmp329;(_tmp329=0,Cyc_fprintf(Cyc_stderr,((_tmp32A="Internal error: log file is NULL\n",
_tag_dyneither(_tmp32A,sizeof(char),34))),_tag_dyneither(_tmp329,sizeof(void*),0)));}
exit(1);}{int _tmp2=Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_log_file),
fmt,ap);Cyc_fflush((struct Cyc___cycFILE*)((struct Cyc___cycFILE*)_check_null(Cyc_log_file)));
return _tmp2;}}static struct _dyneither_ptr*Cyc_current_source=0;static struct Cyc_List_List*
Cyc_current_args=0;static struct Cyc_Set_Set**Cyc_current_targets=0;static void Cyc_add_target(
struct _dyneither_ptr*sptr);static void Cyc_add_target(struct _dyneither_ptr*sptr){
struct Cyc_Set_Set**_tmp32B;Cyc_current_targets=((_tmp32B=_cycalloc(sizeof(*
_tmp32B)),((_tmp32B[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),
sptr),_tmp32B))));}struct _tuple17{struct _dyneither_ptr*f1;struct Cyc_Set_Set*f2;}
;struct _tuple17*Cyc_line(struct Cyc_Lexing_lexbuf*);int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);int Cyc_token(struct Cyc_Lexing_lexbuf*);int
Cyc_string(struct Cyc_Lexing_lexbuf*);struct Cyc___cycFILE*Cyc_slurp_out=0;int Cyc_slurp(
struct Cyc_Lexing_lexbuf*);int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm(
struct Cyc_Lexing_lexbuf*);int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm_comment(
struct Cyc_Lexing_lexbuf*);struct _tuple18{struct _dyneither_ptr f1;struct
_dyneither_ptr*f2;};struct _tuple18*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);int
Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_line=(struct _dyneither_ptr){(void*)0,(void*)0,(
void*)(0 + 0)};struct _tuple19{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
struct _tuple20{struct _dyneither_ptr f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct
_tuple20*Cyc_spec(struct Cyc_Lexing_lexbuf*);int Cyc_commands(struct Cyc_Lexing_lexbuf*);
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);int Cyc_block(struct Cyc_Lexing_lexbuf*);
int Cyc_block_string(struct Cyc_Lexing_lexbuf*);int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_headerfile=(struct _dyneither_ptr){(void*)0,(void*)
0,(void*)(0 + 0)};struct Cyc_List_List*Cyc_snarfed_symbols=0;struct Cyc_List_List*
Cyc_current_symbols=0;struct Cyc_List_List*Cyc_current_cstubs=0;struct Cyc_List_List*
Cyc_current_cycstubs=0;struct Cyc_List_List*Cyc_current_hstubs=0;struct Cyc_List_List*
Cyc_current_omit_symbols=0;struct Cyc_List_List*Cyc_current_cpp=0;struct Cyc_Buffer_t*
Cyc_specbuf=0;int Cyc_braces_to_match=0;int Cyc_parens_to_match=0;int Cyc_numdef=0;
const int Cyc_lex_base[406]=(const int[406]){0,0,75,192,305,310,311,166,312,91,27,
384,28,523,637,715,283,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,797,312,
- 6,- 7,- 4,16,- 3,29,11,835,- 3,910,13,- 14,223,12,- 2,216,20,26,28,33,23,48,70,54,64,
74,100,91,107,94,370,386,111,103,95,122,122,375,414,111,111,153,377,1024,1139,
525,176,210,228,214,216,217,242,498,1253,1368,- 9,654,- 10,224,245,508,1482,1597,
684,- 8,720,- 11,432,510,515,1674,1751,1828,1909,434,465,550,1984,249,249,249,248,
244,254,0,13,4,2065,5,628,2073,2138,660,49,467,6,2099,7,705,2161,2199,820,- 21,
1051,1056,261,314,242,251,258,250,271,281,274,275,278,288,295,282,- 19,292,296,
300,313,321,326,309,325,319,315,326,365,400,417,418,404,400,400,427,431,- 16,422,
421,415,436,433,449,427,449,453,441,445,436,436,- 18,444,438,442,453,464,447,449,
482,489,490,1,4,6,491,492,505,504,516,516,524,554,2,21,556,557,654,19,20,21,592,
555,554,585,592,594,23,650,651,- 13,600,603,658,659,660,622,623,678,679,686,636,
637,693,697,698,645,648,703,704,705,- 12,654,655,1030,- 20,1165,654,664,661,671,
670,665,668,698,700,698,712,1144,730,731,730,743,1258,1170,747,758,748,749,747,
760,1370,752,753,765,778,1375,- 7,- 8,8,1263,2231,9,996,2255,2293,1341,1279,- 49,
1150,- 2,817,- 4,818,997,1028,819,993,1023,1448,820,2320,2363,824,866,868,907,2433,
870,991,- 36,- 42,- 37,2508,- 28,909,- 40,- 25,911,- 27,- 45,- 39,- 48,2583,2612,1467,889,
979,1563,2622,2652,1582,2281,2685,2716,2754,1001,1090,2824,2862,1082,1092,1084,
1127,1148,1204,- 6,- 34,927,2794,- 47,- 30,- 32,- 46,- 29,- 31,- 33,962,2902,963,964,2128,
965,1002,1003,1014,1018,1020,1024,1032,1043,2975,3059,- 23,- 17,- 15,- 22,2239,1077,
- 24,- 41,- 38,- 35,- 26,1282,3141,4,3224,1076,15,1051,1055,1056,1058,1054,1071,1141};
const int Cyc_lex_backtrk[406]=(const int[406]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,6,- 1,5,1,0,
- 1,0,1,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,
6,5,2,8,3,5,- 1,6,5,- 1,20,20,20,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,14,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,16,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,
4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,
43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,
- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_default[
406]=(const int[406]){- 1,- 1,- 1,297,286,138,23,36,23,19,- 1,- 1,12,31,46,31,36,23,19,
0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,0,40,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,103,- 1,- 1,
- 1,- 1,- 1,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,-
1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,366,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,
- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,
0,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_trans[
3481]=(const int[3481]){0,0,0,0,0,0,0,0,0,0,22,19,28,398,19,28,19,28,100,19,45,45,
45,45,45,22,45,0,0,0,0,0,21,202,213,399,21,22,- 1,- 1,22,- 1,- 1,45,203,45,204,22,
396,396,396,396,396,396,396,396,396,396,31,103,22,214,114,40,227,396,396,396,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,
396,396,31,245,240,235,396,130,396,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,394,394,394,394,394,394,
394,394,394,394,121,20,74,67,54,55,56,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,57,58,59,60,394,
61,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,394,394,62,63,37,298,299,298,298,299,39,22,64,65,68,69,70,
129,34,34,34,34,34,34,34,34,71,72,298,300,301,75,76,302,303,304,104,104,305,306,
104,307,308,309,310,311,311,311,311,311,311,311,311,311,312,77,313,314,315,104,
19,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,
316,316,316,316,316,316,316,19,- 1,- 1,317,316,101,316,316,316,316,316,316,316,316,
316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,287,318,
37,288,139,139,24,24,139,131,122,94,84,81,85,38,82,86,28,87,24,29,83,25,289,88,
95,139,96,22,26,26,21,21,115,116,117,140,118,119,120,142,191,192,26,35,35,35,35,
35,35,35,35,177,171,162,30,30,30,30,30,30,30,30,66,66,156,149,66,73,73,78,78,73,
150,78,151,152,153,154,66,66,290,155,66,157,158,66,132,123,141,39,73,22,78,159,
143,144,145,160,146,161,27,66,147,31,46,21,73,73,163,164,73,148,165,166,167,113,
113,113,113,113,113,113,113,113,113,- 1,32,- 1,- 1,73,- 1,22,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
- 1,168,- 1,- 1,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,89,89,28,169,89,170,100,172,173,174,
97,97,104,104,97,175,104,112,112,176,178,112,179,89,180,105,105,80,80,105,19,80,
21,97,181,104,182,183,184,185,112,186,187,188,189,190,251,193,105,194,80,195,112,
112,91,196,112,197,198,21,21,21,106,107,106,106,106,106,106,106,106,106,106,106,
21,112,199,200,201,205,206,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,207,208,209,210,106,211,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,47,47,35,212,47,215,216,33,33,33,33,33,33,33,33,33,33,92,92,
230,222,92,223,47,33,33,33,33,33,33,128,128,128,128,128,128,128,128,48,224,92,
225,- 1,226,- 1,228,229,99,99,231,49,99,232,233,234,93,33,33,33,33,33,33,35,35,35,
35,35,35,35,35,99,236,237,238,239,- 1,217,- 1,41,41,218,102,41,101,101,241,242,101,
243,219,50,220,244,91,246,51,52,247,248,249,250,41,53,252,253,268,101,137,137,
137,137,137,137,137,137,263,259,42,42,42,42,42,42,42,42,42,42,221,260,261,262,93,
36,28,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,264,100,265,266,42,267,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,269,270,43,271,272,102,281,33,33,33,33,33,33,33,33,33,33,
276,277,278,279,280,282,283,33,33,33,33,33,33,35,35,35,35,35,35,35,35,284,285,
392,391,365,356,331,44,44,44,44,44,44,44,44,44,44,21,33,33,33,33,33,33,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,327,328,329,
326,44,321,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,323,324,325,28,330,359,100,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,388,254,155,138,
44,100,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,78,78,389,363,78,- 1,253,253,190,102,253,295,295,295,295,295,295,295,295,
319,250,364,100,78,229,390,46,273,273,253,384,273,255,255,- 1,360,255,254,176,79,
79,79,79,79,79,79,79,79,79,383,273,361,362,35,100,255,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,320,385,21,35,79,367,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,31,80,80,
274,400,80,267,267,256,401,267,402,393,275,393,393,403,404,257,35,31,31,36,258,
80,405,21,255,255,267,0,255,273,273,0,393,273,19,0,0,79,79,79,79,79,79,79,79,79,
79,255,35,31,31,36,273,36,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,0,0,0,355,79,36,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,89,89,256,355,89,272,272,274,0,272,296,
296,257,0,296,0,0,275,355,0,0,0,- 1,89,0,0,296,296,272,393,296,393,393,296,0,21,
31,0,0,90,90,90,90,90,90,90,90,90,90,296,355,21,393,0,0,0,90,90,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,0,90,0,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,91,92,92,
280,280,92,0,280,285,285,0,0,285,31,31,31,31,31,31,31,31,0,0,0,92,0,280,0,0,0,0,
285,0,0,28,0,0,0,0,35,90,90,90,90,90,90,90,90,90,90,0,0,0,0,0,0,0,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,0,90,0,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,97,97,
0,357,97,358,358,358,358,358,358,358,358,358,358,0,0,0,0,337,0,337,0,97,338,338,
338,338,338,338,338,338,338,338,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,0,0,0,0,
0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,36,99,99,0,0,99,338,338,338,338,338,338,338,338,338,338,0,0,0,0,
341,0,341,0,99,342,342,342,342,342,342,342,342,342,342,0,0,0,0,0,98,98,98,98,98,
98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,108,108,108,108,108,108,108,108,108,108,
108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,108,0,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,109,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,0,0,0,0,108,0,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,0,0,- 1,108,0,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,
111,0,0,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,0,0,0,0,109,0,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
113,113,113,113,113,113,113,113,113,113,0,0,0,0,0,0,0,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
28,0,0,124,113,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,0,0,28,0,0,133,125,125,125,125,125,125,
125,125,127,127,127,127,127,127,127,127,127,127,0,0,0,0,0,0,0,127,127,127,127,
127,127,0,0,0,134,134,134,134,134,134,134,134,0,0,0,0,0,0,0,31,0,0,- 1,0,385,0,0,
127,127,127,127,127,127,386,386,386,386,386,386,386,386,0,126,127,127,127,127,
127,127,127,127,127,127,31,0,0,0,0,0,0,127,127,127,127,127,127,136,136,136,136,
136,136,136,136,136,136,135,0,0,0,0,0,0,136,136,136,136,136,136,0,0,0,127,127,
127,127,127,127,19,0,0,291,0,0,136,136,136,136,136,136,136,136,136,136,0,136,136,
136,136,136,136,136,136,136,136,136,136,0,0,0,0,0,0,0,0,385,292,292,292,292,292,
292,292,292,387,387,387,387,387,387,387,387,0,136,136,136,136,136,136,0,294,294,
294,294,294,294,294,294,294,294,0,0,0,0,0,0,0,294,294,294,294,294,294,0,0,28,342,
342,342,342,342,342,342,342,342,342,0,0,294,294,294,294,294,294,294,294,294,294,
293,294,294,294,294,294,294,294,294,294,294,294,294,0,0,332,0,343,343,343,343,
343,343,343,343,344,344,0,0,0,0,0,0,0,0,0,0,0,334,294,294,294,294,294,294,345,0,
0,0,0,0,0,0,0,346,0,0,347,332,0,333,333,333,333,333,333,333,333,333,333,334,0,0,
0,0,0,0,345,0,0,0,334,0,0,0,0,346,0,335,347,0,0,0,0,0,0,0,336,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,334,0,0,0,0,0,0,335,0,0,0,0,0,0,0,0,336,322,322,322,322,322,322,322,
322,322,322,0,0,0,0,0,0,0,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,0,0,0,0,322,0,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,0,0,0,0,0,0,0,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,0,0,0,0,322,0,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,339,339,339,339,339,339,339,
339,339,339,0,0,0,0,0,0,0,0,0,0,0,340,93,0,0,0,0,332,93,333,333,333,333,333,333,
333,333,333,333,338,338,338,338,338,338,338,338,338,338,0,334,0,0,340,93,0,0,335,
0,0,93,91,0,0,0,0,336,91,0,339,339,339,339,339,339,339,339,339,339,0,0,0,334,0,0,
0,0,0,0,335,340,93,0,91,0,0,0,93,336,91,0,0,342,342,342,342,342,342,342,342,342,
342,0,0,0,0,0,0,0,0,0,0,340,93,93,0,0,0,0,93,93,332,0,343,343,343,343,343,343,
343,343,344,344,0,0,0,0,0,0,0,0,0,0,0,334,0,93,0,0,0,0,353,93,0,0,0,0,0,0,332,
354,344,344,344,344,344,344,344,344,344,344,0,0,0,0,0,334,0,0,0,0,0,334,353,0,0,
0,0,0,351,0,0,354,0,0,0,0,0,352,0,0,358,358,358,358,358,358,358,358,358,358,0,0,
0,334,0,0,0,0,0,0,351,340,93,0,0,0,0,0,93,352,348,348,348,348,348,348,348,348,
348,348,0,0,0,0,0,0,0,348,348,348,348,348,348,340,93,0,0,0,0,0,93,0,0,0,0,0,0,0,
348,348,348,348,348,348,348,348,348,348,0,348,348,348,348,348,348,348,348,348,
348,348,348,0,0,0,368,0,349,0,0,369,0,0,0,0,0,350,0,0,370,370,370,370,370,370,
370,370,0,348,348,348,348,348,348,371,0,0,0,0,349,0,0,0,0,0,0,0,0,350,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,372,0,0,0,0,373,374,0,0,0,375,0,0,0,0,0,0,0,376,0,0,0,377,0,
378,0,379,0,380,381,381,381,381,381,381,381,381,381,381,0,0,0,0,0,0,0,381,381,
381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,
381,381,381,381,0,0,0,0,0,0,381,381,381,381,381,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,381,381,381,381,381,381,382,0,0,0,0,0,0,0,0,381,381,
381,381,381,381,381,381,381,381,0,0,0,0,0,0,0,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,0,0,0,0,
0,0,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,22,0,0,395,0,0,0,394,394,394,394,394,394,394,394,394,
394,0,0,0,0,0,0,0,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,394,394,394,394,394,394,0,0,0,0,394,0,394,394,394,394,394,
394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,
394,397,0,0,0,0,0,0,0,396,396,396,396,396,396,396,396,396,396,0,0,0,0,0,0,0,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,396,0,0,0,0,396,0,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0};const int Cyc_lex_check[3481]=(const int[3481]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,0,25,29,0,122,124,131,133,288,291,41,41,45,45,41,398,45,- 1,- 1,- 1,- 1,- 1,120,
201,212,0,395,10,12,40,10,12,40,41,202,45,203,20,1,1,1,1,1,1,1,1,1,1,38,48,121,
213,10,38,226,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,129,217,218,
219,1,129,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,
2,9,18,51,52,53,54,55,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,56,57,
58,59,2,60,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,61,62,7,3,3,3,3,3,
7,7,63,64,67,68,69,7,30,30,30,30,30,30,30,30,70,71,3,3,3,74,75,3,3,3,47,47,3,3,
47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,76,3,3,3,47,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,10,12,40,3,3,81,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,4,3,16,4,5,5,6,8,5,6,8,82,83,50,84,16,50,85,27,86,17,27,50,17,4,87,94,5,95,5,
6,8,9,18,114,115,116,5,117,118,119,141,143,143,17,34,34,34,34,34,34,34,34,144,
145,146,27,27,27,27,27,27,27,27,65,65,147,148,65,72,72,77,77,72,149,77,150,151,
152,153,66,66,4,154,66,156,157,65,6,8,5,16,72,16,77,158,142,142,142,159,142,160,
17,66,142,27,161,7,73,73,162,163,73,142,164,165,166,11,11,11,11,11,11,11,11,11,
11,103,27,110,103,73,110,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,111,167,130,111,11,130,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,88,88,66,168,88,169,170,171,172,173,
96,96,104,104,96,174,104,105,105,175,177,105,178,88,179,13,13,80,80,13,73,80,16,
96,180,104,181,182,183,184,105,185,186,187,188,189,191,192,13,193,80,194,112,112,
4,195,112,196,197,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,112,198,199,200,
204,205,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,206,207,208,209,13,210,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,14,14,80,211,14,214,215,32,32,32,32,32,32,32,32,32,32,
92,92,220,221,92,222,14,32,32,32,32,32,32,125,125,125,125,125,125,125,125,14,223,
92,224,103,225,110,227,228,99,99,230,14,99,231,232,233,234,32,32,32,32,32,32,128,
128,128,128,128,128,128,128,99,235,236,237,238,111,216,130,15,15,216,239,15,101,
101,240,241,101,242,216,14,216,243,244,245,14,14,246,247,248,249,15,14,251,252,
256,101,134,134,134,134,134,134,134,134,257,258,15,15,15,15,15,15,15,15,15,15,
216,259,260,261,92,262,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,263,99,264,265,15,266,15,15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,268,269,15,270,271,101,274,33,33,33,
33,33,33,33,33,33,33,275,276,277,278,279,281,282,33,33,33,33,33,33,137,137,137,
137,137,137,137,137,283,284,300,302,305,309,312,42,42,42,42,42,42,42,42,42,42,14,
33,33,33,33,33,33,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,313,313,313,314,42,317,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,44,44,44,44,44,44,44,44,44,44,315,315,324,15,
327,357,335,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,366,368,369,371,44,335,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,78,78,303,306,78,304,253,253,372,373,253,292,292,
292,292,292,292,292,292,318,374,306,336,78,375,303,376,139,139,253,377,139,140,
140,304,307,140,253,378,78,78,78,78,78,78,78,78,78,78,379,139,307,307,345,336,
140,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,
78,318,387,397,345,78,304,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,
78,78,78,78,78,78,78,78,78,79,79,139,399,79,267,267,140,400,267,401,298,139,298,
298,402,403,140,346,349,350,351,140,79,404,405,255,255,267,- 1,255,273,273,- 1,298,
273,267,- 1,- 1,79,79,79,79,79,79,79,79,79,79,255,346,349,350,351,273,352,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,- 1,- 1,- 1,
353,79,352,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,89,89,255,353,89,272,272,273,- 1,272,289,289,255,- 1,289,- 1,- 1,273,354,- 1,
- 1,- 1,304,89,- 1,- 1,296,296,272,393,296,393,393,289,- 1,289,272,- 1,- 1,89,89,89,89,
89,89,89,89,89,89,296,354,296,393,- 1,- 1,- 1,89,89,89,89,89,89,89,89,89,89,89,89,
89,89,89,89,89,89,89,89,89,89,89,89,89,89,- 1,- 1,- 1,- 1,89,- 1,89,89,89,89,89,89,89,
89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,90,90,280,280,90,- 1,
280,285,285,- 1,- 1,285,295,295,295,295,295,295,295,295,- 1,- 1,- 1,90,- 1,280,- 1,- 1,-
1,- 1,285,- 1,- 1,280,- 1,- 1,- 1,- 1,285,90,90,90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,- 1,- 1,- 1,- 1,90,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,90,97,97,- 1,308,97,308,308,308,308,308,308,308,308,308,308,- 1,- 1,
- 1,- 1,334,- 1,334,- 1,97,334,334,334,334,334,334,334,334,334,334,- 1,- 1,- 1,- 1,- 1,97,
97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,97,
97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,97,- 1,97,97,97,97,97,97,
97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,98,98,- 1,- 1,98,
337,337,337,337,337,337,337,337,337,337,- 1,- 1,- 1,- 1,340,- 1,340,- 1,98,340,340,340,
340,340,340,340,340,340,340,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,- 1,- 1,-
1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,- 1,- 1,- 1,- 1,98,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,106,106,106,106,106,106,106,106,106,106,106,106,106,-
1,- 1,- 1,- 1,- 1,- 1,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,106,- 1,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,107,107,107,107,107,107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,- 1,- 1,107,
107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,
107,107,107,107,107,- 1,- 1,- 1,- 1,107,- 1,107,107,107,107,107,107,107,107,107,107,
107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,108,108,108,108,
108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,- 1,- 1,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
109,- 1,- 1,109,108,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,109,109,109,109,109,109,
109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,
- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,113,113,113,113,113,113,113,113,113,113,- 1,-
1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,123,- 1,- 1,123,113,- 1,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,- 1,- 1,132,- 1,- 1,132,123,123,123,123,123,123,123,123,126,126,126,126,126,126,
126,126,126,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,126,126,126,126,126,- 1,- 1,- 1,132,132,
132,132,132,132,132,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,- 1,- 1,109,- 1,370,- 1,- 1,126,126,
126,126,126,126,370,370,370,370,370,370,370,370,- 1,123,127,127,127,127,127,127,
127,127,127,127,132,- 1,- 1,- 1,- 1,- 1,- 1,127,127,127,127,127,127,135,135,135,135,
135,135,135,135,135,135,132,- 1,- 1,- 1,- 1,- 1,- 1,135,135,135,135,135,135,- 1,- 1,- 1,
127,127,127,127,127,127,290,- 1,- 1,290,- 1,- 1,136,136,136,136,136,136,136,136,136,
136,- 1,135,135,135,135,135,135,136,136,136,136,136,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
386,290,290,290,290,290,290,290,290,386,386,386,386,386,386,386,386,- 1,136,136,
136,136,136,136,- 1,293,293,293,293,293,293,293,293,293,293,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
293,293,293,293,293,293,- 1,- 1,290,341,341,341,341,341,341,341,341,341,341,- 1,- 1,
294,294,294,294,294,294,294,294,294,294,290,293,293,293,293,293,293,294,294,294,
294,294,294,- 1,- 1,310,- 1,310,310,310,310,310,310,310,310,310,310,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,310,294,294,294,294,294,294,310,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,310,- 1,
- 1,310,311,- 1,311,311,311,311,311,311,311,311,311,311,310,- 1,- 1,- 1,- 1,- 1,- 1,310,
- 1,- 1,- 1,311,- 1,- 1,- 1,- 1,310,- 1,311,310,- 1,- 1,- 1,- 1,- 1,- 1,- 1,311,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,311,- 1,- 1,- 1,- 1,- 1,- 1,311,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
311,316,316,316,316,316,316,316,316,316,316,- 1,- 1,- 1,- 1,- 1,- 1,- 1,316,316,316,316,
316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,
316,316,- 1,- 1,- 1,- 1,316,- 1,316,316,316,316,316,316,316,316,316,316,316,316,316,
316,316,316,316,316,316,316,316,316,316,316,316,316,322,322,322,322,322,322,322,
322,322,322,- 1,- 1,- 1,- 1,- 1,- 1,- 1,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,322,- 1,- 1,- 1,- 1,322,- 1,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,332,332,332,332,332,332,332,332,332,332,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,332,332,- 1,- 1,- 1,- 1,333,332,333,333,333,333,333,333,333,333,333,333,
338,338,338,338,338,338,338,338,338,338,- 1,333,- 1,- 1,332,332,- 1,- 1,333,- 1,- 1,332,
338,- 1,- 1,- 1,- 1,333,338,- 1,339,339,339,339,339,339,339,339,339,339,- 1,- 1,- 1,333,
- 1,- 1,- 1,- 1,- 1,- 1,333,339,339,- 1,338,- 1,- 1,- 1,339,333,338,- 1,- 1,342,342,342,342,
342,342,342,342,342,342,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,339,339,342,- 1,- 1,- 1,- 1,
339,342,343,- 1,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,343,- 1,342,- 1,- 1,- 1,- 1,343,342,- 1,- 1,- 1,- 1,- 1,- 1,344,343,344,344,344,344,
344,344,344,344,344,344,- 1,- 1,- 1,- 1,- 1,343,- 1,- 1,- 1,- 1,- 1,344,343,- 1,- 1,- 1,- 1,- 1,
344,- 1,- 1,343,- 1,- 1,- 1,- 1,- 1,344,- 1,- 1,358,358,358,358,358,358,358,358,358,358,-
1,- 1,- 1,344,- 1,- 1,- 1,- 1,- 1,- 1,344,358,358,- 1,- 1,- 1,- 1,- 1,358,344,347,347,347,347,
347,347,347,347,347,347,- 1,- 1,- 1,- 1,- 1,- 1,- 1,347,347,347,347,347,347,358,358,- 1,
- 1,- 1,- 1,- 1,358,- 1,- 1,- 1,- 1,- 1,- 1,- 1,348,348,348,348,348,348,348,348,348,348,- 1,
347,347,347,347,347,347,348,348,348,348,348,348,- 1,- 1,- 1,367,- 1,348,- 1,- 1,367,- 1,
- 1,- 1,- 1,- 1,348,- 1,- 1,367,367,367,367,367,367,367,367,- 1,348,348,348,348,348,348,
367,- 1,- 1,- 1,- 1,348,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,348,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,367,- 1,- 1,- 1,- 1,367,367,- 1,- 1,- 1,367,- 1,- 1,- 1,- 1,- 1,- 1,- 1,367,- 1,- 1,- 1,
367,- 1,367,- 1,367,- 1,367,380,380,380,380,380,380,380,380,380,380,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,
380,380,380,380,380,380,380,380,- 1,- 1,- 1,- 1,- 1,- 1,380,380,380,380,380,380,380,
380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,381,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,381,381,381,381,381,381,381,381,381,381,- 1,- 1,- 1,- 1,- 1,-
1,- 1,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,- 1,- 1,- 1,- 1,- 1,- 1,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,394,- 1,-
1,394,- 1,- 1,- 1,394,394,394,394,394,394,394,394,394,394,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,
394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,- 1,- 1,- 1,- 1,394,- 1,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,396,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,396,396,396,396,396,396,396,396,396,396,- 1,- 1,- 1,- 1,- 1,- 1,- 1,396,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,- 1,- 1,- 1,- 1,396,- 1,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};int Cyc_lex_engine(int
start_state,struct Cyc_Lexing_lexbuf*lbuf);int Cyc_lex_engine(int start_state,
struct Cyc_Lexing_lexbuf*lbuf){int state;int base;int backtrk;int c;state=start_state;
if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);lbuf->lex_last_action=
- 1;}else{state=(- state)- 1;}while(1){base=Cyc_lex_base[
_check_known_subscript_notnull(406,state)];if(base < 0)return(- base)- 1;backtrk=
Cyc_lex_backtrk[_check_known_subscript_notnull(406,state)];if(backtrk >= 0){lbuf->lex_last_pos=
lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}else{c=(int)*((char*)
_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}if(Cyc_lex_check[_check_known_subscript_notnull(3481,base + c)]
== state)state=Cyc_lex_trans[_check_known_subscript_notnull(3481,base + c)];else{
state=Cyc_lex_default[_check_known_subscript_notnull(406,state)];}if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;if(lbuf->lex_last_action == - 1){struct Cyc_Lexing_Error_struct
_tmp331;const char*_tmp330;struct Cyc_Lexing_Error_struct*_tmp32F;(int)_throw((
void*)((_tmp32F=_cycalloc(sizeof(*_tmp32F)),((_tmp32F[0]=((_tmp331.tag=Cyc_Lexing_Error,((
_tmp331.f1=((_tmp330="empty token",_tag_dyneither(_tmp330,sizeof(char),12))),
_tmp331)))),_tmp32F)))));}else{return lbuf->lex_last_action;}}else{if(c == 256)
lbuf->lex_eof_reached=0;}}}struct _tuple17*Cyc_line_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);struct _tuple17*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,
int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0:
_LL0: Cyc_macroname(lexbuf);for(0;Cyc_current_args != 0;Cyc_current_args=((struct
Cyc_List_List*)_check_null(Cyc_current_args))->tl){struct Cyc_Set_Set**_tmp332;
Cyc_current_targets=((_tmp332=_cycalloc(sizeof(*_tmp332)),((_tmp332[0]=((struct
Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_delete)(*((
struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(struct _dyneither_ptr*)((
struct Cyc_List_List*)_check_null(Cyc_current_args))->hd),_tmp332))));}{struct
_tuple17*_tmp333;return(_tmp333=_cycalloc(sizeof(*_tmp333)),((_tmp333->f1=(
struct _dyneither_ptr*)_check_null(Cyc_current_source),((_tmp333->f2=*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets)),_tmp333)))));}case 1: _LL1: return Cyc_line(lexbuf);
case 2: _LL2: return 0;default: _LL3:(lexbuf->refill_buff)(lexbuf);return Cyc_line_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp339;const char*_tmp338;struct
Cyc_Lexing_Error_struct*_tmp337;(int)_throw((void*)((_tmp337=_cycalloc(sizeof(*
_tmp337)),((_tmp337[0]=((_tmp339.tag=Cyc_Lexing_Error,((_tmp339.f1=((_tmp338="some action didn't return!",
_tag_dyneither(_tmp338,sizeof(char),27))),_tmp339)))),_tmp337)))));}}struct
_tuple17*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf);struct _tuple17*Cyc_line(struct
Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}int Cyc_macroname_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL5:{struct _dyneither_ptr*_tmp33A;Cyc_current_source=((_tmp33A=_cycalloc(
sizeof(*_tmp33A)),((_tmp33A[0]=(struct _dyneither_ptr)Cyc_substring((struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2)),_tmp33A))));}Cyc_current_args=0;{
struct Cyc_Set_Set**_tmp33B;Cyc_current_targets=((_tmp33B=_cycalloc(sizeof(*
_tmp33B)),((_tmp33B[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp33B))));}Cyc_token(
lexbuf);return 0;case 1: _LL6:{struct _dyneither_ptr*_tmp33C;Cyc_current_source=((
_tmp33C=_cycalloc(sizeof(*_tmp33C)),((_tmp33C[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp33C))));}Cyc_current_args=0;{
struct Cyc_Set_Set**_tmp33D;Cyc_current_targets=((_tmp33D=_cycalloc(sizeof(*
_tmp33D)),((_tmp33D[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp33D))));}Cyc_args(
lexbuf);return 0;case 2: _LL7:{struct _dyneither_ptr*_tmp33E;Cyc_current_source=((
_tmp33E=_cycalloc(sizeof(*_tmp33E)),((_tmp33E[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf),_tmp33E))));}Cyc_current_args=0;{struct Cyc_Set_Set**_tmp33F;Cyc_current_targets=((
_tmp33F=_cycalloc(sizeof(*_tmp33F)),((_tmp33F[0]=((struct Cyc_Set_Set*(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),
_tmp33F))));}Cyc_token(lexbuf);return 0;default: _LL8:(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp345;
const char*_tmp344;struct Cyc_Lexing_Error_struct*_tmp343;(int)_throw((void*)((
_tmp343=_cycalloc(sizeof(*_tmp343)),((_tmp343[0]=((_tmp345.tag=Cyc_Lexing_Error,((
_tmp345.f1=((_tmp344="some action didn't return!",_tag_dyneither(_tmp344,sizeof(
char),27))),_tmp345)))),_tmp343)))));}}int Cyc_macroname(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(
lexbuf,1);}int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_args_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLA: {struct _dyneither_ptr*_tmp346;struct
_dyneither_ptr*_tmp15=(_tmp346=_cycalloc(sizeof(*_tmp346)),((_tmp346[0]=(struct
_dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(
unsigned long)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 
2)),_tmp346)));{struct Cyc_List_List*_tmp347;Cyc_current_args=((_tmp347=_cycalloc(
sizeof(*_tmp347)),((_tmp347->hd=_tmp15,((_tmp347->tl=Cyc_current_args,_tmp347))))));}
return Cyc_args(lexbuf);}case 1: _LLB: {struct _dyneither_ptr*_tmp348;struct
_dyneither_ptr*_tmp18=(_tmp348=_cycalloc(sizeof(*_tmp348)),((_tmp348[0]=(struct
_dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(
unsigned long)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 
1)),_tmp348)));{struct Cyc_List_List*_tmp349;Cyc_current_args=((_tmp349=_cycalloc(
sizeof(*_tmp349)),((_tmp349->hd=_tmp18,((_tmp349->tl=Cyc_current_args,_tmp349))))));}
return Cyc_token(lexbuf);}default: _LLC:(lexbuf->refill_buff)(lexbuf);return Cyc_args_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp34F;const char*_tmp34E;struct
Cyc_Lexing_Error_struct*_tmp34D;(int)_throw((void*)((_tmp34D=_cycalloc(sizeof(*
_tmp34D)),((_tmp34D[0]=((_tmp34F.tag=Cyc_Lexing_Error,((_tmp34F.f1=((_tmp34E="some action didn't return!",
_tag_dyneither(_tmp34E,sizeof(char),27))),_tmp34F)))),_tmp34D)))));}}int Cyc_args(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_args_rec(lexbuf,2);}int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,
int lexstate);int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLE:{struct
_dyneither_ptr*_tmp350;Cyc_add_target(((_tmp350=_cycalloc(sizeof(*_tmp350)),((
_tmp350[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp350)))));}return
Cyc_token(lexbuf);case 1: _LLF: return 0;case 2: _LL10: return Cyc_token(lexbuf);case 3:
_LL11: Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4: _LL12: return Cyc_token(
lexbuf);case 5: _LL13: return Cyc_token(lexbuf);case 6: _LL14: return Cyc_token(lexbuf);
case 7: _LL15: return Cyc_token(lexbuf);case 8: _LL16: return Cyc_token(lexbuf);case 9:
_LL17: return Cyc_token(lexbuf);case 10: _LL18: return Cyc_token(lexbuf);case 11: _LL19:
return Cyc_token(lexbuf);case 12: _LL1A: return Cyc_token(lexbuf);case 13: _LL1B: return
Cyc_token(lexbuf);case 14: _LL1C: return Cyc_token(lexbuf);case 15: _LL1D: return Cyc_token(
lexbuf);case 16: _LL1E: return Cyc_token(lexbuf);case 17: _LL1F: return Cyc_token(lexbuf);
case 18: _LL20: return Cyc_token(lexbuf);case 19: _LL21: return Cyc_token(lexbuf);case 20:
_LL22: return Cyc_token(lexbuf);case 21: _LL23: return Cyc_token(lexbuf);case 22: _LL24:
return Cyc_token(lexbuf);case 23: _LL25: return Cyc_token(lexbuf);case 24: _LL26: return
Cyc_token(lexbuf);case 25: _LL27: return Cyc_token(lexbuf);case 26: _LL28: return Cyc_token(
lexbuf);case 27: _LL29: return Cyc_token(lexbuf);case 28: _LL2A: return Cyc_token(lexbuf);
case 29: _LL2B: return Cyc_token(lexbuf);case 30: _LL2C: return Cyc_token(lexbuf);case 31:
_LL2D: return Cyc_token(lexbuf);case 32: _LL2E: return Cyc_token(lexbuf);case 33: _LL2F:
return Cyc_token(lexbuf);case 34: _LL30: return Cyc_token(lexbuf);case 35: _LL31: return
Cyc_token(lexbuf);case 36: _LL32: return Cyc_token(lexbuf);case 37: _LL33: return Cyc_token(
lexbuf);case 38: _LL34: return Cyc_token(lexbuf);case 39: _LL35: return Cyc_token(lexbuf);
case 40: _LL36: return Cyc_token(lexbuf);case 41: _LL37: return Cyc_token(lexbuf);case 42:
_LL38: return Cyc_token(lexbuf);case 43: _LL39: return Cyc_token(lexbuf);case 44: _LL3A:
return Cyc_token(lexbuf);case 45: _LL3B: return Cyc_token(lexbuf);case 46: _LL3C: return
Cyc_token(lexbuf);case 47: _LL3D: return Cyc_token(lexbuf);case 48: _LL3E: return Cyc_token(
lexbuf);default: _LL3F:(lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(lexbuf,
lexstate);}{struct Cyc_Lexing_Error_struct _tmp356;const char*_tmp355;struct Cyc_Lexing_Error_struct*
_tmp354;(int)_throw((void*)((_tmp354=_cycalloc(sizeof(*_tmp354)),((_tmp354[0]=((
_tmp356.tag=Cyc_Lexing_Error,((_tmp356.f1=((_tmp355="some action didn't return!",
_tag_dyneither(_tmp355,sizeof(char),27))),_tmp356)))),_tmp354)))));}}int Cyc_token(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_token_rec(lexbuf,3);}int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,
int lexstate);int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL41: return Cyc_string(
lexbuf);case 1: _LL42: return 0;case 2: _LL43: return Cyc_string(lexbuf);case 3: _LL44:
return Cyc_string(lexbuf);case 4: _LL45: return Cyc_string(lexbuf);case 5: _LL46: return
Cyc_string(lexbuf);case 6: _LL47: return Cyc_string(lexbuf);case 7: _LL48: return 0;case
8: _LL49: return 0;case 9: _LL4A: return Cyc_string(lexbuf);default: _LL4B:(lexbuf->refill_buff)(
lexbuf);return Cyc_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp35C;const char*_tmp35B;struct Cyc_Lexing_Error_struct*_tmp35A;(int)_throw((
void*)((_tmp35A=_cycalloc(sizeof(*_tmp35A)),((_tmp35A[0]=((_tmp35C.tag=Cyc_Lexing_Error,((
_tmp35C.f1=((_tmp35B="some action didn't return!",_tag_dyneither(_tmp35B,sizeof(
char),27))),_tmp35C)))),_tmp35A)))));}}int Cyc_string(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(
lexbuf,4);}int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_slurp_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL4D: return 0;case 1: _LL4E: Cyc_fputc((int)'"',(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2: _LL4F: Cyc_fputs((const char*)"*__IGNORE_FOR_CYCLONE_MALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp35F;void*_tmp35E;(
_tmp35E=0,Cyc_log(((_tmp35F="Warning: declaration of malloc sidestepped\n",
_tag_dyneither(_tmp35F,sizeof(char),44))),_tag_dyneither(_tmp35E,sizeof(void*),0)));}
return 1;case 3: _LL50: Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_MALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp362;void*_tmp361;(
_tmp361=0,Cyc_log(((_tmp362="Warning: declaration of malloc sidestepped\n",
_tag_dyneither(_tmp362,sizeof(char),44))),_tag_dyneither(_tmp361,sizeof(void*),0)));}
return 1;case 4: _LL51: Cyc_fputs((const char*)"*__IGNORE_FOR_CYCLONE_CALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp365;void*_tmp364;(
_tmp364=0,Cyc_log(((_tmp365="Warning: declaration of calloc sidestepped\n",
_tag_dyneither(_tmp365,sizeof(char),44))),_tag_dyneither(_tmp364,sizeof(void*),0)));}
return 1;case 5: _LL52: Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_CALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp368;void*_tmp367;(
_tmp367=0,Cyc_log(((_tmp368="Warning: declaration of calloc sidestepped\n",
_tag_dyneither(_tmp368,sizeof(char),44))),_tag_dyneither(_tmp367,sizeof(void*),0)));}
return 1;case 6: _LL53: Cyc_fputs((const char*)"__region",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));{const char*_tmp36B;void*_tmp36A;(_tmp36A=0,Cyc_log(((
_tmp36B="Warning: use of region sidestepped\n",_tag_dyneither(_tmp36B,sizeof(
char),36))),_tag_dyneither(_tmp36A,sizeof(void*),0)));}return 1;case 7: _LL54:
return 1;case 8: _LL55: return 1;case 9: _LL56: return 1;case 10: _LL57: return 1;case 11:
_LL58: return 1;case 12: _LL59: return 1;case 13: _LL5A: Cyc_fputs((const char*)"inline",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 14: _LL5B: Cyc_fputs((
const char*)"inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 15: _LL5C: Cyc_fputs((const char*)"const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 16: _LL5D: Cyc_fputs((const char*)"const",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 17: _LL5E: Cyc_fputs((const char*)"int",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 18: _LL5F: return 1;
case 19: _LL60: Cyc_parens_to_match=1;while(Cyc_asm(lexbuf)){;}Cyc_fputs((const char*)"0",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp36E;void*_tmp36D;(
_tmp36D=0,Cyc_log(((_tmp36E="Warning: replacing use of __asm__ with 0\n",
_tag_dyneither(_tmp36E,sizeof(char),42))),_tag_dyneither(_tmp36D,sizeof(void*),0)));}
return 1;case 20: _LL61: Cyc_fputc((int)Cyc_Lexing_lexeme_char(lexbuf,0),(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;default: _LL62:(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp374;
const char*_tmp373;struct Cyc_Lexing_Error_struct*_tmp372;(int)_throw((void*)((
_tmp372=_cycalloc(sizeof(*_tmp372)),((_tmp372[0]=((_tmp374.tag=Cyc_Lexing_Error,((
_tmp374.f1=((_tmp373="some action didn't return!",_tag_dyneither(_tmp373,sizeof(
char),27))),_tmp374)))),_tmp372)))));}}int Cyc_slurp(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,
5);}int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_slurp_string_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL64: return 0;case 1: _LL65: Cyc_fputc((int)'"',(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2: _LL66:{const char*
_tmp377;void*_tmp376;(_tmp376=0,Cyc_log(((_tmp377="Warning: unclosed string\n",
_tag_dyneither(_tmp377,sizeof(char),26))),_tag_dyneither(_tmp376,sizeof(void*),0)));}{
const char*_tmp37B;void*_tmp37A[1];struct Cyc_String_pa_struct _tmp379;(_tmp379.tag=
0,((_tmp379.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf)),((_tmp37A[0]=& _tmp379,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp37B="%s",_tag_dyneither(_tmp37B,sizeof(char),3))),_tag_dyneither(_tmp37A,
sizeof(void*),1)))))));}return 1;case 3: _LL67:{const char*_tmp37F;void*_tmp37E[1];
struct Cyc_String_pa_struct _tmp37D;(_tmp37D.tag=0,((_tmp37D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp37E[0]=&
_tmp37D,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp37F="%s",
_tag_dyneither(_tmp37F,sizeof(char),3))),_tag_dyneither(_tmp37E,sizeof(void*),1)))))));}
return 1;case 4: _LL68:{const char*_tmp383;void*_tmp382[1];struct Cyc_String_pa_struct
_tmp381;(_tmp381.tag=0,((_tmp381.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Lexing_lexeme(lexbuf)),((_tmp382[0]=& _tmp381,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out),((_tmp383="%s",_tag_dyneither(_tmp383,sizeof(char),3))),
_tag_dyneither(_tmp382,sizeof(void*),1)))))));}return 1;case 5: _LL69:{const char*
_tmp387;void*_tmp386[1];struct Cyc_String_pa_struct _tmp385;(_tmp385.tag=0,((
_tmp385.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((
_tmp386[0]=& _tmp385,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp387="%s",_tag_dyneither(_tmp387,sizeof(char),3))),_tag_dyneither(_tmp386,
sizeof(void*),1)))))));}return 1;case 6: _LL6A:{const char*_tmp38B;void*_tmp38A[1];
struct Cyc_String_pa_struct _tmp389;(_tmp389.tag=0,((_tmp389.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp38A[0]=&
_tmp389,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp38B="%s",
_tag_dyneither(_tmp38B,sizeof(char),3))),_tag_dyneither(_tmp38A,sizeof(void*),1)))))));}
return 1;case 7: _LL6B:{const char*_tmp38F;void*_tmp38E[1];struct Cyc_String_pa_struct
_tmp38D;(_tmp38D.tag=0,((_tmp38D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Lexing_lexeme(lexbuf)),((_tmp38E[0]=& _tmp38D,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out),((_tmp38F="%s",_tag_dyneither(_tmp38F,sizeof(char),3))),
_tag_dyneither(_tmp38E,sizeof(void*),1)))))));}return 1;case 8: _LL6C:{const char*
_tmp393;void*_tmp392[1];struct Cyc_String_pa_struct _tmp391;(_tmp391.tag=0,((
_tmp391.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((
_tmp392[0]=& _tmp391,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp393="%s",_tag_dyneither(_tmp393,sizeof(char),3))),_tag_dyneither(_tmp392,
sizeof(void*),1)))))));}return 1;default: _LL6D:(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp399;const char*_tmp398;struct Cyc_Lexing_Error_struct*_tmp397;(int)_throw((
void*)((_tmp397=_cycalloc(sizeof(*_tmp397)),((_tmp397[0]=((_tmp399.tag=Cyc_Lexing_Error,((
_tmp399.f1=((_tmp398="some action didn't return!",_tag_dyneither(_tmp398,sizeof(
char),27))),_tmp399)))),_tmp397)))));}}int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(
lexbuf,6);}int Cyc_asm_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_asm_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL6F: return 0;case 1: _LL70: if(Cyc_parens_to_match
== 1)return 0;-- Cyc_parens_to_match;return 1;case 2: _LL71: ++ Cyc_parens_to_match;
return 1;case 3: _LL72: while(Cyc_asm_string(lexbuf)){;}return 1;case 4: _LL73: while(
Cyc_asm_comment(lexbuf)){;}return 1;case 5: _LL74: return 1;case 6: _LL75: return 1;
default: _LL76:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_rec(lexbuf,lexstate);}{
struct Cyc_Lexing_Error_struct _tmp39F;const char*_tmp39E;struct Cyc_Lexing_Error_struct*
_tmp39D;(int)_throw((void*)((_tmp39D=_cycalloc(sizeof(*_tmp39D)),((_tmp39D[0]=((
_tmp39F.tag=Cyc_Lexing_Error,((_tmp39F.f1=((_tmp39E="some action didn't return!",
_tag_dyneither(_tmp39E,sizeof(char),27))),_tmp39F)))),_tmp39D)))));}}int Cyc_asm(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_asm(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_asm_rec(lexbuf,7);}int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL78:{
const char*_tmp3A2;void*_tmp3A1;(_tmp3A1=0,Cyc_log(((_tmp3A2="Warning: unclosed string\n",
_tag_dyneither(_tmp3A2,sizeof(char),26))),_tag_dyneither(_tmp3A1,sizeof(void*),0)));}
return 0;case 1: _LL79: return 0;case 2: _LL7A:{const char*_tmp3A5;void*_tmp3A4;(_tmp3A4=
0,Cyc_log(((_tmp3A5="Warning: unclosed string\n",_tag_dyneither(_tmp3A5,sizeof(
char),26))),_tag_dyneither(_tmp3A4,sizeof(void*),0)));}return 1;case 3: _LL7B:
return 1;case 4: _LL7C: return 1;case 5: _LL7D: return 1;case 6: _LL7E: return 1;case 7: _LL7F:
return 1;case 8: _LL80: return 1;default: _LL81:(lexbuf->refill_buff)(lexbuf);return
Cyc_asm_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3AB;const
char*_tmp3AA;struct Cyc_Lexing_Error_struct*_tmp3A9;(int)_throw((void*)((_tmp3A9=
_cycalloc(sizeof(*_tmp3A9)),((_tmp3A9[0]=((_tmp3AB.tag=Cyc_Lexing_Error,((
_tmp3AB.f1=((_tmp3AA="some action didn't return!",_tag_dyneither(_tmp3AA,sizeof(
char),27))),_tmp3AB)))),_tmp3A9)))));}}int Cyc_asm_string(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(
lexbuf,8);}int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LL83:{const char*_tmp3AE;void*_tmp3AD;(
_tmp3AD=0,Cyc_log(((_tmp3AE="Warning: unclosed comment\n",_tag_dyneither(_tmp3AE,
sizeof(char),27))),_tag_dyneither(_tmp3AD,sizeof(void*),0)));}return 0;case 1:
_LL84: return 0;case 2: _LL85: return 1;default: _LL86:(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp3B4;const char*_tmp3B3;struct Cyc_Lexing_Error_struct*_tmp3B2;(int)_throw((
void*)((_tmp3B2=_cycalloc(sizeof(*_tmp3B2)),((_tmp3B2[0]=((_tmp3B4.tag=Cyc_Lexing_Error,((
_tmp3B4.f1=((_tmp3B3="some action didn't return!",_tag_dyneither(_tmp3B3,sizeof(
char),27))),_tmp3B4)))),_tmp3B2)))));}}int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(
lexbuf,9);}struct _tuple18*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate);struct _tuple18*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL88:{
const char*_tmp3B5;Cyc_current_line=((_tmp3B5="#define ",_tag_dyneither(_tmp3B5,
sizeof(char),9)));}Cyc_suck_macroname(lexbuf);{struct _tuple18*_tmp3B6;return(
_tmp3B6=_cycalloc(sizeof(*_tmp3B6)),((_tmp3B6->f1=Cyc_current_line,((_tmp3B6->f2=(
struct _dyneither_ptr*)_check_null(Cyc_current_source),_tmp3B6)))));}case 1: _LL89:
return Cyc_suck_line(lexbuf);case 2: _LL8A: return 0;default: _LL8B:(lexbuf->refill_buff)(
lexbuf);return Cyc_suck_line_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp3BC;const char*_tmp3BB;struct Cyc_Lexing_Error_struct*_tmp3BA;(int)_throw((
void*)((_tmp3BA=_cycalloc(sizeof(*_tmp3BA)),((_tmp3BA[0]=((_tmp3BC.tag=Cyc_Lexing_Error,((
_tmp3BC.f1=((_tmp3BB="some action didn't return!",_tag_dyneither(_tmp3BB,sizeof(
char),27))),_tmp3BC)))),_tmp3BA)))));}}struct _tuple18*Cyc_suck_line(struct Cyc_Lexing_lexbuf*
lexbuf);struct _tuple18*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(
lexbuf,10);}int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL8D:{struct
_dyneither_ptr*_tmp3BD;Cyc_current_source=((_tmp3BD=_cycalloc(sizeof(*_tmp3BD)),((
_tmp3BD[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp3BD))));}Cyc_current_line=(
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(
struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));
return Cyc_suck_restofline(lexbuf);default: _LL8E:(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp3C3;const char*_tmp3C2;struct Cyc_Lexing_Error_struct*_tmp3C1;(int)_throw((
void*)((_tmp3C1=_cycalloc(sizeof(*_tmp3C1)),((_tmp3C1[0]=((_tmp3C3.tag=Cyc_Lexing_Error,((
_tmp3C3.f1=((_tmp3C2="some action didn't return!",_tag_dyneither(_tmp3C2,sizeof(
char),27))),_tmp3C3)))),_tmp3C1)))));}}int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(
lexbuf,11);}int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate);int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL90: Cyc_current_line=(
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 0;default: _LL91:(lexbuf->refill_buff)(
lexbuf);return Cyc_suck_restofline_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp3C9;const char*_tmp3C8;struct Cyc_Lexing_Error_struct*_tmp3C7;(int)_throw((
void*)((_tmp3C7=_cycalloc(sizeof(*_tmp3C7)),((_tmp3C7[0]=((_tmp3C9.tag=Cyc_Lexing_Error,((
_tmp3C9.f1=((_tmp3C8="some action didn't return!",_tag_dyneither(_tmp3C8,sizeof(
char),27))),_tmp3C9)))),_tmp3C7)))));}}int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(
lexbuf,12);}struct _tuple20*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate);struct _tuple20*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL93: return Cyc_spec(
lexbuf);case 1: _LL94: Cyc_current_headerfile=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));Cyc_current_symbols=0;Cyc_current_omit_symbols=
0;Cyc_current_cstubs=0;Cyc_current_cycstubs=0;Cyc_current_hstubs=0;while(Cyc_commands(
lexbuf)){;}Cyc_current_hstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_current_hstubs);Cyc_current_cstubs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);Cyc_current_cycstubs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cycstubs);{
struct _tuple20*_tmp3CA;return(_tmp3CA=_cycalloc(sizeof(*_tmp3CA)),((_tmp3CA->f1=
Cyc_current_headerfile,((_tmp3CA->f2=Cyc_current_symbols,((_tmp3CA->f3=Cyc_current_omit_symbols,((
_tmp3CA->f4=Cyc_current_hstubs,((_tmp3CA->f5=Cyc_current_cstubs,((_tmp3CA->f6=
Cyc_current_cycstubs,_tmp3CA)))))))))))));}case 2: _LL95: return Cyc_spec(lexbuf);
case 3: _LL96: return 0;case 4: _LL97:{const char*_tmp3CE;void*_tmp3CD[1];struct Cyc_Int_pa_struct
_tmp3CC;(_tmp3CC.tag=1,((_tmp3CC.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)),((_tmp3CD[0]=& _tmp3CC,Cyc_fprintf(Cyc_stderr,((_tmp3CE="Error in .cys file: expected header file name, found '%c' instead\n",
_tag_dyneither(_tmp3CE,sizeof(char),67))),_tag_dyneither(_tmp3CD,sizeof(void*),1)))))));}
return 0;default: _LL98:(lexbuf->refill_buff)(lexbuf);return Cyc_spec_rec(lexbuf,
lexstate);}{struct Cyc_Lexing_Error_struct _tmp3D4;const char*_tmp3D3;struct Cyc_Lexing_Error_struct*
_tmp3D2;(int)_throw((void*)((_tmp3D2=_cycalloc(sizeof(*_tmp3D2)),((_tmp3D2[0]=((
_tmp3D4.tag=Cyc_Lexing_Error,((_tmp3D4.f1=((_tmp3D3="some action didn't return!",
_tag_dyneither(_tmp3D3,sizeof(char),27))),_tmp3D4)))),_tmp3D2)))));}}struct
_tuple20*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf);struct _tuple20*Cyc_spec(struct
Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}int Cyc_commands_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL9A: return 0;case 1: _LL9B: return 0;case 2: _LL9C: Cyc_snarfed_symbols=0;while(
Cyc_snarfsymbols(lexbuf)){;}Cyc_current_symbols=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3: _LL9D: Cyc_snarfed_symbols=0;while(Cyc_snarfsymbols(lexbuf)){;}Cyc_current_omit_symbols=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_snarfed_symbols,Cyc_current_omit_symbols);return 1;case 4: _LL9E: Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple19*_tmp3D5;struct _tuple19*x=(_tmp3D5=_cycalloc(sizeof(*_tmp3D5)),((
_tmp3D5->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0),((_tmp3D5->f2=(struct
_dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp3D5)))));{struct Cyc_List_List*_tmp3D6;Cyc_current_hstubs=((_tmp3D6=_cycalloc(
sizeof(*_tmp3D6)),((_tmp3D6->hd=x,((_tmp3D6->tl=Cyc_current_hstubs,_tmp3D6))))));}
return 1;}case 5: _LL9F: {struct _dyneither_ptr _tmp72=Cyc_Lexing_lexeme(lexbuf);{
const char*_tmp3D7;_dyneither_ptr_inplace_plus(& _tmp72,sizeof(char),(int)Cyc_strlen(((
_tmp3D7="hstub",_tag_dyneither(_tmp3D7,sizeof(char),6)))));}while(isspace((int)*((
char*)_check_dyneither_subscript(_tmp72,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& _tmp72,sizeof(char),1);}{struct _dyneither_ptr t=
_tmp72;while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp74=Cyc_substring((
struct _dyneither_ptr)_tmp72,0,(unsigned long)((t.curr - _tmp72.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp3D8;struct _tuple19*x=(_tmp3D8=
_cycalloc(sizeof(*_tmp3D8)),((_tmp3D8->f1=(struct _dyneither_ptr)_tmp74,((_tmp3D8->f2=(
struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp3D8)))));{struct Cyc_List_List*_tmp3D9;Cyc_current_hstubs=((_tmp3D9=_cycalloc(
sizeof(*_tmp3D9)),((_tmp3D9->hd=x,((_tmp3D9->tl=Cyc_current_hstubs,_tmp3D9))))));}
return 1;}}}}case 6: _LLA0: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp3DA;struct
_tuple19*x=(_tmp3DA=_cycalloc(sizeof(*_tmp3DA)),((_tmp3DA->f1=(struct
_dyneither_ptr)_tag_dyneither(0,0,0),((_tmp3DA->f2=(struct _dyneither_ptr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp3DA)))));{struct Cyc_List_List*
_tmp3DB;Cyc_current_cstubs=((_tmp3DB=_cycalloc(sizeof(*_tmp3DB)),((_tmp3DB->hd=x,((
_tmp3DB->tl=Cyc_current_cstubs,_tmp3DB))))));}return 1;}case 7: _LLA1: {struct
_dyneither_ptr _tmp79=Cyc_Lexing_lexeme(lexbuf);{const char*_tmp3DC;
_dyneither_ptr_inplace_plus(& _tmp79,sizeof(char),(int)Cyc_strlen(((_tmp3DC="cstub",
_tag_dyneither(_tmp3DC,sizeof(char),6)))));}while(isspace((int)*((char*)
_check_dyneither_subscript(_tmp79,sizeof(char),0)))){_dyneither_ptr_inplace_plus(&
_tmp79,sizeof(char),1);}{struct _dyneither_ptr t=_tmp79;while(!isspace((int)*((
char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp7B=Cyc_substring((
struct _dyneither_ptr)_tmp79,0,(unsigned long)((t.curr - _tmp79.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp3DD;struct _tuple19*x=(_tmp3DD=
_cycalloc(sizeof(*_tmp3DD)),((_tmp3DD->f1=(struct _dyneither_ptr)_tmp7B,((_tmp3DD->f2=(
struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp3DD)))));{struct Cyc_List_List*_tmp3DE;Cyc_current_cstubs=((_tmp3DE=_cycalloc(
sizeof(*_tmp3DE)),((_tmp3DE->hd=x,((_tmp3DE->tl=Cyc_current_cstubs,_tmp3DE))))));}
return 1;}}}}case 8: _LLA2: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp3DF;struct
_tuple19*x=(_tmp3DF=_cycalloc(sizeof(*_tmp3DF)),((_tmp3DF->f1=(struct
_dyneither_ptr)_tag_dyneither(0,0,0),((_tmp3DF->f2=(struct _dyneither_ptr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp3DF)))));{struct Cyc_List_List*
_tmp3E0;Cyc_current_cycstubs=((_tmp3E0=_cycalloc(sizeof(*_tmp3E0)),((_tmp3E0->hd=
x,((_tmp3E0->tl=Cyc_current_cycstubs,_tmp3E0))))));}return 1;}case 9: _LLA3: {
struct _dyneither_ptr _tmp80=Cyc_Lexing_lexeme(lexbuf);{const char*_tmp3E1;
_dyneither_ptr_inplace_plus(& _tmp80,sizeof(char),(int)Cyc_strlen(((_tmp3E1="cycstub",
_tag_dyneither(_tmp3E1,sizeof(char),8)))));}while(isspace((int)*((char*)
_check_dyneither_subscript(_tmp80,sizeof(char),0)))){_dyneither_ptr_inplace_plus(&
_tmp80,sizeof(char),1);}{struct _dyneither_ptr t=_tmp80;while(!isspace((int)*((
char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp82=Cyc_substring((
struct _dyneither_ptr)_tmp80,0,(unsigned long)((t.curr - _tmp80.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp3E2;struct _tuple19*x=(_tmp3E2=
_cycalloc(sizeof(*_tmp3E2)),((_tmp3E2->f1=(struct _dyneither_ptr)_tmp82,((_tmp3E2->f2=(
struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp3E2)))));{struct Cyc_List_List*_tmp3E3;Cyc_current_cycstubs=((_tmp3E3=
_cycalloc(sizeof(*_tmp3E3)),((_tmp3E3->hd=x,((_tmp3E3->tl=Cyc_current_cycstubs,
_tmp3E3))))));}return 1;}}}}case 10: _LLA4: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_dyneither_ptr*_tmp3E4;struct _dyneither_ptr*x=(_tmp3E4=_cycalloc(sizeof(*_tmp3E4)),((
_tmp3E4[0]=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf)),_tmp3E4)));{struct Cyc_List_List*_tmp3E5;Cyc_current_cpp=((
_tmp3E5=_cycalloc(sizeof(*_tmp3E5)),((_tmp3E5->hd=x,((_tmp3E5->tl=Cyc_current_cpp,
_tmp3E5))))));}return 1;}case 11: _LLA5: return 1;case 12: _LLA6: return 1;case 13: _LLA7:{
const char*_tmp3E9;void*_tmp3E8[1];struct Cyc_Int_pa_struct _tmp3E7;(_tmp3E7.tag=1,((
_tmp3E7.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp3E8[0]=&
_tmp3E7,Cyc_fprintf(Cyc_stderr,((_tmp3E9="Error in .cys file: expected command, found '%c' instead\n",
_tag_dyneither(_tmp3E9,sizeof(char),58))),_tag_dyneither(_tmp3E8,sizeof(void*),1)))))));}
return 0;default: _LLA8:(lexbuf->refill_buff)(lexbuf);return Cyc_commands_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3EF;const char*_tmp3EE;struct
Cyc_Lexing_Error_struct*_tmp3ED;(int)_throw((void*)((_tmp3ED=_cycalloc(sizeof(*
_tmp3ED)),((_tmp3ED[0]=((_tmp3EF.tag=Cyc_Lexing_Error,((_tmp3EF.f1=((_tmp3EE="some action didn't return!",
_tag_dyneither(_tmp3EE,sizeof(char),27))),_tmp3EF)))),_tmp3ED)))));}}int Cyc_commands(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_commands_rec(lexbuf,14);}int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLAA:{
struct _dyneither_ptr*_tmp3F2;struct Cyc_List_List*_tmp3F1;Cyc_snarfed_symbols=((
_tmp3F1=_cycalloc(sizeof(*_tmp3F1)),((_tmp3F1->hd=((_tmp3F2=_cycalloc(sizeof(*
_tmp3F2)),((_tmp3F2[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp3F2)))),((
_tmp3F1->tl=Cyc_snarfed_symbols,_tmp3F1))))));}return 1;case 1: _LLAB: return 1;case 2:
_LLAC: return 0;case 3: _LLAD:{const char*_tmp3F5;void*_tmp3F4;(_tmp3F4=0,Cyc_fprintf(
Cyc_stderr,((_tmp3F5="Error in .cys file: unexpected end-of-file\n",
_tag_dyneither(_tmp3F5,sizeof(char),44))),_tag_dyneither(_tmp3F4,sizeof(void*),0)));}
return 0;case 4: _LLAE:{const char*_tmp3F9;void*_tmp3F8[1];struct Cyc_Int_pa_struct
_tmp3F7;(_tmp3F7.tag=1,((_tmp3F7.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)),((_tmp3F8[0]=& _tmp3F7,Cyc_fprintf(Cyc_stderr,((_tmp3F9="Error in .cys file: expected symbol, found '%c' instead\n",
_tag_dyneither(_tmp3F9,sizeof(char),57))),_tag_dyneither(_tmp3F8,sizeof(void*),1)))))));}
return 0;default: _LLAF:(lexbuf->refill_buff)(lexbuf);return Cyc_snarfsymbols_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3FF;const char*_tmp3FE;struct
Cyc_Lexing_Error_struct*_tmp3FD;(int)_throw((void*)((_tmp3FD=_cycalloc(sizeof(*
_tmp3FD)),((_tmp3FD[0]=((_tmp3FF.tag=Cyc_Lexing_Error,((_tmp3FF.f1=((_tmp3FE="some action didn't return!",
_tag_dyneither(_tmp3FE,sizeof(char),27))),_tmp3FF)))),_tmp3FD)))));}}int Cyc_snarfsymbols(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}int Cyc_block_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLB1:{const char*
_tmp402;void*_tmp401;(_tmp401=0,Cyc_log(((_tmp402="Warning: unclosed brace\n",
_tag_dyneither(_tmp402,sizeof(char),25))),_tag_dyneither(_tmp401,sizeof(void*),0)));}
return 0;case 1: _LLB2: if(Cyc_braces_to_match == 1)return 0;-- Cyc_braces_to_match;Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');return 1;case 2: _LLB3: ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');return 1;
case 3: _LLB4: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}return 1;case 4: _LLB5:{const char*_tmp403;Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),((_tmp403="/*",_tag_dyneither(
_tmp403,sizeof(char),3))));}while(Cyc_block_comment(lexbuf)){;}return 1;case 5:
_LLB6: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 6: _LLB7: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;default: _LLB8:(lexbuf->refill_buff)(lexbuf);return Cyc_block_rec(lexbuf,
lexstate);}{struct Cyc_Lexing_Error_struct _tmp409;const char*_tmp408;struct Cyc_Lexing_Error_struct*
_tmp407;(int)_throw((void*)((_tmp407=_cycalloc(sizeof(*_tmp407)),((_tmp407[0]=((
_tmp409.tag=Cyc_Lexing_Error,((_tmp409.f1=((_tmp408="some action didn't return!",
_tag_dyneither(_tmp408,sizeof(char),27))),_tmp409)))),_tmp407)))));}}int Cyc_block(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_block_rec(lexbuf,16);}int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLBA:{
const char*_tmp40C;void*_tmp40B;(_tmp40B=0,Cyc_log(((_tmp40C="Warning: unclosed string\n",
_tag_dyneither(_tmp40C,sizeof(char),26))),_tag_dyneither(_tmp40B,sizeof(void*),0)));}
return 0;case 1: _LLBB: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
return 0;case 2: _LLBC:{const char*_tmp40F;void*_tmp40E;(_tmp40E=0,Cyc_log(((_tmp40F="Warning: unclosed string\n",
_tag_dyneither(_tmp40F,sizeof(char),26))),_tag_dyneither(_tmp40E,sizeof(void*),0)));}
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 3: _LLBD: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 4: _LLBE: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return
1;case 5: _LLBF: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 6: _LLC0: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 7: _LLC1: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return
1;case 8: _LLC2: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;default: _LLC3:(lexbuf->refill_buff)(
lexbuf);return Cyc_block_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp415;const char*_tmp414;struct Cyc_Lexing_Error_struct*_tmp413;(int)_throw((
void*)((_tmp413=_cycalloc(sizeof(*_tmp413)),((_tmp413[0]=((_tmp415.tag=Cyc_Lexing_Error,((
_tmp415.f1=((_tmp414="some action didn't return!",_tag_dyneither(_tmp414,sizeof(
char),27))),_tmp415)))),_tmp413)))));}}int Cyc_block_string(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(
lexbuf,17);}int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLC5:{const char*_tmp418;
void*_tmp417;(_tmp417=0,Cyc_log(((_tmp418="Warning: unclosed comment\n",
_tag_dyneither(_tmp418,sizeof(char),27))),_tag_dyneither(_tmp417,sizeof(void*),0)));}
return 0;case 1: _LLC6:{const char*_tmp419;Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),((_tmp419="*/",_tag_dyneither(_tmp419,sizeof(char),3))));}
return 0;case 2: _LLC7: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;default: _LLC8:(lexbuf->refill_buff)(
lexbuf);return Cyc_block_comment_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp41F;const char*_tmp41E;struct Cyc_Lexing_Error_struct*_tmp41D;(int)_throw((
void*)((_tmp41D=_cycalloc(sizeof(*_tmp41D)),((_tmp41D[0]=((_tmp41F.tag=Cyc_Lexing_Error,((
_tmp41F.f1=((_tmp41E="some action didn't return!",_tag_dyneither(_tmp41E,sizeof(
char),27))),_tmp41F)))),_tmp41D)))));}}int Cyc_block_comment(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(
lexbuf,18);}void Cyc_scan_type(void*t);struct _tuple21{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};void Cyc_scan_exp(struct Cyc_Absyn_Exp*e);void Cyc_scan_exp(
struct Cyc_Absyn_Exp*e){void*_tmpAA=((struct Cyc_Absyn_Exp*)_check_null(e))->r;
struct _tuple0*_tmpAB;struct _tuple0 _tmpAC;struct _dyneither_ptr*_tmpAD;struct
_tuple0*_tmpAE;struct _tuple0 _tmpAF;struct _dyneither_ptr*_tmpB0;struct Cyc_List_List*
_tmpB1;struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_Absyn_Exp*_tmpB3;struct Cyc_Absyn_Exp*
_tmpB4;struct Cyc_Absyn_Exp*_tmpB5;struct Cyc_Absyn_Exp*_tmpB6;struct Cyc_Absyn_Exp*
_tmpB7;struct Cyc_Absyn_Exp*_tmpB8;struct Cyc_Absyn_Exp*_tmpB9;struct Cyc_Absyn_Exp*
_tmpBA;struct Cyc_Absyn_Exp*_tmpBB;struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_Absyn_Exp*
_tmpBD;struct Cyc_Absyn_Exp*_tmpBE;struct Cyc_Absyn_Exp*_tmpBF;struct Cyc_Absyn_Exp*
_tmpC0;struct Cyc_Absyn_Exp*_tmpC1;struct Cyc_Absyn_Exp*_tmpC2;struct Cyc_Absyn_Exp*
_tmpC3;struct Cyc_List_List*_tmpC4;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_List_List*
_tmpC6;void*_tmpC7;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_MallocInfo _tmpC9;
int _tmpCA;struct Cyc_Absyn_Exp*_tmpCB;void**_tmpCC;struct Cyc_Absyn_Exp*_tmpCD;
struct Cyc_Absyn_Exp*_tmpCE;void*_tmpCF;void*_tmpD0;struct Cyc_Absyn_Exp*_tmpD1;
struct _dyneither_ptr*_tmpD2;struct Cyc_Absyn_Exp*_tmpD3;struct _dyneither_ptr*
_tmpD4;void*_tmpD5;void*_tmpD6;struct Cyc_List_List*_tmpD7;_LLCB: if(*((int*)
_tmpAA)!= 1)goto _LLCD;_tmpAB=((struct Cyc_Absyn_Var_e_struct*)_tmpAA)->f1;_tmpAC=*
_tmpAB;_tmpAD=_tmpAC.f2;_LLCC: _tmpB0=_tmpAD;goto _LLCE;_LLCD: if(*((int*)_tmpAA)!= 
2)goto _LLCF;_tmpAE=((struct Cyc_Absyn_UnknownId_e_struct*)_tmpAA)->f1;_tmpAF=*
_tmpAE;_tmpB0=_tmpAF.f2;_LLCE: Cyc_add_target(_tmpB0);return;_LLCF: if(*((int*)
_tmpAA)!= 3)goto _LLD1;_tmpB1=((struct Cyc_Absyn_Primop_e_struct*)_tmpAA)->f2;
_LLD0: for(0;_tmpB1 != 0;_tmpB1=_tmpB1->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_tmpB1->hd));}return;_LLD1: if(*((int*)_tmpAA)!= 25)goto
_LLD3;_tmpB2=((struct Cyc_Absyn_Subscript_e_struct*)_tmpAA)->f1;_tmpB3=((struct
Cyc_Absyn_Subscript_e_struct*)_tmpAA)->f2;_LLD2: _tmpB4=_tmpB2;_tmpB5=_tmpB3;goto
_LLD4;_LLD3: if(*((int*)_tmpAA)!= 9)goto _LLD5;_tmpB4=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpAA)->f1;_tmpB5=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpAA)->f2;_LLD4: _tmpB6=
_tmpB4;_tmpB7=_tmpB5;goto _LLD6;_LLD5: if(*((int*)_tmpAA)!= 4)goto _LLD7;_tmpB6=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpAA)->f1;_tmpB7=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpAA)->f3;_LLD6: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpB6);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpB7);return;_LLD7: if(*((int*)_tmpAA)!= 22)goto _LLD9;
_tmpB8=((struct Cyc_Absyn_Deref_e_struct*)_tmpAA)->f1;_LLD8: _tmpB9=_tmpB8;goto
_LLDA;_LLD9: if(*((int*)_tmpAA)!= 19)goto _LLDB;_tmpB9=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmpAA)->f1;_LLDA: _tmpBA=_tmpB9;goto _LLDC;_LLDB: if(*((int*)_tmpAA)!= 16)goto
_LLDD;_tmpBA=((struct Cyc_Absyn_Address_e_struct*)_tmpAA)->f1;_LLDC: _tmpBB=_tmpBA;
goto _LLDE;_LLDD: if(*((int*)_tmpAA)!= 5)goto _LLDF;_tmpBB=((struct Cyc_Absyn_Increment_e_struct*)
_tmpAA)->f1;_LLDE: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpBB);return;_LLDF: if(*((
int*)_tmpAA)!= 6)goto _LLE1;_tmpBC=((struct Cyc_Absyn_Conditional_e_struct*)_tmpAA)->f1;
_tmpBD=((struct Cyc_Absyn_Conditional_e_struct*)_tmpAA)->f2;_tmpBE=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpAA)->f3;_LLE0: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpBC);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpBD);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpBE);return;
_LLE1: if(*((int*)_tmpAA)!= 7)goto _LLE3;_tmpBF=((struct Cyc_Absyn_And_e_struct*)
_tmpAA)->f1;_tmpC0=((struct Cyc_Absyn_And_e_struct*)_tmpAA)->f2;_LLE2: _tmpC1=
_tmpBF;_tmpC2=_tmpC0;goto _LLE4;_LLE3: if(*((int*)_tmpAA)!= 8)goto _LLE5;_tmpC1=((
struct Cyc_Absyn_Or_e_struct*)_tmpAA)->f1;_tmpC2=((struct Cyc_Absyn_Or_e_struct*)
_tmpAA)->f2;_LLE4: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpC1);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpC2);return;_LLE5: if(*((int*)_tmpAA)!= 11)goto _LLE7;
_tmpC3=((struct Cyc_Absyn_FnCall_e_struct*)_tmpAA)->f1;_tmpC4=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpAA)->f2;_LLE6: _tmpC5=_tmpC3;_tmpC6=_tmpC4;goto _LLE8;_LLE7: if(*((int*)_tmpAA)
!= 10)goto _LLE9;_tmpC5=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmpAA)->f1;
_tmpC6=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmpAA)->f2;_LLE8: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpC5);for(0;_tmpC6 != 0;_tmpC6=_tmpC6->tl){Cyc_scan_exp((
struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)_tmpC6->hd));}return;_LLE9: if(*((int*)
_tmpAA)!= 15)goto _LLEB;_tmpC7=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpAA)->f1;
_tmpC8=((struct Cyc_Absyn_Cast_e_struct*)_tmpAA)->f2;_LLEA: Cyc_scan_type(_tmpC7);
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpC8);return;_LLEB: if(*((int*)_tmpAA)!= 35)
goto _LLED;_tmpC9=((struct Cyc_Absyn_Malloc_e_struct*)_tmpAA)->f1;_tmpCA=_tmpC9.is_calloc;
_tmpCB=_tmpC9.rgn;_tmpCC=_tmpC9.elt_type;_tmpCD=_tmpC9.num_elts;_LLEC: if(_tmpCB
!= 0)Cyc_scan_exp((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)_tmpCB));if(
_tmpCC != 0)Cyc_scan_type(*_tmpCC);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpCD);
return;_LLED: if(*((int*)_tmpAA)!= 39)goto _LLEF;_tmpCE=((struct Cyc_Absyn_Tagcheck_e_struct*)
_tmpAA)->f1;_LLEE: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpCE);return;_LLEF: if(*((
int*)_tmpAA)!= 40)goto _LLF1;_tmpCF=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmpAA)->f1;_LLF0: _tmpD0=_tmpCF;goto _LLF2;_LLF1: if(*((int*)_tmpAA)!= 18)goto
_LLF3;_tmpD0=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpAA)->f1;_LLF2: Cyc_scan_type(
_tmpD0);return;_LLF3: if(*((int*)_tmpAA)!= 23)goto _LLF5;_tmpD1=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpAA)->f1;_tmpD2=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpAA)->f2;_LLF4:
_tmpD3=_tmpD1;_tmpD4=_tmpD2;goto _LLF6;_LLF5: if(*((int*)_tmpAA)!= 24)goto _LLF7;
_tmpD3=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpAA)->f1;_tmpD4=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpAA)->f2;_LLF6: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpD3);Cyc_add_target(
_tmpD4);return;_LLF7: if(*((int*)_tmpAA)!= 20)goto _LLF9;_tmpD5=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmpAA)->f1;_tmpD6=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpAA)->f2;_LLF8:
Cyc_scan_type(_tmpD5);{void*_tmpD8=_tmpD6;struct _dyneither_ptr*_tmpD9;_LL11E: if(*((
int*)_tmpD8)!= 0)goto _LL120;_tmpD9=((struct Cyc_Absyn_StructField_struct*)_tmpD8)->f1;
_LL11F: Cyc_add_target(_tmpD9);goto _LL11D;_LL120: if(*((int*)_tmpD8)!= 1)goto
_LL11D;_LL121: goto _LL11D;_LL11D:;}return;_LLF9: if(*((int*)_tmpAA)!= 0)goto _LLFB;
_LLFA: return;_LLFB: if(*((int*)_tmpAA)!= 37)goto _LLFD;_tmpD7=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpAA)->f2;_LLFC: for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){struct _tuple21 _tmpDB;
struct Cyc_Absyn_Exp*_tmpDC;struct _tuple21*_tmpDA=(struct _tuple21*)_tmpD7->hd;
_tmpDB=*_tmpDA;_tmpDC=_tmpDB.f2;Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpDC);}
return;_LLFD: if(*((int*)_tmpAA)!= 36)goto _LLFF;_LLFE:{const char*_tmp422;void*
_tmp421;(_tmp421=0,Cyc_fprintf(Cyc_stderr,((_tmp422="Error: unexpected Swap_e\n",
_tag_dyneither(_tmp422,sizeof(char),26))),_tag_dyneither(_tmp421,sizeof(void*),0)));}
exit(1);return;_LLFF: if(*((int*)_tmpAA)!= 38)goto _LL101;_LL100:{const char*
_tmp425;void*_tmp424;(_tmp424=0,Cyc_fprintf(Cyc_stderr,((_tmp425="Error: unexpected Stmt_e\n",
_tag_dyneither(_tmp425,sizeof(char),26))),_tag_dyneither(_tmp424,sizeof(void*),0)));}
exit(1);return;_LL101: if(*((int*)_tmpAA)!= 12)goto _LL103;_LL102:{const char*
_tmp428;void*_tmp427;(_tmp427=0,Cyc_fprintf(Cyc_stderr,((_tmp428="Error: unexpected Throw_e\n",
_tag_dyneither(_tmp428,sizeof(char),27))),_tag_dyneither(_tmp427,sizeof(void*),0)));}
exit(1);return;_LL103: if(*((int*)_tmpAA)!= 13)goto _LL105;_LL104:{const char*
_tmp42B;void*_tmp42A;(_tmp42A=0,Cyc_fprintf(Cyc_stderr,((_tmp42B="Error: unexpected NoInstantiate_e\n",
_tag_dyneither(_tmp42B,sizeof(char),35))),_tag_dyneither(_tmp42A,sizeof(void*),0)));}
exit(1);return;_LL105: if(*((int*)_tmpAA)!= 14)goto _LL107;_LL106:{const char*
_tmp42E;void*_tmp42D;(_tmp42D=0,Cyc_fprintf(Cyc_stderr,((_tmp42E="Error: unexpected Instantiate_e\n",
_tag_dyneither(_tmp42E,sizeof(char),33))),_tag_dyneither(_tmp42D,sizeof(void*),0)));}
exit(1);return;_LL107: if(*((int*)_tmpAA)!= 17)goto _LL109;_LL108:{const char*
_tmp431;void*_tmp430;(_tmp430=0,Cyc_fprintf(Cyc_stderr,((_tmp431="Error: unexpected New_e\n",
_tag_dyneither(_tmp431,sizeof(char),25))),_tag_dyneither(_tmp430,sizeof(void*),0)));}
exit(1);return;_LL109: if(*((int*)_tmpAA)!= 21)goto _LL10B;_LL10A:{const char*
_tmp434;void*_tmp433;(_tmp433=0,Cyc_fprintf(Cyc_stderr,((_tmp434="Error: unexpected Gentyp_e\n",
_tag_dyneither(_tmp434,sizeof(char),28))),_tag_dyneither(_tmp433,sizeof(void*),0)));}
exit(1);return;_LL10B: if(*((int*)_tmpAA)!= 26)goto _LL10D;_LL10C:{const char*
_tmp437;void*_tmp436;(_tmp436=0,Cyc_fprintf(Cyc_stderr,((_tmp437="Error: unexpected Tuple_e\n",
_tag_dyneither(_tmp437,sizeof(char),27))),_tag_dyneither(_tmp436,sizeof(void*),0)));}
exit(1);return;_LL10D: if(*((int*)_tmpAA)!= 27)goto _LL10F;_LL10E:{const char*
_tmp43A;void*_tmp439;(_tmp439=0,Cyc_fprintf(Cyc_stderr,((_tmp43A="Error: unexpected CompoundLit_e\n",
_tag_dyneither(_tmp43A,sizeof(char),33))),_tag_dyneither(_tmp439,sizeof(void*),0)));}
exit(1);return;_LL10F: if(*((int*)_tmpAA)!= 28)goto _LL111;_LL110:{const char*
_tmp43D;void*_tmp43C;(_tmp43C=0,Cyc_fprintf(Cyc_stderr,((_tmp43D="Error: unexpected Array_e\n",
_tag_dyneither(_tmp43D,sizeof(char),27))),_tag_dyneither(_tmp43C,sizeof(void*),0)));}
exit(1);return;_LL111: if(*((int*)_tmpAA)!= 29)goto _LL113;_LL112:{const char*
_tmp440;void*_tmp43F;(_tmp43F=0,Cyc_fprintf(Cyc_stderr,((_tmp440="Error: unexpected Comprehension_e\n",
_tag_dyneither(_tmp440,sizeof(char),35))),_tag_dyneither(_tmp43F,sizeof(void*),0)));}
exit(1);return;_LL113: if(*((int*)_tmpAA)!= 30)goto _LL115;_LL114:{const char*
_tmp443;void*_tmp442;(_tmp442=0,Cyc_fprintf(Cyc_stderr,((_tmp443="Error: unexpected Aggregate_e\n",
_tag_dyneither(_tmp443,sizeof(char),31))),_tag_dyneither(_tmp442,sizeof(void*),0)));}
exit(1);return;_LL115: if(*((int*)_tmpAA)!= 31)goto _LL117;_LL116:{const char*
_tmp446;void*_tmp445;(_tmp445=0,Cyc_fprintf(Cyc_stderr,((_tmp446="Error: unexpected AnonStruct_e\n",
_tag_dyneither(_tmp446,sizeof(char),32))),_tag_dyneither(_tmp445,sizeof(void*),0)));}
exit(1);return;_LL117: if(*((int*)_tmpAA)!= 32)goto _LL119;_LL118:{const char*
_tmp449;void*_tmp448;(_tmp448=0,Cyc_fprintf(Cyc_stderr,((_tmp449="Error: unexpected Datatype_e\n",
_tag_dyneither(_tmp449,sizeof(char),30))),_tag_dyneither(_tmp448,sizeof(void*),0)));}
exit(1);return;_LL119: if(*((int*)_tmpAA)!= 33)goto _LL11B;_LL11A:{const char*
_tmp44C;void*_tmp44B;(_tmp44B=0,Cyc_fprintf(Cyc_stderr,((_tmp44C="Error: unexpected Enum_e\n",
_tag_dyneither(_tmp44C,sizeof(char),26))),_tag_dyneither(_tmp44B,sizeof(void*),0)));}
exit(1);return;_LL11B: if(*((int*)_tmpAA)!= 34)goto _LLCA;_LL11C:{const char*
_tmp44F;void*_tmp44E;(_tmp44E=0,Cyc_fprintf(Cyc_stderr,((_tmp44F="Error: unexpected AnonEnum_e\n",
_tag_dyneither(_tmp44F,sizeof(char),30))),_tag_dyneither(_tmp44E,sizeof(void*),0)));}
exit(1);return;_LLCA:;}void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo);void Cyc_scan_exp_opt(
struct Cyc_Absyn_Exp*eo){if((unsigned int)eo)Cyc_scan_exp(eo);return;}void Cyc_scan_type(
void*t);void Cyc_scan_type(void*t){void*_tmpFD=t;struct Cyc_Absyn_PtrInfo _tmpFE;
struct Cyc_Absyn_ArrayInfo _tmpFF;void*_tmp100;struct Cyc_Absyn_Exp*_tmp101;union
Cyc_Absyn_Constraint*_tmp102;struct Cyc_Absyn_FnInfo _tmp103;struct Cyc_List_List*
_tmp104;struct Cyc_Absyn_AggrInfo _tmp105;union Cyc_Absyn_AggrInfoU _tmp106;struct
_tuple0*_tmp107;struct _tuple0 _tmp108;struct _dyneither_ptr*_tmp109;struct _tuple0*
_tmp10A;struct _tuple0 _tmp10B;struct _dyneither_ptr*_tmp10C;_LL123: if((int)_tmpFD
!= 0)goto _LL125;_LL124: goto _LL126;_LL125: if(_tmpFD <= (void*)4)goto _LL127;if(*((
int*)_tmpFD)!= 5)goto _LL127;_LL126: goto _LL128;_LL127: if((int)_tmpFD != 1)goto
_LL129;_LL128: goto _LL12A;_LL129: if(_tmpFD <= (void*)4)goto _LL149;if(*((int*)
_tmpFD)!= 6)goto _LL12B;_LL12A: return;_LL12B: if(*((int*)_tmpFD)!= 4)goto _LL12D;
_tmpFE=((struct Cyc_Absyn_PointerType_struct*)_tmpFD)->f1;_LL12C: Cyc_scan_type(
_tmpFE.elt_typ);return;_LL12D: if(*((int*)_tmpFD)!= 7)goto _LL12F;_tmpFF=((struct
Cyc_Absyn_ArrayType_struct*)_tmpFD)->f1;_tmp100=_tmpFF.elt_type;_tmp101=_tmpFF.num_elts;
_tmp102=_tmpFF.zero_term;_LL12E: Cyc_scan_type(_tmp100);Cyc_scan_exp_opt(_tmp101);
return;_LL12F: if(*((int*)_tmpFD)!= 8)goto _LL131;_tmp103=((struct Cyc_Absyn_FnType_struct*)
_tmpFD)->f1;_LL130: Cyc_scan_type(_tmp103.ret_typ);{struct Cyc_List_List*_tmp10D=
_tmp103.args;for(0;_tmp10D != 0;_tmp10D=_tmp10D->tl){struct _tuple7 _tmp10F;void*
_tmp110;struct _tuple7*_tmp10E=(struct _tuple7*)_tmp10D->hd;_tmp10F=*_tmp10E;
_tmp110=_tmp10F.f3;Cyc_scan_type(_tmp110);}}if(_tmp103.cyc_varargs != 0)Cyc_scan_type((
_tmp103.cyc_varargs)->type);return;_LL131: if(*((int*)_tmpFD)!= 11)goto _LL133;
_tmp104=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpFD)->f2;_LL132: for(0;_tmp104
!= 0;_tmp104=_tmp104->tl){Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)_tmp104->hd)->type);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmp104->hd)->width);}return;
_LL133: if(*((int*)_tmpFD)!= 13)goto _LL135;_LL134: return;_LL135: if(*((int*)_tmpFD)
!= 10)goto _LL137;_tmp105=((struct Cyc_Absyn_AggrType_struct*)_tmpFD)->f1;_tmp106=
_tmp105.aggr_info;_LL136: {struct _tuple0*_tmp112;struct _tuple0 _tmp113;struct
_dyneither_ptr*_tmp114;struct _tuple9 _tmp111=Cyc_Absyn_aggr_kinded_name(_tmp106);
_tmp112=_tmp111.f2;_tmp113=*_tmp112;_tmp114=_tmp113.f2;_tmp109=_tmp114;goto
_LL138;}_LL137: if(*((int*)_tmpFD)!= 12)goto _LL139;_tmp107=((struct Cyc_Absyn_EnumType_struct*)
_tmpFD)->f1;_tmp108=*_tmp107;_tmp109=_tmp108.f2;_LL138: _tmp10C=_tmp109;goto
_LL13A;_LL139: if(*((int*)_tmpFD)!= 16)goto _LL13B;_tmp10A=((struct Cyc_Absyn_TypedefType_struct*)
_tmpFD)->f1;_tmp10B=*_tmp10A;_tmp10C=_tmp10B.f2;_LL13A: Cyc_add_target(_tmp10C);
return;_LL13B: if(*((int*)_tmpFD)!= 0)goto _LL13D;_LL13C:{const char*_tmp452;void*
_tmp451;(_tmp451=0,Cyc_fprintf(Cyc_stderr,((_tmp452="Error: unexpected Evar\n",
_tag_dyneither(_tmp452,sizeof(char),24))),_tag_dyneither(_tmp451,sizeof(void*),0)));}
exit(1);return;_LL13D: if(*((int*)_tmpFD)!= 1)goto _LL13F;_LL13E:{const char*
_tmp455;void*_tmp454;(_tmp454=0,Cyc_fprintf(Cyc_stderr,((_tmp455="Error: unexpected VarType\n",
_tag_dyneither(_tmp455,sizeof(char),27))),_tag_dyneither(_tmp454,sizeof(void*),0)));}
exit(1);return;_LL13F: if(*((int*)_tmpFD)!= 2)goto _LL141;_LL140:{const char*
_tmp458;void*_tmp457;(_tmp457=0,Cyc_fprintf(Cyc_stderr,((_tmp458="Error: unexpected DatatypeType\n",
_tag_dyneither(_tmp458,sizeof(char),32))),_tag_dyneither(_tmp457,sizeof(void*),0)));}
exit(1);return;_LL141: if(*((int*)_tmpFD)!= 3)goto _LL143;_LL142:{const char*
_tmp45B;void*_tmp45A;(_tmp45A=0,Cyc_fprintf(Cyc_stderr,((_tmp45B="Error: unexpected DatatypeFieldType\n",
_tag_dyneither(_tmp45B,sizeof(char),37))),_tag_dyneither(_tmp45A,sizeof(void*),0)));}
exit(1);return;_LL143: if(*((int*)_tmpFD)!= 9)goto _LL145;_LL144:{const char*
_tmp45E;void*_tmp45D;(_tmp45D=0,Cyc_fprintf(Cyc_stderr,((_tmp45E="Error: unexpected TupleType\n",
_tag_dyneither(_tmp45E,sizeof(char),29))),_tag_dyneither(_tmp45D,sizeof(void*),0)));}
exit(1);return;_LL145: if(*((int*)_tmpFD)!= 14)goto _LL147;_LL146:{const char*
_tmp461;void*_tmp460;(_tmp460=0,Cyc_fprintf(Cyc_stderr,((_tmp461="Error: unexpected RgnHandleType\n",
_tag_dyneither(_tmp461,sizeof(char),33))),_tag_dyneither(_tmp460,sizeof(void*),0)));}
exit(1);return;_LL147: if(*((int*)_tmpFD)!= 15)goto _LL149;_LL148:{const char*
_tmp464;void*_tmp463;(_tmp463=0,Cyc_fprintf(Cyc_stderr,((_tmp464="Error: unexpected DynRgnType\n",
_tag_dyneither(_tmp464,sizeof(char),30))),_tag_dyneither(_tmp463,sizeof(void*),0)));}
exit(1);return;_LL149: if((int)_tmpFD != 2)goto _LL14B;_LL14A:{const char*_tmp467;
void*_tmp466;(_tmp466=0,Cyc_fprintf(Cyc_stderr,((_tmp467="Error: unexpected HeapRgn\n",
_tag_dyneither(_tmp467,sizeof(char),27))),_tag_dyneither(_tmp466,sizeof(void*),0)));}
exit(1);return;_LL14B: if((int)_tmpFD != 3)goto _LL14D;_LL14C:{const char*_tmp46A;
void*_tmp469;(_tmp469=0,Cyc_fprintf(Cyc_stderr,((_tmp46A="Error: unexpected UniqueRgn\n",
_tag_dyneither(_tmp46A,sizeof(char),29))),_tag_dyneither(_tmp469,sizeof(void*),0)));}
exit(1);return;_LL14D: if(_tmpFD <= (void*)4)goto _LL14F;if(*((int*)_tmpFD)!= 19)
goto _LL14F;_LL14E:{const char*_tmp46D;void*_tmp46C;(_tmp46C=0,Cyc_fprintf(Cyc_stderr,((
_tmp46D="Error: unexpected AccessEff\n",_tag_dyneither(_tmp46D,sizeof(char),29))),
_tag_dyneither(_tmp46C,sizeof(void*),0)));}exit(1);return;_LL14F: if(_tmpFD <= (
void*)4)goto _LL151;if(*((int*)_tmpFD)!= 20)goto _LL151;_LL150:{const char*_tmp470;
void*_tmp46F;(_tmp46F=0,Cyc_fprintf(Cyc_stderr,((_tmp470="Error: unexpected JoinEff\n",
_tag_dyneither(_tmp470,sizeof(char),27))),_tag_dyneither(_tmp46F,sizeof(void*),0)));}
exit(1);return;_LL151: if(_tmpFD <= (void*)4)goto _LL153;if(*((int*)_tmpFD)!= 21)
goto _LL153;_LL152:{const char*_tmp473;void*_tmp472;(_tmp472=0,Cyc_fprintf(Cyc_stderr,((
_tmp473="Error: unexpected RgnsEff\n",_tag_dyneither(_tmp473,sizeof(char),27))),
_tag_dyneither(_tmp472,sizeof(void*),0)));}exit(1);return;_LL153: if(_tmpFD <= (
void*)4)goto _LL155;if(*((int*)_tmpFD)!= 18)goto _LL155;_LL154:{const char*_tmp476;
void*_tmp475;(_tmp475=0,Cyc_fprintf(Cyc_stderr,((_tmp476="Error: unexpected tag_t\n",
_tag_dyneither(_tmp476,sizeof(char),25))),_tag_dyneither(_tmp475,sizeof(void*),0)));}
exit(1);return;_LL155: if(_tmpFD <= (void*)4)goto _LL122;if(*((int*)_tmpFD)!= 17)
goto _LL122;_LL156:{const char*_tmp479;void*_tmp478;(_tmp478=0,Cyc_fprintf(Cyc_stderr,((
_tmp479="Error: unexpected valueof_t\n",_tag_dyneither(_tmp479,sizeof(char),29))),
_tag_dyneither(_tmp478,sizeof(void*),0)));}exit(1);return;_LL122:;}struct
_tuple22{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct
_tuple17*Cyc_scan_decl(struct Cyc_Absyn_Decl*d);struct _tuple17*Cyc_scan_decl(
struct Cyc_Absyn_Decl*d){{struct Cyc_Set_Set**_tmp47A;Cyc_current_targets=((
_tmp47A=_cycalloc(sizeof(*_tmp47A)),((_tmp47A[0]=((struct Cyc_Set_Set*(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),
_tmp47A))));}{void*_tmp132=d->r;struct Cyc_Absyn_Vardecl*_tmp133;struct Cyc_Absyn_Fndecl*
_tmp134;struct Cyc_Absyn_Aggrdecl*_tmp135;struct Cyc_Absyn_Enumdecl*_tmp136;struct
Cyc_Absyn_Typedefdecl*_tmp137;_LL158: if(_tmp132 <= (void*)2)goto _LL166;if(*((int*)
_tmp132)!= 0)goto _LL15A;_tmp133=((struct Cyc_Absyn_Var_d_struct*)_tmp132)->f1;
_LL159: {struct _tuple0 _tmp139;struct _dyneither_ptr*_tmp13A;struct _tuple0*_tmp138=
_tmp133->name;_tmp139=*_tmp138;_tmp13A=_tmp139.f2;Cyc_current_source=(struct
_dyneither_ptr*)_tmp13A;Cyc_scan_type(_tmp133->type);Cyc_scan_exp_opt(_tmp133->initializer);
goto _LL157;}_LL15A: if(*((int*)_tmp132)!= 1)goto _LL15C;_tmp134=((struct Cyc_Absyn_Fn_d_struct*)
_tmp132)->f1;_LL15B: {struct _tuple0 _tmp13C;struct _dyneither_ptr*_tmp13D;struct
_tuple0*_tmp13B=_tmp134->name;_tmp13C=*_tmp13B;_tmp13D=_tmp13C.f2;Cyc_current_source=(
struct _dyneither_ptr*)_tmp13D;Cyc_scan_type(_tmp134->ret_type);{struct Cyc_List_List*
_tmp13E=_tmp134->args;for(0;_tmp13E != 0;_tmp13E=_tmp13E->tl){struct _tuple22
_tmp140;void*_tmp141;struct _tuple22*_tmp13F=(struct _tuple22*)_tmp13E->hd;_tmp140=*
_tmp13F;_tmp141=_tmp140.f3;Cyc_scan_type(_tmp141);}}if(_tmp134->cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp134->cyc_varargs))->type);
if(_tmp134->is_inline){const char*_tmp47D;void*_tmp47C;(_tmp47C=0,Cyc_fprintf(Cyc_stderr,((
_tmp47D="Warning: ignoring inline function\n",_tag_dyneither(_tmp47D,sizeof(char),
35))),_tag_dyneither(_tmp47C,sizeof(void*),0)));}goto _LL157;}_LL15C: if(*((int*)
_tmp132)!= 6)goto _LL15E;_tmp135=((struct Cyc_Absyn_Aggr_d_struct*)_tmp132)->f1;
_LL15D: {struct _tuple0 _tmp145;struct _dyneither_ptr*_tmp146;struct _tuple0*_tmp144=
_tmp135->name;_tmp145=*_tmp144;_tmp146=_tmp145.f2;Cyc_current_source=(struct
_dyneither_ptr*)_tmp146;if((unsigned int)_tmp135->impl){{struct Cyc_List_List*
_tmp147=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp135->impl))->fields;for(
0;_tmp147 != 0;_tmp147=_tmp147->tl){struct Cyc_Absyn_Aggrfield*_tmp148=(struct Cyc_Absyn_Aggrfield*)
_tmp147->hd;Cyc_scan_type(_tmp148->type);Cyc_scan_exp_opt(_tmp148->width);}}{
struct Cyc_List_List*_tmp149=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp135->impl))->fields;
for(0;_tmp149 != 0;_tmp149=_tmp149->tl){;}}}goto _LL157;}_LL15E: if(*((int*)_tmp132)
!= 8)goto _LL160;_tmp136=((struct Cyc_Absyn_Enum_d_struct*)_tmp132)->f1;_LL15F: {
struct _tuple0 _tmp14B;struct _dyneither_ptr*_tmp14C;struct _tuple0*_tmp14A=_tmp136->name;
_tmp14B=*_tmp14A;_tmp14C=_tmp14B.f2;Cyc_current_source=(struct _dyneither_ptr*)
_tmp14C;if((unsigned int)_tmp136->fields){{struct Cyc_List_List*_tmp14D=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp136->fields))->v;for(0;
_tmp14D != 0;_tmp14D=_tmp14D->tl){struct Cyc_Absyn_Enumfield*_tmp14E=(struct Cyc_Absyn_Enumfield*)
_tmp14D->hd;Cyc_scan_exp_opt(_tmp14E->tag);}}{struct Cyc_List_List*_tmp14F=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp136->fields))->v;for(
0;_tmp14F != 0;_tmp14F=_tmp14F->tl){;}}}goto _LL157;}_LL160: if(*((int*)_tmp132)!= 
9)goto _LL162;_tmp137=((struct Cyc_Absyn_Typedef_d_struct*)_tmp132)->f1;_LL161: {
struct _tuple0 _tmp151;struct _dyneither_ptr*_tmp152;struct _tuple0*_tmp150=_tmp137->name;
_tmp151=*_tmp150;_tmp152=_tmp151.f2;Cyc_current_source=(struct _dyneither_ptr*)
_tmp152;if((unsigned int)_tmp137->defn)Cyc_scan_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp137->defn))->v);goto _LL157;}_LL162: if(*((int*)_tmp132)!= 4)goto
_LL164;_LL163:{const char*_tmp480;void*_tmp47F;(_tmp47F=0,Cyc_fprintf(Cyc_stderr,((
_tmp480="Error: unexpected region declaration",_tag_dyneither(_tmp480,sizeof(
char),37))),_tag_dyneither(_tmp47F,sizeof(void*),0)));}exit(1);_LL164: if(*((int*)
_tmp132)!= 5)goto _LL166;_LL165:{const char*_tmp483;void*_tmp482;(_tmp482=0,Cyc_fprintf(
Cyc_stderr,((_tmp483="Error: unexpected alias declaration",_tag_dyneither(
_tmp483,sizeof(char),36))),_tag_dyneither(_tmp482,sizeof(void*),0)));}exit(1);
_LL166: if((int)_tmp132 != 0)goto _LL168;_LL167:{const char*_tmp486;void*_tmp485;(
_tmp485=0,Cyc_fprintf(Cyc_stderr,((_tmp486="Error: unexpected __cyclone_port_on__",
_tag_dyneither(_tmp486,sizeof(char),38))),_tag_dyneither(_tmp485,sizeof(void*),0)));}
exit(1);return 0;_LL168: if((int)_tmp132 != 1)goto _LL16A;_LL169:{const char*_tmp489;
void*_tmp488;(_tmp488=0,Cyc_fprintf(Cyc_stderr,((_tmp489="Error: unexpected __cyclone_port_off__",
_tag_dyneither(_tmp489,sizeof(char),39))),_tag_dyneither(_tmp488,sizeof(void*),0)));}
exit(1);return 0;_LL16A: if(_tmp132 <= (void*)2)goto _LL16C;if(*((int*)_tmp132)!= 2)
goto _LL16C;_LL16B:{const char*_tmp48C;void*_tmp48B;(_tmp48B=0,Cyc_fprintf(Cyc_stderr,((
_tmp48C="Error: unexpected let declaration\n",_tag_dyneither(_tmp48C,sizeof(char),
35))),_tag_dyneither(_tmp48B,sizeof(void*),0)));}exit(1);return 0;_LL16C: if(
_tmp132 <= (void*)2)goto _LL16E;if(*((int*)_tmp132)!= 7)goto _LL16E;_LL16D:{const
char*_tmp48F;void*_tmp48E;(_tmp48E=0,Cyc_fprintf(Cyc_stderr,((_tmp48F="Error: unexpected datatype declaration\n",
_tag_dyneither(_tmp48F,sizeof(char),40))),_tag_dyneither(_tmp48E,sizeof(void*),0)));}
exit(1);return 0;_LL16E: if(_tmp132 <= (void*)2)goto _LL170;if(*((int*)_tmp132)!= 3)
goto _LL170;_LL16F:{const char*_tmp492;void*_tmp491;(_tmp491=0,Cyc_fprintf(Cyc_stderr,((
_tmp492="Error: unexpected let declaration\n",_tag_dyneither(_tmp492,sizeof(char),
35))),_tag_dyneither(_tmp491,sizeof(void*),0)));}exit(1);return 0;_LL170: if(
_tmp132 <= (void*)2)goto _LL172;if(*((int*)_tmp132)!= 10)goto _LL172;_LL171:{const
char*_tmp495;void*_tmp494;(_tmp494=0,Cyc_fprintf(Cyc_stderr,((_tmp495="Error: unexpected namespace declaration\n",
_tag_dyneither(_tmp495,sizeof(char),41))),_tag_dyneither(_tmp494,sizeof(void*),0)));}
exit(1);return 0;_LL172: if(_tmp132 <= (void*)2)goto _LL174;if(*((int*)_tmp132)!= 11)
goto _LL174;_LL173:{const char*_tmp498;void*_tmp497;(_tmp497=0,Cyc_fprintf(Cyc_stderr,((
_tmp498="Error: unexpected using declaration\n",_tag_dyneither(_tmp498,sizeof(
char),37))),_tag_dyneither(_tmp497,sizeof(void*),0)));}exit(1);return 0;_LL174:
if(_tmp132 <= (void*)2)goto _LL176;if(*((int*)_tmp132)!= 12)goto _LL176;_LL175:{
const char*_tmp49B;void*_tmp49A;(_tmp49A=0,Cyc_fprintf(Cyc_stderr,((_tmp49B="Error: unexpected extern \"C\" declaration\n",
_tag_dyneither(_tmp49B,sizeof(char),42))),_tag_dyneither(_tmp49A,sizeof(void*),0)));}
exit(1);return 0;_LL176: if(_tmp132 <= (void*)2)goto _LL157;if(*((int*)_tmp132)!= 13)
goto _LL157;_LL177:{const char*_tmp49E;void*_tmp49D;(_tmp49D=0,Cyc_fprintf(Cyc_stderr,((
_tmp49E="Error: unexpected extern \"C include\" declaration\n",_tag_dyneither(
_tmp49E,sizeof(char),50))),_tag_dyneither(_tmp49D,sizeof(void*),0)));}exit(1);
return 0;_LL157:;}{struct _tuple17*_tmp49F;return(_tmp49F=_cycalloc(sizeof(*
_tmp49F)),((_tmp49F->f1=(struct _dyneither_ptr*)_check_null(Cyc_current_source),((
_tmp49F->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),_tmp49F)))));}}
struct Cyc_Hashtable_Table*Cyc_new_deps();struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(
107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}struct Cyc_Set_Set*Cyc_find(
struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x);struct Cyc_Set_Set*Cyc_find(
struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){struct _handler_cons _tmp16A;
_push_handler(& _tmp16A);{int _tmp16C=0;if(setjmp(_tmp16A.handler))_tmp16C=1;if(!
_tmp16C){{struct Cyc_Set_Set*_tmp16D=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*
t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0);return
_tmp16D;};_pop_handler();}else{void*_tmp16B=(void*)_exn_thrown;void*_tmp16F=
_tmp16B;_LL179: if(_tmp16F != Cyc_Core_Not_found)goto _LL17B;_LL17A: return((struct
Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(
Cyc_strptrcmp);_LL17B:;_LL17C:(void)_throw(_tmp16F);_LL178:;}}}struct Cyc_Set_Set*
Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t);struct Cyc_Set_Set*
Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){struct Cyc_Set_Set*
emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct
_dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);struct Cyc_Set_Set*curr;for(curr=
emptyset;init != 0;init=init->tl){curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*
s,struct _dyneither_ptr*elt))Cyc_Set_insert)(curr,(struct _dyneither_ptr*)init->hd);}{
struct Cyc_Set_Set*_tmp170=curr;struct _dyneither_ptr*_tmp171=_init_dyneither_ptr(
_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);while(((int(*)(struct
Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp170)> 0){struct Cyc_Set_Set*_tmp172=
emptyset;struct Cyc_Iter_Iter _tmp173=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*
rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,_tmp170);while(((
int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(_tmp173,&
_tmp171)){_tmp172=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_union_two)(_tmp172,Cyc_find(t,_tmp171));}_tmp170=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(_tmp172,curr);curr=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
curr,_tmp170);}return curr;}}static void*Cyc_mode=(void*)0;static int Cyc_gathering();
static int Cyc_gathering(){return Cyc_mode == (void*)1  || Cyc_mode == (void*)2;}
static struct Cyc___cycFILE*Cyc_script_file=0;int Cyc_prscript(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap);int Cyc_prscript(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap){if(Cyc_script_file == 0){{const char*_tmp4A2;void*_tmp4A1;(
_tmp4A1=0,Cyc_fprintf(Cyc_stderr,((_tmp4A2="Internal error: script file is NULL\n",
_tag_dyneither(_tmp4A2,sizeof(char),37))),_tag_dyneither(_tmp4A1,sizeof(void*),0)));}
exit(1);}return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),
fmt,ap);}int Cyc_force_directory(struct _dyneither_ptr d);int Cyc_force_directory(
struct _dyneither_ptr d){if(Cyc_mode == (void*)2){const char*_tmp4A7;void*_tmp4A6[2];
struct Cyc_String_pa_struct _tmp4A5;struct Cyc_String_pa_struct _tmp4A4;(_tmp4A4.tag=
0,((_tmp4A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((_tmp4A5.tag=0,((
_tmp4A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((_tmp4A6[0]=& _tmp4A5,((
_tmp4A6[1]=& _tmp4A4,Cyc_prscript(((_tmp4A7="if ! test -e %s; then mkdir %s; fi\n",
_tag_dyneither(_tmp4A7,sizeof(char),36))),_tag_dyneither(_tmp4A6,sizeof(void*),2)))))))))))));}
else{unsigned short _tmp4A8[0];int _tmp17B=Cyc_open((const char*)_check_null(
_untag_dyneither_ptr(d,sizeof(char),1)),0,_tag_dyneither(_tmp4A8,sizeof(
unsigned short),0));if(_tmp17B == - 1){if(mkdir((const char*)_untag_dyneither_ptr(d,
sizeof(char),1),448)== - 1){{const char*_tmp4AC;void*_tmp4AB[1];struct Cyc_String_pa_struct
_tmp4AA;(_tmp4AA.tag=0,((_tmp4AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
d),((_tmp4AB[0]=& _tmp4AA,Cyc_fprintf(Cyc_stderr,((_tmp4AC="Error: could not create directory %s\n",
_tag_dyneither(_tmp4AC,sizeof(char),38))),_tag_dyneither(_tmp4AB,sizeof(void*),1)))))));}
return 1;}}else{close(_tmp17B);}}return 0;}int Cyc_force_directory_prefixes(struct
_dyneither_ptr file);int Cyc_force_directory_prefixes(struct _dyneither_ptr file){
struct _dyneither_ptr _tmp180=Cyc_strdup((struct _dyneither_ptr)file);struct Cyc_List_List*
_tmp181=0;while(1){_tmp180=Cyc_Filename_dirname((struct _dyneither_ptr)_tmp180);
if(Cyc_strlen((struct _dyneither_ptr)_tmp180)== 0)break;{struct _dyneither_ptr*
_tmp4AF;struct Cyc_List_List*_tmp4AE;_tmp181=((_tmp4AE=_cycalloc(sizeof(*_tmp4AE)),((
_tmp4AE->hd=((_tmp4AF=_cycalloc(sizeof(*_tmp4AF)),((_tmp4AF[0]=(struct
_dyneither_ptr)_tmp180,_tmp4AF)))),((_tmp4AE->tl=_tmp181,_tmp4AE))))));}}for(0;
_tmp181 != 0;_tmp181=_tmp181->tl){if(Cyc_force_directory(*((struct _dyneither_ptr*)
_tmp181->hd)))return 1;}return 0;}char Cyc_NO_SUPPORT[15]="\000\000\000\000NO_SUPPORT\000";
struct Cyc_NO_SUPPORT_struct{char*tag;struct _dyneither_ptr f1;};struct _tuple23{
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};int Cyc_process_file(const char*
filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*omit_symbols,
struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*
cycstubs);int Cyc_process_file(const char*filename,struct Cyc_List_List*
start_symbols,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct
Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs){struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;struct Cyc___cycFILE*out_file;int errorcode=0;{const
char*_tmp4B5;const char*_tmp4B4;void*_tmp4B3[1];struct Cyc_String_pa_struct _tmp4B2;(
_tmp4B2.tag=0,((_tmp4B2.f1=(struct _dyneither_ptr)((_tmp4B5=filename,
_tag_dyneither(_tmp4B5,sizeof(char),_get_zero_arr_size_char(_tmp4B5,1)))),((
_tmp4B3[0]=& _tmp4B2,Cyc_fprintf(Cyc_stderr,((_tmp4B4="********************************* %s...\n",
_tag_dyneither(_tmp4B4,sizeof(char),41))),_tag_dyneither(_tmp4B3,sizeof(void*),1)))))));}
if(!Cyc_gathering()){const char*_tmp4BB;const char*_tmp4BA;void*_tmp4B9[1];struct
Cyc_String_pa_struct _tmp4B8;(_tmp4B8.tag=0,((_tmp4B8.f1=(struct _dyneither_ptr)((
_tmp4BB=filename,_tag_dyneither(_tmp4BB,sizeof(char),_get_zero_arr_size_char(
_tmp4BB,1)))),((_tmp4B9[0]=& _tmp4B8,Cyc_log(((_tmp4BA="\n%s:\n",_tag_dyneither(
_tmp4BA,sizeof(char),6))),_tag_dyneither(_tmp4B9,sizeof(void*),1)))))));}{const
char*_tmp4BC;struct _dyneither_ptr _tmp18C=Cyc_Filename_basename(((_tmp4BC=
filename,_tag_dyneither(_tmp4BC,sizeof(char),_get_zero_arr_size_char(_tmp4BC,1)))));
const char*_tmp4BD;struct _dyneither_ptr _tmp18D=Cyc_Filename_dirname(((_tmp4BD=
filename,_tag_dyneither(_tmp4BD,sizeof(char),_get_zero_arr_size_char(_tmp4BD,1)))));
struct _dyneither_ptr _tmp18E=Cyc_Filename_chop_extension((struct _dyneither_ptr)
_tmp18C);const char*_tmp4BE;const char*_tmp18F=(const char*)_untag_dyneither_ptr(
Cyc_strconcat((struct _dyneither_ptr)_tmp18E,((_tmp4BE=".iA",_tag_dyneither(
_tmp4BE,sizeof(char),4)))),sizeof(char),1);const char*_tmp4C6;void*_tmp4C5[1];
struct Cyc_String_pa_struct _tmp4C4;const char*_tmp4C2;void*_tmp4C1[1];struct Cyc_String_pa_struct
_tmp4C0;const char*_tmp190=(const char*)_untag_dyneither_ptr(_get_dyneither_size(
_tmp18D,sizeof(char))== 0?(_tmp4C4.tag=0,((_tmp4C4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp18E),((_tmp4C5[0]=& _tmp4C4,Cyc_aprintf(((_tmp4C6="%s.iB",
_tag_dyneither(_tmp4C6,sizeof(char),6))),_tag_dyneither(_tmp4C5,sizeof(void*),1))))))):
Cyc_Filename_concat((struct _dyneither_ptr)_tmp18D,(struct _dyneither_ptr)((
_tmp4C0.tag=0,((_tmp4C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp18E),((
_tmp4C1[0]=& _tmp4C0,Cyc_aprintf(((_tmp4C2="%s.iB",_tag_dyneither(_tmp4C2,sizeof(
char),6))),_tag_dyneither(_tmp4C1,sizeof(void*),1))))))))),sizeof(char),1);const
char*_tmp4CE;void*_tmp4CD[1];struct Cyc_String_pa_struct _tmp4CC;const char*_tmp4CA;
void*_tmp4C9[1];struct Cyc_String_pa_struct _tmp4C8;const char*_tmp191=(const char*)
_untag_dyneither_ptr(_get_dyneither_size(_tmp18D,sizeof(char))== 0?(_tmp4CC.tag=
0,((_tmp4CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp18E),((_tmp4CD[0]=&
_tmp4CC,Cyc_aprintf(((_tmp4CE="%s.iC",_tag_dyneither(_tmp4CE,sizeof(char),6))),
_tag_dyneither(_tmp4CD,sizeof(void*),1))))))): Cyc_Filename_concat((struct
_dyneither_ptr)_tmp18D,(struct _dyneither_ptr)((_tmp4C8.tag=0,((_tmp4C8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp18E),((_tmp4C9[0]=& _tmp4C8,Cyc_aprintf(((
_tmp4CA="%s.iC",_tag_dyneither(_tmp4CA,sizeof(char),6))),_tag_dyneither(_tmp4C9,
sizeof(void*),1))))))))),sizeof(char),1);const char*_tmp4D6;void*_tmp4D5[1];
struct Cyc_String_pa_struct _tmp4D4;const char*_tmp4D2;void*_tmp4D1[1];struct Cyc_String_pa_struct
_tmp4D0;const char*_tmp192=(const char*)_untag_dyneither_ptr(_get_dyneither_size(
_tmp18D,sizeof(char))== 0?(_tmp4D4.tag=0,((_tmp4D4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp18E),((_tmp4D5[0]=& _tmp4D4,Cyc_aprintf(((_tmp4D6="%s.iD",
_tag_dyneither(_tmp4D6,sizeof(char),6))),_tag_dyneither(_tmp4D5,sizeof(void*),1))))))):
Cyc_Filename_concat((struct _dyneither_ptr)_tmp18D,(struct _dyneither_ptr)((
_tmp4D0.tag=0,((_tmp4D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp18E),((
_tmp4D1[0]=& _tmp4D0,Cyc_aprintf(((_tmp4D2="%s.iD",_tag_dyneither(_tmp4D2,sizeof(
char),6))),_tag_dyneither(_tmp4D1,sizeof(void*),1))))))))),sizeof(char),1);
struct _handler_cons _tmp193;_push_handler(& _tmp193);{int _tmp195=0;if(setjmp(
_tmp193.handler))_tmp195=1;if(!_tmp195){{const char*_tmp4D7;if(Cyc_force_directory_prefixes(((
_tmp4D7=filename,_tag_dyneither(_tmp4D7,sizeof(char),_get_zero_arr_size_char(
_tmp4D7,1)))))){int _tmp197=1;_npop_handler(0);return _tmp197;}}if(Cyc_mode != (
void*)3){Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cpp);if(Cyc_mode == (void*)2){{const char*_tmp4DD;const char*_tmp4DC;
void*_tmp4DB[1];struct Cyc_String_pa_struct _tmp4DA;(_tmp4DA.tag=0,((_tmp4DA.f1=(
struct _dyneither_ptr)((_tmp4DD=_tmp18F,_tag_dyneither(_tmp4DD,sizeof(char),
_get_zero_arr_size_char(_tmp4DD,1)))),((_tmp4DB[0]=& _tmp4DA,Cyc_prscript(((
_tmp4DC="cat >%s <<XXX\n",_tag_dyneither(_tmp4DC,sizeof(char),15))),
_tag_dyneither(_tmp4DB,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmp19C=Cyc_current_cpp;
for(0;_tmp19C != 0;_tmp19C=_tmp19C->tl){const char*_tmp4E1;void*_tmp4E0[1];struct
Cyc_String_pa_struct _tmp4DF;(_tmp4DF.tag=0,((_tmp4DF.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp19C->hd)),((_tmp4E0[0]=&
_tmp4DF,Cyc_prscript(((_tmp4E1="%s",_tag_dyneither(_tmp4E1,sizeof(char),3))),
_tag_dyneither(_tmp4E0,sizeof(void*),1)))))));}}{const char*_tmp4E7;const char*
_tmp4E6;void*_tmp4E5[1];struct Cyc_String_pa_struct _tmp4E4;(_tmp4E4.tag=0,((
_tmp4E4.f1=(struct _dyneither_ptr)((_tmp4E7=filename,_tag_dyneither(_tmp4E7,
sizeof(char),_get_zero_arr_size_char(_tmp4E7,1)))),((_tmp4E5[0]=& _tmp4E4,Cyc_prscript(((
_tmp4E6="#include <%s>\n",_tag_dyneither(_tmp4E6,sizeof(char),15))),
_tag_dyneither(_tmp4E5,sizeof(void*),1)))))));}{const char*_tmp4EA;void*_tmp4E9;(
_tmp4E9=0,Cyc_prscript(((_tmp4EA="XXX\n",_tag_dyneither(_tmp4EA,sizeof(char),5))),
_tag_dyneither(_tmp4E9,sizeof(void*),0)));}{const char*_tmp4F3;const char*_tmp4F2;
const char*_tmp4F1;void*_tmp4F0[2];struct Cyc_String_pa_struct _tmp4EF;struct Cyc_String_pa_struct
_tmp4EE;(_tmp4EE.tag=0,((_tmp4EE.f1=(struct _dyneither_ptr)((_tmp4F3=_tmp18F,
_tag_dyneither(_tmp4F3,sizeof(char),_get_zero_arr_size_char(_tmp4F3,1)))),((
_tmp4EF.tag=0,((_tmp4EF.f1=(struct _dyneither_ptr)((_tmp4F2=_tmp190,
_tag_dyneither(_tmp4F2,sizeof(char),_get_zero_arr_size_char(_tmp4F2,1)))),((
_tmp4F0[0]=& _tmp4EF,((_tmp4F0[1]=& _tmp4EE,Cyc_prscript(((_tmp4F1="$GCC -E -dM -o %s -x c %s && \\\n",
_tag_dyneither(_tmp4F1,sizeof(char),32))),_tag_dyneither(_tmp4F0,sizeof(void*),2)))))))))))));}{
const char*_tmp4FC;const char*_tmp4FB;const char*_tmp4FA;void*_tmp4F9[2];struct Cyc_String_pa_struct
_tmp4F8;struct Cyc_String_pa_struct _tmp4F7;(_tmp4F7.tag=0,((_tmp4F7.f1=(struct
_dyneither_ptr)((_tmp4FC=_tmp18F,_tag_dyneither(_tmp4FC,sizeof(char),
_get_zero_arr_size_char(_tmp4FC,1)))),((_tmp4F8.tag=0,((_tmp4F8.f1=(struct
_dyneither_ptr)((_tmp4FB=_tmp191,_tag_dyneither(_tmp4FB,sizeof(char),
_get_zero_arr_size_char(_tmp4FB,1)))),((_tmp4F9[0]=& _tmp4F8,((_tmp4F9[1]=&
_tmp4F7,Cyc_prscript(((_tmp4FA="$GCC -E     -o %s -x c %s;\n",_tag_dyneither(
_tmp4FA,sizeof(char),28))),_tag_dyneither(_tmp4F9,sizeof(void*),2)))))))))))));}{
const char*_tmp502;const char*_tmp501;void*_tmp500[1];struct Cyc_String_pa_struct
_tmp4FF;(_tmp4FF.tag=0,((_tmp4FF.f1=(struct _dyneither_ptr)((_tmp502=_tmp18F,
_tag_dyneither(_tmp502,sizeof(char),_get_zero_arr_size_char(_tmp502,1)))),((
_tmp500[0]=& _tmp4FF,Cyc_prscript(((_tmp501="rm %s\n",_tag_dyneither(_tmp501,
sizeof(char),7))),_tag_dyneither(_tmp500,sizeof(void*),1)))))));}}else{maybe=Cyc_fopen(
_tmp18F,(const char*)"w");if(!((unsigned int)maybe)){{const char*_tmp508;const char*
_tmp507;void*_tmp506[1];struct Cyc_String_pa_struct _tmp505;(_tmp505.tag=0,((
_tmp505.f1=(struct _dyneither_ptr)((_tmp508=_tmp18F,_tag_dyneither(_tmp508,
sizeof(char),_get_zero_arr_size_char(_tmp508,1)))),((_tmp506[0]=& _tmp505,Cyc_fprintf(
Cyc_stderr,((_tmp507="Error: could not create file %s\n",_tag_dyneither(_tmp507,
sizeof(char),33))),_tag_dyneither(_tmp506,sizeof(void*),1)))))));}{int _tmp1BA=1;
_npop_handler(0);return _tmp1BA;}}out_file=(struct Cyc___cycFILE*)maybe;{struct Cyc_List_List*
_tmp1BB=Cyc_current_cpp;for(0;_tmp1BB != 0;_tmp1BB=_tmp1BB->tl){Cyc_fputs((const
char*)_untag_dyneither_ptr(*((struct _dyneither_ptr*)_tmp1BB->hd),sizeof(char),1),
out_file);}}{const char*_tmp50E;const char*_tmp50D;void*_tmp50C[1];struct Cyc_String_pa_struct
_tmp50B;(_tmp50B.tag=0,((_tmp50B.f1=(struct _dyneither_ptr)((_tmp50E=filename,
_tag_dyneither(_tmp50E,sizeof(char),_get_zero_arr_size_char(_tmp50E,1)))),((
_tmp50C[0]=& _tmp50B,Cyc_fprintf(out_file,((_tmp50D="#include <%s>\n",
_tag_dyneither(_tmp50D,sizeof(char),15))),_tag_dyneither(_tmp50C,sizeof(void*),1)))))));}
Cyc_fclose(out_file);{struct _dyneither_ptr _tmp1C0=Cstring_to_string(Ccomp);const
char*_tmp518;const char*_tmp517;const char*_tmp516;void*_tmp515[3];struct Cyc_String_pa_struct
_tmp514;struct Cyc_String_pa_struct _tmp513;struct Cyc_String_pa_struct _tmp512;char*
cmd=(char*)_untag_dyneither_ptr(((_tmp512.tag=0,((_tmp512.f1=(struct
_dyneither_ptr)((_tmp518=_tmp18F,_tag_dyneither(_tmp518,sizeof(char),
_get_zero_arr_size_char(_tmp518,1)))),((_tmp513.tag=0,((_tmp513.f1=(struct
_dyneither_ptr)((_tmp517=_tmp190,_tag_dyneither(_tmp517,sizeof(char),
_get_zero_arr_size_char(_tmp517,1)))),((_tmp514.tag=0,((_tmp514.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp1C0),((_tmp515[0]=& _tmp514,((_tmp515[1]=&
_tmp513,((_tmp515[2]=& _tmp512,Cyc_aprintf(((_tmp516="%s -E -dM -o %s -x c %s",
_tag_dyneither(_tmp516,sizeof(char),24))),_tag_dyneither(_tmp515,sizeof(void*),3)))))))))))))))))))),
sizeof(char),1);if(!system((const char*)cmd)){{const char*_tmp522;const char*
_tmp521;const char*_tmp520;void*_tmp51F[3];struct Cyc_String_pa_struct _tmp51E;
struct Cyc_String_pa_struct _tmp51D;struct Cyc_String_pa_struct _tmp51C;cmd=(char*)
_untag_dyneither_ptr(((_tmp51C.tag=0,((_tmp51C.f1=(struct _dyneither_ptr)((
_tmp522=_tmp18F,_tag_dyneither(_tmp522,sizeof(char),_get_zero_arr_size_char(
_tmp522,1)))),((_tmp51D.tag=0,((_tmp51D.f1=(struct _dyneither_ptr)((_tmp521=
_tmp191,_tag_dyneither(_tmp521,sizeof(char),_get_zero_arr_size_char(_tmp521,1)))),((
_tmp51E.tag=0,((_tmp51E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1C0),((
_tmp51F[0]=& _tmp51E,((_tmp51F[1]=& _tmp51D,((_tmp51F[2]=& _tmp51C,Cyc_aprintf(((
_tmp520="%s -E -o %s -x c %s",_tag_dyneither(_tmp520,sizeof(char),20))),
_tag_dyneither(_tmp51F,sizeof(void*),3)))))))))))))))))))),sizeof(char),1);}
system((const char*)cmd);}remove(_tmp18F);}}}if(Cyc_gathering()){int _tmp1CF=0;
_npop_handler(0);return _tmp1CF;}{struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(_tmp190,(const char*)"r");if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct
_tmp533;const char*_tmp532;const char*_tmp531;void*_tmp530[1];struct Cyc_String_pa_struct
_tmp52F;struct Cyc_NO_SUPPORT_struct*_tmp52E;(int)_throw((void*)((_tmp52E=
_cycalloc(sizeof(*_tmp52E)),((_tmp52E[0]=((_tmp533.tag=Cyc_NO_SUPPORT,((_tmp533.f1=(
struct _dyneither_ptr)((_tmp52F.tag=0,((_tmp52F.f1=(struct _dyneither_ptr)((
_tmp532=_tmp190,_tag_dyneither(_tmp532,sizeof(char),_get_zero_arr_size_char(
_tmp532,1)))),((_tmp530[0]=& _tmp52F,Cyc_aprintf(((_tmp531="can't open macrosfile %s",
_tag_dyneither(_tmp531,sizeof(char),25))),_tag_dyneither(_tmp530,sizeof(void*),1)))))))),
_tmp533)))),_tmp52E)))));}in_file=(struct Cyc___cycFILE*)maybe;{struct Cyc_Lexing_lexbuf*
_tmp1D6=Cyc_Lexing_from_file(in_file);struct _tuple17*entry;while((entry=((struct
_tuple17*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(_tmp1D6))!= 0){struct
_tuple17 _tmp1D8;struct _dyneither_ptr*_tmp1D9;struct Cyc_Set_Set*_tmp1DA;struct
_tuple17*_tmp1D7=(struct _tuple17*)_check_null(entry);_tmp1D8=*_tmp1D7;_tmp1D9=
_tmp1D8.f1;_tmp1DA=_tmp1D8.f2;((void(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp1D9,
_tmp1DA);}Cyc_fclose(in_file);maybe=Cyc_fopen(_tmp191,(const char*)"r");if(!((
unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct _tmp544;const char*_tmp543;const
char*_tmp542;void*_tmp541[1];struct Cyc_String_pa_struct _tmp540;struct Cyc_NO_SUPPORT_struct*
_tmp53F;(int)_throw((void*)((_tmp53F=_cycalloc(sizeof(*_tmp53F)),((_tmp53F[0]=((
_tmp544.tag=Cyc_NO_SUPPORT,((_tmp544.f1=(struct _dyneither_ptr)((_tmp540.tag=0,((
_tmp540.f1=(struct _dyneither_ptr)((_tmp543=_tmp191,_tag_dyneither(_tmp543,
sizeof(char),_get_zero_arr_size_char(_tmp543,1)))),((_tmp541[0]=& _tmp540,Cyc_aprintf(((
_tmp542="can't open declsfile %s",_tag_dyneither(_tmp542,sizeof(char),24))),
_tag_dyneither(_tmp541,sizeof(void*),1)))))))),_tmp544)))),_tmp53F)))));}in_file=(
struct Cyc___cycFILE*)maybe;_tmp1D6=Cyc_Lexing_from_file(in_file);Cyc_slurp_out=
Cyc_fopen(_tmp192,(const char*)"w");if(!((unsigned int)Cyc_slurp_out)){int _tmp1E1=
1;_npop_handler(0);return _tmp1E1;}while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))
Cyc_slurp)(_tmp1D6)){;}Cyc_fclose(in_file);Cyc_fclose((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));if(Cyc_mode != (void*)3)remove(_tmp191);maybe=Cyc_fopen(
_tmp192,(const char*)"r");if(!((unsigned int)maybe)){int _tmp1E2=1;_npop_handler(0);
return _tmp1E2;}in_file=(struct Cyc___cycFILE*)maybe;{const char*_tmp545;Cyc_Position_reset_position(((
_tmp545=_tmp192,_tag_dyneither(_tmp545,sizeof(char),_get_zero_arr_size_char(
_tmp545,1)))));}Cyc_Lex_lex_init(0);{struct Cyc_List_List*_tmp1E4=Cyc_Parse_parse_file(
in_file);Cyc_Lex_lex_init(0);Cyc_fclose(in_file);remove(_tmp192);{struct Cyc_List_List*
_tmp1E5=_tmp1E4;for(0;_tmp1E5 != 0;_tmp1E5=_tmp1E5->tl){struct _tuple17*_tmp1E6=
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp1E5->hd);if(_tmp1E6 == 0)continue;{
struct _tuple17 _tmp1E8;struct _dyneither_ptr*_tmp1E9;struct Cyc_Set_Set*_tmp1EA;
struct _tuple17*_tmp1E7=(struct _tuple17*)_tmp1E6;_tmp1E8=*_tmp1E7;_tmp1E9=_tmp1E8.f1;
_tmp1EA=_tmp1E8.f2;{struct Cyc_Set_Set*old;{struct _handler_cons _tmp1EB;
_push_handler(& _tmp1EB);{int _tmp1ED=0;if(setjmp(_tmp1EB.handler))_tmp1ED=1;if(!
_tmp1ED){old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key))Cyc_Hashtable_lookup)(t,_tmp1E9);;_pop_handler();}else{void*
_tmp1EC=(void*)_exn_thrown;void*_tmp1EF=_tmp1EC;_LL17E: if(_tmp1EF != Cyc_Core_Not_found)
goto _LL180;_LL17F: old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL17D;_LL180:;_LL181:(
void)_throw(_tmp1EF);_LL17D:;}}}((void(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp1E9,((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp1EA,old));}}}}{struct Cyc_Set_Set*_tmp1F0=Cyc_reachable(start_symbols,t);
struct Cyc_List_List*_tmp1F1=0;struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);{
struct Cyc_List_List*_tmp1F2=_tmp1E4;for(0;_tmp1F2 != 0;_tmp1F2=_tmp1F2->tl){
struct Cyc_Absyn_Decl*_tmp1F3=(struct Cyc_Absyn_Decl*)_tmp1F2->hd;struct
_dyneither_ptr*name;{void*_tmp1F4=_tmp1F3->r;struct Cyc_Absyn_Vardecl*_tmp1F5;
struct Cyc_Absyn_Fndecl*_tmp1F6;struct Cyc_Absyn_Aggrdecl*_tmp1F7;struct Cyc_Absyn_Enumdecl*
_tmp1F8;struct Cyc_Absyn_Typedefdecl*_tmp1F9;_LL183: if(_tmp1F4 <= (void*)2)goto
_LL18D;if(*((int*)_tmp1F4)!= 0)goto _LL185;_tmp1F5=((struct Cyc_Absyn_Var_d_struct*)
_tmp1F4)->f1;_LL184: {struct _tuple0 _tmp1FB;struct _dyneither_ptr*_tmp1FC;struct
_tuple0*_tmp1FA=_tmp1F5->name;_tmp1FB=*_tmp1FA;_tmp1FC=_tmp1FB.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_insert)(defined_symbols,_tmp1FC);if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp1FC))name=0;else{
name=(struct _dyneither_ptr*)_tmp1FC;}goto _LL182;}_LL185: if(*((int*)_tmp1F4)!= 1)
goto _LL187;_tmp1F6=((struct Cyc_Absyn_Fn_d_struct*)_tmp1F4)->f1;_LL186: {struct
_tuple0 _tmp1FE;struct _dyneither_ptr*_tmp1FF;struct _tuple0*_tmp1FD=_tmp1F6->name;
_tmp1FE=*_tmp1FD;_tmp1FF=_tmp1FE.f2;defined_symbols=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,
_tmp1FF);if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),
struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,
omit_symbols,_tmp1FF))name=0;else{name=(struct _dyneither_ptr*)_tmp1FF;}goto
_LL182;}_LL187: if(*((int*)_tmp1F4)!= 6)goto _LL189;_tmp1F7=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp1F4)->f1;_LL188: {struct _tuple0 _tmp201;struct _dyneither_ptr*_tmp202;struct
_tuple0*_tmp200=_tmp1F7->name;_tmp201=*_tmp200;_tmp202=_tmp201.f2;name=(struct
_dyneither_ptr*)_tmp202;goto _LL182;}_LL189: if(*((int*)_tmp1F4)!= 8)goto _LL18B;
_tmp1F8=((struct Cyc_Absyn_Enum_d_struct*)_tmp1F4)->f1;_LL18A: {struct _tuple0
_tmp204;struct _dyneither_ptr*_tmp205;struct _tuple0*_tmp203=_tmp1F8->name;_tmp204=*
_tmp203;_tmp205=_tmp204.f2;name=(struct _dyneither_ptr*)_tmp205;if(name != 0  && ((
int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp1F0,(
struct _dyneither_ptr*)name)){struct Cyc_List_List*_tmp546;_tmp1F1=((_tmp546=
_cycalloc(sizeof(*_tmp546)),((_tmp546->hd=_tmp1F3,((_tmp546->tl=_tmp1F1,_tmp546))))));}
else{if((unsigned int)_tmp1F8->fields){struct Cyc_List_List*_tmp207=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1F8->fields))->v;for(0;_tmp207 != 0;_tmp207=
_tmp207->tl){struct Cyc_Absyn_Enumfield*_tmp208=(struct Cyc_Absyn_Enumfield*)
_tmp207->hd;struct _tuple0 _tmp20A;struct _dyneither_ptr*_tmp20B;struct _tuple0*
_tmp209=_tmp208->name;_tmp20A=*_tmp209;_tmp20B=_tmp20A.f2;if(((int(*)(struct Cyc_Set_Set*
s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp1F0,_tmp20B)){{struct Cyc_List_List*
_tmp547;_tmp1F1=((_tmp547=_cycalloc(sizeof(*_tmp547)),((_tmp547->hd=_tmp1F3,((
_tmp547->tl=_tmp1F1,_tmp547))))));}break;}}}}name=0;goto _LL182;}_LL18B: if(*((int*)
_tmp1F4)!= 9)goto _LL18D;_tmp1F9=((struct Cyc_Absyn_Typedef_d_struct*)_tmp1F4)->f1;
_LL18C: {struct _tuple0 _tmp20E;struct _dyneither_ptr*_tmp20F;struct _tuple0*_tmp20D=
_tmp1F9->name;_tmp20E=*_tmp20D;_tmp20F=_tmp20E.f2;name=(struct _dyneither_ptr*)
_tmp20F;goto _LL182;}_LL18D: if((int)_tmp1F4 != 0)goto _LL18F;_LL18E: goto _LL190;
_LL18F: if((int)_tmp1F4 != 1)goto _LL191;_LL190: goto _LL192;_LL191: if(_tmp1F4 <= (
void*)2)goto _LL193;if(*((int*)_tmp1F4)!= 2)goto _LL193;_LL192: goto _LL194;_LL193:
if(_tmp1F4 <= (void*)2)goto _LL195;if(*((int*)_tmp1F4)!= 7)goto _LL195;_LL194: goto
_LL196;_LL195: if(_tmp1F4 <= (void*)2)goto _LL197;if(*((int*)_tmp1F4)!= 3)goto
_LL197;_LL196: goto _LL198;_LL197: if(_tmp1F4 <= (void*)2)goto _LL199;if(*((int*)
_tmp1F4)!= 10)goto _LL199;_LL198: goto _LL19A;_LL199: if(_tmp1F4 <= (void*)2)goto
_LL19B;if(*((int*)_tmp1F4)!= 11)goto _LL19B;_LL19A: goto _LL19C;_LL19B: if(_tmp1F4 <= (
void*)2)goto _LL19D;if(*((int*)_tmp1F4)!= 12)goto _LL19D;_LL19C: goto _LL19E;_LL19D:
if(_tmp1F4 <= (void*)2)goto _LL19F;if(*((int*)_tmp1F4)!= 13)goto _LL19F;_LL19E: goto
_LL1A0;_LL19F: if(_tmp1F4 <= (void*)2)goto _LL1A1;if(*((int*)_tmp1F4)!= 4)goto
_LL1A1;_LL1A0: goto _LL1A2;_LL1A1: if(_tmp1F4 <= (void*)2)goto _LL182;if(*((int*)
_tmp1F4)!= 5)goto _LL182;_LL1A2: name=0;goto _LL182;_LL182:;}if(name != 0  && ((int(*)(
struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp1F0,(struct
_dyneither_ptr*)name)){struct Cyc_List_List*_tmp548;_tmp1F1=((_tmp548=_cycalloc(
sizeof(*_tmp548)),((_tmp548->hd=_tmp1F3,((_tmp548->tl=_tmp1F1,_tmp548))))));}}}
if(!Cyc_do_setjmp){maybe=Cyc_fopen(filename,(const char*)"w");if(!((unsigned int)
maybe)){int _tmp211=1;_npop_handler(0);return _tmp211;}out_file=(struct Cyc___cycFILE*)
maybe;}else{out_file=Cyc_stdout;}{const char*_tmp54E;const char*_tmp54D;void*
_tmp54C[1];struct Cyc_String_pa_struct _tmp54B;struct _dyneither_ptr ifdefmacro=(
_tmp54B.tag=0,((_tmp54B.f1=(struct _dyneither_ptr)((_tmp54E=filename,
_tag_dyneither(_tmp54E,sizeof(char),_get_zero_arr_size_char(_tmp54E,1)))),((
_tmp54C[0]=& _tmp54B,Cyc_aprintf(((_tmp54D="_%s_",_tag_dyneither(_tmp54D,sizeof(
char),5))),_tag_dyneither(_tmp54C,sizeof(void*),1)))))));{int _tmp212=0;for(0;
_tmp212 < _get_dyneither_size(ifdefmacro,sizeof(char));++ _tmp212){if(((char*)
ifdefmacro.curr)[_tmp212]== '.'  || ((char*)ifdefmacro.curr)[_tmp212]== '/'){char
_tmp551;char _tmp550;struct _dyneither_ptr _tmp54F;(_tmp54F=_dyneither_ptr_plus(
ifdefmacro,sizeof(char),_tmp212),((_tmp550=*((char*)_check_dyneither_subscript(
_tmp54F,sizeof(char),0)),((_tmp551='_',((_get_dyneither_size(_tmp54F,sizeof(char))
== 1  && (_tmp550 == '\000'  && _tmp551 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp54F.curr)=_tmp551)))))));}else{if(((char*)ifdefmacro.curr)[_tmp212]!= '_'
 && ((char*)ifdefmacro.curr)[_tmp212]!= '/'){char _tmp554;char _tmp553;struct
_dyneither_ptr _tmp552;(_tmp552=_dyneither_ptr_plus(ifdefmacro,sizeof(char),
_tmp212),((_tmp553=*((char*)_check_dyneither_subscript(_tmp552,sizeof(char),0)),((
_tmp554=(char)toupper((int)((char*)ifdefmacro.curr)[_tmp212]),((
_get_dyneither_size(_tmp552,sizeof(char))== 1  && (_tmp553 == '\000'  && _tmp554 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp552.curr)=_tmp554)))))));}}}}{const char*
_tmp559;void*_tmp558[2];struct Cyc_String_pa_struct _tmp557;struct Cyc_String_pa_struct
_tmp556;(_tmp556.tag=0,((_tmp556.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
ifdefmacro),((_tmp557.tag=0,((_tmp557.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ifdefmacro),((_tmp558[0]=& _tmp557,((_tmp558[1]=& _tmp556,Cyc_fprintf(
out_file,((_tmp559="#ifndef %s\n#define %s\n",_tag_dyneither(_tmp559,sizeof(char),
23))),_tag_dyneither(_tmp558,sizeof(void*),2)))))))))))));}{struct Cyc_List_List*
_tmp21D=0;struct Cyc_List_List*_tmp21E=0;{struct Cyc_List_List*_tmp21F=_tmp1F1;
for(0;_tmp21F != 0;_tmp21F=_tmp21F->tl){struct Cyc_Absyn_Decl*_tmp220=(struct Cyc_Absyn_Decl*)
_tmp21F->hd;int _tmp221=0;struct _dyneither_ptr*name;{void*_tmp222=_tmp220->r;
struct Cyc_Absyn_Vardecl*_tmp223;struct Cyc_Absyn_Fndecl*_tmp224;struct Cyc_Absyn_Aggrdecl*
_tmp225;struct Cyc_Absyn_Enumdecl*_tmp226;struct Cyc_Absyn_Typedefdecl*_tmp227;
_LL1A4: if(_tmp222 <= (void*)2)goto _LL1B2;if(*((int*)_tmp222)!= 0)goto _LL1A6;
_tmp223=((struct Cyc_Absyn_Var_d_struct*)_tmp222)->f1;_LL1A5: {struct _tuple0
_tmp229;struct _dyneither_ptr*_tmp22A;struct _tuple0*_tmp228=_tmp223->name;_tmp229=*
_tmp228;_tmp22A=_tmp229.f2;name=(struct _dyneither_ptr*)_tmp22A;goto _LL1A3;}
_LL1A6: if(*((int*)_tmp222)!= 1)goto _LL1A8;_tmp224=((struct Cyc_Absyn_Fn_d_struct*)
_tmp222)->f1;_LL1A7: {struct _tuple0 _tmp22C;struct _dyneither_ptr*_tmp22D;struct
_tuple0*_tmp22B=_tmp224->name;_tmp22C=*_tmp22B;_tmp22D=_tmp22C.f2;name=(struct
_dyneither_ptr*)_tmp22D;goto _LL1A3;}_LL1A8: if(*((int*)_tmp222)!= 6)goto _LL1AA;
_tmp225=((struct Cyc_Absyn_Aggr_d_struct*)_tmp222)->f1;_LL1A9: {struct _tuple0
_tmp22F;struct _dyneither_ptr*_tmp230;struct _tuple0*_tmp22E=_tmp225->name;_tmp22F=*
_tmp22E;_tmp230=_tmp22F.f2;name=(struct _dyneither_ptr*)_tmp230;goto _LL1A3;}
_LL1AA: if(*((int*)_tmp222)!= 8)goto _LL1AC;_tmp226=((struct Cyc_Absyn_Enum_d_struct*)
_tmp222)->f1;_LL1AB: {struct _tuple0 _tmp232;struct _dyneither_ptr*_tmp233;struct
_tuple0*_tmp231=_tmp226->name;_tmp232=*_tmp231;_tmp233=_tmp232.f2;name=(struct
_dyneither_ptr*)_tmp233;goto _LL1A3;}_LL1AC: if(*((int*)_tmp222)!= 9)goto _LL1AE;
_tmp227=((struct Cyc_Absyn_Typedef_d_struct*)_tmp222)->f1;_LL1AD: {struct _tuple0
_tmp235;struct _dyneither_ptr*_tmp236;struct _tuple0*_tmp234=_tmp227->name;_tmp235=*
_tmp234;_tmp236=_tmp235.f2;name=(struct _dyneither_ptr*)_tmp236;goto _LL1A3;}
_LL1AE: if(*((int*)_tmp222)!= 4)goto _LL1B0;_LL1AF: goto _LL1B1;_LL1B0: if(*((int*)
_tmp222)!= 5)goto _LL1B2;_LL1B1: goto _LL1B3;_LL1B2: if((int)_tmp222 != 0)goto _LL1B4;
_LL1B3: goto _LL1B5;_LL1B4: if((int)_tmp222 != 1)goto _LL1B6;_LL1B5: goto _LL1B7;_LL1B6:
if(_tmp222 <= (void*)2)goto _LL1B8;if(*((int*)_tmp222)!= 2)goto _LL1B8;_LL1B7: goto
_LL1B9;_LL1B8: if(_tmp222 <= (void*)2)goto _LL1BA;if(*((int*)_tmp222)!= 7)goto
_LL1BA;_LL1B9: goto _LL1BB;_LL1BA: if(_tmp222 <= (void*)2)goto _LL1BC;if(*((int*)
_tmp222)!= 3)goto _LL1BC;_LL1BB: goto _LL1BD;_LL1BC: if(_tmp222 <= (void*)2)goto
_LL1BE;if(*((int*)_tmp222)!= 10)goto _LL1BE;_LL1BD: goto _LL1BF;_LL1BE: if(_tmp222 <= (
void*)2)goto _LL1C0;if(*((int*)_tmp222)!= 11)goto _LL1C0;_LL1BF: goto _LL1C1;_LL1C0:
if(_tmp222 <= (void*)2)goto _LL1C2;if(*((int*)_tmp222)!= 12)goto _LL1C2;_LL1C1: goto
_LL1C3;_LL1C2: if(_tmp222 <= (void*)2)goto _LL1A3;if(*((int*)_tmp222)!= 13)goto
_LL1A3;_LL1C3: name=0;goto _LL1A3;_LL1A3:;}if(!((unsigned int)name) && !_tmp221)
continue;{struct Cyc_List_List*_tmp55A;_tmp21D=((_tmp55A=_cycalloc(sizeof(*
_tmp55A)),((_tmp55A->hd=_tmp220,((_tmp55A->tl=_tmp21D,_tmp55A))))));}{struct Cyc_List_List*
_tmp55B;_tmp21E=((_tmp55B=_cycalloc(sizeof(*_tmp55B)),((_tmp55B->hd=name,((
_tmp55B->tl=_tmp21E,_tmp55B))))));}}}{struct _RegionHandle _tmp239=_new_region("tc_rgn");
struct _RegionHandle*tc_rgn=& _tmp239;_push_region(tc_rgn);{struct Cyc_Tcenv_Tenv*
_tmp23A=Cyc_Tcenv_tc_init(tc_rgn);Cyc_Tc_tc(tc_rgn,_tmp23A,1,_tmp21D);}{struct
Cyc_List_List*_tmp23D;struct Cyc_List_List*_tmp23E;struct _tuple23 _tmp55C;struct
_tuple23 _tmp23C=(_tmp55C.f1=_tmp21D,((_tmp55C.f2=_tmp21E,_tmp55C)));_tmp23D=
_tmp23C.f1;_tmp23E=_tmp23C.f2;for(0;_tmp23D != 0  && _tmp23E != 0;(_tmp23D=_tmp23D->tl,
_tmp23E=_tmp23E->tl)){struct Cyc_Absyn_Decl*_tmp23F=(struct Cyc_Absyn_Decl*)
_tmp23D->hd;struct _dyneither_ptr*_tmp240=(struct _dyneither_ptr*)_tmp23E->hd;int
_tmp241=0;if(!((unsigned int)_tmp240))_tmp241=1;if((unsigned int)_tmp240){{const
char*_tmp560;void*_tmp55F[1];struct Cyc_String_pa_struct _tmp55E;ifdefmacro=((
_tmp55E.tag=0,((_tmp55E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp240),((
_tmp55F[0]=& _tmp55E,Cyc_aprintf(((_tmp560="_%s_def_",_tag_dyneither(_tmp560,
sizeof(char),9))),_tag_dyneither(_tmp55F,sizeof(void*),1))))))));}{const char*
_tmp564;void*_tmp563[1];struct Cyc_String_pa_struct _tmp562;(_tmp562.tag=0,((
_tmp562.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp563[0]=&
_tmp562,Cyc_fprintf(out_file,((_tmp564="#ifndef %s\n",_tag_dyneither(_tmp564,
sizeof(char),12))),_tag_dyneither(_tmp563,sizeof(void*),1)))))));}{const char*
_tmp568;void*_tmp567[1];struct Cyc_String_pa_struct _tmp566;(_tmp566.tag=0,((
_tmp566.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp567[0]=&
_tmp566,Cyc_fprintf(out_file,((_tmp568="#define %s\n",_tag_dyneither(_tmp568,
sizeof(char),12))),_tag_dyneither(_tmp567,sizeof(void*),1)))))));}{struct Cyc_Absyn_Decl*
_tmp569[1];Cyc_Absynpp_decllist2file(((_tmp569[0]=_tmp23F,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp569,sizeof(struct Cyc_Absyn_Decl*),
1)))),out_file);}{const char*_tmp56C;void*_tmp56B;(_tmp56B=0,Cyc_fprintf(out_file,((
_tmp56C="#endif\n",_tag_dyneither(_tmp56C,sizeof(char),8))),_tag_dyneither(
_tmp56B,sizeof(void*),0)));}}else{struct Cyc_Absyn_Decl*_tmp56D[1];Cyc_Absynpp_decllist2file(((
_tmp56D[0]=_tmp23F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp56D,sizeof(struct Cyc_Absyn_Decl*),1)))),out_file);}}}maybe=
Cyc_fopen(_tmp190,(const char*)"r");if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct
_tmp57E;const char*_tmp57D;const char*_tmp57C;void*_tmp57B[1];struct Cyc_String_pa_struct
_tmp57A;struct Cyc_NO_SUPPORT_struct*_tmp579;(int)_throw((void*)((_tmp579=
_cycalloc(sizeof(*_tmp579)),((_tmp579[0]=((_tmp57E.tag=Cyc_NO_SUPPORT,((_tmp57E.f1=(
struct _dyneither_ptr)((_tmp57A.tag=0,((_tmp57A.f1=(struct _dyneither_ptr)((
_tmp57D=_tmp190,_tag_dyneither(_tmp57D,sizeof(char),_get_zero_arr_size_char(
_tmp57D,1)))),((_tmp57B[0]=& _tmp57A,Cyc_aprintf(((_tmp57C="can't open macrosfile %s",
_tag_dyneither(_tmp57C,sizeof(char),25))),_tag_dyneither(_tmp57B,sizeof(void*),1)))))))),
_tmp57E)))),_tmp579)))));}in_file=(struct Cyc___cycFILE*)maybe;_tmp1D6=Cyc_Lexing_from_file(
in_file);{struct _tuple18*entry2;while((entry2=((struct _tuple18*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_suck_line)(_tmp1D6))!= 0){struct _tuple18 _tmp256;struct _dyneither_ptr
_tmp257;struct _dyneither_ptr*_tmp258;struct _tuple18*_tmp255=(struct _tuple18*)
_check_null(entry2);_tmp256=*_tmp255;_tmp257=_tmp256.f1;_tmp258=_tmp256.f2;if(((
int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp1F0,
_tmp258)){{const char*_tmp582;void*_tmp581[1];struct Cyc_String_pa_struct _tmp580;(
_tmp580.tag=0,((_tmp580.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp258),((
_tmp581[0]=& _tmp580,Cyc_fprintf(out_file,((_tmp582="#ifndef %s\n",_tag_dyneither(
_tmp582,sizeof(char),12))),_tag_dyneither(_tmp581,sizeof(void*),1)))))));}{const
char*_tmp586;void*_tmp585[1];struct Cyc_String_pa_struct _tmp584;(_tmp584.tag=0,((
_tmp584.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp257),((_tmp585[0]=&
_tmp584,Cyc_fprintf(out_file,((_tmp586="%s\n",_tag_dyneither(_tmp586,sizeof(char),
4))),_tag_dyneither(_tmp585,sizeof(void*),1)))))));}{const char*_tmp589;void*
_tmp588;(_tmp588=0,Cyc_fprintf(out_file,((_tmp589="#endif\n",_tag_dyneither(
_tmp589,sizeof(char),8))),_tag_dyneither(_tmp588,sizeof(void*),0)));}}}Cyc_fclose(
in_file);if(Cyc_mode != (void*)3)remove(_tmp190);if(hstubs != 0){struct Cyc_List_List*
_tmp261=hstubs;for(0;_tmp261 != 0;_tmp261=_tmp261->tl){struct _tuple19 _tmp263;
struct _dyneither_ptr _tmp264;struct _dyneither_ptr _tmp265;struct _tuple19*_tmp262=(
struct _tuple19*)_tmp261->hd;_tmp263=*_tmp262;_tmp264=_tmp263.f1;_tmp265=_tmp263.f2;{
struct _dyneither_ptr*_tmp58A;if(_tmp265.curr != (_tag_dyneither(0,0,0)).curr  && (
_tmp264.curr == (_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,
struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp58A=_cycalloc(
sizeof(*_tmp58A)),((_tmp58A[0]=_tmp264,_tmp58A)))))))Cyc_fputs((const char*)
_untag_dyneither_ptr(_tmp265,sizeof(char),1),out_file);else{const char*_tmp58E;
void*_tmp58D[1];struct Cyc_String_pa_struct _tmp58C;(_tmp58C.tag=0,((_tmp58C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp264),((_tmp58D[0]=& _tmp58C,Cyc_log(((
_tmp58E="%s is not supported on this platform\n",_tag_dyneither(_tmp58E,sizeof(
char),38))),_tag_dyneither(_tmp58D,sizeof(void*),1)))))));}}}}{const char*_tmp591;
void*_tmp590;(_tmp590=0,Cyc_fprintf(out_file,((_tmp591="#endif\n",_tag_dyneither(
_tmp591,sizeof(char),8))),_tag_dyneither(_tmp590,sizeof(void*),0)));}if(Cyc_do_setjmp){
int _tmp26C=0;_npop_handler(1);return _tmp26C;}else{Cyc_fclose(out_file);}if(
cstubs != 0){out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{struct
Cyc_List_List*_tmp26D=cstubs;for(0;_tmp26D != 0;_tmp26D=_tmp26D->tl){struct
_tuple19 _tmp26F;struct _dyneither_ptr _tmp270;struct _dyneither_ptr _tmp271;struct
_tuple19*_tmp26E=(struct _tuple19*)_tmp26D->hd;_tmp26F=*_tmp26E;_tmp270=_tmp26F.f1;
_tmp271=_tmp26F.f2;{struct _dyneither_ptr*_tmp592;if(_tmp271.curr != (
_tag_dyneither(0,0,0)).curr  && (_tmp270.curr == (_tag_dyneither(0,0,0)).curr  || ((
int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
defined_symbols,((_tmp592=_cycalloc(sizeof(*_tmp592)),((_tmp592[0]=_tmp270,
_tmp592)))))))Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp271,sizeof(char),1),
out_file);}}}}out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);{
const char*_tmp598;const char*_tmp597;void*_tmp596[1];struct Cyc_String_pa_struct
_tmp595;(_tmp595.tag=0,((_tmp595.f1=(struct _dyneither_ptr)((_tmp598=filename,
_tag_dyneither(_tmp598,sizeof(char),_get_zero_arr_size_char(_tmp598,1)))),((
_tmp596[0]=& _tmp595,Cyc_fprintf(out_file,((_tmp597="#include <%s>\n\n",
_tag_dyneither(_tmp597,sizeof(char),16))),_tag_dyneither(_tmp596,sizeof(void*),1)))))));}
if(cycstubs != 0){out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);{
struct Cyc_List_List*_tmp277=cycstubs;for(0;_tmp277 != 0;_tmp277=_tmp277->tl){
struct _tuple19 _tmp279;struct _dyneither_ptr _tmp27A;struct _dyneither_ptr _tmp27B;
struct _tuple19*_tmp278=(struct _tuple19*)_tmp277->hd;_tmp279=*_tmp278;_tmp27A=
_tmp279.f1;_tmp27B=_tmp279.f2;{struct _dyneither_ptr*_tmp599;if(_tmp27B.curr != (
_tag_dyneither(0,0,0)).curr  && (_tmp27A.curr == (_tag_dyneither(0,0,0)).curr  || ((
int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
defined_symbols,((_tmp599=_cycalloc(sizeof(*_tmp599)),((_tmp599[0]=_tmp27A,
_tmp599)))))))Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp27B,sizeof(char),1),
out_file);}}}{const char*_tmp59C;void*_tmp59B;(_tmp59B=0,Cyc_fprintf(out_file,((
_tmp59C="\n",_tag_dyneither(_tmp59C,sizeof(char),2))),_tag_dyneither(_tmp59B,
sizeof(void*),0)));}}{int _tmp27F=0;_npop_handler(1);return _tmp27F;}};_pop_region(
tc_rgn);}}}}}}};_pop_handler();}else{void*_tmp194=(void*)_exn_thrown;void*
_tmp285=_tmp194;struct _dyneither_ptr _tmp286;_LL1C5: if(*((void**)_tmp285)!= Cyc_NO_SUPPORT)
goto _LL1C7;_tmp286=((struct Cyc_NO_SUPPORT_struct*)_tmp285)->f1;_LL1C6:{const char*
_tmp5A0;void*_tmp59F[1];struct Cyc_String_pa_struct _tmp59E;(_tmp59E.tag=0,((
_tmp59E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp286),((_tmp59F[0]=&
_tmp59E,Cyc_fprintf(Cyc_stderr,((_tmp5A0="No support because %s\n",
_tag_dyneither(_tmp5A0,sizeof(char),23))),_tag_dyneither(_tmp59F,sizeof(void*),1)))))));}
goto _LL1C8;_LL1C7:;_LL1C8: maybe=Cyc_fopen(filename,(const char*)"w");if(!((
unsigned int)maybe)){{const char*_tmp5A6;const char*_tmp5A5;void*_tmp5A4[1];struct
Cyc_String_pa_struct _tmp5A3;(_tmp5A3.tag=0,((_tmp5A3.f1=(struct _dyneither_ptr)((
_tmp5A6=filename,_tag_dyneither(_tmp5A6,sizeof(char),_get_zero_arr_size_char(
_tmp5A6,1)))),((_tmp5A4[0]=& _tmp5A3,Cyc_fprintf(Cyc_stderr,((_tmp5A5="Error: could not create file %s\n",
_tag_dyneither(_tmp5A5,sizeof(char),33))),_tag_dyneither(_tmp5A4,sizeof(void*),1)))))));}
return 1;}out_file=(struct Cyc___cycFILE*)maybe;{const char*_tmp5AC;const char*
_tmp5AB;void*_tmp5AA[1];struct Cyc_String_pa_struct _tmp5A9;(_tmp5A9.tag=0,((
_tmp5A9.f1=(struct _dyneither_ptr)((_tmp5AC=filename,_tag_dyneither(_tmp5AC,
sizeof(char),_get_zero_arr_size_char(_tmp5AC,1)))),((_tmp5AA[0]=& _tmp5A9,Cyc_fprintf(
out_file,((_tmp5AB="#error -- %s is not supported on this platform\n",
_tag_dyneither(_tmp5AB,sizeof(char),48))),_tag_dyneither(_tmp5AA,sizeof(void*),1)))))));}
Cyc_fclose(out_file);{const char*_tmp5B2;const char*_tmp5B1;void*_tmp5B0[1];struct
Cyc_String_pa_struct _tmp5AF;(_tmp5AF.tag=0,((_tmp5AF.f1=(struct _dyneither_ptr)((
_tmp5B2=filename,_tag_dyneither(_tmp5B2,sizeof(char),_get_zero_arr_size_char(
_tmp5B2,1)))),((_tmp5B0[0]=& _tmp5AF,Cyc_fprintf(Cyc_stderr,((_tmp5B1="Warning: %s will not be supported on this platform\n",
_tag_dyneither(_tmp5B1,sizeof(char),52))),_tag_dyneither(_tmp5B0,sizeof(void*),1)))))));}{
const char*_tmp5B5;void*_tmp5B4;(_tmp5B4=0,Cyc_log(((_tmp5B5="Not supported on this platform\n",
_tag_dyneither(_tmp5B5,sizeof(char),32))),_tag_dyneither(_tmp5B4,sizeof(void*),0)));}
remove(_tmp190);remove(_tmp191);remove(_tmp192);return 0;_LL1C9:;_LL1CA:(void)
_throw(_tmp285);_LL1C4:;}}}}int Cyc_process_specfile(const char*file,const char*dir);
static void _tmp5C0(unsigned int*_tmp5BF,unsigned int*_tmp5BE,char**_tmp5BD){for(*
_tmp5BF=0;*_tmp5BF < *_tmp5BE;(*_tmp5BF)++){(*_tmp5BD)[*_tmp5BF]='\000';}}int Cyc_process_specfile(
const char*file,const char*dir){struct Cyc___cycFILE*_tmp2AD=Cyc_fopen(file,(const
char*)"r");if(!((unsigned int)_tmp2AD)){{const char*_tmp5BB;const char*_tmp5BA;
void*_tmp5B9[1];struct Cyc_String_pa_struct _tmp5B8;(_tmp5B8.tag=0,((_tmp5B8.f1=(
struct _dyneither_ptr)((_tmp5BB=file,_tag_dyneither(_tmp5BB,sizeof(char),
_get_zero_arr_size_char(_tmp5BB,1)))),((_tmp5B9[0]=& _tmp5B8,Cyc_fprintf(Cyc_stderr,((
_tmp5BA="Error: could not open %s\n",_tag_dyneither(_tmp5BA,sizeof(char),26))),
_tag_dyneither(_tmp5B9,sizeof(void*),1)))))));}return 1;}{struct Cyc___cycFILE*
_tmp2B2=(struct Cyc___cycFILE*)_tmp2AD;char*_tmp5C5;unsigned int _tmp5C4;char*
_tmp5C3;unsigned int _tmp5C2;unsigned int _tmp5C1;struct _dyneither_ptr buf=(_tmp5C5=((
_tmp5C4=(unsigned int)1024,((_tmp5C3=(char*)_cycalloc_atomic(_check_times(
sizeof(char),_tmp5C4 + 1)),((((_tmp5C2=_tmp5C4,((_tmp5C0(& _tmp5C1,& _tmp5C2,&
_tmp5C3),_tmp5C3[_tmp5C2]=(char)0)))),_tmp5C3)))))),_tag_dyneither(_tmp5C5,
sizeof(char),_get_zero_arr_size_char(_tmp5C5,(unsigned int)1024 + 1)));struct
_dyneither_ptr _tmp2B3=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));if(
Cyc_mode != (void*)2){if(chdir(dir)){{const char*_tmp5CB;const char*_tmp5CA;void*
_tmp5C9[1];struct Cyc_String_pa_struct _tmp5C8;(_tmp5C8.tag=0,((_tmp5C8.f1=(struct
_dyneither_ptr)((_tmp5CB=dir,_tag_dyneither(_tmp5CB,sizeof(char),
_get_zero_arr_size_char(_tmp5CB,1)))),((_tmp5C9[0]=& _tmp5C8,Cyc_fprintf(Cyc_stderr,((
_tmp5CA="Error: can't change directory to %s\n",_tag_dyneither(_tmp5CA,sizeof(
char),37))),_tag_dyneither(_tmp5C9,sizeof(void*),1)))))));}return 1;}}if(Cyc_mode
== (void*)1){struct _dyneither_ptr _tmp2B8=Cstring_to_string(Ccomp);const char*
_tmp5CF;void*_tmp5CE[1];struct Cyc_String_pa_struct _tmp5CD;system((const char*)
_untag_dyneither_ptr(((_tmp5CD.tag=0,((_tmp5CD.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp2B8),((_tmp5CE[0]=& _tmp5CD,Cyc_aprintf(((_tmp5CF="echo | %s -E -dM - -o INITMACROS.h\n",
_tag_dyneither(_tmp5CF,sizeof(char),36))),_tag_dyneither(_tmp5CE,sizeof(void*),1)))))))),
sizeof(char),1));}{struct Cyc_Lexing_lexbuf*_tmp2BC=Cyc_Lexing_from_file(_tmp2B2);
struct _tuple20*entry;while((entry=((struct _tuple20*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_spec)(_tmp2BC))!= 0){struct _tuple20 _tmp2BE;struct _dyneither_ptr
_tmp2BF;struct Cyc_List_List*_tmp2C0;struct Cyc_List_List*_tmp2C1;struct Cyc_List_List*
_tmp2C2;struct Cyc_List_List*_tmp2C3;struct Cyc_List_List*_tmp2C4;struct _tuple20*
_tmp2BD=(struct _tuple20*)_check_null(entry);_tmp2BE=*_tmp2BD;_tmp2BF=_tmp2BE.f1;
_tmp2C0=_tmp2BE.f2;_tmp2C1=_tmp2BE.f3;_tmp2C2=_tmp2BE.f4;_tmp2C3=_tmp2BE.f5;
_tmp2C4=_tmp2BE.f6;if(Cyc_process_file((const char*)_untag_dyneither_ptr(_tmp2BF,
sizeof(char),1),_tmp2C0,_tmp2C1,_tmp2C2,_tmp2C3,_tmp2C4))return 1;}Cyc_fclose(
_tmp2B2);if(Cyc_mode != (void*)2){if(chdir((const char*)((char*)
_untag_dyneither_ptr(_tmp2B3,sizeof(char),1)))){{const char*_tmp5D3;void*_tmp5D2[
1];struct Cyc_String_pa_struct _tmp5D1;(_tmp5D1.tag=0,((_tmp5D1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp2B3),((_tmp5D2[0]=& _tmp5D1,Cyc_fprintf(
Cyc_stderr,((_tmp5D3="Error: could not change directory to %s\n",_tag_dyneither(
_tmp5D3,sizeof(char),41))),_tag_dyneither(_tmp5D2,sizeof(void*),1)))))));}return
1;}}return 0;}}}int Cyc_process_setjmp(const char*dir);static void _tmp5D8(
unsigned int*_tmp5D7,unsigned int*_tmp5D6,char**_tmp5D5){for(*_tmp5D7=0;*_tmp5D7
< *_tmp5D6;(*_tmp5D7)++){(*_tmp5D5)[*_tmp5D7]='\000';}}int Cyc_process_setjmp(
const char*dir){char*_tmp5DD;unsigned int _tmp5DC;char*_tmp5DB;unsigned int _tmp5DA;
unsigned int _tmp5D9;struct _dyneither_ptr buf=(_tmp5DD=((_tmp5DC=(unsigned int)
1024,((_tmp5DB=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp5DC + 1)),((((
_tmp5DA=_tmp5DC,((_tmp5D8(& _tmp5D9,& _tmp5DA,& _tmp5DB),_tmp5DB[_tmp5DA]=(char)0)))),
_tmp5DB)))))),_tag_dyneither(_tmp5DD,sizeof(char),_get_zero_arr_size_char(
_tmp5DD,(unsigned int)1024 + 1)));struct _dyneither_ptr _tmp2CC=Cyc_getcwd(buf,
_get_dyneither_size(buf,sizeof(char)));if(chdir(dir)){{const char*_tmp5E3;const
char*_tmp5E2;void*_tmp5E1[1];struct Cyc_String_pa_struct _tmp5E0;(_tmp5E0.tag=0,((
_tmp5E0.f1=(struct _dyneither_ptr)((_tmp5E3=dir,_tag_dyneither(_tmp5E3,sizeof(
char),_get_zero_arr_size_char(_tmp5E3,1)))),((_tmp5E1[0]=& _tmp5E0,Cyc_fprintf(
Cyc_stderr,((_tmp5E2="Error: can't change directory to %s\n",_tag_dyneither(
_tmp5E2,sizeof(char),37))),_tag_dyneither(_tmp5E1,sizeof(void*),1)))))));}return
1;}{struct _tuple19*_tmp5ED;const char*_tmp5EC;const char*_tmp5EB;struct _tuple19*
_tmp5EA[1];struct _dyneither_ptr*_tmp5E4[1];if(Cyc_process_file((const char*)"setjmp.h",((
_tmp5E4[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"jmp_buf",
sizeof(char),8),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp5E4,sizeof(struct _dyneither_ptr*),1)))),0,((_tmp5EA[0]=((
_tmp5ED=_cycalloc(sizeof(*_tmp5ED)),((_tmp5ED->f1=((_tmp5EB="setjmp",
_tag_dyneither(_tmp5EB,sizeof(char),7))),((_tmp5ED->f2=((_tmp5EC="extern int setjmp(jmp_buf);\n",
_tag_dyneither(_tmp5EC,sizeof(char),29))),_tmp5ED)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5EA,sizeof(struct
_tuple19*),1)))),0,0))return 1;}if(chdir((const char*)((char*)_untag_dyneither_ptr(
_tmp2CC,sizeof(char),1)))){{const char*_tmp5F1;void*_tmp5F0[1];struct Cyc_String_pa_struct
_tmp5EF;(_tmp5EF.tag=0,((_tmp5EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp2CC),((_tmp5F0[0]=& _tmp5EF,Cyc_fprintf(Cyc_stderr,((_tmp5F1="Error: could not change directory to %s\n",
_tag_dyneither(_tmp5F1,sizeof(char),41))),_tag_dyneither(_tmp5F0,sizeof(void*),1)))))));}
return 1;}return 0;}static char _tmp2DE[13]="BUILDLIB.OUT";static struct
_dyneither_ptr Cyc_output_dir={_tmp2DE,_tmp2DE,_tmp2DE + 13};static void Cyc_set_output_dir(
struct _dyneither_ptr s);static void Cyc_set_output_dir(struct _dyneither_ptr s){Cyc_output_dir=
s;}static struct Cyc_List_List*Cyc_spec_files=0;static void Cyc_add_spec_file(struct
_dyneither_ptr s);static void Cyc_add_spec_file(struct _dyneither_ptr s){struct Cyc_List_List*
_tmp5F2;Cyc_spec_files=((_tmp5F2=_cycalloc(sizeof(*_tmp5F2)),((_tmp5F2->hd=(
const char*)_untag_dyneither_ptr(s,sizeof(char),1),((_tmp5F2->tl=Cyc_spec_files,
_tmp5F2))))));}static void Cyc_set_GATHER();static void Cyc_set_GATHER(){Cyc_mode=(
void*)1;}static void Cyc_set_GATHERSCRIPT();static void Cyc_set_GATHERSCRIPT(){Cyc_mode=(
void*)2;}static void Cyc_set_FINISH();static void Cyc_set_FINISH(){Cyc_mode=(void*)3;}
static int Cyc_badparse=0;static void Cyc_unsupported_option(struct _dyneither_ptr s);
static void Cyc_unsupported_option(struct _dyneither_ptr s){{const char*_tmp5F6;void*
_tmp5F5[1];struct Cyc_String_pa_struct _tmp5F4;(_tmp5F4.tag=0,((_tmp5F4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)s),((_tmp5F5[0]=& _tmp5F4,Cyc_fprintf(Cyc_stderr,((
_tmp5F6="Unsupported option %s\n",_tag_dyneither(_tmp5F6,sizeof(char),23))),
_tag_dyneither(_tmp5F5,sizeof(void*),1)))))));}Cyc_badparse=1;}void
GC_blacklist_warn_clear();struct _tuple24{struct _dyneither_ptr f1;int f2;struct
_dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};int Cyc_main(int argc,struct
_dyneither_ptr argv);int Cyc_main(int argc,struct _dyneither_ptr argv){
GC_blacklist_warn_clear();{struct _tuple24*_tmp663;const char*_tmp662;struct Cyc_Arg_Flag_spec_struct
_tmp661;struct Cyc_Arg_Flag_spec_struct*_tmp660;const char*_tmp65F;const char*
_tmp65E;struct _tuple24*_tmp65D;const char*_tmp65C;struct Cyc_Arg_Set_spec_struct
_tmp65B;struct Cyc_Arg_Set_spec_struct*_tmp65A;const char*_tmp659;const char*
_tmp658;struct _tuple24*_tmp657;const char*_tmp656;struct Cyc_Arg_Unit_spec_struct
_tmp655;struct Cyc_Arg_Unit_spec_struct*_tmp654;const char*_tmp653;const char*
_tmp652;struct _tuple24*_tmp651;const char*_tmp650;struct Cyc_Arg_Unit_spec_struct
_tmp64F;struct Cyc_Arg_Unit_spec_struct*_tmp64E;const char*_tmp64D;const char*
_tmp64C;struct _tuple24*_tmp64B;const char*_tmp64A;struct Cyc_Arg_Unit_spec_struct
_tmp649;struct Cyc_Arg_Unit_spec_struct*_tmp648;const char*_tmp647;const char*
_tmp646;struct _tuple24*_tmp645;const char*_tmp644;struct Cyc_Arg_String_spec_struct
_tmp643;struct Cyc_Arg_String_spec_struct*_tmp642;const char*_tmp641;const char*
_tmp640;struct _tuple24*_tmp63F[6];struct Cyc_List_List*options=(_tmp63F[5]=((
_tmp663=_cycalloc(sizeof(*_tmp663)),((_tmp663->f1=((_tmp65E="-",_tag_dyneither(
_tmp65E,sizeof(char),2))),((_tmp663->f2=1,((_tmp663->f3=((_tmp65F="",
_tag_dyneither(_tmp65F,sizeof(char),1))),((_tmp663->f4=(void*)((_tmp660=
_cycalloc(sizeof(*_tmp660)),((_tmp660[0]=((_tmp661.tag=1,((_tmp661.f1=Cyc_unsupported_option,
_tmp661)))),_tmp660)))),((_tmp663->f5=((_tmp662="",_tag_dyneither(_tmp662,
sizeof(char),1))),_tmp663)))))))))))),((_tmp63F[4]=((_tmp65D=_cycalloc(sizeof(*
_tmp65D)),((_tmp65D->f1=((_tmp658="-setjmp",_tag_dyneither(_tmp658,sizeof(char),
8))),((_tmp65D->f2=0,((_tmp65D->f3=((_tmp659="",_tag_dyneither(_tmp659,sizeof(
char),1))),((_tmp65D->f4=(void*)((_tmp65A=_cycalloc(sizeof(*_tmp65A)),((_tmp65A[
0]=((_tmp65B.tag=3,((_tmp65B.f1=& Cyc_do_setjmp,_tmp65B)))),_tmp65A)))),((_tmp65D->f5=((
_tmp65C="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.",
_tag_dyneither(_tmp65C,sizeof(char),186))),_tmp65D)))))))))))),((_tmp63F[3]=((
_tmp657=_cycalloc(sizeof(*_tmp657)),((_tmp657->f1=((_tmp652="-finish",
_tag_dyneither(_tmp652,sizeof(char),8))),((_tmp657->f2=0,((_tmp657->f3=((_tmp653="",
_tag_dyneither(_tmp653,sizeof(char),1))),((_tmp657->f4=(void*)((_tmp654=
_cycalloc(sizeof(*_tmp654)),((_tmp654[0]=((_tmp655.tag=0,((_tmp655.f1=Cyc_set_FINISH,
_tmp655)))),_tmp654)))),((_tmp657->f5=((_tmp656="Produce Cyclone headers from pre-gathered C library info",
_tag_dyneither(_tmp656,sizeof(char),57))),_tmp657)))))))))))),((_tmp63F[2]=((
_tmp651=_cycalloc(sizeof(*_tmp651)),((_tmp651->f1=((_tmp64C="-gatherscript",
_tag_dyneither(_tmp64C,sizeof(char),14))),((_tmp651->f2=0,((_tmp651->f3=((
_tmp64D="",_tag_dyneither(_tmp64D,sizeof(char),1))),((_tmp651->f4=(void*)((
_tmp64E=_cycalloc(sizeof(*_tmp64E)),((_tmp64E[0]=((_tmp64F.tag=0,((_tmp64F.f1=
Cyc_set_GATHERSCRIPT,_tmp64F)))),_tmp64E)))),((_tmp651->f5=((_tmp650="Produce a script to gather C library info",
_tag_dyneither(_tmp650,sizeof(char),42))),_tmp651)))))))))))),((_tmp63F[1]=((
_tmp64B=_cycalloc(sizeof(*_tmp64B)),((_tmp64B->f1=((_tmp646="-gather",
_tag_dyneither(_tmp646,sizeof(char),8))),((_tmp64B->f2=0,((_tmp64B->f3=((_tmp647="",
_tag_dyneither(_tmp647,sizeof(char),1))),((_tmp64B->f4=(void*)((_tmp648=
_cycalloc(sizeof(*_tmp648)),((_tmp648[0]=((_tmp649.tag=0,((_tmp649.f1=Cyc_set_GATHER,
_tmp649)))),_tmp648)))),((_tmp64B->f5=((_tmp64A="Gather C library info but don't produce Cyclone headers",
_tag_dyneither(_tmp64A,sizeof(char),56))),_tmp64B)))))))))))),((_tmp63F[0]=((
_tmp645=_cycalloc(sizeof(*_tmp645)),((_tmp645->f1=((_tmp640="-d",_tag_dyneither(
_tmp640,sizeof(char),3))),((_tmp645->f2=0,((_tmp645->f3=((_tmp641=" <file>",
_tag_dyneither(_tmp641,sizeof(char),8))),((_tmp645->f4=(void*)((_tmp642=
_cycalloc(sizeof(*_tmp642)),((_tmp642[0]=((_tmp643.tag=5,((_tmp643.f1=Cyc_set_output_dir,
_tmp643)))),_tmp642)))),((_tmp645->f5=((_tmp644="Set the output directory to <file>",
_tag_dyneither(_tmp644,sizeof(char),35))),_tmp645)))))))))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp63F,sizeof(struct
_tuple24*),6)))))))))))));{const char*_tmp664;Cyc_Arg_parse(options,Cyc_add_spec_file,((
_tmp664="Options:",_tag_dyneither(_tmp664,sizeof(char),9))),argv);}if((((Cyc_badparse
 || !Cyc_do_setjmp  && Cyc_spec_files == 0) || Cyc_do_setjmp  && Cyc_spec_files != 0)
 || Cyc_do_setjmp  && Cyc_mode == (void*)1) || Cyc_do_setjmp  && Cyc_mode == (void*)
2){{const char*_tmp665;Cyc_Arg_usage(options,((_tmp665="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:",
_tag_dyneither(_tmp665,sizeof(char),59))));}return 1;}if(Cyc_mode == (void*)2){Cyc_script_file=
Cyc_fopen((const char*)"BUILDLIB.sh",(const char*)"w");if(!((unsigned int)Cyc_script_file)){{
const char*_tmp668;void*_tmp667;(_tmp667=0,Cyc_fprintf(Cyc_stderr,((_tmp668="Could not create file BUILDLIB.sh\n",
_tag_dyneither(_tmp668,sizeof(char),35))),_tag_dyneither(_tmp667,sizeof(void*),0)));}
exit(1);}{const char*_tmp66B;void*_tmp66A;(_tmp66A=0,Cyc_prscript(((_tmp66B="#!/bin/sh\n",
_tag_dyneither(_tmp66B,sizeof(char),11))),_tag_dyneither(_tmp66A,sizeof(void*),0)));}{
const char*_tmp66E;void*_tmp66D;(_tmp66D=0,Cyc_prscript(((_tmp66E="GCC=\"gcc\"\n",
_tag_dyneither(_tmp66E,sizeof(char),11))),_tag_dyneither(_tmp66D,sizeof(void*),0)));}}
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){{
const char*_tmp672;void*_tmp671[1];struct Cyc_String_pa_struct _tmp670;(_tmp670.tag=
0,((_tmp670.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((
_tmp671[0]=& _tmp670,Cyc_fprintf(Cyc_stderr,((_tmp672="Error: could not create directory %s\n",
_tag_dyneither(_tmp672,sizeof(char),38))),_tag_dyneither(_tmp671,sizeof(void*),1)))))));}
return 1;}if(Cyc_mode == (void*)2){{const char*_tmp676;void*_tmp675[1];struct Cyc_String_pa_struct
_tmp674;(_tmp674.tag=0,((_tmp674.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_output_dir),((_tmp675[0]=& _tmp674,Cyc_prscript(((_tmp676="cd %s\n",
_tag_dyneither(_tmp676,sizeof(char),7))),_tag_dyneither(_tmp675,sizeof(void*),1)))))));}{
const char*_tmp679;void*_tmp678;(_tmp678=0,Cyc_prscript(((_tmp679="echo | $GCC -E -dM - -o INITMACROS.h\n",
_tag_dyneither(_tmp679,sizeof(char),38))),_tag_dyneither(_tmp678,sizeof(void*),0)));}}
if(!Cyc_gathering()){{const char*_tmp67A;Cyc_log_file=Cyc_fopen((const char*)
_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp67A="BUILDLIB.LOG",
_tag_dyneither(_tmp67A,sizeof(char),13)))),sizeof(char),1),(const char*)"w");}if(
!((unsigned int)Cyc_log_file)){{const char*_tmp67E;void*_tmp67D[1];struct Cyc_String_pa_struct
_tmp67C;(_tmp67C.tag=0,((_tmp67C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_output_dir),((_tmp67D[0]=& _tmp67C,Cyc_fprintf(Cyc_stderr,((_tmp67E="Error: could not create log file in directory %s\n",
_tag_dyneither(_tmp67E,sizeof(char),50))),_tag_dyneither(_tmp67D,sizeof(void*),1)))))));}
return 1;}if(!Cyc_do_setjmp){{const char*_tmp67F;Cyc_cstubs_file=Cyc_fopen((const
char*)_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp67F="cstubs.c",
_tag_dyneither(_tmp67F,sizeof(char),9)))),sizeof(char),1),(const char*)"w");}if(!((
unsigned int)Cyc_cstubs_file)){{const char*_tmp683;void*_tmp682[1];struct Cyc_String_pa_struct
_tmp681;(_tmp681.tag=0,((_tmp681.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_output_dir),((_tmp682[0]=& _tmp681,Cyc_fprintf(Cyc_stderr,((_tmp683="Error: could not create cstubs.c in directory %s\n",
_tag_dyneither(_tmp683,sizeof(char),50))),_tag_dyneither(_tmp682,sizeof(void*),1)))))));}
return 1;}{const char*_tmp684;Cyc_cycstubs_file=Cyc_fopen((const char*)
_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp684="cycstubs.cyc",
_tag_dyneither(_tmp684,sizeof(char),13)))),sizeof(char),1),(const char*)"w");}if(
!((unsigned int)Cyc_cycstubs_file)){{const char*_tmp688;void*_tmp687[1];struct Cyc_String_pa_struct
_tmp686;(_tmp686.tag=0,((_tmp686.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_output_dir),((_tmp687[0]=& _tmp686,Cyc_fprintf(Cyc_stderr,((_tmp688="Error: could not create cycstubs.c in directory %s\n",
_tag_dyneither(_tmp688,sizeof(char),52))),_tag_dyneither(_tmp687,sizeof(void*),1)))))));}
return 1;}{const char*_tmp68B;void*_tmp68A;(_tmp68A=0,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_cycstubs_file),((_tmp68B="#include <core.h>\nusing Core;\n\n",
_tag_dyneither(_tmp68B,sizeof(char),32))),_tag_dyneither(_tmp68A,sizeof(void*),0)));}}}{
const char*outdir=(const char*)_untag_dyneither_ptr(Cyc_output_dir,sizeof(char),1);
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))return 1;else{for(0;Cyc_spec_files
!= 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){if(
Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,
outdir)){{const char*_tmp68E;void*_tmp68D;(_tmp68D=0,Cyc_fprintf(Cyc_stderr,((
_tmp68E="FATAL ERROR -- QUIT!\n",_tag_dyneither(_tmp68E,sizeof(char),22))),
_tag_dyneither(_tmp68D,sizeof(void*),0)));}exit(1);}}}if(Cyc_mode == (void*)2)Cyc_fclose((
struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{if(!Cyc_gathering()){Cyc_fclose((
struct Cyc___cycFILE*)_check_null(Cyc_log_file));if(!Cyc_do_setjmp){Cyc_fclose((
struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));Cyc_fclose((struct Cyc___cycFILE*)
_check_null(Cyc_cycstubs_file));}}}return 0;}}}
