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
void*v;};int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*
Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);int Cyc_List_length(
struct Cyc_List_List*x);void*Cyc_List_hd(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_tl(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rcopy(struct
_RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*tag;};void
Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void Cyc_List_iter_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rappend(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_struct{char*tag;};int Cyc_List_exists_c(int(*pred)(void*,
void*),void*env,struct Cyc_List_List*x);int Cyc_List_mem(int(*compare)(void*,void*),
struct Cyc_List_List*l,void*x);struct Cyc_List_List*Cyc_List_delete_cmp(int(*cmp)(
void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*cmp)(void*,
void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Iter_Iter{void*
env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*
tag;};struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);
struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct
Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{
char*tag;};struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(
void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**
ans);void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d);void Cyc_Dict_iter_c(
void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict
Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2);struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,
void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct
_tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,
struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct
Cyc_Dict_Dict d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,
struct Cyc_Dict_Dict d);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct
_dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;int Cyc_Position_segment_equals(struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*);struct _dyneither_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple1{union
Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};enum Cyc_Absyn_Scope{Cyc_Absyn_Static
 = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC
 = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;
int q_restrict;int real_const;struct Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{
Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz
 = 3,Cyc_Absyn_LongLong_sz  = 4};enum Cyc_Absyn_Kind{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind
 = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_UniqueRgnKind  = 4,Cyc_Absyn_TopRgnKind
 = 5,Cyc_Absyn_EffKind  = 6,Cyc_Absyn_IntKind  = 7};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed
 = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA
 = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};
struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};
struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{
struct _union_Constraint_Eq_constr Eq_constr;struct
_union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr
No_constr;};struct Cyc_Absyn_Eq_kb_struct{int tag;enum Cyc_Absyn_Kind f1;};struct
Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;enum Cyc_Absyn_Kind f2;};struct Cyc_Absyn_Tvar{struct
_dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_struct{
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
rgn_loc;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;
union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*
zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;
struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_struct{int tag;
void*f1;};struct Cyc_Absyn_Thin_ptrqual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{
int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{
int tag;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;
struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_VoidType_struct{int tag;};struct Cyc_Absyn_Evar_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;}
;struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{
int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;enum 
Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_struct{int
tag;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_ValueofType_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};
struct Cyc_Absyn_HeapRgn_struct{int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;
};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,
Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct
Cyc_Absyn_Stdcall_att_struct{int tag;};struct Cyc_Absyn_Cdecl_att_struct{int tag;};
struct Cyc_Absyn_Fastcall_att_struct{int tag;};struct Cyc_Absyn_Noreturn_att_struct{
int tag;};struct Cyc_Absyn_Const_att_struct{int tag;};struct Cyc_Absyn_Aligned_att_struct{
int tag;int f1;};struct Cyc_Absyn_Packed_att_struct{int tag;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_struct{int tag;};
struct Cyc_Absyn_Shared_att_struct{int tag;};struct Cyc_Absyn_Unused_att_struct{int
tag;};struct Cyc_Absyn_Weak_att_struct{int tag;};struct Cyc_Absyn_Dllimport_att_struct{
int tag;};struct Cyc_Absyn_Dllexport_att_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_struct{
int tag;};struct Cyc_Absyn_Constructor_att_struct{int tag;};struct Cyc_Absyn_Destructor_att_struct{
int tag;};struct Cyc_Absyn_No_check_memory_usage_att_struct{int tag;};struct Cyc_Absyn_Format_att_struct{
int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{
int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{int tag;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;union
Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct
_tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7
val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct
_union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times
 = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,
Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte
 = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor
 = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,
Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc
 = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,
Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion
 = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**
elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct
Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;
};struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnknownCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*
f3;};struct Cyc_Absyn_Throw_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{
int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct
Cyc_Absyn_Address_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{
int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{
int tag;struct Cyc_List_List*f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple8{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple8*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*
f3;};struct Cyc_Absyn_Malloc_e_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};
struct Cyc_Absyn_Swap_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{int tag;struct Cyc_Core_Opt*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Tagcheck_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{
struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*loc;void*annot;};
struct Cyc_Absyn_Skip_s_struct{int tag;};struct Cyc_Absyn_Exp_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;
};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*loc;struct Cyc_List_List*
non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_struct{int tag;}
;struct Cyc_Absyn_Var_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*
f2;};struct Cyc_Absyn_Reference_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct
Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{int tag;struct Cyc_Absyn_Tvar*
f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{int tag;struct Cyc_List_List*
f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};
struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*
f3;int f4;};struct Cyc_Absyn_Null_p_struct{int tag;};struct Cyc_Absyn_Int_p_struct{
int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Enum_p_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{
int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual
tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*defn;
struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*
f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*
f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Datatype_d_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{
int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int
tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};extern struct Cyc_Absyn_EmptyAnnot_struct Cyc_Absyn_EmptyAnnot_val;int
Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_nontagged_union_type(
void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_NotLoop_j_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_struct{
int tag;};struct Cyc_Tcenv_FnEnd_j_struct{int tag;};struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
int Cyc_Tcutil_typecmp(void*,void*);int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct _tuple10{unsigned int f1;int f2;};struct _tuple10 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_UniquePlace{struct
Cyc_CfFlowInfo_Place place;struct Cyc_List_List*path;};enum Cyc_CfFlowInfo_InitLevel{
Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple11{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple11 val;};struct
_union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;};union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_EqualConst(unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(
struct Cyc_Absyn_Vardecl*,void*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(
struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(
unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*);
struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp
rop;};struct Cyc_CfFlowInfo_TagCmp{enum Cyc_Absyn_Primop cmp;void*bd;};char Cyc_CfFlowInfo_HasTagCmps[
11]="HasTagCmps";struct Cyc_CfFlowInfo_HasTagCmps_struct{char*tag;struct Cyc_List_List*
f1;};char Cyc_CfFlowInfo_IsZero[7]="IsZero";struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;};char Cyc_CfFlowInfo_NotZero[8]="NotZero";struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[9]="UnknownZ";
struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};extern
struct Cyc_CfFlowInfo_IsZero_struct Cyc_CfFlowInfo_IsZero_val;struct Cyc_List_List*
Cyc_CfFlowInfo_copy_tagcmps(struct _RegionHandle*,struct Cyc_List_List*);struct
_union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct
_union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct
_union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_UnknownL();struct Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;int f1;
struct _dyneither_ptr f2;};int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*
set,void*place,struct Cyc_Position_Segment*loc);int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_equals(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_uplace_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_CfFlowInfo_and_consume(struct _RegionHandle*,struct Cyc_CfFlowInfo_ConsumeInfo
c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);int Cyc_CfFlowInfo_consume_approx(struct
Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);struct
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};struct
_union_FlowInfo_ReachableFL{int tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{
struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL
ReachableFL;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*,struct Cyc_CfFlowInfo_ConsumeInfo);
struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;
void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*
Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);int Cyc_CfFlowInfo_get_field_index(
void*t,struct _dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*
fs,struct _dyneither_ptr*f);int Cyc_CfFlowInfo_root_cmp(void*,void*);int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,void*leafval);enum Cyc_CfFlowInfo_InitLevel
Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*
r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,
union Cyc_CfFlowInfo_FlowInfo f2);void Cyc_CfFlowInfo_print_absrval(void*rval);void
Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);void Cyc_CfFlowInfo_print_root(
void*root);void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_dict_set(
struct Cyc_Dict_Dict p,void(*pr)(void*));void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*
p,void(*pr)(void*));void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);void
Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
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
place,void*r);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,
int);struct _tuple13{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,
struct _tuple13 pr2,int);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,
union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo
f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};extern struct Cyc_CfFlowInfo_UniqueRgn_k_struct
Cyc_CfFlowInfo_UniqueRgn_k_val;int Cyc_CfFlowInfo_contains_region(void*rgn,void*t);
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo f);void Cyc_CfFlowInfo_check_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo f);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo f);struct _tuple14{struct Cyc_CfFlowInfo_ConsumeInfo
f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple14 Cyc_CfFlowInfo_save_consume_info(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo f,int clear);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_CfFlowInfo_ConsumeInfo
c,int may_consume_only);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_IsZero_struct
Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};struct Cyc_CfFlowInfo_UniqueRgn_k_struct
Cyc_CfFlowInfo_UniqueRgn_k_val={0};union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(
struct Cyc_CfFlowInfo_Place*x);union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(
struct Cyc_CfFlowInfo_Place*x){union Cyc_CfFlowInfo_AbsLVal _tmp4B7;return((_tmp4B7.PlaceL).val=
x,(((_tmp4B7.PlaceL).tag=1,_tmp4B7)));}union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){union Cyc_CfFlowInfo_AbsLVal
_tmp4B8;return((_tmp4B8.UnknownL).val=0,(((_tmp4B8.UnknownL).tag=2,_tmp4B8)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo _tmp4B9;return((_tmp4B9.BottomFL).val=
0,(((_tmp4B9.BottomFL).tag=1,_tmp4B9)));}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(
struct Cyc_Dict_Dict fd,struct Cyc_List_List*r,struct Cyc_CfFlowInfo_ConsumeInfo c);
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,
struct Cyc_List_List*r,struct Cyc_CfFlowInfo_ConsumeInfo c){struct _tuple12 _tmp4BC;
union Cyc_CfFlowInfo_FlowInfo _tmp4BB;return((_tmp4BB.ReachableFL).val=((_tmp4BC.f1=
fd,((_tmp4BC.f2=r,((_tmp4BC.f3=c,_tmp4BC)))))),(((_tmp4BB.ReachableFL).tag=2,
_tmp4BB)));}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*
p2);int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(
struct _RegionHandle*r);struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(
struct _RegionHandle*r){static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,{.Null_c={
1,0}}};static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_Zero_struct*_tmp4F3;struct Cyc_CfFlowInfo_Zero_struct _tmp4F2;
struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp4F1;struct Cyc_CfFlowInfo_NotZeroAll_struct
_tmp4F0;struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp4EF;struct Cyc_CfFlowInfo_NotZeroThis_struct
_tmp4EE;struct Cyc_CfFlowInfo_UnknownR_struct*_tmp4ED;struct Cyc_CfFlowInfo_UnknownR_struct
_tmp4EC;struct Cyc_CfFlowInfo_UnknownR_struct*_tmp4EB;struct Cyc_CfFlowInfo_UnknownR_struct
_tmp4EA;struct Cyc_CfFlowInfo_UnknownR_struct*_tmp4E9;struct Cyc_CfFlowInfo_UnknownR_struct
_tmp4E8;struct Cyc_CfFlowInfo_Esc_struct*_tmp4E7;struct Cyc_CfFlowInfo_Esc_struct
_tmp4E6;struct Cyc_CfFlowInfo_Esc_struct*_tmp4E5;struct Cyc_CfFlowInfo_Esc_struct
_tmp4E4;struct Cyc_CfFlowInfo_Esc_struct*_tmp4E3;struct Cyc_CfFlowInfo_Esc_struct
_tmp4E2;struct Cyc_CfFlowInfo_Place*_tmp4E1;struct Cyc_CfFlowInfo_MallocPt_struct
_tmp4E0;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp4DF;struct Cyc_CfFlowInfo_FlowEnv*
_tmp4DE;return(_tmp4DE=_region_malloc(r,sizeof(*_tmp4DE)),((_tmp4DE->r=r,((
_tmp4DE->zero=(void*)((_tmp4F3=_region_malloc(r,sizeof(*_tmp4F3)),((_tmp4F3[0]=((
_tmp4F2.tag=0,_tmp4F2)),_tmp4F3)))),((_tmp4DE->notzeroall=(void*)((_tmp4F1=
_region_malloc(r,sizeof(*_tmp4F1)),((_tmp4F1[0]=((_tmp4F0.tag=1,_tmp4F0)),
_tmp4F1)))),((_tmp4DE->notzerothis=(void*)((_tmp4EF=_region_malloc(r,sizeof(*
_tmp4EF)),((_tmp4EF[0]=((_tmp4EE.tag=2,_tmp4EE)),_tmp4EF)))),((_tmp4DE->unknown_none=(
void*)((_tmp4ED=_region_malloc(r,sizeof(*_tmp4ED)),((_tmp4ED[0]=((_tmp4EC.tag=3,((
_tmp4EC.f1=Cyc_CfFlowInfo_NoneIL,_tmp4EC)))),_tmp4ED)))),((_tmp4DE->unknown_this=(
void*)((_tmp4EB=_region_malloc(r,sizeof(*_tmp4EB)),((_tmp4EB[0]=((_tmp4EA.tag=3,((
_tmp4EA.f1=Cyc_CfFlowInfo_ThisIL,_tmp4EA)))),_tmp4EB)))),((_tmp4DE->unknown_all=(
void*)((_tmp4E9=_region_malloc(r,sizeof(*_tmp4E9)),((_tmp4E9[0]=((_tmp4E8.tag=3,((
_tmp4E8.f1=Cyc_CfFlowInfo_AllIL,_tmp4E8)))),_tmp4E9)))),((_tmp4DE->esc_none=(
void*)((_tmp4E7=_region_malloc(r,sizeof(*_tmp4E7)),((_tmp4E7[0]=((_tmp4E6.tag=4,((
_tmp4E6.f1=Cyc_CfFlowInfo_NoneIL,_tmp4E6)))),_tmp4E7)))),((_tmp4DE->esc_this=(
void*)((_tmp4E5=_region_malloc(r,sizeof(*_tmp4E5)),((_tmp4E5[0]=((_tmp4E4.tag=4,((
_tmp4E4.f1=Cyc_CfFlowInfo_ThisIL,_tmp4E4)))),_tmp4E5)))),((_tmp4DE->esc_all=(
void*)((_tmp4E3=_region_malloc(r,sizeof(*_tmp4E3)),((_tmp4E3[0]=((_tmp4E2.tag=4,((
_tmp4E2.f1=Cyc_CfFlowInfo_AllIL,_tmp4E2)))),_tmp4E3)))),((_tmp4DE->mt_flowdict=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(
r,Cyc_CfFlowInfo_root_cmp),((_tmp4DE->mt_place_set=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))
Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp4DE->dummy_place=((_tmp4E1=
_region_malloc(r,sizeof(*_tmp4E1)),((_tmp4E1->root=(void*)((_tmp4DF=
_region_malloc(r,sizeof(*_tmp4DF)),((_tmp4DF[0]=((_tmp4E0.tag=1,((_tmp4E0.f1=&
dummy_exp,((_tmp4E0.f2=(void*)((void*)& Cyc_Absyn_VoidType_val),_tmp4E0)))))),
_tmp4DF)))),((_tmp4E1->fields=0,_tmp4E1)))))),_tmp4DE)))))))))))))))))))))))))));}
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,struct Cyc_Position_Segment*
loc);int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,
struct Cyc_Position_Segment*loc){if(set != 0){if(((int(*)(struct Cyc_Dict_Dict d,
void*k))Cyc_Dict_member)(*set,place))return 1;else{*set=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,struct Cyc_Position_Segment*v))Cyc_Dict_insert)(*set,
place,loc);}}return 0;}struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct
Cyc_CfFlowInfo_Place*place);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place){void*_tmp1F;struct Cyc_List_List*_tmp20;struct
Cyc_CfFlowInfo_Place _tmp1E=*place;_tmp1F=_tmp1E.root;_tmp20=_tmp1E.fields;{void*
_tmp21=_tmp1F;struct Cyc_Absyn_Vardecl*_tmp23;_LL1: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp22=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp21;if(_tmp22->tag != 0)goto _LL3;
else{_tmp23=_tmp22->f1;}}_LL2: if(_tmp20 == 0)return Cyc_Absynpp_qvar2string(_tmp23->name);
else{const char*_tmp4F7;void*_tmp4F6[1];struct Cyc_String_pa_struct _tmp4F5;return(
struct _dyneither_ptr)((_tmp4F5.tag=0,((_tmp4F5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp23->name)),((_tmp4F6[0]=& _tmp4F5,Cyc_aprintf(((
_tmp4F7="reachable from %s",_tag_dyneither(_tmp4F7,sizeof(char),18))),
_tag_dyneither(_tmp4F6,sizeof(void*),1))))))));}_LL3:;_LL4: {const char*_tmp4FA;
void*_tmp4F9;(_tmp4F9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp4FA="error locations not for VarRoots",_tag_dyneither(
_tmp4FA,sizeof(char),33))),_tag_dyneither(_tmp4F9,sizeof(void*),0)));}_LL0:;}}
struct _dyneither_ptr Cyc_CfFlowInfo_uplace_err_string(struct Cyc_CfFlowInfo_UniquePlace*
place);struct _dyneither_ptr Cyc_CfFlowInfo_uplace_err_string(struct Cyc_CfFlowInfo_UniquePlace*
place){return Cyc_CfFlowInfo_place_err_string(& place->place);}static void Cyc_CfFlowInfo_unique_err(
void*place,struct _dyneither_ptr(*toString)(void*),struct _dyneither_ptr err_msg1,
struct _dyneither_ptr err_msg2,struct Cyc_Position_Segment*consumed_loc,struct Cyc_Position_Segment*
loc);static void Cyc_CfFlowInfo_unique_err(void*place,struct _dyneither_ptr(*
toString)(void*),struct _dyneither_ptr err_msg1,struct _dyneither_ptr err_msg2,
struct Cyc_Position_Segment*consumed_loc,struct Cyc_Position_Segment*loc){struct
_dyneither_ptr _tmp29=toString(place);if(!Cyc_Position_segment_equals(
consumed_loc,loc)){struct _dyneither_ptr _tmp2A=Cyc_Position_string_of_segment(
consumed_loc);void*_tmp4FD[2];struct Cyc_String_pa_struct _tmp4FC;struct Cyc_String_pa_struct
_tmp4FB;(_tmp4FB.tag=0,((_tmp4FB.f1=(struct _dyneither_ptr)_tmp2A,((_tmp4FC.tag=0,((
_tmp4FC.f1=(struct _dyneither_ptr)_tmp29,((_tmp4FD[0]=& _tmp4FC,((_tmp4FD[1]=&
_tmp4FB,Cyc_Tcutil_terr(loc,err_msg1,_tag_dyneither(_tmp4FD,sizeof(void*),2)))))))))))));}
else{void*_tmp4FF[1];struct Cyc_String_pa_struct _tmp4FE;(_tmp4FE.tag=0,((_tmp4FE.f1=(
struct _dyneither_ptr)_tmp29,((_tmp4FF[0]=& _tmp4FE,Cyc_Tcutil_terr(loc,err_msg2,
_tag_dyneither(_tmp4FF,sizeof(void*),1)))))));}}struct _tuple15{int f1;struct
_dyneither_ptr(*f2)(void*);};static struct Cyc_Position_Segment*Cyc_CfFlowInfo_combine_consume_f(
struct _tuple15*env,void*place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*
loc2);static struct Cyc_Position_Segment*Cyc_CfFlowInfo_combine_consume_f(struct
_tuple15*env,void*place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*
loc2){struct _tuple15 _tmp31;int _tmp32;struct _dyneither_ptr(*_tmp33)(void*);struct
_tuple15*_tmp30=env;_tmp31=*_tmp30;_tmp32=_tmp31.f1;_tmp33=_tmp31.f2;if(_tmp32){
const char*_tmp501;const char*_tmp500;Cyc_CfFlowInfo_unique_err(place,_tmp33,((
_tmp500="May consume non-aliased pointer %s more than once (cf. %s)",
_tag_dyneither(_tmp500,sizeof(char),59))),((_tmp501="May consume non-aliased pointer %s more than once",
_tag_dyneither(_tmp501,sizeof(char),50))),loc1,loc2);}return loc1;}struct _tuple16{
int f1;struct _dyneither_ptr(*f2)(struct Cyc_CfFlowInfo_Place*place);};struct Cyc_Dict_Dict
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int
disjoint);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2,int disjoint){struct _tuple16 _tmp502;struct _tuple16 _tmp36=(
_tmp502.f1=disjoint,((_tmp502.f2=Cyc_CfFlowInfo_place_err_string,_tmp502)));
struct Cyc_Dict_Dict _tmp37=((struct Cyc_Dict_Dict(*)(struct Cyc_Position_Segment*(*
f)(struct _tuple16*,struct Cyc_CfFlowInfo_Place*,struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*),struct _tuple16*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_union_two_c)((struct Cyc_Position_Segment*(*)(struct _tuple16*env,
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*
loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp36,s1,s2);return _tmp37;}struct
_tuple17{int f1;struct _dyneither_ptr(*f2)(struct Cyc_CfFlowInfo_UniquePlace*place);
};struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_uplace_set(struct Cyc_Dict_Dict s1,
struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_uplace_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint){struct _tuple17 _tmp503;
struct _tuple17 _tmp39=(_tmp503.f1=disjoint,((_tmp503.f2=Cyc_CfFlowInfo_uplace_err_string,
_tmp503)));return((struct Cyc_Dict_Dict(*)(struct Cyc_Position_Segment*(*f)(struct
_tuple17*,struct Cyc_CfFlowInfo_UniquePlace*,struct Cyc_Position_Segment*,struct
Cyc_Position_Segment*),struct _tuple17*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_union_two_c)((struct Cyc_Position_Segment*(*)(struct _tuple17*env,
struct Cyc_CfFlowInfo_UniquePlace*place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*
loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp39,s1,s2);}struct _tuple18{void*f1;
struct Cyc_Position_Segment*f2;};int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2){if((int)s1.t == (int)s2.t)return 1;{struct _handler_cons
_tmp3B;_push_handler(& _tmp3B);{int _tmp3D=0;if(setjmp(_tmp3B.handler))_tmp3D=1;
if(!_tmp3D){{struct _RegionHandle _tmp3E=_new_region("temp");struct _RegionHandle*
temp=& _tmp3E;_push_region(temp);{struct Cyc_Iter_Iter _tmp3F=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(temp,s1);
struct _tuple18 _tmp40=*((struct _tuple18*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict
d))Cyc_Dict_rchoose)(temp,s1);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))
Cyc_Iter_next)(_tmp3F,& _tmp40)){void*_tmp41=_tmp40.f1;if(!((int(*)(struct Cyc_Dict_Dict
d,void*k))Cyc_Dict_member)(s2,_tmp41)){int _tmp42=0;_npop_handler(1);return _tmp42;}}{
int _tmp43=1;_npop_handler(1);return _tmp43;}};_pop_region(temp);};_pop_handler();}
else{void*_tmp3C=(void*)_exn_thrown;void*_tmp45=_tmp3C;_LL6:;_LL7: return 1;_LL8:;
_LL9:(void)_throw(_tmp45);_LL5:;}}}}int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2){return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(
s2,s1);}int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct
_dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,
struct _dyneither_ptr*f){int n=0;for(0;(unsigned int)fs;fs=fs->tl){struct
_dyneither_ptr*_tmp46=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;if(Cyc_strptrcmp(
_tmp46,f)== 0)return n;n=n + 1;}{struct Cyc_Core_Impossible_struct _tmp510;const char*
_tmp50F;void*_tmp50E[1];struct Cyc_String_pa_struct _tmp50D;struct Cyc_Core_Impossible_struct*
_tmp50C;(int)_throw((void*)((_tmp50C=_cycalloc(sizeof(*_tmp50C)),((_tmp50C[0]=((
_tmp510.tag=Cyc_Core_Impossible,((_tmp510.f1=(struct _dyneither_ptr)((_tmp50D.tag=
0,((_tmp50D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp50E[0]=&
_tmp50D,Cyc_aprintf(((_tmp50F="get_field_index_fs failed: %s",_tag_dyneither(
_tmp50F,sizeof(char),30))),_tag_dyneither(_tmp50E,sizeof(void*),1)))))))),
_tmp510)))),_tmp50C)))));}}int Cyc_CfFlowInfo_get_field_index(void*t,struct
_dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*
f){void*_tmp4C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp4E;union Cyc_Absyn_AggrInfoU
_tmp4F;struct Cyc_List_List*_tmp51;_LLB: {struct Cyc_Absyn_AggrType_struct*_tmp4D=(
struct Cyc_Absyn_AggrType_struct*)_tmp4C;if(_tmp4D->tag != 12)goto _LLD;else{_tmp4E=
_tmp4D->f1;_tmp4F=_tmp4E.aggr_info;}}_LLC: {struct Cyc_List_List*_tmp52=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp4F))->impl))->fields;
_tmp51=_tmp52;goto _LLE;}_LLD: {struct Cyc_Absyn_AnonAggrType_struct*_tmp50=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp4C;if(_tmp50->tag != 13)goto _LLF;else{
_tmp51=_tmp50->f2;}}_LLE: return Cyc_CfFlowInfo_get_field_index_fs(_tmp51,f);_LLF:;
_LL10: {struct Cyc_Core_Impossible_struct _tmp51D;const char*_tmp51C;void*_tmp51B[1];
struct Cyc_String_pa_struct _tmp51A;struct Cyc_Core_Impossible_struct*_tmp519;(int)
_throw((void*)((_tmp519=_cycalloc(sizeof(*_tmp519)),((_tmp519[0]=((_tmp51D.tag=
Cyc_Core_Impossible,((_tmp51D.f1=(struct _dyneither_ptr)((_tmp51A.tag=0,((_tmp51A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp51B[
0]=& _tmp51A,Cyc_aprintf(((_tmp51C="get_field_index failed: %s",_tag_dyneither(
_tmp51C,sizeof(char),27))),_tag_dyneither(_tmp51B,sizeof(void*),1)))))))),
_tmp51D)))),_tmp519)))));}_LLA:;}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);int
Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)r1 == (int)r2)return 0;{struct
_tuple0 _tmp51E;struct _tuple0 _tmp59=(_tmp51E.f1=r1,((_tmp51E.f2=r2,_tmp51E)));
void*_tmp5A;struct Cyc_Absyn_Vardecl*_tmp5C;void*_tmp5D;struct Cyc_Absyn_Vardecl*
_tmp5F;void*_tmp60;void*_tmp62;void*_tmp64;struct Cyc_Absyn_Exp*_tmp66;void*
_tmp67;struct Cyc_Absyn_Exp*_tmp69;void*_tmp6A;void*_tmp6C;void*_tmp6E;int _tmp70;
void*_tmp71;int _tmp73;_LL12: _tmp5A=_tmp59.f1;{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp5B=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp5A;if(_tmp5B->tag != 0)goto _LL14;
else{_tmp5C=_tmp5B->f1;}}_tmp5D=_tmp59.f2;{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp5E=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp5D;if(_tmp5E->tag != 0)goto _LL14;
else{_tmp5F=_tmp5E->f1;}}_LL13: return(int)_tmp5C - (int)_tmp5F;_LL14: _tmp60=
_tmp59.f1;{struct Cyc_CfFlowInfo_VarRoot_struct*_tmp61=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp60;if(_tmp61->tag != 0)goto _LL16;}_LL15: return - 1;_LL16: _tmp62=_tmp59.f2;{
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp63=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp62;if(_tmp63->tag != 0)goto _LL18;}_LL17: return 1;_LL18: _tmp64=_tmp59.f1;{
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp65=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp64;if(_tmp65->tag != 1)goto _LL1A;else{_tmp66=_tmp65->f1;}}_tmp67=_tmp59.f2;{
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp68=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp67;if(_tmp68->tag != 1)goto _LL1A;else{_tmp69=_tmp68->f1;}}_LL19: return(int)
_tmp66 - (int)_tmp69;_LL1A: _tmp6A=_tmp59.f1;{struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp6B=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp6A;if(_tmp6B->tag != 1)goto
_LL1C;}_LL1B: return - 1;_LL1C: _tmp6C=_tmp59.f2;{struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp6D=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp6C;if(_tmp6D->tag != 1)goto
_LL1E;}_LL1D: return 1;_LL1E: _tmp6E=_tmp59.f1;{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp6F=(struct Cyc_CfFlowInfo_InitParam_struct*)_tmp6E;if(_tmp6F->tag != 2)goto
_LL11;else{_tmp70=_tmp6F->f1;}}_tmp71=_tmp59.f2;{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp72=(struct Cyc_CfFlowInfo_InitParam_struct*)_tmp71;if(_tmp72->tag != 2)goto
_LL11;else{_tmp73=_tmp72->f1;}}_LL1F: return _tmp70 - _tmp73;_LL11:;}}static int Cyc_CfFlowInfo_intlist_cmp(
struct Cyc_List_List*list1,struct Cyc_List_List*list2);static int Cyc_CfFlowInfo_intlist_cmp(
struct Cyc_List_List*list1,struct Cyc_List_List*list2){return((int(*)(int(*cmp)(
int,int),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,
list1,list2);}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct
Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*
p1,struct Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)p2)return 0;{void*_tmp75;
struct Cyc_List_List*_tmp76;struct Cyc_CfFlowInfo_Place _tmp74=*p1;_tmp75=_tmp74.root;
_tmp76=_tmp74.fields;{void*_tmp78;struct Cyc_List_List*_tmp79;struct Cyc_CfFlowInfo_Place
_tmp77=*p2;_tmp78=_tmp77.root;_tmp79=_tmp77.fields;{int i=Cyc_CfFlowInfo_root_cmp(
_tmp75,_tmp78);if(i != 0)return i;return Cyc_CfFlowInfo_intlist_cmp(_tmp76,_tmp79);}}}}
int Cyc_CfFlowInfo_uplace_cmp(struct Cyc_CfFlowInfo_UniquePlace*p1,struct Cyc_CfFlowInfo_UniquePlace*
p2);int Cyc_CfFlowInfo_uplace_cmp(struct Cyc_CfFlowInfo_UniquePlace*p1,struct Cyc_CfFlowInfo_UniquePlace*
p2){if((int)p1 == (int)p2)return 0;{struct Cyc_CfFlowInfo_Place _tmp7B;struct Cyc_List_List*
_tmp7C;struct Cyc_CfFlowInfo_UniquePlace _tmp7A=*p1;_tmp7B=_tmp7A.place;_tmp7C=
_tmp7A.path;{struct Cyc_CfFlowInfo_Place _tmp7E;struct Cyc_List_List*_tmp7F;struct
Cyc_CfFlowInfo_UniquePlace _tmp7D=*p2;_tmp7E=_tmp7D.place;_tmp7F=_tmp7D.path;{int
i=Cyc_CfFlowInfo_place_cmp(& _tmp7B,& _tmp7E);if(i != 0)return i;return((int(*)(int(*
cmp)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_CfFlowInfo_intlist_cmp,_tmp7C,_tmp7F);}}}}static
struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p);
static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*
p){struct Cyc_List_List*sl=0;void*_tmp81;struct Cyc_List_List*_tmp82;struct Cyc_CfFlowInfo_Place
_tmp80=*p;_tmp81=_tmp80.root;_tmp82=_tmp80.fields;{void*_tmp83=_tmp81;struct Cyc_Absyn_Vardecl*
_tmp85;struct Cyc_Absyn_Exp*_tmp87;int _tmp89;_LL21: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp84=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp83;if(_tmp84->tag != 0)goto _LL23;
else{_tmp85=_tmp84->f1;}}_LL22:{struct _dyneither_ptr*_tmp52B;const char*_tmp52A;
void*_tmp529[1];struct Cyc_String_pa_struct _tmp528;struct Cyc_List_List*_tmp527;sl=((
_tmp527=_cycalloc(sizeof(*_tmp527)),((_tmp527->hd=((_tmp52B=_cycalloc(sizeof(*
_tmp52B)),((_tmp52B[0]=((_tmp528.tag=0,((_tmp528.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp85->name).f2),((_tmp529[0]=& _tmp528,Cyc_aprintf(((
_tmp52A="%s",_tag_dyneither(_tmp52A,sizeof(char),3))),_tag_dyneither(_tmp529,
sizeof(void*),1)))))))),_tmp52B)))),((_tmp527->tl=sl,_tmp527))))));}goto _LL20;
_LL23: {struct Cyc_CfFlowInfo_MallocPt_struct*_tmp86=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp83;if(_tmp86->tag != 1)goto _LL25;else{_tmp87=_tmp86->f1;}}_LL24:{struct
_dyneither_ptr*_tmp538;const char*_tmp537;void*_tmp536[1];struct Cyc_Int_pa_struct
_tmp535;struct Cyc_List_List*_tmp534;sl=((_tmp534=_cycalloc(sizeof(*_tmp534)),((
_tmp534->hd=((_tmp538=_cycalloc(sizeof(*_tmp538)),((_tmp538[0]=((_tmp535.tag=1,((
_tmp535.f1=(unsigned long)((int)_tmp87),((_tmp536[0]=& _tmp535,Cyc_aprintf(((
_tmp537="mpt%d",_tag_dyneither(_tmp537,sizeof(char),6))),_tag_dyneither(_tmp536,
sizeof(void*),1)))))))),_tmp538)))),((_tmp534->tl=sl,_tmp534))))));}goto _LL20;
_LL25: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp88=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp83;if(_tmp88->tag != 2)goto _LL20;else{_tmp89=_tmp88->f1;}}_LL26:{struct
_dyneither_ptr*_tmp545;const char*_tmp544;void*_tmp543[1];struct Cyc_Int_pa_struct
_tmp542;struct Cyc_List_List*_tmp541;sl=((_tmp541=_cycalloc(sizeof(*_tmp541)),((
_tmp541->hd=((_tmp545=_cycalloc(sizeof(*_tmp545)),((_tmp545[0]=((_tmp542.tag=1,((
_tmp542.f1=(unsigned long)_tmp89,((_tmp543[0]=& _tmp542,Cyc_aprintf(((_tmp544="param%d",
_tag_dyneither(_tmp544,sizeof(char),8))),_tag_dyneither(_tmp543,sizeof(void*),1)))))))),
_tmp545)))),((_tmp541->tl=sl,_tmp541))))));}goto _LL20;_LL20:;}for(0;_tmp82 != 0;
_tmp82=_tmp82->tl){struct _dyneither_ptr*_tmp552;const char*_tmp551;void*_tmp550[1];
struct Cyc_Int_pa_struct _tmp54F;struct Cyc_List_List*_tmp54E;sl=((_tmp54E=
_cycalloc(sizeof(*_tmp54E)),((_tmp54E->hd=((_tmp552=_cycalloc(sizeof(*_tmp552)),((
_tmp552[0]=((_tmp54F.tag=1,((_tmp54F.f1=(unsigned long)((int)_tmp82->hd),((
_tmp550[0]=& _tmp54F,Cyc_aprintf(((_tmp551="%d",_tag_dyneither(_tmp551,sizeof(
char),3))),_tag_dyneither(_tmp550,sizeof(void*),1)))))))),_tmp552)))),((_tmp54E->tl=
sl,_tmp54E))))));}{struct Cyc_String_pa_struct _tmp55D;void*_tmp55C[1];const char*
_tmp55B;const char*_tmp55A;struct _dyneither_ptr*_tmp559;struct _dyneither_ptr*
_tmp9E=(_tmp559=_cycalloc(sizeof(*_tmp559)),((_tmp559[0]=((_tmp55D.tag=0,((
_tmp55D.f1=(struct _dyneither_ptr)((_tmp55A="",_tag_dyneither(_tmp55A,sizeof(char),
1))),((_tmp55C[0]=& _tmp55D,Cyc_aprintf(((_tmp55B="%s",_tag_dyneither(_tmp55B,
sizeof(char),3))),_tag_dyneither(_tmp55C,sizeof(void*),1)))))))),_tmp559)));for(
0;sl != 0;sl=sl->tl){const char*_tmp562;void*_tmp561[2];struct Cyc_String_pa_struct
_tmp560;struct Cyc_String_pa_struct _tmp55F;*_tmp9E=((_tmp55F.tag=0,((_tmp55F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9E),((_tmp560.tag=0,((_tmp560.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((
_tmp561[0]=& _tmp560,((_tmp561[1]=& _tmp55F,Cyc_aprintf(((_tmp562="%s.%s",
_tag_dyneither(_tmp562,sizeof(char),6))),_tag_dyneither(_tmp561,sizeof(void*),2))))))))))))));}
return _tmp9E;}}static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*
fenv,int allow_zeroterm,void*t,void*leafval);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,void*leafval);static
void _tmp568(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp567,unsigned int*
_tmp566,void***_tmp564){for(*_tmp567=0;*_tmp567 < *_tmp566;(*_tmp567)++){(*
_tmp564)[*_tmp567]=(*fenv)->unknown_all;}}struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,void*leafval){
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
unsigned int _tmp567;unsigned int _tmp566;struct _dyneither_ptr _tmp565;void**
_tmp564;unsigned int _tmp563;struct _dyneither_ptr d=(_tmp563=sz,((_tmp564=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp563)),((_tmp565=
_tag_dyneither(_tmp564,sizeof(void*),_tmp563),((((_tmp566=_tmp563,_tmp568(& fenv,&
_tmp567,& _tmp566,& _tmp564))),_tmp565)))))));{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield _tmpA9;struct _dyneither_ptr*_tmpAA;void*_tmpAB;struct
Cyc_Absyn_Aggrfield*_tmpA8=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)
_check_null(fs))->hd;_tmpA9=*_tmpA8;_tmpAA=_tmpA9.name;_tmpAB=_tmpA9.type;if(
_get_dyneither_size(*_tmpAA,sizeof(char))!= 1)*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,_tmpAB,leafval);}}return d;}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;
};static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
int allow_zeroterm,void*t,void*leafval);static void _tmp56E(struct Cyc_CfFlowInfo_FlowEnv**
fenv,unsigned int*_tmp56D,unsigned int*_tmp56C,void***_tmp56A){for(*_tmp56D=0;*
_tmp56D < *_tmp56C;(*_tmp56D)++){(*_tmp56A)[*_tmp56D]=(*fenv)->unknown_all;}}
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int
allow_zeroterm,void*t,void*leafval){if(Cyc_Absyn_is_nontagged_union_type(t))
return fenv->unknown_all;{void*_tmpB0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_DatatypeFieldInfo
_tmpB2;union Cyc_Absyn_DatatypeFieldInfoU _tmpB3;struct _tuple2 _tmpB4;struct Cyc_Absyn_Datatypefield*
_tmpB5;struct Cyc_List_List*_tmpB7;struct Cyc_Absyn_AggrInfo _tmpB9;union Cyc_Absyn_AggrInfoU
_tmpBA;enum Cyc_Absyn_AggrKind _tmpBC;struct Cyc_List_List*_tmpBD;struct Cyc_Absyn_ArrayInfo
_tmpBF;void*_tmpC0;union Cyc_Absyn_Constraint*_tmpC1;void*_tmpC3;struct Cyc_Absyn_PtrInfo
_tmpC5;struct Cyc_Absyn_PtrAtts _tmpC6;union Cyc_Absyn_Constraint*_tmpC7;_LL28: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmpB1=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpB0;if(_tmpB1->tag != 4)goto _LL2A;else{_tmpB2=_tmpB1->f1;_tmpB3=_tmpB2.field_info;
if((_tmpB3.KnownDatatypefield).tag != 2)goto _LL2A;_tmpB4=(struct _tuple2)(_tmpB3.KnownDatatypefield).val;
_tmpB5=_tmpB4.f2;}}_LL29: if(_tmpB5->typs == 0)return leafval;_tmpB7=_tmpB5->typs;
goto _LL2B;_LL2A: {struct Cyc_Absyn_TupleType_struct*_tmpB6=(struct Cyc_Absyn_TupleType_struct*)
_tmpB0;if(_tmpB6->tag != 11)goto _LL2C;else{_tmpB7=_tmpB6->f1;}}_LL2B: {
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpB7);unsigned int _tmp56D;unsigned int _tmp56C;struct _dyneither_ptr _tmp56B;void**
_tmp56A;unsigned int _tmp569;struct _dyneither_ptr d=(_tmp569=sz,((_tmp56A=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp569)),((_tmp56B=
_tag_dyneither(_tmp56A,sizeof(void*),_tmp569),((((_tmp56C=_tmp569,_tmp56E(& fenv,&
_tmp56D,& _tmp56C,& _tmp56A))),_tmp56B)))))));{int i=0;for(0;i < sz;++ i){*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmpB7))->hd)).f2,
leafval);_tmpB7=_tmpB7->tl;}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp571;
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp570;return(void*)((_tmp570=
_region_malloc(fenv->r,sizeof(*_tmp570)),((_tmp570[0]=((_tmp571.tag=7,((_tmp571.f1=
0,((_tmp571.f2=d,_tmp571)))))),_tmp570))));}}_LL2C: {struct Cyc_Absyn_AggrType_struct*
_tmpB8=(struct Cyc_Absyn_AggrType_struct*)_tmpB0;if(_tmpB8->tag != 12)goto _LL2E;
else{_tmpB9=_tmpB8->f1;_tmpBA=_tmpB9.aggr_info;}}_LL2D: {struct Cyc_Absyn_Aggrdecl*
_tmpCE=Cyc_Absyn_get_known_aggrdecl(_tmpBA);if(_tmpCE->impl == 0)goto _LL27;_tmpBC=
_tmpCE->kind;_tmpBD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCE->impl))->fields;
goto _LL2F;}_LL2E: {struct Cyc_Absyn_AnonAggrType_struct*_tmpBB=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpB0;if(_tmpBB->tag != 13)goto _LL30;else{_tmpBC=_tmpBB->f1;_tmpBD=_tmpBB->f2;}}
_LL2F: {struct Cyc_CfFlowInfo_Aggregate_struct _tmp574;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp573;return(void*)((_tmp573=_region_malloc(fenv->r,sizeof(*_tmp573)),((
_tmp573[0]=((_tmp574.tag=7,((_tmp574.f1=_tmpBC == Cyc_Absyn_UnionA,((_tmp574.f2=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmpBD,leafval),_tmp574)))))),_tmp573))));}
_LL30:{struct Cyc_Absyn_ArrayType_struct*_tmpBE=(struct Cyc_Absyn_ArrayType_struct*)
_tmpB0;if(_tmpBE->tag != 9)goto _LL32;else{_tmpBF=_tmpBE->f1;_tmpC0=_tmpBF.elt_type;
_tmpC1=_tmpBF.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmpC1))goto _LL32;_LL31: return allow_zeroterm  && Cyc_Tcutil_bits_only(_tmpC0)?
fenv->unknown_all: leafval;_LL32: {struct Cyc_Absyn_TagType_struct*_tmpC2=(struct
Cyc_Absyn_TagType_struct*)_tmpB0;if(_tmpC2->tag != 20)goto _LL34;else{_tmpC3=(void*)
_tmpC2->f1;}}_LL33: {void*_tmpD1=leafval;enum Cyc_CfFlowInfo_InitLevel _tmpD3;
enum Cyc_CfFlowInfo_InitLevel _tmpD5;_LL39: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmpD2=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpD1;if(_tmpD2->tag != 3)goto
_LL3B;else{_tmpD3=_tmpD2->f1;if(_tmpD3 != Cyc_CfFlowInfo_AllIL)goto _LL3B;}}_LL3A:
goto _LL3C;_LL3B: {struct Cyc_CfFlowInfo_Esc_struct*_tmpD4=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmpD1;if(_tmpD4->tag != 4)goto _LL3D;else{_tmpD5=_tmpD4->f1;if(_tmpD5 != Cyc_CfFlowInfo_AllIL)
goto _LL3D;}}_LL3C: goto _LL3E;_LL3D: {struct Cyc_CfFlowInfo_Zero_struct*_tmpD6=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmpD1;if(_tmpD6->tag != 0)goto _LL3F;}_LL3E:
goto _LL40;_LL3F: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmpD7=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmpD1;if(_tmpD7->tag != 1)goto _LL41;}_LL40: {struct Cyc_CfFlowInfo_TagCmps_struct
_tmp57E;struct Cyc_CfFlowInfo_TagCmp*_tmp57D;struct Cyc_CfFlowInfo_TagCmp*_tmp57C[
1];struct Cyc_CfFlowInfo_TagCmps_struct*_tmp57B;return(void*)((_tmp57B=
_region_malloc(fenv->r,sizeof(*_tmp57B)),((_tmp57B[0]=((_tmp57E.tag=6,((_tmp57E.f1=((
_tmp57C[0]=((_tmp57D=_region_malloc(fenv->r,sizeof(*_tmp57D)),((_tmp57D->cmp=Cyc_Absyn_Eq,((
_tmp57D->bd=_tmpC3,_tmp57D)))))),((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(fenv->r,_tag_dyneither(_tmp57C,sizeof(
struct Cyc_CfFlowInfo_TagCmp*),1)))),_tmp57E)))),_tmp57B))));}_LL41:;_LL42: return
leafval;_LL38:;}_LL34:{struct Cyc_Absyn_PointerType_struct*_tmpC4=(struct Cyc_Absyn_PointerType_struct*)
_tmpB0;if(_tmpC4->tag != 5)goto _LL36;else{_tmpC5=_tmpC4->f1;_tmpC6=_tmpC5.ptr_atts;
_tmpC7=_tmpC6.nullable;}}if(!(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmpC7)))goto _LL36;_LL35:{void*_tmpDC=leafval;enum Cyc_CfFlowInfo_InitLevel
_tmpDE;enum Cyc_CfFlowInfo_InitLevel _tmpE0;_LL44: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmpDD=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpDC;if(_tmpDD->tag != 3)goto
_LL46;else{_tmpDE=_tmpDD->f1;if(_tmpDE != Cyc_CfFlowInfo_ThisIL)goto _LL46;}}_LL45:
return fenv->notzerothis;_LL46: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpDF=(
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpDC;if(_tmpDF->tag != 3)goto _LL48;else{
_tmpE0=_tmpDF->f1;if(_tmpE0 != Cyc_CfFlowInfo_AllIL)goto _LL48;}}_LL47: return fenv->notzeroall;
_LL48:;_LL49: goto _LL43;_LL43:;}goto _LL27;_LL36:;_LL37: goto _LL27;_LL27:;}return
Cyc_Tcutil_bits_only(t)?fenv->unknown_all: leafval;}void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,void*leafval);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,void*leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,1,t,leafval);}struct _tuple20{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*
f2;};static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,
struct Cyc_Dict_Dict set);static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Dict_Dict set){struct _RegionHandle _tmpE1=_new_region("r");struct
_RegionHandle*r=& _tmpE1;_push_region(r);{struct _tuple20 _tmp57F;struct _tuple20
_tmpE2=(_tmp57F.f1=place,((_tmp57F.f2=0,_tmp57F)));struct Cyc_Iter_Iter _tmpE3=((
struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
r,set);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple20*))Cyc_Iter_next)(_tmpE3,&
_tmpE2)){struct Cyc_CfFlowInfo_Place*_tmpE4=_tmpE2.f1;void*_tmpE6;struct Cyc_List_List*
_tmpE7;struct Cyc_CfFlowInfo_Place _tmpE5=*place;_tmpE6=_tmpE5.root;_tmpE7=_tmpE5.fields;{
void*_tmpE9;struct Cyc_List_List*_tmpEA;struct Cyc_CfFlowInfo_Place _tmpE8=*_tmpE4;
_tmpE9=_tmpE8.root;_tmpEA=_tmpE8.fields;if(Cyc_CfFlowInfo_root_cmp(_tmpE6,_tmpE9)
!= 0)continue;for(0;_tmpE7 != 0  && _tmpEA != 0;(_tmpE7=_tmpE7->tl,_tmpEA=_tmpEA->tl)){
if((int)_tmpE7->hd != (int)_tmpEA->hd)break;}if(_tmpE7 == 0){int _tmpEB=1;
_npop_handler(0);return _tmpEB;}}}{int _tmpEC=0;_npop_handler(0);return _tmpEC;}};
_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*
places;};static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,
struct Cyc_CfFlowInfo_Place*place);static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*
pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place)){struct Cyc_List_List*
_tmp580;pile->places=((_tmp580=_region_malloc(pile->rgn,sizeof(*_tmp580)),((
_tmp580->hd=place,((_tmp580->tl=pile->places,_tmp580))))));}}static void Cyc_CfFlowInfo_add_places(
struct Cyc_CfFlowInfo_EscPile*pile,void*r);static void Cyc_CfFlowInfo_add_places(
struct Cyc_CfFlowInfo_EscPile*pile,void*r){void*_tmpEF=r;struct Cyc_CfFlowInfo_Place*
_tmpF1;int _tmpF3;struct _dyneither_ptr _tmpF4;_LL4B: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmpF0=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpEF;if(_tmpF0->tag != 5)goto
_LL4D;else{_tmpF1=_tmpF0->f1;}}_LL4C: Cyc_CfFlowInfo_add_place(pile,_tmpF1);
return;_LL4D: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmpF2=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpEF;if(_tmpF2->tag != 7)goto _LL4F;else{_tmpF3=_tmpF2->f1;_tmpF4=_tmpF2->f2;}}
_LL4E:{int i=0;for(0;i < _get_dyneither_size(_tmpF4,sizeof(void*));++ i){Cyc_CfFlowInfo_add_places(
pile,((void**)_tmpF4.curr)[i]);}}return;_LL4F:;_LL50: return;_LL4A:;}static void*
Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*
old_val);static void _tmp586(struct _RegionHandle**r,void**new_val,struct
_dyneither_ptr*_tmpF8,unsigned int*_tmp585,unsigned int*_tmp584,void***_tmp582){
for(*_tmp585=0;*_tmp585 < *_tmp584;(*_tmp585)++){(*_tmp582)[*_tmp585]=Cyc_CfFlowInfo_insert_place_inner(*
r,*new_val,((void**)(*_tmpF8).curr)[(int)*_tmp585]);}}static void*Cyc_CfFlowInfo_insert_place_inner(
struct _RegionHandle*r,void*new_val,void*old_val){void*_tmpF5=old_val;int _tmpF7;
struct _dyneither_ptr _tmpF8;_LL52: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmpF6=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpF5;if(_tmpF6->tag != 7)goto _LL54;else{
_tmpF7=_tmpF6->f1;_tmpF8=_tmpF6->f2;}}_LL53: {unsigned int _tmp585;unsigned int
_tmp584;struct _dyneither_ptr _tmp583;void**_tmp582;unsigned int _tmp581;struct
_dyneither_ptr d2=(_tmp581=_get_dyneither_size(_tmpF8,sizeof(void*)),((_tmp582=(
void**)_region_malloc(r,_check_times(sizeof(void*),_tmp581)),((_tmp583=
_tag_dyneither(_tmp582,sizeof(void*),_tmp581),((((_tmp584=_tmp581,_tmp586(& r,&
new_val,& _tmpF8,& _tmp585,& _tmp584,& _tmp582))),_tmp583)))))));struct Cyc_CfFlowInfo_Aggregate_struct
_tmp589;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp588;return(void*)((_tmp588=
_region_malloc(r,sizeof(*_tmp588)),((_tmp588[0]=((_tmp589.tag=7,((_tmp589.f1=
_tmpF7,((_tmp589.f2=d2,_tmp589)))))),_tmp588))));}_LL54:;_LL55: return new_val;
_LL51:;}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval);static void _tmp58F(struct
_dyneither_ptr*d,unsigned int*_tmp58E,unsigned int*_tmp58D,void***_tmp58B){for(*
_tmp58E=0;*_tmp58E < *_tmp58D;(*_tmp58E)++){(*_tmp58B)[*_tmp58E]=((void**)(*d).curr)[(
int)*_tmp58E];}}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){void*_tmpFF=*((void**)
_check_dyneither_subscript(d,sizeof(void*),n));if(_tmpFF == rval)return d;{
unsigned int _tmp58E;unsigned int _tmp58D;struct _dyneither_ptr _tmp58C;void**
_tmp58B;unsigned int _tmp58A;struct _dyneither_ptr res=(_tmp58A=_get_dyneither_size(
d,sizeof(void*)),((_tmp58B=(void**)_region_malloc(r,_check_times(sizeof(void*),
_tmp58A)),((_tmp58C=_tag_dyneither(_tmp58B,sizeof(void*),_tmp58A),((((_tmp58D=
_tmp58A,_tmp58F(& d,& _tmp58E,& _tmp58D,& _tmp58B))),_tmp58C)))))));*((void**)
_check_dyneither_subscript(res,sizeof(void*),n))=rval;return res;}}struct _tuple21{
struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val);static
void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*
fs,void*old_val,void*new_val){if(fs == 0)return Cyc_CfFlowInfo_insert_place_inner(
r,new_val,old_val);{struct _tuple21 _tmp590;struct _tuple21 _tmp105=(_tmp590.f1=fs,((
_tmp590.f2=old_val,_tmp590)));struct Cyc_List_List*_tmp106;struct Cyc_List_List
_tmp107;int _tmp108;struct Cyc_List_List*_tmp109;void*_tmp10A;int _tmp10C;struct
_dyneither_ptr _tmp10D;_LL57: _tmp106=_tmp105.f1;if(_tmp106 == 0)goto _LL59;_tmp107=*
_tmp106;_tmp108=(int)_tmp107.hd;_tmp109=_tmp107.tl;_tmp10A=_tmp105.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp10B=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp10A;if(_tmp10B->tag != 7)goto
_LL59;else{_tmp10C=_tmp10B->f1;_tmp10D=_tmp10B->f2;}}_LL58: {void*_tmp10E=Cyc_CfFlowInfo_insert_place_outer(
r,_tmp109,*((void**)_check_dyneither_subscript(_tmp10D,sizeof(void*),_tmp108)),
new_val);struct _dyneither_ptr _tmp10F=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp10D,
_tmp108,_tmp10E);if(_tmp10F.curr == _tmp10D.curr)return old_val;else{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp593;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp592;return(void*)((_tmp592=
_region_malloc(r,sizeof(*_tmp592)),((_tmp592[0]=((_tmp593.tag=7,((_tmp593.f1=
_tmp10C,((_tmp593.f2=_tmp10F,_tmp593)))))),_tmp592))));}}_LL59:;_LL5A: {struct
Cyc_Core_Impossible_struct _tmp599;const char*_tmp598;struct Cyc_Core_Impossible_struct*
_tmp597;(int)_throw((void*)((_tmp597=_cycalloc(sizeof(*_tmp597)),((_tmp597[0]=((
_tmp599.tag=Cyc_Core_Impossible,((_tmp599.f1=((_tmp598="bad insert place",
_tag_dyneither(_tmp598,sizeof(char),17))),_tmp599)))),_tmp597)))));}_LL56:;}}
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict
d);static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict
d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*_tmp115=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(pile->places))->hd;pile->places=((struct Cyc_List_List*)
_check_null(pile->places))->tl;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
all_changed,_tmp115,0);{void*oldval;void*newval;{struct _handler_cons _tmp116;
_push_handler(& _tmp116);{int _tmp118=0;if(setjmp(_tmp116.handler))_tmp118=1;if(!
_tmp118){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmp115);;_pop_handler();}else{
void*_tmp117=(void*)_exn_thrown;void*_tmp11A=_tmp117;_LL5C: {struct Cyc_Dict_Absent_struct*
_tmp11B=(struct Cyc_Dict_Absent_struct*)_tmp11A;if(_tmp11B->tag != Cyc_Dict_Absent)
goto _LL5E;}_LL5D: continue;_LL5E:;_LL5F:(void)_throw(_tmp11A);_LL5B:;}}}switch(
Cyc_CfFlowInfo_initlevel(fenv,d,oldval)){case Cyc_CfFlowInfo_AllIL: _LL60: newval=
fenv->esc_all;break;case Cyc_CfFlowInfo_ThisIL: _LL61: newval=fenv->esc_this;break;
case Cyc_CfFlowInfo_NoneIL: _LL62: newval=fenv->esc_none;break;}Cyc_CfFlowInfo_add_places(
pile,oldval);{void*_tmp11D;struct Cyc_List_List*_tmp11E;struct Cyc_CfFlowInfo_Place
_tmp11C=*_tmp115;_tmp11D=_tmp11C.root;_tmp11E=_tmp11C.fields;d=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp11D,Cyc_CfFlowInfo_insert_place_outer(
fenv->r,_tmp11E,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,
_tmp11D),newval));}}}return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct
_RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};static enum Cyc_CfFlowInfo_InitLevel
Cyc_CfFlowInfo_initlevel_approx(void*r);static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(
void*r){void*_tmp11F=r;enum Cyc_CfFlowInfo_InitLevel _tmp121;enum Cyc_CfFlowInfo_InitLevel
_tmp123;_LL65: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp120=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp11F;if(_tmp120->tag != 3)goto _LL67;else{_tmp121=_tmp120->f1;}}_LL66: return
_tmp121;_LL67: {struct Cyc_CfFlowInfo_Esc_struct*_tmp122=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp11F;if(_tmp122->tag != 4)goto _LL69;else{_tmp123=_tmp122->f1;}}_LL68: return
_tmp123;_LL69: {struct Cyc_CfFlowInfo_Zero_struct*_tmp124=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp11F;if(_tmp124->tag != 0)goto _LL6B;}_LL6A: goto _LL6C;_LL6B: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp125=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp11F;if(_tmp125->tag != 1)
goto _LL6D;}_LL6C: return Cyc_CfFlowInfo_AllIL;_LL6D: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp126=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp11F;if(_tmp126->tag != 2)
goto _LL6F;}_LL6E: return Cyc_CfFlowInfo_ThisIL;_LL6F: {struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp127=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp11F;if(_tmp127->tag != 6)goto
_LL71;}_LL70: return Cyc_CfFlowInfo_AllIL;_LL71:;_LL72: {struct Cyc_Core_Impossible_struct
_tmp59F;const char*_tmp59E;struct Cyc_Core_Impossible_struct*_tmp59D;(int)_throw((
void*)((_tmp59D=_cycalloc(sizeof(*_tmp59D)),((_tmp59D[0]=((_tmp59F.tag=Cyc_Core_Impossible,((
_tmp59F.f1=((_tmp59E="initlevel_approx",_tag_dyneither(_tmp59E,sizeof(char),17))),
_tmp59F)))),_tmp59D)))));}_LL64:;}static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(
struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc);
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*
env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){enum Cyc_CfFlowInfo_InitLevel
this_ans;if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;{void*
_tmp12B=r;int _tmp12D;struct _dyneither_ptr _tmp12E;int _tmp130;struct _dyneither_ptr
_tmp131;struct Cyc_CfFlowInfo_Place*_tmp133;_LL74: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp12C=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp12B;if(_tmp12C->tag != 7)goto
_LL76;else{_tmp12D=_tmp12C->f1;if(_tmp12D != 1)goto _LL76;_tmp12E=_tmp12C->f2;}}
_LL75: {unsigned int _tmp134=_get_dyneither_size(_tmp12E,sizeof(void*));this_ans=
Cyc_CfFlowInfo_NoneIL;{int i=0;for(0;i < _tmp134;++ i){if(Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmp12E,sizeof(void*),i)),Cyc_CfFlowInfo_AllIL)
== Cyc_CfFlowInfo_AllIL){this_ans=Cyc_CfFlowInfo_AllIL;break;}}}goto _LL73;}_LL76: {
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp12F=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp12B;if(_tmp12F->tag != 7)goto _LL78;else{_tmp130=_tmp12F->f1;if(_tmp130 != 0)
goto _LL78;_tmp131=_tmp12F->f2;}}_LL77: {unsigned int _tmp135=_get_dyneither_size(
_tmp131,sizeof(void*));this_ans=Cyc_CfFlowInfo_AllIL;{int i=0;for(0;i < _tmp135;++
i){this_ans=Cyc_CfFlowInfo_initlevel_rec(env,*((void**)
_check_dyneither_subscript(_tmp131,sizeof(void*),i)),this_ans);}}goto _LL73;}
_LL78: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp132=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp12B;if(_tmp132->tag != 5)goto _LL7A;else{_tmp133=_tmp132->f1;}}_LL79: if(((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmp133))this_ans=Cyc_CfFlowInfo_AllIL;else{{struct Cyc_List_List*_tmp5A0;env->seen=((
_tmp5A0=_region_malloc(env->e,sizeof(*_tmp5A0)),((_tmp5A0->hd=_tmp133,((_tmp5A0->tl=
env->seen,_tmp5A0))))));}this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(
env->d,_tmp133),Cyc_CfFlowInfo_AllIL);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == Cyc_CfFlowInfo_NoneIL)this_ans=Cyc_CfFlowInfo_ThisIL;}
goto _LL73;_LL7A:;_LL7B: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL73:;}if(
this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;if(this_ans == Cyc_CfFlowInfo_ThisIL
 || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;return Cyc_CfFlowInfo_AllIL;}
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*
env,struct Cyc_Dict_Dict d,void*r);enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(
struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){struct
_RegionHandle*_tmp137=env->r;struct Cyc_CfFlowInfo_InitlevelEnv _tmp5A1;struct Cyc_CfFlowInfo_InitlevelEnv
_tmp138=(_tmp5A1.e=_tmp137,((_tmp5A1.d=d,((_tmp5A1.seen=0,_tmp5A1)))));return Cyc_CfFlowInfo_initlevel_rec(&
_tmp138,r,Cyc_CfFlowInfo_AllIL);}struct _tuple22{void*f1;int f2;};void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){void*_tmp13B;struct Cyc_List_List*
_tmp13C;struct Cyc_CfFlowInfo_Place _tmp13A=*place;_tmp13B=_tmp13A.root;_tmp13C=
_tmp13A.fields;{void*_tmp13D=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
d,_tmp13B);for(0;_tmp13C != 0;_tmp13C=_tmp13C->tl){struct _tuple22 _tmp5A2;struct
_tuple22 _tmp13F=(_tmp5A2.f1=_tmp13D,((_tmp5A2.f2=(int)_tmp13C->hd,_tmp5A2)));
void*_tmp140;int _tmp142;struct _dyneither_ptr _tmp143;int _tmp144;_LL7D: _tmp140=
_tmp13F.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp141=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp140;if(_tmp141->tag != 7)goto _LL7F;else{_tmp142=_tmp141->f1;_tmp143=_tmp141->f2;}}
_tmp144=_tmp13F.f2;_LL7E: _tmp13D=*((void**)_check_dyneither_subscript(_tmp143,
sizeof(void*),_tmp144));goto _LL7C;_LL7F:;_LL80: {struct Cyc_Core_Impossible_struct
_tmp5A8;const char*_tmp5A7;struct Cyc_Core_Impossible_struct*_tmp5A6;(int)_throw((
void*)((_tmp5A6=_cycalloc(sizeof(*_tmp5A6)),((_tmp5A6[0]=((_tmp5A8.tag=Cyc_Core_Impossible,((
_tmp5A8.f1=((_tmp5A7="bad lookup_place",_tag_dyneither(_tmp5A7,sizeof(char),17))),
_tmp5A8)))),_tmp5A6)))));}_LL7C:;}return _tmp13D;}}static int Cyc_CfFlowInfo_is_rval_unescaped(
void*rval);static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){void*_tmp148=
rval;int _tmp14B;struct _dyneither_ptr _tmp14C;_LL82: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp149=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp148;if(_tmp149->tag != 4)goto _LL84;}
_LL83: return 0;_LL84: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp14A=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp148;if(_tmp14A->tag != 7)goto _LL86;else{_tmp14B=_tmp14A->f1;_tmp14C=_tmp14A->f2;}}
_LL85: {unsigned int sz=_get_dyneither_size(_tmp14C,sizeof(void*));{int i=0;for(0;
i < sz;++ i){if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp14C.curr)[i]))
return 0;}}return 1;}_LL86:;_LL87: return 1;_LL81:;}int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){return Cyc_CfFlowInfo_is_rval_unescaped(
Cyc_CfFlowInfo_lookup_place(d,place));}struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,void*r);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){struct
_RegionHandle*_tmp14D=fenv->r;struct Cyc_CfFlowInfo_EscPile*_tmp5A9;struct Cyc_CfFlowInfo_EscPile*
pile=(_tmp5A9=_region_malloc(_tmp14D,sizeof(*_tmp5A9)),((_tmp5A9->rgn=_tmp14D,((
_tmp5A9->places=0,_tmp5A9)))));Cyc_CfFlowInfo_add_places(pile,r);return Cyc_CfFlowInfo_escape_these(
fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*
fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*oldval,void*newval);static void _tmp5B3(struct Cyc_CfFlowInfo_AssignEnv**
env,unsigned int*_tmp5B2,unsigned int*_tmp5B1,void***_tmp5AF,struct
_dyneither_ptr*_tmp15F,struct _dyneither_ptr*_tmp15B){for(*_tmp5B2=0;*_tmp5B2 < *
_tmp5B1;(*_tmp5B2)++){(*_tmp5AF)[*_tmp5B2]=Cyc_CfFlowInfo_assign_place_inner(*
env,((void**)(*_tmp15B).curr)[(int)*_tmp5B2],*((void**)
_check_dyneither_subscript(*_tmp15F,sizeof(void*),(int)*_tmp5B2)));}}static void*
Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,
void*newval){struct _tuple0 _tmp5AA;struct _tuple0 _tmp150=(_tmp5AA.f1=oldval,((
_tmp5AA.f2=newval,_tmp5AA)));void*_tmp151;void*_tmp153;struct Cyc_CfFlowInfo_Place*
_tmp155;void*_tmp156;void*_tmp158;int _tmp15A;struct _dyneither_ptr _tmp15B;void*
_tmp15C;int _tmp15E;struct _dyneither_ptr _tmp15F;void*_tmp160;enum Cyc_CfFlowInfo_InitLevel
_tmp162;_LL89: _tmp151=_tmp150.f1;{struct Cyc_CfFlowInfo_Esc_struct*_tmp152=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp151;if(_tmp152->tag != 4)goto _LL8B;}_tmp153=
_tmp150.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp154=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp153;if(_tmp154->tag != 5)goto _LL8B;else{_tmp155=_tmp154->f1;}}_LL8A: Cyc_CfFlowInfo_add_place(
env->pile,_tmp155);goto _LL8C;_LL8B: _tmp156=_tmp150.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp157=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp156;if(_tmp157->tag != 4)goto _LL8D;}
_LL8C: if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp5AD;void*_tmp5AC;(_tmp5AC=0,Cyc_Tcutil_terr(env->loc,((_tmp5AD="assignment puts possibly-uninitialized data in an escaped location",
_tag_dyneither(_tmp5AD,sizeof(char),67))),_tag_dyneither(_tmp5AC,sizeof(void*),0)));}
return(env->fenv)->esc_all;_LL8D: _tmp158=_tmp150.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp159=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp158;if(_tmp159->tag != 7)goto
_LL8F;else{_tmp15A=_tmp159->f1;_tmp15B=_tmp159->f2;}}_tmp15C=_tmp150.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp15D=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp15C;if(_tmp15D->tag != 7)goto _LL8F;else{_tmp15E=_tmp15D->f1;_tmp15F=_tmp15D->f2;}}
_LL8E: {unsigned int _tmp5B2;unsigned int _tmp5B1;struct _dyneither_ptr _tmp5B0;void**
_tmp5AF;unsigned int _tmp5AE;struct _dyneither_ptr new_d=(_tmp5AE=
_get_dyneither_size(_tmp15B,sizeof(void*)),((_tmp5AF=(void**)_region_malloc((env->fenv)->r,
_check_times(sizeof(void*),_tmp5AE)),((_tmp5B0=_tag_dyneither(_tmp5AF,sizeof(
void*),_tmp5AE),((((_tmp5B1=_tmp5AE,_tmp5B3(& env,& _tmp5B2,& _tmp5B1,& _tmp5AF,&
_tmp15F,& _tmp15B))),_tmp5B0)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp15B,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp15B.curr)[i]){
change=1;break;}}}if(!change)return oldval;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp15B,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp15F,sizeof(void*),i))){change=1;break;}}}if(!
change)return newval;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp5B6;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5B5;return(void*)((_tmp5B5=_region_malloc((env->fenv)->r,sizeof(*_tmp5B5)),((
_tmp5B5[0]=((_tmp5B6.tag=7,((_tmp5B6.f1=_tmp15A,((_tmp5B6.f2=new_d,_tmp5B6)))))),
_tmp5B5))));}}_LL8F: _tmp160=_tmp150.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp161=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp160;if(_tmp161->tag != 4)goto _LL91;else{
_tmp162=_tmp161->f1;}}_LL90: switch(_tmp162){case Cyc_CfFlowInfo_NoneIL: _LL93:
return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL: _LL94: return(env->fenv)->unknown_this;
case Cyc_CfFlowInfo_AllIL: _LL95: return(env->fenv)->unknown_all;}_LL91:;_LL92:
return newval;_LL88:;}static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*
env,struct Cyc_List_List*fs,void*oldval,void*newval);static void*Cyc_CfFlowInfo_assign_place_outer(
struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,void*oldval,void*
newval){if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple21 _tmp5B7;struct _tuple21 _tmp16C=(_tmp5B7.f1=fs,((_tmp5B7.f2=oldval,
_tmp5B7)));struct Cyc_List_List*_tmp16D;struct Cyc_List_List _tmp16E;int _tmp16F;
struct Cyc_List_List*_tmp170;void*_tmp171;int _tmp173;struct _dyneither_ptr _tmp174;
_LL98: _tmp16D=_tmp16C.f1;if(_tmp16D == 0)goto _LL9A;_tmp16E=*_tmp16D;_tmp16F=(int)
_tmp16E.hd;_tmp170=_tmp16E.tl;_tmp171=_tmp16C.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp172=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp171;if(_tmp172->tag != 7)goto
_LL9A;else{_tmp173=_tmp172->f1;_tmp174=_tmp172->f2;}}_LL99: {void*_tmp175=Cyc_CfFlowInfo_assign_place_outer(
env,_tmp170,*((void**)_check_dyneither_subscript(_tmp174,sizeof(void*),_tmp16F)),
newval);struct _dyneither_ptr _tmp176=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,
_tmp174,_tmp16F,_tmp175);if(_tmp176.curr == _tmp174.curr)return oldval;else{struct
Cyc_CfFlowInfo_Aggregate_struct _tmp5BA;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5B9;return(void*)((_tmp5B9=_region_malloc((env->fenv)->r,sizeof(*_tmp5B9)),((
_tmp5B9[0]=((_tmp5BA.tag=7,((_tmp5BA.f1=_tmp173,((_tmp5BA.f2=_tmp176,_tmp5BA)))))),
_tmp5B9))));}}_LL9A:;_LL9B: {struct Cyc_Core_Impossible_struct _tmp5C0;const char*
_tmp5BF;struct Cyc_Core_Impossible_struct*_tmp5BE;(int)_throw((void*)((_tmp5BE=
_cycalloc(sizeof(*_tmp5BE)),((_tmp5BE[0]=((_tmp5C0.tag=Cyc_Core_Impossible,((
_tmp5C0.f1=((_tmp5BF="bad assign place",_tag_dyneither(_tmp5BF,sizeof(char),17))),
_tmp5C0)))),_tmp5BE)))));}_LL97:;}}struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){((int(*)(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
all_changed,place,0);{struct _RegionHandle*_tmp17C=fenv->r;struct Cyc_CfFlowInfo_Place
_tmp17E;void*_tmp17F;struct Cyc_List_List*_tmp180;struct Cyc_CfFlowInfo_Place*
_tmp17D=place;_tmp17E=*_tmp17D;_tmp17F=_tmp17E.root;_tmp180=_tmp17E.fields;{
struct Cyc_CfFlowInfo_EscPile*_tmp5C3;struct Cyc_CfFlowInfo_AssignEnv _tmp5C2;
struct Cyc_CfFlowInfo_AssignEnv env=(_tmp5C2.fenv=fenv,((_tmp5C2.pile=((_tmp5C3=
_region_malloc(_tmp17C,sizeof(*_tmp5C3)),((_tmp5C3->rgn=_tmp17C,((_tmp5C3->places=
0,_tmp5C3)))))),((_tmp5C2.d=d,((_tmp5C2.loc=loc,_tmp5C2)))))));void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmp180,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp17F),
r);return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp17F,newval));}}}
struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict
chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2);static struct Cyc_List_List*Cyc_CfFlowInfo_join_tag_cmps(
struct _RegionHandle*r,struct Cyc_List_List*l1,struct Cyc_List_List*l2);static
struct Cyc_List_List*Cyc_CfFlowInfo_join_tag_cmps(struct _RegionHandle*r,struct Cyc_List_List*
l1,struct Cyc_List_List*l2){if(l1 == l2)return l1;{struct Cyc_List_List*_tmp183=0;
for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp _tmp185;enum Cyc_Absyn_Primop
_tmp186;void*_tmp187;struct Cyc_CfFlowInfo_TagCmp*_tmp184=(struct Cyc_CfFlowInfo_TagCmp*)
l2->hd;_tmp185=*_tmp184;_tmp186=_tmp185.cmp;_tmp187=_tmp185.bd;{int found=0;enum 
Cyc_Absyn_Primop joined_cmp=Cyc_Absyn_Lte;{struct Cyc_List_List*_tmp188=l1;for(0;
_tmp188 != 0;_tmp188=_tmp188->tl){struct Cyc_CfFlowInfo_TagCmp _tmp18A;enum Cyc_Absyn_Primop
_tmp18B;void*_tmp18C;struct Cyc_CfFlowInfo_TagCmp*_tmp189=(struct Cyc_CfFlowInfo_TagCmp*)
_tmp188->hd;_tmp18A=*_tmp189;_tmp18B=_tmp18A.cmp;_tmp18C=_tmp18A.bd;if(Cyc_Tcutil_typecmp(
_tmp187,_tmp18C)== 0){found=1;if(_tmp18B == _tmp186){joined_cmp=_tmp18B;break;}}}}
if(found){struct Cyc_CfFlowInfo_TagCmp*_tmp5C6;struct Cyc_List_List*_tmp5C5;
_tmp183=((_tmp5C5=_region_malloc(r,sizeof(*_tmp5C5)),((_tmp5C5->hd=((_tmp5C6=
_region_malloc(r,sizeof(*_tmp5C6)),((_tmp5C6->cmp=joined_cmp,((_tmp5C6->bd=
_tmp187,_tmp5C6)))))),((_tmp5C5->tl=_tmp183,_tmp5C5))))));}}}return _tmp183;}}
struct _tuple23{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;}
;static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,
void*r1,void*r2);static void _tmp5CD(struct Cyc_CfFlowInfo_JoinEnv**env,
unsigned int*_tmp5CC,unsigned int*_tmp5CB,void***_tmp5C9,struct _dyneither_ptr*
_tmp1AD,struct _dyneither_ptr*_tmp1A9){for(*_tmp5CC=0;*_tmp5CC < *_tmp5CB;(*
_tmp5CC)++){(*_tmp5C9)[*_tmp5CC]=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int
a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp1A9).curr)[(
int)*_tmp5CC],*((void**)_check_dyneither_subscript(*_tmp1AD,sizeof(void*),(int)*
_tmp5CC)));}}static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*
env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{struct _RegionHandle*_tmp18F=(
env->fenv)->r;{struct _tuple0 _tmp5C7;struct _tuple0 _tmp191=(_tmp5C7.f1=r1,((
_tmp5C7.f2=r2,_tmp5C7)));void*_tmp192;struct Cyc_CfFlowInfo_Place*_tmp194;void*
_tmp195;struct Cyc_CfFlowInfo_Place*_tmp197;void*_tmp198;struct Cyc_CfFlowInfo_Place*
_tmp19A;void*_tmp19B;struct Cyc_CfFlowInfo_Place*_tmp19D;void*_tmp19E;void*
_tmp1A0;void*_tmp1A2;void*_tmp1A4;void*_tmp1A6;int _tmp1A8;struct _dyneither_ptr
_tmp1A9;void*_tmp1AA;int _tmp1AC;struct _dyneither_ptr _tmp1AD;void*_tmp1AE;struct
Cyc_List_List*_tmp1B0;void*_tmp1B1;struct Cyc_List_List*_tmp1B3;void*_tmp1B4;void*
_tmp1B6;_LL9D: _tmp192=_tmp191.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp193=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp192;if(_tmp193->tag != 5)goto _LL9F;
else{_tmp194=_tmp193->f1;}}_tmp195=_tmp191.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp196=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp195;if(_tmp196->tag != 5)goto
_LL9F;else{_tmp197=_tmp196->f1;}}_LL9E: if(Cyc_CfFlowInfo_place_cmp(_tmp194,
_tmp197)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmp194);Cyc_CfFlowInfo_add_place(
env->pile,_tmp197);goto _LL9C;_LL9F: _tmp198=_tmp191.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp199=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp198;if(_tmp199->tag != 5)goto
_LLA1;else{_tmp19A=_tmp199->f1;}}_LLA0: Cyc_CfFlowInfo_add_place(env->pile,
_tmp19A);goto _LL9C;_LLA1: _tmp19B=_tmp191.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp19C=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp19B;if(_tmp19C->tag != 5)goto
_LLA3;else{_tmp19D=_tmp19C->f1;}}_LLA2: Cyc_CfFlowInfo_add_place(env->pile,
_tmp19D);goto _LL9C;_LLA3: _tmp19E=_tmp191.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp19F=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp19E;if(_tmp19F->tag != 1)
goto _LLA5;}_tmp1A0=_tmp191.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp1A1=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1A0;if(_tmp1A1->tag != 2)goto _LLA5;}
_LLA4: goto _LLA6;_LLA5: _tmp1A2=_tmp191.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1A3=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1A2;if(_tmp1A3->tag != 2)
goto _LLA7;}_tmp1A4=_tmp191.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp1A5=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp1A4;if(_tmp1A5->tag != 1)goto _LLA7;}
_LLA6: return(env->fenv)->notzerothis;_LLA7: _tmp1A6=_tmp191.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp1A7=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1A6;if(_tmp1A7->tag != 7)goto
_LLA9;else{_tmp1A8=_tmp1A7->f1;_tmp1A9=_tmp1A7->f2;}}_tmp1AA=_tmp191.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp1AB=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1AA;if(_tmp1AB->tag != 7)goto _LLA9;else{_tmp1AC=_tmp1AB->f1;_tmp1AD=_tmp1AB->f2;}}
_LLA8: {unsigned int _tmp5CC;unsigned int _tmp5CB;struct _dyneither_ptr _tmp5CA;void**
_tmp5C9;unsigned int _tmp5C8;struct _dyneither_ptr new_d=(_tmp5C8=
_get_dyneither_size(_tmp1A9,sizeof(void*)),((_tmp5C9=(void**)_region_malloc(
_tmp18F,_check_times(sizeof(void*),_tmp5C8)),((_tmp5CA=_tag_dyneither(_tmp5C9,
sizeof(void*),_tmp5C8),((((_tmp5CB=_tmp5C8,_tmp5CD(& env,& _tmp5CC,& _tmp5CB,&
_tmp5C9,& _tmp1AD,& _tmp1A9))),_tmp5CA)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp1A9,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1A9.curr)[i]){
change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp1A9,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp1AD,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp5D0;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5CF;return(void*)((_tmp5CF=_region_malloc(_tmp18F,sizeof(*_tmp5CF)),((
_tmp5CF[0]=((_tmp5D0.tag=7,((_tmp5D0.f1=_tmp1A8,((_tmp5D0.f2=new_d,_tmp5D0)))))),
_tmp5CF))));}}_LLA9: _tmp1AE=_tmp191.f1;{struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp1AF=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp1AE;if(_tmp1AF->tag != 6)goto
_LLAB;else{_tmp1B0=_tmp1AF->f1;}}_tmp1B1=_tmp191.f2;{struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp1B2=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp1B1;if(_tmp1B2->tag != 6)goto
_LLAB;else{_tmp1B3=_tmp1B2->f1;}}_LLAA: {struct Cyc_List_List*_tmp1BE=Cyc_CfFlowInfo_join_tag_cmps(
_tmp18F,_tmp1B0,_tmp1B3);if(_tmp1BE == _tmp1B0)return r1;{struct Cyc_CfFlowInfo_TagCmps_struct
_tmp5D3;struct Cyc_CfFlowInfo_TagCmps_struct*_tmp5D2;return(void*)((_tmp5D2=
_region_malloc(_tmp18F,sizeof(*_tmp5D2)),((_tmp5D2[0]=((_tmp5D3.tag=6,((_tmp5D3.f1=
_tmp1BE,_tmp5D3)))),_tmp5D2))));}}_LLAB: _tmp1B4=_tmp191.f1;{struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp1B5=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp1B4;if(_tmp1B5->tag != 6)goto
_LLAD;}_LLAC: return r2;_LLAD: _tmp1B6=_tmp191.f2;{struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp1B7=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp1B6;if(_tmp1B7->tag != 6)goto
_LLAF;}_LLAE: return r1;_LLAF:;_LLB0: goto _LL9C;_LL9C:;}{enum Cyc_CfFlowInfo_InitLevel
il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel
il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);struct _tuple0 _tmp5D4;struct
_tuple0 _tmp1C2=(_tmp5D4.f1=r1,((_tmp5D4.f2=r2,_tmp5D4)));void*_tmp1C3;void*
_tmp1C5;_LLB2: _tmp1C3=_tmp1C2.f1;{struct Cyc_CfFlowInfo_Esc_struct*_tmp1C4=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp1C3;if(_tmp1C4->tag != 4)goto _LLB4;}_LLB3:
goto _LLB5;_LLB4: _tmp1C5=_tmp1C2.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp1C6=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmp1C5;if(_tmp1C6->tag != 4)goto _LLB6;}_LLB5: {
struct _tuple23 _tmp5D5;struct _tuple23 _tmp1C8=(_tmp5D5.f1=il1,((_tmp5D5.f2=il2,
_tmp5D5)));enum Cyc_CfFlowInfo_InitLevel _tmp1C9;enum Cyc_CfFlowInfo_InitLevel
_tmp1CA;enum Cyc_CfFlowInfo_InitLevel _tmp1CB;enum Cyc_CfFlowInfo_InitLevel
_tmp1CC;_LLB9: _tmp1C9=_tmp1C8.f2;if(_tmp1C9 != Cyc_CfFlowInfo_NoneIL)goto _LLBB;
_LLBA: goto _LLBC;_LLBB: _tmp1CA=_tmp1C8.f1;if(_tmp1CA != Cyc_CfFlowInfo_NoneIL)goto
_LLBD;_LLBC: return(env->fenv)->esc_none;_LLBD: _tmp1CB=_tmp1C8.f2;if(_tmp1CB != 
Cyc_CfFlowInfo_ThisIL)goto _LLBF;_LLBE: goto _LLC0;_LLBF: _tmp1CC=_tmp1C8.f1;if(
_tmp1CC != Cyc_CfFlowInfo_ThisIL)goto _LLC1;_LLC0: return(env->fenv)->esc_this;
_LLC1:;_LLC2: return(env->fenv)->esc_all;_LLB8:;}_LLB6:;_LLB7: {struct _tuple23
_tmp5D6;struct _tuple23 _tmp1CE=(_tmp5D6.f1=il1,((_tmp5D6.f2=il2,_tmp5D6)));enum 
Cyc_CfFlowInfo_InitLevel _tmp1CF;enum Cyc_CfFlowInfo_InitLevel _tmp1D0;enum Cyc_CfFlowInfo_InitLevel
_tmp1D1;enum Cyc_CfFlowInfo_InitLevel _tmp1D2;_LLC4: _tmp1CF=_tmp1CE.f2;if(_tmp1CF
!= Cyc_CfFlowInfo_NoneIL)goto _LLC6;_LLC5: goto _LLC7;_LLC6: _tmp1D0=_tmp1CE.f1;if(
_tmp1D0 != Cyc_CfFlowInfo_NoneIL)goto _LLC8;_LLC7: return(env->fenv)->unknown_none;
_LLC8: _tmp1D1=_tmp1CE.f2;if(_tmp1D1 != Cyc_CfFlowInfo_ThisIL)goto _LLCA;_LLC9: goto
_LLCB;_LLCA: _tmp1D2=_tmp1CE.f1;if(_tmp1D2 != Cyc_CfFlowInfo_ThisIL)goto _LLCC;
_LLCB: return(env->fenv)->unknown_this;_LLCC:;_LLCD: return(env->fenv)->unknown_all;
_LLC3:;}_LLB1:;}}}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(
unsigned int i);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(unsigned int i){
union Cyc_CfFlowInfo_RelnOp _tmp5D7;return((_tmp5D7.EqualConst).val=i,(((_tmp5D7.EqualConst).tag=
1,_tmp5D7)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*
v,void*t);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*
v,void*t){struct _tuple11 _tmp5DA;union Cyc_CfFlowInfo_RelnOp _tmp5D9;return((
_tmp5D9.LessVar).val=((_tmp5DA.f1=v,((_tmp5DA.f2=t,_tmp5DA)))),(((_tmp5D9.LessVar).tag=
2,_tmp5D9)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*
v);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*
v){union Cyc_CfFlowInfo_RelnOp _tmp5DB;return((_tmp5DB.LessNumelts).val=v,(((
_tmp5DB.LessNumelts).tag=3,_tmp5DB)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(
unsigned int i);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int i){
union Cyc_CfFlowInfo_RelnOp _tmp5DC;return((_tmp5DC.LessConst).val=i,(((_tmp5DC.LessConst).tag=
4,_tmp5DC)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*
v);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*
v){union Cyc_CfFlowInfo_RelnOp _tmp5DD;return((_tmp5DD.LessEqNumelts).val=v,(((
_tmp5DD.LessEqNumelts).tag=5,_tmp5DD)));}struct _tuple24{union Cyc_CfFlowInfo_RelnOp
f1;union Cyc_CfFlowInfo_RelnOp f2;};static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2);static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2){struct _tuple24 _tmp5DE;struct _tuple24 _tmp1DA=(
_tmp5DE.f1=r1,((_tmp5DE.f2=r2,_tmp5DE)));union Cyc_CfFlowInfo_RelnOp _tmp1DB;
unsigned int _tmp1DC;union Cyc_CfFlowInfo_RelnOp _tmp1DD;unsigned int _tmp1DE;union
Cyc_CfFlowInfo_RelnOp _tmp1DF;struct _tuple11 _tmp1E0;struct Cyc_Absyn_Vardecl*
_tmp1E1;union Cyc_CfFlowInfo_RelnOp _tmp1E2;struct _tuple11 _tmp1E3;struct Cyc_Absyn_Vardecl*
_tmp1E4;union Cyc_CfFlowInfo_RelnOp _tmp1E5;struct Cyc_Absyn_Vardecl*_tmp1E6;union
Cyc_CfFlowInfo_RelnOp _tmp1E7;struct Cyc_Absyn_Vardecl*_tmp1E8;union Cyc_CfFlowInfo_RelnOp
_tmp1E9;unsigned int _tmp1EA;union Cyc_CfFlowInfo_RelnOp _tmp1EB;unsigned int
_tmp1EC;union Cyc_CfFlowInfo_RelnOp _tmp1ED;struct Cyc_Absyn_Vardecl*_tmp1EE;union
Cyc_CfFlowInfo_RelnOp _tmp1EF;struct Cyc_Absyn_Vardecl*_tmp1F0;_LLCF: _tmp1DB=
_tmp1DA.f1;if((_tmp1DB.EqualConst).tag != 1)goto _LLD1;_tmp1DC=(unsigned int)(
_tmp1DB.EqualConst).val;_tmp1DD=_tmp1DA.f2;if((_tmp1DD.EqualConst).tag != 1)goto
_LLD1;_tmp1DE=(unsigned int)(_tmp1DD.EqualConst).val;_LLD0: return _tmp1DC == 
_tmp1DE;_LLD1: _tmp1DF=_tmp1DA.f1;if((_tmp1DF.LessVar).tag != 2)goto _LLD3;_tmp1E0=(
struct _tuple11)(_tmp1DF.LessVar).val;_tmp1E1=_tmp1E0.f1;_tmp1E2=_tmp1DA.f2;if((
_tmp1E2.LessVar).tag != 2)goto _LLD3;_tmp1E3=(struct _tuple11)(_tmp1E2.LessVar).val;
_tmp1E4=_tmp1E3.f1;_LLD2: return _tmp1E1 == _tmp1E4;_LLD3: _tmp1E5=_tmp1DA.f1;if((
_tmp1E5.LessNumelts).tag != 3)goto _LLD5;_tmp1E6=(struct Cyc_Absyn_Vardecl*)(
_tmp1E5.LessNumelts).val;_tmp1E7=_tmp1DA.f2;if((_tmp1E7.LessNumelts).tag != 3)
goto _LLD5;_tmp1E8=(struct Cyc_Absyn_Vardecl*)(_tmp1E7.LessNumelts).val;_LLD4:
return _tmp1E6 == _tmp1E8;_LLD5: _tmp1E9=_tmp1DA.f1;if((_tmp1E9.LessConst).tag != 4)
goto _LLD7;_tmp1EA=(unsigned int)(_tmp1E9.LessConst).val;_tmp1EB=_tmp1DA.f2;if((
_tmp1EB.LessConst).tag != 4)goto _LLD7;_tmp1EC=(unsigned int)(_tmp1EB.LessConst).val;
_LLD6: return _tmp1EA == _tmp1EC;_LLD7: _tmp1ED=_tmp1DA.f1;if((_tmp1ED.LessEqNumelts).tag
!= 5)goto _LLD9;_tmp1EE=(struct Cyc_Absyn_Vardecl*)(_tmp1ED.LessEqNumelts).val;
_tmp1EF=_tmp1DA.f2;if((_tmp1EF.LessEqNumelts).tag != 5)goto _LLD9;_tmp1F0=(struct
Cyc_Absyn_Vardecl*)(_tmp1EF.LessEqNumelts).val;_LLD8: return _tmp1EE == _tmp1F0;
_LLD9:;_LLDA: return 0;_LLCE:;}static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(
struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s);static
struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct _RegionHandle*r,struct Cyc_List_List*
r1s,struct Cyc_List_List*r2s){if(r1s == r2s)return r1s;{struct Cyc_List_List*res=0;
int diff=0;{struct Cyc_List_List*_tmp1F1=r1s;for(0;_tmp1F1 != 0;_tmp1F1=_tmp1F1->tl){
struct Cyc_CfFlowInfo_Reln*_tmp1F2=(struct Cyc_CfFlowInfo_Reln*)_tmp1F1->hd;int
found=0;{struct Cyc_List_List*_tmp1F3=r2s;for(0;_tmp1F3 != 0;_tmp1F3=_tmp1F3->tl){
struct Cyc_CfFlowInfo_Reln*_tmp1F4=(struct Cyc_CfFlowInfo_Reln*)_tmp1F3->hd;if(
_tmp1F2 == _tmp1F4  || _tmp1F2->vd == _tmp1F4->vd  && Cyc_CfFlowInfo_same_relop(
_tmp1F2->rop,_tmp1F4->rop)){{struct Cyc_List_List*_tmp5DF;res=((_tmp5DF=
_region_malloc(r,sizeof(*_tmp5DF)),((_tmp5DF->hd=_tmp1F2,((_tmp5DF->tl=res,
_tmp5DF))))));}found=1;break;}}}if(!found)diff=1;}}if(!diff)res=r1s;return res;}}
static void Cyc_CfFlowInfo_remove_f(struct Cyc_List_List**l,struct Cyc_CfFlowInfo_Place*
place);static void Cyc_CfFlowInfo_remove_f(struct Cyc_List_List**l,struct Cyc_CfFlowInfo_Place*
place){struct _handler_cons _tmp1F6;_push_handler(& _tmp1F6);{int _tmp1F8=0;if(
setjmp(_tmp1F6.handler))_tmp1F8=1;if(!_tmp1F8){*l=((struct Cyc_List_List*(*)(int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_delete_cmp)(Cyc_CfFlowInfo_place_cmp,*l,
place);;_pop_handler();}else{void*_tmp1F7=(void*)_exn_thrown;void*_tmp1FA=
_tmp1F7;_LLDC: {struct Cyc_Core_Not_found_struct*_tmp1FB=(struct Cyc_Core_Not_found_struct*)
_tmp1FA;if(_tmp1FB->tag != Cyc_Core_Not_found)goto _LLDE;}_LLDD: goto _LLDB;_LLDE:;
_LLDF:(void)_throw(_tmp1FA);_LLDB:;}}}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_or_consume(
struct _RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2);static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_or_consume(struct
_RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*mc=c2.may_consume;struct _RegionHandle*_tmp1FC=r;{struct
Cyc_List_List*_tmp1FD=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rcopy)(_tmp1FC,c1.may_consume);((void(*)(void(*f)(struct Cyc_List_List**,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List**env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_CfFlowInfo_remove_f,& _tmp1FD,c2.may_consume);while(_tmp1FD != 0){{struct Cyc_List_List*
_tmp5E0;mc=((_tmp5E0=_region_malloc(r,sizeof(*_tmp5E0)),((_tmp5E0->hd=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmp1FD))->hd,((_tmp5E0->tl=mc,_tmp5E0))))));}
_tmp1FD=((struct Cyc_List_List*)_check_null(_tmp1FD))->tl;}}{struct Cyc_CfFlowInfo_ConsumeInfo
_tmp5E1;return(_tmp5E1.consumed=Cyc_CfFlowInfo_union_place_set(c1.consumed,c2.consumed,
0),((_tmp5E1.may_consume=mc,_tmp5E1)));}}struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(
struct _RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2);struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(struct
_RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*_tmp200=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,c1.may_consume,
c2.may_consume);struct Cyc_CfFlowInfo_ConsumeInfo _tmp5E2;return(_tmp5E2.consumed=
Cyc_CfFlowInfo_union_place_set(c1.consumed,c2.consumed,1),((_tmp5E2.may_consume=
_tmp200,_tmp5E2)));}struct _tuple25{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo
f2;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2,int or_consumed);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct
Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo
f1,union Cyc_CfFlowInfo_FlowInfo f2,int or_consumed){struct _tuple25 _tmp5E3;struct
_tuple25 _tmp203=(_tmp5E3.f1=f1,((_tmp5E3.f2=f2,_tmp5E3)));union Cyc_CfFlowInfo_FlowInfo
_tmp204;int _tmp205;union Cyc_CfFlowInfo_FlowInfo _tmp206;int _tmp207;union Cyc_CfFlowInfo_FlowInfo
_tmp208;struct _tuple12 _tmp209;struct Cyc_Dict_Dict _tmp20A;struct Cyc_List_List*
_tmp20B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp20C;union Cyc_CfFlowInfo_FlowInfo
_tmp20D;struct _tuple12 _tmp20E;struct Cyc_Dict_Dict _tmp20F;struct Cyc_List_List*
_tmp210;struct Cyc_CfFlowInfo_ConsumeInfo _tmp211;_LLE1: _tmp204=_tmp203.f1;if((
_tmp204.BottomFL).tag != 1)goto _LLE3;_tmp205=(int)(_tmp204.BottomFL).val;_LLE2:
return f2;_LLE3: _tmp206=_tmp203.f2;if((_tmp206.BottomFL).tag != 1)goto _LLE5;
_tmp207=(int)(_tmp206.BottomFL).val;_LLE4: return f1;_LLE5: _tmp208=_tmp203.f1;if((
_tmp208.ReachableFL).tag != 2)goto _LLE0;_tmp209=(struct _tuple12)(_tmp208.ReachableFL).val;
_tmp20A=_tmp209.f1;_tmp20B=_tmp209.f2;_tmp20C=_tmp209.f3;_tmp20D=_tmp203.f2;if((
_tmp20D.ReachableFL).tag != 2)goto _LLE0;_tmp20E=(struct _tuple12)(_tmp20D.ReachableFL).val;
_tmp20F=_tmp20E.f1;_tmp210=_tmp20E.f2;_tmp211=_tmp20E.f3;_LLE6: if(((_tmp20A.t == 
_tmp20F.t  && _tmp20B == _tmp210) && _tmp20C.may_consume == _tmp211.may_consume)
 && (_tmp20C.consumed).t == (_tmp211.consumed).t)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
f1,f2))return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle*_tmp212=fenv->r;struct Cyc_CfFlowInfo_EscPile*_tmp5E6;struct Cyc_CfFlowInfo_JoinEnv
_tmp5E5;struct Cyc_CfFlowInfo_JoinEnv _tmp213=(_tmp5E5.fenv=fenv,((_tmp5E5.pile=((
_tmp5E6=_region_malloc(_tmp212,sizeof(*_tmp5E6)),((_tmp5E6->rgn=_tmp212,((
_tmp5E6->places=0,_tmp5E6)))))),((_tmp5E5.d1=_tmp20A,((_tmp5E5.d2=_tmp20F,
_tmp5E5)))))));struct Cyc_Dict_Dict _tmp214=((struct Cyc_Dict_Dict(*)(void*(*f)(
struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(
struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,&
_tmp213,_tmp20A,_tmp20F);struct Cyc_List_List*_tmp215=Cyc_CfFlowInfo_join_relns(
fenv->r,_tmp20B,_tmp210);struct Cyc_CfFlowInfo_ConsumeInfo _tmp216=or_consumed?Cyc_CfFlowInfo_or_consume(
fenv->r,_tmp20C,_tmp211): Cyc_CfFlowInfo_and_consume(fenv->r,_tmp20C,_tmp211);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp213.pile,
all_changed,_tmp214),_tmp215,_tmp216);}_LLE0:;}struct _tuple26{union Cyc_CfFlowInfo_FlowInfo
f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};struct
_tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2,int
or_consumed);struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2,int
or_consumed){union Cyc_CfFlowInfo_FlowInfo _tmp21A;void*_tmp21B;struct _tuple13
_tmp219=pr1;_tmp21A=_tmp219.f1;_tmp21B=_tmp219.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp21D;void*_tmp21E;struct _tuple13 _tmp21C=pr2;_tmp21D=_tmp21C.f1;_tmp21E=
_tmp21C.f2;{union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,
all_changed,_tmp21A,_tmp21D,or_consumed);struct _tuple26 _tmp5E7;struct _tuple26
_tmp220=(_tmp5E7.f1=_tmp21A,((_tmp5E7.f2=_tmp21D,((_tmp5E7.f3=outflow,_tmp5E7)))));
union Cyc_CfFlowInfo_FlowInfo _tmp221;int _tmp222;union Cyc_CfFlowInfo_FlowInfo
_tmp223;int _tmp224;union Cyc_CfFlowInfo_FlowInfo _tmp225;struct _tuple12 _tmp226;
struct Cyc_Dict_Dict _tmp227;union Cyc_CfFlowInfo_FlowInfo _tmp228;struct _tuple12
_tmp229;struct Cyc_Dict_Dict _tmp22A;union Cyc_CfFlowInfo_FlowInfo _tmp22B;struct
_tuple12 _tmp22C;struct Cyc_Dict_Dict _tmp22D;struct Cyc_List_List*_tmp22E;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp22F;_LLE8: _tmp221=_tmp220.f1;if((_tmp221.BottomFL).tag != 1)goto _LLEA;_tmp222=(
int)(_tmp221.BottomFL).val;_LLE9: {struct _tuple13 _tmp5E8;return(_tmp5E8.f1=
outflow,((_tmp5E8.f2=_tmp21E,_tmp5E8)));}_LLEA: _tmp223=_tmp220.f2;if((_tmp223.BottomFL).tag
!= 1)goto _LLEC;_tmp224=(int)(_tmp223.BottomFL).val;_LLEB: {struct _tuple13 _tmp5E9;
return(_tmp5E9.f1=outflow,((_tmp5E9.f2=_tmp21B,_tmp5E9)));}_LLEC: _tmp225=_tmp220.f1;
if((_tmp225.ReachableFL).tag != 2)goto _LLEE;_tmp226=(struct _tuple12)(_tmp225.ReachableFL).val;
_tmp227=_tmp226.f1;_tmp228=_tmp220.f2;if((_tmp228.ReachableFL).tag != 2)goto _LLEE;
_tmp229=(struct _tuple12)(_tmp228.ReachableFL).val;_tmp22A=_tmp229.f1;_tmp22B=
_tmp220.f3;if((_tmp22B.ReachableFL).tag != 2)goto _LLEE;_tmp22C=(struct _tuple12)(
_tmp22B.ReachableFL).val;_tmp22D=_tmp22C.f1;_tmp22E=_tmp22C.f2;_tmp22F=_tmp22C.f3;
_LLED: if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(
0,_tmp21B,_tmp21E)){struct _tuple13 _tmp5EA;return(_tmp5EA.f1=outflow,((_tmp5EA.f2=
_tmp21E,_tmp5EA)));}if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(
0,_tmp21E,_tmp21B)){struct _tuple13 _tmp5EB;return(_tmp5EB.f1=outflow,((_tmp5EB.f2=
_tmp21B,_tmp5EB)));}{struct _RegionHandle*_tmp234=fenv->r;struct Cyc_CfFlowInfo_EscPile*
_tmp5EE;struct Cyc_CfFlowInfo_JoinEnv _tmp5ED;struct Cyc_CfFlowInfo_JoinEnv _tmp235=(
_tmp5ED.fenv=fenv,((_tmp5ED.pile=((_tmp5EE=_region_malloc(_tmp234,sizeof(*
_tmp5EE)),((_tmp5EE->rgn=_tmp234,((_tmp5EE->places=0,_tmp5EE)))))),((_tmp5ED.d1=
_tmp227,((_tmp5ED.d2=_tmp22A,_tmp5ED)))))));void*_tmp236=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp235,0,_tmp21B,
_tmp21E);struct _tuple13 _tmp5EF;return(_tmp5EF.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,_tmp235.pile,all_changed,_tmp22D),_tmp22E,_tmp22F),((_tmp5EF.f2=_tmp236,
_tmp5EF)));}_LLEE:;_LLEF: {struct Cyc_Core_Impossible_struct _tmp5F5;const char*
_tmp5F4;struct Cyc_Core_Impossible_struct*_tmp5F3;(int)_throw((void*)((_tmp5F3=
_cycalloc(sizeof(*_tmp5F3)),((_tmp5F3[0]=((_tmp5F5.tag=Cyc_Core_Impossible,((
_tmp5F5.f1=((_tmp5F4="join_flow_and_rval: BottomFL outflow",_tag_dyneither(
_tmp5F4,sizeof(char),37))),_tmp5F5)))),_tmp5F3)))));}_LLE7:;}}}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp5FC(struct Cyc_CfFlowInfo_AfterEnv**
env,unsigned int*_tmp5FB,unsigned int*_tmp5FA,void***_tmp5F8,struct
_dyneither_ptr*_tmp246,struct _dyneither_ptr*_tmp242){for(*_tmp5FB=0;*_tmp5FB < *
_tmp5FA;(*_tmp5FB)++){(*_tmp5F8)[*_tmp5FB]=Cyc_CfFlowInfo_after_absRval_d(*env,(
int)*_tmp5FB,((void**)(*_tmp242).curr)[(int)*_tmp5FB],*((void**)
_check_dyneither_subscript(*_tmp246,sizeof(void*),(int)*_tmp5FB)));}}static void*
Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);int
changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);if(
changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*
r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);if(changed1){if(
!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))return r1;env->changed=
Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,
env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{struct _tuple0 _tmp5F6;
struct _tuple0 _tmp23E=(_tmp5F6.f1=r1,((_tmp5F6.f2=r2,_tmp5F6)));void*_tmp23F;int
_tmp241;struct _dyneither_ptr _tmp242;void*_tmp243;int _tmp245;struct _dyneither_ptr
_tmp246;_LLF1: _tmp23F=_tmp23E.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp240=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp23F;if(_tmp240->tag != 7)goto _LLF3;
else{_tmp241=_tmp240->f1;_tmp242=_tmp240->f2;}}_tmp243=_tmp23E.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp244=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp243;if(_tmp244->tag != 7)goto
_LLF3;else{_tmp245=_tmp244->f1;_tmp246=_tmp244->f2;}}_LLF2: {unsigned int _tmp5FB;
unsigned int _tmp5FA;struct _dyneither_ptr _tmp5F9;void**_tmp5F8;unsigned int
_tmp5F7;struct _dyneither_ptr new_d=(_tmp5F7=_get_dyneither_size(_tmp242,sizeof(
void*)),((_tmp5F8=(void**)_region_malloc(((env->joinenv).fenv)->r,_check_times(
sizeof(void*),_tmp5F7)),((_tmp5F9=_tag_dyneither(_tmp5F8,sizeof(void*),_tmp5F7),((((
_tmp5FA=_tmp5F7,_tmp5FC(& env,& _tmp5FB,& _tmp5FA,& _tmp5F8,& _tmp246,& _tmp242))),
_tmp5F9)))))));int change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp242,sizeof(
void*));++ i){if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((
void**)_tmp242.curr)[i]){change=1;break;}}}if(!change)return r1;change=0;{int i=0;
for(0;i < _get_dyneither_size(_tmp242,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp246,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp5FF;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5FE;return(void*)((_tmp5FE=_region_malloc(((env->joinenv).fenv)->r,sizeof(*
_tmp5FE)),((_tmp5FE[0]=((_tmp5FF.tag=7,((_tmp5FF.f1=_tmp241,((_tmp5FF.f2=new_d,
_tmp5FF)))))),_tmp5FE))));}}_LLF3:;_LLF4: {struct Cyc_Core_Impossible_struct
_tmp605;const char*_tmp604;struct Cyc_Core_Impossible_struct*_tmp603;(int)_throw((
void*)((_tmp603=_cycalloc(sizeof(*_tmp603)),((_tmp603[0]=((_tmp605.tag=Cyc_Core_Impossible,((
_tmp605.f1=((_tmp604="after_absRval -- non-aggregates!",_tag_dyneither(_tmp604,
sizeof(char),33))),_tmp605)))),_tmp603)))));}_LLF0:;}}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2);static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp250=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged
_tmp251=env->changed;{struct Cyc_List_List*_tmp606;*env->last_field_cell=((
_tmp606=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp606)),((_tmp606->hd=(
void*)key,((_tmp606->tl=0,_tmp606))))));}env->last_field_cell=(struct Cyc_List_List**)&((
struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{void*_tmp253=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp250;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp251;return _tmp253;}}}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2);static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_CfFlowInfo_Place _tmp607;*env->curr_place=((_tmp607.root=root,((_tmp607.fields=
0,_tmp607))));}env->last_field_cell=(struct Cyc_List_List**)&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*
all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2,struct
Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){struct _tuple25 _tmp608;struct _tuple25
_tmp256=(_tmp608.f1=f1,((_tmp608.f2=f2,_tmp608)));union Cyc_CfFlowInfo_FlowInfo
_tmp257;int _tmp258;union Cyc_CfFlowInfo_FlowInfo _tmp259;int _tmp25A;union Cyc_CfFlowInfo_FlowInfo
_tmp25B;struct _tuple12 _tmp25C;struct Cyc_Dict_Dict _tmp25D;struct Cyc_List_List*
_tmp25E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp25F;union Cyc_CfFlowInfo_FlowInfo
_tmp260;struct _tuple12 _tmp261;struct Cyc_Dict_Dict _tmp262;struct Cyc_List_List*
_tmp263;struct Cyc_CfFlowInfo_ConsumeInfo _tmp264;_LLF6: _tmp257=_tmp256.f1;if((
_tmp257.BottomFL).tag != 1)goto _LLF8;_tmp258=(int)(_tmp257.BottomFL).val;_LLF7:
goto _LLF9;_LLF8: _tmp259=_tmp256.f2;if((_tmp259.BottomFL).tag != 1)goto _LLFA;
_tmp25A=(int)(_tmp259.BottomFL).val;_LLF9: return Cyc_CfFlowInfo_BottomFL();_LLFA:
_tmp25B=_tmp256.f1;if((_tmp25B.ReachableFL).tag != 2)goto _LLF5;_tmp25C=(struct
_tuple12)(_tmp25B.ReachableFL).val;_tmp25D=_tmp25C.f1;_tmp25E=_tmp25C.f2;_tmp25F=
_tmp25C.f3;_tmp260=_tmp256.f2;if((_tmp260.ReachableFL).tag != 2)goto _LLF5;_tmp261=(
struct _tuple12)(_tmp260.ReachableFL).val;_tmp262=_tmp261.f1;_tmp263=_tmp261.f2;
_tmp264=_tmp261.f3;_LLFB: if(((_tmp25D.t == _tmp262.t  && _tmp25E == _tmp263) && 
_tmp25F.may_consume == _tmp264.may_consume) && (_tmp25F.consumed).t == (_tmp264.consumed).t)
return f1;{struct _RegionHandle*_tmp265=fenv->r;struct Cyc_CfFlowInfo_Place*_tmp266=
fenv->dummy_place;struct Cyc_CfFlowInfo_JoinEnv _tmp60E;struct Cyc_CfFlowInfo_EscPile*
_tmp60D;struct Cyc_CfFlowInfo_AfterEnv _tmp60C;struct Cyc_CfFlowInfo_AfterEnv
_tmp267=(_tmp60C.joinenv=((_tmp60E.fenv=fenv,((_tmp60E.pile=((_tmp60D=
_region_malloc(_tmp265,sizeof(*_tmp60D)),((_tmp60D->rgn=_tmp265,((_tmp60D->places=
0,_tmp60D)))))),((_tmp60E.d1=_tmp25D,((_tmp60E.d2=_tmp262,_tmp60E)))))))),((
_tmp60C.chg1=chg1,((_tmp60C.chg2=chg2,((_tmp60C.curr_place=_tmp266,((_tmp60C.last_field_cell=(
struct Cyc_List_List**)& _tmp266->fields,((_tmp60C.changed=Cyc_CfFlowInfo_Neither,
_tmp60C)))))))))));struct Cyc_Dict_Dict _tmp268=((struct Cyc_Dict_Dict(*)(void*(*f)(
struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp267,_tmp25D,_tmp262);struct Cyc_List_List*_tmp269=Cyc_CfFlowInfo_join_relns(
fenv->r,_tmp25E,_tmp263);struct Cyc_CfFlowInfo_ConsumeInfo _tmp26A=Cyc_CfFlowInfo_and_consume(
fenv->r,_tmp25F,_tmp264);return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,(_tmp267.joinenv).pile,all_changed,_tmp268),_tmp269,_tmp26A);}_LLF5:;}
static int Cyc_CfFlowInfo_tag_cmps_lessthan_approx(struct Cyc_List_List*l1,struct
Cyc_List_List*l2);static int Cyc_CfFlowInfo_tag_cmps_lessthan_approx(struct Cyc_List_List*
l1,struct Cyc_List_List*l2){for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp26F;enum Cyc_Absyn_Primop _tmp270;void*_tmp271;struct Cyc_CfFlowInfo_TagCmp*
_tmp26E=(struct Cyc_CfFlowInfo_TagCmp*)l2->hd;_tmp26F=*_tmp26E;_tmp270=_tmp26F.cmp;
_tmp271=_tmp26F.bd;{struct Cyc_List_List*_tmp272=l1;for(0;_tmp272 != 0;_tmp272=
_tmp272->tl){struct Cyc_CfFlowInfo_TagCmp _tmp274;enum Cyc_Absyn_Primop _tmp275;
void*_tmp276;struct Cyc_CfFlowInfo_TagCmp*_tmp273=(struct Cyc_CfFlowInfo_TagCmp*)
_tmp272->hd;_tmp274=*_tmp273;_tmp275=_tmp274.cmp;_tmp276=_tmp274.bd;if(_tmp275 == 
_tmp270  && Cyc_Tcutil_typecmp(_tmp276,_tmp271)== 0)break;}if(_tmp272 == 0)return 0;}}
return 1;}static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,
void*r2);static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,
void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp60F;struct _tuple0 _tmp278=(_tmp60F.f1=
r1,((_tmp60F.f2=r2,_tmp60F)));void*_tmp279;struct Cyc_CfFlowInfo_Place*_tmp27B;
void*_tmp27C;struct Cyc_CfFlowInfo_Place*_tmp27E;void*_tmp27F;void*_tmp281;void*
_tmp283;int _tmp285;struct _dyneither_ptr _tmp286;void*_tmp287;int _tmp289;struct
_dyneither_ptr _tmp28A;void*_tmp28B;void*_tmp28D;void*_tmp28F;void*_tmp291;void*
_tmp293;void*_tmp295;void*_tmp297;void*_tmp299;void*_tmp29B;struct Cyc_List_List*
_tmp29D;void*_tmp29E;struct Cyc_List_List*_tmp2A0;void*_tmp2A1;_LLFD: _tmp279=
_tmp278.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp27A=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp279;if(_tmp27A->tag != 5)goto _LLFF;else{_tmp27B=_tmp27A->f1;}}_tmp27C=_tmp278.f2;{
struct Cyc_CfFlowInfo_AddressOf_struct*_tmp27D=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp27C;if(_tmp27D->tag != 5)goto _LLFF;else{_tmp27E=_tmp27D->f1;}}_LLFE: return Cyc_CfFlowInfo_place_cmp(
_tmp27B,_tmp27E)== 0;_LLFF: _tmp27F=_tmp278.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp280=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp27F;if(_tmp280->tag != 5)goto
_LL101;}_LL100: goto _LL102;_LL101: _tmp281=_tmp278.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp282=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp281;if(_tmp282->tag != 5)goto
_LL103;}_LL102: return 0;_LL103: _tmp283=_tmp278.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp284=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp283;if(_tmp284->tag != 7)goto
_LL105;else{_tmp285=_tmp284->f1;_tmp286=_tmp284->f2;}}_tmp287=_tmp278.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp288=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp287;if(_tmp288->tag != 7)goto _LL105;else{_tmp289=_tmp288->f1;_tmp28A=_tmp288->f2;}}
_LL104: if(_tmp286.curr == _tmp28A.curr)return 1;{int i=0;for(0;i < 
_get_dyneither_size(_tmp286,sizeof(void*));++ i){if(!((int(*)(int ignore,void*r1,
void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp286.curr)[i],*((
void**)_check_dyneither_subscript(_tmp28A,sizeof(void*),i))))return 0;}}return 1;
_LL105: _tmp28B=_tmp278.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp28C=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp28B;if(_tmp28C->tag != 1)goto _LL107;}
_tmp28D=_tmp278.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp28E=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp28D;if(_tmp28E->tag != 2)goto _LL107;}_LL106: return 1;_LL107: _tmp28F=_tmp278.f2;{
struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp290=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp28F;if(_tmp290->tag != 2)goto _LL109;}_LL108: return 0;_LL109: _tmp291=_tmp278.f2;{
struct Cyc_CfFlowInfo_Zero_struct*_tmp292=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp291;if(_tmp292->tag != 0)goto _LL10B;}_LL10A: goto _LL10C;_LL10B: _tmp293=_tmp278.f2;{
struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp294=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp293;if(_tmp294->tag != 1)goto _LL10D;}_LL10C: return 0;_LL10D: _tmp295=_tmp278.f1;{
struct Cyc_CfFlowInfo_Esc_struct*_tmp296=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp295;if(_tmp296->tag != 4)goto _LL10F;}_tmp297=_tmp278.f2;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp298=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp297;if(_tmp298->tag != 4)goto _LL10F;}
_LL10E: goto _LLFC;_LL10F: _tmp299=_tmp278.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp29A=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp299;if(_tmp29A->tag != 4)goto _LL111;}
_LL110: return 0;_LL111: _tmp29B=_tmp278.f1;{struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp29C=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp29B;if(_tmp29C->tag != 6)goto
_LL113;else{_tmp29D=_tmp29C->f1;}}_tmp29E=_tmp278.f2;{struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp29F=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp29E;if(_tmp29F->tag != 6)goto
_LL113;else{_tmp2A0=_tmp29F->f1;}}_LL112: return Cyc_CfFlowInfo_tag_cmps_lessthan_approx(
_tmp29D,_tmp2A0);_LL113: _tmp2A1=_tmp278.f2;{struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp2A2=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp2A1;if(_tmp2A2->tag != 6)goto
_LL115;}_LL114: return 0;_LL115:;_LL116: goto _LLFC;_LLFC:;}{struct _tuple23 _tmp610;
struct _tuple23 _tmp2A4=(_tmp610.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp610.f2=
Cyc_CfFlowInfo_initlevel_approx(r2),_tmp610)));enum Cyc_CfFlowInfo_InitLevel
_tmp2A5;enum Cyc_CfFlowInfo_InitLevel _tmp2A6;enum Cyc_CfFlowInfo_InitLevel
_tmp2A7;enum Cyc_CfFlowInfo_InitLevel _tmp2A8;enum Cyc_CfFlowInfo_InitLevel
_tmp2A9;enum Cyc_CfFlowInfo_InitLevel _tmp2AA;_LL118: _tmp2A5=_tmp2A4.f1;if(
_tmp2A5 != Cyc_CfFlowInfo_AllIL)goto _LL11A;_tmp2A6=_tmp2A4.f2;if(_tmp2A6 != Cyc_CfFlowInfo_AllIL)
goto _LL11A;_LL119: return 1;_LL11A: _tmp2A7=_tmp2A4.f2;if(_tmp2A7 != Cyc_CfFlowInfo_NoneIL)
goto _LL11C;_LL11B: return 1;_LL11C: _tmp2A8=_tmp2A4.f1;if(_tmp2A8 != Cyc_CfFlowInfo_NoneIL)
goto _LL11E;_LL11D: return 0;_LL11E: _tmp2A9=_tmp2A4.f2;if(_tmp2A9 != Cyc_CfFlowInfo_ThisIL)
goto _LL120;_LL11F: return 1;_LL120: _tmp2AA=_tmp2A4.f1;if(_tmp2AA != Cyc_CfFlowInfo_ThisIL)
goto _LL117;_LL121: return 0;_LL117:;}}static int Cyc_CfFlowInfo_relns_approx(struct
Cyc_List_List*r2s,struct Cyc_List_List*r1s);static int Cyc_CfFlowInfo_relns_approx(
struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s
!= 0;r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp2AB=(struct Cyc_CfFlowInfo_Reln*)
r1s->hd;int found=0;{struct Cyc_List_List*_tmp2AC=r2s;for(0;_tmp2AC != 0;_tmp2AC=
_tmp2AC->tl){struct Cyc_CfFlowInfo_Reln*_tmp2AD=(struct Cyc_CfFlowInfo_Reln*)
_tmp2AC->hd;if(_tmp2AB == _tmp2AD  || _tmp2AB->vd == _tmp2AD->vd  && Cyc_CfFlowInfo_same_relop(
_tmp2AB->rop,_tmp2AD->rop)){found=1;break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_consume_approx(
struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);int Cyc_CfFlowInfo_consume_approx(
struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2){if(c1.may_consume
!= c2.may_consume){struct Cyc_List_List*_tmp2AE=c1.may_consume;struct Cyc_List_List*
_tmp2AF=c2.may_consume;for(0;_tmp2AF != 0  && _tmp2AE != 0;_tmp2AF=_tmp2AF->tl){if((
unsigned int)_tmp2AE == (unsigned int)_tmp2AF)goto compare_consumed_set;{int
_tmp2B0=Cyc_CfFlowInfo_place_cmp((struct Cyc_CfFlowInfo_Place*)_tmp2AE->hd,(
struct Cyc_CfFlowInfo_Place*)_tmp2AF->hd);if(_tmp2B0 == 0)_tmp2AE=_tmp2AE->tl;}}
if(_tmp2AE != 0)return 0;}compare_consumed_set: return((int(*)(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2))Cyc_CfFlowInfo_place_set_subset)(c1.consumed,c2.consumed);}
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2);int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union
Cyc_CfFlowInfo_FlowInfo f2){struct _tuple25 _tmp611;struct _tuple25 _tmp2B2=(_tmp611.f1=
f1,((_tmp611.f2=f2,_tmp611)));union Cyc_CfFlowInfo_FlowInfo _tmp2B3;int _tmp2B4;
union Cyc_CfFlowInfo_FlowInfo _tmp2B5;int _tmp2B6;union Cyc_CfFlowInfo_FlowInfo
_tmp2B7;struct _tuple12 _tmp2B8;struct Cyc_Dict_Dict _tmp2B9;struct Cyc_List_List*
_tmp2BA;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2BB;union Cyc_CfFlowInfo_FlowInfo
_tmp2BC;struct _tuple12 _tmp2BD;struct Cyc_Dict_Dict _tmp2BE;struct Cyc_List_List*
_tmp2BF;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2C0;_LL123: _tmp2B3=_tmp2B2.f1;if((
_tmp2B3.BottomFL).tag != 1)goto _LL125;_tmp2B4=(int)(_tmp2B3.BottomFL).val;_LL124:
return 1;_LL125: _tmp2B5=_tmp2B2.f2;if((_tmp2B5.BottomFL).tag != 1)goto _LL127;
_tmp2B6=(int)(_tmp2B5.BottomFL).val;_LL126: return 0;_LL127: _tmp2B7=_tmp2B2.f1;if((
_tmp2B7.ReachableFL).tag != 2)goto _LL122;_tmp2B8=(struct _tuple12)(_tmp2B7.ReachableFL).val;
_tmp2B9=_tmp2B8.f1;_tmp2BA=_tmp2B8.f2;_tmp2BB=_tmp2B8.f3;_tmp2BC=_tmp2B2.f2;if((
_tmp2BC.ReachableFL).tag != 2)goto _LL122;_tmp2BD=(struct _tuple12)(_tmp2BC.ReachableFL).val;
_tmp2BE=_tmp2BD.f1;_tmp2BF=_tmp2BD.f2;_tmp2C0=_tmp2BD.f3;_LL128: if(((_tmp2B9.t == 
_tmp2BE.t  && _tmp2BA == _tmp2BF) && _tmp2BB.may_consume == _tmp2C0.may_consume)
 && (_tmp2BB.consumed).t == (_tmp2C0.consumed).t)return 1;return(((int(*)(int(*f)(
void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_forall_intersect)((
int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp2B9,_tmp2BE) && Cyc_CfFlowInfo_relns_approx(_tmp2BA,_tmp2BF)) && Cyc_CfFlowInfo_consume_approx(
_tmp2BB,_tmp2C0);_LL122:;}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(
struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v);struct
Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*
rs,struct Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=0;for(p=rs;!found
 && p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp2C1=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp2C1->vd == v){found=1;break;}{union Cyc_CfFlowInfo_RelnOp _tmp2C2=
_tmp2C1->rop;struct _tuple11 _tmp2C3;struct Cyc_Absyn_Vardecl*_tmp2C4;struct Cyc_Absyn_Vardecl*
_tmp2C5;struct Cyc_Absyn_Vardecl*_tmp2C6;_LL12A: if((_tmp2C2.LessVar).tag != 2)goto
_LL12C;_tmp2C3=(struct _tuple11)(_tmp2C2.LessVar).val;_tmp2C4=_tmp2C3.f1;_LL12B:
_tmp2C5=_tmp2C4;goto _LL12D;_LL12C: if((_tmp2C2.LessNumelts).tag != 3)goto _LL12E;
_tmp2C5=(struct Cyc_Absyn_Vardecl*)(_tmp2C2.LessNumelts).val;_LL12D: _tmp2C6=
_tmp2C5;goto _LL12F;_LL12E: if((_tmp2C2.LessEqNumelts).tag != 5)goto _LL130;_tmp2C6=(
struct Cyc_Absyn_Vardecl*)(_tmp2C2.LessEqNumelts).val;_LL12F: if(v == _tmp2C6)found=
1;goto _LL129;_LL130:;_LL131: goto _LL129;_LL129:;}}if(!found)return rs;{struct Cyc_List_List*
_tmp2C7=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp2C8=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp2C8->vd != v){{union Cyc_CfFlowInfo_RelnOp _tmp2C9=_tmp2C8->rop;struct
_tuple11 _tmp2CA;struct Cyc_Absyn_Vardecl*_tmp2CB;struct Cyc_Absyn_Vardecl*_tmp2CC;
struct Cyc_Absyn_Vardecl*_tmp2CD;_LL133: if((_tmp2C9.LessVar).tag != 2)goto _LL135;
_tmp2CA=(struct _tuple11)(_tmp2C9.LessVar).val;_tmp2CB=_tmp2CA.f1;_LL134: _tmp2CC=
_tmp2CB;goto _LL136;_LL135: if((_tmp2C9.LessNumelts).tag != 3)goto _LL137;_tmp2CC=(
struct Cyc_Absyn_Vardecl*)(_tmp2C9.LessNumelts).val;_LL136: _tmp2CD=_tmp2CC;goto
_LL138;_LL137: if((_tmp2C9.LessEqNumelts).tag != 5)goto _LL139;_tmp2CD=(struct Cyc_Absyn_Vardecl*)(
_tmp2C9.LessEqNumelts).val;_LL138: if(v == _tmp2CD)continue;goto _LL132;_LL139:;
_LL13A: goto _LL132;_LL132:;}{struct Cyc_List_List*_tmp612;_tmp2C7=((_tmp612=
_region_malloc(rgn,sizeof(*_tmp612)),((_tmp612->hd=_tmp2C8,((_tmp612->tl=_tmp2C7,
_tmp612))))));}}}return _tmp2C7;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e){{void*_tmp2CF=e->r;void*_tmp2D1;struct Cyc_Absyn_Vardecl*
_tmp2D3;void*_tmp2D5;struct Cyc_Absyn_Vardecl*_tmp2D7;void*_tmp2D9;struct Cyc_Absyn_Vardecl*
_tmp2DB;void*_tmp2DD;struct Cyc_Absyn_Vardecl*_tmp2DF;_LL13C: {struct Cyc_Absyn_Var_e_struct*
_tmp2D0=(struct Cyc_Absyn_Var_e_struct*)_tmp2CF;if(_tmp2D0->tag != 1)goto _LL13E;
else{_tmp2D1=(void*)_tmp2D0->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2D2=(struct
Cyc_Absyn_Global_b_struct*)_tmp2D1;if(_tmp2D2->tag != 1)goto _LL13E;else{_tmp2D3=
_tmp2D2->f1;}}}}_LL13D: _tmp2D7=_tmp2D3;goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_struct*
_tmp2D4=(struct Cyc_Absyn_Var_e_struct*)_tmp2CF;if(_tmp2D4->tag != 1)goto _LL140;
else{_tmp2D5=(void*)_tmp2D4->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2D6=(struct
Cyc_Absyn_Param_b_struct*)_tmp2D5;if(_tmp2D6->tag != 3)goto _LL140;else{_tmp2D7=
_tmp2D6->f1;}}}}_LL13F: _tmp2DB=_tmp2D7;goto _LL141;_LL140: {struct Cyc_Absyn_Var_e_struct*
_tmp2D8=(struct Cyc_Absyn_Var_e_struct*)_tmp2CF;if(_tmp2D8->tag != 1)goto _LL142;
else{_tmp2D9=(void*)_tmp2D8->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2DA=(struct
Cyc_Absyn_Local_b_struct*)_tmp2D9;if(_tmp2DA->tag != 4)goto _LL142;else{_tmp2DB=
_tmp2DA->f1;}}}}_LL141: _tmp2DF=_tmp2DB;goto _LL143;_LL142: {struct Cyc_Absyn_Var_e_struct*
_tmp2DC=(struct Cyc_Absyn_Var_e_struct*)_tmp2CF;if(_tmp2DC->tag != 1)goto _LL144;
else{_tmp2DD=(void*)_tmp2DC->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2DE=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2DD;if(_tmp2DE->tag != 5)goto _LL144;else{_tmp2DF=_tmp2DE->f1;}}}}_LL143: if(!
_tmp2DF->escapes)return Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp2DF);goto _LL13B;
_LL144:;_LL145: goto _LL13B;_LL13B:;}return r;}struct Cyc_CfFlowInfo_TagCmp*Cyc_CfFlowInfo_copy_tagcmp(
struct _RegionHandle*r2,struct Cyc_CfFlowInfo_TagCmp*t);struct Cyc_CfFlowInfo_TagCmp*
Cyc_CfFlowInfo_copy_tagcmp(struct _RegionHandle*r2,struct Cyc_CfFlowInfo_TagCmp*t){
struct Cyc_CfFlowInfo_TagCmp*_tmp613;return(_tmp613=_region_malloc(r2,sizeof(*
_tmp613)),((_tmp613[0]=*t,_tmp613)));}struct Cyc_List_List*Cyc_CfFlowInfo_copy_tagcmps(
struct _RegionHandle*r2,struct Cyc_List_List*ts);struct Cyc_List_List*Cyc_CfFlowInfo_copy_tagcmps(
struct _RegionHandle*r2,struct Cyc_List_List*ts){return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_CfFlowInfo_TagCmp*(*f)(struct _RegionHandle*,
struct Cyc_CfFlowInfo_TagCmp*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r2,Cyc_CfFlowInfo_copy_tagcmp,r2,ts);}struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(
struct _RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r);struct Cyc_CfFlowInfo_Reln*
Cyc_CfFlowInfo_copy_reln(struct _RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r){
struct Cyc_CfFlowInfo_Reln*_tmp614;return(_tmp614=_region_malloc(r2,sizeof(*
_tmp614)),((_tmp614[0]=*r,_tmp614)));}struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(
struct _RegionHandle*r2,struct Cyc_List_List*relns);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(
struct _RegionHandle*r2,struct Cyc_List_List*relns){return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*f)(struct _RegionHandle*,struct
Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*
r1,struct Cyc_List_List*r2);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*r1,
struct Cyc_List_List*r2){for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){struct Cyc_CfFlowInfo_Reln*
_tmp2E2=(struct Cyc_CfFlowInfo_Reln*)r1->hd;struct Cyc_CfFlowInfo_Reln*_tmp2E3=(
struct Cyc_CfFlowInfo_Reln*)r2->hd;if(_tmp2E2->vd != _tmp2E3->vd  || !Cyc_CfFlowInfo_same_relop(
_tmp2E2->rop,_tmp2E3->rop))return 0;}if(r1 != 0  || r2 != 0)return 0;else{return 1;}}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,
struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)return r;r=Cyc_CfFlowInfo_reln_kill_var(
rgn,r,v);{void*_tmp2E4=e->r;struct Cyc_Absyn_MallocInfo _tmp2E6;struct Cyc_Absyn_Exp*
_tmp2E7;int _tmp2E8;_LL147: {struct Cyc_Absyn_Malloc_e_struct*_tmp2E5=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp2E4;if(_tmp2E5->tag != 35)goto _LL149;else{_tmp2E6=_tmp2E5->f1;_tmp2E7=_tmp2E6.num_elts;
_tmp2E8=_tmp2E6.fat_result;if(_tmp2E8 != 1)goto _LL149;}}_LL148: malloc_loop: {void*
_tmp2E9=_tmp2E7->r;struct Cyc_Absyn_Exp*_tmp2EB;void*_tmp2ED;struct Cyc_Absyn_Vardecl*
_tmp2EF;void*_tmp2F1;struct Cyc_Absyn_Vardecl*_tmp2F3;void*_tmp2F5;struct Cyc_Absyn_Vardecl*
_tmp2F7;void*_tmp2F9;struct Cyc_Absyn_Vardecl*_tmp2FB;_LL14C: {struct Cyc_Absyn_Cast_e_struct*
_tmp2EA=(struct Cyc_Absyn_Cast_e_struct*)_tmp2E9;if(_tmp2EA->tag != 15)goto _LL14E;
else{_tmp2EB=_tmp2EA->f2;}}_LL14D: _tmp2E7=_tmp2EB;goto malloc_loop;_LL14E: {
struct Cyc_Absyn_Var_e_struct*_tmp2EC=(struct Cyc_Absyn_Var_e_struct*)_tmp2E9;if(
_tmp2EC->tag != 1)goto _LL150;else{_tmp2ED=(void*)_tmp2EC->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp2EE=(struct Cyc_Absyn_Pat_b_struct*)_tmp2ED;if(_tmp2EE->tag != 5)goto _LL150;
else{_tmp2EF=_tmp2EE->f1;}}}}_LL14F: _tmp2F3=_tmp2EF;goto _LL151;_LL150: {struct
Cyc_Absyn_Var_e_struct*_tmp2F0=(struct Cyc_Absyn_Var_e_struct*)_tmp2E9;if(_tmp2F0->tag
!= 1)goto _LL152;else{_tmp2F1=(void*)_tmp2F0->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp2F2=(struct Cyc_Absyn_Local_b_struct*)_tmp2F1;if(_tmp2F2->tag != 4)goto _LL152;
else{_tmp2F3=_tmp2F2->f1;}}}}_LL151: _tmp2F7=_tmp2F3;goto _LL153;_LL152: {struct
Cyc_Absyn_Var_e_struct*_tmp2F4=(struct Cyc_Absyn_Var_e_struct*)_tmp2E9;if(_tmp2F4->tag
!= 1)goto _LL154;else{_tmp2F5=(void*)_tmp2F4->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp2F6=(struct Cyc_Absyn_Param_b_struct*)_tmp2F5;if(_tmp2F6->tag != 3)goto _LL154;
else{_tmp2F7=_tmp2F6->f1;}}}}_LL153: _tmp2FB=_tmp2F7;goto _LL155;_LL154: {struct
Cyc_Absyn_Var_e_struct*_tmp2F8=(struct Cyc_Absyn_Var_e_struct*)_tmp2E9;if(_tmp2F8->tag
!= 1)goto _LL156;else{_tmp2F9=(void*)_tmp2F8->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp2FA=(struct Cyc_Absyn_Global_b_struct*)_tmp2F9;if(_tmp2FA->tag != 1)goto _LL156;
else{_tmp2FB=_tmp2FA->f1;}}}}_LL155: if(_tmp2FB->escapes)return r;{struct Cyc_CfFlowInfo_Reln*
_tmp617;struct Cyc_List_List*_tmp616;return(_tmp616=_region_malloc(rgn,sizeof(*
_tmp616)),((_tmp616->hd=((_tmp617=_region_malloc(rgn,sizeof(*_tmp617)),((_tmp617->vd=
_tmp2FB,((_tmp617->rop=Cyc_CfFlowInfo_LessEqNumelts(v),_tmp617)))))),((_tmp616->tl=
r,_tmp616)))));}_LL156:;_LL157: return r;_LL14B:;}_LL149:;_LL14A: goto _LL146;_LL146:;}{
void*_tmp2FE=Cyc_Tcutil_compress(v->type);_LL159: {struct Cyc_Absyn_IntType_struct*
_tmp2FF=(struct Cyc_Absyn_IntType_struct*)_tmp2FE;if(_tmp2FF->tag != 6)goto _LL15B;}
_LL15A: goto _LL158;_LL15B:;_LL15C: return r;_LL158:;}loop: {void*_tmp300=e->r;
struct Cyc_Absyn_Exp*_tmp302;union Cyc_Absyn_Cnst _tmp304;struct _tuple6 _tmp305;int
_tmp306;enum Cyc_Absyn_Primop _tmp308;struct Cyc_List_List*_tmp309;struct Cyc_List_List
_tmp30A;struct Cyc_List_List*_tmp30B;struct Cyc_List_List _tmp30C;struct Cyc_Absyn_Exp*
_tmp30D;enum Cyc_Absyn_Primop _tmp30F;struct Cyc_List_List*_tmp310;struct Cyc_List_List
_tmp311;struct Cyc_Absyn_Exp*_tmp312;_LL15E: {struct Cyc_Absyn_Cast_e_struct*
_tmp301=(struct Cyc_Absyn_Cast_e_struct*)_tmp300;if(_tmp301->tag != 15)goto _LL160;
else{_tmp302=_tmp301->f2;}}_LL15F: e=_tmp302;goto loop;_LL160: {struct Cyc_Absyn_Const_e_struct*
_tmp303=(struct Cyc_Absyn_Const_e_struct*)_tmp300;if(_tmp303->tag != 0)goto _LL162;
else{_tmp304=_tmp303->f1;if((_tmp304.Int_c).tag != 4)goto _LL162;_tmp305=(struct
_tuple6)(_tmp304.Int_c).val;_tmp306=_tmp305.f2;}}_LL161: {struct Cyc_CfFlowInfo_Reln*
_tmp61A;struct Cyc_List_List*_tmp619;return(_tmp619=_region_malloc(rgn,sizeof(*
_tmp619)),((_tmp619->hd=((_tmp61A=_region_malloc(rgn,sizeof(*_tmp61A)),((_tmp61A->vd=
v,((_tmp61A->rop=Cyc_CfFlowInfo_EqualConst((unsigned int)_tmp306),_tmp61A)))))),((
_tmp619->tl=r,_tmp619)))));}_LL162: {struct Cyc_Absyn_Primop_e_struct*_tmp307=(
struct Cyc_Absyn_Primop_e_struct*)_tmp300;if(_tmp307->tag != 3)goto _LL164;else{
_tmp308=_tmp307->f1;if(_tmp308 != Cyc_Absyn_Mod)goto _LL164;_tmp309=_tmp307->f2;
if(_tmp309 == 0)goto _LL164;_tmp30A=*_tmp309;_tmp30B=_tmp30A.tl;if(_tmp30B == 0)
goto _LL164;_tmp30C=*_tmp30B;_tmp30D=(struct Cyc_Absyn_Exp*)_tmp30C.hd;}}_LL163:{
void*_tmp315=_tmp30D->r;enum Cyc_Absyn_Primop _tmp317;struct Cyc_List_List*_tmp318;
struct Cyc_List_List _tmp319;struct Cyc_Absyn_Exp*_tmp31A;_LL169: {struct Cyc_Absyn_Primop_e_struct*
_tmp316=(struct Cyc_Absyn_Primop_e_struct*)_tmp315;if(_tmp316->tag != 3)goto _LL16B;
else{_tmp317=_tmp316->f1;_tmp318=_tmp316->f2;if(_tmp318 == 0)goto _LL16B;_tmp319=*
_tmp318;_tmp31A=(struct Cyc_Absyn_Exp*)_tmp319.hd;}}_LL16A:{void*_tmp31B=_tmp31A->r;
void*_tmp31D;struct Cyc_Absyn_Vardecl*_tmp31F;void*_tmp321;struct Cyc_Absyn_Vardecl*
_tmp323;void*_tmp325;struct Cyc_Absyn_Vardecl*_tmp327;void*_tmp329;struct Cyc_Absyn_Vardecl*
_tmp32B;_LL16E: {struct Cyc_Absyn_Var_e_struct*_tmp31C=(struct Cyc_Absyn_Var_e_struct*)
_tmp31B;if(_tmp31C->tag != 1)goto _LL170;else{_tmp31D=(void*)_tmp31C->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp31E=(struct Cyc_Absyn_Global_b_struct*)_tmp31D;if(
_tmp31E->tag != 1)goto _LL170;else{_tmp31F=_tmp31E->f1;}}}}_LL16F: _tmp323=_tmp31F;
goto _LL171;_LL170: {struct Cyc_Absyn_Var_e_struct*_tmp320=(struct Cyc_Absyn_Var_e_struct*)
_tmp31B;if(_tmp320->tag != 1)goto _LL172;else{_tmp321=(void*)_tmp320->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp322=(struct Cyc_Absyn_Local_b_struct*)_tmp321;if(
_tmp322->tag != 4)goto _LL172;else{_tmp323=_tmp322->f1;}}}}_LL171: _tmp327=_tmp323;
goto _LL173;_LL172: {struct Cyc_Absyn_Var_e_struct*_tmp324=(struct Cyc_Absyn_Var_e_struct*)
_tmp31B;if(_tmp324->tag != 1)goto _LL174;else{_tmp325=(void*)_tmp324->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp326=(struct Cyc_Absyn_Param_b_struct*)_tmp325;if(
_tmp326->tag != 3)goto _LL174;else{_tmp327=_tmp326->f1;}}}}_LL173: _tmp32B=_tmp327;
goto _LL175;_LL174: {struct Cyc_Absyn_Var_e_struct*_tmp328=(struct Cyc_Absyn_Var_e_struct*)
_tmp31B;if(_tmp328->tag != 1)goto _LL176;else{_tmp329=(void*)_tmp328->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp32A=(struct Cyc_Absyn_Pat_b_struct*)_tmp329;if(_tmp32A->tag
!= 5)goto _LL176;else{_tmp32B=_tmp32A->f1;}}}}_LL175: if(_tmp32B->escapes)return r;{
struct Cyc_CfFlowInfo_Reln*_tmp61D;struct Cyc_List_List*_tmp61C;return(_tmp61C=
_region_malloc(rgn,sizeof(*_tmp61C)),((_tmp61C->hd=((_tmp61D=_region_malloc(rgn,
sizeof(*_tmp61D)),((_tmp61D->vd=v,((_tmp61D->rop=Cyc_CfFlowInfo_LessNumelts(
_tmp32B),_tmp61D)))))),((_tmp61C->tl=r,_tmp61C)))));}_LL176:;_LL177: goto _LL16D;
_LL16D:;}goto _LL168;_LL16B:;_LL16C: goto _LL168;_LL168:;}goto _LL15D;_LL164: {
struct Cyc_Absyn_Primop_e_struct*_tmp30E=(struct Cyc_Absyn_Primop_e_struct*)
_tmp300;if(_tmp30E->tag != 3)goto _LL166;else{_tmp30F=_tmp30E->f1;_tmp310=_tmp30E->f2;
if(_tmp310 == 0)goto _LL166;_tmp311=*_tmp310;_tmp312=(struct Cyc_Absyn_Exp*)_tmp311.hd;}}
_LL165:{void*_tmp32E=_tmp312->r;void*_tmp330;struct Cyc_Absyn_Vardecl*_tmp332;
void*_tmp334;struct Cyc_Absyn_Vardecl*_tmp336;void*_tmp338;struct Cyc_Absyn_Vardecl*
_tmp33A;void*_tmp33C;struct Cyc_Absyn_Vardecl*_tmp33E;_LL179: {struct Cyc_Absyn_Var_e_struct*
_tmp32F=(struct Cyc_Absyn_Var_e_struct*)_tmp32E;if(_tmp32F->tag != 1)goto _LL17B;
else{_tmp330=(void*)_tmp32F->f2;{struct Cyc_Absyn_Global_b_struct*_tmp331=(struct
Cyc_Absyn_Global_b_struct*)_tmp330;if(_tmp331->tag != 1)goto _LL17B;else{_tmp332=
_tmp331->f1;}}}}_LL17A: _tmp336=_tmp332;goto _LL17C;_LL17B: {struct Cyc_Absyn_Var_e_struct*
_tmp333=(struct Cyc_Absyn_Var_e_struct*)_tmp32E;if(_tmp333->tag != 1)goto _LL17D;
else{_tmp334=(void*)_tmp333->f2;{struct Cyc_Absyn_Local_b_struct*_tmp335=(struct
Cyc_Absyn_Local_b_struct*)_tmp334;if(_tmp335->tag != 4)goto _LL17D;else{_tmp336=
_tmp335->f1;}}}}_LL17C: _tmp33A=_tmp336;goto _LL17E;_LL17D: {struct Cyc_Absyn_Var_e_struct*
_tmp337=(struct Cyc_Absyn_Var_e_struct*)_tmp32E;if(_tmp337->tag != 1)goto _LL17F;
else{_tmp338=(void*)_tmp337->f2;{struct Cyc_Absyn_Param_b_struct*_tmp339=(struct
Cyc_Absyn_Param_b_struct*)_tmp338;if(_tmp339->tag != 3)goto _LL17F;else{_tmp33A=
_tmp339->f1;}}}}_LL17E: _tmp33E=_tmp33A;goto _LL180;_LL17F: {struct Cyc_Absyn_Var_e_struct*
_tmp33B=(struct Cyc_Absyn_Var_e_struct*)_tmp32E;if(_tmp33B->tag != 1)goto _LL181;
else{_tmp33C=(void*)_tmp33B->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp33D=(struct Cyc_Absyn_Pat_b_struct*)
_tmp33C;if(_tmp33D->tag != 5)goto _LL181;else{_tmp33E=_tmp33D->f1;}}}}_LL180: if(
_tmp33E->escapes)return r;{struct Cyc_CfFlowInfo_Reln*_tmp620;struct Cyc_List_List*
_tmp61F;return(_tmp61F=_region_malloc(rgn,sizeof(*_tmp61F)),((_tmp61F->hd=((
_tmp620=_region_malloc(rgn,sizeof(*_tmp620)),((_tmp620->vd=v,((_tmp620->rop=Cyc_CfFlowInfo_LessEqNumelts(
_tmp33E),_tmp620)))))),((_tmp61F->tl=r,_tmp61F)))));}_LL181:;_LL182: goto _LL178;
_LL178:;}goto _LL15D;_LL166:;_LL167: goto _LL15D;_LL15D:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){{void*_tmp341=e1->r;void*_tmp343;struct Cyc_Absyn_Vardecl*_tmp345;void*
_tmp347;struct Cyc_Absyn_Vardecl*_tmp349;void*_tmp34B;struct Cyc_Absyn_Vardecl*
_tmp34D;void*_tmp34F;struct Cyc_Absyn_Vardecl*_tmp351;_LL184: {struct Cyc_Absyn_Var_e_struct*
_tmp342=(struct Cyc_Absyn_Var_e_struct*)_tmp341;if(_tmp342->tag != 1)goto _LL186;
else{_tmp343=(void*)_tmp342->f2;{struct Cyc_Absyn_Global_b_struct*_tmp344=(struct
Cyc_Absyn_Global_b_struct*)_tmp343;if(_tmp344->tag != 1)goto _LL186;else{_tmp345=
_tmp344->f1;}}}}_LL185: _tmp349=_tmp345;goto _LL187;_LL186: {struct Cyc_Absyn_Var_e_struct*
_tmp346=(struct Cyc_Absyn_Var_e_struct*)_tmp341;if(_tmp346->tag != 1)goto _LL188;
else{_tmp347=(void*)_tmp346->f2;{struct Cyc_Absyn_Param_b_struct*_tmp348=(struct
Cyc_Absyn_Param_b_struct*)_tmp347;if(_tmp348->tag != 3)goto _LL188;else{_tmp349=
_tmp348->f1;}}}}_LL187: _tmp34D=_tmp349;goto _LL189;_LL188: {struct Cyc_Absyn_Var_e_struct*
_tmp34A=(struct Cyc_Absyn_Var_e_struct*)_tmp341;if(_tmp34A->tag != 1)goto _LL18A;
else{_tmp34B=(void*)_tmp34A->f2;{struct Cyc_Absyn_Local_b_struct*_tmp34C=(struct
Cyc_Absyn_Local_b_struct*)_tmp34B;if(_tmp34C->tag != 4)goto _LL18A;else{_tmp34D=
_tmp34C->f1;}}}}_LL189: _tmp351=_tmp34D;goto _LL18B;_LL18A: {struct Cyc_Absyn_Var_e_struct*
_tmp34E=(struct Cyc_Absyn_Var_e_struct*)_tmp341;if(_tmp34E->tag != 1)goto _LL18C;
else{_tmp34F=(void*)_tmp34E->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp350=(struct Cyc_Absyn_Pat_b_struct*)
_tmp34F;if(_tmp350->tag != 5)goto _LL18C;else{_tmp351=_tmp350->f1;}}}}_LL18B: if(!
_tmp351->escapes)return Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp351,e2);goto
_LL183;_LL18C:;_LL18D: goto _LL183;_LL183:;}return r;}static void Cyc_CfFlowInfo_print_reln(
struct Cyc_CfFlowInfo_Reln*r);static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*
r){{const char*_tmp624;void*_tmp623[1];struct Cyc_String_pa_struct _tmp622;(_tmp622.tag=
0,((_tmp622.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((
r->vd)->name)),((_tmp623[0]=& _tmp622,Cyc_fprintf(Cyc_stderr,((_tmp624="%s",
_tag_dyneither(_tmp624,sizeof(char),3))),_tag_dyneither(_tmp623,sizeof(void*),1)))))));}{
union Cyc_CfFlowInfo_RelnOp _tmp355=r->rop;unsigned int _tmp356;struct _tuple11
_tmp357;struct Cyc_Absyn_Vardecl*_tmp358;struct Cyc_Absyn_Vardecl*_tmp359;
unsigned int _tmp35A;struct Cyc_Absyn_Vardecl*_tmp35B;_LL18F: if((_tmp355.EqualConst).tag
!= 1)goto _LL191;_tmp356=(unsigned int)(_tmp355.EqualConst).val;_LL190:{const char*
_tmp628;void*_tmp627[1];struct Cyc_Int_pa_struct _tmp626;(_tmp626.tag=1,((_tmp626.f1=(
unsigned long)((int)_tmp356),((_tmp627[0]=& _tmp626,Cyc_fprintf(Cyc_stderr,((
_tmp628="==%d",_tag_dyneither(_tmp628,sizeof(char),5))),_tag_dyneither(_tmp627,
sizeof(void*),1)))))));}goto _LL18E;_LL191: if((_tmp355.LessVar).tag != 2)goto
_LL193;_tmp357=(struct _tuple11)(_tmp355.LessVar).val;_tmp358=_tmp357.f1;_LL192:{
const char*_tmp62C;void*_tmp62B[1];struct Cyc_String_pa_struct _tmp62A;(_tmp62A.tag=
0,((_tmp62A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp358->name)),((_tmp62B[0]=& _tmp62A,Cyc_fprintf(Cyc_stderr,((_tmp62C="<%s",
_tag_dyneither(_tmp62C,sizeof(char),4))),_tag_dyneither(_tmp62B,sizeof(void*),1)))))));}
goto _LL18E;_LL193: if((_tmp355.LessNumelts).tag != 3)goto _LL195;_tmp359=(struct Cyc_Absyn_Vardecl*)(
_tmp355.LessNumelts).val;_LL194:{const char*_tmp630;void*_tmp62F[1];struct Cyc_String_pa_struct
_tmp62E;(_tmp62E.tag=0,((_tmp62E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp359->name)),((_tmp62F[0]=& _tmp62E,Cyc_fprintf(Cyc_stderr,((
_tmp630="<numelts(%s)",_tag_dyneither(_tmp630,sizeof(char),13))),_tag_dyneither(
_tmp62F,sizeof(void*),1)))))));}goto _LL18E;_LL195: if((_tmp355.LessConst).tag != 4)
goto _LL197;_tmp35A=(unsigned int)(_tmp355.LessConst).val;_LL196:{const char*
_tmp634;void*_tmp633[1];struct Cyc_Int_pa_struct _tmp632;(_tmp632.tag=1,((_tmp632.f1=(
unsigned long)((int)_tmp35A),((_tmp633[0]=& _tmp632,Cyc_fprintf(Cyc_stderr,((
_tmp634="<%d",_tag_dyneither(_tmp634,sizeof(char),4))),_tag_dyneither(_tmp633,
sizeof(void*),1)))))));}goto _LL18E;_LL197: if((_tmp355.LessEqNumelts).tag != 5)
goto _LL18E;_tmp35B=(struct Cyc_Absyn_Vardecl*)(_tmp355.LessEqNumelts).val;_LL198:{
const char*_tmp638;void*_tmp637[1];struct Cyc_String_pa_struct _tmp636;(_tmp636.tag=
0,((_tmp636.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp35B->name)),((_tmp637[0]=& _tmp636,Cyc_fprintf(Cyc_stderr,((_tmp638="<=numelts(%s)",
_tag_dyneither(_tmp638,sizeof(char),14))),_tag_dyneither(_tmp637,sizeof(void*),1)))))));}
goto _LL18E;_LL18E:;}}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r);void
Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){for(0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((
struct Cyc_CfFlowInfo_Reln*)r->hd);if(r->tl != 0){const char*_tmp63B;void*_tmp63A;(
_tmp63A=0,Cyc_fprintf(Cyc_stderr,((_tmp63B=",",_tag_dyneither(_tmp63B,sizeof(
char),2))),_tag_dyneither(_tmp63A,sizeof(void*),0)));}}}void Cyc_CfFlowInfo_print_initlevel(
enum Cyc_CfFlowInfo_InitLevel il);void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel
il){switch(il){case Cyc_CfFlowInfo_NoneIL: _LL199:{const char*_tmp63E;void*_tmp63D;(
_tmp63D=0,Cyc_fprintf(Cyc_stderr,((_tmp63E="uninitialized",_tag_dyneither(
_tmp63E,sizeof(char),14))),_tag_dyneither(_tmp63D,sizeof(void*),0)));}break;case
Cyc_CfFlowInfo_ThisIL: _LL19A:{const char*_tmp641;void*_tmp640;(_tmp640=0,Cyc_fprintf(
Cyc_stderr,((_tmp641="this-initialized",_tag_dyneither(_tmp641,sizeof(char),17))),
_tag_dyneither(_tmp640,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_AllIL: _LL19B:{
const char*_tmp644;void*_tmp643;(_tmp643=0,Cyc_fprintf(Cyc_stderr,((_tmp644="all-initialized",
_tag_dyneither(_tmp644,sizeof(char),16))),_tag_dyneither(_tmp643,sizeof(void*),0)));}
break;}}void Cyc_CfFlowInfo_print_root(void*root);void Cyc_CfFlowInfo_print_root(
void*root){void*_tmp373=root;struct Cyc_Absyn_Vardecl*_tmp375;struct Cyc_Absyn_Exp*
_tmp377;void*_tmp378;_LL19E: {struct Cyc_CfFlowInfo_VarRoot_struct*_tmp374=(
struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp373;if(_tmp374->tag != 0)goto _LL1A0;
else{_tmp375=_tmp374->f1;}}_LL19F:{const char*_tmp648;void*_tmp647[1];struct Cyc_String_pa_struct
_tmp646;(_tmp646.tag=0,((_tmp646.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp375->name)),((_tmp647[0]=& _tmp646,Cyc_fprintf(Cyc_stderr,((
_tmp648="Root(%s)",_tag_dyneither(_tmp648,sizeof(char),9))),_tag_dyneither(
_tmp647,sizeof(void*),1)))))));}goto _LL19D;_LL1A0: {struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp376=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp373;if(_tmp376->tag != 1)goto
_LL1A2;else{_tmp377=_tmp376->f1;_tmp378=(void*)_tmp376->f2;}}_LL1A1:{const char*
_tmp64D;void*_tmp64C[2];struct Cyc_String_pa_struct _tmp64B;struct Cyc_String_pa_struct
_tmp64A;(_tmp64A.tag=0,((_tmp64A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp378)),((_tmp64B.tag=0,((_tmp64B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp377)),((_tmp64C[
0]=& _tmp64B,((_tmp64C[1]=& _tmp64A,Cyc_fprintf(Cyc_stderr,((_tmp64D="MallocPt(%s,%s)",
_tag_dyneither(_tmp64D,sizeof(char),16))),_tag_dyneither(_tmp64C,sizeof(void*),2)))))))))))));}
goto _LL19D;_LL1A2: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp379=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp373;if(_tmp379->tag != 2)goto _LL19D;}_LL1A3:{const char*_tmp650;void*_tmp64F;(
_tmp64F=0,Cyc_fprintf(Cyc_stderr,((_tmp650="InitParam(_,_)",_tag_dyneither(
_tmp650,sizeof(char),15))),_tag_dyneither(_tmp64F,sizeof(void*),0)));}goto _LL19D;
_LL19D:;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_place(
struct Cyc_CfFlowInfo_Place*p){Cyc_CfFlowInfo_print_root(p->root);{struct Cyc_List_List*
_tmp383=p->fields;while(_tmp383 != 0){{const char*_tmp654;void*_tmp653[1];struct
Cyc_Int_pa_struct _tmp652;(_tmp652.tag=1,((_tmp652.f1=(unsigned long)((int)
_tmp383->hd),((_tmp653[0]=& _tmp652,Cyc_fprintf(Cyc_stderr,((_tmp654=".%d",
_tag_dyneither(_tmp654,sizeof(char),4))),_tag_dyneither(_tmp653,sizeof(void*),1)))))));}
_tmp383=_tmp383->tl;}{const char*_tmp657;void*_tmp656;(_tmp656=0,Cyc_fprintf(Cyc_stderr,((
_tmp657=" ",_tag_dyneither(_tmp657,sizeof(char),2))),_tag_dyneither(_tmp656,
sizeof(void*),0)));}}}void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*
pr)(void*));void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp389=1;while(x != 0){if(!_tmp389){{const char*_tmp65A;void*_tmp659;(_tmp659=
0,Cyc_fprintf(Cyc_stderr,((_tmp65A=", ",_tag_dyneither(_tmp65A,sizeof(char),3))),
_tag_dyneither(_tmp659,sizeof(void*),0)));}_tmp389=0;}pr((void*)x->hd);x=x->tl;}{
const char*_tmp65D;void*_tmp65C;(_tmp65C=0,Cyc_fprintf(Cyc_stderr,((_tmp65D="\n",
_tag_dyneither(_tmp65D,sizeof(char),2))),_tag_dyneither(_tmp65C,sizeof(void*),0)));}}
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*));void Cyc_CfFlowInfo_print_dict_set(
struct Cyc_Dict_Dict p,void(*pr)(void*)){struct _RegionHandle _tmp38E=_new_region("r");
struct _RegionHandle*r=& _tmp38E;_push_region(r);{struct _handler_cons _tmp38F;
_push_handler(& _tmp38F);{int _tmp391=0;if(setjmp(_tmp38F.handler))_tmp391=1;if(!
_tmp391){{struct Cyc_Iter_Iter _tmp392=((struct Cyc_Iter_Iter(*)(struct
_RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);struct _tuple18
_tmp393=*((struct _tuple18*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
r,p);int first=1;{const char*_tmp660;void*_tmp65F;(_tmp65F=0,Cyc_fprintf(Cyc_stderr,((
_tmp660="{ ",_tag_dyneither(_tmp660,sizeof(char),3))),_tag_dyneither(_tmp65F,
sizeof(void*),0)));}while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(
_tmp392,& _tmp393)){void*_tmp396=_tmp393.f1;if(!first){{const char*_tmp663;void*
_tmp662;(_tmp662=0,Cyc_fprintf(Cyc_stderr,((_tmp663=", ",_tag_dyneither(_tmp663,
sizeof(char),3))),_tag_dyneither(_tmp662,sizeof(void*),0)));}first=0;}pr(_tmp396);}{
const char*_tmp666;void*_tmp665;(_tmp665=0,Cyc_fprintf(Cyc_stderr,((_tmp666="}\n",
_tag_dyneither(_tmp666,sizeof(char),3))),_tag_dyneither(_tmp665,sizeof(void*),0)));}};
_pop_handler();}else{void*_tmp390=(void*)_exn_thrown;void*_tmp39C=_tmp390;_LL1A5:;
_LL1A6:{const char*_tmp669;void*_tmp668;(_tmp668=0,Cyc_fprintf(Cyc_stderr,((
_tmp669="{ }\n",_tag_dyneither(_tmp669,sizeof(char),5))),_tag_dyneither(_tmp668,
sizeof(void*),0)));}_npop_handler(0);return;_LL1A7:;_LL1A8:(void)_throw(_tmp39C);
_LL1A4:;}}};_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(void*rval);void Cyc_CfFlowInfo_print_absrval(
void*rval){void*_tmp39F=rval;enum Cyc_CfFlowInfo_InitLevel _tmp3A4;enum Cyc_CfFlowInfo_InitLevel
_tmp3A6;struct Cyc_CfFlowInfo_Place*_tmp3A8;int _tmp3AB;struct _dyneither_ptr
_tmp3AC;_LL1AA: {struct Cyc_CfFlowInfo_Zero_struct*_tmp3A0=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp39F;if(_tmp3A0->tag != 0)goto _LL1AC;}_LL1AB:{const char*_tmp66C;void*_tmp66B;(
_tmp66B=0,Cyc_fprintf(Cyc_stderr,((_tmp66C="Zero",_tag_dyneither(_tmp66C,sizeof(
char),5))),_tag_dyneither(_tmp66B,sizeof(void*),0)));}goto _LL1A9;_LL1AC: {struct
Cyc_CfFlowInfo_NotZeroAll_struct*_tmp3A1=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp39F;if(_tmp3A1->tag != 1)goto _LL1AE;}_LL1AD:{const char*_tmp66F;void*_tmp66E;(
_tmp66E=0,Cyc_fprintf(Cyc_stderr,((_tmp66F="NotZeroAll",_tag_dyneither(_tmp66F,
sizeof(char),11))),_tag_dyneither(_tmp66E,sizeof(void*),0)));}goto _LL1A9;_LL1AE: {
struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp3A2=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp39F;if(_tmp3A2->tag != 2)goto _LL1B0;}_LL1AF:{const char*_tmp672;void*_tmp671;(
_tmp671=0,Cyc_fprintf(Cyc_stderr,((_tmp672="NotZeroThis",_tag_dyneither(_tmp672,
sizeof(char),12))),_tag_dyneither(_tmp671,sizeof(void*),0)));}goto _LL1A9;_LL1B0: {
struct Cyc_CfFlowInfo_UnknownR_struct*_tmp3A3=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp39F;if(_tmp3A3->tag != 3)goto _LL1B2;else{_tmp3A4=_tmp3A3->f1;}}_LL1B1:{const
char*_tmp675;void*_tmp674;(_tmp674=0,Cyc_fprintf(Cyc_stderr,((_tmp675="Unknown(",
_tag_dyneither(_tmp675,sizeof(char),9))),_tag_dyneither(_tmp674,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_initlevel(_tmp3A4);{const char*_tmp678;void*_tmp677;(_tmp677=
0,Cyc_fprintf(Cyc_stderr,((_tmp678=")",_tag_dyneither(_tmp678,sizeof(char),2))),
_tag_dyneither(_tmp677,sizeof(void*),0)));}goto _LL1A9;_LL1B2: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp3A5=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp39F;if(_tmp3A5->tag != 4)goto _LL1B4;
else{_tmp3A6=_tmp3A5->f1;}}_LL1B3:{const char*_tmp67B;void*_tmp67A;(_tmp67A=0,Cyc_fprintf(
Cyc_stderr,((_tmp67B="Esc(",_tag_dyneither(_tmp67B,sizeof(char),5))),
_tag_dyneither(_tmp67A,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(
_tmp3A6);{const char*_tmp67E;void*_tmp67D;(_tmp67D=0,Cyc_fprintf(Cyc_stderr,((
_tmp67E=")",_tag_dyneither(_tmp67E,sizeof(char),2))),_tag_dyneither(_tmp67D,
sizeof(void*),0)));}goto _LL1A9;_LL1B4: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3A7=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp39F;if(_tmp3A7->tag != 5)goto
_LL1B6;else{_tmp3A8=_tmp3A7->f1;}}_LL1B5:{const char*_tmp681;void*_tmp680;(
_tmp680=0,Cyc_fprintf(Cyc_stderr,((_tmp681="AddrOf(",_tag_dyneither(_tmp681,
sizeof(char),8))),_tag_dyneither(_tmp680,sizeof(void*),0)));}Cyc_CfFlowInfo_print_place(
_tmp3A8);{const char*_tmp684;void*_tmp683;(_tmp683=0,Cyc_fprintf(Cyc_stderr,((
_tmp684=")",_tag_dyneither(_tmp684,sizeof(char),2))),_tag_dyneither(_tmp683,
sizeof(void*),0)));}goto _LL1A9;_LL1B6: {struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp3A9=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp39F;if(_tmp3A9->tag != 6)goto
_LL1B8;}_LL1B7:{const char*_tmp687;void*_tmp686;(_tmp686=0,Cyc_fprintf(Cyc_stderr,((
_tmp687="TagCmps(?)",_tag_dyneither(_tmp687,sizeof(char),11))),_tag_dyneither(
_tmp686,sizeof(void*),0)));}goto _LL1A9;_LL1B8: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp3AA=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp39F;if(_tmp3AA->tag != 7)goto
_LL1A9;else{_tmp3AB=_tmp3AA->f1;_tmp3AC=_tmp3AA->f2;}}_LL1B9: if(_tmp3AB){const
char*_tmp68A;void*_tmp689;(_tmp689=0,Cyc_fprintf(Cyc_stderr,((_tmp68A="AggrUnion{",
_tag_dyneither(_tmp68A,sizeof(char),11))),_tag_dyneither(_tmp689,sizeof(void*),0)));}
else{const char*_tmp68D;void*_tmp68C;(_tmp68C=0,Cyc_fprintf(Cyc_stderr,((_tmp68D="AggrStruct{",
_tag_dyneither(_tmp68D,sizeof(char),12))),_tag_dyneither(_tmp68C,sizeof(void*),0)));}{
int i=0;for(0;i < _get_dyneither_size(_tmp3AC,sizeof(void*));++ i){Cyc_CfFlowInfo_print_absrval(((
void**)_tmp3AC.curr)[i]);if(i + 1 < _get_dyneither_size(_tmp3AC,sizeof(void*))){
const char*_tmp690;void*_tmp68F;(_tmp68F=0,Cyc_fprintf(Cyc_stderr,((_tmp690=",",
_tag_dyneither(_tmp690,sizeof(char),2))),_tag_dyneither(_tmp68F,sizeof(void*),0)));}}}{
const char*_tmp693;void*_tmp692;(_tmp692=0,Cyc_fprintf(Cyc_stderr,((_tmp693="}",
_tag_dyneither(_tmp693,sizeof(char),2))),_tag_dyneither(_tmp692,sizeof(void*),0)));}
goto _LL1A9;_LL1A9:;}static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*
rval);static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){{const
char*_tmp696;void*_tmp695;(_tmp695=0,Cyc_fprintf(Cyc_stderr,((_tmp696="    ",
_tag_dyneither(_tmp696,sizeof(char),5))),_tag_dyneither(_tmp695,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_root(root);{const char*_tmp699;void*_tmp698;(_tmp698=0,Cyc_fprintf(
Cyc_stderr,((_tmp699=" --> ",_tag_dyneither(_tmp699,sizeof(char),6))),
_tag_dyneither(_tmp698,sizeof(void*),0)));}Cyc_CfFlowInfo_print_absrval(rval);{
const char*_tmp69C;void*_tmp69B;(_tmp69B=0,Cyc_fprintf(Cyc_stderr,((_tmp69C="\n",
_tag_dyneither(_tmp69C,sizeof(char),2))),_tag_dyneither(_tmp69B,sizeof(void*),0)));}}
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);void Cyc_CfFlowInfo_print_flowdict(
struct Cyc_Dict_Dict d){((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(
Cyc_CfFlowInfo_print_flow_mapping,d);}void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo
f);void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo
_tmp3CF=f;int _tmp3D0;struct _tuple12 _tmp3D1;struct Cyc_Dict_Dict _tmp3D2;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3D3;_LL1BB: if((_tmp3CF.BottomFL).tag != 1)goto _LL1BD;_tmp3D0=(int)(_tmp3CF.BottomFL).val;
_LL1BC:{const char*_tmp69F;void*_tmp69E;(_tmp69E=0,Cyc_fprintf(Cyc_stderr,((
_tmp69F="  BottomFL\n",_tag_dyneither(_tmp69F,sizeof(char),12))),_tag_dyneither(
_tmp69E,sizeof(void*),0)));}goto _LL1BA;_LL1BD: if((_tmp3CF.ReachableFL).tag != 2)
goto _LL1BA;_tmp3D1=(struct _tuple12)(_tmp3CF.ReachableFL).val;_tmp3D2=_tmp3D1.f1;
_tmp3D3=_tmp3D1.f3;_LL1BE:{const char*_tmp6A2;void*_tmp6A1;(_tmp6A1=0,Cyc_fprintf(
Cyc_stderr,((_tmp6A2="  ReachableFL:\n",_tag_dyneither(_tmp6A2,sizeof(char),16))),
_tag_dyneither(_tmp6A1,sizeof(void*),0)));}((void(*)(void(*f)(void*,void*),
struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,_tmp3D2);{
const char*_tmp6A5;void*_tmp6A4;(_tmp6A4=0,Cyc_fprintf(Cyc_stderr,((_tmp6A5="\n    consumed: ",
_tag_dyneither(_tmp6A5,sizeof(char),16))),_tag_dyneither(_tmp6A4,sizeof(void*),0)));}((
void(*)(struct Cyc_Dict_Dict p,void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(
_tmp3D3.consumed,Cyc_CfFlowInfo_print_place);{const char*_tmp6A8;void*_tmp6A7;(
_tmp6A7=0,Cyc_fprintf(Cyc_stderr,((_tmp6A8="\n    may_consume: ",_tag_dyneither(
_tmp6A8,sizeof(char),19))),_tag_dyneither(_tmp6A7,sizeof(void*),0)));}((void(*)(
struct Cyc_List_List*x,void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(
_tmp3D3.may_consume,Cyc_CfFlowInfo_print_place);{const char*_tmp6AB;void*_tmp6AA;(
_tmp6AA=0,Cyc_fprintf(Cyc_stderr,((_tmp6AB="\n",_tag_dyneither(_tmp6AB,sizeof(
char),2))),_tag_dyneither(_tmp6AA,sizeof(void*),0)));}goto _LL1BA;_LL1BA:;}int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t);int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){void*_tmp3DE=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp3EA;struct Cyc_Absyn_DatatypeInfo
_tmp3EC;struct Cyc_List_List*_tmp3ED;struct Cyc_List_List*_tmp3EF;struct Cyc_Absyn_AggrInfo
_tmp3F1;struct Cyc_List_List*_tmp3F2;struct Cyc_Absyn_DatatypeFieldInfo _tmp3F4;
struct Cyc_List_List*_tmp3F5;struct Cyc_List_List*_tmp3F7;struct Cyc_Absyn_PtrInfo
_tmp3F9;void*_tmp3FA;struct Cyc_Absyn_PtrAtts _tmp3FB;void*_tmp3FC;struct Cyc_List_List*
_tmp3FF;struct Cyc_List_List*_tmp401;struct Cyc_Absyn_ArrayInfo _tmp403;void*
_tmp404;void*_tmp406;void*_tmp408;void*_tmp40A;void*_tmp40B;void*_tmp40D;_LL1C0: {
struct Cyc_Absyn_VoidType_struct*_tmp3DF=(struct Cyc_Absyn_VoidType_struct*)
_tmp3DE;if(_tmp3DF->tag != 0)goto _LL1C2;}_LL1C1: goto _LL1C3;_LL1C2: {struct Cyc_Absyn_IntType_struct*
_tmp3E0=(struct Cyc_Absyn_IntType_struct*)_tmp3DE;if(_tmp3E0->tag != 6)goto _LL1C4;}
_LL1C3: goto _LL1C5;_LL1C4: {struct Cyc_Absyn_FloatType_struct*_tmp3E1=(struct Cyc_Absyn_FloatType_struct*)
_tmp3DE;if(_tmp3E1->tag != 7)goto _LL1C6;}_LL1C5: goto _LL1C7;_LL1C6: {struct Cyc_Absyn_DoubleType_struct*
_tmp3E2=(struct Cyc_Absyn_DoubleType_struct*)_tmp3DE;if(_tmp3E2->tag != 8)goto
_LL1C8;}_LL1C7: goto _LL1C9;_LL1C8: {struct Cyc_Absyn_EnumType_struct*_tmp3E3=(
struct Cyc_Absyn_EnumType_struct*)_tmp3DE;if(_tmp3E3->tag != 14)goto _LL1CA;}_LL1C9:
goto _LL1CB;_LL1CA: {struct Cyc_Absyn_AnonEnumType_struct*_tmp3E4=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3DE;if(_tmp3E4->tag != 15)goto _LL1CC;}_LL1CB: goto _LL1CD;_LL1CC: {struct Cyc_Absyn_TagType_struct*
_tmp3E5=(struct Cyc_Absyn_TagType_struct*)_tmp3DE;if(_tmp3E5->tag != 20)goto _LL1CE;}
_LL1CD: goto _LL1CF;_LL1CE: {struct Cyc_Absyn_HeapRgn_struct*_tmp3E6=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp3DE;if(_tmp3E6->tag != 21)goto _LL1D0;}_LL1CF: goto _LL1D1;_LL1D0: {struct Cyc_Absyn_Evar_struct*
_tmp3E7=(struct Cyc_Absyn_Evar_struct*)_tmp3DE;if(_tmp3E7->tag != 1)goto _LL1D2;}
_LL1D1: return 0;_LL1D2: {struct Cyc_Absyn_UniqueRgn_struct*_tmp3E8=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp3DE;if(_tmp3E8->tag != 22)goto _LL1D4;}_LL1D3: {void*_tmp40F=rgn;_LL1F5: {
struct Cyc_CfFlowInfo_UniqueRgn_k_struct*_tmp410=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)
_tmp40F;if(_tmp410->tag != 0)goto _LL1F7;}_LL1F6: return 1;_LL1F7:;_LL1F8: return 0;
_LL1F4:;}_LL1D4: {struct Cyc_Absyn_VarType_struct*_tmp3E9=(struct Cyc_Absyn_VarType_struct*)
_tmp3DE;if(_tmp3E9->tag != 2)goto _LL1D6;else{_tmp3EA=_tmp3E9->f1;}}_LL1D5: {void*
_tmp411=rgn;struct Cyc_Absyn_Tvar*_tmp414;_LL1FA: {struct Cyc_CfFlowInfo_UniqueRgn_k_struct*
_tmp412=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)_tmp411;if(_tmp412->tag != 0)
goto _LL1FC;}_LL1FB: return 0;_LL1FC: {struct Cyc_CfFlowInfo_Region_k_struct*_tmp413=(
struct Cyc_CfFlowInfo_Region_k_struct*)_tmp411;if(_tmp413->tag != 1)goto _LL1F9;
else{_tmp414=_tmp413->f1;}}_LL1FD: return Cyc_Absyn_tvar_cmp(_tmp3EA,_tmp414)== 0;
_LL1F9:;}_LL1D6: {struct Cyc_Absyn_DatatypeType_struct*_tmp3EB=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp3DE;if(_tmp3EB->tag != 3)goto _LL1D8;else{_tmp3EC=_tmp3EB->f1;_tmp3ED=_tmp3EC.targs;}}
_LL1D7: _tmp3EF=_tmp3ED;goto _LL1D9;_LL1D8: {struct Cyc_Absyn_TypedefType_struct*
_tmp3EE=(struct Cyc_Absyn_TypedefType_struct*)_tmp3DE;if(_tmp3EE->tag != 18)goto
_LL1DA;else{_tmp3EF=_tmp3EE->f2;}}_LL1D9: _tmp3F2=_tmp3EF;goto _LL1DB;_LL1DA: {
struct Cyc_Absyn_AggrType_struct*_tmp3F0=(struct Cyc_Absyn_AggrType_struct*)
_tmp3DE;if(_tmp3F0->tag != 12)goto _LL1DC;else{_tmp3F1=_tmp3F0->f1;_tmp3F2=_tmp3F1.targs;}}
_LL1DB: _tmp3F5=_tmp3F2;goto _LL1DD;_LL1DC: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp3F3=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp3DE;if(_tmp3F3->tag != 4)
goto _LL1DE;else{_tmp3F4=_tmp3F3->f1;_tmp3F5=_tmp3F4.targs;}}_LL1DD: _tmp3F7=
_tmp3F5;goto _LL1DF;_LL1DE: {struct Cyc_Absyn_JoinEff_struct*_tmp3F6=(struct Cyc_Absyn_JoinEff_struct*)
_tmp3DE;if(_tmp3F6->tag != 24)goto _LL1E0;else{_tmp3F7=_tmp3F6->f1;}}_LL1DF: return((
int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(
Cyc_CfFlowInfo_contains_region,rgn,_tmp3F7);_LL1E0: {struct Cyc_Absyn_PointerType_struct*
_tmp3F8=(struct Cyc_Absyn_PointerType_struct*)_tmp3DE;if(_tmp3F8->tag != 5)goto
_LL1E2;else{_tmp3F9=_tmp3F8->f1;_tmp3FA=_tmp3F9.elt_typ;_tmp3FB=_tmp3F9.ptr_atts;
_tmp3FC=_tmp3FB.rgn;}}_LL1E1: return Cyc_CfFlowInfo_contains_region(rgn,_tmp3FC)
 || Cyc_CfFlowInfo_contains_region(rgn,_tmp3FA);_LL1E2: {struct Cyc_Absyn_FnType_struct*
_tmp3FD=(struct Cyc_Absyn_FnType_struct*)_tmp3DE;if(_tmp3FD->tag != 10)goto _LL1E4;}
_LL1E3: return 0;_LL1E4: {struct Cyc_Absyn_TupleType_struct*_tmp3FE=(struct Cyc_Absyn_TupleType_struct*)
_tmp3DE;if(_tmp3FE->tag != 11)goto _LL1E6;else{_tmp3FF=_tmp3FE->f1;}}_LL1E5: for(0;
_tmp3FF != 0;_tmp3FF=_tmp3FF->tl){if(Cyc_CfFlowInfo_contains_region(rgn,(*((
struct _tuple19*)_tmp3FF->hd)).f2))return 1;}return 0;_LL1E6: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp400=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp3DE;if(_tmp400->tag != 13)goto
_LL1E8;else{_tmp401=_tmp400->f2;}}_LL1E7: for(0;_tmp401 != 0;_tmp401=_tmp401->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp401->hd)->type))
return 1;}return 0;_LL1E8: {struct Cyc_Absyn_ArrayType_struct*_tmp402=(struct Cyc_Absyn_ArrayType_struct*)
_tmp3DE;if(_tmp402->tag != 9)goto _LL1EA;else{_tmp403=_tmp402->f1;_tmp404=_tmp403.elt_type;}}
_LL1E9: _tmp406=_tmp404;goto _LL1EB;_LL1EA: {struct Cyc_Absyn_AccessEff_struct*
_tmp405=(struct Cyc_Absyn_AccessEff_struct*)_tmp3DE;if(_tmp405->tag != 23)goto
_LL1EC;else{_tmp406=(void*)_tmp405->f1;}}_LL1EB: _tmp408=_tmp406;goto _LL1ED;
_LL1EC: {struct Cyc_Absyn_RgnsEff_struct*_tmp407=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp3DE;if(_tmp407->tag != 25)goto _LL1EE;else{_tmp408=(void*)_tmp407->f1;}}_LL1ED:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp408);_LL1EE: {struct Cyc_Absyn_DynRgnType_struct*
_tmp409=(struct Cyc_Absyn_DynRgnType_struct*)_tmp3DE;if(_tmp409->tag != 17)goto
_LL1F0;else{_tmp40A=(void*)_tmp409->f1;_tmp40B=(void*)_tmp409->f2;}}_LL1EF:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp40A) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp40B);_LL1F0: {struct Cyc_Absyn_RgnHandleType_struct*_tmp40C=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp3DE;if(_tmp40C->tag != 16)goto _LL1F2;else{_tmp40D=(void*)_tmp40C->f1;}}_LL1F1:
return 0;_LL1F2: {struct Cyc_Absyn_ValueofType_struct*_tmp40E=(struct Cyc_Absyn_ValueofType_struct*)
_tmp3DE;if(_tmp40E->tag != 19)goto _LL1BF;}_LL1F3: return 0;_LL1BF:;}struct _tuple27{
struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};static void Cyc_CfFlowInfo_kill_root(
struct _tuple27*env,void*root,void*rval);static void Cyc_CfFlowInfo_kill_root(
struct _tuple27*env,void*root,void*rval){struct _tuple27 _tmp416;struct Cyc_CfFlowInfo_FlowEnv*
_tmp417;struct Cyc_Dict_Dict _tmp418;struct Cyc_Dict_Dict*_tmp419;void*_tmp41A;
struct _tuple27*_tmp415=env;_tmp416=*_tmp415;_tmp417=_tmp416.f1;_tmp418=_tmp416.f2;
_tmp419=(struct Cyc_Dict_Dict*)&(*_tmp415).f2;_tmp41A=_tmp416.f3;{void*_tmp41B=
root;struct Cyc_Absyn_Vardecl*_tmp41D;void*_tmp41F;_LL1FF: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp41C=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp41B;if(_tmp41C->tag != 0)goto
_LL201;else{_tmp41D=_tmp41C->f1;}}_LL200: if(Cyc_CfFlowInfo_contains_region(
_tmp41A,_tmp41D->type))rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp417,_tmp41D->type,
_tmp417->unknown_none);*_tmp419=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(*_tmp419,root,rval);goto _LL1FE;_LL201: {struct
Cyc_CfFlowInfo_MallocPt_struct*_tmp41E=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp41B;if(_tmp41E->tag != 1)goto _LL203;else{_tmp41F=(void*)_tmp41E->f2;}}_LL202:
if(!Cyc_CfFlowInfo_contains_region(_tmp41A,_tmp41F))*_tmp419=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp419,root,rval);goto
_LL1FE;_LL203: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp420=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp41B;if(_tmp420->tag != 2)goto _LL1FE;}_LL204: goto _LL1FE;_LL1FE:;}}static struct
Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict fd,void*rgn);static struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict fd,void*rgn){void*_tmp421=
Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp423;_LL206: {struct Cyc_Absyn_VarType_struct*
_tmp422=(struct Cyc_Absyn_VarType_struct*)_tmp421;if(_tmp422->tag != 2)goto _LL208;
else{_tmp423=_tmp422->f1;}}_LL207: {struct Cyc_CfFlowInfo_Region_k_struct _tmp6AC;
struct Cyc_CfFlowInfo_Region_k_struct v=(_tmp6AC.tag=1,((_tmp6AC.f1=_tmp423,
_tmp6AC)));void*v2=(void*)& v;struct _tuple27 _tmp6AD;struct _tuple27 env=(_tmp6AD.f1=
fenv,((_tmp6AD.f2=fenv->mt_flowdict,((_tmp6AD.f3=v2,_tmp6AD)))));((void(*)(void(*
f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f2;}_LL208: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp424=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp421;if(_tmp424->tag != 22)goto
_LL20A;}_LL209: {struct _tuple27 _tmp6AE;struct _tuple27 env=(_tmp6AE.f1=fenv,((
_tmp6AE.f2=fenv->mt_flowdict,((_tmp6AE.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val,
_tmp6AE)))));((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,
struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);return
env.f2;}_LL20A:;_LL20B: {struct Cyc_Core_Impossible_struct _tmp6B4;const char*
_tmp6B3;struct Cyc_Core_Impossible_struct*_tmp6B2;(int)_throw((void*)((_tmp6B2=
_cycalloc(sizeof(*_tmp6B2)),((_tmp6B2[0]=((_tmp6B4.tag=Cyc_Core_Impossible,((
_tmp6B4.f1=((_tmp6B3="kill_flowdict_region",_tag_dyneither(_tmp6B3,sizeof(char),
21))),_tmp6B4)))),_tmp6B2)))));}_LL205:;}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn);static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn){return 0;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);union
Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){union Cyc_CfFlowInfo_FlowInfo _tmp42B=
f;int _tmp42C;struct _tuple12 _tmp42D;struct Cyc_Dict_Dict _tmp42E;struct Cyc_List_List*
_tmp42F;struct Cyc_CfFlowInfo_ConsumeInfo _tmp430;_LL20D: if((_tmp42B.BottomFL).tag
!= 1)goto _LL20F;_tmp42C=(int)(_tmp42B.BottomFL).val;_LL20E: return f;_LL20F: if((
_tmp42B.ReachableFL).tag != 2)goto _LL20C;_tmp42D=(struct _tuple12)(_tmp42B.ReachableFL).val;
_tmp42E=_tmp42D.f1;_tmp42F=_tmp42D.f2;_tmp430=_tmp42D.f3;_LL210: {struct Cyc_Dict_Dict
_tmp431=Cyc_CfFlowInfo_kill_flowdict_region(fenv,_tmp42E,rgn);struct Cyc_List_List*
_tmp432=Cyc_CfFlowInfo_kill_relns_region(_tmp42F,rgn);return Cyc_CfFlowInfo_ReachableFL(
_tmp431,_tmp432,_tmp430);}_LL20C:;}static struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of_rec(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds,struct Cyc_List_List*
path);static struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of_rec(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds,struct Cyc_List_List*
path){void*_tmp433=e->r;struct Cyc_Absyn_Exp*_tmp435;struct Cyc_Absyn_Exp*_tmp437;
struct Cyc_Absyn_Exp*_tmp439;void*_tmp43B;struct Cyc_Absyn_Vardecl*_tmp43D;void*
_tmp43F;struct Cyc_Absyn_Vardecl*_tmp441;void*_tmp443;struct Cyc_Absyn_Vardecl*
_tmp445;void*_tmp447;struct Cyc_Absyn_Vardecl*_tmp449;struct Cyc_Absyn_Exp*_tmp44B;
struct _dyneither_ptr*_tmp44C;struct Cyc_Absyn_Exp*_tmp44E;struct Cyc_Absyn_Exp*
_tmp450;struct Cyc_Absyn_Exp*_tmp451;struct Cyc_Absyn_Exp*_tmp453;struct
_dyneither_ptr*_tmp454;_LL212: {struct Cyc_Absyn_Cast_e_struct*_tmp434=(struct Cyc_Absyn_Cast_e_struct*)
_tmp433;if(_tmp434->tag != 15)goto _LL214;else{_tmp435=_tmp434->f2;}}_LL213:
_tmp437=_tmp435;goto _LL215;_LL214: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp436=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp433;if(_tmp436->tag != 13)
goto _LL216;else{_tmp437=_tmp436->f1;}}_LL215: _tmp439=_tmp437;goto _LL217;_LL216: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp438=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp433;if(_tmp438->tag != 14)goto _LL218;else{_tmp439=_tmp438->f1;}}_LL217: return
Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp439,flds,path);_LL218: {struct Cyc_Absyn_Var_e_struct*
_tmp43A=(struct Cyc_Absyn_Var_e_struct*)_tmp433;if(_tmp43A->tag != 1)goto _LL21A;
else{_tmp43B=(void*)_tmp43A->f2;{struct Cyc_Absyn_Param_b_struct*_tmp43C=(struct
Cyc_Absyn_Param_b_struct*)_tmp43B;if(_tmp43C->tag != 3)goto _LL21A;else{_tmp43D=
_tmp43C->f1;}}}}_LL219: _tmp441=_tmp43D;goto _LL21B;_LL21A: {struct Cyc_Absyn_Var_e_struct*
_tmp43E=(struct Cyc_Absyn_Var_e_struct*)_tmp433;if(_tmp43E->tag != 1)goto _LL21C;
else{_tmp43F=(void*)_tmp43E->f2;{struct Cyc_Absyn_Local_b_struct*_tmp440=(struct
Cyc_Absyn_Local_b_struct*)_tmp43F;if(_tmp440->tag != 4)goto _LL21C;else{_tmp441=
_tmp440->f1;}}}}_LL21B: _tmp445=_tmp441;goto _LL21D;_LL21C: {struct Cyc_Absyn_Var_e_struct*
_tmp442=(struct Cyc_Absyn_Var_e_struct*)_tmp433;if(_tmp442->tag != 1)goto _LL21E;
else{_tmp443=(void*)_tmp442->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp444=(struct Cyc_Absyn_Pat_b_struct*)
_tmp443;if(_tmp444->tag != 5)goto _LL21E;else{_tmp445=_tmp444->f1;}}}}_LL21D: {
struct Cyc_CfFlowInfo_Place _tmp6BE;struct Cyc_CfFlowInfo_VarRoot_struct _tmp6BD;
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp6BC;struct Cyc_CfFlowInfo_UniquePlace*
_tmp6BB;return(_tmp6BB=_region_malloc(r,sizeof(*_tmp6BB)),((_tmp6BB->place=((
_tmp6BE.root=(void*)((_tmp6BC=_region_malloc(r,sizeof(*_tmp6BC)),((_tmp6BC[0]=((
_tmp6BD.tag=0,((_tmp6BD.f1=_tmp445,_tmp6BD)))),_tmp6BC)))),((_tmp6BE.fields=flds,
_tmp6BE)))),((_tmp6BB->path=path,_tmp6BB)))));}_LL21E: {struct Cyc_Absyn_Var_e_struct*
_tmp446=(struct Cyc_Absyn_Var_e_struct*)_tmp433;if(_tmp446->tag != 1)goto _LL220;
else{_tmp447=(void*)_tmp446->f2;{struct Cyc_Absyn_Global_b_struct*_tmp448=(struct
Cyc_Absyn_Global_b_struct*)_tmp447;if(_tmp448->tag != 1)goto _LL220;else{_tmp449=
_tmp448->f1;}}}}_LL21F: {struct Cyc_Core_Impossible_struct _tmp6C4;const char*
_tmp6C3;struct Cyc_Core_Impossible_struct*_tmp6C2;(int)_throw((void*)((_tmp6C2=
_cycalloc(sizeof(*_tmp6C2)),((_tmp6C2[0]=((_tmp6C4.tag=Cyc_Core_Impossible,((
_tmp6C4.f1=((_tmp6C3="non-unique path",_tag_dyneither(_tmp6C3,sizeof(char),16))),
_tmp6C4)))),_tmp6C2)))));}_LL220: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp44A=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp433;if(_tmp44A->tag != 24)goto _LL222;
else{_tmp44B=_tmp44A->f1;_tmp44C=_tmp44A->f2;}}_LL221:{void*_tmp45C=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp44B->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp45E;void*_tmp45F;_LL22B: {struct Cyc_Absyn_PointerType_struct*_tmp45D=(struct
Cyc_Absyn_PointerType_struct*)_tmp45C;if(_tmp45D->tag != 5)goto _LL22D;else{
_tmp45E=_tmp45D->f1;_tmp45F=_tmp45E.elt_typ;}}_LL22C: if(!Cyc_Absyn_is_union_type(
_tmp45F)){struct Cyc_List_List*_tmp6C5;flds=((_tmp6C5=_region_malloc(r,sizeof(*
_tmp6C5)),((_tmp6C5->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp45F,_tmp44C),((
_tmp6C5->tl=flds,_tmp6C5))))));}goto _LL22A;_LL22D:;_LL22E: {struct Cyc_Core_Impossible_struct
_tmp6CB;const char*_tmp6CA;struct Cyc_Core_Impossible_struct*_tmp6C9;(int)_throw((
void*)((_tmp6C9=_cycalloc(sizeof(*_tmp6C9)),((_tmp6C9[0]=((_tmp6CB.tag=Cyc_Core_Impossible,((
_tmp6CB.f1=((_tmp6CA="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp6CA,sizeof(
char),25))),_tmp6CB)))),_tmp6C9)))));}_LL22A:;}_tmp44E=_tmp44B;goto _LL223;_LL222: {
struct Cyc_Absyn_Deref_e_struct*_tmp44D=(struct Cyc_Absyn_Deref_e_struct*)_tmp433;
if(_tmp44D->tag != 22)goto _LL224;else{_tmp44E=_tmp44D->f1;}}_LL223: {struct Cyc_List_List*
_tmp6CC;return Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp44E,0,((_tmp6CC=
_region_malloc(r,sizeof(*_tmp6CC)),((_tmp6CC->hd=flds,((_tmp6CC->tl=path,_tmp6CC)))))));}
_LL224: {struct Cyc_Absyn_Subscript_e_struct*_tmp44F=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp433;if(_tmp44F->tag != 25)goto _LL226;else{_tmp450=_tmp44F->f1;_tmp451=_tmp44F->f2;}}
_LL225: {void*_tmp465=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp450->topt))->v);struct Cyc_Absyn_PtrInfo _tmp468;void*_tmp469;
struct Cyc_Absyn_PtrAtts _tmp46A;union Cyc_Absyn_Constraint*_tmp46B;_LL230: {struct
Cyc_Absyn_TupleType_struct*_tmp466=(struct Cyc_Absyn_TupleType_struct*)_tmp465;
if(_tmp466->tag != 11)goto _LL232;}_LL231: {unsigned int _tmp46C=(Cyc_Evexp_eval_const_uint_exp(
_tmp451)).f1;struct Cyc_List_List*_tmp6CD;return Cyc_CfFlowInfo_unique_place_of_rec(
r,_tmp450,((_tmp6CD=_region_malloc(r,sizeof(*_tmp6CD)),((_tmp6CD->hd=(void*)((
int)_tmp46C),((_tmp6CD->tl=flds,_tmp6CD)))))),path);}_LL232: {struct Cyc_Absyn_PointerType_struct*
_tmp467=(struct Cyc_Absyn_PointerType_struct*)_tmp465;if(_tmp467->tag != 5)goto
_LL234;else{_tmp468=_tmp467->f1;_tmp469=_tmp468.elt_typ;_tmp46A=_tmp468.ptr_atts;
_tmp46B=_tmp46A.bounds;}}_LL233: {struct Cyc_Core_Impossible_struct _tmp6D3;const
char*_tmp6D2;struct Cyc_Core_Impossible_struct*_tmp6D1;(int)_throw((void*)((
_tmp6D1=_cycalloc(sizeof(*_tmp6D1)),((_tmp6D1[0]=((_tmp6D3.tag=Cyc_Core_Impossible,((
_tmp6D3.f1=((_tmp6D2="subscripts unsupported for unique places",_tag_dyneither(
_tmp6D2,sizeof(char),41))),_tmp6D3)))),_tmp6D1)))));}_LL234:;_LL235: {struct Cyc_Core_Impossible_struct
_tmp6D9;const char*_tmp6D8;struct Cyc_Core_Impossible_struct*_tmp6D7;(int)_throw((
void*)((_tmp6D7=_cycalloc(sizeof(*_tmp6D7)),((_tmp6D7[0]=((_tmp6D9.tag=Cyc_Core_Impossible,((
_tmp6D9.f1=((_tmp6D8="unique place subscript -- bad type",_tag_dyneither(_tmp6D8,
sizeof(char),35))),_tmp6D9)))),_tmp6D7)))));}_LL22F:;}_LL226: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp452=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp433;if(_tmp452->tag != 23)goto
_LL228;else{_tmp453=_tmp452->f1;_tmp454=_tmp452->f2;}}_LL227: if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp453->topt))->v)){struct Cyc_Core_Impossible_struct
_tmp6DF;const char*_tmp6DE;struct Cyc_Core_Impossible_struct*_tmp6DD;(int)_throw((
void*)((_tmp6DD=_cycalloc(sizeof(*_tmp6DD)),((_tmp6DD[0]=((_tmp6DF.tag=Cyc_Core_Impossible,((
_tmp6DF.f1=((_tmp6DE="unions can't contain unique places",_tag_dyneither(_tmp6DE,
sizeof(char),35))),_tmp6DF)))),_tmp6DD)))));}{int _tmp477=Cyc_CfFlowInfo_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp453->topt))->v,_tmp454);struct Cyc_List_List*
_tmp6E0;return Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp453,((_tmp6E0=
_region_malloc(r,sizeof(*_tmp6E0)),((_tmp6E0->hd=(void*)_tmp477,((_tmp6E0->tl=
flds,_tmp6E0)))))),path);}_LL228:;_LL229: {struct Cyc_Core_Impossible_struct
_tmp6E6;const char*_tmp6E5;struct Cyc_Core_Impossible_struct*_tmp6E4;(int)_throw((
void*)((_tmp6E4=_cycalloc(sizeof(*_tmp6E4)),((_tmp6E4[0]=((_tmp6E6.tag=Cyc_Core_Impossible,((
_tmp6E6.f1=((_tmp6E5="bad unique path",_tag_dyneither(_tmp6E5,sizeof(char),16))),
_tmp6E6)))),_tmp6E4)))));}_LL211:;}struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_UniquePlace*
Cyc_CfFlowInfo_unique_place_of(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
return Cyc_CfFlowInfo_unique_place_of_rec(r,e,0,0);}struct _tuple28{struct Cyc_CfFlowInfo_FlowEnv*
f1;struct Cyc_Dict_Dict*f2;struct Cyc_Dict_Dict f3;struct Cyc_Position_Segment*f4;};
static void Cyc_CfFlowInfo_consume_f(struct _tuple28*env,struct Cyc_CfFlowInfo_Place*
place);static void Cyc_CfFlowInfo_consume_f(struct _tuple28*env,struct Cyc_CfFlowInfo_Place*
place){struct _tuple28 _tmp47D;struct Cyc_CfFlowInfo_FlowEnv*_tmp47E;struct Cyc_Dict_Dict*
_tmp47F;struct Cyc_Dict_Dict _tmp480;struct Cyc_Position_Segment*_tmp481;struct
_tuple28*_tmp47C=env;_tmp47D=*_tmp47C;_tmp47E=_tmp47D.f1;_tmp47F=_tmp47D.f2;
_tmp480=_tmp47D.f3;_tmp481=_tmp47D.f4;{void*rval=_tmp47E->zero;if(((int(*)(
struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp480,place->root,&
rval) && rval == _tmp47E->zero)return;if(((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)((
struct Cyc_Dict_Dict*)_tmp47F,place,_tmp481)){struct Cyc_Position_Segment*_tmp482=((
struct Cyc_Position_Segment*(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))
Cyc_Dict_lookup)(*_tmp47F,place);const char*_tmp6E8;const char*_tmp6E7;((void(*)(
struct Cyc_CfFlowInfo_Place*place,struct _dyneither_ptr(*toString)(struct Cyc_CfFlowInfo_Place*),
struct _dyneither_ptr err_msg1,struct _dyneither_ptr err_msg2,struct Cyc_Position_Segment*
consumed_loc,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_unique_err)(place,
Cyc_CfFlowInfo_place_err_string,((_tmp6E7="May consume non-aliased pointer %s more than once (cf. %s)",
_tag_dyneither(_tmp6E7,sizeof(char),59))),((_tmp6E8="May consume non-aliased pointer %s more than once",
_tag_dyneither(_tmp6E8,sizeof(char),50))),_tmp482,_tmp481);}}}union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo f);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_consume_unique_rvals(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f){union Cyc_CfFlowInfo_FlowInfo _tmp485=f;int _tmp486;struct _tuple12 _tmp487;struct
Cyc_Dict_Dict _tmp488;struct Cyc_List_List*_tmp489;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp48A;_LL237: if((_tmp485.BottomFL).tag != 1)goto _LL239;_tmp486=(int)(_tmp485.BottomFL).val;
_LL238: return f;_LL239: if((_tmp485.ReachableFL).tag != 2)goto _LL236;_tmp487=(
struct _tuple12)(_tmp485.ReachableFL).val;_tmp488=_tmp487.f1;_tmp489=_tmp487.f2;
_tmp48A=_tmp487.f3;_LL23A: if(_tmp48A.may_consume == 0)return f;{struct Cyc_Dict_Dict
_tmp48B=_tmp48A.consumed;struct _tuple28 _tmp6E9;struct _tuple28 _tmp48C=(_tmp6E9.f1=
fenv,((_tmp6E9.f2=& _tmp48B,((_tmp6E9.f3=_tmp488,((_tmp6E9.f4=loc,_tmp6E9)))))));((
void(*)(void(*f)(struct _tuple28*,struct Cyc_CfFlowInfo_Place*),struct _tuple28*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_CfFlowInfo_consume_f,& _tmp48C,
_tmp48A.may_consume);{struct Cyc_CfFlowInfo_ConsumeInfo _tmp6EA;return Cyc_CfFlowInfo_ReachableFL(
_tmp488,_tmp489,((_tmp6EA.consumed=_tmp48B,((_tmp6EA.may_consume=0,_tmp6EA)))));}}
_LL236:;}void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,
union Cyc_CfFlowInfo_FlowInfo f);void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo _tmp48F=f;int
_tmp490;struct _tuple12 _tmp491;struct Cyc_Dict_Dict _tmp492;struct Cyc_List_List*
_tmp493;struct Cyc_CfFlowInfo_ConsumeInfo _tmp494;_LL23C: if((_tmp48F.BottomFL).tag
!= 1)goto _LL23E;_tmp490=(int)(_tmp48F.BottomFL).val;_LL23D: return;_LL23E: if((
_tmp48F.ReachableFL).tag != 2)goto _LL23B;_tmp491=(struct _tuple12)(_tmp48F.ReachableFL).val;
_tmp492=_tmp491.f1;_tmp493=_tmp491.f2;_tmp494=_tmp491.f3;_LL23F: {struct Cyc_List_List*
_tmp495=_tmp494.may_consume;while(_tmp495 != 0){if(((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp494.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp495->hd)){struct Cyc_Position_Segment*_tmp496=((struct Cyc_Position_Segment*(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_lookup)(_tmp494.consumed,(
struct Cyc_CfFlowInfo_Place*)_tmp495->hd);{const char*_tmp6EC;const char*_tmp6EB;((
void(*)(struct Cyc_CfFlowInfo_Place*place,struct _dyneither_ptr(*toString)(struct
Cyc_CfFlowInfo_Place*),struct _dyneither_ptr err_msg1,struct _dyneither_ptr err_msg2,
struct Cyc_Position_Segment*consumed_loc,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_unique_err)((
struct Cyc_CfFlowInfo_Place*)_tmp495->hd,Cyc_CfFlowInfo_place_err_string,((
_tmp6EB="Read through possibly consumed unique pointer %s (cf. %s)",
_tag_dyneither(_tmp6EB,sizeof(char),58))),((_tmp6EC="Read through possibly consumed unique pointer %s",
_tag_dyneither(_tmp6EC,sizeof(char),49))),_tmp496,loc);}break;}_tmp495=_tmp495->tl;}
return;}_LL23B:;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_drop_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo f);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f){union Cyc_CfFlowInfo_FlowInfo _tmp499=f;int _tmp49A;struct _tuple12 _tmp49B;struct
Cyc_Dict_Dict _tmp49C;struct Cyc_List_List*_tmp49D;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp49E;_LL241: if((_tmp499.BottomFL).tag != 1)goto _LL243;_tmp49A=(int)(_tmp499.BottomFL).val;
_LL242: return f;_LL243: if((_tmp499.ReachableFL).tag != 2)goto _LL240;_tmp49B=(
struct _tuple12)(_tmp499.ReachableFL).val;_tmp49C=_tmp49B.f1;_tmp49D=_tmp49B.f2;
_tmp49E=_tmp49B.f3;_LL244: _tmp49E.may_consume=0;return Cyc_CfFlowInfo_ReachableFL(
_tmp49C,_tmp49D,_tmp49E);_LL240:;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_readthrough_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo f);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f){Cyc_CfFlowInfo_check_unique_rvals(loc,f);{union Cyc_CfFlowInfo_FlowInfo _tmp49F=
f;int _tmp4A0;struct _tuple12 _tmp4A1;struct Cyc_Dict_Dict _tmp4A2;struct Cyc_List_List*
_tmp4A3;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4A4;_LL246: if((_tmp49F.BottomFL).tag
!= 1)goto _LL248;_tmp4A0=(int)(_tmp49F.BottomFL).val;_LL247: return f;_LL248: if((
_tmp49F.ReachableFL).tag != 2)goto _LL245;_tmp4A1=(struct _tuple12)(_tmp49F.ReachableFL).val;
_tmp4A2=_tmp4A1.f1;_tmp4A3=_tmp4A1.f2;_tmp4A4=_tmp4A1.f3;_LL249: _tmp4A4.may_consume=
0;return Cyc_CfFlowInfo_ReachableFL(_tmp4A2,_tmp4A3,_tmp4A4);_LL245:;}}struct
_tuple14 Cyc_CfFlowInfo_save_consume_info(struct Cyc_CfFlowInfo_FlowEnv*fenv,union
Cyc_CfFlowInfo_FlowInfo f,int clear);struct _tuple14 Cyc_CfFlowInfo_save_consume_info(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,int clear){union
Cyc_CfFlowInfo_FlowInfo _tmp4A5=f;int _tmp4A6;struct _tuple12 _tmp4A7;struct Cyc_Dict_Dict
_tmp4A8;struct Cyc_List_List*_tmp4A9;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4AA;
_LL24B: if((_tmp4A5.BottomFL).tag != 1)goto _LL24D;_tmp4A6=(int)(_tmp4A5.BottomFL).val;
_LL24C: {struct Cyc_CfFlowInfo_ConsumeInfo _tmp6EF;struct _tuple14 _tmp6EE;return(
_tmp6EE.f1=((_tmp6EF.consumed=fenv->mt_place_set,((_tmp6EF.may_consume=0,_tmp6EF)))),((
_tmp6EE.f2=f,_tmp6EE)));}_LL24D: if((_tmp4A5.ReachableFL).tag != 2)goto _LL24A;
_tmp4A7=(struct _tuple12)(_tmp4A5.ReachableFL).val;_tmp4A8=_tmp4A7.f1;_tmp4A9=
_tmp4A7.f2;_tmp4AA=_tmp4A7.f3;_LL24E: if(clear){struct Cyc_CfFlowInfo_ConsumeInfo
_tmp6F2;struct _tuple14 _tmp6F1;return(_tmp6F1.f1=_tmp4AA,((_tmp6F1.f2=Cyc_CfFlowInfo_ReachableFL(
_tmp4A8,_tmp4A9,((_tmp6F2.consumed=fenv->mt_place_set,((_tmp6F2.may_consume=0,
_tmp6F2))))),_tmp6F1)));}else{struct _tuple14 _tmp6F3;return(_tmp6F3.f1=_tmp4AA,((
_tmp6F3.f2=f,_tmp6F3)));}_LL24A:;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_restore_consume_info(
union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_CfFlowInfo_ConsumeInfo c,int
may_consume_only);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_restore_consume_info(
union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_CfFlowInfo_ConsumeInfo c,int
may_consume_only){union Cyc_CfFlowInfo_FlowInfo _tmp4B0=f;int _tmp4B1;struct
_tuple12 _tmp4B2;struct Cyc_Dict_Dict _tmp4B3;struct Cyc_List_List*_tmp4B4;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp4B5;_LL250: if((_tmp4B0.BottomFL).tag != 1)goto _LL252;_tmp4B1=(int)(_tmp4B0.BottomFL).val;
_LL251: return f;_LL252: if((_tmp4B0.ReachableFL).tag != 2)goto _LL24F;_tmp4B2=(
struct _tuple12)(_tmp4B0.ReachableFL).val;_tmp4B3=_tmp4B2.f1;_tmp4B4=_tmp4B2.f2;
_tmp4B5=_tmp4B2.f3;_LL253: if(may_consume_only){struct Cyc_CfFlowInfo_ConsumeInfo
_tmp6F4;return Cyc_CfFlowInfo_ReachableFL(_tmp4B3,_tmp4B4,((_tmp6F4.consumed=
_tmp4B5.consumed,((_tmp6F4.may_consume=c.may_consume,_tmp6F4)))));}return Cyc_CfFlowInfo_ReachableFL(
_tmp4B3,_tmp4B4,c);_LL24F:;}
