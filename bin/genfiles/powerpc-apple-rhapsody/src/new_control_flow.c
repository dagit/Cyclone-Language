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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);int
Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[16];
extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct
_dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*,int(*cmp)(void*,void*),
struct Cyc_List_List*a,struct Cyc_List_List*b);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_rzip(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc___cycFILE;extern struct
Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
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
char*tag;struct _dyneither_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,
void**ans);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict
d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,
void*);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(
struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void**Cyc_Hashtable_lookup_opt(
struct Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dyneither_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};
extern char Cyc_Position_Nocontext[14];int Cyc_Position_error_p();struct Cyc_Absyn_Loc_n_struct{
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
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{
struct _tuple1*name;int is_extensible;int is_flat;};struct Cyc_Absyn_UnknownDatatype_struct{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo f1;};struct Cyc_Absyn_KnownDatatype_struct{
int tag;struct Cyc_Absyn_Datatypedecl**f1;};union Cyc_Absyn_DatatypeInfoU_union{
struct Cyc_Absyn_UnknownDatatype_struct UnknownDatatype;struct Cyc_Absyn_KnownDatatype_struct
KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU_union
datatype_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct
Cyc_Absyn_UnknownDatatypefield_struct{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
f1;};struct Cyc_Absyn_KnownDatatypefield_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;struct Cyc_Absyn_Datatypefield*f2;};union Cyc_Absyn_DatatypeFieldInfoU_union{
struct Cyc_Absyn_UnknownDatatypefield_struct UnknownDatatypefield;struct Cyc_Absyn_KnownDatatypefield_struct
KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple1*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
_dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple2{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple2*f1;struct
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
f1;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{
struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*loc;void*annot;};
struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct
_tuple3{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
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
struct Cyc_Core_Opt*fields;int is_extensible;int is_flat;};struct Cyc_Absyn_Enumfield{
struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};
struct Cyc_Absyn_Enumdecl{void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};
struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*
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
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);int Cyc_Absyn_is_nontagged_union_type(void*);int Cyc_Absyn_is_aggr_type(void*t);
struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
_dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*,void*t);int Cyc_Tcutil_is_noreturn(void*);struct _tuple4{
unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};
struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_UniquePlace{struct
Cyc_CfFlowInfo_Place place;struct Cyc_List_List*path;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct Cyc_CfFlowInfo_LessNumelts_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqNumelts_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};union Cyc_CfFlowInfo_RelnOp_union{struct Cyc_CfFlowInfo_EqualConst_struct
EqualConst;struct Cyc_CfFlowInfo_LessVar_struct LessVar;struct Cyc_CfFlowInfo_LessNumelts_struct
LessNumelts;struct Cyc_CfFlowInfo_LessConst_struct LessConst;struct Cyc_CfFlowInfo_LessEqNumelts_struct
LessEqNumelts;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp_union
rop;};struct Cyc_CfFlowInfo_TagCmp{void*cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[
15];struct Cyc_CfFlowInfo_HasTagCmps_struct{char*tag;struct Cyc_List_List*f1;};
extern char Cyc_CfFlowInfo_IsZero[11];extern char Cyc_CfFlowInfo_NotZero[12];struct
Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[
13];struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};
struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct
Cyc_CfFlowInfo_UnknownL_struct{int tag;};union Cyc_CfFlowInfo_AbsLVal_union{struct
Cyc_CfFlowInfo_PlaceL_struct PlaceL;struct Cyc_CfFlowInfo_UnknownL_struct UnknownL;
};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;int f1;struct _dyneither_ptr f2;};int Cyc_CfFlowInfo_update_place_set(
struct Cyc_Dict_Dict*set,void*place,struct Cyc_Position_Segment*loc);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int
disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict consumed;struct Cyc_List_List*
may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(struct
_RegionHandle*,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2);int Cyc_CfFlowInfo_consume_approx(struct Cyc_CfFlowInfo_ConsumeInfo c1,struct
Cyc_CfFlowInfo_ConsumeInfo c2);struct Cyc_CfFlowInfo_BottomFL_struct{int tag;};
struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;struct Cyc_List_List*
f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r);int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*
f);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,
void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(union
Cyc_CfFlowInfo_FlowInfo_union f1,union Cyc_CfFlowInfo_FlowInfo_union f2);void Cyc_CfFlowInfo_print_place(
struct Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict
p,void(*pr)(void*));void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(
void*));struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,
struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,void*r);struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,
union Cyc_CfFlowInfo_FlowInfo_union,union Cyc_CfFlowInfo_FlowInfo_union,int);
struct _tuple5{union Cyc_CfFlowInfo_FlowInfo_union f1;void*f2;};struct _tuple5 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple5 pr1,
struct _tuple5 pr2,int);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_after_flow(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo_union,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo_union f,void*rgn);struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_consume_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);void Cyc_CfFlowInfo_check_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo_union f);struct _tuple6{struct Cyc_CfFlowInfo_ConsumeInfo
f1;union Cyc_CfFlowInfo_FlowInfo_union f2;};struct _tuple6 Cyc_CfFlowInfo_save_consume_info(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo_union f,int clear);
union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo_union
f,struct Cyc_CfFlowInfo_ConsumeInfo c,int may_consume_only);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*
enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(struct
Cyc_List_List*ds);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_all_effects: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt*encloser;int visited;};static char Cyc_NewControlFlow_CFAnnot[
12]="\000\000\000\000CFAnnot\000";struct Cyc_NewControlFlow_CFAnnot_struct{char*
tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){struct Cyc_NewControlFlow_CFAnnot_struct
_tmp5C6;struct Cyc_NewControlFlow_CFStmtAnnot _tmp5C5;struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp5C4;(void*)(enclosee->annot=(void*)((void*)((_tmp5C4=_cycalloc(sizeof(*
_tmp5C4)),((_tmp5C4[0]=((_tmp5C6.tag=Cyc_NewControlFlow_CFAnnot,((_tmp5C6.f1=((
_tmp5C5.encloser=encloser,((_tmp5C5.visited=0,_tmp5C5)))),_tmp5C6)))),_tmp5C4))))));}
struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*
fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo_union
tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;struct Cyc_List_List*
param_roots;struct Cyc_Hashtable_Table*flow_table;};static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,
struct Cyc_Absyn_Stmt*);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Decl*);
struct _tuple7{union Cyc_CfFlowInfo_FlowInfo_union f1;union Cyc_CfFlowInfo_AbsLVal_union
f2;};static struct _tuple7 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);
struct _tuple8{union Cyc_CfFlowInfo_FlowInfo_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};static struct _tuple8 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);static struct Cyc_List_List*
Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_Place*p,void*t);static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt*s);static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt*s){void*_tmp3=(void*)s->annot;struct Cyc_NewControlFlow_CFStmtAnnot
_tmp4;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;_LL1: if(*((void**)_tmp3)!= Cyc_NewControlFlow_CFAnnot)
goto _LL3;_tmp4=((struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_tmp5=(
struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_struct*)
_tmp3)->f1;_LL2: return _tmp5;_LL3:;_LL4: {struct Cyc_Core_Impossible_struct _tmp5CC;
const char*_tmp5CB;struct Cyc_Core_Impossible_struct*_tmp5CA;(int)_throw((void*)((
_tmp5CA=_cycalloc(sizeof(*_tmp5CA)),((_tmp5CA[0]=((_tmp5CC.tag=Cyc_Core_Impossible,((
_tmp5CC.f1=((_tmp5CB="ControlFlow -- wrong stmt annotation",_tag_dyneither(
_tmp5CB,sizeof(char),37))),_tmp5CC)))),_tmp5CA)))));}_LL0:;}static union Cyc_CfFlowInfo_FlowInfo_union*
Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*
s);static union Cyc_CfFlowInfo_FlowInfo_union*Cyc_NewControlFlow_get_stmt_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union**
sflow=((union Cyc_CfFlowInfo_FlowInfo_union**(*)(struct Cyc_Hashtable_Table*t,
struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);if(sflow
== 0){union Cyc_CfFlowInfo_FlowInfo_union _tmp5CF;union Cyc_CfFlowInfo_FlowInfo_union*
_tmp5CE;union Cyc_CfFlowInfo_FlowInfo_union*res=(_tmp5CE=_region_malloc(env->r,
sizeof(*_tmp5CE)),((_tmp5CE[0]=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp5CF.BottomFL).tag=
0,_tmp5CF)),_tmp5CE)));((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*
key,union Cyc_CfFlowInfo_FlowInfo_union*val))Cyc_Hashtable_insert)(env->flow_table,
s,res);return res;}return*sflow;}struct _tuple9{struct Cyc_NewControlFlow_CFStmtAnnot*
f1;union Cyc_CfFlowInfo_FlowInfo_union*f2;};static struct _tuple9 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Stmt*s);static struct _tuple9 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo_union*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,
s);*_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC,1);
_tmpB->visited=env->iteration_num;{struct _tuple9 _tmp5D0;return(_tmp5D0.f1=_tmpB,((
_tmp5D0.f2=_tmpC,_tmp5D0)));}}static void Cyc_NewControlFlow_update_tryflow(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union new_flow);
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,
union Cyc_CfFlowInfo_FlowInfo_union new_flow){if(env->in_try){env->tryflow=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,new_flow,env->tryflow,1);{struct Cyc_CfFlowInfo_ConsumeInfo
_tmpF;struct Cyc_Dict_Dict _tmp10;struct Cyc_List_List*_tmp11;struct _tuple6 _tmpE=
Cyc_CfFlowInfo_save_consume_info(env->fenv,env->tryflow,0);_tmpF=_tmpE.f1;_tmp10=
_tmpF.consumed;_tmp11=_tmpF.may_consume;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp5D1;
env->tryflow=Cyc_CfFlowInfo_restore_consume_info(env->tryflow,((_tmp5D1.consumed=
_tmp10,((_tmp5D1.may_consume=0,_tmp5D1)))),0);}}}}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo_union
flow);static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo_union flow){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp13=Cyc_NewControlFlow_get_stmt_annot(s);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp14=Cyc_NewControlFlow_get_stmt_flow(env,s);union Cyc_CfFlowInfo_FlowInfo_union
_tmp15=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,flow,*_tmp14,1);if(!
Cyc_CfFlowInfo_flow_lessthan_approx(_tmp15,*_tmp14)){*_tmp14=_tmp15;if(_tmp13->visited
== env->iteration_num)env->iterate_again=1;}}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*vds,void*leafval,struct Cyc_Position_Segment*loc);
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_List_List*vds,void*
leafval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo_union _tmp16=
inflow;struct Cyc_Dict_Dict _tmp17;struct Cyc_List_List*_tmp18;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp19;_LL6: if((_tmp16.BottomFL).tag != 0)goto _LL8;_LL7: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp5D2;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp5D2.BottomFL).tag=0,
_tmp5D2));}_LL8: if((_tmp16.ReachableFL).tag != 1)goto _LL5;_tmp17=(_tmp16.ReachableFL).f1;
_tmp18=(_tmp16.ReachableFL).f2;_tmp19=(_tmp16.ReachableFL).f3;_LL9: for(0;vds != 0;
vds=vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct _tmp5D5;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp5D4;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp1B=(_tmp5D4=_region_malloc(fenv->r,
sizeof(*_tmp5D4)),((_tmp5D4[0]=((_tmp5D5.tag=0,((_tmp5D5.f1=(struct Cyc_Absyn_Vardecl*)
vds->hd,_tmp5D5)))),_tmp5D4)));_tmp17=Cyc_Dict_insert(_tmp17,(void*)_tmp1B,Cyc_CfFlowInfo_typ_to_absrval(
fenv,(void*)((struct Cyc_Absyn_Vardecl*)vds->hd)->type,leafval));}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5D6;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp5D6.ReachableFL).tag=1,(((
_tmp5D6.ReachableFL).f1=_tmp17,(((_tmp5D6.ReachableFL).f2=_tmp18,(((_tmp5D6.ReachableFL).f3=
_tmp19,_tmp5D6))))))));}_LL5:;}struct _tuple10{struct Cyc_CfFlowInfo_Place*f1;
struct Cyc_Position_Segment*f2;};static void Cyc_NewControlFlow_remove_vars(struct
Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Dict_Dict*
out_unique_places,struct Cyc_Dict_Dict old_unique_places);static void Cyc_NewControlFlow_remove_vars(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union outflow,
struct Cyc_Dict_Dict*out_unique_places,struct Cyc_Dict_Dict old_unique_places){
struct Cyc_CfFlowInfo_ConsumeInfo _tmp20;struct Cyc_Dict_Dict _tmp21;struct _tuple6
_tmp1F=Cyc_CfFlowInfo_save_consume_info(fenv,outflow,0);_tmp20=_tmp1F.f1;_tmp21=
_tmp20.consumed;{struct _RegionHandle*_tmp22=fenv->r;{struct Cyc_Iter_Iter _tmp23=((
struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
_tmp22,*((struct Cyc_Dict_Dict*)_check_null(out_unique_places)));struct _tuple10
_tmp24=*((struct _tuple10*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
_tmp22,*out_unique_places);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple10*))
Cyc_Iter_next)(_tmp23,& _tmp24)){struct Cyc_CfFlowInfo_Place*_tmp25=_tmp24.f1;if(!((
int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(
_tmp21,_tmp25)){const char*_tmp5DA;void*_tmp5D9[1];struct Cyc_String_pa_struct
_tmp5D8;(_tmp5D8.tag=0,((_tmp5D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_CfFlowInfo_place_err_string(_tmp25)),((_tmp5D9[0]=& _tmp5D8,Cyc_Tcutil_terr(
_tmp24.f2,((_tmp5DA="Failed to consume unique variable %s",_tag_dyneither(
_tmp5DA,sizeof(char),37))),_tag_dyneither(_tmp5D9,sizeof(void*),1)))))));}}}*
out_unique_places=old_unique_places;}}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo_union inflow,void*r);static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo_union inflow,void*r){union Cyc_CfFlowInfo_FlowInfo_union
_tmp29=inflow;struct Cyc_Dict_Dict _tmp2A;struct Cyc_List_List*_tmp2B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2C;_LLB: if((_tmp29.BottomFL).tag != 0)goto _LLD;_LLC: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp5DB;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp5DB.BottomFL).tag=0,
_tmp5DB));}_LLD: if((_tmp29.ReachableFL).tag != 1)goto _LLA;_tmp2A=(_tmp29.ReachableFL).f1;
_tmp2B=(_tmp29.ReachableFL).f2;_tmp2C=(_tmp29.ReachableFL).f3;_LLE: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp2A,r)!= (void*)2){const char*_tmp5DE;void*_tmp5DD;(_tmp5DD=0,Cyc_Tcutil_terr(
loc,((_tmp5DE="expression may not be fully initialized",_tag_dyneither(_tmp5DE,
sizeof(char),40))),_tag_dyneither(_tmp5DD,sizeof(void*),0)));}{struct Cyc_Dict_Dict
_tmp30=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp2A,env->all_changed,r);if(
_tmp2A.t == _tmp30.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union _tmp5DF;
union Cyc_CfFlowInfo_FlowInfo_union _tmp31=((_tmp5DF.ReachableFL).tag=1,(((_tmp5DF.ReachableFL).f1=
_tmp30,(((_tmp5DF.ReachableFL).f2=_tmp2B,(((_tmp5DF.ReachableFL).f3=_tmp2C,
_tmp5DF)))))));Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp31);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp31;}}_LLA:;}struct _tuple11{
union Cyc_CfFlowInfo_FlowInfo_union f1;struct Cyc_List_List*f2;};static struct
_tuple11 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct
Cyc_List_List*es,int arg1_unconsumed);static struct _tuple11 Cyc_NewControlFlow_anal_unordered_Rexps(
struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*es,int arg1_unconsumed){if(es == 0){struct _tuple11
_tmp5E0;return(_tmp5E0.f1=inflow,((_tmp5E0.f2=0,_tmp5E0)));}if(es->tl == 0){union
Cyc_CfFlowInfo_FlowInfo_union _tmp35;void*_tmp36;struct _tuple5 _tmp34=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp35=_tmp34.f1;_tmp36=_tmp34.f2;{
struct Cyc_List_List*_tmp5E3;struct _tuple11 _tmp5E2;return(_tmp5E2.f1=_tmp35,((
_tmp5E2.f2=((_tmp5E3=_region_malloc(rgn,sizeof(*_tmp5E3)),((_tmp5E3->hd=(void*)
_tmp36,((_tmp5E3->tl=0,_tmp5E3)))))),_tmp5E2)));}}{struct Cyc_Dict_Dict*
outer_all_changed=env->all_changed;struct Cyc_Dict_Dict this_all_changed;union Cyc_CfFlowInfo_FlowInfo_union
old_inflow;union Cyc_CfFlowInfo_FlowInfo_union outflow;struct Cyc_List_List*rvals;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp3A;union Cyc_CfFlowInfo_FlowInfo_union _tmp3B;
struct _tuple6 _tmp39=Cyc_CfFlowInfo_save_consume_info(env->fenv,inflow,1);_tmp3A=
_tmp39.f1;_tmp3B=_tmp39.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp5E4;struct Cyc_CfFlowInfo_ConsumeInfo
outflow_consume=(_tmp5E4.consumed=(env->fenv)->mt_place_set,((_tmp5E4.may_consume=
0,_tmp5E4)));int init_consume=0;do{this_all_changed=(env->fenv)->mt_place_set;
_tmp3B=Cyc_CfFlowInfo_restore_consume_info(_tmp3B,_tmp3A,0);{struct Cyc_Dict_Dict*
_tmp5E5;env->all_changed=((_tmp5E5=_region_malloc(env->r,sizeof(*_tmp5E5)),((
_tmp5E5[0]=(env->fenv)->mt_place_set,_tmp5E5))));}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp3E;void*_tmp3F;struct _tuple5 _tmp3D=Cyc_NewControlFlow_anal_Rexp(env,_tmp3B,(
struct Cyc_Absyn_Exp*)es->hd);_tmp3E=_tmp3D.f1;_tmp3F=_tmp3D.f2;outflow=_tmp3E;{
struct Cyc_List_List*_tmp5E6;rvals=((_tmp5E6=_region_malloc(rgn,sizeof(*_tmp5E6)),((
_tmp5E6->hd=(void*)_tmp3F,((_tmp5E6->tl=0,_tmp5E6))))));}this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);if(
arg1_unconsumed){union Cyc_CfFlowInfo_FlowInfo_union _tmp41=outflow;struct Cyc_Dict_Dict
_tmp42;struct Cyc_List_List*_tmp43;struct Cyc_CfFlowInfo_ConsumeInfo _tmp44;_LL10:
if((_tmp41.BottomFL).tag != 0)goto _LL12;_LL11: goto _LLF;_LL12: if((_tmp41.ReachableFL).tag
!= 1)goto _LLF;_tmp42=(_tmp41.ReachableFL).f1;_tmp43=(_tmp41.ReachableFL).f2;
_tmp44=(_tmp41.ReachableFL).f3;_LL13: {struct Cyc_CfFlowInfo_ConsumeInfo _tmp5E9;
union Cyc_CfFlowInfo_FlowInfo_union _tmp5E8;outflow=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp5E8.ReachableFL).tag=1,(((_tmp5E8.ReachableFL).f1=_tmp42,(((_tmp5E8.ReachableFL).f2=
_tmp43,(((_tmp5E8.ReachableFL).f3=((_tmp5E9.consumed=_tmp44.consumed,((_tmp5E9.may_consume=
_tmp3A.may_consume,_tmp5E9)))),_tmp5E8))))))));}_LLF:;}{struct Cyc_List_List*es2=
es->tl;for(0;es2 != 0;es2=es2->tl){{struct Cyc_Dict_Dict*_tmp5EA;env->all_changed=((
_tmp5EA=_region_malloc(env->r,sizeof(*_tmp5EA)),((_tmp5EA[0]=(env->fenv)->mt_place_set,
_tmp5EA))));}{union Cyc_CfFlowInfo_FlowInfo_union _tmp49;void*_tmp4A;struct _tuple5
_tmp48=Cyc_NewControlFlow_anal_Rexp(env,_tmp3B,(struct Cyc_Absyn_Exp*)es2->hd);
_tmp49=_tmp48.f1;_tmp4A=_tmp48.f2;{struct Cyc_List_List*_tmp5EB;rvals=((_tmp5EB=
_region_malloc(rgn,sizeof(*_tmp5EB)),((_tmp5EB->hd=(void*)_tmp4A,((_tmp5EB->tl=
rvals,_tmp5EB))))));}outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp49,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);}}}{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp4D;struct _tuple6 _tmp4C=Cyc_CfFlowInfo_save_consume_info(
env->fenv,outflow,0);_tmp4D=_tmp4C.f1;if(init_consume){if(!Cyc_CfFlowInfo_consume_approx(
outflow_consume,_tmp4D)){{const char*_tmp5EE;void*_tmp5ED;(_tmp5ED=0,Cyc_fprintf(
Cyc_stderr,((_tmp5EE="sanity consumed: ",_tag_dyneither(_tmp5EE,sizeof(char),18))),
_tag_dyneither(_tmp5ED,sizeof(void*),0)));}((void(*)(struct Cyc_Dict_Dict p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(outflow_consume.consumed,
Cyc_CfFlowInfo_print_place);{const char*_tmp5F1;void*_tmp5F0;(_tmp5F0=0,Cyc_fprintf(
Cyc_stderr,((_tmp5F1="\ncurrent consumed: ",_tag_dyneither(_tmp5F1,sizeof(char),
20))),_tag_dyneither(_tmp5F0,sizeof(void*),0)));}((void(*)(struct Cyc_Dict_Dict p,
void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(_tmp4D.consumed,
Cyc_CfFlowInfo_print_place);{const char*_tmp5F4;void*_tmp5F3;(_tmp5F3=0,Cyc_fprintf(
Cyc_stderr,((_tmp5F4="\nsanity may_consume: ",_tag_dyneither(_tmp5F4,sizeof(char),
22))),_tag_dyneither(_tmp5F3,sizeof(void*),0)));}((void(*)(struct Cyc_List_List*p,
void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(
outflow_consume.may_consume,Cyc_CfFlowInfo_print_place);{const char*_tmp5F7;void*
_tmp5F6;(_tmp5F6=0,Cyc_fprintf(Cyc_stderr,((_tmp5F7="\ncurrent may_consume: ",
_tag_dyneither(_tmp5F7,sizeof(char),23))),_tag_dyneither(_tmp5F6,sizeof(void*),0)));}((
void(*)(struct Cyc_List_List*p,void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(
_tmp4D.may_consume,Cyc_CfFlowInfo_print_place);{const char*_tmp5FA;void*_tmp5F9;(
_tmp5F9=0,Cyc_fprintf(Cyc_stderr,((_tmp5FA="\n",_tag_dyneither(_tmp5FA,sizeof(
char),2))),_tag_dyneither(_tmp5F9,sizeof(void*),0)));}{const char*_tmp5FD;void*
_tmp5FC;(_tmp5FC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp5FD="anal_unordered_exps failed to preserve consumed info",_tag_dyneither(
_tmp5FD,sizeof(char),53))),_tag_dyneither(_tmp5FC,sizeof(void*),0)));}}
old_inflow=Cyc_CfFlowInfo_restore_consume_info(_tmp3B,outflow_consume,0);}else{
old_inflow=_tmp3B;}init_consume=1;outflow_consume=_tmp4D;_tmp3B=Cyc_CfFlowInfo_join_flow(
env->fenv,outer_all_changed,_tmp3B,outflow,1);}}}while(!Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp3B,old_inflow));if(outer_all_changed == 0)env->all_changed=0;else{struct Cyc_Dict_Dict*
_tmp5FE;env->all_changed=((_tmp5FE=_region_malloc(env->r,sizeof(*_tmp5FE)),((
_tmp5FE[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),
_tmp5FE))));}Cyc_NewControlFlow_update_tryflow(env,outflow);{struct _tuple11
_tmp5FF;return(_tmp5FF.f1=outflow,((_tmp5FF.f2=Cyc_List_imp_rev(rvals),_tmp5FF)));}}}}
static struct _tuple5 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_List_List*es,int
arg1_unconsumed);static struct _tuple5 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_List_List*es,int
arg1_unconsumed){struct _RegionHandle*_tmp5D=env->r;union Cyc_CfFlowInfo_FlowInfo_union
_tmp5F;struct Cyc_List_List*_tmp60;struct _tuple11 _tmp5E=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp5D,env,inflow,es,arg1_unconsumed);_tmp5F=_tmp5E.f1;_tmp60=_tmp5E.f2;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp61=_tmp5F;struct Cyc_Dict_Dict _tmp62;_LL15: if((
_tmp61.ReachableFL).tag != 1)goto _LL17;_tmp62=(_tmp61.ReachableFL).f1;_LL16: for(0;
_tmp60 != 0;(_tmp60=_tmp60->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp62,(void*)_tmp60->hd)== (void*)0){
const char*_tmp602;void*_tmp601;(_tmp601=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp602="expression may not be initialized",
_tag_dyneither(_tmp602,sizeof(char),34))),_tag_dyneither(_tmp601,sizeof(void*),0)));}}
goto _LL14;_LL17: if((_tmp61.BottomFL).tag != 0)goto _LL14;_LL18: goto _LL14;_LL14:;}{
struct _tuple5 _tmp603;return(_tmp603.f1=_tmp5F,((_tmp603.f2=(void*)(env->fenv)->unknown_all,
_tmp603)));}}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_notzero(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Absyn_Exp*e,void*il);static
union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Absyn_Exp*e,void*il){union Cyc_CfFlowInfo_FlowInfo_union _tmp66=
outflow;struct Cyc_Dict_Dict _tmp67;struct Cyc_List_List*_tmp68;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp69;_LL1A: if((_tmp66.BottomFL).tag != 0)goto _LL1C;_LL1B: return outflow;_LL1C:
if((_tmp66.ReachableFL).tag != 1)goto _LL19;_tmp67=(_tmp66.ReachableFL).f1;_tmp68=(
_tmp66.ReachableFL).f2;_tmp69=(_tmp66.ReachableFL).f3;_LL1D: {union Cyc_CfFlowInfo_AbsLVal_union
_tmp6A=(Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*
_tmp6B;_LL1F: if((_tmp6A.UnknownL).tag != 1)goto _LL21;_LL20: return outflow;_LL21:
if((_tmp6A.PlaceL).tag != 0)goto _LL1E;_tmp6B=(_tmp6A.PlaceL).f1;_LL22: {void*
nzval=il == (void*)2?(void*)1:(void*)2;union Cyc_CfFlowInfo_FlowInfo_union _tmp604;
union Cyc_CfFlowInfo_FlowInfo_union _tmp6C=((_tmp604.ReachableFL).tag=1,(((_tmp604.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp67,env->all_changed,_tmp6B,
nzval),(((_tmp604.ReachableFL).f2=_tmp68,(((_tmp604.ReachableFL).f3=_tmp69,
_tmp604)))))));return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp6C;}_LL1E:;}_LL19:;}
static struct _tuple8 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Absyn_Exp*e,void*il);static struct _tuple8 Cyc_NewControlFlow_splitzero(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Absyn_Exp*e,void*il){union
Cyc_CfFlowInfo_FlowInfo_union _tmp6E=outflow;struct Cyc_Dict_Dict _tmp6F;struct Cyc_List_List*
_tmp70;struct Cyc_CfFlowInfo_ConsumeInfo _tmp71;_LL24: if((_tmp6E.BottomFL).tag != 0)
goto _LL26;_LL25: {struct _tuple8 _tmp605;return(_tmp605.f1=outflow,((_tmp605.f2=
outflow,_tmp605)));}_LL26: if((_tmp6E.ReachableFL).tag != 1)goto _LL23;_tmp6F=(
_tmp6E.ReachableFL).f1;_tmp70=(_tmp6E.ReachableFL).f2;_tmp71=(_tmp6E.ReachableFL).f3;
_LL27: {union Cyc_CfFlowInfo_AbsLVal_union _tmp73=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp74;_LL29: if((_tmp73.UnknownL).tag
!= 1)goto _LL2B;_LL2A: {struct _tuple8 _tmp606;return(_tmp606.f1=outflow,((_tmp606.f2=
outflow,_tmp606)));}_LL2B: if((_tmp73.PlaceL).tag != 0)goto _LL28;_tmp74=(_tmp73.PlaceL).f1;
_LL2C: {void*nzval=il == (void*)2?(void*)1:(void*)2;union Cyc_CfFlowInfo_FlowInfo_union
_tmp60B;union Cyc_CfFlowInfo_FlowInfo_union _tmp60A;struct _tuple8 _tmp609;return(
_tmp609.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp60B.ReachableFL).tag=1,(((
_tmp60B.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp6F,env->all_changed,
_tmp74,nzval),(((_tmp60B.ReachableFL).f2=_tmp70,(((_tmp60B.ReachableFL).f3=
_tmp71,_tmp60B)))))))),((_tmp609.f2=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp60A.ReachableFL).tag=1,(((_tmp60A.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp6F,env->all_changed,_tmp74,(void*)0),(((_tmp60A.ReachableFL).f2=
_tmp70,(((_tmp60A.ReachableFL).f3=_tmp71,_tmp60A)))))))),_tmp609)));}_LL28:;}
_LL23:;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_if_tagcmp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union flow,
struct Cyc_Absyn_Exp*e1,void*r1,void*p,void*r2);static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_if_tagcmp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
flow,struct Cyc_Absyn_Exp*e1,void*r1,void*p,void*r2){union Cyc_CfFlowInfo_FlowInfo_union
_tmp79=flow;struct Cyc_Dict_Dict _tmp7A;struct Cyc_List_List*_tmp7B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp7C;_LL2E: if((_tmp79.BottomFL).tag != 0)goto _LL30;_LL2F: return flow;_LL30: if((
_tmp79.ReachableFL).tag != 1)goto _LL2D;_tmp7A=(_tmp79.ReachableFL).f1;_tmp7B=(
_tmp79.ReachableFL).f2;_tmp7C=(_tmp79.ReachableFL).f3;_LL31: {void*_tmp7D=r2;
struct Cyc_List_List*_tmp7E;_LL33: if(_tmp7D <= (void*)3)goto _LL35;if(*((int*)
_tmp7D)!= 3)goto _LL35;_tmp7E=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp7D)->f1;
_LL34: {union Cyc_CfFlowInfo_AbsLVal_union _tmp7F=(Cyc_NewControlFlow_anal_Lexp(
env,flow,e1)).f2;struct Cyc_CfFlowInfo_Place*_tmp80;_LL38: if((_tmp7F.UnknownL).tag
!= 1)goto _LL3A;_LL39: return flow;_LL3A: if((_tmp7F.PlaceL).tag != 0)goto _LL37;
_tmp80=(_tmp7F.PlaceL).f1;_LL3B: {struct Cyc_List_List*new_cl;{void*_tmp81=r1;
struct Cyc_List_List*_tmp82;void*_tmp83;_LL3D: if(_tmp81 <= (void*)3)goto _LL41;if(*((
int*)_tmp81)!= 3)goto _LL3F;_tmp82=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp81)->f1;
_LL3E: new_cl=_tmp82;goto _LL3C;_LL3F: if(*((int*)_tmp81)!= 0)goto _LL41;_tmp83=(
void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp81)->f1;if((int)_tmp83 != 2)
goto _LL41;_LL40: goto _LL42;_LL41: if((int)_tmp81 != 0)goto _LL43;_LL42: goto _LL44;
_LL43: if((int)_tmp81 != 1)goto _LL45;_LL44: new_cl=0;goto _LL3C;_LL45:;_LL46: return
flow;_LL3C:;}for(0;_tmp7E != 0;_tmp7E=_tmp7E->tl){void*new_cmp;{struct _tuple0
_tmp60C;struct _tuple0 _tmp85=(_tmp60C.f1=p,((_tmp60C.f2=(void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp7E->hd)->cmp,_tmp60C)));void*_tmp86;void*_tmp87;void*_tmp88;void*_tmp89;
_LL48: _tmp86=_tmp85.f1;if((int)_tmp86 != 8)goto _LL4A;_LL49: goto _LL4B;_LL4A: _tmp87=
_tmp85.f2;if((int)_tmp87 != 8)goto _LL4C;_LL4B: new_cmp=(void*)8;goto _LL47;_LL4C:
_tmp88=_tmp85.f1;if((int)_tmp88 != 5)goto _LL4E;_tmp89=_tmp85.f2;if((int)_tmp89 != 
5)goto _LL4E;_LL4D: new_cmp=(void*)5;goto _LL47;_LL4E:;_LL4F: new_cmp=(void*)10;goto
_LL47;_LL47:;}{struct Cyc_CfFlowInfo_TagCmp*_tmp60F;struct Cyc_List_List*_tmp60E;
new_cl=((_tmp60E=_region_malloc(env->r,sizeof(*_tmp60E)),((_tmp60E->hd=((_tmp60F=
_region_malloc(env->r,sizeof(*_tmp60F)),((_tmp60F->cmp=(void*)new_cmp,((_tmp60F->bd=(
void*)((void*)((struct Cyc_CfFlowInfo_TagCmp*)_tmp7E->hd)->bd),_tmp60F)))))),((
_tmp60E->tl=new_cl,_tmp60E))))));}}{struct Cyc_CfFlowInfo_TagCmps_struct*_tmp615;
struct Cyc_CfFlowInfo_TagCmps_struct _tmp614;union Cyc_CfFlowInfo_FlowInfo_union
_tmp613;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp613.ReachableFL).tag=1,(((
_tmp613.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(env->fenv,e1->loc,_tmp7A,env->all_changed,
_tmp80,(void*)((_tmp615=_region_malloc(env->r,sizeof(*_tmp615)),((_tmp615[0]=((
_tmp614.tag=3,((_tmp614.f1=new_cl,_tmp614)))),_tmp615))))),(((_tmp613.ReachableFL).f2=
_tmp7B,(((_tmp613.ReachableFL).f3=_tmp7C,_tmp613))))))));}}_LL37:;}_LL35:;_LL36:
return flow;_LL32:;}_LL2D:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct Cyc_NewControlFlow_mt_unknownz_v={
Cyc_CfFlowInfo_UnknownZ,0};static struct _tuple5 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_Absyn_Exp*e,void*r);static struct
_tuple5 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union f,struct
Cyc_Absyn_Exp*e,void*r){void*_tmp91=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp92;void*_tmp93;struct Cyc_Absyn_PtrAtts
_tmp94;struct Cyc_Absyn_Conref*_tmp95;struct Cyc_Absyn_Conref*_tmp96;_LL51: if(
_tmp91 <= (void*)4)goto _LL53;if(*((int*)_tmp91)!= 4)goto _LL53;_tmp92=((struct Cyc_Absyn_PointerType_struct*)
_tmp91)->f1;_tmp93=(void*)_tmp92.elt_typ;_tmp94=_tmp92.ptr_atts;_tmp95=_tmp94.bounds;
_tmp96=_tmp94.zero_term;_LL52: {union Cyc_CfFlowInfo_FlowInfo_union _tmp97=f;
struct Cyc_Dict_Dict _tmp98;struct Cyc_List_List*_tmp99;_LL56: if((_tmp97.BottomFL).tag
!= 0)goto _LL58;_LL57: {struct _tuple5 _tmp616;return(_tmp616.f1=f,((_tmp616.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp93,(void*)(env->fenv)->unknown_all),_tmp616)));}_LL58: if((_tmp97.ReachableFL).tag
!= 1)goto _LL55;_tmp98=(_tmp97.ReachableFL).f1;_tmp99=(_tmp97.ReachableFL).f2;
_LL59: if(Cyc_Tcutil_is_bound_one(_tmp95)){void*_tmp9B=r;struct Cyc_CfFlowInfo_Place*
_tmp9C;void*_tmp9D;_LL5B: if((int)_tmp9B != 1)goto _LL5D;_LL5C: goto _LL5E;_LL5D: if((
int)_tmp9B != 2)goto _LL5F;_LL5E:{void*_tmp9E=(void*)e->annot;struct Cyc_List_List*
_tmp9F;_LL68: if(*((void**)_tmp9E)!= Cyc_CfFlowInfo_NotZero)goto _LL6A;_tmp9F=((
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp9E)->f1;_LL69: if(!Cyc_CfFlowInfo_same_relns(
_tmp99,_tmp9F))goto _LL6B;goto _LL67;_LL6A:;_LL6B:{void*_tmpA0=(void*)e->r;_LL6D:
if(*((int*)_tmpA0)!= 25)goto _LL6F;_LL6E:{struct Cyc_CfFlowInfo_NotZero_struct
_tmp619;struct Cyc_CfFlowInfo_NotZero_struct*_tmp618;(void*)(e->annot=(void*)((
void*)((_tmp618=_cycalloc(sizeof(*_tmp618)),((_tmp618[0]=((_tmp619.tag=Cyc_CfFlowInfo_NotZero,((
_tmp619.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp99),_tmp619)))),
_tmp618))))));}goto _LL6C;_LL6F:;_LL70:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_notzero_v));
goto _LL6C;_LL6C:;}goto _LL67;_LL67:;}goto _LL5A;_LL5F: if(_tmp9B <= (void*)3)goto
_LL61;if(*((int*)_tmp9B)!= 2)goto _LL61;_tmp9C=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp9B)->f1;_LL60:{void*_tmpA3=(void*)e->annot;struct Cyc_List_List*_tmpA4;_LL72:
if(*((void**)_tmpA3)!= Cyc_CfFlowInfo_NotZero)goto _LL74;_tmpA4=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpA3)->f1;_LL73: if(!Cyc_CfFlowInfo_same_relns(_tmp99,_tmpA4))goto _LL75;goto
_LL71;_LL74:;_LL75:{void*_tmpA5=(void*)e->r;_LL77: if(*((int*)_tmpA5)!= 25)goto
_LL79;_LL78:{struct Cyc_CfFlowInfo_NotZero_struct _tmp61C;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp61B;(void*)(e->annot=(void*)((void*)((_tmp61B=_cycalloc(sizeof(*_tmp61B)),((
_tmp61B[0]=((_tmp61C.tag=Cyc_CfFlowInfo_NotZero,((_tmp61C.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp99),_tmp61C)))),_tmp61B))))));}goto _LL76;_LL79:;_LL7A:(
void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_notzero_v));goto _LL76;
_LL76:;}goto _LL71;_LL71:;}{struct _tuple5 _tmp61D;return(_tmp61D.f1=f,((_tmp61D.f2=
Cyc_CfFlowInfo_lookup_place(_tmp98,_tmp9C),_tmp61D)));}_LL61: if((int)_tmp9B != 0)
goto _LL63;_LL62:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));{union
Cyc_CfFlowInfo_FlowInfo_union _tmp620;struct _tuple5 _tmp61F;return(_tmp61F.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp620.BottomFL).tag=0,_tmp620)),((
_tmp61F.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp93,(void*)(env->fenv)->unknown_all),
_tmp61F)));}_LL63: if(_tmp9B <= (void*)3)goto _LL65;if(*((int*)_tmp9B)!= 0)goto
_LL65;_tmp9D=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp9B)->f1;_LL64: f=
Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp9D);goto _LL66;_LL65:;_LL66:{void*
_tmpAB=(void*)e->r;_LL7C: if(*((int*)_tmpAB)!= 25)goto _LL7E;_LL7D:{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp623;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp622;(void*)(e->annot=(void*)((
void*)((_tmp622=_cycalloc(sizeof(*_tmp622)),((_tmp622[0]=((_tmp623.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp623.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp99),_tmp623)))),
_tmp622))))));}goto _LL7B;_LL7E:;_LL7F:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_unknownz_v));
goto _LL7B;_LL7B:;}goto _LL5A;_LL5A:;}else{void*_tmpAE=(void*)e->annot;struct Cyc_List_List*
_tmpAF;_LL81: if(*((void**)_tmpAE)!= Cyc_CfFlowInfo_UnknownZ)goto _LL83;_tmpAF=((
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmpAE)->f1;_LL82: if(!Cyc_CfFlowInfo_same_relns(
_tmp99,_tmpAF))goto _LL84;goto _LL80;_LL83:;_LL84:{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp626;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp625;(void*)(e->annot=(void*)((
void*)((_tmp625=_cycalloc(sizeof(*_tmp625)),((_tmp625[0]=((_tmp626.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp626.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp99),_tmp626)))),
_tmp625))))));}goto _LL80;_LL80:;}{void*_tmpB2=Cyc_CfFlowInfo_initlevel(env->fenv,
_tmp98,r);_LL86: if((int)_tmpB2 != 0)goto _LL88;_LL87:{const char*_tmp629;void*
_tmp628;(_tmp628=0,Cyc_Tcutil_terr(e->loc,((_tmp629="dereference of possibly uninitialized pointer",
_tag_dyneither(_tmp629,sizeof(char),46))),_tag_dyneither(_tmp628,sizeof(void*),0)));}
goto _LL89;_LL88: if((int)_tmpB2 != 2)goto _LL8A;_LL89: {struct _tuple5 _tmp62A;return(
_tmp62A.f1=f,((_tmp62A.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp93,(void*)(
env->fenv)->unknown_all),_tmp62A)));}_LL8A: if((int)_tmpB2 != 1)goto _LL85;_LL8B: {
struct _tuple5 _tmp62B;return(_tmp62B.f1=f,((_tmp62B.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp93,(void*)(env->fenv)->unknown_none),_tmp62B)));}_LL85:;}_LL55:;}
_LL53:;_LL54: {struct Cyc_Core_Impossible_struct _tmp631;const char*_tmp630;struct
Cyc_Core_Impossible_struct*_tmp62F;(int)_throw((void*)((_tmp62F=_cycalloc(
sizeof(*_tmp62F)),((_tmp62F[0]=((_tmp631.tag=Cyc_Core_Impossible,((_tmp631.f1=((
_tmp630="right deref of non-pointer-type",_tag_dyneither(_tmp630,sizeof(char),32))),
_tmp631)))),_tmp62F)))));}_LL50:;}static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(
struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2);static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(
struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){void*_tmpBA=(void*)e1->r;void*_tmpBB;struct Cyc_Absyn_Vardecl*
_tmpBC;void*_tmpBD;struct Cyc_Absyn_Vardecl*_tmpBE;void*_tmpBF;struct Cyc_Absyn_Vardecl*
_tmpC0;void*_tmpC1;struct Cyc_Absyn_Vardecl*_tmpC2;_LL8D: if(*((int*)_tmpBA)!= 1)
goto _LL8F;_tmpBB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpBB <= (
void*)1)goto _LL8F;if(*((int*)_tmpBB)!= 4)goto _LL8F;_tmpBC=((struct Cyc_Absyn_Pat_b_struct*)
_tmpBB)->f1;_LL8E: _tmpBE=_tmpBC;goto _LL90;_LL8F: if(*((int*)_tmpBA)!= 1)goto _LL91;
_tmpBD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpBD <= (void*)1)
goto _LL91;if(*((int*)_tmpBD)!= 3)goto _LL91;_tmpBE=((struct Cyc_Absyn_Local_b_struct*)
_tmpBD)->f1;_LL90: _tmpC0=_tmpBE;goto _LL92;_LL91: if(*((int*)_tmpBA)!= 1)goto _LL93;
_tmpBF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpBF <= (void*)1)
goto _LL93;if(*((int*)_tmpBF)!= 2)goto _LL93;_tmpC0=((struct Cyc_Absyn_Param_b_struct*)
_tmpBF)->f1;_LL92: _tmpC2=_tmpC0;goto _LL94;_LL93: if(*((int*)_tmpBA)!= 1)goto _LL95;
_tmpC1=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f2;if(_tmpC1 <= (void*)1)
goto _LL95;if(*((int*)_tmpC1)!= 0)goto _LL95;_tmpC2=((struct Cyc_Absyn_Global_b_struct*)
_tmpC1)->f1;_LL94: if(!_tmpC2->escapes){void*_tmpC3=(void*)e2->r;void*_tmpC4;
struct Cyc_Absyn_Vardecl*_tmpC5;void*_tmpC6;struct Cyc_Absyn_Vardecl*_tmpC7;void*
_tmpC8;struct Cyc_Absyn_Vardecl*_tmpC9;void*_tmpCA;struct Cyc_Absyn_Vardecl*_tmpCB;
_LL98: if(*((int*)_tmpC3)!= 1)goto _LL9A;_tmpC4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpC4 <= (void*)1)goto _LL9A;if(*((int*)_tmpC4)!= 4)goto _LL9A;
_tmpC5=((struct Cyc_Absyn_Pat_b_struct*)_tmpC4)->f1;_LL99: _tmpC7=_tmpC5;goto _LL9B;
_LL9A: if(*((int*)_tmpC3)!= 1)goto _LL9C;_tmpC6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpC6 <= (void*)1)goto _LL9C;if(*((int*)_tmpC6)!= 3)goto _LL9C;
_tmpC7=((struct Cyc_Absyn_Local_b_struct*)_tmpC6)->f1;_LL9B: _tmpC9=_tmpC7;goto
_LL9D;_LL9C: if(*((int*)_tmpC3)!= 1)goto _LL9E;_tmpC8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpC8 <= (void*)1)goto _LL9E;if(*((int*)_tmpC8)!= 2)goto _LL9E;
_tmpC9=((struct Cyc_Absyn_Param_b_struct*)_tmpC8)->f1;_LL9D: _tmpCB=_tmpC9;goto
_LL9F;_LL9E: if(*((int*)_tmpC3)!= 1)goto _LLA0;_tmpCA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f2;if(_tmpCA <= (void*)1)goto _LLA0;if(*((int*)_tmpCA)!= 0)goto _LLA0;
_tmpCB=((struct Cyc_Absyn_Global_b_struct*)_tmpCA)->f1;_LL9F: if(!_tmpCB->escapes){
int found=0;{struct Cyc_List_List*_tmpCC=relns;for(0;_tmpCC != 0;_tmpCC=_tmpCC->tl){
struct Cyc_CfFlowInfo_Reln*_tmpCD=(struct Cyc_CfFlowInfo_Reln*)_tmpCC->hd;if(
_tmpCD->vd == _tmpCB){union Cyc_CfFlowInfo_RelnOp_union _tmpCE=_tmpCD->rop;struct
Cyc_Absyn_Vardecl*_tmpCF;_LLA3: if((_tmpCE.LessNumelts).tag != 2)goto _LLA5;_tmpCF=(
_tmpCE.LessNumelts).f1;if(!(_tmpCF == _tmpC2))goto _LLA5;_LLA4: return relns;_LLA5:;
_LLA6: continue;_LLA2:;}}}if(!found){struct Cyc_CfFlowInfo_Reln*_tmp637;union Cyc_CfFlowInfo_RelnOp_union
_tmp636;struct Cyc_List_List*_tmp635;return(_tmp635=_region_malloc(rgn,sizeof(*
_tmp635)),((_tmp635->hd=((_tmp637=_region_malloc(rgn,sizeof(*_tmp637)),((_tmp637->vd=
_tmpCB,((_tmp637->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((_tmp636.LessNumelts).tag=
2,(((_tmp636.LessNumelts).f1=_tmpC2,_tmp636)))),_tmp637)))))),((_tmp635->tl=
relns,_tmp635)))));}}return relns;_LLA0:;_LLA1: return relns;_LL97:;}else{return
relns;}_LL95:;_LL96: return relns;_LL8C:;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_may_consume_place(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_CfFlowInfo_Place*place,void*t,struct
Cyc_List_List**ps);static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_may_consume_place(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,
struct Cyc_CfFlowInfo_Place*place,void*t,struct Cyc_List_List**ps){struct Cyc_CfFlowInfo_FlowEnv*
_tmpD3=env->fenv;struct Cyc_List_List*_tmpD4=Cyc_NewControlFlow_noalias_ptrs_rec(
env,place,t);if(ps != 0)*ps=_tmpD4;if(_tmpD4 == 0)return cinfo;cinfo.may_consume=((
struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(
_tmpD3->r,Cyc_CfFlowInfo_place_cmp,_tmpD4,cinfo.may_consume);return cinfo;}struct
_tuple12{union Cyc_CfFlowInfo_AbsLVal_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_may_consume_exp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_may_consume_exp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct _RegionHandle*_tmpD5=env->r;if(Cyc_Tcutil_is_noalias_path(
_tmpD5,e)){struct Cyc_CfFlowInfo_FlowEnv*_tmpD6=env->fenv;union Cyc_CfFlowInfo_AbsLVal_union
_tmpD8;struct _tuple7 _tmpD7=Cyc_NewControlFlow_anal_Lexp(env,inflow,e);_tmpD8=
_tmpD7.f2;{struct _tuple12 _tmp638;struct _tuple12 _tmpDA=(_tmp638.f1=_tmpD8,((
_tmp638.f2=inflow,_tmp638)));union Cyc_CfFlowInfo_AbsLVal_union _tmpDB;struct Cyc_CfFlowInfo_Place*
_tmpDC;union Cyc_CfFlowInfo_FlowInfo_union _tmpDD;struct Cyc_Dict_Dict _tmpDE;struct
Cyc_List_List*_tmpDF;struct Cyc_CfFlowInfo_ConsumeInfo _tmpE0;_LLA8: _tmpDB=_tmpDA.f1;
if(((_tmpDA.f1).PlaceL).tag != 0)goto _LLAA;_tmpDC=(_tmpDB.PlaceL).f1;_tmpDD=
_tmpDA.f2;if(((_tmpDA.f2).ReachableFL).tag != 1)goto _LLAA;_tmpDE=(_tmpDD.ReachableFL).f1;
_tmpDF=(_tmpDD.ReachableFL).f2;_tmpE0=(_tmpDD.ReachableFL).f3;_LLA9: {struct Cyc_CfFlowInfo_ConsumeInfo
_tmpE1=Cyc_NewControlFlow_may_consume_place(env,_tmpE0,_tmpDC,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);union Cyc_CfFlowInfo_FlowInfo_union _tmp639;return(
union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp639.ReachableFL).tag=1,(((_tmp639.ReachableFL).f1=
_tmpDE,(((_tmp639.ReachableFL).f2=_tmpDF,(((_tmp639.ReachableFL).f3=_tmpE1,
_tmp639))))))));}_LLAA:;_LLAB:{const char*_tmp63D;void*_tmp63C[1];struct Cyc_String_pa_struct
_tmp63B;(_tmp63B.tag=0,((_tmp63B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp63C[0]=& _tmp63B,Cyc_fprintf(Cyc_stderr,((
_tmp63D="Oops---no location for noalias_path |%s|\n",_tag_dyneither(_tmp63D,
sizeof(char),42))),_tag_dyneither(_tmp63C,sizeof(void*),1)))))));}return inflow;
_LLA7:;}}return inflow;}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_consume_place(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo
cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Position_Segment*loc);static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_consume_place(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*
p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Position_Segment*
loc){if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(
cinfo.consumed,p)){struct Cyc_CfFlowInfo_Place _tmpE7;void*_tmpE8;struct Cyc_List_List*
_tmpE9;struct Cyc_CfFlowInfo_Place*_tmpE6=p;_tmpE7=*_tmpE6;_tmpE8=(void*)_tmpE7.root;
_tmpE9=_tmpE7.fields;{void*rval=(void*)0;if((Cyc_Dict_lookup_bool(outdict,_tmpE8,&
rval) && Cyc_CfFlowInfo_initlevel(env->fenv,outdict,rval)!= (void*)0) && rval != (
void*)0){void*_tmpEA=_tmpE8;struct Cyc_Absyn_Vardecl*_tmpEB;_LLAD: if(*((int*)
_tmpEA)!= 0)goto _LLAF;_tmpEB=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmpEA)->f1;
_LLAE: {struct _dyneither_ptr _tmpEC=Cyc_Absynpp_qvar2string(_tmpEB->name);if(
_tmpE9 == 0){const char*_tmp641;void*_tmp640[1];struct Cyc_String_pa_struct _tmp63F;(
_tmp63F.tag=0,((_tmp63F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpEC),((
_tmp640[0]=& _tmp63F,Cyc_Tcutil_warn(loc,((_tmp641="may clobber unique pointer %s",
_tag_dyneither(_tmp641,sizeof(char),30))),_tag_dyneither(_tmp640,sizeof(void*),1)))))));}
else{const char*_tmp645;void*_tmp644[1];struct Cyc_String_pa_struct _tmp643;(
_tmp643.tag=0,((_tmp643.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpEC),((
_tmp644[0]=& _tmp643,Cyc_Tcutil_warn(loc,((_tmp645="may clobber unique pointer contained in %s",
_tag_dyneither(_tmp645,sizeof(char),43))),_tag_dyneither(_tmp644,sizeof(void*),1)))))));}
goto _LLAC;}_LLAF:;_LLB0: {const char*_tmp648;void*_tmp647;(_tmp647=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp648="warning locations not for VarRoots",
_tag_dyneither(_tmp648,sizeof(char),35))),_tag_dyneither(_tmp647,sizeof(void*),0)));}
_LLAC:;}}}if(env->all_changed == 0)cinfo.consumed=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_rdelete)((
env->fenv)->r,cinfo.consumed,p);return cinfo;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_consume_assignment(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict
outdict,struct Cyc_Absyn_Exp*e);static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_consume_assignment(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo
cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Absyn_Exp*e){struct _RegionHandle*
_tmpF5=env->r;if(Cyc_Tcutil_is_noalias_path(_tmpF5,e)){struct Cyc_CfFlowInfo_FlowEnv*
_tmpF6=env->fenv;struct Cyc_List_List*_tmpF7=0;cinfo=Cyc_NewControlFlow_may_consume_place(
env,cinfo,p,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(struct Cyc_List_List**)&
_tmpF7);while(_tmpF7 != 0){cinfo=Cyc_NewControlFlow_consume_place(env,(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmpF7))->hd,cinfo,outdict,e->loc);_tmpF7=((
struct Cyc_List_List*)_check_null(_tmpF7))->tl;}}return cinfo;}static struct Cyc_List_List*
Cyc_NewControlFlow_noalias_ptrs_aux(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*ts);static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_aux(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*
ts){struct Cyc_List_List*l=0;struct Cyc_CfFlowInfo_Place _tmpF9;void*_tmpFA;struct
Cyc_List_List*_tmpFB;struct Cyc_CfFlowInfo_Place*_tmpF8=p;_tmpF9=*_tmpF8;_tmpFA=(
void*)_tmpF9.root;_tmpFB=_tmpF9.fields;{int fld=0;for(0;ts != 0;(fld ++,ts=ts->tl)){
void*_tmpFC=(void*)ts->hd;if(Cyc_Tcutil_is_noalias_pointer(_tmpFC)){int _tmp649[1];
struct Cyc_List_List*_tmpFD=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(env->r,_tmpFB,((_tmp649[
0]=fld,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
env->r,_tag_dyneither(_tmp649,sizeof(int),1)))));struct Cyc_CfFlowInfo_Place*
_tmp64C;struct Cyc_List_List*_tmp64B;l=((struct Cyc_List_List*(*)(struct
_RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,((
_tmp64B=_region_malloc(env->r,sizeof(*_tmp64B)),((_tmp64B->hd=((_tmp64C=
_region_malloc(env->r,sizeof(*_tmp64C)),((_tmp64C->root=(void*)_tmpFA,((_tmp64C->fields=
_tmpFD,_tmp64C)))))),((_tmp64B->tl=0,_tmp64B)))))),l);}else{if(Cyc_Absyn_is_aggr_type(
_tmpFC)){int _tmp64D[1];struct Cyc_List_List*_tmp101=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
env->r,_tmpFB,((_tmp64D[0]=fld,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(_tmp64D,sizeof(int),
1)))));struct Cyc_CfFlowInfo_Place*_tmp64E;struct Cyc_CfFlowInfo_Place*_tmp102=(
_tmp64E=_region_malloc(env->r,sizeof(*_tmp64E)),((_tmp64E->root=(void*)_tmpFA,((
_tmp64E->fields=_tmp101,_tmp64E)))));l=((struct Cyc_List_List*(*)(struct
_RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,
l,Cyc_NewControlFlow_noalias_ptrs_rec(env,_tmp102,_tmpFC));}}}}return l;}struct
_tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);static
struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,void*t){if(Cyc_Tcutil_is_noalias_pointer(t)){
struct Cyc_List_List*_tmp64F;return(_tmp64F=_region_malloc(env->r,sizeof(*_tmp64F)),((
_tmp64F->hd=p,((_tmp64F->tl=0,_tmp64F)))));}{void*_tmp106=Cyc_Tcutil_compress(t);
struct Cyc_List_List*_tmp107;struct Cyc_List_List*_tmp108;struct Cyc_Absyn_AggrInfo
_tmp109;union Cyc_Absyn_AggrInfoU_union _tmp10A;struct Cyc_Absyn_Aggrdecl**_tmp10B;
struct Cyc_List_List*_tmp10C;struct Cyc_Absyn_AggrInfo _tmp10D;union Cyc_Absyn_AggrInfoU_union
_tmp10E;struct Cyc_Absyn_DatatypeFieldInfo _tmp10F;union Cyc_Absyn_DatatypeFieldInfoU_union
_tmp110;struct Cyc_List_List*_tmp111;_LLB2: if(_tmp106 <= (void*)4)goto _LLBE;if(*((
int*)_tmp106)!= 9)goto _LLB4;_tmp107=((struct Cyc_Absyn_TupleType_struct*)_tmp106)->f1;
_LLB3: {struct Cyc_List_List*_tmp112=0;while(_tmp107 != 0){{struct Cyc_List_List*
_tmp650;_tmp112=((_tmp650=_region_malloc(env->r,sizeof(*_tmp650)),((_tmp650->hd=(
void*)(*((struct _tuple13*)_tmp107->hd)).f2,((_tmp650->tl=_tmp112,_tmp650))))));}
_tmp107=_tmp107->tl;}_tmp112=Cyc_List_imp_rev(_tmp112);return Cyc_NewControlFlow_noalias_ptrs_aux(
env,p,_tmp112);}_LLB4: if(*((int*)_tmp106)!= 11)goto _LLB6;_tmp108=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp106)->f2;_LLB5: {struct Cyc_List_List*_tmp114=0;while(_tmp108 != 0){{struct Cyc_List_List*
_tmp651;_tmp114=((_tmp651=_region_malloc(env->r,sizeof(*_tmp651)),((_tmp651->hd=(
void*)((void*)((struct Cyc_Absyn_Aggrfield*)_tmp108->hd)->type),((_tmp651->tl=
_tmp114,_tmp651))))));}_tmp108=_tmp108->tl;}_tmp114=Cyc_List_imp_rev(_tmp114);
return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp114);}_LLB6: if(*((int*)
_tmp106)!= 10)goto _LLB8;_tmp109=((struct Cyc_Absyn_AggrType_struct*)_tmp106)->f1;
_tmp10A=_tmp109.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp106)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LLB8;_tmp10B=(_tmp10A.KnownAggr).f1;_tmp10C=_tmp109.targs;_LLB7: if((*
_tmp10B)->impl == 0)return 0;else{struct Cyc_List_List*_tmp116=0;struct
_RegionHandle*_tmp117=env->r;{struct Cyc_List_List*_tmp118=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(_tmp117,_tmp117,(*_tmp10B)->tvs,_tmp10C);struct Cyc_List_List*
_tmp119=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp10B)->impl))->fields;
while(_tmp119 != 0){{struct Cyc_List_List*_tmp652;_tmp116=((_tmp652=_region_malloc(
env->r,sizeof(*_tmp652)),((_tmp652->hd=(void*)Cyc_Tcutil_rsubstitute(_tmp117,
_tmp118,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp119->hd)->type),((_tmp652->tl=
_tmp116,_tmp652))))));}_tmp119=_tmp119->tl;}}_tmp116=Cyc_List_imp_rev(_tmp116);
return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp116);}_LLB8: if(*((int*)
_tmp106)!= 10)goto _LLBA;_tmp10D=((struct Cyc_Absyn_AggrType_struct*)_tmp106)->f1;
_tmp10E=_tmp10D.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp106)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LLBA;_LLB9: {const char*_tmp655;void*_tmp654;(_tmp654=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp655="got unknown aggr in noalias_ptrs_rec",
_tag_dyneither(_tmp655,sizeof(char),37))),_tag_dyneither(_tmp654,sizeof(void*),0)));}
_LLBA: if(*((int*)_tmp106)!= 2)goto _LLBC;_LLBB: {struct _RegionHandle*_tmp11D=env->r;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp11D,t)){struct Cyc_List_List*_tmp656;
return(_tmp656=_region_malloc(env->r,sizeof(*_tmp656)),((_tmp656->hd=p,((_tmp656->tl=
0,_tmp656)))));}else{return 0;}}_LLBC: if(*((int*)_tmp106)!= 3)goto _LLBE;_tmp10F=((
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp106)->f1;_tmp110=_tmp10F.field_info;
_tmp111=_tmp10F.targs;_LLBD: {union Cyc_Absyn_DatatypeFieldInfoU_union _tmp11F=
_tmp110;struct Cyc_Absyn_Datatypedecl*_tmp120;struct Cyc_Absyn_Datatypefield*
_tmp121;_LLC1: if((_tmp11F.UnknownDatatypefield).tag != 0)goto _LLC3;_LLC2: {const
char*_tmp659;void*_tmp658;(_tmp658=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp659="got unknown datatype field in noalias_ptrs_rec",
_tag_dyneither(_tmp659,sizeof(char),47))),_tag_dyneither(_tmp658,sizeof(void*),0)));}
_LLC3: if((_tmp11F.KnownDatatypefield).tag != 1)goto _LLC0;_tmp120=(_tmp11F.KnownDatatypefield).f1;
_tmp121=(_tmp11F.KnownDatatypefield).f2;_LLC4: {struct Cyc_List_List*_tmp124=0;
struct _RegionHandle*_tmp125=env->r;{struct Cyc_List_List*_tmp126=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(_tmp125,_tmp125,_tmp120->tvs,_tmp111);struct Cyc_List_List*
_tmp127=_tmp121->typs;while(_tmp127 != 0){{struct Cyc_List_List*_tmp65A;_tmp124=((
_tmp65A=_region_malloc(env->r,sizeof(*_tmp65A)),((_tmp65A->hd=(void*)Cyc_Tcutil_rsubstitute(
_tmp125,_tmp126,(*((struct _tuple13*)_tmp127->hd)).f2),((_tmp65A->tl=_tmp124,
_tmp65A))))));}_tmp127=_tmp127->tl;}}_tmp124=Cyc_List_imp_rev(_tmp124);return Cyc_NewControlFlow_noalias_ptrs_aux(
env,p,_tmp124);}_LLC0:;}_LLBE:;_LLBF: return 0;_LLB1:;}}static struct _tuple5 Cyc_NewControlFlow_do_assign(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal_union lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_CfFlowInfo_ConsumeInfo outer_cinfo,struct Cyc_Position_Segment*
loc);static struct _tuple5 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal_union lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_CfFlowInfo_ConsumeInfo outer_cinfo,struct Cyc_Position_Segment*
loc){outflow=Cyc_CfFlowInfo_consume_unique_rvals(loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp129=outflow;struct Cyc_Dict_Dict _tmp12A;struct Cyc_List_List*_tmp12B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp12C;_LLC6: if((_tmp129.BottomFL).tag != 0)goto _LLC8;_LLC7: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp65D;struct _tuple5 _tmp65C;return(_tmp65C.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp65D.BottomFL).tag=0,_tmp65D)),((_tmp65C.f2=rval,_tmp65C)));}_LLC8: if((
_tmp129.ReachableFL).tag != 1)goto _LLC5;_tmp12A=(_tmp129.ReachableFL).f1;_tmp12B=(
_tmp129.ReachableFL).f2;_tmp12C=(_tmp129.ReachableFL).f3;_LLC9: _tmp12C=Cyc_CfFlowInfo_and_consume(
fenv->r,outer_cinfo,_tmp12C);{union Cyc_CfFlowInfo_AbsLVal_union _tmp12F=lval;
struct Cyc_CfFlowInfo_Place*_tmp130;_LLCB: if((_tmp12F.PlaceL).tag != 0)goto _LLCD;
_tmp130=(_tmp12F.PlaceL).f1;_LLCC: _tmp12C=Cyc_NewControlFlow_consume_assignment(
env,_tmp130,_tmp12C,_tmp12A,lexp);_tmp12A=Cyc_CfFlowInfo_assign_place(fenv,loc,
_tmp12A,env->all_changed,_tmp130,rval);_tmp12B=Cyc_CfFlowInfo_reln_assign_exp(
fenv->r,_tmp12B,lexp,rexp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp65E;outflow=(
union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp65E.ReachableFL).tag=1,(((_tmp65E.ReachableFL).f1=
_tmp12A,(((_tmp65E.ReachableFL).f2=_tmp12B,(((_tmp65E.ReachableFL).f3=_tmp12C,
_tmp65E))))))));}Cyc_NewControlFlow_update_tryflow(env,outflow);{struct _tuple5
_tmp65F;return(_tmp65F.f1=outflow,((_tmp65F.f2=rval,_tmp65F)));}_LLCD: if((
_tmp12F.UnknownL).tag != 1)goto _LLCA;_LLCE: {struct _tuple5 _tmp660;return(_tmp660.f1=
Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp660.f2=rval,
_tmp660)));}_LLCA:;}_LLC5:;}}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e);static void
_tmp701(unsigned int*_tmp700,unsigned int*_tmp6FF,void***_tmp6FD,struct Cyc_List_List**
_tmp2BA){for(*_tmp700=0;*_tmp700 < *_tmp6FF;(*_tmp700)++){void*_tmp6FB;(*_tmp6FD)[*
_tmp700]=((_tmp6FB=(void*)((struct Cyc_List_List*)_check_null(*_tmp2BA))->hd,((*
_tmp2BA=(*_tmp2BA)->tl,_tmp6FB))));}}static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp134=env->fenv;struct Cyc_Dict_Dict
d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo cinfo;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp135=inflow;struct Cyc_Dict_Dict _tmp136;struct Cyc_List_List*_tmp137;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp138;_LLD0: if((_tmp135.BottomFL).tag != 0)goto _LLD2;_LLD1: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp663;struct _tuple5 _tmp662;return(_tmp662.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp663.BottomFL).tag=0,_tmp663)),((_tmp662.f2=(void*)_tmp134->unknown_all,
_tmp662)));}_LLD2: if((_tmp135.ReachableFL).tag != 1)goto _LLCF;_tmp136=(_tmp135.ReachableFL).f1;
_tmp137=(_tmp135.ReachableFL).f2;_tmp138=(_tmp135.ReachableFL).f3;_LLD3: d=
_tmp136;relns=_tmp137;cinfo=_tmp138;_LLCF:;}{void*_tmp13B=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp13C;void*_tmp13D;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13F;
struct Cyc_Absyn_Exp*_tmp140;union Cyc_Absyn_Cnst_union _tmp141;union Cyc_Absyn_Cnst_union
_tmp142;int _tmp143;union Cyc_Absyn_Cnst_union _tmp144;void*_tmp145;struct Cyc_List_List*
_tmp146;void*_tmp147;void*_tmp148;struct Cyc_Absyn_Vardecl*_tmp149;void*_tmp14A;
struct Cyc_Absyn_Vardecl*_tmp14B;void*_tmp14C;struct Cyc_Absyn_Vardecl*_tmp14D;
void*_tmp14E;struct Cyc_List_List*_tmp14F;struct Cyc_Absyn_Exp*_tmp150;struct Cyc_Absyn_Exp*
_tmp151;struct Cyc_Core_Opt*_tmp152;struct Cyc_Core_Opt _tmp153;struct Cyc_Absyn_Exp*
_tmp154;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Core_Opt*_tmp156;struct Cyc_Absyn_Exp*
_tmp157;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Exp*
_tmp15A;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_List_List*_tmp15C;struct Cyc_Absyn_MallocInfo
_tmp15D;int _tmp15E;struct Cyc_Absyn_Exp*_tmp15F;void**_tmp160;struct Cyc_Absyn_Exp*
_tmp161;int _tmp162;struct Cyc_Absyn_Exp*_tmp163;struct Cyc_Absyn_Exp*_tmp164;
struct Cyc_Absyn_Exp*_tmp165;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Exp*
_tmp167;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*_tmp169;struct
_dyneither_ptr*_tmp16A;struct Cyc_Absyn_Exp*_tmp16B;struct _dyneither_ptr*_tmp16C;
struct Cyc_Absyn_Exp*_tmp16D;struct Cyc_Absyn_Exp*_tmp16E;struct Cyc_Absyn_Exp*
_tmp16F;struct Cyc_Absyn_Exp*_tmp170;struct Cyc_Absyn_Exp*_tmp171;struct Cyc_Absyn_Exp*
_tmp172;struct Cyc_Absyn_Exp*_tmp173;struct Cyc_Absyn_Exp*_tmp174;struct Cyc_Absyn_Exp*
_tmp175;struct Cyc_List_List*_tmp176;struct Cyc_Absyn_Datatypedecl*_tmp177;struct
Cyc_List_List*_tmp178;struct Cyc_List_List*_tmp179;struct Cyc_List_List*_tmp17A;
struct Cyc_Absyn_Aggrdecl*_tmp17B;struct Cyc_Absyn_Aggrdecl _tmp17C;void*_tmp17D;
struct Cyc_Absyn_AggrdeclImpl*_tmp17E;struct Cyc_Absyn_AggrdeclImpl _tmp17F;struct
Cyc_List_List*_tmp180;struct Cyc_List_List*_tmp181;struct Cyc_Absyn_Vardecl*
_tmp182;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Exp*_tmp184;int _tmp185;
struct Cyc_Absyn_Stmt*_tmp186;void*_tmp187;_LLD5: if(*((int*)_tmp13B)!= 15)goto
_LLD7;_tmp13C=((struct Cyc_Absyn_Cast_e_struct*)_tmp13B)->f2;_tmp13D=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp13B)->f4;if((int)_tmp13D != 2)goto _LLD7;_LLD6: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp189;void*_tmp18A;struct _tuple5 _tmp188=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp13C);_tmp189=_tmp188.f1;_tmp18A=_tmp188.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp18C;void*_tmp18D;struct _tuple5 _tmp18B=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp189,_tmp13C,_tmp18A);_tmp18C=_tmp18B.f1;_tmp18D=_tmp18B.f2;{struct
_tuple5 _tmp664;return(_tmp664.f1=_tmp18C,((_tmp664.f2=_tmp18A,_tmp664)));}}}
_LLD7: if(*((int*)_tmp13B)!= 15)goto _LLD9;_tmp13E=((struct Cyc_Absyn_Cast_e_struct*)
_tmp13B)->f2;_LLD8: _tmp13F=_tmp13E;goto _LLDA;_LLD9: if(*((int*)_tmp13B)!= 13)goto
_LLDB;_tmp13F=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp13B)->f1;_LLDA:
_tmp140=_tmp13F;goto _LLDC;_LLDB: if(*((int*)_tmp13B)!= 14)goto _LLDD;_tmp140=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmp13B)->f1;_LLDC: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp140);_LLDD: if(*((int*)_tmp13B)!= 0)goto _LLDF;_tmp141=((struct Cyc_Absyn_Const_e_struct*)
_tmp13B)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp13B)->f1).Null_c).tag != 
6)goto _LLDF;_LLDE: goto _LLE0;_LLDF: if(*((int*)_tmp13B)!= 0)goto _LLE1;_tmp142=((
struct Cyc_Absyn_Const_e_struct*)_tmp13B)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp13B)->f1).Int_c).tag != 2)goto _LLE1;_tmp143=(_tmp142.Int_c).f2;if(_tmp143 != 0)
goto _LLE1;_LLE0: {struct _tuple5 _tmp665;return(_tmp665.f1=inflow,((_tmp665.f2=(
void*)0,_tmp665)));}_LLE1: if(*((int*)_tmp13B)!= 0)goto _LLE3;_tmp144=((struct Cyc_Absyn_Const_e_struct*)
_tmp13B)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp13B)->f1).Int_c).tag != 2)
goto _LLE3;_LLE2: goto _LLE4;_LLE3: if(*((int*)_tmp13B)!= 1)goto _LLE5;_tmp145=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp13B)->f2;if(_tmp145 <= (void*)1)goto _LLE5;if(*((
int*)_tmp145)!= 1)goto _LLE5;_LLE4: {struct _tuple5 _tmp666;return(_tmp666.f1=
inflow,((_tmp666.f2=(void*)1,_tmp666)));}_LLE5: if(*((int*)_tmp13B)!= 32)goto
_LLE7;_tmp146=((struct Cyc_Absyn_Datatype_e_struct*)_tmp13B)->f1;if(_tmp146 != 0)
goto _LLE7;_LLE6: goto _LLE8;_LLE7: if(*((int*)_tmp13B)!= 0)goto _LLE9;_LLE8: goto
_LLEA;_LLE9: if(*((int*)_tmp13B)!= 19)goto _LLEB;_LLEA: goto _LLEC;_LLEB: if(*((int*)
_tmp13B)!= 18)goto _LLED;_LLEC: goto _LLEE;_LLED: if(*((int*)_tmp13B)!= 20)goto _LLEF;
_LLEE: goto _LLF0;_LLEF: if(*((int*)_tmp13B)!= 21)goto _LLF1;_LLF0: goto _LLF2;_LLF1:
if(*((int*)_tmp13B)!= 34)goto _LLF3;_LLF2: goto _LLF4;_LLF3: if(*((int*)_tmp13B)!= 
33)goto _LLF5;_LLF4: {struct _tuple5 _tmp667;return(_tmp667.f1=inflow,((_tmp667.f2=(
void*)_tmp134->unknown_all,_tmp667)));}_LLF5: if(*((int*)_tmp13B)!= 1)goto _LLF7;
_tmp147=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp13B)->f2;if(_tmp147 <= (void*)
1)goto _LLF7;if(*((int*)_tmp147)!= 0)goto _LLF7;_LLF6: {struct _tuple5 _tmp668;
return(_tmp668.f1=inflow,((_tmp668.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp134,(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp134->unknown_all),
_tmp668)));}_LLF7: if(*((int*)_tmp13B)!= 1)goto _LLF9;_tmp148=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13B)->f2;if(_tmp148 <= (void*)1)goto _LLF9;if(*((int*)_tmp148)!= 2)goto _LLF9;
_tmp149=((struct Cyc_Absyn_Param_b_struct*)_tmp148)->f1;_LLF8: _tmp14B=_tmp149;
goto _LLFA;_LLF9: if(*((int*)_tmp13B)!= 1)goto _LLFB;_tmp14A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13B)->f2;if(_tmp14A <= (void*)1)goto _LLFB;if(*((int*)_tmp14A)!= 3)goto _LLFB;
_tmp14B=((struct Cyc_Absyn_Local_b_struct*)_tmp14A)->f1;_LLFA: _tmp14D=_tmp14B;
goto _LLFC;_LLFB: if(*((int*)_tmp13B)!= 1)goto _LLFD;_tmp14C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13B)->f2;if(_tmp14C <= (void*)1)goto _LLFD;if(*((int*)_tmp14C)!= 4)goto _LLFD;
_tmp14D=((struct Cyc_Absyn_Pat_b_struct*)_tmp14C)->f1;_LLFC: inflow=Cyc_NewControlFlow_may_consume_exp(
env,inflow,e);{struct Cyc_CfFlowInfo_VarRoot_struct*_tmp66E;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp66D;struct _tuple5 _tmp66C;return(_tmp66C.f1=inflow,((_tmp66C.f2=Cyc_Dict_lookup(
d,(void*)((_tmp66E=_region_malloc(env->r,sizeof(*_tmp66E)),((_tmp66E[0]=((
_tmp66D.tag=0,((_tmp66D.f1=_tmp14D,_tmp66D)))),_tmp66E))))),_tmp66C)));}_LLFD:
if(*((int*)_tmp13B)!= 3)goto _LLFF;_tmp14E=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp13B)->f1;_tmp14F=((struct Cyc_Absyn_Primop_e_struct*)_tmp13B)->f2;_LLFE: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp197;void*_tmp198;struct _tuple5 _tmp196=Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmp14F,0);_tmp197=_tmp196.f1;_tmp198=_tmp196.f2;{void*_tmp199=
_tmp14E;_LL13E: if((int)_tmp199 != 0)goto _LL140;_LL13F: goto _LL141;_LL140: if((int)
_tmp199 != 2)goto _LL142;_LL141: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp14F))->hd)->loc,_tmp197);goto _LL13D;_LL142:;
_LL143: _tmp197=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp14F))->hd)->loc,_tmp197);goto _LL13D;_LL13D:;}{
struct _tuple5 _tmp66F;return(_tmp66F.f1=_tmp197,((_tmp66F.f2=_tmp198,_tmp66F)));}}
_LLFF: if(*((int*)_tmp13B)!= 5)goto _LL101;_tmp150=((struct Cyc_Absyn_Increment_e_struct*)
_tmp13B)->f1;_LL100: {struct Cyc_List_List _tmp670;struct Cyc_List_List _tmp19B=(
_tmp670.hd=_tmp150,((_tmp670.tl=0,_tmp670)));union Cyc_CfFlowInfo_FlowInfo_union
_tmp19D;struct _tuple5 _tmp19C=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct
Cyc_List_List*)& _tmp19B,0);_tmp19D=_tmp19C.f1;Cyc_CfFlowInfo_check_unique_rvals(
_tmp150->loc,_tmp19D);{union Cyc_CfFlowInfo_AbsLVal_union _tmp19F;struct _tuple7
_tmp19E=Cyc_NewControlFlow_anal_Lexp(env,_tmp19D,_tmp150);_tmp19F=_tmp19E.f2;{
struct _tuple12 _tmp671;struct _tuple12 _tmp1A1=(_tmp671.f1=_tmp19F,((_tmp671.f2=
_tmp19D,_tmp671)));union Cyc_CfFlowInfo_AbsLVal_union _tmp1A2;struct Cyc_CfFlowInfo_Place*
_tmp1A3;union Cyc_CfFlowInfo_FlowInfo_union _tmp1A4;struct Cyc_Dict_Dict _tmp1A5;
struct Cyc_List_List*_tmp1A6;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1A7;_LL145:
_tmp1A2=_tmp1A1.f1;if(((_tmp1A1.f1).PlaceL).tag != 0)goto _LL147;_tmp1A3=(_tmp1A2.PlaceL).f1;
_tmp1A4=_tmp1A1.f2;if(((_tmp1A1.f2).ReachableFL).tag != 1)goto _LL147;_tmp1A5=(
_tmp1A4.ReachableFL).f1;_tmp1A6=(_tmp1A4.ReachableFL).f2;_tmp1A7=(_tmp1A4.ReachableFL).f3;
_LL146: _tmp1A6=Cyc_CfFlowInfo_reln_kill_exp(_tmp134->r,_tmp1A6,_tmp150);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp672;_tmp19D=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp672.ReachableFL).tag=1,(((_tmp672.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
_tmp134,_tmp150->loc,_tmp1A5,env->all_changed,_tmp1A3,(void*)_tmp134->unknown_all),(((
_tmp672.ReachableFL).f2=_tmp1A6,(((_tmp672.ReachableFL).f3=_tmp1A7,_tmp672))))))));}
goto _LL144;_LL147:;_LL148: goto _LL144;_LL144:;}{struct _tuple5 _tmp673;return(
_tmp673.f1=_tmp19D,((_tmp673.f2=(void*)_tmp134->unknown_all,_tmp673)));}}}_LL101:
if(*((int*)_tmp13B)!= 4)goto _LL103;_tmp151=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp13B)->f1;_tmp152=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13B)->f2;if(
_tmp152 == 0)goto _LL103;_tmp153=*_tmp152;_tmp154=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp13B)->f3;_LL102: {struct Cyc_List_List _tmp674;struct Cyc_List_List _tmp1AB=(
_tmp674.hd=_tmp154,((_tmp674.tl=0,_tmp674)));struct Cyc_List_List _tmp675;struct
Cyc_List_List _tmp1AC=(_tmp675.hd=_tmp151,((_tmp675.tl=(struct Cyc_List_List*)&
_tmp1AB,_tmp675)));union Cyc_CfFlowInfo_FlowInfo_union _tmp1AE;struct _tuple5
_tmp1AD=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct Cyc_List_List*)&
_tmp1AC,1);_tmp1AE=_tmp1AD.f1;{union Cyc_CfFlowInfo_AbsLVal_union _tmp1B0;struct
_tuple7 _tmp1AF=Cyc_NewControlFlow_anal_Lexp(env,_tmp1AE,_tmp151);_tmp1B0=_tmp1AF.f2;
_tmp1AE=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp1AE);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1B1=_tmp1AE;struct Cyc_Dict_Dict _tmp1B2;struct Cyc_List_List*_tmp1B3;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1B4;_LL14A: if((_tmp1B1.ReachableFL).tag != 1)goto _LL14C;_tmp1B2=(_tmp1B1.ReachableFL).f1;
_tmp1B3=(_tmp1B1.ReachableFL).f2;_tmp1B4=(_tmp1B1.ReachableFL).f3;_LL14B:{union
Cyc_CfFlowInfo_AbsLVal_union _tmp1B5=_tmp1B0;struct Cyc_CfFlowInfo_Place*_tmp1B6;
_LL14F: if((_tmp1B5.PlaceL).tag != 0)goto _LL151;_tmp1B6=(_tmp1B5.PlaceL).f1;_LL150:
_tmp1B4=Cyc_NewControlFlow_consume_assignment(env,_tmp1B6,_tmp1B4,_tmp1B2,
_tmp151);_tmp1B3=Cyc_CfFlowInfo_reln_kill_exp(_tmp134->r,_tmp1B3,_tmp151);
_tmp1B2=Cyc_CfFlowInfo_assign_place(_tmp134,_tmp151->loc,_tmp1B2,env->all_changed,
_tmp1B6,(void*)_tmp134->unknown_all);{union Cyc_CfFlowInfo_FlowInfo_union _tmp676;
_tmp1AE=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp676.ReachableFL).tag=1,(((
_tmp676.ReachableFL).f1=_tmp1B2,(((_tmp676.ReachableFL).f2=_tmp1B3,(((_tmp676.ReachableFL).f3=
_tmp1B4,_tmp676))))))));}goto _LL14E;_LL151: if((_tmp1B5.UnknownL).tag != 1)goto
_LL14E;_LL152: goto _LL14E;_LL14E:;}goto _LL149;_LL14C:;_LL14D: goto _LL149;_LL149:;}{
struct _tuple5 _tmp677;return(_tmp677.f1=_tmp1AE,((_tmp677.f2=(void*)_tmp134->unknown_all,
_tmp677)));}}}_LL103: if(*((int*)_tmp13B)!= 4)goto _LL105;_tmp155=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp13B)->f1;_tmp156=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13B)->f2;if(
_tmp156 != 0)goto _LL105;_tmp157=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13B)->f3;
_LL104: {struct Cyc_Dict_Dict*_tmp1BB=env->all_changed;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1BD;union Cyc_CfFlowInfo_FlowInfo_union _tmp1BE;struct _tuple6 _tmp1BC=Cyc_CfFlowInfo_save_consume_info(
_tmp134,inflow,1);_tmp1BD=_tmp1BC.f1;_tmp1BE=_tmp1BC.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
_tmp678;struct Cyc_CfFlowInfo_ConsumeInfo empty_consume=(_tmp678.consumed=_tmp134->mt_place_set,((
_tmp678.may_consume=0,_tmp678)));struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=
empty_consume;int init_consume=0;while(1){{struct Cyc_Dict_Dict*_tmp679;env->all_changed=((
_tmp679=_region_malloc(env->r,sizeof(*_tmp679)),((_tmp679[0]=_tmp134->mt_place_set,
_tmp679))));}{union Cyc_CfFlowInfo_FlowInfo_union _tmp1C1;union Cyc_CfFlowInfo_AbsLVal_union
_tmp1C2;struct _tuple7 _tmp1C0=Cyc_NewControlFlow_anal_Lexp(env,_tmp1BE,_tmp155);
_tmp1C1=_tmp1C0.f1;_tmp1C2=_tmp1C0.f2;{struct Cyc_Dict_Dict _tmp1C3=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));{struct Cyc_Dict_Dict*_tmp67A;env->all_changed=((
_tmp67A=_region_malloc(env->r,sizeof(*_tmp67A)),((_tmp67A[0]=_tmp134->mt_place_set,
_tmp67A))));}{union Cyc_CfFlowInfo_FlowInfo_union _tmp1C6;void*_tmp1C7;struct
_tuple5 _tmp1C5=Cyc_NewControlFlow_anal_Rexp(env,_tmp1BE,_tmp157);_tmp1C6=_tmp1C5.f1;
_tmp1C7=_tmp1C5.f2;{struct Cyc_Dict_Dict _tmp1C8=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo_union _tmp1C9=Cyc_CfFlowInfo_after_flow(
_tmp134,(struct Cyc_Dict_Dict*)& _tmp1C3,_tmp1C1,_tmp1C6,_tmp1C3,_tmp1C8);union Cyc_CfFlowInfo_FlowInfo_union
_tmp1CA=Cyc_CfFlowInfo_join_flow(_tmp134,_tmp1BB,_tmp1BE,_tmp1C9,1);struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1CC;struct _tuple6 _tmp1CB=Cyc_CfFlowInfo_save_consume_info(_tmp134,_tmp1CA,0);
_tmp1CC=_tmp1CB.f1;if(init_consume){if(!Cyc_CfFlowInfo_consume_approx(
outflow_consume,_tmp1CC)){{const char*_tmp67D;void*_tmp67C;(_tmp67C=0,Cyc_fprintf(
Cyc_stderr,((_tmp67D="sanity consumed: ",_tag_dyneither(_tmp67D,sizeof(char),18))),
_tag_dyneither(_tmp67C,sizeof(void*),0)));}((void(*)(struct Cyc_Dict_Dict p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(outflow_consume.consumed,
Cyc_CfFlowInfo_print_place);{const char*_tmp680;void*_tmp67F;(_tmp67F=0,Cyc_fprintf(
Cyc_stderr,((_tmp680="\ncurrent consumed: ",_tag_dyneither(_tmp680,sizeof(char),
20))),_tag_dyneither(_tmp67F,sizeof(void*),0)));}((void(*)(struct Cyc_Dict_Dict p,
void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(_tmp1CC.consumed,
Cyc_CfFlowInfo_print_place);{const char*_tmp683;void*_tmp682;(_tmp682=0,Cyc_fprintf(
Cyc_stderr,((_tmp683="\nsanity may_consume: ",_tag_dyneither(_tmp683,sizeof(char),
22))),_tag_dyneither(_tmp682,sizeof(void*),0)));}((void(*)(struct Cyc_List_List*p,
void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(
outflow_consume.may_consume,Cyc_CfFlowInfo_print_place);{const char*_tmp686;void*
_tmp685;(_tmp685=0,Cyc_fprintf(Cyc_stderr,((_tmp686="\ncurrent may_consume: ",
_tag_dyneither(_tmp686,sizeof(char),23))),_tag_dyneither(_tmp685,sizeof(void*),0)));}((
void(*)(struct Cyc_List_List*p,void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(
_tmp1CC.may_consume,Cyc_CfFlowInfo_print_place);{const char*_tmp689;void*_tmp688;(
_tmp688=0,Cyc_fprintf(Cyc_stderr,((_tmp689="\n",_tag_dyneither(_tmp689,sizeof(
char),2))),_tag_dyneither(_tmp688,sizeof(void*),0)));}{const char*_tmp68C;void*
_tmp68B;(_tmp68B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp68C="assignOp_e failed to preserve consume info",_tag_dyneither(_tmp68C,
sizeof(char),43))),_tag_dyneither(_tmp68B,sizeof(void*),0)));}}_tmp1BE=Cyc_CfFlowInfo_restore_consume_info(
_tmp1BE,outflow_consume,0);}outflow_consume=_tmp1CC;init_consume=1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp1CA,_tmp1BE)){if(_tmp1BB == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp1BB,Cyc_CfFlowInfo_union_place_set(
_tmp1C3,_tmp1C8,0),0);}return Cyc_NewControlFlow_do_assign(_tmp134,env,_tmp1C9,
_tmp155,_tmp1C2,_tmp157,_tmp1C7,_tmp1BD,e->loc);}_tmp1BE=Cyc_CfFlowInfo_restore_consume_info(
_tmp1CA,empty_consume,0);}}}}}}}_LL105: if(*((int*)_tmp13B)!= 9)goto _LL107;
_tmp158=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp13B)->f1;_tmp159=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp13B)->f2;_LL106: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1DB;void*_tmp1DC;
struct _tuple5 _tmp1DA=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp158);_tmp1DB=
_tmp1DA.f1;_tmp1DC=_tmp1DA.f2;_tmp1DB=Cyc_CfFlowInfo_drop_unique_rvals(_tmp158->loc,
_tmp1DB);return Cyc_NewControlFlow_anal_Rexp(env,_tmp1DB,_tmp159);}_LL107: if(*((
int*)_tmp13B)!= 12)goto _LL109;_tmp15A=((struct Cyc_Absyn_Throw_e_struct*)_tmp13B)->f1;
_LL108: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1DE;void*_tmp1DF;struct _tuple5
_tmp1DD=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp15A);_tmp1DE=_tmp1DD.f1;
_tmp1DF=_tmp1DD.f2;_tmp1DE=Cyc_CfFlowInfo_consume_unique_rvals(_tmp15A->loc,
_tmp1DE);Cyc_NewControlFlow_use_Rval(env,_tmp15A->loc,_tmp1DE,_tmp1DF);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp68F;struct _tuple5 _tmp68E;return(_tmp68E.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp68F.BottomFL).tag=0,_tmp68F)),((_tmp68E.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp134,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp134->unknown_all),
_tmp68E)));}}_LL109: if(*((int*)_tmp13B)!= 11)goto _LL10B;_tmp15B=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp13B)->f1;_tmp15C=((struct Cyc_Absyn_FnCall_e_struct*)_tmp13B)->f2;_LL10A: {
struct _RegionHandle*_tmp1E2=env->r;union Cyc_CfFlowInfo_FlowInfo_union _tmp1E5;
struct Cyc_List_List*_tmp1E6;struct Cyc_List_List*_tmp690;struct _tuple11 _tmp1E4=
Cyc_NewControlFlow_anal_unordered_Rexps(_tmp1E2,env,inflow,((_tmp690=
_region_malloc(_tmp1E2,sizeof(*_tmp690)),((_tmp690->hd=_tmp15B,((_tmp690->tl=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(
_tmp1E2,_tmp15C),_tmp690)))))),1);_tmp1E5=_tmp1E4.f1;_tmp1E6=_tmp1E4.f2;_tmp1E5=
Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp1E5);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1E7=Cyc_NewControlFlow_use_Rval(env,_tmp15B->loc,_tmp1E5,(void*)((struct Cyc_List_List*)
_check_null(_tmp1E6))->hd);_tmp1E6=_tmp1E6->tl;{struct Cyc_List_List*init_params=
0;{void*_tmp1E8=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp15B->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1E9;void*_tmp1EA;_LL154: if(
_tmp1E8 <= (void*)4)goto _LL156;if(*((int*)_tmp1E8)!= 4)goto _LL156;_tmp1E9=((
struct Cyc_Absyn_PointerType_struct*)_tmp1E8)->f1;_tmp1EA=(void*)_tmp1E9.elt_typ;
_LL155:{void*_tmp1EB=Cyc_Tcutil_compress(_tmp1EA);struct Cyc_Absyn_FnInfo _tmp1EC;
struct Cyc_List_List*_tmp1ED;_LL159: if(_tmp1EB <= (void*)4)goto _LL15B;if(*((int*)
_tmp1EB)!= 8)goto _LL15B;_tmp1EC=((struct Cyc_Absyn_FnType_struct*)_tmp1EB)->f1;
_tmp1ED=_tmp1EC.attributes;_LL15A: for(0;_tmp1ED != 0;_tmp1ED=_tmp1ED->tl){void*
_tmp1EE=(void*)_tmp1ED->hd;int _tmp1EF;_LL15E: if(_tmp1EE <= (void*)17)goto _LL160;
if(*((int*)_tmp1EE)!= 4)goto _LL160;_tmp1EF=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1EE)->f1;_LL15F:{struct Cyc_List_List*_tmp691;init_params=((_tmp691=
_region_malloc(_tmp1E2,sizeof(*_tmp691)),((_tmp691->hd=(void*)_tmp1EF,((_tmp691->tl=
init_params,_tmp691))))));}goto _LL15D;_LL160:;_LL161: goto _LL15D;_LL15D:;}goto
_LL158;_LL15B:;_LL15C: {const char*_tmp694;void*_tmp693;(_tmp693=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp694="anal_Rexp: bad function type",
_tag_dyneither(_tmp694,sizeof(char),29))),_tag_dyneither(_tmp693,sizeof(void*),0)));}
_LL158:;}goto _LL153;_LL156:;_LL157: {const char*_tmp697;void*_tmp696;(_tmp696=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp697="anal_Rexp: bad function type",_tag_dyneither(_tmp697,sizeof(char),29))),
_tag_dyneither(_tmp696,sizeof(void*),0)));}_LL153:;}{int i=1;for(0;_tmp1E6 != 0;(((
_tmp1E6=_tmp1E6->tl,_tmp15C=((struct Cyc_List_List*)_check_null(_tmp15C))->tl)),
++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
_tmp1E7=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp15C))->hd)->loc,_tmp1E7,(void*)_tmp1E6->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1F5=_tmp1E5;struct Cyc_Dict_Dict _tmp1F6;_LL163: if((_tmp1F5.BottomFL).tag != 0)
goto _LL165;_LL164: goto _LL162;_LL165: if((_tmp1F5.ReachableFL).tag != 1)goto _LL162;
_tmp1F6=(_tmp1F5.ReachableFL).f1;_LL166: if(Cyc_CfFlowInfo_initlevel(env->fenv,
_tmp1F6,(void*)_tmp1E6->hd)== (void*)0){const char*_tmp69A;void*_tmp699;(_tmp699=
0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp15C))->hd)->loc,((_tmp69A="expression may not be initialized",_tag_dyneither(
_tmp69A,sizeof(char),34))),_tag_dyneither(_tmp699,sizeof(void*),0)));}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1F9=_tmp1E7;struct Cyc_Dict_Dict _tmp1FA;struct Cyc_List_List*_tmp1FB;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1FC;_LL168: if((_tmp1F9.BottomFL).tag != 0)goto _LL16A;_LL169: goto _LL167;_LL16A:
if((_tmp1F9.ReachableFL).tag != 1)goto _LL167;_tmp1FA=(_tmp1F9.ReachableFL).f1;
_tmp1FB=(_tmp1F9.ReachableFL).f2;_tmp1FC=(_tmp1F9.ReachableFL).f3;_LL16B: {
struct Cyc_Dict_Dict _tmp1FD=Cyc_CfFlowInfo_escape_deref(_tmp134,_tmp1FA,env->all_changed,(
void*)_tmp1E6->hd);{void*_tmp1FE=(void*)_tmp1E6->hd;struct Cyc_CfFlowInfo_Place*
_tmp1FF;_LL16D: if(_tmp1FE <= (void*)3)goto _LL16F;if(*((int*)_tmp1FE)!= 2)goto
_LL16F;_tmp1FF=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1FE)->f1;_LL16E:{
void*_tmp200=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp15C))->hd)->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp201;void*_tmp202;_LL172: if(_tmp200 <= (void*)4)goto
_LL174;if(*((int*)_tmp200)!= 4)goto _LL174;_tmp201=((struct Cyc_Absyn_PointerType_struct*)
_tmp200)->f1;_tmp202=(void*)_tmp201.elt_typ;_LL173: _tmp1FD=Cyc_CfFlowInfo_assign_place(
_tmp134,((struct Cyc_Absyn_Exp*)_tmp15C->hd)->loc,_tmp1FD,env->all_changed,
_tmp1FF,Cyc_CfFlowInfo_typ_to_absrval(_tmp134,_tmp202,(void*)_tmp134->esc_all));
goto _LL171;_LL174:;_LL175: {const char*_tmp69D;void*_tmp69C;(_tmp69C=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp69D="anal_Rexp:bad type for initialized arg",
_tag_dyneither(_tmp69D,sizeof(char),39))),_tag_dyneither(_tmp69C,sizeof(void*),0)));}
_LL171:;}goto _LL16C;_LL16F:;_LL170: goto _LL16C;_LL16C:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp69E;_tmp1E7=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp69E.ReachableFL).tag=
1,(((_tmp69E.ReachableFL).f1=_tmp1FD,(((_tmp69E.ReachableFL).f2=_tmp1FB,(((
_tmp69E.ReachableFL).f3=_tmp1FC,_tmp69E))))))));}goto _LL167;}_LL167:;}goto _LL162;
_LL162:;}}}if(Cyc_Tcutil_is_noreturn((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp15B->topt))->v)){union Cyc_CfFlowInfo_FlowInfo_union _tmp6A1;struct _tuple5
_tmp6A0;return(_tmp6A0.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp6A1.BottomFL).tag=
0,_tmp6A1)),((_tmp6A0.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp134,(void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp134->unknown_all),_tmp6A0)));}
else{struct _tuple5 _tmp6A2;return(_tmp6A2.f1=_tmp1E7,((_tmp6A2.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp134,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp134->unknown_all),
_tmp6A2)));}}}}_LL10B: if(*((int*)_tmp13B)!= 35)goto _LL10D;_tmp15D=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp13B)->f1;_tmp15E=_tmp15D.is_calloc;_tmp15F=_tmp15D.rgn;_tmp160=_tmp15D.elt_type;
_tmp161=_tmp15D.num_elts;_tmp162=_tmp15D.fat_result;_LL10C: {struct Cyc_CfFlowInfo_MallocPt_struct
_tmp6A5;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp6A4;void*root=(void*)((_tmp6A4=
_region_malloc(_tmp134->r,sizeof(*_tmp6A4)),((_tmp6A4[0]=((_tmp6A5.tag=1,((
_tmp6A5.f1=_tmp161,((_tmp6A5.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v),_tmp6A5)))))),_tmp6A4))));struct Cyc_CfFlowInfo_Place*_tmp6A6;
struct Cyc_CfFlowInfo_Place*place=(_tmp6A6=_region_malloc(_tmp134->r,sizeof(*
_tmp6A6)),((_tmp6A6->root=(void*)root,((_tmp6A6->fields=0,_tmp6A6)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp6A9;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp6A8;void*rval=(void*)((_tmp6A8=
_region_malloc(_tmp134->r,sizeof(*_tmp6A8)),((_tmp6A8[0]=((_tmp6A9.tag=2,((
_tmp6A9.f1=place,_tmp6A9)))),_tmp6A8))));void*place_val=_tmp162?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(
_tmp134,*((void**)_check_null(_tmp160)),(void*)_tmp134->unknown_none);union Cyc_CfFlowInfo_FlowInfo_union
outflow;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct
Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,
place,0);if(_tmp15F != 0){struct _RegionHandle*_tmp209=env->r;union Cyc_CfFlowInfo_FlowInfo_union
_tmp20C;struct Cyc_List_List*_tmp20D;struct Cyc_Absyn_Exp*_tmp6AA[2];struct
_tuple11 _tmp20B=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp209,env,inflow,((
_tmp6AA[1]=_tmp161,((_tmp6AA[0]=(struct Cyc_Absyn_Exp*)_tmp15F,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp209,
_tag_dyneither(_tmp6AA,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);_tmp20C=_tmp20B.f1;
_tmp20D=_tmp20B.f2;outflow=_tmp20C;}else{union Cyc_CfFlowInfo_FlowInfo_union
_tmp20F;struct _tuple5 _tmp20E=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp161);
_tmp20F=_tmp20E.f1;outflow=_tmp20F;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp161->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp210=outflow;struct
Cyc_Dict_Dict _tmp211;struct Cyc_List_List*_tmp212;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp213;_LL177: if((_tmp210.BottomFL).tag != 0)goto _LL179;_LL178: {struct _tuple5
_tmp6AB;return(_tmp6AB.f1=outflow,((_tmp6AB.f2=rval,_tmp6AB)));}_LL179: if((
_tmp210.ReachableFL).tag != 1)goto _LL176;_tmp211=(_tmp210.ReachableFL).f1;_tmp212=(
_tmp210.ReachableFL).f2;_tmp213=(_tmp210.ReachableFL).f3;_LL17A: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp6AE;struct _tuple5 _tmp6AD;return(_tmp6AD.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp6AE.ReachableFL).tag=1,(((_tmp6AE.ReachableFL).f1=Cyc_Dict_insert(_tmp211,
root,place_val),(((_tmp6AE.ReachableFL).f2=_tmp212,(((_tmp6AE.ReachableFL).f3=
_tmp213,_tmp6AE)))))))),((_tmp6AD.f2=rval,_tmp6AD)));}_LL176:;}}_LL10D: if(*((int*)
_tmp13B)!= 36)goto _LL10F;_tmp163=((struct Cyc_Absyn_Swap_e_struct*)_tmp13B)->f1;
_tmp164=((struct Cyc_Absyn_Swap_e_struct*)_tmp13B)->f2;_LL10E: {void*left_rval;
void*right_rval;union Cyc_CfFlowInfo_FlowInfo_union outflow;struct _RegionHandle*
_tmp21C=env->r;{union Cyc_CfFlowInfo_FlowInfo_union _tmp21F;struct Cyc_List_List*
_tmp220;struct Cyc_Absyn_Exp*_tmp6AF[2];struct _tuple11 _tmp21E=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp21C,env,inflow,((_tmp6AF[1]=_tmp164,((_tmp6AF[0]=_tmp163,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp21C,
_tag_dyneither(_tmp6AF,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);_tmp21F=_tmp21E.f1;
_tmp220=_tmp21E.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp220))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp220->tl))->hd;outflow=
_tmp21F;}Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,outflow);{union Cyc_CfFlowInfo_AbsLVal_union
_tmp222;struct _tuple7 _tmp221=Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmp163);
_tmp222=_tmp221.f2;{union Cyc_CfFlowInfo_AbsLVal_union _tmp224;struct _tuple7
_tmp223=Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmp164);_tmp224=_tmp223.f2;{
union Cyc_CfFlowInfo_FlowInfo_union _tmp225=outflow;struct Cyc_Dict_Dict _tmp226;
struct Cyc_List_List*_tmp227;struct Cyc_CfFlowInfo_ConsumeInfo _tmp228;_LL17C: if((
_tmp225.ReachableFL).tag != 1)goto _LL17E;_tmp226=(_tmp225.ReachableFL).f1;_tmp227=(
_tmp225.ReachableFL).f2;_tmp228=(_tmp225.ReachableFL).f3;_LL17D:{union Cyc_CfFlowInfo_AbsLVal_union
_tmp229=_tmp222;struct Cyc_CfFlowInfo_Place*_tmp22A;_LL181: if((_tmp229.PlaceL).tag
!= 0)goto _LL183;_tmp22A=(_tmp229.PlaceL).f1;_LL182: _tmp226=Cyc_CfFlowInfo_assign_place(
_tmp134,_tmp163->loc,_tmp226,env->all_changed,_tmp22A,right_rval);goto _LL180;
_LL183: if((_tmp229.UnknownL).tag != 1)goto _LL180;_LL184: goto _LL180;_LL180:;}{
union Cyc_CfFlowInfo_AbsLVal_union _tmp22B=_tmp224;struct Cyc_CfFlowInfo_Place*
_tmp22C;_LL186: if((_tmp22B.PlaceL).tag != 0)goto _LL188;_tmp22C=(_tmp22B.PlaceL).f1;
_LL187: _tmp226=Cyc_CfFlowInfo_assign_place(_tmp134,_tmp164->loc,_tmp226,env->all_changed,
_tmp22C,left_rval);goto _LL185;_LL188: if((_tmp22B.UnknownL).tag != 1)goto _LL185;
_LL189: goto _LL185;_LL185:;}_tmp227=Cyc_CfFlowInfo_reln_kill_exp(_tmp134->r,
_tmp227,_tmp163);_tmp227=Cyc_CfFlowInfo_reln_kill_exp(_tmp134->r,_tmp227,_tmp164);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp6B0;outflow=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp6B0.ReachableFL).tag=1,(((_tmp6B0.ReachableFL).f1=_tmp226,(((_tmp6B0.ReachableFL).f2=
_tmp227,(((_tmp6B0.ReachableFL).f3=_tmp228,_tmp6B0))))))));}goto _LL17B;_LL17E:;
_LL17F: goto _LL17B;_LL17B:;}{struct _tuple5 _tmp6B1;return(_tmp6B1.f1=outflow,((
_tmp6B1.f2=(void*)_tmp134->unknown_all,_tmp6B1)));}}}}_LL10F: if(*((int*)_tmp13B)
!= 17)goto _LL111;_tmp165=((struct Cyc_Absyn_New_e_struct*)_tmp13B)->f1;_tmp166=((
struct Cyc_Absyn_New_e_struct*)_tmp13B)->f2;_LL110: {struct Cyc_CfFlowInfo_MallocPt_struct
_tmp6B4;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp6B3;void*root=(void*)((_tmp6B3=
_region_malloc(_tmp134->r,sizeof(*_tmp6B3)),((_tmp6B3[0]=((_tmp6B4.tag=1,((
_tmp6B4.f1=_tmp166,((_tmp6B4.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v),_tmp6B4)))))),_tmp6B3))));struct Cyc_CfFlowInfo_Place*_tmp6B5;
struct Cyc_CfFlowInfo_Place*place=(_tmp6B5=_region_malloc(_tmp134->r,sizeof(*
_tmp6B5)),((_tmp6B5->root=(void*)root,((_tmp6B5->fields=0,_tmp6B5)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp6B8;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp6B7;void*rval=(void*)((_tmp6B7=
_region_malloc(_tmp134->r,sizeof(*_tmp6B7)),((_tmp6B7[0]=((_tmp6B8.tag=2,((
_tmp6B8.f1=place,_tmp6B8)))),_tmp6B7))));((int(*)(struct Cyc_Dict_Dict*set,struct
Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo_union outflow;void*
place_val;if(_tmp165 != 0){struct _RegionHandle*_tmp22F=env->r;union Cyc_CfFlowInfo_FlowInfo_union
_tmp232;struct Cyc_List_List*_tmp233;struct Cyc_Absyn_Exp*_tmp6B9[2];struct
_tuple11 _tmp231=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp22F,env,inflow,((
_tmp6B9[1]=_tmp166,((_tmp6B9[0]=(struct Cyc_Absyn_Exp*)_tmp165,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp22F,
_tag_dyneither(_tmp6B9,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);_tmp232=_tmp231.f1;
_tmp233=_tmp231.f2;outflow=_tmp232;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp233))->tl))->hd;}else{union
Cyc_CfFlowInfo_FlowInfo_union _tmp235;void*_tmp236;struct _tuple5 _tmp234=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp166);_tmp235=_tmp234.f1;_tmp236=_tmp234.f2;outflow=_tmp235;
place_val=_tmp236;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp166->loc,
outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp237=outflow;struct Cyc_Dict_Dict
_tmp238;struct Cyc_List_List*_tmp239;struct Cyc_CfFlowInfo_ConsumeInfo _tmp23A;
_LL18B: if((_tmp237.BottomFL).tag != 0)goto _LL18D;_LL18C: {struct _tuple5 _tmp6BA;
return(_tmp6BA.f1=outflow,((_tmp6BA.f2=rval,_tmp6BA)));}_LL18D: if((_tmp237.ReachableFL).tag
!= 1)goto _LL18A;_tmp238=(_tmp237.ReachableFL).f1;_tmp239=(_tmp237.ReachableFL).f2;
_tmp23A=(_tmp237.ReachableFL).f3;_LL18E: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp6BD;struct _tuple5 _tmp6BC;return(_tmp6BC.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp6BD.ReachableFL).tag=1,(((_tmp6BD.ReachableFL).f1=Cyc_Dict_insert(_tmp238,
root,place_val),(((_tmp6BD.ReachableFL).f2=_tmp239,(((_tmp6BD.ReachableFL).f3=
_tmp23A,_tmp6BD)))))))),((_tmp6BC.f2=rval,_tmp6BC)));}_LL18A:;}}}_LL111: if(*((
int*)_tmp13B)!= 16)goto _LL113;_tmp167=((struct Cyc_Absyn_Address_e_struct*)
_tmp13B)->f1;_LL112: {union Cyc_CfFlowInfo_FlowInfo_union _tmp244;struct _tuple5
_tmp243=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp167);_tmp244=_tmp243.f1;{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp246;struct _tuple6 _tmp245=Cyc_CfFlowInfo_save_consume_info(
env->fenv,_tmp244,0);_tmp246=_tmp245.f1;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp248;union Cyc_CfFlowInfo_AbsLVal_union _tmp249;struct _tuple7 _tmp247=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmp167);_tmp248=_tmp247.f1;_tmp249=_tmp247.f2;_tmp248=Cyc_CfFlowInfo_restore_consume_info(
_tmp248,_tmp246,0);{union Cyc_CfFlowInfo_AbsLVal_union _tmp24A=_tmp249;struct Cyc_CfFlowInfo_Place*
_tmp24B;_LL190: if((_tmp24A.UnknownL).tag != 1)goto _LL192;_LL191: {struct _tuple5
_tmp6BE;return(_tmp6BE.f1=_tmp248,((_tmp6BE.f2=(void*)1,_tmp6BE)));}_LL192: if((
_tmp24A.PlaceL).tag != 0)goto _LL18F;_tmp24B=(_tmp24A.PlaceL).f1;_LL193: {struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp6C4;struct Cyc_CfFlowInfo_AddressOf_struct
_tmp6C3;struct _tuple5 _tmp6C2;return(_tmp6C2.f1=_tmp248,((_tmp6C2.f2=(void*)((
_tmp6C4=_region_malloc(env->r,sizeof(*_tmp6C4)),((_tmp6C4[0]=((_tmp6C3.tag=2,((
_tmp6C3.f1=_tmp24B,_tmp6C3)))),_tmp6C4)))),_tmp6C2)));}_LL18F:;}}}}_LL113: if(*((
int*)_tmp13B)!= 22)goto _LL115;_tmp168=((struct Cyc_Absyn_Deref_e_struct*)_tmp13B)->f1;
_LL114: {union Cyc_CfFlowInfo_FlowInfo_union _tmp251;void*_tmp252;struct _tuple5
_tmp250=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp168);_tmp251=_tmp250.f1;
_tmp252=_tmp250.f2;_tmp251=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp251);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp251,_tmp168,_tmp252);}
_LL115: if(*((int*)_tmp13B)!= 23)goto _LL117;_tmp169=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp13B)->f1;_tmp16A=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp13B)->f2;_LL116: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp254;void*_tmp255;struct _tuple5 _tmp253=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp169);_tmp254=_tmp253.f1;_tmp255=_tmp253.f2;_tmp254=Cyc_CfFlowInfo_drop_unique_rvals(
e->loc,_tmp254);_tmp254=Cyc_NewControlFlow_may_consume_exp(env,_tmp254,e);if(Cyc_Absyn_is_nontagged_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp169->topt))->v)){struct _tuple5
_tmp6C5;return(_tmp6C5.f1=_tmp254,((_tmp6C5.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp134,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp134->unknown_all),
_tmp6C5)));}{void*_tmp257=_tmp255;int _tmp258;struct _dyneither_ptr _tmp259;_LL195:
if(_tmp257 <= (void*)3)goto _LL197;if(*((int*)_tmp257)!= 4)goto _LL197;_tmp258=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp257)->f1;_tmp259=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp257)->f2;_LL196: {int _tmp25A=Cyc_CfFlowInfo_get_field_index((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp169->topt))->v,_tmp16A);struct _tuple5 _tmp6C6;
return(_tmp6C6.f1=_tmp254,((_tmp6C6.f2=*((void**)_check_dyneither_subscript(
_tmp259,sizeof(void*),_tmp25A)),_tmp6C6)));}_LL197:;_LL198: {struct Cyc_Core_Impossible_struct
_tmp6D3;const char*_tmp6D2;void*_tmp6D1[1];struct Cyc_String_pa_struct _tmp6D0;
struct Cyc_Core_Impossible_struct*_tmp6CF;(int)_throw((void*)((_tmp6CF=_cycalloc(
sizeof(*_tmp6CF)),((_tmp6CF[0]=((_tmp6D3.tag=Cyc_Core_Impossible,((_tmp6D3.f1=(
struct _dyneither_ptr)((_tmp6D0.tag=0,((_tmp6D0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp6D1[0]=& _tmp6D0,Cyc_aprintf(((
_tmp6D2="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp6D2,sizeof(char),26))),
_tag_dyneither(_tmp6D1,sizeof(void*),1)))))))),_tmp6D3)))),_tmp6CF)))));}_LL194:;}}
_LL117: if(*((int*)_tmp13B)!= 24)goto _LL119;_tmp16B=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp13B)->f1;_tmp16C=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp13B)->f2;_LL118: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp262;void*_tmp263;struct _tuple5 _tmp261=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp16B);_tmp262=_tmp261.f1;_tmp263=_tmp261.f2;_tmp262=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp262);{union Cyc_CfFlowInfo_FlowInfo_union _tmp265;void*_tmp266;struct
_tuple5 _tmp264=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp262,_tmp16B,_tmp263);
_tmp265=_tmp264.f1;_tmp266=_tmp264.f2;{void*_tmp267=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp16B->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp268;void*_tmp269;_LL19A: if(_tmp267 <= (void*)4)goto _LL19C;if(*((int*)_tmp267)
!= 4)goto _LL19C;_tmp268=((struct Cyc_Absyn_PointerType_struct*)_tmp267)->f1;
_tmp269=(void*)_tmp268.elt_typ;_LL19B: if(Cyc_Absyn_is_nontagged_union_type(
_tmp269)){struct _tuple5 _tmp6D4;return(_tmp6D4.f1=_tmp265,((_tmp6D4.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp134,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp134->unknown_all),
_tmp6D4)));}{void*_tmp26B=_tmp266;int _tmp26C;struct _dyneither_ptr _tmp26D;_LL19F:
if(_tmp26B <= (void*)3)goto _LL1A1;if(*((int*)_tmp26B)!= 4)goto _LL1A1;_tmp26C=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp26B)->f1;_tmp26D=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp26B)->f2;_LL1A0: {int _tmp26E=Cyc_CfFlowInfo_get_field_index(_tmp269,_tmp16C);
struct _tuple5 _tmp6D5;return(_tmp6D5.f1=_tmp265,((_tmp6D5.f2=*((void**)
_check_dyneither_subscript(_tmp26D,sizeof(void*),_tmp26E)),_tmp6D5)));}_LL1A1:;
_LL1A2: {struct Cyc_Core_Impossible_struct _tmp6DB;const char*_tmp6DA;struct Cyc_Core_Impossible_struct*
_tmp6D9;(int)_throw((void*)((_tmp6D9=_cycalloc(sizeof(*_tmp6D9)),((_tmp6D9[0]=((
_tmp6DB.tag=Cyc_Core_Impossible,((_tmp6DB.f1=((_tmp6DA="anal_Rexp: AggrArrow",
_tag_dyneither(_tmp6DA,sizeof(char),21))),_tmp6DB)))),_tmp6D9)))));}_LL19E:;}
_LL19C:;_LL19D: {struct Cyc_Core_Impossible_struct _tmp6E1;const char*_tmp6E0;
struct Cyc_Core_Impossible_struct*_tmp6DF;(int)_throw((void*)((_tmp6DF=_cycalloc(
sizeof(*_tmp6DF)),((_tmp6DF[0]=((_tmp6E1.tag=Cyc_Core_Impossible,((_tmp6E1.f1=((
_tmp6E0="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp6E0,sizeof(char),25))),
_tmp6E1)))),_tmp6DF)))));}_LL199:;}}}_LL119: if(*((int*)_tmp13B)!= 6)goto _LL11B;
_tmp16D=((struct Cyc_Absyn_Conditional_e_struct*)_tmp13B)->f1;_tmp16E=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp13B)->f2;_tmp16F=((struct Cyc_Absyn_Conditional_e_struct*)_tmp13B)->f3;_LL11A: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp277;union Cyc_CfFlowInfo_FlowInfo_union
_tmp278;struct _tuple8 _tmp276=Cyc_NewControlFlow_anal_test(env,inflow,_tmp16D);
_tmp277=_tmp276.f1;_tmp278=_tmp276.f2;_tmp277=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp16D->loc,_tmp277);_tmp278=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp16D->loc,
_tmp278);{struct _tuple5 _tmp279=Cyc_NewControlFlow_anal_Rexp(env,_tmp277,_tmp16E);
struct _tuple5 _tmp27A=Cyc_NewControlFlow_anal_Rexp(env,_tmp278,_tmp16F);return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp134,env->all_changed,_tmp279,_tmp27A,1);}}_LL11B: if(*((int*)_tmp13B)!= 7)
goto _LL11D;_tmp170=((struct Cyc_Absyn_And_e_struct*)_tmp13B)->f1;_tmp171=((struct
Cyc_Absyn_And_e_struct*)_tmp13B)->f2;_LL11C: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp27C;union Cyc_CfFlowInfo_FlowInfo_union _tmp27D;struct _tuple8 _tmp27B=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp170);_tmp27C=_tmp27B.f1;_tmp27D=_tmp27B.f2;_tmp27C=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp170->loc,_tmp27C);_tmp27D=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp170->loc,
_tmp27D);{union Cyc_CfFlowInfo_FlowInfo_union _tmp27F;void*_tmp280;struct _tuple5
_tmp27E=Cyc_NewControlFlow_anal_Rexp(env,_tmp27C,_tmp171);_tmp27F=_tmp27E.f1;
_tmp280=_tmp27E.f2;_tmp27F=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp171->loc,
_tmp27F);{struct _tuple5 _tmp6E2;struct _tuple5 _tmp281=(_tmp6E2.f1=_tmp27F,((
_tmp6E2.f2=_tmp280,_tmp6E2)));struct _tuple5 _tmp6E3;struct _tuple5 _tmp282=(_tmp6E3.f1=
_tmp27D,((_tmp6E3.f2=(void*)((void*)0),_tmp6E3)));return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp134,env->all_changed,_tmp281,_tmp282,0);}}}_LL11D: if(*((int*)_tmp13B)!= 8)
goto _LL11F;_tmp172=((struct Cyc_Absyn_Or_e_struct*)_tmp13B)->f1;_tmp173=((struct
Cyc_Absyn_Or_e_struct*)_tmp13B)->f2;_LL11E: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp286;union Cyc_CfFlowInfo_FlowInfo_union _tmp287;struct _tuple8 _tmp285=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp172);_tmp286=_tmp285.f1;_tmp287=_tmp285.f2;_tmp286=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp172->loc,_tmp286);_tmp287=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp172->loc,
_tmp287);{union Cyc_CfFlowInfo_FlowInfo_union _tmp289;void*_tmp28A;struct _tuple5
_tmp288=Cyc_NewControlFlow_anal_Rexp(env,_tmp287,_tmp173);_tmp289=_tmp288.f1;
_tmp28A=_tmp288.f2;_tmp289=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp173->loc,
_tmp289);{struct _tuple5 _tmp6E4;struct _tuple5 _tmp28B=(_tmp6E4.f1=_tmp289,((
_tmp6E4.f2=_tmp28A,_tmp6E4)));struct _tuple5 _tmp6E5;struct _tuple5 _tmp28C=(_tmp6E5.f1=
_tmp286,((_tmp6E5.f2=(void*)((void*)1),_tmp6E5)));return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp134,env->all_changed,_tmp28B,_tmp28C,0);}}}_LL11F: if(*((int*)_tmp13B)!= 25)
goto _LL121;_tmp174=((struct Cyc_Absyn_Subscript_e_struct*)_tmp13B)->f1;_tmp175=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp13B)->f2;_LL120: {struct _RegionHandle*
_tmp28F=env->r;union Cyc_CfFlowInfo_FlowInfo_union _tmp292;struct Cyc_List_List*
_tmp293;struct Cyc_Absyn_Exp*_tmp6E6[2];struct _tuple11 _tmp291=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp28F,env,inflow,((_tmp6E6[1]=_tmp175,((_tmp6E6[0]=_tmp174,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp28F,
_tag_dyneither(_tmp6E6,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);_tmp292=_tmp291.f1;
_tmp293=_tmp291.f2;_tmp292=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp175->loc,
_tmp292);{union Cyc_CfFlowInfo_FlowInfo_union _tmp294=_tmp292;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp295=_tmp292;struct Cyc_Dict_Dict _tmp296;struct Cyc_List_List*_tmp297;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp298;_LL1A4: if((_tmp295.ReachableFL).tag != 1)goto _LL1A6;_tmp296=(_tmp295.ReachableFL).f1;
_tmp297=(_tmp295.ReachableFL).f2;_tmp298=(_tmp295.ReachableFL).f3;_LL1A5: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp296,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp293))->tl))->hd)== (void*)0){const char*_tmp6E9;void*_tmp6E8;(
_tmp6E8=0,Cyc_Tcutil_terr(_tmp175->loc,((_tmp6E9="expression may not be initialized",
_tag_dyneither(_tmp6E9,sizeof(char),34))),_tag_dyneither(_tmp6E8,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp29B=Cyc_NewControlFlow_add_subscript_reln(_tmp134->r,
_tmp297,_tmp174,_tmp175);if(_tmp297 != _tmp29B){union Cyc_CfFlowInfo_FlowInfo_union
_tmp6EA;_tmp294=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp6EA.ReachableFL).tag=
1,(((_tmp6EA.ReachableFL).f1=_tmp296,(((_tmp6EA.ReachableFL).f2=_tmp29B,(((
_tmp6EA.ReachableFL).f3=_tmp298,_tmp6EA))))))));}goto _LL1A3;}_LL1A6:;_LL1A7: goto
_LL1A3;_LL1A3:;}{void*_tmp29D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp174->topt))->v);struct Cyc_List_List*_tmp29E;struct Cyc_Absyn_PtrInfo
_tmp29F;struct Cyc_Absyn_PtrAtts _tmp2A0;struct Cyc_Absyn_Conref*_tmp2A1;_LL1A9: if(
_tmp29D <= (void*)4)goto _LL1AD;if(*((int*)_tmp29D)!= 9)goto _LL1AB;_tmp29E=((
struct Cyc_Absyn_TupleType_struct*)_tmp29D)->f1;_LL1AA: {void*_tmp2A2=(void*)((
struct Cyc_List_List*)_check_null(_tmp293))->hd;int _tmp2A3;struct _dyneither_ptr
_tmp2A4;_LL1B0: if(_tmp2A2 <= (void*)3)goto _LL1B2;if(*((int*)_tmp2A2)!= 4)goto
_LL1B2;_tmp2A3=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2A2)->f1;_tmp2A4=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2A2)->f2;_LL1B1: _tmp294=Cyc_NewControlFlow_may_consume_exp(
env,_tmp294,e);{unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp175)).f1;struct
_tuple5 _tmp6EB;return(_tmp6EB.f1=_tmp294,((_tmp6EB.f2=*((void**)
_check_dyneither_subscript(_tmp2A4,sizeof(void*),(int)i)),_tmp6EB)));}_LL1B2:;
_LL1B3: {struct Cyc_Core_Impossible_struct _tmp6F1;const char*_tmp6F0;struct Cyc_Core_Impossible_struct*
_tmp6EF;(int)_throw((void*)((_tmp6EF=_cycalloc(sizeof(*_tmp6EF)),((_tmp6EF[0]=((
_tmp6F1.tag=Cyc_Core_Impossible,((_tmp6F1.f1=((_tmp6F0="anal_Rexp: Subscript",
_tag_dyneither(_tmp6F0,sizeof(char),21))),_tmp6F1)))),_tmp6EF)))));}_LL1AF:;}
_LL1AB: if(*((int*)_tmp29D)!= 4)goto _LL1AD;_tmp29F=((struct Cyc_Absyn_PointerType_struct*)
_tmp29D)->f1;_tmp2A0=_tmp29F.ptr_atts;_tmp2A1=_tmp2A0.bounds;_LL1AC: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp2AA;void*_tmp2AB;struct _tuple5 _tmp2A9=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp292,_tmp174,(void*)((struct Cyc_List_List*)_check_null(_tmp293))->hd);
_tmp2AA=_tmp2A9.f1;_tmp2AB=_tmp2A9.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp2AC=
_tmp2AA;_LL1B5: if((_tmp2AC.BottomFL).tag != 0)goto _LL1B7;_LL1B6: {struct _tuple5
_tmp6F2;return(_tmp6F2.f1=_tmp2AA,((_tmp6F2.f2=_tmp2AB,_tmp6F2)));}_LL1B7:;
_LL1B8: {struct _tuple5 _tmp6F3;return(_tmp6F3.f1=_tmp294,((_tmp6F3.f2=_tmp2AB,
_tmp6F3)));}_LL1B4:;}}_LL1AD:;_LL1AE: {struct Cyc_Core_Impossible_struct _tmp6F9;
const char*_tmp6F8;struct Cyc_Core_Impossible_struct*_tmp6F7;(int)_throw((void*)((
_tmp6F7=_cycalloc(sizeof(*_tmp6F7)),((_tmp6F7[0]=((_tmp6F9.tag=Cyc_Core_Impossible,((
_tmp6F9.f1=((_tmp6F8="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp6F8,
sizeof(char),33))),_tmp6F9)))),_tmp6F7)))));}_LL1A8:;}}}_LL121: if(*((int*)
_tmp13B)!= 32)goto _LL123;_tmp176=((struct Cyc_Absyn_Datatype_e_struct*)_tmp13B)->f1;
_tmp177=((struct Cyc_Absyn_Datatype_e_struct*)_tmp13B)->f2;_LL122: if(_tmp177->is_flat){
struct _RegionHandle*_tmp2B2=env->r;union Cyc_CfFlowInfo_FlowInfo_union _tmp2B4;
struct Cyc_List_List*_tmp2B5;struct _tuple11 _tmp2B3=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp2B2,env,inflow,_tmp176,0);_tmp2B4=_tmp2B3.f1;_tmp2B5=_tmp2B3.f2;_tmp2B4=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2B4);for(0;(unsigned int)_tmp176;(_tmp176=_tmp176->tl,_tmp2B5=_tmp2B5->tl)){
_tmp2B4=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)_tmp176->hd)->loc,
_tmp2B4,(void*)((struct Cyc_List_List*)_check_null(_tmp2B5))->hd);}{struct _tuple5
_tmp6FA;return(_tmp6FA.f1=_tmp2B4,((_tmp6FA.f2=(void*)_tmp134->unknown_all,
_tmp6FA)));}}_tmp178=_tmp176;goto _LL124;_LL123: if(*((int*)_tmp13B)!= 26)goto
_LL125;_tmp178=((struct Cyc_Absyn_Tuple_e_struct*)_tmp13B)->f1;_LL124: {struct
_RegionHandle*_tmp2B7=env->r;union Cyc_CfFlowInfo_FlowInfo_union _tmp2B9;struct Cyc_List_List*
_tmp2BA;struct _tuple11 _tmp2B8=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2B7,
env,inflow,_tmp178,0);_tmp2B9=_tmp2B8.f1;_tmp2BA=_tmp2B8.f2;_tmp2B9=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2B9);{unsigned int _tmp700;unsigned int _tmp6FF;struct _dyneither_ptr
_tmp6FE;void**_tmp6FD;unsigned int _tmp6FC;struct _dyneither_ptr aggrdict=(_tmp6FC=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp178),((_tmp6FD=(
void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp6FC)),((_tmp6FE=
_tag_dyneither(_tmp6FD,sizeof(void*),_tmp6FC),((((_tmp6FF=_tmp6FC,_tmp701(&
_tmp700,& _tmp6FF,& _tmp6FD,& _tmp2BA))),_tmp6FE)))))));struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp707;struct Cyc_CfFlowInfo_Aggregate_struct _tmp706;struct _tuple5 _tmp705;return(
_tmp705.f1=_tmp2B9,((_tmp705.f2=(void*)((_tmp707=_region_malloc(env->r,sizeof(*
_tmp707)),((_tmp707[0]=((_tmp706.tag=4,((_tmp706.f1=0,((_tmp706.f2=aggrdict,
_tmp706)))))),_tmp707)))),_tmp705)));}}_LL125: if(*((int*)_tmp13B)!= 31)goto
_LL127;_tmp179=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp13B)->f2;_LL126: {
struct Cyc_List_List*fs;{void*_tmp2C3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_List_List*_tmp2C4;_LL1BA: if(_tmp2C3 <= (void*)
4)goto _LL1BC;if(*((int*)_tmp2C3)!= 11)goto _LL1BC;_tmp2C4=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2C3)->f2;_LL1BB: fs=_tmp2C4;goto _LL1B9;_LL1BC:;_LL1BD: {struct Cyc_Core_Impossible_struct
_tmp70D;const char*_tmp70C;struct Cyc_Core_Impossible_struct*_tmp70B;(int)_throw((
void*)((_tmp70B=_cycalloc(sizeof(*_tmp70B)),((_tmp70B[0]=((_tmp70D.tag=Cyc_Core_Impossible,((
_tmp70D.f1=((_tmp70C="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp70C,
sizeof(char),35))),_tmp70D)))),_tmp70B)))));}_LL1B9:;}_tmp17A=_tmp179;_tmp17D=(
void*)0;_tmp180=fs;goto _LL128;}_LL127: if(*((int*)_tmp13B)!= 30)goto _LL129;
_tmp17A=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp13B)->f3;_tmp17B=((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp13B)->f4;if(_tmp17B == 0)goto _LL129;_tmp17C=*_tmp17B;_tmp17D=(void*)_tmp17C.kind;
_tmp17E=_tmp17C.impl;if(_tmp17E == 0)goto _LL129;_tmp17F=*_tmp17E;_tmp180=_tmp17F.fields;
_LL128: {struct _RegionHandle*_tmp2C8=env->r;union Cyc_CfFlowInfo_FlowInfo_union
_tmp2CA;struct Cyc_List_List*_tmp2CB;struct _tuple11 _tmp2C9=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp2C8,env,inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp2C8,(struct Cyc_Absyn_Exp*(*)(
struct _tuple14*))Cyc_Core_snd,_tmp17A),0);_tmp2CA=_tmp2C9.f1;_tmp2CB=_tmp2C9.f2;
_tmp2CA=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp2CA);{struct
_dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp134,_tmp180,(
void*)_tmp134->unknown_all);{int i=0;for(0;_tmp2CB != 0;(((_tmp2CB=_tmp2CB->tl,
_tmp17A=_tmp17A->tl)),++ i)){struct Cyc_List_List*ds=(*((struct _tuple14*)((struct
Cyc_List_List*)_check_null(_tmp17A))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*
_tmp2CC=(void*)ds->hd;struct _dyneither_ptr*_tmp2CD;_LL1BF: if(*((int*)_tmp2CC)!= 
0)goto _LL1C1;_LL1C0: {struct Cyc_Core_Impossible_struct _tmp713;const char*_tmp712;
struct Cyc_Core_Impossible_struct*_tmp711;(int)_throw((void*)((_tmp711=_cycalloc(
sizeof(*_tmp711)),((_tmp711[0]=((_tmp713.tag=Cyc_Core_Impossible,((_tmp713.f1=((
_tmp712="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp712,sizeof(char),22))),
_tmp713)))),_tmp711)))));}_LL1C1: if(*((int*)_tmp2CC)!= 1)goto _LL1BE;_tmp2CD=((
struct Cyc_Absyn_FieldName_struct*)_tmp2CC)->f1;_LL1C2: {int _tmp2D1=Cyc_CfFlowInfo_get_field_index_fs(
_tmp180,_tmp2CD);*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),
_tmp2D1))=(void*)_tmp2CB->hd;}_LL1BE:;}}}{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp719;struct Cyc_CfFlowInfo_Aggregate_struct _tmp718;struct _tuple5 _tmp717;return(
_tmp717.f1=_tmp2CA,((_tmp717.f2=(void*)((_tmp719=_region_malloc(env->r,sizeof(*
_tmp719)),((_tmp719[0]=((_tmp718.tag=4,((_tmp718.f1=_tmp17D == (void*)1,((_tmp718.f2=
aggrdict,_tmp718)))))),_tmp719)))),_tmp717)));}}}_LL129: if(*((int*)_tmp13B)!= 30)
goto _LL12B;_LL12A: {struct Cyc_Core_Impossible_struct _tmp71F;const char*_tmp71E;
struct Cyc_Core_Impossible_struct*_tmp71D;(int)_throw((void*)((_tmp71D=_cycalloc(
sizeof(*_tmp71D)),((_tmp71D[0]=((_tmp71F.tag=Cyc_Core_Impossible,((_tmp71F.f1=((
_tmp71E="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp71E,sizeof(char),31))),
_tmp71F)))),_tmp71D)))));}_LL12B: if(*((int*)_tmp13B)!= 28)goto _LL12D;_tmp181=((
struct Cyc_Absyn_Array_e_struct*)_tmp13B)->f1;_LL12C: {struct _RegionHandle*
_tmp2D8=env->r;struct Cyc_List_List*_tmp2D9=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple14*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmp2D8,(struct Cyc_Absyn_Exp*(*)(struct _tuple14*))Cyc_Core_snd,
_tmp181);union Cyc_CfFlowInfo_FlowInfo_union _tmp2DB;struct Cyc_List_List*_tmp2DC;
struct _tuple11 _tmp2DA=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2D8,env,inflow,
_tmp2D9,0);_tmp2DB=_tmp2DA.f1;_tmp2DC=_tmp2DA.f2;_tmp2DB=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2DB);for(0;_tmp2DC != 0;(_tmp2DC=_tmp2DC->tl,_tmp2D9=_tmp2D9->tl)){
_tmp2DB=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2D9))->hd)->loc,_tmp2DB,(void*)_tmp2DC->hd);}{struct _tuple5
_tmp720;return(_tmp720.f1=_tmp2DB,((_tmp720.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp134,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp134->unknown_all),
_tmp720)));}}_LL12D: if(*((int*)_tmp13B)!= 29)goto _LL12F;_tmp182=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp13B)->f1;_tmp183=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp13B)->f2;
_tmp184=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp13B)->f3;_tmp185=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp13B)->f4;_LL12E: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp2DF;void*_tmp2E0;struct _tuple5 _tmp2DE=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp183);_tmp2DF=_tmp2DE.f1;_tmp2E0=_tmp2DE.f2;_tmp2DF=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp183->loc,_tmp2DF);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2E1=_tmp2DF;struct
Cyc_Dict_Dict _tmp2E2;struct Cyc_List_List*_tmp2E3;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2E4;_LL1C4: if((_tmp2E1.BottomFL).tag != 0)goto _LL1C6;_LL1C5: {struct _tuple5
_tmp721;return(_tmp721.f1=_tmp2DF,((_tmp721.f2=(void*)_tmp134->unknown_all,
_tmp721)));}_LL1C6: if((_tmp2E1.ReachableFL).tag != 1)goto _LL1C3;_tmp2E2=(_tmp2E1.ReachableFL).f1;
_tmp2E3=(_tmp2E1.ReachableFL).f2;_tmp2E4=(_tmp2E1.ReachableFL).f3;_LL1C7: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp2E2,_tmp2E0)== (void*)0){const char*_tmp724;void*_tmp723;(_tmp723=0,
Cyc_Tcutil_terr(_tmp183->loc,((_tmp724="expression may not be initialized",
_tag_dyneither(_tmp724,sizeof(char),34))),_tag_dyneither(_tmp723,sizeof(void*),0)));}{
struct Cyc_List_List*new_relns=_tmp2E3;comp_loop: {void*_tmp2E8=(void*)_tmp183->r;
struct Cyc_Absyn_Exp*_tmp2E9;void*_tmp2EA;struct Cyc_Absyn_Vardecl*_tmp2EB;void*
_tmp2EC;struct Cyc_Absyn_Vardecl*_tmp2ED;void*_tmp2EE;struct Cyc_Absyn_Vardecl*
_tmp2EF;void*_tmp2F0;struct Cyc_Absyn_Vardecl*_tmp2F1;union Cyc_Absyn_Cnst_union
_tmp2F2;int _tmp2F3;void*_tmp2F4;struct Cyc_List_List*_tmp2F5;struct Cyc_List_List
_tmp2F6;struct Cyc_Absyn_Exp*_tmp2F7;_LL1C9: if(*((int*)_tmp2E8)!= 15)goto _LL1CB;
_tmp2E9=((struct Cyc_Absyn_Cast_e_struct*)_tmp2E8)->f2;_LL1CA: _tmp183=_tmp2E9;
goto comp_loop;_LL1CB: if(*((int*)_tmp2E8)!= 1)goto _LL1CD;_tmp2EA=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp2E8)->f2;if(_tmp2EA <= (void*)1)goto _LL1CD;if(*((int*)
_tmp2EA)!= 0)goto _LL1CD;_tmp2EB=((struct Cyc_Absyn_Global_b_struct*)_tmp2EA)->f1;
if(!(!_tmp2EB->escapes))goto _LL1CD;_LL1CC: _tmp2ED=_tmp2EB;goto _LL1CE;_LL1CD: if(*((
int*)_tmp2E8)!= 1)goto _LL1CF;_tmp2EC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E8)->f2;if(_tmp2EC <= (void*)1)goto _LL1CF;if(*((int*)_tmp2EC)!= 3)goto _LL1CF;
_tmp2ED=((struct Cyc_Absyn_Local_b_struct*)_tmp2EC)->f1;if(!(!_tmp2ED->escapes))
goto _LL1CF;_LL1CE: _tmp2EF=_tmp2ED;goto _LL1D0;_LL1CF: if(*((int*)_tmp2E8)!= 1)goto
_LL1D1;_tmp2EE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E8)->f2;if(_tmp2EE <= (
void*)1)goto _LL1D1;if(*((int*)_tmp2EE)!= 4)goto _LL1D1;_tmp2EF=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2EE)->f1;if(!(!_tmp2EF->escapes))goto _LL1D1;_LL1D0: _tmp2F1=_tmp2EF;goto
_LL1D2;_LL1D1: if(*((int*)_tmp2E8)!= 1)goto _LL1D3;_tmp2F0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E8)->f2;if(_tmp2F0 <= (void*)1)goto _LL1D3;if(*((int*)_tmp2F0)!= 2)goto _LL1D3;
_tmp2F1=((struct Cyc_Absyn_Param_b_struct*)_tmp2F0)->f1;if(!(!_tmp2F1->escapes))
goto _LL1D3;_LL1D2:{struct Cyc_CfFlowInfo_Reln*_tmp72A;union Cyc_CfFlowInfo_RelnOp_union
_tmp729;struct Cyc_List_List*_tmp728;new_relns=((_tmp728=_region_malloc(env->r,
sizeof(*_tmp728)),((_tmp728->hd=((_tmp72A=_region_malloc(env->r,sizeof(*_tmp72A)),((
_tmp72A->vd=_tmp182,((_tmp72A->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((_tmp729.LessVar).tag=
1,(((_tmp729.LessVar).f1=_tmp2F1,(((_tmp729.LessVar).f2=(void*)((void*)_tmp2F1->type),
_tmp729)))))),_tmp72A)))))),((_tmp728->tl=_tmp2E3,_tmp728))))));}goto _LL1C8;
_LL1D3: if(*((int*)_tmp2E8)!= 0)goto _LL1D5;_tmp2F2=((struct Cyc_Absyn_Const_e_struct*)
_tmp2E8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp2E8)->f1).Int_c).tag != 2)
goto _LL1D5;_tmp2F3=(_tmp2F2.Int_c).f2;_LL1D4:{struct Cyc_CfFlowInfo_Reln*_tmp730;
union Cyc_CfFlowInfo_RelnOp_union _tmp72F;struct Cyc_List_List*_tmp72E;new_relns=((
_tmp72E=_region_malloc(env->r,sizeof(*_tmp72E)),((_tmp72E->hd=((_tmp730=
_region_malloc(env->r,sizeof(*_tmp730)),((_tmp730->vd=_tmp182,((_tmp730->rop=(
union Cyc_CfFlowInfo_RelnOp_union)(((_tmp72F.LessConst).tag=3,(((_tmp72F.LessConst).f1=(
unsigned int)_tmp2F3,_tmp72F)))),_tmp730)))))),((_tmp72E->tl=_tmp2E3,_tmp72E))))));}
goto _LL1C8;_LL1D5: if(*((int*)_tmp2E8)!= 3)goto _LL1D7;_tmp2F4=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp2E8)->f1;_tmp2F5=((struct Cyc_Absyn_Primop_e_struct*)_tmp2E8)->f2;if(_tmp2F5
== 0)goto _LL1D7;_tmp2F6=*_tmp2F5;_tmp2F7=(struct Cyc_Absyn_Exp*)_tmp2F6.hd;_LL1D6:{
void*_tmp2FE=(void*)_tmp2F7->r;void*_tmp2FF;struct Cyc_Absyn_Vardecl*_tmp300;void*
_tmp301;struct Cyc_Absyn_Vardecl*_tmp302;void*_tmp303;struct Cyc_Absyn_Vardecl*
_tmp304;void*_tmp305;struct Cyc_Absyn_Vardecl*_tmp306;_LL1DA: if(*((int*)_tmp2FE)
!= 1)goto _LL1DC;_tmp2FF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2FE)->f2;if(
_tmp2FF <= (void*)1)goto _LL1DC;if(*((int*)_tmp2FF)!= 0)goto _LL1DC;_tmp300=((
struct Cyc_Absyn_Global_b_struct*)_tmp2FF)->f1;if(!(!_tmp300->escapes))goto _LL1DC;
_LL1DB: _tmp302=_tmp300;goto _LL1DD;_LL1DC: if(*((int*)_tmp2FE)!= 1)goto _LL1DE;
_tmp301=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2FE)->f2;if(_tmp301 <= (void*)
1)goto _LL1DE;if(*((int*)_tmp301)!= 3)goto _LL1DE;_tmp302=((struct Cyc_Absyn_Local_b_struct*)
_tmp301)->f1;if(!(!_tmp302->escapes))goto _LL1DE;_LL1DD: _tmp304=_tmp302;goto
_LL1DF;_LL1DE: if(*((int*)_tmp2FE)!= 1)goto _LL1E0;_tmp303=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2FE)->f2;if(_tmp303 <= (void*)1)goto _LL1E0;if(*((int*)_tmp303)!= 4)goto _LL1E0;
_tmp304=((struct Cyc_Absyn_Pat_b_struct*)_tmp303)->f1;if(!(!_tmp304->escapes))
goto _LL1E0;_LL1DF: _tmp306=_tmp304;goto _LL1E1;_LL1E0: if(*((int*)_tmp2FE)!= 1)goto
_LL1E2;_tmp305=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2FE)->f2;if(_tmp305 <= (
void*)1)goto _LL1E2;if(*((int*)_tmp305)!= 2)goto _LL1E2;_tmp306=((struct Cyc_Absyn_Param_b_struct*)
_tmp305)->f1;if(!(!_tmp306->escapes))goto _LL1E2;_LL1E1:{struct Cyc_CfFlowInfo_Reln*
_tmp736;union Cyc_CfFlowInfo_RelnOp_union _tmp735;struct Cyc_List_List*_tmp734;
new_relns=((_tmp734=_region_malloc(env->r,sizeof(*_tmp734)),((_tmp734->hd=((
_tmp736=_region_malloc(env->r,sizeof(*_tmp736)),((_tmp736->vd=_tmp182,((_tmp736->rop=(
union Cyc_CfFlowInfo_RelnOp_union)(((_tmp735.LessNumelts).tag=2,(((_tmp735.LessNumelts).f1=
_tmp306,_tmp735)))),_tmp736)))))),((_tmp734->tl=_tmp2E3,_tmp734))))));}goto
_LL1D9;_LL1E2:;_LL1E3: goto _LL1D9;_LL1D9:;}goto _LL1C8;_LL1D7:;_LL1D8: goto _LL1C8;
_LL1C8:;}if(_tmp2E3 != new_relns){union Cyc_CfFlowInfo_FlowInfo_union _tmp737;
_tmp2DF=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp737.ReachableFL).tag=1,(((
_tmp737.ReachableFL).f1=_tmp2E2,(((_tmp737.ReachableFL).f2=new_relns,(((_tmp737.ReachableFL).f3=
_tmp2E4,_tmp737))))))));}{void*_tmp30B=_tmp2E0;_LL1E5: if((int)_tmp30B != 0)goto
_LL1E7;_LL1E6: {struct _tuple5 _tmp738;return(_tmp738.f1=_tmp2DF,((_tmp738.f2=(
void*)_tmp134->unknown_all,_tmp738)));}_LL1E7: if((int)_tmp30B != 2)goto _LL1E9;
_LL1E8: goto _LL1EA;_LL1E9: if((int)_tmp30B != 1)goto _LL1EB;_LL1EA: goto _LL1EC;_LL1EB:
if(_tmp30B <= (void*)3)goto _LL1ED;if(*((int*)_tmp30B)!= 2)goto _LL1ED;_LL1EC: {
struct Cyc_List_List _tmp739;struct Cyc_List_List _tmp30D=(_tmp739.hd=_tmp182,((
_tmp739.tl=0,_tmp739)));_tmp2DF=Cyc_NewControlFlow_add_vars(_tmp134,_tmp2DF,(
struct Cyc_List_List*)& _tmp30D,(void*)_tmp134->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp30F;void*_tmp310;struct _tuple5 _tmp30E=Cyc_NewControlFlow_anal_Rexp(env,
_tmp2DF,_tmp184);_tmp30F=_tmp30E.f1;_tmp310=_tmp30E.f2;_tmp30F=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp184->loc,_tmp30F);{union Cyc_CfFlowInfo_FlowInfo_union _tmp311=_tmp30F;struct
Cyc_Dict_Dict _tmp312;struct Cyc_CfFlowInfo_ConsumeInfo _tmp313;_LL1F0: if((_tmp311.BottomFL).tag
!= 0)goto _LL1F2;_LL1F1: {struct _tuple5 _tmp73A;return(_tmp73A.f1=_tmp30F,((
_tmp73A.f2=(void*)_tmp134->unknown_all,_tmp73A)));}_LL1F2: if((_tmp311.ReachableFL).tag
!= 1)goto _LL1EF;_tmp312=(_tmp311.ReachableFL).f1;_tmp313=(_tmp311.ReachableFL).f3;
_LL1F3: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp312,_tmp310)!= (void*)2){{const
char*_tmp73D;void*_tmp73C;(_tmp73C=0,Cyc_Tcutil_terr(_tmp183->loc,((_tmp73D="expression may not be initialized",
_tag_dyneither(_tmp73D,sizeof(char),34))),_tag_dyneither(_tmp73C,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp740;struct _tuple5 _tmp73F;return(_tmp73F.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp740.BottomFL).tag=0,_tmp740)),((
_tmp73F.f2=(void*)_tmp134->unknown_all,_tmp73F)));}}_LL1EF:;}_tmp2DF=_tmp30F;
goto _LL1EE;}}_LL1ED:;_LL1EE: while(1){struct Cyc_List_List _tmp741;struct Cyc_List_List
_tmp31A=(_tmp741.hd=_tmp182,((_tmp741.tl=0,_tmp741)));_tmp2DF=Cyc_NewControlFlow_add_vars(
_tmp134,_tmp2DF,(struct Cyc_List_List*)& _tmp31A,(void*)_tmp134->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp31C;void*_tmp31D;struct _tuple5 _tmp31B=Cyc_NewControlFlow_anal_Rexp(
env,_tmp2DF,_tmp184);_tmp31C=_tmp31B.f1;_tmp31D=_tmp31B.f2;_tmp31C=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp184->loc,_tmp31C);{union Cyc_CfFlowInfo_FlowInfo_union _tmp31E=_tmp31C;struct
Cyc_Dict_Dict _tmp31F;struct Cyc_CfFlowInfo_ConsumeInfo _tmp320;_LL1F5: if((_tmp31E.BottomFL).tag
!= 0)goto _LL1F7;_LL1F6: goto _LL1F4;_LL1F7: if((_tmp31E.ReachableFL).tag != 1)goto
_LL1F4;_tmp31F=(_tmp31E.ReachableFL).f1;_tmp320=(_tmp31E.ReachableFL).f3;_LL1F8:
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp31F,_tmp31D)!= (void*)2){{const char*
_tmp744;void*_tmp743;(_tmp743=0,Cyc_Tcutil_terr(_tmp183->loc,((_tmp744="expression may not be initialized",
_tag_dyneither(_tmp744,sizeof(char),34))),_tag_dyneither(_tmp743,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp747;struct _tuple5 _tmp746;return(_tmp746.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp747.BottomFL).tag=0,_tmp747)),((
_tmp746.f2=(void*)_tmp134->unknown_all,_tmp746)));}}_LL1F4:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp325=Cyc_CfFlowInfo_join_flow(_tmp134,env->all_changed,_tmp2DF,_tmp31C,1);if(
Cyc_CfFlowInfo_flow_lessthan_approx(_tmp325,_tmp2DF))break;_tmp2DF=_tmp325;}}}{
struct _tuple5 _tmp748;return(_tmp748.f1=_tmp2DF,((_tmp748.f2=(void*)_tmp134->unknown_all,
_tmp748)));}_LL1E4:;}}_LL1C3:;}}_LL12F: if(*((int*)_tmp13B)!= 38)goto _LL131;
_tmp186=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp13B)->f1;_LL130: while(1){union
Cyc_CfFlowInfo_FlowInfo_union*_tmp329;struct _tuple9 _tmp328=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,_tmp186);_tmp329=_tmp328.f2;inflow=*_tmp329;{void*_tmp32A=(void*)
_tmp186->r;struct Cyc_Absyn_Stmt*_tmp32B;struct Cyc_Absyn_Stmt*_tmp32C;struct Cyc_Absyn_Decl*
_tmp32D;struct Cyc_Absyn_Stmt*_tmp32E;struct Cyc_Absyn_Exp*_tmp32F;_LL1FA: if(
_tmp32A <= (void*)1)goto _LL200;if(*((int*)_tmp32A)!= 1)goto _LL1FC;_tmp32B=((
struct Cyc_Absyn_Seq_s_struct*)_tmp32A)->f1;_tmp32C=((struct Cyc_Absyn_Seq_s_struct*)
_tmp32A)->f2;_LL1FB: inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp32B);
_tmp186=_tmp32C;goto _LL1F9;_LL1FC: if(*((int*)_tmp32A)!= 11)goto _LL1FE;_tmp32D=((
struct Cyc_Absyn_Decl_s_struct*)_tmp32A)->f1;_tmp32E=((struct Cyc_Absyn_Decl_s_struct*)
_tmp32A)->f2;_LL1FD: inflow=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp32D);
_tmp186=_tmp32E;goto _LL1F9;_LL1FE: if(*((int*)_tmp32A)!= 0)goto _LL200;_tmp32F=((
struct Cyc_Absyn_Exp_s_struct*)_tmp32A)->f1;_LL1FF: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp32F);_LL200:;_LL201: {struct Cyc_Core_Impossible_struct _tmp74E;
const char*_tmp74D;struct Cyc_Core_Impossible_struct*_tmp74C;(int)_throw((void*)((
_tmp74C=_cycalloc(sizeof(*_tmp74C)),((_tmp74C[0]=((_tmp74E.tag=Cyc_Core_Impossible,((
_tmp74E.f1=((_tmp74D="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp74D,
sizeof(char),33))),_tmp74E)))),_tmp74C)))));}_LL1F9:;}}_LL131: if(*((int*)_tmp13B)
!= 1)goto _LL133;_tmp187=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp13B)->f2;if((
int)_tmp187 != 0)goto _LL133;_LL132: goto _LL134;_LL133: if(*((int*)_tmp13B)!= 2)goto
_LL135;_LL134: goto _LL136;_LL135: if(*((int*)_tmp13B)!= 10)goto _LL137;_LL136: goto
_LL138;_LL137: if(*((int*)_tmp13B)!= 37)goto _LL139;_LL138: goto _LL13A;_LL139: if(*((
int*)_tmp13B)!= 27)goto _LL13B;_LL13A: goto _LL13C;_LL13B: if(*((int*)_tmp13B)!= 39)
goto _LLD4;_LL13C: {struct Cyc_Core_Impossible_struct _tmp754;const char*_tmp753;
struct Cyc_Core_Impossible_struct*_tmp752;(int)_throw((void*)((_tmp752=_cycalloc(
sizeof(*_tmp752)),((_tmp752[0]=((_tmp754.tag=Cyc_Core_Impossible,((_tmp754.f1=((
_tmp753="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp753,sizeof(char),31))),
_tmp754)))),_tmp752)))));}_LLD4:;}}static struct _tuple7 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo_union f,void*r,struct Cyc_List_List*
flds);static struct _tuple7 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo_union
f,void*r,struct Cyc_List_List*flds){struct Cyc_CfFlowInfo_FlowEnv*_tmp336=env->fenv;
void*_tmp337=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp338;void*_tmp339;struct Cyc_Absyn_PtrAtts _tmp33A;
struct Cyc_Absyn_Conref*_tmp33B;struct Cyc_Absyn_Conref*_tmp33C;_LL203: if(_tmp337
<= (void*)4)goto _LL205;if(*((int*)_tmp337)!= 4)goto _LL205;_tmp338=((struct Cyc_Absyn_PointerType_struct*)
_tmp337)->f1;_tmp339=(void*)_tmp338.elt_typ;_tmp33A=_tmp338.ptr_atts;_tmp33B=
_tmp33A.bounds;_tmp33C=_tmp33A.zero_term;_LL204: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp33D=f;struct Cyc_Dict_Dict _tmp33E;struct Cyc_List_List*_tmp33F;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp340;_LL208: if((_tmp33D.BottomFL).tag != 0)goto _LL20A;_LL209: {union Cyc_CfFlowInfo_AbsLVal_union
_tmp757;struct _tuple7 _tmp756;return(_tmp756.f1=f,((_tmp756.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp757.UnknownL).tag=1,_tmp757)),_tmp756)));}_LL20A: if((_tmp33D.ReachableFL).tag
!= 1)goto _LL207;_tmp33E=(_tmp33D.ReachableFL).f1;_tmp33F=(_tmp33D.ReachableFL).f2;
_tmp340=(_tmp33D.ReachableFL).f3;_LL20B: if(Cyc_Tcutil_is_bound_one(_tmp33B)){
void*_tmp343=r;struct Cyc_CfFlowInfo_Place*_tmp344;struct Cyc_CfFlowInfo_Place
_tmp345;void*_tmp346;struct Cyc_List_List*_tmp347;void*_tmp348;_LL20D: if((int)
_tmp343 != 1)goto _LL20F;_LL20E: goto _LL210;_LL20F: if((int)_tmp343 != 2)goto _LL211;
_LL210:{struct Cyc_CfFlowInfo_NotZero_struct _tmp75A;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp759;(void*)(e->annot=(void*)((void*)((_tmp759=_cycalloc(sizeof(*_tmp759)),((
_tmp759[0]=((_tmp75A.tag=Cyc_CfFlowInfo_NotZero,((_tmp75A.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp33F),_tmp75A)))),_tmp759))))));}goto _LL20C;_LL211: if(
_tmp343 <= (void*)3)goto _LL213;if(*((int*)_tmp343)!= 2)goto _LL213;_tmp344=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp343)->f1;_tmp345=*_tmp344;_tmp346=(
void*)_tmp345.root;_tmp347=_tmp345.fields;_LL212:{struct Cyc_CfFlowInfo_NotZero_struct
_tmp75D;struct Cyc_CfFlowInfo_NotZero_struct*_tmp75C;(void*)(e->annot=(void*)((
void*)((_tmp75C=_cycalloc(sizeof(*_tmp75C)),((_tmp75C[0]=((_tmp75D.tag=Cyc_CfFlowInfo_NotZero,((
_tmp75D.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp33F),_tmp75D)))),
_tmp75C))))));}{union Cyc_CfFlowInfo_AbsLVal_union _tmp763;struct Cyc_CfFlowInfo_Place*
_tmp762;struct _tuple7 _tmp761;return(_tmp761.f1=f,((_tmp761.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp763.PlaceL).tag=0,(((_tmp763.PlaceL).f1=((_tmp762=_region_malloc(_tmp336->r,
sizeof(*_tmp762)),((_tmp762->root=(void*)_tmp346,((_tmp762->fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp336->r,_tmp347,flds),_tmp762)))))),_tmp763)))),_tmp761)));}_LL213: if((int)
_tmp343 != 0)goto _LL215;_LL214:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));{
union Cyc_CfFlowInfo_FlowInfo_union _tmp768;union Cyc_CfFlowInfo_AbsLVal_union
_tmp767;struct _tuple7 _tmp766;return(_tmp766.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp768.BottomFL).tag=0,_tmp768)),((_tmp766.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp767.UnknownL).tag=1,_tmp767)),_tmp766)));}_LL215: if(_tmp343 <= (void*)3)goto
_LL217;if(*((int*)_tmp343)!= 0)goto _LL217;_tmp348=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp343)->f1;_LL216: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp348);goto
_LL218;_LL217:;_LL218: {struct Cyc_CfFlowInfo_UnknownZ_struct _tmp76B;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp76A;(void*)(e->annot=(void*)((void*)((_tmp76A=_cycalloc(sizeof(*_tmp76A)),((
_tmp76A[0]=((_tmp76B.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp76B.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp33F),_tmp76B)))),_tmp76A))))));}_LL20C:;}else{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp76E;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp76D;(void*)(e->annot=(void*)((
void*)((_tmp76D=_cycalloc(sizeof(*_tmp76D)),((_tmp76D[0]=((_tmp76E.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp76E.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp33F),_tmp76E)))),
_tmp76D))))));}if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp33E,r)== (void*)0){
const char*_tmp771;void*_tmp770;(_tmp770=0,Cyc_Tcutil_terr(e->loc,((_tmp771="dereference of possibly uninitialized pointer",
_tag_dyneither(_tmp771,sizeof(char),46))),_tag_dyneither(_tmp770,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_AbsLVal_union _tmp774;struct _tuple7 _tmp773;return(_tmp773.f1=
f,((_tmp773.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((_tmp774.UnknownL).tag=1,
_tmp774)),_tmp773)));}_LL207:;}_LL205:;_LL206: {struct Cyc_Core_Impossible_struct
_tmp77A;const char*_tmp779;struct Cyc_Core_Impossible_struct*_tmp778;(int)_throw((
void*)((_tmp778=_cycalloc(sizeof(*_tmp778)),((_tmp778[0]=((_tmp77A.tag=Cyc_Core_Impossible,((
_tmp77A.f1=((_tmp779="left deref of non-pointer-type",_tag_dyneither(_tmp779,
sizeof(char),31))),_tmp77A)))),_tmp778)))));}_LL202:;}static struct _tuple7 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds);static struct _tuple7 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*
_tmp35E=env->fenv;{union Cyc_CfFlowInfo_FlowInfo_union _tmp35F=inflow;struct Cyc_Dict_Dict
_tmp360;struct Cyc_List_List*_tmp361;_LL21A: if((_tmp35F.BottomFL).tag != 0)goto
_LL21C;_LL21B: {union Cyc_CfFlowInfo_FlowInfo_union _tmp77F;union Cyc_CfFlowInfo_AbsLVal_union
_tmp77E;struct _tuple7 _tmp77D;return(_tmp77D.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp77F.BottomFL).tag=0,_tmp77F)),((_tmp77D.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp77E.UnknownL).tag=1,_tmp77E)),_tmp77D)));}_LL21C: if((_tmp35F.ReachableFL).tag
!= 1)goto _LL219;_tmp360=(_tmp35F.ReachableFL).f1;_tmp361=(_tmp35F.ReachableFL).f2;
_LL21D: d=_tmp360;_LL219:;}{void*_tmp365=(void*)e->r;struct Cyc_Absyn_Exp*_tmp366;
struct Cyc_Absyn_Exp*_tmp367;struct Cyc_Absyn_Exp*_tmp368;void*_tmp369;struct Cyc_Absyn_Vardecl*
_tmp36A;void*_tmp36B;struct Cyc_Absyn_Vardecl*_tmp36C;void*_tmp36D;struct Cyc_Absyn_Vardecl*
_tmp36E;void*_tmp36F;struct Cyc_Absyn_Vardecl*_tmp370;struct Cyc_Absyn_Exp*_tmp371;
struct _dyneither_ptr*_tmp372;struct Cyc_Absyn_Exp*_tmp373;struct Cyc_Absyn_Exp*
_tmp374;struct Cyc_Absyn_Exp*_tmp375;struct Cyc_Absyn_Exp*_tmp376;struct
_dyneither_ptr*_tmp377;_LL21F: if(*((int*)_tmp365)!= 15)goto _LL221;_tmp366=((
struct Cyc_Absyn_Cast_e_struct*)_tmp365)->f2;_LL220: _tmp367=_tmp366;goto _LL222;
_LL221: if(*((int*)_tmp365)!= 13)goto _LL223;_tmp367=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp365)->f1;_LL222: _tmp368=_tmp367;goto _LL224;_LL223: if(*((int*)_tmp365)!= 14)
goto _LL225;_tmp368=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp365)->f1;_LL224:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp368,flds);_LL225: if(*((int*)
_tmp365)!= 1)goto _LL227;_tmp369=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp365)->f2;
if(_tmp369 <= (void*)1)goto _LL227;if(*((int*)_tmp369)!= 2)goto _LL227;_tmp36A=((
struct Cyc_Absyn_Param_b_struct*)_tmp369)->f1;_LL226: _tmp36C=_tmp36A;goto _LL228;
_LL227: if(*((int*)_tmp365)!= 1)goto _LL229;_tmp36B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp365)->f2;if(_tmp36B <= (void*)1)goto _LL229;if(*((int*)_tmp36B)!= 3)goto _LL229;
_tmp36C=((struct Cyc_Absyn_Local_b_struct*)_tmp36B)->f1;_LL228: _tmp36E=_tmp36C;
goto _LL22A;_LL229: if(*((int*)_tmp365)!= 1)goto _LL22B;_tmp36D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp365)->f2;if(_tmp36D <= (void*)1)goto _LL22B;if(*((int*)_tmp36D)!= 4)goto _LL22B;
_tmp36E=((struct Cyc_Absyn_Pat_b_struct*)_tmp36D)->f1;_LL22A: {union Cyc_CfFlowInfo_AbsLVal_union
_tmp78E;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp78D;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp78C;struct Cyc_CfFlowInfo_Place*_tmp78B;struct _tuple7 _tmp78A;return(_tmp78A.f1=
inflow,((_tmp78A.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((_tmp78E.PlaceL).tag=0,(((
_tmp78E.PlaceL).f1=((_tmp78B=_region_malloc(env->r,sizeof(*_tmp78B)),((_tmp78B->root=(
void*)((void*)((_tmp78D=_region_malloc(env->r,sizeof(*_tmp78D)),((_tmp78D[0]=((
_tmp78C.tag=0,((_tmp78C.f1=_tmp36E,_tmp78C)))),_tmp78D))))),((_tmp78B->fields=
flds,_tmp78B)))))),_tmp78E)))),_tmp78A)));}_LL22B: if(*((int*)_tmp365)!= 1)goto
_LL22D;_tmp36F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp365)->f2;if(_tmp36F <= (
void*)1)goto _LL22D;if(*((int*)_tmp36F)!= 0)goto _LL22D;_tmp370=((struct Cyc_Absyn_Global_b_struct*)
_tmp36F)->f1;_LL22C: {union Cyc_CfFlowInfo_AbsLVal_union _tmp791;struct _tuple7
_tmp790;return(_tmp790.f1=inflow,((_tmp790.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp791.UnknownL).tag=1,_tmp791)),_tmp790)));}_LL22D: if(*((int*)_tmp365)!= 24)
goto _LL22F;_tmp371=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp365)->f1;_tmp372=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp365)->f2;_LL22E:{void*_tmp37F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp371->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp380;void*_tmp381;_LL238: if(_tmp37F <= (void*)4)goto _LL23A;if(*((int*)_tmp37F)
!= 4)goto _LL23A;_tmp380=((struct Cyc_Absyn_PointerType_struct*)_tmp37F)->f1;
_tmp381=(void*)_tmp380.elt_typ;_LL239: if(!Cyc_Absyn_is_nontagged_union_type(
_tmp381)){struct Cyc_List_List*_tmp792;flds=((_tmp792=_region_malloc(env->r,
sizeof(*_tmp792)),((_tmp792->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp381,
_tmp372),((_tmp792->tl=flds,_tmp792))))));}goto _LL237;_LL23A:;_LL23B: {struct Cyc_Core_Impossible_struct
_tmp798;const char*_tmp797;struct Cyc_Core_Impossible_struct*_tmp796;(int)_throw((
void*)((_tmp796=_cycalloc(sizeof(*_tmp796)),((_tmp796[0]=((_tmp798.tag=Cyc_Core_Impossible,((
_tmp798.f1=((_tmp797="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp797,sizeof(
char),25))),_tmp798)))),_tmp796)))));}_LL237:;}_tmp373=_tmp371;goto _LL230;_LL22F:
if(*((int*)_tmp365)!= 22)goto _LL231;_tmp373=((struct Cyc_Absyn_Deref_e_struct*)
_tmp365)->f1;_LL230: {union Cyc_CfFlowInfo_FlowInfo_union _tmp387;void*_tmp388;
struct _tuple5 _tmp386=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp373);_tmp387=
_tmp386.f1;_tmp388=_tmp386.f2;_tmp387=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp387);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp373,_tmp387,_tmp388,
flds);}_LL231: if(*((int*)_tmp365)!= 25)goto _LL233;_tmp374=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp365)->f1;_tmp375=((struct Cyc_Absyn_Subscript_e_struct*)_tmp365)->f2;_LL232: {
void*_tmp389=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp374->topt))->v);struct Cyc_Absyn_PtrInfo _tmp38A;struct Cyc_Absyn_PtrAtts
_tmp38B;struct Cyc_Absyn_Conref*_tmp38C;_LL23D: if(_tmp389 <= (void*)4)goto _LL241;
if(*((int*)_tmp389)!= 9)goto _LL23F;_LL23E: {unsigned int _tmp38D=(Cyc_Evexp_eval_const_uint_exp(
_tmp375)).f1;struct Cyc_List_List*_tmp799;return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp374,((_tmp799=_region_malloc(env->r,sizeof(*_tmp799)),((_tmp799->hd=(
void*)((int)_tmp38D),((_tmp799->tl=flds,_tmp799)))))));}_LL23F: if(*((int*)
_tmp389)!= 4)goto _LL241;_tmp38A=((struct Cyc_Absyn_PointerType_struct*)_tmp389)->f1;
_tmp38B=_tmp38A.ptr_atts;_tmp38C=_tmp38B.bounds;_LL240: {struct _RegionHandle*
_tmp38F=env->r;union Cyc_CfFlowInfo_FlowInfo_union _tmp392;struct Cyc_List_List*
_tmp393;struct Cyc_Absyn_Exp*_tmp79A[2];struct _tuple11 _tmp391=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp38F,env,inflow,((_tmp79A[1]=_tmp375,((_tmp79A[0]=_tmp374,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp38F,
_tag_dyneither(_tmp79A,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);_tmp392=_tmp391.f1;
_tmp393=_tmp391.f2;_tmp392=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp375->loc,
_tmp392);{union Cyc_CfFlowInfo_FlowInfo_union _tmp394=_tmp392;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp395=_tmp392;struct Cyc_Dict_Dict _tmp396;struct Cyc_List_List*_tmp397;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp398;_LL244: if((_tmp395.ReachableFL).tag != 1)goto _LL246;_tmp396=(_tmp395.ReachableFL).f1;
_tmp397=(_tmp395.ReachableFL).f2;_tmp398=(_tmp395.ReachableFL).f3;_LL245: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp396,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp393))->tl))->hd)== (void*)0){const char*_tmp79D;void*_tmp79C;(
_tmp79C=0,Cyc_Tcutil_terr(_tmp375->loc,((_tmp79D="expression may not be initialized",
_tag_dyneither(_tmp79D,sizeof(char),34))),_tag_dyneither(_tmp79C,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp39B=Cyc_NewControlFlow_add_subscript_reln(_tmp35E->r,
_tmp397,_tmp374,_tmp375);if(_tmp397 != _tmp39B){union Cyc_CfFlowInfo_FlowInfo_union
_tmp79E;_tmp394=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp79E.ReachableFL).tag=
1,(((_tmp79E.ReachableFL).f1=_tmp396,(((_tmp79E.ReachableFL).f2=_tmp39B,(((
_tmp79E.ReachableFL).f3=_tmp398,_tmp79E))))))));}goto _LL243;}_LL246:;_LL247: goto
_LL243;_LL243:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp39E;union Cyc_CfFlowInfo_AbsLVal_union
_tmp39F;struct _tuple7 _tmp39D=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp374,
_tmp392,(void*)((struct Cyc_List_List*)_check_null(_tmp393))->hd,flds);_tmp39E=
_tmp39D.f1;_tmp39F=_tmp39D.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp3A0=_tmp39E;
_LL249: if((_tmp3A0.BottomFL).tag != 0)goto _LL24B;_LL24A: {struct _tuple7 _tmp79F;
return(_tmp79F.f1=_tmp39E,((_tmp79F.f2=_tmp39F,_tmp79F)));}_LL24B:;_LL24C: {
struct _tuple7 _tmp7A0;return(_tmp7A0.f1=_tmp394,((_tmp7A0.f2=_tmp39F,_tmp7A0)));}
_LL248:;}}}}_LL241:;_LL242: {struct Cyc_Core_Impossible_struct _tmp7A6;const char*
_tmp7A5;struct Cyc_Core_Impossible_struct*_tmp7A4;(int)_throw((void*)((_tmp7A4=
_cycalloc(sizeof(*_tmp7A4)),((_tmp7A4[0]=((_tmp7A6.tag=Cyc_Core_Impossible,((
_tmp7A6.f1=((_tmp7A5="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp7A5,
sizeof(char),33))),_tmp7A6)))),_tmp7A4)))));}_LL23C:;}_LL233: if(*((int*)_tmp365)
!= 23)goto _LL235;_tmp376=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp365)->f1;
_tmp377=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp365)->f2;_LL234: if(Cyc_Absyn_is_nontagged_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp376->topt))->v)){union Cyc_CfFlowInfo_AbsLVal_union
_tmp7A9;struct _tuple7 _tmp7A8;return(_tmp7A8.f1=inflow,((_tmp7A8.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp7A9.UnknownL).tag=1,_tmp7A9)),_tmp7A8)));}{struct Cyc_List_List*_tmp7AA;
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp376,((_tmp7AA=
_region_malloc(env->r,sizeof(*_tmp7AA)),((_tmp7AA->hd=(void*)Cyc_CfFlowInfo_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp376->topt))->v,_tmp377),((_tmp7AA->tl=
flds,_tmp7AA)))))));}_LL235:;_LL236: {union Cyc_CfFlowInfo_FlowInfo_union _tmp7AF;
union Cyc_CfFlowInfo_AbsLVal_union _tmp7AE;struct _tuple7 _tmp7AD;return(_tmp7AD.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp7AF.BottomFL).tag=0,_tmp7AF)),((
_tmp7AD.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((_tmp7AE.UnknownL).tag=1,_tmp7AE)),
_tmp7AD)));}_LL21E:;}}static struct _tuple7 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e);static struct
_tuple7 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3AD;struct _tuple6 _tmp3AC=Cyc_CfFlowInfo_save_consume_info(env->fenv,inflow,0);
_tmp3AD=_tmp3AC.f1;{union Cyc_CfFlowInfo_FlowInfo_union _tmp3AF;union Cyc_CfFlowInfo_AbsLVal_union
_tmp3B0;struct _tuple7 _tmp3AE=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,e,0);
_tmp3AF=_tmp3AE.f1;_tmp3B0=_tmp3AE.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp3B2;
struct _tuple6 _tmp3B1=Cyc_CfFlowInfo_save_consume_info(env->fenv,inflow,0);
_tmp3B2=_tmp3B1.f1;if(_tmp3AD.may_consume != _tmp3B2.may_consume  || (_tmp3AD.consumed).t
!= (_tmp3B2.consumed).t){const char*_tmp7B2;void*_tmp7B1;(_tmp7B1=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7B2="oops: anal_Lexp modified consume lists",
_tag_dyneither(_tmp7B2,sizeof(char),39))),_tag_dyneither(_tmp7B1,sizeof(void*),0)));}{
struct _tuple7 _tmp7B3;return(_tmp7B3.f1=_tmp3AF,((_tmp7B3.f2=_tmp3B0,_tmp7B3)));}}}}
static struct _tuple8 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e);static struct
_tuple8 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp3B6=env->fenv;{void*_tmp3B7=(void*)e->r;struct Cyc_Absyn_Exp*_tmp3B8;struct
Cyc_Absyn_Exp*_tmp3B9;struct Cyc_Absyn_Exp*_tmp3BA;struct Cyc_Absyn_Exp*_tmp3BB;
struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_Absyn_Exp*_tmp3BD;struct Cyc_Absyn_Exp*
_tmp3BE;struct Cyc_Absyn_Exp*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3C0;void*_tmp3C1;
struct Cyc_List_List*_tmp3C2;struct Cyc_List_List _tmp3C3;struct Cyc_Absyn_Exp*
_tmp3C4;struct Cyc_List_List*_tmp3C5;void*_tmp3C6;struct Cyc_List_List*_tmp3C7;
_LL24E: if(*((int*)_tmp3B7)!= 6)goto _LL250;_tmp3B8=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp3B7)->f1;_tmp3B9=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3B7)->f2;
_tmp3BA=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3B7)->f3;_LL24F: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3C9;union Cyc_CfFlowInfo_FlowInfo_union _tmp3CA;struct _tuple8 _tmp3C8=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3B8);_tmp3C9=_tmp3C8.f1;_tmp3CA=_tmp3C8.f2;_tmp3C9=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3B8->loc,_tmp3C9);_tmp3CA=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3B8->loc,
_tmp3CA);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3CC;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3CD;struct _tuple8 _tmp3CB=Cyc_NewControlFlow_anal_test(env,_tmp3C9,_tmp3B9);
_tmp3CC=_tmp3CB.f1;_tmp3CD=_tmp3CB.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp3CF;
union Cyc_CfFlowInfo_FlowInfo_union _tmp3D0;struct _tuple8 _tmp3CE=Cyc_NewControlFlow_anal_test(
env,_tmp3CA,_tmp3BA);_tmp3CF=_tmp3CE.f1;_tmp3D0=_tmp3CE.f2;{struct _tuple8 _tmp7B4;
return(_tmp7B4.f1=Cyc_CfFlowInfo_join_flow(_tmp3B6,env->all_changed,_tmp3CC,
_tmp3CF,1),((_tmp7B4.f2=Cyc_CfFlowInfo_join_flow(_tmp3B6,env->all_changed,
_tmp3CD,_tmp3D0,1),_tmp7B4)));}}}}_LL250: if(*((int*)_tmp3B7)!= 7)goto _LL252;
_tmp3BB=((struct Cyc_Absyn_And_e_struct*)_tmp3B7)->f1;_tmp3BC=((struct Cyc_Absyn_And_e_struct*)
_tmp3B7)->f2;_LL251: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3D3;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D4;struct _tuple8 _tmp3D2=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3BB);
_tmp3D3=_tmp3D2.f1;_tmp3D4=_tmp3D2.f2;_tmp3D3=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3BB->loc,_tmp3D3);_tmp3D4=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3BB->loc,
_tmp3D4);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3D6;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D7;struct _tuple8 _tmp3D5=Cyc_NewControlFlow_anal_test(env,_tmp3D3,_tmp3BC);
_tmp3D6=_tmp3D5.f1;_tmp3D7=_tmp3D5.f2;_tmp3D6=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3BC->loc,_tmp3D6);_tmp3D7=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3BC->loc,
_tmp3D7);{struct _tuple8 _tmp7B5;return(_tmp7B5.f1=_tmp3D6,((_tmp7B5.f2=Cyc_CfFlowInfo_join_flow(
_tmp3B6,env->all_changed,_tmp3D4,_tmp3D7,0),_tmp7B5)));}}}_LL252: if(*((int*)
_tmp3B7)!= 8)goto _LL254;_tmp3BD=((struct Cyc_Absyn_Or_e_struct*)_tmp3B7)->f1;
_tmp3BE=((struct Cyc_Absyn_Or_e_struct*)_tmp3B7)->f2;_LL253: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3DA;union Cyc_CfFlowInfo_FlowInfo_union _tmp3DB;struct _tuple8 _tmp3D9=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3BD);_tmp3DA=_tmp3D9.f1;_tmp3DB=_tmp3D9.f2;_tmp3DA=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3BD->loc,_tmp3DA);_tmp3DB=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3BD->loc,
_tmp3DB);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3DD;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3DE;struct _tuple8 _tmp3DC=Cyc_NewControlFlow_anal_test(env,_tmp3DB,_tmp3BE);
_tmp3DD=_tmp3DC.f1;_tmp3DE=_tmp3DC.f2;_tmp3DD=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3BE->loc,_tmp3DD);_tmp3DE=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3BE->loc,
_tmp3DE);{struct _tuple8 _tmp7B6;return(_tmp7B6.f1=Cyc_CfFlowInfo_join_flow(
_tmp3B6,env->all_changed,_tmp3DA,_tmp3DD,0),((_tmp7B6.f2=_tmp3DE,_tmp7B6)));}}}
_LL254: if(*((int*)_tmp3B7)!= 9)goto _LL256;_tmp3BF=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp3B7)->f1;_tmp3C0=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp3B7)->f2;_LL255: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp3E1;void*_tmp3E2;struct _tuple5 _tmp3E0=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp3BF);_tmp3E1=_tmp3E0.f1;_tmp3E2=_tmp3E0.f2;_tmp3E1=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp3BF->loc,_tmp3E1);return Cyc_NewControlFlow_anal_test(env,_tmp3E1,_tmp3C0);}
_LL256: if(*((int*)_tmp3B7)!= 3)goto _LL258;_tmp3C1=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B7)->f1;if((int)_tmp3C1 != 11)goto _LL258;_tmp3C2=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B7)->f2;if(_tmp3C2 == 0)goto _LL258;_tmp3C3=*_tmp3C2;_tmp3C4=(struct Cyc_Absyn_Exp*)
_tmp3C3.hd;_tmp3C5=_tmp3C3.tl;if(_tmp3C5 != 0)goto _LL258;_LL257: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3E4;union Cyc_CfFlowInfo_FlowInfo_union _tmp3E5;struct _tuple8 _tmp3E3=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3C4);_tmp3E4=_tmp3E3.f1;_tmp3E5=_tmp3E3.f2;{struct _tuple8 _tmp7B7;
return(_tmp7B7.f1=_tmp3E5,((_tmp7B7.f2=_tmp3E4,_tmp7B7)));}}_LL258: if(*((int*)
_tmp3B7)!= 3)goto _LL25A;_tmp3C6=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B7)->f1;_tmp3C7=((struct Cyc_Absyn_Primop_e_struct*)_tmp3B7)->f2;_LL259: {
void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo_union f;struct _RegionHandle*_tmp3E7=
env->r;{union Cyc_CfFlowInfo_FlowInfo_union _tmp3E9;struct Cyc_List_List*_tmp3EA;
struct _tuple11 _tmp3E8=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp3E7,env,inflow,
_tmp3C7,0);_tmp3E9=_tmp3E8.f1;_tmp3EA=_tmp3E8.f2;r1=(void*)((struct Cyc_List_List*)
_check_null(_tmp3EA))->hd;r2=(void*)((struct Cyc_List_List*)_check_null(_tmp3EA->tl))->hd;
f=_tmp3E9;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp3EB=f;struct Cyc_Dict_Dict
_tmp3EC;struct Cyc_List_List*_tmp3ED;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3EE;
_LL25D: if((_tmp3EB.BottomFL).tag != 0)goto _LL25F;_LL25E: {struct _tuple8 _tmp7B8;
return(_tmp7B8.f1=f,((_tmp7B8.f2=f,_tmp7B8)));}_LL25F: if((_tmp3EB.ReachableFL).tag
!= 1)goto _LL25C;_tmp3EC=(_tmp3EB.ReachableFL).f1;_tmp3ED=(_tmp3EB.ReachableFL).f2;
_tmp3EE=(_tmp3EB.ReachableFL).f3;_LL260: {struct Cyc_Absyn_Exp*_tmp3F0=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3C7))->hd;struct Cyc_Absyn_Exp*_tmp3F1=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3C7->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp3EC,r1)== (void*)0){const char*_tmp7BB;void*_tmp7BA;(_tmp7BA=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp3C7->hd)->loc,((_tmp7BB="expression may not be initialized",
_tag_dyneither(_tmp7BB,sizeof(char),34))),_tag_dyneither(_tmp7BA,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp3EC,r2)== (void*)0){const char*_tmp7BE;
void*_tmp7BD;(_tmp7BD=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3C7->tl))->hd)->loc,((_tmp7BE="expression may not be initialized",
_tag_dyneither(_tmp7BE,sizeof(char),34))),_tag_dyneither(_tmp7BD,sizeof(void*),0)));}
if(_tmp3C6 == (void*)5  || _tmp3C6 == (void*)6){struct _tuple0 _tmp7BF;struct _tuple0
_tmp3F7=(_tmp7BF.f1=r1,((_tmp7BF.f2=r2,_tmp7BF)));void*_tmp3F8;void*_tmp3F9;void*
_tmp3FA;void*_tmp3FB;void*_tmp3FC;void*_tmp3FD;void*_tmp3FE;void*_tmp3FF;void*
_tmp400;void*_tmp401;void*_tmp402;void*_tmp403;void*_tmp404;void*_tmp405;void*
_tmp406;void*_tmp407;void*_tmp408;void*_tmp409;void*_tmp40A;void*_tmp40B;_LL262:
_tmp3F8=_tmp3F7.f1;if(_tmp3F8 <= (void*)3)goto _LL264;if(*((int*)_tmp3F8)!= 0)goto
_LL264;_tmp3F9=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3F8)->f1;
_tmp3FA=_tmp3F7.f2;if((int)_tmp3FA != 0)goto _LL264;_LL263: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp40D;union Cyc_CfFlowInfo_FlowInfo_union _tmp40E;struct _tuple8 _tmp40C=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp3F0,_tmp3F9);_tmp40D=_tmp40C.f1;_tmp40E=_tmp40C.f2;{void*_tmp40F=
_tmp3C6;_LL277: if((int)_tmp40F != 5)goto _LL279;_LL278: {struct _tuple8 _tmp7C0;
return(_tmp7C0.f1=_tmp40E,((_tmp7C0.f2=_tmp40D,_tmp7C0)));}_LL279: if((int)
_tmp40F != 6)goto _LL27B;_LL27A: {struct _tuple8 _tmp7C1;return(_tmp7C1.f1=_tmp40D,((
_tmp7C1.f2=_tmp40E,_tmp7C1)));}_LL27B:;_LL27C: {struct Cyc_Core_Impossible_struct
_tmp7C7;const char*_tmp7C6;struct Cyc_Core_Impossible_struct*_tmp7C5;(int)_throw((
void*)((_tmp7C5=_cycalloc(sizeof(*_tmp7C5)),((_tmp7C5[0]=((_tmp7C7.tag=Cyc_Core_Impossible,((
_tmp7C7.f1=((_tmp7C6="anal_test, zero-split",_tag_dyneither(_tmp7C6,sizeof(char),
22))),_tmp7C7)))),_tmp7C5)))));}_LL276:;}}_LL264: _tmp3FB=_tmp3F7.f1;if((int)
_tmp3FB != 0)goto _LL266;_tmp3FC=_tmp3F7.f2;if(_tmp3FC <= (void*)3)goto _LL266;if(*((
int*)_tmp3FC)!= 0)goto _LL266;_tmp3FD=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp3FC)->f1;_LL265: {union Cyc_CfFlowInfo_FlowInfo_union _tmp416;union Cyc_CfFlowInfo_FlowInfo_union
_tmp417;struct _tuple8 _tmp415=Cyc_NewControlFlow_splitzero(env,f,f,_tmp3F1,
_tmp3FD);_tmp416=_tmp415.f1;_tmp417=_tmp415.f2;{void*_tmp418=_tmp3C6;_LL27E: if((
int)_tmp418 != 5)goto _LL280;_LL27F: {struct _tuple8 _tmp7C8;return(_tmp7C8.f1=
_tmp417,((_tmp7C8.f2=_tmp416,_tmp7C8)));}_LL280: if((int)_tmp418 != 6)goto _LL282;
_LL281: {struct _tuple8 _tmp7C9;return(_tmp7C9.f1=_tmp416,((_tmp7C9.f2=_tmp417,
_tmp7C9)));}_LL282:;_LL283: {struct Cyc_Core_Impossible_struct _tmp7CF;const char*
_tmp7CE;struct Cyc_Core_Impossible_struct*_tmp7CD;(int)_throw((void*)((_tmp7CD=
_cycalloc(sizeof(*_tmp7CD)),((_tmp7CD[0]=((_tmp7CF.tag=Cyc_Core_Impossible,((
_tmp7CF.f1=((_tmp7CE="anal_test, zero-split",_tag_dyneither(_tmp7CE,sizeof(char),
22))),_tmp7CF)))),_tmp7CD)))));}_LL27D:;}}_LL266: _tmp3FE=_tmp3F7.f1;if((int)
_tmp3FE != 0)goto _LL268;_tmp3FF=_tmp3F7.f2;if((int)_tmp3FF != 0)goto _LL268;_LL267:
if(_tmp3C6 == (void*)5){union Cyc_CfFlowInfo_FlowInfo_union _tmp7D2;struct _tuple8
_tmp7D1;return(_tmp7D1.f1=f,((_tmp7D1.f2=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7D2.BottomFL).tag=0,_tmp7D2)),_tmp7D1)));}else{union Cyc_CfFlowInfo_FlowInfo_union
_tmp7D5;struct _tuple8 _tmp7D4;return(_tmp7D4.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7D5.BottomFL).tag=0,_tmp7D5)),((_tmp7D4.f2=f,_tmp7D4)));}_LL268: _tmp400=
_tmp3F7.f1;if((int)_tmp400 != 0)goto _LL26A;_tmp401=_tmp3F7.f2;if((int)_tmp401 != 1)
goto _LL26A;_LL269: goto _LL26B;_LL26A: _tmp402=_tmp3F7.f1;if((int)_tmp402 != 0)goto
_LL26C;_tmp403=_tmp3F7.f2;if((int)_tmp403 != 2)goto _LL26C;_LL26B: goto _LL26D;
_LL26C: _tmp404=_tmp3F7.f1;if((int)_tmp404 != 0)goto _LL26E;_tmp405=_tmp3F7.f2;if(
_tmp405 <= (void*)3)goto _LL26E;if(*((int*)_tmp405)!= 2)goto _LL26E;_LL26D: goto
_LL26F;_LL26E: _tmp406=_tmp3F7.f1;if((int)_tmp406 != 1)goto _LL270;_tmp407=_tmp3F7.f2;
if((int)_tmp407 != 0)goto _LL270;_LL26F: goto _LL271;_LL270: _tmp408=_tmp3F7.f1;if((
int)_tmp408 != 2)goto _LL272;_tmp409=_tmp3F7.f2;if((int)_tmp409 != 0)goto _LL272;
_LL271: goto _LL273;_LL272: _tmp40A=_tmp3F7.f1;if(_tmp40A <= (void*)3)goto _LL274;if(*((
int*)_tmp40A)!= 2)goto _LL274;_tmp40B=_tmp3F7.f2;if((int)_tmp40B != 0)goto _LL274;
_LL273: if(_tmp3C6 == (void*)6){union Cyc_CfFlowInfo_FlowInfo_union _tmp7D8;struct
_tuple8 _tmp7D7;return(_tmp7D7.f1=f,((_tmp7D7.f2=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7D8.BottomFL).tag=0,_tmp7D8)),_tmp7D7)));}else{union Cyc_CfFlowInfo_FlowInfo_union
_tmp7DB;struct _tuple8 _tmp7DA;return(_tmp7DA.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7DB.BottomFL).tag=0,_tmp7DB)),((_tmp7DA.f2=f,_tmp7DA)));}_LL274:;_LL275: goto
_LL261;_LL261:;}{struct _tuple0 _tmp7DC;struct _tuple0 _tmp427=(_tmp7DC.f1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F0->topt))->v),((_tmp7DC.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F1->topt))->v),_tmp7DC)));void*
_tmp428;void*_tmp429;void*_tmp42A;void*_tmp42B;void*_tmp42C;void*_tmp42D;_LL285:
_tmp428=_tmp427.f1;if(_tmp428 <= (void*)4)goto _LL287;if(*((int*)_tmp428)!= 5)goto
_LL287;_tmp429=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp428)->f1;if((int)
_tmp429 != 1)goto _LL287;_LL286: goto _LL288;_LL287: _tmp42A=_tmp427.f2;if(_tmp42A <= (
void*)4)goto _LL289;if(*((int*)_tmp42A)!= 5)goto _LL289;_tmp42B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp42A)->f1;if((int)_tmp42B != 1)goto _LL289;_LL288: goto _LL28A;_LL289: _tmp42C=
_tmp427.f1;if(_tmp42C <= (void*)4)goto _LL28B;if(*((int*)_tmp42C)!= 18)goto _LL28B;
_LL28A: goto _LL28C;_LL28B: _tmp42D=_tmp427.f2;if(_tmp42D <= (void*)4)goto _LL28D;if(*((
int*)_tmp42D)!= 18)goto _LL28D;_LL28C: goto _LL284;_LL28D:;_LL28E: {struct _tuple8
_tmp7DD;return(_tmp7DD.f1=f,((_tmp7DD.f2=f,_tmp7DD)));}_LL284:;}{void*_tmp42F=
_tmp3C6;_LL290: if((int)_tmp42F != 5)goto _LL292;_LL291: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp430=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F0,r1,(void*)5,r2);_tmp430=Cyc_NewControlFlow_if_tagcmp(
env,_tmp430,_tmp3F1,r2,(void*)5,r1);{struct _tuple8 _tmp7DE;return(_tmp7DE.f1=
_tmp430,((_tmp7DE.f2=f,_tmp7DE)));}}_LL292: if((int)_tmp42F != 6)goto _LL294;_LL293: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp432=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3F0,r1,(void*)5,r2);_tmp432=Cyc_NewControlFlow_if_tagcmp(env,_tmp432,_tmp3F1,
r2,(void*)5,r1);{struct _tuple8 _tmp7DF;return(_tmp7DF.f1=f,((_tmp7DF.f2=_tmp432,
_tmp7DF)));}}_LL294: if((int)_tmp42F != 7)goto _LL296;_LL295: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp434=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F1,r2,(void*)8,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp435=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F0,r1,(void*)10,r2);struct
_tuple8 _tmp7E0;return(_tmp7E0.f1=_tmp434,((_tmp7E0.f2=_tmp435,_tmp7E0)));}_LL296:
if((int)_tmp42F != 9)goto _LL298;_LL297: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp437=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F1,r2,(void*)10,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp438=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F0,r1,(void*)8,r2);struct _tuple8
_tmp7E1;return(_tmp7E1.f1=_tmp437,((_tmp7E1.f2=_tmp438,_tmp7E1)));}_LL298: if((
int)_tmp42F != 8)goto _LL29A;_LL299: {union Cyc_CfFlowInfo_FlowInfo_union _tmp43A=
Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F0,r1,(void*)8,r2);union Cyc_CfFlowInfo_FlowInfo_union
_tmp43B=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F1,r2,(void*)10,r1);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp43C=_tmp43A;struct Cyc_Dict_Dict _tmp43D;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp43E;_LL29F: if((_tmp43C.BottomFL).tag != 0)goto _LL2A1;_LL2A0: {struct Cyc_Core_Impossible_struct
_tmp7E7;const char*_tmp7E6;struct Cyc_Core_Impossible_struct*_tmp7E5;(int)_throw((
void*)((_tmp7E5=_cycalloc(sizeof(*_tmp7E5)),((_tmp7E5[0]=((_tmp7E7.tag=Cyc_Core_Impossible,((
_tmp7E7.f1=((_tmp7E6="anal_test, Lt",_tag_dyneither(_tmp7E6,sizeof(char),14))),
_tmp7E7)))),_tmp7E5)))));}_LL2A1: if((_tmp43C.ReachableFL).tag != 1)goto _LL29E;
_tmp43D=(_tmp43C.ReachableFL).f1;_tmp43E=(_tmp43C.ReachableFL).f3;_LL2A2: _tmp3EC=
_tmp43D;_tmp3EE=_tmp43E;_LL29E:;}{void*_tmp442=(void*)_tmp3F0->r;void*_tmp443;
struct Cyc_Absyn_Vardecl*_tmp444;void*_tmp445;struct Cyc_Absyn_Vardecl*_tmp446;
void*_tmp447;struct Cyc_Absyn_Vardecl*_tmp448;void*_tmp449;struct Cyc_Absyn_Vardecl*
_tmp44A;_LL2A4: if(*((int*)_tmp442)!= 1)goto _LL2A6;_tmp443=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp442)->f2;if(_tmp443 <= (void*)1)goto _LL2A6;if(*((int*)_tmp443)!= 0)goto _LL2A6;
_tmp444=((struct Cyc_Absyn_Global_b_struct*)_tmp443)->f1;if(!(!_tmp444->escapes))
goto _LL2A6;_LL2A5: _tmp446=_tmp444;goto _LL2A7;_LL2A6: if(*((int*)_tmp442)!= 1)goto
_LL2A8;_tmp445=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp442)->f2;if(_tmp445 <= (
void*)1)goto _LL2A8;if(*((int*)_tmp445)!= 3)goto _LL2A8;_tmp446=((struct Cyc_Absyn_Local_b_struct*)
_tmp445)->f1;if(!(!_tmp446->escapes))goto _LL2A8;_LL2A7: _tmp448=_tmp446;goto
_LL2A9;_LL2A8: if(*((int*)_tmp442)!= 1)goto _LL2AA;_tmp447=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp442)->f2;if(_tmp447 <= (void*)1)goto _LL2AA;if(*((int*)_tmp447)!= 4)goto _LL2AA;
_tmp448=((struct Cyc_Absyn_Pat_b_struct*)_tmp447)->f1;if(!(!_tmp448->escapes))
goto _LL2AA;_LL2A9: _tmp44A=_tmp448;goto _LL2AB;_LL2AA: if(*((int*)_tmp442)!= 1)goto
_LL2AC;_tmp449=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp442)->f2;if(_tmp449 <= (
void*)1)goto _LL2AC;if(*((int*)_tmp449)!= 2)goto _LL2AC;_tmp44A=((struct Cyc_Absyn_Param_b_struct*)
_tmp449)->f1;if(!(!_tmp44A->escapes))goto _LL2AC;_LL2AB: {void*_tmp44B=(void*)
_tmp3F1->r;void*_tmp44C;struct Cyc_Absyn_Vardecl*_tmp44D;void*_tmp44E;struct Cyc_Absyn_Vardecl*
_tmp44F;void*_tmp450;struct Cyc_Absyn_Vardecl*_tmp451;void*_tmp452;struct Cyc_Absyn_Vardecl*
_tmp453;union Cyc_Absyn_Cnst_union _tmp454;int _tmp455;void*_tmp456;struct Cyc_List_List*
_tmp457;struct Cyc_List_List _tmp458;struct Cyc_Absyn_Exp*_tmp459;_LL2AF: if(*((int*)
_tmp44B)!= 1)goto _LL2B1;_tmp44C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp44B)->f2;
if(_tmp44C <= (void*)1)goto _LL2B1;if(*((int*)_tmp44C)!= 0)goto _LL2B1;_tmp44D=((
struct Cyc_Absyn_Global_b_struct*)_tmp44C)->f1;if(!(!_tmp44D->escapes))goto _LL2B1;
_LL2B0: _tmp44F=_tmp44D;goto _LL2B2;_LL2B1: if(*((int*)_tmp44B)!= 1)goto _LL2B3;
_tmp44E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp44B)->f2;if(_tmp44E <= (void*)
1)goto _LL2B3;if(*((int*)_tmp44E)!= 3)goto _LL2B3;_tmp44F=((struct Cyc_Absyn_Local_b_struct*)
_tmp44E)->f1;if(!(!_tmp44F->escapes))goto _LL2B3;_LL2B2: _tmp451=_tmp44F;goto
_LL2B4;_LL2B3: if(*((int*)_tmp44B)!= 1)goto _LL2B5;_tmp450=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp44B)->f2;if(_tmp450 <= (void*)1)goto _LL2B5;if(*((int*)_tmp450)!= 4)goto _LL2B5;
_tmp451=((struct Cyc_Absyn_Pat_b_struct*)_tmp450)->f1;if(!(!_tmp451->escapes))
goto _LL2B5;_LL2B4: _tmp453=_tmp451;goto _LL2B6;_LL2B5: if(*((int*)_tmp44B)!= 1)goto
_LL2B7;_tmp452=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp44B)->f2;if(_tmp452 <= (
void*)1)goto _LL2B7;if(*((int*)_tmp452)!= 2)goto _LL2B7;_tmp453=((struct Cyc_Absyn_Param_b_struct*)
_tmp452)->f1;if(!(!_tmp453->escapes))goto _LL2B7;_LL2B6: {struct _RegionHandle*
_tmp45A=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp7ED;union Cyc_CfFlowInfo_RelnOp_union
_tmp7EC;struct Cyc_List_List*_tmp7EB;struct Cyc_List_List*_tmp45B=(_tmp7EB=
_region_malloc(_tmp45A,sizeof(*_tmp7EB)),((_tmp7EB->hd=((_tmp7ED=_region_malloc(
_tmp45A,sizeof(*_tmp7ED)),((_tmp7ED->vd=_tmp44A,((_tmp7ED->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((
_tmp7EC.LessVar).tag=1,(((_tmp7EC.LessVar).f1=_tmp453,(((_tmp7EC.LessVar).f2=(
void*)((void*)_tmp453->type),_tmp7EC)))))),_tmp7ED)))))),((_tmp7EB->tl=_tmp3ED,
_tmp7EB)))));union Cyc_CfFlowInfo_FlowInfo_union _tmp7F0;struct _tuple8 _tmp7EF;
return(_tmp7EF.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp7F0.ReachableFL).tag=
1,(((_tmp7F0.ReachableFL).f1=_tmp3EC,(((_tmp7F0.ReachableFL).f2=_tmp45B,(((
_tmp7F0.ReachableFL).f3=_tmp3EE,_tmp7F0)))))))),((_tmp7EF.f2=_tmp43B,_tmp7EF)));}
_LL2B7: if(*((int*)_tmp44B)!= 0)goto _LL2B9;_tmp454=((struct Cyc_Absyn_Const_e_struct*)
_tmp44B)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp44B)->f1).Int_c).tag != 2)
goto _LL2B9;_tmp455=(_tmp454.Int_c).f2;_LL2B8: {struct _RegionHandle*_tmp461=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp7F6;union Cyc_CfFlowInfo_RelnOp_union _tmp7F5;struct
Cyc_List_List*_tmp7F4;struct Cyc_List_List*_tmp462=(_tmp7F4=_region_malloc(
_tmp461,sizeof(*_tmp7F4)),((_tmp7F4->hd=((_tmp7F6=_region_malloc(_tmp461,sizeof(*
_tmp7F6)),((_tmp7F6->vd=_tmp44A,((_tmp7F6->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((
_tmp7F5.LessConst).tag=3,(((_tmp7F5.LessConst).f1=(unsigned int)_tmp455,_tmp7F5)))),
_tmp7F6)))))),((_tmp7F4->tl=_tmp3ED,_tmp7F4)))));union Cyc_CfFlowInfo_FlowInfo_union
_tmp7F9;struct _tuple8 _tmp7F8;return(_tmp7F8.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7F9.ReachableFL).tag=1,(((_tmp7F9.ReachableFL).f1=_tmp3EC,(((_tmp7F9.ReachableFL).f2=
_tmp462,(((_tmp7F9.ReachableFL).f3=_tmp3EE,_tmp7F9)))))))),((_tmp7F8.f2=_tmp43B,
_tmp7F8)));}_LL2B9: if(*((int*)_tmp44B)!= 3)goto _LL2BB;_tmp456=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp44B)->f1;_tmp457=((struct Cyc_Absyn_Primop_e_struct*)_tmp44B)->f2;if(_tmp457
== 0)goto _LL2BB;_tmp458=*_tmp457;_tmp459=(struct Cyc_Absyn_Exp*)_tmp458.hd;_LL2BA: {
void*_tmp468=(void*)_tmp459->r;void*_tmp469;struct Cyc_Absyn_Vardecl*_tmp46A;void*
_tmp46B;struct Cyc_Absyn_Vardecl*_tmp46C;void*_tmp46D;struct Cyc_Absyn_Vardecl*
_tmp46E;void*_tmp46F;struct Cyc_Absyn_Vardecl*_tmp470;_LL2BE: if(*((int*)_tmp468)
!= 1)goto _LL2C0;_tmp469=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp468)->f2;if(
_tmp469 <= (void*)1)goto _LL2C0;if(*((int*)_tmp469)!= 0)goto _LL2C0;_tmp46A=((
struct Cyc_Absyn_Global_b_struct*)_tmp469)->f1;if(!(!_tmp46A->escapes))goto _LL2C0;
_LL2BF: _tmp46C=_tmp46A;goto _LL2C1;_LL2C0: if(*((int*)_tmp468)!= 1)goto _LL2C2;
_tmp46B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp468)->f2;if(_tmp46B <= (void*)
1)goto _LL2C2;if(*((int*)_tmp46B)!= 3)goto _LL2C2;_tmp46C=((struct Cyc_Absyn_Local_b_struct*)
_tmp46B)->f1;if(!(!_tmp46C->escapes))goto _LL2C2;_LL2C1: _tmp46E=_tmp46C;goto
_LL2C3;_LL2C2: if(*((int*)_tmp468)!= 1)goto _LL2C4;_tmp46D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp468)->f2;if(_tmp46D <= (void*)1)goto _LL2C4;if(*((int*)_tmp46D)!= 4)goto _LL2C4;
_tmp46E=((struct Cyc_Absyn_Pat_b_struct*)_tmp46D)->f1;if(!(!_tmp46E->escapes))
goto _LL2C4;_LL2C3: _tmp470=_tmp46E;goto _LL2C5;_LL2C4: if(*((int*)_tmp468)!= 1)goto
_LL2C6;_tmp46F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp468)->f2;if(_tmp46F <= (
void*)1)goto _LL2C6;if(*((int*)_tmp46F)!= 2)goto _LL2C6;_tmp470=((struct Cyc_Absyn_Param_b_struct*)
_tmp46F)->f1;if(!(!_tmp470->escapes))goto _LL2C6;_LL2C5: {struct _RegionHandle*
_tmp471=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp7FF;union Cyc_CfFlowInfo_RelnOp_union
_tmp7FE;struct Cyc_List_List*_tmp7FD;struct Cyc_List_List*_tmp472=(_tmp7FD=
_region_malloc(_tmp471,sizeof(*_tmp7FD)),((_tmp7FD->hd=((_tmp7FF=_region_malloc(
_tmp471,sizeof(*_tmp7FF)),((_tmp7FF->vd=_tmp44A,((_tmp7FF->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((
_tmp7FE.LessNumelts).tag=2,(((_tmp7FE.LessNumelts).f1=_tmp470,_tmp7FE)))),
_tmp7FF)))))),((_tmp7FD->tl=_tmp3ED,_tmp7FD)))));union Cyc_CfFlowInfo_FlowInfo_union
_tmp802;struct _tuple8 _tmp801;return(_tmp801.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp802.ReachableFL).tag=1,(((_tmp802.ReachableFL).f1=_tmp3EC,(((_tmp802.ReachableFL).f2=
_tmp472,(((_tmp802.ReachableFL).f3=_tmp3EE,_tmp802)))))))),((_tmp801.f2=_tmp43B,
_tmp801)));}_LL2C6:;_LL2C7: {struct _tuple8 _tmp803;return(_tmp803.f1=_tmp43A,((
_tmp803.f2=_tmp43B,_tmp803)));}_LL2BD:;}_LL2BB:;_LL2BC: {struct _tuple8 _tmp804;
return(_tmp804.f1=_tmp43A,((_tmp804.f2=_tmp43B,_tmp804)));}_LL2AE:;}_LL2AC:;
_LL2AD: {struct _tuple8 _tmp805;return(_tmp805.f1=_tmp43A,((_tmp805.f2=_tmp43B,
_tmp805)));}_LL2A3:;}}_LL29A: if((int)_tmp42F != 10)goto _LL29C;_LL29B: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp47B=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F0,r1,(void*)10,r2);union Cyc_CfFlowInfo_FlowInfo_union
_tmp47C=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F1,r1,(void*)8,r1);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp47D=_tmp47B;struct Cyc_Dict_Dict _tmp47E;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp47F;_LL2C9: if((_tmp47D.BottomFL).tag != 0)goto _LL2CB;_LL2CA: {struct Cyc_Core_Impossible_struct
_tmp80B;const char*_tmp80A;struct Cyc_Core_Impossible_struct*_tmp809;(int)_throw((
void*)((_tmp809=_cycalloc(sizeof(*_tmp809)),((_tmp809[0]=((_tmp80B.tag=Cyc_Core_Impossible,((
_tmp80B.f1=((_tmp80A="anal_test, Lte",_tag_dyneither(_tmp80A,sizeof(char),15))),
_tmp80B)))),_tmp809)))));}_LL2CB: if((_tmp47D.ReachableFL).tag != 1)goto _LL2C8;
_tmp47E=(_tmp47D.ReachableFL).f1;_tmp47F=(_tmp47D.ReachableFL).f3;_LL2CC: _tmp3EC=
_tmp47E;_tmp3EE=_tmp47F;_LL2C8:;}{void*_tmp483=(void*)_tmp3F0->r;void*_tmp484;
struct Cyc_Absyn_Vardecl*_tmp485;void*_tmp486;struct Cyc_Absyn_Vardecl*_tmp487;
void*_tmp488;struct Cyc_Absyn_Vardecl*_tmp489;void*_tmp48A;struct Cyc_Absyn_Vardecl*
_tmp48B;_LL2CE: if(*((int*)_tmp483)!= 1)goto _LL2D0;_tmp484=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp483)->f2;if(_tmp484 <= (void*)1)goto _LL2D0;if(*((int*)_tmp484)!= 0)goto _LL2D0;
_tmp485=((struct Cyc_Absyn_Global_b_struct*)_tmp484)->f1;if(!(!_tmp485->escapes))
goto _LL2D0;_LL2CF: _tmp487=_tmp485;goto _LL2D1;_LL2D0: if(*((int*)_tmp483)!= 1)goto
_LL2D2;_tmp486=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp483)->f2;if(_tmp486 <= (
void*)1)goto _LL2D2;if(*((int*)_tmp486)!= 3)goto _LL2D2;_tmp487=((struct Cyc_Absyn_Local_b_struct*)
_tmp486)->f1;if(!(!_tmp487->escapes))goto _LL2D2;_LL2D1: _tmp489=_tmp487;goto
_LL2D3;_LL2D2: if(*((int*)_tmp483)!= 1)goto _LL2D4;_tmp488=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp483)->f2;if(_tmp488 <= (void*)1)goto _LL2D4;if(*((int*)_tmp488)!= 4)goto _LL2D4;
_tmp489=((struct Cyc_Absyn_Pat_b_struct*)_tmp488)->f1;if(!(!_tmp489->escapes))
goto _LL2D4;_LL2D3: _tmp48B=_tmp489;goto _LL2D5;_LL2D4: if(*((int*)_tmp483)!= 1)goto
_LL2D6;_tmp48A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp483)->f2;if(_tmp48A <= (
void*)1)goto _LL2D6;if(*((int*)_tmp48A)!= 2)goto _LL2D6;_tmp48B=((struct Cyc_Absyn_Param_b_struct*)
_tmp48A)->f1;if(!(!_tmp48B->escapes))goto _LL2D6;_LL2D5: {void*_tmp48C=(void*)
_tmp3F1->r;void*_tmp48D;struct Cyc_List_List*_tmp48E;struct Cyc_List_List _tmp48F;
struct Cyc_Absyn_Exp*_tmp490;_LL2D9: if(*((int*)_tmp48C)!= 3)goto _LL2DB;_tmp48D=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp48C)->f1;_tmp48E=((struct Cyc_Absyn_Primop_e_struct*)
_tmp48C)->f2;if(_tmp48E == 0)goto _LL2DB;_tmp48F=*_tmp48E;_tmp490=(struct Cyc_Absyn_Exp*)
_tmp48F.hd;_LL2DA: {void*_tmp491=(void*)_tmp490->r;void*_tmp492;struct Cyc_Absyn_Vardecl*
_tmp493;void*_tmp494;struct Cyc_Absyn_Vardecl*_tmp495;void*_tmp496;struct Cyc_Absyn_Vardecl*
_tmp497;void*_tmp498;struct Cyc_Absyn_Vardecl*_tmp499;_LL2DE: if(*((int*)_tmp491)
!= 1)goto _LL2E0;_tmp492=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp491)->f2;if(
_tmp492 <= (void*)1)goto _LL2E0;if(*((int*)_tmp492)!= 0)goto _LL2E0;_tmp493=((
struct Cyc_Absyn_Global_b_struct*)_tmp492)->f1;if(!(!_tmp493->escapes))goto _LL2E0;
_LL2DF: _tmp495=_tmp493;goto _LL2E1;_LL2E0: if(*((int*)_tmp491)!= 1)goto _LL2E2;
_tmp494=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp491)->f2;if(_tmp494 <= (void*)
1)goto _LL2E2;if(*((int*)_tmp494)!= 3)goto _LL2E2;_tmp495=((struct Cyc_Absyn_Local_b_struct*)
_tmp494)->f1;if(!(!_tmp495->escapes))goto _LL2E2;_LL2E1: _tmp497=_tmp495;goto
_LL2E3;_LL2E2: if(*((int*)_tmp491)!= 1)goto _LL2E4;_tmp496=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp491)->f2;if(_tmp496 <= (void*)1)goto _LL2E4;if(*((int*)_tmp496)!= 4)goto _LL2E4;
_tmp497=((struct Cyc_Absyn_Pat_b_struct*)_tmp496)->f1;if(!(!_tmp497->escapes))
goto _LL2E4;_LL2E3: _tmp499=_tmp497;goto _LL2E5;_LL2E4: if(*((int*)_tmp491)!= 1)goto
_LL2E6;_tmp498=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp491)->f2;if(_tmp498 <= (
void*)1)goto _LL2E6;if(*((int*)_tmp498)!= 2)goto _LL2E6;_tmp499=((struct Cyc_Absyn_Param_b_struct*)
_tmp498)->f1;if(!(!_tmp499->escapes))goto _LL2E6;_LL2E5: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp49A=env->fenv;struct Cyc_CfFlowInfo_Reln*_tmp811;union Cyc_CfFlowInfo_RelnOp_union
_tmp810;struct Cyc_List_List*_tmp80F;struct Cyc_List_List*_tmp49B=(_tmp80F=
_region_malloc(_tmp49A->r,sizeof(*_tmp80F)),((_tmp80F->hd=((_tmp811=
_region_malloc(_tmp49A->r,sizeof(*_tmp811)),((_tmp811->vd=_tmp48B,((_tmp811->rop=(
union Cyc_CfFlowInfo_RelnOp_union)(((_tmp810.LessEqNumelts).tag=4,(((_tmp810.LessEqNumelts).f1=
_tmp499,_tmp810)))),_tmp811)))))),((_tmp80F->tl=_tmp3ED,_tmp80F)))));union Cyc_CfFlowInfo_FlowInfo_union
_tmp814;struct _tuple8 _tmp813;return(_tmp813.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp814.ReachableFL).tag=1,(((_tmp814.ReachableFL).f1=_tmp3EC,(((_tmp814.ReachableFL).f2=
_tmp49B,(((_tmp814.ReachableFL).f3=_tmp3EE,_tmp814)))))))),((_tmp813.f2=_tmp47C,
_tmp813)));}_LL2E6:;_LL2E7: {struct _tuple8 _tmp815;return(_tmp815.f1=_tmp47B,((
_tmp815.f2=_tmp47C,_tmp815)));}_LL2DD:;}_LL2DB:;_LL2DC: {struct _tuple8 _tmp816;
return(_tmp816.f1=_tmp47B,((_tmp816.f2=_tmp47C,_tmp816)));}_LL2D8:;}_LL2D6:;
_LL2D7: {struct _tuple8 _tmp817;return(_tmp817.f1=_tmp47B,((_tmp817.f2=_tmp47C,
_tmp817)));}_LL2CD:;}}_LL29C:;_LL29D: {struct _tuple8 _tmp818;return(_tmp818.f1=f,((
_tmp818.f2=f,_tmp818)));}_LL28F:;}}_LL25C:;}}_LL25A:;_LL25B: goto _LL24D;_LL24D:;}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp4A6;void*_tmp4A7;struct _tuple5 _tmp4A5=Cyc_NewControlFlow_anal_Rexp(
env,inflow,e);_tmp4A6=_tmp4A5.f1;_tmp4A7=_tmp4A5.f2;_tmp4A6=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp4A6);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4A8=_tmp4A6;struct Cyc_Dict_Dict
_tmp4A9;_LL2E9: if((_tmp4A8.BottomFL).tag != 0)goto _LL2EB;_LL2EA: {struct _tuple8
_tmp819;return(_tmp819.f1=_tmp4A6,((_tmp819.f2=_tmp4A6,_tmp819)));}_LL2EB: if((
_tmp4A8.ReachableFL).tag != 1)goto _LL2E8;_tmp4A9=(_tmp4A8.ReachableFL).f1;_LL2EC: {
void*_tmp4AB=_tmp4A7;void*_tmp4AC;void*_tmp4AD;void*_tmp4AE;_LL2EE: if((int)
_tmp4AB != 0)goto _LL2F0;_LL2EF: {union Cyc_CfFlowInfo_FlowInfo_union _tmp81C;struct
_tuple8 _tmp81B;return(_tmp81B.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp81C.BottomFL).tag=
0,_tmp81C)),((_tmp81B.f2=_tmp4A6,_tmp81B)));}_LL2F0: if((int)_tmp4AB != 2)goto
_LL2F2;_LL2F1: goto _LL2F3;_LL2F2: if((int)_tmp4AB != 1)goto _LL2F4;_LL2F3: goto _LL2F5;
_LL2F4: if(_tmp4AB <= (void*)3)goto _LL2F6;if(*((int*)_tmp4AB)!= 2)goto _LL2F6;
_LL2F5: {union Cyc_CfFlowInfo_FlowInfo_union _tmp81F;struct _tuple8 _tmp81E;return(
_tmp81E.f1=_tmp4A6,((_tmp81E.f2=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp81F.BottomFL).tag=
0,_tmp81F)),_tmp81E)));}_LL2F6: if(_tmp4AB <= (void*)3)goto _LL2F8;if(*((int*)
_tmp4AB)!= 0)goto _LL2F8;_tmp4AC=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4AB)->f1;if((int)_tmp4AC != 0)goto _LL2F8;_LL2F7: goto _LL2F9;_LL2F8: if(_tmp4AB
<= (void*)3)goto _LL2FA;if(*((int*)_tmp4AB)!= 1)goto _LL2FA;_tmp4AD=(void*)((
struct Cyc_CfFlowInfo_Esc_struct*)_tmp4AB)->f1;if((int)_tmp4AD != 0)goto _LL2FA;
_LL2F9:{const char*_tmp822;void*_tmp821;(_tmp821=0,Cyc_Tcutil_terr(e->loc,((
_tmp822="expression may not be initialized",_tag_dyneither(_tmp822,sizeof(char),
34))),_tag_dyneither(_tmp821,sizeof(void*),0)));}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp827;union Cyc_CfFlowInfo_FlowInfo_union _tmp826;struct _tuple8 _tmp825;return(
_tmp825.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp827.BottomFL).tag=0,
_tmp827)),((_tmp825.f2=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp826.BottomFL).tag=
0,_tmp826)),_tmp825)));}_LL2FA: if(_tmp4AB <= (void*)3)goto _LL2FC;if(*((int*)
_tmp4AB)!= 0)goto _LL2FC;_tmp4AE=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4AB)->f1;_LL2FB: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4A6,e,
_tmp4AE);_LL2FC: if(_tmp4AB <= (void*)3)goto _LL2FE;if(*((int*)_tmp4AB)!= 1)goto
_LL2FE;_LL2FD: {struct _tuple8 _tmp828;return(_tmp828.f1=_tmp4A6,((_tmp828.f2=
_tmp4A6,_tmp828)));}_LL2FE: if(_tmp4AB <= (void*)3)goto _LL300;if(*((int*)_tmp4AB)
!= 3)goto _LL300;_LL2FF: {struct _tuple8 _tmp829;return(_tmp829.f1=_tmp4A6,((
_tmp829.f2=_tmp4A6,_tmp829)));}_LL300: if(_tmp4AB <= (void*)3)goto _LL2ED;if(*((int*)
_tmp4AB)!= 4)goto _LL2ED;_LL301: {struct Cyc_Core_Impossible_struct _tmp82F;const
char*_tmp82E;struct Cyc_Core_Impossible_struct*_tmp82D;(int)_throw((void*)((
_tmp82D=_cycalloc(sizeof(*_tmp82D)),((_tmp82D[0]=((_tmp82F.tag=Cyc_Core_Impossible,((
_tmp82F.f1=((_tmp82E="anal_test",_tag_dyneither(_tmp82E,sizeof(char),10))),
_tmp82F)))),_tmp82D)))));}_LL2ED:;}_LL2E8:;}}}static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
flow);static void Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*
loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union flow){
union Cyc_CfFlowInfo_FlowInfo_union _tmp4BD=flow;struct Cyc_Dict_Dict _tmp4BE;_LL303:
if((_tmp4BD.BottomFL).tag != 0)goto _LL305;_LL304: return;_LL305: if((_tmp4BD.ReachableFL).tag
!= 1)goto _LL302;_tmp4BE=(_tmp4BD.ReachableFL).f1;_LL306:{struct Cyc_List_List*
_tmp4BF=env->param_roots;for(0;_tmp4BF != 0;_tmp4BF=_tmp4BF->tl){if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp4BE,Cyc_CfFlowInfo_lookup_place(_tmp4BE,(struct Cyc_CfFlowInfo_Place*)
_tmp4BF->hd))!= (void*)2){const char*_tmp832;void*_tmp831;(_tmp831=0,Cyc_Tcutil_terr(
loc,((_tmp832="function may not initialize all the parameters with attribute 'initializes'",
_tag_dyneither(_tmp832,sizeof(char),76))),_tag_dyneither(_tmp831,sizeof(void*),0)));}}}
return;_LL302:;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_List_List*scs);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp4C2=env->fenv;for(0;
scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp4C4;struct Cyc_Core_Opt*
_tmp4C5;struct Cyc_Absyn_Exp*_tmp4C6;struct Cyc_Absyn_Stmt*_tmp4C7;struct Cyc_Position_Segment*
_tmp4C8;struct Cyc_Absyn_Switch_clause*_tmp4C3=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp4C4=*_tmp4C3;_tmp4C5=_tmp4C4.pat_vars;_tmp4C6=_tmp4C4.where_clause;
_tmp4C7=_tmp4C4.body;_tmp4C8=_tmp4C4.loc;{union Cyc_CfFlowInfo_FlowInfo_union
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp4C2,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp4C5))->v,(void*)_tmp4C2->unknown_all,_tmp4C8);
union Cyc_CfFlowInfo_FlowInfo_union clause_outflow;if(_tmp4C6 != 0){struct Cyc_Absyn_Exp*
wexp=(struct Cyc_Absyn_Exp*)_tmp4C6;union Cyc_CfFlowInfo_FlowInfo_union _tmp4CA;
union Cyc_CfFlowInfo_FlowInfo_union _tmp4CB;struct _tuple8 _tmp4C9=Cyc_NewControlFlow_anal_test(
env,clause_inflow,wexp);_tmp4CA=_tmp4C9.f1;_tmp4CB=_tmp4C9.f2;_tmp4CA=Cyc_CfFlowInfo_readthrough_unique_rvals(
wexp->loc,_tmp4CA);_tmp4CB=Cyc_CfFlowInfo_readthrough_unique_rvals(wexp->loc,
_tmp4CB);inflow=_tmp4CB;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp4CA,
_tmp4C7);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,
_tmp4C7);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp4CC=clause_outflow;_LL308: if((
_tmp4CC.BottomFL).tag != 0)goto _LL30A;_LL309: goto _LL307;_LL30A:;_LL30B: if(scs->tl
== 0)return clause_outflow;else{if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(
scs->tl))->hd)->pat_vars))->v != 0){const char*_tmp835;void*_tmp834;(_tmp834=0,Cyc_Tcutil_terr(
_tmp4C7->loc,((_tmp835="switch clause may implicitly fallthru",_tag_dyneither(
_tmp835,sizeof(char),38))),_tag_dyneither(_tmp834,sizeof(void*),0)));}else{const
char*_tmp838;void*_tmp837;(_tmp837=0,Cyc_Tcutil_warn(_tmp4C7->loc,((_tmp838="switch clause may implicitly fallthru",
_tag_dyneither(_tmp838,sizeof(char),38))),_tag_dyneither(_tmp837,sizeof(void*),0)));}
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)
_check_null(scs->tl))->hd)->body,clause_outflow);}goto _LL307;_LL307:;}}}{union
Cyc_CfFlowInfo_FlowInfo_union _tmp839;return(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp839.BottomFL).tag=0,_tmp839));}}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Stmt*s);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union outflow;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp4D3;union Cyc_CfFlowInfo_FlowInfo_union*_tmp4D4;struct _tuple9 _tmp4D2=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp4D3=_tmp4D2.f1;_tmp4D4=_tmp4D2.f2;inflow=*_tmp4D4;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp4D5=env->fenv;{void*_tmp4D6=(void*)s->r;struct Cyc_Absyn_Exp*_tmp4D7;struct
Cyc_Absyn_Exp*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Absyn_Stmt*_tmp4DA;
struct Cyc_Absyn_Stmt*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DC;struct Cyc_Absyn_Stmt*
_tmp4DD;struct Cyc_Absyn_Stmt*_tmp4DE;struct _tuple3 _tmp4DF;struct Cyc_Absyn_Exp*
_tmp4E0;struct Cyc_Absyn_Stmt*_tmp4E1;struct Cyc_Absyn_Stmt*_tmp4E2;struct Cyc_Absyn_Stmt*
_tmp4E3;struct _tuple3 _tmp4E4;struct Cyc_Absyn_Exp*_tmp4E5;struct Cyc_Absyn_Stmt*
_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E7;struct _tuple3 _tmp4E8;struct Cyc_Absyn_Exp*
_tmp4E9;struct Cyc_Absyn_Stmt*_tmp4EA;struct _tuple3 _tmp4EB;struct Cyc_Absyn_Exp*
_tmp4EC;struct Cyc_Absyn_Stmt*_tmp4ED;struct Cyc_Absyn_Stmt*_tmp4EE;struct Cyc_Absyn_Stmt*
_tmp4EF;struct Cyc_List_List*_tmp4F0;struct Cyc_Absyn_Switch_clause**_tmp4F1;
struct Cyc_Absyn_Switch_clause*_tmp4F2;struct Cyc_Absyn_Stmt*_tmp4F3;struct Cyc_Absyn_Stmt*
_tmp4F4;struct Cyc_Absyn_Stmt*_tmp4F5;struct Cyc_Absyn_Exp*_tmp4F6;struct Cyc_List_List*
_tmp4F7;struct Cyc_Absyn_Stmt*_tmp4F8;struct Cyc_List_List*_tmp4F9;struct Cyc_Absyn_Decl*
_tmp4FA;struct Cyc_Absyn_Decl _tmp4FB;void*_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FD;
struct Cyc_Absyn_Tvar*_tmp4FE;struct Cyc_Absyn_Vardecl*_tmp4FF;struct Cyc_Absyn_Stmt*
_tmp500;struct Cyc_Absyn_Decl*_tmp501;struct Cyc_Absyn_Stmt*_tmp502;struct Cyc_Absyn_Stmt*
_tmp503;struct Cyc_Absyn_Exp*_tmp504;_LL30D: if((int)_tmp4D6 != 0)goto _LL30F;_LL30E:
return inflow;_LL30F: if(_tmp4D6 <= (void*)1)goto _LL335;if(*((int*)_tmp4D6)!= 2)
goto _LL311;_tmp4D7=((struct Cyc_Absyn_Return_s_struct*)_tmp4D6)->f1;if(_tmp4D7 != 
0)goto _LL311;_LL310: if(env->noreturn){const char*_tmp83C;void*_tmp83B;(_tmp83B=0,
Cyc_Tcutil_terr(s->loc,((_tmp83C="`noreturn' function might return",
_tag_dyneither(_tmp83C,sizeof(char),33))),_tag_dyneither(_tmp83B,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp83D;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp83D.BottomFL).tag=0,
_tmp83D));}_LL311: if(*((int*)_tmp4D6)!= 2)goto _LL313;_tmp4D8=((struct Cyc_Absyn_Return_s_struct*)
_tmp4D6)->f1;_LL312: if(env->noreturn){const char*_tmp840;void*_tmp83F;(_tmp83F=0,
Cyc_Tcutil_terr(s->loc,((_tmp840="`noreturn' function might return",
_tag_dyneither(_tmp840,sizeof(char),33))),_tag_dyneither(_tmp83F,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp50B;void*_tmp50C;struct _tuple5 _tmp50A=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp4D8));_tmp50B=_tmp50A.f1;
_tmp50C=_tmp50A.f2;_tmp50B=Cyc_CfFlowInfo_consume_unique_rvals(_tmp4D8->loc,
_tmp50B);_tmp50B=Cyc_NewControlFlow_use_Rval(env,_tmp4D8->loc,_tmp50B,_tmp50C);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp50B);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp841;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp841.BottomFL).tag=0,
_tmp841));}}_LL313: if(*((int*)_tmp4D6)!= 0)goto _LL315;_tmp4D9=((struct Cyc_Absyn_Exp_s_struct*)
_tmp4D6)->f1;_LL314: outflow=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp4D9)).f1;
goto _LL30C;_LL315: if(*((int*)_tmp4D6)!= 1)goto _LL317;_tmp4DA=((struct Cyc_Absyn_Seq_s_struct*)
_tmp4D6)->f1;_tmp4DB=((struct Cyc_Absyn_Seq_s_struct*)_tmp4D6)->f2;_LL316: outflow=
Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4DA),
_tmp4DB);goto _LL30C;_LL317: if(*((int*)_tmp4D6)!= 3)goto _LL319;_tmp4DC=((struct
Cyc_Absyn_IfThenElse_s_struct*)_tmp4D6)->f1;_tmp4DD=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp4D6)->f2;_tmp4DE=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4D6)->f3;_LL318: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp50F;union Cyc_CfFlowInfo_FlowInfo_union
_tmp510;struct _tuple8 _tmp50E=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4DC);
_tmp50F=_tmp50E.f1;_tmp510=_tmp50E.f2;_tmp50F=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4DC->loc,_tmp50F);_tmp510=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4DC->loc,
_tmp510);outflow=Cyc_CfFlowInfo_join_flow(_tmp4D5,env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp50F,_tmp4DD),Cyc_NewControlFlow_anal_stmt(env,_tmp510,_tmp4DE),1);goto
_LL30C;}_LL319: if(*((int*)_tmp4D6)!= 4)goto _LL31B;_tmp4DF=((struct Cyc_Absyn_While_s_struct*)
_tmp4D6)->f1;_tmp4E0=_tmp4DF.f1;_tmp4E1=_tmp4DF.f2;_tmp4E2=((struct Cyc_Absyn_While_s_struct*)
_tmp4D6)->f2;_LL31A: {union Cyc_CfFlowInfo_FlowInfo_union*_tmp512;struct _tuple9
_tmp511=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp4E1);_tmp512=_tmp511.f2;{
union Cyc_CfFlowInfo_FlowInfo_union _tmp513=*_tmp512;union Cyc_CfFlowInfo_FlowInfo_union
_tmp515;union Cyc_CfFlowInfo_FlowInfo_union _tmp516;struct _tuple8 _tmp514=Cyc_NewControlFlow_anal_test(
env,_tmp513,_tmp4E0);_tmp515=_tmp514.f1;_tmp516=_tmp514.f2;_tmp515=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4E0->loc,_tmp515);_tmp516=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4E0->loc,
_tmp516);{union Cyc_CfFlowInfo_FlowInfo_union _tmp517=Cyc_NewControlFlow_anal_stmt(
env,_tmp515,_tmp4E2);Cyc_NewControlFlow_update_flow(env,_tmp4E1,_tmp517);outflow=
_tmp516;goto _LL30C;}}}_LL31B: if(*((int*)_tmp4D6)!= 13)goto _LL31D;_tmp4E3=((
struct Cyc_Absyn_Do_s_struct*)_tmp4D6)->f1;_tmp4E4=((struct Cyc_Absyn_Do_s_struct*)
_tmp4D6)->f2;_tmp4E5=_tmp4E4.f1;_tmp4E6=_tmp4E4.f2;_LL31C: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp518=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4E3);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp51A;struct _tuple9 _tmp519=Cyc_NewControlFlow_pre_stmt_check(env,_tmp518,
_tmp4E6);_tmp51A=_tmp519.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp51B=*_tmp51A;
union Cyc_CfFlowInfo_FlowInfo_union _tmp51D;union Cyc_CfFlowInfo_FlowInfo_union
_tmp51E;struct _tuple8 _tmp51C=Cyc_NewControlFlow_anal_test(env,_tmp51B,_tmp4E5);
_tmp51D=_tmp51C.f1;_tmp51E=_tmp51C.f2;Cyc_NewControlFlow_update_flow(env,_tmp4E3,
_tmp51D);outflow=_tmp51E;goto _LL30C;}}_LL31D: if(*((int*)_tmp4D6)!= 8)goto _LL31F;
_tmp4E7=((struct Cyc_Absyn_For_s_struct*)_tmp4D6)->f1;_tmp4E8=((struct Cyc_Absyn_For_s_struct*)
_tmp4D6)->f2;_tmp4E9=_tmp4E8.f1;_tmp4EA=_tmp4E8.f2;_tmp4EB=((struct Cyc_Absyn_For_s_struct*)
_tmp4D6)->f3;_tmp4EC=_tmp4EB.f1;_tmp4ED=_tmp4EB.f2;_tmp4EE=((struct Cyc_Absyn_For_s_struct*)
_tmp4D6)->f4;_LL31E: {union Cyc_CfFlowInfo_FlowInfo_union _tmp51F=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4E7)).f1;_tmp51F=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4E7->loc,
_tmp51F);{union Cyc_CfFlowInfo_FlowInfo_union*_tmp521;struct _tuple9 _tmp520=Cyc_NewControlFlow_pre_stmt_check(
env,_tmp51F,_tmp4EA);_tmp521=_tmp520.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp522=*_tmp521;union Cyc_CfFlowInfo_FlowInfo_union _tmp524;union Cyc_CfFlowInfo_FlowInfo_union
_tmp525;struct _tuple8 _tmp523=Cyc_NewControlFlow_anal_test(env,_tmp522,_tmp4E9);
_tmp524=_tmp523.f1;_tmp525=_tmp523.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp526=
Cyc_NewControlFlow_anal_stmt(env,_tmp524,_tmp4EE);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp528;struct _tuple9 _tmp527=Cyc_NewControlFlow_pre_stmt_check(env,_tmp526,
_tmp4ED);_tmp528=_tmp527.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp529=*_tmp528;
union Cyc_CfFlowInfo_FlowInfo_union _tmp52A=(Cyc_NewControlFlow_anal_Rexp(env,
_tmp529,_tmp4EC)).f1;_tmp52A=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4EC->loc,
_tmp52A);Cyc_NewControlFlow_update_flow(env,_tmp4EA,_tmp52A);outflow=_tmp525;
goto _LL30C;}}}}}_LL31F: if(*((int*)_tmp4D6)!= 5)goto _LL321;_tmp4EF=((struct Cyc_Absyn_Break_s_struct*)
_tmp4D6)->f1;if(_tmp4EF != 0)goto _LL321;_LL320: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp842;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp842.BottomFL).tag=0,
_tmp842));}_LL321: if(*((int*)_tmp4D6)!= 10)goto _LL323;_tmp4F0=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp4D6)->f1;_tmp4F1=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp4D6)->f2;if(
_tmp4F1 == 0)goto _LL323;_tmp4F2=*_tmp4F1;_LL322: {struct _RegionHandle*_tmp52C=env->r;
union Cyc_CfFlowInfo_FlowInfo_union _tmp52E;struct Cyc_List_List*_tmp52F;struct
_tuple11 _tmp52D=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp52C,env,inflow,
_tmp4F0,0);_tmp52E=_tmp52D.f1;_tmp52F=_tmp52D.f2;for(0;_tmp52F != 0;(_tmp52F=
_tmp52F->tl,_tmp4F0=_tmp4F0->tl)){_tmp52E=Cyc_NewControlFlow_use_Rval(env,((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4F0))->hd)->loc,
_tmp52E,(void*)_tmp52F->hd);}_tmp52E=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,
_tmp52E);_tmp52E=Cyc_NewControlFlow_add_vars(_tmp4D5,_tmp52E,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp4F2->pat_vars))->v,(void*)_tmp4D5->unknown_all,
s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp4F2->body,
_tmp52E);{union Cyc_CfFlowInfo_FlowInfo_union _tmp843;return(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp843.BottomFL).tag=0,_tmp843));}}_LL323: if(*((int*)_tmp4D6)!= 5)goto _LL325;
_tmp4F3=((struct Cyc_Absyn_Break_s_struct*)_tmp4D6)->f1;_LL324: _tmp4F4=_tmp4F3;
goto _LL326;_LL325: if(*((int*)_tmp4D6)!= 6)goto _LL327;_tmp4F4=((struct Cyc_Absyn_Continue_s_struct*)
_tmp4D6)->f1;_LL326: _tmp4F5=_tmp4F4;goto _LL328;_LL327: if(*((int*)_tmp4D6)!= 7)
goto _LL329;_tmp4F5=((struct Cyc_Absyn_Goto_s_struct*)_tmp4D6)->f2;_LL328: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp531=_tmp4D3->encloser;struct Cyc_Absyn_Stmt*
_tmp532=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp4F5)))->encloser;while(_tmp532 != _tmp531){struct Cyc_Absyn_Stmt*_tmp533=(Cyc_NewControlFlow_get_stmt_annot(
_tmp531))->encloser;if(_tmp533 == _tmp531){{const char*_tmp846;void*_tmp845;(
_tmp845=0,Cyc_Tcutil_terr(s->loc,((_tmp846="goto enters local scope or exception handler",
_tag_dyneither(_tmp846,sizeof(char),45))),_tag_dyneither(_tmp845,sizeof(void*),0)));}
break;}_tmp531=_tmp533;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_check_null(_tmp4F5),inflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp847;return(
union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp847.BottomFL).tag=0,_tmp847));}_LL329:
if(*((int*)_tmp4D6)!= 9)goto _LL32B;_tmp4F6=((struct Cyc_Absyn_Switch_s_struct*)
_tmp4D6)->f1;_tmp4F7=((struct Cyc_Absyn_Switch_s_struct*)_tmp4D6)->f2;_LL32A: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp538;void*_tmp539;struct _tuple5 _tmp537=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4F6);_tmp538=_tmp537.f1;_tmp539=_tmp537.f2;_tmp538=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4F6->loc,_tmp538);_tmp538=Cyc_NewControlFlow_use_Rval(env,_tmp4F6->loc,
_tmp538,_tmp539);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp538,_tmp4F7);goto
_LL30C;}_LL32B: if(*((int*)_tmp4D6)!= 14)goto _LL32D;_tmp4F8=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp4D6)->f1;_tmp4F9=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4D6)->f2;_LL32C: {
int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo_union old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo_union s1_outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4F8);union Cyc_CfFlowInfo_FlowInfo_union
scs_inflow=env->tryflow;env->in_try=old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(
env,scs_inflow);{union Cyc_CfFlowInfo_FlowInfo_union scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp4F9);{union Cyc_CfFlowInfo_FlowInfo_union _tmp53A=scs_outflow;
_LL338: if((_tmp53A.BottomFL).tag != 0)goto _LL33A;_LL339: goto _LL337;_LL33A:;_LL33B: {
const char*_tmp84A;void*_tmp849;(_tmp849=0,Cyc_Tcutil_terr(s->loc,((_tmp84A="last catch clause may implicitly fallthru",
_tag_dyneither(_tmp84A,sizeof(char),42))),_tag_dyneither(_tmp849,sizeof(void*),0)));}
_LL337:;}outflow=s1_outflow;goto _LL30C;}}}_LL32D: if(*((int*)_tmp4D6)!= 11)goto
_LL32F;_tmp4FA=((struct Cyc_Absyn_Decl_s_struct*)_tmp4D6)->f1;_tmp4FB=*_tmp4FA;
_tmp4FC=(void*)_tmp4FB.r;if(_tmp4FC <= (void*)2)goto _LL32F;if(*((int*)_tmp4FC)!= 
5)goto _LL32F;_tmp4FD=((struct Cyc_Absyn_Alias_d_struct*)_tmp4FC)->f1;_tmp4FE=((
struct Cyc_Absyn_Alias_d_struct*)_tmp4FC)->f2;_tmp4FF=((struct Cyc_Absyn_Alias_d_struct*)
_tmp4FC)->f3;_tmp500=((struct Cyc_Absyn_Decl_s_struct*)_tmp4D6)->f2;_LL32E: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp53E;void*_tmp53F;struct _tuple5 _tmp53D=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4FD);_tmp53E=_tmp53D.f1;_tmp53F=_tmp53D.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
_tmp541;struct Cyc_List_List*_tmp542;struct _tuple6 _tmp540=Cyc_CfFlowInfo_save_consume_info(
_tmp4D5,_tmp53E,0);_tmp541=_tmp540.f1;_tmp542=_tmp541.may_consume;_tmp53E=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4FD->loc,_tmp53E);_tmp53E=Cyc_NewControlFlow_use_Rval(env,_tmp4FD->loc,
_tmp53E,_tmp53F);{struct Cyc_List_List _tmp84B;struct Cyc_List_List _tmp543=(_tmp84B.hd=
_tmp4FF,((_tmp84B.tl=0,_tmp84B)));_tmp53E=Cyc_NewControlFlow_add_vars(_tmp4D5,
_tmp53E,(struct Cyc_List_List*)& _tmp543,(void*)_tmp4D5->unknown_all,s->loc);
outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp53E,_tmp500);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp544=outflow;struct Cyc_Dict_Dict _tmp545;struct Cyc_List_List*_tmp546;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp547;_LL33D: if((_tmp544.BottomFL).tag != 0)goto _LL33F;_LL33E: goto _LL33C;_LL33F:
if((_tmp544.ReachableFL).tag != 1)goto _LL33C;_tmp545=(_tmp544.ReachableFL).f1;
_tmp546=(_tmp544.ReachableFL).f2;_tmp547=(_tmp544.ReachableFL).f3;_LL340: while(
_tmp542 != 0){struct Cyc_Dict_Dict _tmp548=_tmp547.consumed;_tmp547.consumed=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))
Cyc_Dict_rdelete)(_tmp4D5->r,_tmp547.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp542->hd);if((_tmp547.consumed).t != _tmp548.t);_tmp542=_tmp542->tl;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp84C;outflow=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp84C.ReachableFL).tag=
1,(((_tmp84C.ReachableFL).f1=_tmp545,(((_tmp84C.ReachableFL).f2=_tmp546,(((
_tmp84C.ReachableFL).f3=_tmp547,_tmp84C))))))));}goto _LL33C;_LL33C:;}goto _LL30C;}}}
_LL32F: if(*((int*)_tmp4D6)!= 11)goto _LL331;_tmp501=((struct Cyc_Absyn_Decl_s_struct*)
_tmp4D6)->f1;_tmp502=((struct Cyc_Absyn_Decl_s_struct*)_tmp4D6)->f2;_LL330:
outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp501),_tmp502);goto _LL30C;_LL331: if(*((int*)_tmp4D6)!= 12)goto _LL333;_tmp503=((
struct Cyc_Absyn_Label_s_struct*)_tmp4D6)->f2;_LL332: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp503);goto _LL30C;_LL333: if(*((int*)_tmp4D6)!= 15)goto _LL335;
_tmp504=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp4D6)->f1;_LL334: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp54C;void*_tmp54D;struct _tuple5 _tmp54B=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp504);_tmp54C=_tmp54B.f1;_tmp54D=_tmp54B.f2;_tmp54C=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp504->loc,_tmp54C);{union Cyc_CfFlowInfo_FlowInfo_union _tmp54E=Cyc_NewControlFlow_use_Rval(
env,_tmp504->loc,_tmp54C,_tmp54D);{void*_tmp54F=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp504->topt))->v);void*_tmp550;_LL342: if(
_tmp54F <= (void*)4)goto _LL344;if(*((int*)_tmp54F)!= 14)goto _LL344;_tmp550=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp54F)->f1;_LL343: outflow=Cyc_CfFlowInfo_kill_flow_region(
_tmp4D5,_tmp54C,_tmp550);goto _LL341;_LL344:;_LL345: {struct Cyc_Core_Impossible_struct
_tmp852;const char*_tmp851;struct Cyc_Core_Impossible_struct*_tmp850;(int)_throw((
void*)((_tmp850=_cycalloc(sizeof(*_tmp850)),((_tmp850[0]=((_tmp852.tag=Cyc_Core_Impossible,((
_tmp852.f1=((_tmp851="anal_stmt -- reset_region",_tag_dyneither(_tmp851,sizeof(
char),26))),_tmp852)))),_tmp850)))));}_LL341:;}goto _LL30C;}}_LL335:;_LL336: {
struct Cyc_Core_Impossible_struct _tmp858;const char*_tmp857;struct Cyc_Core_Impossible_struct*
_tmp856;(int)_throw((void*)((_tmp856=_cycalloc(sizeof(*_tmp856)),((_tmp856[0]=((
_tmp858.tag=Cyc_Core_Impossible,((_tmp858.f1=((_tmp857="anal_stmt -- bad stmt syntax or unimplemented stmt form",
_tag_dyneither(_tmp857,sizeof(char),56))),_tmp858)))),_tmp856)))));}_LL30C:;}
outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp557=outflow;struct Cyc_CfFlowInfo_ConsumeInfo _tmp558;_LL347: if((_tmp557.ReachableFL).tag
!= 1)goto _LL349;_tmp558=(_tmp557.ReachableFL).f3;_LL348: goto _LL346;_LL349:;
_LL34A: goto _LL346;_LL346:;}return outflow;}}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*
fd);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct
Cyc_Absyn_Decl*decl);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Decl*decl){void*_tmp559=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp55A;struct Cyc_Core_Opt*_tmp55B;struct Cyc_Core_Opt _tmp55C;struct Cyc_List_List*
_tmp55D;struct Cyc_Absyn_Exp*_tmp55E;struct Cyc_List_List*_tmp55F;struct Cyc_Absyn_Fndecl*
_tmp560;struct Cyc_Absyn_Tvar*_tmp561;struct Cyc_Absyn_Vardecl*_tmp562;int _tmp563;
struct Cyc_Absyn_Exp*_tmp564;_LL34C: if(_tmp559 <= (void*)2)goto _LL356;if(*((int*)
_tmp559)!= 0)goto _LL34E;_tmp55A=((struct Cyc_Absyn_Var_d_struct*)_tmp559)->f1;
_LL34D: {struct Cyc_List_List _tmp859;struct Cyc_List_List _tmp565=(_tmp859.hd=
_tmp55A,((_tmp859.tl=0,_tmp859)));inflow=Cyc_NewControlFlow_add_vars(env->fenv,
inflow,(struct Cyc_List_List*)& _tmp565,(void*)(env->fenv)->unknown_none,decl->loc);{
struct Cyc_Absyn_Exp*_tmp566=_tmp55A->initializer;if(_tmp566 == 0)return inflow;{
union Cyc_CfFlowInfo_FlowInfo_union _tmp568;void*_tmp569;struct _tuple5 _tmp567=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)_tmp566);_tmp568=_tmp567.f1;_tmp569=_tmp567.f2;
_tmp568=Cyc_CfFlowInfo_consume_unique_rvals(_tmp566->loc,_tmp568);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp56A=_tmp568;struct Cyc_Dict_Dict _tmp56B;struct Cyc_List_List*_tmp56C;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp56D;_LL359: if((_tmp56A.BottomFL).tag != 0)goto _LL35B;_LL35A: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp85A;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp85A.BottomFL).tag=0,
_tmp85A));}_LL35B: if((_tmp56A.ReachableFL).tag != 1)goto _LL358;_tmp56B=(_tmp56A.ReachableFL).f1;
_tmp56C=(_tmp56A.ReachableFL).f2;_tmp56D=(_tmp56A.ReachableFL).f3;_LL35C:{struct
Cyc_CfFlowInfo_VarRoot_struct*_tmp860;struct Cyc_CfFlowInfo_VarRoot_struct _tmp85F;
struct Cyc_CfFlowInfo_Place*_tmp85E;_tmp56B=Cyc_CfFlowInfo_assign_place(env->fenv,
decl->loc,_tmp56B,env->all_changed,((_tmp85E=_region_malloc(env->r,sizeof(*
_tmp85E)),((_tmp85E->root=(void*)((void*)((_tmp860=_region_malloc(env->r,sizeof(*
_tmp860)),((_tmp860[0]=((_tmp85F.tag=0,((_tmp85F.f1=_tmp55A,_tmp85F)))),_tmp860))))),((
_tmp85E->fields=0,_tmp85E)))))),_tmp569);}_tmp56C=Cyc_CfFlowInfo_reln_assign_var(
env->r,_tmp56C,_tmp55A,(struct Cyc_Absyn_Exp*)_check_null(_tmp55A->initializer));{
union Cyc_CfFlowInfo_FlowInfo_union _tmp861;union Cyc_CfFlowInfo_FlowInfo_union
_tmp572=((_tmp861.ReachableFL).tag=1,(((_tmp861.ReachableFL).f1=_tmp56B,(((
_tmp861.ReachableFL).f2=_tmp56C,(((_tmp861.ReachableFL).f3=_tmp56D,_tmp861)))))));
Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp572);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp572;}_LL358:;}}}}_LL34E:
if(*((int*)_tmp559)!= 2)goto _LL350;_tmp55B=((struct Cyc_Absyn_Let_d_struct*)
_tmp559)->f2;if(_tmp55B == 0)goto _LL350;_tmp55C=*_tmp55B;_tmp55D=(struct Cyc_List_List*)
_tmp55C.v;_tmp55E=((struct Cyc_Absyn_Let_d_struct*)_tmp559)->f3;_LL34F: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp576;void*_tmp577;struct _tuple5 _tmp575=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp55E);_tmp576=_tmp575.f1;_tmp577=_tmp575.f2;_tmp576=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp55E->loc,_tmp576);_tmp576=Cyc_NewControlFlow_use_Rval(env,_tmp55E->loc,
_tmp576,_tmp577);return Cyc_NewControlFlow_add_vars(env->fenv,_tmp576,_tmp55D,(
void*)(env->fenv)->unknown_all,decl->loc);}_LL350: if(*((int*)_tmp559)!= 3)goto
_LL352;_tmp55F=((struct Cyc_Absyn_Letv_d_struct*)_tmp559)->f1;_LL351: return Cyc_NewControlFlow_add_vars(
env->fenv,inflow,_tmp55F,(void*)(env->fenv)->unknown_none,decl->loc);_LL352: if(*((
int*)_tmp559)!= 1)goto _LL354;_tmp560=((struct Cyc_Absyn_Fn_d_struct*)_tmp559)->f1;
_LL353: Cyc_NewControlFlow_check_nested_fun(env->fenv,inflow,_tmp560);{void*
_tmp578=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp560->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp579=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp560->fn_vardecl);
struct Cyc_List_List*_tmp862;return Cyc_NewControlFlow_add_vars(env->fenv,inflow,((
_tmp862=_region_malloc(env->r,sizeof(*_tmp862)),((_tmp862->hd=_tmp579,((_tmp862->tl=
0,_tmp862)))))),(void*)(env->fenv)->unknown_all,decl->loc);}_LL354: if(*((int*)
_tmp559)!= 4)goto _LL356;_tmp561=((struct Cyc_Absyn_Region_d_struct*)_tmp559)->f1;
_tmp562=((struct Cyc_Absyn_Region_d_struct*)_tmp559)->f2;_tmp563=((struct Cyc_Absyn_Region_d_struct*)
_tmp559)->f3;_tmp564=((struct Cyc_Absyn_Region_d_struct*)_tmp559)->f4;_LL355: if(
_tmp564 != 0){struct Cyc_Absyn_Exp*_tmp57B=(struct Cyc_Absyn_Exp*)_tmp564;union Cyc_CfFlowInfo_FlowInfo_union
_tmp57D;void*_tmp57E;struct _tuple5 _tmp57C=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp57B);_tmp57D=_tmp57C.f1;_tmp57E=_tmp57C.f2;_tmp57D=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp57B->loc,_tmp57D);inflow=Cyc_NewControlFlow_use_Rval(env,_tmp57B->loc,
_tmp57D,_tmp57E);}{struct Cyc_List_List _tmp863;struct Cyc_List_List _tmp57F=(
_tmp863.hd=_tmp562,((_tmp863.tl=0,_tmp863)));return Cyc_NewControlFlow_add_vars(
env->fenv,inflow,(struct Cyc_List_List*)& _tmp57F,(void*)(env->fenv)->unknown_all,
decl->loc);}_LL356:;_LL357: {struct Cyc_Core_Impossible_struct _tmp869;const char*
_tmp868;struct Cyc_Core_Impossible_struct*_tmp867;(int)_throw((void*)((_tmp867=
_cycalloc(sizeof(*_tmp867)),((_tmp867[0]=((_tmp869.tag=Cyc_Core_Impossible,((
_tmp869.f1=((_tmp868="anal_decl: unexpected decl variant",_tag_dyneither(_tmp868,
sizeof(char),35))),_tmp869)))),_tmp867)))));}_LL34B:;}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd);static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle _tmp584=_new_region("frgn");struct _RegionHandle*frgn=&
_tmp584;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);struct Cyc_CfFlowInfo_ConsumeInfo _tmp86C;union Cyc_CfFlowInfo_FlowInfo_union
_tmp86B;Cyc_NewControlFlow_check_nested_fun(fenv,(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp86B.ReachableFL).tag=1,(((_tmp86B.ReachableFL).f1=fenv->mt_flowdict,(((
_tmp86B.ReachableFL).f2=0,(((_tmp86B.ReachableFL).f3=((_tmp86C.consumed=fenv->mt_place_set,((
_tmp86C.may_consume=0,_tmp86C)))),_tmp86B)))))))),fd);};_pop_region(frgn);}
static int Cyc_NewControlFlow_hash_ptr(void*s);static int Cyc_NewControlFlow_hash_ptr(
void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*fd);static
void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union
Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp587=fenv->r;struct Cyc_Position_Segment*_tmp588=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(void*)fenv->unknown_all,_tmp588);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo_union _tmp589=inflow;struct Cyc_Dict_Dict
_tmp58A;struct Cyc_List_List*_tmp58B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp58C;
_LL35E: if((_tmp589.BottomFL).tag != 0)goto _LL360;_LL35F: {const char*_tmp86F;void*
_tmp86E;(_tmp86E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp86F="check_fun",_tag_dyneither(_tmp86F,sizeof(char),10))),_tag_dyneither(
_tmp86E,sizeof(void*),0)));}_LL360: if((_tmp589.ReachableFL).tag != 1)goto _LL35D;
_tmp58A=(_tmp589.ReachableFL).f1;_tmp58B=(_tmp589.ReachableFL).f2;_tmp58C=(
_tmp589.ReachableFL).f3;_LL361: {struct Cyc_List_List*atts;{void*_tmp58F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo
_tmp590;struct Cyc_List_List*_tmp591;_LL363: if(_tmp58F <= (void*)4)goto _LL365;if(*((
int*)_tmp58F)!= 8)goto _LL365;_tmp590=((struct Cyc_Absyn_FnType_struct*)_tmp58F)->f1;
_tmp591=_tmp590.attributes;_LL364: atts=_tmp591;goto _LL362;_LL365:;_LL366: {const
char*_tmp872;void*_tmp871;(_tmp871=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp872="check_fun: non-function type cached with fndecl_t",
_tag_dyneither(_tmp872,sizeof(char),50))),_tag_dyneither(_tmp871,sizeof(void*),0)));}
_LL362:;}for(0;atts != 0;atts=atts->tl){void*_tmp594=(void*)atts->hd;int _tmp595;
_LL368: if(_tmp594 <= (void*)17)goto _LL36A;if(*((int*)_tmp594)!= 4)goto _LL36A;
_tmp595=((struct Cyc_Absyn_Initializes_att_struct*)_tmp594)->f1;_LL369: {
unsigned int j=(unsigned int)_tmp595;if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){{
const char*_tmp875;void*_tmp874;(_tmp874=0,Cyc_Tcutil_terr(_tmp588,((_tmp875="initializes attribute exceeds number of parameters",
_tag_dyneither(_tmp875,sizeof(char),51))),_tag_dyneither(_tmp874,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp598=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp599=Cyc_Tcutil_compress((
void*)_tmp598->type);struct Cyc_Absyn_PtrInfo _tmp59A;void*_tmp59B;struct Cyc_Absyn_PtrAtts
_tmp59C;struct Cyc_Absyn_Conref*_tmp59D;struct Cyc_Absyn_Conref*_tmp59E;struct Cyc_Absyn_Conref*
_tmp59F;_LL36D: if(_tmp599 <= (void*)4)goto _LL36F;if(*((int*)_tmp599)!= 4)goto
_LL36F;_tmp59A=((struct Cyc_Absyn_PointerType_struct*)_tmp599)->f1;_tmp59B=(void*)
_tmp59A.elt_typ;_tmp59C=_tmp59A.ptr_atts;_tmp59D=_tmp59C.nullable;_tmp59E=
_tmp59C.bounds;_tmp59F=_tmp59C.zero_term;_LL36E: if(((int(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_val)(_tmp59D)){const char*_tmp878;void*_tmp877;(_tmp877=0,Cyc_Tcutil_terr(
_tmp588,((_tmp878="initializes attribute not allowed on nullable pointers",
_tag_dyneither(_tmp878,sizeof(char),55))),_tag_dyneither(_tmp877,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_bound_one(_tmp59E)){const char*_tmp87B;void*_tmp87A;(_tmp87A=0,
Cyc_Tcutil_terr(_tmp588,((_tmp87B="initializes attribute allowed only on pointers of size 1",
_tag_dyneither(_tmp87B,sizeof(char),57))),_tag_dyneither(_tmp87A,sizeof(void*),0)));}
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp59F)){
const char*_tmp87E;void*_tmp87D;(_tmp87D=0,Cyc_Tcutil_terr(_tmp588,((_tmp87E="initializes attribute allowed only on pointers to non-zero-terminated arrays",
_tag_dyneither(_tmp87E,sizeof(char),77))),_tag_dyneither(_tmp87D,sizeof(void*),0)));}{
struct Cyc_CfFlowInfo_InitParam_struct _tmp881;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp880;struct Cyc_CfFlowInfo_InitParam_struct*_tmp5A6=(_tmp880=_region_malloc(
_tmp587,sizeof(*_tmp880)),((_tmp880[0]=((_tmp881.tag=2,((_tmp881.f1=(int)j,((
_tmp881.f2=(void*)_tmp59B,_tmp881)))))),_tmp880)));struct Cyc_CfFlowInfo_Place*
_tmp882;struct Cyc_CfFlowInfo_Place*_tmp5A7=(_tmp882=_region_malloc(_tmp587,
sizeof(*_tmp882)),((_tmp882->root=(void*)((void*)_tmp5A6),((_tmp882->fields=0,
_tmp882)))));_tmp58A=Cyc_Dict_insert(_tmp58A,(void*)_tmp5A6,Cyc_CfFlowInfo_typ_to_absrval(
fenv,_tmp59B,(void*)fenv->esc_none));{struct Cyc_CfFlowInfo_AddressOf_struct
_tmp888;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp887;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp885;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp884;_tmp58A=Cyc_Dict_insert(
_tmp58A,(void*)((_tmp884=_region_malloc(_tmp587,sizeof(*_tmp884)),((_tmp884[0]=((
_tmp885.tag=0,((_tmp885.f1=_tmp598,_tmp885)))),_tmp884)))),(void*)((_tmp887=
_region_malloc(_tmp587,sizeof(*_tmp887)),((_tmp887[0]=((_tmp888.tag=2,((_tmp888.f1=
_tmp5A7,_tmp888)))),_tmp887)))));}{struct Cyc_List_List*_tmp889;param_roots=((
_tmp889=_region_malloc(_tmp587,sizeof(*_tmp889)),((_tmp889->hd=_tmp5A7,((_tmp889->tl=
param_roots,_tmp889))))));}goto _LL36C;}_LL36F:;_LL370: {const char*_tmp88C;void*
_tmp88B;(_tmp88B=0,Cyc_Tcutil_terr(_tmp588,((_tmp88C="initializes attribute on non-pointer",
_tag_dyneither(_tmp88C,sizeof(char),37))),_tag_dyneither(_tmp88B,sizeof(void*),0)));}
_LL36C:;}goto _LL367;}}_LL36A:;_LL36B: goto _LL367;_LL367:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp88D;inflow=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp88D.ReachableFL).tag=1,(((
_tmp88D.ReachableFL).f1=_tmp58A,(((_tmp88D.ReachableFL).f2=_tmp58B,(((_tmp88D.ReachableFL).f3=
_tmp58C,_tmp88D))))))));}}_LL35D:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp587,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*_tmp88E;struct
Cyc_NewControlFlow_AnalEnv*env=(_tmp88E=_region_malloc(_tmp587,sizeof(*_tmp88E)),((
_tmp88E->r=_tmp587,((_tmp88E->fenv=fenv,((_tmp88E->iterate_again=1,((_tmp88E->iteration_num=
0,((_tmp88E->in_try=0,((_tmp88E->tryflow=inflow,((_tmp88E->all_changed=0,((
_tmp88E->noreturn=noreturn,((_tmp88E->param_roots=param_roots,((_tmp88E->flow_table=
flow_table,_tmp88E)))))))))))))))))))));union Cyc_CfFlowInfo_FlowInfo_union
outflow=inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5B3=outflow;_LL372: if((_tmp5B3.BottomFL).tag != 0)goto _LL374;_LL373: goto
_LL371;_LL374:;_LL375: Cyc_NewControlFlow_check_init_params(_tmp588,env,outflow);
if(noreturn){const char*_tmp891;void*_tmp890;(_tmp890=0,Cyc_Tcutil_terr(_tmp588,((
_tmp891="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp891,
sizeof(char),46))),_tag_dyneither(_tmp890,sizeof(void*),0)));}else{void*_tmp5B6=
Cyc_Tcutil_compress((void*)fd->ret_type);_LL377: if((int)_tmp5B6 != 0)goto _LL379;
_LL378: goto _LL376;_LL379: if(_tmp5B6 <= (void*)4)goto _LL37B;if(*((int*)_tmp5B6)!= 
6)goto _LL37B;_LL37A: goto _LL37C;_LL37B: if((int)_tmp5B6 != 1)goto _LL37D;_LL37C: goto
_LL37E;_LL37D: if(_tmp5B6 <= (void*)4)goto _LL37F;if(*((int*)_tmp5B6)!= 5)goto
_LL37F;_LL37E:{const char*_tmp894;void*_tmp893;(_tmp893=0,Cyc_Tcutil_warn(_tmp588,((
_tmp894="function may not return a value",_tag_dyneither(_tmp894,sizeof(char),32))),
_tag_dyneither(_tmp893,sizeof(void*),0)));}goto _LL376;_LL37F:;_LL380:{const char*
_tmp897;void*_tmp896;(_tmp896=0,Cyc_Tcutil_terr(_tmp588,((_tmp897="function may not return a value",
_tag_dyneither(_tmp897,sizeof(char),32))),_tag_dyneither(_tmp896,sizeof(void*),0)));}
goto _LL376;_LL376:;}goto _LL371;_LL371:;}}}}void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds);void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){void*_tmp5BC=(void*)((struct Cyc_Absyn_Decl*)ds->hd)->r;
struct Cyc_Absyn_Fndecl*_tmp5BD;struct Cyc_List_List*_tmp5BE;struct Cyc_List_List*
_tmp5BF;struct Cyc_List_List*_tmp5C0;_LL382: if(_tmp5BC <= (void*)2)goto _LL38C;if(*((
int*)_tmp5BC)!= 1)goto _LL384;_tmp5BD=((struct Cyc_Absyn_Fn_d_struct*)_tmp5BC)->f1;
_LL383: Cyc_NewControlFlow_check_fun(_tmp5BD);goto _LL381;_LL384: if(*((int*)
_tmp5BC)!= 12)goto _LL386;_tmp5BE=((struct Cyc_Absyn_ExternC_d_struct*)_tmp5BC)->f1;
_LL385: _tmp5BF=_tmp5BE;goto _LL387;_LL386: if(*((int*)_tmp5BC)!= 11)goto _LL388;
_tmp5BF=((struct Cyc_Absyn_Using_d_struct*)_tmp5BC)->f2;_LL387: _tmp5C0=_tmp5BF;
goto _LL389;_LL388: if(*((int*)_tmp5BC)!= 10)goto _LL38A;_tmp5C0=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp5BC)->f2;_LL389: Cyc_NewControlFlow_cf_check(_tmp5C0);goto _LL381;_LL38A: if(*((
int*)_tmp5BC)!= 13)goto _LL38C;_LL38B: goto _LL381;_LL38C:;_LL38D: goto _LL381;_LL381:;}}
