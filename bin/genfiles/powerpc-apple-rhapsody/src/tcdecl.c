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
 struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_NewRegion{
struct _DynRegionHandle*dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[16];
extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dyneither_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,
struct _dyneither_ptr);extern char Cyc_Position_Nocontext[14];void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple0{union Cyc_Absyn_Nmspace_union
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;int is_flat;};
struct Cyc_Absyn_UnknownDatatype_struct{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
f1;};struct Cyc_Absyn_KnownDatatype_struct{int tag;struct Cyc_Absyn_Datatypedecl**
f1;};union Cyc_Absyn_DatatypeInfoU_union{struct Cyc_Absyn_UnknownDatatype_struct
UnknownDatatype;struct Cyc_Absyn_KnownDatatype_struct KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU_union datatype_info;struct Cyc_List_List*targs;struct
Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*
datatype_name;struct _tuple0*field_name;int is_extensible;};struct Cyc_Absyn_UnknownDatatypefield_struct{
int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo f1;};struct Cyc_Absyn_KnownDatatypefield_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};union
Cyc_Absyn_DatatypeFieldInfoU_union{struct Cyc_Absyn_UnknownDatatypefield_struct
UnknownDatatypefield;struct Cyc_Absyn_KnownDatatypefield_struct KnownDatatypefield;
};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*
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
struct Cyc_Core_Opt*fields;int is_extensible;int is_flat;};struct Cyc_Absyn_Enumfield{
struct _tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};
struct Cyc_Absyn_Enumdecl{void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};
struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*
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
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple0*,struct _tuple0*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);void*Cyc_Absyn_force_kb(void*kb);struct _dyneither_ptr Cyc_Absyn_attribute2string(
void*);extern char Cyc_Tcdecl_Incompatible[17];struct Cyc_Tcdecl_Xdatatypefielddecl{
struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};void Cyc_Tcdecl_merr(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);struct _tuple3{void*f1;int f2;};struct _tuple3 Cyc_Tcdecl_merge_scope(
void*s0,void*s1,struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg,int allow_externC_extern_merge);struct Cyc_Absyn_Aggrdecl*
Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Datatypedecl*
Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Enumdecl*
Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Typedefdecl*
Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);void*Cyc_Tcdecl_merge_binding(
void*d0,void*d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct
Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*
d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg);struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct
Cyc_List_List*f,int*res,struct _dyneither_ptr*v,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr*msg);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple4{void*f1;void*f2;};struct _tuple4*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple4*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(struct
_dyneither_ptr,struct _dyneither_ptr);struct Cyc_Set_Set;extern char Cyc_Set_Absent[
11];struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
le;int allow_valueof;};void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,
void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);int Cyc_Tcutil_same_atts(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(void*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*);struct _dyneither_ptr Cyc_Absynpp_scope2string(void*sc);char Cyc_Tcdecl_Incompatible[
17]="\000\000\000\000Incompatible\000";void Cyc_Tcdecl_merr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
static void _tmp193(struct _dyneither_ptr**msg1,struct _dyneither_ptr*ap,
unsigned int*_tmp192,unsigned int*_tmp191,void***_tmp18F){for(*_tmp192=0;*
_tmp192 < *_tmp191;(*_tmp192)++){struct Cyc_String_pa_struct _tmp18D;struct Cyc_String_pa_struct*
_tmp18C;(*_tmp18F)[*_tmp192]=*_tmp192 == 0?(void*)((_tmp18C=_cycalloc(sizeof(*
_tmp18C)),((_tmp18C[0]=((_tmp18D.tag=0,((_tmp18D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*msg1)),_tmp18D)))),_tmp18C)))):*((void**)
_check_dyneither_subscript(*ap,sizeof(void*),(int)(*_tmp192 - 1)));}}void Cyc_Tcdecl_merr(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap){if(msg1 == 0)(int)_throw((void*)Cyc_Tcdecl_Incompatible);{
const char*_tmp18A;struct _dyneither_ptr fmt2=(struct _dyneither_ptr)Cyc_strconcat(((
_tmp18A="%s ",_tag_dyneither(_tmp18A,sizeof(char),4))),(struct _dyneither_ptr)fmt);
unsigned int _tmp192;unsigned int _tmp191;struct _dyneither_ptr _tmp190;void**
_tmp18F;unsigned int _tmp18E;struct _dyneither_ptr ap2=(_tmp18E=_get_dyneither_size(
ap,sizeof(void*))+ 1,((_tmp18F=(void**)_cycalloc(_check_times(sizeof(void*),
_tmp18E)),((_tmp190=_tag_dyneither(_tmp18F,sizeof(void*),_tmp18E),((((_tmp191=
_tmp18E,_tmp193(& msg1,& ap,& _tmp192,& _tmp191,& _tmp18F))),_tmp190)))))));Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt2,ap2)));}}static void Cyc_Tcdecl_merge_scope_err(void*s0,void*s1,struct
_dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg);static void Cyc_Tcdecl_merge_scope_err(void*s0,void*s1,struct
_dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){const char*_tmp19A;void*_tmp199[4];struct Cyc_String_pa_struct
_tmp198;struct Cyc_String_pa_struct _tmp197;struct Cyc_String_pa_struct _tmp196;
struct Cyc_String_pa_struct _tmp195;(_tmp195.tag=0,((_tmp195.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_scope2string(s0)),((_tmp196.tag=
0,((_tmp196.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_scope2string(
s1)),((_tmp197.tag=0,((_tmp197.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((
_tmp198.tag=0,((_tmp198.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((
_tmp199[0]=& _tmp198,((_tmp199[1]=& _tmp197,((_tmp199[2]=& _tmp196,((_tmp199[3]=&
_tmp195,Cyc_Tcdecl_merr(loc,msg,((_tmp19A="%s %s is %s whereas expected scope is %s",
_tag_dyneither(_tmp19A,sizeof(char),41))),_tag_dyneither(_tmp199,sizeof(void*),4)))))))))))))))))))))))));}
struct _tuple3 Cyc_Tcdecl_merge_scope(void*s0,void*s1,struct _dyneither_ptr t,struct
_dyneither_ptr v,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg,int
externCmerge);struct _tuple3 Cyc_Tcdecl_merge_scope(void*s0,void*s1,struct
_dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg,int externCmerge){{struct _tuple4 _tmp19B;struct _tuple4 _tmpE=(
_tmp19B.f1=s0,((_tmp19B.f2=s1,_tmp19B)));void*_tmpF;void*_tmp10;void*_tmp11;void*
_tmp12;void*_tmp13;void*_tmp14;void*_tmp15;void*_tmp16;void*_tmp17;void*_tmp18;
void*_tmp19;void*_tmp1A;void*_tmp1B;void*_tmp1C;void*_tmp1D;void*_tmp1E;void*
_tmp1F;void*_tmp20;_LL1: _tmpF=_tmpE.f1;if((int)_tmpF != 4)goto _LL3;_tmp10=_tmpE.f2;
if((int)_tmp10 != 4)goto _LL3;_LL2: goto _LL0;_LL3: _tmp11=_tmpE.f1;if((int)_tmp11 != 
4)goto _LL5;_tmp12=_tmpE.f2;if((int)_tmp12 != 3)goto _LL5;_LL4: goto _LL6;_LL5: _tmp13=
_tmpE.f1;if((int)_tmp13 != 3)goto _LL7;_tmp14=_tmpE.f2;if((int)_tmp14 != 4)goto _LL7;
_LL6: if(externCmerge)goto _LL0;goto _LL8;_LL7: _tmp15=_tmpE.f1;if((int)_tmp15 != 4)
goto _LL9;_LL8: goto _LLA;_LL9: _tmp16=_tmpE.f2;if((int)_tmp16 != 4)goto _LLB;_LLA: Cyc_Tcdecl_merge_scope_err(
s0,s1,t,v,loc,msg);{struct _tuple3 _tmp19C;return(_tmp19C.f1=s1,((_tmp19C.f2=0,
_tmp19C)));}_LLB: _tmp17=_tmpE.f2;if((int)_tmp17 != 3)goto _LLD;_LLC: s1=s0;goto _LL0;
_LLD: _tmp18=_tmpE.f1;if((int)_tmp18 != 3)goto _LLF;_LLE: goto _LL0;_LLF: _tmp19=_tmpE.f1;
if((int)_tmp19 != 0)goto _LL11;_tmp1A=_tmpE.f2;if((int)_tmp1A != 0)goto _LL11;_LL10:
goto _LL12;_LL11: _tmp1B=_tmpE.f1;if((int)_tmp1B != 2)goto _LL13;_tmp1C=_tmpE.f2;if((
int)_tmp1C != 2)goto _LL13;_LL12: goto _LL14;_LL13: _tmp1D=_tmpE.f1;if((int)_tmp1D != 
1)goto _LL15;_tmp1E=_tmpE.f2;if((int)_tmp1E != 1)goto _LL15;_LL14: goto _LL0;_LL15:
_tmp1F=_tmpE.f1;if((int)_tmp1F != 5)goto _LL17;_tmp20=_tmpE.f2;if((int)_tmp20 != 5)
goto _LL17;_LL16: goto _LL0;_LL17:;_LL18: Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,
msg);{struct _tuple3 _tmp19D;return(_tmp19D.f1=s1,((_tmp19D.f2=0,_tmp19D)));}_LL0:;}{
struct _tuple3 _tmp19E;return(_tmp19E.f1=s1,((_tmp19E.f2=1,_tmp19E)));}}static int
Cyc_Tcdecl_check_type(void*t0,void*t1);static int Cyc_Tcdecl_check_type(void*t0,
void*t1){return Cyc_Tcutil_unify(t0,t1);}static unsigned int Cyc_Tcdecl_get_uint_const_value(
struct Cyc_Absyn_Exp*e);static unsigned int Cyc_Tcdecl_get_uint_const_value(struct
Cyc_Absyn_Exp*e){void*_tmp24=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp25;int
_tmp26;_LL1A: if(*((int*)_tmp24)!= 0)goto _LL1C;_tmp25=((struct Cyc_Absyn_Const_e_struct*)
_tmp24)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp24)->f1).Int_c).tag != 2)
goto _LL1C;_tmp26=(_tmp25.Int_c).f2;_LL1B: return(unsigned int)_tmp26;_LL1C:;_LL1D: {
struct Cyc_Core_Invalid_argument_struct _tmp1A4;const char*_tmp1A3;struct Cyc_Core_Invalid_argument_struct*
_tmp1A2;(int)_throw((void*)((_tmp1A2=_cycalloc(sizeof(*_tmp1A2)),((_tmp1A2[0]=((
_tmp1A4.tag=Cyc_Core_Invalid_argument,((_tmp1A4.f1=((_tmp1A3="Tcdecl::get_uint_const_value",
_tag_dyneither(_tmp1A3,sizeof(char),29))),_tmp1A4)))),_tmp1A2)))));}_LL19:;}
static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,
struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr*msg);inline static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*
tvs0,struct Cyc_List_List*tvs1,struct _dyneither_ptr t,struct _dyneither_ptr v,struct
Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(tvs0)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
tvs1)){{const char*_tmp1A9;void*_tmp1A8[2];struct Cyc_String_pa_struct _tmp1A7;
struct Cyc_String_pa_struct _tmp1A6;(_tmp1A6.tag=0,((_tmp1A6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1A7.tag=0,((_tmp1A7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1A8[0]=& _tmp1A7,((_tmp1A8[1]=&
_tmp1A6,Cyc_Tcdecl_merr(loc,msg,((_tmp1A9="%s %s has a different number of type parameters",
_tag_dyneither(_tmp1A9,sizeof(char),48))),_tag_dyneither(_tmp1A8,sizeof(void*),2)))))))))))));}
return 0;}{struct Cyc_List_List*_tmp2E=tvs0;struct Cyc_List_List*_tmp2F=tvs1;for(0;
_tmp2E != 0;(_tmp2E=_tmp2E->tl,_tmp2F=_tmp2F->tl)){void*_tmp30=Cyc_Absyn_force_kb((
void*)((struct Cyc_Absyn_Tvar*)_tmp2E->hd)->kind);void*_tmp31=Cyc_Absyn_force_kb((
void*)((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp2F))->hd)->kind);
if(_tmp30 == _tmp31)continue;{const char*_tmp1B1;void*_tmp1B0[5];struct Cyc_String_pa_struct
_tmp1AF;struct Cyc_String_pa_struct _tmp1AE;struct Cyc_String_pa_struct _tmp1AD;
struct Cyc_String_pa_struct _tmp1AC;struct Cyc_String_pa_struct _tmp1AB;(_tmp1AB.tag=
0,((_tmp1AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp31)),((_tmp1AC.tag=0,((_tmp1AC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp2E->hd)->name),((_tmp1AD.tag=0,((
_tmp1AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp30)),((_tmp1AE.tag=0,((_tmp1AE.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)v),((_tmp1AF.tag=0,((_tmp1AF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)t),((_tmp1B0[0]=& _tmp1AF,((_tmp1B0[1]=& _tmp1AE,((_tmp1B0[2]=&
_tmp1AD,((_tmp1B0[3]=& _tmp1AC,((_tmp1B0[4]=& _tmp1AB,Cyc_Tcdecl_merr(loc,msg,((
_tmp1B1="%s %s has a different kind (%s) for type parameter %s (%s)",
_tag_dyneither(_tmp1B1,sizeof(char),59))),_tag_dyneither(_tmp1B0,sizeof(void*),5)))))))))))))))))))))))))))))));}
return 0;}return 1;}}static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,
struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*
atts0,struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){if(!Cyc_Tcutil_same_atts(
atts0,atts1)){{const char*_tmp1B6;void*_tmp1B5[2];struct Cyc_String_pa_struct
_tmp1B4;struct Cyc_String_pa_struct _tmp1B3;(_tmp1B3.tag=0,((_tmp1B3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1B4.tag=0,((_tmp1B4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1B5[0]=& _tmp1B4,((_tmp1B5[1]=&
_tmp1B3,Cyc_Tcdecl_merr(loc,msg,((_tmp1B6="%s %s has different attributes",
_tag_dyneither(_tmp1B6,sizeof(char),31))),_tag_dyneither(_tmp1B5,sizeof(void*),2)))))))))))));}
return 0;}return 1;}struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};static struct
Cyc_List_List*Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*tvs0,struct Cyc_List_List*
tvs1);static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*
tvs0,struct Cyc_List_List*tvs1){struct Cyc_List_List*inst=0;for(0;tvs0 != 0;(tvs0=
tvs0->tl,tvs1=tvs1->tl)){struct _tuple5*_tmp1C0;struct Cyc_Absyn_VarType_struct
_tmp1BF;struct Cyc_Absyn_VarType_struct*_tmp1BE;struct Cyc_List_List*_tmp1BD;inst=((
_tmp1BD=_cycalloc(sizeof(*_tmp1BD)),((_tmp1BD->hd=((_tmp1C0=_cycalloc(sizeof(*
_tmp1C0)),((_tmp1C0->f1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(tvs1))->hd,((_tmp1C0->f2=(void*)((_tmp1BE=_cycalloc(sizeof(*_tmp1BE)),((
_tmp1BE[0]=((_tmp1BF.tag=1,((_tmp1BF.f1=(struct Cyc_Absyn_Tvar*)tvs0->hd,_tmp1BF)))),
_tmp1BE)))),_tmp1C0)))))),((_tmp1BD->tl=inst,_tmp1BD))))));}return inst;}struct
_tuple6{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};struct
Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct
Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,
struct Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*
msg){struct _dyneither_ptr _tmp41=Cyc_Absynpp_qvar2string(d0->name);int _tmp42=1;
if(!((void*)d0->kind == (void*)d1->kind))return 0;{const char*_tmp1C1;if(!Cyc_Tcdecl_check_tvs(
d0->tvs,d1->tvs,((_tmp1C1="type",_tag_dyneither(_tmp1C1,sizeof(char),5))),_tmp41,
loc,msg))return 0;}{void*_tmp46;int _tmp47;const char*_tmp1C2;struct _tuple3 _tmp45=
Cyc_Tcdecl_merge_scope((void*)d0->sc,(void*)d1->sc,((_tmp1C2="type",
_tag_dyneither(_tmp1C2,sizeof(char),5))),_tmp41,loc,msg,1);_tmp46=_tmp45.f1;
_tmp47=_tmp45.f2;if(!_tmp47)_tmp42=0;{const char*_tmp1C3;if(!Cyc_Tcdecl_check_atts(
d0->attributes,d1->attributes,((_tmp1C3="type",_tag_dyneither(_tmp1C3,sizeof(
char),5))),_tmp41,loc,msg))_tmp42=0;}{struct Cyc_Absyn_Aggrdecl*d2;{struct _tuple6
_tmp1C4;struct _tuple6 _tmp4A=(_tmp1C4.f1=d0->impl,((_tmp1C4.f2=d1->impl,_tmp1C4)));
struct Cyc_Absyn_AggrdeclImpl*_tmp4B;struct Cyc_Absyn_AggrdeclImpl*_tmp4C;struct
Cyc_Absyn_AggrdeclImpl*_tmp4D;struct Cyc_Absyn_AggrdeclImpl _tmp4E;struct Cyc_List_List*
_tmp4F;struct Cyc_List_List*_tmp50;struct Cyc_List_List*_tmp51;int _tmp52;struct Cyc_Absyn_AggrdeclImpl*
_tmp53;struct Cyc_Absyn_AggrdeclImpl _tmp54;struct Cyc_List_List*_tmp55;struct Cyc_List_List*
_tmp56;struct Cyc_List_List*_tmp57;int _tmp58;_LL1F: _tmp4B=_tmp4A.f2;if(_tmp4B != 0)
goto _LL21;_LL20: d2=d0;goto _LL1E;_LL21: _tmp4C=_tmp4A.f1;if(_tmp4C != 0)goto _LL23;
_LL22: d2=d1;goto _LL1E;_LL23: _tmp4D=_tmp4A.f1;if(_tmp4D == 0)goto _LL1E;_tmp4E=*
_tmp4D;_tmp4F=_tmp4E.exist_vars;_tmp50=_tmp4E.rgn_po;_tmp51=_tmp4E.fields;_tmp52=
_tmp4E.tagged;_tmp53=_tmp4A.f2;if(_tmp53 == 0)goto _LL1E;_tmp54=*_tmp53;_tmp55=
_tmp54.exist_vars;_tmp56=_tmp54.rgn_po;_tmp57=_tmp54.fields;_tmp58=_tmp54.tagged;
_LL24:{const char*_tmp1C5;if(!Cyc_Tcdecl_check_tvs(_tmp4F,_tmp55,((_tmp1C5="type",
_tag_dyneither(_tmp1C5,sizeof(char),5))),_tmp41,loc,msg))return 0;}{struct Cyc_List_List*
_tmp5A=Cyc_Tcdecl_build_tvs_map(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(d0->tvs,_tmp4F),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d1->tvs,_tmp55));
for(0;_tmp50 != 0  && _tmp56 != 0;(_tmp50=_tmp50->tl,_tmp56=_tmp56->tl)){Cyc_Tcdecl_check_type((*((
struct _tuple4*)_tmp50->hd)).f1,(*((struct _tuple4*)_tmp56->hd)).f1);Cyc_Tcdecl_check_type((*((
struct _tuple4*)_tmp50->hd)).f2,(*((struct _tuple4*)_tmp56->hd)).f2);}for(0;_tmp51
!= 0  && _tmp57 != 0;(_tmp51=_tmp51->tl,_tmp57=_tmp57->tl)){struct Cyc_Absyn_Aggrfield
_tmp5C;struct _dyneither_ptr*_tmp5D;struct Cyc_Absyn_Tqual _tmp5E;void*_tmp5F;
struct Cyc_Absyn_Exp*_tmp60;struct Cyc_List_List*_tmp61;struct Cyc_Absyn_Aggrfield*
_tmp5B=(struct Cyc_Absyn_Aggrfield*)_tmp51->hd;_tmp5C=*_tmp5B;_tmp5D=_tmp5C.name;
_tmp5E=_tmp5C.tq;_tmp5F=(void*)_tmp5C.type;_tmp60=_tmp5C.width;_tmp61=_tmp5C.attributes;{
struct Cyc_Absyn_Aggrfield _tmp63;struct _dyneither_ptr*_tmp64;struct Cyc_Absyn_Tqual
_tmp65;void*_tmp66;struct Cyc_Absyn_Exp*_tmp67;struct Cyc_List_List*_tmp68;struct
Cyc_Absyn_Aggrfield*_tmp62=(struct Cyc_Absyn_Aggrfield*)_tmp57->hd;_tmp63=*_tmp62;
_tmp64=_tmp63.name;_tmp65=_tmp63.tq;_tmp66=(void*)_tmp63.type;_tmp67=_tmp63.width;
_tmp68=_tmp63.attributes;if(Cyc_strptrcmp(_tmp5D,_tmp64)!= 0){{const char*_tmp1CE;
const char*_tmp1CD;void*_tmp1CC[4];struct Cyc_String_pa_struct _tmp1CB;struct Cyc_String_pa_struct
_tmp1CA;struct Cyc_String_pa_struct _tmp1C9;struct Cyc_String_pa_struct _tmp1C8;(
_tmp1C8.tag=0,((_tmp1C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp64),((
_tmp1C9.tag=0,((_tmp1C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp5D),((
_tmp1CA.tag=0,((_tmp1CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp41),((
_tmp1CB.tag=0,((_tmp1CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)((
_tmp1CE="type",_tag_dyneither(_tmp1CE,sizeof(char),5)))),((_tmp1CC[0]=& _tmp1CB,((
_tmp1CC[1]=& _tmp1CA,((_tmp1CC[2]=& _tmp1C9,((_tmp1CC[3]=& _tmp1C8,Cyc_Tcdecl_merr(
loc,msg,((_tmp1CD="%s %s : field name mismatch %s != %s",_tag_dyneither(_tmp1CD,
sizeof(char),37))),_tag_dyneither(_tmp1CC,sizeof(void*),4)))))))))))))))))))))))));}
return 0;}if(!Cyc_Tcutil_same_atts(_tmp61,_tmp68)){{const char*_tmp1D6;const char*
_tmp1D5;void*_tmp1D4[3];struct Cyc_String_pa_struct _tmp1D3;struct Cyc_String_pa_struct
_tmp1D2;struct Cyc_String_pa_struct _tmp1D1;(_tmp1D1.tag=0,((_tmp1D1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp5D),((_tmp1D2.tag=0,((_tmp1D2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp41),((_tmp1D3.tag=0,((_tmp1D3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)((_tmp1D6="type",_tag_dyneither(
_tmp1D6,sizeof(char),5)))),((_tmp1D4[0]=& _tmp1D3,((_tmp1D4[1]=& _tmp1D2,((_tmp1D4[
2]=& _tmp1D1,Cyc_Tcdecl_merr(loc,msg,((_tmp1D5="%s %s : attribute mismatch on field %s",
_tag_dyneither(_tmp1D5,sizeof(char),39))),_tag_dyneither(_tmp1D4,sizeof(void*),3)))))))))))))))))));}
_tmp42=0;}if(!Cyc_Tcutil_equal_tqual(_tmp5E,_tmp65)){{const char*_tmp1DE;const
char*_tmp1DD;void*_tmp1DC[3];struct Cyc_String_pa_struct _tmp1DB;struct Cyc_String_pa_struct
_tmp1DA;struct Cyc_String_pa_struct _tmp1D9;(_tmp1D9.tag=0,((_tmp1D9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp5D),((_tmp1DA.tag=0,((_tmp1DA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp41),((_tmp1DB.tag=0,((_tmp1DB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)((_tmp1DE="type",_tag_dyneither(
_tmp1DE,sizeof(char),5)))),((_tmp1DC[0]=& _tmp1DB,((_tmp1DC[1]=& _tmp1DA,((_tmp1DC[
2]=& _tmp1D9,Cyc_Tcdecl_merr(loc,msg,((_tmp1DD="%s %s : qualifier mismatch on field %s",
_tag_dyneither(_tmp1DD,sizeof(char),39))),_tag_dyneither(_tmp1DC,sizeof(void*),3)))))))))))))))))));}
_tmp42=0;}if(((_tmp60 != 0  && _tmp67 != 0) && Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)_tmp60)!= Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)
_tmp67) || _tmp60 == 0  && _tmp67 != 0) || _tmp60 != 0  && _tmp67 == 0){{const char*
_tmp1E6;const char*_tmp1E5;void*_tmp1E4[3];struct Cyc_String_pa_struct _tmp1E3;
struct Cyc_String_pa_struct _tmp1E2;struct Cyc_String_pa_struct _tmp1E1;(_tmp1E1.tag=
0,((_tmp1E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp5D),((_tmp1E2.tag=
0,((_tmp1E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp41),((_tmp1E3.tag=
0,((_tmp1E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)((_tmp1E6="type",
_tag_dyneither(_tmp1E6,sizeof(char),5)))),((_tmp1E4[0]=& _tmp1E3,((_tmp1E4[1]=&
_tmp1E2,((_tmp1E4[2]=& _tmp1E1,Cyc_Tcdecl_merr(loc,msg,((_tmp1E5="%s %s : bitfield mismatch on field %s",
_tag_dyneither(_tmp1E5,sizeof(char),38))),_tag_dyneither(_tmp1E4,sizeof(void*),3)))))))))))))))))));}
_tmp42=0;}{void*subst_t1=Cyc_Tcutil_substitute(_tmp5A,_tmp66);if(!Cyc_Tcdecl_check_type(
_tmp5F,subst_t1)){{const char*_tmp1ED;void*_tmp1EC[4];struct Cyc_String_pa_struct
_tmp1EB;struct Cyc_String_pa_struct _tmp1EA;struct Cyc_String_pa_struct _tmp1E9;
struct Cyc_String_pa_struct _tmp1E8;(_tmp1E8.tag=0,((_tmp1E8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_t1)),((
_tmp1E9.tag=0,((_tmp1E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp5F)),((_tmp1EA.tag=0,((_tmp1EA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp5D),((_tmp1EB.tag=0,((_tmp1EB.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp41),((_tmp1EC[0]=& _tmp1EB,((_tmp1EC[1]=& _tmp1EA,((
_tmp1EC[2]=& _tmp1E9,((_tmp1EC[3]=& _tmp1E8,Cyc_Tcdecl_merr(loc,msg,((_tmp1ED="type %s : type mismatch on field %s: %s != %s",
_tag_dyneither(_tmp1ED,sizeof(char),46))),_tag_dyneither(_tmp1EC,sizeof(void*),4)))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();_tmp42=0;}}}}if(_tmp51 != 0){{const char*_tmp1F2;void*
_tmp1F1[2];struct Cyc_String_pa_struct _tmp1F0;struct Cyc_String_pa_struct _tmp1EF;(
_tmp1EF.tag=0,((_tmp1EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Aggrfield*)_tmp51->hd)->name),((_tmp1F0.tag=0,((_tmp1F0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp41),((_tmp1F1[0]=& _tmp1F0,((
_tmp1F1[1]=& _tmp1EF,Cyc_Tcdecl_merr(loc,msg,((_tmp1F2="type %s is missing field %s",
_tag_dyneither(_tmp1F2,sizeof(char),28))),_tag_dyneither(_tmp1F1,sizeof(void*),2)))))))))))));}
_tmp42=0;}if(_tmp57 != 0){{const char*_tmp1F7;void*_tmp1F6[2];struct Cyc_String_pa_struct
_tmp1F5;struct Cyc_String_pa_struct _tmp1F4;(_tmp1F4.tag=0,((_tmp1F4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp57->hd)->name),((
_tmp1F5.tag=0,((_tmp1F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp41),((
_tmp1F6[0]=& _tmp1F5,((_tmp1F6[1]=& _tmp1F4,Cyc_Tcdecl_merr(loc,msg,((_tmp1F7="type %s has extra field %s",
_tag_dyneither(_tmp1F7,sizeof(char),27))),_tag_dyneither(_tmp1F6,sizeof(void*),2)))))))))))));}
_tmp42=0;}if(_tmp52 != _tmp58){{const char*_tmp1FB;void*_tmp1FA[1];struct Cyc_String_pa_struct
_tmp1F9;(_tmp1F9.tag=0,((_tmp1F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp41),((_tmp1FA[0]=& _tmp1F9,Cyc_Tcdecl_merr(loc,msg,((_tmp1FB="%s : previous declaration disagrees with @tagged qualifier",
_tag_dyneither(_tmp1FB,sizeof(char),59))),_tag_dyneither(_tmp1FA,sizeof(void*),1)))))));}
_tmp42=0;}d2=d0;goto _LL1E;}_LL1E:;}if(!_tmp42)return 0;if(_tmp46 == (void*)d2->sc)
return(struct Cyc_Absyn_Aggrdecl*)d2;else{{struct Cyc_Absyn_Aggrdecl*_tmp1FC;d2=((
_tmp1FC=_cycalloc(sizeof(*_tmp1FC)),((_tmp1FC[0]=*d2,_tmp1FC))));}(void*)(d2->sc=(
void*)_tmp46);return(struct Cyc_Absyn_Aggrdecl*)d2;}}}}static struct _dyneither_ptr
Cyc_Tcdecl_is_x2string(int is_x);inline static struct _dyneither_ptr Cyc_Tcdecl_is_x2string(
int is_x){const char*_tmp1FE;const char*_tmp1FD;return is_x?(_tmp1FE="@extensible datatype",
_tag_dyneither(_tmp1FE,sizeof(char),21)):((_tmp1FD="datatype",_tag_dyneither(
_tmp1FD,sizeof(char),9)));}struct _tuple7{struct Cyc_Absyn_Tqual f1;void*f2;};
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*
f0,struct Cyc_Absyn_Datatypefield*f1,struct Cyc_List_List*inst,struct
_dyneither_ptr t,struct _dyneither_ptr v,struct _dyneither_ptr*msg);static struct Cyc_Absyn_Datatypefield*
Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*f0,struct Cyc_Absyn_Datatypefield*
f1,struct Cyc_List_List*inst,struct _dyneither_ptr t,struct _dyneither_ptr v,struct
_dyneither_ptr*msg){struct Cyc_Position_Segment*loc=f1->loc;if(Cyc_strptrcmp((*f0->name).f2,(*
f1->name).f2)!= 0){{const char*_tmp205;void*_tmp204[4];struct Cyc_String_pa_struct
_tmp203;struct Cyc_String_pa_struct _tmp202;struct Cyc_String_pa_struct _tmp201;
struct Cyc_String_pa_struct _tmp200;(_tmp200.tag=0,((_tmp200.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*f0->name).f2),((_tmp201.tag=0,((_tmp201.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*f1->name).f2),((_tmp202.tag=0,((
_tmp202.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp203.tag=0,((
_tmp203.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp204[0]=& _tmp203,((
_tmp204[1]=& _tmp202,((_tmp204[2]=& _tmp201,((_tmp204[3]=& _tmp200,Cyc_Tcdecl_merr(
loc,msg,((_tmp205="%s %s: field name mismatch %s != %s",_tag_dyneither(_tmp205,
sizeof(char),36))),_tag_dyneither(_tmp204,sizeof(void*),4)))))))))))))))))))))))));}
return 0;}{struct _dyneither_ptr _tmp9C=*(*f0->name).f2;void*_tmpA2;int _tmpA3;const
char*_tmp20A;void*_tmp209[2];struct Cyc_String_pa_struct _tmp208;struct Cyc_String_pa_struct
_tmp207;struct _tuple3 _tmpA1=Cyc_Tcdecl_merge_scope((void*)f0->sc,(void*)f1->sc,(
struct _dyneither_ptr)((_tmp207.tag=0,((_tmp207.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)v),((_tmp208.tag=0,((_tmp208.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)t),((_tmp209[0]=& _tmp208,((_tmp209[1]=& _tmp207,Cyc_aprintf(((
_tmp20A="in %s %s, field",_tag_dyneither(_tmp20A,sizeof(char),16))),
_tag_dyneither(_tmp209,sizeof(void*),2)))))))))))))),_tmp9C,loc,msg,0);_tmpA2=
_tmpA1.f1;_tmpA3=_tmpA1.f2;{struct Cyc_List_List*_tmpA4=f0->typs;struct Cyc_List_List*
_tmpA5=f1->typs;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA4)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA5)){{const char*_tmp210;void*
_tmp20F[3];struct Cyc_String_pa_struct _tmp20E;struct Cyc_String_pa_struct _tmp20D;
struct Cyc_String_pa_struct _tmp20C;(_tmp20C.tag=0,((_tmp20C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp9C),((_tmp20D.tag=0,((_tmp20D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp20E.tag=0,((_tmp20E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp20F[0]=& _tmp20E,((_tmp20F[1]=&
_tmp20D,((_tmp20F[2]=& _tmp20C,Cyc_Tcdecl_merr(loc,msg,((_tmp210="%s %s, field %s: parameter number mismatch",
_tag_dyneither(_tmp210,sizeof(char),43))),_tag_dyneither(_tmp20F,sizeof(void*),3)))))))))))))))))));}
_tmpA3=0;}for(0;_tmpA4 != 0;(_tmpA4=_tmpA4->tl,_tmpA5=_tmpA5->tl)){if(!Cyc_Tcutil_equal_tqual((*((
struct _tuple7*)_tmpA4->hd)).f1,(*((struct _tuple7*)((struct Cyc_List_List*)
_check_null(_tmpA5))->hd)).f1)){{const char*_tmp216;void*_tmp215[3];struct Cyc_String_pa_struct
_tmp214;struct Cyc_String_pa_struct _tmp213;struct Cyc_String_pa_struct _tmp212;(
_tmp212.tag=0,((_tmp212.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9C),((
_tmp213.tag=0,((_tmp213.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((
_tmp214.tag=0,((_tmp214.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((
_tmp215[0]=& _tmp214,((_tmp215[1]=& _tmp213,((_tmp215[2]=& _tmp212,Cyc_Tcdecl_merr(
loc,msg,((_tmp216="%s %s, field %s: parameter qualifier",_tag_dyneither(_tmp216,
sizeof(char),37))),_tag_dyneither(_tmp215,sizeof(void*),3)))))))))))))))))));}
_tmpA3=0;}{void*subst_t1=Cyc_Tcutil_substitute(inst,(*((struct _tuple7*)_tmpA5->hd)).f2);
if(!Cyc_Tcdecl_check_type((*((struct _tuple7*)_tmpA4->hd)).f2,subst_t1)){{const
char*_tmp21E;void*_tmp21D[5];struct Cyc_String_pa_struct _tmp21C;struct Cyc_String_pa_struct
_tmp21B;struct Cyc_String_pa_struct _tmp21A;struct Cyc_String_pa_struct _tmp219;
struct Cyc_String_pa_struct _tmp218;(_tmp218.tag=0,((_tmp218.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_t1)),((
_tmp219.tag=0,((_tmp219.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((*((
struct _tuple7*)_tmpA4->hd)).f2)),((_tmp21A.tag=0,((_tmp21A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp9C),((_tmp21B.tag=0,((_tmp21B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp21C.tag=0,((_tmp21C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp21D[0]=& _tmp21C,((_tmp21D[1]=&
_tmp21B,((_tmp21D[2]=& _tmp21A,((_tmp21D[3]=& _tmp219,((_tmp21D[4]=& _tmp218,Cyc_Tcdecl_merr(
loc,msg,((_tmp21E="%s %s, field %s: parameter type mismatch %s != %s",
_tag_dyneither(_tmp21E,sizeof(char),50))),_tag_dyneither(_tmp21D,sizeof(void*),5)))))))))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();_tmpA3=0;}}}if(!_tmpA3)return 0;if((void*)f0->sc != 
_tmpA2){struct Cyc_Absyn_Datatypefield*_tmp21F;struct Cyc_Absyn_Datatypefield*
_tmpB7=(_tmp21F=_cycalloc(sizeof(*_tmp21F)),((_tmp21F[0]=*f0,_tmp21F)));(void*)(((
struct Cyc_Absyn_Datatypefield*)_check_null(_tmpB7))->sc=(void*)_tmpA2);return
_tmpB7;}else{return(struct Cyc_Absyn_Datatypefield*)f0;}}}}static struct _tuple7*
Cyc_Tcdecl_substitute_datatypefield_f2(struct Cyc_List_List*inst,struct _tuple7*x);
static struct _tuple7*Cyc_Tcdecl_substitute_datatypefield_f2(struct Cyc_List_List*
inst,struct _tuple7*x){struct _tuple7 _tmpBA;struct Cyc_Absyn_Tqual _tmpBB;void*
_tmpBC;struct _tuple7*_tmpB9=x;_tmpBA=*_tmpB9;_tmpBB=_tmpBA.f1;_tmpBC=_tmpBA.f2;{
struct _tuple7*_tmp220;return(_tmp220=_cycalloc(sizeof(*_tmp220)),((_tmp220->f1=
_tmpBB,((_tmp220->f2=Cyc_Tcutil_substitute(inst,_tmpBC),_tmp220)))));}}static
struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_substitute_datatypefield(struct Cyc_List_List*
inst1,struct Cyc_Absyn_Datatypefield*f1);static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_substitute_datatypefield(
struct Cyc_List_List*inst1,struct Cyc_Absyn_Datatypefield*f1){struct Cyc_Absyn_Datatypefield*
_tmp221;struct Cyc_Absyn_Datatypefield*_tmpBE=(_tmp221=_cycalloc(sizeof(*_tmp221)),((
_tmp221[0]=*f1,_tmp221)));_tmpBE->typs=((struct Cyc_List_List*(*)(struct _tuple7*(*
f)(struct Cyc_List_List*,struct _tuple7*),struct Cyc_List_List*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcdecl_substitute_datatypefield_f2,inst1,f1->typs);return
_tmpBE;}static struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(struct Cyc_List_List*
f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct Cyc_List_List*tvs0,
struct Cyc_List_List*tvs1,int*res,int*incl,struct _dyneither_ptr t,struct
_dyneither_ptr v,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);static
struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(struct Cyc_List_List*f0s,
struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct Cyc_List_List*tvs0,struct
Cyc_List_List*tvs1,int*res,int*incl,struct _dyneither_ptr t,struct _dyneither_ptr v,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){struct Cyc_List_List**
_tmp222;struct Cyc_List_List**f2sp=(_tmp222=_cycalloc(sizeof(*_tmp222)),((_tmp222[
0]=0,_tmp222)));struct Cyc_List_List**_tmpC0=f2sp;int cmp=- 1;for(0;f0s != 0  && f1s
!= 0;f1s=f1s->tl){while(f0s != 0  && (cmp=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)
f0s->hd)->name,((struct Cyc_Absyn_Datatypefield*)f1s->hd)->name))< 0){struct Cyc_List_List*
_tmp223;struct Cyc_List_List*_tmpC1=(_tmp223=_cycalloc(sizeof(*_tmp223)),((
_tmp223->hd=(struct Cyc_Absyn_Datatypefield*)f0s->hd,((_tmp223->tl=0,_tmp223)))));*
_tmpC0=_tmpC1;_tmpC0=&((struct Cyc_List_List*)_check_null(_tmpC1))->tl;f0s=f0s->tl;}
if(f0s == 0  || cmp > 0){*incl=0;{struct Cyc_List_List*_tmp224;struct Cyc_List_List*
_tmpC3=(_tmp224=_cycalloc(sizeof(*_tmp224)),((_tmp224->hd=Cyc_Tcdecl_substitute_datatypefield(
inst,(struct Cyc_Absyn_Datatypefield*)f1s->hd),((_tmp224->tl=0,_tmp224)))));*
_tmpC0=_tmpC3;_tmpC0=&((struct Cyc_List_List*)_check_null(_tmpC3))->tl;}}else{
struct Cyc_Absyn_Datatypefield*_tmpC5=Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)
f0s->hd,(struct Cyc_Absyn_Datatypefield*)f1s->hd,inst,t,v,msg);if(_tmpC5 != 0){if(
_tmpC5 != (struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)f0s->hd))*
incl=0;{struct Cyc_List_List*_tmp225;struct Cyc_List_List*_tmpC6=(_tmp225=
_cycalloc(sizeof(*_tmp225)),((_tmp225->hd=(struct Cyc_Absyn_Datatypefield*)_tmpC5,((
_tmp225->tl=0,_tmp225)))));*_tmpC0=_tmpC6;_tmpC0=&((struct Cyc_List_List*)
_check_null(_tmpC6))->tl;}}else{*res=0;}f0s=f0s->tl;}}if(f1s != 0){*incl=0;*
_tmpC0=f1s;}else{*_tmpC0=f0s;}return*f2sp;}struct _tuple8{struct Cyc_List_List*f1;
struct Cyc_List_List*f2;};static struct _tuple8 Cyc_Tcdecl_split(struct Cyc_List_List*
f);static struct _tuple8 Cyc_Tcdecl_split(struct Cyc_List_List*f){if(f == 0){struct
_tuple8 _tmp226;return(_tmp226.f1=0,((_tmp226.f2=0,_tmp226)));}if(f->tl == 0){
struct _tuple8 _tmp227;return(_tmp227.f1=f,((_tmp227.f2=0,_tmp227)));}{struct Cyc_List_List*
_tmpCC;struct Cyc_List_List*_tmpCD;struct _tuple8 _tmpCB=Cyc_Tcdecl_split(((struct
Cyc_List_List*)_check_null(f->tl))->tl);_tmpCC=_tmpCB.f1;_tmpCD=_tmpCB.f2;{
struct Cyc_List_List*_tmp22C;struct Cyc_List_List*_tmp22B;struct _tuple8 _tmp22A;
return(_tmp22A.f1=((_tmp22C=_cycalloc(sizeof(*_tmp22C)),((_tmp22C->hd=(void*)((
void*)f->hd),((_tmp22C->tl=_tmpCC,_tmp22C)))))),((_tmp22A.f2=((_tmp22B=_cycalloc(
sizeof(*_tmp22B)),((_tmp22B->hd=(void*)((void*)((struct Cyc_List_List*)
_check_null(f->tl))->hd),((_tmp22B->tl=_tmpCD,_tmp22B)))))),_tmp22A)));}}}struct
Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,
struct _dyneither_ptr*v,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*
res,struct _dyneither_ptr*v,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*
msg){struct Cyc_List_List*_tmpD2;struct Cyc_List_List*_tmpD3;struct _tuple8 _tmpD1=((
struct _tuple8(*)(struct Cyc_List_List*f))Cyc_Tcdecl_split)(f);_tmpD2=_tmpD1.f1;
_tmpD3=_tmpD1.f2;if(_tmpD2 != 0  && _tmpD2->tl != 0)_tmpD2=Cyc_Tcdecl_sort_xdatatype_fields(
_tmpD2,res,v,loc,msg);if(_tmpD3 != 0  && _tmpD3->tl != 0)_tmpD3=Cyc_Tcdecl_sort_xdatatype_fields(
_tmpD3,res,v,loc,msg);{const char*_tmp22E;int*_tmp22D;return Cyc_Tcdecl_merge_xdatatype_fields(
_tmpD2,_tmpD3,0,0,0,res,((_tmp22D=_cycalloc_atomic(sizeof(*_tmp22D)),((_tmp22D[0]=
1,_tmp22D)))),((_tmp22E="@extensible datatype",_tag_dyneither(_tmp22E,sizeof(
char),21))),*v,loc,msg);}}struct _tuple9{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;};struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*
d0,struct Cyc_Absyn_Datatypedecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg);struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(
struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg){struct _dyneither_ptr _tmpD6=Cyc_Absynpp_qvar2string(
d0->name);const char*_tmp22F;struct _dyneither_ptr t=(_tmp22F="datatype",
_tag_dyneither(_tmp22F,sizeof(char),9));int _tmpD7=1;if(d0->is_extensible != d1->is_extensible){{
const char*_tmp235;void*_tmp234[3];struct Cyc_String_pa_struct _tmp233;struct Cyc_String_pa_struct
_tmp232;struct Cyc_String_pa_struct _tmp231;(_tmp231.tag=0,((_tmp231.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(d1->is_extensible)),((
_tmp232.tag=0,((_tmp232.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(
d0->is_extensible)),((_tmp233.tag=0,((_tmp233.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmpD6),((_tmp234[0]=& _tmp233,((_tmp234[1]=& _tmp232,((_tmp234[2]=&
_tmp231,Cyc_Tcdecl_merr(loc,msg,((_tmp235="expected %s to be a %s instead of a %s",
_tag_dyneither(_tmp235,sizeof(char),39))),_tag_dyneither(_tmp234,sizeof(void*),3)))))))))))))))))));}
_tmpD7=0;}else{t=Cyc_Tcdecl_is_x2string(d0->is_extensible);}if(d0->is_flat != d1->is_flat){
if(d0->is_flat){const char*_tmp238;void*_tmp237;(_tmp237=0,Cyc_Tcdecl_merr(loc,
msg,((_tmp238="expected __attribute__((flat))",_tag_dyneither(_tmp238,sizeof(
char),31))),_tag_dyneither(_tmp237,sizeof(void*),0)));}else{const char*_tmp23B;
void*_tmp23A;(_tmp23A=0,Cyc_Tcdecl_merr(loc,msg,((_tmp23B="did not expect __attribute__((flat))",
_tag_dyneither(_tmp23B,sizeof(char),37))),_tag_dyneither(_tmp23A,sizeof(void*),0)));}}
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,t,_tmpD6,loc,msg))return 0;{void*_tmpE2;
int _tmpE3;struct _tuple3 _tmpE1=Cyc_Tcdecl_merge_scope((void*)d0->sc,(void*)d1->sc,
t,_tmpD6,loc,msg,0);_tmpE2=_tmpE1.f1;_tmpE3=_tmpE1.f2;if(!_tmpE3)_tmpD7=0;{
struct Cyc_Absyn_Datatypedecl*d2;{struct _tuple9 _tmp23C;struct _tuple9 _tmpE5=(
_tmp23C.f1=d0->fields,((_tmp23C.f2=d1->fields,_tmp23C)));struct Cyc_Core_Opt*
_tmpE6;struct Cyc_Core_Opt*_tmpE7;struct Cyc_Core_Opt*_tmpE8;struct Cyc_Core_Opt
_tmpE9;struct Cyc_List_List*_tmpEA;struct Cyc_Core_Opt*_tmpEB;struct Cyc_Core_Opt
_tmpEC;struct Cyc_List_List*_tmpED;_LL26: _tmpE6=_tmpE5.f2;if(_tmpE6 != 0)goto _LL28;
_LL27: d2=d0;goto _LL25;_LL28: _tmpE7=_tmpE5.f1;if(_tmpE7 != 0)goto _LL2A;_LL29: d2=d1;
goto _LL25;_LL2A: _tmpE8=_tmpE5.f1;if(_tmpE8 == 0)goto _LL25;_tmpE9=*_tmpE8;_tmpEA=(
struct Cyc_List_List*)_tmpE9.v;_tmpEB=_tmpE5.f2;if(_tmpEB == 0)goto _LL25;_tmpEC=*
_tmpEB;_tmpED=(struct Cyc_List_List*)_tmpEC.v;_LL2B: {struct Cyc_List_List*_tmpEE=
Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);if(d0->is_extensible){int _tmpEF=1;
struct Cyc_List_List*_tmpF0=Cyc_Tcdecl_merge_xdatatype_fields(_tmpEA,_tmpED,
_tmpEE,d0->tvs,d1->tvs,& _tmpD7,& _tmpEF,t,_tmpD6,loc,msg);if(_tmpEF)d2=d0;else{{
struct Cyc_Absyn_Datatypedecl*_tmp23D;d2=((_tmp23D=_cycalloc(sizeof(*_tmp23D)),((
_tmp23D[0]=*d0,_tmp23D))));}(void*)(d2->sc=(void*)_tmpE2);{struct Cyc_Core_Opt*
_tmp23E;d2->fields=((_tmp23E=_cycalloc(sizeof(*_tmp23E)),((_tmp23E->v=_tmpF0,
_tmp23E))));}}}else{for(0;_tmpEA != 0  && _tmpED != 0;(_tmpEA=_tmpEA->tl,_tmpED=
_tmpED->tl)){Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)
_tmpEA->hd,(struct Cyc_Absyn_Datatypefield*)_tmpED->hd,_tmpEE,t,_tmpD6,msg);}if(
_tmpEA != 0){{const char*_tmp244;void*_tmp243[3];struct Cyc_String_pa_struct _tmp242;
struct Cyc_String_pa_struct _tmp241;struct Cyc_String_pa_struct _tmp240;(_tmp240.tag=
0,((_tmp240.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)((
struct Cyc_List_List*)_check_null(_tmpED))->hd)->name).f2),((_tmp241.tag=0,((
_tmp241.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD6),((_tmp242.tag=0,((
_tmp242.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp243[0]=& _tmp242,((
_tmp243[1]=& _tmp241,((_tmp243[2]=& _tmp240,Cyc_Tcdecl_merr(loc,msg,((_tmp244="%s %s has extra field %s",
_tag_dyneither(_tmp244,sizeof(char),25))),_tag_dyneither(_tmp243,sizeof(void*),3)))))))))))))))))));}
_tmpD7=0;}if(_tmpED != 0){{const char*_tmp24A;void*_tmp249[3];struct Cyc_String_pa_struct
_tmp248;struct Cyc_String_pa_struct _tmp247;struct Cyc_String_pa_struct _tmp246;(
_tmp246.tag=0,((_tmp246.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((
struct Cyc_Absyn_Datatypefield*)_tmpED->hd)->name).f2),((_tmp247.tag=0,((_tmp247.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD6),((_tmp248.tag=0,((_tmp248.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp249[0]=& _tmp248,((_tmp249[1]=&
_tmp247,((_tmp249[2]=& _tmp246,Cyc_Tcdecl_merr(loc,msg,((_tmp24A="%s %s is missing field %s",
_tag_dyneither(_tmp24A,sizeof(char),26))),_tag_dyneither(_tmp249,sizeof(void*),3)))))))))))))))))));}
_tmpD7=0;}d2=d0;}goto _LL25;}_LL25:;}if(!_tmpD7)return 0;if(_tmpE2 == (void*)d2->sc)
return(struct Cyc_Absyn_Datatypedecl*)d2;else{{struct Cyc_Absyn_Datatypedecl*
_tmp24B;d2=((_tmp24B=_cycalloc(sizeof(*_tmp24B)),((_tmp24B[0]=*d2,_tmp24B))));}(
void*)(d2->sc=(void*)_tmpE2);return(struct Cyc_Absyn_Datatypedecl*)d2;}}}}struct
Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct
Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,
struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*
msg){struct _dyneither_ptr _tmpFF=Cyc_Absynpp_qvar2string(d0->name);int _tmp100=1;
void*_tmp103;int _tmp104;const char*_tmp24C;struct _tuple3 _tmp102=Cyc_Tcdecl_merge_scope((
void*)d0->sc,(void*)d1->sc,((_tmp24C="enum",_tag_dyneither(_tmp24C,sizeof(char),
5))),_tmpFF,loc,msg,1);_tmp103=_tmp102.f1;_tmp104=_tmp102.f2;if(!_tmp104)_tmp100=
0;{struct Cyc_Absyn_Enumdecl*d2;{struct _tuple9 _tmp24D;struct _tuple9 _tmp106=(
_tmp24D.f1=d0->fields,((_tmp24D.f2=d1->fields,_tmp24D)));struct Cyc_Core_Opt*
_tmp107;struct Cyc_Core_Opt*_tmp108;struct Cyc_Core_Opt*_tmp109;struct Cyc_Core_Opt
_tmp10A;struct Cyc_List_List*_tmp10B;struct Cyc_Core_Opt*_tmp10C;struct Cyc_Core_Opt
_tmp10D;struct Cyc_List_List*_tmp10E;_LL2D: _tmp107=_tmp106.f2;if(_tmp107 != 0)goto
_LL2F;_LL2E: d2=d0;goto _LL2C;_LL2F: _tmp108=_tmp106.f1;if(_tmp108 != 0)goto _LL31;
_LL30: d2=d1;goto _LL2C;_LL31: _tmp109=_tmp106.f1;if(_tmp109 == 0)goto _LL2C;_tmp10A=*
_tmp109;_tmp10B=(struct Cyc_List_List*)_tmp10A.v;_tmp10C=_tmp106.f2;if(_tmp10C == 
0)goto _LL2C;_tmp10D=*_tmp10C;_tmp10E=(struct Cyc_List_List*)_tmp10D.v;_LL32: for(0;
_tmp10B != 0  && _tmp10E != 0;(_tmp10B=_tmp10B->tl,_tmp10E=_tmp10E->tl)){struct Cyc_Absyn_Enumfield
_tmp110;struct _tuple0*_tmp111;struct _tuple0 _tmp112;struct _dyneither_ptr*_tmp113;
struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Position_Segment*_tmp115;struct Cyc_Absyn_Enumfield*
_tmp10F=(struct Cyc_Absyn_Enumfield*)_tmp10B->hd;_tmp110=*_tmp10F;_tmp111=_tmp110.name;
_tmp112=*_tmp111;_tmp113=_tmp112.f2;_tmp114=_tmp110.tag;_tmp115=_tmp110.loc;{
struct Cyc_Absyn_Enumfield _tmp117;struct _tuple0*_tmp118;struct _tuple0 _tmp119;
struct _dyneither_ptr*_tmp11A;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Position_Segment*
_tmp11C;struct Cyc_Absyn_Enumfield*_tmp116=(struct Cyc_Absyn_Enumfield*)_tmp10E->hd;
_tmp117=*_tmp116;_tmp118=_tmp117.name;_tmp119=*_tmp118;_tmp11A=_tmp119.f2;
_tmp11B=_tmp117.tag;_tmp11C=_tmp117.loc;if(Cyc_strptrcmp(_tmp113,_tmp11A)!= 0){{
const char*_tmp253;void*_tmp252[3];struct Cyc_String_pa_struct _tmp251;struct Cyc_String_pa_struct
_tmp250;struct Cyc_String_pa_struct _tmp24F;(_tmp24F.tag=0,((_tmp24F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp11A),((_tmp250.tag=0,((_tmp250.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp113),((_tmp251.tag=0,((_tmp251.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFF),((_tmp252[0]=& _tmp251,((
_tmp252[1]=& _tmp250,((_tmp252[2]=& _tmp24F,Cyc_Tcdecl_merr(loc,msg,((_tmp253="enum %s: field name mismatch %s != %s",
_tag_dyneither(_tmp253,sizeof(char),38))),_tag_dyneither(_tmp252,sizeof(void*),3)))))))))))))))))));}
_tmp100=0;}if(Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(
_tmp114))!= Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(
_tmp11B))){{const char*_tmp258;void*_tmp257[2];struct Cyc_String_pa_struct _tmp256;
struct Cyc_String_pa_struct _tmp255;(_tmp255.tag=0,((_tmp255.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp11A),((_tmp256.tag=0,((_tmp256.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFF),((_tmp257[0]=& _tmp256,((
_tmp257[1]=& _tmp255,Cyc_Tcdecl_merr(loc,msg,((_tmp258="enum %s, field %s, value mismatch",
_tag_dyneither(_tmp258,sizeof(char),34))),_tag_dyneither(_tmp257,sizeof(void*),2)))))))))))));}
_tmp100=0;}}}d2=d0;goto _LL2C;_LL2C:;}if(!_tmp100)return 0;if((void*)d2->sc == 
_tmp103)return(struct Cyc_Absyn_Enumdecl*)d2;else{{struct Cyc_Absyn_Enumdecl*
_tmp259;d2=((_tmp259=_cycalloc(sizeof(*_tmp259)),((_tmp259[0]=*d2,_tmp259))));}(
void*)(d2->sc=(void*)_tmp103);return(struct Cyc_Absyn_Enumdecl*)d2;}}}static
struct _tuple3 Cyc_Tcdecl_check_var_or_fn_decl(void*sc0,void*t0,struct Cyc_Absyn_Tqual
tq0,struct Cyc_List_List*atts0,void*sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct
Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);static struct _tuple3 Cyc_Tcdecl_check_var_or_fn_decl(
void*sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*atts0,void*sc1,
void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*atts1,struct _dyneither_ptr t,
struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){
int _tmp127=1;void*_tmp129;int _tmp12A;struct _tuple3 _tmp128=Cyc_Tcdecl_merge_scope(
sc0,sc1,t,v,loc,msg,0);_tmp129=_tmp128.f1;_tmp12A=_tmp128.f2;if(!_tmp12A)_tmp127=
0;if(!Cyc_Tcdecl_check_type(t0,t1)){{const char*_tmp260;void*_tmp25F[4];struct Cyc_String_pa_struct
_tmp25E;struct Cyc_String_pa_struct _tmp25D;struct Cyc_String_pa_struct _tmp25C;
struct Cyc_String_pa_struct _tmp25B;(_tmp25B.tag=0,((_tmp25B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t0)),((_tmp25C.tag=
0,((_tmp25C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp25D.tag=0,((_tmp25D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((
_tmp25E.tag=0,((_tmp25E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((
_tmp25F[0]=& _tmp25E,((_tmp25F[1]=& _tmp25D,((_tmp25F[2]=& _tmp25C,((_tmp25F[3]=&
_tmp25B,Cyc_Tcdecl_merr(loc,msg,((_tmp260="%s %s has type \n%s\n instead of \n%s\n",
_tag_dyneither(_tmp260,sizeof(char),36))),_tag_dyneither(_tmp25F,sizeof(void*),4)))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();_tmp127=0;}if(!Cyc_Tcutil_equal_tqual(tq0,tq1)){{
const char*_tmp265;void*_tmp264[2];struct Cyc_String_pa_struct _tmp263;struct Cyc_String_pa_struct
_tmp262;(_tmp262.tag=0,((_tmp262.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
v),((_tmp263.tag=0,((_tmp263.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((
_tmp264[0]=& _tmp263,((_tmp264[1]=& _tmp262,Cyc_Tcdecl_merr(loc,msg,((_tmp265="%s %s has different type qualifiers",
_tag_dyneither(_tmp265,sizeof(char),36))),_tag_dyneither(_tmp264,sizeof(void*),2)))))))))))));}
_tmp127=0;}if(!Cyc_Tcutil_same_atts(atts0,atts1)){{const char*_tmp26A;void*
_tmp269[2];struct Cyc_String_pa_struct _tmp268;struct Cyc_String_pa_struct _tmp267;(
_tmp267.tag=0,((_tmp267.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((
_tmp268.tag=0,((_tmp268.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((
_tmp269[0]=& _tmp268,((_tmp269[1]=& _tmp267,Cyc_Tcdecl_merr(loc,msg,((_tmp26A="%s %s has different attributes",
_tag_dyneither(_tmp26A,sizeof(char),31))),_tag_dyneither(_tmp269,sizeof(void*),2)))))))))))));}{
const char*_tmp26D;void*_tmp26C;(_tmp26C=0,Cyc_fprintf(Cyc_stderr,((_tmp26D="previous attributes: ",
_tag_dyneither(_tmp26D,sizeof(char),22))),_tag_dyneither(_tmp26C,sizeof(void*),0)));}
for(0;atts0 != 0;atts0=atts0->tl){const char*_tmp271;void*_tmp270[1];struct Cyc_String_pa_struct
_tmp26F;(_tmp26F.tag=0,((_tmp26F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts0->hd)),((_tmp270[0]=& _tmp26F,Cyc_fprintf(
Cyc_stderr,((_tmp271="%s ",_tag_dyneither(_tmp271,sizeof(char),4))),
_tag_dyneither(_tmp270,sizeof(void*),1)))))));}{const char*_tmp274;void*_tmp273;(
_tmp273=0,Cyc_fprintf(Cyc_stderr,((_tmp274="\ncurrent attributes: ",
_tag_dyneither(_tmp274,sizeof(char),22))),_tag_dyneither(_tmp273,sizeof(void*),0)));}
for(0;atts1 != 0;atts1=atts1->tl){const char*_tmp278;void*_tmp277[1];struct Cyc_String_pa_struct
_tmp276;(_tmp276.tag=0,((_tmp276.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts1->hd)),((_tmp277[0]=& _tmp276,Cyc_fprintf(
Cyc_stderr,((_tmp278="%s ",_tag_dyneither(_tmp278,sizeof(char),4))),
_tag_dyneither(_tmp277,sizeof(void*),1)))))));}{const char*_tmp27B;void*_tmp27A;(
_tmp27A=0,Cyc_fprintf(Cyc_stderr,((_tmp27B="\n",_tag_dyneither(_tmp27B,sizeof(
char),2))),_tag_dyneither(_tmp27A,sizeof(void*),0)));}_tmp127=0;}{struct _tuple3
_tmp27C;return(_tmp27C.f1=_tmp129,((_tmp27C.f2=_tmp127,_tmp27C)));}}struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){struct _dyneither_ptr
_tmp146=Cyc_Absynpp_qvar2string(d0->name);void*_tmp149;int _tmp14A;const char*
_tmp27D;struct _tuple3 _tmp148=Cyc_Tcdecl_check_var_or_fn_decl((void*)d0->sc,(void*)
d0->type,d0->tq,d0->attributes,(void*)d1->sc,(void*)d1->type,d1->tq,d1->attributes,((
_tmp27D="variable",_tag_dyneither(_tmp27D,sizeof(char),9))),_tmp146,loc,msg);
_tmp149=_tmp148.f1;_tmp14A=_tmp148.f2;if(!_tmp14A)return 0;if((void*)d0->sc == 
_tmp149)return(struct Cyc_Absyn_Vardecl*)d0;else{struct Cyc_Absyn_Vardecl*_tmp27E;
struct Cyc_Absyn_Vardecl*_tmp14B=(_tmp27E=_cycalloc(sizeof(*_tmp27E)),((_tmp27E[0]=*
d0,_tmp27E)));(void*)(((struct Cyc_Absyn_Vardecl*)_check_null(_tmp14B))->sc=(void*)
_tmp149);return _tmp14B;}}struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg){struct _dyneither_ptr _tmp14D=Cyc_Absynpp_qvar2string(
d0->name);{const char*_tmp27F;if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,((_tmp27F="typedef",
_tag_dyneither(_tmp27F,sizeof(char),8))),_tmp14D,loc,msg))return 0;}{struct Cyc_List_List*
_tmp14F=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);if(d0->defn != 0  && d1->defn != 
0){void*subst_defn1=Cyc_Tcutil_substitute(_tmp14F,(void*)((struct Cyc_Core_Opt*)
_check_null(d1->defn))->v);if(!Cyc_Tcdecl_check_type((void*)((struct Cyc_Core_Opt*)
_check_null(d0->defn))->v,subst_defn1)){{const char*_tmp285;void*_tmp284[3];
struct Cyc_String_pa_struct _tmp283;struct Cyc_String_pa_struct _tmp282;struct Cyc_String_pa_struct
_tmp281;(_tmp281.tag=0,((_tmp281.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(d0->defn))->v)),((
_tmp282.tag=0,((_tmp282.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
subst_defn1)),((_tmp283.tag=0,((_tmp283.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp14D),((_tmp284[0]=& _tmp283,((_tmp284[1]=& _tmp282,((_tmp284[2]=&
_tmp281,Cyc_Tcdecl_merr(loc,msg,((_tmp285="typedef %s does not refer to the same type: %s != %s",
_tag_dyneither(_tmp285,sizeof(char),53))),_tag_dyneither(_tmp284,sizeof(void*),3)))))))))))))))))));}
return 0;}}return(struct Cyc_Absyn_Typedefdecl*)d0;}}void*Cyc_Tcdecl_merge_binding(
void*b0,void*b1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);void*
Cyc_Tcdecl_merge_binding(void*b0,void*b1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){struct _tuple4 _tmp286;struct _tuple4 _tmp156=(_tmp286.f1=b0,((
_tmp286.f2=b1,_tmp286)));void*_tmp157;void*_tmp158;void*_tmp159;struct Cyc_Absyn_Vardecl*
_tmp15A;void*_tmp15B;struct Cyc_Absyn_Vardecl*_tmp15C;void*_tmp15D;struct Cyc_Absyn_Vardecl*
_tmp15E;void*_tmp15F;struct Cyc_Absyn_Fndecl*_tmp160;void*_tmp161;void*_tmp162;
struct Cyc_Absyn_Fndecl*_tmp163;void*_tmp164;struct Cyc_Absyn_Fndecl*_tmp165;void*
_tmp166;struct Cyc_Absyn_Vardecl*_tmp167;_LL34: _tmp157=_tmp156.f1;if((int)_tmp157
!= 0)goto _LL36;_tmp158=_tmp156.f2;if((int)_tmp158 != 0)goto _LL36;_LL35: return(
void*)0;_LL36: _tmp159=_tmp156.f1;if(_tmp159 <= (void*)1)goto _LL38;if(*((int*)
_tmp159)!= 0)goto _LL38;_tmp15A=((struct Cyc_Absyn_Global_b_struct*)_tmp159)->f1;
_tmp15B=_tmp156.f2;if(_tmp15B <= (void*)1)goto _LL38;if(*((int*)_tmp15B)!= 0)goto
_LL38;_tmp15C=((struct Cyc_Absyn_Global_b_struct*)_tmp15B)->f1;_LL37: {struct Cyc_Absyn_Vardecl*
_tmp168=Cyc_Tcdecl_merge_vardecl(_tmp15A,_tmp15C,loc,msg);if(_tmp168 == 0)return(
void*)0;if(_tmp168 == (struct Cyc_Absyn_Vardecl*)_tmp15A)return b0;if(_tmp168 == (
struct Cyc_Absyn_Vardecl*)_tmp15C)return b1;{struct Cyc_Absyn_Global_b_struct
_tmp289;struct Cyc_Absyn_Global_b_struct*_tmp288;return(void*)((_tmp288=_cycalloc(
sizeof(*_tmp288)),((_tmp288[0]=((_tmp289.tag=0,((_tmp289.f1=(struct Cyc_Absyn_Vardecl*)
_tmp168,_tmp289)))),_tmp288))));}}_LL38: _tmp15D=_tmp156.f1;if(_tmp15D <= (void*)1)
goto _LL3A;if(*((int*)_tmp15D)!= 0)goto _LL3A;_tmp15E=((struct Cyc_Absyn_Global_b_struct*)
_tmp15D)->f1;_tmp15F=_tmp156.f2;if(_tmp15F <= (void*)1)goto _LL3A;if(*((int*)
_tmp15F)!= 1)goto _LL3A;_tmp160=((struct Cyc_Absyn_Funname_b_struct*)_tmp15F)->f1;
_LL39: {int _tmp16D;const char*_tmp28A;struct _tuple3 _tmp16C=Cyc_Tcdecl_check_var_or_fn_decl((
void*)_tmp15E->sc,(void*)_tmp15E->type,_tmp15E->tq,_tmp15E->attributes,(void*)
_tmp160->sc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp160->cached_typ))->v,
Cyc_Absyn_empty_tqual(0),_tmp160->attributes,((_tmp28A="function",_tag_dyneither(
_tmp28A,sizeof(char),9))),Cyc_Absynpp_qvar2string(_tmp15E->name),loc,msg);
_tmp16D=_tmp16C.f2;return _tmp16D?b1:(void*)0;}_LL3A: _tmp161=_tmp156.f1;if(
_tmp161 <= (void*)1)goto _LL3C;if(*((int*)_tmp161)!= 1)goto _LL3C;_tmp162=_tmp156.f2;
if(_tmp162 <= (void*)1)goto _LL3C;if(*((int*)_tmp162)!= 1)goto _LL3C;_tmp163=((
struct Cyc_Absyn_Funname_b_struct*)_tmp162)->f1;_LL3B:{const char*_tmp28E;void*
_tmp28D[1];struct Cyc_String_pa_struct _tmp28C;(_tmp28C.tag=0,((_tmp28C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp163->name)),((
_tmp28D[0]=& _tmp28C,Cyc_Tcdecl_merr(loc,msg,((_tmp28E="redefinition of function %s",
_tag_dyneither(_tmp28E,sizeof(char),28))),_tag_dyneither(_tmp28D,sizeof(void*),1)))))));}
return(void*)0;_LL3C: _tmp164=_tmp156.f1;if(_tmp164 <= (void*)1)goto _LL3E;if(*((
int*)_tmp164)!= 1)goto _LL3E;_tmp165=((struct Cyc_Absyn_Funname_b_struct*)_tmp164)->f1;
_tmp166=_tmp156.f2;if(_tmp166 <= (void*)1)goto _LL3E;if(*((int*)_tmp166)!= 0)goto
_LL3E;_tmp167=((struct Cyc_Absyn_Global_b_struct*)_tmp166)->f1;_LL3D: {int _tmp173;
const char*_tmp28F;struct _tuple3 _tmp172=Cyc_Tcdecl_check_var_or_fn_decl((void*)
_tmp165->sc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp165->cached_typ))->v,
Cyc_Absyn_empty_tqual(0),_tmp165->attributes,(void*)_tmp167->sc,(void*)_tmp167->type,
_tmp167->tq,_tmp167->attributes,((_tmp28F="variable",_tag_dyneither(_tmp28F,
sizeof(char),9))),Cyc_Absynpp_qvar2string(_tmp165->name),loc,msg);_tmp173=
_tmp172.f2;return _tmp173?b0:(void*)0;}_LL3E:;_LL3F: {struct Cyc_Core_Invalid_argument_struct
_tmp295;const char*_tmp294;struct Cyc_Core_Invalid_argument_struct*_tmp293;(int)
_throw((void*)((_tmp293=_cycalloc(sizeof(*_tmp293)),((_tmp293[0]=((_tmp295.tag=
Cyc_Core_Invalid_argument,((_tmp295.f1=((_tmp294="Tcdecl::merge_binding",
_tag_dyneither(_tmp294,sizeof(char),22))),_tmp295)))),_tmp293)))));}_LL33:;}
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct
Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct
Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Tcdecl_Xdatatypefielddecl*
Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,
struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){struct Cyc_Tcdecl_Xdatatypefielddecl _tmp178;struct Cyc_Absyn_Datatypedecl*
_tmp179;struct Cyc_Absyn_Datatypefield*_tmp17A;struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp177=d0;_tmp178=*_tmp177;_tmp179=_tmp178.base;_tmp17A=_tmp178.field;{struct
Cyc_Tcdecl_Xdatatypefielddecl _tmp17C;struct Cyc_Absyn_Datatypedecl*_tmp17D;struct
Cyc_Absyn_Datatypefield*_tmp17E;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp17B=d1;
_tmp17C=*_tmp17B;_tmp17D=_tmp17C.base;_tmp17E=_tmp17C.field;{struct
_dyneither_ptr _tmp17F=Cyc_Absynpp_qvar2string(_tmp17A->name);if(Cyc_Absyn_qvar_cmp(
_tmp179->name,_tmp17D->name)!= 0){{const char*_tmp29B;void*_tmp29A[3];struct Cyc_String_pa_struct
_tmp299;struct Cyc_String_pa_struct _tmp298;struct Cyc_String_pa_struct _tmp297;(
_tmp297.tag=0,((_tmp297.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp17F),((
_tmp298.tag=0,((_tmp298.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp17D->name)),((_tmp299.tag=0,((_tmp299.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp179->name)),((_tmp29A[0]=& _tmp299,((
_tmp29A[1]=& _tmp298,((_tmp29A[2]=& _tmp297,Cyc_Tcdecl_merr(loc,msg,((_tmp29B="@extensible datatype %s and %s have both a field named %s",
_tag_dyneither(_tmp29B,sizeof(char),58))),_tag_dyneither(_tmp29A,sizeof(void*),3)))))))))))))))))));}
return 0;}{const char*_tmp29C;if(!Cyc_Tcdecl_check_tvs(_tmp179->tvs,_tmp17D->tvs,((
_tmp29C="@extensible datatype",_tag_dyneither(_tmp29C,sizeof(char),21))),Cyc_Absynpp_qvar2string(
_tmp179->name),loc,msg))return 0;}{struct Cyc_List_List*_tmp186=Cyc_Tcdecl_build_tvs_map(
_tmp179->tvs,_tmp17D->tvs);const char*_tmp29D;struct Cyc_Absyn_Datatypefield*
_tmp187=Cyc_Tcdecl_merge_datatypefield(_tmp17A,_tmp17E,_tmp186,((_tmp29D="@extensible datatype field",
_tag_dyneither(_tmp29D,sizeof(char),27))),_tmp17F,msg);if(_tmp187 == 0)return 0;
if(_tmp187 == (struct Cyc_Absyn_Datatypefield*)_tmp17A)return(struct Cyc_Tcdecl_Xdatatypefielddecl*)
d0;{struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp29E;return(_tmp29E=_cycalloc(sizeof(*
_tmp29E)),((_tmp29E->base=_tmp179,((_tmp29E->field=(struct Cyc_Absyn_Datatypefield*)
_tmp187,_tmp29E)))));}}}}}
