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
void*v;};void*Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc___cycFILE;
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
char*tag;struct _dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(struct
Cyc_List_List*x);extern char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];struct
_union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple0{union
Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
loc;};struct _union_Constraint_Eq_constr{int tag;void*val;};struct
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,
struct Cyc_List_List*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;extern struct _tuple0*
Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init);void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*
eff_typ,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(
void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict
d,void*accum);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict
d);struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d);struct
_tuple9{void*f1;void*f2;};struct _tuple9*Cyc_Dict_rchoose(struct _RegionHandle*r,
struct Cyc_Dict_Dict d);struct _tuple9*Cyc_Dict_rchoose(struct _RegionHandle*,struct
Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
struct Cyc_Interface_I;struct Cyc_Interface_I*Cyc_Interface_empty();struct Cyc_Interface_I*
Cyc_Interface_final();struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict
ae);struct _tuple10{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info);struct Cyc_Interface_I*
Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct
_tuple10*info);struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*
li,struct Cyc_List_List*linfo);struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(
struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*
linfo);void Cyc_Interface_print(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);void Cyc_Interface_save(
struct Cyc_Interface_I*,struct Cyc___cycFILE*);struct Cyc_Interface_I*Cyc_Interface_load(
struct Cyc___cycFILE*);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct
_dyneither_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_all_effects: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*);void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int
var_default_init,struct Cyc_List_List*ds);extern char Cyc_Lexing_Error[10];struct
Cyc_Lexing_Error_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{
void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct
_dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int
lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct
_union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;
struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple11{
struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*
f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple11*val;};struct
_union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;void*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*val;};struct
_union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY11{
int tag;struct Cyc_Absyn_Pat*val;};struct _tuple12{struct Cyc_List_List*f1;int f2;};
struct _union_YYSTYPE_YY12{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY13{
int tag;struct Cyc_List_List*val;};struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple13*val;};struct
_union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY16{
int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Absyn_Fndecl*
val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec*val;};struct _tuple14{
struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY20{int
tag;struct _tuple14*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY22{int tag;void*val;};struct _union_YYSTYPE_YY23{int
tag;void*val;};struct _union_YYSTYPE_YY24{int tag;void*val;};struct
_union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY26{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY29{int tag;struct Cyc_Declarator*val;};struct _union_YYSTYPE_YY30{
int tag;struct Cyc_Abstractdeclarator*val;};struct _union_YYSTYPE_YY31{int tag;int
val;};struct _union_YYSTYPE_YY32{int tag;void*val;};struct _union_YYSTYPE_YY33{int
tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct
Cyc_List_List*val;};struct _tuple15{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple15*val;
};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY37{int tag;struct _tuple7*val;};struct _union_YYSTYPE_YY38{int tag;
struct Cyc_List_List*val;};struct _tuple16{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*
f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int
tag;struct _tuple16*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*
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
timestamp;int first_line;int first_column;int last_line;int last_column;};void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*
Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_function_type(void*t);void*Cyc_Tcutil_kind_to_bound(
void*k);extern char Cyc_Tcdecl_Incompatible[17];struct Cyc_Tcdecl_Xdatatypefielddecl{
struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};void Cyc_Tcdecl_merr(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(
struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(
struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(
struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);void Cyc_Lex_lex_init();
struct Cyc_Interface_Ienv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict
datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefdecls;
struct Cyc_Dict_Dict vardecls;struct Cyc_Dict_Dict xdatatypefielddecls;};struct Cyc_Interface_I{
struct Cyc_Dict_Dict imports;struct Cyc_Dict_Dict exports;};static struct Cyc_Interface_Ienv*
Cyc_Interface_new_ienv();static struct Cyc_Interface_Ienv*Cyc_Interface_new_ienv(){
struct Cyc_Interface_Ienv*_tmp246;return(_tmp246=_cycalloc(sizeof(*_tmp246)),((
_tmp246->aggrdecls=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp246->datatypedecls=((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp246->enumdecls=((struct Cyc_Dict_Dict(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((
_tmp246->typedefdecls=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp246->vardecls=((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Dict_empty)(Cyc_strptrcmp),((_tmp246->xdatatypefielddecls=((struct Cyc_Dict_Dict(*)(
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),
_tmp246)))))))))))));}static struct Cyc_Interface_Ienv*Cyc_Interface_lazy_new_ienv();
static struct Cyc_Interface_Ienv*Cyc_Interface_lazy_new_ienv(){static struct Cyc_Interface_Ienv*
ie=0;if(ie == 0)ie=(struct Cyc_Interface_Ienv*)Cyc_Interface_new_ienv();return(
struct Cyc_Interface_Ienv*)ie;}static struct Cyc_Interface_Ienv*Cyc_Interface_skel2i_f(
void*ignored);static struct Cyc_Interface_Ienv*Cyc_Interface_skel2i_f(void*ignored){
return Cyc_Interface_new_ienv();}static struct Cyc_Interface_I*Cyc_Interface_skel2i(
struct Cyc_Dict_Dict skel);static struct Cyc_Interface_I*Cyc_Interface_skel2i(struct
Cyc_Dict_Dict skel){struct Cyc_Interface_I*_tmp247;return(_tmp247=_cycalloc(
sizeof(*_tmp247)),((_tmp247->imports=((struct Cyc_Dict_Dict(*)(struct Cyc_Interface_Ienv*(*
f)(void*),struct Cyc_Dict_Dict d))Cyc_Dict_map)(Cyc_Interface_skel2i_f,skel),((
_tmp247->exports=((struct Cyc_Dict_Dict(*)(struct Cyc_Interface_Ienv*(*f)(void*),
struct Cyc_Dict_Dict d))Cyc_Dict_map)(Cyc_Interface_skel2i_f,skel),_tmp247)))));}
struct Cyc_Interface_I*Cyc_Interface_empty();struct Cyc_Interface_I*Cyc_Interface_empty(){
struct Cyc_Interface_I*_tmp248;return(_tmp248=_cycalloc(sizeof(*_tmp248)),((
_tmp248->imports=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct
Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),((_tmp248->exports=((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(
Cyc_Absyn_varlist_cmp),_tmp248)))));}struct Cyc_Interface_I*Cyc_Interface_final();
struct Cyc_Interface_I*Cyc_Interface_final(){struct Cyc_Interface_I*_tmp3=Cyc_Interface_empty();
struct Cyc_Interface_Ienv*_tmp4=Cyc_Interface_new_ienv();struct Cyc_Absyn_Datatypedecl*
_tmp249;struct Cyc_Absyn_Datatypedecl*_tmp5=(_tmp249=_cycalloc(sizeof(*_tmp249)),((
_tmp249[0]=*Cyc_Absyn_exn_tud,_tmp249)));_tmp5->sc=(void*)2;_tmp4->datatypedecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*
v))Cyc_Dict_insert)(_tmp4->datatypedecls,(*Cyc_Absyn_exn_name).f2,_tmp5);{struct
Cyc_List_List*_tmp6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;
for(0;_tmp6 != 0;_tmp6=_tmp6->tl){struct Cyc_Absyn_Datatypefield*_tmp24A;struct Cyc_Absyn_Datatypefield*
_tmp7=(_tmp24A=_cycalloc(sizeof(*_tmp24A)),((_tmp24A[0]=*((struct Cyc_Absyn_Datatypefield*)
_tmp6->hd),_tmp24A)));_tmp7->sc=(void*)2;{struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp24B;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp8=(_tmp24B=_cycalloc(sizeof(*
_tmp24B)),((_tmp24B->base=_tmp5,((_tmp24B->field=_tmp7,_tmp24B)))));_tmp4->xdatatypefielddecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Tcdecl_Xdatatypefielddecl*
v))Cyc_Dict_insert)(_tmp4->xdatatypefielddecls,(*((struct Cyc_Absyn_Datatypefield*)
_tmp6->hd)->name).f2,_tmp8);}}}_tmp3->exports=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(_tmp3->exports,
0,_tmp4);{struct Cyc_Interface_Ienv*_tmpB=Cyc_Interface_new_ienv();union Cyc_Absyn_Nmspace
_tmp253;struct _dyneither_ptr*_tmp252;const char*_tmp251;struct _tuple0*_tmp250;
struct _tuple0*qmain=(_tmp250=_cycalloc(sizeof(*_tmp250)),((_tmp250->f1=(((
_tmp253.Abs_n).val=0,(((_tmp253.Abs_n).tag=2,_tmp253)))),((_tmp250->f2=((_tmp252=
_cycalloc(sizeof(*_tmp252)),((_tmp252[0]=(struct _dyneither_ptr)((_tmp251="main",
_tag_dyneither(_tmp251,sizeof(char),5))),_tmp252)))),_tmp250)))));struct _tuple7*
_tmp25B;struct Cyc_List_List*_tmp25A;struct _tuple7*_tmp259;struct Cyc_List_List*
_tmp258;struct Cyc_Absyn_Vardecl*_tmpC=Cyc_Absyn_new_vardecl(qmain,Cyc_Absyn_function_typ(
0,0,Cyc_Absyn_sint_typ,((_tmp258=_cycalloc(sizeof(*_tmp258)),((_tmp258->hd=((
_tmp25B=_cycalloc(sizeof(*_tmp25B)),((_tmp25B->f1=0,((_tmp25B->f2=Cyc_Absyn_empty_tqual(
0),((_tmp25B->f3=Cyc_Absyn_uint_typ,_tmp25B)))))))),((_tmp258->tl=((_tmp25A=
_cycalloc(sizeof(*_tmp25A)),((_tmp25A->hd=((_tmp259=_cycalloc(sizeof(*_tmp259)),((
_tmp259->f1=0,((_tmp259->f2=Cyc_Absyn_empty_tqual(0),((_tmp259->f3=Cyc_Absyn_dyneither_typ(
Cyc_Absyn_dyneither_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),(
void*)2,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),_tmp259)))))))),((
_tmp25A->tl=0,_tmp25A)))))),_tmp258)))))),0,0,0,0),0);_tmpC->sc=(void*)3;_tmpB->vardecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Vardecl*
v))Cyc_Dict_insert)(_tmpB->vardecls,(*qmain).f2,_tmpC);_tmp3->imports=((struct
Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*
v))Cyc_Dict_insert)(_tmp3->exports,0,_tmpB);return _tmp3;}}static void Cyc_Interface_err(
struct _dyneither_ptr msg);static void Cyc_Interface_err(struct _dyneither_ptr msg){
void*_tmp25C;(_tmp25C=0,Cyc_Tcutil_terr(0,msg,_tag_dyneither(_tmp25C,sizeof(void*),
0)));}static void*Cyc_Interface_invalid_arg(struct _dyneither_ptr s);static void*Cyc_Interface_invalid_arg(
struct _dyneither_ptr s){struct Cyc_Core_Invalid_argument_struct _tmp25F;struct Cyc_Core_Invalid_argument_struct*
_tmp25E;(int)_throw((void*)((_tmp25E=_cycalloc(sizeof(*_tmp25E)),((_tmp25E[0]=((
_tmp25F.tag=Cyc_Core_Invalid_argument,((_tmp25F.f1=s,_tmp25F)))),_tmp25E)))));}
static void Cyc_Interface_fields_err(struct _dyneither_ptr sc,struct _dyneither_ptr t,
struct _tuple0*n);static void Cyc_Interface_fields_err(struct _dyneither_ptr sc,
struct _dyneither_ptr t,struct _tuple0*n){const char*_tmp265;void*_tmp264[3];struct
Cyc_String_pa_struct _tmp263;struct Cyc_String_pa_struct _tmp262;struct Cyc_String_pa_struct
_tmp261;Cyc_Interface_err((struct _dyneither_ptr)((_tmp261.tag=0,((_tmp261.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(n)),((
_tmp262.tag=0,((_tmp262.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((
_tmp263.tag=0,((_tmp263.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sc),((
_tmp264[0]=& _tmp263,((_tmp264[1]=& _tmp262,((_tmp264[2]=& _tmp261,Cyc_aprintf(((
_tmp265="fields of %s %s %s have never been defined",_tag_dyneither(_tmp265,
sizeof(char),43))),_tag_dyneither(_tmp264,sizeof(void*),3)))))))))))))))))))));}
static void Cyc_Interface_body_err(struct _dyneither_ptr sc,struct _tuple0*n);static
void Cyc_Interface_body_err(struct _dyneither_ptr sc,struct _tuple0*n){const char*
_tmp26A;void*_tmp269[2];struct Cyc_String_pa_struct _tmp268;struct Cyc_String_pa_struct
_tmp267;Cyc_Interface_err((struct _dyneither_ptr)((_tmp267.tag=0,((_tmp267.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(n)),((
_tmp268.tag=0,((_tmp268.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sc),((
_tmp269[0]=& _tmp268,((_tmp269[1]=& _tmp267,Cyc_aprintf(((_tmp26A="the body of %s function %s has never been defined",
_tag_dyneither(_tmp26A,sizeof(char),50))),_tag_dyneither(_tmp269,sizeof(void*),2)))))))))))))));}
static void Cyc_Interface_static_err(struct _dyneither_ptr obj1,struct _tuple0*name1,
struct _dyneither_ptr obj2,struct _tuple0*name2);static void Cyc_Interface_static_err(
struct _dyneither_ptr obj1,struct _tuple0*name1,struct _dyneither_ptr obj2,struct
_tuple0*name2){if(obj1.curr != (_tag_dyneither(0,0,0)).curr){const char*_tmp271;
void*_tmp270[4];struct Cyc_String_pa_struct _tmp26F;struct Cyc_String_pa_struct
_tmp26E;struct Cyc_String_pa_struct _tmp26D;struct Cyc_String_pa_struct _tmp26C;Cyc_Interface_err((
struct _dyneither_ptr)((_tmp26C.tag=0,((_tmp26C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(name2)),((_tmp26D.tag=0,((_tmp26D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)obj2),((_tmp26E.tag=0,((_tmp26E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(name1)),((
_tmp26F.tag=0,((_tmp26F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj1),((
_tmp270[0]=& _tmp26F,((_tmp270[1]=& _tmp26E,((_tmp270[2]=& _tmp26D,((_tmp270[3]=&
_tmp26C,Cyc_aprintf(((_tmp271="declaration of %s %s relies on static %s %s",
_tag_dyneither(_tmp271,sizeof(char),44))),_tag_dyneither(_tmp270,sizeof(void*),4)))))))))))))))))))))))))));}}
static void Cyc_Interface_abstract_err(struct _dyneither_ptr obj1,struct _tuple0*
name1,struct _dyneither_ptr obj2,struct _tuple0*name2);static void Cyc_Interface_abstract_err(
struct _dyneither_ptr obj1,struct _tuple0*name1,struct _dyneither_ptr obj2,struct
_tuple0*name2){if(obj1.curr != (_tag_dyneither(0,0,0)).curr){const char*_tmp278;
void*_tmp277[4];struct Cyc_String_pa_struct _tmp276;struct Cyc_String_pa_struct
_tmp275;struct Cyc_String_pa_struct _tmp274;struct Cyc_String_pa_struct _tmp273;Cyc_Interface_err((
struct _dyneither_ptr)((_tmp273.tag=0,((_tmp273.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(name2)),((_tmp274.tag=0,((_tmp274.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)obj2),((_tmp275.tag=0,((_tmp275.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(name1)),((
_tmp276.tag=0,((_tmp276.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj1),((
_tmp277[0]=& _tmp276,((_tmp277[1]=& _tmp275,((_tmp277[2]=& _tmp274,((_tmp277[3]=&
_tmp273,Cyc_aprintf(((_tmp278="declaration of %s %s relies on fields of abstract %s %s",
_tag_dyneither(_tmp278,sizeof(char),56))),_tag_dyneither(_tmp277,sizeof(void*),4)))))))))))))))))))))))))));}}
struct Cyc_Interface_Seen{struct Cyc_Dict_Dict aggrs;struct Cyc_Dict_Dict datatypes;}
;static struct Cyc_Interface_Seen*Cyc_Interface_new_seen();static struct Cyc_Interface_Seen*
Cyc_Interface_new_seen(){struct Cyc_Interface_Seen*_tmp279;return(_tmp279=
_cycalloc(sizeof(*_tmp279)),((_tmp279->aggrs=((struct Cyc_Dict_Dict(*)(int(*cmp)(
struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp),((_tmp279->datatypes=((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp),_tmp279)))));}static int Cyc_Interface_check_public_type(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,
struct _tuple0*name,void*t);static int Cyc_Interface_check_public_type_list(struct
Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct
_tuple0*name,void*(*f)(void*),struct Cyc_List_List*l);static int Cyc_Interface_check_public_type_list(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,
struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*l){int _tmp2F=1;for(0;l != 
0;l=l->tl){if(!Cyc_Interface_check_public_type(ae,seen,obj,name,f((void*)l->hd)))
_tmp2F=0;}return _tmp2F;}static int Cyc_Interface_check_public_aggrdecl(struct Cyc_Dict_Dict
ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Aggrdecl*d);static int Cyc_Interface_check_public_aggrdecl(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Aggrdecl*d){{
struct _handler_cons _tmp30;_push_handler(& _tmp30);{int _tmp32=0;if(setjmp(_tmp30.handler))
_tmp32=1;if(!_tmp32){{int _tmp33=((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))
Cyc_Dict_lookup)(seen->aggrs,d->name);_npop_handler(0);return _tmp33;};
_pop_handler();}else{void*_tmp31=(void*)_exn_thrown;void*_tmp35=_tmp31;_LL1: if(
_tmp35 != Cyc_Dict_Absent)goto _LL3;_LL2: goto _LL0;_LL3:;_LL4:(void)_throw(_tmp35);
_LL0:;}}}{int _tmp36=1;seen->aggrs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp36);if(d->impl != 
0){struct Cyc_List_List*_tmp37=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(d->impl))->fields;
for(0;_tmp37 != 0;_tmp37=_tmp37->tl){const char*_tmp27A;if(!Cyc_Interface_check_public_type(
ae,seen,((_tmp27A="type",_tag_dyneither(_tmp27A,sizeof(char),5))),d->name,((
struct Cyc_Absyn_Aggrfield*)_tmp37->hd)->type))_tmp36=0;}}seen->aggrs=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,
_tmp36);return _tmp36;}}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};static
void*Cyc_Interface_get_type1(struct _tuple17*x);static void*Cyc_Interface_get_type1(
struct _tuple17*x){return(*x).f2;}static void*Cyc_Interface_get_type2(struct
_tuple7*x);static void*Cyc_Interface_get_type2(struct _tuple7*x){return(*x).f3;}
static int Cyc_Interface_check_public_datatypedecl(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*
seen,struct Cyc_Absyn_Datatypedecl*d);static int Cyc_Interface_check_public_datatypedecl(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Datatypedecl*
d){{struct _handler_cons _tmp39;_push_handler(& _tmp39);{int _tmp3B=0;if(setjmp(
_tmp39.handler))_tmp3B=1;if(!_tmp3B){{int _tmp3C=((int(*)(struct Cyc_Dict_Dict d,
struct _tuple0*k))Cyc_Dict_lookup)(seen->datatypes,d->name);_npop_handler(0);
return _tmp3C;};_pop_handler();}else{void*_tmp3A=(void*)_exn_thrown;void*_tmp3E=
_tmp3A;_LL6: if(_tmp3E != Cyc_Dict_Absent)goto _LL8;_LL7: goto _LL5;_LL8:;_LL9:(void)
_throw(_tmp3E);_LL5:;}}}{int _tmp3F=1;seen->datatypes=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->datatypes,d->name,
_tmp3F);if(d->fields != 0){struct Cyc_List_List*_tmp40=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(d->fields))->v;for(0;_tmp40 != 0;_tmp40=_tmp40->tl){
const char*_tmp27B;if(!((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*
seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple17*),
struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,seen,((_tmp27B="datatype",
_tag_dyneither(_tmp27B,sizeof(char),9))),d->name,Cyc_Interface_get_type1,((
struct Cyc_Absyn_Datatypefield*)_tmp40->hd)->typs))_tmp3F=0;}}seen->datatypes=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(
seen->datatypes,d->name,_tmp3F);return _tmp3F;}}static int Cyc_Interface_check_public_enumdecl(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Enumdecl*d);
static int Cyc_Interface_check_public_enumdecl(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*
seen,struct Cyc_Absyn_Enumdecl*d){return 1;}static int Cyc_Interface_check_public_typedefdecl(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*
d);static int Cyc_Interface_check_public_typedefdecl(struct Cyc_Dict_Dict ae,struct
Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*d){if(d->defn != 0)return Cyc_Interface_check_public_type(
ae,seen,(struct _dyneither_ptr)_tag_dyneither(0,0,0),d->name,(void*)((struct Cyc_Core_Opt*)
_check_null(d->defn))->v);else{return 1;}}static int Cyc_Interface_check_public_vardecl(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Vardecl*d);
static int Cyc_Interface_check_public_vardecl(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*
seen,struct Cyc_Absyn_Vardecl*d){const char*_tmp27C;return Cyc_Interface_check_public_type(
ae,seen,((_tmp27C="variable",_tag_dyneither(_tmp27C,sizeof(char),9))),d->name,d->type);}
static struct Cyc_List_List*Cyc_Interface_get_abs_ns(union Cyc_Absyn_Nmspace ns);
static struct Cyc_List_List*Cyc_Interface_get_abs_ns(union Cyc_Absyn_Nmspace ns){
union Cyc_Absyn_Nmspace _tmp43=ns;struct Cyc_List_List*_tmp44;_LLB: if((_tmp43.Abs_n).tag
!= 2)goto _LLD;_tmp44=(struct Cyc_List_List*)(_tmp43.Abs_n).val;_LLC: return _tmp44;
_LLD:;_LLE: {const char*_tmp27D;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((
_tmp27D="get_abs_ns",_tag_dyneither(_tmp27D,sizeof(char),11))));}_LLA:;}static
int Cyc_Interface_check_public_type(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*
seen,struct _dyneither_ptr obj,struct _tuple0*name,void*t);static int Cyc_Interface_check_public_type(
struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,
struct _tuple0*name,void*t){void*_tmp46=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp47;void*_tmp48;struct Cyc_Absyn_ArrayInfo _tmp49;void*_tmp4A;void**_tmp4B;void*
_tmp4C;struct Cyc_Absyn_FnInfo _tmp4D;void*_tmp4E;struct Cyc_List_List*_tmp4F;
struct Cyc_Absyn_VarargInfo*_tmp50;struct Cyc_List_List*_tmp51;struct Cyc_Absyn_AggrInfo
_tmp52;union Cyc_Absyn_AggrInfoU _tmp53;struct Cyc_List_List*_tmp54;struct _tuple0*
_tmp55;struct Cyc_Absyn_DatatypeInfo _tmp56;union Cyc_Absyn_DatatypeInfoU _tmp57;
struct Cyc_Absyn_Datatypedecl**_tmp58;struct Cyc_Absyn_Datatypedecl*_tmp59;struct
Cyc_List_List*_tmp5A;struct Cyc_Absyn_DatatypeFieldInfo _tmp5B;union Cyc_Absyn_DatatypeFieldInfoU
_tmp5C;struct _tuple1 _tmp5D;struct Cyc_Absyn_Datatypedecl*_tmp5E;struct Cyc_Absyn_Datatypefield*
_tmp5F;struct Cyc_List_List*_tmp60;_LL10: if(_tmp46 <= (void*)4)goto _LL22;if(*((int*)
_tmp46)!= 4)goto _LL12;_tmp47=((struct Cyc_Absyn_PointerType_struct*)_tmp46)->f1;
_tmp48=_tmp47.elt_typ;_LL11: _tmp4A=_tmp48;goto _LL13;_LL12: if(*((int*)_tmp46)!= 7)
goto _LL14;_tmp49=((struct Cyc_Absyn_ArrayType_struct*)_tmp46)->f1;_tmp4A=_tmp49.elt_type;
_LL13: _tmp4C=_tmp4A;goto _LL15;_LL14: if(*((int*)_tmp46)!= 16)goto _LL16;_tmp4B=((
struct Cyc_Absyn_TypedefType_struct*)_tmp46)->f4;if(_tmp4B == 0)goto _LL16;_tmp4C=*
_tmp4B;_LL15: return Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp4C);
_LL16: if(*((int*)_tmp46)!= 8)goto _LL18;_tmp4D=((struct Cyc_Absyn_FnType_struct*)
_tmp46)->f1;_tmp4E=_tmp4D.ret_typ;_tmp4F=_tmp4D.args;_tmp50=_tmp4D.cyc_varargs;
_LL17: {int b=((int(*)(struct Cyc_Dict_Dict ae,struct Cyc_Interface_Seen*seen,struct
_dyneither_ptr obj,struct _tuple0*name,void*(*f)(struct _tuple7*),struct Cyc_List_List*
l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type2,
_tmp4F) && Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp4E);if(_tmp50 != 
0){void*_tmp62;struct Cyc_Absyn_VarargInfo _tmp61=*_tmp50;_tmp62=_tmp61.type;b=Cyc_Interface_check_public_type(
ae,seen,obj,name,_tmp62);}return b;}_LL18: if(*((int*)_tmp46)!= 9)goto _LL1A;_tmp51=((
struct Cyc_Absyn_TupleType_struct*)_tmp46)->f1;_LL19: return((int(*)(struct Cyc_Dict_Dict
ae,struct Cyc_Interface_Seen*seen,struct _dyneither_ptr obj,struct _tuple0*name,void*(*
f)(struct _tuple17*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(
ae,seen,obj,name,Cyc_Interface_get_type1,_tmp51);_LL1A: if(*((int*)_tmp46)!= 10)
goto _LL1C;_tmp52=((struct Cyc_Absyn_AggrType_struct*)_tmp46)->f1;_tmp53=_tmp52.aggr_info;
_tmp54=_tmp52.targs;_LL1B: {struct Cyc_Absyn_Aggrdecl*_tmp63=Cyc_Absyn_get_known_aggrdecl(
_tmp53);if(_tmp63->sc == (void*)0){{const char*_tmp27E;Cyc_Interface_static_err(
obj,name,((_tmp27E="type",_tag_dyneither(_tmp27E,sizeof(char),5))),_tmp63->name);}
return 0;}return Cyc_Interface_check_public_type_list(ae,seen,obj,name,Cyc_Core_identity,
_tmp54) && Cyc_Interface_check_public_aggrdecl(ae,seen,_tmp63);}_LL1C: if(*((int*)
_tmp46)!= 12)goto _LL1E;_tmp55=((struct Cyc_Absyn_EnumType_struct*)_tmp46)->f1;
_LL1D: {struct _tuple0 _tmp66;union Cyc_Absyn_Nmspace _tmp67;struct _dyneither_ptr*
_tmp68;struct _tuple0*_tmp65=_tmp55;_tmp66=*_tmp65;_tmp67=_tmp66.f1;_tmp68=_tmp66.f2;{
struct Cyc_List_List*_tmp69=Cyc_Interface_get_abs_ns(_tmp67);struct Cyc_Absyn_Enumdecl*
ed;{struct _handler_cons _tmp6A;_push_handler(& _tmp6A);{int _tmp6C=0;if(setjmp(
_tmp6A.handler))_tmp6C=1;if(!_tmp6C){{struct Cyc_Tcenv_Genv*_tmp6D=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp69);ed=*((
struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp6D->enumdecls,_tmp68);};_pop_handler();}else{void*_tmp6B=(void*)_exn_thrown;
void*_tmp6F=_tmp6B;_LL25: if(_tmp6F != Cyc_Dict_Absent)goto _LL27;_LL26: {const char*
_tmp282;void*_tmp281[1];struct Cyc_String_pa_struct _tmp280;((int(*)(struct
_dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)((_tmp280.tag=
0,((_tmp280.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp55)),((_tmp281[0]=& _tmp280,Cyc_aprintf(((_tmp282="check_public_type (can't find enum %s)",
_tag_dyneither(_tmp282,sizeof(char),39))),_tag_dyneither(_tmp281,sizeof(void*),1)))))))));}
_LL27:;_LL28:(void)_throw(_tmp6F);_LL24:;}}}if(ed->sc == (void*)0){{const char*
_tmp283;Cyc_Interface_static_err(obj,_tmp55,((_tmp283="enum",_tag_dyneither(
_tmp283,sizeof(char),5))),ed->name);}return 0;}return 1;}}_LL1E: if(*((int*)_tmp46)
!= 2)goto _LL20;_tmp56=((struct Cyc_Absyn_DatatypeType_struct*)_tmp46)->f1;_tmp57=
_tmp56.datatype_info;if((_tmp57.KnownDatatype).tag != 2)goto _LL20;_tmp58=(struct
Cyc_Absyn_Datatypedecl**)(_tmp57.KnownDatatype).val;_tmp59=*_tmp58;_tmp5A=_tmp56.targs;
_LL1F: {struct _tuple0 _tmp75;union Cyc_Absyn_Nmspace _tmp76;struct _dyneither_ptr*
_tmp77;struct _tuple0*_tmp74=_tmp59->name;_tmp75=*_tmp74;_tmp76=_tmp75.f1;_tmp77=
_tmp75.f2;{struct Cyc_List_List*_tmp78=Cyc_Interface_get_abs_ns(_tmp76);struct Cyc_Absyn_Datatypedecl*
tud;{struct _handler_cons _tmp79;_push_handler(& _tmp79);{int _tmp7B=0;if(setjmp(
_tmp79.handler))_tmp7B=1;if(!_tmp7B){{struct Cyc_Tcenv_Genv*_tmp7C=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp78);tud=*((
struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))
Cyc_Dict_lookup)(_tmp7C->datatypedecls,_tmp77);};_pop_handler();}else{void*
_tmp7A=(void*)_exn_thrown;void*_tmp7E=_tmp7A;_LL2A: if(_tmp7E != Cyc_Dict_Absent)
goto _LL2C;_LL2B: {const char*_tmp287;void*_tmp286[1];struct Cyc_String_pa_struct
_tmp285;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct
_dyneither_ptr)((_tmp285.tag=0,((_tmp285.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp59->name)),((_tmp286[0]=& _tmp285,Cyc_aprintf(((
_tmp287="check_public_type (can't find datatype %s)",_tag_dyneither(_tmp287,
sizeof(char),43))),_tag_dyneither(_tmp286,sizeof(void*),1)))))))));}_LL2C:;_LL2D:(
void)_throw(_tmp7E);_LL29:;}}}if(tud->sc == (void*)0){{const char*_tmp288;Cyc_Interface_static_err(
obj,name,((_tmp288="datatype",_tag_dyneither(_tmp288,sizeof(char),9))),tud->name);}
return 0;}return Cyc_Interface_check_public_type_list(ae,seen,obj,name,Cyc_Core_identity,
_tmp5A) && Cyc_Interface_check_public_datatypedecl(ae,seen,tud);}}_LL20: if(*((
int*)_tmp46)!= 3)goto _LL22;_tmp5B=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp46)->f1;_tmp5C=_tmp5B.field_info;if((_tmp5C.KnownDatatypefield).tag != 2)goto
_LL22;_tmp5D=(struct _tuple1)(_tmp5C.KnownDatatypefield).val;_tmp5E=_tmp5D.f1;
_tmp5F=_tmp5D.f2;_tmp60=_tmp5B.targs;_LL21: {struct _tuple0 _tmp84;union Cyc_Absyn_Nmspace
_tmp85;struct _dyneither_ptr*_tmp86;struct _tuple0*_tmp83=_tmp5E->name;_tmp84=*
_tmp83;_tmp85=_tmp84.f1;_tmp86=_tmp84.f2;{struct Cyc_List_List*_tmp87=Cyc_Interface_get_abs_ns(
_tmp85);struct Cyc_Absyn_Datatypedecl*tud;{struct _handler_cons _tmp88;
_push_handler(& _tmp88);{int _tmp8A=0;if(setjmp(_tmp88.handler))_tmp8A=1;if(!
_tmp8A){{struct Cyc_Tcenv_Genv*_tmp8B=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp87);tud=*((struct Cyc_Absyn_Datatypedecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8B->datatypedecls,
_tmp86);};_pop_handler();}else{void*_tmp89=(void*)_exn_thrown;void*_tmp8D=_tmp89;
_LL2F: if(_tmp8D != Cyc_Dict_Absent)goto _LL31;_LL30: {const char*_tmp28C;void*
_tmp28B[1];struct Cyc_String_pa_struct _tmp28A;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((
struct _dyneither_ptr)((_tmp28A.tag=0,((_tmp28A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5E->name)),((_tmp28B[0]=& _tmp28A,Cyc_aprintf(((
_tmp28C="check_public_type (can't find datatype %s and search its fields)",
_tag_dyneither(_tmp28C,sizeof(char),65))),_tag_dyneither(_tmp28B,sizeof(void*),1)))))))));}
_LL31:;_LL32:(void)_throw(_tmp8D);_LL2E:;}}}if(tud->fields == 0){const char*
_tmp290;void*_tmp28F[1];struct Cyc_String_pa_struct _tmp28E;((int(*)(struct
_dyneither_ptr s))Cyc_Interface_invalid_arg)((struct _dyneither_ptr)((_tmp28E.tag=
0,((_tmp28E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tud->name)),((_tmp28F[0]=& _tmp28E,Cyc_aprintf(((_tmp290="check_public_type (datatype %s has no fields)",
_tag_dyneither(_tmp290,sizeof(char),46))),_tag_dyneither(_tmp28F,sizeof(void*),1)))))))));}{
struct Cyc_Absyn_Datatypefield*tuf1=0;{struct Cyc_List_List*_tmp94=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp94 != 0;_tmp94=_tmp94->tl){
if(Cyc_strptrcmp((*_tmp5F->name).f2,(*((struct Cyc_Absyn_Datatypefield*)_tmp94->hd)->name).f2)
== 0){tuf1=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)
_tmp94->hd);break;}}}if(tuf1 == 0){const char*_tmp294;void*_tmp293[1];struct Cyc_String_pa_struct
_tmp292;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)((struct
_dyneither_ptr)((_tmp292.tag=0,((_tmp292.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5F->name)),((_tmp293[0]=& _tmp292,Cyc_aprintf(((
_tmp294="check_public_type (can't find datatypefield %s)",_tag_dyneither(_tmp294,
sizeof(char),48))),_tag_dyneither(_tmp293,sizeof(void*),1)))))))));}{struct Cyc_Absyn_Datatypefield*
tuf=(struct Cyc_Absyn_Datatypefield*)tuf1;if(tud->sc == (void*)0){{const char*
_tmp295;Cyc_Interface_static_err(obj,name,((_tmp295="datatype",_tag_dyneither(
_tmp295,sizeof(char),9))),tud->name);}return 0;}if(tud->sc == (void*)1){{const char*
_tmp296;Cyc_Interface_abstract_err(obj,name,((_tmp296="datatype",_tag_dyneither(
_tmp296,sizeof(char),9))),tud->name);}return 0;}if(tuf->sc == (void*)0){{const char*
_tmp29A;void*_tmp299[1];struct Cyc_String_pa_struct _tmp298;Cyc_Interface_static_err(
obj,name,(struct _dyneither_ptr)((_tmp298.tag=0,((_tmp298.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp299[0]=& _tmp298,Cyc_aprintf(((_tmp29A="field %s of",_tag_dyneither(_tmp29A,
sizeof(char),12))),_tag_dyneither(_tmp299,sizeof(void*),1)))))))),tud->name);}
return 0;}return Cyc_Interface_check_public_type_list(ae,seen,obj,name,Cyc_Core_identity,
_tmp60) && Cyc_Interface_check_public_datatypedecl(ae,seen,tud);}}}}_LL22:;_LL23:
return 1;_LLF:;}struct _tuple18{struct Cyc_Interface_Ienv*f1;struct Cyc_Interface_Ienv*
f2;int f3;struct Cyc_Dict_Dict f4;struct Cyc_Interface_Seen*f5;struct Cyc_Interface_I*
f6;};static void Cyc_Interface_extract_aggrdecl(struct _tuple18*env,struct
_dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl**dp);static void Cyc_Interface_extract_aggrdecl(
struct _tuple18*env,struct _dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl**dp){struct
_tuple18 _tmp9E;struct Cyc_Interface_Ienv*_tmp9F;struct Cyc_Interface_Ienv*_tmpA0;
int _tmpA1;struct Cyc_Dict_Dict _tmpA2;struct Cyc_Interface_Seen*_tmpA3;struct
_tuple18*_tmp9D=env;_tmp9E=*_tmp9D;_tmp9F=_tmp9E.f1;_tmpA0=_tmp9E.f2;_tmpA1=
_tmp9E.f3;_tmpA2=_tmp9E.f4;_tmpA3=_tmp9E.f5;{struct Cyc_Absyn_Aggrdecl*_tmpA4=*dp;
void*_tmpA5=_tmpA4->sc;_LL34: if((int)_tmpA5 != 0)goto _LL36;_LL35: if(_tmpA1  && 
_tmpA4->impl == 0){const char*_tmp29C;const char*_tmp29B;Cyc_Interface_fields_err(((
_tmp29B="static",_tag_dyneither(_tmp29B,sizeof(char),7))),((_tmp29C="struct/union",
_tag_dyneither(_tmp29C,sizeof(char),13))),_tmpA4->name);}goto _LL33;_LL36: if((int)
_tmpA5 != 1)goto _LL38;_LL37: if(_tmpA4->impl == 0){if(_tmpA1){const char*_tmp29E;
const char*_tmp29D;Cyc_Interface_fields_err(((_tmp29D="abstract",_tag_dyneither(
_tmp29D,sizeof(char),9))),((_tmp29E="struct/union",_tag_dyneither(_tmp29E,
sizeof(char),13))),_tmpA4->name);}}else{{struct Cyc_Absyn_Aggrdecl*_tmp29F;_tmpA4=((
_tmp29F=_cycalloc(sizeof(*_tmp29F)),((_tmp29F[0]=*_tmpA4,_tmp29F))));}_tmpA4->impl=
0;}if(Cyc_Interface_check_public_aggrdecl(_tmpA2,_tmpA3,_tmpA4))_tmpA0->aggrdecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*
v))Cyc_Dict_insert)(_tmpA0->aggrdecls,x,_tmpA4);goto _LL33;_LL38: if((int)_tmpA5 != 
2)goto _LL3A;_LL39: if(_tmpA4->impl == 0){{const char*_tmp2A1;const char*_tmp2A0;Cyc_Interface_fields_err(((
_tmp2A0="public",_tag_dyneither(_tmp2A0,sizeof(char),7))),((_tmp2A1="struct/union",
_tag_dyneither(_tmp2A1,sizeof(char),13))),_tmpA4->name);}{struct Cyc_Absyn_Aggrdecl*
_tmp2A2;_tmpA4=((_tmp2A2=_cycalloc(sizeof(*_tmp2A2)),((_tmp2A2[0]=*_tmpA4,
_tmp2A2))));}_tmpA4->sc=(void*)1;}if(Cyc_Interface_check_public_aggrdecl(_tmpA2,
_tmpA3,_tmpA4))_tmpA0->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmpA0->aggrdecls,
x,_tmpA4);goto _LL33;_LL3A: if((int)_tmpA5 != 4)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if((
int)_tmpA5 != 3)goto _LL3E;_LL3D: if(Cyc_Interface_check_public_aggrdecl(_tmpA2,
_tmpA3,_tmpA4))_tmp9F->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp9F->aggrdecls,
x,_tmpA4);goto _LL33;_LL3E: if((int)_tmpA5 != 5)goto _LL33;_LL3F:{const char*_tmp2A3;((
int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp2A3="add_aggrdecl",
_tag_dyneither(_tmp2A3,sizeof(char),13))));}goto _LL33;_LL33:;}}static void Cyc_Interface_extract_enumdecl(
struct _tuple18*env,struct _dyneither_ptr*x,struct Cyc_Absyn_Enumdecl**dp);static
void Cyc_Interface_extract_enumdecl(struct _tuple18*env,struct _dyneither_ptr*x,
struct Cyc_Absyn_Enumdecl**dp){struct _tuple18 _tmpB0;struct Cyc_Interface_Ienv*
_tmpB1;struct Cyc_Interface_Ienv*_tmpB2;int _tmpB3;struct Cyc_Dict_Dict _tmpB4;
struct Cyc_Interface_Seen*_tmpB5;struct _tuple18*_tmpAF=env;_tmpB0=*_tmpAF;_tmpB1=
_tmpB0.f1;_tmpB2=_tmpB0.f2;_tmpB3=_tmpB0.f3;_tmpB4=_tmpB0.f4;_tmpB5=_tmpB0.f5;{
struct Cyc_Absyn_Enumdecl*_tmpB6=*dp;void*_tmpB7=_tmpB6->sc;_LL41: if((int)_tmpB7
!= 0)goto _LL43;_LL42: if(_tmpB3  && _tmpB6->fields == 0){const char*_tmp2A5;const
char*_tmp2A4;Cyc_Interface_fields_err(((_tmp2A4="static",_tag_dyneither(_tmp2A4,
sizeof(char),7))),((_tmp2A5="enum",_tag_dyneither(_tmp2A5,sizeof(char),5))),
_tmpB6->name);}goto _LL40;_LL43: if((int)_tmpB7 != 1)goto _LL45;_LL44: if(_tmpB6->fields
== 0){if(_tmpB3){const char*_tmp2A7;const char*_tmp2A6;Cyc_Interface_fields_err(((
_tmp2A6="abstract",_tag_dyneither(_tmp2A6,sizeof(char),9))),((_tmp2A7="enum",
_tag_dyneither(_tmp2A7,sizeof(char),5))),_tmpB6->name);}}else{{struct Cyc_Absyn_Enumdecl*
_tmp2A8;_tmpB6=((_tmp2A8=_cycalloc(sizeof(*_tmp2A8)),((_tmp2A8[0]=*_tmpB6,
_tmp2A8))));}_tmpB6->fields=0;}if(Cyc_Interface_check_public_enumdecl(_tmpB4,
_tmpB5,_tmpB6))_tmpB2->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmpB2->enumdecls,
x,_tmpB6);goto _LL40;_LL45: if((int)_tmpB7 != 2)goto _LL47;_LL46: if(_tmpB6->fields == 
0){{const char*_tmp2AA;const char*_tmp2A9;Cyc_Interface_fields_err(((_tmp2A9="public",
_tag_dyneither(_tmp2A9,sizeof(char),7))),((_tmp2AA="enum",_tag_dyneither(_tmp2AA,
sizeof(char),5))),_tmpB6->name);}{struct Cyc_Absyn_Enumdecl*_tmp2AB;_tmpB6=((
_tmp2AB=_cycalloc(sizeof(*_tmp2AB)),((_tmp2AB[0]=*_tmpB6,_tmp2AB))));}_tmpB6->sc=(
void*)1;}if(Cyc_Interface_check_public_enumdecl(_tmpB4,_tmpB5,_tmpB6))_tmpB2->enumdecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*
v))Cyc_Dict_insert)(_tmpB2->enumdecls,x,_tmpB6);goto _LL40;_LL47: if((int)_tmpB7 != 
4)goto _LL49;_LL48: goto _LL4A;_LL49: if((int)_tmpB7 != 3)goto _LL4B;_LL4A: if(Cyc_Interface_check_public_enumdecl(
_tmpB4,_tmpB5,_tmpB6))_tmpB1->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmpB1->enumdecls,
x,_tmpB6);goto _LL40;_LL4B: if((int)_tmpB7 != 5)goto _LL40;_LL4C:{const char*_tmp2AC;((
int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp2AC="add_enumdecl",
_tag_dyneither(_tmp2AC,sizeof(char),13))));}goto _LL40;_LL40:;}}static void Cyc_Interface_extract_xdatatypefielddecl(
struct Cyc_Interface_I*i,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_Absyn_Datatypefield*
f);static void Cyc_Interface_extract_xdatatypefielddecl(struct Cyc_Interface_I*i,
struct Cyc_Absyn_Datatypedecl*d,struct Cyc_Absyn_Datatypefield*f){struct Cyc_List_List*
_tmpC1=0;{union Cyc_Absyn_Nmspace _tmpC2=(*f->name).f1;struct Cyc_List_List*_tmpC3;
struct Cyc_List_List*_tmpC4;_LL4E: if((_tmpC2.Rel_n).tag != 1)goto _LL50;_tmpC3=(
struct Cyc_List_List*)(_tmpC2.Rel_n).val;_LL4F: _tmpC4=_tmpC3;goto _LL51;_LL50: if((
_tmpC2.Abs_n).tag != 2)goto _LL52;_tmpC4=(struct Cyc_List_List*)(_tmpC2.Abs_n).val;
_LL51: _tmpC1=_tmpC4;goto _LL4D;_LL52:;_LL53: goto _LL4D;_LL4D:;}{struct Cyc_Dict_Dict*
dict;{void*_tmpC5=f->sc;_LL55: if((int)_tmpC5 != 0)goto _LL57;_LL56: return;_LL57:
if((int)_tmpC5 != 3)goto _LL59;_LL58: dict=& i->imports;goto _LL54;_LL59: if((int)
_tmpC5 != 2)goto _LL5B;_LL5A: dict=& i->exports;goto _LL54;_LL5B:;_LL5C: {const char*
_tmp2AD;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp2AD="add_xdatatypefielddecl",
_tag_dyneither(_tmp2AD,sizeof(char),23))));}_LL54:;}{struct Cyc_Interface_Ienv*
env;{struct _handler_cons _tmpC7;_push_handler(& _tmpC7);{int _tmpC9=0;if(setjmp(
_tmpC7.handler))_tmpC9=1;if(!_tmpC9){env=((struct Cyc_Interface_Ienv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(*dict,_tmpC1);;
_pop_handler();}else{void*_tmpC8=(void*)_exn_thrown;void*_tmpCB=_tmpC8;_LL5E: if(
_tmpCB != Cyc_Dict_Absent)goto _LL60;_LL5F: env=Cyc_Interface_new_ienv();*dict=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*
v))Cyc_Dict_insert)(*dict,_tmpC1,env);goto _LL5D;_LL60:;_LL61:(void)_throw(_tmpCB);
_LL5D:;}}}{struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp2AE;env->xdatatypefielddecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Tcdecl_Xdatatypefielddecl*
v))Cyc_Dict_insert)(env->xdatatypefielddecls,(*f->name).f2,((_tmp2AE=_cycalloc(
sizeof(*_tmp2AE)),((_tmp2AE->base=d,((_tmp2AE->field=f,_tmp2AE)))))));}}}}static
void Cyc_Interface_extract_datatypedecl(struct _tuple18*env,struct _dyneither_ptr*x,
struct Cyc_Absyn_Datatypedecl**dp);static void Cyc_Interface_extract_datatypedecl(
struct _tuple18*env,struct _dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl**dp){
struct _tuple18 _tmpCE;struct Cyc_Interface_Ienv*_tmpCF;struct Cyc_Interface_Ienv*
_tmpD0;int _tmpD1;struct Cyc_Dict_Dict _tmpD2;struct Cyc_Interface_Seen*_tmpD3;
struct Cyc_Interface_I*_tmpD4;struct _tuple18*_tmpCD=env;_tmpCE=*_tmpCD;_tmpCF=
_tmpCE.f1;_tmpD0=_tmpCE.f2;_tmpD1=_tmpCE.f3;_tmpD2=_tmpCE.f4;_tmpD3=_tmpCE.f5;
_tmpD4=_tmpCE.f6;{struct Cyc_Absyn_Datatypedecl*_tmpD5=*dp;void*_tmpD6=_tmpD5->sc;
_LL63: if((int)_tmpD6 != 0)goto _LL65;_LL64: if((!_tmpD5->is_extensible  && _tmpD1)
 && _tmpD5->fields == 0){const char*_tmp2B0;const char*_tmp2AF;Cyc_Interface_fields_err(((
_tmp2AF="static",_tag_dyneither(_tmp2AF,sizeof(char),7))),((_tmp2B0="datatype",
_tag_dyneither(_tmp2B0,sizeof(char),9))),_tmpD5->name);}goto _LL62;_LL65: if((int)
_tmpD6 != 1)goto _LL67;_LL66: if(_tmpD5->fields == 0){if(!_tmpD5->is_extensible  && 
_tmpD1){const char*_tmp2B2;const char*_tmp2B1;Cyc_Interface_fields_err(((_tmp2B1="abstract",
_tag_dyneither(_tmp2B1,sizeof(char),9))),((_tmp2B2="datatype",_tag_dyneither(
_tmp2B2,sizeof(char),9))),_tmpD5->name);}}else{{struct Cyc_Absyn_Datatypedecl*
_tmp2B3;_tmpD5=((_tmp2B3=_cycalloc(sizeof(*_tmp2B3)),((_tmp2B3[0]=*_tmpD5,
_tmp2B3))));}_tmpD5->fields=0;}if(Cyc_Interface_check_public_datatypedecl(_tmpD2,
_tmpD3,_tmpD5))_tmpD0->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpD0->datatypedecls,
x,_tmpD5);goto _LL62;_LL67: if((int)_tmpD6 != 2)goto _LL69;_LL68:{struct Cyc_Absyn_Datatypedecl*
_tmp2B4;_tmpD5=((_tmp2B4=_cycalloc(sizeof(*_tmp2B4)),((_tmp2B4[0]=*_tmpD5,
_tmp2B4))));}if(!_tmpD5->is_extensible  && _tmpD5->fields == 0){{const char*_tmp2B6;
const char*_tmp2B5;Cyc_Interface_fields_err(((_tmp2B5="public",_tag_dyneither(
_tmp2B5,sizeof(char),7))),((_tmp2B6="datatype",_tag_dyneither(_tmp2B6,sizeof(
char),9))),_tmpD5->name);}_tmpD5->sc=(void*)1;}if(Cyc_Interface_check_public_datatypedecl(
_tmpD2,_tmpD3,_tmpD5)){if(_tmpD5->is_extensible  && _tmpD5->fields != 0){struct Cyc_List_List*
_tmpDF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpD5->fields))->v;
_tmpD5->fields=0;{struct Cyc_List_List*_tmpE0=_tmpDF;for(0;_tmpE0 != 0;_tmpE0=
_tmpE0->tl){Cyc_Interface_extract_xdatatypefielddecl(_tmpD4,_tmpD5,(struct Cyc_Absyn_Datatypefield*)
_tmpE0->hd);}}}_tmpD0->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*v))Cyc_Dict_insert)(_tmpD0->datatypedecls,
x,_tmpD5);}goto _LL62;_LL69: if((int)_tmpD6 != 4)goto _LL6B;_LL6A: {const char*
_tmp2B7;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((_tmp2B7="extract_datatypedecl",
_tag_dyneither(_tmp2B7,sizeof(char),21))));}_LL6B: if((int)_tmpD6 != 3)goto _LL6D;
_LL6C: if(Cyc_Interface_check_public_datatypedecl(_tmpD2,_tmpD3,_tmpD5)){if(
_tmpD5->is_extensible  && _tmpD5->fields != 0){{struct Cyc_Absyn_Datatypedecl*
_tmp2B8;_tmpD5=((_tmp2B8=_cycalloc(sizeof(*_tmp2B8)),((_tmp2B8[0]=*_tmpD5,
_tmp2B8))));}{struct Cyc_List_List*_tmpE3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmpD5->fields))->v;_tmpD5->fields=0;{struct Cyc_List_List*_tmpE4=
_tmpE3;for(0;_tmpE4 != 0;_tmpE4=_tmpE4->tl){Cyc_Interface_extract_xdatatypefielddecl(
_tmpD4,_tmpD5,(struct Cyc_Absyn_Datatypefield*)_tmpE4->hd);}}}}_tmpCF->datatypedecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl*
v))Cyc_Dict_insert)(_tmpCF->datatypedecls,x,_tmpD5);}goto _LL62;_LL6D: if((int)
_tmpD6 != 5)goto _LL62;_LL6E:{const char*_tmp2B9;((int(*)(struct _dyneither_ptr s))
Cyc_Interface_invalid_arg)(((_tmp2B9="add_datatypedecl",_tag_dyneither(_tmp2B9,
sizeof(char),17))));}goto _LL62;_LL62:;}}static void Cyc_Interface_extract_typedef(
struct _tuple18*env,struct _dyneither_ptr*x,struct Cyc_Absyn_Typedefdecl*d);static
void Cyc_Interface_extract_typedef(struct _tuple18*env,struct _dyneither_ptr*x,
struct Cyc_Absyn_Typedefdecl*d){struct _tuple18 _tmpE7;struct Cyc_Interface_Ienv*
_tmpE8;struct Cyc_Dict_Dict _tmpE9;struct Cyc_Interface_Seen*_tmpEA;struct _tuple18*
_tmpE6=env;_tmpE7=*_tmpE6;_tmpE8=_tmpE7.f2;_tmpE9=_tmpE7.f4;_tmpEA=_tmpE7.f5;if(
Cyc_Interface_check_public_typedefdecl(_tmpE9,_tmpEA,d))_tmpE8->typedefdecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*
v))Cyc_Dict_insert)(_tmpE8->typedefdecls,x,d);}static void Cyc_Interface_extract_ordinarie(
struct _tuple18*env,struct _dyneither_ptr*x,struct _tuple5*v);static void Cyc_Interface_extract_ordinarie(
struct _tuple18*env,struct _dyneither_ptr*x,struct _tuple5*v){struct _tuple18 _tmpEC;
struct Cyc_Interface_Ienv*_tmpED;struct Cyc_Interface_Ienv*_tmpEE;int _tmpEF;struct
Cyc_Dict_Dict _tmpF0;struct Cyc_Interface_Seen*_tmpF1;struct _tuple18*_tmpEB=env;
_tmpEC=*_tmpEB;_tmpED=_tmpEC.f1;_tmpEE=_tmpEC.f2;_tmpEF=_tmpEC.f3;_tmpF0=_tmpEC.f4;
_tmpF1=_tmpEC.f5;{void*_tmpF2=(*v).f1;void*_tmpF3=_tmpF2;void*_tmpF4;_LL70: if(*((
int*)_tmpF3)!= 0)goto _LL72;_tmpF4=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmpF3)->f1;
_LL71:{void*_tmpF5=_tmpF4;struct Cyc_Absyn_Fndecl*_tmpF6;struct Cyc_Absyn_Vardecl*
_tmpF7;_LL75: if(_tmpF5 <= (void*)1)goto _LL79;if(*((int*)_tmpF5)!= 1)goto _LL77;
_tmpF6=((struct Cyc_Absyn_Funname_b_struct*)_tmpF5)->f1;_LL76: {struct Cyc_Absyn_Vardecl*
_tmp2BA;struct Cyc_Absyn_Vardecl*_tmpF8=(_tmp2BA=_cycalloc(sizeof(*_tmp2BA)),((
_tmp2BA->sc=_tmpF6->sc,((_tmp2BA->name=_tmpF6->name,((_tmp2BA->tq=Cyc_Absyn_empty_tqual(
0),((_tmp2BA->type=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF6->cached_typ))->v,((
_tmp2BA->initializer=0,((_tmp2BA->rgn=0,((_tmp2BA->attributes=0,((_tmp2BA->escapes=
0,_tmp2BA)))))))))))))))));_tmpEF=0;_tmpF7=_tmpF8;goto _LL78;}_LL77: if(*((int*)
_tmpF5)!= 0)goto _LL79;_tmpF7=((struct Cyc_Absyn_Global_b_struct*)_tmpF5)->f1;
_LL78: if(_tmpF7->initializer != 0){{struct Cyc_Absyn_Vardecl*_tmp2BB;_tmpF7=((
_tmp2BB=_cycalloc(sizeof(*_tmp2BB)),((_tmp2BB[0]=*_tmpF7,_tmp2BB))));}_tmpF7->initializer=
0;}{void*_tmpFB=_tmpF7->sc;_LL7C: if((int)_tmpFB != 0)goto _LL7E;_LL7D: if(_tmpEF
 && Cyc_Tcutil_is_function_type(_tmpF7->type)){const char*_tmp2BC;Cyc_Interface_body_err(((
_tmp2BC="static",_tag_dyneither(_tmp2BC,sizeof(char),7))),_tmpF7->name);}goto
_LL7B;_LL7E: if((int)_tmpFB != 5)goto _LL80;_LL7F: goto _LL81;_LL80: if((int)_tmpFB != 
1)goto _LL82;_LL81: {const char*_tmp2BD;((int(*)(struct _dyneither_ptr s))Cyc_Interface_invalid_arg)(((
_tmp2BD="extract_ordinarie",_tag_dyneither(_tmp2BD,sizeof(char),18))));}_LL82:
if((int)_tmpFB != 2)goto _LL84;_LL83: if(_tmpEF  && Cyc_Tcutil_is_function_type(
_tmpF7->type)){const char*_tmp2BE;Cyc_Interface_body_err(((_tmp2BE="public",
_tag_dyneither(_tmp2BE,sizeof(char),7))),_tmpF7->name);}if(Cyc_Interface_check_public_vardecl(
_tmpF0,_tmpF1,_tmpF7))_tmpEE->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpEE->vardecls,
x,_tmpF7);goto _LL7B;_LL84: if((int)_tmpFB != 4)goto _LL86;_LL85: goto _LL87;_LL86: if((
int)_tmpFB != 3)goto _LL7B;_LL87: if(Cyc_Interface_check_public_vardecl(_tmpF0,
_tmpF1,_tmpF7))_tmpED->vardecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpED->vardecls,
x,_tmpF7);goto _LL7B;_LL7B:;}goto _LL74;_LL79:;_LL7A: goto _LL74;_LL74:;}goto _LL6F;
_LL72:;_LL73: goto _LL6F;_LL6F:;}}struct _tuple19{struct Cyc_Interface_I*f1;int f2;
struct Cyc_Dict_Dict f3;struct Cyc_Interface_Seen*f4;};static void Cyc_Interface_extract_f(
struct _tuple19*env_f,struct Cyc_List_List*ns,struct Cyc_Tcenv_Genv*ge);static void
Cyc_Interface_extract_f(struct _tuple19*env_f,struct Cyc_List_List*ns,struct Cyc_Tcenv_Genv*
ge){struct _tuple19 _tmp100;struct Cyc_Interface_I*_tmp101;int _tmp102;struct Cyc_Dict_Dict
_tmp103;struct Cyc_Interface_Seen*_tmp104;struct _tuple19*_tmpFF=env_f;_tmp100=*
_tmpFF;_tmp101=_tmp100.f1;_tmp102=_tmp100.f2;_tmp103=_tmp100.f3;_tmp104=_tmp100.f4;{
struct _tuple18 _tmp2BF;struct _tuple18 _tmp105=(_tmp2BF.f1=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp101->imports,ns),((
_tmp2BF.f2=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(_tmp101->exports,ns),((_tmp2BF.f3=_tmp102,((_tmp2BF.f4=
_tmp103,((_tmp2BF.f5=_tmp104,((_tmp2BF.f6=_tmp101,_tmp2BF)))))))))));((void(*)(
void(*f)(struct _tuple18*,struct _dyneither_ptr*,struct Cyc_Absyn_Aggrdecl**),
struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_aggrdecl,&
_tmp105,ge->aggrdecls);((void(*)(void(*f)(struct _tuple18*,struct _dyneither_ptr*,
struct Cyc_Absyn_Datatypedecl**),struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_Interface_extract_datatypedecl,& _tmp105,ge->datatypedecls);((void(*)(void(*f)(
struct _tuple18*,struct _dyneither_ptr*,struct Cyc_Absyn_Enumdecl**),struct _tuple18*
env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_enumdecl,&
_tmp105,ge->enumdecls);((void(*)(void(*f)(struct _tuple18*,struct _dyneither_ptr*,
struct Cyc_Absyn_Typedefdecl*),struct _tuple18*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_Interface_extract_typedef,& _tmp105,ge->typedefs);((void(*)(void(*f)(struct
_tuple18*,struct _dyneither_ptr*,struct _tuple5*),struct _tuple18*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_Interface_extract_ordinarie,& _tmp105,ge->ordinaries);}}
static struct Cyc_Interface_I*Cyc_Interface_gen_extract(struct Cyc_Dict_Dict ae,int
check_complete_defs);static struct Cyc_Interface_I*Cyc_Interface_gen_extract(
struct Cyc_Dict_Dict ae,int check_complete_defs){struct _tuple19 _tmp2C0;struct
_tuple19 env=(_tmp2C0.f1=((struct Cyc_Interface_I*(*)(struct Cyc_Dict_Dict skel))Cyc_Interface_skel2i)(
ae),((_tmp2C0.f2=check_complete_defs,((_tmp2C0.f3=ae,((_tmp2C0.f4=Cyc_Interface_new_seen(),
_tmp2C0)))))));((void(*)(void(*f)(struct _tuple19*,struct Cyc_List_List*,struct Cyc_Tcenv_Genv*),
struct _tuple19*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_extract_f,&
env,ae);return env.f1;}struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict
ae);struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae){return Cyc_Interface_gen_extract(
ae,1);}static void Cyc_Interface_check_err(struct _dyneither_ptr*msg1,struct
_dyneither_ptr msg2);inline static void Cyc_Interface_check_err(struct _dyneither_ptr*
msg1,struct _dyneither_ptr msg2){const char*_tmp2C4;void*_tmp2C3[1];struct Cyc_String_pa_struct
_tmp2C2;(_tmp2C2.tag=0,((_tmp2C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
msg2),((_tmp2C3[0]=& _tmp2C2,Cyc_Tcdecl_merr(0,msg1,((_tmp2C4="%s",_tag_dyneither(
_tmp2C4,sizeof(char),3))),_tag_dyneither(_tmp2C3,sizeof(void*),1)))))));}struct
_tuple20{int f1;struct Cyc_Dict_Dict f2;int(*f3)(void*,void*,struct _dyneither_ptr*);
struct _dyneither_ptr f4;struct _dyneither_ptr*f5;};static void Cyc_Interface_incl_dict_f(
struct _tuple20*env,struct _dyneither_ptr*x,void*y1);static void Cyc_Interface_incl_dict_f(
struct _tuple20*env,struct _dyneither_ptr*x,void*y1){struct _tuple20 _tmp10C;int
_tmp10D;int*_tmp10E;struct Cyc_Dict_Dict _tmp10F;int(*_tmp110)(void*,void*,struct
_dyneither_ptr*);struct _dyneither_ptr _tmp111;struct _dyneither_ptr*_tmp112;struct
_tuple20*_tmp10B=env;_tmp10C=*_tmp10B;_tmp10D=_tmp10C.f1;_tmp10E=(int*)&(*
_tmp10B).f1;_tmp10F=_tmp10C.f2;_tmp110=_tmp10C.f3;_tmp111=_tmp10C.f4;_tmp112=
_tmp10C.f5;{struct _handler_cons _tmp113;_push_handler(& _tmp113);{int _tmp115=0;if(
setjmp(_tmp113.handler))_tmp115=1;if(!_tmp115){{void*_tmp116=((void*(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp10F,x);if(!_tmp110(
y1,_tmp116,_tmp112))*_tmp10E=0;};_pop_handler();}else{void*_tmp114=(void*)
_exn_thrown;void*_tmp118=_tmp114;_LL89: if(_tmp118 != Cyc_Dict_Absent)goto _LL8B;
_LL8A:{const char*_tmp2C9;void*_tmp2C8[2];struct Cyc_String_pa_struct _tmp2C7;
struct Cyc_String_pa_struct _tmp2C6;Cyc_Interface_check_err(_tmp112,(struct
_dyneither_ptr)((_tmp2C6.tag=0,((_tmp2C6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*x),((_tmp2C7.tag=0,((_tmp2C7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp111),((_tmp2C8[0]=& _tmp2C7,((_tmp2C8[1]=& _tmp2C6,Cyc_aprintf(((
_tmp2C9="%s %s is missing",_tag_dyneither(_tmp2C9,sizeof(char),17))),
_tag_dyneither(_tmp2C8,sizeof(void*),2)))))))))))))));}*_tmp10E=0;goto _LL88;
_LL8B:;_LL8C:(void)_throw(_tmp118);_LL88:;}}}}static int Cyc_Interface_incl_dict(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(void*,void*,struct
_dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_dict(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(void*,void*,struct
_dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg){struct _tuple20
_tmp2CA;struct _tuple20 _tmp11D=(_tmp2CA.f1=1,((_tmp2CA.f2=dic2,((_tmp2CA.f3=
incl_f,((_tmp2CA.f4=t,((_tmp2CA.f5=msg,_tmp2CA)))))))));((void(*)(void(*f)(
struct _tuple20*,struct _dyneither_ptr*,void*),struct _tuple20*env,struct Cyc_Dict_Dict
d))Cyc_Dict_iter_c)(Cyc_Interface_incl_dict_f,& _tmp11D,dic1);return _tmp11D.f1;}
static int Cyc_Interface_incl_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*
d1,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_aggrdecl(struct Cyc_Absyn_Aggrdecl*
d0,struct Cyc_Absyn_Aggrdecl*d1,struct _dyneither_ptr*msg){struct Cyc_Absyn_Aggrdecl*
_tmp11F=Cyc_Tcdecl_merge_aggrdecl(d0,d1,0,msg);if(_tmp11F == 0)return 0;if((struct
Cyc_Absyn_Aggrdecl*)d0 != _tmp11F){{const char*_tmp2CE;void*_tmp2CD[1];struct Cyc_String_pa_struct
_tmp2CC;Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2CC.tag=0,((
_tmp2CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
d1->name)),((_tmp2CD[0]=& _tmp2CC,Cyc_aprintf(((_tmp2CE="declaration of type %s discloses too much information",
_tag_dyneither(_tmp2CE,sizeof(char),54))),_tag_dyneither(_tmp2CD,sizeof(void*),1)))))))));}
return 0;}return 1;}static int Cyc_Interface_incl_datatypedecl(struct Cyc_Absyn_Datatypedecl*
d0,struct Cyc_Absyn_Datatypedecl*d1,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_datatypedecl(
struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,struct
_dyneither_ptr*msg){struct Cyc_Absyn_Datatypedecl*_tmp123=Cyc_Tcdecl_merge_datatypedecl(
d0,d1,0,msg);if(_tmp123 == 0)return 0;if((struct Cyc_Absyn_Datatypedecl*)d0 != 
_tmp123){{const char*_tmp2D2;void*_tmp2D1[1];struct Cyc_String_pa_struct _tmp2D0;
Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2D0.tag=0,((_tmp2D0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((
_tmp2D1[0]=& _tmp2D0,Cyc_aprintf(((_tmp2D2="declaration of datatype %s discloses too much information",
_tag_dyneither(_tmp2D2,sizeof(char),58))),_tag_dyneither(_tmp2D1,sizeof(void*),1)))))))));}
return 0;}return 1;}static int Cyc_Interface_incl_enumdecl(struct Cyc_Absyn_Enumdecl*
d0,struct Cyc_Absyn_Enumdecl*d1,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct _dyneither_ptr*msg){
struct Cyc_Absyn_Enumdecl*_tmp127=Cyc_Tcdecl_merge_enumdecl(d0,d1,0,msg);if(
_tmp127 == 0)return 0;if((struct Cyc_Absyn_Enumdecl*)d0 != _tmp127){{const char*
_tmp2D6;void*_tmp2D5[1];struct Cyc_String_pa_struct _tmp2D4;Cyc_Interface_check_err(
msg,(struct _dyneither_ptr)((_tmp2D4.tag=0,((_tmp2D4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((_tmp2D5[0]=& _tmp2D4,
Cyc_aprintf(((_tmp2D6="declaration of enum %s discloses too much information",
_tag_dyneither(_tmp2D6,sizeof(char),54))),_tag_dyneither(_tmp2D5,sizeof(void*),1)))))))));}
return 0;}return 1;}static int Cyc_Interface_incl_vardecl(struct Cyc_Absyn_Vardecl*d0,
struct Cyc_Absyn_Vardecl*d1,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_vardecl(
struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct _dyneither_ptr*msg){
struct Cyc_Absyn_Vardecl*_tmp12B=Cyc_Tcdecl_merge_vardecl(d0,d1,0,msg);if(_tmp12B
== 0)return 0;if((struct Cyc_Absyn_Vardecl*)d0 != _tmp12B){{const char*_tmp2DA;void*
_tmp2D9[1];struct Cyc_String_pa_struct _tmp2D8;Cyc_Interface_check_err(msg,(struct
_dyneither_ptr)((_tmp2D8.tag=0,((_tmp2D8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((_tmp2D9[0]=& _tmp2D8,Cyc_aprintf(((
_tmp2DA="declaration of variable %s discloses too much information",
_tag_dyneither(_tmp2DA,sizeof(char),58))),_tag_dyneither(_tmp2D9,sizeof(void*),1)))))))));}
return 0;}return 1;}static int Cyc_Interface_incl_typedefdecl(struct Cyc_Absyn_Typedefdecl*
d0,struct Cyc_Absyn_Typedefdecl*d1,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_typedefdecl(
struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct
_dyneither_ptr*msg){struct Cyc_Absyn_Typedefdecl*_tmp12F=Cyc_Tcdecl_merge_typedefdecl(
d0,d1,0,msg);if(_tmp12F == 0)return 0;if((struct Cyc_Absyn_Typedefdecl*)d0 != 
_tmp12F){{const char*_tmp2DE;void*_tmp2DD[1];struct Cyc_String_pa_struct _tmp2DC;
Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2DC.tag=0,((_tmp2DC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(d1->name)),((
_tmp2DD[0]=& _tmp2DC,Cyc_aprintf(((_tmp2DE="declaration of typedef %s discloses too much information",
_tag_dyneither(_tmp2DE,sizeof(char),57))),_tag_dyneither(_tmp2DD,sizeof(void*),1)))))))));}
return 0;}return 1;}static int Cyc_Interface_incl_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*
d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_xdatatypefielddecl(
struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,
struct _dyneither_ptr*msg){struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp133=Cyc_Tcdecl_merge_xdatatypefielddecl(
d0,d1,0,msg);if(_tmp133 == 0)return 0;if((struct Cyc_Tcdecl_Xdatatypefielddecl*)d0
!= _tmp133){{const char*_tmp2E2;void*_tmp2E1[1];struct Cyc_String_pa_struct _tmp2E0;
Cyc_Interface_check_err(msg,(struct _dyneither_ptr)((_tmp2E0.tag=0,((_tmp2E0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((d1->field)->name)),((
_tmp2E1[0]=& _tmp2E0,Cyc_aprintf(((_tmp2E2="declaration of xdatatypefield %s discloses too much information",
_tag_dyneither(_tmp2E2,sizeof(char),64))),_tag_dyneither(_tmp2E1,sizeof(void*),1)))))))));}
return 0;}return 1;}struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,
struct Cyc_Position_Segment*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct
_dyneither_ptr*msg);static int Cyc_Interface_incl_ienv(struct Cyc_Interface_Ienv*
ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_ienv(
struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*
msg){const char*_tmp2E3;int _tmp137=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,int(*incl_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,struct
_dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(
ie1->aggrdecls,ie2->aggrdecls,Cyc_Interface_incl_aggrdecl,((_tmp2E3="type",
_tag_dyneither(_tmp2E3,sizeof(char),5))),msg);const char*_tmp2E4;int _tmp138=((int(*)(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Datatypedecl*,
struct Cyc_Absyn_Datatypedecl*,struct _dyneither_ptr*),struct _dyneither_ptr t,
struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->datatypedecls,ie2->datatypedecls,
Cyc_Interface_incl_datatypedecl,((_tmp2E4="datatype",_tag_dyneither(_tmp2E4,
sizeof(char),9))),msg);const char*_tmp2E5;int _tmp139=((int(*)(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(
ie1->enumdecls,ie2->enumdecls,Cyc_Interface_incl_enumdecl,((_tmp2E5="enum",
_tag_dyneither(_tmp2E5,sizeof(char),5))),msg);const char*_tmp2E6;int _tmp13A=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct
Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,struct _dyneither_ptr*),
struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls,ie2->typedefdecls,((struct Cyc_Dict_Dict(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_typedefdecl,((
_tmp2E6="typedef",_tag_dyneither(_tmp2E6,sizeof(char),8))),msg)!= 0;const char*
_tmp2E7;int _tmp13B=((int(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,int(*
incl_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct _dyneither_ptr*),
struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->vardecls,
ie2->vardecls,Cyc_Interface_incl_vardecl,((_tmp2E7="variable",_tag_dyneither(
_tmp2E7,sizeof(char),9))),msg);const char*_tmp2E8;int _tmp13C=((int(*)(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,int(*incl_f)(struct Cyc_Tcdecl_Xdatatypefielddecl*,
struct Cyc_Tcdecl_Xdatatypefielddecl*,struct _dyneither_ptr*),struct _dyneither_ptr
t,struct _dyneither_ptr*msg))Cyc_Interface_incl_dict)(ie1->xdatatypefielddecls,
ie2->xdatatypefielddecls,Cyc_Interface_incl_xdatatypefielddecl,((_tmp2E8="xdatatypefield",
_tag_dyneither(_tmp2E8,sizeof(char),15))),msg);return((((_tmp137  && _tmp138) && 
_tmp139) && _tmp13A) && _tmp13B) && _tmp13C;}struct _tuple21{int f1;struct Cyc_Dict_Dict
f2;struct _dyneither_ptr*f3;};static void Cyc_Interface_incl_ns_f(struct _tuple21*
env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie1);static void Cyc_Interface_incl_ns_f(
struct _tuple21*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie1){struct
_tuple21 _tmp144;int _tmp145;int*_tmp146;struct Cyc_Dict_Dict _tmp147;struct
_dyneither_ptr*_tmp148;struct _tuple21*_tmp143=env;_tmp144=*_tmp143;_tmp145=
_tmp144.f1;_tmp146=(int*)&(*_tmp143).f1;_tmp147=_tmp144.f2;_tmp148=_tmp144.f3;{
struct Cyc_Interface_Ienv*ie2;{struct _handler_cons _tmp149;_push_handler(& _tmp149);{
int _tmp14B=0;if(setjmp(_tmp149.handler))_tmp14B=1;if(!_tmp14B){ie2=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp147,ns);;
_pop_handler();}else{void*_tmp14A=(void*)_exn_thrown;void*_tmp14D=_tmp14A;_LL8E:
if(_tmp14D != Cyc_Dict_Absent)goto _LL90;_LL8F: ie2=Cyc_Interface_lazy_new_ienv();
goto _LL8D;_LL90:;_LL91:(void)_throw(_tmp14D);_LL8D:;}}}if(!Cyc_Interface_incl_ienv(
ie1,ie2,_tmp148))*_tmp146=0;}}static int Cyc_Interface_incl_ns(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr*msg);static int Cyc_Interface_incl_ns(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr*msg){struct
_tuple21 _tmp2E9;struct _tuple21 _tmp14E=(_tmp2E9.f1=1,((_tmp2E9.f2=dic2,((_tmp2E9.f3=
msg,_tmp2E9)))));((void(*)(void(*f)(struct _tuple21*,struct Cyc_List_List*,struct
Cyc_Interface_Ienv*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_Interface_incl_ns_f,& _tmp14E,dic1);return _tmp14E.f1;}int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info);int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple10*info){struct
_handler_cons _tmp150;_push_handler(& _tmp150);{int _tmp152=0;if(setjmp(_tmp150.handler))
_tmp152=1;if(!_tmp152){{int _tmp153=1;struct _dyneither_ptr*msg=0;if(info != 0){
struct Cyc_String_pa_struct _tmp2F3;struct Cyc_String_pa_struct _tmp2F2;void*_tmp2F1[
2];const char*_tmp2F0;struct _dyneither_ptr*_tmp2EF;msg=((_tmp2EF=_cycalloc(
sizeof(*_tmp2EF)),((_tmp2EF[0]=(struct _dyneither_ptr)((_tmp2F3.tag=0,((_tmp2F3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2),((_tmp2F2.tag=0,((
_tmp2F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((_tmp2F1[0]=&
_tmp2F2,((_tmp2F1[1]=& _tmp2F3,Cyc_aprintf(((_tmp2F0="checking inclusion of %s exports into %s exports,",
_tag_dyneither(_tmp2F0,sizeof(char),50))),_tag_dyneither(_tmp2F1,sizeof(void*),2)))))))))))))),
_tmp2EF))));}if(!Cyc_Interface_incl_ns(i1->exports,i2->exports,msg))_tmp153=0;
if(info != 0){struct Cyc_String_pa_struct _tmp2FD;struct Cyc_String_pa_struct _tmp2FC;
void*_tmp2FB[2];const char*_tmp2FA;struct _dyneither_ptr*_tmp2F9;msg=((_tmp2F9=
_cycalloc(sizeof(*_tmp2F9)),((_tmp2F9[0]=(struct _dyneither_ptr)((_tmp2FD.tag=0,((
_tmp2FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((_tmp2FC.tag=
0,((_tmp2FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2),((
_tmp2FB[0]=& _tmp2FC,((_tmp2FB[1]=& _tmp2FD,Cyc_aprintf(((_tmp2FA="checking inclusion of %s imports into %s imports,",
_tag_dyneither(_tmp2FA,sizeof(char),50))),_tag_dyneither(_tmp2FB,sizeof(void*),2)))))))))))))),
_tmp2F9))));}if(!Cyc_Interface_incl_ns(i2->imports,i1->imports,msg))_tmp153=0;{
int _tmp15E=_tmp153;_npop_handler(0);return _tmp15E;}};_pop_handler();}else{void*
_tmp151=(void*)_exn_thrown;void*_tmp160=_tmp151;_LL93: if(_tmp160 != Cyc_Tcdecl_Incompatible)
goto _LL95;_LL94: return 0;_LL95:;_LL96:(void)_throw(_tmp160);_LL92:;}}}struct
_tuple22{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct Cyc_Dict_Dict f4;
void*(*f5)(void*,void*,struct Cyc_Position_Segment*,struct _dyneither_ptr*);struct
_dyneither_ptr f6;struct _dyneither_ptr*f7;};void Cyc_Interface_compat_merge_dict_f(
struct _tuple22*env,struct _dyneither_ptr*x,void*y2);void Cyc_Interface_compat_merge_dict_f(
struct _tuple22*env,struct _dyneither_ptr*x,void*y2){struct _tuple22 _tmp162;int
_tmp163;int*_tmp164;struct Cyc_Dict_Dict _tmp165;struct Cyc_Dict_Dict*_tmp166;
struct Cyc_Dict_Dict _tmp167;struct Cyc_Dict_Dict _tmp168;void*(*_tmp169)(void*,void*,
struct Cyc_Position_Segment*,struct _dyneither_ptr*);struct _dyneither_ptr _tmp16A;
struct _dyneither_ptr*_tmp16B;struct _tuple22*_tmp161=env;_tmp162=*_tmp161;_tmp163=
_tmp162.f1;_tmp164=(int*)&(*_tmp161).f1;_tmp165=_tmp162.f2;_tmp166=(struct Cyc_Dict_Dict*)&(*
_tmp161).f2;_tmp167=_tmp162.f3;_tmp168=_tmp162.f4;_tmp169=_tmp162.f5;_tmp16A=
_tmp162.f6;_tmp16B=_tmp162.f7;{void*y;{struct _handler_cons _tmp16C;_push_handler(&
_tmp16C);{int _tmp16E=0;if(setjmp(_tmp16C.handler))_tmp16E=1;if(!_tmp16E){{void*
_tmp16F=((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp167,x);void*_tmp170=_tmp169(_tmp16F,y2,0,_tmp16B);if(!((unsigned int)_tmp170)){*
_tmp164=0;_npop_handler(0);return;}y=(void*)_tmp170;};_pop_handler();}else{void*
_tmp16D=(void*)_exn_thrown;void*_tmp172=_tmp16D;_LL98: if(_tmp172 != Cyc_Dict_Absent)
goto _LL9A;_LL99: y=y2;goto _LL97;_LL9A:;_LL9B:(void)_throw(_tmp172);_LL97:;}}}{
struct _handler_cons _tmp173;_push_handler(& _tmp173);{int _tmp175=0;if(setjmp(
_tmp173.handler))_tmp175=1;if(!_tmp175){{void*_tmp176=((void*(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp168,x);void*_tmp177=_tmp169(
_tmp176,y,0,_tmp16B);if(_tmp177 != (void*)_tmp176){if((unsigned int)_tmp177){
const char*_tmp302;void*_tmp301[2];struct Cyc_String_pa_struct _tmp300;struct Cyc_String_pa_struct
_tmp2FF;Cyc_Interface_check_err(_tmp16B,(struct _dyneither_ptr)((_tmp2FF.tag=0,((
_tmp2FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*x),((_tmp300.tag=0,((
_tmp300.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16A),((_tmp301[0]=&
_tmp300,((_tmp301[1]=& _tmp2FF,Cyc_aprintf(((_tmp302="abstract %s %s is being imported as non-abstract",
_tag_dyneither(_tmp302,sizeof(char),49))),_tag_dyneither(_tmp301,sizeof(void*),2)))))))))))))));}*
_tmp164=0;}};_pop_handler();}else{void*_tmp174=(void*)_exn_thrown;void*_tmp17D=
_tmp174;_LL9D: if(_tmp17D != Cyc_Dict_Absent)goto _LL9F;_LL9E: if(*_tmp164)*_tmp166=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(*
_tmp166,x,y);goto _LL9C;_LL9F:;_LLA0:(void)_throw(_tmp17D);_LL9C:;}}}}}struct Cyc_Dict_Dict*
Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,
struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg);struct Cyc_Dict_Dict*
Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,
struct Cyc_Dict_Dict excl,void*(*merge_f)(void*,void*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg){struct
_tuple22 _tmp303;struct _tuple22 _tmp17E=(_tmp303.f1=1,((_tmp303.f2=dic1,((_tmp303.f3=
dic1,((_tmp303.f4=excl,((_tmp303.f5=merge_f,((_tmp303.f6=t,((_tmp303.f7=msg,
_tmp303)))))))))))));((void(*)(void(*f)(struct _tuple22*,struct _dyneither_ptr*,
void*),struct _tuple22*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_dict_f,&
_tmp17E,dic2);if(_tmp17E.f1){struct Cyc_Dict_Dict*_tmp304;return(_tmp304=
_cycalloc(sizeof(*_tmp304)),((_tmp304[0]=_tmp17E.f2,_tmp304)));}return 0;}struct
Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(struct Cyc_Interface_Ienv*ie1,
struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*iexcl,struct _dyneither_ptr*
msg);struct Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(struct Cyc_Interface_Ienv*
ie1,struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*iexcl,struct
_dyneither_ptr*msg){const char*_tmp305;struct Cyc_Dict_Dict*_tmp181=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct
Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,
struct Cyc_Position_Segment*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct
_dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->aggrdecls,ie2->aggrdecls,
iexcl->aggrdecls,Cyc_Tcdecl_merge_aggrdecl,((_tmp305="type",_tag_dyneither(
_tmp305,sizeof(char),5))),msg);const char*_tmp306;struct Cyc_Dict_Dict*_tmp182=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct
Cyc_Dict_Dict excl,struct Cyc_Absyn_Datatypedecl*(*merge_f)(struct Cyc_Absyn_Datatypedecl*,
struct Cyc_Absyn_Datatypedecl*,struct Cyc_Position_Segment*,struct _dyneither_ptr*),
struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->datatypedecls,ie2->datatypedecls,iexcl->datatypedecls,Cyc_Tcdecl_merge_datatypedecl,((
_tmp306="datatype",_tag_dyneither(_tmp306,sizeof(char),9))),msg);const char*
_tmp307;struct Cyc_Dict_Dict*_tmp183=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Enumdecl*(*
merge_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->enumdecls,ie2->enumdecls,iexcl->enumdecls,Cyc_Tcdecl_merge_enumdecl,((
_tmp307="enum",_tag_dyneither(_tmp307,sizeof(char),5))),msg);const char*_tmp308;
struct Cyc_Dict_Dict*_tmp184=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,
struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*
merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls,ie2->typedefdecls,iexcl->typedefdecls,Cyc_Tcdecl_merge_typedefdecl,((
_tmp308="typedef",_tag_dyneither(_tmp308,sizeof(char),8))),msg);const char*
_tmp309;struct Cyc_Dict_Dict*_tmp185=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Vardecl*(*
merge_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->vardecls,ie2->vardecls,iexcl->vardecls,Cyc_Tcdecl_merge_vardecl,((_tmp309="variable",
_tag_dyneither(_tmp309,sizeof(char),9))),msg);const char*_tmp30A;struct Cyc_Dict_Dict*
_tmp186=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct Cyc_Dict_Dict excl,struct Cyc_Tcdecl_Xdatatypefielddecl*(*merge_f)(
struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct
Cyc_Position_Segment*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct
_dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->xdatatypefielddecls,
ie2->xdatatypefielddecls,iexcl->xdatatypefielddecls,Cyc_Tcdecl_merge_xdatatypefielddecl,((
_tmp30A="xdatatypefield",_tag_dyneither(_tmp30A,sizeof(char),15))),msg);if(((((!((
unsigned int)_tmp181) || !((unsigned int)_tmp182)) || !((unsigned int)_tmp183))
 || !((unsigned int)_tmp184)) || !((unsigned int)_tmp185)) || !((unsigned int)
_tmp186))return 0;{struct Cyc_Interface_Ienv*_tmp30B;return(_tmp30B=_cycalloc(
sizeof(*_tmp30B)),((_tmp30B->aggrdecls=*_tmp181,((_tmp30B->datatypedecls=*
_tmp182,((_tmp30B->enumdecls=*_tmp183,((_tmp30B->typedefdecls=*_tmp184,((_tmp30B->vardecls=*
_tmp185,((_tmp30B->xdatatypefielddecls=*_tmp186,_tmp30B)))))))))))));}}struct
_tuple23{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct Cyc_Dict_Dict f4;
struct _dyneither_ptr*f5;};void Cyc_Interface_compat_merge_ns_f(struct _tuple23*env,
struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2);void Cyc_Interface_compat_merge_ns_f(
struct _tuple23*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2){struct
_tuple23 _tmp18F;int _tmp190;int*_tmp191;struct Cyc_Dict_Dict _tmp192;struct Cyc_Dict_Dict*
_tmp193;struct Cyc_Dict_Dict _tmp194;struct Cyc_Dict_Dict _tmp195;struct
_dyneither_ptr*_tmp196;struct _tuple23*_tmp18E=env;_tmp18F=*_tmp18E;_tmp190=
_tmp18F.f1;_tmp191=(int*)&(*_tmp18E).f1;_tmp192=_tmp18F.f2;_tmp193=(struct Cyc_Dict_Dict*)&(*
_tmp18E).f2;_tmp194=_tmp18F.f3;_tmp195=_tmp18F.f4;_tmp196=_tmp18F.f5;{struct Cyc_Interface_Ienv*
ie1;struct Cyc_Interface_Ienv*iexcl;{struct _handler_cons _tmp197;_push_handler(&
_tmp197);{int _tmp199=0;if(setjmp(_tmp197.handler))_tmp199=1;if(!_tmp199){iexcl=((
struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
_tmp195,ns);;_pop_handler();}else{void*_tmp198=(void*)_exn_thrown;void*_tmp19B=
_tmp198;_LLA2: if(_tmp19B != Cyc_Dict_Absent)goto _LLA4;_LLA3: iexcl=Cyc_Interface_lazy_new_ienv();
goto _LLA1;_LLA4:;_LLA5:(void)_throw(_tmp19B);_LLA1:;}}}{struct _handler_cons
_tmp19C;_push_handler(& _tmp19C);{int _tmp19E=0;if(setjmp(_tmp19C.handler))_tmp19E=
1;if(!_tmp19E){ie1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(_tmp194,ns);{struct Cyc_Interface_Ienv*_tmp19F=Cyc_Interface_compat_merge_ienv(
Cyc_Interface_lazy_new_ienv(),ie1,iexcl,_tmp196);if(_tmp19F == 0){*_tmp191=0;
_npop_handler(0);return;}ie1=(struct Cyc_Interface_Ienv*)_tmp19F;};_pop_handler();}
else{void*_tmp19D=(void*)_exn_thrown;void*_tmp1A1=_tmp19D;_LLA7: if(_tmp1A1 != Cyc_Dict_Absent)
goto _LLA9;_LLA8: ie1=Cyc_Interface_lazy_new_ienv();goto _LLA6;_LLA9:;_LLAA:(void)
_throw(_tmp1A1);_LLA6:;}}}{struct Cyc_Interface_Ienv*_tmp1A2=Cyc_Interface_compat_merge_ienv(
ie1,ie2,iexcl,_tmp196);if(_tmp1A2 == 0)*_tmp191=0;else{if(*_tmp191)*_tmp193=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*
v))Cyc_Dict_insert)(*_tmp193,ns,(struct Cyc_Interface_Ienv*)_tmp1A2);}}}}struct
Cyc_Dict_Dict*Cyc_Interface_compat_merge_ns(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct Cyc_Dict_Dict excl,struct _dyneither_ptr*msg);struct Cyc_Dict_Dict*Cyc_Interface_compat_merge_ns(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct
_dyneither_ptr*msg){struct Cyc_Dict_Dict d=((struct Cyc_Dict_Dict(*)(int(*cmp)(
struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp);
struct _tuple23 _tmp30C;struct _tuple23 _tmp1A3=(_tmp30C.f1=1,((_tmp30C.f2=d,((
_tmp30C.f3=dic1,((_tmp30C.f4=excl,((_tmp30C.f5=msg,_tmp30C)))))))));((void(*)(
void(*f)(struct _tuple23*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct
_tuple23*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_ns_f,&
_tmp1A3,dic2);if(_tmp1A3.f1){struct Cyc_Dict_Dict*_tmp30D;return(_tmp30D=
_cycalloc(sizeof(*_tmp30D)),((_tmp30D[0]=_tmp1A3.f2,_tmp30D)));}return 0;}struct
_tuple24{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct _dyneither_ptr
f4;struct _dyneither_ptr*f5;};void Cyc_Interface_disj_merge_dict_f(struct _tuple24*
env,struct _dyneither_ptr*x,void*y);void Cyc_Interface_disj_merge_dict_f(struct
_tuple24*env,struct _dyneither_ptr*x,void*y){struct _tuple24 _tmp1A7;int _tmp1A8;int*
_tmp1A9;struct Cyc_Dict_Dict _tmp1AA;struct Cyc_Dict_Dict*_tmp1AB;struct Cyc_Dict_Dict
_tmp1AC;struct _dyneither_ptr _tmp1AD;struct _dyneither_ptr*_tmp1AE;struct _tuple24*
_tmp1A6=env;_tmp1A7=*_tmp1A6;_tmp1A8=_tmp1A7.f1;_tmp1A9=(int*)&(*_tmp1A6).f1;
_tmp1AA=_tmp1A7.f2;_tmp1AB=(struct Cyc_Dict_Dict*)&(*_tmp1A6).f2;_tmp1AC=_tmp1A7.f3;
_tmp1AD=_tmp1A7.f4;_tmp1AE=_tmp1A7.f5;if(((int(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_member)(_tmp1AC,x)){{const char*_tmp312;void*_tmp311[2];
struct Cyc_String_pa_struct _tmp310;struct Cyc_String_pa_struct _tmp30F;Cyc_Interface_check_err(
_tmp1AE,(struct _dyneither_ptr)((_tmp30F.tag=0,((_tmp30F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*x),((_tmp310.tag=0,((_tmp310.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp1AD),((_tmp311[0]=& _tmp310,((_tmp311[1]=& _tmp30F,Cyc_aprintf(((
_tmp312="%s %s is exported more than once",_tag_dyneither(_tmp312,sizeof(char),
33))),_tag_dyneither(_tmp311,sizeof(void*),2)))))))))))))));}*_tmp1A9=0;}else{
if(*_tmp1A9)*_tmp1AB=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,void*v))Cyc_Dict_insert)(*_tmp1AB,x,y);}}struct Cyc_Dict_Dict*
Cyc_Interface_disj_merge_dict(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,
struct _dyneither_ptr t,struct _dyneither_ptr*msg);struct Cyc_Dict_Dict*Cyc_Interface_disj_merge_dict(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct
_dyneither_ptr*msg){struct _tuple24 _tmp313;struct _tuple24 _tmp1B3=(_tmp313.f1=1,((
_tmp313.f2=dic1,((_tmp313.f3=dic1,((_tmp313.f4=t,((_tmp313.f5=msg,_tmp313)))))))));((
void(*)(void(*f)(struct _tuple24*,struct _dyneither_ptr*,void*),struct _tuple24*env,
struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_disj_merge_dict_f,& _tmp1B3,
dic2);if(_tmp1B3.f1){struct Cyc_Dict_Dict*_tmp314;return(_tmp314=_cycalloc(
sizeof(*_tmp314)),((_tmp314[0]=_tmp1B3.f2,_tmp314)));}return 0;}struct Cyc_Interface_Ienv*
Cyc_Interface_disj_merge_ienv(struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*
ie2,struct _dyneither_ptr*msg);struct Cyc_Interface_Ienv*Cyc_Interface_disj_merge_ienv(
struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct _dyneither_ptr*
msg){const char*_tmp315;struct Cyc_Dict_Dict*_tmp1B6=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct
Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,
struct Cyc_Position_Segment*,struct _dyneither_ptr*),struct _dyneither_ptr t,struct
_dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(ie1->aggrdecls,ie2->aggrdecls,((
struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_aggrdecl,((_tmp315="type",
_tag_dyneither(_tmp315,sizeof(char),5))),msg);const char*_tmp316;struct Cyc_Dict_Dict*
_tmp1B7=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(
ie1->datatypedecls,ie2->datatypedecls,((_tmp316="datatype",_tag_dyneither(
_tmp316,sizeof(char),9))),msg);const char*_tmp317;struct Cyc_Dict_Dict*_tmp1B8=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct
_dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->enumdecls,
ie2->enumdecls,((_tmp317="enum",_tag_dyneither(_tmp317,sizeof(char),5))),msg);
const char*_tmp318;struct Cyc_Dict_Dict*_tmp1B9=((struct Cyc_Dict_Dict*(*)(struct
Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct Cyc_Dict_Dict excl,struct Cyc_Absyn_Typedefdecl*(*
merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,
struct _dyneither_ptr*),struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls,ie2->typedefdecls,((struct Cyc_Dict_Dict(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp),Cyc_Tcdecl_merge_typedefdecl,((
_tmp318="typedef",_tag_dyneither(_tmp318,sizeof(char),8))),msg);const char*
_tmp319;struct Cyc_Dict_Dict*_tmp1BA=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict
dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct _dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(
ie1->vardecls,ie2->vardecls,((_tmp319="variable",_tag_dyneither(_tmp319,sizeof(
char),9))),msg);const char*_tmp31A;struct Cyc_Dict_Dict*_tmp1BB=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr t,struct
_dyneither_ptr*msg))Cyc_Interface_disj_merge_dict)(ie1->xdatatypefielddecls,ie2->xdatatypefielddecls,((
_tmp31A="xdatatypefield",_tag_dyneither(_tmp31A,sizeof(char),15))),msg);if(((((!((
unsigned int)_tmp1B6) || !((unsigned int)_tmp1B7)) || !((unsigned int)_tmp1B8))
 || !((unsigned int)_tmp1B9)) || !((unsigned int)_tmp1BA)) || !((unsigned int)
_tmp1BB))return 0;{struct Cyc_Interface_Ienv*_tmp31B;return(_tmp31B=_cycalloc(
sizeof(*_tmp31B)),((_tmp31B->aggrdecls=*_tmp1B6,((_tmp31B->datatypedecls=*
_tmp1B7,((_tmp31B->enumdecls=*_tmp1B8,((_tmp31B->typedefdecls=*_tmp1B9,((_tmp31B->vardecls=*
_tmp1BA,((_tmp31B->xdatatypefielddecls=*_tmp1BB,_tmp31B)))))))))))));}}struct
_tuple25{int f1;struct Cyc_Dict_Dict f2;struct Cyc_Dict_Dict f3;struct _dyneither_ptr*
f4;};void Cyc_Interface_disj_merge_ns_f(struct _tuple25*env,struct Cyc_List_List*ns,
struct Cyc_Interface_Ienv*ie2);void Cyc_Interface_disj_merge_ns_f(struct _tuple25*
env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2){struct _tuple25 _tmp1C4;
int _tmp1C5;int*_tmp1C6;struct Cyc_Dict_Dict _tmp1C7;struct Cyc_Dict_Dict*_tmp1C8;
struct Cyc_Dict_Dict _tmp1C9;struct _dyneither_ptr*_tmp1CA;struct _tuple25*_tmp1C3=
env;_tmp1C4=*_tmp1C3;_tmp1C5=_tmp1C4.f1;_tmp1C6=(int*)&(*_tmp1C3).f1;_tmp1C7=
_tmp1C4.f2;_tmp1C8=(struct Cyc_Dict_Dict*)&(*_tmp1C3).f2;_tmp1C9=_tmp1C4.f3;
_tmp1CA=_tmp1C4.f4;{struct Cyc_Interface_Ienv*ie1;{struct _handler_cons _tmp1CB;
_push_handler(& _tmp1CB);{int _tmp1CD=0;if(setjmp(_tmp1CB.handler))_tmp1CD=1;if(!
_tmp1CD){ie1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(_tmp1C9,ns);;_pop_handler();}else{void*_tmp1CC=(void*)
_exn_thrown;void*_tmp1CF=_tmp1CC;_LLAC: if(_tmp1CF != Cyc_Dict_Absent)goto _LLAE;
_LLAD: ie1=Cyc_Interface_lazy_new_ienv();goto _LLAB;_LLAE:;_LLAF:(void)_throw(
_tmp1CF);_LLAB:;}}}{struct Cyc_Interface_Ienv*_tmp1D0=Cyc_Interface_disj_merge_ienv(
ie1,ie2,_tmp1CA);if(_tmp1D0 == 0)*_tmp1C6=0;else{if(*_tmp1C6)*_tmp1C8=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*
_tmp1C8,ns,(struct Cyc_Interface_Ienv*)_tmp1D0);}}}}struct Cyc_Dict_Dict*Cyc_Interface_disj_merge_ns(
struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict dic2,struct _dyneither_ptr*msg);struct
Cyc_Dict_Dict*Cyc_Interface_disj_merge_ns(struct Cyc_Dict_Dict dic1,struct Cyc_Dict_Dict
dic2,struct _dyneither_ptr*msg){struct _tuple25 _tmp31C;struct _tuple25 _tmp1D1=(
_tmp31C.f1=1,((_tmp31C.f2=dic1,((_tmp31C.f3=dic1,((_tmp31C.f4=msg,_tmp31C)))))));((
void(*)(void(*f)(struct _tuple25*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct _tuple25*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_disj_merge_ns_f,&
_tmp1D1,dic2);if(_tmp1D1.f1){struct Cyc_Dict_Dict*_tmp31D;return(_tmp31D=
_cycalloc(sizeof(*_tmp31D)),((_tmp31D[0]=_tmp1D1.f2,_tmp31D)));}return 0;}struct
Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*
i2,struct _tuple10*info);struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*
i1,struct Cyc_Interface_I*i2,struct _tuple10*info){struct _handler_cons _tmp1D4;
_push_handler(& _tmp1D4);{int _tmp1D6=0;if(setjmp(_tmp1D4.handler))_tmp1D6=1;if(!
_tmp1D6){{struct _dyneither_ptr*msg=0;if(info != 0){struct Cyc_String_pa_struct
_tmp327;struct Cyc_String_pa_struct _tmp326;void*_tmp325[2];const char*_tmp324;
struct _dyneither_ptr*_tmp323;msg=((_tmp323=_cycalloc(sizeof(*_tmp323)),((_tmp323[
0]=(struct _dyneither_ptr)((_tmp327.tag=0,((_tmp327.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)(*info).f2),((_tmp326.tag=0,((_tmp326.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((_tmp325[0]=& _tmp326,((
_tmp325[1]=& _tmp327,Cyc_aprintf(((_tmp324="merging exports of %s and %s,",
_tag_dyneither(_tmp324,sizeof(char),30))),_tag_dyneither(_tmp325,sizeof(void*),2)))))))))))))),
_tmp323))));}{struct Cyc_Dict_Dict*_tmp1DC=Cyc_Interface_disj_merge_ns(i1->exports,
i2->exports,msg);if(_tmp1DC == 0){struct Cyc_Interface_I*_tmp1DD=0;_npop_handler(0);
return _tmp1DD;}if(info != 0){struct Cyc_String_pa_struct _tmp331;struct Cyc_String_pa_struct
_tmp330;void*_tmp32F[2];const char*_tmp32E;struct _dyneither_ptr*_tmp32D;msg=((
_tmp32D=_cycalloc(sizeof(*_tmp32D)),((_tmp32D[0]=(struct _dyneither_ptr)((_tmp331.tag=
0,((_tmp331.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f2),((
_tmp330.tag=0,((_tmp330.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(*info).f1),((
_tmp32F[0]=& _tmp330,((_tmp32F[1]=& _tmp331,Cyc_aprintf(((_tmp32E="merging imports of %s and %s,",
_tag_dyneither(_tmp32E,sizeof(char),30))),_tag_dyneither(_tmp32F,sizeof(void*),2)))))))))))))),
_tmp32D))));}{struct Cyc_Dict_Dict*_tmp1E3=Cyc_Interface_compat_merge_ns(i1->imports,
i2->imports,*_tmp1DC,msg);if(_tmp1E3 == 0){struct Cyc_Interface_I*_tmp1E4=0;
_npop_handler(0);return _tmp1E4;}{struct Cyc_Interface_I*_tmp332;struct Cyc_Interface_I*
_tmp1E6=(_tmp332=_cycalloc(sizeof(*_tmp332)),((_tmp332->imports=*_tmp1E3,((
_tmp332->exports=*_tmp1DC,_tmp332)))));_npop_handler(0);return _tmp1E6;}}}};
_pop_handler();}else{void*_tmp1D5=(void*)_exn_thrown;void*_tmp1E8=_tmp1D5;_LLB1:
if(_tmp1E8 != Cyc_Tcdecl_Incompatible)goto _LLB3;_LLB2: return 0;_LLB3:;_LLB4:(void)
_throw(_tmp1E8);_LLB0:;}}}struct Cyc_Interface_I*Cyc_Interface_merge_list(struct
Cyc_List_List*li,struct Cyc_List_List*linfo);struct Cyc_Interface_I*Cyc_Interface_merge_list(
struct Cyc_List_List*li,struct Cyc_List_List*linfo){if(li == 0)return(struct Cyc_Interface_I*)
Cyc_Interface_empty();{struct Cyc_Interface_I*curr_i=(struct Cyc_Interface_I*)li->hd;
struct _dyneither_ptr*curr_info=linfo != 0?(struct _dyneither_ptr*)((struct
_dyneither_ptr*)linfo->hd): 0;li=li->tl;if(linfo != 0)linfo=linfo->tl;for(0;li != 0;
li=li->tl){struct _tuple10*_tmp333;struct Cyc_Interface_I*_tmp1E9=Cyc_Interface_merge(
curr_i,(struct Cyc_Interface_I*)li->hd,curr_info != 0  && linfo != 0?(_tmp333=
_cycalloc(sizeof(*_tmp333)),((_tmp333->f1=*curr_info,((_tmp333->f2=*((struct
_dyneither_ptr*)linfo->hd),_tmp333))))): 0);if(_tmp1E9 == 0)return 0;curr_i=(struct
Cyc_Interface_I*)_tmp1E9;if(curr_info != 0){struct Cyc_String_pa_struct _tmp33D;
struct Cyc_String_pa_struct _tmp33C;void*_tmp33B[2];const char*_tmp33A;struct
_dyneither_ptr*_tmp339;curr_info=linfo != 0?(_tmp339=_cycalloc(sizeof(*_tmp339)),((
_tmp339[0]=(struct _dyneither_ptr)((_tmp33D.tag=0,((_tmp33D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)linfo->hd)),((
_tmp33C.tag=0,((_tmp33C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
curr_info),((_tmp33B[0]=& _tmp33C,((_tmp33B[1]=& _tmp33D,Cyc_aprintf(((_tmp33A="%s+%s",
_tag_dyneither(_tmp33A,sizeof(char),6))),_tag_dyneither(_tmp33B,sizeof(void*),2)))))))))))))),
_tmp339))): 0;}if(linfo != 0)linfo=linfo->tl;}return(struct Cyc_Interface_I*)curr_i;}}
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*
get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo);struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*
la,struct Cyc_List_List*linfo){if(la == 0)return(struct Cyc_Interface_I*)Cyc_Interface_empty();{
struct Cyc_Interface_I*curr_i=get((void*)la->hd);struct _dyneither_ptr*curr_info=
linfo != 0?(struct _dyneither_ptr*)((struct _dyneither_ptr*)linfo->hd): 0;la=la->tl;
if(linfo != 0)linfo=linfo->tl;for(0;la != 0;la=la->tl){struct _tuple10*_tmp33E;
struct Cyc_Interface_I*_tmp1F0=Cyc_Interface_merge(curr_i,get((void*)la->hd),
curr_info != 0  && linfo != 0?(_tmp33E=_cycalloc(sizeof(*_tmp33E)),((_tmp33E->f1=*
curr_info,((_tmp33E->f2=*((struct _dyneither_ptr*)linfo->hd),_tmp33E))))): 0);if(
_tmp1F0 == 0)return 0;curr_i=(struct Cyc_Interface_I*)_tmp1F0;if(curr_info != 0){
struct Cyc_String_pa_struct _tmp348;struct Cyc_String_pa_struct _tmp347;void*_tmp346[
2];const char*_tmp345;struct _dyneither_ptr*_tmp344;curr_info=linfo != 0?(_tmp344=
_cycalloc(sizeof(*_tmp344)),((_tmp344[0]=(struct _dyneither_ptr)((_tmp348.tag=0,((
_tmp348.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)
linfo->hd)),((_tmp347.tag=0,((_tmp347.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*curr_info),((_tmp346[0]=& _tmp347,((_tmp346[1]=& _tmp348,Cyc_aprintf(((
_tmp345="%s+%s",_tag_dyneither(_tmp345,sizeof(char),6))),_tag_dyneither(_tmp346,
sizeof(void*),2)))))))))))))),_tmp344))): 0;}if(linfo != 0)linfo=linfo->tl;}return(
struct Cyc_Interface_I*)curr_i;}}static struct Cyc_List_List*Cyc_Interface_add_namespace(
struct Cyc_List_List*tds,struct Cyc_List_List*ns);static struct Cyc_List_List*Cyc_Interface_add_namespace(
struct Cyc_List_List*tds,struct Cyc_List_List*ns){if(ns == 0)return tds;{struct Cyc_Absyn_Namespace_d_struct*
_tmp34E;struct Cyc_Absyn_Namespace_d_struct _tmp34D;struct Cyc_List_List*_tmp34C;
return(_tmp34C=_cycalloc(sizeof(*_tmp34C)),((_tmp34C->hd=Cyc_Absyn_new_decl((
void*)((_tmp34E=_cycalloc(sizeof(*_tmp34E)),((_tmp34E[0]=((_tmp34D.tag=10,((
_tmp34D.f1=(struct _dyneither_ptr*)ns->hd,((_tmp34D.f2=Cyc_Interface_add_namespace(
tds,ns->tl),_tmp34D)))))),_tmp34E)))),0),((_tmp34C->tl=0,_tmp34C)))));}}static
struct Cyc_List_List*Cyc_Interface_add_aggrdecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl*
d,struct Cyc_List_List*tds);static struct Cyc_List_List*Cyc_Interface_add_aggrdecl(
struct _dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){
struct Cyc_Absyn_Aggr_d_struct*_tmp354;struct Cyc_Absyn_Aggr_d_struct _tmp353;
struct Cyc_List_List*_tmp352;return(_tmp352=_cycalloc(sizeof(*_tmp352)),((_tmp352->hd=
Cyc_Absyn_new_decl((void*)((_tmp354=_cycalloc(sizeof(*_tmp354)),((_tmp354[0]=((
_tmp353.tag=6,((_tmp353.f1=d,_tmp353)))),_tmp354)))),0),((_tmp352->tl=tds,
_tmp352)))));}static struct Cyc_List_List*Cyc_Interface_add_aggrdecl_header(struct
_dyneither_ptr*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds);static
struct Cyc_List_List*Cyc_Interface_add_aggrdecl_header(struct _dyneither_ptr*x,
struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){{struct Cyc_Absyn_Aggrdecl*
_tmp355;d=((_tmp355=_cycalloc(sizeof(*_tmp355)),((_tmp355[0]=*d,_tmp355))));}d->impl=
0;if(d->sc != (void*)4)d->sc=(void*)3;{struct Cyc_Absyn_Aggr_d_struct*_tmp35B;
struct Cyc_Absyn_Aggr_d_struct _tmp35A;struct Cyc_List_List*_tmp359;return(_tmp359=
_cycalloc(sizeof(*_tmp359)),((_tmp359->hd=Cyc_Absyn_new_decl((void*)((_tmp35B=
_cycalloc(sizeof(*_tmp35B)),((_tmp35B[0]=((_tmp35A.tag=6,((_tmp35A.f1=d,_tmp35A)))),
_tmp35B)))),0),((_tmp359->tl=tds,_tmp359)))));}}static struct Cyc_List_List*Cyc_Interface_add_datatypedecl(
struct _dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds);
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl(struct _dyneither_ptr*x,
struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){struct Cyc_Absyn_Datatype_d_struct*
_tmp361;struct Cyc_Absyn_Datatype_d_struct _tmp360;struct Cyc_List_List*_tmp35F;
return(_tmp35F=_cycalloc(sizeof(*_tmp35F)),((_tmp35F->hd=Cyc_Absyn_new_decl((
void*)((_tmp361=_cycalloc(sizeof(*_tmp361)),((_tmp361[0]=((_tmp360.tag=7,((
_tmp360.f1=d,_tmp360)))),_tmp361)))),0),((_tmp35F->tl=tds,_tmp35F)))));}static
char _tmp204[2]="_";static struct _dyneither_ptr Cyc_Interface_us={_tmp204,_tmp204,
_tmp204 + 2};static struct _dyneither_ptr*Cyc_Interface_us_p=& Cyc_Interface_us;
static struct _tuple17*Cyc_Interface_rewrite_datatypefield_type(struct _tuple17*x);
static struct _tuple17*Cyc_Interface_rewrite_datatypefield_type(struct _tuple17*x){
struct Cyc_Absyn_VarType_struct*_tmp36B;struct Cyc_Absyn_Tvar*_tmp36A;struct Cyc_Absyn_VarType_struct
_tmp369;struct _tuple17*_tmp368;return(_tmp368=_cycalloc(sizeof(*_tmp368)),((
_tmp368->f1=Cyc_Absyn_empty_tqual(0),((_tmp368->f2=(void*)((_tmp36B=_cycalloc(
sizeof(*_tmp36B)),((_tmp36B[0]=((_tmp369.tag=1,((_tmp369.f1=((_tmp36A=_cycalloc(
sizeof(*_tmp36A)),((_tmp36A->name=Cyc_Interface_us_p,((_tmp36A->identity=- 1,((
_tmp36A->kind=Cyc_Tcutil_kind_to_bound((void*)2),_tmp36A)))))))),_tmp369)))),
_tmp36B)))),_tmp368)))));}static struct Cyc_Absyn_Datatypefield*Cyc_Interface_rewrite_datatypefield(
struct Cyc_Absyn_Datatypefield*f);static struct Cyc_Absyn_Datatypefield*Cyc_Interface_rewrite_datatypefield(
struct Cyc_Absyn_Datatypefield*f){{struct Cyc_Absyn_Datatypefield*_tmp36C;f=((
_tmp36C=_cycalloc(sizeof(*_tmp36C)),((_tmp36C[0]=*f,_tmp36C))));}f->typs=((
struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct _tuple17*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield_type,f->typs);return f;}
static struct Cyc_List_List*Cyc_Interface_add_datatypedecl_header(struct
_dyneither_ptr*x,struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds);static
struct Cyc_List_List*Cyc_Interface_add_datatypedecl_header(struct _dyneither_ptr*x,
struct Cyc_Absyn_Datatypedecl*d,struct Cyc_List_List*tds){{struct Cyc_Absyn_Datatypedecl*
_tmp36D;d=((_tmp36D=_cycalloc(sizeof(*_tmp36D)),((_tmp36D[0]=*d,_tmp36D))));}if(
d->fields != 0){struct Cyc_Core_Opt*_tmp36E;d->fields=((_tmp36E=_cycalloc(sizeof(*
_tmp36E)),((_tmp36E->v=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Datatypefield*(*
f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_datatypefield,(
struct Cyc_List_List*)(d->fields)->v),_tmp36E))));}if(d->sc != (void*)4)d->sc=(
void*)3;{struct Cyc_Absyn_Datatype_d_struct*_tmp374;struct Cyc_Absyn_Datatype_d_struct
_tmp373;struct Cyc_List_List*_tmp372;return(_tmp372=_cycalloc(sizeof(*_tmp372)),((
_tmp372->hd=Cyc_Absyn_new_decl((void*)((_tmp374=_cycalloc(sizeof(*_tmp374)),((
_tmp374[0]=((_tmp373.tag=7,((_tmp373.f1=d,_tmp373)))),_tmp374)))),0),((_tmp372->tl=
tds,_tmp372)))));}}static struct Cyc_List_List*Cyc_Interface_add_enumdecl(struct
_dyneither_ptr*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds);static
struct Cyc_List_List*Cyc_Interface_add_enumdecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Enumdecl*
d,struct Cyc_List_List*tds){struct Cyc_Absyn_Enum_d_struct*_tmp37A;struct Cyc_Absyn_Enum_d_struct
_tmp379;struct Cyc_List_List*_tmp378;return(_tmp378=_cycalloc(sizeof(*_tmp378)),((
_tmp378->hd=Cyc_Absyn_new_decl((void*)((_tmp37A=_cycalloc(sizeof(*_tmp37A)),((
_tmp37A[0]=((_tmp379.tag=8,((_tmp379.f1=d,_tmp379)))),_tmp37A)))),0),((_tmp378->tl=
tds,_tmp378)))));}static struct Cyc_List_List*Cyc_Interface_add_enumdecl_header(
struct _dyneither_ptr*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds);
static struct Cyc_List_List*Cyc_Interface_add_enumdecl_header(struct _dyneither_ptr*
x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){{struct Cyc_Absyn_Enumdecl*
_tmp37B;d=((_tmp37B=_cycalloc(sizeof(*_tmp37B)),((_tmp37B[0]=*d,_tmp37B))));}d->fields=
0;if(d->sc != (void*)4)d->sc=(void*)3;{struct Cyc_Absyn_Enum_d_struct*_tmp381;
struct Cyc_Absyn_Enum_d_struct _tmp380;struct Cyc_List_List*_tmp37F;return(_tmp37F=
_cycalloc(sizeof(*_tmp37F)),((_tmp37F->hd=Cyc_Absyn_new_decl((void*)((_tmp381=
_cycalloc(sizeof(*_tmp381)),((_tmp381[0]=((_tmp380.tag=8,((_tmp380.f1=d,_tmp380)))),
_tmp381)))),0),((_tmp37F->tl=tds,_tmp37F)))));}}static struct Cyc_List_List*Cyc_Interface_add_typedef(
struct _dyneither_ptr*x,struct Cyc_Absyn_Typedefdecl*d,struct Cyc_List_List*tds);
static struct Cyc_List_List*Cyc_Interface_add_typedef(struct _dyneither_ptr*x,
struct Cyc_Absyn_Typedefdecl*d,struct Cyc_List_List*tds){struct Cyc_Absyn_Typedef_d_struct*
_tmp387;struct Cyc_Absyn_Typedef_d_struct _tmp386;struct Cyc_List_List*_tmp385;
return(_tmp385=_cycalloc(sizeof(*_tmp385)),((_tmp385->hd=Cyc_Absyn_new_decl((
void*)((_tmp387=_cycalloc(sizeof(*_tmp387)),((_tmp387[0]=((_tmp386.tag=9,((
_tmp386.f1=d,_tmp386)))),_tmp387)))),0),((_tmp385->tl=tds,_tmp385)))));}static
struct Cyc_List_List*Cyc_Interface_add_vardecl(struct _dyneither_ptr*x,struct Cyc_Absyn_Vardecl*
d,struct Cyc_List_List*tds);static struct Cyc_List_List*Cyc_Interface_add_vardecl(
struct _dyneither_ptr*x,struct Cyc_Absyn_Vardecl*d,struct Cyc_List_List*tds){struct
Cyc_Absyn_Var_d_struct*_tmp38D;struct Cyc_Absyn_Var_d_struct _tmp38C;struct Cyc_List_List*
_tmp38B;return(_tmp38B=_cycalloc(sizeof(*_tmp38B)),((_tmp38B->hd=Cyc_Absyn_new_decl((
void*)((_tmp38D=_cycalloc(sizeof(*_tmp38D)),((_tmp38D[0]=((_tmp38C.tag=0,((
_tmp38C.f1=d,_tmp38C)))),_tmp38D)))),0),((_tmp38B->tl=tds,_tmp38B)))));}static
struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl(struct _dyneither_ptr*x,
struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*tds);static struct Cyc_List_List*
Cyc_Interface_add_xdatatypefielddecl(struct _dyneither_ptr*x,struct Cyc_Tcdecl_Xdatatypefielddecl*
d,struct Cyc_List_List*tds){struct Cyc_Tcdecl_Xdatatypefielddecl _tmp21D;struct Cyc_Absyn_Datatypedecl*
_tmp21E;struct Cyc_Absyn_Datatypefield*_tmp21F;struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp21C=d;_tmp21D=*_tmp21C;_tmp21E=_tmp21D.base;_tmp21F=_tmp21D.field;{struct Cyc_Absyn_Datatypedecl*
_tmp38E;_tmp21E=((_tmp38E=_cycalloc(sizeof(*_tmp38E)),((_tmp38E[0]=*_tmp21E,
_tmp38E))));}{struct Cyc_List_List*_tmp391;struct Cyc_Core_Opt*_tmp390;_tmp21E->fields=((
_tmp390=_cycalloc(sizeof(*_tmp390)),((_tmp390->v=((_tmp391=_cycalloc(sizeof(*
_tmp391)),((_tmp391->hd=_tmp21F,((_tmp391->tl=0,_tmp391)))))),_tmp390))));}
_tmp21E->sc=(void*)3;{struct Cyc_Absyn_Datatype_d_struct*_tmp397;struct Cyc_Absyn_Datatype_d_struct
_tmp396;struct Cyc_List_List*_tmp395;return(_tmp395=_cycalloc(sizeof(*_tmp395)),((
_tmp395->hd=Cyc_Absyn_new_decl((void*)((_tmp397=_cycalloc(sizeof(*_tmp397)),((
_tmp397[0]=((_tmp396.tag=7,((_tmp396.f1=_tmp21E,_tmp396)))),_tmp397)))),0),((
_tmp395->tl=tds,_tmp395)))));}}static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl_header(
struct _dyneither_ptr*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*
tds);static struct Cyc_List_List*Cyc_Interface_add_xdatatypefielddecl_header(
struct _dyneither_ptr*x,struct Cyc_Tcdecl_Xdatatypefielddecl*d,struct Cyc_List_List*
tds){struct Cyc_Tcdecl_Xdatatypefielddecl _tmp227;struct Cyc_Absyn_Datatypedecl*
_tmp228;struct Cyc_Absyn_Datatypefield*_tmp229;struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp226=d;_tmp227=*_tmp226;_tmp228=_tmp227.base;_tmp229=_tmp227.field;{struct Cyc_Absyn_Datatypedecl*
_tmp398;_tmp228=((_tmp398=_cycalloc(sizeof(*_tmp398)),((_tmp398[0]=*_tmp228,
_tmp398))));}_tmp229=Cyc_Interface_rewrite_datatypefield(_tmp229);_tmp229->sc=(
void*)3;{struct Cyc_List_List*_tmp39B;struct Cyc_Core_Opt*_tmp39A;_tmp228->fields=((
_tmp39A=_cycalloc(sizeof(*_tmp39A)),((_tmp39A->v=((_tmp39B=_cycalloc(sizeof(*
_tmp39B)),((_tmp39B->hd=_tmp229,((_tmp39B->tl=0,_tmp39B)))))),_tmp39A))));}
_tmp228->sc=(void*)3;{struct Cyc_Absyn_Datatype_d_struct*_tmp3A1;struct Cyc_Absyn_Datatype_d_struct
_tmp3A0;struct Cyc_List_List*_tmp39F;return(_tmp39F=_cycalloc(sizeof(*_tmp39F)),((
_tmp39F->hd=Cyc_Absyn_new_decl((void*)((_tmp3A1=_cycalloc(sizeof(*_tmp3A1)),((
_tmp3A1[0]=((_tmp3A0.tag=7,((_tmp3A0.f1=_tmp228,_tmp3A0)))),_tmp3A1)))),0),((
_tmp39F->tl=tds,_tmp39F)))));}}static void Cyc_Interface_print_ns_headers(struct
Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie);static void
Cyc_Interface_print_ns_headers(struct Cyc___cycFILE*f,struct Cyc_List_List*ns,
struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*_tmp230=0;_tmp230=((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrdecl*,struct
Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_aggrdecl_header,ie->aggrdecls,_tmp230);_tmp230=((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Datatypedecl*,
struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_datatypedecl_header,ie->datatypedecls,_tmp230);_tmp230=((
struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Absyn_Enumdecl*,
struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_enumdecl_header,ie->enumdecls,_tmp230);if(_tmp230 != 0){_tmp230=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp230);
_tmp230=Cyc_Interface_add_namespace(_tmp230,ns);Cyc_Absynpp_decllist2file(
_tmp230,f);}}static void Cyc_Interface_print_ns_xdatatypefielddecl_headers(struct
Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie);static void
Cyc_Interface_print_ns_xdatatypefielddecl_headers(struct Cyc___cycFILE*f,struct
Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*_tmp231=((
struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _dyneither_ptr*,struct Cyc_Tcdecl_Xdatatypefielddecl*,
struct Cyc_List_List*),struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_xdatatypefielddecl_header,ie->xdatatypefielddecls,0);if(
_tmp231 != 0){_tmp231=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp231);_tmp231=Cyc_Interface_add_namespace(_tmp231,ns);Cyc_Absynpp_decllist2file(
_tmp231,f);}}static void Cyc_Interface_print_ns_typedefs(struct Cyc___cycFILE*f,
struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie);static void Cyc_Interface_print_ns_typedefs(
struct Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct
Cyc_List_List*_tmp232=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_dyneither_ptr*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_typedef,ie->typedefdecls,
0);if(_tmp232 != 0){_tmp232=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp232);_tmp232=Cyc_Interface_add_namespace(_tmp232,ns);Cyc_Absynpp_decllist2file(
_tmp232,f);}}static void Cyc_Interface_print_ns_decls(struct Cyc___cycFILE*f,struct
Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie);static void Cyc_Interface_print_ns_decls(
struct Cyc___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct
Cyc_List_List*_tmp233=0;_tmp233=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*
f)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct
Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl,
ie->aggrdecls,_tmp233);_tmp233=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(
struct _dyneither_ptr*,struct Cyc_Absyn_Datatypedecl*,struct Cyc_List_List*),struct
Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_datatypedecl,
ie->datatypedecls,_tmp233);_tmp233=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*
f)(struct _dyneither_ptr*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct
Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl,
ie->enumdecls,_tmp233);_tmp233=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(
struct _dyneither_ptr*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_vardecl,ie->vardecls,
_tmp233);_tmp233=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_dyneither_ptr*,struct Cyc_Tcdecl_Xdatatypefielddecl*,struct Cyc_List_List*),
struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xdatatypefielddecl,
ie->xdatatypefielddecls,_tmp233);if(_tmp233 != 0){_tmp233=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp233);_tmp233=Cyc_Interface_add_namespace(
_tmp233,ns);Cyc_Absynpp_decllist2file(_tmp233,f);}}void Cyc_Interface_print(
struct Cyc_Interface_I*i,struct Cyc___cycFILE*f);void Cyc_Interface_print(struct Cyc_Interface_I*
i,struct Cyc___cycFILE*f){Cyc_Absynpp_set_params(& Cyc_Absynpp_cyci_params_r);{
const char*_tmp3A4;void*_tmp3A3;(_tmp3A3=0,Cyc_fprintf(f,((_tmp3A4="/****** needed (headers) ******/\n",
_tag_dyneither(_tmp3A4,sizeof(char),34))),_tag_dyneither(_tmp3A3,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,
f,i->imports);{const char*_tmp3A7;void*_tmp3A6;(_tmp3A6=0,Cyc_fprintf(f,((_tmp3A7="\n/****** provided (headers) ******/\n",
_tag_dyneither(_tmp3A7,sizeof(char),37))),_tag_dyneither(_tmp3A6,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,
f,i->exports);{const char*_tmp3AA;void*_tmp3A9;(_tmp3A9=0,Cyc_fprintf(f,((_tmp3AA="\n/****** needed (headers of xdatatypefielddecls) ******/\n",
_tag_dyneither(_tmp3AA,sizeof(char),58))),_tag_dyneither(_tmp3A9,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_xdatatypefielddecl_headers,
f,i->imports);{const char*_tmp3AD;void*_tmp3AC;(_tmp3AC=0,Cyc_fprintf(f,((_tmp3AD="\n/****** provided (headers of xdatatypefielddecls) ******/\n",
_tag_dyneither(_tmp3AD,sizeof(char),60))),_tag_dyneither(_tmp3AC,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_xdatatypefielddecl_headers,
f,i->exports);{const char*_tmp3B0;void*_tmp3AF;(_tmp3AF=0,Cyc_fprintf(f,((_tmp3B0="\n/****** provided (typedefs) ******/\n",
_tag_dyneither(_tmp3B0,sizeof(char),38))),_tag_dyneither(_tmp3AF,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_typedefs,
f,i->exports);{const char*_tmp3B3;void*_tmp3B2;(_tmp3B2=0,Cyc_fprintf(f,((_tmp3B3="\n/****** needed (declarations) ******/\n",
_tag_dyneither(_tmp3B3,sizeof(char),40))),_tag_dyneither(_tmp3B2,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,
f,i->imports);{const char*_tmp3B6;void*_tmp3B5;(_tmp3B5=0,Cyc_fprintf(f,((_tmp3B6="\n/****** provided (declarations) ******/\n",
_tag_dyneither(_tmp3B6,sizeof(char),42))),_tag_dyneither(_tmp3B5,sizeof(void*),0)));}((
void(*)(void(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct Cyc___cycFILE*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,
f,i->exports);}struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*f);
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*f){Cyc_Lex_lex_init();{
struct Cyc_List_List*_tmp242=Cyc_Parse_parse_file(f);struct _RegionHandle _tmp243=
_new_region("r");struct _RegionHandle*r=& _tmp243;_push_region(r);{struct Cyc_Tcenv_Tenv*
_tmp244=Cyc_Tcenv_tc_init(r);Cyc_Tc_tc(r,_tmp244,0,_tmp242);{struct Cyc_Interface_I*
_tmp245=Cyc_Interface_gen_extract(_tmp244->ae,0);_npop_handler(0);return _tmp245;}};
_pop_region(r);}}void Cyc_Interface_save(struct Cyc_Interface_I*i,struct Cyc___cycFILE*
f);void Cyc_Interface_save(struct Cyc_Interface_I*i,struct Cyc___cycFILE*f){Cyc_Interface_print(
i,f);}struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*f);struct Cyc_Interface_I*
Cyc_Interface_load(struct Cyc___cycFILE*f){return Cyc_Interface_parse(f);}
