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
Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);int Cyc_Absyn_is_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
struct _tuple4{unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_UniquePlace{struct
Cyc_CfFlowInfo_Place place;struct Cyc_List_List*path;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct Cyc_CfFlowInfo_LessNumelts_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqNumelts_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};union Cyc_CfFlowInfo_RelnOp_union{struct Cyc_CfFlowInfo_EqualConst_struct
EqualConst;struct Cyc_CfFlowInfo_LessVar_struct LessVar;struct Cyc_CfFlowInfo_LessNumelts_struct
LessNumelts;struct Cyc_CfFlowInfo_LessConst_struct LessConst;struct Cyc_CfFlowInfo_LessEqNumelts_struct
LessEqNumelts;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp_union
rop;};struct Cyc_CfFlowInfo_TagCmp{void*cmp;void*bd;};char Cyc_CfFlowInfo_HasTagCmps[
15]="\000\000\000\000HasTagCmps\000";struct Cyc_CfFlowInfo_HasTagCmps_struct{char*
tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_IsZero[11]="\000\000\000\000IsZero\000";
char Cyc_CfFlowInfo_NotZero[12]="\000\000\000\000NotZero\000";struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[13]="\000\000\000\000UnknownZ\000";
struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};struct Cyc_List_List*
Cyc_CfFlowInfo_copy_tagcmps(struct _RegionHandle*,struct Cyc_List_List*);struct Cyc_CfFlowInfo_PlaceL_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UnknownL_struct{int
tag;};union Cyc_CfFlowInfo_AbsLVal_union{struct Cyc_CfFlowInfo_PlaceL_struct PlaceL;
struct Cyc_CfFlowInfo_UnknownL_struct UnknownL;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct
_dyneither_ptr f1;};int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,
void*place,struct Cyc_Position_Segment*loc);int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_equals(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_uplace_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_CfFlowInfo_and_consume(struct _RegionHandle*,struct Cyc_CfFlowInfo_ConsumeInfo
c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);int Cyc_CfFlowInfo_consume_approx(struct
Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*
f);int Cyc_CfFlowInfo_root_cmp(void*,void*);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(
struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(union
Cyc_CfFlowInfo_FlowInfo_union f1,union Cyc_CfFlowInfo_FlowInfo_union f2);void Cyc_CfFlowInfo_print_absrval(
void*rval);void Cyc_CfFlowInfo_print_initlevel(void*il);void Cyc_CfFlowInfo_print_root(
void*root);void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_dict_set(
struct Cyc_Dict_Dict p,void(*pr)(void*));void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*
p,void(*pr)(void*));void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);void
Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo_union f);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct
Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,
struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(
struct Cyc_List_List*,struct Cyc_List_List*);void Cyc_CfFlowInfo_print_relns(struct
Cyc_List_List*);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_join_flow(struct
Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo_union,
union Cyc_CfFlowInfo_FlowInfo_union,int);struct _tuple5{union Cyc_CfFlowInfo_FlowInfo_union
f1;void*f2;};struct _tuple5 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*all_changed,struct _tuple5 pr1,struct _tuple5 pr2,int);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,
union Cyc_CfFlowInfo_FlowInfo_union,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Dict_Dict,
struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union f,void*rgn);
struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_consume_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);void Cyc_CfFlowInfo_check_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo_union f);struct _tuple6{struct Cyc_CfFlowInfo_ConsumeInfo
f1;union Cyc_CfFlowInfo_FlowInfo_union f2;};struct _tuple6 Cyc_CfFlowInfo_save_consume_info(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo_union f,int clear);
union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo_union
f,struct Cyc_CfFlowInfo_ConsumeInfo c,int may_consume_only);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);int Cyc_CfFlowInfo_place_cmp(struct
Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_root_cmp(
void*r1,void*r2);struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r);struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r){static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,(union Cyc_Absyn_Cnst_union)((
struct Cyc_Absyn_Null_c_struct){6})};static struct Cyc_Absyn_Exp dummy_exp={0,(void*)((
void*)& dummy_rawexp),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3DD;struct Cyc_CfFlowInfo_UnknownR_struct _tmp3DC;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3DB;struct Cyc_CfFlowInfo_UnknownR_struct _tmp3DA;struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3D9;struct Cyc_CfFlowInfo_UnknownR_struct _tmp3D8;struct Cyc_CfFlowInfo_Esc_struct*
_tmp3D7;struct Cyc_CfFlowInfo_Esc_struct _tmp3D6;struct Cyc_CfFlowInfo_Esc_struct*
_tmp3D5;struct Cyc_CfFlowInfo_Esc_struct _tmp3D4;struct Cyc_CfFlowInfo_Esc_struct*
_tmp3D3;struct Cyc_CfFlowInfo_Esc_struct _tmp3D2;struct Cyc_CfFlowInfo_Place*
_tmp3D1;struct Cyc_CfFlowInfo_MallocPt_struct _tmp3D0;struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp3CF;struct Cyc_CfFlowInfo_FlowEnv*_tmp3CE;return(_tmp3CE=_region_malloc(r,
sizeof(*_tmp3CE)),((_tmp3CE->r=r,((_tmp3CE->unknown_none=(void*)((void*)((
_tmp3DD=_region_malloc(r,sizeof(*_tmp3DD)),((_tmp3DD[0]=((_tmp3DC.tag=0,((
_tmp3DC.f1=(void*)((void*)0),_tmp3DC)))),_tmp3DD))))),((_tmp3CE->unknown_this=(
void*)((void*)((_tmp3DB=_region_malloc(r,sizeof(*_tmp3DB)),((_tmp3DB[0]=((
_tmp3DA.tag=0,((_tmp3DA.f1=(void*)((void*)1),_tmp3DA)))),_tmp3DB))))),((_tmp3CE->unknown_all=(
void*)((void*)((_tmp3D9=_region_malloc(r,sizeof(*_tmp3D9)),((_tmp3D9[0]=((
_tmp3D8.tag=0,((_tmp3D8.f1=(void*)((void*)2),_tmp3D8)))),_tmp3D9))))),((_tmp3CE->esc_none=(
void*)((void*)((_tmp3D7=_region_malloc(r,sizeof(*_tmp3D7)),((_tmp3D7[0]=((
_tmp3D6.tag=1,((_tmp3D6.f1=(void*)((void*)0),_tmp3D6)))),_tmp3D7))))),((_tmp3CE->esc_this=(
void*)((void*)((_tmp3D5=_region_malloc(r,sizeof(*_tmp3D5)),((_tmp3D5[0]=((
_tmp3D4.tag=1,((_tmp3D4.f1=(void*)((void*)1),_tmp3D4)))),_tmp3D5))))),((_tmp3CE->esc_all=(
void*)((void*)((_tmp3D3=_region_malloc(r,sizeof(*_tmp3D3)),((_tmp3D3[0]=((
_tmp3D2.tag=1,((_tmp3D2.f1=(void*)((void*)2),_tmp3D2)))),_tmp3D3))))),((_tmp3CE->mt_flowdict=
Cyc_Dict_rempty(r,Cyc_CfFlowInfo_root_cmp),((_tmp3CE->mt_place_set=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))
Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp3CE->dummy_place=((_tmp3D1=
_region_malloc(r,sizeof(*_tmp3D1)),((_tmp3D1->root=(void*)((void*)((_tmp3CF=
_region_malloc(r,sizeof(*_tmp3CF)),((_tmp3CF[0]=((_tmp3D0.tag=1,((_tmp3D0.f1=&
dummy_exp,((_tmp3D0.f2=(void*)((void*)0),_tmp3D0)))))),_tmp3CF))))),((_tmp3D1->fields=
0,_tmp3D1)))))),_tmp3CE)))))))))))))))))))));}int Cyc_CfFlowInfo_update_place_set(
struct Cyc_Dict_Dict*set,void*place,struct Cyc_Position_Segment*loc);int Cyc_CfFlowInfo_update_place_set(
struct Cyc_Dict_Dict*set,void*place,struct Cyc_Position_Segment*loc){if(set != 0){
if(((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(*set,place))return 1;
else{*set=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Position_Segment*
v))Cyc_Dict_insert)(*set,place,loc);}}return 0;}struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place){void*_tmp13;struct Cyc_List_List*_tmp14;struct
Cyc_CfFlowInfo_Place _tmp12=*place;_tmp13=(void*)_tmp12.root;_tmp14=_tmp12.fields;{
void*_tmp15=_tmp13;struct Cyc_Absyn_Vardecl*_tmp16;_LL1: if(*((int*)_tmp15)!= 0)
goto _LL3;_tmp16=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp15)->f1;_LL2: if(
_tmp14 == 0)return Cyc_Absynpp_qvar2string(_tmp16->name);else{const char*_tmp3E1;
void*_tmp3E0[1];struct Cyc_String_pa_struct _tmp3DF;return(struct _dyneither_ptr)((
_tmp3DF.tag=0,((_tmp3DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp16->name)),((_tmp3E0[0]=& _tmp3DF,Cyc_aprintf(((_tmp3E1="reachable from %s",
_tag_dyneither(_tmp3E1,sizeof(char),18))),_tag_dyneither(_tmp3E0,sizeof(void*),1))))))));}
_LL3:;_LL4: {const char*_tmp3E4;void*_tmp3E3;(_tmp3E3=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3E4="error locations not for VarRoots",
_tag_dyneither(_tmp3E4,sizeof(char),33))),_tag_dyneither(_tmp3E3,sizeof(void*),0)));}
_LL0:;}}struct _dyneither_ptr Cyc_CfFlowInfo_uplace_err_string(struct Cyc_CfFlowInfo_UniquePlace*
place);struct _dyneither_ptr Cyc_CfFlowInfo_uplace_err_string(struct Cyc_CfFlowInfo_UniquePlace*
place){return Cyc_CfFlowInfo_place_err_string(& place->place);}static void Cyc_CfFlowInfo_unique_err(
void*place,struct _dyneither_ptr(*toString)(void*),struct _dyneither_ptr err_msg1,
struct _dyneither_ptr err_msg2,struct Cyc_Position_Segment*consumed_loc,struct Cyc_Position_Segment*
loc);static void Cyc_CfFlowInfo_unique_err(void*place,struct _dyneither_ptr(*
toString)(void*),struct _dyneither_ptr err_msg1,struct _dyneither_ptr err_msg2,
struct Cyc_Position_Segment*consumed_loc,struct Cyc_Position_Segment*loc){struct
_dyneither_ptr _tmp1C=toString(place);if(!Cyc_Position_segment_equals(
consumed_loc,loc)){struct _dyneither_ptr _tmp1D=Cyc_Position_string_of_segment(
consumed_loc);void*_tmp3E7[2];struct Cyc_String_pa_struct _tmp3E6;struct Cyc_String_pa_struct
_tmp3E5;(_tmp3E5.tag=0,((_tmp3E5.f1=(struct _dyneither_ptr)_tmp1D,((_tmp3E6.tag=0,((
_tmp3E6.f1=(struct _dyneither_ptr)_tmp1C,((_tmp3E7[0]=& _tmp3E6,((_tmp3E7[1]=&
_tmp3E5,Cyc_Tcutil_terr(loc,err_msg1,_tag_dyneither(_tmp3E7,sizeof(void*),2)))))))))))));}
else{void*_tmp3E9[1];struct Cyc_String_pa_struct _tmp3E8;(_tmp3E8.tag=0,((_tmp3E8.f1=(
struct _dyneither_ptr)_tmp1C,((_tmp3E9[0]=& _tmp3E8,Cyc_Tcutil_terr(loc,err_msg2,
_tag_dyneither(_tmp3E9,sizeof(void*),1)))))));}}struct _tuple7{int f1;struct
_dyneither_ptr(*f2)(void*);};static struct Cyc_Position_Segment*Cyc_CfFlowInfo_combine_consume_f(
struct _tuple7*env,void*place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*
loc2);static struct Cyc_Position_Segment*Cyc_CfFlowInfo_combine_consume_f(struct
_tuple7*env,void*place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*
loc2){struct _tuple7 _tmp24;int _tmp25;struct _dyneither_ptr(*_tmp26)(void*);struct
_tuple7*_tmp23=env;_tmp24=*_tmp23;_tmp25=_tmp24.f1;_tmp26=_tmp24.f2;if(_tmp25){
const char*_tmp3EB;const char*_tmp3EA;Cyc_CfFlowInfo_unique_err(place,_tmp26,((
_tmp3EA="May consume non-aliased pointer %s more than once (cf. %s)",
_tag_dyneither(_tmp3EA,sizeof(char),59))),((_tmp3EB="May consume non-aliased pointer %s more than once",
_tag_dyneither(_tmp3EB,sizeof(char),50))),loc1,loc2);}return loc1;}struct _tuple8{
int f1;struct _dyneither_ptr(*f2)(struct Cyc_CfFlowInfo_Place*place);};struct Cyc_Dict_Dict
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int
disjoint);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2,int disjoint){struct _tuple8 _tmp3EC;struct _tuple8 _tmp29=(
_tmp3EC.f1=disjoint,((_tmp3EC.f2=Cyc_CfFlowInfo_place_err_string,_tmp3EC)));
struct Cyc_Dict_Dict _tmp2A=((struct Cyc_Dict_Dict(*)(struct Cyc_Position_Segment*(*
f)(struct _tuple8*,struct Cyc_CfFlowInfo_Place*,struct Cyc_Position_Segment*,struct
Cyc_Position_Segment*),struct _tuple8*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_union_two_c)((struct Cyc_Position_Segment*(*)(struct _tuple8*env,
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*
loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp29,s1,s2);return _tmp2A;}struct
_tuple9{int f1;struct _dyneither_ptr(*f2)(struct Cyc_CfFlowInfo_UniquePlace*place);
};struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_uplace_set(struct Cyc_Dict_Dict s1,
struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_uplace_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint){struct _tuple9 _tmp3ED;
struct _tuple9 _tmp2C=(_tmp3ED.f1=disjoint,((_tmp3ED.f2=Cyc_CfFlowInfo_uplace_err_string,
_tmp3ED)));return((struct Cyc_Dict_Dict(*)(struct Cyc_Position_Segment*(*f)(struct
_tuple9*,struct Cyc_CfFlowInfo_UniquePlace*,struct Cyc_Position_Segment*,struct Cyc_Position_Segment*),
struct _tuple9*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)((
struct Cyc_Position_Segment*(*)(struct _tuple9*env,struct Cyc_CfFlowInfo_UniquePlace*
place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*loc2))Cyc_CfFlowInfo_combine_consume_f,&
_tmp2C,s1,s2);}struct _tuple10{void*f1;struct Cyc_Position_Segment*f2;};int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){if((int)s1.t == (int)s2.t)return 1;{
struct _handler_cons _tmp2E;_push_handler(& _tmp2E);{int _tmp30=0;if(setjmp(_tmp2E.handler))
_tmp30=1;if(!_tmp30){{struct _RegionHandle _tmp31=_new_region("temp");struct
_RegionHandle*temp=& _tmp31;_push_region(temp);{struct Cyc_Iter_Iter _tmp32=((
struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
temp,s1);struct _tuple10 _tmp33=*((struct _tuple10*(*)(struct _RegionHandle*r,struct
Cyc_Dict_Dict d))Cyc_Dict_rchoose)(temp,s1);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple10*))Cyc_Iter_next)(_tmp32,& _tmp33)){void*_tmp34=_tmp33.f1;if(!((int(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(s2,_tmp34)){int _tmp35=0;
_npop_handler(1);return _tmp35;}}{int _tmp36=1;_npop_handler(1);return _tmp36;}};
_pop_region(temp);};_pop_handler();}else{void*_tmp2F=(void*)_exn_thrown;void*
_tmp38=_tmp2F;_LL6:;_LL7: return 1;_LL8:;_LL9:(void)_throw(_tmp38);_LL5:;}}}}int
Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);int
Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(
s2,s1);}int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct
_dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,
struct _dyneither_ptr*f){int n=0;for(0;(unsigned int)fs;fs=fs->tl){struct
_dyneither_ptr*_tmp39=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;if(Cyc_strptrcmp(
_tmp39,f)== 0)return n;n=n + 1;}{struct Cyc_Core_Impossible_struct _tmp3FA;const char*
_tmp3F9;void*_tmp3F8[1];struct Cyc_String_pa_struct _tmp3F7;struct Cyc_Core_Impossible_struct*
_tmp3F6;(int)_throw((void*)((_tmp3F6=_cycalloc(sizeof(*_tmp3F6)),((_tmp3F6[0]=((
_tmp3FA.tag=Cyc_Core_Impossible,((_tmp3FA.f1=(struct _dyneither_ptr)((_tmp3F7.tag=
0,((_tmp3F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp3F8[0]=&
_tmp3F7,Cyc_aprintf(((_tmp3F9="get_field_index_fs failed: %s",_tag_dyneither(
_tmp3F9,sizeof(char),30))),_tag_dyneither(_tmp3F8,sizeof(void*),1)))))))),
_tmp3FA)))),_tmp3F6)))));}}int Cyc_CfFlowInfo_get_field_index(void*t,struct
_dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*
f){void*_tmp3F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp40;union Cyc_Absyn_AggrInfoU_union
_tmp41;struct Cyc_List_List*_tmp42;_LLB: if(_tmp3F <= (void*)4)goto _LLF;if(*((int*)
_tmp3F)!= 10)goto _LLD;_tmp40=((struct Cyc_Absyn_AggrType_struct*)_tmp3F)->f1;
_tmp41=_tmp40.aggr_info;_LLC: {struct Cyc_List_List*_tmp43=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp41))->impl))->fields;_tmp42=_tmp43;
goto _LLE;}_LLD: if(*((int*)_tmp3F)!= 11)goto _LLF;_tmp42=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp3F)->f2;_LLE: return Cyc_CfFlowInfo_get_field_index_fs(_tmp42,f);_LLF:;_LL10: {
struct Cyc_Core_Impossible_struct _tmp407;const char*_tmp406;void*_tmp405[1];struct
Cyc_String_pa_struct _tmp404;struct Cyc_Core_Impossible_struct*_tmp403;(int)_throw((
void*)((_tmp403=_cycalloc(sizeof(*_tmp403)),((_tmp403[0]=((_tmp407.tag=Cyc_Core_Impossible,((
_tmp407.f1=(struct _dyneither_ptr)((_tmp404.tag=0,((_tmp404.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp405[0]=&
_tmp404,Cyc_aprintf(((_tmp406="get_field_index failed: %s",_tag_dyneither(
_tmp406,sizeof(char),27))),_tag_dyneither(_tmp405,sizeof(void*),1)))))))),
_tmp407)))),_tmp403)))));}_LLA:;}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);int
Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)r1 == (int)r2)return 0;{struct
_tuple0 _tmp408;struct _tuple0 _tmp4A=(_tmp408.f1=r1,((_tmp408.f2=r2,_tmp408)));
void*_tmp4B;struct Cyc_Absyn_Vardecl*_tmp4C;void*_tmp4D;struct Cyc_Absyn_Vardecl*
_tmp4E;void*_tmp4F;void*_tmp50;void*_tmp51;struct Cyc_Absyn_Exp*_tmp52;void*
_tmp53;struct Cyc_Absyn_Exp*_tmp54;void*_tmp55;void*_tmp56;void*_tmp57;int _tmp58;
void*_tmp59;int _tmp5A;_LL12: _tmp4B=_tmp4A.f1;if(*((int*)_tmp4B)!= 0)goto _LL14;
_tmp4C=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp4B)->f1;_tmp4D=_tmp4A.f2;if(*((
int*)_tmp4D)!= 0)goto _LL14;_tmp4E=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp4D)->f1;
_LL13: return(int)_tmp4C - (int)_tmp4E;_LL14: _tmp4F=_tmp4A.f1;if(*((int*)_tmp4F)!= 
0)goto _LL16;_LL15: return - 1;_LL16: _tmp50=_tmp4A.f2;if(*((int*)_tmp50)!= 0)goto
_LL18;_LL17: return 1;_LL18: _tmp51=_tmp4A.f1;if(*((int*)_tmp51)!= 1)goto _LL1A;
_tmp52=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp51)->f1;_tmp53=_tmp4A.f2;if(*((
int*)_tmp53)!= 1)goto _LL1A;_tmp54=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp53)->f1;
_LL19: return(int)_tmp52 - (int)_tmp54;_LL1A: _tmp55=_tmp4A.f1;if(*((int*)_tmp55)!= 
1)goto _LL1C;_LL1B: return - 1;_LL1C: _tmp56=_tmp4A.f2;if(*((int*)_tmp56)!= 1)goto
_LL1E;_LL1D: return 1;_LL1E: _tmp57=_tmp4A.f1;if(*((int*)_tmp57)!= 2)goto _LL11;
_tmp58=((struct Cyc_CfFlowInfo_InitParam_struct*)_tmp57)->f1;_tmp59=_tmp4A.f2;if(*((
int*)_tmp59)!= 2)goto _LL11;_tmp5A=((struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp59)->f1;_LL1F: return _tmp58 - _tmp5A;_LL11:;}}static int Cyc_CfFlowInfo_intlist_cmp(
struct Cyc_List_List*list1,struct Cyc_List_List*list2);static int Cyc_CfFlowInfo_intlist_cmp(
struct Cyc_List_List*list1,struct Cyc_List_List*list2){return((int(*)(int(*cmp)(
int,int),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,
list1,list2);}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct
Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*
p1,struct Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)p2)return 0;{void*_tmp5C;
struct Cyc_List_List*_tmp5D;struct Cyc_CfFlowInfo_Place _tmp5B=*p1;_tmp5C=(void*)
_tmp5B.root;_tmp5D=_tmp5B.fields;{void*_tmp5F;struct Cyc_List_List*_tmp60;struct
Cyc_CfFlowInfo_Place _tmp5E=*p2;_tmp5F=(void*)_tmp5E.root;_tmp60=_tmp5E.fields;{
int i=Cyc_CfFlowInfo_root_cmp(_tmp5C,_tmp5F);if(i != 0)return i;return Cyc_CfFlowInfo_intlist_cmp(
_tmp5D,_tmp60);}}}}int Cyc_CfFlowInfo_uplace_cmp(struct Cyc_CfFlowInfo_UniquePlace*
p1,struct Cyc_CfFlowInfo_UniquePlace*p2);int Cyc_CfFlowInfo_uplace_cmp(struct Cyc_CfFlowInfo_UniquePlace*
p1,struct Cyc_CfFlowInfo_UniquePlace*p2){if((int)p1 == (int)p2)return 0;{struct Cyc_CfFlowInfo_Place
_tmp62;struct Cyc_List_List*_tmp63;struct Cyc_CfFlowInfo_UniquePlace _tmp61=*p1;
_tmp62=_tmp61.place;_tmp63=_tmp61.path;{struct Cyc_CfFlowInfo_Place _tmp65;struct
Cyc_List_List*_tmp66;struct Cyc_CfFlowInfo_UniquePlace _tmp64=*p2;_tmp65=_tmp64.place;
_tmp66=_tmp64.path;{int i=Cyc_CfFlowInfo_place_cmp(& _tmp62,& _tmp65);if(i != 0)
return i;return((int(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_CfFlowInfo_intlist_cmp,
_tmp63,_tmp66);}}}}static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(struct
Cyc_CfFlowInfo_Place*p);static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(
struct Cyc_CfFlowInfo_Place*p){struct Cyc_List_List*sl=0;void*_tmp68;struct Cyc_List_List*
_tmp69;struct Cyc_CfFlowInfo_Place _tmp67=*p;_tmp68=(void*)_tmp67.root;_tmp69=
_tmp67.fields;{void*_tmp6A=_tmp68;struct Cyc_Absyn_Vardecl*_tmp6B;struct Cyc_Absyn_Exp*
_tmp6C;int _tmp6D;_LL21: if(*((int*)_tmp6A)!= 0)goto _LL23;_tmp6B=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp6A)->f1;_LL22:{struct _dyneither_ptr*_tmp415;const char*_tmp414;void*_tmp413[1];
struct Cyc_String_pa_struct _tmp412;struct Cyc_List_List*_tmp411;sl=((_tmp411=
_cycalloc(sizeof(*_tmp411)),((_tmp411->hd=((_tmp415=_cycalloc(sizeof(*_tmp415)),((
_tmp415[0]=((_tmp412.tag=0,((_tmp412.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*(*_tmp6B->name).f2),((_tmp413[0]=& _tmp412,Cyc_aprintf(((_tmp414="%s",
_tag_dyneither(_tmp414,sizeof(char),3))),_tag_dyneither(_tmp413,sizeof(void*),1)))))))),
_tmp415)))),((_tmp411->tl=sl,_tmp411))))));}goto _LL20;_LL23: if(*((int*)_tmp6A)!= 
1)goto _LL25;_tmp6C=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp6A)->f1;_LL24:{
struct _dyneither_ptr*_tmp422;const char*_tmp421;void*_tmp420[1];struct Cyc_Int_pa_struct
_tmp41F;struct Cyc_List_List*_tmp41E;sl=((_tmp41E=_cycalloc(sizeof(*_tmp41E)),((
_tmp41E->hd=((_tmp422=_cycalloc(sizeof(*_tmp422)),((_tmp422[0]=((_tmp41F.tag=1,((
_tmp41F.f1=(unsigned long)((int)_tmp6C),((_tmp420[0]=& _tmp41F,Cyc_aprintf(((
_tmp421="mpt%d",_tag_dyneither(_tmp421,sizeof(char),6))),_tag_dyneither(_tmp420,
sizeof(void*),1)))))))),_tmp422)))),((_tmp41E->tl=sl,_tmp41E))))));}goto _LL20;
_LL25: if(*((int*)_tmp6A)!= 2)goto _LL20;_tmp6D=((struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp6A)->f1;_LL26:{struct _dyneither_ptr*_tmp42F;const char*_tmp42E;void*_tmp42D[1];
struct Cyc_Int_pa_struct _tmp42C;struct Cyc_List_List*_tmp42B;sl=((_tmp42B=
_cycalloc(sizeof(*_tmp42B)),((_tmp42B->hd=((_tmp42F=_cycalloc(sizeof(*_tmp42F)),((
_tmp42F[0]=((_tmp42C.tag=1,((_tmp42C.f1=(unsigned long)_tmp6D,((_tmp42D[0]=&
_tmp42C,Cyc_aprintf(((_tmp42E="param%d",_tag_dyneither(_tmp42E,sizeof(char),8))),
_tag_dyneither(_tmp42D,sizeof(void*),1)))))))),_tmp42F)))),((_tmp42B->tl=sl,
_tmp42B))))));}goto _LL20;_LL20:;}for(0;_tmp69 != 0;_tmp69=_tmp69->tl){struct
_dyneither_ptr*_tmp43C;const char*_tmp43B;void*_tmp43A[1];struct Cyc_Int_pa_struct
_tmp439;struct Cyc_List_List*_tmp438;sl=((_tmp438=_cycalloc(sizeof(*_tmp438)),((
_tmp438->hd=((_tmp43C=_cycalloc(sizeof(*_tmp43C)),((_tmp43C[0]=((_tmp439.tag=1,((
_tmp439.f1=(unsigned long)((int)_tmp69->hd),((_tmp43A[0]=& _tmp439,Cyc_aprintf(((
_tmp43B="%d",_tag_dyneither(_tmp43B,sizeof(char),3))),_tag_dyneither(_tmp43A,
sizeof(void*),1)))))))),_tmp43C)))),((_tmp438->tl=sl,_tmp438))))));}{struct Cyc_String_pa_struct
_tmp447;void*_tmp446[1];const char*_tmp445;const char*_tmp444;struct _dyneither_ptr*
_tmp443;struct _dyneither_ptr*_tmp82=(_tmp443=_cycalloc(sizeof(*_tmp443)),((
_tmp443[0]=((_tmp447.tag=0,((_tmp447.f1=(struct _dyneither_ptr)((_tmp444="",
_tag_dyneither(_tmp444,sizeof(char),1))),((_tmp446[0]=& _tmp447,Cyc_aprintf(((
_tmp445="%s",_tag_dyneither(_tmp445,sizeof(char),3))),_tag_dyneither(_tmp446,
sizeof(void*),1)))))))),_tmp443)));for(0;sl != 0;sl=sl->tl){const char*_tmp44C;
void*_tmp44B[2];struct Cyc_String_pa_struct _tmp44A;struct Cyc_String_pa_struct
_tmp449;*_tmp82=((_tmp449.tag=0,((_tmp449.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp82),((_tmp44A.tag=0,((_tmp44A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((_tmp44B[0]=& _tmp44A,((
_tmp44B[1]=& _tmp449,Cyc_aprintf(((_tmp44C="%s.%s",_tag_dyneither(_tmp44C,sizeof(
char),6))),_tag_dyneither(_tmp44B,sizeof(void*),2))))))))))))));}return _tmp82;}}
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int
allow_zeroterm,void*t,void*leafval);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,void*leafval);static
void _tmp452(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp451,unsigned int*
_tmp450,void***_tmp44E){for(*_tmp451=0;*_tmp451 < *_tmp450;(*_tmp451)++){(*
_tmp44E)[*_tmp451]=(void*)(*fenv)->unknown_all;}}struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,void*leafval){
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
unsigned int _tmp451;unsigned int _tmp450;struct _dyneither_ptr _tmp44F;void**
_tmp44E;unsigned int _tmp44D;struct _dyneither_ptr d=(_tmp44D=sz,((_tmp44E=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp44D)),((_tmp44F=
_tag_dyneither(_tmp44E,sizeof(void*),_tmp44D),((((_tmp450=_tmp44D,_tmp452(& fenv,&
_tmp451,& _tmp450,& _tmp44E))),_tmp44F)))))));{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield _tmp8D;struct _dyneither_ptr*_tmp8E;void*_tmp8F;struct
Cyc_Absyn_Aggrfield*_tmp8C=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)
_check_null(fs))->hd;_tmp8D=*_tmp8C;_tmp8E=_tmp8D.name;_tmp8F=(void*)_tmp8D.type;
if(_get_dyneither_size(*_tmp8E,sizeof(char))!= 1)*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,_tmp8F,leafval);}}return d;}struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;
};static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
int allow_zeroterm,void*t,void*leafval);static void _tmp458(struct Cyc_CfFlowInfo_FlowEnv**
fenv,unsigned int*_tmp457,unsigned int*_tmp456,void***_tmp454){for(*_tmp457=0;*
_tmp457 < *_tmp456;(*_tmp457)++){(*_tmp454)[*_tmp457]=(void*)(*fenv)->unknown_all;}}
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int
allow_zeroterm,void*t,void*leafval){if(Cyc_Absyn_is_union_type(t))return(void*)
fenv->unknown_all;{void*_tmp94=Cyc_Tcutil_compress(t);struct Cyc_Absyn_TunionFieldInfo
_tmp95;union Cyc_Absyn_TunionFieldInfoU_union _tmp96;struct Cyc_Absyn_Tunionfield*
_tmp97;struct Cyc_List_List*_tmp98;struct Cyc_Absyn_AggrInfo _tmp99;union Cyc_Absyn_AggrInfoU_union
_tmp9A;void*_tmp9B;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_ArrayInfo _tmp9D;
void*_tmp9E;struct Cyc_Absyn_Conref*_tmp9F;void*_tmpA0;struct Cyc_Absyn_PtrInfo
_tmpA1;struct Cyc_Absyn_PtrAtts _tmpA2;struct Cyc_Absyn_Conref*_tmpA3;_LL28: if(
_tmp94 <= (void*)4)goto _LL36;if(*((int*)_tmp94)!= 3)goto _LL2A;_tmp95=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp94)->f1;_tmp96=_tmp95.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp94)->f1).field_info).KnownTunionfield).tag != 1)goto _LL2A;_tmp97=(_tmp96.KnownTunionfield).f2;
_LL29: if(_tmp97->typs == 0)return leafval;_tmp98=_tmp97->typs;goto _LL2B;_LL2A: if(*((
int*)_tmp94)!= 9)goto _LL2C;_tmp98=((struct Cyc_Absyn_TupleType_struct*)_tmp94)->f1;
_LL2B: {unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp98);unsigned int _tmp457;unsigned int _tmp456;struct _dyneither_ptr _tmp455;void**
_tmp454;unsigned int _tmp453;struct _dyneither_ptr d=(_tmp453=sz,((_tmp454=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp453)),((_tmp455=
_tag_dyneither(_tmp454,sizeof(void*),_tmp453),((((_tmp456=_tmp453,_tmp458(& fenv,&
_tmp457,& _tmp456,& _tmp454))),_tmp455)))))));{int i=0;for(0;i < sz;++ i){*((void**)
_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp98))->hd)).f2,
leafval);_tmp98=_tmp98->tl;}}{struct Cyc_CfFlowInfo_Aggregate_struct _tmp45B;
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp45A;return(void*)((_tmp45A=
_region_malloc(fenv->r,sizeof(*_tmp45A)),((_tmp45A[0]=((_tmp45B.tag=4,((_tmp45B.f1=
d,_tmp45B)))),_tmp45A))));}}_LL2C: if(*((int*)_tmp94)!= 10)goto _LL2E;_tmp99=((
struct Cyc_Absyn_AggrType_struct*)_tmp94)->f1;_tmp9A=_tmp99.aggr_info;_LL2D: {
struct Cyc_Absyn_Aggrdecl*_tmpAA=Cyc_Absyn_get_known_aggrdecl(_tmp9A);if(_tmpAA->impl
== 0)goto _LL27;_tmp9C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAA->impl))->fields;
goto _LL2F;}_LL2E: if(*((int*)_tmp94)!= 11)goto _LL30;_tmp9B=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp94)->f1;if((int)_tmp9B != 0)goto _LL30;_tmp9C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp94)->f2;_LL2F: {struct Cyc_CfFlowInfo_Aggregate_struct _tmp45E;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp45D;return(void*)((_tmp45D=_region_malloc(fenv->r,sizeof(*_tmp45D)),((
_tmp45D[0]=((_tmp45E.tag=4,((_tmp45E.f1=Cyc_CfFlowInfo_aggrfields_to_aggrdict(
fenv,_tmp9C,leafval),_tmp45E)))),_tmp45D))));}_LL30: if(*((int*)_tmp94)!= 7)goto
_LL32;_tmp9D=((struct Cyc_Absyn_ArrayType_struct*)_tmp94)->f1;_tmp9E=(void*)
_tmp9D.elt_type;_tmp9F=_tmp9D.zero_term;if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp9F))goto _LL32;_LL31: return allow_zeroterm  && Cyc_Tcutil_bits_only(
_tmp9E)?(void*)fenv->unknown_all: leafval;_LL32: if(*((int*)_tmp94)!= 18)goto _LL34;
_tmpA0=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp94)->f1;_LL33: {void*_tmpAD=
leafval;void*_tmpAE;void*_tmpAF;_LL39: if(_tmpAD <= (void*)3)goto _LL3D;if(*((int*)
_tmpAD)!= 0)goto _LL3B;_tmpAE=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpAD)->f1;if((int)_tmpAE != 2)goto _LL3B;_LL3A: goto _LL3C;_LL3B: if(*((int*)_tmpAD)
!= 1)goto _LL3D;_tmpAF=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmpAD)->f1;if((
int)_tmpAF != 2)goto _LL3D;_LL3C: goto _LL3E;_LL3D: if((int)_tmpAD != 0)goto _LL3F;
_LL3E: goto _LL40;_LL3F: if((int)_tmpAD != 1)goto _LL41;_LL40: {struct Cyc_CfFlowInfo_TagCmps_struct
_tmp468;struct Cyc_CfFlowInfo_TagCmp*_tmp467;struct Cyc_CfFlowInfo_TagCmp*_tmp466[
1];struct Cyc_CfFlowInfo_TagCmps_struct*_tmp465;return(void*)((_tmp465=
_region_malloc(fenv->r,sizeof(*_tmp465)),((_tmp465[0]=((_tmp468.tag=3,((_tmp468.f1=((
_tmp466[0]=((_tmp467=_region_malloc(fenv->r,sizeof(*_tmp467)),((_tmp467->cmp=(
void*)((void*)5),((_tmp467->bd=(void*)_tmpA0,_tmp467)))))),((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(fenv->r,
_tag_dyneither(_tmp466,sizeof(struct Cyc_CfFlowInfo_TagCmp*),1)))),_tmp468)))),
_tmp465))));}_LL41:;_LL42: return leafval;_LL38:;}_LL34: if(*((int*)_tmp94)!= 4)
goto _LL36;_tmpA1=((struct Cyc_Absyn_PointerType_struct*)_tmp94)->f1;_tmpA2=_tmpA1.ptr_atts;
_tmpA3=_tmpA2.nullable;if(!(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpA3)))goto _LL36;_LL35:{void*_tmpB4=leafval;void*_tmpB5;void*_tmpB6;_LL44:
if(_tmpB4 <= (void*)3)goto _LL48;if(*((int*)_tmpB4)!= 0)goto _LL46;_tmpB5=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpB4)->f1;if((int)_tmpB5 != 1)goto _LL46;
_LL45: return(void*)2;_LL46: if(*((int*)_tmpB4)!= 0)goto _LL48;_tmpB6=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpB4)->f1;if((int)_tmpB6 != 2)goto _LL48;
_LL47: return(void*)1;_LL48:;_LL49: goto _LL43;_LL43:;}goto _LL27;_LL36:;_LL37: goto
_LL27;_LL27:;}return Cyc_Tcutil_bits_only(t)?(void*)fenv->unknown_all: leafval;}
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,void*
leafval);void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
void*t,void*leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,t,leafval);}
struct _tuple12{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*f2;};
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct
Cyc_Dict_Dict set);static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*
place,struct Cyc_Dict_Dict set){struct _RegionHandle _tmpB7=_new_region("r");struct
_RegionHandle*r=& _tmpB7;_push_region(r);{struct _tuple12 _tmp469;struct _tuple12
_tmpB8=(_tmp469.f1=place,((_tmp469.f2=0,_tmp469)));struct Cyc_Iter_Iter _tmpB9=((
struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
r,set);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple12*))Cyc_Iter_next)(_tmpB9,&
_tmpB8)){struct Cyc_CfFlowInfo_Place*_tmpBA=_tmpB8.f1;void*_tmpBC;struct Cyc_List_List*
_tmpBD;struct Cyc_CfFlowInfo_Place _tmpBB=*place;_tmpBC=(void*)_tmpBB.root;_tmpBD=
_tmpBB.fields;{void*_tmpBF;struct Cyc_List_List*_tmpC0;struct Cyc_CfFlowInfo_Place
_tmpBE=*_tmpBA;_tmpBF=(void*)_tmpBE.root;_tmpC0=_tmpBE.fields;if(Cyc_CfFlowInfo_root_cmp(
_tmpBC,_tmpBF)!= 0)continue;for(0;_tmpBD != 0  && _tmpC0 != 0;(_tmpBD=_tmpBD->tl,
_tmpC0=_tmpC0->tl)){if((int)_tmpBD->hd != (int)_tmpC0->hd)break;}if(_tmpBD == 0){
int _tmpC1=1;_npop_handler(0);return _tmpC1;}}}{int _tmpC2=0;_npop_handler(0);
return _tmpC2;}};_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*
rgn;struct Cyc_List_List*places;};static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*
pile,struct Cyc_CfFlowInfo_Place*place);static void Cyc_CfFlowInfo_add_place(struct
Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*
compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,
place)){struct Cyc_List_List*_tmp46A;pile->places=((_tmp46A=_region_malloc(pile->rgn,
sizeof(*_tmp46A)),((_tmp46A->hd=place,((_tmp46A->tl=pile->places,_tmp46A))))));}}
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r);
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
void*_tmpC5=r;struct Cyc_CfFlowInfo_Place*_tmpC6;struct _dyneither_ptr _tmpC7;_LL4B:
if(_tmpC5 <= (void*)3)goto _LL4F;if(*((int*)_tmpC5)!= 2)goto _LL4D;_tmpC6=((struct
Cyc_CfFlowInfo_AddressOf_struct*)_tmpC5)->f1;_LL4C: Cyc_CfFlowInfo_add_place(pile,
_tmpC6);return;_LL4D: if(*((int*)_tmpC5)!= 4)goto _LL4F;_tmpC7=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpC5)->f1;_LL4E:{int i=0;for(0;i < _get_dyneither_size(_tmpC7,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,((void**)_tmpC7.curr)[i]);}}return;_LL4F:;_LL50:
return;_LL4A:;}static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*
r,void*new_val,void*old_val);static void _tmp470(struct _RegionHandle**r,void**
new_val,struct _dyneither_ptr*_tmpC9,unsigned int*_tmp46F,unsigned int*_tmp46E,
void***_tmp46C){for(*_tmp46F=0;*_tmp46F < *_tmp46E;(*_tmp46F)++){(*_tmp46C)[*
_tmp46F]=Cyc_CfFlowInfo_insert_place_inner(*r,*new_val,((void**)(*_tmpC9).curr)[(
int)*_tmp46F]);}}static void*Cyc_CfFlowInfo_insert_place_inner(struct
_RegionHandle*r,void*new_val,void*old_val){void*_tmpC8=old_val;struct
_dyneither_ptr _tmpC9;_LL52: if(_tmpC8 <= (void*)3)goto _LL54;if(*((int*)_tmpC8)!= 4)
goto _LL54;_tmpC9=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpC8)->f1;_LL53: {
unsigned int _tmp46F;unsigned int _tmp46E;struct _dyneither_ptr _tmp46D;void**
_tmp46C;unsigned int _tmp46B;struct _dyneither_ptr d2=(_tmp46B=_get_dyneither_size(
_tmpC9,sizeof(void*)),((_tmp46C=(void**)_region_malloc(r,_check_times(sizeof(
void*),_tmp46B)),((_tmp46D=_tag_dyneither(_tmp46C,sizeof(void*),_tmp46B),((((
_tmp46E=_tmp46B,_tmp470(& r,& new_val,& _tmpC9,& _tmp46F,& _tmp46E,& _tmp46C))),
_tmp46D)))))));struct Cyc_CfFlowInfo_Aggregate_struct _tmp473;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp472;return(void*)((_tmp472=_region_malloc(r,sizeof(*_tmp472)),((_tmp472[0]=((
_tmp473.tag=4,((_tmp473.f1=d2,_tmp473)))),_tmp472))));}_LL54:;_LL55: return
new_val;_LL51:;}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval);static void _tmp479(struct
_dyneither_ptr*d,unsigned int*_tmp478,unsigned int*_tmp477,void***_tmp475){for(*
_tmp478=0;*_tmp478 < *_tmp477;(*_tmp478)++){(*_tmp475)[*_tmp478]=((void**)(*d).curr)[(
int)*_tmp478];}}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct
_RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){void*_tmpD0=*((void**)
_check_dyneither_subscript(d,sizeof(void*),n));if(_tmpD0 == rval)return d;{
unsigned int _tmp478;unsigned int _tmp477;struct _dyneither_ptr _tmp476;void**
_tmp475;unsigned int _tmp474;struct _dyneither_ptr res=(_tmp474=_get_dyneither_size(
d,sizeof(void*)),((_tmp475=(void**)_region_malloc(r,_check_times(sizeof(void*),
_tmp474)),((_tmp476=_tag_dyneither(_tmp475,sizeof(void*),_tmp474),((((_tmp477=
_tmp474,_tmp479(& d,& _tmp478,& _tmp477,& _tmp475))),_tmp476)))))));*((void**)
_check_dyneither_subscript(res,sizeof(void*),n))=rval;return res;}}struct _tuple13{
struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val);static
void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*
fs,void*old_val,void*new_val){if(fs == 0)return Cyc_CfFlowInfo_insert_place_inner(
r,new_val,old_val);{struct _tuple13 _tmp47A;struct _tuple13 _tmpD6=(_tmp47A.f1=fs,((
_tmp47A.f2=old_val,_tmp47A)));struct Cyc_List_List*_tmpD7;struct Cyc_List_List
_tmpD8;int _tmpD9;struct Cyc_List_List*_tmpDA;void*_tmpDB;struct _dyneither_ptr
_tmpDC;_LL57: _tmpD7=_tmpD6.f1;if(_tmpD7 == 0)goto _LL59;_tmpD8=*_tmpD7;_tmpD9=(int)
_tmpD8.hd;_tmpDA=_tmpD8.tl;_tmpDB=_tmpD6.f2;if(_tmpDB <= (void*)3)goto _LL59;if(*((
int*)_tmpDB)!= 4)goto _LL59;_tmpDC=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpDB)->f1;_LL58: {void*_tmpDD=Cyc_CfFlowInfo_insert_place_outer(r,_tmpDA,*((
void**)_check_dyneither_subscript(_tmpDC,sizeof(void*),_tmpD9)),new_val);struct
_dyneither_ptr _tmpDE=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmpDC,_tmpD9,_tmpDD);if(
_tmpDE.curr == _tmpDC.curr)return old_val;else{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp47D;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp47C;return(void*)((_tmp47C=
_region_malloc(r,sizeof(*_tmp47C)),((_tmp47C[0]=((_tmp47D.tag=4,((_tmp47D.f1=
_tmpDE,_tmp47D)))),_tmp47C))));}}_LL59:;_LL5A: {struct Cyc_Core_Impossible_struct
_tmp483;const char*_tmp482;struct Cyc_Core_Impossible_struct*_tmp481;(int)_throw((
void*)((_tmp481=_cycalloc(sizeof(*_tmp481)),((_tmp481[0]=((_tmp483.tag=Cyc_Core_Impossible,((
_tmp483.f1=((_tmp482="bad insert place",_tag_dyneither(_tmp482,sizeof(char),17))),
_tmp483)))),_tmp481)))));}_LL56:;}}static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*
all_changed,struct Cyc_Dict_Dict d);static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*
all_changed,struct Cyc_Dict_Dict d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*
_tmpE4=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;((int(*)(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_update_place_set)(all_changed,_tmpE4,0);{void*oldval;void*
newval;{struct _handler_cons _tmpE5;_push_handler(& _tmpE5);{int _tmpE7=0;if(setjmp(
_tmpE5.handler))_tmpE7=1;if(!_tmpE7){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmpE4);;
_pop_handler();}else{void*_tmpE6=(void*)_exn_thrown;void*_tmpE9=_tmpE6;_LL5C: if(
_tmpE9 != Cyc_Dict_Absent)goto _LL5E;_LL5D: continue;_LL5E:;_LL5F:(void)_throw(
_tmpE9);_LL5B:;}}}{void*_tmpEA=Cyc_CfFlowInfo_initlevel(d,oldval);_LL61: if((int)
_tmpEA != 2)goto _LL63;_LL62: newval=(void*)fenv->esc_all;goto _LL60;_LL63: if((int)
_tmpEA != 1)goto _LL65;_LL64: newval=(void*)fenv->esc_this;goto _LL60;_LL65: if((int)
_tmpEA != 0)goto _LL60;_LL66: newval=(void*)fenv->esc_none;goto _LL60;_LL60:;}Cyc_CfFlowInfo_add_places(
pile,oldval);{void*_tmpEC;struct Cyc_List_List*_tmpED;struct Cyc_CfFlowInfo_Place
_tmpEB=*_tmpE4;_tmpEC=(void*)_tmpEB.root;_tmpED=_tmpEB.fields;d=Cyc_Dict_insert(
d,_tmpEC,Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmpED,Cyc_Dict_lookup(d,
_tmpEC),newval));}}}return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct
_RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};static void*Cyc_CfFlowInfo_initlevel_approx(
void*r);static void*Cyc_CfFlowInfo_initlevel_approx(void*r){void*_tmpEE=r;void*
_tmpEF;void*_tmpF0;_LL68: if(_tmpEE <= (void*)3)goto _LL6C;if(*((int*)_tmpEE)!= 0)
goto _LL6A;_tmpEF=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpEE)->f1;
_LL69: return _tmpEF;_LL6A: if(*((int*)_tmpEE)!= 1)goto _LL6C;_tmpF0=(void*)((struct
Cyc_CfFlowInfo_Esc_struct*)_tmpEE)->f1;_LL6B: return _tmpF0;_LL6C: if((int)_tmpEE != 
0)goto _LL6E;_LL6D: goto _LL6F;_LL6E: if((int)_tmpEE != 1)goto _LL70;_LL6F: return(void*)
2;_LL70: if((int)_tmpEE != 2)goto _LL72;_LL71: return(void*)1;_LL72: if(_tmpEE <= (
void*)3)goto _LL74;if(*((int*)_tmpEE)!= 3)goto _LL74;_LL73: return(void*)2;_LL74:;
_LL75: {struct Cyc_Core_Impossible_struct _tmp489;const char*_tmp488;struct Cyc_Core_Impossible_struct*
_tmp487;(int)_throw((void*)((_tmp487=_cycalloc(sizeof(*_tmp487)),((_tmp487[0]=((
_tmp489.tag=Cyc_Core_Impossible,((_tmp489.f1=((_tmp488="initlevel_approx",
_tag_dyneither(_tmp488,sizeof(char),17))),_tmp489)))),_tmp487)))));}_LL67:;}
static void*Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,
void*r,void*acc);static void*Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*
env,void*r,void*acc){void*this_ans;if(acc == (void*)0)return(void*)0;{void*_tmpF4=
r;struct _dyneither_ptr _tmpF5;struct Cyc_CfFlowInfo_Place*_tmpF6;_LL77: if(_tmpF4 <= (
void*)3)goto _LL7B;if(*((int*)_tmpF4)!= 4)goto _LL79;_tmpF5=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmpF4)->f1;_LL78: {unsigned int _tmpF7=_get_dyneither_size(_tmpF5,sizeof(void*));
this_ans=(void*)2;{int i=0;for(0;i < _tmpF7;++ i){this_ans=Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmpF5,sizeof(void*),i)),this_ans);}}
goto _LL76;}_LL79: if(*((int*)_tmpF4)!= 2)goto _LL7B;_tmpF6=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpF4)->f1;_LL7A: if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct
Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(
Cyc_CfFlowInfo_place_cmp,env->seen,_tmpF6))this_ans=(void*)2;else{{struct Cyc_List_List*
_tmp48A;env->seen=((_tmp48A=_region_malloc(env->e,sizeof(*_tmp48A)),((_tmp48A->hd=
_tmpF6,((_tmp48A->tl=env->seen,_tmp48A))))));}this_ans=Cyc_CfFlowInfo_initlevel_rec(
env,Cyc_CfFlowInfo_lookup_place(env->d,_tmpF6),(void*)2);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == (void*)0)this_ans=(void*)1;}goto _LL76;
_LL7B:;_LL7C: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL76:;}if(this_ans == (
void*)0)return(void*)0;if(this_ans == (void*)1  || acc == (void*)1)return(void*)1;
return(void*)2;}void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict d,void*r);void*
Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict d,void*r){struct _RegionHandle _tmpF9=
_new_region("env_rgn");struct _RegionHandle*env_rgn=& _tmpF9;_push_region(env_rgn);{
struct Cyc_CfFlowInfo_InitlevelEnv _tmp48B;struct Cyc_CfFlowInfo_InitlevelEnv _tmpFA=(
_tmp48B.e=env_rgn,((_tmp48B.d=d,((_tmp48B.seen=0,_tmp48B)))));void*_tmpFB=Cyc_CfFlowInfo_initlevel_rec(&
_tmpFA,r,(void*)2);_npop_handler(0);return _tmpFB;};_pop_region(env_rgn);}struct
_tuple14{void*f1;int f2;};void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*place);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place){void*_tmpFE;struct Cyc_List_List*_tmpFF;
struct Cyc_CfFlowInfo_Place _tmpFD=*place;_tmpFE=(void*)_tmpFD.root;_tmpFF=_tmpFD.fields;{
void*_tmp100=Cyc_Dict_lookup(d,_tmpFE);for(0;_tmpFF != 0;_tmpFF=_tmpFF->tl){
struct _tuple14 _tmp48C;struct _tuple14 _tmp102=(_tmp48C.f1=_tmp100,((_tmp48C.f2=(
int)_tmpFF->hd,_tmp48C)));void*_tmp103;struct _dyneither_ptr _tmp104;int _tmp105;
_LL7E: _tmp103=_tmp102.f1;if(_tmp103 <= (void*)3)goto _LL80;if(*((int*)_tmp103)!= 4)
goto _LL80;_tmp104=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp103)->f1;_tmp105=
_tmp102.f2;_LL7F: _tmp100=*((void**)_check_dyneither_subscript(_tmp104,sizeof(
void*),_tmp105));goto _LL7D;_LL80:;_LL81: {struct Cyc_Core_Impossible_struct
_tmp492;const char*_tmp491;struct Cyc_Core_Impossible_struct*_tmp490;(int)_throw((
void*)((_tmp490=_cycalloc(sizeof(*_tmp490)),((_tmp490[0]=((_tmp492.tag=Cyc_Core_Impossible,((
_tmp492.f1=((_tmp491="bad lookup_place",_tag_dyneither(_tmp491,sizeof(char),17))),
_tmp492)))),_tmp490)))));}_LL7D:;}return _tmp100;}}static int Cyc_CfFlowInfo_is_rval_unescaped(
void*rval);static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){void*_tmp109=
rval;struct _dyneither_ptr _tmp10A;_LL83: if(_tmp109 <= (void*)3)goto _LL87;if(*((int*)
_tmp109)!= 1)goto _LL85;_LL84: return 0;_LL85: if(*((int*)_tmp109)!= 4)goto _LL87;
_tmp10A=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp109)->f1;_LL86: {
unsigned int sz=_get_dyneither_size(_tmp10A,sizeof(void*));{int i=0;for(0;i < sz;++
i){if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp10A.curr)[i]))return 0;}}
return 1;}_LL87:;_LL88: return 1;_LL82:;}int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place){return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(
d,place));}struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*all_changed,void*r){struct _RegionHandle _tmp10B=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp10B;_push_region(rgn);{struct Cyc_CfFlowInfo_EscPile*
_tmp493;struct Cyc_CfFlowInfo_EscPile*pile=(_tmp493=_region_malloc(rgn,sizeof(*
_tmp493)),((_tmp493->rgn=rgn,((_tmp493->places=0,_tmp493)))));Cyc_CfFlowInfo_add_places(
pile,r);{struct Cyc_Dict_Dict _tmp10C=Cyc_CfFlowInfo_escape_these(fenv,pile,
all_changed,d);_npop_handler(0);return _tmp10C;}};_pop_region(rgn);}struct Cyc_CfFlowInfo_AssignEnv{
struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict
d;struct Cyc_Position_Segment*loc;};static void*Cyc_CfFlowInfo_assign_place_inner(
struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval);static void _tmp49D(
struct Cyc_CfFlowInfo_AssignEnv**env,unsigned int*_tmp49C,unsigned int*_tmp49B,
void***_tmp499,struct _dyneither_ptr*_tmp117,struct _dyneither_ptr*_tmp115){for(*
_tmp49C=0;*_tmp49C < *_tmp49B;(*_tmp49C)++){(*_tmp499)[*_tmp49C]=Cyc_CfFlowInfo_assign_place_inner(*
env,((void**)(*_tmp115).curr)[(int)*_tmp49C],*((void**)
_check_dyneither_subscript(*_tmp117,sizeof(void*),(int)*_tmp49C)));}}static void*
Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,
void*newval){struct _tuple0 _tmp494;struct _tuple0 _tmp10F=(_tmp494.f1=oldval,((
_tmp494.f2=newval,_tmp494)));void*_tmp110;void*_tmp111;struct Cyc_CfFlowInfo_Place*
_tmp112;void*_tmp113;void*_tmp114;struct _dyneither_ptr _tmp115;void*_tmp116;
struct _dyneither_ptr _tmp117;void*_tmp118;void*_tmp119;_LL8A: _tmp110=_tmp10F.f1;
if(_tmp110 <= (void*)3)goto _LL8C;if(*((int*)_tmp110)!= 1)goto _LL8C;_tmp111=
_tmp10F.f2;if(_tmp111 <= (void*)3)goto _LL8C;if(*((int*)_tmp111)!= 2)goto _LL8C;
_tmp112=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp111)->f1;_LL8B: Cyc_CfFlowInfo_add_place(
env->pile,_tmp112);goto _LL8D;_LL8C: _tmp113=_tmp10F.f1;if(_tmp113 <= (void*)3)goto
_LL8E;if(*((int*)_tmp113)!= 1)goto _LL8E;_LL8D: if(Cyc_CfFlowInfo_initlevel(env->d,
newval)!= (void*)2){const char*_tmp497;void*_tmp496;(_tmp496=0,Cyc_Tcutil_terr(
env->loc,((_tmp497="assignment puts possibly-uninitialized data in an escaped location",
_tag_dyneither(_tmp497,sizeof(char),67))),_tag_dyneither(_tmp496,sizeof(void*),0)));}
return(void*)(env->fenv)->esc_all;_LL8E: _tmp114=_tmp10F.f1;if(_tmp114 <= (void*)3)
goto _LL90;if(*((int*)_tmp114)!= 4)goto _LL90;_tmp115=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp114)->f1;_tmp116=_tmp10F.f2;if(_tmp116 <= (void*)3)goto _LL90;if(*((int*)
_tmp116)!= 4)goto _LL90;_tmp117=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp116)->f1;
_LL8F: {unsigned int _tmp49C;unsigned int _tmp49B;struct _dyneither_ptr _tmp49A;void**
_tmp499;unsigned int _tmp498;struct _dyneither_ptr new_d=(_tmp498=
_get_dyneither_size(_tmp115,sizeof(void*)),((_tmp499=(void**)_region_malloc((env->fenv)->r,
_check_times(sizeof(void*),_tmp498)),((_tmp49A=_tag_dyneither(_tmp499,sizeof(
void*),_tmp498),((((_tmp49B=_tmp498,_tmp49D(& env,& _tmp49C,& _tmp49B,& _tmp499,&
_tmp117,& _tmp115))),_tmp49A)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp115,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp115.curr)[i]){
change=1;break;}}}if(!change)return oldval;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp115,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp117,sizeof(void*),i))){change=1;break;}}}if(!
change)return newval;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp4A0;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp49F;return(void*)((_tmp49F=_region_malloc((env->fenv)->r,sizeof(*_tmp49F)),((
_tmp49F[0]=((_tmp4A0.tag=4,((_tmp4A0.f1=new_d,_tmp4A0)))),_tmp49F))));}}_LL90:
_tmp118=_tmp10F.f2;if(_tmp118 <= (void*)3)goto _LL92;if(*((int*)_tmp118)!= 1)goto
_LL92;_tmp119=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp118)->f1;_LL91: {
void*_tmp122=_tmp119;_LL95: if((int)_tmp122 != 0)goto _LL97;_LL96: return(void*)(env->fenv)->unknown_none;
_LL97: if((int)_tmp122 != 1)goto _LL99;_LL98: return(void*)(env->fenv)->unknown_this;
_LL99: if((int)_tmp122 != 2)goto _LL94;_LL9A: return(void*)(env->fenv)->unknown_all;
_LL94:;}_LL92:;_LL93: return newval;_LL89:;}static void*Cyc_CfFlowInfo_assign_place_outer(
struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,void*oldval,void*
newval);static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*
env,struct Cyc_List_List*fs,void*oldval,void*newval){if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(
env,oldval,newval);{struct _tuple13 _tmp4A1;struct _tuple13 _tmp124=(_tmp4A1.f1=fs,((
_tmp4A1.f2=oldval,_tmp4A1)));struct Cyc_List_List*_tmp125;struct Cyc_List_List
_tmp126;int _tmp127;struct Cyc_List_List*_tmp128;void*_tmp129;struct _dyneither_ptr
_tmp12A;_LL9C: _tmp125=_tmp124.f1;if(_tmp125 == 0)goto _LL9E;_tmp126=*_tmp125;
_tmp127=(int)_tmp126.hd;_tmp128=_tmp126.tl;_tmp129=_tmp124.f2;if(_tmp129 <= (void*)
3)goto _LL9E;if(*((int*)_tmp129)!= 4)goto _LL9E;_tmp12A=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp129)->f1;_LL9D: {void*_tmp12B=Cyc_CfFlowInfo_assign_place_outer(env,_tmp128,*((
void**)_check_dyneither_subscript(_tmp12A,sizeof(void*),_tmp127)),newval);struct
_dyneither_ptr _tmp12C=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp12A,
_tmp127,_tmp12B);if(_tmp12C.curr == _tmp12A.curr)return oldval;else{struct Cyc_CfFlowInfo_Aggregate_struct
_tmp4A4;struct Cyc_CfFlowInfo_Aggregate_struct*_tmp4A3;return(void*)((_tmp4A3=
_region_malloc((env->fenv)->r,sizeof(*_tmp4A3)),((_tmp4A3[0]=((_tmp4A4.tag=4,((
_tmp4A4.f1=_tmp12C,_tmp4A4)))),_tmp4A3))));}}_LL9E:;_LL9F: {struct Cyc_Core_Impossible_struct
_tmp4AA;const char*_tmp4A9;struct Cyc_Core_Impossible_struct*_tmp4A8;(int)_throw((
void*)((_tmp4A8=_cycalloc(sizeof(*_tmp4A8)),((_tmp4A8[0]=((_tmp4AA.tag=Cyc_Core_Impossible,((
_tmp4AA.f1=((_tmp4A9="bad assign place",_tag_dyneither(_tmp4A9,sizeof(char),17))),
_tmp4AA)))),_tmp4A8)))));}_LL9B:;}}struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict
d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){((int(*)(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
all_changed,place,0);{struct _RegionHandle _tmp132=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp132;_push_region(rgn);{struct Cyc_CfFlowInfo_Place _tmp134;
void*_tmp135;struct Cyc_List_List*_tmp136;struct Cyc_CfFlowInfo_Place*_tmp133=
place;_tmp134=*_tmp133;_tmp135=(void*)_tmp134.root;_tmp136=_tmp134.fields;{
struct Cyc_CfFlowInfo_EscPile*_tmp4AD;struct Cyc_CfFlowInfo_AssignEnv _tmp4AC;
struct Cyc_CfFlowInfo_AssignEnv env=(_tmp4AC.fenv=fenv,((_tmp4AC.pile=((_tmp4AD=
_region_malloc(rgn,sizeof(*_tmp4AD)),((_tmp4AD->rgn=rgn,((_tmp4AD->places=0,
_tmp4AD)))))),((_tmp4AC.d=d,((_tmp4AC.loc=loc,_tmp4AC)))))));void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmp136,Cyc_Dict_lookup(d,_tmp135),r);struct Cyc_Dict_Dict _tmp137=Cyc_CfFlowInfo_escape_these(
fenv,env.pile,all_changed,Cyc_Dict_insert(d,_tmp135,newval));_npop_handler(0);
return _tmp137;}};_pop_region(rgn);}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*
fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict
d2;};enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One
 = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv
joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*
curr_place;struct Cyc_List_List**last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged
changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,
void*r2);static struct Cyc_List_List*Cyc_CfFlowInfo_join_tag_cmps(struct
_RegionHandle*r,struct Cyc_List_List*l1,struct Cyc_List_List*l2);static struct Cyc_List_List*
Cyc_CfFlowInfo_join_tag_cmps(struct _RegionHandle*r,struct Cyc_List_List*l1,struct
Cyc_List_List*l2){if(l1 == l2)return l1;{struct Cyc_List_List*_tmp13A=0;for(0;l2 != 
0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp _tmp13C;void*_tmp13D;void*_tmp13E;struct
Cyc_CfFlowInfo_TagCmp*_tmp13B=(struct Cyc_CfFlowInfo_TagCmp*)l2->hd;_tmp13C=*
_tmp13B;_tmp13D=(void*)_tmp13C.cmp;_tmp13E=(void*)_tmp13C.bd;{int found=0;void*
joined_cmp=(void*)10;{struct Cyc_List_List*_tmp13F=l1;for(0;_tmp13F != 0;_tmp13F=
_tmp13F->tl){struct Cyc_CfFlowInfo_TagCmp _tmp141;void*_tmp142;void*_tmp143;struct
Cyc_CfFlowInfo_TagCmp*_tmp140=(struct Cyc_CfFlowInfo_TagCmp*)_tmp13F->hd;_tmp141=*
_tmp140;_tmp142=(void*)_tmp141.cmp;_tmp143=(void*)_tmp141.bd;if(Cyc_Tcutil_typecmp(
_tmp13E,_tmp143)== 0){found=1;if(_tmp142 == _tmp13D){joined_cmp=_tmp142;break;}}}}
if(found){struct Cyc_CfFlowInfo_TagCmp*_tmp4B0;struct Cyc_List_List*_tmp4AF;
_tmp13A=((_tmp4AF=_region_malloc(r,sizeof(*_tmp4AF)),((_tmp4AF->hd=((_tmp4B0=
_region_malloc(r,sizeof(*_tmp4B0)),((_tmp4B0->cmp=(void*)joined_cmp,((_tmp4B0->bd=(
void*)_tmp13E,_tmp4B0)))))),((_tmp4AF->tl=_tmp13A,_tmp4AF))))));}}}return _tmp13A;}}
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,
void*r1,void*r2);static void _tmp4B7(struct Cyc_CfFlowInfo_JoinEnv**env,
unsigned int*_tmp4B6,unsigned int*_tmp4B5,void***_tmp4B3,struct _dyneither_ptr*
_tmp158,struct _dyneither_ptr*_tmp156){for(*_tmp4B6=0;*_tmp4B6 < *_tmp4B5;(*
_tmp4B6)++){(*_tmp4B3)[*_tmp4B6]=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int
a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp156).curr)[(
int)*_tmp4B6],*((void**)_check_dyneither_subscript(*_tmp158,sizeof(void*),(int)*
_tmp4B6)));}}static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*
env,void*a,void*r1,void*r2){if(r1 == r2)return r1;{struct _RegionHandle*_tmp146=(
env->fenv)->r;{struct _tuple0 _tmp4B1;struct _tuple0 _tmp148=(_tmp4B1.f1=r1,((
_tmp4B1.f2=r2,_tmp4B1)));void*_tmp149;struct Cyc_CfFlowInfo_Place*_tmp14A;void*
_tmp14B;struct Cyc_CfFlowInfo_Place*_tmp14C;void*_tmp14D;struct Cyc_CfFlowInfo_Place*
_tmp14E;void*_tmp14F;struct Cyc_CfFlowInfo_Place*_tmp150;void*_tmp151;void*
_tmp152;void*_tmp153;void*_tmp154;void*_tmp155;struct _dyneither_ptr _tmp156;void*
_tmp157;struct _dyneither_ptr _tmp158;void*_tmp159;struct Cyc_List_List*_tmp15A;
void*_tmp15B;struct Cyc_List_List*_tmp15C;void*_tmp15D;void*_tmp15E;_LLA1: _tmp149=
_tmp148.f1;if(_tmp149 <= (void*)3)goto _LLA3;if(*((int*)_tmp149)!= 2)goto _LLA3;
_tmp14A=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp149)->f1;_tmp14B=_tmp148.f2;
if(_tmp14B <= (void*)3)goto _LLA3;if(*((int*)_tmp14B)!= 2)goto _LLA3;_tmp14C=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp14B)->f1;_LLA2: if(Cyc_CfFlowInfo_place_cmp(
_tmp14A,_tmp14C)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmp14A);Cyc_CfFlowInfo_add_place(
env->pile,_tmp14C);goto _LLA0;_LLA3: _tmp14D=_tmp148.f1;if(_tmp14D <= (void*)3)goto
_LLA5;if(*((int*)_tmp14D)!= 2)goto _LLA5;_tmp14E=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp14D)->f1;_LLA4: Cyc_CfFlowInfo_add_place(env->pile,_tmp14E);goto _LLA0;_LLA5:
_tmp14F=_tmp148.f2;if(_tmp14F <= (void*)3)goto _LLA7;if(*((int*)_tmp14F)!= 2)goto
_LLA7;_tmp150=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp14F)->f1;_LLA6: Cyc_CfFlowInfo_add_place(
env->pile,_tmp150);goto _LLA0;_LLA7: _tmp151=_tmp148.f1;if((int)_tmp151 != 1)goto
_LLA9;_tmp152=_tmp148.f2;if((int)_tmp152 != 2)goto _LLA9;_LLA8: goto _LLAA;_LLA9:
_tmp153=_tmp148.f1;if((int)_tmp153 != 2)goto _LLAB;_tmp154=_tmp148.f2;if((int)
_tmp154 != 1)goto _LLAB;_LLAA: return(void*)2;_LLAB: _tmp155=_tmp148.f1;if(_tmp155 <= (
void*)3)goto _LLAD;if(*((int*)_tmp155)!= 4)goto _LLAD;_tmp156=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp155)->f1;_tmp157=_tmp148.f2;if(_tmp157 <= (void*)3)goto _LLAD;if(*((int*)
_tmp157)!= 4)goto _LLAD;_tmp158=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp157)->f1;
_LLAC: {unsigned int _tmp4B6;unsigned int _tmp4B5;struct _dyneither_ptr _tmp4B4;void**
_tmp4B3;unsigned int _tmp4B2;struct _dyneither_ptr new_d=(_tmp4B2=
_get_dyneither_size(_tmp156,sizeof(void*)),((_tmp4B3=(void**)_region_malloc(
_tmp146,_check_times(sizeof(void*),_tmp4B2)),((_tmp4B4=_tag_dyneither(_tmp4B3,
sizeof(void*),_tmp4B2),((((_tmp4B5=_tmp4B2,_tmp4B7(& env,& _tmp4B6,& _tmp4B5,&
_tmp4B3,& _tmp158,& _tmp156))),_tmp4B4)))))));int change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp156,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp156.curr)[i]){
change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp156,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp158,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp4BA;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp4B9;return(void*)((_tmp4B9=_region_malloc(_tmp146,sizeof(*_tmp4B9)),((
_tmp4B9[0]=((_tmp4BA.tag=4,((_tmp4BA.f1=new_d,_tmp4BA)))),_tmp4B9))));}}_LLAD:
_tmp159=_tmp148.f1;if(_tmp159 <= (void*)3)goto _LLAF;if(*((int*)_tmp159)!= 3)goto
_LLAF;_tmp15A=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp159)->f1;_tmp15B=
_tmp148.f2;if(_tmp15B <= (void*)3)goto _LLAF;if(*((int*)_tmp15B)!= 3)goto _LLAF;
_tmp15C=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp15B)->f1;_LLAE: {struct Cyc_List_List*
_tmp165=Cyc_CfFlowInfo_join_tag_cmps(_tmp146,_tmp15A,_tmp15C);if(_tmp165 == 
_tmp15A)return r1;{struct Cyc_CfFlowInfo_TagCmps_struct _tmp4BD;struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp4BC;return(void*)((_tmp4BC=_region_malloc(_tmp146,sizeof(*_tmp4BC)),((
_tmp4BC[0]=((_tmp4BD.tag=3,((_tmp4BD.f1=_tmp165,_tmp4BD)))),_tmp4BC))));}}_LLAF:
_tmp15D=_tmp148.f1;if(_tmp15D <= (void*)3)goto _LLB1;if(*((int*)_tmp15D)!= 3)goto
_LLB1;_LLB0: return r2;_LLB1: _tmp15E=_tmp148.f2;if(_tmp15E <= (void*)3)goto _LLB3;
if(*((int*)_tmp15E)!= 3)goto _LLB3;_LLB2: return r1;_LLB3:;_LLB4: goto _LLA0;_LLA0:;}{
void*il1=Cyc_CfFlowInfo_initlevel(env->d1,r1);void*il2=Cyc_CfFlowInfo_initlevel(
env->d2,r2);struct _tuple0 _tmp4BE;struct _tuple0 _tmp169=(_tmp4BE.f1=r1,((_tmp4BE.f2=
r2,_tmp4BE)));void*_tmp16A;void*_tmp16B;_LLB6: _tmp16A=_tmp169.f1;if(_tmp16A <= (
void*)3)goto _LLB8;if(*((int*)_tmp16A)!= 1)goto _LLB8;_LLB7: goto _LLB9;_LLB8:
_tmp16B=_tmp169.f2;if(_tmp16B <= (void*)3)goto _LLBA;if(*((int*)_tmp16B)!= 1)goto
_LLBA;_LLB9: {struct _tuple0 _tmp4BF;struct _tuple0 _tmp16D=(_tmp4BF.f1=il1,((
_tmp4BF.f2=il2,_tmp4BF)));void*_tmp16E;void*_tmp16F;void*_tmp170;void*_tmp171;
_LLBD: _tmp16E=_tmp16D.f2;if((int)_tmp16E != 0)goto _LLBF;_LLBE: goto _LLC0;_LLBF:
_tmp16F=_tmp16D.f1;if((int)_tmp16F != 0)goto _LLC1;_LLC0: return(void*)(env->fenv)->esc_none;
_LLC1: _tmp170=_tmp16D.f2;if((int)_tmp170 != 1)goto _LLC3;_LLC2: goto _LLC4;_LLC3:
_tmp171=_tmp16D.f1;if((int)_tmp171 != 1)goto _LLC5;_LLC4: return(void*)(env->fenv)->esc_this;
_LLC5:;_LLC6: return(void*)(env->fenv)->esc_all;_LLBC:;}_LLBA:;_LLBB: {struct
_tuple0 _tmp4C0;struct _tuple0 _tmp173=(_tmp4C0.f1=il1,((_tmp4C0.f2=il2,_tmp4C0)));
void*_tmp174;void*_tmp175;void*_tmp176;void*_tmp177;_LLC8: _tmp174=_tmp173.f2;if((
int)_tmp174 != 0)goto _LLCA;_LLC9: goto _LLCB;_LLCA: _tmp175=_tmp173.f1;if((int)
_tmp175 != 0)goto _LLCC;_LLCB: return(void*)(env->fenv)->unknown_none;_LLCC: _tmp176=
_tmp173.f2;if((int)_tmp176 != 1)goto _LLCE;_LLCD: goto _LLCF;_LLCE: _tmp177=_tmp173.f1;
if((int)_tmp177 != 1)goto _LLD0;_LLCF: return(void*)(env->fenv)->unknown_this;_LLD0:;
_LLD1: return(void*)(env->fenv)->unknown_all;_LLC7:;}_LLB5:;}}}struct _tuple15{
union Cyc_CfFlowInfo_RelnOp_union f1;union Cyc_CfFlowInfo_RelnOp_union f2;};static
int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp_union r1,union Cyc_CfFlowInfo_RelnOp_union
r2);static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp_union r1,union
Cyc_CfFlowInfo_RelnOp_union r2){struct _tuple15 _tmp4C1;struct _tuple15 _tmp179=(
_tmp4C1.f1=r1,((_tmp4C1.f2=r2,_tmp4C1)));union Cyc_CfFlowInfo_RelnOp_union _tmp17A;
unsigned int _tmp17B;union Cyc_CfFlowInfo_RelnOp_union _tmp17C;unsigned int _tmp17D;
union Cyc_CfFlowInfo_RelnOp_union _tmp17E;struct Cyc_Absyn_Vardecl*_tmp17F;union Cyc_CfFlowInfo_RelnOp_union
_tmp180;struct Cyc_Absyn_Vardecl*_tmp181;union Cyc_CfFlowInfo_RelnOp_union _tmp182;
struct Cyc_Absyn_Vardecl*_tmp183;union Cyc_CfFlowInfo_RelnOp_union _tmp184;struct
Cyc_Absyn_Vardecl*_tmp185;union Cyc_CfFlowInfo_RelnOp_union _tmp186;unsigned int
_tmp187;union Cyc_CfFlowInfo_RelnOp_union _tmp188;unsigned int _tmp189;union Cyc_CfFlowInfo_RelnOp_union
_tmp18A;struct Cyc_Absyn_Vardecl*_tmp18B;union Cyc_CfFlowInfo_RelnOp_union _tmp18C;
struct Cyc_Absyn_Vardecl*_tmp18D;_LLD3: _tmp17A=_tmp179.f1;if(((_tmp179.f1).EqualConst).tag
!= 0)goto _LLD5;_tmp17B=(_tmp17A.EqualConst).f1;_tmp17C=_tmp179.f2;if(((_tmp179.f2).EqualConst).tag
!= 0)goto _LLD5;_tmp17D=(_tmp17C.EqualConst).f1;_LLD4: return _tmp17B == _tmp17D;
_LLD5: _tmp17E=_tmp179.f1;if(((_tmp179.f1).LessVar).tag != 1)goto _LLD7;_tmp17F=(
_tmp17E.LessVar).f1;_tmp180=_tmp179.f2;if(((_tmp179.f2).LessVar).tag != 1)goto
_LLD7;_tmp181=(_tmp180.LessVar).f1;_LLD6: return _tmp17F == _tmp181;_LLD7: _tmp182=
_tmp179.f1;if(((_tmp179.f1).LessNumelts).tag != 2)goto _LLD9;_tmp183=(_tmp182.LessNumelts).f1;
_tmp184=_tmp179.f2;if(((_tmp179.f2).LessNumelts).tag != 2)goto _LLD9;_tmp185=(
_tmp184.LessNumelts).f1;_LLD8: return _tmp183 == _tmp185;_LLD9: _tmp186=_tmp179.f1;
if(((_tmp179.f1).LessConst).tag != 3)goto _LLDB;_tmp187=(_tmp186.LessConst).f1;
_tmp188=_tmp179.f2;if(((_tmp179.f2).LessConst).tag != 3)goto _LLDB;_tmp189=(
_tmp188.LessConst).f1;_LLDA: return _tmp187 == _tmp189;_LLDB: _tmp18A=_tmp179.f1;if(((
_tmp179.f1).LessEqNumelts).tag != 4)goto _LLDD;_tmp18B=(_tmp18A.LessEqNumelts).f1;
_tmp18C=_tmp179.f2;if(((_tmp179.f2).LessEqNumelts).tag != 4)goto _LLDD;_tmp18D=(
_tmp18C.LessEqNumelts).f1;_LLDC: return _tmp18B == _tmp18D;_LLDD:;_LLDE: return 0;
_LLD2:;}static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(struct _RegionHandle*
r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s);static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(
struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){if(r1s == 
r2s)return r1s;{struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*_tmp18E=
r1s;for(0;_tmp18E != 0;_tmp18E=_tmp18E->tl){struct Cyc_CfFlowInfo_Reln*_tmp18F=(
struct Cyc_CfFlowInfo_Reln*)_tmp18E->hd;int found=0;{struct Cyc_List_List*_tmp190=
r2s;for(0;_tmp190 != 0;_tmp190=_tmp190->tl){struct Cyc_CfFlowInfo_Reln*_tmp191=(
struct Cyc_CfFlowInfo_Reln*)_tmp190->hd;if(_tmp18F == _tmp191  || _tmp18F->vd == 
_tmp191->vd  && Cyc_CfFlowInfo_same_relop(_tmp18F->rop,_tmp191->rop)){{struct Cyc_List_List*
_tmp4C2;res=((_tmp4C2=_region_malloc(r,sizeof(*_tmp4C2)),((_tmp4C2->hd=_tmp18F,((
_tmp4C2->tl=res,_tmp4C2))))));}found=1;break;}}}if(!found)diff=1;}}if(!diff)res=
r1s;return res;}}static void Cyc_CfFlowInfo_remove_f(struct Cyc_List_List**l,struct
Cyc_CfFlowInfo_Place*place);static void Cyc_CfFlowInfo_remove_f(struct Cyc_List_List**
l,struct Cyc_CfFlowInfo_Place*place){struct _handler_cons _tmp193;_push_handler(&
_tmp193);{int _tmp195=0;if(setjmp(_tmp193.handler))_tmp195=1;if(!_tmp195){*l=((
struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_delete_cmp)(Cyc_CfFlowInfo_place_cmp,*
l,place);;_pop_handler();}else{void*_tmp194=(void*)_exn_thrown;void*_tmp197=
_tmp194;_LLE0: if(_tmp197 != Cyc_Core_Not_found)goto _LLE2;_LLE1: goto _LLDF;_LLE2:;
_LLE3:(void)_throw(_tmp197);_LLDF:;}}}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_or_consume(
struct _RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2);static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_or_consume(struct
_RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*mc=c2.may_consume;{struct _RegionHandle _tmp198=
_new_region("temp");struct _RegionHandle*temp=& _tmp198;_push_region(temp);{struct
Cyc_List_List*_tmp199=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rcopy)(temp,c1.may_consume);((void(*)(void(*f)(struct Cyc_List_List**,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List**env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_CfFlowInfo_remove_f,& _tmp199,c2.may_consume);while(_tmp199 != 0){{struct Cyc_List_List*
_tmp4C3;mc=((_tmp4C3=_region_malloc(r,sizeof(*_tmp4C3)),((_tmp4C3->hd=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmp199))->hd,((_tmp4C3->tl=mc,_tmp4C3))))));}
_tmp199=((struct Cyc_List_List*)_check_null(_tmp199))->tl;}};_pop_region(temp);}{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp4C4;return(_tmp4C4.consumed=Cyc_CfFlowInfo_union_place_set(
c1.consumed,c2.consumed,0),((_tmp4C4.may_consume=mc,_tmp4C4)));}}struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_CfFlowInfo_and_consume(struct _RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo
c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(
struct _RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*_tmp19C=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,c1.may_consume,
c2.may_consume);struct Cyc_CfFlowInfo_ConsumeInfo _tmp4C5;return(_tmp4C5.consumed=
Cyc_CfFlowInfo_union_place_set(c1.consumed,c2.consumed,1),((_tmp4C5.may_consume=
_tmp19C,_tmp4C5)));}struct _tuple16{union Cyc_CfFlowInfo_FlowInfo_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo_union f1,union
Cyc_CfFlowInfo_FlowInfo_union f2,int or_consumed);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*
all_changed,union Cyc_CfFlowInfo_FlowInfo_union f1,union Cyc_CfFlowInfo_FlowInfo_union
f2,int or_consumed){struct _tuple16 _tmp4C6;struct _tuple16 _tmp19F=(_tmp4C6.f1=f1,((
_tmp4C6.f2=f2,_tmp4C6)));union Cyc_CfFlowInfo_FlowInfo_union _tmp1A0;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1A1;union Cyc_CfFlowInfo_FlowInfo_union _tmp1A2;struct Cyc_Dict_Dict _tmp1A3;
struct Cyc_List_List*_tmp1A4;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1A5;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1A6;struct Cyc_Dict_Dict _tmp1A7;struct Cyc_List_List*_tmp1A8;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1A9;_LLE5: _tmp1A0=_tmp19F.f1;if(((_tmp19F.f1).BottomFL).tag != 0)goto _LLE7;
_LLE6: return f2;_LLE7: _tmp1A1=_tmp19F.f2;if(((_tmp19F.f2).BottomFL).tag != 0)goto
_LLE9;_LLE8: return f1;_LLE9: _tmp1A2=_tmp19F.f1;if(((_tmp19F.f1).ReachableFL).tag
!= 1)goto _LLE4;_tmp1A3=(_tmp1A2.ReachableFL).f1;_tmp1A4=(_tmp1A2.ReachableFL).f2;
_tmp1A5=(_tmp1A2.ReachableFL).f3;_tmp1A6=_tmp19F.f2;if(((_tmp19F.f2).ReachableFL).tag
!= 1)goto _LLE4;_tmp1A7=(_tmp1A6.ReachableFL).f1;_tmp1A8=(_tmp1A6.ReachableFL).f2;
_tmp1A9=(_tmp1A6.ReachableFL).f3;_LLEA: if(((_tmp1A3.t == _tmp1A7.t  && _tmp1A4 == 
_tmp1A8) && _tmp1A5.may_consume == _tmp1A9.may_consume) && (_tmp1A5.consumed).t == (
_tmp1A9.consumed).t)return f1;if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))
return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{struct
_RegionHandle _tmp1AA=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1AA;
_push_region(rgn);{struct Cyc_CfFlowInfo_EscPile*_tmp4C9;struct Cyc_CfFlowInfo_JoinEnv
_tmp4C8;struct Cyc_CfFlowInfo_JoinEnv _tmp1AB=(_tmp4C8.fenv=fenv,((_tmp4C8.pile=((
_tmp4C9=_region_malloc(rgn,sizeof(*_tmp4C9)),((_tmp4C9->rgn=rgn,((_tmp4C9->places=
0,_tmp4C9)))))),((_tmp4C8.d1=_tmp1A3,((_tmp4C8.d2=_tmp1A7,_tmp4C8)))))));struct
Cyc_Dict_Dict _tmp1AC=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,
void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct
Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_absRval,& _tmp1AB,
_tmp1A3,_tmp1A7);struct Cyc_List_List*_tmp1AD=Cyc_CfFlowInfo_join_relns(fenv->r,
_tmp1A4,_tmp1A8);struct Cyc_CfFlowInfo_ConsumeInfo _tmp1AE=or_consumed?Cyc_CfFlowInfo_or_consume(
fenv->r,_tmp1A5,_tmp1A9): Cyc_CfFlowInfo_and_consume(fenv->r,_tmp1A5,_tmp1A9);
union Cyc_CfFlowInfo_FlowInfo_union _tmp4CA;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1B0=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp4CA.ReachableFL).tag=1,(((
_tmp4CA.ReachableFL).f1=Cyc_CfFlowInfo_escape_these(fenv,_tmp1AB.pile,
all_changed,_tmp1AC),(((_tmp4CA.ReachableFL).f2=_tmp1AD,(((_tmp4CA.ReachableFL).f3=
_tmp1AE,_tmp4CA))))))));_npop_handler(0);return _tmp1B0;};_pop_region(rgn);}_LLE4:;}
struct _tuple17{union Cyc_CfFlowInfo_FlowInfo_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;union Cyc_CfFlowInfo_FlowInfo_union f3;};struct _tuple5 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple5
pr1,struct _tuple5 pr2,int or_consumed);struct _tuple5 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple5
pr1,struct _tuple5 pr2,int or_consumed){union Cyc_CfFlowInfo_FlowInfo_union _tmp1B4;
void*_tmp1B5;struct _tuple5 _tmp1B3=pr1;_tmp1B4=_tmp1B3.f1;_tmp1B5=_tmp1B3.f2;{
union Cyc_CfFlowInfo_FlowInfo_union _tmp1B7;void*_tmp1B8;struct _tuple5 _tmp1B6=pr2;
_tmp1B7=_tmp1B6.f1;_tmp1B8=_tmp1B6.f2;{union Cyc_CfFlowInfo_FlowInfo_union outflow=
Cyc_CfFlowInfo_join_flow(fenv,all_changed,_tmp1B4,_tmp1B7,or_consumed);struct
_tuple17 _tmp4CB;struct _tuple17 _tmp1BA=(_tmp4CB.f1=_tmp1B4,((_tmp4CB.f2=_tmp1B7,((
_tmp4CB.f3=outflow,_tmp4CB)))));union Cyc_CfFlowInfo_FlowInfo_union _tmp1BB;union
Cyc_CfFlowInfo_FlowInfo_union _tmp1BC;union Cyc_CfFlowInfo_FlowInfo_union _tmp1BD;
struct Cyc_Dict_Dict _tmp1BE;union Cyc_CfFlowInfo_FlowInfo_union _tmp1BF;struct Cyc_Dict_Dict
_tmp1C0;union Cyc_CfFlowInfo_FlowInfo_union _tmp1C1;struct Cyc_Dict_Dict _tmp1C2;
struct Cyc_List_List*_tmp1C3;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1C4;_LLEC:
_tmp1BB=_tmp1BA.f1;if(((_tmp1BA.f1).BottomFL).tag != 0)goto _LLEE;_LLED: {struct
_tuple5 _tmp4CC;return(_tmp4CC.f1=outflow,((_tmp4CC.f2=_tmp1B8,_tmp4CC)));}_LLEE:
_tmp1BC=_tmp1BA.f2;if(((_tmp1BA.f2).BottomFL).tag != 0)goto _LLF0;_LLEF: {struct
_tuple5 _tmp4CD;return(_tmp4CD.f1=outflow,((_tmp4CD.f2=_tmp1B5,_tmp4CD)));}_LLF0:
_tmp1BD=_tmp1BA.f1;if(((_tmp1BA.f1).ReachableFL).tag != 1)goto _LLF2;_tmp1BE=(
_tmp1BD.ReachableFL).f1;_tmp1BF=_tmp1BA.f2;if(((_tmp1BA.f2).ReachableFL).tag != 1)
goto _LLF2;_tmp1C0=(_tmp1BF.ReachableFL).f1;_tmp1C1=_tmp1BA.f3;if(((_tmp1BA.f3).ReachableFL).tag
!= 1)goto _LLF2;_tmp1C2=(_tmp1C1.ReachableFL).f1;_tmp1C3=(_tmp1C1.ReachableFL).f2;
_tmp1C4=(_tmp1C1.ReachableFL).f3;_LLF1: if(((int(*)(int ignore,void*r1,void*r2))
Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp1B5,_tmp1B8)){struct _tuple5 _tmp4CE;
return(_tmp4CE.f1=outflow,((_tmp4CE.f2=_tmp1B8,_tmp4CE)));}if(((int(*)(int ignore,
void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp1B8,_tmp1B5)){
struct _tuple5 _tmp4CF;return(_tmp4CF.f1=outflow,((_tmp4CF.f2=_tmp1B5,_tmp4CF)));}{
struct _RegionHandle _tmp1C9=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1C9;
_push_region(rgn);{struct Cyc_CfFlowInfo_EscPile*_tmp4D2;struct Cyc_CfFlowInfo_JoinEnv
_tmp4D1;struct Cyc_CfFlowInfo_JoinEnv _tmp1CA=(_tmp4D1.fenv=fenv,((_tmp4D1.pile=((
_tmp4D2=_region_malloc(rgn,sizeof(*_tmp4D2)),((_tmp4D2->rgn=rgn,((_tmp4D2->places=
0,_tmp4D2)))))),((_tmp4D1.d1=_tmp1BE,((_tmp4D1.d2=_tmp1C0,_tmp4D1)))))));void*
_tmp1CB=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(&
_tmp1CA,0,_tmp1B5,_tmp1B8);union Cyc_CfFlowInfo_FlowInfo_union _tmp4D5;struct
_tuple5 _tmp4D4;struct _tuple5 _tmp1CE=(_tmp4D4.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp4D5.ReachableFL).tag=1,(((_tmp4D5.ReachableFL).f1=Cyc_CfFlowInfo_escape_these(
fenv,_tmp1CA.pile,all_changed,_tmp1C2),(((_tmp4D5.ReachableFL).f2=_tmp1C3,(((
_tmp4D5.ReachableFL).f3=_tmp1C4,_tmp4D5)))))))),((_tmp4D4.f2=_tmp1CB,_tmp4D4)));
_npop_handler(0);return _tmp1CE;};_pop_region(rgn);}_LLF2:;_LLF3: {struct Cyc_Core_Impossible_struct
_tmp4DB;const char*_tmp4DA;struct Cyc_Core_Impossible_struct*_tmp4D9;(int)_throw((
void*)((_tmp4D9=_cycalloc(sizeof(*_tmp4D9)),((_tmp4D9[0]=((_tmp4DB.tag=Cyc_Core_Impossible,((
_tmp4DB.f1=((_tmp4DA="join_flow_and_rval: BottomFL outflow",_tag_dyneither(
_tmp4DA,sizeof(char),37))),_tmp4DB)))),_tmp4D9)))));}_LLEB:;}}}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp4E2(struct Cyc_CfFlowInfo_AfterEnv**
env,unsigned int*_tmp4E1,unsigned int*_tmp4E0,void***_tmp4DE,struct
_dyneither_ptr*_tmp1D9,struct _dyneither_ptr*_tmp1D7){for(*_tmp4E1=0;*_tmp4E1 < *
_tmp4E0;(*_tmp4E1)++){(*_tmp4DE)[*_tmp4E1]=Cyc_CfFlowInfo_after_absRval_d(*env,(
int)*_tmp4E1,((void**)(*_tmp1D7).curr)[(int)*_tmp4E1],*((void**)
_check_dyneither_subscript(*_tmp1D9,sizeof(void*),(int)*_tmp4E1)));}}static void*
Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);int
changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);if(
changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*
r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);if(changed1){if(
!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))return r1;env->changed=
Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,
env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{struct _tuple0 _tmp4DC;
struct _tuple0 _tmp1D5=(_tmp4DC.f1=r1,((_tmp4DC.f2=r2,_tmp4DC)));void*_tmp1D6;
struct _dyneither_ptr _tmp1D7;void*_tmp1D8;struct _dyneither_ptr _tmp1D9;_LLF5:
_tmp1D6=_tmp1D5.f1;if(_tmp1D6 <= (void*)3)goto _LLF7;if(*((int*)_tmp1D6)!= 4)goto
_LLF7;_tmp1D7=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1D6)->f1;_tmp1D8=
_tmp1D5.f2;if(_tmp1D8 <= (void*)3)goto _LLF7;if(*((int*)_tmp1D8)!= 4)goto _LLF7;
_tmp1D9=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1D8)->f1;_LLF6: {
unsigned int _tmp4E1;unsigned int _tmp4E0;struct _dyneither_ptr _tmp4DF;void**
_tmp4DE;unsigned int _tmp4DD;struct _dyneither_ptr new_d=(_tmp4DD=
_get_dyneither_size(_tmp1D7,sizeof(void*)),((_tmp4DE=(void**)_region_malloc(((
env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp4DD)),((_tmp4DF=
_tag_dyneither(_tmp4DE,sizeof(void*),_tmp4DD),((((_tmp4E0=_tmp4DD,_tmp4E2(& env,&
_tmp4E1,& _tmp4E0,& _tmp4DE,& _tmp1D9,& _tmp1D7))),_tmp4DF)))))));int change=0;{int i=
0;for(0;i < _get_dyneither_size(_tmp1D7,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1D7.curr)[i]){
change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp1D7,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp1D9,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;{struct Cyc_CfFlowInfo_Aggregate_struct _tmp4E5;struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp4E4;return(void*)((_tmp4E4=_region_malloc(((env->joinenv).fenv)->r,sizeof(*
_tmp4E4)),((_tmp4E4[0]=((_tmp4E5.tag=4,((_tmp4E5.f1=new_d,_tmp4E5)))),_tmp4E4))));}}
_LLF7:;_LLF8: {struct Cyc_Core_Impossible_struct _tmp4EB;const char*_tmp4EA;struct
Cyc_Core_Impossible_struct*_tmp4E9;(int)_throw((void*)((_tmp4E9=_cycalloc(
sizeof(*_tmp4E9)),((_tmp4E9[0]=((_tmp4EB.tag=Cyc_Core_Impossible,((_tmp4EB.f1=((
_tmp4EA="after_absRval -- non-aggregates!",_tag_dyneither(_tmp4EA,sizeof(char),
33))),_tmp4EB)))),_tmp4E9)))));}_LLF4:;}}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2);static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp1E3=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged
_tmp1E4=env->changed;{struct Cyc_List_List*_tmp4EC;*env->last_field_cell=((
_tmp4EC=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp4EC)),((_tmp4EC->hd=(
void*)key,((_tmp4EC->tl=0,_tmp4EC))))));}env->last_field_cell=(struct Cyc_List_List**)&((
struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{void*_tmp1E6=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp1E3;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp1E4;return _tmp1E6;}}}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2);static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2)return r1;{
struct Cyc_CfFlowInfo_Place _tmp4ED;*env->curr_place=((_tmp4ED.root=(void*)root,((
_tmp4ED.fields=0,_tmp4ED))));}env->last_field_cell=(struct Cyc_List_List**)&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo_union f1,union
Cyc_CfFlowInfo_FlowInfo_union f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2);
union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo_union f1,union
Cyc_CfFlowInfo_FlowInfo_union f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){
struct _tuple16 _tmp4EE;struct _tuple16 _tmp1E9=(_tmp4EE.f1=f1,((_tmp4EE.f2=f2,
_tmp4EE)));union Cyc_CfFlowInfo_FlowInfo_union _tmp1EA;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1EB;union Cyc_CfFlowInfo_FlowInfo_union _tmp1EC;struct Cyc_Dict_Dict _tmp1ED;
struct Cyc_List_List*_tmp1EE;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1EF;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1F0;struct Cyc_Dict_Dict _tmp1F1;struct Cyc_List_List*_tmp1F2;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1F3;_LLFA: _tmp1EA=_tmp1E9.f1;if(((_tmp1E9.f1).BottomFL).tag != 0)goto _LLFC;
_LLFB: goto _LLFD;_LLFC: _tmp1EB=_tmp1E9.f2;if(((_tmp1E9.f2).BottomFL).tag != 0)goto
_LLFE;_LLFD: {union Cyc_CfFlowInfo_FlowInfo_union _tmp4EF;return(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp4EF.BottomFL).tag=0,_tmp4EF));}_LLFE: _tmp1EC=_tmp1E9.f1;if(((_tmp1E9.f1).ReachableFL).tag
!= 1)goto _LLF9;_tmp1ED=(_tmp1EC.ReachableFL).f1;_tmp1EE=(_tmp1EC.ReachableFL).f2;
_tmp1EF=(_tmp1EC.ReachableFL).f3;_tmp1F0=_tmp1E9.f2;if(((_tmp1E9.f2).ReachableFL).tag
!= 1)goto _LLF9;_tmp1F1=(_tmp1F0.ReachableFL).f1;_tmp1F2=(_tmp1F0.ReachableFL).f2;
_tmp1F3=(_tmp1F0.ReachableFL).f3;_LLFF: if(((_tmp1ED.t == _tmp1F1.t  && _tmp1EE == 
_tmp1F2) && _tmp1EF.may_consume == _tmp1F3.may_consume) && (_tmp1EF.consumed).t == (
_tmp1F3.consumed).t)return f1;{struct _RegionHandle _tmp1F5=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp1F5;_push_region(rgn);{struct Cyc_CfFlowInfo_Place*
_tmp1F6=fenv->dummy_place;struct Cyc_CfFlowInfo_JoinEnv _tmp4F5;struct Cyc_CfFlowInfo_EscPile*
_tmp4F4;struct Cyc_CfFlowInfo_AfterEnv _tmp4F3;struct Cyc_CfFlowInfo_AfterEnv
_tmp1F7=(_tmp4F3.joinenv=((_tmp4F5.fenv=fenv,((_tmp4F5.pile=((_tmp4F4=
_region_malloc(rgn,sizeof(*_tmp4F4)),((_tmp4F4->rgn=rgn,((_tmp4F4->places=0,
_tmp4F4)))))),((_tmp4F5.d1=_tmp1ED,((_tmp4F5.d2=_tmp1F1,_tmp4F5)))))))),((
_tmp4F3.chg1=chg1,((_tmp4F3.chg2=chg2,((_tmp4F3.curr_place=_tmp1F6,((_tmp4F3.last_field_cell=(
struct Cyc_List_List**)& _tmp1F6->fields,((_tmp4F3.changed=Cyc_CfFlowInfo_Neither,
_tmp4F3)))))))))));struct Cyc_Dict_Dict _tmp1F8=((struct Cyc_Dict_Dict(*)(void*(*f)(
struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp1F7,_tmp1ED,_tmp1F1);struct Cyc_List_List*_tmp1F9=Cyc_CfFlowInfo_join_relns(
fenv->r,_tmp1EE,_tmp1F2);struct Cyc_CfFlowInfo_ConsumeInfo _tmp1FA=Cyc_CfFlowInfo_and_consume(
fenv->r,_tmp1EF,_tmp1F3);union Cyc_CfFlowInfo_FlowInfo_union _tmp4F6;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1FC=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp4F6.ReachableFL).tag=1,(((
_tmp4F6.ReachableFL).f1=Cyc_CfFlowInfo_escape_these(fenv,(_tmp1F7.joinenv).pile,
all_changed,_tmp1F8),(((_tmp4F6.ReachableFL).f2=_tmp1F9,(((_tmp4F6.ReachableFL).f3=
_tmp1FA,_tmp4F6))))))));_npop_handler(0);return _tmp1FC;};_pop_region(rgn);}_LLF9:;}
static int Cyc_CfFlowInfo_tag_cmps_lessthan_approx(struct Cyc_List_List*l1,struct
Cyc_List_List*l2);static int Cyc_CfFlowInfo_tag_cmps_lessthan_approx(struct Cyc_List_List*
l1,struct Cyc_List_List*l2){for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp201;void*_tmp202;void*_tmp203;struct Cyc_CfFlowInfo_TagCmp*_tmp200=(struct Cyc_CfFlowInfo_TagCmp*)
l2->hd;_tmp201=*_tmp200;_tmp202=(void*)_tmp201.cmp;_tmp203=(void*)_tmp201.bd;{
struct Cyc_List_List*_tmp204=l1;for(0;_tmp204 != 0;_tmp204=_tmp204->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp206;void*_tmp207;void*_tmp208;struct Cyc_CfFlowInfo_TagCmp*_tmp205=(struct Cyc_CfFlowInfo_TagCmp*)
_tmp204->hd;_tmp206=*_tmp205;_tmp207=(void*)_tmp206.cmp;_tmp208=(void*)_tmp206.bd;
if(_tmp207 == _tmp202  && Cyc_Tcutil_typecmp(_tmp208,_tmp203)== 0)break;}if(
_tmp204 == 0)return 0;}}return 1;}static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2);static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2){if(r1 == r2)return 1;{struct _tuple0 _tmp4F7;struct
_tuple0 _tmp20A=(_tmp4F7.f1=r1,((_tmp4F7.f2=r2,_tmp4F7)));void*_tmp20B;struct Cyc_CfFlowInfo_Place*
_tmp20C;void*_tmp20D;struct Cyc_CfFlowInfo_Place*_tmp20E;void*_tmp20F;void*
_tmp210;void*_tmp211;struct _dyneither_ptr _tmp212;void*_tmp213;struct
_dyneither_ptr _tmp214;void*_tmp215;void*_tmp216;void*_tmp217;void*_tmp218;void*
_tmp219;void*_tmp21A;void*_tmp21B;struct Cyc_List_List*_tmp21C;void*_tmp21D;
struct Cyc_List_List*_tmp21E;void*_tmp21F;_LL101: _tmp20B=_tmp20A.f1;if(_tmp20B <= (
void*)3)goto _LL103;if(*((int*)_tmp20B)!= 2)goto _LL103;_tmp20C=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp20B)->f1;_tmp20D=_tmp20A.f2;if(_tmp20D <= (void*)3)goto _LL103;if(*((int*)
_tmp20D)!= 2)goto _LL103;_tmp20E=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp20D)->f1;
_LL102: return Cyc_CfFlowInfo_place_cmp(_tmp20C,_tmp20E)== 0;_LL103: _tmp20F=
_tmp20A.f1;if(_tmp20F <= (void*)3)goto _LL105;if(*((int*)_tmp20F)!= 2)goto _LL105;
_LL104: goto _LL106;_LL105: _tmp210=_tmp20A.f2;if(_tmp210 <= (void*)3)goto _LL107;if(*((
int*)_tmp210)!= 2)goto _LL107;_LL106: return 0;_LL107: _tmp211=_tmp20A.f1;if(_tmp211
<= (void*)3)goto _LL109;if(*((int*)_tmp211)!= 4)goto _LL109;_tmp212=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp211)->f1;_tmp213=_tmp20A.f2;if(_tmp213 <= (void*)3)goto _LL109;if(*((int*)
_tmp213)!= 4)goto _LL109;_tmp214=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp213)->f1;
_LL108: if(_tmp212.curr == _tmp214.curr)return 1;{int i=0;for(0;i < 
_get_dyneither_size(_tmp212,sizeof(void*));++ i){if(!((int(*)(int ignore,void*r1,
void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp212.curr)[i],*((
void**)_check_dyneither_subscript(_tmp214,sizeof(void*),i))))return 0;}}return 1;
_LL109: _tmp215=_tmp20A.f2;if((int)_tmp215 != 2)goto _LL10B;_LL10A: return r1 == (void*)
1;_LL10B: _tmp216=_tmp20A.f2;if((int)_tmp216 != 0)goto _LL10D;_LL10C: goto _LL10E;
_LL10D: _tmp217=_tmp20A.f2;if((int)_tmp217 != 1)goto _LL10F;_LL10E: return 0;_LL10F:
_tmp218=_tmp20A.f1;if(_tmp218 <= (void*)3)goto _LL111;if(*((int*)_tmp218)!= 1)goto
_LL111;_tmp219=_tmp20A.f2;if(_tmp219 <= (void*)3)goto _LL111;if(*((int*)_tmp219)!= 
1)goto _LL111;_LL110: goto _LL100;_LL111: _tmp21A=_tmp20A.f1;if(_tmp21A <= (void*)3)
goto _LL113;if(*((int*)_tmp21A)!= 1)goto _LL113;_LL112: return 0;_LL113: _tmp21B=
_tmp20A.f1;if(_tmp21B <= (void*)3)goto _LL115;if(*((int*)_tmp21B)!= 3)goto _LL115;
_tmp21C=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp21B)->f1;_tmp21D=_tmp20A.f2;
if(_tmp21D <= (void*)3)goto _LL115;if(*((int*)_tmp21D)!= 3)goto _LL115;_tmp21E=((
struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp21D)->f1;_LL114: return Cyc_CfFlowInfo_tag_cmps_lessthan_approx(
_tmp21C,_tmp21E);_LL115: _tmp21F=_tmp20A.f2;if(_tmp21F <= (void*)3)goto _LL117;if(*((
int*)_tmp21F)!= 3)goto _LL117;_LL116: return 0;_LL117:;_LL118: goto _LL100;_LL100:;}{
struct _tuple0 _tmp4F8;struct _tuple0 _tmp221=(_tmp4F8.f1=Cyc_CfFlowInfo_initlevel_approx(
r1),((_tmp4F8.f2=Cyc_CfFlowInfo_initlevel_approx(r2),_tmp4F8)));void*_tmp222;
void*_tmp223;void*_tmp224;void*_tmp225;void*_tmp226;void*_tmp227;_LL11A: _tmp222=
_tmp221.f1;if((int)_tmp222 != 2)goto _LL11C;_tmp223=_tmp221.f2;if((int)_tmp223 != 2)
goto _LL11C;_LL11B: return 1;_LL11C: _tmp224=_tmp221.f2;if((int)_tmp224 != 0)goto
_LL11E;_LL11D: return 1;_LL11E: _tmp225=_tmp221.f1;if((int)_tmp225 != 0)goto _LL120;
_LL11F: return 0;_LL120: _tmp226=_tmp221.f2;if((int)_tmp226 != 1)goto _LL122;_LL121:
return 1;_LL122: _tmp227=_tmp221.f1;if((int)_tmp227 != 1)goto _LL119;_LL123: return 0;
_LL119:;}}static int Cyc_CfFlowInfo_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*
r1s);static int Cyc_CfFlowInfo_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*
r1s){if(r1s == r2s)return 1;for(0;r1s != 0;r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*
_tmp228=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;int found=0;{struct Cyc_List_List*
_tmp229=r2s;for(0;_tmp229 != 0;_tmp229=_tmp229->tl){struct Cyc_CfFlowInfo_Reln*
_tmp22A=(struct Cyc_CfFlowInfo_Reln*)_tmp229->hd;if(_tmp228 == _tmp22A  || _tmp228->vd
== _tmp22A->vd  && Cyc_CfFlowInfo_same_relop(_tmp228->rop,_tmp22A->rop)){found=1;
break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_consume_approx(struct Cyc_CfFlowInfo_ConsumeInfo
c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);int Cyc_CfFlowInfo_consume_approx(struct
Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2){if(c1.may_consume
!= c2.may_consume){struct Cyc_List_List*_tmp22B=c1.may_consume;struct Cyc_List_List*
_tmp22C=c2.may_consume;for(0;_tmp22C != 0  && _tmp22B != 0;_tmp22C=_tmp22C->tl){if((
unsigned int)_tmp22B == (unsigned int)_tmp22C)goto compare_consumed_set;{int
_tmp22D=Cyc_CfFlowInfo_place_cmp((struct Cyc_CfFlowInfo_Place*)_tmp22B->hd,(
struct Cyc_CfFlowInfo_Place*)_tmp22C->hd);if(_tmp22D == 0)_tmp22B=_tmp22B->tl;}}
if(_tmp22B != 0)return 0;}compare_consumed_set: return((int(*)(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2))Cyc_CfFlowInfo_place_set_subset)(c1.consumed,c2.consumed);}
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo_union f1,
union Cyc_CfFlowInfo_FlowInfo_union f2);int Cyc_CfFlowInfo_flow_lessthan_approx(
union Cyc_CfFlowInfo_FlowInfo_union f1,union Cyc_CfFlowInfo_FlowInfo_union f2){
struct _tuple16 _tmp4F9;struct _tuple16 _tmp22F=(_tmp4F9.f1=f1,((_tmp4F9.f2=f2,
_tmp4F9)));union Cyc_CfFlowInfo_FlowInfo_union _tmp230;union Cyc_CfFlowInfo_FlowInfo_union
_tmp231;union Cyc_CfFlowInfo_FlowInfo_union _tmp232;struct Cyc_Dict_Dict _tmp233;
struct Cyc_List_List*_tmp234;struct Cyc_CfFlowInfo_ConsumeInfo _tmp235;union Cyc_CfFlowInfo_FlowInfo_union
_tmp236;struct Cyc_Dict_Dict _tmp237;struct Cyc_List_List*_tmp238;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp239;_LL125: _tmp230=_tmp22F.f1;if(((_tmp22F.f1).BottomFL).tag != 0)goto _LL127;
_LL126: return 1;_LL127: _tmp231=_tmp22F.f2;if(((_tmp22F.f2).BottomFL).tag != 0)goto
_LL129;_LL128: return 0;_LL129: _tmp232=_tmp22F.f1;if(((_tmp22F.f1).ReachableFL).tag
!= 1)goto _LL124;_tmp233=(_tmp232.ReachableFL).f1;_tmp234=(_tmp232.ReachableFL).f2;
_tmp235=(_tmp232.ReachableFL).f3;_tmp236=_tmp22F.f2;if(((_tmp22F.f2).ReachableFL).tag
!= 1)goto _LL124;_tmp237=(_tmp236.ReachableFL).f1;_tmp238=(_tmp236.ReachableFL).f2;
_tmp239=(_tmp236.ReachableFL).f3;_LL12A: if(((_tmp233.t == _tmp237.t  && _tmp234 == 
_tmp238) && _tmp235.may_consume == _tmp239.may_consume) && (_tmp235.consumed).t == (
_tmp239.consumed).t)return 1;return(Cyc_Dict_forall_intersect(Cyc_CfFlowInfo_absRval_lessthan_approx,
_tmp233,_tmp237) && Cyc_CfFlowInfo_relns_approx(_tmp234,_tmp238)) && Cyc_CfFlowInfo_consume_approx(
_tmp235,_tmp239);_LL124:;}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(
struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v);struct
Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*
rs,struct Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=0;for(p=rs;!found
 && p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp23A=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp23A->vd == v){found=1;break;}{union Cyc_CfFlowInfo_RelnOp_union
_tmp23B=_tmp23A->rop;struct Cyc_Absyn_Vardecl*_tmp23C;struct Cyc_Absyn_Vardecl*
_tmp23D;struct Cyc_Absyn_Vardecl*_tmp23E;_LL12C: if((_tmp23B.LessVar).tag != 1)goto
_LL12E;_tmp23C=(_tmp23B.LessVar).f1;_LL12D: _tmp23D=_tmp23C;goto _LL12F;_LL12E: if((
_tmp23B.LessNumelts).tag != 2)goto _LL130;_tmp23D=(_tmp23B.LessNumelts).f1;_LL12F:
_tmp23E=_tmp23D;goto _LL131;_LL130: if((_tmp23B.LessEqNumelts).tag != 4)goto _LL132;
_tmp23E=(_tmp23B.LessEqNumelts).f1;_LL131: if(v == _tmp23E)found=1;goto _LL12B;
_LL132:;_LL133: goto _LL12B;_LL12B:;}}if(!found)return rs;{struct Cyc_List_List*
_tmp23F=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp240=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp240->vd != v){{union Cyc_CfFlowInfo_RelnOp_union _tmp241=_tmp240->rop;
struct Cyc_Absyn_Vardecl*_tmp242;struct Cyc_Absyn_Vardecl*_tmp243;struct Cyc_Absyn_Vardecl*
_tmp244;_LL135: if((_tmp241.LessVar).tag != 1)goto _LL137;_tmp242=(_tmp241.LessVar).f1;
_LL136: _tmp243=_tmp242;goto _LL138;_LL137: if((_tmp241.LessNumelts).tag != 2)goto
_LL139;_tmp243=(_tmp241.LessNumelts).f1;_LL138: _tmp244=_tmp243;goto _LL13A;_LL139:
if((_tmp241.LessEqNumelts).tag != 4)goto _LL13B;_tmp244=(_tmp241.LessEqNumelts).f1;
_LL13A: if(v == _tmp244)continue;goto _LL134;_LL13B:;_LL13C: goto _LL134;_LL134:;}{
struct Cyc_List_List*_tmp4FA;_tmp23F=((_tmp4FA=_region_malloc(rgn,sizeof(*_tmp4FA)),((
_tmp4FA->hd=_tmp240,((_tmp4FA->tl=_tmp23F,_tmp4FA))))));}}}return _tmp23F;}}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*rgn,struct
Cyc_List_List*r,struct Cyc_Absyn_Exp*e);struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*
_tmp246=(void*)e->r;void*_tmp247;struct Cyc_Absyn_Vardecl*_tmp248;void*_tmp249;
struct Cyc_Absyn_Vardecl*_tmp24A;void*_tmp24B;struct Cyc_Absyn_Vardecl*_tmp24C;
void*_tmp24D;struct Cyc_Absyn_Vardecl*_tmp24E;_LL13E: if(*((int*)_tmp246)!= 1)goto
_LL140;_tmp247=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp246)->f2;if(_tmp247 <= (
void*)1)goto _LL140;if(*((int*)_tmp247)!= 0)goto _LL140;_tmp248=((struct Cyc_Absyn_Global_b_struct*)
_tmp247)->f1;_LL13F: _tmp24A=_tmp248;goto _LL141;_LL140: if(*((int*)_tmp246)!= 1)
goto _LL142;_tmp249=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp246)->f2;if(
_tmp249 <= (void*)1)goto _LL142;if(*((int*)_tmp249)!= 2)goto _LL142;_tmp24A=((
struct Cyc_Absyn_Param_b_struct*)_tmp249)->f1;_LL141: _tmp24C=_tmp24A;goto _LL143;
_LL142: if(*((int*)_tmp246)!= 1)goto _LL144;_tmp24B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp246)->f2;if(_tmp24B <= (void*)1)goto _LL144;if(*((int*)_tmp24B)!= 3)goto _LL144;
_tmp24C=((struct Cyc_Absyn_Local_b_struct*)_tmp24B)->f1;_LL143: _tmp24E=_tmp24C;
goto _LL145;_LL144: if(*((int*)_tmp246)!= 1)goto _LL146;_tmp24D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp246)->f2;if(_tmp24D <= (void*)1)goto _LL146;if(*((int*)_tmp24D)!= 4)goto _LL146;
_tmp24E=((struct Cyc_Absyn_Pat_b_struct*)_tmp24D)->f1;_LL145: if(!_tmp24E->escapes)
return Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp24E);goto _LL13D;_LL146:;_LL147: goto
_LL13D;_LL13D:;}return r;}struct Cyc_CfFlowInfo_TagCmp*Cyc_CfFlowInfo_copy_tagcmp(
struct _RegionHandle*r2,struct Cyc_CfFlowInfo_TagCmp*t);struct Cyc_CfFlowInfo_TagCmp*
Cyc_CfFlowInfo_copy_tagcmp(struct _RegionHandle*r2,struct Cyc_CfFlowInfo_TagCmp*t){
struct Cyc_CfFlowInfo_TagCmp*_tmp4FB;return(_tmp4FB=_region_malloc(r2,sizeof(*
_tmp4FB)),((_tmp4FB[0]=*t,_tmp4FB)));}struct Cyc_List_List*Cyc_CfFlowInfo_copy_tagcmps(
struct _RegionHandle*r2,struct Cyc_List_List*ts);struct Cyc_List_List*Cyc_CfFlowInfo_copy_tagcmps(
struct _RegionHandle*r2,struct Cyc_List_List*ts){return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_CfFlowInfo_TagCmp*(*f)(struct _RegionHandle*,
struct Cyc_CfFlowInfo_TagCmp*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r2,Cyc_CfFlowInfo_copy_tagcmp,r2,ts);}struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(
struct _RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r);struct Cyc_CfFlowInfo_Reln*
Cyc_CfFlowInfo_copy_reln(struct _RegionHandle*r2,struct Cyc_CfFlowInfo_Reln*r){
struct Cyc_CfFlowInfo_Reln*_tmp4FC;return(_tmp4FC=_region_malloc(r2,sizeof(*
_tmp4FC)),((_tmp4FC[0]=*r,_tmp4FC)));}struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(
struct _RegionHandle*r2,struct Cyc_List_List*relns);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(
struct _RegionHandle*r2,struct Cyc_List_List*relns){return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*f)(struct _RegionHandle*,struct
Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*
r1,struct Cyc_List_List*r2);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*r1,
struct Cyc_List_List*r2){for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){struct Cyc_CfFlowInfo_Reln*
_tmp251=(struct Cyc_CfFlowInfo_Reln*)r1->hd;struct Cyc_CfFlowInfo_Reln*_tmp252=(
struct Cyc_CfFlowInfo_Reln*)r2->hd;if(_tmp251->vd != _tmp252->vd  || !Cyc_CfFlowInfo_same_relop(
_tmp251->rop,_tmp252->rop))return 0;}if(r1 != 0  || r2 != 0)return 0;else{return 1;}}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,
struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)return r;r=Cyc_CfFlowInfo_reln_kill_var(
rgn,r,v);{void*_tmp253=(void*)e->r;struct Cyc_Absyn_MallocInfo _tmp254;struct Cyc_Absyn_Exp*
_tmp255;int _tmp256;_LL149: if(*((int*)_tmp253)!= 35)goto _LL14B;_tmp254=((struct
Cyc_Absyn_Malloc_e_struct*)_tmp253)->f1;_tmp255=_tmp254.num_elts;_tmp256=_tmp254.fat_result;
if(_tmp256 != 1)goto _LL14B;_LL14A: malloc_loop: {void*_tmp257=(void*)_tmp255->r;
struct Cyc_Absyn_Exp*_tmp258;void*_tmp259;struct Cyc_Absyn_Vardecl*_tmp25A;void*
_tmp25B;struct Cyc_Absyn_Vardecl*_tmp25C;void*_tmp25D;struct Cyc_Absyn_Vardecl*
_tmp25E;void*_tmp25F;struct Cyc_Absyn_Vardecl*_tmp260;_LL14E: if(*((int*)_tmp257)
!= 15)goto _LL150;_tmp258=((struct Cyc_Absyn_Cast_e_struct*)_tmp257)->f2;_LL14F:
_tmp255=_tmp258;goto malloc_loop;_LL150: if(*((int*)_tmp257)!= 1)goto _LL152;
_tmp259=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp257)->f2;if(_tmp259 <= (void*)
1)goto _LL152;if(*((int*)_tmp259)!= 4)goto _LL152;_tmp25A=((struct Cyc_Absyn_Pat_b_struct*)
_tmp259)->f1;_LL151: _tmp25C=_tmp25A;goto _LL153;_LL152: if(*((int*)_tmp257)!= 1)
goto _LL154;_tmp25B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp257)->f2;if(
_tmp25B <= (void*)1)goto _LL154;if(*((int*)_tmp25B)!= 3)goto _LL154;_tmp25C=((
struct Cyc_Absyn_Local_b_struct*)_tmp25B)->f1;_LL153: _tmp25E=_tmp25C;goto _LL155;
_LL154: if(*((int*)_tmp257)!= 1)goto _LL156;_tmp25D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp257)->f2;if(_tmp25D <= (void*)1)goto _LL156;if(*((int*)_tmp25D)!= 2)goto _LL156;
_tmp25E=((struct Cyc_Absyn_Param_b_struct*)_tmp25D)->f1;_LL155: _tmp260=_tmp25E;
goto _LL157;_LL156: if(*((int*)_tmp257)!= 1)goto _LL158;_tmp25F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp257)->f2;if(_tmp25F <= (void*)1)goto _LL158;if(*((int*)_tmp25F)!= 0)goto _LL158;
_tmp260=((struct Cyc_Absyn_Global_b_struct*)_tmp25F)->f1;_LL157: if(_tmp260->escapes)
return r;{struct Cyc_CfFlowInfo_Reln*_tmp502;union Cyc_CfFlowInfo_RelnOp_union
_tmp501;struct Cyc_List_List*_tmp500;return(_tmp500=_region_malloc(rgn,sizeof(*
_tmp500)),((_tmp500->hd=((_tmp502=_region_malloc(rgn,sizeof(*_tmp502)),((_tmp502->vd=
_tmp260,((_tmp502->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((_tmp501.LessEqNumelts).tag=
4,(((_tmp501.LessEqNumelts).f1=v,_tmp501)))),_tmp502)))))),((_tmp500->tl=r,
_tmp500)))));}_LL158:;_LL159: return r;_LL14D:;}_LL14B:;_LL14C: goto _LL148;_LL148:;}{
void*_tmp264=Cyc_Tcutil_compress((void*)v->type);_LL15B: if(_tmp264 <= (void*)4)
goto _LL15D;if(*((int*)_tmp264)!= 5)goto _LL15D;_LL15C: goto _LL15A;_LL15D:;_LL15E:
return r;_LL15A:;}loop: {void*_tmp265=(void*)e->r;struct Cyc_Absyn_Exp*_tmp266;
union Cyc_Absyn_Cnst_union _tmp267;int _tmp268;void*_tmp269;struct Cyc_List_List*
_tmp26A;struct Cyc_List_List _tmp26B;struct Cyc_List_List*_tmp26C;struct Cyc_List_List
_tmp26D;struct Cyc_Absyn_Exp*_tmp26E;void*_tmp26F;struct Cyc_List_List*_tmp270;
struct Cyc_List_List _tmp271;struct Cyc_Absyn_Exp*_tmp272;_LL160: if(*((int*)_tmp265)
!= 15)goto _LL162;_tmp266=((struct Cyc_Absyn_Cast_e_struct*)_tmp265)->f2;_LL161: e=
_tmp266;goto loop;_LL162: if(*((int*)_tmp265)!= 0)goto _LL164;_tmp267=((struct Cyc_Absyn_Const_e_struct*)
_tmp265)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp265)->f1).Int_c).tag != 2)
goto _LL164;_tmp268=(_tmp267.Int_c).f2;_LL163: {struct Cyc_CfFlowInfo_Reln*_tmp508;
union Cyc_CfFlowInfo_RelnOp_union _tmp507;struct Cyc_List_List*_tmp506;return(
_tmp506=_region_malloc(rgn,sizeof(*_tmp506)),((_tmp506->hd=((_tmp508=
_region_malloc(rgn,sizeof(*_tmp508)),((_tmp508->vd=v,((_tmp508->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((
_tmp507.EqualConst).tag=0,(((_tmp507.EqualConst).f1=(unsigned int)_tmp268,
_tmp507)))),_tmp508)))))),((_tmp506->tl=r,_tmp506)))));}_LL164: if(*((int*)
_tmp265)!= 3)goto _LL166;_tmp269=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp265)->f1;if((int)_tmp269 != 4)goto _LL166;_tmp26A=((struct Cyc_Absyn_Primop_e_struct*)
_tmp265)->f2;if(_tmp26A == 0)goto _LL166;_tmp26B=*_tmp26A;_tmp26C=_tmp26B.tl;if(
_tmp26C == 0)goto _LL166;_tmp26D=*_tmp26C;_tmp26E=(struct Cyc_Absyn_Exp*)_tmp26D.hd;
_LL165:{void*_tmp276=(void*)_tmp26E->r;void*_tmp277;struct Cyc_List_List*_tmp278;
struct Cyc_List_List _tmp279;struct Cyc_Absyn_Exp*_tmp27A;_LL16B: if(*((int*)_tmp276)
!= 3)goto _LL16D;_tmp277=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp276)->f1;
_tmp278=((struct Cyc_Absyn_Primop_e_struct*)_tmp276)->f2;if(_tmp278 == 0)goto
_LL16D;_tmp279=*_tmp278;_tmp27A=(struct Cyc_Absyn_Exp*)_tmp279.hd;_LL16C:{void*
_tmp27B=(void*)_tmp27A->r;void*_tmp27C;struct Cyc_Absyn_Vardecl*_tmp27D;void*
_tmp27E;struct Cyc_Absyn_Vardecl*_tmp27F;void*_tmp280;struct Cyc_Absyn_Vardecl*
_tmp281;void*_tmp282;struct Cyc_Absyn_Vardecl*_tmp283;_LL170: if(*((int*)_tmp27B)
!= 1)goto _LL172;_tmp27C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp27B)->f2;if(
_tmp27C <= (void*)1)goto _LL172;if(*((int*)_tmp27C)!= 0)goto _LL172;_tmp27D=((
struct Cyc_Absyn_Global_b_struct*)_tmp27C)->f1;_LL171: _tmp27F=_tmp27D;goto _LL173;
_LL172: if(*((int*)_tmp27B)!= 1)goto _LL174;_tmp27E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp27B)->f2;if(_tmp27E <= (void*)1)goto _LL174;if(*((int*)_tmp27E)!= 3)goto _LL174;
_tmp27F=((struct Cyc_Absyn_Local_b_struct*)_tmp27E)->f1;_LL173: _tmp281=_tmp27F;
goto _LL175;_LL174: if(*((int*)_tmp27B)!= 1)goto _LL176;_tmp280=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp27B)->f2;if(_tmp280 <= (void*)1)goto _LL176;if(*((int*)_tmp280)!= 2)goto _LL176;
_tmp281=((struct Cyc_Absyn_Param_b_struct*)_tmp280)->f1;_LL175: _tmp283=_tmp281;
goto _LL177;_LL176: if(*((int*)_tmp27B)!= 1)goto _LL178;_tmp282=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp27B)->f2;if(_tmp282 <= (void*)1)goto _LL178;if(*((int*)_tmp282)!= 4)goto _LL178;
_tmp283=((struct Cyc_Absyn_Pat_b_struct*)_tmp282)->f1;_LL177: if(_tmp283->escapes)
return r;{struct Cyc_CfFlowInfo_Reln*_tmp50E;union Cyc_CfFlowInfo_RelnOp_union
_tmp50D;struct Cyc_List_List*_tmp50C;return(_tmp50C=_region_malloc(rgn,sizeof(*
_tmp50C)),((_tmp50C->hd=((_tmp50E=_region_malloc(rgn,sizeof(*_tmp50E)),((_tmp50E->vd=
v,((_tmp50E->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((_tmp50D.LessNumelts).tag=2,(((
_tmp50D.LessNumelts).f1=_tmp283,_tmp50D)))),_tmp50E)))))),((_tmp50C->tl=r,
_tmp50C)))));}_LL178:;_LL179: goto _LL16F;_LL16F:;}goto _LL16A;_LL16D:;_LL16E: goto
_LL16A;_LL16A:;}goto _LL15F;_LL166: if(*((int*)_tmp265)!= 3)goto _LL168;_tmp26F=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp265)->f1;_tmp270=((struct Cyc_Absyn_Primop_e_struct*)
_tmp265)->f2;if(_tmp270 == 0)goto _LL168;_tmp271=*_tmp270;_tmp272=(struct Cyc_Absyn_Exp*)
_tmp271.hd;_LL167:{void*_tmp287=(void*)_tmp272->r;void*_tmp288;struct Cyc_Absyn_Vardecl*
_tmp289;void*_tmp28A;struct Cyc_Absyn_Vardecl*_tmp28B;void*_tmp28C;struct Cyc_Absyn_Vardecl*
_tmp28D;void*_tmp28E;struct Cyc_Absyn_Vardecl*_tmp28F;_LL17B: if(*((int*)_tmp287)
!= 1)goto _LL17D;_tmp288=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp287)->f2;if(
_tmp288 <= (void*)1)goto _LL17D;if(*((int*)_tmp288)!= 0)goto _LL17D;_tmp289=((
struct Cyc_Absyn_Global_b_struct*)_tmp288)->f1;_LL17C: _tmp28B=_tmp289;goto _LL17E;
_LL17D: if(*((int*)_tmp287)!= 1)goto _LL17F;_tmp28A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp287)->f2;if(_tmp28A <= (void*)1)goto _LL17F;if(*((int*)_tmp28A)!= 3)goto _LL17F;
_tmp28B=((struct Cyc_Absyn_Local_b_struct*)_tmp28A)->f1;_LL17E: _tmp28D=_tmp28B;
goto _LL180;_LL17F: if(*((int*)_tmp287)!= 1)goto _LL181;_tmp28C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp287)->f2;if(_tmp28C <= (void*)1)goto _LL181;if(*((int*)_tmp28C)!= 2)goto _LL181;
_tmp28D=((struct Cyc_Absyn_Param_b_struct*)_tmp28C)->f1;_LL180: _tmp28F=_tmp28D;
goto _LL182;_LL181: if(*((int*)_tmp287)!= 1)goto _LL183;_tmp28E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp287)->f2;if(_tmp28E <= (void*)1)goto _LL183;if(*((int*)_tmp28E)!= 4)goto _LL183;
_tmp28F=((struct Cyc_Absyn_Pat_b_struct*)_tmp28E)->f1;_LL182: if(_tmp28F->escapes)
return r;{struct Cyc_CfFlowInfo_Reln*_tmp514;union Cyc_CfFlowInfo_RelnOp_union
_tmp513;struct Cyc_List_List*_tmp512;return(_tmp512=_region_malloc(rgn,sizeof(*
_tmp512)),((_tmp512->hd=((_tmp514=_region_malloc(rgn,sizeof(*_tmp514)),((_tmp514->vd=
v,((_tmp514->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((_tmp513.LessEqNumelts).tag=
4,(((_tmp513.LessEqNumelts).f1=_tmp28F,_tmp513)))),_tmp514)))))),((_tmp512->tl=r,
_tmp512)))));}_LL183:;_LL184: goto _LL17A;_LL17A:;}goto _LL15F;_LL168:;_LL169: goto
_LL15F;_LL15F:;}return r;}struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*rgn,
struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*
_tmp293=(void*)e1->r;void*_tmp294;struct Cyc_Absyn_Vardecl*_tmp295;void*_tmp296;
struct Cyc_Absyn_Vardecl*_tmp297;void*_tmp298;struct Cyc_Absyn_Vardecl*_tmp299;
void*_tmp29A;struct Cyc_Absyn_Vardecl*_tmp29B;_LL186: if(*((int*)_tmp293)!= 1)goto
_LL188;_tmp294=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp293)->f2;if(_tmp294 <= (
void*)1)goto _LL188;if(*((int*)_tmp294)!= 0)goto _LL188;_tmp295=((struct Cyc_Absyn_Global_b_struct*)
_tmp294)->f1;_LL187: _tmp297=_tmp295;goto _LL189;_LL188: if(*((int*)_tmp293)!= 1)
goto _LL18A;_tmp296=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp293)->f2;if(
_tmp296 <= (void*)1)goto _LL18A;if(*((int*)_tmp296)!= 2)goto _LL18A;_tmp297=((
struct Cyc_Absyn_Param_b_struct*)_tmp296)->f1;_LL189: _tmp299=_tmp297;goto _LL18B;
_LL18A: if(*((int*)_tmp293)!= 1)goto _LL18C;_tmp298=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp293)->f2;if(_tmp298 <= (void*)1)goto _LL18C;if(*((int*)_tmp298)!= 3)goto _LL18C;
_tmp299=((struct Cyc_Absyn_Local_b_struct*)_tmp298)->f1;_LL18B: _tmp29B=_tmp299;
goto _LL18D;_LL18C: if(*((int*)_tmp293)!= 1)goto _LL18E;_tmp29A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp293)->f2;if(_tmp29A <= (void*)1)goto _LL18E;if(*((int*)_tmp29A)!= 4)goto _LL18E;
_tmp29B=((struct Cyc_Absyn_Pat_b_struct*)_tmp29A)->f1;_LL18D: if(!_tmp29B->escapes)
return Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp29B,e2);goto _LL185;_LL18E:;_LL18F:
goto _LL185;_LL185:;}return r;}static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*
r);static void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*r){{const char*
_tmp518;void*_tmp517[1];struct Cyc_String_pa_struct _tmp516;(_tmp516.tag=0,((
_tmp516.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((
r->vd)->name)),((_tmp517[0]=& _tmp516,Cyc_fprintf(Cyc_stderr,((_tmp518="%s",
_tag_dyneither(_tmp518,sizeof(char),3))),_tag_dyneither(_tmp517,sizeof(void*),1)))))));}{
union Cyc_CfFlowInfo_RelnOp_union _tmp29F=r->rop;unsigned int _tmp2A0;struct Cyc_Absyn_Vardecl*
_tmp2A1;struct Cyc_Absyn_Vardecl*_tmp2A2;unsigned int _tmp2A3;struct Cyc_Absyn_Vardecl*
_tmp2A4;_LL191: if((_tmp29F.EqualConst).tag != 0)goto _LL193;_tmp2A0=(_tmp29F.EqualConst).f1;
_LL192:{const char*_tmp51C;void*_tmp51B[1];struct Cyc_Int_pa_struct _tmp51A;(
_tmp51A.tag=1,((_tmp51A.f1=(unsigned long)((int)_tmp2A0),((_tmp51B[0]=& _tmp51A,
Cyc_fprintf(Cyc_stderr,((_tmp51C="==%d",_tag_dyneither(_tmp51C,sizeof(char),5))),
_tag_dyneither(_tmp51B,sizeof(void*),1)))))));}goto _LL190;_LL193: if((_tmp29F.LessVar).tag
!= 1)goto _LL195;_tmp2A1=(_tmp29F.LessVar).f1;_LL194:{const char*_tmp520;void*
_tmp51F[1];struct Cyc_String_pa_struct _tmp51E;(_tmp51E.tag=0,((_tmp51E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2A1->name)),((
_tmp51F[0]=& _tmp51E,Cyc_fprintf(Cyc_stderr,((_tmp520="<%s",_tag_dyneither(
_tmp520,sizeof(char),4))),_tag_dyneither(_tmp51F,sizeof(void*),1)))))));}goto
_LL190;_LL195: if((_tmp29F.LessNumelts).tag != 2)goto _LL197;_tmp2A2=(_tmp29F.LessNumelts).f1;
_LL196:{const char*_tmp524;void*_tmp523[1];struct Cyc_String_pa_struct _tmp522;(
_tmp522.tag=0,((_tmp522.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp2A2->name)),((_tmp523[0]=& _tmp522,Cyc_fprintf(Cyc_stderr,((_tmp524="<numelts(%s)",
_tag_dyneither(_tmp524,sizeof(char),13))),_tag_dyneither(_tmp523,sizeof(void*),1)))))));}
goto _LL190;_LL197: if((_tmp29F.LessConst).tag != 3)goto _LL199;_tmp2A3=(_tmp29F.LessConst).f1;
_LL198:{const char*_tmp528;void*_tmp527[1];struct Cyc_Int_pa_struct _tmp526;(
_tmp526.tag=1,((_tmp526.f1=(unsigned long)((int)_tmp2A3),((_tmp527[0]=& _tmp526,
Cyc_fprintf(Cyc_stderr,((_tmp528="<%d",_tag_dyneither(_tmp528,sizeof(char),4))),
_tag_dyneither(_tmp527,sizeof(void*),1)))))));}goto _LL190;_LL199: if((_tmp29F.LessEqNumelts).tag
!= 4)goto _LL190;_tmp2A4=(_tmp29F.LessEqNumelts).f1;_LL19A:{const char*_tmp52C;
void*_tmp52B[1];struct Cyc_String_pa_struct _tmp52A;(_tmp52A.tag=0,((_tmp52A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2A4->name)),((
_tmp52B[0]=& _tmp52A,Cyc_fprintf(Cyc_stderr,((_tmp52C="<=numelts(%s)",
_tag_dyneither(_tmp52C,sizeof(char),14))),_tag_dyneither(_tmp52B,sizeof(void*),1)))))));}
goto _LL190;_LL190:;}}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r);void
Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){for(0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((
struct Cyc_CfFlowInfo_Reln*)r->hd);if(r->tl != 0){const char*_tmp52F;void*_tmp52E;(
_tmp52E=0,Cyc_fprintf(Cyc_stderr,((_tmp52F=",",_tag_dyneither(_tmp52F,sizeof(
char),2))),_tag_dyneither(_tmp52E,sizeof(void*),0)));}}}void Cyc_CfFlowInfo_print_initlevel(
void*il);void Cyc_CfFlowInfo_print_initlevel(void*il){void*_tmp2B6=il;_LL19C: if((
int)_tmp2B6 != 0)goto _LL19E;_LL19D:{const char*_tmp532;void*_tmp531;(_tmp531=0,Cyc_fprintf(
Cyc_stderr,((_tmp532="uninitialized",_tag_dyneither(_tmp532,sizeof(char),14))),
_tag_dyneither(_tmp531,sizeof(void*),0)));}goto _LL19B;_LL19E: if((int)_tmp2B6 != 1)
goto _LL1A0;_LL19F:{const char*_tmp535;void*_tmp534;(_tmp534=0,Cyc_fprintf(Cyc_stderr,((
_tmp535="this-initialized",_tag_dyneither(_tmp535,sizeof(char),17))),
_tag_dyneither(_tmp534,sizeof(void*),0)));}goto _LL19B;_LL1A0: if((int)_tmp2B6 != 2)
goto _LL19B;_LL1A1:{const char*_tmp538;void*_tmp537;(_tmp537=0,Cyc_fprintf(Cyc_stderr,((
_tmp538="all-initialized",_tag_dyneither(_tmp538,sizeof(char),16))),
_tag_dyneither(_tmp537,sizeof(void*),0)));}goto _LL19B;_LL19B:;}void Cyc_CfFlowInfo_print_root(
void*root);void Cyc_CfFlowInfo_print_root(void*root){void*_tmp2BD=root;struct Cyc_Absyn_Vardecl*
_tmp2BE;struct Cyc_Absyn_Exp*_tmp2BF;void*_tmp2C0;_LL1A3: if(*((int*)_tmp2BD)!= 0)
goto _LL1A5;_tmp2BE=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp2BD)->f1;_LL1A4:{
const char*_tmp53C;void*_tmp53B[1];struct Cyc_String_pa_struct _tmp53A;(_tmp53A.tag=
0,((_tmp53A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp2BE->name)),((_tmp53B[0]=& _tmp53A,Cyc_fprintf(Cyc_stderr,((_tmp53C="Root(%s)",
_tag_dyneither(_tmp53C,sizeof(char),9))),_tag_dyneither(_tmp53B,sizeof(void*),1)))))));}
goto _LL1A2;_LL1A5: if(*((int*)_tmp2BD)!= 1)goto _LL1A7;_tmp2BF=((struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp2BD)->f1;_tmp2C0=(void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp2BD)->f2;
_LL1A6:{const char*_tmp541;void*_tmp540[2];struct Cyc_String_pa_struct _tmp53F;
struct Cyc_String_pa_struct _tmp53E;(_tmp53E.tag=0,((_tmp53E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C0)),((_tmp53F.tag=
0,((_tmp53F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
_tmp2BF)),((_tmp540[0]=& _tmp53F,((_tmp540[1]=& _tmp53E,Cyc_fprintf(Cyc_stderr,((
_tmp541="MallocPt(%s,%s)",_tag_dyneither(_tmp541,sizeof(char),16))),
_tag_dyneither(_tmp540,sizeof(void*),2)))))))))))));}goto _LL1A2;_LL1A7: if(*((int*)
_tmp2BD)!= 2)goto _LL1A2;_LL1A8:{const char*_tmp544;void*_tmp543;(_tmp543=0,Cyc_fprintf(
Cyc_stderr,((_tmp544="InitParam(_,_)",_tag_dyneither(_tmp544,sizeof(char),15))),
_tag_dyneither(_tmp543,sizeof(void*),0)));}goto _LL1A2;_LL1A2:;}void Cyc_CfFlowInfo_print_place(
struct Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*
p){Cyc_CfFlowInfo_print_root((void*)p->root);{struct Cyc_List_List*_tmp2CA=p->fields;
while(_tmp2CA != 0){{const char*_tmp548;void*_tmp547[1];struct Cyc_Int_pa_struct
_tmp546;(_tmp546.tag=1,((_tmp546.f1=(unsigned long)((int)_tmp2CA->hd),((_tmp547[
0]=& _tmp546,Cyc_fprintf(Cyc_stderr,((_tmp548=".%d",_tag_dyneither(_tmp548,
sizeof(char),4))),_tag_dyneither(_tmp547,sizeof(void*),1)))))));}_tmp2CA=_tmp2CA->tl;}{
const char*_tmp54B;void*_tmp54A;(_tmp54A=0,Cyc_fprintf(Cyc_stderr,((_tmp54B=" ",
_tag_dyneither(_tmp54B,sizeof(char),2))),_tag_dyneither(_tmp54A,sizeof(void*),0)));}}}
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*));void Cyc_CfFlowInfo_print_list(
struct Cyc_List_List*x,void(*pr)(void*)){int _tmp2D0=1;while(x != 0){if(!_tmp2D0){{
const char*_tmp54E;void*_tmp54D;(_tmp54D=0,Cyc_fprintf(Cyc_stderr,((_tmp54E=", ",
_tag_dyneither(_tmp54E,sizeof(char),3))),_tag_dyneither(_tmp54D,sizeof(void*),0)));}
_tmp2D0=0;}pr((void*)x->hd);x=x->tl;}{const char*_tmp551;void*_tmp550;(_tmp550=0,
Cyc_fprintf(Cyc_stderr,((_tmp551="\n",_tag_dyneither(_tmp551,sizeof(char),2))),
_tag_dyneither(_tmp550,sizeof(void*),0)));}}void Cyc_CfFlowInfo_print_dict_set(
struct Cyc_Dict_Dict p,void(*pr)(void*));void Cyc_CfFlowInfo_print_dict_set(struct
Cyc_Dict_Dict p,void(*pr)(void*)){struct _RegionHandle _tmp2D5=_new_region("r");
struct _RegionHandle*r=& _tmp2D5;_push_region(r);{struct _handler_cons _tmp2D6;
_push_handler(& _tmp2D6);{int _tmp2D8=0;if(setjmp(_tmp2D6.handler))_tmp2D8=1;if(!
_tmp2D8){{struct Cyc_Iter_Iter _tmp2D9=((struct Cyc_Iter_Iter(*)(struct
_RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);struct _tuple10
_tmp2DA=*((struct _tuple10*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
r,p);int first=1;{const char*_tmp554;void*_tmp553;(_tmp553=0,Cyc_fprintf(Cyc_stderr,((
_tmp554="{ ",_tag_dyneither(_tmp554,sizeof(char),3))),_tag_dyneither(_tmp553,
sizeof(void*),0)));}while(((int(*)(struct Cyc_Iter_Iter,struct _tuple10*))Cyc_Iter_next)(
_tmp2D9,& _tmp2DA)){void*_tmp2DD=_tmp2DA.f1;if(!first){{const char*_tmp557;void*
_tmp556;(_tmp556=0,Cyc_fprintf(Cyc_stderr,((_tmp557=", ",_tag_dyneither(_tmp557,
sizeof(char),3))),_tag_dyneither(_tmp556,sizeof(void*),0)));}first=0;}pr(_tmp2DD);}{
const char*_tmp55A;void*_tmp559;(_tmp559=0,Cyc_fprintf(Cyc_stderr,((_tmp55A="}\n",
_tag_dyneither(_tmp55A,sizeof(char),3))),_tag_dyneither(_tmp559,sizeof(void*),0)));}};
_pop_handler();}else{void*_tmp2D7=(void*)_exn_thrown;void*_tmp2E3=_tmp2D7;_LL1AA:;
_LL1AB:{const char*_tmp55D;void*_tmp55C;(_tmp55C=0,Cyc_fprintf(Cyc_stderr,((
_tmp55D="{ }\n",_tag_dyneither(_tmp55D,sizeof(char),5))),_tag_dyneither(_tmp55C,
sizeof(void*),0)));}_npop_handler(0);return;_LL1AC:;_LL1AD:(void)_throw(_tmp2E3);
_LL1A9:;}}};_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(void*rval);void Cyc_CfFlowInfo_print_absrval(
void*rval){void*_tmp2E6=rval;void*_tmp2E7;void*_tmp2E8;struct Cyc_CfFlowInfo_Place*
_tmp2E9;struct _dyneither_ptr _tmp2EA;_LL1AF: if((int)_tmp2E6 != 0)goto _LL1B1;_LL1B0:{
const char*_tmp560;void*_tmp55F;(_tmp55F=0,Cyc_fprintf(Cyc_stderr,((_tmp560="Zero",
_tag_dyneither(_tmp560,sizeof(char),5))),_tag_dyneither(_tmp55F,sizeof(void*),0)));}
goto _LL1AE;_LL1B1: if((int)_tmp2E6 != 1)goto _LL1B3;_LL1B2:{const char*_tmp563;void*
_tmp562;(_tmp562=0,Cyc_fprintf(Cyc_stderr,((_tmp563="NotZeroAll",_tag_dyneither(
_tmp563,sizeof(char),11))),_tag_dyneither(_tmp562,sizeof(void*),0)));}goto _LL1AE;
_LL1B3: if((int)_tmp2E6 != 2)goto _LL1B5;_LL1B4:{const char*_tmp566;void*_tmp565;(
_tmp565=0,Cyc_fprintf(Cyc_stderr,((_tmp566="NotZeroThis",_tag_dyneither(_tmp566,
sizeof(char),12))),_tag_dyneither(_tmp565,sizeof(void*),0)));}goto _LL1AE;_LL1B5:
if(_tmp2E6 <= (void*)3)goto _LL1B7;if(*((int*)_tmp2E6)!= 0)goto _LL1B7;_tmp2E7=(
void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp2E6)->f1;_LL1B6:{const char*
_tmp569;void*_tmp568;(_tmp568=0,Cyc_fprintf(Cyc_stderr,((_tmp569="Unknown(",
_tag_dyneither(_tmp569,sizeof(char),9))),_tag_dyneither(_tmp568,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_initlevel(_tmp2E7);{const char*_tmp56C;void*_tmp56B;(_tmp56B=
0,Cyc_fprintf(Cyc_stderr,((_tmp56C=")",_tag_dyneither(_tmp56C,sizeof(char),2))),
_tag_dyneither(_tmp56B,sizeof(void*),0)));}goto _LL1AE;_LL1B7: if(_tmp2E6 <= (void*)
3)goto _LL1B9;if(*((int*)_tmp2E6)!= 1)goto _LL1B9;_tmp2E8=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp2E6)->f1;_LL1B8:{const char*_tmp56F;void*_tmp56E;(_tmp56E=0,Cyc_fprintf(Cyc_stderr,((
_tmp56F="Esc(",_tag_dyneither(_tmp56F,sizeof(char),5))),_tag_dyneither(_tmp56E,
sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(_tmp2E8);{const char*_tmp572;
void*_tmp571;(_tmp571=0,Cyc_fprintf(Cyc_stderr,((_tmp572=")",_tag_dyneither(
_tmp572,sizeof(char),2))),_tag_dyneither(_tmp571,sizeof(void*),0)));}goto _LL1AE;
_LL1B9: if(_tmp2E6 <= (void*)3)goto _LL1BB;if(*((int*)_tmp2E6)!= 2)goto _LL1BB;
_tmp2E9=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp2E6)->f1;_LL1BA:{const char*
_tmp575;void*_tmp574;(_tmp574=0,Cyc_fprintf(Cyc_stderr,((_tmp575="AddrOf(",
_tag_dyneither(_tmp575,sizeof(char),8))),_tag_dyneither(_tmp574,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_place(_tmp2E9);{const char*_tmp578;void*_tmp577;(_tmp577=0,
Cyc_fprintf(Cyc_stderr,((_tmp578=")",_tag_dyneither(_tmp578,sizeof(char),2))),
_tag_dyneither(_tmp577,sizeof(void*),0)));}goto _LL1AE;_LL1BB: if(_tmp2E6 <= (void*)
3)goto _LL1BD;if(*((int*)_tmp2E6)!= 3)goto _LL1BD;_LL1BC:{const char*_tmp57B;void*
_tmp57A;(_tmp57A=0,Cyc_fprintf(Cyc_stderr,((_tmp57B="TagCmps(?)",_tag_dyneither(
_tmp57B,sizeof(char),11))),_tag_dyneither(_tmp57A,sizeof(void*),0)));}goto _LL1AE;
_LL1BD: if(_tmp2E6 <= (void*)3)goto _LL1AE;if(*((int*)_tmp2E6)!= 4)goto _LL1AE;
_tmp2EA=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2E6)->f1;_LL1BE:{const char*
_tmp57E;void*_tmp57D;(_tmp57D=0,Cyc_fprintf(Cyc_stderr,((_tmp57E="Aggregate{",
_tag_dyneither(_tmp57E,sizeof(char),11))),_tag_dyneither(_tmp57D,sizeof(void*),0)));}{
int i=0;for(0;i < _get_dyneither_size(_tmp2EA,sizeof(void*));++ i){Cyc_CfFlowInfo_print_absrval(((
void**)_tmp2EA.curr)[i]);if(i + 1 < _get_dyneither_size(_tmp2EA,sizeof(void*))){
const char*_tmp581;void*_tmp580;(_tmp580=0,Cyc_fprintf(Cyc_stderr,((_tmp581=",",
_tag_dyneither(_tmp581,sizeof(char),2))),_tag_dyneither(_tmp580,sizeof(void*),0)));}}}{
const char*_tmp584;void*_tmp583;(_tmp583=0,Cyc_fprintf(Cyc_stderr,((_tmp584="}",
_tag_dyneither(_tmp584,sizeof(char),2))),_tag_dyneither(_tmp583,sizeof(void*),0)));}
goto _LL1AE;_LL1AE:;}static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*
rval);static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){{const
char*_tmp587;void*_tmp586;(_tmp586=0,Cyc_fprintf(Cyc_stderr,((_tmp587="    ",
_tag_dyneither(_tmp587,sizeof(char),5))),_tag_dyneither(_tmp586,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_root(root);{const char*_tmp58A;void*_tmp589;(_tmp589=0,Cyc_fprintf(
Cyc_stderr,((_tmp58A=" --> ",_tag_dyneither(_tmp58A,sizeof(char),6))),
_tag_dyneither(_tmp589,sizeof(void*),0)));}Cyc_CfFlowInfo_print_absrval(rval);{
const char*_tmp58D;void*_tmp58C;(_tmp58C=0,Cyc_fprintf(Cyc_stderr,((_tmp58D="\n",
_tag_dyneither(_tmp58D,sizeof(char),2))),_tag_dyneither(_tmp58C,sizeof(void*),0)));}}
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);void Cyc_CfFlowInfo_print_flowdict(
struct Cyc_Dict_Dict d){Cyc_Dict_iter(Cyc_CfFlowInfo_print_flow_mapping,d);}void
Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo_union f);void Cyc_CfFlowInfo_print_flow(
union Cyc_CfFlowInfo_FlowInfo_union f){union Cyc_CfFlowInfo_FlowInfo_union _tmp30B=f;
struct Cyc_Dict_Dict _tmp30C;struct Cyc_CfFlowInfo_ConsumeInfo _tmp30D;_LL1C0: if((
_tmp30B.BottomFL).tag != 0)goto _LL1C2;_LL1C1:{const char*_tmp590;void*_tmp58F;(
_tmp58F=0,Cyc_fprintf(Cyc_stderr,((_tmp590="  BottomFL\n",_tag_dyneither(_tmp590,
sizeof(char),12))),_tag_dyneither(_tmp58F,sizeof(void*),0)));}goto _LL1BF;_LL1C2:
if((_tmp30B.ReachableFL).tag != 1)goto _LL1BF;_tmp30C=(_tmp30B.ReachableFL).f1;
_tmp30D=(_tmp30B.ReachableFL).f3;_LL1C3:{const char*_tmp593;void*_tmp592;(_tmp592=
0,Cyc_fprintf(Cyc_stderr,((_tmp593="  ReachableFL:\n",_tag_dyneither(_tmp593,
sizeof(char),16))),_tag_dyneither(_tmp592,sizeof(void*),0)));}Cyc_Dict_iter(Cyc_CfFlowInfo_print_flow_mapping,
_tmp30C);{const char*_tmp596;void*_tmp595;(_tmp595=0,Cyc_fprintf(Cyc_stderr,((
_tmp596="\n    consumed: ",_tag_dyneither(_tmp596,sizeof(char),16))),
_tag_dyneither(_tmp595,sizeof(void*),0)));}((void(*)(struct Cyc_Dict_Dict p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(_tmp30D.consumed,
Cyc_CfFlowInfo_print_place);{const char*_tmp599;void*_tmp598;(_tmp598=0,Cyc_fprintf(
Cyc_stderr,((_tmp599="\n    may_consume: ",_tag_dyneither(_tmp599,sizeof(char),
19))),_tag_dyneither(_tmp598,sizeof(void*),0)));}((void(*)(struct Cyc_List_List*x,
void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(_tmp30D.may_consume,
Cyc_CfFlowInfo_print_place);{const char*_tmp59C;void*_tmp59B;(_tmp59B=0,Cyc_fprintf(
Cyc_stderr,((_tmp59C="\n",_tag_dyneither(_tmp59C,sizeof(char),2))),
_tag_dyneither(_tmp59B,sizeof(void*),0)));}goto _LL1BF;_LL1BF:;}int Cyc_CfFlowInfo_contains_region(
void*rgn,void*t);int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){void*_tmp318=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp319;struct Cyc_Absyn_TunionInfo
_tmp31A;struct Cyc_List_List*_tmp31B;struct Cyc_Core_Opt*_tmp31C;struct Cyc_List_List*
_tmp31D;struct Cyc_Absyn_AggrInfo _tmp31E;struct Cyc_List_List*_tmp31F;struct Cyc_Absyn_TunionFieldInfo
_tmp320;struct Cyc_List_List*_tmp321;struct Cyc_List_List*_tmp322;struct Cyc_Absyn_PtrInfo
_tmp323;void*_tmp324;struct Cyc_Absyn_PtrAtts _tmp325;void*_tmp326;struct Cyc_List_List*
_tmp327;struct Cyc_List_List*_tmp328;struct Cyc_Absyn_ArrayInfo _tmp329;void*
_tmp32A;void*_tmp32B;void*_tmp32C;void*_tmp32D;void*_tmp32E;void*_tmp32F;_LL1C5:
if((int)_tmp318 != 0)goto _LL1C7;_LL1C6: goto _LL1C8;_LL1C7: if(_tmp318 <= (void*)4)
goto _LL1C9;if(*((int*)_tmp318)!= 5)goto _LL1C9;_LL1C8: goto _LL1CA;_LL1C9: if((int)
_tmp318 != 1)goto _LL1CB;_LL1CA: goto _LL1CC;_LL1CB: if(_tmp318 <= (void*)4)goto _LL1D3;
if(*((int*)_tmp318)!= 6)goto _LL1CD;_LL1CC: goto _LL1CE;_LL1CD: if(*((int*)_tmp318)
!= 12)goto _LL1CF;_LL1CE: goto _LL1D0;_LL1CF: if(*((int*)_tmp318)!= 13)goto _LL1D1;
_LL1D0: goto _LL1D2;_LL1D1: if(*((int*)_tmp318)!= 18)goto _LL1D3;_LL1D2: goto _LL1D4;
_LL1D3: if((int)_tmp318 != 2)goto _LL1D5;_LL1D4: goto _LL1D6;_LL1D5: if(_tmp318 <= (
void*)4)goto _LL1D7;if(*((int*)_tmp318)!= 0)goto _LL1D7;_LL1D6: return 0;_LL1D7: if((
int)_tmp318 != 3)goto _LL1D9;_LL1D8: {void*_tmp330=rgn;_LL1FA: if((int)_tmp330 != 0)
goto _LL1FC;_LL1FB: return 1;_LL1FC:;_LL1FD: return 0;_LL1F9:;}_LL1D9: if(_tmp318 <= (
void*)4)goto _LL1DB;if(*((int*)_tmp318)!= 1)goto _LL1DB;_tmp319=((struct Cyc_Absyn_VarType_struct*)
_tmp318)->f1;_LL1DA: {void*_tmp331=rgn;struct Cyc_Absyn_Tvar*_tmp332;_LL1FF: if((
int)_tmp331 != 0)goto _LL201;_LL200: return 0;_LL201: if(_tmp331 <= (void*)1)goto
_LL1FE;if(*((int*)_tmp331)!= 0)goto _LL1FE;_tmp332=((struct Cyc_CfFlowInfo_Region_k_struct*)
_tmp331)->f1;_LL202: return Cyc_Absyn_tvar_cmp(_tmp319,_tmp332)== 0;_LL1FE:;}
_LL1DB: if(_tmp318 <= (void*)4)goto _LL1DD;if(*((int*)_tmp318)!= 2)goto _LL1DD;
_tmp31A=((struct Cyc_Absyn_TunionType_struct*)_tmp318)->f1;_tmp31B=_tmp31A.targs;
_tmp31C=_tmp31A.rgn;_LL1DC: if((unsigned int)_tmp31C  && Cyc_CfFlowInfo_contains_region(
rgn,(void*)_tmp31C->v))return 1;_tmp31D=_tmp31B;goto _LL1DE;_LL1DD: if(_tmp318 <= (
void*)4)goto _LL1DF;if(*((int*)_tmp318)!= 16)goto _LL1DF;_tmp31D=((struct Cyc_Absyn_TypedefType_struct*)
_tmp318)->f2;_LL1DE: _tmp31F=_tmp31D;goto _LL1E0;_LL1DF: if(_tmp318 <= (void*)4)goto
_LL1E1;if(*((int*)_tmp318)!= 10)goto _LL1E1;_tmp31E=((struct Cyc_Absyn_AggrType_struct*)
_tmp318)->f1;_tmp31F=_tmp31E.targs;_LL1E0: _tmp321=_tmp31F;goto _LL1E2;_LL1E1: if(
_tmp318 <= (void*)4)goto _LL1E3;if(*((int*)_tmp318)!= 3)goto _LL1E3;_tmp320=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp318)->f1;_tmp321=_tmp320.targs;
_LL1E2: _tmp322=_tmp321;goto _LL1E4;_LL1E3: if(_tmp318 <= (void*)4)goto _LL1E5;if(*((
int*)_tmp318)!= 20)goto _LL1E5;_tmp322=((struct Cyc_Absyn_JoinEff_struct*)_tmp318)->f1;
_LL1E4: return Cyc_List_exists_c(Cyc_CfFlowInfo_contains_region,rgn,_tmp322);
_LL1E5: if(_tmp318 <= (void*)4)goto _LL1E7;if(*((int*)_tmp318)!= 4)goto _LL1E7;
_tmp323=((struct Cyc_Absyn_PointerType_struct*)_tmp318)->f1;_tmp324=(void*)
_tmp323.elt_typ;_tmp325=_tmp323.ptr_atts;_tmp326=(void*)_tmp325.rgn;_LL1E6:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp326) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp324);_LL1E7: if(_tmp318 <= (void*)4)goto _LL1E9;if(*((int*)_tmp318)!= 8)
goto _LL1E9;_LL1E8: return 0;_LL1E9: if(_tmp318 <= (void*)4)goto _LL1EB;if(*((int*)
_tmp318)!= 9)goto _LL1EB;_tmp327=((struct Cyc_Absyn_TupleType_struct*)_tmp318)->f1;
_LL1EA: for(0;_tmp327 != 0;_tmp327=_tmp327->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(*((struct _tuple11*)_tmp327->hd)).f2))return 1;}return 0;_LL1EB: if(_tmp318 <= (
void*)4)goto _LL1ED;if(*((int*)_tmp318)!= 11)goto _LL1ED;_tmp328=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp318)->f2;_LL1EC: for(0;_tmp328 != 0;_tmp328=_tmp328->tl){if(Cyc_CfFlowInfo_contains_region(
rgn,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp328->hd)->type))return 1;}return 0;
_LL1ED: if(_tmp318 <= (void*)4)goto _LL1EF;if(*((int*)_tmp318)!= 7)goto _LL1EF;
_tmp329=((struct Cyc_Absyn_ArrayType_struct*)_tmp318)->f1;_tmp32A=(void*)_tmp329.elt_type;
_LL1EE: _tmp32B=_tmp32A;goto _LL1F0;_LL1EF: if(_tmp318 <= (void*)4)goto _LL1F1;if(*((
int*)_tmp318)!= 19)goto _LL1F1;_tmp32B=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp318)->f1;_LL1F0: _tmp32C=_tmp32B;goto _LL1F2;_LL1F1: if(_tmp318 <= (void*)4)goto
_LL1F3;if(*((int*)_tmp318)!= 21)goto _LL1F3;_tmp32C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp318)->f1;_LL1F2: return Cyc_CfFlowInfo_contains_region(rgn,_tmp32C);_LL1F3: if(
_tmp318 <= (void*)4)goto _LL1F5;if(*((int*)_tmp318)!= 15)goto _LL1F5;_tmp32D=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp318)->f1;_tmp32E=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp318)->f2;_LL1F4: return Cyc_CfFlowInfo_contains_region(rgn,_tmp32D) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp32E);_LL1F5: if(_tmp318 <= (void*)4)goto _LL1F7;if(*((int*)_tmp318)!= 14)
goto _LL1F7;_tmp32F=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp318)->f1;
_LL1F6: return 0;_LL1F7: if(_tmp318 <= (void*)4)goto _LL1C4;if(*((int*)_tmp318)!= 17)
goto _LL1C4;_LL1F8: return 0;_LL1C4:;}struct _tuple18{struct Cyc_CfFlowInfo_FlowEnv*
f1;struct Cyc_Dict_Dict f2;void*f3;};static void Cyc_CfFlowInfo_kill_root(struct
_tuple18*env,void*root,void*rval);static void Cyc_CfFlowInfo_kill_root(struct
_tuple18*env,void*root,void*rval){struct _tuple18 _tmp334;struct Cyc_CfFlowInfo_FlowEnv*
_tmp335;struct Cyc_Dict_Dict _tmp336;struct Cyc_Dict_Dict*_tmp337;void*_tmp338;
struct _tuple18*_tmp333=env;_tmp334=*_tmp333;_tmp335=_tmp334.f1;_tmp336=_tmp334.f2;
_tmp337=(struct Cyc_Dict_Dict*)&(*_tmp333).f2;_tmp338=_tmp334.f3;{void*_tmp339=
root;struct Cyc_Absyn_Vardecl*_tmp33A;void*_tmp33B;_LL204: if(*((int*)_tmp339)!= 0)
goto _LL206;_tmp33A=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp339)->f1;_LL205:
if(Cyc_CfFlowInfo_contains_region(_tmp338,(void*)_tmp33A->type))rval=Cyc_CfFlowInfo_typ_to_absrval(
_tmp335,(void*)_tmp33A->type,(void*)_tmp335->unknown_none);*_tmp337=Cyc_Dict_insert(*
_tmp337,root,rval);goto _LL203;_LL206: if(*((int*)_tmp339)!= 1)goto _LL208;_tmp33B=(
void*)((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp339)->f2;_LL207: if(!Cyc_CfFlowInfo_contains_region(
_tmp338,_tmp33B))*_tmp337=Cyc_Dict_insert(*_tmp337,root,rval);goto _LL203;_LL208:
if(*((int*)_tmp339)!= 2)goto _LL203;_LL209: goto _LL203;_LL203:;}}static struct Cyc_Dict_Dict
Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict
fd,void*rgn);static struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct
Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict fd,void*rgn){void*_tmp33C=Cyc_Tcutil_compress(
rgn);struct Cyc_Absyn_Tvar*_tmp33D;_LL20B: if(_tmp33C <= (void*)4)goto _LL20D;if(*((
int*)_tmp33C)!= 1)goto _LL20D;_tmp33D=((struct Cyc_Absyn_VarType_struct*)_tmp33C)->f1;
_LL20C: {struct Cyc_CfFlowInfo_Region_k_struct _tmp59D;struct Cyc_CfFlowInfo_Region_k_struct
v=(_tmp59D.tag=0,((_tmp59D.f1=_tmp33D,_tmp59D)));void*v2=(void*)& v;struct
_tuple18 _tmp59E;struct _tuple18 env=(_tmp59E.f1=fenv,((_tmp59E.f2=fenv->mt_flowdict,((
_tmp59E.f3=v2,_tmp59E)))));((void(*)(void(*f)(struct _tuple18*,void*,void*),
struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,&
env,fd);return env.f2;}_LL20D: if((int)_tmp33C != 3)goto _LL20F;_LL20E: {struct
_tuple18 _tmp59F;struct _tuple18 env=(_tmp59F.f1=fenv,((_tmp59F.f2=fenv->mt_flowdict,((
_tmp59F.f3=(void*)0,_tmp59F)))));((void(*)(void(*f)(struct _tuple18*,void*,void*),
struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,&
env,fd);return env.f2;}_LL20F:;_LL210: {struct Cyc_Core_Impossible_struct _tmp5A5;
const char*_tmp5A4;struct Cyc_Core_Impossible_struct*_tmp5A3;(int)_throw((void*)((
_tmp5A3=_cycalloc(sizeof(*_tmp5A3)),((_tmp5A3[0]=((_tmp5A5.tag=Cyc_Core_Impossible,((
_tmp5A5.f1=((_tmp5A4="kill_flowdict_region",_tag_dyneither(_tmp5A4,sizeof(char),
21))),_tmp5A5)))),_tmp5A3)))));}_LL20A:;}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn);static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn){return 0;}union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union
f,void*rgn);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union f,void*rgn){
union Cyc_CfFlowInfo_FlowInfo_union _tmp344=f;struct Cyc_Dict_Dict _tmp345;struct Cyc_List_List*
_tmp346;struct Cyc_CfFlowInfo_ConsumeInfo _tmp347;_LL212: if((_tmp344.BottomFL).tag
!= 0)goto _LL214;_LL213: return f;_LL214: if((_tmp344.ReachableFL).tag != 1)goto
_LL211;_tmp345=(_tmp344.ReachableFL).f1;_tmp346=(_tmp344.ReachableFL).f2;_tmp347=(
_tmp344.ReachableFL).f3;_LL215: {struct Cyc_Dict_Dict _tmp348=Cyc_CfFlowInfo_kill_flowdict_region(
fenv,_tmp345,rgn);struct Cyc_List_List*_tmp349=Cyc_CfFlowInfo_kill_relns_region(
_tmp346,rgn);union Cyc_CfFlowInfo_FlowInfo_union _tmp5A6;return(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp5A6.ReachableFL).tag=1,(((_tmp5A6.ReachableFL).f1=_tmp348,(((_tmp5A6.ReachableFL).f2=
_tmp349,(((_tmp5A6.ReachableFL).f3=_tmp347,_tmp5A6))))))));}_LL211:;}static
struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of_rec(struct
_RegionHandle*r,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds,struct Cyc_List_List*
path);static struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of_rec(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds,struct Cyc_List_List*
path){void*_tmp34B=(void*)e->r;struct Cyc_Absyn_Exp*_tmp34C;struct Cyc_Absyn_Exp*
_tmp34D;struct Cyc_Absyn_Exp*_tmp34E;void*_tmp34F;struct Cyc_Absyn_Vardecl*_tmp350;
void*_tmp351;struct Cyc_Absyn_Vardecl*_tmp352;void*_tmp353;struct Cyc_Absyn_Vardecl*
_tmp354;void*_tmp355;struct Cyc_Absyn_Vardecl*_tmp356;struct Cyc_Absyn_Exp*_tmp357;
struct _dyneither_ptr*_tmp358;struct Cyc_Absyn_Exp*_tmp359;struct Cyc_Absyn_Exp*
_tmp35A;struct Cyc_Absyn_Exp*_tmp35B;struct Cyc_Absyn_Exp*_tmp35C;struct
_dyneither_ptr*_tmp35D;_LL217: if(*((int*)_tmp34B)!= 15)goto _LL219;_tmp34C=((
struct Cyc_Absyn_Cast_e_struct*)_tmp34B)->f2;_LL218: _tmp34D=_tmp34C;goto _LL21A;
_LL219: if(*((int*)_tmp34B)!= 13)goto _LL21B;_tmp34D=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp34B)->f1;_LL21A: _tmp34E=_tmp34D;goto _LL21C;_LL21B: if(*((int*)_tmp34B)!= 14)
goto _LL21D;_tmp34E=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp34B)->f1;_LL21C:
return Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp34E,flds,path);_LL21D: if(*((int*)
_tmp34B)!= 1)goto _LL21F;_tmp34F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp34B)->f2;
if(_tmp34F <= (void*)1)goto _LL21F;if(*((int*)_tmp34F)!= 2)goto _LL21F;_tmp350=((
struct Cyc_Absyn_Param_b_struct*)_tmp34F)->f1;_LL21E: _tmp352=_tmp350;goto _LL220;
_LL21F: if(*((int*)_tmp34B)!= 1)goto _LL221;_tmp351=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp34B)->f2;if(_tmp351 <= (void*)1)goto _LL221;if(*((int*)_tmp351)!= 3)goto _LL221;
_tmp352=((struct Cyc_Absyn_Local_b_struct*)_tmp351)->f1;_LL220: _tmp354=_tmp352;
goto _LL222;_LL221: if(*((int*)_tmp34B)!= 1)goto _LL223;_tmp353=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp34B)->f2;if(_tmp353 <= (void*)1)goto _LL223;if(*((int*)_tmp353)!= 4)goto _LL223;
_tmp354=((struct Cyc_Absyn_Pat_b_struct*)_tmp353)->f1;_LL222: {struct Cyc_CfFlowInfo_Place
_tmp5B0;struct Cyc_CfFlowInfo_VarRoot_struct _tmp5AF;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp5AE;struct Cyc_CfFlowInfo_UniquePlace*_tmp5AD;return(_tmp5AD=_region_malloc(r,
sizeof(*_tmp5AD)),((_tmp5AD->place=((_tmp5B0.root=(void*)((void*)((_tmp5AE=
_region_malloc(r,sizeof(*_tmp5AE)),((_tmp5AE[0]=((_tmp5AF.tag=0,((_tmp5AF.f1=
_tmp354,_tmp5AF)))),_tmp5AE))))),((_tmp5B0.fields=flds,_tmp5B0)))),((_tmp5AD->path=
path,_tmp5AD)))));}_LL223: if(*((int*)_tmp34B)!= 1)goto _LL225;_tmp355=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp34B)->f2;if(_tmp355 <= (void*)1)goto _LL225;if(*((
int*)_tmp355)!= 0)goto _LL225;_tmp356=((struct Cyc_Absyn_Global_b_struct*)_tmp355)->f1;
_LL224: {struct Cyc_Core_Impossible_struct _tmp5B6;const char*_tmp5B5;struct Cyc_Core_Impossible_struct*
_tmp5B4;(int)_throw((void*)((_tmp5B4=_cycalloc(sizeof(*_tmp5B4)),((_tmp5B4[0]=((
_tmp5B6.tag=Cyc_Core_Impossible,((_tmp5B6.f1=((_tmp5B5="non-unique path",
_tag_dyneither(_tmp5B5,sizeof(char),16))),_tmp5B6)))),_tmp5B4)))));}_LL225: if(*((
int*)_tmp34B)!= 24)goto _LL227;_tmp357=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp34B)->f1;_tmp358=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp34B)->f2;_LL226:{
void*_tmp365=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp357->topt))->v);struct Cyc_Absyn_PtrInfo _tmp366;void*_tmp367;_LL230: if(
_tmp365 <= (void*)4)goto _LL232;if(*((int*)_tmp365)!= 4)goto _LL232;_tmp366=((
struct Cyc_Absyn_PointerType_struct*)_tmp365)->f1;_tmp367=(void*)_tmp366.elt_typ;
_LL231: if(!Cyc_Absyn_is_union_type(_tmp367)){struct Cyc_List_List*_tmp5B7;flds=((
_tmp5B7=_region_malloc(r,sizeof(*_tmp5B7)),((_tmp5B7->hd=(void*)Cyc_CfFlowInfo_get_field_index(
_tmp367,_tmp358),((_tmp5B7->tl=flds,_tmp5B7))))));}goto _LL22F;_LL232:;_LL233: {
struct Cyc_Core_Impossible_struct _tmp5BD;const char*_tmp5BC;struct Cyc_Core_Impossible_struct*
_tmp5BB;(int)_throw((void*)((_tmp5BB=_cycalloc(sizeof(*_tmp5BB)),((_tmp5BB[0]=((
_tmp5BD.tag=Cyc_Core_Impossible,((_tmp5BD.f1=((_tmp5BC="anal_Rexp: AggrArrow ptr",
_tag_dyneither(_tmp5BC,sizeof(char),25))),_tmp5BD)))),_tmp5BB)))));}_LL22F:;}
_tmp359=_tmp357;goto _LL228;_LL227: if(*((int*)_tmp34B)!= 22)goto _LL229;_tmp359=((
struct Cyc_Absyn_Deref_e_struct*)_tmp34B)->f1;_LL228: {struct Cyc_List_List*
_tmp5BE;return Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp359,0,((_tmp5BE=
_region_malloc(r,sizeof(*_tmp5BE)),((_tmp5BE->hd=flds,((_tmp5BE->tl=path,_tmp5BE)))))));}
_LL229: if(*((int*)_tmp34B)!= 25)goto _LL22B;_tmp35A=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp34B)->f1;_tmp35B=((struct Cyc_Absyn_Subscript_e_struct*)_tmp34B)->f2;_LL22A: {
void*_tmp36D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp35A->topt))->v);struct Cyc_Absyn_PtrInfo _tmp36E;void*_tmp36F;struct Cyc_Absyn_PtrAtts
_tmp370;struct Cyc_Absyn_Conref*_tmp371;_LL235: if(_tmp36D <= (void*)4)goto _LL239;
if(*((int*)_tmp36D)!= 9)goto _LL237;_LL236: {unsigned int _tmp372=(Cyc_Evexp_eval_const_uint_exp(
_tmp35B)).f1;struct Cyc_List_List*_tmp5BF;return Cyc_CfFlowInfo_unique_place_of_rec(
r,_tmp35A,((_tmp5BF=_region_malloc(r,sizeof(*_tmp5BF)),((_tmp5BF->hd=(void*)((
int)_tmp372),((_tmp5BF->tl=flds,_tmp5BF)))))),path);}_LL237: if(*((int*)_tmp36D)
!= 4)goto _LL239;_tmp36E=((struct Cyc_Absyn_PointerType_struct*)_tmp36D)->f1;
_tmp36F=(void*)_tmp36E.elt_typ;_tmp370=_tmp36E.ptr_atts;_tmp371=_tmp370.bounds;
_LL238: {struct Cyc_Core_Impossible_struct _tmp5C5;const char*_tmp5C4;struct Cyc_Core_Impossible_struct*
_tmp5C3;(int)_throw((void*)((_tmp5C3=_cycalloc(sizeof(*_tmp5C3)),((_tmp5C3[0]=((
_tmp5C5.tag=Cyc_Core_Impossible,((_tmp5C5.f1=((_tmp5C4="subscripts unsupported for unique places",
_tag_dyneither(_tmp5C4,sizeof(char),41))),_tmp5C5)))),_tmp5C3)))));}_LL239:;
_LL23A: {struct Cyc_Core_Impossible_struct _tmp5CB;const char*_tmp5CA;struct Cyc_Core_Impossible_struct*
_tmp5C9;(int)_throw((void*)((_tmp5C9=_cycalloc(sizeof(*_tmp5C9)),((_tmp5C9[0]=((
_tmp5CB.tag=Cyc_Core_Impossible,((_tmp5CB.f1=((_tmp5CA="unique place subscript -- bad type",
_tag_dyneither(_tmp5CA,sizeof(char),35))),_tmp5CB)))),_tmp5C9)))));}_LL234:;}
_LL22B: if(*((int*)_tmp34B)!= 23)goto _LL22D;_tmp35C=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp34B)->f1;_tmp35D=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp34B)->f2;_LL22C:
if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp35C->topt))->v)){
struct Cyc_Core_Impossible_struct _tmp5D1;const char*_tmp5D0;struct Cyc_Core_Impossible_struct*
_tmp5CF;(int)_throw((void*)((_tmp5CF=_cycalloc(sizeof(*_tmp5CF)),((_tmp5CF[0]=((
_tmp5D1.tag=Cyc_Core_Impossible,((_tmp5D1.f1=((_tmp5D0="unions can't contain unique places",
_tag_dyneither(_tmp5D0,sizeof(char),35))),_tmp5D1)))),_tmp5CF)))));}{int _tmp37D=
Cyc_CfFlowInfo_get_field_index((void*)((struct Cyc_Core_Opt*)_check_null(_tmp35C->topt))->v,
_tmp35D);struct Cyc_List_List*_tmp5D2;return Cyc_CfFlowInfo_unique_place_of_rec(r,
_tmp35C,((_tmp5D2=_region_malloc(r,sizeof(*_tmp5D2)),((_tmp5D2->hd=(void*)
_tmp37D,((_tmp5D2->tl=flds,_tmp5D2)))))),path);}_LL22D:;_LL22E: {struct Cyc_Core_Impossible_struct
_tmp5D8;const char*_tmp5D7;struct Cyc_Core_Impossible_struct*_tmp5D6;(int)_throw((
void*)((_tmp5D6=_cycalloc(sizeof(*_tmp5D6)),((_tmp5D6[0]=((_tmp5D8.tag=Cyc_Core_Impossible,((
_tmp5D8.f1=((_tmp5D7="bad unique path",_tag_dyneither(_tmp5D7,sizeof(char),16))),
_tmp5D8)))),_tmp5D6)))));}_LL216:;}struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_UniquePlace*
Cyc_CfFlowInfo_unique_place_of(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
return Cyc_CfFlowInfo_unique_place_of_rec(r,e,0,0);}struct _tuple19{struct Cyc_Dict_Dict*
f1;struct Cyc_Dict_Dict f2;struct Cyc_Position_Segment*f3;};static void Cyc_CfFlowInfo_consume_f(
struct _tuple19*env,struct Cyc_CfFlowInfo_Place*place);static void Cyc_CfFlowInfo_consume_f(
struct _tuple19*env,struct Cyc_CfFlowInfo_Place*place){struct _tuple19 _tmp383;
struct Cyc_Dict_Dict*_tmp384;struct Cyc_Dict_Dict _tmp385;struct Cyc_Position_Segment*
_tmp386;struct _tuple19*_tmp382=env;_tmp383=*_tmp382;_tmp384=_tmp383.f1;_tmp385=
_tmp383.f2;_tmp386=_tmp383.f3;{void*rval=(void*)0;if(Cyc_Dict_lookup_bool(
_tmp385,(void*)place->root,& rval) && rval == (void*)0)return;if(((int(*)(struct
Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_update_place_set)((struct Cyc_Dict_Dict*)_tmp384,place,
_tmp386)){struct Cyc_Position_Segment*_tmp387=((struct Cyc_Position_Segment*(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_lookup)(*_tmp384,
place);const char*_tmp5DA;const char*_tmp5D9;((void(*)(struct Cyc_CfFlowInfo_Place*
place,struct _dyneither_ptr(*toString)(struct Cyc_CfFlowInfo_Place*),struct
_dyneither_ptr err_msg1,struct _dyneither_ptr err_msg2,struct Cyc_Position_Segment*
consumed_loc,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_unique_err)(place,
Cyc_CfFlowInfo_place_err_string,((_tmp5D9="May consume non-aliased pointer %s more than once (cf. %s)",
_tag_dyneither(_tmp5D9,sizeof(char),59))),((_tmp5DA="May consume non-aliased pointer %s more than once",
_tag_dyneither(_tmp5DA,sizeof(char),50))),_tmp387,_tmp386);}}}union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_consume_unique_rvals(struct
Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f){union Cyc_CfFlowInfo_FlowInfo_union
_tmp38A=f;struct Cyc_Dict_Dict _tmp38B;struct Cyc_List_List*_tmp38C;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp38D;_LL23C: if((_tmp38A.BottomFL).tag != 0)goto _LL23E;_LL23D: return f;_LL23E:
if((_tmp38A.ReachableFL).tag != 1)goto _LL23B;_tmp38B=(_tmp38A.ReachableFL).f1;
_tmp38C=(_tmp38A.ReachableFL).f2;_tmp38D=(_tmp38A.ReachableFL).f3;_LL23F: if(
_tmp38D.may_consume == 0)return f;{struct Cyc_Dict_Dict _tmp38E=_tmp38D.consumed;
struct _tuple19 _tmp5DB;struct _tuple19 _tmp38F=(_tmp5DB.f1=& _tmp38E,((_tmp5DB.f2=
_tmp38B,((_tmp5DB.f3=loc,_tmp5DB)))));((void(*)(void(*f)(struct _tuple19*,struct
Cyc_CfFlowInfo_Place*),struct _tuple19*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_CfFlowInfo_consume_f,& _tmp38F,_tmp38D.may_consume);{struct Cyc_CfFlowInfo_ConsumeInfo
_tmp5DE;union Cyc_CfFlowInfo_FlowInfo_union _tmp5DD;return(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp5DD.ReachableFL).tag=1,(((_tmp5DD.ReachableFL).f1=_tmp38B,(((_tmp5DD.ReachableFL).f2=
_tmp38C,(((_tmp5DD.ReachableFL).f3=((_tmp5DE.consumed=_tmp38E,((_tmp5DE.may_consume=
0,_tmp5DE)))),_tmp5DD))))))));}}_LL23B:;}void Cyc_CfFlowInfo_check_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);void Cyc_CfFlowInfo_check_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f){union Cyc_CfFlowInfo_FlowInfo_union
_tmp393=f;struct Cyc_Dict_Dict _tmp394;struct Cyc_List_List*_tmp395;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp396;_LL241: if((_tmp393.BottomFL).tag != 0)goto _LL243;_LL242: return;_LL243: if((
_tmp393.ReachableFL).tag != 1)goto _LL240;_tmp394=(_tmp393.ReachableFL).f1;_tmp395=(
_tmp393.ReachableFL).f2;_tmp396=(_tmp393.ReachableFL).f3;_LL244: {struct Cyc_List_List*
_tmp397=_tmp396.may_consume;while(_tmp397 != 0){if(((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp396.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp397->hd)){struct Cyc_Position_Segment*_tmp398=((struct Cyc_Position_Segment*(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_lookup)(_tmp396.consumed,(
struct Cyc_CfFlowInfo_Place*)_tmp397->hd);{const char*_tmp5E0;const char*_tmp5DF;((
void(*)(struct Cyc_CfFlowInfo_Place*place,struct _dyneither_ptr(*toString)(struct
Cyc_CfFlowInfo_Place*),struct _dyneither_ptr err_msg1,struct _dyneither_ptr err_msg2,
struct Cyc_Position_Segment*consumed_loc,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_unique_err)((
struct Cyc_CfFlowInfo_Place*)_tmp397->hd,Cyc_CfFlowInfo_place_err_string,((
_tmp5DF="Read through possibly consumed unique pointer %s (cf. %s)",
_tag_dyneither(_tmp5DF,sizeof(char),58))),((_tmp5E0="Read through possibly consumed unique pointer %s",
_tag_dyneither(_tmp5E0,sizeof(char),49))),_tmp398,loc);}break;}_tmp397=_tmp397->tl;}
return;}_LL240:;}union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_drop_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f){union Cyc_CfFlowInfo_FlowInfo_union _tmp39B=f;struct Cyc_Dict_Dict _tmp39C;struct
Cyc_List_List*_tmp39D;struct Cyc_CfFlowInfo_ConsumeInfo _tmp39E;_LL246: if((_tmp39B.BottomFL).tag
!= 0)goto _LL248;_LL247: return f;_LL248: if((_tmp39B.ReachableFL).tag != 1)goto
_LL245;_tmp39C=(_tmp39B.ReachableFL).f1;_tmp39D=(_tmp39B.ReachableFL).f2;_tmp39E=(
_tmp39B.ReachableFL).f3;_LL249: _tmp39E.may_consume=0;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5E1;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp5E1.ReachableFL).tag=1,(((
_tmp5E1.ReachableFL).f1=_tmp39C,(((_tmp5E1.ReachableFL).f2=_tmp39D,(((_tmp5E1.ReachableFL).f3=
_tmp39E,_tmp5E1))))))));}_LL245:;}union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_readthrough_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f){Cyc_CfFlowInfo_check_unique_rvals(loc,f);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A0=f;struct Cyc_Dict_Dict _tmp3A1;struct Cyc_List_List*_tmp3A2;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3A3;_LL24B: if((_tmp3A0.BottomFL).tag != 0)goto _LL24D;_LL24C: return f;_LL24D:
if((_tmp3A0.ReachableFL).tag != 1)goto _LL24A;_tmp3A1=(_tmp3A0.ReachableFL).f1;
_tmp3A2=(_tmp3A0.ReachableFL).f2;_tmp3A3=(_tmp3A0.ReachableFL).f3;_LL24E: _tmp3A3.may_consume=
0;{union Cyc_CfFlowInfo_FlowInfo_union _tmp5E2;return(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp5E2.ReachableFL).tag=1,(((_tmp5E2.ReachableFL).f1=_tmp3A1,(((_tmp5E2.ReachableFL).f2=
_tmp3A2,(((_tmp5E2.ReachableFL).f3=_tmp3A3,_tmp5E2))))))));}_LL24A:;}}struct
_tuple6 Cyc_CfFlowInfo_save_consume_info(struct Cyc_CfFlowInfo_FlowEnv*fenv,union
Cyc_CfFlowInfo_FlowInfo_union f,int clear);struct _tuple6 Cyc_CfFlowInfo_save_consume_info(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union f,int clear){
union Cyc_CfFlowInfo_FlowInfo_union _tmp3A5=f;struct Cyc_Dict_Dict _tmp3A6;struct Cyc_List_List*
_tmp3A7;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3A8;_LL250: if((_tmp3A5.BottomFL).tag
!= 0)goto _LL252;_LL251: {struct Cyc_CfFlowInfo_ConsumeInfo _tmp5E5;struct _tuple6
_tmp5E4;return(_tmp5E4.f1=((_tmp5E5.consumed=fenv->mt_place_set,((_tmp5E5.may_consume=
0,_tmp5E5)))),((_tmp5E4.f2=f,_tmp5E4)));}_LL252: if((_tmp3A5.ReachableFL).tag != 1)
goto _LL24F;_tmp3A6=(_tmp3A5.ReachableFL).f1;_tmp3A7=(_tmp3A5.ReachableFL).f2;
_tmp3A8=(_tmp3A5.ReachableFL).f3;_LL253: if(clear){union Cyc_CfFlowInfo_FlowInfo_union
_tmp5EB;struct Cyc_CfFlowInfo_ConsumeInfo _tmp5EA;struct _tuple6 _tmp5E9;return(
_tmp5E9.f1=_tmp3A8,((_tmp5E9.f2=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp5EB.ReachableFL).tag=
1,(((_tmp5EB.ReachableFL).f1=_tmp3A6,(((_tmp5EB.ReachableFL).f2=_tmp3A7,(((
_tmp5EB.ReachableFL).f3=((_tmp5EA.consumed=fenv->mt_place_set,((_tmp5EA.may_consume=
0,_tmp5EA)))),_tmp5EB)))))))),_tmp5E9)));}else{struct _tuple6 _tmp5EC;return(
_tmp5EC.f1=_tmp3A8,((_tmp5EC.f2=f,_tmp5EC)));}_LL24F:;}union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo_union f,struct
Cyc_CfFlowInfo_ConsumeInfo c,int may_consume_only);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo_union f,struct
Cyc_CfFlowInfo_ConsumeInfo c,int may_consume_only){union Cyc_CfFlowInfo_FlowInfo_union
_tmp3AF=f;struct Cyc_Dict_Dict _tmp3B0;struct Cyc_List_List*_tmp3B1;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3B2;_LL255: if((_tmp3AF.BottomFL).tag != 0)goto _LL257;_LL256: return f;_LL257:
if((_tmp3AF.ReachableFL).tag != 1)goto _LL254;_tmp3B0=(_tmp3AF.ReachableFL).f1;
_tmp3B1=(_tmp3AF.ReachableFL).f2;_tmp3B2=(_tmp3AF.ReachableFL).f3;_LL258: if(
may_consume_only){struct Cyc_CfFlowInfo_ConsumeInfo _tmp5EF;union Cyc_CfFlowInfo_FlowInfo_union
_tmp5EE;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp5EE.ReachableFL).tag=1,(((
_tmp5EE.ReachableFL).f1=_tmp3B0,(((_tmp5EE.ReachableFL).f2=_tmp3B1,(((_tmp5EE.ReachableFL).f3=((
_tmp5EF.consumed=_tmp3B2.consumed,((_tmp5EF.may_consume=c.may_consume,_tmp5EF)))),
_tmp5EE))))))));}{union Cyc_CfFlowInfo_FlowInfo_union _tmp5F0;return(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp5F0.ReachableFL).tag=1,(((_tmp5F0.ReachableFL).f1=_tmp3B0,(((_tmp5F0.ReachableFL).f2=
_tmp3B1,(((_tmp5F0.ReachableFL).f3=c,_tmp5F0))))))));}_LL254:;}
