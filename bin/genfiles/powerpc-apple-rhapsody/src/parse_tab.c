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
struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[16];
extern char Cyc_Core_Free_Region[16];struct Cyc___cycFILE;extern struct Cyc___cycFILE*
Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct
Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;
long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,
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
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(
struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void*Cyc_List_fold_right(
void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x);extern char Cyc_List_Nth[
8];struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*
r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x);unsigned long Cyc_strlen(struct
_dyneither_ptr s);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_zstrcmp(struct _dyneither_ptr,struct _dyneither_ptr);int Cyc_zstrptrcmp(
struct _dyneither_ptr*,struct _dyneither_ptr*);struct _dyneither_ptr Cyc_strcat(
struct _dyneither_ptr dest,struct _dyneither_ptr src);struct _dyneither_ptr Cyc_strcpy(
struct _dyneither_ptr dest,struct _dyneither_ptr src);struct _dyneither_ptr Cyc_substring(
struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*
Cyc_Position_segment_of_abs(int,int);struct _dyneither_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dyneither_ptr source;
struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};struct Cyc_Position_Error*
Cyc_Position_mk_err_parse(struct Cyc_Position_Segment*,struct _dyneither_ptr);
extern char Cyc_Position_Nocontext[14];void Cyc_Position_post_error(struct Cyc_Position_Error*);
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_is_qvar_qualified(
struct _tuple1*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual
x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);void*Cyc_Absyn_int_typ(void*,void*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(int);void*Cyc_Absyn_starb_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zero_term);
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(
void*,struct Cyc_List_List*es,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*
exists,struct Cyc_List_List*po,struct Cyc_List_List*fs);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(
void*k,void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_tunion_decl(void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_xtunion,int is_flat,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(void*a);extern int Cyc_Absyn_porting_c_code;
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;
struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple4{void*f1;void*f2;};struct _tuple4*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple4*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;int allow_valueof;};void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_copy_type(void*t);void*
Cyc_Tcutil_compress(void*t);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct
Cyc_Core_Opt Cyc_Tcutil_trk;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt
Cyc_Tcutil_ik;struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(void*k);void*Cyc_Tcutil_kind_to_bound(
void*k);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Lex_register_typedef(
struct _tuple1*s);void Cyc_Lex_enter_namespace(struct _dyneither_ptr*);void Cyc_Lex_leave_namespace();
void Cyc_Lex_enter_using(struct _tuple1*);void Cyc_Lex_leave_using();int Cyc_Parse_no_register=
0;struct Cyc_Parse_Signed_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Unsigned_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Short_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct
Cyc_Parse_Long_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Type_spec_struct{
int tag;void*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Parse_Decl_spec_struct{
int tag;struct Cyc_Absyn_Decl*f1;};struct Cyc_Parse_Declaration_spec{struct Cyc_Core_Opt*
sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int is_inline;struct Cyc_List_List*
attributes;};struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;
};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _tuple5{
void*f1;struct Cyc_Core_Opt*f2;};static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);struct _tuple6{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple6
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
static struct Cyc_List_List*Cyc_Parse_parse_result=0;static void Cyc_Parse_err(
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg);static void Cyc_Parse_err(
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){Cyc_Position_post_error(
Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);static void*Cyc_Parse_abort(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
Cyc_Parse_err((struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);(
int)_throw((void*)Cyc_Position_Exit);}static void Cyc_Parse_unimp(struct
_dyneither_ptr msg,struct Cyc_Position_Segment*sg);static void Cyc_Parse_unimp(
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){{const char*_tmp8ED;void*
_tmp8EC[2];struct Cyc_String_pa_struct _tmp8EB;struct Cyc_String_pa_struct _tmp8EA;(
_tmp8EA.tag=0,((_tmp8EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((
_tmp8EB.tag=0,((_tmp8EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
sg)),((_tmp8EC[0]=& _tmp8EB,((_tmp8EC[1]=& _tmp8EA,Cyc_fprintf(Cyc_stderr,((
_tmp8ED="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp8ED,
sizeof(char),46))),_tag_dyneither(_tmp8EC,sizeof(void*),2)))))))))))));}return;}
static int Cyc_Parse_enum_counter=0;struct _tuple1*Cyc_Parse_gensym_enum();struct
_tuple1*Cyc_Parse_gensym_enum(){union Cyc_Absyn_Nmspace_union _tmp8FC;struct
_dyneither_ptr*_tmp8FB;const char*_tmp8FA;void*_tmp8F9[1];struct Cyc_Int_pa_struct
_tmp8F8;struct _tuple1*_tmp8F7;return(_tmp8F7=_cycalloc(sizeof(*_tmp8F7)),((
_tmp8F7->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp8FC.Rel_n).tag=1,(((_tmp8FC.Rel_n).f1=
0,_tmp8FC)))),((_tmp8F7->f2=((_tmp8FB=_cycalloc(sizeof(*_tmp8FB)),((_tmp8FB[0]=(
struct _dyneither_ptr)((_tmp8F8.tag=1,((_tmp8F8.f1=(unsigned long)Cyc_Parse_enum_counter
++,((_tmp8F9[0]=& _tmp8F8,Cyc_aprintf(((_tmp8FA="__anonymous_enum_%d__",
_tag_dyneither(_tmp8FA,sizeof(char),22))),_tag_dyneither(_tmp8F9,sizeof(void*),1)))))))),
_tmp8FB)))),_tmp8F7)))));}struct _tuple7{struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;
void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple8{struct
_tuple7*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
struct Cyc_Position_Segment*loc,struct _tuple8*field_info);static struct Cyc_Absyn_Aggrfield*
Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*loc,struct _tuple8*
field_info){struct _tuple8 _tmpB;struct _tuple7*_tmpC;struct _tuple7 _tmpD;struct
_tuple1*_tmpE;struct Cyc_Absyn_Tqual _tmpF;void*_tmp10;struct Cyc_List_List*_tmp11;
struct Cyc_List_List*_tmp12;struct Cyc_Absyn_Exp*_tmp13;struct _tuple8*_tmpA=
field_info;_tmpB=*_tmpA;_tmpC=_tmpB.f1;_tmpD=*_tmpC;_tmpE=_tmpD.f1;_tmpF=_tmpD.f2;
_tmp10=_tmpD.f3;_tmp11=_tmpD.f4;_tmp12=_tmpD.f5;_tmp13=_tmpB.f2;if(_tmp11 != 0){
const char*_tmp8FD;Cyc_Parse_err(((_tmp8FD="bad type params in struct field",
_tag_dyneither(_tmp8FD,sizeof(char),32))),loc);}if(Cyc_Absyn_is_qvar_qualified(
_tmpE)){const char*_tmp8FE;Cyc_Parse_err(((_tmp8FE="struct or union field cannot be qualified with a namespace",
_tag_dyneither(_tmp8FE,sizeof(char),59))),loc);}{struct Cyc_Absyn_Aggrfield*
_tmp8FF;return(_tmp8FF=_cycalloc(sizeof(*_tmp8FF)),((_tmp8FF->name=(*_tmpE).f2,((
_tmp8FF->tq=_tmpF,((_tmp8FF->type=(void*)_tmp10,((_tmp8FF->width=_tmp13,((
_tmp8FF->attributes=_tmp12,_tmp8FF)))))))))));}}static void*Cyc_Parse_type_spec(
void*t,struct Cyc_Position_Segment*loc);static void*Cyc_Parse_type_spec(void*t,
struct Cyc_Position_Segment*loc){struct Cyc_Parse_Type_spec_struct _tmp902;struct
Cyc_Parse_Type_spec_struct*_tmp901;return(void*)((_tmp901=_cycalloc(sizeof(*
_tmp901)),((_tmp901[0]=((_tmp902.tag=4,((_tmp902.f1=(void*)t,((_tmp902.f2=loc,
_tmp902)))))),_tmp901))));}static void*Cyc_Parse_array2ptr(void*t,int argposn);
static void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp19=t;struct Cyc_Absyn_ArrayInfo
_tmp1A;void*_tmp1B;struct Cyc_Absyn_Tqual _tmp1C;struct Cyc_Absyn_Exp*_tmp1D;struct
Cyc_Absyn_Conref*_tmp1E;struct Cyc_Position_Segment*_tmp1F;_LL1: if(_tmp19 <= (void*)
4)goto _LL3;if(*((int*)_tmp19)!= 7)goto _LL3;_tmp1A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp19)->f1;_tmp1B=(void*)_tmp1A.elt_type;_tmp1C=_tmp1A.tq;_tmp1D=_tmp1A.num_elts;
_tmp1E=_tmp1A.zero_term;_tmp1F=_tmp1A.zt_loc;_LL2: {struct Cyc_Absyn_Upper_b_struct
_tmp905;struct Cyc_Absyn_Upper_b_struct*_tmp904;return Cyc_Absyn_starb_typ(_tmp1B,
argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0):(void*)2,
_tmp1C,_tmp1D == 0?(void*)0:(void*)((_tmp904=_cycalloc(sizeof(*_tmp904)),((
_tmp904[0]=((_tmp905.tag=0,((_tmp905.f1=(struct Cyc_Absyn_Exp*)_tmp1D,_tmp905)))),
_tmp904)))),_tmp1E);}_LL3:;_LL4: return t;_LL0:;}static void Cyc_Parse_arg_array2ptr(
struct _tuple2*x);static void Cyc_Parse_arg_array2ptr(struct _tuple2*x){(*x).f3=Cyc_Parse_array2ptr((*
x).f3,1);}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple9*
Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct _tuple2*t);static
struct _tuple9*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct
_tuple2*t){struct _tuple9*_tmp906;return(_tmp906=_cycalloc(sizeof(*_tmp906)),((
_tmp906->f1=(*t).f2,((_tmp906->f2=(*t).f3,_tmp906)))));}static struct Cyc_Absyn_Vardecl*
Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d);static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(
struct Cyc_Absyn_Decl*d){void*_tmp23=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp24;
_LL6: if(_tmp23 <= (void*)2)goto _LL8;if(*((int*)_tmp23)!= 0)goto _LL8;_tmp24=((
struct Cyc_Absyn_Var_d_struct*)_tmp23)->f1;_LL7: return _tmp24;_LL8:;_LL9: {const
char*_tmp909;void*_tmp908;(_tmp908=0,((int(*)(struct Cyc_Position_Segment*sg,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(d->loc,((
_tmp909="bad declaration in `forarray' statement",_tag_dyneither(_tmp909,sizeof(
char),40))),_tag_dyneither(_tmp908,sizeof(void*),0)));}_LL5:;}static int Cyc_Parse_is_typeparam(
void*tm);static int Cyc_Parse_is_typeparam(void*tm){void*_tmp27=tm;_LLB: if(*((int*)
_tmp27)!= 4)goto _LLD;_LLC: return 1;_LLD:;_LLE: return 0;_LLA:;}static void*Cyc_Parse_id2type(
struct _dyneither_ptr s,void*k);static void*Cyc_Parse_id2type(struct _dyneither_ptr s,
void*k){const char*_tmp90A;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp90A="`H",
_tag_dyneither(_tmp90A,sizeof(char),3))))== 0)return(void*)2;else{const char*
_tmp90B;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp90B="`U",_tag_dyneither(
_tmp90B,sizeof(char),3))))== 0)return(void*)3;else{struct Cyc_Absyn_VarType_struct
_tmp915;struct _dyneither_ptr*_tmp914;struct Cyc_Absyn_Tvar*_tmp913;struct Cyc_Absyn_VarType_struct*
_tmp912;return(void*)((_tmp912=_cycalloc(sizeof(*_tmp912)),((_tmp912[0]=((
_tmp915.tag=1,((_tmp915.f1=((_tmp913=_cycalloc(sizeof(*_tmp913)),((_tmp913->name=((
_tmp914=_cycalloc(sizeof(*_tmp914)),((_tmp914[0]=s,_tmp914)))),((_tmp913->identity=
- 1,((_tmp913->kind=(void*)k,_tmp913)))))))),_tmp915)))),_tmp912))));}}}static
struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t);static struct Cyc_Absyn_Tvar*
Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){void*k;{void*_tmp2E=Cyc_Absyn_compress_kb((
void*)t->kind);void*_tmp2F;_LL10: if(*((int*)_tmp2E)!= 1)goto _LL12;_LL11:{struct
Cyc_Absyn_Unknown_kb_struct _tmp918;struct Cyc_Absyn_Unknown_kb_struct*_tmp917;k=(
void*)((_tmp917=_cycalloc(sizeof(*_tmp917)),((_tmp917[0]=((_tmp918.tag=1,((
_tmp918.f1=0,_tmp918)))),_tmp917))));}goto _LLF;_LL12: if(*((int*)_tmp2E)!= 2)goto
_LL14;_tmp2F=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp2E)->f2;_LL13:{struct
Cyc_Absyn_Less_kb_struct _tmp91B;struct Cyc_Absyn_Less_kb_struct*_tmp91A;k=(void*)((
_tmp91A=_cycalloc(sizeof(*_tmp91A)),((_tmp91A[0]=((_tmp91B.tag=2,((_tmp91B.f1=0,((
_tmp91B.f2=(void*)_tmp2F,_tmp91B)))))),_tmp91A))));}goto _LLF;_LL14:;_LL15: k=
_tmp2E;goto _LLF;_LLF:;}{struct Cyc_Absyn_Tvar*_tmp91C;return(_tmp91C=_cycalloc(
sizeof(*_tmp91C)),((_tmp91C->name=t->name,((_tmp91C->identity=- 1,((_tmp91C->kind=(
void*)k,_tmp91C)))))));}}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t);static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t){void*_tmp35=t;struct Cyc_Absyn_Tvar*_tmp36;_LL17: if(_tmp35 <= (void*)4)
goto _LL19;if(*((int*)_tmp35)!= 1)goto _LL19;_tmp36=((struct Cyc_Absyn_VarType_struct*)
_tmp35)->f1;_LL18: return _tmp36;_LL19:;_LL1A: {const char*_tmp91F;void*_tmp91E;(
_tmp91E=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp91F="expecting a list of type variables, not types",
_tag_dyneither(_tmp91F,sizeof(char),46))),_tag_dyneither(_tmp91E,sizeof(void*),0)));}
_LL16:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr);static void*Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar*pr){struct Cyc_Absyn_VarType_struct _tmp922;struct Cyc_Absyn_VarType_struct*
_tmp921;return(void*)((_tmp921=_cycalloc(sizeof(*_tmp921)),((_tmp921[0]=((
_tmp922.tag=1,((_tmp922.f1=pr,_tmp922)))),_tmp921))));}static void Cyc_Parse_set_vartyp_kind(
void*t,void*k,int leq);static void Cyc_Parse_set_vartyp_kind(void*t,void*k,int leq){
void*_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp3C;struct Cyc_Absyn_Tvar
_tmp3D;void*_tmp3E;void**_tmp3F;_LL1C: if(_tmp3B <= (void*)4)goto _LL1E;if(*((int*)
_tmp3B)!= 1)goto _LL1E;_tmp3C=((struct Cyc_Absyn_VarType_struct*)_tmp3B)->f1;
_tmp3D=*_tmp3C;_tmp3E=(void*)_tmp3D.kind;_tmp3F=(void**)&(*((struct Cyc_Absyn_VarType_struct*)
_tmp3B)->f1).kind;_LL1D: {void*_tmp40=Cyc_Absyn_compress_kb(*_tmp3F);_LL21: if(*((
int*)_tmp40)!= 1)goto _LL23;_LL22: if(!leq)*_tmp3F=Cyc_Tcutil_kind_to_bound(k);
else{struct Cyc_Absyn_Less_kb_struct _tmp925;struct Cyc_Absyn_Less_kb_struct*
_tmp924;*_tmp3F=(void*)((_tmp924=_cycalloc(sizeof(*_tmp924)),((_tmp924[0]=((
_tmp925.tag=2,((_tmp925.f1=0,((_tmp925.f2=(void*)k,_tmp925)))))),_tmp924))));}
return;_LL23:;_LL24: return;_LL20:;}_LL1E:;_LL1F: return;_LL1B:;}static struct Cyc_List_List*
Cyc_Parse_oldstyle2newstyle(struct Cyc_List_List*tms,struct Cyc_List_List*tds,
struct Cyc_Position_Segment*loc);static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp43=(void*)tms->hd;void*_tmp44;
_LL26: if(*((int*)_tmp43)!= 3)goto _LL28;_tmp44=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp43)->f1;_LL27: if(tms->tl == 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 
0){void*_tmp45=_tmp44;struct Cyc_List_List*_tmp46;_LL2B: if(*((int*)_tmp45)!= 1)
goto _LL2D;_LL2C:{const char*_tmp928;void*_tmp927;(_tmp927=0,Cyc_Tcutil_warn(loc,((
_tmp928="function declaration with both new- and old-style parameter declarations; ignoring old-style",
_tag_dyneither(_tmp928,sizeof(char),93))),_tag_dyneither(_tmp927,sizeof(void*),0)));}
return tms;_LL2D: if(*((int*)_tmp45)!= 0)goto _LL2A;_tmp46=((struct Cyc_Absyn_NoTypes_struct*)
_tmp45)->f1;_LL2E: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp46)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){const char*_tmp92B;void*
_tmp92A;(_tmp92A=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp92B="wrong number of parameter declarations in old-style function declaration",
_tag_dyneither(_tmp92B,sizeof(char),73))),_tag_dyneither(_tmp92A,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp4B=0;for(0;_tmp46 != 0;_tmp46=_tmp46->tl){struct Cyc_List_List*
_tmp4C=tds;for(0;_tmp4C != 0;_tmp4C=_tmp4C->tl){struct Cyc_Absyn_Decl*_tmp4D=(
struct Cyc_Absyn_Decl*)_tmp4C->hd;void*_tmp4E=(void*)_tmp4D->r;struct Cyc_Absyn_Vardecl*
_tmp4F;_LL30: if(_tmp4E <= (void*)2)goto _LL32;if(*((int*)_tmp4E)!= 0)goto _LL32;
_tmp4F=((struct Cyc_Absyn_Var_d_struct*)_tmp4E)->f1;_LL31: if(Cyc_zstrptrcmp((*
_tmp4F->name).f2,(struct _dyneither_ptr*)_tmp46->hd)!= 0)continue;if(_tmp4F->initializer
!= 0){const char*_tmp92E;void*_tmp92D;(_tmp92D=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,((
_tmp92E="initializer found in parameter declaration",_tag_dyneither(_tmp92E,
sizeof(char),43))),_tag_dyneither(_tmp92D,sizeof(void*),0)));}if(Cyc_Absyn_is_qvar_qualified(
_tmp4F->name)){const char*_tmp931;void*_tmp930;(_tmp930=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,((
_tmp931="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp931,
sizeof(char),47))),_tag_dyneither(_tmp930,sizeof(void*),0)));}{struct _tuple2*
_tmp937;struct Cyc_Core_Opt*_tmp936;struct Cyc_List_List*_tmp935;_tmp4B=((_tmp935=
_cycalloc(sizeof(*_tmp935)),((_tmp935->hd=((_tmp937=_cycalloc(sizeof(*_tmp937)),((
_tmp937->f1=((_tmp936=_cycalloc(sizeof(*_tmp936)),((_tmp936->v=(*_tmp4F->name).f2,
_tmp936)))),((_tmp937->f2=_tmp4F->tq,((_tmp937->f3=(void*)_tmp4F->type,_tmp937)))))))),((
_tmp935->tl=_tmp4B,_tmp935))))));}goto L;_LL32:;_LL33: {const char*_tmp93A;void*
_tmp939;(_tmp939=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,((_tmp93A="nonvariable declaration in parameter type",
_tag_dyneither(_tmp93A,sizeof(char),42))),_tag_dyneither(_tmp939,sizeof(void*),0)));}
_LL2F:;}L: if(_tmp4C == 0){const char*_tmp93E;void*_tmp93D[1];struct Cyc_String_pa_struct
_tmp93C;(_tmp93C.tag=0,((_tmp93C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmp46->hd)),((_tmp93D[0]=& _tmp93C,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp93E="%s is not given a type",_tag_dyneither(_tmp93E,sizeof(char),23))),
_tag_dyneither(_tmp93D,sizeof(void*),1)))))));}}{struct Cyc_Absyn_Function_mod_struct*
_tmp94D;struct Cyc_Absyn_WithTypes_struct*_tmp94C;struct Cyc_Absyn_WithTypes_struct
_tmp94B;struct Cyc_Absyn_Function_mod_struct _tmp94A;struct Cyc_List_List*_tmp949;
return(_tmp949=_cycalloc(sizeof(*_tmp949)),((_tmp949->hd=(void*)((void*)((
_tmp94D=_cycalloc(sizeof(*_tmp94D)),((_tmp94D[0]=((_tmp94A.tag=3,((_tmp94A.f1=(
void*)((void*)((_tmp94C=_cycalloc(sizeof(*_tmp94C)),((_tmp94C[0]=((_tmp94B.tag=1,((
_tmp94B.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp4B),((_tmp94B.f2=0,((_tmp94B.f3=0,((_tmp94B.f4=0,((_tmp94B.f5=0,_tmp94B)))))))))))),
_tmp94C))))),_tmp94A)))),_tmp94D))))),((_tmp949->tl=0,_tmp949)))));}}_LL2A:;}
goto _LL29;_LL28:;_LL29: {struct Cyc_List_List*_tmp94E;return(_tmp94E=_cycalloc(
sizeof(*_tmp94E)),((_tmp94E->hd=(void*)((void*)tms->hd),((_tmp94E->tl=Cyc_Parse_oldstyle2newstyle(
tms->tl,tds,loc),_tmp94E)))));}_LL25:;}}struct _tuple10{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(
struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,
struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc);static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,
struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){
if(tds != 0){struct Cyc_Parse_Declarator*_tmp94F;d=((_tmp94F=_cycalloc(sizeof(*
_tmp94F)),((_tmp94F->id=d->id,((_tmp94F->tms=Cyc_Parse_oldstyle2newstyle(d->tms,
tds,loc),_tmp94F))))));}{void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual
tq=Cyc_Absyn_empty_tqual(0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){
tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)
dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;
atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc != 0){void*_tmp63=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v;_LL35: if((int)_tmp63 != 1)goto _LL37;
_LL36: sc=(void*)3;goto _LL34;_LL37: if((int)_tmp63 != 3)goto _LL39;_LL38: sc=(void*)0;
goto _LL34;_LL39:;_LL3A:{const char*_tmp950;Cyc_Parse_err(((_tmp950="bad storage class on function",
_tag_dyneither(_tmp950,sizeof(char),30))),loc);}goto _LL34;_LL34:;}}{void*_tmp66;
struct Cyc_Core_Opt*_tmp67;struct _tuple5 _tmp65=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp66=_tmp65.f1;_tmp67=_tmp65.f2;{struct Cyc_Absyn_Tqual _tmp69;void*
_tmp6A;struct Cyc_List_List*_tmp6B;struct Cyc_List_List*_tmp6C;struct _tuple6 _tmp68=
Cyc_Parse_apply_tms(tq,_tmp66,atts,d->tms);_tmp69=_tmp68.f1;_tmp6A=_tmp68.f2;
_tmp6B=_tmp68.f3;_tmp6C=_tmp68.f4;if(_tmp67 != 0){const char*_tmp953;void*_tmp952;(
_tmp952=0,Cyc_Tcutil_warn(loc,((_tmp953="nested type declaration within function prototype",
_tag_dyneither(_tmp953,sizeof(char),50))),_tag_dyneither(_tmp952,sizeof(void*),0)));}
if(_tmp6B != 0){const char*_tmp956;void*_tmp955;(_tmp955=0,Cyc_Tcutil_warn(loc,((
_tmp956="bad type params, ignoring",_tag_dyneither(_tmp956,sizeof(char),26))),
_tag_dyneither(_tmp955,sizeof(void*),0)));}{void*_tmp71=_tmp6A;struct Cyc_Absyn_FnInfo
_tmp72;struct Cyc_List_List*_tmp73;struct Cyc_Core_Opt*_tmp74;void*_tmp75;struct
Cyc_List_List*_tmp76;int _tmp77;struct Cyc_Absyn_VarargInfo*_tmp78;struct Cyc_List_List*
_tmp79;struct Cyc_List_List*_tmp7A;_LL3C: if(_tmp71 <= (void*)4)goto _LL3E;if(*((int*)
_tmp71)!= 8)goto _LL3E;_tmp72=((struct Cyc_Absyn_FnType_struct*)_tmp71)->f1;_tmp73=
_tmp72.tvars;_tmp74=_tmp72.effect;_tmp75=(void*)_tmp72.ret_typ;_tmp76=_tmp72.args;
_tmp77=_tmp72.c_varargs;_tmp78=_tmp72.cyc_varargs;_tmp79=_tmp72.rgn_po;_tmp7A=
_tmp72.attributes;_LL3D: {struct Cyc_List_List*_tmp7B=0;{struct Cyc_List_List*
_tmp7C=_tmp76;for(0;_tmp7C != 0;_tmp7C=_tmp7C->tl){struct _tuple2 _tmp7E;struct Cyc_Core_Opt*
_tmp7F;struct Cyc_Absyn_Tqual _tmp80;void*_tmp81;struct _tuple2*_tmp7D=(struct
_tuple2*)_tmp7C->hd;_tmp7E=*_tmp7D;_tmp7F=_tmp7E.f1;_tmp80=_tmp7E.f2;_tmp81=
_tmp7E.f3;if(_tmp7F == 0){{const char*_tmp957;Cyc_Parse_err(((_tmp957="missing argument variable in function prototype",
_tag_dyneither(_tmp957,sizeof(char),48))),loc);}{struct _tuple10*_tmp95A;struct
Cyc_List_List*_tmp959;_tmp7B=((_tmp959=_cycalloc(sizeof(*_tmp959)),((_tmp959->hd=((
_tmp95A=_cycalloc(sizeof(*_tmp95A)),((_tmp95A->f1=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp95A->f2=_tmp80,((_tmp95A->f3=
_tmp81,_tmp95A)))))))),((_tmp959->tl=_tmp7B,_tmp959))))));}}else{struct _tuple10*
_tmp95D;struct Cyc_List_List*_tmp95C;_tmp7B=((_tmp95C=_cycalloc(sizeof(*_tmp95C)),((
_tmp95C->hd=((_tmp95D=_cycalloc(sizeof(*_tmp95D)),((_tmp95D->f1=(struct
_dyneither_ptr*)_tmp7F->v,((_tmp95D->f2=_tmp80,((_tmp95D->f3=_tmp81,_tmp95D)))))))),((
_tmp95C->tl=_tmp7B,_tmp95C))))));}}}{struct Cyc_Absyn_Fndecl*_tmp95E;return(
_tmp95E=_cycalloc(sizeof(*_tmp95E)),((_tmp95E->sc=(void*)sc,((_tmp95E->name=d->id,((
_tmp95E->tvs=_tmp73,((_tmp95E->is_inline=is_inline,((_tmp95E->effect=_tmp74,((
_tmp95E->ret_type=(void*)_tmp75,((_tmp95E->args=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp7B),((_tmp95E->c_varargs=_tmp77,((_tmp95E->cyc_varargs=
_tmp78,((_tmp95E->rgn_po=_tmp79,((_tmp95E->body=body,((_tmp95E->cached_typ=0,((
_tmp95E->param_vardecls=0,((_tmp95E->fn_vardecl=0,((_tmp95E->attributes=Cyc_List_append(
_tmp7A,_tmp6C),_tmp95E)))))))))))))))))))))))))))))));}}_LL3E:;_LL3F: {const char*
_tmp961;void*_tmp960;(_tmp960=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp961="declarator is not a function prototype",
_tag_dyneither(_tmp961,sizeof(char),39))),_tag_dyneither(_tmp960,sizeof(void*),0)));}
_LL3B:;}}}}}static char _tmp8B[52]="at most one type may appear within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg1={_tmp8B,_tmp8B,_tmp8B + 52};static char
_tmp8C[63]="const or volatile may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg2={_tmp8C,_tmp8C,_tmp8C + 63};static char
_tmp8D[50]="type specifier includes more than one declaration";static struct
_dyneither_ptr Cyc_Parse_msg3={_tmp8D,_tmp8D,_tmp8D + 50};static char _tmp8E[60]="sign specifier may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp8E,_tmp8E,_tmp8E + 60};static struct
_tuple5 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc);static struct _tuple5 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*
ts,struct Cyc_Position_Segment*loc){struct Cyc_Core_Opt*declopt=0;int seen_type=0;
int seen_sign=0;int seen_size=0;void*t=(void*)0;void*sz=(void*)2;void*sgn=(void*)0;
struct Cyc_Position_Segment*last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp8F=(
void*)ts->hd;void*_tmp90;struct Cyc_Position_Segment*_tmp91;struct Cyc_Position_Segment*
_tmp92;struct Cyc_Position_Segment*_tmp93;struct Cyc_Position_Segment*_tmp94;
struct Cyc_Position_Segment*_tmp95;struct Cyc_Absyn_Decl*_tmp96;_LL41: if(*((int*)
_tmp8F)!= 4)goto _LL43;_tmp90=(void*)((struct Cyc_Parse_Type_spec_struct*)_tmp8F)->f1;
_tmp91=((struct Cyc_Parse_Type_spec_struct*)_tmp8F)->f2;_LL42: if(seen_type)Cyc_Parse_err(
Cyc_Parse_msg1,_tmp91);last_loc=_tmp91;seen_type=1;t=_tmp90;goto _LL40;_LL43: if(*((
int*)_tmp8F)!= 0)goto _LL45;_tmp92=((struct Cyc_Parse_Signed_spec_struct*)_tmp8F)->f1;
_LL44: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp92);if(seen_type){const char*
_tmp962;Cyc_Parse_err(((_tmp962="signed qualifier must come before type",
_tag_dyneither(_tmp962,sizeof(char),39))),_tmp92);}last_loc=_tmp92;seen_sign=1;
sgn=(void*)0;goto _LL40;_LL45: if(*((int*)_tmp8F)!= 1)goto _LL47;_tmp93=((struct Cyc_Parse_Unsigned_spec_struct*)
_tmp8F)->f1;_LL46: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp93);if(seen_type){
const char*_tmp963;Cyc_Parse_err(((_tmp963="signed qualifier must come before type",
_tag_dyneither(_tmp963,sizeof(char),39))),_tmp93);}last_loc=_tmp93;seen_sign=1;
sgn=(void*)1;goto _LL40;_LL47: if(*((int*)_tmp8F)!= 2)goto _LL49;_tmp94=((struct Cyc_Parse_Short_spec_struct*)
_tmp8F)->f1;_LL48: if(seen_size){const char*_tmp964;Cyc_Parse_err(((_tmp964="integral size may appear only once within a type specifier",
_tag_dyneither(_tmp964,sizeof(char),59))),_tmp94);}if(seen_type){const char*
_tmp965;Cyc_Parse_err(((_tmp965="short modifier must come before base type",
_tag_dyneither(_tmp965,sizeof(char),42))),_tmp94);}last_loc=_tmp94;seen_size=1;
sz=(void*)1;goto _LL40;_LL49: if(*((int*)_tmp8F)!= 3)goto _LL4B;_tmp95=((struct Cyc_Parse_Long_spec_struct*)
_tmp8F)->f1;_LL4A: if(seen_type){const char*_tmp966;Cyc_Parse_err(((_tmp966="long modifier must come before base type",
_tag_dyneither(_tmp966,sizeof(char),41))),_tmp95);}if(seen_size){void*_tmp9C=sz;
_LL4E: if((int)_tmp9C != 3)goto _LL50;_LL4F: sz=(void*)4;goto _LL4D;_LL50:;_LL51:{
const char*_tmp967;Cyc_Parse_err(((_tmp967="extra long in type specifier",
_tag_dyneither(_tmp967,sizeof(char),29))),_tmp95);}goto _LL4D;_LL4D:;}else{sz=(
void*)3;}last_loc=_tmp95;seen_size=1;goto _LL40;_LL4B: if(*((int*)_tmp8F)!= 5)goto
_LL40;_tmp96=((struct Cyc_Parse_Decl_spec_struct*)_tmp8F)->f1;_LL4C: last_loc=
_tmp96->loc;if(declopt == 0  && !seen_type){seen_type=1;{void*_tmp9E=(void*)_tmp96->r;
struct Cyc_Absyn_Aggrdecl*_tmp9F;struct Cyc_Absyn_Tuniondecl*_tmpA0;struct Cyc_Absyn_Enumdecl*
_tmpA1;_LL53: if(_tmp9E <= (void*)2)goto _LL59;if(*((int*)_tmp9E)!= 6)goto _LL55;
_tmp9F=((struct Cyc_Absyn_Aggr_d_struct*)_tmp9E)->f1;_LL54: {struct Cyc_List_List*
_tmpA2=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmp9F->tvs));{struct Cyc_Absyn_AggrType_struct _tmp971;union Cyc_Absyn_AggrInfoU_union
_tmp970;struct Cyc_Absyn_AggrInfo _tmp96F;struct Cyc_Absyn_AggrType_struct*_tmp96E;
t=(void*)((_tmp96E=_cycalloc(sizeof(*_tmp96E)),((_tmp96E[0]=((_tmp971.tag=10,((
_tmp971.f1=((_tmp96F.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((_tmp970.UnknownAggr).tag=
0,(((_tmp970.UnknownAggr).f1=(void*)((void*)_tmp9F->kind),(((_tmp970.UnknownAggr).f2=
_tmp9F->name,_tmp970)))))),((_tmp96F.targs=_tmpA2,_tmp96F)))),_tmp971)))),
_tmp96E))));}if(_tmp9F->impl != 0){struct Cyc_Core_Opt*_tmp972;declopt=((_tmp972=
_cycalloc(sizeof(*_tmp972)),((_tmp972->v=_tmp96,_tmp972))));}goto _LL52;}_LL55:
if(*((int*)_tmp9E)!= 7)goto _LL57;_tmpA0=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp9E)->f1;_LL56: {struct Cyc_List_List*_tmpA8=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmpA0->tvs));struct Cyc_Core_Opt*
_tmp973;struct Cyc_Core_Opt*ropt=_tmpA0->is_flat?0:((_tmp973=_cycalloc(sizeof(*
_tmp973)),((_tmp973->v=(void*)((void*)2),_tmp973))));{struct Cyc_Absyn_TunionType_struct
_tmp982;union Cyc_Absyn_TunionInfoU_union _tmp981;struct Cyc_Absyn_Tuniondecl**
_tmp980;struct Cyc_Absyn_TunionInfo _tmp97F;struct Cyc_Absyn_TunionType_struct*
_tmp97E;t=(void*)((_tmp97E=_cycalloc(sizeof(*_tmp97E)),((_tmp97E[0]=((_tmp982.tag=
2,((_tmp982.f1=((_tmp97F.tunion_info=(union Cyc_Absyn_TunionInfoU_union)(((
_tmp981.KnownTunion).tag=1,(((_tmp981.KnownTunion).f1=((_tmp980=_cycalloc(
sizeof(*_tmp980)),((_tmp980[0]=_tmpA0,_tmp980)))),_tmp981)))),((_tmp97F.targs=
_tmpA8,((_tmp97F.rgn=ropt,_tmp97F)))))),_tmp982)))),_tmp97E))));}if(_tmpA0->fields
!= 0){struct Cyc_Core_Opt*_tmp983;declopt=((_tmp983=_cycalloc(sizeof(*_tmp983)),((
_tmp983->v=_tmp96,_tmp983))));}goto _LL52;}_LL57: if(*((int*)_tmp9E)!= 8)goto _LL59;
_tmpA1=((struct Cyc_Absyn_Enum_d_struct*)_tmp9E)->f1;_LL58:{struct Cyc_Absyn_EnumType_struct
_tmp986;struct Cyc_Absyn_EnumType_struct*_tmp985;t=(void*)((_tmp985=_cycalloc(
sizeof(*_tmp985)),((_tmp985[0]=((_tmp986.tag=12,((_tmp986.f1=_tmpA1->name,((
_tmp986.f2=0,_tmp986)))))),_tmp985))));}{struct Cyc_Core_Opt*_tmp987;declopt=((
_tmp987=_cycalloc(sizeof(*_tmp987)),((_tmp987->v=_tmp96,_tmp987))));}goto _LL52;
_LL59:;_LL5A: {const char*_tmp98A;void*_tmp989;(_tmp989=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp96->loc,((
_tmp98A="bad declaration within type specifier",_tag_dyneither(_tmp98A,sizeof(
char),38))),_tag_dyneither(_tmp989,sizeof(void*),0)));}_LL52:;}}else{Cyc_Parse_err(
Cyc_Parse_msg3,_tmp96->loc);}goto _LL40;_LL40:;}if(!seen_type){if(!seen_sign  && !
seen_size){const char*_tmp98D;void*_tmp98C;(_tmp98C=0,Cyc_Tcutil_warn(last_loc,((
_tmp98D="missing type within specifier",_tag_dyneither(_tmp98D,sizeof(char),30))),
_tag_dyneither(_tmp98C,sizeof(void*),0)));}t=Cyc_Absyn_int_typ(sgn,sz);}else{if(
seen_sign){void*_tmpB7=t;void*_tmpB8;void*_tmpB9;_LL5C: if(_tmpB7 <= (void*)4)goto
_LL5E;if(*((int*)_tmpB7)!= 5)goto _LL5E;_tmpB8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpB7)->f1;_tmpB9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB7)->f2;_LL5D:
if(_tmpB8 != sgn)t=Cyc_Absyn_int_typ(sgn,_tmpB9);goto _LL5B;_LL5E:;_LL5F:{const
char*_tmp98E;Cyc_Parse_err(((_tmp98E="sign specification on non-integral type",
_tag_dyneither(_tmp98E,sizeof(char),40))),last_loc);}goto _LL5B;_LL5B:;}if(
seen_size){void*_tmpBB=t;void*_tmpBC;void*_tmpBD;_LL61: if(_tmpBB <= (void*)4)goto
_LL65;if(*((int*)_tmpBB)!= 5)goto _LL63;_tmpBC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpBB)->f1;_tmpBD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBB)->f2;_LL62:
if(_tmpBD != sz)t=Cyc_Absyn_int_typ(_tmpBC,sz);goto _LL60;_LL63: if(*((int*)_tmpBB)
!= 6)goto _LL65;_LL64: t=Cyc_Absyn_double_typ(1);goto _LL60;_LL65:;_LL66:{const char*
_tmp98F;Cyc_Parse_err(((_tmp98F="size qualifier on non-integral type",
_tag_dyneither(_tmp98F,sizeof(char),36))),last_loc);}goto _LL60;_LL60:;}}{struct
_tuple5 _tmp990;return(_tmp990.f1=t,((_tmp990.f2=declopt,_tmp990)));}}static
struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts);static struct
Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,
void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{
struct Cyc_Parse_Declarator*_tmpC0=(struct Cyc_Parse_Declarator*)ds->hd;struct
_tuple1*_tmpC1=_tmpC0->id;struct Cyc_Absyn_Tqual _tmpC3;void*_tmpC4;struct Cyc_List_List*
_tmpC5;struct Cyc_List_List*_tmpC6;struct _tuple6 _tmpC2=Cyc_Parse_apply_tms(tq,t,
shared_atts,_tmpC0->tms);_tmpC3=_tmpC2.f1;_tmpC4=_tmpC2.f2;_tmpC5=_tmpC2.f3;
_tmpC6=_tmpC2.f4;if(ds->tl == 0){struct _tuple7*_tmp993;struct Cyc_List_List*
_tmp992;return(_tmp992=_region_malloc(r,sizeof(*_tmp992)),((_tmp992->hd=((
_tmp993=_region_malloc(r,sizeof(*_tmp993)),((_tmp993->f1=_tmpC1,((_tmp993->f2=
_tmpC3,((_tmp993->f3=_tmpC4,((_tmp993->f4=_tmpC5,((_tmp993->f5=_tmpC6,_tmp993)))))))))))),((
_tmp992->tl=0,_tmp992)))));}else{struct _tuple7*_tmp996;struct Cyc_List_List*
_tmp995;return(_tmp995=_region_malloc(r,sizeof(*_tmp995)),((_tmp995->hd=((
_tmp996=_region_malloc(r,sizeof(*_tmp996)),((_tmp996->f1=_tmpC1,((_tmp996->f2=
_tmpC3,((_tmp996->f3=_tmpC4,((_tmp996->f4=_tmpC5,((_tmp996->f5=_tmpC6,_tmp996)))))))))))),((
_tmp995->tl=Cyc_Parse_apply_tmss(r,_tmpC3,Cyc_Tcutil_copy_type(t),ds->tl,
shared_atts),_tmp995)))));}}}static struct _tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms);static struct _tuple6
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,
struct Cyc_List_List*tms){if(tms == 0){struct _tuple6 _tmp997;return(_tmp997.f1=tq,((
_tmp997.f2=t,((_tmp997.f3=0,((_tmp997.f4=atts,_tmp997)))))));}{void*_tmpCC=(void*)
tms->hd;struct Cyc_Absyn_Conref*_tmpCD;struct Cyc_Position_Segment*_tmpCE;struct
Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Conref*_tmpD0;struct Cyc_Position_Segment*
_tmpD1;void*_tmpD2;struct Cyc_List_List*_tmpD3;struct Cyc_Position_Segment*_tmpD4;
struct Cyc_Absyn_PtrAtts _tmpD5;struct Cyc_Absyn_Tqual _tmpD6;struct Cyc_Position_Segment*
_tmpD7;struct Cyc_List_List*_tmpD8;_LL68: if(*((int*)_tmpCC)!= 0)goto _LL6A;_tmpCD=((
struct Cyc_Absyn_Carray_mod_struct*)_tmpCC)->f1;_tmpCE=((struct Cyc_Absyn_Carray_mod_struct*)
_tmpCC)->f2;_LL69: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(
t,tq,0,_tmpCD,_tmpCE),atts,tms->tl);_LL6A: if(*((int*)_tmpCC)!= 1)goto _LL6C;
_tmpCF=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCC)->f1;_tmpD0=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpCC)->f2;_tmpD1=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCC)->f3;_LL6B:
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(
struct Cyc_Absyn_Exp*)_tmpCF,_tmpD0,_tmpD1),atts,tms->tl);_LL6C: if(*((int*)_tmpCC)
!= 3)goto _LL6E;_tmpD2=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmpCC)->f1;
_LL6D: {void*_tmpD9=_tmpD2;struct Cyc_List_List*_tmpDA;int _tmpDB;struct Cyc_Absyn_VarargInfo*
_tmpDC;struct Cyc_Core_Opt*_tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_Position_Segment*
_tmpDF;_LL75: if(*((int*)_tmpD9)!= 1)goto _LL77;_tmpDA=((struct Cyc_Absyn_WithTypes_struct*)
_tmpD9)->f1;_tmpDB=((struct Cyc_Absyn_WithTypes_struct*)_tmpD9)->f2;_tmpDC=((
struct Cyc_Absyn_WithTypes_struct*)_tmpD9)->f3;_tmpDD=((struct Cyc_Absyn_WithTypes_struct*)
_tmpD9)->f4;_tmpDE=((struct Cyc_Absyn_WithTypes_struct*)_tmpD9)->f5;_LL76: {
struct Cyc_List_List*typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*
new_atts=0;{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((
void*)as->hd)){struct Cyc_List_List*_tmp998;fn_atts=((_tmp998=_cycalloc(sizeof(*
_tmp998)),((_tmp998->hd=(void*)((void*)as->hd),((_tmp998->tl=fn_atts,_tmp998))))));}
else{struct Cyc_List_List*_tmp999;new_atts=((_tmp999=_cycalloc(sizeof(*_tmp999)),((
_tmp999->hd=(void*)((void*)as->hd),((_tmp999->tl=new_atts,_tmp999))))));}}}if(
tms->tl != 0){void*_tmpE2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;
struct Cyc_List_List*_tmpE3;_LL7A: if(*((int*)_tmpE2)!= 4)goto _LL7C;_tmpE3=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpE2)->f1;_LL7B: typvars=_tmpE3;tms=tms->tl;
goto _LL79;_LL7C:;_LL7D: goto _LL79;_LL79:;}if(((((!_tmpDB  && _tmpDC == 0) && _tmpDA
!= 0) && _tmpDA->tl == 0) && (*((struct _tuple2*)_tmpDA->hd)).f1 == 0) && (*((
struct _tuple2*)_tmpDA->hd)).f3 == (void*)0)_tmpDA=0;t=Cyc_Parse_array2ptr(t,0);((
void(*)(void(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,
_tmpDA);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(tq.loc),Cyc_Absyn_function_typ(
typvars,_tmpDD,t,_tmpDA,_tmpDB,_tmpDC,_tmpDE,fn_atts),new_atts,((struct Cyc_List_List*)
_check_null(tms))->tl);}_LL77: if(*((int*)_tmpD9)!= 0)goto _LL74;_tmpDF=((struct
Cyc_Absyn_NoTypes_struct*)_tmpD9)->f2;_LL78: {const char*_tmp99C;void*_tmp99B;(
_tmp99B=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(_tmpDF,((_tmp99C="function declaration without parameter types",
_tag_dyneither(_tmp99C,sizeof(char),45))),_tag_dyneither(_tmp99B,sizeof(void*),0)));}
_LL74:;}_LL6E: if(*((int*)_tmpCC)!= 4)goto _LL70;_tmpD3=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpCC)->f1;_tmpD4=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCC)->f2;_LL6F:
if(tms->tl == 0){struct _tuple6 _tmp99D;return(_tmp99D.f1=tq,((_tmp99D.f2=t,((
_tmp99D.f3=_tmpD3,((_tmp99D.f4=atts,_tmp99D)))))));}{const char*_tmp9A0;void*
_tmp99F;(_tmp99F=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD4,((_tmp9A0="type parameters must appear before function arguments in declarator",
_tag_dyneither(_tmp9A0,sizeof(char),68))),_tag_dyneither(_tmp99F,sizeof(void*),0)));}
_LL70: if(*((int*)_tmpCC)!= 2)goto _LL72;_tmpD5=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpCC)->f1;_tmpD6=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCC)->f2;_LL71: {
struct Cyc_Absyn_PointerType_struct _tmp9A6;struct Cyc_Absyn_PtrInfo _tmp9A5;struct
Cyc_Absyn_PointerType_struct*_tmp9A4;return Cyc_Parse_apply_tms(_tmpD6,(void*)((
_tmp9A4=_cycalloc(sizeof(*_tmp9A4)),((_tmp9A4[0]=((_tmp9A6.tag=4,((_tmp9A6.f1=((
_tmp9A5.elt_typ=(void*)t,((_tmp9A5.elt_tq=tq,((_tmp9A5.ptr_atts=_tmpD5,_tmp9A5)))))),
_tmp9A6)))),_tmp9A4)))),atts,tms->tl);}_LL72: if(*((int*)_tmpCC)!= 5)goto _LL67;
_tmpD7=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpCC)->f1;_tmpD8=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpCC)->f2;_LL73: return Cyc_Parse_apply_tms(tq,t,Cyc_List_append(atts,_tmpD8),
tms->tl);_LL67:;}}void*Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*
loc);void*Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*
loc){void*_tmpED;struct Cyc_Core_Opt*_tmpEE;struct _tuple5 _tmpEC=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmpED=_tmpEC.f1;_tmpEE=_tmpEC.f2;if(_tmpEE != 0){const char*_tmp9A9;void*
_tmp9A8;(_tmp9A8=0,Cyc_Tcutil_warn(loc,((_tmp9A9="ignoring nested type declaration(s) in specifier list",
_tag_dyneither(_tmp9A9,sizeof(char),54))),_tag_dyneither(_tmp9A8,sizeof(void*),0)));}
return _tmpED;}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*
loc,struct _tuple7*t);static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment*loc,struct _tuple7*t){struct _tuple7 _tmpF2;struct
_tuple1*_tmpF3;struct Cyc_Absyn_Tqual _tmpF4;void*_tmpF5;struct Cyc_List_List*
_tmpF6;struct Cyc_List_List*_tmpF7;struct _tuple7*_tmpF1=t;_tmpF2=*_tmpF1;_tmpF3=
_tmpF2.f1;_tmpF4=_tmpF2.f2;_tmpF5=_tmpF2.f3;_tmpF6=_tmpF2.f4;_tmpF7=_tmpF2.f5;
Cyc_Lex_register_typedef(_tmpF3);{struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
type;{void*_tmpF8=_tmpF5;struct Cyc_Core_Opt*_tmpF9;_LL7F: if(_tmpF8 <= (void*)4)
goto _LL81;if(*((int*)_tmpF8)!= 0)goto _LL81;_tmpF9=((struct Cyc_Absyn_Evar_struct*)
_tmpF8)->f1;_LL80: type=0;if(_tmpF9 == 0)kind=(struct Cyc_Core_Opt*)& Cyc_Tcutil_bk;
else{kind=_tmpF9;}goto _LL7E;_LL81:;_LL82: kind=0;{struct Cyc_Core_Opt*_tmp9AA;type=((
_tmp9AA=_cycalloc(sizeof(*_tmp9AA)),((_tmp9AA->v=(void*)_tmpF5,_tmp9AA))));}goto
_LL7E;_LL7E:;}{struct Cyc_Absyn_Typedef_d_struct _tmp9B0;struct Cyc_Absyn_Typedefdecl*
_tmp9AF;struct Cyc_Absyn_Typedef_d_struct*_tmp9AE;return Cyc_Absyn_new_decl((void*)((
_tmp9AE=_cycalloc(sizeof(*_tmp9AE)),((_tmp9AE[0]=((_tmp9B0.tag=9,((_tmp9B0.f1=((
_tmp9AF=_cycalloc(sizeof(*_tmp9AF)),((_tmp9AF->name=_tmpF3,((_tmp9AF->tvs=_tmpF6,((
_tmp9AF->kind=kind,((_tmp9AF->defn=type,((_tmp9AF->atts=_tmpF7,((_tmp9AF->tq=
_tmpF4,_tmp9AF)))))))))))))),_tmp9B0)))),_tmp9AE)))),loc);}}}static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*
s){struct Cyc_Absyn_Decl_s_struct _tmp9B3;struct Cyc_Absyn_Decl_s_struct*_tmp9B2;
return Cyc_Absyn_new_stmt((void*)((_tmp9B2=_cycalloc(sizeof(*_tmp9B2)),((_tmp9B2[
0]=((_tmp9B3.tag=11,((_tmp9B3.f1=d,((_tmp9B3.f2=s,_tmp9B3)))))),_tmp9B2)))),d->loc);}
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,
struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc);static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc){struct _RegionHandle _tmp100=_new_region("mkrgn");struct _RegionHandle*mkrgn=&
_tmp100;_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp102;struct Cyc_Absyn_Tqual
_tmp103;struct Cyc_List_List*_tmp104;struct Cyc_List_List*_tmp105;struct Cyc_Parse_Declaration_spec*
_tmp101=ds;_tmp102=*_tmp101;_tmp103=_tmp102.tq;_tmp104=_tmp102.type_specs;
_tmp105=_tmp102.attributes;if(_tmp103.loc == 0)_tmp103.loc=tqual_loc;if(ds->is_inline){
const char*_tmp9B4;Cyc_Parse_err(((_tmp9B4="inline is allowed only on function definitions",
_tag_dyneither(_tmp9B4,sizeof(char),47))),loc);}if(_tmp104 == 0){{const char*
_tmp9B5;Cyc_Parse_err(((_tmp9B5="missing type specifiers in declaration",
_tag_dyneither(_tmp9B5,sizeof(char),39))),loc);}{struct Cyc_List_List*_tmp108=0;
_npop_handler(0);return _tmp108;}}{void*s=(void*)2;int istypedef=0;if(ds->sc != 0){
void*_tmp109=(void*)((struct Cyc_Core_Opt*)_check_null(ds->sc))->v;_LL84: if((int)
_tmp109 != 0)goto _LL86;_LL85: istypedef=1;goto _LL83;_LL86: if((int)_tmp109 != 1)goto
_LL88;_LL87: s=(void*)3;goto _LL83;_LL88: if((int)_tmp109 != 2)goto _LL8A;_LL89: s=(
void*)4;goto _LL83;_LL8A: if((int)_tmp109 != 3)goto _LL8C;_LL8B: s=(void*)0;goto _LL83;
_LL8C: if((int)_tmp109 != 4)goto _LL8E;_LL8D: s=(void*)2;goto _LL83;_LL8E: if((int)
_tmp109 != 5)goto _LL90;_LL8F: if(Cyc_Parse_no_register)s=(void*)2;else{s=(void*)5;}
goto _LL83;_LL90: if((int)_tmp109 != 6)goto _LL83;_LL91: s=(void*)1;goto _LL83;_LL83:;}{
struct Cyc_List_List*_tmp10B;struct Cyc_List_List*_tmp10C;struct _tuple0 _tmp10A=((
struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(mkrgn,mkrgn,ids);_tmp10B=_tmp10A.f1;_tmp10C=_tmp10A.f2;{int
exps_empty=1;{struct Cyc_List_List*es=_tmp10C;for(0;es != 0;es=es->tl){if((struct
Cyc_Absyn_Exp*)es->hd != 0){exps_empty=0;break;}}}{struct _tuple5 _tmp10D=Cyc_Parse_collapse_type_specifiers(
_tmp104,loc);if(_tmp10B == 0){void*_tmp10F;struct Cyc_Core_Opt*_tmp110;struct
_tuple5 _tmp10E=_tmp10D;_tmp10F=_tmp10E.f1;_tmp110=_tmp10E.f2;if(_tmp110 != 0){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp110->v;{void*_tmp111=(void*)d->r;
struct Cyc_Absyn_Enumdecl*_tmp112;struct Cyc_Absyn_Aggrdecl*_tmp113;struct Cyc_Absyn_Tuniondecl*
_tmp114;_LL93: if(_tmp111 <= (void*)2)goto _LL99;if(*((int*)_tmp111)!= 8)goto _LL95;
_tmp112=((struct Cyc_Absyn_Enum_d_struct*)_tmp111)->f1;_LL94:(void*)(_tmp112->sc=(
void*)s);if(_tmp105 != 0){const char*_tmp9B6;Cyc_Parse_err(((_tmp9B6="bad attributes on enum",
_tag_dyneither(_tmp9B6,sizeof(char),23))),loc);}goto _LL92;_LL95: if(*((int*)
_tmp111)!= 6)goto _LL97;_tmp113=((struct Cyc_Absyn_Aggr_d_struct*)_tmp111)->f1;
_LL96:(void*)(_tmp113->sc=(void*)s);_tmp113->attributes=_tmp105;goto _LL92;_LL97:
if(*((int*)_tmp111)!= 7)goto _LL99;_tmp114=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp111)->f1;_LL98:(void*)(_tmp114->sc=(void*)s);if(_tmp105 != 0){const char*
_tmp9B7;Cyc_Parse_err(((_tmp9B7="bad attributes on tunion",_tag_dyneither(
_tmp9B7,sizeof(char),25))),loc);}goto _LL92;_LL99:;_LL9A:{const char*_tmp9B8;Cyc_Parse_err(((
_tmp9B8="bad declaration",_tag_dyneither(_tmp9B8,sizeof(char),16))),loc);}{
struct Cyc_List_List*_tmp118=0;_npop_handler(0);return _tmp118;}_LL92:;}{struct Cyc_List_List*
_tmp9B9;struct Cyc_List_List*_tmp11A=(_tmp9B9=_cycalloc(sizeof(*_tmp9B9)),((
_tmp9B9->hd=d,((_tmp9B9->tl=0,_tmp9B9)))));_npop_handler(0);return _tmp11A;}}
else{void*_tmp11B=_tmp10F;struct Cyc_Absyn_AggrInfo _tmp11C;union Cyc_Absyn_AggrInfoU_union
_tmp11D;void*_tmp11E;struct _tuple1*_tmp11F;struct Cyc_List_List*_tmp120;struct Cyc_Absyn_TunionInfo
_tmp121;union Cyc_Absyn_TunionInfoU_union _tmp122;struct Cyc_Absyn_Tuniondecl**
_tmp123;struct Cyc_Absyn_TunionInfo _tmp124;union Cyc_Absyn_TunionInfoU_union
_tmp125;struct Cyc_Absyn_UnknownTunionInfo _tmp126;struct _tuple1*_tmp127;int
_tmp128;int _tmp129;struct Cyc_List_List*_tmp12A;struct _tuple1*_tmp12B;struct Cyc_List_List*
_tmp12C;_LL9C: if(_tmp11B <= (void*)4)goto _LLA6;if(*((int*)_tmp11B)!= 10)goto _LL9E;
_tmp11C=((struct Cyc_Absyn_AggrType_struct*)_tmp11B)->f1;_tmp11D=_tmp11C.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp11B)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL9E;_tmp11E=(_tmp11D.UnknownAggr).f1;_tmp11F=(_tmp11D.UnknownAggr).f2;
_tmp120=_tmp11C.targs;_LL9D: {struct Cyc_List_List*_tmp12D=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp120);
struct Cyc_Absyn_Aggrdecl*_tmp9BA;struct Cyc_Absyn_Aggrdecl*_tmp12E=(_tmp9BA=
_cycalloc(sizeof(*_tmp9BA)),((_tmp9BA->kind=(void*)_tmp11E,((_tmp9BA->sc=(void*)
s,((_tmp9BA->name=_tmp11F,((_tmp9BA->tvs=_tmp12D,((_tmp9BA->impl=0,((_tmp9BA->attributes=
0,_tmp9BA)))))))))))));if(_tmp105 != 0){const char*_tmp9BB;Cyc_Parse_err(((_tmp9BB="bad attributes on type declaration",
_tag_dyneither(_tmp9BB,sizeof(char),35))),loc);}{struct Cyc_Absyn_Aggr_d_struct*
_tmp9C1;struct Cyc_Absyn_Aggr_d_struct _tmp9C0;struct Cyc_List_List*_tmp9BF;struct
Cyc_List_List*_tmp133=(_tmp9BF=_cycalloc(sizeof(*_tmp9BF)),((_tmp9BF->hd=Cyc_Absyn_new_decl((
void*)((_tmp9C1=_cycalloc(sizeof(*_tmp9C1)),((_tmp9C1[0]=((_tmp9C0.tag=6,((
_tmp9C0.f1=_tmp12E,_tmp9C0)))),_tmp9C1)))),loc),((_tmp9BF->tl=0,_tmp9BF)))));
_npop_handler(0);return _tmp133;}}_LL9E: if(*((int*)_tmp11B)!= 2)goto _LLA0;_tmp121=((
struct Cyc_Absyn_TunionType_struct*)_tmp11B)->f1;_tmp122=_tmp121.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp11B)->f1).tunion_info).KnownTunion).tag != 
1)goto _LLA0;_tmp123=(_tmp122.KnownTunion).f1;_LL9F: if(_tmp105 != 0){const char*
_tmp9C2;Cyc_Parse_err(((_tmp9C2="bad attributes on tunion",_tag_dyneither(
_tmp9C2,sizeof(char),25))),loc);}{struct Cyc_Absyn_Tunion_d_struct*_tmp9C8;struct
Cyc_Absyn_Tunion_d_struct _tmp9C7;struct Cyc_List_List*_tmp9C6;struct Cyc_List_List*
_tmp139=(_tmp9C6=_cycalloc(sizeof(*_tmp9C6)),((_tmp9C6->hd=Cyc_Absyn_new_decl((
void*)((_tmp9C8=_cycalloc(sizeof(*_tmp9C8)),((_tmp9C8[0]=((_tmp9C7.tag=7,((
_tmp9C7.f1=*_tmp123,_tmp9C7)))),_tmp9C8)))),loc),((_tmp9C6->tl=0,_tmp9C6)))));
_npop_handler(0);return _tmp139;}_LLA0: if(*((int*)_tmp11B)!= 2)goto _LLA2;_tmp124=((
struct Cyc_Absyn_TunionType_struct*)_tmp11B)->f1;_tmp125=_tmp124.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp11B)->f1).tunion_info).UnknownTunion).tag
!= 0)goto _LLA2;_tmp126=(_tmp125.UnknownTunion).f1;_tmp127=_tmp126.name;_tmp128=
_tmp126.is_xtunion;_tmp129=_tmp126.is_flat;_tmp12A=_tmp124.targs;_LLA1: {struct
Cyc_List_List*_tmp13A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp12A);struct Cyc_Absyn_Decl*_tmp13B=
Cyc_Absyn_tunion_decl(s,_tmp127,_tmp13A,0,_tmp128,_tmp129,loc);if(_tmp105 != 0){
const char*_tmp9C9;Cyc_Parse_err(((_tmp9C9="bad attributes on tunion",
_tag_dyneither(_tmp9C9,sizeof(char),25))),loc);}{struct Cyc_List_List*_tmp9CA;
struct Cyc_List_List*_tmp13E=(_tmp9CA=_cycalloc(sizeof(*_tmp9CA)),((_tmp9CA->hd=
_tmp13B,((_tmp9CA->tl=0,_tmp9CA)))));_npop_handler(0);return _tmp13E;}}_LLA2: if(*((
int*)_tmp11B)!= 12)goto _LLA4;_tmp12B=((struct Cyc_Absyn_EnumType_struct*)_tmp11B)->f1;
_LLA3: {struct Cyc_Absyn_Enumdecl*_tmp9CB;struct Cyc_Absyn_Enumdecl*_tmp13F=(
_tmp9CB=_cycalloc(sizeof(*_tmp9CB)),((_tmp9CB->sc=(void*)s,((_tmp9CB->name=
_tmp12B,((_tmp9CB->fields=0,_tmp9CB)))))));if(_tmp105 != 0){const char*_tmp9CC;Cyc_Parse_err(((
_tmp9CC="bad attributes on enum",_tag_dyneither(_tmp9CC,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp9D6;struct Cyc_Absyn_Enum_d_struct _tmp9D5;struct Cyc_Absyn_Enum_d_struct*
_tmp9D4;struct Cyc_List_List*_tmp9D3;struct Cyc_List_List*_tmp145=(_tmp9D3=
_cycalloc(sizeof(*_tmp9D3)),((_tmp9D3->hd=((_tmp9D6=_cycalloc(sizeof(*_tmp9D6)),((
_tmp9D6->r=(void*)((void*)((_tmp9D4=_cycalloc(sizeof(*_tmp9D4)),((_tmp9D4[0]=((
_tmp9D5.tag=8,((_tmp9D5.f1=_tmp13F,_tmp9D5)))),_tmp9D4))))),((_tmp9D6->loc=loc,
_tmp9D6)))))),((_tmp9D3->tl=0,_tmp9D3)))));_npop_handler(0);return _tmp145;}}
_LLA4: if(*((int*)_tmp11B)!= 13)goto _LLA6;_tmp12C=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp11B)->f1;_LLA5: {struct Cyc_Core_Opt*_tmp9D9;struct Cyc_Absyn_Enumdecl*_tmp9D8;
struct Cyc_Absyn_Enumdecl*_tmp147=(_tmp9D8=_cycalloc(sizeof(*_tmp9D8)),((_tmp9D8->sc=(
void*)s,((_tmp9D8->name=Cyc_Parse_gensym_enum(),((_tmp9D8->fields=((_tmp9D9=
_cycalloc(sizeof(*_tmp9D9)),((_tmp9D9->v=_tmp12C,_tmp9D9)))),_tmp9D8)))))));if(
_tmp105 != 0){const char*_tmp9DA;Cyc_Parse_err(((_tmp9DA="bad attributes on enum",
_tag_dyneither(_tmp9DA,sizeof(char),23))),loc);}{struct Cyc_Absyn_Decl*_tmp9E4;
struct Cyc_Absyn_Enum_d_struct _tmp9E3;struct Cyc_Absyn_Enum_d_struct*_tmp9E2;
struct Cyc_List_List*_tmp9E1;struct Cyc_List_List*_tmp14D=(_tmp9E1=_cycalloc(
sizeof(*_tmp9E1)),((_tmp9E1->hd=((_tmp9E4=_cycalloc(sizeof(*_tmp9E4)),((_tmp9E4->r=(
void*)((void*)((_tmp9E2=_cycalloc(sizeof(*_tmp9E2)),((_tmp9E2[0]=((_tmp9E3.tag=8,((
_tmp9E3.f1=_tmp147,_tmp9E3)))),_tmp9E2))))),((_tmp9E4->loc=loc,_tmp9E4)))))),((
_tmp9E1->tl=0,_tmp9E1)))));_npop_handler(0);return _tmp14D;}}_LLA6:;_LLA7:{const
char*_tmp9E5;Cyc_Parse_err(((_tmp9E5="missing declarator",_tag_dyneither(_tmp9E5,
sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp151=0;_npop_handler(0);return
_tmp151;}_LL9B:;}}else{void*t=_tmp10D.f1;struct Cyc_List_List*_tmp152=Cyc_Parse_apply_tmss(
mkrgn,_tmp103,t,_tmp10B,_tmp105);if(istypedef){if(!exps_empty){const char*_tmp9E6;
Cyc_Parse_err(((_tmp9E6="initializer in typedef declaration",_tag_dyneither(
_tmp9E6,sizeof(char),35))),loc);}{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple7*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp152);if(_tmp10D.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(
_tmp10D.f2)->v;{void*_tmp154=(void*)d->r;struct Cyc_Absyn_Aggrdecl*_tmp155;struct
Cyc_Absyn_Tuniondecl*_tmp156;struct Cyc_Absyn_Enumdecl*_tmp157;_LLA9: if(_tmp154 <= (
void*)2)goto _LLAF;if(*((int*)_tmp154)!= 6)goto _LLAB;_tmp155=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp154)->f1;_LLAA:(void*)(_tmp155->sc=(void*)s);_tmp155->attributes=_tmp105;
_tmp105=0;goto _LLA8;_LLAB: if(*((int*)_tmp154)!= 7)goto _LLAD;_tmp156=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp154)->f1;_LLAC:(void*)(_tmp156->sc=(void*)s);goto _LLA8;_LLAD: if(*((int*)
_tmp154)!= 8)goto _LLAF;_tmp157=((struct Cyc_Absyn_Enum_d_struct*)_tmp154)->f1;
_LLAE:(void*)(_tmp157->sc=(void*)s);goto _LLA8;_LLAF:;_LLB0:{const char*_tmp9E7;
Cyc_Parse_err(((_tmp9E7="declaration within typedef is not a struct, union, tunion, or xtunion",
_tag_dyneither(_tmp9E7,sizeof(char),70))),loc);}goto _LLA8;_LLA8:;}{struct Cyc_List_List*
_tmp9E8;decls=((_tmp9E8=_cycalloc(sizeof(*_tmp9E8)),((_tmp9E8->hd=d,((_tmp9E8->tl=
decls,_tmp9E8))))));}}{struct Cyc_List_List*_tmp15A=decls;_npop_handler(0);return
_tmp15A;}}}else{if(_tmp10D.f2 != 0){const char*_tmp9E9;Cyc_Parse_unimp(((_tmp9E9="nested type declaration within declarator",
_tag_dyneither(_tmp9E9,sizeof(char),42))),loc);}{struct Cyc_List_List*decls=0;{
struct Cyc_List_List*_tmp15C=_tmp152;for(0;_tmp15C != 0;(_tmp15C=_tmp15C->tl,
_tmp10C=_tmp10C->tl)){struct _tuple7 _tmp15E;struct _tuple1*_tmp15F;struct Cyc_Absyn_Tqual
_tmp160;void*_tmp161;struct Cyc_List_List*_tmp162;struct Cyc_List_List*_tmp163;
struct _tuple7*_tmp15D=(struct _tuple7*)_tmp15C->hd;_tmp15E=*_tmp15D;_tmp15F=
_tmp15E.f1;_tmp160=_tmp15E.f2;_tmp161=_tmp15E.f3;_tmp162=_tmp15E.f4;_tmp163=
_tmp15E.f5;if(_tmp162 != 0){const char*_tmp9EC;void*_tmp9EB;(_tmp9EB=0,Cyc_Tcutil_warn(
loc,((_tmp9EC="bad type params, ignoring",_tag_dyneither(_tmp9EC,sizeof(char),26))),
_tag_dyneither(_tmp9EB,sizeof(void*),0)));}if(_tmp10C == 0){const char*_tmp9EF;
void*_tmp9EE;(_tmp9EE=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp9EF="unexpected NULL in parse!",
_tag_dyneither(_tmp9EF,sizeof(char),26))),_tag_dyneither(_tmp9EE,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp168=(struct Cyc_Absyn_Exp*)_tmp10C->hd;struct Cyc_Absyn_Vardecl*
_tmp169=Cyc_Absyn_new_vardecl(_tmp15F,_tmp161,_tmp168);_tmp169->tq=_tmp160;(void*)(
_tmp169->sc=(void*)s);_tmp169->attributes=_tmp163;{struct Cyc_Absyn_Var_d_struct*
_tmp9F5;struct Cyc_Absyn_Var_d_struct _tmp9F4;struct Cyc_Absyn_Decl*_tmp9F3;struct
Cyc_Absyn_Decl*_tmp16A=(_tmp9F3=_cycalloc(sizeof(*_tmp9F3)),((_tmp9F3->r=(void*)((
void*)((_tmp9F5=_cycalloc(sizeof(*_tmp9F5)),((_tmp9F5[0]=((_tmp9F4.tag=0,((
_tmp9F4.f1=_tmp169,_tmp9F4)))),_tmp9F5))))),((_tmp9F3->loc=loc,_tmp9F3)))));
struct Cyc_List_List*_tmp9F6;decls=((_tmp9F6=_cycalloc(sizeof(*_tmp9F6)),((
_tmp9F6->hd=_tmp16A,((_tmp9F6->tl=decls,_tmp9F6))))));}}}}{struct Cyc_List_List*
_tmp16F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);
_npop_handler(0);return _tmp16F;}}}}}}}}};_pop_region(mkrgn);}static void*Cyc_Parse_id_to_kind(
struct _dyneither_ptr s,struct Cyc_Position_Segment*loc);static void*Cyc_Parse_id_to_kind(
struct _dyneither_ptr s,struct Cyc_Position_Segment*loc){if(Cyc_strlen((struct
_dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2)switch(*((const
char*)_check_dyneither_subscript(s,sizeof(char),0))){case 'A': _LLB1: return(void*)
0;case 'M': _LLB2: return(void*)1;case 'B': _LLB3: return(void*)2;case 'R': _LLB4: return(
void*)3;case 'U': _LLB5: if(*((const char*)_check_dyneither_subscript(s,sizeof(char),
1))== 'R')return(void*)4;else{break;}case 'T': _LLB6: if(*((const char*)
_check_dyneither_subscript(s,sizeof(char),1))== 'R')return(void*)5;else{break;}
case 'E': _LLB7: return(void*)6;case 'I': _LLB8: return(void*)7;default: _LLB9: break;}{
const char*_tmp9FB;void*_tmp9FA[2];struct Cyc_String_pa_struct _tmp9F9;struct Cyc_Int_pa_struct
_tmp9F8;Cyc_Parse_err((struct _dyneither_ptr)((_tmp9F8.tag=1,((_tmp9F8.f1=(
unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp9F9.tag=0,((
_tmp9F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp9FA[0]=& _tmp9F9,((
_tmp9FA[1]=& _tmp9F8,Cyc_aprintf(((_tmp9FB="bad kind: %s; strlen=%d",
_tag_dyneither(_tmp9FB,sizeof(char),24))),_tag_dyneither(_tmp9FA,sizeof(void*),2)))))))))))))),
loc);}return(void*)2;}static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*
p);static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*
_tmp174=(void*)p->r;struct _tuple1*_tmp175;struct Cyc_Absyn_Vardecl*_tmp176;struct
Cyc_Absyn_Pat*_tmp177;struct Cyc_Absyn_Pat _tmp178;void*_tmp179;struct Cyc_Absyn_Pat*
_tmp17A;void*_tmp17B;int _tmp17C;char _tmp17D;struct _dyneither_ptr _tmp17E;struct
_tuple1*_tmp17F;struct Cyc_List_List*_tmp180;int _tmp181;struct Cyc_Absyn_Exp*
_tmp182;_LLBC: if(_tmp174 <= (void*)2)goto _LLC2;if(*((int*)_tmp174)!= 12)goto _LLBE;
_tmp175=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp174)->f1;_LLBD: {struct Cyc_Absyn_UnknownId_e_struct
_tmp9FE;struct Cyc_Absyn_UnknownId_e_struct*_tmp9FD;return Cyc_Absyn_new_exp((void*)((
_tmp9FD=_cycalloc(sizeof(*_tmp9FD)),((_tmp9FD[0]=((_tmp9FE.tag=2,((_tmp9FE.f1=
_tmp175,_tmp9FE)))),_tmp9FD)))),p->loc);}_LLBE: if(*((int*)_tmp174)!= 1)goto _LLC0;
_tmp176=((struct Cyc_Absyn_Reference_p_struct*)_tmp174)->f1;_tmp177=((struct Cyc_Absyn_Reference_p_struct*)
_tmp174)->f2;_tmp178=*_tmp177;_tmp179=(void*)_tmp178.r;if((int)_tmp179 != 0)goto
_LLC0;_LLBF: {struct Cyc_Absyn_UnknownId_e_struct _tmpA01;struct Cyc_Absyn_UnknownId_e_struct*
_tmpA00;return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)((_tmpA00=_cycalloc(
sizeof(*_tmpA00)),((_tmpA00[0]=((_tmpA01.tag=2,((_tmpA01.f1=_tmp176->name,
_tmpA01)))),_tmpA00)))),p->loc),p->loc);}_LLC0: if(*((int*)_tmp174)!= 4)goto _LLC2;
_tmp17A=((struct Cyc_Absyn_Pointer_p_struct*)_tmp174)->f1;_LLC1: return Cyc_Absyn_address_exp(
Cyc_Parse_pat2exp(_tmp17A),p->loc);_LLC2: if((int)_tmp174 != 1)goto _LLC4;_LLC3:
return Cyc_Absyn_null_exp(p->loc);_LLC4: if(_tmp174 <= (void*)2)goto _LLCE;if(*((int*)
_tmp174)!= 7)goto _LLC6;_tmp17B=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp174)->f1;
_tmp17C=((struct Cyc_Absyn_Int_p_struct*)_tmp174)->f2;_LLC5: return Cyc_Absyn_int_exp(
_tmp17B,_tmp17C,p->loc);_LLC6: if(*((int*)_tmp174)!= 8)goto _LLC8;_tmp17D=((struct
Cyc_Absyn_Char_p_struct*)_tmp174)->f1;_LLC7: return Cyc_Absyn_char_exp(_tmp17D,p->loc);
_LLC8: if(*((int*)_tmp174)!= 9)goto _LLCA;_tmp17E=((struct Cyc_Absyn_Float_p_struct*)
_tmp174)->f1;_LLC9: return Cyc_Absyn_float_exp(_tmp17E,p->loc);_LLCA: if(*((int*)
_tmp174)!= 13)goto _LLCC;_tmp17F=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp174)->f1;
_tmp180=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp174)->f2;_tmp181=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp174)->f3;if(_tmp181 != 0)goto _LLCC;_LLCB: {struct Cyc_Absyn_UnknownId_e_struct
_tmpA04;struct Cyc_Absyn_UnknownId_e_struct*_tmpA03;struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((
void*)((_tmpA03=_cycalloc(sizeof(*_tmpA03)),((_tmpA03[0]=((_tmpA04.tag=2,((
_tmpA04.f1=_tmp17F,_tmpA04)))),_tmpA03)))),p->loc);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp180);return Cyc_Absyn_unknowncall_exp(e1,
es,p->loc);}_LLCC: if(*((int*)_tmp174)!= 14)goto _LLCE;_tmp182=((struct Cyc_Absyn_Exp_p_struct*)
_tmp174)->f1;_LLCD: return _tmp182;_LLCE:;_LLCF:{const char*_tmpA05;Cyc_Parse_err(((
_tmpA05="cannot mix patterns and expressions in case",_tag_dyneither(_tmpA05,
sizeof(char),44))),p->loc);}return Cyc_Absyn_null_exp(p->loc);_LLBB:;}struct
_tuple11{void*f1;int f2;};struct Cyc_Int_tok_struct{int tag;struct _tuple11 f1;};
struct Cyc_Char_tok_struct{int tag;char f1;};struct Cyc_String_tok_struct{int tag;
struct _dyneither_ptr f1;};struct Cyc_Stringopt_tok_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_QualId_tok_struct{int tag;struct _tuple1*f1;};struct _tuple12{struct
Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Absyn_Conref*f3;};
struct Cyc_YY1_struct{int tag;struct _tuple12*f1;};struct Cyc_YY2_struct{int tag;
struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int tag;struct Cyc_Absyn_Exp*f1;}
;struct Cyc_YY4_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_YY5_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY7_struct{int tag;void*f1;};struct Cyc_YY8_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_YY9_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_YY10_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY11_struct{int tag;struct Cyc_Absyn_Pat*
f1;};struct _tuple13{struct Cyc_List_List*f1;int f2;};struct Cyc_YY12_struct{int tag;
struct _tuple13*f1;};struct Cyc_YY13_struct{int tag;struct Cyc_List_List*f1;};struct
_tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_YY14_struct{
int tag;struct _tuple14*f1;};struct Cyc_YY15_struct{int tag;struct Cyc_List_List*f1;}
;struct Cyc_YY16_struct{int tag;struct _tuple13*f1;};struct Cyc_YY17_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_YY18_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY19_struct{int tag;struct Cyc_Parse_Declaration_spec*f1;};struct
_tuple15{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_YY20_struct{
int tag;struct _tuple15*f1;};struct Cyc_YY21_struct{int tag;struct Cyc_List_List*f1;}
;struct Cyc_YY22_struct{int tag;void*f1;};struct Cyc_YY23_struct{int tag;void*f1;};
struct Cyc_YY24_struct{int tag;void*f1;};struct Cyc_YY25_struct{int tag;struct Cyc_Absyn_Tqual
f1;};struct Cyc_YY26_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY27_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY28_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY29_struct{int tag;struct Cyc_Parse_Declarator*f1;};struct Cyc_YY30_struct{
int tag;struct Cyc_Parse_Abstractdeclarator*f1;};struct Cyc_YY31_struct{int tag;int
f1;};struct Cyc_YY32_struct{int tag;void*f1;};struct Cyc_YY33_struct{int tag;struct
Cyc_Absyn_Tunionfield*f1;};struct Cyc_YY34_struct{int tag;struct Cyc_List_List*f1;}
;struct _tuple16{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct Cyc_YY35_struct{int tag;struct _tuple16*f1;};struct Cyc_YY36_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY37_struct{int tag;struct _tuple2*f1;};
struct Cyc_YY38_struct{int tag;struct Cyc_List_List*f1;};struct _tuple17{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};struct Cyc_YY39_struct{int tag;struct _tuple17*f1;};struct Cyc_YY40_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY41_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY42_struct{int tag;void*f1;};struct Cyc_YY43_struct{int tag;void*f1;};
struct Cyc_YY44_struct{int tag;void*f1;};struct Cyc_YY45_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY46_struct{int tag;void*f1;};struct Cyc_YY47_struct{int tag;struct
Cyc_Absyn_Enumfield*f1;};struct Cyc_YY48_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY49_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY50_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY51_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_YY52_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YYINITIALSVAL_struct{
int tag;};union Cyc_YYSTYPE_union{struct Cyc_Int_tok_struct Int_tok;struct Cyc_Char_tok_struct
Char_tok;struct Cyc_String_tok_struct String_tok;struct Cyc_Stringopt_tok_struct
Stringopt_tok;struct Cyc_QualId_tok_struct QualId_tok;struct Cyc_YY1_struct YY1;
struct Cyc_YY2_struct YY2;struct Cyc_YY3_struct YY3;struct Cyc_YY4_struct YY4;struct
Cyc_YY5_struct YY5;struct Cyc_YY6_struct YY6;struct Cyc_YY7_struct YY7;struct Cyc_YY8_struct
YY8;struct Cyc_YY9_struct YY9;struct Cyc_YY10_struct YY10;struct Cyc_YY11_struct YY11;
struct Cyc_YY12_struct YY12;struct Cyc_YY13_struct YY13;struct Cyc_YY14_struct YY14;
struct Cyc_YY15_struct YY15;struct Cyc_YY16_struct YY16;struct Cyc_YY17_struct YY17;
struct Cyc_YY18_struct YY18;struct Cyc_YY19_struct YY19;struct Cyc_YY20_struct YY20;
struct Cyc_YY21_struct YY21;struct Cyc_YY22_struct YY22;struct Cyc_YY23_struct YY23;
struct Cyc_YY24_struct YY24;struct Cyc_YY25_struct YY25;struct Cyc_YY26_struct YY26;
struct Cyc_YY27_struct YY27;struct Cyc_YY28_struct YY28;struct Cyc_YY29_struct YY29;
struct Cyc_YY30_struct YY30;struct Cyc_YY31_struct YY31;struct Cyc_YY32_struct YY32;
struct Cyc_YY33_struct YY33;struct Cyc_YY34_struct YY34;struct Cyc_YY35_struct YY35;
struct Cyc_YY36_struct YY36;struct Cyc_YY37_struct YY37;struct Cyc_YY38_struct YY38;
struct Cyc_YY39_struct YY39;struct Cyc_YY40_struct YY40;struct Cyc_YY41_struct YY41;
struct Cyc_YY42_struct YY42;struct Cyc_YY43_struct YY43;struct Cyc_YY44_struct YY44;
struct Cyc_YY45_struct YY45;struct Cyc_YY46_struct YY46;struct Cyc_YY47_struct YY47;
struct Cyc_YY48_struct YY48;struct Cyc_YY49_struct YY49;struct Cyc_YY50_struct YY50;
struct Cyc_YY51_struct YY51;struct Cyc_YY52_struct YY52;struct Cyc_YYINITIALSVAL_struct
YYINITIALSVAL;};static char _tmp18B[14]="$(sign_t,int)";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp18B,_tmp18B,_tmp18B + 14}};struct
_tuple11 Cyc_yyget_Int_tok(union Cyc_YYSTYPE_union yy1);struct _tuple11 Cyc_yyget_Int_tok(
union Cyc_YYSTYPE_union yy1){struct _tuple11 yyzzz;{union Cyc_YYSTYPE_union _tmp18C=
yy1;struct _tuple11 _tmp18D;_LLD1: if((_tmp18C.Int_tok).tag != 0)goto _LLD3;_tmp18D=(
_tmp18C.Int_tok).f1;_LLD2: yyzzz=_tmp18D;goto _LLD0;_LLD3:;_LLD4:(int)_throw((void*)&
Cyc_yyfail_Int_tok);_LLD0:;}return yyzzz;}static char _tmp18F[5]="char";static
struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp18F,
_tmp18F,_tmp18F + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE_union yy1);char Cyc_yyget_Char_tok(
union Cyc_YYSTYPE_union yy1){char yyzzz;{union Cyc_YYSTYPE_union _tmp190=yy1;char
_tmp191;_LLD6: if((_tmp190.Char_tok).tag != 1)goto _LLD8;_tmp191=(_tmp190.Char_tok).f1;
_LLD7: yyzzz=_tmp191;goto _LLD5;_LLD8:;_LLD9:(int)_throw((void*)& Cyc_yyfail_Char_tok);
_LLD5:;}return yyzzz;}static char _tmp193[9]="string_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp193,_tmp193,_tmp193 + 9}};struct
_dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE_union yy1);struct
_dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE_union yy1){struct
_dyneither_ptr yyzzz;{union Cyc_YYSTYPE_union _tmp194=yy1;struct _dyneither_ptr
_tmp195;_LLDB: if((_tmp194.String_tok).tag != 2)goto _LLDD;_tmp195=(_tmp194.String_tok).f1;
_LLDC: yyzzz=_tmp195;goto _LLDA;_LLDD:;_LLDE:(int)_throw((void*)& Cyc_yyfail_String_tok);
_LLDA:;}return yyzzz;}static char _tmp197[54]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp197,
_tmp197,_tmp197 + 54}};struct _tuple12*Cyc_yyget_YY1(union Cyc_YYSTYPE_union yy1);
struct _tuple12*Cyc_yyget_YY1(union Cyc_YYSTYPE_union yy1){struct _tuple12*yyzzz;{
union Cyc_YYSTYPE_union _tmp198=yy1;struct _tuple12*_tmp199;_LLE0: if((_tmp198.YY1).tag
!= 5)goto _LLE2;_tmp199=(_tmp198.YY1).f1;_LLE1: yyzzz=_tmp199;goto _LLDF;_LLE2:;
_LLE3:(int)_throw((void*)& Cyc_yyfail_YY1);_LLDF:;}return yyzzz;}static char _tmp19B[
19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{
_tmp19B,_tmp19B,_tmp19B + 19}};struct Cyc_Absyn_Conref*Cyc_yyget_YY2(union Cyc_YYSTYPE_union
yy1);struct Cyc_Absyn_Conref*Cyc_yyget_YY2(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Conref*
yyzzz;{union Cyc_YYSTYPE_union _tmp19C=yy1;struct Cyc_Absyn_Conref*_tmp19D;_LLE5:
if((_tmp19C.YY2).tag != 6)goto _LLE7;_tmp19D=(_tmp19C.YY2).f1;_LLE6: yyzzz=_tmp19D;
goto _LLE4;_LLE7:;_LLE8:(int)_throw((void*)& Cyc_yyfail_YY2);_LLE4:;}return yyzzz;}
static char _tmp19F[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={
Cyc_Core_Failure,{_tmp19F,_tmp19F,_tmp19F + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(
union Cyc_YYSTYPE_union yy1);struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union Cyc_YYSTYPE_union
yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE_union _tmp1A0=yy1;struct Cyc_Absyn_Exp*
_tmp1A1;_LLEA: if((_tmp1A0.YY3).tag != 7)goto _LLEC;_tmp1A1=(_tmp1A0.YY3).f1;_LLEB:
yyzzz=_tmp1A1;goto _LLE9;_LLEC:;_LLED:(int)_throw((void*)& Cyc_yyfail_YY3);_LLE9:;}
return yyzzz;}static char _tmp1A3[10]="exp_opt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1A3,_tmp1A3,_tmp1A3 + 10}};struct Cyc_Absyn_Exp*
Cyc_yyget_YY4(union Cyc_YYSTYPE_union yy1);struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union
Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE_union _tmp1A4=
yy1;struct Cyc_Absyn_Exp*_tmp1A5;_LLEF: if((_tmp1A4.YY4).tag != 8)goto _LLF1;_tmp1A5=(
_tmp1A4.YY4).f1;_LLF0: yyzzz=_tmp1A5;goto _LLEE;_LLF1:;_LLF2:(int)_throw((void*)&
Cyc_yyfail_YY4);_LLEE:;}return yyzzz;}static char _tmp1A7[14]="list_t<exp_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1A7,
_tmp1A7,_tmp1A7 + 14}};struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE_union
yy1);struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp1A8=yy1;struct Cyc_List_List*_tmp1A9;_LLF4: if((
_tmp1A8.YY5).tag != 9)goto _LLF6;_tmp1A9=(_tmp1A8.YY5).f1;_LLF5: yyzzz=_tmp1A9;goto
_LLF3;_LLF6:;_LLF7:(int)_throw((void*)& Cyc_yyfail_YY5);_LLF3:;}return yyzzz;}
static char _tmp1AB[39]="list_t<$(list_t<designator_t>,exp_t)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1AB,_tmp1AB,_tmp1AB + 39}};struct Cyc_List_List*
Cyc_yyget_YY6(union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY6(union
Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1AC=
yy1;struct Cyc_List_List*_tmp1AD;_LLF9: if((_tmp1AC.YY6).tag != 10)goto _LLFB;
_tmp1AD=(_tmp1AC.YY6).f1;_LLFA: yyzzz=_tmp1AD;goto _LLF8;_LLFB:;_LLFC:(int)_throw((
void*)& Cyc_yyfail_YY6);_LLF8:;}return yyzzz;}static char _tmp1AF[9]="primop_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1AF,
_tmp1AF,_tmp1AF + 9}};void*Cyc_yyget_YY7(union Cyc_YYSTYPE_union yy1);void*Cyc_yyget_YY7(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1B0=yy1;void*
_tmp1B1;_LLFE: if((_tmp1B0.YY7).tag != 11)goto _LL100;_tmp1B1=(_tmp1B0.YY7).f1;
_LLFF: yyzzz=_tmp1B1;goto _LLFD;_LL100:;_LL101:(int)_throw((void*)& Cyc_yyfail_YY7);
_LLFD:;}return yyzzz;}static char _tmp1B3[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp1B3,_tmp1B3,_tmp1B3 + 16}};struct Cyc_Core_Opt*
Cyc_yyget_YY8(union Cyc_YYSTYPE_union yy1);struct Cyc_Core_Opt*Cyc_yyget_YY8(union
Cyc_YYSTYPE_union yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE_union _tmp1B4=
yy1;struct Cyc_Core_Opt*_tmp1B5;_LL103: if((_tmp1B4.YY8).tag != 12)goto _LL105;
_tmp1B5=(_tmp1B4.YY8).f1;_LL104: yyzzz=_tmp1B5;goto _LL102;_LL105:;_LL106:(int)
_throw((void*)& Cyc_yyfail_YY8);_LL102:;}return yyzzz;}static char _tmp1B7[7]="qvar_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{
_tmp1B7,_tmp1B7,_tmp1B7 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE_union
yy1);struct _tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE_union yy1){struct _tuple1*
yyzzz;{union Cyc_YYSTYPE_union _tmp1B8=yy1;struct _tuple1*_tmp1B9;_LL108: if((
_tmp1B8.QualId_tok).tag != 4)goto _LL10A;_tmp1B9=(_tmp1B8.QualId_tok).f1;_LL109:
yyzzz=_tmp1B9;goto _LL107;_LL10A:;_LL10B:(int)_throw((void*)& Cyc_yyfail_QualId_tok);
_LL107:;}return yyzzz;}static char _tmp1BB[7]="stmt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp1BB,_tmp1BB,_tmp1BB + 7}};struct Cyc_Absyn_Stmt*
Cyc_yyget_YY9(union Cyc_YYSTYPE_union yy1);struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE_union
_tmp1BC=yy1;struct Cyc_Absyn_Stmt*_tmp1BD;_LL10D: if((_tmp1BC.YY9).tag != 13)goto
_LL10F;_tmp1BD=(_tmp1BC.YY9).f1;_LL10E: yyzzz=_tmp1BD;goto _LL10C;_LL10F:;_LL110:(
int)_throw((void*)& Cyc_yyfail_YY9);_LL10C:;}return yyzzz;}static char _tmp1BF[24]="list_t<switch_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp1BF,
_tmp1BF,_tmp1BF + 24}};struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE_union
yy1);struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp1C0=yy1;struct Cyc_List_List*_tmp1C1;_LL112: if((
_tmp1C0.YY10).tag != 14)goto _LL114;_tmp1C1=(_tmp1C0.YY10).f1;_LL113: yyzzz=_tmp1C1;
goto _LL111;_LL114:;_LL115:(int)_throw((void*)& Cyc_yyfail_YY10);_LL111:;}return
yyzzz;}static char _tmp1C3[6]="pat_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY11={
Cyc_Core_Failure,{_tmp1C3,_tmp1C3,_tmp1C3 + 6}};struct Cyc_Absyn_Pat*Cyc_yyget_YY11(
union Cyc_YYSTYPE_union yy1);struct Cyc_Absyn_Pat*Cyc_yyget_YY11(union Cyc_YYSTYPE_union
yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE_union _tmp1C4=yy1;struct Cyc_Absyn_Pat*
_tmp1C5;_LL117: if((_tmp1C4.YY11).tag != 15)goto _LL119;_tmp1C5=(_tmp1C4.YY11).f1;
_LL118: yyzzz=_tmp1C5;goto _LL116;_LL119:;_LL11A:(int)_throw((void*)& Cyc_yyfail_YY11);
_LL116:;}return yyzzz;}static char _tmp1C7[23]="$(list_t<pat_t>,bool)@";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp1C7,_tmp1C7,
_tmp1C7 + 23}};struct _tuple13*Cyc_yyget_YY12(union Cyc_YYSTYPE_union yy1);struct
_tuple13*Cyc_yyget_YY12(union Cyc_YYSTYPE_union yy1){struct _tuple13*yyzzz;{union
Cyc_YYSTYPE_union _tmp1C8=yy1;struct _tuple13*_tmp1C9;_LL11C: if((_tmp1C8.YY12).tag
!= 16)goto _LL11E;_tmp1C9=(_tmp1C8.YY12).f1;_LL11D: yyzzz=_tmp1C9;goto _LL11B;
_LL11E:;_LL11F:(int)_throw((void*)& Cyc_yyfail_YY12);_LL11B:;}return yyzzz;}static
char _tmp1CB[14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY13={
Cyc_Core_Failure,{_tmp1CB,_tmp1CB,_tmp1CB + 14}};struct Cyc_List_List*Cyc_yyget_YY13(
union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1CC=yy1;struct Cyc_List_List*
_tmp1CD;_LL121: if((_tmp1CC.YY13).tag != 17)goto _LL123;_tmp1CD=(_tmp1CC.YY13).f1;
_LL122: yyzzz=_tmp1CD;goto _LL120;_LL123:;_LL124:(int)_throw((void*)& Cyc_yyfail_YY13);
_LL120:;}return yyzzz;}static char _tmp1CF[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp1CF,
_tmp1CF,_tmp1CF + 31}};struct _tuple14*Cyc_yyget_YY14(union Cyc_YYSTYPE_union yy1);
struct _tuple14*Cyc_yyget_YY14(union Cyc_YYSTYPE_union yy1){struct _tuple14*yyzzz;{
union Cyc_YYSTYPE_union _tmp1D0=yy1;struct _tuple14*_tmp1D1;_LL126: if((_tmp1D0.YY14).tag
!= 18)goto _LL128;_tmp1D1=(_tmp1D0.YY14).f1;_LL127: yyzzz=_tmp1D1;goto _LL125;
_LL128:;_LL129:(int)_throw((void*)& Cyc_yyfail_YY14);_LL125:;}return yyzzz;}static
char _tmp1D3[39]="list_t<$(list_t<designator_t>,pat_t)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp1D3,_tmp1D3,_tmp1D3 + 39}};struct Cyc_List_List*
Cyc_yyget_YY15(union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY15(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp1D4=yy1;struct Cyc_List_List*_tmp1D5;_LL12B: if((_tmp1D4.YY15).tag != 19)goto
_LL12D;_tmp1D5=(_tmp1D4.YY15).f1;_LL12C: yyzzz=_tmp1D5;goto _LL12A;_LL12D:;_LL12E:(
int)_throw((void*)& Cyc_yyfail_YY15);_LL12A:;}return yyzzz;}static char _tmp1D7[48]="$(list_t<$(list_t<designator_t>,pat_t)@>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp1D7,
_tmp1D7,_tmp1D7 + 48}};struct _tuple13*Cyc_yyget_YY16(union Cyc_YYSTYPE_union yy1);
struct _tuple13*Cyc_yyget_YY16(union Cyc_YYSTYPE_union yy1){struct _tuple13*yyzzz;{
union Cyc_YYSTYPE_union _tmp1D8=yy1;struct _tuple13*_tmp1D9;_LL130: if((_tmp1D8.YY16).tag
!= 20)goto _LL132;_tmp1D9=(_tmp1D8.YY16).f1;_LL131: yyzzz=_tmp1D9;goto _LL12F;
_LL132:;_LL133:(int)_throw((void*)& Cyc_yyfail_YY16);_LL12F:;}return yyzzz;}static
char _tmp1DB[9]="fndecl_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY17={
Cyc_Core_Failure,{_tmp1DB,_tmp1DB,_tmp1DB + 9}};struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(
union Cyc_YYSTYPE_union yy1);struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(union Cyc_YYSTYPE_union
yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE_union _tmp1DC=yy1;struct Cyc_Absyn_Fndecl*
_tmp1DD;_LL135: if((_tmp1DC.YY17).tag != 21)goto _LL137;_tmp1DD=(_tmp1DC.YY17).f1;
_LL136: yyzzz=_tmp1DD;goto _LL134;_LL137:;_LL138:(int)_throw((void*)& Cyc_yyfail_YY17);
_LL134:;}return yyzzz;}static char _tmp1DF[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp1DF,_tmp1DF,_tmp1DF + 15}};struct Cyc_List_List*
Cyc_yyget_YY18(union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY18(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp1E0=yy1;struct Cyc_List_List*_tmp1E1;_LL13A: if((_tmp1E0.YY18).tag != 22)goto
_LL13C;_tmp1E1=(_tmp1E0.YY18).f1;_LL13B: yyzzz=_tmp1E1;goto _LL139;_LL13C:;_LL13D:(
int)_throw((void*)& Cyc_yyfail_YY18);_LL139:;}return yyzzz;}static char _tmp1E3[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp1E3,
_tmp1E3,_tmp1E3 + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE_union
yy1);struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE_union yy1){
struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE_union _tmp1E4=yy1;struct
Cyc_Parse_Declaration_spec*_tmp1E5;_LL13F: if((_tmp1E4.YY19).tag != 23)goto _LL141;
_tmp1E5=(_tmp1E4.YY19).f1;_LL140: yyzzz=_tmp1E5;goto _LL13E;_LL141:;_LL142:(int)
_throw((void*)& Cyc_yyfail_YY19);_LL13E:;}return yyzzz;}static char _tmp1E7[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp1E7,
_tmp1E7,_tmp1E7 + 27}};struct _tuple15*Cyc_yyget_YY20(union Cyc_YYSTYPE_union yy1);
struct _tuple15*Cyc_yyget_YY20(union Cyc_YYSTYPE_union yy1){struct _tuple15*yyzzz;{
union Cyc_YYSTYPE_union _tmp1E8=yy1;struct _tuple15*_tmp1E9;_LL144: if((_tmp1E8.YY20).tag
!= 24)goto _LL146;_tmp1E9=(_tmp1E8.YY20).f1;_LL145: yyzzz=_tmp1E9;goto _LL143;
_LL146:;_LL147:(int)_throw((void*)& Cyc_yyfail_YY20);_LL143:;}return yyzzz;}static
char _tmp1EB[35]="list_t<$(declarator_t,exp_opt_t)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp1EB,_tmp1EB,_tmp1EB + 35}};struct Cyc_List_List*
Cyc_yyget_YY21(union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY21(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp1EC=yy1;struct Cyc_List_List*_tmp1ED;_LL149: if((_tmp1EC.YY21).tag != 25)goto
_LL14B;_tmp1ED=(_tmp1EC.YY21).f1;_LL14A: yyzzz=_tmp1ED;goto _LL148;_LL14B:;_LL14C:(
int)_throw((void*)& Cyc_yyfail_YY21);_LL148:;}return yyzzz;}static char _tmp1EF[16]="storage_class_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp1EF,
_tmp1EF,_tmp1EF + 16}};void*Cyc_yyget_YY22(union Cyc_YYSTYPE_union yy1);void*Cyc_yyget_YY22(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1F0=yy1;void*
_tmp1F1;_LL14E: if((_tmp1F0.YY22).tag != 26)goto _LL150;_tmp1F1=(_tmp1F0.YY22).f1;
_LL14F: yyzzz=_tmp1F1;goto _LL14D;_LL150:;_LL151:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL14D:;}return yyzzz;}static char _tmp1F3[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp1F3,_tmp1F3,_tmp1F3 + 17}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE_union yy1);void*Cyc_yyget_YY23(union Cyc_YYSTYPE_union yy1){void*
yyzzz;{union Cyc_YYSTYPE_union _tmp1F4=yy1;void*_tmp1F5;_LL153: if((_tmp1F4.YY23).tag
!= 27)goto _LL155;_tmp1F5=(_tmp1F4.YY23).f1;_LL154: yyzzz=_tmp1F5;goto _LL152;
_LL155:;_LL156:(int)_throw((void*)& Cyc_yyfail_YY23);_LL152:;}return yyzzz;}static
char _tmp1F7[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY24={
Cyc_Core_Failure,{_tmp1F7,_tmp1F7,_tmp1F7 + 12}};void*Cyc_yyget_YY24(union Cyc_YYSTYPE_union
yy1);void*Cyc_yyget_YY24(union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union
_tmp1F8=yy1;void*_tmp1F9;_LL158: if((_tmp1F8.YY24).tag != 28)goto _LL15A;_tmp1F9=(
_tmp1F8.YY24).f1;_LL159: yyzzz=_tmp1F9;goto _LL157;_LL15A:;_LL15B:(int)_throw((
void*)& Cyc_yyfail_YY24);_LL157:;}return yyzzz;}static char _tmp1FB[8]="tqual_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp1FB,
_tmp1FB,_tmp1FB + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE_union
yy1);struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Tqual
yyzzz;{union Cyc_YYSTYPE_union _tmp1FC=yy1;struct Cyc_Absyn_Tqual _tmp1FD;_LL15D: if((
_tmp1FC.YY25).tag != 29)goto _LL15F;_tmp1FD=(_tmp1FC.YY25).f1;_LL15E: yyzzz=_tmp1FD;
goto _LL15C;_LL15F:;_LL160:(int)_throw((void*)& Cyc_yyfail_YY25);_LL15C:;}return
yyzzz;}static char _tmp1FF[20]="list_t<aggrfield_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp1FF,_tmp1FF,_tmp1FF + 20}};struct Cyc_List_List*
Cyc_yyget_YY26(union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY26(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp200=yy1;struct Cyc_List_List*_tmp201;_LL162: if((_tmp200.YY26).tag != 30)goto
_LL164;_tmp201=(_tmp200.YY26).f1;_LL163: yyzzz=_tmp201;goto _LL161;_LL164:;_LL165:(
int)_throw((void*)& Cyc_yyfail_YY26);_LL161:;}return yyzzz;}static char _tmp203[28]="list_t<list_t<aggrfield_t>>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp203,
_tmp203,_tmp203 + 28}};struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE_union
yy1);struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp204=yy1;struct Cyc_List_List*_tmp205;_LL167: if((
_tmp204.YY27).tag != 31)goto _LL169;_tmp205=(_tmp204.YY27).f1;_LL168: yyzzz=_tmp205;
goto _LL166;_LL169:;_LL16A:(int)_throw((void*)& Cyc_yyfail_YY27);_LL166:;}return
yyzzz;}static char _tmp207[24]="list_t<type_modifier_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp207,_tmp207,_tmp207 + 24}};struct Cyc_List_List*
Cyc_yyget_YY28(union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY28(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp208=yy1;struct Cyc_List_List*_tmp209;_LL16C: if((_tmp208.YY28).tag != 32)goto
_LL16E;_tmp209=(_tmp208.YY28).f1;_LL16D: yyzzz=_tmp209;goto _LL16B;_LL16E:;_LL16F:(
int)_throw((void*)& Cyc_yyfail_YY28);_LL16B:;}return yyzzz;}static char _tmp20B[13]="declarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp20B,
_tmp20B,_tmp20B + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(union Cyc_YYSTYPE_union
yy1);struct Cyc_Parse_Declarator*Cyc_yyget_YY29(union Cyc_YYSTYPE_union yy1){struct
Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE_union _tmp20C=yy1;struct Cyc_Parse_Declarator*
_tmp20D;_LL171: if((_tmp20C.YY29).tag != 33)goto _LL173;_tmp20D=(_tmp20C.YY29).f1;
_LL172: yyzzz=_tmp20D;goto _LL170;_LL173:;_LL174:(int)_throw((void*)& Cyc_yyfail_YY29);
_LL170:;}return yyzzz;}static char _tmp20F[21]="abstractdeclarator_t";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp20F,_tmp20F,
_tmp20F + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_YY30(union Cyc_YYSTYPE_union
yy1);struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_YY30(union Cyc_YYSTYPE_union yy1){
struct Cyc_Parse_Abstractdeclarator*yyzzz;{union Cyc_YYSTYPE_union _tmp210=yy1;
struct Cyc_Parse_Abstractdeclarator*_tmp211;_LL176: if((_tmp210.YY30).tag != 34)
goto _LL178;_tmp211=(_tmp210.YY30).f1;_LL177: yyzzz=_tmp211;goto _LL175;_LL178:;
_LL179:(int)_throw((void*)& Cyc_yyfail_YY30);_LL175:;}return yyzzz;}static char
_tmp213[5]="bool";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{
_tmp213,_tmp213,_tmp213 + 5}};int Cyc_yyget_YY31(union Cyc_YYSTYPE_union yy1);int Cyc_yyget_YY31(
union Cyc_YYSTYPE_union yy1){int yyzzz;{union Cyc_YYSTYPE_union _tmp214=yy1;int
_tmp215;_LL17B: if((_tmp214.YY31).tag != 35)goto _LL17D;_tmp215=(_tmp214.YY31).f1;
_LL17C: yyzzz=_tmp215;goto _LL17A;_LL17D:;_LL17E:(int)_throw((void*)& Cyc_yyfail_YY31);
_LL17A:;}return yyzzz;}static char _tmp217[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp217,_tmp217,_tmp217 + 8}};void*Cyc_yyget_YY32(
union Cyc_YYSTYPE_union yy1);void*Cyc_yyget_YY32(union Cyc_YYSTYPE_union yy1){void*
yyzzz;{union Cyc_YYSTYPE_union _tmp218=yy1;void*_tmp219;_LL180: if((_tmp218.YY32).tag
!= 36)goto _LL182;_tmp219=(_tmp218.YY32).f1;_LL181: yyzzz=_tmp219;goto _LL17F;
_LL182:;_LL183:(int)_throw((void*)& Cyc_yyfail_YY32);_LL17F:;}return yyzzz;}static
char _tmp21B[14]="tunionfield_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY33={
Cyc_Core_Failure,{_tmp21B,_tmp21B,_tmp21B + 14}};struct Cyc_Absyn_Tunionfield*Cyc_yyget_YY33(
union Cyc_YYSTYPE_union yy1);struct Cyc_Absyn_Tunionfield*Cyc_yyget_YY33(union Cyc_YYSTYPE_union
yy1){struct Cyc_Absyn_Tunionfield*yyzzz;{union Cyc_YYSTYPE_union _tmp21C=yy1;struct
Cyc_Absyn_Tunionfield*_tmp21D;_LL185: if((_tmp21C.YY33).tag != 37)goto _LL187;
_tmp21D=(_tmp21C.YY33).f1;_LL186: yyzzz=_tmp21D;goto _LL184;_LL187:;_LL188:(int)
_throw((void*)& Cyc_yyfail_YY33);_LL184:;}return yyzzz;}static char _tmp21F[22]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp21F,
_tmp21F,_tmp21F + 22}};struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE_union
yy1);struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp220=yy1;struct Cyc_List_List*_tmp221;_LL18A: if((
_tmp220.YY34).tag != 38)goto _LL18C;_tmp221=(_tmp220.YY34).f1;_LL18B: yyzzz=_tmp221;
goto _LL189;_LL18C:;_LL18D:(int)_throw((void*)& Cyc_yyfail_YY34);_LL189:;}return
yyzzz;}static char _tmp223[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp223,
_tmp223,_tmp223 + 50}};struct _tuple16*Cyc_yyget_YY35(union Cyc_YYSTYPE_union yy1);
struct _tuple16*Cyc_yyget_YY35(union Cyc_YYSTYPE_union yy1){struct _tuple16*yyzzz;{
union Cyc_YYSTYPE_union _tmp224=yy1;struct _tuple16*_tmp225;_LL18F: if((_tmp224.YY35).tag
!= 39)goto _LL191;_tmp225=(_tmp224.YY35).f1;_LL190: yyzzz=_tmp225;goto _LL18E;
_LL191:;_LL192:(int)_throw((void*)& Cyc_yyfail_YY35);_LL18E:;}return yyzzz;}static
char _tmp227[14]="list_t<var_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY36={
Cyc_Core_Failure,{_tmp227,_tmp227,_tmp227 + 14}};struct Cyc_List_List*Cyc_yyget_YY36(
union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp228=yy1;struct Cyc_List_List*
_tmp229;_LL194: if((_tmp228.YY36).tag != 40)goto _LL196;_tmp229=(_tmp228.YY36).f1;
_LL195: yyzzz=_tmp229;goto _LL193;_LL196:;_LL197:(int)_throw((void*)& Cyc_yyfail_YY36);
_LL193:;}return yyzzz;}static char _tmp22B[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp22B,
_tmp22B,_tmp22B + 32}};struct _tuple2*Cyc_yyget_YY37(union Cyc_YYSTYPE_union yy1);
struct _tuple2*Cyc_yyget_YY37(union Cyc_YYSTYPE_union yy1){struct _tuple2*yyzzz;{
union Cyc_YYSTYPE_union _tmp22C=yy1;struct _tuple2*_tmp22D;_LL199: if((_tmp22C.YY37).tag
!= 41)goto _LL19B;_tmp22D=(_tmp22C.YY37).f1;_LL19A: yyzzz=_tmp22D;goto _LL198;
_LL19B:;_LL19C:(int)_throw((void*)& Cyc_yyfail_YY37);_LL198:;}return yyzzz;}static
char _tmp22F[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp22F,_tmp22F,_tmp22F + 40}};struct Cyc_List_List*
Cyc_yyget_YY38(union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY38(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp230=yy1;struct Cyc_List_List*_tmp231;_LL19E: if((_tmp230.YY38).tag != 42)goto
_LL1A0;_tmp231=(_tmp230.YY38).f1;_LL19F: yyzzz=_tmp231;goto _LL19D;_LL1A0:;_LL1A1:(
int)_throw((void*)& Cyc_yyfail_YY38);_LL19D:;}return yyzzz;}static char _tmp233[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp233,
_tmp233,_tmp233 + 107}};struct _tuple17*Cyc_yyget_YY39(union Cyc_YYSTYPE_union yy1);
struct _tuple17*Cyc_yyget_YY39(union Cyc_YYSTYPE_union yy1){struct _tuple17*yyzzz;{
union Cyc_YYSTYPE_union _tmp234=yy1;struct _tuple17*_tmp235;_LL1A3: if((_tmp234.YY39).tag
!= 43)goto _LL1A5;_tmp235=(_tmp234.YY39).f1;_LL1A4: yyzzz=_tmp235;goto _LL1A2;
_LL1A5:;_LL1A6:(int)_throw((void*)& Cyc_yyfail_YY39);_LL1A2:;}return yyzzz;}static
char _tmp237[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY40={
Cyc_Core_Failure,{_tmp237,_tmp237,_tmp237 + 15}};struct Cyc_List_List*Cyc_yyget_YY40(
union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp238=yy1;struct Cyc_List_List*
_tmp239;_LL1A8: if((_tmp238.YY40).tag != 44)goto _LL1AA;_tmp239=(_tmp238.YY40).f1;
_LL1A9: yyzzz=_tmp239;goto _LL1A7;_LL1AA:;_LL1AB:(int)_throw((void*)& Cyc_yyfail_YY40);
_LL1A7:;}return yyzzz;}static char _tmp23B[21]="list_t<designator_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp23B,_tmp23B,
_tmp23B + 21}};struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE_union yy1);
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp23C=yy1;struct Cyc_List_List*_tmp23D;_LL1AD: if((
_tmp23C.YY41).tag != 45)goto _LL1AF;_tmp23D=(_tmp23C.YY41).f1;_LL1AE: yyzzz=_tmp23D;
goto _LL1AC;_LL1AF:;_LL1B0:(int)_throw((void*)& Cyc_yyfail_YY41);_LL1AC:;}return
yyzzz;}static char _tmp23F[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp23F,_tmp23F,_tmp23F + 13}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE_union yy1);void*Cyc_yyget_YY42(union Cyc_YYSTYPE_union yy1){void*
yyzzz;{union Cyc_YYSTYPE_union _tmp240=yy1;void*_tmp241;_LL1B2: if((_tmp240.YY42).tag
!= 46)goto _LL1B4;_tmp241=(_tmp240.YY42).f1;_LL1B3: yyzzz=_tmp241;goto _LL1B1;
_LL1B4:;_LL1B5:(int)_throw((void*)& Cyc_yyfail_YY42);_LL1B1:;}return yyzzz;}static
char _tmp243[7]="kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{
_tmp243,_tmp243,_tmp243 + 7}};void*Cyc_yyget_YY43(union Cyc_YYSTYPE_union yy1);void*
Cyc_yyget_YY43(union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union
_tmp244=yy1;void*_tmp245;_LL1B7: if((_tmp244.YY43).tag != 47)goto _LL1B9;_tmp245=(
_tmp244.YY43).f1;_LL1B8: yyzzz=_tmp245;goto _LL1B6;_LL1B9:;_LL1BA:(int)_throw((
void*)& Cyc_yyfail_YY43);_LL1B6:;}return yyzzz;}static char _tmp247[7]="type_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp247,
_tmp247,_tmp247 + 7}};void*Cyc_yyget_YY44(union Cyc_YYSTYPE_union yy1);void*Cyc_yyget_YY44(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp248=yy1;void*
_tmp249;_LL1BC: if((_tmp248.YY44).tag != 48)goto _LL1BE;_tmp249=(_tmp248.YY44).f1;
_LL1BD: yyzzz=_tmp249;goto _LL1BB;_LL1BE:;_LL1BF:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1BB:;}return yyzzz;}static char _tmp24B[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp24B,_tmp24B,
_tmp24B + 20}};struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE_union yy1);
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp24C=yy1;struct Cyc_List_List*_tmp24D;_LL1C1: if((
_tmp24C.YY45).tag != 49)goto _LL1C3;_tmp24D=(_tmp24C.YY45).f1;_LL1C2: yyzzz=_tmp24D;
goto _LL1C0;_LL1C3:;_LL1C4:(int)_throw((void*)& Cyc_yyfail_YY45);_LL1C0:;}return
yyzzz;}static char _tmp24F[12]="attribute_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp24F,_tmp24F,_tmp24F + 12}};void*Cyc_yyget_YY46(
union Cyc_YYSTYPE_union yy1);void*Cyc_yyget_YY46(union Cyc_YYSTYPE_union yy1){void*
yyzzz;{union Cyc_YYSTYPE_union _tmp250=yy1;void*_tmp251;_LL1C6: if((_tmp250.YY46).tag
!= 50)goto _LL1C8;_tmp251=(_tmp250.YY46).f1;_LL1C7: yyzzz=_tmp251;goto _LL1C5;
_LL1C8:;_LL1C9:(int)_throw((void*)& Cyc_yyfail_YY46);_LL1C5:;}return yyzzz;}static
char _tmp253[12]="enumfield_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY47={
Cyc_Core_Failure,{_tmp253,_tmp253,_tmp253 + 12}};struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(
union Cyc_YYSTYPE_union yy1);struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE_union
yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union Cyc_YYSTYPE_union _tmp254=yy1;struct
Cyc_Absyn_Enumfield*_tmp255;_LL1CB: if((_tmp254.YY47).tag != 51)goto _LL1CD;_tmp255=(
_tmp254.YY47).f1;_LL1CC: yyzzz=_tmp255;goto _LL1CA;_LL1CD:;_LL1CE:(int)_throw((
void*)& Cyc_yyfail_YY47);_LL1CA:;}return yyzzz;}static char _tmp257[20]="list_t<enumfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp257,
_tmp257,_tmp257 + 20}};struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE_union
yy1);struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp258=yy1;struct Cyc_List_List*_tmp259;_LL1D0: if((
_tmp258.YY48).tag != 52)goto _LL1D2;_tmp259=(_tmp258.YY48).f1;_LL1D1: yyzzz=_tmp259;
goto _LL1CF;_LL1D2:;_LL1D3:(int)_throw((void*)& Cyc_yyfail_YY48);_LL1CF:;}return
yyzzz;}static char _tmp25B[14]="opt_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp25B,_tmp25B,_tmp25B + 14}};struct Cyc_Core_Opt*
Cyc_yyget_YY49(union Cyc_YYSTYPE_union yy1);struct Cyc_Core_Opt*Cyc_yyget_YY49(
union Cyc_YYSTYPE_union yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE_union
_tmp25C=yy1;struct Cyc_Core_Opt*_tmp25D;_LL1D5: if((_tmp25C.YY49).tag != 53)goto
_LL1D7;_tmp25D=(_tmp25C.YY49).f1;_LL1D6: yyzzz=_tmp25D;goto _LL1D4;_LL1D7:;_LL1D8:(
int)_throw((void*)& Cyc_yyfail_YY49);_LL1D4:;}return yyzzz;}static char _tmp25F[26]="list_t<$(type_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp25F,
_tmp25F,_tmp25F + 26}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE_union
yy1);struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp260=yy1;struct Cyc_List_List*_tmp261;_LL1DA: if((
_tmp260.YY50).tag != 54)goto _LL1DC;_tmp261=(_tmp260.YY50).f1;_LL1DB: yyzzz=_tmp261;
goto _LL1D9;_LL1DC:;_LL1DD:(int)_throw((void*)& Cyc_yyfail_YY50);_LL1D9:;}return
yyzzz;}static char _tmp263[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp263,_tmp263,_tmp263 + 15}};struct Cyc_Absyn_Conref*
Cyc_yyget_YY51(union Cyc_YYSTYPE_union yy1);struct Cyc_Absyn_Conref*Cyc_yyget_YY51(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Conref*yyzzz;{union Cyc_YYSTYPE_union
_tmp264=yy1;struct Cyc_Absyn_Conref*_tmp265;_LL1DF: if((_tmp264.YY51).tag != 55)
goto _LL1E1;_tmp265=(_tmp264.YY51).f1;_LL1E0: yyzzz=_tmp265;goto _LL1DE;_LL1E1:;
_LL1E2:(int)_throw((void*)& Cyc_yyfail_YY51);_LL1DE:;}return yyzzz;}static char
_tmp267[40]="list_t<$(Position::seg_t,qvar_t,bool)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp267,_tmp267,_tmp267 + 40}};struct Cyc_List_List*
Cyc_yyget_YY52(union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY52(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp268=yy1;struct Cyc_List_List*_tmp269;_LL1E4: if((_tmp268.YY52).tag != 56)goto
_LL1E6;_tmp269=(_tmp268.YY52).f1;_LL1E5: yyzzz=_tmp269;goto _LL1E3;_LL1E6:;_LL1E7:(
int)_throw((void*)& Cyc_yyfail_YY52);_LL1E3:;}return yyzzz;}struct Cyc_Yyltype{int
timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_Yyltype
Cyc_yynewloc();struct Cyc_Yyltype Cyc_yynewloc(){struct Cyc_Yyltype _tmpA06;return(
_tmpA06.timestamp=0,((_tmpA06.first_line=0,((_tmpA06.first_column=0,((_tmpA06.last_line=
0,((_tmpA06.last_column=0,_tmpA06)))))))));}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,
0};static short Cyc_yytranslate[362]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,133,2,2,119,131,128,2,116,117,122,125,110,129,121,130,2,2,
2,2,2,2,2,2,2,2,120,107,112,111,113,124,123,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,114,2,115,127,118,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,108,126,109,132,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,
44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,
71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,
98,99,100,101,102,103,104,105,106};static char _tmp26B[2]="$";static char _tmp26C[6]="error";
static char _tmp26D[12]="$undefined.";static char _tmp26E[5]="AUTO";static char
_tmp26F[9]="REGISTER";static char _tmp270[7]="STATIC";static char _tmp271[7]="EXTERN";
static char _tmp272[8]="TYPEDEF";static char _tmp273[5]="VOID";static char _tmp274[5]="CHAR";
static char _tmp275[6]="SHORT";static char _tmp276[4]="INT";static char _tmp277[5]="LONG";
static char _tmp278[6]="FLOAT";static char _tmp279[7]="DOUBLE";static char _tmp27A[7]="SIGNED";
static char _tmp27B[9]="UNSIGNED";static char _tmp27C[6]="CONST";static char _tmp27D[9]="VOLATILE";
static char _tmp27E[9]="RESTRICT";static char _tmp27F[7]="STRUCT";static char _tmp280[
6]="UNION";static char _tmp281[5]="CASE";static char _tmp282[8]="DEFAULT";static char
_tmp283[7]="INLINE";static char _tmp284[7]="SIZEOF";static char _tmp285[9]="OFFSETOF";
static char _tmp286[3]="IF";static char _tmp287[5]="ELSE";static char _tmp288[7]="SWITCH";
static char _tmp289[6]="WHILE";static char _tmp28A[3]="DO";static char _tmp28B[4]="FOR";
static char _tmp28C[5]="GOTO";static char _tmp28D[9]="CONTINUE";static char _tmp28E[6]="BREAK";
static char _tmp28F[7]="RETURN";static char _tmp290[5]="ENUM";static char _tmp291[8]="NULL_kw";
static char _tmp292[4]="LET";static char _tmp293[6]="THROW";static char _tmp294[4]="TRY";
static char _tmp295[6]="CATCH";static char _tmp296[7]="EXPORT";static char _tmp297[4]="NEW";
static char _tmp298[9]="ABSTRACT";static char _tmp299[9]="FALLTHRU";static char
_tmp29A[6]="USING";static char _tmp29B[10]="NAMESPACE";static char _tmp29C[7]="TUNION";
static char _tmp29D[8]="XTUNION";static char _tmp29E[7]="MALLOC";static char _tmp29F[8]="RMALLOC";
static char _tmp2A0[7]="CALLOC";static char _tmp2A1[8]="RCALLOC";static char _tmp2A2[5]="SWAP";
static char _tmp2A3[9]="REGION_T";static char _tmp2A4[6]="TAG_T";static char _tmp2A5[7]="REGION";
static char _tmp2A6[5]="RNEW";static char _tmp2A7[8]="REGIONS";static char _tmp2A8[13]="RESET_REGION";
static char _tmp2A9[4]="GEN";static char _tmp2AA[14]="NOZEROTERM_kw";static char
_tmp2AB[12]="ZEROTERM_kw";static char _tmp2AC[7]="PORTON";static char _tmp2AD[8]="PORTOFF";
static char _tmp2AE[8]="FLAT_kw";static char _tmp2AF[12]="DYNREGION_T";static char
_tmp2B0[6]="ALIAS";static char _tmp2B1[8]="NUMELTS";static char _tmp2B2[8]="VALUEOF";
static char _tmp2B3[10]="VALUEOF_T";static char _tmp2B4[7]="PTR_OP";static char
_tmp2B5[7]="INC_OP";static char _tmp2B6[7]="DEC_OP";static char _tmp2B7[8]="LEFT_OP";
static char _tmp2B8[9]="RIGHT_OP";static char _tmp2B9[6]="LE_OP";static char _tmp2BA[6]="GE_OP";
static char _tmp2BB[6]="EQ_OP";static char _tmp2BC[6]="NE_OP";static char _tmp2BD[7]="AND_OP";
static char _tmp2BE[6]="OR_OP";static char _tmp2BF[11]="MUL_ASSIGN";static char
_tmp2C0[11]="DIV_ASSIGN";static char _tmp2C1[11]="MOD_ASSIGN";static char _tmp2C2[11]="ADD_ASSIGN";
static char _tmp2C3[11]="SUB_ASSIGN";static char _tmp2C4[12]="LEFT_ASSIGN";static
char _tmp2C5[13]="RIGHT_ASSIGN";static char _tmp2C6[11]="AND_ASSIGN";static char
_tmp2C7[11]="XOR_ASSIGN";static char _tmp2C8[10]="OR_ASSIGN";static char _tmp2C9[9]="ELLIPSIS";
static char _tmp2CA[11]="LEFT_RIGHT";static char _tmp2CB[12]="COLON_COLON";static
char _tmp2CC[11]="IDENTIFIER";static char _tmp2CD[17]="INTEGER_CONSTANT";static char
_tmp2CE[7]="STRING";static char _tmp2CF[19]="CHARACTER_CONSTANT";static char _tmp2D0[
18]="FLOATING_CONSTANT";static char _tmp2D1[9]="TYPE_VAR";static char _tmp2D2[13]="TYPEDEF_NAME";
static char _tmp2D3[16]="QUAL_IDENTIFIER";static char _tmp2D4[18]="QUAL_TYPEDEF_NAME";
static char _tmp2D5[10]="ATTRIBUTE";static char _tmp2D6[4]="';'";static char _tmp2D7[4]="'{'";
static char _tmp2D8[4]="'}'";static char _tmp2D9[4]="','";static char _tmp2DA[4]="'='";
static char _tmp2DB[4]="'<'";static char _tmp2DC[4]="'>'";static char _tmp2DD[4]="'['";
static char _tmp2DE[4]="']'";static char _tmp2DF[4]="'('";static char _tmp2E0[4]="')'";
static char _tmp2E1[4]="'_'";static char _tmp2E2[4]="'$'";static char _tmp2E3[4]="':'";
static char _tmp2E4[4]="'.'";static char _tmp2E5[4]="'*'";static char _tmp2E6[4]="'@'";
static char _tmp2E7[4]="'?'";static char _tmp2E8[4]="'+'";static char _tmp2E9[4]="'|'";
static char _tmp2EA[4]="'^'";static char _tmp2EB[4]="'&'";static char _tmp2EC[4]="'-'";
static char _tmp2ED[4]="'/'";static char _tmp2EE[4]="'%'";static char _tmp2EF[4]="'~'";
static char _tmp2F0[4]="'!'";static char _tmp2F1[5]="prog";static char _tmp2F2[17]="translation_unit";
static char _tmp2F3[12]="export_list";static char _tmp2F4[19]="export_list_values";
static char _tmp2F5[21]="external_declaration";static char _tmp2F6[20]="function_definition";
static char _tmp2F7[21]="function_definition2";static char _tmp2F8[13]="using_action";
static char _tmp2F9[15]="unusing_action";static char _tmp2FA[17]="namespace_action";
static char _tmp2FB[19]="unnamespace_action";static char _tmp2FC[12]="declaration";
static char _tmp2FD[17]="declaration_list";static char _tmp2FE[23]="declaration_specifiers";
static char _tmp2FF[24]="storage_class_specifier";static char _tmp300[15]="attributes_opt";
static char _tmp301[11]="attributes";static char _tmp302[15]="attribute_list";static
char _tmp303[10]="attribute";static char _tmp304[15]="type_specifier";static char
_tmp305[25]="type_specifier_notypedef";static char _tmp306[5]="kind";static char
_tmp307[15]="type_qualifier";static char _tmp308[15]="enum_specifier";static char
_tmp309[11]="enum_field";static char _tmp30A[22]="enum_declaration_list";static
char _tmp30B[26]="struct_or_union_specifier";static char _tmp30C[16]="type_params_opt";
static char _tmp30D[16]="struct_or_union";static char _tmp30E[24]="struct_declaration_list";
static char _tmp30F[25]="struct_declaration_list0";static char _tmp310[21]="init_declarator_list";
static char _tmp311[22]="init_declarator_list0";static char _tmp312[16]="init_declarator";
static char _tmp313[19]="struct_declaration";static char _tmp314[25]="specifier_qualifier_list";
static char _tmp315[35]="notypedef_specifier_qualifier_list";static char _tmp316[23]="struct_declarator_list";
static char _tmp317[24]="struct_declarator_list0";static char _tmp318[18]="struct_declarator";
static char _tmp319[17]="tunion_specifier";static char _tmp31A[9]="flat_opt";static
char _tmp31B[18]="tunion_or_xtunion";static char _tmp31C[17]="tunionfield_list";
static char _tmp31D[18]="tunionfield_scope";static char _tmp31E[12]="tunionfield";
static char _tmp31F[11]="declarator";static char _tmp320[23]="declarator_withtypedef";
static char _tmp321[18]="direct_declarator";static char _tmp322[30]="direct_declarator_withtypedef";
static char _tmp323[8]="pointer";static char _tmp324[12]="one_pointer";static char
_tmp325[23]="pointer_null_and_bound";static char _tmp326[14]="pointer_bound";
static char _tmp327[18]="zeroterm_qual_opt";static char _tmp328[12]="opt_rgn_opt";
static char _tmp329[8]="rgn_opt";static char _tmp32A[11]="tqual_list";static char
_tmp32B[20]="parameter_type_list";static char _tmp32C[9]="type_var";static char
_tmp32D[16]="optional_effect";static char _tmp32E[19]="optional_rgn_order";static
char _tmp32F[10]="rgn_order";static char _tmp330[16]="optional_inject";static char
_tmp331[11]="effect_set";static char _tmp332[14]="atomic_effect";static char _tmp333[
11]="region_set";static char _tmp334[15]="parameter_list";static char _tmp335[22]="parameter_declaration";
static char _tmp336[16]="identifier_list";static char _tmp337[17]="identifier_list0";
static char _tmp338[12]="initializer";static char _tmp339[18]="array_initializer";
static char _tmp33A[17]="initializer_list";static char _tmp33B[12]="designation";
static char _tmp33C[16]="designator_list";static char _tmp33D[11]="designator";
static char _tmp33E[10]="type_name";static char _tmp33F[14]="any_type_name";static
char _tmp340[15]="type_name_list";static char _tmp341[20]="abstract_declarator";
static char _tmp342[27]="direct_abstract_declarator";static char _tmp343[10]="statement";
static char _tmp344[13]="open_exp_opt";static char _tmp345[18]="labeled_statement";
static char _tmp346[21]="expression_statement";static char _tmp347[19]="compound_statement";
static char _tmp348[16]="block_item_list";static char _tmp349[20]="selection_statement";
static char _tmp34A[15]="switch_clauses";static char _tmp34B[20]="iteration_statement";
static char _tmp34C[15]="jump_statement";static char _tmp34D[12]="exp_pattern";
static char _tmp34E[20]="conditional_pattern";static char _tmp34F[19]="logical_or_pattern";
static char _tmp350[20]="logical_and_pattern";static char _tmp351[21]="inclusive_or_pattern";
static char _tmp352[21]="exclusive_or_pattern";static char _tmp353[12]="and_pattern";
static char _tmp354[17]="equality_pattern";static char _tmp355[19]="relational_pattern";
static char _tmp356[14]="shift_pattern";static char _tmp357[17]="additive_pattern";
static char _tmp358[23]="multiplicative_pattern";static char _tmp359[13]="cast_pattern";
static char _tmp35A[14]="unary_pattern";static char _tmp35B[16]="postfix_pattern";
static char _tmp35C[16]="primary_pattern";static char _tmp35D[8]="pattern";static
char _tmp35E[19]="tuple_pattern_list";static char _tmp35F[20]="tuple_pattern_list0";
static char _tmp360[14]="field_pattern";static char _tmp361[19]="field_pattern_list";
static char _tmp362[20]="field_pattern_list0";static char _tmp363[11]="expression";
static char _tmp364[22]="assignment_expression";static char _tmp365[20]="assignment_operator";
static char _tmp366[23]="conditional_expression";static char _tmp367[20]="constant_expression";
static char _tmp368[22]="logical_or_expression";static char _tmp369[23]="logical_and_expression";
static char _tmp36A[24]="inclusive_or_expression";static char _tmp36B[24]="exclusive_or_expression";
static char _tmp36C[15]="and_expression";static char _tmp36D[20]="equality_expression";
static char _tmp36E[22]="relational_expression";static char _tmp36F[17]="shift_expression";
static char _tmp370[20]="additive_expression";static char _tmp371[26]="multiplicative_expression";
static char _tmp372[16]="cast_expression";static char _tmp373[17]="unary_expression";
static char _tmp374[15]="unary_operator";static char _tmp375[19]="postfix_expression";
static char _tmp376[19]="primary_expression";static char _tmp377[25]="argument_expression_list";
static char _tmp378[26]="argument_expression_list0";static char _tmp379[9]="constant";
static char _tmp37A[20]="qual_opt_identifier";static char _tmp37B[17]="qual_opt_typedef";
static char _tmp37C[18]="struct_union_name";static char _tmp37D[11]="field_name";
static char _tmp37E[12]="right_angle";static struct _dyneither_ptr Cyc_yytname[276]={{
_tmp26B,_tmp26B,_tmp26B + 2},{_tmp26C,_tmp26C,_tmp26C + 6},{_tmp26D,_tmp26D,
_tmp26D + 12},{_tmp26E,_tmp26E,_tmp26E + 5},{_tmp26F,_tmp26F,_tmp26F + 9},{_tmp270,
_tmp270,_tmp270 + 7},{_tmp271,_tmp271,_tmp271 + 7},{_tmp272,_tmp272,_tmp272 + 8},{
_tmp273,_tmp273,_tmp273 + 5},{_tmp274,_tmp274,_tmp274 + 5},{_tmp275,_tmp275,
_tmp275 + 6},{_tmp276,_tmp276,_tmp276 + 4},{_tmp277,_tmp277,_tmp277 + 5},{_tmp278,
_tmp278,_tmp278 + 6},{_tmp279,_tmp279,_tmp279 + 7},{_tmp27A,_tmp27A,_tmp27A + 7},{
_tmp27B,_tmp27B,_tmp27B + 9},{_tmp27C,_tmp27C,_tmp27C + 6},{_tmp27D,_tmp27D,
_tmp27D + 9},{_tmp27E,_tmp27E,_tmp27E + 9},{_tmp27F,_tmp27F,_tmp27F + 7},{_tmp280,
_tmp280,_tmp280 + 6},{_tmp281,_tmp281,_tmp281 + 5},{_tmp282,_tmp282,_tmp282 + 8},{
_tmp283,_tmp283,_tmp283 + 7},{_tmp284,_tmp284,_tmp284 + 7},{_tmp285,_tmp285,
_tmp285 + 9},{_tmp286,_tmp286,_tmp286 + 3},{_tmp287,_tmp287,_tmp287 + 5},{_tmp288,
_tmp288,_tmp288 + 7},{_tmp289,_tmp289,_tmp289 + 6},{_tmp28A,_tmp28A,_tmp28A + 3},{
_tmp28B,_tmp28B,_tmp28B + 4},{_tmp28C,_tmp28C,_tmp28C + 5},{_tmp28D,_tmp28D,
_tmp28D + 9},{_tmp28E,_tmp28E,_tmp28E + 6},{_tmp28F,_tmp28F,_tmp28F + 7},{_tmp290,
_tmp290,_tmp290 + 5},{_tmp291,_tmp291,_tmp291 + 8},{_tmp292,_tmp292,_tmp292 + 4},{
_tmp293,_tmp293,_tmp293 + 6},{_tmp294,_tmp294,_tmp294 + 4},{_tmp295,_tmp295,
_tmp295 + 6},{_tmp296,_tmp296,_tmp296 + 7},{_tmp297,_tmp297,_tmp297 + 4},{_tmp298,
_tmp298,_tmp298 + 9},{_tmp299,_tmp299,_tmp299 + 9},{_tmp29A,_tmp29A,_tmp29A + 6},{
_tmp29B,_tmp29B,_tmp29B + 10},{_tmp29C,_tmp29C,_tmp29C + 7},{_tmp29D,_tmp29D,
_tmp29D + 8},{_tmp29E,_tmp29E,_tmp29E + 7},{_tmp29F,_tmp29F,_tmp29F + 8},{_tmp2A0,
_tmp2A0,_tmp2A0 + 7},{_tmp2A1,_tmp2A1,_tmp2A1 + 8},{_tmp2A2,_tmp2A2,_tmp2A2 + 5},{
_tmp2A3,_tmp2A3,_tmp2A3 + 9},{_tmp2A4,_tmp2A4,_tmp2A4 + 6},{_tmp2A5,_tmp2A5,
_tmp2A5 + 7},{_tmp2A6,_tmp2A6,_tmp2A6 + 5},{_tmp2A7,_tmp2A7,_tmp2A7 + 8},{_tmp2A8,
_tmp2A8,_tmp2A8 + 13},{_tmp2A9,_tmp2A9,_tmp2A9 + 4},{_tmp2AA,_tmp2AA,_tmp2AA + 14},{
_tmp2AB,_tmp2AB,_tmp2AB + 12},{_tmp2AC,_tmp2AC,_tmp2AC + 7},{_tmp2AD,_tmp2AD,
_tmp2AD + 8},{_tmp2AE,_tmp2AE,_tmp2AE + 8},{_tmp2AF,_tmp2AF,_tmp2AF + 12},{_tmp2B0,
_tmp2B0,_tmp2B0 + 6},{_tmp2B1,_tmp2B1,_tmp2B1 + 8},{_tmp2B2,_tmp2B2,_tmp2B2 + 8},{
_tmp2B3,_tmp2B3,_tmp2B3 + 10},{_tmp2B4,_tmp2B4,_tmp2B4 + 7},{_tmp2B5,_tmp2B5,
_tmp2B5 + 7},{_tmp2B6,_tmp2B6,_tmp2B6 + 7},{_tmp2B7,_tmp2B7,_tmp2B7 + 8},{_tmp2B8,
_tmp2B8,_tmp2B8 + 9},{_tmp2B9,_tmp2B9,_tmp2B9 + 6},{_tmp2BA,_tmp2BA,_tmp2BA + 6},{
_tmp2BB,_tmp2BB,_tmp2BB + 6},{_tmp2BC,_tmp2BC,_tmp2BC + 6},{_tmp2BD,_tmp2BD,
_tmp2BD + 7},{_tmp2BE,_tmp2BE,_tmp2BE + 6},{_tmp2BF,_tmp2BF,_tmp2BF + 11},{_tmp2C0,
_tmp2C0,_tmp2C0 + 11},{_tmp2C1,_tmp2C1,_tmp2C1 + 11},{_tmp2C2,_tmp2C2,_tmp2C2 + 11},{
_tmp2C3,_tmp2C3,_tmp2C3 + 11},{_tmp2C4,_tmp2C4,_tmp2C4 + 12},{_tmp2C5,_tmp2C5,
_tmp2C5 + 13},{_tmp2C6,_tmp2C6,_tmp2C6 + 11},{_tmp2C7,_tmp2C7,_tmp2C7 + 11},{
_tmp2C8,_tmp2C8,_tmp2C8 + 10},{_tmp2C9,_tmp2C9,_tmp2C9 + 9},{_tmp2CA,_tmp2CA,
_tmp2CA + 11},{_tmp2CB,_tmp2CB,_tmp2CB + 12},{_tmp2CC,_tmp2CC,_tmp2CC + 11},{
_tmp2CD,_tmp2CD,_tmp2CD + 17},{_tmp2CE,_tmp2CE,_tmp2CE + 7},{_tmp2CF,_tmp2CF,
_tmp2CF + 19},{_tmp2D0,_tmp2D0,_tmp2D0 + 18},{_tmp2D1,_tmp2D1,_tmp2D1 + 9},{_tmp2D2,
_tmp2D2,_tmp2D2 + 13},{_tmp2D3,_tmp2D3,_tmp2D3 + 16},{_tmp2D4,_tmp2D4,_tmp2D4 + 18},{
_tmp2D5,_tmp2D5,_tmp2D5 + 10},{_tmp2D6,_tmp2D6,_tmp2D6 + 4},{_tmp2D7,_tmp2D7,
_tmp2D7 + 4},{_tmp2D8,_tmp2D8,_tmp2D8 + 4},{_tmp2D9,_tmp2D9,_tmp2D9 + 4},{_tmp2DA,
_tmp2DA,_tmp2DA + 4},{_tmp2DB,_tmp2DB,_tmp2DB + 4},{_tmp2DC,_tmp2DC,_tmp2DC + 4},{
_tmp2DD,_tmp2DD,_tmp2DD + 4},{_tmp2DE,_tmp2DE,_tmp2DE + 4},{_tmp2DF,_tmp2DF,
_tmp2DF + 4},{_tmp2E0,_tmp2E0,_tmp2E0 + 4},{_tmp2E1,_tmp2E1,_tmp2E1 + 4},{_tmp2E2,
_tmp2E2,_tmp2E2 + 4},{_tmp2E3,_tmp2E3,_tmp2E3 + 4},{_tmp2E4,_tmp2E4,_tmp2E4 + 4},{
_tmp2E5,_tmp2E5,_tmp2E5 + 4},{_tmp2E6,_tmp2E6,_tmp2E6 + 4},{_tmp2E7,_tmp2E7,
_tmp2E7 + 4},{_tmp2E8,_tmp2E8,_tmp2E8 + 4},{_tmp2E9,_tmp2E9,_tmp2E9 + 4},{_tmp2EA,
_tmp2EA,_tmp2EA + 4},{_tmp2EB,_tmp2EB,_tmp2EB + 4},{_tmp2EC,_tmp2EC,_tmp2EC + 4},{
_tmp2ED,_tmp2ED,_tmp2ED + 4},{_tmp2EE,_tmp2EE,_tmp2EE + 4},{_tmp2EF,_tmp2EF,
_tmp2EF + 4},{_tmp2F0,_tmp2F0,_tmp2F0 + 4},{_tmp2F1,_tmp2F1,_tmp2F1 + 5},{_tmp2F2,
_tmp2F2,_tmp2F2 + 17},{_tmp2F3,_tmp2F3,_tmp2F3 + 12},{_tmp2F4,_tmp2F4,_tmp2F4 + 19},{
_tmp2F5,_tmp2F5,_tmp2F5 + 21},{_tmp2F6,_tmp2F6,_tmp2F6 + 20},{_tmp2F7,_tmp2F7,
_tmp2F7 + 21},{_tmp2F8,_tmp2F8,_tmp2F8 + 13},{_tmp2F9,_tmp2F9,_tmp2F9 + 15},{
_tmp2FA,_tmp2FA,_tmp2FA + 17},{_tmp2FB,_tmp2FB,_tmp2FB + 19},{_tmp2FC,_tmp2FC,
_tmp2FC + 12},{_tmp2FD,_tmp2FD,_tmp2FD + 17},{_tmp2FE,_tmp2FE,_tmp2FE + 23},{
_tmp2FF,_tmp2FF,_tmp2FF + 24},{_tmp300,_tmp300,_tmp300 + 15},{_tmp301,_tmp301,
_tmp301 + 11},{_tmp302,_tmp302,_tmp302 + 15},{_tmp303,_tmp303,_tmp303 + 10},{
_tmp304,_tmp304,_tmp304 + 15},{_tmp305,_tmp305,_tmp305 + 25},{_tmp306,_tmp306,
_tmp306 + 5},{_tmp307,_tmp307,_tmp307 + 15},{_tmp308,_tmp308,_tmp308 + 15},{_tmp309,
_tmp309,_tmp309 + 11},{_tmp30A,_tmp30A,_tmp30A + 22},{_tmp30B,_tmp30B,_tmp30B + 26},{
_tmp30C,_tmp30C,_tmp30C + 16},{_tmp30D,_tmp30D,_tmp30D + 16},{_tmp30E,_tmp30E,
_tmp30E + 24},{_tmp30F,_tmp30F,_tmp30F + 25},{_tmp310,_tmp310,_tmp310 + 21},{
_tmp311,_tmp311,_tmp311 + 22},{_tmp312,_tmp312,_tmp312 + 16},{_tmp313,_tmp313,
_tmp313 + 19},{_tmp314,_tmp314,_tmp314 + 25},{_tmp315,_tmp315,_tmp315 + 35},{
_tmp316,_tmp316,_tmp316 + 23},{_tmp317,_tmp317,_tmp317 + 24},{_tmp318,_tmp318,
_tmp318 + 18},{_tmp319,_tmp319,_tmp319 + 17},{_tmp31A,_tmp31A,_tmp31A + 9},{_tmp31B,
_tmp31B,_tmp31B + 18},{_tmp31C,_tmp31C,_tmp31C + 17},{_tmp31D,_tmp31D,_tmp31D + 18},{
_tmp31E,_tmp31E,_tmp31E + 12},{_tmp31F,_tmp31F,_tmp31F + 11},{_tmp320,_tmp320,
_tmp320 + 23},{_tmp321,_tmp321,_tmp321 + 18},{_tmp322,_tmp322,_tmp322 + 30},{
_tmp323,_tmp323,_tmp323 + 8},{_tmp324,_tmp324,_tmp324 + 12},{_tmp325,_tmp325,
_tmp325 + 23},{_tmp326,_tmp326,_tmp326 + 14},{_tmp327,_tmp327,_tmp327 + 18},{
_tmp328,_tmp328,_tmp328 + 12},{_tmp329,_tmp329,_tmp329 + 8},{_tmp32A,_tmp32A,
_tmp32A + 11},{_tmp32B,_tmp32B,_tmp32B + 20},{_tmp32C,_tmp32C,_tmp32C + 9},{_tmp32D,
_tmp32D,_tmp32D + 16},{_tmp32E,_tmp32E,_tmp32E + 19},{_tmp32F,_tmp32F,_tmp32F + 10},{
_tmp330,_tmp330,_tmp330 + 16},{_tmp331,_tmp331,_tmp331 + 11},{_tmp332,_tmp332,
_tmp332 + 14},{_tmp333,_tmp333,_tmp333 + 11},{_tmp334,_tmp334,_tmp334 + 15},{
_tmp335,_tmp335,_tmp335 + 22},{_tmp336,_tmp336,_tmp336 + 16},{_tmp337,_tmp337,
_tmp337 + 17},{_tmp338,_tmp338,_tmp338 + 12},{_tmp339,_tmp339,_tmp339 + 18},{
_tmp33A,_tmp33A,_tmp33A + 17},{_tmp33B,_tmp33B,_tmp33B + 12},{_tmp33C,_tmp33C,
_tmp33C + 16},{_tmp33D,_tmp33D,_tmp33D + 11},{_tmp33E,_tmp33E,_tmp33E + 10},{
_tmp33F,_tmp33F,_tmp33F + 14},{_tmp340,_tmp340,_tmp340 + 15},{_tmp341,_tmp341,
_tmp341 + 20},{_tmp342,_tmp342,_tmp342 + 27},{_tmp343,_tmp343,_tmp343 + 10},{
_tmp344,_tmp344,_tmp344 + 13},{_tmp345,_tmp345,_tmp345 + 18},{_tmp346,_tmp346,
_tmp346 + 21},{_tmp347,_tmp347,_tmp347 + 19},{_tmp348,_tmp348,_tmp348 + 16},{
_tmp349,_tmp349,_tmp349 + 20},{_tmp34A,_tmp34A,_tmp34A + 15},{_tmp34B,_tmp34B,
_tmp34B + 20},{_tmp34C,_tmp34C,_tmp34C + 15},{_tmp34D,_tmp34D,_tmp34D + 12},{
_tmp34E,_tmp34E,_tmp34E + 20},{_tmp34F,_tmp34F,_tmp34F + 19},{_tmp350,_tmp350,
_tmp350 + 20},{_tmp351,_tmp351,_tmp351 + 21},{_tmp352,_tmp352,_tmp352 + 21},{
_tmp353,_tmp353,_tmp353 + 12},{_tmp354,_tmp354,_tmp354 + 17},{_tmp355,_tmp355,
_tmp355 + 19},{_tmp356,_tmp356,_tmp356 + 14},{_tmp357,_tmp357,_tmp357 + 17},{
_tmp358,_tmp358,_tmp358 + 23},{_tmp359,_tmp359,_tmp359 + 13},{_tmp35A,_tmp35A,
_tmp35A + 14},{_tmp35B,_tmp35B,_tmp35B + 16},{_tmp35C,_tmp35C,_tmp35C + 16},{
_tmp35D,_tmp35D,_tmp35D + 8},{_tmp35E,_tmp35E,_tmp35E + 19},{_tmp35F,_tmp35F,
_tmp35F + 20},{_tmp360,_tmp360,_tmp360 + 14},{_tmp361,_tmp361,_tmp361 + 19},{
_tmp362,_tmp362,_tmp362 + 20},{_tmp363,_tmp363,_tmp363 + 11},{_tmp364,_tmp364,
_tmp364 + 22},{_tmp365,_tmp365,_tmp365 + 20},{_tmp366,_tmp366,_tmp366 + 23},{
_tmp367,_tmp367,_tmp367 + 20},{_tmp368,_tmp368,_tmp368 + 22},{_tmp369,_tmp369,
_tmp369 + 23},{_tmp36A,_tmp36A,_tmp36A + 24},{_tmp36B,_tmp36B,_tmp36B + 24},{
_tmp36C,_tmp36C,_tmp36C + 15},{_tmp36D,_tmp36D,_tmp36D + 20},{_tmp36E,_tmp36E,
_tmp36E + 22},{_tmp36F,_tmp36F,_tmp36F + 17},{_tmp370,_tmp370,_tmp370 + 20},{
_tmp371,_tmp371,_tmp371 + 26},{_tmp372,_tmp372,_tmp372 + 16},{_tmp373,_tmp373,
_tmp373 + 17},{_tmp374,_tmp374,_tmp374 + 15},{_tmp375,_tmp375,_tmp375 + 19},{
_tmp376,_tmp376,_tmp376 + 19},{_tmp377,_tmp377,_tmp377 + 25},{_tmp378,_tmp378,
_tmp378 + 26},{_tmp379,_tmp379,_tmp379 + 9},{_tmp37A,_tmp37A,_tmp37A + 20},{_tmp37B,
_tmp37B,_tmp37B + 17},{_tmp37C,_tmp37C,_tmp37C + 18},{_tmp37D,_tmp37D,_tmp37D + 11},{
_tmp37E,_tmp37E,_tmp37E + 12}};static short Cyc_yyr1[500]={0,134,135,135,135,135,
135,135,135,135,135,135,136,136,137,137,138,138,138,139,139,139,139,140,140,141,
142,143,144,145,145,145,145,145,145,145,145,145,146,146,147,147,147,147,147,147,
147,147,147,147,148,148,148,148,148,148,148,149,149,150,151,151,152,152,152,152,
152,152,153,153,154,154,154,154,154,154,154,154,154,154,154,154,154,154,154,154,
154,154,154,154,154,154,155,156,156,156,157,157,157,158,158,159,159,159,160,160,
160,161,161,162,162,163,163,164,164,165,166,166,167,167,168,169,169,169,169,169,
169,170,170,170,170,170,170,171,172,172,173,173,173,174,174,174,175,175,176,176,
177,177,177,177,178,178,178,179,179,180,180,181,181,182,182,182,182,182,182,182,
182,182,182,183,183,183,183,183,183,183,183,183,183,183,184,184,185,186,186,186,
187,187,188,188,188,189,189,189,190,190,190,191,191,192,192,192,192,193,193,194,
194,195,195,196,196,197,197,198,198,199,199,199,199,200,200,201,201,202,202,202,
203,204,204,205,205,206,206,206,206,207,207,207,207,208,209,209,210,210,211,211,
212,212,212,212,212,213,213,214,214,214,215,215,215,215,215,215,215,215,215,215,
215,216,216,216,216,216,216,216,217,217,218,219,219,220,220,221,221,221,221,221,
221,222,222,222,222,222,222,223,223,223,223,223,223,224,224,224,224,224,224,224,
224,224,224,224,224,224,224,225,225,225,225,225,225,225,225,226,227,227,228,228,
229,229,230,230,231,231,232,232,233,233,233,234,234,234,234,234,235,235,235,236,
236,236,237,237,237,237,238,238,239,239,239,239,239,239,239,240,241,242,242,242,
242,242,242,242,242,242,242,242,242,242,242,243,243,243,244,244,245,245,246,246,
246,247,247,248,248,249,249,249,250,250,250,250,250,250,250,250,250,250,250,251,
251,251,251,251,251,251,252,253,253,254,254,255,255,256,256,257,257,258,258,258,
259,259,259,259,259,260,260,260,261,261,261,262,262,262,262,263,263,264,264,264,
264,264,264,264,264,264,264,264,264,264,264,264,264,264,264,265,265,265,266,266,
266,266,266,266,266,266,266,266,267,267,267,267,267,267,267,267,267,268,269,269,
270,270,270,270,271,271,272,272,273,273,274,274,275,275};static short Cyc_yyr2[500]={
0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,6,4,9,6,8,1,2,
1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,
1,1,1,1,1,4,4,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,0,3,1,1,0,1,1,2,1,1,3,1,3,
3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,3,8,5,7,1,0,1,1,1,2,3,3,0,1,1,2,5,1,2,1,2,1,
3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,2,2,1,0,3,0,1,1,0,1,1,0,1,1,0,2,3,
5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,
1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,
3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,
5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,
6,1,1,1,3,1,1,3,4,4,5,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,
1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,
3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,4,4,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,1,3,2,5,
4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[1006]={0,18,50,
51,52,53,55,70,73,74,75,76,77,78,79,80,93,94,95,109,110,46,0,0,56,0,0,0,90,0,0,0,
142,0,0,0,490,203,492,491,493,0,0,71,0,186,186,185,1,0,16,0,0,17,0,40,48,42,68,
44,81,82,0,83,0,143,155,0,180,188,84,159,107,54,53,47,0,97,489,0,490,486,487,488,
0,363,0,0,0,0,226,0,365,366,25,27,143,143,274,0,0,0,0,143,0,0,0,0,0,0,0,143,0,
183,184,2,0,0,0,0,29,0,115,116,118,41,49,43,45,143,494,495,107,144,145,191,143,
38,143,0,19,143,0,143,168,156,181,190,189,194,143,69,0,54,101,0,99,0,490,372,0,0,
0,0,0,0,0,0,0,0,0,107,0,0,0,0,476,143,0,0,0,0,463,461,462,0,389,391,405,413,415,
417,419,421,423,426,431,434,437,441,0,443,464,475,474,0,373,371,32,0,0,107,0,0,0,
125,121,123,244,246,0,0,0,0,0,0,9,10,0,0,0,496,497,204,92,0,0,160,72,224,0,221,0,
3,0,5,0,30,0,0,143,20,0,143,113,0,106,193,0,192,0,0,0,0,0,0,0,0,0,0,0,0,490,277,
279,0,285,281,0,283,267,268,269,0,270,271,272,0,39,21,118,251,0,188,0,211,227,0,
0,207,205,0,196,57,195,0,0,102,98,0,0,367,0,0,143,451,143,407,441,0,408,409,0,0,
0,0,0,0,0,143,143,444,445,143,0,0,0,447,448,446,0,364,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,395,396,397,398,399,400,401,402,403,404,394,0,449,0,470,471,0,0,0,
478,0,107,379,380,0,377,0,228,0,0,0,143,247,219,0,126,131,127,129,122,124,0,143,
253,245,254,499,498,0,86,89,0,34,0,0,143,87,0,91,63,62,0,60,161,143,223,157,253,
225,169,170,143,85,187,26,0,28,0,117,119,230,229,22,104,114,0,0,0,133,134,136,0,
107,107,0,0,0,0,0,0,143,0,314,315,316,0,0,318,0,0,0,143,286,282,118,284,280,278,
143,167,162,188,212,143,0,0,218,206,213,164,0,0,143,207,166,197,58,108,0,103,412,
100,96,375,376,0,0,0,231,0,0,235,0,0,0,240,0,0,0,0,0,143,0,0,0,0,0,477,484,0,483,
390,414,0,416,418,420,422,424,425,429,430,427,428,432,433,435,436,438,439,440,
393,392,469,0,466,0,468,143,0,368,0,374,31,386,0,382,387,0,384,369,0,0,248,132,
128,130,188,0,0,207,0,255,143,0,143,266,250,0,0,0,0,0,0,0,0,0,125,0,107,143,0,
143,179,158,222,4,6,0,137,120,0,0,207,0,140,0,0,0,293,0,0,0,0,0,313,317,0,0,0,0,
276,143,23,252,163,205,143,215,0,0,208,0,165,211,199,197,182,0,7,0,450,0,0,0,243,
232,0,236,239,241,455,0,0,0,0,0,459,460,0,0,0,442,480,0,0,465,467,0,0,378,381,
383,370,0,249,220,257,188,262,0,256,0,188,0,0,207,0,33,36,0,88,0,0,0,0,59,61,0,
171,0,188,0,0,207,0,135,138,143,107,150,0,0,0,0,0,0,0,0,0,0,0,0,0,293,319,0,0,24,
207,0,216,214,0,207,143,198,0,8,0,0,0,242,233,237,0,0,0,0,410,411,454,482,0,485,
406,479,481,0,385,388,259,261,265,258,188,264,0,275,0,0,0,66,64,65,172,178,173,
188,175,0,177,0,141,152,151,0,0,146,287,293,0,0,0,143,0,0,321,322,324,326,328,
330,332,334,337,342,345,348,352,354,361,362,0,143,290,299,0,0,0,0,0,0,0,0,0,0,
320,273,201,217,209,200,205,13,0,0,453,452,0,238,456,143,0,472,0,260,263,0,37,0,
174,176,105,139,153,150,150,0,0,293,143,358,143,0,355,143,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,356,294,0,301,0,0,0,309,0,0,0,0,0,0,292,0,207,12,14,0,0,0,0,
473,35,0,143,149,148,288,289,0,0,0,0,293,295,325,0,327,329,331,333,335,336,340,
341,338,339,343,344,346,347,349,350,351,0,300,302,303,0,311,310,0,305,0,0,0,210,
202,15,0,0,143,0,0,291,357,0,353,296,0,143,304,312,306,307,0,234,457,0,67,154,0,
0,323,293,297,308,0,360,359,298,458,0,0,0};static short Cyc_yydefgoto[142]={1003,
48,652,847,49,50,276,51,437,52,439,53,138,54,55,497,215,423,424,216,58,233,217,
60,154,155,61,151,62,252,253,121,122,123,254,218,401,449,450,451,63,64,135,798,
799,800,65,452,66,428,67,68,69,113,149,258,303,649,581,70,582,493,643,485,489,
490,396,300,241,89,90,513,442,514,515,516,517,219,291,292,583,407,279,223,280,
281,282,283,284,728,285,286,808,809,810,811,812,813,814,815,816,817,818,819,820,
821,822,823,385,386,387,569,570,571,287,186,373,187,503,188,189,190,191,192,193,
194,195,196,197,198,199,200,201,202,531,532,203,204,72,848,234,411};static short
Cyc_yypact[1006]={2410,- -32768,- -32768,- -32768,- -32768,- 61,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,2915,187,1063,- -32768,- 4,- 34,8,27,137,55,63,- -32768,70,76,
97,- -32768,134,- -32768,- -32768,- -32768,125,935,213,138,251,251,- -32768,- -32768,
2285,- -32768,301,421,- -32768,1251,2915,2915,2915,- -32768,2915,- -32768,- -32768,
401,- -32768,531,2726,311,136,149,726,- -32768,- -32768,279,350,337,- -32768,- 4,375,
- -32768,2402,189,- -32768,- -32768,- -32768,5612,- -32768,372,444,2402,413,447,478,-
-32768,- 19,- -32768,- -32768,3303,3303,488,495,525,2285,2285,3303,540,5612,530,516,
158,520,530,3638,5612,- -32768,- -32768,- -32768,2285,2534,2285,2534,- -32768,576,
605,- -32768,2656,- -32768,- -32768,- -32768,- -32768,3437,- -32768,- -32768,279,-
-32768,- -32768,- 51,1643,- -32768,2726,1251,- -32768,3303,4198,3169,- -32768,311,-
-32768,- -32768,- -32768,118,3303,- -32768,2534,- -32768,617,627,633,- 4,10,- -32768,
2402,120,5737,622,5612,1024,632,634,642,652,654,279,661,664,5777,5777,- -32768,
1900,701,5860,5860,5860,- -32768,- -32768,- -32768,58,- -32768,- -32768,- 3,702,699,
704,708,573,114,743,344,275,- -32768,584,5860,98,- 37,- -32768,730,2152,759,- -32768,
- -32768,765,5612,279,2152,750,196,3370,3705,3370,535,- -32768,35,35,778,772,775,
782,- -32768,- -32768,81,787,238,- -32768,- -32768,- -32768,- -32768,62,773,- -32768,-
-32768,659,272,- -32768,793,- -32768,794,- -32768,795,- -32768,158,4281,2726,- -32768,
796,3504,- -32768,760,798,- -32768,- 4,- -32768,800,256,804,3768,806,810,802,803,
4367,3768,140,815,792,- -32768,- -32768,821,1512,1512,1251,1512,- -32768,- -32768,-
-32768,829,- -32768,- -32768,- -32768,240,- -32768,- -32768,832,801,56,726,825,847,-
-32768,- 31,830,831,500,837,- -32768,849,- -32768,56,850,- 4,- -32768,5612,853,-
-32768,844,845,1900,- -32768,3638,- -32768,- -32768,3854,- -32768,881,5612,5612,5612,
5612,5612,851,5612,3638,1900,- -32768,- -32768,1774,852,286,5612,- -32768,- -32768,-
-32768,5612,- -32768,5860,5612,5860,5860,5860,5860,5860,5860,5860,5860,5860,5860,
5860,5860,5860,5860,5860,5860,5860,5612,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,5612,- -32768,530,- -32768,- -32768,
5612,4450,530,- -32768,856,279,- -32768,- -32768,854,855,2402,- -32768,538,891,857,
3303,- -32768,865,867,- -32768,3705,3705,3705,- -32768,- -32768,4533,2982,423,-
-32768,328,- -32768,- -32768,- 31,- -32768,- -32768,861,- -32768,882,14,3303,- -32768,
883,- -32768,- -32768,866,870,874,- -32768,2793,- -32768,343,719,- -32768,- -32768,-
-32768,3638,- -32768,- -32768,- -32768,2285,- -32768,2285,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,949,5612,890,880,- -32768,884,455,279,157,5612,
5612,886,898,5612,981,1191,907,- -32768,- -32768,- -32768,572,973,- -32768,4616,5612,
3768,2026,- -32768,- -32768,2656,- -32768,- -32768,- -32768,3303,- -32768,- -32768,726,
- -32768,3638,900,312,- -32768,- -32768,892,- -32768,- 31,901,3571,831,- -32768,769,-
-32768,- -32768,2160,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,903,915,932,
- -32768,5612,530,- -32768,731,4281,230,- -32768,331,924,925,926,353,3638,362,920,
921,933,5652,- -32768,- -32768,927,937,- -32768,702,90,699,704,708,573,114,114,743,
743,743,743,344,344,275,275,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,219,
- -32768,928,- -32768,3303,4112,- -32768,2277,- -32768,- -32768,- -32768,2402,- -32768,
- -32768,934,946,- -32768,215,959,- -32768,- -32768,- -32768,- -32768,726,952,951,831,
953,328,3303,4699,3236,- -32768,- -32768,5612,962,972,978,35,970,714,966,62,3099,
967,279,3303,4782,3169,- -32768,343,- -32768,- -32768,- -32768,1117,- -32768,- -32768,
760,5612,831,- 4,982,404,469,5612,826,483,975,4865,4948,574,- -32768,- -32768,984,
986,979,499,- -32768,2726,- -32768,801,- -32768,990,3303,- -32768,980,- 31,- -32768,
989,- -32768,111,- -32768,769,- -32768,997,- -32768,2285,998,644,995,993,- -32768,-
-32768,3940,- -32768,- -32768,- -32768,- -32768,5612,1085,5612,1024,994,- -32768,-
-32768,998,999,4112,- -32768,- -32768,5612,5612,- -32768,- -32768,56,741,- -32768,-
-32768,- -32768,- -32768,2754,- -32768,- -32768,- -32768,726,- -32768,1001,- -32768,56,
726,1000,1002,831,518,- -32768,- -32768,1007,- -32768,5612,526,1009,1012,- -32768,-
-32768,1013,- -32768,56,726,1016,1017,831,1018,- -32768,- -32768,3437,279,848,3768,
1004,1019,817,1022,1005,3768,5612,5031,610,5114,619,5197,826,- -32768,1026,1030,-
-32768,831,329,- -32768,- -32768,1037,831,3638,- -32768,207,- -32768,1027,1033,5612,
- -32768,- -32768,- -32768,4281,568,1031,1041,- -32768,881,- -32768,- -32768,751,-
-32768,- -32768,- -32768,- -32768,4112,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,726,- -32768,1038,- -32768,1057,624,1060,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,726,- -32768,1042,- -32768,1053,- -32768,- -32768,- -32768,1056,- 4,645,
1140,826,1061,5900,1054,2026,5860,1051,- -32768,11,- -32768,1090,1047,742,791,146,
797,387,371,- -32768,- -32768,- -32768,- -32768,1092,5860,1774,- -32768,- -32768,570,
3768,586,5280,3768,587,5363,5446,662,1066,- -32768,- -32768,- -32768,- -32768,1067,-
-32768,990,- -32768,1069,669,- -32768,- -32768,147,- -32768,- -32768,3638,1151,-
-32768,4026,- -32768,- -32768,1076,- -32768,1074,- -32768,- -32768,- -32768,- -32768,
1070,69,848,3768,1079,826,1900,- -32768,3638,1072,- -32768,1381,5860,5612,5860,
5860,5860,5860,5860,5860,5860,5860,5860,5860,5860,5860,5860,5860,5860,5860,5860,
5612,- -32768,- -32768,1083,- -32768,3768,3768,589,- -32768,3768,3768,590,3768,591,
5529,- -32768,- 31,831,- -32768,- -32768,559,5612,1075,1077,- -32768,- -32768,1115,
3638,- -32768,- -32768,- -32768,- -32768,1109,1102,1113,5860,826,- -32768,702,208,699,
704,704,573,114,114,743,743,743,743,344,344,275,275,- -32768,- -32768,- -32768,210,
- -32768,- -32768,- -32768,3768,- -32768,- -32768,3768,- -32768,3768,3768,601,- -32768,
- -32768,- -32768,777,1107,3638,1108,606,- -32768,998,668,- -32768,- -32768,5612,1381,
- -32768,- -32768,- -32768,- -32768,3768,- -32768,- -32768,1110,- -32768,- -32768,1120,
1121,- -32768,826,- -32768,- -32768,1129,- -32768,- -32768,- -32768,- -32768,1226,1234,
- -32768};static short Cyc_yypgoto[142]={- -32768,486,- -32768,333,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- 33,- 91,- 11,- -32768,- -32768,0,657,- -32768,
85,- 201,1142,13,- -32768,- -32768,- 122,- -32768,- 130,- -32768,536,- -32768,- -32768,-
-32768,1020,1011,- 107,429,- -32768,- -32768,656,- -32768,- -32768,- -32768,26,- -32768,
- -32768,45,- 215,1200,- 365,99,- -32768,- -32768,1224,206,- -32768,- -32768,623,- 96,46,
- 123,- 367,358,628,631,- 396,- 427,- 104,- 390,- 120,- -32768,- 240,- 112,- 509,- 103,-
-32768,761,- 140,- 13,- 125,- 157,- 222,123,- -32768,- -32768,- -32768,- 15,- 260,- -32768,
- 557,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,182,1064,
- -32768,592,- -32768,- -32768,268,171,- -32768,- 146,- 350,- 89,- 334,- 332,- 340,936,-
319,- 322,- 196,- 324,- 317,127,450,553,- -32768,- -32768,- 378,- -32768,322,314,- 59,
1218,- 369,66};static short Cyc_yytable[6034]={56,558,256,131,239,537,555,240,534,
441,75,559,536,59,589,399,474,475,317,477,299,56,255,301,427,305,540,541,539,486,
546,547,137,250,59,310,239,334,73,548,549,327,108,607,125,126,127,298,128,56,140,
37,681,320,139,56,56,56,381,56,641,406,59,95,606,56,144,257,59,59,59,37,59,527,
796,797,321,487,59,421,342,391,430,220,221,57,382,109,606,211,228,137,631,36,879,
638,644,212,611,124,39,56,56,277,607,288,57,160,397,251,402,592,408,139,59,59,56,
56,56,56,96,343,161,289,56,278,593,139,647,59,59,59,59,408,57,880,56,59,56,97,57,
57,57,657,57,144,255,688,409,59,57,59,56,236,542,543,544,545,408,422,410,656,101,
502,765,59,480,146,340,409,102,375,376,377,507,341,508,495,- 147,838,432,259,103,
584,290,501,57,57,104,525,526,417,350,351,409,304,432,399,399,399,340,57,57,57,
57,91,410,584,484,57,677,600,378,105,379,692,398,288,297,380,37,57,312,57,887,
888,352,353,- 205,400,106,- 205,600,36,98,444,302,57,313,139,39,107,725,277,277,
871,277,469,720,99,56,100,42,561,111,36,470,340,889,890,395,159,39,59,719,278,
278,919,278,150,207,45,46,47,42,660,56,56,616,56,45,46,47,242,36,752,160,412,567,
59,59,39,59,290,418,76,- 227,239,37,- 227,277,161,502,498,36,394,337,338,339,110,
38,39,40,294,71,930,846,405,340,239,340,935,278,476,615,617,239,374,980,340,981,
779,687,56,678,57,77,93,429,94,410,661,311,488,511,92,59,479,340,792,340,512,185,
36,453,420,71,845,481,112,39,57,57,71,57,502,239,601,71,240,432,499,457,230,841,
458,130,979,239,844,573,71,433,668,710,634,461,239,432,434,156,150,468,93,432,
710,340,358,398,398,398,92,93,529,594,359,360,588,116,117,92,400,400,400,37,71,
71,41,57,756,443,640,71,141,996,142,599,143,605,625,71,71,71,71,41,680,153,56,
1001,56,585,340,586,137,587,335,842,610,663,41,59,139,59,71,410,602,488,603,152,
604,694,635,56,340,698,139,71,636,502,356,667,156,340,357,93,59,56,712,390,669,
239,716,92,157,717,550,551,552,205,59,443,697,397,895,519,520,521,239,36,482,56,
896,897,405,38,39,40,530,715,129,648,533,893,59,340,741,894,852,644,93,208,723,
57,746,57,429,93,92,118,119,756,767,553,395,92,115,467,403,488,404,601,206,939,
940,554,936,453,57,968,938,530,432,36,431,432,761,757,209,38,39,40,57,71,942,943,
941,900,948,949,431,564,447,455,568,580,459,950,951,762,340,133,134,335,567,588,
57,724,226,227,210,518,795,71,340,522,633,524,224,335,222,729,288,243,244,245,
246,605,297,993,340,494,535,315,255,318,318,739,756,934,740,395,156,225,139,331,
332,742,231,340,318,318,318,235,232,56,780,783,237,306,361,239,530,229,784,530,
565,556,59,340,403,318,404,56,348,349,674,36,45,46,47,703,648,38,39,40,59,876,
757,362,363,364,365,366,367,368,369,370,371,340,628,340,735,340,247,340,853,443,
901,488,637,131,944,945,946,947,372,340,340,453,340,340,340,93,903,907,93,959,
962,964,453,92,340,453,92,920,248,433,832,986,57,340,995,915,991,618,619,835,307,
622,340,626,861,443,931,340,932,308,57,316,632,431,231,751,431,309,683,768,232,
322,684,323,71,868,71,757,869,36,696,324,318,775,431,38,39,40,231,992,431,325,
912,326,232,340,403,714,426,917,328,788,918,329,45,46,47,344,689,16,17,18,147,
148,530,318,277,318,318,318,318,318,318,318,318,318,318,318,318,318,318,318,318,
318,705,706,707,71,278,36,336,239,354,355,974,38,39,40,345,56,576,577,578,443,
346,989,403,994,426,347,760,383,59,658,659,804,805,443,277,801,766,726,727,769,
770,828,796,797,78,388,36,699,131,856,857,389,38,39,40,393,278,568,883,884,885,
886,891,892,413,447,93,56,414,448,93,45,46,47,92,987,340,415,92,425,59,732,734,
926,927,773,416,318,79,419,776,435,436,438,445,454,463,824,464,465,57,472,431,
158,81,456,82,83,789,460,39,462,608,431,609,410,431,443,78,721,471,758,806,877,
85,86,473,478,87,483,443,807,249,484,88,182,491,277,183,184,492,899,902,496,41,
906,505,506,500,488,318,504,57,342,563,71,523,560,528,278,562,782,79,572,574,575,
590,318,591,595,56,596,858,598,566,651,597,158,81,613,82,83,928,59,39,863,612,
829,831,93,834,620,837,614,511,621,84,92,85,86,623,512,87,627,629,639,642,645,88,
653,851,952,953,954,654,957,958,443,655,960,961,36,963,664,665,666,670,671,39,93,
41,672,685,675,679,36,676,92,162,163,42,38,39,40,41,686,45,46,47,978,37,78,130,
318,447,57,690,691,700,693,45,46,47,185,166,167,168,169,701,702,704,982,708,711,
983,171,984,985,743,722,730,736,737,172,173,738,297,174,175,905,78,745,909,911,
749,673,753,754,997,759,763,802,867,827,777,764,318,774,778,781,36,81,176,82,83,
785,318,39,786,787,790,319,839,791,793,803,840,750,843,177,335,826,178,849,79,
179,854,937,180,850,855,181,182,860,859,183,184,862,864,80,81,865,82,83,866,955,
39,870,872,875,878,881,882,898,913,921,914,916,84,966,85,86,923,924,87,925,970,
929,933,956,88,971,972,2,3,4,74,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,973,36,
21,162,163,975,976,38,39,40,977,988,990,1004,998,22,78,23,164,130,447,1005,165,
24,999,1000,45,46,47,166,167,168,169,1002,27,28,29,170,969,238,171,874,709,794,
318,32,33,34,172,173,35,446,174,175,145,440,718,114,748,967,744,747,318,392,662,
772,825,132,0,538,0,0,0,0,0,36,81,176,82,83,37,38,39,40,41,624,0,0,0,0,0,0,0,0,
177,0,43,275,0,0,179,0,0,180,0,0,181,182,0,0,183,184,0,0,0,0,318,0,318,318,318,
318,318,318,318,318,318,318,318,318,318,318,318,318,318,36,0,0,0,0,0,0,39,0,0,
120,0,0,0,0,0,0,0,0,42,0,0,0,0,0,45,46,47,0,0,0,0,0,0,0,318,2,3,4,74,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,726,727,21,162,163,260,0,261,262,263,264,265,266,
267,268,22,78,23,164,269,0,0,165,24,270,0,0,318,0,166,167,168,169,0,27,28,29,170,
0,271,171,0,0,0,0,32,33,34,172,173,35,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,272,81,176,82,83,37,38,39,40,41,273,136,- 293,0,0,0,0,0,0,177,0,43,275,
0,0,179,0,0,180,0,0,181,182,0,0,183,184,2,3,4,74,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,162,163,260,0,261,262,263,264,265,266,267,268,22,78,23,164,269,0,
0,165,24,270,0,0,- 143,- 143,166,167,168,169,0,27,28,29,170,0,271,171,0,0,0,0,32,
33,34,172,173,35,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,272,81,176,
82,83,37,38,39,40,41,273,136,0,0,0,0,0,0,0,177,0,43,275,0,0,179,0,0,180,0,0,181,
182,0,0,183,184,2,3,4,74,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,162,163,
260,0,261,262,263,264,265,266,267,268,22,78,23,164,269,0,0,165,24,270,0,0,0,0,
166,167,168,169,0,27,28,29,170,0,271,171,0,0,0,0,32,33,34,172,173,35,0,174,175,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,272,81,176,82,83,37,38,39,40,41,273,136,
274,0,0,0,0,0,0,177,0,43,275,0,0,179,0,0,180,0,0,181,182,0,0,183,184,2,3,4,74,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,162,163,260,0,261,262,263,264,265,
266,267,268,22,78,23,164,269,0,0,165,24,270,0,0,0,0,166,167,168,169,0,27,28,29,
170,0,271,171,0,0,0,0,32,33,34,172,173,35,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,272,81,176,82,83,37,38,39,40,41,273,136,0,0,0,0,0,0,0,177,0,43,275,
0,0,179,0,0,180,0,0,181,182,0,0,183,184,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,0,162,163,0,0,0,0,0,0,0,0,0,0,22,78,0,164,0,0,0,165,0,0,0,0,0,0,166,167,168,
169,0,27,28,0,170,0,0,171,0,0,0,0,32,33,0,172,173,35,0,174,175,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,37,38,39,40,41,0,333,0,0,0,0,0,0,0,177,0,
43,275,0,0,179,0,0,180,0,0,181,182,0,0,183,184,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,162,163,0,0,0,0,0,0,0,0,0,0,22,78,0,164,0,0,0,165,0,0,0,0,0,0,166,
167,168,169,0,27,28,0,170,0,0,171,0,0,0,0,32,33,0,172,173,35,0,174,175,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,37,38,39,40,41,0,0,0,0,0,0,0,0,0,
177,0,43,275,0,0,179,0,0,180,0,0,181,182,0,0,183,184,- 11,1,0,2,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,78,0,0,0,0,0,0,22,0,23,0,0,0,650,
0,24,0,25,26,- 143,- 143,0,0,0,0,0,27,28,29,0,0,0,0,0,0,30,31,32,33,34,0,0,35,0,79,
0,0,0,0,0,0,0,0,0,0,0,384,0,0,158,81,0,82,83,0,0,39,36,0,0,0,0,37,38,39,40,41,0,
84,- 11,85,86,0,0,87,0,42,0,43,44,88,0,45,46,47,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,78,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,
25,26,- 143,- 143,0,0,0,0,0,27,28,29,0,0,0,0,0,0,30,31,32,33,34,0,0,35,0,79,0,0,0,
0,0,0,0,0,0,0,0,682,0,0,158,81,0,82,83,0,0,39,36,0,0,0,0,37,38,39,40,41,0,84,- 11,
85,86,0,0,87,0,42,0,43,44,88,0,45,46,47,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,21,0,0,0,0,0,78,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,-
143,- 143,0,0,0,0,0,27,28,29,0,0,0,0,0,0,30,31,32,33,34,0,0,35,0,79,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,158,81,0,82,83,0,0,39,36,0,0,0,0,37,38,39,40,41,0,84,0,85,86,0,0,
87,0,42,0,43,44,88,0,45,46,47,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,- 143,- 143,0,0,0,0,
0,27,28,29,0,0,0,0,0,0,30,31,32,33,34,0,0,35,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,36,0,0,0,0,37,38,39,40,41,0,0,- 11,0,0,0,0,0,0,42,0,43,44,0,0,45,46,
47,2,3,4,74,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,
0,0,22,0,23,0,0,0,0,0,24,0,0,0,- 143,- 143,0,0,0,0,0,27,28,29,0,0,0,0,0,0,0,0,32,
33,34,0,0,35,2,3,4,74,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,
0,0,37,38,0,40,41,22,136,23,0,249,0,0,0,24,0,0,43,44,0,0,0,0,0,0,27,28,29,0,0,0,
0,0,0,0,78,32,33,34,0,0,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,
0,0,0,0,0,0,0,0,37,38,22,40,41,0,136,0,79,0,0,0,0,0,0,0,43,44,0,0,771,27,28,158,
81,0,82,83,0,0,39,0,32,33,0,0,0,35,0,0,511,0,84,0,85,86,0,512,87,0,0,0,0,0,88,0,
0,0,0,295,0,0,36,0,0,0,0,37,38,39,40,41,297,0,0,0,0,0,0,403,0,426,- 205,43,44,-
205,0,45,46,47,2,3,4,74,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,
0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,24,0,0,0,- 143,- 143,0,0,0,0,0,27,28,0,0,0,0,0,0,
0,0,0,32,33,0,0,0,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,
0,0,0,0,0,37,38,22,40,41,0,0,0,0,0,0,0,0,0,0,0,43,44,0,0,0,27,28,0,0,0,0,0,0,0,0,
0,32,33,0,0,0,35,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,295,0,0,0,0,0,0,0,37,
38,0,40,41,297,0,0,0,0,0,0,403,0,404,- 205,43,44,- 205,0,45,46,47,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,0,-
143,- 143,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,0,0,0,0,0,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,0,0,0,36,0,0,0,0,37,38,39,40,41,22,0,0,0,0,0,0,0,
0,447,0,43,44,0,0,45,46,47,0,27,28,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,0,0,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,295,0,0,296,0,0,0,0,37,38,22,40,41,297,0,
0,0,0,0,0,0,0,0,- 205,43,44,- 205,0,0,27,28,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,0,0,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,295,0,0,0,0,0,0,0,37,38,22,40,41,
297,0,0,0,0,0,0,0,0,0,- 205,43,44,- 205,0,0,27,28,0,0,213,0,0,0,0,0,0,32,33,0,0,0,
35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,37,38,22,
40,41,0,214,0,0,0,0,0,0,0,- 143,- 143,43,44,0,0,0,27,28,0,0,0,0,0,0,0,0,0,32,33,0,
0,0,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,37,
38,22,40,41,0,0,0,0,0,0,0,0,0,0,0,43,44,0,0,0,27,28,0,0,0,0,0,0,0,0,0,32,33,0,0,
0,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,37,38,
22,40,41,0,0,- 111,0,0,0,0,0,0,0,0,43,44,0,0,0,27,28,0,0,0,0,0,0,0,0,0,32,33,0,0,
0,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,37,38,
22,40,41,0,0,- 112,0,0,0,0,0,0,0,0,43,44,0,0,0,27,28,0,0,0,0,0,0,0,0,0,32,33,0,0,
0,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,646,0,0,0,0,0,0,0,37,
38,22,40,41,0,0,0,0,0,0,0,0,0,0,0,43,44,0,0,0,27,28,0,0,0,0,0,0,0,0,0,32,33,0,0,
0,35,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,37,38,
22,40,41,0,0,0,0,0,0,0,0,0,- 143,- 143,43,44,0,0,0,27,28,0,0,0,0,0,0,0,0,0,32,33,0,
0,0,35,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,162,163,260,0,261,262,263,264,265,266,267,
268,0,78,37,164,269,0,41,165,0,270,0,0,0,0,166,167,168,169,43,44,0,0,170,0,271,
171,0,0,0,0,0,0,0,172,173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
272,81,176,82,83,0,0,39,0,0,273,136,0,0,162,163,0,0,0,177,0,509,178,0,0,179,0,78,
180,164,0,181,182,165,0,183,184,0,0,0,166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,
0,0,0,0,172,173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,
82,83,0,0,39,0,0,0,319,510,0,162,163,0,511,0,177,0,0,178,0,512,179,0,78,180,164,
0,181,182,165,0,183,184,0,0,0,166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,
172,173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,
0,39,0,0,0,319,755,0,162,163,0,511,0,177,0,0,178,0,512,179,0,78,180,164,0,181,
182,165,0,183,184,0,0,0,166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,
173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,
0,0,0,319,922,0,162,163,0,511,0,177,0,0,178,0,512,179,0,78,180,164,0,181,182,165,
0,183,184,0,0,0,166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,
174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,
319,0,0,162,163,0,511,0,177,0,0,178,0,512,179,0,78,180,164,0,181,182,165,0,183,
184,0,0,0,166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,174,175,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,162,163,0,
0,0,0,0,293,177,0,0,178,0,78,179,164,0,180,0,165,181,182,0,0,183,184,166,167,168,
169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,319,0,0,162,163,0,0,0,177,0,0,
178,0,0,179,0,78,180,164,0,181,182,165,0,183,184,0,0,0,166,167,168,169,0,0,0,0,
170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,36,81,176,82,83,0,0,39,0,0,466,162,163,0,0,0,0,0,0,177,0,0,178,0,78,179,
164,0,180,0,165,181,182,0,0,183,184,166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,
0,0,0,172,173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,
83,0,0,39,0,0,0,162,163,0,0,0,0,0,0,177,557,0,178,0,78,179,164,0,180,0,165,181,
182,0,0,183,184,166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,
174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,
162,163,0,0,0,0,0,579,177,0,0,178,0,78,179,164,0,180,0,165,181,182,0,0,183,184,
166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,174,175,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,162,163,0,0,0,0,0,0,
177,630,0,178,0,78,179,164,0,180,0,165,181,182,0,0,183,184,166,167,168,169,0,0,0,
0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,162,163,0,0,0,0,0,695,177,0,0,178,0,78,
179,164,0,180,0,165,181,182,0,0,183,184,166,167,168,169,0,0,0,0,170,0,0,171,0,0,
0,0,0,0,0,172,173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,
176,82,83,0,0,39,0,0,0,162,163,0,0,0,0,0,713,177,0,0,178,0,78,179,164,0,180,0,
165,181,182,0,0,183,184,166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,
173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,
0,0,731,162,163,0,0,0,0,0,0,177,0,0,178,0,78,179,164,0,180,0,165,181,182,0,0,183,
184,166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,174,175,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,733,162,163,0,0,0,
0,0,0,177,0,0,178,0,78,179,164,0,180,0,165,181,182,0,0,183,184,166,167,168,169,0,
0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,162,163,0,0,0,0,0,0,177,830,0,178,0,
78,179,164,0,180,0,165,181,182,0,0,183,184,166,167,168,169,0,0,0,0,170,0,0,171,0,
0,0,0,0,0,0,172,173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,
176,82,83,0,0,39,0,0,0,162,163,0,0,0,0,0,0,177,833,0,178,0,78,179,164,0,180,0,
165,181,182,0,0,183,184,166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,
173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,
0,0,836,162,163,0,0,0,0,0,0,177,0,0,178,0,78,179,164,0,180,0,165,181,182,0,0,183,
184,166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,174,175,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,162,163,0,0,0,0,
0,0,177,904,0,178,0,78,179,164,0,180,0,165,181,182,0,0,183,184,166,167,168,169,0,
0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,0,0,0,162,163,0,0,0,0,0,0,177,908,0,178,0,
78,179,164,0,180,0,165,181,182,0,0,183,184,166,167,168,169,0,0,0,0,170,0,0,171,0,
0,0,0,0,0,0,172,173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,
176,82,83,0,0,39,0,0,0,162,163,0,0,0,0,0,0,177,910,0,178,0,78,179,164,0,180,0,
165,181,182,0,0,183,184,166,167,168,169,0,0,0,0,170,0,0,171,0,0,0,0,0,0,0,172,
173,0,0,174,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,81,176,82,83,0,0,39,
0,0,0,162,163,0,0,0,0,0,0,177,965,0,178,0,78,179,164,0,180,0,165,181,182,0,0,183,
184,166,167,168,169,0,0,0,0,170,0,0,171,0,0,162,163,0,0,0,172,173,0,0,174,175,0,
0,78,0,0,0,0,0,0,0,0,0,0,0,0,166,167,168,169,0,0,36,81,176,82,83,171,0,39,0,0,0,
0,0,172,173,0,0,174,175,177,0,0,178,0,0,179,0,0,180,0,0,181,182,0,0,183,184,0,0,
0,36,81,176,82,83,0,0,39,0,0,0,673,0,162,163,0,0,0,0,177,0,0,178,0,0,179,78,0,
180,0,0,181,182,0,0,183,184,0,0,166,167,168,169,0,0,0,0,0,0,0,171,0,0,162,163,0,
0,0,172,173,0,0,174,175,0,0,78,0,0,0,0,0,0,0,0,0,0,0,0,166,167,168,169,0,0,36,81,
176,82,83,171,0,39,0,0,0,0,0,172,173,0,0,174,175,314,0,0,178,0,0,179,0,0,180,0,0,
181,182,0,0,183,184,0,0,0,36,81,176,82,83,0,0,39,0,0,0,162,163,0,0,0,0,0,0,330,0,
0,178,0,78,179,0,0,180,0,0,181,182,0,0,183,184,166,167,168,169,0,0,0,0,0,0,0,171,
0,0,162,163,0,0,0,172,173,0,0,174,175,0,0,78,0,0,0,0,0,0,0,0,0,0,0,0,166,167,168,
169,0,0,36,81,176,82,83,171,0,39,0,0,0,0,0,172,173,0,0,174,175,177,0,0,178,0,0,
179,0,0,180,0,0,181,182,0,0,183,184,0,0,0,36,81,176,82,83,0,0,39,0,0,0,0,0,0,0,0,
0,0,0,873,0,0,178,0,0,179,0,0,180,0,0,181,182,0,0,183,184};static short Cyc_yycheck[
6034]={0,379,132,62,111,345,375,111,342,249,21,380,344,0,410,216,276,277,164,279,
143,21,129,143,239,150,348,349,347,60,354,355,65,124,21,157,143,177,99,356,357,
171,42,433,55,56,57,143,59,49,65,102,561,165,65,55,56,57,95,59,487,218,49,97,429,
65,66,118,55,56,57,102,59,333,5,6,165,108,65,17,83,211,239,96,97,0,123,42,453,
108,103,124,470,97,83,485,492,116,448,54,104,101,102,136,494,138,21,97,215,124,
217,97,77,124,101,102,116,117,118,119,112,124,112,138,124,136,112,138,495,116,
117,118,119,77,49,124,136,124,138,112,55,56,57,512,59,145,253,574,113,136,65,138,
152,108,350,351,352,353,77,97,125,511,107,309,673,152,110,68,110,113,107,73,74,
75,314,117,316,300,109,736,239,135,112,405,139,307,101,102,112,329,330,110,78,79,
113,149,255,398,399,400,110,116,117,118,119,23,125,429,97,124,120,426,114,116,
116,582,216,250,107,121,102,136,102,138,78,79,112,113,117,216,96,120,447,97,97,
250,118,152,118,250,104,116,620,276,277,802,279,107,615,112,250,114,116,383,116,
97,116,110,112,113,214,79,104,250,614,276,277,120,279,112,88,122,123,124,116,515,
276,277,121,279,122,123,124,112,97,654,97,221,391,276,277,104,279,248,228,108,
107,404,102,110,333,112,448,303,97,109,179,180,181,96,103,104,105,142,0,872,109,
218,110,426,110,878,333,278,454,455,433,200,120,110,120,698,117,333,115,250,22,
23,239,25,125,111,160,297,114,23,333,107,110,716,110,121,84,97,255,117,42,747,
292,108,104,276,277,49,279,511,473,426,54,473,429,305,116,105,741,119,62,934,485,
746,393,67,110,523,599,476,263,494,447,117,76,112,269,79,453,610,110,122,398,399,
400,79,88,117,417,130,131,407,107,108,88,398,399,400,102,101,102,106,333,659,249,
109,108,112,981,114,426,116,428,462,116,117,118,119,106,560,99,437,995,439,112,
110,114,476,116,177,117,447,117,106,437,462,439,139,125,112,410,114,108,116,585,
476,462,110,587,476,152,480,614,125,117,157,110,129,160,462,476,602,210,117,587,
604,160,108,604,358,359,360,116,476,319,587,599,122,323,324,325,604,97,293,500,
130,131,404,103,104,105,336,604,108,497,340,125,500,110,638,129,757,914,205,107,
117,437,646,439,426,212,205,107,108,770,677,361,487,212,49,268,114,492,116,599,
97,882,883,373,879,447,462,915,881,379,610,97,239,613,667,659,110,103,104,105,
476,248,885,886,884,826,891,892,255,388,116,258,391,403,261,893,894,667,110,49,
50,314,686,584,500,117,101,102,111,322,721,278,110,326,472,328,102,330,111,117,
634,116,117,118,119,606,107,977,110,110,343,162,720,164,165,117,857,878,634,574,
307,97,634,174,175,639,97,110,179,180,181,116,103,634,117,110,117,152,55,747,470,
102,117,473,107,378,634,110,114,200,116,652,80,81,528,97,122,123,124,594,648,103,
104,105,652,806,770,84,85,86,87,88,89,90,91,92,93,110,107,110,107,110,107,110,
117,515,117,642,483,749,887,888,889,890,111,110,110,599,110,110,110,388,117,117,
391,117,117,117,610,388,110,613,391,854,110,110,107,117,634,110,981,845,117,456,
457,107,110,460,110,462,107,561,873,110,875,109,652,116,471,426,97,98,429,111,
563,680,103,116,567,116,437,107,439,857,110,97,586,116,309,694,447,103,104,105,
97,98,453,116,107,116,103,110,114,603,116,107,116,712,110,116,122,123,124,82,579,
17,18,19,63,64,620,342,826,344,345,346,347,348,349,350,351,352,353,354,355,356,
357,358,359,360,97,98,99,500,826,97,116,925,76,77,925,103,104,105,126,826,398,
399,400,659,127,972,114,980,116,128,666,108,826,109,110,25,26,673,878,723,676,22,
23,109,110,729,5,6,38,97,97,590,918,109,110,97,103,104,105,116,878,686,127,128,
80,81,76,77,97,116,563,878,107,120,567,122,123,124,563,109,110,113,567,117,878,
624,625,868,869,690,115,448,82,113,695,109,109,109,109,108,97,726,107,107,826,
120,599,97,98,116,100,101,713,116,104,116,437,610,439,125,613,757,38,616,116,664,
116,807,118,119,116,109,122,115,770,125,111,97,128,129,117,981,132,133,120,825,
830,117,106,833,113,113,109,914,511,109,878,83,110,652,116,112,117,981,117,704,
82,117,110,109,116,528,97,97,981,116,777,110,94,500,117,97,98,110,100,101,870,
981,104,790,107,730,731,686,733,116,735,120,114,108,116,686,118,119,30,121,122,
107,42,116,125,117,128,117,753,895,896,897,110,903,904,857,97,907,908,97,910,110,
110,110,117,117,104,726,106,109,109,117,117,97,110,726,25,26,116,103,104,105,106,
110,122,123,124,933,102,38,749,614,116,981,115,117,107,117,122,123,124,806,51,52,
53,54,107,102,111,959,117,117,962,62,964,965,109,108,116,108,107,70,71,117,107,
74,75,832,38,113,835,836,108,108,112,115,986,25,117,108,799,109,115,117,667,117,
117,113,97,98,99,100,101,117,677,104,117,117,115,108,107,117,117,117,107,652,102,
116,873,120,119,117,82,122,116,880,125,117,110,128,129,97,117,132,133,98,117,97,
98,109,100,101,109,898,104,28,108,116,120,82,126,82,109,25,110,109,116,912,118,
119,107,110,122,116,919,109,117,107,128,117,116,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,98,97,24,25,26,109,117,103,104,105,110,117,117,0,117,37,38,39,
40,918,116,0,44,45,117,117,122,123,124,51,52,53,54,117,56,57,58,59,918,110,62,
804,598,720,807,67,68,69,70,71,72,253,74,75,67,248,613,46,648,914,642,646,825,
212,516,686,726,62,- 1,346,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,102,103,104,105,106,
107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,
- 1,132,133,- 1,- 1,- 1,- 1,879,- 1,881,882,883,884,885,886,887,888,889,890,891,892,
893,894,895,896,897,97,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
116,- 1,- 1,- 1,- 1,- 1,122,123,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,933,3,4,5,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,
40,41,- 1,- 1,44,45,46,- 1,- 1,980,- 1,51,52,53,54,- 1,56,57,58,59,- 1,61,62,- 1,- 1,- 1,-
1,67,68,69,70,71,72,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,97,98,99,100,101,102,103,104,105,106,107,108,109,- 1,- 1,- 1,- 1,- 1,- 1,
116,- 1,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,
38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,54,- 1,56,57,58,59,- 1,61,62,- 1,- 1,
- 1,- 1,67,68,69,70,71,72,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,102,103,104,105,106,107,108,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
116,- 1,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,
38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,- 1,- 1,51,52,53,54,- 1,56,57,58,59,- 1,61,62,- 1,- 1,
- 1,- 1,67,68,69,70,71,72,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,102,103,104,105,106,107,108,109,- 1,- 1,- 1,- 1,- 1,-
1,116,- 1,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,
37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,- 1,- 1,51,52,53,54,- 1,56,57,58,59,- 1,61,62,- 1,
- 1,- 1,- 1,67,68,69,70,71,72,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,102,103,104,105,106,107,108,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,116,- 1,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,
40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,56,57,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,
67,68,- 1,70,71,72,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,97,98,99,100,101,102,103,104,105,106,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,
118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,
44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,- 1,56,57,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,67,68,- 1,70,
71,72,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,
98,99,100,101,102,103,104,105,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,118,119,- 1,-
1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,
- 1,43,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,65,66,67,68,
69,- 1,- 1,72,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,97,98,- 1,100,101,- 1,
- 1,104,97,- 1,- 1,- 1,- 1,102,103,104,105,106,- 1,116,109,118,119,- 1,- 1,122,- 1,116,- 1,
118,119,128,- 1,122,123,124,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,
48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,65,66,67,68,69,- 1,- 1,72,- 1,82,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,97,98,- 1,100,101,- 1,- 1,104,97,- 1,- 1,- 1,
- 1,102,103,104,105,106,- 1,116,109,118,119,- 1,- 1,122,- 1,116,- 1,118,119,128,- 1,122,
123,124,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,
- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,
- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,65,66,67,68,69,- 1,- 1,72,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,100,101,- 1,- 1,104,97,- 1,- 1,- 1,- 1,102,103,104,
105,106,- 1,116,- 1,118,119,- 1,- 1,122,- 1,116,- 1,118,119,128,- 1,122,123,124,1,- 1,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,
- 1,- 1,- 1,- 1,- 1,65,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,102,103,104,105,106,- 1,- 1,109,- 1,- 1,
- 1,- 1,- 1,- 1,116,- 1,118,119,- 1,- 1,122,123,124,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,
- 1,45,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,69,- 1,
- 1,72,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,102,103,- 1,105,106,37,108,39,- 1,111,- 1,- 1,- 1,45,- 1,- 1,118,119,- 1,- 1,- 1,- 1,
- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,67,68,69,- 1,- 1,72,- 1,- 1,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,37,105,
106,- 1,108,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,119,- 1,- 1,94,56,57,97,98,- 1,100,101,- 1,
- 1,104,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,114,- 1,116,- 1,118,119,- 1,121,122,- 1,- 1,- 1,- 1,-
1,128,- 1,- 1,- 1,- 1,94,- 1,- 1,97,- 1,- 1,- 1,- 1,102,103,104,105,106,107,- 1,- 1,- 1,- 1,- 1,
- 1,114,- 1,116,117,118,119,120,- 1,122,123,124,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,45,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,
- 1,72,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,102,103,37,105,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,119,- 1,- 1,- 1,
56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,105,106,107,
- 1,- 1,- 1,- 1,- 1,- 1,114,- 1,116,117,118,119,120,- 1,122,123,124,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,
- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,-
1,97,- 1,- 1,- 1,- 1,102,103,104,105,106,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,118,119,-
1,- 1,122,123,124,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,97,- 1,- 1,- 1,- 1,102,
103,37,105,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,117,118,119,120,- 1,- 1,56,57,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,37,105,106,107,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,117,118,119,120,- 1,- 1,56,57,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,-
1,- 1,72,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,102,103,37,105,106,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,50,118,119,- 1,- 1,-
1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,37,105,106,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,119,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
67,68,- 1,- 1,- 1,72,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,37,105,106,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,
119,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,37,
105,106,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,119,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,-
1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,37,105,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,118,119,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
102,103,37,105,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,50,118,119,- 1,- 1,- 1,56,57,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,102,40,41,- 1,106,44,- 1,46,- 1,- 1,- 1,
- 1,51,52,53,54,118,119,- 1,- 1,59,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,
- 1,- 1,104,- 1,- 1,107,108,- 1,- 1,25,26,- 1,- 1,- 1,116,- 1,32,119,- 1,- 1,122,- 1,38,125,
40,- 1,128,129,44,- 1,132,133,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,-
1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,108,109,- 1,25,26,- 1,114,- 1,
116,- 1,- 1,119,- 1,121,122,- 1,38,125,40,- 1,128,129,44,- 1,132,133,- 1,- 1,- 1,51,52,53,
54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,
- 1,108,109,- 1,25,26,- 1,114,- 1,116,- 1,- 1,119,- 1,121,122,- 1,38,125,40,- 1,128,129,
44,- 1,132,133,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,108,109,- 1,25,26,- 1,114,- 1,116,- 1,- 1,119,- 1,
121,122,- 1,38,125,40,- 1,128,129,44,- 1,132,133,- 1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,
59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,108,- 1,- 1,
25,26,- 1,114,- 1,116,- 1,- 1,119,- 1,121,122,- 1,38,125,40,- 1,128,129,44,- 1,132,133,-
1,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,
75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,
101,- 1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,115,116,- 1,- 1,119,- 1,38,122,40,- 1,
125,- 1,44,128,129,- 1,- 1,132,133,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,108,- 1,- 1,25,26,- 1,- 1,- 1,116,- 1,- 1,
119,- 1,- 1,122,- 1,38,125,40,- 1,128,129,44,- 1,132,133,- 1,- 1,- 1,51,52,53,54,- 1,- 1,-
1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,107,25,
26,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,119,- 1,38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,
133,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,-
1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,- 1,38,122,40,- 1,125,- 1,
44,128,129,- 1,- 1,132,133,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,-
1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,115,116,- 1,- 1,119,- 1,
38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,
116,117,- 1,119,- 1,38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,51,52,53,54,- 1,- 1,
- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,25,26,
- 1,- 1,- 1,- 1,- 1,115,116,- 1,- 1,119,- 1,38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,
51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,
104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,115,116,- 1,- 1,119,- 1,38,122,40,- 1,125,- 1,44,
128,129,- 1,- 1,132,133,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
97,98,99,100,101,- 1,- 1,104,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,119,- 1,38,
122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,- 1,- 1,
116,- 1,- 1,119,- 1,38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,51,52,53,54,- 1,- 1,
- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,25,26,
- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,- 1,38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,
51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,
104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,- 1,38,122,40,- 1,125,- 1,44,
128,129,- 1,- 1,132,133,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
97,98,99,100,101,- 1,- 1,104,- 1,- 1,107,25,26,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,119,- 1,38,
122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,
116,117,- 1,119,- 1,38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,51,52,53,54,- 1,- 1,
- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,25,26,
- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,- 1,38,122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,
51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,
104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,- 1,38,122,40,- 1,125,- 1,44,
128,129,- 1,- 1,132,133,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,- 1,38,
122,40,- 1,125,- 1,44,128,129,- 1,- 1,132,133,51,52,53,54,- 1,- 1,- 1,- 1,59,- 1,- 1,62,- 1,
- 1,25,26,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
51,52,53,54,- 1,- 1,97,98,99,100,101,62,- 1,104,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,
116,- 1,- 1,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,- 1,- 1,- 1,97,98,99,
100,101,- 1,- 1,104,- 1,- 1,- 1,108,- 1,25,26,- 1,- 1,- 1,- 1,116,- 1,- 1,119,- 1,- 1,122,38,-
1,125,- 1,- 1,128,129,- 1,- 1,132,133,- 1,- 1,51,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,- 1,-
1,25,26,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
51,52,53,54,- 1,- 1,97,98,99,100,101,62,- 1,104,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,
116,- 1,- 1,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,- 1,- 1,- 1,97,98,99,
100,101,- 1,- 1,104,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,119,- 1,38,122,- 1,- 1,
125,- 1,- 1,128,129,- 1,- 1,132,133,51,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,- 1,- 1,25,26,
- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,
54,- 1,- 1,97,98,99,100,101,62,- 1,104,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,116,- 1,- 1,
119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,- 1,- 1,- 1,97,98,99,100,101,- 1,
- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,- 1,- 1,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,
128,129,- 1,- 1,132,133};char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible\000";
char Cyc_Yystack_overflow[21]="\000\000\000\000Yystack_overflow\000";void Cyc_yyerror(
struct _dyneither_ptr);int Cyc_yylex(struct Cyc_Lexing_lexbuf*);static int Cyc_yychar=(
int)'\000';union Cyc_YYSTYPE_union Cyc_yylval=(union Cyc_YYSTYPE_union)((struct Cyc_YYINITIALSVAL_struct){
57});static int Cyc_yynerrs=0;struct _tuple18{struct Cyc_Position_Segment*f1;struct
_tuple1*f2;int f3;};struct _tuple19{struct _dyneither_ptr f1;void*f2;};static char
_tmp46D[8]="stdcall";static char _tmp46E[6]="cdecl";static char _tmp46F[9]="fastcall";
static char _tmp470[9]="noreturn";static char _tmp471[6]="const";static char _tmp472[8]="aligned";
static char _tmp473[7]="packed";static char _tmp474[7]="shared";static char _tmp475[7]="unused";
static char _tmp476[5]="weak";static char _tmp477[10]="dllimport";static char _tmp478[
10]="dllexport";static char _tmp479[23]="no_instrument_function";static char _tmp47A[
12]="constructor";static char _tmp47B[11]="destructor";static char _tmp47C[22]="no_check_memory_usage";
static char _tmp47D[5]="pure";struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmpA0B(
unsigned int*_tmpA0A,unsigned int*_tmpA09,short**_tmpA08){for(*_tmpA0A=0;*
_tmpA0A < *_tmpA09;(*_tmpA0A)++){(*_tmpA08)[*_tmpA0A]=(short)0;}}static void
_tmpA10(unsigned int*_tmpA0F,unsigned int*_tmpA0E,union Cyc_YYSTYPE_union**
_tmpA0D){for(*_tmpA0F=0;*_tmpA0F < *_tmpA0E;(*_tmpA0F)++){(*_tmpA0D)[*_tmpA0F]=
Cyc_yylval;}}static void _tmpA15(unsigned int*_tmpA14,unsigned int*_tmpA13,struct
Cyc_Yyltype**_tmpA12){for(*_tmpA14=0;*_tmpA14 < *_tmpA13;(*_tmpA14)++){(*_tmpA12)[*
_tmpA14]=Cyc_yynewloc();}}static void _tmp12E8(unsigned int*_tmp12E7,unsigned int*
_tmp12E6,char**_tmp12E4){for(*_tmp12E7=0;*_tmp12E7 < *_tmp12E6;(*_tmp12E7)++){(*
_tmp12E4)[*_tmp12E7]='\000';}}int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){
struct _RegionHandle _tmp380=_new_region("yyregion");struct _RegionHandle*yyregion=&
_tmp380;_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;
int yyssp_offset;unsigned int _tmpA0A;unsigned int _tmpA09;short*_tmpA08;
unsigned int _tmpA07;short*yyss=(short*)((_tmpA07=10000,((_tmpA08=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmpA07)),((((_tmpA09=_tmpA07,
_tmpA0B(& _tmpA0A,& _tmpA09,& _tmpA08))),_tmpA08))))));int yyvsp_offset;unsigned int
_tmpA0F;unsigned int _tmpA0E;union Cyc_YYSTYPE_union*_tmpA0D;unsigned int _tmpA0C;
union Cyc_YYSTYPE_union*yyvs=(union Cyc_YYSTYPE_union*)((_tmpA0C=10000,((_tmpA0D=(
union Cyc_YYSTYPE_union*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE_union),
_tmpA0C)),((((_tmpA0E=_tmpA0C,_tmpA10(& _tmpA0F,& _tmpA0E,& _tmpA0D))),_tmpA0D))))));
int yylsp_offset;unsigned int _tmpA14;unsigned int _tmpA13;struct Cyc_Yyltype*
_tmpA12;unsigned int _tmpA11;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)((_tmpA11=
10000,((_tmpA12=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(
struct Cyc_Yyltype),_tmpA11)),((((_tmpA13=_tmpA11,_tmpA15(& _tmpA14,& _tmpA13,&
_tmpA12))),_tmpA12))))));int yystacksize=10000;union Cyc_YYSTYPE_union yyval=Cyc_yylval;
int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;
yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(
10000,++ yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(
yystacksize >= 10000){{const char*_tmpA16;Cyc_yyerror(((_tmpA16="parser stack overflow",
_tag_dyneither(_tmpA16,sizeof(char),22))));}(int)_throw((void*)Cyc_Yystack_overflow);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1006,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 361?(int)Cyc_yytranslate[_check_known_subscript_notnull(362,Cyc_yychar)]:
276;}yyn +=yychar1;if((yyn < 0  || yyn > 6033) || Cyc_yycheck[
_check_known_subscript_notnull(6034,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6034,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1005){int
_tmp382=0;_npop_handler(0);return _tmp382;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)-- yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1006,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(500,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL1E8: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL1E9: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp383;_push_handler(& _tmp383);{int _tmp385=0;if(setjmp(_tmp383.handler))_tmp385=
1;if(!_tmp385){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp384=(void*)_exn_thrown;void*
_tmp387=_tmp384;_LL1EC: if(*((void**)_tmp387)!= Cyc_Core_Failure)goto _LL1EE;
_LL1ED: x=0;goto _LL1EB;_LL1EE:;_LL1EF:(void)_throw(_tmp387);_LL1EB:;}}}{struct
_handler_cons _tmp388;_push_handler(& _tmp388);{int _tmp38A=0;if(setjmp(_tmp388.handler))
_tmp38A=1;if(!_tmp38A){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);;_pop_handler();}else{void*_tmp389=(void*)_exn_thrown;void*
_tmp38C=_tmp389;_LL1F1: if(*((void**)_tmp38C)!= Cyc_Core_Failure)goto _LL1F3;
_LL1F2: y=0;goto _LL1F0;_LL1F3:;_LL1F4:(void)_throw(_tmp38C);_LL1F0:;}}}{union Cyc_YYSTYPE_union
_tmpA17;yyval=(union Cyc_YYSTYPE_union)(((_tmpA17.YY18).tag=22,(((_tmpA17.YY18).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
x,y),_tmpA17))));}break;}case 3: _LL1EA:{struct Cyc_List_List*_tmpA26;struct Cyc_Absyn_Using_d_struct*
_tmpA25;struct Cyc_Absyn_Using_d_struct _tmpA24;struct Cyc_Absyn_Decl*_tmpA23;union
Cyc_YYSTYPE_union _tmpA22;yyval=(union Cyc_YYSTYPE_union)(((_tmpA22.YY18).tag=22,(((
_tmpA22.YY18).f1=((_tmpA26=_cycalloc(sizeof(*_tmpA26)),((_tmpA26->hd=((_tmpA23=
_cycalloc(sizeof(*_tmpA23)),((_tmpA23->r=(void*)((void*)((_tmpA25=_cycalloc(
sizeof(*_tmpA25)),((_tmpA25[0]=((_tmpA24.tag=11,((_tmpA24.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA24.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA24)))))),_tmpA25))))),((
_tmpA23->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpA23)))))),((_tmpA26->tl=0,_tmpA26)))))),_tmpA22))));}
Cyc_Lex_leave_using();break;case 4: _LL1F5:{struct Cyc_List_List*_tmpA35;struct Cyc_Absyn_Using_d_struct*
_tmpA34;struct Cyc_Absyn_Using_d_struct _tmpA33;struct Cyc_Absyn_Decl*_tmpA32;union
Cyc_YYSTYPE_union _tmpA31;yyval=(union Cyc_YYSTYPE_union)(((_tmpA31.YY18).tag=22,(((
_tmpA31.YY18).f1=((_tmpA35=_cycalloc(sizeof(*_tmpA35)),((_tmpA35->hd=((_tmpA32=
_cycalloc(sizeof(*_tmpA32)),((_tmpA32->r=(void*)((void*)((_tmpA34=_cycalloc(
sizeof(*_tmpA34)),((_tmpA34[0]=((_tmpA33.tag=11,((_tmpA33.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpA33.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA33)))))),
_tmpA34))))),((_tmpA32->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA32)))))),((
_tmpA35->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA35)))))),_tmpA31))));}break;case 5: _LL1F6:{struct Cyc_List_List*
_tmpA4A;struct Cyc_Absyn_Namespace_d_struct*_tmpA49;struct _dyneither_ptr*_tmpA48;
struct Cyc_Absyn_Namespace_d_struct _tmpA47;struct Cyc_Absyn_Decl*_tmpA46;union Cyc_YYSTYPE_union
_tmpA45;yyval=(union Cyc_YYSTYPE_union)(((_tmpA45.YY18).tag=22,(((_tmpA45.YY18).f1=((
_tmpA4A=_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A->hd=((_tmpA46=_cycalloc(sizeof(*
_tmpA46)),((_tmpA46->r=(void*)((void*)((_tmpA49=_cycalloc(sizeof(*_tmpA49)),((
_tmpA49[0]=((_tmpA47.tag=10,((_tmpA47.f1=((_tmpA48=_cycalloc(sizeof(*_tmpA48)),((
_tmpA48[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),_tmpA48)))),((_tmpA47.f2=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA47)))))),_tmpA49))))),((
_tmpA46->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpA46)))))),((_tmpA4A->tl=0,_tmpA4A)))))),_tmpA45))));}
Cyc_Lex_leave_namespace();break;case 6: _LL1F7: {struct _dyneither_ptr nspace;struct
Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons _tmp39E;_push_handler(&
_tmp39E);{int _tmp3A0=0;if(setjmp(_tmp39E.handler))_tmp3A0=1;if(!_tmp3A0){nspace=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;
_pop_handler();}else{void*_tmp39F=(void*)_exn_thrown;void*_tmp3A2=_tmp39F;_LL1FA:
if(*((void**)_tmp3A2)!= Cyc_Core_Failure)goto _LL1FC;_LL1FB:{const char*_tmpA4B;
nspace=((_tmpA4B="",_tag_dyneither(_tmpA4B,sizeof(char),1)));}goto _LL1F9;_LL1FC:;
_LL1FD:(void)_throw(_tmp3A2);_LL1F9:;}}}{struct _handler_cons _tmp3A4;
_push_handler(& _tmp3A4);{int _tmp3A6=0;if(setjmp(_tmp3A4.handler))_tmp3A6=1;if(!
_tmp3A6){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]);;_pop_handler();}else{void*_tmp3A5=(void*)_exn_thrown;void*_tmp3A8=
_tmp3A5;_LL1FF: if(*((void**)_tmp3A8)!= Cyc_Core_Failure)goto _LL201;_LL200: x=0;
goto _LL1FE;_LL201:;_LL202:(void)_throw(_tmp3A8);_LL1FE:;}}}{struct _handler_cons
_tmp3A9;_push_handler(& _tmp3A9);{int _tmp3AB=0;if(setjmp(_tmp3A9.handler))_tmp3AB=
1;if(!_tmp3AB){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);;_pop_handler();}else{void*_tmp3AA=(void*)_exn_thrown;void*
_tmp3AD=_tmp3AA;_LL204: if(*((void**)_tmp3AD)!= Cyc_Core_Failure)goto _LL206;
_LL205: y=0;goto _LL203;_LL206:;_LL207:(void)_throw(_tmp3AD);_LL203:;}}}{struct Cyc_List_List*
_tmpA60;struct Cyc_Absyn_Namespace_d_struct*_tmpA5F;struct _dyneither_ptr*_tmpA5E;
struct Cyc_Absyn_Namespace_d_struct _tmpA5D;struct Cyc_Absyn_Decl*_tmpA5C;union Cyc_YYSTYPE_union
_tmpA5B;yyval=(union Cyc_YYSTYPE_union)(((_tmpA5B.YY18).tag=22,(((_tmpA5B.YY18).f1=((
_tmpA60=_cycalloc(sizeof(*_tmpA60)),((_tmpA60->hd=((_tmpA5C=_cycalloc(sizeof(*
_tmpA5C)),((_tmpA5C->r=(void*)((void*)((_tmpA5F=_cycalloc(sizeof(*_tmpA5F)),((
_tmpA5F[0]=((_tmpA5D.tag=10,((_tmpA5D.f1=((_tmpA5E=_cycalloc(sizeof(*_tmpA5E)),((
_tmpA5E[0]=nspace,_tmpA5E)))),((_tmpA5D.f2=x,_tmpA5D)))))),_tmpA5F))))),((
_tmpA5C->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmpA5C)))))),((_tmpA60->tl=y,_tmpA60)))))),
_tmpA5B))));}break;}case 7: _LL1F8:{const char*_tmpA61;if(Cyc_strcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),((_tmpA61="C",_tag_dyneither(_tmpA61,sizeof(char),2))))== 0){
struct Cyc_List_List*_tmpA70;struct Cyc_Absyn_ExternC_d_struct*_tmpA6F;struct Cyc_Absyn_ExternC_d_struct
_tmpA6E;struct Cyc_Absyn_Decl*_tmpA6D;union Cyc_YYSTYPE_union _tmpA6C;yyval=(union
Cyc_YYSTYPE_union)(((_tmpA6C.YY18).tag=22,(((_tmpA6C.YY18).f1=((_tmpA70=
_cycalloc(sizeof(*_tmpA70)),((_tmpA70->hd=((_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((
_tmpA6D->r=(void*)((void*)((_tmpA6F=_cycalloc(sizeof(*_tmpA6F)),((_tmpA6F[0]=((
_tmpA6E.tag=12,((_tmpA6E.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),_tmpA6E)))),_tmpA6F))))),((_tmpA6D->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA6D)))))),((
_tmpA70->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA70)))))),_tmpA6C))));}else{{const char*_tmpA71;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),((_tmpA71="C include",_tag_dyneither(_tmpA71,sizeof(
char),10))))!= 0){const char*_tmpA72;Cyc_Parse_err(((_tmpA72="expecting \"C\" or \"C include\"",
_tag_dyneither(_tmpA72,sizeof(char),29))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));}}{struct Cyc_List_List*
_tmpA81;struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA80;struct Cyc_Absyn_ExternCinclude_d_struct
_tmpA7F;struct Cyc_Absyn_Decl*_tmpA7E;union Cyc_YYSTYPE_union _tmpA7D;yyval=(union
Cyc_YYSTYPE_union)(((_tmpA7D.YY18).tag=22,(((_tmpA7D.YY18).f1=((_tmpA81=
_cycalloc(sizeof(*_tmpA81)),((_tmpA81->hd=((_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((
_tmpA7E->r=(void*)((void*)((_tmpA80=_cycalloc(sizeof(*_tmpA80)),((_tmpA80[0]=((
_tmpA7F.tag=13,((_tmpA7F.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),((_tmpA7F.f2=0,_tmpA7F)))))),_tmpA80))))),((_tmpA7E->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmpA7E)))))),((_tmpA81->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA81)))))),_tmpA7D))));}}}
break;case 8: _LL208:{const char*_tmpA82;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),((_tmpA82="C include",
_tag_dyneither(_tmpA82,sizeof(char),10))))!= 0){const char*_tmpA83;Cyc_Parse_err(((
_tmpA83="expecting \"C include\"",_tag_dyneither(_tmpA83,sizeof(char),22))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{
struct Cyc_List_List*_tmpA92;struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA91;
struct Cyc_Absyn_ExternCinclude_d_struct _tmpA90;struct Cyc_Absyn_Decl*_tmpA8F;
union Cyc_YYSTYPE_union _tmpA8E;yyval=(union Cyc_YYSTYPE_union)(((_tmpA8E.YY18).tag=
22,(((_tmpA8E.YY18).f1=((_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92->hd=((
_tmpA8F=_cycalloc(sizeof(*_tmpA8F)),((_tmpA8F->r=(void*)((void*)((_tmpA91=
_cycalloc(sizeof(*_tmpA91)),((_tmpA91[0]=((_tmpA90.tag=13,((_tmpA90.f1=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA90.f2=exs,
_tmpA90)))))),_tmpA91))))),((_tmpA8F->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA8F)))))),((
_tmpA92->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA92)))))),_tmpA8E))));}break;}case 9: _LL209:{struct Cyc_List_List*
_tmpA98;struct Cyc_Absyn_Decl*_tmpA97;union Cyc_YYSTYPE_union _tmpA96;yyval=(union
Cyc_YYSTYPE_union)(((_tmpA96.YY18).tag=22,(((_tmpA96.YY18).f1=((_tmpA98=
_cycalloc(sizeof(*_tmpA98)),((_tmpA98->hd=((_tmpA97=_cycalloc(sizeof(*_tmpA97)),((
_tmpA97->r=(void*)((void*)0),((_tmpA97->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA97)))))),((
_tmpA98->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA98)))))),_tmpA96))));}break;case 10: _LL20A:{struct Cyc_List_List*
_tmpA9E;struct Cyc_Absyn_Decl*_tmpA9D;union Cyc_YYSTYPE_union _tmpA9C;yyval=(union
Cyc_YYSTYPE_union)(((_tmpA9C.YY18).tag=22,(((_tmpA9C.YY18).f1=((_tmpA9E=
_cycalloc(sizeof(*_tmpA9E)),((_tmpA9E->hd=((_tmpA9D=_cycalloc(sizeof(*_tmpA9D)),((
_tmpA9D->r=(void*)((void*)1),((_tmpA9D->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA9D)))))),((
_tmpA9E->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA9E)))))),_tmpA9C))));}break;case 11: _LL20B:{union Cyc_YYSTYPE_union
_tmpA9F;yyval=(union Cyc_YYSTYPE_union)(((_tmpA9F.YY18).tag=22,(((_tmpA9F.YY18).f1=
0,_tmpA9F))));}break;case 12: _LL20C: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 13: _LL20D:{union Cyc_YYSTYPE_union _tmpAA0;yyval=(
union Cyc_YYSTYPE_union)(((_tmpAA0.YY52).tag=56,(((_tmpAA0.YY52).f1=0,_tmpAA0))));}
break;case 14: _LL20E:{struct Cyc_List_List*_tmpAA6;struct _tuple18*_tmpAA5;union Cyc_YYSTYPE_union
_tmpAA4;yyval=(union Cyc_YYSTYPE_union)(((_tmpAA4.YY52).tag=56,(((_tmpAA4.YY52).f1=((
_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6->hd=((_tmpAA5=_cycalloc(sizeof(*
_tmpAA5)),((_tmpAA5->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpAA5->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpAA5->f3=0,_tmpAA5)))))))),((_tmpAA6->tl=0,_tmpAA6)))))),_tmpAA4))));}break;
case 15: _LL20F:{struct Cyc_List_List*_tmpAAC;struct _tuple18*_tmpAAB;union Cyc_YYSTYPE_union
_tmpAAA;yyval=(union Cyc_YYSTYPE_union)(((_tmpAAA.YY52).tag=56,(((_tmpAAA.YY52).f1=((
_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC->hd=((_tmpAAB=_cycalloc(sizeof(*
_tmpAAB)),((_tmpAAB->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpAAB->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpAAB->f3=0,_tmpAAB)))))))),((_tmpAAC->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAAC)))))),_tmpAAA))));}
break;case 16: _LL210:{struct Cyc_List_List*_tmpAB6;struct Cyc_Absyn_Fn_d_struct
_tmpAB5;struct Cyc_Absyn_Fn_d_struct*_tmpAB4;union Cyc_YYSTYPE_union _tmpAB3;yyval=(
union Cyc_YYSTYPE_union)(((_tmpAB3.YY18).tag=22,(((_tmpAB3.YY18).f1=((_tmpAB6=
_cycalloc(sizeof(*_tmpAB6)),((_tmpAB6->hd=Cyc_Absyn_new_decl((void*)((_tmpAB4=
_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4[0]=((_tmpAB5.tag=1,((_tmpAB5.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAB5)))),_tmpAB4)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpAB6->tl=0,_tmpAB6)))))),_tmpAB3))));}break;
case 17: _LL211: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 18: _LL212:{union Cyc_YYSTYPE_union _tmpAB7;yyval=(union Cyc_YYSTYPE_union)(((
_tmpAB7.YY18).tag=22,(((_tmpAB7.YY18).f1=0,_tmpAB7))));}break;case 19: _LL213:{
union Cyc_YYSTYPE_union _tmpAB8;yyval=(union Cyc_YYSTYPE_union)(((_tmpAB8.YY17).tag=
21,(((_tmpAB8.YY17).f1=Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpAB8))));}
break;case 20: _LL214:{struct Cyc_Core_Opt*_tmpABB;union Cyc_YYSTYPE_union _tmpABA;
yyval=(union Cyc_YYSTYPE_union)(((_tmpABA.YY17).tag=21,(((_tmpABA.YY17).f1=Cyc_Parse_make_function(((
_tmpABB=_cycalloc(sizeof(*_tmpABB)),((_tmpABB->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpABB)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpABA))));}
break;case 21: _LL215:{union Cyc_YYSTYPE_union _tmpABC;yyval=(union Cyc_YYSTYPE_union)(((
_tmpABC.YY17).tag=21,(((_tmpABC.YY17).f1=Cyc_Parse_make_function(0,Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpABC))));}
break;case 22: _LL216:{struct Cyc_Core_Opt*_tmpABF;union Cyc_YYSTYPE_union _tmpABE;
yyval=(union Cyc_YYSTYPE_union)(((_tmpABE.YY17).tag=21,(((_tmpABE.YY17).f1=Cyc_Parse_make_function(((
_tmpABF=_cycalloc(sizeof(*_tmpABF)),((_tmpABF->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpABF)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpABE))));}
break;case 23: _LL217:{struct Cyc_Core_Opt*_tmpAC2;union Cyc_YYSTYPE_union _tmpAC1;
yyval=(union Cyc_YYSTYPE_union)(((_tmpAC1.YY17).tag=21,(((_tmpAC1.YY17).f1=Cyc_Parse_make_function(((
_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpAC2)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpAC1))));}
break;case 24: _LL218:{struct Cyc_Core_Opt*_tmpAC5;union Cyc_YYSTYPE_union _tmpAC4;
yyval=(union Cyc_YYSTYPE_union)(((_tmpAC4.YY17).tag=21,(((_tmpAC4.YY17).f1=Cyc_Parse_make_function(((
_tmpAC5=_cycalloc(sizeof(*_tmpAC5)),((_tmpAC5->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpAC5)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpAC4))));}
break;case 25: _LL219: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL21A: Cyc_Lex_leave_using();
break;case 27: _LL21B:{struct _dyneither_ptr*_tmpAC6;Cyc_Lex_enter_namespace(((
_tmpAC6=_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAC6)))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL21C: Cyc_Lex_leave_namespace();
break;case 29: _LL21D:{union Cyc_YYSTYPE_union _tmpAC7;yyval=(union Cyc_YYSTYPE_union)(((
_tmpAC7.YY18).tag=22,(((_tmpAC7.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmpAC7))));}
break;case 30: _LL21E:{union Cyc_YYSTYPE_union _tmpAC8;yyval=(union Cyc_YYSTYPE_union)(((
_tmpAC8.YY18).tag=22,(((_tmpAC8.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpAC8))));}
break;case 31: _LL21F:{struct Cyc_List_List*_tmpACB;union Cyc_YYSTYPE_union _tmpACA;
yyval=(union Cyc_YYSTYPE_union)(((_tmpACA.YY18).tag=22,(((_tmpACA.YY18).f1=((
_tmpACB=_cycalloc(sizeof(*_tmpACB)),((_tmpACB->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpACB->tl=0,
_tmpACB)))))),_tmpACA))));}break;case 32: _LL220: {struct Cyc_List_List*_tmp3EA=0;{
struct Cyc_List_List*_tmp3EB=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);for(0;_tmp3EB != 0;_tmp3EB=_tmp3EB->tl){struct
_dyneither_ptr*_tmp3EC=(struct _dyneither_ptr*)_tmp3EB->hd;union Cyc_Absyn_Nmspace_union
_tmpACE;struct _tuple1*_tmpACD;struct _tuple1*qv=(_tmpACD=_cycalloc(sizeof(*
_tmpACD)),((_tmpACD->f1=(union Cyc_Absyn_Nmspace_union)(((_tmpACE.Rel_n).tag=1,(((
_tmpACE.Rel_n).f1=0,_tmpACE)))),((_tmpACD->f2=_tmp3EC,_tmpACD)))));struct Cyc_Absyn_Vardecl*
_tmp3ED=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);struct Cyc_List_List*
_tmpACF;_tmp3EA=((_tmpACF=_cycalloc(sizeof(*_tmpACF)),((_tmpACF->hd=_tmp3ED,((
_tmpACF->tl=_tmp3EA,_tmpACF))))));}}_tmp3EA=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp3EA);{struct Cyc_List_List*_tmpAD2;union Cyc_YYSTYPE_union
_tmpAD1;yyval=(union Cyc_YYSTYPE_union)(((_tmpAD1.YY18).tag=22,(((_tmpAD1.YY18).f1=((
_tmpAD2=_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2->hd=Cyc_Absyn_letv_decl(_tmp3EA,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpAD2->tl=0,_tmpAD2)))))),_tmpAD1))));}break;}
case 33: _LL221:{const char*_tmpAD4;const char*_tmpAD3;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmpAD4="`H",_tag_dyneither(_tmpAD4,sizeof(char),3))))== 0
 || Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpAD3="`U",
_tag_dyneither(_tmpAD3,sizeof(char),3))))== 0){const char*_tmpAD8;void*_tmpAD7[1];
struct Cyc_String_pa_struct _tmpAD6;Cyc_Parse_err((struct _dyneither_ptr)((_tmpAD6.tag=
0,((_tmpAD6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])),((_tmpAD7[0]=&
_tmpAD6,Cyc_aprintf(((_tmpAD8="bad occurrence of heap region %s",_tag_dyneither(
_tmpAD8,sizeof(char),33))),_tag_dyneither(_tmpAD7,sizeof(void*),1)))))))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}{struct
_dyneither_ptr*_tmpADB;struct Cyc_Absyn_Tvar*_tmpADA;struct Cyc_Absyn_Tvar*tv=(
_tmpADA=_cycalloc(sizeof(*_tmpADA)),((_tmpADA->name=((_tmpADB=_cycalloc(sizeof(*
_tmpADB)),((_tmpADB[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),_tmpADB)))),((_tmpADA->identity=- 1,((_tmpADA->kind=(
void*)Cyc_Tcutil_kind_to_bound((void*)3),_tmpADA)))))));struct Cyc_Absyn_VarType_struct
_tmpADE;struct Cyc_Absyn_VarType_struct*_tmpADD;void*t=(void*)((_tmpADD=_cycalloc(
sizeof(*_tmpADD)),((_tmpADD[0]=((_tmpADE.tag=1,((_tmpADE.f1=tv,_tmpADE)))),
_tmpADD))));struct Cyc_Absyn_RgnHandleType_struct _tmpAE6;struct Cyc_Absyn_RgnHandleType_struct*
_tmpAE5;union Cyc_Absyn_Nmspace_union _tmpAE3;struct _dyneither_ptr*_tmpAE2;struct
_tuple1*_tmpAE1;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpAE1=
_cycalloc(sizeof(*_tmpAE1)),((_tmpAE1->f1=(union Cyc_Absyn_Nmspace_union)(((
_tmpAE3.Loc_n).tag=0,_tmpAE3)),((_tmpAE1->f2=((_tmpAE2=_cycalloc(sizeof(*_tmpAE2)),((
_tmpAE2[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpAE2)))),_tmpAE1)))))),(void*)((_tmpAE5=_cycalloc(sizeof(*
_tmpAE5)),((_tmpAE5[0]=((_tmpAE6.tag=14,((_tmpAE6.f1=(void*)t,_tmpAE6)))),
_tmpAE5)))),0);{struct Cyc_List_List*_tmpAE9;union Cyc_YYSTYPE_union _tmpAE8;yyval=(
union Cyc_YYSTYPE_union)(((_tmpAE8.YY18).tag=22,(((_tmpAE8.YY18).f1=((_tmpAE9=
_cycalloc(sizeof(*_tmpAE9)),((_tmpAE9->hd=Cyc_Absyn_region_decl(tv,vd,0,0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAE9->tl=0,
_tmpAE9)))))),_tmpAE8))));}break;}case 34: _LL222:{const char*_tmpAEA;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),((_tmpAEA="H",_tag_dyneither(_tmpAEA,sizeof(char),2))))
== 0){const char*_tmpAEB;Cyc_Parse_err(((_tmpAEB="bad occurrence of heap region `H",
_tag_dyneither(_tmpAEB,sizeof(char),33))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));}}{struct
_dyneither_ptr*_tmpAF8;const char*_tmpAF7;void*_tmpAF6[1];struct Cyc_String_pa_struct
_tmpAF5;struct Cyc_Absyn_Tvar*_tmpAF4;struct Cyc_Absyn_Tvar*tv=(_tmpAF4=_cycalloc(
sizeof(*_tmpAF4)),((_tmpAF4->name=((_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((
_tmpAF8[0]=(struct _dyneither_ptr)((_tmpAF5.tag=0,((_tmpAF5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpAF6[0]=& _tmpAF5,
Cyc_aprintf(((_tmpAF7="`%s",_tag_dyneither(_tmpAF7,sizeof(char),4))),
_tag_dyneither(_tmpAF6,sizeof(void*),1)))))))),_tmpAF8)))),((_tmpAF4->identity=-
1,((_tmpAF4->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3),_tmpAF4)))))));
struct Cyc_Absyn_VarType_struct _tmpAFB;struct Cyc_Absyn_VarType_struct*_tmpAFA;
void*t=(void*)((_tmpAFA=_cycalloc(sizeof(*_tmpAFA)),((_tmpAFA[0]=((_tmpAFB.tag=1,((
_tmpAFB.f1=tv,_tmpAFB)))),_tmpAFA))));struct Cyc_Absyn_RgnHandleType_struct
_tmpB03;struct Cyc_Absyn_RgnHandleType_struct*_tmpB02;union Cyc_Absyn_Nmspace_union
_tmpB00;struct _dyneither_ptr*_tmpAFF;struct _tuple1*_tmpAFE;struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(((_tmpAFE=_cycalloc(sizeof(*_tmpAFE)),((_tmpAFE->f1=(
union Cyc_Absyn_Nmspace_union)(((_tmpB00.Loc_n).tag=0,_tmpB00)),((_tmpAFE->f2=((
_tmpAFF=_cycalloc(sizeof(*_tmpAFF)),((_tmpAFF[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpAFF)))),_tmpAFE)))))),(
void*)((_tmpB02=_cycalloc(sizeof(*_tmpB02)),((_tmpB02[0]=((_tmpB03.tag=14,((
_tmpB03.f1=(void*)t,_tmpB03)))),_tmpB02)))),0);{struct Cyc_List_List*_tmpB06;
union Cyc_YYSTYPE_union _tmpB05;yyval=(union Cyc_YYSTYPE_union)(((_tmpB05.YY18).tag=
22,(((_tmpB05.YY18).f1=((_tmpB06=_cycalloc(sizeof(*_tmpB06)),((_tmpB06->hd=Cyc_Absyn_region_decl(
tv,vd,0,Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpB06->tl=0,_tmpB06)))))),_tmpB05))));}break;}
case 35: _LL223:{const char*_tmpB07;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((_tmpB07="resetable",
_tag_dyneither(_tmpB07,sizeof(char),10))))!= 0){const char*_tmpB08;Cyc_Parse_err(((
_tmpB08="expecting [resetable]",_tag_dyneither(_tmpB08,sizeof(char),22))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));}}{const char*
_tmpB0A;const char*_tmpB09;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB0A="`H",
_tag_dyneither(_tmpB0A,sizeof(char),3))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmpB09="`U",_tag_dyneither(_tmpB09,sizeof(char),3))))){
const char*_tmpB0E;void*_tmpB0D[1];struct Cyc_String_pa_struct _tmpB0C;Cyc_Parse_err((
struct _dyneither_ptr)((_tmpB0C.tag=0,((_tmpB0C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)])),((_tmpB0D[0]=& _tmpB0C,Cyc_aprintf(((_tmpB0E="bad occurrence of heap region %s",
_tag_dyneither(_tmpB0E,sizeof(char),33))),_tag_dyneither(_tmpB0D,sizeof(void*),1)))))))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));}}{struct _dyneither_ptr*_tmpB11;struct Cyc_Absyn_Tvar*
_tmpB10;struct Cyc_Absyn_Tvar*tv=(_tmpB10=_cycalloc(sizeof(*_tmpB10)),((_tmpB10->name=((
_tmpB11=_cycalloc(sizeof(*_tmpB11)),((_tmpB11[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpB11)))),((_tmpB10->identity=
- 1,((_tmpB10->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3),_tmpB10)))))));
struct Cyc_Absyn_VarType_struct _tmpB14;struct Cyc_Absyn_VarType_struct*_tmpB13;
void*t=(void*)((_tmpB13=_cycalloc(sizeof(*_tmpB13)),((_tmpB13[0]=((_tmpB14.tag=1,((
_tmpB14.f1=tv,_tmpB14)))),_tmpB13))));struct Cyc_Absyn_RgnHandleType_struct
_tmpB1C;struct Cyc_Absyn_RgnHandleType_struct*_tmpB1B;union Cyc_Absyn_Nmspace_union
_tmpB19;struct _dyneither_ptr*_tmpB18;struct _tuple1*_tmpB17;struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(((_tmpB17=_cycalloc(sizeof(*_tmpB17)),((_tmpB17->f1=(
union Cyc_Absyn_Nmspace_union)(((_tmpB19.Loc_n).tag=0,_tmpB19)),((_tmpB17->f2=((
_tmpB18=_cycalloc(sizeof(*_tmpB18)),((_tmpB18[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB18)))),_tmpB17)))))),(
void*)((_tmpB1B=_cycalloc(sizeof(*_tmpB1B)),((_tmpB1B[0]=((_tmpB1C.tag=14,((
_tmpB1C.f1=(void*)t,_tmpB1C)))),_tmpB1B)))),0);{struct Cyc_List_List*_tmpB1F;
union Cyc_YYSTYPE_union _tmpB1E;yyval=(union Cyc_YYSTYPE_union)(((_tmpB1E.YY18).tag=
22,(((_tmpB1E.YY18).f1=((_tmpB1F=_cycalloc(sizeof(*_tmpB1F)),((_tmpB1F->hd=Cyc_Absyn_region_decl(
tv,vd,1,0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 8)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpB1F->tl=0,_tmpB1F)))))),_tmpB1E))));}break;}
case 36: _LL224:{const char*_tmpB20;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB20="resetable",
_tag_dyneither(_tmpB20,sizeof(char),10))))!= 0){const char*_tmpB21;Cyc_Parse_err(((
_tmpB21="expecting `resetable'",_tag_dyneither(_tmpB21,sizeof(char),22))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}{const char*
_tmpB22;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB22="H",
_tag_dyneither(_tmpB22,sizeof(char),2))))== 0){const char*_tmpB23;Cyc_Parse_err(((
_tmpB23="bad occurrence of heap region `H",_tag_dyneither(_tmpB23,sizeof(char),
33))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}}{struct _dyneither_ptr*_tmpB30;const char*_tmpB2F;
void*_tmpB2E[1];struct Cyc_String_pa_struct _tmpB2D;struct Cyc_Absyn_Tvar*_tmpB2C;
struct Cyc_Absyn_Tvar*tv=(_tmpB2C=_cycalloc(sizeof(*_tmpB2C)),((_tmpB2C->name=((
_tmpB30=_cycalloc(sizeof(*_tmpB30)),((_tmpB30[0]=(struct _dyneither_ptr)((_tmpB2D.tag=
0,((_tmpB2D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),((_tmpB2E[0]=&
_tmpB2D,Cyc_aprintf(((_tmpB2F="`%s",_tag_dyneither(_tmpB2F,sizeof(char),4))),
_tag_dyneither(_tmpB2E,sizeof(void*),1)))))))),_tmpB30)))),((_tmpB2C->identity=-
1,((_tmpB2C->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3),_tmpB2C)))))));
struct Cyc_Absyn_VarType_struct _tmpB33;struct Cyc_Absyn_VarType_struct*_tmpB32;
void*t=(void*)((_tmpB32=_cycalloc(sizeof(*_tmpB32)),((_tmpB32[0]=((_tmpB33.tag=1,((
_tmpB33.f1=tv,_tmpB33)))),_tmpB32))));struct Cyc_Absyn_RgnHandleType_struct
_tmpB3B;struct Cyc_Absyn_RgnHandleType_struct*_tmpB3A;union Cyc_Absyn_Nmspace_union
_tmpB38;struct _dyneither_ptr*_tmpB37;struct _tuple1*_tmpB36;struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(((_tmpB36=_cycalloc(sizeof(*_tmpB36)),((_tmpB36->f1=(
union Cyc_Absyn_Nmspace_union)(((_tmpB38.Loc_n).tag=0,_tmpB38)),((_tmpB36->f2=((
_tmpB37=_cycalloc(sizeof(*_tmpB37)),((_tmpB37[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB37)))),_tmpB36)))))),(
void*)((_tmpB3A=_cycalloc(sizeof(*_tmpB3A)),((_tmpB3A[0]=((_tmpB3B.tag=14,((
_tmpB3B.f1=(void*)t,_tmpB3B)))),_tmpB3A)))),0);{struct Cyc_List_List*_tmpB3E;
union Cyc_YYSTYPE_union _tmpB3D;yyval=(union Cyc_YYSTYPE_union)(((_tmpB3D.YY18).tag=
22,(((_tmpB3D.YY18).f1=((_tmpB3E=_cycalloc(sizeof(*_tmpB3E)),((_tmpB3E->hd=Cyc_Absyn_region_decl(
tv,vd,1,0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpB3E->tl=0,_tmpB3E)))))),_tmpB3D))));}break;}
case 37: _LL225: {struct _dyneither_ptr*_tmpB46;struct Cyc_Absyn_Eq_kb_struct*
_tmpB45;struct Cyc_Absyn_Eq_kb_struct _tmpB44;struct Cyc_Absyn_Tvar*_tmpB43;struct
Cyc_Absyn_Tvar*tv=(_tmpB43=_cycalloc(sizeof(*_tmpB43)),((_tmpB43->name=((_tmpB46=
_cycalloc(sizeof(*_tmpB46)),((_tmpB46[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpB46)))),((_tmpB43->identity=
- 1,((_tmpB43->kind=(void*)((void*)((_tmpB45=_cycalloc(sizeof(*_tmpB45)),((
_tmpB45[0]=((_tmpB44.tag=0,((_tmpB44.f1=(void*)((void*)3),_tmpB44)))),_tmpB45))))),
_tmpB43)))))));union Cyc_Absyn_Nmspace_union _tmpB4B;struct _dyneither_ptr*_tmpB4A;
struct _tuple1*_tmpB49;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpB49=
_cycalloc(sizeof(*_tmpB49)),((_tmpB49->f1=(union Cyc_Absyn_Nmspace_union)(((
_tmpB4B.Loc_n).tag=0,_tmpB4B)),((_tmpB49->f2=((_tmpB4A=_cycalloc(sizeof(*_tmpB4A)),((
_tmpB4A[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),_tmpB4A)))),_tmpB49)))))),(void*)0,0);{struct Cyc_List_List*
_tmpB4E;union Cyc_YYSTYPE_union _tmpB4D;yyval=(union Cyc_YYSTYPE_union)(((_tmpB4D.YY18).tag=
22,(((_tmpB4D.YY18).f1=((_tmpB4E=_cycalloc(sizeof(*_tmpB4E)),((_tmpB4E->hd=Cyc_Absyn_alias_decl(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),tv,vd,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpB4E->tl=0,_tmpB4E)))))),_tmpB4D))));}break;}
case 38: _LL226: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 39: _LL227:{union Cyc_YYSTYPE_union _tmpB4F;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB4F.YY18).tag=22,(((_tmpB4F.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpB4F))));}break;case 40:
_LL228:{struct Cyc_Parse_Declaration_spec*_tmpB55;struct Cyc_Core_Opt*_tmpB54;
union Cyc_YYSTYPE_union _tmpB53;yyval=(union Cyc_YYSTYPE_union)(((_tmpB53.YY19).tag=
23,(((_tmpB53.YY19).f1=((_tmpB55=_cycalloc(sizeof(*_tmpB55)),((_tmpB55->sc=((
_tmpB54=_cycalloc(sizeof(*_tmpB54)),((_tmpB54->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB54)))),((_tmpB55->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB55->type_specs=
0,((_tmpB55->is_inline=0,((_tmpB55->attributes=0,_tmpB55)))))))))))),_tmpB53))));}
break;case 41: _LL229: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc != 0){const char*_tmpB58;void*_tmpB57;(_tmpB57=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpB58="Only one storage class is allowed in a declaration",
_tag_dyneither(_tmpB58,sizeof(char),51))),_tag_dyneither(_tmpB57,sizeof(void*),0)));}{
struct Cyc_Parse_Declaration_spec*_tmpB5E;struct Cyc_Core_Opt*_tmpB5D;union Cyc_YYSTYPE_union
_tmpB5C;yyval=(union Cyc_YYSTYPE_union)(((_tmpB5C.YY19).tag=23,(((_tmpB5C.YY19).f1=((
_tmpB5E=_cycalloc(sizeof(*_tmpB5E)),((_tmpB5E->sc=((_tmpB5D=_cycalloc(sizeof(*
_tmpB5D)),((_tmpB5D->v=(void*)Cyc_yyget_YY22(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpB5D)))),((_tmpB5E->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((_tmpB5E->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpB5E->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpB5E->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpB5E)))))))))))),
_tmpB5C))));}break;case 42: _LL22A:{struct Cyc_Parse_Declaration_spec*_tmpB64;
struct Cyc_List_List*_tmpB63;union Cyc_YYSTYPE_union _tmpB62;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB62.YY19).tag=23,(((_tmpB62.YY19).f1=((_tmpB64=_cycalloc(sizeof(*_tmpB64)),((
_tmpB64->sc=0,((_tmpB64->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB64->type_specs=((
_tmpB63=_cycalloc(sizeof(*_tmpB63)),((_tmpB63->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB63->tl=0,_tmpB63)))))),((
_tmpB64->is_inline=0,((_tmpB64->attributes=0,_tmpB64)))))))))))),_tmpB62))));}
break;case 43: _LL22B:{struct Cyc_Parse_Declaration_spec*_tmpB6A;struct Cyc_List_List*
_tmpB69;union Cyc_YYSTYPE_union _tmpB68;yyval=(union Cyc_YYSTYPE_union)(((_tmpB68.YY19).tag=
23,(((_tmpB68.YY19).f1=((_tmpB6A=_cycalloc(sizeof(*_tmpB6A)),((_tmpB6A->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpB6A->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpB6A->type_specs=((_tmpB69=_cycalloc(sizeof(*_tmpB69)),((_tmpB69->hd=(void*)
Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpB69->tl=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,_tmpB69)))))),((_tmpB6A->is_inline=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmpB6A->attributes=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,
_tmpB6A)))))))))))),_tmpB68))));}break;case 44: _LL22C:{struct Cyc_Parse_Declaration_spec*
_tmpB6D;union Cyc_YYSTYPE_union _tmpB6C;yyval=(union Cyc_YYSTYPE_union)(((_tmpB6C.YY19).tag=
23,(((_tmpB6C.YY19).f1=((_tmpB6D=_cycalloc(sizeof(*_tmpB6D)),((_tmpB6D->sc=0,((
_tmpB6D->tq=Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpB6D->type_specs=0,((_tmpB6D->is_inline=0,((_tmpB6D->attributes=
0,_tmpB6D)))))))))))),_tmpB6C))));}break;case 45: _LL22D:{struct Cyc_Parse_Declaration_spec*
_tmpB70;union Cyc_YYSTYPE_union _tmpB6F;yyval=(union Cyc_YYSTYPE_union)(((_tmpB6F.YY19).tag=
23,(((_tmpB6F.YY19).f1=((_tmpB70=_cycalloc(sizeof(*_tmpB70)),((_tmpB70->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpB70->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq),((_tmpB70->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpB70->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpB70->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpB70)))))))))))),
_tmpB6F))));}break;case 46: _LL22E:{struct Cyc_Parse_Declaration_spec*_tmpB73;union
Cyc_YYSTYPE_union _tmpB72;yyval=(union Cyc_YYSTYPE_union)(((_tmpB72.YY19).tag=23,(((
_tmpB72.YY19).f1=((_tmpB73=_cycalloc(sizeof(*_tmpB73)),((_tmpB73->sc=0,((_tmpB73->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB73->type_specs=
0,((_tmpB73->is_inline=1,((_tmpB73->attributes=0,_tmpB73)))))))))))),_tmpB72))));}
break;case 47: _LL22F:{struct Cyc_Parse_Declaration_spec*_tmpB76;union Cyc_YYSTYPE_union
_tmpB75;yyval=(union Cyc_YYSTYPE_union)(((_tmpB75.YY19).tag=23,(((_tmpB75.YY19).f1=((
_tmpB76=_cycalloc(sizeof(*_tmpB76)),((_tmpB76->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpB76->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((_tmpB76->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpB76->is_inline=1,((_tmpB76->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpB76)))))))))))),
_tmpB75))));}break;case 48: _LL230:{struct Cyc_Parse_Declaration_spec*_tmpB79;union
Cyc_YYSTYPE_union _tmpB78;yyval=(union Cyc_YYSTYPE_union)(((_tmpB78.YY19).tag=23,(((
_tmpB78.YY19).f1=((_tmpB79=_cycalloc(sizeof(*_tmpB79)),((_tmpB79->sc=0,((_tmpB79->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB79->type_specs=
0,((_tmpB79->is_inline=0,((_tmpB79->attributes=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB79)))))))))))),_tmpB78))));}
break;case 49: _LL231:{struct Cyc_Parse_Declaration_spec*_tmpB7C;union Cyc_YYSTYPE_union
_tmpB7B;yyval=(union Cyc_YYSTYPE_union)(((_tmpB7B.YY19).tag=23,(((_tmpB7B.YY19).f1=((
_tmpB7C=_cycalloc(sizeof(*_tmpB7C)),((_tmpB7C->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpB7C->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((_tmpB7C->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpB7C->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpB7C->attributes=Cyc_List_imp_append(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes),_tmpB7C)))))))))))),
_tmpB7B))));}break;case 50: _LL232:{union Cyc_YYSTYPE_union _tmpB7D;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB7D.YY22).tag=26,(((_tmpB7D.YY22).f1=(void*)((void*)4),_tmpB7D))));}break;
case 51: _LL233:{union Cyc_YYSTYPE_union _tmpB7E;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB7E.YY22).tag=26,(((_tmpB7E.YY22).f1=(void*)((void*)5),_tmpB7E))));}break;
case 52: _LL234:{union Cyc_YYSTYPE_union _tmpB7F;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB7F.YY22).tag=26,(((_tmpB7F.YY22).f1=(void*)((void*)3),_tmpB7F))));}break;
case 53: _LL235:{union Cyc_YYSTYPE_union _tmpB80;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB80.YY22).tag=26,(((_tmpB80.YY22).f1=(void*)((void*)1),_tmpB80))));}break;
case 54: _LL236:{const char*_tmpB81;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB81="C",
_tag_dyneither(_tmpB81,sizeof(char),2))))!= 0){const char*_tmpB82;Cyc_Parse_err(((
_tmpB82="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpB82,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}}{union Cyc_YYSTYPE_union _tmpB83;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB83.YY22).tag=26,(((_tmpB83.YY22).f1=(void*)((void*)2),_tmpB83))));}break;
case 55: _LL237:{union Cyc_YYSTYPE_union _tmpB84;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB84.YY22).tag=26,(((_tmpB84.YY22).f1=(void*)((void*)0),_tmpB84))));}break;
case 56: _LL238:{union Cyc_YYSTYPE_union _tmpB85;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB85.YY22).tag=26,(((_tmpB85.YY22).f1=(void*)((void*)6),_tmpB85))));}break;
case 57: _LL239:{union Cyc_YYSTYPE_union _tmpB86;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB86.YY45).tag=49,(((_tmpB86.YY45).f1=0,_tmpB86))));}break;case 58: _LL23A:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 59:
_LL23B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 60: _LL23C:{struct Cyc_List_List*_tmpB89;union Cyc_YYSTYPE_union _tmpB88;yyval=(
union Cyc_YYSTYPE_union)(((_tmpB88.YY45).tag=49,(((_tmpB88.YY45).f1=((_tmpB89=
_cycalloc(sizeof(*_tmpB89)),((_tmpB89->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB89->tl=0,_tmpB89)))))),
_tmpB88))));}break;case 61: _LL23D:{struct Cyc_List_List*_tmpB8C;union Cyc_YYSTYPE_union
_tmpB8B;yyval=(union Cyc_YYSTYPE_union)(((_tmpB8B.YY45).tag=49,(((_tmpB8B.YY45).f1=((
_tmpB8C=_cycalloc(sizeof(*_tmpB8C)),((_tmpB8C->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB8C->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB8C)))))),_tmpB8B))));}
break;case 62: _LL23E: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={1,- 1};
static struct _tuple19 att_map[17]={{{_tmp46D,_tmp46D,_tmp46D + 8},(void*)0},{{
_tmp46E,_tmp46E,_tmp46E + 6},(void*)1},{{_tmp46F,_tmp46F,_tmp46F + 9},(void*)2},{{
_tmp470,_tmp470,_tmp470 + 9},(void*)3},{{_tmp471,_tmp471,_tmp471 + 6},(void*)4},{{
_tmp472,_tmp472,_tmp472 + 8},(void*)& att_aligned},{{_tmp473,_tmp473,_tmp473 + 7},(
void*)5},{{_tmp474,_tmp474,_tmp474 + 7},(void*)7},{{_tmp475,_tmp475,_tmp475 + 7},(
void*)8},{{_tmp476,_tmp476,_tmp476 + 5},(void*)9},{{_tmp477,_tmp477,_tmp477 + 10},(
void*)10},{{_tmp478,_tmp478,_tmp478 + 10},(void*)11},{{_tmp479,_tmp479,_tmp479 + 
23},(void*)12},{{_tmp47A,_tmp47A,_tmp47A + 12},(void*)13},{{_tmp47B,_tmp47B,
_tmp47B + 11},(void*)14},{{_tmp47C,_tmp47C,_tmp47C + 22},(void*)15},{{_tmp47D,
_tmp47D,_tmp47D + 5},(void*)16}};struct _dyneither_ptr _tmp469=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((
_get_dyneither_size(_tmp469,sizeof(char))> 4  && *((const char*)
_check_dyneither_subscript(_tmp469,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp469,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp469,sizeof(char),(int)(_get_dyneither_size(
_tmp469,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp469,sizeof(char),(int)(_get_dyneither_size(_tmp469,sizeof(char))- 3)))== '_')
_tmp469=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp469,2,
_get_dyneither_size(_tmp469,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp469,(struct _dyneither_ptr)(att_map[i]).f1)== 0){{union
Cyc_YYSTYPE_union _tmpB8D;yyval=(union Cyc_YYSTYPE_union)(((_tmpB8D.YY46).tag=50,(((
_tmpB8D.YY46).f1=(void*)(att_map[i]).f2,_tmpB8D))));}break;}}if(i == 17){{const
char*_tmpB8E;Cyc_Parse_err(((_tmpB8E="unrecognized attribute",_tag_dyneither(
_tmpB8E,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}{union Cyc_YYSTYPE_union
_tmpB8F;yyval=(union Cyc_YYSTYPE_union)(((_tmpB8F.YY46).tag=50,(((_tmpB8F.YY46).f1=(
void*)((void*)1),_tmpB8F))));}}break;}}case 63: _LL23F:{union Cyc_YYSTYPE_union
_tmpB90;yyval=(union Cyc_YYSTYPE_union)(((_tmpB90.YY46).tag=50,(((_tmpB90.YY46).f1=(
void*)((void*)4),_tmpB90))));}break;case 64: _LL240: {struct _dyneither_ptr _tmp480=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
int _tmp482;struct _tuple11 _tmp481=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp482=_tmp481.f2;{void*
a;{const char*_tmpB92;const char*_tmpB91;if(Cyc_zstrcmp((struct _dyneither_ptr)
_tmp480,((_tmpB92="regparm",_tag_dyneither(_tmpB92,sizeof(char),8))))== 0  || Cyc_zstrcmp((
struct _dyneither_ptr)_tmp480,((_tmpB91="__regparm__",_tag_dyneither(_tmpB91,
sizeof(char),12))))== 0){if(_tmp482 < 0  || _tmp482 > 3){const char*_tmpB93;Cyc_Parse_err(((
_tmpB93="regparm requires value between 0 and 3",_tag_dyneither(_tmpB93,sizeof(
char),39))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Regparm_att_struct _tmpB96;
struct Cyc_Absyn_Regparm_att_struct*_tmpB95;a=(void*)((_tmpB95=_cycalloc_atomic(
sizeof(*_tmpB95)),((_tmpB95[0]=((_tmpB96.tag=0,((_tmpB96.f1=_tmp482,_tmpB96)))),
_tmpB95))));}}else{const char*_tmpB98;const char*_tmpB97;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp480,((_tmpB98="aligned",_tag_dyneither(_tmpB98,sizeof(char),8))))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp480,((_tmpB97="__aligned__",
_tag_dyneither(_tmpB97,sizeof(char),12))))== 0){if(_tmp482 < 0){const char*_tmpB99;
Cyc_Parse_err(((_tmpB99="aligned requires positive power of two",_tag_dyneither(
_tmpB99,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{
unsigned int j=(unsigned int)_tmp482;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){
const char*_tmpB9A;Cyc_Parse_err(((_tmpB9A="aligned requires positive power of two",
_tag_dyneither(_tmpB9A,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Aligned_att_struct
_tmpB9D;struct Cyc_Absyn_Aligned_att_struct*_tmpB9C;a=(void*)((_tmpB9C=
_cycalloc_atomic(sizeof(*_tmpB9C)),((_tmpB9C[0]=((_tmpB9D.tag=1,((_tmpB9D.f1=
_tmp482,_tmpB9D)))),_tmpB9C))));}}}else{const char*_tmpB9F;const char*_tmpB9E;if(
Cyc_zstrcmp((struct _dyneither_ptr)_tmp480,((_tmpB9F="initializes",_tag_dyneither(
_tmpB9F,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp480,((
_tmpB9E="__initializes__",_tag_dyneither(_tmpB9E,sizeof(char),16))))== 0){struct
Cyc_Absyn_Initializes_att_struct _tmpBA2;struct Cyc_Absyn_Initializes_att_struct*
_tmpBA1;a=(void*)((_tmpBA1=_cycalloc_atomic(sizeof(*_tmpBA1)),((_tmpBA1[0]=((
_tmpBA2.tag=4,((_tmpBA2.f1=_tmp482,_tmpBA2)))),_tmpBA1))));}else{{const char*
_tmpBA3;Cyc_Parse_err(((_tmpBA3="unrecognized attribute",_tag_dyneither(_tmpBA3,
sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)1;}}}}{
union Cyc_YYSTYPE_union _tmpBA4;yyval=(union Cyc_YYSTYPE_union)(((_tmpBA4.YY46).tag=
50,(((_tmpBA4.YY46).f1=(void*)a,_tmpBA4))));}break;}}case 65: _LL241: {struct
_dyneither_ptr _tmp494=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp495=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpBA6;const char*_tmpBA5;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp494,((_tmpBA6="section",
_tag_dyneither(_tmpBA6,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp494,((_tmpBA5="__section__",_tag_dyneither(_tmpBA5,sizeof(
char),12))))== 0){struct Cyc_Absyn_Section_att_struct _tmpBA9;struct Cyc_Absyn_Section_att_struct*
_tmpBA8;a=(void*)((_tmpBA8=_cycalloc(sizeof(*_tmpBA8)),((_tmpBA8[0]=((_tmpBA9.tag=
2,((_tmpBA9.f1=_tmp495,_tmpBA9)))),_tmpBA8))));}else{{const char*_tmpBAA;Cyc_Parse_err(((
_tmpBAA="unrecognized attribute",_tag_dyneither(_tmpBAA,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)1;}}{
union Cyc_YYSTYPE_union _tmpBAB;yyval=(union Cyc_YYSTYPE_union)(((_tmpBAB.YY46).tag=
50,(((_tmpBAB.YY46).f1=(void*)a,_tmpBAB))));}break;}case 66: _LL242: {struct
_dyneither_ptr _tmp49C=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp49D=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpBAC;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp49C,((_tmpBAC="__mode__",
_tag_dyneither(_tmpBAC,sizeof(char),9))))== 0){struct Cyc_Absyn_Mode_att_struct
_tmpBAF;struct Cyc_Absyn_Mode_att_struct*_tmpBAE;a=(void*)((_tmpBAE=_cycalloc(
sizeof(*_tmpBAE)),((_tmpBAE[0]=((_tmpBAF.tag=5,((_tmpBAF.f1=_tmp49D,_tmpBAF)))),
_tmpBAE))));}else{{const char*_tmpBB0;Cyc_Parse_err(((_tmpBB0="unrecognized attribute",
_tag_dyneither(_tmpBB0,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)1;}}{
union Cyc_YYSTYPE_union _tmpBB1;yyval=(union Cyc_YYSTYPE_union)(((_tmpBB1.YY46).tag=
50,(((_tmpBB1.YY46).f1=(void*)a,_tmpBB1))));}break;}case 67: _LL243: {struct
_dyneither_ptr _tmp4A3=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dyneither_ptr _tmp4A4=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp4A6;struct
_tuple11 _tmp4A5=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp4A6=_tmp4A5.f2;{int _tmp4A8;struct _tuple11 _tmp4A7=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4A8=_tmp4A7.f2;{
void*a=(void*)1;{const char*_tmpBB3;const char*_tmpBB2;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp4A3,((_tmpBB3="format",_tag_dyneither(_tmpBB3,sizeof(char),7))))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4A3,((_tmpBB2="__format__",
_tag_dyneither(_tmpBB2,sizeof(char),11))))== 0){const char*_tmpBB4;if(Cyc_zstrcmp((
struct _dyneither_ptr)_tmp4A4,((_tmpBB4="printf",_tag_dyneither(_tmpBB4,sizeof(
char),7))))== 0){struct Cyc_Absyn_Format_att_struct _tmpBB7;struct Cyc_Absyn_Format_att_struct*
_tmpBB6;a=(void*)((_tmpBB6=_cycalloc(sizeof(*_tmpBB6)),((_tmpBB6[0]=((_tmpBB7.tag=
3,((_tmpBB7.f1=(void*)((void*)0),((_tmpBB7.f2=_tmp4A6,((_tmpBB7.f3=_tmp4A8,
_tmpBB7)))))))),_tmpBB6))));}else{const char*_tmpBB8;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp4A4,((_tmpBB8="scanf",_tag_dyneither(_tmpBB8,sizeof(char),6))))
== 0){struct Cyc_Absyn_Format_att_struct _tmpBBB;struct Cyc_Absyn_Format_att_struct*
_tmpBBA;a=(void*)((_tmpBBA=_cycalloc(sizeof(*_tmpBBA)),((_tmpBBA[0]=((_tmpBBB.tag=
3,((_tmpBBB.f1=(void*)((void*)1),((_tmpBBB.f2=_tmp4A6,((_tmpBBB.f3=_tmp4A8,
_tmpBBB)))))))),_tmpBBA))));}else{const char*_tmpBBC;Cyc_Parse_err(((_tmpBBC="unrecognized format type",
_tag_dyneither(_tmpBBC,sizeof(char),25))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{
const char*_tmpBBD;Cyc_Parse_err(((_tmpBBD="unrecognized attribute",
_tag_dyneither(_tmpBBD,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}{union Cyc_YYSTYPE_union
_tmpBBE;yyval=(union Cyc_YYSTYPE_union)(((_tmpBBE.YY46).tag=50,(((_tmpBBE.YY46).f1=(
void*)a,_tmpBBE))));}break;}}}case 68: _LL244: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 69: _LL245:{struct
Cyc_Absyn_TypedefType_struct*_tmpBC4;struct Cyc_Absyn_TypedefType_struct _tmpBC3;
union Cyc_YYSTYPE_union _tmpBC2;yyval=(union Cyc_YYSTYPE_union)(((_tmpBC2.YY23).tag=
27,(((_tmpBC2.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpBC4=_cycalloc(
sizeof(*_tmpBC4)),((_tmpBC4[0]=((_tmpBC3.tag=16,((_tmpBC3.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpBC3.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBC3.f3=0,((
_tmpBC3.f4=0,_tmpBC3)))))))))),_tmpBC4)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpBC2))));}
break;case 70: _LL246:{union Cyc_YYSTYPE_union _tmpBC5;yyval=(union Cyc_YYSTYPE_union)(((
_tmpBC5.YY23).tag=27,(((_tmpBC5.YY23).f1=(void*)Cyc_Parse_type_spec((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpBC5))));}
break;case 71: _LL247:{union Cyc_YYSTYPE_union _tmpBC6;yyval=(union Cyc_YYSTYPE_union)(((
_tmpBC6.YY23).tag=27,(((_tmpBC6.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(
0,0),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpBC6))));}break;case 72: _LL248:{union Cyc_YYSTYPE_union
_tmpBC7;yyval=(union Cyc_YYSTYPE_union)(((_tmpBC7.YY23).tag=27,(((_tmpBC7.YY23).f1=(
void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpBC7))));}
break;case 73: _LL249:{union Cyc_YYSTYPE_union _tmpBC8;yyval=(union Cyc_YYSTYPE_union)(((
_tmpBC8.YY23).tag=27,(((_tmpBC8.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_char_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpBC8))));}break;case 74: _LL24A:{struct Cyc_Parse_Short_spec_struct*
_tmpBCE;struct Cyc_Parse_Short_spec_struct _tmpBCD;union Cyc_YYSTYPE_union _tmpBCC;
yyval=(union Cyc_YYSTYPE_union)(((_tmpBCC.YY23).tag=27,(((_tmpBCC.YY23).f1=(void*)((
void*)((_tmpBCE=_cycalloc(sizeof(*_tmpBCE)),((_tmpBCE[0]=((_tmpBCD.tag=2,((
_tmpBCD.f1=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpBCD)))),_tmpBCE))))),_tmpBCC))));}break;case 75:
_LL24B:{union Cyc_YYSTYPE_union _tmpBCF;yyval=(union Cyc_YYSTYPE_union)(((_tmpBCF.YY23).tag=
27,(((_tmpBCF.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpBCF))));}
break;case 76: _LL24C:{struct Cyc_Parse_Long_spec_struct*_tmpBD5;struct Cyc_Parse_Long_spec_struct
_tmpBD4;union Cyc_YYSTYPE_union _tmpBD3;yyval=(union Cyc_YYSTYPE_union)(((_tmpBD3.YY23).tag=
27,(((_tmpBD3.YY23).f1=(void*)((void*)((_tmpBD5=_cycalloc(sizeof(*_tmpBD5)),((
_tmpBD5[0]=((_tmpBD4.tag=3,((_tmpBD4.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBD4)))),
_tmpBD5))))),_tmpBD3))));}break;case 77: _LL24D:{union Cyc_YYSTYPE_union _tmpBD6;
yyval=(union Cyc_YYSTYPE_union)(((_tmpBD6.YY23).tag=27,(((_tmpBD6.YY23).f1=(void*)
Cyc_Parse_type_spec(Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpBD6))));}
break;case 78: _LL24E:{union Cyc_YYSTYPE_union _tmpBD7;yyval=(union Cyc_YYSTYPE_union)(((
_tmpBD7.YY23).tag=27,(((_tmpBD7.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_typ(
0),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpBD7))));}break;case 79: _LL24F:{struct Cyc_Parse_Signed_spec_struct*
_tmpBDD;struct Cyc_Parse_Signed_spec_struct _tmpBDC;union Cyc_YYSTYPE_union _tmpBDB;
yyval=(union Cyc_YYSTYPE_union)(((_tmpBDB.YY23).tag=27,(((_tmpBDB.YY23).f1=(void*)((
void*)((_tmpBDD=_cycalloc(sizeof(*_tmpBDD)),((_tmpBDD[0]=((_tmpBDC.tag=0,((
_tmpBDC.f1=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpBDC)))),_tmpBDD))))),_tmpBDB))));}break;case 80:
_LL250:{struct Cyc_Parse_Unsigned_spec_struct*_tmpBE3;struct Cyc_Parse_Unsigned_spec_struct
_tmpBE2;union Cyc_YYSTYPE_union _tmpBE1;yyval=(union Cyc_YYSTYPE_union)(((_tmpBE1.YY23).tag=
27,(((_tmpBE1.YY23).f1=(void*)((void*)((_tmpBE3=_cycalloc(sizeof(*_tmpBE3)),((
_tmpBE3[0]=((_tmpBE2.tag=1,((_tmpBE2.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBE2)))),
_tmpBE3))))),_tmpBE1))));}break;case 81: _LL251: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 82: _LL252: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 83: _LL253:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 84:
_LL254:{union Cyc_YYSTYPE_union _tmpBE4;yyval=(union Cyc_YYSTYPE_union)(((_tmpBE4.YY23).tag=
27,(((_tmpBE4.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpBE4))));}
break;case 85: _LL255:{struct Cyc_Absyn_TupleType_struct*_tmpBEA;struct Cyc_Absyn_TupleType_struct
_tmpBE9;union Cyc_YYSTYPE_union _tmpBE8;yyval=(union Cyc_YYSTYPE_union)(((_tmpBE8.YY23).tag=
27,(((_tmpBE8.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpBEA=_cycalloc(
sizeof(*_tmpBEA)),((_tmpBEA[0]=((_tmpBE9.tag=9,((_tmpBE9.f1=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))),_tmpBE9)))),_tmpBEA)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpBE8))));}break;case 86: _LL256:{struct Cyc_Absyn_RgnHandleType_struct*
_tmpBF0;struct Cyc_Absyn_RgnHandleType_struct _tmpBEF;union Cyc_YYSTYPE_union
_tmpBEE;yyval=(union Cyc_YYSTYPE_union)(((_tmpBEE.YY23).tag=27,(((_tmpBEE.YY23).f1=(
void*)Cyc_Parse_type_spec((void*)((_tmpBF0=_cycalloc(sizeof(*_tmpBF0)),((_tmpBF0[
0]=((_tmpBEF.tag=14,((_tmpBEF.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpBEF)))),_tmpBF0)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpBEE))));}break;case 87: _LL257: {void*_tmp4D1=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0);{struct Cyc_Absyn_DynRgnType_struct*_tmpBF6;
struct Cyc_Absyn_DynRgnType_struct _tmpBF5;union Cyc_YYSTYPE_union _tmpBF4;yyval=(
union Cyc_YYSTYPE_union)(((_tmpBF4.YY23).tag=27,(((_tmpBF4.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)((_tmpBF6=_cycalloc(sizeof(*_tmpBF6)),((_tmpBF6[0]=((_tmpBF5.tag=15,((
_tmpBF5.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpBF5.f2=(void*)_tmp4D1,_tmpBF5)))))),_tmpBF6)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpBF4))));}
break;}case 88: _LL258:{struct Cyc_Absyn_DynRgnType_struct*_tmpBFC;struct Cyc_Absyn_DynRgnType_struct
_tmpBFB;union Cyc_YYSTYPE_union _tmpBFA;yyval=(union Cyc_YYSTYPE_union)(((_tmpBFA.YY23).tag=
27,(((_tmpBFA.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpBFC=_cycalloc(
sizeof(*_tmpBFC)),((_tmpBFC[0]=((_tmpBFB.tag=15,((_tmpBFB.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpBFB.f2=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpBFB)))))),_tmpBFC)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpBFA))));}
break;case 89: _LL259:{struct Cyc_Absyn_TagType_struct*_tmpC02;struct Cyc_Absyn_TagType_struct
_tmpC01;union Cyc_YYSTYPE_union _tmpC00;yyval=(union Cyc_YYSTYPE_union)(((_tmpC00.YY23).tag=
27,(((_tmpC00.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpC02=_cycalloc(
sizeof(*_tmpC02)),((_tmpC02[0]=((_tmpC01.tag=18,((_tmpC01.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC01)))),_tmpC02)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpC00))));}break;case 90: _LL25A:{struct Cyc_Absyn_TagType_struct*
_tmpC08;struct Cyc_Absyn_TagType_struct _tmpC07;union Cyc_YYSTYPE_union _tmpC06;
yyval=(union Cyc_YYSTYPE_union)(((_tmpC06.YY23).tag=27,(((_tmpC06.YY23).f1=(void*)
Cyc_Parse_type_spec((void*)((_tmpC08=_cycalloc(sizeof(*_tmpC08)),((_tmpC08[0]=((
_tmpC07.tag=18,((_tmpC07.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ik,
0),_tmpC07)))),_tmpC08)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpC06))));}
break;case 91: _LL25B:{struct Cyc_Absyn_ValueofType_struct*_tmpC0E;struct Cyc_Absyn_ValueofType_struct
_tmpC0D;union Cyc_YYSTYPE_union _tmpC0C;yyval=(union Cyc_YYSTYPE_union)(((_tmpC0C.YY23).tag=
27,(((_tmpC0C.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpC0E=_cycalloc(
sizeof(*_tmpC0E)),((_tmpC0E[0]=((_tmpC0D.tag=17,((_tmpC0D.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC0D)))),_tmpC0E)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpC0C))));}break;case 92: _LL25C:{union Cyc_YYSTYPE_union
_tmpC0F;yyval=(union Cyc_YYSTYPE_union)(((_tmpC0F.YY43).tag=47,(((_tmpC0F.YY43).f1=(
void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpC0F))));}
break;case 93: _LL25D: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line): 0;{struct Cyc_Absyn_Tqual _tmpC12;union Cyc_YYSTYPE_union
_tmpC11;yyval=(union Cyc_YYSTYPE_union)(((_tmpC11.YY25).tag=29,(((_tmpC11.YY25).f1=((
_tmpC12.print_const=1,((_tmpC12.q_volatile=0,((_tmpC12.q_restrict=0,((_tmpC12.real_const=
1,((_tmpC12.loc=loc,_tmpC12)))))))))),_tmpC11))));}break;}case 94: _LL25E:{struct
Cyc_Absyn_Tqual _tmpC15;union Cyc_YYSTYPE_union _tmpC14;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC14.YY25).tag=29,(((_tmpC14.YY25).f1=((_tmpC15.print_const=0,((_tmpC15.q_volatile=
1,((_tmpC15.q_restrict=0,((_tmpC15.real_const=0,((_tmpC15.loc=0,_tmpC15)))))))))),
_tmpC14))));}break;case 95: _LL25F:{struct Cyc_Absyn_Tqual _tmpC18;union Cyc_YYSTYPE_union
_tmpC17;yyval=(union Cyc_YYSTYPE_union)(((_tmpC17.YY25).tag=29,(((_tmpC17.YY25).f1=((
_tmpC18.print_const=0,((_tmpC18.q_volatile=0,((_tmpC18.q_restrict=1,((_tmpC18.real_const=
0,((_tmpC18.loc=0,_tmpC18)))))))))),_tmpC17))));}break;case 96: _LL260:{struct Cyc_Parse_Decl_spec_struct*
_tmpC3C;struct Cyc_Absyn_Decl*_tmpC3B;struct Cyc_Absyn_Enum_d_struct _tmpC3A;struct
Cyc_Core_Opt*_tmpC39;struct Cyc_Absyn_Enumdecl*_tmpC38;struct Cyc_Absyn_Enum_d_struct*
_tmpC37;struct Cyc_Parse_Decl_spec_struct _tmpC36;union Cyc_YYSTYPE_union _tmpC35;
yyval=(union Cyc_YYSTYPE_union)(((_tmpC35.YY23).tag=27,(((_tmpC35.YY23).f1=(void*)((
void*)((_tmpC3C=_cycalloc(sizeof(*_tmpC3C)),((_tmpC3C[0]=((_tmpC36.tag=5,((
_tmpC36.f1=((_tmpC3B=_cycalloc(sizeof(*_tmpC3B)),((_tmpC3B->r=(void*)((void*)((
_tmpC37=_cycalloc(sizeof(*_tmpC37)),((_tmpC37[0]=((_tmpC3A.tag=8,((_tmpC3A.f1=((
_tmpC38=_cycalloc(sizeof(*_tmpC38)),((_tmpC38->sc=(void*)((void*)2),((_tmpC38->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpC38->fields=((_tmpC39=_cycalloc(sizeof(*_tmpC39)),((_tmpC39->v=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC39)))),_tmpC38)))))))),
_tmpC3A)))),_tmpC37))))),((_tmpC3B->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC3B)))))),
_tmpC36)))),_tmpC3C))))),_tmpC35))));}break;case 97: _LL261:{struct Cyc_Absyn_EnumType_struct*
_tmpC42;struct Cyc_Absyn_EnumType_struct _tmpC41;union Cyc_YYSTYPE_union _tmpC40;
yyval=(union Cyc_YYSTYPE_union)(((_tmpC40.YY23).tag=27,(((_tmpC40.YY23).f1=(void*)
Cyc_Parse_type_spec((void*)((_tmpC42=_cycalloc(sizeof(*_tmpC42)),((_tmpC42[0]=((
_tmpC41.tag=12,((_tmpC41.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC41.f2=0,_tmpC41)))))),
_tmpC42)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpC40))));}break;case 98: _LL262:{struct Cyc_Parse_Type_spec_struct*
_tmpC51;struct Cyc_Absyn_AnonEnumType_struct*_tmpC50;struct Cyc_Absyn_AnonEnumType_struct
_tmpC4F;struct Cyc_Parse_Type_spec_struct _tmpC4E;union Cyc_YYSTYPE_union _tmpC4D;
yyval=(union Cyc_YYSTYPE_union)(((_tmpC4D.YY23).tag=27,(((_tmpC4D.YY23).f1=(void*)((
void*)((_tmpC51=_cycalloc(sizeof(*_tmpC51)),((_tmpC51[0]=((_tmpC4E.tag=4,((
_tmpC4E.f1=(void*)((void*)((_tmpC50=_cycalloc(sizeof(*_tmpC50)),((_tmpC50[0]=((
_tmpC4F.tag=13,((_tmpC4F.f1=Cyc_yyget_YY48(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpC4F)))),_tmpC50))))),((_tmpC4E.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC4E)))))),
_tmpC51))))),_tmpC4D))));}break;case 99: _LL263:{struct Cyc_Absyn_Enumfield*_tmpC54;
union Cyc_YYSTYPE_union _tmpC53;yyval=(union Cyc_YYSTYPE_union)(((_tmpC53.YY47).tag=
51,(((_tmpC53.YY47).f1=((_tmpC54=_cycalloc(sizeof(*_tmpC54)),((_tmpC54->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC54->tag=0,((
_tmpC54->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpC54)))))))),_tmpC53))));}break;case 100: _LL264:{
struct Cyc_Absyn_Enumfield*_tmpC57;union Cyc_YYSTYPE_union _tmpC56;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC56.YY47).tag=51,(((_tmpC56.YY47).f1=((_tmpC57=_cycalloc(sizeof(*_tmpC57)),((
_tmpC57->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmpC57->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC57->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC57)))))))),
_tmpC56))));}break;case 101: _LL265:{struct Cyc_List_List*_tmpC5A;union Cyc_YYSTYPE_union
_tmpC59;yyval=(union Cyc_YYSTYPE_union)(((_tmpC59.YY48).tag=52,(((_tmpC59.YY48).f1=((
_tmpC5A=_cycalloc(sizeof(*_tmpC5A)),((_tmpC5A->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC5A->tl=0,_tmpC5A)))))),
_tmpC59))));}break;case 102: _LL266:{struct Cyc_List_List*_tmpC5D;union Cyc_YYSTYPE_union
_tmpC5C;yyval=(union Cyc_YYSTYPE_union)(((_tmpC5C.YY48).tag=52,(((_tmpC5C.YY48).f1=((
_tmpC5D=_cycalloc(sizeof(*_tmpC5D)),((_tmpC5D->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpC5D->tl=0,_tmpC5D)))))),
_tmpC5C))));}break;case 103: _LL267:{struct Cyc_List_List*_tmpC60;union Cyc_YYSTYPE_union
_tmpC5F;yyval=(union Cyc_YYSTYPE_union)(((_tmpC5F.YY48).tag=52,(((_tmpC5F.YY48).f1=((
_tmpC60=_cycalloc(sizeof(*_tmpC60)),((_tmpC60->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC60->tl=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC60)))))),_tmpC5F))));}
break;case 104: _LL268:{struct Cyc_Absyn_AnonAggrType_struct*_tmpC66;struct Cyc_Absyn_AnonAggrType_struct
_tmpC65;union Cyc_YYSTYPE_union _tmpC64;yyval=(union Cyc_YYSTYPE_union)(((_tmpC64.YY23).tag=
27,(((_tmpC64.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpC66=_cycalloc(
sizeof(*_tmpC66)),((_tmpC66[0]=((_tmpC65.tag=11,((_tmpC65.f1=(void*)Cyc_yyget_YY24(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpC65.f2=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC65)))))),
_tmpC66)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpC64))));}break;case 105: _LL269: {struct Cyc_List_List*
_tmp505=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]));struct Cyc_List_List*_tmp506=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct*
_tmpC6C;struct Cyc_Parse_Decl_spec_struct _tmpC6B;union Cyc_YYSTYPE_union _tmpC6A;
yyval=(union Cyc_YYSTYPE_union)(((_tmpC6A.YY23).tag=27,(((_tmpC6A.YY23).f1=(void*)((
void*)((_tmpC6C=_cycalloc(sizeof(*_tmpC6C)),((_tmpC6C[0]=((_tmpC6B.tag=5,((
_tmpC6B.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp505,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp506,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpC6B)))),
_tmpC6C))))),_tmpC6A))));}break;}case 106: _LL26A:{struct Cyc_Absyn_AggrType_struct*
_tmpC7B;struct Cyc_Absyn_AggrInfo _tmpC7A;union Cyc_Absyn_AggrInfoU_union _tmpC79;
struct Cyc_Absyn_AggrType_struct _tmpC78;union Cyc_YYSTYPE_union _tmpC77;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC77.YY23).tag=27,(((_tmpC77.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)((_tmpC7B=_cycalloc(sizeof(*_tmpC7B)),((_tmpC7B[0]=((_tmpC78.tag=10,((
_tmpC78.f1=((_tmpC7A.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((_tmpC79.UnknownAggr).tag=
0,(((_tmpC79.UnknownAggr).f1=(void*)Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(((_tmpC79.UnknownAggr).f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpC79)))))),((_tmpC7A.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),_tmpC7A)))),_tmpC78)))),_tmpC7B)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpC77))));}
break;case 107: _LL26B:{union Cyc_YYSTYPE_union _tmpC7C;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC7C.YY40).tag=44,(((_tmpC7C.YY40).f1=0,_tmpC7C))));}break;case 108: _LL26C:{
union Cyc_YYSTYPE_union _tmpC7D;yyval=(union Cyc_YYSTYPE_union)(((_tmpC7D.YY40).tag=
44,(((_tmpC7D.YY40).f1=Cyc_List_imp_rev(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),_tmpC7D))));}break;case
109: _LL26D:{union Cyc_YYSTYPE_union _tmpC7E;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC7E.YY24).tag=28,(((_tmpC7E.YY24).f1=(void*)((void*)0),_tmpC7E))));}break;
case 110: _LL26E:{union Cyc_YYSTYPE_union _tmpC7F;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC7F.YY24).tag=28,(((_tmpC7F.YY24).f1=(void*)((void*)1),_tmpC7F))));}break;
case 111: _LL26F:{union Cyc_YYSTYPE_union _tmpC80;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC80.YY26).tag=30,(((_tmpC80.YY26).f1=0,_tmpC80))));}break;case 112: _LL270:{
union Cyc_YYSTYPE_union _tmpC81;yyval=(union Cyc_YYSTYPE_union)(((_tmpC81.YY26).tag=
30,(((_tmpC81.YY26).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))),_tmpC81))));}break;
case 113: _LL271:{struct Cyc_List_List*_tmpC84;union Cyc_YYSTYPE_union _tmpC83;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC83.YY27).tag=31,(((_tmpC83.YY27).f1=((_tmpC84=
_cycalloc(sizeof(*_tmpC84)),((_tmpC84->hd=Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC84->tl=0,_tmpC84)))))),
_tmpC83))));}break;case 114: _LL272:{struct Cyc_List_List*_tmpC87;union Cyc_YYSTYPE_union
_tmpC86;yyval=(union Cyc_YYSTYPE_union)(((_tmpC86.YY27).tag=31,(((_tmpC86.YY27).f1=((
_tmpC87=_cycalloc(sizeof(*_tmpC87)),((_tmpC87->hd=Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC87->tl=Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC87)))))),
_tmpC86))));}break;case 115: _LL273:{union Cyc_YYSTYPE_union _tmpC88;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC88.YY21).tag=25,(((_tmpC88.YY21).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])),_tmpC88))));}break;case 116: _LL274:{struct Cyc_List_List*_tmpC8B;
union Cyc_YYSTYPE_union _tmpC8A;yyval=(union Cyc_YYSTYPE_union)(((_tmpC8A.YY21).tag=
25,(((_tmpC8A.YY21).f1=((_tmpC8B=_cycalloc(sizeof(*_tmpC8B)),((_tmpC8B->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC8B->tl=0,
_tmpC8B)))))),_tmpC8A))));}break;case 117: _LL275:{struct Cyc_List_List*_tmpC8E;
union Cyc_YYSTYPE_union _tmpC8D;yyval=(union Cyc_YYSTYPE_union)(((_tmpC8D.YY21).tag=
25,(((_tmpC8D.YY21).f1=((_tmpC8E=_cycalloc(sizeof(*_tmpC8E)),((_tmpC8E->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC8E->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC8E)))))),
_tmpC8D))));}break;case 118: _LL276:{struct _tuple15*_tmpC91;union Cyc_YYSTYPE_union
_tmpC90;yyval=(union Cyc_YYSTYPE_union)(((_tmpC90.YY20).tag=24,(((_tmpC90.YY20).f1=((
_tmpC91=_cycalloc(sizeof(*_tmpC91)),((_tmpC91->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC91->f2=0,_tmpC91)))))),
_tmpC90))));}break;case 119: _LL277:{struct _tuple15*_tmpC94;union Cyc_YYSTYPE_union
_tmpC93;yyval=(union Cyc_YYSTYPE_union)(((_tmpC93.YY20).tag=24,(((_tmpC93.YY20).f1=((
_tmpC94=_cycalloc(sizeof(*_tmpC94)),((_tmpC94->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC94->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC94)))))),
_tmpC93))));}break;case 120: _LL278: {struct _RegionHandle _tmp522=_new_region("temp");
struct _RegionHandle*temp=& _tmp522;_push_region(temp);{struct _tuple16 _tmp524;
struct Cyc_Absyn_Tqual _tmp525;struct Cyc_List_List*_tmp526;struct Cyc_List_List*
_tmp527;struct _tuple16*_tmp523=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp524=*_tmp523;_tmp525=
_tmp524.f1;_tmp526=_tmp524.f2;_tmp527=_tmp524.f3;if(_tmp525.loc == 0)_tmp525.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp529;struct Cyc_List_List*
_tmp52A;struct _tuple0 _tmp528=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(temp,temp,Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp529=_tmp528.f1;
_tmp52A=_tmp528.f2;{void*_tmp52B=Cyc_Parse_speclist2typ(_tmp526,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp52C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(
temp,_tmp525,_tmp52B,_tmp529,_tmp527),_tmp52A);union Cyc_YYSTYPE_union _tmpC95;
yyval=(union Cyc_YYSTYPE_union)(((_tmpC95.YY26).tag=30,(((_tmpC95.YY26).f1=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,
struct _tuple8*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp52C),
_tmpC95))));}}}_npop_handler(0);break;;_pop_region(temp);}case 121: _LL279:{struct
_tuple16*_tmpC9B;struct Cyc_List_List*_tmpC9A;union Cyc_YYSTYPE_union _tmpC99;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC99.YY35).tag=39,(((_tmpC99.YY35).f1=((_tmpC9B=
_cycalloc(sizeof(*_tmpC9B)),((_tmpC9B->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpC9B->f2=((
_tmpC9A=_cycalloc(sizeof(*_tmpC9A)),((_tmpC9A->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC9A->tl=0,_tmpC9A)))))),((
_tmpC9B->f3=0,_tmpC9B)))))))),_tmpC99))));}break;case 122: _LL27A:{struct _tuple16*
_tmpCA1;struct Cyc_List_List*_tmpCA0;union Cyc_YYSTYPE_union _tmpC9F;yyval=(union
Cyc_YYSTYPE_union)(((_tmpC9F.YY35).tag=39,(((_tmpC9F.YY35).f1=((_tmpCA1=
_cycalloc(sizeof(*_tmpCA1)),((_tmpCA1->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpCA1->f2=((_tmpCA0=
_cycalloc(sizeof(*_tmpCA0)),((_tmpCA0->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCA0->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpCA0)))))),((
_tmpCA1->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpCA1)))))))),_tmpC9F))));}break;case 123: _LL27B:{struct
_tuple16*_tmpCA4;union Cyc_YYSTYPE_union _tmpCA3;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCA3.YY35).tag=39,(((_tmpCA3.YY35).f1=((_tmpCA4=_cycalloc(sizeof(*_tmpCA4)),((
_tmpCA4->f1=Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpCA4->f2=0,((_tmpCA4->f3=0,_tmpCA4)))))))),_tmpCA3))));}
break;case 124: _LL27C:{struct _tuple16*_tmpCA7;union Cyc_YYSTYPE_union _tmpCA6;yyval=(
union Cyc_YYSTYPE_union)(((_tmpCA6.YY35).tag=39,(((_tmpCA6.YY35).f1=((_tmpCA7=
_cycalloc(sizeof(*_tmpCA7)),((_tmpCA7->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpCA7->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpCA7->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpCA7)))))))),_tmpCA6))));}break;case 125: _LL27D:{struct
_tuple16*_tmpCAA;union Cyc_YYSTYPE_union _tmpCA9;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCA9.YY35).tag=39,(((_tmpCA9.YY35).f1=((_tmpCAA=_cycalloc(sizeof(*_tmpCAA)),((
_tmpCAA->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpCAA->f2=0,((
_tmpCAA->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpCAA)))))))),_tmpCA9))));}break;case 126: _LL27E:{struct
_tuple16*_tmpCAD;union Cyc_YYSTYPE_union _tmpCAC;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCAC.YY35).tag=39,(((_tmpCAC.YY35).f1=((_tmpCAD=_cycalloc(sizeof(*_tmpCAD)),((
_tmpCAD->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1,((_tmpCAD->f2=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpCAD->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpCAD)))))))),_tmpCAC))));}break;case 127: _LL27F:{struct _tuple16*_tmpCB3;struct
Cyc_List_List*_tmpCB2;union Cyc_YYSTYPE_union _tmpCB1;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCB1.YY35).tag=39,(((_tmpCB1.YY35).f1=((_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((
_tmpCB3->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpCB3->f2=((
_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((_tmpCB2->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCB2->tl=0,_tmpCB2)))))),((
_tmpCB3->f3=0,_tmpCB3)))))))),_tmpCB1))));}break;case 128: _LL280:{struct _tuple16*
_tmpCB9;struct Cyc_List_List*_tmpCB8;union Cyc_YYSTYPE_union _tmpCB7;yyval=(union
Cyc_YYSTYPE_union)(((_tmpCB7.YY35).tag=39,(((_tmpCB7.YY35).f1=((_tmpCB9=
_cycalloc(sizeof(*_tmpCB9)),((_tmpCB9->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpCB9->f2=((_tmpCB8=
_cycalloc(sizeof(*_tmpCB8)),((_tmpCB8->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCB8->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpCB8)))))),((
_tmpCB9->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpCB9)))))))),_tmpCB7))));}break;case 129: _LL281:{struct
_tuple16*_tmpCBC;union Cyc_YYSTYPE_union _tmpCBB;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCBB.YY35).tag=39,(((_tmpCBB.YY35).f1=((_tmpCBC=_cycalloc(sizeof(*_tmpCBC)),((
_tmpCBC->f1=Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpCBC->f2=0,((_tmpCBC->f3=0,_tmpCBC)))))))),_tmpCBB))));}
break;case 130: _LL282:{struct _tuple16*_tmpCBF;union Cyc_YYSTYPE_union _tmpCBE;yyval=(
union Cyc_YYSTYPE_union)(((_tmpCBE.YY35).tag=39,(((_tmpCBE.YY35).f1=((_tmpCBF=
_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpCBF->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpCBF->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpCBF)))))))),_tmpCBE))));}break;case 131: _LL283:{struct
_tuple16*_tmpCC2;union Cyc_YYSTYPE_union _tmpCC1;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCC1.YY35).tag=39,(((_tmpCC1.YY35).f1=((_tmpCC2=_cycalloc(sizeof(*_tmpCC2)),((
_tmpCC2->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpCC2->f2=0,((
_tmpCC2->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpCC2)))))))),_tmpCC1))));}break;case 132: _LL284:{struct
_tuple16*_tmpCC5;union Cyc_YYSTYPE_union _tmpCC4;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCC4.YY35).tag=39,(((_tmpCC4.YY35).f1=((_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((
_tmpCC5->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1,((_tmpCC5->f2=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpCC5->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpCC5)))))))),_tmpCC4))));}break;case 133: _LL285:{union Cyc_YYSTYPE_union _tmpCC6;
yyval=(union Cyc_YYSTYPE_union)(((_tmpCC6.YY21).tag=25,(((_tmpCC6.YY21).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpCC6))));}break;
case 134: _LL286:{struct Cyc_List_List*_tmpCC9;union Cyc_YYSTYPE_union _tmpCC8;yyval=(
union Cyc_YYSTYPE_union)(((_tmpCC8.YY21).tag=25,(((_tmpCC8.YY21).f1=((_tmpCC9=
_cycalloc(sizeof(*_tmpCC9)),((_tmpCC9->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCC9->tl=0,_tmpCC9)))))),
_tmpCC8))));}break;case 135: _LL287:{struct Cyc_List_List*_tmpCCC;union Cyc_YYSTYPE_union
_tmpCCB;yyval=(union Cyc_YYSTYPE_union)(((_tmpCCB.YY21).tag=25,(((_tmpCCB.YY21).f1=((
_tmpCCC=_cycalloc(sizeof(*_tmpCCC)),((_tmpCCC->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCCC->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpCCC)))))),
_tmpCCB))));}break;case 136: _LL288:{struct _tuple15*_tmpCCF;union Cyc_YYSTYPE_union
_tmpCCE;yyval=(union Cyc_YYSTYPE_union)(((_tmpCCE.YY20).tag=24,(((_tmpCCE.YY20).f1=((
_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCCF->f2=0,_tmpCCF)))))),
_tmpCCE))));}break;case 137: _LL289:{struct _tuple15*_tmpCDE;struct _tuple1*_tmpCDD;
union Cyc_Absyn_Nmspace_union _tmpCDC;struct Cyc_Parse_Declarator*_tmpCDB;union Cyc_YYSTYPE_union
_tmpCDA;yyval=(union Cyc_YYSTYPE_union)(((_tmpCDA.YY20).tag=24,(((_tmpCDA.YY20).f1=((
_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((_tmpCDE->f1=((_tmpCDB=_cycalloc(sizeof(*
_tmpCDB)),((_tmpCDB->id=((_tmpCDD=_cycalloc(sizeof(*_tmpCDD)),((_tmpCDD->f1=(
union Cyc_Absyn_Nmspace_union)(((_tmpCDC.Rel_n).tag=1,(((_tmpCDC.Rel_n).f1=0,
_tmpCDC)))),((_tmpCDD->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),"",sizeof(char),1),_tmpCDD)))))),((_tmpCDB->tms=0,_tmpCDB)))))),((
_tmpCDE->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCDE)))))),_tmpCDA))));}
break;case 138: _LL28A:{struct _tuple15*_tmpCE1;union Cyc_YYSTYPE_union _tmpCE0;yyval=(
union Cyc_YYSTYPE_union)(((_tmpCE0.YY20).tag=24,(((_tmpCE0.YY20).f1=((_tmpCE1=
_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpCE1->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCE1)))))),
_tmpCE0))));}break;case 139: _LL28B: {struct Cyc_List_List*_tmp559=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct*
_tmpCEB;struct Cyc_Core_Opt*_tmpCEA;struct Cyc_Parse_Decl_spec_struct _tmpCE9;union
Cyc_YYSTYPE_union _tmpCE8;yyval=(union Cyc_YYSTYPE_union)(((_tmpCE8.YY23).tag=27,(((
_tmpCE8.YY23).f1=(void*)((void*)((_tmpCEB=_cycalloc(sizeof(*_tmpCEB)),((_tmpCEB[
0]=((_tmpCE9.tag=5,((_tmpCE9.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp559,((_tmpCEA=
_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA->v=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpCEA)))),Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpCE9)))),
_tmpCEB))))),_tmpCE8))));}break;}case 140: _LL28C:{struct Cyc_Absyn_TunionType_struct*
_tmpD00;struct Cyc_Absyn_TunionInfo _tmpCFF;struct Cyc_Absyn_UnknownTunionInfo
_tmpCFE;union Cyc_Absyn_TunionInfoU_union _tmpCFD;struct Cyc_Absyn_TunionType_struct
_tmpCFC;union Cyc_YYSTYPE_union _tmpCFB;yyval=(union Cyc_YYSTYPE_union)(((_tmpCFB.YY23).tag=
27,(((_tmpCFB.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpD00=_cycalloc(
sizeof(*_tmpD00)),((_tmpD00[0]=((_tmpCFC.tag=2,((_tmpCFC.f1=((_tmpCFF.tunion_info=(
union Cyc_Absyn_TunionInfoU_union)(((_tmpCFD.UnknownTunion).tag=0,(((_tmpCFD.UnknownTunion).f1=((
_tmpCFE.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpCFE.is_xtunion=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpCFE.is_flat=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmpCFE)))))),
_tmpCFD)))),((_tmpCFF.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpCFF.rgn=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpCFF)))))),_tmpCFC)))),
_tmpD00)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpCFB))));}break;case 141: _LL28D:{struct Cyc_Absyn_TunionFieldType_struct*
_tmpD15;struct Cyc_Absyn_TunionFieldInfo _tmpD14;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmpD13;union Cyc_Absyn_TunionFieldInfoU_union _tmpD12;struct Cyc_Absyn_TunionFieldType_struct
_tmpD11;union Cyc_YYSTYPE_union _tmpD10;yyval=(union Cyc_YYSTYPE_union)(((_tmpD10.YY23).tag=
27,(((_tmpD10.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpD15=_cycalloc(
sizeof(*_tmpD15)),((_tmpD15[0]=((_tmpD11.tag=3,((_tmpD11.f1=((_tmpD14.field_info=(
union Cyc_Absyn_TunionFieldInfoU_union)(((_tmpD12.UnknownTunionfield).tag=0,(((
_tmpD12.UnknownTunionfield).f1=((_tmpD13.tunion_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpD13.field_name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD13.is_xtunion=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
_tmpD13)))))),_tmpD12)))),((_tmpD14.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD14)))),_tmpD11)))),
_tmpD15)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpD10))));}break;case 142: _LL28E:{union Cyc_YYSTYPE_union
_tmpD16;yyval=(union Cyc_YYSTYPE_union)(((_tmpD16.YY31).tag=35,(((_tmpD16.YY31).f1=
1,_tmpD16))));}break;case 143: _LL28F:{union Cyc_YYSTYPE_union _tmpD17;yyval=(union
Cyc_YYSTYPE_union)(((_tmpD17.YY31).tag=35,(((_tmpD17.YY31).f1=0,_tmpD17))));}
break;case 144: _LL290:{union Cyc_YYSTYPE_union _tmpD18;yyval=(union Cyc_YYSTYPE_union)(((
_tmpD18.YY31).tag=35,(((_tmpD18.YY31).f1=0,_tmpD18))));}break;case 145: _LL291:{
union Cyc_YYSTYPE_union _tmpD19;yyval=(union Cyc_YYSTYPE_union)(((_tmpD19.YY31).tag=
35,(((_tmpD19.YY31).f1=1,_tmpD19))));}break;case 146: _LL292:{struct Cyc_List_List*
_tmpD1C;union Cyc_YYSTYPE_union _tmpD1B;yyval=(union Cyc_YYSTYPE_union)(((_tmpD1B.YY34).tag=
38,(((_tmpD1B.YY34).f1=((_tmpD1C=_cycalloc(sizeof(*_tmpD1C)),((_tmpD1C->hd=Cyc_yyget_YY33(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD1C->tl=0,
_tmpD1C)))))),_tmpD1B))));}break;case 147: _LL293:{struct Cyc_List_List*_tmpD1F;
union Cyc_YYSTYPE_union _tmpD1E;yyval=(union Cyc_YYSTYPE_union)(((_tmpD1E.YY34).tag=
38,(((_tmpD1E.YY34).f1=((_tmpD1F=_cycalloc(sizeof(*_tmpD1F)),((_tmpD1F->hd=Cyc_yyget_YY33(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD1F->tl=0,
_tmpD1F)))))),_tmpD1E))));}break;case 148: _LL294:{struct Cyc_List_List*_tmpD22;
union Cyc_YYSTYPE_union _tmpD21;yyval=(union Cyc_YYSTYPE_union)(((_tmpD21.YY34).tag=
38,(((_tmpD21.YY34).f1=((_tmpD22=_cycalloc(sizeof(*_tmpD22)),((_tmpD22->hd=Cyc_yyget_YY33(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD22->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD22)))))),_tmpD21))));}
break;case 149: _LL295:{struct Cyc_List_List*_tmpD25;union Cyc_YYSTYPE_union _tmpD24;
yyval=(union Cyc_YYSTYPE_union)(((_tmpD24.YY34).tag=38,(((_tmpD24.YY34).f1=((
_tmpD25=_cycalloc(sizeof(*_tmpD25)),((_tmpD25->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD25->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD25)))))),_tmpD24))));}
break;case 150: _LL296:{union Cyc_YYSTYPE_union _tmpD26;yyval=(union Cyc_YYSTYPE_union)(((
_tmpD26.YY32).tag=36,(((_tmpD26.YY32).f1=(void*)((void*)2),_tmpD26))));}break;
case 151: _LL297:{union Cyc_YYSTYPE_union _tmpD27;yyval=(union Cyc_YYSTYPE_union)(((
_tmpD27.YY32).tag=36,(((_tmpD27.YY32).f1=(void*)((void*)3),_tmpD27))));}break;
case 152: _LL298:{union Cyc_YYSTYPE_union _tmpD28;yyval=(union Cyc_YYSTYPE_union)(((
_tmpD28.YY32).tag=36,(((_tmpD28.YY32).f1=(void*)((void*)0),_tmpD28))));}break;
case 153: _LL299:{struct Cyc_Absyn_Tunionfield*_tmpD2B;union Cyc_YYSTYPE_union
_tmpD2A;yyval=(union Cyc_YYSTYPE_union)(((_tmpD2A.YY33).tag=37,(((_tmpD2A.YY33).f1=((
_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD2B->typs=0,((_tmpD2B->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpD2B->sc=(void*)Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD2B)))))))))),
_tmpD2A))));}break;case 154: _LL29A: {struct Cyc_List_List*_tmp57B=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Absyn_Tunionfield*
_tmpD2E;union Cyc_YYSTYPE_union _tmpD2D;yyval=(union Cyc_YYSTYPE_union)(((_tmpD2D.YY33).tag=
37,(((_tmpD2D.YY33).f1=((_tmpD2E=_cycalloc(sizeof(*_tmpD2E)),((_tmpD2E->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpD2E->typs=
_tmp57B,((_tmpD2E->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD2E->sc=(
void*)Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tmpD2E)))))))))),_tmpD2D))));}break;}case 155: _LL29B: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 156: _LL29C:{struct
Cyc_Parse_Declarator*_tmpD31;union Cyc_YYSTYPE_union _tmpD30;yyval=(union Cyc_YYSTYPE_union)(((
_tmpD30.YY29).tag=33,(((_tmpD30.YY29).f1=((_tmpD31=_cycalloc(sizeof(*_tmpD31)),((
_tmpD31->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id,((_tmpD31->tms=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpD31)))))),
_tmpD30))));}break;case 157: _LL29D: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 158: _LL29E:{struct Cyc_Parse_Declarator*_tmpD34;
union Cyc_YYSTYPE_union _tmpD33;yyval=(union Cyc_YYSTYPE_union)(((_tmpD33.YY29).tag=
33,(((_tmpD33.YY29).f1=((_tmpD34=_cycalloc(sizeof(*_tmpD34)),((_tmpD34->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->id,((_tmpD34->tms=
Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->tms),_tmpD34)))))),_tmpD33))));}break;case 159: _LL29F:{struct
Cyc_Parse_Declarator*_tmpD37;union Cyc_YYSTYPE_union _tmpD36;yyval=(union Cyc_YYSTYPE_union)(((
_tmpD36.YY29).tag=33,(((_tmpD36.YY29).f1=((_tmpD37=_cycalloc(sizeof(*_tmpD37)),((
_tmpD37->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpD37->tms=0,_tmpD37)))))),_tmpD36))));}break;case 160: _LL2A0:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 161:
_LL2A1: {struct Cyc_Parse_Declarator*_tmp584=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpD3D;struct Cyc_Absyn_Attributes_mod_struct _tmpD3C;struct Cyc_List_List*_tmpD3B;
_tmp584->tms=((_tmpD3B=_cycalloc(sizeof(*_tmpD3B)),((_tmpD3B->hd=(void*)((void*)((
_tmpD3D=_cycalloc(sizeof(*_tmpD3D)),((_tmpD3D[0]=((_tmpD3C.tag=5,((_tmpD3C.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpD3C.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpD3C)))))),_tmpD3D))))),((
_tmpD3B->tl=_tmp584->tms,_tmpD3B))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 162: _LL2A2:{
struct Cyc_Parse_Declarator*_tmpD4C;struct Cyc_Absyn_Carray_mod_struct*_tmpD4B;
struct Cyc_Absyn_Carray_mod_struct _tmpD4A;struct Cyc_List_List*_tmpD49;union Cyc_YYSTYPE_union
_tmpD48;yyval=(union Cyc_YYSTYPE_union)(((_tmpD48.YY29).tag=33,(((_tmpD48.YY29).f1=((
_tmpD4C=_cycalloc(sizeof(*_tmpD4C)),((_tmpD4C->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpD4C->tms=((
_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49->hd=(void*)((void*)((_tmpD4B=
_cycalloc(sizeof(*_tmpD4B)),((_tmpD4B[0]=((_tmpD4A.tag=0,((_tmpD4A.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD4A.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD4A)))))),
_tmpD4B))))),((_tmpD49->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpD49)))))),_tmpD4C)))))),_tmpD48))));}break;
case 163: _LL2A3:{struct Cyc_Parse_Declarator*_tmpD5B;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpD5A;struct Cyc_Absyn_ConstArray_mod_struct _tmpD59;struct Cyc_List_List*_tmpD58;
union Cyc_YYSTYPE_union _tmpD57;yyval=(union Cyc_YYSTYPE_union)(((_tmpD57.YY29).tag=
33,(((_tmpD57.YY29).f1=((_tmpD5B=_cycalloc(sizeof(*_tmpD5B)),((_tmpD5B->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpD5B->tms=((
_tmpD58=_cycalloc(sizeof(*_tmpD58)),((_tmpD58->hd=(void*)((void*)((_tmpD5A=
_cycalloc(sizeof(*_tmpD5A)),((_tmpD5A[0]=((_tmpD59.tag=1,((_tmpD59.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD59.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD59.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD59)))))))),
_tmpD5A))))),((_tmpD58->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpD58)))))),_tmpD5B)))))),_tmpD57))));}break;
case 164: _LL2A4: {struct _tuple17 _tmp593;struct Cyc_List_List*_tmp594;int _tmp595;
struct Cyc_Absyn_VarargInfo*_tmp596;struct Cyc_Core_Opt*_tmp597;struct Cyc_List_List*
_tmp598;struct _tuple17*_tmp592=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp593=*_tmp592;_tmp594=
_tmp593.f1;_tmp595=_tmp593.f2;_tmp596=_tmp593.f3;_tmp597=_tmp593.f4;_tmp598=
_tmp593.f5;{struct Cyc_Parse_Declarator*_tmpD77;struct Cyc_Absyn_Function_mod_struct*
_tmpD76;struct Cyc_Absyn_WithTypes_struct*_tmpD75;struct Cyc_Absyn_WithTypes_struct
_tmpD74;struct Cyc_Absyn_Function_mod_struct _tmpD73;struct Cyc_List_List*_tmpD72;
union Cyc_YYSTYPE_union _tmpD71;yyval=(union Cyc_YYSTYPE_union)(((_tmpD71.YY29).tag=
33,(((_tmpD71.YY29).f1=((_tmpD77=_cycalloc(sizeof(*_tmpD77)),((_tmpD77->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpD77->tms=((
_tmpD72=_cycalloc(sizeof(*_tmpD72)),((_tmpD72->hd=(void*)((void*)((_tmpD76=
_cycalloc(sizeof(*_tmpD76)),((_tmpD76[0]=((_tmpD73.tag=3,((_tmpD73.f1=(void*)((
void*)((_tmpD75=_cycalloc(sizeof(*_tmpD75)),((_tmpD75[0]=((_tmpD74.tag=1,((
_tmpD74.f1=_tmp594,((_tmpD74.f2=_tmp595,((_tmpD74.f3=_tmp596,((_tmpD74.f4=
_tmp597,((_tmpD74.f5=_tmp598,_tmpD74)))))))))))),_tmpD75))))),_tmpD73)))),
_tmpD76))))),((_tmpD72->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpD72)))))),_tmpD77)))))),_tmpD71))));}break;}
case 165: _LL2A5:{struct Cyc_Parse_Declarator*_tmpD93;struct Cyc_Absyn_Function_mod_struct*
_tmpD92;struct Cyc_Absyn_WithTypes_struct*_tmpD91;struct Cyc_Absyn_WithTypes_struct
_tmpD90;struct Cyc_Absyn_Function_mod_struct _tmpD8F;struct Cyc_List_List*_tmpD8E;
union Cyc_YYSTYPE_union _tmpD8D;yyval=(union Cyc_YYSTYPE_union)(((_tmpD8D.YY29).tag=
33,(((_tmpD8D.YY29).f1=((_tmpD93=_cycalloc(sizeof(*_tmpD93)),((_tmpD93->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpD93->tms=((
_tmpD8E=_cycalloc(sizeof(*_tmpD8E)),((_tmpD8E->hd=(void*)((void*)((_tmpD92=
_cycalloc(sizeof(*_tmpD92)),((_tmpD92[0]=((_tmpD8F.tag=3,((_tmpD8F.f1=(void*)((
void*)((_tmpD91=_cycalloc(sizeof(*_tmpD91)),((_tmpD91[0]=((_tmpD90.tag=1,((
_tmpD90.f1=0,((_tmpD90.f2=0,((_tmpD90.f3=0,((_tmpD90.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD90.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD90)))))))))))),
_tmpD91))))),_tmpD8F)))),_tmpD92))))),((_tmpD8E->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpD8E)))))),
_tmpD93)))))),_tmpD8D))));}break;case 166: _LL2A6:{struct Cyc_Parse_Declarator*
_tmpDAF;struct Cyc_Absyn_Function_mod_struct*_tmpDAE;struct Cyc_Absyn_NoTypes_struct*
_tmpDAD;struct Cyc_Absyn_NoTypes_struct _tmpDAC;struct Cyc_Absyn_Function_mod_struct
_tmpDAB;struct Cyc_List_List*_tmpDAA;union Cyc_YYSTYPE_union _tmpDA9;yyval=(union
Cyc_YYSTYPE_union)(((_tmpDA9.YY29).tag=33,(((_tmpDA9.YY29).f1=((_tmpDAF=
_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpDAF->tms=((
_tmpDAA=_cycalloc(sizeof(*_tmpDAA)),((_tmpDAA->hd=(void*)((void*)((_tmpDAE=
_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE[0]=((_tmpDAB.tag=3,((_tmpDAB.f1=(void*)((
void*)((_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD[0]=((_tmpDAC.tag=0,((
_tmpDAC.f1=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),((_tmpDAC.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpDAC)))))),
_tmpDAD))))),_tmpDAB)))),_tmpDAE))))),((_tmpDAA->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmpDAA)))))),
_tmpDAF)))))),_tmpDA9))));}break;case 167: _LL2A7: {struct Cyc_List_List*_tmp5AE=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Parse_Declarator*
_tmpDBE;struct Cyc_Absyn_TypeParams_mod_struct*_tmpDBD;struct Cyc_Absyn_TypeParams_mod_struct
_tmpDBC;struct Cyc_List_List*_tmpDBB;union Cyc_YYSTYPE_union _tmpDBA;yyval=(union
Cyc_YYSTYPE_union)(((_tmpDBA.YY29).tag=33,(((_tmpDBA.YY29).f1=((_tmpDBE=
_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpDBE->tms=((
_tmpDBB=_cycalloc(sizeof(*_tmpDBB)),((_tmpDBB->hd=(void*)((void*)((_tmpDBD=
_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD[0]=((_tmpDBC.tag=4,((_tmpDBC.f1=_tmp5AE,((
_tmpDBC.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpDBC.f3=0,_tmpDBC)))))))),_tmpDBD))))),((_tmpDBB->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpDBB)))))),_tmpDBE)))))),_tmpDBA))));}break;}case 168: _LL2A8:{struct Cyc_Parse_Declarator*
_tmpDCD;struct Cyc_Absyn_Attributes_mod_struct*_tmpDCC;struct Cyc_Absyn_Attributes_mod_struct
_tmpDCB;struct Cyc_List_List*_tmpDCA;union Cyc_YYSTYPE_union _tmpDC9;yyval=(union
Cyc_YYSTYPE_union)(((_tmpDC9.YY29).tag=33,(((_tmpDC9.YY29).f1=((_tmpDCD=
_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpDCD->tms=((
_tmpDCA=_cycalloc(sizeof(*_tmpDCA)),((_tmpDCA->hd=(void*)((void*)((_tmpDCC=
_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC[0]=((_tmpDCB.tag=5,((_tmpDCB.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpDCB.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDCB)))))),_tmpDCC))))),((
_tmpDCA->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpDCA)))))),_tmpDCD)))))),_tmpDC9))));}break;case 169:
_LL2A9:{struct Cyc_Parse_Declarator*_tmpDD0;union Cyc_YYSTYPE_union _tmpDCF;yyval=(
union Cyc_YYSTYPE_union)(((_tmpDCF.YY29).tag=33,(((_tmpDCF.YY29).f1=((_tmpDD0=
_cycalloc(sizeof(*_tmpDD0)),((_tmpDD0->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpDD0->tms=0,_tmpDD0)))))),
_tmpDCF))));}break;case 170: _LL2AA:{struct Cyc_Parse_Declarator*_tmpDD3;union Cyc_YYSTYPE_union
_tmpDD2;yyval=(union Cyc_YYSTYPE_union)(((_tmpDD2.YY29).tag=33,(((_tmpDD2.YY29).f1=((
_tmpDD3=_cycalloc(sizeof(*_tmpDD3)),((_tmpDD3->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpDD3->tms=0,_tmpDD3)))))),
_tmpDD2))));}break;case 171: _LL2AB: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 172: _LL2AC: {struct Cyc_Parse_Declarator*
_tmp5BD=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{
struct Cyc_Absyn_Attributes_mod_struct*_tmpDD9;struct Cyc_Absyn_Attributes_mod_struct
_tmpDD8;struct Cyc_List_List*_tmpDD7;_tmp5BD->tms=((_tmpDD7=_cycalloc(sizeof(*
_tmpDD7)),((_tmpDD7->hd=(void*)((void*)((_tmpDD9=_cycalloc(sizeof(*_tmpDD9)),((
_tmpDD9[0]=((_tmpDD8.tag=5,((_tmpDD8.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpDD8.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpDD8)))))),_tmpDD9))))),((_tmpDD7->tl=_tmp5BD->tms,_tmpDD7))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 173: _LL2AD:{
struct Cyc_Parse_Declarator*_tmpDE8;struct Cyc_Absyn_Carray_mod_struct*_tmpDE7;
struct Cyc_Absyn_Carray_mod_struct _tmpDE6;struct Cyc_List_List*_tmpDE5;union Cyc_YYSTYPE_union
_tmpDE4;yyval=(union Cyc_YYSTYPE_union)(((_tmpDE4.YY29).tag=33,(((_tmpDE4.YY29).f1=((
_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpDE8->tms=((
_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5->hd=(void*)((void*)((_tmpDE7=
_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7[0]=((_tmpDE6.tag=0,((_tmpDE6.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpDE6.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpDE6)))))),
_tmpDE7))))),((_tmpDE5->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpDE5)))))),_tmpDE8)))))),_tmpDE4))));}break;
case 174: _LL2AE:{struct Cyc_Parse_Declarator*_tmpDF7;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpDF6;struct Cyc_Absyn_ConstArray_mod_struct _tmpDF5;struct Cyc_List_List*_tmpDF4;
union Cyc_YYSTYPE_union _tmpDF3;yyval=(union Cyc_YYSTYPE_union)(((_tmpDF3.YY29).tag=
33,(((_tmpDF3.YY29).f1=((_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((_tmpDF7->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpDF7->tms=((
_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4->hd=(void*)((void*)((_tmpDF6=
_cycalloc(sizeof(*_tmpDF6)),((_tmpDF6[0]=((_tmpDF5.tag=1,((_tmpDF5.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDF5.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpDF5.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpDF5)))))))),
_tmpDF6))))),((_tmpDF4->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpDF4)))))),_tmpDF7)))))),_tmpDF3))));}break;
case 175: _LL2AF: {struct _tuple17 _tmp5CC;struct Cyc_List_List*_tmp5CD;int _tmp5CE;
struct Cyc_Absyn_VarargInfo*_tmp5CF;struct Cyc_Core_Opt*_tmp5D0;struct Cyc_List_List*
_tmp5D1;struct _tuple17*_tmp5CB=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5CC=*_tmp5CB;_tmp5CD=
_tmp5CC.f1;_tmp5CE=_tmp5CC.f2;_tmp5CF=_tmp5CC.f3;_tmp5D0=_tmp5CC.f4;_tmp5D1=
_tmp5CC.f5;{struct Cyc_Parse_Declarator*_tmpE13;struct Cyc_Absyn_Function_mod_struct*
_tmpE12;struct Cyc_Absyn_WithTypes_struct*_tmpE11;struct Cyc_Absyn_WithTypes_struct
_tmpE10;struct Cyc_Absyn_Function_mod_struct _tmpE0F;struct Cyc_List_List*_tmpE0E;
union Cyc_YYSTYPE_union _tmpE0D;yyval=(union Cyc_YYSTYPE_union)(((_tmpE0D.YY29).tag=
33,(((_tmpE0D.YY29).f1=((_tmpE13=_cycalloc(sizeof(*_tmpE13)),((_tmpE13->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpE13->tms=((
_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E->hd=(void*)((void*)((_tmpE12=
_cycalloc(sizeof(*_tmpE12)),((_tmpE12[0]=((_tmpE0F.tag=3,((_tmpE0F.f1=(void*)((
void*)((_tmpE11=_cycalloc(sizeof(*_tmpE11)),((_tmpE11[0]=((_tmpE10.tag=1,((
_tmpE10.f1=_tmp5CD,((_tmpE10.f2=_tmp5CE,((_tmpE10.f3=_tmp5CF,((_tmpE10.f4=
_tmp5D0,((_tmpE10.f5=_tmp5D1,_tmpE10)))))))))))),_tmpE11))))),_tmpE0F)))),
_tmpE12))))),((_tmpE0E->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpE0E)))))),_tmpE13)))))),_tmpE0D))));}break;}
case 176: _LL2B0:{struct Cyc_Parse_Declarator*_tmpE2F;struct Cyc_Absyn_Function_mod_struct*
_tmpE2E;struct Cyc_Absyn_WithTypes_struct*_tmpE2D;struct Cyc_Absyn_WithTypes_struct
_tmpE2C;struct Cyc_Absyn_Function_mod_struct _tmpE2B;struct Cyc_List_List*_tmpE2A;
union Cyc_YYSTYPE_union _tmpE29;yyval=(union Cyc_YYSTYPE_union)(((_tmpE29.YY29).tag=
33,(((_tmpE29.YY29).f1=((_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpE2F->tms=((
_tmpE2A=_cycalloc(sizeof(*_tmpE2A)),((_tmpE2A->hd=(void*)((void*)((_tmpE2E=
_cycalloc(sizeof(*_tmpE2E)),((_tmpE2E[0]=((_tmpE2B.tag=3,((_tmpE2B.f1=(void*)((
void*)((_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((_tmpE2D[0]=((_tmpE2C.tag=1,((
_tmpE2C.f1=0,((_tmpE2C.f2=0,((_tmpE2C.f3=0,((_tmpE2C.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpE2C.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE2C)))))))))))),
_tmpE2D))))),_tmpE2B)))),_tmpE2E))))),((_tmpE2A->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpE2A)))))),
_tmpE2F)))))),_tmpE29))));}break;case 177: _LL2B1:{struct Cyc_Parse_Declarator*
_tmpE4B;struct Cyc_Absyn_Function_mod_struct*_tmpE4A;struct Cyc_Absyn_NoTypes_struct*
_tmpE49;struct Cyc_Absyn_NoTypes_struct _tmpE48;struct Cyc_Absyn_Function_mod_struct
_tmpE47;struct Cyc_List_List*_tmpE46;union Cyc_YYSTYPE_union _tmpE45;yyval=(union
Cyc_YYSTYPE_union)(((_tmpE45.YY29).tag=33,(((_tmpE45.YY29).f1=((_tmpE4B=
_cycalloc(sizeof(*_tmpE4B)),((_tmpE4B->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpE4B->tms=((
_tmpE46=_cycalloc(sizeof(*_tmpE46)),((_tmpE46->hd=(void*)((void*)((_tmpE4A=
_cycalloc(sizeof(*_tmpE4A)),((_tmpE4A[0]=((_tmpE47.tag=3,((_tmpE47.f1=(void*)((
void*)((_tmpE49=_cycalloc(sizeof(*_tmpE49)),((_tmpE49[0]=((_tmpE48.tag=0,((
_tmpE48.f1=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),((_tmpE48.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpE48)))))),
_tmpE49))))),_tmpE47)))),_tmpE4A))))),((_tmpE46->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmpE46)))))),
_tmpE4B)))))),_tmpE45))));}break;case 178: _LL2B2: {struct Cyc_List_List*_tmp5E7=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Parse_Declarator*
_tmpE5A;struct Cyc_Absyn_TypeParams_mod_struct*_tmpE59;struct Cyc_Absyn_TypeParams_mod_struct
_tmpE58;struct Cyc_List_List*_tmpE57;union Cyc_YYSTYPE_union _tmpE56;yyval=(union
Cyc_YYSTYPE_union)(((_tmpE56.YY29).tag=33,(((_tmpE56.YY29).f1=((_tmpE5A=
_cycalloc(sizeof(*_tmpE5A)),((_tmpE5A->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpE5A->tms=((
_tmpE57=_cycalloc(sizeof(*_tmpE57)),((_tmpE57->hd=(void*)((void*)((_tmpE59=
_cycalloc(sizeof(*_tmpE59)),((_tmpE59[0]=((_tmpE58.tag=4,((_tmpE58.f1=_tmp5E7,((
_tmpE58.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpE58.f3=0,_tmpE58)))))))),_tmpE59))))),((_tmpE57->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpE57)))))),_tmpE5A)))))),_tmpE56))));}break;}case 179: _LL2B3:{struct Cyc_Parse_Declarator*
_tmpE69;struct Cyc_Absyn_Attributes_mod_struct*_tmpE68;struct Cyc_Absyn_Attributes_mod_struct
_tmpE67;struct Cyc_List_List*_tmpE66;union Cyc_YYSTYPE_union _tmpE65;yyval=(union
Cyc_YYSTYPE_union)(((_tmpE65.YY29).tag=33,(((_tmpE65.YY29).f1=((_tmpE69=
_cycalloc(sizeof(*_tmpE69)),((_tmpE69->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpE69->tms=((
_tmpE66=_cycalloc(sizeof(*_tmpE66)),((_tmpE66->hd=(void*)((void*)((_tmpE68=
_cycalloc(sizeof(*_tmpE68)),((_tmpE68[0]=((_tmpE67.tag=5,((_tmpE67.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpE67.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE67)))))),_tmpE68))))),((
_tmpE66->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpE66)))))),_tmpE69)))))),_tmpE65))));}break;case 180:
_LL2B4: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
181: _LL2B5:{union Cyc_YYSTYPE_union _tmpE6A;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE6A.YY28).tag=32,(((_tmpE6A.YY28).f1=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpE6A))));}break;case 182:
_LL2B6: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){struct Cyc_Absyn_Attributes_mod_struct*
_tmpE70;struct Cyc_Absyn_Attributes_mod_struct _tmpE6F;struct Cyc_List_List*_tmpE6E;
ans=((_tmpE6E=_cycalloc(sizeof(*_tmpE6E)),((_tmpE6E->hd=(void*)((void*)((_tmpE70=
_cycalloc(sizeof(*_tmpE70)),((_tmpE70[0]=((_tmpE6F.tag=5,((_tmpE6F.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpE6F.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpE6F)))))),_tmpE70))))),((_tmpE6E->tl=ans,_tmpE6E))))));}{struct Cyc_Absyn_PtrLoc*
ptrloc=0;if(Cyc_Absyn_porting_c_code){struct Cyc_Absyn_PtrLoc*_tmpE71;ptrloc=((
_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71->ptr_loc=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1,((_tmpE71->rgn_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpE71->zt_loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),_tmpE71))))))));}{
struct Cyc_Absyn_Pointer_mod_struct*_tmpE7B;struct Cyc_Absyn_PtrAtts _tmpE7A;struct
Cyc_Absyn_Pointer_mod_struct _tmpE79;struct Cyc_List_List*_tmpE78;ans=((_tmpE78=
_cycalloc(sizeof(*_tmpE78)),((_tmpE78->hd=(void*)((void*)((_tmpE7B=_cycalloc(
sizeof(*_tmpE7B)),((_tmpE7B[0]=((_tmpE79.tag=2,((_tmpE79.f1=((_tmpE7A.rgn=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpE7A.nullable=(*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])).f2,((_tmpE7A.bounds=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f3,((_tmpE7A.zero_term=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpE7A.ptrloc=ptrloc,_tmpE7A)))))))))),((_tmpE79.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE79)))))),_tmpE7B))))),((
_tmpE78->tl=ans,_tmpE78))))));}{union Cyc_YYSTYPE_union _tmpE7C;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE7C.YY28).tag=32,(((_tmpE7C.YY28).f1=ans,_tmpE7C))));}break;}}case 183: _LL2B7: {
struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line): 0;{struct
_tuple12*_tmpE7F;union Cyc_YYSTYPE_union _tmpE7E;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE7E.YY1).tag=5,(((_tmpE7E.YY1).f1=((_tmpE7F=_cycalloc(sizeof(*_tmpE7F)),((
_tmpE7F->f1=loc,((_tmpE7F->f2=Cyc_Absyn_true_conref,((_tmpE7F->f3=Cyc_yyget_YY2(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE7F)))))))),
_tmpE7E))));}break;}case 184: _LL2B8: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line): 0;{struct _tuple12*_tmpE82;union Cyc_YYSTYPE_union
_tmpE81;yyval=(union Cyc_YYSTYPE_union)(((_tmpE81.YY1).tag=5,(((_tmpE81.YY1).f1=((
_tmpE82=_cycalloc(sizeof(*_tmpE82)),((_tmpE82->f1=loc,((_tmpE82->f2=Cyc_Absyn_false_conref,((
_tmpE82->f3=Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpE82)))))))),_tmpE81))));}break;}case 185: _LL2B9: {struct Cyc_Position_Segment*
loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line): 0;{struct
_tuple12*_tmpE85;union Cyc_YYSTYPE_union _tmpE84;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE84.YY1).tag=5,(((_tmpE84.YY1).f1=((_tmpE85=_cycalloc(sizeof(*_tmpE85)),((
_tmpE85->f1=loc,((_tmpE85->f2=Cyc_Absyn_true_conref,((_tmpE85->f3=Cyc_Absyn_bounds_dyneither_conref,
_tmpE85)))))))),_tmpE84))));}break;}case 186: _LL2BA:{union Cyc_YYSTYPE_union
_tmpE86;yyval=(union Cyc_YYSTYPE_union)(((_tmpE86.YY2).tag=6,(((_tmpE86.YY2).f1=
Cyc_Absyn_bounds_one_conref,_tmpE86))));}break;case 187: _LL2BB:{struct Cyc_Absyn_Upper_b_struct*
_tmpE8C;struct Cyc_Absyn_Upper_b_struct _tmpE8B;union Cyc_YYSTYPE_union _tmpE8A;
yyval=(union Cyc_YYSTYPE_union)(((_tmpE8A.YY2).tag=6,(((_tmpE8A.YY2).f1=Cyc_Absyn_new_conref((
void*)((_tmpE8C=_cycalloc(sizeof(*_tmpE8C)),((_tmpE8C[0]=((_tmpE8B.tag=0,((
_tmpE8B.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]),_tmpE8B)))),_tmpE8C))))),_tmpE8A))));}break;case 188: _LL2BC:{union Cyc_YYSTYPE_union
_tmpE8D;yyval=(union Cyc_YYSTYPE_union)(((_tmpE8D.YY51).tag=55,(((_tmpE8D.YY51).f1=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),_tmpE8D))));}break;case
189: _LL2BD:{union Cyc_YYSTYPE_union _tmpE8E;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE8E.YY51).tag=55,(((_tmpE8E.YY51).f1=Cyc_Absyn_true_conref,_tmpE8E))));}
break;case 190: _LL2BE:{union Cyc_YYSTYPE_union _tmpE8F;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE8F.YY51).tag=55,(((_tmpE8F.YY51).f1=Cyc_Absyn_false_conref,_tmpE8F))));}
break;case 191: _LL2BF:{union Cyc_YYSTYPE_union _tmpE90;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE90.YY49).tag=53,(((_tmpE90.YY49).f1=0,_tmpE90))));}break;case 192: _LL2C0: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)
3,1);{struct Cyc_Core_Opt*_tmpE93;union Cyc_YYSTYPE_union _tmpE92;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE92.YY49).tag=53,(((_tmpE92.YY49).f1=((_tmpE93=_cycalloc(sizeof(*_tmpE93)),((
_tmpE93->v=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE93)))),_tmpE92))));}break;case 193: _LL2C1:{struct Cyc_Core_Opt*
_tmpE96;union Cyc_YYSTYPE_union _tmpE95;yyval=(union Cyc_YYSTYPE_union)(((_tmpE95.YY49).tag=
53,(((_tmpE95.YY49).f1=((_tmpE96=_cycalloc(sizeof(*_tmpE96)),((_tmpE96->v=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0),_tmpE96)))),_tmpE95))));}
break;case 194: _LL2C2:{union Cyc_YYSTYPE_union _tmpE97;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE97.YY44).tag=48,(((_tmpE97.YY44).f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,0),_tmpE97))));}break;case 195: _LL2C3: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)
5,1);yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
196: _LL2C4:{union Cyc_YYSTYPE_union _tmpE98;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE98.YY44).tag=48,(((_tmpE98.YY44).f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,0),_tmpE98))));}break;case 197: _LL2C5:{union Cyc_YYSTYPE_union
_tmpE99;yyval=(union Cyc_YYSTYPE_union)(((_tmpE99.YY25).tag=29,(((_tmpE99.YY25).f1=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line)),_tmpE99))));}
break;case 198: _LL2C6:{union Cyc_YYSTYPE_union _tmpE9A;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE9A.YY25).tag=29,(((_tmpE9A.YY25).f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpE9A))));}break;case 199:
_LL2C7:{struct _tuple17*_tmpE9D;union Cyc_YYSTYPE_union _tmpE9C;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE9C.YY39).tag=43,(((_tmpE9C.YY39).f1=((_tmpE9D=_cycalloc(sizeof(*_tmpE9D)),((
_tmpE9D->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((
_tmpE9D->f2=0,((_tmpE9D->f3=0,((_tmpE9D->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE9D->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE9D)))))))))))),
_tmpE9C))));}break;case 200: _LL2C8:{struct _tuple17*_tmpEA0;union Cyc_YYSTYPE_union
_tmpE9F;yyval=(union Cyc_YYSTYPE_union)(((_tmpE9F.YY39).tag=43,(((_tmpE9F.YY39).f1=((
_tmpEA0=_cycalloc(sizeof(*_tmpEA0)),((_tmpEA0->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),((_tmpEA0->f2=1,((
_tmpEA0->f3=0,((_tmpEA0->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((_tmpEA0->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEA0)))))))))))),_tmpE9F))));}
break;case 201: _LL2C9: {struct _tuple2 _tmp617;struct Cyc_Core_Opt*_tmp618;struct Cyc_Absyn_Tqual
_tmp619;void*_tmp61A;struct _tuple2*_tmp616=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp617=*_tmp616;_tmp618=
_tmp617.f1;_tmp619=_tmp617.f2;_tmp61A=_tmp617.f3;{struct Cyc_Absyn_VarargInfo*
_tmpEA1;struct Cyc_Absyn_VarargInfo*_tmp61B=(_tmpEA1=_cycalloc(sizeof(*_tmpEA1)),((
_tmpEA1->name=_tmp618,((_tmpEA1->tq=_tmp619,((_tmpEA1->type=(void*)_tmp61A,((
_tmpEA1->inject=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),_tmpEA1)))))))));{struct _tuple17*_tmpEA4;union Cyc_YYSTYPE_union
_tmpEA3;yyval=(union Cyc_YYSTYPE_union)(((_tmpEA3.YY39).tag=43,(((_tmpEA3.YY39).f1=((
_tmpEA4=_cycalloc(sizeof(*_tmpEA4)),((_tmpEA4->f1=0,((_tmpEA4->f2=0,((_tmpEA4->f3=
_tmp61B,((_tmpEA4->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpEA4->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEA4)))))))))))),_tmpEA3))));}
break;}}case 202: _LL2CA: {struct _tuple2 _tmp620;struct Cyc_Core_Opt*_tmp621;struct
Cyc_Absyn_Tqual _tmp622;void*_tmp623;struct _tuple2*_tmp61F=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp620=*_tmp61F;_tmp621=
_tmp620.f1;_tmp622=_tmp620.f2;_tmp623=_tmp620.f3;{struct Cyc_Absyn_VarargInfo*
_tmpEA5;struct Cyc_Absyn_VarargInfo*_tmp624=(_tmpEA5=_cycalloc(sizeof(*_tmpEA5)),((
_tmpEA5->name=_tmp621,((_tmpEA5->tq=_tmp622,((_tmpEA5->type=(void*)_tmp623,((
_tmpEA5->inject=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),_tmpEA5)))))))));{struct _tuple17*_tmpEA8;union Cyc_YYSTYPE_union
_tmpEA7;yyval=(union Cyc_YYSTYPE_union)(((_tmpEA7.YY39).tag=43,(((_tmpEA7.YY39).f1=((
_tmpEA8=_cycalloc(sizeof(*_tmpEA8)),((_tmpEA8->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)])),((_tmpEA8->f2=0,((
_tmpEA8->f3=_tmp624,((_tmpEA8->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpEA8->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEA8)))))))))))),
_tmpEA7))));}break;}}case 203: _LL2CB:{struct Cyc_Absyn_Unknown_kb_struct*_tmpEAE;
struct Cyc_Absyn_Unknown_kb_struct _tmpEAD;union Cyc_YYSTYPE_union _tmpEAC;yyval=(
union Cyc_YYSTYPE_union)(((_tmpEAC.YY44).tag=48,(((_tmpEAC.YY44).f1=(void*)Cyc_Parse_id2type(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)((_tmpEAE=_cycalloc(sizeof(*_tmpEAE)),((_tmpEAE[0]=((_tmpEAD.tag=1,((
_tmpEAD.f1=0,_tmpEAD)))),_tmpEAE))))),_tmpEAC))));}break;case 204: _LL2CC:{union
Cyc_YYSTYPE_union _tmpEAF;yyval=(union Cyc_YYSTYPE_union)(((_tmpEAF.YY44).tag=48,(((
_tmpEAF.YY44).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))),
_tmpEAF))));}break;case 205: _LL2CD:{union Cyc_YYSTYPE_union _tmpEB0;yyval=(union Cyc_YYSTYPE_union)(((
_tmpEB0.YY49).tag=53,(((_tmpEB0.YY49).f1=0,_tmpEB0))));}break;case 206: _LL2CE:{
struct Cyc_Core_Opt*_tmpEBA;struct Cyc_Absyn_JoinEff_struct _tmpEB9;struct Cyc_Absyn_JoinEff_struct*
_tmpEB8;union Cyc_YYSTYPE_union _tmpEB7;yyval=(union Cyc_YYSTYPE_union)(((_tmpEB7.YY49).tag=
53,(((_tmpEB7.YY49).f1=((_tmpEBA=_cycalloc(sizeof(*_tmpEBA)),((_tmpEBA->v=(void*)((
void*)((_tmpEB8=_cycalloc(sizeof(*_tmpEB8)),((_tmpEB8[0]=((_tmpEB9.tag=20,((
_tmpEB9.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpEB9)))),_tmpEB8))))),_tmpEBA)))),_tmpEB7))));}break;case 207: _LL2CF:{union Cyc_YYSTYPE_union
_tmpEBB;yyval=(union Cyc_YYSTYPE_union)(((_tmpEBB.YY50).tag=54,(((_tmpEBB.YY50).f1=
0,_tmpEBB))));}break;case 208: _LL2D0: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 209: _LL2D1: {struct Cyc_Absyn_Less_kb_struct
_tmpEBE;struct Cyc_Absyn_Less_kb_struct*_tmpEBD;struct Cyc_Absyn_Less_kb_struct*
_tmp632=(_tmpEBD=_cycalloc(sizeof(*_tmpEBD)),((_tmpEBD[0]=((_tmpEBE.tag=2,((
_tmpEBE.f1=0,((_tmpEBE.f2=(void*)((void*)5),_tmpEBE)))))),_tmpEBD)));struct
_dyneither_ptr _tmp633=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);void*_tmp634=Cyc_Parse_id2type(_tmp633,(void*)_tmp632);{
struct Cyc_List_List*_tmpECD;struct Cyc_Absyn_JoinEff_struct*_tmpECC;struct Cyc_Absyn_JoinEff_struct
_tmpECB;struct _tuple4*_tmpECA;union Cyc_YYSTYPE_union _tmpEC9;yyval=(union Cyc_YYSTYPE_union)(((
_tmpEC9.YY50).tag=54,(((_tmpEC9.YY50).f1=((_tmpECD=_cycalloc(sizeof(*_tmpECD)),((
_tmpECD->hd=((_tmpECA=_cycalloc(sizeof(*_tmpECA)),((_tmpECA->f1=(void*)((_tmpECC=
_cycalloc(sizeof(*_tmpECC)),((_tmpECC[0]=((_tmpECB.tag=20,((_tmpECB.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpECB)))),_tmpECC)))),((
_tmpECA->f2=_tmp634,_tmpECA)))))),((_tmpECD->tl=0,_tmpECD)))))),_tmpEC9))));}
break;}case 210: _LL2D2: {struct Cyc_Absyn_Less_kb_struct _tmpED0;struct Cyc_Absyn_Less_kb_struct*
_tmpECF;struct Cyc_Absyn_Less_kb_struct*_tmp63C=(_tmpECF=_cycalloc(sizeof(*
_tmpECF)),((_tmpECF[0]=((_tmpED0.tag=2,((_tmpED0.f1=0,((_tmpED0.f2=(void*)((void*)
5),_tmpED0)))))),_tmpECF)));struct _dyneither_ptr _tmp63D=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);void*_tmp63E=Cyc_Parse_id2type(
_tmp63D,(void*)_tmp63C);{struct Cyc_List_List*_tmpEDF;struct Cyc_Absyn_JoinEff_struct*
_tmpEDE;struct Cyc_Absyn_JoinEff_struct _tmpEDD;struct _tuple4*_tmpEDC;union Cyc_YYSTYPE_union
_tmpEDB;yyval=(union Cyc_YYSTYPE_union)(((_tmpEDB.YY50).tag=54,(((_tmpEDB.YY50).f1=((
_tmpEDF=_cycalloc(sizeof(*_tmpEDF)),((_tmpEDF->hd=((_tmpEDC=_cycalloc(sizeof(*
_tmpEDC)),((_tmpEDC->f1=(void*)((_tmpEDE=_cycalloc(sizeof(*_tmpEDE)),((_tmpEDE[0]=((
_tmpEDD.tag=20,((_tmpEDD.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),_tmpEDD)))),_tmpEDE)))),((_tmpEDC->f2=_tmp63E,_tmpEDC)))))),((
_tmpEDF->tl=Cyc_yyget_YY50(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpEDF)))))),_tmpEDB))));}break;}case 211: _LL2D3:{union Cyc_YYSTYPE_union
_tmpEE0;yyval=(union Cyc_YYSTYPE_union)(((_tmpEE0.YY31).tag=35,(((_tmpEE0.YY31).f1=
0,_tmpEE0))));}break;case 212: _LL2D4:{const char*_tmpEE1;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpEE1="inject",_tag_dyneither(_tmpEE1,sizeof(char),7))))!= 0){
const char*_tmpEE2;Cyc_Parse_err(((_tmpEE2="missing type in function declaration",
_tag_dyneither(_tmpEE2,sizeof(char),37))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}{union Cyc_YYSTYPE_union
_tmpEE3;yyval=(union Cyc_YYSTYPE_union)(((_tmpEE3.YY31).tag=35,(((_tmpEE3.YY31).f1=
1,_tmpEE3))));}break;case 213: _LL2D5: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 214: _LL2D6:{union Cyc_YYSTYPE_union _tmpEE4;yyval=(
union Cyc_YYSTYPE_union)(((_tmpEE4.YY40).tag=44,(((_tmpEE4.YY40).f1=Cyc_List_imp_append(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpEE4))));}break;
case 215: _LL2D7:{union Cyc_YYSTYPE_union _tmpEE5;yyval=(union Cyc_YYSTYPE_union)(((
_tmpEE5.YY40).tag=44,(((_tmpEE5.YY40).f1=0,_tmpEE5))));}break;case 216: _LL2D8:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 217:
_LL2D9:{struct Cyc_List_List*_tmpEEF;struct Cyc_Absyn_RgnsEff_struct _tmpEEE;struct
Cyc_Absyn_RgnsEff_struct*_tmpEED;union Cyc_YYSTYPE_union _tmpEEC;yyval=(union Cyc_YYSTYPE_union)(((
_tmpEEC.YY40).tag=44,(((_tmpEEC.YY40).f1=((_tmpEEF=_cycalloc(sizeof(*_tmpEEF)),((
_tmpEEF->hd=(void*)((void*)((_tmpEED=_cycalloc(sizeof(*_tmpEED)),((_tmpEED[0]=((
_tmpEEE.tag=21,((_tmpEEE.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpEEE)))),_tmpEED))))),((
_tmpEEF->tl=0,_tmpEEF)))))),_tmpEEC))));}break;case 218: _LL2DA: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)
6,0);{struct Cyc_List_List*_tmpEF2;union Cyc_YYSTYPE_union _tmpEF1;yyval=(union Cyc_YYSTYPE_union)(((
_tmpEF1.YY40).tag=44,(((_tmpEF1.YY40).f1=((_tmpEF2=_cycalloc(sizeof(*_tmpEF2)),((
_tmpEF2->hd=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpEF2->tl=0,_tmpEF2)))))),_tmpEF1))));}break;case 219: _LL2DB:
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),(void*)5,1);{struct Cyc_List_List*_tmpEFC;struct Cyc_Absyn_AccessEff_struct
_tmpEFB;struct Cyc_Absyn_AccessEff_struct*_tmpEFA;union Cyc_YYSTYPE_union _tmpEF9;
yyval=(union Cyc_YYSTYPE_union)(((_tmpEF9.YY40).tag=44,(((_tmpEF9.YY40).f1=((
_tmpEFC=_cycalloc(sizeof(*_tmpEFC)),((_tmpEFC->hd=(void*)((void*)((_tmpEFA=
_cycalloc(sizeof(*_tmpEFA)),((_tmpEFA[0]=((_tmpEFB.tag=19,((_tmpEFB.f1=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEFB)))),
_tmpEFA))))),((_tmpEFC->tl=0,_tmpEFC)))))),_tmpEF9))));}break;case 220: _LL2DC: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
void*)5,1);{struct Cyc_List_List*_tmpF06;struct Cyc_Absyn_AccessEff_struct _tmpF05;
struct Cyc_Absyn_AccessEff_struct*_tmpF04;union Cyc_YYSTYPE_union _tmpF03;yyval=(
union Cyc_YYSTYPE_union)(((_tmpF03.YY40).tag=44,(((_tmpF03.YY40).f1=((_tmpF06=
_cycalloc(sizeof(*_tmpF06)),((_tmpF06->hd=(void*)((void*)((_tmpF04=_cycalloc(
sizeof(*_tmpF04)),((_tmpF04[0]=((_tmpF05.tag=19,((_tmpF05.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpF05)))),_tmpF04))))),((
_tmpF06->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpF06)))))),_tmpF03))));}break;case 221: _LL2DD:{struct Cyc_List_List*
_tmpF09;union Cyc_YYSTYPE_union _tmpF08;yyval=(union Cyc_YYSTYPE_union)(((_tmpF08.YY38).tag=
42,(((_tmpF08.YY38).f1=((_tmpF09=_cycalloc(sizeof(*_tmpF09)),((_tmpF09->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpF09->tl=0,
_tmpF09)))))),_tmpF08))));}break;case 222: _LL2DE:{struct Cyc_List_List*_tmpF0C;
union Cyc_YYSTYPE_union _tmpF0B;yyval=(union Cyc_YYSTYPE_union)(((_tmpF0B.YY38).tag=
42,(((_tmpF0B.YY38).f1=((_tmpF0C=_cycalloc(sizeof(*_tmpF0C)),((_tmpF0C->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpF0C->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpF0C)))))),
_tmpF0B))));}break;case 223: _LL2DF: {struct _tuple16 _tmp65F;struct Cyc_Absyn_Tqual
_tmp660;struct Cyc_List_List*_tmp661;struct Cyc_List_List*_tmp662;struct _tuple16*
_tmp65E=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp65F=*_tmp65E;_tmp660=_tmp65F.f1;_tmp661=_tmp65F.f2;_tmp662=_tmp65F.f3;if(
_tmp660.loc == 0)_tmp660.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator
_tmp664;struct _tuple1*_tmp665;struct Cyc_List_List*_tmp666;struct Cyc_Parse_Declarator*
_tmp663=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp664=*_tmp663;_tmp665=_tmp664.id;_tmp666=_tmp664.tms;{void*_tmp667=Cyc_Parse_speclist2typ(
_tmp661,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp669;void*_tmp66A;struct
Cyc_List_List*_tmp66B;struct Cyc_List_List*_tmp66C;struct _tuple6 _tmp668=Cyc_Parse_apply_tms(
_tmp660,_tmp667,_tmp662,_tmp666);_tmp669=_tmp668.f1;_tmp66A=_tmp668.f2;_tmp66B=
_tmp668.f3;_tmp66C=_tmp668.f4;if(_tmp66B != 0){const char*_tmpF0D;Cyc_Parse_err(((
_tmpF0D="parameter with bad type params",_tag_dyneither(_tmpF0D,sizeof(char),31))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}if(Cyc_Absyn_is_qvar_qualified(_tmp665)){const char*
_tmpF0E;Cyc_Parse_err(((_tmpF0E="parameter cannot be qualified with a namespace",
_tag_dyneither(_tmpF0E,sizeof(char),47))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Core_Opt*
_tmpF0F;struct Cyc_Core_Opt*_tmp66F=(struct Cyc_Core_Opt*)((_tmpF0F=_cycalloc(
sizeof(*_tmpF0F)),((_tmpF0F->v=(*_tmp665).f2,_tmpF0F))));if(_tmp66C != 0){const
char*_tmpF12;void*_tmpF11;(_tmpF11=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpF12="extra attributes on parameter, ignoring",
_tag_dyneither(_tmpF12,sizeof(char),40))),_tag_dyneither(_tmpF11,sizeof(void*),0)));}{
struct _tuple2*_tmpF15;union Cyc_YYSTYPE_union _tmpF14;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF14.YY37).tag=41,(((_tmpF14.YY37).f1=((_tmpF15=_cycalloc(sizeof(*_tmpF15)),((
_tmpF15->f1=_tmp66F,((_tmpF15->f2=_tmp669,((_tmpF15->f3=_tmp66A,_tmpF15)))))))),
_tmpF14))));}break;}}}}case 224: _LL2E0: {struct _tuple16 _tmp676;struct Cyc_Absyn_Tqual
_tmp677;struct Cyc_List_List*_tmp678;struct Cyc_List_List*_tmp679;struct _tuple16*
_tmp675=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp676=*_tmp675;_tmp677=_tmp676.f1;_tmp678=_tmp676.f2;_tmp679=_tmp676.f3;if(
_tmp677.loc == 0)_tmp677.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);{void*_tmp67A=
Cyc_Parse_speclist2typ(_tmp678,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp679 != 0){
const char*_tmpF18;void*_tmpF17;(_tmpF17=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpF18="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpF18,sizeof(char),38))),_tag_dyneither(_tmpF17,sizeof(void*),0)));}{
struct _tuple2*_tmpF1B;union Cyc_YYSTYPE_union _tmpF1A;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF1A.YY37).tag=41,(((_tmpF1A.YY37).f1=((_tmpF1B=_cycalloc(sizeof(*_tmpF1B)),((
_tmpF1B->f1=0,((_tmpF1B->f2=_tmp677,((_tmpF1B->f3=_tmp67A,_tmpF1B)))))))),
_tmpF1A))));}break;}}case 225: _LL2E1: {struct _tuple16 _tmp680;struct Cyc_Absyn_Tqual
_tmp681;struct Cyc_List_List*_tmp682;struct Cyc_List_List*_tmp683;struct _tuple16*
_tmp67F=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp680=*_tmp67F;_tmp681=_tmp680.f1;_tmp682=_tmp680.f2;_tmp683=_tmp680.f3;if(
_tmp681.loc == 0)_tmp681.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{void*_tmp684=
Cyc_Parse_speclist2typ(_tmp682,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp685=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp687;void*_tmp688;struct Cyc_List_List*_tmp689;struct Cyc_List_List*
_tmp68A;struct _tuple6 _tmp686=Cyc_Parse_apply_tms(_tmp681,_tmp684,_tmp683,_tmp685);
_tmp687=_tmp686.f1;_tmp688=_tmp686.f2;_tmp689=_tmp686.f3;_tmp68A=_tmp686.f4;if(
_tmp689 != 0){const char*_tmpF1E;void*_tmpF1D;(_tmpF1D=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpF1E="bad type parameters on formal argument, ignoring",
_tag_dyneither(_tmpF1E,sizeof(char),49))),_tag_dyneither(_tmpF1D,sizeof(void*),0)));}
if(_tmp68A != 0){const char*_tmpF21;void*_tmpF20;(_tmpF20=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpF21="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpF21,sizeof(char),38))),_tag_dyneither(_tmpF20,sizeof(void*),0)));}{
struct _tuple2*_tmpF24;union Cyc_YYSTYPE_union _tmpF23;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF23.YY37).tag=41,(((_tmpF23.YY37).f1=((_tmpF24=_cycalloc(sizeof(*_tmpF24)),((
_tmpF24->f1=0,((_tmpF24->f2=_tmp687,((_tmpF24->f3=_tmp688,_tmpF24)))))))),
_tmpF23))));}break;}}case 226: _LL2E2:{union Cyc_YYSTYPE_union _tmpF25;yyval=(union
Cyc_YYSTYPE_union)(((_tmpF25.YY36).tag=40,(((_tmpF25.YY36).f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpF25))));}break;case 227:
_LL2E3:{struct Cyc_List_List*_tmpF2B;struct _dyneither_ptr*_tmpF2A;union Cyc_YYSTYPE_union
_tmpF29;yyval=(union Cyc_YYSTYPE_union)(((_tmpF29.YY36).tag=40,(((_tmpF29.YY36).f1=((
_tmpF2B=_cycalloc(sizeof(*_tmpF2B)),((_tmpF2B->hd=((_tmpF2A=_cycalloc(sizeof(*
_tmpF2A)),((_tmpF2A[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),_tmpF2A)))),((_tmpF2B->tl=0,_tmpF2B)))))),_tmpF29))));}
break;case 228: _LL2E4:{struct Cyc_List_List*_tmpF31;struct _dyneither_ptr*_tmpF30;
union Cyc_YYSTYPE_union _tmpF2F;yyval=(union Cyc_YYSTYPE_union)(((_tmpF2F.YY36).tag=
40,(((_tmpF2F.YY36).f1=((_tmpF31=_cycalloc(sizeof(*_tmpF31)),((_tmpF31->hd=((
_tmpF30=_cycalloc(sizeof(*_tmpF30)),((_tmpF30[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF30)))),((_tmpF31->tl=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpF31)))))),_tmpF2F))));}break;case 229: _LL2E5: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 230: _LL2E6: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 231: _LL2E7:{
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpF37;struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpF36;union Cyc_YYSTYPE_union _tmpF35;yyval=(union Cyc_YYSTYPE_union)(((_tmpF35.YY3).tag=
7,(((_tmpF35.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmpF37=_cycalloc(sizeof(*
_tmpF37)),((_tmpF37[0]=((_tmpF36.tag=37,((_tmpF36.f1=0,((_tmpF36.f2=0,_tmpF36)))))),
_tmpF37)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpF35))));}break;case 232: _LL2E8:{struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpF3D;struct Cyc_Absyn_UnresolvedMem_e_struct _tmpF3C;union Cyc_YYSTYPE_union
_tmpF3B;yyval=(union Cyc_YYSTYPE_union)(((_tmpF3B.YY3).tag=7,(((_tmpF3B.YY3).f1=
Cyc_Absyn_new_exp((void*)((_tmpF3D=_cycalloc(sizeof(*_tmpF3D)),((_tmpF3D[0]=((
_tmpF3C.tag=37,((_tmpF3C.f1=0,((_tmpF3C.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])),_tmpF3C)))))),_tmpF3D)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpF3B))));}
break;case 233: _LL2E9:{struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpF43;struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpF42;union Cyc_YYSTYPE_union _tmpF41;yyval=(union Cyc_YYSTYPE_union)(((_tmpF41.YY3).tag=
7,(((_tmpF41.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmpF43=_cycalloc(sizeof(*
_tmpF43)),((_tmpF43[0]=((_tmpF42.tag=37,((_tmpF42.f1=0,((_tmpF42.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),_tmpF42)))))),_tmpF43)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpF41))));}break;case 234: _LL2EA: {union Cyc_Absyn_Nmspace_union
_tmpF48;struct _dyneither_ptr*_tmpF47;struct _tuple1*_tmpF46;struct Cyc_Absyn_Vardecl*
_tmp6A1=Cyc_Absyn_new_vardecl(((_tmpF46=_cycalloc(sizeof(*_tmpF46)),((_tmpF46->f1=(
union Cyc_Absyn_Nmspace_union)(((_tmpF48.Loc_n).tag=0,_tmpF48)),((_tmpF46->f2=((
_tmpF47=_cycalloc(sizeof(*_tmpF47)),((_tmpF47[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpF47)))),_tmpF46)))))),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp6A1->tq).real_const=
1;{struct Cyc_Absyn_Comprehension_e_struct*_tmpF4E;struct Cyc_Absyn_Comprehension_e_struct
_tmpF4D;union Cyc_YYSTYPE_union _tmpF4C;yyval=(union Cyc_YYSTYPE_union)(((_tmpF4C.YY3).tag=
7,(((_tmpF4C.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmpF4E=_cycalloc(sizeof(*
_tmpF4E)),((_tmpF4E[0]=((_tmpF4D.tag=29,((_tmpF4D.f1=_tmp6A1,((_tmpF4D.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpF4D.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpF4D.f4=0,
_tmpF4D)))))))))),_tmpF4E)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpF4C))));}
break;}case 235: _LL2EB:{struct Cyc_List_List*_tmpF54;struct _tuple20*_tmpF53;union
Cyc_YYSTYPE_union _tmpF52;yyval=(union Cyc_YYSTYPE_union)(((_tmpF52.YY6).tag=10,(((
_tmpF52.YY6).f1=((_tmpF54=_cycalloc(sizeof(*_tmpF54)),((_tmpF54->hd=((_tmpF53=
_cycalloc(sizeof(*_tmpF53)),((_tmpF53->f1=0,((_tmpF53->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF53)))))),((_tmpF54->tl=
0,_tmpF54)))))),_tmpF52))));}break;case 236: _LL2EC:{struct Cyc_List_List*_tmpF5A;
struct _tuple20*_tmpF59;union Cyc_YYSTYPE_union _tmpF58;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF58.YY6).tag=10,(((_tmpF58.YY6).f1=((_tmpF5A=_cycalloc(sizeof(*_tmpF5A)),((
_tmpF5A->hd=((_tmpF59=_cycalloc(sizeof(*_tmpF59)),((_tmpF59->f1=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpF59->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF59)))))),((
_tmpF5A->tl=0,_tmpF5A)))))),_tmpF58))));}break;case 237: _LL2ED:{struct Cyc_List_List*
_tmpF60;struct _tuple20*_tmpF5F;union Cyc_YYSTYPE_union _tmpF5E;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF5E.YY6).tag=10,(((_tmpF5E.YY6).f1=((_tmpF60=_cycalloc(sizeof(*_tmpF60)),((
_tmpF60->hd=((_tmpF5F=_cycalloc(sizeof(*_tmpF5F)),((_tmpF5F->f1=0,((_tmpF5F->f2=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF5F)))))),((
_tmpF60->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]),_tmpF60)))))),_tmpF5E))));}break;case 238: _LL2EE:{struct Cyc_List_List*
_tmpF66;struct _tuple20*_tmpF65;union Cyc_YYSTYPE_union _tmpF64;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF64.YY6).tag=10,(((_tmpF64.YY6).f1=((_tmpF66=_cycalloc(sizeof(*_tmpF66)),((
_tmpF66->hd=((_tmpF65=_cycalloc(sizeof(*_tmpF65)),((_tmpF65->f1=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpF65->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF65)))))),((
_tmpF66->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]),_tmpF66)))))),_tmpF64))));}break;case 239: _LL2EF:{union Cyc_YYSTYPE_union
_tmpF67;yyval=(union Cyc_YYSTYPE_union)(((_tmpF67.YY41).tag=45,(((_tmpF67.YY41).f1=
Cyc_List_imp_rev(Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])),_tmpF67))));}break;case 240: _LL2F0:{struct Cyc_List_List*
_tmpF6A;union Cyc_YYSTYPE_union _tmpF69;yyval=(union Cyc_YYSTYPE_union)(((_tmpF69.YY41).tag=
45,(((_tmpF69.YY41).f1=((_tmpF6A=_cycalloc(sizeof(*_tmpF6A)),((_tmpF6A->hd=(void*)
Cyc_yyget_YY42(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpF6A->tl=0,_tmpF6A)))))),_tmpF69))));}break;case 241: _LL2F1:{struct Cyc_List_List*
_tmpF6D;union Cyc_YYSTYPE_union _tmpF6C;yyval=(union Cyc_YYSTYPE_union)(((_tmpF6C.YY41).tag=
45,(((_tmpF6C.YY41).f1=((_tmpF6D=_cycalloc(sizeof(*_tmpF6D)),((_tmpF6D->hd=(void*)
Cyc_yyget_YY42(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpF6D->tl=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpF6D)))))),_tmpF6C))));}break;case 242: _LL2F2:{struct Cyc_Absyn_ArrayElement_struct*
_tmpF73;struct Cyc_Absyn_ArrayElement_struct _tmpF72;union Cyc_YYSTYPE_union _tmpF71;
yyval=(union Cyc_YYSTYPE_union)(((_tmpF71.YY42).tag=46,(((_tmpF71.YY42).f1=(void*)((
void*)((_tmpF73=_cycalloc(sizeof(*_tmpF73)),((_tmpF73[0]=((_tmpF72.tag=0,((
_tmpF72.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]),_tmpF72)))),_tmpF73))))),_tmpF71))));}break;case 243: _LL2F3:{struct Cyc_Absyn_FieldName_struct*
_tmpF7D;struct _dyneither_ptr*_tmpF7C;struct Cyc_Absyn_FieldName_struct _tmpF7B;
union Cyc_YYSTYPE_union _tmpF7A;yyval=(union Cyc_YYSTYPE_union)(((_tmpF7A.YY42).tag=
46,(((_tmpF7A.YY42).f1=(void*)((void*)((_tmpF7D=_cycalloc(sizeof(*_tmpF7D)),((
_tmpF7D[0]=((_tmpF7B.tag=1,((_tmpF7B.f1=((_tmpF7C=_cycalloc(sizeof(*_tmpF7C)),((
_tmpF7C[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpF7C)))),_tmpF7B)))),_tmpF7D))))),_tmpF7A))));}break;case 244:
_LL2F4: {struct _tuple16 _tmp6C1;struct Cyc_Absyn_Tqual _tmp6C2;struct Cyc_List_List*
_tmp6C3;struct Cyc_List_List*_tmp6C4;struct _tuple16*_tmp6C0=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6C1=*_tmp6C0;_tmp6C2=
_tmp6C1.f1;_tmp6C3=_tmp6C1.f2;_tmp6C4=_tmp6C1.f3;{void*_tmp6C5=Cyc_Parse_speclist2typ(
_tmp6C3,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp6C4 != 0){const char*_tmpF80;void*_tmpF7F;(
_tmpF7F=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpF80="ignoring attributes in type",
_tag_dyneither(_tmpF80,sizeof(char),28))),_tag_dyneither(_tmpF7F,sizeof(void*),0)));}{
struct _tuple2*_tmpF83;union Cyc_YYSTYPE_union _tmpF82;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF82.YY37).tag=41,(((_tmpF82.YY37).f1=((_tmpF83=_cycalloc(sizeof(*_tmpF83)),((
_tmpF83->f1=0,((_tmpF83->f2=_tmp6C2,((_tmpF83->f3=_tmp6C5,_tmpF83)))))))),
_tmpF82))));}break;}}case 245: _LL2F5: {struct _tuple16 _tmp6CB;struct Cyc_Absyn_Tqual
_tmp6CC;struct Cyc_List_List*_tmp6CD;struct Cyc_List_List*_tmp6CE;struct _tuple16*
_tmp6CA=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp6CB=*_tmp6CA;_tmp6CC=_tmp6CB.f1;_tmp6CD=_tmp6CB.f2;_tmp6CE=_tmp6CB.f3;{void*
_tmp6CF=Cyc_Parse_speclist2typ(_tmp6CD,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6D0=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct _tuple6 _tmp6D1=Cyc_Parse_apply_tms(_tmp6CC,_tmp6CF,_tmp6CE,_tmp6D0);if(
_tmp6D1.f3 != 0){const char*_tmpF86;void*_tmpF85;(_tmpF85=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpF86="bad type params, ignoring",
_tag_dyneither(_tmpF86,sizeof(char),26))),_tag_dyneither(_tmpF85,sizeof(void*),0)));}
if(_tmp6D1.f4 != 0){const char*_tmpF89;void*_tmpF88;(_tmpF88=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpF89="bad specifiers, ignoring",
_tag_dyneither(_tmpF89,sizeof(char),25))),_tag_dyneither(_tmpF88,sizeof(void*),0)));}{
struct _tuple2*_tmpF8C;union Cyc_YYSTYPE_union _tmpF8B;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF8B.YY37).tag=41,(((_tmpF8B.YY37).f1=((_tmpF8C=_cycalloc(sizeof(*_tmpF8C)),((
_tmpF8C->f1=0,((_tmpF8C->f2=_tmp6D1.f1,((_tmpF8C->f3=_tmp6D1.f2,_tmpF8C)))))))),
_tmpF8B))));}break;}}case 246: _LL2F6:{union Cyc_YYSTYPE_union _tmpF8D;yyval=(union
Cyc_YYSTYPE_union)(((_tmpF8D.YY44).tag=48,(((_tmpF8D.YY44).f1=(void*)(*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,_tmpF8D))));}
break;case 247: _LL2F7:{struct Cyc_Absyn_JoinEff_struct*_tmpF93;struct Cyc_Absyn_JoinEff_struct
_tmpF92;union Cyc_YYSTYPE_union _tmpF91;yyval=(union Cyc_YYSTYPE_union)(((_tmpF91.YY44).tag=
48,(((_tmpF91.YY44).f1=(void*)((void*)((_tmpF93=_cycalloc(sizeof(*_tmpF93)),((
_tmpF93[0]=((_tmpF92.tag=20,((_tmpF92.f1=0,_tmpF92)))),_tmpF93))))),_tmpF91))));}
break;case 248: _LL2F8:{struct Cyc_Absyn_JoinEff_struct*_tmpF99;struct Cyc_Absyn_JoinEff_struct
_tmpF98;union Cyc_YYSTYPE_union _tmpF97;yyval=(union Cyc_YYSTYPE_union)(((_tmpF97.YY44).tag=
48,(((_tmpF97.YY44).f1=(void*)((void*)((_tmpF99=_cycalloc(sizeof(*_tmpF99)),((
_tmpF99[0]=((_tmpF98.tag=20,((_tmpF98.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpF98)))),_tmpF99))))),
_tmpF97))));}break;case 249: _LL2F9:{struct Cyc_Absyn_RgnsEff_struct*_tmpF9F;struct
Cyc_Absyn_RgnsEff_struct _tmpF9E;union Cyc_YYSTYPE_union _tmpF9D;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF9D.YY44).tag=48,(((_tmpF9D.YY44).f1=(void*)((void*)((_tmpF9F=_cycalloc(
sizeof(*_tmpF9F)),((_tmpF9F[0]=((_tmpF9E.tag=21,((_tmpF9E.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpF9E)))),_tmpF9F))))),
_tmpF9D))));}break;case 250: _LL2FA:{struct Cyc_Absyn_JoinEff_struct*_tmpFA9;struct
Cyc_List_List*_tmpFA8;struct Cyc_Absyn_JoinEff_struct _tmpFA7;union Cyc_YYSTYPE_union
_tmpFA6;yyval=(union Cyc_YYSTYPE_union)(((_tmpFA6.YY44).tag=48,(((_tmpFA6.YY44).f1=(
void*)((void*)((_tmpFA9=_cycalloc(sizeof(*_tmpFA9)),((_tmpFA9[0]=((_tmpFA7.tag=
20,((_tmpFA7.f1=((_tmpFA8=_cycalloc(sizeof(*_tmpFA8)),((_tmpFA8->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpFA8->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpFA8)))))),_tmpFA7)))),
_tmpFA9))))),_tmpFA6))));}break;case 251: _LL2FB:{struct Cyc_List_List*_tmpFAC;
union Cyc_YYSTYPE_union _tmpFAB;yyval=(union Cyc_YYSTYPE_union)(((_tmpFAB.YY40).tag=
44,(((_tmpFAB.YY40).f1=((_tmpFAC=_cycalloc(sizeof(*_tmpFAC)),((_tmpFAC->hd=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpFAC->tl=0,_tmpFAC)))))),_tmpFAB))));}break;case 252: _LL2FC:{struct Cyc_List_List*
_tmpFAF;union Cyc_YYSTYPE_union _tmpFAE;yyval=(union Cyc_YYSTYPE_union)(((_tmpFAE.YY40).tag=
44,(((_tmpFAE.YY40).f1=((_tmpFAF=_cycalloc(sizeof(*_tmpFAF)),((_tmpFAF->hd=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpFAF->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),_tmpFAF)))))),_tmpFAE))));}break;case 253: _LL2FD:{struct Cyc_Parse_Abstractdeclarator*
_tmpFB2;union Cyc_YYSTYPE_union _tmpFB1;yyval=(union Cyc_YYSTYPE_union)(((_tmpFB1.YY30).tag=
34,(((_tmpFB1.YY30).f1=((_tmpFB2=_cycalloc(sizeof(*_tmpFB2)),((_tmpFB2->tms=Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpFB2)))),_tmpFB1))));}
break;case 254: _LL2FE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 255: _LL2FF:{struct Cyc_Parse_Abstractdeclarator*_tmpFB5;
union Cyc_YYSTYPE_union _tmpFB4;yyval=(union Cyc_YYSTYPE_union)(((_tmpFB4.YY30).tag=
34,(((_tmpFB4.YY30).f1=((_tmpFB5=_cycalloc(sizeof(*_tmpFB5)),((_tmpFB5->tms=Cyc_List_imp_append(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpFB5)))),
_tmpFB4))));}break;case 256: _LL300: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 257: _LL301:{struct Cyc_Parse_Abstractdeclarator*
_tmpFC4;struct Cyc_Absyn_Carray_mod_struct*_tmpFC3;struct Cyc_Absyn_Carray_mod_struct
_tmpFC2;struct Cyc_List_List*_tmpFC1;union Cyc_YYSTYPE_union _tmpFC0;yyval=(union
Cyc_YYSTYPE_union)(((_tmpFC0.YY30).tag=34,(((_tmpFC0.YY30).f1=((_tmpFC4=
_cycalloc(sizeof(*_tmpFC4)),((_tmpFC4->tms=((_tmpFC1=_cycalloc(sizeof(*_tmpFC1)),((
_tmpFC1->hd=(void*)((void*)((_tmpFC3=_cycalloc(sizeof(*_tmpFC3)),((_tmpFC3[0]=((
_tmpFC2.tag=0,((_tmpFC2.f1=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpFC2.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpFC2)))))),
_tmpFC3))))),((_tmpFC1->tl=0,_tmpFC1)))))),_tmpFC4)))),_tmpFC0))));}break;case
258: _LL302:{struct Cyc_Parse_Abstractdeclarator*_tmpFD3;struct Cyc_Absyn_Carray_mod_struct*
_tmpFD2;struct Cyc_Absyn_Carray_mod_struct _tmpFD1;struct Cyc_List_List*_tmpFD0;
union Cyc_YYSTYPE_union _tmpFCF;yyval=(union Cyc_YYSTYPE_union)(((_tmpFCF.YY30).tag=
34,(((_tmpFCF.YY30).f1=((_tmpFD3=_cycalloc(sizeof(*_tmpFD3)),((_tmpFD3->tms=((
_tmpFD0=_cycalloc(sizeof(*_tmpFD0)),((_tmpFD0->hd=(void*)((void*)((_tmpFD2=
_cycalloc(sizeof(*_tmpFD2)),((_tmpFD2[0]=((_tmpFD1.tag=0,((_tmpFD1.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpFD1.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpFD1)))))),
_tmpFD2))))),((_tmpFD0->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpFD0)))))),_tmpFD3)))),_tmpFCF))));}break;case
259: _LL303:{struct Cyc_Parse_Abstractdeclarator*_tmpFE2;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpFE1;struct Cyc_Absyn_ConstArray_mod_struct _tmpFE0;struct Cyc_List_List*_tmpFDF;
union Cyc_YYSTYPE_union _tmpFDE;yyval=(union Cyc_YYSTYPE_union)(((_tmpFDE.YY30).tag=
34,(((_tmpFDE.YY30).f1=((_tmpFE2=_cycalloc(sizeof(*_tmpFE2)),((_tmpFE2->tms=((
_tmpFDF=_cycalloc(sizeof(*_tmpFDF)),((_tmpFDF->hd=(void*)((void*)((_tmpFE1=
_cycalloc(sizeof(*_tmpFE1)),((_tmpFE1[0]=((_tmpFE0.tag=1,((_tmpFE0.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpFE0.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpFE0.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpFE0)))))))),
_tmpFE1))))),((_tmpFDF->tl=0,_tmpFDF)))))),_tmpFE2)))),_tmpFDE))));}break;case
260: _LL304:{struct Cyc_Parse_Abstractdeclarator*_tmpFF1;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpFF0;struct Cyc_Absyn_ConstArray_mod_struct _tmpFEF;struct Cyc_List_List*_tmpFEE;
union Cyc_YYSTYPE_union _tmpFED;yyval=(union Cyc_YYSTYPE_union)(((_tmpFED.YY30).tag=
34,(((_tmpFED.YY30).f1=((_tmpFF1=_cycalloc(sizeof(*_tmpFF1)),((_tmpFF1->tms=((
_tmpFEE=_cycalloc(sizeof(*_tmpFEE)),((_tmpFEE->hd=(void*)((void*)((_tmpFF0=
_cycalloc(sizeof(*_tmpFF0)),((_tmpFF0[0]=((_tmpFEF.tag=1,((_tmpFEF.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpFEF.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpFEF.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpFEF)))))))),
_tmpFF0))))),((_tmpFEE->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpFEE)))))),_tmpFF1)))),_tmpFED))));}break;case
261: _LL305:{struct Cyc_Parse_Abstractdeclarator*_tmp100D;struct Cyc_Absyn_Function_mod_struct*
_tmp100C;struct Cyc_Absyn_WithTypes_struct*_tmp100B;struct Cyc_Absyn_WithTypes_struct
_tmp100A;struct Cyc_Absyn_Function_mod_struct _tmp1009;struct Cyc_List_List*
_tmp1008;union Cyc_YYSTYPE_union _tmp1007;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1007.YY30).tag=34,(((_tmp1007.YY30).f1=((_tmp100D=_cycalloc(sizeof(*_tmp100D)),((
_tmp100D->tms=((_tmp1008=_cycalloc(sizeof(*_tmp1008)),((_tmp1008->hd=(void*)((
void*)((_tmp100C=_cycalloc(sizeof(*_tmp100C)),((_tmp100C[0]=((_tmp1009.tag=3,((
_tmp1009.f1=(void*)((void*)((_tmp100B=_cycalloc(sizeof(*_tmp100B)),((_tmp100B[0]=((
_tmp100A.tag=1,((_tmp100A.f1=0,((_tmp100A.f2=0,((_tmp100A.f3=0,((_tmp100A.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp100A.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmp100A)))))))))))),
_tmp100B))))),_tmp1009)))),_tmp100C))))),((_tmp1008->tl=0,_tmp1008)))))),
_tmp100D)))),_tmp1007))));}break;case 262: _LL306: {struct _tuple17 _tmp70A;struct
Cyc_List_List*_tmp70B;int _tmp70C;struct Cyc_Absyn_VarargInfo*_tmp70D;struct Cyc_Core_Opt*
_tmp70E;struct Cyc_List_List*_tmp70F;struct _tuple17*_tmp709=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp70A=*_tmp709;_tmp70B=
_tmp70A.f1;_tmp70C=_tmp70A.f2;_tmp70D=_tmp70A.f3;_tmp70E=_tmp70A.f4;_tmp70F=
_tmp70A.f5;{struct Cyc_Parse_Abstractdeclarator*_tmp1029;struct Cyc_Absyn_Function_mod_struct*
_tmp1028;struct Cyc_Absyn_WithTypes_struct*_tmp1027;struct Cyc_Absyn_WithTypes_struct
_tmp1026;struct Cyc_Absyn_Function_mod_struct _tmp1025;struct Cyc_List_List*
_tmp1024;union Cyc_YYSTYPE_union _tmp1023;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1023.YY30).tag=34,(((_tmp1023.YY30).f1=((_tmp1029=_cycalloc(sizeof(*_tmp1029)),((
_tmp1029->tms=((_tmp1024=_cycalloc(sizeof(*_tmp1024)),((_tmp1024->hd=(void*)((
void*)((_tmp1028=_cycalloc(sizeof(*_tmp1028)),((_tmp1028[0]=((_tmp1025.tag=3,((
_tmp1025.f1=(void*)((void*)((_tmp1027=_cycalloc(sizeof(*_tmp1027)),((_tmp1027[0]=((
_tmp1026.tag=1,((_tmp1026.f1=_tmp70B,((_tmp1026.f2=_tmp70C,((_tmp1026.f3=_tmp70D,((
_tmp1026.f4=_tmp70E,((_tmp1026.f5=_tmp70F,_tmp1026)))))))))))),_tmp1027))))),
_tmp1025)))),_tmp1028))))),((_tmp1024->tl=0,_tmp1024)))))),_tmp1029)))),_tmp1023))));}
break;}case 263: _LL307:{struct Cyc_Parse_Abstractdeclarator*_tmp1045;struct Cyc_Absyn_Function_mod_struct*
_tmp1044;struct Cyc_Absyn_WithTypes_struct*_tmp1043;struct Cyc_Absyn_WithTypes_struct
_tmp1042;struct Cyc_Absyn_Function_mod_struct _tmp1041;struct Cyc_List_List*
_tmp1040;union Cyc_YYSTYPE_union _tmp103F;yyval=(union Cyc_YYSTYPE_union)(((
_tmp103F.YY30).tag=34,(((_tmp103F.YY30).f1=((_tmp1045=_cycalloc(sizeof(*_tmp1045)),((
_tmp1045->tms=((_tmp1040=_cycalloc(sizeof(*_tmp1040)),((_tmp1040->hd=(void*)((
void*)((_tmp1044=_cycalloc(sizeof(*_tmp1044)),((_tmp1044[0]=((_tmp1041.tag=3,((
_tmp1041.f1=(void*)((void*)((_tmp1043=_cycalloc(sizeof(*_tmp1043)),((_tmp1043[0]=((
_tmp1042.tag=1,((_tmp1042.f1=0,((_tmp1042.f2=0,((_tmp1042.f3=0,((_tmp1042.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp1042.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmp1042)))))))))))),
_tmp1043))))),_tmp1041)))),_tmp1044))))),((_tmp1040->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmp1040)))))),
_tmp1045)))),_tmp103F))));}break;case 264: _LL308: {struct _tuple17 _tmp71F;struct
Cyc_List_List*_tmp720;int _tmp721;struct Cyc_Absyn_VarargInfo*_tmp722;struct Cyc_Core_Opt*
_tmp723;struct Cyc_List_List*_tmp724;struct _tuple17*_tmp71E=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp71F=*_tmp71E;_tmp720=
_tmp71F.f1;_tmp721=_tmp71F.f2;_tmp722=_tmp71F.f3;_tmp723=_tmp71F.f4;_tmp724=
_tmp71F.f5;{struct Cyc_Parse_Abstractdeclarator*_tmp1061;struct Cyc_Absyn_Function_mod_struct*
_tmp1060;struct Cyc_Absyn_WithTypes_struct*_tmp105F;struct Cyc_Absyn_WithTypes_struct
_tmp105E;struct Cyc_Absyn_Function_mod_struct _tmp105D;struct Cyc_List_List*
_tmp105C;union Cyc_YYSTYPE_union _tmp105B;yyval=(union Cyc_YYSTYPE_union)(((
_tmp105B.YY30).tag=34,(((_tmp105B.YY30).f1=((_tmp1061=_cycalloc(sizeof(*_tmp1061)),((
_tmp1061->tms=((_tmp105C=_cycalloc(sizeof(*_tmp105C)),((_tmp105C->hd=(void*)((
void*)((_tmp1060=_cycalloc(sizeof(*_tmp1060)),((_tmp1060[0]=((_tmp105D.tag=3,((
_tmp105D.f1=(void*)((void*)((_tmp105F=_cycalloc(sizeof(*_tmp105F)),((_tmp105F[0]=((
_tmp105E.tag=1,((_tmp105E.f1=_tmp720,((_tmp105E.f2=_tmp721,((_tmp105E.f3=_tmp722,((
_tmp105E.f4=_tmp723,((_tmp105E.f5=_tmp724,_tmp105E)))))))))))),_tmp105F))))),
_tmp105D)))),_tmp1060))))),((_tmp105C->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmp105C)))))),
_tmp1061)))),_tmp105B))));}break;}case 265: _LL309: {struct Cyc_List_List*_tmp72C=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Parse_Abstractdeclarator*
_tmp1070;struct Cyc_Absyn_TypeParams_mod_struct*_tmp106F;struct Cyc_Absyn_TypeParams_mod_struct
_tmp106E;struct Cyc_List_List*_tmp106D;union Cyc_YYSTYPE_union _tmp106C;yyval=(
union Cyc_YYSTYPE_union)(((_tmp106C.YY30).tag=34,(((_tmp106C.YY30).f1=((_tmp1070=
_cycalloc(sizeof(*_tmp1070)),((_tmp1070->tms=((_tmp106D=_cycalloc(sizeof(*
_tmp106D)),((_tmp106D->hd=(void*)((void*)((_tmp106F=_cycalloc(sizeof(*_tmp106F)),((
_tmp106F[0]=((_tmp106E.tag=4,((_tmp106E.f1=_tmp72C,((_tmp106E.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmp106E.f3=0,
_tmp106E)))))))),_tmp106F))))),((_tmp106D->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmp106D)))))),
_tmp1070)))),_tmp106C))));}break;}case 266: _LL30A:{struct Cyc_Parse_Abstractdeclarator*
_tmp107F;struct Cyc_Absyn_Attributes_mod_struct*_tmp107E;struct Cyc_Absyn_Attributes_mod_struct
_tmp107D;struct Cyc_List_List*_tmp107C;union Cyc_YYSTYPE_union _tmp107B;yyval=(
union Cyc_YYSTYPE_union)(((_tmp107B.YY30).tag=34,(((_tmp107B.YY30).f1=((_tmp107F=
_cycalloc(sizeof(*_tmp107F)),((_tmp107F->tms=((_tmp107C=_cycalloc(sizeof(*
_tmp107C)),((_tmp107C->hd=(void*)((void*)((_tmp107E=_cycalloc(sizeof(*_tmp107E)),((
_tmp107E[0]=((_tmp107D.tag=5,((_tmp107D.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmp107D.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmp107D)))))),_tmp107E))))),((_tmp107C->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms,_tmp107C)))))),
_tmp107F)))),_tmp107B))));}break;case 267: _LL30B: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 268: _LL30C: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 269: _LL30D:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 270:
_LL30E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
271: _LL30F: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 272: _LL310: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 273: _LL311:{struct Cyc_Absyn_ResetRegion_s_struct*_tmp1085;struct Cyc_Absyn_ResetRegion_s_struct
_tmp1084;union Cyc_YYSTYPE_union _tmp1083;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1083.YY9).tag=13,(((_tmp1083.YY9).f1=Cyc_Absyn_new_stmt((void*)((_tmp1085=
_cycalloc(sizeof(*_tmp1085)),((_tmp1085[0]=((_tmp1084.tag=15,((_tmp1084.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp1084)))),
_tmp1085)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1083))));}break;case 274: _LL312:{union Cyc_YYSTYPE_union
_tmp1086;yyval=(union Cyc_YYSTYPE_union)(((_tmp1086.YY4).tag=8,(((_tmp1086.YY4).f1=
0,_tmp1086))));}break;case 275: _LL313:{const char*_tmp1087;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmp1087="open",_tag_dyneither(_tmp1087,sizeof(char),5))))
!= 0){const char*_tmp1088;Cyc_Parse_err(((_tmp1088="expecting `open'",
_tag_dyneither(_tmp1088,sizeof(char),17))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}{union Cyc_YYSTYPE_union
_tmp1089;yyval=(union Cyc_YYSTYPE_union)(((_tmp1089.YY4).tag=8,(((_tmp1089.YY4).f1=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmp1089))));}break;case 276: _LL314:{struct Cyc_Absyn_Label_s_struct*
_tmp1093;struct _dyneither_ptr*_tmp1092;struct Cyc_Absyn_Label_s_struct _tmp1091;
union Cyc_YYSTYPE_union _tmp1090;yyval=(union Cyc_YYSTYPE_union)(((_tmp1090.YY9).tag=
13,(((_tmp1090.YY9).f1=Cyc_Absyn_new_stmt((void*)((_tmp1093=_cycalloc(sizeof(*
_tmp1093)),((_tmp1093[0]=((_tmp1091.tag=12,((_tmp1091.f1=((_tmp1092=_cycalloc(
sizeof(*_tmp1092)),((_tmp1092[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp1092)))),((_tmp1091.f2=
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1091)))))),
_tmp1093)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1090))));}break;case 277: _LL315:{union Cyc_YYSTYPE_union
_tmp1094;yyval=(union Cyc_YYSTYPE_union)(((_tmp1094.YY9).tag=13,(((_tmp1094.YY9).f1=
Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1094))));}
break;case 278: _LL316:{union Cyc_YYSTYPE_union _tmp1095;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1095.YY9).tag=13,(((_tmp1095.YY9).f1=Cyc_Absyn_exp_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1095))));}
break;case 279: _LL317:{union Cyc_YYSTYPE_union _tmp1096;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1096.YY9).tag=13,(((_tmp1096.YY9).f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1096))));}
break;case 280: _LL318: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 281: _LL319:{union Cyc_YYSTYPE_union _tmp1097;yyval=(
union Cyc_YYSTYPE_union)(((_tmp1097.YY9).tag=13,(((_tmp1097.YY9).f1=Cyc_Parse_flatten_declarations(
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(
0)),_tmp1097))));}break;case 282: _LL31A:{union Cyc_YYSTYPE_union _tmp1098;yyval=(
union Cyc_YYSTYPE_union)(((_tmp1098.YY9).tag=13,(((_tmp1098.YY9).f1=Cyc_Parse_flatten_declarations(
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmp1098))));}break;
case 283: _LL31B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 284: _LL31C:{union Cyc_YYSTYPE_union _tmp1099;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1099.YY9).tag=13,(((_tmp1099.YY9).f1=Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1099))));}
break;case 285: _LL31D:{struct Cyc_Absyn_Fn_d_struct*_tmp109F;struct Cyc_Absyn_Fn_d_struct
_tmp109E;union Cyc_YYSTYPE_union _tmp109D;yyval=(union Cyc_YYSTYPE_union)(((
_tmp109D.YY9).tag=13,(((_tmp109D.YY9).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)((_tmp109F=_cycalloc(sizeof(*_tmp109F)),((_tmp109F[0]=((_tmp109E.tag=1,((
_tmp109E.f1=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp109E)))),_tmp109F)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(
0)),_tmp109D))));}break;case 286: _LL31E:{struct Cyc_Absyn_Fn_d_struct*_tmp10A5;
struct Cyc_Absyn_Fn_d_struct _tmp10A4;union Cyc_YYSTYPE_union _tmp10A3;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10A3.YY9).tag=13,(((_tmp10A3.YY9).f1=Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl((void*)((_tmp10A5=_cycalloc(sizeof(*_tmp10A5)),((_tmp10A5[0]=((
_tmp10A4.tag=1,((_tmp10A4.f1=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmp10A4)))),_tmp10A5)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmp10A3))));}break;
case 287: _LL31F:{union Cyc_YYSTYPE_union _tmp10A6;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10A6.YY9).tag=13,(((_tmp10A6.YY9).f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10A6))));}
break;case 288: _LL320:{union Cyc_YYSTYPE_union _tmp10A7;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10A7.YY9).tag=13,(((_tmp10A7.YY9).f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10A7))));}
break;case 289: _LL321:{union Cyc_YYSTYPE_union _tmp10A8;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10A8.YY9).tag=13,(((_tmp10A8.YY9).f1=Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10A8))));}
break;case 290: _LL322: {struct Cyc_Absyn_UnknownId_e_struct _tmp10AB;struct Cyc_Absyn_UnknownId_e_struct*
_tmp10AA;struct Cyc_Absyn_Exp*_tmp751=Cyc_Absyn_new_exp((void*)((_tmp10AA=
_cycalloc(sizeof(*_tmp10AA)),((_tmp10AA[0]=((_tmp10AB.tag=2,((_tmp10AB.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp10AB)))),
_tmp10AA)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));{union Cyc_YYSTYPE_union _tmp10AC;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10AC.YY9).tag=13,(((_tmp10AC.YY9).f1=Cyc_Absyn_switch_stmt(_tmp751,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10AC))));}
break;}case 291: _LL323: {struct Cyc_Absyn_Exp*_tmp755=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{union Cyc_YYSTYPE_union
_tmp10AD;yyval=(union Cyc_YYSTYPE_union)(((_tmp10AD.YY9).tag=13,(((_tmp10AD.YY9).f1=
Cyc_Absyn_switch_stmt(_tmp755,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10AD))));}
break;}case 292: _LL324:{union Cyc_YYSTYPE_union _tmp10AE;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10AE.YY9).tag=13,(((_tmp10AE.YY9).f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10AE))));}
break;case 293: _LL325:{union Cyc_YYSTYPE_union _tmp10AF;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10AF.YY10).tag=14,(((_tmp10AF.YY10).f1=0,_tmp10AF))));}break;case 294: _LL326:{
struct Cyc_List_List*_tmp10B5;struct Cyc_Absyn_Switch_clause*_tmp10B4;union Cyc_YYSTYPE_union
_tmp10B3;yyval=(union Cyc_YYSTYPE_union)(((_tmp10B3.YY10).tag=14,(((_tmp10B3.YY10).f1=((
_tmp10B5=_cycalloc(sizeof(*_tmp10B5)),((_tmp10B5->hd=((_tmp10B4=_cycalloc(
sizeof(*_tmp10B4)),((_tmp10B4->pattern=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)),((_tmp10B4->pat_vars=
0,((_tmp10B4->where_clause=0,((_tmp10B4->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp10B4->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmp10B4)))))))))))),((
_tmp10B5->tl=0,_tmp10B5)))))),_tmp10B3))));}break;case 295: _LL327:{struct Cyc_List_List*
_tmp10BB;struct Cyc_Absyn_Switch_clause*_tmp10BA;union Cyc_YYSTYPE_union _tmp10B9;
yyval=(union Cyc_YYSTYPE_union)(((_tmp10B9.YY10).tag=14,(((_tmp10B9.YY10).f1=((
_tmp10BB=_cycalloc(sizeof(*_tmp10BB)),((_tmp10BB->hd=((_tmp10BA=_cycalloc(
sizeof(*_tmp10BA)),((_tmp10BA->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp10BA->pat_vars=0,((
_tmp10BA->where_clause=0,((_tmp10BA->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),((_tmp10BA->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmp10BA)))))))))))),((_tmp10BB->tl=Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp10BB)))))),
_tmp10B9))));}break;case 296: _LL328:{struct Cyc_List_List*_tmp10C1;struct Cyc_Absyn_Switch_clause*
_tmp10C0;union Cyc_YYSTYPE_union _tmp10BF;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10BF.YY10).tag=14,(((_tmp10BF.YY10).f1=((_tmp10C1=_cycalloc(sizeof(*_tmp10C1)),((
_tmp10C1->hd=((_tmp10C0=_cycalloc(sizeof(*_tmp10C0)),((_tmp10C0->pattern=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp10C0->pat_vars=
0,((_tmp10C0->where_clause=0,((_tmp10C0->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp10C0->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp10C0)))))))))))),((
_tmp10C1->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp10C1)))))),_tmp10BF))));}break;case 297: _LL329:{struct Cyc_List_List*
_tmp10C7;struct Cyc_Absyn_Switch_clause*_tmp10C6;union Cyc_YYSTYPE_union _tmp10C5;
yyval=(union Cyc_YYSTYPE_union)(((_tmp10C5.YY10).tag=14,(((_tmp10C5.YY10).f1=((
_tmp10C7=_cycalloc(sizeof(*_tmp10C7)),((_tmp10C7->hd=((_tmp10C6=_cycalloc(
sizeof(*_tmp10C6)),((_tmp10C6->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmp10C6->pat_vars=0,((
_tmp10C6->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp10C6->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),((_tmp10C6->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmp10C6)))))))))))),((
_tmp10C7->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp10C7)))))),_tmp10C5))));}break;case 298: _LL32A:{struct Cyc_List_List*
_tmp10CD;struct Cyc_Absyn_Switch_clause*_tmp10CC;union Cyc_YYSTYPE_union _tmp10CB;
yyval=(union Cyc_YYSTYPE_union)(((_tmp10CB.YY10).tag=14,(((_tmp10CB.YY10).f1=((
_tmp10CD=_cycalloc(sizeof(*_tmp10CD)),((_tmp10CD->hd=((_tmp10CC=_cycalloc(
sizeof(*_tmp10CC)),((_tmp10CC->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),((_tmp10CC->pat_vars=0,((
_tmp10CC->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp10CC->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp10CC->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmp10CC)))))))))))),((
_tmp10CD->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp10CD)))))),_tmp10CB))));}break;case 299: _LL32B:{union Cyc_YYSTYPE_union
_tmp10CE;yyval=(union Cyc_YYSTYPE_union)(((_tmp10CE.YY9).tag=13,(((_tmp10CE.YY9).f1=
Cyc_Absyn_while_stmt(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp10CE))));}break;case 300: _LL32C:{union Cyc_YYSTYPE_union
_tmp10CF;yyval=(union Cyc_YYSTYPE_union)(((_tmp10CF.YY9).tag=13,(((_tmp10CF.YY9).f1=
Cyc_Absyn_do_stmt(Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10CF))));}
break;case 301: _LL32D:{union Cyc_YYSTYPE_union _tmp10D0;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10D0.YY9).tag=13,(((_tmp10D0.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10D0))));}
break;case 302: _LL32E:{union Cyc_YYSTYPE_union _tmp10D1;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10D1.YY9).tag=13,(((_tmp10D1.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp10D1))));}break;case 303: _LL32F:{union Cyc_YYSTYPE_union
_tmp10D2;yyval=(union Cyc_YYSTYPE_union)(((_tmp10D2.YY9).tag=13,(((_tmp10D2.YY9).f1=
Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10D2))));}
break;case 304: _LL330:{union Cyc_YYSTYPE_union _tmp10D3;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10D3.YY9).tag=13,(((_tmp10D3.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp10D3))));}
break;case 305: _LL331:{union Cyc_YYSTYPE_union _tmp10D4;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10D4.YY9).tag=13,(((_tmp10D4.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp10D4))));}break;case 306: _LL332:{union Cyc_YYSTYPE_union
_tmp10D5;yyval=(union Cyc_YYSTYPE_union)(((_tmp10D5.YY9).tag=13,(((_tmp10D5.YY9).f1=
Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10D5))));}
break;case 307: _LL333:{union Cyc_YYSTYPE_union _tmp10D6;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10D6.YY9).tag=13,(((_tmp10D6.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10D6))));}
break;case 308: _LL334:{union Cyc_YYSTYPE_union _tmp10D7;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10D7.YY9).tag=13,(((_tmp10D7.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10D7))));}
break;case 309: _LL335: {struct Cyc_List_List*_tmp772=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp773=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{union Cyc_YYSTYPE_union
_tmp10D8;yyval=(union Cyc_YYSTYPE_union)(((_tmp10D8.YY9).tag=13,(((_tmp10D8.YY9).f1=
Cyc_Parse_flatten_declarations(_tmp772,_tmp773),_tmp10D8))));}break;}case 310:
_LL336: {struct Cyc_List_List*_tmp775=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp776=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{union Cyc_YYSTYPE_union
_tmp10D9;yyval=(union Cyc_YYSTYPE_union)(((_tmp10D9.YY9).tag=13,(((_tmp10D9.YY9).f1=
Cyc_Parse_flatten_declarations(_tmp775,_tmp776),_tmp10D9))));}break;}case 311:
_LL337: {struct Cyc_List_List*_tmp778=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp779=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{union Cyc_YYSTYPE_union
_tmp10DA;yyval=(union Cyc_YYSTYPE_union)(((_tmp10DA.YY9).tag=13,(((_tmp10DA.YY9).f1=
Cyc_Parse_flatten_declarations(_tmp778,_tmp779),_tmp10DA))));}break;}case 312:
_LL338: {struct Cyc_List_List*_tmp77B=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*
_tmp77C=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{union Cyc_YYSTYPE_union
_tmp10DB;yyval=(union Cyc_YYSTYPE_union)(((_tmp10DB.YY9).tag=13,(((_tmp10DB.YY9).f1=
Cyc_Parse_flatten_declarations(_tmp77B,_tmp77C),_tmp10DB))));}break;}case 313:
_LL339:{struct _dyneither_ptr*_tmp10DE;union Cyc_YYSTYPE_union _tmp10DD;yyval=(
union Cyc_YYSTYPE_union)(((_tmp10DD.YY9).tag=13,(((_tmp10DD.YY9).f1=Cyc_Absyn_goto_stmt(((
_tmp10DE=_cycalloc(sizeof(*_tmp10DE)),((_tmp10DE[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmp10DE)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp10DD))));}
break;case 314: _LL33A:{union Cyc_YYSTYPE_union _tmp10DF;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10DF.YY9).tag=13,(((_tmp10DF.YY9).f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp10DF))));}
break;case 315: _LL33B:{union Cyc_YYSTYPE_union _tmp10E0;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10E0.YY9).tag=13,(((_tmp10E0.YY9).f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp10E0))));}
break;case 316: _LL33C:{union Cyc_YYSTYPE_union _tmp10E1;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10E1.YY9).tag=13,(((_tmp10E1.YY9).f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp10E1))));}
break;case 317: _LL33D:{union Cyc_YYSTYPE_union _tmp10E2;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10E2.YY9).tag=13,(((_tmp10E2.YY9).f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp10E2))));}
break;case 318: _LL33E:{union Cyc_YYSTYPE_union _tmp10E3;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10E3.YY9).tag=13,(((_tmp10E3.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp10E3))));}
break;case 319: _LL33F:{union Cyc_YYSTYPE_union _tmp10E4;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10E4.YY9).tag=13,(((_tmp10E4.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)),_tmp10E4))));}
break;case 320: _LL340:{union Cyc_YYSTYPE_union _tmp10E5;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10E5.YY9).tag=13,(((_tmp10E5.YY9).f1=Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line)),_tmp10E5))));}
break;case 321: _LL341: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 322: _LL342: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 323: _LL343:{union Cyc_YYSTYPE_union _tmp10E6;yyval=(
union Cyc_YYSTYPE_union)(((_tmp10E6.YY11).tag=15,(((_tmp10E6.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10E6))));}
break;case 324: _LL344: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 325: _LL345:{union Cyc_YYSTYPE_union _tmp10E7;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10E7.YY11).tag=15,(((_tmp10E7.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10E7))));}
break;case 326: _LL346: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 327: _LL347:{union Cyc_YYSTYPE_union _tmp10E8;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10E8.YY11).tag=15,(((_tmp10E8.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10E8))));}
break;case 328: _LL348: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 329: _LL349:{union Cyc_YYSTYPE_union _tmp10E9;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10E9.YY11).tag=15,(((_tmp10E9.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)14,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10E9))));}
break;case 330: _LL34A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 331: _LL34B:{union Cyc_YYSTYPE_union _tmp10EA;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10EA.YY11).tag=15,(((_tmp10EA.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)15,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10EA))));}
break;case 332: _LL34C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 333: _LL34D:{union Cyc_YYSTYPE_union _tmp10EB;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10EB.YY11).tag=15,(((_tmp10EB.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)13,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10EB))));}
break;case 334: _LL34E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 335: _LL34F:{union Cyc_YYSTYPE_union _tmp10EC;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10EC.YY11).tag=15,(((_tmp10EC.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10EC))));}
break;case 336: _LL350:{union Cyc_YYSTYPE_union _tmp10ED;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10ED.YY11).tag=15,(((_tmp10ED.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp10ED))));}break;case 337: _LL351: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 338: _LL352:{union
Cyc_YYSTYPE_union _tmp10EE;yyval=(union Cyc_YYSTYPE_union)(((_tmp10EE.YY11).tag=15,(((
_tmp10EE.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10EE))));}
break;case 339: _LL353:{union Cyc_YYSTYPE_union _tmp10EF;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10EF.YY11).tag=15,(((_tmp10EF.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp10EF))));}break;case 340: _LL354:{union Cyc_YYSTYPE_union
_tmp10F0;yyval=(union Cyc_YYSTYPE_union)(((_tmp10F0.YY11).tag=15,(((_tmp10F0.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10F0))));}
break;case 341: _LL355:{union Cyc_YYSTYPE_union _tmp10F1;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10F1.YY11).tag=15,(((_tmp10F1.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp10F1))));}break;case 342: _LL356: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 343: _LL357:{union
Cyc_YYSTYPE_union _tmp10F2;yyval=(union Cyc_YYSTYPE_union)(((_tmp10F2.YY11).tag=15,(((
_tmp10F2.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)16,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10F2))));}
break;case 344: _LL358:{union Cyc_YYSTYPE_union _tmp10F3;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10F3.YY11).tag=15,(((_tmp10F3.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)17,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10F3))));}
break;case 345: _LL359: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 346: _LL35A:{union Cyc_YYSTYPE_union _tmp10F4;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10F4.YY11).tag=15,(((_tmp10F4.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)0,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10F4))));}
break;case 347: _LL35B:{union Cyc_YYSTYPE_union _tmp10F5;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10F5.YY11).tag=15,(((_tmp10F5.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)2,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10F5))));}
break;case 348: _LL35C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 349: _LL35D:{union Cyc_YYSTYPE_union _tmp10F6;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10F6.YY11).tag=15,(((_tmp10F6.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)1,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10F6))));}
break;case 350: _LL35E:{union Cyc_YYSTYPE_union _tmp10F7;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10F7.YY11).tag=15,(((_tmp10F7.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)3,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10F7))));}
break;case 351: _LL35F:{union Cyc_YYSTYPE_union _tmp10F8;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10F8.YY11).tag=15,(((_tmp10F8.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)4,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10F8))));}
break;case 352: _LL360: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 353: _LL361:{union Cyc_YYSTYPE_union _tmp10F9;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10F9.YY11).tag=15,(((_tmp10F9.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10F9))));}
break;case 354: _LL362: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 355: _LL363:{union Cyc_YYSTYPE_union _tmp10FA;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10FA.YY11).tag=15,(((_tmp10FA.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),
_tmp10FA))));}break;case 356: _LL364:{union Cyc_YYSTYPE_union _tmp10FB;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10FB.YY11).tag=15,(((_tmp10FB.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp10FB))));}break;case 357: _LL365:{union Cyc_YYSTYPE_union
_tmp10FC;yyval=(union Cyc_YYSTYPE_union)(((_tmp10FC.YY11).tag=15,(((_tmp10FC.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10FC))));}
break;case 358: _LL366:{union Cyc_YYSTYPE_union _tmp10FD;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10FD.YY11).tag=15,(((_tmp10FD.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10FD))));}
break;case 359: _LL367:{struct Cyc_Absyn_StructField_struct*_tmp1107;struct
_dyneither_ptr*_tmp1106;struct Cyc_Absyn_StructField_struct _tmp1105;union Cyc_YYSTYPE_union
_tmp1104;yyval=(union Cyc_YYSTYPE_union)(((_tmp1104.YY11).tag=15,(((_tmp1104.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)((_tmp1107=
_cycalloc(sizeof(*_tmp1107)),((_tmp1107[0]=((_tmp1105.tag=0,((_tmp1105.f1=((
_tmp1106=_cycalloc(sizeof(*_tmp1106)),((_tmp1106[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmp1106)))),_tmp1105)))),
_tmp1107)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp1104))));}break;case 360: _LL368:{struct Cyc_Absyn_TupleIndex_struct*
_tmp110D;struct Cyc_Absyn_TupleIndex_struct _tmp110C;union Cyc_YYSTYPE_union
_tmp110B;yyval=(union Cyc_YYSTYPE_union)(((_tmp110B.YY11).tag=15,(((_tmp110B.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)((_tmp110D=
_cycalloc_atomic(sizeof(*_tmp110D)),((_tmp110D[0]=((_tmp110C.tag=1,((_tmp110C.f1=(
unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f2,_tmp110C)))),_tmp110D)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp110B))));}
break;case 361: _LL369: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 362: _LL36A: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 363: _LL36B:{union Cyc_YYSTYPE_union _tmp110E;yyval=(
union Cyc_YYSTYPE_union)(((_tmp110E.YY11).tag=15,(((_tmp110E.YY11).f1=Cyc_Absyn_new_pat((
void*)0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp110E))));}break;case 364: _LL36C:{union Cyc_YYSTYPE_union
_tmp110F;yyval=(union Cyc_YYSTYPE_union)(((_tmp110F.YY11).tag=15,(((_tmp110F.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])),_tmp110F))));}break;case 365: _LL36D: {struct Cyc_Absyn_Exp*e=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);{void*
_tmp7A8=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp7A9;void*_tmp7AA;char _tmp7AB;
union Cyc_Absyn_Cnst_union _tmp7AC;void*_tmp7AD;short _tmp7AE;union Cyc_Absyn_Cnst_union
_tmp7AF;void*_tmp7B0;int _tmp7B1;union Cyc_Absyn_Cnst_union _tmp7B2;struct
_dyneither_ptr _tmp7B3;union Cyc_Absyn_Cnst_union _tmp7B4;union Cyc_Absyn_Cnst_union
_tmp7B5;union Cyc_Absyn_Cnst_union _tmp7B6;_LL370: if(*((int*)_tmp7A8)!= 0)goto
_LL372;_tmp7A9=((struct Cyc_Absyn_Const_e_struct*)_tmp7A8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7A8)->f1).Char_c).tag != 0)goto _LL372;_tmp7AA=(_tmp7A9.Char_c).f1;_tmp7AB=(
_tmp7A9.Char_c).f2;_LL371:{struct Cyc_Absyn_Char_p_struct*_tmp1115;struct Cyc_Absyn_Char_p_struct
_tmp1114;union Cyc_YYSTYPE_union _tmp1113;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1113.YY11).tag=15,(((_tmp1113.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp1115=
_cycalloc_atomic(sizeof(*_tmp1115)),((_tmp1115[0]=((_tmp1114.tag=8,((_tmp1114.f1=
_tmp7AB,_tmp1114)))),_tmp1115)))),e->loc),_tmp1113))));}goto _LL36F;_LL372: if(*((
int*)_tmp7A8)!= 0)goto _LL374;_tmp7AC=((struct Cyc_Absyn_Const_e_struct*)_tmp7A8)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A8)->f1).Short_c).tag != 1)goto _LL374;
_tmp7AD=(_tmp7AC.Short_c).f1;_tmp7AE=(_tmp7AC.Short_c).f2;_LL373:{struct Cyc_Absyn_Int_p_struct*
_tmp111B;struct Cyc_Absyn_Int_p_struct _tmp111A;union Cyc_YYSTYPE_union _tmp1119;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1119.YY11).tag=15,(((_tmp1119.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp111B=_cycalloc(sizeof(*_tmp111B)),((_tmp111B[0]=((_tmp111A.tag=7,((
_tmp111A.f1=(void*)_tmp7AD,((_tmp111A.f2=(int)_tmp7AE,_tmp111A)))))),_tmp111B)))),
e->loc),_tmp1119))));}goto _LL36F;_LL374: if(*((int*)_tmp7A8)!= 0)goto _LL376;
_tmp7AF=((struct Cyc_Absyn_Const_e_struct*)_tmp7A8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7A8)->f1).Int_c).tag != 2)goto _LL376;_tmp7B0=(_tmp7AF.Int_c).f1;_tmp7B1=(
_tmp7AF.Int_c).f2;_LL375:{struct Cyc_Absyn_Int_p_struct*_tmp1121;struct Cyc_Absyn_Int_p_struct
_tmp1120;union Cyc_YYSTYPE_union _tmp111F;yyval=(union Cyc_YYSTYPE_union)(((
_tmp111F.YY11).tag=15,(((_tmp111F.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp1121=
_cycalloc(sizeof(*_tmp1121)),((_tmp1121[0]=((_tmp1120.tag=7,((_tmp1120.f1=(void*)
_tmp7B0,((_tmp1120.f2=_tmp7B1,_tmp1120)))))),_tmp1121)))),e->loc),_tmp111F))));}
goto _LL36F;_LL376: if(*((int*)_tmp7A8)!= 0)goto _LL378;_tmp7B2=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A8)->f1).Float_c).tag != 
4)goto _LL378;_tmp7B3=(_tmp7B2.Float_c).f1;_LL377:{struct Cyc_Absyn_Float_p_struct*
_tmp1127;struct Cyc_Absyn_Float_p_struct _tmp1126;union Cyc_YYSTYPE_union _tmp1125;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1125.YY11).tag=15,(((_tmp1125.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp1127=_cycalloc(sizeof(*_tmp1127)),((_tmp1127[0]=((_tmp1126.tag=9,((
_tmp1126.f1=_tmp7B3,_tmp1126)))),_tmp1127)))),e->loc),_tmp1125))));}goto _LL36F;
_LL378: if(*((int*)_tmp7A8)!= 0)goto _LL37A;_tmp7B4=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A8)->f1).Null_c).tag != 
6)goto _LL37A;_LL379:{union Cyc_YYSTYPE_union _tmp1128;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1128.YY11).tag=15,(((_tmp1128.YY11).f1=Cyc_Absyn_new_pat((void*)1,e->loc),
_tmp1128))));}goto _LL36F;_LL37A: if(*((int*)_tmp7A8)!= 0)goto _LL37C;_tmp7B5=((
struct Cyc_Absyn_Const_e_struct*)_tmp7A8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7A8)->f1).String_c).tag != 5)goto _LL37C;_LL37B:{const char*_tmp1129;Cyc_Parse_err(((
_tmp1129="strings cannot occur within patterns",_tag_dyneither(_tmp1129,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}goto _LL36F;_LL37C: if(*((int*)_tmp7A8)!= 0)goto
_LL37E;_tmp7B6=((struct Cyc_Absyn_Const_e_struct*)_tmp7A8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7A8)->f1).LongLong_c).tag != 3)goto _LL37E;_LL37D:{const char*_tmp112A;Cyc_Parse_unimp(((
_tmp112A="long long's in patterns",_tag_dyneither(_tmp112A,sizeof(char),24))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}goto _LL36F;_LL37E:;_LL37F: {const char*_tmp112B;Cyc_Parse_err(((
_tmp112B="bad constant in case",_tag_dyneither(_tmp112B,sizeof(char),21))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}_LL36F:;}
break;}case 366: _LL36E:{struct Cyc_Absyn_UnknownId_p_struct*_tmp1131;struct Cyc_Absyn_UnknownId_p_struct
_tmp1130;union Cyc_YYSTYPE_union _tmp112F;yyval=(union Cyc_YYSTYPE_union)(((
_tmp112F.YY11).tag=15,(((_tmp112F.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp1131=
_cycalloc(sizeof(*_tmp1131)),((_tmp1131[0]=((_tmp1130.tag=12,((_tmp1130.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1130)))),_tmp1131)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp112F))));}break;case 367: _LL380:{const char*
_tmp1132;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp1132="as",
_tag_dyneither(_tmp1132,sizeof(char),3))))!= 0){const char*_tmp1133;Cyc_Parse_err(((
_tmp1133="expecting `as'",_tag_dyneither(_tmp1133,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Var_p_struct*
_tmp1147;struct _tuple1*_tmp1146;struct _dyneither_ptr*_tmp1145;union Cyc_Absyn_Nmspace_union
_tmp1144;struct Cyc_Absyn_Var_p_struct _tmp1143;union Cyc_YYSTYPE_union _tmp1142;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1142.YY11).tag=15,(((_tmp1142.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp1147=_cycalloc(sizeof(*_tmp1147)),((_tmp1147[0]=((_tmp1143.tag=0,((
_tmp1143.f1=Cyc_Absyn_new_vardecl(((_tmp1146=_cycalloc(sizeof(*_tmp1146)),((
_tmp1146->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp1144.Loc_n).tag=0,_tmp1144)),((
_tmp1146->f2=((_tmp1145=_cycalloc(sizeof(*_tmp1145)),((_tmp1145[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp1145)))),
_tmp1146)))))),(void*)0,0),((_tmp1143.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1143)))))),_tmp1147)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)),_tmp1142))));}break;case 368: _LL381: {struct Cyc_List_List*
_tmp7D3;int _tmp7D4;struct _tuple13 _tmp7D2=*Cyc_yyget_YY12(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7D3=_tmp7D2.f1;
_tmp7D4=_tmp7D2.f2;{struct Cyc_Absyn_Tuple_p_struct*_tmp114D;struct Cyc_Absyn_Tuple_p_struct
_tmp114C;union Cyc_YYSTYPE_union _tmp114B;yyval=(union Cyc_YYSTYPE_union)(((
_tmp114B.YY11).tag=15,(((_tmp114B.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp114D=
_cycalloc(sizeof(*_tmp114D)),((_tmp114D[0]=((_tmp114C.tag=3,((_tmp114C.f1=
_tmp7D3,((_tmp114C.f2=_tmp7D4,_tmp114C)))))),_tmp114D)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp114B))));}
break;}case 369: _LL382: {struct Cyc_List_List*_tmp7D9;int _tmp7DA;struct _tuple13
_tmp7D8=*Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7D9=_tmp7D8.f1;_tmp7DA=_tmp7D8.f2;{struct Cyc_Absyn_UnknownCall_p_struct*
_tmp1153;struct Cyc_Absyn_UnknownCall_p_struct _tmp1152;union Cyc_YYSTYPE_union
_tmp1151;yyval=(union Cyc_YYSTYPE_union)(((_tmp1151.YY11).tag=15,(((_tmp1151.YY11).f1=
Cyc_Absyn_new_pat((void*)((_tmp1153=_cycalloc(sizeof(*_tmp1153)),((_tmp1153[0]=((
_tmp1152.tag=13,((_tmp1152.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp1152.f2=_tmp7D9,((
_tmp1152.f3=_tmp7DA,_tmp1152)))))))),_tmp1153)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1151))));}
break;}case 370: _LL383: {struct Cyc_List_List*_tmp7DF;int _tmp7E0;struct _tuple13
_tmp7DE=*Cyc_yyget_YY16(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7DF=_tmp7DE.f1;_tmp7E0=_tmp7DE.f2;{struct Cyc_List_List*_tmp7E1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct*_tmp1162;struct Cyc_Absyn_AggrInfo
_tmp1161;union Cyc_Absyn_AggrInfoU_union _tmp1160;struct Cyc_Absyn_Aggr_p_struct
_tmp115F;union Cyc_YYSTYPE_union _tmp115E;yyval=(union Cyc_YYSTYPE_union)(((
_tmp115E.YY11).tag=15,(((_tmp115E.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp1162=
_cycalloc(sizeof(*_tmp1162)),((_tmp1162[0]=((_tmp115F.tag=5,((_tmp115F.f1=((
_tmp1161.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((_tmp1160.UnknownAggr).tag=
0,(((_tmp1160.UnknownAggr).f1=(void*)((void*)0),(((_tmp1160.UnknownAggr).f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp1160)))))),((
_tmp1161.targs=0,_tmp1161)))),((_tmp115F.f2=_tmp7E1,((_tmp115F.f3=_tmp7DF,((
_tmp115F.f4=_tmp7E0,_tmp115F)))))))))),_tmp1162)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp115E))));}
break;}}case 371: _LL384:{struct Cyc_Absyn_Pointer_p_struct*_tmp1168;struct Cyc_Absyn_Pointer_p_struct
_tmp1167;union Cyc_YYSTYPE_union _tmp1166;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1166.YY11).tag=15,(((_tmp1166.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp1168=
_cycalloc(sizeof(*_tmp1168)),((_tmp1168[0]=((_tmp1167.tag=4,((_tmp1167.f1=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1167)))),_tmp1168)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1166))));}break;case 372: _LL385:{struct Cyc_Absyn_Pointer_p_struct*
_tmp1177;struct Cyc_Absyn_Pointer_p_struct*_tmp1176;struct Cyc_Absyn_Pointer_p_struct
_tmp1175;struct Cyc_Absyn_Pointer_p_struct _tmp1174;union Cyc_YYSTYPE_union _tmp1173;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1173.YY11).tag=15,(((_tmp1173.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp1177=_cycalloc(sizeof(*_tmp1177)),((_tmp1177[0]=((_tmp1174.tag=4,((
_tmp1174.f1=Cyc_Absyn_new_pat((void*)((_tmp1176=_cycalloc(sizeof(*_tmp1176)),((
_tmp1176[0]=((_tmp1175.tag=4,((_tmp1175.f1=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1175)))),_tmp1176)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1174)))),_tmp1177)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1173))));}
break;case 373: _LL386:{struct Cyc_Absyn_Reference_p_struct*_tmp118B;struct _tuple1*
_tmp118A;struct _dyneither_ptr*_tmp1189;union Cyc_Absyn_Nmspace_union _tmp1188;
struct Cyc_Absyn_Reference_p_struct _tmp1187;union Cyc_YYSTYPE_union _tmp1186;yyval=(
union Cyc_YYSTYPE_union)(((_tmp1186.YY11).tag=15,(((_tmp1186.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp118B=_cycalloc(sizeof(*_tmp118B)),((_tmp118B[0]=((_tmp1187.tag=1,((
_tmp1187.f1=Cyc_Absyn_new_vardecl(((_tmp118A=_cycalloc(sizeof(*_tmp118A)),((
_tmp118A->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp1188.Loc_n).tag=0,_tmp1188)),((
_tmp118A->f2=((_tmp1189=_cycalloc(sizeof(*_tmp1189)),((_tmp1189[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1189)))),_tmp118A)))))),(
void*)0,0),((_tmp1187.f2=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1187)))))),
_tmp118B)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1186))));}break;case 374: _LL387:{const char*
_tmp118C;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp118C="as",
_tag_dyneither(_tmp118C,sizeof(char),3))))!= 0){const char*_tmp118D;Cyc_Parse_err(((
_tmp118D="expecting `as'",_tag_dyneither(_tmp118D,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Reference_p_struct*
_tmp11A1;struct _tuple1*_tmp11A0;struct _dyneither_ptr*_tmp119F;union Cyc_Absyn_Nmspace_union
_tmp119E;struct Cyc_Absyn_Reference_p_struct _tmp119D;union Cyc_YYSTYPE_union
_tmp119C;yyval=(union Cyc_YYSTYPE_union)(((_tmp119C.YY11).tag=15,(((_tmp119C.YY11).f1=
Cyc_Absyn_new_pat((void*)((_tmp11A1=_cycalloc(sizeof(*_tmp11A1)),((_tmp11A1[0]=((
_tmp119D.tag=1,((_tmp119D.f1=Cyc_Absyn_new_vardecl(((_tmp11A0=_cycalloc(sizeof(*
_tmp11A0)),((_tmp11A0->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp119E.Loc_n).tag=0,
_tmp119E)),((_tmp11A0->f2=((_tmp119F=_cycalloc(sizeof(*_tmp119F)),((_tmp119F[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmp119F)))),_tmp11A0)))))),(void*)0,0),((_tmp119D.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp119D)))))),_tmp11A1)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)),_tmp119C))));}break;case 375: _LL388: {void*_tmp7FD=
Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_Tcutil_kind_to_bound((void*)7));{struct Cyc_Absyn_TagInt_p_struct*
_tmp11BE;struct _tuple1*_tmp11BD;struct _dyneither_ptr*_tmp11BC;union Cyc_Absyn_Nmspace_union
_tmp11BB;struct Cyc_Absyn_TagType_struct*_tmp11BA;struct Cyc_Absyn_TagType_struct
_tmp11B9;struct Cyc_Absyn_TagInt_p_struct _tmp11B8;union Cyc_YYSTYPE_union _tmp11B7;
yyval=(union Cyc_YYSTYPE_union)(((_tmp11B7.YY11).tag=15,(((_tmp11B7.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp11BE=_cycalloc(sizeof(*_tmp11BE)),((_tmp11BE[0]=((_tmp11B8.tag=2,((
_tmp11B8.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp7FD),((
_tmp11B8.f2=Cyc_Absyn_new_vardecl(((_tmp11BD=_cycalloc(sizeof(*_tmp11BD)),((
_tmp11BD->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp11BB.Loc_n).tag=0,_tmp11BB)),((
_tmp11BD->f2=((_tmp11BC=_cycalloc(sizeof(*_tmp11BC)),((_tmp11BC[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp11BC)))),
_tmp11BD)))))),(void*)((_tmp11BA=_cycalloc(sizeof(*_tmp11BA)),((_tmp11BA[0]=((
_tmp11B9.tag=18,((_tmp11B9.f1=(void*)_tmp7FD,_tmp11B9)))),_tmp11BA)))),0),
_tmp11B8)))))),_tmp11BE)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp11B7))));}
break;}case 376: _LL389: {struct Cyc_Absyn_Tvar*_tmp806=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound((
void*)7));{struct Cyc_Absyn_TagInt_p_struct*_tmp11E8;struct _tuple1*_tmp11E7;
struct _dyneither_ptr*_tmp11E6;union Cyc_Absyn_Nmspace_union _tmp11E5;struct Cyc_Absyn_TagType_struct*
_tmp11E4;struct Cyc_Absyn_VarType_struct*_tmp11E3;struct Cyc_Absyn_VarType_struct
_tmp11E2;struct Cyc_Absyn_TagType_struct _tmp11E1;struct Cyc_Absyn_TagInt_p_struct
_tmp11E0;union Cyc_YYSTYPE_union _tmp11DF;yyval=(union Cyc_YYSTYPE_union)(((
_tmp11DF.YY11).tag=15,(((_tmp11DF.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp11E8=
_cycalloc(sizeof(*_tmp11E8)),((_tmp11E8[0]=((_tmp11E0.tag=2,((_tmp11E0.f1=
_tmp806,((_tmp11E0.f2=Cyc_Absyn_new_vardecl(((_tmp11E7=_cycalloc(sizeof(*
_tmp11E7)),((_tmp11E7->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp11E5.Loc_n).tag=0,
_tmp11E5)),((_tmp11E7->f2=((_tmp11E6=_cycalloc(sizeof(*_tmp11E6)),((_tmp11E6[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmp11E6)))),_tmp11E7)))))),(void*)((_tmp11E4=_cycalloc(sizeof(*_tmp11E4)),((
_tmp11E4[0]=((_tmp11E1.tag=18,((_tmp11E1.f1=(void*)((void*)((_tmp11E3=_cycalloc(
sizeof(*_tmp11E3)),((_tmp11E3[0]=((_tmp11E2.tag=1,((_tmp11E2.f1=_tmp806,_tmp11E2)))),
_tmp11E3))))),_tmp11E1)))),_tmp11E4)))),0),_tmp11E0)))))),_tmp11E8)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp11DF))));}
break;}case 377: _LL38A:{struct _tuple13*_tmp11EB;union Cyc_YYSTYPE_union _tmp11EA;
yyval=(union Cyc_YYSTYPE_union)(((_tmp11EA.YY12).tag=16,(((_tmp11EA.YY12).f1=((
_tmp11EB=_cycalloc(sizeof(*_tmp11EB)),((_tmp11EB->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),((_tmp11EB->f2=0,_tmp11EB)))))),
_tmp11EA))));}break;case 378: _LL38B:{struct _tuple13*_tmp11EE;union Cyc_YYSTYPE_union
_tmp11ED;yyval=(union Cyc_YYSTYPE_union)(((_tmp11ED.YY12).tag=16,(((_tmp11ED.YY12).f1=((
_tmp11EE=_cycalloc(sizeof(*_tmp11EE)),((_tmp11EE->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmp11EE->f2=1,
_tmp11EE)))))),_tmp11ED))));}break;case 379: _LL38C:{struct _tuple13*_tmp11F1;union
Cyc_YYSTYPE_union _tmp11F0;yyval=(union Cyc_YYSTYPE_union)(((_tmp11F0.YY12).tag=16,(((
_tmp11F0.YY12).f1=((_tmp11F1=_cycalloc(sizeof(*_tmp11F1)),((_tmp11F1->f1=0,((
_tmp11F1->f2=1,_tmp11F1)))))),_tmp11F0))));}break;case 380: _LL38D:{struct Cyc_List_List*
_tmp11F4;union Cyc_YYSTYPE_union _tmp11F3;yyval=(union Cyc_YYSTYPE_union)(((
_tmp11F3.YY13).tag=17,(((_tmp11F3.YY13).f1=((_tmp11F4=_cycalloc(sizeof(*_tmp11F4)),((
_tmp11F4->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmp11F4->tl=0,_tmp11F4)))))),_tmp11F3))));}break;case 381:
_LL38E:{struct Cyc_List_List*_tmp11F7;union Cyc_YYSTYPE_union _tmp11F6;yyval=(union
Cyc_YYSTYPE_union)(((_tmp11F6.YY13).tag=17,(((_tmp11F6.YY13).f1=((_tmp11F7=
_cycalloc(sizeof(*_tmp11F7)),((_tmp11F7->hd=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp11F7->tl=Cyc_yyget_YY13(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp11F7)))))),
_tmp11F6))));}break;case 382: _LL38F:{struct _tuple14*_tmp11FA;union Cyc_YYSTYPE_union
_tmp11F9;yyval=(union Cyc_YYSTYPE_union)(((_tmp11F9.YY14).tag=18,(((_tmp11F9.YY14).f1=((
_tmp11FA=_cycalloc(sizeof(*_tmp11FA)),((_tmp11FA->f1=0,((_tmp11FA->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp11FA)))))),
_tmp11F9))));}break;case 383: _LL390:{struct _tuple14*_tmp11FD;union Cyc_YYSTYPE_union
_tmp11FC;yyval=(union Cyc_YYSTYPE_union)(((_tmp11FC.YY14).tag=18,(((_tmp11FC.YY14).f1=((
_tmp11FD=_cycalloc(sizeof(*_tmp11FD)),((_tmp11FD->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp11FD->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp11FD)))))),
_tmp11FC))));}break;case 384: _LL391:{struct _tuple13*_tmp1200;union Cyc_YYSTYPE_union
_tmp11FF;yyval=(union Cyc_YYSTYPE_union)(((_tmp11FF.YY16).tag=20,(((_tmp11FF.YY16).f1=((
_tmp1200=_cycalloc(sizeof(*_tmp1200)),((_tmp1200->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),((_tmp1200->f2=0,_tmp1200)))))),
_tmp11FF))));}break;case 385: _LL392:{struct _tuple13*_tmp1203;union Cyc_YYSTYPE_union
_tmp1202;yyval=(union Cyc_YYSTYPE_union)(((_tmp1202.YY16).tag=20,(((_tmp1202.YY16).f1=((
_tmp1203=_cycalloc(sizeof(*_tmp1203)),((_tmp1203->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmp1203->f2=1,
_tmp1203)))))),_tmp1202))));}break;case 386: _LL393:{struct _tuple13*_tmp1206;union
Cyc_YYSTYPE_union _tmp1205;yyval=(union Cyc_YYSTYPE_union)(((_tmp1205.YY16).tag=20,(((
_tmp1205.YY16).f1=((_tmp1206=_cycalloc(sizeof(*_tmp1206)),((_tmp1206->f1=0,((
_tmp1206->f2=1,_tmp1206)))))),_tmp1205))));}break;case 387: _LL394:{struct Cyc_List_List*
_tmp1209;union Cyc_YYSTYPE_union _tmp1208;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1208.YY15).tag=19,(((_tmp1208.YY15).f1=((_tmp1209=_cycalloc(sizeof(*_tmp1209)),((
_tmp1209->hd=Cyc_yyget_YY14(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmp1209->tl=0,_tmp1209)))))),_tmp1208))));}break;case 388:
_LL395:{struct Cyc_List_List*_tmp120C;union Cyc_YYSTYPE_union _tmp120B;yyval=(union
Cyc_YYSTYPE_union)(((_tmp120B.YY15).tag=19,(((_tmp120B.YY15).f1=((_tmp120C=
_cycalloc(sizeof(*_tmp120C)),((_tmp120C->hd=Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp120C->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp120C)))))),
_tmp120B))));}break;case 389: _LL396: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 390: _LL397:{union Cyc_YYSTYPE_union _tmp120D;yyval=(
union Cyc_YYSTYPE_union)(((_tmp120D.YY3).tag=7,(((_tmp120D.YY3).f1=Cyc_Absyn_seq_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp120D))));}
break;case 391: _LL398: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 392: _LL399:{union Cyc_YYSTYPE_union _tmp120E;yyval=(union
Cyc_YYSTYPE_union)(((_tmp120E.YY3).tag=7,(((_tmp120E.YY3).f1=Cyc_Absyn_assignop_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp120E))));}
break;case 393: _LL39A:{struct Cyc_Absyn_Swap_e_struct*_tmp1214;struct Cyc_Absyn_Swap_e_struct
_tmp1213;union Cyc_YYSTYPE_union _tmp1212;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1212.YY3).tag=7,(((_tmp1212.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp1214=
_cycalloc(sizeof(*_tmp1214)),((_tmp1214[0]=((_tmp1213.tag=36,((_tmp1213.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp1213.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1213)))))),
_tmp1214)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1212))));}break;case 394: _LL39B:{union Cyc_YYSTYPE_union
_tmp1215;yyval=(union Cyc_YYSTYPE_union)(((_tmp1215.YY8).tag=12,(((_tmp1215.YY8).f1=
0,_tmp1215))));}break;case 395: _LL39C:{struct Cyc_Core_Opt*_tmp1218;union Cyc_YYSTYPE_union
_tmp1217;yyval=(union Cyc_YYSTYPE_union)(((_tmp1217.YY8).tag=12,(((_tmp1217.YY8).f1=((
_tmp1218=_cycalloc(sizeof(*_tmp1218)),((_tmp1218->v=(void*)((void*)1),_tmp1218)))),
_tmp1217))));}break;case 396: _LL39D:{struct Cyc_Core_Opt*_tmp121B;union Cyc_YYSTYPE_union
_tmp121A;yyval=(union Cyc_YYSTYPE_union)(((_tmp121A.YY8).tag=12,(((_tmp121A.YY8).f1=((
_tmp121B=_cycalloc(sizeof(*_tmp121B)),((_tmp121B->v=(void*)((void*)3),_tmp121B)))),
_tmp121A))));}break;case 397: _LL39E:{struct Cyc_Core_Opt*_tmp121E;union Cyc_YYSTYPE_union
_tmp121D;yyval=(union Cyc_YYSTYPE_union)(((_tmp121D.YY8).tag=12,(((_tmp121D.YY8).f1=((
_tmp121E=_cycalloc(sizeof(*_tmp121E)),((_tmp121E->v=(void*)((void*)4),_tmp121E)))),
_tmp121D))));}break;case 398: _LL39F:{struct Cyc_Core_Opt*_tmp1221;union Cyc_YYSTYPE_union
_tmp1220;yyval=(union Cyc_YYSTYPE_union)(((_tmp1220.YY8).tag=12,(((_tmp1220.YY8).f1=((
_tmp1221=_cycalloc(sizeof(*_tmp1221)),((_tmp1221->v=(void*)((void*)0),_tmp1221)))),
_tmp1220))));}break;case 399: _LL3A0:{struct Cyc_Core_Opt*_tmp1224;union Cyc_YYSTYPE_union
_tmp1223;yyval=(union Cyc_YYSTYPE_union)(((_tmp1223.YY8).tag=12,(((_tmp1223.YY8).f1=((
_tmp1224=_cycalloc(sizeof(*_tmp1224)),((_tmp1224->v=(void*)((void*)2),_tmp1224)))),
_tmp1223))));}break;case 400: _LL3A1:{struct Cyc_Core_Opt*_tmp1227;union Cyc_YYSTYPE_union
_tmp1226;yyval=(union Cyc_YYSTYPE_union)(((_tmp1226.YY8).tag=12,(((_tmp1226.YY8).f1=((
_tmp1227=_cycalloc(sizeof(*_tmp1227)),((_tmp1227->v=(void*)((void*)16),_tmp1227)))),
_tmp1226))));}break;case 401: _LL3A2:{struct Cyc_Core_Opt*_tmp122A;union Cyc_YYSTYPE_union
_tmp1229;yyval=(union Cyc_YYSTYPE_union)(((_tmp1229.YY8).tag=12,(((_tmp1229.YY8).f1=((
_tmp122A=_cycalloc(sizeof(*_tmp122A)),((_tmp122A->v=(void*)((void*)17),_tmp122A)))),
_tmp1229))));}break;case 402: _LL3A3:{struct Cyc_Core_Opt*_tmp122D;union Cyc_YYSTYPE_union
_tmp122C;yyval=(union Cyc_YYSTYPE_union)(((_tmp122C.YY8).tag=12,(((_tmp122C.YY8).f1=((
_tmp122D=_cycalloc(sizeof(*_tmp122D)),((_tmp122D->v=(void*)((void*)13),_tmp122D)))),
_tmp122C))));}break;case 403: _LL3A4:{struct Cyc_Core_Opt*_tmp1230;union Cyc_YYSTYPE_union
_tmp122F;yyval=(union Cyc_YYSTYPE_union)(((_tmp122F.YY8).tag=12,(((_tmp122F.YY8).f1=((
_tmp1230=_cycalloc(sizeof(*_tmp1230)),((_tmp1230->v=(void*)((void*)15),_tmp1230)))),
_tmp122F))));}break;case 404: _LL3A5:{struct Cyc_Core_Opt*_tmp1233;union Cyc_YYSTYPE_union
_tmp1232;yyval=(union Cyc_YYSTYPE_union)(((_tmp1232.YY8).tag=12,(((_tmp1232.YY8).f1=((
_tmp1233=_cycalloc(sizeof(*_tmp1233)),((_tmp1233->v=(void*)((void*)14),_tmp1233)))),
_tmp1232))));}break;case 405: _LL3A6: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 406: _LL3A7:{union Cyc_YYSTYPE_union _tmp1234;yyval=(
union Cyc_YYSTYPE_union)(((_tmp1234.YY3).tag=7,(((_tmp1234.YY3).f1=Cyc_Absyn_conditional_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1234))));}
break;case 407: _LL3A8:{union Cyc_YYSTYPE_union _tmp1235;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1235.YY3).tag=7,(((_tmp1235.YY3).f1=Cyc_Absyn_throw_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1235))));}
break;case 408: _LL3A9:{union Cyc_YYSTYPE_union _tmp1236;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1236.YY3).tag=7,(((_tmp1236.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1236))));}
break;case 409: _LL3AA:{union Cyc_YYSTYPE_union _tmp1237;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1237.YY3).tag=7,(((_tmp1237.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1237))));}
break;case 410: _LL3AB:{union Cyc_YYSTYPE_union _tmp1238;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1238.YY3).tag=7,(((_tmp1238.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1238))));}
break;case 411: _LL3AC:{union Cyc_YYSTYPE_union _tmp1239;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1239.YY3).tag=7,(((_tmp1239.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1239))));}
break;case 412: _LL3AD: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 413: _LL3AE: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 414: _LL3AF:{union Cyc_YYSTYPE_union _tmp123A;yyval=(
union Cyc_YYSTYPE_union)(((_tmp123A.YY3).tag=7,(((_tmp123A.YY3).f1=Cyc_Absyn_or_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp123A))));}
break;case 415: _LL3B0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 416: _LL3B1:{union Cyc_YYSTYPE_union _tmp123B;yyval=(union
Cyc_YYSTYPE_union)(((_tmp123B.YY3).tag=7,(((_tmp123B.YY3).f1=Cyc_Absyn_and_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp123B))));}
break;case 417: _LL3B2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 418: _LL3B3:{union Cyc_YYSTYPE_union _tmp123C;yyval=(union
Cyc_YYSTYPE_union)(((_tmp123C.YY3).tag=7,(((_tmp123C.YY3).f1=Cyc_Absyn_prim2_exp((
void*)14,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp123C))));}
break;case 419: _LL3B4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 420: _LL3B5:{union Cyc_YYSTYPE_union _tmp123D;yyval=(union
Cyc_YYSTYPE_union)(((_tmp123D.YY3).tag=7,(((_tmp123D.YY3).f1=Cyc_Absyn_prim2_exp((
void*)15,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp123D))));}
break;case 421: _LL3B6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 422: _LL3B7:{union Cyc_YYSTYPE_union _tmp123E;yyval=(union
Cyc_YYSTYPE_union)(((_tmp123E.YY3).tag=7,(((_tmp123E.YY3).f1=Cyc_Absyn_prim2_exp((
void*)13,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp123E))));}
break;case 423: _LL3B8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 424: _LL3B9:{union Cyc_YYSTYPE_union _tmp123F;yyval=(union
Cyc_YYSTYPE_union)(((_tmp123F.YY3).tag=7,(((_tmp123F.YY3).f1=Cyc_Absyn_eq_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp123F))));}
break;case 425: _LL3BA:{union Cyc_YYSTYPE_union _tmp1240;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1240.YY3).tag=7,(((_tmp1240.YY3).f1=Cyc_Absyn_neq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1240))));}
break;case 426: _LL3BB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 427: _LL3BC:{union Cyc_YYSTYPE_union _tmp1241;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1241.YY3).tag=7,(((_tmp1241.YY3).f1=Cyc_Absyn_lt_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1241))));}
break;case 428: _LL3BD:{union Cyc_YYSTYPE_union _tmp1242;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1242.YY3).tag=7,(((_tmp1242.YY3).f1=Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1242))));}
break;case 429: _LL3BE:{union Cyc_YYSTYPE_union _tmp1243;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1243.YY3).tag=7,(((_tmp1243.YY3).f1=Cyc_Absyn_lte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1243))));}
break;case 430: _LL3BF:{union Cyc_YYSTYPE_union _tmp1244;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1244.YY3).tag=7,(((_tmp1244.YY3).f1=Cyc_Absyn_gte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1244))));}
break;case 431: _LL3C0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 432: _LL3C1:{union Cyc_YYSTYPE_union _tmp1245;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1245.YY3).tag=7,(((_tmp1245.YY3).f1=Cyc_Absyn_prim2_exp((
void*)16,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1245))));}
break;case 433: _LL3C2:{union Cyc_YYSTYPE_union _tmp1246;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1246.YY3).tag=7,(((_tmp1246.YY3).f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1246))));}
break;case 434: _LL3C3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 435: _LL3C4:{union Cyc_YYSTYPE_union _tmp1247;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1247.YY3).tag=7,(((_tmp1247.YY3).f1=Cyc_Absyn_prim2_exp((
void*)0,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1247))));}
break;case 436: _LL3C5:{union Cyc_YYSTYPE_union _tmp1248;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1248.YY3).tag=7,(((_tmp1248.YY3).f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1248))));}
break;case 437: _LL3C6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 438: _LL3C7:{union Cyc_YYSTYPE_union _tmp1249;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1249.YY3).tag=7,(((_tmp1249.YY3).f1=Cyc_Absyn_prim2_exp((
void*)1,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1249))));}
break;case 439: _LL3C8:{union Cyc_YYSTYPE_union _tmp124A;yyval=(union Cyc_YYSTYPE_union)(((
_tmp124A.YY3).tag=7,(((_tmp124A.YY3).f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp124A))));}
break;case 440: _LL3C9:{union Cyc_YYSTYPE_union _tmp124B;yyval=(union Cyc_YYSTYPE_union)(((
_tmp124B.YY3).tag=7,(((_tmp124B.YY3).f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp124B))));}
break;case 441: _LL3CA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 442: _LL3CB:{union Cyc_YYSTYPE_union _tmp124C;yyval=(union
Cyc_YYSTYPE_union)(((_tmp124C.YY3).tag=7,(((_tmp124C.YY3).f1=Cyc_Absyn_cast_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp124C))));}break;case 443: _LL3CC: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 444: _LL3CD:{union
Cyc_YYSTYPE_union _tmp124D;yyval=(union Cyc_YYSTYPE_union)(((_tmp124D.YY3).tag=7,(((
_tmp124D.YY3).f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp124D))));}
break;case 445: _LL3CE:{union Cyc_YYSTYPE_union _tmp124E;yyval=(union Cyc_YYSTYPE_union)(((
_tmp124E.YY3).tag=7,(((_tmp124E.YY3).f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp124E))));}
break;case 446: _LL3CF:{union Cyc_YYSTYPE_union _tmp124F;yyval=(union Cyc_YYSTYPE_union)(((
_tmp124F.YY3).tag=7,(((_tmp124F.YY3).f1=Cyc_Absyn_address_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp124F))));}
break;case 447: _LL3D0:{union Cyc_YYSTYPE_union _tmp1250;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1250.YY3).tag=7,(((_tmp1250.YY3).f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1250))));}
break;case 448: _LL3D1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 449: _LL3D2:{union Cyc_YYSTYPE_union _tmp1251;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1251.YY3).tag=7,(((_tmp1251.YY3).f1=Cyc_Absyn_prim1_exp(
Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1251))));}
break;case 450: _LL3D3:{union Cyc_YYSTYPE_union _tmp1252;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1252.YY3).tag=7,(((_tmp1252.YY3).f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1252))));}
break;case 451: _LL3D4:{union Cyc_YYSTYPE_union _tmp1253;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1253.YY3).tag=7,(((_tmp1253.YY3).f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1253))));}
break;case 452: _LL3D5:{struct Cyc_Absyn_StructField_struct*_tmp125D;struct
_dyneither_ptr*_tmp125C;struct Cyc_Absyn_StructField_struct _tmp125B;union Cyc_YYSTYPE_union
_tmp125A;yyval=(union Cyc_YYSTYPE_union)(((_tmp125A.YY3).tag=7,(((_tmp125A.YY3).f1=
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)((_tmp125D=_cycalloc(sizeof(*_tmp125D)),((
_tmp125D[0]=((_tmp125B.tag=0,((_tmp125B.f1=((_tmp125C=_cycalloc(sizeof(*_tmp125C)),((
_tmp125C[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmp125C)))),_tmp125B)))),_tmp125D)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp125A))));}
break;case 453: _LL3D6:{struct Cyc_Absyn_TupleIndex_struct*_tmp1263;struct Cyc_Absyn_TupleIndex_struct
_tmp1262;union Cyc_YYSTYPE_union _tmp1261;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1261.YY3).tag=7,(((_tmp1261.YY3).f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)((
_tmp1263=_cycalloc_atomic(sizeof(*_tmp1263)),((_tmp1263[0]=((_tmp1262.tag=1,((
_tmp1262.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2,_tmp1262)))),_tmp1263)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1261))));}
break;case 454: _LL3D7: {struct Cyc_Position_Segment*_tmp86A=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp86B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp86A,Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{
union Cyc_YYSTYPE_union _tmp1264;yyval=(union Cyc_YYSTYPE_union)(((_tmp1264.YY3).tag=
7,(((_tmp1264.YY3).f1=Cyc_Absyn_gentyp_exp(_tmp86B,(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1264))));}
break;}case 455: _LL3D8:{struct Cyc_Absyn_Malloc_e_struct*_tmp126E;struct Cyc_Absyn_MallocInfo
_tmp126D;struct Cyc_Absyn_Malloc_e_struct _tmp126C;union Cyc_YYSTYPE_union _tmp126B;
yyval=(union Cyc_YYSTYPE_union)(((_tmp126B.YY3).tag=7,(((_tmp126B.YY3).f1=Cyc_Absyn_new_exp((
void*)((_tmp126E=_cycalloc(sizeof(*_tmp126E)),((_tmp126E[0]=((_tmp126C.tag=35,((
_tmp126C.f1=((_tmp126D.is_calloc=0,((_tmp126D.rgn=0,((_tmp126D.elt_type=0,((
_tmp126D.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmp126D.fat_result=0,_tmp126D)))))))))),_tmp126C)))),
_tmp126E)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp126B))));}break;case 456: _LL3D9:{struct Cyc_Absyn_Malloc_e_struct*
_tmp1278;struct Cyc_Absyn_MallocInfo _tmp1277;struct Cyc_Absyn_Malloc_e_struct
_tmp1276;union Cyc_YYSTYPE_union _tmp1275;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1275.YY3).tag=7,(((_tmp1275.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp1278=
_cycalloc(sizeof(*_tmp1278)),((_tmp1278[0]=((_tmp1276.tag=35,((_tmp1276.f1=((
_tmp1277.is_calloc=0,((_tmp1277.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp1277.elt_type=0,((
_tmp1277.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmp1277.fat_result=0,_tmp1277)))))))))),_tmp1276)))),
_tmp1278)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1275))));}break;case 457: _LL3DA: {void*_tmp876;
struct _tuple2 _tmp875=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp876=_tmp875.f3;{struct Cyc_Absyn_Malloc_e_struct*_tmp1287;
struct Cyc_Absyn_MallocInfo _tmp1286;void**_tmp1285;struct Cyc_Absyn_Malloc_e_struct
_tmp1284;union Cyc_YYSTYPE_union _tmp1283;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1283.YY3).tag=7,(((_tmp1283.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp1287=
_cycalloc(sizeof(*_tmp1287)),((_tmp1287[0]=((_tmp1284.tag=35,((_tmp1284.f1=((
_tmp1286.is_calloc=1,((_tmp1286.rgn=0,((_tmp1286.elt_type=((_tmp1285=_cycalloc(
sizeof(*_tmp1285)),((_tmp1285[0]=_tmp876,_tmp1285)))),((_tmp1286.num_elts=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((_tmp1286.fat_result=
0,_tmp1286)))))))))),_tmp1284)))),_tmp1287)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1283))));}
break;}case 458: _LL3DB: {void*_tmp87D;struct _tuple2 _tmp87C=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp87D=_tmp87C.f3;{
struct Cyc_Absyn_Malloc_e_struct*_tmp1296;struct Cyc_Absyn_MallocInfo _tmp1295;void**
_tmp1294;struct Cyc_Absyn_Malloc_e_struct _tmp1293;union Cyc_YYSTYPE_union _tmp1292;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1292.YY3).tag=7,(((_tmp1292.YY3).f1=Cyc_Absyn_new_exp((
void*)((_tmp1296=_cycalloc(sizeof(*_tmp1296)),((_tmp1296[0]=((_tmp1293.tag=35,((
_tmp1293.f1=((_tmp1295.is_calloc=1,((_tmp1295.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 8)]),((_tmp1295.elt_type=((
_tmp1294=_cycalloc(sizeof(*_tmp1294)),((_tmp1294[0]=_tmp87D,_tmp1294)))),((
_tmp1295.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 6)]),((_tmp1295.fat_result=0,_tmp1295)))))))))),_tmp1293)))),
_tmp1296)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 10)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1292))));}break;}case 459: _LL3DC:{struct Cyc_Absyn_Exp*
_tmp1299[1];union Cyc_YYSTYPE_union _tmp1298;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1298.YY3).tag=7,(((_tmp1298.YY3).f1=Cyc_Absyn_primop_exp((void*)19,((
_tmp1299[0]=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp1299,sizeof(struct Cyc_Absyn_Exp*),1)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1298))));}
break;case 460: _LL3DD: {void*_tmp886;struct _tuple2 _tmp885=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp886=_tmp885.f3;{
union Cyc_YYSTYPE_union _tmp129A;yyval=(union Cyc_YYSTYPE_union)(((_tmp129A.YY3).tag=
7,(((_tmp129A.YY3).f1=Cyc_Absyn_valueof_exp(_tmp886,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp129A))));}
break;}case 461: _LL3DE:{union Cyc_YYSTYPE_union _tmp129B;yyval=(union Cyc_YYSTYPE_union)(((
_tmp129B.YY7).tag=11,(((_tmp129B.YY7).f1=(void*)((void*)12),_tmp129B))));}break;
case 462: _LL3DF:{union Cyc_YYSTYPE_union _tmp129C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp129C.YY7).tag=11,(((_tmp129C.YY7).f1=(void*)((void*)11),_tmp129C))));}break;
case 463: _LL3E0:{union Cyc_YYSTYPE_union _tmp129D;yyval=(union Cyc_YYSTYPE_union)(((
_tmp129D.YY7).tag=11,(((_tmp129D.YY7).f1=(void*)((void*)2),_tmp129D))));}break;
case 464: _LL3E1: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 465: _LL3E2:{union Cyc_YYSTYPE_union _tmp129E;yyval=(union Cyc_YYSTYPE_union)(((
_tmp129E.YY3).tag=7,(((_tmp129E.YY3).f1=Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp129E))));}
break;case 466: _LL3E3:{union Cyc_YYSTYPE_union _tmp129F;yyval=(union Cyc_YYSTYPE_union)(((
_tmp129F.YY3).tag=7,(((_tmp129F.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp129F))));}
break;case 467: _LL3E4:{union Cyc_YYSTYPE_union _tmp12A0;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12A0.YY3).tag=7,(((_tmp12A0.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A0))));}
break;case 468: _LL3E5:{struct _dyneither_ptr*_tmp12A3;union Cyc_YYSTYPE_union
_tmp12A2;yyval=(union Cyc_YYSTYPE_union)(((_tmp12A2.YY3).tag=7,(((_tmp12A2.YY3).f1=
Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmp12A3=_cycalloc(sizeof(*_tmp12A3)),((_tmp12A3[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp12A3)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A2))));}
break;case 469: _LL3E6:{struct _dyneither_ptr*_tmp12A6;union Cyc_YYSTYPE_union
_tmp12A5;yyval=(union Cyc_YYSTYPE_union)(((_tmp12A5.YY3).tag=7,(((_tmp12A5.YY3).f1=
Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmp12A6=_cycalloc(sizeof(*_tmp12A6)),((_tmp12A6[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp12A6)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A5))));}
break;case 470: _LL3E7:{union Cyc_YYSTYPE_union _tmp12A7;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12A7.YY3).tag=7,(((_tmp12A7.YY3).f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A7))));}
break;case 471: _LL3E8:{union Cyc_YYSTYPE_union _tmp12A8;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12A8.YY3).tag=7,(((_tmp12A8.YY3).f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A8))));}
break;case 472: _LL3E9:{struct Cyc_Absyn_CompoundLit_e_struct*_tmp12AE;struct Cyc_Absyn_CompoundLit_e_struct
_tmp12AD;union Cyc_YYSTYPE_union _tmp12AC;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12AC.YY3).tag=7,(((_tmp12AC.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp12AE=
_cycalloc(sizeof(*_tmp12AE)),((_tmp12AE[0]=((_tmp12AD.tag=27,((_tmp12AD.f1=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmp12AD.f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),_tmp12AD)))))),
_tmp12AE)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp12AC))));}break;case 473: _LL3EA:{struct Cyc_Absyn_CompoundLit_e_struct*
_tmp12B4;struct Cyc_Absyn_CompoundLit_e_struct _tmp12B3;union Cyc_YYSTYPE_union
_tmp12B2;yyval=(union Cyc_YYSTYPE_union)(((_tmp12B2.YY3).tag=7,(((_tmp12B2.YY3).f1=
Cyc_Absyn_new_exp((void*)((_tmp12B4=_cycalloc(sizeof(*_tmp12B4)),((_tmp12B4[0]=((
_tmp12B3.tag=27,((_tmp12B3.f1=Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]),((_tmp12B3.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),_tmp12B3)))))),_tmp12B4)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12B2))));}
break;case 474: _LL3EB:{struct Cyc_Absyn_UnknownId_e_struct*_tmp12BA;struct Cyc_Absyn_UnknownId_e_struct
_tmp12B9;union Cyc_YYSTYPE_union _tmp12B8;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12B8.YY3).tag=7,(((_tmp12B8.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp12BA=
_cycalloc(sizeof(*_tmp12BA)),((_tmp12BA[0]=((_tmp12B9.tag=2,((_tmp12B9.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp12B9)))),_tmp12BA)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp12B8))));}break;case 475: _LL3EC: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 476: _LL3ED:{union
Cyc_YYSTYPE_union _tmp12BB;yyval=(union Cyc_YYSTYPE_union)(((_tmp12BB.YY3).tag=7,(((
_tmp12BB.YY3).f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12BB))));}
break;case 477: _LL3EE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 478: _LL3EF:{union Cyc_YYSTYPE_union _tmp12BC;yyval=(
union Cyc_YYSTYPE_union)(((_tmp12BC.YY3).tag=7,(((_tmp12BC.YY3).f1=Cyc_Absyn_noinstantiate_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12BC))));}
break;case 479: _LL3F0:{union Cyc_YYSTYPE_union _tmp12BD;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12BD.YY3).tag=7,(((_tmp12BD.YY3).f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12BD))));}
break;case 480: _LL3F1:{union Cyc_YYSTYPE_union _tmp12BE;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12BE.YY3).tag=7,(((_tmp12BE.YY3).f1=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12BE))));}
break;case 481: _LL3F2:{struct Cyc_Absyn_Struct_e_struct*_tmp12C4;struct Cyc_Absyn_Struct_e_struct
_tmp12C3;union Cyc_YYSTYPE_union _tmp12C2;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12C2.YY3).tag=7,(((_tmp12C2.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp12C4=
_cycalloc(sizeof(*_tmp12C4)),((_tmp12C4[0]=((_tmp12C3.tag=30,((_tmp12C3.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmp12C3.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp12C3.f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),((_tmp12C3.f4=0,
_tmp12C3)))))))))),_tmp12C4)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12C2))));}
break;case 482: _LL3F3:{union Cyc_YYSTYPE_union _tmp12C5;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12C5.YY3).tag=7,(((_tmp12C5.YY3).f1=Cyc_Absyn_stmt_exp(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12C5))));}
break;case 483: _LL3F4:{union Cyc_YYSTYPE_union _tmp12C6;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12C6.YY5).tag=9,(((_tmp12C6.YY5).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])),_tmp12C6))));}break;case 484: _LL3F5:{struct Cyc_List_List*
_tmp12C9;union Cyc_YYSTYPE_union _tmp12C8;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12C8.YY5).tag=9,(((_tmp12C8.YY5).f1=((_tmp12C9=_cycalloc(sizeof(*_tmp12C9)),((
_tmp12C9->hd=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmp12C9->tl=0,_tmp12C9)))))),_tmp12C8))));}break;case 485:
_LL3F6:{struct Cyc_List_List*_tmp12CC;union Cyc_YYSTYPE_union _tmp12CB;yyval=(union
Cyc_YYSTYPE_union)(((_tmp12CB.YY5).tag=9,(((_tmp12CB.YY5).f1=((_tmp12CC=
_cycalloc(sizeof(*_tmp12CC)),((_tmp12CC->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp12CC->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp12CC)))))),
_tmp12CB))));}break;case 486: _LL3F7:{union Cyc_YYSTYPE_union _tmp12CD;yyval=(union
Cyc_YYSTYPE_union)(((_tmp12CD.YY3).tag=7,(((_tmp12CD.YY3).f1=Cyc_Absyn_int_exp((
Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(
Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp12CD))));}break;case 487: _LL3F8:{union Cyc_YYSTYPE_union
_tmp12CE;yyval=(union Cyc_YYSTYPE_union)(((_tmp12CE.YY3).tag=7,(((_tmp12CE.YY3).f1=
Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp12CE))));}break;case 488: _LL3F9:{union Cyc_YYSTYPE_union
_tmp12CF;yyval=(union Cyc_YYSTYPE_union)(((_tmp12CF.YY3).tag=7,(((_tmp12CF.YY3).f1=
Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12CF))));}
break;case 489: _LL3FA:{union Cyc_YYSTYPE_union _tmp12D0;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12D0.YY3).tag=7,(((_tmp12D0.YY3).f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12D0))));}
break;case 490: _LL3FB:{struct _tuple1*_tmp12D9;struct _dyneither_ptr*_tmp12D8;union
Cyc_Absyn_Nmspace_union _tmp12D7;union Cyc_YYSTYPE_union _tmp12D6;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12D6.QualId_tok).tag=4,(((_tmp12D6.QualId_tok).f1=((_tmp12D9=_cycalloc(
sizeof(*_tmp12D9)),((_tmp12D9->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp12D7.Rel_n).tag=
1,(((_tmp12D7.Rel_n).f1=0,_tmp12D7)))),((_tmp12D9->f2=((_tmp12D8=_cycalloc(
sizeof(*_tmp12D8)),((_tmp12D8[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp12D8)))),_tmp12D9)))))),
_tmp12D6))));}break;case 491: _LL3FC: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 492: _LL3FD:{struct _tuple1*_tmp12E2;struct
_dyneither_ptr*_tmp12E1;union Cyc_Absyn_Nmspace_union _tmp12E0;union Cyc_YYSTYPE_union
_tmp12DF;yyval=(union Cyc_YYSTYPE_union)(((_tmp12DF.QualId_tok).tag=4,(((_tmp12DF.QualId_tok).f1=((
_tmp12E2=_cycalloc(sizeof(*_tmp12E2)),((_tmp12E2->f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp12E0.Rel_n).tag=1,(((_tmp12E0.Rel_n).f1=0,_tmp12E0)))),((_tmp12E2->f2=((
_tmp12E1=_cycalloc(sizeof(*_tmp12E1)),((_tmp12E1[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp12E1)))),_tmp12E2)))))),
_tmp12DF))));}break;case 493: _LL3FE: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 494: _LL3FF: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 495: _LL400: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 496: _LL401:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 497:
_LL402: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
498: _LL403: break;case 499: _LL404: yylex_buf->lex_curr_pos -=1;break;default: _LL405:
break;}yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=yyval;++ yylsp_offset;if(
yylen == 0){(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line=
Cyc_yylloc.first_line;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_column=
Cyc_yylloc.first_column;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_column;}else{(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_column;}
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(500,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(142,yyn - 134)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
6033) && Cyc_yycheck[_check_known_subscript_notnull(6034,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(6034,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(142,yyn - 134)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1006,yystate)];if(yyn > - 32768  && yyn < 6033){int
sze=0;struct _dyneither_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 276 / 
sizeof(char*);++ x){if(Cyc_yycheck[_check_known_subscript_notnull(6034,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(276,x)])+ 15,count ++);}{unsigned int _tmp12E7;
unsigned int _tmp12E6;struct _dyneither_ptr _tmp12E5;char*_tmp12E4;unsigned int
_tmp12E3;msg=((_tmp12E3=(unsigned int)(sze + 15),((_tmp12E4=(char*)_region_malloc(
yyregion,_check_times(sizeof(char),_tmp12E3 + 1)),((_tmp12E5=_tag_dyneither(
_tmp12E4,sizeof(char),_tmp12E3 + 1),((((_tmp12E6=_tmp12E3,((_tmp12E8(& _tmp12E7,&
_tmp12E6,& _tmp12E4),_tmp12E4[_tmp12E6]=(char)0)))),_tmp12E5))))))));}{const char*
_tmp12E9;Cyc_strcpy(msg,((_tmp12E9="parse error",_tag_dyneither(_tmp12E9,sizeof(
char),12))));}if(count < 5){count=0;for(x=yyn < 0?- yyn: 0;x < 276 / sizeof(char*);++ x){
if(Cyc_yycheck[_check_known_subscript_notnull(6034,x + yyn)]== x){{const char*
_tmp12EB;const char*_tmp12EA;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(
struct _dyneither_ptr)((_tmp12EB=", expecting `",_tag_dyneither(_tmp12EB,sizeof(
char),14))):(struct _dyneither_ptr)((_tmp12EA=" or `",_tag_dyneither(_tmp12EA,
sizeof(char),6)))));}Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(276,x)]);{const char*_tmp12EC;Cyc_strcat(msg,((
_tmp12EC="'",_tag_dyneither(_tmp12EC,sizeof(char),2))));}++ count;}}}Cyc_yyerror((
struct _dyneither_ptr)msg);}else{const char*_tmp12ED;Cyc_yyerror(((_tmp12ED="parse error",
_tag_dyneither(_tmp12ED,sizeof(char),12))));}}goto yyerrlab1;yyerrlab1: if(
yyerrstatus == 3){if(Cyc_yychar == 0){int _tmp8BF=1;_npop_handler(0);return _tmp8BF;}
Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(
yyssp_offset == 0){int _tmp8C0=1;_npop_handler(0);return _tmp8C0;}-- yyvsp_offset;
yystate=(int)yyss[_check_known_subscript_notnull(10000,-- yyssp_offset)];--
yylsp_offset;yyerrhandle: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1006,
yystate)];if(yyn == - 32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6033) || 
Cyc_yycheck[_check_known_subscript_notnull(6034,yyn)]!= 1)goto yyerrdefault;yyn=(
int)Cyc_yytable[_check_known_subscript_notnull(6034,yyn)];if(yyn < 0){if(yyn == -
32768)goto yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 
1005){int _tmp8C1=0;_npop_handler(0);return _tmp8C1;}yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto yynewstate;};
_pop_region(yyregion);}void Cyc_yyprint(int i,union Cyc_YYSTYPE_union v);void Cyc_yyprint(
int i,union Cyc_YYSTYPE_union v){union Cyc_YYSTYPE_union _tmp8CB=v;struct _tuple11
_tmp8CC;int _tmp8CD;char _tmp8CE;struct _dyneither_ptr _tmp8CF;struct _tuple1*_tmp8D0;
struct _tuple1 _tmp8D1;union Cyc_Absyn_Nmspace_union _tmp8D2;struct _dyneither_ptr*
_tmp8D3;_LL408: if((_tmp8CB.Int_tok).tag != 0)goto _LL40A;_tmp8CC=(_tmp8CB.Int_tok).f1;
_tmp8CD=_tmp8CC.f2;_LL409:{const char*_tmp12F1;void*_tmp12F0[1];struct Cyc_Int_pa_struct
_tmp12EF;(_tmp12EF.tag=1,((_tmp12EF.f1=(unsigned long)_tmp8CD,((_tmp12F0[0]=&
_tmp12EF,Cyc_fprintf(Cyc_stderr,((_tmp12F1="%d",_tag_dyneither(_tmp12F1,sizeof(
char),3))),_tag_dyneither(_tmp12F0,sizeof(void*),1)))))));}goto _LL407;_LL40A: if((
_tmp8CB.Char_tok).tag != 1)goto _LL40C;_tmp8CE=(_tmp8CB.Char_tok).f1;_LL40B:{const
char*_tmp12F5;void*_tmp12F4[1];struct Cyc_Int_pa_struct _tmp12F3;(_tmp12F3.tag=1,((
_tmp12F3.f1=(unsigned long)((int)_tmp8CE),((_tmp12F4[0]=& _tmp12F3,Cyc_fprintf(
Cyc_stderr,((_tmp12F5="%c",_tag_dyneither(_tmp12F5,sizeof(char),3))),
_tag_dyneither(_tmp12F4,sizeof(void*),1)))))));}goto _LL407;_LL40C: if((_tmp8CB.String_tok).tag
!= 2)goto _LL40E;_tmp8CF=(_tmp8CB.String_tok).f1;_LL40D:{const char*_tmp12F9;void*
_tmp12F8[1];struct Cyc_String_pa_struct _tmp12F7;(_tmp12F7.tag=0,((_tmp12F7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp8CF),((_tmp12F8[0]=& _tmp12F7,Cyc_fprintf(
Cyc_stderr,((_tmp12F9="\"%s\"",_tag_dyneither(_tmp12F9,sizeof(char),5))),
_tag_dyneither(_tmp12F8,sizeof(void*),1)))))));}goto _LL407;_LL40E: if((_tmp8CB.QualId_tok).tag
!= 4)goto _LL410;_tmp8D0=(_tmp8CB.QualId_tok).f1;_tmp8D1=*_tmp8D0;_tmp8D2=_tmp8D1.f1;
_tmp8D3=_tmp8D1.f2;_LL40F: {struct Cyc_List_List*_tmp8DD=0;{union Cyc_Absyn_Nmspace_union
_tmp8DE=_tmp8D2;struct Cyc_List_List*_tmp8DF;struct Cyc_List_List*_tmp8E0;_LL413:
if((_tmp8DE.Rel_n).tag != 1)goto _LL415;_tmp8DF=(_tmp8DE.Rel_n).f1;_LL414: _tmp8DD=
_tmp8DF;goto _LL412;_LL415: if((_tmp8DE.Abs_n).tag != 2)goto _LL417;_tmp8E0=(_tmp8DE.Abs_n).f1;
_LL416: _tmp8DD=_tmp8E0;goto _LL412;_LL417: if((_tmp8DE.Loc_n).tag != 0)goto _LL412;
_LL418: goto _LL412;_LL412:;}for(0;_tmp8DD != 0;_tmp8DD=_tmp8DD->tl){const char*
_tmp12FD;void*_tmp12FC[1];struct Cyc_String_pa_struct _tmp12FB;(_tmp12FB.tag=0,((
_tmp12FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)
_tmp8DD->hd)),((_tmp12FC[0]=& _tmp12FB,Cyc_fprintf(Cyc_stderr,((_tmp12FD="%s::",
_tag_dyneither(_tmp12FD,sizeof(char),5))),_tag_dyneither(_tmp12FC,sizeof(void*),
1)))))));}{const char*_tmp1301;void*_tmp1300[1];struct Cyc_String_pa_struct
_tmp12FF;(_tmp12FF.tag=0,((_tmp12FF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp8D3),((_tmp1300[0]=& _tmp12FF,Cyc_fprintf(Cyc_stderr,((
_tmp1301="%s::",_tag_dyneither(_tmp1301,sizeof(char),5))),_tag_dyneither(
_tmp1300,sizeof(void*),1)))))));}goto _LL407;}_LL410:;_LL411:{const char*_tmp1304;
void*_tmp1303;(_tmp1303=0,Cyc_fprintf(Cyc_stderr,((_tmp1304="?",_tag_dyneither(
_tmp1304,sizeof(char),2))),_tag_dyneither(_tmp1303,sizeof(void*),0)));}goto
_LL407;_LL407:;}struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){Cyc_Parse_parse_result=
0;Cyc_yyparse(Cyc_Lexing_from_file(f));return Cyc_Parse_parse_result;}
