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
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*
Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct
Cyc_List_List*fs);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct
_tuple1*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(
void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int
is_xtunion,int is_flat,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
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
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){{const char*_tmp8F4;void*
_tmp8F3[2];struct Cyc_String_pa_struct _tmp8F2;struct Cyc_String_pa_struct _tmp8F1;(
_tmp8F1.tag=0,((_tmp8F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((
_tmp8F2.tag=0,((_tmp8F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
sg)),((_tmp8F3[0]=& _tmp8F2,((_tmp8F3[1]=& _tmp8F1,Cyc_fprintf(Cyc_stderr,((
_tmp8F4="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp8F4,
sizeof(char),46))),_tag_dyneither(_tmp8F3,sizeof(void*),2)))))))))))));}return;}
static int Cyc_Parse_enum_counter=0;struct _tuple1*Cyc_Parse_gensym_enum();struct
_tuple1*Cyc_Parse_gensym_enum(){union Cyc_Absyn_Nmspace_union _tmp903;struct
_dyneither_ptr*_tmp902;const char*_tmp901;void*_tmp900[1];struct Cyc_Int_pa_struct
_tmp8FF;struct _tuple1*_tmp8FE;return(_tmp8FE=_cycalloc(sizeof(*_tmp8FE)),((
_tmp8FE->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp903.Rel_n).tag=1,(((_tmp903.Rel_n).f1=
0,_tmp903)))),((_tmp8FE->f2=((_tmp902=_cycalloc(sizeof(*_tmp902)),((_tmp902[0]=(
struct _dyneither_ptr)((_tmp8FF.tag=1,((_tmp8FF.f1=(unsigned long)Cyc_Parse_enum_counter
++,((_tmp900[0]=& _tmp8FF,Cyc_aprintf(((_tmp901="__anonymous_enum_%d__",
_tag_dyneither(_tmp901,sizeof(char),22))),_tag_dyneither(_tmp900,sizeof(void*),1)))))))),
_tmp902)))),_tmp8FE)))));}struct _tuple7{struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;
void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple8{struct
_tuple7*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
struct Cyc_Position_Segment*loc,struct _tuple8*field_info);static struct Cyc_Absyn_Aggrfield*
Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*loc,struct _tuple8*
field_info){struct _tuple8 _tmpB;struct _tuple7*_tmpC;struct _tuple7 _tmpD;struct
_tuple1*_tmpE;struct Cyc_Absyn_Tqual _tmpF;void*_tmp10;struct Cyc_List_List*_tmp11;
struct Cyc_List_List*_tmp12;struct Cyc_Absyn_Exp*_tmp13;struct _tuple8*_tmpA=
field_info;_tmpB=*_tmpA;_tmpC=_tmpB.f1;_tmpD=*_tmpC;_tmpE=_tmpD.f1;_tmpF=_tmpD.f2;
_tmp10=_tmpD.f3;_tmp11=_tmpD.f4;_tmp12=_tmpD.f5;_tmp13=_tmpB.f2;if(_tmp11 != 0){
const char*_tmp904;Cyc_Parse_err(((_tmp904="bad type params in struct field",
_tag_dyneither(_tmp904,sizeof(char),32))),loc);}if(Cyc_Absyn_is_qvar_qualified(
_tmpE)){const char*_tmp905;Cyc_Parse_err(((_tmp905="struct or union field cannot be qualified with a namespace",
_tag_dyneither(_tmp905,sizeof(char),59))),loc);}{struct Cyc_Absyn_Aggrfield*
_tmp906;return(_tmp906=_cycalloc(sizeof(*_tmp906)),((_tmp906->name=(*_tmpE).f2,((
_tmp906->tq=_tmpF,((_tmp906->type=(void*)_tmp10,((_tmp906->width=_tmp13,((
_tmp906->attributes=_tmp12,_tmp906)))))))))));}}static void*Cyc_Parse_type_spec(
void*t,struct Cyc_Position_Segment*loc);static void*Cyc_Parse_type_spec(void*t,
struct Cyc_Position_Segment*loc){struct Cyc_Parse_Type_spec_struct _tmp909;struct
Cyc_Parse_Type_spec_struct*_tmp908;return(void*)((_tmp908=_cycalloc(sizeof(*
_tmp908)),((_tmp908[0]=((_tmp909.tag=4,((_tmp909.f1=(void*)t,((_tmp909.f2=loc,
_tmp909)))))),_tmp908))));}static void*Cyc_Parse_array2ptr(void*t,int argposn);
static void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp19=t;struct Cyc_Absyn_ArrayInfo
_tmp1A;void*_tmp1B;struct Cyc_Absyn_Tqual _tmp1C;struct Cyc_Absyn_Exp*_tmp1D;struct
Cyc_Absyn_Conref*_tmp1E;struct Cyc_Position_Segment*_tmp1F;_LL1: if(_tmp19 <= (void*)
4)goto _LL3;if(*((int*)_tmp19)!= 7)goto _LL3;_tmp1A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp19)->f1;_tmp1B=(void*)_tmp1A.elt_type;_tmp1C=_tmp1A.tq;_tmp1D=_tmp1A.num_elts;
_tmp1E=_tmp1A.zero_term;_tmp1F=_tmp1A.zt_loc;_LL2: {struct Cyc_Absyn_Upper_b_struct
_tmp90C;struct Cyc_Absyn_Upper_b_struct*_tmp90B;return Cyc_Absyn_starb_typ(_tmp1B,
argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0):(void*)2,
_tmp1C,_tmp1D == 0?(void*)0:(void*)((_tmp90B=_cycalloc(sizeof(*_tmp90B)),((
_tmp90B[0]=((_tmp90C.tag=0,((_tmp90C.f1=(struct Cyc_Absyn_Exp*)_tmp1D,_tmp90C)))),
_tmp90B)))),_tmp1E);}_LL3:;_LL4: return t;_LL0:;}static void Cyc_Parse_arg_array2ptr(
struct _tuple2*x);static void Cyc_Parse_arg_array2ptr(struct _tuple2*x){(*x).f3=Cyc_Parse_array2ptr((*
x).f3,1);}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple9*
Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct _tuple2*t);static
struct _tuple9*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct
_tuple2*t){struct _tuple9*_tmp90D;return(_tmp90D=_cycalloc(sizeof(*_tmp90D)),((
_tmp90D->f1=(*t).f2,((_tmp90D->f2=(*t).f3,_tmp90D)))));}static struct Cyc_Absyn_Vardecl*
Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d);static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(
struct Cyc_Absyn_Decl*d){void*_tmp23=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp24;
_LL6: if(_tmp23 <= (void*)2)goto _LL8;if(*((int*)_tmp23)!= 0)goto _LL8;_tmp24=((
struct Cyc_Absyn_Var_d_struct*)_tmp23)->f1;_LL7: return _tmp24;_LL8:;_LL9: {const
char*_tmp910;void*_tmp90F;(_tmp90F=0,((int(*)(struct Cyc_Position_Segment*sg,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(d->loc,((
_tmp910="bad declaration in `forarray' statement",_tag_dyneither(_tmp910,sizeof(
char),40))),_tag_dyneither(_tmp90F,sizeof(void*),0)));}_LL5:;}static int Cyc_Parse_is_typeparam(
void*tm);static int Cyc_Parse_is_typeparam(void*tm){void*_tmp27=tm;_LLB: if(*((int*)
_tmp27)!= 4)goto _LLD;_LLC: return 1;_LLD:;_LLE: return 0;_LLA:;}static void*Cyc_Parse_id2type(
struct _dyneither_ptr s,void*k);static void*Cyc_Parse_id2type(struct _dyneither_ptr s,
void*k){const char*_tmp911;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp911="`H",
_tag_dyneither(_tmp911,sizeof(char),3))))== 0)return(void*)2;else{const char*
_tmp912;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp912="`U",_tag_dyneither(
_tmp912,sizeof(char),3))))== 0)return(void*)3;else{struct Cyc_Absyn_VarType_struct
_tmp91C;struct _dyneither_ptr*_tmp91B;struct Cyc_Absyn_Tvar*_tmp91A;struct Cyc_Absyn_VarType_struct*
_tmp919;return(void*)((_tmp919=_cycalloc(sizeof(*_tmp919)),((_tmp919[0]=((
_tmp91C.tag=1,((_tmp91C.f1=((_tmp91A=_cycalloc(sizeof(*_tmp91A)),((_tmp91A->name=((
_tmp91B=_cycalloc(sizeof(*_tmp91B)),((_tmp91B[0]=s,_tmp91B)))),((_tmp91A->identity=
- 1,((_tmp91A->kind=(void*)k,_tmp91A)))))))),_tmp91C)))),_tmp919))));}}}static
struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t);static struct Cyc_Absyn_Tvar*
Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){void*k;{void*_tmp2E=Cyc_Absyn_compress_kb((
void*)t->kind);void*_tmp2F;_LL10: if(*((int*)_tmp2E)!= 1)goto _LL12;_LL11:{struct
Cyc_Absyn_Unknown_kb_struct _tmp91F;struct Cyc_Absyn_Unknown_kb_struct*_tmp91E;k=(
void*)((_tmp91E=_cycalloc(sizeof(*_tmp91E)),((_tmp91E[0]=((_tmp91F.tag=1,((
_tmp91F.f1=0,_tmp91F)))),_tmp91E))));}goto _LLF;_LL12: if(*((int*)_tmp2E)!= 2)goto
_LL14;_tmp2F=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp2E)->f2;_LL13:{struct
Cyc_Absyn_Less_kb_struct _tmp922;struct Cyc_Absyn_Less_kb_struct*_tmp921;k=(void*)((
_tmp921=_cycalloc(sizeof(*_tmp921)),((_tmp921[0]=((_tmp922.tag=2,((_tmp922.f1=0,((
_tmp922.f2=(void*)_tmp2F,_tmp922)))))),_tmp921))));}goto _LLF;_LL14:;_LL15: k=
_tmp2E;goto _LLF;_LLF:;}{struct Cyc_Absyn_Tvar*_tmp923;return(_tmp923=_cycalloc(
sizeof(*_tmp923)),((_tmp923->name=t->name,((_tmp923->identity=- 1,((_tmp923->kind=(
void*)k,_tmp923)))))));}}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t);static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t){void*_tmp35=t;struct Cyc_Absyn_Tvar*_tmp36;_LL17: if(_tmp35 <= (void*)4)
goto _LL19;if(*((int*)_tmp35)!= 1)goto _LL19;_tmp36=((struct Cyc_Absyn_VarType_struct*)
_tmp35)->f1;_LL18: return _tmp36;_LL19:;_LL1A: {const char*_tmp926;void*_tmp925;(
_tmp925=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp926="expecting a list of type variables, not types",
_tag_dyneither(_tmp926,sizeof(char),46))),_tag_dyneither(_tmp925,sizeof(void*),0)));}
_LL16:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr);static void*Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar*pr){struct Cyc_Absyn_VarType_struct _tmp929;struct Cyc_Absyn_VarType_struct*
_tmp928;return(void*)((_tmp928=_cycalloc(sizeof(*_tmp928)),((_tmp928[0]=((
_tmp929.tag=1,((_tmp929.f1=pr,_tmp929)))),_tmp928))));}static void Cyc_Parse_set_vartyp_kind(
void*t,void*k,int leq);static void Cyc_Parse_set_vartyp_kind(void*t,void*k,int leq){
void*_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp3C;struct Cyc_Absyn_Tvar
_tmp3D;void*_tmp3E;void**_tmp3F;_LL1C: if(_tmp3B <= (void*)4)goto _LL1E;if(*((int*)
_tmp3B)!= 1)goto _LL1E;_tmp3C=((struct Cyc_Absyn_VarType_struct*)_tmp3B)->f1;
_tmp3D=*_tmp3C;_tmp3E=(void*)_tmp3D.kind;_tmp3F=(void**)&(*((struct Cyc_Absyn_VarType_struct*)
_tmp3B)->f1).kind;_LL1D: {void*_tmp40=Cyc_Absyn_compress_kb(*_tmp3F);_LL21: if(*((
int*)_tmp40)!= 1)goto _LL23;_LL22: if(!leq)*_tmp3F=Cyc_Tcutil_kind_to_bound(k);
else{struct Cyc_Absyn_Less_kb_struct _tmp92C;struct Cyc_Absyn_Less_kb_struct*
_tmp92B;*_tmp3F=(void*)((_tmp92B=_cycalloc(sizeof(*_tmp92B)),((_tmp92B[0]=((
_tmp92C.tag=2,((_tmp92C.f1=0,((_tmp92C.f2=(void*)k,_tmp92C)))))),_tmp92B))));}
return;_LL23:;_LL24: return;_LL20:;}_LL1E:;_LL1F: return;_LL1B:;}static struct Cyc_List_List*
Cyc_Parse_oldstyle2newstyle(struct Cyc_List_List*tms,struct Cyc_List_List*tds,
struct Cyc_Position_Segment*loc);static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp43=(void*)tms->hd;void*_tmp44;
_LL26: if(*((int*)_tmp43)!= 3)goto _LL28;_tmp44=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp43)->f1;_LL27: if(tms->tl == 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 
0){void*_tmp45=_tmp44;struct Cyc_List_List*_tmp46;_LL2B: if(*((int*)_tmp45)!= 1)
goto _LL2D;_LL2C:{const char*_tmp92F;void*_tmp92E;(_tmp92E=0,Cyc_Tcutil_warn(loc,((
_tmp92F="function declaration with both new- and old-style parameter declarations; ignoring old-style",
_tag_dyneither(_tmp92F,sizeof(char),93))),_tag_dyneither(_tmp92E,sizeof(void*),0)));}
return tms;_LL2D: if(*((int*)_tmp45)!= 0)goto _LL2A;_tmp46=((struct Cyc_Absyn_NoTypes_struct*)
_tmp45)->f1;_LL2E: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp46)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){const char*_tmp932;void*
_tmp931;(_tmp931=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp932="wrong number of parameter declarations in old-style function declaration",
_tag_dyneither(_tmp932,sizeof(char),73))),_tag_dyneither(_tmp931,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp4B=0;for(0;_tmp46 != 0;_tmp46=_tmp46->tl){struct Cyc_List_List*
_tmp4C=tds;for(0;_tmp4C != 0;_tmp4C=_tmp4C->tl){struct Cyc_Absyn_Decl*_tmp4D=(
struct Cyc_Absyn_Decl*)_tmp4C->hd;void*_tmp4E=(void*)_tmp4D->r;struct Cyc_Absyn_Vardecl*
_tmp4F;_LL30: if(_tmp4E <= (void*)2)goto _LL32;if(*((int*)_tmp4E)!= 0)goto _LL32;
_tmp4F=((struct Cyc_Absyn_Var_d_struct*)_tmp4E)->f1;_LL31: if(Cyc_zstrptrcmp((*
_tmp4F->name).f2,(struct _dyneither_ptr*)_tmp46->hd)!= 0)continue;if(_tmp4F->initializer
!= 0){const char*_tmp935;void*_tmp934;(_tmp934=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,((
_tmp935="initializer found in parameter declaration",_tag_dyneither(_tmp935,
sizeof(char),43))),_tag_dyneither(_tmp934,sizeof(void*),0)));}if(Cyc_Absyn_is_qvar_qualified(
_tmp4F->name)){const char*_tmp938;void*_tmp937;(_tmp937=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,((
_tmp938="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp938,
sizeof(char),47))),_tag_dyneither(_tmp937,sizeof(void*),0)));}{struct _tuple2*
_tmp93E;struct Cyc_Core_Opt*_tmp93D;struct Cyc_List_List*_tmp93C;_tmp4B=((_tmp93C=
_cycalloc(sizeof(*_tmp93C)),((_tmp93C->hd=((_tmp93E=_cycalloc(sizeof(*_tmp93E)),((
_tmp93E->f1=((_tmp93D=_cycalloc(sizeof(*_tmp93D)),((_tmp93D->v=(*_tmp4F->name).f2,
_tmp93D)))),((_tmp93E->f2=_tmp4F->tq,((_tmp93E->f3=(void*)_tmp4F->type,_tmp93E)))))))),((
_tmp93C->tl=_tmp4B,_tmp93C))))));}goto L;_LL32:;_LL33: {const char*_tmp941;void*
_tmp940;(_tmp940=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,((_tmp941="nonvariable declaration in parameter type",
_tag_dyneither(_tmp941,sizeof(char),42))),_tag_dyneither(_tmp940,sizeof(void*),0)));}
_LL2F:;}L: if(_tmp4C == 0){const char*_tmp945;void*_tmp944[1];struct Cyc_String_pa_struct
_tmp943;(_tmp943.tag=0,((_tmp943.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmp46->hd)),((_tmp944[0]=& _tmp943,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp945="%s is not given a type",_tag_dyneither(_tmp945,sizeof(char),23))),
_tag_dyneither(_tmp944,sizeof(void*),1)))))));}}{struct Cyc_Absyn_Function_mod_struct*
_tmp954;struct Cyc_Absyn_WithTypes_struct*_tmp953;struct Cyc_Absyn_WithTypes_struct
_tmp952;struct Cyc_Absyn_Function_mod_struct _tmp951;struct Cyc_List_List*_tmp950;
return(_tmp950=_cycalloc(sizeof(*_tmp950)),((_tmp950->hd=(void*)((void*)((
_tmp954=_cycalloc(sizeof(*_tmp954)),((_tmp954[0]=((_tmp951.tag=3,((_tmp951.f1=(
void*)((void*)((_tmp953=_cycalloc(sizeof(*_tmp953)),((_tmp953[0]=((_tmp952.tag=1,((
_tmp952.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp4B),((_tmp952.f2=0,((_tmp952.f3=0,((_tmp952.f4=0,((_tmp952.f5=0,_tmp952)))))))))))),
_tmp953))))),_tmp951)))),_tmp954))))),((_tmp950->tl=0,_tmp950)))));}}_LL2A:;}
goto _LL29;_LL28:;_LL29: {struct Cyc_List_List*_tmp955;return(_tmp955=_cycalloc(
sizeof(*_tmp955)),((_tmp955->hd=(void*)((void*)tms->hd),((_tmp955->tl=Cyc_Parse_oldstyle2newstyle(
tms->tl,tds,loc),_tmp955)))));}_LL25:;}}struct _tuple10{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(
struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,
struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc);static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,
struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){
if(tds != 0){struct Cyc_Parse_Declarator*_tmp956;d=((_tmp956=_cycalloc(sizeof(*
_tmp956)),((_tmp956->id=d->id,((_tmp956->tms=Cyc_Parse_oldstyle2newstyle(d->tms,
tds,loc),_tmp956))))));}{void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual
tq=Cyc_Absyn_empty_tqual(0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){
tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)
dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;
atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc != 0){void*_tmp63=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v;_LL35: if((int)_tmp63 != 1)goto _LL37;
_LL36: sc=(void*)3;goto _LL34;_LL37: if((int)_tmp63 != 3)goto _LL39;_LL38: sc=(void*)0;
goto _LL34;_LL39:;_LL3A:{const char*_tmp957;Cyc_Parse_err(((_tmp957="bad storage class on function",
_tag_dyneither(_tmp957,sizeof(char),30))),loc);}goto _LL34;_LL34:;}}{void*_tmp66;
struct Cyc_Core_Opt*_tmp67;struct _tuple5 _tmp65=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp66=_tmp65.f1;_tmp67=_tmp65.f2;{struct Cyc_Absyn_Tqual _tmp69;void*
_tmp6A;struct Cyc_List_List*_tmp6B;struct Cyc_List_List*_tmp6C;struct _tuple6 _tmp68=
Cyc_Parse_apply_tms(tq,_tmp66,atts,d->tms);_tmp69=_tmp68.f1;_tmp6A=_tmp68.f2;
_tmp6B=_tmp68.f3;_tmp6C=_tmp68.f4;if(_tmp67 != 0){const char*_tmp95A;void*_tmp959;(
_tmp959=0,Cyc_Tcutil_warn(loc,((_tmp95A="nested type declaration within function prototype",
_tag_dyneither(_tmp95A,sizeof(char),50))),_tag_dyneither(_tmp959,sizeof(void*),0)));}
if(_tmp6B != 0){const char*_tmp95D;void*_tmp95C;(_tmp95C=0,Cyc_Tcutil_warn(loc,((
_tmp95D="bad type params, ignoring",_tag_dyneither(_tmp95D,sizeof(char),26))),
_tag_dyneither(_tmp95C,sizeof(void*),0)));}{void*_tmp71=_tmp6A;struct Cyc_Absyn_FnInfo
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
_tmp7E.f3;if(_tmp7F == 0){{const char*_tmp95E;Cyc_Parse_err(((_tmp95E="missing argument variable in function prototype",
_tag_dyneither(_tmp95E,sizeof(char),48))),loc);}{struct _tuple10*_tmp961;struct
Cyc_List_List*_tmp960;_tmp7B=((_tmp960=_cycalloc(sizeof(*_tmp960)),((_tmp960->hd=((
_tmp961=_cycalloc(sizeof(*_tmp961)),((_tmp961->f1=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp961->f2=_tmp80,((_tmp961->f3=
_tmp81,_tmp961)))))))),((_tmp960->tl=_tmp7B,_tmp960))))));}}else{struct _tuple10*
_tmp964;struct Cyc_List_List*_tmp963;_tmp7B=((_tmp963=_cycalloc(sizeof(*_tmp963)),((
_tmp963->hd=((_tmp964=_cycalloc(sizeof(*_tmp964)),((_tmp964->f1=(struct
_dyneither_ptr*)_tmp7F->v,((_tmp964->f2=_tmp80,((_tmp964->f3=_tmp81,_tmp964)))))))),((
_tmp963->tl=_tmp7B,_tmp963))))));}}}{struct Cyc_Absyn_Fndecl*_tmp965;return(
_tmp965=_cycalloc(sizeof(*_tmp965)),((_tmp965->sc=(void*)sc,((_tmp965->name=d->id,((
_tmp965->tvs=_tmp73,((_tmp965->is_inline=is_inline,((_tmp965->effect=_tmp74,((
_tmp965->ret_type=(void*)_tmp75,((_tmp965->args=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp7B),((_tmp965->c_varargs=_tmp77,((_tmp965->cyc_varargs=
_tmp78,((_tmp965->rgn_po=_tmp79,((_tmp965->body=body,((_tmp965->cached_typ=0,((
_tmp965->param_vardecls=0,((_tmp965->fn_vardecl=0,((_tmp965->attributes=Cyc_List_append(
_tmp7A,_tmp6C),_tmp965)))))))))))))))))))))))))))))));}}_LL3E:;_LL3F: {const char*
_tmp968;void*_tmp967;(_tmp967=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp968="declarator is not a function prototype",
_tag_dyneither(_tmp968,sizeof(char),39))),_tag_dyneither(_tmp967,sizeof(void*),0)));}
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
_tmp969;Cyc_Parse_err(((_tmp969="signed qualifier must come before type",
_tag_dyneither(_tmp969,sizeof(char),39))),_tmp92);}last_loc=_tmp92;seen_sign=1;
sgn=(void*)0;goto _LL40;_LL45: if(*((int*)_tmp8F)!= 1)goto _LL47;_tmp93=((struct Cyc_Parse_Unsigned_spec_struct*)
_tmp8F)->f1;_LL46: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp93);if(seen_type){
const char*_tmp96A;Cyc_Parse_err(((_tmp96A="signed qualifier must come before type",
_tag_dyneither(_tmp96A,sizeof(char),39))),_tmp93);}last_loc=_tmp93;seen_sign=1;
sgn=(void*)1;goto _LL40;_LL47: if(*((int*)_tmp8F)!= 2)goto _LL49;_tmp94=((struct Cyc_Parse_Short_spec_struct*)
_tmp8F)->f1;_LL48: if(seen_size){const char*_tmp96B;Cyc_Parse_err(((_tmp96B="integral size may appear only once within a type specifier",
_tag_dyneither(_tmp96B,sizeof(char),59))),_tmp94);}if(seen_type){const char*
_tmp96C;Cyc_Parse_err(((_tmp96C="short modifier must come before base type",
_tag_dyneither(_tmp96C,sizeof(char),42))),_tmp94);}last_loc=_tmp94;seen_size=1;
sz=(void*)1;goto _LL40;_LL49: if(*((int*)_tmp8F)!= 3)goto _LL4B;_tmp95=((struct Cyc_Parse_Long_spec_struct*)
_tmp8F)->f1;_LL4A: if(seen_type){const char*_tmp96D;Cyc_Parse_err(((_tmp96D="long modifier must come before base type",
_tag_dyneither(_tmp96D,sizeof(char),41))),_tmp95);}if(seen_size){void*_tmp9C=sz;
_LL4E: if((int)_tmp9C != 3)goto _LL50;_LL4F: sz=(void*)4;goto _LL4D;_LL50:;_LL51:{
const char*_tmp96E;Cyc_Parse_err(((_tmp96E="extra long in type specifier",
_tag_dyneither(_tmp96E,sizeof(char),29))),_tmp95);}goto _LL4D;_LL4D:;}else{sz=(
void*)3;}last_loc=_tmp95;seen_size=1;goto _LL40;_LL4B: if(*((int*)_tmp8F)!= 5)goto
_LL40;_tmp96=((struct Cyc_Parse_Decl_spec_struct*)_tmp8F)->f1;_LL4C: last_loc=
_tmp96->loc;if(declopt == 0  && !seen_type){seen_type=1;{void*_tmp9E=(void*)_tmp96->r;
struct Cyc_Absyn_Aggrdecl*_tmp9F;struct Cyc_Absyn_Tuniondecl*_tmpA0;struct Cyc_Absyn_Enumdecl*
_tmpA1;_LL53: if(_tmp9E <= (void*)2)goto _LL59;if(*((int*)_tmp9E)!= 4)goto _LL55;
_tmp9F=((struct Cyc_Absyn_Aggr_d_struct*)_tmp9E)->f1;_LL54: {struct Cyc_List_List*
_tmpA2=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmp9F->tvs));{struct Cyc_Absyn_AggrType_struct _tmp978;union Cyc_Absyn_AggrInfoU_union
_tmp977;struct Cyc_Absyn_AggrInfo _tmp976;struct Cyc_Absyn_AggrType_struct*_tmp975;
t=(void*)((_tmp975=_cycalloc(sizeof(*_tmp975)),((_tmp975[0]=((_tmp978.tag=10,((
_tmp978.f1=((_tmp976.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((_tmp977.UnknownAggr).tag=
0,(((_tmp977.UnknownAggr).f1=(void*)((void*)_tmp9F->kind),(((_tmp977.UnknownAggr).f2=
_tmp9F->name,_tmp977)))))),((_tmp976.targs=_tmpA2,_tmp976)))),_tmp978)))),
_tmp975))));}if(_tmp9F->impl != 0){struct Cyc_Core_Opt*_tmp979;declopt=((_tmp979=
_cycalloc(sizeof(*_tmp979)),((_tmp979->v=_tmp96,_tmp979))));}goto _LL52;}_LL55:
if(*((int*)_tmp9E)!= 5)goto _LL57;_tmpA0=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp9E)->f1;_LL56: {struct Cyc_List_List*_tmpA8=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmpA0->tvs));struct Cyc_Core_Opt*
_tmp97A;struct Cyc_Core_Opt*ropt=_tmpA0->is_flat?0:((_tmp97A=_cycalloc(sizeof(*
_tmp97A)),((_tmp97A->v=(void*)((void*)2),_tmp97A))));{struct Cyc_Absyn_TunionType_struct
_tmp989;union Cyc_Absyn_TunionInfoU_union _tmp988;struct Cyc_Absyn_Tuniondecl**
_tmp987;struct Cyc_Absyn_TunionInfo _tmp986;struct Cyc_Absyn_TunionType_struct*
_tmp985;t=(void*)((_tmp985=_cycalloc(sizeof(*_tmp985)),((_tmp985[0]=((_tmp989.tag=
2,((_tmp989.f1=((_tmp986.tunion_info=(union Cyc_Absyn_TunionInfoU_union)(((
_tmp988.KnownTunion).tag=1,(((_tmp988.KnownTunion).f1=((_tmp987=_cycalloc(
sizeof(*_tmp987)),((_tmp987[0]=_tmpA0,_tmp987)))),_tmp988)))),((_tmp986.targs=
_tmpA8,((_tmp986.rgn=ropt,_tmp986)))))),_tmp989)))),_tmp985))));}if(_tmpA0->fields
!= 0){struct Cyc_Core_Opt*_tmp98A;declopt=((_tmp98A=_cycalloc(sizeof(*_tmp98A)),((
_tmp98A->v=_tmp96,_tmp98A))));}goto _LL52;}_LL57: if(*((int*)_tmp9E)!= 6)goto _LL59;
_tmpA1=((struct Cyc_Absyn_Enum_d_struct*)_tmp9E)->f1;_LL58:{struct Cyc_Absyn_EnumType_struct
_tmp98D;struct Cyc_Absyn_EnumType_struct*_tmp98C;t=(void*)((_tmp98C=_cycalloc(
sizeof(*_tmp98C)),((_tmp98C[0]=((_tmp98D.tag=12,((_tmp98D.f1=_tmpA1->name,((
_tmp98D.f2=0,_tmp98D)))))),_tmp98C))));}{struct Cyc_Core_Opt*_tmp98E;declopt=((
_tmp98E=_cycalloc(sizeof(*_tmp98E)),((_tmp98E->v=_tmp96,_tmp98E))));}goto _LL52;
_LL59:;_LL5A: {const char*_tmp991;void*_tmp990;(_tmp990=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp96->loc,((
_tmp991="bad declaration within type specifier",_tag_dyneither(_tmp991,sizeof(
char),38))),_tag_dyneither(_tmp990,sizeof(void*),0)));}_LL52:;}}else{Cyc_Parse_err(
Cyc_Parse_msg3,_tmp96->loc);}goto _LL40;_LL40:;}if(!seen_type){if(!seen_sign  && !
seen_size){const char*_tmp994;void*_tmp993;(_tmp993=0,Cyc_Tcutil_warn(last_loc,((
_tmp994="missing type within specifier",_tag_dyneither(_tmp994,sizeof(char),30))),
_tag_dyneither(_tmp993,sizeof(void*),0)));}t=Cyc_Absyn_int_typ(sgn,sz);}else{if(
seen_sign){void*_tmpB7=t;void*_tmpB8;void*_tmpB9;_LL5C: if(_tmpB7 <= (void*)4)goto
_LL5E;if(*((int*)_tmpB7)!= 5)goto _LL5E;_tmpB8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpB7)->f1;_tmpB9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB7)->f2;_LL5D:
if(_tmpB8 != sgn)t=Cyc_Absyn_int_typ(sgn,_tmpB9);goto _LL5B;_LL5E:;_LL5F:{const
char*_tmp995;Cyc_Parse_err(((_tmp995="sign specification on non-integral type",
_tag_dyneither(_tmp995,sizeof(char),40))),last_loc);}goto _LL5B;_LL5B:;}if(
seen_size){void*_tmpBB=t;void*_tmpBC;void*_tmpBD;_LL61: if(_tmpBB <= (void*)4)goto
_LL65;if(*((int*)_tmpBB)!= 5)goto _LL63;_tmpBC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpBB)->f1;_tmpBD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBB)->f2;_LL62:
if(_tmpBD != sz)t=Cyc_Absyn_int_typ(_tmpBC,sz);goto _LL60;_LL63: if(*((int*)_tmpBB)
!= 6)goto _LL65;_LL64: t=Cyc_Absyn_double_typ(1);goto _LL60;_LL65:;_LL66:{const char*
_tmp996;Cyc_Parse_err(((_tmp996="size qualifier on non-integral type",
_tag_dyneither(_tmp996,sizeof(char),36))),last_loc);}goto _LL60;_LL60:;}}{struct
_tuple5 _tmp997;return(_tmp997.f1=t,((_tmp997.f2=declopt,_tmp997)));}}static
struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts);static struct
Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,
void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{
struct Cyc_Parse_Declarator*_tmpC0=(struct Cyc_Parse_Declarator*)ds->hd;struct
_tuple1*_tmpC1=_tmpC0->id;struct Cyc_Absyn_Tqual _tmpC3;void*_tmpC4;struct Cyc_List_List*
_tmpC5;struct Cyc_List_List*_tmpC6;struct _tuple6 _tmpC2=Cyc_Parse_apply_tms(tq,t,
shared_atts,_tmpC0->tms);_tmpC3=_tmpC2.f1;_tmpC4=_tmpC2.f2;_tmpC5=_tmpC2.f3;
_tmpC6=_tmpC2.f4;if(ds->tl == 0){struct _tuple7*_tmp99A;struct Cyc_List_List*
_tmp999;return(_tmp999=_region_malloc(r,sizeof(*_tmp999)),((_tmp999->hd=((
_tmp99A=_region_malloc(r,sizeof(*_tmp99A)),((_tmp99A->f1=_tmpC1,((_tmp99A->f2=
_tmpC3,((_tmp99A->f3=_tmpC4,((_tmp99A->f4=_tmpC5,((_tmp99A->f5=_tmpC6,_tmp99A)))))))))))),((
_tmp999->tl=0,_tmp999)))));}else{struct _tuple7*_tmp99D;struct Cyc_List_List*
_tmp99C;return(_tmp99C=_region_malloc(r,sizeof(*_tmp99C)),((_tmp99C->hd=((
_tmp99D=_region_malloc(r,sizeof(*_tmp99D)),((_tmp99D->f1=_tmpC1,((_tmp99D->f2=
_tmpC3,((_tmp99D->f3=_tmpC4,((_tmp99D->f4=_tmpC5,((_tmp99D->f5=_tmpC6,_tmp99D)))))))))))),((
_tmp99C->tl=Cyc_Parse_apply_tmss(r,_tmpC3,Cyc_Tcutil_copy_type(t),ds->tl,
shared_atts),_tmp99C)))));}}}static struct _tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms);static struct _tuple6
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,
struct Cyc_List_List*tms){if(tms == 0){struct _tuple6 _tmp99E;return(_tmp99E.f1=tq,((
_tmp99E.f2=t,((_tmp99E.f3=0,((_tmp99E.f4=atts,_tmp99E)))))));}{void*_tmpCC=(void*)
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
void*)as->hd)){struct Cyc_List_List*_tmp99F;fn_atts=((_tmp99F=_cycalloc(sizeof(*
_tmp99F)),((_tmp99F->hd=(void*)((void*)as->hd),((_tmp99F->tl=fn_atts,_tmp99F))))));}
else{struct Cyc_List_List*_tmp9A0;new_atts=((_tmp9A0=_cycalloc(sizeof(*_tmp9A0)),((
_tmp9A0->hd=(void*)((void*)as->hd),((_tmp9A0->tl=new_atts,_tmp9A0))))));}}}if(
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
Cyc_Absyn_NoTypes_struct*)_tmpD9)->f2;_LL78: {const char*_tmp9A3;void*_tmp9A2;(
_tmp9A2=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(_tmpDF,((_tmp9A3="function declaration without parameter types",
_tag_dyneither(_tmp9A3,sizeof(char),45))),_tag_dyneither(_tmp9A2,sizeof(void*),0)));}
_LL74:;}_LL6E: if(*((int*)_tmpCC)!= 4)goto _LL70;_tmpD3=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpCC)->f1;_tmpD4=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCC)->f2;_LL6F:
if(tms->tl == 0){struct _tuple6 _tmp9A4;return(_tmp9A4.f1=tq,((_tmp9A4.f2=t,((
_tmp9A4.f3=_tmpD3,((_tmp9A4.f4=atts,_tmp9A4)))))));}{const char*_tmp9A7;void*
_tmp9A6;(_tmp9A6=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD4,((_tmp9A7="type parameters must appear before function arguments in declarator",
_tag_dyneither(_tmp9A7,sizeof(char),68))),_tag_dyneither(_tmp9A6,sizeof(void*),0)));}
_LL70: if(*((int*)_tmpCC)!= 2)goto _LL72;_tmpD5=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpCC)->f1;_tmpD6=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCC)->f2;_LL71: {
struct Cyc_Absyn_PointerType_struct _tmp9AD;struct Cyc_Absyn_PtrInfo _tmp9AC;struct
Cyc_Absyn_PointerType_struct*_tmp9AB;return Cyc_Parse_apply_tms(_tmpD6,(void*)((
_tmp9AB=_cycalloc(sizeof(*_tmp9AB)),((_tmp9AB[0]=((_tmp9AD.tag=4,((_tmp9AD.f1=((
_tmp9AC.elt_typ=(void*)t,((_tmp9AC.elt_tq=tq,((_tmp9AC.ptr_atts=_tmpD5,_tmp9AC)))))),
_tmp9AD)))),_tmp9AB)))),atts,tms->tl);}_LL72: if(*((int*)_tmpCC)!= 5)goto _LL67;
_tmpD7=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpCC)->f1;_tmpD8=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpCC)->f2;_LL73: return Cyc_Parse_apply_tms(tq,t,Cyc_List_append(atts,_tmpD8),
tms->tl);_LL67:;}}void*Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*
loc);void*Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*
loc){void*_tmpED;struct Cyc_Core_Opt*_tmpEE;struct _tuple5 _tmpEC=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmpED=_tmpEC.f1;_tmpEE=_tmpEC.f2;if(_tmpEE != 0){const char*_tmp9B0;void*
_tmp9AF;(_tmp9AF=0,Cyc_Tcutil_warn(loc,((_tmp9B0="ignoring nested type declaration(s) in specifier list",
_tag_dyneither(_tmp9B0,sizeof(char),54))),_tag_dyneither(_tmp9AF,sizeof(void*),0)));}
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
else{kind=_tmpF9;}goto _LL7E;_LL81:;_LL82: kind=0;{struct Cyc_Core_Opt*_tmp9B1;type=((
_tmp9B1=_cycalloc(sizeof(*_tmp9B1)),((_tmp9B1->v=(void*)_tmpF5,_tmp9B1))));}goto
_LL7E;_LL7E:;}{struct Cyc_Absyn_Typedef_d_struct _tmp9B7;struct Cyc_Absyn_Typedefdecl*
_tmp9B6;struct Cyc_Absyn_Typedef_d_struct*_tmp9B5;return Cyc_Absyn_new_decl((void*)((
_tmp9B5=_cycalloc(sizeof(*_tmp9B5)),((_tmp9B5[0]=((_tmp9B7.tag=7,((_tmp9B7.f1=((
_tmp9B6=_cycalloc(sizeof(*_tmp9B6)),((_tmp9B6->name=_tmpF3,((_tmp9B6->tvs=_tmpF6,((
_tmp9B6->kind=kind,((_tmp9B6->defn=type,((_tmp9B6->atts=_tmpF7,((_tmp9B6->tq=
_tmpF4,_tmp9B6)))))))))))))),_tmp9B7)))),_tmp9B5)))),loc);}}}static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*
s){struct Cyc_Absyn_Decl_s_struct _tmp9BA;struct Cyc_Absyn_Decl_s_struct*_tmp9B9;
return Cyc_Absyn_new_stmt((void*)((_tmp9B9=_cycalloc(sizeof(*_tmp9B9)),((_tmp9B9[
0]=((_tmp9BA.tag=11,((_tmp9BA.f1=d,((_tmp9BA.f2=s,_tmp9BA)))))),_tmp9B9)))),d->loc);}
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
const char*_tmp9BB;Cyc_Parse_err(((_tmp9BB="inline is allowed only on function definitions",
_tag_dyneither(_tmp9BB,sizeof(char),47))),loc);}if(_tmp104 == 0){{const char*
_tmp9BC;Cyc_Parse_err(((_tmp9BC="missing type specifiers in declaration",
_tag_dyneither(_tmp9BC,sizeof(char),39))),loc);}{struct Cyc_List_List*_tmp108=0;
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
_tmp114;_LL93: if(_tmp111 <= (void*)2)goto _LL99;if(*((int*)_tmp111)!= 6)goto _LL95;
_tmp112=((struct Cyc_Absyn_Enum_d_struct*)_tmp111)->f1;_LL94:(void*)(_tmp112->sc=(
void*)s);if(_tmp105 != 0){const char*_tmp9BD;Cyc_Parse_err(((_tmp9BD="bad attributes on enum",
_tag_dyneither(_tmp9BD,sizeof(char),23))),loc);}goto _LL92;_LL95: if(*((int*)
_tmp111)!= 4)goto _LL97;_tmp113=((struct Cyc_Absyn_Aggr_d_struct*)_tmp111)->f1;
_LL96:(void*)(_tmp113->sc=(void*)s);_tmp113->attributes=_tmp105;goto _LL92;_LL97:
if(*((int*)_tmp111)!= 5)goto _LL99;_tmp114=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp111)->f1;_LL98:(void*)(_tmp114->sc=(void*)s);if(_tmp105 != 0){const char*
_tmp9BE;Cyc_Parse_err(((_tmp9BE="bad attributes on tunion",_tag_dyneither(
_tmp9BE,sizeof(char),25))),loc);}goto _LL92;_LL99:;_LL9A:{const char*_tmp9BF;Cyc_Parse_err(((
_tmp9BF="bad declaration",_tag_dyneither(_tmp9BF,sizeof(char),16))),loc);}{
struct Cyc_List_List*_tmp118=0;_npop_handler(0);return _tmp118;}_LL92:;}{struct Cyc_List_List*
_tmp9C0;struct Cyc_List_List*_tmp11A=(_tmp9C0=_cycalloc(sizeof(*_tmp9C0)),((
_tmp9C0->hd=d,((_tmp9C0->tl=0,_tmp9C0)))));_npop_handler(0);return _tmp11A;}}
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
struct Cyc_Absyn_Aggrdecl*_tmp9C1;struct Cyc_Absyn_Aggrdecl*_tmp12E=(_tmp9C1=
_cycalloc(sizeof(*_tmp9C1)),((_tmp9C1->kind=(void*)_tmp11E,((_tmp9C1->sc=(void*)
s,((_tmp9C1->name=_tmp11F,((_tmp9C1->tvs=_tmp12D,((_tmp9C1->impl=0,((_tmp9C1->attributes=
0,_tmp9C1)))))))))))));if(_tmp105 != 0){const char*_tmp9C2;Cyc_Parse_err(((_tmp9C2="bad attributes on type declaration",
_tag_dyneither(_tmp9C2,sizeof(char),35))),loc);}{struct Cyc_Absyn_Aggr_d_struct*
_tmp9C8;struct Cyc_Absyn_Aggr_d_struct _tmp9C7;struct Cyc_List_List*_tmp9C6;struct
Cyc_List_List*_tmp133=(_tmp9C6=_cycalloc(sizeof(*_tmp9C6)),((_tmp9C6->hd=Cyc_Absyn_new_decl((
void*)((_tmp9C8=_cycalloc(sizeof(*_tmp9C8)),((_tmp9C8[0]=((_tmp9C7.tag=4,((
_tmp9C7.f1=_tmp12E,_tmp9C7)))),_tmp9C8)))),loc),((_tmp9C6->tl=0,_tmp9C6)))));
_npop_handler(0);return _tmp133;}}_LL9E: if(*((int*)_tmp11B)!= 2)goto _LLA0;_tmp121=((
struct Cyc_Absyn_TunionType_struct*)_tmp11B)->f1;_tmp122=_tmp121.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp11B)->f1).tunion_info).KnownTunion).tag != 
1)goto _LLA0;_tmp123=(_tmp122.KnownTunion).f1;_LL9F: if(_tmp105 != 0){const char*
_tmp9C9;Cyc_Parse_err(((_tmp9C9="bad attributes on tunion",_tag_dyneither(
_tmp9C9,sizeof(char),25))),loc);}{struct Cyc_Absyn_Tunion_d_struct*_tmp9CF;struct
Cyc_Absyn_Tunion_d_struct _tmp9CE;struct Cyc_List_List*_tmp9CD;struct Cyc_List_List*
_tmp139=(_tmp9CD=_cycalloc(sizeof(*_tmp9CD)),((_tmp9CD->hd=Cyc_Absyn_new_decl((
void*)((_tmp9CF=_cycalloc(sizeof(*_tmp9CF)),((_tmp9CF[0]=((_tmp9CE.tag=5,((
_tmp9CE.f1=*_tmp123,_tmp9CE)))),_tmp9CF)))),loc),((_tmp9CD->tl=0,_tmp9CD)))));
_npop_handler(0);return _tmp139;}_LLA0: if(*((int*)_tmp11B)!= 2)goto _LLA2;_tmp124=((
struct Cyc_Absyn_TunionType_struct*)_tmp11B)->f1;_tmp125=_tmp124.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp11B)->f1).tunion_info).UnknownTunion).tag
!= 0)goto _LLA2;_tmp126=(_tmp125.UnknownTunion).f1;_tmp127=_tmp126.name;_tmp128=
_tmp126.is_xtunion;_tmp129=_tmp126.is_flat;_tmp12A=_tmp124.targs;_LLA1: {struct
Cyc_List_List*_tmp13A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp12A);struct Cyc_Absyn_Decl*_tmp13B=
Cyc_Absyn_tunion_decl(s,_tmp127,_tmp13A,0,_tmp128,_tmp129,loc);if(_tmp105 != 0){
const char*_tmp9D0;Cyc_Parse_err(((_tmp9D0="bad attributes on tunion",
_tag_dyneither(_tmp9D0,sizeof(char),25))),loc);}{struct Cyc_List_List*_tmp9D1;
struct Cyc_List_List*_tmp13E=(_tmp9D1=_cycalloc(sizeof(*_tmp9D1)),((_tmp9D1->hd=
_tmp13B,((_tmp9D1->tl=0,_tmp9D1)))));_npop_handler(0);return _tmp13E;}}_LLA2: if(*((
int*)_tmp11B)!= 12)goto _LLA4;_tmp12B=((struct Cyc_Absyn_EnumType_struct*)_tmp11B)->f1;
_LLA3: {struct Cyc_Absyn_Enumdecl*_tmp9D2;struct Cyc_Absyn_Enumdecl*_tmp13F=(
_tmp9D2=_cycalloc(sizeof(*_tmp9D2)),((_tmp9D2->sc=(void*)s,((_tmp9D2->name=
_tmp12B,((_tmp9D2->fields=0,_tmp9D2)))))));if(_tmp105 != 0){const char*_tmp9D3;Cyc_Parse_err(((
_tmp9D3="bad attributes on enum",_tag_dyneither(_tmp9D3,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp9DD;struct Cyc_Absyn_Enum_d_struct _tmp9DC;struct Cyc_Absyn_Enum_d_struct*
_tmp9DB;struct Cyc_List_List*_tmp9DA;struct Cyc_List_List*_tmp145=(_tmp9DA=
_cycalloc(sizeof(*_tmp9DA)),((_tmp9DA->hd=((_tmp9DD=_cycalloc(sizeof(*_tmp9DD)),((
_tmp9DD->r=(void*)((void*)((_tmp9DB=_cycalloc(sizeof(*_tmp9DB)),((_tmp9DB[0]=((
_tmp9DC.tag=6,((_tmp9DC.f1=_tmp13F,_tmp9DC)))),_tmp9DB))))),((_tmp9DD->loc=loc,
_tmp9DD)))))),((_tmp9DA->tl=0,_tmp9DA)))));_npop_handler(0);return _tmp145;}}
_LLA4: if(*((int*)_tmp11B)!= 13)goto _LLA6;_tmp12C=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp11B)->f1;_LLA5: {struct Cyc_Core_Opt*_tmp9E0;struct Cyc_Absyn_Enumdecl*_tmp9DF;
struct Cyc_Absyn_Enumdecl*_tmp147=(_tmp9DF=_cycalloc(sizeof(*_tmp9DF)),((_tmp9DF->sc=(
void*)s,((_tmp9DF->name=Cyc_Parse_gensym_enum(),((_tmp9DF->fields=((_tmp9E0=
_cycalloc(sizeof(*_tmp9E0)),((_tmp9E0->v=_tmp12C,_tmp9E0)))),_tmp9DF)))))));if(
_tmp105 != 0){const char*_tmp9E1;Cyc_Parse_err(((_tmp9E1="bad attributes on enum",
_tag_dyneither(_tmp9E1,sizeof(char),23))),loc);}{struct Cyc_Absyn_Decl*_tmp9EB;
struct Cyc_Absyn_Enum_d_struct _tmp9EA;struct Cyc_Absyn_Enum_d_struct*_tmp9E9;
struct Cyc_List_List*_tmp9E8;struct Cyc_List_List*_tmp14D=(_tmp9E8=_cycalloc(
sizeof(*_tmp9E8)),((_tmp9E8->hd=((_tmp9EB=_cycalloc(sizeof(*_tmp9EB)),((_tmp9EB->r=(
void*)((void*)((_tmp9E9=_cycalloc(sizeof(*_tmp9E9)),((_tmp9E9[0]=((_tmp9EA.tag=6,((
_tmp9EA.f1=_tmp147,_tmp9EA)))),_tmp9E9))))),((_tmp9EB->loc=loc,_tmp9EB)))))),((
_tmp9E8->tl=0,_tmp9E8)))));_npop_handler(0);return _tmp14D;}}_LLA6:;_LLA7:{const
char*_tmp9EC;Cyc_Parse_err(((_tmp9EC="missing declarator",_tag_dyneither(_tmp9EC,
sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp151=0;_npop_handler(0);return
_tmp151;}_LL9B:;}}else{void*t=_tmp10D.f1;struct Cyc_List_List*_tmp152=Cyc_Parse_apply_tmss(
mkrgn,_tmp103,t,_tmp10B,_tmp105);if(istypedef){if(!exps_empty){const char*_tmp9ED;
Cyc_Parse_err(((_tmp9ED="initializer in typedef declaration",_tag_dyneither(
_tmp9ED,sizeof(char),35))),loc);}{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple7*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp152);if(_tmp10D.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(
_tmp10D.f2)->v;{void*_tmp154=(void*)d->r;struct Cyc_Absyn_Aggrdecl*_tmp155;struct
Cyc_Absyn_Tuniondecl*_tmp156;struct Cyc_Absyn_Enumdecl*_tmp157;_LLA9: if(_tmp154 <= (
void*)2)goto _LLAF;if(*((int*)_tmp154)!= 4)goto _LLAB;_tmp155=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp154)->f1;_LLAA:(void*)(_tmp155->sc=(void*)s);_tmp155->attributes=_tmp105;
_tmp105=0;goto _LLA8;_LLAB: if(*((int*)_tmp154)!= 5)goto _LLAD;_tmp156=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp154)->f1;_LLAC:(void*)(_tmp156->sc=(void*)s);goto _LLA8;_LLAD: if(*((int*)
_tmp154)!= 6)goto _LLAF;_tmp157=((struct Cyc_Absyn_Enum_d_struct*)_tmp154)->f1;
_LLAE:(void*)(_tmp157->sc=(void*)s);goto _LLA8;_LLAF:;_LLB0:{const char*_tmp9EE;
Cyc_Parse_err(((_tmp9EE="declaration within typedef is not a struct, union, tunion, or xtunion",
_tag_dyneither(_tmp9EE,sizeof(char),70))),loc);}goto _LLA8;_LLA8:;}{struct Cyc_List_List*
_tmp9EF;decls=((_tmp9EF=_cycalloc(sizeof(*_tmp9EF)),((_tmp9EF->hd=d,((_tmp9EF->tl=
decls,_tmp9EF))))));}}{struct Cyc_List_List*_tmp15A=decls;_npop_handler(0);return
_tmp15A;}}}else{if(_tmp10D.f2 != 0){const char*_tmp9F0;Cyc_Parse_unimp(((_tmp9F0="nested type declaration within declarator",
_tag_dyneither(_tmp9F0,sizeof(char),42))),loc);}{struct Cyc_List_List*decls=0;{
struct Cyc_List_List*_tmp15C=_tmp152;for(0;_tmp15C != 0;(_tmp15C=_tmp15C->tl,
_tmp10C=_tmp10C->tl)){struct _tuple7 _tmp15E;struct _tuple1*_tmp15F;struct Cyc_Absyn_Tqual
_tmp160;void*_tmp161;struct Cyc_List_List*_tmp162;struct Cyc_List_List*_tmp163;
struct _tuple7*_tmp15D=(struct _tuple7*)_tmp15C->hd;_tmp15E=*_tmp15D;_tmp15F=
_tmp15E.f1;_tmp160=_tmp15E.f2;_tmp161=_tmp15E.f3;_tmp162=_tmp15E.f4;_tmp163=
_tmp15E.f5;if(_tmp162 != 0){const char*_tmp9F3;void*_tmp9F2;(_tmp9F2=0,Cyc_Tcutil_warn(
loc,((_tmp9F3="bad type params, ignoring",_tag_dyneither(_tmp9F3,sizeof(char),26))),
_tag_dyneither(_tmp9F2,sizeof(void*),0)));}if(_tmp10C == 0){const char*_tmp9F6;
void*_tmp9F5;(_tmp9F5=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp9F6="unexpected NULL in parse!",
_tag_dyneither(_tmp9F6,sizeof(char),26))),_tag_dyneither(_tmp9F5,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp168=(struct Cyc_Absyn_Exp*)_tmp10C->hd;struct Cyc_Absyn_Vardecl*
_tmp169=Cyc_Absyn_new_vardecl(_tmp15F,_tmp161,_tmp168);_tmp169->tq=_tmp160;(void*)(
_tmp169->sc=(void*)s);_tmp169->attributes=_tmp163;{struct Cyc_Absyn_Var_d_struct*
_tmp9FC;struct Cyc_Absyn_Var_d_struct _tmp9FB;struct Cyc_Absyn_Decl*_tmp9FA;struct
Cyc_Absyn_Decl*_tmp16A=(_tmp9FA=_cycalloc(sizeof(*_tmp9FA)),((_tmp9FA->r=(void*)((
void*)((_tmp9FC=_cycalloc(sizeof(*_tmp9FC)),((_tmp9FC[0]=((_tmp9FB.tag=0,((
_tmp9FB.f1=_tmp169,_tmp9FB)))),_tmp9FC))))),((_tmp9FA->loc=loc,_tmp9FA)))));
struct Cyc_List_List*_tmp9FD;decls=((_tmp9FD=_cycalloc(sizeof(*_tmp9FD)),((
_tmp9FD->hd=_tmp16A,((_tmp9FD->tl=decls,_tmp9FD))))));}}}}{struct Cyc_List_List*
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
const char*_tmpA02;void*_tmpA01[2];struct Cyc_String_pa_struct _tmpA00;struct Cyc_Int_pa_struct
_tmp9FF;Cyc_Parse_err((struct _dyneither_ptr)((_tmp9FF.tag=1,((_tmp9FF.f1=(
unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmpA00.tag=0,((
_tmpA00.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmpA01[0]=& _tmpA00,((
_tmpA01[1]=& _tmp9FF,Cyc_aprintf(((_tmpA02="bad kind: %s; strlen=%d",
_tag_dyneither(_tmpA02,sizeof(char),24))),_tag_dyneither(_tmpA01,sizeof(void*),2)))))))))))))),
loc);}return(void*)2;}static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*
p);static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*
_tmp174=(void*)p->r;struct _tuple1*_tmp175;struct Cyc_Absyn_Vardecl*_tmp176;struct
Cyc_Absyn_Pat*_tmp177;struct Cyc_Absyn_Pat _tmp178;void*_tmp179;struct Cyc_Absyn_Pat*
_tmp17A;void*_tmp17B;int _tmp17C;char _tmp17D;struct _dyneither_ptr _tmp17E;struct
_tuple1*_tmp17F;struct Cyc_List_List*_tmp180;int _tmp181;struct Cyc_Absyn_Exp*
_tmp182;_LLBC: if(_tmp174 <= (void*)2)goto _LLC2;if(*((int*)_tmp174)!= 12)goto _LLBE;
_tmp175=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp174)->f1;_LLBD: {struct Cyc_Absyn_UnknownId_e_struct
_tmpA05;struct Cyc_Absyn_UnknownId_e_struct*_tmpA04;return Cyc_Absyn_new_exp((void*)((
_tmpA04=_cycalloc(sizeof(*_tmpA04)),((_tmpA04[0]=((_tmpA05.tag=2,((_tmpA05.f1=
_tmp175,_tmpA05)))),_tmpA04)))),p->loc);}_LLBE: if(*((int*)_tmp174)!= 1)goto _LLC0;
_tmp176=((struct Cyc_Absyn_Reference_p_struct*)_tmp174)->f1;_tmp177=((struct Cyc_Absyn_Reference_p_struct*)
_tmp174)->f2;_tmp178=*_tmp177;_tmp179=(void*)_tmp178.r;if((int)_tmp179 != 0)goto
_LLC0;_LLBF: {struct Cyc_Absyn_UnknownId_e_struct _tmpA08;struct Cyc_Absyn_UnknownId_e_struct*
_tmpA07;return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)((_tmpA07=_cycalloc(
sizeof(*_tmpA07)),((_tmpA07[0]=((_tmpA08.tag=2,((_tmpA08.f1=_tmp176->name,
_tmpA08)))),_tmpA07)))),p->loc),p->loc);}_LLC0: if(*((int*)_tmp174)!= 4)goto _LLC2;
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
_tmpA0B;struct Cyc_Absyn_UnknownId_e_struct*_tmpA0A;struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((
void*)((_tmpA0A=_cycalloc(sizeof(*_tmpA0A)),((_tmpA0A[0]=((_tmpA0B.tag=2,((
_tmpA0B.f1=_tmp17F,_tmpA0B)))),_tmpA0A)))),p->loc);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp180);return Cyc_Absyn_unknowncall_exp(e1,
es,p->loc);}_LLCC: if(*((int*)_tmp174)!= 14)goto _LLCE;_tmp182=((struct Cyc_Absyn_Exp_p_struct*)
_tmp174)->f1;_LLCD: return _tmp182;_LLCE:;_LLCF:{const char*_tmpA0C;Cyc_Parse_err(((
_tmpA0C="cannot mix patterns and expressions in case",_tag_dyneither(_tmpA0C,
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
Cyc_yynewloc();struct Cyc_Yyltype Cyc_yynewloc(){struct Cyc_Yyltype _tmpA0D;return(
_tmpA0D.timestamp=0,((_tmpA0D.first_line=0,((_tmpA0D.first_column=0,((_tmpA0D.last_line=
0,((_tmpA0D.last_column=0,_tmpA0D)))))))));}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,
0};static short Cyc_yytranslate[362]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,133,2,2,115,131,128,2,112,113,121,125,110,129,118,130,2,2,
2,2,2,2,2,2,2,2,117,107,116,111,124,123,122,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,119,2,120,127,114,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
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
static char _tmp2DB[4]="'('";static char _tmp2DC[4]="')'";static char _tmp2DD[4]="'_'";
static char _tmp2DE[4]="'$'";static char _tmp2DF[4]="'<'";static char _tmp2E0[4]="':'";
static char _tmp2E1[4]="'.'";static char _tmp2E2[4]="'['";static char _tmp2E3[4]="']'";
static char _tmp2E4[4]="'*'";static char _tmp2E5[4]="'@'";static char _tmp2E6[4]="'?'";
static char _tmp2E7[4]="'>'";static char _tmp2E8[4]="'+'";static char _tmp2E9[4]="'|'";
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
142,143,144,145,145,145,145,146,146,147,147,147,147,147,147,147,147,147,147,148,
148,148,148,148,148,148,149,149,150,151,151,152,152,152,152,152,152,153,153,154,
154,154,154,154,154,154,154,154,154,154,154,154,154,154,154,154,154,154,154,154,
154,155,156,156,156,157,157,157,158,158,159,159,159,160,160,160,161,161,162,162,
163,163,164,164,165,166,166,167,167,168,169,169,169,169,169,169,170,170,170,170,
170,170,171,172,172,173,173,173,174,174,174,175,175,176,176,177,177,177,177,178,
178,178,179,179,180,180,181,181,182,182,182,182,182,182,182,182,182,182,183,183,
183,183,183,183,183,183,183,183,183,184,184,185,186,186,186,187,187,188,188,188,
189,189,189,190,190,190,191,191,192,192,192,192,193,193,194,194,195,195,196,196,
197,197,198,198,199,199,199,199,200,200,201,201,202,202,202,203,204,204,205,205,
206,206,206,206,207,207,207,207,208,209,209,210,210,211,211,212,212,212,212,212,
213,213,214,214,214,215,215,215,215,215,215,215,215,215,215,215,216,216,216,216,
216,216,216,216,216,216,216,216,217,217,218,219,219,220,220,221,221,221,221,221,
221,222,222,222,222,222,222,223,223,223,223,223,223,224,224,224,224,224,224,224,
224,224,224,224,224,224,224,225,225,225,225,225,225,225,225,226,227,227,228,228,
229,229,230,230,231,231,232,232,233,233,233,234,234,234,234,234,235,235,235,236,
236,236,237,237,237,237,238,238,239,239,239,239,239,239,239,240,241,242,242,242,
242,242,242,242,242,242,242,242,242,242,242,243,243,243,244,244,245,245,246,246,
246,247,247,248,248,249,249,250,250,250,250,250,250,250,250,250,250,250,251,251,
251,251,251,251,251,252,253,253,254,254,255,255,256,256,257,257,258,258,258,259,
259,259,259,259,260,260,260,261,261,261,262,262,262,262,263,263,264,264,264,264,
264,264,264,264,264,264,264,264,264,264,264,264,264,264,264,265,265,265,266,266,
266,266,266,266,266,266,266,266,267,267,267,267,267,267,267,267,267,268,269,269,
270,270,270,270,271,271,272,272,273,273,274,274,275,275};static short Cyc_yyr2[500]={
0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,1,2,1,2,1,2,1,
2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,
4,4,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,
1,2,1,2,1,2,1,2,1,1,3,1,2,3,8,5,7,1,0,1,1,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,
5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,2,2,1,0,3,0,1,1,0,1,1,0,1,1,0,2,3,5,5,7,1,3,
0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,
2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,6,4,9,6,5,9,0,5,3,1,2,2,
3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,
5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,
6,1,1,1,3,1,1,3,4,4,5,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,1,1,1,1,1,1,1,
1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,
1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,6,4,4,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,1,3,2,5,
4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[1011]={0,18,45,
46,47,48,50,65,68,69,70,71,72,73,74,75,88,89,90,104,105,41,0,0,51,0,0,0,85,0,0,
137,0,0,490,198,492,491,493,0,0,66,0,181,181,180,1,0,16,0,0,17,0,35,43,37,63,39,
76,77,0,78,0,138,150,0,175,183,79,154,102,49,48,42,0,92,489,0,490,486,487,488,0,
363,0,0,0,0,221,0,365,366,25,27,138,138,0,0,138,0,0,0,0,0,0,138,0,178,179,2,0,0,
0,0,29,0,110,111,113,36,44,38,40,138,494,495,102,139,140,186,138,33,138,0,19,138,
138,0,163,151,176,185,184,189,138,64,0,49,96,0,94,0,490,372,0,0,0,0,0,0,0,0,0,0,
0,0,102,0,0,0,0,476,138,0,0,0,0,463,461,462,0,389,391,404,412,414,416,418,420,
422,425,430,433,436,440,0,442,464,475,474,0,373,371,32,0,0,102,0,0,0,120,116,118,
239,241,0,0,9,10,0,0,496,497,199,87,0,0,155,67,219,0,216,0,3,0,5,0,30,0,0,138,20,
0,138,108,0,101,188,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,490,277,279,0,285,281,0,
283,262,263,264,0,265,266,267,0,34,21,113,206,222,0,0,202,200,0,246,0,183,0,191,
52,190,0,0,97,93,0,0,367,0,0,138,450,138,406,440,0,407,408,0,0,0,0,0,0,0,0,138,
138,443,444,138,0,0,0,446,447,445,0,364,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
394,395,396,397,398,399,400,401,402,403,393,0,448,0,470,471,0,0,0,478,0,102,379,
380,0,377,0,223,0,0,0,138,242,214,0,121,126,122,124,117,119,138,0,248,240,249,
499,498,0,81,84,138,82,86,58,57,0,55,156,138,218,152,248,220,164,165,138,80,182,
26,0,28,0,112,114,225,224,22,99,109,0,0,0,128,129,131,0,102,102,0,0,0,0,0,0,138,
0,314,315,316,0,0,318,0,274,0,0,0,0,0,138,286,282,113,284,280,278,207,138,0,0,
213,201,208,159,0,0,138,202,161,138,162,157,183,192,53,103,0,98,411,95,91,375,
376,0,0,0,226,0,0,230,0,0,0,235,0,0,0,0,0,0,138,0,0,0,0,0,477,484,0,483,390,413,
0,415,417,419,421,423,424,428,429,426,427,431,432,434,435,437,438,439,392,469,
466,0,468,0,138,0,368,0,374,31,386,0,382,387,0,384,369,0,0,243,127,123,125,0,202,
0,183,0,250,138,138,0,261,245,0,0,0,0,120,0,102,138,138,0,174,153,217,4,6,0,132,
115,0,0,202,0,135,0,0,0,293,0,0,0,0,0,313,317,0,0,0,0,0,0,0,0,0,276,138,23,200,
138,210,0,0,203,0,160,206,194,247,158,192,177,0,7,0,449,0,0,238,0,227,0,231,234,
236,454,0,0,0,0,0,0,459,460,0,0,0,441,480,0,0,467,465,0,0,378,381,383,370,0,244,
215,257,0,251,252,183,0,202,0,183,0,83,0,0,0,54,56,0,166,0,202,0,0,183,0,130,133,
138,102,145,0,0,0,0,0,0,0,0,0,0,0,0,0,293,319,0,0,269,0,0,0,0,24,202,0,211,209,0,
202,138,193,0,8,0,0,0,237,228,232,0,0,0,0,0,409,410,453,482,0,485,405,479,481,0,
385,388,256,254,259,0,260,253,183,0,61,59,60,167,170,0,172,173,168,183,0,136,147,
146,0,0,141,287,293,0,0,0,138,0,0,321,322,324,326,328,330,332,334,337,342,345,
348,352,354,361,362,0,138,290,299,0,0,0,0,0,0,0,0,0,0,320,0,0,0,0,272,0,196,212,
204,195,200,13,0,0,452,451,0,233,455,138,0,458,472,0,258,255,0,171,169,100,134,
148,145,145,0,0,293,138,358,138,0,355,138,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,356,294,0,301,0,0,0,309,0,0,0,0,0,0,292,0,268,271,0,0,0,202,12,14,0,0,0,0,473,
0,138,144,143,288,289,0,0,0,0,293,295,325,0,327,329,331,333,335,336,340,341,338,
339,343,344,346,347,349,350,351,0,300,302,303,0,311,310,0,305,0,0,0,275,0,0,205,
197,15,0,0,138,0,0,291,357,0,353,296,0,138,304,312,306,307,0,0,0,229,456,0,62,
149,0,0,323,293,297,308,270,273,0,360,359,298,457,0,0,0};static short Cyc_yydefgoto[
142]={1008,46,643,842,47,48,268,49,424,50,426,51,132,52,53,488,210,410,411,211,
56,223,212,58,148,149,59,145,60,242,243,115,116,117,244,213,393,436,437,438,61,
62,129,788,789,790,63,439,64,415,65,66,67,107,143,248,295,640,570,68,571,480,632,
472,476,477,388,288,231,87,88,504,429,505,506,507,508,214,290,291,572,399,271,
619,272,273,274,275,276,715,277,278,798,799,800,801,802,803,804,805,806,807,808,
809,810,811,812,813,377,378,379,560,561,562,279,181,365,182,494,183,184,185,186,
187,188,189,190,191,192,193,194,195,196,197,523,524,198,199,70,843,224,403};
static short Cyc_yypact[1011]={2525,- -32768,- -32768,- -32768,- -32768,- 1,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,3098,205,1187,- -32768,- 2,32,38,55,112,130,- -32768,
119,165,- -32768,237,- -32768,- -32768,- -32768,226,208,270,243,249,249,- -32768,-
-32768,2401,- -32768,185,216,- -32768,627,3098,3098,3098,- -32768,3098,- -32768,-
-32768,513,- -32768,353,3169,261,154,415,372,- -32768,- -32768,335,374,275,- -32768,
- 2,381,- -32768,1350,200,- -32768,- -32768,- -32768,5115,- -32768,274,364,1350,394,
393,396,- -32768,207,- -32768,- -32768,5474,5474,2401,2401,5474,5115,380,399,168,
411,380,5809,5115,- -32768,- -32768,- -32768,2401,2648,2401,2648,- -32768,441,448,-
-32768,3026,- -32768,- -32768,- -32768,- -32768,5608,- -32768,- -32768,335,- -32768,-
-32768,73,1755,- -32768,3169,627,- -32768,3236,5474,3882,- -32768,261,- -32768,-
-32768,- -32768,140,5474,- -32768,2648,- -32768,455,467,479,- 2,4,- -32768,1350,204,
5236,466,5115,1109,483,493,512,518,520,522,335,527,549,5292,5292,- -32768,1312,
556,5348,5348,5348,- -32768,- -32768,- -32768,24,- -32768,- -32768,78,571,558,548,574,
616,114,637,258,297,- -32768,899,5348,209,- 19,- -32768,610,714,631,- -32768,- -32768,
640,5115,335,714,621,144,5541,5876,5541,560,- -32768,- 25,- 25,- -32768,- -32768,- 18,
333,- -32768,- -32768,- -32768,- -32768,22,653,- -32768,- -32768,555,352,- -32768,661,-
-32768,663,- -32768,674,- -32768,168,3979,3169,- -32768,684,5675,- -32768,903,687,-
-32768,- 2,- -32768,689,266,701,3396,710,712,720,732,4044,3396,169,69,733,755,766,
- -32768,- -32768,737,1624,1624,627,1624,- -32768,- -32768,- -32768,778,- -32768,-
-32768,- -32768,149,- -32768,- -32768,777,793,- -32768,107,779,774,365,780,769,28,
372,775,- -32768,792,- -32768,28,795,- 2,- -32768,5115,798,- -32768,784,785,1312,-
-32768,5809,- -32768,- -32768,3494,- -32768,829,5115,5115,5115,5115,5115,5115,806,
5115,5809,1312,- -32768,- -32768,1886,807,395,5115,- -32768,- -32768,- -32768,5115,-
-32768,5348,5115,5348,5348,5348,5348,5348,5348,5348,5348,5348,5348,5348,5348,
5348,5348,5348,5348,5348,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,5115,- -32768,380,- -32768,- -32768,4109,380,5115,-
-32768,803,335,- -32768,- -32768,810,812,1350,- -32768,418,2754,813,5474,- -32768,
815,820,- -32768,5876,5876,5876,- -32768,- -32768,2789,4174,148,- -32768,289,- -32768,
- -32768,107,- -32768,- -32768,5474,- -32768,- -32768,- -32768,818,821,823,- -32768,
2718,- -32768,357,336,- -32768,- -32768,- -32768,5809,- -32768,- -32768,- -32768,2401,-
-32768,2401,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,954,5115,828,
826,- -32768,841,326,335,- 58,5115,5115,847,852,5115,931,2017,855,- -32768,- -32768,
- -32768,456,921,- -32768,4271,854,864,870,5115,866,3396,2143,- -32768,- -32768,3026,
- -32768,- -32768,- -32768,- -32768,5809,858,240,- -32768,- -32768,846,- -32768,107,859,
5742,774,- -32768,5474,- -32768,- -32768,372,672,- -32768,- -32768,2277,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,860,867,877,- -32768,380,5115,- -32768,636,
3979,278,- -32768,478,869,871,883,884,496,5809,534,863,885,887,5171,- -32768,-
-32768,889,893,- -32768,571,259,558,548,574,616,114,114,637,637,637,637,258,258,
297,297,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,898,- -32768,123,5474,
3785,- -32768,754,- -32768,- -32768,- -32768,1350,- -32768,- -32768,888,904,- -32768,97,
878,- -32768,- -32768,- -32768,- -32768,900,774,905,372,896,289,3303,5474,4336,-
-32768,- -32768,- 25,516,906,22,2905,908,335,3236,5474,4433,- -32768,357,- -32768,-
-32768,- -32768,998,- -32768,- -32768,903,5115,774,- 2,909,541,575,5115,809,588,910,
4530,4595,596,- -32768,- -32768,915,920,916,933,3396,907,912,597,911,- -32768,3169,
- -32768,926,5474,- -32768,929,107,- -32768,918,- -32768,228,- -32768,769,- -32768,672,
- -32768,932,- -32768,2401,935,559,928,- -32768,927,- -32768,3591,- -32768,- -32768,-
-32768,- -32768,5115,1023,5115,5115,1109,939,- -32768,- -32768,935,941,3785,- -32768,
- -32768,5115,5115,- -32768,- -32768,28,667,- -32768,- -32768,- -32768,- -32768,3264,-
-32768,- -32768,- -32768,942,- -32768,- -32768,372,948,774,28,372,944,- -32768,602,
949,952,- -32768,- -32768,955,- -32768,956,774,957,28,372,947,- -32768,- -32768,5608,
335,851,3396,965,961,1011,962,969,3396,5115,4660,613,4725,620,4790,809,- -32768,
973,970,- -32768,984,39,977,990,- -32768,774,219,- -32768,- -32768,986,774,5809,-
-32768,499,- -32768,979,981,5115,- -32768,- -32768,- -32768,3979,641,978,988,643,-
-32768,829,- -32768,- -32768,751,- -32768,- -32768,- -32768,- -32768,3785,- -32768,-
-32768,- -32768,- -32768,- -32768,983,- -32768,- -32768,372,1001,- -32768,- -32768,-
-32768,- -32768,- -32768,987,- -32768,- -32768,- -32768,372,995,- -32768,- -32768,-
-32768,997,- 2,657,1085,809,1006,5404,1004,2143,5348,1000,- -32768,108,- -32768,
1036,1002,746,796,128,802,370,401,- -32768,- -32768,- -32768,- -32768,1045,5348,1886,
- -32768,- -32768,658,3396,675,4855,3396,681,4920,4985,690,1020,- -32768,5115,3396,
3396,1029,- -32768,1022,- -32768,- -32768,1027,- -32768,926,- -32768,1033,695,- -32768,
- -32768,290,- -32768,- -32768,5809,1120,- -32768,- -32768,3688,- -32768,- -32768,1038,
- -32768,- -32768,- -32768,- -32768,1034,85,851,3396,1043,809,1312,- -32768,5809,1040,
- -32768,1493,5348,5115,5348,5348,5348,5348,5348,5348,5348,5348,5348,5348,5348,
5348,5348,5348,5348,5348,5348,5115,- -32768,- -32768,1047,- -32768,3396,3396,693,-
-32768,3396,3396,697,3396,707,5050,- -32768,730,- -32768,- -32768,1031,5115,107,774,
- -32768,- -32768,532,5115,1044,1046,- -32768,1058,5809,- -32768,- -32768,- -32768,-
-32768,1050,1053,1057,5348,809,- -32768,571,302,558,548,548,616,114,114,637,637,
637,637,258,258,297,297,- -32768,- -32768,- -32768,315,- -32768,- -32768,- -32768,3396,
- -32768,- -32768,3396,- -32768,3396,3396,734,- -32768,1071,111,- -32768,- -32768,-
-32768,771,1056,5809,1059,740,- -32768,935,567,- -32768,- -32768,5115,1493,- -32768,
- -32768,- -32768,- -32768,3396,3396,3396,- -32768,- -32768,1060,- -32768,- -32768,1061,
1062,- -32768,809,- -32768,- -32768,- -32768,- -32768,1063,- -32768,- -32768,- -32768,-
-32768,1170,1178,- -32768};static short Cyc_yypgoto[142]={- -32768,116,- -32768,264,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 51,- 105,17,- -32768,-
-32768,0,601,- -32768,68,- 142,1082,30,- -32768,- -32768,- 117,- -32768,18,- -32768,480,
- -32768,- -32768,- -32768,950,951,199,473,- -32768,- -32768,591,- -32768,- -32768,-
-32768,23,- -32768,- -32768,26,- 179,1126,- 307,159,- -32768,- -32768,1148,58,- -32768,
- -32768,554,- 118,- 115,- 131,- 219,282,561,566,- 393,- 412,- 104,- 364,- 100,- -32768,-
203,- 136,- 549,- 310,- -32768,691,- 150,101,- 99,- 125,- 200,318,- -32768,- -32768,-
-32768,- 57,- 261,- -32768,- 518,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,91,992,- -32768,523,- -32768,- -32768,179,599,- -32768,- 156,- 353,- 126,
- 320,- 313,- 327,872,- 311,- 323,- 302,- 316,- 301,- 149,604,489,- -32768,- -32768,- 333,-
-32768,146,487,- 55,1143,- 351,20};static short Cyc_yytable[5992]={54,230,309,673,
287,125,134,465,466,580,468,529,131,240,249,526,546,286,532,533,549,54,327,312,
528,330,331,332,296,531,57,538,539,313,302,289,428,548,73,408,102,534,535,536,
537,297,366,54,540,541,414,57,400,54,54,54,593,54,144,400,602,241,630,54,138,519,
103,131,55,391,119,120,121,558,122,281,373,57,118,269,133,280,597,57,57,57,633,
57,398,55,786,787,405,57,387,34,54,54,71,401,402,154,37,374,417,400,401,402,627,
592,54,54,54,54,89,55,758,593,54,409,155,55,55,55,617,55,57,57,226,93,54,55,54,
592,333,133,832,334,484,138,57,57,57,57,246,493,54,270,57,133,648,647,401,680,94,
833,498,482,499,282,57,335,57,109,55,55,458,473,153,90,475,95,517,518,419,35,57,
202,55,55,55,55,492,431,320,459,55,247,460,280,419,873,343,344,- 142,215,216,575,
55,219,55,336,542,543,544,828,881,882,989,35,679,390,217,218,55,474,575,269,269,
96,269,333,402,90,383,140,233,234,235,236,345,874,90,333,586,98,404,97,346,406,
54,392,35,671,883,303,35,706,391,391,391,34,884,386,294,586,470,133,37,333,395,
180,298,636,282,34,40,396,54,54,57,54,37,712,865,269,456,99,220,493,40,457,367,
368,369,270,270,475,270,43,44,45,110,111,744,489,467,154,57,57,90,57,34,651,229,
34,304,- 222,55,37,- 222,485,37,74,39,206,155,490,305,207,40,370,245,112,113,471,
54,371,372,43,44,45,837,100,229,285,55,55,101,55,749,- 200,35,270,402,- 200,90,493,
928,629,486,328,682,90,933,105,57,106,587,387,230,419,625,34,475,660,104,39,558,
333,37,666,397,135,147,840,669,136,444,419,137,445,707,349,382,419,200,350,416,
652,390,390,390,552,55,39,502,503,611,579,333,576,127,128,440,577,697,918,578,
389,626,394,333,585,980,591,131,697,351,981,392,392,392,34,54,333,54,352,353,36,
37,38,982,34,596,141,142,454,434,36,37,38,245,333,493,687,407,748,413,54,387,144,
672,749,57,396,57,700,686,601,603,201,420,39,999,421,133,54,769,588,699,555,285,
589,559,481,590,221,688,57,1006,779,146,222,133,328,564,69,701,151,702,54,55,509,
55,887,733,57,514,888,516,203,328,204,738,333,581,205,521,75,91,225,92,760,836,
527,475,55,639,839,633,57,889,754,227,556,90,69,333,90,587,890,891,755,69,55,43,
44,45,69,594,419,595,749,419,638,847,124,237,937,938,550,69,934,397,894,940,941,
238,55,748,150,936,614,91,299,333,939,732,946,947,448,416,91,280,579,300,455,308,
942,943,944,945,69,69,637,948,949,333,69,301,654,591,440,229,314,34,69,69,69,69,
691,36,37,38,315,333,642,841,659,34,932,229,692,693,694,36,37,38,229,69,123,604,
605,316,54,608,996,612,34,317,684,318,69,319,36,37,38,150,321,622,91,133,54,333,
675,870,661,871,676,748,333,34,337,710,57,221,743,36,37,38,322,222,229,221,995,
893,413,329,639,222,229,395,57,396,339,43,44,45,396,229,43,44,45,338,333,125,91,
711,16,17,18,761,55,91,969,341,342,333,919,90,716,340,722,90,232,333,333,770,914,
730,55,773,347,348,774,418,929,375,930,822,998,781,333,34,69,785,825,380,734,333,
37,418,385,114,442,293,381,446,40,950,951,952,767,440,649,650,771,43,44,45,333,
76,333,848,440,851,69,440,742,307,782,310,310,862,269,412,863,333,559,422,895,
423,324,325,229,762,763,310,310,310,624,979,425,389,333,150,229,897,719,721,333,
76,432,901,441,77,906,475,310,333,443,916,333,814,917,957,333,376,450,960,152,79,
447,80,81,54,333,37,975,962,269,449,992,90,997,82,451,83,84,855,713,714,270,750,
85,77,753,430,452,333,858,86,965,333,461,57,987,674,464,420,152,79,994,80,81,786,
787,37,90,852,853,125,567,568,569,82,91,83,84,91,462,54,877,878,85,879,880,885,
886,990,333,86,463,55,924,925,469,239,270,471,479,478,483,402,487,819,821,39,824,
418,827,57,418,491,310,245,495,496,497,430,69,335,69,510,511,512,513,515,551,520,
418,554,553,846,565,563,418,522,566,582,269,525,584,583,598,599,727,229,310,55,
310,310,310,310,310,310,310,310,310,310,310,310,310,310,310,310,310,600,606,607,
609,613,615,545,618,620,621,623,522,628,631,634,644,646,180,662,645,69,655,35,
656,54,354,355,356,357,358,359,360,361,362,363,657,658,574,664,677,663,270,34,
899,667,668,903,905,36,37,38,908,364,670,57,681,678,434,685,709,683,695,435,698,
717,723,43,44,45,724,791,725,726,728,729,285,818,731,794,795,735,310,741,91,737,
665,745,91,328,746,751,76,55,34,756,935,757,766,522,36,37,38,39,768,775,522,772,
776,434,783,777,778,780,953,418,792,793,43,44,45,817,816,829,831,830,418,834,964,
418,835,838,708,849,967,844,77,845,34,854,971,850,856,857,36,37,38,859,430,860,
310,152,79,434,80,81,864,866,37,869,872,875,43,44,45,229,796,310,83,84,892,876,
907,69,911,85,912,156,157,797,913,896,86,177,900,915,178,179,920,923,76,922,909,
910,430,927,931,954,966,974,972,973,976,160,161,162,163,164,91,977,978,988,991,
1009,166,993,1003,1004,1005,1007,690,1010,167,168,970,926,169,170,696,228,784,
427,704,705,139,108,740,433,968,739,736,653,384,91,765,815,126,310,522,34,79,171,
80,81,530,0,37,0,955,956,311,0,958,959,172,961,0,173,76,0,0,124,0,174,0,0,0,175,
0,0,176,177,0,0,178,179,0,0,0,0,0,0,430,0,0,0,0,0,0,752,0,0,0,0,0,0,310,430,0,0,
759,0,77,0,0,0,310,0,983,861,0,984,0,985,986,0,0,78,79,0,80,81,0,0,37,0,0,0,0,0,
0,0,82,0,83,84,0,0,1000,1001,1002,85,0,0,0,0,0,0,86,0,0,0,0,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,0,156,157,0,0,0,0,0,0,0,0,0,430,22,76,0,158,0,0,0,159,0,0,
0,0,0,430,160,161,162,163,164,27,28,0,165,0,0,166,0,0,0,0,31,32,0,167,168,33,0,
169,170,76,0,0,0,0,0,0,0,0,0,868,0,0,310,0,0,124,0,0,0,0,34,79,171,80,81,35,36,
37,38,39,310,326,0,0,0,172,0,41,267,0,0,0,0,77,174,0,0,0,175,0,0,176,177,0,0,178,
179,0,152,79,0,80,81,430,0,37,0,0,0,0,0,0,0,82,0,83,84,0,0,0,0,0,85,0,0,0,0,0,
310,86,310,310,310,310,310,310,310,310,310,310,310,310,310,310,310,310,310,2,3,4,
72,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,713,714,21,156,157,250,0,251,252,253,
254,255,256,257,258,22,76,23,158,259,310,0,159,24,260,0,0,0,0,160,161,162,163,
164,27,28,261,165,0,262,166,0,0,0,0,31,32,263,167,168,33,0,169,170,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,310,0,0,0,0,264,79,171,80,81,35,36,37,38,39,265,130,- 293,0,0,
172,0,41,267,0,0,0,0,0,174,0,0,0,175,0,0,176,177,0,0,178,179,2,3,4,72,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,21,156,157,250,0,251,252,253,254,255,256,257,
258,22,76,23,158,259,0,0,159,24,260,0,0,- 138,- 138,160,161,162,163,164,27,28,261,
165,0,262,166,0,0,0,0,31,32,263,167,168,33,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,264,79,171,80,81,35,36,37,38,39,265,130,0,0,0,172,0,41,267,0,0,0,0,
0,174,0,0,0,175,0,0,176,177,0,0,178,179,2,3,4,72,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,156,157,250,0,251,252,253,254,255,256,257,258,22,76,23,158,259,0,
0,159,24,260,0,0,0,0,160,161,162,163,164,27,28,261,165,0,262,166,0,0,0,0,31,32,
263,167,168,33,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,264,79,171,80,
81,35,36,37,38,39,265,130,266,0,0,172,0,41,267,0,0,0,0,0,174,0,0,0,175,0,0,176,
177,0,0,178,179,2,3,4,72,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,156,157,
250,0,251,252,253,254,255,256,257,258,22,76,23,158,259,0,0,159,24,260,0,0,0,0,
160,161,162,163,164,27,28,261,165,0,262,166,0,0,0,0,31,32,263,167,168,33,0,169,
170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,264,79,171,80,81,35,36,37,38,39,
265,130,0,0,0,172,0,41,267,0,0,0,0,0,174,0,0,0,175,0,0,176,177,0,0,178,179,2,3,4,
72,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,156,157,0,0,0,0,0,0,0,0,0,0,
22,76,23,158,0,0,0,159,24,0,0,0,0,0,160,161,162,163,164,27,28,0,165,0,0,166,0,0,
0,0,31,32,0,167,168,33,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,79,
171,80,81,35,36,37,38,39,610,0,0,0,0,172,0,41,267,0,0,0,0,0,174,0,0,0,175,0,0,
176,177,0,0,178,179,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,156,157,0,0,0,0,
0,0,0,0,0,0,22,76,0,158,0,0,0,159,0,0,0,0,0,0,160,161,162,163,164,27,28,0,165,0,
0,166,0,0,0,0,31,32,0,167,168,33,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,34,79,171,80,81,35,36,37,38,39,0,0,0,0,0,172,0,41,267,0,0,0,0,0,174,0,0,0,
175,0,0,176,177,0,0,178,179,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,641,0,24,0,25,26,- 138,- 138,0,
0,0,0,0,27,28,0,0,0,0,0,0,0,29,30,31,32,0,0,0,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,0,0,- 11,0,0,40,0,41,42,0,0,0,0,0,43,44,
45,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,
0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,- 138,- 138,0,0,0,0,0,27,28,0,0,0,0,0,0,0,
29,30,31,32,0,0,0,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,
35,36,37,38,39,0,0,- 11,0,0,40,0,41,42,0,0,0,0,0,43,44,45,- 11,1,0,2,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,
0,24,0,25,26,- 138,- 138,0,0,0,0,0,27,28,0,0,0,0,0,0,0,29,30,31,32,0,0,0,33,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,0,0,0,0,0,40,
0,41,42,0,0,0,0,0,43,44,45,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,- 138,- 138,0,0,0,0,0,
27,28,0,0,0,0,0,0,0,29,30,31,32,0,0,0,33,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,22,0,- 11,0,0,40,0,41,42,0,0,0,0,0,
43,44,45,0,0,27,28,0,0,0,0,0,0,0,0,0,31,32,0,0,0,33,0,76,0,0,0,0,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,283,0,0,34,0,0,0,0,35,36,37,38,39,285,22,0,0,0,413,-
200,41,42,0,- 200,77,396,0,43,44,45,0,0,0,27,28,0,557,0,0,152,79,0,80,81,31,32,37,
0,0,33,0,0,0,0,82,0,83,84,0,0,502,503,0,85,0,0,0,0,0,0,86,283,0,0,0,0,0,0,0,35,
36,0,38,39,285,0,0,0,0,395,- 200,41,42,0,- 200,0,396,0,43,44,45,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,0,- 138,
- 138,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,31,32,0,0,0,33,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,0,0,0,0,0,434,0,41,42,0,0,0,0,0,
43,44,45,2,3,4,72,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,
0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,0,0,- 138,- 138,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,
31,32,0,0,0,33,0,0,2,3,4,72,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,
0,0,0,35,36,0,38,39,0,130,22,0,239,0,0,41,42,0,24,0,0,0,- 138,- 138,0,0,0,0,0,27,
28,0,0,0,0,0,0,0,0,0,31,32,0,0,0,33,0,2,3,4,72,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,0,0,0,0,0,0,35,36,0,38,39,0,22,0,23,0,0,0,41,42,24,0,0,0,0,0,0,0,
0,0,0,27,28,0,0,0,0,0,0,0,0,0,31,32,0,0,0,33,0,0,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,35,36,22,38,39,0,130,0,0,0,0,0,41,42,0,0,0,0,
0,0,0,27,28,0,0,0,0,0,0,0,0,76,31,32,0,0,0,33,0,0,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,0,0,0,283,0,0,284,0,0,0,0,35,36,22,38,39,285,0,0,77,0,0,- 200,41,42,
0,- 200,0,0,0,0,764,27,28,152,79,0,80,81,0,0,37,0,31,32,0,0,0,33,82,0,83,84,0,0,
502,503,0,85,0,0,0,0,0,0,86,0,0,0,0,283,0,0,0,0,0,0,0,35,36,0,38,39,285,0,0,0,0,
0,- 200,41,42,0,- 200,156,157,250,0,251,252,253,254,255,256,257,258,0,76,0,158,259,
0,0,159,0,260,0,0,0,0,160,161,162,163,164,0,0,261,165,0,262,166,0,0,0,0,0,0,263,
167,168,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,264,79,171,80,81,0,
0,37,0,0,265,130,0,0,0,172,0,0,173,0,0,0,0,0,174,0,156,157,175,0,0,176,177,500,0,
178,179,0,0,76,0,158,0,0,0,159,0,0,0,0,0,0,160,161,162,163,164,0,0,0,165,0,0,166,
0,0,0,0,0,0,0,167,168,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,
79,171,80,81,0,0,37,0,0,0,311,501,0,0,172,0,0,173,0,0,502,503,0,174,156,157,0,
175,0,0,176,177,0,0,178,179,0,76,0,158,0,0,0,159,0,0,0,0,0,0,160,161,162,163,164,
0,0,0,165,0,0,166,0,0,0,0,0,0,0,167,168,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,34,79,171,80,81,0,0,37,0,0,0,311,747,0,0,172,0,0,173,0,0,502,503,0,
174,156,157,0,175,0,0,176,177,0,0,178,179,0,76,0,158,0,0,0,159,0,0,0,0,0,0,160,
161,162,163,164,0,0,0,165,0,0,166,0,0,0,0,0,0,0,167,168,0,0,169,170,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,79,171,80,81,0,0,37,0,0,0,311,921,0,0,172,0,0,
173,0,0,502,503,0,174,156,157,0,175,0,0,176,177,0,0,178,179,0,76,0,158,0,0,0,159,
0,0,0,0,0,0,160,161,162,163,164,0,0,0,165,0,0,166,0,0,0,0,0,0,0,167,168,0,0,169,
170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,79,171,80,81,0,0,37,0,0,0,311,0,
0,0,172,0,0,173,0,0,502,503,0,174,156,157,0,175,0,0,176,177,0,0,178,179,0,76,0,
158,0,0,0,159,0,0,0,0,0,0,160,161,162,163,164,0,0,0,165,0,0,166,0,0,0,0,0,0,0,
167,168,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,79,171,80,81,0,
0,37,0,0,0,0,0,0,0,172,0,0,173,0,0,0,0,292,174,156,157,0,175,0,0,176,177,0,0,178,
179,0,76,0,158,0,0,0,159,0,0,0,0,0,0,160,161,162,163,164,0,0,0,165,0,0,166,0,0,0,
0,0,0,0,167,168,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,0,0,34,79,
171,80,81,0,76,37,158,0,0,311,159,0,0,172,0,0,173,160,161,162,163,164,174,0,0,
165,175,0,166,176,177,0,0,178,179,0,167,168,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,156,157,0,0,0,0,0,34,79,171,80,81,0,76,37,158,0,453,0,159,0,0,172,0,0,173,
160,161,162,163,164,174,0,0,165,175,0,166,176,177,0,0,178,179,0,167,168,0,0,169,
170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,0,0,34,79,171,80,81,0,76,37,158,0,
0,0,159,0,0,172,547,0,173,160,161,162,163,164,174,0,0,165,175,0,166,176,177,0,0,
178,179,0,167,168,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,79,
171,80,81,0,0,37,0,0,0,0,0,0,0,172,0,0,173,0,0,0,0,573,174,156,157,0,175,0,0,176,
177,0,0,178,179,0,76,0,158,0,0,0,159,0,0,0,0,0,0,160,161,162,163,164,0,0,0,165,0,
0,166,0,0,0,0,0,0,0,167,168,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,
0,0,0,34,79,171,80,81,0,76,37,158,0,0,0,159,0,0,172,616,0,173,160,161,162,163,
164,174,0,0,165,175,0,166,176,177,0,0,178,179,0,167,168,0,0,169,170,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,79,171,80,81,0,0,37,0,0,0,0,0,0,0,172,0,0,173,0,
0,0,0,689,174,156,157,0,175,0,0,176,177,0,0,178,179,0,76,0,158,0,0,0,159,0,0,0,0,
0,0,160,161,162,163,164,0,0,0,165,0,0,166,0,0,0,0,0,0,0,167,168,0,0,169,170,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,79,171,80,81,0,0,37,0,0,0,0,0,0,0,172,0,
0,173,0,0,0,0,703,174,156,157,0,175,0,0,176,177,0,0,178,179,0,76,0,158,0,0,0,159,
0,0,0,0,0,0,160,161,162,163,164,0,0,0,165,0,0,166,0,0,0,0,0,0,0,167,168,0,0,169,
170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,0,0,34,79,171,80,81,0,76,37,158,0,
718,0,159,0,0,172,0,0,173,160,161,162,163,164,174,0,0,165,175,0,166,176,177,0,0,
178,179,0,167,168,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,0,0,34,
79,171,80,81,0,76,37,158,0,720,0,159,0,0,172,0,0,173,160,161,162,163,164,174,0,0,
165,175,0,166,176,177,0,0,178,179,0,167,168,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,156,157,0,0,0,0,0,34,79,171,80,81,0,76,37,158,0,0,0,159,0,0,172,820,0,173,
160,161,162,163,164,174,0,0,165,175,0,166,176,177,0,0,178,179,0,167,168,0,0,169,
170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,0,0,34,79,171,80,81,0,76,37,158,0,
0,0,159,0,0,172,823,0,173,160,161,162,163,164,174,0,0,165,175,0,166,176,177,0,0,
178,179,0,167,168,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,0,0,34,
79,171,80,81,0,76,37,158,0,826,0,159,0,0,172,0,0,173,160,161,162,163,164,174,0,0,
165,175,0,166,176,177,0,0,178,179,0,167,168,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,156,157,0,0,0,0,0,34,79,171,80,81,0,76,37,158,0,0,0,159,0,0,172,898,0,173,
160,161,162,163,164,174,0,0,165,175,0,166,176,177,0,0,178,179,0,167,168,0,0,169,
170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,0,0,34,79,171,80,81,0,76,37,158,0,
0,0,159,0,0,172,902,0,173,160,161,162,163,164,174,0,0,165,175,0,166,176,177,0,0,
178,179,0,167,168,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,0,0,34,
79,171,80,81,0,76,37,158,0,0,0,159,0,0,172,904,0,173,160,161,162,163,164,174,0,0,
165,175,0,166,176,177,0,0,178,179,0,167,168,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,156,157,0,0,0,0,0,34,79,171,80,81,0,76,37,158,0,0,0,159,0,0,172,963,0,173,
160,161,162,163,164,174,0,0,165,175,0,166,176,177,0,0,178,179,0,167,168,0,0,169,
170,0,0,0,0,0,156,157,0,0,0,0,0,0,0,0,0,0,0,76,0,0,34,79,171,80,81,0,0,37,0,0,
160,161,162,163,164,172,0,0,173,0,0,166,0,0,174,0,0,0,175,167,168,176,177,169,
170,178,179,0,0,0,0,0,0,0,0,0,0,0,0,156,157,0,0,0,0,0,34,79,171,80,81,0,76,37,0,
0,0,665,0,0,0,172,0,0,173,160,161,162,163,164,174,0,0,0,175,0,166,176,177,0,0,
178,179,0,167,168,0,0,169,170,0,0,0,0,0,156,157,0,0,0,0,0,0,0,0,0,0,0,76,0,0,34,
79,171,80,81,0,0,37,0,0,160,161,162,163,164,306,0,0,173,0,0,166,0,0,174,0,0,0,
175,167,168,176,177,169,170,178,179,0,0,0,156,157,0,0,0,0,0,0,0,0,0,0,0,76,0,0,
34,79,171,80,81,0,0,37,0,0,160,161,162,163,164,323,0,0,173,0,0,166,0,0,174,0,0,0,
175,167,168,176,177,169,170,178,179,0,0,0,156,157,0,0,0,0,0,0,0,0,0,0,0,76,0,0,
34,79,171,80,81,0,0,37,0,0,160,161,162,163,164,172,0,0,173,0,0,166,0,0,174,0,0,0,
175,167,168,176,177,169,170,178,179,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,
0,0,34,79,171,80,81,0,0,37,0,0,22,0,0,0,0,867,0,0,173,0,0,0,0,0,174,0,0,0,175,27,
28,176,177,208,0,178,179,0,0,0,31,32,0,0,0,33,0,0,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,35,36,22,38,39,0,209,0,0,0,0,0,41,42,- 138,-
138,0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,31,32,0,0,0,33,0,0,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,35,36,22,38,39,0,0,0,0,0,0,0,41,42,0,0,
0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,31,32,0,0,0,33,0,0,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,35,36,22,38,39,0,0,- 106,0,0,0,0,41,42,0,0,
0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,31,32,0,0,0,33,0,0,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,35,36,22,38,39,0,0,- 107,0,0,0,0,41,42,0,0,
0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,31,32,0,0,0,33,0,0,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,0,0,635,0,0,0,0,0,0,0,35,36,22,38,39,0,0,0,0,0,0,0,41,42,0,0,0,
0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,31,32,0,0,0,33,0,0,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,35,36,22,38,39,0,0,0,0,0,0,0,41,42,- 138,- 138,
0,0,0,0,0,27,28,0,0,0,0,0,0,0,0,0,31,32,0,0,0,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,35,0,0,0,39,0,0,0,0,0,0,0,41,42};static short Cyc_yycheck[
5992]={0,105,158,552,135,60,63,268,269,402,271,338,63,118,129,335,367,135,341,
342,371,21,172,159,337,174,175,176,143,340,0,347,348,159,151,135,239,370,21,17,
40,343,344,345,346,144,195,47,349,350,229,21,77,53,54,55,420,57,116,77,118,118,
474,63,64,326,40,118,0,211,53,54,55,383,57,132,95,47,52,130,63,132,435,53,54,55,
479,57,213,21,5,6,110,63,209,97,96,97,99,124,125,97,104,122,229,77,124,125,472,
416,110,111,112,113,23,47,665,481,118,97,116,53,54,55,457,57,96,97,102,97,130,63,
132,440,110,118,97,113,110,139,110,111,112,113,126,301,146,130,118,132,503,502,
124,565,116,116,306,288,308,133,130,83,132,47,96,97,97,60,77,23,285,116,322,323,
229,102,146,86,110,111,112,113,299,240,166,116,118,114,119,240,245,83,78,79,109,
94,95,397,130,98,132,123,351,352,353,723,78,79,97,102,113,211,96,97,146,108,416,
268,269,107,271,110,125,77,206,66,110,111,112,113,116,123,86,110,413,116,216,107,
124,219,240,211,102,120,116,154,102,600,390,391,392,97,124,109,114,434,107,240,
104,110,112,82,146,482,238,97,112,119,268,269,240,271,104,606,792,326,107,112,99,
435,112,112,73,74,75,268,269,402,271,121,122,123,107,108,645,295,270,97,268,269,
154,271,97,506,105,97,102,107,240,104,110,291,104,108,106,108,116,297,114,112,
112,112,123,107,108,97,326,118,119,121,122,123,113,96,135,107,268,269,112,271,
650,113,102,326,125,117,200,503,866,109,292,172,571,207,872,112,326,108,413,474,
464,416,467,97,479,515,96,106,678,110,104,520,213,112,99,739,117,116,112,434,119,
115,601,125,205,440,112,129,229,111,390,391,392,375,326,106,118,119,449,399,110,
112,49,50,245,116,585,117,119,210,467,212,110,413,932,415,467,596,121,117,390,
391,392,97,424,110,426,130,131,103,104,105,117,97,434,63,64,258,112,103,104,105,
243,110,600,576,113,650,112,449,565,116,551,763,424,119,426,588,576,441,442,97,
110,106,982,113,449,467,687,112,588,380,107,116,383,110,119,97,577,449,998,700,
108,103,467,306,385,0,588,108,589,491,424,314,426,125,627,467,319,129,321,107,
323,110,635,110,405,111,113,22,23,112,25,669,733,336,631,449,488,738,913,491,121,
659,113,107,380,40,110,383,585,130,131,659,47,467,121,122,123,52,424,596,426,853,
599,487,749,60,107,876,877,372,65,873,395,816,879,880,110,491,763,74,875,107,77,
110,110,878,625,885,886,253,413,86,625,575,109,259,112,881,882,883,884,96,97,484,
887,888,110,102,111,113,592,434,395,112,97,110,111,112,113,581,103,104,105,112,
110,491,109,113,97,872,413,97,98,99,103,104,105,420,133,108,443,444,112,625,447,
978,449,97,112,573,112,146,112,103,104,105,151,112,461,154,625,643,110,554,796,
113,797,558,853,110,97,82,113,625,97,98,103,104,105,112,103,464,97,98,815,112,
112,639,103,472,112,643,119,127,121,122,123,119,481,121,122,123,126,110,741,200,
113,17,18,19,672,625,207,914,80,81,110,849,554,113,128,107,558,106,110,110,688,
840,113,643,110,76,77,113,229,867,108,869,107,982,702,110,97,238,708,107,97,628,
110,104,245,112,107,248,137,97,251,112,889,890,891,685,585,109,110,689,121,122,
123,110,38,110,113,596,113,270,599,643,156,703,158,159,107,816,113,110,110,678,
109,113,109,169,170,576,109,110,174,175,176,463,931,109,585,110,299,588,113,610,
611,110,38,109,113,108,82,107,913,195,110,112,107,110,713,110,113,110,94,97,113,
97,98,112,100,101,816,110,104,923,113,872,112,973,678,981,112,107,114,115,772,22,
23,816,655,121,82,658,239,107,110,783,128,113,110,112,816,113,94,112,110,97,98,
113,100,101,5,6,104,713,109,110,917,390,391,392,112,380,114,115,383,116,872,127,
128,121,80,81,76,77,109,110,128,117,816,862,863,109,111,872,97,117,113,113,125,
120,717,718,106,720,413,722,872,416,109,301,707,109,124,124,311,424,83,426,315,
316,317,318,112,116,113,434,110,113,745,110,113,440,329,109,112,982,333,110,113,
107,110,619,739,335,872,337,338,339,340,341,342,343,344,345,346,347,348,349,350,
351,352,353,117,112,108,30,107,42,365,111,102,97,102,370,112,125,113,113,97,796,
113,110,491,110,102,110,982,84,85,86,87,88,89,90,91,92,93,110,110,396,109,109,
113,982,97,822,113,110,825,826,103,104,105,830,111,113,982,113,110,112,120,108,
113,113,117,113,112,108,121,122,123,107,710,113,97,124,120,107,716,124,25,26,109,
435,108,554,124,108,116,558,867,120,25,38,982,97,113,874,113,113,457,103,104,105,
106,113,113,464,120,113,112,120,113,113,113,892,585,108,113,121,122,123,109,117,
107,97,112,596,107,906,599,97,102,602,112,912,113,82,113,97,113,918,110,98,113,
103,104,105,109,506,109,503,97,98,112,100,101,28,108,104,112,117,82,121,122,123,
923,112,520,114,115,82,126,109,643,102,121,111,25,26,125,110,820,128,129,823,109,
132,133,25,112,38,110,831,832,552,109,113,107,124,98,113,112,109,51,52,53,54,55,
678,113,110,97,113,0,62,113,113,113,113,113,578,0,70,71,917,864,74,75,584,104,
707,238,590,599,65,44,639,243,913,635,631,507,207,713,678,713,60,600,606,97,98,
99,100,101,339,- 1,104,- 1,897,898,108,- 1,901,902,112,904,- 1,115,38,- 1,- 1,741,- 1,
121,- 1,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,- 1,- 1,- 1,- 1,- 1,- 1,650,- 1,- 1,- 1,- 1,-
1,- 1,657,- 1,- 1,- 1,- 1,- 1,- 1,659,665,- 1,- 1,668,- 1,82,- 1,- 1,- 1,669,- 1,957,789,- 1,
960,- 1,962,963,- 1,- 1,97,98,- 1,100,101,- 1,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,114,
115,- 1,- 1,987,988,989,121,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,749,37,38,- 1,40,-
1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,763,51,52,53,54,55,56,57,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,67,
68,- 1,70,71,72,- 1,74,75,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,794,- 1,- 1,797,- 1,- 1,917,- 1,
- 1,- 1,- 1,97,98,99,100,101,102,103,104,105,106,815,108,- 1,- 1,- 1,112,- 1,114,115,- 1,
- 1,- 1,- 1,82,121,- 1,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,- 1,97,98,- 1,100,101,853,
- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,114,115,- 1,- 1,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,- 1,873,
128,875,876,877,878,879,880,881,882,883,884,885,886,887,888,889,890,891,3,4,5,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,
35,36,37,38,39,40,41,931,- 1,44,45,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,59,- 1,
61,62,- 1,- 1,- 1,- 1,67,68,69,70,71,72,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,981,- 1,- 1,- 1,- 1,97,98,99,100,101,102,103,104,105,106,107,108,109,- 1,
- 1,112,- 1,114,115,- 1,- 1,- 1,- 1,- 1,121,- 1,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,
33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,50,51,52,53,54,55,56,57,58,59,
- 1,61,62,- 1,- 1,- 1,- 1,67,68,69,70,71,72,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,102,103,104,105,106,107,108,- 1,- 1,
- 1,112,- 1,114,115,- 1,- 1,- 1,- 1,- 1,121,- 1,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,
33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,59,
- 1,61,62,- 1,- 1,- 1,- 1,67,68,69,70,71,72,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,102,103,104,105,106,107,108,109,-
1,- 1,112,- 1,114,115,- 1,- 1,- 1,- 1,- 1,121,- 1,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,
32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,
59,- 1,61,62,- 1,- 1,- 1,- 1,67,68,69,70,71,72,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,102,103,104,105,106,107,108,- 1,
- 1,- 1,112,- 1,114,115,- 1,- 1,- 1,- 1,- 1,121,- 1,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,- 1,
59,- 1,- 1,62,- 1,- 1,- 1,- 1,67,68,- 1,70,71,72,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,102,103,104,105,106,107,- 1,- 1,
- 1,- 1,112,- 1,114,115,- 1,- 1,- 1,- 1,- 1,121,- 1,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,- 1,59,- 1,- 1,
62,- 1,- 1,- 1,- 1,67,68,- 1,70,71,72,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,102,103,104,105,106,- 1,- 1,- 1,- 1,- 1,112,
- 1,114,115,- 1,- 1,- 1,- 1,- 1,121,- 1,- 1,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,0,1,- 1,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,65,66,67,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,102,103,104,105,106,- 1,- 1,109,- 1,- 1,
112,- 1,114,115,- 1,- 1,- 1,- 1,- 1,121,122,123,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,
- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,66,67,68,- 1,
- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
97,- 1,- 1,- 1,- 1,102,103,104,105,106,- 1,- 1,109,- 1,- 1,112,- 1,114,115,- 1,- 1,- 1,- 1,- 1,
121,122,123,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,
- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,66,67,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,102,103,104,
105,106,- 1,- 1,- 1,- 1,- 1,112,- 1,114,115,- 1,- 1,- 1,- 1,- 1,121,122,123,1,- 1,3,4,5,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,65,66,67,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,102,103,104,105,106,37,- 1,109,- 1,- 1,112,-
1,114,115,- 1,- 1,- 1,- 1,- 1,121,122,123,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,
68,- 1,- 1,- 1,72,- 1,38,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,94,-
1,- 1,97,- 1,- 1,- 1,- 1,102,103,104,105,106,107,37,- 1,- 1,- 1,112,113,114,115,- 1,117,
82,119,- 1,121,122,123,- 1,- 1,- 1,56,57,- 1,94,- 1,- 1,97,98,- 1,100,101,67,68,104,- 1,-
1,72,- 1,- 1,- 1,- 1,112,- 1,114,115,- 1,- 1,118,119,- 1,121,- 1,- 1,- 1,- 1,- 1,- 1,128,94,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,105,106,107,- 1,- 1,- 1,- 1,112,113,114,115,- 1,117,- 1,
119,- 1,121,122,123,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,- 1,
56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,102,103,104,105,106,- 1,
- 1,- 1,- 1,- 1,112,- 1,114,115,- 1,- 1,- 1,- 1,- 1,121,122,123,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,
- 1,- 1,- 1,- 1,45,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,
68,- 1,- 1,- 1,72,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,
- 1,- 1,- 1,- 1,- 1,102,103,- 1,105,106,- 1,108,37,- 1,111,- 1,- 1,114,115,- 1,45,- 1,- 1,- 1,
49,50,- 1,- 1,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,3,4,5,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,102,103,-
1,105,106,- 1,37,- 1,39,- 1,- 1,- 1,114,115,45,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,57,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,37,105,106,- 1,108,- 1,- 1,
- 1,- 1,- 1,114,115,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,67,68,- 1,
- 1,- 1,72,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,
97,- 1,- 1,- 1,- 1,102,103,37,105,106,107,- 1,- 1,82,- 1,- 1,113,114,115,- 1,117,- 1,- 1,- 1,
- 1,94,56,57,97,98,- 1,100,101,- 1,- 1,104,- 1,67,68,- 1,- 1,- 1,72,112,- 1,114,115,- 1,- 1,
118,119,- 1,121,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,
- 1,105,106,107,- 1,- 1,- 1,- 1,- 1,113,114,115,- 1,117,25,26,27,- 1,29,30,31,32,33,34,
35,36,- 1,38,- 1,40,41,- 1,- 1,44,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,58,59,- 1,61,
62,- 1,- 1,- 1,- 1,- 1,- 1,69,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,107,108,- 1,- 1,- 1,112,- 1,
- 1,115,- 1,- 1,- 1,- 1,- 1,121,- 1,25,26,125,- 1,- 1,128,129,32,- 1,132,133,- 1,- 1,38,- 1,
40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,- 1,115,- 1,- 1,
118,119,- 1,121,25,26,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,- 1,38,- 1,40,- 1,- 1,- 1,44,
- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,
- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,
99,100,101,- 1,- 1,104,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,- 1,115,- 1,- 1,118,119,- 1,121,
25,26,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,
- 1,104,- 1,- 1,- 1,108,109,- 1,- 1,112,- 1,- 1,115,- 1,- 1,118,119,- 1,121,25,26,- 1,125,- 1,
- 1,128,129,- 1,- 1,132,133,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,
55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,
108,- 1,- 1,- 1,112,- 1,- 1,115,- 1,- 1,118,119,- 1,121,25,26,- 1,125,- 1,- 1,128,129,- 1,- 1,
132,133,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,59,- 1,
- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,-
1,- 1,115,- 1,- 1,- 1,- 1,120,121,25,26,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,- 1,38,- 1,
40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,
- 1,- 1,- 1,97,98,99,100,101,- 1,38,104,40,- 1,- 1,108,44,- 1,- 1,112,- 1,- 1,115,51,52,53,
54,55,121,- 1,- 1,59,125,- 1,62,128,129,- 1,- 1,132,133,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,38,104,
40,- 1,107,- 1,44,- 1,- 1,112,- 1,- 1,115,51,52,53,54,55,121,- 1,- 1,59,125,- 1,62,128,
129,- 1,- 1,132,133,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
25,26,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,38,104,40,- 1,- 1,- 1,44,- 1,- 1,112,113,- 1,
115,51,52,53,54,55,121,- 1,- 1,59,125,- 1,62,128,129,- 1,- 1,132,133,- 1,70,71,- 1,- 1,
74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,
100,101,- 1,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,- 1,115,- 1,- 1,- 1,- 1,120,121,25,26,-
1,125,- 1,- 1,128,129,- 1,- 1,132,133,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,
52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,38,104,
40,- 1,- 1,- 1,44,- 1,- 1,112,113,- 1,115,51,52,53,54,55,121,- 1,- 1,59,125,- 1,62,128,
129,- 1,- 1,132,133,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,- 1,
115,- 1,- 1,- 1,- 1,120,121,25,26,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,- 1,38,- 1,40,- 1,
- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,- 1,115,- 1,- 1,- 1,- 1,120,
121,25,26,- 1,125,- 1,- 1,128,129,- 1,- 1,132,133,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,
- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,59,- 1,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,- 1,74,75,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,
- 1,38,104,40,- 1,107,- 1,44,- 1,- 1,112,- 1,- 1,115,51,52,53,54,55,121,- 1,- 1,59,125,- 1,
62,128,129,- 1,- 1,132,133,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,38,104,40,- 1,107,- 1,44,- 1,- 1,
112,- 1,- 1,115,51,52,53,54,55,121,- 1,- 1,59,125,- 1,62,128,129,- 1,- 1,132,133,- 1,70,
71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,97,
98,99,100,101,- 1,38,104,40,- 1,- 1,- 1,44,- 1,- 1,112,113,- 1,115,51,52,53,54,55,121,-
1,- 1,59,125,- 1,62,128,129,- 1,- 1,132,133,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,38,104,40,- 1,- 1,
- 1,44,- 1,- 1,112,113,- 1,115,51,52,53,54,55,121,- 1,- 1,59,125,- 1,62,128,129,- 1,- 1,
132,133,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,
- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,38,104,40,- 1,107,- 1,44,- 1,- 1,112,- 1,- 1,115,51,52,
53,54,55,121,- 1,- 1,59,125,- 1,62,128,129,- 1,- 1,132,133,- 1,70,71,- 1,- 1,74,75,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,38,
104,40,- 1,- 1,- 1,44,- 1,- 1,112,113,- 1,115,51,52,53,54,55,121,- 1,- 1,59,125,- 1,62,
128,129,- 1,- 1,132,133,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,38,104,40,- 1,- 1,- 1,44,- 1,- 1,112,
113,- 1,115,51,52,53,54,55,121,- 1,- 1,59,125,- 1,62,128,129,- 1,- 1,132,133,- 1,70,71,
- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,97,98,
99,100,101,- 1,38,104,40,- 1,- 1,- 1,44,- 1,- 1,112,113,- 1,115,51,52,53,54,55,121,- 1,-
1,59,125,- 1,62,128,129,- 1,- 1,132,133,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,38,104,40,- 1,- 1,- 1,
44,- 1,- 1,112,113,- 1,115,51,52,53,54,55,121,- 1,- 1,59,125,- 1,62,128,129,- 1,- 1,132,
133,- 1,70,71,- 1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
38,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,51,52,53,54,55,112,- 1,- 1,115,- 1,- 1,62,
- 1,- 1,121,- 1,- 1,- 1,125,70,71,128,129,74,75,132,133,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,38,104,- 1,- 1,- 1,108,- 1,- 1,- 1,112,
- 1,- 1,115,51,52,53,54,55,121,- 1,- 1,- 1,125,- 1,62,128,129,- 1,- 1,132,133,- 1,70,71,-
1,- 1,74,75,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,97,98,
99,100,101,- 1,- 1,104,- 1,- 1,51,52,53,54,55,112,- 1,- 1,115,- 1,- 1,62,- 1,- 1,121,- 1,- 1,
- 1,125,70,71,128,129,74,75,132,133,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,38,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,51,52,53,54,55,112,- 1,- 1,115,- 1,- 1,
62,- 1,- 1,121,- 1,- 1,- 1,125,70,71,128,129,74,75,132,133,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,51,52,53,54,55,
112,- 1,- 1,115,- 1,- 1,62,- 1,- 1,121,- 1,- 1,- 1,125,70,71,128,129,74,75,132,133,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,97,98,99,100,101,- 1,- 1,104,- 1,- 1,
37,- 1,- 1,- 1,- 1,112,- 1,- 1,115,- 1,- 1,- 1,- 1,- 1,121,- 1,- 1,- 1,125,56,57,128,129,60,- 1,
132,133,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,37,105,106,- 1,108,- 1,- 1,- 1,- 1,- 1,
114,115,49,50,- 1,- 1,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,
- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,102,103,37,105,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,57,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,37,105,106,- 1,- 1,109,
- 1,- 1,- 1,- 1,114,115,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,
- 1,- 1,- 1,72,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,102,103,37,105,106,- 1,- 1,109,- 1,- 1,- 1,- 1,114,115,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,37,105,
106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,37,105,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,49,
50,- 1,- 1,- 1,- 1,- 1,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,-
1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115};char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible\000";
char Cyc_Yystack_overflow[21]="\000\000\000\000Yystack_overflow\000";void Cyc_yyerror(
struct _dyneither_ptr);int Cyc_yylex(struct Cyc_Lexing_lexbuf*);static int Cyc_yychar=(
int)'\000';union Cyc_YYSTYPE_union Cyc_yylval=(union Cyc_YYSTYPE_union)((struct Cyc_YYINITIALSVAL_struct){
57});static int Cyc_yynerrs=0;struct _tuple18{struct Cyc_Position_Segment*f1;struct
_tuple1*f2;int f3;};struct _tuple19{struct _dyneither_ptr f1;void*f2;};static char
_tmp420[8]="stdcall";static char _tmp421[6]="cdecl";static char _tmp422[9]="fastcall";
static char _tmp423[9]="noreturn";static char _tmp424[6]="const";static char _tmp425[8]="aligned";
static char _tmp426[7]="packed";static char _tmp427[7]="shared";static char _tmp428[7]="unused";
static char _tmp429[5]="weak";static char _tmp42A[10]="dllimport";static char _tmp42B[
10]="dllexport";static char _tmp42C[23]="no_instrument_function";static char _tmp42D[
12]="constructor";static char _tmp42E[11]="destructor";static char _tmp42F[22]="no_check_memory_usage";
static char _tmp430[5]="pure";struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmpA12(
unsigned int*_tmpA11,unsigned int*_tmpA10,short**_tmpA0F){for(*_tmpA11=0;*
_tmpA11 < *_tmpA10;(*_tmpA11)++){(*_tmpA0F)[*_tmpA11]=(short)0;}}static void
_tmpA17(unsigned int*_tmpA16,unsigned int*_tmpA15,union Cyc_YYSTYPE_union**
_tmpA14){for(*_tmpA16=0;*_tmpA16 < *_tmpA15;(*_tmpA16)++){(*_tmpA14)[*_tmpA16]=
Cyc_yylval;}}static void _tmpA1C(unsigned int*_tmpA1B,unsigned int*_tmpA1A,struct
Cyc_Yyltype**_tmpA19){for(*_tmpA1B=0;*_tmpA1B < *_tmpA1A;(*_tmpA1B)++){(*_tmpA19)[*
_tmpA1B]=Cyc_yynewloc();}}static void _tmp1345(unsigned int*_tmp1344,unsigned int*
_tmp1343,char**_tmp1341){for(*_tmp1344=0;*_tmp1344 < *_tmp1343;(*_tmp1344)++){(*
_tmp1341)[*_tmp1344]='\000';}}int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){
struct _RegionHandle _tmp380=_new_region("yyregion");struct _RegionHandle*yyregion=&
_tmp380;_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;
int yyssp_offset;unsigned int _tmpA11;unsigned int _tmpA10;short*_tmpA0F;
unsigned int _tmpA0E;short*yyss=(short*)((_tmpA0E=10000,((_tmpA0F=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmpA0E)),((((_tmpA10=_tmpA0E,
_tmpA12(& _tmpA11,& _tmpA10,& _tmpA0F))),_tmpA0F))))));int yyvsp_offset;unsigned int
_tmpA16;unsigned int _tmpA15;union Cyc_YYSTYPE_union*_tmpA14;unsigned int _tmpA13;
union Cyc_YYSTYPE_union*yyvs=(union Cyc_YYSTYPE_union*)((_tmpA13=10000,((_tmpA14=(
union Cyc_YYSTYPE_union*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE_union),
_tmpA13)),((((_tmpA15=_tmpA13,_tmpA17(& _tmpA16,& _tmpA15,& _tmpA14))),_tmpA14))))));
int yylsp_offset;unsigned int _tmpA1B;unsigned int _tmpA1A;struct Cyc_Yyltype*
_tmpA19;unsigned int _tmpA18;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)((_tmpA18=
10000,((_tmpA19=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(
struct Cyc_Yyltype),_tmpA18)),((((_tmpA1A=_tmpA18,_tmpA1C(& _tmpA1B,& _tmpA1A,&
_tmpA19))),_tmpA19))))));int yystacksize=10000;union Cyc_YYSTYPE_union yyval=Cyc_yylval;
int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;
yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(
10000,++ yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(
yystacksize >= 10000){{const char*_tmpA1D;Cyc_yyerror(((_tmpA1D="parser stack overflow",
_tag_dyneither(_tmpA1D,sizeof(char),22))));}(int)_throw((void*)Cyc_Yystack_overflow);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1011,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 361?(int)Cyc_yytranslate[_check_known_subscript_notnull(362,Cyc_yychar)]:
276;}yyn +=yychar1;if((yyn < 0  || yyn > 5991) || Cyc_yycheck[
_check_known_subscript_notnull(5992,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(5992,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1010){int
_tmp382=0;_npop_handler(0);return _tmp382;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)-- yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1011,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(500,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL1E8: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[yyvsp_offset]);break;case 2: _LL1E9: {struct Cyc_List_List*x;
struct Cyc_List_List*y;{struct _handler_cons _tmp383;_push_handler(& _tmp383);{int
_tmp385=0;if(setjmp(_tmp383.handler))_tmp385=1;if(!_tmp385){x=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);;_pop_handler();}
else{void*_tmp384=(void*)_exn_thrown;void*_tmp387=_tmp384;_LL1EC: if(*((void**)
_tmp387)!= Cyc_Core_Failure)goto _LL1EE;_LL1ED: x=0;goto _LL1EB;_LL1EE:;_LL1EF:(
void)_throw(_tmp387);_LL1EB:;}}}{struct _handler_cons _tmp388;_push_handler(&
_tmp388);{int _tmp38A=0;if(setjmp(_tmp388.handler))_tmp38A=1;if(!_tmp38A){y=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp389=(void*)_exn_thrown;void*_tmp38C=_tmp389;_LL1F1: if(*((void**)_tmp38C)
!= Cyc_Core_Failure)goto _LL1F3;_LL1F2: y=0;goto _LL1F0;_LL1F3:;_LL1F4:(void)_throw(
_tmp38C);_LL1F0:;}}}{union Cyc_YYSTYPE_union _tmpA1E;yyval=(union Cyc_YYSTYPE_union)(((
_tmpA1E.YY18).tag=22,(((_tmpA1E.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y),_tmpA1E))));}break;}case 3:
_LL1EA:{struct Cyc_List_List*_tmpA2D;struct Cyc_Absyn_Using_d_struct*_tmpA2C;
struct Cyc_Absyn_Using_d_struct _tmpA2B;struct Cyc_Absyn_Decl*_tmpA2A;union Cyc_YYSTYPE_union
_tmpA29;yyval=(union Cyc_YYSTYPE_union)(((_tmpA29.YY18).tag=22,(((_tmpA29.YY18).f1=((
_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D->hd=((_tmpA2A=_cycalloc(sizeof(*
_tmpA2A)),((_tmpA2A->r=(void*)((void*)((_tmpA2C=_cycalloc(sizeof(*_tmpA2C)),((
_tmpA2C[0]=((_tmpA2B.tag=9,((_tmpA2B.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA2B.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA2B)))))),_tmpA2C))))),((
_tmpA2A->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpA2A)))))),((_tmpA2D->tl=0,_tmpA2D)))))),_tmpA29))));}
Cyc_Lex_leave_using();break;case 4: _LL1F5:{struct Cyc_List_List*_tmpA3C;struct Cyc_Absyn_Using_d_struct*
_tmpA3B;struct Cyc_Absyn_Using_d_struct _tmpA3A;struct Cyc_Absyn_Decl*_tmpA39;union
Cyc_YYSTYPE_union _tmpA38;yyval=(union Cyc_YYSTYPE_union)(((_tmpA38.YY18).tag=22,(((
_tmpA38.YY18).f1=((_tmpA3C=_cycalloc(sizeof(*_tmpA3C)),((_tmpA3C->hd=((_tmpA39=
_cycalloc(sizeof(*_tmpA39)),((_tmpA39->r=(void*)((void*)((_tmpA3B=_cycalloc(
sizeof(*_tmpA3B)),((_tmpA3B[0]=((_tmpA3A.tag=9,((_tmpA3A.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpA3A.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA3A)))))),
_tmpA3B))))),((_tmpA39->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA39)))))),((
_tmpA3C->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA3C)))))),_tmpA38))));}break;case 5: _LL1F6:{struct Cyc_List_List*
_tmpA51;struct Cyc_Absyn_Namespace_d_struct*_tmpA50;struct _dyneither_ptr*_tmpA4F;
struct Cyc_Absyn_Namespace_d_struct _tmpA4E;struct Cyc_Absyn_Decl*_tmpA4D;union Cyc_YYSTYPE_union
_tmpA4C;yyval=(union Cyc_YYSTYPE_union)(((_tmpA4C.YY18).tag=22,(((_tmpA4C.YY18).f1=((
_tmpA51=_cycalloc(sizeof(*_tmpA51)),((_tmpA51->hd=((_tmpA4D=_cycalloc(sizeof(*
_tmpA4D)),((_tmpA4D->r=(void*)((void*)((_tmpA50=_cycalloc(sizeof(*_tmpA50)),((
_tmpA50[0]=((_tmpA4E.tag=8,((_tmpA4E.f1=((_tmpA4F=_cycalloc(sizeof(*_tmpA4F)),((
_tmpA4F[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),_tmpA4F)))),((_tmpA4E.f2=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA4E)))))),_tmpA50))))),((
_tmpA4D->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpA4D)))))),((_tmpA51->tl=0,_tmpA51)))))),_tmpA4C))));}
Cyc_Lex_leave_namespace();break;case 6: _LL1F7: {struct _dyneither_ptr nspace;struct
Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons _tmp39E;_push_handler(&
_tmp39E);{int _tmp3A0=0;if(setjmp(_tmp39E.handler))_tmp3A0=1;if(!_tmp3A0){nspace=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;
_pop_handler();}else{void*_tmp39F=(void*)_exn_thrown;void*_tmp3A2=_tmp39F;_LL1FA:
if(*((void**)_tmp3A2)!= Cyc_Core_Failure)goto _LL1FC;_LL1FB:{const char*_tmpA52;
nspace=((_tmpA52="",_tag_dyneither(_tmpA52,sizeof(char),1)));}goto _LL1F9;_LL1FC:;
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
_tmpA67;struct Cyc_Absyn_Namespace_d_struct*_tmpA66;struct _dyneither_ptr*_tmpA65;
struct Cyc_Absyn_Namespace_d_struct _tmpA64;struct Cyc_Absyn_Decl*_tmpA63;union Cyc_YYSTYPE_union
_tmpA62;yyval=(union Cyc_YYSTYPE_union)(((_tmpA62.YY18).tag=22,(((_tmpA62.YY18).f1=((
_tmpA67=_cycalloc(sizeof(*_tmpA67)),((_tmpA67->hd=((_tmpA63=_cycalloc(sizeof(*
_tmpA63)),((_tmpA63->r=(void*)((void*)((_tmpA66=_cycalloc(sizeof(*_tmpA66)),((
_tmpA66[0]=((_tmpA64.tag=8,((_tmpA64.f1=((_tmpA65=_cycalloc(sizeof(*_tmpA65)),((
_tmpA65[0]=nspace,_tmpA65)))),((_tmpA64.f2=x,_tmpA64)))))),_tmpA66))))),((
_tmpA63->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmpA63)))))),((_tmpA67->tl=y,_tmpA67)))))),
_tmpA62))));}break;}case 7: _LL1F8:{const char*_tmpA68;if(Cyc_strcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),((_tmpA68="C",_tag_dyneither(_tmpA68,sizeof(char),2))))== 0){
struct Cyc_List_List*_tmpA77;struct Cyc_Absyn_ExternC_d_struct*_tmpA76;struct Cyc_Absyn_ExternC_d_struct
_tmpA75;struct Cyc_Absyn_Decl*_tmpA74;union Cyc_YYSTYPE_union _tmpA73;yyval=(union
Cyc_YYSTYPE_union)(((_tmpA73.YY18).tag=22,(((_tmpA73.YY18).f1=((_tmpA77=
_cycalloc(sizeof(*_tmpA77)),((_tmpA77->hd=((_tmpA74=_cycalloc(sizeof(*_tmpA74)),((
_tmpA74->r=(void*)((void*)((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76[0]=((
_tmpA75.tag=10,((_tmpA75.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),_tmpA75)))),_tmpA76))))),((_tmpA74->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA74)))))),((
_tmpA77->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA77)))))),_tmpA73))));}else{{const char*_tmpA78;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),((_tmpA78="C include",_tag_dyneither(_tmpA78,sizeof(
char),10))))!= 0){const char*_tmpA79;Cyc_Parse_err(((_tmpA79="expecting \"C\" or \"C include\"",
_tag_dyneither(_tmpA79,sizeof(char),29))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));}}{struct Cyc_List_List*
_tmpA88;struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA87;struct Cyc_Absyn_ExternCinclude_d_struct
_tmpA86;struct Cyc_Absyn_Decl*_tmpA85;union Cyc_YYSTYPE_union _tmpA84;yyval=(union
Cyc_YYSTYPE_union)(((_tmpA84.YY18).tag=22,(((_tmpA84.YY18).f1=((_tmpA88=
_cycalloc(sizeof(*_tmpA88)),((_tmpA88->hd=((_tmpA85=_cycalloc(sizeof(*_tmpA85)),((
_tmpA85->r=(void*)((void*)((_tmpA87=_cycalloc(sizeof(*_tmpA87)),((_tmpA87[0]=((
_tmpA86.tag=11,((_tmpA86.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),((_tmpA86.f2=0,_tmpA86)))))),_tmpA87))))),((_tmpA85->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmpA85)))))),((_tmpA88->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA88)))))),_tmpA84))));}}}
break;case 8: _LL208:{const char*_tmpA89;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),((_tmpA89="C include",
_tag_dyneither(_tmpA89,sizeof(char),10))))!= 0){const char*_tmpA8A;Cyc_Parse_err(((
_tmpA8A="expecting \"C include\"",_tag_dyneither(_tmpA8A,sizeof(char),22))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{
struct Cyc_List_List*_tmpA99;struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA98;
struct Cyc_Absyn_ExternCinclude_d_struct _tmpA97;struct Cyc_Absyn_Decl*_tmpA96;
union Cyc_YYSTYPE_union _tmpA95;yyval=(union Cyc_YYSTYPE_union)(((_tmpA95.YY18).tag=
22,(((_tmpA95.YY18).f1=((_tmpA99=_cycalloc(sizeof(*_tmpA99)),((_tmpA99->hd=((
_tmpA96=_cycalloc(sizeof(*_tmpA96)),((_tmpA96->r=(void*)((void*)((_tmpA98=
_cycalloc(sizeof(*_tmpA98)),((_tmpA98[0]=((_tmpA97.tag=11,((_tmpA97.f1=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA97.f2=exs,
_tmpA97)))))),_tmpA98))))),((_tmpA96->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA96)))))),((
_tmpA99->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA99)))))),_tmpA95))));}break;}case 9: _LL209:{struct Cyc_List_List*
_tmpA9F;struct Cyc_Absyn_Decl*_tmpA9E;union Cyc_YYSTYPE_union _tmpA9D;yyval=(union
Cyc_YYSTYPE_union)(((_tmpA9D.YY18).tag=22,(((_tmpA9D.YY18).f1=((_tmpA9F=
_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F->hd=((_tmpA9E=_cycalloc(sizeof(*_tmpA9E)),((
_tmpA9E->r=(void*)((void*)0),((_tmpA9E->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA9E)))))),((
_tmpA9F->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA9F)))))),_tmpA9D))));}break;case 10: _LL20A:{struct Cyc_List_List*
_tmpAA5;struct Cyc_Absyn_Decl*_tmpAA4;union Cyc_YYSTYPE_union _tmpAA3;yyval=(union
Cyc_YYSTYPE_union)(((_tmpAA3.YY18).tag=22,(((_tmpAA3.YY18).f1=((_tmpAA5=
_cycalloc(sizeof(*_tmpAA5)),((_tmpAA5->hd=((_tmpAA4=_cycalloc(sizeof(*_tmpAA4)),((
_tmpAA4->r=(void*)((void*)1),((_tmpAA4->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpAA4)))))),((
_tmpAA5->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpAA5)))))),_tmpAA3))));}break;case 11: _LL20B:{union Cyc_YYSTYPE_union
_tmpAA6;yyval=(union Cyc_YYSTYPE_union)(((_tmpAA6.YY18).tag=22,(((_tmpAA6.YY18).f1=
0,_tmpAA6))));}break;case 12: _LL20C: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 13: _LL20D:{union Cyc_YYSTYPE_union _tmpAA7;yyval=(
union Cyc_YYSTYPE_union)(((_tmpAA7.YY52).tag=56,(((_tmpAA7.YY52).f1=0,_tmpAA7))));}
break;case 14: _LL20E:{struct Cyc_List_List*_tmpAAD;struct _tuple18*_tmpAAC;union Cyc_YYSTYPE_union
_tmpAAB;yyval=(union Cyc_YYSTYPE_union)(((_tmpAAB.YY52).tag=56,(((_tmpAAB.YY52).f1=((
_tmpAAD=_cycalloc(sizeof(*_tmpAAD)),((_tmpAAD->hd=((_tmpAAC=_cycalloc(sizeof(*
_tmpAAC)),((_tmpAAC->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpAAC->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpAAC->f3=0,_tmpAAC)))))))),((_tmpAAD->tl=0,_tmpAAD)))))),_tmpAAB))));}break;
case 15: _LL20F:{struct Cyc_List_List*_tmpAB3;struct _tuple18*_tmpAB2;union Cyc_YYSTYPE_union
_tmpAB1;yyval=(union Cyc_YYSTYPE_union)(((_tmpAB1.YY52).tag=56,(((_tmpAB1.YY52).f1=((
_tmpAB3=_cycalloc(sizeof(*_tmpAB3)),((_tmpAB3->hd=((_tmpAB2=_cycalloc(sizeof(*
_tmpAB2)),((_tmpAB2->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpAB2->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpAB2->f3=0,_tmpAB2)))))))),((_tmpAB3->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAB3)))))),_tmpAB1))));}
break;case 16: _LL210:{struct Cyc_List_List*_tmpABD;struct Cyc_Absyn_Fn_d_struct
_tmpABC;struct Cyc_Absyn_Fn_d_struct*_tmpABB;union Cyc_YYSTYPE_union _tmpABA;yyval=(
union Cyc_YYSTYPE_union)(((_tmpABA.YY18).tag=22,(((_tmpABA.YY18).f1=((_tmpABD=
_cycalloc(sizeof(*_tmpABD)),((_tmpABD->hd=Cyc_Absyn_new_decl((void*)((_tmpABB=
_cycalloc(sizeof(*_tmpABB)),((_tmpABB[0]=((_tmpABC.tag=1,((_tmpABC.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpABC)))),_tmpABB)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpABD->tl=0,_tmpABD)))))),_tmpABA))));}break;
case 17: _LL211: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 18: _LL212:{union Cyc_YYSTYPE_union _tmpABE;yyval=(union Cyc_YYSTYPE_union)(((
_tmpABE.YY18).tag=22,(((_tmpABE.YY18).f1=0,_tmpABE))));}break;case 19: _LL213:{
union Cyc_YYSTYPE_union _tmpABF;yyval=(union Cyc_YYSTYPE_union)(((_tmpABF.YY17).tag=
21,(((_tmpABF.YY17).f1=Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpABF))));}
break;case 20: _LL214:{struct Cyc_Core_Opt*_tmpAC2;union Cyc_YYSTYPE_union _tmpAC1;
yyval=(union Cyc_YYSTYPE_union)(((_tmpAC1.YY17).tag=21,(((_tmpAC1.YY17).f1=Cyc_Parse_make_function(((
_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpAC2)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpAC1))));}
break;case 21: _LL215:{union Cyc_YYSTYPE_union _tmpAC3;yyval=(union Cyc_YYSTYPE_union)(((
_tmpAC3.YY17).tag=21,(((_tmpAC3.YY17).f1=Cyc_Parse_make_function(0,Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpAC3))));}
break;case 22: _LL216:{struct Cyc_Core_Opt*_tmpAC6;union Cyc_YYSTYPE_union _tmpAC5;
yyval=(union Cyc_YYSTYPE_union)(((_tmpAC5.YY17).tag=21,(((_tmpAC5.YY17).f1=Cyc_Parse_make_function(((
_tmpAC6=_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpAC6)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpAC5))));}
break;case 23: _LL217:{struct Cyc_Core_Opt*_tmpAC9;union Cyc_YYSTYPE_union _tmpAC8;
yyval=(union Cyc_YYSTYPE_union)(((_tmpAC8.YY17).tag=21,(((_tmpAC8.YY17).f1=Cyc_Parse_make_function(((
_tmpAC9=_cycalloc(sizeof(*_tmpAC9)),((_tmpAC9->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpAC9)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpAC8))));}
break;case 24: _LL218:{struct Cyc_Core_Opt*_tmpACC;union Cyc_YYSTYPE_union _tmpACB;
yyval=(union Cyc_YYSTYPE_union)(((_tmpACB.YY17).tag=21,(((_tmpACB.YY17).f1=Cyc_Parse_make_function(((
_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpACC)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpACB))));}
break;case 25: _LL219: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL21A: Cyc_Lex_leave_using();
break;case 27: _LL21B:{struct _dyneither_ptr*_tmpACD;Cyc_Lex_enter_namespace(((
_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpACD)))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL21C: Cyc_Lex_leave_namespace();
break;case 29: _LL21D:{union Cyc_YYSTYPE_union _tmpACE;yyval=(union Cyc_YYSTYPE_union)(((
_tmpACE.YY18).tag=22,(((_tmpACE.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmpACE))));}
break;case 30: _LL21E:{union Cyc_YYSTYPE_union _tmpACF;yyval=(union Cyc_YYSTYPE_union)(((
_tmpACF.YY18).tag=22,(((_tmpACF.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpACF))));}
break;case 31: _LL21F:{struct Cyc_List_List*_tmpAD2;union Cyc_YYSTYPE_union _tmpAD1;
yyval=(union Cyc_YYSTYPE_union)(((_tmpAD1.YY18).tag=22,(((_tmpAD1.YY18).f1=((
_tmpAD2=_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAD2->tl=0,
_tmpAD2)))))),_tmpAD1))));}break;case 32: _LL220: {struct Cyc_List_List*_tmp3EA=0;{
struct Cyc_List_List*_tmp3EB=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);for(0;_tmp3EB != 0;_tmp3EB=_tmp3EB->tl){struct
_dyneither_ptr*_tmp3EC=(struct _dyneither_ptr*)_tmp3EB->hd;union Cyc_Absyn_Nmspace_union
_tmpAD5;struct _tuple1*_tmpAD4;struct _tuple1*qv=(_tmpAD4=_cycalloc(sizeof(*
_tmpAD4)),((_tmpAD4->f1=(union Cyc_Absyn_Nmspace_union)(((_tmpAD5.Rel_n).tag=1,(((
_tmpAD5.Rel_n).f1=0,_tmpAD5)))),((_tmpAD4->f2=_tmp3EC,_tmpAD4)))));struct Cyc_Absyn_Vardecl*
_tmp3ED=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);struct Cyc_List_List*
_tmpAD6;_tmp3EA=((_tmpAD6=_cycalloc(sizeof(*_tmpAD6)),((_tmpAD6->hd=_tmp3ED,((
_tmpAD6->tl=_tmp3EA,_tmpAD6))))));}}_tmp3EA=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp3EA);{struct Cyc_List_List*_tmpAD9;union Cyc_YYSTYPE_union
_tmpAD8;yyval=(union Cyc_YYSTYPE_union)(((_tmpAD8.YY18).tag=22,(((_tmpAD8.YY18).f1=((
_tmpAD9=_cycalloc(sizeof(*_tmpAD9)),((_tmpAD9->hd=Cyc_Absyn_letv_decl(_tmp3EA,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpAD9->tl=0,_tmpAD9)))))),_tmpAD8))));}break;}
case 33: _LL221: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 34: _LL222:{union Cyc_YYSTYPE_union _tmpADA;yyval=(union Cyc_YYSTYPE_union)(((
_tmpADA.YY18).tag=22,(((_tmpADA.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpADA))));}break;case 35:
_LL223:{struct Cyc_Parse_Declaration_spec*_tmpAE0;struct Cyc_Core_Opt*_tmpADF;
union Cyc_YYSTYPE_union _tmpADE;yyval=(union Cyc_YYSTYPE_union)(((_tmpADE.YY19).tag=
23,(((_tmpADE.YY19).f1=((_tmpAE0=_cycalloc(sizeof(*_tmpAE0)),((_tmpAE0->sc=((
_tmpADF=_cycalloc(sizeof(*_tmpADF)),((_tmpADF->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpADF)))),((_tmpAE0->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAE0->type_specs=
0,((_tmpAE0->is_inline=0,((_tmpAE0->attributes=0,_tmpAE0)))))))))))),_tmpADE))));}
break;case 36: _LL224: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc != 0){const char*_tmpAE3;void*_tmpAE2;(_tmpAE2=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpAE3="Only one storage class is allowed in a declaration",
_tag_dyneither(_tmpAE3,sizeof(char),51))),_tag_dyneither(_tmpAE2,sizeof(void*),0)));}{
struct Cyc_Parse_Declaration_spec*_tmpAE9;struct Cyc_Core_Opt*_tmpAE8;union Cyc_YYSTYPE_union
_tmpAE7;yyval=(union Cyc_YYSTYPE_union)(((_tmpAE7.YY19).tag=23,(((_tmpAE7.YY19).f1=((
_tmpAE9=_cycalloc(sizeof(*_tmpAE9)),((_tmpAE9->sc=((_tmpAE8=_cycalloc(sizeof(*
_tmpAE8)),((_tmpAE8->v=(void*)Cyc_yyget_YY22(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpAE8)))),((_tmpAE9->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((_tmpAE9->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpAE9->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpAE9->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpAE9)))))))))))),
_tmpAE7))));}break;case 37: _LL225:{struct Cyc_Parse_Declaration_spec*_tmpAEF;
struct Cyc_List_List*_tmpAEE;union Cyc_YYSTYPE_union _tmpAED;yyval=(union Cyc_YYSTYPE_union)(((
_tmpAED.YY19).tag=23,(((_tmpAED.YY19).f1=((_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((
_tmpAEF->sc=0,((_tmpAEF->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAEF->type_specs=((
_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAEE->tl=0,_tmpAEE)))))),((
_tmpAEF->is_inline=0,((_tmpAEF->attributes=0,_tmpAEF)))))))))))),_tmpAED))));}
break;case 38: _LL226:{struct Cyc_Parse_Declaration_spec*_tmpAF5;struct Cyc_List_List*
_tmpAF4;union Cyc_YYSTYPE_union _tmpAF3;yyval=(union Cyc_YYSTYPE_union)(((_tmpAF3.YY19).tag=
23,(((_tmpAF3.YY19).f1=((_tmpAF5=_cycalloc(sizeof(*_tmpAF5)),((_tmpAF5->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAF5->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAF5->type_specs=((_tmpAF4=_cycalloc(sizeof(*_tmpAF4)),((_tmpAF4->hd=(void*)
Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpAF4->tl=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,_tmpAF4)))))),((_tmpAF5->is_inline=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmpAF5->attributes=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,
_tmpAF5)))))))))))),_tmpAF3))));}break;case 39: _LL227:{struct Cyc_Parse_Declaration_spec*
_tmpAF8;union Cyc_YYSTYPE_union _tmpAF7;yyval=(union Cyc_YYSTYPE_union)(((_tmpAF7.YY19).tag=
23,(((_tmpAF7.YY19).f1=((_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((_tmpAF8->sc=0,((
_tmpAF8->tq=Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpAF8->type_specs=0,((_tmpAF8->is_inline=0,((_tmpAF8->attributes=
0,_tmpAF8)))))))))))),_tmpAF7))));}break;case 40: _LL228:{struct Cyc_Parse_Declaration_spec*
_tmpAFB;union Cyc_YYSTYPE_union _tmpAFA;yyval=(union Cyc_YYSTYPE_union)(((_tmpAFA.YY19).tag=
23,(((_tmpAFA.YY19).f1=((_tmpAFB=_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAFB->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq),((_tmpAFB->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpAFB->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpAFB->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpAFB)))))))))))),
_tmpAFA))));}break;case 41: _LL229:{struct Cyc_Parse_Declaration_spec*_tmpAFE;union
Cyc_YYSTYPE_union _tmpAFD;yyval=(union Cyc_YYSTYPE_union)(((_tmpAFD.YY19).tag=23,(((
_tmpAFD.YY19).f1=((_tmpAFE=_cycalloc(sizeof(*_tmpAFE)),((_tmpAFE->sc=0,((_tmpAFE->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAFE->type_specs=
0,((_tmpAFE->is_inline=1,((_tmpAFE->attributes=0,_tmpAFE)))))))))))),_tmpAFD))));}
break;case 42: _LL22A:{struct Cyc_Parse_Declaration_spec*_tmpB01;union Cyc_YYSTYPE_union
_tmpB00;yyval=(union Cyc_YYSTYPE_union)(((_tmpB00.YY19).tag=23,(((_tmpB00.YY19).f1=((
_tmpB01=_cycalloc(sizeof(*_tmpB01)),((_tmpB01->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpB01->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((_tmpB01->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpB01->is_inline=1,((_tmpB01->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpB01)))))))))))),
_tmpB00))));}break;case 43: _LL22B:{struct Cyc_Parse_Declaration_spec*_tmpB04;union
Cyc_YYSTYPE_union _tmpB03;yyval=(union Cyc_YYSTYPE_union)(((_tmpB03.YY19).tag=23,(((
_tmpB03.YY19).f1=((_tmpB04=_cycalloc(sizeof(*_tmpB04)),((_tmpB04->sc=0,((_tmpB04->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB04->type_specs=
0,((_tmpB04->is_inline=0,((_tmpB04->attributes=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB04)))))))))))),_tmpB03))));}
break;case 44: _LL22C:{struct Cyc_Parse_Declaration_spec*_tmpB07;union Cyc_YYSTYPE_union
_tmpB06;yyval=(union Cyc_YYSTYPE_union)(((_tmpB06.YY19).tag=23,(((_tmpB06.YY19).f1=((
_tmpB07=_cycalloc(sizeof(*_tmpB07)),((_tmpB07->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpB07->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((_tmpB07->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpB07->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpB07->attributes=Cyc_List_imp_append(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes),_tmpB07)))))))))))),
_tmpB06))));}break;case 45: _LL22D:{union Cyc_YYSTYPE_union _tmpB08;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB08.YY22).tag=26,(((_tmpB08.YY22).f1=(void*)((void*)4),_tmpB08))));}break;
case 46: _LL22E:{union Cyc_YYSTYPE_union _tmpB09;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB09.YY22).tag=26,(((_tmpB09.YY22).f1=(void*)((void*)5),_tmpB09))));}break;
case 47: _LL22F:{union Cyc_YYSTYPE_union _tmpB0A;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB0A.YY22).tag=26,(((_tmpB0A.YY22).f1=(void*)((void*)3),_tmpB0A))));}break;
case 48: _LL230:{union Cyc_YYSTYPE_union _tmpB0B;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB0B.YY22).tag=26,(((_tmpB0B.YY22).f1=(void*)((void*)1),_tmpB0B))));}break;
case 49: _LL231:{const char*_tmpB0C;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB0C="C",
_tag_dyneither(_tmpB0C,sizeof(char),2))))!= 0){const char*_tmpB0D;Cyc_Parse_err(((
_tmpB0D="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpB0D,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}}{union Cyc_YYSTYPE_union _tmpB0E;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB0E.YY22).tag=26,(((_tmpB0E.YY22).f1=(void*)((void*)2),_tmpB0E))));}break;
case 50: _LL232:{union Cyc_YYSTYPE_union _tmpB0F;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB0F.YY22).tag=26,(((_tmpB0F.YY22).f1=(void*)((void*)0),_tmpB0F))));}break;
case 51: _LL233:{union Cyc_YYSTYPE_union _tmpB10;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB10.YY22).tag=26,(((_tmpB10.YY22).f1=(void*)((void*)6),_tmpB10))));}break;
case 52: _LL234:{union Cyc_YYSTYPE_union _tmpB11;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB11.YY45).tag=49,(((_tmpB11.YY45).f1=0,_tmpB11))));}break;case 53: _LL235:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 54:
_LL236: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 55: _LL237:{struct Cyc_List_List*_tmpB14;union Cyc_YYSTYPE_union _tmpB13;yyval=(
union Cyc_YYSTYPE_union)(((_tmpB13.YY45).tag=49,(((_tmpB13.YY45).f1=((_tmpB14=
_cycalloc(sizeof(*_tmpB14)),((_tmpB14->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB14->tl=0,_tmpB14)))))),
_tmpB13))));}break;case 56: _LL238:{struct Cyc_List_List*_tmpB17;union Cyc_YYSTYPE_union
_tmpB16;yyval=(union Cyc_YYSTYPE_union)(((_tmpB16.YY45).tag=49,(((_tmpB16.YY45).f1=((
_tmpB17=_cycalloc(sizeof(*_tmpB17)),((_tmpB17->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB17->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB17)))))),_tmpB16))));}
break;case 57: _LL239: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={1,- 1};
static struct _tuple19 att_map[17]={{{_tmp420,_tmp420,_tmp420 + 8},(void*)0},{{
_tmp421,_tmp421,_tmp421 + 6},(void*)1},{{_tmp422,_tmp422,_tmp422 + 9},(void*)2},{{
_tmp423,_tmp423,_tmp423 + 9},(void*)3},{{_tmp424,_tmp424,_tmp424 + 6},(void*)4},{{
_tmp425,_tmp425,_tmp425 + 8},(void*)& att_aligned},{{_tmp426,_tmp426,_tmp426 + 7},(
void*)5},{{_tmp427,_tmp427,_tmp427 + 7},(void*)7},{{_tmp428,_tmp428,_tmp428 + 7},(
void*)8},{{_tmp429,_tmp429,_tmp429 + 5},(void*)9},{{_tmp42A,_tmp42A,_tmp42A + 10},(
void*)10},{{_tmp42B,_tmp42B,_tmp42B + 10},(void*)11},{{_tmp42C,_tmp42C,_tmp42C + 
23},(void*)12},{{_tmp42D,_tmp42D,_tmp42D + 12},(void*)13},{{_tmp42E,_tmp42E,
_tmp42E + 11},(void*)14},{{_tmp42F,_tmp42F,_tmp42F + 22},(void*)15},{{_tmp430,
_tmp430,_tmp430 + 5},(void*)16}};struct _dyneither_ptr _tmp41C=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((
_get_dyneither_size(_tmp41C,sizeof(char))> 4  && *((const char*)
_check_dyneither_subscript(_tmp41C,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp41C,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp41C,sizeof(char),(int)(_get_dyneither_size(
_tmp41C,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp41C,sizeof(char),(int)(_get_dyneither_size(_tmp41C,sizeof(char))- 3)))== '_')
_tmp41C=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp41C,2,
_get_dyneither_size(_tmp41C,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp41C,(struct _dyneither_ptr)(att_map[i]).f1)== 0){{union
Cyc_YYSTYPE_union _tmpB18;yyval=(union Cyc_YYSTYPE_union)(((_tmpB18.YY46).tag=50,(((
_tmpB18.YY46).f1=(void*)(att_map[i]).f2,_tmpB18))));}break;}}if(i == 17){{const
char*_tmpB19;Cyc_Parse_err(((_tmpB19="unrecognized attribute",_tag_dyneither(
_tmpB19,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}{union Cyc_YYSTYPE_union
_tmpB1A;yyval=(union Cyc_YYSTYPE_union)(((_tmpB1A.YY46).tag=50,(((_tmpB1A.YY46).f1=(
void*)((void*)1),_tmpB1A))));}}break;}}case 58: _LL23A:{union Cyc_YYSTYPE_union
_tmpB1B;yyval=(union Cyc_YYSTYPE_union)(((_tmpB1B.YY46).tag=50,(((_tmpB1B.YY46).f1=(
void*)((void*)4),_tmpB1B))));}break;case 59: _LL23B: {struct _dyneither_ptr _tmp433=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
int _tmp435;struct _tuple11 _tmp434=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp435=_tmp434.f2;{void*
a;{const char*_tmpB1D;const char*_tmpB1C;if(Cyc_zstrcmp((struct _dyneither_ptr)
_tmp433,((_tmpB1D="regparm",_tag_dyneither(_tmpB1D,sizeof(char),8))))== 0  || Cyc_zstrcmp((
struct _dyneither_ptr)_tmp433,((_tmpB1C="__regparm__",_tag_dyneither(_tmpB1C,
sizeof(char),12))))== 0){if(_tmp435 < 0  || _tmp435 > 3){const char*_tmpB1E;Cyc_Parse_err(((
_tmpB1E="regparm requires value between 0 and 3",_tag_dyneither(_tmpB1E,sizeof(
char),39))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Regparm_att_struct _tmpB21;
struct Cyc_Absyn_Regparm_att_struct*_tmpB20;a=(void*)((_tmpB20=_cycalloc_atomic(
sizeof(*_tmpB20)),((_tmpB20[0]=((_tmpB21.tag=0,((_tmpB21.f1=_tmp435,_tmpB21)))),
_tmpB20))));}}else{const char*_tmpB23;const char*_tmpB22;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp433,((_tmpB23="aligned",_tag_dyneither(_tmpB23,sizeof(char),8))))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp433,((_tmpB22="__aligned__",
_tag_dyneither(_tmpB22,sizeof(char),12))))== 0){if(_tmp435 < 0){const char*_tmpB24;
Cyc_Parse_err(((_tmpB24="aligned requires positive power of two",_tag_dyneither(
_tmpB24,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{
unsigned int j=(unsigned int)_tmp435;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){
const char*_tmpB25;Cyc_Parse_err(((_tmpB25="aligned requires positive power of two",
_tag_dyneither(_tmpB25,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Aligned_att_struct
_tmpB28;struct Cyc_Absyn_Aligned_att_struct*_tmpB27;a=(void*)((_tmpB27=
_cycalloc_atomic(sizeof(*_tmpB27)),((_tmpB27[0]=((_tmpB28.tag=1,((_tmpB28.f1=
_tmp435,_tmpB28)))),_tmpB27))));}}}else{const char*_tmpB2A;const char*_tmpB29;if(
Cyc_zstrcmp((struct _dyneither_ptr)_tmp433,((_tmpB2A="initializes",_tag_dyneither(
_tmpB2A,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp433,((
_tmpB29="__initializes__",_tag_dyneither(_tmpB29,sizeof(char),16))))== 0){struct
Cyc_Absyn_Initializes_att_struct _tmpB2D;struct Cyc_Absyn_Initializes_att_struct*
_tmpB2C;a=(void*)((_tmpB2C=_cycalloc_atomic(sizeof(*_tmpB2C)),((_tmpB2C[0]=((
_tmpB2D.tag=4,((_tmpB2D.f1=_tmp435,_tmpB2D)))),_tmpB2C))));}else{{const char*
_tmpB2E;Cyc_Parse_err(((_tmpB2E="unrecognized attribute",_tag_dyneither(_tmpB2E,
sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)1;}}}}{
union Cyc_YYSTYPE_union _tmpB2F;yyval=(union Cyc_YYSTYPE_union)(((_tmpB2F.YY46).tag=
50,(((_tmpB2F.YY46).f1=(void*)a,_tmpB2F))));}break;}}case 60: _LL23C: {struct
_dyneither_ptr _tmp447=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp448=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpB31;const char*_tmpB30;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp447,((_tmpB31="section",
_tag_dyneither(_tmpB31,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp447,((_tmpB30="__section__",_tag_dyneither(_tmpB30,sizeof(
char),12))))== 0){struct Cyc_Absyn_Section_att_struct _tmpB34;struct Cyc_Absyn_Section_att_struct*
_tmpB33;a=(void*)((_tmpB33=_cycalloc(sizeof(*_tmpB33)),((_tmpB33[0]=((_tmpB34.tag=
2,((_tmpB34.f1=_tmp448,_tmpB34)))),_tmpB33))));}else{{const char*_tmpB35;Cyc_Parse_err(((
_tmpB35="unrecognized attribute",_tag_dyneither(_tmpB35,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)1;}}{
union Cyc_YYSTYPE_union _tmpB36;yyval=(union Cyc_YYSTYPE_union)(((_tmpB36.YY46).tag=
50,(((_tmpB36.YY46).f1=(void*)a,_tmpB36))));}break;}case 61: _LL23D: {struct
_dyneither_ptr _tmp44F=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp450=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpB37;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp44F,((_tmpB37="__mode__",
_tag_dyneither(_tmpB37,sizeof(char),9))))== 0){struct Cyc_Absyn_Mode_att_struct
_tmpB3A;struct Cyc_Absyn_Mode_att_struct*_tmpB39;a=(void*)((_tmpB39=_cycalloc(
sizeof(*_tmpB39)),((_tmpB39[0]=((_tmpB3A.tag=5,((_tmpB3A.f1=_tmp450,_tmpB3A)))),
_tmpB39))));}else{{const char*_tmpB3B;Cyc_Parse_err(((_tmpB3B="unrecognized attribute",
_tag_dyneither(_tmpB3B,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)1;}}{
union Cyc_YYSTYPE_union _tmpB3C;yyval=(union Cyc_YYSTYPE_union)(((_tmpB3C.YY46).tag=
50,(((_tmpB3C.YY46).f1=(void*)a,_tmpB3C))));}break;}case 62: _LL23E: {struct
_dyneither_ptr _tmp456=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dyneither_ptr _tmp457=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp459;struct
_tuple11 _tmp458=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp459=_tmp458.f2;{int _tmp45B;struct _tuple11 _tmp45A=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp45B=_tmp45A.f2;{
void*a=(void*)1;{const char*_tmpB3E;const char*_tmpB3D;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp456,((_tmpB3E="format",_tag_dyneither(_tmpB3E,sizeof(char),7))))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp456,((_tmpB3D="__format__",
_tag_dyneither(_tmpB3D,sizeof(char),11))))== 0){const char*_tmpB3F;if(Cyc_zstrcmp((
struct _dyneither_ptr)_tmp457,((_tmpB3F="printf",_tag_dyneither(_tmpB3F,sizeof(
char),7))))== 0){struct Cyc_Absyn_Format_att_struct _tmpB42;struct Cyc_Absyn_Format_att_struct*
_tmpB41;a=(void*)((_tmpB41=_cycalloc(sizeof(*_tmpB41)),((_tmpB41[0]=((_tmpB42.tag=
3,((_tmpB42.f1=(void*)((void*)0),((_tmpB42.f2=_tmp459,((_tmpB42.f3=_tmp45B,
_tmpB42)))))))),_tmpB41))));}else{const char*_tmpB43;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp457,((_tmpB43="scanf",_tag_dyneither(_tmpB43,sizeof(char),6))))
== 0){struct Cyc_Absyn_Format_att_struct _tmpB46;struct Cyc_Absyn_Format_att_struct*
_tmpB45;a=(void*)((_tmpB45=_cycalloc(sizeof(*_tmpB45)),((_tmpB45[0]=((_tmpB46.tag=
3,((_tmpB46.f1=(void*)((void*)1),((_tmpB46.f2=_tmp459,((_tmpB46.f3=_tmp45B,
_tmpB46)))))))),_tmpB45))));}else{const char*_tmpB47;Cyc_Parse_err(((_tmpB47="unrecognized format type",
_tag_dyneither(_tmpB47,sizeof(char),25))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{
const char*_tmpB48;Cyc_Parse_err(((_tmpB48="unrecognized attribute",
_tag_dyneither(_tmpB48,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}{union Cyc_YYSTYPE_union
_tmpB49;yyval=(union Cyc_YYSTYPE_union)(((_tmpB49.YY46).tag=50,(((_tmpB49.YY46).f1=(
void*)a,_tmpB49))));}break;}}}case 63: _LL23F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 64: _LL240:{struct
Cyc_Absyn_TypedefType_struct*_tmpB4F;struct Cyc_Absyn_TypedefType_struct _tmpB4E;
union Cyc_YYSTYPE_union _tmpB4D;yyval=(union Cyc_YYSTYPE_union)(((_tmpB4D.YY23).tag=
27,(((_tmpB4D.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpB4F=_cycalloc(
sizeof(*_tmpB4F)),((_tmpB4F[0]=((_tmpB4E.tag=16,((_tmpB4E.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB4E.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB4E.f3=0,((
_tmpB4E.f4=0,_tmpB4E)))))))))),_tmpB4F)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB4D))));}
break;case 65: _LL241:{union Cyc_YYSTYPE_union _tmpB50;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB50.YY23).tag=27,(((_tmpB50.YY23).f1=(void*)Cyc_Parse_type_spec((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB50))));}
break;case 66: _LL242:{union Cyc_YYSTYPE_union _tmpB51;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB51.YY23).tag=27,(((_tmpB51.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(
0,0),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpB51))));}break;case 67: _LL243:{union Cyc_YYSTYPE_union
_tmpB52;yyval=(union Cyc_YYSTYPE_union)(((_tmpB52.YY23).tag=27,(((_tmpB52.YY23).f1=(
void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB52))));}
break;case 68: _LL244:{union Cyc_YYSTYPE_union _tmpB53;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB53.YY23).tag=27,(((_tmpB53.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_char_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpB53))));}break;case 69: _LL245:{struct Cyc_Parse_Short_spec_struct*
_tmpB59;struct Cyc_Parse_Short_spec_struct _tmpB58;union Cyc_YYSTYPE_union _tmpB57;
yyval=(union Cyc_YYSTYPE_union)(((_tmpB57.YY23).tag=27,(((_tmpB57.YY23).f1=(void*)((
void*)((_tmpB59=_cycalloc(sizeof(*_tmpB59)),((_tmpB59[0]=((_tmpB58.tag=2,((
_tmpB58.f1=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpB58)))),_tmpB59))))),_tmpB57))));}break;case 70:
_LL246:{union Cyc_YYSTYPE_union _tmpB5A;yyval=(union Cyc_YYSTYPE_union)(((_tmpB5A.YY23).tag=
27,(((_tmpB5A.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB5A))));}
break;case 71: _LL247:{struct Cyc_Parse_Long_spec_struct*_tmpB60;struct Cyc_Parse_Long_spec_struct
_tmpB5F;union Cyc_YYSTYPE_union _tmpB5E;yyval=(union Cyc_YYSTYPE_union)(((_tmpB5E.YY23).tag=
27,(((_tmpB5E.YY23).f1=(void*)((void*)((_tmpB60=_cycalloc(sizeof(*_tmpB60)),((
_tmpB60[0]=((_tmpB5F.tag=3,((_tmpB5F.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB5F)))),
_tmpB60))))),_tmpB5E))));}break;case 72: _LL248:{union Cyc_YYSTYPE_union _tmpB61;
yyval=(union Cyc_YYSTYPE_union)(((_tmpB61.YY23).tag=27,(((_tmpB61.YY23).f1=(void*)
Cyc_Parse_type_spec(Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB61))));}
break;case 73: _LL249:{union Cyc_YYSTYPE_union _tmpB62;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB62.YY23).tag=27,(((_tmpB62.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_typ(
0),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpB62))));}break;case 74: _LL24A:{struct Cyc_Parse_Signed_spec_struct*
_tmpB68;struct Cyc_Parse_Signed_spec_struct _tmpB67;union Cyc_YYSTYPE_union _tmpB66;
yyval=(union Cyc_YYSTYPE_union)(((_tmpB66.YY23).tag=27,(((_tmpB66.YY23).f1=(void*)((
void*)((_tmpB68=_cycalloc(sizeof(*_tmpB68)),((_tmpB68[0]=((_tmpB67.tag=0,((
_tmpB67.f1=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpB67)))),_tmpB68))))),_tmpB66))));}break;case 75:
_LL24B:{struct Cyc_Parse_Unsigned_spec_struct*_tmpB6E;struct Cyc_Parse_Unsigned_spec_struct
_tmpB6D;union Cyc_YYSTYPE_union _tmpB6C;yyval=(union Cyc_YYSTYPE_union)(((_tmpB6C.YY23).tag=
27,(((_tmpB6C.YY23).f1=(void*)((void*)((_tmpB6E=_cycalloc(sizeof(*_tmpB6E)),((
_tmpB6E[0]=((_tmpB6D.tag=1,((_tmpB6D.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB6D)))),
_tmpB6E))))),_tmpB6C))));}break;case 76: _LL24C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 77: _LL24D: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 78: _LL24E:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 79:
_LL24F:{union Cyc_YYSTYPE_union _tmpB6F;yyval=(union Cyc_YYSTYPE_union)(((_tmpB6F.YY23).tag=
27,(((_tmpB6F.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB6F))));}
break;case 80: _LL250:{struct Cyc_Absyn_TupleType_struct*_tmpB75;struct Cyc_Absyn_TupleType_struct
_tmpB74;union Cyc_YYSTYPE_union _tmpB73;yyval=(union Cyc_YYSTYPE_union)(((_tmpB73.YY23).tag=
27,(((_tmpB73.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpB75=_cycalloc(
sizeof(*_tmpB75)),((_tmpB75[0]=((_tmpB74.tag=9,((_tmpB74.f1=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))),_tmpB74)))),_tmpB75)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpB73))));}break;case 81: _LL251:{struct Cyc_Absyn_RgnHandleType_struct*
_tmpB7B;struct Cyc_Absyn_RgnHandleType_struct _tmpB7A;union Cyc_YYSTYPE_union
_tmpB79;yyval=(union Cyc_YYSTYPE_union)(((_tmpB79.YY23).tag=27,(((_tmpB79.YY23).f1=(
void*)Cyc_Parse_type_spec((void*)((_tmpB7B=_cycalloc(sizeof(*_tmpB7B)),((_tmpB7B[
0]=((_tmpB7A.tag=14,((_tmpB7A.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB7A)))),_tmpB7B)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpB79))));}break;case 82: _LL252: {void*_tmp484=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0);{struct Cyc_Absyn_DynRgnType_struct*_tmpB81;
struct Cyc_Absyn_DynRgnType_struct _tmpB80;union Cyc_YYSTYPE_union _tmpB7F;yyval=(
union Cyc_YYSTYPE_union)(((_tmpB7F.YY23).tag=27,(((_tmpB7F.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)((_tmpB81=_cycalloc(sizeof(*_tmpB81)),((_tmpB81[0]=((_tmpB80.tag=15,((
_tmpB80.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB80.f2=(void*)_tmp484,_tmpB80)))))),_tmpB81)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB7F))));}
break;}case 83: _LL253:{struct Cyc_Absyn_DynRgnType_struct*_tmpB87;struct Cyc_Absyn_DynRgnType_struct
_tmpB86;union Cyc_YYSTYPE_union _tmpB85;yyval=(union Cyc_YYSTYPE_union)(((_tmpB85.YY23).tag=
27,(((_tmpB85.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpB87=_cycalloc(
sizeof(*_tmpB87)),((_tmpB87[0]=((_tmpB86.tag=15,((_tmpB86.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB86.f2=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpB86)))))),_tmpB87)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB85))));}
break;case 84: _LL254:{struct Cyc_Absyn_TagType_struct*_tmpB8D;struct Cyc_Absyn_TagType_struct
_tmpB8C;union Cyc_YYSTYPE_union _tmpB8B;yyval=(union Cyc_YYSTYPE_union)(((_tmpB8B.YY23).tag=
27,(((_tmpB8B.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpB8D=_cycalloc(
sizeof(*_tmpB8D)),((_tmpB8D[0]=((_tmpB8C.tag=18,((_tmpB8C.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB8C)))),_tmpB8D)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpB8B))));}break;case 85: _LL255:{struct Cyc_Absyn_TagType_struct*
_tmpB93;struct Cyc_Absyn_TagType_struct _tmpB92;union Cyc_YYSTYPE_union _tmpB91;
yyval=(union Cyc_YYSTYPE_union)(((_tmpB91.YY23).tag=27,(((_tmpB91.YY23).f1=(void*)
Cyc_Parse_type_spec((void*)((_tmpB93=_cycalloc(sizeof(*_tmpB93)),((_tmpB93[0]=((
_tmpB92.tag=18,((_tmpB92.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ik,
0),_tmpB92)))),_tmpB93)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB91))));}
break;case 86: _LL256:{struct Cyc_Absyn_ValueofType_struct*_tmpB99;struct Cyc_Absyn_ValueofType_struct
_tmpB98;union Cyc_YYSTYPE_union _tmpB97;yyval=(union Cyc_YYSTYPE_union)(((_tmpB97.YY23).tag=
27,(((_tmpB97.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpB99=_cycalloc(
sizeof(*_tmpB99)),((_tmpB99[0]=((_tmpB98.tag=17,((_tmpB98.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB98)))),_tmpB99)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpB97))));}break;case 87: _LL257:{union Cyc_YYSTYPE_union
_tmpB9A;yyval=(union Cyc_YYSTYPE_union)(((_tmpB9A.YY43).tag=47,(((_tmpB9A.YY43).f1=(
void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB9A))));}
break;case 88: _LL258: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line): 0;{struct Cyc_Absyn_Tqual _tmpB9D;union Cyc_YYSTYPE_union
_tmpB9C;yyval=(union Cyc_YYSTYPE_union)(((_tmpB9C.YY25).tag=29,(((_tmpB9C.YY25).f1=((
_tmpB9D.print_const=1,((_tmpB9D.q_volatile=0,((_tmpB9D.q_restrict=0,((_tmpB9D.real_const=
1,((_tmpB9D.loc=loc,_tmpB9D)))))))))),_tmpB9C))));}break;}case 89: _LL259:{struct
Cyc_Absyn_Tqual _tmpBA0;union Cyc_YYSTYPE_union _tmpB9F;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB9F.YY25).tag=29,(((_tmpB9F.YY25).f1=((_tmpBA0.print_const=0,((_tmpBA0.q_volatile=
1,((_tmpBA0.q_restrict=0,((_tmpBA0.real_const=0,((_tmpBA0.loc=0,_tmpBA0)))))))))),
_tmpB9F))));}break;case 90: _LL25A:{struct Cyc_Absyn_Tqual _tmpBA3;union Cyc_YYSTYPE_union
_tmpBA2;yyval=(union Cyc_YYSTYPE_union)(((_tmpBA2.YY25).tag=29,(((_tmpBA2.YY25).f1=((
_tmpBA3.print_const=0,((_tmpBA3.q_volatile=0,((_tmpBA3.q_restrict=1,((_tmpBA3.real_const=
0,((_tmpBA3.loc=0,_tmpBA3)))))))))),_tmpBA2))));}break;case 91: _LL25B:{struct Cyc_Parse_Decl_spec_struct*
_tmpBC7;struct Cyc_Absyn_Decl*_tmpBC6;struct Cyc_Absyn_Enum_d_struct _tmpBC5;struct
Cyc_Core_Opt*_tmpBC4;struct Cyc_Absyn_Enumdecl*_tmpBC3;struct Cyc_Absyn_Enum_d_struct*
_tmpBC2;struct Cyc_Parse_Decl_spec_struct _tmpBC1;union Cyc_YYSTYPE_union _tmpBC0;
yyval=(union Cyc_YYSTYPE_union)(((_tmpBC0.YY23).tag=27,(((_tmpBC0.YY23).f1=(void*)((
void*)((_tmpBC7=_cycalloc(sizeof(*_tmpBC7)),((_tmpBC7[0]=((_tmpBC1.tag=5,((
_tmpBC1.f1=((_tmpBC6=_cycalloc(sizeof(*_tmpBC6)),((_tmpBC6->r=(void*)((void*)((
_tmpBC2=_cycalloc(sizeof(*_tmpBC2)),((_tmpBC2[0]=((_tmpBC5.tag=6,((_tmpBC5.f1=((
_tmpBC3=_cycalloc(sizeof(*_tmpBC3)),((_tmpBC3->sc=(void*)((void*)2),((_tmpBC3->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpBC3->fields=((_tmpBC4=_cycalloc(sizeof(*_tmpBC4)),((_tmpBC4->v=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpBC4)))),_tmpBC3)))))))),
_tmpBC5)))),_tmpBC2))))),((_tmpBC6->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBC6)))))),
_tmpBC1)))),_tmpBC7))))),_tmpBC0))));}break;case 92: _LL25C:{struct Cyc_Absyn_EnumType_struct*
_tmpBCD;struct Cyc_Absyn_EnumType_struct _tmpBCC;union Cyc_YYSTYPE_union _tmpBCB;
yyval=(union Cyc_YYSTYPE_union)(((_tmpBCB.YY23).tag=27,(((_tmpBCB.YY23).f1=(void*)
Cyc_Parse_type_spec((void*)((_tmpBCD=_cycalloc(sizeof(*_tmpBCD)),((_tmpBCD[0]=((
_tmpBCC.tag=12,((_tmpBCC.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBCC.f2=0,_tmpBCC)))))),
_tmpBCD)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpBCB))));}break;case 93: _LL25D:{struct Cyc_Parse_Type_spec_struct*
_tmpBDC;struct Cyc_Absyn_AnonEnumType_struct*_tmpBDB;struct Cyc_Absyn_AnonEnumType_struct
_tmpBDA;struct Cyc_Parse_Type_spec_struct _tmpBD9;union Cyc_YYSTYPE_union _tmpBD8;
yyval=(union Cyc_YYSTYPE_union)(((_tmpBD8.YY23).tag=27,(((_tmpBD8.YY23).f1=(void*)((
void*)((_tmpBDC=_cycalloc(sizeof(*_tmpBDC)),((_tmpBDC[0]=((_tmpBD9.tag=4,((
_tmpBD9.f1=(void*)((void*)((_tmpBDB=_cycalloc(sizeof(*_tmpBDB)),((_tmpBDB[0]=((
_tmpBDA.tag=13,((_tmpBDA.f1=Cyc_yyget_YY48(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpBDA)))),_tmpBDB))))),((_tmpBD9.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBD9)))))),
_tmpBDC))))),_tmpBD8))));}break;case 94: _LL25E:{struct Cyc_Absyn_Enumfield*_tmpBDF;
union Cyc_YYSTYPE_union _tmpBDE;yyval=(union Cyc_YYSTYPE_union)(((_tmpBDE.YY47).tag=
51,(((_tmpBDE.YY47).f1=((_tmpBDF=_cycalloc(sizeof(*_tmpBDF)),((_tmpBDF->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBDF->tag=0,((
_tmpBDF->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpBDF)))))))),_tmpBDE))));}break;case 95: _LL25F:{
struct Cyc_Absyn_Enumfield*_tmpBE2;union Cyc_YYSTYPE_union _tmpBE1;yyval=(union Cyc_YYSTYPE_union)(((
_tmpBE1.YY47).tag=51,(((_tmpBE1.YY47).f1=((_tmpBE2=_cycalloc(sizeof(*_tmpBE2)),((
_tmpBE2->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmpBE2->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBE2->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBE2)))))))),
_tmpBE1))));}break;case 96: _LL260:{struct Cyc_List_List*_tmpBE5;union Cyc_YYSTYPE_union
_tmpBE4;yyval=(union Cyc_YYSTYPE_union)(((_tmpBE4.YY48).tag=52,(((_tmpBE4.YY48).f1=((
_tmpBE5=_cycalloc(sizeof(*_tmpBE5)),((_tmpBE5->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBE5->tl=0,_tmpBE5)))))),
_tmpBE4))));}break;case 97: _LL261:{struct Cyc_List_List*_tmpBE8;union Cyc_YYSTYPE_union
_tmpBE7;yyval=(union Cyc_YYSTYPE_union)(((_tmpBE7.YY48).tag=52,(((_tmpBE7.YY48).f1=((
_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpBE8->tl=0,_tmpBE8)))))),
_tmpBE7))));}break;case 98: _LL262:{struct Cyc_List_List*_tmpBEB;union Cyc_YYSTYPE_union
_tmpBEA;yyval=(union Cyc_YYSTYPE_union)(((_tmpBEA.YY48).tag=52,(((_tmpBEA.YY48).f1=((
_tmpBEB=_cycalloc(sizeof(*_tmpBEB)),((_tmpBEB->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBEB->tl=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBEB)))))),_tmpBEA))));}
break;case 99: _LL263:{struct Cyc_Absyn_AnonAggrType_struct*_tmpBF1;struct Cyc_Absyn_AnonAggrType_struct
_tmpBF0;union Cyc_YYSTYPE_union _tmpBEF;yyval=(union Cyc_YYSTYPE_union)(((_tmpBEF.YY23).tag=
27,(((_tmpBEF.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpBF1=_cycalloc(
sizeof(*_tmpBF1)),((_tmpBF1[0]=((_tmpBF0.tag=11,((_tmpBF0.f1=(void*)Cyc_yyget_YY24(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpBF0.f2=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpBF0)))))),
_tmpBF1)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpBEF))));}break;case 100: _LL264: {struct Cyc_List_List*
_tmp4B8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]));struct Cyc_List_List*_tmp4B9=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct*
_tmpBF7;struct Cyc_Parse_Decl_spec_struct _tmpBF6;union Cyc_YYSTYPE_union _tmpBF5;
yyval=(union Cyc_YYSTYPE_union)(((_tmpBF5.YY23).tag=27,(((_tmpBF5.YY23).f1=(void*)((
void*)((_tmpBF7=_cycalloc(sizeof(*_tmpBF7)),((_tmpBF7[0]=((_tmpBF6.tag=5,((
_tmpBF6.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4B8,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4B9,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpBF6)))),
_tmpBF7))))),_tmpBF5))));}break;}case 101: _LL265:{struct Cyc_Absyn_AggrType_struct*
_tmpC06;struct Cyc_Absyn_AggrInfo _tmpC05;union Cyc_Absyn_AggrInfoU_union _tmpC04;
struct Cyc_Absyn_AggrType_struct _tmpC03;union Cyc_YYSTYPE_union _tmpC02;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC02.YY23).tag=27,(((_tmpC02.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)((_tmpC06=_cycalloc(sizeof(*_tmpC06)),((_tmpC06[0]=((_tmpC03.tag=10,((
_tmpC03.f1=((_tmpC05.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((_tmpC04.UnknownAggr).tag=
0,(((_tmpC04.UnknownAggr).f1=(void*)Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(((_tmpC04.UnknownAggr).f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpC04)))))),((_tmpC05.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),_tmpC05)))),_tmpC03)))),_tmpC06)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpC02))));}
break;case 102: _LL266:{union Cyc_YYSTYPE_union _tmpC07;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC07.YY40).tag=44,(((_tmpC07.YY40).f1=0,_tmpC07))));}break;case 103: _LL267:{
union Cyc_YYSTYPE_union _tmpC08;yyval=(union Cyc_YYSTYPE_union)(((_tmpC08.YY40).tag=
44,(((_tmpC08.YY40).f1=Cyc_List_imp_rev(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),_tmpC08))));}break;case
104: _LL268:{union Cyc_YYSTYPE_union _tmpC09;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC09.YY24).tag=28,(((_tmpC09.YY24).f1=(void*)((void*)0),_tmpC09))));}break;
case 105: _LL269:{union Cyc_YYSTYPE_union _tmpC0A;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC0A.YY24).tag=28,(((_tmpC0A.YY24).f1=(void*)((void*)1),_tmpC0A))));}break;
case 106: _LL26A:{union Cyc_YYSTYPE_union _tmpC0B;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC0B.YY26).tag=30,(((_tmpC0B.YY26).f1=0,_tmpC0B))));}break;case 107: _LL26B:{
union Cyc_YYSTYPE_union _tmpC0C;yyval=(union Cyc_YYSTYPE_union)(((_tmpC0C.YY26).tag=
30,(((_tmpC0C.YY26).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))),_tmpC0C))));}break;
case 108: _LL26C:{struct Cyc_List_List*_tmpC0F;union Cyc_YYSTYPE_union _tmpC0E;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC0E.YY27).tag=31,(((_tmpC0E.YY27).f1=((_tmpC0F=
_cycalloc(sizeof(*_tmpC0F)),((_tmpC0F->hd=Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC0F->tl=0,_tmpC0F)))))),
_tmpC0E))));}break;case 109: _LL26D:{struct Cyc_List_List*_tmpC12;union Cyc_YYSTYPE_union
_tmpC11;yyval=(union Cyc_YYSTYPE_union)(((_tmpC11.YY27).tag=31,(((_tmpC11.YY27).f1=((
_tmpC12=_cycalloc(sizeof(*_tmpC12)),((_tmpC12->hd=Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC12->tl=Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC12)))))),
_tmpC11))));}break;case 110: _LL26E:{union Cyc_YYSTYPE_union _tmpC13;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC13.YY21).tag=25,(((_tmpC13.YY21).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])),_tmpC13))));}break;case 111: _LL26F:{struct Cyc_List_List*_tmpC16;
union Cyc_YYSTYPE_union _tmpC15;yyval=(union Cyc_YYSTYPE_union)(((_tmpC15.YY21).tag=
25,(((_tmpC15.YY21).f1=((_tmpC16=_cycalloc(sizeof(*_tmpC16)),((_tmpC16->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC16->tl=0,
_tmpC16)))))),_tmpC15))));}break;case 112: _LL270:{struct Cyc_List_List*_tmpC19;
union Cyc_YYSTYPE_union _tmpC18;yyval=(union Cyc_YYSTYPE_union)(((_tmpC18.YY21).tag=
25,(((_tmpC18.YY21).f1=((_tmpC19=_cycalloc(sizeof(*_tmpC19)),((_tmpC19->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC19->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC19)))))),
_tmpC18))));}break;case 113: _LL271:{struct _tuple15*_tmpC1C;union Cyc_YYSTYPE_union
_tmpC1B;yyval=(union Cyc_YYSTYPE_union)(((_tmpC1B.YY20).tag=24,(((_tmpC1B.YY20).f1=((
_tmpC1C=_cycalloc(sizeof(*_tmpC1C)),((_tmpC1C->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC1C->f2=0,_tmpC1C)))))),
_tmpC1B))));}break;case 114: _LL272:{struct _tuple15*_tmpC1F;union Cyc_YYSTYPE_union
_tmpC1E;yyval=(union Cyc_YYSTYPE_union)(((_tmpC1E.YY20).tag=24,(((_tmpC1E.YY20).f1=((
_tmpC1F=_cycalloc(sizeof(*_tmpC1F)),((_tmpC1F->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC1F->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC1F)))))),
_tmpC1E))));}break;case 115: _LL273:{struct _RegionHandle _tmp4D5=_new_region("temp");
struct _RegionHandle*temp=& _tmp4D5;_push_region(temp);{struct _tuple16 _tmp4D7;
struct Cyc_Absyn_Tqual _tmp4D8;struct Cyc_List_List*_tmp4D9;struct Cyc_List_List*
_tmp4DA;struct _tuple16*_tmp4D6=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4D7=*_tmp4D6;_tmp4D8=
_tmp4D7.f1;_tmp4D9=_tmp4D7.f2;_tmp4DA=_tmp4D7.f3;if(_tmp4D8.loc == 0)_tmp4D8.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp4DC;struct Cyc_List_List*
_tmp4DD;struct _tuple0 _tmp4DB=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(temp,temp,Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4DC=_tmp4DB.f1;
_tmp4DD=_tmp4DB.f2;{void*_tmp4DE=Cyc_Parse_speclist2typ(_tmp4D9,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp4DF=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(
temp,_tmp4D8,_tmp4DE,_tmp4DC,_tmp4DA),_tmp4DD);union Cyc_YYSTYPE_union _tmpC20;
yyval=(union Cyc_YYSTYPE_union)(((_tmpC20.YY26).tag=30,(((_tmpC20.YY26).f1=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,
struct _tuple8*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp4DF),
_tmpC20))));}}};_pop_region(temp);}break;case 116: _LL274:{struct _tuple16*_tmpC26;
struct Cyc_List_List*_tmpC25;union Cyc_YYSTYPE_union _tmpC24;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC24.YY35).tag=39,(((_tmpC24.YY35).f1=((_tmpC26=_cycalloc(sizeof(*_tmpC26)),((
_tmpC26->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpC26->f2=((
_tmpC25=_cycalloc(sizeof(*_tmpC25)),((_tmpC25->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC25->tl=0,_tmpC25)))))),((
_tmpC26->f3=0,_tmpC26)))))))),_tmpC24))));}break;case 117: _LL275:{struct _tuple16*
_tmpC2C;struct Cyc_List_List*_tmpC2B;union Cyc_YYSTYPE_union _tmpC2A;yyval=(union
Cyc_YYSTYPE_union)(((_tmpC2A.YY35).tag=39,(((_tmpC2A.YY35).f1=((_tmpC2C=
_cycalloc(sizeof(*_tmpC2C)),((_tmpC2C->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpC2C->f2=((_tmpC2B=
_cycalloc(sizeof(*_tmpC2B)),((_tmpC2B->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpC2B->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpC2B)))))),((
_tmpC2C->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpC2C)))))))),_tmpC2A))));}break;case 118: _LL276:{struct
_tuple16*_tmpC2F;union Cyc_YYSTYPE_union _tmpC2E;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC2E.YY35).tag=39,(((_tmpC2E.YY35).f1=((_tmpC2F=_cycalloc(sizeof(*_tmpC2F)),((
_tmpC2F->f1=Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpC2F->f2=0,((_tmpC2F->f3=0,_tmpC2F)))))))),_tmpC2E))));}
break;case 119: _LL277:{struct _tuple16*_tmpC32;union Cyc_YYSTYPE_union _tmpC31;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC31.YY35).tag=39,(((_tmpC31.YY35).f1=((_tmpC32=
_cycalloc(sizeof(*_tmpC32)),((_tmpC32->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpC32->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpC32->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpC32)))))))),_tmpC31))));}break;case 120: _LL278:{struct
_tuple16*_tmpC35;union Cyc_YYSTYPE_union _tmpC34;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC34.YY35).tag=39,(((_tmpC34.YY35).f1=((_tmpC35=_cycalloc(sizeof(*_tmpC35)),((
_tmpC35->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpC35->f2=0,((
_tmpC35->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpC35)))))))),_tmpC34))));}break;case 121: _LL279:{struct
_tuple16*_tmpC38;union Cyc_YYSTYPE_union _tmpC37;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC37.YY35).tag=39,(((_tmpC37.YY35).f1=((_tmpC38=_cycalloc(sizeof(*_tmpC38)),((
_tmpC38->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1,((_tmpC38->f2=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpC38->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpC38)))))))),_tmpC37))));}break;case 122: _LL27A:{struct _tuple16*_tmpC3E;struct
Cyc_List_List*_tmpC3D;union Cyc_YYSTYPE_union _tmpC3C;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC3C.YY35).tag=39,(((_tmpC3C.YY35).f1=((_tmpC3E=_cycalloc(sizeof(*_tmpC3E)),((
_tmpC3E->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpC3E->f2=((
_tmpC3D=_cycalloc(sizeof(*_tmpC3D)),((_tmpC3D->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC3D->tl=0,_tmpC3D)))))),((
_tmpC3E->f3=0,_tmpC3E)))))))),_tmpC3C))));}break;case 123: _LL27B:{struct _tuple16*
_tmpC44;struct Cyc_List_List*_tmpC43;union Cyc_YYSTYPE_union _tmpC42;yyval=(union
Cyc_YYSTYPE_union)(((_tmpC42.YY35).tag=39,(((_tmpC42.YY35).f1=((_tmpC44=
_cycalloc(sizeof(*_tmpC44)),((_tmpC44->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpC44->f2=((_tmpC43=
_cycalloc(sizeof(*_tmpC43)),((_tmpC43->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpC43->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpC43)))))),((
_tmpC44->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpC44)))))))),_tmpC42))));}break;case 124: _LL27C:{struct
_tuple16*_tmpC47;union Cyc_YYSTYPE_union _tmpC46;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC46.YY35).tag=39,(((_tmpC46.YY35).f1=((_tmpC47=_cycalloc(sizeof(*_tmpC47)),((
_tmpC47->f1=Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpC47->f2=0,((_tmpC47->f3=0,_tmpC47)))))))),_tmpC46))));}
break;case 125: _LL27D:{struct _tuple16*_tmpC4A;union Cyc_YYSTYPE_union _tmpC49;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC49.YY35).tag=39,(((_tmpC49.YY35).f1=((_tmpC4A=
_cycalloc(sizeof(*_tmpC4A)),((_tmpC4A->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpC4A->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpC4A->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpC4A)))))))),_tmpC49))));}break;case 126: _LL27E:{struct
_tuple16*_tmpC4D;union Cyc_YYSTYPE_union _tmpC4C;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC4C.YY35).tag=39,(((_tmpC4C.YY35).f1=((_tmpC4D=_cycalloc(sizeof(*_tmpC4D)),((
_tmpC4D->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpC4D->f2=0,((
_tmpC4D->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpC4D)))))))),_tmpC4C))));}break;case 127: _LL27F:{struct
_tuple16*_tmpC50;union Cyc_YYSTYPE_union _tmpC4F;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC4F.YY35).tag=39,(((_tmpC4F.YY35).f1=((_tmpC50=_cycalloc(sizeof(*_tmpC50)),((
_tmpC50->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1,((_tmpC50->f2=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpC50->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpC50)))))))),_tmpC4F))));}break;case 128: _LL280:{union Cyc_YYSTYPE_union _tmpC51;
yyval=(union Cyc_YYSTYPE_union)(((_tmpC51.YY21).tag=25,(((_tmpC51.YY21).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpC51))));}break;
case 129: _LL281:{struct Cyc_List_List*_tmpC54;union Cyc_YYSTYPE_union _tmpC53;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC53.YY21).tag=25,(((_tmpC53.YY21).f1=((_tmpC54=
_cycalloc(sizeof(*_tmpC54)),((_tmpC54->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC54->tl=0,_tmpC54)))))),
_tmpC53))));}break;case 130: _LL282:{struct Cyc_List_List*_tmpC57;union Cyc_YYSTYPE_union
_tmpC56;yyval=(union Cyc_YYSTYPE_union)(((_tmpC56.YY21).tag=25,(((_tmpC56.YY21).f1=((
_tmpC57=_cycalloc(sizeof(*_tmpC57)),((_tmpC57->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC57->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC57)))))),
_tmpC56))));}break;case 131: _LL283:{struct _tuple15*_tmpC5A;union Cyc_YYSTYPE_union
_tmpC59;yyval=(union Cyc_YYSTYPE_union)(((_tmpC59.YY20).tag=24,(((_tmpC59.YY20).f1=((
_tmpC5A=_cycalloc(sizeof(*_tmpC5A)),((_tmpC5A->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC5A->f2=0,_tmpC5A)))))),
_tmpC59))));}break;case 132: _LL284:{struct _tuple15*_tmpC69;struct _tuple1*_tmpC68;
union Cyc_Absyn_Nmspace_union _tmpC67;struct Cyc_Parse_Declarator*_tmpC66;union Cyc_YYSTYPE_union
_tmpC65;yyval=(union Cyc_YYSTYPE_union)(((_tmpC65.YY20).tag=24,(((_tmpC65.YY20).f1=((
_tmpC69=_cycalloc(sizeof(*_tmpC69)),((_tmpC69->f1=((_tmpC66=_cycalloc(sizeof(*
_tmpC66)),((_tmpC66->id=((_tmpC68=_cycalloc(sizeof(*_tmpC68)),((_tmpC68->f1=(
union Cyc_Absyn_Nmspace_union)(((_tmpC67.Rel_n).tag=1,(((_tmpC67.Rel_n).f1=0,
_tmpC67)))),((_tmpC68->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),"",sizeof(char),1),_tmpC68)))))),((_tmpC66->tms=0,_tmpC66)))))),((
_tmpC69->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC69)))))),_tmpC65))));}
break;case 133: _LL285:{struct _tuple15*_tmpC6C;union Cyc_YYSTYPE_union _tmpC6B;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC6B.YY20).tag=24,(((_tmpC6B.YY20).f1=((_tmpC6C=
_cycalloc(sizeof(*_tmpC6C)),((_tmpC6C->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC6C->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC6C)))))),
_tmpC6B))));}break;case 134: _LL286: {struct Cyc_List_List*_tmp50C=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct*
_tmpC76;struct Cyc_Core_Opt*_tmpC75;struct Cyc_Parse_Decl_spec_struct _tmpC74;union
Cyc_YYSTYPE_union _tmpC73;yyval=(union Cyc_YYSTYPE_union)(((_tmpC73.YY23).tag=27,(((
_tmpC73.YY23).f1=(void*)((void*)((_tmpC76=_cycalloc(sizeof(*_tmpC76)),((_tmpC76[
0]=((_tmpC74.tag=5,((_tmpC74.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp50C,((_tmpC75=
_cycalloc(sizeof(*_tmpC75)),((_tmpC75->v=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC75)))),Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpC74)))),
_tmpC76))))),_tmpC73))));}break;}case 135: _LL287:{struct Cyc_Absyn_TunionType_struct*
_tmpC8B;struct Cyc_Absyn_TunionInfo _tmpC8A;struct Cyc_Absyn_UnknownTunionInfo
_tmpC89;union Cyc_Absyn_TunionInfoU_union _tmpC88;struct Cyc_Absyn_TunionType_struct
_tmpC87;union Cyc_YYSTYPE_union _tmpC86;yyval=(union Cyc_YYSTYPE_union)(((_tmpC86.YY23).tag=
27,(((_tmpC86.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpC8B=_cycalloc(
sizeof(*_tmpC8B)),((_tmpC8B[0]=((_tmpC87.tag=2,((_tmpC87.f1=((_tmpC8A.tunion_info=(
union Cyc_Absyn_TunionInfoU_union)(((_tmpC88.UnknownTunion).tag=0,(((_tmpC88.UnknownTunion).f1=((
_tmpC89.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpC89.is_xtunion=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpC89.is_flat=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmpC89)))))),
_tmpC88)))),((_tmpC8A.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpC8A.rgn=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC8A)))))),_tmpC87)))),
_tmpC8B)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpC86))));}break;case 136: _LL288:{struct Cyc_Absyn_TunionFieldType_struct*
_tmpCA0;struct Cyc_Absyn_TunionFieldInfo _tmpC9F;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmpC9E;union Cyc_Absyn_TunionFieldInfoU_union _tmpC9D;struct Cyc_Absyn_TunionFieldType_struct
_tmpC9C;union Cyc_YYSTYPE_union _tmpC9B;yyval=(union Cyc_YYSTYPE_union)(((_tmpC9B.YY23).tag=
27,(((_tmpC9B.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpCA0=_cycalloc(
sizeof(*_tmpCA0)),((_tmpCA0[0]=((_tmpC9C.tag=3,((_tmpC9C.f1=((_tmpC9F.field_info=(
union Cyc_Absyn_TunionFieldInfoU_union)(((_tmpC9D.UnknownTunionfield).tag=0,(((
_tmpC9D.UnknownTunionfield).f1=((_tmpC9E.tunion_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpC9E.field_name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpC9E.is_xtunion=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
_tmpC9E)))))),_tmpC9D)))),((_tmpC9F.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC9F)))),_tmpC9C)))),
_tmpCA0)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpC9B))));}break;case 137: _LL289:{union Cyc_YYSTYPE_union
_tmpCA1;yyval=(union Cyc_YYSTYPE_union)(((_tmpCA1.YY31).tag=35,(((_tmpCA1.YY31).f1=
1,_tmpCA1))));}break;case 138: _LL28A:{union Cyc_YYSTYPE_union _tmpCA2;yyval=(union
Cyc_YYSTYPE_union)(((_tmpCA2.YY31).tag=35,(((_tmpCA2.YY31).f1=0,_tmpCA2))));}
break;case 139: _LL28B:{union Cyc_YYSTYPE_union _tmpCA3;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCA3.YY31).tag=35,(((_tmpCA3.YY31).f1=0,_tmpCA3))));}break;case 140: _LL28C:{
union Cyc_YYSTYPE_union _tmpCA4;yyval=(union Cyc_YYSTYPE_union)(((_tmpCA4.YY31).tag=
35,(((_tmpCA4.YY31).f1=1,_tmpCA4))));}break;case 141: _LL28D:{struct Cyc_List_List*
_tmpCA7;union Cyc_YYSTYPE_union _tmpCA6;yyval=(union Cyc_YYSTYPE_union)(((_tmpCA6.YY34).tag=
38,(((_tmpCA6.YY34).f1=((_tmpCA7=_cycalloc(sizeof(*_tmpCA7)),((_tmpCA7->hd=Cyc_yyget_YY33(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCA7->tl=0,
_tmpCA7)))))),_tmpCA6))));}break;case 142: _LL28E:{struct Cyc_List_List*_tmpCAA;
union Cyc_YYSTYPE_union _tmpCA9;yyval=(union Cyc_YYSTYPE_union)(((_tmpCA9.YY34).tag=
38,(((_tmpCA9.YY34).f1=((_tmpCAA=_cycalloc(sizeof(*_tmpCAA)),((_tmpCAA->hd=Cyc_yyget_YY33(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCAA->tl=0,
_tmpCAA)))))),_tmpCA9))));}break;case 143: _LL28F:{struct Cyc_List_List*_tmpCAD;
union Cyc_YYSTYPE_union _tmpCAC;yyval=(union Cyc_YYSTYPE_union)(((_tmpCAC.YY34).tag=
38,(((_tmpCAC.YY34).f1=((_tmpCAD=_cycalloc(sizeof(*_tmpCAD)),((_tmpCAD->hd=Cyc_yyget_YY33(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpCAD->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCAD)))))),_tmpCAC))));}
break;case 144: _LL290:{struct Cyc_List_List*_tmpCB0;union Cyc_YYSTYPE_union _tmpCAF;
yyval=(union Cyc_YYSTYPE_union)(((_tmpCAF.YY34).tag=38,(((_tmpCAF.YY34).f1=((
_tmpCB0=_cycalloc(sizeof(*_tmpCB0)),((_tmpCB0->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpCB0->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCB0)))))),_tmpCAF))));}
break;case 145: _LL291:{union Cyc_YYSTYPE_union _tmpCB1;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCB1.YY32).tag=36,(((_tmpCB1.YY32).f1=(void*)((void*)2),_tmpCB1))));}break;
case 146: _LL292:{union Cyc_YYSTYPE_union _tmpCB2;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCB2.YY32).tag=36,(((_tmpCB2.YY32).f1=(void*)((void*)3),_tmpCB2))));}break;
case 147: _LL293:{union Cyc_YYSTYPE_union _tmpCB3;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCB3.YY32).tag=36,(((_tmpCB3.YY32).f1=(void*)((void*)0),_tmpCB3))));}break;
case 148: _LL294:{struct Cyc_Absyn_Tunionfield*_tmpCB6;union Cyc_YYSTYPE_union
_tmpCB5;yyval=(union Cyc_YYSTYPE_union)(((_tmpCB5.YY33).tag=37,(((_tmpCB5.YY33).f1=((
_tmpCB6=_cycalloc(sizeof(*_tmpCB6)),((_tmpCB6->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCB6->typs=0,((_tmpCB6->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpCB6->sc=(void*)Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpCB6)))))))))),
_tmpCB5))));}break;case 149: _LL295: {struct Cyc_List_List*_tmp52E=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Absyn_Tunionfield*
_tmpCB9;union Cyc_YYSTYPE_union _tmpCB8;yyval=(union Cyc_YYSTYPE_union)(((_tmpCB8.YY33).tag=
37,(((_tmpCB8.YY33).f1=((_tmpCB9=_cycalloc(sizeof(*_tmpCB9)),((_tmpCB9->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpCB9->typs=
_tmp52E,((_tmpCB9->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpCB9->sc=(
void*)Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tmpCB9)))))))))),_tmpCB8))));}break;}case 150: _LL296: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 151: _LL297:{struct
Cyc_Parse_Declarator*_tmpCBC;union Cyc_YYSTYPE_union _tmpCBB;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCBB.YY29).tag=33,(((_tmpCBB.YY29).f1=((_tmpCBC=_cycalloc(sizeof(*_tmpCBC)),((
_tmpCBC->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id,((_tmpCBC->tms=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpCBC)))))),
_tmpCBB))));}break;case 152: _LL298: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 153: _LL299:{struct Cyc_Parse_Declarator*_tmpCBF;
union Cyc_YYSTYPE_union _tmpCBE;yyval=(union Cyc_YYSTYPE_union)(((_tmpCBE.YY29).tag=
33,(((_tmpCBE.YY29).f1=((_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->id,((_tmpCBF->tms=
Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->tms),_tmpCBF)))))),_tmpCBE))));}break;case 154: _LL29A:{struct
Cyc_Parse_Declarator*_tmpCC2;union Cyc_YYSTYPE_union _tmpCC1;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCC1.YY29).tag=33,(((_tmpCC1.YY29).f1=((_tmpCC2=_cycalloc(sizeof(*_tmpCC2)),((
_tmpCC2->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpCC2->tms=0,_tmpCC2)))))),_tmpCC1))));}break;case 155: _LL29B:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 156:
_LL29C: {struct Cyc_Parse_Declarator*_tmp537=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpCC8;struct Cyc_Absyn_Attributes_mod_struct _tmpCC7;struct Cyc_List_List*_tmpCC6;
_tmp537->tms=((_tmpCC6=_cycalloc(sizeof(*_tmpCC6)),((_tmpCC6->hd=(void*)((void*)((
_tmpCC8=_cycalloc(sizeof(*_tmpCC8)),((_tmpCC8[0]=((_tmpCC7.tag=5,((_tmpCC7.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpCC7.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpCC7)))))),_tmpCC8))))),((
_tmpCC6->tl=_tmp537->tms,_tmpCC6))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 157: _LL29D:{
struct Cyc_Parse_Declarator*_tmpCD7;struct Cyc_Absyn_Carray_mod_struct*_tmpCD6;
struct Cyc_Absyn_Carray_mod_struct _tmpCD5;struct Cyc_List_List*_tmpCD4;union Cyc_YYSTYPE_union
_tmpCD3;yyval=(union Cyc_YYSTYPE_union)(((_tmpCD3.YY29).tag=33,(((_tmpCD3.YY29).f1=((
_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpCD7->tms=((
_tmpCD4=_cycalloc(sizeof(*_tmpCD4)),((_tmpCD4->hd=(void*)((void*)((_tmpCD6=
_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6[0]=((_tmpCD5.tag=0,((_tmpCD5.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCD5.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpCD5)))))),
_tmpCD6))))),((_tmpCD4->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpCD4)))))),_tmpCD7)))))),_tmpCD3))));}break;
case 158: _LL29E:{struct Cyc_Parse_Declarator*_tmpCE6;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpCE5;struct Cyc_Absyn_ConstArray_mod_struct _tmpCE4;struct Cyc_List_List*_tmpCE3;
union Cyc_YYSTYPE_union _tmpCE2;yyval=(union Cyc_YYSTYPE_union)(((_tmpCE2.YY29).tag=
33,(((_tmpCE2.YY29).f1=((_tmpCE6=_cycalloc(sizeof(*_tmpCE6)),((_tmpCE6->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpCE6->tms=((
_tmpCE3=_cycalloc(sizeof(*_tmpCE3)),((_tmpCE3->hd=(void*)((void*)((_tmpCE5=
_cycalloc(sizeof(*_tmpCE5)),((_tmpCE5[0]=((_tmpCE4.tag=1,((_tmpCE4.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpCE4.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCE4.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpCE4)))))))),
_tmpCE5))))),((_tmpCE3->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpCE3)))))),_tmpCE6)))))),_tmpCE2))));}break;
case 159: _LL29F: {struct _tuple17 _tmp546;struct Cyc_List_List*_tmp547;int _tmp548;
struct Cyc_Absyn_VarargInfo*_tmp549;struct Cyc_Core_Opt*_tmp54A;struct Cyc_List_List*
_tmp54B;struct _tuple17*_tmp545=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp546=*_tmp545;_tmp547=
_tmp546.f1;_tmp548=_tmp546.f2;_tmp549=_tmp546.f3;_tmp54A=_tmp546.f4;_tmp54B=
_tmp546.f5;{struct Cyc_Parse_Declarator*_tmpD02;struct Cyc_Absyn_Function_mod_struct*
_tmpD01;struct Cyc_Absyn_WithTypes_struct*_tmpD00;struct Cyc_Absyn_WithTypes_struct
_tmpCFF;struct Cyc_Absyn_Function_mod_struct _tmpCFE;struct Cyc_List_List*_tmpCFD;
union Cyc_YYSTYPE_union _tmpCFC;yyval=(union Cyc_YYSTYPE_union)(((_tmpCFC.YY29).tag=
33,(((_tmpCFC.YY29).f1=((_tmpD02=_cycalloc(sizeof(*_tmpD02)),((_tmpD02->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpD02->tms=((
_tmpCFD=_cycalloc(sizeof(*_tmpCFD)),((_tmpCFD->hd=(void*)((void*)((_tmpD01=
_cycalloc(sizeof(*_tmpD01)),((_tmpD01[0]=((_tmpCFE.tag=3,((_tmpCFE.f1=(void*)((
void*)((_tmpD00=_cycalloc(sizeof(*_tmpD00)),((_tmpD00[0]=((_tmpCFF.tag=1,((
_tmpCFF.f1=_tmp547,((_tmpCFF.f2=_tmp548,((_tmpCFF.f3=_tmp549,((_tmpCFF.f4=
_tmp54A,((_tmpCFF.f5=_tmp54B,_tmpCFF)))))))))))),_tmpD00))))),_tmpCFE)))),
_tmpD01))))),((_tmpCFD->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpCFD)))))),_tmpD02)))))),_tmpCFC))));}break;}
case 160: _LL2A0:{struct Cyc_Parse_Declarator*_tmpD1E;struct Cyc_Absyn_Function_mod_struct*
_tmpD1D;struct Cyc_Absyn_WithTypes_struct*_tmpD1C;struct Cyc_Absyn_WithTypes_struct
_tmpD1B;struct Cyc_Absyn_Function_mod_struct _tmpD1A;struct Cyc_List_List*_tmpD19;
union Cyc_YYSTYPE_union _tmpD18;yyval=(union Cyc_YYSTYPE_union)(((_tmpD18.YY29).tag=
33,(((_tmpD18.YY29).f1=((_tmpD1E=_cycalloc(sizeof(*_tmpD1E)),((_tmpD1E->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpD1E->tms=((
_tmpD19=_cycalloc(sizeof(*_tmpD19)),((_tmpD19->hd=(void*)((void*)((_tmpD1D=
_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D[0]=((_tmpD1A.tag=3,((_tmpD1A.f1=(void*)((
void*)((_tmpD1C=_cycalloc(sizeof(*_tmpD1C)),((_tmpD1C[0]=((_tmpD1B.tag=1,((
_tmpD1B.f1=0,((_tmpD1B.f2=0,((_tmpD1B.f3=0,((_tmpD1B.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD1B.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD1B)))))))))))),
_tmpD1C))))),_tmpD1A)))),_tmpD1D))))),((_tmpD19->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpD19)))))),
_tmpD1E)))))),_tmpD18))));}break;case 161: _LL2A1:{struct Cyc_Parse_Declarator*
_tmpD3A;struct Cyc_Absyn_Function_mod_struct*_tmpD39;struct Cyc_Absyn_NoTypes_struct*
_tmpD38;struct Cyc_Absyn_NoTypes_struct _tmpD37;struct Cyc_Absyn_Function_mod_struct
_tmpD36;struct Cyc_List_List*_tmpD35;union Cyc_YYSTYPE_union _tmpD34;yyval=(union
Cyc_YYSTYPE_union)(((_tmpD34.YY29).tag=33,(((_tmpD34.YY29).f1=((_tmpD3A=
_cycalloc(sizeof(*_tmpD3A)),((_tmpD3A->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpD3A->tms=((
_tmpD35=_cycalloc(sizeof(*_tmpD35)),((_tmpD35->hd=(void*)((void*)((_tmpD39=
_cycalloc(sizeof(*_tmpD39)),((_tmpD39[0]=((_tmpD36.tag=3,((_tmpD36.f1=(void*)((
void*)((_tmpD38=_cycalloc(sizeof(*_tmpD38)),((_tmpD38[0]=((_tmpD37.tag=0,((
_tmpD37.f1=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),((_tmpD37.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD37)))))),
_tmpD38))))),_tmpD36)))),_tmpD39))))),((_tmpD35->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmpD35)))))),
_tmpD3A)))))),_tmpD34))));}break;case 162: _LL2A2: {struct Cyc_List_List*_tmp561=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Parse_Declarator*
_tmpD49;struct Cyc_Absyn_TypeParams_mod_struct*_tmpD48;struct Cyc_Absyn_TypeParams_mod_struct
_tmpD47;struct Cyc_List_List*_tmpD46;union Cyc_YYSTYPE_union _tmpD45;yyval=(union
Cyc_YYSTYPE_union)(((_tmpD45.YY29).tag=33,(((_tmpD45.YY29).f1=((_tmpD49=
_cycalloc(sizeof(*_tmpD49)),((_tmpD49->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpD49->tms=((
_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46->hd=(void*)((void*)((_tmpD48=
_cycalloc(sizeof(*_tmpD48)),((_tmpD48[0]=((_tmpD47.tag=4,((_tmpD47.f1=_tmp561,((
_tmpD47.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpD47.f3=0,_tmpD47)))))))),_tmpD48))))),((_tmpD46->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpD46)))))),_tmpD49)))))),_tmpD45))));}break;}case 163: _LL2A3:{struct Cyc_Parse_Declarator*
_tmpD58;struct Cyc_Absyn_Attributes_mod_struct*_tmpD57;struct Cyc_Absyn_Attributes_mod_struct
_tmpD56;struct Cyc_List_List*_tmpD55;union Cyc_YYSTYPE_union _tmpD54;yyval=(union
Cyc_YYSTYPE_union)(((_tmpD54.YY29).tag=33,(((_tmpD54.YY29).f1=((_tmpD58=
_cycalloc(sizeof(*_tmpD58)),((_tmpD58->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpD58->tms=((
_tmpD55=_cycalloc(sizeof(*_tmpD55)),((_tmpD55->hd=(void*)((void*)((_tmpD57=
_cycalloc(sizeof(*_tmpD57)),((_tmpD57[0]=((_tmpD56.tag=5,((_tmpD56.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD56.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD56)))))),_tmpD57))))),((
_tmpD55->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpD55)))))),_tmpD58)))))),_tmpD54))));}break;case 164:
_LL2A4:{struct Cyc_Parse_Declarator*_tmpD5B;union Cyc_YYSTYPE_union _tmpD5A;yyval=(
union Cyc_YYSTYPE_union)(((_tmpD5A.YY29).tag=33,(((_tmpD5A.YY29).f1=((_tmpD5B=
_cycalloc(sizeof(*_tmpD5B)),((_tmpD5B->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD5B->tms=0,_tmpD5B)))))),
_tmpD5A))));}break;case 165: _LL2A5:{struct Cyc_Parse_Declarator*_tmpD5E;union Cyc_YYSTYPE_union
_tmpD5D;yyval=(union Cyc_YYSTYPE_union)(((_tmpD5D.YY29).tag=33,(((_tmpD5D.YY29).f1=((
_tmpD5E=_cycalloc(sizeof(*_tmpD5E)),((_tmpD5E->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD5E->tms=0,_tmpD5E)))))),
_tmpD5D))));}break;case 166: _LL2A6: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 167: _LL2A7: {struct Cyc_Parse_Declarator*
_tmp570=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{
struct Cyc_Absyn_Attributes_mod_struct*_tmpD64;struct Cyc_Absyn_Attributes_mod_struct
_tmpD63;struct Cyc_List_List*_tmpD62;_tmp570->tms=((_tmpD62=_cycalloc(sizeof(*
_tmpD62)),((_tmpD62->hd=(void*)((void*)((_tmpD64=_cycalloc(sizeof(*_tmpD64)),((
_tmpD64[0]=((_tmpD63.tag=5,((_tmpD63.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpD63.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpD63)))))),_tmpD64))))),((_tmpD62->tl=_tmp570->tms,_tmpD62))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 168: _LL2A8:{
struct Cyc_Parse_Declarator*_tmpD73;struct Cyc_Absyn_Carray_mod_struct*_tmpD72;
struct Cyc_Absyn_Carray_mod_struct _tmpD71;struct Cyc_List_List*_tmpD70;union Cyc_YYSTYPE_union
_tmpD6F;yyval=(union Cyc_YYSTYPE_union)(((_tmpD6F.YY29).tag=33,(((_tmpD6F.YY29).f1=((
_tmpD73=_cycalloc(sizeof(*_tmpD73)),((_tmpD73->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpD73->tms=((
_tmpD70=_cycalloc(sizeof(*_tmpD70)),((_tmpD70->hd=(void*)((void*)((_tmpD72=
_cycalloc(sizeof(*_tmpD72)),((_tmpD72[0]=((_tmpD71.tag=0,((_tmpD71.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD71.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD71)))))),
_tmpD72))))),((_tmpD70->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpD70)))))),_tmpD73)))))),_tmpD6F))));}break;
case 169: _LL2A9:{struct Cyc_Parse_Declarator*_tmpD82;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpD81;struct Cyc_Absyn_ConstArray_mod_struct _tmpD80;struct Cyc_List_List*_tmpD7F;
union Cyc_YYSTYPE_union _tmpD7E;yyval=(union Cyc_YYSTYPE_union)(((_tmpD7E.YY29).tag=
33,(((_tmpD7E.YY29).f1=((_tmpD82=_cycalloc(sizeof(*_tmpD82)),((_tmpD82->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpD82->tms=((
_tmpD7F=_cycalloc(sizeof(*_tmpD7F)),((_tmpD7F->hd=(void*)((void*)((_tmpD81=
_cycalloc(sizeof(*_tmpD81)),((_tmpD81[0]=((_tmpD80.tag=1,((_tmpD80.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD80.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD80.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD80)))))))),
_tmpD81))))),((_tmpD7F->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpD7F)))))),_tmpD82)))))),_tmpD7E))));}break;
case 170: _LL2AA: {struct _tuple17 _tmp57F;struct Cyc_List_List*_tmp580;int _tmp581;
struct Cyc_Absyn_VarargInfo*_tmp582;struct Cyc_Core_Opt*_tmp583;struct Cyc_List_List*
_tmp584;struct _tuple17*_tmp57E=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp57F=*_tmp57E;_tmp580=
_tmp57F.f1;_tmp581=_tmp57F.f2;_tmp582=_tmp57F.f3;_tmp583=_tmp57F.f4;_tmp584=
_tmp57F.f5;{struct Cyc_Parse_Declarator*_tmpD9E;struct Cyc_Absyn_Function_mod_struct*
_tmpD9D;struct Cyc_Absyn_WithTypes_struct*_tmpD9C;struct Cyc_Absyn_WithTypes_struct
_tmpD9B;struct Cyc_Absyn_Function_mod_struct _tmpD9A;struct Cyc_List_List*_tmpD99;
union Cyc_YYSTYPE_union _tmpD98;yyval=(union Cyc_YYSTYPE_union)(((_tmpD98.YY29).tag=
33,(((_tmpD98.YY29).f1=((_tmpD9E=_cycalloc(sizeof(*_tmpD9E)),((_tmpD9E->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpD9E->tms=((
_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99->hd=(void*)((void*)((_tmpD9D=
_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D[0]=((_tmpD9A.tag=3,((_tmpD9A.f1=(void*)((
void*)((_tmpD9C=_cycalloc(sizeof(*_tmpD9C)),((_tmpD9C[0]=((_tmpD9B.tag=1,((
_tmpD9B.f1=_tmp580,((_tmpD9B.f2=_tmp581,((_tmpD9B.f3=_tmp582,((_tmpD9B.f4=
_tmp583,((_tmpD9B.f5=_tmp584,_tmpD9B)))))))))))),_tmpD9C))))),_tmpD9A)))),
_tmpD9D))))),((_tmpD99->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpD99)))))),_tmpD9E)))))),_tmpD98))));}break;}
case 171: _LL2AB:{struct Cyc_Parse_Declarator*_tmpDBA;struct Cyc_Absyn_Function_mod_struct*
_tmpDB9;struct Cyc_Absyn_WithTypes_struct*_tmpDB8;struct Cyc_Absyn_WithTypes_struct
_tmpDB7;struct Cyc_Absyn_Function_mod_struct _tmpDB6;struct Cyc_List_List*_tmpDB5;
union Cyc_YYSTYPE_union _tmpDB4;yyval=(union Cyc_YYSTYPE_union)(((_tmpDB4.YY29).tag=
33,(((_tmpDB4.YY29).f1=((_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((_tmpDBA->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpDBA->tms=((
_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5->hd=(void*)((void*)((_tmpDB9=
_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9[0]=((_tmpDB6.tag=3,((_tmpDB6.f1=(void*)((
void*)((_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8[0]=((_tmpDB7.tag=1,((
_tmpDB7.f1=0,((_tmpDB7.f2=0,((_tmpDB7.f3=0,((_tmpDB7.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDB7.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpDB7)))))))))))),
_tmpDB8))))),_tmpDB6)))),_tmpDB9))))),((_tmpDB5->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpDB5)))))),
_tmpDBA)))))),_tmpDB4))));}break;case 172: _LL2AC:{struct Cyc_Parse_Declarator*
_tmpDD6;struct Cyc_Absyn_Function_mod_struct*_tmpDD5;struct Cyc_Absyn_NoTypes_struct*
_tmpDD4;struct Cyc_Absyn_NoTypes_struct _tmpDD3;struct Cyc_Absyn_Function_mod_struct
_tmpDD2;struct Cyc_List_List*_tmpDD1;union Cyc_YYSTYPE_union _tmpDD0;yyval=(union
Cyc_YYSTYPE_union)(((_tmpDD0.YY29).tag=33,(((_tmpDD0.YY29).f1=((_tmpDD6=
_cycalloc(sizeof(*_tmpDD6)),((_tmpDD6->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpDD6->tms=((
_tmpDD1=_cycalloc(sizeof(*_tmpDD1)),((_tmpDD1->hd=(void*)((void*)((_tmpDD5=
_cycalloc(sizeof(*_tmpDD5)),((_tmpDD5[0]=((_tmpDD2.tag=3,((_tmpDD2.f1=(void*)((
void*)((_tmpDD4=_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4[0]=((_tmpDD3.tag=0,((
_tmpDD3.f1=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),((_tmpDD3.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpDD3)))))),
_tmpDD4))))),_tmpDD2)))),_tmpDD5))))),((_tmpDD1->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmpDD1)))))),
_tmpDD6)))))),_tmpDD0))));}break;case 173: _LL2AD: {struct Cyc_List_List*_tmp59A=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Parse_Declarator*
_tmpDE5;struct Cyc_Absyn_TypeParams_mod_struct*_tmpDE4;struct Cyc_Absyn_TypeParams_mod_struct
_tmpDE3;struct Cyc_List_List*_tmpDE2;union Cyc_YYSTYPE_union _tmpDE1;yyval=(union
Cyc_YYSTYPE_union)(((_tmpDE1.YY29).tag=33,(((_tmpDE1.YY29).f1=((_tmpDE5=
_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpDE5->tms=((
_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2->hd=(void*)((void*)((_tmpDE4=
_cycalloc(sizeof(*_tmpDE4)),((_tmpDE4[0]=((_tmpDE3.tag=4,((_tmpDE3.f1=_tmp59A,((
_tmpDE3.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpDE3.f3=0,_tmpDE3)))))))),_tmpDE4))))),((_tmpDE2->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpDE2)))))),_tmpDE5)))))),_tmpDE1))));}break;}case 174: _LL2AE:{struct Cyc_Parse_Declarator*
_tmpDF4;struct Cyc_Absyn_Attributes_mod_struct*_tmpDF3;struct Cyc_Absyn_Attributes_mod_struct
_tmpDF2;struct Cyc_List_List*_tmpDF1;union Cyc_YYSTYPE_union _tmpDF0;yyval=(union
Cyc_YYSTYPE_union)(((_tmpDF0.YY29).tag=33,(((_tmpDF0.YY29).f1=((_tmpDF4=
_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpDF4->tms=((
_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1->hd=(void*)((void*)((_tmpDF3=
_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3[0]=((_tmpDF2.tag=5,((_tmpDF2.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpDF2.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDF2)))))),_tmpDF3))))),((
_tmpDF1->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpDF1)))))),_tmpDF4)))))),_tmpDF0))));}break;case 175:
_LL2AF: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
176: _LL2B0:{union Cyc_YYSTYPE_union _tmpDF5;yyval=(union Cyc_YYSTYPE_union)(((
_tmpDF5.YY28).tag=32,(((_tmpDF5.YY28).f1=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpDF5))));}break;case 177:
_LL2B1: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){struct Cyc_Absyn_Attributes_mod_struct*
_tmpDFB;struct Cyc_Absyn_Attributes_mod_struct _tmpDFA;struct Cyc_List_List*_tmpDF9;
ans=((_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9->hd=(void*)((void*)((_tmpDFB=
_cycalloc(sizeof(*_tmpDFB)),((_tmpDFB[0]=((_tmpDFA.tag=5,((_tmpDFA.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpDFA.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpDFA)))))),_tmpDFB))))),((_tmpDF9->tl=ans,_tmpDF9))))));}{struct Cyc_Absyn_PtrLoc*
ptrloc=0;if(Cyc_Absyn_porting_c_code){struct Cyc_Absyn_PtrLoc*_tmpDFC;ptrloc=((
_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC->ptr_loc=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1,((_tmpDFC->rgn_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpDFC->zt_loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),_tmpDFC))))))));}{
struct Cyc_Absyn_Pointer_mod_struct*_tmpE06;struct Cyc_Absyn_PtrAtts _tmpE05;struct
Cyc_Absyn_Pointer_mod_struct _tmpE04;struct Cyc_List_List*_tmpE03;ans=((_tmpE03=
_cycalloc(sizeof(*_tmpE03)),((_tmpE03->hd=(void*)((void*)((_tmpE06=_cycalloc(
sizeof(*_tmpE06)),((_tmpE06[0]=((_tmpE04.tag=2,((_tmpE04.f1=((_tmpE05.rgn=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpE05.nullable=(*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])).f2,((_tmpE05.bounds=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f3,((_tmpE05.zero_term=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpE05.ptrloc=ptrloc,_tmpE05)))))))))),((_tmpE04.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE04)))))),_tmpE06))))),((
_tmpE03->tl=ans,_tmpE03))))));}{union Cyc_YYSTYPE_union _tmpE07;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE07.YY28).tag=32,(((_tmpE07.YY28).f1=ans,_tmpE07))));}break;}}case 178: _LL2B2: {
struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line): 0;{struct
_tuple12*_tmpE0A;union Cyc_YYSTYPE_union _tmpE09;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE09.YY1).tag=5,(((_tmpE09.YY1).f1=((_tmpE0A=_cycalloc(sizeof(*_tmpE0A)),((
_tmpE0A->f1=loc,((_tmpE0A->f2=Cyc_Absyn_true_conref,((_tmpE0A->f3=Cyc_yyget_YY2(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE0A)))))))),
_tmpE09))));}break;}case 179: _LL2B3: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line): 0;{struct _tuple12*_tmpE0D;union Cyc_YYSTYPE_union
_tmpE0C;yyval=(union Cyc_YYSTYPE_union)(((_tmpE0C.YY1).tag=5,(((_tmpE0C.YY1).f1=((
_tmpE0D=_cycalloc(sizeof(*_tmpE0D)),((_tmpE0D->f1=loc,((_tmpE0D->f2=Cyc_Absyn_false_conref,((
_tmpE0D->f3=Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpE0D)))))))),_tmpE0C))));}break;}case 180: _LL2B4: {struct Cyc_Position_Segment*
loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line): 0;{struct
_tuple12*_tmpE10;union Cyc_YYSTYPE_union _tmpE0F;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE0F.YY1).tag=5,(((_tmpE0F.YY1).f1=((_tmpE10=_cycalloc(sizeof(*_tmpE10)),((
_tmpE10->f1=loc,((_tmpE10->f2=Cyc_Absyn_true_conref,((_tmpE10->f3=Cyc_Absyn_bounds_dyneither_conref,
_tmpE10)))))))),_tmpE0F))));}break;}case 181: _LL2B5:{union Cyc_YYSTYPE_union
_tmpE11;yyval=(union Cyc_YYSTYPE_union)(((_tmpE11.YY2).tag=6,(((_tmpE11.YY2).f1=
Cyc_Absyn_bounds_one_conref,_tmpE11))));}break;case 182: _LL2B6:{struct Cyc_Absyn_Upper_b_struct*
_tmpE17;struct Cyc_Absyn_Upper_b_struct _tmpE16;union Cyc_YYSTYPE_union _tmpE15;
yyval=(union Cyc_YYSTYPE_union)(((_tmpE15.YY2).tag=6,(((_tmpE15.YY2).f1=Cyc_Absyn_new_conref((
void*)((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17[0]=((_tmpE16.tag=0,((
_tmpE16.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]),_tmpE16)))),_tmpE17))))),_tmpE15))));}break;case 183: _LL2B7:{union Cyc_YYSTYPE_union
_tmpE18;yyval=(union Cyc_YYSTYPE_union)(((_tmpE18.YY51).tag=55,(((_tmpE18.YY51).f1=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),_tmpE18))));}break;case
184: _LL2B8:{union Cyc_YYSTYPE_union _tmpE19;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE19.YY51).tag=55,(((_tmpE19.YY51).f1=Cyc_Absyn_true_conref,_tmpE19))));}
break;case 185: _LL2B9:{union Cyc_YYSTYPE_union _tmpE1A;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE1A.YY51).tag=55,(((_tmpE1A.YY51).f1=Cyc_Absyn_false_conref,_tmpE1A))));}
break;case 186: _LL2BA:{union Cyc_YYSTYPE_union _tmpE1B;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE1B.YY49).tag=53,(((_tmpE1B.YY49).f1=0,_tmpE1B))));}break;case 187: _LL2BB: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)
3,1);{struct Cyc_Core_Opt*_tmpE1E;union Cyc_YYSTYPE_union _tmpE1D;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE1D.YY49).tag=53,(((_tmpE1D.YY49).f1=((_tmpE1E=_cycalloc(sizeof(*_tmpE1E)),((
_tmpE1E->v=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE1E)))),_tmpE1D))));}break;case 188: _LL2BC:{struct Cyc_Core_Opt*
_tmpE21;union Cyc_YYSTYPE_union _tmpE20;yyval=(union Cyc_YYSTYPE_union)(((_tmpE20.YY49).tag=
53,(((_tmpE20.YY49).f1=((_tmpE21=_cycalloc(sizeof(*_tmpE21)),((_tmpE21->v=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0),_tmpE21)))),_tmpE20))));}
break;case 189: _LL2BD:{union Cyc_YYSTYPE_union _tmpE22;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE22.YY44).tag=48,(((_tmpE22.YY44).f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,0),_tmpE22))));}break;case 190: _LL2BE: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)
5,1);yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
191: _LL2BF:{union Cyc_YYSTYPE_union _tmpE23;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE23.YY44).tag=48,(((_tmpE23.YY44).f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,0),_tmpE23))));}break;case 192: _LL2C0:{union Cyc_YYSTYPE_union
_tmpE24;yyval=(union Cyc_YYSTYPE_union)(((_tmpE24.YY25).tag=29,(((_tmpE24.YY25).f1=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line)),_tmpE24))));}
break;case 193: _LL2C1:{union Cyc_YYSTYPE_union _tmpE25;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE25.YY25).tag=29,(((_tmpE25.YY25).f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpE25))));}break;case 194:
_LL2C2:{struct _tuple17*_tmpE28;union Cyc_YYSTYPE_union _tmpE27;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE27.YY39).tag=43,(((_tmpE27.YY39).f1=((_tmpE28=_cycalloc(sizeof(*_tmpE28)),((
_tmpE28->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((
_tmpE28->f2=0,((_tmpE28->f3=0,((_tmpE28->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE28->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE28)))))))))))),
_tmpE27))));}break;case 195: _LL2C3:{struct _tuple17*_tmpE2B;union Cyc_YYSTYPE_union
_tmpE2A;yyval=(union Cyc_YYSTYPE_union)(((_tmpE2A.YY39).tag=43,(((_tmpE2A.YY39).f1=((
_tmpE2B=_cycalloc(sizeof(*_tmpE2B)),((_tmpE2B->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),((_tmpE2B->f2=1,((
_tmpE2B->f3=0,((_tmpE2B->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((_tmpE2B->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE2B)))))))))))),_tmpE2A))));}
break;case 196: _LL2C4: {struct _tuple2 _tmp5CA;struct Cyc_Core_Opt*_tmp5CB;struct Cyc_Absyn_Tqual
_tmp5CC;void*_tmp5CD;struct _tuple2*_tmp5C9=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5CA=*_tmp5C9;_tmp5CB=
_tmp5CA.f1;_tmp5CC=_tmp5CA.f2;_tmp5CD=_tmp5CA.f3;{struct Cyc_Absyn_VarargInfo*
_tmpE2C;struct Cyc_Absyn_VarargInfo*_tmp5CE=(_tmpE2C=_cycalloc(sizeof(*_tmpE2C)),((
_tmpE2C->name=_tmp5CB,((_tmpE2C->tq=_tmp5CC,((_tmpE2C->type=(void*)_tmp5CD,((
_tmpE2C->inject=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),_tmpE2C)))))))));{struct _tuple17*_tmpE2F;union Cyc_YYSTYPE_union
_tmpE2E;yyval=(union Cyc_YYSTYPE_union)(((_tmpE2E.YY39).tag=43,(((_tmpE2E.YY39).f1=((
_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F->f1=0,((_tmpE2F->f2=0,((_tmpE2F->f3=
_tmp5CE,((_tmpE2F->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpE2F->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE2F)))))))))))),_tmpE2E))));}
break;}}case 197: _LL2C5: {struct _tuple2 _tmp5D3;struct Cyc_Core_Opt*_tmp5D4;struct
Cyc_Absyn_Tqual _tmp5D5;void*_tmp5D6;struct _tuple2*_tmp5D2=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5D3=*_tmp5D2;_tmp5D4=
_tmp5D3.f1;_tmp5D5=_tmp5D3.f2;_tmp5D6=_tmp5D3.f3;{struct Cyc_Absyn_VarargInfo*
_tmpE30;struct Cyc_Absyn_VarargInfo*_tmp5D7=(_tmpE30=_cycalloc(sizeof(*_tmpE30)),((
_tmpE30->name=_tmp5D4,((_tmpE30->tq=_tmp5D5,((_tmpE30->type=(void*)_tmp5D6,((
_tmpE30->inject=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),_tmpE30)))))))));{struct _tuple17*_tmpE33;union Cyc_YYSTYPE_union
_tmpE32;yyval=(union Cyc_YYSTYPE_union)(((_tmpE32.YY39).tag=43,(((_tmpE32.YY39).f1=((
_tmpE33=_cycalloc(sizeof(*_tmpE33)),((_tmpE33->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)])),((_tmpE33->f2=0,((
_tmpE33->f3=_tmp5D7,((_tmpE33->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE33->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE33)))))))))))),
_tmpE32))));}break;}}case 198: _LL2C6:{struct Cyc_Absyn_Unknown_kb_struct*_tmpE39;
struct Cyc_Absyn_Unknown_kb_struct _tmpE38;union Cyc_YYSTYPE_union _tmpE37;yyval=(
union Cyc_YYSTYPE_union)(((_tmpE37.YY44).tag=48,(((_tmpE37.YY44).f1=(void*)Cyc_Parse_id2type(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)((_tmpE39=_cycalloc(sizeof(*_tmpE39)),((_tmpE39[0]=((_tmpE38.tag=1,((
_tmpE38.f1=0,_tmpE38)))),_tmpE39))))),_tmpE37))));}break;case 199: _LL2C7:{union
Cyc_YYSTYPE_union _tmpE3A;yyval=(union Cyc_YYSTYPE_union)(((_tmpE3A.YY44).tag=48,(((
_tmpE3A.YY44).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))),
_tmpE3A))));}break;case 200: _LL2C8:{union Cyc_YYSTYPE_union _tmpE3B;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE3B.YY49).tag=53,(((_tmpE3B.YY49).f1=0,_tmpE3B))));}break;case 201: _LL2C9:{
struct Cyc_Core_Opt*_tmpE45;struct Cyc_Absyn_JoinEff_struct _tmpE44;struct Cyc_Absyn_JoinEff_struct*
_tmpE43;union Cyc_YYSTYPE_union _tmpE42;yyval=(union Cyc_YYSTYPE_union)(((_tmpE42.YY49).tag=
53,(((_tmpE42.YY49).f1=((_tmpE45=_cycalloc(sizeof(*_tmpE45)),((_tmpE45->v=(void*)((
void*)((_tmpE43=_cycalloc(sizeof(*_tmpE43)),((_tmpE43[0]=((_tmpE44.tag=20,((
_tmpE44.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpE44)))),_tmpE43))))),_tmpE45)))),_tmpE42))));}break;case 202: _LL2CA:{union Cyc_YYSTYPE_union
_tmpE46;yyval=(union Cyc_YYSTYPE_union)(((_tmpE46.YY50).tag=54,(((_tmpE46.YY50).f1=
0,_tmpE46))));}break;case 203: _LL2CB: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 204: _LL2CC: {struct Cyc_Absyn_Less_kb_struct
_tmpE49;struct Cyc_Absyn_Less_kb_struct*_tmpE48;struct Cyc_Absyn_Less_kb_struct*
_tmp5E5=(_tmpE48=_cycalloc(sizeof(*_tmpE48)),((_tmpE48[0]=((_tmpE49.tag=2,((
_tmpE49.f1=0,((_tmpE49.f2=(void*)((void*)5),_tmpE49)))))),_tmpE48)));struct
_dyneither_ptr _tmp5E6=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);void*_tmp5E7=Cyc_Parse_id2type(_tmp5E6,(void*)_tmp5E5);{
struct Cyc_List_List*_tmpE58;struct Cyc_Absyn_JoinEff_struct*_tmpE57;struct Cyc_Absyn_JoinEff_struct
_tmpE56;struct _tuple4*_tmpE55;union Cyc_YYSTYPE_union _tmpE54;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE54.YY50).tag=54,(((_tmpE54.YY50).f1=((_tmpE58=_cycalloc(sizeof(*_tmpE58)),((
_tmpE58->hd=((_tmpE55=_cycalloc(sizeof(*_tmpE55)),((_tmpE55->f1=(void*)((_tmpE57=
_cycalloc(sizeof(*_tmpE57)),((_tmpE57[0]=((_tmpE56.tag=20,((_tmpE56.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE56)))),_tmpE57)))),((
_tmpE55->f2=_tmp5E7,_tmpE55)))))),((_tmpE58->tl=0,_tmpE58)))))),_tmpE54))));}
break;}case 205: _LL2CD: {struct Cyc_Absyn_Less_kb_struct _tmpE5B;struct Cyc_Absyn_Less_kb_struct*
_tmpE5A;struct Cyc_Absyn_Less_kb_struct*_tmp5EF=(_tmpE5A=_cycalloc(sizeof(*
_tmpE5A)),((_tmpE5A[0]=((_tmpE5B.tag=2,((_tmpE5B.f1=0,((_tmpE5B.f2=(void*)((void*)
5),_tmpE5B)))))),_tmpE5A)));struct _dyneither_ptr _tmp5F0=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);void*_tmp5F1=Cyc_Parse_id2type(
_tmp5F0,(void*)_tmp5EF);{struct Cyc_List_List*_tmpE6A;struct Cyc_Absyn_JoinEff_struct*
_tmpE69;struct Cyc_Absyn_JoinEff_struct _tmpE68;struct _tuple4*_tmpE67;union Cyc_YYSTYPE_union
_tmpE66;yyval=(union Cyc_YYSTYPE_union)(((_tmpE66.YY50).tag=54,(((_tmpE66.YY50).f1=((
_tmpE6A=_cycalloc(sizeof(*_tmpE6A)),((_tmpE6A->hd=((_tmpE67=_cycalloc(sizeof(*
_tmpE67)),((_tmpE67->f1=(void*)((_tmpE69=_cycalloc(sizeof(*_tmpE69)),((_tmpE69[0]=((
_tmpE68.tag=20,((_tmpE68.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),_tmpE68)))),_tmpE69)))),((_tmpE67->f2=_tmp5F1,_tmpE67)))))),((
_tmpE6A->tl=Cyc_yyget_YY50(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE6A)))))),_tmpE66))));}break;}case 206: _LL2CE:{union Cyc_YYSTYPE_union
_tmpE6B;yyval=(union Cyc_YYSTYPE_union)(((_tmpE6B.YY31).tag=35,(((_tmpE6B.YY31).f1=
0,_tmpE6B))));}break;case 207: _LL2CF:{const char*_tmpE6C;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpE6C="inject",_tag_dyneither(_tmpE6C,sizeof(char),7))))!= 0){
const char*_tmpE6D;Cyc_Parse_err(((_tmpE6D="missing type in function declaration",
_tag_dyneither(_tmpE6D,sizeof(char),37))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}{union Cyc_YYSTYPE_union
_tmpE6E;yyval=(union Cyc_YYSTYPE_union)(((_tmpE6E.YY31).tag=35,(((_tmpE6E.YY31).f1=
1,_tmpE6E))));}break;case 208: _LL2D0: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 209: _LL2D1:{union Cyc_YYSTYPE_union _tmpE6F;yyval=(
union Cyc_YYSTYPE_union)(((_tmpE6F.YY40).tag=44,(((_tmpE6F.YY40).f1=Cyc_List_imp_append(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpE6F))));}break;
case 210: _LL2D2:{union Cyc_YYSTYPE_union _tmpE70;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE70.YY40).tag=44,(((_tmpE70.YY40).f1=0,_tmpE70))));}break;case 211: _LL2D3:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 212:
_LL2D4:{struct Cyc_List_List*_tmpE7A;struct Cyc_Absyn_RgnsEff_struct _tmpE79;struct
Cyc_Absyn_RgnsEff_struct*_tmpE78;union Cyc_YYSTYPE_union _tmpE77;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE77.YY40).tag=44,(((_tmpE77.YY40).f1=((_tmpE7A=_cycalloc(sizeof(*_tmpE7A)),((
_tmpE7A->hd=(void*)((void*)((_tmpE78=_cycalloc(sizeof(*_tmpE78)),((_tmpE78[0]=((
_tmpE79.tag=21,((_tmpE79.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE79)))),_tmpE78))))),((
_tmpE7A->tl=0,_tmpE7A)))))),_tmpE77))));}break;case 213: _LL2D5: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)
6,0);{struct Cyc_List_List*_tmpE7D;union Cyc_YYSTYPE_union _tmpE7C;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE7C.YY40).tag=44,(((_tmpE7C.YY40).f1=((_tmpE7D=_cycalloc(sizeof(*_tmpE7D)),((
_tmpE7D->hd=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpE7D->tl=0,_tmpE7D)))))),_tmpE7C))));}break;case 214: _LL2D6:
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),(void*)5,1);{struct Cyc_List_List*_tmpE87;struct Cyc_Absyn_AccessEff_struct
_tmpE86;struct Cyc_Absyn_AccessEff_struct*_tmpE85;union Cyc_YYSTYPE_union _tmpE84;
yyval=(union Cyc_YYSTYPE_union)(((_tmpE84.YY40).tag=44,(((_tmpE84.YY40).f1=((
_tmpE87=_cycalloc(sizeof(*_tmpE87)),((_tmpE87->hd=(void*)((void*)((_tmpE85=
_cycalloc(sizeof(*_tmpE85)),((_tmpE85[0]=((_tmpE86.tag=19,((_tmpE86.f1=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE86)))),
_tmpE85))))),((_tmpE87->tl=0,_tmpE87)))))),_tmpE84))));}break;case 215: _LL2D7: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
void*)5,1);{struct Cyc_List_List*_tmpE91;struct Cyc_Absyn_AccessEff_struct _tmpE90;
struct Cyc_Absyn_AccessEff_struct*_tmpE8F;union Cyc_YYSTYPE_union _tmpE8E;yyval=(
union Cyc_YYSTYPE_union)(((_tmpE8E.YY40).tag=44,(((_tmpE8E.YY40).f1=((_tmpE91=
_cycalloc(sizeof(*_tmpE91)),((_tmpE91->hd=(void*)((void*)((_tmpE8F=_cycalloc(
sizeof(*_tmpE8F)),((_tmpE8F[0]=((_tmpE90.tag=19,((_tmpE90.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE90)))),_tmpE8F))))),((
_tmpE91->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE91)))))),_tmpE8E))));}break;case 216: _LL2D8:{struct Cyc_List_List*
_tmpE94;union Cyc_YYSTYPE_union _tmpE93;yyval=(union Cyc_YYSTYPE_union)(((_tmpE93.YY38).tag=
42,(((_tmpE93.YY38).f1=((_tmpE94=_cycalloc(sizeof(*_tmpE94)),((_tmpE94->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpE94->tl=0,
_tmpE94)))))),_tmpE93))));}break;case 217: _LL2D9:{struct Cyc_List_List*_tmpE97;
union Cyc_YYSTYPE_union _tmpE96;yyval=(union Cyc_YYSTYPE_union)(((_tmpE96.YY38).tag=
42,(((_tmpE96.YY38).f1=((_tmpE97=_cycalloc(sizeof(*_tmpE97)),((_tmpE97->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpE97->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE97)))))),
_tmpE96))));}break;case 218: _LL2DA: {struct _tuple16 _tmp612;struct Cyc_Absyn_Tqual
_tmp613;struct Cyc_List_List*_tmp614;struct Cyc_List_List*_tmp615;struct _tuple16*
_tmp611=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp612=*_tmp611;_tmp613=_tmp612.f1;_tmp614=_tmp612.f2;_tmp615=_tmp612.f3;if(
_tmp613.loc == 0)_tmp613.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator
_tmp617;struct _tuple1*_tmp618;struct Cyc_List_List*_tmp619;struct Cyc_Parse_Declarator*
_tmp616=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp617=*_tmp616;_tmp618=_tmp617.id;_tmp619=_tmp617.tms;{void*_tmp61A=Cyc_Parse_speclist2typ(
_tmp614,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp61C;void*_tmp61D;struct
Cyc_List_List*_tmp61E;struct Cyc_List_List*_tmp61F;struct _tuple6 _tmp61B=Cyc_Parse_apply_tms(
_tmp613,_tmp61A,_tmp615,_tmp619);_tmp61C=_tmp61B.f1;_tmp61D=_tmp61B.f2;_tmp61E=
_tmp61B.f3;_tmp61F=_tmp61B.f4;if(_tmp61E != 0){const char*_tmpE98;Cyc_Parse_err(((
_tmpE98="parameter with bad type params",_tag_dyneither(_tmpE98,sizeof(char),31))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}if(Cyc_Absyn_is_qvar_qualified(_tmp618)){const char*
_tmpE99;Cyc_Parse_err(((_tmpE99="parameter cannot be qualified with a namespace",
_tag_dyneither(_tmpE99,sizeof(char),47))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Core_Opt*
_tmpE9A;struct Cyc_Core_Opt*_tmp622=(struct Cyc_Core_Opt*)((_tmpE9A=_cycalloc(
sizeof(*_tmpE9A)),((_tmpE9A->v=(*_tmp618).f2,_tmpE9A))));if(_tmp61F != 0){const
char*_tmpE9D;void*_tmpE9C;(_tmpE9C=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpE9D="extra attributes on parameter, ignoring",
_tag_dyneither(_tmpE9D,sizeof(char),40))),_tag_dyneither(_tmpE9C,sizeof(void*),0)));}{
struct _tuple2*_tmpEA0;union Cyc_YYSTYPE_union _tmpE9F;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE9F.YY37).tag=41,(((_tmpE9F.YY37).f1=((_tmpEA0=_cycalloc(sizeof(*_tmpEA0)),((
_tmpEA0->f1=_tmp622,((_tmpEA0->f2=_tmp61C,((_tmpEA0->f3=_tmp61D,_tmpEA0)))))))),
_tmpE9F))));}break;}}}}case 219: _LL2DB: {struct _tuple16 _tmp629;struct Cyc_Absyn_Tqual
_tmp62A;struct Cyc_List_List*_tmp62B;struct Cyc_List_List*_tmp62C;struct _tuple16*
_tmp628=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp629=*_tmp628;_tmp62A=_tmp629.f1;_tmp62B=_tmp629.f2;_tmp62C=_tmp629.f3;if(
_tmp62A.loc == 0)_tmp62A.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);{void*_tmp62D=
Cyc_Parse_speclist2typ(_tmp62B,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp62C != 0){
const char*_tmpEA3;void*_tmpEA2;(_tmpEA2=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpEA3="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpEA3,sizeof(char),38))),_tag_dyneither(_tmpEA2,sizeof(void*),0)));}{
struct _tuple2*_tmpEA6;union Cyc_YYSTYPE_union _tmpEA5;yyval=(union Cyc_YYSTYPE_union)(((
_tmpEA5.YY37).tag=41,(((_tmpEA5.YY37).f1=((_tmpEA6=_cycalloc(sizeof(*_tmpEA6)),((
_tmpEA6->f1=0,((_tmpEA6->f2=_tmp62A,((_tmpEA6->f3=_tmp62D,_tmpEA6)))))))),
_tmpEA5))));}break;}}case 220: _LL2DC: {struct _tuple16 _tmp633;struct Cyc_Absyn_Tqual
_tmp634;struct Cyc_List_List*_tmp635;struct Cyc_List_List*_tmp636;struct _tuple16*
_tmp632=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp633=*_tmp632;_tmp634=_tmp633.f1;_tmp635=_tmp633.f2;_tmp636=_tmp633.f3;if(
_tmp634.loc == 0)_tmp634.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{void*_tmp637=
Cyc_Parse_speclist2typ(_tmp635,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp638=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp63A;void*_tmp63B;struct Cyc_List_List*_tmp63C;struct Cyc_List_List*
_tmp63D;struct _tuple6 _tmp639=Cyc_Parse_apply_tms(_tmp634,_tmp637,_tmp636,_tmp638);
_tmp63A=_tmp639.f1;_tmp63B=_tmp639.f2;_tmp63C=_tmp639.f3;_tmp63D=_tmp639.f4;if(
_tmp63C != 0){const char*_tmpEA9;void*_tmpEA8;(_tmpEA8=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpEA9="bad type parameters on formal argument, ignoring",
_tag_dyneither(_tmpEA9,sizeof(char),49))),_tag_dyneither(_tmpEA8,sizeof(void*),0)));}
if(_tmp63D != 0){const char*_tmpEAC;void*_tmpEAB;(_tmpEAB=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpEAC="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpEAC,sizeof(char),38))),_tag_dyneither(_tmpEAB,sizeof(void*),0)));}{
struct _tuple2*_tmpEAF;union Cyc_YYSTYPE_union _tmpEAE;yyval=(union Cyc_YYSTYPE_union)(((
_tmpEAE.YY37).tag=41,(((_tmpEAE.YY37).f1=((_tmpEAF=_cycalloc(sizeof(*_tmpEAF)),((
_tmpEAF->f1=0,((_tmpEAF->f2=_tmp63A,((_tmpEAF->f3=_tmp63B,_tmpEAF)))))))),
_tmpEAE))));}break;}}case 221: _LL2DD:{union Cyc_YYSTYPE_union _tmpEB0;yyval=(union
Cyc_YYSTYPE_union)(((_tmpEB0.YY36).tag=40,(((_tmpEB0.YY36).f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpEB0))));}break;case 222:
_LL2DE:{struct Cyc_List_List*_tmpEB6;struct _dyneither_ptr*_tmpEB5;union Cyc_YYSTYPE_union
_tmpEB4;yyval=(union Cyc_YYSTYPE_union)(((_tmpEB4.YY36).tag=40,(((_tmpEB4.YY36).f1=((
_tmpEB6=_cycalloc(sizeof(*_tmpEB6)),((_tmpEB6->hd=((_tmpEB5=_cycalloc(sizeof(*
_tmpEB5)),((_tmpEB5[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),_tmpEB5)))),((_tmpEB6->tl=0,_tmpEB6)))))),_tmpEB4))));}
break;case 223: _LL2DF:{struct Cyc_List_List*_tmpEBC;struct _dyneither_ptr*_tmpEBB;
union Cyc_YYSTYPE_union _tmpEBA;yyval=(union Cyc_YYSTYPE_union)(((_tmpEBA.YY36).tag=
40,(((_tmpEBA.YY36).f1=((_tmpEBC=_cycalloc(sizeof(*_tmpEBC)),((_tmpEBC->hd=((
_tmpEBB=_cycalloc(sizeof(*_tmpEBB)),((_tmpEBB[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEBB)))),((_tmpEBC->tl=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpEBC)))))),_tmpEBA))));}break;case 224: _LL2E0: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 225: _LL2E1: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 226: _LL2E2:{
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpEC2;struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpEC1;union Cyc_YYSTYPE_union _tmpEC0;yyval=(union Cyc_YYSTYPE_union)(((_tmpEC0.YY3).tag=
7,(((_tmpEC0.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmpEC2=_cycalloc(sizeof(*
_tmpEC2)),((_tmpEC2[0]=((_tmpEC1.tag=37,((_tmpEC1.f1=0,((_tmpEC1.f2=0,_tmpEC1)))))),
_tmpEC2)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpEC0))));}break;case 227: _LL2E3:{struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpEC8;struct Cyc_Absyn_UnresolvedMem_e_struct _tmpEC7;union Cyc_YYSTYPE_union
_tmpEC6;yyval=(union Cyc_YYSTYPE_union)(((_tmpEC6.YY3).tag=7,(((_tmpEC6.YY3).f1=
Cyc_Absyn_new_exp((void*)((_tmpEC8=_cycalloc(sizeof(*_tmpEC8)),((_tmpEC8[0]=((
_tmpEC7.tag=37,((_tmpEC7.f1=0,((_tmpEC7.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])),_tmpEC7)))))),_tmpEC8)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpEC6))));}
break;case 228: _LL2E4:{struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpECE;struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpECD;union Cyc_YYSTYPE_union _tmpECC;yyval=(union Cyc_YYSTYPE_union)(((_tmpECC.YY3).tag=
7,(((_tmpECC.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmpECE=_cycalloc(sizeof(*
_tmpECE)),((_tmpECE[0]=((_tmpECD.tag=37,((_tmpECD.f1=0,((_tmpECD.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),_tmpECD)))))),_tmpECE)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpECC))));}break;case 229: _LL2E5: {union Cyc_Absyn_Nmspace_union
_tmpED3;struct _dyneither_ptr*_tmpED2;struct _tuple1*_tmpED1;struct Cyc_Absyn_Vardecl*
_tmp654=Cyc_Absyn_new_vardecl(((_tmpED1=_cycalloc(sizeof(*_tmpED1)),((_tmpED1->f1=(
union Cyc_Absyn_Nmspace_union)(((_tmpED3.Loc_n).tag=0,_tmpED3)),((_tmpED1->f2=((
_tmpED2=_cycalloc(sizeof(*_tmpED2)),((_tmpED2[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpED2)))),_tmpED1)))))),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp654->tq).real_const=
1;{struct Cyc_Absyn_Comprehension_e_struct*_tmpED9;struct Cyc_Absyn_Comprehension_e_struct
_tmpED8;union Cyc_YYSTYPE_union _tmpED7;yyval=(union Cyc_YYSTYPE_union)(((_tmpED7.YY3).tag=
7,(((_tmpED7.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmpED9=_cycalloc(sizeof(*
_tmpED9)),((_tmpED9[0]=((_tmpED8.tag=29,((_tmpED8.f1=_tmp654,((_tmpED8.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpED8.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpED8.f4=0,
_tmpED8)))))))))),_tmpED9)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpED7))));}
break;}case 230: _LL2E6:{struct Cyc_List_List*_tmpEDF;struct _tuple20*_tmpEDE;union
Cyc_YYSTYPE_union _tmpEDD;yyval=(union Cyc_YYSTYPE_union)(((_tmpEDD.YY6).tag=10,(((
_tmpEDD.YY6).f1=((_tmpEDF=_cycalloc(sizeof(*_tmpEDF)),((_tmpEDF->hd=((_tmpEDE=
_cycalloc(sizeof(*_tmpEDE)),((_tmpEDE->f1=0,((_tmpEDE->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEDE)))))),((_tmpEDF->tl=
0,_tmpEDF)))))),_tmpEDD))));}break;case 231: _LL2E7:{struct Cyc_List_List*_tmpEE5;
struct _tuple20*_tmpEE4;union Cyc_YYSTYPE_union _tmpEE3;yyval=(union Cyc_YYSTYPE_union)(((
_tmpEE3.YY6).tag=10,(((_tmpEE3.YY6).f1=((_tmpEE5=_cycalloc(sizeof(*_tmpEE5)),((
_tmpEE5->hd=((_tmpEE4=_cycalloc(sizeof(*_tmpEE4)),((_tmpEE4->f1=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpEE4->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEE4)))))),((
_tmpEE5->tl=0,_tmpEE5)))))),_tmpEE3))));}break;case 232: _LL2E8:{struct Cyc_List_List*
_tmpEEB;struct _tuple20*_tmpEEA;union Cyc_YYSTYPE_union _tmpEE9;yyval=(union Cyc_YYSTYPE_union)(((
_tmpEE9.YY6).tag=10,(((_tmpEE9.YY6).f1=((_tmpEEB=_cycalloc(sizeof(*_tmpEEB)),((
_tmpEEB->hd=((_tmpEEA=_cycalloc(sizeof(*_tmpEEA)),((_tmpEEA->f1=0,((_tmpEEA->f2=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEEA)))))),((
_tmpEEB->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]),_tmpEEB)))))),_tmpEE9))));}break;case 233: _LL2E9:{struct Cyc_List_List*
_tmpEF1;struct _tuple20*_tmpEF0;union Cyc_YYSTYPE_union _tmpEEF;yyval=(union Cyc_YYSTYPE_union)(((
_tmpEEF.YY6).tag=10,(((_tmpEEF.YY6).f1=((_tmpEF1=_cycalloc(sizeof(*_tmpEF1)),((
_tmpEF1->hd=((_tmpEF0=_cycalloc(sizeof(*_tmpEF0)),((_tmpEF0->f1=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpEF0->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEF0)))))),((
_tmpEF1->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]),_tmpEF1)))))),_tmpEEF))));}break;case 234: _LL2EA:{union Cyc_YYSTYPE_union
_tmpEF2;yyval=(union Cyc_YYSTYPE_union)(((_tmpEF2.YY41).tag=45,(((_tmpEF2.YY41).f1=
Cyc_List_imp_rev(Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])),_tmpEF2))));}break;case 235: _LL2EB:{struct Cyc_List_List*
_tmpEF5;union Cyc_YYSTYPE_union _tmpEF4;yyval=(union Cyc_YYSTYPE_union)(((_tmpEF4.YY41).tag=
45,(((_tmpEF4.YY41).f1=((_tmpEF5=_cycalloc(sizeof(*_tmpEF5)),((_tmpEF5->hd=(void*)
Cyc_yyget_YY42(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpEF5->tl=0,_tmpEF5)))))),_tmpEF4))));}break;case 236: _LL2EC:{struct Cyc_List_List*
_tmpEF8;union Cyc_YYSTYPE_union _tmpEF7;yyval=(union Cyc_YYSTYPE_union)(((_tmpEF7.YY41).tag=
45,(((_tmpEF7.YY41).f1=((_tmpEF8=_cycalloc(sizeof(*_tmpEF8)),((_tmpEF8->hd=(void*)
Cyc_yyget_YY42(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpEF8->tl=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpEF8)))))),_tmpEF7))));}break;case 237: _LL2ED:{struct Cyc_Absyn_ArrayElement_struct*
_tmpEFE;struct Cyc_Absyn_ArrayElement_struct _tmpEFD;union Cyc_YYSTYPE_union _tmpEFC;
yyval=(union Cyc_YYSTYPE_union)(((_tmpEFC.YY42).tag=46,(((_tmpEFC.YY42).f1=(void*)((
void*)((_tmpEFE=_cycalloc(sizeof(*_tmpEFE)),((_tmpEFE[0]=((_tmpEFD.tag=0,((
_tmpEFD.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]),_tmpEFD)))),_tmpEFE))))),_tmpEFC))));}break;case 238: _LL2EE:{struct Cyc_Absyn_FieldName_struct*
_tmpF08;struct _dyneither_ptr*_tmpF07;struct Cyc_Absyn_FieldName_struct _tmpF06;
union Cyc_YYSTYPE_union _tmpF05;yyval=(union Cyc_YYSTYPE_union)(((_tmpF05.YY42).tag=
46,(((_tmpF05.YY42).f1=(void*)((void*)((_tmpF08=_cycalloc(sizeof(*_tmpF08)),((
_tmpF08[0]=((_tmpF06.tag=1,((_tmpF06.f1=((_tmpF07=_cycalloc(sizeof(*_tmpF07)),((
_tmpF07[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpF07)))),_tmpF06)))),_tmpF08))))),_tmpF05))));}break;case 239:
_LL2EF: {struct _tuple16 _tmp674;struct Cyc_Absyn_Tqual _tmp675;struct Cyc_List_List*
_tmp676;struct Cyc_List_List*_tmp677;struct _tuple16*_tmp673=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp674=*_tmp673;_tmp675=
_tmp674.f1;_tmp676=_tmp674.f2;_tmp677=_tmp674.f3;{void*_tmp678=Cyc_Parse_speclist2typ(
_tmp676,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp677 != 0){const char*_tmpF0B;void*_tmpF0A;(
_tmpF0A=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpF0B="ignoring attributes in type",
_tag_dyneither(_tmpF0B,sizeof(char),28))),_tag_dyneither(_tmpF0A,sizeof(void*),0)));}{
struct _tuple2*_tmpF0E;union Cyc_YYSTYPE_union _tmpF0D;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF0D.YY37).tag=41,(((_tmpF0D.YY37).f1=((_tmpF0E=_cycalloc(sizeof(*_tmpF0E)),((
_tmpF0E->f1=0,((_tmpF0E->f2=_tmp675,((_tmpF0E->f3=_tmp678,_tmpF0E)))))))),
_tmpF0D))));}break;}}case 240: _LL2F0: {struct _tuple16 _tmp67E;struct Cyc_Absyn_Tqual
_tmp67F;struct Cyc_List_List*_tmp680;struct Cyc_List_List*_tmp681;struct _tuple16*
_tmp67D=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp67E=*_tmp67D;_tmp67F=_tmp67E.f1;_tmp680=_tmp67E.f2;_tmp681=_tmp67E.f3;{void*
_tmp682=Cyc_Parse_speclist2typ(_tmp680,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp683=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct _tuple6 _tmp684=Cyc_Parse_apply_tms(_tmp67F,_tmp682,_tmp681,_tmp683);if(
_tmp684.f3 != 0){const char*_tmpF11;void*_tmpF10;(_tmpF10=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpF11="bad type params, ignoring",
_tag_dyneither(_tmpF11,sizeof(char),26))),_tag_dyneither(_tmpF10,sizeof(void*),0)));}
if(_tmp684.f4 != 0){const char*_tmpF14;void*_tmpF13;(_tmpF13=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpF14="bad specifiers, ignoring",
_tag_dyneither(_tmpF14,sizeof(char),25))),_tag_dyneither(_tmpF13,sizeof(void*),0)));}{
struct _tuple2*_tmpF17;union Cyc_YYSTYPE_union _tmpF16;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF16.YY37).tag=41,(((_tmpF16.YY37).f1=((_tmpF17=_cycalloc(sizeof(*_tmpF17)),((
_tmpF17->f1=0,((_tmpF17->f2=_tmp684.f1,((_tmpF17->f3=_tmp684.f2,_tmpF17)))))))),
_tmpF16))));}break;}}case 241: _LL2F1:{union Cyc_YYSTYPE_union _tmpF18;yyval=(union
Cyc_YYSTYPE_union)(((_tmpF18.YY44).tag=48,(((_tmpF18.YY44).f1=(void*)(*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,_tmpF18))));}
break;case 242: _LL2F2:{struct Cyc_Absyn_JoinEff_struct*_tmpF1E;struct Cyc_Absyn_JoinEff_struct
_tmpF1D;union Cyc_YYSTYPE_union _tmpF1C;yyval=(union Cyc_YYSTYPE_union)(((_tmpF1C.YY44).tag=
48,(((_tmpF1C.YY44).f1=(void*)((void*)((_tmpF1E=_cycalloc(sizeof(*_tmpF1E)),((
_tmpF1E[0]=((_tmpF1D.tag=20,((_tmpF1D.f1=0,_tmpF1D)))),_tmpF1E))))),_tmpF1C))));}
break;case 243: _LL2F3:{struct Cyc_Absyn_JoinEff_struct*_tmpF24;struct Cyc_Absyn_JoinEff_struct
_tmpF23;union Cyc_YYSTYPE_union _tmpF22;yyval=(union Cyc_YYSTYPE_union)(((_tmpF22.YY44).tag=
48,(((_tmpF22.YY44).f1=(void*)((void*)((_tmpF24=_cycalloc(sizeof(*_tmpF24)),((
_tmpF24[0]=((_tmpF23.tag=20,((_tmpF23.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpF23)))),_tmpF24))))),
_tmpF22))));}break;case 244: _LL2F4:{struct Cyc_Absyn_RgnsEff_struct*_tmpF2A;struct
Cyc_Absyn_RgnsEff_struct _tmpF29;union Cyc_YYSTYPE_union _tmpF28;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF28.YY44).tag=48,(((_tmpF28.YY44).f1=(void*)((void*)((_tmpF2A=_cycalloc(
sizeof(*_tmpF2A)),((_tmpF2A[0]=((_tmpF29.tag=21,((_tmpF29.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpF29)))),_tmpF2A))))),
_tmpF28))));}break;case 245: _LL2F5:{struct Cyc_Absyn_JoinEff_struct*_tmpF34;struct
Cyc_List_List*_tmpF33;struct Cyc_Absyn_JoinEff_struct _tmpF32;union Cyc_YYSTYPE_union
_tmpF31;yyval=(union Cyc_YYSTYPE_union)(((_tmpF31.YY44).tag=48,(((_tmpF31.YY44).f1=(
void*)((void*)((_tmpF34=_cycalloc(sizeof(*_tmpF34)),((_tmpF34[0]=((_tmpF32.tag=
20,((_tmpF32.f1=((_tmpF33=_cycalloc(sizeof(*_tmpF33)),((_tmpF33->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpF33->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF33)))))),_tmpF32)))),
_tmpF34))))),_tmpF31))));}break;case 246: _LL2F6:{struct Cyc_List_List*_tmpF37;
union Cyc_YYSTYPE_union _tmpF36;yyval=(union Cyc_YYSTYPE_union)(((_tmpF36.YY40).tag=
44,(((_tmpF36.YY40).f1=((_tmpF37=_cycalloc(sizeof(*_tmpF37)),((_tmpF37->hd=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpF37->tl=0,_tmpF37)))))),_tmpF36))));}break;case 247: _LL2F7:{struct Cyc_List_List*
_tmpF3A;union Cyc_YYSTYPE_union _tmpF39;yyval=(union Cyc_YYSTYPE_union)(((_tmpF39.YY40).tag=
44,(((_tmpF39.YY40).f1=((_tmpF3A=_cycalloc(sizeof(*_tmpF3A)),((_tmpF3A->hd=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpF3A->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),_tmpF3A)))))),_tmpF39))));}break;case 248: _LL2F8:{struct Cyc_Parse_Abstractdeclarator*
_tmpF3D;union Cyc_YYSTYPE_union _tmpF3C;yyval=(union Cyc_YYSTYPE_union)(((_tmpF3C.YY30).tag=
34,(((_tmpF3C.YY30).f1=((_tmpF3D=_cycalloc(sizeof(*_tmpF3D)),((_tmpF3D->tms=Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF3D)))),_tmpF3C))));}
break;case 249: _LL2F9: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 250: _LL2FA:{struct Cyc_Parse_Abstractdeclarator*_tmpF40;
union Cyc_YYSTYPE_union _tmpF3F;yyval=(union Cyc_YYSTYPE_union)(((_tmpF3F.YY30).tag=
34,(((_tmpF3F.YY30).f1=((_tmpF40=_cycalloc(sizeof(*_tmpF40)),((_tmpF40->tms=Cyc_List_imp_append(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpF40)))),
_tmpF3F))));}break;case 251: _LL2FB: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 252: _LL2FC:{struct Cyc_Parse_Abstractdeclarator*
_tmpF4F;struct Cyc_Absyn_Carray_mod_struct*_tmpF4E;struct Cyc_Absyn_Carray_mod_struct
_tmpF4D;struct Cyc_List_List*_tmpF4C;union Cyc_YYSTYPE_union _tmpF4B;yyval=(union
Cyc_YYSTYPE_union)(((_tmpF4B.YY30).tag=34,(((_tmpF4B.YY30).f1=((_tmpF4F=
_cycalloc(sizeof(*_tmpF4F)),((_tmpF4F->tms=((_tmpF4C=_cycalloc(sizeof(*_tmpF4C)),((
_tmpF4C->hd=(void*)((void*)((_tmpF4E=_cycalloc(sizeof(*_tmpF4E)),((_tmpF4E[0]=((
_tmpF4D.tag=0,((_tmpF4D.f1=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpF4D.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpF4D)))))),
_tmpF4E))))),((_tmpF4C->tl=0,_tmpF4C)))))),_tmpF4F)))),_tmpF4B))));}break;case
253: _LL2FD:{struct Cyc_Parse_Abstractdeclarator*_tmpF5E;struct Cyc_Absyn_Carray_mod_struct*
_tmpF5D;struct Cyc_Absyn_Carray_mod_struct _tmpF5C;struct Cyc_List_List*_tmpF5B;
union Cyc_YYSTYPE_union _tmpF5A;yyval=(union Cyc_YYSTYPE_union)(((_tmpF5A.YY30).tag=
34,(((_tmpF5A.YY30).f1=((_tmpF5E=_cycalloc(sizeof(*_tmpF5E)),((_tmpF5E->tms=((
_tmpF5B=_cycalloc(sizeof(*_tmpF5B)),((_tmpF5B->hd=(void*)((void*)((_tmpF5D=
_cycalloc(sizeof(*_tmpF5D)),((_tmpF5D[0]=((_tmpF5C.tag=0,((_tmpF5C.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpF5C.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpF5C)))))),
_tmpF5D))))),((_tmpF5B->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpF5B)))))),_tmpF5E)))),_tmpF5A))));}break;case
254: _LL2FE:{struct Cyc_Parse_Abstractdeclarator*_tmpF6D;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpF6C;struct Cyc_Absyn_ConstArray_mod_struct _tmpF6B;struct Cyc_List_List*_tmpF6A;
union Cyc_YYSTYPE_union _tmpF69;yyval=(union Cyc_YYSTYPE_union)(((_tmpF69.YY30).tag=
34,(((_tmpF69.YY30).f1=((_tmpF6D=_cycalloc(sizeof(*_tmpF6D)),((_tmpF6D->tms=((
_tmpF6A=_cycalloc(sizeof(*_tmpF6A)),((_tmpF6A->hd=(void*)((void*)((_tmpF6C=
_cycalloc(sizeof(*_tmpF6C)),((_tmpF6C[0]=((_tmpF6B.tag=1,((_tmpF6B.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpF6B.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpF6B.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpF6B)))))))),
_tmpF6C))))),((_tmpF6A->tl=0,_tmpF6A)))))),_tmpF6D)))),_tmpF69))));}break;case
255: _LL2FF:{struct Cyc_Parse_Abstractdeclarator*_tmpF7C;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpF7B;struct Cyc_Absyn_ConstArray_mod_struct _tmpF7A;struct Cyc_List_List*_tmpF79;
union Cyc_YYSTYPE_union _tmpF78;yyval=(union Cyc_YYSTYPE_union)(((_tmpF78.YY30).tag=
34,(((_tmpF78.YY30).f1=((_tmpF7C=_cycalloc(sizeof(*_tmpF7C)),((_tmpF7C->tms=((
_tmpF79=_cycalloc(sizeof(*_tmpF79)),((_tmpF79->hd=(void*)((void*)((_tmpF7B=
_cycalloc(sizeof(*_tmpF7B)),((_tmpF7B[0]=((_tmpF7A.tag=1,((_tmpF7A.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpF7A.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpF7A.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpF7A)))))))),
_tmpF7B))))),((_tmpF79->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpF79)))))),_tmpF7C)))),_tmpF78))));}break;case
256: _LL300:{struct Cyc_Parse_Abstractdeclarator*_tmpF98;struct Cyc_Absyn_Function_mod_struct*
_tmpF97;struct Cyc_Absyn_WithTypes_struct*_tmpF96;struct Cyc_Absyn_WithTypes_struct
_tmpF95;struct Cyc_Absyn_Function_mod_struct _tmpF94;struct Cyc_List_List*_tmpF93;
union Cyc_YYSTYPE_union _tmpF92;yyval=(union Cyc_YYSTYPE_union)(((_tmpF92.YY30).tag=
34,(((_tmpF92.YY30).f1=((_tmpF98=_cycalloc(sizeof(*_tmpF98)),((_tmpF98->tms=((
_tmpF93=_cycalloc(sizeof(*_tmpF93)),((_tmpF93->hd=(void*)((void*)((_tmpF97=
_cycalloc(sizeof(*_tmpF97)),((_tmpF97[0]=((_tmpF94.tag=3,((_tmpF94.f1=(void*)((
void*)((_tmpF96=_cycalloc(sizeof(*_tmpF96)),((_tmpF96[0]=((_tmpF95.tag=1,((
_tmpF95.f1=0,((_tmpF95.f2=0,((_tmpF95.f3=0,((_tmpF95.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpF95.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpF95)))))))))))),
_tmpF96))))),_tmpF94)))),_tmpF97))))),((_tmpF93->tl=0,_tmpF93)))))),_tmpF98)))),
_tmpF92))));}break;case 257: _LL301: {struct _tuple17 _tmp6BD;struct Cyc_List_List*
_tmp6BE;int _tmp6BF;struct Cyc_Absyn_VarargInfo*_tmp6C0;struct Cyc_Core_Opt*_tmp6C1;
struct Cyc_List_List*_tmp6C2;struct _tuple17*_tmp6BC=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6BD=*_tmp6BC;_tmp6BE=
_tmp6BD.f1;_tmp6BF=_tmp6BD.f2;_tmp6C0=_tmp6BD.f3;_tmp6C1=_tmp6BD.f4;_tmp6C2=
_tmp6BD.f5;{struct Cyc_Parse_Abstractdeclarator*_tmpFB4;struct Cyc_Absyn_Function_mod_struct*
_tmpFB3;struct Cyc_Absyn_WithTypes_struct*_tmpFB2;struct Cyc_Absyn_WithTypes_struct
_tmpFB1;struct Cyc_Absyn_Function_mod_struct _tmpFB0;struct Cyc_List_List*_tmpFAF;
union Cyc_YYSTYPE_union _tmpFAE;yyval=(union Cyc_YYSTYPE_union)(((_tmpFAE.YY30).tag=
34,(((_tmpFAE.YY30).f1=((_tmpFB4=_cycalloc(sizeof(*_tmpFB4)),((_tmpFB4->tms=((
_tmpFAF=_cycalloc(sizeof(*_tmpFAF)),((_tmpFAF->hd=(void*)((void*)((_tmpFB3=
_cycalloc(sizeof(*_tmpFB3)),((_tmpFB3[0]=((_tmpFB0.tag=3,((_tmpFB0.f1=(void*)((
void*)((_tmpFB2=_cycalloc(sizeof(*_tmpFB2)),((_tmpFB2[0]=((_tmpFB1.tag=1,((
_tmpFB1.f1=_tmp6BE,((_tmpFB1.f2=_tmp6BF,((_tmpFB1.f3=_tmp6C0,((_tmpFB1.f4=
_tmp6C1,((_tmpFB1.f5=_tmp6C2,_tmpFB1)))))))))))),_tmpFB2))))),_tmpFB0)))),
_tmpFB3))))),((_tmpFAF->tl=0,_tmpFAF)))))),_tmpFB4)))),_tmpFAE))));}break;}case
258: _LL302:{struct Cyc_Parse_Abstractdeclarator*_tmpFD0;struct Cyc_Absyn_Function_mod_struct*
_tmpFCF;struct Cyc_Absyn_WithTypes_struct*_tmpFCE;struct Cyc_Absyn_WithTypes_struct
_tmpFCD;struct Cyc_Absyn_Function_mod_struct _tmpFCC;struct Cyc_List_List*_tmpFCB;
union Cyc_YYSTYPE_union _tmpFCA;yyval=(union Cyc_YYSTYPE_union)(((_tmpFCA.YY30).tag=
34,(((_tmpFCA.YY30).f1=((_tmpFD0=_cycalloc(sizeof(*_tmpFD0)),((_tmpFD0->tms=((
_tmpFCB=_cycalloc(sizeof(*_tmpFCB)),((_tmpFCB->hd=(void*)((void*)((_tmpFCF=
_cycalloc(sizeof(*_tmpFCF)),((_tmpFCF[0]=((_tmpFCC.tag=3,((_tmpFCC.f1=(void*)((
void*)((_tmpFCE=_cycalloc(sizeof(*_tmpFCE)),((_tmpFCE[0]=((_tmpFCD.tag=1,((
_tmpFCD.f1=0,((_tmpFCD.f2=0,((_tmpFCD.f3=0,((_tmpFCD.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpFCD.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpFCD)))))))))))),
_tmpFCE))))),_tmpFCC)))),_tmpFCF))))),((_tmpFCB->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpFCB)))))),
_tmpFD0)))),_tmpFCA))));}break;case 259: _LL303: {struct _tuple17 _tmp6D2;struct Cyc_List_List*
_tmp6D3;int _tmp6D4;struct Cyc_Absyn_VarargInfo*_tmp6D5;struct Cyc_Core_Opt*_tmp6D6;
struct Cyc_List_List*_tmp6D7;struct _tuple17*_tmp6D1=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6D2=*_tmp6D1;_tmp6D3=
_tmp6D2.f1;_tmp6D4=_tmp6D2.f2;_tmp6D5=_tmp6D2.f3;_tmp6D6=_tmp6D2.f4;_tmp6D7=
_tmp6D2.f5;{struct Cyc_Parse_Abstractdeclarator*_tmpFEC;struct Cyc_Absyn_Function_mod_struct*
_tmpFEB;struct Cyc_Absyn_WithTypes_struct*_tmpFEA;struct Cyc_Absyn_WithTypes_struct
_tmpFE9;struct Cyc_Absyn_Function_mod_struct _tmpFE8;struct Cyc_List_List*_tmpFE7;
union Cyc_YYSTYPE_union _tmpFE6;yyval=(union Cyc_YYSTYPE_union)(((_tmpFE6.YY30).tag=
34,(((_tmpFE6.YY30).f1=((_tmpFEC=_cycalloc(sizeof(*_tmpFEC)),((_tmpFEC->tms=((
_tmpFE7=_cycalloc(sizeof(*_tmpFE7)),((_tmpFE7->hd=(void*)((void*)((_tmpFEB=
_cycalloc(sizeof(*_tmpFEB)),((_tmpFEB[0]=((_tmpFE8.tag=3,((_tmpFE8.f1=(void*)((
void*)((_tmpFEA=_cycalloc(sizeof(*_tmpFEA)),((_tmpFEA[0]=((_tmpFE9.tag=1,((
_tmpFE9.f1=_tmp6D3,((_tmpFE9.f2=_tmp6D4,((_tmpFE9.f3=_tmp6D5,((_tmpFE9.f4=
_tmp6D6,((_tmpFE9.f5=_tmp6D7,_tmpFE9)))))))))))),_tmpFEA))))),_tmpFE8)))),
_tmpFEB))))),((_tmpFE7->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpFE7)))))),_tmpFEC)))),_tmpFE6))));}break;}
case 260: _LL304: {struct Cyc_List_List*_tmp6DF=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{
struct Cyc_Parse_Abstractdeclarator*_tmpFFB;struct Cyc_Absyn_TypeParams_mod_struct*
_tmpFFA;struct Cyc_Absyn_TypeParams_mod_struct _tmpFF9;struct Cyc_List_List*_tmpFF8;
union Cyc_YYSTYPE_union _tmpFF7;yyval=(union Cyc_YYSTYPE_union)(((_tmpFF7.YY30).tag=
34,(((_tmpFF7.YY30).f1=((_tmpFFB=_cycalloc(sizeof(*_tmpFFB)),((_tmpFFB->tms=((
_tmpFF8=_cycalloc(sizeof(*_tmpFF8)),((_tmpFF8->hd=(void*)((void*)((_tmpFFA=
_cycalloc(sizeof(*_tmpFFA)),((_tmpFFA[0]=((_tmpFF9.tag=4,((_tmpFF9.f1=_tmp6DF,((
_tmpFF9.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpFF9.f3=0,_tmpFF9)))))))),_tmpFFA))))),((_tmpFF8->tl=(
Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpFF8)))))),_tmpFFB)))),_tmpFF7))));}break;}case 261: _LL305:{struct Cyc_Parse_Abstractdeclarator*
_tmp100A;struct Cyc_Absyn_Attributes_mod_struct*_tmp1009;struct Cyc_Absyn_Attributes_mod_struct
_tmp1008;struct Cyc_List_List*_tmp1007;union Cyc_YYSTYPE_union _tmp1006;yyval=(
union Cyc_YYSTYPE_union)(((_tmp1006.YY30).tag=34,(((_tmp1006.YY30).f1=((_tmp100A=
_cycalloc(sizeof(*_tmp100A)),((_tmp100A->tms=((_tmp1007=_cycalloc(sizeof(*
_tmp1007)),((_tmp1007->hd=(void*)((void*)((_tmp1009=_cycalloc(sizeof(*_tmp1009)),((
_tmp1009[0]=((_tmp1008.tag=5,((_tmp1008.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmp1008.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmp1008)))))),_tmp1009))))),((_tmp1007->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms,_tmp1007)))))),
_tmp100A)))),_tmp1006))));}break;case 262: _LL306: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 263: _LL307: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 264: _LL308:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 265:
_LL309: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
266: _LL30A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 267: _LL30B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 268: _LL30C:{const char*_tmp100C;const char*_tmp100B;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),((_tmp100C="`H",_tag_dyneither(_tmp100C,sizeof(char),3))))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp100B="`U",
_tag_dyneither(_tmp100B,sizeof(char),3))))== 0){const char*_tmp1010;void*_tmp100F[
1];struct Cyc_String_pa_struct _tmp100E;Cyc_Parse_err((struct _dyneither_ptr)((
_tmp100E.tag=0,((_tmp100E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])),((_tmp100F[0]=&
_tmp100E,Cyc_aprintf(((_tmp1010="bad occurrence of heap region %s",
_tag_dyneither(_tmp1010,sizeof(char),33))),_tag_dyneither(_tmp100F,sizeof(void*),
1)))))))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));}}{struct _dyneither_ptr*_tmp1013;struct Cyc_Absyn_Tvar*
_tmp1012;struct Cyc_Absyn_Tvar*tv=(_tmp1012=_cycalloc(sizeof(*_tmp1012)),((
_tmp1012->name=((_tmp1013=_cycalloc(sizeof(*_tmp1013)),((_tmp1013[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp1013)))),((
_tmp1012->identity=- 1,((_tmp1012->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3),
_tmp1012)))))));struct Cyc_Absyn_VarType_struct _tmp1016;struct Cyc_Absyn_VarType_struct*
_tmp1015;void*t=(void*)((_tmp1015=_cycalloc(sizeof(*_tmp1015)),((_tmp1015[0]=((
_tmp1016.tag=1,((_tmp1016.f1=tv,_tmp1016)))),_tmp1015))));{struct Cyc_Absyn_Region_s_struct*
_tmp1033;struct _tuple1*_tmp1032;struct _dyneither_ptr*_tmp1031;union Cyc_Absyn_Nmspace_union
_tmp1030;struct Cyc_Absyn_RgnHandleType_struct*_tmp102F;struct Cyc_Absyn_RgnHandleType_struct
_tmp102E;struct Cyc_Absyn_Region_s_struct _tmp102D;union Cyc_YYSTYPE_union _tmp102C;
yyval=(union Cyc_YYSTYPE_union)(((_tmp102C.YY9).tag=13,(((_tmp102C.YY9).f1=Cyc_Absyn_new_stmt((
void*)((_tmp1033=_cycalloc(sizeof(*_tmp1033)),((_tmp1033[0]=((_tmp102D.tag=15,((
_tmp102D.f1=tv,((_tmp102D.f2=Cyc_Absyn_new_vardecl(((_tmp1032=_cycalloc(sizeof(*
_tmp1032)),((_tmp1032->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp1030.Loc_n).tag=0,
_tmp1030)),((_tmp1032->f2=((_tmp1031=_cycalloc(sizeof(*_tmp1031)),((_tmp1031[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmp1031)))),_tmp1032)))))),(void*)((_tmp102F=_cycalloc(sizeof(*_tmp102F)),((
_tmp102F[0]=((_tmp102E.tag=14,((_tmp102E.f1=(void*)t,_tmp102E)))),_tmp102F)))),0),((
_tmp102D.f3=0,((_tmp102D.f4=0,((_tmp102D.f5=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp102D)))))))))))),
_tmp1033)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp102C))));}break;}case 269: _LL30D:{const char*
_tmp1034;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp1034="H",
_tag_dyneither(_tmp1034,sizeof(char),2))))== 0){const char*_tmp1035;Cyc_Parse_err(((
_tmp1035="bad occurrence of heap region `H",_tag_dyneither(_tmp1035,sizeof(char),
33))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));}}{struct _dyneither_ptr*_tmp1042;const char*
_tmp1041;void*_tmp1040[1];struct Cyc_String_pa_struct _tmp103F;struct Cyc_Absyn_Tvar*
_tmp103E;struct Cyc_Absyn_Tvar*tv=(_tmp103E=_cycalloc(sizeof(*_tmp103E)),((
_tmp103E->name=((_tmp1042=_cycalloc(sizeof(*_tmp1042)),((_tmp1042[0]=(struct
_dyneither_ptr)((_tmp103F.tag=0,((_tmp103F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),((_tmp1040[0]=& _tmp103F,Cyc_aprintf(((_tmp1041="`%s",
_tag_dyneither(_tmp1041,sizeof(char),4))),_tag_dyneither(_tmp1040,sizeof(void*),
1)))))))),_tmp1042)))),((_tmp103E->identity=- 1,((_tmp103E->kind=(void*)Cyc_Tcutil_kind_to_bound((
void*)3),_tmp103E)))))));struct Cyc_Absyn_VarType_struct _tmp1045;struct Cyc_Absyn_VarType_struct*
_tmp1044;void*t=(void*)((_tmp1044=_cycalloc(sizeof(*_tmp1044)),((_tmp1044[0]=((
_tmp1045.tag=1,((_tmp1045.f1=tv,_tmp1045)))),_tmp1044))));{struct Cyc_Absyn_Region_s_struct*
_tmp1062;struct _tuple1*_tmp1061;struct _dyneither_ptr*_tmp1060;union Cyc_Absyn_Nmspace_union
_tmp105F;struct Cyc_Absyn_RgnHandleType_struct*_tmp105E;struct Cyc_Absyn_RgnHandleType_struct
_tmp105D;struct Cyc_Absyn_Region_s_struct _tmp105C;union Cyc_YYSTYPE_union _tmp105B;
yyval=(union Cyc_YYSTYPE_union)(((_tmp105B.YY9).tag=13,(((_tmp105B.YY9).f1=Cyc_Absyn_new_stmt((
void*)((_tmp1062=_cycalloc(sizeof(*_tmp1062)),((_tmp1062[0]=((_tmp105C.tag=15,((
_tmp105C.f1=tv,((_tmp105C.f2=Cyc_Absyn_new_vardecl(((_tmp1061=_cycalloc(sizeof(*
_tmp1061)),((_tmp1061->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp105F.Loc_n).tag=0,
_tmp105F)),((_tmp1061->f2=((_tmp1060=_cycalloc(sizeof(*_tmp1060)),((_tmp1060[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmp1060)))),_tmp1061)))))),(void*)((_tmp105E=_cycalloc(sizeof(*_tmp105E)),((
_tmp105E[0]=((_tmp105D.tag=14,((_tmp105D.f1=(void*)t,_tmp105D)))),_tmp105E)))),0),((
_tmp105C.f3=0,((_tmp105C.f4=Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((_tmp105C.f5=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp105C)))))))))))),
_tmp1062)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),_tmp105B))));}break;}case 270: _LL30E:{const char*
_tmp1063;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((_tmp1063="resetable",
_tag_dyneither(_tmp1063,sizeof(char),10))))!= 0){const char*_tmp1064;Cyc_Parse_err(((
_tmp1064="expecting [resetable]",_tag_dyneither(_tmp1064,sizeof(char),22))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));}}{const char*
_tmp1066;const char*_tmp1065;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp1066="`H",
_tag_dyneither(_tmp1066,sizeof(char),3))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmp1065="`U",_tag_dyneither(_tmp1065,sizeof(char),3))))){
const char*_tmp106A;void*_tmp1069[1];struct Cyc_String_pa_struct _tmp1068;Cyc_Parse_err((
struct _dyneither_ptr)((_tmp1068.tag=0,((_tmp1068.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])),((_tmp1069[0]=& _tmp1068,Cyc_aprintf(((_tmp106A="bad occurrence of heap region %s",
_tag_dyneither(_tmp106A,sizeof(char),33))),_tag_dyneither(_tmp1069,sizeof(void*),
1)))))))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));}}{struct _dyneither_ptr*_tmp106D;struct Cyc_Absyn_Tvar*
_tmp106C;struct Cyc_Absyn_Tvar*tv=(_tmp106C=_cycalloc(sizeof(*_tmp106C)),((
_tmp106C->name=((_tmp106D=_cycalloc(sizeof(*_tmp106D)),((_tmp106D[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp106D)))),((
_tmp106C->identity=- 1,((_tmp106C->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3),
_tmp106C)))))));struct Cyc_Absyn_VarType_struct _tmp1070;struct Cyc_Absyn_VarType_struct*
_tmp106F;void*t=(void*)((_tmp106F=_cycalloc(sizeof(*_tmp106F)),((_tmp106F[0]=((
_tmp1070.tag=1,((_tmp1070.f1=tv,_tmp1070)))),_tmp106F))));{struct Cyc_Absyn_Region_s_struct*
_tmp108D;struct _tuple1*_tmp108C;struct _dyneither_ptr*_tmp108B;union Cyc_Absyn_Nmspace_union
_tmp108A;struct Cyc_Absyn_RgnHandleType_struct*_tmp1089;struct Cyc_Absyn_RgnHandleType_struct
_tmp1088;struct Cyc_Absyn_Region_s_struct _tmp1087;union Cyc_YYSTYPE_union _tmp1086;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1086.YY9).tag=13,(((_tmp1086.YY9).f1=Cyc_Absyn_new_stmt((
void*)((_tmp108D=_cycalloc(sizeof(*_tmp108D)),((_tmp108D[0]=((_tmp1087.tag=15,((
_tmp1087.f1=tv,((_tmp1087.f2=Cyc_Absyn_new_vardecl(((_tmp108C=_cycalloc(sizeof(*
_tmp108C)),((_tmp108C->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp108A.Loc_n).tag=0,
_tmp108A)),((_tmp108C->f2=((_tmp108B=_cycalloc(sizeof(*_tmp108B)),((_tmp108B[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmp108B)))),_tmp108C)))))),(void*)((_tmp1089=_cycalloc(sizeof(*_tmp1089)),((
_tmp1089[0]=((_tmp1088.tag=14,((_tmp1088.f1=(void*)t,_tmp1088)))),_tmp1089)))),0),((
_tmp1087.f3=1,((_tmp1087.f4=0,((_tmp1087.f5=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1087)))))))))))),
_tmp108D)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 8)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1086))));}break;}case 271: _LL30F:{const char*
_tmp108E;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp108E="resetable",
_tag_dyneither(_tmp108E,sizeof(char),10))))!= 0){const char*_tmp108F;Cyc_Parse_err(((
_tmp108F="expecting `resetable'",_tag_dyneither(_tmp108F,sizeof(char),22))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}{const char*
_tmp1090;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp1090="H",
_tag_dyneither(_tmp1090,sizeof(char),2))))== 0){const char*_tmp1091;Cyc_Parse_err(((
_tmp1091="bad occurrence of heap region `H",_tag_dyneither(_tmp1091,sizeof(char),
33))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}}{struct _dyneither_ptr*_tmp109E;const char*
_tmp109D;void*_tmp109C[1];struct Cyc_String_pa_struct _tmp109B;struct Cyc_Absyn_Tvar*
_tmp109A;struct Cyc_Absyn_Tvar*tv=(_tmp109A=_cycalloc(sizeof(*_tmp109A)),((
_tmp109A->name=((_tmp109E=_cycalloc(sizeof(*_tmp109E)),((_tmp109E[0]=(struct
_dyneither_ptr)((_tmp109B.tag=0,((_tmp109B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])),((_tmp109C[0]=& _tmp109B,Cyc_aprintf(((_tmp109D="`%s",
_tag_dyneither(_tmp109D,sizeof(char),4))),_tag_dyneither(_tmp109C,sizeof(void*),
1)))))))),_tmp109E)))),((_tmp109A->identity=- 1,((_tmp109A->kind=(void*)Cyc_Tcutil_kind_to_bound((
void*)3),_tmp109A)))))));struct Cyc_Absyn_VarType_struct _tmp10A1;struct Cyc_Absyn_VarType_struct*
_tmp10A0;void*t=(void*)((_tmp10A0=_cycalloc(sizeof(*_tmp10A0)),((_tmp10A0[0]=((
_tmp10A1.tag=1,((_tmp10A1.f1=tv,_tmp10A1)))),_tmp10A0))));{struct Cyc_Absyn_Region_s_struct*
_tmp10BE;struct _tuple1*_tmp10BD;struct _dyneither_ptr*_tmp10BC;union Cyc_Absyn_Nmspace_union
_tmp10BB;struct Cyc_Absyn_RgnHandleType_struct*_tmp10BA;struct Cyc_Absyn_RgnHandleType_struct
_tmp10B9;struct Cyc_Absyn_Region_s_struct _tmp10B8;union Cyc_YYSTYPE_union _tmp10B7;
yyval=(union Cyc_YYSTYPE_union)(((_tmp10B7.YY9).tag=13,(((_tmp10B7.YY9).f1=Cyc_Absyn_new_stmt((
void*)((_tmp10BE=_cycalloc(sizeof(*_tmp10BE)),((_tmp10BE[0]=((_tmp10B8.tag=15,((
_tmp10B8.f1=tv,((_tmp10B8.f2=Cyc_Absyn_new_vardecl(((_tmp10BD=_cycalloc(sizeof(*
_tmp10BD)),((_tmp10BD->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp10BB.Loc_n).tag=0,
_tmp10BB)),((_tmp10BD->f2=((_tmp10BC=_cycalloc(sizeof(*_tmp10BC)),((_tmp10BC[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmp10BC)))),_tmp10BD)))))),(void*)((_tmp10BA=_cycalloc(sizeof(*_tmp10BA)),((
_tmp10BA[0]=((_tmp10B9.tag=14,((_tmp10B9.f1=(void*)t,_tmp10B9)))),_tmp10BA)))),0),((
_tmp10B8.f3=1,((_tmp10B8.f4=0,((_tmp10B8.f5=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp10B8)))))))))))),
_tmp10BE)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp10B7))));}break;}case 272: _LL310:{struct Cyc_Absyn_ResetRegion_s_struct*
_tmp10C4;struct Cyc_Absyn_ResetRegion_s_struct _tmp10C3;union Cyc_YYSTYPE_union
_tmp10C2;yyval=(union Cyc_YYSTYPE_union)(((_tmp10C2.YY9).tag=13,(((_tmp10C2.YY9).f1=
Cyc_Absyn_new_stmt((void*)((_tmp10C4=_cycalloc(sizeof(*_tmp10C4)),((_tmp10C4[0]=((
_tmp10C3.tag=16,((_tmp10C3.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),_tmp10C3)))),_tmp10C4)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10C2))));}
break;case 273: _LL311:{const char*_tmp10C5;if(Cyc_zstrcmp((struct _dyneither_ptr)
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmp10C5="in",_tag_dyneither(_tmp10C5,sizeof(char),3))))!= 0){const char*_tmp10C6;
Cyc_Parse_err(((_tmp10C6="expecting `in'",_tag_dyneither(_tmp10C6,sizeof(char),
15))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}}{struct _dyneither_ptr*_tmp10CE;struct Cyc_Absyn_Eq_kb_struct*
_tmp10CD;struct Cyc_Absyn_Eq_kb_struct _tmp10CC;struct Cyc_Absyn_Tvar*_tmp10CB;
struct Cyc_Absyn_Tvar*tv=(_tmp10CB=_cycalloc(sizeof(*_tmp10CB)),((_tmp10CB->name=((
_tmp10CE=_cycalloc(sizeof(*_tmp10CE)),((_tmp10CE[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp10CE)))),((_tmp10CB->identity=
- 1,((_tmp10CB->kind=(void*)((void*)((_tmp10CD=_cycalloc(sizeof(*_tmp10CD)),((
_tmp10CD[0]=((_tmp10CC.tag=0,((_tmp10CC.f1=(void*)((void*)3),_tmp10CC)))),
_tmp10CD))))),_tmp10CB)))))));{struct Cyc_Absyn_Alias_s_struct*_tmp10E2;struct
_tuple1*_tmp10E1;struct _dyneither_ptr*_tmp10E0;union Cyc_Absyn_Nmspace_union
_tmp10DF;struct Cyc_Absyn_Alias_s_struct _tmp10DE;union Cyc_YYSTYPE_union _tmp10DD;
yyval=(union Cyc_YYSTYPE_union)(((_tmp10DD.YY9).tag=13,(((_tmp10DD.YY9).f1=Cyc_Absyn_new_stmt((
void*)((_tmp10E2=_cycalloc(sizeof(*_tmp10E2)),((_tmp10E2[0]=((_tmp10DE.tag=17,((
_tmp10DE.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]),((_tmp10DE.f2=tv,((_tmp10DE.f3=Cyc_Absyn_new_vardecl(((_tmp10E1=_cycalloc(
sizeof(*_tmp10E1)),((_tmp10E1->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp10DF.Loc_n).tag=
0,_tmp10DF)),((_tmp10E1->f2=((_tmp10E0=_cycalloc(sizeof(*_tmp10E0)),((_tmp10E0[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tmp10E0)))),_tmp10E1)))))),(void*)0,0),((_tmp10DE.f4=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp10DE)))))))))),_tmp10E2)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 8)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp10DD))));}break;}case 274: _LL312:{union Cyc_YYSTYPE_union
_tmp10E3;yyval=(union Cyc_YYSTYPE_union)(((_tmp10E3.YY4).tag=8,(((_tmp10E3.YY4).f1=
0,_tmp10E3))));}break;case 275: _LL313:{const char*_tmp10E4;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmp10E4="open",_tag_dyneither(_tmp10E4,sizeof(char),5))))
!= 0){const char*_tmp10E5;Cyc_Parse_err(((_tmp10E5="expecting `open'",
_tag_dyneither(_tmp10E5,sizeof(char),17))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}{union Cyc_YYSTYPE_union
_tmp10E6;yyval=(union Cyc_YYSTYPE_union)(((_tmp10E6.YY4).tag=8,(((_tmp10E6.YY4).f1=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmp10E6))));}break;case 276: _LL314:{struct Cyc_Absyn_Label_s_struct*
_tmp10F0;struct _dyneither_ptr*_tmp10EF;struct Cyc_Absyn_Label_s_struct _tmp10EE;
union Cyc_YYSTYPE_union _tmp10ED;yyval=(union Cyc_YYSTYPE_union)(((_tmp10ED.YY9).tag=
13,(((_tmp10ED.YY9).f1=Cyc_Absyn_new_stmt((void*)((_tmp10F0=_cycalloc(sizeof(*
_tmp10F0)),((_tmp10F0[0]=((_tmp10EE.tag=12,((_tmp10EE.f1=((_tmp10EF=_cycalloc(
sizeof(*_tmp10EF)),((_tmp10EF[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp10EF)))),((_tmp10EE.f2=
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp10EE)))))),
_tmp10F0)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp10ED))));}break;case 277: _LL315:{union Cyc_YYSTYPE_union
_tmp10F1;yyval=(union Cyc_YYSTYPE_union)(((_tmp10F1.YY9).tag=13,(((_tmp10F1.YY9).f1=
Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10F1))));}
break;case 278: _LL316:{union Cyc_YYSTYPE_union _tmp10F2;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10F2.YY9).tag=13,(((_tmp10F2.YY9).f1=Cyc_Absyn_exp_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10F2))));}
break;case 279: _LL317:{union Cyc_YYSTYPE_union _tmp10F3;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10F3.YY9).tag=13,(((_tmp10F3.YY9).f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10F3))));}
break;case 280: _LL318: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 281: _LL319:{union Cyc_YYSTYPE_union _tmp10F4;yyval=(
union Cyc_YYSTYPE_union)(((_tmp10F4.YY9).tag=13,(((_tmp10F4.YY9).f1=Cyc_Parse_flatten_declarations(
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(
0)),_tmp10F4))));}break;case 282: _LL31A:{union Cyc_YYSTYPE_union _tmp10F5;yyval=(
union Cyc_YYSTYPE_union)(((_tmp10F5.YY9).tag=13,(((_tmp10F5.YY9).f1=Cyc_Parse_flatten_declarations(
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmp10F5))));}break;
case 283: _LL31B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 284: _LL31C:{union Cyc_YYSTYPE_union _tmp10F6;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10F6.YY9).tag=13,(((_tmp10F6.YY9).f1=Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10F6))));}
break;case 285: _LL31D:{struct Cyc_Absyn_Fn_d_struct*_tmp10FC;struct Cyc_Absyn_Fn_d_struct
_tmp10FB;union Cyc_YYSTYPE_union _tmp10FA;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10FA.YY9).tag=13,(((_tmp10FA.YY9).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)((_tmp10FC=_cycalloc(sizeof(*_tmp10FC)),((_tmp10FC[0]=((_tmp10FB.tag=1,((
_tmp10FB.f1=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp10FB)))),_tmp10FC)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(
0)),_tmp10FA))));}break;case 286: _LL31E:{struct Cyc_Absyn_Fn_d_struct*_tmp1102;
struct Cyc_Absyn_Fn_d_struct _tmp1101;union Cyc_YYSTYPE_union _tmp1100;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1100.YY9).tag=13,(((_tmp1100.YY9).f1=Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl((void*)((_tmp1102=_cycalloc(sizeof(*_tmp1102)),((_tmp1102[0]=((
_tmp1101.tag=1,((_tmp1101.f1=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmp1101)))),_tmp1102)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmp1100))));}break;
case 287: _LL31F:{union Cyc_YYSTYPE_union _tmp1103;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1103.YY9).tag=13,(((_tmp1103.YY9).f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1103))));}
break;case 288: _LL320:{union Cyc_YYSTYPE_union _tmp1104;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1104.YY9).tag=13,(((_tmp1104.YY9).f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1104))));}
break;case 289: _LL321:{union Cyc_YYSTYPE_union _tmp1105;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1105.YY9).tag=13,(((_tmp1105.YY9).f1=Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1105))));}
break;case 290: _LL322: {struct Cyc_Absyn_UnknownId_e_struct _tmp1108;struct Cyc_Absyn_UnknownId_e_struct*
_tmp1107;struct Cyc_Absyn_Exp*_tmp758=Cyc_Absyn_new_exp((void*)((_tmp1107=
_cycalloc(sizeof(*_tmp1107)),((_tmp1107[0]=((_tmp1108.tag=2,((_tmp1108.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp1108)))),
_tmp1107)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));{union Cyc_YYSTYPE_union _tmp1109;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1109.YY9).tag=13,(((_tmp1109.YY9).f1=Cyc_Absyn_switch_stmt(_tmp758,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1109))));}
break;}case 291: _LL323: {struct Cyc_Absyn_Exp*_tmp75C=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{union Cyc_YYSTYPE_union
_tmp110A;yyval=(union Cyc_YYSTYPE_union)(((_tmp110A.YY9).tag=13,(((_tmp110A.YY9).f1=
Cyc_Absyn_switch_stmt(_tmp75C,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp110A))));}
break;}case 292: _LL324:{union Cyc_YYSTYPE_union _tmp110B;yyval=(union Cyc_YYSTYPE_union)(((
_tmp110B.YY9).tag=13,(((_tmp110B.YY9).f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp110B))));}
break;case 293: _LL325:{union Cyc_YYSTYPE_union _tmp110C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp110C.YY10).tag=14,(((_tmp110C.YY10).f1=0,_tmp110C))));}break;case 294: _LL326:{
struct Cyc_List_List*_tmp1112;struct Cyc_Absyn_Switch_clause*_tmp1111;union Cyc_YYSTYPE_union
_tmp1110;yyval=(union Cyc_YYSTYPE_union)(((_tmp1110.YY10).tag=14,(((_tmp1110.YY10).f1=((
_tmp1112=_cycalloc(sizeof(*_tmp1112)),((_tmp1112->hd=((_tmp1111=_cycalloc(
sizeof(*_tmp1111)),((_tmp1111->pattern=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)),((_tmp1111->pat_vars=
0,((_tmp1111->where_clause=0,((_tmp1111->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp1111->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmp1111)))))))))))),((
_tmp1112->tl=0,_tmp1112)))))),_tmp1110))));}break;case 295: _LL327:{struct Cyc_List_List*
_tmp1118;struct Cyc_Absyn_Switch_clause*_tmp1117;union Cyc_YYSTYPE_union _tmp1116;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1116.YY10).tag=14,(((_tmp1116.YY10).f1=((
_tmp1118=_cycalloc(sizeof(*_tmp1118)),((_tmp1118->hd=((_tmp1117=_cycalloc(
sizeof(*_tmp1117)),((_tmp1117->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp1117->pat_vars=0,((
_tmp1117->where_clause=0,((_tmp1117->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),((_tmp1117->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmp1117)))))))))))),((_tmp1118->tl=Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1118)))))),
_tmp1116))));}break;case 296: _LL328:{struct Cyc_List_List*_tmp111E;struct Cyc_Absyn_Switch_clause*
_tmp111D;union Cyc_YYSTYPE_union _tmp111C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp111C.YY10).tag=14,(((_tmp111C.YY10).f1=((_tmp111E=_cycalloc(sizeof(*_tmp111E)),((
_tmp111E->hd=((_tmp111D=_cycalloc(sizeof(*_tmp111D)),((_tmp111D->pattern=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp111D->pat_vars=
0,((_tmp111D->where_clause=0,((_tmp111D->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp111D->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp111D)))))))))))),((
_tmp111E->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp111E)))))),_tmp111C))));}break;case 297: _LL329:{struct Cyc_List_List*
_tmp1124;struct Cyc_Absyn_Switch_clause*_tmp1123;union Cyc_YYSTYPE_union _tmp1122;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1122.YY10).tag=14,(((_tmp1122.YY10).f1=((
_tmp1124=_cycalloc(sizeof(*_tmp1124)),((_tmp1124->hd=((_tmp1123=_cycalloc(
sizeof(*_tmp1123)),((_tmp1123->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmp1123->pat_vars=0,((
_tmp1123->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp1123->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),((_tmp1123->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmp1123)))))))))))),((
_tmp1124->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp1124)))))),_tmp1122))));}break;case 298: _LL32A:{struct Cyc_List_List*
_tmp112A;struct Cyc_Absyn_Switch_clause*_tmp1129;union Cyc_YYSTYPE_union _tmp1128;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1128.YY10).tag=14,(((_tmp1128.YY10).f1=((
_tmp112A=_cycalloc(sizeof(*_tmp112A)),((_tmp112A->hd=((_tmp1129=_cycalloc(
sizeof(*_tmp1129)),((_tmp1129->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),((_tmp1129->pat_vars=0,((
_tmp1129->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp1129->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp1129->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmp1129)))))))))))),((
_tmp112A->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp112A)))))),_tmp1128))));}break;case 299: _LL32B:{union Cyc_YYSTYPE_union
_tmp112B;yyval=(union Cyc_YYSTYPE_union)(((_tmp112B.YY9).tag=13,(((_tmp112B.YY9).f1=
Cyc_Absyn_while_stmt(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp112B))));}break;case 300: _LL32C:{union Cyc_YYSTYPE_union
_tmp112C;yyval=(union Cyc_YYSTYPE_union)(((_tmp112C.YY9).tag=13,(((_tmp112C.YY9).f1=
Cyc_Absyn_do_stmt(Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp112C))));}
break;case 301: _LL32D:{union Cyc_YYSTYPE_union _tmp112D;yyval=(union Cyc_YYSTYPE_union)(((
_tmp112D.YY9).tag=13,(((_tmp112D.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp112D))));}
break;case 302: _LL32E:{union Cyc_YYSTYPE_union _tmp112E;yyval=(union Cyc_YYSTYPE_union)(((
_tmp112E.YY9).tag=13,(((_tmp112E.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp112E))));}break;case 303: _LL32F:{union Cyc_YYSTYPE_union
_tmp112F;yyval=(union Cyc_YYSTYPE_union)(((_tmp112F.YY9).tag=13,(((_tmp112F.YY9).f1=
Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp112F))));}
break;case 304: _LL330:{union Cyc_YYSTYPE_union _tmp1130;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1130.YY9).tag=13,(((_tmp1130.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp1130))));}
break;case 305: _LL331:{union Cyc_YYSTYPE_union _tmp1131;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1131.YY9).tag=13,(((_tmp1131.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1131))));}break;case 306: _LL332:{union Cyc_YYSTYPE_union
_tmp1132;yyval=(union Cyc_YYSTYPE_union)(((_tmp1132.YY9).tag=13,(((_tmp1132.YY9).f1=
Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1132))));}
break;case 307: _LL333:{union Cyc_YYSTYPE_union _tmp1133;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1133.YY9).tag=13,(((_tmp1133.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1133))));}
break;case 308: _LL334:{union Cyc_YYSTYPE_union _tmp1134;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1134.YY9).tag=13,(((_tmp1134.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1134))));}
break;case 309: _LL335: {struct Cyc_List_List*_tmp779=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp77A=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{union Cyc_YYSTYPE_union
_tmp1135;yyval=(union Cyc_YYSTYPE_union)(((_tmp1135.YY9).tag=13,(((_tmp1135.YY9).f1=
Cyc_Parse_flatten_declarations(_tmp779,_tmp77A),_tmp1135))));}break;}case 310:
_LL336: {struct Cyc_List_List*_tmp77C=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp77D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{union Cyc_YYSTYPE_union
_tmp1136;yyval=(union Cyc_YYSTYPE_union)(((_tmp1136.YY9).tag=13,(((_tmp1136.YY9).f1=
Cyc_Parse_flatten_declarations(_tmp77C,_tmp77D),_tmp1136))));}break;}case 311:
_LL337: {struct Cyc_List_List*_tmp77F=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp780=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{union Cyc_YYSTYPE_union
_tmp1137;yyval=(union Cyc_YYSTYPE_union)(((_tmp1137.YY9).tag=13,(((_tmp1137.YY9).f1=
Cyc_Parse_flatten_declarations(_tmp77F,_tmp780),_tmp1137))));}break;}case 312:
_LL338: {struct Cyc_List_List*_tmp782=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*
_tmp783=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{union Cyc_YYSTYPE_union
_tmp1138;yyval=(union Cyc_YYSTYPE_union)(((_tmp1138.YY9).tag=13,(((_tmp1138.YY9).f1=
Cyc_Parse_flatten_declarations(_tmp782,_tmp783),_tmp1138))));}break;}case 313:
_LL339:{struct _dyneither_ptr*_tmp113B;union Cyc_YYSTYPE_union _tmp113A;yyval=(
union Cyc_YYSTYPE_union)(((_tmp113A.YY9).tag=13,(((_tmp113A.YY9).f1=Cyc_Absyn_goto_stmt(((
_tmp113B=_cycalloc(sizeof(*_tmp113B)),((_tmp113B[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmp113B)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp113A))));}
break;case 314: _LL33A:{union Cyc_YYSTYPE_union _tmp113C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp113C.YY9).tag=13,(((_tmp113C.YY9).f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp113C))));}
break;case 315: _LL33B:{union Cyc_YYSTYPE_union _tmp113D;yyval=(union Cyc_YYSTYPE_union)(((
_tmp113D.YY9).tag=13,(((_tmp113D.YY9).f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp113D))));}
break;case 316: _LL33C:{union Cyc_YYSTYPE_union _tmp113E;yyval=(union Cyc_YYSTYPE_union)(((
_tmp113E.YY9).tag=13,(((_tmp113E.YY9).f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp113E))));}
break;case 317: _LL33D:{union Cyc_YYSTYPE_union _tmp113F;yyval=(union Cyc_YYSTYPE_union)(((
_tmp113F.YY9).tag=13,(((_tmp113F.YY9).f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp113F))));}
break;case 318: _LL33E:{union Cyc_YYSTYPE_union _tmp1140;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1140.YY9).tag=13,(((_tmp1140.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp1140))));}
break;case 319: _LL33F:{union Cyc_YYSTYPE_union _tmp1141;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1141.YY9).tag=13,(((_tmp1141.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)),_tmp1141))));}
break;case 320: _LL340:{union Cyc_YYSTYPE_union _tmp1142;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1142.YY9).tag=13,(((_tmp1142.YY9).f1=Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line)),_tmp1142))));}
break;case 321: _LL341: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 322: _LL342: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 323: _LL343:{union Cyc_YYSTYPE_union _tmp1143;yyval=(
union Cyc_YYSTYPE_union)(((_tmp1143.YY11).tag=15,(((_tmp1143.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1143))));}
break;case 324: _LL344: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 325: _LL345:{union Cyc_YYSTYPE_union _tmp1144;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1144.YY11).tag=15,(((_tmp1144.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1144))));}
break;case 326: _LL346: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 327: _LL347:{union Cyc_YYSTYPE_union _tmp1145;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1145.YY11).tag=15,(((_tmp1145.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1145))));}
break;case 328: _LL348: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 329: _LL349:{union Cyc_YYSTYPE_union _tmp1146;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1146.YY11).tag=15,(((_tmp1146.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)14,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1146))));}
break;case 330: _LL34A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 331: _LL34B:{union Cyc_YYSTYPE_union _tmp1147;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1147.YY11).tag=15,(((_tmp1147.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)15,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1147))));}
break;case 332: _LL34C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 333: _LL34D:{union Cyc_YYSTYPE_union _tmp1148;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1148.YY11).tag=15,(((_tmp1148.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)13,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1148))));}
break;case 334: _LL34E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 335: _LL34F:{union Cyc_YYSTYPE_union _tmp1149;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1149.YY11).tag=15,(((_tmp1149.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1149))));}
break;case 336: _LL350:{union Cyc_YYSTYPE_union _tmp114A;yyval=(union Cyc_YYSTYPE_union)(((
_tmp114A.YY11).tag=15,(((_tmp114A.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp114A))));}break;case 337: _LL351: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 338: _LL352:{union
Cyc_YYSTYPE_union _tmp114B;yyval=(union Cyc_YYSTYPE_union)(((_tmp114B.YY11).tag=15,(((
_tmp114B.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp114B))));}
break;case 339: _LL353:{union Cyc_YYSTYPE_union _tmp114C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp114C.YY11).tag=15,(((_tmp114C.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp114C))));}break;case 340: _LL354:{union Cyc_YYSTYPE_union
_tmp114D;yyval=(union Cyc_YYSTYPE_union)(((_tmp114D.YY11).tag=15,(((_tmp114D.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp114D))));}
break;case 341: _LL355:{union Cyc_YYSTYPE_union _tmp114E;yyval=(union Cyc_YYSTYPE_union)(((
_tmp114E.YY11).tag=15,(((_tmp114E.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp114E))));}break;case 342: _LL356: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 343: _LL357:{union
Cyc_YYSTYPE_union _tmp114F;yyval=(union Cyc_YYSTYPE_union)(((_tmp114F.YY11).tag=15,(((
_tmp114F.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)16,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp114F))));}
break;case 344: _LL358:{union Cyc_YYSTYPE_union _tmp1150;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1150.YY11).tag=15,(((_tmp1150.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)17,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1150))));}
break;case 345: _LL359: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 346: _LL35A:{union Cyc_YYSTYPE_union _tmp1151;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1151.YY11).tag=15,(((_tmp1151.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)0,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1151))));}
break;case 347: _LL35B:{union Cyc_YYSTYPE_union _tmp1152;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1152.YY11).tag=15,(((_tmp1152.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)2,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1152))));}
break;case 348: _LL35C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 349: _LL35D:{union Cyc_YYSTYPE_union _tmp1153;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1153.YY11).tag=15,(((_tmp1153.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)1,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1153))));}
break;case 350: _LL35E:{union Cyc_YYSTYPE_union _tmp1154;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1154.YY11).tag=15,(((_tmp1154.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)3,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1154))));}
break;case 351: _LL35F:{union Cyc_YYSTYPE_union _tmp1155;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1155.YY11).tag=15,(((_tmp1155.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)4,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1155))));}
break;case 352: _LL360: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 353: _LL361:{union Cyc_YYSTYPE_union _tmp1156;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1156.YY11).tag=15,(((_tmp1156.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1156))));}
break;case 354: _LL362: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 355: _LL363:{union Cyc_YYSTYPE_union _tmp1157;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1157.YY11).tag=15,(((_tmp1157.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),
_tmp1157))));}break;case 356: _LL364:{union Cyc_YYSTYPE_union _tmp1158;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1158.YY11).tag=15,(((_tmp1158.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp1158))));}break;case 357: _LL365:{union Cyc_YYSTYPE_union
_tmp1159;yyval=(union Cyc_YYSTYPE_union)(((_tmp1159.YY11).tag=15,(((_tmp1159.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1159))));}
break;case 358: _LL366:{union Cyc_YYSTYPE_union _tmp115A;yyval=(union Cyc_YYSTYPE_union)(((
_tmp115A.YY11).tag=15,(((_tmp115A.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp115A))));}
break;case 359: _LL367:{struct Cyc_Absyn_StructField_struct*_tmp1164;struct
_dyneither_ptr*_tmp1163;struct Cyc_Absyn_StructField_struct _tmp1162;union Cyc_YYSTYPE_union
_tmp1161;yyval=(union Cyc_YYSTYPE_union)(((_tmp1161.YY11).tag=15,(((_tmp1161.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)((_tmp1164=
_cycalloc(sizeof(*_tmp1164)),((_tmp1164[0]=((_tmp1162.tag=0,((_tmp1162.f1=((
_tmp1163=_cycalloc(sizeof(*_tmp1163)),((_tmp1163[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmp1163)))),_tmp1162)))),
_tmp1164)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp1161))));}break;case 360: _LL368:{struct Cyc_Absyn_TupleIndex_struct*
_tmp116A;struct Cyc_Absyn_TupleIndex_struct _tmp1169;union Cyc_YYSTYPE_union
_tmp1168;yyval=(union Cyc_YYSTYPE_union)(((_tmp1168.YY11).tag=15,(((_tmp1168.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)((_tmp116A=
_cycalloc_atomic(sizeof(*_tmp116A)),((_tmp116A[0]=((_tmp1169.tag=1,((_tmp1169.f1=(
unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f2,_tmp1169)))),_tmp116A)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1168))));}
break;case 361: _LL369: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 362: _LL36A: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 363: _LL36B:{union Cyc_YYSTYPE_union _tmp116B;yyval=(
union Cyc_YYSTYPE_union)(((_tmp116B.YY11).tag=15,(((_tmp116B.YY11).f1=Cyc_Absyn_new_pat((
void*)0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp116B))));}break;case 364: _LL36C:{union Cyc_YYSTYPE_union
_tmp116C;yyval=(union Cyc_YYSTYPE_union)(((_tmp116C.YY11).tag=15,(((_tmp116C.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])),_tmp116C))));}break;case 365: _LL36D: {struct Cyc_Absyn_Exp*e=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);{void*
_tmp7AF=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp7B0;void*_tmp7B1;char _tmp7B2;
union Cyc_Absyn_Cnst_union _tmp7B3;void*_tmp7B4;short _tmp7B5;union Cyc_Absyn_Cnst_union
_tmp7B6;void*_tmp7B7;int _tmp7B8;union Cyc_Absyn_Cnst_union _tmp7B9;struct
_dyneither_ptr _tmp7BA;union Cyc_Absyn_Cnst_union _tmp7BB;union Cyc_Absyn_Cnst_union
_tmp7BC;union Cyc_Absyn_Cnst_union _tmp7BD;_LL370: if(*((int*)_tmp7AF)!= 0)goto
_LL372;_tmp7B0=((struct Cyc_Absyn_Const_e_struct*)_tmp7AF)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7AF)->f1).Char_c).tag != 0)goto _LL372;_tmp7B1=(_tmp7B0.Char_c).f1;_tmp7B2=(
_tmp7B0.Char_c).f2;_LL371:{struct Cyc_Absyn_Char_p_struct*_tmp1172;struct Cyc_Absyn_Char_p_struct
_tmp1171;union Cyc_YYSTYPE_union _tmp1170;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1170.YY11).tag=15,(((_tmp1170.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp1172=
_cycalloc_atomic(sizeof(*_tmp1172)),((_tmp1172[0]=((_tmp1171.tag=8,((_tmp1171.f1=
_tmp7B2,_tmp1171)))),_tmp1172)))),e->loc),_tmp1170))));}goto _LL36F;_LL372: if(*((
int*)_tmp7AF)!= 0)goto _LL374;_tmp7B3=((struct Cyc_Absyn_Const_e_struct*)_tmp7AF)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7AF)->f1).Short_c).tag != 1)goto _LL374;
_tmp7B4=(_tmp7B3.Short_c).f1;_tmp7B5=(_tmp7B3.Short_c).f2;_LL373:{struct Cyc_Absyn_Int_p_struct*
_tmp1178;struct Cyc_Absyn_Int_p_struct _tmp1177;union Cyc_YYSTYPE_union _tmp1176;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1176.YY11).tag=15,(((_tmp1176.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp1178=_cycalloc(sizeof(*_tmp1178)),((_tmp1178[0]=((_tmp1177.tag=7,((
_tmp1177.f1=(void*)_tmp7B4,((_tmp1177.f2=(int)_tmp7B5,_tmp1177)))))),_tmp1178)))),
e->loc),_tmp1176))));}goto _LL36F;_LL374: if(*((int*)_tmp7AF)!= 0)goto _LL376;
_tmp7B6=((struct Cyc_Absyn_Const_e_struct*)_tmp7AF)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7AF)->f1).Int_c).tag != 2)goto _LL376;_tmp7B7=(_tmp7B6.Int_c).f1;_tmp7B8=(
_tmp7B6.Int_c).f2;_LL375:{struct Cyc_Absyn_Int_p_struct*_tmp117E;struct Cyc_Absyn_Int_p_struct
_tmp117D;union Cyc_YYSTYPE_union _tmp117C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp117C.YY11).tag=15,(((_tmp117C.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp117E=
_cycalloc(sizeof(*_tmp117E)),((_tmp117E[0]=((_tmp117D.tag=7,((_tmp117D.f1=(void*)
_tmp7B7,((_tmp117D.f2=_tmp7B8,_tmp117D)))))),_tmp117E)))),e->loc),_tmp117C))));}
goto _LL36F;_LL376: if(*((int*)_tmp7AF)!= 0)goto _LL378;_tmp7B9=((struct Cyc_Absyn_Const_e_struct*)
_tmp7AF)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7AF)->f1).Float_c).tag != 
4)goto _LL378;_tmp7BA=(_tmp7B9.Float_c).f1;_LL377:{struct Cyc_Absyn_Float_p_struct*
_tmp1184;struct Cyc_Absyn_Float_p_struct _tmp1183;union Cyc_YYSTYPE_union _tmp1182;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1182.YY11).tag=15,(((_tmp1182.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp1184=_cycalloc(sizeof(*_tmp1184)),((_tmp1184[0]=((_tmp1183.tag=9,((
_tmp1183.f1=_tmp7BA,_tmp1183)))),_tmp1184)))),e->loc),_tmp1182))));}goto _LL36F;
_LL378: if(*((int*)_tmp7AF)!= 0)goto _LL37A;_tmp7BB=((struct Cyc_Absyn_Const_e_struct*)
_tmp7AF)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7AF)->f1).Null_c).tag != 
6)goto _LL37A;_LL379:{union Cyc_YYSTYPE_union _tmp1185;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1185.YY11).tag=15,(((_tmp1185.YY11).f1=Cyc_Absyn_new_pat((void*)1,e->loc),
_tmp1185))));}goto _LL36F;_LL37A: if(*((int*)_tmp7AF)!= 0)goto _LL37C;_tmp7BC=((
struct Cyc_Absyn_Const_e_struct*)_tmp7AF)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7AF)->f1).String_c).tag != 5)goto _LL37C;_LL37B:{const char*_tmp1186;Cyc_Parse_err(((
_tmp1186="strings cannot occur within patterns",_tag_dyneither(_tmp1186,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}goto _LL36F;_LL37C: if(*((int*)_tmp7AF)!= 0)goto
_LL37E;_tmp7BD=((struct Cyc_Absyn_Const_e_struct*)_tmp7AF)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7AF)->f1).LongLong_c).tag != 3)goto _LL37E;_LL37D:{const char*_tmp1187;Cyc_Parse_unimp(((
_tmp1187="long long's in patterns",_tag_dyneither(_tmp1187,sizeof(char),24))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}goto _LL36F;_LL37E:;_LL37F: {const char*_tmp1188;Cyc_Parse_err(((
_tmp1188="bad constant in case",_tag_dyneither(_tmp1188,sizeof(char),21))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}_LL36F:;}
break;}case 366: _LL36E:{struct Cyc_Absyn_UnknownId_p_struct*_tmp118E;struct Cyc_Absyn_UnknownId_p_struct
_tmp118D;union Cyc_YYSTYPE_union _tmp118C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp118C.YY11).tag=15,(((_tmp118C.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp118E=
_cycalloc(sizeof(*_tmp118E)),((_tmp118E[0]=((_tmp118D.tag=12,((_tmp118D.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp118D)))),_tmp118E)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp118C))));}break;case 367: _LL380:{const char*
_tmp118F;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp118F="as",
_tag_dyneither(_tmp118F,sizeof(char),3))))!= 0){const char*_tmp1190;Cyc_Parse_err(((
_tmp1190="expecting `as'",_tag_dyneither(_tmp1190,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Var_p_struct*
_tmp11A4;struct _tuple1*_tmp11A3;struct _dyneither_ptr*_tmp11A2;union Cyc_Absyn_Nmspace_union
_tmp11A1;struct Cyc_Absyn_Var_p_struct _tmp11A0;union Cyc_YYSTYPE_union _tmp119F;
yyval=(union Cyc_YYSTYPE_union)(((_tmp119F.YY11).tag=15,(((_tmp119F.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp11A4=_cycalloc(sizeof(*_tmp11A4)),((_tmp11A4[0]=((_tmp11A0.tag=0,((
_tmp11A0.f1=Cyc_Absyn_new_vardecl(((_tmp11A3=_cycalloc(sizeof(*_tmp11A3)),((
_tmp11A3->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp11A1.Loc_n).tag=0,_tmp11A1)),((
_tmp11A3->f2=((_tmp11A2=_cycalloc(sizeof(*_tmp11A2)),((_tmp11A2[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp11A2)))),
_tmp11A3)))))),(void*)0,0),((_tmp11A0.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp11A0)))))),_tmp11A4)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)),_tmp119F))));}break;case 368: _LL381: {struct Cyc_List_List*
_tmp7DA;int _tmp7DB;struct _tuple13 _tmp7D9=*Cyc_yyget_YY12(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7DA=_tmp7D9.f1;
_tmp7DB=_tmp7D9.f2;{struct Cyc_Absyn_Tuple_p_struct*_tmp11AA;struct Cyc_Absyn_Tuple_p_struct
_tmp11A9;union Cyc_YYSTYPE_union _tmp11A8;yyval=(union Cyc_YYSTYPE_union)(((
_tmp11A8.YY11).tag=15,(((_tmp11A8.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp11AA=
_cycalloc(sizeof(*_tmp11AA)),((_tmp11AA[0]=((_tmp11A9.tag=3,((_tmp11A9.f1=
_tmp7DA,((_tmp11A9.f2=_tmp7DB,_tmp11A9)))))),_tmp11AA)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp11A8))));}
break;}case 369: _LL382: {struct Cyc_List_List*_tmp7E0;int _tmp7E1;struct _tuple13
_tmp7DF=*Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7E0=_tmp7DF.f1;_tmp7E1=_tmp7DF.f2;{struct Cyc_Absyn_UnknownCall_p_struct*
_tmp11B0;struct Cyc_Absyn_UnknownCall_p_struct _tmp11AF;union Cyc_YYSTYPE_union
_tmp11AE;yyval=(union Cyc_YYSTYPE_union)(((_tmp11AE.YY11).tag=15,(((_tmp11AE.YY11).f1=
Cyc_Absyn_new_pat((void*)((_tmp11B0=_cycalloc(sizeof(*_tmp11B0)),((_tmp11B0[0]=((
_tmp11AF.tag=13,((_tmp11AF.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp11AF.f2=_tmp7E0,((
_tmp11AF.f3=_tmp7E1,_tmp11AF)))))))),_tmp11B0)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp11AE))));}
break;}case 370: _LL383: {struct Cyc_List_List*_tmp7E6;int _tmp7E7;struct _tuple13
_tmp7E5=*Cyc_yyget_YY16(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7E6=_tmp7E5.f1;_tmp7E7=_tmp7E5.f2;{struct Cyc_List_List*_tmp7E8=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct*_tmp11BF;struct Cyc_Absyn_AggrInfo
_tmp11BE;union Cyc_Absyn_AggrInfoU_union _tmp11BD;struct Cyc_Absyn_Aggr_p_struct
_tmp11BC;union Cyc_YYSTYPE_union _tmp11BB;yyval=(union Cyc_YYSTYPE_union)(((
_tmp11BB.YY11).tag=15,(((_tmp11BB.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp11BF=
_cycalloc(sizeof(*_tmp11BF)),((_tmp11BF[0]=((_tmp11BC.tag=5,((_tmp11BC.f1=((
_tmp11BE.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((_tmp11BD.UnknownAggr).tag=
0,(((_tmp11BD.UnknownAggr).f1=(void*)((void*)0),(((_tmp11BD.UnknownAggr).f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp11BD)))))),((
_tmp11BE.targs=0,_tmp11BE)))),((_tmp11BC.f2=_tmp7E8,((_tmp11BC.f3=_tmp7E6,((
_tmp11BC.f4=_tmp7E7,_tmp11BC)))))))))),_tmp11BF)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp11BB))));}
break;}}case 371: _LL384:{struct Cyc_Absyn_Pointer_p_struct*_tmp11C5;struct Cyc_Absyn_Pointer_p_struct
_tmp11C4;union Cyc_YYSTYPE_union _tmp11C3;yyval=(union Cyc_YYSTYPE_union)(((
_tmp11C3.YY11).tag=15,(((_tmp11C3.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp11C5=
_cycalloc(sizeof(*_tmp11C5)),((_tmp11C5[0]=((_tmp11C4.tag=4,((_tmp11C4.f1=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp11C4)))),_tmp11C5)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp11C3))));}break;case 372: _LL385:{struct Cyc_Absyn_Pointer_p_struct*
_tmp11D4;struct Cyc_Absyn_Pointer_p_struct*_tmp11D3;struct Cyc_Absyn_Pointer_p_struct
_tmp11D2;struct Cyc_Absyn_Pointer_p_struct _tmp11D1;union Cyc_YYSTYPE_union _tmp11D0;
yyval=(union Cyc_YYSTYPE_union)(((_tmp11D0.YY11).tag=15,(((_tmp11D0.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp11D4=_cycalloc(sizeof(*_tmp11D4)),((_tmp11D4[0]=((_tmp11D1.tag=4,((
_tmp11D1.f1=Cyc_Absyn_new_pat((void*)((_tmp11D3=_cycalloc(sizeof(*_tmp11D3)),((
_tmp11D3[0]=((_tmp11D2.tag=4,((_tmp11D2.f1=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp11D2)))),_tmp11D3)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp11D1)))),_tmp11D4)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp11D0))));}
break;case 373: _LL386:{struct Cyc_Absyn_Reference_p_struct*_tmp11E8;struct _tuple1*
_tmp11E7;struct _dyneither_ptr*_tmp11E6;union Cyc_Absyn_Nmspace_union _tmp11E5;
struct Cyc_Absyn_Reference_p_struct _tmp11E4;union Cyc_YYSTYPE_union _tmp11E3;yyval=(
union Cyc_YYSTYPE_union)(((_tmp11E3.YY11).tag=15,(((_tmp11E3.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp11E8=_cycalloc(sizeof(*_tmp11E8)),((_tmp11E8[0]=((_tmp11E4.tag=1,((
_tmp11E4.f1=Cyc_Absyn_new_vardecl(((_tmp11E7=_cycalloc(sizeof(*_tmp11E7)),((
_tmp11E7->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp11E5.Loc_n).tag=0,_tmp11E5)),((
_tmp11E7->f2=((_tmp11E6=_cycalloc(sizeof(*_tmp11E6)),((_tmp11E6[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp11E6)))),_tmp11E7)))))),(
void*)0,0),((_tmp11E4.f2=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp11E4)))))),
_tmp11E8)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp11E3))));}break;case 374: _LL387:{const char*
_tmp11E9;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp11E9="as",
_tag_dyneither(_tmp11E9,sizeof(char),3))))!= 0){const char*_tmp11EA;Cyc_Parse_err(((
_tmp11EA="expecting `as'",_tag_dyneither(_tmp11EA,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Reference_p_struct*
_tmp11FE;struct _tuple1*_tmp11FD;struct _dyneither_ptr*_tmp11FC;union Cyc_Absyn_Nmspace_union
_tmp11FB;struct Cyc_Absyn_Reference_p_struct _tmp11FA;union Cyc_YYSTYPE_union
_tmp11F9;yyval=(union Cyc_YYSTYPE_union)(((_tmp11F9.YY11).tag=15,(((_tmp11F9.YY11).f1=
Cyc_Absyn_new_pat((void*)((_tmp11FE=_cycalloc(sizeof(*_tmp11FE)),((_tmp11FE[0]=((
_tmp11FA.tag=1,((_tmp11FA.f1=Cyc_Absyn_new_vardecl(((_tmp11FD=_cycalloc(sizeof(*
_tmp11FD)),((_tmp11FD->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp11FB.Loc_n).tag=0,
_tmp11FB)),((_tmp11FD->f2=((_tmp11FC=_cycalloc(sizeof(*_tmp11FC)),((_tmp11FC[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmp11FC)))),_tmp11FD)))))),(void*)0,0),((_tmp11FA.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp11FA)))))),_tmp11FE)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)),_tmp11F9))));}break;case 375: _LL388: {void*_tmp804=
Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_Tcutil_kind_to_bound((void*)7));{struct Cyc_Absyn_TagInt_p_struct*
_tmp121B;struct _tuple1*_tmp121A;struct _dyneither_ptr*_tmp1219;union Cyc_Absyn_Nmspace_union
_tmp1218;struct Cyc_Absyn_TagType_struct*_tmp1217;struct Cyc_Absyn_TagType_struct
_tmp1216;struct Cyc_Absyn_TagInt_p_struct _tmp1215;union Cyc_YYSTYPE_union _tmp1214;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1214.YY11).tag=15,(((_tmp1214.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp121B=_cycalloc(sizeof(*_tmp121B)),((_tmp121B[0]=((_tmp1215.tag=2,((
_tmp1215.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp804),((
_tmp1215.f2=Cyc_Absyn_new_vardecl(((_tmp121A=_cycalloc(sizeof(*_tmp121A)),((
_tmp121A->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp1218.Loc_n).tag=0,_tmp1218)),((
_tmp121A->f2=((_tmp1219=_cycalloc(sizeof(*_tmp1219)),((_tmp1219[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp1219)))),
_tmp121A)))))),(void*)((_tmp1217=_cycalloc(sizeof(*_tmp1217)),((_tmp1217[0]=((
_tmp1216.tag=18,((_tmp1216.f1=(void*)_tmp804,_tmp1216)))),_tmp1217)))),0),
_tmp1215)))))),_tmp121B)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1214))));}
break;}case 376: _LL389: {struct Cyc_Absyn_Tvar*_tmp80D=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound((
void*)7));{struct Cyc_Absyn_TagInt_p_struct*_tmp1245;struct _tuple1*_tmp1244;
struct _dyneither_ptr*_tmp1243;union Cyc_Absyn_Nmspace_union _tmp1242;struct Cyc_Absyn_TagType_struct*
_tmp1241;struct Cyc_Absyn_VarType_struct*_tmp1240;struct Cyc_Absyn_VarType_struct
_tmp123F;struct Cyc_Absyn_TagType_struct _tmp123E;struct Cyc_Absyn_TagInt_p_struct
_tmp123D;union Cyc_YYSTYPE_union _tmp123C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp123C.YY11).tag=15,(((_tmp123C.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp1245=
_cycalloc(sizeof(*_tmp1245)),((_tmp1245[0]=((_tmp123D.tag=2,((_tmp123D.f1=
_tmp80D,((_tmp123D.f2=Cyc_Absyn_new_vardecl(((_tmp1244=_cycalloc(sizeof(*
_tmp1244)),((_tmp1244->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp1242.Loc_n).tag=0,
_tmp1242)),((_tmp1244->f2=((_tmp1243=_cycalloc(sizeof(*_tmp1243)),((_tmp1243[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmp1243)))),_tmp1244)))))),(void*)((_tmp1241=_cycalloc(sizeof(*_tmp1241)),((
_tmp1241[0]=((_tmp123E.tag=18,((_tmp123E.f1=(void*)((void*)((_tmp1240=_cycalloc(
sizeof(*_tmp1240)),((_tmp1240[0]=((_tmp123F.tag=1,((_tmp123F.f1=_tmp80D,_tmp123F)))),
_tmp1240))))),_tmp123E)))),_tmp1241)))),0),_tmp123D)))))),_tmp1245)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp123C))));}
break;}case 377: _LL38A:{struct _tuple13*_tmp1248;union Cyc_YYSTYPE_union _tmp1247;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1247.YY12).tag=16,(((_tmp1247.YY12).f1=((
_tmp1248=_cycalloc(sizeof(*_tmp1248)),((_tmp1248->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),((_tmp1248->f2=0,_tmp1248)))))),
_tmp1247))));}break;case 378: _LL38B:{struct _tuple13*_tmp124B;union Cyc_YYSTYPE_union
_tmp124A;yyval=(union Cyc_YYSTYPE_union)(((_tmp124A.YY12).tag=16,(((_tmp124A.YY12).f1=((
_tmp124B=_cycalloc(sizeof(*_tmp124B)),((_tmp124B->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmp124B->f2=1,
_tmp124B)))))),_tmp124A))));}break;case 379: _LL38C:{struct _tuple13*_tmp124E;union
Cyc_YYSTYPE_union _tmp124D;yyval=(union Cyc_YYSTYPE_union)(((_tmp124D.YY12).tag=16,(((
_tmp124D.YY12).f1=((_tmp124E=_cycalloc(sizeof(*_tmp124E)),((_tmp124E->f1=0,((
_tmp124E->f2=1,_tmp124E)))))),_tmp124D))));}break;case 380: _LL38D:{struct Cyc_List_List*
_tmp1251;union Cyc_YYSTYPE_union _tmp1250;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1250.YY13).tag=17,(((_tmp1250.YY13).f1=((_tmp1251=_cycalloc(sizeof(*_tmp1251)),((
_tmp1251->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmp1251->tl=0,_tmp1251)))))),_tmp1250))));}break;case 381:
_LL38E:{struct Cyc_List_List*_tmp1254;union Cyc_YYSTYPE_union _tmp1253;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1253.YY13).tag=17,(((_tmp1253.YY13).f1=((_tmp1254=
_cycalloc(sizeof(*_tmp1254)),((_tmp1254->hd=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp1254->tl=Cyc_yyget_YY13(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp1254)))))),
_tmp1253))));}break;case 382: _LL38F:{struct _tuple14*_tmp1257;union Cyc_YYSTYPE_union
_tmp1256;yyval=(union Cyc_YYSTYPE_union)(((_tmp1256.YY14).tag=18,(((_tmp1256.YY14).f1=((
_tmp1257=_cycalloc(sizeof(*_tmp1257)),((_tmp1257->f1=0,((_tmp1257->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1257)))))),
_tmp1256))));}break;case 383: _LL390:{struct _tuple14*_tmp125A;union Cyc_YYSTYPE_union
_tmp1259;yyval=(union Cyc_YYSTYPE_union)(((_tmp1259.YY14).tag=18,(((_tmp1259.YY14).f1=((
_tmp125A=_cycalloc(sizeof(*_tmp125A)),((_tmp125A->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp125A->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp125A)))))),
_tmp1259))));}break;case 384: _LL391:{struct _tuple13*_tmp125D;union Cyc_YYSTYPE_union
_tmp125C;yyval=(union Cyc_YYSTYPE_union)(((_tmp125C.YY16).tag=20,(((_tmp125C.YY16).f1=((
_tmp125D=_cycalloc(sizeof(*_tmp125D)),((_tmp125D->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),((_tmp125D->f2=0,_tmp125D)))))),
_tmp125C))));}break;case 385: _LL392:{struct _tuple13*_tmp1260;union Cyc_YYSTYPE_union
_tmp125F;yyval=(union Cyc_YYSTYPE_union)(((_tmp125F.YY16).tag=20,(((_tmp125F.YY16).f1=((
_tmp1260=_cycalloc(sizeof(*_tmp1260)),((_tmp1260->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmp1260->f2=1,
_tmp1260)))))),_tmp125F))));}break;case 386: _LL393:{struct _tuple13*_tmp1263;union
Cyc_YYSTYPE_union _tmp1262;yyval=(union Cyc_YYSTYPE_union)(((_tmp1262.YY16).tag=20,(((
_tmp1262.YY16).f1=((_tmp1263=_cycalloc(sizeof(*_tmp1263)),((_tmp1263->f1=0,((
_tmp1263->f2=1,_tmp1263)))))),_tmp1262))));}break;case 387: _LL394:{struct Cyc_List_List*
_tmp1266;union Cyc_YYSTYPE_union _tmp1265;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1265.YY15).tag=19,(((_tmp1265.YY15).f1=((_tmp1266=_cycalloc(sizeof(*_tmp1266)),((
_tmp1266->hd=Cyc_yyget_YY14(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmp1266->tl=0,_tmp1266)))))),_tmp1265))));}break;case 388:
_LL395:{struct Cyc_List_List*_tmp1269;union Cyc_YYSTYPE_union _tmp1268;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1268.YY15).tag=19,(((_tmp1268.YY15).f1=((_tmp1269=
_cycalloc(sizeof(*_tmp1269)),((_tmp1269->hd=Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp1269->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp1269)))))),
_tmp1268))));}break;case 389: _LL396: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 390: _LL397:{union Cyc_YYSTYPE_union _tmp126A;yyval=(
union Cyc_YYSTYPE_union)(((_tmp126A.YY3).tag=7,(((_tmp126A.YY3).f1=Cyc_Absyn_seq_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp126A))));}
break;case 391: _LL398: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 392: _LL399:{union Cyc_YYSTYPE_union _tmp126B;yyval=(union
Cyc_YYSTYPE_union)(((_tmp126B.YY3).tag=7,(((_tmp126B.YY3).f1=Cyc_Absyn_assignop_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp126B))));}
break;case 393: _LL39A:{union Cyc_YYSTYPE_union _tmp126C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp126C.YY8).tag=12,(((_tmp126C.YY8).f1=0,_tmp126C))));}break;case 394: _LL39B:{
struct Cyc_Core_Opt*_tmp126F;union Cyc_YYSTYPE_union _tmp126E;yyval=(union Cyc_YYSTYPE_union)(((
_tmp126E.YY8).tag=12,(((_tmp126E.YY8).f1=((_tmp126F=_cycalloc(sizeof(*_tmp126F)),((
_tmp126F->v=(void*)((void*)1),_tmp126F)))),_tmp126E))));}break;case 395: _LL39C:{
struct Cyc_Core_Opt*_tmp1272;union Cyc_YYSTYPE_union _tmp1271;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1271.YY8).tag=12,(((_tmp1271.YY8).f1=((_tmp1272=_cycalloc(sizeof(*_tmp1272)),((
_tmp1272->v=(void*)((void*)3),_tmp1272)))),_tmp1271))));}break;case 396: _LL39D:{
struct Cyc_Core_Opt*_tmp1275;union Cyc_YYSTYPE_union _tmp1274;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1274.YY8).tag=12,(((_tmp1274.YY8).f1=((_tmp1275=_cycalloc(sizeof(*_tmp1275)),((
_tmp1275->v=(void*)((void*)4),_tmp1275)))),_tmp1274))));}break;case 397: _LL39E:{
struct Cyc_Core_Opt*_tmp1278;union Cyc_YYSTYPE_union _tmp1277;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1277.YY8).tag=12,(((_tmp1277.YY8).f1=((_tmp1278=_cycalloc(sizeof(*_tmp1278)),((
_tmp1278->v=(void*)((void*)0),_tmp1278)))),_tmp1277))));}break;case 398: _LL39F:{
struct Cyc_Core_Opt*_tmp127B;union Cyc_YYSTYPE_union _tmp127A;yyval=(union Cyc_YYSTYPE_union)(((
_tmp127A.YY8).tag=12,(((_tmp127A.YY8).f1=((_tmp127B=_cycalloc(sizeof(*_tmp127B)),((
_tmp127B->v=(void*)((void*)2),_tmp127B)))),_tmp127A))));}break;case 399: _LL3A0:{
struct Cyc_Core_Opt*_tmp127E;union Cyc_YYSTYPE_union _tmp127D;yyval=(union Cyc_YYSTYPE_union)(((
_tmp127D.YY8).tag=12,(((_tmp127D.YY8).f1=((_tmp127E=_cycalloc(sizeof(*_tmp127E)),((
_tmp127E->v=(void*)((void*)16),_tmp127E)))),_tmp127D))));}break;case 400: _LL3A1:{
struct Cyc_Core_Opt*_tmp1281;union Cyc_YYSTYPE_union _tmp1280;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1280.YY8).tag=12,(((_tmp1280.YY8).f1=((_tmp1281=_cycalloc(sizeof(*_tmp1281)),((
_tmp1281->v=(void*)((void*)17),_tmp1281)))),_tmp1280))));}break;case 401: _LL3A2:{
struct Cyc_Core_Opt*_tmp1284;union Cyc_YYSTYPE_union _tmp1283;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1283.YY8).tag=12,(((_tmp1283.YY8).f1=((_tmp1284=_cycalloc(sizeof(*_tmp1284)),((
_tmp1284->v=(void*)((void*)13),_tmp1284)))),_tmp1283))));}break;case 402: _LL3A3:{
struct Cyc_Core_Opt*_tmp1287;union Cyc_YYSTYPE_union _tmp1286;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1286.YY8).tag=12,(((_tmp1286.YY8).f1=((_tmp1287=_cycalloc(sizeof(*_tmp1287)),((
_tmp1287->v=(void*)((void*)15),_tmp1287)))),_tmp1286))));}break;case 403: _LL3A4:{
struct Cyc_Core_Opt*_tmp128A;union Cyc_YYSTYPE_union _tmp1289;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1289.YY8).tag=12,(((_tmp1289.YY8).f1=((_tmp128A=_cycalloc(sizeof(*_tmp128A)),((
_tmp128A->v=(void*)((void*)14),_tmp128A)))),_tmp1289))));}break;case 404: _LL3A5:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 405:
_LL3A6:{union Cyc_YYSTYPE_union _tmp128B;yyval=(union Cyc_YYSTYPE_union)(((_tmp128B.YY3).tag=
7,(((_tmp128B.YY3).f1=Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp128B))));}
break;case 406: _LL3A7:{union Cyc_YYSTYPE_union _tmp128C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp128C.YY3).tag=7,(((_tmp128C.YY3).f1=Cyc_Absyn_throw_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp128C))));}
break;case 407: _LL3A8:{union Cyc_YYSTYPE_union _tmp128D;yyval=(union Cyc_YYSTYPE_union)(((
_tmp128D.YY3).tag=7,(((_tmp128D.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp128D))));}
break;case 408: _LL3A9:{union Cyc_YYSTYPE_union _tmp128E;yyval=(union Cyc_YYSTYPE_union)(((
_tmp128E.YY3).tag=7,(((_tmp128E.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp128E))));}
break;case 409: _LL3AA:{union Cyc_YYSTYPE_union _tmp128F;yyval=(union Cyc_YYSTYPE_union)(((
_tmp128F.YY3).tag=7,(((_tmp128F.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp128F))));}
break;case 410: _LL3AB:{union Cyc_YYSTYPE_union _tmp1290;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1290.YY3).tag=7,(((_tmp1290.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1290))));}
break;case 411: _LL3AC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 412: _LL3AD: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 413: _LL3AE:{union Cyc_YYSTYPE_union _tmp1291;yyval=(
union Cyc_YYSTYPE_union)(((_tmp1291.YY3).tag=7,(((_tmp1291.YY3).f1=Cyc_Absyn_or_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1291))));}
break;case 414: _LL3AF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 415: _LL3B0:{union Cyc_YYSTYPE_union _tmp1292;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1292.YY3).tag=7,(((_tmp1292.YY3).f1=Cyc_Absyn_and_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1292))));}
break;case 416: _LL3B1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 417: _LL3B2:{union Cyc_YYSTYPE_union _tmp1293;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1293.YY3).tag=7,(((_tmp1293.YY3).f1=Cyc_Absyn_prim2_exp((
void*)14,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1293))));}
break;case 418: _LL3B3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 419: _LL3B4:{union Cyc_YYSTYPE_union _tmp1294;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1294.YY3).tag=7,(((_tmp1294.YY3).f1=Cyc_Absyn_prim2_exp((
void*)15,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1294))));}
break;case 420: _LL3B5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 421: _LL3B6:{union Cyc_YYSTYPE_union _tmp1295;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1295.YY3).tag=7,(((_tmp1295.YY3).f1=Cyc_Absyn_prim2_exp((
void*)13,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1295))));}
break;case 422: _LL3B7: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 423: _LL3B8:{union Cyc_YYSTYPE_union _tmp1296;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1296.YY3).tag=7,(((_tmp1296.YY3).f1=Cyc_Absyn_eq_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1296))));}
break;case 424: _LL3B9:{union Cyc_YYSTYPE_union _tmp1297;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1297.YY3).tag=7,(((_tmp1297.YY3).f1=Cyc_Absyn_neq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1297))));}
break;case 425: _LL3BA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 426: _LL3BB:{union Cyc_YYSTYPE_union _tmp1298;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1298.YY3).tag=7,(((_tmp1298.YY3).f1=Cyc_Absyn_lt_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1298))));}
break;case 427: _LL3BC:{union Cyc_YYSTYPE_union _tmp1299;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1299.YY3).tag=7,(((_tmp1299.YY3).f1=Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1299))));}
break;case 428: _LL3BD:{union Cyc_YYSTYPE_union _tmp129A;yyval=(union Cyc_YYSTYPE_union)(((
_tmp129A.YY3).tag=7,(((_tmp129A.YY3).f1=Cyc_Absyn_lte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp129A))));}
break;case 429: _LL3BE:{union Cyc_YYSTYPE_union _tmp129B;yyval=(union Cyc_YYSTYPE_union)(((
_tmp129B.YY3).tag=7,(((_tmp129B.YY3).f1=Cyc_Absyn_gte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp129B))));}
break;case 430: _LL3BF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 431: _LL3C0:{union Cyc_YYSTYPE_union _tmp129C;yyval=(union
Cyc_YYSTYPE_union)(((_tmp129C.YY3).tag=7,(((_tmp129C.YY3).f1=Cyc_Absyn_prim2_exp((
void*)16,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp129C))));}
break;case 432: _LL3C1:{union Cyc_YYSTYPE_union _tmp129D;yyval=(union Cyc_YYSTYPE_union)(((
_tmp129D.YY3).tag=7,(((_tmp129D.YY3).f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp129D))));}
break;case 433: _LL3C2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 434: _LL3C3:{union Cyc_YYSTYPE_union _tmp129E;yyval=(union
Cyc_YYSTYPE_union)(((_tmp129E.YY3).tag=7,(((_tmp129E.YY3).f1=Cyc_Absyn_prim2_exp((
void*)0,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp129E))));}
break;case 435: _LL3C4:{union Cyc_YYSTYPE_union _tmp129F;yyval=(union Cyc_YYSTYPE_union)(((
_tmp129F.YY3).tag=7,(((_tmp129F.YY3).f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp129F))));}
break;case 436: _LL3C5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 437: _LL3C6:{union Cyc_YYSTYPE_union _tmp12A0;yyval=(union
Cyc_YYSTYPE_union)(((_tmp12A0.YY3).tag=7,(((_tmp12A0.YY3).f1=Cyc_Absyn_prim2_exp((
void*)1,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A0))));}
break;case 438: _LL3C7:{union Cyc_YYSTYPE_union _tmp12A1;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12A1.YY3).tag=7,(((_tmp12A1.YY3).f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A1))));}
break;case 439: _LL3C8:{union Cyc_YYSTYPE_union _tmp12A2;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12A2.YY3).tag=7,(((_tmp12A2.YY3).f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A2))));}
break;case 440: _LL3C9: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 441: _LL3CA:{union Cyc_YYSTYPE_union _tmp12A3;yyval=(union
Cyc_YYSTYPE_union)(((_tmp12A3.YY3).tag=7,(((_tmp12A3.YY3).f1=Cyc_Absyn_cast_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp12A3))));}break;case 442: _LL3CB: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 443: _LL3CC:{union
Cyc_YYSTYPE_union _tmp12A4;yyval=(union Cyc_YYSTYPE_union)(((_tmp12A4.YY3).tag=7,(((
_tmp12A4.YY3).f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A4))));}
break;case 444: _LL3CD:{union Cyc_YYSTYPE_union _tmp12A5;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12A5.YY3).tag=7,(((_tmp12A5.YY3).f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A5))));}
break;case 445: _LL3CE:{union Cyc_YYSTYPE_union _tmp12A6;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12A6.YY3).tag=7,(((_tmp12A6.YY3).f1=Cyc_Absyn_address_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A6))));}
break;case 446: _LL3CF:{union Cyc_YYSTYPE_union _tmp12A7;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12A7.YY3).tag=7,(((_tmp12A7.YY3).f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A7))));}
break;case 447: _LL3D0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 448: _LL3D1:{union Cyc_YYSTYPE_union _tmp12A8;yyval=(union
Cyc_YYSTYPE_union)(((_tmp12A8.YY3).tag=7,(((_tmp12A8.YY3).f1=Cyc_Absyn_prim1_exp(
Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A8))));}
break;case 449: _LL3D2:{union Cyc_YYSTYPE_union _tmp12A9;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12A9.YY3).tag=7,(((_tmp12A9.YY3).f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A9))));}
break;case 450: _LL3D3:{union Cyc_YYSTYPE_union _tmp12AA;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12AA.YY3).tag=7,(((_tmp12AA.YY3).f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12AA))));}
break;case 451: _LL3D4:{struct Cyc_Absyn_StructField_struct*_tmp12B4;struct
_dyneither_ptr*_tmp12B3;struct Cyc_Absyn_StructField_struct _tmp12B2;union Cyc_YYSTYPE_union
_tmp12B1;yyval=(union Cyc_YYSTYPE_union)(((_tmp12B1.YY3).tag=7,(((_tmp12B1.YY3).f1=
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)((_tmp12B4=_cycalloc(sizeof(*_tmp12B4)),((
_tmp12B4[0]=((_tmp12B2.tag=0,((_tmp12B2.f1=((_tmp12B3=_cycalloc(sizeof(*_tmp12B3)),((
_tmp12B3[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmp12B3)))),_tmp12B2)))),_tmp12B4)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12B1))));}
break;case 452: _LL3D5:{struct Cyc_Absyn_TupleIndex_struct*_tmp12BA;struct Cyc_Absyn_TupleIndex_struct
_tmp12B9;union Cyc_YYSTYPE_union _tmp12B8;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12B8.YY3).tag=7,(((_tmp12B8.YY3).f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)((
_tmp12BA=_cycalloc_atomic(sizeof(*_tmp12BA)),((_tmp12BA[0]=((_tmp12B9.tag=1,((
_tmp12B9.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2,_tmp12B9)))),_tmp12BA)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12B8))));}
break;case 453: _LL3D6: {struct Cyc_Position_Segment*_tmp86E=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp86F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp86E,Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{
union Cyc_YYSTYPE_union _tmp12BB;yyval=(union Cyc_YYSTYPE_union)(((_tmp12BB.YY3).tag=
7,(((_tmp12BB.YY3).f1=Cyc_Absyn_gentyp_exp(_tmp86F,(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12BB))));}
break;}case 454: _LL3D7:{struct Cyc_Absyn_Malloc_e_struct*_tmp12C5;struct Cyc_Absyn_MallocInfo
_tmp12C4;struct Cyc_Absyn_Malloc_e_struct _tmp12C3;union Cyc_YYSTYPE_union _tmp12C2;
yyval=(union Cyc_YYSTYPE_union)(((_tmp12C2.YY3).tag=7,(((_tmp12C2.YY3).f1=Cyc_Absyn_new_exp((
void*)((_tmp12C5=_cycalloc(sizeof(*_tmp12C5)),((_tmp12C5[0]=((_tmp12C3.tag=35,((
_tmp12C3.f1=((_tmp12C4.is_calloc=0,((_tmp12C4.rgn=0,((_tmp12C4.elt_type=0,((
_tmp12C4.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmp12C4.fat_result=0,_tmp12C4)))))))))),_tmp12C3)))),
_tmp12C5)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp12C2))));}break;case 455: _LL3D8:{struct Cyc_Absyn_Malloc_e_struct*
_tmp12CF;struct Cyc_Absyn_MallocInfo _tmp12CE;struct Cyc_Absyn_Malloc_e_struct
_tmp12CD;union Cyc_YYSTYPE_union _tmp12CC;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12CC.YY3).tag=7,(((_tmp12CC.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp12CF=
_cycalloc(sizeof(*_tmp12CF)),((_tmp12CF[0]=((_tmp12CD.tag=35,((_tmp12CD.f1=((
_tmp12CE.is_calloc=0,((_tmp12CE.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp12CE.elt_type=0,((
_tmp12CE.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmp12CE.fat_result=0,_tmp12CE)))))))))),_tmp12CD)))),
_tmp12CF)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp12CC))));}break;case 456: _LL3D9: {void*_tmp87A;
struct _tuple2 _tmp879=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp87A=_tmp879.f3;{struct Cyc_Absyn_Malloc_e_struct*_tmp12DE;
struct Cyc_Absyn_MallocInfo _tmp12DD;void**_tmp12DC;struct Cyc_Absyn_Malloc_e_struct
_tmp12DB;union Cyc_YYSTYPE_union _tmp12DA;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12DA.YY3).tag=7,(((_tmp12DA.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp12DE=
_cycalloc(sizeof(*_tmp12DE)),((_tmp12DE[0]=((_tmp12DB.tag=35,((_tmp12DB.f1=((
_tmp12DD.is_calloc=1,((_tmp12DD.rgn=0,((_tmp12DD.elt_type=((_tmp12DC=_cycalloc(
sizeof(*_tmp12DC)),((_tmp12DC[0]=_tmp87A,_tmp12DC)))),((_tmp12DD.num_elts=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((_tmp12DD.fat_result=
0,_tmp12DD)))))))))),_tmp12DB)))),_tmp12DE)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12DA))));}
break;}case 457: _LL3DA: {void*_tmp881;struct _tuple2 _tmp880=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp881=_tmp880.f3;{
struct Cyc_Absyn_Malloc_e_struct*_tmp12ED;struct Cyc_Absyn_MallocInfo _tmp12EC;void**
_tmp12EB;struct Cyc_Absyn_Malloc_e_struct _tmp12EA;union Cyc_YYSTYPE_union _tmp12E9;
yyval=(union Cyc_YYSTYPE_union)(((_tmp12E9.YY3).tag=7,(((_tmp12E9.YY3).f1=Cyc_Absyn_new_exp((
void*)((_tmp12ED=_cycalloc(sizeof(*_tmp12ED)),((_tmp12ED[0]=((_tmp12EA.tag=35,((
_tmp12EA.f1=((_tmp12EC.is_calloc=1,((_tmp12EC.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 8)]),((_tmp12EC.elt_type=((
_tmp12EB=_cycalloc(sizeof(*_tmp12EB)),((_tmp12EB[0]=_tmp881,_tmp12EB)))),((
_tmp12EC.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 6)]),((_tmp12EC.fat_result=0,_tmp12EC)))))))))),_tmp12EA)))),
_tmp12ED)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 10)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp12E9))));}break;}case 458: _LL3DB:{struct Cyc_Absyn_Swap_e_struct*
_tmp12F3;struct Cyc_Absyn_Swap_e_struct _tmp12F2;union Cyc_YYSTYPE_union _tmp12F1;
yyval=(union Cyc_YYSTYPE_union)(((_tmp12F1.YY3).tag=7,(((_tmp12F1.YY3).f1=Cyc_Absyn_new_exp((
void*)((_tmp12F3=_cycalloc(sizeof(*_tmp12F3)),((_tmp12F3[0]=((_tmp12F2.tag=36,((
_tmp12F2.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]),((_tmp12F2.f2=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmp12F2)))))),_tmp12F3)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12F1))));}
break;case 459: _LL3DC:{struct Cyc_Absyn_Exp*_tmp12F6[1];union Cyc_YYSTYPE_union
_tmp12F5;yyval=(union Cyc_YYSTYPE_union)(((_tmp12F5.YY3).tag=7,(((_tmp12F5.YY3).f1=
Cyc_Absyn_primop_exp((void*)19,((_tmp12F6[0]=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp12F6,sizeof(struct Cyc_Absyn_Exp*),
1)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp12F5))));}break;case 460: _LL3DD: {void*_tmp88D;
struct _tuple2 _tmp88C=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp88D=_tmp88C.f3;{union Cyc_YYSTYPE_union _tmp12F7;yyval=(
union Cyc_YYSTYPE_union)(((_tmp12F7.YY3).tag=7,(((_tmp12F7.YY3).f1=Cyc_Absyn_valueof_exp(
_tmp88D,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp12F7))));}break;}case 461: _LL3DE:{union Cyc_YYSTYPE_union
_tmp12F8;yyval=(union Cyc_YYSTYPE_union)(((_tmp12F8.YY7).tag=11,(((_tmp12F8.YY7).f1=(
void*)((void*)12),_tmp12F8))));}break;case 462: _LL3DF:{union Cyc_YYSTYPE_union
_tmp12F9;yyval=(union Cyc_YYSTYPE_union)(((_tmp12F9.YY7).tag=11,(((_tmp12F9.YY7).f1=(
void*)((void*)11),_tmp12F9))));}break;case 463: _LL3E0:{union Cyc_YYSTYPE_union
_tmp12FA;yyval=(union Cyc_YYSTYPE_union)(((_tmp12FA.YY7).tag=11,(((_tmp12FA.YY7).f1=(
void*)((void*)2),_tmp12FA))));}break;case 464: _LL3E1: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 465: _LL3E2:{union
Cyc_YYSTYPE_union _tmp12FB;yyval=(union Cyc_YYSTYPE_union)(((_tmp12FB.YY3).tag=7,(((
_tmp12FB.YY3).f1=Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12FB))));}
break;case 466: _LL3E3:{union Cyc_YYSTYPE_union _tmp12FC;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12FC.YY3).tag=7,(((_tmp12FC.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12FC))));}
break;case 467: _LL3E4:{union Cyc_YYSTYPE_union _tmp12FD;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12FD.YY3).tag=7,(((_tmp12FD.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12FD))));}
break;case 468: _LL3E5:{struct _dyneither_ptr*_tmp1300;union Cyc_YYSTYPE_union
_tmp12FF;yyval=(union Cyc_YYSTYPE_union)(((_tmp12FF.YY3).tag=7,(((_tmp12FF.YY3).f1=
Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmp1300=_cycalloc(sizeof(*_tmp1300)),((_tmp1300[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1300)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12FF))));}
break;case 469: _LL3E6:{struct _dyneither_ptr*_tmp1303;union Cyc_YYSTYPE_union
_tmp1302;yyval=(union Cyc_YYSTYPE_union)(((_tmp1302.YY3).tag=7,(((_tmp1302.YY3).f1=
Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmp1303=_cycalloc(sizeof(*_tmp1303)),((_tmp1303[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1303)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1302))));}
break;case 470: _LL3E7:{union Cyc_YYSTYPE_union _tmp1304;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1304.YY3).tag=7,(((_tmp1304.YY3).f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1304))));}
break;case 471: _LL3E8:{union Cyc_YYSTYPE_union _tmp1305;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1305.YY3).tag=7,(((_tmp1305.YY3).f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1305))));}
break;case 472: _LL3E9:{struct Cyc_Absyn_CompoundLit_e_struct*_tmp130B;struct Cyc_Absyn_CompoundLit_e_struct
_tmp130A;union Cyc_YYSTYPE_union _tmp1309;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1309.YY3).tag=7,(((_tmp1309.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp130B=
_cycalloc(sizeof(*_tmp130B)),((_tmp130B[0]=((_tmp130A.tag=27,((_tmp130A.f1=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmp130A.f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),_tmp130A)))))),
_tmp130B)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1309))));}break;case 473: _LL3EA:{struct Cyc_Absyn_CompoundLit_e_struct*
_tmp1311;struct Cyc_Absyn_CompoundLit_e_struct _tmp1310;union Cyc_YYSTYPE_union
_tmp130F;yyval=(union Cyc_YYSTYPE_union)(((_tmp130F.YY3).tag=7,(((_tmp130F.YY3).f1=
Cyc_Absyn_new_exp((void*)((_tmp1311=_cycalloc(sizeof(*_tmp1311)),((_tmp1311[0]=((
_tmp1310.tag=27,((_tmp1310.f1=Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]),((_tmp1310.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),_tmp1310)))))),_tmp1311)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp130F))));}
break;case 474: _LL3EB:{struct Cyc_Absyn_UnknownId_e_struct*_tmp1317;struct Cyc_Absyn_UnknownId_e_struct
_tmp1316;union Cyc_YYSTYPE_union _tmp1315;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1315.YY3).tag=7,(((_tmp1315.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp1317=
_cycalloc(sizeof(*_tmp1317)),((_tmp1317[0]=((_tmp1316.tag=2,((_tmp1316.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1316)))),_tmp1317)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1315))));}break;case 475: _LL3EC: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 476: _LL3ED:{union
Cyc_YYSTYPE_union _tmp1318;yyval=(union Cyc_YYSTYPE_union)(((_tmp1318.YY3).tag=7,(((
_tmp1318.YY3).f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1318))));}
break;case 477: _LL3EE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 478: _LL3EF:{union Cyc_YYSTYPE_union _tmp1319;yyval=(
union Cyc_YYSTYPE_union)(((_tmp1319.YY3).tag=7,(((_tmp1319.YY3).f1=Cyc_Absyn_noinstantiate_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1319))));}
break;case 479: _LL3F0:{union Cyc_YYSTYPE_union _tmp131A;yyval=(union Cyc_YYSTYPE_union)(((
_tmp131A.YY3).tag=7,(((_tmp131A.YY3).f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp131A))));}
break;case 480: _LL3F1:{union Cyc_YYSTYPE_union _tmp131B;yyval=(union Cyc_YYSTYPE_union)(((
_tmp131B.YY3).tag=7,(((_tmp131B.YY3).f1=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp131B))));}
break;case 481: _LL3F2:{struct Cyc_Absyn_Struct_e_struct*_tmp1321;struct Cyc_Absyn_Struct_e_struct
_tmp1320;union Cyc_YYSTYPE_union _tmp131F;yyval=(union Cyc_YYSTYPE_union)(((
_tmp131F.YY3).tag=7,(((_tmp131F.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp1321=
_cycalloc(sizeof(*_tmp1321)),((_tmp1321[0]=((_tmp1320.tag=30,((_tmp1320.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmp1320.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp1320.f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),((_tmp1320.f4=0,
_tmp1320)))))))))),_tmp1321)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp131F))));}
break;case 482: _LL3F3:{union Cyc_YYSTYPE_union _tmp1322;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1322.YY3).tag=7,(((_tmp1322.YY3).f1=Cyc_Absyn_stmt_exp(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1322))));}
break;case 483: _LL3F4:{union Cyc_YYSTYPE_union _tmp1323;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1323.YY5).tag=9,(((_tmp1323.YY5).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])),_tmp1323))));}break;case 484: _LL3F5:{struct Cyc_List_List*
_tmp1326;union Cyc_YYSTYPE_union _tmp1325;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1325.YY5).tag=9,(((_tmp1325.YY5).f1=((_tmp1326=_cycalloc(sizeof(*_tmp1326)),((
_tmp1326->hd=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmp1326->tl=0,_tmp1326)))))),_tmp1325))));}break;case 485:
_LL3F6:{struct Cyc_List_List*_tmp1329;union Cyc_YYSTYPE_union _tmp1328;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1328.YY5).tag=9,(((_tmp1328.YY5).f1=((_tmp1329=
_cycalloc(sizeof(*_tmp1329)),((_tmp1329->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp1329->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp1329)))))),
_tmp1328))));}break;case 486: _LL3F7:{union Cyc_YYSTYPE_union _tmp132A;yyval=(union
Cyc_YYSTYPE_union)(((_tmp132A.YY3).tag=7,(((_tmp132A.YY3).f1=Cyc_Absyn_int_exp((
Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(
Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp132A))));}break;case 487: _LL3F8:{union Cyc_YYSTYPE_union
_tmp132B;yyval=(union Cyc_YYSTYPE_union)(((_tmp132B.YY3).tag=7,(((_tmp132B.YY3).f1=
Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp132B))));}break;case 488: _LL3F9:{union Cyc_YYSTYPE_union
_tmp132C;yyval=(union Cyc_YYSTYPE_union)(((_tmp132C.YY3).tag=7,(((_tmp132C.YY3).f1=
Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp132C))));}
break;case 489: _LL3FA:{union Cyc_YYSTYPE_union _tmp132D;yyval=(union Cyc_YYSTYPE_union)(((
_tmp132D.YY3).tag=7,(((_tmp132D.YY3).f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp132D))));}
break;case 490: _LL3FB:{struct _tuple1*_tmp1336;struct _dyneither_ptr*_tmp1335;union
Cyc_Absyn_Nmspace_union _tmp1334;union Cyc_YYSTYPE_union _tmp1333;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1333.QualId_tok).tag=4,(((_tmp1333.QualId_tok).f1=((_tmp1336=_cycalloc(
sizeof(*_tmp1336)),((_tmp1336->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp1334.Rel_n).tag=
1,(((_tmp1334.Rel_n).f1=0,_tmp1334)))),((_tmp1336->f2=((_tmp1335=_cycalloc(
sizeof(*_tmp1335)),((_tmp1335[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1335)))),_tmp1336)))))),
_tmp1333))));}break;case 491: _LL3FC: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 492: _LL3FD:{struct _tuple1*_tmp133F;struct
_dyneither_ptr*_tmp133E;union Cyc_Absyn_Nmspace_union _tmp133D;union Cyc_YYSTYPE_union
_tmp133C;yyval=(union Cyc_YYSTYPE_union)(((_tmp133C.QualId_tok).tag=4,(((_tmp133C.QualId_tok).f1=((
_tmp133F=_cycalloc(sizeof(*_tmp133F)),((_tmp133F->f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp133D.Rel_n).tag=1,(((_tmp133D.Rel_n).f1=0,_tmp133D)))),((_tmp133F->f2=((
_tmp133E=_cycalloc(sizeof(*_tmp133E)),((_tmp133E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp133E)))),_tmp133F)))))),
_tmp133C))));}break;case 493: _LL3FE: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 494: _LL3FF: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 495: _LL400: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 496: _LL401:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 497:
_LL402: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
498: _LL403: break;case 499: _LL404: yylex_buf->lex_curr_pos -=1;break;default: _LL405:
break;}yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=yyval;++ yylsp_offset;if(
yylen == 0){(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line=
Cyc_yylloc.first_line;(yyls[yylsp_offset]).first_column=Cyc_yylloc.first_column;(
yyls[yylsp_offset]).last_line=(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line;(yyls[yylsp_offset]).last_column=(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_column;}else{(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_line;(yyls[
yylsp_offset]).last_column=(yyls[_check_known_subscript_notnull(10000,(
yylsp_offset + yylen)- 1)]).last_column;}yyn=(int)Cyc_yyr1[
_check_known_subscript_notnull(500,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(142,yyn - 134)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
5991) && Cyc_yycheck[_check_known_subscript_notnull(5992,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(5992,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(142,yyn - 134)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[yystate];if(yyn > - 32768  && 
yyn < 5991){int sze=0;struct _dyneither_ptr msg;int x;int count;count=0;for(x=yyn < 0?-
yyn: 0;x < 276 / sizeof(char*);++ x){if(Cyc_yycheck[_check_known_subscript_notnull(
5992,x + yyn)]== x)(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(276,x)])+ 15,count ++);}{unsigned int _tmp1344;
unsigned int _tmp1343;struct _dyneither_ptr _tmp1342;char*_tmp1341;unsigned int
_tmp1340;msg=((_tmp1340=(unsigned int)(sze + 15),((_tmp1341=(char*)_region_malloc(
yyregion,_check_times(sizeof(char),_tmp1340 + 1)),((_tmp1342=_tag_dyneither(
_tmp1341,sizeof(char),_tmp1340 + 1),((((_tmp1343=_tmp1340,((_tmp1345(& _tmp1344,&
_tmp1343,& _tmp1341),_tmp1341[_tmp1343]=(char)0)))),_tmp1342))))))));}{const char*
_tmp1346;Cyc_strcpy(msg,((_tmp1346="parse error",_tag_dyneither(_tmp1346,sizeof(
char),12))));}if(count < 5){count=0;for(x=yyn < 0?- yyn: 0;x < 276 / sizeof(char*);++ x){
if(Cyc_yycheck[_check_known_subscript_notnull(5992,x + yyn)]== x){{const char*
_tmp1348;const char*_tmp1347;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(
struct _dyneither_ptr)((_tmp1348=", expecting `",_tag_dyneither(_tmp1348,sizeof(
char),14))):(struct _dyneither_ptr)((_tmp1347=" or `",_tag_dyneither(_tmp1347,
sizeof(char),6)))));}Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(276,x)]);{const char*_tmp1349;Cyc_strcat(msg,((
_tmp1349="'",_tag_dyneither(_tmp1349,sizeof(char),2))));}++ count;}}}Cyc_yyerror((
struct _dyneither_ptr)msg);}else{const char*_tmp134A;Cyc_yyerror(((_tmp134A="parse error",
_tag_dyneither(_tmp134A,sizeof(char),12))));}}goto yyerrlab1;yyerrlab1: if(
yyerrstatus == 3){if(Cyc_yychar == 0){int _tmp8C6=1;_npop_handler(0);return _tmp8C6;}
Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(
yyssp_offset == 0){int _tmp8C7=1;_npop_handler(0);return _tmp8C7;}-- yyvsp_offset;
yystate=(int)yyss[_check_known_subscript_notnull(10000,-- yyssp_offset)];--
yylsp_offset;yyerrhandle: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1011,
yystate)];if(yyn == - 32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 5991) || 
Cyc_yycheck[_check_known_subscript_notnull(5992,yyn)]!= 1)goto yyerrdefault;yyn=(
int)Cyc_yytable[_check_known_subscript_notnull(5992,yyn)];if(yyn < 0){if(yyn == -
32768)goto yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 
1010){int _tmp8C8=0;_npop_handler(0);return _tmp8C8;}yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto yynewstate;};
_pop_region(yyregion);}void Cyc_yyprint(int i,union Cyc_YYSTYPE_union v);void Cyc_yyprint(
int i,union Cyc_YYSTYPE_union v){union Cyc_YYSTYPE_union _tmp8D2=v;struct _tuple11
_tmp8D3;int _tmp8D4;char _tmp8D5;struct _dyneither_ptr _tmp8D6;struct _tuple1*_tmp8D7;
struct _tuple1 _tmp8D8;union Cyc_Absyn_Nmspace_union _tmp8D9;struct _dyneither_ptr*
_tmp8DA;_LL408: if((_tmp8D2.Int_tok).tag != 0)goto _LL40A;_tmp8D3=(_tmp8D2.Int_tok).f1;
_tmp8D4=_tmp8D3.f2;_LL409:{const char*_tmp134E;void*_tmp134D[1];struct Cyc_Int_pa_struct
_tmp134C;(_tmp134C.tag=1,((_tmp134C.f1=(unsigned long)_tmp8D4,((_tmp134D[0]=&
_tmp134C,Cyc_fprintf(Cyc_stderr,((_tmp134E="%d",_tag_dyneither(_tmp134E,sizeof(
char),3))),_tag_dyneither(_tmp134D,sizeof(void*),1)))))));}goto _LL407;_LL40A: if((
_tmp8D2.Char_tok).tag != 1)goto _LL40C;_tmp8D5=(_tmp8D2.Char_tok).f1;_LL40B:{const
char*_tmp1352;void*_tmp1351[1];struct Cyc_Int_pa_struct _tmp1350;(_tmp1350.tag=1,((
_tmp1350.f1=(unsigned long)((int)_tmp8D5),((_tmp1351[0]=& _tmp1350,Cyc_fprintf(
Cyc_stderr,((_tmp1352="%c",_tag_dyneither(_tmp1352,sizeof(char),3))),
_tag_dyneither(_tmp1351,sizeof(void*),1)))))));}goto _LL407;_LL40C: if((_tmp8D2.String_tok).tag
!= 2)goto _LL40E;_tmp8D6=(_tmp8D2.String_tok).f1;_LL40D:{const char*_tmp1356;void*
_tmp1355[1];struct Cyc_String_pa_struct _tmp1354;(_tmp1354.tag=0,((_tmp1354.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp8D6),((_tmp1355[0]=& _tmp1354,Cyc_fprintf(
Cyc_stderr,((_tmp1356="\"%s\"",_tag_dyneither(_tmp1356,sizeof(char),5))),
_tag_dyneither(_tmp1355,sizeof(void*),1)))))));}goto _LL407;_LL40E: if((_tmp8D2.QualId_tok).tag
!= 4)goto _LL410;_tmp8D7=(_tmp8D2.QualId_tok).f1;_tmp8D8=*_tmp8D7;_tmp8D9=_tmp8D8.f1;
_tmp8DA=_tmp8D8.f2;_LL40F: {struct Cyc_List_List*_tmp8E4=0;{union Cyc_Absyn_Nmspace_union
_tmp8E5=_tmp8D9;struct Cyc_List_List*_tmp8E6;struct Cyc_List_List*_tmp8E7;_LL413:
if((_tmp8E5.Rel_n).tag != 1)goto _LL415;_tmp8E6=(_tmp8E5.Rel_n).f1;_LL414: _tmp8E4=
_tmp8E6;goto _LL412;_LL415: if((_tmp8E5.Abs_n).tag != 2)goto _LL417;_tmp8E7=(_tmp8E5.Abs_n).f1;
_LL416: _tmp8E4=_tmp8E7;goto _LL412;_LL417: if((_tmp8E5.Loc_n).tag != 0)goto _LL412;
_LL418: goto _LL412;_LL412:;}for(0;_tmp8E4 != 0;_tmp8E4=_tmp8E4->tl){const char*
_tmp135A;void*_tmp1359[1];struct Cyc_String_pa_struct _tmp1358;(_tmp1358.tag=0,((
_tmp1358.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)
_tmp8E4->hd)),((_tmp1359[0]=& _tmp1358,Cyc_fprintf(Cyc_stderr,((_tmp135A="%s::",
_tag_dyneither(_tmp135A,sizeof(char),5))),_tag_dyneither(_tmp1359,sizeof(void*),
1)))))));}{const char*_tmp135E;void*_tmp135D[1];struct Cyc_String_pa_struct
_tmp135C;(_tmp135C.tag=0,((_tmp135C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp8DA),((_tmp135D[0]=& _tmp135C,Cyc_fprintf(Cyc_stderr,((
_tmp135E="%s::",_tag_dyneither(_tmp135E,sizeof(char),5))),_tag_dyneither(
_tmp135D,sizeof(void*),1)))))));}goto _LL407;}_LL410:;_LL411:{const char*_tmp1361;
void*_tmp1360;(_tmp1360=0,Cyc_fprintf(Cyc_stderr,((_tmp1361="?",_tag_dyneither(
_tmp1361,sizeof(char),2))),_tag_dyneither(_tmp1360,sizeof(void*),0)));}goto
_LL407;_LL407:;}struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){Cyc_Parse_parse_result=
0;Cyc_yyparse(Cyc_Lexing_from_file(f));return Cyc_Parse_parse_result;}
