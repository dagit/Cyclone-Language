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
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];extern char
Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];
struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};int Cyc_strptrcmp(struct _dyneither_ptr*s1,
struct _dyneither_ptr*s2);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
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
_union_Cnst_String_c String_c;};union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(void*,int);
struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;
struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{int tag;
struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;unsigned int
f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**
elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
union Cyc_Absyn_Cnst,struct Cyc_Position_Segment*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple9{void*f1;void*f2;};struct _tuple9*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple9*Cyc_Dict_rchoose(
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
le;int allow_valueof;};void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(
void*,void*);struct _tuple10{unsigned int f1;int f2;};struct _tuple10 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_okay_szofarg(void*t);struct _tuple11{
union Cyc_Absyn_Cnst f1;int f2;};static struct _tuple11 Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp*);static union Cyc_Absyn_Cnst Cyc_Evexp_promote_const(union Cyc_Absyn_Cnst
cn);static union Cyc_Absyn_Cnst Cyc_Evexp_promote_const(union Cyc_Absyn_Cnst cn){
union Cyc_Absyn_Cnst _tmp0=cn;struct _tuple3 _tmp1;void*_tmp2;char _tmp3;struct
_tuple4 _tmp4;void*_tmp5;short _tmp6;_LL1: if((_tmp0.Char_c).tag != 2)goto _LL3;_tmp1=(
struct _tuple3)(_tmp0.Char_c).val;_tmp2=_tmp1.f1;_tmp3=_tmp1.f2;_LL2: return Cyc_Absyn_Int_c(
_tmp2,(int)_tmp3);_LL3: if((_tmp0.Short_c).tag != 3)goto _LL5;_tmp4=(struct _tuple4)(
_tmp0.Short_c).val;_tmp5=_tmp4.f1;_tmp6=_tmp4.f2;_LL4: return Cyc_Absyn_Int_c(
_tmp5,(int)_tmp6);_LL5:;_LL6: return cn;_LL0:;}struct _tuple10 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e){union Cyc_Absyn_Cnst _tmp8;int _tmp9;struct _tuple11 _tmp7=Cyc_Evexp_eval_const_exp(
e);_tmp8=_tmp7.f1;_tmp9=_tmp7.f2;if(!_tmp9){struct _tuple10 _tmp17F;return(_tmp17F.f1=
0,((_tmp17F.f2=0,_tmp17F)));}{union Cyc_Absyn_Cnst _tmpB=Cyc_Evexp_promote_const(
_tmp8);struct _tuple5 _tmpC;int _tmpD;_LL8: if((_tmpB.Int_c).tag != 4)goto _LLA;_tmpC=(
struct _tuple5)(_tmpB.Int_c).val;_tmpD=_tmpC.f2;_LL9: {struct _tuple10 _tmp180;
return(_tmp180.f1=(unsigned int)_tmpD,((_tmp180.f2=1,_tmp180)));}_LLA:;_LLB:{
const char*_tmp183;void*_tmp182;(_tmp182=0,Cyc_Tcutil_terr(e->loc,((_tmp183="expecting unsigned int",
_tag_dyneither(_tmp183,sizeof(char),23))),_tag_dyneither(_tmp182,sizeof(void*),0)));}{
struct _tuple10 _tmp184;return(_tmp184.f1=0,((_tmp184.f2=1,_tmp184)));}_LL7:;}}
struct _tuple12{int f1;int f2;};static struct _tuple12 Cyc_Evexp_eval_const_bool_exp(
struct Cyc_Absyn_Exp*e);static struct _tuple12 Cyc_Evexp_eval_const_bool_exp(struct
Cyc_Absyn_Exp*e){union Cyc_Absyn_Cnst _tmp13;int _tmp14;struct _tuple11 _tmp12=Cyc_Evexp_eval_const_exp(
e);_tmp13=_tmp12.f1;_tmp14=_tmp12.f2;if(!_tmp14){struct _tuple12 _tmp185;return(
_tmp185.f1=0,((_tmp185.f2=0,_tmp185)));}{union Cyc_Absyn_Cnst _tmp16=Cyc_Evexp_promote_const(
_tmp13);struct _tuple5 _tmp17;int _tmp18;struct _tuple6 _tmp19;long long _tmp1A;int
_tmp1B;_LLD: if((_tmp16.Int_c).tag != 4)goto _LLF;_tmp17=(struct _tuple5)(_tmp16.Int_c).val;
_tmp18=_tmp17.f2;_LLE: {struct _tuple12 _tmp186;return(_tmp186.f1=_tmp18 != 0,((
_tmp186.f2=1,_tmp186)));}_LLF: if((_tmp16.LongLong_c).tag != 5)goto _LL11;_tmp19=(
struct _tuple6)(_tmp16.LongLong_c).val;_tmp1A=_tmp19.f2;_LL10: {struct _tuple12
_tmp187;return(_tmp187.f1=_tmp1A != 0,((_tmp187.f2=1,_tmp187)));}_LL11: if((_tmp16.Null_c).tag
!= 1)goto _LL13;_tmp1B=(int)(_tmp16.Null_c).val;_LL12: {struct _tuple12 _tmp188;
return(_tmp188.f1=0,((_tmp188.f2=0,_tmp188)));}_LL13:;_LL14:{const char*_tmp18B;
void*_tmp18A;(_tmp18A=0,Cyc_Tcutil_terr(e->loc,((_tmp18B="expecting bool",
_tag_dyneither(_tmp18B,sizeof(char),15))),_tag_dyneither(_tmp18A,sizeof(void*),0)));}{
struct _tuple12 _tmp18C;return(_tmp18C.f1=0,((_tmp18C.f2=0,_tmp18C)));}_LLC:;}}
struct _tuple13{void*f1;union Cyc_Absyn_Cnst f2;};static struct _tuple11 Cyc_Evexp_eval_const_unprimop(
void*p,struct Cyc_Absyn_Exp*e);static struct _tuple11 Cyc_Evexp_eval_const_unprimop(
void*p,struct Cyc_Absyn_Exp*e){union Cyc_Absyn_Cnst _tmp23;int _tmp24;struct _tuple11
_tmp22=Cyc_Evexp_eval_const_exp(e);_tmp23=_tmp22.f1;_tmp24=_tmp22.f2;if(!_tmp24){
struct _tuple11 _tmp18D;return(_tmp18D.f1=_tmp23,((_tmp18D.f2=0,_tmp18D)));}{
struct _tuple13 _tmp18E;struct _tuple13 _tmp27=(_tmp18E.f1=p,((_tmp18E.f2=_tmp23,
_tmp18E)));void*_tmp28;void*_tmp29;union Cyc_Absyn_Cnst _tmp2A;struct _tuple5 _tmp2B;
void*_tmp2C;int _tmp2D;void*_tmp2E;union Cyc_Absyn_Cnst _tmp2F;struct _tuple5 _tmp30;
void*_tmp31;int _tmp32;void*_tmp33;union Cyc_Absyn_Cnst _tmp34;struct _tuple5 _tmp35;
int _tmp36;void*_tmp37;union Cyc_Absyn_Cnst _tmp38;int _tmp39;_LL16: _tmp28=_tmp27.f1;
if((int)_tmp28 != 0)goto _LL18;_LL17: goto _LL15;_LL18: _tmp29=_tmp27.f1;if((int)
_tmp29 != 2)goto _LL1A;_tmp2A=_tmp27.f2;if((_tmp2A.Int_c).tag != 4)goto _LL1A;_tmp2B=(
struct _tuple5)(_tmp2A.Int_c).val;_tmp2C=_tmp2B.f1;_tmp2D=_tmp2B.f2;_LL19: _tmp23=
Cyc_Absyn_Int_c((void*)0,- _tmp2D);goto _LL15;_LL1A: _tmp2E=_tmp27.f1;if((int)
_tmp2E != 12)goto _LL1C;_tmp2F=_tmp27.f2;if((_tmp2F.Int_c).tag != 4)goto _LL1C;
_tmp30=(struct _tuple5)(_tmp2F.Int_c).val;_tmp31=_tmp30.f1;_tmp32=_tmp30.f2;_LL1B:
_tmp23=Cyc_Absyn_Int_c((void*)1,~ _tmp32);goto _LL15;_LL1C: _tmp33=_tmp27.f1;if((
int)_tmp33 != 11)goto _LL1E;_tmp34=_tmp27.f2;if((_tmp34.Int_c).tag != 4)goto _LL1E;
_tmp35=(struct _tuple5)(_tmp34.Int_c).val;_tmp36=_tmp35.f2;_LL1D: _tmp23=Cyc_Absyn_Int_c((
void*)0,_tmp36 == 0?1: 0);goto _LL15;_LL1E: _tmp37=_tmp27.f1;if((int)_tmp37 != 11)
goto _LL20;_tmp38=_tmp27.f2;if((_tmp38.Null_c).tag != 1)goto _LL20;_tmp39=(int)(
_tmp38.Null_c).val;_LL1F: _tmp23=Cyc_Absyn_Int_c((void*)0,1);goto _LL15;_LL20:;
_LL21:{const char*_tmp191;void*_tmp190;(_tmp190=0,Cyc_Tcutil_terr(e->loc,((
_tmp191="bad constant expression",_tag_dyneither(_tmp191,sizeof(char),24))),
_tag_dyneither(_tmp190,sizeof(void*),0)));}goto _LL15;_LL15:;}{struct _tuple11
_tmp192;return(_tmp192.f1=_tmp23,((_tmp192.f2=1,_tmp192)));}}static struct
_tuple11 Cyc_Evexp_eval_const_binprimop(void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);static struct _tuple11 Cyc_Evexp_eval_const_binprimop(void*p,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){union Cyc_Absyn_Cnst _tmp3E;int _tmp3F;struct _tuple11
_tmp3D=Cyc_Evexp_eval_const_exp(e1);_tmp3E=_tmp3D.f1;_tmp3F=_tmp3D.f2;{union Cyc_Absyn_Cnst
_tmp41;int _tmp42;struct _tuple11 _tmp40=Cyc_Evexp_eval_const_exp(e2);_tmp41=_tmp40.f1;
_tmp42=_tmp40.f2;if(!_tmp3F  || !_tmp42){struct _tuple11 _tmp193;return(_tmp193.f1=
_tmp3E,((_tmp193.f2=0,_tmp193)));}_tmp3E=Cyc_Evexp_promote_const(_tmp3E);_tmp41=
Cyc_Evexp_promote_const(_tmp41);{void*s1;void*s2;int i1;int i2;{union Cyc_Absyn_Cnst
_tmp44=_tmp3E;struct _tuple5 _tmp45;void*_tmp46;int _tmp47;_LL23: if((_tmp44.Int_c).tag
!= 4)goto _LL25;_tmp45=(struct _tuple5)(_tmp44.Int_c).val;_tmp46=_tmp45.f1;_tmp47=
_tmp45.f2;_LL24: s1=_tmp46;i1=_tmp47;goto _LL22;_LL25:;_LL26:{const char*_tmp196;
void*_tmp195;(_tmp195=0,Cyc_Tcutil_terr(e1->loc,((_tmp196="bad constant expression",
_tag_dyneither(_tmp196,sizeof(char),24))),_tag_dyneither(_tmp195,sizeof(void*),0)));}{
struct _tuple11 _tmp197;return(_tmp197.f1=_tmp3E,((_tmp197.f2=1,_tmp197)));}_LL22:;}{
union Cyc_Absyn_Cnst _tmp4B=_tmp41;struct _tuple5 _tmp4C;void*_tmp4D;int _tmp4E;_LL28:
if((_tmp4B.Int_c).tag != 4)goto _LL2A;_tmp4C=(struct _tuple5)(_tmp4B.Int_c).val;
_tmp4D=_tmp4C.f1;_tmp4E=_tmp4C.f2;_LL29: s2=_tmp4D;i2=_tmp4E;goto _LL27;_LL2A:;
_LL2B:{const char*_tmp19A;void*_tmp199;(_tmp199=0,Cyc_Tcutil_terr(e2->loc,((
_tmp19A="bad constant expression",_tag_dyneither(_tmp19A,sizeof(char),24))),
_tag_dyneither(_tmp199,sizeof(void*),0)));}{struct _tuple11 _tmp19B;return(_tmp19B.f1=
_tmp3E,((_tmp19B.f2=1,_tmp19B)));}_LL27:;}{void*_tmp52=p;_LL2D: if((int)_tmp52 != 
3)goto _LL2F;_LL2E: goto _LL30;_LL2F: if((int)_tmp52 != 4)goto _LL31;_LL30: if(i2 == 0){{
const char*_tmp19E;void*_tmp19D;(_tmp19D=0,Cyc_Tcutil_terr(e2->loc,((_tmp19E="division by zero in constant expression",
_tag_dyneither(_tmp19E,sizeof(char),40))),_tag_dyneither(_tmp19D,sizeof(void*),0)));}{
struct _tuple11 _tmp19F;return(_tmp19F.f1=_tmp3E,((_tmp19F.f2=1,_tmp19F)));}}goto
_LL2C;_LL31:;_LL32: goto _LL2C;_LL2C:;}{int has_u_arg=s1 == (void*)1  || s2 == (void*)
1;unsigned int u1=(unsigned int)i1;unsigned int u2=(unsigned int)i2;int i3=0;
unsigned int u3=0;int b3=1;int use_i3=0;int use_u3=0;int use_b3=0;{struct _tuple5
_tmp1A0;struct _tuple5 _tmp57=(_tmp1A0.f1=p,((_tmp1A0.f2=has_u_arg,_tmp1A0)));void*
_tmp58;int _tmp59;void*_tmp5A;int _tmp5B;void*_tmp5C;int _tmp5D;void*_tmp5E;int
_tmp5F;void*_tmp60;int _tmp61;void*_tmp62;int _tmp63;void*_tmp64;int _tmp65;void*
_tmp66;int _tmp67;void*_tmp68;int _tmp69;void*_tmp6A;int _tmp6B;void*_tmp6C;void*
_tmp6D;void*_tmp6E;int _tmp6F;void*_tmp70;int _tmp71;void*_tmp72;int _tmp73;void*
_tmp74;int _tmp75;void*_tmp76;int _tmp77;void*_tmp78;int _tmp79;void*_tmp7A;int
_tmp7B;void*_tmp7C;int _tmp7D;void*_tmp7E;void*_tmp7F;void*_tmp80;void*_tmp81;
void*_tmp82;void*_tmp83;_LL34: _tmp58=_tmp57.f1;if((int)_tmp58 != 0)goto _LL36;
_tmp59=_tmp57.f2;if(_tmp59 != 0)goto _LL36;_LL35: i3=i1 + i2;use_i3=1;goto _LL33;
_LL36: _tmp5A=_tmp57.f1;if((int)_tmp5A != 1)goto _LL38;_tmp5B=_tmp57.f2;if(_tmp5B != 
0)goto _LL38;_LL37: i3=i1 * i2;use_i3=1;goto _LL33;_LL38: _tmp5C=_tmp57.f1;if((int)
_tmp5C != 2)goto _LL3A;_tmp5D=_tmp57.f2;if(_tmp5D != 0)goto _LL3A;_LL39: i3=i1 - i2;
use_i3=1;goto _LL33;_LL3A: _tmp5E=_tmp57.f1;if((int)_tmp5E != 3)goto _LL3C;_tmp5F=
_tmp57.f2;if(_tmp5F != 0)goto _LL3C;_LL3B: i3=i1 / i2;use_i3=1;goto _LL33;_LL3C:
_tmp60=_tmp57.f1;if((int)_tmp60 != 4)goto _LL3E;_tmp61=_tmp57.f2;if(_tmp61 != 0)
goto _LL3E;_LL3D: i3=i1 % i2;use_i3=1;goto _LL33;_LL3E: _tmp62=_tmp57.f1;if((int)
_tmp62 != 0)goto _LL40;_tmp63=_tmp57.f2;if(_tmp63 != 1)goto _LL40;_LL3F: u3=u1 + u2;
use_u3=1;goto _LL33;_LL40: _tmp64=_tmp57.f1;if((int)_tmp64 != 1)goto _LL42;_tmp65=
_tmp57.f2;if(_tmp65 != 1)goto _LL42;_LL41: u3=u1 * u2;use_u3=1;goto _LL33;_LL42:
_tmp66=_tmp57.f1;if((int)_tmp66 != 2)goto _LL44;_tmp67=_tmp57.f2;if(_tmp67 != 1)
goto _LL44;_LL43: u3=u1 - u2;use_u3=1;goto _LL33;_LL44: _tmp68=_tmp57.f1;if((int)
_tmp68 != 3)goto _LL46;_tmp69=_tmp57.f2;if(_tmp69 != 1)goto _LL46;_LL45: u3=u1 / u2;
use_u3=1;goto _LL33;_LL46: _tmp6A=_tmp57.f1;if((int)_tmp6A != 4)goto _LL48;_tmp6B=
_tmp57.f2;if(_tmp6B != 1)goto _LL48;_LL47: u3=u1 % u2;use_u3=1;goto _LL33;_LL48:
_tmp6C=_tmp57.f1;if((int)_tmp6C != 5)goto _LL4A;_LL49: b3=i1 == i2;use_b3=1;goto
_LL33;_LL4A: _tmp6D=_tmp57.f1;if((int)_tmp6D != 6)goto _LL4C;_LL4B: b3=i1 != i2;
use_b3=1;goto _LL33;_LL4C: _tmp6E=_tmp57.f1;if((int)_tmp6E != 7)goto _LL4E;_tmp6F=
_tmp57.f2;if(_tmp6F != 0)goto _LL4E;_LL4D: b3=i1 > i2;use_b3=1;goto _LL33;_LL4E:
_tmp70=_tmp57.f1;if((int)_tmp70 != 8)goto _LL50;_tmp71=_tmp57.f2;if(_tmp71 != 0)
goto _LL50;_LL4F: b3=i1 < i2;use_b3=1;goto _LL33;_LL50: _tmp72=_tmp57.f1;if((int)
_tmp72 != 9)goto _LL52;_tmp73=_tmp57.f2;if(_tmp73 != 0)goto _LL52;_LL51: b3=i1 >= i2;
use_b3=1;goto _LL33;_LL52: _tmp74=_tmp57.f1;if((int)_tmp74 != 10)goto _LL54;_tmp75=
_tmp57.f2;if(_tmp75 != 0)goto _LL54;_LL53: b3=i1 <= i2;use_b3=1;goto _LL33;_LL54:
_tmp76=_tmp57.f1;if((int)_tmp76 != 7)goto _LL56;_tmp77=_tmp57.f2;if(_tmp77 != 1)
goto _LL56;_LL55: b3=u1 > u2;use_b3=1;goto _LL33;_LL56: _tmp78=_tmp57.f1;if((int)
_tmp78 != 8)goto _LL58;_tmp79=_tmp57.f2;if(_tmp79 != 1)goto _LL58;_LL57: b3=u1 < u2;
use_b3=1;goto _LL33;_LL58: _tmp7A=_tmp57.f1;if((int)_tmp7A != 9)goto _LL5A;_tmp7B=
_tmp57.f2;if(_tmp7B != 1)goto _LL5A;_LL59: b3=u1 >= u2;use_b3=1;goto _LL33;_LL5A:
_tmp7C=_tmp57.f1;if((int)_tmp7C != 10)goto _LL5C;_tmp7D=_tmp57.f2;if(_tmp7D != 1)
goto _LL5C;_LL5B: b3=u1 <= u2;use_b3=1;goto _LL33;_LL5C: _tmp7E=_tmp57.f1;if((int)
_tmp7E != 13)goto _LL5E;_LL5D: u3=u1 & u2;use_u3=1;goto _LL33;_LL5E: _tmp7F=_tmp57.f1;
if((int)_tmp7F != 14)goto _LL60;_LL5F: u3=u1 | u2;use_u3=1;goto _LL33;_LL60: _tmp80=
_tmp57.f1;if((int)_tmp80 != 15)goto _LL62;_LL61: u3=u1 ^ u2;use_u3=1;goto _LL33;_LL62:
_tmp81=_tmp57.f1;if((int)_tmp81 != 16)goto _LL64;_LL63: u3=u1 << u2;use_u3=1;goto
_LL33;_LL64: _tmp82=_tmp57.f1;if((int)_tmp82 != 17)goto _LL66;_LL65: u3=u1 >> u2;
use_u3=1;goto _LL33;_LL66: _tmp83=_tmp57.f1;if((int)_tmp83 != 18)goto _LL68;_LL67:{
const char*_tmp1A3;void*_tmp1A2;(_tmp1A2=0,Cyc_Tcutil_terr(e1->loc,((_tmp1A3=">>> NOT IMPLEMENTED",
_tag_dyneither(_tmp1A3,sizeof(char),20))),_tag_dyneither(_tmp1A2,sizeof(void*),0)));}
goto _LL69;_LL68:;_LL69:{const char*_tmp1A6;void*_tmp1A5;(_tmp1A5=0,Cyc_Tcutil_terr(
e1->loc,((_tmp1A6="bad constant expression",_tag_dyneither(_tmp1A6,sizeof(char),
24))),_tag_dyneither(_tmp1A5,sizeof(void*),0)));}{struct _tuple11 _tmp1A7;return(
_tmp1A7.f1=_tmp3E,((_tmp1A7.f2=1,_tmp1A7)));}_LL33:;}if(use_i3){struct _tuple11
_tmp1A8;return(_tmp1A8.f1=Cyc_Absyn_Int_c((void*)0,i3),((_tmp1A8.f2=1,_tmp1A8)));}
if(use_u3){struct _tuple11 _tmp1A9;return(_tmp1A9.f1=Cyc_Absyn_Int_c((void*)1,(int)
u3),((_tmp1A9.f2=1,_tmp1A9)));}if(use_b3){struct _tuple11 _tmp1AA;return(_tmp1AA.f1=
Cyc_Absyn_Int_c((void*)0,b3?1: 0),((_tmp1AA.f2=1,_tmp1AA)));}{const char*_tmp1AD;
void*_tmp1AC;(_tmp1AC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1AD="Evexp::eval_const_binop",_tag_dyneither(_tmp1AD,
sizeof(char),24))),_tag_dyneither(_tmp1AC,sizeof(void*),0)));}}}}}static struct
_tuple11 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e);static struct _tuple11 Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp*e){struct _tuple11 ans;{void*_tmp8E=e->r;union Cyc_Absyn_Cnst
_tmp8F;struct Cyc_Absyn_Exp*_tmp90;struct Cyc_Absyn_Exp*_tmp91;struct Cyc_Absyn_Exp*
_tmp92;struct Cyc_Absyn_Exp*_tmp93;struct Cyc_Absyn_Exp*_tmp94;struct Cyc_Absyn_Exp*
_tmp95;struct Cyc_Absyn_Exp*_tmp96;void*_tmp97;struct Cyc_List_List*_tmp98;void*
_tmp99;void*_tmp9A;struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_Absyn_Enumfield*_tmp9C;
struct Cyc_Absyn_Enumfield*_tmp9D;_LL6B: if(*((int*)_tmp8E)!= 0)goto _LL6D;_tmp8F=((
struct Cyc_Absyn_Const_e_struct*)_tmp8E)->f1;_LL6C: {struct _tuple11 _tmp1AE;return(
_tmp1AE.f1=_tmp8F,((_tmp1AE.f2=1,_tmp1AE)));}_LL6D: if(*((int*)_tmp8E)!= 6)goto
_LL6F;_tmp90=((struct Cyc_Absyn_Conditional_e_struct*)_tmp8E)->f1;_tmp91=((struct
Cyc_Absyn_Conditional_e_struct*)_tmp8E)->f2;_tmp92=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp8E)->f3;_LL6E: {int _tmpA0;int _tmpA1;struct _tuple12 _tmp9F=Cyc_Evexp_eval_const_bool_exp(
_tmp90);_tmpA0=_tmp9F.f1;_tmpA1=_tmp9F.f2;if(!_tmpA1){Cyc_Evexp_eval_const_exp(
_tmp91);Cyc_Evexp_eval_const_exp(_tmp92);{struct _tuple11 _tmp1AF;return(_tmp1AF.f1=
Cyc_Absyn_Int_c((void*)0,0),((_tmp1AF.f2=0,_tmp1AF)));}}ans=_tmpA0?Cyc_Evexp_eval_const_exp(
_tmp91): Cyc_Evexp_eval_const_exp(_tmp92);goto _LL6A;}_LL6F: if(*((int*)_tmp8E)!= 7)
goto _LL71;_tmp93=((struct Cyc_Absyn_And_e_struct*)_tmp8E)->f1;_tmp94=((struct Cyc_Absyn_And_e_struct*)
_tmp8E)->f2;_LL70: {int _tmpA4;int _tmpA5;struct _tuple12 _tmpA3=Cyc_Evexp_eval_const_bool_exp(
_tmp93);_tmpA4=_tmpA3.f1;_tmpA5=_tmpA3.f2;if(!_tmpA5){Cyc_Evexp_eval_const_exp(
_tmp94);{struct _tuple11 _tmp1B0;return(_tmp1B0.f1=Cyc_Absyn_Int_c((void*)0,0),((
_tmp1B0.f2=0,_tmp1B0)));}}if(_tmpA4)ans=Cyc_Evexp_eval_const_exp(_tmp94);else{
struct _tuple11 _tmp1B1;ans=((_tmp1B1.f1=Cyc_Absyn_Int_c((void*)0,0),((_tmp1B1.f2=
1,_tmp1B1))));}goto _LL6A;}_LL71: if(*((int*)_tmp8E)!= 8)goto _LL73;_tmp95=((struct
Cyc_Absyn_Or_e_struct*)_tmp8E)->f1;_tmp96=((struct Cyc_Absyn_Or_e_struct*)_tmp8E)->f2;
_LL72: {int _tmpA9;int _tmpAA;struct _tuple12 _tmpA8=Cyc_Evexp_eval_const_bool_exp(
_tmp95);_tmpA9=_tmpA8.f1;_tmpAA=_tmpA8.f2;if(!_tmpAA){Cyc_Evexp_eval_const_exp(
_tmp96);{struct _tuple11 _tmp1B2;return(_tmp1B2.f1=Cyc_Absyn_Int_c((void*)0,0),((
_tmp1B2.f2=0,_tmp1B2)));}}if(!_tmpA9)ans=Cyc_Evexp_eval_const_exp(_tmp96);else{
struct _tuple11 _tmp1B3;ans=((_tmp1B3.f1=Cyc_Absyn_Int_c((void*)0,1),((_tmp1B3.f2=
1,_tmp1B3))));}goto _LL6A;}_LL73: if(*((int*)_tmp8E)!= 3)goto _LL75;_tmp97=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp8E)->f1;_tmp98=((struct Cyc_Absyn_Primop_e_struct*)
_tmp8E)->f2;_LL74: if(_tmp98 == 0){{const char*_tmp1B6;void*_tmp1B5;(_tmp1B5=0,Cyc_Tcutil_terr(
e->loc,((_tmp1B6="bad static expression (no args to primop)",_tag_dyneither(
_tmp1B6,sizeof(char),42))),_tag_dyneither(_tmp1B5,sizeof(void*),0)));}{struct
_tuple11 _tmp1B7;return(_tmp1B7.f1=Cyc_Absyn_Int_c((void*)0,0),((_tmp1B7.f2=1,
_tmp1B7)));}}if(_tmp98->tl == 0){ans=Cyc_Evexp_eval_const_unprimop(_tmp97,(struct
Cyc_Absyn_Exp*)_tmp98->hd);goto _LL6A;}if(((struct Cyc_List_List*)_check_null(
_tmp98->tl))->tl == 0){ans=Cyc_Evexp_eval_const_binprimop(_tmp97,(struct Cyc_Absyn_Exp*)
_tmp98->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp98->tl))->hd);
goto _LL6A;}{const char*_tmp1BA;void*_tmp1B9;(_tmp1B9=0,Cyc_Tcutil_terr(e->loc,((
_tmp1BA="bad static expression (too many args to primop)",_tag_dyneither(_tmp1BA,
sizeof(char),48))),_tag_dyneither(_tmp1B9,sizeof(void*),0)));}{struct _tuple11
_tmp1BB;return(_tmp1BB.f1=Cyc_Absyn_Int_c((void*)0,0),((_tmp1BB.f2=1,_tmp1BB)));}
_LL75: if(*((int*)_tmp8E)!= 40)goto _LL77;_tmp99=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp8E)->f1;_LL76:{void*_tmpB3=Cyc_Tcutil_compress(_tmp99);struct Cyc_Absyn_Exp*
_tmpB4;_LL86: if(_tmpB3 <= (void*)4)goto _LL88;if(*((int*)_tmpB3)!= 17)goto _LL88;
_tmpB4=((struct Cyc_Absyn_ValueofType_struct*)_tmpB3)->f1;_LL87: return Cyc_Evexp_eval_const_exp(
_tmpB4);_LL88:;_LL89: goto _LL85;_LL85:;}goto _LL78;_LL77: if(*((int*)_tmp8E)!= 18)
goto _LL79;_LL78: goto _LL7A;_LL79: if(*((int*)_tmp8E)!= 19)goto _LL7B;_LL7A: goto
_LL7C;_LL7B: if(*((int*)_tmp8E)!= 20)goto _LL7D;_LL7C:{struct _tuple11 _tmp1BC;ans=((
_tmp1BC.f1=Cyc_Absyn_Int_c((void*)0,0),((_tmp1BC.f2=0,_tmp1BC))));}goto _LL6A;
_LL7D: if(*((int*)_tmp8E)!= 15)goto _LL7F;_tmp9A=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp8E)->f1;_tmp9B=((struct Cyc_Absyn_Cast_e_struct*)_tmp8E)->f2;_LL7E: ans=Cyc_Evexp_eval_const_exp(
_tmp9B);if(ans.f2){struct _tuple13 _tmp1BD;struct _tuple13 _tmpB7=(_tmp1BD.f1=Cyc_Tcutil_compress(
_tmp9A),((_tmp1BD.f2=ans.f1,_tmp1BD)));void*_tmpB8;void*_tmpB9;void*_tmpBA;union
Cyc_Absyn_Cnst _tmpBB;struct _tuple3 _tmpBC;void*_tmpBD;char _tmpBE;void*_tmpBF;void*
_tmpC0;void*_tmpC1;union Cyc_Absyn_Cnst _tmpC2;struct _tuple4 _tmpC3;void*_tmpC4;
short _tmpC5;void*_tmpC6;void*_tmpC7;void*_tmpC8;union Cyc_Absyn_Cnst _tmpC9;struct
_tuple5 _tmpCA;void*_tmpCB;int _tmpCC;void*_tmpCD;void*_tmpCE;union Cyc_Absyn_Cnst
_tmpCF;struct _tuple3 _tmpD0;void*_tmpD1;char _tmpD2;void*_tmpD3;void*_tmpD4;union
Cyc_Absyn_Cnst _tmpD5;struct _tuple4 _tmpD6;void*_tmpD7;short _tmpD8;void*_tmpD9;
void*_tmpDA;union Cyc_Absyn_Cnst _tmpDB;struct _tuple5 _tmpDC;void*_tmpDD;int _tmpDE;
_LL8B: _tmpB8=_tmpB7.f1;if(_tmpB8 <= (void*)4)goto _LL8D;if(*((int*)_tmpB8)!= 5)
goto _LL8D;_tmpB9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB8)->f1;_tmpBA=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmpB8)->f2;_tmpBB=_tmpB7.f2;if((_tmpBB.Char_c).tag
!= 2)goto _LL8D;_tmpBC=(struct _tuple3)(_tmpBB.Char_c).val;_tmpBD=_tmpBC.f1;_tmpBE=
_tmpBC.f2;_LL8C: _tmpC0=_tmpB9;_tmpC1=_tmpBA;_tmpC4=_tmpBD;_tmpC5=(short)_tmpBE;
goto _LL8E;_LL8D: _tmpBF=_tmpB7.f1;if(_tmpBF <= (void*)4)goto _LL8F;if(*((int*)
_tmpBF)!= 5)goto _LL8F;_tmpC0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBF)->f1;
_tmpC1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBF)->f2;_tmpC2=_tmpB7.f2;if((
_tmpC2.Short_c).tag != 3)goto _LL8F;_tmpC3=(struct _tuple4)(_tmpC2.Short_c).val;
_tmpC4=_tmpC3.f1;_tmpC5=_tmpC3.f2;_LL8E: _tmpC7=_tmpC0;_tmpC8=_tmpC1;_tmpCB=
_tmpC4;_tmpCC=(int)_tmpC5;goto _LL90;_LL8F: _tmpC6=_tmpB7.f1;if(_tmpC6 <= (void*)4)
goto _LL91;if(*((int*)_tmpC6)!= 5)goto _LL91;_tmpC7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpC6)->f1;_tmpC8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpC6)->f2;_tmpC9=
_tmpB7.f2;if((_tmpC9.Int_c).tag != 4)goto _LL91;_tmpCA=(struct _tuple5)(_tmpC9.Int_c).val;
_tmpCB=_tmpCA.f1;_tmpCC=_tmpCA.f2;_LL90: if(_tmpC7 != _tmpCB)ans.f1=Cyc_Absyn_Int_c(
_tmpC7,_tmpCC);goto _LL8A;_LL91: _tmpCD=_tmpB7.f1;if(_tmpCD <= (void*)4)goto _LL93;
if(*((int*)_tmpCD)!= 18)goto _LL93;_tmpCE=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmpCD)->f1;_tmpCF=_tmpB7.f2;if((_tmpCF.Char_c).tag != 2)goto _LL93;_tmpD0=(struct
_tuple3)(_tmpCF.Char_c).val;_tmpD1=_tmpD0.f1;_tmpD2=_tmpD0.f2;_LL92: _tmpD4=
_tmpCE;_tmpD7=_tmpD1;_tmpD8=(short)_tmpD2;goto _LL94;_LL93: _tmpD3=_tmpB7.f1;if(
_tmpD3 <= (void*)4)goto _LL95;if(*((int*)_tmpD3)!= 18)goto _LL95;_tmpD4=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmpD3)->f1;_tmpD5=_tmpB7.f2;if((_tmpD5.Short_c).tag
!= 3)goto _LL95;_tmpD6=(struct _tuple4)(_tmpD5.Short_c).val;_tmpD7=_tmpD6.f1;
_tmpD8=_tmpD6.f2;_LL94: _tmpDA=_tmpD4;_tmpDD=_tmpD7;_tmpDE=(int)_tmpD8;goto _LL96;
_LL95: _tmpD9=_tmpB7.f1;if(_tmpD9 <= (void*)4)goto _LL97;if(*((int*)_tmpD9)!= 18)
goto _LL97;_tmpDA=(void*)((struct Cyc_Absyn_TagType_struct*)_tmpD9)->f1;_tmpDB=
_tmpB7.f2;if((_tmpDB.Int_c).tag != 4)goto _LL97;_tmpDC=(struct _tuple5)(_tmpDB.Int_c).val;
_tmpDD=_tmpDC.f1;_tmpDE=_tmpDC.f2;_LL96: if(_tmpDE < 0){const char*_tmp1C0;void*
_tmp1BF;(_tmp1BF=0,Cyc_Tcutil_terr(e->loc,((_tmp1C0="cannot cast negative number to a tag type",
_tag_dyneither(_tmp1C0,sizeof(char),42))),_tag_dyneither(_tmp1BF,sizeof(void*),0)));}{
struct Cyc_Absyn_ValueofType_struct _tmp1C3;struct Cyc_Absyn_ValueofType_struct*
_tmp1C2;Cyc_Tcutil_unify(_tmpDA,(void*)((_tmp1C2=_cycalloc(sizeof(*_tmp1C2)),((
_tmp1C2[0]=((_tmp1C3.tag=17,((_tmp1C3.f1=Cyc_Absyn_const_exp(ans.f1,0),_tmp1C3)))),
_tmp1C2)))));}ans.f1=Cyc_Absyn_Int_c((void*)1,_tmpDE);goto _LL8A;_LL97:;_LL98:{
const char*_tmp1C7;void*_tmp1C6[1];struct Cyc_String_pa_struct _tmp1C5;(_tmp1C5.tag=
0,((_tmp1C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp9A)),((_tmp1C6[0]=& _tmp1C5,Cyc_Tcutil_terr(e->loc,((_tmp1C7="cannot cast to %s",
_tag_dyneither(_tmp1C7,sizeof(char),18))),_tag_dyneither(_tmp1C6,sizeof(void*),1)))))));}
goto _LL8A;_LL8A:;}goto _LL6A;_LL7F: if(*((int*)_tmp8E)!= 34)goto _LL81;_tmp9C=((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp8E)->f3;_LL80: _tmp9D=_tmp9C;goto _LL82;
_LL81: if(*((int*)_tmp8E)!= 33)goto _LL83;_tmp9D=((struct Cyc_Absyn_Enum_e_struct*)
_tmp8E)->f3;_LL82: ans=Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)_check_null(((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp9D))->tag));goto _LL6A;_LL83:;_LL84:{
const char*_tmp1CB;void*_tmp1CA[1];struct Cyc_String_pa_struct _tmp1C9;(_tmp1C9.tag=
0,((_tmp1C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp1CA[0]=& _tmp1C9,Cyc_Tcutil_terr(e->loc,((_tmp1CB="bad static expression %s",
_tag_dyneither(_tmp1CB,sizeof(char),25))),_tag_dyneither(_tmp1CA,sizeof(void*),1)))))));}{
struct _tuple11 _tmp1CC;return(_tmp1CC.f1=Cyc_Absyn_Int_c((void*)0,0),((_tmp1CC.f2=
1,_tmp1CC)));}_LL6A:;}if(ans.f2){struct Cyc_Absyn_Const_e_struct _tmp1CF;struct Cyc_Absyn_Const_e_struct*
_tmp1CE;e->r=(void*)((_tmp1CE=_cycalloc(sizeof(*_tmp1CE)),((_tmp1CE[0]=((_tmp1CF.tag=
0,((_tmp1CF.f1=ans.f1,_tmp1CF)))),_tmp1CE))));}return ans;}int Cyc_Evexp_c_can_eval(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e){void*
_tmpEC=e->r;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Exp*
_tmpEF;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*
_tmpF2;struct Cyc_Absyn_Exp*_tmpF3;void*_tmpF4;struct Cyc_List_List*_tmpF5;struct
Cyc_Absyn_Exp*_tmpF6;_LL9A: if(*((int*)_tmpEC)!= 34)goto _LL9C;_LL9B: goto _LL9D;
_LL9C: if(*((int*)_tmpEC)!= 33)goto _LL9E;_LL9D: goto _LL9F;_LL9E: if(*((int*)_tmpEC)
!= 18)goto _LLA0;_LL9F: goto _LLA1;_LLA0: if(*((int*)_tmpEC)!= 19)goto _LLA2;_LLA1:
goto _LLA3;_LLA2: if(*((int*)_tmpEC)!= 20)goto _LLA4;_LLA3: goto _LLA5;_LLA4: if(*((
int*)_tmpEC)!= 0)goto _LLA6;_LLA5: return 1;_LLA6: if(*((int*)_tmpEC)!= 6)goto _LLA8;
_tmpED=((struct Cyc_Absyn_Conditional_e_struct*)_tmpEC)->f1;_tmpEE=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpEC)->f2;_tmpEF=((struct Cyc_Absyn_Conditional_e_struct*)_tmpEC)->f3;_LLA7:
return(Cyc_Evexp_c_can_eval(_tmpED) && Cyc_Evexp_c_can_eval(_tmpEE)) && Cyc_Evexp_c_can_eval(
_tmpEF);_LLA8: if(*((int*)_tmpEC)!= 7)goto _LLAA;_tmpF0=((struct Cyc_Absyn_And_e_struct*)
_tmpEC)->f1;_tmpF1=((struct Cyc_Absyn_And_e_struct*)_tmpEC)->f2;_LLA9: _tmpF2=
_tmpF0;_tmpF3=_tmpF1;goto _LLAB;_LLAA: if(*((int*)_tmpEC)!= 8)goto _LLAC;_tmpF2=((
struct Cyc_Absyn_Or_e_struct*)_tmpEC)->f1;_tmpF3=((struct Cyc_Absyn_Or_e_struct*)
_tmpEC)->f2;_LLAB: return Cyc_Evexp_c_can_eval(_tmpF2) && Cyc_Evexp_c_can_eval(
_tmpF3);_LLAC: if(*((int*)_tmpEC)!= 3)goto _LLAE;_tmpF4=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmpEC)->f1;_tmpF5=((struct Cyc_Absyn_Primop_e_struct*)_tmpEC)->f2;_LLAD: for(0;
_tmpF5 != 0;_tmpF5=_tmpF5->tl){if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)
_tmpF5->hd))return 0;}return 1;_LLAE: if(*((int*)_tmpEC)!= 40)goto _LLB0;_LLAF:
return 0;_LLB0: if(*((int*)_tmpEC)!= 15)goto _LLB2;_tmpF6=((struct Cyc_Absyn_Cast_e_struct*)
_tmpEC)->f2;_LLB1: return Cyc_Evexp_c_can_eval(_tmpF6);_LLB2:;_LLB3: return 0;_LL99:;}
static int Cyc_Evexp_const_exp_case_number(struct Cyc_Absyn_Exp*e);static int Cyc_Evexp_const_exp_case_number(
struct Cyc_Absyn_Exp*e){void*_tmpF7=e->r;_LLB5: if(*((int*)_tmpF7)!= 0)goto _LLB7;
_LLB6: return 1;_LLB7: if(*((int*)_tmpF7)!= 6)goto _LLB9;_LLB8: return 2;_LLB9: if(*((
int*)_tmpF7)!= 3)goto _LLBB;_LLBA: return 3;_LLBB: if(*((int*)_tmpF7)!= 18)goto _LLBD;
_LLBC: goto _LLBE;_LLBD: if(*((int*)_tmpF7)!= 19)goto _LLBF;_LLBE: return 4;_LLBF: if(*((
int*)_tmpF7)!= 20)goto _LLC1;_LLC0: return 5;_LLC1: if(*((int*)_tmpF7)!= 15)goto
_LLC3;_LLC2: return 6;_LLC3: if(*((int*)_tmpF7)!= 7)goto _LLC5;_LLC4: return 7;_LLC5:
if(*((int*)_tmpF7)!= 8)goto _LLC7;_LLC6: return 8;_LLC7: if(*((int*)_tmpF7)!= 40)
goto _LLC9;_LLC8: return 9;_LLC9:;_LLCA:{const char*_tmp1D3;void*_tmp1D2[1];struct
Cyc_String_pa_struct _tmp1D1;(_tmp1D1.tag=0,((_tmp1D1.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp1D2[0]=& _tmp1D1,Cyc_Tcutil_terr(
e->loc,((_tmp1D3="bad static expression %s",_tag_dyneither(_tmp1D3,sizeof(char),
25))),_tag_dyneither(_tmp1D2,sizeof(void*),1)))))));}return 0;_LLB4:;}int Cyc_Evexp_const_exp_cmp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){unsigned int _tmpFC;int _tmpFD;struct
_tuple10 _tmpFB=Cyc_Evexp_eval_const_uint_exp(e1);_tmpFC=_tmpFB.f1;_tmpFD=_tmpFB.f2;{
unsigned int _tmpFF;int _tmp100;struct _tuple10 _tmpFE=Cyc_Evexp_eval_const_uint_exp(
e2);_tmpFF=_tmpFE.f1;_tmp100=_tmpFE.f2;if(_tmpFD  && _tmp100)return(int)(_tmpFC - 
_tmpFF);{int _tmp101=Cyc_Evexp_const_exp_case_number(e1);int _tmp102=Cyc_Evexp_const_exp_case_number(
e2);if(_tmp101 != _tmp102)return _tmp101 - _tmp102;{struct _tuple9 _tmp1D4;struct
_tuple9 _tmp104=(_tmp1D4.f1=e1->r,((_tmp1D4.f2=e2->r,_tmp1D4)));void*_tmp105;
struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*
_tmp108;void*_tmp109;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp10B;
struct Cyc_Absyn_Exp*_tmp10C;void*_tmp10D;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*
_tmp10F;void*_tmp110;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp112;
void*_tmp113;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp115;void*
_tmp116;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp118;void*_tmp119;
void*_tmp11A;struct Cyc_List_List*_tmp11B;void*_tmp11C;void*_tmp11D;struct Cyc_List_List*
_tmp11E;void*_tmp11F;void*_tmp120;void*_tmp121;void*_tmp122;void*_tmp123;void*
_tmp124;void*_tmp125;struct Cyc_Absyn_Exp*_tmp126;void*_tmp127;struct Cyc_Absyn_Exp*
_tmp128;void*_tmp129;void*_tmp12A;void*_tmp12B;struct Cyc_Absyn_Exp*_tmp12C;void*
_tmp12D;struct Cyc_Absyn_Exp*_tmp12E;void*_tmp12F;void*_tmp130;void*_tmp131;void*
_tmp132;void*_tmp133;void*_tmp134;void*_tmp135;void*_tmp136;struct Cyc_Absyn_Exp*
_tmp137;void*_tmp138;void*_tmp139;struct Cyc_Absyn_Exp*_tmp13A;void*_tmp13B;void*
_tmp13C;void*_tmp13D;void*_tmp13E;_LLCC: _tmp105=_tmp104.f1;if(*((int*)_tmp105)!= 
6)goto _LLCE;_tmp106=((struct Cyc_Absyn_Conditional_e_struct*)_tmp105)->f1;_tmp107=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp105)->f2;_tmp108=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp105)->f3;_tmp109=_tmp104.f2;if(*((int*)_tmp109)!= 6)goto _LLCE;_tmp10A=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp109)->f1;_tmp10B=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp109)->f2;_tmp10C=((struct Cyc_Absyn_Conditional_e_struct*)_tmp109)->f3;_LLCD: {
int _tmp13F=Cyc_Evexp_const_exp_cmp(_tmp108,_tmp10C);if(_tmp13F != 0)return _tmp13F;
_tmp10E=_tmp106;_tmp10F=_tmp107;_tmp111=_tmp10A;_tmp112=_tmp10B;goto _LLCF;}_LLCE:
_tmp10D=_tmp104.f1;if(*((int*)_tmp10D)!= 7)goto _LLD0;_tmp10E=((struct Cyc_Absyn_And_e_struct*)
_tmp10D)->f1;_tmp10F=((struct Cyc_Absyn_And_e_struct*)_tmp10D)->f2;_tmp110=
_tmp104.f2;if(*((int*)_tmp110)!= 7)goto _LLD0;_tmp111=((struct Cyc_Absyn_And_e_struct*)
_tmp110)->f1;_tmp112=((struct Cyc_Absyn_And_e_struct*)_tmp110)->f2;_LLCF: _tmp114=
_tmp10E;_tmp115=_tmp10F;_tmp117=_tmp111;_tmp118=_tmp112;goto _LLD1;_LLD0: _tmp113=
_tmp104.f1;if(*((int*)_tmp113)!= 8)goto _LLD2;_tmp114=((struct Cyc_Absyn_Or_e_struct*)
_tmp113)->f1;_tmp115=((struct Cyc_Absyn_Or_e_struct*)_tmp113)->f2;_tmp116=_tmp104.f2;
if(*((int*)_tmp116)!= 8)goto _LLD2;_tmp117=((struct Cyc_Absyn_Or_e_struct*)_tmp116)->f1;
_tmp118=((struct Cyc_Absyn_Or_e_struct*)_tmp116)->f2;_LLD1: {int _tmp140=Cyc_Evexp_const_exp_cmp(
_tmp114,_tmp117);if(_tmp140 != 0)return _tmp140;return Cyc_Evexp_const_exp_cmp(
_tmp115,_tmp118);}_LLD2: _tmp119=_tmp104.f1;if(*((int*)_tmp119)!= 3)goto _LLD4;
_tmp11A=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp119)->f1;_tmp11B=((struct
Cyc_Absyn_Primop_e_struct*)_tmp119)->f2;_tmp11C=_tmp104.f2;if(*((int*)_tmp11C)!= 
3)goto _LLD4;_tmp11D=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp11C)->f1;
_tmp11E=((struct Cyc_Absyn_Primop_e_struct*)_tmp11C)->f2;_LLD3: {int _tmp141=(int)
_tmp11A - (int)_tmp11D;if(_tmp141 != 0)return _tmp141;for(0;_tmp11B != 0  && _tmp11E
!= 0;(_tmp11B=_tmp11B->tl,_tmp11E=_tmp11E->tl)){int _tmp142=Cyc_Evexp_const_exp_cmp((
struct Cyc_Absyn_Exp*)_tmp11B->hd,(struct Cyc_Absyn_Exp*)_tmp11E->hd);if(_tmp142 != 
0)return _tmp142;}return 0;}_LLD4: _tmp11F=_tmp104.f1;if(*((int*)_tmp11F)!= 18)goto
_LLD6;_tmp120=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp11F)->f1;_tmp121=
_tmp104.f2;if(*((int*)_tmp121)!= 18)goto _LLD6;_tmp122=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp121)->f1;_LLD5: return Cyc_Tcutil_typecmp(_tmp120,_tmp122);_LLD6: _tmp123=
_tmp104.f1;if(*((int*)_tmp123)!= 18)goto _LLD8;_tmp124=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp123)->f1;_tmp125=_tmp104.f2;if(*((int*)_tmp125)!= 19)goto _LLD8;_tmp126=((
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp125)->f1;_LLD7: if(_tmp126->topt == 0){
const char*_tmp1D7;void*_tmp1D6;(_tmp1D6=0,Cyc_Tcutil_terr(e2->loc,((_tmp1D7="cannot handle sizeof(exp) here -- use sizeof(type)",
_tag_dyneither(_tmp1D7,sizeof(char),51))),_tag_dyneither(_tmp1D6,sizeof(void*),0)));}
return Cyc_Tcutil_typecmp(_tmp124,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp126->topt))->v);_LLD8: _tmp127=_tmp104.f1;if(*((int*)_tmp127)!= 19)goto _LLDA;
_tmp128=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp127)->f1;_tmp129=_tmp104.f2;
if(*((int*)_tmp129)!= 18)goto _LLDA;_tmp12A=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp129)->f1;_LLD9: if(_tmp128->topt == 0){const char*_tmp1DA;void*_tmp1D9;(_tmp1D9=
0,Cyc_Tcutil_terr(e1->loc,((_tmp1DA="cannot handle sizeof(exp) here -- use sizeof(type)",
_tag_dyneither(_tmp1DA,sizeof(char),51))),_tag_dyneither(_tmp1D9,sizeof(void*),0)));}
return Cyc_Tcutil_typecmp((void*)((struct Cyc_Core_Opt*)_check_null(_tmp128->topt))->v,
_tmp12A);_LLDA: _tmp12B=_tmp104.f1;if(*((int*)_tmp12B)!= 19)goto _LLDC;_tmp12C=((
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp12B)->f1;_tmp12D=_tmp104.f2;if(*((int*)
_tmp12D)!= 19)goto _LLDC;_tmp12E=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp12D)->f1;
_LLDB: if(_tmp12C->topt == 0){const char*_tmp1DD;void*_tmp1DC;(_tmp1DC=0,Cyc_Tcutil_terr(
e1->loc,((_tmp1DD="cannot handle sizeof(exp) here -- use sizeof(type)",
_tag_dyneither(_tmp1DD,sizeof(char),51))),_tag_dyneither(_tmp1DC,sizeof(void*),0)));}
if(_tmp12E->topt == 0){const char*_tmp1E0;void*_tmp1DF;(_tmp1DF=0,Cyc_Tcutil_terr(
e2->loc,((_tmp1E0="cannot handle sizeof(exp) here -- use sizeof(type)",
_tag_dyneither(_tmp1E0,sizeof(char),51))),_tag_dyneither(_tmp1DF,sizeof(void*),0)));}
return Cyc_Tcutil_typecmp((void*)((struct Cyc_Core_Opt*)_check_null(_tmp12C->topt))->v,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp12E->topt))->v);_LLDC: _tmp12F=
_tmp104.f1;if(*((int*)_tmp12F)!= 20)goto _LLDE;_tmp130=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp12F)->f1;_tmp131=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp12F)->f2;
_tmp132=_tmp104.f2;if(*((int*)_tmp132)!= 20)goto _LLDE;_tmp133=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp132)->f1;_tmp134=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp132)->f2;
_LLDD: {int _tmp14B=Cyc_Tcutil_typecmp(_tmp130,_tmp133);if(_tmp14B != 0)return
_tmp14B;{struct _tuple9 _tmp1E1;struct _tuple9 _tmp14D=(_tmp1E1.f1=_tmp131,((_tmp1E1.f2=
_tmp134,_tmp1E1)));void*_tmp14E;void*_tmp14F;void*_tmp150;void*_tmp151;void*
_tmp152;struct _dyneither_ptr*_tmp153;void*_tmp154;struct _dyneither_ptr*_tmp155;
void*_tmp156;unsigned int _tmp157;void*_tmp158;unsigned int _tmp159;_LLE5: _tmp14E=
_tmp14D.f1;if(*((int*)_tmp14E)!= 0)goto _LLE7;_tmp14F=_tmp14D.f2;if(*((int*)
_tmp14F)!= 1)goto _LLE7;_LLE6: return - 1;_LLE7: _tmp150=_tmp14D.f1;if(*((int*)
_tmp150)!= 1)goto _LLE9;_tmp151=_tmp14D.f2;if(*((int*)_tmp151)!= 0)goto _LLE9;
_LLE8: return 1;_LLE9: _tmp152=_tmp14D.f1;if(*((int*)_tmp152)!= 0)goto _LLEB;_tmp153=((
struct Cyc_Absyn_StructField_struct*)_tmp152)->f1;_tmp154=_tmp14D.f2;if(*((int*)
_tmp154)!= 0)goto _LLEB;_tmp155=((struct Cyc_Absyn_StructField_struct*)_tmp154)->f1;
_LLEA: return Cyc_strptrcmp(_tmp153,_tmp155);_LLEB: _tmp156=_tmp14D.f1;if(*((int*)
_tmp156)!= 1)goto _LLE4;_tmp157=((struct Cyc_Absyn_TupleIndex_struct*)_tmp156)->f1;
_tmp158=_tmp14D.f2;if(*((int*)_tmp158)!= 1)goto _LLE4;_tmp159=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp158)->f1;_LLEC: return(int)(_tmp157 - _tmp159);_LLE4:;}}_LLDE: _tmp135=_tmp104.f1;
if(*((int*)_tmp135)!= 15)goto _LLE0;_tmp136=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp135)->f1;_tmp137=((struct Cyc_Absyn_Cast_e_struct*)_tmp135)->f2;_tmp138=
_tmp104.f2;if(*((int*)_tmp138)!= 15)goto _LLE0;_tmp139=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp138)->f1;_tmp13A=((struct Cyc_Absyn_Cast_e_struct*)_tmp138)->f2;_LLDF: {int
_tmp15A=Cyc_Tcutil_typecmp(_tmp136,_tmp139);if(_tmp15A != 0)return _tmp15A;return
Cyc_Evexp_const_exp_cmp(_tmp137,_tmp13A);}_LLE0: _tmp13B=_tmp104.f1;if(*((int*)
_tmp13B)!= 40)goto _LLE2;_tmp13C=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp13B)->f1;_tmp13D=_tmp104.f2;if(*((int*)_tmp13D)!= 40)goto _LLE2;_tmp13E=(void*)((
struct Cyc_Absyn_Valueof_e_struct*)_tmp13D)->f1;_LLE1: if(Cyc_Tcutil_unify(_tmp13C,
_tmp13E))return 0;return Cyc_Tcutil_typecmp(_tmp13C,_tmp13E);_LLE2:;_LLE3: {const
char*_tmp1E4;void*_tmp1E3;(_tmp1E3=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1E4="Evexp::const_exp_cmp, unexpected case",
_tag_dyneither(_tmp1E4,sizeof(char),38))),_tag_dyneither(_tmp1E3,sizeof(void*),0)));}
_LLCB:;}}}}int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return Cyc_Evexp_const_exp_cmp(e1,e2)== 0;}int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){unsigned int _tmp15E;int _tmp15F;struct _tuple10 _tmp15D=Cyc_Evexp_eval_const_uint_exp(
e1);_tmp15E=_tmp15D.f1;_tmp15F=_tmp15D.f2;{unsigned int _tmp161;int _tmp162;struct
_tuple10 _tmp160=Cyc_Evexp_eval_const_uint_exp(e2);_tmp161=_tmp160.f1;_tmp162=
_tmp160.f2;if(_tmp15F  && _tmp162)return _tmp15E <= _tmp161;return Cyc_Evexp_same_const_exp(
e1,e2);}}struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;};int Cyc_Evexp_okay_szofarg(
void*t);int Cyc_Evexp_okay_szofarg(void*t){void*_tmp163=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_Tvar*_tmp164;struct Cyc_Core_Opt*_tmp165;struct Cyc_Core_Opt
_tmp166;void*_tmp167;struct Cyc_Absyn_DatatypeFieldInfo _tmp168;union Cyc_Absyn_DatatypeFieldInfoU
_tmp169;struct _tuple1 _tmp16A;struct Cyc_Absyn_Datatypefield*_tmp16B;struct Cyc_List_List*
_tmp16C;struct Cyc_Absyn_ArrayInfo _tmp16D;void*_tmp16E;struct Cyc_Absyn_Exp*
_tmp16F;struct Cyc_Absyn_AggrInfo _tmp170;union Cyc_Absyn_AggrInfoU _tmp171;struct
Cyc_List_List*_tmp172;struct Cyc_Absyn_Typedefdecl*_tmp173;_LLEE: if((int)_tmp163
!= 0)goto _LLF0;_LLEF: return 0;_LLF0: if(_tmp163 <= (void*)4)goto _LL104;if(*((int*)
_tmp163)!= 1)goto _LLF2;_tmp164=((struct Cyc_Absyn_VarType_struct*)_tmp163)->f1;
_LLF1: {void*_tmp174=Cyc_Tcutil_tvar_kind(_tmp164);_LL127: if((int)_tmp174 != 2)
goto _LL129;_LL128: return 1;_LL129:;_LL12A: return 0;_LL126:;}_LLF2: if(*((int*)
_tmp163)!= 0)goto _LLF4;_tmp165=((struct Cyc_Absyn_Evar_struct*)_tmp163)->f1;if(
_tmp165 == 0)goto _LLF4;_tmp166=*_tmp165;_tmp167=(void*)_tmp166.v;if((int)_tmp167
!= 2)goto _LLF4;_LLF3: return 1;_LLF4: if(*((int*)_tmp163)!= 0)goto _LLF6;_LLF5:
return 0;_LLF6: if(*((int*)_tmp163)!= 2)goto _LLF8;_LLF7: return 1;_LLF8: if(*((int*)
_tmp163)!= 3)goto _LLFA;_tmp168=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp163)->f1;_tmp169=_tmp168.field_info;if((_tmp169.KnownDatatypefield).tag != 2)
goto _LLFA;_tmp16A=(struct _tuple1)(_tmp169.KnownDatatypefield).val;_tmp16B=
_tmp16A.f2;_LLF9: _tmp16C=_tmp16B->typs;goto _LLFB;_LLFA: if(*((int*)_tmp163)!= 9)
goto _LLFC;_tmp16C=((struct Cyc_Absyn_TupleType_struct*)_tmp163)->f1;_LLFB: for(0;
_tmp16C != 0;_tmp16C=_tmp16C->tl){if(!Cyc_Evexp_okay_szofarg((*((struct _tuple14*)
_tmp16C->hd)).f2))return 0;}return 1;_LLFC: if(*((int*)_tmp163)!= 3)goto _LLFE;_LLFD:
return 0;_LLFE: if(*((int*)_tmp163)!= 4)goto _LL100;_LLFF: return 1;_LL100: if(*((int*)
_tmp163)!= 18)goto _LL102;_LL101: goto _LL103;_LL102: if(*((int*)_tmp163)!= 5)goto
_LL104;_LL103: goto _LL105;_LL104: if((int)_tmp163 != 1)goto _LL106;_LL105: goto _LL107;
_LL106: if(_tmp163 <= (void*)4)goto _LL118;if(*((int*)_tmp163)!= 6)goto _LL108;
_LL107: return 1;_LL108: if(*((int*)_tmp163)!= 7)goto _LL10A;_tmp16D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp163)->f1;_tmp16E=_tmp16D.elt_type;_tmp16F=_tmp16D.num_elts;_LL109: return
_tmp16F != 0;_LL10A: if(*((int*)_tmp163)!= 8)goto _LL10C;_LL10B: return 0;_LL10C: if(*((
int*)_tmp163)!= 14)goto _LL10E;_LL10D: return 1;_LL10E: if(*((int*)_tmp163)!= 15)
goto _LL110;_LL10F: return 1;_LL110: if(*((int*)_tmp163)!= 10)goto _LL112;_tmp170=((
struct Cyc_Absyn_AggrType_struct*)_tmp163)->f1;_tmp171=_tmp170.aggr_info;_LL111: {
struct Cyc_Absyn_Aggrdecl*ad;{union Cyc_Absyn_AggrInfoU _tmp175=_tmp171;struct
_tuple2 _tmp176;struct Cyc_Absyn_Aggrdecl**_tmp177;struct Cyc_Absyn_Aggrdecl*
_tmp178;_LL12C: if((_tmp175.UnknownAggr).tag != 1)goto _LL12E;_tmp176=(struct
_tuple2)(_tmp175.UnknownAggr).val;_LL12D: {const char*_tmp1E7;void*_tmp1E6;(
_tmp1E6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1E7="szof on unchecked StructType",_tag_dyneither(_tmp1E7,sizeof(char),29))),
_tag_dyneither(_tmp1E6,sizeof(void*),0)));}_LL12E: if((_tmp175.KnownAggr).tag != 2)
goto _LL12B;_tmp177=(struct Cyc_Absyn_Aggrdecl**)(_tmp175.KnownAggr).val;_tmp178=*
_tmp177;_LL12F: ad=_tmp178;_LL12B:;}if(ad->impl == 0)return 0;_tmp172=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;goto _LL113;}_LL112: if(*((int*)_tmp163)!= 11)goto
_LL114;_tmp172=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp163)->f2;_LL113: for(0;
_tmp172 != 0;_tmp172=_tmp172->tl){if(!Cyc_Evexp_okay_szofarg(((struct Cyc_Absyn_Aggrfield*)
_tmp172->hd)->type))return 0;}return 1;_LL114: if(*((int*)_tmp163)!= 12)goto _LL116;
_LL115: goto _LL117;_LL116: if(*((int*)_tmp163)!= 13)goto _LL118;_LL117: return 1;
_LL118: if((int)_tmp163 != 2)goto _LL11A;_LL119: goto _LL11B;_LL11A: if((int)_tmp163 != 
3)goto _LL11C;_LL11B: goto _LL11D;_LL11C: if(_tmp163 <= (void*)4)goto _LL11E;if(*((int*)
_tmp163)!= 20)goto _LL11E;_LL11D: goto _LL11F;_LL11E: if(_tmp163 <= (void*)4)goto
_LL120;if(*((int*)_tmp163)!= 19)goto _LL120;_LL11F: goto _LL121;_LL120: if(_tmp163 <= (
void*)4)goto _LL122;if(*((int*)_tmp163)!= 17)goto _LL122;_LL121: goto _LL123;_LL122:
if(_tmp163 <= (void*)4)goto _LL124;if(*((int*)_tmp163)!= 21)goto _LL124;_LL123:
return 0;_LL124: if(_tmp163 <= (void*)4)goto _LLED;if(*((int*)_tmp163)!= 16)goto
_LLED;_tmp173=((struct Cyc_Absyn_TypedefType_struct*)_tmp163)->f3;_LL125: if(
_tmp173 == 0  || _tmp173->kind == 0){struct Cyc_String_pa_struct _tmp1EF;void*_tmp1EE[
1];const char*_tmp1ED;void*_tmp1EC;(_tmp1EC=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp1EF.tag=0,((
_tmp1EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp1EE[0]=& _tmp1EF,Cyc_aprintf(((_tmp1ED="szof typedeftype %s",_tag_dyneither(
_tmp1ED,sizeof(char),20))),_tag_dyneither(_tmp1EE,sizeof(void*),1)))))))),
_tag_dyneither(_tmp1EC,sizeof(void*),0)));}return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp173->kind))->v == (void*)2;_LLED:;}
