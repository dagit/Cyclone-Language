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
extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void Cyc_List_iter_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_List_List*Cyc_List_filter_c(
int(*f)(void*,void*),void*env,struct Cyc_List_List*x);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dyneither_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};
extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{int tag;};
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct
Loc_n;struct Cyc_Absyn_Rel_n_struct Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};
struct _tuple0{union Cyc_Absyn_Nmspace_union f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Conref;
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
_tuple0*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};union Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct
UnknownTunion;struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{
union Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*tunion_name;struct
_tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int
tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};union Cyc_Absyn_TunionFieldInfoU_union{
struct Cyc_Absyn_UnknownTunionfield_struct UnknownTunionfield;struct Cyc_Absyn_KnownTunionfield_struct
KnownTunionfield;};struct Cyc_Absyn_TunionFieldInfo{union Cyc_Absyn_TunionFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*
f1;};struct Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
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
_tuple0*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;
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
f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Struct_e_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tunion_e_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*
f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;
};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};
struct _tuple2{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple2 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple2 f2;struct _tuple2 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple2 f2;};struct Cyc_Absyn_TryCatch_s_struct{
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple0*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple0*,struct _tuple0*);struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();void*Cyc_Absyn_compress_kb(void*);
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_string_typ(
void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct Cyc___cycFILE;struct
Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr
f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;struct _dyneither_ptr Cyc_Absynpp_typ2string(
void*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,
int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,
struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);
extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d);int Cyc_Dict_member(
struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d);struct
_tuple3{void*f1;void*f2;};struct _tuple3*Cyc_Dict_rchoose(struct _RegionHandle*r,
struct Cyc_Dict_Dict d);struct _tuple3*Cyc_Dict_rchoose(struct _RegionHandle*,struct
Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*,int(*
f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
le;int allow_valueof;};struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct
_RegionHandle*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _dyneither_ptr*,struct
Cyc_List_List*);struct Cyc_Absyn_Tuniondecl***Cyc_Tcenv_lookup_xtuniondecl(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple0*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*
te);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void
Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);int
Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int
Cyc_Tcutil_is_function_type(void*t);void*Cyc_Tcutil_kind_to_bound(void*k);void
Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*
k,int allow_evars,void*);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,
struct Cyc_List_List*);int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_supports_default(
void*);int Cyc_Tcutil_extract_const_from_typedef(struct Cyc_Position_Segment*,int
declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*
t,struct Cyc_List_List*atts);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Stmt*s,int new_block);struct _tuple4{unsigned int f1;int f2;};struct
_tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);void Cyc_Tc_tc(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*
ds);struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Tuniondecl*);void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Enumdecl*);extern char Cyc_Tcdecl_Incompatible[17];struct Cyc_Tcdecl_Xtunionfielddecl{
struct Cyc_Absyn_Tuniondecl*base;struct Cyc_Absyn_Tunionfield*field;};struct Cyc_Absyn_Aggrdecl*
Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Tuniondecl*
Cyc_Tcdecl_merge_tuniondecl(struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Enumdecl*
Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);void*Cyc_Tcdecl_merge_binding(
void*d0,void*d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct
Cyc_List_List*Cyc_Tcdecl_sort_xtunion_fields(struct Cyc_List_List*f,int*res,
struct _dyneither_ptr*v,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);
struct Cyc_Tcgenrep_RepInfo;struct Cyc_Dict_Dict Cyc_Tcgenrep_empty_typerep_dict();
struct _tuple5{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Exp*
f3;};struct _tuple5 Cyc_Tcgenrep_tcGenrep(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*
ge,struct Cyc_Position_Segment*loc,void*type,struct Cyc_Dict_Dict dict);static char
_tmp0[1]="";static struct _dyneither_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};
static struct _dyneither_ptr*Cyc_Tc_tc_msg=(struct _dyneither_ptr*)& Cyc_Tc_tc_msg_c;
struct _tuple6{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};static int
Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports);static int Cyc_Tc_export_member(
struct _tuple0*x,struct Cyc_List_List*exports){for(0;exports != 0;exports=exports->tl){
struct _tuple6*_tmp1=(struct _tuple6*)exports->hd;if(Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)
== 0){(*_tmp1).f3=1;return 1;}}return 0;}struct _tuple7{void*f1;int f2;};static void
Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int ignore_init,struct Cyc_List_List**
exports);static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*
ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,
int ignore_init,struct Cyc_List_List**exports){struct Cyc_Absyn_Vardecl _tmp3;void*
_tmp4;struct _tuple0*_tmp5;struct _tuple0 _tmp6;union Cyc_Absyn_Nmspace_union _tmp7;
struct _dyneither_ptr*_tmp8;void*_tmp9;struct Cyc_Absyn_Exp*_tmpA;struct Cyc_List_List*
_tmpB;struct Cyc_Absyn_Vardecl*_tmp2=vd;_tmp3=*_tmp2;_tmp4=(void*)_tmp3.sc;_tmp5=
_tmp3.name;_tmp6=*_tmp5;_tmp7=_tmp6.f1;_tmp8=_tmp6.f2;_tmp9=(void*)_tmp3.type;
_tmpA=_tmp3.initializer;_tmpB=_tmp3.attributes;{union Cyc_Absyn_Nmspace_union
_tmpC=_tmp7;struct Cyc_List_List*_tmpD;struct Cyc_List_List*_tmpE;_LL1: if((_tmpC.Rel_n).tag
!= 1)goto _LL3;_tmpD=(_tmpC.Rel_n).f1;if(_tmpD != 0)goto _LL3;_LL2: goto _LL4;_LL3:
if((_tmpC.Abs_n).tag != 2)goto _LL5;_tmpE=(_tmpC.Abs_n).f1;if(_tmpE != 0)goto _LL5;
_LL4: goto _LL0;_LL5:;_LL6:{const char*_tmp224;void*_tmp223[1];struct Cyc_String_pa_struct
_tmp222;(_tmp222.tag=0,((_tmp222.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name)),((_tmp223[0]=& _tmp222,Cyc_Tcutil_terr(loc,((
_tmp224="qualified variable declarations are not implemented (%s)",
_tag_dyneither(_tmp224,sizeof(char),57))),_tag_dyneither(_tmp223,sizeof(void*),1)))))));}
return;_LL0:;}{union Cyc_Absyn_Nmspace_union _tmp225;(*vd->name).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp225.Abs_n).tag=2,(((_tmp225.Abs_n).f1=te->ns,_tmp225))));}{void*_tmp13=Cyc_Tcutil_compress(
_tmp9);struct Cyc_Absyn_ArrayInfo _tmp14;void*_tmp15;struct Cyc_Absyn_Tqual _tmp16;
struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Conref*_tmp18;struct Cyc_Position_Segment*
_tmp19;_LL8: if(_tmp13 <= (void*)4)goto _LLA;if(*((int*)_tmp13)!= 7)goto _LLA;_tmp14=((
struct Cyc_Absyn_ArrayType_struct*)_tmp13)->f1;_tmp15=(void*)_tmp14.elt_type;
_tmp16=_tmp14.tq;_tmp17=_tmp14.num_elts;if(_tmp17 != 0)goto _LLA;_tmp18=_tmp14.zero_term;
_tmp19=_tmp14.zt_loc;if(!(_tmpA != 0))goto _LLA;_LL9:{void*_tmp1A=(void*)_tmpA->r;
union Cyc_Absyn_Cnst_union _tmp1B;struct _dyneither_ptr _tmp1C;struct Cyc_Absyn_Exp*
_tmp1D;struct Cyc_List_List*_tmp1E;struct Cyc_List_List*_tmp1F;_LLD: if(*((int*)
_tmp1A)!= 0)goto _LLF;_tmp1B=((struct Cyc_Absyn_Const_e_struct*)_tmp1A)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmp1A)->f1).String_c).tag != 5)goto _LLF;_tmp1C=(
_tmp1B.String_c).f1;_LLE: _tmp9=(void*)(vd->type=(void*)Cyc_Absyn_array_typ(
_tmp15,_tmp16,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(_get_dyneither_size(
_tmp1C,sizeof(char)),0),_tmp18,_tmp19));goto _LLC;_LLF: if(*((int*)_tmp1A)!= 29)
goto _LL11;_tmp1D=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1A)->f2;_LL10:
_tmp9=(void*)(vd->type=(void*)Cyc_Absyn_array_typ(_tmp15,_tmp16,(struct Cyc_Absyn_Exp*)
_tmp1D,_tmp18,_tmp19));goto _LLC;_LL11: if(*((int*)_tmp1A)!= 37)goto _LL13;_tmp1E=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1A)->f2;_LL12: _tmp1F=_tmp1E;goto
_LL14;_LL13: if(*((int*)_tmp1A)!= 28)goto _LL15;_tmp1F=((struct Cyc_Absyn_Array_e_struct*)
_tmp1A)->f1;_LL14: _tmp9=(void*)(vd->type=(void*)Cyc_Absyn_array_typ(_tmp15,
_tmp16,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp1F),0),_tmp18,_tmp19));goto _LLC;_LL15:;_LL16: goto _LLC;
_LLC:;}goto _LL7;_LLA:;_LLB: goto _LL7;_LL7:;}Cyc_Tcutil_check_valid_toplevel_type(
loc,te,_tmp9);(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,
_tmp9);{void*_tmp20=Cyc_Tcutil_compress(_tmp9);_LL18: if(_tmp20 <= (void*)4)goto
_LL1A;if(*((int*)_tmp20)!= 7)goto _LL1A;_LL19: vd->escapes=0;goto _LL17;_LL1A:;
_LL1B: vd->escapes=1;goto _LL17;_LL17:;}if(Cyc_Tcutil_is_function_type(_tmp9))
_tmpB=Cyc_Tcutil_transfer_fn_type_atts(_tmp9,_tmpB);if(_tmp4 == (void*)3  || _tmp4
== (void*)4){if(_tmpA != 0){const char*_tmp228;void*_tmp227;(_tmp227=0,Cyc_Tcutil_terr(
loc,((_tmp228="extern declaration should not have initializer",_tag_dyneither(
_tmp228,sizeof(char),47))),_tag_dyneither(_tmp227,sizeof(void*),0)));}}else{if(!
Cyc_Tcutil_is_function_type(_tmp9)){for(0;_tmpB != 0;_tmpB=_tmpB->tl){void*_tmp23=(
void*)_tmpB->hd;_LL1D: if(_tmp23 <= (void*)17)goto _LL21;if(*((int*)_tmp23)!= 1)
goto _LL1F;_LL1E: goto _LL20;_LL1F: if(*((int*)_tmp23)!= 2)goto _LL21;_LL20: goto _LL22;
_LL21: if((int)_tmp23 != 6)goto _LL23;_LL22: goto _LL24;_LL23: if((int)_tmp23 != 7)goto
_LL25;_LL24: goto _LL26;_LL25: if((int)_tmp23 != 8)goto _LL27;_LL26: goto _LL28;_LL27:
if((int)_tmp23 != 9)goto _LL29;_LL28: goto _LL2A;_LL29: if((int)_tmp23 != 10)goto _LL2B;
_LL2A: goto _LL2C;_LL2B: if((int)_tmp23 != 11)goto _LL2D;_LL2C: continue;_LL2D:;_LL2E:{
const char*_tmp22D;void*_tmp22C[2];struct Cyc_String_pa_struct _tmp22B;struct Cyc_String_pa_struct
_tmp22A;(_tmp22A.tag=0,((_tmp22A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name)),((_tmp22B.tag=0,((_tmp22B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmpB->hd)),((
_tmp22C[0]=& _tmp22B,((_tmp22C[1]=& _tmp22A,Cyc_Tcutil_terr(loc,((_tmp22D="bad attribute %s for variable %s",
_tag_dyneither(_tmp22D,sizeof(char),33))),_tag_dyneither(_tmp22C,sizeof(void*),2)))))))))))));}
goto _LL1C;_LL1C:;}if(_tmpA == 0  || ignore_init){if(check_var_init  && !Cyc_Tcutil_supports_default(
_tmp9)){const char*_tmp232;void*_tmp231[2];struct Cyc_String_pa_struct _tmp230;
struct Cyc_String_pa_struct _tmp22F;(_tmp22F.tag=0,((_tmp22F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp9)),((_tmp230.tag=
0,((_tmp230.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
vd->name)),((_tmp231[0]=& _tmp230,((_tmp231[1]=& _tmp22F,Cyc_Tcutil_terr(loc,((
_tmp232="initializer required for variable %s of type %s",_tag_dyneither(_tmp232,
sizeof(char),48))),_tag_dyneither(_tmp231,sizeof(void*),2)))))))))))));}}else{
struct Cyc_Absyn_Exp*_tmp2C=(struct Cyc_Absyn_Exp*)_tmpA;void*_tmp2D=Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp9,_tmp2C);if(!Cyc_Tcutil_coerce_assign(te,_tmp2C,_tmp9)){{const
char*_tmp238;void*_tmp237[3];struct Cyc_String_pa_struct _tmp236;struct Cyc_String_pa_struct
_tmp235;struct Cyc_String_pa_struct _tmp234;(_tmp234.tag=0,((_tmp234.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2D)),((_tmp235.tag=
0,((_tmp235.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp9)),((_tmp236.tag=0,((_tmp236.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp237[0]=& _tmp236,((
_tmp237[1]=& _tmp235,((_tmp237[2]=& _tmp234,Cyc_Tcutil_terr(loc,((_tmp238="%s declared with type \n%s\n but initialized with type \n%s",
_tag_dyneither(_tmp238,sizeof(char),57))),_tag_dyneither(_tmp237,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}if(!Cyc_Tcutil_is_const_exp(te,_tmp2C)){const char*
_tmp23B;void*_tmp23A;(_tmp23A=0,Cyc_Tcutil_terr(loc,((_tmp23B="initializer is not a constant expression",
_tag_dyneither(_tmp23B,sizeof(char),41))),_tag_dyneither(_tmp23A,sizeof(void*),0)));}}}
else{for(0;_tmpB != 0;_tmpB=_tmpB->tl){void*_tmp35=(void*)_tmpB->hd;_LL30: if(
_tmp35 <= (void*)17)goto _LL32;if(*((int*)_tmp35)!= 0)goto _LL32;_LL31: goto _LL33;
_LL32: if((int)_tmp35 != 0)goto _LL34;_LL33: goto _LL35;_LL34: if((int)_tmp35 != 1)goto
_LL36;_LL35: goto _LL37;_LL36: if((int)_tmp35 != 2)goto _LL38;_LL37: goto _LL39;_LL38:
if((int)_tmp35 != 3)goto _LL3A;_LL39: goto _LL3B;_LL3A: if(_tmp35 <= (void*)17)goto
_LL3E;if(*((int*)_tmp35)!= 3)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if(*((int*)_tmp35)
!= 4)goto _LL3E;_LL3D: goto _LL3F;_LL3E: if((int)_tmp35 != 16)goto _LL40;_LL3F: goto
_LL41;_LL40: if((int)_tmp35 != 4)goto _LL42;_LL41: {const char*_tmp23E;void*_tmp23D;(
_tmp23D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp23E="tcVardecl: fn type atts in function var decl",_tag_dyneither(_tmp23E,
sizeof(char),45))),_tag_dyneither(_tmp23D,sizeof(void*),0)));}_LL42: if(_tmp35 <= (
void*)17)goto _LL44;if(*((int*)_tmp35)!= 1)goto _LL44;_LL43: goto _LL45;_LL44: if((
int)_tmp35 != 5)goto _LL46;_LL45:{const char*_tmp242;void*_tmp241[1];struct Cyc_String_pa_struct
_tmp240;(_tmp240.tag=0,((_tmp240.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)_tmpB->hd)),((_tmp241[0]=& _tmp240,Cyc_Tcutil_terr(
loc,((_tmp242="bad attribute %s in function declaration",_tag_dyneither(_tmp242,
sizeof(char),41))),_tag_dyneither(_tmp241,sizeof(void*),1)))))));}goto _LL2F;
_LL46:;_LL47: continue;_LL2F:;}}}{struct _handler_cons _tmp3B;_push_handler(& _tmp3B);{
int _tmp3D=0;if(setjmp(_tmp3B.handler))_tmp3D=1;if(!_tmp3D){{struct _tuple7*_tmp3E=((
struct _tuple7*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
ge->ordinaries,_tmp8);void*_tmp3F=(*_tmp3E).f1;void*_tmp40;_LL49: if(*((int*)
_tmp3F)!= 0)goto _LL4B;_tmp40=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp3F)->f1;
_LL4A: {struct Cyc_Absyn_Global_b_struct _tmp245;struct Cyc_Absyn_Global_b_struct*
_tmp244;struct Cyc_Absyn_Global_b_struct*_tmp41=(_tmp244=_cycalloc(sizeof(*
_tmp244)),((_tmp244[0]=((_tmp245.tag=0,((_tmp245.f1=vd,_tmp245)))),_tmp244)));
void*_tmp42=Cyc_Tcdecl_merge_binding(_tmp40,(void*)_tmp41,loc,Cyc_Tc_tc_msg);if(
_tmp42 == (void*)0){_npop_handler(0);return;}if(_tmp42 == _tmp40  && (*_tmp3E).f2){
_npop_handler(0);return;}if(exports == 0  || Cyc_Tc_export_member(vd->name,*
exports)){struct Cyc_Tcenv_VarRes_struct*_tmp24B;struct Cyc_Tcenv_VarRes_struct
_tmp24A;struct _tuple7*_tmp249;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple7*v))Cyc_Dict_insert)(ge->ordinaries,_tmp8,(
struct _tuple7*)((_tmp249=_cycalloc(sizeof(*_tmp249)),((_tmp249->f1=(void*)((
_tmp24B=_cycalloc(sizeof(*_tmp24B)),((_tmp24B[0]=((_tmp24A.tag=0,((_tmp24A.f1=(
void*)_tmp42,_tmp24A)))),_tmp24B)))),((_tmp249->f2=1,_tmp249)))))));}
_npop_handler(0);return;}_LL4B: if(*((int*)_tmp3F)!= 1)goto _LL4D;_LL4C:{const char*
_tmp24E;void*_tmp24D;(_tmp24D=0,Cyc_Tcutil_warn(loc,((_tmp24E="variable declaration shadows previous struct declaration",
_tag_dyneither(_tmp24E,sizeof(char),57))),_tag_dyneither(_tmp24D,sizeof(void*),0)));}
goto _LL48;_LL4D: if(*((int*)_tmp3F)!= 2)goto _LL4F;_LL4E:{const char*_tmp251;void*
_tmp250;(_tmp250=0,Cyc_Tcutil_warn(loc,((_tmp251="variable declaration shadows previous [x]tunion constructor",
_tag_dyneither(_tmp251,sizeof(char),60))),_tag_dyneither(_tmp250,sizeof(void*),0)));}
goto _LL48;_LL4F: if(*((int*)_tmp3F)!= 4)goto _LL51;_LL50: goto _LL52;_LL51: if(*((int*)
_tmp3F)!= 3)goto _LL48;_LL52:{const char*_tmp254;void*_tmp253;(_tmp253=0,Cyc_Tcutil_warn(
loc,((_tmp254="variable declaration shadows previous enum tag",_tag_dyneither(
_tmp254,sizeof(char),47))),_tag_dyneither(_tmp253,sizeof(void*),0)));}goto _LL48;
_LL48:;};_pop_handler();}else{void*_tmp3C=(void*)_exn_thrown;void*_tmp4F=_tmp3C;
_LL54: if(_tmp4F != Cyc_Dict_Absent)goto _LL56;_LL55: goto _LL53;_LL56:;_LL57:(void)
_throw(_tmp4F);_LL53:;}}}if(exports == 0  || Cyc_Tc_export_member(vd->name,*
exports)){struct Cyc_Tcenv_VarRes_struct*_tmp263;struct Cyc_Absyn_Global_b_struct*
_tmp262;struct Cyc_Absyn_Global_b_struct _tmp261;struct Cyc_Tcenv_VarRes_struct
_tmp260;struct _tuple7*_tmp25F;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple7*v))Cyc_Dict_insert)(ge->ordinaries,_tmp8,(
struct _tuple7*)((_tmp25F=_cycalloc(sizeof(*_tmp25F)),((_tmp25F->f1=(void*)((
_tmp263=_cycalloc(sizeof(*_tmp263)),((_tmp263[0]=((_tmp260.tag=0,((_tmp260.f1=(
void*)((void*)((_tmp262=_cycalloc(sizeof(*_tmp262)),((_tmp262[0]=((_tmp261.tag=0,((
_tmp261.f1=vd,_tmp261)))),_tmp262))))),_tmp260)))),_tmp263)))),((_tmp25F->f2=0,
_tmp25F)))))));}}static int Cyc_Tc_is_main(struct _tuple0*n);static int Cyc_Tc_is_main(
struct _tuple0*n){struct _tuple0 _tmp56;union Cyc_Absyn_Nmspace_union _tmp57;struct
_dyneither_ptr*_tmp58;struct _tuple0*_tmp55=n;_tmp56=*_tmp55;_tmp57=_tmp56.f1;
_tmp58=_tmp56.f2;{union Cyc_Absyn_Nmspace_union _tmp59=_tmp57;struct Cyc_List_List*
_tmp5A;_LL59: if((_tmp59.Abs_n).tag != 2)goto _LL5B;_tmp5A=(_tmp59.Abs_n).f1;if(
_tmp5A != 0)goto _LL5B;_LL5A: {const char*_tmp264;return Cyc_strcmp((struct
_dyneither_ptr)*_tmp58,((_tmp264="main",_tag_dyneither(_tmp264,sizeof(char),5))))
== 0;}_LL5B:;_LL5C: return 0;_LL58:;}}struct _tuple8{struct _dyneither_ptr*f1;struct
Cyc_Absyn_Tqual f2;void*f3;};static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Fndecl*fd,
int ignore_body,struct Cyc_List_List**exports);static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Fndecl*
fd,int ignore_body,struct Cyc_List_List**exports){struct _dyneither_ptr*v=(*fd->name).f2;
if((void*)fd->sc == (void*)4  && !ignore_body){const char*_tmp267;void*_tmp266;(
_tmp266=0,Cyc_Tcutil_terr(loc,((_tmp267="extern \"C\" functions cannot be implemented in Cyclone",
_tag_dyneither(_tmp267,sizeof(char),54))),_tag_dyneither(_tmp266,sizeof(void*),0)));}{
union Cyc_Absyn_Nmspace_union _tmp5E=(*fd->name).f1;struct Cyc_List_List*_tmp5F;
struct Cyc_List_List*_tmp60;_LL5E: if((_tmp5E.Rel_n).tag != 1)goto _LL60;_tmp5F=(
_tmp5E.Rel_n).f1;if(_tmp5F != 0)goto _LL60;_LL5F: goto _LL5D;_LL60: if((_tmp5E.Abs_n).tag
!= 2)goto _LL62;_tmp60=(_tmp5E.Abs_n).f1;_LL61: {const char*_tmp26A;void*_tmp269;(
_tmp269=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp26A="tc: Abs_n in tcFndecl",_tag_dyneither(_tmp26A,sizeof(char),22))),
_tag_dyneither(_tmp269,sizeof(void*),0)));}_LL62:;_LL63:{const char*_tmp26E;void*
_tmp26D[1];struct Cyc_String_pa_struct _tmp26C;(_tmp26C.tag=0,((_tmp26C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(fd->name)),((
_tmp26D[0]=& _tmp26C,Cyc_Tcutil_terr(loc,((_tmp26E="qualified function declarations are not implemented (%s)",
_tag_dyneither(_tmp26E,sizeof(char),57))),_tag_dyneither(_tmp26D,sizeof(void*),1)))))));}
return;_LL5D:;}{union Cyc_Absyn_Nmspace_union _tmp26F;(*fd->name).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp26F.Abs_n).tag=2,(((_tmp26F.Abs_n).f1=te->ns,_tmp26F))));}Cyc_Tcutil_check_fndecl_valid_type(
loc,te,fd);{void*t=Cyc_Tcutil_fndecl2typ(fd);fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(
t,fd->attributes);{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=
atts->tl){void*_tmp67=(void*)atts->hd;_LL65: if((int)_tmp67 != 5)goto _LL67;_LL66:
goto _LL68;_LL67: if(_tmp67 <= (void*)17)goto _LL69;if(*((int*)_tmp67)!= 1)goto _LL69;
_LL68:{const char*_tmp273;void*_tmp272[1];struct Cyc_String_pa_struct _tmp271;(
_tmp271.tag=0,((_tmp271.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)atts->hd)),((_tmp272[0]=& _tmp271,Cyc_Tcutil_terr(loc,((_tmp273="bad attribute %s for function",
_tag_dyneither(_tmp273,sizeof(char),30))),_tag_dyneither(_tmp272,sizeof(void*),1)))))));}
goto _LL64;_LL69:;_LL6A: goto _LL64;_LL64:;}}{struct _handler_cons _tmp6B;
_push_handler(& _tmp6B);{int _tmp6D=0;if(setjmp(_tmp6B.handler))_tmp6D=1;if(!
_tmp6D){{struct _tuple7*_tmp6E=((struct _tuple7*(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,v);void*_tmp6F=(*_tmp6E).f1;
void*_tmp70;_LL6C: if(*((int*)_tmp6F)!= 0)goto _LL6E;_tmp70=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmp6F)->f1;_LL6D: {struct Cyc_Absyn_Funname_b_struct _tmp276;struct Cyc_Absyn_Funname_b_struct*
_tmp275;struct Cyc_Absyn_Funname_b_struct*_tmp71=(_tmp275=_cycalloc(sizeof(*
_tmp275)),((_tmp275[0]=((_tmp276.tag=1,((_tmp276.f1=fd,_tmp276)))),_tmp275)));
void*_tmp72=Cyc_Tcdecl_merge_binding(_tmp70,(void*)_tmp71,loc,Cyc_Tc_tc_msg);if(
_tmp72 == (void*)0)goto _LL6B;if(_tmp72 == _tmp70  && (*_tmp6E).f2)goto _LL6B;if(
exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){struct Cyc_Tcenv_VarRes_struct*
_tmp27C;struct Cyc_Tcenv_VarRes_struct _tmp27B;struct _tuple7*_tmp27A;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple7*
v))Cyc_Dict_insert)(ge->ordinaries,v,(struct _tuple7*)((_tmp27A=_cycalloc(sizeof(*
_tmp27A)),((_tmp27A->f1=(void*)((_tmp27C=_cycalloc(sizeof(*_tmp27C)),((_tmp27C[0]=((
_tmp27B.tag=0,((_tmp27B.f1=(void*)_tmp72,_tmp27B)))),_tmp27C)))),((_tmp27A->f2=1,
_tmp27A)))))));}goto _LL6B;}_LL6E: if(*((int*)_tmp6F)!= 1)goto _LL70;_LL6F:{const
char*_tmp27F;void*_tmp27E;(_tmp27E=0,Cyc_Tcutil_warn(loc,((_tmp27F="function declaration shadows previous type declaration",
_tag_dyneither(_tmp27F,sizeof(char),55))),_tag_dyneither(_tmp27E,sizeof(void*),0)));}
goto _LL6B;_LL70: if(*((int*)_tmp6F)!= 2)goto _LL72;_LL71:{const char*_tmp282;void*
_tmp281;(_tmp281=0,Cyc_Tcutil_warn(loc,((_tmp282="function declaration shadows previous [x]tunion constructor",
_tag_dyneither(_tmp282,sizeof(char),60))),_tag_dyneither(_tmp281,sizeof(void*),0)));}
goto _LL6B;_LL72: if(*((int*)_tmp6F)!= 4)goto _LL74;_LL73: goto _LL75;_LL74: if(*((int*)
_tmp6F)!= 3)goto _LL6B;_LL75:{const char*_tmp285;void*_tmp284;(_tmp284=0,Cyc_Tcutil_warn(
loc,((_tmp285="function declaration shadows previous enum tag",_tag_dyneither(
_tmp285,sizeof(char),47))),_tag_dyneither(_tmp284,sizeof(void*),0)));}goto _LL6B;
_LL6B:;};_pop_handler();}else{void*_tmp6C=(void*)_exn_thrown;void*_tmp7F=_tmp6C;
_LL77: if(_tmp7F != Cyc_Dict_Absent)goto _LL79;_LL78: if(exports == 0  || Cyc_Tc_export_member(
fd->name,*exports)){struct Cyc_Tcenv_VarRes_struct*_tmp294;struct Cyc_Absyn_Funname_b_struct*
_tmp293;struct Cyc_Absyn_Funname_b_struct _tmp292;struct Cyc_Tcenv_VarRes_struct
_tmp291;struct _tuple7*_tmp290;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple7*v))Cyc_Dict_insert)(ge->ordinaries,v,(
struct _tuple7*)((_tmp290=_cycalloc(sizeof(*_tmp290)),((_tmp290->f1=(void*)((
_tmp294=_cycalloc(sizeof(*_tmp294)),((_tmp294[0]=((_tmp291.tag=0,((_tmp291.f1=(
void*)((void*)((_tmp293=_cycalloc(sizeof(*_tmp293)),((_tmp293[0]=((_tmp292.tag=1,((
_tmp292.f1=fd,_tmp292)))),_tmp293))))),_tmp291)))),_tmp294)))),((_tmp290->f2=0,
_tmp290)))))));}goto _LL76;_LL79:;_LL7A:(void)_throw(_tmp7F);_LL76:;}}}if(
ignore_body)return;{struct _RegionHandle _tmp85=_new_region("fnrgn");struct
_RegionHandle*fnrgn=& _tmp85;_push_region(fnrgn);{struct Cyc_Tcenv_Fenv*_tmp86=Cyc_Tcenv_new_fenv(
fnrgn,loc,fd);struct Cyc_Tcenv_Tenv*_tmp295;struct Cyc_Tcenv_Tenv*_tmp87=(_tmp295=
_region_malloc(fnrgn,sizeof(*_tmp295)),((_tmp295->ns=te->ns,((_tmp295->ae=te->ae,((
_tmp295->le=(struct Cyc_Tcenv_Fenv*)_tmp86,((_tmp295->allow_valueof=0,_tmp295)))))))));
Cyc_Tcstmt_tcStmt(_tmp87,fd->body,0);Cyc_Tcenv_check_delayed_effects(_tmp87);Cyc_Tcenv_check_delayed_constraints(
_tmp87);if(!Cyc_Tcenv_all_labels_resolved(_tmp87)){const char*_tmp298;void*
_tmp297;(_tmp297=0,Cyc_Tcutil_terr(loc,((_tmp298="function has goto statements to undefined labels",
_tag_dyneither(_tmp298,sizeof(char),49))),_tag_dyneither(_tmp297,sizeof(void*),0)));}};
_pop_region(fnrgn);}if(Cyc_Tc_is_main(fd->name)){{void*_tmp8B=Cyc_Tcutil_compress((
void*)fd->ret_type);void*_tmp8C;_LL7C: if((int)_tmp8B != 0)goto _LL7E;_LL7D:{const
char*_tmp29B;void*_tmp29A;(_tmp29A=0,Cyc_Tcutil_warn(loc,((_tmp29B="main declared with return type void",
_tag_dyneither(_tmp29B,sizeof(char),36))),_tag_dyneither(_tmp29A,sizeof(void*),0)));}
goto _LL7B;_LL7E: if(_tmp8B <= (void*)4)goto _LL80;if(*((int*)_tmp8B)!= 5)goto _LL80;
_tmp8C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp8B)->f2;_LL7F: goto _LL7B;
_LL80:;_LL81:{const char*_tmp29F;void*_tmp29E[1];struct Cyc_String_pa_struct
_tmp29D;(_tmp29D.tag=0,((_tmp29D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)fd->ret_type)),((_tmp29E[0]=& _tmp29D,Cyc_Tcutil_terr(
loc,((_tmp29F="main declared with return type %s instead of int or void",
_tag_dyneither(_tmp29F,sizeof(char),57))),_tag_dyneither(_tmp29E,sizeof(void*),1)))))));}
goto _LL7B;_LL7B:;}if(fd->c_varargs  || fd->cyc_varargs != 0){const char*_tmp2A2;
void*_tmp2A1;(_tmp2A1=0,Cyc_Tcutil_terr(loc,((_tmp2A2="main declared with varargs",
_tag_dyneither(_tmp2A2,sizeof(char),27))),_tag_dyneither(_tmp2A1,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp94=fd->args;if(_tmp94 != 0){struct _tuple8 _tmp96;void*
_tmp97;struct _tuple8*_tmp95=(struct _tuple8*)_tmp94->hd;_tmp96=*_tmp95;_tmp97=
_tmp96.f3;{void*_tmp98=Cyc_Tcutil_compress(_tmp97);void*_tmp99;_LL83: if(_tmp98 <= (
void*)4)goto _LL85;if(*((int*)_tmp98)!= 5)goto _LL85;_tmp99=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp98)->f2;_LL84: goto _LL82;_LL85:;_LL86:{const char*_tmp2A6;void*_tmp2A5[1];
struct Cyc_String_pa_struct _tmp2A4;(_tmp2A4.tag=0,((_tmp2A4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp97)),((_tmp2A5[
0]=& _tmp2A4,Cyc_Tcutil_terr(loc,((_tmp2A6="main declared with first argument of type %s instead of int",
_tag_dyneither(_tmp2A6,sizeof(char),60))),_tag_dyneither(_tmp2A5,sizeof(void*),1)))))));}
goto _LL82;_LL82:;}_tmp94=_tmp94->tl;if(_tmp94 != 0){struct _tuple8 _tmp9E;void*
_tmp9F;struct _tuple8*_tmp9D=(struct _tuple8*)_tmp94->hd;_tmp9E=*_tmp9D;_tmp9F=
_tmp9E.f3;_tmp94=_tmp94->tl;if(_tmp94 != 0){const char*_tmp2A9;void*_tmp2A8;(
_tmp2A8=0,Cyc_Tcutil_terr(loc,((_tmp2A9="main declared with too many arguments",
_tag_dyneither(_tmp2A9,sizeof(char),38))),_tag_dyneither(_tmp2A8,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmp2AA;struct Cyc_Core_Opt*tvs=(_tmp2AA=_cycalloc(sizeof(*
_tmp2AA)),((_tmp2AA->v=fd->tvs,_tmp2AA)));struct Cyc_Core_Opt*_tmp2B2;struct Cyc_Core_Opt*
_tmp2B1;struct Cyc_Core_Opt*_tmp2B0;struct Cyc_Core_Opt*_tmp2AF;struct Cyc_Core_Opt*
_tmp2AE;struct Cyc_Core_Opt*_tmp2AD;struct Cyc_Core_Opt*_tmp2AC;struct Cyc_Core_Opt*
_tmp2AB;if(((!Cyc_Tcutil_unify(_tmp9F,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(
Cyc_Absyn_new_evar(((_tmp2B1=_cycalloc(sizeof(*_tmp2B1)),((_tmp2B1->v=(void*)((
void*)3),_tmp2B1)))),tvs)),Cyc_Absyn_new_evar(((_tmp2B2=_cycalloc(sizeof(*
_tmp2B2)),((_tmp2B2->v=(void*)((void*)3),_tmp2B2)))),tvs),Cyc_Absyn_empty_tqual(
0),((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)())) && !Cyc_Tcutil_unify(
_tmp9F,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((
_tmp2AF=_cycalloc(sizeof(*_tmp2AF)),((_tmp2AF->v=(void*)((void*)3),_tmp2AF)))),
tvs)),Cyc_Absyn_new_evar(((_tmp2B0=_cycalloc(sizeof(*_tmp2B0)),((_tmp2B0->v=(
void*)((void*)3),_tmp2B0)))),tvs),Cyc_Absyn_empty_tqual(0),((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)()))) && !Cyc_Tcutil_unify(_tmp9F,Cyc_Absyn_dyneither_typ(
Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp2AD=_cycalloc(sizeof(*_tmp2AD)),((
_tmp2AD->v=(void*)((void*)3),_tmp2AD)))),tvs)),Cyc_Absyn_new_evar(((_tmp2AE=
_cycalloc(sizeof(*_tmp2AE)),((_tmp2AE->v=(void*)((void*)3),_tmp2AE)))),tvs),Cyc_Absyn_const_tqual(
0),((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)()))) && !Cyc_Tcutil_unify(
_tmp9F,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((
_tmp2AB=_cycalloc(sizeof(*_tmp2AB)),((_tmp2AB->v=(void*)((void*)3),_tmp2AB)))),
tvs)),Cyc_Absyn_new_evar(((_tmp2AC=_cycalloc(sizeof(*_tmp2AC)),((_tmp2AC->v=(
void*)((void*)3),_tmp2AC)))),tvs),Cyc_Absyn_const_tqual(0),((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)()))){const char*_tmp2B6;void*_tmp2B5[1];struct Cyc_String_pa_struct
_tmp2B4;(_tmp2B4.tag=0,((_tmp2B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp9F)),((_tmp2B5[0]=& _tmp2B4,Cyc_Tcutil_terr(loc,((
_tmp2B6="second argument of main has type %s instead of char??",_tag_dyneither(
_tmp2B6,sizeof(char),54))),_tag_dyneither(_tmp2B5,sizeof(void*),1)))))));}}}}}}}}
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*td);static void Cyc_Tc_tcTypedefdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Typedefdecl*td){struct _dyneither_ptr*v=(*td->name).f2;{union Cyc_Absyn_Nmspace_union
_tmpAE=(*td->name).f1;struct Cyc_List_List*_tmpAF;struct Cyc_List_List*_tmpB0;
_LL88: if((_tmpAE.Rel_n).tag != 1)goto _LL8A;_tmpAF=(_tmpAE.Rel_n).f1;if(_tmpAF != 0)
goto _LL8A;_LL89: goto _LL8B;_LL8A: if((_tmpAE.Abs_n).tag != 2)goto _LL8C;_tmpB0=(
_tmpAE.Abs_n).f1;if(_tmpB0 != 0)goto _LL8C;_LL8B: goto _LL87;_LL8C:;_LL8D:{const char*
_tmp2BA;void*_tmp2B9[1];struct Cyc_String_pa_struct _tmp2B8;(_tmp2B8.tag=0,((
_tmp2B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
td->name)),((_tmp2B9[0]=& _tmp2B8,Cyc_Tcutil_terr(loc,((_tmp2BA="qualified typedef declarations are not implemented (%s)",
_tag_dyneither(_tmp2BA,sizeof(char),56))),_tag_dyneither(_tmp2B9,sizeof(void*),1)))))));}
return;_LL87:;}if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(
ge->typedefs,v)){{const char*_tmp2BE;void*_tmp2BD[1];struct Cyc_String_pa_struct
_tmp2BC;(_tmp2BC.tag=0,((_tmp2BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
v),((_tmp2BD[0]=& _tmp2BC,Cyc_Tcutil_terr(loc,((_tmp2BE="redeclaration of typedef %s",
_tag_dyneither(_tmp2BE,sizeof(char),28))),_tag_dyneither(_tmp2BD,sizeof(void*),1)))))));}
return;}{union Cyc_Absyn_Nmspace_union _tmp2BF;(*td->name).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp2BF.Abs_n).tag=2,(((_tmp2BF.Abs_n).f1=te->ns,_tmp2BF))));}Cyc_Tcutil_check_unique_tvars(
loc,td->tvs);Cyc_Tcutil_add_tvar_identities(td->tvs);if(td->defn != 0){Cyc_Tcutil_check_type(
loc,te,td->tvs,(void*)0,0,(void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);(
td->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(
void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);}{struct Cyc_List_List*tvs=
td->tvs;for(0;tvs != 0;tvs=tvs->tl){void*_tmpB8=Cyc_Absyn_compress_kb((void*)((
struct Cyc_Absyn_Tvar*)tvs->hd)->kind);struct Cyc_Core_Opt*_tmpB9;struct Cyc_Core_Opt**
_tmpBA;struct Cyc_Core_Opt*_tmpBB;struct Cyc_Core_Opt**_tmpBC;void*_tmpBD;_LL8F:
if(*((int*)_tmpB8)!= 1)goto _LL91;_tmpB9=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB8)->f1;_tmpBA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB8)->f1;_LL90: if(td->defn != 0){const char*_tmp2C3;void*_tmp2C2[1];struct Cyc_String_pa_struct
_tmp2C1;(_tmp2C1.tag=0,((_tmp2C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Tvar*)tvs->hd)->name),((_tmp2C2[0]=& _tmp2C1,Cyc_Tcutil_warn(loc,((
_tmp2C3="type variable %s is not used in typedef definition",_tag_dyneither(
_tmp2C3,sizeof(char),51))),_tag_dyneither(_tmp2C2,sizeof(void*),1)))))));}{
struct Cyc_Core_Opt*_tmp2C4;*_tmpBA=((_tmp2C4=_cycalloc(sizeof(*_tmp2C4)),((
_tmp2C4->v=(void*)Cyc_Tcutil_kind_to_bound((void*)2),_tmp2C4))));}goto _LL8E;
_LL91: if(*((int*)_tmpB8)!= 2)goto _LL93;_tmpBB=((struct Cyc_Absyn_Less_kb_struct*)
_tmpB8)->f1;_tmpBC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmpB8)->f1;_tmpBD=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpB8)->f2;_LL92:{
struct Cyc_Core_Opt*_tmp2C5;*_tmpBC=((_tmp2C5=_cycalloc(sizeof(*_tmp2C5)),((
_tmp2C5->v=(void*)Cyc_Tcutil_kind_to_bound(_tmpBD),_tmp2C5))));}goto _LL8E;_LL93:;
_LL94: continue;_LL8E:;}}ge->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(ge->typedefs,
v,td);}static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*
ge,struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_List_List*
rpo,struct Cyc_List_List*fields);static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){struct _RegionHandle
_tmpC3=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpC3;
_push_region(uprev_rgn);for(0;rpo != 0;rpo=rpo->tl){struct _tuple3 _tmpC5;void*
_tmpC6;void*_tmpC7;struct _tuple3*_tmpC4=(struct _tuple3*)rpo->hd;_tmpC5=*_tmpC4;
_tmpC6=_tmpC5.f1;_tmpC7=_tmpC5.f2;Cyc_Tcutil_check_type(loc,te,tvs,(void*)6,0,
_tmpC6);Cyc_Tcutil_check_type(loc,te,tvs,(void*)5,0,_tmpC7);}{struct Cyc_List_List*
prev_fields=0;struct Cyc_List_List*_tmpC8=fields;for(0;_tmpC8 != 0;_tmpC8=_tmpC8->tl){
struct Cyc_Absyn_Aggrfield _tmpCA;struct _dyneither_ptr*_tmpCB;struct Cyc_Absyn_Tqual
_tmpCC;void*_tmpCD;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_List_List*_tmpCF;struct
Cyc_Absyn_Aggrfield*_tmpC9=(struct Cyc_Absyn_Aggrfield*)_tmpC8->hd;_tmpCA=*_tmpC9;
_tmpCB=_tmpCA.name;_tmpCC=_tmpCA.tq;_tmpCD=(void*)_tmpCA.type;_tmpCE=_tmpCA.width;
_tmpCF=_tmpCA.attributes;if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct
_dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,
prev_fields,_tmpCB)){const char*_tmp2C9;void*_tmp2C8[1];struct Cyc_String_pa_struct
_tmp2C7;(_tmp2C7.tag=0,((_tmp2C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpCB),((_tmp2C8[0]=& _tmp2C7,Cyc_Tcutil_terr(loc,((_tmp2C9="duplicate field %s",
_tag_dyneither(_tmp2C9,sizeof(char),19))),_tag_dyneither(_tmp2C8,sizeof(void*),1)))))));}{
const char*_tmp2CA;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpCB,((_tmp2CA="",
_tag_dyneither(_tmp2CA,sizeof(char),1))))!= 0){struct Cyc_List_List*_tmp2CB;
prev_fields=((_tmp2CB=_region_malloc(uprev_rgn,sizeof(*_tmp2CB)),((_tmp2CB->hd=
_tmpCB,((_tmp2CB->tl=prev_fields,_tmp2CB))))));}}Cyc_Tcutil_check_type(loc,te,
tvs,(void*)1,0,_tmpCD);(((struct Cyc_Absyn_Aggrfield*)_tmpC8->hd)->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmpC8->hd)->tq).print_const,
_tmpCD);Cyc_Tcutil_check_bitfield(loc,te,_tmpCD,_tmpCE,_tmpCB);}};_pop_region(
uprev_rgn);}struct _tuple9{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***
f2;};void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl*ad);void Cyc_Tc_tcAggrdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Aggrdecl*ad){struct _dyneither_ptr*_tmpD5=(*ad->name).f2;{struct
Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){void*_tmpD6=(
void*)atts->hd;_LL96: if((int)_tmpD6 != 5)goto _LL98;_LL97: goto _LL99;_LL98: if(
_tmpD6 <= (void*)17)goto _LL9A;if(*((int*)_tmpD6)!= 1)goto _LL9A;_LL99: continue;
_LL9A:;_LL9B:{const char*_tmp2D0;void*_tmp2CF[2];struct Cyc_String_pa_struct
_tmp2CE;struct Cyc_String_pa_struct _tmp2CD;(_tmp2CD.tag=0,((_tmp2CD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpD5),((_tmp2CE.tag=0,((_tmp2CE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)
atts->hd)),((_tmp2CF[0]=& _tmp2CE,((_tmp2CF[1]=& _tmp2CD,Cyc_Tcutil_terr(loc,((
_tmp2D0="bad attribute %s in  %s definition",_tag_dyneither(_tmp2D0,sizeof(char),
35))),_tag_dyneither(_tmp2CF,sizeof(void*),2)))))))))))));}goto _LL95;_LL95:;}}{
struct Cyc_List_List*_tmpDB=ad->tvs;{struct Cyc_List_List*tvs2=_tmpDB;for(0;tvs2 != 
0;tvs2=tvs2->tl){void*_tmpDC=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind);struct Cyc_Core_Opt*_tmpDD;struct Cyc_Core_Opt**_tmpDE;struct Cyc_Core_Opt*
_tmpDF;struct Cyc_Core_Opt**_tmpE0;void*_tmpE1;struct Cyc_Core_Opt*_tmpE2;struct
Cyc_Core_Opt**_tmpE3;void*_tmpE4;void*_tmpE5;_LL9D: if(*((int*)_tmpDC)!= 1)goto
_LL9F;_tmpDD=((struct Cyc_Absyn_Unknown_kb_struct*)_tmpDC)->f1;_tmpDE=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmpDC)->f1;_LL9E: _tmpE0=_tmpDE;goto _LLA0;
_LL9F: if(*((int*)_tmpDC)!= 2)goto _LLA1;_tmpDF=((struct Cyc_Absyn_Less_kb_struct*)
_tmpDC)->f1;_tmpE0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmpDC)->f1;_tmpE1=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpDC)->f2;if((int)
_tmpE1 != 1)goto _LLA1;_LLA0: _tmpE3=_tmpE0;goto _LLA2;_LLA1: if(*((int*)_tmpDC)!= 2)
goto _LLA3;_tmpE2=((struct Cyc_Absyn_Less_kb_struct*)_tmpDC)->f1;_tmpE3=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmpDC)->f1;_tmpE4=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpDC)->f2;if((int)_tmpE4 != 0)goto _LLA3;_LLA2:{struct Cyc_Core_Opt*_tmp2D1;*
_tmpE3=((_tmp2D1=_cycalloc(sizeof(*_tmp2D1)),((_tmp2D1->v=(void*)Cyc_Tcutil_kind_to_bound((
void*)2),_tmp2D1))));}continue;_LLA3: if(*((int*)_tmpDC)!= 0)goto _LLA5;_tmpE5=(
void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpDC)->f1;if((int)_tmpE5 != 1)goto _LLA5;
_LLA4:{const char*_tmp2D6;void*_tmp2D5[2];struct Cyc_String_pa_struct _tmp2D4;
struct Cyc_String_pa_struct _tmp2D3;(_tmp2D3.tag=0,((_tmp2D3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((
_tmp2D4.tag=0,((_tmp2D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpD5),((
_tmp2D5[0]=& _tmp2D4,((_tmp2D5[1]=& _tmp2D3,Cyc_Tcutil_terr(loc,((_tmp2D6="type %s attempts to abstract type variable %s of kind M",
_tag_dyneither(_tmp2D6,sizeof(char),56))),_tag_dyneither(_tmp2D5,sizeof(void*),2)))))))))))));}
continue;_LLA5:;_LLA6: continue;_LL9C:;}}{union Cyc_Absyn_Nmspace_union _tmpEB=(*ad->name).f1;
struct Cyc_List_List*_tmpEC;struct Cyc_List_List*_tmpED;_LLA8: if((_tmpEB.Rel_n).tag
!= 1)goto _LLAA;_tmpEC=(_tmpEB.Rel_n).f1;if(_tmpEC != 0)goto _LLAA;_LLA9: goto _LLAB;
_LLAA: if((_tmpEB.Abs_n).tag != 2)goto _LLAC;_tmpED=(_tmpEB.Abs_n).f1;if(_tmpED != 0)
goto _LLAC;_LLAB: goto _LLA7;_LLAC:;_LLAD:{const char*_tmp2DA;void*_tmp2D9[1];struct
Cyc_String_pa_struct _tmp2D8;(_tmp2D8.tag=0,((_tmp2D8.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(ad->name)),((_tmp2D9[0]=& _tmp2D8,
Cyc_Tcutil_terr(loc,((_tmp2DA="qualified struct declarations are not implemented (%s)",
_tag_dyneither(_tmp2DA,sizeof(char),55))),_tag_dyneither(_tmp2D9,sizeof(void*),1)))))));}
return;_LLA7:;}{union Cyc_Absyn_Nmspace_union _tmp2DB;(*ad->name).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp2DB.Abs_n).tag=2,(((_tmp2DB.Abs_n).f1=te->ns,_tmp2DB))));}Cyc_Tcutil_check_unique_tvars(
loc,ad->tvs);Cyc_Tcutil_add_tvar_identities(ad->tvs);{struct _tuple9 _tmp2DC;
struct _tuple9 _tmpF3=(_tmp2DC.f1=ad->impl,((_tmp2DC.f2=((struct Cyc_Absyn_Aggrdecl***(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->aggrdecls,
_tmpD5),_tmp2DC)));struct Cyc_Absyn_AggrdeclImpl*_tmpF4;struct Cyc_Absyn_Aggrdecl***
_tmpF5;struct Cyc_Absyn_AggrdeclImpl*_tmpF6;struct Cyc_Absyn_AggrdeclImpl _tmpF7;
struct Cyc_List_List*_tmpF8;struct Cyc_List_List*_tmpF9;struct Cyc_List_List*_tmpFA;
struct Cyc_Absyn_Aggrdecl***_tmpFB;struct Cyc_Absyn_AggrdeclImpl*_tmpFC;struct Cyc_Absyn_AggrdeclImpl
_tmpFD;struct Cyc_List_List*_tmpFE;struct Cyc_List_List*_tmpFF;struct Cyc_List_List*
_tmp100;struct Cyc_Absyn_Aggrdecl***_tmp101;struct Cyc_Absyn_Aggrdecl**_tmp102;
struct Cyc_Absyn_AggrdeclImpl*_tmp103;struct Cyc_Absyn_Aggrdecl***_tmp104;struct
Cyc_Absyn_Aggrdecl**_tmp105;_LLAF: _tmpF4=_tmpF3.f1;if(_tmpF4 != 0)goto _LLB1;
_tmpF5=_tmpF3.f2;if(_tmpF5 != 0)goto _LLB1;_LLB0:{struct Cyc_Absyn_Aggrdecl**
_tmp2DD;ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,
_tmpD5,((_tmp2DD=_cycalloc(sizeof(*_tmp2DD)),((_tmp2DD[0]=ad,_tmp2DD)))));}goto
_LLAE;_LLB1: _tmpF6=_tmpF3.f1;if(_tmpF6 == 0)goto _LLB3;_tmpF7=*_tmpF6;_tmpF8=
_tmpF7.exist_vars;_tmpF9=_tmpF7.rgn_po;_tmpFA=_tmpF7.fields;_tmpFB=_tmpF3.f2;if(
_tmpFB != 0)goto _LLB3;_LLB2: {struct Cyc_Absyn_Aggrdecl*_tmp2E0;struct Cyc_Absyn_Aggrdecl**
_tmp2DF;struct Cyc_Absyn_Aggrdecl**_tmp107=(_tmp2DF=_cycalloc(sizeof(*_tmp2DF)),((
_tmp2DF[0]=((_tmp2E0=_cycalloc(sizeof(*_tmp2E0)),((_tmp2E0->kind=(void*)((void*)
ad->kind),((_tmp2E0->sc=(void*)((void*)3),((_tmp2E0->name=ad->name,((_tmp2E0->tvs=
_tmpDB,((_tmp2E0->impl=0,((_tmp2E0->attributes=ad->attributes,_tmp2E0)))))))))))))),
_tmp2DF)));ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,
_tmpD5,_tmp107);Cyc_Tcutil_check_unique_tvars(loc,_tmpF8);Cyc_Tcutil_add_tvar_identities(
_tmpF8);{struct Cyc_List_List*tvs2=_tmpF8;for(0;tvs2 != 0;tvs2=tvs2->tl){void*
_tmp108=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);
struct Cyc_Core_Opt*_tmp109;struct Cyc_Core_Opt**_tmp10A;struct Cyc_Core_Opt*
_tmp10B;struct Cyc_Core_Opt**_tmp10C;void*_tmp10D;struct Cyc_Core_Opt*_tmp10E;
struct Cyc_Core_Opt**_tmp10F;void*_tmp110;void*_tmp111;_LLB8: if(*((int*)_tmp108)
!= 1)goto _LLBA;_tmp109=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp108)->f1;_tmp10A=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp108)->f1;_LLB9:
_tmp10C=_tmp10A;goto _LLBB;_LLBA: if(*((int*)_tmp108)!= 2)goto _LLBC;_tmp10B=((
struct Cyc_Absyn_Less_kb_struct*)_tmp108)->f1;_tmp10C=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp108)->f1;_tmp10D=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp108)->f2;if((int)_tmp10D != 1)goto _LLBC;_LLBB: _tmp10F=_tmp10C;goto _LLBD;_LLBC:
if(*((int*)_tmp108)!= 2)goto _LLBE;_tmp10E=((struct Cyc_Absyn_Less_kb_struct*)
_tmp108)->f1;_tmp10F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp108)->f1;_tmp110=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp108)->f2;if((
int)_tmp110 != 0)goto _LLBE;_LLBD:{struct Cyc_Core_Opt*_tmp2E1;*_tmp10F=((_tmp2E1=
_cycalloc(sizeof(*_tmp2E1)),((_tmp2E1->v=(void*)Cyc_Tcutil_kind_to_bound((void*)
2),_tmp2E1))));}continue;_LLBE: if(*((int*)_tmp108)!= 0)goto _LLC0;_tmp111=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp108)->f1;if((int)_tmp111 != 1)goto _LLC0;_LLBF:{
const char*_tmp2E6;void*_tmp2E5[2];struct Cyc_String_pa_struct _tmp2E4;struct Cyc_String_pa_struct
_tmp2E3;(_tmp2E3.tag=0,((_tmp2E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp2E4.tag=0,((_tmp2E4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpD5),((_tmp2E5[0]=& _tmp2E4,((_tmp2E5[1]=&
_tmp2E3,Cyc_Tcutil_terr(loc,((_tmp2E6="type %s attempts to abstract type variable %s of kind M",
_tag_dyneither(_tmp2E6,sizeof(char),56))),_tag_dyneither(_tmp2E5,sizeof(void*),2)))))))))))));}
continue;_LLC0:;_LLC1: continue;_LLB7:;}}Cyc_Tc_tcAggrImpl(te,ge,loc,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpDB,_tmpF8),
_tmpF9,_tmpFA);if((void*)ad->kind == (void*)1){struct Cyc_List_List*f=_tmpFA;for(0;
f != 0;f=f->tl){if(!Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)f->hd)->type)){
const char*_tmp2EC;void*_tmp2EB[3];struct Cyc_String_pa_struct _tmp2EA;struct Cyc_String_pa_struct
_tmp2E9;struct Cyc_String_pa_struct _tmp2E8;(_tmp2E8.tag=0,((_tmp2E8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Absyn_Aggrfield*)
f->hd)->type)),((_tmp2E9.tag=0,((_tmp2E9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpD5),((_tmp2EA.tag=0,((_tmp2EA.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name),((_tmp2EB[0]=&
_tmp2EA,((_tmp2EB[1]=& _tmp2E9,((_tmp2EB[2]=& _tmp2E8,Cyc_Tcutil_warn(loc,((
_tmp2EC="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp2EC,sizeof(char),98))),_tag_dyneither(_tmp2EB,sizeof(void*),3)))))))))))))))))));}}}*
_tmp107=ad;goto _LLAE;}_LLB3: _tmpFC=_tmpF3.f1;if(_tmpFC == 0)goto _LLB5;_tmpFD=*
_tmpFC;_tmpFE=_tmpFD.exist_vars;_tmpFF=_tmpFD.rgn_po;_tmp100=_tmpFD.fields;
_tmp101=_tmpF3.f2;if(_tmp101 == 0)goto _LLB5;_tmp102=*_tmp101;_LLB4: if((void*)ad->kind
!= (void*)(*_tmp102)->kind){const char*_tmp2EF;void*_tmp2EE;(_tmp2EE=0,Cyc_Tcutil_terr(
loc,((_tmp2EF="cannot reuse struct names for unions and vice-versa",
_tag_dyneither(_tmp2EF,sizeof(char),52))),_tag_dyneither(_tmp2EE,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*_tmp120=*_tmp102;{struct Cyc_Absyn_Aggrdecl*_tmp2F0;*
_tmp102=((_tmp2F0=_cycalloc(sizeof(*_tmp2F0)),((_tmp2F0->kind=(void*)((void*)ad->kind),((
_tmp2F0->sc=(void*)((void*)3),((_tmp2F0->name=ad->name,((_tmp2F0->tvs=_tmpDB,((
_tmp2F0->impl=0,((_tmp2F0->attributes=ad->attributes,_tmp2F0))))))))))))));}Cyc_Tcutil_check_unique_tvars(
loc,_tmpFE);Cyc_Tcutil_add_tvar_identities(_tmpFE);Cyc_Tc_tcAggrImpl(te,ge,loc,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmpDB,_tmpFE),_tmpFF,_tmp100);*_tmp102=_tmp120;_tmp105=_tmp102;goto _LLB6;}_LLB5:
_tmp103=_tmpF3.f1;if(_tmp103 != 0)goto _LLAE;_tmp104=_tmpF3.f2;if(_tmp104 == 0)goto
_LLAE;_tmp105=*_tmp104;_LLB6: {struct Cyc_Absyn_Aggrdecl*_tmp122=Cyc_Tcdecl_merge_aggrdecl(*
_tmp105,ad,loc,Cyc_Tc_tc_msg);if(_tmp122 == 0)return;else{*_tmp105=(struct Cyc_Absyn_Aggrdecl*)
_tmp122;ad=(struct Cyc_Absyn_Aggrdecl*)_tmp122;goto _LLAE;}}_LLAE:;}{struct Cyc_Tcenv_AggrRes_struct*
_tmp2F6;struct Cyc_Tcenv_AggrRes_struct _tmp2F5;struct _tuple7*_tmp2F4;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple7*
v))Cyc_Dict_insert)(ge->ordinaries,_tmpD5,(struct _tuple7*)((_tmp2F4=_cycalloc(
sizeof(*_tmp2F4)),((_tmp2F4->f1=(void*)((_tmp2F6=_cycalloc(sizeof(*_tmp2F6)),((
_tmp2F6[0]=((_tmp2F5.tag=1,((_tmp2F5.f1=ad,_tmp2F5)))),_tmp2F6)))),((_tmp2F4->f2=
1,_tmp2F4)))))));}}}struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};static
struct Cyc_List_List*Cyc_Tc_tcTunionFields(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*
ge,struct Cyc_Position_Segment*loc,struct _dyneither_ptr obj,int is_xtunion,struct
_tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tuniondecl*
tudres);static struct Cyc_List_List*Cyc_Tc_tcTunionFields(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct _dyneither_ptr obj,
int is_xtunion,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*
tvs,struct Cyc_Absyn_Tuniondecl*tudres){{struct Cyc_List_List*_tmp126=fields;for(0;
_tmp126 != 0;_tmp126=_tmp126->tl){struct Cyc_Absyn_Tunionfield*_tmp127=(struct Cyc_Absyn_Tunionfield*)
_tmp126->hd;{struct Cyc_List_List*typs=_tmp127->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tcutil_check_type(_tmp127->loc,te,tvs,(void*)1,0,(*((struct _tuple10*)typs->hd)).f2);
if(!tudres->is_flat  && Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple10*)
typs->hd)).f2)){const char*_tmp2FA;void*_tmp2F9[1];struct Cyc_String_pa_struct
_tmp2F8;(_tmp2F8.tag=0,((_tmp2F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp127->name)),((_tmp2F9[0]=& _tmp2F8,Cyc_Tcutil_terr(
_tmp127->loc,((_tmp2FA="noalias pointers in non-flat tunions are not allowed (%s)",
_tag_dyneither(_tmp2FA,sizeof(char),58))),_tag_dyneither(_tmp2F9,sizeof(void*),1)))))));}((*((
struct _tuple10*)typs->hd)).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
_tmp127->loc,((*((struct _tuple10*)typs->hd)).f1).print_const,(*((struct _tuple10*)
typs->hd)).f2);}}{union Cyc_Absyn_Nmspace_union _tmp12B=(*_tmp127->name).f1;struct
Cyc_List_List*_tmp12C;_LLC3: if((_tmp12B.Rel_n).tag != 1)goto _LLC5;_tmp12C=(
_tmp12B.Rel_n).f1;if(_tmp12C != 0)goto _LLC5;_LLC4: if(is_xtunion){union Cyc_Absyn_Nmspace_union
_tmp2FB;(*_tmp127->name).f1=(union Cyc_Absyn_Nmspace_union)(((_tmp2FB.Abs_n).tag=
2,(((_tmp2FB.Abs_n).f1=te->ns,_tmp2FB))));}else{(*_tmp127->name).f1=(*name).f1;}
goto _LLC2;_LLC5: if((_tmp12B.Rel_n).tag != 1)goto _LLC7;_LLC6:{const char*_tmp2FF;
void*_tmp2FE[1];struct Cyc_String_pa_struct _tmp2FD;(_tmp2FD.tag=0,((_tmp2FD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp127->name)),((
_tmp2FE[0]=& _tmp2FD,Cyc_Tcutil_terr(_tmp127->loc,((_tmp2FF="qualified tunionfield declarations are not allowed (%s)",
_tag_dyneither(_tmp2FF,sizeof(char),56))),_tag_dyneither(_tmp2FE,sizeof(void*),1)))))));}
goto _LLC2;_LLC7: if((_tmp12B.Abs_n).tag != 2)goto _LLC9;_LLC8: goto _LLC2;_LLC9: if((
_tmp12B.Loc_n).tag != 0)goto _LLC2;_LLCA: {const char*_tmp302;void*_tmp301;(_tmp301=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp302="tcTunionFields: Loc_n",_tag_dyneither(_tmp302,sizeof(char),22))),
_tag_dyneither(_tmp301,sizeof(void*),0)));}_LLC2:;}}}{struct Cyc_List_List*
fields2;if(is_xtunion){int _tmp133=1;struct Cyc_List_List*_tmp134=Cyc_Tcdecl_sort_xtunion_fields(
fields,& _tmp133,(*name).f2,loc,Cyc_Tc_tc_msg);if(_tmp133)fields2=_tmp134;else{
fields2=0;}}else{struct _RegionHandle _tmp135=_new_region("uprev_rgn");struct
_RegionHandle*uprev_rgn=& _tmp135;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Tunionfield*
_tmp136=(struct Cyc_Absyn_Tunionfield*)fs->hd;if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp136->name).f2)){
const char*_tmp307;void*_tmp306[2];struct Cyc_String_pa_struct _tmp305;struct Cyc_String_pa_struct
_tmp304;(_tmp304.tag=0,((_tmp304.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
obj),((_tmp305.tag=0,((_tmp305.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*
_tmp136->name).f2),((_tmp306[0]=& _tmp305,((_tmp306[1]=& _tmp304,Cyc_Tcutil_terr(
_tmp136->loc,((_tmp307="duplicate field name %s in %s",_tag_dyneither(_tmp307,
sizeof(char),30))),_tag_dyneither(_tmp306,sizeof(void*),2)))))))))))));}else{
struct Cyc_List_List*_tmp308;prev_fields=((_tmp308=_region_malloc(uprev_rgn,
sizeof(*_tmp308)),((_tmp308->hd=(*_tmp136->name).f2,((_tmp308->tl=prev_fields,
_tmp308))))));}if((void*)_tmp136->sc != (void*)2){{const char*_tmp30C;void*_tmp30B[
1];struct Cyc_String_pa_struct _tmp30A;(_tmp30A.tag=0,((_tmp30A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*_tmp136->name).f2),((_tmp30B[0]=&
_tmp30A,Cyc_Tcutil_warn(loc,((_tmp30C="ignoring scope of field %s",
_tag_dyneither(_tmp30C,sizeof(char),27))),_tag_dyneither(_tmp30B,sizeof(void*),1)))))));}(
void*)(_tmp136->sc=(void*)((void*)2));}}}fields2=fields;};_pop_region(uprev_rgn);}{
struct Cyc_List_List*_tmp13F=fields;for(0;_tmp13F != 0;_tmp13F=_tmp13F->tl){struct
Cyc_Absyn_Tunionfield*_tmp140=(struct Cyc_Absyn_Tunionfield*)_tmp13F->hd;struct
Cyc_Tcenv_TunionRes_struct*_tmp312;struct Cyc_Tcenv_TunionRes_struct _tmp311;
struct _tuple7*_tmp310;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple7*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmp140->name).f2,(struct _tuple7*)((_tmp310=_cycalloc(sizeof(*_tmp310)),((
_tmp310->f1=(void*)((_tmp312=_cycalloc(sizeof(*_tmp312)),((_tmp312[0]=((_tmp311.tag=
2,((_tmp311.f1=tudres,((_tmp311.f2=_tmp140,_tmp311)))))),_tmp312)))),((_tmp310->f2=
1,_tmp310)))))));}}return fields2;}}struct _tuple11{struct Cyc_Core_Opt*f1;struct
Cyc_Absyn_Tuniondecl***f2;};void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Tuniondecl*
tud);void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Tuniondecl*tud){struct
_dyneither_ptr*v=(*tud->name).f2;const char*_tmp314;const char*_tmp313;struct
_dyneither_ptr obj=tud->is_xtunion?(_tmp314="xtunion",_tag_dyneither(_tmp314,
sizeof(char),8)):((_tmp313="tunion",_tag_dyneither(_tmp313,sizeof(char),7)));
struct Cyc_List_List*tvs=tud->tvs;{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;
tvs2=tvs2->tl){void*_tmp144=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind);struct Cyc_Core_Opt*_tmp145;struct Cyc_Core_Opt**_tmp146;struct
Cyc_Core_Opt*_tmp147;struct Cyc_Core_Opt**_tmp148;void*_tmp149;struct Cyc_Core_Opt*
_tmp14A;struct Cyc_Core_Opt**_tmp14B;void*_tmp14C;struct Cyc_Core_Opt*_tmp14D;
struct Cyc_Core_Opt**_tmp14E;void*_tmp14F;void*_tmp150;struct Cyc_Core_Opt*_tmp151;
struct Cyc_Core_Opt**_tmp152;void*_tmp153;void*_tmp154;void*_tmp155;_LLCC: if(*((
int*)_tmp144)!= 1)goto _LLCE;_tmp145=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp144)->f1;
_tmp146=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp144)->f1;
_LLCD: _tmp148=_tmp146;goto _LLCF;_LLCE: if(*((int*)_tmp144)!= 2)goto _LLD0;_tmp147=((
struct Cyc_Absyn_Less_kb_struct*)_tmp144)->f1;_tmp148=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp144)->f1;_tmp149=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp144)->f2;if((int)_tmp149 != 1)goto _LLD0;_LLCF: _tmp14B=_tmp148;goto _LLD1;_LLD0:
if(*((int*)_tmp144)!= 2)goto _LLD2;_tmp14A=((struct Cyc_Absyn_Less_kb_struct*)
_tmp144)->f1;_tmp14B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp144)->f1;_tmp14C=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp144)->f2;if((
int)_tmp14C != 0)goto _LLD2;_LLD1:{struct Cyc_Core_Opt*_tmp315;*_tmp14B=((_tmp315=
_cycalloc(sizeof(*_tmp315)),((_tmp315->v=(void*)Cyc_Tcutil_kind_to_bound((void*)
2),_tmp315))));}goto _LLCB;_LLD2: if(*((int*)_tmp144)!= 2)goto _LLD4;_tmp14D=((
struct Cyc_Absyn_Less_kb_struct*)_tmp144)->f1;_tmp14E=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp144)->f1;_tmp14F=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp144)->f2;if((int)_tmp14F != 5)goto _LLD4;_LLD3:{struct Cyc_Core_Opt*_tmp316;*
_tmp14E=((_tmp316=_cycalloc(sizeof(*_tmp316)),((_tmp316->v=(void*)Cyc_Tcutil_kind_to_bound((
void*)3),_tmp316))));}goto _LLCB;_LLD4: if(*((int*)_tmp144)!= 0)goto _LLD6;_tmp150=(
void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp144)->f1;if((int)_tmp150 != 5)goto _LLD6;
_LLD5: if(!tud->is_flat){const char*_tmp31C;void*_tmp31B[3];struct Cyc_String_pa_struct
_tmp31A;struct Cyc_String_pa_struct _tmp319;struct Cyc_String_pa_struct _tmp318;(
_tmp318.tag=0,((_tmp318.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp319.tag=0,((_tmp319.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp31A.tag=0,((_tmp31A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)obj),((_tmp31B[0]=& _tmp31A,((_tmp31B[1]=&
_tmp319,((_tmp31B[2]=& _tmp318,Cyc_Tcutil_terr(loc,((_tmp31C="%s %s attempts to abstract type variable %s of kind TR",
_tag_dyneither(_tmp31C,sizeof(char),55))),_tag_dyneither(_tmp31B,sizeof(void*),3)))))))))))))))))));}
goto _LLCB;_LLD6: if(*((int*)_tmp144)!= 2)goto _LLD8;_tmp151=((struct Cyc_Absyn_Less_kb_struct*)
_tmp144)->f1;_tmp152=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp144)->f1;_tmp153=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp144)->f2;_LLD7:
goto _LLD9;_LLD8: if(*((int*)_tmp144)!= 0)goto _LLDA;_tmp154=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp144)->f1;if((int)_tmp154 != 4)goto _LLDA;_LLD9: if(!tud->is_flat){const char*
_tmp322;void*_tmp321[3];struct Cyc_String_pa_struct _tmp320;struct Cyc_String_pa_struct
_tmp31F;struct Cyc_String_pa_struct _tmp31E;(_tmp31E.tag=0,((_tmp31E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((
_tmp31F.tag=0,((_tmp31F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((
_tmp320.tag=0,((_tmp320.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj),((
_tmp321[0]=& _tmp320,((_tmp321[1]=& _tmp31F,((_tmp321[2]=& _tmp31E,Cyc_Tcutil_terr(
loc,((_tmp322="%s %s attempts to abstract type variable %s of kind UR",
_tag_dyneither(_tmp322,sizeof(char),55))),_tag_dyneither(_tmp321,sizeof(void*),3)))))))))))))))))));}
goto _LLCB;_LLDA: if(*((int*)_tmp144)!= 0)goto _LLDC;_tmp155=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp144)->f1;if((int)_tmp155 != 1)goto _LLDC;_LLDB:{const char*_tmp328;void*_tmp327[
3];struct Cyc_String_pa_struct _tmp326;struct Cyc_String_pa_struct _tmp325;struct Cyc_String_pa_struct
_tmp324;(_tmp324.tag=0,((_tmp324.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp325.tag=0,((_tmp325.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp326.tag=0,((_tmp326.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)obj),((_tmp327[0]=& _tmp326,((_tmp327[1]=&
_tmp325,((_tmp327[2]=& _tmp324,Cyc_Tcutil_terr(loc,((_tmp328="%s %s attempts to abstract type variable %s of kind M",
_tag_dyneither(_tmp328,sizeof(char),54))),_tag_dyneither(_tmp327,sizeof(void*),3)))))))))))))))))));}
goto _LLCB;_LLDC:;_LLDD: goto _LLCB;_LLCB:;}}Cyc_Tcutil_check_unique_tvars(loc,tvs);
Cyc_Tcutil_add_tvar_identities(tvs);{struct _RegionHandle _tmp167=_new_region("temp");
struct _RegionHandle*temp=& _tmp167;_push_region(temp);{struct Cyc_Absyn_Tuniondecl***
tud_opt;if(tud->is_xtunion){{struct _handler_cons _tmp168;_push_handler(& _tmp168);{
int _tmp16A=0;if(setjmp(_tmp168.handler))_tmp16A=1;if(!_tmp16A){tud_opt=Cyc_Tcenv_lookup_xtuniondecl(
temp,te,loc,tud->name);;_pop_handler();}else{void*_tmp169=(void*)_exn_thrown;
void*_tmp16C=_tmp169;_LLDF: if(_tmp16C != Cyc_Dict_Absent)goto _LLE1;_LLE0:{const
char*_tmp32C;void*_tmp32B[1];struct Cyc_String_pa_struct _tmp32A;(_tmp32A.tag=0,((
_tmp32A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tud->name)),((_tmp32B[0]=& _tmp32A,Cyc_Tcutil_terr(loc,((_tmp32C="qualified xtunion declaration %s is not an existing xtunion",
_tag_dyneither(_tmp32C,sizeof(char),60))),_tag_dyneither(_tmp32B,sizeof(void*),1)))))));}
_npop_handler(0);return;_LLE1:;_LLE2:(void)_throw(_tmp16C);_LLDE:;}}}if(tud_opt
!= 0)tud->name=(*(*tud_opt))->name;else{union Cyc_Absyn_Nmspace_union _tmp32D;(*
tud->name).f1=(union Cyc_Absyn_Nmspace_union)(((_tmp32D.Abs_n).tag=2,(((_tmp32D.Abs_n).f1=
te->ns,_tmp32D))));}}else{{union Cyc_Absyn_Nmspace_union _tmp171=(*tud->name).f1;
struct Cyc_List_List*_tmp172;_LLE4: if((_tmp171.Rel_n).tag != 1)goto _LLE6;_tmp172=(
_tmp171.Rel_n).f1;if(_tmp172 != 0)goto _LLE6;_LLE5:{union Cyc_Absyn_Nmspace_union
_tmp32E;(*tud->name).f1=(union Cyc_Absyn_Nmspace_union)(((_tmp32E.Abs_n).tag=2,(((
_tmp32E.Abs_n).f1=te->ns,_tmp32E))));}goto _LLE3;_LLE6: if((_tmp171.Abs_n).tag != 2)
goto _LLE8;_LLE7: goto _LLE9;_LLE8:;_LLE9:{const char*_tmp332;void*_tmp331[1];struct
Cyc_String_pa_struct _tmp330;(_tmp330.tag=0,((_tmp330.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tud->name)),((_tmp331[0]=& _tmp330,
Cyc_Tcutil_terr(loc,((_tmp332="qualified tunion declarations are not implemented (%s)",
_tag_dyneither(_tmp332,sizeof(char),55))),_tag_dyneither(_tmp331,sizeof(void*),1)))))));}
_npop_handler(0);return;_LLE3:;}{struct Cyc_Absyn_Tuniondecl***_tmp177=((struct
Cyc_Absyn_Tuniondecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(
ge->tuniondecls,v);struct Cyc_Absyn_Tuniondecl***_tmp333;tud_opt=(unsigned int)
_tmp177?(_tmp333=_region_malloc(temp,sizeof(*_tmp333)),((_tmp333[0]=*_tmp177,
_tmp333))): 0;}}{struct _tuple11 _tmp334;struct _tuple11 _tmp17A=(_tmp334.f1=tud->fields,((
_tmp334.f2=tud_opt,_tmp334)));struct Cyc_Core_Opt*_tmp17B;struct Cyc_Absyn_Tuniondecl***
_tmp17C;struct Cyc_Core_Opt*_tmp17D;struct Cyc_Core_Opt _tmp17E;struct Cyc_List_List*
_tmp17F;struct Cyc_List_List**_tmp180;struct Cyc_Absyn_Tuniondecl***_tmp181;struct
Cyc_Core_Opt*_tmp182;struct Cyc_Core_Opt _tmp183;struct Cyc_List_List*_tmp184;
struct Cyc_List_List**_tmp185;struct Cyc_Absyn_Tuniondecl***_tmp186;struct Cyc_Absyn_Tuniondecl**
_tmp187;struct Cyc_Core_Opt*_tmp188;struct Cyc_Absyn_Tuniondecl***_tmp189;struct
Cyc_Absyn_Tuniondecl**_tmp18A;_LLEB: _tmp17B=_tmp17A.f1;if(_tmp17B != 0)goto _LLED;
_tmp17C=_tmp17A.f2;if(_tmp17C != 0)goto _LLED;_LLEC:{struct Cyc_Absyn_Tuniondecl**
_tmp335;ge->tuniondecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Tuniondecl**v))Cyc_Dict_insert)(ge->tuniondecls,
v,((_tmp335=_cycalloc(sizeof(*_tmp335)),((_tmp335[0]=tud,_tmp335)))));}goto _LLEA;
_LLED: _tmp17D=_tmp17A.f1;if(_tmp17D == 0)goto _LLEF;_tmp17E=*_tmp17D;_tmp17F=(
struct Cyc_List_List*)_tmp17E.v;_tmp180=(struct Cyc_List_List**)&(*_tmp17A.f1).v;
_tmp181=_tmp17A.f2;if(_tmp181 != 0)goto _LLEF;_LLEE: {struct Cyc_Absyn_Tuniondecl*
_tmp338;struct Cyc_Absyn_Tuniondecl**_tmp337;struct Cyc_Absyn_Tuniondecl**_tmp18C=(
_tmp337=_cycalloc(sizeof(*_tmp337)),((_tmp337[0]=((_tmp338=_cycalloc(sizeof(*
_tmp338)),((_tmp338->sc=(void*)((void*)3),((_tmp338->name=tud->name,((_tmp338->tvs=
tvs,((_tmp338->fields=0,((_tmp338->is_xtunion=tud->is_xtunion,((_tmp338->is_flat=
tud->is_flat,_tmp338)))))))))))))),_tmp337)));ge->tuniondecls=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Tuniondecl**v))Cyc_Dict_insert)(
ge->tuniondecls,v,_tmp18C);*_tmp180=Cyc_Tc_tcTunionFields(te,ge,loc,obj,tud->is_xtunion,
tud->name,*_tmp180,tvs,tud);*_tmp18C=tud;goto _LLEA;}_LLEF: _tmp182=_tmp17A.f1;if(
_tmp182 == 0)goto _LLF1;_tmp183=*_tmp182;_tmp184=(struct Cyc_List_List*)_tmp183.v;
_tmp185=(struct Cyc_List_List**)&(*_tmp17A.f1).v;_tmp186=_tmp17A.f2;if(_tmp186 == 
0)goto _LLF1;_tmp187=*_tmp186;_LLF0: {struct Cyc_Absyn_Tuniondecl*_tmp18F=*_tmp187;{
struct Cyc_Absyn_Tuniondecl*_tmp339;*_tmp187=((_tmp339=_cycalloc(sizeof(*_tmp339)),((
_tmp339->sc=(void*)((void*)3),((_tmp339->name=tud->name,((_tmp339->tvs=tvs,((
_tmp339->fields=0,((_tmp339->is_xtunion=tud->is_xtunion,((_tmp339->is_flat=tud->is_flat,
_tmp339))))))))))))));}*_tmp185=Cyc_Tc_tcTunionFields(te,ge,loc,obj,tud->is_xtunion,
tud->name,*_tmp185,tvs,tud);*_tmp187=_tmp18F;_tmp18A=_tmp187;goto _LLF2;}_LLF1:
_tmp188=_tmp17A.f1;if(_tmp188 != 0)goto _LLEA;_tmp189=_tmp17A.f2;if(_tmp189 == 0)
goto _LLEA;_tmp18A=*_tmp189;_LLF2: {struct Cyc_Absyn_Tuniondecl*_tmp191=Cyc_Tcdecl_merge_tuniondecl(*
_tmp18A,tud,loc,Cyc_Tc_tc_msg);if(_tmp191 == 0){_npop_handler(0);return;}else{*
_tmp18A=(struct Cyc_Absyn_Tuniondecl*)_tmp191;goto _LLEA;}}_LLEA:;}};_pop_region(
temp);}}void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Enumdecl*ed);void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Enumdecl*ed){struct _dyneither_ptr*v=(*ed->name).f2;{union Cyc_Absyn_Nmspace_union
_tmp194=(*ed->name).f1;struct Cyc_List_List*_tmp195;struct Cyc_List_List*_tmp196;
_LLF4: if((_tmp194.Rel_n).tag != 1)goto _LLF6;_tmp195=(_tmp194.Rel_n).f1;if(_tmp195
!= 0)goto _LLF6;_LLF5: goto _LLF7;_LLF6: if((_tmp194.Abs_n).tag != 2)goto _LLF8;
_tmp196=(_tmp194.Abs_n).f1;if(_tmp196 != 0)goto _LLF8;_LLF7: goto _LLF3;_LLF8:;_LLF9:{
const char*_tmp33D;void*_tmp33C[1];struct Cyc_String_pa_struct _tmp33B;(_tmp33B.tag=
0,((_tmp33B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
ed->name)),((_tmp33C[0]=& _tmp33B,Cyc_Tcutil_terr(loc,((_tmp33D="qualified enum declarations are not implemented (%s)",
_tag_dyneither(_tmp33D,sizeof(char),53))),_tag_dyneither(_tmp33C,sizeof(void*),1)))))));}
return;_LLF3:;}{union Cyc_Absyn_Nmspace_union _tmp33E;(*ed->name).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp33E.Abs_n).tag=2,(((_tmp33E.Abs_n).f1=te->ns,_tmp33E))));}if(ed->fields != 0){
struct _RegionHandle _tmp19B=_new_region("uprev_rgn");struct _RegionHandle*
uprev_rgn=& _tmp19B;_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp19C=(struct Cyc_Absyn_Enumfield*)fs->hd;if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp19C->name).f2)){
const char*_tmp342;void*_tmp341[1];struct Cyc_String_pa_struct _tmp340;(_tmp340.tag=
0,((_tmp340.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp19C->name).f2),((
_tmp341[0]=& _tmp340,Cyc_Tcutil_terr(_tmp19C->loc,((_tmp342="duplicate field name %s",
_tag_dyneither(_tmp342,sizeof(char),24))),_tag_dyneither(_tmp341,sizeof(void*),1)))))));}
else{struct Cyc_List_List*_tmp343;prev_fields=((_tmp343=_region_malloc(uprev_rgn,
sizeof(*_tmp343)),((_tmp343->hd=(*_tmp19C->name).f2,((_tmp343->tl=prev_fields,
_tmp343))))));}if(_tmp19C->tag == 0)_tmp19C->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp19C->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp19C->tag))){const char*_tmp348;void*_tmp347[2];struct Cyc_String_pa_struct
_tmp346;struct Cyc_String_pa_struct _tmp345;(_tmp345.tag=0,((_tmp345.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*_tmp19C->name).f2),((_tmp346.tag=0,((
_tmp346.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp347[0]=&
_tmp346,((_tmp347[1]=& _tmp345,Cyc_Tcutil_terr(loc,((_tmp348="enum %s, field %s: expression is not constant",
_tag_dyneither(_tmp348,sizeof(char),46))),_tag_dyneither(_tmp347,sizeof(void*),2)))))))))))));}}{
unsigned int _tmp1A6;int _tmp1A7;struct _tuple4 _tmp1A5=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp19C->tag));_tmp1A6=_tmp1A5.f1;_tmp1A7=
_tmp1A5.f2;if(!_tmp1A7){const char*_tmp34B;void*_tmp34A;(_tmp34A=0,Cyc_Tcutil_terr(
loc,((_tmp34B="Cyclone enum tags cannot use sizeof or offsetof",_tag_dyneither(
_tmp34B,sizeof(char),48))),_tag_dyneither(_tmp34A,sizeof(void*),0)));}tag_count=
_tmp1A6 + 1;{union Cyc_Absyn_Nmspace_union _tmp34C;(*_tmp19C->name).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp34C.Abs_n).tag=2,(((_tmp34C.Abs_n).f1=te->ns,_tmp34C))));}}}};_pop_region(
uprev_rgn);}{struct _handler_cons _tmp1AB;_push_handler(& _tmp1AB);{int _tmp1AD=0;
if(setjmp(_tmp1AB.handler))_tmp1AD=1;if(!_tmp1AD){{struct Cyc_Absyn_Enumdecl**
_tmp1AE=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->enumdecls,v);struct Cyc_Absyn_Enumdecl*
_tmp1AF=Cyc_Tcdecl_merge_enumdecl(*_tmp1AE,ed,loc,Cyc_Tc_tc_msg);if(_tmp1AF == 0){
_npop_handler(0);return;}*_tmp1AE=(struct Cyc_Absyn_Enumdecl*)_tmp1AF;};
_pop_handler();}else{void*_tmp1AC=(void*)_exn_thrown;void*_tmp1B1=_tmp1AC;_LLFB:
if(_tmp1B1 != Cyc_Dict_Absent)goto _LLFD;_LLFC: {struct Cyc_Absyn_Enumdecl**_tmp34D;
struct Cyc_Absyn_Enumdecl**_tmp1B2=(_tmp34D=_cycalloc(sizeof(*_tmp34D)),((_tmp34D[
0]=ed,_tmp34D)));ge->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,
v,_tmp1B2);goto _LLFA;}_LLFD:;_LLFE:(void)_throw(_tmp1B1);_LLFA:;}}}if(ed->fields
!= 0){struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp1B4=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Tcenv_EnumRes_struct*
_tmp353;struct Cyc_Tcenv_EnumRes_struct _tmp352;struct _tuple7*_tmp351;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple7*
v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1B4->name).f2,(struct _tuple7*)((_tmp351=
_cycalloc(sizeof(*_tmp351)),((_tmp351->f1=(void*)((_tmp353=_cycalloc(sizeof(*
_tmp353)),((_tmp353[0]=((_tmp352.tag=3,((_tmp352.f1=ed,((_tmp352.f2=_tmp1B4,
_tmp352)))))),_tmp353)))),((_tmp351->f2=1,_tmp351)))))));}}}static int Cyc_Tc_okay_externC(
struct Cyc_Position_Segment*loc,void*sc);static int Cyc_Tc_okay_externC(struct Cyc_Position_Segment*
loc,void*sc){void*_tmp1B8=sc;_LL100: if((int)_tmp1B8 != 0)goto _LL102;_LL101:{const
char*_tmp356;void*_tmp355;(_tmp355=0,Cyc_Tcutil_warn(loc,((_tmp356="static declaration nested within extern \"C\"",
_tag_dyneither(_tmp356,sizeof(char),44))),_tag_dyneither(_tmp355,sizeof(void*),0)));}
return 0;_LL102: if((int)_tmp1B8 != 1)goto _LL104;_LL103:{const char*_tmp359;void*
_tmp358;(_tmp358=0,Cyc_Tcutil_warn(loc,((_tmp359="abstract declaration nested within extern \"C\"",
_tag_dyneither(_tmp359,sizeof(char),46))),_tag_dyneither(_tmp358,sizeof(void*),0)));}
return 0;_LL104: if((int)_tmp1B8 != 2)goto _LL106;_LL105: goto _LL107;_LL106: if((int)
_tmp1B8 != 5)goto _LL108;_LL107: goto _LL109;_LL108: if((int)_tmp1B8 != 3)goto _LL10A;
_LL109: return 1;_LL10A: if((int)_tmp1B8 != 4)goto _LLFF;_LL10B:{const char*_tmp35C;
void*_tmp35B;(_tmp35B=0,Cyc_Tcutil_warn(loc,((_tmp35C="nested extern \"C\" declaration",
_tag_dyneither(_tmp35C,sizeof(char),30))),_tag_dyneither(_tmp35B,sizeof(void*),0)));}
return 1;_LLFF:;}static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*
te,struct _tuple6*exp);static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*
te,struct _tuple6*exp){struct Cyc_Position_Segment*_tmp1C0;struct _tuple0*_tmp1C1;
struct _tuple6 _tmp1BF=*exp;_tmp1C0=_tmp1BF.f1;_tmp1C1=_tmp1BF.f2;{struct _tuple0
_tmp1C3;union Cyc_Absyn_Nmspace_union _tmp1C4;struct _dyneither_ptr*_tmp1C5;struct
_tuple0*_tmp1C2=_tmp1C1;_tmp1C3=*_tmp1C2;_tmp1C4=_tmp1C3.f1;_tmp1C5=_tmp1C3.f2;{
union Cyc_Absyn_Nmspace_union _tmp1C6=_tmp1C4;struct Cyc_List_List*_tmp1C7;struct
Cyc_List_List*_tmp1C8;_LL10D: if((_tmp1C6.Rel_n).tag != 1)goto _LL10F;_tmp1C7=(
_tmp1C6.Rel_n).f1;if(_tmp1C7 != 0)goto _LL10F;_LL10E: goto _LL110;_LL10F: if((_tmp1C6.Abs_n).tag
!= 2)goto _LL111;_tmp1C8=(_tmp1C6.Abs_n).f1;if(_tmp1C8 != 0)goto _LL111;_LL110: goto
_LL10C;_LL111:;_LL112:{const char*_tmp360;void*_tmp35F[1];struct Cyc_String_pa_struct
_tmp35E;(_tmp35E.tag=0,((_tmp35E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp1C1)),((_tmp35F[0]=& _tmp35E,Cyc_Tcutil_terr(_tmp1C0,((
_tmp360="qualified export variables are not implemented (%s)",_tag_dyneither(
_tmp360,sizeof(char),52))),_tag_dyneither(_tmp35F,sizeof(void*),1)))))));}
return;_LL10C:;}{union Cyc_Absyn_Nmspace_union _tmp361;(*_tmp1C1).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp361.Abs_n).tag=2,(((_tmp361.Abs_n).f1=te->ns,_tmp361))));}}}static void Cyc_Tc_tc_decls(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int
in_externCinclude,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**
exports);static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
ds0,int in_externC,int in_externCinclude,int check_var_init,struct _RegionHandle*
grgn,struct Cyc_List_List**exports){struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_List_List*last=0;struct Cyc_Dict_Dict dict=Cyc_Tcgenrep_empty_typerep_dict();
struct Cyc_List_List*_tmp1CD=ds0;for(0;_tmp1CD != 0;(last=_tmp1CD,_tmp1CD=_tmp1CD->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp1CD->hd;struct Cyc_Position_Segment*
loc=d->loc;void*_tmp1CE=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp1CF;struct Cyc_Absyn_Fndecl*
_tmp1D0;struct Cyc_Absyn_Typedefdecl*_tmp1D1;struct Cyc_Absyn_Aggrdecl*_tmp1D2;
struct Cyc_Absyn_Tuniondecl*_tmp1D3;struct Cyc_Absyn_Enumdecl*_tmp1D4;struct
_dyneither_ptr*_tmp1D5;struct Cyc_List_List*_tmp1D6;struct _tuple0*_tmp1D7;struct
_tuple0 _tmp1D8;union Cyc_Absyn_Nmspace_union _tmp1D9;struct _dyneither_ptr*_tmp1DA;
struct Cyc_List_List*_tmp1DB;struct Cyc_List_List*_tmp1DC;struct Cyc_List_List*
_tmp1DD;struct Cyc_List_List*_tmp1DE;_LL114: if(_tmp1CE <= (void*)2)goto _LL124;if(*((
int*)_tmp1CE)!= 2)goto _LL116;_LL115: goto _LL117;_LL116: if(*((int*)_tmp1CE)!= 3)
goto _LL118;_LL117:{const char*_tmp364;void*_tmp363;(_tmp363=0,Cyc_Tcutil_terr(loc,((
_tmp364="top level let-declarations are not implemented",_tag_dyneither(_tmp364,
sizeof(char),47))),_tag_dyneither(_tmp363,sizeof(void*),0)));}goto _LL113;_LL118:
if(*((int*)_tmp1CE)!= 0)goto _LL11A;_tmp1CF=((struct Cyc_Absyn_Var_d_struct*)
_tmp1CE)->f1;_LL119: if(in_externC  && Cyc_Tc_okay_externC(d->loc,(void*)_tmp1CF->sc))(
void*)(_tmp1CF->sc=(void*)((void*)4));if(_tmp1CF->initializer != 0){void*_tmp1E1=(
void*)((struct Cyc_Absyn_Exp*)_check_null(_tmp1CF->initializer))->r;void*_tmp1E2;
_LL131: if(*((int*)_tmp1E1)!= 21)goto _LL133;_tmp1E2=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)
_tmp1E1)->f2;_LL132: {struct Cyc_Dict_Dict _tmp1E4;struct Cyc_List_List*_tmp1E5;
struct Cyc_Absyn_Exp*_tmp1E6;struct _tuple5 _tmp1E3=Cyc_Tcgenrep_tcGenrep(te,ge,loc,
_tmp1E2,dict);_tmp1E4=_tmp1E3.f1;_tmp1E5=_tmp1E3.f2;_tmp1E6=_tmp1E3.f3;dict=
_tmp1E4;Cyc_Tc_tc_decls(te,_tmp1E5,in_externC,in_externCinclude,check_var_init,
grgn,exports);_tmp1CF->initializer=(struct Cyc_Absyn_Exp*)_tmp1E6;Cyc_Tc_tcVardecl(
te,ge,loc,_tmp1CF,check_var_init,in_externCinclude,exports);if(_tmp1E5 != 0){if(
last != 0){((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp1E5,_tmp1CD);last->tl=_tmp1E5;}else{struct Cyc_List_List
_tmp365;struct Cyc_List_List tmp=(_tmp365.hd=(struct Cyc_Absyn_Decl*)_tmp1CD->hd,((
_tmp365.tl=_tmp1CD->tl,_tmp365)));(struct Cyc_Absyn_Decl*)(_tmp1CD->hd=(void*)((
struct Cyc_Absyn_Decl*)_tmp1E5->hd));_tmp1CD->tl=_tmp1E5->tl;(struct Cyc_Absyn_Decl*)(
_tmp1E5->hd=(void*)((struct Cyc_Absyn_Decl*)tmp.hd));_tmp1E5->tl=tmp.tl;((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp1CD,_tmp1E5);}}continue;}_LL133:;_LL134: goto _LL130;_LL130:;}Cyc_Tc_tcVardecl(
te,ge,loc,_tmp1CF,check_var_init,in_externCinclude,exports);goto _LL113;_LL11A:
if(*((int*)_tmp1CE)!= 1)goto _LL11C;_tmp1D0=((struct Cyc_Absyn_Fn_d_struct*)
_tmp1CE)->f1;_LL11B: if(in_externC  && Cyc_Tc_okay_externC(d->loc,(void*)_tmp1D0->sc))(
void*)(_tmp1D0->sc=(void*)((void*)4));Cyc_Tc_tcFndecl(te,ge,loc,_tmp1D0,
in_externCinclude,exports);goto _LL113;_LL11C: if(*((int*)_tmp1CE)!= 7)goto _LL11E;
_tmp1D1=((struct Cyc_Absyn_Typedef_d_struct*)_tmp1CE)->f1;_LL11D: Cyc_Tc_tcTypedefdecl(
te,ge,loc,_tmp1D1);goto _LL113;_LL11E: if(*((int*)_tmp1CE)!= 4)goto _LL120;_tmp1D2=((
struct Cyc_Absyn_Aggr_d_struct*)_tmp1CE)->f1;_LL11F: if(in_externC  && Cyc_Tc_okay_externC(
d->loc,(void*)_tmp1D2->sc))(void*)(_tmp1D2->sc=(void*)((void*)4));Cyc_Tc_tcAggrdecl(
te,ge,loc,_tmp1D2);goto _LL113;_LL120: if(*((int*)_tmp1CE)!= 5)goto _LL122;_tmp1D3=((
struct Cyc_Absyn_Tunion_d_struct*)_tmp1CE)->f1;_LL121: if(in_externC  && Cyc_Tc_okay_externC(
d->loc,(void*)_tmp1D3->sc))(void*)(_tmp1D3->sc=(void*)((void*)4));Cyc_Tc_tcTuniondecl(
te,ge,loc,_tmp1D3);goto _LL113;_LL122: if(*((int*)_tmp1CE)!= 6)goto _LL124;_tmp1D4=((
struct Cyc_Absyn_Enum_d_struct*)_tmp1CE)->f1;_LL123: if(in_externC  && Cyc_Tc_okay_externC(
d->loc,(void*)_tmp1D4->sc))(void*)(_tmp1D4->sc=(void*)((void*)4));Cyc_Tc_tcEnumdecl(
te,ge,loc,_tmp1D4);goto _LL113;_LL124: if((int)_tmp1CE != 0)goto _LL126;_LL125:{
const char*_tmp368;void*_tmp367;(_tmp367=0,Cyc_Tcutil_warn(d->loc,((_tmp368="spurious __cyclone_port_on__",
_tag_dyneither(_tmp368,sizeof(char),29))),_tag_dyneither(_tmp367,sizeof(void*),0)));}
goto _LL113;_LL126: if((int)_tmp1CE != 1)goto _LL128;_LL127: goto _LL113;_LL128: if(
_tmp1CE <= (void*)2)goto _LL12A;if(*((int*)_tmp1CE)!= 8)goto _LL12A;_tmp1D5=((
struct Cyc_Absyn_Namespace_d_struct*)_tmp1CE)->f1;_tmp1D6=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp1CE)->f2;_LL129: {struct Cyc_List_List*_tmp1EA=te->ns;struct Cyc_List_List*
_tmp369;struct Cyc_List_List*_tmp1EB=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1EA,((_tmp369=_cycalloc(sizeof(*
_tmp369)),((_tmp369->hd=_tmp1D5,((_tmp369->tl=0,_tmp369)))))));if(!((int(*)(
struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ge->namespaces,
_tmp1D5)){ge->namespaces=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*
s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(grgn,ge->namespaces,_tmp1D5);te->ae=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*
v))Cyc_Dict_insert)(te->ae,_tmp1EB,Cyc_Tcenv_empty_genv(grgn));}te->ns=_tmp1EB;
Cyc_Tc_tc_decls(te,_tmp1D6,in_externC,in_externCinclude,check_var_init,grgn,
exports);te->ns=_tmp1EA;goto _LL113;}_LL12A: if(_tmp1CE <= (void*)2)goto _LL12C;if(*((
int*)_tmp1CE)!= 9)goto _LL12C;_tmp1D7=((struct Cyc_Absyn_Using_d_struct*)_tmp1CE)->f1;
_tmp1D8=*_tmp1D7;_tmp1D9=_tmp1D8.f1;_tmp1DA=_tmp1D8.f2;_tmp1DB=((struct Cyc_Absyn_Using_d_struct*)
_tmp1CE)->f2;_LL12B: {struct _dyneither_ptr*first;struct Cyc_List_List*rest;{union
Cyc_Absyn_Nmspace_union _tmp1ED=_tmp1D9;struct Cyc_List_List*_tmp1EE;struct Cyc_List_List*
_tmp1EF;struct Cyc_List_List*_tmp1F0;struct Cyc_List_List _tmp1F1;struct
_dyneither_ptr*_tmp1F2;struct Cyc_List_List*_tmp1F3;struct Cyc_List_List*_tmp1F4;
struct Cyc_List_List _tmp1F5;struct _dyneither_ptr*_tmp1F6;struct Cyc_List_List*
_tmp1F7;_LL136: if((_tmp1ED.Loc_n).tag != 0)goto _LL138;_LL137: goto _LL139;_LL138:
if((_tmp1ED.Rel_n).tag != 1)goto _LL13A;_tmp1EE=(_tmp1ED.Rel_n).f1;if(_tmp1EE != 0)
goto _LL13A;_LL139: goto _LL13B;_LL13A: if((_tmp1ED.Abs_n).tag != 2)goto _LL13C;
_tmp1EF=(_tmp1ED.Abs_n).f1;if(_tmp1EF != 0)goto _LL13C;_LL13B: first=_tmp1DA;rest=0;
goto _LL135;_LL13C: if((_tmp1ED.Rel_n).tag != 1)goto _LL13E;_tmp1F0=(_tmp1ED.Rel_n).f1;
if(_tmp1F0 == 0)goto _LL13E;_tmp1F1=*_tmp1F0;_tmp1F2=(struct _dyneither_ptr*)
_tmp1F1.hd;_tmp1F3=_tmp1F1.tl;_LL13D: _tmp1F6=_tmp1F2;_tmp1F7=_tmp1F3;goto _LL13F;
_LL13E: if((_tmp1ED.Abs_n).tag != 2)goto _LL135;_tmp1F4=(_tmp1ED.Abs_n).f1;if(
_tmp1F4 == 0)goto _LL135;_tmp1F5=*_tmp1F4;_tmp1F6=(struct _dyneither_ptr*)_tmp1F5.hd;
_tmp1F7=_tmp1F5.tl;_LL13F: first=_tmp1F6;{struct Cyc_List_List*_tmp36A;rest=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1F7,((_tmp36A=_cycalloc(sizeof(*_tmp36A)),((_tmp36A->hd=_tmp1DA,((_tmp36A->tl=
0,_tmp36A)))))));}goto _LL135;_LL135:;}{struct Cyc_List_List*_tmp1F9=Cyc_Tcenv_resolve_namespace(
te,loc,first,rest);{struct Cyc_List_List*_tmp36B;ge->availables=(struct Cyc_List_List*)((
_tmp36B=_cycalloc(sizeof(*_tmp36B)),((_tmp36B->hd=_tmp1F9,((_tmp36B->tl=ge->availables,
_tmp36B))))));}Cyc_Tc_tc_decls(te,_tmp1DB,in_externC,in_externCinclude,
check_var_init,grgn,exports);ge->availables=((struct Cyc_List_List*)_check_null(
ge->availables))->tl;goto _LL113;}}_LL12C: if(_tmp1CE <= (void*)2)goto _LL12E;if(*((
int*)_tmp1CE)!= 10)goto _LL12E;_tmp1DC=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp1CE)->f1;_LL12D: Cyc_Tc_tc_decls(te,_tmp1DC,1,in_externCinclude,
check_var_init,grgn,exports);goto _LL113;_LL12E: if(_tmp1CE <= (void*)2)goto _LL113;
if(*((int*)_tmp1CE)!= 11)goto _LL113;_tmp1DD=((struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp1CE)->f1;_tmp1DE=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp1CE)->f2;
_LL12F:((void(*)(void(*f)(struct Cyc_Tcenv_Tenv*,struct _tuple6*),struct Cyc_Tcenv_Tenv*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tc_resolve_export_namespace,te,
_tmp1DE);{struct Cyc_List_List*newexs=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1DE,(unsigned int)exports?*exports:
0);Cyc_Tc_tc_decls(te,_tmp1DD,1,1,check_var_init,grgn,(struct Cyc_List_List**)&
newexs);for(0;_tmp1DE != 0;_tmp1DE=_tmp1DE->tl){struct _tuple6*_tmp1FB=(struct
_tuple6*)_tmp1DE->hd;if(!(*_tmp1FB).f3){const char*_tmp36F;void*_tmp36E[1];struct
Cyc_String_pa_struct _tmp36D;(_tmp36D.tag=0,((_tmp36D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string((*_tmp1FB).f2)),((_tmp36E[0]=&
_tmp36D,Cyc_Tcutil_warn((*_tmp1FB).f1,((_tmp36F="%s is exported but not defined",
_tag_dyneither(_tmp36F,sizeof(char),31))),_tag_dyneither(_tmp36E,sizeof(void*),1)))))));}}
goto _LL113;}_LL113:;}}void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*
te,int check_var_init,struct Cyc_List_List*ds);void Cyc_Tc_tc(struct _RegionHandle*g,
struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){Cyc_Absynpp_set_params(&
Cyc_Absynpp_tc_params_r);Cyc_Tc_tc_decls(te,ds,0,0,check_var_init,g,0);}struct
Cyc_Tc_TreeshakeEnv{struct _RegionHandle*rgn;int in_cinclude;struct Cyc_Dict_Dict
nsdict;};static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*
d);static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,
struct Cyc_List_List*ds);static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*
env,struct Cyc_List_List*ds){return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,
struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(
Cyc_Tc_vardecl_needed,env,ds);}struct _tuple12{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*
f2;};static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*
d);static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*
d){void*_tmp1FF=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp200;struct Cyc_List_List*
_tmp201;struct Cyc_List_List**_tmp202;struct Cyc_List_List*_tmp203;struct Cyc_List_List**
_tmp204;struct Cyc_List_List*_tmp205;struct Cyc_List_List**_tmp206;struct Cyc_List_List*
_tmp207;struct Cyc_List_List**_tmp208;_LL141: if(_tmp1FF <= (void*)2)goto _LL14B;if(*((
int*)_tmp1FF)!= 0)goto _LL143;_tmp200=((struct Cyc_Absyn_Var_d_struct*)_tmp1FF)->f1;
_LL142: if(env->in_cinclude  || (void*)_tmp200->sc != (void*)3  && (void*)_tmp200->sc
!= (void*)4)return 1;{struct _tuple0 _tmp20A;union Cyc_Absyn_Nmspace_union _tmp20B;
struct _dyneither_ptr*_tmp20C;struct _tuple0*_tmp209=_tmp200->name;_tmp20A=*
_tmp209;_tmp20B=_tmp20A.f1;_tmp20C=_tmp20A.f2;{struct Cyc_List_List*ns;{union Cyc_Absyn_Nmspace_union
_tmp20D=_tmp20B;struct Cyc_List_List*_tmp20E;struct Cyc_List_List*_tmp20F;_LL14E:
if((_tmp20D.Loc_n).tag != 0)goto _LL150;_LL14F: ns=0;goto _LL14D;_LL150: if((_tmp20D.Rel_n).tag
!= 1)goto _LL152;_tmp20E=(_tmp20D.Rel_n).f1;_LL151: ns=_tmp20E;goto _LL14D;_LL152:
if((_tmp20D.Abs_n).tag != 2)goto _LL14D;_tmp20F=(_tmp20D.Abs_n).f1;_LL153: ns=
_tmp20F;goto _LL14D;_LL14D:;}{struct _tuple12*_tmp210=((struct _tuple12*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(env->nsdict,ns);struct Cyc_Tcenv_Genv*
_tmp211=(*_tmp210).f1;int _tmp212=(*((struct _tuple7*(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp211->ordinaries,_tmp20C)).f2;if(!
_tmp212)(*_tmp210).f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*
s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(env->rgn,(*_tmp210).f2,_tmp20C);
return _tmp212;}}}_LL143: if(*((int*)_tmp1FF)!= 10)goto _LL145;_tmp201=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp1FF)->f1;_tmp202=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_struct*)
_tmp1FF)->f1;_LL144: _tmp204=_tmp202;goto _LL146;_LL145: if(*((int*)_tmp1FF)!= 9)
goto _LL147;_tmp203=((struct Cyc_Absyn_Using_d_struct*)_tmp1FF)->f2;_tmp204=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_struct*)_tmp1FF)->f2;_LL146:
_tmp206=_tmp204;goto _LL148;_LL147: if(*((int*)_tmp1FF)!= 8)goto _LL149;_tmp205=((
struct Cyc_Absyn_Namespace_d_struct*)_tmp1FF)->f2;_tmp206=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Namespace_d_struct*)_tmp1FF)->f2;_LL148:*_tmp206=Cyc_Tc_treeshake_f(
env,*_tmp206);return 1;_LL149: if(*((int*)_tmp1FF)!= 11)goto _LL14B;_tmp207=((
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp1FF)->f1;_tmp208=(struct Cyc_List_List**)&((
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp1FF)->f1;_LL14A: {int in_cinclude=
env->in_cinclude;env->in_cinclude=1;*_tmp208=Cyc_Tc_treeshake_f(env,*_tmp208);
env->in_cinclude=in_cinclude;return 1;}_LL14B:;_LL14C: return 1;_LL140:;}static int
Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,void*y);
static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,
void*y){return !((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
set,x);}static struct _tuple12*Cyc_Tc_treeshake_make_env_f(struct _RegionHandle*rgn,
struct Cyc_Tcenv_Genv*ge);static struct _tuple12*Cyc_Tc_treeshake_make_env_f(struct
_RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge){struct _tuple12*_tmp370;return(_tmp370=
_region_malloc(rgn,sizeof(*_tmp370)),((_tmp370->f1=ge,((_tmp370->f2=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Set_rempty)(rgn,Cyc_strptrcmp),_tmp370)))));}struct _tuple13{struct Cyc_List_List*
f1;struct _tuple12*f2;};struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*ds){struct _RegionHandle _tmp214=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp214;_push_region(rgn);{struct Cyc_Tc_TreeshakeEnv _tmp371;
struct Cyc_Tc_TreeshakeEnv _tmp215=(_tmp371.rgn=rgn,((_tmp371.in_cinclude=0,((
_tmp371.nsdict=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct _tuple12*(*f)(
struct _RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict
d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae),_tmp371)))));
struct Cyc_List_List*_tmp216=Cyc_Tc_treeshake_f(& _tmp215,ds);if(((int(*)(struct
Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp215.nsdict)){struct Cyc_List_List*_tmp217=
_tmp216;_npop_handler(0);return _tmp217;}{struct Cyc_Iter_Iter _tmp218=((struct Cyc_Iter_Iter(*)(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,
_tmp215.nsdict);struct _tuple13 _tmp219=*((struct _tuple13*(*)(struct _RegionHandle*
r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(rgn,_tmp215.nsdict);while(((int(*)(
struct Cyc_Iter_Iter,struct _tuple13*))Cyc_Iter_next)(_tmp218,& _tmp219)){struct
_tuple12*_tmp21B;struct _tuple12 _tmp21C;struct Cyc_Tcenv_Genv*_tmp21D;struct Cyc_Set_Set*
_tmp21E;struct _tuple13 _tmp21A=_tmp219;_tmp21B=_tmp21A.f2;_tmp21C=*_tmp21B;
_tmp21D=_tmp21C.f1;_tmp21E=_tmp21C.f2;_tmp21D->ordinaries=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*f)(struct Cyc_Set_Set*,struct _dyneither_ptr*,struct
_tuple7*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(
_tmp21D->grgn,(int(*)(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,struct _tuple7*
y))Cyc_Tc_treeshake_remove_f,_tmp21E,_tmp21D->ordinaries);}{struct Cyc_List_List*
_tmp21F=_tmp216;_npop_handler(0);return _tmp21F;}}};_pop_region(rgn);}
