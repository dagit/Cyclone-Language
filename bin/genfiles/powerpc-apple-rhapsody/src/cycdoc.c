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
dynregion;};struct Cyc_Core_Opt{void*v;};struct _dyneither_ptr Cyc_Core_new_string(
unsigned int);struct _tuple0{void*f1;void*f2;};void*Cyc_Core_fst(struct _tuple0*);
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[21];struct
Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[
12];struct Cyc_Core_Failure_struct{char*tag;struct _dyneither_ptr f1;};extern char
Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stdout;extern struct Cyc___cycFILE*Cyc_stderr;struct
Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr
f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_fflush(struct Cyc___cycFILE*);
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _dyneither_ptr,struct _dyneither_ptr);int Cyc_fputc(int,struct Cyc___cycFILE*);
int Cyc_fputs(const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{int
tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct
Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);int puts(const char*);int
remove(const char*);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};void Cyc_file_close(
struct Cyc___cycFILE*);extern char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern
char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_strconcat_l(
struct Cyc_List_List*);struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,
struct _dyneither_ptr);struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int
ofs,unsigned long n);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};void Cyc_Lineno_poss_of_abss(struct
_dyneither_ptr filename,struct Cyc_List_List*places);struct _dyneither_ptr Cyc_Filename_chop_extension(
struct _dyneither_ptr);extern char Cyc_Position_Exit[9];void Cyc_Position_reset_position(
struct _dyneither_ptr);struct Cyc_Position_Segment;struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];typedef struct{int quot;
int rem;}Cyc_div_t;typedef struct{long quot;long rem;}Cyc_ldiv_t;void*abort();void
exit(int);int system(const char*);extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct _dyneither_ptr Cyc_PP_string_of_doc(
struct Cyc_PP_Doc*d,int w);extern char Cyc_Arg_Bad[8];struct Cyc_Arg_Bad_struct{char*
tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[10];struct Cyc_Arg_Unit_spec_struct{
int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_struct{int tag;int*
f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;
void(*f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};void Cyc_Arg_usage(struct Cyc_List_List*,struct _dyneither_ptr);
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),
struct _dyneither_ptr errmsg,struct _dyneither_ptr args);struct Cyc_Absyn_Loc_n_struct{
int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct
Loc_n;struct Cyc_Absyn_Rel_n_struct Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};
struct _tuple1{union Cyc_Absyn_Nmspace_union f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Conref;
struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;
};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*f1;};
struct Cyc_Absyn_No_constr_struct{int tag;};union Cyc_Absyn_Constraint_union{struct
Cyc_Absyn_Eq_constr_struct Eq_constr;struct Cyc_Absyn_Forward_constr_struct
Forward_constr;struct Cyc_Absyn_No_constr_struct No_constr;};struct Cyc_Absyn_Conref{
union Cyc_Absyn_Constraint_union v;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*
name;int identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
rgn_loc;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;
struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*
zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;
struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct
_tuple1*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};union Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct
UnknownTunion;struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{
union Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
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
f1;};struct Cyc_Absyn_Region_d_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f);struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
struct _tuple4{void*f1;int f2;};struct Cyc_Int_tok_struct{int tag;struct _tuple4 f1;};
struct Cyc_Char_tok_struct{int tag;char f1;};struct Cyc_String_tok_struct{int tag;
struct _dyneither_ptr f1;};struct Cyc_Stringopt_tok_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_QualId_tok_struct{int tag;struct _tuple1*f1;};struct _tuple5{struct
Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Absyn_Conref*f3;};
struct Cyc_YY1_struct{int tag;struct _tuple5*f1;};struct Cyc_YY2_struct{int tag;
struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int tag;struct Cyc_Absyn_Exp*f1;}
;struct Cyc_YY4_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_YY5_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY7_struct{int tag;void*f1;};struct Cyc_YY8_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_YY9_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_YY10_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY11_struct{int tag;struct Cyc_Absyn_Pat*
f1;};struct _tuple6{struct Cyc_List_List*f1;int f2;};struct Cyc_YY12_struct{int tag;
struct _tuple6*f1;};struct Cyc_YY13_struct{int tag;struct Cyc_List_List*f1;};struct
_tuple7{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_YY14_struct{
int tag;struct _tuple7*f1;};struct Cyc_YY15_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY16_struct{int tag;struct _tuple6*f1;};struct Cyc_YY17_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_YY18_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY19_struct{int tag;struct Cyc_Declaration_spec*f1;};struct _tuple8{
struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_YY20_struct{int tag;
struct _tuple8*f1;};struct Cyc_YY21_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_YY22_struct{int tag;void*f1;};struct Cyc_YY23_struct{int tag;void*f1;};struct
Cyc_YY24_struct{int tag;void*f1;};struct Cyc_YY25_struct{int tag;struct Cyc_Absyn_Tqual
f1;};struct Cyc_YY26_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY27_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY28_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY29_struct{int tag;struct Cyc_Declarator*f1;};struct Cyc_YY30_struct{
int tag;struct Cyc_Abstractdeclarator*f1;};struct Cyc_YY31_struct{int tag;int f1;};
struct Cyc_YY32_struct{int tag;void*f1;};struct Cyc_YY33_struct{int tag;struct Cyc_Absyn_Tunionfield*
f1;};struct Cyc_YY34_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_YY35_struct{
int tag;struct _tuple9*f1;};struct Cyc_YY36_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY37_struct{int tag;struct _tuple2*f1;};struct Cyc_YY38_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple10{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*
f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_YY39_struct{int tag;
struct _tuple10*f1;};struct Cyc_YY40_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_YY41_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY42_struct{int tag;
void*f1;};struct Cyc_YY43_struct{int tag;void*f1;};struct Cyc_YY44_struct{int tag;
void*f1;};struct Cyc_YY45_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY46_struct{
int tag;void*f1;};struct Cyc_YY47_struct{int tag;struct Cyc_Absyn_Enumfield*f1;};
struct Cyc_YY48_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY49_struct{int
tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY50_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY51_struct{int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_YY52_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YYINITIALSVAL_struct{int tag;};union Cyc_YYSTYPE_union{
struct Cyc_Int_tok_struct Int_tok;struct Cyc_Char_tok_struct Char_tok;struct Cyc_String_tok_struct
String_tok;struct Cyc_Stringopt_tok_struct Stringopt_tok;struct Cyc_QualId_tok_struct
QualId_tok;struct Cyc_YY1_struct YY1;struct Cyc_YY2_struct YY2;struct Cyc_YY3_struct
YY3;struct Cyc_YY4_struct YY4;struct Cyc_YY5_struct YY5;struct Cyc_YY6_struct YY6;
struct Cyc_YY7_struct YY7;struct Cyc_YY8_struct YY8;struct Cyc_YY9_struct YY9;struct
Cyc_YY10_struct YY10;struct Cyc_YY11_struct YY11;struct Cyc_YY12_struct YY12;struct
Cyc_YY13_struct YY13;struct Cyc_YY14_struct YY14;struct Cyc_YY15_struct YY15;struct
Cyc_YY16_struct YY16;struct Cyc_YY17_struct YY17;struct Cyc_YY18_struct YY18;struct
Cyc_YY19_struct YY19;struct Cyc_YY20_struct YY20;struct Cyc_YY21_struct YY21;struct
Cyc_YY22_struct YY22;struct Cyc_YY23_struct YY23;struct Cyc_YY24_struct YY24;struct
Cyc_YY25_struct YY25;struct Cyc_YY26_struct YY26;struct Cyc_YY27_struct YY27;struct
Cyc_YY28_struct YY28;struct Cyc_YY29_struct YY29;struct Cyc_YY30_struct YY30;struct
Cyc_YY31_struct YY31;struct Cyc_YY32_struct YY32;struct Cyc_YY33_struct YY33;struct
Cyc_YY34_struct YY34;struct Cyc_YY35_struct YY35;struct Cyc_YY36_struct YY36;struct
Cyc_YY37_struct YY37;struct Cyc_YY38_struct YY38;struct Cyc_YY39_struct YY39;struct
Cyc_YY40_struct YY40;struct Cyc_YY41_struct YY41;struct Cyc_YY42_struct YY42;struct
Cyc_YY43_struct YY43;struct Cyc_YY44_struct YY44;struct Cyc_YY45_struct YY45;struct
Cyc_YY46_struct YY46;struct Cyc_YY47_struct YY47;struct Cyc_YY48_struct YY48;struct
Cyc_YY49_struct YY49;struct Cyc_YY50_struct YY50;struct Cyc_YY51_struct YY51;struct
Cyc_YY52_struct YY52;struct Cyc_YYINITIALSVAL_struct YYINITIALSVAL;};struct Cyc_Yyltype{
int timestamp;int first_line;int first_column;int last_line;int last_column;};struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_all_effects: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern int Cyc_Absynpp_print_scopes;struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl*d);void Cyc_Lex_lex_init(int use_cyclone_keywords);struct Cyc_Position_Segment{
int start;int end;};struct Cyc_MatchDecl_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_Standalone_struct{int tag;struct _dyneither_ptr f1;};struct _tuple11{int f1;
void*f2;};struct _tuple11*Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf);const int Cyc_lex_base[
15]=(const int[15]){0,- 4,0,- 3,1,2,3,0,4,6,7,- 1,8,9,- 2};const int Cyc_lex_backtrk[15]=(
const int[15]){- 1,- 1,3,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_default[
15]=(const int[15]){1,0,- 1,0,- 1,- 1,6,- 1,8,8,8,0,6,6,0};const int Cyc_lex_trans[266]=(
const int[266]){0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,
0,6,0,0,0,0,0,0,0,4,5,7,12,9,2,10,10,13,13,0,11,- 1,14,- 1,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,- 1,- 1,0,- 1,- 1,- 1,- 1};
const int Cyc_lex_check[266]=(const int[266]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,7,- 1,5,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,2,4,5,6,8,0,9,10,12,13,- 1,9,10,12,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,
- 1,6,8,- 1,9,10,12,13};int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*
lbuf);int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){int state;
int base;int backtrk;int c;state=start_state;if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=
lbuf->lex_curr_pos);lbuf->lex_last_action=- 1;}else{state=(- state)- 1;}while(1){
base=Cyc_lex_base[_check_known_subscript_notnull(15,state)];if(base < 0)return(-
base)- 1;backtrk=Cyc_lex_backtrk[_check_known_subscript_notnull(15,state)];if(
backtrk >= 0){lbuf->lex_last_pos=lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){if(!lbuf->lex_eof_reached)return(-
state)- 1;else{c=256;}}else{c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,
sizeof(char),lbuf->lex_curr_pos ++));if(c == - 1)c=256;}if(Cyc_lex_check[
_check_known_subscript_notnull(266,base + c)]== state)state=Cyc_lex_trans[
_check_known_subscript_notnull(266,base + c)];else{state=Cyc_lex_default[
_check_known_subscript_notnull(15,state)];}if(state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1){struct Cyc_Lexing_Error_struct _tmp93;const char*
_tmp92;struct Cyc_Lexing_Error_struct*_tmp91;(int)_throw((void*)((_tmp91=
_cycalloc(sizeof(*_tmp91)),((_tmp91[0]=((_tmp93.tag=Cyc_Lexing_Error,((_tmp93.f1=((
_tmp92="empty token",_tag_dyneither(_tmp92,sizeof(char),12))),_tmp93)))),_tmp91)))));}
else{return lbuf->lex_last_action;}}else{if(c == 256)lbuf->lex_eof_reached=0;}}}
struct _tuple11*Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);struct
_tuple11*Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LL0: {int _tmp3=lexbuf->lex_start_pos + 5;
int _tmp4=(lexbuf->lex_curr_pos - lexbuf->lex_start_pos)- 7;struct Cyc_Standalone_struct*
_tmp99;struct Cyc_Standalone_struct _tmp98;struct _tuple11*_tmp97;return(_tmp97=
_cycalloc(sizeof(*_tmp97)),((_tmp97->f1=Cyc_Lexing_lexeme_start(lexbuf),((_tmp97->f2=(
void*)((_tmp99=_cycalloc(sizeof(*_tmp99)),((_tmp99[0]=((_tmp98.tag=1,((_tmp98.f1=
Cyc_substring((struct _dyneither_ptr)lexbuf->lex_buffer,_tmp3,(unsigned long)
_tmp4),_tmp98)))),_tmp99)))),_tmp97)))));}case 1: _LL1: {int _tmp8=lexbuf->lex_start_pos
+ 4;int _tmp9=(lexbuf->lex_curr_pos - lexbuf->lex_start_pos)- 6;struct Cyc_MatchDecl_struct*
_tmp9F;struct Cyc_MatchDecl_struct _tmp9E;struct _tuple11*_tmp9D;return(_tmp9D=
_cycalloc(sizeof(*_tmp9D)),((_tmp9D->f1=Cyc_Lexing_lexeme_start(lexbuf),((_tmp9D->f2=(
void*)((_tmp9F=_cycalloc(sizeof(*_tmp9F)),((_tmp9F[0]=((_tmp9E.tag=0,((_tmp9E.f1=
Cyc_substring((struct _dyneither_ptr)lexbuf->lex_buffer,_tmp8,(unsigned long)
_tmp9),_tmp9E)))),_tmp9F)))),_tmp9D)))));}case 2: _LL2: return 0;case 3: _LL3: return
Cyc_token(lexbuf);default: _LL4:(lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmpA5;const char*_tmpA4;struct
Cyc_Lexing_Error_struct*_tmpA3;(int)_throw((void*)((_tmpA3=_cycalloc(sizeof(*
_tmpA3)),((_tmpA3[0]=((_tmpA5.tag=Cyc_Lexing_Error,((_tmpA5.f1=((_tmpA4="some action didn't return!",
_tag_dyneither(_tmpA4,sizeof(char),27))),_tmpA5)))),_tmpA3)))));}}struct _tuple11*
Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf);struct _tuple11*Cyc_token(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_token_rec(lexbuf,0);}struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*
r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;
struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;
struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict
tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};static void Cyc_dump_begin();
static void Cyc_dump_begin(){puts((const char*)"%%HEVEA \\begin{latexonly}\n\\begin{list}{}{\\setlength\\itemsep{0pt}\\setlength\\topsep{0pt}\\setlength\\leftmargin{\\parindent}\\setlength\\itemindent{-\\leftmargin}}\\item[]\\colorbox{cycdochighlight}{\\ttfamily\\begin{minipage}[b]{\\textwidth}\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}<dl><dt><table><tr><td bgcolor=\"c0d0ff\">\\end{rawhtml}\n\\begin{tabbing}");}
static void Cyc_dump_middle();static void Cyc_dump_middle(){puts((const char*)"\\end{tabbing}\n%%HEVEA \\begin{latexonly}\n\\end{minipage}}\\\\\\strut\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</td></tr></table><dd>\\end{rawhtml}");}
static void Cyc_dump_end();static void Cyc_dump_end(){puts((const char*)"%%HEVEA \\begin{latexonly}\n\\end{list}\\smallskip\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</dl>\\end{rawhtml}");}
static void Cyc_pr_comment(struct Cyc___cycFILE*outf,struct _dyneither_ptr s);static
void Cyc_pr_comment(struct Cyc___cycFILE*outf,struct _dyneither_ptr s){int depth=0;
int len=(int)Cyc_strlen((struct _dyneither_ptr)s);int i=0;for(0;i < len;++ i){char c=*((
const char*)_check_dyneither_subscript(s,sizeof(char),i));if(c != '['){Cyc_fputc((
int)c,outf);continue;}Cyc_fputs((const char*)"\\texttt{",outf);++ i;++ depth;for(0;
i < len;++ i){char c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(c == ']'){-- depth;if(depth == 0){Cyc_fputc((int)'}',outf);break;}}else{if(c == '[')
++ depth;}Cyc_fputc((int)c,outf);}}}static int Cyc_width=50;static void Cyc_set_width(
int w);static void Cyc_set_width(int w){Cyc_width=w;}static struct Cyc_List_List*Cyc_cycdoc_files=
0;static void Cyc_add_other(struct _dyneither_ptr s);static void Cyc_add_other(struct
_dyneither_ptr s){struct _dyneither_ptr*_tmpA8;struct Cyc_List_List*_tmpA7;Cyc_cycdoc_files=((
_tmpA7=_cycalloc(sizeof(*_tmpA7)),((_tmpA7->hd=((_tmpA8=_cycalloc(sizeof(*_tmpA8)),((
_tmpA8[0]=s,_tmpA8)))),((_tmpA7->tl=Cyc_cycdoc_files,_tmpA7))))));}static struct
Cyc_List_List*Cyc_cycargs=0;static void Cyc_add_cycarg(struct _dyneither_ptr s);
static void Cyc_add_cycarg(struct _dyneither_ptr s){struct _dyneither_ptr*_tmpAB;
struct Cyc_List_List*_tmpAA;Cyc_cycargs=((_tmpAA=_cycalloc(sizeof(*_tmpAA)),((
_tmpAA->hd=((_tmpAB=_cycalloc(sizeof(*_tmpAB)),((_tmpAB[0]=s,_tmpAB)))),((_tmpAA->tl=
Cyc_cycargs,_tmpAA))))));}static char _tmp14[8]="cyclone";static struct
_dyneither_ptr Cyc_cyclone_file={_tmp14,_tmp14,_tmp14 + 8};static void Cyc_set_cyclone_file(
struct _dyneither_ptr s);static void Cyc_set_cyclone_file(struct _dyneither_ptr s){Cyc_cyclone_file=
s;}static void Cyc_dumpdecl(struct Cyc_Absyn_Decl*d,struct _dyneither_ptr comment);
static void Cyc_dumpdecl(struct Cyc_Absyn_Decl*d,struct _dyneither_ptr comment){Cyc_dump_begin();{
const char*_tmpAF;void*_tmpAE[1];struct Cyc_String_pa_struct _tmpAD;(_tmpAD.tag=0,((
_tmpAD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(Cyc_Absynpp_decl2doc(
d),50)),((_tmpAE[0]=& _tmpAD,Cyc_printf(((_tmpAF="%s",_tag_dyneither(_tmpAF,
sizeof(char),3))),_tag_dyneither(_tmpAE,sizeof(void*),1)))))));}Cyc_dump_middle();
Cyc_pr_comment(Cyc_stdout,comment);{const char*_tmpB2;void*_tmpB1;(_tmpB1=0,Cyc_printf(((
_tmpB2="\n",_tag_dyneither(_tmpB2,sizeof(char),2))),_tag_dyneither(_tmpB1,
sizeof(void*),0)));}Cyc_dump_end();}static int Cyc_is_other_special(char c);static
int Cyc_is_other_special(char c){switch(c){case '\\': _LL6: goto _LL7;case '"': _LL7:
goto _LL8;case ';': _LL8: goto _LL9;case '&': _LL9: goto _LLA;case '(': _LLA: goto _LLB;case ')':
_LLB: goto _LLC;case '|': _LLC: goto _LLD;case '^': _LLD: goto _LLE;case '<': _LLE: goto _LLF;
case '>': _LLF: goto _LL10;case ' ': _LL10: goto _LL11;case '\n': _LL11: goto _LL12;case '\t':
_LL12: return 1;default: _LL13: return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(
struct _dyneither_ptr s);static void _tmpBB(unsigned int*_tmpBA,unsigned int*_tmpB9,
char**_tmpB7){for(*_tmpBA=0;*_tmpBA < *_tmpB9;(*_tmpBA)++){(*_tmpB7)[*_tmpBA]='\000';}}
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmp1A=Cyc_strlen((struct _dyneither_ptr)s);int _tmp1B=0;int _tmp1C=0;{
int i=0;for(0;i < _tmp1A;++ i){char _tmp1D=*((const char*)_check_dyneither_subscript(
s,sizeof(char),i));if(_tmp1D == '\'')++ _tmp1B;else{if(Cyc_is_other_special(_tmp1D))
++ _tmp1C;}}}if(_tmp1B == 0  && _tmp1C == 0)return s;if(_tmp1B == 0){struct
_dyneither_ptr*_tmpB5;struct _dyneither_ptr*_tmpB4[3];return(struct _dyneither_ptr)
Cyc_strconcat_l(((_tmpB4[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),"'",sizeof(char),2),((_tmpB4[1]=((_tmpB5=_cycalloc(sizeof(*
_tmpB5)),((_tmpB5[0]=(struct _dyneither_ptr)s,_tmpB5)))),((_tmpB4[0]=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpB4,sizeof(struct _dyneither_ptr*),3)))))))));}{unsigned long _tmp22=(_tmp1A + 
_tmp1B)+ _tmp1C;unsigned int _tmpBA;unsigned int _tmpB9;struct _dyneither_ptr _tmpB8;
char*_tmpB7;unsigned int _tmpB6;struct _dyneither_ptr s2=(_tmpB6=_tmp22 + 1,((_tmpB7=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmpB6 + 1)),((_tmpB8=
_tag_dyneither(_tmpB7,sizeof(char),_tmpB6 + 1),((((_tmpB9=_tmpB6,((_tmpBB(& _tmpBA,&
_tmpB9,& _tmpB7),_tmpB7[_tmpB9]=(char)0)))),_tmpB8)))))));int _tmp23=0;int _tmp24=0;
for(0;_tmp23 < _tmp1A;++ _tmp23){char _tmp25=*((const char*)
_check_dyneither_subscript(s,sizeof(char),_tmp23));if(_tmp25 == '\''  || Cyc_is_other_special(
_tmp25)){char _tmpBE;char _tmpBD;struct _dyneither_ptr _tmpBC;(_tmpBC=
_dyneither_ptr_plus(s2,sizeof(char),_tmp24 ++),((_tmpBD=*((char*)
_check_dyneither_subscript(_tmpBC,sizeof(char),0)),((_tmpBE='\\',((
_get_dyneither_size(_tmpBC,sizeof(char))== 1  && (_tmpBD == '\000'  && _tmpBE != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpBC.curr)=_tmpBE)))))));}{char _tmpC1;char
_tmpC0;struct _dyneither_ptr _tmpBF;(_tmpBF=_dyneither_ptr_plus(s2,sizeof(char),
_tmp24 ++),((_tmpC0=*((char*)_check_dyneither_subscript(_tmpBF,sizeof(char),0)),((
_tmpC1=_tmp25,((_get_dyneither_size(_tmpBF,sizeof(char))== 1  && (_tmpC0 == '\000'
 && _tmpC1 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpBF.curr)=_tmpC1)))))));}}
