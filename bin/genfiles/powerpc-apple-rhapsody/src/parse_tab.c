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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;int is_flat;};
struct Cyc_Absyn_UnknownDatatype_struct{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
f1;};struct Cyc_Absyn_KnownDatatype_struct{int tag;struct Cyc_Absyn_Datatypedecl**
f1;};union Cyc_Absyn_DatatypeInfoU_union{struct Cyc_Absyn_UnknownDatatype_struct
UnknownDatatype;struct Cyc_Absyn_KnownDatatype_struct KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU_union datatype_info;struct Cyc_List_List*targs;struct
Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*
datatype_name;struct _tuple1*field_name;int is_extensible;};struct Cyc_Absyn_UnknownDatatypefield_struct{
int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo f1;};struct Cyc_Absyn_KnownDatatypefield_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};union
Cyc_Absyn_DatatypeFieldInfoU_union{struct Cyc_Absyn_UnknownDatatypefield_struct
UnknownDatatypefield;struct Cyc_Absyn_KnownDatatypefield_struct KnownDatatypefield;
};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU_union
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
exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);struct Cyc_Absyn_Decl*
Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct
Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(void*s,struct _tuple1*n,struct Cyc_List_List*
ts,struct Cyc_Core_Opt*fs,int is_extensible,int is_flat,struct Cyc_Position_Segment*
loc);void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*
eff_typ,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(
void*a);extern int Cyc_Absyn_porting_c_code;struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct _tuple4{void*f1;void*f2;};struct _tuple4*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple4*Cyc_Dict_rchoose(
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
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){{const char*_tmp8E7;void*
_tmp8E6[2];struct Cyc_String_pa_struct _tmp8E5;struct Cyc_String_pa_struct _tmp8E4;(
_tmp8E4.tag=0,((_tmp8E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((
_tmp8E5.tag=0,((_tmp8E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
sg)),((_tmp8E6[0]=& _tmp8E5,((_tmp8E6[1]=& _tmp8E4,Cyc_fprintf(Cyc_stderr,((
_tmp8E7="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp8E7,
sizeof(char),46))),_tag_dyneither(_tmp8E6,sizeof(void*),2)))))))))))));}return;}
static int Cyc_Parse_enum_counter=0;struct _tuple1*Cyc_Parse_gensym_enum();struct
_tuple1*Cyc_Parse_gensym_enum(){union Cyc_Absyn_Nmspace_union _tmp8F6;struct
_dyneither_ptr*_tmp8F5;const char*_tmp8F4;void*_tmp8F3[1];struct Cyc_Int_pa_struct
_tmp8F2;struct _tuple1*_tmp8F1;return(_tmp8F1=_cycalloc(sizeof(*_tmp8F1)),((
_tmp8F1->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp8F6.Rel_n).tag=1,(((_tmp8F6.Rel_n).f1=
0,_tmp8F6)))),((_tmp8F1->f2=((_tmp8F5=_cycalloc(sizeof(*_tmp8F5)),((_tmp8F5[0]=(
struct _dyneither_ptr)((_tmp8F2.tag=1,((_tmp8F2.f1=(unsigned long)Cyc_Parse_enum_counter
++,((_tmp8F3[0]=& _tmp8F2,Cyc_aprintf(((_tmp8F4="__anonymous_enum_%d__",
_tag_dyneither(_tmp8F4,sizeof(char),22))),_tag_dyneither(_tmp8F3,sizeof(void*),1)))))))),
_tmp8F5)))),_tmp8F1)))));}struct _tuple7{struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;
void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple8{struct
_tuple7*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
struct Cyc_Position_Segment*loc,struct _tuple8*field_info);static struct Cyc_Absyn_Aggrfield*
Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*loc,struct _tuple8*
field_info){struct _tuple8 _tmpB;struct _tuple7*_tmpC;struct _tuple7 _tmpD;struct
_tuple1*_tmpE;struct Cyc_Absyn_Tqual _tmpF;void*_tmp10;struct Cyc_List_List*_tmp11;
struct Cyc_List_List*_tmp12;struct Cyc_Absyn_Exp*_tmp13;struct _tuple8*_tmpA=
field_info;_tmpB=*_tmpA;_tmpC=_tmpB.f1;_tmpD=*_tmpC;_tmpE=_tmpD.f1;_tmpF=_tmpD.f2;
_tmp10=_tmpD.f3;_tmp11=_tmpD.f4;_tmp12=_tmpD.f5;_tmp13=_tmpB.f2;if(_tmp11 != 0){
const char*_tmp8F7;Cyc_Parse_err(((_tmp8F7="bad type params in struct field",
_tag_dyneither(_tmp8F7,sizeof(char),32))),loc);}if(Cyc_Absyn_is_qvar_qualified(
_tmpE)){const char*_tmp8F8;Cyc_Parse_err(((_tmp8F8="struct or union field cannot be qualified with a namespace",
_tag_dyneither(_tmp8F8,sizeof(char),59))),loc);}{struct Cyc_Absyn_Aggrfield*
_tmp8F9;return(_tmp8F9=_cycalloc(sizeof(*_tmp8F9)),((_tmp8F9->name=(*_tmpE).f2,((
_tmp8F9->tq=_tmpF,((_tmp8F9->type=(void*)_tmp10,((_tmp8F9->width=_tmp13,((
_tmp8F9->attributes=_tmp12,_tmp8F9)))))))))));}}static void*Cyc_Parse_type_spec(
void*t,struct Cyc_Position_Segment*loc);static void*Cyc_Parse_type_spec(void*t,
struct Cyc_Position_Segment*loc){struct Cyc_Parse_Type_spec_struct _tmp8FC;struct
Cyc_Parse_Type_spec_struct*_tmp8FB;return(void*)((_tmp8FB=_cycalloc(sizeof(*
_tmp8FB)),((_tmp8FB[0]=((_tmp8FC.tag=4,((_tmp8FC.f1=(void*)t,((_tmp8FC.f2=loc,
_tmp8FC)))))),_tmp8FB))));}static void*Cyc_Parse_array2ptr(void*t,int argposn);
static void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp19=t;struct Cyc_Absyn_ArrayInfo
_tmp1A;void*_tmp1B;struct Cyc_Absyn_Tqual _tmp1C;struct Cyc_Absyn_Exp*_tmp1D;struct
Cyc_Absyn_Conref*_tmp1E;struct Cyc_Position_Segment*_tmp1F;_LL1: if(_tmp19 <= (void*)
4)goto _LL3;if(*((int*)_tmp19)!= 7)goto _LL3;_tmp1A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp19)->f1;_tmp1B=(void*)_tmp1A.elt_type;_tmp1C=_tmp1A.tq;_tmp1D=_tmp1A.num_elts;
_tmp1E=_tmp1A.zero_term;_tmp1F=_tmp1A.zt_loc;_LL2: {struct Cyc_Absyn_Upper_b_struct
_tmp8FF;struct Cyc_Absyn_Upper_b_struct*_tmp8FE;return Cyc_Absyn_starb_typ(_tmp1B,
argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0):(void*)2,
_tmp1C,_tmp1D == 0?(void*)0:(void*)((_tmp8FE=_cycalloc(sizeof(*_tmp8FE)),((
_tmp8FE[0]=((_tmp8FF.tag=0,((_tmp8FF.f1=(struct Cyc_Absyn_Exp*)_tmp1D,_tmp8FF)))),
_tmp8FE)))),_tmp1E);}_LL3:;_LL4: return t;_LL0:;}static void Cyc_Parse_arg_array2ptr(
struct _tuple2*x);static void Cyc_Parse_arg_array2ptr(struct _tuple2*x){(*x).f3=Cyc_Parse_array2ptr((*
x).f3,1);}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple9*
Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct _tuple2*t);static
struct _tuple9*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct
_tuple2*t){struct _tuple9*_tmp900;return(_tmp900=_cycalloc(sizeof(*_tmp900)),((
_tmp900->f1=(*t).f2,((_tmp900->f2=(*t).f3,_tmp900)))));}static struct Cyc_Absyn_Vardecl*
Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d);static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(
struct Cyc_Absyn_Decl*d){void*_tmp23=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp24;
_LL6: if(_tmp23 <= (void*)2)goto _LL8;if(*((int*)_tmp23)!= 0)goto _LL8;_tmp24=((
struct Cyc_Absyn_Var_d_struct*)_tmp23)->f1;_LL7: return _tmp24;_LL8:;_LL9: {const
char*_tmp903;void*_tmp902;(_tmp902=0,((int(*)(struct Cyc_Position_Segment*sg,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(d->loc,((
_tmp903="bad declaration in `forarray' statement",_tag_dyneither(_tmp903,sizeof(
char),40))),_tag_dyneither(_tmp902,sizeof(void*),0)));}_LL5:;}static int Cyc_Parse_is_typeparam(
void*tm);static int Cyc_Parse_is_typeparam(void*tm){void*_tmp27=tm;_LLB: if(*((int*)
_tmp27)!= 4)goto _LLD;_LLC: return 1;_LLD:;_LLE: return 0;_LLA:;}static void*Cyc_Parse_id2type(
struct _dyneither_ptr s,void*k);static void*Cyc_Parse_id2type(struct _dyneither_ptr s,
void*k){const char*_tmp904;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp904="`H",
_tag_dyneither(_tmp904,sizeof(char),3))))== 0)return(void*)2;else{const char*
_tmp905;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp905="`U",_tag_dyneither(
_tmp905,sizeof(char),3))))== 0)return(void*)3;else{struct Cyc_Absyn_VarType_struct
_tmp90F;struct _dyneither_ptr*_tmp90E;struct Cyc_Absyn_Tvar*_tmp90D;struct Cyc_Absyn_VarType_struct*
_tmp90C;return(void*)((_tmp90C=_cycalloc(sizeof(*_tmp90C)),((_tmp90C[0]=((
_tmp90F.tag=1,((_tmp90F.f1=((_tmp90D=_cycalloc(sizeof(*_tmp90D)),((_tmp90D->name=((
_tmp90E=_cycalloc(sizeof(*_tmp90E)),((_tmp90E[0]=s,_tmp90E)))),((_tmp90D->identity=
- 1,((_tmp90D->kind=(void*)k,_tmp90D)))))))),_tmp90F)))),_tmp90C))));}}}static
struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t);static struct Cyc_Absyn_Tvar*
Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){void*k;{void*_tmp2E=Cyc_Absyn_compress_kb((
void*)t->kind);void*_tmp2F;_LL10: if(*((int*)_tmp2E)!= 1)goto _LL12;_LL11:{struct
Cyc_Absyn_Unknown_kb_struct _tmp912;struct Cyc_Absyn_Unknown_kb_struct*_tmp911;k=(
void*)((_tmp911=_cycalloc(sizeof(*_tmp911)),((_tmp911[0]=((_tmp912.tag=1,((
_tmp912.f1=0,_tmp912)))),_tmp911))));}goto _LLF;_LL12: if(*((int*)_tmp2E)!= 2)goto
_LL14;_tmp2F=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp2E)->f2;_LL13:{struct
Cyc_Absyn_Less_kb_struct _tmp915;struct Cyc_Absyn_Less_kb_struct*_tmp914;k=(void*)((
_tmp914=_cycalloc(sizeof(*_tmp914)),((_tmp914[0]=((_tmp915.tag=2,((_tmp915.f1=0,((
_tmp915.f2=(void*)_tmp2F,_tmp915)))))),_tmp914))));}goto _LLF;_LL14:;_LL15: k=
_tmp2E;goto _LLF;_LLF:;}{struct Cyc_Absyn_Tvar*_tmp916;return(_tmp916=_cycalloc(
sizeof(*_tmp916)),((_tmp916->name=t->name,((_tmp916->identity=- 1,((_tmp916->kind=(
void*)k,_tmp916)))))));}}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t);static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t){void*_tmp35=t;struct Cyc_Absyn_Tvar*_tmp36;_LL17: if(_tmp35 <= (void*)4)
goto _LL19;if(*((int*)_tmp35)!= 1)goto _LL19;_tmp36=((struct Cyc_Absyn_VarType_struct*)
_tmp35)->f1;_LL18: return _tmp36;_LL19:;_LL1A: {const char*_tmp919;void*_tmp918;(
_tmp918=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp919="expecting a list of type variables, not types",
_tag_dyneither(_tmp919,sizeof(char),46))),_tag_dyneither(_tmp918,sizeof(void*),0)));}
_LL16:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr);static void*Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar*pr){struct Cyc_Absyn_VarType_struct _tmp91C;struct Cyc_Absyn_VarType_struct*
_tmp91B;return(void*)((_tmp91B=_cycalloc(sizeof(*_tmp91B)),((_tmp91B[0]=((
_tmp91C.tag=1,((_tmp91C.f1=pr,_tmp91C)))),_tmp91B))));}static void Cyc_Parse_set_vartyp_kind(
void*t,void*k,int leq);static void Cyc_Parse_set_vartyp_kind(void*t,void*k,int leq){
void*_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp3C;struct Cyc_Absyn_Tvar
_tmp3D;void*_tmp3E;void**_tmp3F;_LL1C: if(_tmp3B <= (void*)4)goto _LL1E;if(*((int*)
_tmp3B)!= 1)goto _LL1E;_tmp3C=((struct Cyc_Absyn_VarType_struct*)_tmp3B)->f1;
_tmp3D=*_tmp3C;_tmp3E=(void*)_tmp3D.kind;_tmp3F=(void**)&(*((struct Cyc_Absyn_VarType_struct*)
_tmp3B)->f1).kind;_LL1D: {void*_tmp40=Cyc_Absyn_compress_kb(*_tmp3F);_LL21: if(*((
int*)_tmp40)!= 1)goto _LL23;_LL22: if(!leq)*_tmp3F=Cyc_Tcutil_kind_to_bound(k);
else{struct Cyc_Absyn_Less_kb_struct _tmp91F;struct Cyc_Absyn_Less_kb_struct*
_tmp91E;*_tmp3F=(void*)((_tmp91E=_cycalloc(sizeof(*_tmp91E)),((_tmp91E[0]=((
_tmp91F.tag=2,((_tmp91F.f1=0,((_tmp91F.f2=(void*)k,_tmp91F)))))),_tmp91E))));}
return;_LL23:;_LL24: return;_LL20:;}_LL1E:;_LL1F: return;_LL1B:;}static struct Cyc_List_List*
Cyc_Parse_oldstyle2newstyle(struct Cyc_List_List*tms,struct Cyc_List_List*tds,
struct Cyc_Position_Segment*loc);static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp43=(void*)tms->hd;void*_tmp44;
_LL26: if(*((int*)_tmp43)!= 3)goto _LL28;_tmp44=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp43)->f1;_LL27: if(tms->tl == 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 
0){void*_tmp45=_tmp44;struct Cyc_List_List*_tmp46;_LL2B: if(*((int*)_tmp45)!= 1)
goto _LL2D;_LL2C:{const char*_tmp922;void*_tmp921;(_tmp921=0,Cyc_Tcutil_warn(loc,((
_tmp922="function declaration with both new- and old-style parameter declarations; ignoring old-style",
_tag_dyneither(_tmp922,sizeof(char),93))),_tag_dyneither(_tmp921,sizeof(void*),0)));}
return tms;_LL2D: if(*((int*)_tmp45)!= 0)goto _LL2A;_tmp46=((struct Cyc_Absyn_NoTypes_struct*)
_tmp45)->f1;_LL2E: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp46)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){const char*_tmp925;void*
_tmp924;(_tmp924=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp925="wrong number of parameter declarations in old-style function declaration",
_tag_dyneither(_tmp925,sizeof(char),73))),_tag_dyneither(_tmp924,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp4B=0;for(0;_tmp46 != 0;_tmp46=_tmp46->tl){struct Cyc_List_List*
_tmp4C=tds;for(0;_tmp4C != 0;_tmp4C=_tmp4C->tl){struct Cyc_Absyn_Decl*_tmp4D=(
struct Cyc_Absyn_Decl*)_tmp4C->hd;void*_tmp4E=(void*)_tmp4D->r;struct Cyc_Absyn_Vardecl*
_tmp4F;_LL30: if(_tmp4E <= (void*)2)goto _LL32;if(*((int*)_tmp4E)!= 0)goto _LL32;
_tmp4F=((struct Cyc_Absyn_Var_d_struct*)_tmp4E)->f1;_LL31: if(Cyc_zstrptrcmp((*
_tmp4F->name).f2,(struct _dyneither_ptr*)_tmp46->hd)!= 0)continue;if(_tmp4F->initializer
!= 0){const char*_tmp928;void*_tmp927;(_tmp927=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,((
_tmp928="initializer found in parameter declaration",_tag_dyneither(_tmp928,
sizeof(char),43))),_tag_dyneither(_tmp927,sizeof(void*),0)));}if(Cyc_Absyn_is_qvar_qualified(
_tmp4F->name)){const char*_tmp92B;void*_tmp92A;(_tmp92A=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,((
_tmp92B="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp92B,
sizeof(char),47))),_tag_dyneither(_tmp92A,sizeof(void*),0)));}{struct _tuple2*
_tmp931;struct Cyc_Core_Opt*_tmp930;struct Cyc_List_List*_tmp92F;_tmp4B=((_tmp92F=
_cycalloc(sizeof(*_tmp92F)),((_tmp92F->hd=((_tmp931=_cycalloc(sizeof(*_tmp931)),((
_tmp931->f1=((_tmp930=_cycalloc(sizeof(*_tmp930)),((_tmp930->v=(*_tmp4F->name).f2,
_tmp930)))),((_tmp931->f2=_tmp4F->tq,((_tmp931->f3=(void*)_tmp4F->type,_tmp931)))))))),((
_tmp92F->tl=_tmp4B,_tmp92F))))));}goto L;_LL32:;_LL33: {const char*_tmp934;void*
_tmp933;(_tmp933=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,((_tmp934="nonvariable declaration in parameter type",
_tag_dyneither(_tmp934,sizeof(char),42))),_tag_dyneither(_tmp933,sizeof(void*),0)));}
_LL2F:;}L: if(_tmp4C == 0){const char*_tmp938;void*_tmp937[1];struct Cyc_String_pa_struct
_tmp936;(_tmp936.tag=0,((_tmp936.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmp46->hd)),((_tmp937[0]=& _tmp936,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp938="%s is not given a type",_tag_dyneither(_tmp938,sizeof(char),23))),
_tag_dyneither(_tmp937,sizeof(void*),1)))))));}}{struct Cyc_Absyn_Function_mod_struct*
_tmp947;struct Cyc_Absyn_WithTypes_struct*_tmp946;struct Cyc_Absyn_WithTypes_struct
_tmp945;struct Cyc_Absyn_Function_mod_struct _tmp944;struct Cyc_List_List*_tmp943;
return(_tmp943=_cycalloc(sizeof(*_tmp943)),((_tmp943->hd=(void*)((void*)((
_tmp947=_cycalloc(sizeof(*_tmp947)),((_tmp947[0]=((_tmp944.tag=3,((_tmp944.f1=(
void*)((void*)((_tmp946=_cycalloc(sizeof(*_tmp946)),((_tmp946[0]=((_tmp945.tag=1,((
_tmp945.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp4B),((_tmp945.f2=0,((_tmp945.f3=0,((_tmp945.f4=0,((_tmp945.f5=0,_tmp945)))))))))))),
_tmp946))))),_tmp944)))),_tmp947))))),((_tmp943->tl=0,_tmp943)))));}}_LL2A:;}
goto _LL29;_LL28:;_LL29: {struct Cyc_List_List*_tmp948;return(_tmp948=_cycalloc(
sizeof(*_tmp948)),((_tmp948->hd=(void*)((void*)tms->hd),((_tmp948->tl=Cyc_Parse_oldstyle2newstyle(
tms->tl,tds,loc),_tmp948)))));}_LL25:;}}struct _tuple10{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(
struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,
struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc);static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,
struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){
if(tds != 0){struct Cyc_Parse_Declarator*_tmp949;d=((_tmp949=_cycalloc(sizeof(*
_tmp949)),((_tmp949->id=d->id,((_tmp949->tms=Cyc_Parse_oldstyle2newstyle(d->tms,
tds,loc),_tmp949))))));}{void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual
tq=Cyc_Absyn_empty_tqual(0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){
tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)
dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;
atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc != 0){void*_tmp63=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v;_LL35: if((int)_tmp63 != 1)goto _LL37;
_LL36: sc=(void*)3;goto _LL34;_LL37: if((int)_tmp63 != 3)goto _LL39;_LL38: sc=(void*)0;
goto _LL34;_LL39:;_LL3A:{const char*_tmp94A;Cyc_Parse_err(((_tmp94A="bad storage class on function",
_tag_dyneither(_tmp94A,sizeof(char),30))),loc);}goto _LL34;_LL34:;}}{void*_tmp66;
struct Cyc_Core_Opt*_tmp67;struct _tuple5 _tmp65=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp66=_tmp65.f1;_tmp67=_tmp65.f2;{struct Cyc_Absyn_Tqual _tmp69;void*
_tmp6A;struct Cyc_List_List*_tmp6B;struct Cyc_List_List*_tmp6C;struct _tuple6 _tmp68=
Cyc_Parse_apply_tms(tq,_tmp66,atts,d->tms);_tmp69=_tmp68.f1;_tmp6A=_tmp68.f2;
_tmp6B=_tmp68.f3;_tmp6C=_tmp68.f4;if(_tmp67 != 0){const char*_tmp94D;void*_tmp94C;(
_tmp94C=0,Cyc_Tcutil_warn(loc,((_tmp94D="nested type declaration within function prototype",
_tag_dyneither(_tmp94D,sizeof(char),50))),_tag_dyneither(_tmp94C,sizeof(void*),0)));}
if(_tmp6B != 0){const char*_tmp950;void*_tmp94F;(_tmp94F=0,Cyc_Tcutil_warn(loc,((
_tmp950="bad type params, ignoring",_tag_dyneither(_tmp950,sizeof(char),26))),
_tag_dyneither(_tmp94F,sizeof(void*),0)));}{void*_tmp71=_tmp6A;struct Cyc_Absyn_FnInfo
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
_tmp7E.f3;if(_tmp7F == 0){{const char*_tmp951;Cyc_Parse_err(((_tmp951="missing argument variable in function prototype",
_tag_dyneither(_tmp951,sizeof(char),48))),loc);}{struct _tuple10*_tmp954;struct
Cyc_List_List*_tmp953;_tmp7B=((_tmp953=_cycalloc(sizeof(*_tmp953)),((_tmp953->hd=((
_tmp954=_cycalloc(sizeof(*_tmp954)),((_tmp954->f1=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp954->f2=_tmp80,((_tmp954->f3=
_tmp81,_tmp954)))))))),((_tmp953->tl=_tmp7B,_tmp953))))));}}else{struct _tuple10*
_tmp957;struct Cyc_List_List*_tmp956;_tmp7B=((_tmp956=_cycalloc(sizeof(*_tmp956)),((
_tmp956->hd=((_tmp957=_cycalloc(sizeof(*_tmp957)),((_tmp957->f1=(struct
_dyneither_ptr*)_tmp7F->v,((_tmp957->f2=_tmp80,((_tmp957->f3=_tmp81,_tmp957)))))))),((
_tmp956->tl=_tmp7B,_tmp956))))));}}}{struct Cyc_Absyn_Fndecl*_tmp958;return(
_tmp958=_cycalloc(sizeof(*_tmp958)),((_tmp958->sc=(void*)sc,((_tmp958->name=d->id,((
_tmp958->tvs=_tmp73,((_tmp958->is_inline=is_inline,((_tmp958->effect=_tmp74,((
_tmp958->ret_type=(void*)_tmp75,((_tmp958->args=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp7B),((_tmp958->c_varargs=_tmp77,((_tmp958->cyc_varargs=
_tmp78,((_tmp958->rgn_po=_tmp79,((_tmp958->body=body,((_tmp958->cached_typ=0,((
_tmp958->param_vardecls=0,((_tmp958->fn_vardecl=0,((_tmp958->attributes=Cyc_List_append(
_tmp7A,_tmp6C),_tmp958)))))))))))))))))))))))))))))));}}_LL3E:;_LL3F: {const char*
_tmp95B;void*_tmp95A;(_tmp95A=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp95B="declarator is not a function prototype",
_tag_dyneither(_tmp95B,sizeof(char),39))),_tag_dyneither(_tmp95A,sizeof(void*),0)));}
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
_tmp95C;Cyc_Parse_err(((_tmp95C="signed qualifier must come before type",
_tag_dyneither(_tmp95C,sizeof(char),39))),_tmp92);}last_loc=_tmp92;seen_sign=1;
sgn=(void*)0;goto _LL40;_LL45: if(*((int*)_tmp8F)!= 1)goto _LL47;_tmp93=((struct Cyc_Parse_Unsigned_spec_struct*)
_tmp8F)->f1;_LL46: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp93);if(seen_type){
const char*_tmp95D;Cyc_Parse_err(((_tmp95D="signed qualifier must come before type",
_tag_dyneither(_tmp95D,sizeof(char),39))),_tmp93);}last_loc=_tmp93;seen_sign=1;
sgn=(void*)1;goto _LL40;_LL47: if(*((int*)_tmp8F)!= 2)goto _LL49;_tmp94=((struct Cyc_Parse_Short_spec_struct*)
_tmp8F)->f1;_LL48: if(seen_size){const char*_tmp95E;Cyc_Parse_err(((_tmp95E="integral size may appear only once within a type specifier",
_tag_dyneither(_tmp95E,sizeof(char),59))),_tmp94);}if(seen_type){const char*
_tmp95F;Cyc_Parse_err(((_tmp95F="short modifier must come before base type",
_tag_dyneither(_tmp95F,sizeof(char),42))),_tmp94);}last_loc=_tmp94;seen_size=1;
sz=(void*)1;goto _LL40;_LL49: if(*((int*)_tmp8F)!= 3)goto _LL4B;_tmp95=((struct Cyc_Parse_Long_spec_struct*)
_tmp8F)->f1;_LL4A: if(seen_type){const char*_tmp960;Cyc_Parse_err(((_tmp960="long modifier must come before base type",
_tag_dyneither(_tmp960,sizeof(char),41))),_tmp95);}if(seen_size){void*_tmp9C=sz;
_LL4E: if((int)_tmp9C != 3)goto _LL50;_LL4F: sz=(void*)4;goto _LL4D;_LL50:;_LL51:{
const char*_tmp961;Cyc_Parse_err(((_tmp961="extra long in type specifier",
_tag_dyneither(_tmp961,sizeof(char),29))),_tmp95);}goto _LL4D;_LL4D:;}else{sz=(
void*)3;}last_loc=_tmp95;seen_size=1;goto _LL40;_LL4B: if(*((int*)_tmp8F)!= 5)goto
_LL40;_tmp96=((struct Cyc_Parse_Decl_spec_struct*)_tmp8F)->f1;_LL4C: last_loc=
_tmp96->loc;if(declopt == 0  && !seen_type){seen_type=1;{void*_tmp9E=(void*)_tmp96->r;
struct Cyc_Absyn_Aggrdecl*_tmp9F;struct Cyc_Absyn_Datatypedecl*_tmpA0;struct Cyc_Absyn_Enumdecl*
_tmpA1;_LL53: if(_tmp9E <= (void*)2)goto _LL59;if(*((int*)_tmp9E)!= 6)goto _LL55;
_tmp9F=((struct Cyc_Absyn_Aggr_d_struct*)_tmp9E)->f1;_LL54: {struct Cyc_List_List*
_tmpA2=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmp9F->tvs));{struct Cyc_Absyn_AggrType_struct _tmp96B;union Cyc_Absyn_AggrInfoU_union
_tmp96A;struct Cyc_Absyn_AggrInfo _tmp969;struct Cyc_Absyn_AggrType_struct*_tmp968;
t=(void*)((_tmp968=_cycalloc(sizeof(*_tmp968)),((_tmp968[0]=((_tmp96B.tag=10,((
_tmp96B.f1=((_tmp969.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((_tmp96A.UnknownAggr).tag=
0,(((_tmp96A.UnknownAggr).f1=(void*)((void*)_tmp9F->kind),(((_tmp96A.UnknownAggr).f2=
_tmp9F->name,_tmp96A)))))),((_tmp969.targs=_tmpA2,_tmp969)))),_tmp96B)))),
_tmp968))));}if(_tmp9F->impl != 0){struct Cyc_Core_Opt*_tmp96C;declopt=((_tmp96C=
_cycalloc(sizeof(*_tmp96C)),((_tmp96C->v=_tmp96,_tmp96C))));}goto _LL52;}_LL55:
if(*((int*)_tmp9E)!= 7)goto _LL57;_tmpA0=((struct Cyc_Absyn_Datatype_d_struct*)
_tmp9E)->f1;_LL56: {struct Cyc_List_List*_tmpA8=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmpA0->tvs));struct Cyc_Core_Opt*
_tmp96D;struct Cyc_Core_Opt*ropt=_tmpA0->is_flat?0:((_tmp96D=_cycalloc(sizeof(*
_tmp96D)),((_tmp96D->v=(void*)((void*)2),_tmp96D))));{struct Cyc_Absyn_DatatypeType_struct
_tmp97C;union Cyc_Absyn_DatatypeInfoU_union _tmp97B;struct Cyc_Absyn_Datatypedecl**
_tmp97A;struct Cyc_Absyn_DatatypeInfo _tmp979;struct Cyc_Absyn_DatatypeType_struct*
_tmp978;t=(void*)((_tmp978=_cycalloc(sizeof(*_tmp978)),((_tmp978[0]=((_tmp97C.tag=
2,((_tmp97C.f1=((_tmp979.datatype_info=(union Cyc_Absyn_DatatypeInfoU_union)(((
_tmp97B.KnownDatatype).tag=1,(((_tmp97B.KnownDatatype).f1=((_tmp97A=_cycalloc(
sizeof(*_tmp97A)),((_tmp97A[0]=_tmpA0,_tmp97A)))),_tmp97B)))),((_tmp979.targs=
_tmpA8,((_tmp979.rgn=ropt,_tmp979)))))),_tmp97C)))),_tmp978))));}if(_tmpA0->fields
!= 0){struct Cyc_Core_Opt*_tmp97D;declopt=((_tmp97D=_cycalloc(sizeof(*_tmp97D)),((
_tmp97D->v=_tmp96,_tmp97D))));}goto _LL52;}_LL57: if(*((int*)_tmp9E)!= 8)goto _LL59;
_tmpA1=((struct Cyc_Absyn_Enum_d_struct*)_tmp9E)->f1;_LL58:{struct Cyc_Absyn_EnumType_struct
_tmp980;struct Cyc_Absyn_EnumType_struct*_tmp97F;t=(void*)((_tmp97F=_cycalloc(
sizeof(*_tmp97F)),((_tmp97F[0]=((_tmp980.tag=12,((_tmp980.f1=_tmpA1->name,((
_tmp980.f2=0,_tmp980)))))),_tmp97F))));}{struct Cyc_Core_Opt*_tmp981;declopt=((
_tmp981=_cycalloc(sizeof(*_tmp981)),((_tmp981->v=_tmp96,_tmp981))));}goto _LL52;
_LL59:;_LL5A: {const char*_tmp984;void*_tmp983;(_tmp983=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp96->loc,((
_tmp984="bad declaration within type specifier",_tag_dyneither(_tmp984,sizeof(
char),38))),_tag_dyneither(_tmp983,sizeof(void*),0)));}_LL52:;}}else{Cyc_Parse_err(
Cyc_Parse_msg3,_tmp96->loc);}goto _LL40;_LL40:;}if(!seen_type){if(!seen_sign  && !
seen_size){const char*_tmp987;void*_tmp986;(_tmp986=0,Cyc_Tcutil_warn(last_loc,((
_tmp987="missing type within specifier",_tag_dyneither(_tmp987,sizeof(char),30))),
_tag_dyneither(_tmp986,sizeof(void*),0)));}t=Cyc_Absyn_int_typ(sgn,sz);}else{if(
seen_sign){void*_tmpB7=t;void*_tmpB8;void*_tmpB9;_LL5C: if(_tmpB7 <= (void*)4)goto
_LL5E;if(*((int*)_tmpB7)!= 5)goto _LL5E;_tmpB8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpB7)->f1;_tmpB9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB7)->f2;_LL5D:
if(_tmpB8 != sgn)t=Cyc_Absyn_int_typ(sgn,_tmpB9);goto _LL5B;_LL5E:;_LL5F:{const
char*_tmp988;Cyc_Parse_err(((_tmp988="sign specification on non-integral type",
_tag_dyneither(_tmp988,sizeof(char),40))),last_loc);}goto _LL5B;_LL5B:;}if(
seen_size){void*_tmpBB=t;void*_tmpBC;void*_tmpBD;_LL61: if(_tmpBB <= (void*)4)goto
_LL65;if(*((int*)_tmpBB)!= 5)goto _LL63;_tmpBC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpBB)->f1;_tmpBD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBB)->f2;_LL62:
if(_tmpBD != sz)t=Cyc_Absyn_int_typ(_tmpBC,sz);goto _LL60;_LL63: if(*((int*)_tmpBB)
!= 6)goto _LL65;_LL64: t=Cyc_Absyn_double_typ(1);goto _LL60;_LL65:;_LL66:{const char*
_tmp989;Cyc_Parse_err(((_tmp989="size qualifier on non-integral type",
_tag_dyneither(_tmp989,sizeof(char),36))),last_loc);}goto _LL60;_LL60:;}}{struct
_tuple5 _tmp98A;return(_tmp98A.f1=t,((_tmp98A.f2=declopt,_tmp98A)));}}static
struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts);static struct
Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,
void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{
struct Cyc_Parse_Declarator*_tmpC0=(struct Cyc_Parse_Declarator*)ds->hd;struct
_tuple1*_tmpC1=_tmpC0->id;struct Cyc_Absyn_Tqual _tmpC3;void*_tmpC4;struct Cyc_List_List*
_tmpC5;struct Cyc_List_List*_tmpC6;struct _tuple6 _tmpC2=Cyc_Parse_apply_tms(tq,t,
shared_atts,_tmpC0->tms);_tmpC3=_tmpC2.f1;_tmpC4=_tmpC2.f2;_tmpC5=_tmpC2.f3;
_tmpC6=_tmpC2.f4;if(ds->tl == 0){struct _tuple7*_tmp98D;struct Cyc_List_List*
_tmp98C;return(_tmp98C=_region_malloc(r,sizeof(*_tmp98C)),((_tmp98C->hd=((
_tmp98D=_region_malloc(r,sizeof(*_tmp98D)),((_tmp98D->f1=_tmpC1,((_tmp98D->f2=
_tmpC3,((_tmp98D->f3=_tmpC4,((_tmp98D->f4=_tmpC5,((_tmp98D->f5=_tmpC6,_tmp98D)))))))))))),((
_tmp98C->tl=0,_tmp98C)))));}else{struct _tuple7*_tmp990;struct Cyc_List_List*
_tmp98F;return(_tmp98F=_region_malloc(r,sizeof(*_tmp98F)),((_tmp98F->hd=((
_tmp990=_region_malloc(r,sizeof(*_tmp990)),((_tmp990->f1=_tmpC1,((_tmp990->f2=
_tmpC3,((_tmp990->f3=_tmpC4,((_tmp990->f4=_tmpC5,((_tmp990->f5=_tmpC6,_tmp990)))))))))))),((
_tmp98F->tl=Cyc_Parse_apply_tmss(r,_tmpC3,Cyc_Tcutil_copy_type(t),ds->tl,
shared_atts),_tmp98F)))));}}}static struct _tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms);static struct _tuple6
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,
struct Cyc_List_List*tms){if(tms == 0){struct _tuple6 _tmp991;return(_tmp991.f1=tq,((
_tmp991.f2=t,((_tmp991.f3=0,((_tmp991.f4=atts,_tmp991)))))));}{void*_tmpCC=(void*)
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
void*)as->hd)){struct Cyc_List_List*_tmp992;fn_atts=((_tmp992=_cycalloc(sizeof(*
_tmp992)),((_tmp992->hd=(void*)((void*)as->hd),((_tmp992->tl=fn_atts,_tmp992))))));}
else{struct Cyc_List_List*_tmp993;new_atts=((_tmp993=_cycalloc(sizeof(*_tmp993)),((
_tmp993->hd=(void*)((void*)as->hd),((_tmp993->tl=new_atts,_tmp993))))));}}}if(
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
Cyc_Absyn_NoTypes_struct*)_tmpD9)->f2;_LL78: {const char*_tmp996;void*_tmp995;(
_tmp995=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(_tmpDF,((_tmp996="function declaration without parameter types",
_tag_dyneither(_tmp996,sizeof(char),45))),_tag_dyneither(_tmp995,sizeof(void*),0)));}
_LL74:;}_LL6E: if(*((int*)_tmpCC)!= 4)goto _LL70;_tmpD3=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpCC)->f1;_tmpD4=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCC)->f2;_LL6F:
if(tms->tl == 0){struct _tuple6 _tmp997;return(_tmp997.f1=tq,((_tmp997.f2=t,((
_tmp997.f3=_tmpD3,((_tmp997.f4=atts,_tmp997)))))));}{const char*_tmp99A;void*
_tmp999;(_tmp999=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD4,((_tmp99A="type parameters must appear before function arguments in declarator",
_tag_dyneither(_tmp99A,sizeof(char),68))),_tag_dyneither(_tmp999,sizeof(void*),0)));}
_LL70: if(*((int*)_tmpCC)!= 2)goto _LL72;_tmpD5=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmpCC)->f1;_tmpD6=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCC)->f2;_LL71: {
struct Cyc_Absyn_PointerType_struct _tmp9A0;struct Cyc_Absyn_PtrInfo _tmp99F;struct
Cyc_Absyn_PointerType_struct*_tmp99E;return Cyc_Parse_apply_tms(_tmpD6,(void*)((
_tmp99E=_cycalloc(sizeof(*_tmp99E)),((_tmp99E[0]=((_tmp9A0.tag=4,((_tmp9A0.f1=((
_tmp99F.elt_typ=(void*)t,((_tmp99F.elt_tq=tq,((_tmp99F.ptr_atts=_tmpD5,_tmp99F)))))),
_tmp9A0)))),_tmp99E)))),atts,tms->tl);}_LL72: if(*((int*)_tmpCC)!= 5)goto _LL67;
_tmpD7=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpCC)->f1;_tmpD8=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpCC)->f2;_LL73: return Cyc_Parse_apply_tms(tq,t,Cyc_List_append(atts,_tmpD8),
tms->tl);_LL67:;}}void*Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*
loc);void*Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*
loc){void*_tmpED;struct Cyc_Core_Opt*_tmpEE;struct _tuple5 _tmpEC=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmpED=_tmpEC.f1;_tmpEE=_tmpEC.f2;if(_tmpEE != 0){const char*_tmp9A3;void*
_tmp9A2;(_tmp9A2=0,Cyc_Tcutil_warn(loc,((_tmp9A3="ignoring nested type declaration(s) in specifier list",
_tag_dyneither(_tmp9A3,sizeof(char),54))),_tag_dyneither(_tmp9A2,sizeof(void*),0)));}
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
else{kind=_tmpF9;}goto _LL7E;_LL81:;_LL82: kind=0;{struct Cyc_Core_Opt*_tmp9A4;type=((
_tmp9A4=_cycalloc(sizeof(*_tmp9A4)),((_tmp9A4->v=(void*)_tmpF5,_tmp9A4))));}goto
_LL7E;_LL7E:;}{struct Cyc_Absyn_Typedef_d_struct _tmp9AA;struct Cyc_Absyn_Typedefdecl*
_tmp9A9;struct Cyc_Absyn_Typedef_d_struct*_tmp9A8;return Cyc_Absyn_new_decl((void*)((
_tmp9A8=_cycalloc(sizeof(*_tmp9A8)),((_tmp9A8[0]=((_tmp9AA.tag=9,((_tmp9AA.f1=((
_tmp9A9=_cycalloc(sizeof(*_tmp9A9)),((_tmp9A9->name=_tmpF3,((_tmp9A9->tvs=_tmpF6,((
_tmp9A9->kind=kind,((_tmp9A9->defn=type,((_tmp9A9->atts=_tmpF7,((_tmp9A9->tq=
_tmpF4,_tmp9A9)))))))))))))),_tmp9AA)))),_tmp9A8)))),loc);}}}static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*
s){struct Cyc_Absyn_Decl_s_struct _tmp9AD;struct Cyc_Absyn_Decl_s_struct*_tmp9AC;
return Cyc_Absyn_new_stmt((void*)((_tmp9AC=_cycalloc(sizeof(*_tmp9AC)),((_tmp9AC[
0]=((_tmp9AD.tag=11,((_tmp9AD.f1=d,((_tmp9AD.f2=s,_tmp9AD)))))),_tmp9AC)))),d->loc);}
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
const char*_tmp9AE;Cyc_Parse_err(((_tmp9AE="inline is allowed only on function definitions",
_tag_dyneither(_tmp9AE,sizeof(char),47))),loc);}if(_tmp104 == 0){{const char*
_tmp9AF;Cyc_Parse_err(((_tmp9AF="missing type specifiers in declaration",
_tag_dyneither(_tmp9AF,sizeof(char),39))),loc);}{struct Cyc_List_List*_tmp108=0;
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
struct Cyc_Absyn_Enumdecl*_tmp112;struct Cyc_Absyn_Aggrdecl*_tmp113;struct Cyc_Absyn_Datatypedecl*
_tmp114;_LL93: if(_tmp111 <= (void*)2)goto _LL99;if(*((int*)_tmp111)!= 8)goto _LL95;
_tmp112=((struct Cyc_Absyn_Enum_d_struct*)_tmp111)->f1;_LL94:(void*)(_tmp112->sc=(
void*)s);if(_tmp105 != 0){const char*_tmp9B0;Cyc_Parse_err(((_tmp9B0="bad attributes on enum",
_tag_dyneither(_tmp9B0,sizeof(char),23))),loc);}goto _LL92;_LL95: if(*((int*)
_tmp111)!= 6)goto _LL97;_tmp113=((struct Cyc_Absyn_Aggr_d_struct*)_tmp111)->f1;
_LL96:(void*)(_tmp113->sc=(void*)s);_tmp113->attributes=_tmp105;goto _LL92;_LL97:
if(*((int*)_tmp111)!= 7)goto _LL99;_tmp114=((struct Cyc_Absyn_Datatype_d_struct*)
_tmp111)->f1;_LL98:(void*)(_tmp114->sc=(void*)s);if(_tmp105 != 0){const char*
_tmp9B1;Cyc_Parse_err(((_tmp9B1="bad attributes on datatype",_tag_dyneither(
_tmp9B1,sizeof(char),27))),loc);}goto _LL92;_LL99:;_LL9A:{const char*_tmp9B2;Cyc_Parse_err(((
_tmp9B2="bad declaration",_tag_dyneither(_tmp9B2,sizeof(char),16))),loc);}{
struct Cyc_List_List*_tmp118=0;_npop_handler(0);return _tmp118;}_LL92:;}{struct Cyc_List_List*
_tmp9B3;struct Cyc_List_List*_tmp11A=(_tmp9B3=_cycalloc(sizeof(*_tmp9B3)),((
_tmp9B3->hd=d,((_tmp9B3->tl=0,_tmp9B3)))));_npop_handler(0);return _tmp11A;}}
else{void*_tmp11B=_tmp10F;struct Cyc_Absyn_AggrInfo _tmp11C;union Cyc_Absyn_AggrInfoU_union
_tmp11D;void*_tmp11E;struct _tuple1*_tmp11F;struct Cyc_List_List*_tmp120;struct Cyc_Absyn_DatatypeInfo
_tmp121;union Cyc_Absyn_DatatypeInfoU_union _tmp122;struct Cyc_Absyn_Datatypedecl**
_tmp123;struct Cyc_Absyn_DatatypeInfo _tmp124;union Cyc_Absyn_DatatypeInfoU_union
_tmp125;struct Cyc_Absyn_UnknownDatatypeInfo _tmp126;struct _tuple1*_tmp127;int
_tmp128;int _tmp129;struct Cyc_List_List*_tmp12A;struct _tuple1*_tmp12B;struct Cyc_List_List*
_tmp12C;_LL9C: if(_tmp11B <= (void*)4)goto _LLA6;if(*((int*)_tmp11B)!= 10)goto _LL9E;
_tmp11C=((struct Cyc_Absyn_AggrType_struct*)_tmp11B)->f1;_tmp11D=_tmp11C.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp11B)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL9E;_tmp11E=(_tmp11D.UnknownAggr).f1;_tmp11F=(_tmp11D.UnknownAggr).f2;
_tmp120=_tmp11C.targs;_LL9D: {struct Cyc_List_List*_tmp12D=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp120);
struct Cyc_Absyn_Aggrdecl*_tmp9B4;struct Cyc_Absyn_Aggrdecl*_tmp12E=(_tmp9B4=
_cycalloc(sizeof(*_tmp9B4)),((_tmp9B4->kind=(void*)_tmp11E,((_tmp9B4->sc=(void*)
s,((_tmp9B4->name=_tmp11F,((_tmp9B4->tvs=_tmp12D,((_tmp9B4->impl=0,((_tmp9B4->attributes=
0,_tmp9B4)))))))))))));if(_tmp105 != 0){const char*_tmp9B5;Cyc_Parse_err(((_tmp9B5="bad attributes on type declaration",
_tag_dyneither(_tmp9B5,sizeof(char),35))),loc);}{struct Cyc_Absyn_Aggr_d_struct*
_tmp9BB;struct Cyc_Absyn_Aggr_d_struct _tmp9BA;struct Cyc_List_List*_tmp9B9;struct
Cyc_List_List*_tmp133=(_tmp9B9=_cycalloc(sizeof(*_tmp9B9)),((_tmp9B9->hd=Cyc_Absyn_new_decl((
void*)((_tmp9BB=_cycalloc(sizeof(*_tmp9BB)),((_tmp9BB[0]=((_tmp9BA.tag=6,((
_tmp9BA.f1=_tmp12E,_tmp9BA)))),_tmp9BB)))),loc),((_tmp9B9->tl=0,_tmp9B9)))));
_npop_handler(0);return _tmp133;}}_LL9E: if(*((int*)_tmp11B)!= 2)goto _LLA0;_tmp121=((
struct Cyc_Absyn_DatatypeType_struct*)_tmp11B)->f1;_tmp122=_tmp121.datatype_info;
if((((((struct Cyc_Absyn_DatatypeType_struct*)_tmp11B)->f1).datatype_info).KnownDatatype).tag
!= 1)goto _LLA0;_tmp123=(_tmp122.KnownDatatype).f1;_LL9F: if(_tmp105 != 0){const
char*_tmp9BC;Cyc_Parse_err(((_tmp9BC="bad attributes on datatype",_tag_dyneither(
_tmp9BC,sizeof(char),27))),loc);}{struct Cyc_Absyn_Datatype_d_struct*_tmp9C2;
struct Cyc_Absyn_Datatype_d_struct _tmp9C1;struct Cyc_List_List*_tmp9C0;struct Cyc_List_List*
_tmp139=(_tmp9C0=_cycalloc(sizeof(*_tmp9C0)),((_tmp9C0->hd=Cyc_Absyn_new_decl((
void*)((_tmp9C2=_cycalloc(sizeof(*_tmp9C2)),((_tmp9C2[0]=((_tmp9C1.tag=7,((
_tmp9C1.f1=*_tmp123,_tmp9C1)))),_tmp9C2)))),loc),((_tmp9C0->tl=0,_tmp9C0)))));
_npop_handler(0);return _tmp139;}_LLA0: if(*((int*)_tmp11B)!= 2)goto _LLA2;_tmp124=((
struct Cyc_Absyn_DatatypeType_struct*)_tmp11B)->f1;_tmp125=_tmp124.datatype_info;
if((((((struct Cyc_Absyn_DatatypeType_struct*)_tmp11B)->f1).datatype_info).UnknownDatatype).tag
!= 0)goto _LLA2;_tmp126=(_tmp125.UnknownDatatype).f1;_tmp127=_tmp126.name;_tmp128=
_tmp126.is_extensible;_tmp129=_tmp126.is_flat;_tmp12A=_tmp124.targs;_LLA1: {
struct Cyc_List_List*_tmp13A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp12A);struct Cyc_Absyn_Decl*_tmp13B=
Cyc_Absyn_datatype_decl(s,_tmp127,_tmp13A,0,_tmp128,_tmp129,loc);if(_tmp105 != 0){
const char*_tmp9C3;Cyc_Parse_err(((_tmp9C3="bad attributes on datatype",
_tag_dyneither(_tmp9C3,sizeof(char),27))),loc);}{struct Cyc_List_List*_tmp9C4;
struct Cyc_List_List*_tmp13E=(_tmp9C4=_cycalloc(sizeof(*_tmp9C4)),((_tmp9C4->hd=
_tmp13B,((_tmp9C4->tl=0,_tmp9C4)))));_npop_handler(0);return _tmp13E;}}_LLA2: if(*((
int*)_tmp11B)!= 12)goto _LLA4;_tmp12B=((struct Cyc_Absyn_EnumType_struct*)_tmp11B)->f1;
_LLA3: {struct Cyc_Absyn_Enumdecl*_tmp9C5;struct Cyc_Absyn_Enumdecl*_tmp13F=(
_tmp9C5=_cycalloc(sizeof(*_tmp9C5)),((_tmp9C5->sc=(void*)s,((_tmp9C5->name=
_tmp12B,((_tmp9C5->fields=0,_tmp9C5)))))));if(_tmp105 != 0){const char*_tmp9C6;Cyc_Parse_err(((
_tmp9C6="bad attributes on enum",_tag_dyneither(_tmp9C6,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp9D0;struct Cyc_Absyn_Enum_d_struct _tmp9CF;struct Cyc_Absyn_Enum_d_struct*
_tmp9CE;struct Cyc_List_List*_tmp9CD;struct Cyc_List_List*_tmp145=(_tmp9CD=
_cycalloc(sizeof(*_tmp9CD)),((_tmp9CD->hd=((_tmp9D0=_cycalloc(sizeof(*_tmp9D0)),((
_tmp9D0->r=(void*)((void*)((_tmp9CE=_cycalloc(sizeof(*_tmp9CE)),((_tmp9CE[0]=((
_tmp9CF.tag=8,((_tmp9CF.f1=_tmp13F,_tmp9CF)))),_tmp9CE))))),((_tmp9D0->loc=loc,
_tmp9D0)))))),((_tmp9CD->tl=0,_tmp9CD)))));_npop_handler(0);return _tmp145;}}
_LLA4: if(*((int*)_tmp11B)!= 13)goto _LLA6;_tmp12C=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp11B)->f1;_LLA5: {struct Cyc_Core_Opt*_tmp9D3;struct Cyc_Absyn_Enumdecl*_tmp9D2;
struct Cyc_Absyn_Enumdecl*_tmp147=(_tmp9D2=_cycalloc(sizeof(*_tmp9D2)),((_tmp9D2->sc=(
void*)s,((_tmp9D2->name=Cyc_Parse_gensym_enum(),((_tmp9D2->fields=((_tmp9D3=
_cycalloc(sizeof(*_tmp9D3)),((_tmp9D3->v=_tmp12C,_tmp9D3)))),_tmp9D2)))))));if(
_tmp105 != 0){const char*_tmp9D4;Cyc_Parse_err(((_tmp9D4="bad attributes on enum",
_tag_dyneither(_tmp9D4,sizeof(char),23))),loc);}{struct Cyc_Absyn_Decl*_tmp9DE;
struct Cyc_Absyn_Enum_d_struct _tmp9DD;struct Cyc_Absyn_Enum_d_struct*_tmp9DC;
struct Cyc_List_List*_tmp9DB;struct Cyc_List_List*_tmp14D=(_tmp9DB=_cycalloc(
sizeof(*_tmp9DB)),((_tmp9DB->hd=((_tmp9DE=_cycalloc(sizeof(*_tmp9DE)),((_tmp9DE->r=(
void*)((void*)((_tmp9DC=_cycalloc(sizeof(*_tmp9DC)),((_tmp9DC[0]=((_tmp9DD.tag=8,((
_tmp9DD.f1=_tmp147,_tmp9DD)))),_tmp9DC))))),((_tmp9DE->loc=loc,_tmp9DE)))))),((
_tmp9DB->tl=0,_tmp9DB)))));_npop_handler(0);return _tmp14D;}}_LLA6:;_LLA7:{const
char*_tmp9DF;Cyc_Parse_err(((_tmp9DF="missing declarator",_tag_dyneither(_tmp9DF,
sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp151=0;_npop_handler(0);return
_tmp151;}_LL9B:;}}else{void*t=_tmp10D.f1;struct Cyc_List_List*_tmp152=Cyc_Parse_apply_tmss(
mkrgn,_tmp103,t,_tmp10B,_tmp105);if(istypedef){if(!exps_empty){const char*_tmp9E0;
Cyc_Parse_err(((_tmp9E0="initializer in typedef declaration",_tag_dyneither(
_tmp9E0,sizeof(char),35))),loc);}{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple7*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp152);if(_tmp10D.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(
_tmp10D.f2)->v;{void*_tmp154=(void*)d->r;struct Cyc_Absyn_Aggrdecl*_tmp155;struct
Cyc_Absyn_Datatypedecl*_tmp156;struct Cyc_Absyn_Enumdecl*_tmp157;_LLA9: if(_tmp154
<= (void*)2)goto _LLAF;if(*((int*)_tmp154)!= 6)goto _LLAB;_tmp155=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp154)->f1;_LLAA:(void*)(_tmp155->sc=(void*)s);_tmp155->attributes=_tmp105;
_tmp105=0;goto _LLA8;_LLAB: if(*((int*)_tmp154)!= 7)goto _LLAD;_tmp156=((struct Cyc_Absyn_Datatype_d_struct*)
_tmp154)->f1;_LLAC:(void*)(_tmp156->sc=(void*)s);goto _LLA8;_LLAD: if(*((int*)
_tmp154)!= 8)goto _LLAF;_tmp157=((struct Cyc_Absyn_Enum_d_struct*)_tmp154)->f1;
_LLAE:(void*)(_tmp157->sc=(void*)s);goto _LLA8;_LLAF:;_LLB0:{const char*_tmp9E1;
Cyc_Parse_err(((_tmp9E1="declaration within typedef is not a struct, union, or datatype",
_tag_dyneither(_tmp9E1,sizeof(char),63))),loc);}goto _LLA8;_LLA8:;}{struct Cyc_List_List*
_tmp9E2;decls=((_tmp9E2=_cycalloc(sizeof(*_tmp9E2)),((_tmp9E2->hd=d,((_tmp9E2->tl=
decls,_tmp9E2))))));}}{struct Cyc_List_List*_tmp15A=decls;_npop_handler(0);return
_tmp15A;}}}else{if(_tmp10D.f2 != 0){const char*_tmp9E3;Cyc_Parse_unimp(((_tmp9E3="nested type declaration within declarator",
_tag_dyneither(_tmp9E3,sizeof(char),42))),loc);}{struct Cyc_List_List*decls=0;{
struct Cyc_List_List*_tmp15C=_tmp152;for(0;_tmp15C != 0;(_tmp15C=_tmp15C->tl,
_tmp10C=_tmp10C->tl)){struct _tuple7 _tmp15E;struct _tuple1*_tmp15F;struct Cyc_Absyn_Tqual
_tmp160;void*_tmp161;struct Cyc_List_List*_tmp162;struct Cyc_List_List*_tmp163;
struct _tuple7*_tmp15D=(struct _tuple7*)_tmp15C->hd;_tmp15E=*_tmp15D;_tmp15F=
_tmp15E.f1;_tmp160=_tmp15E.f2;_tmp161=_tmp15E.f3;_tmp162=_tmp15E.f4;_tmp163=
_tmp15E.f5;if(_tmp162 != 0){const char*_tmp9E6;void*_tmp9E5;(_tmp9E5=0,Cyc_Tcutil_warn(
loc,((_tmp9E6="bad type params, ignoring",_tag_dyneither(_tmp9E6,sizeof(char),26))),
_tag_dyneither(_tmp9E5,sizeof(void*),0)));}if(_tmp10C == 0){const char*_tmp9E9;
void*_tmp9E8;(_tmp9E8=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp9E9="unexpected NULL in parse!",
_tag_dyneither(_tmp9E9,sizeof(char),26))),_tag_dyneither(_tmp9E8,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp168=(struct Cyc_Absyn_Exp*)_tmp10C->hd;struct Cyc_Absyn_Vardecl*
_tmp169=Cyc_Absyn_new_vardecl(_tmp15F,_tmp161,_tmp168);_tmp169->tq=_tmp160;(void*)(
_tmp169->sc=(void*)s);_tmp169->attributes=_tmp163;{struct Cyc_Absyn_Var_d_struct*
_tmp9EF;struct Cyc_Absyn_Var_d_struct _tmp9EE;struct Cyc_Absyn_Decl*_tmp9ED;struct
Cyc_Absyn_Decl*_tmp16A=(_tmp9ED=_cycalloc(sizeof(*_tmp9ED)),((_tmp9ED->r=(void*)((
void*)((_tmp9EF=_cycalloc(sizeof(*_tmp9EF)),((_tmp9EF[0]=((_tmp9EE.tag=0,((
_tmp9EE.f1=_tmp169,_tmp9EE)))),_tmp9EF))))),((_tmp9ED->loc=loc,_tmp9ED)))));
struct Cyc_List_List*_tmp9F0;decls=((_tmp9F0=_cycalloc(sizeof(*_tmp9F0)),((
_tmp9F0->hd=_tmp16A,((_tmp9F0->tl=decls,_tmp9F0))))));}}}}{struct Cyc_List_List*
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
const char*_tmp9F5;void*_tmp9F4[2];struct Cyc_String_pa_struct _tmp9F3;struct Cyc_Int_pa_struct
_tmp9F2;Cyc_Parse_err((struct _dyneither_ptr)((_tmp9F2.tag=1,((_tmp9F2.f1=(
unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp9F3.tag=0,((
_tmp9F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp9F4[0]=& _tmp9F3,((
_tmp9F4[1]=& _tmp9F2,Cyc_aprintf(((_tmp9F5="bad kind: %s; strlen=%d",
_tag_dyneither(_tmp9F5,sizeof(char),24))),_tag_dyneither(_tmp9F4,sizeof(void*),2)))))))))))))),
loc);}return(void*)2;}static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*
p);static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*
_tmp174=(void*)p->r;struct _tuple1*_tmp175;struct Cyc_Absyn_Vardecl*_tmp176;struct
Cyc_Absyn_Pat*_tmp177;struct Cyc_Absyn_Pat _tmp178;void*_tmp179;struct Cyc_Absyn_Pat*
_tmp17A;void*_tmp17B;int _tmp17C;char _tmp17D;struct _dyneither_ptr _tmp17E;struct
_tuple1*_tmp17F;struct Cyc_List_List*_tmp180;int _tmp181;struct Cyc_Absyn_Exp*
_tmp182;_LLBC: if(_tmp174 <= (void*)2)goto _LLC2;if(*((int*)_tmp174)!= 12)goto _LLBE;
_tmp175=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp174)->f1;_LLBD: {struct Cyc_Absyn_UnknownId_e_struct
_tmp9F8;struct Cyc_Absyn_UnknownId_e_struct*_tmp9F7;return Cyc_Absyn_new_exp((void*)((
_tmp9F7=_cycalloc(sizeof(*_tmp9F7)),((_tmp9F7[0]=((_tmp9F8.tag=2,((_tmp9F8.f1=
_tmp175,_tmp9F8)))),_tmp9F7)))),p->loc);}_LLBE: if(*((int*)_tmp174)!= 1)goto _LLC0;
_tmp176=((struct Cyc_Absyn_Reference_p_struct*)_tmp174)->f1;_tmp177=((struct Cyc_Absyn_Reference_p_struct*)
_tmp174)->f2;_tmp178=*_tmp177;_tmp179=(void*)_tmp178.r;if((int)_tmp179 != 0)goto
_LLC0;_LLBF: {struct Cyc_Absyn_UnknownId_e_struct _tmp9FB;struct Cyc_Absyn_UnknownId_e_struct*
_tmp9FA;return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)((_tmp9FA=_cycalloc(
sizeof(*_tmp9FA)),((_tmp9FA[0]=((_tmp9FB.tag=2,((_tmp9FB.f1=_tmp176->name,
_tmp9FB)))),_tmp9FA)))),p->loc),p->loc);}_LLC0: if(*((int*)_tmp174)!= 4)goto _LLC2;
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
_tmp9FE;struct Cyc_Absyn_UnknownId_e_struct*_tmp9FD;struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((
void*)((_tmp9FD=_cycalloc(sizeof(*_tmp9FD)),((_tmp9FD[0]=((_tmp9FE.tag=2,((
_tmp9FE.f1=_tmp17F,_tmp9FE)))),_tmp9FD)))),p->loc);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp180);return Cyc_Absyn_unknowncall_exp(e1,
es,p->loc);}_LLCC: if(*((int*)_tmp174)!= 14)goto _LLCE;_tmp182=((struct Cyc_Absyn_Exp_p_struct*)
_tmp174)->f1;_LLCD: return _tmp182;_LLCE:;_LLCF:{const char*_tmp9FF;Cyc_Parse_err(((
_tmp9FF="cannot mix patterns and expressions in case",_tag_dyneither(_tmp9FF,
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
Cyc_Absyn_Datatypefield*f1;};struct Cyc_YY34_struct{int tag;struct Cyc_List_List*f1;
};struct _tuple16{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*
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
f1;};struct Cyc_YY52_struct{int tag;struct Cyc_List_List*f1;};struct _tuple18{int f1;
int f2;};struct Cyc_YY53_struct{int tag;struct _tuple18 f1;};struct Cyc_YYINITIALSVAL_struct{
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
struct Cyc_YY51_struct YY51;struct Cyc_YY52_struct YY52;struct Cyc_YY53_struct YY53;
struct Cyc_YYINITIALSVAL_struct YYINITIALSVAL;};static char _tmp18B[14]="$(sign_t,int)";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp18B,
_tmp18B,_tmp18B + 14}};struct _tuple11 Cyc_yyget_Int_tok(union Cyc_YYSTYPE_union yy1);
struct _tuple11 Cyc_yyget_Int_tok(union Cyc_YYSTYPE_union yy1){struct _tuple11 yyzzz;{
union Cyc_YYSTYPE_union _tmp18C=yy1;struct _tuple11 _tmp18D;_LLD1: if((_tmp18C.Int_tok).tag
!= 0)goto _LLD3;_tmp18D=(_tmp18C.Int_tok).f1;_LLD2: yyzzz=_tmp18D;goto _LLD0;_LLD3:;
_LLD4:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLD0:;}return yyzzz;}static char
_tmp18F[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{
_tmp18F,_tmp18F,_tmp18F + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE_union yy1);
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE_union yy1){char yyzzz;{union Cyc_YYSTYPE_union
_tmp190=yy1;char _tmp191;_LLD6: if((_tmp190.Char_tok).tag != 1)goto _LLD8;_tmp191=(
_tmp190.Char_tok).f1;_LLD7: yyzzz=_tmp191;goto _LLD5;_LLD8:;_LLD9:(int)_throw((
void*)& Cyc_yyfail_Char_tok);_LLD5:;}return yyzzz;}static char _tmp193[9]="string_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{
_tmp193,_tmp193,_tmp193 + 9}};struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE_union
yy1);struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE_union yy1){struct
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
char _tmp21B[16]="datatypefield_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY33={
Cyc_Core_Failure,{_tmp21B,_tmp21B,_tmp21B + 16}};struct Cyc_Absyn_Datatypefield*
Cyc_yyget_YY33(union Cyc_YYSTYPE_union yy1);struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Datatypefield*yyzzz;{union Cyc_YYSTYPE_union
_tmp21C=yy1;struct Cyc_Absyn_Datatypefield*_tmp21D;_LL185: if((_tmp21C.YY33).tag != 
37)goto _LL187;_tmp21D=(_tmp21C.YY33).f1;_LL186: yyzzz=_tmp21D;goto _LL184;_LL187:;
_LL188:(int)_throw((void*)& Cyc_yyfail_YY33);_LL184:;}return yyzzz;}static char
_tmp21F[24]="list_t<datatypefield_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY34={
Cyc_Core_Failure,{_tmp21F,_tmp21F,_tmp21F + 24}};struct Cyc_List_List*Cyc_yyget_YY34(
union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp220=yy1;struct Cyc_List_List*
_tmp221;_LL18A: if((_tmp220.YY34).tag != 38)goto _LL18C;_tmp221=(_tmp220.YY34).f1;
_LL18B: yyzzz=_tmp221;goto _LL189;_LL18C:;_LL18D:(int)_throw((void*)& Cyc_yyfail_YY34);
_LL189:;}return yyzzz;}static char _tmp223[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
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
int)_throw((void*)& Cyc_yyfail_YY52);_LL1E3:;}return yyzzz;}static char _tmp26B[13]="$(bool,bool)";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp26B,
_tmp26B,_tmp26B + 13}};struct _tuple18 Cyc_yyget_YY53(union Cyc_YYSTYPE_union yy1);
struct _tuple18 Cyc_yyget_YY53(union Cyc_YYSTYPE_union yy1){struct _tuple18 yyzzz;{
union Cyc_YYSTYPE_union _tmp26C=yy1;struct _tuple18 _tmp26D;_LL1E9: if((_tmp26C.YY53).tag
!= 57)goto _LL1EB;_tmp26D=(_tmp26C.YY53).f1;_LL1EA: yyzzz=_tmp26D;goto _LL1E8;
_LL1EB:;_LL1EC:(int)_throw((void*)& Cyc_yyfail_YY53);_LL1E8:;}return yyzzz;}struct
Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int
last_column;};struct Cyc_Yyltype Cyc_yynewloc();struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmpA00;return(_tmpA00.timestamp=0,((_tmpA00.first_line=0,((
_tmpA00.first_column=0,((_tmpA00.last_line=0,((_tmpA00.last_column=0,_tmpA00)))))))));}
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[366]={0,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,137,2,2,121,135,132,2,
118,119,126,129,114,133,123,134,2,2,2,2,2,2,2,2,2,2,122,111,116,115,117,128,127,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,124,2,125,131,120,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,112,130,113,136,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,
58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,
85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,
109,110};static char _tmp26F[2]="$";static char _tmp270[6]="error";static char _tmp271[
12]="$undefined.";static char _tmp272[5]="AUTO";static char _tmp273[9]="REGISTER";
static char _tmp274[7]="STATIC";static char _tmp275[7]="EXTERN";static char _tmp276[8]="TYPEDEF";
static char _tmp277[5]="VOID";static char _tmp278[5]="CHAR";static char _tmp279[6]="SHORT";
static char _tmp27A[4]="INT";static char _tmp27B[5]="LONG";static char _tmp27C[6]="FLOAT";
static char _tmp27D[7]="DOUBLE";static char _tmp27E[7]="SIGNED";static char _tmp27F[9]="UNSIGNED";
static char _tmp280[6]="CONST";static char _tmp281[9]="VOLATILE";static char _tmp282[9]="RESTRICT";
static char _tmp283[7]="STRUCT";static char _tmp284[6]="UNION";static char _tmp285[5]="CASE";
static char _tmp286[8]="DEFAULT";static char _tmp287[7]="INLINE";static char _tmp288[7]="SIZEOF";
static char _tmp289[9]="OFFSETOF";static char _tmp28A[3]="IF";static char _tmp28B[5]="ELSE";
static char _tmp28C[7]="SWITCH";static char _tmp28D[6]="WHILE";static char _tmp28E[3]="DO";
static char _tmp28F[4]="FOR";static char _tmp290[5]="GOTO";static char _tmp291[9]="CONTINUE";
static char _tmp292[6]="BREAK";static char _tmp293[7]="RETURN";static char _tmp294[5]="ENUM";
static char _tmp295[8]="NULL_kw";static char _tmp296[4]="LET";static char _tmp297[6]="THROW";
static char _tmp298[4]="TRY";static char _tmp299[6]="CATCH";static char _tmp29A[7]="EXPORT";
static char _tmp29B[4]="NEW";static char _tmp29C[9]="ABSTRACT";static char _tmp29D[9]="FALLTHRU";
static char _tmp29E[6]="USING";static char _tmp29F[10]="NAMESPACE";static char _tmp2A0[
9]="DATATYPE";static char _tmp2A1[8]="XTUNION";static char _tmp2A2[7]="TUNION";
static char _tmp2A3[7]="MALLOC";static char _tmp2A4[8]="RMALLOC";static char _tmp2A5[7]="CALLOC";
static char _tmp2A6[8]="RCALLOC";static char _tmp2A7[5]="SWAP";static char _tmp2A8[9]="REGION_T";
static char _tmp2A9[6]="TAG_T";static char _tmp2AA[7]="REGION";static char _tmp2AB[5]="RNEW";
static char _tmp2AC[8]="REGIONS";static char _tmp2AD[13]="RESET_REGION";static char
_tmp2AE[4]="GEN";static char _tmp2AF[16]="NOZEROTERM_QUAL";static char _tmp2B0[14]="ZEROTERM_QUAL";
static char _tmp2B1[7]="PORTON";static char _tmp2B2[8]="PORTOFF";static char _tmp2B3[8]="FLAT_kw";
static char _tmp2B4[12]="DYNREGION_T";static char _tmp2B5[6]="ALIAS";static char
_tmp2B6[8]="NUMELTS";static char _tmp2B7[8]="VALUEOF";static char _tmp2B8[10]="VALUEOF_T";
static char _tmp2B9[12]="TAGGED_QUAL";static char _tmp2BA[16]="EXTENSIBLE_QUAL";
static char _tmp2BB[15]="RESETABLE_QUAL";static char _tmp2BC[7]="PTR_OP";static char
_tmp2BD[7]="INC_OP";static char _tmp2BE[7]="DEC_OP";static char _tmp2BF[8]="LEFT_OP";
static char _tmp2C0[9]="RIGHT_OP";static char _tmp2C1[6]="LE_OP";static char _tmp2C2[6]="GE_OP";
static char _tmp2C3[6]="EQ_OP";static char _tmp2C4[6]="NE_OP";static char _tmp2C5[7]="AND_OP";
static char _tmp2C6[6]="OR_OP";static char _tmp2C7[11]="MUL_ASSIGN";static char
_tmp2C8[11]="DIV_ASSIGN";static char _tmp2C9[11]="MOD_ASSIGN";static char _tmp2CA[11]="ADD_ASSIGN";
static char _tmp2CB[11]="SUB_ASSIGN";static char _tmp2CC[12]="LEFT_ASSIGN";static
char _tmp2CD[13]="RIGHT_ASSIGN";static char _tmp2CE[11]="AND_ASSIGN";static char
_tmp2CF[11]="XOR_ASSIGN";static char _tmp2D0[10]="OR_ASSIGN";static char _tmp2D1[9]="ELLIPSIS";
static char _tmp2D2[11]="LEFT_RIGHT";static char _tmp2D3[12]="COLON_COLON";static
char _tmp2D4[11]="IDENTIFIER";static char _tmp2D5[17]="INTEGER_CONSTANT";static char
_tmp2D6[7]="STRING";static char _tmp2D7[19]="CHARACTER_CONSTANT";static char _tmp2D8[
18]="FLOATING_CONSTANT";static char _tmp2D9[9]="TYPE_VAR";static char _tmp2DA[13]="TYPEDEF_NAME";
static char _tmp2DB[16]="QUAL_IDENTIFIER";static char _tmp2DC[18]="QUAL_TYPEDEF_NAME";
static char _tmp2DD[10]="ATTRIBUTE";static char _tmp2DE[4]="';'";static char _tmp2DF[4]="'{'";
static char _tmp2E0[4]="'}'";static char _tmp2E1[4]="','";static char _tmp2E2[4]="'='";
static char _tmp2E3[4]="'<'";static char _tmp2E4[4]="'>'";static char _tmp2E5[4]="'('";
static char _tmp2E6[4]="')'";static char _tmp2E7[4]="'_'";static char _tmp2E8[4]="'$'";
static char _tmp2E9[4]="':'";static char _tmp2EA[4]="'.'";static char _tmp2EB[4]="'['";
static char _tmp2EC[4]="']'";static char _tmp2ED[4]="'*'";static char _tmp2EE[4]="'@'";
static char _tmp2EF[4]="'?'";static char _tmp2F0[4]="'+'";static char _tmp2F1[4]="'|'";
static char _tmp2F2[4]="'^'";static char _tmp2F3[4]="'&'";static char _tmp2F4[4]="'-'";
static char _tmp2F5[4]="'/'";static char _tmp2F6[4]="'%'";static char _tmp2F7[4]="'~'";
static char _tmp2F8[4]="'!'";static char _tmp2F9[5]="prog";static char _tmp2FA[17]="translation_unit";
static char _tmp2FB[12]="export_list";static char _tmp2FC[19]="export_list_values";
static char _tmp2FD[21]="external_declaration";static char _tmp2FE[20]="function_definition";
static char _tmp2FF[21]="function_definition2";static char _tmp300[13]="using_action";
static char _tmp301[15]="unusing_action";static char _tmp302[17]="namespace_action";
static char _tmp303[19]="unnamespace_action";static char _tmp304[12]="declaration";
static char _tmp305[19]="resetable_qual_opt";static char _tmp306[17]="declaration_list";
static char _tmp307[23]="declaration_specifiers";static char _tmp308[24]="storage_class_specifier";
static char _tmp309[15]="attributes_opt";static char _tmp30A[11]="attributes";static
char _tmp30B[15]="attribute_list";static char _tmp30C[10]="attribute";static char
_tmp30D[15]="type_specifier";static char _tmp30E[25]="type_specifier_notypedef";
static char _tmp30F[5]="kind";static char _tmp310[15]="type_qualifier";static char
_tmp311[15]="enum_specifier";static char _tmp312[11]="enum_field";static char
_tmp313[22]="enum_declaration_list";static char _tmp314[26]="struct_or_union_specifier";
static char _tmp315[16]="type_params_opt";static char _tmp316[16]="struct_or_union";
static char _tmp317[24]="struct_declaration_list";static char _tmp318[25]="struct_declaration_list0";
static char _tmp319[21]="init_declarator_list";static char _tmp31A[22]="init_declarator_list0";
static char _tmp31B[16]="init_declarator";static char _tmp31C[19]="struct_declaration";
static char _tmp31D[25]="specifier_qualifier_list";static char _tmp31E[35]="notypedef_specifier_qualifier_list";
static char _tmp31F[23]="struct_declarator_list";static char _tmp320[24]="struct_declarator_list0";
static char _tmp321[18]="struct_declarator";static char _tmp322[19]="datatype_specifier";
static char _tmp323[14]="qual_datatype";static char _tmp324[19]="datatypefield_list";
static char _tmp325[20]="datatypefield_scope";static char _tmp326[14]="datatypefield";
static char _tmp327[11]="declarator";static char _tmp328[23]="declarator_withtypedef";
static char _tmp329[18]="direct_declarator";static char _tmp32A[30]="direct_declarator_withtypedef";
static char _tmp32B[8]="pointer";static char _tmp32C[12]="one_pointer";static char
_tmp32D[23]="pointer_null_and_bound";static char _tmp32E[14]="pointer_bound";
static char _tmp32F[18]="zeroterm_qual_opt";static char _tmp330[12]="opt_rgn_opt";
static char _tmp331[8]="rgn_opt";static char _tmp332[11]="tqual_list";static char
_tmp333[20]="parameter_type_list";static char _tmp334[9]="type_var";static char
_tmp335[16]="optional_effect";static char _tmp336[19]="optional_rgn_order";static
char _tmp337[10]="rgn_order";static char _tmp338[16]="optional_inject";static char
_tmp339[11]="effect_set";static char _tmp33A[14]="atomic_effect";static char _tmp33B[
11]="region_set";static char _tmp33C[15]="parameter_list";static char _tmp33D[22]="parameter_declaration";
static char _tmp33E[16]="identifier_list";static char _tmp33F[17]="identifier_list0";
static char _tmp340[12]="initializer";static char _tmp341[18]="array_initializer";
static char _tmp342[17]="initializer_list";static char _tmp343[12]="designation";
static char _tmp344[16]="designator_list";static char _tmp345[11]="designator";
static char _tmp346[10]="type_name";static char _tmp347[14]="any_type_name";static
char _tmp348[15]="type_name_list";static char _tmp349[20]="abstract_declarator";
static char _tmp34A[27]="direct_abstract_declarator";static char _tmp34B[10]="statement";
static char _tmp34C[13]="open_exp_opt";static char _tmp34D[18]="labeled_statement";
static char _tmp34E[21]="expression_statement";static char _tmp34F[19]="compound_statement";
static char _tmp350[16]="block_item_list";static char _tmp351[20]="selection_statement";
static char _tmp352[15]="switch_clauses";static char _tmp353[20]="iteration_statement";
static char _tmp354[15]="jump_statement";static char _tmp355[12]="exp_pattern";
static char _tmp356[20]="conditional_pattern";static char _tmp357[19]="logical_or_pattern";
static char _tmp358[20]="logical_and_pattern";static char _tmp359[21]="inclusive_or_pattern";
static char _tmp35A[21]="exclusive_or_pattern";static char _tmp35B[12]="and_pattern";
static char _tmp35C[17]="equality_pattern";static char _tmp35D[19]="relational_pattern";
static char _tmp35E[14]="shift_pattern";static char _tmp35F[17]="additive_pattern";
static char _tmp360[23]="multiplicative_pattern";static char _tmp361[13]="cast_pattern";
static char _tmp362[14]="unary_pattern";static char _tmp363[16]="postfix_pattern";
static char _tmp364[16]="primary_pattern";static char _tmp365[8]="pattern";static
char _tmp366[19]="tuple_pattern_list";static char _tmp367[20]="tuple_pattern_list0";
static char _tmp368[14]="field_pattern";static char _tmp369[19]="field_pattern_list";
static char _tmp36A[20]="field_pattern_list0";static char _tmp36B[11]="expression";
static char _tmp36C[22]="assignment_expression";static char _tmp36D[20]="assignment_operator";
static char _tmp36E[23]="conditional_expression";static char _tmp36F[20]="constant_expression";
static char _tmp370[22]="logical_or_expression";static char _tmp371[23]="logical_and_expression";
static char _tmp372[24]="inclusive_or_expression";static char _tmp373[24]="exclusive_or_expression";
static char _tmp374[15]="and_expression";static char _tmp375[20]="equality_expression";
static char _tmp376[22]="relational_expression";static char _tmp377[17]="shift_expression";
static char _tmp378[20]="additive_expression";static char _tmp379[26]="multiplicative_expression";
static char _tmp37A[16]="cast_expression";static char _tmp37B[17]="unary_expression";
static char _tmp37C[15]="unary_operator";static char _tmp37D[19]="postfix_expression";
static char _tmp37E[19]="primary_expression";static char _tmp37F[25]="argument_expression_list";
static char _tmp380[26]="argument_expression_list0";static char _tmp381[9]="constant";
static char _tmp382[20]="qual_opt_identifier";static char _tmp383[17]="qual_opt_typedef";
static char _tmp384[18]="struct_union_name";static char _tmp385[11]="field_name";
static char _tmp386[12]="right_angle";static struct _dyneither_ptr Cyc_yytname[280]={{
_tmp26F,_tmp26F,_tmp26F + 2},{_tmp270,_tmp270,_tmp270 + 6},{_tmp271,_tmp271,
_tmp271 + 12},{_tmp272,_tmp272,_tmp272 + 5},{_tmp273,_tmp273,_tmp273 + 9},{_tmp274,
_tmp274,_tmp274 + 7},{_tmp275,_tmp275,_tmp275 + 7},{_tmp276,_tmp276,_tmp276 + 8},{
_tmp277,_tmp277,_tmp277 + 5},{_tmp278,_tmp278,_tmp278 + 5},{_tmp279,_tmp279,
_tmp279 + 6},{_tmp27A,_tmp27A,_tmp27A + 4},{_tmp27B,_tmp27B,_tmp27B + 5},{_tmp27C,
_tmp27C,_tmp27C + 6},{_tmp27D,_tmp27D,_tmp27D + 7},{_tmp27E,_tmp27E,_tmp27E + 7},{
_tmp27F,_tmp27F,_tmp27F + 9},{_tmp280,_tmp280,_tmp280 + 6},{_tmp281,_tmp281,
_tmp281 + 9},{_tmp282,_tmp282,_tmp282 + 9},{_tmp283,_tmp283,_tmp283 + 7},{_tmp284,
_tmp284,_tmp284 + 6},{_tmp285,_tmp285,_tmp285 + 5},{_tmp286,_tmp286,_tmp286 + 8},{
_tmp287,_tmp287,_tmp287 + 7},{_tmp288,_tmp288,_tmp288 + 7},{_tmp289,_tmp289,
_tmp289 + 9},{_tmp28A,_tmp28A,_tmp28A + 3},{_tmp28B,_tmp28B,_tmp28B + 5},{_tmp28C,
_tmp28C,_tmp28C + 7},{_tmp28D,_tmp28D,_tmp28D + 6},{_tmp28E,_tmp28E,_tmp28E + 3},{
_tmp28F,_tmp28F,_tmp28F + 4},{_tmp290,_tmp290,_tmp290 + 5},{_tmp291,_tmp291,
_tmp291 + 9},{_tmp292,_tmp292,_tmp292 + 6},{_tmp293,_tmp293,_tmp293 + 7},{_tmp294,
_tmp294,_tmp294 + 5},{_tmp295,_tmp295,_tmp295 + 8},{_tmp296,_tmp296,_tmp296 + 4},{
_tmp297,_tmp297,_tmp297 + 6},{_tmp298,_tmp298,_tmp298 + 4},{_tmp299,_tmp299,
_tmp299 + 6},{_tmp29A,_tmp29A,_tmp29A + 7},{_tmp29B,_tmp29B,_tmp29B + 4},{_tmp29C,
_tmp29C,_tmp29C + 9},{_tmp29D,_tmp29D,_tmp29D + 9},{_tmp29E,_tmp29E,_tmp29E + 6},{
_tmp29F,_tmp29F,_tmp29F + 10},{_tmp2A0,_tmp2A0,_tmp2A0 + 9},{_tmp2A1,_tmp2A1,
_tmp2A1 + 8},{_tmp2A2,_tmp2A2,_tmp2A2 + 7},{_tmp2A3,_tmp2A3,_tmp2A3 + 7},{_tmp2A4,
_tmp2A4,_tmp2A4 + 8},{_tmp2A5,_tmp2A5,_tmp2A5 + 7},{_tmp2A6,_tmp2A6,_tmp2A6 + 8},{
_tmp2A7,_tmp2A7,_tmp2A7 + 5},{_tmp2A8,_tmp2A8,_tmp2A8 + 9},{_tmp2A9,_tmp2A9,
_tmp2A9 + 6},{_tmp2AA,_tmp2AA,_tmp2AA + 7},{_tmp2AB,_tmp2AB,_tmp2AB + 5},{_tmp2AC,
_tmp2AC,_tmp2AC + 8},{_tmp2AD,_tmp2AD,_tmp2AD + 13},{_tmp2AE,_tmp2AE,_tmp2AE + 4},{
_tmp2AF,_tmp2AF,_tmp2AF + 16},{_tmp2B0,_tmp2B0,_tmp2B0 + 14},{_tmp2B1,_tmp2B1,
_tmp2B1 + 7},{_tmp2B2,_tmp2B2,_tmp2B2 + 8},{_tmp2B3,_tmp2B3,_tmp2B3 + 8},{_tmp2B4,
_tmp2B4,_tmp2B4 + 12},{_tmp2B5,_tmp2B5,_tmp2B5 + 6},{_tmp2B6,_tmp2B6,_tmp2B6 + 8},{
_tmp2B7,_tmp2B7,_tmp2B7 + 8},{_tmp2B8,_tmp2B8,_tmp2B8 + 10},{_tmp2B9,_tmp2B9,
_tmp2B9 + 12},{_tmp2BA,_tmp2BA,_tmp2BA + 16},{_tmp2BB,_tmp2BB,_tmp2BB + 15},{
_tmp2BC,_tmp2BC,_tmp2BC + 7},{_tmp2BD,_tmp2BD,_tmp2BD + 7},{_tmp2BE,_tmp2BE,
_tmp2BE + 7},{_tmp2BF,_tmp2BF,_tmp2BF + 8},{_tmp2C0,_tmp2C0,_tmp2C0 + 9},{_tmp2C1,
_tmp2C1,_tmp2C1 + 6},{_tmp2C2,_tmp2C2,_tmp2C2 + 6},{_tmp2C3,_tmp2C3,_tmp2C3 + 6},{
_tmp2C4,_tmp2C4,_tmp2C4 + 6},{_tmp2C5,_tmp2C5,_tmp2C5 + 7},{_tmp2C6,_tmp2C6,
_tmp2C6 + 6},{_tmp2C7,_tmp2C7,_tmp2C7 + 11},{_tmp2C8,_tmp2C8,_tmp2C8 + 11},{_tmp2C9,
_tmp2C9,_tmp2C9 + 11},{_tmp2CA,_tmp2CA,_tmp2CA + 11},{_tmp2CB,_tmp2CB,_tmp2CB + 11},{
_tmp2CC,_tmp2CC,_tmp2CC + 12},{_tmp2CD,_tmp2CD,_tmp2CD + 13},{_tmp2CE,_tmp2CE,
_tmp2CE + 11},{_tmp2CF,_tmp2CF,_tmp2CF + 11},{_tmp2D0,_tmp2D0,_tmp2D0 + 10},{
_tmp2D1,_tmp2D1,_tmp2D1 + 9},{_tmp2D2,_tmp2D2,_tmp2D2 + 11},{_tmp2D3,_tmp2D3,
_tmp2D3 + 12},{_tmp2D4,_tmp2D4,_tmp2D4 + 11},{_tmp2D5,_tmp2D5,_tmp2D5 + 17},{
_tmp2D6,_tmp2D6,_tmp2D6 + 7},{_tmp2D7,_tmp2D7,_tmp2D7 + 19},{_tmp2D8,_tmp2D8,
_tmp2D8 + 18},{_tmp2D9,_tmp2D9,_tmp2D9 + 9},{_tmp2DA,_tmp2DA,_tmp2DA + 13},{_tmp2DB,
_tmp2DB,_tmp2DB + 16},{_tmp2DC,_tmp2DC,_tmp2DC + 18},{_tmp2DD,_tmp2DD,_tmp2DD + 10},{
_tmp2DE,_tmp2DE,_tmp2DE + 4},{_tmp2DF,_tmp2DF,_tmp2DF + 4},{_tmp2E0,_tmp2E0,
_tmp2E0 + 4},{_tmp2E1,_tmp2E1,_tmp2E1 + 4},{_tmp2E2,_tmp2E2,_tmp2E2 + 4},{_tmp2E3,
_tmp2E3,_tmp2E3 + 4},{_tmp2E4,_tmp2E4,_tmp2E4 + 4},{_tmp2E5,_tmp2E5,_tmp2E5 + 4},{
_tmp2E6,_tmp2E6,_tmp2E6 + 4},{_tmp2E7,_tmp2E7,_tmp2E7 + 4},{_tmp2E8,_tmp2E8,
_tmp2E8 + 4},{_tmp2E9,_tmp2E9,_tmp2E9 + 4},{_tmp2EA,_tmp2EA,_tmp2EA + 4},{_tmp2EB,
_tmp2EB,_tmp2EB + 4},{_tmp2EC,_tmp2EC,_tmp2EC + 4},{_tmp2ED,_tmp2ED,_tmp2ED + 4},{
_tmp2EE,_tmp2EE,_tmp2EE + 4},{_tmp2EF,_tmp2EF,_tmp2EF + 4},{_tmp2F0,_tmp2F0,
_tmp2F0 + 4},{_tmp2F1,_tmp2F1,_tmp2F1 + 4},{_tmp2F2,_tmp2F2,_tmp2F2 + 4},{_tmp2F3,
_tmp2F3,_tmp2F3 + 4},{_tmp2F4,_tmp2F4,_tmp2F4 + 4},{_tmp2F5,_tmp2F5,_tmp2F5 + 4},{
_tmp2F6,_tmp2F6,_tmp2F6 + 4},{_tmp2F7,_tmp2F7,_tmp2F7 + 4},{_tmp2F8,_tmp2F8,
_tmp2F8 + 4},{_tmp2F9,_tmp2F9,_tmp2F9 + 5},{_tmp2FA,_tmp2FA,_tmp2FA + 17},{_tmp2FB,
_tmp2FB,_tmp2FB + 12},{_tmp2FC,_tmp2FC,_tmp2FC + 19},{_tmp2FD,_tmp2FD,_tmp2FD + 21},{
_tmp2FE,_tmp2FE,_tmp2FE + 20},{_tmp2FF,_tmp2FF,_tmp2FF + 21},{_tmp300,_tmp300,
_tmp300 + 13},{_tmp301,_tmp301,_tmp301 + 15},{_tmp302,_tmp302,_tmp302 + 17},{
_tmp303,_tmp303,_tmp303 + 19},{_tmp304,_tmp304,_tmp304 + 12},{_tmp305,_tmp305,
_tmp305 + 19},{_tmp306,_tmp306,_tmp306 + 17},{_tmp307,_tmp307,_tmp307 + 23},{
_tmp308,_tmp308,_tmp308 + 24},{_tmp309,_tmp309,_tmp309 + 15},{_tmp30A,_tmp30A,
_tmp30A + 11},{_tmp30B,_tmp30B,_tmp30B + 15},{_tmp30C,_tmp30C,_tmp30C + 10},{
_tmp30D,_tmp30D,_tmp30D + 15},{_tmp30E,_tmp30E,_tmp30E + 25},{_tmp30F,_tmp30F,
_tmp30F + 5},{_tmp310,_tmp310,_tmp310 + 15},{_tmp311,_tmp311,_tmp311 + 15},{_tmp312,
_tmp312,_tmp312 + 11},{_tmp313,_tmp313,_tmp313 + 22},{_tmp314,_tmp314,_tmp314 + 26},{
_tmp315,_tmp315,_tmp315 + 16},{_tmp316,_tmp316,_tmp316 + 16},{_tmp317,_tmp317,
_tmp317 + 24},{_tmp318,_tmp318,_tmp318 + 25},{_tmp319,_tmp319,_tmp319 + 21},{
_tmp31A,_tmp31A,_tmp31A + 22},{_tmp31B,_tmp31B,_tmp31B + 16},{_tmp31C,_tmp31C,
_tmp31C + 19},{_tmp31D,_tmp31D,_tmp31D + 25},{_tmp31E,_tmp31E,_tmp31E + 35},{
_tmp31F,_tmp31F,_tmp31F + 23},{_tmp320,_tmp320,_tmp320 + 24},{_tmp321,_tmp321,
_tmp321 + 18},{_tmp322,_tmp322,_tmp322 + 19},{_tmp323,_tmp323,_tmp323 + 14},{
_tmp324,_tmp324,_tmp324 + 19},{_tmp325,_tmp325,_tmp325 + 20},{_tmp326,_tmp326,
_tmp326 + 14},{_tmp327,_tmp327,_tmp327 + 11},{_tmp328,_tmp328,_tmp328 + 23},{
_tmp329,_tmp329,_tmp329 + 18},{_tmp32A,_tmp32A,_tmp32A + 30},{_tmp32B,_tmp32B,
_tmp32B + 8},{_tmp32C,_tmp32C,_tmp32C + 12},{_tmp32D,_tmp32D,_tmp32D + 23},{_tmp32E,
_tmp32E,_tmp32E + 14},{_tmp32F,_tmp32F,_tmp32F + 18},{_tmp330,_tmp330,_tmp330 + 12},{
_tmp331,_tmp331,_tmp331 + 8},{_tmp332,_tmp332,_tmp332 + 11},{_tmp333,_tmp333,
_tmp333 + 20},{_tmp334,_tmp334,_tmp334 + 9},{_tmp335,_tmp335,_tmp335 + 16},{_tmp336,
_tmp336,_tmp336 + 19},{_tmp337,_tmp337,_tmp337 + 10},{_tmp338,_tmp338,_tmp338 + 16},{
_tmp339,_tmp339,_tmp339 + 11},{_tmp33A,_tmp33A,_tmp33A + 14},{_tmp33B,_tmp33B,
_tmp33B + 11},{_tmp33C,_tmp33C,_tmp33C + 15},{_tmp33D,_tmp33D,_tmp33D + 22},{
_tmp33E,_tmp33E,_tmp33E + 16},{_tmp33F,_tmp33F,_tmp33F + 17},{_tmp340,_tmp340,
_tmp340 + 12},{_tmp341,_tmp341,_tmp341 + 18},{_tmp342,_tmp342,_tmp342 + 17},{
_tmp343,_tmp343,_tmp343 + 12},{_tmp344,_tmp344,_tmp344 + 16},{_tmp345,_tmp345,
_tmp345 + 11},{_tmp346,_tmp346,_tmp346 + 10},{_tmp347,_tmp347,_tmp347 + 14},{
_tmp348,_tmp348,_tmp348 + 15},{_tmp349,_tmp349,_tmp349 + 20},{_tmp34A,_tmp34A,
_tmp34A + 27},{_tmp34B,_tmp34B,_tmp34B + 10},{_tmp34C,_tmp34C,_tmp34C + 13},{
_tmp34D,_tmp34D,_tmp34D + 18},{_tmp34E,_tmp34E,_tmp34E + 21},{_tmp34F,_tmp34F,
_tmp34F + 19},{_tmp350,_tmp350,_tmp350 + 16},{_tmp351,_tmp351,_tmp351 + 20},{
_tmp352,_tmp352,_tmp352 + 15},{_tmp353,_tmp353,_tmp353 + 20},{_tmp354,_tmp354,
_tmp354 + 15},{_tmp355,_tmp355,_tmp355 + 12},{_tmp356,_tmp356,_tmp356 + 20},{
_tmp357,_tmp357,_tmp357 + 19},{_tmp358,_tmp358,_tmp358 + 20},{_tmp359,_tmp359,
_tmp359 + 21},{_tmp35A,_tmp35A,_tmp35A + 21},{_tmp35B,_tmp35B,_tmp35B + 12},{
_tmp35C,_tmp35C,_tmp35C + 17},{_tmp35D,_tmp35D,_tmp35D + 19},{_tmp35E,_tmp35E,
_tmp35E + 14},{_tmp35F,_tmp35F,_tmp35F + 17},{_tmp360,_tmp360,_tmp360 + 23},{
_tmp361,_tmp361,_tmp361 + 13},{_tmp362,_tmp362,_tmp362 + 14},{_tmp363,_tmp363,
_tmp363 + 16},{_tmp364,_tmp364,_tmp364 + 16},{_tmp365,_tmp365,_tmp365 + 8},{_tmp366,
_tmp366,_tmp366 + 19},{_tmp367,_tmp367,_tmp367 + 20},{_tmp368,_tmp368,_tmp368 + 14},{
_tmp369,_tmp369,_tmp369 + 19},{_tmp36A,_tmp36A,_tmp36A + 20},{_tmp36B,_tmp36B,
_tmp36B + 11},{_tmp36C,_tmp36C,_tmp36C + 22},{_tmp36D,_tmp36D,_tmp36D + 20},{
_tmp36E,_tmp36E,_tmp36E + 23},{_tmp36F,_tmp36F,_tmp36F + 20},{_tmp370,_tmp370,
_tmp370 + 22},{_tmp371,_tmp371,_tmp371 + 23},{_tmp372,_tmp372,_tmp372 + 24},{
_tmp373,_tmp373,_tmp373 + 24},{_tmp374,_tmp374,_tmp374 + 15},{_tmp375,_tmp375,
_tmp375 + 20},{_tmp376,_tmp376,_tmp376 + 22},{_tmp377,_tmp377,_tmp377 + 17},{
_tmp378,_tmp378,_tmp378 + 20},{_tmp379,_tmp379,_tmp379 + 26},{_tmp37A,_tmp37A,
_tmp37A + 16},{_tmp37B,_tmp37B,_tmp37B + 17},{_tmp37C,_tmp37C,_tmp37C + 15},{
_tmp37D,_tmp37D,_tmp37D + 19},{_tmp37E,_tmp37E,_tmp37E + 19},{_tmp37F,_tmp37F,
_tmp37F + 25},{_tmp380,_tmp380,_tmp380 + 26},{_tmp381,_tmp381,_tmp381 + 9},{_tmp382,
_tmp382,_tmp382 + 20},{_tmp383,_tmp383,_tmp383 + 17},{_tmp384,_tmp384,_tmp384 + 18},{
_tmp385,_tmp385,_tmp385 + 11},{_tmp386,_tmp386,_tmp386 + 12}};static short Cyc_yyr1[
500]={0,138,139,139,139,139,139,139,139,139,139,139,140,140,141,141,142,142,142,
143,143,143,143,144,144,145,146,147,148,149,149,149,149,149,149,149,150,150,151,
151,152,152,152,152,152,152,152,152,152,152,153,153,153,153,153,153,153,154,154,
155,156,156,157,157,157,157,157,157,158,158,159,159,159,159,159,159,159,159,159,
159,159,159,159,159,159,159,159,159,159,159,159,159,160,161,161,161,162,162,162,
163,163,164,164,164,165,165,165,165,166,166,167,167,168,168,169,169,170,171,171,
172,172,173,174,174,174,174,174,174,175,175,175,175,175,175,176,177,177,178,178,
178,179,179,179,180,180,180,181,181,181,181,182,182,182,183,183,184,184,185,185,
186,186,186,186,186,186,186,186,186,186,187,187,187,187,187,187,187,187,187,187,
187,188,188,189,190,190,190,191,191,192,192,192,193,193,193,194,194,194,195,195,
196,196,196,196,197,197,198,198,199,199,200,200,201,201,202,202,203,203,203,203,
204,204,205,205,206,206,206,207,208,208,209,209,210,210,210,210,211,211,211,211,
212,213,213,214,214,215,215,216,216,216,216,216,217,217,218,218,218,219,219,219,
219,219,219,219,219,219,219,219,220,220,220,220,220,220,220,221,221,222,223,223,
224,224,225,225,225,225,225,225,226,226,226,226,226,226,227,227,227,227,227,227,
228,228,228,228,228,228,228,228,228,228,228,228,228,228,229,229,229,229,229,229,
229,229,230,231,231,232,232,233,233,234,234,235,235,236,236,237,237,237,238,238,
238,238,238,239,239,239,240,240,240,241,241,241,241,242,242,243,243,243,243,243,
243,243,244,245,246,246,246,246,246,246,246,246,246,246,246,246,246,246,247,247,
247,248,248,249,249,250,250,250,251,251,252,252,253,253,253,254,254,254,254,254,
254,254,254,254,254,254,255,255,255,255,255,255,255,256,257,257,258,258,259,259,
260,260,261,261,262,262,262,263,263,263,263,263,264,264,264,265,265,265,266,266,
266,266,267,267,268,268,268,268,268,268,268,268,268,268,268,268,268,268,268,268,
268,268,269,269,269,270,270,270,270,270,270,270,270,270,270,271,271,271,271,271,
271,271,271,271,272,273,273,274,274,274,274,275,275,276,276,277,277,278,278,279,
279};static short Cyc_yyr2[500]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,1,1,2,3,3,4,3,4,
2,1,2,1,2,3,5,3,7,5,8,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,
4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,
9,8,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,3,7,4,6,1,2,
2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,2,2,
1,0,3,0,1,1,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,
2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,
4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,
7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,
1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,1,3,4,4,5,2,2,2,4,4,4,1,3,1,1,3,1,2,1,
3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,
3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,4,4,1,1,1,1,
4,3,4,3,3,2,2,6,7,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static
short Cyc_yydefact[1008]={0,18,50,51,52,53,55,70,73,74,75,76,77,78,79,80,93,94,95,
110,111,46,0,0,56,0,0,143,0,90,0,0,0,0,0,0,0,0,37,490,203,492,491,493,0,0,71,0,
186,186,185,1,0,16,0,0,17,0,0,40,48,42,68,44,81,82,0,83,191,36,155,0,180,188,84,
159,108,54,53,47,0,97,489,0,490,486,487,488,0,363,0,0,0,0,226,0,365,366,25,27,0,
0,0,0,144,0,0,0,0,145,0,0,0,0,0,0,0,183,184,2,0,0,0,0,0,29,0,116,117,119,41,49,
43,45,112,494,495,108,193,0,192,36,38,36,0,19,0,205,0,168,156,181,190,189,194,0,
69,0,54,101,0,99,0,490,372,0,0,0,0,0,0,0,0,0,0,0,108,0,0,0,0,476,0,0,0,0,0,463,
461,462,0,389,391,405,413,415,417,419,421,423,426,431,434,437,441,0,443,464,475,
474,0,373,371,32,0,0,108,0,0,0,126,122,124,244,246,0,0,9,10,0,0,0,108,496,497,
204,92,0,0,160,72,224,0,221,0,3,0,5,0,274,0,30,0,0,36,20,0,113,114,0,107,108,0,0,
0,0,0,0,0,0,0,0,0,0,490,277,279,0,285,281,0,283,267,268,269,0,270,271,272,0,39,
21,119,251,0,211,227,0,0,207,205,0,188,0,196,57,195,0,0,102,98,0,0,367,0,0,0,451,
0,407,441,0,408,409,0,0,0,0,0,0,0,0,0,444,445,36,0,0,0,447,448,446,0,364,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,395,396,397,398,399,400,401,402,403,404,394,0,
449,0,470,471,0,0,0,478,0,108,379,380,0,377,0,228,0,0,0,0,247,219,0,127,132,128,
130,123,125,205,0,253,245,254,499,498,0,86,89,0,87,0,91,0,63,62,0,60,161,205,223,
157,253,225,169,170,0,85,187,26,0,28,0,0,0,0,118,120,230,229,22,104,115,0,0,0,
134,135,137,0,108,0,141,0,0,0,0,0,0,36,0,314,315,316,0,0,318,0,0,0,0,286,282,119,
284,280,278,0,167,212,0,0,0,218,206,213,164,0,0,0,207,166,162,188,197,58,109,0,
103,412,100,96,375,376,0,0,0,231,0,0,235,0,0,0,240,0,0,0,0,0,0,0,0,0,0,0,477,484,
0,483,390,414,0,416,418,420,422,424,425,429,430,427,428,432,433,435,436,438,439,
440,393,392,469,466,0,468,0,0,0,368,0,374,31,386,0,382,387,0,384,369,0,0,248,133,
129,131,0,207,0,188,0,255,0,205,0,266,250,0,0,108,0,0,0,126,0,108,0,205,0,179,
158,222,4,6,0,34,0,0,138,121,0,0,207,108,150,0,0,0,293,0,0,0,0,0,313,317,0,0,0,0,
276,36,23,252,205,0,215,0,0,208,0,165,211,199,163,197,182,0,7,0,450,0,0,243,0,
232,0,236,239,241,455,0,0,0,0,0,459,460,0,0,0,442,480,0,0,467,465,0,0,378,381,
383,370,0,249,220,262,0,256,257,188,0,0,207,188,0,88,0,207,0,0,0,59,61,0,171,0,0,
207,0,188,0,0,0,136,139,112,142,152,151,0,0,146,0,0,0,0,0,0,0,0,0,0,0,0,0,293,
319,0,0,24,207,0,216,214,0,207,0,198,0,8,0,0,0,242,233,237,0,0,0,0,410,411,454,
482,0,485,406,479,481,0,385,388,261,259,265,264,0,258,188,0,112,0,66,64,65,172,
178,175,0,177,173,188,0,33,0,140,153,150,150,287,293,0,0,0,0,0,0,321,322,324,326,
328,330,332,334,337,342,345,348,352,354,361,362,0,36,290,299,0,0,0,0,0,0,0,0,0,0,
320,273,201,217,209,200,205,13,0,0,453,452,0,238,456,0,0,472,0,263,260,35,0,0,
176,174,275,106,0,149,148,0,0,293,0,358,0,0,355,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,356,294,0,301,0,0,0,309,0,0,0,0,0,0,292,0,207,12,14,0,0,0,0,473,105,0,
0,288,289,0,0,0,0,293,295,325,0,327,329,331,333,335,336,340,341,338,339,343,344,
346,347,349,350,351,0,300,302,303,0,311,310,0,305,0,0,0,210,202,15,0,0,0,0,154,
291,357,0,353,296,0,36,304,312,306,307,0,234,457,0,67,0,0,323,293,297,308,0,360,
359,298,458,0,0,0};static short Cyc_yydefgoto[142]={1005,51,655,852,52,53,278,54,
436,55,438,56,57,143,58,59,500,220,422,423,221,62,235,222,64,159,160,65,156,66,
256,257,126,127,128,258,223,403,451,452,453,67,68,726,727,728,69,454,70,427,71,
72,73,117,154,139,305,652,582,74,583,494,645,486,490,491,398,300,243,93,94,516,
444,517,518,519,520,224,293,294,584,409,281,440,282,283,284,285,286,734,287,288,
813,814,815,816,817,818,819,820,821,822,823,824,825,826,827,828,387,388,389,572,
573,574,289,191,375,192,506,193,194,195,196,197,198,199,200,201,202,203,204,205,
206,207,534,535,208,209,76,853,236,413};static short Cyc_yypact[1008]={2616,-
-32768,- -32768,- -32768,- -32768,7,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1159,145,
1117,- -32768,- 15,- 53,- -32768,24,36,59,84,162,113,166,174,303,239,- -32768,- -32768,
208,- -32768,- -32768,- -32768,203,598,234,236,274,274,- -32768,- -32768,2487,- -32768,
133,490,- -32768,358,496,1159,1159,1159,- -32768,1159,- -32768,- -32768,687,- -32768,
73,3118,267,201,254,548,- -32768,- -32768,297,307,325,- -32768,- 15,336,- -32768,3491,
154,- -32768,- -32768,- -32768,5449,- -32768,345,353,3491,364,372,373,- -32768,74,-
-32768,- -32768,3260,3260,2487,2487,- -32768,3260,392,5449,260,- -32768,170,386,410,
387,170,3464,5449,- -32768,- -32768,- -32768,2487,2744,2487,2744,- 62,- -32768,405,
427,- -32768,3044,- -32768,- -32768,- -32768,- -32768,3464,- -32768,- -32768,297,-
-32768,- 15,- -32768,1555,- -32768,3118,496,- -32768,3260,3192,3955,- -32768,267,-
-32768,- -32768,- -32768,82,3260,- -32768,2744,- -32768,451,458,460,- 15,- 39,- -32768,
3491,100,5626,455,5449,5493,461,466,474,528,535,297,559,563,5712,5712,- -32768,
2090,577,5756,5756,5756,- -32768,- -32768,- -32768,- 68,- -32768,- -32768,- 72,611,571,
572,614,543,159,558,224,169,- -32768,1028,5756,104,- 55,- -32768,641,855,654,-
-32768,- -32768,658,5449,297,855,651,126,3464,3532,3464,593,- -32768,72,72,- -32768,
- -32768,61,662,53,297,- -32768,- -32768,- -32768,- -32768,32,656,- -32768,- -32768,
1030,312,- -32768,680,- -32768,692,- -32768,697,719,726,- -32768,410,4045,3118,-
-32768,727,3464,- -32768,922,729,210,725,172,730,3595,731,744,739,741,4135,3595,
225,736,737,- -32768,- -32768,738,1690,1690,496,1690,- -32768,- -32768,- -32768,754,-
-32768,- -32768,- -32768,285,- -32768,- -32768,753,740,1,770,- -32768,107,756,750,318,
758,548,748,- -32768,769,- -32768,1,767,- 15,- -32768,5449,768,- -32768,765,772,2090,
- -32768,3464,- -32768,- -32768,3685,- -32768,797,5449,5449,5449,5449,5449,773,5449,
3464,2090,- -32768,- -32768,1825,766,368,5449,- -32768,- -32768,- -32768,5449,- -32768,
5756,5449,5756,5756,5756,5756,5756,5756,5756,5756,5756,5756,5756,5756,5756,5756,
5756,5756,5756,5449,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,5449,- -32768,170,- -32768,- -32768,4222,170,5449,-
-32768,778,297,- -32768,- -32768,779,783,3491,- -32768,333,5833,781,3260,- -32768,
787,789,- -32768,3532,3532,3532,- -32768,- -32768,2894,4309,232,- -32768,279,- -32768,
- -32768,107,- -32768,- -32768,3260,- -32768,785,- -32768,792,- -32768,788,786,793,-
-32768,2818,- -32768,294,365,- -32768,- -32768,- -32768,3464,- -32768,- -32768,- -32768,
2487,- -32768,2487,807,799,800,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,811,5449,803,809,- -32768,802,315,297,- 15,815,5449,5449,810,819,5449,904,
1960,825,- -32768,- -32768,- -32768,403,901,- -32768,4399,5449,3595,2220,- -32768,-
-32768,3044,- -32768,- -32768,- -32768,3260,- -32768,- -32768,3464,827,238,- -32768,-
-32768,818,- -32768,107,829,3396,750,- -32768,- -32768,548,649,- -32768,- -32768,2358,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,830,836,850,- -32768,170,5449,-
-32768,650,4045,191,- -32768,376,841,844,847,382,3464,388,845,848,858,5582,-
-32768,- -32768,849,860,- -32768,611,- 8,571,572,614,543,159,159,558,558,558,558,
224,224,169,169,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,866,-
-32768,- 67,3260,3865,- -32768,3946,- -32768,- -32768,- -32768,3491,- -32768,- -32768,
873,865,- -32768,- 10,882,- -32768,- -32768,- -32768,- -32768,872,750,874,548,867,279,
3260,3328,4486,- -32768,- -32768,72,879,297,626,880,32,2970,883,297,3260,3192,4576,
- -32768,294,- -32768,- -32768,- -32768,885,- -32768,900,1163,- -32768,- -32768,922,
5449,750,297,734,416,425,5449,749,426,887,4666,4753,415,- -32768,- -32768,894,896,
889,462,- -32768,3118,- -32768,740,899,3260,- -32768,898,107,- -32768,897,- -32768,
219,- -32768,- -32768,649,- -32768,903,- -32768,2487,905,291,902,- -32768,888,- -32768,
1267,- -32768,- -32768,- -32768,- -32768,5449,991,5449,5493,907,- -32768,- -32768,905,
908,3865,- -32768,- -32768,5449,5449,- -32768,- -32768,1,660,- -32768,- -32768,- -32768,
- -32768,5884,- -32768,- -32768,- -32768,909,- -32768,- -32768,548,1,916,750,548,895,-
-32768,5449,750,469,917,919,- -32768,- -32768,920,- -32768,1,923,750,926,548,928,
5449,910,- -32768,- -32768,3464,- -32768,- -32768,- -32768,934,- 15,445,3595,929,932,
957,933,943,3595,5449,4840,515,4927,599,5014,749,- -32768,946,949,- -32768,750,85,
- -32768,- -32768,958,750,3464,- -32768,344,- -32768,947,950,5449,- -32768,- -32768,-
-32768,4045,477,952,960,- -32768,797,- -32768,- -32768,668,- -32768,- -32768,- -32768,
- -32768,3865,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,953,- -32768,548,630,
3464,969,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,961,- -32768,- -32768,548,
489,- -32768,968,- -32768,967,86,734,1059,749,979,5842,974,2220,5756,973,- -32768,-
32,- -32768,1011,971,653,706,229,717,237,213,- -32768,- -32768,- -32768,- -32768,1012,
5756,1825,- -32768,- -32768,511,3595,555,5101,3595,564,5188,5275,634,986,- -32768,-
-32768,- -32768,- -32768,988,- -32768,899,- -32768,990,643,- -32768,- -32768,37,-
-32768,- -32768,3464,1079,- -32768,3775,- -32768,- -32768,- -32768,992,994,- -32768,-
-32768,- -32768,- -32768,3464,- -32768,- -32768,3595,999,749,2090,- -32768,3464,995,-
-32768,1420,5756,5449,5756,5756,5756,5756,5756,5756,5756,5756,5756,5756,5756,
5756,5756,5756,5756,5756,5756,5449,- -32768,- -32768,1004,- -32768,3595,3595,566,-
-32768,3595,3595,573,3595,574,5362,- -32768,107,750,- -32768,- -32768,260,5449,1008,
1010,- -32768,- -32768,1031,590,- -32768,- -32768,1017,1013,1020,5756,749,- -32768,
611,111,571,572,572,543,159,159,558,558,558,558,224,224,169,169,- -32768,- -32768,
- -32768,144,- -32768,- -32768,- -32768,3595,- -32768,- -32768,3595,- -32768,3595,3595,
623,- -32768,- -32768,- -32768,694,1022,3464,1023,- -32768,- -32768,905,354,- -32768,-
-32768,5449,1420,- -32768,- -32768,- -32768,- -32768,3595,- -32768,- -32768,1025,-
-32768,1027,1032,- -32768,749,- -32768,- -32768,1034,- -32768,- -32768,- -32768,-
-32768,1136,1147,- -32768};static short Cyc_yypgoto[142]={- -32768,128,- -32768,226,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 44,- -32768,- 107,20,-
-32768,- -32768,0,552,- -32768,95,- 193,1046,5,- -32768,- -32768,- 132,- -32768,457,
1125,- 682,- -32768,- -32768,- -32768,930,927,- 17,366,- -32768,- -32768,569,- -32768,-
-32768,33,- -32768,- -32768,79,- 153,1116,- 289,66,- -32768,- -32768,1139,62,- -32768,-
-32768,538,- 139,- 54,- 127,- 56,271,544,547,- 409,- 465,- 111,- 382,- 135,- -32768,- 251,-
161,- 531,- 362,- -32768,674,- 147,- 63,- 121,- 170,- 317,57,- -32768,- -32768,- -32768,- 42,
- 262,- -32768,- 7,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
164,977,- -32768,506,- -32768,- -32768,370,536,- -32768,- 137,- 421,- 146,- 334,- 333,-
341,851,- 331,- 275,- 91,- 225,- 223,132,465,468,- -32768,- -32768,- 313,- -32768,177,497,
- 65,- 30,- 370,68};static short Cyc_yytable[6017]={60,136,443,592,242,63,540,558,298,
322,537,561,301,539,140,344,477,478,542,480,299,60,254,643,323,142,63,145,401,
614,312,570,319,684,307,336,137,225,226,249,801,79,229,136,383,112,342,342,99,
420,607,343,60,408,250,884,345,63,682,60,60,60,165,60,63,63,63,560,63,60,149,429,
384,530,63,543,544,166,232,130,131,132,410,133,646,142,39,255,426,144,587,724,
725,42,660,61,885,279,241,290,306,291,60,60,640,866,342,63,63,690,77,587,691,607,
680,483,61,259,411,412,60,60,60,60,113,63,63,63,63,60,241,549,550,421,63,551,552,
129,151,606,100,60,410,60,659,771,63,61,63,144,149,342,101,410,61,61,61,60,61,
924,634,280,63,144,61,397,606,342,487,510,102,511,418,496,505,415,431,504,411,40,
119,377,378,379,528,529,216,95,40,411,412,238,217,138,431,103,721,61,61,- 147,96,
412,304,399,847,404,314,401,401,401,290,104,446,40,412,61,61,61,61,488,315,400,
380,292,61,342,402,381,382,105,227,228,40,983,279,279,61,279,61,396,259,352,353,
489,120,121,39,164,245,246,247,248,61,42,60,165,212,80,342,63,96,545,546,547,548,
- 227,984,663,- 227,96,166,233,600,39,144,354,355,234,60,60,42,60,106,63,63,308,63,
758,109,407,460,279,107,461,414,360,600,416,280,280,763,280,39,361,362,501,664,
428,110,42,731,892,893,505,514,515,339,340,341,45,485,111,464,19,20,455,155,570,
471,313,297,292,576,457,114,60,472,376,- 205,900,63,- 205,96,473,40,894,895,901,
902,61,405,642,593,358,115,280,406,359,489,479,601,39,484,431,498,242,898,41,42,
43,899,850,637,61,61,502,61,44,505,671,48,49,50,146,431,147,116,96,241,44,431,
148,233,757,96,588,482,589,234,342,400,400,400,590,44,402,402,402,241,591,602,
762,603,155,763,241,39,124,604,157,639,628,41,42,43,599,432,605,158,297,61,433,
495,449,397,142,60,638,60,489,649,63,683,63,568,39,710,342,162,613,698,41,42,43,
211,233,994,851,190,241,710,234,699,210,713,60,39,697,715,241,63,407,41,42,43,
213,714,231,241,60,505,712,342,425,63,144,214,532,215,406,342,428,553,554,555,
666,342,75,230,144,763,670,342,60,237,651,239,672,63,767,646,39,857,747,631,455,
251,342,42,81,97,752,98,397,768,762,741,693,45,342,342,61,636,61,601,729,48,49,
50,342,342,252,75,773,730,735,942,943,431,75,939,431,337,941,567,75,804,571,944,
805,61,650,722,135,608,309,609,96,75,905,96,310,241,318,61,311,342,161,748,324,
97,745,399,788,325,392,241,591,789,97,489,342,326,290,260,746,858,39,61,75,75,
122,123,342,42,135,605,125,870,75,995,762,152,153,45,945,946,75,75,75,75,937,48,
49,50,342,837,350,351,342,906,654,317,329,320,320,261,60,356,357,470,75,63,783,
333,334,327,695,787,320,320,320,244,328,75,60,651,144,795,161,63,702,97,677,881,
455,16,17,18,342,320,951,952,393,908,953,954,330,342,455,342,331,455,912,303,962,
337,342,342,419,136,846,965,967,521,338,849,346,525,39,527,347,337,348,432,259,
42,97,44,977,840,405,925,342,97,538,45,406,458,48,49,50,997,920,48,49,50,705,706,
707,686,934,61,935,687,843,241,342,430,724,725,865,989,96,342,917,349,96,342,75,
61,774,562,385,922,390,430,923,780,391,462,930,784,661,662,781,579,580,581,395,
259,732,733,775,776,424,320,75,797,417,793,861,862,756,888,889,806,279,39,445,
890,891,833,434,41,42,43,896,897,134,876,947,948,949,950,435,161,990,342,320,437,
320,320,320,320,320,320,320,320,320,320,320,320,320,320,320,320,320,992,621,622,
60,441,625,439,629,63,873,874,279,447,456,564,459,635,466,996,864,463,465,467,
280,468,571,474,241,476,445,136,475,869,522,523,524,971,489,96,481,253,412,933,
485,493,499,533,492,938,497,536,44,503,507,508,60,344,531,594,97,63,509,97,526,
907,82,563,911,829,566,565,556,575,577,578,280,595,597,596,598,610,96,611,557,39,
618,615,320,533,612,41,42,43,44,430,616,617,430,61,620,623,449,982,624,931,75,
626,75,630,48,49,50,279,83,586,632,882,641,430,644,647,656,657,658,430,386,619,
667,163,85,668,86,87,669,904,42,673,960,961,674,678,963,964,675,966,88,679,89,90,
998,61,689,320,91,809,810,60,681,688,92,40,63,1003,692,696,694,703,82,320,738,
740,708,75,719,711,718,280,736,742,743,744,533,297,749,533,760,751,755,765,676,
759,985,785,800,986,39,987,988,769,770,779,41,42,43,955,956,957,782,790,764,791,
792,449,807,794,83,450,796,999,802,48,49,50,808,704,798,445,831,832,844,163,85,
845,86,87,97,848,42,854,97,981,855,859,867,863,786,860,811,723,89,90,61,868,871,
320,91,363,872,812,875,799,92,187,877,880,188,189,883,430,886,903,918,445,887,
919,921,926,928,834,836,929,839,430,842,932,430,936,959,364,365,366,367,368,369,
370,371,372,373,701,974,975,856,978,39,979,976,980,320,1006,41,42,43,717,991,993,
374,1000,320,1001,1007,425,972,709,1002,75,1004,406,82,48,49,50,533,240,108,2,3,
4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,190,442,21,448,720,97,150,118,754,
970,750,753,665,394,778,22,0,445,541,830,0,0,83,24,766,0,910,27,0,914,916,445,0,
0,772,28,29,84,85,0,86,87,0,803,42,0,32,33,97,0,0,35,36,37,88,0,89,90,0,0,0,0,91,
0,0,0,0,337,92,0,0,135,0,0,940,0,0,0,0,0,0,0,0,39,40,41,0,43,44,41,42,43,958,879,
0,0,320,0,46,47,449,0,0,0,0,0,969,0,48,49,50,167,168,973,320,0,0,0,445,0,0,0,0,0,
82,0,169,0,0,0,170,445,0,0,0,0,0,0,171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,0,0,
0,0,177,178,0,0,0,0,0,179,180,0,0,320,0,320,320,320,320,320,320,320,320,320,320,
320,320,320,320,320,320,320,39,85,181,86,87,0,0,42,0,0,0,321,761,0,0,0,0,182,0,0,
183,0,514,515,0,184,0,0,185,0,445,186,187,320,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,135,0,0,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,732,733,21,167,
168,262,320,263,264,265,266,267,268,269,270,22,82,23,169,271,0,0,170,24,272,0,0,
27,0,0,171,172,173,174,0,28,29,0,175,0,273,176,0,0,0,0,32,33,34,177,178,35,36,37,
38,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,274,85,181,86,87,40,41,42,
43,44,275,141,- 293,0,0,0,0,182,0,46,277,0,0,0,0,184,0,0,185,0,0,186,187,0,0,188,
189,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,167,168,262,0,263,
264,265,266,267,268,269,270,22,82,23,169,271,0,0,170,24,272,0,0,27,0,0,171,172,
173,174,0,28,29,0,175,0,273,176,0,0,0,0,32,33,34,177,178,35,36,37,38,0,179,180,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,274,85,181,86,87,40,41,42,43,44,275,141,
276,0,0,0,0,182,0,46,277,0,0,0,0,184,0,0,185,0,0,186,187,0,0,188,189,2,3,4,78,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,167,168,262,0,263,264,265,266,267,
268,269,270,22,82,23,169,271,0,0,170,24,272,0,0,27,0,0,171,172,173,174,0,28,29,-
36,175,0,273,176,0,0,0,0,32,33,34,177,178,35,36,37,38,0,179,180,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,274,85,181,86,87,40,41,42,43,44,275,141,0,0,0,0,0,182,
0,46,277,0,0,0,0,184,0,0,185,0,0,186,187,0,0,188,189,2,3,4,78,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,21,167,168,262,0,263,264,265,266,267,268,269,270,22,
82,23,169,271,0,0,170,24,272,0,0,27,0,0,171,172,173,174,0,28,29,0,175,0,273,176,
0,0,0,0,32,33,34,177,178,35,36,37,38,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,274,85,181,86,87,40,41,42,43,44,275,141,0,0,0,0,0,182,0,46,277,0,0,0,0,
184,0,0,185,0,0,186,187,0,0,188,189,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,21,167,168,0,0,0,0,0,0,0,0,0,0,22,82,23,169,0,0,0,170,24,0,0,0,27,0,0,
171,172,173,174,0,28,29,0,175,0,0,176,0,0,0,0,32,33,34,177,178,35,36,37,38,0,179,
180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,40,41,42,43,44,627,
0,0,0,0,0,0,182,0,46,277,0,0,0,0,184,0,0,185,0,0,186,187,0,0,188,189,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,0,167,168,0,0,0,0,0,0,0,0,0,0,22,82,0,169,0,0,0,
170,0,0,0,0,27,0,0,171,172,173,174,0,28,29,0,175,0,0,176,0,0,0,0,32,33,0,177,178,
35,36,37,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,
40,41,42,43,44,0,335,0,0,0,0,0,182,0,46,277,0,0,0,0,184,0,0,185,0,0,186,187,0,0,
188,189,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,167,168,0,0,0,0,0,0,0,0,0,0,
22,82,0,169,0,0,0,170,0,0,0,0,27,0,0,171,172,173,174,0,28,29,0,175,0,0,176,0,0,0,
0,32,33,0,177,178,35,36,37,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
39,85,181,86,87,40,41,42,43,44,0,0,0,0,0,0,0,182,0,46,277,0,0,0,0,184,0,0,185,0,
0,186,187,0,0,188,189,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,653,0,24,0,25,26,27,0,0,0,0,0,0,0,28,
29,- 36,0,0,0,0,0,0,30,31,32,33,34,0,0,35,36,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,39,0,0,0,0,40,41,42,43,44,0,0,- 11,0,0,0,0,45,0,46,47,0,0,0,0,
48,49,50,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,
0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,
0,0,30,31,32,33,34,0,0,35,36,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,39,0,0,0,0,40,41,42,43,44,0,0,- 11,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,- 11,1,0,
2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,
22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,30,31,32,33,
34,0,0,35,36,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,40,
41,42,43,44,0,0,0,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,1,0,2,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,
25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,30,31,32,33,34,0,0,35,36,37,38,0,0,
0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,39,0,0,0,0,40,41,42,43,44,
22,0,- 11,0,0,0,0,45,0,46,47,0,27,0,0,48,49,50,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,
0,0,0,35,36,37,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,295,0,0,39,
0,0,0,0,40,41,42,43,44,297,0,22,0,0,0,0,425,0,46,47,0,0,406,27,48,49,50,0,0,0,0,
28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,36,37,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,295,0,0,0,0,0,0,0,40,41,0,43,44,297,0,22,0,0,0,0,405,0,46,
47,0,0,406,27,48,49,50,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,36,37,0,2,
3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,39,0,0,0,0,40,41,42,
43,44,22,0,23,0,0,0,0,449,24,46,47,0,27,0,0,48,49,50,0,0,28,29,- 36,0,0,0,0,0,0,0,
0,32,33,34,0,0,35,36,37,38,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
21,0,0,0,0,0,0,0,40,41,0,43,44,22,141,23,0,253,0,0,0,24,46,47,0,27,0,0,0,0,0,0,0,
28,29,0,0,0,0,0,0,0,0,0,32,33,34,0,0,35,36,37,38,0,0,0,0,0,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,141,0,0,0,0,0,0,0,46,47,0,
27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,36,37,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,295,0,0,296,0,0,0,22,40,41,0,43,44,297,0,0,
0,0,0,27,0,0,46,47,0,0,0,28,29,0,0,218,0,0,0,0,0,0,32,33,0,0,0,35,36,37,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,40,41,0,43,44,0,
219,0,0,0,0,27,0,0,46,47,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,36,37,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,295,0,0,0,0,0,0,22,40,41,0,43,
44,297,0,0,0,0,0,27,0,0,46,47,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,35,36,37,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,648,0,0,0,0,0,0,22,40,41,
0,43,44,0,0,0,0,0,0,27,0,0,46,47,0,0,0,28,29,0,0,0,0,0,0,82,0,0,32,33,0,0,0,35,
36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,40,
41,0,43,44,0,0,83,0,0,0,27,0,0,46,47,0,0,0,28,29,0,163,85,0,86,87,0,0,42,32,33,0,
0,0,35,36,37,0,88,0,89,90,0,0,0,0,91,0,0,167,168,262,92,263,264,265,266,267,268,
269,270,0,82,0,169,271,0,40,170,0,272,44,0,0,0,0,171,172,173,174,0,46,47,0,175,0,
273,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,274,85,181,86,87,0,0,42,0,0,275,141,0,0,167,168,0,182,0,0,183,512,0,0,
0,184,0,82,185,169,0,186,187,170,0,188,189,0,0,0,0,171,172,173,174,0,0,0,0,175,0,
0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,39,85,181,86,87,0,0,42,0,0,0,321,513,0,167,168,0,182,0,0,183,0,514,515,0,
184,0,82,185,169,0,186,187,170,0,188,189,0,0,0,0,171,172,173,174,0,0,0,0,175,0,0,
176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,39,85,181,86,87,0,0,42,0,0,0,321,927,0,167,168,0,182,0,0,183,0,514,515,0,
184,0,82,185,169,0,186,187,170,0,188,189,0,0,0,0,171,172,173,174,0,0,0,0,175,0,0,
176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,39,85,181,86,87,0,0,42,0,0,0,321,0,0,167,168,0,182,82,0,183,0,514,515,0,
184,0,82,185,169,0,186,187,170,0,188,189,0,0,0,0,171,172,173,174,0,0,0,0,175,0,0,
176,0,0,0,0,0,0,0,177,178,0,0,0,0,83,179,180,0,0,0,0,0,0,0,0,0,685,0,0,163,85,0,
86,87,0,0,42,0,39,85,181,86,87,0,0,42,88,0,89,90,0,0,167,168,91,182,0,0,183,0,92,
0,302,184,0,82,185,169,0,186,187,170,0,188,189,0,0,0,0,171,172,173,174,0,0,0,0,
175,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,0,321,0,0,167,168,0,182,0,0,183,0,0,0,0,
184,0,82,185,169,0,186,187,170,0,188,189,0,0,0,0,171,172,173,174,0,0,0,0,175,0,0,
176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,39,85,181,86,87,0,0,42,0,0,469,167,168,0,0,0,0,182,0,0,183,0,0,0,82,184,
169,0,185,0,170,186,187,0,0,188,189,0,171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,
0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,
85,181,86,87,0,0,42,0,0,0,167,168,0,0,0,0,182,559,0,183,0,0,0,82,184,169,0,185,0,
170,186,187,0,0,188,189,0,171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,
178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,
0,0,42,0,0,0,0,0,0,167,168,0,182,0,0,183,0,0,0,585,184,0,82,185,169,0,186,187,
170,0,188,189,0,0,0,0,171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,178,
0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,0,0,
42,0,0,0,167,168,0,0,0,0,182,633,0,183,0,0,0,82,184,169,0,185,0,170,186,187,0,0,
188,189,0,171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,
179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,0,0,
0,0,167,168,0,182,0,0,183,0,0,0,700,184,0,82,185,169,0,186,187,170,0,188,189,0,0,
0,0,171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,0,0,0,0,167,
168,0,182,0,0,183,0,0,0,716,184,0,82,185,169,0,186,187,170,0,188,189,0,0,0,0,171,
172,173,174,0,0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,737,167,168,0,0,0,0,
182,0,0,183,0,0,0,82,184,169,0,185,0,170,186,187,0,0,188,189,0,171,172,173,174,0,
0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,739,167,168,0,0,0,0,182,0,0,183,0,
0,0,82,184,169,0,185,0,170,186,187,0,0,188,189,0,171,172,173,174,0,0,0,0,175,0,0,
176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,39,85,181,86,87,0,0,42,0,0,0,167,168,0,0,0,0,182,835,0,183,0,0,0,82,184,
169,0,185,0,170,186,187,0,0,188,189,0,171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,
0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,
85,181,86,87,0,0,42,0,0,0,167,168,0,0,0,0,182,838,0,183,0,0,0,82,184,169,0,185,0,
170,186,187,0,0,188,189,0,171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,
178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,
0,0,42,0,0,841,167,168,0,0,0,0,182,0,0,183,0,0,0,82,184,169,0,185,0,170,186,187,
0,0,188,189,0,171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,
0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,0,
167,168,0,0,0,0,182,909,0,183,0,0,0,82,184,169,0,185,0,170,186,187,0,0,188,189,0,
171,172,173,174,0,0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,0,167,168,0,0,0,
0,182,913,0,183,0,0,0,82,184,169,0,185,0,170,186,187,0,0,188,189,0,171,172,173,
174,0,0,0,0,175,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,0,167,168,0,0,0,0,182,915,0,
183,0,0,0,82,184,169,0,185,0,170,186,187,0,0,188,189,0,171,172,173,174,0,0,0,0,
175,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,0,179,180,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,0,167,168,0,0,0,0,182,968,0,183,0,0,0,
82,184,169,0,185,0,170,186,187,0,0,188,189,0,171,172,173,174,0,0,0,0,175,0,0,176,
0,0,0,0,0,167,168,177,178,0,0,0,0,0,179,180,0,0,82,0,0,0,0,0,0,0,0,0,0,0,0,0,171,
172,173,174,0,39,85,181,86,87,0,176,42,0,0,0,0,0,0,177,178,0,182,0,0,183,179,180,
0,0,184,0,0,185,0,0,186,187,0,0,188,189,0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,
0,321,0,167,168,0,0,182,0,0,183,0,0,0,0,184,82,0,185,0,0,186,187,0,0,188,189,0,0,
0,171,172,173,174,0,0,0,0,0,0,0,176,0,0,0,0,0,167,168,177,178,0,0,0,0,0,179,180,
0,0,82,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,173,174,0,39,85,181,86,87,0,176,42,0,0,
0,676,0,0,177,178,0,182,0,0,183,179,180,0,0,184,0,0,185,0,0,186,187,0,0,188,189,
0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,167,168,0,0,0,0,0,316,0,0,183,0,0,82,0,
184,0,0,185,0,0,186,187,0,0,188,189,171,172,173,174,0,0,0,0,0,0,0,176,0,0,0,0,0,
167,168,177,178,0,0,0,0,0,179,180,0,0,82,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,173,
174,0,39,85,181,86,87,0,176,42,0,0,0,0,0,0,177,178,0,332,0,0,183,179,180,0,0,184,
0,0,185,0,0,186,187,0,0,188,189,0,0,0,0,0,0,0,39,85,181,86,87,0,0,42,0,0,167,168,
0,0,82,0,0,182,0,0,183,0,0,82,0,184,0,0,185,0,0,186,187,0,0,188,189,171,172,173,
174,0,0,0,0,0,0,0,176,0,0,0,0,0,0,0,177,178,0,0,0,0,83,179,180,82,0,0,0,0,0,0,0,
0,569,0,0,163,85,0,86,87,0,0,42,0,39,85,181,86,87,0,0,42,88,0,89,90,0,514,515,0,
91,878,0,0,183,0,92,0,0,184,0,83,185,0,0,186,187,0,0,188,189,0,0,777,0,0,163,85,
0,86,87,0,0,42,0,0,0,0,0,0,0,0,0,88,0,89,90,0,514,515,0,91,0,0,0,0,0,92};static
short Cyc_yycheck[6017]={0,66,253,412,115,0,347,377,147,170,344,381,147,346,68,87,
278,279,349,281,147,21,129,488,170,69,21,69,221,450,162,393,169,564,155,182,66,
100,101,101,722,21,105,108,99,45,114,114,101,17,432,119,52,223,116,87,128,52,125,
59,60,61,101,63,59,60,61,380,63,69,70,241,127,335,69,350,351,116,108,59,60,61,81,
63,493,129,101,129,241,69,407,5,6,108,515,0,128,141,115,143,154,143,102,103,486,
787,114,102,103,119,103,428,577,495,122,114,21,134,117,129,120,121,122,123,45,
120,121,122,123,129,147,356,357,101,129,358,359,58,72,428,116,141,81,143,514,676,
141,52,143,129,150,114,116,81,59,60,61,157,63,122,473,141,157,143,69,219,455,114,
61,316,111,318,119,300,311,114,241,309,117,106,52,77,78,79,331,332,112,23,106,
117,129,112,118,120,259,111,617,102,103,113,23,129,120,220,119,222,106,400,401,
402,254,49,254,106,129,120,121,122,123,112,120,221,118,144,129,114,221,123,124,
116,102,103,106,122,278,279,141,281,143,113,257,82,83,297,111,112,101,83,120,121,
122,123,157,108,254,101,92,112,114,254,83,352,353,354,355,111,122,518,114,92,116,
101,425,101,254,116,117,107,278,279,108,281,116,278,279,157,281,657,49,223,118,
335,118,121,226,126,449,229,278,279,662,281,101,134,135,305,115,241,100,108,623,
82,83,450,123,124,184,185,186,118,101,118,265,20,21,259,116,689,271,165,111,252,
395,123,100,335,111,205,119,126,335,122,165,118,106,116,117,134,135,254,118,113,
415,129,118,335,124,133,412,280,425,101,294,428,302,476,129,107,108,109,133,753,
479,278,279,307,281,110,515,526,126,127,128,116,449,118,112,210,405,110,455,124,
101,102,217,116,111,118,107,114,400,401,402,124,110,400,401,402,425,409,116,662,
118,116,776,432,101,59,124,112,483,465,107,108,109,425,114,427,103,111,335,119,
114,118,488,479,436,479,438,493,496,436,563,438,111,101,599,114,112,449,589,107,
108,109,101,101,102,113,88,476,613,107,589,118,603,465,101,588,603,486,465,405,
107,108,109,111,603,107,495,479,617,602,114,118,479,465,114,119,115,124,114,425,
360,361,362,119,114,0,106,479,862,119,114,503,118,500,119,119,503,670,919,101,
763,640,111,449,111,114,108,22,23,648,25,577,670,776,111,583,118,114,114,436,475,
438,599,119,126,127,128,114,114,114,45,680,119,119,887,888,613,52,884,616,182,
886,390,58,111,393,889,114,465,499,618,66,436,114,438,390,71,831,393,113,589,118,
479,115,114,80,641,118,83,119,599,114,118,215,603,587,119,92,644,114,118,637,137,
637,119,101,503,102,103,111,112,114,108,108,606,111,119,112,980,862,64,65,118,
890,891,120,121,122,123,883,126,127,128,114,111,84,85,114,119,503,167,176,169,
170,139,637,80,81,270,144,637,699,179,180,118,585,704,184,185,186,116,118,157,
655,651,637,714,162,655,593,165,531,811,599,17,18,19,114,205,896,897,216,119,898,
899,118,114,613,114,118,616,119,148,119,316,114,114,232,755,747,119,119,324,118,
752,86,328,101,330,130,332,131,114,722,108,210,110,119,111,118,859,114,217,345,
118,124,261,126,127,128,984,850,126,127,128,101,102,103,566,878,637,880,570,742,
753,114,241,5,6,111,119,566,114,111,132,570,114,252,655,683,382,112,111,101,259,
114,696,101,263,872,700,113,114,697,400,401,402,118,787,22,23,113,114,119,311,
280,716,117,712,113,114,655,131,132,729,831,101,253,84,85,735,113,107,108,109,80,
81,112,807,892,893,894,895,113,309,113,114,344,113,346,347,348,349,350,351,352,
353,354,355,356,357,358,359,360,361,362,975,459,460,831,106,463,115,465,831,804,
805,883,113,112,385,118,474,101,983,785,118,118,111,831,111,689,118,872,118,321,
923,122,798,325,326,327,920,919,689,113,115,129,877,101,122,125,338,119,883,119,
342,110,113,113,117,883,87,119,101,390,883,117,393,118,835,38,116,838,732,114,
119,363,119,114,113,883,112,119,118,114,101,732,111,375,101,456,111,450,380,117,
107,108,109,110,425,114,122,428,831,112,118,118,937,112,875,436,30,438,111,126,
127,128,984,86,406,42,812,118,449,129,119,119,114,101,455,98,457,114,101,102,114,
104,105,114,830,108,119,908,909,119,119,912,913,113,915,118,114,120,121,984,883,
114,515,126,25,26,984,119,113,132,106,984,997,119,125,119,115,38,531,627,628,119,
503,101,119,118,984,118,112,111,119,473,111,113,476,125,117,112,25,112,116,962,
125,111,965,101,967,968,119,119,119,107,108,109,900,901,902,119,119,667,119,119,
118,112,119,86,122,119,989,113,126,127,128,119,595,125,518,122,113,111,101,102,
111,104,105,566,106,108,119,570,936,119,118,102,119,703,114,118,619,120,121,984,
119,113,617,126,56,118,129,28,718,132,133,112,118,136,137,122,599,86,86,113,564,
130,114,113,25,113,736,737,114,739,613,741,113,616,119,111,88,89,90,91,92,93,94,
95,96,97,590,119,118,759,113,101,119,102,114,670,0,107,108,109,604,119,119,115,
119,680,119,0,118,923,598,119,655,119,124,38,126,127,128,623,114,36,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,811,252,24,257,616,689,71,49,651,919,644,
648,519,217,689,37,- 1,662,348,732,- 1,- 1,86,45,669,- 1,837,49,- 1,840,841,676,- 1,- 1,
679,57,58,101,102,- 1,104,105,- 1,727,108,- 1,68,69,732,- 1,- 1,73,74,75,118,- 1,120,
121,- 1,- 1,- 1,- 1,126,- 1,- 1,- 1,- 1,878,132,- 1,- 1,755,- 1,- 1,885,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,101,106,107,- 1,109,110,107,108,109,903,809,- 1,- 1,812,- 1,120,121,118,- 1,- 1,- 1,
- 1,- 1,917,- 1,126,127,128,25,26,924,830,- 1,- 1,- 1,763,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,-
1,- 1,44,776,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,884,- 1,886,887,888,889,890,891,892,893,
894,895,896,897,898,899,900,901,902,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,112,
113,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,123,124,- 1,126,- 1,- 1,129,- 1,862,132,133,936,- 1,
136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,923,- 1,- 1,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,983,29,30,31,32,33,34,35,36,
37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,63,
- 1,- 1,- 1,- 1,68,69,70,71,72,73,74,75,76,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,106,107,108,109,110,111,112,
113,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,
137,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,
31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,
58,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,71,72,73,74,75,76,- 1,78,79,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,106,107,108,
109,110,111,112,113,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,
132,133,- 1,- 1,136,137,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,
25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,
52,53,54,55,- 1,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,71,72,73,74,75,76,- 1,78,
79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,
104,105,106,107,108,109,110,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,
126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,
45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,71,
72,73,74,75,76,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,101,102,103,104,105,106,107,108,109,110,111,112,- 1,- 1,- 1,- 1,- 1,118,- 1,120,
121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,3,4,5,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,
40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,63,- 1,- 1,- 1,
- 1,68,69,70,71,72,73,74,75,76,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,106,107,108,109,110,111,- 1,- 1,- 1,- 1,- 1,
- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,63,- 1,
- 1,- 1,- 1,68,69,- 1,71,72,73,74,75,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,106,107,108,109,110,- 1,112,- 1,- 1,
- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,
63,- 1,- 1,- 1,- 1,68,69,- 1,71,72,73,74,75,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,106,107,108,109,110,- 1,- 1,-
1,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,
137,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,70,- 1,- 1,73,74,75,76,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,106,107,
108,109,110,- 1,- 1,113,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,127,128,0,1,- 1,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,
59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,70,- 1,- 1,73,74,75,76,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,106,107,108,109,110,
- 1,- 1,113,- 1,- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,127,128,0,1,- 1,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,
- 1,- 1,- 1,66,67,68,69,70,- 1,- 1,73,74,75,76,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,127,128,1,- 1,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,
- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,
68,69,70,- 1,- 1,73,74,75,76,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,106,107,108,109,110,37,- 1,113,- 1,- 1,- 1,- 1,118,
- 1,120,121,- 1,49,- 1,- 1,126,127,128,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,
- 1,- 1,- 1,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,98,- 1,- 1,101,- 1,- 1,- 1,- 1,106,107,108,109,110,111,- 1,37,- 1,- 1,- 1,- 1,118,- 1,120,
121,- 1,- 1,124,49,126,127,128,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,
- 1,- 1,- 1,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,- 1,109,110,111,- 1,37,- 1,- 1,- 1,- 1,118,- 1,120,
121,- 1,- 1,124,49,126,127,128,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,
- 1,- 1,- 1,73,74,75,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,
- 1,- 1,101,- 1,- 1,- 1,- 1,106,107,108,109,110,37,- 1,39,- 1,- 1,- 1,- 1,118,45,120,121,- 1,
49,- 1,- 1,126,127,128,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,73,74,
75,76,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,106,107,- 1,109,110,37,112,39,- 1,115,- 1,- 1,- 1,45,120,121,- 1,49,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,73,74,75,76,- 1,- 1,- 1,- 1,
- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,
- 1,109,110,37,112,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,73,74,75,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,101,- 1,- 1,- 1,37,106,107,- 1,109,110,
111,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,120,121,- 1,- 1,- 1,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,68,
69,- 1,- 1,- 1,73,74,75,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,106,107,- 1,109,110,- 1,112,- 1,- 1,- 1,- 1,49,- 1,- 1,120,
121,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,73,74,75,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,37,
106,107,- 1,109,110,111,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,120,121,- 1,- 1,- 1,57,58,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,73,74,75,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,37,106,107,- 1,109,110,- 1,- 1,- 1,- 1,-
1,- 1,49,- 1,- 1,120,121,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,68,69,- 1,- 1,- 1,
73,74,75,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,37,106,107,- 1,109,110,- 1,- 1,86,- 1,- 1,- 1,49,- 1,- 1,120,121,- 1,- 1,- 1,
57,58,- 1,101,102,- 1,104,105,- 1,- 1,108,68,69,- 1,- 1,- 1,73,74,75,- 1,118,- 1,120,121,
- 1,- 1,- 1,- 1,126,- 1,- 1,25,26,27,132,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,106,
44,- 1,46,110,- 1,- 1,- 1,- 1,52,53,54,55,- 1,120,121,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,111,112,- 1,- 1,25,26,- 1,118,- 1,- 1,
121,32,- 1,- 1,- 1,126,- 1,38,129,40,- 1,132,133,44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,
- 1,108,- 1,- 1,- 1,112,113,- 1,25,26,- 1,118,- 1,- 1,121,- 1,123,124,- 1,126,- 1,38,129,40,
- 1,132,133,44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,-
1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,112,113,- 1,25,26,
- 1,118,- 1,- 1,121,- 1,123,124,- 1,126,- 1,38,129,40,- 1,132,133,44,- 1,136,137,- 1,- 1,-
1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,
- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,
103,104,105,- 1,- 1,108,- 1,- 1,- 1,112,- 1,- 1,25,26,- 1,118,38,- 1,121,- 1,123,124,- 1,
126,- 1,38,129,40,- 1,132,133,44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,86,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,98,- 1,- 1,101,102,- 1,104,105,- 1,- 1,108,- 1,101,102,103,104,105,- 1,- 1,108,118,- 1,
120,121,- 1,- 1,25,26,126,118,- 1,- 1,121,- 1,132,- 1,125,126,- 1,38,129,40,- 1,132,133,
44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,112,- 1,- 1,25,26,- 1,118,- 1,- 1,
121,- 1,- 1,- 1,- 1,126,- 1,38,129,40,- 1,132,133,44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,
- 1,108,- 1,- 1,111,25,26,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,
132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,-
1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,118,119,- 1,
121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,-
1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,
- 1,- 1,- 1,- 1,- 1,25,26,- 1,118,- 1,- 1,121,- 1,- 1,- 1,125,126,- 1,38,129,40,- 1,132,133,
44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,118,119,- 1,
121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,-
1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,
- 1,- 1,- 1,- 1,- 1,25,26,- 1,118,- 1,- 1,121,- 1,- 1,- 1,125,126,- 1,38,129,40,- 1,132,133,
44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,118,- 1,- 1,
121,- 1,- 1,- 1,125,126,- 1,38,129,40,- 1,132,133,44,- 1,136,137,- 1,- 1,- 1,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,
- 1,108,- 1,- 1,111,25,26,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,
132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,-
1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,111,25,26,- 1,- 1,- 1,- 1,118,- 1,- 1,
121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,-
1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,118,119,- 1,121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,-
1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,-
1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,118,119,- 1,121,- 1,- 1,
- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,
- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,111,25,
26,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,
137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,-
1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,
102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,118,119,- 1,121,- 1,- 1,- 1,38,
126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,
- 1,- 1,118,119,- 1,121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,
53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,
105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,118,119,- 1,121,- 1,- 1,- 1,38,126,40,- 1,
129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,
118,119,- 1,121,- 1,- 1,- 1,38,126,40,- 1,129,- 1,44,132,133,- 1,- 1,136,137,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,25,26,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,
38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,101,102,103,104,105,- 1,
63,108,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,118,- 1,- 1,121,78,79,- 1,- 1,126,- 1,- 1,129,- 1,- 1,
132,133,- 1,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,-
1,112,- 1,25,26,- 1,- 1,118,- 1,- 1,121,- 1,- 1,- 1,- 1,126,38,- 1,129,- 1,- 1,132,133,- 1,- 1,
136,137,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,25,26,71,72,
- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,
- 1,101,102,103,104,105,- 1,63,108,- 1,- 1,- 1,112,- 1,- 1,71,72,- 1,118,- 1,- 1,121,78,79,
- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,103,
104,105,- 1,- 1,108,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,118,- 1,- 1,121,- 1,- 1,38,- 1,126,- 1,- 1,
129,- 1,- 1,132,133,- 1,- 1,136,137,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,- 1,
- 1,25,26,71,72,- 1,- 1,- 1,- 1,- 1,78,79,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,52,53,54,55,- 1,101,102,103,104,105,- 1,63,108,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,118,-
1,- 1,121,78,79,- 1,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,101,102,103,104,105,- 1,- 1,108,- 1,- 1,25,26,- 1,- 1,38,- 1,- 1,118,- 1,- 1,121,- 1,- 1,
38,- 1,126,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,- 1,- 1,- 1,86,78,79,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,
- 1,- 1,101,102,- 1,104,105,- 1,- 1,108,- 1,101,102,103,104,105,- 1,- 1,108,118,- 1,120,
121,- 1,123,124,- 1,126,118,- 1,- 1,121,- 1,132,- 1,- 1,126,- 1,86,129,- 1,- 1,132,133,- 1,
- 1,136,137,- 1,- 1,98,- 1,- 1,101,102,- 1,104,105,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,118,- 1,120,121,- 1,123,124,- 1,126,- 1,- 1,- 1,- 1,- 1,132};char Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible\000";
char Cyc_Yystack_overflow[21]="\000\000\000\000Yystack_overflow\000";void Cyc_yyerror(
struct _dyneither_ptr);int Cyc_yylex(struct Cyc_Lexing_lexbuf*);static int Cyc_yychar=(
int)'\000';union Cyc_YYSTYPE_union Cyc_yylval=(union Cyc_YYSTYPE_union)((struct Cyc_YYINITIALSVAL_struct){
58});static int Cyc_yynerrs=0;struct _tuple19{struct Cyc_Position_Segment*f1;struct
_tuple1*f2;int f3;};struct _tuple20{struct _dyneither_ptr f1;void*f2;};static char
_tmp457[8]="stdcall";static char _tmp458[6]="cdecl";static char _tmp459[9]="fastcall";
static char _tmp45A[9]="noreturn";static char _tmp45B[6]="const";static char _tmp45C[8]="aligned";
static char _tmp45D[7]="packed";static char _tmp45E[7]="shared";static char _tmp45F[7]="unused";
static char _tmp460[5]="weak";static char _tmp461[10]="dllimport";static char _tmp462[
10]="dllexport";static char _tmp463[23]="no_instrument_function";static char _tmp464[
12]="constructor";static char _tmp465[11]="destructor";static char _tmp466[22]="no_check_memory_usage";
static char _tmp467[5]="pure";struct _tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmpA05(
unsigned int*_tmpA04,unsigned int*_tmpA03,short**_tmpA02){for(*_tmpA04=0;*
_tmpA04 < *_tmpA03;(*_tmpA04)++){(*_tmpA02)[*_tmpA04]=(short)0;}}static void
_tmpA0A(unsigned int*_tmpA09,unsigned int*_tmpA08,union Cyc_YYSTYPE_union**
_tmpA07){for(*_tmpA09=0;*_tmpA09 < *_tmpA08;(*_tmpA09)++){(*_tmpA07)[*_tmpA09]=
Cyc_yylval;}}static void _tmpA0F(unsigned int*_tmpA0E,unsigned int*_tmpA0D,struct
Cyc_Yyltype**_tmpA0C){for(*_tmpA0E=0;*_tmpA0E < *_tmpA0D;(*_tmpA0E)++){(*_tmpA0C)[*
_tmpA0E]=Cyc_yynewloc();}}static void _tmp12BC(unsigned int*_tmp12BB,unsigned int*
_tmp12BA,char**_tmp12B8){for(*_tmp12BB=0;*_tmp12BB < *_tmp12BA;(*_tmp12BB)++){(*
_tmp12B8)[*_tmp12BB]='\000';}}int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){
struct _RegionHandle _tmp388=_new_region("yyregion");struct _RegionHandle*yyregion=&
_tmp388;_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;
int yyssp_offset;unsigned int _tmpA04;unsigned int _tmpA03;short*_tmpA02;
unsigned int _tmpA01;short*yyss=(short*)((_tmpA01=10000,((_tmpA02=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmpA01)),((((_tmpA03=_tmpA01,
_tmpA05(& _tmpA04,& _tmpA03,& _tmpA02))),_tmpA02))))));int yyvsp_offset;unsigned int
_tmpA09;unsigned int _tmpA08;union Cyc_YYSTYPE_union*_tmpA07;unsigned int _tmpA06;
union Cyc_YYSTYPE_union*yyvs=(union Cyc_YYSTYPE_union*)((_tmpA06=10000,((_tmpA07=(
union Cyc_YYSTYPE_union*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE_union),
_tmpA06)),((((_tmpA08=_tmpA06,_tmpA0A(& _tmpA09,& _tmpA08,& _tmpA07))),_tmpA07))))));
int yylsp_offset;unsigned int _tmpA0E;unsigned int _tmpA0D;struct Cyc_Yyltype*
_tmpA0C;unsigned int _tmpA0B;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)((_tmpA0B=
10000,((_tmpA0C=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(
struct Cyc_Yyltype),_tmpA0B)),((((_tmpA0D=_tmpA0B,_tmpA0F(& _tmpA0E,& _tmpA0D,&
_tmpA0C))),_tmpA0C))))));int yystacksize=10000;union Cyc_YYSTYPE_union yyval=Cyc_yylval;
int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;
yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(
10000,++ yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(
yystacksize >= 10000){{const char*_tmpA10;Cyc_yyerror(((_tmpA10="parser stack overflow",
_tag_dyneither(_tmpA10,sizeof(char),22))));}(int)_throw((void*)Cyc_Yystack_overflow);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1008,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 365?(int)Cyc_yytranslate[_check_known_subscript_notnull(366,Cyc_yychar)]:
280;}yyn +=yychar1;if((yyn < 0  || yyn > 6016) || Cyc_yycheck[
_check_known_subscript_notnull(6017,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6017,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1007){int
_tmp38A=0;_npop_handler(0);return _tmp38A;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)-- yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1008,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(500,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL1ED: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL1EE: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp38B;_push_handler(& _tmp38B);{int _tmp38D=0;if(setjmp(_tmp38B.handler))_tmp38D=
1;if(!_tmp38D){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp38C=(void*)_exn_thrown;void*
_tmp38F=_tmp38C;_LL1F1: if(*((void**)_tmp38F)!= Cyc_Core_Failure)goto _LL1F3;
_LL1F2: x=0;goto _LL1F0;_LL1F3:;_LL1F4:(void)_throw(_tmp38F);_LL1F0:;}}}{struct
_handler_cons _tmp390;_push_handler(& _tmp390);{int _tmp392=0;if(setjmp(_tmp390.handler))
_tmp392=1;if(!_tmp392){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);;_pop_handler();}else{void*_tmp391=(void*)_exn_thrown;void*
_tmp394=_tmp391;_LL1F6: if(*((void**)_tmp394)!= Cyc_Core_Failure)goto _LL1F8;
_LL1F7: y=0;goto _LL1F5;_LL1F8:;_LL1F9:(void)_throw(_tmp394);_LL1F5:;}}}{union Cyc_YYSTYPE_union
_tmpA11;yyval=(union Cyc_YYSTYPE_union)(((_tmpA11.YY18).tag=22,(((_tmpA11.YY18).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
x,y),_tmpA11))));}break;}case 3: _LL1EF:{struct Cyc_List_List*_tmpA20;struct Cyc_Absyn_Using_d_struct*
_tmpA1F;struct Cyc_Absyn_Using_d_struct _tmpA1E;struct Cyc_Absyn_Decl*_tmpA1D;union
Cyc_YYSTYPE_union _tmpA1C;yyval=(union Cyc_YYSTYPE_union)(((_tmpA1C.YY18).tag=22,(((
_tmpA1C.YY18).f1=((_tmpA20=_cycalloc(sizeof(*_tmpA20)),((_tmpA20->hd=((_tmpA1D=
_cycalloc(sizeof(*_tmpA1D)),((_tmpA1D->r=(void*)((void*)((_tmpA1F=_cycalloc(
sizeof(*_tmpA1F)),((_tmpA1F[0]=((_tmpA1E.tag=11,((_tmpA1E.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA1E.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA1E)))))),_tmpA1F))))),((
_tmpA1D->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpA1D)))))),((_tmpA20->tl=0,_tmpA20)))))),_tmpA1C))));}
Cyc_Lex_leave_using();break;case 4: _LL1FA:{struct Cyc_List_List*_tmpA2F;struct Cyc_Absyn_Using_d_struct*
_tmpA2E;struct Cyc_Absyn_Using_d_struct _tmpA2D;struct Cyc_Absyn_Decl*_tmpA2C;union
Cyc_YYSTYPE_union _tmpA2B;yyval=(union Cyc_YYSTYPE_union)(((_tmpA2B.YY18).tag=22,(((
_tmpA2B.YY18).f1=((_tmpA2F=_cycalloc(sizeof(*_tmpA2F)),((_tmpA2F->hd=((_tmpA2C=
_cycalloc(sizeof(*_tmpA2C)),((_tmpA2C->r=(void*)((void*)((_tmpA2E=_cycalloc(
sizeof(*_tmpA2E)),((_tmpA2E[0]=((_tmpA2D.tag=11,((_tmpA2D.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpA2D.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA2D)))))),
_tmpA2E))))),((_tmpA2C->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA2C)))))),((
_tmpA2F->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA2F)))))),_tmpA2B))));}break;case 5: _LL1FB:{struct Cyc_List_List*
_tmpA44;struct Cyc_Absyn_Namespace_d_struct*_tmpA43;struct _dyneither_ptr*_tmpA42;
struct Cyc_Absyn_Namespace_d_struct _tmpA41;struct Cyc_Absyn_Decl*_tmpA40;union Cyc_YYSTYPE_union
_tmpA3F;yyval=(union Cyc_YYSTYPE_union)(((_tmpA3F.YY18).tag=22,(((_tmpA3F.YY18).f1=((
_tmpA44=_cycalloc(sizeof(*_tmpA44)),((_tmpA44->hd=((_tmpA40=_cycalloc(sizeof(*
_tmpA40)),((_tmpA40->r=(void*)((void*)((_tmpA43=_cycalloc(sizeof(*_tmpA43)),((
_tmpA43[0]=((_tmpA41.tag=10,((_tmpA41.f1=((_tmpA42=_cycalloc(sizeof(*_tmpA42)),((
_tmpA42[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),_tmpA42)))),((_tmpA41.f2=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA41)))))),_tmpA43))))),((
_tmpA40->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpA40)))))),((_tmpA44->tl=0,_tmpA44)))))),_tmpA3F))));}
Cyc_Lex_leave_namespace();break;case 6: _LL1FC: {struct _dyneither_ptr nspace;struct
Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons _tmp3A6;_push_handler(&
_tmp3A6);{int _tmp3A8=0;if(setjmp(_tmp3A6.handler))_tmp3A8=1;if(!_tmp3A8){nspace=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;
_pop_handler();}else{void*_tmp3A7=(void*)_exn_thrown;void*_tmp3AA=_tmp3A7;_LL1FF:
if(*((void**)_tmp3AA)!= Cyc_Core_Failure)goto _LL201;_LL200:{const char*_tmpA45;
nspace=((_tmpA45="",_tag_dyneither(_tmpA45,sizeof(char),1)));}goto _LL1FE;_LL201:;
_LL202:(void)_throw(_tmp3AA);_LL1FE:;}}}{struct _handler_cons _tmp3AC;
_push_handler(& _tmp3AC);{int _tmp3AE=0;if(setjmp(_tmp3AC.handler))_tmp3AE=1;if(!
_tmp3AE){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]);;_pop_handler();}else{void*_tmp3AD=(void*)_exn_thrown;void*_tmp3B0=
_tmp3AD;_LL204: if(*((void**)_tmp3B0)!= Cyc_Core_Failure)goto _LL206;_LL205: x=0;
goto _LL203;_LL206:;_LL207:(void)_throw(_tmp3B0);_LL203:;}}}{struct _handler_cons
_tmp3B1;_push_handler(& _tmp3B1);{int _tmp3B3=0;if(setjmp(_tmp3B1.handler))_tmp3B3=
1;if(!_tmp3B3){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);;_pop_handler();}else{void*_tmp3B2=(void*)_exn_thrown;void*
_tmp3B5=_tmp3B2;_LL209: if(*((void**)_tmp3B5)!= Cyc_Core_Failure)goto _LL20B;
_LL20A: y=0;goto _LL208;_LL20B:;_LL20C:(void)_throw(_tmp3B5);_LL208:;}}}{struct Cyc_List_List*
_tmpA5A;struct Cyc_Absyn_Namespace_d_struct*_tmpA59;struct _dyneither_ptr*_tmpA58;
struct Cyc_Absyn_Namespace_d_struct _tmpA57;struct Cyc_Absyn_Decl*_tmpA56;union Cyc_YYSTYPE_union
_tmpA55;yyval=(union Cyc_YYSTYPE_union)(((_tmpA55.YY18).tag=22,(((_tmpA55.YY18).f1=((
_tmpA5A=_cycalloc(sizeof(*_tmpA5A)),((_tmpA5A->hd=((_tmpA56=_cycalloc(sizeof(*
_tmpA56)),((_tmpA56->r=(void*)((void*)((_tmpA59=_cycalloc(sizeof(*_tmpA59)),((
_tmpA59[0]=((_tmpA57.tag=10,((_tmpA57.f1=((_tmpA58=_cycalloc(sizeof(*_tmpA58)),((
_tmpA58[0]=nspace,_tmpA58)))),((_tmpA57.f2=x,_tmpA57)))))),_tmpA59))))),((
_tmpA56->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmpA56)))))),((_tmpA5A->tl=y,_tmpA5A)))))),
_tmpA55))));}break;}case 7: _LL1FD:{const char*_tmpA5B;if(Cyc_strcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),((_tmpA5B="C",_tag_dyneither(_tmpA5B,sizeof(char),2))))== 0){
struct Cyc_List_List*_tmpA6A;struct Cyc_Absyn_ExternC_d_struct*_tmpA69;struct Cyc_Absyn_ExternC_d_struct
_tmpA68;struct Cyc_Absyn_Decl*_tmpA67;union Cyc_YYSTYPE_union _tmpA66;yyval=(union
Cyc_YYSTYPE_union)(((_tmpA66.YY18).tag=22,(((_tmpA66.YY18).f1=((_tmpA6A=
_cycalloc(sizeof(*_tmpA6A)),((_tmpA6A->hd=((_tmpA67=_cycalloc(sizeof(*_tmpA67)),((
_tmpA67->r=(void*)((void*)((_tmpA69=_cycalloc(sizeof(*_tmpA69)),((_tmpA69[0]=((
_tmpA68.tag=12,((_tmpA68.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),_tmpA68)))),_tmpA69))))),((_tmpA67->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA67)))))),((
_tmpA6A->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA6A)))))),_tmpA66))));}else{{const char*_tmpA6B;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),((_tmpA6B="C include",_tag_dyneither(_tmpA6B,sizeof(
char),10))))!= 0){const char*_tmpA6C;Cyc_Parse_err(((_tmpA6C="expecting \"C\" or \"C include\"",
_tag_dyneither(_tmpA6C,sizeof(char),29))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));}}{struct Cyc_List_List*
_tmpA7B;struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA7A;struct Cyc_Absyn_ExternCinclude_d_struct
_tmpA79;struct Cyc_Absyn_Decl*_tmpA78;union Cyc_YYSTYPE_union _tmpA77;yyval=(union
Cyc_YYSTYPE_union)(((_tmpA77.YY18).tag=22,(((_tmpA77.YY18).f1=((_tmpA7B=
_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B->hd=((_tmpA78=_cycalloc(sizeof(*_tmpA78)),((
_tmpA78->r=(void*)((void*)((_tmpA7A=_cycalloc(sizeof(*_tmpA7A)),((_tmpA7A[0]=((
_tmpA79.tag=13,((_tmpA79.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),((_tmpA79.f2=0,_tmpA79)))))),_tmpA7A))))),((_tmpA78->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmpA78)))))),((_tmpA7B->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA7B)))))),_tmpA77))));}}}
break;case 8: _LL20D:{const char*_tmpA7C;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),((_tmpA7C="C include",
_tag_dyneither(_tmpA7C,sizeof(char),10))))!= 0){const char*_tmpA7D;Cyc_Parse_err(((
_tmpA7D="expecting \"C include\"",_tag_dyneither(_tmpA7D,sizeof(char),22))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{
struct Cyc_List_List*_tmpA8C;struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA8B;
struct Cyc_Absyn_ExternCinclude_d_struct _tmpA8A;struct Cyc_Absyn_Decl*_tmpA89;
union Cyc_YYSTYPE_union _tmpA88;yyval=(union Cyc_YYSTYPE_union)(((_tmpA88.YY18).tag=
22,(((_tmpA88.YY18).f1=((_tmpA8C=_cycalloc(sizeof(*_tmpA8C)),((_tmpA8C->hd=((
_tmpA89=_cycalloc(sizeof(*_tmpA89)),((_tmpA89->r=(void*)((void*)((_tmpA8B=
_cycalloc(sizeof(*_tmpA8B)),((_tmpA8B[0]=((_tmpA8A.tag=13,((_tmpA8A.f1=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA8A.f2=exs,
_tmpA8A)))))),_tmpA8B))))),((_tmpA89->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA89)))))),((
_tmpA8C->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA8C)))))),_tmpA88))));}break;}case 9: _LL20E:{struct Cyc_List_List*
_tmpA92;struct Cyc_Absyn_Decl*_tmpA91;union Cyc_YYSTYPE_union _tmpA90;yyval=(union
Cyc_YYSTYPE_union)(((_tmpA90.YY18).tag=22,(((_tmpA90.YY18).f1=((_tmpA92=
_cycalloc(sizeof(*_tmpA92)),((_tmpA92->hd=((_tmpA91=_cycalloc(sizeof(*_tmpA91)),((
_tmpA91->r=(void*)((void*)0),((_tmpA91->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA91)))))),((
_tmpA92->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA92)))))),_tmpA90))));}break;case 10: _LL20F:{struct Cyc_List_List*
_tmpA98;struct Cyc_Absyn_Decl*_tmpA97;union Cyc_YYSTYPE_union _tmpA96;yyval=(union
Cyc_YYSTYPE_union)(((_tmpA96.YY18).tag=22,(((_tmpA96.YY18).f1=((_tmpA98=
_cycalloc(sizeof(*_tmpA98)),((_tmpA98->hd=((_tmpA97=_cycalloc(sizeof(*_tmpA97)),((
_tmpA97->r=(void*)((void*)1),((_tmpA97->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA97)))))),((
_tmpA98->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA98)))))),_tmpA96))));}break;case 11: _LL210:{union Cyc_YYSTYPE_union
_tmpA99;yyval=(union Cyc_YYSTYPE_union)(((_tmpA99.YY18).tag=22,(((_tmpA99.YY18).f1=
0,_tmpA99))));}break;case 12: _LL211: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 13: _LL212:{union Cyc_YYSTYPE_union _tmpA9A;yyval=(
union Cyc_YYSTYPE_union)(((_tmpA9A.YY52).tag=56,(((_tmpA9A.YY52).f1=0,_tmpA9A))));}
break;case 14: _LL213:{struct Cyc_List_List*_tmpAA0;struct _tuple19*_tmpA9F;union Cyc_YYSTYPE_union
_tmpA9E;yyval=(union Cyc_YYSTYPE_union)(((_tmpA9E.YY52).tag=56,(((_tmpA9E.YY52).f1=((
_tmpAA0=_cycalloc(sizeof(*_tmpAA0)),((_tmpAA0->hd=((_tmpA9F=_cycalloc(sizeof(*
_tmpA9F)),((_tmpA9F->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpA9F->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpA9F->f3=0,_tmpA9F)))))))),((_tmpAA0->tl=0,_tmpAA0)))))),_tmpA9E))));}break;
case 15: _LL214:{struct Cyc_List_List*_tmpAA6;struct _tuple19*_tmpAA5;union Cyc_YYSTYPE_union
_tmpAA4;yyval=(union Cyc_YYSTYPE_union)(((_tmpAA4.YY52).tag=56,(((_tmpAA4.YY52).f1=((
_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6->hd=((_tmpAA5=_cycalloc(sizeof(*
_tmpAA5)),((_tmpAA5->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpAA5->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpAA5->f3=0,_tmpAA5)))))))),((_tmpAA6->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAA6)))))),_tmpAA4))));}
break;case 16: _LL215:{struct Cyc_List_List*_tmpAB0;struct Cyc_Absyn_Fn_d_struct
_tmpAAF;struct Cyc_Absyn_Fn_d_struct*_tmpAAE;union Cyc_YYSTYPE_union _tmpAAD;yyval=(
union Cyc_YYSTYPE_union)(((_tmpAAD.YY18).tag=22,(((_tmpAAD.YY18).f1=((_tmpAB0=
_cycalloc(sizeof(*_tmpAB0)),((_tmpAB0->hd=Cyc_Absyn_new_decl((void*)((_tmpAAE=
_cycalloc(sizeof(*_tmpAAE)),((_tmpAAE[0]=((_tmpAAF.tag=1,((_tmpAAF.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAAF)))),_tmpAAE)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpAB0->tl=0,_tmpAB0)))))),_tmpAAD))));}break;
case 17: _LL216: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 18: _LL217:{union Cyc_YYSTYPE_union _tmpAB1;yyval=(union Cyc_YYSTYPE_union)(((
_tmpAB1.YY18).tag=22,(((_tmpAB1.YY18).f1=0,_tmpAB1))));}break;case 19: _LL218:{
union Cyc_YYSTYPE_union _tmpAB2;yyval=(union Cyc_YYSTYPE_union)(((_tmpAB2.YY17).tag=
21,(((_tmpAB2.YY17).f1=Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpAB2))));}
break;case 20: _LL219:{struct Cyc_Core_Opt*_tmpAB5;union Cyc_YYSTYPE_union _tmpAB4;
yyval=(union Cyc_YYSTYPE_union)(((_tmpAB4.YY17).tag=21,(((_tmpAB4.YY17).f1=Cyc_Parse_make_function(((
_tmpAB5=_cycalloc(sizeof(*_tmpAB5)),((_tmpAB5->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpAB5)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpAB4))));}
break;case 21: _LL21A:{union Cyc_YYSTYPE_union _tmpAB6;yyval=(union Cyc_YYSTYPE_union)(((
_tmpAB6.YY17).tag=21,(((_tmpAB6.YY17).f1=Cyc_Parse_make_function(0,Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpAB6))));}
break;case 22: _LL21B:{struct Cyc_Core_Opt*_tmpAB9;union Cyc_YYSTYPE_union _tmpAB8;
yyval=(union Cyc_YYSTYPE_union)(((_tmpAB8.YY17).tag=21,(((_tmpAB8.YY17).f1=Cyc_Parse_make_function(((
_tmpAB9=_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpAB9)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpAB8))));}
break;case 23: _LL21C:{struct Cyc_Core_Opt*_tmpABC;union Cyc_YYSTYPE_union _tmpABB;
yyval=(union Cyc_YYSTYPE_union)(((_tmpABB.YY17).tag=21,(((_tmpABB.YY17).f1=Cyc_Parse_make_function(((
_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpABC)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpABB))));}
break;case 24: _LL21D:{struct Cyc_Core_Opt*_tmpABF;union Cyc_YYSTYPE_union _tmpABE;
yyval=(union Cyc_YYSTYPE_union)(((_tmpABE.YY17).tag=21,(((_tmpABE.YY17).f1=Cyc_Parse_make_function(((
_tmpABF=_cycalloc(sizeof(*_tmpABF)),((_tmpABF->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpABF)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpABE))));}
break;case 25: _LL21E: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL21F: Cyc_Lex_leave_using();
break;case 27: _LL220:{struct _dyneither_ptr*_tmpAC0;Cyc_Lex_enter_namespace(((
_tmpAC0=_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAC0)))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL221: Cyc_Lex_leave_namespace();
break;case 29: _LL222:{union Cyc_YYSTYPE_union _tmpAC1;yyval=(union Cyc_YYSTYPE_union)(((
_tmpAC1.YY18).tag=22,(((_tmpAC1.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmpAC1))));}
break;case 30: _LL223:{union Cyc_YYSTYPE_union _tmpAC2;yyval=(union Cyc_YYSTYPE_union)(((
_tmpAC2.YY18).tag=22,(((_tmpAC2.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpAC2))));}
break;case 31: _LL224:{struct Cyc_List_List*_tmpAC5;union Cyc_YYSTYPE_union _tmpAC4;
yyval=(union Cyc_YYSTYPE_union)(((_tmpAC4.YY18).tag=22,(((_tmpAC4.YY18).f1=((
_tmpAC5=_cycalloc(sizeof(*_tmpAC5)),((_tmpAC5->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAC5->tl=0,
_tmpAC5)))))),_tmpAC4))));}break;case 32: _LL225: {struct Cyc_List_List*_tmp3F2=0;{
struct Cyc_List_List*_tmp3F3=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);for(0;_tmp3F3 != 0;_tmp3F3=_tmp3F3->tl){struct
_dyneither_ptr*_tmp3F4=(struct _dyneither_ptr*)_tmp3F3->hd;union Cyc_Absyn_Nmspace_union
_tmpAC8;struct _tuple1*_tmpAC7;struct _tuple1*qv=(_tmpAC7=_cycalloc(sizeof(*
_tmpAC7)),((_tmpAC7->f1=(union Cyc_Absyn_Nmspace_union)(((_tmpAC8.Rel_n).tag=1,(((
_tmpAC8.Rel_n).f1=0,_tmpAC8)))),((_tmpAC7->f2=_tmp3F4,_tmpAC7)))));struct Cyc_Absyn_Vardecl*
_tmp3F5=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);struct Cyc_List_List*
_tmpAC9;_tmp3F2=((_tmpAC9=_cycalloc(sizeof(*_tmpAC9)),((_tmpAC9->hd=_tmp3F5,((
_tmpAC9->tl=_tmp3F2,_tmpAC9))))));}}_tmp3F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp3F2);{struct Cyc_List_List*_tmpACC;union Cyc_YYSTYPE_union
_tmpACB;yyval=(union Cyc_YYSTYPE_union)(((_tmpACB.YY18).tag=22,(((_tmpACB.YY18).f1=((
_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC->hd=Cyc_Absyn_letv_decl(_tmp3F2,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpACC->tl=0,_tmpACC)))))),_tmpACB))));}break;}
case 33: _LL226:{const char*_tmpACD;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpACD="`H",
_tag_dyneither(_tmpACD,sizeof(char),3))))== 0){const char*_tmpAD0;void*_tmpACF;
Cyc_Parse_err((struct _dyneither_ptr)((_tmpACF=0,Cyc_aprintf(((_tmpAD0="bad occurrence of heap region",
_tag_dyneither(_tmpAD0,sizeof(char),30))),_tag_dyneither(_tmpACF,sizeof(void*),0)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));}}{const char*_tmpAD1;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmpAD1="`U",_tag_dyneither(_tmpAD1,sizeof(char),3))))== 0){
const char*_tmpAD4;void*_tmpAD3;Cyc_Parse_err((struct _dyneither_ptr)((_tmpAD3=0,
Cyc_aprintf(((_tmpAD4="bad occurrence of unique region",_tag_dyneither(_tmpAD4,
sizeof(char),32))),_tag_dyneither(_tmpAD3,sizeof(void*),0)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}{struct
_dyneither_ptr*_tmpAD7;struct Cyc_Absyn_Tvar*_tmpAD6;struct Cyc_Absyn_Tvar*tv=(
_tmpAD6=_cycalloc(sizeof(*_tmpAD6)),((_tmpAD6->name=((_tmpAD7=_cycalloc(sizeof(*
_tmpAD7)),((_tmpAD7[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),_tmpAD7)))),((_tmpAD6->identity=- 1,((_tmpAD6->kind=(
void*)Cyc_Tcutil_kind_to_bound((void*)3),_tmpAD6)))))));struct Cyc_Absyn_VarType_struct
_tmpADA;struct Cyc_Absyn_VarType_struct*_tmpAD9;void*t=(void*)((_tmpAD9=_cycalloc(
sizeof(*_tmpAD9)),((_tmpAD9[0]=((_tmpADA.tag=1,((_tmpADA.f1=tv,_tmpADA)))),
_tmpAD9))));struct Cyc_Absyn_RgnHandleType_struct _tmpAE2;struct Cyc_Absyn_RgnHandleType_struct*
_tmpAE1;union Cyc_Absyn_Nmspace_union _tmpADF;struct _dyneither_ptr*_tmpADE;struct
_tuple1*_tmpADD;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpADD=
_cycalloc(sizeof(*_tmpADD)),((_tmpADD->f1=(union Cyc_Absyn_Nmspace_union)(((
_tmpADF.Loc_n).tag=0,_tmpADF)),((_tmpADD->f2=((_tmpADE=_cycalloc(sizeof(*_tmpADE)),((
_tmpADE[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpADE)))),_tmpADD)))))),(void*)((_tmpAE1=_cycalloc(sizeof(*
_tmpAE1)),((_tmpAE1[0]=((_tmpAE2.tag=14,((_tmpAE2.f1=(void*)t,_tmpAE2)))),
_tmpAE1)))),0);{struct Cyc_List_List*_tmpAE5;union Cyc_YYSTYPE_union _tmpAE4;yyval=(
union Cyc_YYSTYPE_union)(((_tmpAE4.YY18).tag=22,(((_tmpAE4.YY18).f1=((_tmpAE5=
_cycalloc(sizeof(*_tmpAE5)),((_tmpAE5->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAE5->tl=0,
_tmpAE5)))))),_tmpAE4))));}break;}case 34: _LL227:{const char*_tmpAE6;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),((_tmpAE6="H",_tag_dyneither(_tmpAE6,sizeof(char),2))))
== 0){const char*_tmpAE7;Cyc_Parse_err(((_tmpAE7="bad occurrence of heap region `H",
_tag_dyneither(_tmpAE7,sizeof(char),33))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));}}{const char*
_tmpAE8;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpAE8="U",
_tag_dyneither(_tmpAE8,sizeof(char),2))))== 0){const char*_tmpAE9;Cyc_Parse_err(((
_tmpAE9="bad occurrence of unique region `U",_tag_dyneither(_tmpAE9,sizeof(char),
35))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));}}if(Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]) && Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){const char*_tmpAEA;
Cyc_Parse_err(((_tmpAEA="open regions cannot be @resetable",_tag_dyneither(
_tmpAEA,sizeof(char),34))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}{struct
_dyneither_ptr*_tmpAF7;const char*_tmpAF6;void*_tmpAF5[1];struct Cyc_String_pa_struct
_tmpAF4;struct Cyc_Absyn_Tvar*_tmpAF3;struct Cyc_Absyn_Tvar*tv=(_tmpAF3=_cycalloc(
sizeof(*_tmpAF3)),((_tmpAF3->name=((_tmpAF7=_cycalloc(sizeof(*_tmpAF7)),((
_tmpAF7[0]=(struct _dyneither_ptr)((_tmpAF4.tag=0,((_tmpAF4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpAF5[0]=& _tmpAF4,
Cyc_aprintf(((_tmpAF6="`%s",_tag_dyneither(_tmpAF6,sizeof(char),4))),
_tag_dyneither(_tmpAF5,sizeof(void*),1)))))))),_tmpAF7)))),((_tmpAF3->identity=-
1,((_tmpAF3->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3),_tmpAF3)))))));
struct Cyc_Absyn_VarType_struct _tmpAFA;struct Cyc_Absyn_VarType_struct*_tmpAF9;
void*t=(void*)((_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9[0]=((_tmpAFA.tag=1,((
_tmpAFA.f1=tv,_tmpAFA)))),_tmpAF9))));struct Cyc_Absyn_RgnHandleType_struct
_tmpB02;struct Cyc_Absyn_RgnHandleType_struct*_tmpB01;union Cyc_Absyn_Nmspace_union
_tmpAFF;struct _dyneither_ptr*_tmpAFE;struct _tuple1*_tmpAFD;struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(((_tmpAFD=_cycalloc(sizeof(*_tmpAFD)),((_tmpAFD->f1=(
union Cyc_Absyn_Nmspace_union)(((_tmpAFF.Loc_n).tag=0,_tmpAFF)),((_tmpAFD->f2=((
_tmpAFE=_cycalloc(sizeof(*_tmpAFE)),((_tmpAFE[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpAFE)))),_tmpAFD)))))),(
void*)((_tmpB01=_cycalloc(sizeof(*_tmpB01)),((_tmpB01[0]=((_tmpB02.tag=14,((
_tmpB02.f1=(void*)t,_tmpB02)))),_tmpB01)))),0);{struct Cyc_List_List*_tmpB05;
union Cyc_YYSTYPE_union _tmpB04;yyval=(union Cyc_YYSTYPE_union)(((_tmpB04.YY18).tag=
22,(((_tmpB04.YY18).f1=((_tmpB05=_cycalloc(sizeof(*_tmpB05)),((_tmpB05->hd=Cyc_Absyn_region_decl(
tv,vd,Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB05->tl=0,
_tmpB05)))))),_tmpB04))));}break;}case 35: _LL228: {struct _dyneither_ptr*_tmpB0D;
struct Cyc_Absyn_Eq_kb_struct*_tmpB0C;struct Cyc_Absyn_Eq_kb_struct _tmpB0B;struct
Cyc_Absyn_Tvar*_tmpB0A;struct Cyc_Absyn_Tvar*tv=(_tmpB0A=_cycalloc(sizeof(*
_tmpB0A)),((_tmpB0A->name=((_tmpB0D=_cycalloc(sizeof(*_tmpB0D)),((_tmpB0D[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpB0D)))),((
_tmpB0A->identity=- 1,((_tmpB0A->kind=(void*)((void*)((_tmpB0C=_cycalloc(sizeof(*
_tmpB0C)),((_tmpB0C[0]=((_tmpB0B.tag=0,((_tmpB0B.f1=(void*)((void*)3),_tmpB0B)))),
_tmpB0C))))),_tmpB0A)))))));union Cyc_Absyn_Nmspace_union _tmpB12;struct
_dyneither_ptr*_tmpB11;struct _tuple1*_tmpB10;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpB10=_cycalloc(sizeof(*_tmpB10)),((_tmpB10->f1=(union Cyc_Absyn_Nmspace_union)(((
_tmpB12.Loc_n).tag=0,_tmpB12)),((_tmpB10->f2=((_tmpB11=_cycalloc(sizeof(*_tmpB11)),((
_tmpB11[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),_tmpB11)))),_tmpB10)))))),(void*)0,0);{struct Cyc_List_List*
_tmpB15;union Cyc_YYSTYPE_union _tmpB14;yyval=(union Cyc_YYSTYPE_union)(((_tmpB14.YY18).tag=
22,(((_tmpB14.YY18).f1=((_tmpB15=_cycalloc(sizeof(*_tmpB15)),((_tmpB15->hd=Cyc_Absyn_alias_decl(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),tv,vd,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpB15->tl=0,_tmpB15)))))),_tmpB14))));}break;}
case 36: _LL229:{union Cyc_YYSTYPE_union _tmpB16;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB16.YY31).tag=35,(((_tmpB16.YY31).f1=0,_tmpB16))));}break;case 37: _LL22A:{
union Cyc_YYSTYPE_union _tmpB17;yyval=(union Cyc_YYSTYPE_union)(((_tmpB17.YY31).tag=
35,(((_tmpB17.YY31).f1=1,_tmpB17))));}break;case 38: _LL22B: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 39: _LL22C:{union
Cyc_YYSTYPE_union _tmpB18;yyval=(union Cyc_YYSTYPE_union)(((_tmpB18.YY18).tag=22,(((
_tmpB18.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])),_tmpB18))));}break;case 40: _LL22D:{struct Cyc_Parse_Declaration_spec*
_tmpB1E;struct Cyc_Core_Opt*_tmpB1D;union Cyc_YYSTYPE_union _tmpB1C;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB1C.YY19).tag=23,(((_tmpB1C.YY19).f1=((_tmpB1E=_cycalloc(sizeof(*_tmpB1E)),((
_tmpB1E->sc=((_tmpB1D=_cycalloc(sizeof(*_tmpB1D)),((_tmpB1D->v=(void*)Cyc_yyget_YY22(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB1D)))),((_tmpB1E->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB1E->type_specs=
0,((_tmpB1E->is_inline=0,((_tmpB1E->attributes=0,_tmpB1E)))))))))))),_tmpB1C))));}
break;case 41: _LL22E: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc != 0){const char*_tmpB21;void*_tmpB20;(_tmpB20=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpB21="Only one storage class is allowed in a declaration",
_tag_dyneither(_tmpB21,sizeof(char),51))),_tag_dyneither(_tmpB20,sizeof(void*),0)));}{
struct Cyc_Parse_Declaration_spec*_tmpB27;struct Cyc_Core_Opt*_tmpB26;union Cyc_YYSTYPE_union
_tmpB25;yyval=(union Cyc_YYSTYPE_union)(((_tmpB25.YY19).tag=23,(((_tmpB25.YY19).f1=((
_tmpB27=_cycalloc(sizeof(*_tmpB27)),((_tmpB27->sc=((_tmpB26=_cycalloc(sizeof(*
_tmpB26)),((_tmpB26->v=(void*)Cyc_yyget_YY22(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpB26)))),((_tmpB27->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((_tmpB27->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpB27->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpB27->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpB27)))))))))))),
_tmpB25))));}break;case 42: _LL22F:{struct Cyc_Parse_Declaration_spec*_tmpB2D;
struct Cyc_List_List*_tmpB2C;union Cyc_YYSTYPE_union _tmpB2B;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB2B.YY19).tag=23,(((_tmpB2B.YY19).f1=((_tmpB2D=_cycalloc(sizeof(*_tmpB2D)),((
_tmpB2D->sc=0,((_tmpB2D->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB2D->type_specs=((
_tmpB2C=_cycalloc(sizeof(*_tmpB2C)),((_tmpB2C->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB2C->tl=0,_tmpB2C)))))),((
_tmpB2D->is_inline=0,((_tmpB2D->attributes=0,_tmpB2D)))))))))))),_tmpB2B))));}
break;case 43: _LL230:{struct Cyc_Parse_Declaration_spec*_tmpB33;struct Cyc_List_List*
_tmpB32;union Cyc_YYSTYPE_union _tmpB31;yyval=(union Cyc_YYSTYPE_union)(((_tmpB31.YY19).tag=
23,(((_tmpB31.YY19).f1=((_tmpB33=_cycalloc(sizeof(*_tmpB33)),((_tmpB33->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpB33->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpB33->type_specs=((_tmpB32=_cycalloc(sizeof(*_tmpB32)),((_tmpB32->hd=(void*)
Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpB32->tl=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,_tmpB32)))))),((_tmpB33->is_inline=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmpB33->attributes=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,
_tmpB33)))))))))))),_tmpB31))));}break;case 44: _LL231:{struct Cyc_Parse_Declaration_spec*
_tmpB36;union Cyc_YYSTYPE_union _tmpB35;yyval=(union Cyc_YYSTYPE_union)(((_tmpB35.YY19).tag=
23,(((_tmpB35.YY19).f1=((_tmpB36=_cycalloc(sizeof(*_tmpB36)),((_tmpB36->sc=0,((
_tmpB36->tq=Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpB36->type_specs=0,((_tmpB36->is_inline=0,((_tmpB36->attributes=
0,_tmpB36)))))))))))),_tmpB35))));}break;case 45: _LL232:{struct Cyc_Parse_Declaration_spec*
_tmpB39;union Cyc_YYSTYPE_union _tmpB38;yyval=(union Cyc_YYSTYPE_union)(((_tmpB38.YY19).tag=
23,(((_tmpB38.YY19).f1=((_tmpB39=_cycalloc(sizeof(*_tmpB39)),((_tmpB39->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpB39->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq),((_tmpB39->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpB39->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpB39->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpB39)))))))))))),
_tmpB38))));}break;case 46: _LL233:{struct Cyc_Parse_Declaration_spec*_tmpB3C;union
Cyc_YYSTYPE_union _tmpB3B;yyval=(union Cyc_YYSTYPE_union)(((_tmpB3B.YY19).tag=23,(((
_tmpB3B.YY19).f1=((_tmpB3C=_cycalloc(sizeof(*_tmpB3C)),((_tmpB3C->sc=0,((_tmpB3C->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB3C->type_specs=
0,((_tmpB3C->is_inline=1,((_tmpB3C->attributes=0,_tmpB3C)))))))))))),_tmpB3B))));}
break;case 47: _LL234:{struct Cyc_Parse_Declaration_spec*_tmpB3F;union Cyc_YYSTYPE_union
_tmpB3E;yyval=(union Cyc_YYSTYPE_union)(((_tmpB3E.YY19).tag=23,(((_tmpB3E.YY19).f1=((
_tmpB3F=_cycalloc(sizeof(*_tmpB3F)),((_tmpB3F->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpB3F->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((_tmpB3F->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpB3F->is_inline=1,((_tmpB3F->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpB3F)))))))))))),
_tmpB3E))));}break;case 48: _LL235:{struct Cyc_Parse_Declaration_spec*_tmpB42;union
Cyc_YYSTYPE_union _tmpB41;yyval=(union Cyc_YYSTYPE_union)(((_tmpB41.YY19).tag=23,(((
_tmpB41.YY19).f1=((_tmpB42=_cycalloc(sizeof(*_tmpB42)),((_tmpB42->sc=0,((_tmpB42->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB42->type_specs=
0,((_tmpB42->is_inline=0,((_tmpB42->attributes=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB42)))))))))))),_tmpB41))));}
break;case 49: _LL236:{struct Cyc_Parse_Declaration_spec*_tmpB45;union Cyc_YYSTYPE_union
_tmpB44;yyval=(union Cyc_YYSTYPE_union)(((_tmpB44.YY19).tag=23,(((_tmpB44.YY19).f1=((
_tmpB45=_cycalloc(sizeof(*_tmpB45)),((_tmpB45->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpB45->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((_tmpB45->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpB45->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpB45->attributes=Cyc_List_imp_append(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes),_tmpB45)))))))))))),
_tmpB44))));}break;case 50: _LL237:{union Cyc_YYSTYPE_union _tmpB46;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB46.YY22).tag=26,(((_tmpB46.YY22).f1=(void*)((void*)4),_tmpB46))));}break;
case 51: _LL238:{union Cyc_YYSTYPE_union _tmpB47;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB47.YY22).tag=26,(((_tmpB47.YY22).f1=(void*)((void*)5),_tmpB47))));}break;
case 52: _LL239:{union Cyc_YYSTYPE_union _tmpB48;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB48.YY22).tag=26,(((_tmpB48.YY22).f1=(void*)((void*)3),_tmpB48))));}break;
case 53: _LL23A:{union Cyc_YYSTYPE_union _tmpB49;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB49.YY22).tag=26,(((_tmpB49.YY22).f1=(void*)((void*)1),_tmpB49))));}break;
case 54: _LL23B:{const char*_tmpB4A;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB4A="C",
_tag_dyneither(_tmpB4A,sizeof(char),2))))!= 0){const char*_tmpB4B;Cyc_Parse_err(((
_tmpB4B="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpB4B,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}}{union Cyc_YYSTYPE_union _tmpB4C;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB4C.YY22).tag=26,(((_tmpB4C.YY22).f1=(void*)((void*)2),_tmpB4C))));}break;
case 55: _LL23C:{union Cyc_YYSTYPE_union _tmpB4D;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB4D.YY22).tag=26,(((_tmpB4D.YY22).f1=(void*)((void*)0),_tmpB4D))));}break;
case 56: _LL23D:{union Cyc_YYSTYPE_union _tmpB4E;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB4E.YY22).tag=26,(((_tmpB4E.YY22).f1=(void*)((void*)6),_tmpB4E))));}break;
case 57: _LL23E:{union Cyc_YYSTYPE_union _tmpB4F;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB4F.YY45).tag=49,(((_tmpB4F.YY45).f1=0,_tmpB4F))));}break;case 58: _LL23F:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 59:
_LL240: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 60: _LL241:{struct Cyc_List_List*_tmpB52;union Cyc_YYSTYPE_union _tmpB51;yyval=(
union Cyc_YYSTYPE_union)(((_tmpB51.YY45).tag=49,(((_tmpB51.YY45).f1=((_tmpB52=
_cycalloc(sizeof(*_tmpB52)),((_tmpB52->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB52->tl=0,_tmpB52)))))),
_tmpB51))));}break;case 61: _LL242:{struct Cyc_List_List*_tmpB55;union Cyc_YYSTYPE_union
_tmpB54;yyval=(union Cyc_YYSTYPE_union)(((_tmpB54.YY45).tag=49,(((_tmpB54.YY45).f1=((
_tmpB55=_cycalloc(sizeof(*_tmpB55)),((_tmpB55->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB55->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB55)))))),_tmpB54))));}
break;case 62: _LL243: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={1,- 1};
static struct _tuple20 att_map[17]={{{_tmp457,_tmp457,_tmp457 + 8},(void*)0},{{
_tmp458,_tmp458,_tmp458 + 6},(void*)1},{{_tmp459,_tmp459,_tmp459 + 9},(void*)2},{{
_tmp45A,_tmp45A,_tmp45A + 9},(void*)3},{{_tmp45B,_tmp45B,_tmp45B + 6},(void*)4},{{
_tmp45C,_tmp45C,_tmp45C + 8},(void*)& att_aligned},{{_tmp45D,_tmp45D,_tmp45D + 7},(
void*)5},{{_tmp45E,_tmp45E,_tmp45E + 7},(void*)7},{{_tmp45F,_tmp45F,_tmp45F + 7},(
void*)8},{{_tmp460,_tmp460,_tmp460 + 5},(void*)9},{{_tmp461,_tmp461,_tmp461 + 10},(
void*)10},{{_tmp462,_tmp462,_tmp462 + 10},(void*)11},{{_tmp463,_tmp463,_tmp463 + 
23},(void*)12},{{_tmp464,_tmp464,_tmp464 + 12},(void*)13},{{_tmp465,_tmp465,
_tmp465 + 11},(void*)14},{{_tmp466,_tmp466,_tmp466 + 22},(void*)15},{{_tmp467,
_tmp467,_tmp467 + 5},(void*)16}};struct _dyneither_ptr _tmp453=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((
_get_dyneither_size(_tmp453,sizeof(char))> 4  && *((const char*)
_check_dyneither_subscript(_tmp453,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp453,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp453,sizeof(char),(int)(_get_dyneither_size(
_tmp453,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp453,sizeof(char),(int)(_get_dyneither_size(_tmp453,sizeof(char))- 3)))== '_')
_tmp453=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp453,2,
_get_dyneither_size(_tmp453,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp453,(struct _dyneither_ptr)(att_map[i]).f1)== 0){{union
Cyc_YYSTYPE_union _tmpB56;yyval=(union Cyc_YYSTYPE_union)(((_tmpB56.YY46).tag=50,(((
_tmpB56.YY46).f1=(void*)(att_map[i]).f2,_tmpB56))));}break;}}if(i == 17){{const
char*_tmpB57;Cyc_Parse_err(((_tmpB57="unrecognized attribute",_tag_dyneither(
_tmpB57,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}{union Cyc_YYSTYPE_union
_tmpB58;yyval=(union Cyc_YYSTYPE_union)(((_tmpB58.YY46).tag=50,(((_tmpB58.YY46).f1=(
void*)((void*)1),_tmpB58))));}}break;}}case 63: _LL244:{union Cyc_YYSTYPE_union
_tmpB59;yyval=(union Cyc_YYSTYPE_union)(((_tmpB59.YY46).tag=50,(((_tmpB59.YY46).f1=(
void*)((void*)4),_tmpB59))));}break;case 64: _LL245: {struct _dyneither_ptr _tmp46A=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
int _tmp46C;struct _tuple11 _tmp46B=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp46C=_tmp46B.f2;{void*
a;{const char*_tmpB5B;const char*_tmpB5A;if(Cyc_zstrcmp((struct _dyneither_ptr)
_tmp46A,((_tmpB5B="regparm",_tag_dyneither(_tmpB5B,sizeof(char),8))))== 0  || Cyc_zstrcmp((
struct _dyneither_ptr)_tmp46A,((_tmpB5A="__regparm__",_tag_dyneither(_tmpB5A,
sizeof(char),12))))== 0){if(_tmp46C < 0  || _tmp46C > 3){const char*_tmpB5C;Cyc_Parse_err(((
_tmpB5C="regparm requires value between 0 and 3",_tag_dyneither(_tmpB5C,sizeof(
char),39))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Regparm_att_struct _tmpB5F;
struct Cyc_Absyn_Regparm_att_struct*_tmpB5E;a=(void*)((_tmpB5E=_cycalloc_atomic(
sizeof(*_tmpB5E)),((_tmpB5E[0]=((_tmpB5F.tag=0,((_tmpB5F.f1=_tmp46C,_tmpB5F)))),
_tmpB5E))));}}else{const char*_tmpB61;const char*_tmpB60;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp46A,((_tmpB61="aligned",_tag_dyneither(_tmpB61,sizeof(char),8))))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp46A,((_tmpB60="__aligned__",
_tag_dyneither(_tmpB60,sizeof(char),12))))== 0){if(_tmp46C < 0){const char*_tmpB62;
Cyc_Parse_err(((_tmpB62="aligned requires positive power of two",_tag_dyneither(
_tmpB62,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{
unsigned int j=(unsigned int)_tmp46C;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){
const char*_tmpB63;Cyc_Parse_err(((_tmpB63="aligned requires positive power of two",
_tag_dyneither(_tmpB63,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Aligned_att_struct
_tmpB66;struct Cyc_Absyn_Aligned_att_struct*_tmpB65;a=(void*)((_tmpB65=
_cycalloc_atomic(sizeof(*_tmpB65)),((_tmpB65[0]=((_tmpB66.tag=1,((_tmpB66.f1=
_tmp46C,_tmpB66)))),_tmpB65))));}}}else{const char*_tmpB68;const char*_tmpB67;if(
Cyc_zstrcmp((struct _dyneither_ptr)_tmp46A,((_tmpB68="initializes",_tag_dyneither(
_tmpB68,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp46A,((
_tmpB67="__initializes__",_tag_dyneither(_tmpB67,sizeof(char),16))))== 0){struct
Cyc_Absyn_Initializes_att_struct _tmpB6B;struct Cyc_Absyn_Initializes_att_struct*
_tmpB6A;a=(void*)((_tmpB6A=_cycalloc_atomic(sizeof(*_tmpB6A)),((_tmpB6A[0]=((
_tmpB6B.tag=4,((_tmpB6B.f1=_tmp46C,_tmpB6B)))),_tmpB6A))));}else{{const char*
_tmpB6C;Cyc_Parse_err(((_tmpB6C="unrecognized attribute",_tag_dyneither(_tmpB6C,
sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)1;}}}}{
union Cyc_YYSTYPE_union _tmpB6D;yyval=(union Cyc_YYSTYPE_union)(((_tmpB6D.YY46).tag=
50,(((_tmpB6D.YY46).f1=(void*)a,_tmpB6D))));}break;}}case 65: _LL246: {struct
_dyneither_ptr _tmp47E=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp47F=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpB6F;const char*_tmpB6E;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp47E,((_tmpB6F="section",
_tag_dyneither(_tmpB6F,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp47E,((_tmpB6E="__section__",_tag_dyneither(_tmpB6E,sizeof(
char),12))))== 0){struct Cyc_Absyn_Section_att_struct _tmpB72;struct Cyc_Absyn_Section_att_struct*
_tmpB71;a=(void*)((_tmpB71=_cycalloc(sizeof(*_tmpB71)),((_tmpB71[0]=((_tmpB72.tag=
2,((_tmpB72.f1=_tmp47F,_tmpB72)))),_tmpB71))));}else{{const char*_tmpB73;Cyc_Parse_err(((
_tmpB73="unrecognized attribute",_tag_dyneither(_tmpB73,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)1;}}{
union Cyc_YYSTYPE_union _tmpB74;yyval=(union Cyc_YYSTYPE_union)(((_tmpB74.YY46).tag=
50,(((_tmpB74.YY46).f1=(void*)a,_tmpB74))));}break;}case 66: _LL247: {struct
_dyneither_ptr _tmp486=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp487=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpB75;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp486,((_tmpB75="__mode__",
_tag_dyneither(_tmpB75,sizeof(char),9))))== 0){struct Cyc_Absyn_Mode_att_struct
_tmpB78;struct Cyc_Absyn_Mode_att_struct*_tmpB77;a=(void*)((_tmpB77=_cycalloc(
sizeof(*_tmpB77)),((_tmpB77[0]=((_tmpB78.tag=5,((_tmpB78.f1=_tmp487,_tmpB78)))),
_tmpB77))));}else{{const char*_tmpB79;Cyc_Parse_err(((_tmpB79="unrecognized attribute",
_tag_dyneither(_tmpB79,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)1;}}{
union Cyc_YYSTYPE_union _tmpB7A;yyval=(union Cyc_YYSTYPE_union)(((_tmpB7A.YY46).tag=
50,(((_tmpB7A.YY46).f1=(void*)a,_tmpB7A))));}break;}case 67: _LL248: {struct
_dyneither_ptr _tmp48D=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dyneither_ptr _tmp48E=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp490;struct
_tuple11 _tmp48F=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp490=_tmp48F.f2;{int _tmp492;struct _tuple11 _tmp491=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp492=_tmp491.f2;{
void*a=(void*)1;{const char*_tmpB7C;const char*_tmpB7B;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp48D,((_tmpB7C="format",_tag_dyneither(_tmpB7C,sizeof(char),7))))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp48D,((_tmpB7B="__format__",
_tag_dyneither(_tmpB7B,sizeof(char),11))))== 0){const char*_tmpB7D;if(Cyc_zstrcmp((
struct _dyneither_ptr)_tmp48E,((_tmpB7D="printf",_tag_dyneither(_tmpB7D,sizeof(
char),7))))== 0){struct Cyc_Absyn_Format_att_struct _tmpB80;struct Cyc_Absyn_Format_att_struct*
_tmpB7F;a=(void*)((_tmpB7F=_cycalloc(sizeof(*_tmpB7F)),((_tmpB7F[0]=((_tmpB80.tag=
3,((_tmpB80.f1=(void*)((void*)0),((_tmpB80.f2=_tmp490,((_tmpB80.f3=_tmp492,
_tmpB80)))))))),_tmpB7F))));}else{const char*_tmpB81;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp48E,((_tmpB81="scanf",_tag_dyneither(_tmpB81,sizeof(char),6))))
== 0){struct Cyc_Absyn_Format_att_struct _tmpB84;struct Cyc_Absyn_Format_att_struct*
_tmpB83;a=(void*)((_tmpB83=_cycalloc(sizeof(*_tmpB83)),((_tmpB83[0]=((_tmpB84.tag=
3,((_tmpB84.f1=(void*)((void*)1),((_tmpB84.f2=_tmp490,((_tmpB84.f3=_tmp492,
_tmpB84)))))))),_tmpB83))));}else{const char*_tmpB85;Cyc_Parse_err(((_tmpB85="unrecognized format type",
_tag_dyneither(_tmpB85,sizeof(char),25))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{
const char*_tmpB86;Cyc_Parse_err(((_tmpB86="unrecognized attribute",
_tag_dyneither(_tmpB86,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}{union Cyc_YYSTYPE_union
_tmpB87;yyval=(union Cyc_YYSTYPE_union)(((_tmpB87.YY46).tag=50,(((_tmpB87.YY46).f1=(
void*)a,_tmpB87))));}break;}}}case 68: _LL249: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 69: _LL24A:{struct
Cyc_Absyn_TypedefType_struct*_tmpB8D;struct Cyc_Absyn_TypedefType_struct _tmpB8C;
union Cyc_YYSTYPE_union _tmpB8B;yyval=(union Cyc_YYSTYPE_union)(((_tmpB8B.YY23).tag=
27,(((_tmpB8B.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpB8D=_cycalloc(
sizeof(*_tmpB8D)),((_tmpB8D[0]=((_tmpB8C.tag=16,((_tmpB8C.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB8C.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB8C.f3=0,((
_tmpB8C.f4=0,_tmpB8C)))))))))),_tmpB8D)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB8B))));}
break;case 70: _LL24B:{union Cyc_YYSTYPE_union _tmpB8E;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB8E.YY23).tag=27,(((_tmpB8E.YY23).f1=(void*)Cyc_Parse_type_spec((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB8E))));}
break;case 71: _LL24C:{union Cyc_YYSTYPE_union _tmpB8F;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB8F.YY23).tag=27,(((_tmpB8F.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(
0,0),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpB8F))));}break;case 72: _LL24D:{union Cyc_YYSTYPE_union
_tmpB90;yyval=(union Cyc_YYSTYPE_union)(((_tmpB90.YY23).tag=27,(((_tmpB90.YY23).f1=(
void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB90))));}
break;case 73: _LL24E:{union Cyc_YYSTYPE_union _tmpB91;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB91.YY23).tag=27,(((_tmpB91.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_char_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpB91))));}break;case 74: _LL24F:{struct Cyc_Parse_Short_spec_struct*
_tmpB97;struct Cyc_Parse_Short_spec_struct _tmpB96;union Cyc_YYSTYPE_union _tmpB95;
yyval=(union Cyc_YYSTYPE_union)(((_tmpB95.YY23).tag=27,(((_tmpB95.YY23).f1=(void*)((
void*)((_tmpB97=_cycalloc(sizeof(*_tmpB97)),((_tmpB97[0]=((_tmpB96.tag=2,((
_tmpB96.f1=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpB96)))),_tmpB97))))),_tmpB95))));}break;case 75:
_LL250:{union Cyc_YYSTYPE_union _tmpB98;yyval=(union Cyc_YYSTYPE_union)(((_tmpB98.YY23).tag=
27,(((_tmpB98.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB98))));}
break;case 76: _LL251:{struct Cyc_Parse_Long_spec_struct*_tmpB9E;struct Cyc_Parse_Long_spec_struct
_tmpB9D;union Cyc_YYSTYPE_union _tmpB9C;yyval=(union Cyc_YYSTYPE_union)(((_tmpB9C.YY23).tag=
27,(((_tmpB9C.YY23).f1=(void*)((void*)((_tmpB9E=_cycalloc(sizeof(*_tmpB9E)),((
_tmpB9E[0]=((_tmpB9D.tag=3,((_tmpB9D.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB9D)))),
_tmpB9E))))),_tmpB9C))));}break;case 77: _LL252:{union Cyc_YYSTYPE_union _tmpB9F;
yyval=(union Cyc_YYSTYPE_union)(((_tmpB9F.YY23).tag=27,(((_tmpB9F.YY23).f1=(void*)
Cyc_Parse_type_spec(Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB9F))));}
break;case 78: _LL253:{union Cyc_YYSTYPE_union _tmpBA0;yyval=(union Cyc_YYSTYPE_union)(((
_tmpBA0.YY23).tag=27,(((_tmpBA0.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_typ(
0),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpBA0))));}break;case 79: _LL254:{struct Cyc_Parse_Signed_spec_struct*
_tmpBA6;struct Cyc_Parse_Signed_spec_struct _tmpBA5;union Cyc_YYSTYPE_union _tmpBA4;
yyval=(union Cyc_YYSTYPE_union)(((_tmpBA4.YY23).tag=27,(((_tmpBA4.YY23).f1=(void*)((
void*)((_tmpBA6=_cycalloc(sizeof(*_tmpBA6)),((_tmpBA6[0]=((_tmpBA5.tag=0,((
_tmpBA5.f1=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpBA5)))),_tmpBA6))))),_tmpBA4))));}break;case 80:
_LL255:{struct Cyc_Parse_Unsigned_spec_struct*_tmpBAC;struct Cyc_Parse_Unsigned_spec_struct
_tmpBAB;union Cyc_YYSTYPE_union _tmpBAA;yyval=(union Cyc_YYSTYPE_union)(((_tmpBAA.YY23).tag=
27,(((_tmpBAA.YY23).f1=(void*)((void*)((_tmpBAC=_cycalloc(sizeof(*_tmpBAC)),((
_tmpBAC[0]=((_tmpBAB.tag=1,((_tmpBAB.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBAB)))),
_tmpBAC))))),_tmpBAA))));}break;case 81: _LL256: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 82: _LL257: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 83: _LL258:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 84:
_LL259:{union Cyc_YYSTYPE_union _tmpBAD;yyval=(union Cyc_YYSTYPE_union)(((_tmpBAD.YY23).tag=
27,(((_tmpBAD.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpBAD))));}
break;case 85: _LL25A:{struct Cyc_Absyn_TupleType_struct*_tmpBB3;struct Cyc_Absyn_TupleType_struct
_tmpBB2;union Cyc_YYSTYPE_union _tmpBB1;yyval=(union Cyc_YYSTYPE_union)(((_tmpBB1.YY23).tag=
27,(((_tmpBB1.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpBB3=_cycalloc(
sizeof(*_tmpBB3)),((_tmpBB3[0]=((_tmpBB2.tag=9,((_tmpBB2.f1=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))),_tmpBB2)))),_tmpBB3)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpBB1))));}break;case 86: _LL25B:{struct Cyc_Absyn_RgnHandleType_struct*
_tmpBB9;struct Cyc_Absyn_RgnHandleType_struct _tmpBB8;union Cyc_YYSTYPE_union
_tmpBB7;yyval=(union Cyc_YYSTYPE_union)(((_tmpBB7.YY23).tag=27,(((_tmpBB7.YY23).f1=(
void*)Cyc_Parse_type_spec((void*)((_tmpBB9=_cycalloc(sizeof(*_tmpBB9)),((_tmpBB9[
0]=((_tmpBB8.tag=14,((_tmpBB8.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpBB8)))),_tmpBB9)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpBB7))));}break;case 87: _LL25C: {void*_tmp4BB=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0);{struct Cyc_Absyn_DynRgnType_struct*_tmpBBF;
struct Cyc_Absyn_DynRgnType_struct _tmpBBE;union Cyc_YYSTYPE_union _tmpBBD;yyval=(
union Cyc_YYSTYPE_union)(((_tmpBBD.YY23).tag=27,(((_tmpBBD.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)((_tmpBBF=_cycalloc(sizeof(*_tmpBBF)),((_tmpBBF[0]=((_tmpBBE.tag=15,((
_tmpBBE.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpBBE.f2=(void*)_tmp4BB,_tmpBBE)))))),_tmpBBF)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpBBD))));}
break;}case 88: _LL25D:{struct Cyc_Absyn_DynRgnType_struct*_tmpBC5;struct Cyc_Absyn_DynRgnType_struct
_tmpBC4;union Cyc_YYSTYPE_union _tmpBC3;yyval=(union Cyc_YYSTYPE_union)(((_tmpBC3.YY23).tag=
27,(((_tmpBC3.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpBC5=_cycalloc(
sizeof(*_tmpBC5)),((_tmpBC5[0]=((_tmpBC4.tag=15,((_tmpBC4.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpBC4.f2=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpBC4)))))),_tmpBC5)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpBC3))));}
break;case 89: _LL25E:{struct Cyc_Absyn_TagType_struct*_tmpBCB;struct Cyc_Absyn_TagType_struct
_tmpBCA;union Cyc_YYSTYPE_union _tmpBC9;yyval=(union Cyc_YYSTYPE_union)(((_tmpBC9.YY23).tag=
27,(((_tmpBC9.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpBCB=_cycalloc(
sizeof(*_tmpBCB)),((_tmpBCB[0]=((_tmpBCA.tag=18,((_tmpBCA.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpBCA)))),_tmpBCB)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpBC9))));}break;case 90: _LL25F:{struct Cyc_Absyn_TagType_struct*
_tmpBD1;struct Cyc_Absyn_TagType_struct _tmpBD0;union Cyc_YYSTYPE_union _tmpBCF;
yyval=(union Cyc_YYSTYPE_union)(((_tmpBCF.YY23).tag=27,(((_tmpBCF.YY23).f1=(void*)
Cyc_Parse_type_spec((void*)((_tmpBD1=_cycalloc(sizeof(*_tmpBD1)),((_tmpBD1[0]=((
_tmpBD0.tag=18,((_tmpBD0.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ik,
0),_tmpBD0)))),_tmpBD1)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpBCF))));}
break;case 91: _LL260:{struct Cyc_Absyn_ValueofType_struct*_tmpBD7;struct Cyc_Absyn_ValueofType_struct
_tmpBD6;union Cyc_YYSTYPE_union _tmpBD5;yyval=(union Cyc_YYSTYPE_union)(((_tmpBD5.YY23).tag=
27,(((_tmpBD5.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpBD7=_cycalloc(
sizeof(*_tmpBD7)),((_tmpBD7[0]=((_tmpBD6.tag=17,((_tmpBD6.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpBD6)))),_tmpBD7)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpBD5))));}break;case 92: _LL261:{union Cyc_YYSTYPE_union
_tmpBD8;yyval=(union Cyc_YYSTYPE_union)(((_tmpBD8.YY43).tag=47,(((_tmpBD8.YY43).f1=(
void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpBD8))));}
break;case 93: _LL262: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line): 0;{struct Cyc_Absyn_Tqual _tmpBDB;union Cyc_YYSTYPE_union
_tmpBDA;yyval=(union Cyc_YYSTYPE_union)(((_tmpBDA.YY25).tag=29,(((_tmpBDA.YY25).f1=((
_tmpBDB.print_const=1,((_tmpBDB.q_volatile=0,((_tmpBDB.q_restrict=0,((_tmpBDB.real_const=
1,((_tmpBDB.loc=loc,_tmpBDB)))))))))),_tmpBDA))));}break;}case 94: _LL263:{struct
Cyc_Absyn_Tqual _tmpBDE;union Cyc_YYSTYPE_union _tmpBDD;yyval=(union Cyc_YYSTYPE_union)(((
_tmpBDD.YY25).tag=29,(((_tmpBDD.YY25).f1=((_tmpBDE.print_const=0,((_tmpBDE.q_volatile=
1,((_tmpBDE.q_restrict=0,((_tmpBDE.real_const=0,((_tmpBDE.loc=0,_tmpBDE)))))))))),
_tmpBDD))));}break;case 95: _LL264:{struct Cyc_Absyn_Tqual _tmpBE1;union Cyc_YYSTYPE_union
_tmpBE0;yyval=(union Cyc_YYSTYPE_union)(((_tmpBE0.YY25).tag=29,(((_tmpBE0.YY25).f1=((
_tmpBE1.print_const=0,((_tmpBE1.q_volatile=0,((_tmpBE1.q_restrict=1,((_tmpBE1.real_const=
0,((_tmpBE1.loc=0,_tmpBE1)))))))))),_tmpBE0))));}break;case 96: _LL265:{struct Cyc_Parse_Decl_spec_struct*
_tmpC05;struct Cyc_Absyn_Decl*_tmpC04;struct Cyc_Absyn_Enum_d_struct _tmpC03;struct
Cyc_Core_Opt*_tmpC02;struct Cyc_Absyn_Enumdecl*_tmpC01;struct Cyc_Absyn_Enum_d_struct*
_tmpC00;struct Cyc_Parse_Decl_spec_struct _tmpBFF;union Cyc_YYSTYPE_union _tmpBFE;
yyval=(union Cyc_YYSTYPE_union)(((_tmpBFE.YY23).tag=27,(((_tmpBFE.YY23).f1=(void*)((
void*)((_tmpC05=_cycalloc(sizeof(*_tmpC05)),((_tmpC05[0]=((_tmpBFF.tag=5,((
_tmpBFF.f1=((_tmpC04=_cycalloc(sizeof(*_tmpC04)),((_tmpC04->r=(void*)((void*)((
_tmpC00=_cycalloc(sizeof(*_tmpC00)),((_tmpC00[0]=((_tmpC03.tag=8,((_tmpC03.f1=((
_tmpC01=_cycalloc(sizeof(*_tmpC01)),((_tmpC01->sc=(void*)((void*)2),((_tmpC01->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpC01->fields=((_tmpC02=_cycalloc(sizeof(*_tmpC02)),((_tmpC02->v=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC02)))),_tmpC01)))))))),
_tmpC03)))),_tmpC00))))),((_tmpC04->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC04)))))),
_tmpBFF)))),_tmpC05))))),_tmpBFE))));}break;case 97: _LL266:{struct Cyc_Absyn_EnumType_struct*
_tmpC0B;struct Cyc_Absyn_EnumType_struct _tmpC0A;union Cyc_YYSTYPE_union _tmpC09;
yyval=(union Cyc_YYSTYPE_union)(((_tmpC09.YY23).tag=27,(((_tmpC09.YY23).f1=(void*)
Cyc_Parse_type_spec((void*)((_tmpC0B=_cycalloc(sizeof(*_tmpC0B)),((_tmpC0B[0]=((
_tmpC0A.tag=12,((_tmpC0A.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC0A.f2=0,_tmpC0A)))))),
_tmpC0B)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpC09))));}break;case 98: _LL267:{struct Cyc_Parse_Type_spec_struct*
_tmpC1A;struct Cyc_Absyn_AnonEnumType_struct*_tmpC19;struct Cyc_Absyn_AnonEnumType_struct
_tmpC18;struct Cyc_Parse_Type_spec_struct _tmpC17;union Cyc_YYSTYPE_union _tmpC16;
yyval=(union Cyc_YYSTYPE_union)(((_tmpC16.YY23).tag=27,(((_tmpC16.YY23).f1=(void*)((
void*)((_tmpC1A=_cycalloc(sizeof(*_tmpC1A)),((_tmpC1A[0]=((_tmpC17.tag=4,((
_tmpC17.f1=(void*)((void*)((_tmpC19=_cycalloc(sizeof(*_tmpC19)),((_tmpC19[0]=((
_tmpC18.tag=13,((_tmpC18.f1=Cyc_yyget_YY48(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpC18)))),_tmpC19))))),((_tmpC17.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC17)))))),
_tmpC1A))))),_tmpC16))));}break;case 99: _LL268:{struct Cyc_Absyn_Enumfield*_tmpC1D;
union Cyc_YYSTYPE_union _tmpC1C;yyval=(union Cyc_YYSTYPE_union)(((_tmpC1C.YY47).tag=
51,(((_tmpC1C.YY47).f1=((_tmpC1D=_cycalloc(sizeof(*_tmpC1D)),((_tmpC1D->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC1D->tag=0,((
_tmpC1D->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpC1D)))))))),_tmpC1C))));}break;case 100: _LL269:{
struct Cyc_Absyn_Enumfield*_tmpC20;union Cyc_YYSTYPE_union _tmpC1F;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC1F.YY47).tag=51,(((_tmpC1F.YY47).f1=((_tmpC20=_cycalloc(sizeof(*_tmpC20)),((
_tmpC20->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmpC20->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC20->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC20)))))))),
_tmpC1F))));}break;case 101: _LL26A:{struct Cyc_List_List*_tmpC23;union Cyc_YYSTYPE_union
_tmpC22;yyval=(union Cyc_YYSTYPE_union)(((_tmpC22.YY48).tag=52,(((_tmpC22.YY48).f1=((
_tmpC23=_cycalloc(sizeof(*_tmpC23)),((_tmpC23->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC23->tl=0,_tmpC23)))))),
_tmpC22))));}break;case 102: _LL26B:{struct Cyc_List_List*_tmpC26;union Cyc_YYSTYPE_union
_tmpC25;yyval=(union Cyc_YYSTYPE_union)(((_tmpC25.YY48).tag=52,(((_tmpC25.YY48).f1=((
_tmpC26=_cycalloc(sizeof(*_tmpC26)),((_tmpC26->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpC26->tl=0,_tmpC26)))))),
_tmpC25))));}break;case 103: _LL26C:{struct Cyc_List_List*_tmpC29;union Cyc_YYSTYPE_union
_tmpC28;yyval=(union Cyc_YYSTYPE_union)(((_tmpC28.YY48).tag=52,(((_tmpC28.YY48).f1=((
_tmpC29=_cycalloc(sizeof(*_tmpC29)),((_tmpC29->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC29->tl=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC29)))))),_tmpC28))));}
break;case 104: _LL26D:{struct Cyc_Absyn_AnonAggrType_struct*_tmpC2F;struct Cyc_Absyn_AnonAggrType_struct
_tmpC2E;union Cyc_YYSTYPE_union _tmpC2D;yyval=(union Cyc_YYSTYPE_union)(((_tmpC2D.YY23).tag=
27,(((_tmpC2D.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpC2F=_cycalloc(
sizeof(*_tmpC2F)),((_tmpC2F[0]=((_tmpC2E.tag=11,((_tmpC2E.f1=(void*)Cyc_yyget_YY24(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpC2E.f2=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC2E)))))),
_tmpC2F)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpC2D))));}break;case 105: _LL26E: {struct Cyc_List_List*
_tmp4EF=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]));struct Cyc_List_List*_tmp4F0=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct*
_tmpC35;struct Cyc_Parse_Decl_spec_struct _tmpC34;union Cyc_YYSTYPE_union _tmpC33;
yyval=(union Cyc_YYSTYPE_union)(((_tmpC33.YY23).tag=27,(((_tmpC33.YY23).f1=(void*)((
void*)((_tmpC35=_cycalloc(sizeof(*_tmpC35)),((_tmpC35[0]=((_tmpC34.tag=5,((
_tmpC34.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4EF,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4F0,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),1),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpC34)))),
_tmpC35))))),_tmpC33))));}break;}case 106: _LL26F: {struct Cyc_List_List*_tmp4F4=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]));struct Cyc_List_List*_tmp4F5=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct*
_tmpC3B;struct Cyc_Parse_Decl_spec_struct _tmpC3A;union Cyc_YYSTYPE_union _tmpC39;
yyval=(union Cyc_YYSTYPE_union)(((_tmpC39.YY23).tag=27,(((_tmpC39.YY23).f1=(void*)((
void*)((_tmpC3B=_cycalloc(sizeof(*_tmpC3B)),((_tmpC3B[0]=((_tmpC3A.tag=5,((
_tmpC3A.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4F4,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4F5,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpC3A)))),
_tmpC3B))))),_tmpC39))));}break;}case 107: _LL270:{struct Cyc_Absyn_AggrType_struct*
_tmpC4A;struct Cyc_Absyn_AggrInfo _tmpC49;union Cyc_Absyn_AggrInfoU_union _tmpC48;
struct Cyc_Absyn_AggrType_struct _tmpC47;union Cyc_YYSTYPE_union _tmpC46;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC46.YY23).tag=27,(((_tmpC46.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)((_tmpC4A=_cycalloc(sizeof(*_tmpC4A)),((_tmpC4A[0]=((_tmpC47.tag=10,((
_tmpC47.f1=((_tmpC49.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((_tmpC48.UnknownAggr).tag=
0,(((_tmpC48.UnknownAggr).f1=(void*)Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(((_tmpC48.UnknownAggr).f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpC48)))))),((_tmpC49.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),_tmpC49)))),_tmpC47)))),_tmpC4A)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpC46))));}
break;case 108: _LL271:{union Cyc_YYSTYPE_union _tmpC4B;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC4B.YY40).tag=44,(((_tmpC4B.YY40).f1=0,_tmpC4B))));}break;case 109: _LL272:{
union Cyc_YYSTYPE_union _tmpC4C;yyval=(union Cyc_YYSTYPE_union)(((_tmpC4C.YY40).tag=
44,(((_tmpC4C.YY40).f1=Cyc_List_imp_rev(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),_tmpC4C))));}break;case
110: _LL273:{union Cyc_YYSTYPE_union _tmpC4D;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC4D.YY24).tag=28,(((_tmpC4D.YY24).f1=(void*)((void*)0),_tmpC4D))));}break;
case 111: _LL274:{union Cyc_YYSTYPE_union _tmpC4E;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC4E.YY24).tag=28,(((_tmpC4E.YY24).f1=(void*)((void*)1),_tmpC4E))));}break;
case 112: _LL275:{union Cyc_YYSTYPE_union _tmpC4F;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC4F.YY26).tag=30,(((_tmpC4F.YY26).f1=0,_tmpC4F))));}break;case 113: _LL276:{
union Cyc_YYSTYPE_union _tmpC50;yyval=(union Cyc_YYSTYPE_union)(((_tmpC50.YY26).tag=
30,(((_tmpC50.YY26).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))),_tmpC50))));}break;
case 114: _LL277:{struct Cyc_List_List*_tmpC53;union Cyc_YYSTYPE_union _tmpC52;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC52.YY27).tag=31,(((_tmpC52.YY27).f1=((_tmpC53=
_cycalloc(sizeof(*_tmpC53)),((_tmpC53->hd=Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC53->tl=0,_tmpC53)))))),
_tmpC52))));}break;case 115: _LL278:{struct Cyc_List_List*_tmpC56;union Cyc_YYSTYPE_union
_tmpC55;yyval=(union Cyc_YYSTYPE_union)(((_tmpC55.YY27).tag=31,(((_tmpC55.YY27).f1=((
_tmpC56=_cycalloc(sizeof(*_tmpC56)),((_tmpC56->hd=Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC56->tl=Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC56)))))),
_tmpC55))));}break;case 116: _LL279:{union Cyc_YYSTYPE_union _tmpC57;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC57.YY21).tag=25,(((_tmpC57.YY21).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])),_tmpC57))));}break;case 117: _LL27A:{struct Cyc_List_List*_tmpC5A;
union Cyc_YYSTYPE_union _tmpC59;yyval=(union Cyc_YYSTYPE_union)(((_tmpC59.YY21).tag=
25,(((_tmpC59.YY21).f1=((_tmpC5A=_cycalloc(sizeof(*_tmpC5A)),((_tmpC5A->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC5A->tl=0,
_tmpC5A)))))),_tmpC59))));}break;case 118: _LL27B:{struct Cyc_List_List*_tmpC5D;
union Cyc_YYSTYPE_union _tmpC5C;yyval=(union Cyc_YYSTYPE_union)(((_tmpC5C.YY21).tag=
25,(((_tmpC5C.YY21).f1=((_tmpC5D=_cycalloc(sizeof(*_tmpC5D)),((_tmpC5D->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC5D->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC5D)))))),
_tmpC5C))));}break;case 119: _LL27C:{struct _tuple15*_tmpC60;union Cyc_YYSTYPE_union
_tmpC5F;yyval=(union Cyc_YYSTYPE_union)(((_tmpC5F.YY20).tag=24,(((_tmpC5F.YY20).f1=((
_tmpC60=_cycalloc(sizeof(*_tmpC60)),((_tmpC60->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC60->f2=0,_tmpC60)))))),
_tmpC5F))));}break;case 120: _LL27D:{struct _tuple15*_tmpC63;union Cyc_YYSTYPE_union
_tmpC62;yyval=(union Cyc_YYSTYPE_union)(((_tmpC62.YY20).tag=24,(((_tmpC62.YY20).f1=((
_tmpC63=_cycalloc(sizeof(*_tmpC63)),((_tmpC63->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC63->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC63)))))),
_tmpC62))));}break;case 121: _LL27E: {struct _RegionHandle _tmp511=_new_region("temp");
struct _RegionHandle*temp=& _tmp511;_push_region(temp);{struct _tuple16 _tmp513;
struct Cyc_Absyn_Tqual _tmp514;struct Cyc_List_List*_tmp515;struct Cyc_List_List*
_tmp516;struct _tuple16*_tmp512=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp513=*_tmp512;_tmp514=
_tmp513.f1;_tmp515=_tmp513.f2;_tmp516=_tmp513.f3;if(_tmp514.loc == 0)_tmp514.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp518;struct Cyc_List_List*
_tmp519;struct _tuple0 _tmp517=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(temp,temp,Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp518=_tmp517.f1;
_tmp519=_tmp517.f2;{void*_tmp51A=Cyc_Parse_speclist2typ(_tmp515,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp51B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(
temp,_tmp514,_tmp51A,_tmp518,_tmp516),_tmp519);union Cyc_YYSTYPE_union _tmpC64;
yyval=(union Cyc_YYSTYPE_union)(((_tmpC64.YY26).tag=30,(((_tmpC64.YY26).f1=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,
struct _tuple8*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp51B),
_tmpC64))));}}}_npop_handler(0);break;;_pop_region(temp);}case 122: _LL27F:{struct
_tuple16*_tmpC6A;struct Cyc_List_List*_tmpC69;union Cyc_YYSTYPE_union _tmpC68;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC68.YY35).tag=39,(((_tmpC68.YY35).f1=((_tmpC6A=
_cycalloc(sizeof(*_tmpC6A)),((_tmpC6A->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpC6A->f2=((
_tmpC69=_cycalloc(sizeof(*_tmpC69)),((_tmpC69->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC69->tl=0,_tmpC69)))))),((
_tmpC6A->f3=0,_tmpC6A)))))))),_tmpC68))));}break;case 123: _LL280:{struct _tuple16*
_tmpC70;struct Cyc_List_List*_tmpC6F;union Cyc_YYSTYPE_union _tmpC6E;yyval=(union
Cyc_YYSTYPE_union)(((_tmpC6E.YY35).tag=39,(((_tmpC6E.YY35).f1=((_tmpC70=
_cycalloc(sizeof(*_tmpC70)),((_tmpC70->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpC70->f2=((_tmpC6F=
_cycalloc(sizeof(*_tmpC6F)),((_tmpC6F->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpC6F->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpC6F)))))),((
_tmpC70->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpC70)))))))),_tmpC6E))));}break;case 124: _LL281:{struct
_tuple16*_tmpC73;union Cyc_YYSTYPE_union _tmpC72;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC72.YY35).tag=39,(((_tmpC72.YY35).f1=((_tmpC73=_cycalloc(sizeof(*_tmpC73)),((
_tmpC73->f1=Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpC73->f2=0,((_tmpC73->f3=0,_tmpC73)))))))),_tmpC72))));}
break;case 125: _LL282:{struct _tuple16*_tmpC76;union Cyc_YYSTYPE_union _tmpC75;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC75.YY35).tag=39,(((_tmpC75.YY35).f1=((_tmpC76=
_cycalloc(sizeof(*_tmpC76)),((_tmpC76->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpC76->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpC76->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpC76)))))))),_tmpC75))));}break;case 126: _LL283:{struct
_tuple16*_tmpC79;union Cyc_YYSTYPE_union _tmpC78;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC78.YY35).tag=39,(((_tmpC78.YY35).f1=((_tmpC79=_cycalloc(sizeof(*_tmpC79)),((
_tmpC79->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpC79->f2=0,((
_tmpC79->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpC79)))))))),_tmpC78))));}break;case 127: _LL284:{struct
_tuple16*_tmpC7C;union Cyc_YYSTYPE_union _tmpC7B;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC7B.YY35).tag=39,(((_tmpC7B.YY35).f1=((_tmpC7C=_cycalloc(sizeof(*_tmpC7C)),((
_tmpC7C->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1,((_tmpC7C->f2=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpC7C->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpC7C)))))))),_tmpC7B))));}break;case 128: _LL285:{struct _tuple16*_tmpC82;struct
Cyc_List_List*_tmpC81;union Cyc_YYSTYPE_union _tmpC80;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC80.YY35).tag=39,(((_tmpC80.YY35).f1=((_tmpC82=_cycalloc(sizeof(*_tmpC82)),((
_tmpC82->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpC82->f2=((
_tmpC81=_cycalloc(sizeof(*_tmpC81)),((_tmpC81->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC81->tl=0,_tmpC81)))))),((
_tmpC82->f3=0,_tmpC82)))))))),_tmpC80))));}break;case 129: _LL286:{struct _tuple16*
_tmpC88;struct Cyc_List_List*_tmpC87;union Cyc_YYSTYPE_union _tmpC86;yyval=(union
Cyc_YYSTYPE_union)(((_tmpC86.YY35).tag=39,(((_tmpC86.YY35).f1=((_tmpC88=
_cycalloc(sizeof(*_tmpC88)),((_tmpC88->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpC88->f2=((_tmpC87=
_cycalloc(sizeof(*_tmpC87)),((_tmpC87->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpC87->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpC87)))))),((
_tmpC88->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpC88)))))))),_tmpC86))));}break;case 130: _LL287:{struct
_tuple16*_tmpC8B;union Cyc_YYSTYPE_union _tmpC8A;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC8A.YY35).tag=39,(((_tmpC8A.YY35).f1=((_tmpC8B=_cycalloc(sizeof(*_tmpC8B)),((
_tmpC8B->f1=Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpC8B->f2=0,((_tmpC8B->f3=0,_tmpC8B)))))))),_tmpC8A))));}
break;case 131: _LL288:{struct _tuple16*_tmpC8E;union Cyc_YYSTYPE_union _tmpC8D;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC8D.YY35).tag=39,(((_tmpC8D.YY35).f1=((_tmpC8E=
_cycalloc(sizeof(*_tmpC8E)),((_tmpC8E->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpC8E->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpC8E->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpC8E)))))))),_tmpC8D))));}break;case 132: _LL289:{struct
_tuple16*_tmpC91;union Cyc_YYSTYPE_union _tmpC90;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC90.YY35).tag=39,(((_tmpC90.YY35).f1=((_tmpC91=_cycalloc(sizeof(*_tmpC91)),((
_tmpC91->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpC91->f2=0,((
_tmpC91->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpC91)))))))),_tmpC90))));}break;case 133: _LL28A:{struct
_tuple16*_tmpC94;union Cyc_YYSTYPE_union _tmpC93;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC93.YY35).tag=39,(((_tmpC93.YY35).f1=((_tmpC94=_cycalloc(sizeof(*_tmpC94)),((
_tmpC94->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1,((_tmpC94->f2=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpC94->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpC94)))))))),_tmpC93))));}break;case 134: _LL28B:{union Cyc_YYSTYPE_union _tmpC95;
yyval=(union Cyc_YYSTYPE_union)(((_tmpC95.YY21).tag=25,(((_tmpC95.YY21).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpC95))));}break;
case 135: _LL28C:{struct Cyc_List_List*_tmpC98;union Cyc_YYSTYPE_union _tmpC97;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC97.YY21).tag=25,(((_tmpC97.YY21).f1=((_tmpC98=
_cycalloc(sizeof(*_tmpC98)),((_tmpC98->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC98->tl=0,_tmpC98)))))),
_tmpC97))));}break;case 136: _LL28D:{struct Cyc_List_List*_tmpC9B;union Cyc_YYSTYPE_union
_tmpC9A;yyval=(union Cyc_YYSTYPE_union)(((_tmpC9A.YY21).tag=25,(((_tmpC9A.YY21).f1=((
_tmpC9B=_cycalloc(sizeof(*_tmpC9B)),((_tmpC9B->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC9B->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC9B)))))),
_tmpC9A))));}break;case 137: _LL28E:{struct _tuple15*_tmpC9E;union Cyc_YYSTYPE_union
_tmpC9D;yyval=(union Cyc_YYSTYPE_union)(((_tmpC9D.YY20).tag=24,(((_tmpC9D.YY20).f1=((
_tmpC9E=_cycalloc(sizeof(*_tmpC9E)),((_tmpC9E->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC9E->f2=0,_tmpC9E)))))),
_tmpC9D))));}break;case 138: _LL28F:{struct _tuple15*_tmpCAD;struct _tuple1*_tmpCAC;
union Cyc_Absyn_Nmspace_union _tmpCAB;struct Cyc_Parse_Declarator*_tmpCAA;union Cyc_YYSTYPE_union
_tmpCA9;yyval=(union Cyc_YYSTYPE_union)(((_tmpCA9.YY20).tag=24,(((_tmpCA9.YY20).f1=((
_tmpCAD=_cycalloc(sizeof(*_tmpCAD)),((_tmpCAD->f1=((_tmpCAA=_cycalloc(sizeof(*
_tmpCAA)),((_tmpCAA->id=((_tmpCAC=_cycalloc(sizeof(*_tmpCAC)),((_tmpCAC->f1=(
union Cyc_Absyn_Nmspace_union)(((_tmpCAB.Rel_n).tag=1,(((_tmpCAB.Rel_n).f1=0,
_tmpCAB)))),((_tmpCAC->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),"",sizeof(char),1),_tmpCAC)))))),((_tmpCAA->tms=0,_tmpCAA)))))),((
_tmpCAD->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCAD)))))),_tmpCA9))));}
break;case 139: _LL290:{struct _tuple15*_tmpCB0;union Cyc_YYSTYPE_union _tmpCAF;yyval=(
union Cyc_YYSTYPE_union)(((_tmpCAF.YY20).tag=24,(((_tmpCAF.YY20).f1=((_tmpCB0=
_cycalloc(sizeof(*_tmpCB0)),((_tmpCB0->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpCB0->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCB0)))))),
_tmpCAF))));}break;case 140: _LL291: {int _tmp549;int _tmp54A;struct _tuple18 _tmp548=
Cyc_yyget_YY53(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp549=_tmp548.f1;_tmp54A=_tmp548.f2;{struct Cyc_List_List*_tmp54B=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct*
_tmpCBA;struct Cyc_Core_Opt*_tmpCB9;struct Cyc_Parse_Decl_spec_struct _tmpCB8;union
Cyc_YYSTYPE_union _tmpCB7;yyval=(union Cyc_YYSTYPE_union)(((_tmpCB7.YY23).tag=27,(((
_tmpCB7.YY23).f1=(void*)((void*)((_tmpCBA=_cycalloc(sizeof(*_tmpCBA)),((_tmpCBA[
0]=((_tmpCB8.tag=5,((_tmpCB8.f1=Cyc_Absyn_datatype_decl((void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp54B,((_tmpCB9=
_cycalloc(sizeof(*_tmpCB9)),((_tmpCB9->v=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpCB9)))),_tmp54A,
_tmp549,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpCB8)))),_tmpCBA))))),_tmpCB7))));}break;}}case
141: _LL292: {int _tmp551;int _tmp552;struct _tuple18 _tmp550=Cyc_yyget_YY53(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp551=_tmp550.f1;
_tmp552=_tmp550.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmpCCF;struct Cyc_Absyn_DatatypeInfo
_tmpCCE;struct Cyc_Absyn_UnknownDatatypeInfo _tmpCCD;union Cyc_Absyn_DatatypeInfoU_union
_tmpCCC;struct Cyc_Absyn_DatatypeType_struct _tmpCCB;union Cyc_YYSTYPE_union _tmpCCA;
yyval=(union Cyc_YYSTYPE_union)(((_tmpCCA.YY23).tag=27,(((_tmpCCA.YY23).f1=(void*)
Cyc_Parse_type_spec((void*)((_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF[0]=((
_tmpCCB.tag=2,((_tmpCCB.f1=((_tmpCCE.datatype_info=(union Cyc_Absyn_DatatypeInfoU_union)(((
_tmpCCC.UnknownDatatype).tag=0,(((_tmpCCC.UnknownDatatype).f1=((_tmpCCD.name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCCD.is_extensible=
_tmp552,((_tmpCCD.is_flat=_tmp551,_tmpCCD)))))),_tmpCCC)))),((_tmpCCE.targs=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCCE.rgn=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpCCE)))))),
_tmpCCB)))),_tmpCCF)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpCCA))));}
break;}case 142: _LL293: {int _tmp55A;int _tmp55B;struct _tuple18 _tmp559=Cyc_yyget_YY53(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp55A=_tmp559.f1;
_tmp55B=_tmp559.f2;{struct Cyc_Absyn_DatatypeFieldType_struct*_tmpCE4;struct Cyc_Absyn_DatatypeFieldInfo
_tmpCE3;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpCE2;union Cyc_Absyn_DatatypeFieldInfoU_union
_tmpCE1;struct Cyc_Absyn_DatatypeFieldType_struct _tmpCE0;union Cyc_YYSTYPE_union
_tmpCDF;yyval=(union Cyc_YYSTYPE_union)(((_tmpCDF.YY23).tag=27,(((_tmpCDF.YY23).f1=(
void*)Cyc_Parse_type_spec((void*)((_tmpCE4=_cycalloc(sizeof(*_tmpCE4)),((_tmpCE4[
0]=((_tmpCE0.tag=3,((_tmpCE0.f1=((_tmpCE3.field_info=(union Cyc_Absyn_DatatypeFieldInfoU_union)(((
_tmpCE1.UnknownDatatypefield).tag=0,(((_tmpCE1.UnknownDatatypefield).f1=((
_tmpCE2.datatype_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),((_tmpCE2.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCE2.is_extensible=
_tmp55B,_tmpCE2)))))),_tmpCE1)))),((_tmpCE3.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCE3)))),_tmpCE0)))),
_tmpCE4)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpCDF))));}break;}case 143: _LL294:{struct _tuple18
_tmpCE7;union Cyc_YYSTYPE_union _tmpCE6;yyval=(union Cyc_YYSTYPE_union)(((_tmpCE6.YY53).tag=
57,(((_tmpCE6.YY53).f1=((_tmpCE7.f1=0,((_tmpCE7.f2=0,_tmpCE7)))),_tmpCE6))));}
break;case 144: _LL295:{struct _tuple18 _tmpCEA;union Cyc_YYSTYPE_union _tmpCE9;yyval=(
union Cyc_YYSTYPE_union)(((_tmpCE9.YY53).tag=57,(((_tmpCE9.YY53).f1=((_tmpCEA.f1=
1,((_tmpCEA.f2=0,_tmpCEA)))),_tmpCE9))));}break;case 145: _LL296:{struct _tuple18
_tmpCED;union Cyc_YYSTYPE_union _tmpCEC;yyval=(union Cyc_YYSTYPE_union)(((_tmpCEC.YY53).tag=
57,(((_tmpCEC.YY53).f1=((_tmpCED.f1=0,((_tmpCED.f2=1,_tmpCED)))),_tmpCEC))));}
break;case 146: _LL297:{struct Cyc_List_List*_tmpCF0;union Cyc_YYSTYPE_union _tmpCEF;
yyval=(union Cyc_YYSTYPE_union)(((_tmpCEF.YY34).tag=38,(((_tmpCEF.YY34).f1=((
_tmpCF0=_cycalloc(sizeof(*_tmpCF0)),((_tmpCF0->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCF0->tl=0,_tmpCF0)))))),
_tmpCEF))));}break;case 147: _LL298:{struct Cyc_List_List*_tmpCF3;union Cyc_YYSTYPE_union
_tmpCF2;yyval=(union Cyc_YYSTYPE_union)(((_tmpCF2.YY34).tag=38,(((_tmpCF2.YY34).f1=((
_tmpCF3=_cycalloc(sizeof(*_tmpCF3)),((_tmpCF3->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCF3->tl=0,_tmpCF3)))))),
_tmpCF2))));}break;case 148: _LL299:{struct Cyc_List_List*_tmpCF6;union Cyc_YYSTYPE_union
_tmpCF5;yyval=(union Cyc_YYSTYPE_union)(((_tmpCF5.YY34).tag=38,(((_tmpCF5.YY34).f1=((
_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpCF6->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCF6)))))),_tmpCF5))));}
break;case 149: _LL29A:{struct Cyc_List_List*_tmpCF9;union Cyc_YYSTYPE_union _tmpCF8;
yyval=(union Cyc_YYSTYPE_union)(((_tmpCF8.YY34).tag=38,(((_tmpCF8.YY34).f1=((
_tmpCF9=_cycalloc(sizeof(*_tmpCF9)),((_tmpCF9->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpCF9->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCF9)))))),_tmpCF8))));}
break;case 150: _LL29B:{union Cyc_YYSTYPE_union _tmpCFA;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCFA.YY32).tag=36,(((_tmpCFA.YY32).f1=(void*)((void*)2),_tmpCFA))));}break;
case 151: _LL29C:{union Cyc_YYSTYPE_union _tmpCFB;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCFB.YY32).tag=36,(((_tmpCFB.YY32).f1=(void*)((void*)3),_tmpCFB))));}break;
case 152: _LL29D:{union Cyc_YYSTYPE_union _tmpCFC;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCFC.YY32).tag=36,(((_tmpCFC.YY32).f1=(void*)((void*)0),_tmpCFC))));}break;
case 153: _LL29E:{struct Cyc_Absyn_Datatypefield*_tmpCFF;union Cyc_YYSTYPE_union
_tmpCFE;yyval=(union Cyc_YYSTYPE_union)(((_tmpCFE.YY33).tag=37,(((_tmpCFE.YY33).f1=((
_tmpCFF=_cycalloc(sizeof(*_tmpCFF)),((_tmpCFF->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCFF->typs=0,((_tmpCFF->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpCFF->sc=(void*)Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpCFF)))))))))),
_tmpCFE))));}break;case 154: _LL29F: {struct Cyc_List_List*_tmp575=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Absyn_Datatypefield*
_tmpD02;union Cyc_YYSTYPE_union _tmpD01;yyval=(union Cyc_YYSTYPE_union)(((_tmpD01.YY33).tag=
37,(((_tmpD01.YY33).f1=((_tmpD02=_cycalloc(sizeof(*_tmpD02)),((_tmpD02->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpD02->typs=
_tmp575,((_tmpD02->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD02->sc=(
void*)Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tmpD02)))))))))),_tmpD01))));}break;}case 155: _LL2A0: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 156: _LL2A1:{struct
Cyc_Parse_Declarator*_tmpD05;union Cyc_YYSTYPE_union _tmpD04;yyval=(union Cyc_YYSTYPE_union)(((
_tmpD04.YY29).tag=33,(((_tmpD04.YY29).f1=((_tmpD05=_cycalloc(sizeof(*_tmpD05)),((
_tmpD05->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id,((_tmpD05->tms=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpD05)))))),
_tmpD04))));}break;case 157: _LL2A2: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 158: _LL2A3:{struct Cyc_Parse_Declarator*_tmpD08;
union Cyc_YYSTYPE_union _tmpD07;yyval=(union Cyc_YYSTYPE_union)(((_tmpD07.YY29).tag=
33,(((_tmpD07.YY29).f1=((_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->id,((_tmpD08->tms=
Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->tms),_tmpD08)))))),_tmpD07))));}break;case 159: _LL2A4:{struct
Cyc_Parse_Declarator*_tmpD0B;union Cyc_YYSTYPE_union _tmpD0A;yyval=(union Cyc_YYSTYPE_union)(((
_tmpD0A.YY29).tag=33,(((_tmpD0A.YY29).f1=((_tmpD0B=_cycalloc(sizeof(*_tmpD0B)),((
_tmpD0B->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpD0B->tms=0,_tmpD0B)))))),_tmpD0A))));}break;case 160: _LL2A5:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 161:
_LL2A6: {struct Cyc_Parse_Declarator*_tmp57E=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpD11;struct Cyc_Absyn_Attributes_mod_struct _tmpD10;struct Cyc_List_List*_tmpD0F;
_tmp57E->tms=((_tmpD0F=_cycalloc(sizeof(*_tmpD0F)),((_tmpD0F->hd=(void*)((void*)((
_tmpD11=_cycalloc(sizeof(*_tmpD11)),((_tmpD11[0]=((_tmpD10.tag=5,((_tmpD10.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpD10.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpD10)))))),_tmpD11))))),((
_tmpD0F->tl=_tmp57E->tms,_tmpD0F))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 162: _LL2A7:{
struct Cyc_Parse_Declarator*_tmpD20;struct Cyc_Absyn_Carray_mod_struct*_tmpD1F;
struct Cyc_Absyn_Carray_mod_struct _tmpD1E;struct Cyc_List_List*_tmpD1D;union Cyc_YYSTYPE_union
_tmpD1C;yyval=(union Cyc_YYSTYPE_union)(((_tmpD1C.YY29).tag=33,(((_tmpD1C.YY29).f1=((
_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpD20->tms=((
_tmpD1D=_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D->hd=(void*)((void*)((_tmpD1F=
_cycalloc(sizeof(*_tmpD1F)),((_tmpD1F[0]=((_tmpD1E.tag=0,((_tmpD1E.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD1E.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD1E)))))),
_tmpD1F))))),((_tmpD1D->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpD1D)))))),_tmpD20)))))),_tmpD1C))));}break;
case 163: _LL2A8:{struct Cyc_Parse_Declarator*_tmpD2F;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpD2E;struct Cyc_Absyn_ConstArray_mod_struct _tmpD2D;struct Cyc_List_List*_tmpD2C;
union Cyc_YYSTYPE_union _tmpD2B;yyval=(union Cyc_YYSTYPE_union)(((_tmpD2B.YY29).tag=
33,(((_tmpD2B.YY29).f1=((_tmpD2F=_cycalloc(sizeof(*_tmpD2F)),((_tmpD2F->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpD2F->tms=((
_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C->hd=(void*)((void*)((_tmpD2E=
_cycalloc(sizeof(*_tmpD2E)),((_tmpD2E[0]=((_tmpD2D.tag=1,((_tmpD2D.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD2D.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD2D.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD2D)))))))),
_tmpD2E))))),((_tmpD2C->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpD2C)))))),_tmpD2F)))))),_tmpD2B))));}break;
case 164: _LL2A9: {struct _tuple17 _tmp58D;struct Cyc_List_List*_tmp58E;int _tmp58F;
struct Cyc_Absyn_VarargInfo*_tmp590;struct Cyc_Core_Opt*_tmp591;struct Cyc_List_List*
_tmp592;struct _tuple17*_tmp58C=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp58D=*_tmp58C;_tmp58E=
_tmp58D.f1;_tmp58F=_tmp58D.f2;_tmp590=_tmp58D.f3;_tmp591=_tmp58D.f4;_tmp592=
_tmp58D.f5;{struct Cyc_Parse_Declarator*_tmpD4B;struct Cyc_Absyn_Function_mod_struct*
_tmpD4A;struct Cyc_Absyn_WithTypes_struct*_tmpD49;struct Cyc_Absyn_WithTypes_struct
_tmpD48;struct Cyc_Absyn_Function_mod_struct _tmpD47;struct Cyc_List_List*_tmpD46;
union Cyc_YYSTYPE_union _tmpD45;yyval=(union Cyc_YYSTYPE_union)(((_tmpD45.YY29).tag=
33,(((_tmpD45.YY29).f1=((_tmpD4B=_cycalloc(sizeof(*_tmpD4B)),((_tmpD4B->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpD4B->tms=((
_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46->hd=(void*)((void*)((_tmpD4A=
_cycalloc(sizeof(*_tmpD4A)),((_tmpD4A[0]=((_tmpD47.tag=3,((_tmpD47.f1=(void*)((
void*)((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49[0]=((_tmpD48.tag=1,((
_tmpD48.f1=_tmp58E,((_tmpD48.f2=_tmp58F,((_tmpD48.f3=_tmp590,((_tmpD48.f4=
_tmp591,((_tmpD48.f5=_tmp592,_tmpD48)))))))))))),_tmpD49))))),_tmpD47)))),
_tmpD4A))))),((_tmpD46->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpD46)))))),_tmpD4B)))))),_tmpD45))));}break;}
case 165: _LL2AA:{struct Cyc_Parse_Declarator*_tmpD67;struct Cyc_Absyn_Function_mod_struct*
_tmpD66;struct Cyc_Absyn_WithTypes_struct*_tmpD65;struct Cyc_Absyn_WithTypes_struct
_tmpD64;struct Cyc_Absyn_Function_mod_struct _tmpD63;struct Cyc_List_List*_tmpD62;
union Cyc_YYSTYPE_union _tmpD61;yyval=(union Cyc_YYSTYPE_union)(((_tmpD61.YY29).tag=
33,(((_tmpD61.YY29).f1=((_tmpD67=_cycalloc(sizeof(*_tmpD67)),((_tmpD67->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpD67->tms=((
_tmpD62=_cycalloc(sizeof(*_tmpD62)),((_tmpD62->hd=(void*)((void*)((_tmpD66=
_cycalloc(sizeof(*_tmpD66)),((_tmpD66[0]=((_tmpD63.tag=3,((_tmpD63.f1=(void*)((
void*)((_tmpD65=_cycalloc(sizeof(*_tmpD65)),((_tmpD65[0]=((_tmpD64.tag=1,((
_tmpD64.f1=0,((_tmpD64.f2=0,((_tmpD64.f3=0,((_tmpD64.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD64.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD64)))))))))))),
_tmpD65))))),_tmpD63)))),_tmpD66))))),((_tmpD62->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpD62)))))),
_tmpD67)))))),_tmpD61))));}break;case 166: _LL2AB:{struct Cyc_Parse_Declarator*
_tmpD83;struct Cyc_Absyn_Function_mod_struct*_tmpD82;struct Cyc_Absyn_NoTypes_struct*
_tmpD81;struct Cyc_Absyn_NoTypes_struct _tmpD80;struct Cyc_Absyn_Function_mod_struct
_tmpD7F;struct Cyc_List_List*_tmpD7E;union Cyc_YYSTYPE_union _tmpD7D;yyval=(union
Cyc_YYSTYPE_union)(((_tmpD7D.YY29).tag=33,(((_tmpD7D.YY29).f1=((_tmpD83=
_cycalloc(sizeof(*_tmpD83)),((_tmpD83->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpD83->tms=((
_tmpD7E=_cycalloc(sizeof(*_tmpD7E)),((_tmpD7E->hd=(void*)((void*)((_tmpD82=
_cycalloc(sizeof(*_tmpD82)),((_tmpD82[0]=((_tmpD7F.tag=3,((_tmpD7F.f1=(void*)((
void*)((_tmpD81=_cycalloc(sizeof(*_tmpD81)),((_tmpD81[0]=((_tmpD80.tag=0,((
_tmpD80.f1=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),((_tmpD80.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD80)))))),
_tmpD81))))),_tmpD7F)))),_tmpD82))))),((_tmpD7E->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmpD7E)))))),
_tmpD83)))))),_tmpD7D))));}break;case 167: _LL2AC: {struct Cyc_List_List*_tmp5A8=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Parse_Declarator*
_tmpD92;struct Cyc_Absyn_TypeParams_mod_struct*_tmpD91;struct Cyc_Absyn_TypeParams_mod_struct
_tmpD90;struct Cyc_List_List*_tmpD8F;union Cyc_YYSTYPE_union _tmpD8E;yyval=(union
Cyc_YYSTYPE_union)(((_tmpD8E.YY29).tag=33,(((_tmpD8E.YY29).f1=((_tmpD92=
_cycalloc(sizeof(*_tmpD92)),((_tmpD92->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpD92->tms=((
_tmpD8F=_cycalloc(sizeof(*_tmpD8F)),((_tmpD8F->hd=(void*)((void*)((_tmpD91=
_cycalloc(sizeof(*_tmpD91)),((_tmpD91[0]=((_tmpD90.tag=4,((_tmpD90.f1=_tmp5A8,((
_tmpD90.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpD90.f3=0,_tmpD90)))))))),_tmpD91))))),((_tmpD8F->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpD8F)))))),_tmpD92)))))),_tmpD8E))));}break;}case 168: _LL2AD:{struct Cyc_Parse_Declarator*
_tmpDA1;struct Cyc_Absyn_Attributes_mod_struct*_tmpDA0;struct Cyc_Absyn_Attributes_mod_struct
_tmpD9F;struct Cyc_List_List*_tmpD9E;union Cyc_YYSTYPE_union _tmpD9D;yyval=(union
Cyc_YYSTYPE_union)(((_tmpD9D.YY29).tag=33,(((_tmpD9D.YY29).f1=((_tmpDA1=
_cycalloc(sizeof(*_tmpDA1)),((_tmpDA1->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpDA1->tms=((
_tmpD9E=_cycalloc(sizeof(*_tmpD9E)),((_tmpD9E->hd=(void*)((void*)((_tmpDA0=
_cycalloc(sizeof(*_tmpDA0)),((_tmpDA0[0]=((_tmpD9F.tag=5,((_tmpD9F.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD9F.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD9F)))))),_tmpDA0))))),((
_tmpD9E->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpD9E)))))),_tmpDA1)))))),_tmpD9D))));}break;case 169:
_LL2AE:{struct Cyc_Parse_Declarator*_tmpDA4;union Cyc_YYSTYPE_union _tmpDA3;yyval=(
union Cyc_YYSTYPE_union)(((_tmpDA3.YY29).tag=33,(((_tmpDA3.YY29).f1=((_tmpDA4=
_cycalloc(sizeof(*_tmpDA4)),((_tmpDA4->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpDA4->tms=0,_tmpDA4)))))),
_tmpDA3))));}break;case 170: _LL2AF:{struct Cyc_Parse_Declarator*_tmpDA7;union Cyc_YYSTYPE_union
_tmpDA6;yyval=(union Cyc_YYSTYPE_union)(((_tmpDA6.YY29).tag=33,(((_tmpDA6.YY29).f1=((
_tmpDA7=_cycalloc(sizeof(*_tmpDA7)),((_tmpDA7->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpDA7->tms=0,_tmpDA7)))))),
_tmpDA6))));}break;case 171: _LL2B0: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 172: _LL2B1: {struct Cyc_Parse_Declarator*
_tmp5B7=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{
struct Cyc_Absyn_Attributes_mod_struct*_tmpDAD;struct Cyc_Absyn_Attributes_mod_struct
_tmpDAC;struct Cyc_List_List*_tmpDAB;_tmp5B7->tms=((_tmpDAB=_cycalloc(sizeof(*
_tmpDAB)),((_tmpDAB->hd=(void*)((void*)((_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((
_tmpDAD[0]=((_tmpDAC.tag=5,((_tmpDAC.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpDAC.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpDAC)))))),_tmpDAD))))),((_tmpDAB->tl=_tmp5B7->tms,_tmpDAB))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 173: _LL2B2:{
struct Cyc_Parse_Declarator*_tmpDBC;struct Cyc_Absyn_Carray_mod_struct*_tmpDBB;
struct Cyc_Absyn_Carray_mod_struct _tmpDBA;struct Cyc_List_List*_tmpDB9;union Cyc_YYSTYPE_union
_tmpDB8;yyval=(union Cyc_YYSTYPE_union)(((_tmpDB8.YY29).tag=33,(((_tmpDB8.YY29).f1=((
_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpDBC->tms=((
_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9->hd=(void*)((void*)((_tmpDBB=
_cycalloc(sizeof(*_tmpDBB)),((_tmpDBB[0]=((_tmpDBA.tag=0,((_tmpDBA.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpDBA.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpDBA)))))),
_tmpDBB))))),((_tmpDB9->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpDB9)))))),_tmpDBC)))))),_tmpDB8))));}break;
case 174: _LL2B3:{struct Cyc_Parse_Declarator*_tmpDCB;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpDCA;struct Cyc_Absyn_ConstArray_mod_struct _tmpDC9;struct Cyc_List_List*_tmpDC8;
union Cyc_YYSTYPE_union _tmpDC7;yyval=(union Cyc_YYSTYPE_union)(((_tmpDC7.YY29).tag=
33,(((_tmpDC7.YY29).f1=((_tmpDCB=_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpDCB->tms=((
_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8->hd=(void*)((void*)((_tmpDCA=
_cycalloc(sizeof(*_tmpDCA)),((_tmpDCA[0]=((_tmpDC9.tag=1,((_tmpDC9.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDC9.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpDC9.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpDC9)))))))),
_tmpDCA))))),((_tmpDC8->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpDC8)))))),_tmpDCB)))))),_tmpDC7))));}break;
case 175: _LL2B4: {struct _tuple17 _tmp5C6;struct Cyc_List_List*_tmp5C7;int _tmp5C8;
struct Cyc_Absyn_VarargInfo*_tmp5C9;struct Cyc_Core_Opt*_tmp5CA;struct Cyc_List_List*
_tmp5CB;struct _tuple17*_tmp5C5=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5C6=*_tmp5C5;_tmp5C7=
_tmp5C6.f1;_tmp5C8=_tmp5C6.f2;_tmp5C9=_tmp5C6.f3;_tmp5CA=_tmp5C6.f4;_tmp5CB=
_tmp5C6.f5;{struct Cyc_Parse_Declarator*_tmpDE7;struct Cyc_Absyn_Function_mod_struct*
_tmpDE6;struct Cyc_Absyn_WithTypes_struct*_tmpDE5;struct Cyc_Absyn_WithTypes_struct
_tmpDE4;struct Cyc_Absyn_Function_mod_struct _tmpDE3;struct Cyc_List_List*_tmpDE2;
union Cyc_YYSTYPE_union _tmpDE1;yyval=(union Cyc_YYSTYPE_union)(((_tmpDE1.YY29).tag=
33,(((_tmpDE1.YY29).f1=((_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpDE7->tms=((
_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2->hd=(void*)((void*)((_tmpDE6=
_cycalloc(sizeof(*_tmpDE6)),((_tmpDE6[0]=((_tmpDE3.tag=3,((_tmpDE3.f1=(void*)((
void*)((_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5[0]=((_tmpDE4.tag=1,((
_tmpDE4.f1=_tmp5C7,((_tmpDE4.f2=_tmp5C8,((_tmpDE4.f3=_tmp5C9,((_tmpDE4.f4=
_tmp5CA,((_tmpDE4.f5=_tmp5CB,_tmpDE4)))))))))))),_tmpDE5))))),_tmpDE3)))),
_tmpDE6))))),((_tmpDE2->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpDE2)))))),_tmpDE7)))))),_tmpDE1))));}break;}
case 176: _LL2B5:{struct Cyc_Parse_Declarator*_tmpE03;struct Cyc_Absyn_Function_mod_struct*
_tmpE02;struct Cyc_Absyn_WithTypes_struct*_tmpE01;struct Cyc_Absyn_WithTypes_struct
_tmpE00;struct Cyc_Absyn_Function_mod_struct _tmpDFF;struct Cyc_List_List*_tmpDFE;
union Cyc_YYSTYPE_union _tmpDFD;yyval=(union Cyc_YYSTYPE_union)(((_tmpDFD.YY29).tag=
33,(((_tmpDFD.YY29).f1=((_tmpE03=_cycalloc(sizeof(*_tmpE03)),((_tmpE03->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpE03->tms=((
_tmpDFE=_cycalloc(sizeof(*_tmpDFE)),((_tmpDFE->hd=(void*)((void*)((_tmpE02=
_cycalloc(sizeof(*_tmpE02)),((_tmpE02[0]=((_tmpDFF.tag=3,((_tmpDFF.f1=(void*)((
void*)((_tmpE01=_cycalloc(sizeof(*_tmpE01)),((_tmpE01[0]=((_tmpE00.tag=1,((
_tmpE00.f1=0,((_tmpE00.f2=0,((_tmpE00.f3=0,((_tmpE00.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpE00.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE00)))))))))))),
_tmpE01))))),_tmpDFF)))),_tmpE02))))),((_tmpDFE->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpDFE)))))),
_tmpE03)))))),_tmpDFD))));}break;case 177: _LL2B6:{struct Cyc_Parse_Declarator*
_tmpE1F;struct Cyc_Absyn_Function_mod_struct*_tmpE1E;struct Cyc_Absyn_NoTypes_struct*
_tmpE1D;struct Cyc_Absyn_NoTypes_struct _tmpE1C;struct Cyc_Absyn_Function_mod_struct
_tmpE1B;struct Cyc_List_List*_tmpE1A;union Cyc_YYSTYPE_union _tmpE19;yyval=(union
Cyc_YYSTYPE_union)(((_tmpE19.YY29).tag=33,(((_tmpE19.YY29).f1=((_tmpE1F=
_cycalloc(sizeof(*_tmpE1F)),((_tmpE1F->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpE1F->tms=((
_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((_tmpE1A->hd=(void*)((void*)((_tmpE1E=
_cycalloc(sizeof(*_tmpE1E)),((_tmpE1E[0]=((_tmpE1B.tag=3,((_tmpE1B.f1=(void*)((
void*)((_tmpE1D=_cycalloc(sizeof(*_tmpE1D)),((_tmpE1D[0]=((_tmpE1C.tag=0,((
_tmpE1C.f1=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),((_tmpE1C.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpE1C)))))),
_tmpE1D))))),_tmpE1B)))),_tmpE1E))))),((_tmpE1A->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmpE1A)))))),
_tmpE1F)))))),_tmpE19))));}break;case 178: _LL2B7: {struct Cyc_List_List*_tmp5E1=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Parse_Declarator*
_tmpE2E;struct Cyc_Absyn_TypeParams_mod_struct*_tmpE2D;struct Cyc_Absyn_TypeParams_mod_struct
_tmpE2C;struct Cyc_List_List*_tmpE2B;union Cyc_YYSTYPE_union _tmpE2A;yyval=(union
Cyc_YYSTYPE_union)(((_tmpE2A.YY29).tag=33,(((_tmpE2A.YY29).f1=((_tmpE2E=
_cycalloc(sizeof(*_tmpE2E)),((_tmpE2E->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpE2E->tms=((
_tmpE2B=_cycalloc(sizeof(*_tmpE2B)),((_tmpE2B->hd=(void*)((void*)((_tmpE2D=
_cycalloc(sizeof(*_tmpE2D)),((_tmpE2D[0]=((_tmpE2C.tag=4,((_tmpE2C.f1=_tmp5E1,((
_tmpE2C.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpE2C.f3=0,_tmpE2C)))))))),_tmpE2D))))),((_tmpE2B->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpE2B)))))),_tmpE2E)))))),_tmpE2A))));}break;}case 179: _LL2B8:{struct Cyc_Parse_Declarator*
_tmpE3D;struct Cyc_Absyn_Attributes_mod_struct*_tmpE3C;struct Cyc_Absyn_Attributes_mod_struct
_tmpE3B;struct Cyc_List_List*_tmpE3A;union Cyc_YYSTYPE_union _tmpE39;yyval=(union
Cyc_YYSTYPE_union)(((_tmpE39.YY29).tag=33,(((_tmpE39.YY29).f1=((_tmpE3D=
_cycalloc(sizeof(*_tmpE3D)),((_tmpE3D->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpE3D->tms=((
_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A->hd=(void*)((void*)((_tmpE3C=
_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C[0]=((_tmpE3B.tag=5,((_tmpE3B.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpE3B.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE3B)))))),_tmpE3C))))),((
_tmpE3A->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpE3A)))))),_tmpE3D)))))),_tmpE39))));}break;case 180:
_LL2B9: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
181: _LL2BA:{union Cyc_YYSTYPE_union _tmpE3E;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE3E.YY28).tag=32,(((_tmpE3E.YY28).f1=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpE3E))));}break;case 182:
_LL2BB: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){struct Cyc_Absyn_Attributes_mod_struct*
_tmpE44;struct Cyc_Absyn_Attributes_mod_struct _tmpE43;struct Cyc_List_List*_tmpE42;
ans=((_tmpE42=_cycalloc(sizeof(*_tmpE42)),((_tmpE42->hd=(void*)((void*)((_tmpE44=
_cycalloc(sizeof(*_tmpE44)),((_tmpE44[0]=((_tmpE43.tag=5,((_tmpE43.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpE43.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpE43)))))),_tmpE44))))),((_tmpE42->tl=ans,_tmpE42))))));}{struct Cyc_Absyn_PtrLoc*
ptrloc=0;if(Cyc_Absyn_porting_c_code){struct Cyc_Absyn_PtrLoc*_tmpE45;ptrloc=((
_tmpE45=_cycalloc(sizeof(*_tmpE45)),((_tmpE45->ptr_loc=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1,((_tmpE45->rgn_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpE45->zt_loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),_tmpE45))))))));}{
struct Cyc_Absyn_Pointer_mod_struct*_tmpE4F;struct Cyc_Absyn_PtrAtts _tmpE4E;struct
Cyc_Absyn_Pointer_mod_struct _tmpE4D;struct Cyc_List_List*_tmpE4C;ans=((_tmpE4C=
_cycalloc(sizeof(*_tmpE4C)),((_tmpE4C->hd=(void*)((void*)((_tmpE4F=_cycalloc(
sizeof(*_tmpE4F)),((_tmpE4F[0]=((_tmpE4D.tag=2,((_tmpE4D.f1=((_tmpE4E.rgn=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpE4E.nullable=(*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])).f2,((_tmpE4E.bounds=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f3,((_tmpE4E.zero_term=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpE4E.ptrloc=ptrloc,_tmpE4E)))))))))),((_tmpE4D.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE4D)))))),_tmpE4F))))),((
_tmpE4C->tl=ans,_tmpE4C))))));}{union Cyc_YYSTYPE_union _tmpE50;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE50.YY28).tag=32,(((_tmpE50.YY28).f1=ans,_tmpE50))));}break;}}case 183: _LL2BC: {
struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line): 0;{struct
_tuple12*_tmpE53;union Cyc_YYSTYPE_union _tmpE52;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE52.YY1).tag=5,(((_tmpE52.YY1).f1=((_tmpE53=_cycalloc(sizeof(*_tmpE53)),((
_tmpE53->f1=loc,((_tmpE53->f2=Cyc_Absyn_true_conref,((_tmpE53->f3=Cyc_yyget_YY2(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE53)))))))),
_tmpE52))));}break;}case 184: _LL2BD: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line): 0;{struct _tuple12*_tmpE56;union Cyc_YYSTYPE_union
_tmpE55;yyval=(union Cyc_YYSTYPE_union)(((_tmpE55.YY1).tag=5,(((_tmpE55.YY1).f1=((
_tmpE56=_cycalloc(sizeof(*_tmpE56)),((_tmpE56->f1=loc,((_tmpE56->f2=Cyc_Absyn_false_conref,((
_tmpE56->f3=Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpE56)))))))),_tmpE55))));}break;}case 185: _LL2BE: {struct Cyc_Position_Segment*
loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line): 0;{struct
_tuple12*_tmpE59;union Cyc_YYSTYPE_union _tmpE58;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE58.YY1).tag=5,(((_tmpE58.YY1).f1=((_tmpE59=_cycalloc(sizeof(*_tmpE59)),((
_tmpE59->f1=loc,((_tmpE59->f2=Cyc_Absyn_true_conref,((_tmpE59->f3=Cyc_Absyn_bounds_dyneither_conref,
_tmpE59)))))))),_tmpE58))));}break;}case 186: _LL2BF:{union Cyc_YYSTYPE_union
_tmpE5A;yyval=(union Cyc_YYSTYPE_union)(((_tmpE5A.YY2).tag=6,(((_tmpE5A.YY2).f1=
Cyc_Absyn_bounds_one_conref,_tmpE5A))));}break;case 187: _LL2C0:{struct Cyc_Absyn_Upper_b_struct*
_tmpE60;struct Cyc_Absyn_Upper_b_struct _tmpE5F;union Cyc_YYSTYPE_union _tmpE5E;
yyval=(union Cyc_YYSTYPE_union)(((_tmpE5E.YY2).tag=6,(((_tmpE5E.YY2).f1=Cyc_Absyn_new_conref((
void*)((_tmpE60=_cycalloc(sizeof(*_tmpE60)),((_tmpE60[0]=((_tmpE5F.tag=0,((
_tmpE5F.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]),_tmpE5F)))),_tmpE60))))),_tmpE5E))));}break;case 188: _LL2C1:{union Cyc_YYSTYPE_union
_tmpE61;yyval=(union Cyc_YYSTYPE_union)(((_tmpE61.YY51).tag=55,(((_tmpE61.YY51).f1=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),_tmpE61))));}break;case
189: _LL2C2:{union Cyc_YYSTYPE_union _tmpE62;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE62.YY51).tag=55,(((_tmpE62.YY51).f1=Cyc_Absyn_true_conref,_tmpE62))));}
break;case 190: _LL2C3:{union Cyc_YYSTYPE_union _tmpE63;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE63.YY51).tag=55,(((_tmpE63.YY51).f1=Cyc_Absyn_false_conref,_tmpE63))));}
break;case 191: _LL2C4:{union Cyc_YYSTYPE_union _tmpE64;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE64.YY49).tag=53,(((_tmpE64.YY49).f1=0,_tmpE64))));}break;case 192: _LL2C5: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)
3,1);{struct Cyc_Core_Opt*_tmpE67;union Cyc_YYSTYPE_union _tmpE66;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE66.YY49).tag=53,(((_tmpE66.YY49).f1=((_tmpE67=_cycalloc(sizeof(*_tmpE67)),((
_tmpE67->v=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE67)))),_tmpE66))));}break;case 193: _LL2C6:{struct Cyc_Core_Opt*
_tmpE6A;union Cyc_YYSTYPE_union _tmpE69;yyval=(union Cyc_YYSTYPE_union)(((_tmpE69.YY49).tag=
53,(((_tmpE69.YY49).f1=((_tmpE6A=_cycalloc(sizeof(*_tmpE6A)),((_tmpE6A->v=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0),_tmpE6A)))),_tmpE69))));}
break;case 194: _LL2C7:{union Cyc_YYSTYPE_union _tmpE6B;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE6B.YY44).tag=48,(((_tmpE6B.YY44).f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,0),_tmpE6B))));}break;case 195: _LL2C8: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)
5,1);yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
196: _LL2C9:{union Cyc_YYSTYPE_union _tmpE6C;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE6C.YY44).tag=48,(((_tmpE6C.YY44).f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,0),_tmpE6C))));}break;case 197: _LL2CA:{union Cyc_YYSTYPE_union
_tmpE6D;yyval=(union Cyc_YYSTYPE_union)(((_tmpE6D.YY25).tag=29,(((_tmpE6D.YY25).f1=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line)),_tmpE6D))));}
break;case 198: _LL2CB:{union Cyc_YYSTYPE_union _tmpE6E;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE6E.YY25).tag=29,(((_tmpE6E.YY25).f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpE6E))));}break;case 199:
_LL2CC:{struct _tuple17*_tmpE71;union Cyc_YYSTYPE_union _tmpE70;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE70.YY39).tag=43,(((_tmpE70.YY39).f1=((_tmpE71=_cycalloc(sizeof(*_tmpE71)),((
_tmpE71->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((
_tmpE71->f2=0,((_tmpE71->f3=0,((_tmpE71->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE71->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE71)))))))))))),
_tmpE70))));}break;case 200: _LL2CD:{struct _tuple17*_tmpE74;union Cyc_YYSTYPE_union
_tmpE73;yyval=(union Cyc_YYSTYPE_union)(((_tmpE73.YY39).tag=43,(((_tmpE73.YY39).f1=((
_tmpE74=_cycalloc(sizeof(*_tmpE74)),((_tmpE74->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),((_tmpE74->f2=1,((
_tmpE74->f3=0,((_tmpE74->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((_tmpE74->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE74)))))))))))),_tmpE73))));}
break;case 201: _LL2CE: {struct _tuple2 _tmp611;struct Cyc_Core_Opt*_tmp612;struct Cyc_Absyn_Tqual
_tmp613;void*_tmp614;struct _tuple2*_tmp610=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp611=*_tmp610;_tmp612=
_tmp611.f1;_tmp613=_tmp611.f2;_tmp614=_tmp611.f3;{struct Cyc_Absyn_VarargInfo*
_tmpE75;struct Cyc_Absyn_VarargInfo*_tmp615=(_tmpE75=_cycalloc(sizeof(*_tmpE75)),((
_tmpE75->name=_tmp612,((_tmpE75->tq=_tmp613,((_tmpE75->type=(void*)_tmp614,((
_tmpE75->inject=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),_tmpE75)))))))));{struct _tuple17*_tmpE78;union Cyc_YYSTYPE_union
_tmpE77;yyval=(union Cyc_YYSTYPE_union)(((_tmpE77.YY39).tag=43,(((_tmpE77.YY39).f1=((
_tmpE78=_cycalloc(sizeof(*_tmpE78)),((_tmpE78->f1=0,((_tmpE78->f2=0,((_tmpE78->f3=
_tmp615,((_tmpE78->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpE78->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE78)))))))))))),_tmpE77))));}
break;}}case 202: _LL2CF: {struct _tuple2 _tmp61A;struct Cyc_Core_Opt*_tmp61B;struct
Cyc_Absyn_Tqual _tmp61C;void*_tmp61D;struct _tuple2*_tmp619=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp61A=*_tmp619;_tmp61B=
_tmp61A.f1;_tmp61C=_tmp61A.f2;_tmp61D=_tmp61A.f3;{struct Cyc_Absyn_VarargInfo*
_tmpE79;struct Cyc_Absyn_VarargInfo*_tmp61E=(_tmpE79=_cycalloc(sizeof(*_tmpE79)),((
_tmpE79->name=_tmp61B,((_tmpE79->tq=_tmp61C,((_tmpE79->type=(void*)_tmp61D,((
_tmpE79->inject=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),_tmpE79)))))))));{struct _tuple17*_tmpE7C;union Cyc_YYSTYPE_union
_tmpE7B;yyval=(union Cyc_YYSTYPE_union)(((_tmpE7B.YY39).tag=43,(((_tmpE7B.YY39).f1=((
_tmpE7C=_cycalloc(sizeof(*_tmpE7C)),((_tmpE7C->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)])),((_tmpE7C->f2=0,((
_tmpE7C->f3=_tmp61E,((_tmpE7C->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE7C->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE7C)))))))))))),
_tmpE7B))));}break;}}case 203: _LL2D0:{struct Cyc_Absyn_Unknown_kb_struct*_tmpE82;
struct Cyc_Absyn_Unknown_kb_struct _tmpE81;union Cyc_YYSTYPE_union _tmpE80;yyval=(
union Cyc_YYSTYPE_union)(((_tmpE80.YY44).tag=48,(((_tmpE80.YY44).f1=(void*)Cyc_Parse_id2type(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)((_tmpE82=_cycalloc(sizeof(*_tmpE82)),((_tmpE82[0]=((_tmpE81.tag=1,((
_tmpE81.f1=0,_tmpE81)))),_tmpE82))))),_tmpE80))));}break;case 204: _LL2D1:{union
Cyc_YYSTYPE_union _tmpE83;yyval=(union Cyc_YYSTYPE_union)(((_tmpE83.YY44).tag=48,(((
_tmpE83.YY44).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))),
_tmpE83))));}break;case 205: _LL2D2:{union Cyc_YYSTYPE_union _tmpE84;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE84.YY49).tag=53,(((_tmpE84.YY49).f1=0,_tmpE84))));}break;case 206: _LL2D3:{
struct Cyc_Core_Opt*_tmpE8E;struct Cyc_Absyn_JoinEff_struct _tmpE8D;struct Cyc_Absyn_JoinEff_struct*
_tmpE8C;union Cyc_YYSTYPE_union _tmpE8B;yyval=(union Cyc_YYSTYPE_union)(((_tmpE8B.YY49).tag=
53,(((_tmpE8B.YY49).f1=((_tmpE8E=_cycalloc(sizeof(*_tmpE8E)),((_tmpE8E->v=(void*)((
void*)((_tmpE8C=_cycalloc(sizeof(*_tmpE8C)),((_tmpE8C[0]=((_tmpE8D.tag=20,((
_tmpE8D.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpE8D)))),_tmpE8C))))),_tmpE8E)))),_tmpE8B))));}break;case 207: _LL2D4:{union Cyc_YYSTYPE_union
_tmpE8F;yyval=(union Cyc_YYSTYPE_union)(((_tmpE8F.YY50).tag=54,(((_tmpE8F.YY50).f1=
0,_tmpE8F))));}break;case 208: _LL2D5: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 209: _LL2D6: {struct Cyc_Absyn_Less_kb_struct
_tmpE92;struct Cyc_Absyn_Less_kb_struct*_tmpE91;struct Cyc_Absyn_Less_kb_struct*
_tmp62C=(_tmpE91=_cycalloc(sizeof(*_tmpE91)),((_tmpE91[0]=((_tmpE92.tag=2,((
_tmpE92.f1=0,((_tmpE92.f2=(void*)((void*)5),_tmpE92)))))),_tmpE91)));struct
_dyneither_ptr _tmp62D=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);void*_tmp62E=Cyc_Parse_id2type(_tmp62D,(void*)_tmp62C);{
struct Cyc_List_List*_tmpEA1;struct Cyc_Absyn_JoinEff_struct*_tmpEA0;struct Cyc_Absyn_JoinEff_struct
_tmpE9F;struct _tuple4*_tmpE9E;union Cyc_YYSTYPE_union _tmpE9D;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE9D.YY50).tag=54,(((_tmpE9D.YY50).f1=((_tmpEA1=_cycalloc(sizeof(*_tmpEA1)),((
_tmpEA1->hd=((_tmpE9E=_cycalloc(sizeof(*_tmpE9E)),((_tmpE9E->f1=(void*)((_tmpEA0=
_cycalloc(sizeof(*_tmpEA0)),((_tmpEA0[0]=((_tmpE9F.tag=20,((_tmpE9F.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE9F)))),_tmpEA0)))),((
_tmpE9E->f2=_tmp62E,_tmpE9E)))))),((_tmpEA1->tl=0,_tmpEA1)))))),_tmpE9D))));}
break;}case 210: _LL2D7: {struct Cyc_Absyn_Less_kb_struct _tmpEA4;struct Cyc_Absyn_Less_kb_struct*
_tmpEA3;struct Cyc_Absyn_Less_kb_struct*_tmp636=(_tmpEA3=_cycalloc(sizeof(*
_tmpEA3)),((_tmpEA3[0]=((_tmpEA4.tag=2,((_tmpEA4.f1=0,((_tmpEA4.f2=(void*)((void*)
5),_tmpEA4)))))),_tmpEA3)));struct _dyneither_ptr _tmp637=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);void*_tmp638=Cyc_Parse_id2type(
_tmp637,(void*)_tmp636);{struct Cyc_List_List*_tmpEB3;struct Cyc_Absyn_JoinEff_struct*
_tmpEB2;struct Cyc_Absyn_JoinEff_struct _tmpEB1;struct _tuple4*_tmpEB0;union Cyc_YYSTYPE_union
_tmpEAF;yyval=(union Cyc_YYSTYPE_union)(((_tmpEAF.YY50).tag=54,(((_tmpEAF.YY50).f1=((
_tmpEB3=_cycalloc(sizeof(*_tmpEB3)),((_tmpEB3->hd=((_tmpEB0=_cycalloc(sizeof(*
_tmpEB0)),((_tmpEB0->f1=(void*)((_tmpEB2=_cycalloc(sizeof(*_tmpEB2)),((_tmpEB2[0]=((
_tmpEB1.tag=20,((_tmpEB1.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),_tmpEB1)))),_tmpEB2)))),((_tmpEB0->f2=_tmp638,_tmpEB0)))))),((
_tmpEB3->tl=Cyc_yyget_YY50(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpEB3)))))),_tmpEAF))));}break;}case 211: _LL2D8:{union Cyc_YYSTYPE_union
_tmpEB4;yyval=(union Cyc_YYSTYPE_union)(((_tmpEB4.YY31).tag=35,(((_tmpEB4.YY31).f1=
0,_tmpEB4))));}break;case 212: _LL2D9:{const char*_tmpEB5;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpEB5="inject",_tag_dyneither(_tmpEB5,sizeof(char),7))))!= 0){
const char*_tmpEB6;Cyc_Parse_err(((_tmpEB6="missing type in function declaration",
_tag_dyneither(_tmpEB6,sizeof(char),37))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}{union Cyc_YYSTYPE_union
_tmpEB7;yyval=(union Cyc_YYSTYPE_union)(((_tmpEB7.YY31).tag=35,(((_tmpEB7.YY31).f1=
1,_tmpEB7))));}break;case 213: _LL2DA: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 214: _LL2DB:{union Cyc_YYSTYPE_union _tmpEB8;yyval=(
union Cyc_YYSTYPE_union)(((_tmpEB8.YY40).tag=44,(((_tmpEB8.YY40).f1=Cyc_List_imp_append(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpEB8))));}break;
case 215: _LL2DC:{union Cyc_YYSTYPE_union _tmpEB9;yyval=(union Cyc_YYSTYPE_union)(((
_tmpEB9.YY40).tag=44,(((_tmpEB9.YY40).f1=0,_tmpEB9))));}break;case 216: _LL2DD:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 217:
_LL2DE:{struct Cyc_List_List*_tmpEC3;struct Cyc_Absyn_RgnsEff_struct _tmpEC2;struct
Cyc_Absyn_RgnsEff_struct*_tmpEC1;union Cyc_YYSTYPE_union _tmpEC0;yyval=(union Cyc_YYSTYPE_union)(((
_tmpEC0.YY40).tag=44,(((_tmpEC0.YY40).f1=((_tmpEC3=_cycalloc(sizeof(*_tmpEC3)),((
_tmpEC3->hd=(void*)((void*)((_tmpEC1=_cycalloc(sizeof(*_tmpEC1)),((_tmpEC1[0]=((
_tmpEC2.tag=21,((_tmpEC2.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpEC2)))),_tmpEC1))))),((
_tmpEC3->tl=0,_tmpEC3)))))),_tmpEC0))));}break;case 218: _LL2DF: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)
6,0);{struct Cyc_List_List*_tmpEC6;union Cyc_YYSTYPE_union _tmpEC5;yyval=(union Cyc_YYSTYPE_union)(((
_tmpEC5.YY40).tag=44,(((_tmpEC5.YY40).f1=((_tmpEC6=_cycalloc(sizeof(*_tmpEC6)),((
_tmpEC6->hd=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpEC6->tl=0,_tmpEC6)))))),_tmpEC5))));}break;case 219: _LL2E0:
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),(void*)5,1);{struct Cyc_List_List*_tmpED0;struct Cyc_Absyn_AccessEff_struct
_tmpECF;struct Cyc_Absyn_AccessEff_struct*_tmpECE;union Cyc_YYSTYPE_union _tmpECD;
yyval=(union Cyc_YYSTYPE_union)(((_tmpECD.YY40).tag=44,(((_tmpECD.YY40).f1=((
_tmpED0=_cycalloc(sizeof(*_tmpED0)),((_tmpED0->hd=(void*)((void*)((_tmpECE=
_cycalloc(sizeof(*_tmpECE)),((_tmpECE[0]=((_tmpECF.tag=19,((_tmpECF.f1=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpECF)))),
_tmpECE))))),((_tmpED0->tl=0,_tmpED0)))))),_tmpECD))));}break;case 220: _LL2E1: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
void*)5,1);{struct Cyc_List_List*_tmpEDA;struct Cyc_Absyn_AccessEff_struct _tmpED9;
struct Cyc_Absyn_AccessEff_struct*_tmpED8;union Cyc_YYSTYPE_union _tmpED7;yyval=(
union Cyc_YYSTYPE_union)(((_tmpED7.YY40).tag=44,(((_tmpED7.YY40).f1=((_tmpEDA=
_cycalloc(sizeof(*_tmpEDA)),((_tmpEDA->hd=(void*)((void*)((_tmpED8=_cycalloc(
sizeof(*_tmpED8)),((_tmpED8[0]=((_tmpED9.tag=19,((_tmpED9.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpED9)))),_tmpED8))))),((
_tmpEDA->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpEDA)))))),_tmpED7))));}break;case 221: _LL2E2:{struct Cyc_List_List*
_tmpEDD;union Cyc_YYSTYPE_union _tmpEDC;yyval=(union Cyc_YYSTYPE_union)(((_tmpEDC.YY38).tag=
42,(((_tmpEDC.YY38).f1=((_tmpEDD=_cycalloc(sizeof(*_tmpEDD)),((_tmpEDD->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpEDD->tl=0,
_tmpEDD)))))),_tmpEDC))));}break;case 222: _LL2E3:{struct Cyc_List_List*_tmpEE0;
union Cyc_YYSTYPE_union _tmpEDF;yyval=(union Cyc_YYSTYPE_union)(((_tmpEDF.YY38).tag=
42,(((_tmpEDF.YY38).f1=((_tmpEE0=_cycalloc(sizeof(*_tmpEE0)),((_tmpEE0->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpEE0->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpEE0)))))),
_tmpEDF))));}break;case 223: _LL2E4: {struct _tuple16 _tmp659;struct Cyc_Absyn_Tqual
_tmp65A;struct Cyc_List_List*_tmp65B;struct Cyc_List_List*_tmp65C;struct _tuple16*
_tmp658=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp659=*_tmp658;_tmp65A=_tmp659.f1;_tmp65B=_tmp659.f2;_tmp65C=_tmp659.f3;if(
_tmp65A.loc == 0)_tmp65A.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator
_tmp65E;struct _tuple1*_tmp65F;struct Cyc_List_List*_tmp660;struct Cyc_Parse_Declarator*
_tmp65D=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp65E=*_tmp65D;_tmp65F=_tmp65E.id;_tmp660=_tmp65E.tms;{void*_tmp661=Cyc_Parse_speclist2typ(
_tmp65B,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp663;void*_tmp664;struct
Cyc_List_List*_tmp665;struct Cyc_List_List*_tmp666;struct _tuple6 _tmp662=Cyc_Parse_apply_tms(
_tmp65A,_tmp661,_tmp65C,_tmp660);_tmp663=_tmp662.f1;_tmp664=_tmp662.f2;_tmp665=
_tmp662.f3;_tmp666=_tmp662.f4;if(_tmp665 != 0){const char*_tmpEE1;Cyc_Parse_err(((
_tmpEE1="parameter with bad type params",_tag_dyneither(_tmpEE1,sizeof(char),31))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}if(Cyc_Absyn_is_qvar_qualified(_tmp65F)){const char*
_tmpEE2;Cyc_Parse_err(((_tmpEE2="parameter cannot be qualified with a namespace",
_tag_dyneither(_tmpEE2,sizeof(char),47))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Core_Opt*
_tmpEE3;struct Cyc_Core_Opt*_tmp669=(struct Cyc_Core_Opt*)((_tmpEE3=_cycalloc(
sizeof(*_tmpEE3)),((_tmpEE3->v=(*_tmp65F).f2,_tmpEE3))));if(_tmp666 != 0){const
char*_tmpEE6;void*_tmpEE5;(_tmpEE5=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpEE6="extra attributes on parameter, ignoring",
_tag_dyneither(_tmpEE6,sizeof(char),40))),_tag_dyneither(_tmpEE5,sizeof(void*),0)));}{
struct _tuple2*_tmpEE9;union Cyc_YYSTYPE_union _tmpEE8;yyval=(union Cyc_YYSTYPE_union)(((
_tmpEE8.YY37).tag=41,(((_tmpEE8.YY37).f1=((_tmpEE9=_cycalloc(sizeof(*_tmpEE9)),((
_tmpEE9->f1=_tmp669,((_tmpEE9->f2=_tmp663,((_tmpEE9->f3=_tmp664,_tmpEE9)))))))),
_tmpEE8))));}break;}}}}case 224: _LL2E5: {struct _tuple16 _tmp670;struct Cyc_Absyn_Tqual
_tmp671;struct Cyc_List_List*_tmp672;struct Cyc_List_List*_tmp673;struct _tuple16*
_tmp66F=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp670=*_tmp66F;_tmp671=_tmp670.f1;_tmp672=_tmp670.f2;_tmp673=_tmp670.f3;if(
_tmp671.loc == 0)_tmp671.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);{void*_tmp674=
Cyc_Parse_speclist2typ(_tmp672,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp673 != 0){
const char*_tmpEEC;void*_tmpEEB;(_tmpEEB=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpEEC="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpEEC,sizeof(char),38))),_tag_dyneither(_tmpEEB,sizeof(void*),0)));}{
struct _tuple2*_tmpEEF;union Cyc_YYSTYPE_union _tmpEEE;yyval=(union Cyc_YYSTYPE_union)(((
_tmpEEE.YY37).tag=41,(((_tmpEEE.YY37).f1=((_tmpEEF=_cycalloc(sizeof(*_tmpEEF)),((
_tmpEEF->f1=0,((_tmpEEF->f2=_tmp671,((_tmpEEF->f3=_tmp674,_tmpEEF)))))))),
_tmpEEE))));}break;}}case 225: _LL2E6: {struct _tuple16 _tmp67A;struct Cyc_Absyn_Tqual
_tmp67B;struct Cyc_List_List*_tmp67C;struct Cyc_List_List*_tmp67D;struct _tuple16*
_tmp679=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp67A=*_tmp679;_tmp67B=_tmp67A.f1;_tmp67C=_tmp67A.f2;_tmp67D=_tmp67A.f3;if(
_tmp67B.loc == 0)_tmp67B.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{void*_tmp67E=
Cyc_Parse_speclist2typ(_tmp67C,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp67F=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp681;void*_tmp682;struct Cyc_List_List*_tmp683;struct Cyc_List_List*
_tmp684;struct _tuple6 _tmp680=Cyc_Parse_apply_tms(_tmp67B,_tmp67E,_tmp67D,_tmp67F);
_tmp681=_tmp680.f1;_tmp682=_tmp680.f2;_tmp683=_tmp680.f3;_tmp684=_tmp680.f4;if(
_tmp683 != 0){const char*_tmpEF2;void*_tmpEF1;(_tmpEF1=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpEF2="bad type parameters on formal argument, ignoring",
_tag_dyneither(_tmpEF2,sizeof(char),49))),_tag_dyneither(_tmpEF1,sizeof(void*),0)));}
if(_tmp684 != 0){const char*_tmpEF5;void*_tmpEF4;(_tmpEF4=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpEF5="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpEF5,sizeof(char),38))),_tag_dyneither(_tmpEF4,sizeof(void*),0)));}{
struct _tuple2*_tmpEF8;union Cyc_YYSTYPE_union _tmpEF7;yyval=(union Cyc_YYSTYPE_union)(((
_tmpEF7.YY37).tag=41,(((_tmpEF7.YY37).f1=((_tmpEF8=_cycalloc(sizeof(*_tmpEF8)),((
_tmpEF8->f1=0,((_tmpEF8->f2=_tmp681,((_tmpEF8->f3=_tmp682,_tmpEF8)))))))),
_tmpEF7))));}break;}}case 226: _LL2E7:{union Cyc_YYSTYPE_union _tmpEF9;yyval=(union
Cyc_YYSTYPE_union)(((_tmpEF9.YY36).tag=40,(((_tmpEF9.YY36).f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpEF9))));}break;case 227:
_LL2E8:{struct Cyc_List_List*_tmpEFF;struct _dyneither_ptr*_tmpEFE;union Cyc_YYSTYPE_union
_tmpEFD;yyval=(union Cyc_YYSTYPE_union)(((_tmpEFD.YY36).tag=40,(((_tmpEFD.YY36).f1=((
_tmpEFF=_cycalloc(sizeof(*_tmpEFF)),((_tmpEFF->hd=((_tmpEFE=_cycalloc(sizeof(*
_tmpEFE)),((_tmpEFE[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),_tmpEFE)))),((_tmpEFF->tl=0,_tmpEFF)))))),_tmpEFD))));}
break;case 228: _LL2E9:{struct Cyc_List_List*_tmpF05;struct _dyneither_ptr*_tmpF04;
union Cyc_YYSTYPE_union _tmpF03;yyval=(union Cyc_YYSTYPE_union)(((_tmpF03.YY36).tag=
40,(((_tmpF03.YY36).f1=((_tmpF05=_cycalloc(sizeof(*_tmpF05)),((_tmpF05->hd=((
_tmpF04=_cycalloc(sizeof(*_tmpF04)),((_tmpF04[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF04)))),((_tmpF05->tl=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpF05)))))),_tmpF03))));}break;case 229: _LL2EA: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 230: _LL2EB: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 231: _LL2EC:{
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpF0B;struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpF0A;union Cyc_YYSTYPE_union _tmpF09;yyval=(union Cyc_YYSTYPE_union)(((_tmpF09.YY3).tag=
7,(((_tmpF09.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmpF0B=_cycalloc(sizeof(*
_tmpF0B)),((_tmpF0B[0]=((_tmpF0A.tag=37,((_tmpF0A.f1=0,((_tmpF0A.f2=0,_tmpF0A)))))),
_tmpF0B)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpF09))));}break;case 232: _LL2ED:{struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpF11;struct Cyc_Absyn_UnresolvedMem_e_struct _tmpF10;union Cyc_YYSTYPE_union
_tmpF0F;yyval=(union Cyc_YYSTYPE_union)(((_tmpF0F.YY3).tag=7,(((_tmpF0F.YY3).f1=
Cyc_Absyn_new_exp((void*)((_tmpF11=_cycalloc(sizeof(*_tmpF11)),((_tmpF11[0]=((
_tmpF10.tag=37,((_tmpF10.f1=0,((_tmpF10.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])),_tmpF10)))))),_tmpF11)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpF0F))));}
break;case 233: _LL2EE:{struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpF17;struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpF16;union Cyc_YYSTYPE_union _tmpF15;yyval=(union Cyc_YYSTYPE_union)(((_tmpF15.YY3).tag=
7,(((_tmpF15.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmpF17=_cycalloc(sizeof(*
_tmpF17)),((_tmpF17[0]=((_tmpF16.tag=37,((_tmpF16.f1=0,((_tmpF16.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),_tmpF16)))))),_tmpF17)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpF15))));}break;case 234: _LL2EF: {union Cyc_Absyn_Nmspace_union
_tmpF1C;struct _dyneither_ptr*_tmpF1B;struct _tuple1*_tmpF1A;struct Cyc_Absyn_Vardecl*
_tmp69B=Cyc_Absyn_new_vardecl(((_tmpF1A=_cycalloc(sizeof(*_tmpF1A)),((_tmpF1A->f1=(
union Cyc_Absyn_Nmspace_union)(((_tmpF1C.Loc_n).tag=0,_tmpF1C)),((_tmpF1A->f2=((
_tmpF1B=_cycalloc(sizeof(*_tmpF1B)),((_tmpF1B[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpF1B)))),_tmpF1A)))))),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp69B->tq).real_const=
1;{struct Cyc_Absyn_Comprehension_e_struct*_tmpF22;struct Cyc_Absyn_Comprehension_e_struct
_tmpF21;union Cyc_YYSTYPE_union _tmpF20;yyval=(union Cyc_YYSTYPE_union)(((_tmpF20.YY3).tag=
7,(((_tmpF20.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmpF22=_cycalloc(sizeof(*
_tmpF22)),((_tmpF22[0]=((_tmpF21.tag=29,((_tmpF21.f1=_tmp69B,((_tmpF21.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpF21.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpF21.f4=0,
_tmpF21)))))))))),_tmpF22)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpF20))));}
break;}case 235: _LL2F0:{struct Cyc_List_List*_tmpF28;struct _tuple21*_tmpF27;union
Cyc_YYSTYPE_union _tmpF26;yyval=(union Cyc_YYSTYPE_union)(((_tmpF26.YY6).tag=10,(((
_tmpF26.YY6).f1=((_tmpF28=_cycalloc(sizeof(*_tmpF28)),((_tmpF28->hd=((_tmpF27=
_cycalloc(sizeof(*_tmpF27)),((_tmpF27->f1=0,((_tmpF27->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF27)))))),((_tmpF28->tl=
0,_tmpF28)))))),_tmpF26))));}break;case 236: _LL2F1:{struct Cyc_List_List*_tmpF2E;
struct _tuple21*_tmpF2D;union Cyc_YYSTYPE_union _tmpF2C;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF2C.YY6).tag=10,(((_tmpF2C.YY6).f1=((_tmpF2E=_cycalloc(sizeof(*_tmpF2E)),((
_tmpF2E->hd=((_tmpF2D=_cycalloc(sizeof(*_tmpF2D)),((_tmpF2D->f1=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpF2D->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF2D)))))),((
_tmpF2E->tl=0,_tmpF2E)))))),_tmpF2C))));}break;case 237: _LL2F2:{struct Cyc_List_List*
_tmpF34;struct _tuple21*_tmpF33;union Cyc_YYSTYPE_union _tmpF32;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF32.YY6).tag=10,(((_tmpF32.YY6).f1=((_tmpF34=_cycalloc(sizeof(*_tmpF34)),((
_tmpF34->hd=((_tmpF33=_cycalloc(sizeof(*_tmpF33)),((_tmpF33->f1=0,((_tmpF33->f2=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF33)))))),((
_tmpF34->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]),_tmpF34)))))),_tmpF32))));}break;case 238: _LL2F3:{struct Cyc_List_List*
_tmpF3A;struct _tuple21*_tmpF39;union Cyc_YYSTYPE_union _tmpF38;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF38.YY6).tag=10,(((_tmpF38.YY6).f1=((_tmpF3A=_cycalloc(sizeof(*_tmpF3A)),((
_tmpF3A->hd=((_tmpF39=_cycalloc(sizeof(*_tmpF39)),((_tmpF39->f1=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpF39->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF39)))))),((
_tmpF3A->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]),_tmpF3A)))))),_tmpF38))));}break;case 239: _LL2F4:{union Cyc_YYSTYPE_union
_tmpF3B;yyval=(union Cyc_YYSTYPE_union)(((_tmpF3B.YY41).tag=45,(((_tmpF3B.YY41).f1=
Cyc_List_imp_rev(Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])),_tmpF3B))));}break;case 240: _LL2F5:{struct Cyc_List_List*
_tmpF3E;union Cyc_YYSTYPE_union _tmpF3D;yyval=(union Cyc_YYSTYPE_union)(((_tmpF3D.YY41).tag=
45,(((_tmpF3D.YY41).f1=((_tmpF3E=_cycalloc(sizeof(*_tmpF3E)),((_tmpF3E->hd=(void*)
Cyc_yyget_YY42(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpF3E->tl=0,_tmpF3E)))))),_tmpF3D))));}break;case 241: _LL2F6:{struct Cyc_List_List*
_tmpF41;union Cyc_YYSTYPE_union _tmpF40;yyval=(union Cyc_YYSTYPE_union)(((_tmpF40.YY41).tag=
45,(((_tmpF40.YY41).f1=((_tmpF41=_cycalloc(sizeof(*_tmpF41)),((_tmpF41->hd=(void*)
Cyc_yyget_YY42(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpF41->tl=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpF41)))))),_tmpF40))));}break;case 242: _LL2F7:{struct Cyc_Absyn_ArrayElement_struct*
_tmpF47;struct Cyc_Absyn_ArrayElement_struct _tmpF46;union Cyc_YYSTYPE_union _tmpF45;
yyval=(union Cyc_YYSTYPE_union)(((_tmpF45.YY42).tag=46,(((_tmpF45.YY42).f1=(void*)((
void*)((_tmpF47=_cycalloc(sizeof(*_tmpF47)),((_tmpF47[0]=((_tmpF46.tag=0,((
_tmpF46.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]),_tmpF46)))),_tmpF47))))),_tmpF45))));}break;case 243: _LL2F8:{struct Cyc_Absyn_FieldName_struct*
_tmpF51;struct _dyneither_ptr*_tmpF50;struct Cyc_Absyn_FieldName_struct _tmpF4F;
union Cyc_YYSTYPE_union _tmpF4E;yyval=(union Cyc_YYSTYPE_union)(((_tmpF4E.YY42).tag=
46,(((_tmpF4E.YY42).f1=(void*)((void*)((_tmpF51=_cycalloc(sizeof(*_tmpF51)),((
_tmpF51[0]=((_tmpF4F.tag=1,((_tmpF4F.f1=((_tmpF50=_cycalloc(sizeof(*_tmpF50)),((
_tmpF50[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpF50)))),_tmpF4F)))),_tmpF51))))),_tmpF4E))));}break;case 244:
_LL2F9: {struct _tuple16 _tmp6BB;struct Cyc_Absyn_Tqual _tmp6BC;struct Cyc_List_List*
_tmp6BD;struct Cyc_List_List*_tmp6BE;struct _tuple16*_tmp6BA=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6BB=*_tmp6BA;_tmp6BC=
_tmp6BB.f1;_tmp6BD=_tmp6BB.f2;_tmp6BE=_tmp6BB.f3;{void*_tmp6BF=Cyc_Parse_speclist2typ(
_tmp6BD,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp6BE != 0){const char*_tmpF54;void*_tmpF53;(
_tmpF53=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpF54="ignoring attributes in type",
_tag_dyneither(_tmpF54,sizeof(char),28))),_tag_dyneither(_tmpF53,sizeof(void*),0)));}{
struct _tuple2*_tmpF57;union Cyc_YYSTYPE_union _tmpF56;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF56.YY37).tag=41,(((_tmpF56.YY37).f1=((_tmpF57=_cycalloc(sizeof(*_tmpF57)),((
_tmpF57->f1=0,((_tmpF57->f2=_tmp6BC,((_tmpF57->f3=_tmp6BF,_tmpF57)))))))),
_tmpF56))));}break;}}case 245: _LL2FA: {struct _tuple16 _tmp6C5;struct Cyc_Absyn_Tqual
_tmp6C6;struct Cyc_List_List*_tmp6C7;struct Cyc_List_List*_tmp6C8;struct _tuple16*
_tmp6C4=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp6C5=*_tmp6C4;_tmp6C6=_tmp6C5.f1;_tmp6C7=_tmp6C5.f2;_tmp6C8=_tmp6C5.f3;{void*
_tmp6C9=Cyc_Parse_speclist2typ(_tmp6C7,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6CA=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct _tuple6 _tmp6CB=Cyc_Parse_apply_tms(_tmp6C6,_tmp6C9,_tmp6C8,_tmp6CA);if(
_tmp6CB.f3 != 0){const char*_tmpF5A;void*_tmpF59;(_tmpF59=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpF5A="bad type params, ignoring",
_tag_dyneither(_tmpF5A,sizeof(char),26))),_tag_dyneither(_tmpF59,sizeof(void*),0)));}
if(_tmp6CB.f4 != 0){const char*_tmpF5D;void*_tmpF5C;(_tmpF5C=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpF5D="bad specifiers, ignoring",
_tag_dyneither(_tmpF5D,sizeof(char),25))),_tag_dyneither(_tmpF5C,sizeof(void*),0)));}{
struct _tuple2*_tmpF60;union Cyc_YYSTYPE_union _tmpF5F;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF5F.YY37).tag=41,(((_tmpF5F.YY37).f1=((_tmpF60=_cycalloc(sizeof(*_tmpF60)),((
_tmpF60->f1=0,((_tmpF60->f2=_tmp6CB.f1,((_tmpF60->f3=_tmp6CB.f2,_tmpF60)))))))),
_tmpF5F))));}break;}}case 246: _LL2FB:{union Cyc_YYSTYPE_union _tmpF61;yyval=(union
Cyc_YYSTYPE_union)(((_tmpF61.YY44).tag=48,(((_tmpF61.YY44).f1=(void*)(*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,_tmpF61))));}
break;case 247: _LL2FC:{struct Cyc_Absyn_JoinEff_struct*_tmpF67;struct Cyc_Absyn_JoinEff_struct
_tmpF66;union Cyc_YYSTYPE_union _tmpF65;yyval=(union Cyc_YYSTYPE_union)(((_tmpF65.YY44).tag=
48,(((_tmpF65.YY44).f1=(void*)((void*)((_tmpF67=_cycalloc(sizeof(*_tmpF67)),((
_tmpF67[0]=((_tmpF66.tag=20,((_tmpF66.f1=0,_tmpF66)))),_tmpF67))))),_tmpF65))));}
break;case 248: _LL2FD:{struct Cyc_Absyn_JoinEff_struct*_tmpF6D;struct Cyc_Absyn_JoinEff_struct
_tmpF6C;union Cyc_YYSTYPE_union _tmpF6B;yyval=(union Cyc_YYSTYPE_union)(((_tmpF6B.YY44).tag=
48,(((_tmpF6B.YY44).f1=(void*)((void*)((_tmpF6D=_cycalloc(sizeof(*_tmpF6D)),((
_tmpF6D[0]=((_tmpF6C.tag=20,((_tmpF6C.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpF6C)))),_tmpF6D))))),
_tmpF6B))));}break;case 249: _LL2FE:{struct Cyc_Absyn_RgnsEff_struct*_tmpF73;struct
Cyc_Absyn_RgnsEff_struct _tmpF72;union Cyc_YYSTYPE_union _tmpF71;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF71.YY44).tag=48,(((_tmpF71.YY44).f1=(void*)((void*)((_tmpF73=_cycalloc(
sizeof(*_tmpF73)),((_tmpF73[0]=((_tmpF72.tag=21,((_tmpF72.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpF72)))),_tmpF73))))),
_tmpF71))));}break;case 250: _LL2FF:{struct Cyc_Absyn_JoinEff_struct*_tmpF7D;struct
Cyc_List_List*_tmpF7C;struct Cyc_Absyn_JoinEff_struct _tmpF7B;union Cyc_YYSTYPE_union
_tmpF7A;yyval=(union Cyc_YYSTYPE_union)(((_tmpF7A.YY44).tag=48,(((_tmpF7A.YY44).f1=(
void*)((void*)((_tmpF7D=_cycalloc(sizeof(*_tmpF7D)),((_tmpF7D[0]=((_tmpF7B.tag=
20,((_tmpF7B.f1=((_tmpF7C=_cycalloc(sizeof(*_tmpF7C)),((_tmpF7C->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpF7C->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF7C)))))),_tmpF7B)))),
_tmpF7D))))),_tmpF7A))));}break;case 251: _LL300:{struct Cyc_List_List*_tmpF80;
union Cyc_YYSTYPE_union _tmpF7F;yyval=(union Cyc_YYSTYPE_union)(((_tmpF7F.YY40).tag=
44,(((_tmpF7F.YY40).f1=((_tmpF80=_cycalloc(sizeof(*_tmpF80)),((_tmpF80->hd=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpF80->tl=0,_tmpF80)))))),_tmpF7F))));}break;case 252: _LL301:{struct Cyc_List_List*
_tmpF83;union Cyc_YYSTYPE_union _tmpF82;yyval=(union Cyc_YYSTYPE_union)(((_tmpF82.YY40).tag=
44,(((_tmpF82.YY40).f1=((_tmpF83=_cycalloc(sizeof(*_tmpF83)),((_tmpF83->hd=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpF83->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),_tmpF83)))))),_tmpF82))));}break;case 253: _LL302:{struct Cyc_Parse_Abstractdeclarator*
_tmpF86;union Cyc_YYSTYPE_union _tmpF85;yyval=(union Cyc_YYSTYPE_union)(((_tmpF85.YY30).tag=
34,(((_tmpF85.YY30).f1=((_tmpF86=_cycalloc(sizeof(*_tmpF86)),((_tmpF86->tms=Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF86)))),_tmpF85))));}
break;case 254: _LL303: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 255: _LL304:{struct Cyc_Parse_Abstractdeclarator*_tmpF89;
union Cyc_YYSTYPE_union _tmpF88;yyval=(union Cyc_YYSTYPE_union)(((_tmpF88.YY30).tag=
34,(((_tmpF88.YY30).f1=((_tmpF89=_cycalloc(sizeof(*_tmpF89)),((_tmpF89->tms=Cyc_List_imp_append(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpF89)))),
_tmpF88))));}break;case 256: _LL305: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 257: _LL306:{struct Cyc_Parse_Abstractdeclarator*
_tmpF98;struct Cyc_Absyn_Carray_mod_struct*_tmpF97;struct Cyc_Absyn_Carray_mod_struct
_tmpF96;struct Cyc_List_List*_tmpF95;union Cyc_YYSTYPE_union _tmpF94;yyval=(union
Cyc_YYSTYPE_union)(((_tmpF94.YY30).tag=34,(((_tmpF94.YY30).f1=((_tmpF98=
_cycalloc(sizeof(*_tmpF98)),((_tmpF98->tms=((_tmpF95=_cycalloc(sizeof(*_tmpF95)),((
_tmpF95->hd=(void*)((void*)((_tmpF97=_cycalloc(sizeof(*_tmpF97)),((_tmpF97[0]=((
_tmpF96.tag=0,((_tmpF96.f1=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpF96.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpF96)))))),
_tmpF97))))),((_tmpF95->tl=0,_tmpF95)))))),_tmpF98)))),_tmpF94))));}break;case
258: _LL307:{struct Cyc_Parse_Abstractdeclarator*_tmpFA7;struct Cyc_Absyn_Carray_mod_struct*
_tmpFA6;struct Cyc_Absyn_Carray_mod_struct _tmpFA5;struct Cyc_List_List*_tmpFA4;
union Cyc_YYSTYPE_union _tmpFA3;yyval=(union Cyc_YYSTYPE_union)(((_tmpFA3.YY30).tag=
34,(((_tmpFA3.YY30).f1=((_tmpFA7=_cycalloc(sizeof(*_tmpFA7)),((_tmpFA7->tms=((
_tmpFA4=_cycalloc(sizeof(*_tmpFA4)),((_tmpFA4->hd=(void*)((void*)((_tmpFA6=
_cycalloc(sizeof(*_tmpFA6)),((_tmpFA6[0]=((_tmpFA5.tag=0,((_tmpFA5.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpFA5.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpFA5)))))),
_tmpFA6))))),((_tmpFA4->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpFA4)))))),_tmpFA7)))),_tmpFA3))));}break;case
259: _LL308:{struct Cyc_Parse_Abstractdeclarator*_tmpFB6;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpFB5;struct Cyc_Absyn_ConstArray_mod_struct _tmpFB4;struct Cyc_List_List*_tmpFB3;
union Cyc_YYSTYPE_union _tmpFB2;yyval=(union Cyc_YYSTYPE_union)(((_tmpFB2.YY30).tag=
34,(((_tmpFB2.YY30).f1=((_tmpFB6=_cycalloc(sizeof(*_tmpFB6)),((_tmpFB6->tms=((
_tmpFB3=_cycalloc(sizeof(*_tmpFB3)),((_tmpFB3->hd=(void*)((void*)((_tmpFB5=
_cycalloc(sizeof(*_tmpFB5)),((_tmpFB5[0]=((_tmpFB4.tag=1,((_tmpFB4.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpFB4.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpFB4.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpFB4)))))))),
_tmpFB5))))),((_tmpFB3->tl=0,_tmpFB3)))))),_tmpFB6)))),_tmpFB2))));}break;case
260: _LL309:{struct Cyc_Parse_Abstractdeclarator*_tmpFC5;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpFC4;struct Cyc_Absyn_ConstArray_mod_struct _tmpFC3;struct Cyc_List_List*_tmpFC2;
union Cyc_YYSTYPE_union _tmpFC1;yyval=(union Cyc_YYSTYPE_union)(((_tmpFC1.YY30).tag=
34,(((_tmpFC1.YY30).f1=((_tmpFC5=_cycalloc(sizeof(*_tmpFC5)),((_tmpFC5->tms=((
_tmpFC2=_cycalloc(sizeof(*_tmpFC2)),((_tmpFC2->hd=(void*)((void*)((_tmpFC4=
_cycalloc(sizeof(*_tmpFC4)),((_tmpFC4[0]=((_tmpFC3.tag=1,((_tmpFC3.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpFC3.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpFC3.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpFC3)))))))),
_tmpFC4))))),((_tmpFC2->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpFC2)))))),_tmpFC5)))),_tmpFC1))));}break;case
261: _LL30A:{struct Cyc_Parse_Abstractdeclarator*_tmpFE1;struct Cyc_Absyn_Function_mod_struct*
_tmpFE0;struct Cyc_Absyn_WithTypes_struct*_tmpFDF;struct Cyc_Absyn_WithTypes_struct
_tmpFDE;struct Cyc_Absyn_Function_mod_struct _tmpFDD;struct Cyc_List_List*_tmpFDC;
union Cyc_YYSTYPE_union _tmpFDB;yyval=(union Cyc_YYSTYPE_union)(((_tmpFDB.YY30).tag=
34,(((_tmpFDB.YY30).f1=((_tmpFE1=_cycalloc(sizeof(*_tmpFE1)),((_tmpFE1->tms=((
_tmpFDC=_cycalloc(sizeof(*_tmpFDC)),((_tmpFDC->hd=(void*)((void*)((_tmpFE0=
_cycalloc(sizeof(*_tmpFE0)),((_tmpFE0[0]=((_tmpFDD.tag=3,((_tmpFDD.f1=(void*)((
void*)((_tmpFDF=_cycalloc(sizeof(*_tmpFDF)),((_tmpFDF[0]=((_tmpFDE.tag=1,((
_tmpFDE.f1=0,((_tmpFDE.f2=0,((_tmpFDE.f3=0,((_tmpFDE.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpFDE.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpFDE)))))))))))),
_tmpFDF))))),_tmpFDD)))),_tmpFE0))))),((_tmpFDC->tl=0,_tmpFDC)))))),_tmpFE1)))),
_tmpFDB))));}break;case 262: _LL30B: {struct _tuple17 _tmp704;struct Cyc_List_List*
_tmp705;int _tmp706;struct Cyc_Absyn_VarargInfo*_tmp707;struct Cyc_Core_Opt*_tmp708;
struct Cyc_List_List*_tmp709;struct _tuple17*_tmp703=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp704=*_tmp703;_tmp705=
_tmp704.f1;_tmp706=_tmp704.f2;_tmp707=_tmp704.f3;_tmp708=_tmp704.f4;_tmp709=
_tmp704.f5;{struct Cyc_Parse_Abstractdeclarator*_tmpFFD;struct Cyc_Absyn_Function_mod_struct*
_tmpFFC;struct Cyc_Absyn_WithTypes_struct*_tmpFFB;struct Cyc_Absyn_WithTypes_struct
_tmpFFA;struct Cyc_Absyn_Function_mod_struct _tmpFF9;struct Cyc_List_List*_tmpFF8;
union Cyc_YYSTYPE_union _tmpFF7;yyval=(union Cyc_YYSTYPE_union)(((_tmpFF7.YY30).tag=
34,(((_tmpFF7.YY30).f1=((_tmpFFD=_cycalloc(sizeof(*_tmpFFD)),((_tmpFFD->tms=((
_tmpFF8=_cycalloc(sizeof(*_tmpFF8)),((_tmpFF8->hd=(void*)((void*)((_tmpFFC=
_cycalloc(sizeof(*_tmpFFC)),((_tmpFFC[0]=((_tmpFF9.tag=3,((_tmpFF9.f1=(void*)((
void*)((_tmpFFB=_cycalloc(sizeof(*_tmpFFB)),((_tmpFFB[0]=((_tmpFFA.tag=1,((
_tmpFFA.f1=_tmp705,((_tmpFFA.f2=_tmp706,((_tmpFFA.f3=_tmp707,((_tmpFFA.f4=
_tmp708,((_tmpFFA.f5=_tmp709,_tmpFFA)))))))))))),_tmpFFB))))),_tmpFF9)))),
_tmpFFC))))),((_tmpFF8->tl=0,_tmpFF8)))))),_tmpFFD)))),_tmpFF7))));}break;}case
263: _LL30C:{struct Cyc_Parse_Abstractdeclarator*_tmp1019;struct Cyc_Absyn_Function_mod_struct*
_tmp1018;struct Cyc_Absyn_WithTypes_struct*_tmp1017;struct Cyc_Absyn_WithTypes_struct
_tmp1016;struct Cyc_Absyn_Function_mod_struct _tmp1015;struct Cyc_List_List*
_tmp1014;union Cyc_YYSTYPE_union _tmp1013;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1013.YY30).tag=34,(((_tmp1013.YY30).f1=((_tmp1019=_cycalloc(sizeof(*_tmp1019)),((
_tmp1019->tms=((_tmp1014=_cycalloc(sizeof(*_tmp1014)),((_tmp1014->hd=(void*)((
void*)((_tmp1018=_cycalloc(sizeof(*_tmp1018)),((_tmp1018[0]=((_tmp1015.tag=3,((
_tmp1015.f1=(void*)((void*)((_tmp1017=_cycalloc(sizeof(*_tmp1017)),((_tmp1017[0]=((
_tmp1016.tag=1,((_tmp1016.f1=0,((_tmp1016.f2=0,((_tmp1016.f3=0,((_tmp1016.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp1016.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmp1016)))))))))))),
_tmp1017))))),_tmp1015)))),_tmp1018))))),((_tmp1014->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmp1014)))))),
_tmp1019)))),_tmp1013))));}break;case 264: _LL30D: {struct _tuple17 _tmp719;struct
Cyc_List_List*_tmp71A;int _tmp71B;struct Cyc_Absyn_VarargInfo*_tmp71C;struct Cyc_Core_Opt*
_tmp71D;struct Cyc_List_List*_tmp71E;struct _tuple17*_tmp718=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp719=*_tmp718;_tmp71A=
_tmp719.f1;_tmp71B=_tmp719.f2;_tmp71C=_tmp719.f3;_tmp71D=_tmp719.f4;_tmp71E=
_tmp719.f5;{struct Cyc_Parse_Abstractdeclarator*_tmp1035;struct Cyc_Absyn_Function_mod_struct*
_tmp1034;struct Cyc_Absyn_WithTypes_struct*_tmp1033;struct Cyc_Absyn_WithTypes_struct
_tmp1032;struct Cyc_Absyn_Function_mod_struct _tmp1031;struct Cyc_List_List*
_tmp1030;union Cyc_YYSTYPE_union _tmp102F;yyval=(union Cyc_YYSTYPE_union)(((
_tmp102F.YY30).tag=34,(((_tmp102F.YY30).f1=((_tmp1035=_cycalloc(sizeof(*_tmp1035)),((
_tmp1035->tms=((_tmp1030=_cycalloc(sizeof(*_tmp1030)),((_tmp1030->hd=(void*)((
void*)((_tmp1034=_cycalloc(sizeof(*_tmp1034)),((_tmp1034[0]=((_tmp1031.tag=3,((
_tmp1031.f1=(void*)((void*)((_tmp1033=_cycalloc(sizeof(*_tmp1033)),((_tmp1033[0]=((
_tmp1032.tag=1,((_tmp1032.f1=_tmp71A,((_tmp1032.f2=_tmp71B,((_tmp1032.f3=_tmp71C,((
_tmp1032.f4=_tmp71D,((_tmp1032.f5=_tmp71E,_tmp1032)))))))))))),_tmp1033))))),
_tmp1031)))),_tmp1034))))),((_tmp1030->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmp1030)))))),
_tmp1035)))),_tmp102F))));}break;}case 265: _LL30E: {struct Cyc_List_List*_tmp726=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Parse_Abstractdeclarator*
_tmp1044;struct Cyc_Absyn_TypeParams_mod_struct*_tmp1043;struct Cyc_Absyn_TypeParams_mod_struct
_tmp1042;struct Cyc_List_List*_tmp1041;union Cyc_YYSTYPE_union _tmp1040;yyval=(
union Cyc_YYSTYPE_union)(((_tmp1040.YY30).tag=34,(((_tmp1040.YY30).f1=((_tmp1044=
_cycalloc(sizeof(*_tmp1044)),((_tmp1044->tms=((_tmp1041=_cycalloc(sizeof(*
_tmp1041)),((_tmp1041->hd=(void*)((void*)((_tmp1043=_cycalloc(sizeof(*_tmp1043)),((
_tmp1043[0]=((_tmp1042.tag=4,((_tmp1042.f1=_tmp726,((_tmp1042.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmp1042.f3=0,
_tmp1042)))))))),_tmp1043))))),((_tmp1041->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmp1041)))))),
_tmp1044)))),_tmp1040))));}break;}case 266: _LL30F:{struct Cyc_Parse_Abstractdeclarator*
_tmp1053;struct Cyc_Absyn_Attributes_mod_struct*_tmp1052;struct Cyc_Absyn_Attributes_mod_struct
_tmp1051;struct Cyc_List_List*_tmp1050;union Cyc_YYSTYPE_union _tmp104F;yyval=(
union Cyc_YYSTYPE_union)(((_tmp104F.YY30).tag=34,(((_tmp104F.YY30).f1=((_tmp1053=
_cycalloc(sizeof(*_tmp1053)),((_tmp1053->tms=((_tmp1050=_cycalloc(sizeof(*
_tmp1050)),((_tmp1050->hd=(void*)((void*)((_tmp1052=_cycalloc(sizeof(*_tmp1052)),((
_tmp1052[0]=((_tmp1051.tag=5,((_tmp1051.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmp1051.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmp1051)))))),_tmp1052))))),((_tmp1050->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms,_tmp1050)))))),
_tmp1053)))),_tmp104F))));}break;case 267: _LL310: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 268: _LL311: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 269: _LL312:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 270:
_LL313: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
271: _LL314: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 272: _LL315: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 273: _LL316:{struct Cyc_Absyn_ResetRegion_s_struct*_tmp1059;struct Cyc_Absyn_ResetRegion_s_struct
_tmp1058;union Cyc_YYSTYPE_union _tmp1057;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1057.YY9).tag=13,(((_tmp1057.YY9).f1=Cyc_Absyn_new_stmt((void*)((_tmp1059=
_cycalloc(sizeof(*_tmp1059)),((_tmp1059[0]=((_tmp1058.tag=15,((_tmp1058.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp1058)))),
_tmp1059)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1057))));}break;case 274: _LL317:{union Cyc_YYSTYPE_union
_tmp105A;yyval=(union Cyc_YYSTYPE_union)(((_tmp105A.YY4).tag=8,(((_tmp105A.YY4).f1=
0,_tmp105A))));}break;case 275: _LL318:{const char*_tmp105B;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmp105B="open",_tag_dyneither(_tmp105B,sizeof(char),5))))
!= 0){const char*_tmp105C;Cyc_Parse_err(((_tmp105C="expecting `open'",
_tag_dyneither(_tmp105C,sizeof(char),17))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}{union Cyc_YYSTYPE_union
_tmp105D;yyval=(union Cyc_YYSTYPE_union)(((_tmp105D.YY4).tag=8,(((_tmp105D.YY4).f1=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmp105D))));}break;case 276: _LL319:{struct Cyc_Absyn_Label_s_struct*
_tmp1067;struct _dyneither_ptr*_tmp1066;struct Cyc_Absyn_Label_s_struct _tmp1065;
union Cyc_YYSTYPE_union _tmp1064;yyval=(union Cyc_YYSTYPE_union)(((_tmp1064.YY9).tag=
13,(((_tmp1064.YY9).f1=Cyc_Absyn_new_stmt((void*)((_tmp1067=_cycalloc(sizeof(*
_tmp1067)),((_tmp1067[0]=((_tmp1065.tag=12,((_tmp1065.f1=((_tmp1066=_cycalloc(
sizeof(*_tmp1066)),((_tmp1066[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp1066)))),((_tmp1065.f2=
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1065)))))),
_tmp1067)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1064))));}break;case 277: _LL31A:{union Cyc_YYSTYPE_union
_tmp1068;yyval=(union Cyc_YYSTYPE_union)(((_tmp1068.YY9).tag=13,(((_tmp1068.YY9).f1=
Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1068))));}
break;case 278: _LL31B:{union Cyc_YYSTYPE_union _tmp1069;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1069.YY9).tag=13,(((_tmp1069.YY9).f1=Cyc_Absyn_exp_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1069))));}
break;case 279: _LL31C:{union Cyc_YYSTYPE_union _tmp106A;yyval=(union Cyc_YYSTYPE_union)(((
_tmp106A.YY9).tag=13,(((_tmp106A.YY9).f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp106A))));}
break;case 280: _LL31D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 281: _LL31E:{union Cyc_YYSTYPE_union _tmp106B;yyval=(
union Cyc_YYSTYPE_union)(((_tmp106B.YY9).tag=13,(((_tmp106B.YY9).f1=Cyc_Parse_flatten_declarations(
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(
0)),_tmp106B))));}break;case 282: _LL31F:{union Cyc_YYSTYPE_union _tmp106C;yyval=(
union Cyc_YYSTYPE_union)(((_tmp106C.YY9).tag=13,(((_tmp106C.YY9).f1=Cyc_Parse_flatten_declarations(
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmp106C))));}break;
case 283: _LL320: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 284: _LL321:{union Cyc_YYSTYPE_union _tmp106D;yyval=(union Cyc_YYSTYPE_union)(((
_tmp106D.YY9).tag=13,(((_tmp106D.YY9).f1=Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp106D))));}
break;case 285: _LL322:{struct Cyc_Absyn_Fn_d_struct*_tmp1073;struct Cyc_Absyn_Fn_d_struct
_tmp1072;union Cyc_YYSTYPE_union _tmp1071;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1071.YY9).tag=13,(((_tmp1071.YY9).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)((_tmp1073=_cycalloc(sizeof(*_tmp1073)),((_tmp1073[0]=((_tmp1072.tag=1,((
_tmp1072.f1=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp1072)))),_tmp1073)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(
0)),_tmp1071))));}break;case 286: _LL323:{struct Cyc_Absyn_Fn_d_struct*_tmp1079;
struct Cyc_Absyn_Fn_d_struct _tmp1078;union Cyc_YYSTYPE_union _tmp1077;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1077.YY9).tag=13,(((_tmp1077.YY9).f1=Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl((void*)((_tmp1079=_cycalloc(sizeof(*_tmp1079)),((_tmp1079[0]=((
_tmp1078.tag=1,((_tmp1078.f1=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmp1078)))),_tmp1079)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmp1077))));}break;
case 287: _LL324:{union Cyc_YYSTYPE_union _tmp107A;yyval=(union Cyc_YYSTYPE_union)(((
_tmp107A.YY9).tag=13,(((_tmp107A.YY9).f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp107A))));}
break;case 288: _LL325:{union Cyc_YYSTYPE_union _tmp107B;yyval=(union Cyc_YYSTYPE_union)(((
_tmp107B.YY9).tag=13,(((_tmp107B.YY9).f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp107B))));}
break;case 289: _LL326:{union Cyc_YYSTYPE_union _tmp107C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp107C.YY9).tag=13,(((_tmp107C.YY9).f1=Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp107C))));}
break;case 290: _LL327: {struct Cyc_Absyn_UnknownId_e_struct _tmp107F;struct Cyc_Absyn_UnknownId_e_struct*
_tmp107E;struct Cyc_Absyn_Exp*_tmp74B=Cyc_Absyn_new_exp((void*)((_tmp107E=
_cycalloc(sizeof(*_tmp107E)),((_tmp107E[0]=((_tmp107F.tag=2,((_tmp107F.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp107F)))),
_tmp107E)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));{union Cyc_YYSTYPE_union _tmp1080;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1080.YY9).tag=13,(((_tmp1080.YY9).f1=Cyc_Absyn_switch_stmt(_tmp74B,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1080))));}
break;}case 291: _LL328: {struct Cyc_Absyn_Exp*_tmp74F=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{union Cyc_YYSTYPE_union
_tmp1081;yyval=(union Cyc_YYSTYPE_union)(((_tmp1081.YY9).tag=13,(((_tmp1081.YY9).f1=
Cyc_Absyn_switch_stmt(_tmp74F,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1081))));}
break;}case 292: _LL329:{union Cyc_YYSTYPE_union _tmp1082;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1082.YY9).tag=13,(((_tmp1082.YY9).f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1082))));}
break;case 293: _LL32A:{union Cyc_YYSTYPE_union _tmp1083;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1083.YY10).tag=14,(((_tmp1083.YY10).f1=0,_tmp1083))));}break;case 294: _LL32B:{
struct Cyc_List_List*_tmp1089;struct Cyc_Absyn_Switch_clause*_tmp1088;union Cyc_YYSTYPE_union
_tmp1087;yyval=(union Cyc_YYSTYPE_union)(((_tmp1087.YY10).tag=14,(((_tmp1087.YY10).f1=((
_tmp1089=_cycalloc(sizeof(*_tmp1089)),((_tmp1089->hd=((_tmp1088=_cycalloc(
sizeof(*_tmp1088)),((_tmp1088->pattern=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)),((_tmp1088->pat_vars=
0,((_tmp1088->where_clause=0,((_tmp1088->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp1088->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmp1088)))))))))))),((
_tmp1089->tl=0,_tmp1089)))))),_tmp1087))));}break;case 295: _LL32C:{struct Cyc_List_List*
_tmp108F;struct Cyc_Absyn_Switch_clause*_tmp108E;union Cyc_YYSTYPE_union _tmp108D;
yyval=(union Cyc_YYSTYPE_union)(((_tmp108D.YY10).tag=14,(((_tmp108D.YY10).f1=((
_tmp108F=_cycalloc(sizeof(*_tmp108F)),((_tmp108F->hd=((_tmp108E=_cycalloc(
sizeof(*_tmp108E)),((_tmp108E->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp108E->pat_vars=0,((
_tmp108E->where_clause=0,((_tmp108E->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),((_tmp108E->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmp108E)))))))))))),((_tmp108F->tl=Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp108F)))))),
_tmp108D))));}break;case 296: _LL32D:{struct Cyc_List_List*_tmp1095;struct Cyc_Absyn_Switch_clause*
_tmp1094;union Cyc_YYSTYPE_union _tmp1093;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1093.YY10).tag=14,(((_tmp1093.YY10).f1=((_tmp1095=_cycalloc(sizeof(*_tmp1095)),((
_tmp1095->hd=((_tmp1094=_cycalloc(sizeof(*_tmp1094)),((_tmp1094->pattern=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp1094->pat_vars=
0,((_tmp1094->where_clause=0,((_tmp1094->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp1094->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp1094)))))))))))),((
_tmp1095->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp1095)))))),_tmp1093))));}break;case 297: _LL32E:{struct Cyc_List_List*
_tmp109B;struct Cyc_Absyn_Switch_clause*_tmp109A;union Cyc_YYSTYPE_union _tmp1099;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1099.YY10).tag=14,(((_tmp1099.YY10).f1=((
_tmp109B=_cycalloc(sizeof(*_tmp109B)),((_tmp109B->hd=((_tmp109A=_cycalloc(
sizeof(*_tmp109A)),((_tmp109A->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmp109A->pat_vars=0,((
_tmp109A->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp109A->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),((_tmp109A->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmp109A)))))))))))),((
_tmp109B->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp109B)))))),_tmp1099))));}break;case 298: _LL32F:{struct Cyc_List_List*
_tmp10A1;struct Cyc_Absyn_Switch_clause*_tmp10A0;union Cyc_YYSTYPE_union _tmp109F;
yyval=(union Cyc_YYSTYPE_union)(((_tmp109F.YY10).tag=14,(((_tmp109F.YY10).f1=((
_tmp10A1=_cycalloc(sizeof(*_tmp10A1)),((_tmp10A1->hd=((_tmp10A0=_cycalloc(
sizeof(*_tmp10A0)),((_tmp10A0->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),((_tmp10A0->pat_vars=0,((
_tmp10A0->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp10A0->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp10A0->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmp10A0)))))))))))),((
_tmp10A1->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp10A1)))))),_tmp109F))));}break;case 299: _LL330:{union Cyc_YYSTYPE_union
_tmp10A2;yyval=(union Cyc_YYSTYPE_union)(((_tmp10A2.YY9).tag=13,(((_tmp10A2.YY9).f1=
Cyc_Absyn_while_stmt(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp10A2))));}break;case 300: _LL331:{union Cyc_YYSTYPE_union
_tmp10A3;yyval=(union Cyc_YYSTYPE_union)(((_tmp10A3.YY9).tag=13,(((_tmp10A3.YY9).f1=
Cyc_Absyn_do_stmt(Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10A3))));}
break;case 301: _LL332:{union Cyc_YYSTYPE_union _tmp10A4;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10A4.YY9).tag=13,(((_tmp10A4.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10A4))));}
break;case 302: _LL333:{union Cyc_YYSTYPE_union _tmp10A5;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10A5.YY9).tag=13,(((_tmp10A5.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp10A5))));}break;case 303: _LL334:{union Cyc_YYSTYPE_union
_tmp10A6;yyval=(union Cyc_YYSTYPE_union)(((_tmp10A6.YY9).tag=13,(((_tmp10A6.YY9).f1=
Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10A6))));}
break;case 304: _LL335:{union Cyc_YYSTYPE_union _tmp10A7;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10A7.YY9).tag=13,(((_tmp10A7.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp10A7))));}
break;case 305: _LL336:{union Cyc_YYSTYPE_union _tmp10A8;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10A8.YY9).tag=13,(((_tmp10A8.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp10A8))));}break;case 306: _LL337:{union Cyc_YYSTYPE_union
_tmp10A9;yyval=(union Cyc_YYSTYPE_union)(((_tmp10A9.YY9).tag=13,(((_tmp10A9.YY9).f1=
Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10A9))));}
break;case 307: _LL338:{union Cyc_YYSTYPE_union _tmp10AA;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10AA.YY9).tag=13,(((_tmp10AA.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10AA))));}
break;case 308: _LL339:{union Cyc_YYSTYPE_union _tmp10AB;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10AB.YY9).tag=13,(((_tmp10AB.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10AB))));}
break;case 309: _LL33A: {struct Cyc_List_List*_tmp76C=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp76D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{union Cyc_YYSTYPE_union
_tmp10AC;yyval=(union Cyc_YYSTYPE_union)(((_tmp10AC.YY9).tag=13,(((_tmp10AC.YY9).f1=
Cyc_Parse_flatten_declarations(_tmp76C,_tmp76D),_tmp10AC))));}break;}case 310:
_LL33B: {struct Cyc_List_List*_tmp76F=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp770=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{union Cyc_YYSTYPE_union
_tmp10AD;yyval=(union Cyc_YYSTYPE_union)(((_tmp10AD.YY9).tag=13,(((_tmp10AD.YY9).f1=
Cyc_Parse_flatten_declarations(_tmp76F,_tmp770),_tmp10AD))));}break;}case 311:
_LL33C: {struct Cyc_List_List*_tmp772=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp773=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{union Cyc_YYSTYPE_union
_tmp10AE;yyval=(union Cyc_YYSTYPE_union)(((_tmp10AE.YY9).tag=13,(((_tmp10AE.YY9).f1=
Cyc_Parse_flatten_declarations(_tmp772,_tmp773),_tmp10AE))));}break;}case 312:
_LL33D: {struct Cyc_List_List*_tmp775=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*
_tmp776=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{union Cyc_YYSTYPE_union
_tmp10AF;yyval=(union Cyc_YYSTYPE_union)(((_tmp10AF.YY9).tag=13,(((_tmp10AF.YY9).f1=
Cyc_Parse_flatten_declarations(_tmp775,_tmp776),_tmp10AF))));}break;}case 313:
_LL33E:{struct _dyneither_ptr*_tmp10B2;union Cyc_YYSTYPE_union _tmp10B1;yyval=(
union Cyc_YYSTYPE_union)(((_tmp10B1.YY9).tag=13,(((_tmp10B1.YY9).f1=Cyc_Absyn_goto_stmt(((
_tmp10B2=_cycalloc(sizeof(*_tmp10B2)),((_tmp10B2[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmp10B2)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp10B1))));}
break;case 314: _LL33F:{union Cyc_YYSTYPE_union _tmp10B3;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10B3.YY9).tag=13,(((_tmp10B3.YY9).f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp10B3))));}
break;case 315: _LL340:{union Cyc_YYSTYPE_union _tmp10B4;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10B4.YY9).tag=13,(((_tmp10B4.YY9).f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp10B4))));}
break;case 316: _LL341:{union Cyc_YYSTYPE_union _tmp10B5;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10B5.YY9).tag=13,(((_tmp10B5.YY9).f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp10B5))));}
break;case 317: _LL342:{union Cyc_YYSTYPE_union _tmp10B6;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10B6.YY9).tag=13,(((_tmp10B6.YY9).f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp10B6))));}
break;case 318: _LL343:{union Cyc_YYSTYPE_union _tmp10B7;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10B7.YY9).tag=13,(((_tmp10B7.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp10B7))));}
break;case 319: _LL344:{union Cyc_YYSTYPE_union _tmp10B8;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10B8.YY9).tag=13,(((_tmp10B8.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)),_tmp10B8))));}
break;case 320: _LL345:{union Cyc_YYSTYPE_union _tmp10B9;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10B9.YY9).tag=13,(((_tmp10B9.YY9).f1=Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line)),_tmp10B9))));}
break;case 321: _LL346: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 322: _LL347: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 323: _LL348:{union Cyc_YYSTYPE_union _tmp10BA;yyval=(
union Cyc_YYSTYPE_union)(((_tmp10BA.YY11).tag=15,(((_tmp10BA.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10BA))));}
break;case 324: _LL349: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 325: _LL34A:{union Cyc_YYSTYPE_union _tmp10BB;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10BB.YY11).tag=15,(((_tmp10BB.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10BB))));}
break;case 326: _LL34B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 327: _LL34C:{union Cyc_YYSTYPE_union _tmp10BC;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10BC.YY11).tag=15,(((_tmp10BC.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10BC))));}
break;case 328: _LL34D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 329: _LL34E:{union Cyc_YYSTYPE_union _tmp10BD;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10BD.YY11).tag=15,(((_tmp10BD.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)14,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10BD))));}
break;case 330: _LL34F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 331: _LL350:{union Cyc_YYSTYPE_union _tmp10BE;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10BE.YY11).tag=15,(((_tmp10BE.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)15,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10BE))));}
break;case 332: _LL351: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 333: _LL352:{union Cyc_YYSTYPE_union _tmp10BF;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10BF.YY11).tag=15,(((_tmp10BF.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)13,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10BF))));}
break;case 334: _LL353: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 335: _LL354:{union Cyc_YYSTYPE_union _tmp10C0;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10C0.YY11).tag=15,(((_tmp10C0.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10C0))));}
break;case 336: _LL355:{union Cyc_YYSTYPE_union _tmp10C1;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10C1.YY11).tag=15,(((_tmp10C1.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp10C1))));}break;case 337: _LL356: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 338: _LL357:{union
Cyc_YYSTYPE_union _tmp10C2;yyval=(union Cyc_YYSTYPE_union)(((_tmp10C2.YY11).tag=15,(((
_tmp10C2.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10C2))));}
break;case 339: _LL358:{union Cyc_YYSTYPE_union _tmp10C3;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10C3.YY11).tag=15,(((_tmp10C3.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp10C3))));}break;case 340: _LL359:{union Cyc_YYSTYPE_union
_tmp10C4;yyval=(union Cyc_YYSTYPE_union)(((_tmp10C4.YY11).tag=15,(((_tmp10C4.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10C4))));}
break;case 341: _LL35A:{union Cyc_YYSTYPE_union _tmp10C5;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10C5.YY11).tag=15,(((_tmp10C5.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp10C5))));}break;case 342: _LL35B: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 343: _LL35C:{union
Cyc_YYSTYPE_union _tmp10C6;yyval=(union Cyc_YYSTYPE_union)(((_tmp10C6.YY11).tag=15,(((
_tmp10C6.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)16,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10C6))));}
break;case 344: _LL35D:{union Cyc_YYSTYPE_union _tmp10C7;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10C7.YY11).tag=15,(((_tmp10C7.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)17,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10C7))));}
break;case 345: _LL35E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 346: _LL35F:{union Cyc_YYSTYPE_union _tmp10C8;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10C8.YY11).tag=15,(((_tmp10C8.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)0,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10C8))));}
break;case 347: _LL360:{union Cyc_YYSTYPE_union _tmp10C9;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10C9.YY11).tag=15,(((_tmp10C9.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)2,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10C9))));}
break;case 348: _LL361: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 349: _LL362:{union Cyc_YYSTYPE_union _tmp10CA;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10CA.YY11).tag=15,(((_tmp10CA.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)1,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10CA))));}
break;case 350: _LL363:{union Cyc_YYSTYPE_union _tmp10CB;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10CB.YY11).tag=15,(((_tmp10CB.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)3,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10CB))));}
break;case 351: _LL364:{union Cyc_YYSTYPE_union _tmp10CC;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10CC.YY11).tag=15,(((_tmp10CC.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)4,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10CC))));}
break;case 352: _LL365: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 353: _LL366:{union Cyc_YYSTYPE_union _tmp10CD;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10CD.YY11).tag=15,(((_tmp10CD.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10CD))));}
break;case 354: _LL367: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 355: _LL368:{union Cyc_YYSTYPE_union _tmp10CE;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10CE.YY11).tag=15,(((_tmp10CE.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),
_tmp10CE))));}break;case 356: _LL369:{union Cyc_YYSTYPE_union _tmp10CF;yyval=(union
Cyc_YYSTYPE_union)(((_tmp10CF.YY11).tag=15,(((_tmp10CF.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp10CF))));}break;case 357: _LL36A:{union Cyc_YYSTYPE_union
_tmp10D0;yyval=(union Cyc_YYSTYPE_union)(((_tmp10D0.YY11).tag=15,(((_tmp10D0.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10D0))));}
break;case 358: _LL36B:{union Cyc_YYSTYPE_union _tmp10D1;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10D1.YY11).tag=15,(((_tmp10D1.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10D1))));}
break;case 359: _LL36C:{struct Cyc_Absyn_StructField_struct*_tmp10DB;struct
_dyneither_ptr*_tmp10DA;struct Cyc_Absyn_StructField_struct _tmp10D9;union Cyc_YYSTYPE_union
_tmp10D8;yyval=(union Cyc_YYSTYPE_union)(((_tmp10D8.YY11).tag=15,(((_tmp10D8.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)((_tmp10DB=
_cycalloc(sizeof(*_tmp10DB)),((_tmp10DB[0]=((_tmp10D9.tag=0,((_tmp10D9.f1=((
_tmp10DA=_cycalloc(sizeof(*_tmp10DA)),((_tmp10DA[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmp10DA)))),_tmp10D9)))),
_tmp10DB)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp10D8))));}break;case 360: _LL36D:{struct Cyc_Absyn_TupleIndex_struct*
_tmp10E1;struct Cyc_Absyn_TupleIndex_struct _tmp10E0;union Cyc_YYSTYPE_union
_tmp10DF;yyval=(union Cyc_YYSTYPE_union)(((_tmp10DF.YY11).tag=15,(((_tmp10DF.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)((_tmp10E1=
_cycalloc_atomic(sizeof(*_tmp10E1)),((_tmp10E1[0]=((_tmp10E0.tag=1,((_tmp10E0.f1=(
unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f2,_tmp10E0)))),_tmp10E1)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp10DF))));}
break;case 361: _LL36E: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 362: _LL36F: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 363: _LL370:{union Cyc_YYSTYPE_union _tmp10E2;yyval=(
union Cyc_YYSTYPE_union)(((_tmp10E2.YY11).tag=15,(((_tmp10E2.YY11).f1=Cyc_Absyn_new_pat((
void*)0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp10E2))));}break;case 364: _LL371:{union Cyc_YYSTYPE_union
_tmp10E3;yyval=(union Cyc_YYSTYPE_union)(((_tmp10E3.YY11).tag=15,(((_tmp10E3.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])),_tmp10E3))));}break;case 365: _LL372: {struct Cyc_Absyn_Exp*e=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);{void*
_tmp7A2=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp7A3;void*_tmp7A4;char _tmp7A5;
union Cyc_Absyn_Cnst_union _tmp7A6;void*_tmp7A7;short _tmp7A8;union Cyc_Absyn_Cnst_union
_tmp7A9;void*_tmp7AA;int _tmp7AB;union Cyc_Absyn_Cnst_union _tmp7AC;struct
_dyneither_ptr _tmp7AD;union Cyc_Absyn_Cnst_union _tmp7AE;union Cyc_Absyn_Cnst_union
_tmp7AF;union Cyc_Absyn_Cnst_union _tmp7B0;_LL375: if(*((int*)_tmp7A2)!= 0)goto
_LL377;_tmp7A3=((struct Cyc_Absyn_Const_e_struct*)_tmp7A2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7A2)->f1).Char_c).tag != 0)goto _LL377;_tmp7A4=(_tmp7A3.Char_c).f1;_tmp7A5=(
_tmp7A3.Char_c).f2;_LL376:{struct Cyc_Absyn_Char_p_struct*_tmp10E9;struct Cyc_Absyn_Char_p_struct
_tmp10E8;union Cyc_YYSTYPE_union _tmp10E7;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10E7.YY11).tag=15,(((_tmp10E7.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp10E9=
_cycalloc_atomic(sizeof(*_tmp10E9)),((_tmp10E9[0]=((_tmp10E8.tag=8,((_tmp10E8.f1=
_tmp7A5,_tmp10E8)))),_tmp10E9)))),e->loc),_tmp10E7))));}goto _LL374;_LL377: if(*((
int*)_tmp7A2)!= 0)goto _LL379;_tmp7A6=((struct Cyc_Absyn_Const_e_struct*)_tmp7A2)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A2)->f1).Short_c).tag != 1)goto _LL379;
_tmp7A7=(_tmp7A6.Short_c).f1;_tmp7A8=(_tmp7A6.Short_c).f2;_LL378:{struct Cyc_Absyn_Int_p_struct*
_tmp10EF;struct Cyc_Absyn_Int_p_struct _tmp10EE;union Cyc_YYSTYPE_union _tmp10ED;
yyval=(union Cyc_YYSTYPE_union)(((_tmp10ED.YY11).tag=15,(((_tmp10ED.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp10EF=_cycalloc(sizeof(*_tmp10EF)),((_tmp10EF[0]=((_tmp10EE.tag=7,((
_tmp10EE.f1=(void*)_tmp7A7,((_tmp10EE.f2=(int)_tmp7A8,_tmp10EE)))))),_tmp10EF)))),
e->loc),_tmp10ED))));}goto _LL374;_LL379: if(*((int*)_tmp7A2)!= 0)goto _LL37B;
_tmp7A9=((struct Cyc_Absyn_Const_e_struct*)_tmp7A2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7A2)->f1).Int_c).tag != 2)goto _LL37B;_tmp7AA=(_tmp7A9.Int_c).f1;_tmp7AB=(
_tmp7A9.Int_c).f2;_LL37A:{struct Cyc_Absyn_Int_p_struct*_tmp10F5;struct Cyc_Absyn_Int_p_struct
_tmp10F4;union Cyc_YYSTYPE_union _tmp10F3;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10F3.YY11).tag=15,(((_tmp10F3.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp10F5=
_cycalloc(sizeof(*_tmp10F5)),((_tmp10F5[0]=((_tmp10F4.tag=7,((_tmp10F4.f1=(void*)
_tmp7AA,((_tmp10F4.f2=_tmp7AB,_tmp10F4)))))),_tmp10F5)))),e->loc),_tmp10F3))));}
goto _LL374;_LL37B: if(*((int*)_tmp7A2)!= 0)goto _LL37D;_tmp7AC=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A2)->f1).Float_c).tag != 
4)goto _LL37D;_tmp7AD=(_tmp7AC.Float_c).f1;_LL37C:{struct Cyc_Absyn_Float_p_struct*
_tmp10FB;struct Cyc_Absyn_Float_p_struct _tmp10FA;union Cyc_YYSTYPE_union _tmp10F9;
yyval=(union Cyc_YYSTYPE_union)(((_tmp10F9.YY11).tag=15,(((_tmp10F9.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp10FB=_cycalloc(sizeof(*_tmp10FB)),((_tmp10FB[0]=((_tmp10FA.tag=9,((
_tmp10FA.f1=_tmp7AD,_tmp10FA)))),_tmp10FB)))),e->loc),_tmp10F9))));}goto _LL374;
_LL37D: if(*((int*)_tmp7A2)!= 0)goto _LL37F;_tmp7AE=((struct Cyc_Absyn_Const_e_struct*)
_tmp7A2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7A2)->f1).Null_c).tag != 
6)goto _LL37F;_LL37E:{union Cyc_YYSTYPE_union _tmp10FC;yyval=(union Cyc_YYSTYPE_union)(((
_tmp10FC.YY11).tag=15,(((_tmp10FC.YY11).f1=Cyc_Absyn_new_pat((void*)1,e->loc),
_tmp10FC))));}goto _LL374;_LL37F: if(*((int*)_tmp7A2)!= 0)goto _LL381;_tmp7AF=((
struct Cyc_Absyn_Const_e_struct*)_tmp7A2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7A2)->f1).String_c).tag != 5)goto _LL381;_LL380:{const char*_tmp10FD;Cyc_Parse_err(((
_tmp10FD="strings cannot occur within patterns",_tag_dyneither(_tmp10FD,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}goto _LL374;_LL381: if(*((int*)_tmp7A2)!= 0)goto
_LL383;_tmp7B0=((struct Cyc_Absyn_Const_e_struct*)_tmp7A2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7A2)->f1).LongLong_c).tag != 3)goto _LL383;_LL382:{const char*_tmp10FE;Cyc_Parse_unimp(((
_tmp10FE="long long's in patterns",_tag_dyneither(_tmp10FE,sizeof(char),24))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}goto _LL374;_LL383:;_LL384: {const char*_tmp10FF;Cyc_Parse_err(((
_tmp10FF="bad constant in case",_tag_dyneither(_tmp10FF,sizeof(char),21))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}_LL374:;}
break;}case 366: _LL373:{struct Cyc_Absyn_UnknownId_p_struct*_tmp1105;struct Cyc_Absyn_UnknownId_p_struct
_tmp1104;union Cyc_YYSTYPE_union _tmp1103;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1103.YY11).tag=15,(((_tmp1103.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp1105=
_cycalloc(sizeof(*_tmp1105)),((_tmp1105[0]=((_tmp1104.tag=12,((_tmp1104.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1104)))),_tmp1105)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1103))));}break;case 367: _LL385:{const char*
_tmp1106;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp1106="as",
_tag_dyneither(_tmp1106,sizeof(char),3))))!= 0){const char*_tmp1107;Cyc_Parse_err(((
_tmp1107="expecting `as'",_tag_dyneither(_tmp1107,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Var_p_struct*
_tmp111B;struct _tuple1*_tmp111A;struct _dyneither_ptr*_tmp1119;union Cyc_Absyn_Nmspace_union
_tmp1118;struct Cyc_Absyn_Var_p_struct _tmp1117;union Cyc_YYSTYPE_union _tmp1116;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1116.YY11).tag=15,(((_tmp1116.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp111B=_cycalloc(sizeof(*_tmp111B)),((_tmp111B[0]=((_tmp1117.tag=0,((
_tmp1117.f1=Cyc_Absyn_new_vardecl(((_tmp111A=_cycalloc(sizeof(*_tmp111A)),((
_tmp111A->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp1118.Loc_n).tag=0,_tmp1118)),((
_tmp111A->f2=((_tmp1119=_cycalloc(sizeof(*_tmp1119)),((_tmp1119[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp1119)))),
_tmp111A)))))),(void*)0,0),((_tmp1117.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1117)))))),_tmp111B)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)),_tmp1116))));}break;case 368: _LL386: {struct Cyc_List_List*
_tmp7CD;int _tmp7CE;struct _tuple13 _tmp7CC=*Cyc_yyget_YY12(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7CD=_tmp7CC.f1;
_tmp7CE=_tmp7CC.f2;{struct Cyc_Absyn_Tuple_p_struct*_tmp1121;struct Cyc_Absyn_Tuple_p_struct
_tmp1120;union Cyc_YYSTYPE_union _tmp111F;yyval=(union Cyc_YYSTYPE_union)(((
_tmp111F.YY11).tag=15,(((_tmp111F.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp1121=
_cycalloc(sizeof(*_tmp1121)),((_tmp1121[0]=((_tmp1120.tag=3,((_tmp1120.f1=
_tmp7CD,((_tmp1120.f2=_tmp7CE,_tmp1120)))))),_tmp1121)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp111F))));}
break;}case 369: _LL387: {struct Cyc_List_List*_tmp7D3;int _tmp7D4;struct _tuple13
_tmp7D2=*Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7D3=_tmp7D2.f1;_tmp7D4=_tmp7D2.f2;{struct Cyc_Absyn_UnknownCall_p_struct*
_tmp1127;struct Cyc_Absyn_UnknownCall_p_struct _tmp1126;union Cyc_YYSTYPE_union
_tmp1125;yyval=(union Cyc_YYSTYPE_union)(((_tmp1125.YY11).tag=15,(((_tmp1125.YY11).f1=
Cyc_Absyn_new_pat((void*)((_tmp1127=_cycalloc(sizeof(*_tmp1127)),((_tmp1127[0]=((
_tmp1126.tag=13,((_tmp1126.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp1126.f2=_tmp7D3,((
_tmp1126.f3=_tmp7D4,_tmp1126)))))))),_tmp1127)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1125))));}
break;}case 370: _LL388: {struct Cyc_List_List*_tmp7D9;int _tmp7DA;struct _tuple13
_tmp7D8=*Cyc_yyget_YY16(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7D9=_tmp7D8.f1;_tmp7DA=_tmp7D8.f2;{struct Cyc_List_List*_tmp7DB=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct*_tmp1136;struct Cyc_Absyn_AggrInfo
_tmp1135;union Cyc_Absyn_AggrInfoU_union _tmp1134;struct Cyc_Absyn_Aggr_p_struct
_tmp1133;union Cyc_YYSTYPE_union _tmp1132;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1132.YY11).tag=15,(((_tmp1132.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp1136=
_cycalloc(sizeof(*_tmp1136)),((_tmp1136[0]=((_tmp1133.tag=5,((_tmp1133.f1=((
_tmp1135.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((_tmp1134.UnknownAggr).tag=
0,(((_tmp1134.UnknownAggr).f1=(void*)((void*)0),(((_tmp1134.UnknownAggr).f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp1134)))))),((
_tmp1135.targs=0,_tmp1135)))),((_tmp1133.f2=_tmp7DB,((_tmp1133.f3=_tmp7D9,((
_tmp1133.f4=_tmp7DA,_tmp1133)))))))))),_tmp1136)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1132))));}
break;}}case 371: _LL389:{struct Cyc_Absyn_Pointer_p_struct*_tmp113C;struct Cyc_Absyn_Pointer_p_struct
_tmp113B;union Cyc_YYSTYPE_union _tmp113A;yyval=(union Cyc_YYSTYPE_union)(((
_tmp113A.YY11).tag=15,(((_tmp113A.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp113C=
_cycalloc(sizeof(*_tmp113C)),((_tmp113C[0]=((_tmp113B.tag=4,((_tmp113B.f1=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp113B)))),_tmp113C)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp113A))));}break;case 372: _LL38A:{struct Cyc_Absyn_Pointer_p_struct*
_tmp114B;struct Cyc_Absyn_Pointer_p_struct*_tmp114A;struct Cyc_Absyn_Pointer_p_struct
_tmp1149;struct Cyc_Absyn_Pointer_p_struct _tmp1148;union Cyc_YYSTYPE_union _tmp1147;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1147.YY11).tag=15,(((_tmp1147.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp114B=_cycalloc(sizeof(*_tmp114B)),((_tmp114B[0]=((_tmp1148.tag=4,((
_tmp1148.f1=Cyc_Absyn_new_pat((void*)((_tmp114A=_cycalloc(sizeof(*_tmp114A)),((
_tmp114A[0]=((_tmp1149.tag=4,((_tmp1149.f1=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1149)))),_tmp114A)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1148)))),_tmp114B)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1147))));}
break;case 373: _LL38B:{struct Cyc_Absyn_Reference_p_struct*_tmp115F;struct _tuple1*
_tmp115E;struct _dyneither_ptr*_tmp115D;union Cyc_Absyn_Nmspace_union _tmp115C;
struct Cyc_Absyn_Reference_p_struct _tmp115B;union Cyc_YYSTYPE_union _tmp115A;yyval=(
union Cyc_YYSTYPE_union)(((_tmp115A.YY11).tag=15,(((_tmp115A.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp115F=_cycalloc(sizeof(*_tmp115F)),((_tmp115F[0]=((_tmp115B.tag=1,((
_tmp115B.f1=Cyc_Absyn_new_vardecl(((_tmp115E=_cycalloc(sizeof(*_tmp115E)),((
_tmp115E->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp115C.Loc_n).tag=0,_tmp115C)),((
_tmp115E->f2=((_tmp115D=_cycalloc(sizeof(*_tmp115D)),((_tmp115D[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp115D)))),_tmp115E)))))),(
void*)0,0),((_tmp115B.f2=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp115B)))))),
_tmp115F)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp115A))));}break;case 374: _LL38C:{const char*
_tmp1160;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp1160="as",
_tag_dyneither(_tmp1160,sizeof(char),3))))!= 0){const char*_tmp1161;Cyc_Parse_err(((
_tmp1161="expecting `as'",_tag_dyneither(_tmp1161,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Reference_p_struct*
_tmp1175;struct _tuple1*_tmp1174;struct _dyneither_ptr*_tmp1173;union Cyc_Absyn_Nmspace_union
_tmp1172;struct Cyc_Absyn_Reference_p_struct _tmp1171;union Cyc_YYSTYPE_union
_tmp1170;yyval=(union Cyc_YYSTYPE_union)(((_tmp1170.YY11).tag=15,(((_tmp1170.YY11).f1=
Cyc_Absyn_new_pat((void*)((_tmp1175=_cycalloc(sizeof(*_tmp1175)),((_tmp1175[0]=((
_tmp1171.tag=1,((_tmp1171.f1=Cyc_Absyn_new_vardecl(((_tmp1174=_cycalloc(sizeof(*
_tmp1174)),((_tmp1174->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp1172.Loc_n).tag=0,
_tmp1172)),((_tmp1174->f2=((_tmp1173=_cycalloc(sizeof(*_tmp1173)),((_tmp1173[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmp1173)))),_tmp1174)))))),(void*)0,0),((_tmp1171.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1171)))))),_tmp1175)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)),_tmp1170))));}break;case 375: _LL38D: {void*_tmp7F7=
Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_Tcutil_kind_to_bound((void*)7));{struct Cyc_Absyn_TagInt_p_struct*
_tmp1192;struct _tuple1*_tmp1191;struct _dyneither_ptr*_tmp1190;union Cyc_Absyn_Nmspace_union
_tmp118F;struct Cyc_Absyn_TagType_struct*_tmp118E;struct Cyc_Absyn_TagType_struct
_tmp118D;struct Cyc_Absyn_TagInt_p_struct _tmp118C;union Cyc_YYSTYPE_union _tmp118B;
yyval=(union Cyc_YYSTYPE_union)(((_tmp118B.YY11).tag=15,(((_tmp118B.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp1192=_cycalloc(sizeof(*_tmp1192)),((_tmp1192[0]=((_tmp118C.tag=2,((
_tmp118C.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp7F7),((
_tmp118C.f2=Cyc_Absyn_new_vardecl(((_tmp1191=_cycalloc(sizeof(*_tmp1191)),((
_tmp1191->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp118F.Loc_n).tag=0,_tmp118F)),((
_tmp1191->f2=((_tmp1190=_cycalloc(sizeof(*_tmp1190)),((_tmp1190[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp1190)))),
_tmp1191)))))),(void*)((_tmp118E=_cycalloc(sizeof(*_tmp118E)),((_tmp118E[0]=((
_tmp118D.tag=18,((_tmp118D.f1=(void*)_tmp7F7,_tmp118D)))),_tmp118E)))),0),
_tmp118C)))))),_tmp1192)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp118B))));}
break;}case 376: _LL38E: {struct Cyc_Absyn_Tvar*_tmp800=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound((
void*)7));{struct Cyc_Absyn_TagInt_p_struct*_tmp11BC;struct _tuple1*_tmp11BB;
struct _dyneither_ptr*_tmp11BA;union Cyc_Absyn_Nmspace_union _tmp11B9;struct Cyc_Absyn_TagType_struct*
_tmp11B8;struct Cyc_Absyn_VarType_struct*_tmp11B7;struct Cyc_Absyn_VarType_struct
_tmp11B6;struct Cyc_Absyn_TagType_struct _tmp11B5;struct Cyc_Absyn_TagInt_p_struct
_tmp11B4;union Cyc_YYSTYPE_union _tmp11B3;yyval=(union Cyc_YYSTYPE_union)(((
_tmp11B3.YY11).tag=15,(((_tmp11B3.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp11BC=
_cycalloc(sizeof(*_tmp11BC)),((_tmp11BC[0]=((_tmp11B4.tag=2,((_tmp11B4.f1=
_tmp800,((_tmp11B4.f2=Cyc_Absyn_new_vardecl(((_tmp11BB=_cycalloc(sizeof(*
_tmp11BB)),((_tmp11BB->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp11B9.Loc_n).tag=0,
_tmp11B9)),((_tmp11BB->f2=((_tmp11BA=_cycalloc(sizeof(*_tmp11BA)),((_tmp11BA[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmp11BA)))),_tmp11BB)))))),(void*)((_tmp11B8=_cycalloc(sizeof(*_tmp11B8)),((
_tmp11B8[0]=((_tmp11B5.tag=18,((_tmp11B5.f1=(void*)((void*)((_tmp11B7=_cycalloc(
sizeof(*_tmp11B7)),((_tmp11B7[0]=((_tmp11B6.tag=1,((_tmp11B6.f1=_tmp800,_tmp11B6)))),
_tmp11B7))))),_tmp11B5)))),_tmp11B8)))),0),_tmp11B4)))))),_tmp11BC)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp11B3))));}
break;}case 377: _LL38F:{struct _tuple13*_tmp11BF;union Cyc_YYSTYPE_union _tmp11BE;
yyval=(union Cyc_YYSTYPE_union)(((_tmp11BE.YY12).tag=16,(((_tmp11BE.YY12).f1=((
_tmp11BF=_cycalloc(sizeof(*_tmp11BF)),((_tmp11BF->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),((_tmp11BF->f2=0,_tmp11BF)))))),
_tmp11BE))));}break;case 378: _LL390:{struct _tuple13*_tmp11C2;union Cyc_YYSTYPE_union
_tmp11C1;yyval=(union Cyc_YYSTYPE_union)(((_tmp11C1.YY12).tag=16,(((_tmp11C1.YY12).f1=((
_tmp11C2=_cycalloc(sizeof(*_tmp11C2)),((_tmp11C2->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmp11C2->f2=1,
_tmp11C2)))))),_tmp11C1))));}break;case 379: _LL391:{struct _tuple13*_tmp11C5;union
Cyc_YYSTYPE_union _tmp11C4;yyval=(union Cyc_YYSTYPE_union)(((_tmp11C4.YY12).tag=16,(((
_tmp11C4.YY12).f1=((_tmp11C5=_cycalloc(sizeof(*_tmp11C5)),((_tmp11C5->f1=0,((
_tmp11C5->f2=1,_tmp11C5)))))),_tmp11C4))));}break;case 380: _LL392:{struct Cyc_List_List*
_tmp11C8;union Cyc_YYSTYPE_union _tmp11C7;yyval=(union Cyc_YYSTYPE_union)(((
_tmp11C7.YY13).tag=17,(((_tmp11C7.YY13).f1=((_tmp11C8=_cycalloc(sizeof(*_tmp11C8)),((
_tmp11C8->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmp11C8->tl=0,_tmp11C8)))))),_tmp11C7))));}break;case 381:
_LL393:{struct Cyc_List_List*_tmp11CB;union Cyc_YYSTYPE_union _tmp11CA;yyval=(union
Cyc_YYSTYPE_union)(((_tmp11CA.YY13).tag=17,(((_tmp11CA.YY13).f1=((_tmp11CB=
_cycalloc(sizeof(*_tmp11CB)),((_tmp11CB->hd=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp11CB->tl=Cyc_yyget_YY13(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp11CB)))))),
_tmp11CA))));}break;case 382: _LL394:{struct _tuple14*_tmp11CE;union Cyc_YYSTYPE_union
_tmp11CD;yyval=(union Cyc_YYSTYPE_union)(((_tmp11CD.YY14).tag=18,(((_tmp11CD.YY14).f1=((
_tmp11CE=_cycalloc(sizeof(*_tmp11CE)),((_tmp11CE->f1=0,((_tmp11CE->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp11CE)))))),
_tmp11CD))));}break;case 383: _LL395:{struct _tuple14*_tmp11D1;union Cyc_YYSTYPE_union
_tmp11D0;yyval=(union Cyc_YYSTYPE_union)(((_tmp11D0.YY14).tag=18,(((_tmp11D0.YY14).f1=((
_tmp11D1=_cycalloc(sizeof(*_tmp11D1)),((_tmp11D1->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp11D1->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp11D1)))))),
_tmp11D0))));}break;case 384: _LL396:{struct _tuple13*_tmp11D4;union Cyc_YYSTYPE_union
_tmp11D3;yyval=(union Cyc_YYSTYPE_union)(((_tmp11D3.YY16).tag=20,(((_tmp11D3.YY16).f1=((
_tmp11D4=_cycalloc(sizeof(*_tmp11D4)),((_tmp11D4->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),((_tmp11D4->f2=0,_tmp11D4)))))),
_tmp11D3))));}break;case 385: _LL397:{struct _tuple13*_tmp11D7;union Cyc_YYSTYPE_union
_tmp11D6;yyval=(union Cyc_YYSTYPE_union)(((_tmp11D6.YY16).tag=20,(((_tmp11D6.YY16).f1=((
_tmp11D7=_cycalloc(sizeof(*_tmp11D7)),((_tmp11D7->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmp11D7->f2=1,
_tmp11D7)))))),_tmp11D6))));}break;case 386: _LL398:{struct _tuple13*_tmp11DA;union
Cyc_YYSTYPE_union _tmp11D9;yyval=(union Cyc_YYSTYPE_union)(((_tmp11D9.YY16).tag=20,(((
_tmp11D9.YY16).f1=((_tmp11DA=_cycalloc(sizeof(*_tmp11DA)),((_tmp11DA->f1=0,((
_tmp11DA->f2=1,_tmp11DA)))))),_tmp11D9))));}break;case 387: _LL399:{struct Cyc_List_List*
_tmp11DD;union Cyc_YYSTYPE_union _tmp11DC;yyval=(union Cyc_YYSTYPE_union)(((
_tmp11DC.YY15).tag=19,(((_tmp11DC.YY15).f1=((_tmp11DD=_cycalloc(sizeof(*_tmp11DD)),((
_tmp11DD->hd=Cyc_yyget_YY14(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmp11DD->tl=0,_tmp11DD)))))),_tmp11DC))));}break;case 388:
_LL39A:{struct Cyc_List_List*_tmp11E0;union Cyc_YYSTYPE_union _tmp11DF;yyval=(union
Cyc_YYSTYPE_union)(((_tmp11DF.YY15).tag=19,(((_tmp11DF.YY15).f1=((_tmp11E0=
_cycalloc(sizeof(*_tmp11E0)),((_tmp11E0->hd=Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp11E0->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp11E0)))))),
_tmp11DF))));}break;case 389: _LL39B: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 390: _LL39C:{union Cyc_YYSTYPE_union _tmp11E1;yyval=(
union Cyc_YYSTYPE_union)(((_tmp11E1.YY3).tag=7,(((_tmp11E1.YY3).f1=Cyc_Absyn_seq_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp11E1))));}
break;case 391: _LL39D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 392: _LL39E:{union Cyc_YYSTYPE_union _tmp11E2;yyval=(union
Cyc_YYSTYPE_union)(((_tmp11E2.YY3).tag=7,(((_tmp11E2.YY3).f1=Cyc_Absyn_assignop_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp11E2))));}
break;case 393: _LL39F:{struct Cyc_Absyn_Swap_e_struct*_tmp11E8;struct Cyc_Absyn_Swap_e_struct
_tmp11E7;union Cyc_YYSTYPE_union _tmp11E6;yyval=(union Cyc_YYSTYPE_union)(((
_tmp11E6.YY3).tag=7,(((_tmp11E6.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp11E8=
_cycalloc(sizeof(*_tmp11E8)),((_tmp11E8[0]=((_tmp11E7.tag=36,((_tmp11E7.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp11E7.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp11E7)))))),
_tmp11E8)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp11E6))));}break;case 394: _LL3A0:{union Cyc_YYSTYPE_union
_tmp11E9;yyval=(union Cyc_YYSTYPE_union)(((_tmp11E9.YY8).tag=12,(((_tmp11E9.YY8).f1=
0,_tmp11E9))));}break;case 395: _LL3A1:{struct Cyc_Core_Opt*_tmp11EC;union Cyc_YYSTYPE_union
_tmp11EB;yyval=(union Cyc_YYSTYPE_union)(((_tmp11EB.YY8).tag=12,(((_tmp11EB.YY8).f1=((
_tmp11EC=_cycalloc(sizeof(*_tmp11EC)),((_tmp11EC->v=(void*)((void*)1),_tmp11EC)))),
_tmp11EB))));}break;case 396: _LL3A2:{struct Cyc_Core_Opt*_tmp11EF;union Cyc_YYSTYPE_union
_tmp11EE;yyval=(union Cyc_YYSTYPE_union)(((_tmp11EE.YY8).tag=12,(((_tmp11EE.YY8).f1=((
_tmp11EF=_cycalloc(sizeof(*_tmp11EF)),((_tmp11EF->v=(void*)((void*)3),_tmp11EF)))),
_tmp11EE))));}break;case 397: _LL3A3:{struct Cyc_Core_Opt*_tmp11F2;union Cyc_YYSTYPE_union
_tmp11F1;yyval=(union Cyc_YYSTYPE_union)(((_tmp11F1.YY8).tag=12,(((_tmp11F1.YY8).f1=((
_tmp11F2=_cycalloc(sizeof(*_tmp11F2)),((_tmp11F2->v=(void*)((void*)4),_tmp11F2)))),
_tmp11F1))));}break;case 398: _LL3A4:{struct Cyc_Core_Opt*_tmp11F5;union Cyc_YYSTYPE_union
_tmp11F4;yyval=(union Cyc_YYSTYPE_union)(((_tmp11F4.YY8).tag=12,(((_tmp11F4.YY8).f1=((
_tmp11F5=_cycalloc(sizeof(*_tmp11F5)),((_tmp11F5->v=(void*)((void*)0),_tmp11F5)))),
_tmp11F4))));}break;case 399: _LL3A5:{struct Cyc_Core_Opt*_tmp11F8;union Cyc_YYSTYPE_union
_tmp11F7;yyval=(union Cyc_YYSTYPE_union)(((_tmp11F7.YY8).tag=12,(((_tmp11F7.YY8).f1=((
_tmp11F8=_cycalloc(sizeof(*_tmp11F8)),((_tmp11F8->v=(void*)((void*)2),_tmp11F8)))),
_tmp11F7))));}break;case 400: _LL3A6:{struct Cyc_Core_Opt*_tmp11FB;union Cyc_YYSTYPE_union
_tmp11FA;yyval=(union Cyc_YYSTYPE_union)(((_tmp11FA.YY8).tag=12,(((_tmp11FA.YY8).f1=((
_tmp11FB=_cycalloc(sizeof(*_tmp11FB)),((_tmp11FB->v=(void*)((void*)16),_tmp11FB)))),
_tmp11FA))));}break;case 401: _LL3A7:{struct Cyc_Core_Opt*_tmp11FE;union Cyc_YYSTYPE_union
_tmp11FD;yyval=(union Cyc_YYSTYPE_union)(((_tmp11FD.YY8).tag=12,(((_tmp11FD.YY8).f1=((
_tmp11FE=_cycalloc(sizeof(*_tmp11FE)),((_tmp11FE->v=(void*)((void*)17),_tmp11FE)))),
_tmp11FD))));}break;case 402: _LL3A8:{struct Cyc_Core_Opt*_tmp1201;union Cyc_YYSTYPE_union
_tmp1200;yyval=(union Cyc_YYSTYPE_union)(((_tmp1200.YY8).tag=12,(((_tmp1200.YY8).f1=((
_tmp1201=_cycalloc(sizeof(*_tmp1201)),((_tmp1201->v=(void*)((void*)13),_tmp1201)))),
_tmp1200))));}break;case 403: _LL3A9:{struct Cyc_Core_Opt*_tmp1204;union Cyc_YYSTYPE_union
_tmp1203;yyval=(union Cyc_YYSTYPE_union)(((_tmp1203.YY8).tag=12,(((_tmp1203.YY8).f1=((
_tmp1204=_cycalloc(sizeof(*_tmp1204)),((_tmp1204->v=(void*)((void*)15),_tmp1204)))),
_tmp1203))));}break;case 404: _LL3AA:{struct Cyc_Core_Opt*_tmp1207;union Cyc_YYSTYPE_union
_tmp1206;yyval=(union Cyc_YYSTYPE_union)(((_tmp1206.YY8).tag=12,(((_tmp1206.YY8).f1=((
_tmp1207=_cycalloc(sizeof(*_tmp1207)),((_tmp1207->v=(void*)((void*)14),_tmp1207)))),
_tmp1206))));}break;case 405: _LL3AB: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 406: _LL3AC:{union Cyc_YYSTYPE_union _tmp1208;yyval=(
union Cyc_YYSTYPE_union)(((_tmp1208.YY3).tag=7,(((_tmp1208.YY3).f1=Cyc_Absyn_conditional_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1208))));}
break;case 407: _LL3AD:{union Cyc_YYSTYPE_union _tmp1209;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1209.YY3).tag=7,(((_tmp1209.YY3).f1=Cyc_Absyn_throw_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1209))));}
break;case 408: _LL3AE:{union Cyc_YYSTYPE_union _tmp120A;yyval=(union Cyc_YYSTYPE_union)(((
_tmp120A.YY3).tag=7,(((_tmp120A.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp120A))));}
break;case 409: _LL3AF:{union Cyc_YYSTYPE_union _tmp120B;yyval=(union Cyc_YYSTYPE_union)(((
_tmp120B.YY3).tag=7,(((_tmp120B.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp120B))));}
break;case 410: _LL3B0:{union Cyc_YYSTYPE_union _tmp120C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp120C.YY3).tag=7,(((_tmp120C.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp120C))));}
break;case 411: _LL3B1:{union Cyc_YYSTYPE_union _tmp120D;yyval=(union Cyc_YYSTYPE_union)(((
_tmp120D.YY3).tag=7,(((_tmp120D.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp120D))));}
break;case 412: _LL3B2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 413: _LL3B3: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 414: _LL3B4:{union Cyc_YYSTYPE_union _tmp120E;yyval=(
union Cyc_YYSTYPE_union)(((_tmp120E.YY3).tag=7,(((_tmp120E.YY3).f1=Cyc_Absyn_or_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp120E))));}
break;case 415: _LL3B5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 416: _LL3B6:{union Cyc_YYSTYPE_union _tmp120F;yyval=(union
Cyc_YYSTYPE_union)(((_tmp120F.YY3).tag=7,(((_tmp120F.YY3).f1=Cyc_Absyn_and_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp120F))));}
break;case 417: _LL3B7: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 418: _LL3B8:{union Cyc_YYSTYPE_union _tmp1210;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1210.YY3).tag=7,(((_tmp1210.YY3).f1=Cyc_Absyn_prim2_exp((
void*)14,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1210))));}
break;case 419: _LL3B9: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 420: _LL3BA:{union Cyc_YYSTYPE_union _tmp1211;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1211.YY3).tag=7,(((_tmp1211.YY3).f1=Cyc_Absyn_prim2_exp((
void*)15,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1211))));}
break;case 421: _LL3BB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 422: _LL3BC:{union Cyc_YYSTYPE_union _tmp1212;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1212.YY3).tag=7,(((_tmp1212.YY3).f1=Cyc_Absyn_prim2_exp((
void*)13,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1212))));}
break;case 423: _LL3BD: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 424: _LL3BE:{union Cyc_YYSTYPE_union _tmp1213;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1213.YY3).tag=7,(((_tmp1213.YY3).f1=Cyc_Absyn_eq_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1213))));}
break;case 425: _LL3BF:{union Cyc_YYSTYPE_union _tmp1214;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1214.YY3).tag=7,(((_tmp1214.YY3).f1=Cyc_Absyn_neq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1214))));}
break;case 426: _LL3C0: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 427: _LL3C1:{union Cyc_YYSTYPE_union _tmp1215;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1215.YY3).tag=7,(((_tmp1215.YY3).f1=Cyc_Absyn_lt_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1215))));}
break;case 428: _LL3C2:{union Cyc_YYSTYPE_union _tmp1216;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1216.YY3).tag=7,(((_tmp1216.YY3).f1=Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1216))));}
break;case 429: _LL3C3:{union Cyc_YYSTYPE_union _tmp1217;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1217.YY3).tag=7,(((_tmp1217.YY3).f1=Cyc_Absyn_lte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1217))));}
break;case 430: _LL3C4:{union Cyc_YYSTYPE_union _tmp1218;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1218.YY3).tag=7,(((_tmp1218.YY3).f1=Cyc_Absyn_gte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1218))));}
break;case 431: _LL3C5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 432: _LL3C6:{union Cyc_YYSTYPE_union _tmp1219;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1219.YY3).tag=7,(((_tmp1219.YY3).f1=Cyc_Absyn_prim2_exp((
void*)16,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1219))));}
break;case 433: _LL3C7:{union Cyc_YYSTYPE_union _tmp121A;yyval=(union Cyc_YYSTYPE_union)(((
_tmp121A.YY3).tag=7,(((_tmp121A.YY3).f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp121A))));}
break;case 434: _LL3C8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 435: _LL3C9:{union Cyc_YYSTYPE_union _tmp121B;yyval=(union
Cyc_YYSTYPE_union)(((_tmp121B.YY3).tag=7,(((_tmp121B.YY3).f1=Cyc_Absyn_prim2_exp((
void*)0,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp121B))));}
break;case 436: _LL3CA:{union Cyc_YYSTYPE_union _tmp121C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp121C.YY3).tag=7,(((_tmp121C.YY3).f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp121C))));}
break;case 437: _LL3CB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 438: _LL3CC:{union Cyc_YYSTYPE_union _tmp121D;yyval=(union
Cyc_YYSTYPE_union)(((_tmp121D.YY3).tag=7,(((_tmp121D.YY3).f1=Cyc_Absyn_prim2_exp((
void*)1,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp121D))));}
break;case 439: _LL3CD:{union Cyc_YYSTYPE_union _tmp121E;yyval=(union Cyc_YYSTYPE_union)(((
_tmp121E.YY3).tag=7,(((_tmp121E.YY3).f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp121E))));}
break;case 440: _LL3CE:{union Cyc_YYSTYPE_union _tmp121F;yyval=(union Cyc_YYSTYPE_union)(((
_tmp121F.YY3).tag=7,(((_tmp121F.YY3).f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp121F))));}
break;case 441: _LL3CF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 442: _LL3D0:{union Cyc_YYSTYPE_union _tmp1220;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1220.YY3).tag=7,(((_tmp1220.YY3).f1=Cyc_Absyn_cast_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1220))));}break;case 443: _LL3D1: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 444: _LL3D2:{union
Cyc_YYSTYPE_union _tmp1221;yyval=(union Cyc_YYSTYPE_union)(((_tmp1221.YY3).tag=7,(((
_tmp1221.YY3).f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1221))));}
break;case 445: _LL3D3:{union Cyc_YYSTYPE_union _tmp1222;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1222.YY3).tag=7,(((_tmp1222.YY3).f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1222))));}
break;case 446: _LL3D4:{union Cyc_YYSTYPE_union _tmp1223;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1223.YY3).tag=7,(((_tmp1223.YY3).f1=Cyc_Absyn_address_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1223))));}
break;case 447: _LL3D5:{union Cyc_YYSTYPE_union _tmp1224;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1224.YY3).tag=7,(((_tmp1224.YY3).f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1224))));}
break;case 448: _LL3D6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 449: _LL3D7:{union Cyc_YYSTYPE_union _tmp1225;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1225.YY3).tag=7,(((_tmp1225.YY3).f1=Cyc_Absyn_prim1_exp(
Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1225))));}
break;case 450: _LL3D8:{union Cyc_YYSTYPE_union _tmp1226;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1226.YY3).tag=7,(((_tmp1226.YY3).f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1226))));}
break;case 451: _LL3D9:{union Cyc_YYSTYPE_union _tmp1227;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1227.YY3).tag=7,(((_tmp1227.YY3).f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1227))));}
break;case 452: _LL3DA:{struct Cyc_Absyn_StructField_struct*_tmp1231;struct
_dyneither_ptr*_tmp1230;struct Cyc_Absyn_StructField_struct _tmp122F;union Cyc_YYSTYPE_union
_tmp122E;yyval=(union Cyc_YYSTYPE_union)(((_tmp122E.YY3).tag=7,(((_tmp122E.YY3).f1=
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)((_tmp1231=_cycalloc(sizeof(*_tmp1231)),((
_tmp1231[0]=((_tmp122F.tag=0,((_tmp122F.f1=((_tmp1230=_cycalloc(sizeof(*_tmp1230)),((
_tmp1230[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmp1230)))),_tmp122F)))),_tmp1231)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp122E))));}
break;case 453: _LL3DB:{struct Cyc_Absyn_TupleIndex_struct*_tmp1237;struct Cyc_Absyn_TupleIndex_struct
_tmp1236;union Cyc_YYSTYPE_union _tmp1235;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1235.YY3).tag=7,(((_tmp1235.YY3).f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)((
_tmp1237=_cycalloc_atomic(sizeof(*_tmp1237)),((_tmp1237[0]=((_tmp1236.tag=1,((
_tmp1236.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2,_tmp1236)))),_tmp1237)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1235))));}
break;case 454: _LL3DC: {struct Cyc_Position_Segment*_tmp864=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp865=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp864,Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{
union Cyc_YYSTYPE_union _tmp1238;yyval=(union Cyc_YYSTYPE_union)(((_tmp1238.YY3).tag=
7,(((_tmp1238.YY3).f1=Cyc_Absyn_gentyp_exp(_tmp865,(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1238))));}
break;}case 455: _LL3DD:{struct Cyc_Absyn_Malloc_e_struct*_tmp1242;struct Cyc_Absyn_MallocInfo
_tmp1241;struct Cyc_Absyn_Malloc_e_struct _tmp1240;union Cyc_YYSTYPE_union _tmp123F;
yyval=(union Cyc_YYSTYPE_union)(((_tmp123F.YY3).tag=7,(((_tmp123F.YY3).f1=Cyc_Absyn_new_exp((
void*)((_tmp1242=_cycalloc(sizeof(*_tmp1242)),((_tmp1242[0]=((_tmp1240.tag=35,((
_tmp1240.f1=((_tmp1241.is_calloc=0,((_tmp1241.rgn=0,((_tmp1241.elt_type=0,((
_tmp1241.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmp1241.fat_result=0,_tmp1241)))))))))),_tmp1240)))),
_tmp1242)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp123F))));}break;case 456: _LL3DE:{struct Cyc_Absyn_Malloc_e_struct*
_tmp124C;struct Cyc_Absyn_MallocInfo _tmp124B;struct Cyc_Absyn_Malloc_e_struct
_tmp124A;union Cyc_YYSTYPE_union _tmp1249;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1249.YY3).tag=7,(((_tmp1249.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp124C=
_cycalloc(sizeof(*_tmp124C)),((_tmp124C[0]=((_tmp124A.tag=35,((_tmp124A.f1=((
_tmp124B.is_calloc=0,((_tmp124B.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp124B.elt_type=0,((
_tmp124B.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmp124B.fat_result=0,_tmp124B)))))))))),_tmp124A)))),
_tmp124C)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1249))));}break;case 457: _LL3DF: {void*_tmp870;
struct _tuple2 _tmp86F=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp870=_tmp86F.f3;{struct Cyc_Absyn_Malloc_e_struct*_tmp125B;
struct Cyc_Absyn_MallocInfo _tmp125A;void**_tmp1259;struct Cyc_Absyn_Malloc_e_struct
_tmp1258;union Cyc_YYSTYPE_union _tmp1257;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1257.YY3).tag=7,(((_tmp1257.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp125B=
_cycalloc(sizeof(*_tmp125B)),((_tmp125B[0]=((_tmp1258.tag=35,((_tmp1258.f1=((
_tmp125A.is_calloc=1,((_tmp125A.rgn=0,((_tmp125A.elt_type=((_tmp1259=_cycalloc(
sizeof(*_tmp1259)),((_tmp1259[0]=_tmp870,_tmp1259)))),((_tmp125A.num_elts=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((_tmp125A.fat_result=
0,_tmp125A)))))))))),_tmp1258)))),_tmp125B)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1257))));}
break;}case 458: _LL3E0: {void*_tmp877;struct _tuple2 _tmp876=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp877=_tmp876.f3;{
struct Cyc_Absyn_Malloc_e_struct*_tmp126A;struct Cyc_Absyn_MallocInfo _tmp1269;void**
_tmp1268;struct Cyc_Absyn_Malloc_e_struct _tmp1267;union Cyc_YYSTYPE_union _tmp1266;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1266.YY3).tag=7,(((_tmp1266.YY3).f1=Cyc_Absyn_new_exp((
void*)((_tmp126A=_cycalloc(sizeof(*_tmp126A)),((_tmp126A[0]=((_tmp1267.tag=35,((
_tmp1267.f1=((_tmp1269.is_calloc=1,((_tmp1269.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 8)]),((_tmp1269.elt_type=((
_tmp1268=_cycalloc(sizeof(*_tmp1268)),((_tmp1268[0]=_tmp877,_tmp1268)))),((
_tmp1269.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 6)]),((_tmp1269.fat_result=0,_tmp1269)))))))))),_tmp1267)))),
_tmp126A)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 10)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1266))));}break;}case 459: _LL3E1:{struct Cyc_Absyn_Exp*
_tmp126D[1];union Cyc_YYSTYPE_union _tmp126C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp126C.YY3).tag=7,(((_tmp126C.YY3).f1=Cyc_Absyn_primop_exp((void*)19,((
_tmp126D[0]=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp126D,sizeof(struct Cyc_Absyn_Exp*),1)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp126C))));}
break;case 460: _LL3E2: {void*_tmp880;struct _tuple2 _tmp87F=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp880=_tmp87F.f3;{
union Cyc_YYSTYPE_union _tmp126E;yyval=(union Cyc_YYSTYPE_union)(((_tmp126E.YY3).tag=
7,(((_tmp126E.YY3).f1=Cyc_Absyn_valueof_exp(_tmp880,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp126E))));}
break;}case 461: _LL3E3:{union Cyc_YYSTYPE_union _tmp126F;yyval=(union Cyc_YYSTYPE_union)(((
_tmp126F.YY7).tag=11,(((_tmp126F.YY7).f1=(void*)((void*)12),_tmp126F))));}break;
case 462: _LL3E4:{union Cyc_YYSTYPE_union _tmp1270;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1270.YY7).tag=11,(((_tmp1270.YY7).f1=(void*)((void*)11),_tmp1270))));}break;
case 463: _LL3E5:{union Cyc_YYSTYPE_union _tmp1271;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1271.YY7).tag=11,(((_tmp1271.YY7).f1=(void*)((void*)2),_tmp1271))));}break;
case 464: _LL3E6: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 465: _LL3E7:{union Cyc_YYSTYPE_union _tmp1272;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1272.YY3).tag=7,(((_tmp1272.YY3).f1=Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1272))));}
break;case 466: _LL3E8:{union Cyc_YYSTYPE_union _tmp1273;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1273.YY3).tag=7,(((_tmp1273.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1273))));}
break;case 467: _LL3E9:{union Cyc_YYSTYPE_union _tmp1274;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1274.YY3).tag=7,(((_tmp1274.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1274))));}
break;case 468: _LL3EA:{struct _dyneither_ptr*_tmp1277;union Cyc_YYSTYPE_union
_tmp1276;yyval=(union Cyc_YYSTYPE_union)(((_tmp1276.YY3).tag=7,(((_tmp1276.YY3).f1=
Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmp1277=_cycalloc(sizeof(*_tmp1277)),((_tmp1277[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1277)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1276))));}
break;case 469: _LL3EB:{struct _dyneither_ptr*_tmp127A;union Cyc_YYSTYPE_union
_tmp1279;yyval=(union Cyc_YYSTYPE_union)(((_tmp1279.YY3).tag=7,(((_tmp1279.YY3).f1=
Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmp127A=_cycalloc(sizeof(*_tmp127A)),((_tmp127A[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp127A)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1279))));}
break;case 470: _LL3EC:{union Cyc_YYSTYPE_union _tmp127B;yyval=(union Cyc_YYSTYPE_union)(((
_tmp127B.YY3).tag=7,(((_tmp127B.YY3).f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp127B))));}
break;case 471: _LL3ED:{union Cyc_YYSTYPE_union _tmp127C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp127C.YY3).tag=7,(((_tmp127C.YY3).f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp127C))));}
break;case 472: _LL3EE:{struct Cyc_Absyn_CompoundLit_e_struct*_tmp1282;struct Cyc_Absyn_CompoundLit_e_struct
_tmp1281;union Cyc_YYSTYPE_union _tmp1280;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1280.YY3).tag=7,(((_tmp1280.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp1282=
_cycalloc(sizeof(*_tmp1282)),((_tmp1282[0]=((_tmp1281.tag=27,((_tmp1281.f1=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmp1281.f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),_tmp1281)))))),
_tmp1282)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1280))));}break;case 473: _LL3EF:{struct Cyc_Absyn_CompoundLit_e_struct*
_tmp1288;struct Cyc_Absyn_CompoundLit_e_struct _tmp1287;union Cyc_YYSTYPE_union
_tmp1286;yyval=(union Cyc_YYSTYPE_union)(((_tmp1286.YY3).tag=7,(((_tmp1286.YY3).f1=
Cyc_Absyn_new_exp((void*)((_tmp1288=_cycalloc(sizeof(*_tmp1288)),((_tmp1288[0]=((
_tmp1287.tag=27,((_tmp1287.f1=Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]),((_tmp1287.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),_tmp1287)))))),_tmp1288)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1286))));}
break;case 474: _LL3F0:{struct Cyc_Absyn_UnknownId_e_struct*_tmp128E;struct Cyc_Absyn_UnknownId_e_struct
_tmp128D;union Cyc_YYSTYPE_union _tmp128C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp128C.YY3).tag=7,(((_tmp128C.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp128E=
_cycalloc(sizeof(*_tmp128E)),((_tmp128E[0]=((_tmp128D.tag=2,((_tmp128D.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp128D)))),_tmp128E)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp128C))));}break;case 475: _LL3F1: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 476: _LL3F2:{union
Cyc_YYSTYPE_union _tmp128F;yyval=(union Cyc_YYSTYPE_union)(((_tmp128F.YY3).tag=7,(((
_tmp128F.YY3).f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp128F))));}
break;case 477: _LL3F3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 478: _LL3F4:{union Cyc_YYSTYPE_union _tmp1290;yyval=(
union Cyc_YYSTYPE_union)(((_tmp1290.YY3).tag=7,(((_tmp1290.YY3).f1=Cyc_Absyn_noinstantiate_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1290))));}
break;case 479: _LL3F5:{union Cyc_YYSTYPE_union _tmp1291;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1291.YY3).tag=7,(((_tmp1291.YY3).f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1291))));}
break;case 480: _LL3F6:{union Cyc_YYSTYPE_union _tmp1292;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1292.YY3).tag=7,(((_tmp1292.YY3).f1=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1292))));}
break;case 481: _LL3F7:{struct Cyc_Absyn_Aggregate_e_struct*_tmp1298;struct Cyc_Absyn_Aggregate_e_struct
_tmp1297;union Cyc_YYSTYPE_union _tmp1296;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1296.YY3).tag=7,(((_tmp1296.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp1298=
_cycalloc(sizeof(*_tmp1298)),((_tmp1298[0]=((_tmp1297.tag=30,((_tmp1297.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmp1297.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp1297.f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),((_tmp1297.f4=0,
_tmp1297)))))))))),_tmp1298)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1296))));}
break;case 482: _LL3F8:{union Cyc_YYSTYPE_union _tmp1299;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1299.YY3).tag=7,(((_tmp1299.YY3).f1=Cyc_Absyn_stmt_exp(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1299))));}
break;case 483: _LL3F9:{union Cyc_YYSTYPE_union _tmp129A;yyval=(union Cyc_YYSTYPE_union)(((
_tmp129A.YY5).tag=9,(((_tmp129A.YY5).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])),_tmp129A))));}break;case 484: _LL3FA:{struct Cyc_List_List*
_tmp129D;union Cyc_YYSTYPE_union _tmp129C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp129C.YY5).tag=9,(((_tmp129C.YY5).f1=((_tmp129D=_cycalloc(sizeof(*_tmp129D)),((
_tmp129D->hd=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmp129D->tl=0,_tmp129D)))))),_tmp129C))));}break;case 485:
_LL3FB:{struct Cyc_List_List*_tmp12A0;union Cyc_YYSTYPE_union _tmp129F;yyval=(union
Cyc_YYSTYPE_union)(((_tmp129F.YY5).tag=9,(((_tmp129F.YY5).f1=((_tmp12A0=
_cycalloc(sizeof(*_tmp12A0)),((_tmp12A0->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp12A0->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp12A0)))))),
_tmp129F))));}break;case 486: _LL3FC:{union Cyc_YYSTYPE_union _tmp12A1;yyval=(union
Cyc_YYSTYPE_union)(((_tmp12A1.YY3).tag=7,(((_tmp12A1.YY3).f1=Cyc_Absyn_int_exp((
Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(
Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp12A1))));}break;case 487: _LL3FD:{union Cyc_YYSTYPE_union
_tmp12A2;yyval=(union Cyc_YYSTYPE_union)(((_tmp12A2.YY3).tag=7,(((_tmp12A2.YY3).f1=
Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp12A2))));}break;case 488: _LL3FE:{union Cyc_YYSTYPE_union
_tmp12A3;yyval=(union Cyc_YYSTYPE_union)(((_tmp12A3.YY3).tag=7,(((_tmp12A3.YY3).f1=
Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A3))));}
break;case 489: _LL3FF:{union Cyc_YYSTYPE_union _tmp12A4;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12A4.YY3).tag=7,(((_tmp12A4.YY3).f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A4))));}
break;case 490: _LL400:{struct _tuple1*_tmp12AD;struct _dyneither_ptr*_tmp12AC;union
Cyc_Absyn_Nmspace_union _tmp12AB;union Cyc_YYSTYPE_union _tmp12AA;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12AA.QualId_tok).tag=4,(((_tmp12AA.QualId_tok).f1=((_tmp12AD=_cycalloc(
sizeof(*_tmp12AD)),((_tmp12AD->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp12AB.Rel_n).tag=
1,(((_tmp12AB.Rel_n).f1=0,_tmp12AB)))),((_tmp12AD->f2=((_tmp12AC=_cycalloc(
sizeof(*_tmp12AC)),((_tmp12AC[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp12AC)))),_tmp12AD)))))),
_tmp12AA))));}break;case 491: _LL401: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 492: _LL402:{struct _tuple1*_tmp12B6;struct
_dyneither_ptr*_tmp12B5;union Cyc_Absyn_Nmspace_union _tmp12B4;union Cyc_YYSTYPE_union
_tmp12B3;yyval=(union Cyc_YYSTYPE_union)(((_tmp12B3.QualId_tok).tag=4,(((_tmp12B3.QualId_tok).f1=((
_tmp12B6=_cycalloc(sizeof(*_tmp12B6)),((_tmp12B6->f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp12B4.Rel_n).tag=1,(((_tmp12B4.Rel_n).f1=0,_tmp12B4)))),((_tmp12B6->f2=((
_tmp12B5=_cycalloc(sizeof(*_tmp12B5)),((_tmp12B5[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp12B5)))),_tmp12B6)))))),
_tmp12B3))));}break;case 493: _LL403: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 494: _LL404: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 495: _LL405: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 496: _LL406:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 497:
_LL407: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
498: _LL408: break;case 499: _LL409: yylex_buf->lex_curr_pos -=1;break;default: _LL40A:
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
_check_known_subscript_notnull(142,yyn - 138)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
6016) && Cyc_yycheck[_check_known_subscript_notnull(6017,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(6017,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(142,yyn - 138)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1008,yystate)];if(yyn > - 32768  && yyn < 6016){int
sze=0;struct _dyneither_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 280 / 
sizeof(char*);++ x){if(Cyc_yycheck[_check_known_subscript_notnull(6017,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(280,x)])+ 15,count ++);}{unsigned int _tmp12BB;
unsigned int _tmp12BA;struct _dyneither_ptr _tmp12B9;char*_tmp12B8;unsigned int
_tmp12B7;msg=((_tmp12B7=(unsigned int)(sze + 15),((_tmp12B8=(char*)_region_malloc(
yyregion,_check_times(sizeof(char),_tmp12B7 + 1)),((_tmp12B9=_tag_dyneither(
_tmp12B8,sizeof(char),_tmp12B7 + 1),((((_tmp12BA=_tmp12B7,((_tmp12BC(& _tmp12BB,&
_tmp12BA,& _tmp12B8),_tmp12B8[_tmp12BA]=(char)0)))),_tmp12B9))))))));}{const char*
_tmp12BD;Cyc_strcpy(msg,((_tmp12BD="parse error",_tag_dyneither(_tmp12BD,sizeof(
char),12))));}if(count < 5){count=0;for(x=yyn < 0?- yyn: 0;x < 280 / sizeof(char*);++ x){
if(Cyc_yycheck[_check_known_subscript_notnull(6017,x + yyn)]== x){{const char*
_tmp12BF;const char*_tmp12BE;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(
struct _dyneither_ptr)((_tmp12BF=", expecting `",_tag_dyneither(_tmp12BF,sizeof(
char),14))):(struct _dyneither_ptr)((_tmp12BE=" or `",_tag_dyneither(_tmp12BE,
sizeof(char),6)))));}Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(280,x)]);{const char*_tmp12C0;Cyc_strcat(msg,((
_tmp12C0="'",_tag_dyneither(_tmp12C0,sizeof(char),2))));}++ count;}}}Cyc_yyerror((
struct _dyneither_ptr)msg);}else{const char*_tmp12C1;Cyc_yyerror(((_tmp12C1="parse error",
_tag_dyneither(_tmp12C1,sizeof(char),12))));}}goto yyerrlab1;yyerrlab1: if(
yyerrstatus == 3){if(Cyc_yychar == 0){int _tmp8B9=1;_npop_handler(0);return _tmp8B9;}
Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(
yyssp_offset == 0){int _tmp8BA=1;_npop_handler(0);return _tmp8BA;}-- yyvsp_offset;
yystate=(int)yyss[_check_known_subscript_notnull(10000,-- yyssp_offset)];--
yylsp_offset;yyerrhandle: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1008,
yystate)];if(yyn == - 32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6016) || 
Cyc_yycheck[_check_known_subscript_notnull(6017,yyn)]!= 1)goto yyerrdefault;yyn=(
int)Cyc_yytable[_check_known_subscript_notnull(6017,yyn)];if(yyn < 0){if(yyn == -
32768)goto yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 
1007){int _tmp8BB=0;_npop_handler(0);return _tmp8BB;}yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto yynewstate;};
_pop_region(yyregion);}void Cyc_yyprint(int i,union Cyc_YYSTYPE_union v);void Cyc_yyprint(
int i,union Cyc_YYSTYPE_union v){union Cyc_YYSTYPE_union _tmp8C5=v;struct _tuple11
_tmp8C6;int _tmp8C7;char _tmp8C8;struct _dyneither_ptr _tmp8C9;struct _tuple1*_tmp8CA;
struct _tuple1 _tmp8CB;union Cyc_Absyn_Nmspace_union _tmp8CC;struct _dyneither_ptr*
_tmp8CD;_LL40D: if((_tmp8C5.Int_tok).tag != 0)goto _LL40F;_tmp8C6=(_tmp8C5.Int_tok).f1;
_tmp8C7=_tmp8C6.f2;_LL40E:{const char*_tmp12C5;void*_tmp12C4[1];struct Cyc_Int_pa_struct
_tmp12C3;(_tmp12C3.tag=1,((_tmp12C3.f1=(unsigned long)_tmp8C7,((_tmp12C4[0]=&
_tmp12C3,Cyc_fprintf(Cyc_stderr,((_tmp12C5="%d",_tag_dyneither(_tmp12C5,sizeof(
char),3))),_tag_dyneither(_tmp12C4,sizeof(void*),1)))))));}goto _LL40C;_LL40F: if((
_tmp8C5.Char_tok).tag != 1)goto _LL411;_tmp8C8=(_tmp8C5.Char_tok).f1;_LL410:{const
char*_tmp12C9;void*_tmp12C8[1];struct Cyc_Int_pa_struct _tmp12C7;(_tmp12C7.tag=1,((
_tmp12C7.f1=(unsigned long)((int)_tmp8C8),((_tmp12C8[0]=& _tmp12C7,Cyc_fprintf(
Cyc_stderr,((_tmp12C9="%c",_tag_dyneither(_tmp12C9,sizeof(char),3))),
_tag_dyneither(_tmp12C8,sizeof(void*),1)))))));}goto _LL40C;_LL411: if((_tmp8C5.String_tok).tag
!= 2)goto _LL413;_tmp8C9=(_tmp8C5.String_tok).f1;_LL412:{const char*_tmp12CD;void*
_tmp12CC[1];struct Cyc_String_pa_struct _tmp12CB;(_tmp12CB.tag=0,((_tmp12CB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp8C9),((_tmp12CC[0]=& _tmp12CB,Cyc_fprintf(
Cyc_stderr,((_tmp12CD="\"%s\"",_tag_dyneither(_tmp12CD,sizeof(char),5))),
_tag_dyneither(_tmp12CC,sizeof(void*),1)))))));}goto _LL40C;_LL413: if((_tmp8C5.QualId_tok).tag
!= 4)goto _LL415;_tmp8CA=(_tmp8C5.QualId_tok).f1;_tmp8CB=*_tmp8CA;_tmp8CC=_tmp8CB.f1;
_tmp8CD=_tmp8CB.f2;_LL414: {struct Cyc_List_List*_tmp8D7=0;{union Cyc_Absyn_Nmspace_union
_tmp8D8=_tmp8CC;struct Cyc_List_List*_tmp8D9;struct Cyc_List_List*_tmp8DA;_LL418:
if((_tmp8D8.Rel_n).tag != 1)goto _LL41A;_tmp8D9=(_tmp8D8.Rel_n).f1;_LL419: _tmp8D7=
_tmp8D9;goto _LL417;_LL41A: if((_tmp8D8.Abs_n).tag != 2)goto _LL41C;_tmp8DA=(_tmp8D8.Abs_n).f1;
_LL41B: _tmp8D7=_tmp8DA;goto _LL417;_LL41C: if((_tmp8D8.Loc_n).tag != 0)goto _LL417;
_LL41D: goto _LL417;_LL417:;}for(0;_tmp8D7 != 0;_tmp8D7=_tmp8D7->tl){const char*
_tmp12D1;void*_tmp12D0[1];struct Cyc_String_pa_struct _tmp12CF;(_tmp12CF.tag=0,((
_tmp12CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)
_tmp8D7->hd)),((_tmp12D0[0]=& _tmp12CF,Cyc_fprintf(Cyc_stderr,((_tmp12D1="%s::",
_tag_dyneither(_tmp12D1,sizeof(char),5))),_tag_dyneither(_tmp12D0,sizeof(void*),
1)))))));}{const char*_tmp12D5;void*_tmp12D4[1];struct Cyc_String_pa_struct
_tmp12D3;(_tmp12D3.tag=0,((_tmp12D3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp8CD),((_tmp12D4[0]=& _tmp12D3,Cyc_fprintf(Cyc_stderr,((
_tmp12D5="%s::",_tag_dyneither(_tmp12D5,sizeof(char),5))),_tag_dyneither(
_tmp12D4,sizeof(void*),1)))))));}goto _LL40C;}_LL415:;_LL416:{const char*_tmp12D8;
void*_tmp12D7;(_tmp12D7=0,Cyc_fprintf(Cyc_stderr,((_tmp12D8="?",_tag_dyneither(
_tmp12D8,sizeof(char),2))),_tag_dyneither(_tmp12D7,sizeof(void*),0)));}goto
_LL40C;_LL40C:;}struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){Cyc_Parse_parse_result=
0;Cyc_yyparse(Cyc_Lexing_from_file(f));return Cyc_Parse_parse_result;}
