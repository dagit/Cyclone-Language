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
struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple1{union
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
struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;
struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct
Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct
Cyc_List_List*targs;};struct _tuple3{void*f1;struct _tuple1*f2;struct Cyc_Core_Opt*
f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(void*,struct _tuple1*,
struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;
struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual
tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Position_Segment*
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_is_qvar_qualified(
struct _tuple1*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual
x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);union Cyc_Absyn_Constraint*
Cyc_Absyn_empty_conref();extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_bounds_one_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);void*Cyc_Absyn_int_typ(
void*,void*);extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern
void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(
int);void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,
union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_array_typ(void*elt_type,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct
Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct
_dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
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
exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);struct Cyc_Absyn_Decl*
Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct
Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(void*s,struct _tuple1*n,struct Cyc_List_List*
ts,struct Cyc_Core_Opt*fs,int is_extensible,struct Cyc_Position_Segment*loc);void*
Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*
ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(
void*a);extern int Cyc_Absyn_porting_c_code;struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct _tuple10{void*f1;void*f2;};struct _tuple10*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple10*Cyc_Dict_rchoose(
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
};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _tuple11{
void*f1;struct Cyc_Core_Opt*f2;};static struct _tuple11 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);struct _tuple12{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple12
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
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){{const char*_tmp792;void*
_tmp791[2];struct Cyc_String_pa_struct _tmp790;struct Cyc_String_pa_struct _tmp78F;(
_tmp78F.tag=0,((_tmp78F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((
_tmp790.tag=0,((_tmp790.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
sg)),((_tmp791[0]=& _tmp790,((_tmp791[1]=& _tmp78F,Cyc_fprintf(Cyc_stderr,((
_tmp792="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp792,
sizeof(char),46))),_tag_dyneither(_tmp791,sizeof(void*),2)))))))))))));}return;}
struct _tuple13{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;union
Cyc_Absyn_Constraint*f3;void*f4;};static struct _tuple13 Cyc_Parse_collapse_pointer_quals(
struct Cyc_Position_Segment*loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*
bound,void*rgn,struct Cyc_List_List*pqs);static struct _tuple13 Cyc_Parse_collapse_pointer_quals(
struct Cyc_Position_Segment*loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*
bound,void*rgn,struct Cyc_List_List*pqs){union Cyc_Absyn_Constraint*zeroterm=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();for(0;pqs != 0;pqs=pqs->tl){
void*_tmp4=(void*)pqs->hd;struct Cyc_Absyn_Exp*_tmp5;void*_tmp6;_LL1: if((int)
_tmp4 != 2)goto _LL3;_LL2: zeroterm=Cyc_Absyn_true_conref;goto _LL0;_LL3: if((int)
_tmp4 != 3)goto _LL5;_LL4: zeroterm=Cyc_Absyn_false_conref;goto _LL0;_LL5: if((int)
_tmp4 != 5)goto _LL7;_LL6: nullable=Cyc_Absyn_true_conref;goto _LL0;_LL7: if((int)
_tmp4 != 4)goto _LL9;_LL8: nullable=Cyc_Absyn_false_conref;goto _LL0;_LL9: if((int)
_tmp4 != 1)goto _LLB;_LLA: bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL0;_LLB:
if((int)_tmp4 != 0)goto _LLD;_LLC: bound=Cyc_Absyn_bounds_one_conref;goto _LL0;_LLD:
if(_tmp4 <= (void*)6)goto _LLF;if(*((int*)_tmp4)!= 0)goto _LLF;_tmp5=((struct Cyc_Absyn_Numelts_ptrqual_struct*)
_tmp4)->f1;_LLE:{struct Cyc_Absyn_Upper_b_struct _tmp795;struct Cyc_Absyn_Upper_b_struct*
_tmp794;bound=Cyc_Absyn_new_conref((void*)((_tmp794=_cycalloc(sizeof(*_tmp794)),((
_tmp794[0]=((_tmp795.tag=0,((_tmp795.f1=_tmp5,_tmp795)))),_tmp794)))));}goto _LL0;
_LLF: if(_tmp4 <= (void*)6)goto _LL0;if(*((int*)_tmp4)!= 1)goto _LL0;_tmp6=(void*)((
struct Cyc_Absyn_Region_ptrqual_struct*)_tmp4)->f1;_LL10: rgn=_tmp6;goto _LL0;_LL0:;}{
struct _tuple13 _tmp796;return(_tmp796.f1=nullable,((_tmp796.f2=bound,((_tmp796.f3=
zeroterm,((_tmp796.f4=rgn,_tmp796)))))));}}static int Cyc_Parse_enum_counter=0;
struct _tuple1*Cyc_Parse_gensym_enum();struct _tuple1*Cyc_Parse_gensym_enum(){
struct _dyneither_ptr*_tmp7A3;const char*_tmp7A2;void*_tmp7A1[1];struct Cyc_Int_pa_struct
_tmp7A0;struct _tuple1*_tmp79F;return(_tmp79F=_cycalloc(sizeof(*_tmp79F)),((
_tmp79F->f1=Cyc_Absyn_Rel_n(0),((_tmp79F->f2=((_tmp7A3=_cycalloc(sizeof(*_tmp7A3)),((
_tmp7A3[0]=(struct _dyneither_ptr)((_tmp7A0.tag=1,((_tmp7A0.f1=(unsigned long)Cyc_Parse_enum_counter
++,((_tmp7A1[0]=& _tmp7A0,Cyc_aprintf(((_tmp7A2="__anonymous_enum_%d__",
_tag_dyneither(_tmp7A2,sizeof(char),22))),_tag_dyneither(_tmp7A1,sizeof(void*),1)))))))),
_tmp7A3)))),_tmp79F)))));}struct _tuple14{struct _tuple1*f1;struct Cyc_Absyn_Tqual
f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple15{struct
_tuple14*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
struct Cyc_Position_Segment*loc,struct _tuple15*field_info);static struct Cyc_Absyn_Aggrfield*
Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*loc,struct _tuple15*
field_info){struct _tuple15 _tmp10;struct _tuple14*_tmp11;struct _tuple14 _tmp12;
struct _tuple1*_tmp13;struct Cyc_Absyn_Tqual _tmp14;void*_tmp15;struct Cyc_List_List*
_tmp16;struct Cyc_List_List*_tmp17;struct Cyc_Absyn_Exp*_tmp18;struct _tuple15*
_tmpF=field_info;_tmp10=*_tmpF;_tmp11=_tmp10.f1;_tmp12=*_tmp11;_tmp13=_tmp12.f1;
_tmp14=_tmp12.f2;_tmp15=_tmp12.f3;_tmp16=_tmp12.f4;_tmp17=_tmp12.f5;_tmp18=
_tmp10.f2;if(_tmp16 != 0){const char*_tmp7A4;Cyc_Parse_err(((_tmp7A4="bad type params in struct field",
_tag_dyneither(_tmp7A4,sizeof(char),32))),loc);}if(Cyc_Absyn_is_qvar_qualified(
_tmp13)){const char*_tmp7A5;Cyc_Parse_err(((_tmp7A5="struct or union field cannot be qualified with a namespace",
_tag_dyneither(_tmp7A5,sizeof(char),59))),loc);}{struct Cyc_Absyn_Aggrfield*
_tmp7A6;return(_tmp7A6=_cycalloc(sizeof(*_tmp7A6)),((_tmp7A6->name=(*_tmp13).f2,((
_tmp7A6->tq=_tmp14,((_tmp7A6->type=_tmp15,((_tmp7A6->width=_tmp18,((_tmp7A6->attributes=
_tmp17,_tmp7A6)))))))))));}}static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*
loc);static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*loc){
struct Cyc_Parse_Type_spec_struct _tmp7A9;struct Cyc_Parse_Type_spec_struct*_tmp7A8;
return(void*)((_tmp7A8=_cycalloc(sizeof(*_tmp7A8)),((_tmp7A8[0]=((_tmp7A9.tag=4,((
_tmp7A9.f1=(void*)t,((_tmp7A9.f2=loc,_tmp7A9)))))),_tmp7A8))));}static void*Cyc_Parse_array2ptr(
void*t,int argposn);static void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp1E=
t;struct Cyc_Absyn_ArrayInfo _tmp1F;void*_tmp20;struct Cyc_Absyn_Tqual _tmp21;struct
Cyc_Absyn_Exp*_tmp22;union Cyc_Absyn_Constraint*_tmp23;struct Cyc_Position_Segment*
_tmp24;_LL12: if(_tmp1E <= (void*)4)goto _LL14;if(*((int*)_tmp1E)!= 7)goto _LL14;
_tmp1F=((struct Cyc_Absyn_ArrayType_struct*)_tmp1E)->f1;_tmp20=_tmp1F.elt_type;
_tmp21=_tmp1F.tq;_tmp22=_tmp1F.num_elts;_tmp23=_tmp1F.zero_term;_tmp24=_tmp1F.zt_loc;
_LL13: {struct Cyc_Absyn_Upper_b_struct _tmp7AC;struct Cyc_Absyn_Upper_b_struct*
_tmp7AB;return Cyc_Absyn_starb_typ(_tmp20,argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0):(void*)2,_tmp21,_tmp22 == 0?(void*)0:(void*)((_tmp7AB=_cycalloc(
sizeof(*_tmp7AB)),((_tmp7AB[0]=((_tmp7AC.tag=0,((_tmp7AC.f1=(struct Cyc_Absyn_Exp*)
_tmp22,_tmp7AC)))),_tmp7AB)))),_tmp23);}_LL14:;_LL15: return t;_LL11:;}static void
Cyc_Parse_arg_array2ptr(struct _tuple8*x);static void Cyc_Parse_arg_array2ptr(
struct _tuple8*x){(*x).f3=Cyc_Parse_array2ptr((*x).f3,1);}struct _tuple16{struct
Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple16*Cyc_Parse_get_tqual_typ(struct
Cyc_Position_Segment*loc,struct _tuple8*t);static struct _tuple16*Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment*loc,struct _tuple8*t){struct _tuple16*_tmp7AD;return(
_tmp7AD=_cycalloc(sizeof(*_tmp7AD)),((_tmp7AD->f1=(*t).f2,((_tmp7AD->f2=(*t).f3,
_tmp7AD)))));}static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*
d);static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp28=d->r;struct Cyc_Absyn_Vardecl*_tmp29;_LL17: if(_tmp28 <= (void*)2)goto
_LL19;if(*((int*)_tmp28)!= 0)goto _LL19;_tmp29=((struct Cyc_Absyn_Var_d_struct*)
_tmp28)->f1;_LL18: return _tmp29;_LL19:;_LL1A: {const char*_tmp7B0;void*_tmp7AF;(
_tmp7AF=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(d->loc,((_tmp7B0="bad declaration in `forarray' statement",
_tag_dyneither(_tmp7B0,sizeof(char),40))),_tag_dyneither(_tmp7AF,sizeof(void*),0)));}
_LL16:;}static int Cyc_Parse_is_typeparam(void*tm);static int Cyc_Parse_is_typeparam(
void*tm){void*_tmp2C=tm;_LL1C: if(*((int*)_tmp2C)!= 4)goto _LL1E;_LL1D: return 1;
_LL1E:;_LL1F: return 0;_LL1B:;}static void*Cyc_Parse_id2type(struct _dyneither_ptr s,
void*k);static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){const char*
_tmp7B1;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp7B1="`H",_tag_dyneither(
_tmp7B1,sizeof(char),3))))== 0)return(void*)2;else{const char*_tmp7B2;if(Cyc_zstrcmp((
struct _dyneither_ptr)s,((_tmp7B2="`U",_tag_dyneither(_tmp7B2,sizeof(char),3))))
== 0)return(void*)3;else{struct Cyc_Absyn_VarType_struct _tmp7BC;struct
_dyneither_ptr*_tmp7BB;struct Cyc_Absyn_Tvar*_tmp7BA;struct Cyc_Absyn_VarType_struct*
_tmp7B9;return(void*)((_tmp7B9=_cycalloc(sizeof(*_tmp7B9)),((_tmp7B9[0]=((
_tmp7BC.tag=1,((_tmp7BC.f1=((_tmp7BA=_cycalloc(sizeof(*_tmp7BA)),((_tmp7BA->name=((
_tmp7BB=_cycalloc(sizeof(*_tmp7BB)),((_tmp7BB[0]=s,_tmp7BB)))),((_tmp7BA->identity=
- 1,((_tmp7BA->kind=k,_tmp7BA)))))))),_tmp7BC)))),_tmp7B9))));}}}static struct Cyc_Absyn_Tvar*
Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t);static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(
struct Cyc_Absyn_Tvar*t){void*k;{void*_tmp33=Cyc_Absyn_compress_kb(t->kind);void*
_tmp34;_LL21: if(*((int*)_tmp33)!= 1)goto _LL23;_LL22:{struct Cyc_Absyn_Unknown_kb_struct
_tmp7BF;struct Cyc_Absyn_Unknown_kb_struct*_tmp7BE;k=(void*)((_tmp7BE=_cycalloc(
sizeof(*_tmp7BE)),((_tmp7BE[0]=((_tmp7BF.tag=1,((_tmp7BF.f1=0,_tmp7BF)))),
_tmp7BE))));}goto _LL20;_LL23: if(*((int*)_tmp33)!= 2)goto _LL25;_tmp34=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp33)->f2;_LL24:{struct Cyc_Absyn_Less_kb_struct
_tmp7C2;struct Cyc_Absyn_Less_kb_struct*_tmp7C1;k=(void*)((_tmp7C1=_cycalloc(
sizeof(*_tmp7C1)),((_tmp7C1[0]=((_tmp7C2.tag=2,((_tmp7C2.f1=0,((_tmp7C2.f2=(void*)
_tmp34,_tmp7C2)))))),_tmp7C1))));}goto _LL20;_LL25:;_LL26: k=_tmp33;goto _LL20;
_LL20:;}{struct Cyc_Absyn_Tvar*_tmp7C3;return(_tmp7C3=_cycalloc(sizeof(*_tmp7C3)),((
_tmp7C3->name=t->name,((_tmp7C3->identity=- 1,((_tmp7C3->kind=k,_tmp7C3)))))));}}
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*loc,
void*t);static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t){void*_tmp3A=t;struct Cyc_Absyn_Tvar*_tmp3B;_LL28: if(_tmp3A <= (void*)4)
goto _LL2A;if(*((int*)_tmp3A)!= 1)goto _LL2A;_tmp3B=((struct Cyc_Absyn_VarType_struct*)
_tmp3A)->f1;_LL29: return _tmp3B;_LL2A:;_LL2B: {const char*_tmp7C6;void*_tmp7C5;(
_tmp7C5=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp7C6="expecting a list of type variables, not types",
_tag_dyneither(_tmp7C6,sizeof(char),46))),_tag_dyneither(_tmp7C5,sizeof(void*),0)));}
_LL27:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr);static void*Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar*pr){struct Cyc_Absyn_VarType_struct _tmp7C9;struct Cyc_Absyn_VarType_struct*
_tmp7C8;return(void*)((_tmp7C8=_cycalloc(sizeof(*_tmp7C8)),((_tmp7C8[0]=((
_tmp7C9.tag=1,((_tmp7C9.f1=pr,_tmp7C9)))),_tmp7C8))));}static void Cyc_Parse_set_vartyp_kind(
void*t,void*k,int leq);static void Cyc_Parse_set_vartyp_kind(void*t,void*k,int leq){
void*_tmp40=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp41;struct Cyc_Absyn_Tvar
_tmp42;void*_tmp43;void**_tmp44;_LL2D: if(_tmp40 <= (void*)4)goto _LL2F;if(*((int*)
_tmp40)!= 1)goto _LL2F;_tmp41=((struct Cyc_Absyn_VarType_struct*)_tmp40)->f1;
_tmp42=*_tmp41;_tmp43=_tmp42.kind;_tmp44=(void**)&(*((struct Cyc_Absyn_VarType_struct*)
_tmp40)->f1).kind;_LL2E: {void*_tmp45=Cyc_Absyn_compress_kb(*_tmp44);_LL32: if(*((
int*)_tmp45)!= 1)goto _LL34;_LL33: if(!leq)*_tmp44=Cyc_Tcutil_kind_to_bound(k);
else{struct Cyc_Absyn_Less_kb_struct _tmp7CC;struct Cyc_Absyn_Less_kb_struct*
_tmp7CB;*_tmp44=(void*)((_tmp7CB=_cycalloc(sizeof(*_tmp7CB)),((_tmp7CB[0]=((
_tmp7CC.tag=2,((_tmp7CC.f1=0,((_tmp7CC.f2=(void*)k,_tmp7CC)))))),_tmp7CB))));}
return;_LL34:;_LL35: return;_LL31:;}_LL2F:;_LL30: return;_LL2C:;}static struct Cyc_List_List*
Cyc_Parse_oldstyle2newstyle(struct Cyc_List_List*tms,struct Cyc_List_List*tds,
struct Cyc_Position_Segment*loc);static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp48=(void*)tms->hd;void*_tmp49;
_LL37: if(*((int*)_tmp48)!= 3)goto _LL39;_tmp49=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp48)->f1;_LL38: if(tms->tl == 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 
0){void*_tmp4A=_tmp49;struct Cyc_List_List*_tmp4B;_LL3C: if(*((int*)_tmp4A)!= 1)
goto _LL3E;_LL3D:{const char*_tmp7CF;void*_tmp7CE;(_tmp7CE=0,Cyc_Tcutil_warn(loc,((
_tmp7CF="function declaration with both new- and old-style parameter declarations; ignoring old-style",
_tag_dyneither(_tmp7CF,sizeof(char),93))),_tag_dyneither(_tmp7CE,sizeof(void*),0)));}
return tms;_LL3E: if(*((int*)_tmp4A)!= 0)goto _LL3B;_tmp4B=((struct Cyc_Absyn_NoTypes_struct*)
_tmp4A)->f1;_LL3F: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4B)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){const char*_tmp7D2;void*
_tmp7D1;(_tmp7D1=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp7D2="wrong number of parameter declarations in old-style function declaration",
_tag_dyneither(_tmp7D2,sizeof(char),73))),_tag_dyneither(_tmp7D1,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp50=0;for(0;_tmp4B != 0;_tmp4B=_tmp4B->tl){struct Cyc_List_List*
_tmp51=tds;for(0;_tmp51 != 0;_tmp51=_tmp51->tl){struct Cyc_Absyn_Decl*_tmp52=(
struct Cyc_Absyn_Decl*)_tmp51->hd;void*_tmp53=_tmp52->r;struct Cyc_Absyn_Vardecl*
_tmp54;_LL41: if(_tmp53 <= (void*)2)goto _LL43;if(*((int*)_tmp53)!= 0)goto _LL43;
_tmp54=((struct Cyc_Absyn_Var_d_struct*)_tmp53)->f1;_LL42: if(Cyc_zstrptrcmp((*
_tmp54->name).f2,(struct _dyneither_ptr*)_tmp4B->hd)!= 0)continue;if(_tmp54->initializer
!= 0){const char*_tmp7D5;void*_tmp7D4;(_tmp7D4=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp52->loc,((
_tmp7D5="initializer found in parameter declaration",_tag_dyneither(_tmp7D5,
sizeof(char),43))),_tag_dyneither(_tmp7D4,sizeof(void*),0)));}if(Cyc_Absyn_is_qvar_qualified(
_tmp54->name)){const char*_tmp7D8;void*_tmp7D7;(_tmp7D7=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp52->loc,((
_tmp7D8="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp7D8,
sizeof(char),47))),_tag_dyneither(_tmp7D7,sizeof(void*),0)));}{struct _tuple8*
_tmp7DE;struct Cyc_Core_Opt*_tmp7DD;struct Cyc_List_List*_tmp7DC;_tmp50=((_tmp7DC=
_cycalloc(sizeof(*_tmp7DC)),((_tmp7DC->hd=((_tmp7DE=_cycalloc(sizeof(*_tmp7DE)),((
_tmp7DE->f1=((_tmp7DD=_cycalloc(sizeof(*_tmp7DD)),((_tmp7DD->v=(*_tmp54->name).f2,
_tmp7DD)))),((_tmp7DE->f2=_tmp54->tq,((_tmp7DE->f3=_tmp54->type,_tmp7DE)))))))),((
_tmp7DC->tl=_tmp50,_tmp7DC))))));}goto L;_LL43:;_LL44: {const char*_tmp7E1;void*
_tmp7E0;(_tmp7E0=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp52->loc,((_tmp7E1="nonvariable declaration in parameter type",
_tag_dyneither(_tmp7E1,sizeof(char),42))),_tag_dyneither(_tmp7E0,sizeof(void*),0)));}
_LL40:;}L: if(_tmp51 == 0){const char*_tmp7E5;void*_tmp7E4[1];struct Cyc_String_pa_struct
_tmp7E3;(_tmp7E3.tag=0,((_tmp7E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmp4B->hd)),((_tmp7E4[0]=& _tmp7E3,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp7E5="%s is not given a type",_tag_dyneither(_tmp7E5,sizeof(char),23))),
_tag_dyneither(_tmp7E4,sizeof(void*),1)))))));}}{struct Cyc_Absyn_Function_mod_struct*
_tmp7F4;struct Cyc_Absyn_WithTypes_struct*_tmp7F3;struct Cyc_Absyn_WithTypes_struct
_tmp7F2;struct Cyc_Absyn_Function_mod_struct _tmp7F1;struct Cyc_List_List*_tmp7F0;
return(_tmp7F0=_cycalloc(sizeof(*_tmp7F0)),((_tmp7F0->hd=(void*)((void*)((
_tmp7F4=_cycalloc(sizeof(*_tmp7F4)),((_tmp7F4[0]=((_tmp7F1.tag=3,((_tmp7F1.f1=(
void*)((void*)((_tmp7F3=_cycalloc(sizeof(*_tmp7F3)),((_tmp7F3[0]=((_tmp7F2.tag=1,((
_tmp7F2.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp50),((_tmp7F2.f2=0,((_tmp7F2.f3=0,((_tmp7F2.f4=0,((_tmp7F2.f5=0,_tmp7F2)))))))))))),
_tmp7F3))))),_tmp7F1)))),_tmp7F4))))),((_tmp7F0->tl=0,_tmp7F0)))));}}_LL3B:;}
goto _LL3A;_LL39:;_LL3A: {struct Cyc_List_List*_tmp7F5;return(_tmp7F5=_cycalloc(
sizeof(*_tmp7F5)),((_tmp7F5->hd=(void*)((void*)tms->hd),((_tmp7F5->tl=Cyc_Parse_oldstyle2newstyle(
tms->tl,tds,loc),_tmp7F5)))));}_LL36:;}}struct _tuple17{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(
struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,
struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc);static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,
struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){
if(tds != 0){struct Cyc_Parse_Declarator*_tmp7F6;d=((_tmp7F6=_cycalloc(sizeof(*
_tmp7F6)),((_tmp7F6->id=d->id,((_tmp7F6->tms=Cyc_Parse_oldstyle2newstyle(d->tms,
tds,loc),_tmp7F6))))));}{void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual
tq=Cyc_Absyn_empty_tqual(0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){
tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)
dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;
atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc != 0){void*_tmp68=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v;_LL46: if((int)_tmp68 != 1)goto _LL48;
_LL47: sc=(void*)3;goto _LL45;_LL48: if((int)_tmp68 != 3)goto _LL4A;_LL49: sc=(void*)0;
goto _LL45;_LL4A:;_LL4B:{const char*_tmp7F7;Cyc_Parse_err(((_tmp7F7="bad storage class on function",
_tag_dyneither(_tmp7F7,sizeof(char),30))),loc);}goto _LL45;_LL45:;}}{void*_tmp6B;
struct Cyc_Core_Opt*_tmp6C;struct _tuple11 _tmp6A=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp6B=_tmp6A.f1;_tmp6C=_tmp6A.f2;{struct Cyc_Absyn_Tqual _tmp6E;void*
_tmp6F;struct Cyc_List_List*_tmp70;struct Cyc_List_List*_tmp71;struct _tuple12
_tmp6D=Cyc_Parse_apply_tms(tq,_tmp6B,atts,d->tms);_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;
_tmp70=_tmp6D.f3;_tmp71=_tmp6D.f4;if(_tmp6C != 0){const char*_tmp7FA;void*_tmp7F9;(
_tmp7F9=0,Cyc_Tcutil_warn(loc,((_tmp7FA="nested type declaration within function prototype",
_tag_dyneither(_tmp7FA,sizeof(char),50))),_tag_dyneither(_tmp7F9,sizeof(void*),0)));}
if(_tmp70 != 0){const char*_tmp7FD;void*_tmp7FC;(_tmp7FC=0,Cyc_Tcutil_warn(loc,((
_tmp7FD="bad type params, ignoring",_tag_dyneither(_tmp7FD,sizeof(char),26))),
_tag_dyneither(_tmp7FC,sizeof(void*),0)));}{void*_tmp76=_tmp6F;struct Cyc_Absyn_FnInfo
_tmp77;struct Cyc_List_List*_tmp78;struct Cyc_Core_Opt*_tmp79;void*_tmp7A;struct
Cyc_List_List*_tmp7B;int _tmp7C;struct Cyc_Absyn_VarargInfo*_tmp7D;struct Cyc_List_List*
_tmp7E;struct Cyc_List_List*_tmp7F;_LL4D: if(_tmp76 <= (void*)4)goto _LL4F;if(*((int*)
_tmp76)!= 8)goto _LL4F;_tmp77=((struct Cyc_Absyn_FnType_struct*)_tmp76)->f1;_tmp78=
_tmp77.tvars;_tmp79=_tmp77.effect;_tmp7A=_tmp77.ret_typ;_tmp7B=_tmp77.args;
_tmp7C=_tmp77.c_varargs;_tmp7D=_tmp77.cyc_varargs;_tmp7E=_tmp77.rgn_po;_tmp7F=
_tmp77.attributes;_LL4E: {struct Cyc_List_List*_tmp80=0;{struct Cyc_List_List*
_tmp81=_tmp7B;for(0;_tmp81 != 0;_tmp81=_tmp81->tl){struct _tuple8 _tmp83;struct Cyc_Core_Opt*
_tmp84;struct Cyc_Absyn_Tqual _tmp85;void*_tmp86;struct _tuple8*_tmp82=(struct
_tuple8*)_tmp81->hd;_tmp83=*_tmp82;_tmp84=_tmp83.f1;_tmp85=_tmp83.f2;_tmp86=
_tmp83.f3;if(_tmp84 == 0){{const char*_tmp7FE;Cyc_Parse_err(((_tmp7FE="missing argument variable in function prototype",
_tag_dyneither(_tmp7FE,sizeof(char),48))),loc);}{struct _tuple17*_tmp801;struct
Cyc_List_List*_tmp800;_tmp80=((_tmp800=_cycalloc(sizeof(*_tmp800)),((_tmp800->hd=((
_tmp801=_cycalloc(sizeof(*_tmp801)),((_tmp801->f1=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp801->f2=_tmp85,((_tmp801->f3=
_tmp86,_tmp801)))))))),((_tmp800->tl=_tmp80,_tmp800))))));}}else{struct _tuple17*
_tmp804;struct Cyc_List_List*_tmp803;_tmp80=((_tmp803=_cycalloc(sizeof(*_tmp803)),((
_tmp803->hd=((_tmp804=_cycalloc(sizeof(*_tmp804)),((_tmp804->f1=(struct
_dyneither_ptr*)_tmp84->v,((_tmp804->f2=_tmp85,((_tmp804->f3=_tmp86,_tmp804)))))))),((
_tmp803->tl=_tmp80,_tmp803))))));}}}{struct Cyc_Absyn_Fndecl*_tmp805;return(
_tmp805=_cycalloc(sizeof(*_tmp805)),((_tmp805->sc=sc,((_tmp805->name=d->id,((
_tmp805->tvs=_tmp78,((_tmp805->is_inline=is_inline,((_tmp805->effect=_tmp79,((
_tmp805->ret_type=_tmp7A,((_tmp805->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp80),((_tmp805->c_varargs=_tmp7C,((_tmp805->cyc_varargs=
_tmp7D,((_tmp805->rgn_po=_tmp7E,((_tmp805->body=body,((_tmp805->cached_typ=0,((
_tmp805->param_vardecls=0,((_tmp805->fn_vardecl=0,((_tmp805->attributes=Cyc_List_append(
_tmp7F,_tmp71),_tmp805)))))))))))))))))))))))))))))));}}_LL4F:;_LL50: {const char*
_tmp808;void*_tmp807;(_tmp807=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp808="declarator is not a function prototype",
_tag_dyneither(_tmp808,sizeof(char),39))),_tag_dyneither(_tmp807,sizeof(void*),0)));}
_LL4C:;}}}}}static char _tmp90[52]="at most one type may appear within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg1={_tmp90,_tmp90,_tmp90 + 52};static char
_tmp91[63]="const or volatile may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg2={_tmp91,_tmp91,_tmp91 + 63};static char
_tmp92[50]="type specifier includes more than one declaration";static struct
_dyneither_ptr Cyc_Parse_msg3={_tmp92,_tmp92,_tmp92 + 50};static char _tmp93[60]="sign specifier may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp93,_tmp93,_tmp93 + 60};static struct
_tuple11 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc);static struct _tuple11 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*
ts,struct Cyc_Position_Segment*loc){struct Cyc_Core_Opt*declopt=0;int seen_type=0;
int seen_sign=0;int seen_size=0;void*t=(void*)0;void*sz=(void*)2;void*sgn=(void*)0;
struct Cyc_Position_Segment*last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp94=(
void*)ts->hd;void*_tmp95;struct Cyc_Position_Segment*_tmp96;struct Cyc_Position_Segment*
_tmp97;struct Cyc_Position_Segment*_tmp98;struct Cyc_Position_Segment*_tmp99;
struct Cyc_Position_Segment*_tmp9A;struct Cyc_Absyn_Decl*_tmp9B;_LL52: if(*((int*)
_tmp94)!= 4)goto _LL54;_tmp95=(void*)((struct Cyc_Parse_Type_spec_struct*)_tmp94)->f1;
_tmp96=((struct Cyc_Parse_Type_spec_struct*)_tmp94)->f2;_LL53: if(seen_type)Cyc_Parse_err(
Cyc_Parse_msg1,_tmp96);last_loc=_tmp96;seen_type=1;t=_tmp95;goto _LL51;_LL54: if(*((
int*)_tmp94)!= 0)goto _LL56;_tmp97=((struct Cyc_Parse_Signed_spec_struct*)_tmp94)->f1;
_LL55: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp97);if(seen_type){const char*
_tmp809;Cyc_Parse_err(((_tmp809="signed qualifier must come before type",
_tag_dyneither(_tmp809,sizeof(char),39))),_tmp97);}last_loc=_tmp97;seen_sign=1;
sgn=(void*)0;goto _LL51;_LL56: if(*((int*)_tmp94)!= 1)goto _LL58;_tmp98=((struct Cyc_Parse_Unsigned_spec_struct*)
_tmp94)->f1;_LL57: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp98);if(seen_type){
const char*_tmp80A;Cyc_Parse_err(((_tmp80A="signed qualifier must come before type",
_tag_dyneither(_tmp80A,sizeof(char),39))),_tmp98);}last_loc=_tmp98;seen_sign=1;
sgn=(void*)1;goto _LL51;_LL58: if(*((int*)_tmp94)!= 2)goto _LL5A;_tmp99=((struct Cyc_Parse_Short_spec_struct*)
_tmp94)->f1;_LL59: if(seen_size){const char*_tmp80B;Cyc_Parse_err(((_tmp80B="integral size may appear only once within a type specifier",
_tag_dyneither(_tmp80B,sizeof(char),59))),_tmp99);}if(seen_type){const char*
_tmp80C;Cyc_Parse_err(((_tmp80C="short modifier must come before base type",
_tag_dyneither(_tmp80C,sizeof(char),42))),_tmp99);}last_loc=_tmp99;seen_size=1;
sz=(void*)1;goto _LL51;_LL5A: if(*((int*)_tmp94)!= 3)goto _LL5C;_tmp9A=((struct Cyc_Parse_Long_spec_struct*)
_tmp94)->f1;_LL5B: if(seen_type){const char*_tmp80D;Cyc_Parse_err(((_tmp80D="long modifier must come before base type",
_tag_dyneither(_tmp80D,sizeof(char),41))),_tmp9A);}if(seen_size){void*_tmpA1=sz;
_LL5F: if((int)_tmpA1 != 3)goto _LL61;_LL60: sz=(void*)4;goto _LL5E;_LL61:;_LL62:{
const char*_tmp80E;Cyc_Parse_err(((_tmp80E="extra long in type specifier",
_tag_dyneither(_tmp80E,sizeof(char),29))),_tmp9A);}goto _LL5E;_LL5E:;}else{sz=(
void*)3;}last_loc=_tmp9A;seen_size=1;goto _LL51;_LL5C: if(*((int*)_tmp94)!= 5)goto
_LL51;_tmp9B=((struct Cyc_Parse_Decl_spec_struct*)_tmp94)->f1;_LL5D: last_loc=
_tmp9B->loc;if(declopt == 0  && !seen_type){seen_type=1;{void*_tmpA3=_tmp9B->r;
struct Cyc_Absyn_Aggrdecl*_tmpA4;struct Cyc_Absyn_Datatypedecl*_tmpA5;struct Cyc_Absyn_Enumdecl*
_tmpA6;_LL64: if(_tmpA3 <= (void*)2)goto _LL6A;if(*((int*)_tmpA3)!= 6)goto _LL66;
_tmpA4=((struct Cyc_Absyn_Aggr_d_struct*)_tmpA3)->f1;_LL65: {struct Cyc_List_List*
_tmpA7=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmpA4->tvs));{struct Cyc_Absyn_AggrType_struct _tmp814;struct Cyc_Absyn_AggrInfo
_tmp813;struct Cyc_Absyn_AggrType_struct*_tmp812;t=(void*)((_tmp812=_cycalloc(
sizeof(*_tmp812)),((_tmp812[0]=((_tmp814.tag=10,((_tmp814.f1=((_tmp813.aggr_info=
Cyc_Absyn_UnknownAggr(_tmpA4->kind,_tmpA4->name,0),((_tmp813.targs=_tmpA7,
_tmp813)))),_tmp814)))),_tmp812))));}if(_tmpA4->impl != 0){struct Cyc_Core_Opt*
_tmp815;declopt=((_tmp815=_cycalloc(sizeof(*_tmp815)),((_tmp815->v=_tmp9B,
_tmp815))));}goto _LL63;}_LL66: if(*((int*)_tmpA3)!= 7)goto _LL68;_tmpA5=((struct
Cyc_Absyn_Datatype_d_struct*)_tmpA3)->f1;_LL67: {struct Cyc_List_List*_tmpAC=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmpA5->tvs));struct Cyc_Core_Opt*_tmp816;struct Cyc_Core_Opt*ropt=(_tmp816=
_cycalloc(sizeof(*_tmp816)),((_tmp816->v=(void*)((void*)2),_tmp816)));{struct Cyc_Absyn_DatatypeType_struct
_tmp820;struct Cyc_Absyn_Datatypedecl**_tmp81F;struct Cyc_Absyn_DatatypeInfo
_tmp81E;struct Cyc_Absyn_DatatypeType_struct*_tmp81D;t=(void*)((_tmp81D=_cycalloc(
sizeof(*_tmp81D)),((_tmp81D[0]=((_tmp820.tag=2,((_tmp820.f1=((_tmp81E.datatype_info=
Cyc_Absyn_KnownDatatype(((_tmp81F=_cycalloc(sizeof(*_tmp81F)),((_tmp81F[0]=
_tmpA5,_tmp81F))))),((_tmp81E.targs=_tmpAC,((_tmp81E.rgn=ropt,_tmp81E)))))),
_tmp820)))),_tmp81D))));}if(_tmpA5->fields != 0){struct Cyc_Core_Opt*_tmp821;
declopt=((_tmp821=_cycalloc(sizeof(*_tmp821)),((_tmp821->v=_tmp9B,_tmp821))));}
goto _LL63;}_LL68: if(*((int*)_tmpA3)!= 8)goto _LL6A;_tmpA6=((struct Cyc_Absyn_Enum_d_struct*)
_tmpA3)->f1;_LL69:{struct Cyc_Absyn_EnumType_struct _tmp824;struct Cyc_Absyn_EnumType_struct*
_tmp823;t=(void*)((_tmp823=_cycalloc(sizeof(*_tmp823)),((_tmp823[0]=((_tmp824.tag=
12,((_tmp824.f1=_tmpA6->name,((_tmp824.f2=0,_tmp824)))))),_tmp823))));}{struct
Cyc_Core_Opt*_tmp825;declopt=((_tmp825=_cycalloc(sizeof(*_tmp825)),((_tmp825->v=
_tmp9B,_tmp825))));}goto _LL63;_LL6A:;_LL6B: {const char*_tmp828;void*_tmp827;(
_tmp827=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(_tmp9B->loc,((_tmp828="bad declaration within type specifier",
_tag_dyneither(_tmp828,sizeof(char),38))),_tag_dyneither(_tmp827,sizeof(void*),0)));}
_LL63:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,_tmp9B->loc);}goto _LL51;_LL51:;}if(!
seen_type){if(!seen_sign  && !seen_size){const char*_tmp82B;void*_tmp82A;(_tmp82A=
0,Cyc_Tcutil_warn(last_loc,((_tmp82B="missing type within specifier",
_tag_dyneither(_tmp82B,sizeof(char),30))),_tag_dyneither(_tmp82A,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{if(seen_sign){void*_tmpBA=t;void*_tmpBB;void*
_tmpBC;_LL6D: if(_tmpBA <= (void*)4)goto _LL6F;if(*((int*)_tmpBA)!= 5)goto _LL6F;
_tmpBB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBA)->f1;_tmpBC=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmpBA)->f2;_LL6E: if(_tmpBB != sgn)t=Cyc_Absyn_int_typ(
sgn,_tmpBC);goto _LL6C;_LL6F:;_LL70:{const char*_tmp82C;Cyc_Parse_err(((_tmp82C="sign specification on non-integral type",
_tag_dyneither(_tmp82C,sizeof(char),40))),last_loc);}goto _LL6C;_LL6C:;}if(
seen_size){void*_tmpBE=t;void*_tmpBF;void*_tmpC0;_LL72: if(_tmpBE <= (void*)4)goto
_LL76;if(*((int*)_tmpBE)!= 5)goto _LL74;_tmpBF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpBE)->f1;_tmpC0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBE)->f2;_LL73:
if(_tmpC0 != sz)t=Cyc_Absyn_int_typ(_tmpBF,sz);goto _LL71;_LL74: if(*((int*)_tmpBE)
!= 6)goto _LL76;_LL75: t=Cyc_Absyn_double_typ(1);goto _LL71;_LL76:;_LL77:{const char*
_tmp82D;Cyc_Parse_err(((_tmp82D="size qualifier on non-integral type",
_tag_dyneither(_tmp82D,sizeof(char),36))),last_loc);}goto _LL71;_LL71:;}}{struct
_tuple11 _tmp82E;return(_tmp82E.f1=t,((_tmp82E.f2=declopt,_tmp82E)));}}static
struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts);static struct
Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,
void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{
struct Cyc_Parse_Declarator*_tmpC3=(struct Cyc_Parse_Declarator*)ds->hd;struct
_tuple1*_tmpC4=_tmpC3->id;struct Cyc_Absyn_Tqual _tmpC6;void*_tmpC7;struct Cyc_List_List*
_tmpC8;struct Cyc_List_List*_tmpC9;struct _tuple12 _tmpC5=Cyc_Parse_apply_tms(tq,t,
shared_atts,_tmpC3->tms);_tmpC6=_tmpC5.f1;_tmpC7=_tmpC5.f2;_tmpC8=_tmpC5.f3;
_tmpC9=_tmpC5.f4;if(ds->tl == 0){struct _tuple14*_tmp831;struct Cyc_List_List*
_tmp830;return(_tmp830=_region_malloc(r,sizeof(*_tmp830)),((_tmp830->hd=((
_tmp831=_region_malloc(r,sizeof(*_tmp831)),((_tmp831->f1=_tmpC4,((_tmp831->f2=
_tmpC6,((_tmp831->f3=_tmpC7,((_tmp831->f4=_tmpC8,((_tmp831->f5=_tmpC9,_tmp831)))))))))))),((
_tmp830->tl=0,_tmp830)))));}else{struct _tuple14*_tmp834;struct Cyc_List_List*
_tmp833;return(_tmp833=_region_malloc(r,sizeof(*_tmp833)),((_tmp833->hd=((
_tmp834=_region_malloc(r,sizeof(*_tmp834)),((_tmp834->f1=_tmpC4,((_tmp834->f2=
_tmpC6,((_tmp834->f3=_tmpC7,((_tmp834->f4=_tmpC8,((_tmp834->f5=_tmpC9,_tmp834)))))))))))),((
_tmp833->tl=Cyc_Parse_apply_tmss(r,_tmpC6,Cyc_Tcutil_copy_type(t),ds->tl,
shared_atts),_tmp833)))));}}}static struct _tuple12 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms);static struct _tuple12
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,
struct Cyc_List_List*tms){if(tms == 0){struct _tuple12 _tmp835;return(_tmp835.f1=tq,((
_tmp835.f2=t,((_tmp835.f3=0,((_tmp835.f4=atts,_tmp835)))))));}{void*_tmpCF=(void*)
tms->hd;union Cyc_Absyn_Constraint*_tmpD0;struct Cyc_Position_Segment*_tmpD1;
struct Cyc_Absyn_Exp*_tmpD2;union Cyc_Absyn_Constraint*_tmpD3;struct Cyc_Position_Segment*
_tmpD4;void*_tmpD5;struct Cyc_List_List*_tmpD6;struct Cyc_Position_Segment*_tmpD7;
struct Cyc_Absyn_PtrAtts _tmpD8;struct Cyc_Absyn_Tqual _tmpD9;struct Cyc_Position_Segment*
_tmpDA;struct Cyc_List_List*_tmpDB;_LL79: if(*((int*)_tmpCF)!= 0)goto _LL7B;_tmpD0=((
struct Cyc_Absyn_Carray_mod_struct*)_tmpCF)->f1;_tmpD1=((struct Cyc_Absyn_Carray_mod_struct*)
_tmpCF)->f2;_LL7A: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(
t,tq,0,_tmpD0,_tmpD1),atts,tms->tl);_LL7B: if(*((int*)_tmpCF)!= 1)goto _LL7D;
_tmpD2=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCF)->f1;_tmpD3=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpCF)->f2;_tmpD4=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCF)->f3;_LL7C:
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(
struct Cyc_Absyn_Exp*)_tmpD2,_tmpD3,_tmpD4),atts,tms->tl);_LL7D: if(*((int*)_tmpCF)
!= 3)goto _LL7F;_tmpD5=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmpCF)->f1;
_LL7E: {void*_tmpDC=_tmpD5;struct Cyc_List_List*_tmpDD;int _tmpDE;struct Cyc_Absyn_VarargInfo*
_tmpDF;struct Cyc_Core_Opt*_tmpE0;struct Cyc_List_List*_tmpE1;struct Cyc_Position_Segment*
_tmpE2;_LL86: if(*((int*)_tmpDC)!= 1)goto _LL88;_tmpDD=((struct Cyc_Absyn_WithTypes_struct*)
_tmpDC)->f1;_tmpDE=((struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f2;_tmpDF=((
struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f3;_tmpE0=((struct Cyc_Absyn_WithTypes_struct*)
_tmpDC)->f4;_tmpE1=((struct Cyc_Absyn_WithTypes_struct*)_tmpDC)->f5;_LL87: {
struct Cyc_List_List*typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*
new_atts=0;{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((
void*)as->hd)){struct Cyc_List_List*_tmp836;fn_atts=((_tmp836=_cycalloc(sizeof(*
_tmp836)),((_tmp836->hd=(void*)((void*)as->hd),((_tmp836->tl=fn_atts,_tmp836))))));}
else{struct Cyc_List_List*_tmp837;new_atts=((_tmp837=_cycalloc(sizeof(*_tmp837)),((
_tmp837->hd=(void*)((void*)as->hd),((_tmp837->tl=new_atts,_tmp837))))));}}}if(
tms->tl != 0){void*_tmpE5=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;
struct Cyc_List_List*_tmpE6;_LL8B: if(*((int*)_tmpE5)!= 4)goto _LL8D;_tmpE6=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpE5)->f1;_LL8C: typvars=_tmpE6;tms=tms->tl;
goto _LL8A;_LL8D:;_LL8E: goto _LL8A;_LL8A:;}if(((((!_tmpDE  && _tmpDF == 0) && _tmpDD
!= 0) && _tmpDD->tl == 0) && (*((struct _tuple8*)_tmpDD->hd)).f1 == 0) && (*((
struct _tuple8*)_tmpDD->hd)).f3 == (void*)0)_tmpDD=0;t=Cyc_Parse_array2ptr(t,0);((
void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,
_tmpDD);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(tq.loc),Cyc_Absyn_function_typ(
typvars,_tmpE0,t,_tmpDD,_tmpDE,_tmpDF,_tmpE1,fn_atts),new_atts,((struct Cyc_List_List*)
_check_null(tms))->tl);}_LL88: if(*((int*)_tmpDC)!= 0)goto _LL85;_tmpE2=((struct
Cyc_Absyn_NoTypes_struct*)_tmpDC)->f2;_LL89: {const char*_tmp83A;void*_tmp839;(
_tmp839=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(_tmpE2,((_tmp83A="function declaration without parameter types",
_tag_dyneither(_tmp83A,sizeof(char),45))),_tag_dyneither(_tmp839,sizeof(void*),0)));}
_LL85:;}_LL7F: if(*((int*)_tmpCF)!= 4)goto _LL81;_tmpD6=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpCF)->f1;_tmpD7=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCF)->f2;_LL80:
if(tms->tl == 0){struct _tuple12 _tmp83B;return(_tmp83B.f1=tq,((_tmp83B.f2=t,((
_tmp83B.f3=_tmpD6,((_tmp83B.f4=atts,_tmp83B)))))));}{const char*_tmp83E;void*
_tmp83D;(_tmp83D=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD7,((_tmp83E="type parameters must appear before function arguments in declarator",
_tag_dyneither(_tmp83E,sizeof(char),68))),_tag_dyneither(_tmp83D,sizeof(void*),0)));}
_LL81: if(*((int*)_tmpCF)!= 2)goto _LL83;_tmpD8=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpCF)->f1;_tmpD9=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCF)->f2;_LL82: {
struct Cyc_Absyn_PointerType_struct _tmp844;struct Cyc_Absyn_PtrInfo _tmp843;struct
Cyc_Absyn_PointerType_struct*_tmp842;return Cyc_Parse_apply_tms(_tmpD9,(void*)((
_tmp842=_cycalloc(sizeof(*_tmp842)),((_tmp842[0]=((_tmp844.tag=4,((_tmp844.f1=((
_tmp843.elt_typ=t,((_tmp843.elt_tq=tq,((_tmp843.ptr_atts=_tmpD8,_tmp843)))))),
_tmp844)))),_tmp842)))),atts,tms->tl);}_LL83: if(*((int*)_tmpCF)!= 5)goto _LL78;
_tmpDA=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpCF)->f1;_tmpDB=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpCF)->f2;_LL84: return Cyc_Parse_apply_tms(tq,t,Cyc_List_append(atts,_tmpDB),
tms->tl);_LL78:;}}void*Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*
loc);void*Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*
loc){void*_tmpF0;struct Cyc_Core_Opt*_tmpF1;struct _tuple11 _tmpEF=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmpF0=_tmpEF.f1;_tmpF1=_tmpEF.f2;if(_tmpF1 != 0){const char*_tmp847;void*
_tmp846;(_tmp846=0,Cyc_Tcutil_warn(loc,((_tmp847="ignoring nested type declaration(s) in specifier list",
_tag_dyneither(_tmp847,sizeof(char),54))),_tag_dyneither(_tmp846,sizeof(void*),0)));}
return _tmpF0;}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*
loc,struct _tuple14*t);static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment*loc,struct _tuple14*t){struct _tuple14 _tmpF5;struct
_tuple1*_tmpF6;struct Cyc_Absyn_Tqual _tmpF7;void*_tmpF8;struct Cyc_List_List*
_tmpF9;struct Cyc_List_List*_tmpFA;struct _tuple14*_tmpF4=t;_tmpF5=*_tmpF4;_tmpF6=
_tmpF5.f1;_tmpF7=_tmpF5.f2;_tmpF8=_tmpF5.f3;_tmpF9=_tmpF5.f4;_tmpFA=_tmpF5.f5;
Cyc_Lex_register_typedef(_tmpF6);{struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
type;{void*_tmpFB=_tmpF8;struct Cyc_Core_Opt*_tmpFC;_LL90: if(_tmpFB <= (void*)4)
goto _LL92;if(*((int*)_tmpFB)!= 0)goto _LL92;_tmpFC=((struct Cyc_Absyn_Evar_struct*)
_tmpFB)->f1;_LL91: type=0;if(_tmpFC == 0)kind=(struct Cyc_Core_Opt*)& Cyc_Tcutil_bk;
else{kind=_tmpFC;}goto _LL8F;_LL92:;_LL93: kind=0;{struct Cyc_Core_Opt*_tmp848;type=((
_tmp848=_cycalloc(sizeof(*_tmp848)),((_tmp848->v=(void*)_tmpF8,_tmp848))));}goto
_LL8F;_LL8F:;}{struct Cyc_Absyn_Typedef_d_struct _tmp84E;struct Cyc_Absyn_Typedefdecl*
_tmp84D;struct Cyc_Absyn_Typedef_d_struct*_tmp84C;return Cyc_Absyn_new_decl((void*)((
_tmp84C=_cycalloc(sizeof(*_tmp84C)),((_tmp84C[0]=((_tmp84E.tag=9,((_tmp84E.f1=((
_tmp84D=_cycalloc(sizeof(*_tmp84D)),((_tmp84D->name=_tmpF6,((_tmp84D->tvs=_tmpF9,((
_tmp84D->kind=kind,((_tmp84D->defn=type,((_tmp84D->atts=_tmpFA,((_tmp84D->tq=
_tmpF7,_tmp84D)))))))))))))),_tmp84E)))),_tmp84C)))),loc);}}}static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*
s){struct Cyc_Absyn_Decl_s_struct _tmp851;struct Cyc_Absyn_Decl_s_struct*_tmp850;
return Cyc_Absyn_new_stmt((void*)((_tmp850=_cycalloc(sizeof(*_tmp850)),((_tmp850[
0]=((_tmp851.tag=11,((_tmp851.f1=d,((_tmp851.f2=s,_tmp851)))))),_tmp850)))),d->loc);}
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,
struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc);static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc){struct _RegionHandle _tmp103=_new_region("mkrgn");struct _RegionHandle*mkrgn=&
_tmp103;_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp105;struct Cyc_Absyn_Tqual
_tmp106;struct Cyc_List_List*_tmp107;struct Cyc_List_List*_tmp108;struct Cyc_Parse_Declaration_spec*
_tmp104=ds;_tmp105=*_tmp104;_tmp106=_tmp105.tq;_tmp107=_tmp105.type_specs;
_tmp108=_tmp105.attributes;if(_tmp106.loc == 0)_tmp106.loc=tqual_loc;if(ds->is_inline){
const char*_tmp852;Cyc_Parse_err(((_tmp852="inline is allowed only on function definitions",
_tag_dyneither(_tmp852,sizeof(char),47))),loc);}if(_tmp107 == 0){{const char*
_tmp853;Cyc_Parse_err(((_tmp853="missing type specifiers in declaration",
_tag_dyneither(_tmp853,sizeof(char),39))),loc);}{struct Cyc_List_List*_tmp10B=0;
_npop_handler(0);return _tmp10B;}}{void*s=(void*)2;int istypedef=0;if(ds->sc != 0){
void*_tmp10C=(void*)((struct Cyc_Core_Opt*)_check_null(ds->sc))->v;_LL95: if((int)
_tmp10C != 0)goto _LL97;_LL96: istypedef=1;goto _LL94;_LL97: if((int)_tmp10C != 1)goto
_LL99;_LL98: s=(void*)3;goto _LL94;_LL99: if((int)_tmp10C != 2)goto _LL9B;_LL9A: s=(
void*)4;goto _LL94;_LL9B: if((int)_tmp10C != 3)goto _LL9D;_LL9C: s=(void*)0;goto _LL94;
_LL9D: if((int)_tmp10C != 4)goto _LL9F;_LL9E: s=(void*)2;goto _LL94;_LL9F: if((int)
_tmp10C != 5)goto _LLA1;_LLA0: if(Cyc_Parse_no_register)s=(void*)2;else{s=(void*)5;}
goto _LL94;_LLA1: if((int)_tmp10C != 6)goto _LL94;_LLA2: s=(void*)1;goto _LL94;_LL94:;}{
struct Cyc_List_List*_tmp10E;struct Cyc_List_List*_tmp10F;struct _tuple0 _tmp10D=((
struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(mkrgn,mkrgn,ids);_tmp10E=_tmp10D.f1;_tmp10F=_tmp10D.f2;{int
exps_empty=1;{struct Cyc_List_List*es=_tmp10F;for(0;es != 0;es=es->tl){if((struct
Cyc_Absyn_Exp*)es->hd != 0){exps_empty=0;break;}}}{struct _tuple11 _tmp110=Cyc_Parse_collapse_type_specifiers(
_tmp107,loc);if(_tmp10E == 0){void*_tmp112;struct Cyc_Core_Opt*_tmp113;struct
_tuple11 _tmp111=_tmp110;_tmp112=_tmp111.f1;_tmp113=_tmp111.f2;if(_tmp113 != 0){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp113->v;{void*_tmp114=d->r;
struct Cyc_Absyn_Enumdecl*_tmp115;struct Cyc_Absyn_Aggrdecl*_tmp116;struct Cyc_Absyn_Datatypedecl*
_tmp117;_LLA4: if(_tmp114 <= (void*)2)goto _LLAA;if(*((int*)_tmp114)!= 8)goto _LLA6;
_tmp115=((struct Cyc_Absyn_Enum_d_struct*)_tmp114)->f1;_LLA5: _tmp115->sc=s;if(
_tmp108 != 0){const char*_tmp854;Cyc_Parse_err(((_tmp854="bad attributes on enum",
_tag_dyneither(_tmp854,sizeof(char),23))),loc);}goto _LLA3;_LLA6: if(*((int*)
_tmp114)!= 6)goto _LLA8;_tmp116=((struct Cyc_Absyn_Aggr_d_struct*)_tmp114)->f1;
_LLA7: _tmp116->sc=s;_tmp116->attributes=_tmp108;goto _LLA3;_LLA8: if(*((int*)
_tmp114)!= 7)goto _LLAA;_tmp117=((struct Cyc_Absyn_Datatype_d_struct*)_tmp114)->f1;
_LLA9: _tmp117->sc=s;if(_tmp108 != 0){const char*_tmp855;Cyc_Parse_err(((_tmp855="bad attributes on datatype",
_tag_dyneither(_tmp855,sizeof(char),27))),loc);}goto _LLA3;_LLAA:;_LLAB:{const
char*_tmp856;Cyc_Parse_err(((_tmp856="bad declaration",_tag_dyneither(_tmp856,
sizeof(char),16))),loc);}{struct Cyc_List_List*_tmp11B=0;_npop_handler(0);return
_tmp11B;}_LLA3:;}{struct Cyc_List_List*_tmp857;struct Cyc_List_List*_tmp11D=(
_tmp857=_cycalloc(sizeof(*_tmp857)),((_tmp857->hd=d,((_tmp857->tl=0,_tmp857)))));
_npop_handler(0);return _tmp11D;}}else{void*_tmp11E=_tmp112;struct Cyc_Absyn_AggrInfo
_tmp11F;union Cyc_Absyn_AggrInfoU _tmp120;struct _tuple3 _tmp121;void*_tmp122;struct
_tuple1*_tmp123;struct Cyc_List_List*_tmp124;struct Cyc_Absyn_DatatypeInfo _tmp125;
union Cyc_Absyn_DatatypeInfoU _tmp126;struct Cyc_Absyn_Datatypedecl**_tmp127;struct
Cyc_Absyn_DatatypeInfo _tmp128;union Cyc_Absyn_DatatypeInfoU _tmp129;struct Cyc_Absyn_UnknownDatatypeInfo
_tmp12A;struct _tuple1*_tmp12B;int _tmp12C;struct Cyc_List_List*_tmp12D;struct
_tuple1*_tmp12E;struct Cyc_List_List*_tmp12F;_LLAD: if(_tmp11E <= (void*)4)goto
_LLB7;if(*((int*)_tmp11E)!= 10)goto _LLAF;_tmp11F=((struct Cyc_Absyn_AggrType_struct*)
_tmp11E)->f1;_tmp120=_tmp11F.aggr_info;if((_tmp120.UnknownAggr).tag != 1)goto
_LLAF;_tmp121=(struct _tuple3)(_tmp120.UnknownAggr).val;_tmp122=_tmp121.f1;
_tmp123=_tmp121.f2;_tmp124=_tmp11F.targs;_LLAE: {struct Cyc_List_List*_tmp130=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp124);struct Cyc_Absyn_Aggrdecl*_tmp858;struct Cyc_Absyn_Aggrdecl*_tmp131=(
_tmp858=_cycalloc(sizeof(*_tmp858)),((_tmp858->kind=_tmp122,((_tmp858->sc=s,((
_tmp858->name=_tmp123,((_tmp858->tvs=_tmp130,((_tmp858->impl=0,((_tmp858->attributes=
0,_tmp858)))))))))))));if(_tmp108 != 0){const char*_tmp859;Cyc_Parse_err(((_tmp859="bad attributes on type declaration",
_tag_dyneither(_tmp859,sizeof(char),35))),loc);}{struct Cyc_Absyn_Aggr_d_struct*
_tmp85F;struct Cyc_Absyn_Aggr_d_struct _tmp85E;struct Cyc_List_List*_tmp85D;struct
Cyc_List_List*_tmp136=(_tmp85D=_cycalloc(sizeof(*_tmp85D)),((_tmp85D->hd=Cyc_Absyn_new_decl((
void*)((_tmp85F=_cycalloc(sizeof(*_tmp85F)),((_tmp85F[0]=((_tmp85E.tag=6,((
_tmp85E.f1=_tmp131,_tmp85E)))),_tmp85F)))),loc),((_tmp85D->tl=0,_tmp85D)))));
_npop_handler(0);return _tmp136;}}_LLAF: if(*((int*)_tmp11E)!= 2)goto _LLB1;_tmp125=((
struct Cyc_Absyn_DatatypeType_struct*)_tmp11E)->f1;_tmp126=_tmp125.datatype_info;
if((_tmp126.KnownDatatype).tag != 2)goto _LLB1;_tmp127=(struct Cyc_Absyn_Datatypedecl**)(
_tmp126.KnownDatatype).val;_LLB0: if(_tmp108 != 0){const char*_tmp860;Cyc_Parse_err(((
_tmp860="bad attributes on datatype",_tag_dyneither(_tmp860,sizeof(char),27))),
loc);}{struct Cyc_Absyn_Datatype_d_struct*_tmp866;struct Cyc_Absyn_Datatype_d_struct
_tmp865;struct Cyc_List_List*_tmp864;struct Cyc_List_List*_tmp13C=(_tmp864=
_cycalloc(sizeof(*_tmp864)),((_tmp864->hd=Cyc_Absyn_new_decl((void*)((_tmp866=
_cycalloc(sizeof(*_tmp866)),((_tmp866[0]=((_tmp865.tag=7,((_tmp865.f1=*_tmp127,
_tmp865)))),_tmp866)))),loc),((_tmp864->tl=0,_tmp864)))));_npop_handler(0);
return _tmp13C;}_LLB1: if(*((int*)_tmp11E)!= 2)goto _LLB3;_tmp128=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp11E)->f1;_tmp129=_tmp128.datatype_info;if((_tmp129.UnknownDatatype).tag != 1)
goto _LLB3;_tmp12A=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp129.UnknownDatatype).val;
_tmp12B=_tmp12A.name;_tmp12C=_tmp12A.is_extensible;_tmp12D=_tmp128.targs;_LLB2: {
struct Cyc_List_List*_tmp13D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp12D);struct Cyc_Absyn_Decl*_tmp13E=
Cyc_Absyn_datatype_decl(s,_tmp12B,_tmp13D,0,_tmp12C,loc);if(_tmp108 != 0){const
char*_tmp867;Cyc_Parse_err(((_tmp867="bad attributes on datatype",_tag_dyneither(
_tmp867,sizeof(char),27))),loc);}{struct Cyc_List_List*_tmp868;struct Cyc_List_List*
_tmp141=(_tmp868=_cycalloc(sizeof(*_tmp868)),((_tmp868->hd=_tmp13E,((_tmp868->tl=
0,_tmp868)))));_npop_handler(0);return _tmp141;}}_LLB3: if(*((int*)_tmp11E)!= 12)
goto _LLB5;_tmp12E=((struct Cyc_Absyn_EnumType_struct*)_tmp11E)->f1;_LLB4: {struct
Cyc_Absyn_Enumdecl*_tmp869;struct Cyc_Absyn_Enumdecl*_tmp142=(_tmp869=_cycalloc(
sizeof(*_tmp869)),((_tmp869->sc=s,((_tmp869->name=_tmp12E,((_tmp869->fields=0,
_tmp869)))))));if(_tmp108 != 0){const char*_tmp86A;Cyc_Parse_err(((_tmp86A="bad attributes on enum",
_tag_dyneither(_tmp86A,sizeof(char),23))),loc);}{struct Cyc_Absyn_Decl*_tmp874;
struct Cyc_Absyn_Enum_d_struct _tmp873;struct Cyc_Absyn_Enum_d_struct*_tmp872;
struct Cyc_List_List*_tmp871;struct Cyc_List_List*_tmp148=(_tmp871=_cycalloc(
sizeof(*_tmp871)),((_tmp871->hd=((_tmp874=_cycalloc(sizeof(*_tmp874)),((_tmp874->r=(
void*)((_tmp872=_cycalloc(sizeof(*_tmp872)),((_tmp872[0]=((_tmp873.tag=8,((
_tmp873.f1=_tmp142,_tmp873)))),_tmp872)))),((_tmp874->loc=loc,_tmp874)))))),((
_tmp871->tl=0,_tmp871)))));_npop_handler(0);return _tmp148;}}_LLB5: if(*((int*)
_tmp11E)!= 13)goto _LLB7;_tmp12F=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp11E)->f1;
_LLB6: {struct Cyc_Core_Opt*_tmp877;struct Cyc_Absyn_Enumdecl*_tmp876;struct Cyc_Absyn_Enumdecl*
_tmp14A=(_tmp876=_cycalloc(sizeof(*_tmp876)),((_tmp876->sc=s,((_tmp876->name=Cyc_Parse_gensym_enum(),((
_tmp876->fields=((_tmp877=_cycalloc(sizeof(*_tmp877)),((_tmp877->v=_tmp12F,
_tmp877)))),_tmp876)))))));if(_tmp108 != 0){const char*_tmp878;Cyc_Parse_err(((
_tmp878="bad attributes on enum",_tag_dyneither(_tmp878,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp882;struct Cyc_Absyn_Enum_d_struct _tmp881;struct Cyc_Absyn_Enum_d_struct*
_tmp880;struct Cyc_List_List*_tmp87F;struct Cyc_List_List*_tmp150=(_tmp87F=
_cycalloc(sizeof(*_tmp87F)),((_tmp87F->hd=((_tmp882=_cycalloc(sizeof(*_tmp882)),((
_tmp882->r=(void*)((_tmp880=_cycalloc(sizeof(*_tmp880)),((_tmp880[0]=((_tmp881.tag=
8,((_tmp881.f1=_tmp14A,_tmp881)))),_tmp880)))),((_tmp882->loc=loc,_tmp882)))))),((
_tmp87F->tl=0,_tmp87F)))));_npop_handler(0);return _tmp150;}}_LLB7:;_LLB8:{const
char*_tmp883;Cyc_Parse_err(((_tmp883="missing declarator",_tag_dyneither(_tmp883,
sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp154=0;_npop_handler(0);return
_tmp154;}_LLAC:;}}else{void*t=_tmp110.f1;struct Cyc_List_List*_tmp155=Cyc_Parse_apply_tmss(
mkrgn,_tmp106,t,_tmp10E,_tmp108);if(istypedef){if(!exps_empty){const char*_tmp884;
Cyc_Parse_err(((_tmp884="initializer in typedef declaration",_tag_dyneither(
_tmp884,sizeof(char),35))),loc);}{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple14*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp155);if(_tmp110.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(
_tmp110.f2)->v;{void*_tmp157=d->r;struct Cyc_Absyn_Aggrdecl*_tmp158;struct Cyc_Absyn_Datatypedecl*
_tmp159;struct Cyc_Absyn_Enumdecl*_tmp15A;_LLBA: if(_tmp157 <= (void*)2)goto _LLC0;
if(*((int*)_tmp157)!= 6)goto _LLBC;_tmp158=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp157)->f1;_LLBB: _tmp158->sc=s;_tmp158->attributes=_tmp108;_tmp108=0;goto _LLB9;
_LLBC: if(*((int*)_tmp157)!= 7)goto _LLBE;_tmp159=((struct Cyc_Absyn_Datatype_d_struct*)
_tmp157)->f1;_LLBD: _tmp159->sc=s;goto _LLB9;_LLBE: if(*((int*)_tmp157)!= 8)goto
_LLC0;_tmp15A=((struct Cyc_Absyn_Enum_d_struct*)_tmp157)->f1;_LLBF: _tmp15A->sc=s;
goto _LLB9;_LLC0:;_LLC1:{const char*_tmp885;Cyc_Parse_err(((_tmp885="declaration within typedef is not a struct, union, or datatype",
_tag_dyneither(_tmp885,sizeof(char),63))),loc);}goto _LLB9;_LLB9:;}{struct Cyc_List_List*
_tmp886;decls=((_tmp886=_cycalloc(sizeof(*_tmp886)),((_tmp886->hd=d,((_tmp886->tl=
decls,_tmp886))))));}}{struct Cyc_List_List*_tmp15D=decls;_npop_handler(0);return
_tmp15D;}}}else{if(_tmp110.f2 != 0){const char*_tmp887;Cyc_Parse_unimp(((_tmp887="nested type declaration within declarator",
_tag_dyneither(_tmp887,sizeof(char),42))),loc);}{struct Cyc_List_List*decls=0;{
struct Cyc_List_List*_tmp15F=_tmp155;for(0;_tmp15F != 0;(_tmp15F=_tmp15F->tl,
_tmp10F=_tmp10F->tl)){struct _tuple14 _tmp161;struct _tuple1*_tmp162;struct Cyc_Absyn_Tqual
_tmp163;void*_tmp164;struct Cyc_List_List*_tmp165;struct Cyc_List_List*_tmp166;
struct _tuple14*_tmp160=(struct _tuple14*)_tmp15F->hd;_tmp161=*_tmp160;_tmp162=
_tmp161.f1;_tmp163=_tmp161.f2;_tmp164=_tmp161.f3;_tmp165=_tmp161.f4;_tmp166=
_tmp161.f5;if(_tmp165 != 0){const char*_tmp88A;void*_tmp889;(_tmp889=0,Cyc_Tcutil_warn(
loc,((_tmp88A="bad type params, ignoring",_tag_dyneither(_tmp88A,sizeof(char),26))),
_tag_dyneither(_tmp889,sizeof(void*),0)));}if(_tmp10F == 0){const char*_tmp88D;
void*_tmp88C;(_tmp88C=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp88D="unexpected NULL in parse!",
_tag_dyneither(_tmp88D,sizeof(char),26))),_tag_dyneither(_tmp88C,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp16B=(struct Cyc_Absyn_Exp*)_tmp10F->hd;struct Cyc_Absyn_Vardecl*
_tmp16C=Cyc_Absyn_new_vardecl(_tmp162,_tmp164,_tmp16B);_tmp16C->tq=_tmp163;
_tmp16C->sc=s;_tmp16C->attributes=_tmp166;{struct Cyc_Absyn_Var_d_struct*_tmp893;
struct Cyc_Absyn_Var_d_struct _tmp892;struct Cyc_Absyn_Decl*_tmp891;struct Cyc_Absyn_Decl*
_tmp16D=(_tmp891=_cycalloc(sizeof(*_tmp891)),((_tmp891->r=(void*)((_tmp893=
_cycalloc(sizeof(*_tmp893)),((_tmp893[0]=((_tmp892.tag=0,((_tmp892.f1=_tmp16C,
_tmp892)))),_tmp893)))),((_tmp891->loc=loc,_tmp891)))));struct Cyc_List_List*
_tmp894;decls=((_tmp894=_cycalloc(sizeof(*_tmp894)),((_tmp894->hd=_tmp16D,((
_tmp894->tl=decls,_tmp894))))));}}}}{struct Cyc_List_List*_tmp172=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp172;}}}}}}}}};
_pop_region(mkrgn);}static void*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,struct
Cyc_Position_Segment*loc);static void*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,
struct Cyc_Position_Segment*loc){if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || 
Cyc_strlen((struct _dyneither_ptr)s)== 2)switch(*((const char*)
_check_dyneither_subscript(s,sizeof(char),0))){case 'A': _LLC2: return(void*)0;case
'M': _LLC3: return(void*)1;case 'B': _LLC4: return(void*)2;case 'R': _LLC5: return(void*)
3;case 'U': _LLC6: if(*((const char*)_check_dyneither_subscript(s,sizeof(char),1))== 'R')
return(void*)4;else{break;}case 'T': _LLC7: if(*((const char*)
_check_dyneither_subscript(s,sizeof(char),1))== 'R')return(void*)5;else{break;}
case 'E': _LLC8: return(void*)6;case 'I': _LLC9: return(void*)7;default: _LLCA: break;}{
const char*_tmp899;void*_tmp898[2];struct Cyc_String_pa_struct _tmp897;struct Cyc_Int_pa_struct
_tmp896;Cyc_Parse_err((struct _dyneither_ptr)((_tmp896.tag=1,((_tmp896.f1=(
unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp897.tag=0,((
_tmp897.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp898[0]=& _tmp897,((
_tmp898[1]=& _tmp896,Cyc_aprintf(((_tmp899="bad kind: %s; strlen=%d",
_tag_dyneither(_tmp899,sizeof(char),24))),_tag_dyneither(_tmp898,sizeof(void*),2)))))))))))))),
loc);}return(void*)2;}static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*
p);static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*
_tmp177=p->r;struct _tuple1*_tmp178;struct Cyc_Absyn_Vardecl*_tmp179;struct Cyc_Absyn_Pat*
_tmp17A;struct Cyc_Absyn_Pat _tmp17B;void*_tmp17C;struct Cyc_Absyn_Pat*_tmp17D;void*
_tmp17E;int _tmp17F;char _tmp180;struct _dyneither_ptr _tmp181;struct _tuple1*_tmp182;
struct Cyc_List_List*_tmp183;int _tmp184;struct Cyc_Absyn_Exp*_tmp185;_LLCD: if(
_tmp177 <= (void*)2)goto _LLD3;if(*((int*)_tmp177)!= 12)goto _LLCF;_tmp178=((struct
Cyc_Absyn_UnknownId_p_struct*)_tmp177)->f1;_LLCE: {struct Cyc_Absyn_UnknownId_e_struct
_tmp89C;struct Cyc_Absyn_UnknownId_e_struct*_tmp89B;return Cyc_Absyn_new_exp((void*)((
_tmp89B=_cycalloc(sizeof(*_tmp89B)),((_tmp89B[0]=((_tmp89C.tag=2,((_tmp89C.f1=
_tmp178,_tmp89C)))),_tmp89B)))),p->loc);}_LLCF: if(*((int*)_tmp177)!= 1)goto _LLD1;
_tmp179=((struct Cyc_Absyn_Reference_p_struct*)_tmp177)->f1;_tmp17A=((struct Cyc_Absyn_Reference_p_struct*)
_tmp177)->f2;_tmp17B=*_tmp17A;_tmp17C=_tmp17B.r;if((int)_tmp17C != 0)goto _LLD1;
_LLD0: {struct Cyc_Absyn_UnknownId_e_struct _tmp89F;struct Cyc_Absyn_UnknownId_e_struct*
_tmp89E;return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)((_tmp89E=_cycalloc(
sizeof(*_tmp89E)),((_tmp89E[0]=((_tmp89F.tag=2,((_tmp89F.f1=_tmp179->name,
_tmp89F)))),_tmp89E)))),p->loc),p->loc);}_LLD1: if(*((int*)_tmp177)!= 4)goto _LLD3;
_tmp17D=((struct Cyc_Absyn_Pointer_p_struct*)_tmp177)->f1;_LLD2: return Cyc_Absyn_address_exp(
Cyc_Parse_pat2exp(_tmp17D),p->loc);_LLD3: if((int)_tmp177 != 1)goto _LLD5;_LLD4:
return Cyc_Absyn_null_exp(p->loc);_LLD5: if(_tmp177 <= (void*)2)goto _LLDF;if(*((int*)
_tmp177)!= 7)goto _LLD7;_tmp17E=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp177)->f1;
_tmp17F=((struct Cyc_Absyn_Int_p_struct*)_tmp177)->f2;_LLD6: return Cyc_Absyn_int_exp(
_tmp17E,_tmp17F,p->loc);_LLD7: if(*((int*)_tmp177)!= 8)goto _LLD9;_tmp180=((struct
Cyc_Absyn_Char_p_struct*)_tmp177)->f1;_LLD8: return Cyc_Absyn_char_exp(_tmp180,p->loc);
_LLD9: if(*((int*)_tmp177)!= 9)goto _LLDB;_tmp181=((struct Cyc_Absyn_Float_p_struct*)
_tmp177)->f1;_LLDA: return Cyc_Absyn_float_exp(_tmp181,p->loc);_LLDB: if(*((int*)
_tmp177)!= 13)goto _LLDD;_tmp182=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp177)->f1;
_tmp183=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp177)->f2;_tmp184=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp177)->f3;if(_tmp184 != 0)goto _LLDD;_LLDC: {struct Cyc_Absyn_UnknownId_e_struct
_tmp8A2;struct Cyc_Absyn_UnknownId_e_struct*_tmp8A1;struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((
void*)((_tmp8A1=_cycalloc(sizeof(*_tmp8A1)),((_tmp8A1[0]=((_tmp8A2.tag=2,((
_tmp8A2.f1=_tmp182,_tmp8A2)))),_tmp8A1)))),p->loc);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp183);return Cyc_Absyn_unknowncall_exp(e1,
es,p->loc);}_LLDD: if(*((int*)_tmp177)!= 14)goto _LLDF;_tmp185=((struct Cyc_Absyn_Exp_p_struct*)
_tmp177)->f1;_LLDE: return _tmp185;_LLDF:;_LLE0:{const char*_tmp8A3;Cyc_Parse_err(((
_tmp8A3="cannot mix patterns and expressions in case",_tag_dyneither(_tmp8A3,
sizeof(char),44))),p->loc);}return Cyc_Absyn_null_exp(p->loc);_LLCC:;}union Cyc_YYSTYPE;
struct _union_YYSTYPE_Int_tok{int tag;struct _tuple6 val;};struct
_union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;
struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple1*val;};struct _tuple18{
struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*
f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple18*val;};struct
_union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;void*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*val;};struct
_union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY11{
int tag;struct Cyc_Absyn_Pat*val;};struct _tuple19{struct Cyc_List_List*f1;int f2;};
struct _union_YYSTYPE_YY12{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY13{
int tag;struct Cyc_List_List*val;};struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple20*val;};struct
_union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY16{
int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Absyn_Fndecl*
val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY19{int tag;struct Cyc_Parse_Declaration_spec*val;};struct _tuple21{
struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY20{
int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY22{int tag;void*val;};struct _union_YYSTYPE_YY23{int
tag;void*val;};struct _union_YYSTYPE_YY24{int tag;void*val;};struct
_union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY26{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY29{int tag;struct Cyc_Parse_Declarator*val;};struct
_union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator*val;};struct
_union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;void*val;};
struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct
_union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple22{struct Cyc_Absyn_Tqual
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int
tag;struct _tuple22*val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct
_union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple23*val;};struct
_union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};
struct _union_YYSTYPE_YY43{int tag;void*val;};struct _union_YYSTYPE_YY44{int tag;
void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*
val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY49{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY50{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*
val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct
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
YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp18E[14]="$(sign_t,int)";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp18E,
_tmp18E,_tmp18E + 14}};struct _tuple6 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1);struct
_tuple6 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){struct _tuple6 yyzzz;{union Cyc_YYSTYPE
_tmp18F=yy1;struct _tuple6 _tmp190;_LLE2: if((_tmp18F.Int_tok).tag != 1)goto _LLE4;
_tmp190=(struct _tuple6)(_tmp18F.Int_tok).val;_LLE3: yyzzz=_tmp190;goto _LLE1;_LLE4:;
_LLE5:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLE1:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_Int_tok(struct _tuple6 yy1);union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple6 yy1){
union Cyc_YYSTYPE _tmp8A4;return((_tmp8A4.Int_tok).val=yy1,(((_tmp8A4.Int_tok).tag=
1,_tmp8A4)));}static char _tmp193[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp193,_tmp193,_tmp193 + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE
yy1);char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){char yyzzz;{union Cyc_YYSTYPE
_tmp194=yy1;char _tmp195;_LLE7: if((_tmp194.Char_tok).tag != 2)goto _LLE9;_tmp195=(
char)(_tmp194.Char_tok).val;_LLE8: yyzzz=_tmp195;goto _LLE6;_LLE9:;_LLEA:(int)
_throw((void*)& Cyc_yyfail_Char_tok);_LLE6:;}return yyzzz;}union Cyc_YYSTYPE Cyc_Char_tok(
char yy1);union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp8A5;return((
_tmp8A5.Char_tok).val=yy1,(((_tmp8A5.Char_tok).tag=2,_tmp8A5)));}static char
_tmp198[13]="string_t<`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp198,_tmp198,_tmp198 + 13}};struct _dyneither_ptr Cyc_yyget_String_tok(
union Cyc_YYSTYPE yy1);struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE
yy1){struct _dyneither_ptr yyzzz;{union Cyc_YYSTYPE _tmp199=yy1;struct _dyneither_ptr
_tmp19A;_LLEC: if((_tmp199.String_tok).tag != 3)goto _LLEE;_tmp19A=(struct
_dyneither_ptr)(_tmp199.String_tok).val;_LLED: yyzzz=_tmp19A;goto _LLEB;_LLEE:;
_LLEF:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLEB:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_String_tok(struct _dyneither_ptr yy1);union Cyc_YYSTYPE Cyc_String_tok(struct
_dyneither_ptr yy1){union Cyc_YYSTYPE _tmp8A6;return((_tmp8A6.String_tok).val=yy1,(((
_tmp8A6.String_tok).tag=3,_tmp8A6)));}static char _tmp19D[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp19D,
_tmp19D,_tmp19D + 56}};struct _tuple18*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1);struct
_tuple18*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){struct _tuple18*yyzzz;{union Cyc_YYSTYPE
_tmp19E=yy1;struct _tuple18*_tmp19F;_LLF1: if((_tmp19E.YY1).tag != 6)goto _LLF3;
_tmp19F=(struct _tuple18*)(_tmp19E.YY1).val;_LLF2: yyzzz=_tmp19F;goto _LLF0;_LLF3:;
_LLF4:(int)_throw((void*)& Cyc_yyfail_YY1);_LLF0:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY1(struct _tuple18*yy1);union Cyc_YYSTYPE Cyc_YY1(struct _tuple18*yy1){union Cyc_YYSTYPE
_tmp8A7;return((_tmp8A7.YY1).val=yy1,(((_tmp8A7.YY1).tag=6,_tmp8A7)));}static
char _tmp1A2[19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={
Cyc_Core_Failure,{_tmp1A2,_tmp1A2,_tmp1A2 + 19}};union Cyc_Absyn_Constraint*Cyc_yyget_YY2(
union Cyc_YYSTYPE yy1);union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp1A3=yy1;union Cyc_Absyn_Constraint*
_tmp1A4;_LLF6: if((_tmp1A3.YY2).tag != 7)goto _LLF8;_tmp1A4=(union Cyc_Absyn_Constraint*)(
_tmp1A3.YY2).val;_LLF7: yyzzz=_tmp1A4;goto _LLF5;_LLF8:;_LLF9:(int)_throw((void*)&
Cyc_yyfail_YY2);_LLF5:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*
yy1);union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE
_tmp8A8;return((_tmp8A8.YY2).val=yy1,(((_tmp8A8.YY2).tag=7,_tmp8A8)));}static
char _tmp1A7[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{
_tmp1A7,_tmp1A7,_tmp1A7 + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union Cyc_YYSTYPE
yy1);struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*
yyzzz;{union Cyc_YYSTYPE _tmp1A8=yy1;struct Cyc_Absyn_Exp*_tmp1A9;_LLFB: if((_tmp1A8.YY3).tag
!= 8)goto _LLFD;_tmp1A9=(struct Cyc_Absyn_Exp*)(_tmp1A8.YY3).val;_LLFC: yyzzz=
_tmp1A9;goto _LLFA;_LLFD:;_LLFE:(int)_throw((void*)& Cyc_yyfail_YY3);_LLFA:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY3(struct Cyc_Absyn_Exp*yy1);union Cyc_YYSTYPE
Cyc_YY3(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp8A9;return((_tmp8A9.YY3).val=
yy1,(((_tmp8A9.YY3).tag=8,_tmp8A9)));}static char _tmp1AC[10]="exp_opt_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1AC,_tmp1AC,
_tmp1AC + 10}};struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Exp*
Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE
_tmp1AD=yy1;struct Cyc_Absyn_Exp*_tmp1AE;_LL100: if((_tmp1AD.YY4).tag != 9)goto
_LL102;_tmp1AE=(struct Cyc_Absyn_Exp*)(_tmp1AD.YY4).val;_LL101: yyzzz=_tmp1AE;goto
_LLFF;_LL102:;_LL103:(int)_throw((void*)& Cyc_yyfail_YY4);_LLFF:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1);union Cyc_YYSTYPE Cyc_YY4(struct
Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp8AA;return((_tmp8AA.YY4).val=yy1,(((
_tmp8AA.YY4).tag=9,_tmp8AA)));}static char _tmp1B1[17]="list_t<exp_t,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1B1,_tmp1B1,
_tmp1B1 + 17}};struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1);struct Cyc_List_List*
Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp1B2=yy1;struct Cyc_List_List*_tmp1B3;_LL105: if((_tmp1B2.YY5).tag != 10)goto
_LL107;_tmp1B3=(struct Cyc_List_List*)(_tmp1B2.YY5).val;_LL106: yyzzz=_tmp1B3;goto
_LL104;_LL107:;_LL108:(int)_throw((void*)& Cyc_yyfail_YY5);_LL104:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1);union Cyc_YYSTYPE Cyc_YY5(struct
Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8AB;return((_tmp8AB.YY5).val=yy1,(((
_tmp8AB.YY5).tag=10,_tmp8AB)));}static char _tmp1B6[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1B6,
_tmp1B6,_tmp1B6 + 47}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{
union Cyc_YYSTYPE _tmp1B7=yy1;struct Cyc_List_List*_tmp1B8;_LL10A: if((_tmp1B7.YY6).tag
!= 11)goto _LL10C;_tmp1B8=(struct Cyc_List_List*)(_tmp1B7.YY6).val;_LL10B: yyzzz=
_tmp1B8;goto _LL109;_LL10C:;_LL10D:(int)_throw((void*)& Cyc_yyfail_YY6);_LL109:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY6(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8AC;return((_tmp8AC.YY6).val=
yy1,(((_tmp8AC.YY6).tag=11,_tmp8AC)));}static char _tmp1BB[9]="primop_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1BB,_tmp1BB,
_tmp1BB + 9}};void*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1);void*Cyc_yyget_YY7(union Cyc_YYSTYPE
yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp1BC=yy1;void*_tmp1BD;_LL10F: if((_tmp1BC.YY7).tag
!= 12)goto _LL111;_tmp1BD=(void*)(_tmp1BC.YY7).val;_LL110: yyzzz=_tmp1BD;goto
_LL10E;_LL111:;_LL112:(int)_throw((void*)& Cyc_yyfail_YY7);_LL10E:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY7(void*yy1);union Cyc_YYSTYPE Cyc_YY7(void*yy1){union Cyc_YYSTYPE
_tmp8AD;return((_tmp8AD.YY7).val=yy1,(((_tmp8AD.YY7).tag=12,_tmp8AD)));}static
char _tmp1C0[19]="opt_t<primop_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY8={
Cyc_Core_Failure,{_tmp1C0,_tmp1C0,_tmp1C0 + 19}};struct Cyc_Core_Opt*Cyc_yyget_YY8(
union Cyc_YYSTYPE yy1);struct Cyc_Core_Opt*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp1C1=yy1;struct Cyc_Core_Opt*_tmp1C2;
_LL114: if((_tmp1C1.YY8).tag != 13)goto _LL116;_tmp1C2=(struct Cyc_Core_Opt*)(
_tmp1C1.YY8).val;_LL115: yyzzz=_tmp1C2;goto _LL113;_LL116:;_LL117:(int)_throw((
void*)& Cyc_yyfail_YY8);_LL113:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY8(struct Cyc_Core_Opt*
yy1);union Cyc_YYSTYPE Cyc_YY8(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp8AE;
return((_tmp8AE.YY8).val=yy1,(((_tmp8AE.YY8).tag=13,_tmp8AE)));}static char
_tmp1C5[7]="qvar_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{_tmp1C5,_tmp1C5,_tmp1C5 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(
union Cyc_YYSTYPE yy1);struct _tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple1*yyzzz;{union Cyc_YYSTYPE _tmp1C6=yy1;struct _tuple1*_tmp1C7;_LL119:
if((_tmp1C6.QualId_tok).tag != 5)goto _LL11B;_tmp1C7=(struct _tuple1*)(_tmp1C6.QualId_tok).val;
_LL11A: yyzzz=_tmp1C7;goto _LL118;_LL11B:;_LL11C:(int)_throw((void*)& Cyc_yyfail_QualId_tok);
_LL118:;}return yyzzz;}union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple1*yy1);union Cyc_YYSTYPE
Cyc_QualId_tok(struct _tuple1*yy1){union Cyc_YYSTYPE _tmp8AF;return((_tmp8AF.QualId_tok).val=
yy1,(((_tmp8AF.QualId_tok).tag=5,_tmp8AF)));}static char _tmp1CA[7]="stmt_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp1CA,
_tmp1CA,_tmp1CA + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1);
struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Stmt*
yyzzz;{union Cyc_YYSTYPE _tmp1CB=yy1;struct Cyc_Absyn_Stmt*_tmp1CC;_LL11E: if((
_tmp1CB.YY9).tag != 14)goto _LL120;_tmp1CC=(struct Cyc_Absyn_Stmt*)(_tmp1CB.YY9).val;
_LL11F: yyzzz=_tmp1CC;goto _LL11D;_LL120:;_LL121:(int)_throw((void*)& Cyc_yyfail_YY9);
_LL11D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Stmt*yy1);union Cyc_YYSTYPE
Cyc_YY9(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp8B0;return((_tmp8B0.YY9).val=
yy1,(((_tmp8B0.YY9).tag=14,_tmp8B0)));}static char _tmp1CF[27]="list_t<switch_clause_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp1CF,
_tmp1CF,_tmp1CF + 27}};struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp1D0=yy1;struct Cyc_List_List*_tmp1D1;_LL123: if((
_tmp1D0.YY10).tag != 15)goto _LL125;_tmp1D1=(struct Cyc_List_List*)(_tmp1D0.YY10).val;
_LL124: yyzzz=_tmp1D1;goto _LL122;_LL125:;_LL126:(int)_throw((void*)& Cyc_yyfail_YY10);
_LL122:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY10(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8B1;return((_tmp8B1.YY10).val=
yy1,(((_tmp8B1.YY10).tag=15,_tmp8B1)));}static char _tmp1D4[6]="pat_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp1D4,_tmp1D4,
_tmp1D4 + 6}};struct Cyc_Absyn_Pat*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Pat*
Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE
_tmp1D5=yy1;struct Cyc_Absyn_Pat*_tmp1D6;_LL128: if((_tmp1D5.YY11).tag != 16)goto
_LL12A;_tmp1D6=(struct Cyc_Absyn_Pat*)(_tmp1D5.YY11).val;_LL129: yyzzz=_tmp1D6;
goto _LL127;_LL12A:;_LL12B:(int)_throw((void*)& Cyc_yyfail_YY11);_LL127:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY11(struct Cyc_Absyn_Pat*yy1);union Cyc_YYSTYPE Cyc_YY11(
struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp8B2;return((_tmp8B2.YY11).val=yy1,(((
_tmp8B2.YY11).tag=16,_tmp8B2)));}static char _tmp1D9[28]="$(list_t<pat_t,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp1D9,
_tmp1D9,_tmp1D9 + 28}};struct _tuple19*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1);struct
_tuple19*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){struct _tuple19*yyzzz;{union Cyc_YYSTYPE
_tmp1DA=yy1;struct _tuple19*_tmp1DB;_LL12D: if((_tmp1DA.YY12).tag != 17)goto _LL12F;
_tmp1DB=(struct _tuple19*)(_tmp1DA.YY12).val;_LL12E: yyzzz=_tmp1DB;goto _LL12C;
_LL12F:;_LL130:(int)_throw((void*)& Cyc_yyfail_YY12);_LL12C:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY12(struct _tuple19*yy1);union Cyc_YYSTYPE Cyc_YY12(struct _tuple19*
yy1){union Cyc_YYSTYPE _tmp8B3;return((_tmp8B3.YY12).val=yy1,(((_tmp8B3.YY12).tag=
17,_tmp8B3)));}static char _tmp1DE[17]="list_t<pat_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp1DE,_tmp1DE,_tmp1DE + 17}};struct Cyc_List_List*
Cyc_yyget_YY13(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp1DF=yy1;struct Cyc_List_List*
_tmp1E0;_LL132: if((_tmp1DF.YY13).tag != 18)goto _LL134;_tmp1E0=(struct Cyc_List_List*)(
_tmp1DF.YY13).val;_LL133: yyzzz=_tmp1E0;goto _LL131;_LL134:;_LL135:(int)_throw((
void*)& Cyc_yyfail_YY13);_LL131:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8B4;
return((_tmp8B4.YY13).val=yy1,(((_tmp8B4.YY13).tag=18,_tmp8B4)));}static char
_tmp1E3[36]="$(list_t<designator_t,`H>,pat_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp1E3,_tmp1E3,_tmp1E3 + 36}};struct _tuple20*
Cyc_yyget_YY14(union Cyc_YYSTYPE yy1);struct _tuple20*Cyc_yyget_YY14(union Cyc_YYSTYPE
yy1){struct _tuple20*yyzzz;{union Cyc_YYSTYPE _tmp1E4=yy1;struct _tuple20*_tmp1E5;
_LL137: if((_tmp1E4.YY14).tag != 19)goto _LL139;_tmp1E5=(struct _tuple20*)(_tmp1E4.YY14).val;
_LL138: yyzzz=_tmp1E5;goto _LL136;_LL139:;_LL13A:(int)_throw((void*)& Cyc_yyfail_YY14);
_LL136:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY14(struct _tuple20*yy1);union Cyc_YYSTYPE
Cyc_YY14(struct _tuple20*yy1){union Cyc_YYSTYPE _tmp8B5;return((_tmp8B5.YY14).val=
yy1,(((_tmp8B5.YY14).tag=19,_tmp8B5)));}static char _tmp1E8[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp1E8,
_tmp1E8,_tmp1E8 + 47}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp1E9=yy1;struct Cyc_List_List*_tmp1EA;_LL13C: if((
_tmp1E9.YY15).tag != 20)goto _LL13E;_tmp1EA=(struct Cyc_List_List*)(_tmp1E9.YY15).val;
_LL13D: yyzzz=_tmp1EA;goto _LL13B;_LL13E:;_LL13F:(int)_throw((void*)& Cyc_yyfail_YY15);
_LL13B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY15(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY15(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8B6;return((_tmp8B6.YY15).val=
yy1,(((_tmp8B6.YY15).tag=20,_tmp8B6)));}static char _tmp1ED[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp1ED,
_tmp1ED,_tmp1ED + 58}};struct _tuple19*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1);struct
_tuple19*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){struct _tuple19*yyzzz;{union Cyc_YYSTYPE
_tmp1EE=yy1;struct _tuple19*_tmp1EF;_LL141: if((_tmp1EE.YY16).tag != 21)goto _LL143;
_tmp1EF=(struct _tuple19*)(_tmp1EE.YY16).val;_LL142: yyzzz=_tmp1EF;goto _LL140;
_LL143:;_LL144:(int)_throw((void*)& Cyc_yyfail_YY16);_LL140:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY16(struct _tuple19*yy1);union Cyc_YYSTYPE Cyc_YY16(struct _tuple19*
yy1){union Cyc_YYSTYPE _tmp8B7;return((_tmp8B7.YY16).val=yy1,(((_tmp8B7.YY16).tag=
21,_tmp8B7)));}static char _tmp1F2[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp1F2,_tmp1F2,_tmp1F2 + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_YY17(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(union
Cyc_YYSTYPE yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE _tmp1F3=yy1;struct
Cyc_Absyn_Fndecl*_tmp1F4;_LL146: if((_tmp1F3.YY17).tag != 22)goto _LL148;_tmp1F4=(
struct Cyc_Absyn_Fndecl*)(_tmp1F3.YY17).val;_LL147: yyzzz=_tmp1F4;goto _LL145;
_LL148:;_LL149:(int)_throw((void*)& Cyc_yyfail_YY17);_LL145:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY17(struct Cyc_Absyn_Fndecl*yy1);union Cyc_YYSTYPE Cyc_YY17(struct
Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp8B8;return((_tmp8B8.YY17).val=yy1,(((
_tmp8B8.YY17).tag=22,_tmp8B8)));}static char _tmp1F7[18]="list_t<decl_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp1F7,
_tmp1F7,_tmp1F7 + 18}};struct Cyc_List_List*Cyc_yyget_YY18(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp1F8=yy1;struct Cyc_List_List*_tmp1F9;_LL14B: if((
_tmp1F8.YY18).tag != 23)goto _LL14D;_tmp1F9=(struct Cyc_List_List*)(_tmp1F8.YY18).val;
_LL14C: yyzzz=_tmp1F9;goto _LL14A;_LL14D:;_LL14E:(int)_throw((void*)& Cyc_yyfail_YY18);
_LL14A:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY18(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY18(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8B9;return((_tmp8B9.YY18).val=
yy1,(((_tmp8B9.YY18).tag=23,_tmp8B9)));}static char _tmp1FC[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp1FC,
_tmp1FC,_tmp1FC + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE
yy1);struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){struct
Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE _tmp1FD=yy1;struct Cyc_Parse_Declaration_spec*
_tmp1FE;_LL150: if((_tmp1FD.YY19).tag != 24)goto _LL152;_tmp1FE=(struct Cyc_Parse_Declaration_spec*)(
_tmp1FD.YY19).val;_LL151: yyzzz=_tmp1FE;goto _LL14F;_LL152:;_LL153:(int)_throw((
void*)& Cyc_yyfail_YY19);_LL14F:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*
yy1);union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*yy1){union Cyc_YYSTYPE
_tmp8BA;return((_tmp8BA.YY19).val=yy1,(((_tmp8BA.YY19).tag=24,_tmp8BA)));}static
char _tmp201[29]="$(declarator_t,exp_opt_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp201,_tmp201,_tmp201 + 29}};struct _tuple21*
Cyc_yyget_YY20(union Cyc_YYSTYPE yy1);struct _tuple21*Cyc_yyget_YY20(union Cyc_YYSTYPE
yy1){struct _tuple21*yyzzz;{union Cyc_YYSTYPE _tmp202=yy1;struct _tuple21*_tmp203;
_LL155: if((_tmp202.YY20).tag != 25)goto _LL157;_tmp203=(struct _tuple21*)(_tmp202.YY20).val;
_LL156: yyzzz=_tmp203;goto _LL154;_LL157:;_LL158:(int)_throw((void*)& Cyc_yyfail_YY20);
_LL154:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY20(struct _tuple21*yy1);union Cyc_YYSTYPE
Cyc_YY20(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp8BB;return((_tmp8BB.YY20).val=
yy1,(((_tmp8BB.YY20).tag=25,_tmp8BB)));}static char _tmp206[40]="list_t<$(declarator_t,exp_opt_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp206,
_tmp206,_tmp206 + 40}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp207=yy1;struct Cyc_List_List*_tmp208;_LL15A: if((
_tmp207.YY21).tag != 26)goto _LL15C;_tmp208=(struct Cyc_List_List*)(_tmp207.YY21).val;
_LL15B: yyzzz=_tmp208;goto _LL159;_LL15C:;_LL15D:(int)_throw((void*)& Cyc_yyfail_YY21);
_LL159:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY21(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY21(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8BC;return((_tmp8BC.YY21).val=
yy1,(((_tmp8BC.YY21).tag=26,_tmp8BC)));}static char _tmp20B[16]="storage_class_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp20B,
_tmp20B,_tmp20B + 16}};void*Cyc_yyget_YY22(union Cyc_YYSTYPE yy1);void*Cyc_yyget_YY22(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp20C=yy1;void*_tmp20D;_LL15F:
if((_tmp20C.YY22).tag != 27)goto _LL161;_tmp20D=(void*)(_tmp20C.YY22).val;_LL160:
yyzzz=_tmp20D;goto _LL15E;_LL161:;_LL162:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL15E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY22(void*yy1);union Cyc_YYSTYPE Cyc_YY22(
void*yy1){union Cyc_YYSTYPE _tmp8BD;return((_tmp8BD.YY22).val=yy1,(((_tmp8BD.YY22).tag=
27,_tmp8BD)));}static char _tmp210[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp210,_tmp210,_tmp210 + 17}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE yy1);void*Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){void*yyzzz;{union
Cyc_YYSTYPE _tmp211=yy1;void*_tmp212;_LL164: if((_tmp211.YY23).tag != 28)goto _LL166;
_tmp212=(void*)(_tmp211.YY23).val;_LL165: yyzzz=_tmp212;goto _LL163;_LL166:;_LL167:(
int)_throw((void*)& Cyc_yyfail_YY23);_LL163:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY23(
void*yy1);union Cyc_YYSTYPE Cyc_YY23(void*yy1){union Cyc_YYSTYPE _tmp8BE;return((
_tmp8BE.YY23).val=yy1,(((_tmp8BE.YY23).tag=28,_tmp8BE)));}static char _tmp215[12]="aggr_kind_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp215,
_tmp215,_tmp215 + 12}};void*Cyc_yyget_YY24(union Cyc_YYSTYPE yy1);void*Cyc_yyget_YY24(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp216=yy1;void*_tmp217;_LL169:
if((_tmp216.YY24).tag != 29)goto _LL16B;_tmp217=(void*)(_tmp216.YY24).val;_LL16A:
yyzzz=_tmp217;goto _LL168;_LL16B:;_LL16C:(int)_throw((void*)& Cyc_yyfail_YY24);
_LL168:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY24(void*yy1);union Cyc_YYSTYPE Cyc_YY24(
void*yy1){union Cyc_YYSTYPE _tmp8BF;return((_tmp8BF.YY24).val=yy1,(((_tmp8BF.YY24).tag=
29,_tmp8BF)));}static char _tmp21A[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp21A,_tmp21A,_tmp21A + 8}};struct Cyc_Absyn_Tqual
Cyc_yyget_YY25(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE _tmp21B=yy1;struct Cyc_Absyn_Tqual
_tmp21C;_LL16E: if((_tmp21B.YY25).tag != 30)goto _LL170;_tmp21C=(struct Cyc_Absyn_Tqual)(
_tmp21B.YY25).val;_LL16F: yyzzz=_tmp21C;goto _LL16D;_LL170:;_LL171:(int)_throw((
void*)& Cyc_yyfail_YY25);_LL16D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual
yy1);union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp8C0;
return((_tmp8C0.YY25).val=yy1,(((_tmp8C0.YY25).tag=30,_tmp8C0)));}static char
_tmp21F[23]="list_t<aggrfield_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY26={
Cyc_Core_Failure,{_tmp21F,_tmp21F,_tmp21F + 23}};struct Cyc_List_List*Cyc_yyget_YY26(
union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp220=yy1;struct Cyc_List_List*
_tmp221;_LL173: if((_tmp220.YY26).tag != 31)goto _LL175;_tmp221=(struct Cyc_List_List*)(
_tmp220.YY26).val;_LL174: yyzzz=_tmp221;goto _LL172;_LL175:;_LL176:(int)_throw((
void*)& Cyc_yyfail_YY26);_LL172:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8C1;
return((_tmp8C1.YY26).val=yy1,(((_tmp8C1.YY26).tag=31,_tmp8C1)));}static char
_tmp224[34]="list_t<list_t<aggrfield_t,`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp224,_tmp224,_tmp224 + 34}};struct Cyc_List_List*
Cyc_yyget_YY27(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp225=yy1;struct Cyc_List_List*
_tmp226;_LL178: if((_tmp225.YY27).tag != 32)goto _LL17A;_tmp226=(struct Cyc_List_List*)(
_tmp225.YY27).val;_LL179: yyzzz=_tmp226;goto _LL177;_LL17A:;_LL17B:(int)_throw((
void*)& Cyc_yyfail_YY27);_LL177:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8C2;
return((_tmp8C2.YY27).val=yy1,(((_tmp8C2.YY27).tag=32,_tmp8C2)));}static char
_tmp229[31]="list_t<type_modifier_t<`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp229,_tmp229,_tmp229 + 31}};struct Cyc_List_List*
Cyc_yyget_YY28(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp22A=yy1;struct Cyc_List_List*
_tmp22B;_LL17D: if((_tmp22A.YY28).tag != 33)goto _LL17F;_tmp22B=(struct Cyc_List_List*)(
_tmp22A.YY28).val;_LL17E: yyzzz=_tmp22B;goto _LL17C;_LL17F:;_LL180:(int)_throw((
void*)& Cyc_yyfail_YY28);_LL17C:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8C3;
return((_tmp8C3.YY28).val=yy1,(((_tmp8C3.YY28).tag=33,_tmp8C3)));}static char
_tmp22E[13]="declarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={
Cyc_Core_Failure,{_tmp22E,_tmp22E,_tmp22E + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(
union Cyc_YYSTYPE yy1);struct Cyc_Parse_Declarator*Cyc_yyget_YY29(union Cyc_YYSTYPE
yy1){struct Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE _tmp22F=yy1;struct Cyc_Parse_Declarator*
_tmp230;_LL182: if((_tmp22F.YY29).tag != 34)goto _LL184;_tmp230=(struct Cyc_Parse_Declarator*)(
_tmp22F.YY29).val;_LL183: yyzzz=_tmp230;goto _LL181;_LL184:;_LL185:(int)_throw((
void*)& Cyc_yyfail_YY29);_LL181:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*
yy1);union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*yy1){union Cyc_YYSTYPE
_tmp8C4;return((_tmp8C4.YY29).val=yy1,(((_tmp8C4.YY29).tag=34,_tmp8C4)));}static
char _tmp233[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY30={
Cyc_Core_Failure,{_tmp233,_tmp233,_tmp233 + 21}};struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_YY30(union Cyc_YYSTYPE yy1);struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_YY30(
union Cyc_YYSTYPE yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{union Cyc_YYSTYPE
_tmp234=yy1;struct Cyc_Parse_Abstractdeclarator*_tmp235;_LL187: if((_tmp234.YY30).tag
!= 35)goto _LL189;_tmp235=(struct Cyc_Parse_Abstractdeclarator*)(_tmp234.YY30).val;
_LL188: yyzzz=_tmp235;goto _LL186;_LL189:;_LL18A:(int)_throw((void*)& Cyc_yyfail_YY30);
_LL186:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*
yy1);union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*yy1){union Cyc_YYSTYPE
_tmp8C5;return((_tmp8C5.YY30).val=yy1,(((_tmp8C5.YY30).tag=35,_tmp8C5)));}static
char _tmp238[5]="bool";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{
_tmp238,_tmp238,_tmp238 + 5}};int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1);int Cyc_yyget_YY31(
union Cyc_YYSTYPE yy1){int yyzzz;{union Cyc_YYSTYPE _tmp239=yy1;int _tmp23A;_LL18C: if((
_tmp239.YY31).tag != 36)goto _LL18E;_tmp23A=(int)(_tmp239.YY31).val;_LL18D: yyzzz=
_tmp23A;goto _LL18B;_LL18E:;_LL18F:(int)_throw((void*)& Cyc_yyfail_YY31);_LL18B:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY31(int yy1);union Cyc_YYSTYPE Cyc_YY31(int yy1){
union Cyc_YYSTYPE _tmp8C6;return((_tmp8C6.YY31).val=yy1,(((_tmp8C6.YY31).tag=36,
_tmp8C6)));}static char _tmp23D[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp23D,_tmp23D,_tmp23D + 8}};void*Cyc_yyget_YY32(
union Cyc_YYSTYPE yy1);void*Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){void*yyzzz;{union
Cyc_YYSTYPE _tmp23E=yy1;void*_tmp23F;_LL191: if((_tmp23E.YY32).tag != 37)goto _LL193;
_tmp23F=(void*)(_tmp23E.YY32).val;_LL192: yyzzz=_tmp23F;goto _LL190;_LL193:;_LL194:(
int)_throw((void*)& Cyc_yyfail_YY32);_LL190:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY32(
void*yy1);union Cyc_YYSTYPE Cyc_YY32(void*yy1){union Cyc_YYSTYPE _tmp8C7;return((
_tmp8C7.YY32).val=yy1,(((_tmp8C7.YY32).tag=37,_tmp8C7)));}static char _tmp242[16]="datatypefield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp242,
_tmp242,_tmp242 + 16}};struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE
yy1);struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Datatypefield*
yyzzz;{union Cyc_YYSTYPE _tmp243=yy1;struct Cyc_Absyn_Datatypefield*_tmp244;_LL196:
if((_tmp243.YY33).tag != 38)goto _LL198;_tmp244=(struct Cyc_Absyn_Datatypefield*)(
_tmp243.YY33).val;_LL197: yyzzz=_tmp244;goto _LL195;_LL198:;_LL199:(int)_throw((
void*)& Cyc_yyfail_YY33);_LL195:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*
yy1);union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE
_tmp8C8;return((_tmp8C8.YY33).val=yy1,(((_tmp8C8.YY33).tag=38,_tmp8C8)));}static
char _tmp247[27]="list_t<datatypefield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp247,_tmp247,_tmp247 + 27}};struct Cyc_List_List*
Cyc_yyget_YY34(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp248=yy1;struct Cyc_List_List*
_tmp249;_LL19B: if((_tmp248.YY34).tag != 39)goto _LL19D;_tmp249=(struct Cyc_List_List*)(
_tmp248.YY34).val;_LL19C: yyzzz=_tmp249;goto _LL19A;_LL19D:;_LL19E:(int)_throw((
void*)& Cyc_yyfail_YY34);_LL19A:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8C9;
return((_tmp8C9.YY34).val=yy1,(((_tmp8C9.YY34).tag=39,_tmp8C9)));}static char
_tmp24C[55]="$(tqual_t,list_t<type_specifier_t,`H>,attributes_t)@`H";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp24C,_tmp24C,
_tmp24C + 55}};struct _tuple22*Cyc_yyget_YY35(union Cyc_YYSTYPE yy1);struct _tuple22*
Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){struct _tuple22*yyzzz;{union Cyc_YYSTYPE
_tmp24D=yy1;struct _tuple22*_tmp24E;_LL1A0: if((_tmp24D.YY35).tag != 40)goto _LL1A2;
_tmp24E=(struct _tuple22*)(_tmp24D.YY35).val;_LL1A1: yyzzz=_tmp24E;goto _LL19F;
_LL1A2:;_LL1A3:(int)_throw((void*)& Cyc_yyfail_YY35);_LL19F:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY35(struct _tuple22*yy1);union Cyc_YYSTYPE Cyc_YY35(struct _tuple22*
yy1){union Cyc_YYSTYPE _tmp8CA;return((_tmp8CA.YY35).val=yy1,(((_tmp8CA.YY35).tag=
40,_tmp8CA)));}static char _tmp251[17]="list_t<var_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp251,_tmp251,_tmp251 + 17}};struct Cyc_List_List*
Cyc_yyget_YY36(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp252=yy1;struct Cyc_List_List*
_tmp253;_LL1A5: if((_tmp252.YY36).tag != 41)goto _LL1A7;_tmp253=(struct Cyc_List_List*)(
_tmp252.YY36).val;_LL1A6: yyzzz=_tmp253;goto _LL1A4;_LL1A7:;_LL1A8:(int)_throw((
void*)& Cyc_yyfail_YY36);_LL1A4:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8CB;
return((_tmp8CB.YY36).val=yy1,(((_tmp8CB.YY36).tag=41,_tmp8CB)));}static char
_tmp256[37]="$(opt_t<var_t,`H>,tqual_t,type_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp256,_tmp256,_tmp256 + 37}};struct _tuple8*
Cyc_yyget_YY37(union Cyc_YYSTYPE yy1);struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE
yy1){struct _tuple8*yyzzz;{union Cyc_YYSTYPE _tmp257=yy1;struct _tuple8*_tmp258;
_LL1AA: if((_tmp257.YY37).tag != 42)goto _LL1AC;_tmp258=(struct _tuple8*)(_tmp257.YY37).val;
_LL1AB: yyzzz=_tmp258;goto _LL1A9;_LL1AC:;_LL1AD:(int)_throw((void*)& Cyc_yyfail_YY37);
_LL1A9:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1);union Cyc_YYSTYPE
Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp8CC;return((_tmp8CC.YY37).val=
yy1,(((_tmp8CC.YY37).tag=42,_tmp8CC)));}static char _tmp25B[48]="list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp25B,
_tmp25B,_tmp25B + 48}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp25C=yy1;struct Cyc_List_List*_tmp25D;_LL1AF: if((
_tmp25C.YY38).tag != 43)goto _LL1B1;_tmp25D=(struct Cyc_List_List*)(_tmp25C.YY38).val;
_LL1B0: yyzzz=_tmp25D;goto _LL1AE;_LL1B1:;_LL1B2:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL1AE:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8CD;return((_tmp8CD.YY38).val=
yy1,(((_tmp8CD.YY38).tag=43,_tmp8CD)));}static char _tmp260[127]="$(list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,opt_t<type_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp260,
_tmp260,_tmp260 + 127}};struct _tuple23*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1);struct
_tuple23*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){struct _tuple23*yyzzz;{union Cyc_YYSTYPE
_tmp261=yy1;struct _tuple23*_tmp262;_LL1B4: if((_tmp261.YY39).tag != 44)goto _LL1B6;
_tmp262=(struct _tuple23*)(_tmp261.YY39).val;_LL1B5: yyzzz=_tmp262;goto _LL1B3;
_LL1B6:;_LL1B7:(int)_throw((void*)& Cyc_yyfail_YY39);_LL1B3:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY39(struct _tuple23*yy1);union Cyc_YYSTYPE Cyc_YY39(struct _tuple23*
yy1){union Cyc_YYSTYPE _tmp8CE;return((_tmp8CE.YY39).val=yy1,(((_tmp8CE.YY39).tag=
44,_tmp8CE)));}static char _tmp265[18]="list_t<type_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp265,_tmp265,_tmp265 + 18}};struct Cyc_List_List*
Cyc_yyget_YY40(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp266=yy1;struct Cyc_List_List*
_tmp267;_LL1B9: if((_tmp266.YY40).tag != 45)goto _LL1BB;_tmp267=(struct Cyc_List_List*)(
_tmp266.YY40).val;_LL1BA: yyzzz=_tmp267;goto _LL1B8;_LL1BB:;_LL1BC:(int)_throw((
void*)& Cyc_yyfail_YY40);_LL1B8:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8CF;
return((_tmp8CF.YY40).val=yy1,(((_tmp8CF.YY40).tag=45,_tmp8CF)));}static char
_tmp26A[24]="list_t<designator_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={
Cyc_Core_Failure,{_tmp26A,_tmp26A,_tmp26A + 24}};struct Cyc_List_List*Cyc_yyget_YY41(
union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp26B=yy1;struct Cyc_List_List*
_tmp26C;_LL1BE: if((_tmp26B.YY41).tag != 46)goto _LL1C0;_tmp26C=(struct Cyc_List_List*)(
_tmp26B.YY41).val;_LL1BF: yyzzz=_tmp26C;goto _LL1BD;_LL1C0:;_LL1C1:(int)_throw((
void*)& Cyc_yyfail_YY41);_LL1BD:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8D0;
return((_tmp8D0.YY41).val=yy1,(((_tmp8D0.YY41).tag=46,_tmp8D0)));}static char
_tmp26F[13]="designator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY42={
Cyc_Core_Failure,{_tmp26F,_tmp26F,_tmp26F + 13}};void*Cyc_yyget_YY42(union Cyc_YYSTYPE
yy1);void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp270=yy1;void*_tmp271;_LL1C3: if((_tmp270.YY42).tag != 47)goto _LL1C5;_tmp271=(
void*)(_tmp270.YY42).val;_LL1C4: yyzzz=_tmp271;goto _LL1C2;_LL1C5:;_LL1C6:(int)
_throw((void*)& Cyc_yyfail_YY42);_LL1C2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY42(
void*yy1);union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmp8D1;return((
_tmp8D1.YY42).val=yy1,(((_tmp8D1.YY42).tag=47,_tmp8D1)));}static char _tmp274[7]="kind_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp274,
_tmp274,_tmp274 + 7}};void*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1);void*Cyc_yyget_YY43(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp275=yy1;void*_tmp276;_LL1C8:
if((_tmp275.YY43).tag != 48)goto _LL1CA;_tmp276=(void*)(_tmp275.YY43).val;_LL1C9:
yyzzz=_tmp276;goto _LL1C7;_LL1CA:;_LL1CB:(int)_throw((void*)& Cyc_yyfail_YY43);
_LL1C7:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY43(void*yy1);union Cyc_YYSTYPE Cyc_YY43(
void*yy1){union Cyc_YYSTYPE _tmp8D2;return((_tmp8D2.YY43).val=yy1,(((_tmp8D2.YY43).tag=
48,_tmp8D2)));}static char _tmp279[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp279,_tmp279,_tmp279 + 7}};void*Cyc_yyget_YY44(
union Cyc_YYSTYPE yy1);void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){void*yyzzz;{union
Cyc_YYSTYPE _tmp27A=yy1;void*_tmp27B;_LL1CD: if((_tmp27A.YY44).tag != 49)goto _LL1CF;
_tmp27B=(void*)(_tmp27A.YY44).val;_LL1CE: yyzzz=_tmp27B;goto _LL1CC;_LL1CF:;_LL1D0:(
int)_throw((void*)& Cyc_yyfail_YY44);_LL1CC:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY44(
void*yy1);union Cyc_YYSTYPE Cyc_YY44(void*yy1){union Cyc_YYSTYPE _tmp8D3;return((
_tmp8D3.YY44).val=yy1,(((_tmp8D3.YY44).tag=49,_tmp8D3)));}static char _tmp27E[23]="list_t<attribute_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp27E,
_tmp27E,_tmp27E + 23}};struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp27F=yy1;struct Cyc_List_List*_tmp280;_LL1D2: if((
_tmp27F.YY45).tag != 50)goto _LL1D4;_tmp280=(struct Cyc_List_List*)(_tmp27F.YY45).val;
_LL1D3: yyzzz=_tmp280;goto _LL1D1;_LL1D4:;_LL1D5:(int)_throw((void*)& Cyc_yyfail_YY45);
_LL1D1:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8D4;return((_tmp8D4.YY45).val=
yy1,(((_tmp8D4.YY45).tag=50,_tmp8D4)));}static char _tmp283[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp283,
_tmp283,_tmp283 + 12}};void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1);void*Cyc_yyget_YY46(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp284=yy1;void*_tmp285;_LL1D7:
if((_tmp284.YY46).tag != 51)goto _LL1D9;_tmp285=(void*)(_tmp284.YY46).val;_LL1D8:
yyzzz=_tmp285;goto _LL1D6;_LL1D9:;_LL1DA:(int)_throw((void*)& Cyc_yyfail_YY46);
_LL1D6:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY46(void*yy1);union Cyc_YYSTYPE Cyc_YY46(
void*yy1){union Cyc_YYSTYPE _tmp8D5;return((_tmp8D5.YY46).val=yy1,(((_tmp8D5.YY46).tag=
51,_tmp8D5)));}static char _tmp288[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp288,_tmp288,_tmp288 + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union Cyc_YYSTYPE _tmp289=
yy1;struct Cyc_Absyn_Enumfield*_tmp28A;_LL1DC: if((_tmp289.YY47).tag != 52)goto
_LL1DE;_tmp28A=(struct Cyc_Absyn_Enumfield*)(_tmp289.YY47).val;_LL1DD: yyzzz=
_tmp28A;goto _LL1DB;_LL1DE:;_LL1DF:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1DB:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1);union Cyc_YYSTYPE
Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp8D6;return((_tmp8D6.YY47).val=
yy1,(((_tmp8D6.YY47).tag=52,_tmp8D6)));}static char _tmp28D[23]="list_t<enumfield_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp28D,
_tmp28D,_tmp28D + 23}};struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp28E=yy1;struct Cyc_List_List*_tmp28F;_LL1E1: if((
_tmp28E.YY48).tag != 53)goto _LL1E3;_tmp28F=(struct Cyc_List_List*)(_tmp28E.YY48).val;
_LL1E2: yyzzz=_tmp28F;goto _LL1E0;_LL1E3:;_LL1E4:(int)_throw((void*)& Cyc_yyfail_YY48);
_LL1E0:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8D7;return((_tmp8D7.YY48).val=
yy1,(((_tmp8D7.YY48).tag=53,_tmp8D7)));}static char _tmp292[17]="opt_t<type_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp292,
_tmp292,_tmp292 + 17}};struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1);
struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){struct Cyc_Core_Opt*yyzzz;{
union Cyc_YYSTYPE _tmp293=yy1;struct Cyc_Core_Opt*_tmp294;_LL1E6: if((_tmp293.YY49).tag
!= 54)goto _LL1E8;_tmp294=(struct Cyc_Core_Opt*)(_tmp293.YY49).val;_LL1E7: yyzzz=
_tmp294;goto _LL1E5;_LL1E8:;_LL1E9:(int)_throw((void*)& Cyc_yyfail_YY49);_LL1E5:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY49(struct Cyc_Core_Opt*yy1);union Cyc_YYSTYPE
Cyc_YY49(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp8D8;return((_tmp8D8.YY49).val=
yy1,(((_tmp8D8.YY49).tag=54,_tmp8D8)));}static char _tmp297[31]="list_t<$(type_t,type_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp297,
_tmp297,_tmp297 + 31}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp298=yy1;struct Cyc_List_List*_tmp299;_LL1EB: if((
_tmp298.YY50).tag != 55)goto _LL1ED;_tmp299=(struct Cyc_List_List*)(_tmp298.YY50).val;
_LL1EC: yyzzz=_tmp299;goto _LL1EA;_LL1ED:;_LL1EE:(int)_throw((void*)& Cyc_yyfail_YY50);
_LL1EA:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8D9;return((_tmp8D9.YY50).val=
yy1,(((_tmp8D9.YY50).tag=55,_tmp8D9)));}static char _tmp29C[15]="conref_t<bool>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp29C,
_tmp29C,_tmp29C + 15}};union Cyc_Absyn_Constraint*Cyc_yyget_YY51(union Cyc_YYSTYPE
yy1);union Cyc_Absyn_Constraint*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){union Cyc_Absyn_Constraint*
yyzzz;{union Cyc_YYSTYPE _tmp29D=yy1;union Cyc_Absyn_Constraint*_tmp29E;_LL1F0: if((
_tmp29D.YY51).tag != 56)goto _LL1F2;_tmp29E=(union Cyc_Absyn_Constraint*)(_tmp29D.YY51).val;
_LL1F1: yyzzz=_tmp29E;goto _LL1EF;_LL1F2:;_LL1F3:(int)_throw((void*)& Cyc_yyfail_YY51);
_LL1EF:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1);
union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp8DA;
return((_tmp8DA.YY51).val=yy1,(((_tmp8DA.YY51).tag=56,_tmp8DA)));}static char
_tmp2A1[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp2A1,_tmp2A1,_tmp2A1 + 45}};struct Cyc_List_List*
Cyc_yyget_YY52(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2A2=yy1;struct Cyc_List_List*
_tmp2A3;_LL1F5: if((_tmp2A2.YY52).tag != 57)goto _LL1F7;_tmp2A3=(struct Cyc_List_List*)(
_tmp2A2.YY52).val;_LL1F6: yyzzz=_tmp2A3;goto _LL1F4;_LL1F7:;_LL1F8:(int)_throw((
void*)& Cyc_yyfail_YY52);_LL1F4:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8DB;
return((_tmp8DB.YY52).val=yy1,(((_tmp8DB.YY52).tag=57,_tmp8DB)));}static char
_tmp2A6[15]="pointer_qual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY53={
Cyc_Core_Failure,{_tmp2A6,_tmp2A6,_tmp2A6 + 15}};void*Cyc_yyget_YY53(union Cyc_YYSTYPE
yy1);void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp2A7=yy1;void*_tmp2A8;_LL1FA: if((_tmp2A7.YY53).tag != 58)goto _LL1FC;_tmp2A8=(
void*)(_tmp2A7.YY53).val;_LL1FB: yyzzz=_tmp2A8;goto _LL1F9;_LL1FC:;_LL1FD:(int)
_throw((void*)& Cyc_yyfail_YY53);_LL1F9:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY53(
void*yy1);union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmp8DC;return((
_tmp8DC.YY53).val=yy1,(((_tmp8DC.YY53).tag=58,_tmp8DC)));}static char _tmp2AB[16]="pointer_quals_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp2AB,
_tmp2AB,_tmp2AB + 16}};struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2AC=yy1;struct Cyc_List_List*_tmp2AD;_LL1FF: if((
_tmp2AC.YY54).tag != 59)goto _LL201;_tmp2AD=(struct Cyc_List_List*)(_tmp2AC.YY54).val;
_LL200: yyzzz=_tmp2AD;goto _LL1FE;_LL201:;_LL202:(int)_throw((void*)& Cyc_yyfail_YY54);
_LL1FE:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY54(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp8DD;return((_tmp8DD.YY54).val=
yy1,(((_tmp8DD.YY54).tag=59,_tmp8DD)));}struct Cyc_Yyltype{int timestamp;int
first_line;int first_column;int last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc();
struct Cyc_Yyltype Cyc_yynewloc(){struct Cyc_Yyltype _tmp8DE;return(_tmp8DE.timestamp=
0,((_tmp8DE.first_line=0,((_tmp8DE.first_column=0,((_tmp8DE.last_line=0,((
_tmp8DE.last_column=0,_tmp8DE)))))))));}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
static short Cyc_yytranslate[372]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,143,2,2,127,141,138,2,124,125,132,135,120,139,129,140,2,2,2,2,
2,2,2,2,2,2,128,117,122,121,123,134,133,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,130,2,131,137,126,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,118,136,119,142,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,
45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,
72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,
99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116};static
char _tmp2B0[2]="$";static char _tmp2B1[6]="error";static char _tmp2B2[12]="$undefined.";
static char _tmp2B3[5]="AUTO";static char _tmp2B4[9]="REGISTER";static char _tmp2B5[7]="STATIC";
static char _tmp2B6[7]="EXTERN";static char _tmp2B7[8]="TYPEDEF";static char _tmp2B8[5]="VOID";
static char _tmp2B9[5]="CHAR";static char _tmp2BA[6]="SHORT";static char _tmp2BB[4]="INT";
static char _tmp2BC[5]="LONG";static char _tmp2BD[6]="FLOAT";static char _tmp2BE[7]="DOUBLE";
static char _tmp2BF[7]="SIGNED";static char _tmp2C0[9]="UNSIGNED";static char _tmp2C1[
6]="CONST";static char _tmp2C2[9]="VOLATILE";static char _tmp2C3[9]="RESTRICT";
static char _tmp2C4[7]="STRUCT";static char _tmp2C5[6]="UNION";static char _tmp2C6[5]="CASE";
static char _tmp2C7[8]="DEFAULT";static char _tmp2C8[7]="INLINE";static char _tmp2C9[7]="SIZEOF";
static char _tmp2CA[9]="OFFSETOF";static char _tmp2CB[3]="IF";static char _tmp2CC[5]="ELSE";
static char _tmp2CD[7]="SWITCH";static char _tmp2CE[6]="WHILE";static char _tmp2CF[3]="DO";
static char _tmp2D0[4]="FOR";static char _tmp2D1[5]="GOTO";static char _tmp2D2[9]="CONTINUE";
static char _tmp2D3[6]="BREAK";static char _tmp2D4[7]="RETURN";static char _tmp2D5[5]="ENUM";
static char _tmp2D6[8]="NULL_kw";static char _tmp2D7[4]="LET";static char _tmp2D8[6]="THROW";
static char _tmp2D9[4]="TRY";static char _tmp2DA[6]="CATCH";static char _tmp2DB[7]="EXPORT";
static char _tmp2DC[4]="NEW";static char _tmp2DD[9]="ABSTRACT";static char _tmp2DE[9]="FALLTHRU";
static char _tmp2DF[6]="USING";static char _tmp2E0[10]="NAMESPACE";static char _tmp2E1[
9]="DATATYPE";static char _tmp2E2[8]="XTUNION";static char _tmp2E3[7]="TUNION";
static char _tmp2E4[7]="MALLOC";static char _tmp2E5[8]="RMALLOC";static char _tmp2E6[7]="CALLOC";
static char _tmp2E7[8]="RCALLOC";static char _tmp2E8[5]="SWAP";static char _tmp2E9[9]="REGION_T";
static char _tmp2EA[6]="TAG_T";static char _tmp2EB[7]="REGION";static char _tmp2EC[5]="RNEW";
static char _tmp2ED[8]="REGIONS";static char _tmp2EE[13]="RESET_REGION";static char
_tmp2EF[4]="GEN";static char _tmp2F0[16]="NOZEROTERM_QUAL";static char _tmp2F1[14]="ZEROTERM_QUAL";
static char _tmp2F2[12]="REGION_QUAL";static char _tmp2F3[7]="PORTON";static char
_tmp2F4[8]="PORTOFF";static char _tmp2F5[12]="DYNREGION_T";static char _tmp2F6[6]="ALIAS";
static char _tmp2F7[8]="NUMELTS";static char _tmp2F8[8]="VALUEOF";static char _tmp2F9[
10]="VALUEOF_T";static char _tmp2FA[9]="TAGCHECK";static char _tmp2FB[13]="NUMELTS_QUAL";
static char _tmp2FC[10]="THIN_QUAL";static char _tmp2FD[9]="FAT_QUAL";static char
_tmp2FE[13]="NOTNULL_QUAL";static char _tmp2FF[14]="NULLABLE_QUAL";static char
_tmp300[12]="TAGGED_QUAL";static char _tmp301[16]="EXTENSIBLE_QUAL";static char
_tmp302[15]="RESETABLE_QUAL";static char _tmp303[7]="PTR_OP";static char _tmp304[7]="INC_OP";
static char _tmp305[7]="DEC_OP";static char _tmp306[8]="LEFT_OP";static char _tmp307[9]="RIGHT_OP";
static char _tmp308[6]="LE_OP";static char _tmp309[6]="GE_OP";static char _tmp30A[6]="EQ_OP";
static char _tmp30B[6]="NE_OP";static char _tmp30C[7]="AND_OP";static char _tmp30D[6]="OR_OP";
static char _tmp30E[11]="MUL_ASSIGN";static char _tmp30F[11]="DIV_ASSIGN";static char
_tmp310[11]="MOD_ASSIGN";static char _tmp311[11]="ADD_ASSIGN";static char _tmp312[11]="SUB_ASSIGN";
static char _tmp313[12]="LEFT_ASSIGN";static char _tmp314[13]="RIGHT_ASSIGN";static
char _tmp315[11]="AND_ASSIGN";static char _tmp316[11]="XOR_ASSIGN";static char
_tmp317[10]="OR_ASSIGN";static char _tmp318[9]="ELLIPSIS";static char _tmp319[11]="LEFT_RIGHT";
static char _tmp31A[12]="COLON_COLON";static char _tmp31B[11]="IDENTIFIER";static
char _tmp31C[17]="INTEGER_CONSTANT";static char _tmp31D[7]="STRING";static char
_tmp31E[19]="CHARACTER_CONSTANT";static char _tmp31F[18]="FLOATING_CONSTANT";
static char _tmp320[9]="TYPE_VAR";static char _tmp321[13]="TYPEDEF_NAME";static char
_tmp322[16]="QUAL_IDENTIFIER";static char _tmp323[18]="QUAL_TYPEDEF_NAME";static
char _tmp324[10]="ATTRIBUTE";static char _tmp325[4]="';'";static char _tmp326[4]="'{'";
static char _tmp327[4]="'}'";static char _tmp328[4]="','";static char _tmp329[4]="'='";
static char _tmp32A[4]="'<'";static char _tmp32B[4]="'>'";static char _tmp32C[4]="'('";
static char _tmp32D[4]="')'";static char _tmp32E[4]="'_'";static char _tmp32F[4]="'$'";
static char _tmp330[4]="':'";static char _tmp331[4]="'.'";static char _tmp332[4]="'['";
static char _tmp333[4]="']'";static char _tmp334[4]="'*'";static char _tmp335[4]="'@'";
static char _tmp336[4]="'?'";static char _tmp337[4]="'+'";static char _tmp338[4]="'|'";
static char _tmp339[4]="'^'";static char _tmp33A[4]="'&'";static char _tmp33B[4]="'-'";
static char _tmp33C[4]="'/'";static char _tmp33D[4]="'%'";static char _tmp33E[4]="'~'";
static char _tmp33F[4]="'!'";static char _tmp340[5]="prog";static char _tmp341[17]="translation_unit";
static char _tmp342[12]="export_list";static char _tmp343[19]="export_list_values";
static char _tmp344[21]="external_declaration";static char _tmp345[20]="function_definition";
static char _tmp346[21]="function_definition2";static char _tmp347[13]="using_action";
static char _tmp348[15]="unusing_action";static char _tmp349[17]="namespace_action";
static char _tmp34A[19]="unnamespace_action";static char _tmp34B[12]="declaration";
static char _tmp34C[19]="resetable_qual_opt";static char _tmp34D[17]="declaration_list";
static char _tmp34E[23]="declaration_specifiers";static char _tmp34F[24]="storage_class_specifier";
static char _tmp350[15]="attributes_opt";static char _tmp351[11]="attributes";static
char _tmp352[15]="attribute_list";static char _tmp353[10]="attribute";static char
_tmp354[15]="type_specifier";static char _tmp355[25]="type_specifier_notypedef";
static char _tmp356[5]="kind";static char _tmp357[15]="type_qualifier";static char
_tmp358[15]="enum_specifier";static char _tmp359[11]="enum_field";static char
_tmp35A[22]="enum_declaration_list";static char _tmp35B[26]="struct_or_union_specifier";
static char _tmp35C[16]="type_params_opt";static char _tmp35D[16]="struct_or_union";
static char _tmp35E[24]="struct_declaration_list";static char _tmp35F[25]="struct_declaration_list0";
static char _tmp360[21]="init_declarator_list";static char _tmp361[22]="init_declarator_list0";
static char _tmp362[16]="init_declarator";static char _tmp363[19]="struct_declaration";
static char _tmp364[25]="specifier_qualifier_list";static char _tmp365[35]="notypedef_specifier_qualifier_list";
static char _tmp366[23]="struct_declarator_list";static char _tmp367[24]="struct_declarator_list0";
static char _tmp368[18]="struct_declarator";static char _tmp369[19]="datatype_specifier";
static char _tmp36A[14]="qual_datatype";static char _tmp36B[19]="datatypefield_list";
static char _tmp36C[20]="datatypefield_scope";static char _tmp36D[14]="datatypefield";
static char _tmp36E[11]="declarator";static char _tmp36F[23]="declarator_withtypedef";
static char _tmp370[18]="direct_declarator";static char _tmp371[30]="direct_declarator_withtypedef";
static char _tmp372[8]="pointer";static char _tmp373[12]="one_pointer";static char
_tmp374[14]="pointer_quals";static char _tmp375[13]="pointer_qual";static char
_tmp376[23]="pointer_null_and_bound";static char _tmp377[14]="pointer_bound";
static char _tmp378[18]="zeroterm_qual_opt";static char _tmp379[12]="opt_rgn_opt";
static char _tmp37A[8]="rgn_opt";static char _tmp37B[11]="tqual_list";static char
_tmp37C[20]="parameter_type_list";static char _tmp37D[9]="type_var";static char
_tmp37E[16]="optional_effect";static char _tmp37F[19]="optional_rgn_order";static
char _tmp380[10]="rgn_order";static char _tmp381[16]="optional_inject";static char
_tmp382[11]="effect_set";static char _tmp383[14]="atomic_effect";static char _tmp384[
11]="region_set";static char _tmp385[15]="parameter_list";static char _tmp386[22]="parameter_declaration";
static char _tmp387[16]="identifier_list";static char _tmp388[17]="identifier_list0";
static char _tmp389[12]="initializer";static char _tmp38A[18]="array_initializer";
static char _tmp38B[17]="initializer_list";static char _tmp38C[12]="designation";
static char _tmp38D[16]="designator_list";static char _tmp38E[11]="designator";
static char _tmp38F[10]="type_name";static char _tmp390[14]="any_type_name";static
char _tmp391[15]="type_name_list";static char _tmp392[20]="abstract_declarator";
static char _tmp393[27]="direct_abstract_declarator";static char _tmp394[10]="statement";
static char _tmp395[13]="open_exp_opt";static char _tmp396[18]="labeled_statement";
static char _tmp397[21]="expression_statement";static char _tmp398[19]="compound_statement";
static char _tmp399[16]="block_item_list";static char _tmp39A[20]="selection_statement";
static char _tmp39B[15]="switch_clauses";static char _tmp39C[20]="iteration_statement";
static char _tmp39D[15]="jump_statement";static char _tmp39E[12]="exp_pattern";
static char _tmp39F[20]="conditional_pattern";static char _tmp3A0[19]="logical_or_pattern";
static char _tmp3A1[20]="logical_and_pattern";static char _tmp3A2[21]="inclusive_or_pattern";
static char _tmp3A3[21]="exclusive_or_pattern";static char _tmp3A4[12]="and_pattern";
static char _tmp3A5[17]="equality_pattern";static char _tmp3A6[19]="relational_pattern";
static char _tmp3A7[14]="shift_pattern";static char _tmp3A8[17]="additive_pattern";
static char _tmp3A9[23]="multiplicative_pattern";static char _tmp3AA[13]="cast_pattern";
static char _tmp3AB[14]="unary_pattern";static char _tmp3AC[16]="postfix_pattern";
static char _tmp3AD[16]="primary_pattern";static char _tmp3AE[8]="pattern";static
char _tmp3AF[19]="tuple_pattern_list";static char _tmp3B0[20]="tuple_pattern_list0";
static char _tmp3B1[14]="field_pattern";static char _tmp3B2[19]="field_pattern_list";
static char _tmp3B3[20]="field_pattern_list0";static char _tmp3B4[11]="expression";
static char _tmp3B5[22]="assignment_expression";static char _tmp3B6[20]="assignment_operator";
static char _tmp3B7[23]="conditional_expression";static char _tmp3B8[20]="constant_expression";
static char _tmp3B9[22]="logical_or_expression";static char _tmp3BA[23]="logical_and_expression";
static char _tmp3BB[24]="inclusive_or_expression";static char _tmp3BC[24]="exclusive_or_expression";
static char _tmp3BD[15]="and_expression";static char _tmp3BE[20]="equality_expression";
static char _tmp3BF[22]="relational_expression";static char _tmp3C0[17]="shift_expression";
static char _tmp3C1[20]="additive_expression";static char _tmp3C2[26]="multiplicative_expression";
static char _tmp3C3[16]="cast_expression";static char _tmp3C4[17]="unary_expression";
static char _tmp3C5[15]="unary_operator";static char _tmp3C6[19]="postfix_expression";
static char _tmp3C7[19]="primary_expression";static char _tmp3C8[25]="argument_expression_list";
static char _tmp3C9[26]="argument_expression_list0";static char _tmp3CA[9]="constant";
static char _tmp3CB[20]="qual_opt_identifier";static char _tmp3CC[17]="qual_opt_typedef";
static char _tmp3CD[18]="struct_union_name";static char _tmp3CE[11]="field_name";
static char _tmp3CF[12]="right_angle";static struct _dyneither_ptr Cyc_yytname[288]={{
_tmp2B0,_tmp2B0,_tmp2B0 + 2},{_tmp2B1,_tmp2B1,_tmp2B1 + 6},{_tmp2B2,_tmp2B2,
_tmp2B2 + 12},{_tmp2B3,_tmp2B3,_tmp2B3 + 5},{_tmp2B4,_tmp2B4,_tmp2B4 + 9},{_tmp2B5,
_tmp2B5,_tmp2B5 + 7},{_tmp2B6,_tmp2B6,_tmp2B6 + 7},{_tmp2B7,_tmp2B7,_tmp2B7 + 8},{
_tmp2B8,_tmp2B8,_tmp2B8 + 5},{_tmp2B9,_tmp2B9,_tmp2B9 + 5},{_tmp2BA,_tmp2BA,
_tmp2BA + 6},{_tmp2BB,_tmp2BB,_tmp2BB + 4},{_tmp2BC,_tmp2BC,_tmp2BC + 5},{_tmp2BD,
_tmp2BD,_tmp2BD + 6},{_tmp2BE,_tmp2BE,_tmp2BE + 7},{_tmp2BF,_tmp2BF,_tmp2BF + 7},{
_tmp2C0,_tmp2C0,_tmp2C0 + 9},{_tmp2C1,_tmp2C1,_tmp2C1 + 6},{_tmp2C2,_tmp2C2,
_tmp2C2 + 9},{_tmp2C3,_tmp2C3,_tmp2C3 + 9},{_tmp2C4,_tmp2C4,_tmp2C4 + 7},{_tmp2C5,
_tmp2C5,_tmp2C5 + 6},{_tmp2C6,_tmp2C6,_tmp2C6 + 5},{_tmp2C7,_tmp2C7,_tmp2C7 + 8},{
_tmp2C8,_tmp2C8,_tmp2C8 + 7},{_tmp2C9,_tmp2C9,_tmp2C9 + 7},{_tmp2CA,_tmp2CA,
_tmp2CA + 9},{_tmp2CB,_tmp2CB,_tmp2CB + 3},{_tmp2CC,_tmp2CC,_tmp2CC + 5},{_tmp2CD,
_tmp2CD,_tmp2CD + 7},{_tmp2CE,_tmp2CE,_tmp2CE + 6},{_tmp2CF,_tmp2CF,_tmp2CF + 3},{
_tmp2D0,_tmp2D0,_tmp2D0 + 4},{_tmp2D1,_tmp2D1,_tmp2D1 + 5},{_tmp2D2,_tmp2D2,
_tmp2D2 + 9},{_tmp2D3,_tmp2D3,_tmp2D3 + 6},{_tmp2D4,_tmp2D4,_tmp2D4 + 7},{_tmp2D5,
_tmp2D5,_tmp2D5 + 5},{_tmp2D6,_tmp2D6,_tmp2D6 + 8},{_tmp2D7,_tmp2D7,_tmp2D7 + 4},{
_tmp2D8,_tmp2D8,_tmp2D8 + 6},{_tmp2D9,_tmp2D9,_tmp2D9 + 4},{_tmp2DA,_tmp2DA,
_tmp2DA + 6},{_tmp2DB,_tmp2DB,_tmp2DB + 7},{_tmp2DC,_tmp2DC,_tmp2DC + 4},{_tmp2DD,
_tmp2DD,_tmp2DD + 9},{_tmp2DE,_tmp2DE,_tmp2DE + 9},{_tmp2DF,_tmp2DF,_tmp2DF + 6},{
_tmp2E0,_tmp2E0,_tmp2E0 + 10},{_tmp2E1,_tmp2E1,_tmp2E1 + 9},{_tmp2E2,_tmp2E2,
_tmp2E2 + 8},{_tmp2E3,_tmp2E3,_tmp2E3 + 7},{_tmp2E4,_tmp2E4,_tmp2E4 + 7},{_tmp2E5,
_tmp2E5,_tmp2E5 + 8},{_tmp2E6,_tmp2E6,_tmp2E6 + 7},{_tmp2E7,_tmp2E7,_tmp2E7 + 8},{
_tmp2E8,_tmp2E8,_tmp2E8 + 5},{_tmp2E9,_tmp2E9,_tmp2E9 + 9},{_tmp2EA,_tmp2EA,
_tmp2EA + 6},{_tmp2EB,_tmp2EB,_tmp2EB + 7},{_tmp2EC,_tmp2EC,_tmp2EC + 5},{_tmp2ED,
_tmp2ED,_tmp2ED + 8},{_tmp2EE,_tmp2EE,_tmp2EE + 13},{_tmp2EF,_tmp2EF,_tmp2EF + 4},{
_tmp2F0,_tmp2F0,_tmp2F0 + 16},{_tmp2F1,_tmp2F1,_tmp2F1 + 14},{_tmp2F2,_tmp2F2,
_tmp2F2 + 12},{_tmp2F3,_tmp2F3,_tmp2F3 + 7},{_tmp2F4,_tmp2F4,_tmp2F4 + 8},{_tmp2F5,
_tmp2F5,_tmp2F5 + 12},{_tmp2F6,_tmp2F6,_tmp2F6 + 6},{_tmp2F7,_tmp2F7,_tmp2F7 + 8},{
_tmp2F8,_tmp2F8,_tmp2F8 + 8},{_tmp2F9,_tmp2F9,_tmp2F9 + 10},{_tmp2FA,_tmp2FA,
_tmp2FA + 9},{_tmp2FB,_tmp2FB,_tmp2FB + 13},{_tmp2FC,_tmp2FC,_tmp2FC + 10},{_tmp2FD,
_tmp2FD,_tmp2FD + 9},{_tmp2FE,_tmp2FE,_tmp2FE + 13},{_tmp2FF,_tmp2FF,_tmp2FF + 14},{
_tmp300,_tmp300,_tmp300 + 12},{_tmp301,_tmp301,_tmp301 + 16},{_tmp302,_tmp302,
_tmp302 + 15},{_tmp303,_tmp303,_tmp303 + 7},{_tmp304,_tmp304,_tmp304 + 7},{_tmp305,
_tmp305,_tmp305 + 7},{_tmp306,_tmp306,_tmp306 + 8},{_tmp307,_tmp307,_tmp307 + 9},{
_tmp308,_tmp308,_tmp308 + 6},{_tmp309,_tmp309,_tmp309 + 6},{_tmp30A,_tmp30A,
_tmp30A + 6},{_tmp30B,_tmp30B,_tmp30B + 6},{_tmp30C,_tmp30C,_tmp30C + 7},{_tmp30D,
_tmp30D,_tmp30D + 6},{_tmp30E,_tmp30E,_tmp30E + 11},{_tmp30F,_tmp30F,_tmp30F + 11},{
_tmp310,_tmp310,_tmp310 + 11},{_tmp311,_tmp311,_tmp311 + 11},{_tmp312,_tmp312,
_tmp312 + 11},{_tmp313,_tmp313,_tmp313 + 12},{_tmp314,_tmp314,_tmp314 + 13},{
_tmp315,_tmp315,_tmp315 + 11},{_tmp316,_tmp316,_tmp316 + 11},{_tmp317,_tmp317,
_tmp317 + 10},{_tmp318,_tmp318,_tmp318 + 9},{_tmp319,_tmp319,_tmp319 + 11},{_tmp31A,
_tmp31A,_tmp31A + 12},{_tmp31B,_tmp31B,_tmp31B + 11},{_tmp31C,_tmp31C,_tmp31C + 17},{
_tmp31D,_tmp31D,_tmp31D + 7},{_tmp31E,_tmp31E,_tmp31E + 19},{_tmp31F,_tmp31F,
_tmp31F + 18},{_tmp320,_tmp320,_tmp320 + 9},{_tmp321,_tmp321,_tmp321 + 13},{_tmp322,
_tmp322,_tmp322 + 16},{_tmp323,_tmp323,_tmp323 + 18},{_tmp324,_tmp324,_tmp324 + 10},{
_tmp325,_tmp325,_tmp325 + 4},{_tmp326,_tmp326,_tmp326 + 4},{_tmp327,_tmp327,
_tmp327 + 4},{_tmp328,_tmp328,_tmp328 + 4},{_tmp329,_tmp329,_tmp329 + 4},{_tmp32A,
_tmp32A,_tmp32A + 4},{_tmp32B,_tmp32B,_tmp32B + 4},{_tmp32C,_tmp32C,_tmp32C + 4},{
_tmp32D,_tmp32D,_tmp32D + 4},{_tmp32E,_tmp32E,_tmp32E + 4},{_tmp32F,_tmp32F,
_tmp32F + 4},{_tmp330,_tmp330,_tmp330 + 4},{_tmp331,_tmp331,_tmp331 + 4},{_tmp332,
_tmp332,_tmp332 + 4},{_tmp333,_tmp333,_tmp333 + 4},{_tmp334,_tmp334,_tmp334 + 4},{
_tmp335,_tmp335,_tmp335 + 4},{_tmp336,_tmp336,_tmp336 + 4},{_tmp337,_tmp337,
_tmp337 + 4},{_tmp338,_tmp338,_tmp338 + 4},{_tmp339,_tmp339,_tmp339 + 4},{_tmp33A,
_tmp33A,_tmp33A + 4},{_tmp33B,_tmp33B,_tmp33B + 4},{_tmp33C,_tmp33C,_tmp33C + 4},{
_tmp33D,_tmp33D,_tmp33D + 4},{_tmp33E,_tmp33E,_tmp33E + 4},{_tmp33F,_tmp33F,
_tmp33F + 4},{_tmp340,_tmp340,_tmp340 + 5},{_tmp341,_tmp341,_tmp341 + 17},{_tmp342,
_tmp342,_tmp342 + 12},{_tmp343,_tmp343,_tmp343 + 19},{_tmp344,_tmp344,_tmp344 + 21},{
_tmp345,_tmp345,_tmp345 + 20},{_tmp346,_tmp346,_tmp346 + 21},{_tmp347,_tmp347,
_tmp347 + 13},{_tmp348,_tmp348,_tmp348 + 15},{_tmp349,_tmp349,_tmp349 + 17},{
_tmp34A,_tmp34A,_tmp34A + 19},{_tmp34B,_tmp34B,_tmp34B + 12},{_tmp34C,_tmp34C,
_tmp34C + 19},{_tmp34D,_tmp34D,_tmp34D + 17},{_tmp34E,_tmp34E,_tmp34E + 23},{
_tmp34F,_tmp34F,_tmp34F + 24},{_tmp350,_tmp350,_tmp350 + 15},{_tmp351,_tmp351,
_tmp351 + 11},{_tmp352,_tmp352,_tmp352 + 15},{_tmp353,_tmp353,_tmp353 + 10},{
_tmp354,_tmp354,_tmp354 + 15},{_tmp355,_tmp355,_tmp355 + 25},{_tmp356,_tmp356,
_tmp356 + 5},{_tmp357,_tmp357,_tmp357 + 15},{_tmp358,_tmp358,_tmp358 + 15},{_tmp359,
_tmp359,_tmp359 + 11},{_tmp35A,_tmp35A,_tmp35A + 22},{_tmp35B,_tmp35B,_tmp35B + 26},{
_tmp35C,_tmp35C,_tmp35C + 16},{_tmp35D,_tmp35D,_tmp35D + 16},{_tmp35E,_tmp35E,
_tmp35E + 24},{_tmp35F,_tmp35F,_tmp35F + 25},{_tmp360,_tmp360,_tmp360 + 21},{
_tmp361,_tmp361,_tmp361 + 22},{_tmp362,_tmp362,_tmp362 + 16},{_tmp363,_tmp363,
_tmp363 + 19},{_tmp364,_tmp364,_tmp364 + 25},{_tmp365,_tmp365,_tmp365 + 35},{
_tmp366,_tmp366,_tmp366 + 23},{_tmp367,_tmp367,_tmp367 + 24},{_tmp368,_tmp368,
_tmp368 + 18},{_tmp369,_tmp369,_tmp369 + 19},{_tmp36A,_tmp36A,_tmp36A + 14},{
_tmp36B,_tmp36B,_tmp36B + 19},{_tmp36C,_tmp36C,_tmp36C + 20},{_tmp36D,_tmp36D,
_tmp36D + 14},{_tmp36E,_tmp36E,_tmp36E + 11},{_tmp36F,_tmp36F,_tmp36F + 23},{
_tmp370,_tmp370,_tmp370 + 18},{_tmp371,_tmp371,_tmp371 + 30},{_tmp372,_tmp372,
_tmp372 + 8},{_tmp373,_tmp373,_tmp373 + 12},{_tmp374,_tmp374,_tmp374 + 14},{_tmp375,
_tmp375,_tmp375 + 13},{_tmp376,_tmp376,_tmp376 + 23},{_tmp377,_tmp377,_tmp377 + 14},{
_tmp378,_tmp378,_tmp378 + 18},{_tmp379,_tmp379,_tmp379 + 12},{_tmp37A,_tmp37A,
_tmp37A + 8},{_tmp37B,_tmp37B,_tmp37B + 11},{_tmp37C,_tmp37C,_tmp37C + 20},{_tmp37D,
_tmp37D,_tmp37D + 9},{_tmp37E,_tmp37E,_tmp37E + 16},{_tmp37F,_tmp37F,_tmp37F + 19},{
_tmp380,_tmp380,_tmp380 + 10},{_tmp381,_tmp381,_tmp381 + 16},{_tmp382,_tmp382,
_tmp382 + 11},{_tmp383,_tmp383,_tmp383 + 14},{_tmp384,_tmp384,_tmp384 + 11},{
_tmp385,_tmp385,_tmp385 + 15},{_tmp386,_tmp386,_tmp386 + 22},{_tmp387,_tmp387,
_tmp387 + 16},{_tmp388,_tmp388,_tmp388 + 17},{_tmp389,_tmp389,_tmp389 + 12},{
_tmp38A,_tmp38A,_tmp38A + 18},{_tmp38B,_tmp38B,_tmp38B + 17},{_tmp38C,_tmp38C,
_tmp38C + 12},{_tmp38D,_tmp38D,_tmp38D + 16},{_tmp38E,_tmp38E,_tmp38E + 11},{
_tmp38F,_tmp38F,_tmp38F + 10},{_tmp390,_tmp390,_tmp390 + 14},{_tmp391,_tmp391,
_tmp391 + 15},{_tmp392,_tmp392,_tmp392 + 20},{_tmp393,_tmp393,_tmp393 + 27},{
_tmp394,_tmp394,_tmp394 + 10},{_tmp395,_tmp395,_tmp395 + 13},{_tmp396,_tmp396,
_tmp396 + 18},{_tmp397,_tmp397,_tmp397 + 21},{_tmp398,_tmp398,_tmp398 + 19},{
_tmp399,_tmp399,_tmp399 + 16},{_tmp39A,_tmp39A,_tmp39A + 20},{_tmp39B,_tmp39B,
_tmp39B + 15},{_tmp39C,_tmp39C,_tmp39C + 20},{_tmp39D,_tmp39D,_tmp39D + 15},{
_tmp39E,_tmp39E,_tmp39E + 12},{_tmp39F,_tmp39F,_tmp39F + 20},{_tmp3A0,_tmp3A0,
_tmp3A0 + 19},{_tmp3A1,_tmp3A1,_tmp3A1 + 20},{_tmp3A2,_tmp3A2,_tmp3A2 + 21},{
_tmp3A3,_tmp3A3,_tmp3A3 + 21},{_tmp3A4,_tmp3A4,_tmp3A4 + 12},{_tmp3A5,_tmp3A5,
_tmp3A5 + 17},{_tmp3A6,_tmp3A6,_tmp3A6 + 19},{_tmp3A7,_tmp3A7,_tmp3A7 + 14},{
_tmp3A8,_tmp3A8,_tmp3A8 + 17},{_tmp3A9,_tmp3A9,_tmp3A9 + 23},{_tmp3AA,_tmp3AA,
_tmp3AA + 13},{_tmp3AB,_tmp3AB,_tmp3AB + 14},{_tmp3AC,_tmp3AC,_tmp3AC + 16},{
_tmp3AD,_tmp3AD,_tmp3AD + 16},{_tmp3AE,_tmp3AE,_tmp3AE + 8},{_tmp3AF,_tmp3AF,
_tmp3AF + 19},{_tmp3B0,_tmp3B0,_tmp3B0 + 20},{_tmp3B1,_tmp3B1,_tmp3B1 + 14},{
_tmp3B2,_tmp3B2,_tmp3B2 + 19},{_tmp3B3,_tmp3B3,_tmp3B3 + 20},{_tmp3B4,_tmp3B4,
_tmp3B4 + 11},{_tmp3B5,_tmp3B5,_tmp3B5 + 22},{_tmp3B6,_tmp3B6,_tmp3B6 + 20},{
_tmp3B7,_tmp3B7,_tmp3B7 + 23},{_tmp3B8,_tmp3B8,_tmp3B8 + 20},{_tmp3B9,_tmp3B9,
_tmp3B9 + 22},{_tmp3BA,_tmp3BA,_tmp3BA + 23},{_tmp3BB,_tmp3BB,_tmp3BB + 24},{
_tmp3BC,_tmp3BC,_tmp3BC + 24},{_tmp3BD,_tmp3BD,_tmp3BD + 15},{_tmp3BE,_tmp3BE,
_tmp3BE + 20},{_tmp3BF,_tmp3BF,_tmp3BF + 22},{_tmp3C0,_tmp3C0,_tmp3C0 + 17},{
_tmp3C1,_tmp3C1,_tmp3C1 + 20},{_tmp3C2,_tmp3C2,_tmp3C2 + 26},{_tmp3C3,_tmp3C3,
_tmp3C3 + 16},{_tmp3C4,_tmp3C4,_tmp3C4 + 17},{_tmp3C5,_tmp3C5,_tmp3C5 + 15},{
_tmp3C6,_tmp3C6,_tmp3C6 + 19},{_tmp3C7,_tmp3C7,_tmp3C7 + 19},{_tmp3C8,_tmp3C8,
_tmp3C8 + 25},{_tmp3C9,_tmp3C9,_tmp3C9 + 26},{_tmp3CA,_tmp3CA,_tmp3CA + 9},{_tmp3CB,
_tmp3CB,_tmp3CB + 20},{_tmp3CC,_tmp3CC,_tmp3CC + 17},{_tmp3CD,_tmp3CD,_tmp3CD + 18},{
_tmp3CE,_tmp3CE,_tmp3CE + 11},{_tmp3CF,_tmp3CF,_tmp3CF + 12}};static short Cyc_yyr1[
513]={0,144,145,145,145,145,145,145,145,145,145,145,146,146,147,147,148,148,148,
149,149,149,149,150,150,151,152,153,154,155,155,155,155,155,155,155,156,156,157,
157,158,158,158,158,158,158,158,158,158,158,159,159,159,159,159,159,159,160,160,
161,162,162,163,163,163,163,163,163,164,164,165,165,165,165,165,165,165,165,165,
165,165,165,165,165,165,165,165,165,165,165,165,165,166,167,167,167,168,168,168,
169,169,170,170,170,171,171,171,171,171,172,172,173,173,174,174,175,175,176,177,
177,178,178,179,180,180,180,180,180,180,181,181,181,181,181,181,182,183,183,184,
184,184,185,185,185,186,186,187,187,187,187,188,188,188,189,189,190,190,191,191,
192,192,192,192,192,192,192,192,192,192,193,193,193,193,193,193,193,193,193,193,
193,194,194,195,196,196,197,197,197,197,197,197,197,197,198,198,198,199,199,200,
200,200,201,201,201,202,202,202,203,203,204,204,204,204,205,205,206,206,207,207,
208,208,209,209,210,210,211,211,211,211,212,212,213,213,214,214,214,215,216,216,
217,217,218,218,218,218,219,219,219,219,220,221,221,222,222,223,223,224,224,224,
224,224,225,225,226,226,226,227,227,227,227,227,227,227,227,227,227,227,228,228,
228,228,228,228,228,229,229,230,231,231,232,232,233,233,233,233,233,233,234,234,
234,234,234,234,235,235,235,235,235,235,236,236,236,236,236,236,236,236,236,236,
236,236,236,236,237,237,237,237,237,237,237,237,238,239,239,240,240,241,241,242,
242,243,243,244,244,245,245,245,246,246,246,246,246,247,247,247,248,248,248,249,
249,249,249,250,250,251,251,251,251,251,251,251,252,253,254,254,254,254,254,254,
254,254,254,254,254,254,254,254,254,255,255,255,256,256,257,257,258,258,258,259,
259,260,260,261,261,261,262,262,262,262,262,262,262,262,262,262,262,263,263,263,
263,263,263,263,264,265,265,266,266,267,267,268,268,269,269,270,270,270,271,271,
271,271,271,272,272,272,273,273,273,274,274,274,274,275,275,276,276,276,276,276,
276,276,276,276,276,276,276,276,276,276,276,276,276,276,276,277,277,277,278,278,
278,278,278,278,278,278,278,278,279,279,279,279,279,279,279,279,279,280,281,281,
282,282,282,282,283,283,284,284,285,285,286,286,287,287};static short Cyc_yyr2[513]={
0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,7,5,8,0,1,1,2,
1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,
1,1,1,1,1,4,4,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,
1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,3,7,4,6,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,
3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,
1,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,
1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,
1,1,1,5,0,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,
7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,
3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,1,3,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,
1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,
3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,4,6,6,4,1,1,1,1,4,3,
4,3,3,2,2,6,7,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short
Cyc_yydefact[1035]={0,18,50,51,52,53,55,70,73,74,75,76,77,78,79,80,93,94,95,111,
112,46,0,0,56,0,0,144,0,90,0,0,0,0,0,0,0,37,503,213,505,504,506,0,0,71,0,196,196,
195,1,0,16,0,0,17,0,0,40,48,42,68,44,81,82,0,83,201,36,155,0,180,183,84,159,109,
54,53,47,0,97,502,0,503,499,500,501,109,0,373,0,0,0,0,236,0,375,376,25,27,0,0,0,
0,0,0,0,0,145,0,0,0,0,0,0,0,193,194,2,0,0,0,0,0,29,0,117,118,120,41,49,43,45,113,
507,508,109,203,0,202,36,38,36,0,19,0,215,0,168,156,181,190,189,0,0,187,188,191,
192,204,183,0,69,0,54,101,0,99,0,503,383,0,0,0,0,0,0,0,0,0,0,0,0,109,0,0,0,0,0,
489,0,0,0,0,0,476,474,475,0,400,402,416,424,426,428,430,432,434,437,442,445,448,
452,0,454,477,488,487,0,384,382,32,0,0,109,0,0,0,127,123,125,254,256,0,0,9,10,0,
0,0,109,509,510,214,92,0,0,160,72,234,0,231,0,3,0,5,0,284,0,30,0,0,36,20,0,114,
115,0,108,109,0,0,0,0,0,0,0,0,0,0,0,0,503,287,289,0,295,291,0,293,277,278,279,0,
280,281,282,0,39,21,120,261,0,221,237,0,0,217,215,0,198,0,0,0,206,57,205,184,0,0,
102,98,0,0,377,0,0,397,0,0,0,0,250,393,398,0,395,0,462,0,418,452,0,419,420,0,0,0,
0,0,0,0,0,0,0,455,456,36,0,0,0,458,459,457,0,374,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,406,407,408,409,410,411,412,413,414,415,405,0,460,0,483,484,0,0,0,491,0,
109,390,391,0,388,0,238,0,0,0,0,257,229,0,128,133,129,131,124,126,215,0,263,255,
264,512,511,0,86,89,0,87,0,91,107,63,62,0,60,161,215,233,157,263,235,169,170,0,
85,197,26,0,28,0,0,0,0,119,121,240,239,22,104,116,0,0,0,135,136,138,0,109,0,142,
0,0,0,0,0,0,36,0,324,325,326,0,0,328,0,0,0,0,296,292,120,294,290,288,0,167,222,0,
0,0,228,216,223,164,0,0,0,217,166,200,199,162,198,0,0,207,58,110,0,103,423,100,
96,386,387,253,0,394,249,251,381,0,0,0,0,241,245,0,0,0,0,0,0,0,0,0,0,0,0,0,0,490,
497,0,496,401,425,0,427,429,431,433,435,436,440,441,438,439,443,444,446,447,449,
450,451,404,403,482,479,0,481,0,0,0,378,0,385,31,0,379,0,0,258,134,130,132,0,217,
0,198,0,265,0,215,0,276,260,0,0,109,0,0,0,127,0,109,0,215,0,179,158,232,4,6,0,34,
0,0,139,122,0,0,217,109,150,0,0,0,303,0,0,0,0,0,323,327,0,0,0,0,286,36,23,262,
215,0,225,0,0,218,0,165,221,209,163,186,185,207,182,0,7,0,252,396,399,461,0,0,
242,0,246,466,0,0,0,0,0,470,473,0,0,0,0,0,453,493,0,0,480,478,0,0,389,392,380,
259,230,272,0,266,267,198,0,0,217,198,0,88,0,217,0,0,0,59,61,0,171,0,0,217,0,198,
0,0,0,137,140,113,143,152,151,0,0,146,0,0,0,0,0,0,0,0,0,0,0,0,0,303,329,0,0,24,
217,0,226,224,0,217,0,208,0,8,0,0,0,243,247,0,0,0,0,421,422,465,482,481,495,0,
498,417,492,494,0,271,269,275,274,0,268,198,0,113,0,66,64,65,172,178,175,0,177,
173,198,0,33,0,141,153,150,150,297,303,0,0,0,0,0,0,331,332,334,336,338,340,342,
344,347,352,355,358,362,364,371,372,0,36,300,309,0,0,0,0,0,0,0,0,0,0,330,283,211,
227,219,210,215,13,0,0,464,463,0,248,467,0,0,472,471,485,0,273,270,35,0,0,176,
174,285,106,0,149,148,0,0,303,0,368,0,0,365,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,366,304,0,311,0,0,0,319,0,0,0,0,0,0,302,0,217,12,14,0,0,0,0,486,105,0,0,
298,299,0,0,0,0,303,305,335,0,337,339,341,343,345,346,350,351,348,349,353,354,
356,357,359,360,361,0,310,312,313,0,321,320,0,315,0,0,0,220,212,15,0,0,0,0,154,
301,367,0,363,306,0,36,314,322,316,317,0,244,468,0,67,0,0,333,303,307,318,0,370,
369,308,469,0,0,0};static short Cyc_yydefgoto[144]={1032,50,682,877,51,52,286,53,
458,54,460,55,56,142,57,58,526,228,444,445,229,61,243,230,63,165,166,64,162,65,
264,265,125,126,127,266,231,425,473,474,475,66,67,752,753,754,68,476,69,449,70,
71,159,160,72,116,522,138,315,679,607,73,608,516,670,508,512,513,420,308,251,93,
94,547,466,548,330,331,332,232,301,302,609,431,289,462,290,291,292,293,294,760,
295,296,838,839,840,841,842,843,844,845,846,847,848,849,850,851,852,853,333,410,
411,334,335,336,297,199,397,200,532,201,202,203,204,205,206,207,208,209,210,211,
212,213,214,215,564,565,216,217,75,878,244,435};static short Cyc_yypact[1035]={
2688,- -32768,- -32768,- -32768,- -32768,- 4,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
3306,146,728,- -32768,87,47,- -32768,21,44,80,90,101,147,166,682,252,- -32768,-
-32768,266,- -32768,- -32768,- -32768,297,562,294,335,375,375,- -32768,- -32768,2553,
- -32768,637,660,- -32768,409,934,3306,3306,3306,- -32768,3306,- -32768,- -32768,593,
- -32768,20,3226,189,95,194,999,- -32768,- -32768,353,386,434,- -32768,87,418,-
-32768,912,347,- -32768,- -32768,- -32768,353,5799,- -32768,433,444,912,447,446,449,
- -32768,160,- -32768,- -32768,3454,3454,2553,2553,3454,468,5799,558,- -32768,195,
465,972,443,195,3676,5799,- -32768,- -32768,- -32768,2553,2822,2553,2822,- 36,-
-32768,491,512,- -32768,3146,- -32768,- -32768,- -32768,- -32768,3676,- -32768,- -32768,
353,- -32768,87,- -32768,1579,- -32768,3226,934,- -32768,3454,3380,4299,- -32768,189,
- -32768,- -32768,- -32768,515,536,- -32768,- -32768,- -32768,- -32768,22,999,3454,-
-32768,2822,- -32768,548,570,592,87,30,- -32768,912,65,3316,5935,606,5799,1167,613,
624,633,638,643,353,656,658,664,5976,5976,- -32768,2138,668,6068,6068,6068,-
-32768,- -32768,- -32768,50,- -32768,- -32768,- 60,634,588,609,688,684,245,134,- 56,
103,- -32768,865,6068,127,- 33,- -32768,666,3686,689,- -32768,- -32768,726,5799,353,
3686,716,161,3676,3750,3676,695,- -32768,63,63,- -32768,- -32768,119,730,284,353,-
-32768,- -32768,- -32768,- -32768,98,724,- -32768,- -32768,750,330,- -32768,748,-
-32768,751,- -32768,760,764,769,- -32768,972,1047,3226,- -32768,772,3676,- -32768,
657,784,177,789,234,796,3819,802,825,792,817,4395,3819,226,815,814,- -32768,-
-32768,829,1720,1720,934,1720,- -32768,- -32768,- -32768,835,- -32768,- -32768,-
-32768,264,- -32768,- -32768,848,813,117,863,- -32768,23,847,845,400,850,794,846,
3454,5799,- -32768,866,- -32768,- -32768,117,862,87,- -32768,5799,864,- -32768,861,
870,- -32768,195,5799,912,225,- -32768,- -32768,- -32768,868,869,2138,- -32768,3676,-
-32768,- -32768,3915,- -32768,895,5799,5799,5799,5799,5799,871,5799,3676,707,2138,
- -32768,- -32768,1861,872,357,5799,- -32768,- -32768,- -32768,5799,- -32768,6068,5799,
6068,6068,6068,6068,6068,6068,6068,6068,6068,6068,6068,6068,6068,6068,6068,6068,
6068,5799,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,5799,- -32768,195,- -32768,- -32768,4488,195,5799,- -32768,
877,353,- -32768,- -32768,876,874,912,- -32768,407,3316,878,3454,- -32768,880,886,-
-32768,3750,3750,3750,- -32768,- -32768,2984,4581,200,- -32768,350,- -32768,- -32768,
23,- -32768,- -32768,3454,- -32768,899,- -32768,889,- -32768,891,883,893,- -32768,2902,
- -32768,504,282,- -32768,- -32768,- -32768,3676,- -32768,- -32768,- -32768,2553,-
-32768,2553,911,904,901,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
762,5799,908,907,- -32768,900,530,353,87,915,5799,5799,916,925,5799,1015,2002,929,
- -32768,- -32768,- -32768,413,1007,- -32768,4677,5799,3819,2274,- -32768,- -32768,
3146,- -32768,- -32768,- -32768,3454,- -32768,- -32768,3676,930,240,- -32768,- -32768,
918,- -32768,23,931,3602,845,- -32768,- -32768,- -32768,- -32768,794,68,932,426,-
-32768,- -32768,2418,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,924,
- -32768,- -32768,- -32768,- -32768,3612,936,939,955,- -32768,- -32768,753,1047,364,
949,950,961,390,3676,391,957,281,959,969,5840,- -32768,- -32768,964,970,- -32768,
634,130,588,609,688,684,245,245,134,134,134,134,- 56,- 56,103,103,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,967,- -32768,97,3454,4203,- -32768,6135,-
-32768,- -32768,974,- -32768,91,983,- -32768,- -32768,- -32768,- -32768,973,845,978,
794,977,350,3454,3528,4770,- -32768,- -32768,63,990,353,715,989,98,3066,991,353,
3454,3380,4866,- -32768,504,- -32768,- -32768,- -32768,993,- -32768,1008,823,- -32768,
- -32768,657,5799,845,353,884,394,420,5799,875,427,996,4962,5055,498,- -32768,-
-32768,1005,1009,1003,477,- -32768,3226,- -32768,813,1012,3454,- -32768,1014,23,-
-32768,1001,- -32768,214,- -32768,- -32768,- -32768,- -32768,426,- -32768,1016,- -32768,
2553,- -32768,- -32768,- -32768,1017,307,1018,- -32768,4011,- -32768,- -32768,5799,
1116,5799,1167,1019,- -32768,- -32768,195,195,1017,1020,4203,- -32768,- -32768,5799,
5799,- -32768,- -32768,117,785,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
1021,- -32768,- -32768,794,117,1022,845,794,1011,- -32768,5799,845,484,1023,1024,-
-32768,- -32768,1025,- -32768,117,1026,845,1028,794,1029,5799,1045,- -32768,- -32768,
3676,- -32768,- -32768,- -32768,1044,87,529,3819,1046,1041,1270,1039,1049,3819,5799,
5148,571,5241,600,5334,875,- -32768,1052,1053,- -32768,845,150,- -32768,- -32768,
1031,845,3676,- -32768,646,- -32768,1048,1050,5799,- -32768,- -32768,1047,505,1056,
1063,- -32768,895,- -32768,1051,1059,- -32768,788,- -32768,- -32768,- -32768,- -32768,
4203,- -32768,- -32768,- -32768,- -32768,1062,- -32768,794,602,3676,1064,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,1069,- -32768,- -32768,794,516,- -32768,1076,
- -32768,1072,42,884,1160,875,1079,6109,1075,2274,6068,1073,- -32768,- 53,- -32768,
1108,1066,774,827,313,842,236,358,- -32768,- -32768,- -32768,- -32768,1111,6068,1861,
- -32768,- -32768,528,3819,541,5427,3819,555,5520,5613,621,1085,- -32768,- -32768,-
-32768,- -32768,1086,- -32768,1012,- -32768,1088,622,- -32768,- -32768,184,- -32768,-
-32768,3676,1183,- -32768,- -32768,- -32768,4107,- -32768,- -32768,- -32768,1091,1093,
- -32768,- -32768,- -32768,- -32768,3676,- -32768,- -32768,3819,1098,875,2138,- -32768,
3676,1099,- -32768,1438,6068,5799,6068,6068,6068,6068,6068,6068,6068,6068,6068,
6068,6068,6068,6068,6068,6068,6068,6068,5799,- -32768,- -32768,1106,- -32768,3819,
3819,559,- -32768,3819,3819,565,3819,572,5706,- -32768,23,845,- -32768,- -32768,558,
5799,1100,1103,- -32768,- -32768,1120,579,- -32768,- -32768,1110,1107,1113,6068,875,
- -32768,634,262,588,609,609,684,245,245,134,134,134,134,- 56,- 56,103,103,- -32768,
- -32768,- -32768,298,- -32768,- -32768,- -32768,3819,- -32768,- -32768,3819,- -32768,
3819,3819,590,- -32768,- -32768,- -32768,824,1109,3676,1115,- -32768,- -32768,1017,
325,- -32768,- -32768,5799,1438,- -32768,- -32768,- -32768,- -32768,3819,- -32768,-
-32768,1117,- -32768,1118,1119,- -32768,875,- -32768,- -32768,1121,- -32768,- -32768,-
-32768,- -32768,1231,1235,- -32768};static short Cyc_yypgoto[144]={- -32768,111,-
-32768,286,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 62,- -32768,-
98,31,- -32768,- -32768,0,625,- -32768,36,- 152,1132,5,- -32768,- -32768,- 136,- -32768,
105,1214,- 695,- -32768,- -32768,- -32768,994,985,223,421,- -32768,- -32768,612,-
-32768,- -32768,122,- -32768,- -32768,72,- 225,1185,- 446,14,- -32768,1096,- -32768,-
-32768,1209,- 474,- -32768,- -32768,580,- 119,- 58,- 123,- 464,314,586,595,- 418,- 471,-
112,- 399,- 126,- -32768,- 242,- 174,- 581,- 317,- -32768,938,- 172,196,- 151,- 161,- 278,
276,- -32768,- -32768,- -32768,- 67,- 275,- -32768,- 594,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,173,1037,- -32768,725,857,- -32768,247,632,
- -32768,- 169,- 291,- 155,- 351,- 325,- 361,903,- 354,- 344,- 263,- 337,- 335,- 5,429,521,
933,- -32768,- 365,- -32768,167,456,- 31,- 15,- 293,45};static short Cyc_yytable[6274]={
59,144,250,343,631,62,141,340,570,139,318,499,500,712,502,567,617,572,358,465,
309,59,344,307,448,549,62,306,573,574,262,631,323,366,135,536,60,590,537,668,911,
579,580,569,111,581,582,750,751,675,136,59,78,826,674,632,62,60,59,59,59,263,59,
62,62,62,141,62,59,148,430,257,405,62,367,299,135,423,287,380,298,912,560,381,
509,150,258,60,451,129,130,131,240,132,60,60,60,671,60,143,406,316,59,59,60,76,
588,62,62,665,591,575,576,577,578,442,112,893,632,59,59,59,59,798,62,62,62,62,59,
128,659,717,39,62,39,39,721,171,60,60,59,510,59,100,719,62,137,62,314,148,432,
612,172,531,99,60,60,60,60,143,531,- 147,118,59,60,543,101,544,62,419,364,288,612,
143,868,365,60,325,60,748,557,639,559,246,530,518,433,361,362,363,96,326,173,676,
38,468,95,102,434,60,298,41,38,434,432,443,432,103,398,41,399,400,401,235,236,
300,716,364,453,44,378,379,625,104,287,287,434,287,710,422,253,254,255,256,424,
382,453,505,903,437,433,268,433,383,384,429,625,511,805,96,364,402,809,38,434,
170,403,404,708,96,41,808,59,450,79,220,812,62,822,105,423,423,423,39,319,872,
820,549,224,436,418,477,438,757,225,434,59,59,350,59,106,62,62,143,62,287,233,
234,60,161,237,108,241,531,364,43,479,691,242,871,960,145,951,146,874,527,965,
288,288,147,288,507,60,60,427,60,47,48,49,415,428,305,300,374,375,198,891,249,96,
- 215,96,38,- 215,494,324,441,539,506,41,896,495,747,39,239,328,329,267,59,482,667,
501,483,62,528,700,400,401,376,377,249,1009,925,109,788,480,926,511,583,584,585,
875,504,364,697,364,96,250,549,288,38,1010,409,96,60,784,40,41,42,409,736,113,
919,920,662,364,402,447,795,796,440,701,404,428,736,241,783,626,1025,364,453,242,
110,422,422,422,653,1011,424,424,424,1030,616,241,1021,663,921,922,359,242,141,
453,429,711,16,17,18,453,624,787,630,454,421,419,426,171,455,74,511,59,114,59,
450,723,62,- 237,62,43,- 237,123,172,414,638,613,531,614,161,738,364,80,97,615,98,
562,998,364,477,788,59,267,692,927,725,62,115,60,724,60,96,928,929,74,59,741,538,
163,740,62,74,524,739,364,364,594,74,364,696,698,305,143,755,517,134,792,60,598,
492,74,364,671,59,656,678,143,364,62,167,168,60,97,800,364,793,773,164,419,756,
882,364,97,486,778,219,761,969,970,493,705,218,74,74,966,787,971,134,221,60,222,
74,247,633,223,634,788,972,973,74,74,74,74,96,238,932,96,643,359,597,978,979,968,
245,980,981,550,626,269,772,554,364,556,74,298,359,771,338,813,341,341,453,259,
814,453,511,616,601,568,767,355,356,364,74,43,341,341,341,167,364,627,97,628,97,
883,630,260,618,629,964,364,38,477,312,681,897,341,40,41,42,829,787,364,830,249,
592,477,933,471,477,974,975,976,977,313,364,59,728,908,38,935,62,320,38,249,40,
41,42,97,364,41,249,43,364,939,97,59,678,989,364,44,62,862,321,992,364,364,143,
47,48,49,994,60,454,38,664,19,20,1004,452,40,41,42,96,364,133,952,322,1022,1016,
74,865,60,892,364,249,364,452,369,730,368,484,646,647,339,249,650,961,654,962,
1024,345,944,949,249,364,950,660,74,81,370,252,346,749,135,341,947,38,119,120,
801,347,341,40,41,42,348,96,38,876,81,349,806,714,40,41,42,661,372,373,167,121,
122,311,351,471,352,818,407,472,97,957,353,47,48,49,360,782,287,341,412,341,341,
341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,38,84,189,85,86,427,
82,41,731,732,733,428,371,47,48,49,1019,354,909,413,191,83,84,249,85,86,417,1023,
41,604,605,606,87,421,287,446,931,249,88,439,89,90,59,38,520,521,91,62,774,40,41,
42,92,456,97,38,457,97,689,690,447,40,41,42,43,459,428,463,47,48,49,461,471,288,
511,750,751,469,60,467,47,48,49,758,759,764,766,341,478,452,802,803,452,888,889,
489,59,915,916,481,74,62,74,917,918,135,485,385,982,983,984,96,487,452,923,924,
38,854,488,452,490,644,40,41,42,496,789,288,497,1017,364,525,60,471,434,287,81,
900,901,498,503,47,48,49,1008,386,387,388,389,390,391,392,393,394,395,261,507,
267,514,515,467,519,811,523,551,552,553,529,43,533,534,74,396,541,366,542,341,
824,563,535,596,555,566,561,97,593,602,595,249,600,82,603,619,620,622,859,861,59,
864,623,867,621,62,586,635,169,84,636,85,86,637,640,41,641,642,587,87,831,881,
645,563,267,88,858,89,90,648,38,288,649,91,651,655,60,41,657,92,124,97,669,666,
683,672,677,44,687,611,686,688,151,152,153,47,48,49,693,694,341,174,175,154,155,
156,157,158,38,452,695,699,198,702,81,41,176,703,706,707,177,709,715,452,39,44,
452,718,178,179,180,181,720,47,48,49,182,722,937,183,729,941,943,734,745,737,744,
184,185,762,186,249,768,777,341,769,563,770,305,563,187,188,775,781,704,934,341,
74,938,785,790,810,873,794,797,804,807,815,816,817,819,359,821,38,84,189,85,86,
967,823,41,825,827,832,342,833,856,857,869,870,190,894,879,191,880,886,985,958,
192,884,467,193,885,887,194,195,890,902,196,197,996,174,175,895,898,899,904,1000,
907,913,910,914,930,945,81,946,948,953,828,955,987,988,956,97,990,991,959,993,
178,179,180,181,986,963,1001,467,1002,1003,1005,183,1033,1006,1007,1018,1034,999,
134,184,185,1020,186,1027,1028,1029,248,1031,727,735,107,470,187,188,746,464,149,
317,117,780,779,997,743,416,906,776,1012,341,685,1013,540,1014,1015,599,571,38,
84,189,85,86,855,563,41,0,0,341,342,558,0,0,0,0,190,1026,0,191,834,835,0,0,192,0,
0,193,0,0,194,195,0,81,196,197,0,0,0,0,0,0,0,0,0,0,0,467,0,0,0,0,791,0,0,0,0,0,0,
0,0,467,0,0,799,341,0,341,341,341,341,341,341,341,341,341,341,341,341,341,341,
341,341,341,0,0,0,82,0,0,0,0,0,0,0,0,0,0,0,0,0,0,169,84,0,85,86,0,0,41,0,0,0,87,
0,0,0,341,0,836,0,89,90,0,0,0,0,91,0,0,837,134,0,92,195,0,0,196,197,0,0,0,0,0,0,
467,0,0,0,0,0,0,0,0,0,0,0,0,0,0,467,0,0,0,341,0,2,3,4,77,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,758,759,21,174,175,270,0,271,272,273,274,275,276,277,278,22,81,
23,176,279,0,0,177,24,280,0,0,27,0,0,178,179,180,181,0,28,29,0,182,0,281,183,0,0,
0,0,0,32,33,184,185,34,186,0,0,0,0,0,35,36,37,467,187,188,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,282,84,189,85,86,39,40,41,42,43,283,140,- 303,0,0,0,0,190,0,
45,285,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,21,174,175,270,0,271,272,273,274,275,276,277,278,22,81,
23,176,279,0,0,177,24,280,0,0,27,0,0,178,179,180,181,0,28,29,0,182,0,281,183,0,0,
0,0,0,32,33,184,185,34,186,0,0,0,0,0,35,36,37,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,282,84,189,85,86,39,40,41,42,43,283,140,284,0,0,0,0,190,0,45,
285,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,21,174,175,270,0,271,272,273,274,275,276,277,278,22,81,23,
176,279,0,0,177,24,280,0,0,27,0,0,178,179,180,181,0,28,29,- 36,182,0,281,183,0,0,
0,0,0,32,33,184,185,34,186,0,0,0,0,0,35,36,37,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,282,84,189,85,86,39,40,41,42,43,283,140,0,0,0,0,0,190,0,45,285,
0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,174,175,270,0,271,272,273,274,275,276,277,278,22,81,23,176,
279,0,0,177,24,280,0,0,27,0,0,178,179,180,181,0,28,29,0,182,0,281,183,0,0,0,0,0,
32,33,184,185,34,186,0,0,0,0,0,35,36,37,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,282,84,189,85,86,39,40,41,42,43,283,140,0,0,0,0,0,190,0,45,285,0,0,0,
0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,174,175,0,0,0,0,0,0,0,0,0,0,22,81,23,176,0,0,0,177,24,0,0,0,27,0,
0,178,179,180,181,0,28,29,0,182,0,0,183,0,0,0,0,0,32,33,184,185,34,186,0,0,0,0,0,
35,36,37,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,39,
40,41,42,43,652,0,0,0,0,0,0,190,0,45,285,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,
197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,174,175,0,0,0,0,0,0,0,0,0,0,22,
81,0,176,0,0,0,177,0,0,0,0,27,0,0,178,179,180,181,0,28,29,0,182,0,0,183,0,0,0,0,
0,32,0,184,185,34,186,0,0,0,0,0,35,36,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,38,84,189,85,86,39,40,41,42,43,0,357,0,0,0,0,0,190,0,45,285,0,0,0,0,
192,0,0,193,0,0,194,195,0,0,196,197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,
174,175,0,0,0,0,0,0,0,0,0,0,22,81,0,176,0,0,0,177,0,0,0,0,27,0,0,178,179,180,181,
0,28,29,0,182,0,0,183,0,0,0,0,0,32,0,184,185,34,186,0,0,0,0,0,35,36,0,0,187,188,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,39,40,41,42,43,0,0,0,0,
0,0,0,190,0,45,285,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,- 11,1,0,2,3,4,5,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,
0,0,680,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,33,0,0,34,
0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,
39,40,41,42,43,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,
0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,
0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,39,40,
41,42,43,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,
24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,
0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,39,40,41,
42,43,0,0,0,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,1,0,2,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,
27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,
0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,38,0,0,0,0,39,40,41,
42,43,22,0,- 11,0,0,0,0,44,0,45,46,0,27,0,0,47,48,49,0,0,28,29,0,0,0,0,0,0,0,0,0,
0,32,0,0,0,34,0,0,0,0,0,0,35,36,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,303,0,0,38,0,0,0,0,39,40,41,42,43,305,0,22,0,0,0,0,447,0,45,46,0,0,428,27,
47,48,49,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,0,0,0,0,
0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,303,0,0,0,0,0,0,0,39,40,0,42,43,
305,0,22,0,0,0,0,427,0,45,46,0,0,428,27,47,48,49,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,
0,32,0,0,0,34,0,0,0,0,0,0,35,36,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,0,0,21,0,0,38,0,0,0,0,39,40,41,42,43,22,0,23,0,0,0,0,471,24,45,46,0,27,0,0,47,
48,49,0,0,28,29,- 36,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,35,36,37,2,3,4,77,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,0,42,43,22,
140,23,0,261,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,0,32,33,0,
0,34,0,0,0,0,0,0,35,36,37,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
21,0,0,0,0,0,0,0,39,40,0,42,43,22,140,0,0,0,0,0,0,24,45,46,81,27,0,0,0,0,0,0,0,
28,29,0,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,0,0,0,82,0,0,0,0,0,0,0,0,22,39,40,327,42,43,169,84,0,85,86,
0,27,41,0,45,46,87,0,0,28,29,0,88,0,89,90,0,328,329,0,91,32,0,0,0,34,92,0,0,0,0,
0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,303,0,0,304,0,0,0,
22,39,40,0,42,43,305,0,0,0,0,0,27,0,0,45,46,0,0,0,28,29,0,0,226,0,0,0,0,0,0,0,32,
0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,22,39,40,0,42,43,0,227,0,0,0,0,27,0,0,45,46,0,0,0,28,29,0,0,0,0,
0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
0,0,0,0,0,0,0,0,303,0,0,0,0,0,0,22,39,40,0,42,43,305,0,0,0,0,81,27,0,0,45,46,0,0,
0,28,29,0,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,0,0,0,0,82,0,673,0,0,0,0,0,0,22,39,40,684,42,43,169,84,0,
85,86,81,27,41,0,45,46,87,0,0,28,29,0,88,0,89,90,0,328,329,0,91,32,0,0,0,34,92,0,
0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,82,0,0,0,0,0,0,
0,0,22,39,40,408,42,43,169,84,0,85,86,0,27,41,0,45,46,87,0,0,28,29,0,88,0,89,90,
0,0,0,0,91,32,0,0,0,34,92,0,0,0,0,0,35,36,0,0,0,0,0,0,0,0,0,0,0,0,174,175,270,0,
271,272,273,274,275,276,277,278,0,81,0,176,279,0,39,177,0,280,43,0,0,0,0,178,179,
180,181,0,45,46,0,182,0,281,183,0,0,0,0,0,0,0,184,185,0,186,0,0,0,0,0,0,0,0,0,
187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,282,84,189,85,86,0,0,41,0,0,
283,140,0,0,174,175,0,190,0,0,191,545,0,0,0,192,0,81,193,176,0,194,195,177,0,196,
197,0,0,0,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,185,0,186,0,0,
0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,
0,0,41,0,0,0,342,546,0,174,175,0,190,0,0,191,0,328,329,0,192,0,81,193,176,0,194,
195,177,0,196,197,0,0,0,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,
185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,189,85,86,0,0,41,0,0,0,342,786,0,174,175,0,190,0,0,191,0,328,329,0,192,0,81,
193,176,0,194,195,177,0,196,197,0,0,0,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,
0,0,0,0,0,184,185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,38,84,189,85,86,0,0,41,0,0,0,342,954,0,174,175,0,190,0,0,191,0,328,
329,0,192,0,81,193,176,0,194,195,177,0,196,197,0,0,0,0,178,179,180,181,0,0,0,0,
182,0,0,183,0,0,0,0,0,0,0,184,185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,0,0,41,0,0,0,342,0,0,174,175,0,190,0,
0,191,0,328,329,0,192,0,81,193,176,0,194,195,177,0,196,197,0,0,0,0,178,179,180,
181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,0,0,41,0,0,0,0,0,0,174,
175,0,190,0,0,191,0,0,0,310,192,0,81,193,176,0,194,195,177,0,196,197,0,0,0,0,178,
179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,185,0,186,0,0,0,0,0,0,0,0,0,
187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,0,0,41,0,0,491,
174,175,0,0,0,0,190,0,0,191,0,0,0,81,192,176,0,193,0,177,194,195,0,0,196,197,0,
178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,185,0,186,0,0,0,0,0,0,0,0,
0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,0,0,41,0,0,0,
174,175,0,0,0,0,190,589,0,191,0,0,0,81,192,176,0,193,0,177,194,195,0,0,196,197,0,
178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,185,0,186,0,0,0,0,0,0,0,0,
0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,0,0,41,0,0,0,
0,0,0,174,175,0,190,0,0,191,0,0,0,610,192,0,81,193,176,0,194,195,177,0,196,197,0,
0,0,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,185,0,186,0,0,0,0,0,
0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,0,0,41,
0,0,0,174,175,0,0,0,0,190,658,0,191,0,0,0,81,192,176,0,193,0,177,194,195,0,0,196,
197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,185,0,186,0,0,0,0,0,
0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,0,0,41,
0,0,0,0,0,0,174,175,0,190,0,0,191,0,0,0,726,192,0,81,193,176,0,194,195,177,0,196,
197,0,0,0,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,185,0,186,0,0,
0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,189,85,86,
0,0,41,0,0,0,0,0,0,174,175,0,190,0,0,191,0,0,0,742,192,0,81,193,176,0,194,195,
177,0,196,197,0,0,0,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,185,
0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,
189,85,86,0,0,41,0,0,763,174,175,0,0,0,0,190,0,0,191,0,0,0,81,192,176,0,193,0,
177,194,195,0,0,196,197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,
185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,189,85,86,0,0,41,0,0,765,174,175,0,0,0,0,190,0,0,191,0,0,0,81,192,176,0,193,0,
177,194,195,0,0,196,197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,
185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,189,85,86,0,0,41,0,0,0,174,175,0,0,0,0,190,860,0,191,0,0,0,81,192,176,0,193,0,
177,194,195,0,0,196,197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,
185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,189,85,86,0,0,41,0,0,0,174,175,0,0,0,0,190,863,0,191,0,0,0,81,192,176,0,193,0,
177,194,195,0,0,196,197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,
185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,189,85,86,0,0,41,0,0,866,174,175,0,0,0,0,190,0,0,191,0,0,0,81,192,176,0,193,0,
177,194,195,0,0,196,197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,
185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,189,85,86,0,0,41,0,0,0,174,175,0,0,0,0,190,936,0,191,0,0,0,81,192,176,0,193,0,
177,194,195,0,0,196,197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,
185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,189,85,86,0,0,41,0,0,0,174,175,0,0,0,0,190,940,0,191,0,0,0,81,192,176,0,193,0,
177,194,195,0,0,196,197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,
185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,189,85,86,0,0,41,0,0,0,174,175,0,0,0,0,190,942,0,191,0,0,0,81,192,176,0,193,0,
177,194,195,0,0,196,197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,0,0,0,0,0,184,
185,0,186,0,0,0,0,0,0,0,0,0,187,188,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,189,85,86,0,0,41,0,0,0,174,175,0,0,0,0,190,995,0,191,0,0,0,81,192,176,0,193,0,
177,194,195,0,0,196,197,0,178,179,180,181,0,0,0,0,182,0,0,183,0,0,174,175,0,0,0,
184,185,0,186,0,0,0,0,81,0,0,0,0,187,188,0,0,0,0,0,0,0,178,179,180,181,0,0,0,0,0,
0,0,183,0,0,38,84,189,85,86,184,185,41,186,0,0,0,0,0,0,0,0,190,187,188,191,0,0,0,
0,192,0,0,193,0,0,194,195,0,0,196,197,0,0,0,0,38,84,189,85,86,0,0,41,0,0,0,704,0,
174,175,0,0,190,0,0,191,0,0,0,0,192,81,0,193,0,0,194,195,0,0,196,197,0,0,0,178,
179,180,181,0,0,0,0,0,0,0,183,0,0,174,175,0,0,0,184,185,0,186,0,0,0,0,81,0,0,0,0,
187,188,0,0,0,0,0,0,0,178,179,180,181,0,0,0,0,0,0,0,183,0,0,38,84,189,85,86,184,
185,41,186,0,0,0,0,0,0,0,0,337,187,188,191,0,0,0,0,192,0,0,193,0,0,194,195,0,0,
196,197,0,0,0,0,38,84,189,85,86,0,0,41,0,0,174,175,0,0,0,0,0,354,0,0,191,0,0,81,
0,192,0,0,193,0,0,194,195,0,0,196,197,178,179,180,181,0,0,0,0,0,0,0,183,0,0,174,
175,0,0,0,184,185,0,186,0,0,0,0,81,0,0,0,0,187,188,0,0,0,0,0,0,0,178,179,180,181,
0,0,0,0,0,0,0,183,81,0,38,84,189,85,86,184,185,41,186,0,0,0,0,0,0,0,0,190,187,
188,191,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,0,0,0,0,38,84,189,85,86,0,0,
41,0,0,0,82,0,0,0,0,0,905,0,0,191,0,0,713,0,192,169,84,193,85,86,194,195,41,0,
196,197,87,0,0,0,0,0,88,0,89,90,0,0,0,0,91,0,0,0,0,0,92};static short Cyc_yycheck[
6274]={0,68,114,177,450,0,68,176,369,67,161,286,287,594,289,366,434,371,190,261,
146,21,177,146,249,342,21,146,372,373,128,477,168,93,65,328,0,402,329,510,93,378,
379,368,44,380,381,5,6,523,65,51,21,748,518,454,51,21,58,59,60,128,62,58,59,60,
128,62,68,69,231,107,105,68,134,142,107,229,140,135,142,134,357,139,61,71,122,51,
249,58,59,60,107,62,58,59,60,515,62,68,133,159,102,103,68,109,399,102,103,508,
403,374,375,376,377,17,44,812,517,119,120,121,122,704,119,120,121,122,128,57,495,
602,112,128,112,112,610,107,102,103,140,118,142,122,608,140,126,142,126,149,87,
429,122,322,107,119,120,121,122,128,329,119,51,163,128,337,122,339,163,227,120,
140,450,142,768,125,140,112,142,643,352,472,354,111,320,308,123,192,193,194,23,
126,87,125,107,262,23,117,135,163,262,114,107,135,87,107,87,117,213,114,83,84,85,
102,103,143,125,120,249,124,86,87,447,122,286,287,135,289,131,229,119,120,121,
122,229,132,267,120,832,120,123,136,123,140,141,231,471,305,722,82,120,124,726,
107,135,82,129,130,128,92,114,725,262,249,118,92,730,262,742,122,422,423,424,112,
163,125,740,594,118,234,119,267,237,648,124,135,286,287,183,289,124,286,287,262,
289,357,100,101,262,122,104,49,107,472,120,116,129,549,113,773,904,122,128,124,
778,315,910,286,287,130,289,107,286,287,124,289,132,133,134,224,130,117,260,88,
89,88,810,114,171,125,173,107,128,117,171,240,121,302,114,823,124,642,112,106,
129,130,133,357,124,119,288,127,357,318,83,84,85,122,123,146,964,135,106,690,269,
139,434,382,383,384,779,117,120,555,120,218,498,704,357,107,128,218,225,357,687,
113,114,115,225,624,106,88,89,501,120,124,124,700,701,125,129,130,130,638,107,
108,447,1011,120,450,113,124,422,423,424,487,128,422,423,424,1024,431,107,108,
501,122,123,190,113,501,471,427,593,17,18,19,477,447,690,449,120,228,510,230,107,
125,0,515,458,124,460,447,613,458,117,460,116,120,59,122,223,471,122,642,124,122,
627,120,22,23,130,25,125,947,120,471,803,487,265,125,132,614,487,118,458,614,460,
330,140,141,44,501,628,330,118,628,501,51,312,628,120,120,407,57,120,125,125,117,
487,125,120,65,696,487,117,278,70,120,946,529,117,526,501,120,529,79,118,501,82,
708,120,696,665,109,602,125,788,120,92,273,673,107,125,914,915,279,561,124,102,
103,911,803,916,107,117,529,120,111,125,458,121,460,889,917,918,119,120,121,122,
412,112,856,415,478,337,412,923,924,913,124,925,926,345,624,138,662,349,120,351,
143,662,354,125,174,120,176,177,638,117,125,641,669,612,417,367,117,187,188,120,
163,116,192,193,194,168,120,122,171,124,173,125,631,120,437,130,910,120,107,624,
124,529,125,213,113,114,115,117,889,120,120,427,404,638,125,124,641,919,920,921,
922,124,120,662,618,836,107,125,662,120,107,447,113,114,115,218,120,114,454,116,
120,125,225,682,678,125,120,124,682,117,119,125,120,120,662,132,133,134,125,662,
120,107,505,20,21,125,249,113,114,115,542,120,118,884,121,1007,125,260,117,682,
117,120,498,120,267,136,620,92,271,481,482,124,508,485,905,487,907,1011,124,117,
117,517,120,120,496,288,38,137,115,124,644,781,322,875,107,117,118,711,124,329,
113,114,115,124,596,107,119,38,124,723,596,113,114,115,497,90,91,320,117,118,147,
124,124,124,738,118,128,330,899,124,132,133,134,124,682,856,366,107,368,369,370,
371,372,373,374,375,376,377,378,379,380,381,382,383,384,107,108,109,110,111,124,
92,114,107,108,109,130,138,132,133,134,1002,124,837,107,127,107,108,614,110,111,
124,1010,114,422,423,424,118,624,910,125,855,628,124,123,126,127,856,107,64,65,
132,856,666,113,114,115,138,119,412,107,119,415,119,120,124,113,114,115,116,119,
130,112,132,133,134,121,124,856,946,5,6,119,856,261,132,133,134,22,23,652,653,
472,118,447,119,120,450,119,120,117,910,137,138,124,458,910,460,90,91,950,124,56,
927,928,929,758,124,471,86,87,107,758,107,477,117,479,113,114,115,124,693,910,
128,119,120,313,910,124,135,1011,38,829,830,124,119,132,133,134,963,94,95,96,97,
98,99,100,101,102,103,121,107,748,125,128,342,125,729,131,346,347,348,119,116,
119,123,529,121,119,93,120,561,744,360,123,120,124,364,125,542,122,120,125,779,
125,92,119,107,118,125,762,763,1011,765,120,767,124,1011,385,107,107,108,117,110,
111,123,117,114,120,128,397,118,755,785,118,402,812,124,761,126,127,124,107,1011,
118,132,30,117,1011,114,42,138,117,596,135,124,131,125,125,124,120,428,125,107,
64,65,66,132,133,134,120,120,642,25,26,75,76,77,78,79,107,624,120,125,836,125,38,
114,40,119,125,120,44,125,119,638,112,124,641,125,52,53,54,55,125,132,133,134,60,
131,862,63,121,865,866,125,107,125,124,71,72,124,74,899,118,123,696,117,495,125,
117,498,84,85,119,118,118,860,708,682,863,122,25,131,112,125,125,125,125,125,125,
125,125,905,125,107,108,109,110,111,912,131,114,117,119,118,118,125,128,119,117,
117,124,108,125,127,125,125,930,902,132,124,549,135,120,125,138,139,125,28,142,
143,944,25,26,125,119,124,118,951,124,92,128,136,92,119,38,120,119,25,753,119,
935,936,120,758,939,940,119,942,52,53,54,55,117,125,125,594,124,108,119,63,0,125,
120,125,0,950,781,71,72,125,74,125,125,125,113,125,615,623,35,265,84,85,641,260,
70,160,48,678,673,946,629,225,834,669,989,837,542,992,331,994,995,415,370,107,
108,109,110,111,758,648,114,- 1,- 1,855,118,353,- 1,- 1,- 1,- 1,124,1016,- 1,127,25,26,
- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,38,142,143,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
690,- 1,- 1,- 1,- 1,695,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,704,- 1,- 1,707,911,- 1,913,914,915,916,
917,918,919,920,921,922,923,924,925,926,927,928,929,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,- 1,110,111,- 1,- 1,114,- 1,- 1,- 1,118,- 1,- 1,- 1,963,
- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,950,- 1,138,139,- 1,- 1,142,143,- 1,- 1,-
1,- 1,- 1,- 1,788,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,803,- 1,- 1,- 1,1010,- 1,3,
4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,
33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,
60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,69,70,71,72,73,74,- 1,- 1,- 1,- 1,- 1,80,81,82,889,84,85,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,
111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,-
1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,
- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,69,70,71,72,73,
74,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,-
1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,3,4,5,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,
35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,
62,63,- 1,- 1,- 1,- 1,- 1,69,70,71,72,73,74,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,
113,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,
- 1,- 1,138,139,- 1,- 1,142,143,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,
- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,69,70,71,72,73,74,- 1,- 1,
- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,124,- 1,
126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,63,- 1,
- 1,- 1,- 1,- 1,69,70,71,72,73,74,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,
116,117,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,
- 1,- 1,142,143,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,
58,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,69,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,84,
85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,
110,111,112,113,114,115,116,- 1,118,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,
- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,
- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,69,- 1,71,72,73,74,- 1,- 1,
- 1,- 1,- 1,80,81,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,126,
127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,0,1,- 1,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,67,68,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,112,113,114,115,
116,- 1,- 1,119,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,133,134,0,1,- 1,3,4,5,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,67,68,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,112,113,
114,115,116,- 1,- 1,119,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,133,134,0,1,- 1,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,
59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,
112,113,114,115,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,133,134,
1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,
- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,
- 1,112,113,114,115,116,37,- 1,119,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,49,- 1,- 1,132,133,
134,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,
80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,104,- 1,- 1,
107,- 1,- 1,- 1,- 1,112,113,114,115,116,117,- 1,37,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,
130,49,132,133,134,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,
73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,- 1,115,116,117,- 1,37,- 1,- 1,- 1,- 1,
124,- 1,126,127,- 1,- 1,130,49,132,133,134,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,107,- 1,- 1,- 1,- 1,112,113,114,115,116,37,- 1,39,
- 1,- 1,- 1,- 1,124,45,126,127,- 1,49,- 1,- 1,132,133,134,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,- 1,115,116,37,118,
39,- 1,121,- 1,- 1,- 1,45,126,127,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,- 1,115,116,37,118,
- 1,- 1,- 1,- 1,- 1,- 1,45,126,127,38,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,104,115,116,
107,108,- 1,110,111,- 1,49,114,- 1,126,127,118,- 1,- 1,57,58,- 1,124,- 1,126,127,- 1,129,
130,- 1,132,69,- 1,- 1,- 1,73,138,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,107,- 1,- 1,- 1,37,112,113,- 1,115,116,
117,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,126,127,- 1,- 1,- 1,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,- 1,118,- 1,- 1,-
1,- 1,49,- 1,- 1,126,127,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,
73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,117,- 1,- 1,- 1,- 1,38,49,- 1,
- 1,126,127,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,
- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,
104,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,104,115,116,107,108,- 1,110,111,38,49,114,- 1,126,
127,118,- 1,- 1,57,58,- 1,124,- 1,126,127,- 1,129,130,- 1,132,69,- 1,- 1,- 1,73,138,- 1,- 1,
- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,104,115,116,107,108,- 1,110,111,- 1,49,114,- 1,126,
127,118,- 1,- 1,57,58,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,69,- 1,- 1,- 1,73,138,- 1,- 1,-
1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,
35,36,- 1,38,- 1,40,41,- 1,112,44,- 1,46,116,- 1,- 1,- 1,- 1,52,53,54,55,- 1,126,127,- 1,
60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,
- 1,- 1,114,- 1,- 1,117,118,- 1,- 1,25,26,- 1,124,- 1,- 1,127,32,- 1,- 1,- 1,132,- 1,38,135,
40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,-
1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,
- 1,118,119,- 1,25,26,- 1,124,- 1,- 1,127,- 1,129,130,- 1,132,- 1,38,135,40,- 1,138,139,
44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,118,119,- 1,
25,26,- 1,124,- 1,- 1,127,- 1,129,130,- 1,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,-
1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,118,119,- 1,25,26,- 1,124,- 1,
- 1,127,- 1,129,130,- 1,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,
54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,118,- 1,- 1,25,26,- 1,124,- 1,- 1,127,- 1,129,
130,- 1,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,-
1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,
85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,
110,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,124,- 1,- 1,127,- 1,- 1,- 1,131,132,- 1,
38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,
63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,
- 1,- 1,117,25,26,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,
139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,
72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,
124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,
108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,124,- 1,- 1,127,- 1,- 1,- 1,131,
132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,
- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,
138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,-
1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,25,
26,- 1,124,- 1,- 1,127,- 1,- 1,- 1,131,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,
- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,124,- 1,- 1,127,
- 1,- 1,- 1,131,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,-
1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,
109,110,111,- 1,- 1,114,- 1,- 1,117,25,26,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,38,132,
40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,
117,25,26,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,-
1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,
74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,
125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,
- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,
109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,132,
40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,
117,25,26,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,-
1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,
74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,
125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,
- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,
109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,132,
40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,
- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,-
1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,
74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,
125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,
- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,25,26,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,
- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,107,108,
109,110,111,71,72,114,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,84,85,127,- 1,- 1,- 1,- 1,132,-
1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,-
1,- 1,- 1,118,- 1,25,26,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,- 1,132,38,- 1,135,- 1,- 1,138,139,
- 1,- 1,142,143,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,25,26,- 1,- 1,- 1,
71,72,- 1,74,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,107,108,109,110,111,71,72,114,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,124,84,85,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,- 1,- 1,- 1,
- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,
38,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
63,- 1,- 1,25,26,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,38,- 1,107,108,109,110,111,71,72,114,
74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,84,85,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,
- 1,- 1,142,143,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,
- 1,124,- 1,- 1,127,- 1,- 1,104,- 1,132,107,108,135,110,111,138,139,114,- 1,142,143,118,
- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,- 1,- 1,- 1,138};char Cyc_Yyimpossible[
17]="\000\000\000\000Yyimpossible\000";char Cyc_Yystack_overflow[21]="\000\000\000\000Yystack_overflow\000";
void Cyc_yyerror(struct _dyneither_ptr);int Cyc_yylex(struct Cyc_Lexing_lexbuf*);
static int Cyc_yychar=(int)'\000';union Cyc_YYSTYPE Cyc_yylval={.YYINITIALSVAL={60,0}};
static int Cyc_yynerrs=0;struct _tuple24{struct Cyc_Position_Segment*f1;struct
_tuple1*f2;int f3;};struct _tuple25{struct _dyneither_ptr f1;void*f2;};static char
_tmp465[8]="stdcall";static char _tmp466[6]="cdecl";static char _tmp467[9]="fastcall";
static char _tmp468[9]="noreturn";static char _tmp469[6]="const";static char _tmp46A[8]="aligned";
static char _tmp46B[7]="packed";static char _tmp46C[7]="shared";static char _tmp46D[7]="unused";
static char _tmp46E[5]="weak";static char _tmp46F[10]="dllimport";static char _tmp470[
10]="dllexport";static char _tmp471[23]="no_instrument_function";static char _tmp472[
12]="constructor";static char _tmp473[11]="destructor";static char _tmp474[22]="no_check_memory_usage";
static char _tmp475[5]="pure";struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmp8E3(
unsigned int*_tmp8E2,unsigned int*_tmp8E1,short**_tmp8E0){for(*_tmp8E2=0;*
_tmp8E2 < *_tmp8E1;(*_tmp8E2)++){(*_tmp8E0)[*_tmp8E2]=(short)0;}}static void
_tmp8E8(unsigned int*_tmp8E7,unsigned int*_tmp8E6,union Cyc_YYSTYPE**_tmp8E5){
for(*_tmp8E7=0;*_tmp8E7 < *_tmp8E6;(*_tmp8E7)++){(*_tmp8E5)[*_tmp8E7]=Cyc_yylval;}}
static void _tmp8ED(unsigned int*_tmp8EC,unsigned int*_tmp8EB,struct Cyc_Yyltype**
_tmp8EA){for(*_tmp8EC=0;*_tmp8EC < *_tmp8EB;(*_tmp8EC)++){(*_tmp8EA)[*_tmp8EC]=
Cyc_yynewloc();}}static void _tmpDD2(unsigned int*_tmpDD1,unsigned int*_tmpDD0,
char**_tmpDCE){for(*_tmpDD1=0;*_tmpDD1 < *_tmpDD0;(*_tmpDD1)++){(*_tmpDCE)[*
_tmpDD1]='\000';}}int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){struct
_RegionHandle _tmp3D0=_new_region("yyregion");struct _RegionHandle*yyregion=&
_tmp3D0;_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;
int yyssp_offset;unsigned int _tmp8E2;unsigned int _tmp8E1;short*_tmp8E0;
unsigned int _tmp8DF;short*yyss=(short*)((_tmp8DF=10000,((_tmp8E0=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmp8DF)),((((_tmp8E1=_tmp8DF,
_tmp8E3(& _tmp8E2,& _tmp8E1,& _tmp8E0))),_tmp8E0))))));int yyvsp_offset;unsigned int
_tmp8E7;unsigned int _tmp8E6;union Cyc_YYSTYPE*_tmp8E5;unsigned int _tmp8E4;union
Cyc_YYSTYPE*yyvs=(union Cyc_YYSTYPE*)((_tmp8E4=10000,((_tmp8E5=(union Cyc_YYSTYPE*)
_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp8E4)),((((
_tmp8E6=_tmp8E4,_tmp8E8(& _tmp8E7,& _tmp8E6,& _tmp8E5))),_tmp8E5))))));int
yylsp_offset;unsigned int _tmp8EC;unsigned int _tmp8EB;struct Cyc_Yyltype*_tmp8EA;
unsigned int _tmp8E9;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)((_tmp8E9=10000,((
_tmp8EA=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct
Cyc_Yyltype),_tmp8E9)),((((_tmp8EB=_tmp8E9,_tmp8ED(& _tmp8EC,& _tmp8EB,& _tmp8EA))),
_tmp8EA))))));int yystacksize=10000;union Cyc_YYSTYPE yyval=Cyc_yylval;int yylen;
yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=
0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){{const char*_tmp8EE;Cyc_yyerror(((_tmp8EE="parser stack overflow",
_tag_dyneither(_tmp8EE,sizeof(char),22))));}(int)_throw((void*)Cyc_Yystack_overflow);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1035,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 371?(int)Cyc_yytranslate[_check_known_subscript_notnull(372,Cyc_yychar)]:
288;}yyn +=yychar1;if((yyn < 0  || yyn > 6273) || Cyc_yycheck[
_check_known_subscript_notnull(6274,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6274,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1034){int
_tmp3D2=0;_npop_handler(0);return _tmp3D2;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)-- yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1035,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(513,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL203: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL204: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp3D3;_push_handler(& _tmp3D3);{int _tmp3D5=0;if(setjmp(_tmp3D3.handler))_tmp3D5=
1;if(!_tmp3D5){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp3D4=(void*)_exn_thrown;void*
_tmp3D7=_tmp3D4;_LL207: if(*((void**)_tmp3D7)!= Cyc_Core_Failure)goto _LL209;
_LL208: x=0;goto _LL206;_LL209:;_LL20A:(void)_throw(_tmp3D7);_LL206:;}}}{struct
_handler_cons _tmp3D8;_push_handler(& _tmp3D8);{int _tmp3DA=0;if(setjmp(_tmp3D8.handler))
_tmp3DA=1;if(!_tmp3DA){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);;_pop_handler();}else{void*_tmp3D9=(void*)_exn_thrown;void*
_tmp3DC=_tmp3D9;_LL20C: if(*((void**)_tmp3DC)!= Cyc_Core_Failure)goto _LL20E;
_LL20D: y=0;goto _LL20B;_LL20E:;_LL20F:(void)_throw(_tmp3DC);_LL20B:;}}}yyval=Cyc_YY18(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
x,y));break;}case 3: _LL205:{struct Cyc_Absyn_Decl*_tmp8F8;struct Cyc_Absyn_Using_d_struct
_tmp8F7;struct Cyc_Absyn_Using_d_struct*_tmp8F6;struct Cyc_List_List*_tmp8F5;yyval=
Cyc_YY18(((_tmp8F5=_cycalloc(sizeof(*_tmp8F5)),((_tmp8F5->hd=((_tmp8F8=_cycalloc(
sizeof(*_tmp8F8)),((_tmp8F8->r=(void*)((_tmp8F6=_cycalloc(sizeof(*_tmp8F6)),((
_tmp8F6[0]=((_tmp8F7.tag=11,((_tmp8F7.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp8F7.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp8F7)))))),_tmp8F6)))),((
_tmp8F8->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmp8F8)))))),((_tmp8F5->tl=0,_tmp8F5)))))));}Cyc_Lex_leave_using();
break;case 4: _LL210:{struct Cyc_Absyn_Decl*_tmp902;struct Cyc_Absyn_Using_d_struct
_tmp901;struct Cyc_Absyn_Using_d_struct*_tmp900;struct Cyc_List_List*_tmp8FF;yyval=
Cyc_YY18(((_tmp8FF=_cycalloc(sizeof(*_tmp8FF)),((_tmp8FF->hd=((_tmp902=_cycalloc(
sizeof(*_tmp902)),((_tmp902->r=(void*)((_tmp900=_cycalloc(sizeof(*_tmp900)),((
_tmp900[0]=((_tmp901.tag=11,((_tmp901.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmp901.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp901)))))),
_tmp900)))),((_tmp902->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp902)))))),((
_tmp8FF->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp8FF)))))));}break;case 5: _LL211:{struct Cyc_Absyn_Decl*_tmp911;
struct Cyc_Absyn_Namespace_d_struct _tmp910;struct _dyneither_ptr*_tmp90F;struct Cyc_Absyn_Namespace_d_struct*
_tmp90E;struct Cyc_List_List*_tmp90D;yyval=Cyc_YY18(((_tmp90D=_cycalloc(sizeof(*
_tmp90D)),((_tmp90D->hd=((_tmp911=_cycalloc(sizeof(*_tmp911)),((_tmp911->r=(void*)((
_tmp90E=_cycalloc(sizeof(*_tmp90E)),((_tmp90E[0]=((_tmp910.tag=10,((_tmp910.f1=((
_tmp90F=_cycalloc(sizeof(*_tmp90F)),((_tmp90F[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp90F)))),((_tmp910.f2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp910)))))),
_tmp90E)))),((_tmp911->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmp911)))))),((
_tmp90D->tl=0,_tmp90D)))))));}Cyc_Lex_leave_namespace();break;case 6: _LL212: {
struct _dyneither_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp3EA;_push_handler(& _tmp3EA);{int _tmp3EC=0;if(setjmp(_tmp3EA.handler))
_tmp3EC=1;if(!_tmp3EC){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp3EB=(void*)_exn_thrown;void*_tmp3EE=_tmp3EB;_LL215: if(*((void**)_tmp3EE)
!= Cyc_Core_Failure)goto _LL217;_LL216:{const char*_tmp912;nspace=((_tmp912="",
_tag_dyneither(_tmp912,sizeof(char),1)));}goto _LL214;_LL217:;_LL218:(void)_throw(
_tmp3EE);_LL214:;}}}{struct _handler_cons _tmp3F0;_push_handler(& _tmp3F0);{int
_tmp3F2=0;if(setjmp(_tmp3F0.handler))_tmp3F2=1;if(!_tmp3F2){x=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}
else{void*_tmp3F1=(void*)_exn_thrown;void*_tmp3F4=_tmp3F1;_LL21A: if(*((void**)
_tmp3F4)!= Cyc_Core_Failure)goto _LL21C;_LL21B: x=0;goto _LL219;_LL21C:;_LL21D:(
void)_throw(_tmp3F4);_LL219:;}}}{struct _handler_cons _tmp3F5;_push_handler(&
_tmp3F5);{int _tmp3F7=0;if(setjmp(_tmp3F5.handler))_tmp3F7=1;if(!_tmp3F7){y=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp3F6=(void*)_exn_thrown;void*_tmp3F9=_tmp3F6;_LL21F: if(*((void**)_tmp3F9)
!= Cyc_Core_Failure)goto _LL221;_LL220: y=0;goto _LL21E;_LL221:;_LL222:(void)_throw(
_tmp3F9);_LL21E:;}}}{struct Cyc_Absyn_Decl*_tmp921;struct Cyc_Absyn_Namespace_d_struct
_tmp920;struct _dyneither_ptr*_tmp91F;struct Cyc_Absyn_Namespace_d_struct*_tmp91E;
struct Cyc_List_List*_tmp91D;yyval=Cyc_YY18(((_tmp91D=_cycalloc(sizeof(*_tmp91D)),((
_tmp91D->hd=((_tmp921=_cycalloc(sizeof(*_tmp921)),((_tmp921->r=(void*)((_tmp91E=
_cycalloc(sizeof(*_tmp91E)),((_tmp91E[0]=((_tmp920.tag=10,((_tmp920.f1=((_tmp91F=
_cycalloc(sizeof(*_tmp91F)),((_tmp91F[0]=nspace,_tmp91F)))),((_tmp920.f2=x,
_tmp920)))))),_tmp91E)))),((_tmp921->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp921)))))),((
_tmp91D->tl=y,_tmp91D)))))));}break;}case 7: _LL213:{const char*_tmp922;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),((_tmp922="C",_tag_dyneither(_tmp922,sizeof(char),2))))
== 0){struct Cyc_Absyn_Decl*_tmp92C;struct Cyc_Absyn_ExternC_d_struct _tmp92B;
struct Cyc_Absyn_ExternC_d_struct*_tmp92A;struct Cyc_List_List*_tmp929;yyval=Cyc_YY18(((
_tmp929=_cycalloc(sizeof(*_tmp929)),((_tmp929->hd=((_tmp92C=_cycalloc(sizeof(*
_tmp92C)),((_tmp92C->r=(void*)((_tmp92A=_cycalloc(sizeof(*_tmp92A)),((_tmp92A[0]=((
_tmp92B.tag=12,((_tmp92B.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),_tmp92B)))),_tmp92A)))),((_tmp92C->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmp92C)))))),((
_tmp929->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp929)))))));}else{{const char*_tmp92D;if(Cyc_strcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),((_tmp92D="C include",_tag_dyneither(_tmp92D,sizeof(char),10))))
!= 0){const char*_tmp92E;Cyc_Parse_err(((_tmp92E="expecting \"C\" or \"C include\"",
_tag_dyneither(_tmp92E,sizeof(char),29))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));}}{struct Cyc_Absyn_Decl*
_tmp938;struct Cyc_Absyn_ExternCinclude_d_struct _tmp937;struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp936;struct Cyc_List_List*_tmp935;yyval=Cyc_YY18(((_tmp935=_cycalloc(sizeof(*
_tmp935)),((_tmp935->hd=((_tmp938=_cycalloc(sizeof(*_tmp938)),((_tmp938->r=(void*)((
_tmp936=_cycalloc(sizeof(*_tmp936)),((_tmp936[0]=((_tmp937.tag=13,((_tmp937.f1=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmp937.f2=0,_tmp937)))))),_tmp936)))),((_tmp938->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp938)))))),((
_tmp935->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp935)))))));}}}break;case 8: _LL223:{const char*_tmp939;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]),((_tmp939="C include",_tag_dyneither(_tmp939,sizeof(
char),10))))!= 0){const char*_tmp93A;Cyc_Parse_err(((_tmp93A="expecting \"C include\"",
_tag_dyneither(_tmp93A,sizeof(char),22))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{
struct Cyc_Absyn_Decl*_tmp944;struct Cyc_Absyn_ExternCinclude_d_struct _tmp943;
struct Cyc_Absyn_ExternCinclude_d_struct*_tmp942;struct Cyc_List_List*_tmp941;
yyval=Cyc_YY18(((_tmp941=_cycalloc(sizeof(*_tmp941)),((_tmp941->hd=((_tmp944=
_cycalloc(sizeof(*_tmp944)),((_tmp944->r=(void*)((_tmp942=_cycalloc(sizeof(*
_tmp942)),((_tmp942[0]=((_tmp943.tag=13,((_tmp943.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp943.f2=exs,_tmp943)))))),
_tmp942)))),((_tmp944->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp944)))))),((
_tmp941->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp941)))))));}break;}case 9: _LL224:{struct Cyc_Absyn_Decl*
_tmp947;struct Cyc_List_List*_tmp946;yyval=Cyc_YY18(((_tmp946=_cycalloc(sizeof(*
_tmp946)),((_tmp946->hd=((_tmp947=_cycalloc(sizeof(*_tmp947)),((_tmp947->r=(void*)
0,((_tmp947->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmp947)))))),((
_tmp946->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp946)))))));}break;case 10: _LL225:{struct Cyc_Absyn_Decl*
_tmp94A;struct Cyc_List_List*_tmp949;yyval=Cyc_YY18(((_tmp949=_cycalloc(sizeof(*
_tmp949)),((_tmp949->hd=((_tmp94A=_cycalloc(sizeof(*_tmp94A)),((_tmp94A->r=(void*)
1,((_tmp94A->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmp94A)))))),((
_tmp949->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp949)))))));}break;case 11: _LL226: yyval=Cyc_YY18(0);break;case
12: _LL227: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 13: _LL228: yyval=Cyc_YY52(0);break;case 14: _LL229:{struct _tuple24*
_tmp94D;struct Cyc_List_List*_tmp94C;yyval=Cyc_YY52(((_tmp94C=_cycalloc(sizeof(*
_tmp94C)),((_tmp94C->hd=((_tmp94D=_cycalloc(sizeof(*_tmp94D)),((_tmp94D->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmp94D->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmp94D->f3=0,_tmp94D)))))))),((_tmp94C->tl=0,_tmp94C)))))));}break;case 15:
_LL22A:{struct _tuple24*_tmp950;struct Cyc_List_List*_tmp94F;yyval=Cyc_YY52(((
_tmp94F=_cycalloc(sizeof(*_tmp94F)),((_tmp94F->hd=((_tmp950=_cycalloc(sizeof(*
_tmp950)),((_tmp950->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmp950->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmp950->f3=0,_tmp950)))))))),((_tmp94F->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp94F)))))));}break;case
16: _LL22B:{struct Cyc_Absyn_Fn_d_struct*_tmp956;struct Cyc_Absyn_Fn_d_struct
_tmp955;struct Cyc_List_List*_tmp954;yyval=Cyc_YY18(((_tmp954=_cycalloc(sizeof(*
_tmp954)),((_tmp954->hd=Cyc_Absyn_new_decl((void*)((_tmp956=_cycalloc(sizeof(*
_tmp956)),((_tmp956[0]=((_tmp955.tag=1,((_tmp955.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp955)))),_tmp956)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmp954->tl=0,
_tmp954)))))));}break;case 17: _LL22C: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 18: _LL22D: yyval=Cyc_YY18(0);break;case 19: _LL22E:
yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 20:
_LL22F:{struct Cyc_Core_Opt*_tmp957;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmp957=_cycalloc(sizeof(*_tmp957)),((_tmp957->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp957)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 21:
_LL230: yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 22:
_LL231:{struct Cyc_Core_Opt*_tmp958;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmp958=_cycalloc(sizeof(*_tmp958)),((_tmp958->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp958)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 23:
_LL232:{struct Cyc_Core_Opt*_tmp959;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmp959=_cycalloc(sizeof(*_tmp959)),((_tmp959->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp959)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 24:
_LL233:{struct Cyc_Core_Opt*_tmp95A;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmp95A=_cycalloc(sizeof(*_tmp95A)),((_tmp95A->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp95A)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 25:
_LL234: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL235: Cyc_Lex_leave_using();
break;case 27: _LL236:{struct _dyneither_ptr*_tmp95B;Cyc_Lex_enter_namespace(((
_tmp95B=_cycalloc(sizeof(*_tmp95B)),((_tmp95B[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp95B)))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL237: Cyc_Lex_leave_namespace();
break;case 29: _LL238: yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
30: _LL239: yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 31:
_LL23A:{struct Cyc_List_List*_tmp95C;yyval=Cyc_YY18(((_tmp95C=_cycalloc(sizeof(*
_tmp95C)),((_tmp95C->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmp95C->tl=0,
_tmp95C)))))));}break;case 32: _LL23B: {struct Cyc_List_List*_tmp421=0;{struct Cyc_List_List*
_tmp422=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
for(0;_tmp422 != 0;_tmp422=_tmp422->tl){struct _dyneither_ptr*_tmp423=(struct
_dyneither_ptr*)_tmp422->hd;struct _tuple1*_tmp95D;struct _tuple1*qv=(_tmp95D=
_cycalloc(sizeof(*_tmp95D)),((_tmp95D->f1=Cyc_Absyn_Rel_n(0),((_tmp95D->f2=
_tmp423,_tmp95D)))));struct Cyc_Absyn_Vardecl*_tmp424=Cyc_Absyn_new_vardecl(qv,
Cyc_Absyn_wildtyp(0),0);struct Cyc_List_List*_tmp95E;_tmp421=((_tmp95E=_cycalloc(
sizeof(*_tmp95E)),((_tmp95E->hd=_tmp424,((_tmp95E->tl=_tmp421,_tmp95E))))));}}
_tmp421=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp421);{struct Cyc_List_List*_tmp95F;yyval=Cyc_YY18(((_tmp95F=_cycalloc(sizeof(*
_tmp95F)),((_tmp95F->hd=Cyc_Absyn_letv_decl(_tmp421,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmp95F->tl=0,
_tmp95F)))))));}break;}case 33: _LL23C:{const char*_tmp960;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmp960="`H",_tag_dyneither(_tmp960,sizeof(char),3))))== 0){
const char*_tmp963;void*_tmp962;Cyc_Parse_err((struct _dyneither_ptr)((_tmp962=0,
Cyc_aprintf(((_tmp963="bad occurrence of heap region",_tag_dyneither(_tmp963,
sizeof(char),30))),_tag_dyneither(_tmp962,sizeof(void*),0)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}{const char*
_tmp964;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp964="`U",
_tag_dyneither(_tmp964,sizeof(char),3))))== 0){const char*_tmp967;void*_tmp966;
Cyc_Parse_err((struct _dyneither_ptr)((_tmp966=0,Cyc_aprintf(((_tmp967="bad occurrence of unique region",
_tag_dyneither(_tmp967,sizeof(char),32))),_tag_dyneither(_tmp966,sizeof(void*),0)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));}}{struct _dyneither_ptr*_tmp96A;struct Cyc_Absyn_Tvar*
_tmp969;struct Cyc_Absyn_Tvar*tv=(_tmp969=_cycalloc(sizeof(*_tmp969)),((_tmp969->name=((
_tmp96A=_cycalloc(sizeof(*_tmp96A)),((_tmp96A[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp96A)))),((_tmp969->identity=
- 1,((_tmp969->kind=Cyc_Tcutil_kind_to_bound((void*)3),_tmp969)))))));struct Cyc_Absyn_VarType_struct
_tmp96D;struct Cyc_Absyn_VarType_struct*_tmp96C;void*t=(void*)((_tmp96C=_cycalloc(
sizeof(*_tmp96C)),((_tmp96C[0]=((_tmp96D.tag=1,((_tmp96D.f1=tv,_tmp96D)))),
_tmp96C))));struct Cyc_Absyn_RgnHandleType_struct _tmp973;struct Cyc_Absyn_RgnHandleType_struct*
_tmp972;struct _dyneither_ptr*_tmp970;struct _tuple1*_tmp96F;struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(((_tmp96F=_cycalloc(sizeof(*_tmp96F)),((_tmp96F->f1=Cyc_Absyn_Loc_n,((
_tmp96F->f2=((_tmp970=_cycalloc(sizeof(*_tmp970)),((_tmp970[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmp970)))),_tmp96F)))))),(
void*)((_tmp972=_cycalloc(sizeof(*_tmp972)),((_tmp972[0]=((_tmp973.tag=14,((
_tmp973.f1=(void*)t,_tmp973)))),_tmp972)))),0);{struct Cyc_List_List*_tmp974;
yyval=Cyc_YY18(((_tmp974=_cycalloc(sizeof(*_tmp974)),((_tmp974->hd=Cyc_Absyn_region_decl(
tv,vd,Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmp974->tl=0,_tmp974)))))));}break;}case 34: _LL23D:{
const char*_tmp975;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp975="H",
_tag_dyneither(_tmp975,sizeof(char),2))))== 0){const char*_tmp976;Cyc_Parse_err(((
_tmp976="bad occurrence of heap region `H",_tag_dyneither(_tmp976,sizeof(char),
33))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));}}{const char*_tmp977;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmp977="U",_tag_dyneither(_tmp977,sizeof(char),2))))== 0){
const char*_tmp978;Cyc_Parse_err(((_tmp978="bad occurrence of unique region `U",
_tag_dyneither(_tmp978,sizeof(char),35))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));}}if(Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]) && Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){const char*
_tmp979;Cyc_Parse_err(((_tmp979="open regions cannot be @resetable",
_tag_dyneither(_tmp979,sizeof(char),34))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}{struct
_dyneither_ptr*_tmp986;const char*_tmp985;void*_tmp984[1];struct Cyc_String_pa_struct
_tmp983;struct Cyc_Absyn_Tvar*_tmp982;struct Cyc_Absyn_Tvar*tv=(_tmp982=_cycalloc(
sizeof(*_tmp982)),((_tmp982->name=((_tmp986=_cycalloc(sizeof(*_tmp986)),((
_tmp986[0]=(struct _dyneither_ptr)((_tmp983.tag=0,((_tmp983.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmp984[0]=& _tmp983,
Cyc_aprintf(((_tmp985="`%s",_tag_dyneither(_tmp985,sizeof(char),4))),
_tag_dyneither(_tmp984,sizeof(void*),1)))))))),_tmp986)))),((_tmp982->identity=-
1,((_tmp982->kind=Cyc_Tcutil_kind_to_bound((void*)3),_tmp982)))))));struct Cyc_Absyn_VarType_struct
_tmp989;struct Cyc_Absyn_VarType_struct*_tmp988;void*t=(void*)((_tmp988=_cycalloc(
sizeof(*_tmp988)),((_tmp988[0]=((_tmp989.tag=1,((_tmp989.f1=tv,_tmp989)))),
_tmp988))));struct Cyc_Absyn_RgnHandleType_struct _tmp98F;struct Cyc_Absyn_RgnHandleType_struct*
_tmp98E;struct _dyneither_ptr*_tmp98C;struct _tuple1*_tmp98B;struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(((_tmp98B=_cycalloc(sizeof(*_tmp98B)),((_tmp98B->f1=Cyc_Absyn_Loc_n,((
_tmp98B->f2=((_tmp98C=_cycalloc(sizeof(*_tmp98C)),((_tmp98C[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp98C)))),_tmp98B)))))),(
void*)((_tmp98E=_cycalloc(sizeof(*_tmp98E)),((_tmp98E[0]=((_tmp98F.tag=14,((
_tmp98F.f1=(void*)t,_tmp98F)))),_tmp98E)))),0);{struct Cyc_List_List*_tmp990;
yyval=Cyc_YY18(((_tmp990=_cycalloc(sizeof(*_tmp990)),((_tmp990->hd=Cyc_Absyn_region_decl(
tv,vd,Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmp990->tl=0,
_tmp990)))))));}break;}case 35: _LL23E: {struct _dyneither_ptr*_tmp998;struct Cyc_Absyn_Eq_kb_struct*
_tmp997;struct Cyc_Absyn_Eq_kb_struct _tmp996;struct Cyc_Absyn_Tvar*_tmp995;struct
Cyc_Absyn_Tvar*tv=(_tmp995=_cycalloc(sizeof(*_tmp995)),((_tmp995->name=((_tmp998=
_cycalloc(sizeof(*_tmp998)),((_tmp998[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmp998)))),((_tmp995->identity=
- 1,((_tmp995->kind=(void*)((_tmp997=_cycalloc(sizeof(*_tmp997)),((_tmp997[0]=((
_tmp996.tag=0,((_tmp996.f1=(void*)((void*)3),_tmp996)))),_tmp997)))),_tmp995)))))));
struct _dyneither_ptr*_tmp99B;struct _tuple1*_tmp99A;struct Cyc_Absyn_Vardecl*vd=
Cyc_Absyn_new_vardecl(((_tmp99A=_cycalloc(sizeof(*_tmp99A)),((_tmp99A->f1=Cyc_Absyn_Loc_n,((
_tmp99A->f2=((_tmp99B=_cycalloc(sizeof(*_tmp99B)),((_tmp99B[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp99B)))),_tmp99A)))))),(
void*)0,0);{struct Cyc_List_List*_tmp99C;yyval=Cyc_YY18(((_tmp99C=_cycalloc(
sizeof(*_tmp99C)),((_tmp99C->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),tv,vd,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmp99C->tl=0,
_tmp99C)))))));}break;}case 36: _LL23F: yyval=Cyc_YY31(0);break;case 37: _LL240: yyval=
Cyc_YY31(1);break;case 38: _LL241: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 39: _LL242: yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 40: _LL243:{
struct Cyc_Core_Opt*_tmp99F;struct Cyc_Parse_Declaration_spec*_tmp99E;yyval=Cyc_YY19(((
_tmp99E=_cycalloc(sizeof(*_tmp99E)),((_tmp99E->sc=((_tmp99F=_cycalloc(sizeof(*
_tmp99F)),((_tmp99F->v=(void*)Cyc_yyget_YY22(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),_tmp99F)))),((_tmp99E->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmp99E->type_specs=
0,((_tmp99E->is_inline=0,((_tmp99E->attributes=0,_tmp99E)))))))))))));}break;
case 41: _LL244: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc != 0){const char*_tmp9A2;void*_tmp9A1;(_tmp9A1=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmp9A2="Only one storage class is allowed in a declaration",
_tag_dyneither(_tmp9A2,sizeof(char),51))),_tag_dyneither(_tmp9A1,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmp9A5;struct Cyc_Parse_Declaration_spec*_tmp9A4;yyval=Cyc_YY19(((
_tmp9A4=_cycalloc(sizeof(*_tmp9A4)),((_tmp9A4->sc=((_tmp9A5=_cycalloc(sizeof(*
_tmp9A5)),((_tmp9A5->v=(void*)Cyc_yyget_YY22(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmp9A5)))),((_tmp9A4->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((_tmp9A4->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmp9A4->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmp9A4->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmp9A4)))))))))))));}
break;case 42: _LL245:{struct Cyc_List_List*_tmp9A8;struct Cyc_Parse_Declaration_spec*
_tmp9A7;yyval=Cyc_YY19(((_tmp9A7=_cycalloc(sizeof(*_tmp9A7)),((_tmp9A7->sc=0,((
_tmp9A7->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmp9A7->type_specs=((
_tmp9A8=_cycalloc(sizeof(*_tmp9A8)),((_tmp9A8->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp9A8->tl=0,_tmp9A8)))))),((
_tmp9A7->is_inline=0,((_tmp9A7->attributes=0,_tmp9A7)))))))))))));}break;case 43:
_LL246:{struct Cyc_List_List*_tmp9AB;struct Cyc_Parse_Declaration_spec*_tmp9AA;
yyval=Cyc_YY19(((_tmp9AA=_cycalloc(sizeof(*_tmp9AA)),((_tmp9AA->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmp9AA->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmp9AA->type_specs=((_tmp9AB=_cycalloc(sizeof(*_tmp9AB)),((_tmp9AB->hd=(void*)
Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmp9AB->tl=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,_tmp9AB)))))),((_tmp9AA->is_inline=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmp9AA->attributes=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,
_tmp9AA)))))))))))));}break;case 44: _LL247:{struct Cyc_Parse_Declaration_spec*
_tmp9AC;yyval=Cyc_YY19(((_tmp9AC=_cycalloc(sizeof(*_tmp9AC)),((_tmp9AC->sc=0,((
_tmp9AC->tq=Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmp9AC->type_specs=0,((_tmp9AC->is_inline=0,((_tmp9AC->attributes=
0,_tmp9AC)))))))))))));}break;case 45: _LL248:{struct Cyc_Parse_Declaration_spec*
_tmp9AD;yyval=Cyc_YY19(((_tmp9AD=_cycalloc(sizeof(*_tmp9AD)),((_tmp9AD->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmp9AD->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq),((_tmp9AD->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmp9AD->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmp9AD->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmp9AD)))))))))))));}
break;case 46: _LL249:{struct Cyc_Parse_Declaration_spec*_tmp9AE;yyval=Cyc_YY19(((
_tmp9AE=_cycalloc(sizeof(*_tmp9AE)),((_tmp9AE->sc=0,((_tmp9AE->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmp9AE->type_specs=0,((_tmp9AE->is_inline=1,((
_tmp9AE->attributes=0,_tmp9AE)))))))))))));}break;case 47: _LL24A:{struct Cyc_Parse_Declaration_spec*
_tmp9AF;yyval=Cyc_YY19(((_tmp9AF=_cycalloc(sizeof(*_tmp9AF)),((_tmp9AF->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmp9AF->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmp9AF->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmp9AF->is_inline=1,((_tmp9AF->attributes=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmp9AF)))))))))))));}
break;case 48: _LL24B:{struct Cyc_Parse_Declaration_spec*_tmp9B0;yyval=Cyc_YY19(((
_tmp9B0=_cycalloc(sizeof(*_tmp9B0)),((_tmp9B0->sc=0,((_tmp9B0->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmp9B0->type_specs=0,((_tmp9B0->is_inline=0,((
_tmp9B0->attributes=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp9B0)))))))))))));}break;case 49: _LL24C:{struct Cyc_Parse_Declaration_spec*
_tmp9B1;yyval=Cyc_YY19(((_tmp9B1=_cycalloc(sizeof(*_tmp9B1)),((_tmp9B1->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmp9B1->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmp9B1->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmp9B1->is_inline=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmp9B1->attributes=
Cyc_List_imp_append(Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->attributes),_tmp9B1)))))))))))));}break;case 50: _LL24D: yyval=
Cyc_YY22((void*)4);break;case 51: _LL24E: yyval=Cyc_YY22((void*)5);break;case 52:
_LL24F: yyval=Cyc_YY22((void*)3);break;case 53: _LL250: yyval=Cyc_YY22((void*)1);
break;case 54: _LL251:{const char*_tmp9B2;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp9B2="C",
_tag_dyneither(_tmp9B2,sizeof(char),2))))!= 0){const char*_tmp9B3;Cyc_Parse_err(((
_tmp9B3="only extern or extern \"C\" is allowed",_tag_dyneither(_tmp9B3,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}}yyval=Cyc_YY22((void*)2);break;case 55: _LL252: yyval=
Cyc_YY22((void*)0);break;case 56: _LL253: yyval=Cyc_YY22((void*)6);break;case 57:
_LL254: yyval=Cyc_YY45(0);break;case 58: _LL255: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 59: _LL256: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;case 60: _LL257:{
struct Cyc_List_List*_tmp9B4;yyval=Cyc_YY45(((_tmp9B4=_cycalloc(sizeof(*_tmp9B4)),((
_tmp9B4->hd=(void*)Cyc_yyget_YY46(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmp9B4->tl=0,_tmp9B4)))))));}break;case 61: _LL258:{struct Cyc_List_List*
_tmp9B5;yyval=Cyc_YY45(((_tmp9B5=_cycalloc(sizeof(*_tmp9B5)),((_tmp9B5->hd=(void*)
Cyc_yyget_YY46(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmp9B5->tl=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp9B5)))))));}break;case 62: _LL259: {static struct Cyc_Absyn_Aligned_att_struct
att_aligned={1,- 1};static struct _tuple25 att_map[17]={{{_tmp465,_tmp465,_tmp465 + 8},(
void*)0},{{_tmp466,_tmp466,_tmp466 + 6},(void*)1},{{_tmp467,_tmp467,_tmp467 + 9},(
void*)2},{{_tmp468,_tmp468,_tmp468 + 9},(void*)3},{{_tmp469,_tmp469,_tmp469 + 6},(
void*)4},{{_tmp46A,_tmp46A,_tmp46A + 8},(void*)& att_aligned},{{_tmp46B,_tmp46B,
_tmp46B + 7},(void*)5},{{_tmp46C,_tmp46C,_tmp46C + 7},(void*)7},{{_tmp46D,_tmp46D,
_tmp46D + 7},(void*)8},{{_tmp46E,_tmp46E,_tmp46E + 5},(void*)9},{{_tmp46F,_tmp46F,
_tmp46F + 10},(void*)10},{{_tmp470,_tmp470,_tmp470 + 10},(void*)11},{{_tmp471,
_tmp471,_tmp471 + 23},(void*)12},{{_tmp472,_tmp472,_tmp472 + 12},(void*)13},{{
_tmp473,_tmp473,_tmp473 + 11},(void*)14},{{_tmp474,_tmp474,_tmp474 + 22},(void*)15},{{
_tmp475,_tmp475,_tmp475 + 5},(void*)16}};struct _dyneither_ptr _tmp463=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((
_get_dyneither_size(_tmp463,sizeof(char))> 4  && *((const char*)
_check_dyneither_subscript(_tmp463,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp463,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp463,sizeof(char),(int)(_get_dyneither_size(
_tmp463,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp463,sizeof(char),(int)(_get_dyneither_size(_tmp463,sizeof(char))- 3)))== '_')
_tmp463=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp463,2,
_get_dyneither_size(_tmp463,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp463,(struct _dyneither_ptr)(att_map[i]).f1)== 0){yyval=
Cyc_YY46((att_map[i]).f2);break;}}if(i == 17){{const char*_tmp9B6;Cyc_Parse_err(((
_tmp9B6="unrecognized attribute",_tag_dyneither(_tmp9B6,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=Cyc_YY46((
void*)1);}break;}}case 63: _LL25A: yyval=Cyc_YY46((void*)4);break;case 64: _LL25B: {
struct _dyneither_ptr _tmp477=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);int _tmp479;struct _tuple6
_tmp478=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]);_tmp479=_tmp478.f2;{void*a;{const char*_tmp9B8;const char*_tmp9B7;if(Cyc_zstrcmp((
struct _dyneither_ptr)_tmp477,((_tmp9B8="regparm",_tag_dyneither(_tmp9B8,sizeof(
char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp477,((_tmp9B7="__regparm__",
_tag_dyneither(_tmp9B7,sizeof(char),12))))== 0){if(_tmp479 < 0  || _tmp479 > 3){
const char*_tmp9B9;Cyc_Parse_err(((_tmp9B9="regparm requires value between 0 and 3",
_tag_dyneither(_tmp9B9,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Regparm_att_struct
_tmp9BC;struct Cyc_Absyn_Regparm_att_struct*_tmp9BB;a=(void*)((_tmp9BB=
_cycalloc_atomic(sizeof(*_tmp9BB)),((_tmp9BB[0]=((_tmp9BC.tag=0,((_tmp9BC.f1=
_tmp479,_tmp9BC)))),_tmp9BB))));}}else{const char*_tmp9BE;const char*_tmp9BD;if(
Cyc_zstrcmp((struct _dyneither_ptr)_tmp477,((_tmp9BE="aligned",_tag_dyneither(
_tmp9BE,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp477,((
_tmp9BD="__aligned__",_tag_dyneither(_tmp9BD,sizeof(char),12))))== 0){if(_tmp479
< 0){const char*_tmp9BF;Cyc_Parse_err(((_tmp9BF="aligned requires positive power of two",
_tag_dyneither(_tmp9BF,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{
unsigned int j=(unsigned int)_tmp479;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){
const char*_tmp9C0;Cyc_Parse_err(((_tmp9C0="aligned requires positive power of two",
_tag_dyneither(_tmp9C0,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Aligned_att_struct
_tmp9C3;struct Cyc_Absyn_Aligned_att_struct*_tmp9C2;a=(void*)((_tmp9C2=
_cycalloc_atomic(sizeof(*_tmp9C2)),((_tmp9C2[0]=((_tmp9C3.tag=1,((_tmp9C3.f1=
_tmp479,_tmp9C3)))),_tmp9C2))));}}}else{const char*_tmp9C5;const char*_tmp9C4;if(
Cyc_zstrcmp((struct _dyneither_ptr)_tmp477,((_tmp9C5="initializes",_tag_dyneither(
_tmp9C5,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp477,((
_tmp9C4="__initializes__",_tag_dyneither(_tmp9C4,sizeof(char),16))))== 0){struct
Cyc_Absyn_Initializes_att_struct _tmp9C8;struct Cyc_Absyn_Initializes_att_struct*
_tmp9C7;a=(void*)((_tmp9C7=_cycalloc_atomic(sizeof(*_tmp9C7)),((_tmp9C7[0]=((
_tmp9C8.tag=4,((_tmp9C8.f1=_tmp479,_tmp9C8)))),_tmp9C7))));}else{{const char*
_tmp9C9;Cyc_Parse_err(((_tmp9C9="unrecognized attribute",_tag_dyneither(_tmp9C9,
sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)1;}}}}
yyval=Cyc_YY46(a);break;}}case 65: _LL25C: {struct _dyneither_ptr _tmp48A=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct
_dyneither_ptr _tmp48B=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);void*a;{const char*_tmp9CB;const char*_tmp9CA;if(Cyc_zstrcmp((
struct _dyneither_ptr)_tmp48A,((_tmp9CB="section",_tag_dyneither(_tmp9CB,sizeof(
char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp48A,((_tmp9CA="__section__",
_tag_dyneither(_tmp9CA,sizeof(char),12))))== 0){struct Cyc_Absyn_Section_att_struct
_tmp9CE;struct Cyc_Absyn_Section_att_struct*_tmp9CD;a=(void*)((_tmp9CD=_cycalloc(
sizeof(*_tmp9CD)),((_tmp9CD[0]=((_tmp9CE.tag=2,((_tmp9CE.f1=_tmp48B,_tmp9CE)))),
_tmp9CD))));}else{{const char*_tmp9CF;Cyc_Parse_err(((_tmp9CF="unrecognized attribute",
_tag_dyneither(_tmp9CF,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)1;}}
yyval=Cyc_YY46(a);break;}case 66: _LL25D: {struct _dyneither_ptr _tmp491=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct
_dyneither_ptr _tmp492=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);void*a;{const char*_tmp9D0;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp491,((_tmp9D0="__mode__",_tag_dyneither(_tmp9D0,sizeof(char),
9))))== 0){struct Cyc_Absyn_Mode_att_struct _tmp9D3;struct Cyc_Absyn_Mode_att_struct*
_tmp9D2;a=(void*)((_tmp9D2=_cycalloc(sizeof(*_tmp9D2)),((_tmp9D2[0]=((_tmp9D3.tag=
5,((_tmp9D3.f1=_tmp492,_tmp9D3)))),_tmp9D2))));}else{{const char*_tmp9D4;Cyc_Parse_err(((
_tmp9D4="unrecognized attribute",_tag_dyneither(_tmp9D4,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)1;}}
yyval=Cyc_YY46(a);break;}case 67: _LL25E: {struct _dyneither_ptr _tmp497=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 7)]);struct
_dyneither_ptr _tmp498=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]);int _tmp49A;struct _tuple6 _tmp499=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp49A=_tmp499.f2;{int
_tmp49C;struct _tuple6 _tmp49B=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp49C=_tmp49B.f2;{void*
a=(void*)1;{const char*_tmp9D6;const char*_tmp9D5;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp497,((_tmp9D6="format",_tag_dyneither(_tmp9D6,sizeof(char),7))))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp497,((_tmp9D5="__format__",
_tag_dyneither(_tmp9D5,sizeof(char),11))))== 0){const char*_tmp9D7;if(Cyc_zstrcmp((
struct _dyneither_ptr)_tmp498,((_tmp9D7="printf",_tag_dyneither(_tmp9D7,sizeof(
char),7))))== 0){struct Cyc_Absyn_Format_att_struct _tmp9DA;struct Cyc_Absyn_Format_att_struct*
_tmp9D9;a=(void*)((_tmp9D9=_cycalloc(sizeof(*_tmp9D9)),((_tmp9D9[0]=((_tmp9DA.tag=
3,((_tmp9DA.f1=(void*)((void*)0),((_tmp9DA.f2=_tmp49A,((_tmp9DA.f3=_tmp49C,
_tmp9DA)))))))),_tmp9D9))));}else{const char*_tmp9DB;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp498,((_tmp9DB="scanf",_tag_dyneither(_tmp9DB,sizeof(char),6))))
== 0){struct Cyc_Absyn_Format_att_struct _tmp9DE;struct Cyc_Absyn_Format_att_struct*
_tmp9DD;a=(void*)((_tmp9DD=_cycalloc(sizeof(*_tmp9DD)),((_tmp9DD[0]=((_tmp9DE.tag=
3,((_tmp9DE.f1=(void*)((void*)1),((_tmp9DE.f2=_tmp49A,((_tmp9DE.f3=_tmp49C,
_tmp9DE)))))))),_tmp9DD))));}else{const char*_tmp9DF;Cyc_Parse_err(((_tmp9DF="unrecognized format type",
_tag_dyneither(_tmp9DF,sizeof(char),25))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{
const char*_tmp9E0;Cyc_Parse_err(((_tmp9E0="unrecognized attribute",
_tag_dyneither(_tmp9E0,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}yyval=Cyc_YY46(
a);break;}}}case 68: _LL25F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 69: _LL260:{struct Cyc_Absyn_TypedefType_struct _tmp9E3;
struct Cyc_Absyn_TypedefType_struct*_tmp9E2;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmp9E2=_cycalloc(sizeof(*_tmp9E2)),((_tmp9E2[0]=((_tmp9E3.tag=16,((
_tmp9E3.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmp9E3.f2=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp9E3.f3=0,((_tmp9E3.f4=
0,_tmp9E3)))))))))),_tmp9E2)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 70:
_LL261: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 71:
_LL262: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 72:
_LL263: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 73:
_LL264: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 74:
_LL265:{struct Cyc_Parse_Short_spec_struct _tmp9E6;struct Cyc_Parse_Short_spec_struct*
_tmp9E5;yyval=Cyc_YY23((void*)((_tmp9E5=_cycalloc(sizeof(*_tmp9E5)),((_tmp9E5[0]=((
_tmp9E6.tag=2,((_tmp9E6.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmp9E6)))),
_tmp9E5)))));}break;case 75: _LL266: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 76: _LL267:{struct Cyc_Parse_Long_spec_struct
_tmp9E9;struct Cyc_Parse_Long_spec_struct*_tmp9E8;yyval=Cyc_YY23((void*)((_tmp9E8=
_cycalloc(sizeof(*_tmp9E8)),((_tmp9E8[0]=((_tmp9E9.tag=3,((_tmp9E9.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmp9E9)))),
_tmp9E8)))));}break;case 77: _LL268: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 78: _LL269: yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 79:
_LL26A:{struct Cyc_Parse_Signed_spec_struct _tmp9EC;struct Cyc_Parse_Signed_spec_struct*
_tmp9EB;yyval=Cyc_YY23((void*)((_tmp9EB=_cycalloc(sizeof(*_tmp9EB)),((_tmp9EB[0]=((
_tmp9EC.tag=0,((_tmp9EC.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmp9EC)))),
_tmp9EB)))));}break;case 80: _LL26B:{struct Cyc_Parse_Unsigned_spec_struct _tmp9EF;
struct Cyc_Parse_Unsigned_spec_struct*_tmp9EE;yyval=Cyc_YY23((void*)((_tmp9EE=
_cycalloc(sizeof(*_tmp9EE)),((_tmp9EE[0]=((_tmp9EF.tag=1,((_tmp9EF.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmp9EF)))),
_tmp9EE)))));}break;case 81: _LL26C: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 82: _LL26D: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 83: _LL26E: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 84: _LL26F:
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 85:
_LL270:{struct Cyc_Absyn_TupleType_struct _tmp9F2;struct Cyc_Absyn_TupleType_struct*
_tmp9F1;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmp9F1=_cycalloc(sizeof(*
_tmp9F1)),((_tmp9F1[0]=((_tmp9F2.tag=9,((_tmp9F2.f1=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))),_tmp9F2)))),_tmp9F1)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 86: _LL271:{struct Cyc_Absyn_RgnHandleType_struct
_tmp9F5;struct Cyc_Absyn_RgnHandleType_struct*_tmp9F4;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmp9F4=_cycalloc(sizeof(*_tmp9F4)),((_tmp9F4[0]=((_tmp9F5.tag=14,((
_tmp9F5.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmp9F5)))),_tmp9F4)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 87:
_LL272: {void*_tmp4B5=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0);{
struct Cyc_Absyn_DynRgnType_struct _tmp9F8;struct Cyc_Absyn_DynRgnType_struct*
_tmp9F7;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmp9F7=_cycalloc(sizeof(*
_tmp9F7)),((_tmp9F7[0]=((_tmp9F8.tag=15,((_tmp9F8.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp9F8.f2=(void*)
_tmp4B5,_tmp9F8)))))),_tmp9F7)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case 88:
_LL273:{struct Cyc_Absyn_DynRgnType_struct _tmp9FB;struct Cyc_Absyn_DynRgnType_struct*
_tmp9FA;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmp9FA=_cycalloc(sizeof(*
_tmp9FA)),((_tmp9FA[0]=((_tmp9FB.tag=15,((_tmp9FB.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp9FB.f2=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmp9FB)))))),
_tmp9FA)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 89: _LL274:{struct Cyc_Absyn_TagType_struct
_tmp9FE;struct Cyc_Absyn_TagType_struct*_tmp9FD;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmp9FD=_cycalloc(sizeof(*_tmp9FD)),((_tmp9FD[0]=((_tmp9FE.tag=18,((
_tmp9FE.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmp9FE)))),_tmp9FD)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 90:
_LL275:{struct Cyc_Absyn_TagType_struct _tmpA01;struct Cyc_Absyn_TagType_struct*
_tmpA00;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpA00=_cycalloc(sizeof(*
_tmpA00)),((_tmpA00[0]=((_tmpA01.tag=18,((_tmpA01.f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ik,0),_tmpA01)))),_tmpA00)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 91:
_LL276:{struct Cyc_Absyn_ValueofType_struct _tmpA04;struct Cyc_Absyn_ValueofType_struct*
_tmpA03;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpA03=_cycalloc(sizeof(*
_tmpA03)),((_tmpA03[0]=((_tmpA04.tag=17,((_tmpA04.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpA04)))),_tmpA03)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 92: _LL277: yyval=Cyc_YY43(Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 93: _LL278: {struct Cyc_Position_Segment*
loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line): 0;{struct Cyc_Absyn_Tqual
_tmpA05;yyval=Cyc_YY25(((_tmpA05.print_const=1,((_tmpA05.q_volatile=0,((_tmpA05.q_restrict=
0,((_tmpA05.real_const=1,((_tmpA05.loc=loc,_tmpA05)))))))))));}break;}case 94:
_LL279:{struct Cyc_Absyn_Tqual _tmpA06;yyval=Cyc_YY25(((_tmpA06.print_const=0,((
_tmpA06.q_volatile=1,((_tmpA06.q_restrict=0,((_tmpA06.real_const=0,((_tmpA06.loc=
0,_tmpA06)))))))))));}break;case 95: _LL27A:{struct Cyc_Absyn_Tqual _tmpA07;yyval=
Cyc_YY25(((_tmpA07.print_const=0,((_tmpA07.q_volatile=0,((_tmpA07.q_restrict=1,((
_tmpA07.real_const=0,((_tmpA07.loc=0,_tmpA07)))))))))));}break;case 96: _LL27B:{
struct Cyc_Parse_Decl_spec_struct _tmpA23;struct Cyc_Absyn_Enum_d_struct*_tmpA22;
struct Cyc_Absyn_Enumdecl*_tmpA21;struct Cyc_Core_Opt*_tmpA20;struct Cyc_Absyn_Enum_d_struct
_tmpA1F;struct Cyc_Absyn_Decl*_tmpA1E;struct Cyc_Parse_Decl_spec_struct*_tmpA1D;
yyval=Cyc_YY23((void*)((_tmpA1D=_cycalloc(sizeof(*_tmpA1D)),((_tmpA1D[0]=((
_tmpA23.tag=5,((_tmpA23.f1=((_tmpA1E=_cycalloc(sizeof(*_tmpA1E)),((_tmpA1E->r=(
void*)((_tmpA22=_cycalloc(sizeof(*_tmpA22)),((_tmpA22[0]=((_tmpA1F.tag=8,((
_tmpA1F.f1=((_tmpA21=_cycalloc(sizeof(*_tmpA21)),((_tmpA21->sc=(void*)2,((
_tmpA21->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmpA21->fields=((_tmpA20=_cycalloc(sizeof(*_tmpA20)),((
_tmpA20->v=Cyc_yyget_YY48(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),_tmpA20)))),_tmpA21)))))))),_tmpA1F)))),_tmpA22)))),((_tmpA1E->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpA1E)))))),
_tmpA23)))),_tmpA1D)))));}break;case 97: _LL27C:{struct Cyc_Absyn_EnumType_struct
_tmpA26;struct Cyc_Absyn_EnumType_struct*_tmpA25;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpA25=_cycalloc(sizeof(*_tmpA25)),((_tmpA25[0]=((_tmpA26.tag=12,((
_tmpA26.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpA26.f2=0,_tmpA26)))))),_tmpA25)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 98:
_LL27D:{struct Cyc_Parse_Type_spec_struct _tmpA30;struct Cyc_Absyn_AnonEnumType_struct
_tmpA2F;struct Cyc_Absyn_AnonEnumType_struct*_tmpA2E;struct Cyc_Parse_Type_spec_struct*
_tmpA2D;yyval=Cyc_YY23((void*)((_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D[0]=((
_tmpA30.tag=4,((_tmpA30.f1=(void*)((void*)((_tmpA2E=_cycalloc(sizeof(*_tmpA2E)),((
_tmpA2E[0]=((_tmpA2F.tag=13,((_tmpA2F.f1=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpA2F)))),_tmpA2E))))),((
_tmpA30.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpA30)))))),_tmpA2D)))));}break;case 99: _LL27E:{
struct Cyc_Absyn_Enumfield*_tmpA31;yyval=Cyc_YY47(((_tmpA31=_cycalloc(sizeof(*
_tmpA31)),((_tmpA31->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA31->tag=0,((_tmpA31->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpA31)))))))));}break;case 100: _LL27F:{struct Cyc_Absyn_Enumfield*
_tmpA32;yyval=Cyc_YY47(((_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA32->tag=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpA32->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpA32)))))))));}
break;case 101: _LL280:{struct Cyc_List_List*_tmpA33;yyval=Cyc_YY48(((_tmpA33=
_cycalloc(sizeof(*_tmpA33)),((_tmpA33->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA33->tl=0,_tmpA33)))))));}
break;case 102: _LL281:{struct Cyc_List_List*_tmpA34;yyval=Cyc_YY48(((_tmpA34=
_cycalloc(sizeof(*_tmpA34)),((_tmpA34->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpA34->tl=0,_tmpA34)))))));}
break;case 103: _LL282:{struct Cyc_List_List*_tmpA35;yyval=Cyc_YY48(((_tmpA35=
_cycalloc(sizeof(*_tmpA35)),((_tmpA35->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA35->tl=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA35)))))));}break;
case 104: _LL283:{struct Cyc_Absyn_AnonAggrType_struct _tmpA38;struct Cyc_Absyn_AnonAggrType_struct*
_tmpA37;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpA37=_cycalloc(sizeof(*
_tmpA37)),((_tmpA37[0]=((_tmpA38.tag=11,((_tmpA38.f1=(void*)Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA38.f2=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpA38)))))),
_tmpA37)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 105: _LL284: {struct Cyc_List_List*_tmp4D7=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]));struct Cyc_List_List*_tmp4D8=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct
_tmpA3B;struct Cyc_Parse_Decl_spec_struct*_tmpA3A;yyval=Cyc_YY23((void*)((_tmpA3A=
_cycalloc(sizeof(*_tmpA3A)),((_tmpA3A[0]=((_tmpA3B.tag=5,((_tmpA3B.f1=Cyc_Absyn_aggr_decl(
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(
void*)2,Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 6)]),_tmp4D7,(struct Cyc_Absyn_AggrdeclImpl*)Cyc_Absyn_aggrdecl_impl(
_tmp4D8,Cyc_yyget_YY50(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),1),0,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 8)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpA3B)))),_tmpA3A)))));}break;}case 106: _LL285: {
struct Cyc_List_List*_tmp4DB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp4DC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct _tmpA3E;struct Cyc_Parse_Decl_spec_struct*
_tmpA3D;yyval=Cyc_YY23((void*)((_tmpA3D=_cycalloc(sizeof(*_tmpA3D)),((_tmpA3D[0]=((
_tmpA3E.tag=5,((_tmpA3E.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4DB,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4DC,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpA3E)))),
_tmpA3D)))));}break;}case 107: _LL286:{struct Cyc_Absyn_AggrType_struct _tmpA48;
struct Cyc_Core_Opt*_tmpA47;struct Cyc_Absyn_AggrInfo _tmpA46;struct Cyc_Absyn_AggrType_struct*
_tmpA45;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpA45=_cycalloc(sizeof(*
_tmpA45)),((_tmpA45[0]=((_tmpA48.tag=10,((_tmpA48.f1=((_tmpA46.aggr_info=Cyc_Absyn_UnknownAggr(
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpA47=
_cycalloc_atomic(sizeof(*_tmpA47)),((_tmpA47->v=(void*)1,_tmpA47))))),((_tmpA46.targs=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA46)))),
_tmpA48)))),_tmpA45)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 108:
_LL287:{struct Cyc_Absyn_AggrType_struct _tmpA4E;struct Cyc_Absyn_AggrInfo _tmpA4D;
struct Cyc_Absyn_AggrType_struct*_tmpA4C;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((
_tmpA4C=_cycalloc(sizeof(*_tmpA4C)),((_tmpA4C[0]=((_tmpA4E.tag=10,((_tmpA4E.f1=((
_tmpA4D.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),((_tmpA4D.targs=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA4D)))),
_tmpA4E)))),_tmpA4C)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 109:
_LL288: yyval=Cyc_YY40(0);break;case 110: _LL289: yyval=Cyc_YY40(Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
break;case 111: _LL28A: yyval=Cyc_YY24((void*)0);break;case 112: _LL28B: yyval=Cyc_YY24((
void*)1);break;case 113: _LL28C: yyval=Cyc_YY26(0);break;case 114: _LL28D: yyval=Cyc_YY26(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))));break;case 115: _LL28E:{
struct Cyc_List_List*_tmpA4F;yyval=Cyc_YY27(((_tmpA4F=_cycalloc(sizeof(*_tmpA4F)),((
_tmpA4F->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpA4F->tl=0,_tmpA4F)))))));}break;case 116: _LL28F:{struct Cyc_List_List*
_tmpA50;yyval=Cyc_YY27(((_tmpA50=_cycalloc(sizeof(*_tmpA50)),((_tmpA50->hd=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA50->tl=Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpA50)))))));}
break;case 117: _LL290: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 118: _LL291:{struct Cyc_List_List*_tmpA51;yyval=Cyc_YY21(((
_tmpA51=_cycalloc(sizeof(*_tmpA51)),((_tmpA51->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA51->tl=0,_tmpA51)))))));}
break;case 119: _LL292:{struct Cyc_List_List*_tmpA52;yyval=Cyc_YY21(((_tmpA52=
_cycalloc(sizeof(*_tmpA52)),((_tmpA52->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA52->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA52)))))));}
break;case 120: _LL293:{struct _tuple21*_tmpA53;yyval=Cyc_YY20(((_tmpA53=_cycalloc(
sizeof(*_tmpA53)),((_tmpA53->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA53->f2=0,_tmpA53)))))));}
break;case 121: _LL294:{struct _tuple21*_tmpA54;yyval=Cyc_YY20(((_tmpA54=_cycalloc(
sizeof(*_tmpA54)),((_tmpA54->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA54->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA54)))))));}
break;case 122: _LL295: {struct _RegionHandle _tmp4EC=_new_region("temp");struct
_RegionHandle*temp=& _tmp4EC;_push_region(temp);{struct _tuple22 _tmp4EE;struct Cyc_Absyn_Tqual
_tmp4EF;struct Cyc_List_List*_tmp4F0;struct Cyc_List_List*_tmp4F1;struct _tuple22*
_tmp4ED=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp4EE=*_tmp4ED;_tmp4EF=_tmp4EE.f1;_tmp4F0=_tmp4EE.f2;_tmp4F1=_tmp4EE.f3;if(
_tmp4EF.loc == 0)_tmp4EF.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);{struct Cyc_List_List*
_tmp4F3;struct Cyc_List_List*_tmp4F4;struct _tuple0 _tmp4F2=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
temp,temp,Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]));_tmp4F3=_tmp4F2.f1;_tmp4F4=_tmp4F2.f2;{void*_tmp4F5=Cyc_Parse_speclist2typ(
_tmp4F0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));struct Cyc_List_List*_tmp4F6=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(temp,_tmp4EF,_tmp4F5,_tmp4F3,
_tmp4F1),_tmp4F4);yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Position_Segment*,struct _tuple15*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp4F6));}}}
_npop_handler(0);break;;_pop_region(temp);}case 123: _LL296:{struct Cyc_List_List*
_tmpA57;struct _tuple22*_tmpA56;yyval=Cyc_YY35(((_tmpA56=_cycalloc(sizeof(*
_tmpA56)),((_tmpA56->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA56->f2=((
_tmpA57=_cycalloc(sizeof(*_tmpA57)),((_tmpA57->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA57->tl=0,_tmpA57)))))),((
_tmpA56->f3=0,_tmpA56)))))))));}break;case 124: _LL297:{struct Cyc_List_List*
_tmpA5A;struct _tuple22*_tmpA59;yyval=Cyc_YY35(((_tmpA59=_cycalloc(sizeof(*
_tmpA59)),((_tmpA59->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])).f1,((_tmpA59->f2=((_tmpA5A=_cycalloc(sizeof(*_tmpA5A)),((
_tmpA5A->hd=(void*)Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpA5A->tl=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpA5A)))))),((_tmpA59->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,
_tmpA59)))))))));}break;case 125: _LL298:{struct _tuple22*_tmpA5B;yyval=Cyc_YY35(((
_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA5B->f2=0,((_tmpA5B->f3=
0,_tmpA5B)))))))));}break;case 126: _LL299:{struct _tuple22*_tmpA5C;yyval=Cyc_YY35(((
_tmpA5C=_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpA5C->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpA5C->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpA5C)))))))));}break;case 127: _LL29A:{struct _tuple22*
_tmpA5D;yyval=Cyc_YY35(((_tmpA5D=_cycalloc(sizeof(*_tmpA5D)),((_tmpA5D->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpA5D->f2=0,((_tmpA5D->f3=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA5D)))))))));}break;
case 128: _LL29B:{struct _tuple22*_tmpA5E;yyval=Cyc_YY35(((_tmpA5E=_cycalloc(
sizeof(*_tmpA5E)),((_tmpA5E->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpA5E->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpA5E->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpA5E)))))))));}break;case 129: _LL29C:{struct Cyc_List_List*_tmpA61;struct
_tuple22*_tmpA60;yyval=Cyc_YY35(((_tmpA60=_cycalloc(sizeof(*_tmpA60)),((_tmpA60->f1=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA60->f2=((
_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA61->tl=0,_tmpA61)))))),((
_tmpA60->f3=0,_tmpA60)))))))));}break;case 130: _LL29D:{struct Cyc_List_List*
_tmpA64;struct _tuple22*_tmpA63;yyval=Cyc_YY35(((_tmpA63=_cycalloc(sizeof(*
_tmpA63)),((_tmpA63->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])).f1,((_tmpA63->f2=((_tmpA64=_cycalloc(sizeof(*_tmpA64)),((
_tmpA64->hd=(void*)Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpA64->tl=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpA64)))))),((_tmpA63->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,
_tmpA63)))))))));}break;case 131: _LL29E:{struct _tuple22*_tmpA65;yyval=Cyc_YY35(((
_tmpA65=_cycalloc(sizeof(*_tmpA65)),((_tmpA65->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA65->f2=0,((_tmpA65->f3=
0,_tmpA65)))))))));}break;case 132: _LL29F:{struct _tuple22*_tmpA66;yyval=Cyc_YY35(((
_tmpA66=_cycalloc(sizeof(*_tmpA66)),((_tmpA66->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpA66->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpA66->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpA66)))))))));}break;case 133: _LL2A0:{struct _tuple22*
_tmpA67;yyval=Cyc_YY35(((_tmpA67=_cycalloc(sizeof(*_tmpA67)),((_tmpA67->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpA67->f2=0,((_tmpA67->f3=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA67)))))))));}break;
case 134: _LL2A1:{struct _tuple22*_tmpA68;yyval=Cyc_YY35(((_tmpA68=_cycalloc(
sizeof(*_tmpA68)),((_tmpA68->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpA68->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpA68->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpA68)))))))));}break;case 135: _LL2A2: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 136: _LL2A3:{
struct Cyc_List_List*_tmpA69;yyval=Cyc_YY21(((_tmpA69=_cycalloc(sizeof(*_tmpA69)),((
_tmpA69->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpA69->tl=0,_tmpA69)))))));}break;case 137: _LL2A4:{struct Cyc_List_List*
_tmpA6A;yyval=Cyc_YY21(((_tmpA6A=_cycalloc(sizeof(*_tmpA6A)),((_tmpA6A->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA6A->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA6A)))))));}
break;case 138: _LL2A5:{struct _tuple21*_tmpA6B;yyval=Cyc_YY20(((_tmpA6B=_cycalloc(
sizeof(*_tmpA6B)),((_tmpA6B->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA6B->f2=0,_tmpA6B)))))));}
break;case 139: _LL2A6:{struct Cyc_Parse_Declarator*_tmpA71;struct _tuple1*_tmpA70;
struct _tuple21*_tmpA6F;yyval=Cyc_YY20(((_tmpA6F=_cycalloc(sizeof(*_tmpA6F)),((
_tmpA6F->f1=((_tmpA71=_cycalloc(sizeof(*_tmpA71)),((_tmpA71->id=((_tmpA70=
_cycalloc(sizeof(*_tmpA70)),((_tmpA70->f1=Cyc_Absyn_Rel_n(0),((_tmpA70->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),
_tmpA70)))))),((_tmpA71->tms=0,_tmpA71)))))),((_tmpA6F->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA6F)))))));}
break;case 140: _LL2A7:{struct _tuple21*_tmpA72;yyval=Cyc_YY20(((_tmpA72=_cycalloc(
sizeof(*_tmpA72)),((_tmpA72->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA72->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA72)))))));}
break;case 141: _LL2A8: {int _tmp50F=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);struct Cyc_List_List*
_tmp510=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct _tmpA78;struct Cyc_Core_Opt*
_tmpA77;struct Cyc_Parse_Decl_spec_struct*_tmpA76;yyval=Cyc_YY23((void*)((_tmpA76=
_cycalloc(sizeof(*_tmpA76)),((_tmpA76[0]=((_tmpA78.tag=5,((_tmpA78.f1=Cyc_Absyn_datatype_decl((
void*)2,Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),_tmp510,((_tmpA77=_cycalloc(sizeof(*_tmpA77)),((_tmpA77->v=
Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpA77)))),_tmp50F,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpA78)))),
_tmpA76)))));}break;}case 142: _LL2A9: {int _tmp514=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);{struct Cyc_Absyn_DatatypeType_struct
_tmpA82;struct Cyc_Absyn_UnknownDatatypeInfo _tmpA81;struct Cyc_Absyn_DatatypeInfo
_tmpA80;struct Cyc_Absyn_DatatypeType_struct*_tmpA7F;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpA7F=_cycalloc(sizeof(*_tmpA7F)),((_tmpA7F[0]=((_tmpA82.tag=2,((
_tmpA82.f1=((_tmpA80.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpA81.name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpA81.is_extensible=
_tmp514,_tmpA81))))),((_tmpA80.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA80.rgn=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA80)))))),
_tmpA82)))),_tmpA7F)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
143: _LL2AA: {int _tmp519=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);{struct Cyc_Absyn_DatatypeFieldType_struct _tmpA8C;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpA8B;struct Cyc_Absyn_DatatypeFieldInfo _tmpA8A;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpA89;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpA89=_cycalloc(sizeof(*
_tmpA89)),((_tmpA89[0]=((_tmpA8C.tag=3,((_tmpA8C.f1=((_tmpA8A.field_info=Cyc_Absyn_UnknownDatatypefield(((
_tmpA8B.datatype_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),((_tmpA8B.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpA8B.is_extensible=
_tmp519,_tmpA8B))))))),((_tmpA8A.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA8A)))),_tmpA8C)))),
_tmpA89)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;}case 144: _LL2AB: yyval=Cyc_YY31(0);break;case
145: _LL2AC: yyval=Cyc_YY31(1);break;case 146: _LL2AD:{struct Cyc_List_List*_tmpA8D;
yyval=Cyc_YY34(((_tmpA8D=_cycalloc(sizeof(*_tmpA8D)),((_tmpA8D->hd=Cyc_yyget_YY33(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA8D->tl=0,
_tmpA8D)))))));}break;case 147: _LL2AE:{struct Cyc_List_List*_tmpA8E;yyval=Cyc_YY34(((
_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpA8E->tl=0,_tmpA8E)))))));}
break;case 148: _LL2AF:{struct Cyc_List_List*_tmpA8F;yyval=Cyc_YY34(((_tmpA8F=
_cycalloc(sizeof(*_tmpA8F)),((_tmpA8F->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA8F->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA8F)))))));}break;
case 149: _LL2B0:{struct Cyc_List_List*_tmpA90;yyval=Cyc_YY34(((_tmpA90=_cycalloc(
sizeof(*_tmpA90)),((_tmpA90->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA90->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA90)))))));}break;
case 150: _LL2B1: yyval=Cyc_YY32((void*)2);break;case 151: _LL2B2: yyval=Cyc_YY32((
void*)3);break;case 152: _LL2B3: yyval=Cyc_YY32((void*)0);break;case 153: _LL2B4:{
struct Cyc_Absyn_Datatypefield*_tmpA91;yyval=Cyc_YY33(((_tmpA91=_cycalloc(sizeof(*
_tmpA91)),((_tmpA91->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA91->typs=0,((_tmpA91->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpA91->sc=Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpA91)))))))))));}
break;case 154: _LL2B5: {struct Cyc_List_List*_tmp523=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Absyn_Datatypefield*
_tmpA92;yyval=Cyc_YY33(((_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA92->typs=
_tmp523,((_tmpA92->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpA92->sc=
Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tmpA92)))))))))));}break;}case 155: _LL2B6: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 156: _LL2B7:{struct
Cyc_Parse_Declarator*_tmpA93;yyval=Cyc_YY29(((_tmpA93=_cycalloc(sizeof(*_tmpA93)),((
_tmpA93->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id,((_tmpA93->tms=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpA93)))))));}
break;case 157: _LL2B8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 158: _LL2B9:{struct Cyc_Parse_Declarator*_tmpA94;yyval=
Cyc_YY29(((_tmpA94=_cycalloc(sizeof(*_tmpA94)),((_tmpA94->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->id,((_tmpA94->tms=
Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->tms),_tmpA94)))))));}break;case 159: _LL2BA:{struct Cyc_Parse_Declarator*
_tmpA95;yyval=Cyc_YY29(((_tmpA95=_cycalloc(sizeof(*_tmpA95)),((_tmpA95->id=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA95->tms=0,
_tmpA95)))))));}break;case 160: _LL2BB: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 161: _LL2BC: {struct Cyc_Parse_Declarator*
_tmp528=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{
struct Cyc_Absyn_Attributes_mod_struct*_tmpA9B;struct Cyc_Absyn_Attributes_mod_struct
_tmpA9A;struct Cyc_List_List*_tmpA99;_tmp528->tms=((_tmpA99=_cycalloc(sizeof(*
_tmpA99)),((_tmpA99->hd=(void*)((void*)((_tmpA9B=_cycalloc(sizeof(*_tmpA9B)),((
_tmpA9B[0]=((_tmpA9A.tag=5,((_tmpA9A.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpA9A.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpA9A)))))),_tmpA9B))))),((_tmpA99->tl=_tmp528->tms,_tmpA99))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 162: _LL2BD:{
struct Cyc_List_List*_tmpAA5;struct Cyc_Absyn_Carray_mod_struct _tmpAA4;struct Cyc_Absyn_Carray_mod_struct*
_tmpAA3;struct Cyc_Parse_Declarator*_tmpAA2;yyval=Cyc_YY29(((_tmpAA2=_cycalloc(
sizeof(*_tmpAA2)),((_tmpAA2->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpAA2->tms=((
_tmpAA5=_cycalloc(sizeof(*_tmpAA5)),((_tmpAA5->hd=(void*)((void*)((_tmpAA3=
_cycalloc(sizeof(*_tmpAA3)),((_tmpAA3[0]=((_tmpAA4.tag=0,((_tmpAA4.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAA4.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpAA4)))))),
_tmpAA3))))),((_tmpAA5->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpAA5)))))),_tmpAA2)))))));}break;case 163:
_LL2BE:{struct Cyc_List_List*_tmpAAF;struct Cyc_Absyn_ConstArray_mod_struct _tmpAAE;
struct Cyc_Absyn_ConstArray_mod_struct*_tmpAAD;struct Cyc_Parse_Declarator*_tmpAAC;
yyval=Cyc_YY29(((_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpAAC->tms=((
_tmpAAF=_cycalloc(sizeof(*_tmpAAF)),((_tmpAAF->hd=(void*)((void*)((_tmpAAD=
_cycalloc(sizeof(*_tmpAAD)),((_tmpAAD[0]=((_tmpAAE.tag=1,((_tmpAAE.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpAAE.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAAE.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpAAE)))))))),
_tmpAAD))))),((_tmpAAF->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpAAF)))))),_tmpAAC)))))));}break;case 164:
_LL2BF: {struct _tuple23 _tmp535;struct Cyc_List_List*_tmp536;int _tmp537;struct Cyc_Absyn_VarargInfo*
_tmp538;struct Cyc_Core_Opt*_tmp539;struct Cyc_List_List*_tmp53A;struct _tuple23*
_tmp534=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp535=*_tmp534;_tmp536=_tmp535.f1;_tmp537=_tmp535.f2;_tmp538=_tmp535.f3;
_tmp539=_tmp535.f4;_tmp53A=_tmp535.f5;{struct Cyc_List_List*_tmpAC4;struct Cyc_Absyn_Function_mod_struct
_tmpAC3;struct Cyc_Absyn_WithTypes_struct _tmpAC2;struct Cyc_Absyn_WithTypes_struct*
_tmpAC1;struct Cyc_Absyn_Function_mod_struct*_tmpAC0;struct Cyc_Parse_Declarator*
_tmpABF;yyval=Cyc_YY29(((_tmpABF=_cycalloc(sizeof(*_tmpABF)),((_tmpABF->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpABF->tms=((
_tmpAC4=_cycalloc(sizeof(*_tmpAC4)),((_tmpAC4->hd=(void*)((void*)((_tmpAC0=
_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0[0]=((_tmpAC3.tag=3,((_tmpAC3.f1=(void*)((
void*)((_tmpAC1=_cycalloc(sizeof(*_tmpAC1)),((_tmpAC1[0]=((_tmpAC2.tag=1,((
_tmpAC2.f1=_tmp536,((_tmpAC2.f2=_tmp537,((_tmpAC2.f3=_tmp538,((_tmpAC2.f4=
_tmp539,((_tmpAC2.f5=_tmp53A,_tmpAC2)))))))))))),_tmpAC1))))),_tmpAC3)))),
_tmpAC0))))),((_tmpAC4->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpAC4)))))),_tmpABF)))))));}break;}case 165:
_LL2C0:{struct Cyc_List_List*_tmpAD9;struct Cyc_Absyn_Function_mod_struct _tmpAD8;
struct Cyc_Absyn_WithTypes_struct _tmpAD7;struct Cyc_Absyn_WithTypes_struct*_tmpAD6;
struct Cyc_Absyn_Function_mod_struct*_tmpAD5;struct Cyc_Parse_Declarator*_tmpAD4;
yyval=Cyc_YY29(((_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((_tmpAD4->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpAD4->tms=((
_tmpAD9=_cycalloc(sizeof(*_tmpAD9)),((_tmpAD9->hd=(void*)((void*)((_tmpAD5=
_cycalloc(sizeof(*_tmpAD5)),((_tmpAD5[0]=((_tmpAD8.tag=3,((_tmpAD8.f1=(void*)((
void*)((_tmpAD6=_cycalloc(sizeof(*_tmpAD6)),((_tmpAD6[0]=((_tmpAD7.tag=1,((
_tmpAD7.f1=0,((_tmpAD7.f2=0,((_tmpAD7.f3=0,((_tmpAD7.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpAD7.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpAD7)))))))))))),
_tmpAD6))))),_tmpAD8)))),_tmpAD5))))),((_tmpAD9->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpAD9)))))),
_tmpAD4)))))));}break;case 166: _LL2C1:{struct Cyc_List_List*_tmpAEE;struct Cyc_Absyn_Function_mod_struct
_tmpAED;struct Cyc_Absyn_NoTypes_struct _tmpAEC;struct Cyc_Absyn_NoTypes_struct*
_tmpAEB;struct Cyc_Absyn_Function_mod_struct*_tmpAEA;struct Cyc_Parse_Declarator*
_tmpAE9;yyval=Cyc_YY29(((_tmpAE9=_cycalloc(sizeof(*_tmpAE9)),((_tmpAE9->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpAE9->tms=((
_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE->hd=(void*)((void*)((_tmpAEA=
_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA[0]=((_tmpAED.tag=3,((_tmpAED.f1=(void*)((
void*)((_tmpAEB=_cycalloc(sizeof(*_tmpAEB)),((_tmpAEB[0]=((_tmpAEC.tag=0,((
_tmpAEC.f1=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),((_tmpAEC.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpAEC)))))),
_tmpAEB))))),_tmpAED)))),_tmpAEA))))),((_tmpAEE->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmpAEE)))))),
_tmpAE9)))))));}break;case 167: _LL2C2: {struct Cyc_List_List*_tmp54D=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{
struct Cyc_List_List*_tmpAF8;struct Cyc_Absyn_TypeParams_mod_struct _tmpAF7;struct
Cyc_Absyn_TypeParams_mod_struct*_tmpAF6;struct Cyc_Parse_Declarator*_tmpAF5;yyval=
Cyc_YY29(((_tmpAF5=_cycalloc(sizeof(*_tmpAF5)),((_tmpAF5->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpAF5->tms=((
_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((_tmpAF8->hd=(void*)((void*)((_tmpAF6=
_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6[0]=((_tmpAF7.tag=4,((_tmpAF7.f1=_tmp54D,((
_tmpAF7.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpAF7.f3=0,_tmpAF7)))))))),_tmpAF6))))),((_tmpAF8->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpAF8)))))),_tmpAF5)))))));}break;}case 168: _LL2C3:{struct Cyc_List_List*_tmpB02;
struct Cyc_Absyn_Attributes_mod_struct _tmpB01;struct Cyc_Absyn_Attributes_mod_struct*
_tmpB00;struct Cyc_Parse_Declarator*_tmpAFF;yyval=Cyc_YY29(((_tmpAFF=_cycalloc(
sizeof(*_tmpAFF)),((_tmpAFF->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpAFF->tms=((
_tmpB02=_cycalloc(sizeof(*_tmpB02)),((_tmpB02->hd=(void*)((void*)((_tmpB00=
_cycalloc(sizeof(*_tmpB00)),((_tmpB00[0]=((_tmpB01.tag=5,((_tmpB01.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpB01.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB01)))))),_tmpB00))))),((
_tmpB02->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpB02)))))),_tmpAFF)))))));}break;case 169: _LL2C4:{
struct Cyc_Parse_Declarator*_tmpB03;yyval=Cyc_YY29(((_tmpB03=_cycalloc(sizeof(*
_tmpB03)),((_tmpB03->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpB03->tms=0,_tmpB03)))))));}break;case 170: _LL2C5:{
struct Cyc_Parse_Declarator*_tmpB04;yyval=Cyc_YY29(((_tmpB04=_cycalloc(sizeof(*
_tmpB04)),((_tmpB04->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpB04->tms=0,_tmpB04)))))));}break;case 171: _LL2C6:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 172:
_LL2C7: {struct Cyc_Parse_Declarator*_tmp558=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpB0A;struct Cyc_Absyn_Attributes_mod_struct _tmpB09;struct Cyc_List_List*_tmpB08;
_tmp558->tms=((_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08->hd=(void*)((void*)((
_tmpB0A=_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A[0]=((_tmpB09.tag=5,((_tmpB09.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpB09.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpB09)))))),_tmpB0A))))),((
_tmpB08->tl=_tmp558->tms,_tmpB08))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 173: _LL2C8:{
struct Cyc_List_List*_tmpB14;struct Cyc_Absyn_Carray_mod_struct _tmpB13;struct Cyc_Absyn_Carray_mod_struct*
_tmpB12;struct Cyc_Parse_Declarator*_tmpB11;yyval=Cyc_YY29(((_tmpB11=_cycalloc(
sizeof(*_tmpB11)),((_tmpB11->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpB11->tms=((
_tmpB14=_cycalloc(sizeof(*_tmpB14)),((_tmpB14->hd=(void*)((void*)((_tmpB12=
_cycalloc(sizeof(*_tmpB12)),((_tmpB12[0]=((_tmpB13.tag=0,((_tmpB13.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB13.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB13)))))),
_tmpB12))))),((_tmpB14->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpB14)))))),_tmpB11)))))));}break;case 174:
_LL2C9:{struct Cyc_List_List*_tmpB1E;struct Cyc_Absyn_ConstArray_mod_struct _tmpB1D;
struct Cyc_Absyn_ConstArray_mod_struct*_tmpB1C;struct Cyc_Parse_Declarator*_tmpB1B;
yyval=Cyc_YY29(((_tmpB1B=_cycalloc(sizeof(*_tmpB1B)),((_tmpB1B->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpB1B->tms=((
_tmpB1E=_cycalloc(sizeof(*_tmpB1E)),((_tmpB1E->hd=(void*)((void*)((_tmpB1C=
_cycalloc(sizeof(*_tmpB1C)),((_tmpB1C[0]=((_tmpB1D.tag=1,((_tmpB1D.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB1D.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB1D.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB1D)))))))),
_tmpB1C))))),((_tmpB1E->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpB1E)))))),_tmpB1B)))))));}break;case 175:
_LL2CA: {struct _tuple23 _tmp565;struct Cyc_List_List*_tmp566;int _tmp567;struct Cyc_Absyn_VarargInfo*
_tmp568;struct Cyc_Core_Opt*_tmp569;struct Cyc_List_List*_tmp56A;struct _tuple23*
_tmp564=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp565=*_tmp564;_tmp566=_tmp565.f1;_tmp567=_tmp565.f2;_tmp568=_tmp565.f3;
_tmp569=_tmp565.f4;_tmp56A=_tmp565.f5;{struct Cyc_List_List*_tmpB33;struct Cyc_Absyn_Function_mod_struct
_tmpB32;struct Cyc_Absyn_WithTypes_struct _tmpB31;struct Cyc_Absyn_WithTypes_struct*
_tmpB30;struct Cyc_Absyn_Function_mod_struct*_tmpB2F;struct Cyc_Parse_Declarator*
_tmpB2E;yyval=Cyc_YY29(((_tmpB2E=_cycalloc(sizeof(*_tmpB2E)),((_tmpB2E->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpB2E->tms=((
_tmpB33=_cycalloc(sizeof(*_tmpB33)),((_tmpB33->hd=(void*)((void*)((_tmpB2F=
_cycalloc(sizeof(*_tmpB2F)),((_tmpB2F[0]=((_tmpB32.tag=3,((_tmpB32.f1=(void*)((
void*)((_tmpB30=_cycalloc(sizeof(*_tmpB30)),((_tmpB30[0]=((_tmpB31.tag=1,((
_tmpB31.f1=_tmp566,((_tmpB31.f2=_tmp567,((_tmpB31.f3=_tmp568,((_tmpB31.f4=
_tmp569,((_tmpB31.f5=_tmp56A,_tmpB31)))))))))))),_tmpB30))))),_tmpB32)))),
_tmpB2F))))),((_tmpB33->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpB33)))))),_tmpB2E)))))));}break;}case 176:
_LL2CB:{struct Cyc_List_List*_tmpB48;struct Cyc_Absyn_Function_mod_struct _tmpB47;
struct Cyc_Absyn_WithTypes_struct _tmpB46;struct Cyc_Absyn_WithTypes_struct*_tmpB45;
struct Cyc_Absyn_Function_mod_struct*_tmpB44;struct Cyc_Parse_Declarator*_tmpB43;
yyval=Cyc_YY29(((_tmpB43=_cycalloc(sizeof(*_tmpB43)),((_tmpB43->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpB43->tms=((
_tmpB48=_cycalloc(sizeof(*_tmpB48)),((_tmpB48->hd=(void*)((void*)((_tmpB44=
_cycalloc(sizeof(*_tmpB44)),((_tmpB44[0]=((_tmpB47.tag=3,((_tmpB47.f1=(void*)((
void*)((_tmpB45=_cycalloc(sizeof(*_tmpB45)),((_tmpB45[0]=((_tmpB46.tag=1,((
_tmpB46.f1=0,((_tmpB46.f2=0,((_tmpB46.f3=0,((_tmpB46.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB46.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB46)))))))))))),
_tmpB45))))),_tmpB47)))),_tmpB44))))),((_tmpB48->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpB48)))))),
_tmpB43)))))));}break;case 177: _LL2CC:{struct Cyc_List_List*_tmpB5D;struct Cyc_Absyn_Function_mod_struct
_tmpB5C;struct Cyc_Absyn_NoTypes_struct _tmpB5B;struct Cyc_Absyn_NoTypes_struct*
_tmpB5A;struct Cyc_Absyn_Function_mod_struct*_tmpB59;struct Cyc_Parse_Declarator*
_tmpB58;yyval=Cyc_YY29(((_tmpB58=_cycalloc(sizeof(*_tmpB58)),((_tmpB58->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpB58->tms=((
_tmpB5D=_cycalloc(sizeof(*_tmpB5D)),((_tmpB5D->hd=(void*)((void*)((_tmpB59=
_cycalloc(sizeof(*_tmpB59)),((_tmpB59[0]=((_tmpB5C.tag=3,((_tmpB5C.f1=(void*)((
void*)((_tmpB5A=_cycalloc(sizeof(*_tmpB5A)),((_tmpB5A[0]=((_tmpB5B.tag=0,((
_tmpB5B.f1=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),((_tmpB5B.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB5B)))))),
_tmpB5A))))),_tmpB5C)))),_tmpB59))))),((_tmpB5D->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmpB5D)))))),
_tmpB58)))))));}break;case 178: _LL2CD: {struct Cyc_List_List*_tmp57D=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{
struct Cyc_List_List*_tmpB67;struct Cyc_Absyn_TypeParams_mod_struct _tmpB66;struct
Cyc_Absyn_TypeParams_mod_struct*_tmpB65;struct Cyc_Parse_Declarator*_tmpB64;yyval=
Cyc_YY29(((_tmpB64=_cycalloc(sizeof(*_tmpB64)),((_tmpB64->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpB64->tms=((
_tmpB67=_cycalloc(sizeof(*_tmpB67)),((_tmpB67->hd=(void*)((void*)((_tmpB65=
_cycalloc(sizeof(*_tmpB65)),((_tmpB65[0]=((_tmpB66.tag=4,((_tmpB66.f1=_tmp57D,((
_tmpB66.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpB66.f3=0,_tmpB66)))))))),_tmpB65))))),((_tmpB67->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpB67)))))),_tmpB64)))))));}break;}case 179: _LL2CE:{struct Cyc_List_List*_tmpB71;
struct Cyc_Absyn_Attributes_mod_struct _tmpB70;struct Cyc_Absyn_Attributes_mod_struct*
_tmpB6F;struct Cyc_Parse_Declarator*_tmpB6E;yyval=Cyc_YY29(((_tmpB6E=_cycalloc(
sizeof(*_tmpB6E)),((_tmpB6E->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpB6E->tms=((
_tmpB71=_cycalloc(sizeof(*_tmpB71)),((_tmpB71->hd=(void*)((void*)((_tmpB6F=
_cycalloc(sizeof(*_tmpB6F)),((_tmpB6F[0]=((_tmpB70.tag=5,((_tmpB70.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpB70.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB70)))))),_tmpB6F))))),((
_tmpB71->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpB71)))))),_tmpB6E)))))));}break;case 180: _LL2CF:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 181:
_LL2D0: yyval=Cyc_YY28(Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 182: _LL2D1: {
struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])!= 0){struct Cyc_Absyn_Attributes_mod_struct*_tmpB77;
struct Cyc_Absyn_Attributes_mod_struct _tmpB76;struct Cyc_List_List*_tmpB75;ans=((
_tmpB75=_cycalloc(sizeof(*_tmpB75)),((_tmpB75->hd=(void*)((void*)((_tmpB77=
_cycalloc(sizeof(*_tmpB77)),((_tmpB77[0]=((_tmpB76.tag=5,((_tmpB76.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpB76.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpB76)))))),_tmpB77))))),((_tmpB75->tl=ans,_tmpB75))))));}{struct Cyc_Absyn_PtrLoc*
ptrloc=0;struct Cyc_Position_Segment*_tmp58A;union Cyc_Absyn_Constraint*_tmp58B;
union Cyc_Absyn_Constraint*_tmp58C;struct _tuple18 _tmp589=*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp58A=_tmp589.f1;
_tmp58B=_tmp589.f2;_tmp58C=_tmp589.f3;if(Cyc_Absyn_porting_c_code){struct Cyc_Absyn_PtrLoc*
_tmpB78;ptrloc=((_tmpB78=_cycalloc(sizeof(*_tmpB78)),((_tmpB78->ptr_loc=_tmp58A,((
_tmpB78->rgn_loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpB78->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),_tmpB78))))))));}{union Cyc_Absyn_Constraint*
_tmp58F;union Cyc_Absyn_Constraint*_tmp590;union Cyc_Absyn_Constraint*_tmp591;void*
_tmp592;struct _tuple13 _tmp58E=Cyc_Parse_collapse_pointer_quals(_tmp58A,_tmp58B,
_tmp58C,Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY54(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
_tmp58F=_tmp58E.f1;_tmp590=_tmp58E.f2;_tmp591=_tmp58E.f3;_tmp592=_tmp58E.f4;{
struct Cyc_Absyn_Pointer_mod_struct*_tmpB82;struct Cyc_Absyn_PtrAtts _tmpB81;struct
Cyc_Absyn_Pointer_mod_struct _tmpB80;struct Cyc_List_List*_tmpB7F;ans=((_tmpB7F=
_cycalloc(sizeof(*_tmpB7F)),((_tmpB7F->hd=(void*)((void*)((_tmpB82=_cycalloc(
sizeof(*_tmpB82)),((_tmpB82[0]=((_tmpB80.tag=2,((_tmpB80.f1=((_tmpB81.rgn=
_tmp592,((_tmpB81.nullable=_tmp58F,((_tmpB81.bounds=_tmp590,((_tmpB81.zero_term=
_tmp591,((_tmpB81.ptrloc=ptrloc,_tmpB81)))))))))),((_tmpB80.f2=Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB80)))))),_tmpB82))))),((
_tmpB7F->tl=ans,_tmpB7F))))));}yyval=Cyc_YY28(ans);break;}}}case 183: _LL2D2: yyval=
Cyc_YY54(0);break;case 184: _LL2D3:{struct Cyc_List_List*_tmpB83;yyval=Cyc_YY54(((
_tmpB83=_cycalloc(sizeof(*_tmpB83)),((_tmpB83->hd=(void*)Cyc_yyget_YY53(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB83->tl=Cyc_yyget_YY54(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB83)))))));}break;
case 185: _LL2D4:{struct Cyc_Absyn_Numelts_ptrqual_struct _tmpB86;struct Cyc_Absyn_Numelts_ptrqual_struct*
_tmpB85;yyval=Cyc_YY53((void*)((_tmpB85=_cycalloc(sizeof(*_tmpB85)),((_tmpB85[0]=((
_tmpB86.tag=0,((_tmpB86.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpB86)))),_tmpB85)))));}break;case 186: _LL2D5:{struct
Cyc_Absyn_Region_ptrqual_struct _tmpB89;struct Cyc_Absyn_Region_ptrqual_struct*
_tmpB88;yyval=Cyc_YY53((void*)((_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88[0]=((
_tmpB89.tag=1,((_tmpB89.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB89)))),_tmpB88)))));}
break;case 187: _LL2D6: yyval=Cyc_YY53((void*)0);break;case 188: _LL2D7: yyval=Cyc_YY53((
void*)1);break;case 189: _LL2D8: yyval=Cyc_YY53((void*)2);break;case 190: _LL2D9:
yyval=Cyc_YY53((void*)3);break;case 191: _LL2DA: yyval=Cyc_YY53((void*)4);break;
case 192: _LL2DB: yyval=Cyc_YY53((void*)5);break;case 193: _LL2DC: {struct Cyc_Position_Segment*
loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct _tuple18*_tmpB8A;yyval=Cyc_YY1(((_tmpB8A=
_cycalloc(sizeof(*_tmpB8A)),((_tmpB8A->f1=loc,((_tmpB8A->f2=Cyc_Absyn_true_conref,((
_tmpB8A->f3=Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpB8A)))))))));}break;}case 194: _LL2DD: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct
_tuple18*_tmpB8B;yyval=Cyc_YY1(((_tmpB8B=_cycalloc(sizeof(*_tmpB8B)),((_tmpB8B->f1=
loc,((_tmpB8B->f2=Cyc_Absyn_false_conref,((_tmpB8B->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB8B)))))))));}break;}
case 195: _LL2DE: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);{struct _tuple18*
_tmpB8C;yyval=Cyc_YY1(((_tmpB8C=_cycalloc(sizeof(*_tmpB8C)),((_tmpB8C->f1=loc,((
_tmpB8C->f2=Cyc_Absyn_true_conref,((_tmpB8C->f3=Cyc_Absyn_bounds_dyneither_conref,
_tmpB8C)))))))));}break;}case 196: _LL2DF: yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 197: _LL2E0:{struct Cyc_Absyn_Upper_b_struct _tmpB8F;struct Cyc_Absyn_Upper_b_struct*
_tmpB8E;yyval=Cyc_YY2(Cyc_Absyn_new_conref((void*)((_tmpB8E=_cycalloc(sizeof(*
_tmpB8E)),((_tmpB8E[0]=((_tmpB8F.tag=0,((_tmpB8F.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB8F)))),_tmpB8E))))));}
break;case 198: _LL2E1: yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 199: _LL2E2: yyval=Cyc_YY51(Cyc_Absyn_true_conref);break;case 200: _LL2E3:
yyval=Cyc_YY51(Cyc_Absyn_false_conref);break;case 201: _LL2E4: yyval=Cyc_YY49(0);
break;case 202: _LL2E5: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3,1);{struct Cyc_Core_Opt*
_tmpB90;yyval=Cyc_YY49(((_tmpB90=_cycalloc(sizeof(*_tmpB90)),((_tmpB90->v=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB90)))));}
break;case 203: _LL2E6:{struct Cyc_Core_Opt*_tmpB91;yyval=Cyc_YY49(((_tmpB91=
_cycalloc(sizeof(*_tmpB91)),((_tmpB91->v=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0),_tmpB91)))));}break;case 204: _LL2E7: yyval=Cyc_YY44(Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,0));break;case 205: _LL2E8: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)
5,1);yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
206: _LL2E9: yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,
0));break;case 207: _LL2EA: yyval=Cyc_YY25(Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line)));break;case
208: _LL2EB: yyval=Cyc_YY25(Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 209: _LL2EC:{
struct _tuple23*_tmpB92;yyval=Cyc_YY39(((_tmpB92=_cycalloc(sizeof(*_tmpB92)),((
_tmpB92->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((
_tmpB92->f2=0,((_tmpB92->f3=0,((_tmpB92->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB92->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB92)))))))))))));}
break;case 210: _LL2ED:{struct _tuple23*_tmpB93;yyval=Cyc_YY39(((_tmpB93=_cycalloc(
sizeof(*_tmpB93)),((_tmpB93->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])),((_tmpB93->f2=1,((_tmpB93->f3=0,((_tmpB93->f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB93->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB93)))))))))))));}
break;case 211: _LL2EE: {struct _tuple8 _tmp5A6;struct Cyc_Core_Opt*_tmp5A7;struct Cyc_Absyn_Tqual
_tmp5A8;void*_tmp5A9;struct _tuple8*_tmp5A5=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5A6=*_tmp5A5;_tmp5A7=
_tmp5A6.f1;_tmp5A8=_tmp5A6.f2;_tmp5A9=_tmp5A6.f3;{struct Cyc_Absyn_VarargInfo*
_tmpB94;struct Cyc_Absyn_VarargInfo*_tmp5AA=(_tmpB94=_cycalloc(sizeof(*_tmpB94)),((
_tmpB94->name=_tmp5A7,((_tmpB94->tq=_tmp5A8,((_tmpB94->type=_tmp5A9,((_tmpB94->inject=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpB94)))))))));{struct _tuple23*_tmpB95;yyval=Cyc_YY39(((_tmpB95=_cycalloc(
sizeof(*_tmpB95)),((_tmpB95->f1=0,((_tmpB95->f2=0,((_tmpB95->f3=_tmp5AA,((
_tmpB95->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB95->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB95)))))))))))));}
break;}}case 212: _LL2EF: {struct _tuple8 _tmp5AE;struct Cyc_Core_Opt*_tmp5AF;struct
Cyc_Absyn_Tqual _tmp5B0;void*_tmp5B1;struct _tuple8*_tmp5AD=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5AE=*_tmp5AD;_tmp5AF=
_tmp5AE.f1;_tmp5B0=_tmp5AE.f2;_tmp5B1=_tmp5AE.f3;{struct Cyc_Absyn_VarargInfo*
_tmpB96;struct Cyc_Absyn_VarargInfo*_tmp5B2=(_tmpB96=_cycalloc(sizeof(*_tmpB96)),((
_tmpB96->name=_tmp5AF,((_tmpB96->tq=_tmp5B0,((_tmpB96->type=_tmp5B1,((_tmpB96->inject=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpB96)))))))));{struct _tuple23*_tmpB97;yyval=Cyc_YY39(((_tmpB97=_cycalloc(
sizeof(*_tmpB97)),((_tmpB97->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 6)])),((_tmpB97->f2=0,((_tmpB97->f3=_tmp5B2,((_tmpB97->f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB97->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB97)))))))))))));}
break;}}case 213: _LL2F0:{struct Cyc_Absyn_Unknown_kb_struct _tmpB9A;struct Cyc_Absyn_Unknown_kb_struct*
_tmpB99;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)((_tmpB99=_cycalloc(
sizeof(*_tmpB99)),((_tmpB99[0]=((_tmpB9A.tag=1,((_tmpB9A.f1=0,_tmpB9A)))),
_tmpB99))))));}break;case 214: _LL2F1: yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))));
break;case 215: _LL2F2: yyval=Cyc_YY49(0);break;case 216: _LL2F3:{struct Cyc_Absyn_JoinEff_struct*
_tmpBA0;struct Cyc_Absyn_JoinEff_struct _tmpB9F;struct Cyc_Core_Opt*_tmpB9E;yyval=
Cyc_YY49(((_tmpB9E=_cycalloc(sizeof(*_tmpB9E)),((_tmpB9E->v=(void*)((void*)((
_tmpBA0=_cycalloc(sizeof(*_tmpBA0)),((_tmpBA0[0]=((_tmpB9F.tag=20,((_tmpB9F.f1=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB9F)))),
_tmpBA0))))),_tmpB9E)))));}break;case 217: _LL2F4: yyval=Cyc_YY50(0);break;case 218:
_LL2F5: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
219: _LL2F6: {struct Cyc_Absyn_Less_kb_struct _tmpBA3;struct Cyc_Absyn_Less_kb_struct*
_tmpBA2;struct Cyc_Absyn_Less_kb_struct*_tmp5BA=(_tmpBA2=_cycalloc(sizeof(*
_tmpBA2)),((_tmpBA2[0]=((_tmpBA3.tag=2,((_tmpBA3.f1=0,((_tmpBA3.f2=(void*)((void*)
5),_tmpBA3)))))),_tmpBA2)));struct _dyneither_ptr _tmp5BB=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);void*_tmp5BC=Cyc_Parse_id2type(
_tmp5BB,(void*)_tmp5BA);{struct _tuple10*_tmpBAD;struct Cyc_Absyn_JoinEff_struct
_tmpBAC;struct Cyc_Absyn_JoinEff_struct*_tmpBAB;struct Cyc_List_List*_tmpBAA;yyval=
Cyc_YY50(((_tmpBAA=_cycalloc(sizeof(*_tmpBAA)),((_tmpBAA->hd=((_tmpBAD=_cycalloc(
sizeof(*_tmpBAD)),((_tmpBAD->f1=(void*)((_tmpBAB=_cycalloc(sizeof(*_tmpBAB)),((
_tmpBAB[0]=((_tmpBAC.tag=20,((_tmpBAC.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpBAC)))),_tmpBAB)))),((
_tmpBAD->f2=_tmp5BC,_tmpBAD)))))),((_tmpBAA->tl=0,_tmpBAA)))))));}break;}case 220:
_LL2F7: {struct Cyc_Absyn_Less_kb_struct _tmpBB0;struct Cyc_Absyn_Less_kb_struct*
_tmpBAF;struct Cyc_Absyn_Less_kb_struct*_tmp5C3=(_tmpBAF=_cycalloc(sizeof(*
_tmpBAF)),((_tmpBAF[0]=((_tmpBB0.tag=2,((_tmpBB0.f1=0,((_tmpBB0.f2=(void*)((void*)
5),_tmpBB0)))))),_tmpBAF)));struct _dyneither_ptr _tmp5C4=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);void*_tmp5C5=Cyc_Parse_id2type(
_tmp5C4,(void*)_tmp5C3);{struct _tuple10*_tmpBBA;struct Cyc_Absyn_JoinEff_struct
_tmpBB9;struct Cyc_Absyn_JoinEff_struct*_tmpBB8;struct Cyc_List_List*_tmpBB7;yyval=
Cyc_YY50(((_tmpBB7=_cycalloc(sizeof(*_tmpBB7)),((_tmpBB7->hd=((_tmpBBA=_cycalloc(
sizeof(*_tmpBBA)),((_tmpBBA->f1=(void*)((_tmpBB8=_cycalloc(sizeof(*_tmpBB8)),((
_tmpBB8[0]=((_tmpBB9.tag=20,((_tmpBB9.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmpBB9)))),_tmpBB8)))),((
_tmpBBA->f2=_tmp5C5,_tmpBBA)))))),((_tmpBB7->tl=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBB7)))))));}break;}case
221: _LL2F8: yyval=Cyc_YY31(0);break;case 222: _LL2F9:{const char*_tmpBBB;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpBBB="inject",_tag_dyneither(_tmpBBB,sizeof(char),7))))
!= 0){const char*_tmpBBC;Cyc_Parse_err(((_tmpBBC="missing type in function declaration",
_tag_dyneither(_tmpBBC,sizeof(char),37))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}yyval=Cyc_YY31(
1);break;case 223: _LL2FA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 224: _LL2FB: yyval=Cyc_YY40(Cyc_List_imp_append(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 225: _LL2FC:
yyval=Cyc_YY40(0);break;case 226: _LL2FD: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 227: _LL2FE:{struct Cyc_Absyn_RgnsEff_struct*
_tmpBC2;struct Cyc_Absyn_RgnsEff_struct _tmpBC1;struct Cyc_List_List*_tmpBC0;yyval=
Cyc_YY40(((_tmpBC0=_cycalloc(sizeof(*_tmpBC0)),((_tmpBC0->hd=(void*)((void*)((
_tmpBC2=_cycalloc(sizeof(*_tmpBC2)),((_tmpBC2[0]=((_tmpBC1.tag=21,((_tmpBC1.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpBC1)))),_tmpBC2))))),((_tmpBC0->tl=0,_tmpBC0)))))));}break;case 228: _LL2FF:
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),(void*)6,0);{struct Cyc_List_List*_tmpBC3;yyval=Cyc_YY40(((
_tmpBC3=_cycalloc(sizeof(*_tmpBC3)),((_tmpBC3->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBC3->tl=0,_tmpBC3)))))));}
break;case 229: _LL300: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)5,1);{struct Cyc_Absyn_AccessEff_struct*
_tmpBC9;struct Cyc_Absyn_AccessEff_struct _tmpBC8;struct Cyc_List_List*_tmpBC7;
yyval=Cyc_YY40(((_tmpBC7=_cycalloc(sizeof(*_tmpBC7)),((_tmpBC7->hd=(void*)((void*)((
_tmpBC9=_cycalloc(sizeof(*_tmpBC9)),((_tmpBC9[0]=((_tmpBC8.tag=19,((_tmpBC8.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpBC8)))),_tmpBC9))))),((_tmpBC7->tl=0,_tmpBC7)))))));}break;case 230: _LL301:
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),(void*)5,1);{struct Cyc_Absyn_AccessEff_struct*_tmpBCF;
struct Cyc_Absyn_AccessEff_struct _tmpBCE;struct Cyc_List_List*_tmpBCD;yyval=Cyc_YY40(((
_tmpBCD=_cycalloc(sizeof(*_tmpBCD)),((_tmpBCD->hd=(void*)((void*)((_tmpBCF=
_cycalloc(sizeof(*_tmpBCF)),((_tmpBCF[0]=((_tmpBCE.tag=19,((_tmpBCE.f1=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpBCE)))),_tmpBCF))))),((_tmpBCD->tl=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBCD)))))));}break;case
231: _LL302:{struct Cyc_List_List*_tmpBD0;yyval=Cyc_YY38(((_tmpBD0=_cycalloc(
sizeof(*_tmpBD0)),((_tmpBD0->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBD0->tl=0,_tmpBD0)))))));}
break;case 232: _LL303:{struct Cyc_List_List*_tmpBD1;yyval=Cyc_YY38(((_tmpBD1=
_cycalloc(sizeof(*_tmpBD1)),((_tmpBD1->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBD1->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpBD1)))))));}
break;case 233: _LL304: {struct _tuple22 _tmp5DB;struct Cyc_Absyn_Tqual _tmp5DC;struct
Cyc_List_List*_tmp5DD;struct Cyc_List_List*_tmp5DE;struct _tuple22*_tmp5DA=Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5DB=*_tmp5DA;
_tmp5DC=_tmp5DB.f1;_tmp5DD=_tmp5DB.f2;_tmp5DE=_tmp5DB.f3;if(_tmp5DC.loc == 0)
_tmp5DC.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp5E0;struct _tuple1*
_tmp5E1;struct Cyc_List_List*_tmp5E2;struct Cyc_Parse_Declarator*_tmp5DF=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E0=*_tmp5DF;
_tmp5E1=_tmp5E0.id;_tmp5E2=_tmp5E0.tms;{void*_tmp5E3=Cyc_Parse_speclist2typ(
_tmp5DD,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp5E5;void*_tmp5E6;struct
Cyc_List_List*_tmp5E7;struct Cyc_List_List*_tmp5E8;struct _tuple12 _tmp5E4=Cyc_Parse_apply_tms(
_tmp5DC,_tmp5E3,_tmp5DE,_tmp5E2);_tmp5E5=_tmp5E4.f1;_tmp5E6=_tmp5E4.f2;_tmp5E7=
_tmp5E4.f3;_tmp5E8=_tmp5E4.f4;if(_tmp5E7 != 0){const char*_tmpBD2;Cyc_Parse_err(((
_tmpBD2="parameter with bad type params",_tag_dyneither(_tmpBD2,sizeof(char),31))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}if(Cyc_Absyn_is_qvar_qualified(_tmp5E1)){const char*
_tmpBD3;Cyc_Parse_err(((_tmpBD3="parameter cannot be qualified with a namespace",
_tag_dyneither(_tmpBD3,sizeof(char),47))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Core_Opt*
_tmpBD4;struct Cyc_Core_Opt*_tmp5EB=(struct Cyc_Core_Opt*)((_tmpBD4=_cycalloc(
sizeof(*_tmpBD4)),((_tmpBD4->v=(*_tmp5E1).f2,_tmpBD4))));if(_tmp5E8 != 0){const
char*_tmpBD7;void*_tmpBD6;(_tmpBD6=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpBD7="extra attributes on parameter, ignoring",
_tag_dyneither(_tmpBD7,sizeof(char),40))),_tag_dyneither(_tmpBD6,sizeof(void*),0)));}{
struct _tuple8*_tmpBD8;yyval=Cyc_YY37(((_tmpBD8=_cycalloc(sizeof(*_tmpBD8)),((
_tmpBD8->f1=_tmp5EB,((_tmpBD8->f2=_tmp5E5,((_tmpBD8->f3=_tmp5E6,_tmpBD8)))))))));}
break;}}}}case 234: _LL305: {struct _tuple22 _tmp5F1;struct Cyc_Absyn_Tqual _tmp5F2;
struct Cyc_List_List*_tmp5F3;struct Cyc_List_List*_tmp5F4;struct _tuple22*_tmp5F0=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F1=*
_tmp5F0;_tmp5F2=_tmp5F1.f1;_tmp5F3=_tmp5F1.f2;_tmp5F4=_tmp5F1.f3;if(_tmp5F2.loc
== 0)_tmp5F2.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);{void*_tmp5F5=
Cyc_Parse_speclist2typ(_tmp5F3,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp5F4 != 0){
const char*_tmpBDB;void*_tmpBDA;(_tmpBDA=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpBDB="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpBDB,sizeof(char),38))),_tag_dyneither(_tmpBDA,sizeof(void*),0)));}{
struct _tuple8*_tmpBDC;yyval=Cyc_YY37(((_tmpBDC=_cycalloc(sizeof(*_tmpBDC)),((
_tmpBDC->f1=0,((_tmpBDC->f2=_tmp5F2,((_tmpBDC->f3=_tmp5F5,_tmpBDC)))))))));}
break;}}case 235: _LL306: {struct _tuple22 _tmp5FA;struct Cyc_Absyn_Tqual _tmp5FB;
struct Cyc_List_List*_tmp5FC;struct Cyc_List_List*_tmp5FD;struct _tuple22*_tmp5F9=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5FA=*_tmp5F9;_tmp5FB=_tmp5FA.f1;_tmp5FC=_tmp5FA.f2;_tmp5FD=_tmp5FA.f3;if(
_tmp5FB.loc == 0)_tmp5FB.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{void*_tmp5FE=
Cyc_Parse_speclist2typ(_tmp5FC,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp5FF=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp601;void*_tmp602;struct Cyc_List_List*_tmp603;struct Cyc_List_List*
_tmp604;struct _tuple12 _tmp600=Cyc_Parse_apply_tms(_tmp5FB,_tmp5FE,_tmp5FD,
_tmp5FF);_tmp601=_tmp600.f1;_tmp602=_tmp600.f2;_tmp603=_tmp600.f3;_tmp604=
_tmp600.f4;if(_tmp603 != 0){const char*_tmpBDF;void*_tmpBDE;(_tmpBDE=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpBDF="bad type parameters on formal argument, ignoring",
_tag_dyneither(_tmpBDF,sizeof(char),49))),_tag_dyneither(_tmpBDE,sizeof(void*),0)));}
if(_tmp604 != 0){const char*_tmpBE2;void*_tmpBE1;(_tmpBE1=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpBE2="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpBE2,sizeof(char),38))),_tag_dyneither(_tmpBE1,sizeof(void*),0)));}{
struct _tuple8*_tmpBE3;yyval=Cyc_YY37(((_tmpBE3=_cycalloc(sizeof(*_tmpBE3)),((
_tmpBE3->f1=0,((_tmpBE3->f2=_tmp601,((_tmpBE3->f3=_tmp602,_tmpBE3)))))))));}
break;}}case 236: _LL307: yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 237: _LL308:{struct _dyneither_ptr*_tmpBE6;struct Cyc_List_List*
_tmpBE5;yyval=Cyc_YY36(((_tmpBE5=_cycalloc(sizeof(*_tmpBE5)),((_tmpBE5->hd=((
_tmpBE6=_cycalloc(sizeof(*_tmpBE6)),((_tmpBE6[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBE6)))),((_tmpBE5->tl=0,
_tmpBE5)))))));}break;case 238: _LL309:{struct _dyneither_ptr*_tmpBE9;struct Cyc_List_List*
_tmpBE8;yyval=Cyc_YY36(((_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8->hd=((
_tmpBE9=_cycalloc(sizeof(*_tmpBE9)),((_tmpBE9[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBE9)))),((_tmpBE8->tl=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpBE8)))))));}break;case 239: _LL30A: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 240: _LL30B: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 241: _LL30C:{struct
Cyc_Absyn_UnresolvedMem_e_struct _tmpBEC;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpBEB;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpBEB=_cycalloc(sizeof(*
_tmpBEB)),((_tmpBEB[0]=((_tmpBEC.tag=37,((_tmpBEC.f1=0,((_tmpBEC.f2=0,_tmpBEC)))))),
_tmpBEB)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 242: _LL30D:{struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpBEF;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpBEE;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpBEE=_cycalloc(sizeof(*_tmpBEE)),((_tmpBEE[0]=((_tmpBEF.tag=37,((
_tmpBEF.f1=0,((_tmpBEF.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),
_tmpBEF)))))),_tmpBEE)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 243:
_LL30E:{struct Cyc_Absyn_UnresolvedMem_e_struct _tmpBF2;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpBF1;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpBF1=_cycalloc(sizeof(*
_tmpBF1)),((_tmpBF1[0]=((_tmpBF2.tag=37,((_tmpBF2.f1=0,((_tmpBF2.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),_tmpBF2)))))),_tmpBF1)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 244: _LL30F: {struct _dyneither_ptr*
_tmpBF5;struct _tuple1*_tmpBF4;struct Cyc_Absyn_Vardecl*_tmp614=Cyc_Absyn_new_vardecl(((
_tmpBF4=_cycalloc(sizeof(*_tmpBF4)),((_tmpBF4->f1=Cyc_Absyn_Loc_n,((_tmpBF4->f2=((
_tmpBF5=_cycalloc(sizeof(*_tmpBF5)),((_tmpBF5[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpBF5)))),_tmpBF4)))))),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp614->tq).real_const=
1;{struct Cyc_Absyn_Comprehension_e_struct _tmpBF8;struct Cyc_Absyn_Comprehension_e_struct*
_tmpBF7;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpBF7=_cycalloc(sizeof(*
_tmpBF7)),((_tmpBF7[0]=((_tmpBF8.tag=29,((_tmpBF8.f1=_tmp614,((_tmpBF8.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpBF8.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpBF8.f4=0,
_tmpBF8)))))))))),_tmpBF7)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
245: _LL310:{struct _tuple26*_tmpBFB;struct Cyc_List_List*_tmpBFA;yyval=Cyc_YY6(((
_tmpBFA=_cycalloc(sizeof(*_tmpBFA)),((_tmpBFA->hd=((_tmpBFB=_cycalloc(sizeof(*
_tmpBFB)),((_tmpBFB->f1=0,((_tmpBFB->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBFB)))))),((_tmpBFA->tl=
0,_tmpBFA)))))));}break;case 246: _LL311:{struct _tuple26*_tmpBFE;struct Cyc_List_List*
_tmpBFD;yyval=Cyc_YY6(((_tmpBFD=_cycalloc(sizeof(*_tmpBFD)),((_tmpBFD->hd=((
_tmpBFE=_cycalloc(sizeof(*_tmpBFE)),((_tmpBFE->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpBFE->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBFE)))))),((
_tmpBFD->tl=0,_tmpBFD)))))));}break;case 247: _LL312:{struct _tuple26*_tmpC01;
struct Cyc_List_List*_tmpC00;yyval=Cyc_YY6(((_tmpC00=_cycalloc(sizeof(*_tmpC00)),((
_tmpC00->hd=((_tmpC01=_cycalloc(sizeof(*_tmpC01)),((_tmpC01->f1=0,((_tmpC01->f2=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC01)))))),((
_tmpC00->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]),_tmpC00)))))));}break;case 248: _LL313:{struct _tuple26*_tmpC04;struct Cyc_List_List*
_tmpC03;yyval=Cyc_YY6(((_tmpC03=_cycalloc(sizeof(*_tmpC03)),((_tmpC03->hd=((
_tmpC04=_cycalloc(sizeof(*_tmpC04)),((_tmpC04->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpC04->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC04)))))),((
_tmpC03->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]),_tmpC03)))))));}break;case 249: _LL314: yyval=Cyc_YY41(Cyc_List_imp_rev(Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 250:
_LL315:{struct Cyc_List_List*_tmpC05;yyval=Cyc_YY41(((_tmpC05=_cycalloc(sizeof(*
_tmpC05)),((_tmpC05->hd=(void*)Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC05->tl=0,_tmpC05)))))));}
break;case 251: _LL316:{struct Cyc_List_List*_tmpC06;yyval=Cyc_YY41(((_tmpC06=
_cycalloc(sizeof(*_tmpC06)),((_tmpC06->hd=(void*)Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC06->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC06)))))));}
break;case 252: _LL317:{struct Cyc_Absyn_ArrayElement_struct _tmpC09;struct Cyc_Absyn_ArrayElement_struct*
_tmpC08;yyval=Cyc_YY42((void*)((_tmpC08=_cycalloc(sizeof(*_tmpC08)),((_tmpC08[0]=((
_tmpC09.tag=0,((_tmpC09.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpC09)))),_tmpC08)))));}break;case 253: _LL318:{struct
Cyc_Absyn_FieldName_struct _tmpC0F;struct _dyneither_ptr*_tmpC0E;struct Cyc_Absyn_FieldName_struct*
_tmpC0D;yyval=Cyc_YY42((void*)((_tmpC0D=_cycalloc(sizeof(*_tmpC0D)),((_tmpC0D[0]=((
_tmpC0F.tag=1,((_tmpC0F.f1=((_tmpC0E=_cycalloc(sizeof(*_tmpC0E)),((_tmpC0E[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpC0E)))),_tmpC0F)))),_tmpC0D)))));}break;case 254: _LL319: {struct _tuple22
_tmp629;struct Cyc_Absyn_Tqual _tmp62A;struct Cyc_List_List*_tmp62B;struct Cyc_List_List*
_tmp62C;struct _tuple22*_tmp628=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp629=*_tmp628;_tmp62A=
_tmp629.f1;_tmp62B=_tmp629.f2;_tmp62C=_tmp629.f3;{void*_tmp62D=Cyc_Parse_speclist2typ(
_tmp62B,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp62C != 0){const char*_tmpC12;void*_tmpC11;(
_tmpC11=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC12="ignoring attributes in type",
_tag_dyneither(_tmpC12,sizeof(char),28))),_tag_dyneither(_tmpC11,sizeof(void*),0)));}{
struct _tuple8*_tmpC13;yyval=Cyc_YY37(((_tmpC13=_cycalloc(sizeof(*_tmpC13)),((
_tmpC13->f1=0,((_tmpC13->f2=_tmp62A,((_tmpC13->f3=_tmp62D,_tmpC13)))))))));}
break;}}case 255: _LL31A: {struct _tuple22 _tmp632;struct Cyc_Absyn_Tqual _tmp633;
struct Cyc_List_List*_tmp634;struct Cyc_List_List*_tmp635;struct _tuple22*_tmp631=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp632=*_tmp631;_tmp633=_tmp632.f1;_tmp634=_tmp632.f2;_tmp635=_tmp632.f3;{void*
_tmp636=Cyc_Parse_speclist2typ(_tmp634,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp637=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct _tuple12 _tmp638=Cyc_Parse_apply_tms(_tmp633,_tmp636,_tmp635,_tmp637);if(
_tmp638.f3 != 0){const char*_tmpC16;void*_tmpC15;(_tmpC15=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC16="bad type params, ignoring",
_tag_dyneither(_tmpC16,sizeof(char),26))),_tag_dyneither(_tmpC15,sizeof(void*),0)));}
if(_tmp638.f4 != 0){const char*_tmpC19;void*_tmpC18;(_tmpC18=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC19="bad specifiers, ignoring",
_tag_dyneither(_tmpC19,sizeof(char),25))),_tag_dyneither(_tmpC18,sizeof(void*),0)));}{
struct _tuple8*_tmpC1A;yyval=Cyc_YY37(((_tmpC1A=_cycalloc(sizeof(*_tmpC1A)),((
_tmpC1A->f1=0,((_tmpC1A->f2=_tmp638.f1,((_tmpC1A->f3=_tmp638.f2,_tmpC1A)))))))));}
break;}}case 256: _LL31B: yyval=Cyc_YY44((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);break;case 257: _LL31C:{
struct Cyc_Absyn_JoinEff_struct _tmpC1D;struct Cyc_Absyn_JoinEff_struct*_tmpC1C;
yyval=Cyc_YY44((void*)((_tmpC1C=_cycalloc(sizeof(*_tmpC1C)),((_tmpC1C[0]=((
_tmpC1D.tag=20,((_tmpC1D.f1=0,_tmpC1D)))),_tmpC1C)))));}break;case 258: _LL31D:{
struct Cyc_Absyn_JoinEff_struct _tmpC20;struct Cyc_Absyn_JoinEff_struct*_tmpC1F;
yyval=Cyc_YY44((void*)((_tmpC1F=_cycalloc(sizeof(*_tmpC1F)),((_tmpC1F[0]=((
_tmpC20.tag=20,((_tmpC20.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpC20)))),_tmpC1F)))));}break;case 259: _LL31E:{struct
Cyc_Absyn_RgnsEff_struct _tmpC23;struct Cyc_Absyn_RgnsEff_struct*_tmpC22;yyval=Cyc_YY44((
void*)((_tmpC22=_cycalloc(sizeof(*_tmpC22)),((_tmpC22[0]=((_tmpC23.tag=21,((
_tmpC23.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpC23)))),_tmpC22)))));}break;case 260: _LL31F:{struct Cyc_Absyn_JoinEff_struct
_tmpC29;struct Cyc_List_List*_tmpC28;struct Cyc_Absyn_JoinEff_struct*_tmpC27;yyval=
Cyc_YY44((void*)((_tmpC27=_cycalloc(sizeof(*_tmpC27)),((_tmpC27[0]=((_tmpC29.tag=
20,((_tmpC29.f1=((_tmpC28=_cycalloc(sizeof(*_tmpC28)),((_tmpC28->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC28->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC28)))))),_tmpC29)))),
_tmpC27)))));}break;case 261: _LL320:{struct Cyc_List_List*_tmpC2A;yyval=Cyc_YY40(((
_tmpC2A=_cycalloc(sizeof(*_tmpC2A)),((_tmpC2A->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC2A->tl=0,_tmpC2A)))))));}
break;case 262: _LL321:{struct Cyc_List_List*_tmpC2B;yyval=Cyc_YY40(((_tmpC2B=
_cycalloc(sizeof(*_tmpC2B)),((_tmpC2B->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC2B->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC2B)))))));}
break;case 263: _LL322:{struct Cyc_Parse_Abstractdeclarator*_tmpC2C;yyval=Cyc_YY30(((
_tmpC2C=_cycalloc(sizeof(*_tmpC2C)),((_tmpC2C->tms=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC2C)))));}break;case 264:
_LL323: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
265: _LL324:{struct Cyc_Parse_Abstractdeclarator*_tmpC2D;yyval=Cyc_YY30(((_tmpC2D=
_cycalloc(sizeof(*_tmpC2D)),((_tmpC2D->tms=Cyc_List_imp_append(Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpC2D)))));}
break;case 266: _LL325: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 267: _LL326:{struct Cyc_List_List*_tmpC37;struct Cyc_Absyn_Carray_mod_struct
_tmpC36;struct Cyc_Absyn_Carray_mod_struct*_tmpC35;struct Cyc_Parse_Abstractdeclarator*
_tmpC34;yyval=Cyc_YY30(((_tmpC34=_cycalloc(sizeof(*_tmpC34)),((_tmpC34->tms=((
_tmpC37=_cycalloc(sizeof(*_tmpC37)),((_tmpC37->hd=(void*)((void*)((_tmpC35=
_cycalloc(sizeof(*_tmpC35)),((_tmpC35[0]=((_tmpC36.tag=0,((_tmpC36.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC36.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC36)))))),
_tmpC35))))),((_tmpC37->tl=0,_tmpC37)))))),_tmpC34)))));}break;case 268: _LL327:{
struct Cyc_List_List*_tmpC41;struct Cyc_Absyn_Carray_mod_struct _tmpC40;struct Cyc_Absyn_Carray_mod_struct*
_tmpC3F;struct Cyc_Parse_Abstractdeclarator*_tmpC3E;yyval=Cyc_YY30(((_tmpC3E=
_cycalloc(sizeof(*_tmpC3E)),((_tmpC3E->tms=((_tmpC41=_cycalloc(sizeof(*_tmpC41)),((
_tmpC41->hd=(void*)((void*)((_tmpC3F=_cycalloc(sizeof(*_tmpC3F)),((_tmpC3F[0]=((
_tmpC40.tag=0,((_tmpC40.f1=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpC40.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC40)))))),
_tmpC3F))))),((_tmpC41->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpC41)))))),_tmpC3E)))));}break;case 269: _LL328:{
struct Cyc_List_List*_tmpC4B;struct Cyc_Absyn_ConstArray_mod_struct _tmpC4A;struct
Cyc_Absyn_ConstArray_mod_struct*_tmpC49;struct Cyc_Parse_Abstractdeclarator*
_tmpC48;yyval=Cyc_YY30(((_tmpC48=_cycalloc(sizeof(*_tmpC48)),((_tmpC48->tms=((
_tmpC4B=_cycalloc(sizeof(*_tmpC4B)),((_tmpC4B->hd=(void*)((void*)((_tmpC49=
_cycalloc(sizeof(*_tmpC49)),((_tmpC49[0]=((_tmpC4A.tag=1,((_tmpC4A.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC4A.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC4A.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC4A)))))))),
_tmpC49))))),((_tmpC4B->tl=0,_tmpC4B)))))),_tmpC48)))));}break;case 270: _LL329:{
struct Cyc_List_List*_tmpC55;struct Cyc_Absyn_ConstArray_mod_struct _tmpC54;struct
Cyc_Absyn_ConstArray_mod_struct*_tmpC53;struct Cyc_Parse_Abstractdeclarator*
_tmpC52;yyval=Cyc_YY30(((_tmpC52=_cycalloc(sizeof(*_tmpC52)),((_tmpC52->tms=((
_tmpC55=_cycalloc(sizeof(*_tmpC55)),((_tmpC55->hd=(void*)((void*)((_tmpC53=
_cycalloc(sizeof(*_tmpC53)),((_tmpC53[0]=((_tmpC54.tag=1,((_tmpC54.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC54.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC54.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC54)))))))),
_tmpC53))))),((_tmpC55->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpC55)))))),_tmpC52)))));}break;case 271: _LL32A:{
struct Cyc_List_List*_tmpC6A;struct Cyc_Absyn_Function_mod_struct _tmpC69;struct Cyc_Absyn_WithTypes_struct
_tmpC68;struct Cyc_Absyn_WithTypes_struct*_tmpC67;struct Cyc_Absyn_Function_mod_struct*
_tmpC66;struct Cyc_Parse_Abstractdeclarator*_tmpC65;yyval=Cyc_YY30(((_tmpC65=
_cycalloc(sizeof(*_tmpC65)),((_tmpC65->tms=((_tmpC6A=_cycalloc(sizeof(*_tmpC6A)),((
_tmpC6A->hd=(void*)((void*)((_tmpC66=_cycalloc(sizeof(*_tmpC66)),((_tmpC66[0]=((
_tmpC69.tag=3,((_tmpC69.f1=(void*)((void*)((_tmpC67=_cycalloc(sizeof(*_tmpC67)),((
_tmpC67[0]=((_tmpC68.tag=1,((_tmpC68.f1=0,((_tmpC68.f2=0,((_tmpC68.f3=0,((
_tmpC68.f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]),((_tmpC68.f5=Cyc_yyget_YY50(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpC68)))))))))))),_tmpC67))))),_tmpC69)))),_tmpC66))))),((
_tmpC6A->tl=0,_tmpC6A)))))),_tmpC65)))));}break;case 272: _LL32B: {struct _tuple23
_tmp662;struct Cyc_List_List*_tmp663;int _tmp664;struct Cyc_Absyn_VarargInfo*
_tmp665;struct Cyc_Core_Opt*_tmp666;struct Cyc_List_List*_tmp667;struct _tuple23*
_tmp661=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp662=*_tmp661;_tmp663=_tmp662.f1;_tmp664=_tmp662.f2;_tmp665=_tmp662.f3;
_tmp666=_tmp662.f4;_tmp667=_tmp662.f5;{struct Cyc_List_List*_tmpC7F;struct Cyc_Absyn_Function_mod_struct
_tmpC7E;struct Cyc_Absyn_WithTypes_struct _tmpC7D;struct Cyc_Absyn_WithTypes_struct*
_tmpC7C;struct Cyc_Absyn_Function_mod_struct*_tmpC7B;struct Cyc_Parse_Abstractdeclarator*
_tmpC7A;yyval=Cyc_YY30(((_tmpC7A=_cycalloc(sizeof(*_tmpC7A)),((_tmpC7A->tms=((
_tmpC7F=_cycalloc(sizeof(*_tmpC7F)),((_tmpC7F->hd=(void*)((void*)((_tmpC7B=
_cycalloc(sizeof(*_tmpC7B)),((_tmpC7B[0]=((_tmpC7E.tag=3,((_tmpC7E.f1=(void*)((
void*)((_tmpC7C=_cycalloc(sizeof(*_tmpC7C)),((_tmpC7C[0]=((_tmpC7D.tag=1,((
_tmpC7D.f1=_tmp663,((_tmpC7D.f2=_tmp664,((_tmpC7D.f3=_tmp665,((_tmpC7D.f4=
_tmp666,((_tmpC7D.f5=_tmp667,_tmpC7D)))))))))))),_tmpC7C))))),_tmpC7E)))),
_tmpC7B))))),((_tmpC7F->tl=0,_tmpC7F)))))),_tmpC7A)))));}break;}case 273: _LL32C:{
struct Cyc_List_List*_tmpC94;struct Cyc_Absyn_Function_mod_struct _tmpC93;struct Cyc_Absyn_WithTypes_struct
_tmpC92;struct Cyc_Absyn_WithTypes_struct*_tmpC91;struct Cyc_Absyn_Function_mod_struct*
_tmpC90;struct Cyc_Parse_Abstractdeclarator*_tmpC8F;yyval=Cyc_YY30(((_tmpC8F=
_cycalloc(sizeof(*_tmpC8F)),((_tmpC8F->tms=((_tmpC94=_cycalloc(sizeof(*_tmpC94)),((
_tmpC94->hd=(void*)((void*)((_tmpC90=_cycalloc(sizeof(*_tmpC90)),((_tmpC90[0]=((
_tmpC93.tag=3,((_tmpC93.f1=(void*)((void*)((_tmpC91=_cycalloc(sizeof(*_tmpC91)),((
_tmpC91[0]=((_tmpC92.tag=1,((_tmpC92.f1=0,((_tmpC92.f2=0,((_tmpC92.f3=0,((
_tmpC92.f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]),((_tmpC92.f5=Cyc_yyget_YY50(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpC92)))))))))))),_tmpC91))))),_tmpC93)))),_tmpC90))))),((
_tmpC94->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->tms,_tmpC94)))))),_tmpC8F)))));}break;case 274: _LL32D: {
struct _tuple23 _tmp675;struct Cyc_List_List*_tmp676;int _tmp677;struct Cyc_Absyn_VarargInfo*
_tmp678;struct Cyc_Core_Opt*_tmp679;struct Cyc_List_List*_tmp67A;struct _tuple23*
_tmp674=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp675=*_tmp674;_tmp676=_tmp675.f1;_tmp677=_tmp675.f2;_tmp678=_tmp675.f3;
_tmp679=_tmp675.f4;_tmp67A=_tmp675.f5;{struct Cyc_List_List*_tmpCA9;struct Cyc_Absyn_Function_mod_struct
_tmpCA8;struct Cyc_Absyn_WithTypes_struct _tmpCA7;struct Cyc_Absyn_WithTypes_struct*
_tmpCA6;struct Cyc_Absyn_Function_mod_struct*_tmpCA5;struct Cyc_Parse_Abstractdeclarator*
_tmpCA4;yyval=Cyc_YY30(((_tmpCA4=_cycalloc(sizeof(*_tmpCA4)),((_tmpCA4->tms=((
_tmpCA9=_cycalloc(sizeof(*_tmpCA9)),((_tmpCA9->hd=(void*)((void*)((_tmpCA5=
_cycalloc(sizeof(*_tmpCA5)),((_tmpCA5[0]=((_tmpCA8.tag=3,((_tmpCA8.f1=(void*)((
void*)((_tmpCA6=_cycalloc(sizeof(*_tmpCA6)),((_tmpCA6[0]=((_tmpCA7.tag=1,((
_tmpCA7.f1=_tmp676,((_tmpCA7.f2=_tmp677,((_tmpCA7.f3=_tmp678,((_tmpCA7.f4=
_tmp679,((_tmpCA7.f5=_tmp67A,_tmpCA7)))))))))))),_tmpCA6))))),_tmpCA8)))),
_tmpCA5))))),((_tmpCA9->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpCA9)))))),_tmpCA4)))));}break;}case 275: _LL32E: {
struct Cyc_List_List*_tmp681=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{
struct Cyc_List_List*_tmpCB3;struct Cyc_Absyn_TypeParams_mod_struct _tmpCB2;struct
Cyc_Absyn_TypeParams_mod_struct*_tmpCB1;struct Cyc_Parse_Abstractdeclarator*
_tmpCB0;yyval=Cyc_YY30(((_tmpCB0=_cycalloc(sizeof(*_tmpCB0)),((_tmpCB0->tms=((
_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((_tmpCB3->hd=(void*)((void*)((_tmpCB1=
_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1[0]=((_tmpCB2.tag=4,((_tmpCB2.f1=_tmp681,((
_tmpCB2.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpCB2.f3=0,_tmpCB2)))))))),_tmpCB1))))),((_tmpCB3->tl=(
Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpCB3)))))),_tmpCB0)))));}break;}case 276: _LL32F:{struct Cyc_List_List*_tmpCBD;
struct Cyc_Absyn_Attributes_mod_struct _tmpCBC;struct Cyc_Absyn_Attributes_mod_struct*
_tmpCBB;struct Cyc_Parse_Abstractdeclarator*_tmpCBA;yyval=Cyc_YY30(((_tmpCBA=
_cycalloc(sizeof(*_tmpCBA)),((_tmpCBA->tms=((_tmpCBD=_cycalloc(sizeof(*_tmpCBD)),((
_tmpCBD->hd=(void*)((void*)((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB[0]=((
_tmpCBC.tag=5,((_tmpCBC.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpCBC.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCBC)))))),_tmpCBB))))),((
_tmpCBD->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpCBD)))))),_tmpCBA)))));}break;case 277: _LL330: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 278: _LL331:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 279:
_LL332: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
280: _LL333: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 281: _LL334: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 282: _LL335: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 283: _LL336:{struct Cyc_Absyn_ResetRegion_s_struct _tmpCC0;
struct Cyc_Absyn_ResetRegion_s_struct*_tmpCBF;yyval=Cyc_YY9(Cyc_Absyn_new_stmt((
void*)((_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF[0]=((_tmpCC0.tag=15,((
_tmpCC0.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]),_tmpCC0)))),_tmpCBF)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 284:
_LL337: yyval=Cyc_YY4(0);break;case 285: _LL338:{const char*_tmpCC1;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),((_tmpCC1="open",_tag_dyneither(_tmpCC1,sizeof(char),5))))
!= 0){const char*_tmpCC2;Cyc_Parse_err(((_tmpCC2="expecting `open'",
_tag_dyneither(_tmpCC2,sizeof(char),17))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}yyval=Cyc_YY4((
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));break;case 286: _LL339:{struct Cyc_Absyn_Label_s_struct _tmpCC8;
struct _dyneither_ptr*_tmpCC7;struct Cyc_Absyn_Label_s_struct*_tmpCC6;yyval=Cyc_YY9(
Cyc_Absyn_new_stmt((void*)((_tmpCC6=_cycalloc(sizeof(*_tmpCC6)),((_tmpCC6[0]=((
_tmpCC8.tag=12,((_tmpCC8.f1=((_tmpCC7=_cycalloc(sizeof(*_tmpCC7)),((_tmpCC7[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpCC7)))),((_tmpCC8.f2=Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpCC8)))))),_tmpCC6)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 287:
_LL33A: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 288:
_LL33B: yyval=Cyc_YY9(Cyc_Absyn_exp_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 289:
_LL33C: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 290:
_LL33D: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 291: _LL33E: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0)));
break;case 292: _LL33F: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 293: _LL340:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 294:
_LL341: yyval=Cyc_YY9(Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 295:
_LL342:{struct Cyc_Absyn_Fn_d_struct _tmpCCB;struct Cyc_Absyn_Fn_d_struct*_tmpCCA;
yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpCCA=
_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA[0]=((_tmpCCB.tag=1,((_tmpCCB.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCCB)))),_tmpCCA)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0)));}break;case 296: _LL343:{
struct Cyc_Absyn_Fn_d_struct _tmpCCE;struct Cyc_Absyn_Fn_d_struct*_tmpCCD;yyval=Cyc_YY9(
Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpCCD=_cycalloc(sizeof(*
_tmpCCD)),((_tmpCCD[0]=((_tmpCCE.tag=1,((_tmpCCE.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpCCE)))),_tmpCCD)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));}break;case 297: _LL344: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 298: _LL345: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 299:
_LL346: yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 300:
_LL347: {struct Cyc_Absyn_UnknownId_e_struct _tmpCD1;struct Cyc_Absyn_UnknownId_e_struct*
_tmpCD0;struct Cyc_Absyn_Exp*_tmp695=Cyc_Absyn_new_exp((void*)((_tmpCD0=_cycalloc(
sizeof(*_tmpCD0)),((_tmpCD0[0]=((_tmpCD1.tag=2,((_tmpCD1.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpCD1)))),_tmpCD0)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(_tmp695,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 301:
_LL348: {struct Cyc_Absyn_Exp*_tmp698=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(
Cyc_Absyn_switch_stmt(_tmp698,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 302:
_LL349: yyval=Cyc_YY9(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 303:
_LL34A: yyval=Cyc_YY10(0);break;case 304: _LL34B:{struct Cyc_Absyn_Switch_clause*
_tmpCD4;struct Cyc_List_List*_tmpCD3;yyval=Cyc_YY10(((_tmpCD3=_cycalloc(sizeof(*
_tmpCD3)),((_tmpCD3->hd=((_tmpCD4=_cycalloc(sizeof(*_tmpCD4)),((_tmpCD4->pattern=
Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)),((_tmpCD4->pat_vars=
0,((_tmpCD4->where_clause=0,((_tmpCD4->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCD4->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpCD4)))))))))))),((
_tmpCD3->tl=0,_tmpCD3)))))));}break;case 305: _LL34C:{struct Cyc_Absyn_Switch_clause*
_tmpCD7;struct Cyc_List_List*_tmpCD6;yyval=Cyc_YY10(((_tmpCD6=_cycalloc(sizeof(*
_tmpCD6)),((_tmpCD6->hd=((_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpCD7->pat_vars=0,((_tmpCD7->where_clause=0,((_tmpCD7->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),((_tmpCD7->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpCD7)))))))))))),((
_tmpCD6->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpCD6)))))));}break;case 306: _LL34D:{struct Cyc_Absyn_Switch_clause*
_tmpCDA;struct Cyc_List_List*_tmpCD9;yyval=Cyc_YY10(((_tmpCD9=_cycalloc(sizeof(*
_tmpCD9)),((_tmpCD9->hd=((_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpCDA->pat_vars=0,((_tmpCDA->where_clause=0,((_tmpCDA->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCDA->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpCDA)))))))))))),((
_tmpCD9->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpCD9)))))));}break;case 307: _LL34E:{struct Cyc_Absyn_Switch_clause*
_tmpCDD;struct Cyc_List_List*_tmpCDC;yyval=Cyc_YY10(((_tmpCDC=_cycalloc(sizeof(*
_tmpCDC)),((_tmpCDC->hd=((_tmpCDD=_cycalloc(sizeof(*_tmpCDD)),((_tmpCDD->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((
_tmpCDD->pat_vars=0,((_tmpCDD->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpCDD->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),((_tmpCDD->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpCDD)))))))))))),((
_tmpCDC->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpCDC)))))));}break;case 308: _LL34F:{struct Cyc_Absyn_Switch_clause*
_tmpCE0;struct Cyc_List_List*_tmpCDF;yyval=Cyc_YY10(((_tmpCDF=_cycalloc(sizeof(*
_tmpCDF)),((_tmpCDF->hd=((_tmpCE0=_cycalloc(sizeof(*_tmpCE0)),((_tmpCE0->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),((
_tmpCE0->pat_vars=0,((_tmpCE0->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpCE0->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCE0->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpCE0)))))))))))),((
_tmpCDF->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpCDF)))))));}break;case 309: _LL350: yyval=Cyc_YY9(Cyc_Absyn_while_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 310:
_LL351: yyval=Cyc_YY9(Cyc_Absyn_do_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 311:
_LL352: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 312:
_LL353: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 313:
_LL354: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 314:
_LL355: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
315: _LL356: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 316: _LL357: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 317:
_LL358: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 318:
_LL359: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 319:
_LL35A: {struct Cyc_List_List*_tmp6A3=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp6A4=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp6A3,_tmp6A4));break;}case 320: _LL35B: {struct
Cyc_List_List*_tmp6A5=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp6A6=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp6A5,
_tmp6A6));break;}case 321: _LL35C: {struct Cyc_List_List*_tmp6A7=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp6A8=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp6A7,_tmp6A8));break;}case 322: _LL35D: {struct
Cyc_List_List*_tmp6A9=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp6AA=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp6A9,_tmp6AA));break;}case 323: _LL35E:{struct
_dyneither_ptr*_tmpCE1;yyval=Cyc_YY9(Cyc_Absyn_goto_stmt(((_tmpCE1=_cycalloc(
sizeof(*_tmpCE1)),((_tmpCE1[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpCE1)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));}break;case
324: _LL35F: yyval=Cyc_YY9(Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
325: _LL360: yyval=Cyc_YY9(Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
326: _LL361: yyval=Cyc_YY9(Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
327: _LL362: yyval=Cyc_YY9(Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
328: _LL363: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
329: _LL364: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)));break;case
330: _LL365: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line)));break;case
331: _LL366: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 332: _LL367: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 333: _LL368: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 334: _LL369: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 335: _LL36A: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 336:
_LL36B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
337: _LL36C: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 338:
_LL36D: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
339: _LL36E: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)14,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 340:
_LL36F: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
341: _LL370: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)15,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 342:
_LL371: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
343: _LL372: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)13,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 344:
_LL373: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
345: _LL374: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 346:
_LL375: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 347:
_LL376: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
348: _LL377: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 349:
_LL378: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 350:
_LL379: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 351:
_LL37A: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 352:
_LL37B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
353: _LL37C: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)16,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 354:
_LL37D: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)17,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 355:
_LL37E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
356: _LL37F: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)0,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 357:
_LL380: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)2,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 358:
_LL381: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
359: _LL382: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)1,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 360:
_LL383: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)3,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 361:
_LL384: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)4,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 362:
_LL385: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
363: _LL386: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 364:
_LL387: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
365: _LL388: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 366: _LL389:
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 367:
_LL38A: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 368:
_LL38B: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 369:
_LL38C:{struct Cyc_Absyn_StructField_struct _tmpCE7;struct _dyneither_ptr*_tmpCE6;
struct Cyc_Absyn_StructField_struct*_tmpCE5;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpCE5=_cycalloc(sizeof(*_tmpCE5)),((_tmpCE5[0]=((_tmpCE7.tag=0,((
_tmpCE7.f1=((_tmpCE6=_cycalloc(sizeof(*_tmpCE6)),((_tmpCE6[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpCE6)))),_tmpCE7)))),
_tmpCE5)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));}break;case 370: _LL38D:{struct Cyc_Absyn_TupleIndex_struct
_tmpCEA;struct Cyc_Absyn_TupleIndex_struct*_tmpCE9;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)((_tmpCE9=_cycalloc_atomic(sizeof(*_tmpCE9)),((
_tmpCE9[0]=((_tmpCEA.tag=1,((_tmpCEA.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2,_tmpCEA)))),_tmpCE9)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));}break;case 371: _LL38E: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 372: _LL38F: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 373: _LL390:
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 374:
_LL391: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 375: _LL392: {
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);{void*_tmp6B1=e->r;union Cyc_Absyn_Cnst _tmp6B2;struct _tuple4
_tmp6B3;void*_tmp6B4;char _tmp6B5;union Cyc_Absyn_Cnst _tmp6B6;struct _tuple5 _tmp6B7;
void*_tmp6B8;short _tmp6B9;union Cyc_Absyn_Cnst _tmp6BA;struct _tuple6 _tmp6BB;void*
_tmp6BC;int _tmp6BD;union Cyc_Absyn_Cnst _tmp6BE;struct _dyneither_ptr _tmp6BF;union
Cyc_Absyn_Cnst _tmp6C0;int _tmp6C1;union Cyc_Absyn_Cnst _tmp6C2;struct _dyneither_ptr
_tmp6C3;union Cyc_Absyn_Cnst _tmp6C4;struct _tuple7 _tmp6C5;_LL395: if(*((int*)
_tmp6B1)!= 0)goto _LL397;_tmp6B2=((struct Cyc_Absyn_Const_e_struct*)_tmp6B1)->f1;
if((_tmp6B2.Char_c).tag != 2)goto _LL397;_tmp6B3=(struct _tuple4)(_tmp6B2.Char_c).val;
_tmp6B4=_tmp6B3.f1;_tmp6B5=_tmp6B3.f2;_LL396:{struct Cyc_Absyn_Char_p_struct
_tmpCED;struct Cyc_Absyn_Char_p_struct*_tmpCEC;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpCEC=_cycalloc_atomic(sizeof(*_tmpCEC)),((_tmpCEC[0]=((_tmpCED.tag=8,((
_tmpCED.f1=_tmp6B5,_tmpCED)))),_tmpCEC)))),e->loc));}goto _LL394;_LL397: if(*((int*)
_tmp6B1)!= 0)goto _LL399;_tmp6B6=((struct Cyc_Absyn_Const_e_struct*)_tmp6B1)->f1;
if((_tmp6B6.Short_c).tag != 3)goto _LL399;_tmp6B7=(struct _tuple5)(_tmp6B6.Short_c).val;
_tmp6B8=_tmp6B7.f1;_tmp6B9=_tmp6B7.f2;_LL398:{struct Cyc_Absyn_Int_p_struct
_tmpCF0;struct Cyc_Absyn_Int_p_struct*_tmpCEF;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpCEF=_cycalloc(sizeof(*_tmpCEF)),((_tmpCEF[0]=((_tmpCF0.tag=7,((
_tmpCF0.f1=(void*)_tmp6B8,((_tmpCF0.f2=(int)_tmp6B9,_tmpCF0)))))),_tmpCEF)))),e->loc));}
goto _LL394;_LL399: if(*((int*)_tmp6B1)!= 0)goto _LL39B;_tmp6BA=((struct Cyc_Absyn_Const_e_struct*)
_tmp6B1)->f1;if((_tmp6BA.Int_c).tag != 4)goto _LL39B;_tmp6BB=(struct _tuple6)(
_tmp6BA.Int_c).val;_tmp6BC=_tmp6BB.f1;_tmp6BD=_tmp6BB.f2;_LL39A:{struct Cyc_Absyn_Int_p_struct
_tmpCF3;struct Cyc_Absyn_Int_p_struct*_tmpCF2;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpCF2=_cycalloc(sizeof(*_tmpCF2)),((_tmpCF2[0]=((_tmpCF3.tag=7,((
_tmpCF3.f1=(void*)_tmp6BC,((_tmpCF3.f2=_tmp6BD,_tmpCF3)))))),_tmpCF2)))),e->loc));}
goto _LL394;_LL39B: if(*((int*)_tmp6B1)!= 0)goto _LL39D;_tmp6BE=((struct Cyc_Absyn_Const_e_struct*)
_tmp6B1)->f1;if((_tmp6BE.Float_c).tag != 6)goto _LL39D;_tmp6BF=(struct
_dyneither_ptr)(_tmp6BE.Float_c).val;_LL39C:{struct Cyc_Absyn_Float_p_struct
_tmpCF6;struct Cyc_Absyn_Float_p_struct*_tmpCF5;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpCF5=_cycalloc(sizeof(*_tmpCF5)),((_tmpCF5[0]=((_tmpCF6.tag=9,((
_tmpCF6.f1=_tmp6BF,_tmpCF6)))),_tmpCF5)))),e->loc));}goto _LL394;_LL39D: if(*((int*)
_tmp6B1)!= 0)goto _LL39F;_tmp6C0=((struct Cyc_Absyn_Const_e_struct*)_tmp6B1)->f1;
if((_tmp6C0.Null_c).tag != 1)goto _LL39F;_tmp6C1=(int)(_tmp6C0.Null_c).val;_LL39E:
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)1,e->loc));goto _LL394;_LL39F: if(*((int*)
_tmp6B1)!= 0)goto _LL3A1;_tmp6C2=((struct Cyc_Absyn_Const_e_struct*)_tmp6B1)->f1;
if((_tmp6C2.String_c).tag != 7)goto _LL3A1;_tmp6C3=(struct _dyneither_ptr)(_tmp6C2.String_c).val;
_LL3A0:{const char*_tmpCF7;Cyc_Parse_err(((_tmpCF7="strings cannot occur within patterns",
_tag_dyneither(_tmpCF7,sizeof(char),37))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}goto _LL394;
_LL3A1: if(*((int*)_tmp6B1)!= 0)goto _LL3A3;_tmp6C4=((struct Cyc_Absyn_Const_e_struct*)
_tmp6B1)->f1;if((_tmp6C4.LongLong_c).tag != 5)goto _LL3A3;_tmp6C5=(struct _tuple7)(
_tmp6C4.LongLong_c).val;_LL3A2:{const char*_tmpCF8;Cyc_Parse_unimp(((_tmpCF8="long long's in patterns",
_tag_dyneither(_tmpCF8,sizeof(char),24))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}goto _LL394;
_LL3A3:;_LL3A4: {const char*_tmpCF9;Cyc_Parse_err(((_tmpCF9="bad constant in case",
_tag_dyneither(_tmpCF9,sizeof(char),21))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}_LL394:;}
break;}case 376: _LL393:{struct Cyc_Absyn_UnknownId_p_struct _tmpCFC;struct Cyc_Absyn_UnknownId_p_struct*
_tmpCFB;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpCFB=_cycalloc(sizeof(*
_tmpCFB)),((_tmpCFB[0]=((_tmpCFC.tag=12,((_tmpCFC.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCFC)))),_tmpCFB)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 377:
_LL3A5:{const char*_tmpCFD;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCFD="as",
_tag_dyneither(_tmpCFD,sizeof(char),3))))!= 0){const char*_tmpCFE;Cyc_Parse_err(((
_tmpCFE="expecting `as'",_tag_dyneither(_tmpCFE,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Var_p_struct
_tmpD08;struct _dyneither_ptr*_tmpD07;struct _tuple1*_tmpD06;struct Cyc_Absyn_Var_p_struct*
_tmpD05;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD05=_cycalloc(sizeof(*
_tmpD05)),((_tmpD05[0]=((_tmpD08.tag=0,((_tmpD08.f1=Cyc_Absyn_new_vardecl(((
_tmpD06=_cycalloc(sizeof(*_tmpD06)),((_tmpD06->f1=Cyc_Absyn_Loc_n,((_tmpD06->f2=((
_tmpD07=_cycalloc(sizeof(*_tmpD07)),((_tmpD07[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpD07)))),_tmpD06)))))),(
void*)0,0),((_tmpD08.f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpD08)))))),_tmpD05)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)));}break;case
378: _LL3A6: {struct Cyc_List_List*_tmp6DA;int _tmp6DB;struct _tuple19 _tmp6D9=*Cyc_yyget_YY12(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6DA=_tmp6D9.f1;
_tmp6DB=_tmp6D9.f2;{struct Cyc_Absyn_Tuple_p_struct _tmpD0B;struct Cyc_Absyn_Tuple_p_struct*
_tmpD0A;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD0A=_cycalloc(sizeof(*
_tmpD0A)),((_tmpD0A[0]=((_tmpD0B.tag=3,((_tmpD0B.f1=_tmp6DA,((_tmpD0B.f2=_tmp6DB,
_tmpD0B)))))),_tmpD0A)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
379: _LL3A7: {struct Cyc_List_List*_tmp6DF;int _tmp6E0;struct _tuple19 _tmp6DE=*Cyc_yyget_YY12(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6DF=_tmp6DE.f1;
_tmp6E0=_tmp6DE.f2;{struct Cyc_Absyn_UnknownCall_p_struct _tmpD0E;struct Cyc_Absyn_UnknownCall_p_struct*
_tmpD0D;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD0D=_cycalloc(sizeof(*
_tmpD0D)),((_tmpD0D[0]=((_tmpD0E.tag=13,((_tmpD0E.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpD0E.f2=_tmp6DF,((
_tmpD0E.f3=_tmp6E0,_tmpD0E)))))))),_tmpD0D)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
380: _LL3A8: {struct Cyc_List_List*_tmp6E4;int _tmp6E5;struct _tuple19 _tmp6E3=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6E4=_tmp6E3.f1;
_tmp6E5=_tmp6E3.f2;{struct Cyc_List_List*_tmp6E6=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct
_tmpD14;struct Cyc_Absyn_AggrInfo*_tmpD13;struct Cyc_Absyn_Aggr_p_struct*_tmpD12;
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD12=_cycalloc(sizeof(*_tmpD12)),((
_tmpD12[0]=((_tmpD14.tag=5,((_tmpD14.f1=((_tmpD13=_cycalloc(sizeof(*_tmpD13)),((
_tmpD13->aggr_info=Cyc_Absyn_UnknownAggr((void*)0,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),0),((_tmpD13->targs=0,
_tmpD13)))))),((_tmpD14.f2=_tmp6E6,((_tmpD14.f3=_tmp6E4,((_tmpD14.f4=_tmp6E5,
_tmpD14)))))))))),_tmpD12)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}}case
381: _LL3A9: {struct Cyc_List_List*_tmp6EB;int _tmp6EC;struct _tuple19 _tmp6EA=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6EB=_tmp6EA.f1;
_tmp6EC=_tmp6EA.f2;{struct Cyc_List_List*_tmp6ED=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct
_tmpD17;struct Cyc_Absyn_Aggr_p_struct*_tmpD16;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpD16=_cycalloc(sizeof(*_tmpD16)),((_tmpD16[0]=((_tmpD17.tag=5,((
_tmpD17.f1=0,((_tmpD17.f2=_tmp6ED,((_tmpD17.f3=_tmp6EB,((_tmpD17.f4=_tmp6EC,
_tmpD17)))))))))),_tmpD16)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}}case
382: _LL3AA:{struct Cyc_Absyn_Pointer_p_struct _tmpD1A;struct Cyc_Absyn_Pointer_p_struct*
_tmpD19;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD19=_cycalloc(sizeof(*
_tmpD19)),((_tmpD19[0]=((_tmpD1A.tag=4,((_tmpD1A.f1=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD1A)))),_tmpD19)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 383:
_LL3AB:{struct Cyc_Absyn_Pointer_p_struct _tmpD24;struct Cyc_Absyn_Pointer_p_struct
_tmpD23;struct Cyc_Absyn_Pointer_p_struct*_tmpD22;struct Cyc_Absyn_Pointer_p_struct*
_tmpD21;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD21=_cycalloc(sizeof(*
_tmpD21)),((_tmpD21[0]=((_tmpD24.tag=4,((_tmpD24.f1=Cyc_Absyn_new_pat((void*)((
_tmpD22=_cycalloc(sizeof(*_tmpD22)),((_tmpD22[0]=((_tmpD23.tag=4,((_tmpD23.f1=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD23)))),
_tmpD22)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpD24)))),_tmpD21)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 384:
_LL3AC:{struct Cyc_Absyn_Reference_p_struct _tmpD2E;struct _dyneither_ptr*_tmpD2D;
struct _tuple1*_tmpD2C;struct Cyc_Absyn_Reference_p_struct*_tmpD2B;yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)((_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B[0]=((
_tmpD2E.tag=1,((_tmpD2E.f1=Cyc_Absyn_new_vardecl(((_tmpD2C=_cycalloc(sizeof(*
_tmpD2C)),((_tmpD2C->f1=Cyc_Absyn_Loc_n,((_tmpD2C->f2=((_tmpD2D=_cycalloc(
sizeof(*_tmpD2D)),((_tmpD2D[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD2D)))),_tmpD2C)))))),(
void*)0,0),((_tmpD2E.f2=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpD2E)))))),
_tmpD2B)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 385: _LL3AD:{const char*_tmpD2F;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((_tmpD2F="as",_tag_dyneither(_tmpD2F,sizeof(char),3))))
!= 0){const char*_tmpD30;Cyc_Parse_err(((_tmpD30="expecting `as'",_tag_dyneither(
_tmpD30,sizeof(char),15))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Reference_p_struct
_tmpD3A;struct _dyneither_ptr*_tmpD39;struct _tuple1*_tmpD38;struct Cyc_Absyn_Reference_p_struct*
_tmpD37;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD37=_cycalloc(sizeof(*
_tmpD37)),((_tmpD37[0]=((_tmpD3A.tag=1,((_tmpD3A.f1=Cyc_Absyn_new_vardecl(((
_tmpD38=_cycalloc(sizeof(*_tmpD38)),((_tmpD38->f1=Cyc_Absyn_Loc_n,((_tmpD38->f2=((
_tmpD39=_cycalloc(sizeof(*_tmpD39)),((_tmpD39[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpD39)))),_tmpD38)))))),(
void*)0,0),((_tmpD3A.f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpD3A)))))),_tmpD37)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)));}break;case
386: _LL3AE: {void*_tmp700=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Tcutil_kind_to_bound((
void*)7));{struct Cyc_Absyn_TagInt_p_struct _tmpD4B;struct Cyc_Absyn_TagType_struct
_tmpD4A;struct Cyc_Absyn_TagType_struct*_tmpD49;struct _dyneither_ptr*_tmpD48;
struct _tuple1*_tmpD47;struct Cyc_Absyn_TagInt_p_struct*_tmpD46;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46[0]=((_tmpD4B.tag=2,((
_tmpD4B.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp700),((
_tmpD4B.f2=Cyc_Absyn_new_vardecl(((_tmpD47=_cycalloc(sizeof(*_tmpD47)),((_tmpD47->f1=
Cyc_Absyn_Loc_n,((_tmpD47->f2=((_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpD48)))),_tmpD47)))))),(void*)((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49[
0]=((_tmpD4A.tag=18,((_tmpD4A.f1=(void*)_tmp700,_tmpD4A)))),_tmpD49)))),0),
_tmpD4B)))))),_tmpD46)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
387: _LL3AF: {struct Cyc_Absyn_Tvar*_tmp707=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound((
void*)7));{struct Cyc_Absyn_TagInt_p_struct _tmpD67;struct Cyc_Absyn_TagType_struct
_tmpD66;struct Cyc_Absyn_VarType_struct _tmpD65;struct Cyc_Absyn_VarType_struct*
_tmpD64;struct Cyc_Absyn_TagType_struct*_tmpD63;struct _dyneither_ptr*_tmpD62;
struct _tuple1*_tmpD61;struct Cyc_Absyn_TagInt_p_struct*_tmpD60;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpD60=_cycalloc(sizeof(*_tmpD60)),((_tmpD60[0]=((_tmpD67.tag=2,((
_tmpD67.f1=_tmp707,((_tmpD67.f2=Cyc_Absyn_new_vardecl(((_tmpD61=_cycalloc(
sizeof(*_tmpD61)),((_tmpD61->f1=Cyc_Absyn_Loc_n,((_tmpD61->f2=((_tmpD62=
_cycalloc(sizeof(*_tmpD62)),((_tmpD62[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpD62)))),_tmpD61)))))),(
void*)((_tmpD63=_cycalloc(sizeof(*_tmpD63)),((_tmpD63[0]=((_tmpD66.tag=18,((
_tmpD66.f1=(void*)((void*)((_tmpD64=_cycalloc(sizeof(*_tmpD64)),((_tmpD64[0]=((
_tmpD65.tag=1,((_tmpD65.f1=_tmp707,_tmpD65)))),_tmpD64))))),_tmpD66)))),_tmpD63)))),
0),_tmpD67)))))),_tmpD60)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
388: _LL3B0:{struct _tuple19*_tmpD68;yyval=Cyc_YY12(((_tmpD68=_cycalloc(sizeof(*
_tmpD68)),((_tmpD68->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),((
_tmpD68->f2=0,_tmpD68)))))));}break;case 389: _LL3B1:{struct _tuple19*_tmpD69;yyval=
Cyc_YY12(((_tmpD69=_cycalloc(sizeof(*_tmpD69)),((_tmpD69->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpD69->f2=1,_tmpD69)))))));}
break;case 390: _LL3B2:{struct _tuple19*_tmpD6A;yyval=Cyc_YY12(((_tmpD6A=_cycalloc(
sizeof(*_tmpD6A)),((_tmpD6A->f1=0,((_tmpD6A->f2=1,_tmpD6A)))))));}break;case 391:
_LL3B3:{struct Cyc_List_List*_tmpD6B;yyval=Cyc_YY13(((_tmpD6B=_cycalloc(sizeof(*
_tmpD6B)),((_tmpD6B->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpD6B->tl=0,_tmpD6B)))))));}break;case 392: _LL3B4:{struct Cyc_List_List*
_tmpD6C;yyval=Cyc_YY13(((_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((_tmpD6C->hd=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD6C->tl=Cyc_yyget_YY13(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpD6C)))))));}
break;case 393: _LL3B5:{struct _tuple20*_tmpD6D;yyval=Cyc_YY14(((_tmpD6D=_cycalloc(
sizeof(*_tmpD6D)),((_tmpD6D->f1=0,((_tmpD6D->f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD6D)))))));}break;case
394: _LL3B6:{struct _tuple20*_tmpD6E;yyval=Cyc_YY14(((_tmpD6E=_cycalloc(sizeof(*
_tmpD6E)),((_tmpD6E->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpD6E->f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD6E)))))));}break;case
395: _LL3B7:{struct _tuple19*_tmpD6F;yyval=Cyc_YY16(((_tmpD6F=_cycalloc(sizeof(*
_tmpD6F)),((_tmpD6F->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),((
_tmpD6F->f2=0,_tmpD6F)))))));}break;case 396: _LL3B8:{struct _tuple19*_tmpD70;yyval=
Cyc_YY16(((_tmpD70=_cycalloc(sizeof(*_tmpD70)),((_tmpD70->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpD70->f2=1,_tmpD70)))))));}
break;case 397: _LL3B9:{struct _tuple19*_tmpD71;yyval=Cyc_YY16(((_tmpD71=_cycalloc(
sizeof(*_tmpD71)),((_tmpD71->f1=0,((_tmpD71->f2=1,_tmpD71)))))));}break;case 398:
_LL3BA:{struct Cyc_List_List*_tmpD72;yyval=Cyc_YY15(((_tmpD72=_cycalloc(sizeof(*
_tmpD72)),((_tmpD72->hd=Cyc_yyget_YY14(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpD72->tl=0,_tmpD72)))))));}break;case 399: _LL3BB:{struct Cyc_List_List*
_tmpD73;yyval=Cyc_YY15(((_tmpD73=_cycalloc(sizeof(*_tmpD73)),((_tmpD73->hd=Cyc_yyget_YY14(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD73->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpD73)))))));}
break;case 400: _LL3BC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 401: _LL3BD: yyval=Cyc_YY3(Cyc_Absyn_seq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 402:
_LL3BE: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
403: _LL3BF: yyval=Cyc_YY3(Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 404:
_LL3C0:{struct Cyc_Absyn_Swap_e_struct _tmpD76;struct Cyc_Absyn_Swap_e_struct*
_tmpD75;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD75=_cycalloc(sizeof(*
_tmpD75)),((_tmpD75[0]=((_tmpD76.tag=36,((_tmpD76.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD76.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD76)))))),_tmpD75)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 405: _LL3C1: yyval=Cyc_YY8(0);break;case
406: _LL3C2:{struct Cyc_Core_Opt*_tmpD77;yyval=Cyc_YY8(((_tmpD77=_cycalloc(sizeof(*
_tmpD77)),((_tmpD77->v=(void*)((void*)1),_tmpD77)))));}break;case 407: _LL3C3:{
struct Cyc_Core_Opt*_tmpD78;yyval=Cyc_YY8(((_tmpD78=_cycalloc(sizeof(*_tmpD78)),((
_tmpD78->v=(void*)((void*)3),_tmpD78)))));}break;case 408: _LL3C4:{struct Cyc_Core_Opt*
_tmpD79;yyval=Cyc_YY8(((_tmpD79=_cycalloc(sizeof(*_tmpD79)),((_tmpD79->v=(void*)((
void*)4),_tmpD79)))));}break;case 409: _LL3C5:{struct Cyc_Core_Opt*_tmpD7A;yyval=
Cyc_YY8(((_tmpD7A=_cycalloc(sizeof(*_tmpD7A)),((_tmpD7A->v=(void*)((void*)0),
_tmpD7A)))));}break;case 410: _LL3C6:{struct Cyc_Core_Opt*_tmpD7B;yyval=Cyc_YY8(((
_tmpD7B=_cycalloc(sizeof(*_tmpD7B)),((_tmpD7B->v=(void*)((void*)2),_tmpD7B)))));}
break;case 411: _LL3C7:{struct Cyc_Core_Opt*_tmpD7C;yyval=Cyc_YY8(((_tmpD7C=
_cycalloc(sizeof(*_tmpD7C)),((_tmpD7C->v=(void*)((void*)16),_tmpD7C)))));}break;
case 412: _LL3C8:{struct Cyc_Core_Opt*_tmpD7D;yyval=Cyc_YY8(((_tmpD7D=_cycalloc(
sizeof(*_tmpD7D)),((_tmpD7D->v=(void*)((void*)17),_tmpD7D)))));}break;case 413:
_LL3C9:{struct Cyc_Core_Opt*_tmpD7E;yyval=Cyc_YY8(((_tmpD7E=_cycalloc(sizeof(*
_tmpD7E)),((_tmpD7E->v=(void*)((void*)13),_tmpD7E)))));}break;case 414: _LL3CA:{
struct Cyc_Core_Opt*_tmpD7F;yyval=Cyc_YY8(((_tmpD7F=_cycalloc(sizeof(*_tmpD7F)),((
_tmpD7F->v=(void*)((void*)15),_tmpD7F)))));}break;case 415: _LL3CB:{struct Cyc_Core_Opt*
_tmpD80;yyval=Cyc_YY8(((_tmpD80=_cycalloc(sizeof(*_tmpD80)),((_tmpD80->v=(void*)((
void*)14),_tmpD80)))));}break;case 416: _LL3CC: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 417: _LL3CD: yyval=
Cyc_YY3(Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 418:
_LL3CE: yyval=Cyc_YY3(Cyc_Absyn_throw_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 419:
_LL3CF: yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 420:
_LL3D0: yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 421:
_LL3D1: yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 422:
_LL3D2: yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 423:
_LL3D3: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
424: _LL3D4: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 425: _LL3D5: yyval=Cyc_YY3(Cyc_Absyn_or_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 426:
_LL3D6: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
427: _LL3D7: yyval=Cyc_YY3(Cyc_Absyn_and_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 428:
_LL3D8: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
429: _LL3D9: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 430:
_LL3DA: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
431: _LL3DB: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 432:
_LL3DC: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
433: _LL3DD: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 434:
_LL3DE: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
435: _LL3DF: yyval=Cyc_YY3(Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 436:
_LL3E0: yyval=Cyc_YY3(Cyc_Absyn_neq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 437:
_LL3E1: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
438: _LL3E2: yyval=Cyc_YY3(Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 439:
_LL3E3: yyval=Cyc_YY3(Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 440:
_LL3E4: yyval=Cyc_YY3(Cyc_Absyn_lte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 441:
_LL3E5: yyval=Cyc_YY3(Cyc_Absyn_gte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 442:
_LL3E6: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
443: _LL3E7: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 444:
_LL3E8: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 445:
_LL3E9: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
446: _LL3EA: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 447:
_LL3EB: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 448:
_LL3EC: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
449: _LL3ED: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 450:
_LL3EE: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 451:
_LL3EF: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 452:
_LL3F0: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
453: _LL3F1: yyval=Cyc_YY3(Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 454:
_LL3F2: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
455: _LL3F3: yyval=Cyc_YY3(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 456:
_LL3F4: yyval=Cyc_YY3(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 457:
_LL3F5: yyval=Cyc_YY3(Cyc_Absyn_address_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 458:
_LL3F6: yyval=Cyc_YY3(Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 459:
_LL3F7: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
460: _LL3F8: yyval=Cyc_YY3(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 461:
_LL3F9: yyval=Cyc_YY3(Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 462:
_LL3FA: yyval=Cyc_YY3(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 463:
_LL3FB:{struct Cyc_Absyn_StructField_struct _tmpD86;struct _dyneither_ptr*_tmpD85;
struct Cyc_Absyn_StructField_struct*_tmpD84;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpD84=_cycalloc(sizeof(*_tmpD84)),((_tmpD84[0]=((_tmpD86.tag=0,((
_tmpD86.f1=((_tmpD85=_cycalloc(sizeof(*_tmpD85)),((_tmpD85[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD85)))),_tmpD86)))),
_tmpD84)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 464: _LL3FC:{struct Cyc_Absyn_TupleIndex_struct
_tmpD89;struct Cyc_Absyn_TupleIndex_struct*_tmpD88;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpD88=_cycalloc_atomic(sizeof(*_tmpD88)),((_tmpD88[0]=((_tmpD89.tag=1,((
_tmpD89.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2,_tmpD89)))),_tmpD88)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 465:
_LL3FD: {struct Cyc_Position_Segment*_tmp72D=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp72E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp72D,Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
yyval=Cyc_YY3(Cyc_Absyn_gentyp_exp(_tmp72E,(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 466:
_LL3FE:{struct Cyc_Absyn_Malloc_e_struct _tmpD8F;struct Cyc_Absyn_MallocInfo _tmpD8E;
struct Cyc_Absyn_Malloc_e_struct*_tmpD8D;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpD8D=_cycalloc(sizeof(*_tmpD8D)),((_tmpD8D[0]=((_tmpD8F.tag=35,((_tmpD8F.f1=((
_tmpD8E.is_calloc=0,((_tmpD8E.rgn=0,((_tmpD8E.elt_type=0,((_tmpD8E.num_elts=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD8E.fat_result=
0,_tmpD8E)))))))))),_tmpD8F)))),_tmpD8D)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 467:
_LL3FF:{struct Cyc_Absyn_Malloc_e_struct _tmpD95;struct Cyc_Absyn_MallocInfo _tmpD94;
struct Cyc_Absyn_Malloc_e_struct*_tmpD93;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpD93=_cycalloc(sizeof(*_tmpD93)),((_tmpD93[0]=((_tmpD95.tag=35,((_tmpD95.f1=((
_tmpD94.is_calloc=0,((_tmpD94.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpD94.elt_type=0,((
_tmpD94.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpD94.fat_result=0,_tmpD94)))))))))),_tmpD95)))),_tmpD93)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 468: _LL400: {void*_tmp736;struct _tuple8
_tmp735=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp736=_tmp735.f3;{struct Cyc_Absyn_Malloc_e_struct _tmpD9F;void**_tmpD9E;
struct Cyc_Absyn_MallocInfo _tmpD9D;struct Cyc_Absyn_Malloc_e_struct*_tmpD9C;yyval=
Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD9C=_cycalloc(sizeof(*_tmpD9C)),((_tmpD9C[
0]=((_tmpD9F.tag=35,((_tmpD9F.f1=((_tmpD9D.is_calloc=1,((_tmpD9D.rgn=0,((_tmpD9D.elt_type=((
_tmpD9E=_cycalloc(sizeof(*_tmpD9E)),((_tmpD9E[0]=_tmp736,_tmpD9E)))),((_tmpD9D.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((
_tmpD9D.fat_result=0,_tmpD9D)))))))))),_tmpD9F)))),_tmpD9C)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
469: _LL401: {void*_tmp73C;struct _tuple8 _tmp73B=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp73C=_tmp73B.f3;{
struct Cyc_Absyn_Malloc_e_struct _tmpDA9;void**_tmpDA8;struct Cyc_Absyn_MallocInfo
_tmpDA7;struct Cyc_Absyn_Malloc_e_struct*_tmpDA6;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpDA6=_cycalloc(sizeof(*_tmpDA6)),((_tmpDA6[0]=((_tmpDA9.tag=35,((
_tmpDA9.f1=((_tmpDA7.is_calloc=1,((_tmpDA7.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 8)]),((_tmpDA7.elt_type=((
_tmpDA8=_cycalloc(sizeof(*_tmpDA8)),((_tmpDA8[0]=_tmp73C,_tmpDA8)))),((_tmpDA7.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((
_tmpDA7.fat_result=0,_tmpDA7)))))))))),_tmpDA9)))),_tmpDA6)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
470: _LL402:{struct Cyc_Absyn_Exp*_tmpDAA[1];yyval=Cyc_YY3(Cyc_Absyn_primop_exp((
void*)19,((_tmpDAA[0]=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpDAA,sizeof(struct Cyc_Absyn_Exp*),1)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 471:
_LL403:{struct Cyc_Absyn_Tagcheck_e_struct _tmpDB0;struct _dyneither_ptr*_tmpDAF;
struct Cyc_Absyn_Tagcheck_e_struct*_tmpDAE;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpDAE=_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE[0]=((_tmpDB0.tag=39,((_tmpDB0.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpDB0.f2=((_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpDAF)))),_tmpDB0)))))),
_tmpDAE)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 472: _LL404:{struct Cyc_Absyn_Tagcheck_e_struct
_tmpDB6;struct _dyneither_ptr*_tmpDB5;struct Cyc_Absyn_Tagcheck_e_struct*_tmpDB4;
yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpDB4=_cycalloc(sizeof(*_tmpDB4)),((
_tmpDB4[0]=((_tmpDB6.tag=39,((_tmpDB6.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)),((_tmpDB6.f2=((
_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpDB5)))),_tmpDB6)))))),
_tmpDB4)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 473: _LL405: {void*_tmp749;struct _tuple8
_tmp748=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp749=_tmp748.f3;yyval=Cyc_YY3(Cyc_Absyn_valueof_exp(_tmp749,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 474:
_LL406: yyval=Cyc_YY7((void*)12);break;case 475: _LL407: yyval=Cyc_YY7((void*)11);
break;case 476: _LL408: yyval=Cyc_YY7((void*)2);break;case 477: _LL409: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 478: _LL40A: yyval=
Cyc_YY3(Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 479:
_LL40B: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 480:
_LL40C: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 481:
_LL40D:{struct _dyneither_ptr*_tmpDB7;yyval=Cyc_YY3(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDB7=_cycalloc(
sizeof(*_tmpDB7)),((_tmpDB7[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDB7)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 482:
_LL40E:{struct _dyneither_ptr*_tmpDB8;yyval=Cyc_YY3(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDB8=_cycalloc(
sizeof(*_tmpDB8)),((_tmpDB8[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDB8)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 483:
_LL40F: yyval=Cyc_YY3(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 484:
_LL410: yyval=Cyc_YY3(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 485:
_LL411:{struct Cyc_Absyn_CompoundLit_e_struct _tmpDBB;struct Cyc_Absyn_CompoundLit_e_struct*
_tmpDBA;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpDBA=_cycalloc(sizeof(*
_tmpDBA)),((_tmpDBA[0]=((_tmpDBB.tag=27,((_tmpDBB.f1=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpDBB.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),_tmpDBB)))))),_tmpDBA)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 486: _LL412:{struct Cyc_Absyn_CompoundLit_e_struct
_tmpDBE;struct Cyc_Absyn_CompoundLit_e_struct*_tmpDBD;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD[0]=((_tmpDBE.tag=27,((
_tmpDBE.f1=Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 5)]),((_tmpDBE.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),
_tmpDBE)))))),_tmpDBD)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 487:
_LL413:{struct Cyc_Absyn_UnknownId_e_struct _tmpDC1;struct Cyc_Absyn_UnknownId_e_struct*
_tmpDC0;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpDC0=_cycalloc(sizeof(*
_tmpDC0)),((_tmpDC0[0]=((_tmpDC1.tag=2,((_tmpDC1.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDC1)))),_tmpDC0)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 488:
_LL414: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
489: _LL415: yyval=Cyc_YY3(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 490:
_LL416: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 491: _LL417: yyval=Cyc_YY3(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 492:
_LL418: yyval=Cyc_YY3(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 493:
_LL419: yyval=Cyc_YY3(Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 494:
_LL41A:{struct Cyc_Absyn_Aggregate_e_struct _tmpDC4;struct Cyc_Absyn_Aggregate_e_struct*
_tmpDC3;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpDC3=_cycalloc(sizeof(*
_tmpDC3)),((_tmpDC3[0]=((_tmpDC4.tag=30,((_tmpDC4.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpDC4.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDC4.f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),((_tmpDC4.f4=0,
_tmpDC4)))))))))),_tmpDC3)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 495:
_LL41B: yyval=Cyc_YY3(Cyc_Absyn_stmt_exp(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 496:
_LL41C: yyval=Cyc_YY5(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));break;
case 497: _LL41D:{struct Cyc_List_List*_tmpDC5;yyval=Cyc_YY5(((_tmpDC5=_cycalloc(
sizeof(*_tmpDC5)),((_tmpDC5->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpDC5->tl=0,_tmpDC5)))))));}
break;case 498: _LL41E:{struct Cyc_List_List*_tmpDC6;yyval=Cyc_YY5(((_tmpDC6=
_cycalloc(sizeof(*_tmpDC6)),((_tmpDC6->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpDC6->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpDC6)))))));}
break;case 499: _LL41F: yyval=Cyc_YY3(Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 500:
_LL420: yyval=Cyc_YY3(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 501:
_LL421: yyval=Cyc_YY3(Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 502:
_LL422: yyval=Cyc_YY3(Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 503:
_LL423:{struct _dyneither_ptr*_tmpDC9;struct _tuple1*_tmpDC8;yyval=Cyc_QualId_tok(((
_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8->f1=Cyc_Absyn_Rel_n(0),((_tmpDC8->f2=((
_tmpDC9=_cycalloc(sizeof(*_tmpDC9)),((_tmpDC9[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDC9)))),_tmpDC8)))))));}
break;case 504: _LL424: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 505: _LL425:{struct _dyneither_ptr*_tmpDCC;struct _tuple1*
_tmpDCB;yyval=Cyc_QualId_tok(((_tmpDCB=_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB->f1=
Cyc_Absyn_Rel_n(0),((_tmpDCB->f2=((_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC[
0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpDCC)))),_tmpDCB)))))));}break;case 506: _LL426: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 507: _LL427: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 508: _LL428:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 509:
_LL429: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
510: _LL42A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 511: _LL42B: break;case 512: _LL42C: yylex_buf->lex_curr_pos -=1;break;default:
_LL42D: break;}yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(513,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(144,yyn - 144)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
6273) && Cyc_yycheck[_check_known_subscript_notnull(6274,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(6274,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(144,yyn - 144)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1035,yystate)];if(yyn > - 32768  && yyn < 6273){int
sze=0;struct _dyneither_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 288 / 
sizeof(char*);++ x){if(Cyc_yycheck[_check_known_subscript_notnull(6274,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(288,x)])+ 15,count ++);}{unsigned int _tmpDD1;
unsigned int _tmpDD0;struct _dyneither_ptr _tmpDCF;char*_tmpDCE;unsigned int _tmpDCD;
msg=((_tmpDCD=(unsigned int)(sze + 15),((_tmpDCE=(char*)_region_malloc(yyregion,
_check_times(sizeof(char),_tmpDCD + 1)),((_tmpDCF=_tag_dyneither(_tmpDCE,sizeof(
char),_tmpDCD + 1),((((_tmpDD0=_tmpDCD,((_tmpDD2(& _tmpDD1,& _tmpDD0,& _tmpDCE),
_tmpDCE[_tmpDD0]=(char)0)))),_tmpDCF))))))));}{const char*_tmpDD3;Cyc_strcpy(msg,((
_tmpDD3="parse error",_tag_dyneither(_tmpDD3,sizeof(char),12))));}if(count < 5){
count=0;for(x=yyn < 0?- yyn: 0;x < 288 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(6274,x + yyn)]== x){{const char*_tmpDD5;const char*
_tmpDD4;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)((
_tmpDD5=", expecting `",_tag_dyneither(_tmpDD5,sizeof(char),14))):(struct
_dyneither_ptr)((_tmpDD4=" or `",_tag_dyneither(_tmpDD4,sizeof(char),6)))));}Cyc_strcat(
msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(288,x)]);{
const char*_tmpDD6;Cyc_strcat(msg,((_tmpDD6="'",_tag_dyneither(_tmpDD6,sizeof(
char),2))));}++ count;}}}Cyc_yyerror((struct _dyneither_ptr)msg);}else{const char*
_tmpDD7;Cyc_yyerror(((_tmpDD7="parse error",_tag_dyneither(_tmpDD7,sizeof(char),
12))));}}goto yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){int
_tmp763=1;_npop_handler(0);return _tmp763;}Cyc_yychar=- 2;}yyerrstatus=3;goto
yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0){int _tmp764=1;
_npop_handler(0);return _tmp764;}-- yyvsp_offset;yystate=(int)yyss[
_check_known_subscript_notnull(10000,-- yyssp_offset)];-- yylsp_offset;yyerrhandle:
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1035,yystate)];if(yyn == -
32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6273) || Cyc_yycheck[
_check_known_subscript_notnull(6274,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6274,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 1034){int
_tmp765=0;_npop_handler(0);return _tmp765;}yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;goto yynewstate;};_pop_region(yyregion);}void Cyc_yyprint(
int i,union Cyc_YYSTYPE v);void Cyc_yyprint(int i,union Cyc_YYSTYPE v){union Cyc_YYSTYPE
_tmp76F=v;struct _tuple6 _tmp770;int _tmp771;char _tmp772;struct _dyneither_ptr
_tmp773;struct _tuple1*_tmp774;struct _tuple1 _tmp775;union Cyc_Absyn_Nmspace _tmp776;
struct _dyneither_ptr*_tmp777;_LL430: if((_tmp76F.Int_tok).tag != 1)goto _LL432;
_tmp770=(struct _tuple6)(_tmp76F.Int_tok).val;_tmp771=_tmp770.f2;_LL431:{const
char*_tmpDDB;void*_tmpDDA[1];struct Cyc_Int_pa_struct _tmpDD9;(_tmpDD9.tag=1,((
_tmpDD9.f1=(unsigned long)_tmp771,((_tmpDDA[0]=& _tmpDD9,Cyc_fprintf(Cyc_stderr,((
_tmpDDB="%d",_tag_dyneither(_tmpDDB,sizeof(char),3))),_tag_dyneither(_tmpDDA,
sizeof(void*),1)))))));}goto _LL42F;_LL432: if((_tmp76F.Char_tok).tag != 2)goto
_LL434;_tmp772=(char)(_tmp76F.Char_tok).val;_LL433:{const char*_tmpDDF;void*
_tmpDDE[1];struct Cyc_Int_pa_struct _tmpDDD;(_tmpDDD.tag=1,((_tmpDDD.f1=(
unsigned long)((int)_tmp772),((_tmpDDE[0]=& _tmpDDD,Cyc_fprintf(Cyc_stderr,((
_tmpDDF="%c",_tag_dyneither(_tmpDDF,sizeof(char),3))),_tag_dyneither(_tmpDDE,
sizeof(void*),1)))))));}goto _LL42F;_LL434: if((_tmp76F.String_tok).tag != 3)goto
_LL436;_tmp773=(struct _dyneither_ptr)(_tmp76F.String_tok).val;_LL435:{const char*
_tmpDE3;void*_tmpDE2[1];struct Cyc_String_pa_struct _tmpDE1;(_tmpDE1.tag=0,((
_tmpDE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp773),((_tmpDE2[0]=&
_tmpDE1,Cyc_fprintf(Cyc_stderr,((_tmpDE3="\"%s\"",_tag_dyneither(_tmpDE3,sizeof(
char),5))),_tag_dyneither(_tmpDE2,sizeof(void*),1)))))));}goto _LL42F;_LL436: if((
_tmp76F.QualId_tok).tag != 5)goto _LL438;_tmp774=(struct _tuple1*)(_tmp76F.QualId_tok).val;
_tmp775=*_tmp774;_tmp776=_tmp775.f1;_tmp777=_tmp775.f2;_LL437: {struct Cyc_List_List*
_tmp781=0;{union Cyc_Absyn_Nmspace _tmp782=_tmp776;struct Cyc_List_List*_tmp783;
struct Cyc_List_List*_tmp784;int _tmp785;_LL43B: if((_tmp782.Rel_n).tag != 1)goto
_LL43D;_tmp783=(struct Cyc_List_List*)(_tmp782.Rel_n).val;_LL43C: _tmp781=_tmp783;
goto _LL43A;_LL43D: if((_tmp782.Abs_n).tag != 2)goto _LL43F;_tmp784=(struct Cyc_List_List*)(
_tmp782.Abs_n).val;_LL43E: _tmp781=_tmp784;goto _LL43A;_LL43F: if((_tmp782.Loc_n).tag
!= 3)goto _LL43A;_tmp785=(int)(_tmp782.Loc_n).val;_LL440: goto _LL43A;_LL43A:;}for(
0;_tmp781 != 0;_tmp781=_tmp781->tl){const char*_tmpDE7;void*_tmpDE6[1];struct Cyc_String_pa_struct
_tmpDE5;(_tmpDE5.tag=0,((_tmpDE5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmp781->hd)),((_tmpDE6[0]=& _tmpDE5,Cyc_fprintf(Cyc_stderr,((
_tmpDE7="%s::",_tag_dyneither(_tmpDE7,sizeof(char),5))),_tag_dyneither(_tmpDE6,
sizeof(void*),1)))))));}{const char*_tmpDEB;void*_tmpDEA[1];struct Cyc_String_pa_struct
_tmpDE9;(_tmpDE9.tag=0,((_tmpDE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp777),((_tmpDEA[0]=& _tmpDE9,Cyc_fprintf(Cyc_stderr,((_tmpDEB="%s::",
_tag_dyneither(_tmpDEB,sizeof(char),5))),_tag_dyneither(_tmpDEA,sizeof(void*),1)))))));}
goto _LL42F;}_LL438:;_LL439:{const char*_tmpDEE;void*_tmpDED;(_tmpDED=0,Cyc_fprintf(
Cyc_stderr,((_tmpDEE="?",_tag_dyneither(_tmpDEE,sizeof(char),2))),_tag_dyneither(
_tmpDED,sizeof(void*),0)));}goto _LL42F;_LL42F:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f);struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*
f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(f));return Cyc_Parse_parse_result;}
