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
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;int Cyc_Position_segment_equals(
struct Cyc_Position_Segment*,struct Cyc_Position_Segment*);struct _dyneither_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dyneither_ptr source;
struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};extern char
Cyc_Position_Nocontext[14];struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct
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
struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;struct Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct
_tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple3{void*f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct
_union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct
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
;struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*
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
_tuple4{void*f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};
struct _tuple5{void*f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5
val;};struct _tuple6{void*f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6
val;};struct _tuple7{void*f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;
struct _tuple7 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};
struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};struct Cyc_Absyn_VarargCallInfo{int num_varargs;
struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;
unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;
void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct
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
struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct
_tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_struct{
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
struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{void*sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;void*ret_type;
struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;
struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};
struct Cyc_Absyn_Datatypedecl{void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;
struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct
_tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_conref_def(void*y,
union Cyc_Absyn_Constraint*x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
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
struct _tuple10{unsigned int f1;int f2;};struct _tuple10 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_UniquePlace{struct
Cyc_CfFlowInfo_Place place;struct Cyc_List_List*path;};struct
_union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple11{struct Cyc_Absyn_Vardecl*
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
rop;};struct Cyc_CfFlowInfo_TagCmp{void*cmp;void*bd;};char Cyc_CfFlowInfo_HasTagCmps[
15]="\000\000\000\000HasTagCmps\000";struct Cyc_CfFlowInfo_HasTagCmps_struct{char*
tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_IsZero[11]="\000\000\000\000IsZero\000";
char Cyc_CfFlowInfo_NotZero[12]="\000\000\000\000NotZero\000";struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[13]="\000\000\000\000UnknownZ\000";
struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};struct Cyc_List_List*
Cyc_CfFlowInfo_copy_tagcmps(struct _RegionHandle*,struct Cyc_List_List*);struct
_union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct
_union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct
_union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_UnknownL();struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};
struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;int f1;
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
struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*unknown_none;void*
unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict
mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*
f);int Cyc_CfFlowInfo_root_cmp(void*,void*);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);void Cyc_CfFlowInfo_print_absrval(
void*rval);void Cyc_CfFlowInfo_print_initlevel(void*il);void Cyc_CfFlowInfo_print_root(
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
f,void*rgn);struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;
};int Cyc_CfFlowInfo_contains_region(void*rgn,void*t);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f);void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo f);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_drop_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo f);struct _tuple14{
struct Cyc_CfFlowInfo_ConsumeInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct
_tuple14 Cyc_CfFlowInfo_save_consume_info(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo
f,int clear);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_restore_consume_info(
union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_CfFlowInfo_ConsumeInfo c,int
may_consume_only);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*
place);struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of(struct
_RegionHandle*r,struct Cyc_Absyn_Exp*e);union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(
struct Cyc_CfFlowInfo_Place*x);union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(
struct Cyc_CfFlowInfo_Place*x){union Cyc_CfFlowInfo_AbsLVal _tmp3E8;return((_tmp3E8.PlaceL).val=
x,(((_tmp3E8.PlaceL).tag=1,_tmp3E8)));}union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){union Cyc_CfFlowInfo_AbsLVal
_tmp3E9;return((_tmp3E9.UnknownL).val=0,(((_tmp3E9.UnknownL).tag=2,_tmp3E9)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo _tmp3EA;return((_tmp3EA.BottomFL).val=
0,(((_tmp3EA.BottomFL).tag=1,_tmp3EA)));}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(
struct Cyc_Dict_Dict fd,struct Cyc_List_List*r,struct Cyc_CfFlowInfo_ConsumeInfo c);
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,
struct Cyc_List_List*r,struct Cyc_CfFlowInfo_ConsumeInfo c){struct _tuple12 _tmp3ED;
union Cyc_CfFlowInfo_FlowInfo _tmp3EC;return((_tmp3EC.ReachableFL).val=((_tmp3ED.f1=
fd,((_tmp3ED.f2=r,((_tmp3ED.f3=c,_tmp3ED)))))),(((_tmp3EC.ReachableFL).tag=2,
_tmp3EC)));}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*
p2);int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(
struct _RegionHandle*r);struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(
struct _RegionHandle*r){static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,{.Null_c={
1,0}}};static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)Cyc_Absyn_EmptyAnnot};
struct Cyc_CfFlowInfo_UnknownR_struct*_tmp415;struct Cyc_CfFlowInfo_UnknownR_struct
_tmp414;struct Cyc_CfFlowInfo_UnknownR_struct*_tmp413;struct Cyc_CfFlowInfo_UnknownR_struct
_tmp412;struct Cyc_CfFlowInfo_UnknownR_struct*_tmp411;struct Cyc_CfFlowInfo_UnknownR_struct
_tmp410;struct Cyc_CfFlowInfo_Esc_struct*_tmp40F;struct Cyc_CfFlowInfo_Esc_struct
_tmp40E;struct Cyc_CfFlowInfo_Esc_struct*_tmp40D;struct Cyc_CfFlowInfo_Esc_struct
_tmp40C;struct Cyc_CfFlowInfo_Esc_struct*_tmp40B;struct Cyc_CfFlowInfo_Esc_struct
_tmp40A;struct Cyc_CfFlowInfo_Place*_tmp409;struct Cyc_CfFlowInfo_MallocPt_struct
_tmp408;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp407;struct Cyc_CfFlowInfo_FlowEnv*
_tmp406;return(_tmp406=_region_malloc(r,sizeof(*_tmp406)),((_tmp406->r=r,((
_tmp406->unknown_none=(void*)((_tmp415=_region_malloc(r,sizeof(*_tmp415)),((
_tmp415[0]=((_tmp414.tag=0,((_tmp414.f1=(void*)((void*)0),_tmp414)))),_tmp415)))),((
_tmp406->unknown_this=(void*)((_tmp413=_region_malloc(r,sizeof(*_tmp413)),((
_tmp413[0]=((_tmp412.tag=0,((_tmp412.f1=(void*)((void*)1),_tmp412)))),_tmp413)))),((
_tmp406->unknown_all=(void*)((_tmp411=_region_malloc(r,sizeof(*_tmp411)),((
_tmp411[0]=((_tmp410.tag=0,((_tmp410.f1=(void*)((void*)2),_tmp410)))),_tmp411)))),((
_tmp406->esc_none=(void*)((_tmp40F=_region_malloc(r,sizeof(*_tmp40F)),((_tmp40F[
0]=((_tmp40E.tag=1,((_tmp40E.f1=(void*)((void*)0),_tmp40E)))),_tmp40F)))),((
_tmp406->esc_this=(void*)((_tmp40D=_region_malloc(r,sizeof(*_tmp40D)),((_tmp40D[
0]=((_tmp40C.tag=1,((_tmp40C.f1=(void*)((void*)1),_tmp40C)))),_tmp40D)))),((
_tmp406->esc_all=(void*)((_tmp40B=_region_malloc(r,sizeof(*_tmp40B)),((_tmp40B[0]=((
_tmp40A.tag=1,((_tmp40A.f1=(void*)((void*)2),_tmp40A)))),_tmp40B)))),((_tmp406->mt_flowdict=
Cyc_Dict_rempty(r,Cyc_CfFlowInfo_root_cmp),((_tmp406->mt_place_set=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))
Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp406->dummy_place=((_tmp409=
_region_malloc(r,sizeof(*_tmp409)),((_tmp409->root=(void*)((_tmp407=
_region_malloc(r,sizeof(*_tmp407)),((_tmp407[0]=((_tmp408.tag=1,((_tmp408.f1=&
dummy_exp,((_tmp408.f2=(void*)((void*)0),_tmp408)))))),_tmp407)))),((_tmp409->fields=
0,_tmp409)))))),_tmp406)))))))))))))))))))));}int Cyc_CfFlowInfo_update_place_set(
struct Cyc_Dict_Dict*set,void*place,struct Cyc_Position_Segment*loc);int Cyc_CfFlowInfo_update_place_set(
struct Cyc_Dict_Dict*set,void*place,struct Cyc_Position_Segment*loc){if(set != 0){
if(((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(*set,place))return 1;
else{*set=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Position_Segment*
v))Cyc_Dict_insert)(*set,place,loc);}}return 0;}struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place){void*_tmp17;struct Cyc_List_List*_tmp18;struct
Cyc_CfFlowInfo_Place _tmp16=*place;_tmp17=_tmp16.root;_tmp18=_tmp16.fields;{void*
_tmp19=_tmp17;struct Cyc_Absyn_Vardecl*_tmp1A;_LL1: if(*((int*)_tmp19)!= 0)goto
_LL3;_tmp1A=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp19)->f1;_LL2: if(_tmp18 == 
0)return Cyc_Absynpp_qvar2string(_tmp1A->name);else{const char*_tmp419;void*
_tmp418[1];struct Cyc_String_pa_struct _tmp417;return(struct _dyneither_ptr)((
_tmp417.tag=0,((_tmp417.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp1A->name)),((_tmp418[0]=& _tmp417,Cyc_aprintf(((_tmp419="reachable from %s",
_tag_dyneither(_tmp419,sizeof(char),18))),_tag_dyneither(_tmp418,sizeof(void*),1))))))));}
_LL3:;_LL4: {const char*_tmp41C;void*_tmp41B;(_tmp41B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp41C="error locations not for VarRoots",
_tag_dyneither(_tmp41C,sizeof(char),33))),_tag_dyneither(_tmp41B,sizeof(void*),0)));}
_LL0:;}}struct _dyneither_ptr Cyc_CfFlowInfo_uplace_err_string(struct Cyc_CfFlowInfo_UniquePlace*
place);struct _dyneither_ptr Cyc_CfFlowInfo_uplace_err_string(struct Cyc_CfFlowInfo_UniquePlace*
place){return Cyc_CfFlowInfo_place_err_string(& place->place);}static void Cyc_CfFlowInfo_unique_err(
void*place,struct _dyneither_ptr(*toString)(void*),struct _dyneither_ptr err_msg1,
struct _dyneither_ptr err_msg2,struct Cyc_Position_Segment*consumed_loc,struct Cyc_Position_Segment*
loc);static void Cyc_CfFlowInfo_unique_err(void*place,struct _dyneither_ptr(*
toString)(void*),struct _dyneither_ptr err_msg1,struct _dyneither_ptr err_msg2,
struct Cyc_Position_Segment*consumed_loc,struct Cyc_Position_Segment*loc){struct
_dyneither_ptr _tmp20=toString(place);if(!Cyc_Position_segment_equals(
consumed_loc,loc)){struct _dyneither_ptr _tmp21=Cyc_Position_string_of_segment(
consumed_loc);void*_tmp41F[2];struct Cyc_String_pa_struct _tmp41E;struct Cyc_String_pa_struct
_tmp41D;(_tmp41D.tag=0,((_tmp41D.f1=(struct _dyneither_ptr)_tmp21,((_tmp41E.tag=0,((
_tmp41E.f1=(struct _dyneither_ptr)_tmp20,((_tmp41F[0]=& _tmp41E,((_tmp41F[1]=&
_tmp41D,Cyc_Tcutil_terr(loc,err_msg1,_tag_dyneither(_tmp41F,sizeof(void*),2)))))))))))));}
else{void*_tmp421[1];struct Cyc_String_pa_struct _tmp420;(_tmp420.tag=0,((_tmp420.f1=(
struct _dyneither_ptr)_tmp20,((_tmp421[0]=& _tmp420,Cyc_Tcutil_terr(loc,err_msg2,
_tag_dyneither(_tmp421,sizeof(void*),1)))))));}}struct _tuple15{int f1;struct
_dyneither_ptr(*f2)(void*);};static struct Cyc_Position_Segment*Cyc_CfFlowInfo_combine_consume_f(
struct _tuple15*env,void*place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*
loc2);static struct Cyc_Position_Segment*Cyc_CfFlowInfo_combine_consume_f(struct
_tuple15*env,void*place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*
loc2){struct _tuple15 _tmp28;int _tmp29;struct _dyneither_ptr(*_tmp2A)(void*);struct
_tuple15*_tmp27=env;_tmp28=*_tmp27;_tmp29=_tmp28.f1;_tmp2A=_tmp28.f2;if(_tmp29){
const char*_tmp423;const char*_tmp422;Cyc_CfFlowInfo_unique_err(place,_tmp2A,((
_tmp422="May consume non-aliased pointer %s more than once (cf. %s)",
_tag_dyneither(_tmp422,sizeof(char),59))),((_tmp423="May consume non-aliased pointer %s more than once",
_tag_dyneither(_tmp423,sizeof(char),50))),loc1,loc2);}return loc1;}struct _tuple16{
int f1;struct _dyneither_ptr(*f2)(struct Cyc_CfFlowInfo_Place*place);};struct Cyc_Dict_Dict
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int
disjoint);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2,int disjoint){struct _tuple16 _tmp424;struct _tuple16 _tmp2D=(
_tmp424.f1=disjoint,((_tmp424.f2=Cyc_CfFlowInfo_place_err_string,_tmp424)));
struct Cyc_Dict_Dict _tmp2E=((struct Cyc_Dict_Dict(*)(struct Cyc_Position_Segment*(*
f)(struct _tuple16*,struct Cyc_CfFlowInfo_Place*,struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*),struct _tuple16*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_union_two_c)((struct Cyc_Position_Segment*(*)(struct _tuple16*env,
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*
loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp2D,s1,s2);return _tmp2E;}struct
_tuple17{int f1;struct _dyneither_ptr(*f2)(struct Cyc_CfFlowInfo_UniquePlace*place);
};struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_uplace_set(struct Cyc_Dict_Dict s1,
struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_uplace_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint){struct _tuple17 _tmp425;
struct _tuple17 _tmp30=(_tmp425.f1=disjoint,((_tmp425.f2=Cyc_CfFlowInfo_uplace_err_string,
_tmp425)));return((struct Cyc_Dict_Dict(*)(struct Cyc_Position_Segment*(*f)(struct
_tuple17*,struct Cyc_CfFlowInfo_UniquePlace*,struct Cyc_Position_Segment*,struct
Cyc_Position_Segment*),struct _tuple17*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_union_two_c)((struct Cyc_Position_Segment*(*)(struct _tuple17*env,
struct Cyc_CfFlowInfo_UniquePlace*place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*
loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp30,s1,s2);}struct _tuple18{void*f1;
struct Cyc_Position_Segment*f2;};int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2){if((int)s1.t == (int)s2.t)return 1;{struct _handler_cons
_tmp32;_push_handler(& _tmp32);{int _tmp34=0;if(setjmp(_tmp32.handler))_tmp34=1;
if(!_tmp34){{struct _RegionHandle _tmp35=_new_region("temp");struct _RegionHandle*
temp=& _tmp35;_push_region(temp);{struct Cyc_Iter_Iter _tmp36=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(temp,s1);
struct _tuple18 _tmp37=*((struct _tuple18*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict
d))Cyc_Dict_rchoose)(temp,s1);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))
Cyc_Iter_next)(_tmp36,& _tmp37)){void*_tmp38=_tmp37.f1;if(!((int(*)(struct Cyc_Dict_Dict
d,void*k))Cyc_Dict_member)(s2,_tmp38)){int _tmp39=0;_npop_handler(1);return _tmp39;}}{
int _tmp3A=1;_npop_handler(1);return _tmp3A;}};_pop_region(temp);};_pop_handler();}
else{void*_tmp33=(void*)_exn_thrown;void*_tmp3C=_tmp33;_LL6:;_LL7: return 1;_LL8:;
_LL9:(void)_throw(_tmp3C);_LL5:;}}}}int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2){return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(
s2,s1);}int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct
_dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,
struct _dyneither_ptr*f){int n=0;for(0;(unsigned int)fs;fs=fs->tl){struct
_dyneither_ptr*_tmp3D=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;if(Cyc_strptrcmp(
_tmp3D,f)== 0)return n;n=n + 1;}{struct Cyc_Core_Impossible_struct _tmp432;const char*
_tmp431;void*_tmp430[1];struct Cyc_String_pa_struct _tmp42F;struct Cyc_Core_Impossible_struct*
_tmp42E;(int)_throw((void*)((_tmp42E=_cycalloc(sizeof(*_tmp42E)),((_tmp42E[0]=((
_tmp432.tag=Cyc_Core_Impossible,((_tmp432.f1=(struct _dyneither_ptr)((_tmp42F.tag=
0,((_tmp42F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp430[0]=&
_tmp42F,Cyc_aprintf(((_tmp431="get_field_index_fs failed: %s",_tag_dyneither(
_tmp431,sizeof(char),30))),_tag_dyneither(_tmp430,sizeof(void*),1)))))))),
_tmp432)))),_tmp42E)))));}}int Cyc_CfFlowInfo_get_field_index(void*t,struct
_dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*
f){void*_tmp43=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp44;union Cyc_Absyn_AggrInfoU
_tmp45;struct Cyc_List_List*_tmp46;_LLB: if(_tmp43 <= (void*)4)goto _LLF;if(*((int*)
_tmp43)!= 10)goto _LLD;_tmp44=((struct Cyc_Absyn_AggrType_struct*)_tmp43)->f1;
_tmp45=_tmp44.aggr_info;_LLC: {struct Cyc_List_List*_tmp47=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp45))->impl))->fields;_tmp46=_tmp47;
goto _LLE;}_LLD: if(*((int*)_tmp43)!= 11)goto _LLF;_tmp46=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp43)->f2;_LLE: return Cyc_CfFlowInfo_get_field_index_fs(_tmp46,f);_LLF:;_LL10: {
struct Cyc_Core_Impossible_struct _tmp43F;const char*_tmp43E;void*_tmp43D[1];struct
Cyc_String_pa_struct _tmp43C;struct Cyc_Core_Impossible_struct*_tmp43B;(int)_throw((
void*)((_tmp43B=_cycalloc(sizeof(*_tmp43B)),((_tmp43B[0]=((_tmp43F.tag=Cyc_Core_Impossible,((
_tmp43F.f1=(struct _dyneither_ptr)((_tmp43C.tag=0,((_tmp43C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp43D[0]=&
_tmp43C,Cyc_aprintf(((_tmp43E="get_field_index failed: %s",_tag_dyneither(
_tmp43E,sizeof(char),27))),_tag_dyneither(_tmp43D,sizeof(void*),1)))))))),
_tmp43F)))),_tmp43B)))));}_LLA:;}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);int
Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)r1 == (int)r2)return 0;{struct
_tuple0 _tmp440;struct _tuple0 _tmp4E=(_tmp440.f1=r1,((_tmp440.f2=r2,_tmp440)));
void*_tmp4F;struct Cyc_Absyn_Vardecl*_tmp50;void*_tmp51;struct Cyc_Absyn_Vardecl*
_tmp52;void*_tmp53;void*_tmp54;void*_tmp55;struct Cyc_Absyn_Exp*_tmp56;void*
_tmp57;struct Cyc_Absyn_Exp*_tmp58;void*_tmp59;void*_tmp5A;void*_tmp5B;int _tmp5C;
void*_tmp5D;int _tmp5E;_LL12: _tmp4F=_tmp4E.f1;if(*((int*)_tmp4F)!= 0)goto _LL14;
_tmp50=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp4F)->f1;_tmp51=_tmp4E.f2;if(*((
int*)_tmp51)!= 0)goto _LL14;_tmp52=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp51)->f1;
_LL13: return(int)_tmp50 - (int)_tmp52;_LL14: _tmp53=_tmp4E.f1;if(*((int*)_tmp53)!= 
0)goto _LL16;_LL15: return - 1;_LL16: _tmp54=_tmp4E.f2;if(*((int*)_tmp54)!= 0)goto
_LL18;_LL17: return 1;_LL18: _tmp55=_tmp4E.f1;if(*((int*)_tmp55)!= 1)goto _LL1A;
_tmp56=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp55)->f1;_tmp57=_tmp4E.f2;if(*((
int*)_tmp57)!= 1)goto _LL1A;_tmp58=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp57)->f1;
_LL19: return(int)_tmp56 - (int)_tmp58;_LL1A: _tmp59=_tmp4E.f1;if(*((int*)_tmp59)!= 
1)goto _LL1C;_LL1B: return - 1;_LL1C: _tmp5A=_tmp4E.f2;if(*((int*)_tmp5A)!= 1)goto
_LL1E;_LL1D: return 1;_LL1E: _tmp5B=_tmp4E.f1;if(*((int*)_tmp5B)!= 2)goto _LL11;
_tmp5C=((struct Cyc_CfFlowInfo_InitParam_struct*)_tmp5B)->f1;_tmp5D=_tmp4E.f2;if(*((
int*)_tmp5D)!= 2)goto _LL11;_tmp5E=((struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp5D)->f1;_LL1F: return _tmp5C - _tmp5E;_LL11:;}}static int Cyc_CfFlowInfo_intlist_cmp(
struct Cyc_List_List*list1,struct Cyc_List_List*list2);static int Cyc_CfFlowInfo_intlist_cmp(
struct Cyc_List_List*list1,struct Cyc_List_List*list2){return((int(*)(int(*cmp)(
int,int),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,
list1,list2);}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct
Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*
p1,struct Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)p2)return 0;{void*_tmp60;
struct Cyc_List_List*_tmp61;struct Cyc_CfFlowInfo_Place _tmp5F=*p1;_tmp60=_tmp5F.root;
_tmp61=_tmp5F.fields;{void*_tmp63;struct Cyc_List_List*_tmp64;struct Cyc_CfFlowInfo_Place
_tmp62=*p2;_tmp63=_tmp62.root;_tmp64=_tmp62.fields;{int i=Cyc_CfFlowInfo_root_cmp(
_tmp60,_tmp63);if(i != 0)return i;return Cyc_CfFlowInfo_intlist_cmp(_tmp61,_tmp64);}}}}
int Cyc_CfFlowInfo_uplace_cmp(struct Cyc_CfFlowInfo_UniquePlace*p1,struct Cyc_CfFlowInfo_UniquePlace*
p2);int Cyc_CfFlowInfo_uplace_cmp(struct Cyc_CfFlowInfo_UniquePlace*p1,struct Cyc_CfFlowInfo_UniquePlace*
p2){if((int)p1 == (int)p2)return 0;{struct Cyc_CfFlowInfo_Place _tmp66;struct Cyc_List_List*
_tmp67;struct Cyc_CfFlowInfo_UniquePlace _tmp65=*p1;_tmp66=_tmp65.place;_tmp67=
_tmp65.path;{struct Cyc_CfFlowInfo_Place _tmp69;struct Cyc_List_List*_tmp6A;struct
Cyc_CfFlowInfo_UniquePlace _tmp68=*p2;_tmp69=_tmp68.place;_tmp6A=_tmp68.path;{int
i=Cyc_CfFlowInfo_place_cmp(& _tmp66,& _tmp69);if(i != 0)return i;return((int(*)(int(*
cmp)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_CfFlowInfo_intlist_cmp,_tmp67,_tmp6A);}}}}static
struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p);
static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*
p){struct Cyc_List_List*sl=0;void*_tmp6C;struct Cyc_List_List*_tmp6D;struct Cyc_CfFlowInfo_Place
_tmp6B=*p;_tmp6C=_tmp6B.root;_tmp6D=_tmp6B.fields;{void*_tmp6E=_tmp6C;struct Cyc_Absyn_Vardecl*
_tmp6F;struct Cyc_Absyn_Exp*_tmp70;int _tmp71;_LL21: if(*((int*)_tmp6E)!= 0)goto
_LL23;_tmp6F=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp6E)->f1;_LL22:{struct
_dyneither_ptr*_tmp44D;const char*_tmp44C;void*_tmp44B[1];struct Cyc_String_pa_struct
_tmp44A;struct Cyc_List_List*_tmp449;sl=((_tmp449=_cycalloc(sizeof(*_tmp449)),((
_tmp449->hd=((_tmp44D=_cycalloc(sizeof(*_tmp44D)),((_tmp44D[0]=((_tmp44A.tag=0,((
_tmp44A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6F->name).f2),((
_tmp44B[0]=& _tmp44A,Cyc_aprintf(((_tmp44C="%s",_tag_dyneither(_tmp44C,sizeof(
char),3))),_tag_dyneither(_tmp44B,sizeof(void*),1)))))))),_tmp44D)))),((_tmp449->tl=
sl,_tmp449))))));}goto _LL20;_LL23: if(*((int*)_tmp6E)!= 1)goto _LL25;_tmp70=((
struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp6E)->f1;_LL24:{struct _dyneither_ptr*
_tmp45A;const char*_tmp459;void*_tmp458[1];struct Cyc_Int_pa_struct _tmp457;struct
Cyc_List_List*_tmp456;sl=((_tmp456=_cycalloc(sizeof(*_tmp456)),((_tmp456->hd=((
_tmp45A=_cycalloc(sizeof(*_tmp45A)),((_tmp45A[0]=((_tmp457.tag=1,((_tmp457.f1=(
unsigned long)((int)_tmp70),((_tmp458[0]=& _tmp457,Cyc_aprintf(((_tmp459="mpt%d",
_tag_dyneither(_tmp459,sizeof(char),6))),_tag_dyneither(_tmp458,sizeof(void*),1)))))))),
_tmp45A)))),((_tmp456->tl=sl,_tmp456))))));}goto _LL20;_LL25: if(*((int*)_tmp6E)!= 
2)goto _LL20;_tmp71=((struct Cyc_CfFlowInfo_InitParam_struct*)_tmp6E)->f1;_LL26:{
struct _dyneither_ptr*_tmp467;const char*_tmp466;void*_tmp465[1];struct Cyc_Int_pa_struct
_tmp464;struct Cyc_List_List*_tmp463;sl=((_tmp463=_cycalloc(sizeof(*_tmp463)),((
_tmp463->hd=((_tmp467=_cycalloc(sizeof(*_tmp467)),((_tmp467[0]=((_tmp464.tag=1,((
_tmp464.f1=(unsigned long)_tmp71,((_tmp465[0]=& _tmp464,Cyc_aprintf(((_tmp466="param%d",
_tag_dyneither(_tmp466,sizeof(char),8))),_tag_dyneither(_tmp465,sizeof(void*),1)))))))),
_tmp467)))),((_tmp463->tl=sl,_tmp463))))));}goto _LL20;_LL20:;}for(0;_tmp6D != 0;
_tmp6D=_tmp6D->tl){struct _dyneither_ptr*_tmp474;const char*_tmp473;void*_tmp472[1];
struct Cyc_Int_pa_struct _tmp471;struct Cyc_List_List*_tmp470;sl=((_tmp470=
_cycalloc(sizeof(*_tmp470)),((_tmp470->hd=((_tmp474=_cycalloc(sizeof(*_tmp474)),((
_tmp474[0]=((_tmp471.tag=1,((_tmp471.f1=(unsigned long)((int)_tmp6D->hd),((
_tmp472[0]=& _tmp471,Cyc_aprintf(((_tmp473="%d",_tag_dyneither(_tmp473,sizeof(
char),3))),_tag_dyneither(_tmp472,sizeof(void*),1)))))))),_tmp474)))),((_tmp470->tl=
sl,_tmp470))))));}{struct Cyc_String_pa_struct _tmp47F;void*_tmp47E[1];const char*
_tmp47D;const char*_tmp47C;struct _dyneither_ptr*_tmp47B;struct _dyneither_ptr*
_tmp86=(_tmp47B=_cycalloc(sizeof(*_tmp47B)),((_tmp47B[0]=((_tmp47F.tag=0,((
_tmp47F.f1=(struct _dyneither_ptr)((_tmp47C="",_tag_dyneither(_tmp47C,sizeof(char),
1))),((_tmp47E[0]=& _tmp47F,Cyc_aprintf(((_tmp47D="%s",_tag_dyneither(_tmp47D,
sizeof(char),3))),_tag_dyneither(_tmp47E,sizeof(void*),1)))))))),_tmp47B)));for(
0;sl != 0;sl=sl->tl){const char*_tmp484;void*_tmp483[2];struct Cyc_String_pa_struct
_tmp482;struct Cyc_String_pa_struct _tmp481;*_tmp86=((_tmp481.tag=0,((_tmp481.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp86),((_tmp482.tag=0,((_tmp482.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((
_tmp483[0]=& _tmp482,((_tmp483[1]=& _tmp481,Cyc_aprintf(((_tmp484="%s.%s",
_tag_dyneither(_tmp484,sizeof(char),6))),_tag_dyneither(_tmp483,sizeof(void*),2))))))))))))));}
return _tmp86;}}static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*
fenv,int allow_zeroterm,void*t,void*leafval);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,void*leafval);static
void _tmp48A(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp489,unsigned int*
_tmp488,void***_tmp486){for(*_tmp489=0;*_tmp489 < *_tmp488;(*_tmp489)++){(*
_tmp486)[*_tmp489]=(*fenv)->unknown_all;}}struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,void*leafval){
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
unsigned int _tmp489;unsigned int _tmp488;struct _dyneither_ptr _tmp487;void**
_tmp486;unsigned int _tmp485;struct _dyneither_ptr d=(_tmp485=sz,((_tmp486=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp485)),((_tmp487=
_tag_dyneither(_tmp486,sizeof(void*),_tmp485),((((_tmp488=_tmp485,_tmp48A(& fenv,&
_tmp489,& _tmp488,& _tmp486))),_tmp487)))))));{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield _tmp91;struct _dyneither_ptr*_tmp92;void*_tmp93;struct
Cyc_Absyn_Aggrfield*_tmp90=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)
_check_null(fs))->hd;_tmp91=*_tmp90;_tmp92=_tmp91.name;_tmp93=_tmp91.type;if(
_get_dyneither_size(*_tmp92,sizeof(char))!= 1)*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,_tmp93,leafval);}}return d;}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;
};static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
int allow_zeroterm,void*t,void*leafval);static void _tmp490(struct Cyc_CfFlowInfo_FlowEnv**
fenv,unsigned int*_tmp48F,unsigned int*_tmp48E,void***_tmp48C){for(*_tmp48F=0;*
_tmp48F < *_tmp48E;(*_tmp48F)++){(*_tmp48C)[*_tmp48F]=(*fenv)->unknown_all;}}
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int
allow_zeroterm,void*t,void*leafval){if(Cyc_Absyn_is_nontagged_union_type(t))
return fenv->unknown_all;{void*_tmp98=Cyc_Tcutil_compress(t);struct Cyc_Absyn_DatatypeFieldInfo
_tmp99;union Cyc_Absyn_DatatypeFieldInfoU _tmp9A;struct _tuple2 _tmp9B;struct Cyc_Absyn_Datatypefield*
_tmp9C;struct Cyc_List_List*_tmp9D;struct Cyc_Absyn_AggrInfo _tmp9E;union Cyc_Absyn_AggrInfoU
_tmp9F;void*_tmpA0;struct Cyc_List_List*_tmpA1;struct Cyc_Absyn_ArrayInfo _tmpA2;
void*_tmpA3;union Cyc_Absyn_Constraint*_tmpA4;void*_tmpA5;struct Cyc_Absyn_PtrInfo
_tmpA6;struct Cyc_Absyn_PtrAtts _tmpA7;union Cyc_Absyn_Constraint*_tmpA8;_LL28: if(
_tmp98 <= (void*)4)goto _LL36;if(*((int*)_tmp98)!= 3)goto _LL2A;_tmp99=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp98)->f1;_tmp9A=_tmp99.field_info;if((_tmp9A.KnownDatatypefield).tag != 2)goto
_LL2A;_tmp9B=(struct _tuple2)(_tmp9A.KnownDatatypefield).val;_tmp9C=_tmp9B.f2;
_LL29: if(_tmp9C->typs == 0)return leafval;_tmp9D=_tmp9C->typs;goto _LL2B;_LL2A: if(*((
int*)_tmp98)!= 9)goto _LL2C;_tmp9D=((struct Cyc_Absyn_TupleType_struct*)_tmp98)->f1;
_LL2B: {unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp9D);unsigned int _tmp48F;unsigned int _tmp48E;struct _dyneither_ptr _tmp48D;void**
_tmp48C;unsigned int _tmp48B;struct _dyneither_ptr d=(_tmp48B=sz,((_tmp48C=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp48B)),((_tmp48D=
_tag_dyneither(_tmp48C,sizeof(void*),_tmp48B),((((_tmp48E=_tmp48B,_tmp490(& fenv,&
_tmp48F,& _tmp48E,& _tmp48C))),_tmp48D)))))));{int i=0;for(0;i < sz;++ i){*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmp9D))->hd)).f2,
leafval);_tmp9D=_tmp9D->tl;}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp493;
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp492;return(void*)((_tmp492=
_region_malloc(fenv->r,sizeof(*_tmp492)),((_tmp492[0]=((_tmp493.tag=4,((_tmp493.f1=
0,((_tmp493.f2=d,_tmp493)))))),_tmp492))));}}_LL2C: if(*((int*)_tmp98)!= 10)goto
_LL2E;_tmp9E=((struct Cyc_Absyn_AggrType_struct*)_tmp98)->f1;_tmp9F=_tmp9E.aggr_info;
_LL2D: {struct Cyc_Absyn_Aggrdecl*_tmpAF=Cyc_Absyn_get_known_aggrdecl(_tmp9F);if(
_tmpAF->impl == 0)goto _LL27;_tmpA0=_tmpAF->kind;_tmpA1=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpAF->impl))->fields;goto _LL2F;}_LL2E: if(*((int*)_tmp98)!= 11)goto
_LL30;_tmpA0=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp98)->f1;_tmpA1=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp98)->f2;_LL2F: {struct Cyc_CfFlowInfo_Aggregate_struct
_tmp496;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp495;return(void*)((_tmp495=
_region_malloc(fenv->r,sizeof(*_tmp495)),((_tmp495[0]=((_tmp496.tag=4,((_tmp496.f1=
_tmpA0 == (void*)1,((_tmp496.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmpA1,
leafval),_tmp496)))))),_tmp495))));}_LL30: if(*((int*)_tmp98)!= 7)goto _LL32;
_tmpA2=((struct Cyc_Absyn_ArrayType_struct*)_tmp98)->f1;_tmpA3=_tmpA2.elt_type;
_tmpA4=_tmpA2.zero_term;if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmpA4))goto _LL32;_LL31: return allow_zeroterm  && Cyc_Tcutil_bits_only(_tmpA3)?
fenv->unknown_all: leafval;_LL32: if(*((int*)_tmp98)!= 18)goto _LL34;_tmpA5=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp98)->f1;_LL33: {void*_tmpB2=leafval;void*
_tmpB3;void*_tmpB4;_LL39: if(_tmpB2 <= (void*)3)goto _LL3D;if(*((int*)_tmpB2)!= 0)
goto _LL3B;_tmpB3=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpB2)->f1;if((
int)_tmpB3 != 2)goto _LL3B;_LL3A: goto _LL3C;_LL3B: if(*((int*)_tmpB2)!= 1)goto _LL3D;
_tmpB4=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmpB2)->f1;if((int)_tmpB4 != 2)
goto _LL3D;_LL3C: goto _LL3E;_LL3D: if((int)_tmpB2 != 0)goto _LL3F;_LL3E: goto _LL40;
_LL3F: if((int)_tmpB2 != 1)goto _LL41;_LL40: {struct Cyc_CfFlowInfo_TagCmps_struct
_tmp4A0;struct Cyc_CfFlowInfo_TagCmp*_tmp49F;struct Cyc_CfFlowInfo_TagCmp*_tmp49E[
1];struct Cyc_CfFlowInfo_TagCmps_struct*_tmp49D;return(void*)((_tmp49D=
_region_malloc(fenv->r,sizeof(*_tmp49D)),((_tmp49D[0]=((_tmp4A0.tag=3,((_tmp4A0.f1=((
_tmp49E[0]=((_tmp49F=_region_malloc(fenv->r,sizeof(*_tmp49F)),((_tmp49F->cmp=(
void*)5,((_tmp49F->bd=_tmpA5,_tmp49F)))))),((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(fenv->r,_tag_dyneither(
_tmp49E,sizeof(struct Cyc_CfFlowInfo_TagCmp*),1)))),_tmp4A0)))),_tmp49D))));}
_LL41:;_LL42: return leafval;_LL38:;}_LL34: if(*((int*)_tmp98)!= 4)goto _LL36;_tmpA6=((
struct Cyc_Absyn_PointerType_struct*)_tmp98)->f1;_tmpA7=_tmpA6.ptr_atts;_tmpA8=
_tmpA7.nullable;if(!(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmpA8)))goto _LL36;_LL35:{void*_tmpB9=leafval;void*_tmpBA;void*_tmpBB;_LL44:
if(_tmpB9 <= (void*)3)goto _LL48;if(*((int*)_tmpB9)!= 0)goto _LL46;_tmpBA=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpB9)->f1;if((int)_tmpBA != 1)goto _LL46;
_LL45: return(void*)2;_LL46: if(*((int*)_tmpB9)!= 0)goto _LL48;_tmpBB=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpB9)->f1;if((int)_tmpBB != 2)goto _LL48;
_LL47: return(void*)1;_LL48:;_LL49: goto _LL43;_LL43:;}goto _LL27;_LL36:;_LL37: goto
_LL27;_LL27:;}return Cyc_Tcutil_bits_only(t)?fenv->unknown_all: leafval;}void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,void*leafval);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,void*leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,1,t,leafval);}struct _tuple20{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*
f2;};static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,
struct Cyc_Dict_Dict set);static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Dict_Dict set){struct _RegionHandle _tmpBC=_new_region("r");struct
_RegionHandle*r=& _tmpBC;_push_region(r);{struct _tuple20 _tmp4A1;struct _tuple20
_tmpBD=(_tmp4A1.f1=place,((_tmp4A1.f2=0,_tmp4A1)));struct Cyc_Iter_Iter _tmpBE=((
struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
r,set);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple20*))Cyc_Iter_next)(_tmpBE,&
_tmpBD)){struct Cyc_CfFlowInfo_Place*_tmpBF=_tmpBD.f1;void*_tmpC1;struct Cyc_List_List*
_tmpC2;struct Cyc_CfFlowInfo_Place _tmpC0=*place;_tmpC1=_tmpC0.root;_tmpC2=_tmpC0.fields;{
void*_tmpC4;struct Cyc_List_List*_tmpC5;struct Cyc_CfFlowInfo_Place _tmpC3=*_tmpBF;
_tmpC4=_tmpC3.root;_tmpC5=_tmpC3.fields;if(Cyc_CfFlowInfo_root_cmp(_tmpC1,_tmpC4)
!= 0)continue;for(0;_tmpC2 != 0  && _tmpC5 != 0;(_tmpC2=_tmpC2->tl,_tmpC5=_tmpC5->tl)){
if((int)_tmpC2->hd != (int)_tmpC5->hd)break;}if(_tmpC2 == 0){int _tmpC6=1;
_npop_handler(0);return _tmpC6;}}}{int _tmpC7=0;_npop_handler(0);return _tmpC7;}};
_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*
places;};static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,
struct Cyc_CfFlowInfo_Place*place);static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*
pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place)){struct Cyc_List_List*
_tmp4A2;pile->places=((_tmp4A2=_region_malloc(pile->rgn,sizeof(*_tmp4A2)),((
_tmp4A2->hd=place,((_tmp4A2->tl=pile->places,_tmp4A2))))));}}static void Cyc_CfFlowInfo_add_places(
struct Cyc_CfFlowInfo_EscPile*pile,void*r);static void Cyc_CfFlowInfo_add_places(
struct Cyc_CfFlowInfo_EscPile*pile,void*r){void*_tmpCA=r;struct Cyc_CfFlowInfo_Place*
_tmpCB;int _tmpCC;struct _dyneither_ptr _tmpCD;_LL4B: if(_tmpCA <= (void*)3)goto _LL4F;
if(*((int*)_tmpCA)!= 2)goto _LL4D;_tmpCB=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpCA)->f1;_LL4C: Cyc_CfFlowInfo_add_place(pile,_tmpCB);return;_LL4D: if(*((int*)
_tmpCA)!= 4)goto _LL4F;_tmpCC=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpCA)->f1;
_tmpCD=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpCA)->f2;_LL4E:{int i=0;for(0;
i < _get_dyneither_size(_tmpCD,sizeof(void*));++ i){Cyc_CfFlowInfo_add_places(pile,((
void**)_tmpCD.curr)[i]);}}return;_LL4F:;_LL50: return;_LL4A:;}static void*Cyc_CfFlowInfo_insert_place_inner(
struct _RegionHandle*r,void*new_val,void*old_val);static void _tmp4A8(struct
_RegionHandle**r,void**new_val,struct _dyneither_ptr*_tmpD0,unsigned int*_tmp4A7,
unsigned int*_tmp4A6,void***_tmp4A4){for(*_tmp4A7=0;*_tmp4A7 < *_tmp4A6;(*_tmp4A7)
++){(*_tmp4A4)[*_tmp4A7]=Cyc_CfFlowInfo_insert_place_inner(*r,*new_val,((void**)(*
_tmpD0).curr)[(int)*_tmp4A7]);}}static void*Cyc_CfFlowInfo_insert_place_inner(
struct _RegionHandle*r,void*new_val,void*old_val){void*_tmpCE=old_val;int _tmpCF;
struct _dyneither_ptr _tmpD0;_LL52: if(_tmpCE <= (void*)3)goto _LL54;if(*((int*)
_tmpCE)!= 4)goto _LL54;_tmpCF=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpCE)->f1;
_tmpD0=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpCE)->f2;_LL53: {unsigned int
_tmp4A7;unsigned int _tmp4A6;struct _dyneither_ptr _tmp4A5;void**_tmp4A4;
unsigned int _tmp4A3;struct _dyneither_ptr d2=(_tmp4A3=_get_dyneither_size(_tmpD0,
sizeof(void*)),((_tmp4A4=(void**)_region_malloc(r,_check_times(sizeof(void*),
_tmp4A3)),((_tmp4A5=_tag_dyneither(_tmp4A4,sizeof(void*),_tmp4A3),((((_tmp4A6=
_tmp4A3,_tmp4A8(& r,& new_val,& _tmpD0,& _tmp4A7,& _tmp4A6,& _tmp4A4))),_tmp4A5)))))));
struct Cyc_CfFlowInfo_Aggregate_struct _tmp4AB;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp4AA;return(void*)((_tmp4AA=_region_malloc(r,sizeof(*_tmp4AA)),((_tmp4AA[0]=((
_tmp4AB.tag=4,((_tmp4AB.f1=_tmpCF,((_tmp4AB.f2=d2,_tmp4AB)))))),_tmp4AA))));}
_LL54:;_LL55: return new_val;_LL51:;}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(
struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval);static void _tmp4B1(
struct _dyneither_ptr*d,unsigned int*_tmp4B0,unsigned int*_tmp4AF,void***_tmp4AD){
for(*_tmp4B0=0;*_tmp4B0 < *_tmp4AF;(*_tmp4B0)++){(*_tmp4AD)[*_tmp4B0]=((void**)(*
d).curr)[(int)*_tmp4B0];}}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(
struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){void*_tmpD7=*((void**)
_check_dyneither_subscript(d,sizeof(void*),n));if(_tmpD7 == rval)return d;{
unsigned int _tmp4B0;unsigned int _tmp4AF;struct _dyneither_ptr _tmp4AE;void**
_tmp4AD;unsigned int _tmp4AC;struct _dyneither_ptr res=(_tmp4AC=_get_dyneither_size(
d,sizeof(void*)),((_tmp4AD=(void**)_region_malloc(r,_check_times(sizeof(void*),
_tmp4AC)),((_tmp4AE=_tag_dyneither(_tmp4AD,sizeof(void*),_tmp4AC),((((_tmp4AF=
_tmp4AC,_tmp4B1(& d,& _tmp4B0,& _tmp4AF,& _tmp4AD))),_tmp4AE)))))));*((void**)
_check_dyneither_subscript(res,sizeof(void*),n))=rval;return res;}}struct _tuple21{
struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val);static
void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*
fs,void*old_val,void*new_val){if(fs == 0)return Cyc_CfFlowInfo_insert_place_inner(
r,new_val,old_val);{struct _tuple21 _tmp4B2;struct _tuple21 _tmpDD=(_tmp4B2.f1=fs,((
_tmp4B2.f2=old_val,_tmp4B2)));struct Cyc_List_List*_tmpDE;struct Cyc_List_List
_tmpDF;int _tmpE0;struct Cyc_List_List*_tmpE1;void*_tmpE2;int _tmpE3;struct
_dyneither_ptr _tmpE4;_LL57: _tmpDE=_tmpDD.f1;if(_tmpDE == 0)goto _LL59;_tmpDF=*
_tmpDE;_tmpE0=(int)_tmpDF.hd;_tmpE1=_tmpDF.tl;_tmpE2=_tmpDD.f2;if(_tmpE2 <= (void*)
3)goto _LL59;if(*((int*)_tmpE2)!= 4)goto _LL59;_tmpE3=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpE2)->f1;_tmpE4=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpE2)->f2;_LL58: {
void*_tmpE5=Cyc_CfFlowInfo_insert_place_outer(r,_tmpE1,*((void**)
_check_dyneither_subscript(_tmpE4,sizeof(void*),_tmpE0)),new_val);struct
_dyneither_ptr _tmpE6=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmpE4,_tmpE0,_tmpE5);if(
_tmpE6.curr == _tmpE4.curr)return old_val;else{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp4B5;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp4B4;return(void*)((_tmp4B4=
_region_malloc(r,sizeof(*_tmp4B4)),((_tmp4B4[0]=((_tmp4B5.tag=4,((_tmp4B5.f1=
_tmpE3,((_tmp4B5.f2=_tmpE6,_tmp4B5)))))),_tmp4B4))));}}_LL59:;_LL5A: {struct Cyc_Core_Impossible_struct
_tmp4BB;const char*_tmp4BA;struct Cyc_Core_Impossible_struct*_tmp4B9;(int)_throw((
void*)((_tmp4B9=_cycalloc(sizeof(*_tmp4B9)),((_tmp4B9[0]=((_tmp4BB.tag=Cyc_Core_Impossible,((
_tmp4BB.f1=((_tmp4BA="bad insert place",_tag_dyneither(_tmp4BA,sizeof(char),17))),
_tmp4BB)))),_tmp4B9)))));}_LL56:;}}static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*
all_changed,struct Cyc_Dict_Dict d);static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*
all_changed,struct Cyc_Dict_Dict d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*
_tmpEC=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;((int(*)(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_update_place_set)(all_changed,_tmpEC,0);{void*oldval;void*
newval;{struct _handler_cons _tmpED;_push_handler(& _tmpED);{int _tmpEF=0;if(setjmp(
_tmpED.handler))_tmpEF=1;if(!_tmpEF){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmpEC);;
_pop_handler();}else{void*_tmpEE=(void*)_exn_thrown;void*_tmpF1=_tmpEE;_LL5C: if(
_tmpF1 != Cyc_Dict_Absent)goto _LL5E;_LL5D: continue;_LL5E:;_LL5F:(void)_throw(
_tmpF1);_LL5B:;}}}{void*_tmpF2=Cyc_CfFlowInfo_initlevel(fenv,d,oldval);_LL61: if((
int)_tmpF2 != 2)goto _LL63;_LL62: newval=fenv->esc_all;goto _LL60;_LL63: if((int)
_tmpF2 != 1)goto _LL65;_LL64: newval=fenv->esc_this;goto _LL60;_LL65: if((int)_tmpF2
!= 0)goto _LL60;_LL66: newval=fenv->esc_none;goto _LL60;_LL60:;}Cyc_CfFlowInfo_add_places(
pile,oldval);{void*_tmpF4;struct Cyc_List_List*_tmpF5;struct Cyc_CfFlowInfo_Place
_tmpF3=*_tmpEC;_tmpF4=_tmpF3.root;_tmpF5=_tmpF3.fields;d=Cyc_Dict_insert(d,
_tmpF4,Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmpF5,Cyc_Dict_lookup(d,_tmpF4),
newval));}}}return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;
struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};static void*Cyc_CfFlowInfo_initlevel_approx(
void*r);static void*Cyc_CfFlowInfo_initlevel_approx(void*r){void*_tmpF6=r;void*
_tmpF7;void*_tmpF8;_LL68: if(_tmpF6 <= (void*)3)goto _LL6C;if(*((int*)_tmpF6)!= 0)
goto _LL6A;_tmpF7=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpF6)->f1;
_LL69: return _tmpF7;_LL6A: if(*((int*)_tmpF6)!= 1)goto _LL6C;_tmpF8=(void*)((struct
Cyc_CfFlowInfo_Esc_struct*)_tmpF6)->f1;_LL6B: return _tmpF8;_LL6C: if((int)_tmpF6 != 
0)goto _LL6E;_LL6D: goto _LL6F;_LL6E: if((int)_tmpF6 != 1)goto _LL70;_LL6F: return(void*)
2;_LL70: if((int)_tmpF6 != 2)goto _LL72;_LL71: return(void*)1;_LL72: if(_tmpF6 <= (
void*)3)goto _LL74;if(*((int*)_tmpF6)!= 3)goto _LL74;_LL73: return(void*)2;_LL74:;
_LL75: {struct Cyc_Core_Impossible_struct _tmp4C1;const char*_tmp4C0;struct Cyc_Core_Impossible_struct*
_tmp4BF;(int)_throw((void*)((_tmp4BF=_cycalloc(sizeof(*_tmp4BF)),((_tmp4BF[0]=((
_tmp4C1.tag=Cyc_Core_Impossible,((_tmp4C1.f1=((_tmp4C0="initlevel_approx",
_tag_dyneither(_tmp4C0,sizeof(char),17))),_tmp4C1)))),_tmp4BF)))));}_LL67:;}
static void*Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,
void*r,void*acc);static void*Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*
env,void*r,void*acc){void*this_ans;if(acc == (void*)0)return(void*)0;{void*_tmpFC=
r;int _tmpFD;struct _dyneither_ptr _tmpFE;int _tmpFF;struct _dyneither_ptr _tmp100;
struct Cyc_CfFlowInfo_Place*_tmp101;_LL77: if(_tmpFC <= (void*)3)goto _LL7D;if(*((
int*)_tmpFC)!= 4)goto _LL79;_tmpFD=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpFC)->f1;if(_tmpFD != 1)goto _LL79;_tmpFE=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpFC)->f2;_LL78: {unsigned int _tmp102=_get_dyneither_size(_tmpFE,sizeof(void*));
this_ans=(void*)0;{int i=0;for(0;i < _tmp102;++ i){if(Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmpFE,sizeof(void*),i)),(void*)2)== (
void*)2){this_ans=(void*)2;break;}}}goto _LL76;}_LL79: if(*((int*)_tmpFC)!= 4)goto
_LL7B;_tmpFF=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpFC)->f1;if(_tmpFF != 0)
goto _LL7B;_tmp100=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpFC)->f2;_LL7A: {
unsigned int _tmp103=_get_dyneither_size(_tmp100,sizeof(void*));this_ans=(void*)2;{
int i=0;for(0;i < _tmp103;++ i){this_ans=Cyc_CfFlowInfo_initlevel_rec(env,*((void**)
_check_dyneither_subscript(_tmp100,sizeof(void*),i)),this_ans);}}goto _LL76;}
_LL7B: if(*((int*)_tmpFC)!= 2)goto _LL7D;_tmp101=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpFC)->f1;_LL7C: if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct
Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(
Cyc_CfFlowInfo_place_cmp,env->seen,_tmp101))this_ans=(void*)2;else{{struct Cyc_List_List*
_tmp4C2;env->seen=((_tmp4C2=_region_malloc(env->e,sizeof(*_tmp4C2)),((_tmp4C2->hd=
_tmp101,((_tmp4C2->tl=env->seen,_tmp4C2))))));}this_ans=Cyc_CfFlowInfo_initlevel_rec(
env,Cyc_CfFlowInfo_lookup_place(env->d,_tmp101),(void*)2);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == (void*)0)this_ans=(void*)1;}goto _LL76;
_LL7D:;_LL7E: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL76:;}if(this_ans == (
void*)0)return(void*)0;if(this_ans == (void*)1  || acc == (void*)1)return(void*)1;
return(void*)2;}void*Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,
struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*
env,struct Cyc_Dict_Dict d,void*r){struct _RegionHandle*_tmp105=env->r;struct Cyc_CfFlowInfo_InitlevelEnv
_tmp4C3;struct Cyc_CfFlowInfo_InitlevelEnv _tmp106=(_tmp4C3.e=_tmp105,((_tmp4C3.d=
d,((_tmp4C3.seen=0,_tmp4C3)))));return Cyc_CfFlowInfo_initlevel_rec(& _tmp106,r,(
void*)2);}void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place){void*_tmp109;struct Cyc_List_List*_tmp10A;struct Cyc_CfFlowInfo_Place
_tmp108=*place;_tmp109=_tmp108.root;_tmp10A=_tmp108.fields;{void*_tmp10B=Cyc_Dict_lookup(
d,_tmp109);for(0;_tmp10A != 0;_tmp10A=_tmp10A->tl){struct _tuple6 _tmp4C4;struct
_tuple6 _tmp10D=(_tmp4C4.f1=_tmp10B,((_tmp4C4.f2=(int)_tmp10A->hd,_tmp4C4)));void*
_tmp10E;int _tmp10F;struct _dyneither_ptr _tmp110;int _tmp111;_LL80: _tmp10E=_tmp10D.f1;
if(_tmp10E <= (void*)3)goto _LL82;if(*((int*)_tmp10E)!= 4)goto _LL82;_tmp10F=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp10E)->f1;_tmp110=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp10E)->f2;_tmp111=_tmp10D.f2;_LL81: _tmp10B=*((void**)
_check_dyneither_subscript(_tmp110,sizeof(void*),_tmp111));goto _LL7F;_LL82:;
_LL83: {struct Cyc_Core_Impossible_struct _tmp4CA;const char*_tmp4C9;struct Cyc_Core_Impossible_struct*
_tmp4C8;(int)_throw((void*)((_tmp4C8=_cycalloc(sizeof(*_tmp4C8)),((_tmp4C8[0]=((
_tmp4CA.tag=Cyc_Core_Impossible,((_tmp4CA.f1=((_tmp4C9="bad lookup_place",
_tag_dyneither(_tmp4C9,sizeof(char),17))),_tmp4CA)))),_tmp4C8)))));}_LL7F:;}
return _tmp10B;}}static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval);static int
Cyc_CfFlowInfo_is_rval_unescaped(void*rval){void*_tmp115=rval;int _tmp116;struct
_dyneither_ptr _tmp117;_LL85: if(_tmp115 <= (void*)3)goto _LL89;if(*((int*)_tmp115)
!= 1)goto _LL87;_LL86: return 0;_LL87: if(*((int*)_tmp115)!= 4)goto _LL89;_tmp116=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp115)->f1;_tmp117=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp115)->f2;_LL88: {unsigned int sz=_get_dyneither_size(_tmp117,sizeof(void*));{
int i=0;for(0;i < sz;++ i){if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp117.curr)[
i]))return 0;}}return 1;}_LL89:;_LL8A: return 1;_LL84:;}int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){return Cyc_CfFlowInfo_is_rval_unescaped(
Cyc_CfFlowInfo_lookup_place(d,place));}struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,void*r);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){struct
_RegionHandle*_tmp118=fenv->r;struct Cyc_CfFlowInfo_EscPile*_tmp4CB;struct Cyc_CfFlowInfo_EscPile*
pile=(_tmp4CB=_region_malloc(_tmp118,sizeof(*_tmp4CB)),((_tmp4CB->rgn=_tmp118,((
_tmp4CB->places=0,_tmp4CB)))));Cyc_CfFlowInfo_add_places(pile,r);return Cyc_CfFlowInfo_escape_these(
fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*
fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;struct Cyc_Position_Segment*
loc;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*
env,void*oldval,void*newval);static void _tmp4D5(struct Cyc_CfFlowInfo_AssignEnv**
env,unsigned int*_tmp4D4,unsigned int*_tmp4D3,void***_tmp4D1,struct
_dyneither_ptr*_tmp125,struct _dyneither_ptr*_tmp122){for(*_tmp4D4=0;*_tmp4D4 < *
_tmp4D3;(*_tmp4D4)++){(*_tmp4D1)[*_tmp4D4]=Cyc_CfFlowInfo_assign_place_inner(*
env,((void**)(*_tmp122).curr)[(int)*_tmp4D4],*((void**)
_check_dyneither_subscript(*_tmp125,sizeof(void*),(int)*_tmp4D4)));}}static void*
Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,
void*newval){struct _tuple0 _tmp4CC;struct _tuple0 _tmp11B=(_tmp4CC.f1=oldval,((
_tmp4CC.f2=newval,_tmp4CC)));void*_tmp11C;void*_tmp11D;struct Cyc_CfFlowInfo_Place*
_tmp11E;void*_tmp11F;void*_tmp120;int _tmp121;struct _dyneither_ptr _tmp122;void*
_tmp123;int _tmp124;struct _dyneither_ptr _tmp125;void*_tmp126;void*_tmp127;_LL8C:
_tmp11C=_tmp11B.f1;if(_tmp11C <= (void*)3)goto _LL8E;if(*((int*)_tmp11C)!= 1)goto
_LL8E;_tmp11D=_tmp11B.f2;if(_tmp11D <= (void*)3)goto _LL8E;if(*((int*)_tmp11D)!= 2)
goto _LL8E;_tmp11E=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp11D)->f1;_LL8D:
Cyc_CfFlowInfo_add_place(env->pile,_tmp11E);goto _LL8F;_LL8E: _tmp11F=_tmp11B.f1;
if(_tmp11F <= (void*)3)goto _LL90;if(*((int*)_tmp11F)!= 1)goto _LL90;_LL8F: if(Cyc_CfFlowInfo_initlevel(
env->fenv,env->d,newval)!= (void*)2){const char*_tmp4CF;void*_tmp4CE;(_tmp4CE=0,
Cyc_Tcutil_terr(env->loc,((_tmp4CF="assignment puts possibly-uninitialized data in an escaped location",
_tag_dyneither(_tmp4CF,sizeof(char),67))),_tag_dyneither(_tmp4CE,sizeof(void*),0)));}
return(env->fenv)->esc_all;_LL90: _tmp120=_tmp11B.f1;if(_tmp120 <= (void*)3)goto
_LL92;if(*((int*)_tmp120)!= 4)goto _LL92;_tmp121=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp120)->f1;_tmp122=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp120)->f2;
_tmp123=_tmp11B.f2;if(_tmp123 <= (void*)3)goto _LL92;if(*((int*)_tmp123)!= 4)goto
_LL92;_tmp124=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp123)->f1;_tmp125=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp123)->f2;_LL91: {unsigned int _tmp4D4;
unsigned int _tmp4D3;struct _dyneither_ptr _tmp4D2;void**_tmp4D1;unsigned int
_tmp4D0;struct _dyneither_ptr new_d=(_tmp4D0=_get_dyneither_size(_tmp122,sizeof(
void*)),((_tmp4D1=(void**)_region_malloc((env->fenv)->r,_check_times(sizeof(void*),
_tmp4D0)),((_tmp4D2=_tag_dyneither(_tmp4D1,sizeof(void*),_tmp4D0),((((_tmp4D3=
_tmp4D0,_tmp4D5(& env,& _tmp4D4,& _tmp4D3,& _tmp4D1,& _tmp125,& _tmp122))),_tmp4D2)))))));
int change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp122,sizeof(void*));++ i){if(*((
void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp122.curr)[
i]){change=1;break;}}}if(!change)return oldval;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp122,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp125,sizeof(void*),i))){change=1;break;}}}if(!
change)return newval;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp4D8;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp4D7;return(void*)((_tmp4D7=_region_malloc((env->fenv)->r,sizeof(*_tmp4D7)),((
_tmp4D7[0]=((_tmp4D8.tag=4,((_tmp4D8.f1=_tmp121,((_tmp4D8.f2=new_d,_tmp4D8)))))),
_tmp4D7))));}}_LL92: _tmp126=_tmp11B.f2;if(_tmp126 <= (void*)3)goto _LL94;if(*((int*)
_tmp126)!= 1)goto _LL94;_tmp127=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp126)->f1;
_LL93: {void*_tmp130=_tmp127;_LL97: if((int)_tmp130 != 0)goto _LL99;_LL98: return(
env->fenv)->unknown_none;_LL99: if((int)_tmp130 != 1)goto _LL9B;_LL9A: return(env->fenv)->unknown_this;
_LL9B: if((int)_tmp130 != 2)goto _LL96;_LL9C: return(env->fenv)->unknown_all;_LL96:;}
_LL94:;_LL95: return newval;_LL8B:;}static void*Cyc_CfFlowInfo_assign_place_outer(
struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,void*oldval,void*
newval);static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*
env,struct Cyc_List_List*fs,void*oldval,void*newval){if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(
env,oldval,newval);{struct _tuple21 _tmp4D9;struct _tuple21 _tmp132=(_tmp4D9.f1=fs,((
_tmp4D9.f2=oldval,_tmp4D9)));struct Cyc_List_List*_tmp133;struct Cyc_List_List
_tmp134;int _tmp135;struct Cyc_List_List*_tmp136;void*_tmp137;int _tmp138;struct
_dyneither_ptr _tmp139;_LL9E: _tmp133=_tmp132.f1;if(_tmp133 == 0)goto _LLA0;_tmp134=*
_tmp133;_tmp135=(int)_tmp134.hd;_tmp136=_tmp134.tl;_tmp137=_tmp132.f2;if(_tmp137
<= (void*)3)goto _LLA0;if(*((int*)_tmp137)!= 4)goto _LLA0;_tmp138=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp137)->f1;_tmp139=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp137)->f2;_LL9F: {
void*_tmp13A=Cyc_CfFlowInfo_assign_place_outer(env,_tmp136,*((void**)
_check_dyneither_subscript(_tmp139,sizeof(void*),_tmp135)),newval);struct
_dyneither_ptr _tmp13B=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp139,
_tmp135,_tmp13A);if(_tmp13B.curr == _tmp139.curr)return oldval;else{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp4DC;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp4DB;return(void*)((_tmp4DB=
_region_malloc((env->fenv)->r,sizeof(*_tmp4DB)),((_tmp4DB[0]=((_tmp4DC.tag=4,((
_tmp4DC.f1=_tmp138,((_tmp4DC.f2=_tmp13B,_tmp4DC)))))),_tmp4DB))));}}_LLA0:;_LLA1: {
struct Cyc_Core_Impossible_struct _tmp4E2;const char*_tmp4E1;struct Cyc_Core_Impossible_struct*
_tmp4E0;(int)_throw((void*)((_tmp4E0=_cycalloc(sizeof(*_tmp4E0)),((_tmp4E0[0]=((
_tmp4E2.tag=Cyc_Core_Impossible,((_tmp4E2.f1=((_tmp4E1="bad assign place",
_tag_dyneither(_tmp4E1,sizeof(char),17))),_tmp4E2)))),_tmp4E0)))));}_LL9D:;}}
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){((int(*)(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_update_place_set)(all_changed,place,0);{struct _RegionHandle*
_tmp141=fenv->r;struct Cyc_CfFlowInfo_Place _tmp143;void*_tmp144;struct Cyc_List_List*
_tmp145;struct Cyc_CfFlowInfo_Place*_tmp142=place;_tmp143=*_tmp142;_tmp144=
_tmp143.root;_tmp145=_tmp143.fields;{struct Cyc_CfFlowInfo_EscPile*_tmp4E5;struct
Cyc_CfFlowInfo_AssignEnv _tmp4E4;struct Cyc_CfFlowInfo_AssignEnv env=(_tmp4E4.fenv=
fenv,((_tmp4E4.pile=((_tmp4E5=_region_malloc(_tmp141,sizeof(*_tmp4E5)),((_tmp4E5->rgn=
_tmp141,((_tmp4E5->places=0,_tmp4E5)))))),((_tmp4E4.d=d,((_tmp4E4.loc=loc,
_tmp4E4)))))));void*newval=Cyc_CfFlowInfo_assign_place_outer(& env,_tmp145,Cyc_Dict_lookup(
d,_tmp144),r);return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,Cyc_Dict_insert(
d,_tmp144,newval));}}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*
fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict
d2;};enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One
 = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv
joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*
curr_place;struct Cyc_List_List**last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged
changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,
void*r2);static struct Cyc_List_List*Cyc_CfFlowInfo_join_tag_cmps(struct
_RegionHandle*r,struct Cyc_List_List*l1,struct Cyc_List_List*l2);static struct Cyc_List_List*
Cyc_CfFlowInfo_join_tag_cmps(struct _RegionHandle*r,struct Cyc_List_List*l1,struct
Cyc_List_List*l2){if(l1 == l2)return l1;{struct Cyc_List_List*_tmp148=0;for(0;l2 != 
0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp _tmp14A;void*_tmp14B;void*_tmp14C;struct
Cyc_CfFlowInfo_TagCmp*_tmp149=(struct Cyc_CfFlowInfo_TagCmp*)l2->hd;_tmp14A=*
_tmp149;_tmp14B=_tmp14A.cmp;_tmp14C=_tmp14A.bd;{int found=0;void*joined_cmp=(void*)
10;{struct Cyc_List_List*_tmp14D=l1;for(0;_tmp14D != 0;_tmp14D=_tmp14D->tl){struct
Cyc_CfFlowInfo_TagCmp _tmp14F;void*_tmp150;void*_tmp151;struct Cyc_CfFlowInfo_TagCmp*
_tmp14E=(struct Cyc_CfFlowInfo_TagCmp*)_tmp14D->hd;_tmp14F=*_tmp14E;_tmp150=
_tmp14F.cmp;_tmp151=_tmp14F.bd;if(Cyc_Tcutil_typecmp(_tmp14C,_tmp151)== 0){found=
1;if(_tmp150 == _tmp14B){joined_cmp=_tmp150;break;}}}}if(found){struct Cyc_CfFlowInfo_TagCmp*
_tmp4E8;struct Cyc_List_List*_tmp4E7;_tmp148=((_tmp4E7=_region_malloc(r,sizeof(*
_tmp4E7)),((_tmp4E7->hd=((_tmp4E8=_region_malloc(r,sizeof(*_tmp4E8)),((_tmp4E8->cmp=
joined_cmp,((_tmp4E8->bd=_tmp14C,_tmp4E8)))))),((_tmp4E7->tl=_tmp148,_tmp4E7))))));}}}
return _tmp148;}}static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*
env,void*a,void*r1,void*r2);static void _tmp4EF(struct Cyc_CfFlowInfo_JoinEnv**env,
unsigned int*_tmp4EE,unsigned int*_tmp4ED,void***_tmp4EB,struct _dyneither_ptr*
_tmp168,struct _dyneither_ptr*_tmp165){for(*_tmp4EE=0;*_tmp4EE < *_tmp4ED;(*
_tmp4EE)++){(*_tmp4EB)[*_tmp4EE]=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int
a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp165).curr)[(
int)*_tmp4EE],*((void**)_check_dyneither_subscript(*_tmp168,sizeof(void*),(int)*
_tmp4EE)));}}static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*
env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{struct _RegionHandle*_tmp154=(
env->fenv)->r;{struct _tuple0 _tmp4E9;struct _tuple0 _tmp156=(_tmp4E9.f1=r1,((
_tmp4E9.f2=r2,_tmp4E9)));void*_tmp157;struct Cyc_CfFlowInfo_Place*_tmp158;void*
_tmp159;struct Cyc_CfFlowInfo_Place*_tmp15A;void*_tmp15B;struct Cyc_CfFlowInfo_Place*
_tmp15C;void*_tmp15D;struct Cyc_CfFlowInfo_Place*_tmp15E;void*_tmp15F;void*
_tmp160;void*_tmp161;void*_tmp162;void*_tmp163;int _tmp164;struct _dyneither_ptr
_tmp165;void*_tmp166;int _tmp167;struct _dyneither_ptr _tmp168;void*_tmp169;struct
Cyc_List_List*_tmp16A;void*_tmp16B;struct Cyc_List_List*_tmp16C;void*_tmp16D;void*
_tmp16E;_LLA3: _tmp157=_tmp156.f1;if(_tmp157 <= (void*)3)goto _LLA5;if(*((int*)
_tmp157)!= 2)goto _LLA5;_tmp158=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp157)->f1;
_tmp159=_tmp156.f2;if(_tmp159 <= (void*)3)goto _LLA5;if(*((int*)_tmp159)!= 2)goto
_LLA5;_tmp15A=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp159)->f1;_LLA4: if(Cyc_CfFlowInfo_place_cmp(
_tmp158,_tmp15A)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmp158);Cyc_CfFlowInfo_add_place(
env->pile,_tmp15A);goto _LLA2;_LLA5: _tmp15B=_tmp156.f1;if(_tmp15B <= (void*)3)goto
_LLA7;if(*((int*)_tmp15B)!= 2)goto _LLA7;_tmp15C=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp15B)->f1;_LLA6: Cyc_CfFlowInfo_add_place(env->pile,_tmp15C);goto _LLA2;_LLA7:
_tmp15D=_tmp156.f2;if(_tmp15D <= (void*)3)goto _LLA9;if(*((int*)_tmp15D)!= 2)goto
_LLA9;_tmp15E=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp15D)->f1;_LLA8: Cyc_CfFlowInfo_add_place(
env->pile,_tmp15E);goto _LLA2;_LLA9: _tmp15F=_tmp156.f1;if((int)_tmp15F != 1)goto
_LLAB;_tmp160=_tmp156.f2;if((int)_tmp160 != 2)goto _LLAB;_LLAA: goto _LLAC;_LLAB:
_tmp161=_tmp156.f1;if((int)_tmp161 != 2)goto _LLAD;_tmp162=_tmp156.f2;if((int)
_tmp162 != 1)goto _LLAD;_LLAC: return(void*)2;_LLAD: _tmp163=_tmp156.f1;if(_tmp163 <= (
void*)3)goto _LLAF;if(*((int*)_tmp163)!= 4)goto _LLAF;_tmp164=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp163)->f1;_tmp165=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp163)->f2;
_tmp166=_tmp156.f2;if(_tmp166 <= (void*)3)goto _LLAF;if(*((int*)_tmp166)!= 4)goto
_LLAF;_tmp167=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp166)->f1;_tmp168=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp166)->f2;_LLAE: {unsigned int _tmp4EE;
unsigned int _tmp4ED;struct _dyneither_ptr _tmp4EC;void**_tmp4EB;unsigned int
_tmp4EA;struct _dyneither_ptr new_d=(_tmp4EA=_get_dyneither_size(_tmp165,sizeof(
void*)),((_tmp4EB=(void**)_region_malloc(_tmp154,_check_times(sizeof(void*),
_tmp4EA)),((_tmp4EC=_tag_dyneither(_tmp4EB,sizeof(void*),_tmp4EA),((((_tmp4ED=
_tmp4EA,_tmp4EF(& env,& _tmp4EE,& _tmp4ED,& _tmp4EB,& _tmp168,& _tmp165))),_tmp4EC)))))));
int change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp165,sizeof(void*));++ i){if(*((
void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp165.curr)[
i]){change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp165,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp168,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp4F2;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp4F1;return(void*)((_tmp4F1=_region_malloc(_tmp154,sizeof(*_tmp4F1)),((
_tmp4F1[0]=((_tmp4F2.tag=4,((_tmp4F2.f1=_tmp164,((_tmp4F2.f2=new_d,_tmp4F2)))))),
_tmp4F1))));}}_LLAF: _tmp169=_tmp156.f1;if(_tmp169 <= (void*)3)goto _LLB1;if(*((int*)
_tmp169)!= 3)goto _LLB1;_tmp16A=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp169)->f1;
_tmp16B=_tmp156.f2;if(_tmp16B <= (void*)3)goto _LLB1;if(*((int*)_tmp16B)!= 3)goto
_LLB1;_tmp16C=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp16B)->f1;_LLB0: {struct
Cyc_List_List*_tmp175=Cyc_CfFlowInfo_join_tag_cmps(_tmp154,_tmp16A,_tmp16C);if(
_tmp175 == _tmp16A)return r1;{struct Cyc_CfFlowInfo_TagCmps_struct _tmp4F5;struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp4F4;return(void*)((_tmp4F4=_region_malloc(_tmp154,sizeof(*_tmp4F4)),((
_tmp4F4[0]=((_tmp4F5.tag=3,((_tmp4F5.f1=_tmp175,_tmp4F5)))),_tmp4F4))));}}_LLB1:
_tmp16D=_tmp156.f1;if(_tmp16D <= (void*)3)goto _LLB3;if(*((int*)_tmp16D)!= 3)goto
_LLB3;_LLB2: return r2;_LLB3: _tmp16E=_tmp156.f2;if(_tmp16E <= (void*)3)goto _LLB5;
if(*((int*)_tmp16E)!= 3)goto _LLB5;_LLB4: return r1;_LLB5:;_LLB6: goto _LLA2;_LLA2:;}{
void*il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);void*il2=Cyc_CfFlowInfo_initlevel(
env->fenv,env->d2,r2);struct _tuple0 _tmp4F6;struct _tuple0 _tmp179=(_tmp4F6.f1=r1,((
_tmp4F6.f2=r2,_tmp4F6)));void*_tmp17A;void*_tmp17B;_LLB8: _tmp17A=_tmp179.f1;if(
_tmp17A <= (void*)3)goto _LLBA;if(*((int*)_tmp17A)!= 1)goto _LLBA;_LLB9: goto _LLBB;
_LLBA: _tmp17B=_tmp179.f2;if(_tmp17B <= (void*)3)goto _LLBC;if(*((int*)_tmp17B)!= 1)
goto _LLBC;_LLBB: {struct _tuple0 _tmp4F7;struct _tuple0 _tmp17D=(_tmp4F7.f1=il1,((
_tmp4F7.f2=il2,_tmp4F7)));void*_tmp17E;void*_tmp17F;void*_tmp180;void*_tmp181;
_LLBF: _tmp17E=_tmp17D.f2;if((int)_tmp17E != 0)goto _LLC1;_LLC0: goto _LLC2;_LLC1:
_tmp17F=_tmp17D.f1;if((int)_tmp17F != 0)goto _LLC3;_LLC2: return(env->fenv)->esc_none;
_LLC3: _tmp180=_tmp17D.f2;if((int)_tmp180 != 1)goto _LLC5;_LLC4: goto _LLC6;_LLC5:
_tmp181=_tmp17D.f1;if((int)_tmp181 != 1)goto _LLC7;_LLC6: return(env->fenv)->esc_this;
_LLC7:;_LLC8: return(env->fenv)->esc_all;_LLBE:;}_LLBC:;_LLBD: {struct _tuple0
_tmp4F8;struct _tuple0 _tmp183=(_tmp4F8.f1=il1,((_tmp4F8.f2=il2,_tmp4F8)));void*
_tmp184;void*_tmp185;void*_tmp186;void*_tmp187;_LLCA: _tmp184=_tmp183.f2;if((int)
_tmp184 != 0)goto _LLCC;_LLCB: goto _LLCD;_LLCC: _tmp185=_tmp183.f1;if((int)_tmp185 != 
0)goto _LLCE;_LLCD: return(env->fenv)->unknown_none;_LLCE: _tmp186=_tmp183.f2;if((
int)_tmp186 != 1)goto _LLD0;_LLCF: goto _LLD1;_LLD0: _tmp187=_tmp183.f1;if((int)
_tmp187 != 1)goto _LLD2;_LLD1: return(env->fenv)->unknown_this;_LLD2:;_LLD3: return(
env->fenv)->unknown_all;_LLC9:;}_LLB7:;}}}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(
unsigned int i);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(unsigned int i){
union Cyc_CfFlowInfo_RelnOp _tmp4F9;return((_tmp4F9.EqualConst).val=i,(((_tmp4F9.EqualConst).tag=
1,_tmp4F9)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*
v,void*t);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*
v,void*t){struct _tuple11 _tmp4FC;union Cyc_CfFlowInfo_RelnOp _tmp4FB;return((
_tmp4FB.LessVar).val=((_tmp4FC.f1=v,((_tmp4FC.f2=t,_tmp4FC)))),(((_tmp4FB.LessVar).tag=
2,_tmp4FB)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*
v);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*
v){union Cyc_CfFlowInfo_RelnOp _tmp4FD;return((_tmp4FD.LessNumelts).val=v,(((
_tmp4FD.LessNumelts).tag=3,_tmp4FD)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(
unsigned int i);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(unsigned int i){
union Cyc_CfFlowInfo_RelnOp _tmp4FE;return((_tmp4FE.LessConst).val=i,(((_tmp4FE.LessConst).tag=
4,_tmp4FE)));}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*
v);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*
v){union Cyc_CfFlowInfo_RelnOp _tmp4FF;return((_tmp4FF.LessEqNumelts).val=v,(((
_tmp4FF.LessEqNumelts).tag=5,_tmp4FF)));}struct _tuple22{union Cyc_CfFlowInfo_RelnOp
f1;union Cyc_CfFlowInfo_RelnOp f2;};static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2);static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2){struct _tuple22 _tmp500;struct _tuple22 _tmp18F=(
_tmp500.f1=r1,((_tmp500.f2=r2,_tmp500)));union Cyc_CfFlowInfo_RelnOp _tmp190;
unsigned int _tmp191;union Cyc_CfFlowInfo_RelnOp _tmp192;unsigned int _tmp193;union
Cyc_CfFlowInfo_RelnOp _tmp194;struct _tuple11 _tmp195;struct Cyc_Absyn_Vardecl*
_tmp196;union Cyc_CfFlowInfo_RelnOp _tmp197;struct _tuple11 _tmp198;struct Cyc_Absyn_Vardecl*
_tmp199;union Cyc_CfFlowInfo_RelnOp _tmp19A;struct Cyc_Absyn_Vardecl*_tmp19B;union
Cyc_CfFlowInfo_RelnOp _tmp19C;struct Cyc_Absyn_Vardecl*_tmp19D;union Cyc_CfFlowInfo_RelnOp
_tmp19E;unsigned int _tmp19F;union Cyc_CfFlowInfo_RelnOp _tmp1A0;unsigned int
_tmp1A1;union Cyc_CfFlowInfo_RelnOp _tmp1A2;struct Cyc_Absyn_Vardecl*_tmp1A3;union
Cyc_CfFlowInfo_RelnOp _tmp1A4;struct Cyc_Absyn_Vardecl*_tmp1A5;_LLD5: _tmp190=
_tmp18F.f1;if((_tmp190.EqualConst).tag != 1)goto _LLD7;_tmp191=(unsigned int)(
_tmp190.EqualConst).val;_tmp192=_tmp18F.f2;if((_tmp192.EqualConst).tag != 1)goto
_LLD7;_tmp193=(unsigned int)(_tmp192.EqualConst).val;_LLD6: return _tmp191 == 
_tmp193;_LLD7: _tmp194=_tmp18F.f1;if((_tmp194.LessVar).tag != 2)goto _LLD9;_tmp195=(
struct _tuple11)(_tmp194.LessVar).val;_tmp196=_tmp195.f1;_tmp197=_tmp18F.f2;if((
_tmp197.LessVar).tag != 2)goto _LLD9;_tmp198=(struct _tuple11)(_tmp197.LessVar).val;
_tmp199=_tmp198.f1;_LLD8: return _tmp196 == _tmp199;_LLD9: _tmp19A=_tmp18F.f1;if((
_tmp19A.LessNumelts).tag != 3)goto _LLDB;_tmp19B=(struct Cyc_Absyn_Vardecl*)(
_tmp19A.LessNumelts).val;_tmp19C=_tmp18F.f2;if((_tmp19C.LessNumelts).tag != 3)
goto _LLDB;_tmp19D=(struct Cyc_Absyn_Vardecl*)(_tmp19C.LessNumelts).val;_LLDA:
return _tmp19B == _tmp19D;_LLDB: _tmp19E=_tmp18F.f1;if((_tmp19E.LessConst).tag != 4)
goto _LLDD;_tmp19F=(unsigned int)(_tmp19E.LessConst).val;_tmp1A0=_tmp18F.f2;if((
_tmp1A0.LessConst).tag != 4)goto _LLDD;_tmp1A1=(unsigned int)(_tmp1A0.LessConst).val;
_LLDC: return _tmp19F == _tmp1A1;_LLDD: _tmp1A2=_tmp18F.f1;if((_tmp1A2.LessEqNumelts).tag
!= 5)goto _LLDF;_tmp1A3=(struct Cyc_Absyn_Vardecl*)(_tmp1A2.LessEqNumelts).val;
_tmp1A4=_tmp18F.f2;if((_tmp1A4.LessEqNumelts).tag != 5)goto _LLDF;_tmp1A5=(struct
Cyc_Absyn_Vardecl*)(_tmp1A4.LessEqNumelts).val;_LLDE: return _tmp1A3 == _tmp1A5;
_LLDF:;_LLE0: return 0;_LLD4:;}static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(
struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s);static
struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct _RegionHandle*r,struct Cyc_List_List*
r1s,struct Cyc_List_List*r2s){if(r1s == r2s)return r1s;{struct Cyc_List_List*res=0;
int diff=0;{struct Cyc_List_List*_tmp1A6=r1s;for(0;_tmp1A6 != 0;_tmp1A6=_tmp1A6->tl){
struct Cyc_CfFlowInfo_Reln*_tmp1A7=(struct Cyc_CfFlowInfo_Reln*)_tmp1A6->hd;int
found=0;{struct Cyc_List_List*_tmp1A8=r2s;for(0;_tmp1A8 != 0;_tmp1A8=_tmp1A8->tl){
struct Cyc_CfFlowInfo_Reln*_tmp1A9=(struct Cyc_CfFlowInfo_Reln*)_tmp1A8->hd;if(
_tmp1A7 == _tmp1A9  || _tmp1A7->vd == _tmp1A9->vd  && Cyc_CfFlowInfo_same_relop(
_tmp1A7->rop,_tmp1A9->rop)){{struct Cyc_List_List*_tmp501;res=((_tmp501=
_region_malloc(r,sizeof(*_tmp501)),((_tmp501->hd=_tmp1A7,((_tmp501->tl=res,
_tmp501))))));}found=1;break;}}}if(!found)diff=1;}}if(!diff)res=r1s;return res;}}
static void Cyc_CfFlowInfo_remove_f(struct Cyc_List_List**l,struct Cyc_CfFlowInfo_Place*
place);static void Cyc_CfFlowInfo_remove_f(struct Cyc_List_List**l,struct Cyc_CfFlowInfo_Place*
place){struct _handler_cons _tmp1AB;_push_handler(& _tmp1AB);{int _tmp1AD=0;if(
setjmp(_tmp1AB.handler))_tmp1AD=1;if(!_tmp1AD){*l=((struct Cyc_List_List*(*)(int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_delete_cmp)(Cyc_CfFlowInfo_place_cmp,*l,
place);;_pop_handler();}else{void*_tmp1AC=(void*)_exn_thrown;void*_tmp1AF=
_tmp1AC;_LLE2: if(_tmp1AF != Cyc_Core_Not_found)goto _LLE4;_LLE3: goto _LLE1;_LLE4:;
_LLE5:(void)_throw(_tmp1AF);_LLE1:;}}}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_or_consume(
struct _RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2);static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_or_consume(struct
_RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*mc=c2.may_consume;struct _RegionHandle*_tmp1B0=r;{struct
Cyc_List_List*_tmp1B1=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rcopy)(_tmp1B0,c1.may_consume);((void(*)(void(*f)(struct Cyc_List_List**,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List**env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_CfFlowInfo_remove_f,& _tmp1B1,c2.may_consume);while(_tmp1B1 != 0){{struct Cyc_List_List*
_tmp502;mc=((_tmp502=_region_malloc(r,sizeof(*_tmp502)),((_tmp502->hd=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmp1B1))->hd,((_tmp502->tl=mc,_tmp502))))));}
_tmp1B1=((struct Cyc_List_List*)_check_null(_tmp1B1))->tl;}}{struct Cyc_CfFlowInfo_ConsumeInfo
_tmp503;return(_tmp503.consumed=Cyc_CfFlowInfo_union_place_set(c1.consumed,c2.consumed,
0),((_tmp503.may_consume=mc,_tmp503)));}}struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(
struct _RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2);struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(struct
_RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*_tmp1B4=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,c1.may_consume,
c2.may_consume);struct Cyc_CfFlowInfo_ConsumeInfo _tmp504;return(_tmp504.consumed=
Cyc_CfFlowInfo_union_place_set(c1.consumed,c2.consumed,1),((_tmp504.may_consume=
_tmp1B4,_tmp504)));}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo
f2;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2,int or_consumed);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct
Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo
f1,union Cyc_CfFlowInfo_FlowInfo f2,int or_consumed){struct _tuple23 _tmp505;struct
_tuple23 _tmp1B7=(_tmp505.f1=f1,((_tmp505.f2=f2,_tmp505)));union Cyc_CfFlowInfo_FlowInfo
_tmp1B8;int _tmp1B9;union Cyc_CfFlowInfo_FlowInfo _tmp1BA;int _tmp1BB;union Cyc_CfFlowInfo_FlowInfo
_tmp1BC;struct _tuple12 _tmp1BD;struct Cyc_Dict_Dict _tmp1BE;struct Cyc_List_List*
_tmp1BF;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1C0;union Cyc_CfFlowInfo_FlowInfo
_tmp1C1;struct _tuple12 _tmp1C2;struct Cyc_Dict_Dict _tmp1C3;struct Cyc_List_List*
_tmp1C4;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1C5;_LLE7: _tmp1B8=_tmp1B7.f1;if((
_tmp1B8.BottomFL).tag != 1)goto _LLE9;_tmp1B9=(int)(_tmp1B8.BottomFL).val;_LLE8:
return f2;_LLE9: _tmp1BA=_tmp1B7.f2;if((_tmp1BA.BottomFL).tag != 1)goto _LLEB;
_tmp1BB=(int)(_tmp1BA.BottomFL).val;_LLEA: return f1;_LLEB: _tmp1BC=_tmp1B7.f1;if((
_tmp1BC.ReachableFL).tag != 2)goto _LLE6;_tmp1BD=(struct _tuple12)(_tmp1BC.ReachableFL).val;
_tmp1BE=_tmp1BD.f1;_tmp1BF=_tmp1BD.f2;_tmp1C0=_tmp1BD.f3;_tmp1C1=_tmp1B7.f2;if((
_tmp1C1.ReachableFL).tag != 2)goto _LLE6;_tmp1C2=(struct _tuple12)(_tmp1C1.ReachableFL).val;
_tmp1C3=_tmp1C2.f1;_tmp1C4=_tmp1C2.f2;_tmp1C5=_tmp1C2.f3;_LLEC: if(((_tmp1BE.t == 
_tmp1C3.t  && _tmp1BF == _tmp1C4) && _tmp1C0.may_consume == _tmp1C5.may_consume)
 && (_tmp1C0.consumed).t == (_tmp1C5.consumed).t)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
f1,f2))return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle*_tmp1C6=fenv->r;struct Cyc_CfFlowInfo_EscPile*_tmp508;struct Cyc_CfFlowInfo_JoinEnv
_tmp507;struct Cyc_CfFlowInfo_JoinEnv _tmp1C7=(_tmp507.fenv=fenv,((_tmp507.pile=((
_tmp508=_region_malloc(_tmp1C6,sizeof(*_tmp508)),((_tmp508->rgn=_tmp1C6,((
_tmp508->places=0,_tmp508)))))),((_tmp507.d1=_tmp1BE,((_tmp507.d2=_tmp1C3,
_tmp507)))))));struct Cyc_Dict_Dict _tmp1C8=((struct Cyc_Dict_Dict(*)(void*(*f)(
struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_absRval,&
_tmp1C7,_tmp1BE,_tmp1C3);struct Cyc_List_List*_tmp1C9=Cyc_CfFlowInfo_join_relns(
fenv->r,_tmp1BF,_tmp1C4);struct Cyc_CfFlowInfo_ConsumeInfo _tmp1CA=or_consumed?Cyc_CfFlowInfo_or_consume(
fenv->r,_tmp1C0,_tmp1C5): Cyc_CfFlowInfo_and_consume(fenv->r,_tmp1C0,_tmp1C5);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp1C7.pile,
all_changed,_tmp1C8),_tmp1C9,_tmp1CA);}_LLE6:;}struct _tuple24{union Cyc_CfFlowInfo_FlowInfo
f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};struct
_tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2,int
or_consumed);struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2,int
or_consumed){union Cyc_CfFlowInfo_FlowInfo _tmp1CE;void*_tmp1CF;struct _tuple13
_tmp1CD=pr1;_tmp1CE=_tmp1CD.f1;_tmp1CF=_tmp1CD.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp1D1;void*_tmp1D2;struct _tuple13 _tmp1D0=pr2;_tmp1D1=_tmp1D0.f1;_tmp1D2=
_tmp1D0.f2;{union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,
all_changed,_tmp1CE,_tmp1D1,or_consumed);struct _tuple24 _tmp509;struct _tuple24
_tmp1D4=(_tmp509.f1=_tmp1CE,((_tmp509.f2=_tmp1D1,((_tmp509.f3=outflow,_tmp509)))));
union Cyc_CfFlowInfo_FlowInfo _tmp1D5;int _tmp1D6;union Cyc_CfFlowInfo_FlowInfo
_tmp1D7;int _tmp1D8;union Cyc_CfFlowInfo_FlowInfo _tmp1D9;struct _tuple12 _tmp1DA;
struct Cyc_Dict_Dict _tmp1DB;union Cyc_CfFlowInfo_FlowInfo _tmp1DC;struct _tuple12
_tmp1DD;struct Cyc_Dict_Dict _tmp1DE;union Cyc_CfFlowInfo_FlowInfo _tmp1DF;struct
_tuple12 _tmp1E0;struct Cyc_Dict_Dict _tmp1E1;struct Cyc_List_List*_tmp1E2;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1E3;_LLEE: _tmp1D5=_tmp1D4.f1;if((_tmp1D5.BottomFL).tag != 1)goto _LLF0;_tmp1D6=(
int)(_tmp1D5.BottomFL).val;_LLEF: {struct _tuple13 _tmp50A;return(_tmp50A.f1=
outflow,((_tmp50A.f2=_tmp1D2,_tmp50A)));}_LLF0: _tmp1D7=_tmp1D4.f2;if((_tmp1D7.BottomFL).tag
!= 1)goto _LLF2;_tmp1D8=(int)(_tmp1D7.BottomFL).val;_LLF1: {struct _tuple13 _tmp50B;
return(_tmp50B.f1=outflow,((_tmp50B.f2=_tmp1CF,_tmp50B)));}_LLF2: _tmp1D9=_tmp1D4.f1;
if((_tmp1D9.ReachableFL).tag != 2)goto _LLF4;_tmp1DA=(struct _tuple12)(_tmp1D9.ReachableFL).val;
_tmp1DB=_tmp1DA.f1;_tmp1DC=_tmp1D4.f2;if((_tmp1DC.ReachableFL).tag != 2)goto _LLF4;
_tmp1DD=(struct _tuple12)(_tmp1DC.ReachableFL).val;_tmp1DE=_tmp1DD.f1;_tmp1DF=
_tmp1D4.f3;if((_tmp1DF.ReachableFL).tag != 2)goto _LLF4;_tmp1E0=(struct _tuple12)(
_tmp1DF.ReachableFL).val;_tmp1E1=_tmp1E0.f1;_tmp1E2=_tmp1E0.f2;_tmp1E3=_tmp1E0.f3;
_LLF3: if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(
0,_tmp1CF,_tmp1D2)){struct _tuple13 _tmp50C;return(_tmp50C.f1=outflow,((_tmp50C.f2=
_tmp1D2,_tmp50C)));}if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(
0,_tmp1D2,_tmp1CF)){struct _tuple13 _tmp50D;return(_tmp50D.f1=outflow,((_tmp50D.f2=
_tmp1CF,_tmp50D)));}{struct _RegionHandle*_tmp1E8=fenv->r;struct Cyc_CfFlowInfo_EscPile*
_tmp510;struct Cyc_CfFlowInfo_JoinEnv _tmp50F;struct Cyc_CfFlowInfo_JoinEnv _tmp1E9=(
_tmp50F.fenv=fenv,((_tmp50F.pile=((_tmp510=_region_malloc(_tmp1E8,sizeof(*
_tmp510)),((_tmp510->rgn=_tmp1E8,((_tmp510->places=0,_tmp510)))))),((_tmp50F.d1=
_tmp1DB,((_tmp50F.d2=_tmp1DE,_tmp50F)))))));void*_tmp1EA=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp1E9,0,_tmp1CF,
_tmp1D2);struct _tuple13 _tmp511;return(_tmp511.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,_tmp1E9.pile,all_changed,_tmp1E1),_tmp1E2,_tmp1E3),((_tmp511.f2=_tmp1EA,
_tmp511)));}_LLF4:;_LLF5: {struct Cyc_Core_Impossible_struct _tmp517;const char*
_tmp516;struct Cyc_Core_Impossible_struct*_tmp515;(int)_throw((void*)((_tmp515=
_cycalloc(sizeof(*_tmp515)),((_tmp515[0]=((_tmp517.tag=Cyc_Core_Impossible,((
_tmp517.f1=((_tmp516="join_flow_and_rval: BottomFL outflow",_tag_dyneither(
_tmp516,sizeof(char),37))),_tmp517)))),_tmp515)))));}_LLED:;}}}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp51E(struct Cyc_CfFlowInfo_AfterEnv**
env,unsigned int*_tmp51D,unsigned int*_tmp51C,void***_tmp51A,struct
_dyneither_ptr*_tmp1F8,struct _dyneither_ptr*_tmp1F5){for(*_tmp51D=0;*_tmp51D < *
_tmp51C;(*_tmp51D)++){(*_tmp51A)[*_tmp51D]=Cyc_CfFlowInfo_after_absRval_d(*env,(
int)*_tmp51D,((void**)(*_tmp1F5).curr)[(int)*_tmp51D],*((void**)
_check_dyneither_subscript(*_tmp1F8,sizeof(void*),(int)*_tmp51D)));}}static void*
Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);int
changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);if(
changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*
r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);if(changed1){if(
!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))return r1;env->changed=
Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,
env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{struct _tuple0 _tmp518;
struct _tuple0 _tmp1F2=(_tmp518.f1=r1,((_tmp518.f2=r2,_tmp518)));void*_tmp1F3;int
_tmp1F4;struct _dyneither_ptr _tmp1F5;void*_tmp1F6;int _tmp1F7;struct _dyneither_ptr
_tmp1F8;_LLF7: _tmp1F3=_tmp1F2.f1;if(_tmp1F3 <= (void*)3)goto _LLF9;if(*((int*)
_tmp1F3)!= 4)goto _LLF9;_tmp1F4=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1F3)->f1;
_tmp1F5=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1F3)->f2;_tmp1F6=_tmp1F2.f2;
if(_tmp1F6 <= (void*)3)goto _LLF9;if(*((int*)_tmp1F6)!= 4)goto _LLF9;_tmp1F7=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1F6)->f1;_tmp1F8=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1F6)->f2;_LLF8: {unsigned int _tmp51D;unsigned int _tmp51C;struct
_dyneither_ptr _tmp51B;void**_tmp51A;unsigned int _tmp519;struct _dyneither_ptr
new_d=(_tmp519=_get_dyneither_size(_tmp1F5,sizeof(void*)),((_tmp51A=(void**)
_region_malloc(((env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp519)),((
_tmp51B=_tag_dyneither(_tmp51A,sizeof(void*),_tmp519),((((_tmp51C=_tmp519,
_tmp51E(& env,& _tmp51D,& _tmp51C,& _tmp51A,& _tmp1F8,& _tmp1F5))),_tmp51B)))))));int
change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp1F5,sizeof(void*));++ i){if(*((
void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1F5.curr)[
i]){change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp1F5,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp1F8,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp521;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp520;return(void*)((_tmp520=_region_malloc(((env->joinenv).fenv)->r,sizeof(*
_tmp520)),((_tmp520[0]=((_tmp521.tag=4,((_tmp521.f1=_tmp1F4,((_tmp521.f2=new_d,
_tmp521)))))),_tmp520))));}}_LLF9:;_LLFA: {struct Cyc_Core_Impossible_struct
_tmp527;const char*_tmp526;struct Cyc_Core_Impossible_struct*_tmp525;(int)_throw((
void*)((_tmp525=_cycalloc(sizeof(*_tmp525)),((_tmp525[0]=((_tmp527.tag=Cyc_Core_Impossible,((
_tmp527.f1=((_tmp526="after_absRval -- non-aggregates!",_tag_dyneither(_tmp526,
sizeof(char),33))),_tmp527)))),_tmp525)))));}_LLF6:;}}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2);static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp202=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged
_tmp203=env->changed;{struct Cyc_List_List*_tmp528;*env->last_field_cell=((
_tmp528=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp528)),((_tmp528->hd=(
void*)key,((_tmp528->tl=0,_tmp528))))));}env->last_field_cell=(struct Cyc_List_List**)&((
struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{void*_tmp205=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp202;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp203;return _tmp205;}}}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2);static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_CfFlowInfo_Place _tmp529;*env->curr_place=((_tmp529.root=root,((_tmp529.fields=
0,_tmp529))));}env->last_field_cell=(struct Cyc_List_List**)&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*
all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2,struct
Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){struct _tuple23 _tmp52A;struct _tuple23
_tmp208=(_tmp52A.f1=f1,((_tmp52A.f2=f2,_tmp52A)));union Cyc_CfFlowInfo_FlowInfo
_tmp209;int _tmp20A;union Cyc_CfFlowInfo_FlowInfo _tmp20B;int _tmp20C;union Cyc_CfFlowInfo_FlowInfo
_tmp20D;struct _tuple12 _tmp20E;struct Cyc_Dict_Dict _tmp20F;struct Cyc_List_List*
_tmp210;struct Cyc_CfFlowInfo_ConsumeInfo _tmp211;union Cyc_CfFlowInfo_FlowInfo
_tmp212;struct _tuple12 _tmp213;struct Cyc_Dict_Dict _tmp214;struct Cyc_List_List*
_tmp215;struct Cyc_CfFlowInfo_ConsumeInfo _tmp216;_LLFC: _tmp209=_tmp208.f1;if((
_tmp209.BottomFL).tag != 1)goto _LLFE;_tmp20A=(int)(_tmp209.BottomFL).val;_LLFD:
goto _LLFF;_LLFE: _tmp20B=_tmp208.f2;if((_tmp20B.BottomFL).tag != 1)goto _LL100;
_tmp20C=(int)(_tmp20B.BottomFL).val;_LLFF: return Cyc_CfFlowInfo_BottomFL();_LL100:
_tmp20D=_tmp208.f1;if((_tmp20D.ReachableFL).tag != 2)goto _LLFB;_tmp20E=(struct
_tuple12)(_tmp20D.ReachableFL).val;_tmp20F=_tmp20E.f1;_tmp210=_tmp20E.f2;_tmp211=
_tmp20E.f3;_tmp212=_tmp208.f2;if((_tmp212.ReachableFL).tag != 2)goto _LLFB;_tmp213=(
struct _tuple12)(_tmp212.ReachableFL).val;_tmp214=_tmp213.f1;_tmp215=_tmp213.f2;
_tmp216=_tmp213.f3;_LL101: if(((_tmp20F.t == _tmp214.t  && _tmp210 == _tmp215) && 
_tmp211.may_consume == _tmp216.may_consume) && (_tmp211.consumed).t == (_tmp216.consumed).t)
return f1;{struct _RegionHandle*_tmp217=fenv->r;struct Cyc_CfFlowInfo_Place*_tmp218=
fenv->dummy_place;struct Cyc_CfFlowInfo_JoinEnv _tmp530;struct Cyc_CfFlowInfo_EscPile*
_tmp52F;struct Cyc_CfFlowInfo_AfterEnv _tmp52E;struct Cyc_CfFlowInfo_AfterEnv
_tmp219=(_tmp52E.joinenv=((_tmp530.fenv=fenv,((_tmp530.pile=((_tmp52F=
_region_malloc(_tmp217,sizeof(*_tmp52F)),((_tmp52F->rgn=_tmp217,((_tmp52F->places=
0,_tmp52F)))))),((_tmp530.d1=_tmp20F,((_tmp530.d2=_tmp214,_tmp530)))))))),((
_tmp52E.chg1=chg1,((_tmp52E.chg2=chg2,((_tmp52E.curr_place=_tmp218,((_tmp52E.last_field_cell=(
struct Cyc_List_List**)& _tmp218->fields,((_tmp52E.changed=Cyc_CfFlowInfo_Neither,
_tmp52E)))))))))));struct Cyc_Dict_Dict _tmp21A=((struct Cyc_Dict_Dict(*)(void*(*f)(
struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp219,_tmp20F,_tmp214);struct Cyc_List_List*_tmp21B=Cyc_CfFlowInfo_join_relns(
fenv->r,_tmp210,_tmp215);struct Cyc_CfFlowInfo_ConsumeInfo _tmp21C=Cyc_CfFlowInfo_and_consume(
fenv->r,_tmp211,_tmp216);return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,(_tmp219.joinenv).pile,all_changed,_tmp21A),_tmp21B,_tmp21C);}_LLFB:;}
static int Cyc_CfFlowInfo_tag_cmps_lessthan_approx(struct Cyc_List_List*l1,struct
Cyc_List_List*l2);static int Cyc_CfFlowInfo_tag_cmps_lessthan_approx(struct Cyc_List_List*
l1,struct Cyc_List_List*l2){for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp221;void*_tmp222;void*_tmp223;struct Cyc_CfFlowInfo_TagCmp*_tmp220=(struct Cyc_CfFlowInfo_TagCmp*)
l2->hd;_tmp221=*_tmp220;_tmp222=_tmp221.cmp;_tmp223=_tmp221.bd;{struct Cyc_List_List*
_tmp224=l1;for(0;_tmp224 != 0;_tmp224=_tmp224->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp226;void*_tmp227;void*_tmp228;struct Cyc_CfFlowInfo_TagCmp*_tmp225=(struct Cyc_CfFlowInfo_TagCmp*)
_tmp224->hd;_tmp226=*_tmp225;_tmp227=_tmp226.cmp;_tmp228=_tmp226.bd;if(_tmp227 == 
_tmp222  && Cyc_Tcutil_typecmp(_tmp228,_tmp223)== 0)break;}if(_tmp224 == 0)return 0;}}
return 1;}static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,
void*r2);static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,
void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp531;struct _tuple0 _tmp22A=(_tmp531.f1=
r1,((_tmp531.f2=r2,_tmp531)));void*_tmp22B;struct Cyc_CfFlowInfo_Place*_tmp22C;
void*_tmp22D;struct Cyc_CfFlowInfo_Place*_tmp22E;void*_tmp22F;void*_tmp230;void*
_tmp231;int _tmp232;struct _dyneither_ptr _tmp233;void*_tmp234;int _tmp235;struct
_dyneither_ptr _tmp236;void*_tmp237;void*_tmp238;void*_tmp239;void*_tmp23A;void*
_tmp23B;void*_tmp23C;void*_tmp23D;struct Cyc_List_List*_tmp23E;void*_tmp23F;
struct Cyc_List_List*_tmp240;void*_tmp241;_LL103: _tmp22B=_tmp22A.f1;if(_tmp22B <= (
void*)3)goto _LL105;if(*((int*)_tmp22B)!= 2)goto _LL105;_tmp22C=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp22B)->f1;_tmp22D=_tmp22A.f2;if(_tmp22D <= (void*)3)goto _LL105;if(*((int*)
_tmp22D)!= 2)goto _LL105;_tmp22E=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp22D)->f1;
_LL104: return Cyc_CfFlowInfo_place_cmp(_tmp22C,_tmp22E)== 0;_LL105: _tmp22F=
_tmp22A.f1;if(_tmp22F <= (void*)3)goto _LL107;if(*((int*)_tmp22F)!= 2)goto _LL107;
_LL106: goto _LL108;_LL107: _tmp230=_tmp22A.f2;if(_tmp230 <= (void*)3)goto _LL109;if(*((
int*)_tmp230)!= 2)goto _LL109;_LL108: return 0;_LL109: _tmp231=_tmp22A.f1;if(_tmp231
<= (void*)3)goto _LL10B;if(*((int*)_tmp231)!= 4)goto _LL10B;_tmp232=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp231)->f1;_tmp233=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp231)->f2;
_tmp234=_tmp22A.f2;if(_tmp234 <= (void*)3)goto _LL10B;if(*((int*)_tmp234)!= 4)goto
_LL10B;_tmp235=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp234)->f1;_tmp236=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp234)->f2;_LL10A: if(_tmp233.curr == 
_tmp236.curr)return 1;{int i=0;for(0;i < _get_dyneither_size(_tmp233,sizeof(void*));
++ i){if(!((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(
0,((void**)_tmp233.curr)[i],*((void**)_check_dyneither_subscript(_tmp236,sizeof(
void*),i))))return 0;}}return 1;_LL10B: _tmp237=_tmp22A.f2;if((int)_tmp237 != 2)goto
_LL10D;_LL10C: return r1 == (void*)1;_LL10D: _tmp238=_tmp22A.f2;if((int)_tmp238 != 0)
goto _LL10F;_LL10E: goto _LL110;_LL10F: _tmp239=_tmp22A.f2;if((int)_tmp239 != 1)goto
_LL111;_LL110: return 0;_LL111: _tmp23A=_tmp22A.f1;if(_tmp23A <= (void*)3)goto _LL113;
if(*((int*)_tmp23A)!= 1)goto _LL113;_tmp23B=_tmp22A.f2;if(_tmp23B <= (void*)3)goto
_LL113;if(*((int*)_tmp23B)!= 1)goto _LL113;_LL112: goto _LL102;_LL113: _tmp23C=
_tmp22A.f1;if(_tmp23C <= (void*)3)goto _LL115;if(*((int*)_tmp23C)!= 1)goto _LL115;
_LL114: return 0;_LL115: _tmp23D=_tmp22A.f1;if(_tmp23D <= (void*)3)goto _LL117;if(*((
int*)_tmp23D)!= 3)goto _LL117;_tmp23E=((struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp23D)->f1;_tmp23F=_tmp22A.f2;if(_tmp23F <= (void*)3)goto _LL117;if(*((int*)
_tmp23F)!= 3)goto _LL117;_tmp240=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp23F)->f1;
_LL116: return Cyc_CfFlowInfo_tag_cmps_lessthan_approx(_tmp23E,_tmp240);_LL117:
_tmp241=_tmp22A.f2;if(_tmp241 <= (void*)3)goto _LL119;if(*((int*)_tmp241)!= 3)goto
_LL119;_LL118: return 0;_LL119:;_LL11A: goto _LL102;_LL102:;}{struct _tuple0 _tmp532;
struct _tuple0 _tmp243=(_tmp532.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp532.f2=
Cyc_CfFlowInfo_initlevel_approx(r2),_tmp532)));void*_tmp244;void*_tmp245;void*
_tmp246;void*_tmp247;void*_tmp248;void*_tmp249;_LL11C: _tmp244=_tmp243.f1;if((int)
_tmp244 != 2)goto _LL11E;_tmp245=_tmp243.f2;if((int)_tmp245 != 2)goto _LL11E;_LL11D:
return 1;_LL11E: _tmp246=_tmp243.f2;if((int)_tmp246 != 0)goto _LL120;_LL11F: return 1;
_LL120: _tmp247=_tmp243.f1;if((int)_tmp247 != 0)goto _LL122;_LL121: return 0;_LL122:
_tmp248=_tmp243.f2;if((int)_tmp248 != 1)goto _LL124;_LL123: return 1;_LL124: _tmp249=
_tmp243.f1;if((int)_tmp249 != 1)goto _LL11B;_LL125: return 0;_LL11B:;}}static int Cyc_CfFlowInfo_relns_approx(
struct Cyc_List_List*r2s,struct Cyc_List_List*r1s);static int Cyc_CfFlowInfo_relns_approx(
struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s
!= 0;r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp24A=(struct Cyc_CfFlowInfo_Reln*)
r1s->hd;int found=0;{struct Cyc_List_List*_tmp24B=r2s;for(0;_tmp24B != 0;_tmp24B=
_tmp24B->tl){struct Cyc_CfFlowInfo_Reln*_tmp24C=(struct Cyc_CfFlowInfo_Reln*)
_tmp24B->hd;if(_tmp24A == _tmp24C  || _tmp24A->vd == _tmp24C->vd  && Cyc_CfFlowInfo_same_relop(
_tmp24A->rop,_tmp24C->rop)){found=1;break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_consume_approx(
struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);int Cyc_CfFlowInfo_consume_approx(
struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2){if(c1.may_consume
!= c2.may_consume){struct Cyc_List_List*_tmp24D=c1.may_consume;struct Cyc_List_List*
_tmp24E=c2.may_consume;for(0;_tmp24E != 0  && _tmp24D != 0;_tmp24E=_tmp24E->tl){if((
unsigned int)_tmp24D == (unsigned int)_tmp24E)goto compare_consumed_set;{int
_tmp24F=Cyc_CfFlowInfo_place_cmp((struct Cyc_CfFlowInfo_Place*)_tmp24D->hd,(
struct Cyc_CfFlowInfo_Place*)_tmp24E->hd);if(_tmp24F == 0)_tmp24D=_tmp24D->tl;}}
if(_tmp24D != 0)return 0;}compare_consumed_set: return((int(*)(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2))Cyc_CfFlowInfo_place_set_subset)(c1.consumed,c2.consumed);}
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2);int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union
Cyc_CfFlowInfo_FlowInfo f2){struct _tuple23 _tmp533;struct _tuple23 _tmp251=(_tmp533.f1=
f1,((_tmp533.f2=f2,_tmp533)));union Cyc_CfFlowInfo_FlowInfo _tmp252;int _tmp253;
union Cyc_CfFlowInfo_FlowInfo _tmp254;int _tmp255;union Cyc_CfFlowInfo_FlowInfo
_tmp256;struct _tuple12 _tmp257;struct Cyc_Dict_Dict _tmp258;struct Cyc_List_List*
_tmp259;struct Cyc_CfFlowInfo_ConsumeInfo _tmp25A;union Cyc_CfFlowInfo_FlowInfo
_tmp25B;struct _tuple12 _tmp25C;struct Cyc_Dict_Dict _tmp25D;struct Cyc_List_List*
_tmp25E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp25F;_LL127: _tmp252=_tmp251.f1;if((
_tmp252.BottomFL).tag != 1)goto _LL129;_tmp253=(int)(_tmp252.BottomFL).val;_LL128:
return 1;_LL129: _tmp254=_tmp251.f2;if((_tmp254.BottomFL).tag != 1)goto _LL12B;
_tmp255=(int)(_tmp254.BottomFL).val;_LL12A: return 0;_LL12B: _tmp256=_tmp251.f1;if((
_tmp256.ReachableFL).tag != 2)goto _LL126;_tmp257=(struct _tuple12)(_tmp256.ReachableFL).val;
_tmp258=_tmp257.f1;_tmp259=_tmp257.f2;_tmp25A=_tmp257.f3;_tmp25B=_tmp251.f2;if((
_tmp25B.ReachableFL).tag != 2)goto _LL126;_tmp25C=(struct _tuple12)(_tmp25B.ReachableFL).val;
_tmp25D=_tmp25C.f1;_tmp25E=_tmp25C.f2;_tmp25F=_tmp25C.f3;_LL12C: if(((_tmp258.t == 
_tmp25D.t  && _tmp259 == _tmp25E) && _tmp25A.may_consume == _tmp25F.may_consume)
 && (_tmp25A.consumed).t == (_tmp25F.consumed).t)return 1;return(Cyc_Dict_forall_intersect(
Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp258,_tmp25D) && Cyc_CfFlowInfo_relns_approx(
_tmp259,_tmp25E)) && Cyc_CfFlowInfo_consume_approx(_tmp25A,_tmp25F);_LL126:;}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*rgn,struct
Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v);struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(
struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){struct
Cyc_List_List*p;int found=0;for(p=rs;!found  && p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*
_tmp260=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp260->vd == v){found=1;break;}{
union Cyc_CfFlowInfo_RelnOp _tmp261=_tmp260->rop;struct _tuple11 _tmp262;struct Cyc_Absyn_Vardecl*
_tmp263;struct Cyc_Absyn_Vardecl*_tmp264;struct Cyc_Absyn_Vardecl*_tmp265;_LL12E:
if((_tmp261.LessVar).tag != 2)goto _LL130;_tmp262=(struct _tuple11)(_tmp261.LessVar).val;
_tmp263=_tmp262.f1;_LL12F: _tmp264=_tmp263;goto _LL131;_LL130: if((_tmp261.LessNumelts).tag
!= 3)goto _LL132;_tmp264=(struct Cyc_Absyn_Vardecl*)(_tmp261.LessNumelts).val;
_LL131: _tmp265=_tmp264;goto _LL133;_LL132: if((_tmp261.LessEqNumelts).tag != 5)goto
_LL134;_tmp265=(struct Cyc_Absyn_Vardecl*)(_tmp261.LessEqNumelts).val;_LL133: if(v
== _tmp265)found=1;goto _LL12D;_LL134:;_LL135: goto _LL12D;_LL12D:;}}if(!found)
return rs;{struct Cyc_List_List*_tmp266=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*
_tmp267=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp267->vd != v){{union Cyc_CfFlowInfo_RelnOp
_tmp268=_tmp267->rop;struct _tuple11 _tmp269;struct Cyc_Absyn_Vardecl*_tmp26A;
struct Cyc_Absyn_Vardecl*_tmp26B;struct Cyc_Absyn_Vardecl*_tmp26C;_LL137: if((
_tmp268.LessVar).tag != 2)goto _LL139;_tmp269=(struct _tuple11)(_tmp268.LessVar).val;
_tmp26A=_tmp269.f1;_LL138: _tmp26B=_tmp26A;goto _LL13A;_LL139: if((_tmp268.LessNumelts).tag
!= 3)goto _LL13B;_tmp26B=(struct Cyc_Absyn_Vardecl*)(_tmp268.LessNumelts).val;
_LL13A: _tmp26C=_tmp26B;goto _LL13C;_LL13B: if((_tmp268.LessEqNumelts).tag != 5)goto
_LL13D;_tmp26C=(struct Cyc_Absyn_Vardecl*)(_tmp268.LessEqNumelts).val;_LL13C: if(v
== _tmp26C)continue;goto _LL136;_LL13D:;_LL13E: goto _LL136;_LL136:;}{struct Cyc_List_List*
_tmp534;_tmp266=((_tmp534=_region_malloc(rgn,sizeof(*_tmp534)),((_tmp534->hd=
_tmp267,((_tmp534->tl=_tmp266,_tmp534))))));}}}return _tmp266;}}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e);struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct
_RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*_tmp26E=e->r;
void*_tmp26F;struct Cyc_Absyn_Vardecl*_tmp270;void*_tmp271;struct Cyc_Absyn_Vardecl*
_tmp272;void*_tmp273;struct Cyc_Absyn_Vardecl*_tmp274;void*_tmp275;struct Cyc_Absyn_Vardecl*
_tmp276;_LL140: if(*((int*)_tmp26E)!= 1)goto _LL142;_tmp26F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26E)->f2;if(_tmp26F <= (void*)1)goto _LL142;if(*((int*)_tmp26F)!= 0)goto _LL142;
_tmp270=((struct Cyc_Absyn_Global_b_struct*)_tmp26F)->f1;_LL141: _tmp272=_tmp270;
goto _LL143;_LL142: if(*((int*)_tmp26E)!= 1)goto _LL144;_tmp271=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26E)->f2;if(_tmp271 <= (void*)1)goto _LL144;if(*((int*)_tmp271)!= 2)goto _LL144;
_tmp272=((struct Cyc_Absyn_Param_b_struct*)_tmp271)->f1;_LL143: _tmp274=_tmp272;
goto _LL145;_LL144: if(*((int*)_tmp26E)!= 1)goto _LL146;_tmp273=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26E)->f2;if(_tmp273 <= (void*)1)goto _LL146;if(*((int*)_tmp273)!= 3)goto _LL146;
_tmp274=((struct Cyc_Absyn_Local_b_struct*)_tmp273)->f1;_LL145: _tmp276=_tmp274;
goto _LL147;_LL146: if(*((int*)_tmp26E)!= 1)goto _LL148;_tmp275=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26E)->f2;if(_tmp275 <= (void*)1)goto _LL148;if(*((int*)_tmp275)!= 4)goto _LL148;
_tmp276=((struct Cyc_Absyn_Pat_b_struct*)_tmp275)->f1;_LL147: if(!_tmp276->escapes)
return Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp276);goto _LL13F;_LL148:;_LL149: goto
_LL13F;_LL13F:;}return r;}struct Cyc_CfFlowInfo_TagCmp*Cyc_CfFlowInfo_copy_tagcmp(
struct _RegionHandle*r2,struct Cyc_CfFlowInfo_TagCmp*t);struct Cyc_CfFlowInfo_TagCmp*
Cyc_CfFlowInfo_copy_tagcmp(struct _RegionHandle*r2,struct Cyc_CfFlowInfo_TagCmp*t){
struct Cyc_CfFlowInfo_TagCmp*_tmp535;return(_tmp535=_region_malloc(r2,sizeof(*
_tmp535)),((_tmp535[0]=*t,_tmp535)));}struct Cyc_List_List*Cyc_CfFlowInfo_copy_tagcmps(
struct _RegionHandle*r2,struct Cyc_List_List*ts);struct Cyc_List_List*Cyc_CfFlowInfo_copy_tagcmps(
struct _RegionHandle*r2,struct Cyc_List_List*ts){return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_CfFlowInfo_TagCmp*(*f)(struct _RegionHandle*,
struct Cyc_CfFlowInfo_TagCmp*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r2,Cyc_CfFlowInfo_copy_tagcmp,r2,ts);}struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(
struct _RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r);struct Cyc_CfFlowInfo_Reln*
Cyc_CfFlowInfo_copy_reln(struct _RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r){
struct Cyc_CfFlowInfo_Reln*_tmp536;return(_tmp536=_region_malloc(r2,sizeof(*
_tmp536)),((_tmp536[0]=*r,_tmp536)));}struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(
struct _RegionHandle*r2,struct Cyc_List_List*relns);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(
struct _RegionHandle*r2,struct Cyc_List_List*relns){return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*f)(struct _RegionHandle*,struct
Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*
r1,struct Cyc_List_List*r2);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*r1,
struct Cyc_List_List*r2){for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){struct Cyc_CfFlowInfo_Reln*
_tmp279=(struct Cyc_CfFlowInfo_Reln*)r1->hd;struct Cyc_CfFlowInfo_Reln*_tmp27A=(
struct Cyc_CfFlowInfo_Reln*)r2->hd;if(_tmp279->vd != _tmp27A->vd  || !Cyc_CfFlowInfo_same_relop(
_tmp279->rop,_tmp27A->rop))return 0;}if(r1 != 0  || r2 != 0)return 0;else{return 1;}}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,
struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)return r;r=Cyc_CfFlowInfo_reln_kill_var(
rgn,r,v);{void*_tmp27B=e->r;struct Cyc_Absyn_MallocInfo _tmp27C;struct Cyc_Absyn_Exp*
_tmp27D;int _tmp27E;_LL14B: if(*((int*)_tmp27B)!= 35)goto _LL14D;_tmp27C=((struct
Cyc_Absyn_Malloc_e_struct*)_tmp27B)->f1;_tmp27D=_tmp27C.num_elts;_tmp27E=_tmp27C.fat_result;
if(_tmp27E != 1)goto _LL14D;_LL14C: malloc_loop: {void*_tmp27F=_tmp27D->r;struct Cyc_Absyn_Exp*
_tmp280;void*_tmp281;struct Cyc_Absyn_Vardecl*_tmp282;void*_tmp283;struct Cyc_Absyn_Vardecl*
_tmp284;void*_tmp285;struct Cyc_Absyn_Vardecl*_tmp286;void*_tmp287;struct Cyc_Absyn_Vardecl*
_tmp288;_LL150: if(*((int*)_tmp27F)!= 15)goto _LL152;_tmp280=((struct Cyc_Absyn_Cast_e_struct*)
_tmp27F)->f2;_LL151: _tmp27D=_tmp280;goto malloc_loop;_LL152: if(*((int*)_tmp27F)!= 
1)goto _LL154;_tmp281=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp27F)->f2;if(
_tmp281 <= (void*)1)goto _LL154;if(*((int*)_tmp281)!= 4)goto _LL154;_tmp282=((
struct Cyc_Absyn_Pat_b_struct*)_tmp281)->f1;_LL153: _tmp284=_tmp282;goto _LL155;
_LL154: if(*((int*)_tmp27F)!= 1)goto _LL156;_tmp283=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp27F)->f2;if(_tmp283 <= (void*)1)goto _LL156;if(*((int*)_tmp283)!= 3)goto _LL156;
_tmp284=((struct Cyc_Absyn_Local_b_struct*)_tmp283)->f1;_LL155: _tmp286=_tmp284;
goto _LL157;_LL156: if(*((int*)_tmp27F)!= 1)goto _LL158;_tmp285=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp27F)->f2;if(_tmp285 <= (void*)1)goto _LL158;if(*((int*)_tmp285)!= 2)goto _LL158;
_tmp286=((struct Cyc_Absyn_Param_b_struct*)_tmp285)->f1;_LL157: _tmp288=_tmp286;
goto _LL159;_LL158: if(*((int*)_tmp27F)!= 1)goto _LL15A;_tmp287=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp27F)->f2;if(_tmp287 <= (void*)1)goto _LL15A;if(*((int*)_tmp287)!= 0)goto _LL15A;
_tmp288=((struct Cyc_Absyn_Global_b_struct*)_tmp287)->f1;_LL159: if(_tmp288->escapes)
return r;{struct Cyc_CfFlowInfo_Reln*_tmp539;struct Cyc_List_List*_tmp538;return(
_tmp538=_region_malloc(rgn,sizeof(*_tmp538)),((_tmp538->hd=((_tmp539=
_region_malloc(rgn,sizeof(*_tmp539)),((_tmp539->vd=_tmp288,((_tmp539->rop=Cyc_CfFlowInfo_LessEqNumelts(
v),_tmp539)))))),((_tmp538->tl=r,_tmp538)))));}_LL15A:;_LL15B: return r;_LL14F:;}
_LL14D:;_LL14E: goto _LL14A;_LL14A:;}{void*_tmp28B=Cyc_Tcutil_compress(v->type);
_LL15D: if(_tmp28B <= (void*)4)goto _LL15F;if(*((int*)_tmp28B)!= 5)goto _LL15F;
_LL15E: goto _LL15C;_LL15F:;_LL160: return r;_LL15C:;}loop: {void*_tmp28C=e->r;
struct Cyc_Absyn_Exp*_tmp28D;union Cyc_Absyn_Cnst _tmp28E;struct _tuple6 _tmp28F;int
_tmp290;void*_tmp291;struct Cyc_List_List*_tmp292;struct Cyc_List_List _tmp293;
struct Cyc_List_List*_tmp294;struct Cyc_List_List _tmp295;struct Cyc_Absyn_Exp*
_tmp296;void*_tmp297;struct Cyc_List_List*_tmp298;struct Cyc_List_List _tmp299;
struct Cyc_Absyn_Exp*_tmp29A;_LL162: if(*((int*)_tmp28C)!= 15)goto _LL164;_tmp28D=((
struct Cyc_Absyn_Cast_e_struct*)_tmp28C)->f2;_LL163: e=_tmp28D;goto loop;_LL164: if(*((
int*)_tmp28C)!= 0)goto _LL166;_tmp28E=((struct Cyc_Absyn_Const_e_struct*)_tmp28C)->f1;
if((_tmp28E.Int_c).tag != 4)goto _LL166;_tmp28F=(struct _tuple6)(_tmp28E.Int_c).val;
_tmp290=_tmp28F.f2;_LL165: {struct Cyc_CfFlowInfo_Reln*_tmp53C;struct Cyc_List_List*
_tmp53B;return(_tmp53B=_region_malloc(rgn,sizeof(*_tmp53B)),((_tmp53B->hd=((
_tmp53C=_region_malloc(rgn,sizeof(*_tmp53C)),((_tmp53C->vd=v,((_tmp53C->rop=Cyc_CfFlowInfo_EqualConst((
unsigned int)_tmp290),_tmp53C)))))),((_tmp53B->tl=r,_tmp53B)))));}_LL166: if(*((
int*)_tmp28C)!= 3)goto _LL168;_tmp291=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp28C)->f1;if((int)_tmp291 != 4)goto _LL168;_tmp292=((struct Cyc_Absyn_Primop_e_struct*)
_tmp28C)->f2;if(_tmp292 == 0)goto _LL168;_tmp293=*_tmp292;_tmp294=_tmp293.tl;if(
_tmp294 == 0)goto _LL168;_tmp295=*_tmp294;_tmp296=(struct Cyc_Absyn_Exp*)_tmp295.hd;
_LL167:{void*_tmp29D=_tmp296->r;void*_tmp29E;struct Cyc_List_List*_tmp29F;struct
Cyc_List_List _tmp2A0;struct Cyc_Absyn_Exp*_tmp2A1;_LL16D: if(*((int*)_tmp29D)!= 3)
goto _LL16F;_tmp29E=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp29D)->f1;
_tmp29F=((struct Cyc_Absyn_Primop_e_struct*)_tmp29D)->f2;if(_tmp29F == 0)goto
_LL16F;_tmp2A0=*_tmp29F;_tmp2A1=(struct Cyc_Absyn_Exp*)_tmp2A0.hd;_LL16E:{void*
_tmp2A2=_tmp2A1->r;void*_tmp2A3;struct Cyc_Absyn_Vardecl*_tmp2A4;void*_tmp2A5;
struct Cyc_Absyn_Vardecl*_tmp2A6;void*_tmp2A7;struct Cyc_Absyn_Vardecl*_tmp2A8;
void*_tmp2A9;struct Cyc_Absyn_Vardecl*_tmp2AA;_LL172: if(*((int*)_tmp2A2)!= 1)goto
_LL174;_tmp2A3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2A2)->f2;if(_tmp2A3 <= (
void*)1)goto _LL174;if(*((int*)_tmp2A3)!= 0)goto _LL174;_tmp2A4=((struct Cyc_Absyn_Global_b_struct*)
_tmp2A3)->f1;_LL173: _tmp2A6=_tmp2A4;goto _LL175;_LL174: if(*((int*)_tmp2A2)!= 1)
goto _LL176;_tmp2A5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2A2)->f2;if(
_tmp2A5 <= (void*)1)goto _LL176;if(*((int*)_tmp2A5)!= 3)goto _LL176;_tmp2A6=((
struct Cyc_Absyn_Local_b_struct*)_tmp2A5)->f1;_LL175: _tmp2A8=_tmp2A6;goto _LL177;
_LL176: if(*((int*)_tmp2A2)!= 1)goto _LL178;_tmp2A7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A2)->f2;if(_tmp2A7 <= (void*)1)goto _LL178;if(*((int*)_tmp2A7)!= 2)goto _LL178;
_tmp2A8=((struct Cyc_Absyn_Param_b_struct*)_tmp2A7)->f1;_LL177: _tmp2AA=_tmp2A8;
goto _LL179;_LL178: if(*((int*)_tmp2A2)!= 1)goto _LL17A;_tmp2A9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A2)->f2;if(_tmp2A9 <= (void*)1)goto _LL17A;if(*((int*)_tmp2A9)!= 4)goto _LL17A;
_tmp2AA=((struct Cyc_Absyn_Pat_b_struct*)_tmp2A9)->f1;_LL179: if(_tmp2AA->escapes)
return r;{struct Cyc_CfFlowInfo_Reln*_tmp53F;struct Cyc_List_List*_tmp53E;return(
_tmp53E=_region_malloc(rgn,sizeof(*_tmp53E)),((_tmp53E->hd=((_tmp53F=
_region_malloc(rgn,sizeof(*_tmp53F)),((_tmp53F->vd=v,((_tmp53F->rop=Cyc_CfFlowInfo_LessNumelts(
_tmp2AA),_tmp53F)))))),((_tmp53E->tl=r,_tmp53E)))));}_LL17A:;_LL17B: goto _LL171;
_LL171:;}goto _LL16C;_LL16F:;_LL170: goto _LL16C;_LL16C:;}goto _LL161;_LL168: if(*((
int*)_tmp28C)!= 3)goto _LL16A;_tmp297=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp28C)->f1;_tmp298=((struct Cyc_Absyn_Primop_e_struct*)_tmp28C)->f2;if(_tmp298
== 0)goto _LL16A;_tmp299=*_tmp298;_tmp29A=(struct Cyc_Absyn_Exp*)_tmp299.hd;_LL169:{
void*_tmp2AD=_tmp29A->r;void*_tmp2AE;struct Cyc_Absyn_Vardecl*_tmp2AF;void*
_tmp2B0;struct Cyc_Absyn_Vardecl*_tmp2B1;void*_tmp2B2;struct Cyc_Absyn_Vardecl*
_tmp2B3;void*_tmp2B4;struct Cyc_Absyn_Vardecl*_tmp2B5;_LL17D: if(*((int*)_tmp2AD)
!= 1)goto _LL17F;_tmp2AE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2AD)->f2;if(
_tmp2AE <= (void*)1)goto _LL17F;if(*((int*)_tmp2AE)!= 0)goto _LL17F;_tmp2AF=((
struct Cyc_Absyn_Global_b_struct*)_tmp2AE)->f1;_LL17E: _tmp2B1=_tmp2AF;goto _LL180;
_LL17F: if(*((int*)_tmp2AD)!= 1)goto _LL181;_tmp2B0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AD)->f2;if(_tmp2B0 <= (void*)1)goto _LL181;if(*((int*)_tmp2B0)!= 3)goto _LL181;
_tmp2B1=((struct Cyc_Absyn_Local_b_struct*)_tmp2B0)->f1;_LL180: _tmp2B3=_tmp2B1;
goto _LL182;_LL181: if(*((int*)_tmp2AD)!= 1)goto _LL183;_tmp2B2=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AD)->f2;if(_tmp2B2 <= (void*)1)goto _LL183;if(*((int*)_tmp2B2)!= 2)goto _LL183;
_tmp2B3=((struct Cyc_Absyn_Param_b_struct*)_tmp2B2)->f1;_LL182: _tmp2B5=_tmp2B3;
goto _LL184;_LL183: if(*((int*)_tmp2AD)!= 1)goto _LL185;_tmp2B4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AD)->f2;if(_tmp2B4 <= (void*)1)goto _LL185;if(*((int*)_tmp2B4)!= 4)goto _LL185;
_tmp2B5=((struct Cyc_Absyn_Pat_b_struct*)_tmp2B4)->f1;_LL184: if(_tmp2B5->escapes)
return r;{struct Cyc_CfFlowInfo_Reln*_tmp542;struct Cyc_List_List*_tmp541;return(
_tmp541=_region_malloc(rgn,sizeof(*_tmp541)),((_tmp541->hd=((_tmp542=
_region_malloc(rgn,sizeof(*_tmp542)),((_tmp542->vd=v,((_tmp542->rop=Cyc_CfFlowInfo_LessEqNumelts(
_tmp2B5),_tmp542)))))),((_tmp541->tl=r,_tmp541)))));}_LL185:;_LL186: goto _LL17C;
_LL17C:;}goto _LL161;_LL16A:;_LL16B: goto _LL161;_LL161:;}return r;}struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){{void*_tmp2B8=e1->r;void*_tmp2B9;struct Cyc_Absyn_Vardecl*_tmp2BA;void*
_tmp2BB;struct Cyc_Absyn_Vardecl*_tmp2BC;void*_tmp2BD;struct Cyc_Absyn_Vardecl*
_tmp2BE;void*_tmp2BF;struct Cyc_Absyn_Vardecl*_tmp2C0;_LL188: if(*((int*)_tmp2B8)
!= 1)goto _LL18A;_tmp2B9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2B8)->f2;if(
_tmp2B9 <= (void*)1)goto _LL18A;if(*((int*)_tmp2B9)!= 0)goto _LL18A;_tmp2BA=((
struct Cyc_Absyn_Global_b_struct*)_tmp2B9)->f1;_LL189: _tmp2BC=_tmp2BA;goto _LL18B;
_LL18A: if(*((int*)_tmp2B8)!= 1)goto _LL18C;_tmp2BB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2B8)->f2;if(_tmp2BB <= (void*)1)goto _LL18C;if(*((int*)_tmp2BB)!= 2)goto _LL18C;
_tmp2BC=((struct Cyc_Absyn_Param_b_struct*)_tmp2BB)->f1;_LL18B: _tmp2BE=_tmp2BC;
goto _LL18D;_LL18C: if(*((int*)_tmp2B8)!= 1)goto _LL18E;_tmp2BD=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2B8)->f2;if(_tmp2BD <= (void*)1)goto _LL18E;if(*((int*)_tmp2BD)!= 3)goto _LL18E;
_tmp2BE=((struct Cyc_Absyn_Local_b_struct*)_tmp2BD)->f1;_LL18D: _tmp2C0=_tmp2BE;
goto _LL18F;_LL18E: if(*((int*)_tmp2B8)!= 1)goto _LL190;_tmp2BF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2B8)->f2;if(_tmp2BF <= (void*)1)goto _LL190;if(*((int*)_tmp2BF)!= 4)goto _LL190;
_tmp2C0=((struct Cyc_Absyn_Pat_b_struct*)_tmp2BF)->f1;_LL18F: if(!_tmp2C0->escapes)
return Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp2C0,e2);goto _LL187;_LL190:;_LL191:
goto _LL187;_LL187:;}return r;}static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*
r);static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*r){{const char*
_tmp546;void*_tmp545[1];struct Cyc_String_pa_struct _tmp544;(_tmp544.tag=0,((
_tmp544.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((
r->vd)->name)),((_tmp545[0]=& _tmp544,Cyc_fprintf(Cyc_stderr,((_tmp546="%s",
_tag_dyneither(_tmp546,sizeof(char),3))),_tag_dyneither(_tmp545,sizeof(void*),1)))))));}{
union Cyc_CfFlowInfo_RelnOp _tmp2C4=r->rop;unsigned int _tmp2C5;struct _tuple11
_tmp2C6;struct Cyc_Absyn_Vardecl*_tmp2C7;struct Cyc_Absyn_Vardecl*_tmp2C8;
unsigned int _tmp2C9;struct Cyc_Absyn_Vardecl*_tmp2CA;_LL193: if((_tmp2C4.EqualConst).tag
!= 1)goto _LL195;_tmp2C5=(unsigned int)(_tmp2C4.EqualConst).val;_LL194:{const char*
_tmp54A;void*_tmp549[1];struct Cyc_Int_pa_struct _tmp548;(_tmp548.tag=1,((_tmp548.f1=(
unsigned long)((int)_tmp2C5),((_tmp549[0]=& _tmp548,Cyc_fprintf(Cyc_stderr,((
_tmp54A="==%d",_tag_dyneither(_tmp54A,sizeof(char),5))),_tag_dyneither(_tmp549,
sizeof(void*),1)))))));}goto _LL192;_LL195: if((_tmp2C4.LessVar).tag != 2)goto
_LL197;_tmp2C6=(struct _tuple11)(_tmp2C4.LessVar).val;_tmp2C7=_tmp2C6.f1;_LL196:{
const char*_tmp54E;void*_tmp54D[1];struct Cyc_String_pa_struct _tmp54C;(_tmp54C.tag=
0,((_tmp54C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp2C7->name)),((_tmp54D[0]=& _tmp54C,Cyc_fprintf(Cyc_stderr,((_tmp54E="<%s",
_tag_dyneither(_tmp54E,sizeof(char),4))),_tag_dyneither(_tmp54D,sizeof(void*),1)))))));}
goto _LL192;_LL197: if((_tmp2C4.LessNumelts).tag != 3)goto _LL199;_tmp2C8=(struct Cyc_Absyn_Vardecl*)(
_tmp2C4.LessNumelts).val;_LL198:{const char*_tmp552;void*_tmp551[1];struct Cyc_String_pa_struct
_tmp550;(_tmp550.tag=0,((_tmp550.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp2C8->name)),((_tmp551[0]=& _tmp550,Cyc_fprintf(Cyc_stderr,((
_tmp552="<numelts(%s)",_tag_dyneither(_tmp552,sizeof(char),13))),_tag_dyneither(
_tmp551,sizeof(void*),1)))))));}goto _LL192;_LL199: if((_tmp2C4.LessConst).tag != 4)
goto _LL19B;_tmp2C9=(unsigned int)(_tmp2C4.LessConst).val;_LL19A:{const char*
_tmp556;void*_tmp555[1];struct Cyc_Int_pa_struct _tmp554;(_tmp554.tag=1,((_tmp554.f1=(
unsigned long)((int)_tmp2C9),((_tmp555[0]=& _tmp554,Cyc_fprintf(Cyc_stderr,((
_tmp556="<%d",_tag_dyneither(_tmp556,sizeof(char),4))),_tag_dyneither(_tmp555,
sizeof(void*),1)))))));}goto _LL192;_LL19B: if((_tmp2C4.LessEqNumelts).tag != 5)
goto _LL192;_tmp2CA=(struct Cyc_Absyn_Vardecl*)(_tmp2C4.LessEqNumelts).val;_LL19C:{
const char*_tmp55A;void*_tmp559[1];struct Cyc_String_pa_struct _tmp558;(_tmp558.tag=
0,((_tmp558.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp2CA->name)),((_tmp559[0]=& _tmp558,Cyc_fprintf(Cyc_stderr,((_tmp55A="<=numelts(%s)",
_tag_dyneither(_tmp55A,sizeof(char),14))),_tag_dyneither(_tmp559,sizeof(void*),1)))))));}
goto _LL192;_LL192:;}}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r);void
Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){for(0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((
struct Cyc_CfFlowInfo_Reln*)r->hd);if(r->tl != 0){const char*_tmp55D;void*_tmp55C;(
_tmp55C=0,Cyc_fprintf(Cyc_stderr,((_tmp55D=",",_tag_dyneither(_tmp55D,sizeof(
char),2))),_tag_dyneither(_tmp55C,sizeof(void*),0)));}}}void Cyc_CfFlowInfo_print_initlevel(
void*il);void Cyc_CfFlowInfo_print_initlevel(void*il){void*_tmp2DC=il;_LL19E: if((
int)_tmp2DC != 0)goto _LL1A0;_LL19F:{const char*_tmp560;void*_tmp55F;(_tmp55F=0,Cyc_fprintf(
Cyc_stderr,((_tmp560="uninitialized",_tag_dyneither(_tmp560,sizeof(char),14))),
_tag_dyneither(_tmp55F,sizeof(void*),0)));}goto _LL19D;_LL1A0: if((int)_tmp2DC != 1)
goto _LL1A2;_LL1A1:{const char*_tmp563;void*_tmp562;(_tmp562=0,Cyc_fprintf(Cyc_stderr,((
_tmp563="this-initialized",_tag_dyneither(_tmp563,sizeof(char),17))),
_tag_dyneither(_tmp562,sizeof(void*),0)));}goto _LL19D;_LL1A2: if((int)_tmp2DC != 2)
goto _LL19D;_LL1A3:{const char*_tmp566;void*_tmp565;(_tmp565=0,Cyc_fprintf(Cyc_stderr,((
_tmp566="all-initialized",_tag_dyneither(_tmp566,sizeof(char),16))),
_tag_dyneither(_tmp565,sizeof(void*),0)));}goto _LL19D;_LL19D:;}void Cyc_CfFlowInfo_print_root(
void*root);void Cyc_CfFlowInfo_print_root(void*root){void*_tmp2E3=root;struct Cyc_Absyn_Vardecl*
_tmp2E4;struct Cyc_Absyn_Exp*_tmp2E5;void*_tmp2E6;_LL1A5: if(*((int*)_tmp2E3)!= 0)
goto _LL1A7;_tmp2E4=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp2E3)->f1;_LL1A6:{
const char*_tmp56A;void*_tmp569[1];struct Cyc_String_pa_struct _tmp568;(_tmp568.tag=
0,((_tmp568.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp2E4->name)),((_tmp569[0]=& _tmp568,Cyc_fprintf(Cyc_stderr,((_tmp56A="Root(%s)",
_tag_dyneither(_tmp56A,sizeof(char),9))),_tag_dyneither(_tmp569,sizeof(void*),1)))))));}
goto _LL1A4;_LL1A7: if(*((int*)_tmp2E3)!= 1)goto _LL1A9;_tmp2E5=((struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp2E3)->f1;_tmp2E6=(void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp2E3)->f2;
_LL1A8:{const char*_tmp56F;void*_tmp56E[2];struct Cyc_String_pa_struct _tmp56D;
struct Cyc_String_pa_struct _tmp56C;(_tmp56C.tag=0,((_tmp56C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2E6)),((_tmp56D.tag=
0,((_tmp56D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
_tmp2E5)),((_tmp56E[0]=& _tmp56D,((_tmp56E[1]=& _tmp56C,Cyc_fprintf(Cyc_stderr,((
_tmp56F="MallocPt(%s,%s)",_tag_dyneither(_tmp56F,sizeof(char),16))),
_tag_dyneither(_tmp56E,sizeof(void*),2)))))))))))));}goto _LL1A4;_LL1A9: if(*((int*)
_tmp2E3)!= 2)goto _LL1A4;_LL1AA:{const char*_tmp572;void*_tmp571;(_tmp571=0,Cyc_fprintf(
Cyc_stderr,((_tmp572="InitParam(_,_)",_tag_dyneither(_tmp572,sizeof(char),15))),
_tag_dyneither(_tmp571,sizeof(void*),0)));}goto _LL1A4;_LL1A4:;}void Cyc_CfFlowInfo_print_place(
struct Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*
p){Cyc_CfFlowInfo_print_root(p->root);{struct Cyc_List_List*_tmp2F0=p->fields;
while(_tmp2F0 != 0){{const char*_tmp576;void*_tmp575[1];struct Cyc_Int_pa_struct
_tmp574;(_tmp574.tag=1,((_tmp574.f1=(unsigned long)((int)_tmp2F0->hd),((_tmp575[
0]=& _tmp574,Cyc_fprintf(Cyc_stderr,((_tmp576=".%d",_tag_dyneither(_tmp576,
sizeof(char),4))),_tag_dyneither(_tmp575,sizeof(void*),1)))))));}_tmp2F0=_tmp2F0->tl;}{
const char*_tmp579;void*_tmp578;(_tmp578=0,Cyc_fprintf(Cyc_stderr,((_tmp579=" ",
_tag_dyneither(_tmp579,sizeof(char),2))),_tag_dyneither(_tmp578,sizeof(void*),0)));}}}
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*));void Cyc_CfFlowInfo_print_list(
struct Cyc_List_List*x,void(*pr)(void*)){int _tmp2F6=1;while(x != 0){if(!_tmp2F6){{
const char*_tmp57C;void*_tmp57B;(_tmp57B=0,Cyc_fprintf(Cyc_stderr,((_tmp57C=", ",
_tag_dyneither(_tmp57C,sizeof(char),3))),_tag_dyneither(_tmp57B,sizeof(void*),0)));}
_tmp2F6=0;}pr((void*)x->hd);x=x->tl;}{const char*_tmp57F;void*_tmp57E;(_tmp57E=0,
Cyc_fprintf(Cyc_stderr,((_tmp57F="\n",_tag_dyneither(_tmp57F,sizeof(char),2))),
_tag_dyneither(_tmp57E,sizeof(void*),0)));}}void Cyc_CfFlowInfo_print_dict_set(
struct Cyc_Dict_Dict p,void(*pr)(void*));void Cyc_CfFlowInfo_print_dict_set(struct
Cyc_Dict_Dict p,void(*pr)(void*)){struct _RegionHandle _tmp2FB=_new_region("r");
struct _RegionHandle*r=& _tmp2FB;_push_region(r);{struct _handler_cons _tmp2FC;
_push_handler(& _tmp2FC);{int _tmp2FE=0;if(setjmp(_tmp2FC.handler))_tmp2FE=1;if(!
_tmp2FE){{struct Cyc_Iter_Iter _tmp2FF=((struct Cyc_Iter_Iter(*)(struct
_RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);struct _tuple18
_tmp300=*((struct _tuple18*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
r,p);int first=1;{const char*_tmp582;void*_tmp581;(_tmp581=0,Cyc_fprintf(Cyc_stderr,((
_tmp582="{ ",_tag_dyneither(_tmp582,sizeof(char),3))),_tag_dyneither(_tmp581,
sizeof(void*),0)));}while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(
_tmp2FF,& _tmp300)){void*_tmp303=_tmp300.f1;if(!first){{const char*_tmp585;void*
_tmp584;(_tmp584=0,Cyc_fprintf(Cyc_stderr,((_tmp585=", ",_tag_dyneither(_tmp585,
sizeof(char),3))),_tag_dyneither(_tmp584,sizeof(void*),0)));}first=0;}pr(_tmp303);}{
const char*_tmp588;void*_tmp587;(_tmp587=0,Cyc_fprintf(Cyc_stderr,((_tmp588="}\n",
_tag_dyneither(_tmp588,sizeof(char),3))),_tag_dyneither(_tmp587,sizeof(void*),0)));}};
_pop_handler();}else{void*_tmp2FD=(void*)_exn_thrown;void*_tmp309=_tmp2FD;_LL1AC:;
_LL1AD:{const char*_tmp58B;void*_tmp58A;(_tmp58A=0,Cyc_fprintf(Cyc_stderr,((
_tmp58B="{ }\n",_tag_dyneither(_tmp58B,sizeof(char),5))),_tag_dyneither(_tmp58A,
sizeof(void*),0)));}_npop_handler(0);return;_LL1AE:;_LL1AF:(void)_throw(_tmp309);
_LL1AB:;}}};_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(void*rval);void Cyc_CfFlowInfo_print_absrval(
void*rval){void*_tmp30C=rval;void*_tmp30D;void*_tmp30E;struct Cyc_CfFlowInfo_Place*
_tmp30F;int _tmp310;struct _dyneither_ptr _tmp311;_LL1B1: if((int)_tmp30C != 0)goto
_LL1B3;_LL1B2:{const char*_tmp58E;void*_tmp58D;(_tmp58D=0,Cyc_fprintf(Cyc_stderr,((
_tmp58E="Zero",_tag_dyneither(_tmp58E,sizeof(char),5))),_tag_dyneither(_tmp58D,
sizeof(void*),0)));}goto _LL1B0;_LL1B3: if((int)_tmp30C != 1)goto _LL1B5;_LL1B4:{
const char*_tmp591;void*_tmp590;(_tmp590=0,Cyc_fprintf(Cyc_stderr,((_tmp591="NotZeroAll",
_tag_dyneither(_tmp591,sizeof(char),11))),_tag_dyneither(_tmp590,sizeof(void*),0)));}
goto _LL1B0;_LL1B5: if((int)_tmp30C != 2)goto _LL1B7;_LL1B6:{const char*_tmp594;void*
_tmp593;(_tmp593=0,Cyc_fprintf(Cyc_stderr,((_tmp594="NotZeroThis",_tag_dyneither(
_tmp594,sizeof(char),12))),_tag_dyneither(_tmp593,sizeof(void*),0)));}goto _LL1B0;
_LL1B7: if(_tmp30C <= (void*)3)goto _LL1B9;if(*((int*)_tmp30C)!= 0)goto _LL1B9;
_tmp30D=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp30C)->f1;_LL1B8:{
const char*_tmp597;void*_tmp596;(_tmp596=0,Cyc_fprintf(Cyc_stderr,((_tmp597="Unknown(",
_tag_dyneither(_tmp597,sizeof(char),9))),_tag_dyneither(_tmp596,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_initlevel(_tmp30D);{const char*_tmp59A;void*_tmp599;(_tmp599=
0,Cyc_fprintf(Cyc_stderr,((_tmp59A=")",_tag_dyneither(_tmp59A,sizeof(char),2))),
_tag_dyneither(_tmp599,sizeof(void*),0)));}goto _LL1B0;_LL1B9: if(_tmp30C <= (void*)
3)goto _LL1BB;if(*((int*)_tmp30C)!= 1)goto _LL1BB;_tmp30E=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp30C)->f1;_LL1BA:{const char*_tmp59D;void*_tmp59C;(_tmp59C=0,Cyc_fprintf(Cyc_stderr,((
_tmp59D="Esc(",_tag_dyneither(_tmp59D,sizeof(char),5))),_tag_dyneither(_tmp59C,
sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(_tmp30E);{const char*_tmp5A0;
void*_tmp59F;(_tmp59F=0,Cyc_fprintf(Cyc_stderr,((_tmp5A0=")",_tag_dyneither(
_tmp5A0,sizeof(char),2))),_tag_dyneither(_tmp59F,sizeof(void*),0)));}goto _LL1B0;
_LL1BB: if(_tmp30C <= (void*)3)goto _LL1BD;if(*((int*)_tmp30C)!= 2)goto _LL1BD;
_tmp30F=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp30C)->f1;_LL1BC:{const char*
_tmp5A3;void*_tmp5A2;(_tmp5A2=0,Cyc_fprintf(Cyc_stderr,((_tmp5A3="AddrOf(",
_tag_dyneither(_tmp5A3,sizeof(char),8))),_tag_dyneither(_tmp5A2,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_place(_tmp30F);{const char*_tmp5A6;void*_tmp5A5;(_tmp5A5=0,
Cyc_fprintf(Cyc_stderr,((_tmp5A6=")",_tag_dyneither(_tmp5A6,sizeof(char),2))),
_tag_dyneither(_tmp5A5,sizeof(void*),0)));}goto _LL1B0;_LL1BD: if(_tmp30C <= (void*)
3)goto _LL1BF;if(*((int*)_tmp30C)!= 3)goto _LL1BF;_LL1BE:{const char*_tmp5A9;void*
_tmp5A8;(_tmp5A8=0,Cyc_fprintf(Cyc_stderr,((_tmp5A9="TagCmps(?)",_tag_dyneither(
_tmp5A9,sizeof(char),11))),_tag_dyneither(_tmp5A8,sizeof(void*),0)));}goto _LL1B0;
_LL1BF: if(_tmp30C <= (void*)3)goto _LL1B0;if(*((int*)_tmp30C)!= 4)goto _LL1B0;
_tmp310=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp30C)->f1;_tmp311=((struct
Cyc_CfFlowInfo_Aggregate_struct*)_tmp30C)->f2;_LL1C0: if(_tmp310){const char*
_tmp5AC;void*_tmp5AB;(_tmp5AB=0,Cyc_fprintf(Cyc_stderr,((_tmp5AC="AggrUnion{",
_tag_dyneither(_tmp5AC,sizeof(char),11))),_tag_dyneither(_tmp5AB,sizeof(void*),0)));}
else{const char*_tmp5AF;void*_tmp5AE;(_tmp5AE=0,Cyc_fprintf(Cyc_stderr,((_tmp5AF="AggrStruct{",
_tag_dyneither(_tmp5AF,sizeof(char),12))),_tag_dyneither(_tmp5AE,sizeof(void*),0)));}{
int i=0;for(0;i < _get_dyneither_size(_tmp311,sizeof(void*));++ i){Cyc_CfFlowInfo_print_absrval(((
void**)_tmp311.curr)[i]);if(i + 1 < _get_dyneither_size(_tmp311,sizeof(void*))){
const char*_tmp5B2;void*_tmp5B1;(_tmp5B1=0,Cyc_fprintf(Cyc_stderr,((_tmp5B2=",",
_tag_dyneither(_tmp5B2,sizeof(char),2))),_tag_dyneither(_tmp5B1,sizeof(void*),0)));}}}{
const char*_tmp5B5;void*_tmp5B4;(_tmp5B4=0,Cyc_fprintf(Cyc_stderr,((_tmp5B5="}",
_tag_dyneither(_tmp5B5,sizeof(char),2))),_tag_dyneither(_tmp5B4,sizeof(void*),0)));}
goto _LL1B0;_LL1B0:;}static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*
rval);static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){{const
char*_tmp5B8;void*_tmp5B7;(_tmp5B7=0,Cyc_fprintf(Cyc_stderr,((_tmp5B8="    ",
_tag_dyneither(_tmp5B8,sizeof(char),5))),_tag_dyneither(_tmp5B7,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_root(root);{const char*_tmp5BB;void*_tmp5BA;(_tmp5BA=0,Cyc_fprintf(
Cyc_stderr,((_tmp5BB=" --> ",_tag_dyneither(_tmp5BB,sizeof(char),6))),
_tag_dyneither(_tmp5BA,sizeof(void*),0)));}Cyc_CfFlowInfo_print_absrval(rval);{
const char*_tmp5BE;void*_tmp5BD;(_tmp5BD=0,Cyc_fprintf(Cyc_stderr,((_tmp5BE="\n",
_tag_dyneither(_tmp5BE,sizeof(char),2))),_tag_dyneither(_tmp5BD,sizeof(void*),0)));}}
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);void Cyc_CfFlowInfo_print_flowdict(
struct Cyc_Dict_Dict d){Cyc_Dict_iter(Cyc_CfFlowInfo_print_flow_mapping,d);}void
Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);void Cyc_CfFlowInfo_print_flow(
union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo _tmp334=f;int _tmp335;
struct _tuple12 _tmp336;struct Cyc_Dict_Dict _tmp337;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp338;_LL1C2: if((_tmp334.BottomFL).tag != 1)goto _LL1C4;_tmp335=(int)(_tmp334.BottomFL).val;
_LL1C3:{const char*_tmp5C1;void*_tmp5C0;(_tmp5C0=0,Cyc_fprintf(Cyc_stderr,((
_tmp5C1="  BottomFL\n",_tag_dyneither(_tmp5C1,sizeof(char),12))),_tag_dyneither(
_tmp5C0,sizeof(void*),0)));}goto _LL1C1;_LL1C4: if((_tmp334.ReachableFL).tag != 2)
goto _LL1C1;_tmp336=(struct _tuple12)(_tmp334.ReachableFL).val;_tmp337=_tmp336.f1;
_tmp338=_tmp336.f3;_LL1C5:{const char*_tmp5C4;void*_tmp5C3;(_tmp5C3=0,Cyc_fprintf(
Cyc_stderr,((_tmp5C4="  ReachableFL:\n",_tag_dyneither(_tmp5C4,sizeof(char),16))),
_tag_dyneither(_tmp5C3,sizeof(void*),0)));}Cyc_Dict_iter(Cyc_CfFlowInfo_print_flow_mapping,
_tmp337);{const char*_tmp5C7;void*_tmp5C6;(_tmp5C6=0,Cyc_fprintf(Cyc_stderr,((
_tmp5C7="\n    consumed: ",_tag_dyneither(_tmp5C7,sizeof(char),16))),
_tag_dyneither(_tmp5C6,sizeof(void*),0)));}((void(*)(struct Cyc_Dict_Dict p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(_tmp338.consumed,
Cyc_CfFlowInfo_print_place);{const char*_tmp5CA;void*_tmp5C9;(_tmp5C9=0,Cyc_fprintf(
Cyc_stderr,((_tmp5CA="\n    may_consume: ",_tag_dyneither(_tmp5CA,sizeof(char),
19))),_tag_dyneither(_tmp5C9,sizeof(void*),0)));}((void(*)(struct Cyc_List_List*x,
void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(_tmp338.may_consume,
Cyc_CfFlowInfo_print_place);{const char*_tmp5CD;void*_tmp5CC;(_tmp5CC=0,Cyc_fprintf(
Cyc_stderr,((_tmp5CD="\n",_tag_dyneither(_tmp5CD,sizeof(char),2))),
_tag_dyneither(_tmp5CC,sizeof(void*),0)));}goto _LL1C1;_LL1C1:;}int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t);int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){void*_tmp343=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp344;struct Cyc_Absyn_DatatypeInfo
_tmp345;struct Cyc_List_List*_tmp346;struct Cyc_Core_Opt*_tmp347;struct Cyc_List_List*
_tmp348;struct Cyc_Absyn_AggrInfo _tmp349;struct Cyc_List_List*_tmp34A;struct Cyc_Absyn_DatatypeFieldInfo
_tmp34B;struct Cyc_List_List*_tmp34C;struct Cyc_List_List*_tmp34D;struct Cyc_Absyn_PtrInfo
_tmp34E;void*_tmp34F;struct Cyc_Absyn_PtrAtts _tmp350;void*_tmp351;struct Cyc_List_List*
_tmp352;struct Cyc_List_List*_tmp353;struct Cyc_Absyn_ArrayInfo _tmp354;void*
_tmp355;void*_tmp356;void*_tmp357;void*_tmp358;void*_tmp359;void*_tmp35A;_LL1C7:
if((int)_tmp343 != 0)goto _LL1C9;_LL1C8: goto _LL1CA;_LL1C9: if(_tmp343 <= (void*)4)
goto _LL1CB;if(*((int*)_tmp343)!= 5)goto _LL1CB;_LL1CA: goto _LL1CC;_LL1CB: if((int)
_tmp343 != 1)goto _LL1CD;_LL1CC: goto _LL1CE;_LL1CD: if(_tmp343 <= (void*)4)goto _LL1D5;
if(*((int*)_tmp343)!= 6)goto _LL1CF;_LL1CE: goto _LL1D0;_LL1CF: if(*((int*)_tmp343)
!= 12)goto _LL1D1;_LL1D0: goto _LL1D2;_LL1D1: if(*((int*)_tmp343)!= 13)goto _LL1D3;
_LL1D2: goto _LL1D4;_LL1D3: if(*((int*)_tmp343)!= 18)goto _LL1D5;_LL1D4: goto _LL1D6;
_LL1D5: if((int)_tmp343 != 2)goto _LL1D7;_LL1D6: goto _LL1D8;_LL1D7: if(_tmp343 <= (
void*)4)goto _LL1D9;if(*((int*)_tmp343)!= 0)goto _LL1D9;_LL1D8: return 0;_LL1D9: if((
int)_tmp343 != 3)goto _LL1DB;_LL1DA: {void*_tmp35B=rgn;_LL1FC: if((int)_tmp35B != 0)
goto _LL1FE;_LL1FD: return 1;_LL1FE:;_LL1FF: return 0;_LL1FB:;}_LL1DB: if(_tmp343 <= (
void*)4)goto _LL1DD;if(*((int*)_tmp343)!= 1)goto _LL1DD;_tmp344=((struct Cyc_Absyn_VarType_struct*)
_tmp343)->f1;_LL1DC: {void*_tmp35C=rgn;struct Cyc_Absyn_Tvar*_tmp35D;_LL201: if((
int)_tmp35C != 0)goto _LL203;_LL202: return 0;_LL203: if(_tmp35C <= (void*)1)goto
_LL200;if(*((int*)_tmp35C)!= 0)goto _LL200;_tmp35D=((struct Cyc_CfFlowInfo_Region_k_struct*)
_tmp35C)->f1;_LL204: return Cyc_Absyn_tvar_cmp(_tmp344,_tmp35D)== 0;_LL200:;}
_LL1DD: if(_tmp343 <= (void*)4)goto _LL1DF;if(*((int*)_tmp343)!= 2)goto _LL1DF;
_tmp345=((struct Cyc_Absyn_DatatypeType_struct*)_tmp343)->f1;_tmp346=_tmp345.targs;
_tmp347=_tmp345.rgn;_LL1DE: if((unsigned int)_tmp347  && Cyc_CfFlowInfo_contains_region(
rgn,(void*)_tmp347->v))return 1;_tmp348=_tmp346;goto _LL1E0;_LL1DF: if(_tmp343 <= (
void*)4)goto _LL1E1;if(*((int*)_tmp343)!= 16)goto _LL1E1;_tmp348=((struct Cyc_Absyn_TypedefType_struct*)
_tmp343)->f2;_LL1E0: _tmp34A=_tmp348;goto _LL1E2;_LL1E1: if(_tmp343 <= (void*)4)goto
_LL1E3;if(*((int*)_tmp343)!= 10)goto _LL1E3;_tmp349=((struct Cyc_Absyn_AggrType_struct*)
_tmp343)->f1;_tmp34A=_tmp349.targs;_LL1E2: _tmp34C=_tmp34A;goto _LL1E4;_LL1E3: if(
_tmp343 <= (void*)4)goto _LL1E5;if(*((int*)_tmp343)!= 3)goto _LL1E5;_tmp34B=((
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp343)->f1;_tmp34C=_tmp34B.targs;
_LL1E4: _tmp34D=_tmp34C;goto _LL1E6;_LL1E5: if(_tmp343 <= (void*)4)goto _LL1E7;if(*((
int*)_tmp343)!= 20)goto _LL1E7;_tmp34D=((struct Cyc_Absyn_JoinEff_struct*)_tmp343)->f1;
_LL1E6: return Cyc_List_exists_c(Cyc_CfFlowInfo_contains_region,rgn,_tmp34D);
_LL1E7: if(_tmp343 <= (void*)4)goto _LL1E9;if(*((int*)_tmp343)!= 4)goto _LL1E9;
_tmp34E=((struct Cyc_Absyn_PointerType_struct*)_tmp343)->f1;_tmp34F=_tmp34E.elt_typ;
_tmp350=_tmp34E.ptr_atts;_tmp351=_tmp350.rgn;_LL1E8: return Cyc_CfFlowInfo_contains_region(
rgn,_tmp351) || Cyc_CfFlowInfo_contains_region(rgn,_tmp34F);_LL1E9: if(_tmp343 <= (
void*)4)goto _LL1EB;if(*((int*)_tmp343)!= 8)goto _LL1EB;_LL1EA: return 0;_LL1EB: if(
_tmp343 <= (void*)4)goto _LL1ED;if(*((int*)_tmp343)!= 9)goto _LL1ED;_tmp352=((
struct Cyc_Absyn_TupleType_struct*)_tmp343)->f1;_LL1EC: for(0;_tmp352 != 0;_tmp352=
_tmp352->tl){if(Cyc_CfFlowInfo_contains_region(rgn,(*((struct _tuple19*)_tmp352->hd)).f2))
return 1;}return 0;_LL1ED: if(_tmp343 <= (void*)4)goto _LL1EF;if(*((int*)_tmp343)!= 
11)goto _LL1EF;_tmp353=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp343)->f2;_LL1EE:
for(0;_tmp353 != 0;_tmp353=_tmp353->tl){if(Cyc_CfFlowInfo_contains_region(rgn,((
struct Cyc_Absyn_Aggrfield*)_tmp353->hd)->type))return 1;}return 0;_LL1EF: if(
_tmp343 <= (void*)4)goto _LL1F1;if(*((int*)_tmp343)!= 7)goto _LL1F1;_tmp354=((
struct Cyc_Absyn_ArrayType_struct*)_tmp343)->f1;_tmp355=_tmp354.elt_type;_LL1F0:
_tmp356=_tmp355;goto _LL1F2;_LL1F1: if(_tmp343 <= (void*)4)goto _LL1F3;if(*((int*)
_tmp343)!= 19)goto _LL1F3;_tmp356=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp343)->f1;_LL1F2: _tmp357=_tmp356;goto _LL1F4;_LL1F3: if(_tmp343 <= (void*)4)goto
_LL1F5;if(*((int*)_tmp343)!= 21)goto _LL1F5;_tmp357=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp343)->f1;_LL1F4: return Cyc_CfFlowInfo_contains_region(rgn,_tmp357);_LL1F5: if(
_tmp343 <= (void*)4)goto _LL1F7;if(*((int*)_tmp343)!= 15)goto _LL1F7;_tmp358=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp343)->f1;_tmp359=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp343)->f2;_LL1F6: return Cyc_CfFlowInfo_contains_region(rgn,_tmp358) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp359);_LL1F7: if(_tmp343 <= (void*)4)goto _LL1F9;if(*((int*)_tmp343)!= 14)
goto _LL1F9;_tmp35A=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp343)->f1;
_LL1F8: return 0;_LL1F9: if(_tmp343 <= (void*)4)goto _LL1C6;if(*((int*)_tmp343)!= 17)
goto _LL1C6;_LL1FA: return 0;_LL1C6:;}struct _tuple25{struct Cyc_CfFlowInfo_FlowEnv*
f1;struct Cyc_Dict_Dict f2;void*f3;};static void Cyc_CfFlowInfo_kill_root(struct
_tuple25*env,void*root,void*rval);static void Cyc_CfFlowInfo_kill_root(struct
_tuple25*env,void*root,void*rval){struct _tuple25 _tmp35F;struct Cyc_CfFlowInfo_FlowEnv*
_tmp360;struct Cyc_Dict_Dict _tmp361;struct Cyc_Dict_Dict*_tmp362;void*_tmp363;
struct _tuple25*_tmp35E=env;_tmp35F=*_tmp35E;_tmp360=_tmp35F.f1;_tmp361=_tmp35F.f2;
_tmp362=(struct Cyc_Dict_Dict*)&(*_tmp35E).f2;_tmp363=_tmp35F.f3;{void*_tmp364=
root;struct Cyc_Absyn_Vardecl*_tmp365;void*_tmp366;_LL206: if(*((int*)_tmp364)!= 0)
goto _LL208;_tmp365=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp364)->f1;_LL207:
if(Cyc_CfFlowInfo_contains_region(_tmp363,_tmp365->type))rval=Cyc_CfFlowInfo_typ_to_absrval(
_tmp360,_tmp365->type,_tmp360->unknown_none);*_tmp362=Cyc_Dict_insert(*_tmp362,
root,rval);goto _LL205;_LL208: if(*((int*)_tmp364)!= 1)goto _LL20A;_tmp366=(void*)((
struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp364)->f2;_LL209: if(!Cyc_CfFlowInfo_contains_region(
_tmp363,_tmp366))*_tmp362=Cyc_Dict_insert(*_tmp362,root,rval);goto _LL205;_LL20A:
if(*((int*)_tmp364)!= 2)goto _LL205;_LL20B: goto _LL205;_LL205:;}}static struct Cyc_Dict_Dict
Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict
fd,void*rgn);static struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct
Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict fd,void*rgn){void*_tmp367=Cyc_Tcutil_compress(
rgn);struct Cyc_Absyn_Tvar*_tmp368;_LL20D: if(_tmp367 <= (void*)4)goto _LL20F;if(*((
int*)_tmp367)!= 1)goto _LL20F;_tmp368=((struct Cyc_Absyn_VarType_struct*)_tmp367)->f1;
_LL20E: {struct Cyc_CfFlowInfo_Region_k_struct _tmp5CE;struct Cyc_CfFlowInfo_Region_k_struct
v=(_tmp5CE.tag=0,((_tmp5CE.f1=_tmp368,_tmp5CE)));void*v2=(void*)& v;struct
_tuple25 _tmp5CF;struct _tuple25 env=(_tmp5CF.f1=fenv,((_tmp5CF.f2=fenv->mt_flowdict,((
_tmp5CF.f3=v2,_tmp5CF)))));((void(*)(void(*f)(struct _tuple25*,void*,void*),
struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,&
env,fd);return env.f2;}_LL20F: if((int)_tmp367 != 3)goto _LL211;_LL210: {struct
_tuple25 _tmp5D0;struct _tuple25 env=(_tmp5D0.f1=fenv,((_tmp5D0.f2=fenv->mt_flowdict,((
_tmp5D0.f3=(void*)0,_tmp5D0)))));((void(*)(void(*f)(struct _tuple25*,void*,void*),
struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,&
env,fd);return env.f2;}_LL211:;_LL212: {struct Cyc_Core_Impossible_struct _tmp5D6;
const char*_tmp5D5;struct Cyc_Core_Impossible_struct*_tmp5D4;(int)_throw((void*)((
_tmp5D4=_cycalloc(sizeof(*_tmp5D4)),((_tmp5D4[0]=((_tmp5D6.tag=Cyc_Core_Impossible,((
_tmp5D6.f1=((_tmp5D5="kill_flowdict_region",_tag_dyneither(_tmp5D5,sizeof(char),
21))),_tmp5D6)))),_tmp5D4)))));}_LL20C:;}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn);static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn){return 0;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);union
Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){union Cyc_CfFlowInfo_FlowInfo _tmp36F=
f;int _tmp370;struct _tuple12 _tmp371;struct Cyc_Dict_Dict _tmp372;struct Cyc_List_List*
_tmp373;struct Cyc_CfFlowInfo_ConsumeInfo _tmp374;_LL214: if((_tmp36F.BottomFL).tag
!= 1)goto _LL216;_tmp370=(int)(_tmp36F.BottomFL).val;_LL215: return f;_LL216: if((
_tmp36F.ReachableFL).tag != 2)goto _LL213;_tmp371=(struct _tuple12)(_tmp36F.ReachableFL).val;
_tmp372=_tmp371.f1;_tmp373=_tmp371.f2;_tmp374=_tmp371.f3;_LL217: {struct Cyc_Dict_Dict
_tmp375=Cyc_CfFlowInfo_kill_flowdict_region(fenv,_tmp372,rgn);struct Cyc_List_List*
_tmp376=Cyc_CfFlowInfo_kill_relns_region(_tmp373,rgn);return Cyc_CfFlowInfo_ReachableFL(
_tmp375,_tmp376,_tmp374);}_LL213:;}static struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of_rec(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds,struct Cyc_List_List*
path);static struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of_rec(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds,struct Cyc_List_List*
path){void*_tmp377=e->r;struct Cyc_Absyn_Exp*_tmp378;struct Cyc_Absyn_Exp*_tmp379;
struct Cyc_Absyn_Exp*_tmp37A;void*_tmp37B;struct Cyc_Absyn_Vardecl*_tmp37C;void*
_tmp37D;struct Cyc_Absyn_Vardecl*_tmp37E;void*_tmp37F;struct Cyc_Absyn_Vardecl*
_tmp380;void*_tmp381;struct Cyc_Absyn_Vardecl*_tmp382;struct Cyc_Absyn_Exp*_tmp383;
struct _dyneither_ptr*_tmp384;struct Cyc_Absyn_Exp*_tmp385;struct Cyc_Absyn_Exp*
_tmp386;struct Cyc_Absyn_Exp*_tmp387;struct Cyc_Absyn_Exp*_tmp388;struct
_dyneither_ptr*_tmp389;_LL219: if(*((int*)_tmp377)!= 15)goto _LL21B;_tmp378=((
struct Cyc_Absyn_Cast_e_struct*)_tmp377)->f2;_LL21A: _tmp379=_tmp378;goto _LL21C;
_LL21B: if(*((int*)_tmp377)!= 13)goto _LL21D;_tmp379=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp377)->f1;_LL21C: _tmp37A=_tmp379;goto _LL21E;_LL21D: if(*((int*)_tmp377)!= 14)
goto _LL21F;_tmp37A=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp377)->f1;_LL21E:
return Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp37A,flds,path);_LL21F: if(*((int*)
_tmp377)!= 1)goto _LL221;_tmp37B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp377)->f2;
if(_tmp37B <= (void*)1)goto _LL221;if(*((int*)_tmp37B)!= 2)goto _LL221;_tmp37C=((
struct Cyc_Absyn_Param_b_struct*)_tmp37B)->f1;_LL220: _tmp37E=_tmp37C;goto _LL222;
_LL221: if(*((int*)_tmp377)!= 1)goto _LL223;_tmp37D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp377)->f2;if(_tmp37D <= (void*)1)goto _LL223;if(*((int*)_tmp37D)!= 3)goto _LL223;
_tmp37E=((struct Cyc_Absyn_Local_b_struct*)_tmp37D)->f1;_LL222: _tmp380=_tmp37E;
goto _LL224;_LL223: if(*((int*)_tmp377)!= 1)goto _LL225;_tmp37F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp377)->f2;if(_tmp37F <= (void*)1)goto _LL225;if(*((int*)_tmp37F)!= 4)goto _LL225;
_tmp380=((struct Cyc_Absyn_Pat_b_struct*)_tmp37F)->f1;_LL224: {struct Cyc_CfFlowInfo_Place
_tmp5E0;struct Cyc_CfFlowInfo_VarRoot_struct _tmp5DF;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp5DE;struct Cyc_CfFlowInfo_UniquePlace*_tmp5DD;return(_tmp5DD=_region_malloc(r,
sizeof(*_tmp5DD)),((_tmp5DD->place=((_tmp5E0.root=(void*)((_tmp5DE=
_region_malloc(r,sizeof(*_tmp5DE)),((_tmp5DE[0]=((_tmp5DF.tag=0,((_tmp5DF.f1=
_tmp380,_tmp5DF)))),_tmp5DE)))),((_tmp5E0.fields=flds,_tmp5E0)))),((_tmp5DD->path=
path,_tmp5DD)))));}_LL225: if(*((int*)_tmp377)!= 1)goto _LL227;_tmp381=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp377)->f2;if(_tmp381 <= (void*)1)goto _LL227;if(*((
int*)_tmp381)!= 0)goto _LL227;_tmp382=((struct Cyc_Absyn_Global_b_struct*)_tmp381)->f1;
_LL226: {struct Cyc_Core_Impossible_struct _tmp5E6;const char*_tmp5E5;struct Cyc_Core_Impossible_struct*
_tmp5E4;(int)_throw((void*)((_tmp5E4=_cycalloc(sizeof(*_tmp5E4)),((_tmp5E4[0]=((
_tmp5E6.tag=Cyc_Core_Impossible,((_tmp5E6.f1=((_tmp5E5="non-unique path",
_tag_dyneither(_tmp5E5,sizeof(char),16))),_tmp5E6)))),_tmp5E4)))));}_LL227: if(*((
int*)_tmp377)!= 24)goto _LL229;_tmp383=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp377)->f1;_tmp384=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp377)->f2;_LL228:{
void*_tmp391=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp383->topt))->v);struct Cyc_Absyn_PtrInfo _tmp392;void*_tmp393;_LL232: if(
_tmp391 <= (void*)4)goto _LL234;if(*((int*)_tmp391)!= 4)goto _LL234;_tmp392=((
struct Cyc_Absyn_PointerType_struct*)_tmp391)->f1;_tmp393=_tmp392.elt_typ;_LL233:
if(!Cyc_Absyn_is_union_type(_tmp393)){struct Cyc_List_List*_tmp5E7;flds=((_tmp5E7=
_region_malloc(r,sizeof(*_tmp5E7)),((_tmp5E7->hd=(void*)Cyc_CfFlowInfo_get_field_index(
_tmp393,_tmp384),((_tmp5E7->tl=flds,_tmp5E7))))));}goto _LL231;_LL234:;_LL235: {
struct Cyc_Core_Impossible_struct _tmp5ED;const char*_tmp5EC;struct Cyc_Core_Impossible_struct*
_tmp5EB;(int)_throw((void*)((_tmp5EB=_cycalloc(sizeof(*_tmp5EB)),((_tmp5EB[0]=((
_tmp5ED.tag=Cyc_Core_Impossible,((_tmp5ED.f1=((_tmp5EC="anal_Rexp: AggrArrow ptr",
_tag_dyneither(_tmp5EC,sizeof(char),25))),_tmp5ED)))),_tmp5EB)))));}_LL231:;}
_tmp385=_tmp383;goto _LL22A;_LL229: if(*((int*)_tmp377)!= 22)goto _LL22B;_tmp385=((
struct Cyc_Absyn_Deref_e_struct*)_tmp377)->f1;_LL22A: {struct Cyc_List_List*
_tmp5EE;return Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp385,0,((_tmp5EE=
_region_malloc(r,sizeof(*_tmp5EE)),((_tmp5EE->hd=flds,((_tmp5EE->tl=path,_tmp5EE)))))));}
_LL22B: if(*((int*)_tmp377)!= 25)goto _LL22D;_tmp386=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp377)->f1;_tmp387=((struct Cyc_Absyn_Subscript_e_struct*)_tmp377)->f2;_LL22C: {
void*_tmp399=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp386->topt))->v);struct Cyc_Absyn_PtrInfo _tmp39A;void*_tmp39B;struct Cyc_Absyn_PtrAtts
_tmp39C;union Cyc_Absyn_Constraint*_tmp39D;_LL237: if(_tmp399 <= (void*)4)goto
_LL23B;if(*((int*)_tmp399)!= 9)goto _LL239;_LL238: {unsigned int _tmp39E=(Cyc_Evexp_eval_const_uint_exp(
_tmp387)).f1;struct Cyc_List_List*_tmp5EF;return Cyc_CfFlowInfo_unique_place_of_rec(
r,_tmp386,((_tmp5EF=_region_malloc(r,sizeof(*_tmp5EF)),((_tmp5EF->hd=(void*)((
int)_tmp39E),((_tmp5EF->tl=flds,_tmp5EF)))))),path);}_LL239: if(*((int*)_tmp399)
!= 4)goto _LL23B;_tmp39A=((struct Cyc_Absyn_PointerType_struct*)_tmp399)->f1;
_tmp39B=_tmp39A.elt_typ;_tmp39C=_tmp39A.ptr_atts;_tmp39D=_tmp39C.bounds;_LL23A: {
struct Cyc_Core_Impossible_struct _tmp5F5;const char*_tmp5F4;struct Cyc_Core_Impossible_struct*
_tmp5F3;(int)_throw((void*)((_tmp5F3=_cycalloc(sizeof(*_tmp5F3)),((_tmp5F3[0]=((
_tmp5F5.tag=Cyc_Core_Impossible,((_tmp5F5.f1=((_tmp5F4="subscripts unsupported for unique places",
_tag_dyneither(_tmp5F4,sizeof(char),41))),_tmp5F5)))),_tmp5F3)))));}_LL23B:;
_LL23C: {struct Cyc_Core_Impossible_struct _tmp5FB;const char*_tmp5FA;struct Cyc_Core_Impossible_struct*
_tmp5F9;(int)_throw((void*)((_tmp5F9=_cycalloc(sizeof(*_tmp5F9)),((_tmp5F9[0]=((
_tmp5FB.tag=Cyc_Core_Impossible,((_tmp5FB.f1=((_tmp5FA="unique place subscript -- bad type",
_tag_dyneither(_tmp5FA,sizeof(char),35))),_tmp5FB)))),_tmp5F9)))));}_LL236:;}
_LL22D: if(*((int*)_tmp377)!= 23)goto _LL22F;_tmp388=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp377)->f1;_tmp389=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp377)->f2;_LL22E:
if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp388->topt))->v)){
struct Cyc_Core_Impossible_struct _tmp601;const char*_tmp600;struct Cyc_Core_Impossible_struct*
_tmp5FF;(int)_throw((void*)((_tmp5FF=_cycalloc(sizeof(*_tmp5FF)),((_tmp5FF[0]=((
_tmp601.tag=Cyc_Core_Impossible,((_tmp601.f1=((_tmp600="unions can't contain unique places",
_tag_dyneither(_tmp600,sizeof(char),35))),_tmp601)))),_tmp5FF)))));}{int _tmp3A9=
Cyc_CfFlowInfo_get_field_index((void*)((struct Cyc_Core_Opt*)_check_null(_tmp388->topt))->v,
_tmp389);struct Cyc_List_List*_tmp602;return Cyc_CfFlowInfo_unique_place_of_rec(r,
_tmp388,((_tmp602=_region_malloc(r,sizeof(*_tmp602)),((_tmp602->hd=(void*)
_tmp3A9,((_tmp602->tl=flds,_tmp602)))))),path);}_LL22F:;_LL230: {struct Cyc_Core_Impossible_struct
_tmp608;const char*_tmp607;struct Cyc_Core_Impossible_struct*_tmp606;(int)_throw((
void*)((_tmp606=_cycalloc(sizeof(*_tmp606)),((_tmp606[0]=((_tmp608.tag=Cyc_Core_Impossible,((
_tmp608.f1=((_tmp607="bad unique path",_tag_dyneither(_tmp607,sizeof(char),16))),
_tmp608)))),_tmp606)))));}_LL218:;}struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_UniquePlace*
Cyc_CfFlowInfo_unique_place_of(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
return Cyc_CfFlowInfo_unique_place_of_rec(r,e,0,0);}struct _tuple26{struct Cyc_Dict_Dict*
f1;struct Cyc_Dict_Dict f2;struct Cyc_Position_Segment*f3;};static void Cyc_CfFlowInfo_consume_f(
struct _tuple26*env,struct Cyc_CfFlowInfo_Place*place);static void Cyc_CfFlowInfo_consume_f(
struct _tuple26*env,struct Cyc_CfFlowInfo_Place*place){struct _tuple26 _tmp3AF;
struct Cyc_Dict_Dict*_tmp3B0;struct Cyc_Dict_Dict _tmp3B1;struct Cyc_Position_Segment*
_tmp3B2;struct _tuple26*_tmp3AE=env;_tmp3AF=*_tmp3AE;_tmp3B0=_tmp3AF.f1;_tmp3B1=
_tmp3AF.f2;_tmp3B2=_tmp3AF.f3;{void*rval=(void*)0;if(Cyc_Dict_lookup_bool(
_tmp3B1,place->root,& rval) && rval == (void*)0)return;if(((int(*)(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)((
struct Cyc_Dict_Dict*)_tmp3B0,place,_tmp3B2)){struct Cyc_Position_Segment*_tmp3B3=((
struct Cyc_Position_Segment*(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))
Cyc_Dict_lookup)(*_tmp3B0,place);const char*_tmp60A;const char*_tmp609;((void(*)(
struct Cyc_CfFlowInfo_Place*place,struct _dyneither_ptr(*toString)(struct Cyc_CfFlowInfo_Place*),
struct _dyneither_ptr err_msg1,struct _dyneither_ptr err_msg2,struct Cyc_Position_Segment*
consumed_loc,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_unique_err)(place,
Cyc_CfFlowInfo_place_err_string,((_tmp609="May consume non-aliased pointer %s more than once (cf. %s)",
_tag_dyneither(_tmp609,sizeof(char),59))),((_tmp60A="May consume non-aliased pointer %s more than once",
_tag_dyneither(_tmp60A,sizeof(char),50))),_tmp3B3,_tmp3B2);}}}union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo _tmp3B6=f;int
_tmp3B7;struct _tuple12 _tmp3B8;struct Cyc_Dict_Dict _tmp3B9;struct Cyc_List_List*
_tmp3BA;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3BB;_LL23E: if((_tmp3B6.BottomFL).tag
!= 1)goto _LL240;_tmp3B7=(int)(_tmp3B6.BottomFL).val;_LL23F: return f;_LL240: if((
_tmp3B6.ReachableFL).tag != 2)goto _LL23D;_tmp3B8=(struct _tuple12)(_tmp3B6.ReachableFL).val;
_tmp3B9=_tmp3B8.f1;_tmp3BA=_tmp3B8.f2;_tmp3BB=_tmp3B8.f3;_LL241: if(_tmp3BB.may_consume
== 0)return f;{struct Cyc_Dict_Dict _tmp3BC=_tmp3BB.consumed;struct _tuple26 _tmp60B;
struct _tuple26 _tmp3BD=(_tmp60B.f1=& _tmp3BC,((_tmp60B.f2=_tmp3B9,((_tmp60B.f3=loc,
_tmp60B)))));((void(*)(void(*f)(struct _tuple26*,struct Cyc_CfFlowInfo_Place*),
struct _tuple26*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_CfFlowInfo_consume_f,&
_tmp3BD,_tmp3BB.may_consume);{struct Cyc_CfFlowInfo_ConsumeInfo _tmp60C;return Cyc_CfFlowInfo_ReachableFL(
_tmp3B9,_tmp3BA,((_tmp60C.consumed=_tmp3BC,((_tmp60C.may_consume=0,_tmp60C)))));}}
_LL23D:;}void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,
union Cyc_CfFlowInfo_FlowInfo f);void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo _tmp3C0=f;int
_tmp3C1;struct _tuple12 _tmp3C2;struct Cyc_Dict_Dict _tmp3C3;struct Cyc_List_List*
_tmp3C4;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3C5;_LL243: if((_tmp3C0.BottomFL).tag
!= 1)goto _LL245;_tmp3C1=(int)(_tmp3C0.BottomFL).val;_LL244: return;_LL245: if((
_tmp3C0.ReachableFL).tag != 2)goto _LL242;_tmp3C2=(struct _tuple12)(_tmp3C0.ReachableFL).val;
_tmp3C3=_tmp3C2.f1;_tmp3C4=_tmp3C2.f2;_tmp3C5=_tmp3C2.f3;_LL246: {struct Cyc_List_List*
_tmp3C6=_tmp3C5.may_consume;while(_tmp3C6 != 0){if(((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp3C5.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp3C6->hd)){struct Cyc_Position_Segment*_tmp3C7=((struct Cyc_Position_Segment*(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_lookup)(_tmp3C5.consumed,(
struct Cyc_CfFlowInfo_Place*)_tmp3C6->hd);{const char*_tmp60E;const char*_tmp60D;((
void(*)(struct Cyc_CfFlowInfo_Place*place,struct _dyneither_ptr(*toString)(struct
Cyc_CfFlowInfo_Place*),struct _dyneither_ptr err_msg1,struct _dyneither_ptr err_msg2,
struct Cyc_Position_Segment*consumed_loc,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_unique_err)((
struct Cyc_CfFlowInfo_Place*)_tmp3C6->hd,Cyc_CfFlowInfo_place_err_string,((
_tmp60D="Read through possibly consumed unique pointer %s (cf. %s)",
_tag_dyneither(_tmp60D,sizeof(char),58))),((_tmp60E="Read through possibly consumed unique pointer %s",
_tag_dyneither(_tmp60E,sizeof(char),49))),_tmp3C7,loc);}break;}_tmp3C6=_tmp3C6->tl;}
return;}_LL242:;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_drop_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo f);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f){union Cyc_CfFlowInfo_FlowInfo _tmp3CA=f;int _tmp3CB;struct _tuple12 _tmp3CC;struct
Cyc_Dict_Dict _tmp3CD;struct Cyc_List_List*_tmp3CE;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3CF;_LL248: if((_tmp3CA.BottomFL).tag != 1)goto _LL24A;_tmp3CB=(int)(_tmp3CA.BottomFL).val;
_LL249: return f;_LL24A: if((_tmp3CA.ReachableFL).tag != 2)goto _LL247;_tmp3CC=(
struct _tuple12)(_tmp3CA.ReachableFL).val;_tmp3CD=_tmp3CC.f1;_tmp3CE=_tmp3CC.f2;
_tmp3CF=_tmp3CC.f3;_LL24B: _tmp3CF.may_consume=0;return Cyc_CfFlowInfo_ReachableFL(
_tmp3CD,_tmp3CE,_tmp3CF);_LL247:;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_readthrough_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo f);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f){Cyc_CfFlowInfo_check_unique_rvals(loc,f);{union Cyc_CfFlowInfo_FlowInfo _tmp3D0=
f;int _tmp3D1;struct _tuple12 _tmp3D2;struct Cyc_Dict_Dict _tmp3D3;struct Cyc_List_List*
_tmp3D4;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3D5;_LL24D: if((_tmp3D0.BottomFL).tag
!= 1)goto _LL24F;_tmp3D1=(int)(_tmp3D0.BottomFL).val;_LL24E: return f;_LL24F: if((
_tmp3D0.ReachableFL).tag != 2)goto _LL24C;_tmp3D2=(struct _tuple12)(_tmp3D0.ReachableFL).val;
_tmp3D3=_tmp3D2.f1;_tmp3D4=_tmp3D2.f2;_tmp3D5=_tmp3D2.f3;_LL250: _tmp3D5.may_consume=
0;return Cyc_CfFlowInfo_ReachableFL(_tmp3D3,_tmp3D4,_tmp3D5);_LL24C:;}}struct
_tuple14 Cyc_CfFlowInfo_save_consume_info(struct Cyc_CfFlowInfo_FlowEnv*fenv,union
Cyc_CfFlowInfo_FlowInfo f,int clear);struct _tuple14 Cyc_CfFlowInfo_save_consume_info(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,int clear){union
Cyc_CfFlowInfo_FlowInfo _tmp3D6=f;int _tmp3D7;struct _tuple12 _tmp3D8;struct Cyc_Dict_Dict
_tmp3D9;struct Cyc_List_List*_tmp3DA;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3DB;
_LL252: if((_tmp3D6.BottomFL).tag != 1)goto _LL254;_tmp3D7=(int)(_tmp3D6.BottomFL).val;
_LL253: {struct Cyc_CfFlowInfo_ConsumeInfo _tmp611;struct _tuple14 _tmp610;return(
_tmp610.f1=((_tmp611.consumed=fenv->mt_place_set,((_tmp611.may_consume=0,_tmp611)))),((
_tmp610.f2=f,_tmp610)));}_LL254: if((_tmp3D6.ReachableFL).tag != 2)goto _LL251;
_tmp3D8=(struct _tuple12)(_tmp3D6.ReachableFL).val;_tmp3D9=_tmp3D8.f1;_tmp3DA=
_tmp3D8.f2;_tmp3DB=_tmp3D8.f3;_LL255: if(clear){struct Cyc_CfFlowInfo_ConsumeInfo
_tmp614;struct _tuple14 _tmp613;return(_tmp613.f1=_tmp3DB,((_tmp613.f2=Cyc_CfFlowInfo_ReachableFL(
_tmp3D9,_tmp3DA,((_tmp614.consumed=fenv->mt_place_set,((_tmp614.may_consume=0,
_tmp614))))),_tmp613)));}else{struct _tuple14 _tmp615;return(_tmp615.f1=_tmp3DB,((
_tmp615.f2=f,_tmp615)));}_LL251:;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_restore_consume_info(
union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_CfFlowInfo_ConsumeInfo c,int
may_consume_only);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_restore_consume_info(
union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_CfFlowInfo_ConsumeInfo c,int
may_consume_only){union Cyc_CfFlowInfo_FlowInfo _tmp3E1=f;int _tmp3E2;struct
_tuple12 _tmp3E3;struct Cyc_Dict_Dict _tmp3E4;struct Cyc_List_List*_tmp3E5;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3E6;_LL257: if((_tmp3E1.BottomFL).tag != 1)goto _LL259;_tmp3E2=(int)(_tmp3E1.BottomFL).val;
_LL258: return f;_LL259: if((_tmp3E1.ReachableFL).tag != 2)goto _LL256;_tmp3E3=(
struct _tuple12)(_tmp3E1.ReachableFL).val;_tmp3E4=_tmp3E3.f1;_tmp3E5=_tmp3E3.f2;
_tmp3E6=_tmp3E3.f3;_LL25A: if(may_consume_only){struct Cyc_CfFlowInfo_ConsumeInfo
_tmp616;return Cyc_CfFlowInfo_ReachableFL(_tmp3E4,_tmp3E5,((_tmp616.consumed=
_tmp3E6.consumed,((_tmp616.may_consume=c.may_consume,_tmp616)))));}return Cyc_CfFlowInfo_ReachableFL(
_tmp3E4,_tmp3E5,c);_LL256:;}