return(struct _dyneither_ptr)s2;}}static struct _dyneither_ptr*Cyc_sh_escape_stringptr(
struct _dyneither_ptr*sp);static struct _dyneither_ptr*Cyc_sh_escape_stringptr(
struct _dyneither_ptr*sp){struct _dyneither_ptr*_tmpC2;return(_tmpC2=_cycalloc(
sizeof(*_tmpC2)),((_tmpC2[0]=Cyc_sh_escape_string(*sp),_tmpC2)));}static struct
Cyc_Lineno_Pos*Cyc_new_pos();static struct Cyc_Lineno_Pos*Cyc_new_pos(){const char*
_tmpC5;struct Cyc_Lineno_Pos*_tmpC4;return(_tmpC4=_cycalloc(sizeof(*_tmpC4)),((
_tmpC4->logical_file=((_tmpC5="",_tag_dyneither(_tmpC5,sizeof(char),1))),((
_tmpC4->line=Cyc_Core_new_string(0),((_tmpC4->line_no=0,((_tmpC4->col=0,_tmpC4)))))))));}
struct _tuple12{int f1;struct Cyc_Lineno_Pos*f2;};static struct _tuple12*Cyc_start2pos(
int x);static struct _tuple12*Cyc_start2pos(int x){struct _tuple12*_tmpC6;return(
_tmpC6=_cycalloc(sizeof(*_tmpC6)),((_tmpC6->f1=x,((_tmpC6->f2=Cyc_new_pos(),
_tmpC6)))));}static int Cyc_decl2start(struct Cyc_Absyn_Decl*d);static int Cyc_decl2start(
struct Cyc_Absyn_Decl*d){return((struct Cyc_Position_Segment*)_check_null(d->loc))->start;}
struct _tuple13{struct Cyc_Lineno_Pos*f1;void*f2;};static struct Cyc_List_List*Cyc_this_file(
struct _dyneither_ptr file,struct Cyc_List_List*x);static struct Cyc_List_List*Cyc_this_file(
struct _dyneither_ptr file,struct Cyc_List_List*x){struct Cyc_List_List*_tmp34=0;
for(0;x != 0;x=x->tl){if(Cyc_strcmp((struct _dyneither_ptr)((*((struct _tuple13*)x->hd)).f1)->logical_file,(
struct _dyneither_ptr)file)== 0){struct Cyc_List_List*_tmpC7;_tmp34=((_tmpC7=
_cycalloc(sizeof(*_tmpC7)),((_tmpC7->hd=(struct _tuple13*)x->hd,((_tmpC7->tl=
_tmp34,_tmpC7))))));}}_tmp34=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp34);return _tmp34;}static int Cyc_lineno(struct Cyc_Lineno_Pos*
p);static int Cyc_lineno(struct Cyc_Lineno_Pos*p){return p->line_no;}static struct Cyc_List_List*
Cyc_flatten_decls(struct Cyc_List_List*decls);static struct Cyc_List_List*Cyc_flatten_decls(
struct Cyc_List_List*decls){struct Cyc_List_List*_tmp36=0;while(decls != 0){void*
_tmp37=(void*)((struct Cyc_Absyn_Decl*)decls->hd)->r;struct Cyc_List_List*_tmp38;
struct Cyc_List_List*_tmp39;struct Cyc_List_List*_tmp3A;struct Cyc_List_List*_tmp3B;
_LL16: if(_tmp37 <= (void*)2)goto _LL2E;if(*((int*)_tmp37)!= 10)goto _LL18;_tmp38=((
struct Cyc_Absyn_Namespace_d_struct*)_tmp37)->f2;_LL17: _tmp39=_tmp38;goto _LL19;
_LL18: if(*((int*)_tmp37)!= 11)goto _LL1A;_tmp39=((struct Cyc_Absyn_Using_d_struct*)
_tmp37)->f2;_LL19: _tmp3A=_tmp39;goto _LL1B;_LL1A: if(*((int*)_tmp37)!= 12)goto
_LL1C;_tmp3A=((struct Cyc_Absyn_ExternC_d_struct*)_tmp37)->f1;_LL1B: _tmp3B=_tmp3A;
goto _LL1D;_LL1C: if(*((int*)_tmp37)!= 13)goto _LL1E;_tmp3B=((struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp37)->f1;_LL1D: decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp3B,decls->tl);goto _LL15;_LL1E: if(*((int*)
_tmp37)!= 6)goto _LL20;_LL1F: goto _LL21;_LL20: if(*((int*)_tmp37)!= 0)goto _LL22;
_LL21: goto _LL23;_LL22: if(*((int*)_tmp37)!= 7)goto _LL24;_LL23: goto _LL25;_LL24: if(*((
int*)_tmp37)!= 8)goto _LL26;_LL25: goto _LL27;_LL26: if(*((int*)_tmp37)!= 9)goto
_LL28;_LL27: goto _LL29;_LL28: if(*((int*)_tmp37)!= 1)goto _LL2A;_LL29: goto _LL2B;
_LL2A: if(*((int*)_tmp37)!= 2)goto _LL2C;_LL2B: goto _LL2D;_LL2C: if(*((int*)_tmp37)
!= 3)goto _LL2E;_LL2D: goto _LL2F;_LL2E: if((int)_tmp37 != 0)goto _LL30;_LL2F: goto
_LL31;_LL30: if((int)_tmp37 != 1)goto _LL32;_LL31: goto _LL33;_LL32: if(_tmp37 <= (void*)
2)goto _LL34;if(*((int*)_tmp37)!= 4)goto _LL34;_LL33: goto _LL35;_LL34: if(_tmp37 <= (
void*)2)goto _LL15;if(*((int*)_tmp37)!= 5)goto _LL15;_LL35:{struct Cyc_List_List*
_tmpC8;_tmp36=((_tmpC8=_cycalloc(sizeof(*_tmpC8)),((_tmpC8->hd=(struct Cyc_Absyn_Decl*)
decls->hd,((_tmpC8->tl=_tmp36,_tmpC8))))));}decls=decls->tl;goto _LL15;_LL15:;}
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp36);}
struct _tuple14{int f1;struct Cyc_Absyn_Decl*f2;};struct _tuple15{struct Cyc_Lineno_Pos*
f1;struct Cyc_Absyn_Decl*f2;};static void Cyc_process_file(struct _dyneither_ptr
filename);static void Cyc_process_file(struct _dyneither_ptr filename){struct
_dyneither_ptr _tmp3D=Cyc_Filename_chop_extension(filename);const char*_tmpC9;
const char*_tmp3E=(const char*)_untag_dyneither_ptr(Cyc_strconcat((struct
_dyneither_ptr)_tmp3D,((_tmpC9=".cyp",_tag_dyneither(_tmpC9,sizeof(char),5)))),
sizeof(char),1);const char*_tmpD0;struct _dyneither_ptr*_tmpCF;const char*_tmpCE;
struct Cyc_List_List*_tmpCD;struct _dyneither_ptr _tmp3F=Cyc_str_sepstr(((_tmpCD=
_cycalloc(sizeof(*_tmpCD)),((_tmpCD->hd=((_tmpCF=_cycalloc(sizeof(*_tmpCF)),((
_tmpCF[0]=(struct _dyneither_ptr)((_tmpCE="",_tag_dyneither(_tmpCE,sizeof(char),1))),
_tmpCF)))),((_tmpCD->tl=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(
struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cycargs)),
_tmpCD)))))),((_tmpD0=" ",_tag_dyneither(_tmpD0,sizeof(char),2))));const char*
_tmpD9;const char*_tmpD8;void*_tmpD7[4];struct Cyc_String_pa_struct _tmpD6;struct
Cyc_String_pa_struct _tmpD5;struct Cyc_String_pa_struct _tmpD4;struct Cyc_String_pa_struct
_tmpD3;const char*_tmp40=(const char*)_untag_dyneither_ptr(((_tmpD3.tag=0,((_tmpD3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(filename)),((
_tmpD4.tag=0,((_tmpD4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(((
_tmpD9=_tmp3E,_tag_dyneither(_tmpD9,sizeof(char),_get_zero_arr_size_char(_tmpD9,
1)))))),((_tmpD5.tag=0,((_tmpD5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp3F),((_tmpD6.tag=0,((_tmpD6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_cyclone_file),((_tmpD7[0]=& _tmpD6,((_tmpD7[1]=& _tmpD5,((_tmpD7[2]=& _tmpD4,((
_tmpD7[3]=& _tmpD3,Cyc_aprintf(((_tmpD8="%s %s -E -o %s -x cyc %s",_tag_dyneither(
_tmpD8,sizeof(char),25))),_tag_dyneither(_tmpD7,sizeof(void*),4)))))))))))))))))))))))))),
sizeof(char),1);if(system(_tmp40)!= 0){{const char*_tmpDC;void*_tmpDB;(_tmpDB=0,
Cyc_fprintf(Cyc_stderr,((_tmpDC="\nError: preprocessing\n",_tag_dyneither(_tmpDC,
sizeof(char),23))),_tag_dyneither(_tmpDB,sizeof(void*),0)));}return;}{const char*
_tmpDD;Cyc_Position_reset_position(((_tmpDD=_tmp3E,_tag_dyneither(_tmpDD,sizeof(
char),_get_zero_arr_size_char(_tmpDD,1)))));}{struct Cyc___cycFILE*in_file=(
struct Cyc___cycFILE*)_check_null(Cyc_fopen(_tmp3E,(const char*)"r"));Cyc_Lex_lex_init(
1);{struct Cyc_List_List*_tmp44=Cyc_Parse_parse_file(in_file);Cyc_Lex_lex_init(1);
Cyc_file_close((struct Cyc___cycFILE*)in_file);_tmp44=Cyc_flatten_decls(_tmp44);{
struct Cyc_List_List*_tmp45=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(int),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_start2pos,((struct Cyc_List_List*(*)(int(*
f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_decl2start,
_tmp44));{const char*_tmpDE;Cyc_Lineno_poss_of_abss(((_tmpDE=_tmp3E,
_tag_dyneither(_tmpDE,sizeof(char),_get_zero_arr_size_char(_tmpDE,1)))),_tmp45);}
remove(_tmp3E);{struct Cyc_List_List*_tmp47=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*
f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple12*))Cyc_Core_snd,_tmp45),_tmp44);_tmp47=((struct Cyc_List_List*(*)(
struct _dyneither_ptr file,struct Cyc_List_List*x))Cyc_this_file)(filename,_tmp47);{
struct Cyc_List_List*_tmp48=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Lineno_Pos*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(struct
Cyc_Lineno_Pos*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Lineno_Pos*(*)(struct _tuple15*))Cyc_Core_fst,_tmp47)),((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Decl*(*)(struct _tuple15*))Cyc_Core_snd,_tmp47));struct Cyc___cycFILE*
_tmp49=(struct Cyc___cycFILE*)_check_null(Cyc_fopen((const char*)
_untag_dyneither_ptr(filename,sizeof(char),1),(const char*)"r"));struct Cyc_Lexing_lexbuf*
_tmp4A=Cyc_Lexing_from_file(_tmp49);struct Cyc_List_List*_tmp4B=0;struct _tuple11*
comment;while((comment=((struct _tuple11*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_token)(
_tmp4A))!= 0){struct Cyc_List_List*_tmpDF;_tmp4B=((_tmpDF=_cycalloc(sizeof(*
_tmpDF)),((_tmpDF->hd=(struct _tuple11*)_check_null(comment),((_tmpDF->tl=_tmp4B,
_tmpDF))))));}Cyc_fclose(_tmp49);_tmp4B=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp4B);{struct Cyc_List_List*_tmp4D=((struct Cyc_List_List*(*)(
struct _tuple12*(*f)(int),struct Cyc_List_List*x))Cyc_List_map)(Cyc_start2pos,((
struct Cyc_List_List*(*)(int(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((
int(*)(struct _tuple11*))Cyc_Core_fst,_tmp4B));Cyc_Lineno_poss_of_abss(filename,
_tmp4D);{struct Cyc_List_List*_tmp4E=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*
f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple12*))Cyc_Core_snd,_tmp4D),((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))Cyc_Core_snd,
_tmp4B));_tmp4E=Cyc_this_file(filename,_tmp4E);{struct Cyc_List_List*_tmp4F=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*f)(
struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple13*))Cyc_Core_fst,_tmp4E)),((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))
Cyc_Core_snd,_tmp4E));while(_tmp48 != 0  && _tmp4F != 0){struct _tuple14 _tmp51;int
_tmp52;struct Cyc_Absyn_Decl*_tmp53;struct _tuple14*_tmp50=(struct _tuple14*)_tmp48->hd;
_tmp51=*_tmp50;_tmp52=_tmp51.f1;_tmp53=_tmp51.f2;{struct _tuple11 _tmp55;int _tmp56;
void*_tmp57;struct _tuple11*_tmp54=(struct _tuple11*)_tmp4F->hd;_tmp55=*_tmp54;
_tmp56=_tmp55.f1;_tmp57=_tmp55.f2;{void*_tmp58=_tmp57;struct _dyneither_ptr _tmp59;
struct _dyneither_ptr _tmp5A;_LL37: if(*((int*)_tmp58)!= 1)goto _LL39;_tmp59=((
struct Cyc_Standalone_struct*)_tmp58)->f1;_LL38: Cyc_pr_comment(Cyc_stdout,(struct
_dyneither_ptr)_tmp59);{const char*_tmpE2;void*_tmpE1;(_tmpE1=0,Cyc_printf(((
_tmpE2="\n",_tag_dyneither(_tmpE2,sizeof(char),2))),_tag_dyneither(_tmpE1,
sizeof(void*),0)));}_tmp4F=_tmp4F->tl;goto _LL36;_LL39: if(*((int*)_tmp58)!= 0)
goto _LL36;_tmp5A=((struct Cyc_MatchDecl_struct*)_tmp58)->f1;_LL3A: if(_tmp56 < 
_tmp52){_tmp4F=_tmp4F->tl;continue;}if(_tmp48->tl != 0){struct _tuple14 _tmp5E;int
_tmp5F;struct _tuple14*_tmp5D=(struct _tuple14*)((struct Cyc_List_List*)_check_null(
_tmp48->tl))->hd;_tmp5E=*_tmp5D;_tmp5F=_tmp5E.f1;if(_tmp5F < _tmp56){_tmp48=
_tmp48->tl;continue;}}Cyc_dumpdecl(_tmp53,(struct _dyneither_ptr)_tmp5A);Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stdout);_tmp48=_tmp48->tl;_tmp4F=_tmp4F->tl;goto _LL36;
_LL36:;}}}}}}}}}}}}void GC_blacklist_warn_clear();struct _tuple16{struct
_dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv);int Cyc_main(int argc,struct
_dyneither_ptr argv){GC_blacklist_warn_clear();{struct _tuple16*_tmp13D;const char*
_tmp13C;struct Cyc_Arg_Flag_spec_struct _tmp13B;struct Cyc_Arg_Flag_spec_struct*
_tmp13A;const char*_tmp139;const char*_tmp138;struct _tuple16*_tmp137;const char*
_tmp136;struct Cyc_Arg_Flag_spec_struct _tmp135;struct Cyc_Arg_Flag_spec_struct*
_tmp134;const char*_tmp133;const char*_tmp132;struct _tuple16*_tmp131;const char*
_tmp130;struct Cyc_Arg_Flag_spec_struct _tmp12F;struct Cyc_Arg_Flag_spec_struct*
_tmp12E;const char*_tmp12D;const char*_tmp12C;struct _tuple16*_tmp12B;const char*
_tmp12A;struct Cyc_Arg_Int_spec_struct _tmp129;struct Cyc_Arg_Int_spec_struct*
_tmp128;const char*_tmp127;const char*_tmp126;struct _tuple16*_tmp125;const char*
_tmp124;struct Cyc_Arg_String_spec_struct _tmp123;struct Cyc_Arg_String_spec_struct*
_tmp122;const char*_tmp121;const char*_tmp120;struct _tuple16*_tmp11F[5];struct Cyc_List_List*
options=(_tmp11F[4]=((_tmp13D=_cycalloc(sizeof(*_tmp13D)),((_tmp13D->f1=((
_tmp138="-B",_tag_dyneither(_tmp138,sizeof(char),3))),((_tmp13D->f2=1,((_tmp13D->f3=((
_tmp139="<file>",_tag_dyneither(_tmp139,sizeof(char),7))),((_tmp13D->f4=(void*)((
_tmp13A=_cycalloc(sizeof(*_tmp13A)),((_tmp13A[0]=((_tmp13B.tag=1,((_tmp13B.f1=
Cyc_add_cycarg,_tmp13B)))),_tmp13A)))),((_tmp13D->f5=((_tmp13C="Add to the list of directories to search for compiler files",
_tag_dyneither(_tmp13C,sizeof(char),60))),_tmp13D)))))))))))),((_tmp11F[3]=((
_tmp137=_cycalloc(sizeof(*_tmp137)),((_tmp137->f1=((_tmp132="-I",_tag_dyneither(
_tmp132,sizeof(char),3))),((_tmp137->f2=1,((_tmp137->f3=((_tmp133="<dir>",
_tag_dyneither(_tmp133,sizeof(char),6))),((_tmp137->f4=(void*)((_tmp134=
_cycalloc(sizeof(*_tmp134)),((_tmp134[0]=((_tmp135.tag=1,((_tmp135.f1=Cyc_add_cycarg,
_tmp135)))),_tmp134)))),((_tmp137->f5=((_tmp136="Add to the list of directories to search for include files",
_tag_dyneither(_tmp136,sizeof(char),59))),_tmp137)))))))))))),((_tmp11F[2]=((
_tmp131=_cycalloc(sizeof(*_tmp131)),((_tmp131->f1=((_tmp12C="-D",_tag_dyneither(
_tmp12C,sizeof(char),3))),((_tmp131->f2=1,((_tmp131->f3=((_tmp12D="<name>[=<value>]",
_tag_dyneither(_tmp12D,sizeof(char),17))),((_tmp131->f4=(void*)((_tmp12E=
_cycalloc(sizeof(*_tmp12E)),((_tmp12E[0]=((_tmp12F.tag=1,((_tmp12F.f1=Cyc_add_cycarg,
_tmp12F)))),_tmp12E)))),((_tmp131->f5=((_tmp130="Pass definition to preprocessor",
_tag_dyneither(_tmp130,sizeof(char),32))),_tmp131)))))))))))),((_tmp11F[1]=((
_tmp12B=_cycalloc(sizeof(*_tmp12B)),((_tmp12B->f1=((_tmp126="-w",_tag_dyneither(
_tmp126,sizeof(char),3))),((_tmp12B->f2=0,((_tmp12B->f3=((_tmp127=" <width>",
_tag_dyneither(_tmp127,sizeof(char),9))),((_tmp12B->f4=(void*)((_tmp128=
_cycalloc(sizeof(*_tmp128)),((_tmp128[0]=((_tmp129.tag=6,((_tmp129.f1=Cyc_set_width,
_tmp129)))),_tmp128)))),((_tmp12B->f5=((_tmp12A="Use <width> as the max width for printing declarations",
_tag_dyneither(_tmp12A,sizeof(char),55))),_tmp12B)))))))))))),((_tmp11F[0]=((
_tmp125=_cycalloc(sizeof(*_tmp125)),((_tmp125->f1=((_tmp120="-cyclone",
_tag_dyneither(_tmp120,sizeof(char),9))),((_tmp125->f2=0,((_tmp125->f3=((_tmp121=" <file>",
_tag_dyneither(_tmp121,sizeof(char),8))),((_tmp125->f4=(void*)((_tmp122=
_cycalloc(sizeof(*_tmp122)),((_tmp122[0]=((_tmp123.tag=5,((_tmp123.f1=Cyc_set_cyclone_file,
_tmp123)))),_tmp122)))),((_tmp125->f5=((_tmp124="Use <file> as the cyclone compiler",
_tag_dyneither(_tmp124,sizeof(char),35))),_tmp125)))))))))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp11F,sizeof(struct
_tuple16*),5)))))))))));{const char*_tmp13E;Cyc_Arg_parse(options,Cyc_add_other,((
_tmp13E="Options:",_tag_dyneither(_tmp13E,sizeof(char),9))),argv);}if(Cyc_cycdoc_files
== 0){{const char*_tmp13F;Cyc_Arg_usage(options,((_tmp13F="Usage: cycdoc [options] file1 file2 ...\nOptions:",
_tag_dyneither(_tmp13F,sizeof(char),49))));}exit(1);}Cyc_PP_tex_output=1;Cyc_Absynpp_print_scopes=
0;{struct Cyc_List_List*_tmp6E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(Cyc_cycdoc_files);for(0;_tmp6E != 0;_tmp6E=_tmp6E->tl){Cyc_process_file(*((
struct _dyneither_ptr*)_tmp6E->hd));}}return 0;}}
