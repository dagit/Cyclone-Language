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
struct _dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct
Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;int(*next)(void*env,
void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct
Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict
Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));int Cyc_Dict_is_empty(
struct Cyc_Dict_Dict d);int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct
_RegionHandle*,struct Cyc_Dict_Dict,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*
Cyc_Set_empty(int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rempty(struct
_RegionHandle*r,int(*cmp)(void*,void*));int Cyc_Set_member(struct Cyc_Set_Set*s,
void*elt);extern char Cyc_Set_Absent[11];struct Cyc_SlowDict_Dict;extern char Cyc_SlowDict_Present[
12];extern char Cyc_SlowDict_Absent[11];struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dyneither_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};
extern char Cyc_Position_Nocontext[14];int Cyc_strptrcmp(struct _dyneither_ptr*s1,
struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;
extern void*Cyc_Absyn_empty_effect;extern struct _tuple1*Cyc_Absyn_exn_name;extern
struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;void*Cyc_Absyn_dyneither_typ(void*t,
void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(void*);
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
f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;
struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;
struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict
tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcenv_env_err(
struct _dyneither_ptr msg);struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(struct _RegionHandle*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);struct
Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Fenv*
old_fenv,struct Cyc_Absyn_Fndecl*new_fn);struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _dyneither_ptr*,struct
Cyc_List_List*);void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Tuniondecl***Cyc_Tcenv_lookup_xtuniondecl(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_return_typ(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct
Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_goto(struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);
struct _tuple4{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_Tcenv_CList*
f3;};struct _tuple4*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Switch_clause***);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*
loc);struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple5**
oldtv,struct Cyc_Position_Segment*loc);void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,void*r,int resetable,int opened);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_rgn_resetable(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);void*Cyc_Tcutil_kind_to_bound(
void*k);struct _tuple5 Cyc_Tcutil_swap_kind(void*t,void*kb);int Cyc_Tcutil_subset_effect(
int may_constrain_evars,void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int
constrain,void*r,void*e);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,
struct Cyc_List_List*);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);char Cyc_Tcenv_Env_error[14]="\000\000\000\000Env_error\000";
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg);void*Cyc_Tcenv_env_err(struct
_dyneither_ptr msg){{const char*_tmp2F2;void*_tmp2F1[1];struct Cyc_String_pa_struct
_tmp2F0;(_tmp2F0.tag=0,((_tmp2F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
msg),((_tmp2F1[0]=& _tmp2F0,Cyc_fprintf(Cyc_stderr,((_tmp2F2="Internal error in tcenv: %s\n",
_tag_dyneither(_tmp2F2,sizeof(char),29))),_tag_dyneither(_tmp2F1,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)Cyc_Tcenv_Env_error);}
struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;struct Cyc_Tcenv_CtrlEnv{struct
_RegionHandle*ctrl_rgn;void*continue_stmt;void*break_stmt;struct _tuple4*
fallthru_clause;void*next_stmt;int try_depth;};struct Cyc_Tcenv_SharedFenv{struct
_RegionHandle*frgn;void*return_typ;struct Cyc_Dict_Dict seen_labels;struct Cyc_Dict_Dict
needed_labels;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*
delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*
b;struct Cyc_Tcenv_Bindings*tl;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*
shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;
struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*
ctrl_env;void*capability;void*curr_rgn;int in_notreadctxt;struct _RegionHandle*
fnrgn;};char Cyc_Tcenv_NoBinding[14]="\000\000\000\000NoBinding\000";void*Cyc_Tcenv_lookup_binding(
struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v);void*Cyc_Tcenv_lookup_binding(
struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){for(0;(unsigned int)bs;bs=bs->tl){
if(Cyc_strptrcmp(v,bs->v)== 0)return(void*)bs->b;}(int)_throw((void*)Cyc_Tcenv_NoBinding);}
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*r);struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(struct _RegionHandle*r){struct Cyc_Tcenv_Genv*_tmp2F3;return(
_tmp2F3=_region_malloc(r,sizeof(*_tmp2F3)),((_tmp2F3->grgn=r,((_tmp2F3->namespaces=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp),((_tmp2F3->aggrdecls=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp2F3->tuniondecls=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp2F3->enumdecls=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp2F3->typedefs=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp2F3->ordinaries=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp2F3->availables=
0,_tmp2F3)))))))))))))))));}struct _tuple6{void*f1;int f2;};struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(struct _RegionHandle*r);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(
struct _RegionHandle*r){{struct Cyc_Core_Opt*_tmp2F4;Cyc_Tcutil_empty_var_set=((
_tmp2F4=_cycalloc(sizeof(*_tmp2F4)),((_tmp2F4->v=((struct Cyc_Set_Set*(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),
_tmp2F4))));}{struct Cyc_Tcenv_Genv*_tmp5=Cyc_Tcenv_empty_genv(r);{struct Cyc_Absyn_Tuniondecl**
_tmp2F5;_tmp5->tuniondecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Tuniondecl**v))Cyc_Dict_insert)(_tmp5->tuniondecls,(*
Cyc_Absyn_exn_name).f2,((_tmp2F5=_cycalloc(sizeof(*_tmp2F5)),((_tmp2F5[0]=Cyc_Absyn_exn_tud,
_tmp2F5)))));}{struct Cyc_List_List*_tmp7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp7 != 0;_tmp7=_tmp7->tl){
struct Cyc_Tcenv_TunionRes_struct*_tmp2FB;struct Cyc_Tcenv_TunionRes_struct _tmp2FA;
struct _tuple6*_tmp2F9;_tmp5->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k,struct _tuple6*v))Cyc_Dict_insert)(_tmp5->ordinaries,(*((
struct Cyc_Absyn_Tunionfield*)_tmp7->hd)->name).f2,((_tmp2F9=_region_malloc(r,
sizeof(*_tmp2F9)),((_tmp2F9->f1=(void*)((_tmp2FB=_cycalloc(sizeof(*_tmp2FB)),((
_tmp2FB[0]=((_tmp2FA.tag=2,((_tmp2FA.f1=Cyc_Absyn_exn_tud,((_tmp2FA.f2=(struct
Cyc_Absyn_Tunionfield*)_tmp7->hd,_tmp2FA)))))),_tmp2FB)))),((_tmp2F9->f2=1,
_tmp2F9)))))));}}{struct Cyc_Dict_Dict ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_varlist_cmp),0,_tmp5);struct Cyc_Tcenv_Tenv*_tmp2FC;return(_tmp2FC=
_region_malloc(r,sizeof(*_tmp2FC)),((_tmp2FC->ns=0,((_tmp2FC->ae=ae,((_tmp2FC->le=
0,((_tmp2FC->allow_valueof=0,_tmp2FC)))))))));}}}static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*n,
struct Cyc_List_List*ns);static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*n,
struct Cyc_List_List*ns){return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,
loc,n,ns));}static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*
ns);static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0){const char*_tmp2FD;return((struct Cyc_List_List*(*)(struct
_dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp2FD="outer_namespace",_tag_dyneither(
_tmp2FD,sizeof(char),16))));}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}static int Cyc_Tcenv_same_namespace(
struct Cyc_List_List*n1,struct Cyc_List_List*n2);static int Cyc_Tcenv_same_namespace(
struct Cyc_List_List*n1,struct Cyc_List_List*n2){if(n1 == n2)return 1;for(0;n1 != 0;
n1=n1->tl){if(n2 == 0)return 0;if(Cyc_strptrcmp((struct _dyneither_ptr*)n1->hd,(
struct _dyneither_ptr*)n2->hd)!= 0)return 0;n2=n2->tl;}if(n2 != 0)return 0;return 1;}
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct
_dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss);static void Cyc_Tcenv_check_repeat(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dyneither_ptr*),struct _dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*
nss){for(0;nss != 0;nss=nss->tl){if(!Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)
nss->hd)){struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)nss->hd);
struct _handler_cons _tmpD;_push_handler(& _tmpD);{int _tmpF=0;if(setjmp(_tmpD.handler))
_tmpF=1;if(!_tmpF){lookup(ge2,v);{const char*_tmp301;void*_tmp300[1];struct Cyc_String_pa_struct
_tmp2FF;(_tmp2FF.tag=0,((_tmp2FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
v),((_tmp300[0]=& _tmp2FF,Cyc_Tcutil_terr(loc,((_tmp301="%s is ambiguous",
_tag_dyneither(_tmp301,sizeof(char),16))),_tag_dyneither(_tmp300,sizeof(void*),1)))))));};
_pop_handler();}else{void*_tmpE=(void*)_exn_thrown;void*_tmp14=_tmpE;_LL1: if(
_tmp14 != Cyc_Dict_Absent)goto _LL3;_LL2: goto _LL0;_LL3:;_LL4:(void)_throw(_tmp14);
_LL0:;}}}}return;}static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v);static void*Cyc_Tcenv_scoped_lookup(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dyneither_ptr*),struct _dyneither_ptr*v){struct Cyc_List_List*cns=te->ns;
while(1){struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);{struct _handler_cons _tmp15;
_push_handler(& _tmp15);{int _tmp17=0;if(setjmp(_tmp15.handler))_tmp17=1;if(!
_tmp17){{void*result=lookup(ge,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp18=result;_npop_handler(0);return _tmp18;}};_pop_handler();}else{void*
_tmp16=(void*)_exn_thrown;void*_tmp1A=_tmp16;_LL6: if(_tmp1A != Cyc_Dict_Absent)
goto _LL8;_LL7: goto _LL5;_LL8:;_LL9:(void)_throw(_tmp1A);_LL5:;}}}{struct Cyc_List_List*
_tmp1B=ge->availables;for(0;_tmp1B != 0;_tmp1B=_tmp1B->tl){struct Cyc_Tcenv_Genv*
ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,(struct Cyc_List_List*)_tmp1B->hd);struct _handler_cons _tmp1C;_push_handler(&
_tmp1C);{int _tmp1E=0;if(setjmp(_tmp1C.handler))_tmp1E=1;if(!_tmp1E){{void*result=
lookup(ge2,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)
_tmp1B->hd,_tmp1B->tl);{void*_tmp1F=result;_npop_handler(0);return _tmp1F;}};
_pop_handler();}else{void*_tmp1D=(void*)_exn_thrown;void*_tmp21=_tmp1D;_LLB: if(
_tmp21 != Cyc_Dict_Absent)goto _LLD;_LLC: goto _LLA;_LLD:;_LLE:(void)_throw(_tmp21);
_LLA:;}}}}if(cns == 0)(int)_throw((void*)Cyc_Dict_Absent);cns=Cyc_Tcenv_outer_namespace(
cns);}}static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,
struct _dyneither_ptr*v);static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*
ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp22=ge->ordinaries;struct
_tuple6*ans=((struct _tuple6*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp22,v);(*ans).f2=1;return(*ans).f1;}static void*Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q);static
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q){struct _tuple1 _tmp24;union Cyc_Absyn_Nmspace_union _tmp25;
struct _dyneither_ptr*_tmp26;struct _tuple1*_tmp23=q;_tmp24=*_tmp23;_tmp25=_tmp24.f1;
_tmp26=_tmp24.f2;{union Cyc_Absyn_Nmspace_union _tmp27=_tmp25;struct Cyc_List_List*
_tmp28;struct Cyc_List_List*_tmp29;struct Cyc_List_List _tmp2A;struct _dyneither_ptr*
_tmp2B;struct Cyc_List_List*_tmp2C;struct Cyc_List_List*_tmp2D;_LL10: if((_tmp27.Loc_n).tag
!= 0)goto _LL12;_LL11: goto _LL13;_LL12: if((_tmp27.Rel_n).tag != 1)goto _LL14;_tmp28=(
_tmp27.Rel_n).f1;if(_tmp28 != 0)goto _LL14;_LL13: return Cyc_Tcenv_scoped_lookup(te,
loc,Cyc_Tcenv_lookup_ordinary_global_f,_tmp26);_LL14: if((_tmp27.Rel_n).tag != 1)
goto _LL16;_tmp29=(_tmp27.Rel_n).f1;if(_tmp29 == 0)goto _LL16;_tmp2A=*_tmp29;_tmp2B=(
struct _dyneither_ptr*)_tmp2A.hd;_tmp2C=_tmp2A.tl;_LL15: {struct Cyc_Tcenv_Genv*
_tmp2E=Cyc_Tcenv_lookup_namespace(te,loc,_tmp2B,_tmp2C);return Cyc_Tcenv_lookup_ordinary_global_f(
_tmp2E,_tmp26);}_LL16: if((_tmp27.Abs_n).tag != 2)goto _LLF;_tmp2D=(_tmp27.Abs_n).f1;
_LL17: return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp2D),_tmp26);
_LLF:;}}struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns);
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){struct
Cyc_List_List*_tmp2F=te->ns;struct _RegionHandle*_tmp30=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp31=0;while(1){struct Cyc_Tcenv_Genv*_tmp32=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp2F);
struct Cyc_List_List*_tmp33=_tmp32->availables;struct Cyc_Set_Set*_tmp34=_tmp32->namespaces;{
struct Cyc_List_List*_tmp35=_tmp33;for(0;_tmp35 != 0;_tmp35=_tmp35->tl){struct Cyc_Set_Set*
_tmp36=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp35->hd))->namespaces;if(((int(*)(
struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp36,n)){struct
Cyc_List_List*_tmp304;struct Cyc_List_List*_tmp303;_tmp31=((_tmp303=
_region_malloc(_tmp30,sizeof(*_tmp303)),((_tmp303->hd=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2F,((_tmp304=
_cycalloc(sizeof(*_tmp304)),((_tmp304->hd=n,((_tmp304->tl=ns,_tmp304))))))),((
_tmp303->tl=_tmp31,_tmp303))))));}}}if(((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(_tmp34,n)){struct Cyc_List_List*_tmp307;
struct Cyc_List_List*_tmp306;_tmp31=((_tmp306=_region_malloc(_tmp30,sizeof(*
_tmp306)),((_tmp306->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp2F,((_tmp307=_cycalloc(sizeof(*_tmp307)),((
_tmp307->hd=n,((_tmp307->tl=ns,_tmp307))))))),((_tmp306->tl=_tmp31,_tmp306))))));}
if(_tmp31 != 0){if(_tmp31->tl != 0){const char*_tmp30B;void*_tmp30A[1];struct Cyc_String_pa_struct
_tmp309;(_tmp309.tag=0,((_tmp309.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
n),((_tmp30A[0]=& _tmp309,Cyc_Tcutil_terr(loc,((_tmp30B="%s is ambiguous",
_tag_dyneither(_tmp30B,sizeof(char),16))),_tag_dyneither(_tmp30A,sizeof(void*),1)))))));}
return(struct Cyc_List_List*)_tmp31->hd;}if(_tmp2F == 0)(int)_throw((void*)Cyc_Dict_Absent);
_tmp2F=Cyc_Tcenv_outer_namespace(_tmp2F);}}static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v);static struct Cyc_Absyn_Aggrdecl**
Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){
struct Cyc_Dict_Dict _tmp3E=ge->aggrdecls;return((struct Cyc_Absyn_Aggrdecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp3E,v);}struct
Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1
_tmp40;union Cyc_Absyn_Nmspace_union _tmp41;struct _dyneither_ptr*_tmp42;struct
_tuple1*_tmp3F=q;_tmp40=*_tmp3F;_tmp41=_tmp40.f1;_tmp42=_tmp40.f2;{union Cyc_Absyn_Nmspace_union
_tmp43=_tmp41;struct Cyc_List_List*_tmp44;struct Cyc_List_List*_tmp45;struct Cyc_List_List*
_tmp46;struct Cyc_List_List _tmp47;struct _dyneither_ptr*_tmp48;struct Cyc_List_List*
_tmp49;_LL19: if((_tmp43.Loc_n).tag != 0)goto _LL1B;_LL1A: goto _LL1C;_LL1B: if((
_tmp43.Rel_n).tag != 1)goto _LL1D;_tmp44=(_tmp43.Rel_n).f1;if(_tmp44 != 0)goto _LL1D;
_LL1C: return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_aggrdecl_f,
_tmp42);_LL1D: if((_tmp43.Abs_n).tag != 2)goto _LL1F;_tmp45=(_tmp43.Abs_n).f1;_LL1E: {
struct Cyc_Dict_Dict _tmp4A=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp45))->aggrdecls;return((
struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp4A,_tmp42);}_LL1F: if((_tmp43.Rel_n).tag != 1)goto _LL18;_tmp46=(_tmp43.Rel_n).f1;
if(_tmp46 == 0)goto _LL18;_tmp47=*_tmp46;_tmp48=(struct _dyneither_ptr*)_tmp47.hd;
_tmp49=_tmp47.tl;_LL20: {struct Cyc_Dict_Dict _tmp4B=(Cyc_Tcenv_lookup_namespace(
te,loc,_tmp48,_tmp49))->aggrdecls;return((struct Cyc_Absyn_Aggrdecl**(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4B,_tmp42);}_LL18:;}}
static struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl_f(struct Cyc_Tcenv_Genv*
ge,struct _dyneither_ptr*v);static struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp4C=ge->tuniondecls;
return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4C,v);}struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q);struct
Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1 _tmp4E;union Cyc_Absyn_Nmspace_union
_tmp4F;struct _dyneither_ptr*_tmp50;struct _tuple1*_tmp4D=q;_tmp4E=*_tmp4D;_tmp4F=
_tmp4E.f1;_tmp50=_tmp4E.f2;{union Cyc_Absyn_Nmspace_union _tmp51=_tmp4F;struct Cyc_List_List*
_tmp52;struct Cyc_List_List*_tmp53;struct Cyc_List_List _tmp54;struct _dyneither_ptr*
_tmp55;struct Cyc_List_List*_tmp56;struct Cyc_List_List*_tmp57;_LL22: if((_tmp51.Loc_n).tag
!= 0)goto _LL24;_LL23: goto _LL25;_LL24: if((_tmp51.Rel_n).tag != 1)goto _LL26;_tmp52=(
_tmp51.Rel_n).f1;if(_tmp52 != 0)goto _LL26;_LL25: return((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Tuniondecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_tuniondecl_f,_tmp50);_LL26: if((_tmp51.Rel_n).tag != 1)
goto _LL28;_tmp53=(_tmp51.Rel_n).f1;if(_tmp53 == 0)goto _LL28;_tmp54=*_tmp53;_tmp55=(
struct _dyneither_ptr*)_tmp54.hd;_tmp56=_tmp54.tl;_LL27: {struct Cyc_Dict_Dict
_tmp58=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp55,_tmp56))->tuniondecls;return((
struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp58,_tmp50);}_LL28: if((_tmp51.Abs_n).tag != 2)goto _LL21;_tmp57=(_tmp51.Abs_n).f1;
_LL29: {struct Cyc_Dict_Dict _tmp59=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp57))->tuniondecls;return((
struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp59,_tmp50);}_LL21:;}}static struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_xtuniondecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v);static struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_xtuniondecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->tuniondecls,v);}struct Cyc_Absyn_Tuniondecl***
Cyc_Tcenv_lookup_xtuniondecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q);struct Cyc_Absyn_Tuniondecl***Cyc_Tcenv_lookup_xtuniondecl(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct _tuple1*q){struct _tuple1 _tmp5B;union Cyc_Absyn_Nmspace_union _tmp5C;struct
_dyneither_ptr*_tmp5D;struct _tuple1*_tmp5A=q;_tmp5B=*_tmp5A;_tmp5C=_tmp5B.f1;
_tmp5D=_tmp5B.f2;{union Cyc_Absyn_Nmspace_union _tmp5E=_tmp5C;struct Cyc_List_List*
_tmp5F;struct Cyc_List_List*_tmp60;struct Cyc_List_List _tmp61;struct _dyneither_ptr*
_tmp62;struct Cyc_List_List*_tmp63;struct Cyc_List_List*_tmp64;_LL2B: if((_tmp5E.Rel_n).tag
!= 1)goto _LL2D;_tmp5F=(_tmp5E.Rel_n).f1;if(_tmp5F != 0)goto _LL2D;_LL2C: {struct
_handler_cons _tmp65;_push_handler(& _tmp65);{int _tmp67=0;if(setjmp(_tmp65.handler))
_tmp67=1;if(!_tmp67){{struct Cyc_Absyn_Tuniondecl***_tmp30C;struct Cyc_Absyn_Tuniondecl***
_tmp69=(_tmp30C=_region_malloc(r,sizeof(*_tmp30C)),((_tmp30C[0]=((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Tuniondecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_xtuniondecl_f,_tmp5D),_tmp30C)));_npop_handler(0);return
_tmp69;};_pop_handler();}else{void*_tmp66=(void*)_exn_thrown;void*_tmp6B=_tmp66;
_LL34: if(_tmp6B != Cyc_Dict_Absent)goto _LL36;_LL35: return 0;_LL36:;_LL37:(void)
_throw(_tmp6B);_LL33:;}}}_LL2D: if((_tmp5E.Loc_n).tag != 0)goto _LL2F;_LL2E:{const
char*_tmp30F;void*_tmp30E;(_tmp30E=0,Cyc_Tcutil_terr(loc,((_tmp30F="lookup_xtuniondecl: impossible",
_tag_dyneither(_tmp30F,sizeof(char),31))),_tag_dyneither(_tmp30E,sizeof(void*),0)));}
return 0;_LL2F: if((_tmp5E.Rel_n).tag != 1)goto _LL31;_tmp60=(_tmp5E.Rel_n).f1;if(
_tmp60 == 0)goto _LL31;_tmp61=*_tmp60;_tmp62=(struct _dyneither_ptr*)_tmp61.hd;
_tmp63=_tmp61.tl;_LL30: {struct Cyc_Tcenv_Genv*ge;{struct _handler_cons _tmp6E;
_push_handler(& _tmp6E);{int _tmp70=0;if(setjmp(_tmp6E.handler))_tmp70=1;if(!
_tmp70){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp62,_tmp63);;_pop_handler();}
else{void*_tmp6F=(void*)_exn_thrown;void*_tmp72=_tmp6F;_LL39: if(_tmp72 != Cyc_Dict_Absent)
goto _LL3B;_LL3A:{const char*_tmp312;void*_tmp311;(_tmp311=0,Cyc_Tcutil_terr(loc,((
_tmp312="bad qualified name for xtunion",_tag_dyneither(_tmp312,sizeof(char),31))),
_tag_dyneither(_tmp311,sizeof(void*),0)));}(int)_throw((void*)Cyc_Dict_Absent);
_LL3B:;_LL3C:(void)_throw(_tmp72);_LL38:;}}}{struct Cyc_Dict_Dict _tmp75=ge->tuniondecls;
struct Cyc_Absyn_Tuniondecl***_tmp313;return(_tmp313=_region_malloc(r,sizeof(*
_tmp313)),((_tmp313[0]=((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp75,_tmp5D),_tmp313)));}}_LL31: if((
_tmp5E.Abs_n).tag != 2)goto _LL2A;_tmp64=(_tmp5E.Abs_n).f1;_LL32: {struct Cyc_Dict_Dict
_tmp77=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,_tmp64))->tuniondecls;struct Cyc_Absyn_Tuniondecl***
_tmp314;return(_tmp314=_region_malloc(r,sizeof(*_tmp314)),((_tmp314[0]=((struct
Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp77,_tmp5D),_tmp314)));}_LL2A:;}}static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v);static struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){
struct Cyc_Dict_Dict _tmp79=ge->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp79,v);}struct
Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1
_tmp7B;union Cyc_Absyn_Nmspace_union _tmp7C;struct _dyneither_ptr*_tmp7D;struct
_tuple1*_tmp7A=q;_tmp7B=*_tmp7A;_tmp7C=_tmp7B.f1;_tmp7D=_tmp7B.f2;{union Cyc_Absyn_Nmspace_union
_tmp7E=_tmp7C;struct Cyc_List_List*_tmp7F;struct Cyc_List_List*_tmp80;struct Cyc_List_List
_tmp81;struct _dyneither_ptr*_tmp82;struct Cyc_List_List*_tmp83;struct Cyc_List_List*
_tmp84;_LL3E: if((_tmp7E.Loc_n).tag != 0)goto _LL40;_LL3F: goto _LL41;_LL40: if((
_tmp7E.Rel_n).tag != 1)goto _LL42;_tmp7F=(_tmp7E.Rel_n).f1;if(_tmp7F != 0)goto _LL42;
_LL41: return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,
_tmp7D);_LL42: if((_tmp7E.Rel_n).tag != 1)goto _LL44;_tmp80=(_tmp7E.Rel_n).f1;if(
_tmp80 == 0)goto _LL44;_tmp81=*_tmp80;_tmp82=(struct _dyneither_ptr*)_tmp81.hd;
_tmp83=_tmp81.tl;_LL43: {struct Cyc_Dict_Dict _tmp85=(Cyc_Tcenv_lookup_namespace(
te,loc,_tmp82,_tmp83))->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp85,_tmp7D);}_LL44:
if((_tmp7E.Abs_n).tag != 2)goto _LL3D;_tmp84=(_tmp7E.Abs_n).f1;_LL45: {struct Cyc_Dict_Dict
_tmp86=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,_tmp84))->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp86,_tmp7D);}
_LL3D:;}}static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct
Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v);static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp87=ge->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(_tmp87,v);}struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q);struct
Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1 _tmp89;union Cyc_Absyn_Nmspace_union
_tmp8A;struct _dyneither_ptr*_tmp8B;struct _tuple1*_tmp88=q;_tmp89=*_tmp88;_tmp8A=
_tmp89.f1;_tmp8B=_tmp89.f2;{union Cyc_Absyn_Nmspace_union _tmp8C=_tmp8A;struct Cyc_List_List*
_tmp8D;struct Cyc_List_List*_tmp8E;struct Cyc_List_List _tmp8F;struct _dyneither_ptr*
_tmp90;struct Cyc_List_List*_tmp91;struct Cyc_List_List*_tmp92;_LL47: if((_tmp8C.Loc_n).tag
!= 0)goto _LL49;_LL48: goto _LL4A;_LL49: if((_tmp8C.Rel_n).tag != 1)goto _LL4B;_tmp8D=(
_tmp8C.Rel_n).f1;if(_tmp8D != 0)goto _LL4B;_LL4A: return((struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*(*
lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp8B);_LL4B: if((_tmp8C.Rel_n).tag != 1)
goto _LL4D;_tmp8E=(_tmp8C.Rel_n).f1;if(_tmp8E == 0)goto _LL4D;_tmp8F=*_tmp8E;_tmp90=(
struct _dyneither_ptr*)_tmp8F.hd;_tmp91=_tmp8F.tl;_LL4C: {struct Cyc_Dict_Dict
_tmp93=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp90,_tmp91))->typedefs;return((
struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp93,_tmp8B);}_LL4D: if((_tmp8C.Abs_n).tag != 2)goto _LL46;_tmp92=(_tmp8C.Abs_n).f1;
_LL4E: {struct Cyc_Dict_Dict _tmp94=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp92))->typedefs;return((
struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp94,_tmp8B);}_LL46:;}}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*
te,struct _dyneither_ptr err_msg);static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(
struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){struct Cyc_Tcenv_Fenv*_tmp95=
te->le;if((struct Cyc_Tcenv_Tenv*)te == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(
err_msg);return(struct Cyc_Tcenv_Fenv*)_check_null(_tmp95);}struct _RegionHandle*
Cyc_Tcenv_coerce_heap_region();struct _RegionHandle*Cyc_Tcenv_coerce_heap_region(){
return(struct _RegionHandle*)Cyc_Core_heap_region;}struct _RegionHandle*Cyc_Tcenv_get_fnrgn(
struct Cyc_Tcenv_Tenv*te);struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv*_tmp96=te->le;if(_tmp96 != 0){struct Cyc_Tcenv_Fenv _tmp98;
struct _RegionHandle*_tmp99;struct Cyc_Tcenv_Fenv*_tmp97=(struct Cyc_Tcenv_Fenv*)
_tmp96;_tmp98=*_tmp97;_tmp99=_tmp98.fnrgn;return _tmp99;}return Cyc_Tcenv_coerce_heap_region();}
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Fenv*fe);static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct
_RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){if(te->le == 0){
const char*_tmp315;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp315="put_fenv",
_tag_dyneither(_tmp315,sizeof(char),9))));}{struct Cyc_Tcenv_Tenv*_tmp316;return(
_tmp316=_region_malloc(l,sizeof(*_tmp316)),((_tmp316->ns=te->ns,((_tmp316->ae=te->ae,((
_tmp316->le=(struct Cyc_Tcenv_Fenv*)fe,((_tmp316->allow_valueof=te->allow_valueof,
_tmp316)))))))));}}static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct
_RegionHandle*l,struct Cyc_Tcenv_Tenv*te);static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(
struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_tmp317;
return(_tmp317=_region_malloc(l,sizeof(*_tmp317)),((_tmp317->ns=te->ns,((_tmp317->ae=
te->ae,((_tmp317->le=0,((_tmp317->allow_valueof=te->allow_valueof,_tmp317)))))))));}
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,
struct Cyc_Tcenv_Fenv*f);static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmp9E;
struct Cyc_List_List*_tmp9F;struct Cyc_RgnOrder_RgnPO*_tmpA0;struct Cyc_Tcenv_Bindings*
_tmpA1;struct Cyc_Absyn_Stmt*_tmpA2;struct Cyc_Tcenv_CtrlEnv*_tmpA3;void*_tmpA4;
void*_tmpA5;int _tmpA6;struct _RegionHandle*_tmpA7;struct Cyc_Tcenv_Fenv _tmp9D=*f;
_tmp9E=_tmp9D.shared;_tmp9F=_tmp9D.type_vars;_tmpA0=_tmp9D.region_order;_tmpA1=
_tmp9D.locals;_tmpA2=_tmp9D.encloser;_tmpA3=_tmp9D.ctrl_env;_tmpA4=(void*)_tmp9D.capability;
_tmpA5=(void*)_tmp9D.curr_rgn;_tmpA6=_tmp9D.in_notreadctxt;_tmpA7=_tmp9D.fnrgn;{
struct Cyc_Tcenv_Fenv*_tmp318;return(_tmp318=_region_malloc(l,sizeof(*_tmp318)),((
_tmp318->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp9E,((_tmp318->type_vars=(struct
Cyc_List_List*)_tmp9F,((_tmp318->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA0,((
_tmp318->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmpA1),((
_tmp318->encloser=(struct Cyc_Absyn_Stmt*)_tmpA2,((_tmp318->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmpA3),((_tmp318->capability=(void*)((void*)_tmpA4),((
_tmp318->curr_rgn=(void*)((void*)_tmpA5),((_tmp318->in_notreadctxt=(int)_tmpA6,((
_tmp318->fnrgn=(struct _RegionHandle*)l,_tmp318)))))))))))))))))))));}}static
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct
Cyc_Tcenv_Fenv*f);static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct
_RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmpAA;struct
Cyc_List_List*_tmpAB;struct Cyc_RgnOrder_RgnPO*_tmpAC;struct Cyc_Tcenv_Bindings*
_tmpAD;struct Cyc_Absyn_Stmt*_tmpAE;struct Cyc_Tcenv_CtrlEnv*_tmpAF;void*_tmpB0;
void*_tmpB1;int _tmpB2;struct Cyc_Tcenv_Fenv _tmpA9=*f;_tmpAA=_tmpA9.shared;_tmpAB=
_tmpA9.type_vars;_tmpAC=_tmpA9.region_order;_tmpAD=_tmpA9.locals;_tmpAE=_tmpA9.encloser;
_tmpAF=_tmpA9.ctrl_env;_tmpB0=(void*)_tmpA9.capability;_tmpB1=(void*)_tmpA9.curr_rgn;
_tmpB2=_tmpA9.in_notreadctxt;{struct _RegionHandle*_tmpB4;void*_tmpB5;void*_tmpB6;
struct _tuple4*_tmpB7;void*_tmpB8;int _tmpB9;struct Cyc_Tcenv_CtrlEnv _tmpB3=*_tmpAF;
_tmpB4=_tmpB3.ctrl_rgn;_tmpB5=(void*)_tmpB3.continue_stmt;_tmpB6=(void*)_tmpB3.break_stmt;
_tmpB7=_tmpB3.fallthru_clause;_tmpB8=(void*)_tmpB3.next_stmt;_tmpB9=_tmpB3.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmp319;struct Cyc_Tcenv_CtrlEnv*_tmpBA=(_tmp319=
_region_malloc(l,sizeof(*_tmp319)),((_tmp319->ctrl_rgn=_tmpB4,((_tmp319->continue_stmt=(
void*)_tmpB5,((_tmp319->break_stmt=(void*)_tmpB6,((_tmp319->fallthru_clause=
_tmpB7,((_tmp319->next_stmt=(void*)_tmpB8,((_tmp319->try_depth=_tmpB9,_tmp319)))))))))))));
struct Cyc_Tcenv_Fenv*_tmp31A;return(_tmp31A=_region_malloc(l,sizeof(*_tmp31A)),((
_tmp31A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpAA,((_tmp31A->type_vars=(struct
Cyc_List_List*)_tmpAB,((_tmp31A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpAC,((
_tmp31A->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmpAD),((
_tmp31A->encloser=(struct Cyc_Absyn_Stmt*)_tmpAE,((_tmp31A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)
_tmpBA,((_tmp31A->capability=(void*)((void*)_tmpB0),((_tmp31A->curr_rgn=(void*)((
void*)_tmpB1),((_tmp31A->in_notreadctxt=(int)_tmpB2,((_tmp31A->fnrgn=(struct
_RegionHandle*)l,_tmp31A)))))))))))))))))))));}}}void*Cyc_Tcenv_return_typ(
struct Cyc_Tcenv_Tenv*te);void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv _tmpBF;struct Cyc_Tcenv_SharedFenv*_tmpC0;const char*_tmp31B;
struct Cyc_Tcenv_Fenv*_tmpBE=Cyc_Tcenv_get_fenv(te,((_tmp31B="return_typ",
_tag_dyneither(_tmp31B,sizeof(char),11))));_tmpBF=*_tmpBE;_tmpC0=_tmpBF.shared;{
void*_tmpC2;struct Cyc_Tcenv_SharedFenv _tmpC1=*_tmpC0;_tmpC2=(void*)_tmpC1.return_typ;
return _tmpC2;}}struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*
te);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpC3=te->le;if(te->le == 0)return 0;{struct Cyc_List_List*
_tmpC5;struct Cyc_Tcenv_Fenv _tmpC4=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpC3));
_tmpC5=_tmpC4.type_vars;return _tmpC5;}}struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Core_Opt*_tmp31C;return(_tmp31C=_cycalloc(
sizeof(*_tmp31C)),((_tmp31C->v=Cyc_Tcenv_lookup_type_vars(te),_tmp31C)));}struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*tvs){struct Cyc_Tcenv_Fenv _tmpC9;struct Cyc_Tcenv_SharedFenv*
_tmpCA;struct Cyc_List_List*_tmpCB;struct Cyc_RgnOrder_RgnPO*_tmpCC;struct Cyc_Tcenv_Bindings*
_tmpCD;struct Cyc_Absyn_Stmt*_tmpCE;struct Cyc_Tcenv_CtrlEnv*_tmpCF;void*_tmpD0;
void*_tmpD1;int _tmpD2;const char*_tmp31D;struct Cyc_Tcenv_Fenv*_tmpC8=Cyc_Tcenv_get_fenv(
te,((_tmp31D="add_type_vars",_tag_dyneither(_tmp31D,sizeof(char),14))));_tmpC9=*
_tmpC8;_tmpCA=_tmpC9.shared;_tmpCB=_tmpC9.type_vars;_tmpCC=_tmpC9.region_order;
_tmpCD=_tmpC9.locals;_tmpCE=_tmpC9.encloser;_tmpCF=_tmpC9.ctrl_env;_tmpD0=(void*)
_tmpC9.capability;_tmpD1=(void*)_tmpC9.curr_rgn;_tmpD2=_tmpC9.in_notreadctxt;Cyc_Tcutil_add_tvar_identities(
tvs);{struct Cyc_List_List*_tmpD3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpCB);Cyc_Tcutil_check_unique_tvars(
loc,_tmpD3);{struct Cyc_Tcenv_Fenv*_tmp31E;struct Cyc_Tcenv_Fenv*_tmpD4=(_tmp31E=
_region_malloc(r,sizeof(*_tmp31E)),((_tmp31E->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpCA,((_tmp31E->type_vars=(struct Cyc_List_List*)_tmpD3,((_tmp31E->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpCC,((_tmp31E->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmpCD),((_tmp31E->encloser=(struct Cyc_Absyn_Stmt*)
_tmpCE,((_tmp31E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmpCF),((_tmp31E->capability=(void*)((void*)_tmpD0),((_tmp31E->curr_rgn=(void*)((
void*)_tmpD1),((_tmp31E->in_notreadctxt=(int)_tmpD2,((_tmp31E->fnrgn=(struct
_RegionHandle*)r,_tmp31E)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,
_tmpD4);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*
r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpD6=te->le;if(_tmpD6 == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(
r,(struct Cyc_Tcenv_Fenv*)_tmpD6);return Cyc_Tcenv_put_fenv(r,te,fe);}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Vardecl*vd){struct _dyneither_ptr*_tmpD7=(*vd->name).f2;{union Cyc_Absyn_Nmspace_union
_tmpD8=(*vd->name).f1;_LL50: if((_tmpD8.Loc_n).tag != 0)goto _LL52;_LL51: goto _LL4F;
_LL52:;_LL53: {struct Cyc_Core_Impossible_struct _tmp324;const char*_tmp323;struct
Cyc_Core_Impossible_struct*_tmp322;(int)_throw((void*)((_tmp322=_cycalloc(
sizeof(*_tmp322)),((_tmp322[0]=((_tmp324.tag=Cyc_Core_Impossible,((_tmp324.f1=((
_tmp323="add_local_var: called with Rel_n",_tag_dyneither(_tmp323,sizeof(char),
33))),_tmp324)))),_tmp322)))));}_LL4F:;}{struct Cyc_Tcenv_Fenv _tmpDE;struct Cyc_Tcenv_SharedFenv*
_tmpDF;struct Cyc_List_List*_tmpE0;struct Cyc_RgnOrder_RgnPO*_tmpE1;struct Cyc_Tcenv_Bindings*
_tmpE2;struct Cyc_Absyn_Stmt*_tmpE3;struct Cyc_Tcenv_CtrlEnv*_tmpE4;void*_tmpE5;
void*_tmpE6;int _tmpE7;const char*_tmp325;struct Cyc_Tcenv_Fenv*_tmpDD=Cyc_Tcenv_get_fenv(
te,((_tmp325="add_local_var",_tag_dyneither(_tmp325,sizeof(char),14))));_tmpDE=*
_tmpDD;_tmpDF=_tmpDE.shared;_tmpE0=_tmpDE.type_vars;_tmpE1=_tmpDE.region_order;
_tmpE2=_tmpDE.locals;_tmpE3=_tmpDE.encloser;_tmpE4=_tmpDE.ctrl_env;_tmpE5=(void*)
_tmpDE.capability;_tmpE6=(void*)_tmpDE.curr_rgn;_tmpE7=_tmpDE.in_notreadctxt;{
struct Cyc_Absyn_Local_b_struct*_tmp32B;struct Cyc_Absyn_Local_b_struct _tmp32A;
struct Cyc_Tcenv_Bindings*_tmp329;struct Cyc_Tcenv_Bindings*_tmpE8=(_tmp329=
_region_malloc(r,sizeof(*_tmp329)),((_tmp329->v=_tmpD7,((_tmp329->b=(void*)((
void*)((_tmp32B=_cycalloc(sizeof(*_tmp32B)),((_tmp32B[0]=((_tmp32A.tag=3,((
_tmp32A.f1=vd,_tmp32A)))),_tmp32B))))),((_tmp329->tl=(struct Cyc_Tcenv_Bindings*)
_tmpE2,_tmp329)))))));struct Cyc_Tcenv_Fenv*_tmp32C;struct Cyc_Tcenv_Fenv*_tmpE9=(
_tmp32C=_region_malloc(r,sizeof(*_tmp32C)),((_tmp32C->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpDF,((_tmp32C->type_vars=(struct Cyc_List_List*)_tmpE0,((_tmp32C->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpE1,((_tmp32C->locals=(struct Cyc_Tcenv_Bindings*)
_tmpE8,((_tmp32C->encloser=(struct Cyc_Absyn_Stmt*)_tmpE3,((_tmp32C->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmpE4),((_tmp32C->capability=(
void*)((void*)_tmpE5),((_tmp32C->curr_rgn=(void*)((void*)_tmpE6),((_tmp32C->in_notreadctxt=(
int)_tmpE7,((_tmp32C->fnrgn=(struct _RegionHandle*)r,_tmp32C)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmpE9);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpEE=te->le;
if(_tmpEE == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*
_tmpF0;struct Cyc_List_List*_tmpF1;struct Cyc_RgnOrder_RgnPO*_tmpF2;struct Cyc_Tcenv_Bindings*
_tmpF3;struct Cyc_Absyn_Stmt*_tmpF4;struct Cyc_Tcenv_CtrlEnv*_tmpF5;void*_tmpF6;
void*_tmpF7;int _tmpF8;struct Cyc_Tcenv_Fenv _tmpEF=*_tmpEE;_tmpF0=_tmpEF.shared;
_tmpF1=_tmpEF.type_vars;_tmpF2=_tmpEF.region_order;_tmpF3=_tmpEF.locals;_tmpF4=
_tmpEF.encloser;_tmpF5=_tmpEF.ctrl_env;_tmpF6=(void*)_tmpEF.capability;_tmpF7=(
void*)_tmpEF.curr_rgn;_tmpF8=_tmpEF.in_notreadctxt;{struct Cyc_Tcenv_Fenv*_tmp32D;
struct Cyc_Tcenv_Fenv*_tmpF9=(_tmp32D=_region_malloc(r,sizeof(*_tmp32D)),((
_tmp32D->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpF0,((_tmp32D->type_vars=(struct
Cyc_List_List*)_tmpF1,((_tmp32D->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpF2,((
_tmp32D->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmpF3),((
_tmp32D->encloser=(struct Cyc_Absyn_Stmt*)_tmpF4,((_tmp32D->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmpF5),((_tmp32D->capability=(void*)((void*)_tmpF6),((
_tmp32D->curr_rgn=(void*)((void*)_tmpF7),((_tmp32D->in_notreadctxt=(int)1,((
_tmp32D->fnrgn=(struct _RegionHandle*)r,_tmp32D)))))))))))))))))))));return Cyc_Tcenv_put_fenv(
r,te,_tmpF9);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpFB=te->le;
if(_tmpFB == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*
_tmpFD;struct Cyc_List_List*_tmpFE;struct Cyc_RgnOrder_RgnPO*_tmpFF;struct Cyc_Tcenv_Bindings*
_tmp100;struct Cyc_Absyn_Stmt*_tmp101;struct Cyc_Tcenv_CtrlEnv*_tmp102;void*
_tmp103;void*_tmp104;int _tmp105;struct Cyc_Tcenv_Fenv _tmpFC=*_tmpFB;_tmpFD=_tmpFC.shared;
_tmpFE=_tmpFC.type_vars;_tmpFF=_tmpFC.region_order;_tmp100=_tmpFC.locals;_tmp101=
_tmpFC.encloser;_tmp102=_tmpFC.ctrl_env;_tmp103=(void*)_tmpFC.capability;_tmp104=(
void*)_tmpFC.curr_rgn;_tmp105=_tmpFC.in_notreadctxt;{struct Cyc_Tcenv_Fenv*
_tmp32E;struct Cyc_Tcenv_Fenv*_tmp106=(_tmp32E=_region_malloc(r,sizeof(*_tmp32E)),((
_tmp32E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpFD,((_tmp32E->type_vars=(struct
Cyc_List_List*)_tmpFE,((_tmp32E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpFF,((
_tmp32E->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmp100),((
_tmp32E->encloser=(struct Cyc_Absyn_Stmt*)_tmp101,((_tmp32E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmp102),((_tmp32E->capability=(void*)((void*)_tmp103),((
_tmp32E->curr_rgn=(void*)((void*)_tmp104),((_tmp32E->in_notreadctxt=(int)0,((
_tmp32E->fnrgn=(struct _RegionHandle*)r,_tmp32E)))))))))))))))))))));return Cyc_Tcenv_put_fenv(
r,te,_tmp106);}}}int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp108=te->le;if(_tmp108 == 0)
return 0;{struct Cyc_Tcenv_Fenv _tmp10A;int _tmp10B;struct Cyc_Tcenv_Fenv*_tmp109=(
struct Cyc_Tcenv_Fenv*)_tmp108;_tmp10A=*_tmp109;_tmp10B=_tmp10A.in_notreadctxt;
return _tmp10B;}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*
vd);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){struct _dyneither_ptr*
_tmp10C=(*vd->name).f2;struct Cyc_Tcenv_Fenv _tmp10F;struct Cyc_Tcenv_SharedFenv*
_tmp110;struct Cyc_List_List*_tmp111;struct Cyc_RgnOrder_RgnPO*_tmp112;struct Cyc_Tcenv_Bindings*
_tmp113;struct Cyc_Absyn_Stmt*_tmp114;struct Cyc_Tcenv_CtrlEnv*_tmp115;void*
_tmp116;void*_tmp117;int _tmp118;const char*_tmp32F;struct Cyc_Tcenv_Fenv*_tmp10E=
Cyc_Tcenv_get_fenv(te,((_tmp32F="add_pat_var",_tag_dyneither(_tmp32F,sizeof(char),
12))));_tmp10F=*_tmp10E;_tmp110=_tmp10F.shared;_tmp111=_tmp10F.type_vars;_tmp112=
_tmp10F.region_order;_tmp113=_tmp10F.locals;_tmp114=_tmp10F.encloser;_tmp115=
_tmp10F.ctrl_env;_tmp116=(void*)_tmp10F.capability;_tmp117=(void*)_tmp10F.curr_rgn;
_tmp118=_tmp10F.in_notreadctxt;{struct Cyc_Absyn_Pat_b_struct*_tmp335;struct Cyc_Absyn_Pat_b_struct
_tmp334;struct Cyc_Tcenv_Bindings*_tmp333;struct Cyc_Tcenv_Bindings*_tmp119=(
_tmp333=_region_malloc(r,sizeof(*_tmp333)),((_tmp333->v=_tmp10C,((_tmp333->b=(
void*)((void*)((_tmp335=_cycalloc(sizeof(*_tmp335)),((_tmp335[0]=((_tmp334.tag=4,((
_tmp334.f1=vd,_tmp334)))),_tmp335))))),((_tmp333->tl=(struct Cyc_Tcenv_Bindings*)
_tmp113,_tmp333)))))));struct Cyc_Tcenv_Fenv*_tmp336;struct Cyc_Tcenv_Fenv*_tmp11A=(
_tmp336=_region_malloc(r,sizeof(*_tmp336)),((_tmp336->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp110,((_tmp336->type_vars=(struct Cyc_List_List*)_tmp111,((_tmp336->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp112,((_tmp336->locals=(struct Cyc_Tcenv_Bindings*)
_tmp119,((_tmp336->encloser=(struct Cyc_Absyn_Stmt*)_tmp114,((_tmp336->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp115),((_tmp336->capability=(
void*)((void*)_tmp116),((_tmp336->curr_rgn=(void*)((void*)_tmp117),((_tmp336->in_notreadctxt=(
int)_tmp118,((_tmp336->fnrgn=(struct _RegionHandle*)r,_tmp336)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp11A);}}void*Cyc_Tcenv_lookup_ordinary(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct
_tuple1*q);void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct Cyc_Tcenv_Fenv*_tmp11F=
te->le;struct _tuple1 _tmp121;union Cyc_Absyn_Nmspace_union _tmp122;struct
_dyneither_ptr*_tmp123;struct _tuple1*_tmp120=q;_tmp121=*_tmp120;_tmp122=_tmp121.f1;
_tmp123=_tmp121.f2;{union Cyc_Absyn_Nmspace_union _tmp124=_tmp122;struct Cyc_List_List*
_tmp125;_LL55: if((_tmp124.Loc_n).tag != 0)goto _LL57;_LL56: if(_tmp11F == 0)(int)
_throw((void*)Cyc_Dict_Absent);goto _LL58;_LL57: if((_tmp124.Rel_n).tag != 1)goto
_LL59;_tmp125=(_tmp124.Rel_n).f1;if(_tmp125 != 0)goto _LL59;if(!(_tmp11F != 0))goto
_LL59;_LL58: {struct Cyc_Tcenv_Fenv _tmp127;struct Cyc_Tcenv_Bindings*_tmp128;
struct Cyc_Tcenv_Fenv*_tmp126=(struct Cyc_Tcenv_Fenv*)_tmp11F;_tmp127=*_tmp126;
_tmp128=_tmp127.locals;{struct _handler_cons _tmp129;_push_handler(& _tmp129);{int
_tmp12B=0;if(setjmp(_tmp129.handler))_tmp12B=1;if(!_tmp12B){{struct Cyc_Tcenv_VarRes_struct
_tmp339;struct Cyc_Tcenv_VarRes_struct*_tmp338;void*_tmp12E=(void*)((_tmp338=
_region_malloc(r,sizeof(*_tmp338)),((_tmp338[0]=((_tmp339.tag=0,((_tmp339.f1=(
void*)Cyc_Tcenv_lookup_binding(_tmp128,_tmp123),_tmp339)))),_tmp338))));
_npop_handler(0);return _tmp12E;};_pop_handler();}else{void*_tmp12A=(void*)
_exn_thrown;void*_tmp130=_tmp12A;_LL5C:;_LL5D: return(void*)Cyc_Tcenv_lookup_ordinary_global(
te,loc,q);_LL5E:;_LL5F:(void)_throw(_tmp130);_LL5B:;}}}}_LL59:;_LL5A: {struct
_handler_cons _tmp131;_push_handler(& _tmp131);{int _tmp133=0;if(setjmp(_tmp131.handler))
_tmp133=1;if(!_tmp133){{void*_tmp134=(void*)Cyc_Tcenv_lookup_ordinary_global(te,
loc,q);_npop_handler(0);return _tmp134;};_pop_handler();}else{void*_tmp132=(void*)
_exn_thrown;void*_tmp136=_tmp132;_LL61: if(_tmp136 != Cyc_Dict_Absent)goto _LL63;
_LL62: {struct Cyc_Tcenv_VarRes_struct _tmp33C;struct Cyc_Tcenv_VarRes_struct*
_tmp33B;return(void*)((_tmp33B=_region_malloc(r,sizeof(*_tmp33B)),((_tmp33B[0]=((
_tmp33C.tag=0,((_tmp33C.f1=(void*)((void*)0),_tmp33C)))),_tmp33B))));}_LL63:;
_LL64:(void)_throw(_tmp136);_LL60:;}}}_LL54:;}}void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt);void
Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct
Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv _tmp13B;struct Cyc_Tcenv_CtrlEnv*
_tmp13C;const char*_tmp33D;struct Cyc_Tcenv_Fenv*_tmp13A=Cyc_Tcenv_get_fenv(te,((
_tmp33D="process_continue",_tag_dyneither(_tmp33D,sizeof(char),17))));_tmp13B=*
_tmp13A;_tmp13C=_tmp13B.ctrl_env;{void*_tmp13D=(void*)_tmp13C->continue_stmt;
struct Cyc_Absyn_Stmt*_tmp13E;_LL66: if(_tmp13D <= (void*)3)goto _LL68;if(*((int*)
_tmp13D)!= 0)goto _LL68;_tmp13E=((struct Cyc_Tcenv_Stmt_j_struct*)_tmp13D)->f1;
_LL67:{struct Cyc_List_List*_tmp33E;_tmp13E->non_local_preds=((_tmp33E=_cycalloc(
sizeof(*_tmp33E)),((_tmp33E->hd=s,((_tmp33E->tl=_tmp13E->non_local_preds,_tmp33E))))));}*
sopt=(struct Cyc_Absyn_Stmt*)_tmp13E;return;_LL68: if((int)_tmp13D != 0)goto _LL6A;
_LL69:{const char*_tmp341;void*_tmp340;(_tmp340=0,Cyc_Tcutil_terr(s->loc,((
_tmp341="continue not in a loop",_tag_dyneither(_tmp341,sizeof(char),23))),
_tag_dyneither(_tmp340,sizeof(void*),0)));}return;_LL6A: if((int)_tmp13D != 1)goto
_LL6C;_LL6B: goto _LL6D;_LL6C: if((int)_tmp13D != 2)goto _LL65;_LL6D: {const char*
_tmp342;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp342="bad continue destination",
_tag_dyneither(_tmp342,sizeof(char),25))));}_LL65:;}}void Cyc_Tcenv_process_break(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt);void
Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**
sopt){struct Cyc_Tcenv_Fenv _tmp145;struct Cyc_Tcenv_CtrlEnv*_tmp146;struct Cyc_Tcenv_SharedFenv*
_tmp147;const char*_tmp343;struct Cyc_Tcenv_Fenv*_tmp144=Cyc_Tcenv_get_fenv(te,((
_tmp343="process_break",_tag_dyneither(_tmp343,sizeof(char),14))));_tmp145=*
_tmp144;_tmp146=_tmp145.ctrl_env;_tmp147=_tmp145.shared;{void*_tmp148=(void*)
_tmp146->break_stmt;struct Cyc_Absyn_Stmt*_tmp149;_LL6F: if(_tmp148 <= (void*)3)
goto _LL71;if(*((int*)_tmp148)!= 0)goto _LL71;_tmp149=((struct Cyc_Tcenv_Stmt_j_struct*)
_tmp148)->f1;_LL70:{struct Cyc_List_List*_tmp344;_tmp149->non_local_preds=((
_tmp344=_cycalloc(sizeof(*_tmp344)),((_tmp344->hd=s,((_tmp344->tl=_tmp149->non_local_preds,
_tmp344))))));}*sopt=(struct Cyc_Absyn_Stmt*)_tmp149;return;_LL71: if((int)_tmp148
!= 0)goto _LL73;_LL72:{const char*_tmp347;void*_tmp346;(_tmp346=0,Cyc_Tcutil_terr(
s->loc,((_tmp347="break not in a loop or switch",_tag_dyneither(_tmp347,sizeof(
char),30))),_tag_dyneither(_tmp346,sizeof(void*),0)));}return;_LL73: if((int)
_tmp148 != 2)goto _LL75;_LL74: if((void*)_tmp147->return_typ != (void*)((void*)0)){
const char*_tmp34A;void*_tmp349;(_tmp349=0,Cyc_Tcutil_terr(s->loc,((_tmp34A="break causes function not to return a value",
_tag_dyneither(_tmp34A,sizeof(char),44))),_tag_dyneither(_tmp349,sizeof(void*),0)));}
return;_LL75: if((int)_tmp148 != 1)goto _LL6E;_LL76:{const char*_tmp34D;void*_tmp34C;(
_tmp34C=0,Cyc_Tcutil_terr(s->loc,((_tmp34D="break causes outer switch clause to implicitly fallthru",
_tag_dyneither(_tmp34D,sizeof(char),56))),_tag_dyneither(_tmp34C,sizeof(void*),0)));}
return;_LL6E:;}}void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt);void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**
sopt){struct Cyc_Tcenv_Fenv _tmp153;struct Cyc_Tcenv_SharedFenv*_tmp154;const char*
_tmp34E;struct Cyc_Tcenv_Fenv*_tmp152=Cyc_Tcenv_get_fenv(te,((_tmp34E="process_goto",
_tag_dyneither(_tmp34E,sizeof(char),13))));_tmp153=*_tmp152;_tmp154=_tmp153.shared;{
struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp154->seen_labels,l);if(sopt2 == 
0){struct Cyc_Dict_Dict _tmp155=_tmp154->needed_labels;struct Cyc_List_List**slopt=((
struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(
_tmp155,l);struct _RegionHandle*frgn=_tmp154->frgn;if(slopt == 0){struct Cyc_List_List**
_tmp34F;slopt=((_tmp34F=_region_malloc(frgn,sizeof(*_tmp34F)),((_tmp34F[0]=0,
_tmp34F))));}{struct Cyc_List_List*_tmp350;struct Cyc_List_List*new_needed=(
_tmp350=_cycalloc(sizeof(*_tmp350)),((_tmp350->hd=s,((_tmp350->tl=*((struct Cyc_List_List**)
_check_null(slopt)),_tmp350)))));_tmp154->needed_labels=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(
_tmp155,l,new_needed);}}else{struct Cyc_Absyn_Stmt*s=*sopt2;{struct Cyc_List_List*
_tmp351;s->non_local_preds=((_tmp351=_cycalloc(sizeof(*_tmp351)),((_tmp351->hd=s,((
_tmp351->tl=s->non_local_preds,_tmp351))))));}*sopt=(struct Cyc_Absyn_Stmt*)s;}}}
struct _tuple4*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Switch_clause***clauseopt);struct _tuple4*Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***
clauseopt){struct Cyc_Tcenv_Fenv _tmp15B;struct Cyc_Tcenv_CtrlEnv*_tmp15C;const char*
_tmp352;struct Cyc_Tcenv_Fenv*_tmp15A=Cyc_Tcenv_get_fenv(te,((_tmp352="process_fallthru",
_tag_dyneither(_tmp352,sizeof(char),17))));_tmp15B=*_tmp15A;_tmp15C=_tmp15B.ctrl_env;{
struct _tuple4*_tmp15D=(struct _tuple4*)_tmp15C->fallthru_clause;if(_tmp15D != 0){{
struct Cyc_List_List*_tmp353;(((*_tmp15D).f1)->body)->non_local_preds=((_tmp353=
_cycalloc(sizeof(*_tmp353)),((_tmp353->hd=s,((_tmp353->tl=(((*_tmp15D).f1)->body)->non_local_preds,
_tmp353))))));}{struct Cyc_Absyn_Switch_clause**_tmp354;*clauseopt=(struct Cyc_Absyn_Switch_clause**)((
_tmp354=_cycalloc(sizeof(*_tmp354)),((_tmp354[0]=(*_tmp15D).f1,_tmp354))));}}
return _tmp15D;}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*
r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,
struct Cyc_Absyn_Switch_clause*clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct
Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){struct Cyc_Tcenv_Fenv
_tmp162;struct Cyc_Tcenv_SharedFenv*_tmp163;struct Cyc_List_List*_tmp164;struct Cyc_RgnOrder_RgnPO*
_tmp165;struct Cyc_Tcenv_Bindings*_tmp166;struct Cyc_Absyn_Stmt*_tmp167;struct Cyc_Tcenv_CtrlEnv*
_tmp168;void*_tmp169;void*_tmp16A;int _tmp16B;const char*_tmp355;struct Cyc_Tcenv_Fenv*
_tmp161=Cyc_Tcenv_get_fenv(te,((_tmp355="set_fallthru",_tag_dyneither(_tmp355,
sizeof(char),13))));_tmp162=*_tmp161;_tmp163=_tmp162.shared;_tmp164=_tmp162.type_vars;
_tmp165=_tmp162.region_order;_tmp166=_tmp162.locals;_tmp167=_tmp162.encloser;
_tmp168=_tmp162.ctrl_env;_tmp169=(void*)_tmp162.capability;_tmp16A=(void*)
_tmp162.curr_rgn;_tmp16B=_tmp162.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv _tmp16D;
struct _RegionHandle*_tmp16E;void*_tmp16F;void*_tmp170;struct _tuple4*_tmp171;void*
_tmp172;int _tmp173;struct Cyc_Tcenv_CtrlEnv*_tmp16C=_tmp168;_tmp16D=*_tmp16C;
_tmp16E=_tmp16D.ctrl_rgn;_tmp16F=(void*)_tmp16D.continue_stmt;_tmp170=(void*)
_tmp16D.break_stmt;_tmp171=_tmp16D.fallthru_clause;_tmp172=(void*)_tmp16D.next_stmt;
_tmp173=_tmp16D.try_depth;{struct Cyc_List_List*ft_typ=0;for(0;vds != 0;vds=vds->tl){
struct Cyc_List_List*_tmp356;ft_typ=((_tmp356=_region_malloc(_tmp16E,sizeof(*
_tmp356)),((_tmp356->hd=(void*)((void*)((struct Cyc_Absyn_Vardecl*)vds->hd)->type),((
_tmp356->tl=ft_typ,_tmp356))))));}{struct Cyc_Tcenv_CList*_tmp175=(struct Cyc_Tcenv_CList*)
Cyc_List_imp_rev(ft_typ);struct _tuple4*_tmp359;struct Cyc_Tcenv_CtrlEnv*_tmp358;
struct Cyc_Tcenv_CtrlEnv*_tmp176=(_tmp358=_region_malloc(r,sizeof(*_tmp358)),((
_tmp358->ctrl_rgn=_tmp16E,((_tmp358->continue_stmt=(void*)_tmp16F,((_tmp358->break_stmt=(
void*)_tmp170,((_tmp358->fallthru_clause=((_tmp359=_region_malloc(_tmp16E,
sizeof(*_tmp359)),((_tmp359->f1=clause,((_tmp359->f2=new_tvs,((_tmp359->f3=
_tmp175,_tmp359)))))))),((_tmp358->next_stmt=(void*)_tmp172,((_tmp358->try_depth=
_tmp173,_tmp358)))))))))))));struct Cyc_Tcenv_Fenv*_tmp35A;struct Cyc_Tcenv_Fenv*
_tmp177=(_tmp35A=_region_malloc(r,sizeof(*_tmp35A)),((_tmp35A->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp163,((_tmp35A->type_vars=(struct Cyc_List_List*)_tmp164,((_tmp35A->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp165,((_tmp35A->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp166),((_tmp35A->encloser=(struct Cyc_Absyn_Stmt*)
_tmp167,((_tmp35A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp176,((_tmp35A->capability=(
void*)((void*)_tmp169),((_tmp35A->curr_rgn=(void*)((void*)_tmp16A),((_tmp35A->in_notreadctxt=(
int)_tmp16B,((_tmp35A->fnrgn=(struct _RegionHandle*)r,_tmp35A)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp177);}}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){const char*_tmp35B;struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp35B="clear_fallthru",
_tag_dyneither(_tmp35B,sizeof(char),15)))));struct Cyc_Tcenv_Fenv _tmp17C;struct
Cyc_Tcenv_CtrlEnv*_tmp17D;struct Cyc_Tcenv_Fenv*_tmp17B=fe;_tmp17C=*_tmp17B;
_tmp17D=_tmp17C.ctrl_env;_tmp17D->fallthru_clause=0;return Cyc_Tcenv_put_fenv(r,
te,fe);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
struct Cyc_Tcenv_Fenv _tmp181;struct Cyc_Tcenv_SharedFenv*_tmp182;struct Cyc_List_List*
_tmp183;struct Cyc_RgnOrder_RgnPO*_tmp184;struct Cyc_Tcenv_Bindings*_tmp185;struct
Cyc_Absyn_Stmt*_tmp186;struct Cyc_Tcenv_CtrlEnv*_tmp187;void*_tmp188;void*_tmp189;
int _tmp18A;const char*_tmp35C;struct Cyc_Tcenv_Fenv*_tmp180=Cyc_Tcenv_get_fenv(te,((
_tmp35C="set_in_loop",_tag_dyneither(_tmp35C,sizeof(char),12))));_tmp181=*
_tmp180;_tmp182=_tmp181.shared;_tmp183=_tmp181.type_vars;_tmp184=_tmp181.region_order;
_tmp185=_tmp181.locals;_tmp186=_tmp181.encloser;_tmp187=_tmp181.ctrl_env;_tmp188=(
void*)_tmp181.capability;_tmp189=(void*)_tmp181.curr_rgn;_tmp18A=_tmp181.in_notreadctxt;{
struct Cyc_Tcenv_Stmt_j_struct*_tmp367;struct Cyc_Tcenv_Stmt_j_struct _tmp366;
struct Cyc_Tcenv_Stmt_j_struct*_tmp365;struct Cyc_Tcenv_Stmt_j_struct _tmp364;
struct Cyc_Tcenv_CtrlEnv*_tmp363;struct Cyc_Tcenv_CtrlEnv*new_cenv=(_tmp363=
_region_malloc(r,sizeof(*_tmp363)),((_tmp363->ctrl_rgn=r,((_tmp363->continue_stmt=(
void*)((void*)((_tmp367=_region_malloc(r,sizeof(*_tmp367)),((_tmp367[0]=((
_tmp366.tag=0,((_tmp366.f1=continue_dest,_tmp366)))),_tmp367))))),((_tmp363->break_stmt=(
void*)((void*)((void*)_tmp187->next_stmt)),((_tmp363->next_stmt=(void*)((void*)((
_tmp365=_region_malloc(r,sizeof(*_tmp365)),((_tmp365[0]=((_tmp364.tag=0,((
_tmp364.f1=continue_dest,_tmp364)))),_tmp365))))),((_tmp363->fallthru_clause=(
struct _tuple4*)_tmp187->fallthru_clause,((_tmp363->try_depth=_tmp187->try_depth,
_tmp363)))))))))))));struct Cyc_Tcenv_Fenv*_tmp368;struct Cyc_Tcenv_Fenv*new_fenv=(
_tmp368=_region_malloc(r,sizeof(*_tmp368)),((_tmp368->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp182,((_tmp368->type_vars=(struct Cyc_List_List*)_tmp183,((_tmp368->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp184,((_tmp368->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp185),((_tmp368->encloser=(struct Cyc_Absyn_Stmt*)
_tmp186,((_tmp368->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp368->capability=(
void*)((void*)_tmp188),((_tmp368->curr_rgn=(void*)((void*)_tmp189),((_tmp368->in_notreadctxt=(
int)_tmp18A,((_tmp368->fnrgn=(struct _RegionHandle*)r,_tmp368)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,new_fenv);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){const char*_tmp369;struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp369="enter_try",
_tag_dyneither(_tmp369,sizeof(char),10)))));struct Cyc_Tcenv_Fenv _tmp192;struct
Cyc_Tcenv_CtrlEnv*_tmp193;struct Cyc_Tcenv_Fenv*_tmp191=fe;_tmp192=*_tmp191;
_tmp193=_tmp192.ctrl_env;++ _tmp193->try_depth;return Cyc_Tcenv_put_fenv(r,te,fe);}
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp197;struct Cyc_Tcenv_CtrlEnv*
_tmp198;const char*_tmp36A;struct Cyc_Tcenv_Fenv*_tmp196=Cyc_Tcenv_get_fenv(te,((
_tmp36A="get_try_depth",_tag_dyneither(_tmp36A,sizeof(char),14))));_tmp197=*
_tmp196;_tmp198=_tmp197.ctrl_env;return _tmp198->try_depth;}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){const char*
_tmp36B;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(
te,((_tmp36B="set_in_switch",_tag_dyneither(_tmp36B,sizeof(char),14)))));struct
Cyc_Tcenv_Fenv _tmp19A;struct Cyc_Tcenv_CtrlEnv*_tmp19B;struct Cyc_Tcenv_Fenv*
_tmp199=fe;_tmp19A=*_tmp199;_tmp19B=_tmp19A.ctrl_env;(void*)(_tmp19B->break_stmt=(
void*)((void*)_tmp19B->next_stmt));(void*)(_tmp19B->next_stmt=(void*)((void*)1));
return Cyc_Tcenv_put_fenv(r,te,fe);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j){struct Cyc_Tcenv_SharedFenv*
_tmp19F;struct Cyc_List_List*_tmp1A0;struct Cyc_RgnOrder_RgnPO*_tmp1A1;struct Cyc_Tcenv_Bindings*
_tmp1A2;struct Cyc_Absyn_Stmt*_tmp1A3;struct Cyc_Tcenv_CtrlEnv*_tmp1A4;void*
_tmp1A5;void*_tmp1A6;int _tmp1A7;const char*_tmp36C;struct Cyc_Tcenv_Fenv _tmp19E=*
Cyc_Tcenv_get_fenv(te,((_tmp36C="set_next",_tag_dyneither(_tmp36C,sizeof(char),9))));
_tmp19F=_tmp19E.shared;_tmp1A0=_tmp19E.type_vars;_tmp1A1=_tmp19E.region_order;
_tmp1A2=_tmp19E.locals;_tmp1A3=_tmp19E.encloser;_tmp1A4=_tmp19E.ctrl_env;_tmp1A5=(
void*)_tmp19E.capability;_tmp1A6=(void*)_tmp19E.curr_rgn;_tmp1A7=_tmp19E.in_notreadctxt;{
struct Cyc_Tcenv_CtrlEnv*_tmp36D;struct Cyc_Tcenv_CtrlEnv*new_cenv=(_tmp36D=
_region_malloc(r,sizeof(*_tmp36D)),((_tmp36D->ctrl_rgn=r,((_tmp36D->continue_stmt=(
void*)((void*)((void*)_tmp1A4->continue_stmt)),((_tmp36D->break_stmt=(void*)((
void*)((void*)_tmp1A4->break_stmt)),((_tmp36D->next_stmt=(void*)j,((_tmp36D->fallthru_clause=(
struct _tuple4*)_tmp1A4->fallthru_clause,((_tmp36D->try_depth=_tmp1A4->try_depth,
_tmp36D)))))))))))));struct Cyc_Tcenv_Fenv*_tmp36E;struct Cyc_Tcenv_Fenv*new_fenv=(
_tmp36E=_region_malloc(r,sizeof(*_tmp36E)),((_tmp36E->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp19F,((_tmp36E->type_vars=(struct Cyc_List_List*)_tmp1A0,((_tmp36E->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp1A1,((_tmp36E->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp1A2),((_tmp36E->encloser=(struct Cyc_Absyn_Stmt*)
_tmp1A3,((_tmp36E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp36E->capability=(
void*)((void*)_tmp1A5),((_tmp36E->curr_rgn=(void*)((void*)_tmp1A6),((_tmp36E->in_notreadctxt=(
int)_tmp1A7,((_tmp36E->fnrgn=(struct _RegionHandle*)r,_tmp36E)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,new_fenv);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*
s){struct Cyc_Tcenv_Fenv _tmp1AC;struct Cyc_Tcenv_SharedFenv*_tmp1AD;const char*
_tmp36F;struct Cyc_Tcenv_Fenv*_tmp1AB=Cyc_Tcenv_get_fenv(te,((_tmp36F="add_label",
_tag_dyneither(_tmp36F,sizeof(char),10))));_tmp1AC=*_tmp1AB;_tmp1AD=_tmp1AC.shared;{
struct Cyc_Dict_Dict needed=_tmp1AD->needed_labels;struct Cyc_List_List**sl_opt=((
struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(
needed,v);struct _RegionHandle*frgn=_tmp1AD->frgn;if(sl_opt != 0){_tmp1AD->needed_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct
_dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{struct Cyc_List_List*_tmp1AE=*
sl_opt;s->non_local_preds=_tmp1AE;for(0;_tmp1AE != 0;_tmp1AE=_tmp1AE->tl){void*
_tmp1AF=(void*)((struct Cyc_Absyn_Stmt*)_tmp1AE->hd)->r;struct Cyc_Absyn_Stmt*
_tmp1B0;struct Cyc_Absyn_Stmt**_tmp1B1;_LL78: if(_tmp1AF <= (void*)1)goto _LL7A;if(*((
int*)_tmp1AF)!= 7)goto _LL7A;_tmp1B0=((struct Cyc_Absyn_Goto_s_struct*)_tmp1AF)->f2;
_tmp1B1=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Goto_s_struct*)_tmp1AF)->f2;
_LL79:*_tmp1B1=(struct Cyc_Absyn_Stmt*)s;goto _LL77;_LL7A:;_LL7B:{const char*
_tmp370;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp370="Tcenv: add_label backpatching of non-goto",
_tag_dyneither(_tmp370,sizeof(char),42))));}goto _LL77;_LL77:;}}}if(((int(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp1AD->seen_labels,
v)){const char*_tmp374;void*_tmp373[1];struct Cyc_String_pa_struct _tmp372;(_tmp372.tag=
0,((_tmp372.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp373[0]=&
_tmp372,Cyc_Tcutil_terr(s->loc,((_tmp374="Repeated label: %s",_tag_dyneither(
_tmp374,sizeof(char),19))),_tag_dyneither(_tmp373,sizeof(void*),1)))))));}
_tmp1AD->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp1AD->seen_labels,v,
s);return te;}}int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp1B8;struct Cyc_Tcenv_SharedFenv*
_tmp1B9;const char*_tmp375;struct Cyc_Tcenv_Fenv*_tmp1B7=Cyc_Tcenv_get_fenv(te,((
_tmp375="all_labels_resolved",_tag_dyneither(_tmp375,sizeof(char),20))));_tmp1B8=*
_tmp1B7;_tmp1B9=_tmp1B8.shared;return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(
_tmp1B9->needed_labels);}struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*
te);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){struct
Cyc_Tcenv_Fenv _tmp1BC;struct Cyc_Absyn_Stmt*_tmp1BD;const char*_tmp376;struct Cyc_Tcenv_Fenv*
_tmp1BB=Cyc_Tcenv_get_fenv(te,((_tmp376="get_encloser",_tag_dyneither(_tmp376,
sizeof(char),13))));_tmp1BC=*_tmp1BB;_tmp1BD=_tmp1BC.encloser;return _tmp1BD;}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*
te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct
Cyc_Tcenv_Tenv*te){if(te->le == 0){struct Cyc_Tcenv_Tenv*_tmp377;return(_tmp377=
_region_malloc(r2,sizeof(*_tmp377)),((_tmp377->ns=te->ns,((_tmp377->ae=te->ae,((
_tmp377->le=0,((_tmp377->allow_valueof=1,_tmp377)))))))));}{struct Cyc_Tcenv_SharedFenv*
_tmp1C0;struct Cyc_List_List*_tmp1C1;struct Cyc_RgnOrder_RgnPO*_tmp1C2;struct Cyc_Tcenv_Bindings*
_tmp1C3;struct Cyc_Absyn_Stmt*_tmp1C4;struct Cyc_Tcenv_CtrlEnv*_tmp1C5;void*
_tmp1C6;void*_tmp1C7;int _tmp1C8;struct Cyc_Tcenv_Fenv _tmp1BF=*((struct Cyc_Tcenv_Fenv*)
_check_null(te->le));_tmp1C0=_tmp1BF.shared;_tmp1C1=_tmp1BF.type_vars;_tmp1C2=
_tmp1BF.region_order;_tmp1C3=_tmp1BF.locals;_tmp1C4=_tmp1BF.encloser;_tmp1C5=
_tmp1BF.ctrl_env;_tmp1C6=(void*)_tmp1BF.capability;_tmp1C7=(void*)_tmp1BF.curr_rgn;
_tmp1C8=_tmp1BF.in_notreadctxt;{struct Cyc_Tcenv_Fenv*_tmp378;struct Cyc_Tcenv_Fenv*
_tmp1C9=(_tmp378=_region_malloc(r2,sizeof(*_tmp378)),((_tmp378->shared=(struct
Cyc_Tcenv_SharedFenv*)_tmp1C0,((_tmp378->type_vars=(struct Cyc_List_List*)_tmp1C1,((
_tmp378->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1C2,((_tmp378->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmp1C3),((_tmp378->encloser=(
struct Cyc_Absyn_Stmt*)_tmp1C4,((_tmp378->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmp1C5),((_tmp378->capability=(void*)((void*)_tmp1C6),((
_tmp378->curr_rgn=(void*)((void*)_tmp1C7),((_tmp378->in_notreadctxt=(int)_tmp1C8,((
_tmp378->fnrgn=(struct _RegionHandle*)r2,_tmp378)))))))))))))))))))));struct Cyc_Tcenv_Tenv*
_tmp379;return(_tmp379=_region_malloc(r2,sizeof(*_tmp379)),((_tmp379->ns=te->ns,((
_tmp379->ae=te->ae,((_tmp379->le=_tmp1C9,((_tmp379->allow_valueof=1,_tmp379)))))))));}}}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){struct Cyc_Tcenv_SharedFenv*
_tmp1CE;struct Cyc_List_List*_tmp1CF;struct Cyc_RgnOrder_RgnPO*_tmp1D0;struct Cyc_Tcenv_Bindings*
_tmp1D1;struct Cyc_Absyn_Stmt*_tmp1D2;struct Cyc_Tcenv_CtrlEnv*_tmp1D3;void*
_tmp1D4;void*_tmp1D5;int _tmp1D6;const char*_tmp37A;struct Cyc_Tcenv_Fenv _tmp1CD=*
Cyc_Tcenv_get_fenv(te,((_tmp37A="set_encloser",_tag_dyneither(_tmp37A,sizeof(
char),13))));_tmp1CE=_tmp1CD.shared;_tmp1CF=_tmp1CD.type_vars;_tmp1D0=_tmp1CD.region_order;
_tmp1D1=_tmp1CD.locals;_tmp1D2=_tmp1CD.encloser;_tmp1D3=_tmp1CD.ctrl_env;_tmp1D4=(
void*)_tmp1CD.capability;_tmp1D5=(void*)_tmp1CD.curr_rgn;_tmp1D6=_tmp1CD.in_notreadctxt;{
struct Cyc_Tcenv_Fenv*_tmp37B;struct Cyc_Tcenv_Fenv*_tmp1D7=(_tmp37B=
_region_malloc(r,sizeof(*_tmp37B)),((_tmp37B->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp1CE,((_tmp37B->type_vars=(struct Cyc_List_List*)_tmp1CF,((_tmp37B->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp1D0,((_tmp37B->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp1D1),((_tmp37B->encloser=(struct Cyc_Absyn_Stmt*)s,((
_tmp37B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp1D3),((
_tmp37B->capability=(void*)((void*)_tmp1D4),((_tmp37B->curr_rgn=(void*)((void*)
_tmp1D5),((_tmp37B->in_notreadctxt=(int)_tmp1D6,((_tmp37B->fnrgn=(struct
_RegionHandle*)r,_tmp37B)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,
_tmp1D7);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int
resetable,int opened){struct Cyc_Absyn_Tvar*tv;{void*_tmp1D9=Cyc_Tcutil_compress(
rgn);struct Cyc_Absyn_Tvar*_tmp1DA;_LL7D: if(_tmp1D9 <= (void*)4)goto _LL7F;if(*((
int*)_tmp1D9)!= 1)goto _LL7F;_tmp1DA=((struct Cyc_Absyn_VarType_struct*)_tmp1D9)->f1;
_LL7E: tv=_tmp1DA;goto _LL7C;_LL7F:;_LL80:{const char*_tmp37C;tv=((struct Cyc_Absyn_Tvar*(*)(
struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp37C="bad add region",
_tag_dyneither(_tmp37C,sizeof(char),15))));}goto _LL7C;_LL7C:;}{struct Cyc_Tcenv_SharedFenv*
_tmp1DE;struct Cyc_List_List*_tmp1DF;struct Cyc_RgnOrder_RgnPO*_tmp1E0;struct Cyc_Tcenv_Bindings*
_tmp1E1;struct Cyc_Absyn_Stmt*_tmp1E2;struct Cyc_Tcenv_CtrlEnv*_tmp1E3;void*
_tmp1E4;void*_tmp1E5;int _tmp1E6;const char*_tmp37D;struct Cyc_Tcenv_Fenv _tmp1DD=*
Cyc_Tcenv_get_fenv(te,((_tmp37D="add_region",_tag_dyneither(_tmp37D,sizeof(char),
11))));_tmp1DE=_tmp1DD.shared;_tmp1DF=_tmp1DD.type_vars;_tmp1E0=_tmp1DD.region_order;
_tmp1E1=_tmp1DD.locals;_tmp1E2=_tmp1DD.encloser;_tmp1E3=_tmp1DD.ctrl_env;_tmp1E4=(
void*)_tmp1DD.capability;_tmp1E5=(void*)_tmp1DD.curr_rgn;_tmp1E6=_tmp1DD.in_notreadctxt;
_tmp1E0=Cyc_RgnOrder_add_youngest(_tmp1DE->frgn,_tmp1E0,tv,resetable,opened);{
struct Cyc_Absyn_JoinEff_struct _tmp390;struct Cyc_Absyn_AccessEff_struct*_tmp38F;
struct Cyc_Absyn_AccessEff_struct _tmp38E;struct Cyc_List_List*_tmp38D;struct Cyc_List_List*
_tmp38C;struct Cyc_Absyn_JoinEff_struct*_tmp38B;_tmp1E4=(void*)((_tmp38B=
_cycalloc(sizeof(*_tmp38B)),((_tmp38B[0]=((_tmp390.tag=20,((_tmp390.f1=((_tmp38C=
_cycalloc(sizeof(*_tmp38C)),((_tmp38C->hd=(void*)((void*)((_tmp38F=_cycalloc(
sizeof(*_tmp38F)),((_tmp38F[0]=((_tmp38E.tag=19,((_tmp38E.f1=(void*)rgn,_tmp38E)))),
_tmp38F))))),((_tmp38C->tl=((_tmp38D=_cycalloc(sizeof(*_tmp38D)),((_tmp38D->hd=(
void*)_tmp1E4,((_tmp38D->tl=0,_tmp38D)))))),_tmp38C)))))),_tmp390)))),_tmp38B))));}{
struct Cyc_Tcenv_Fenv*_tmp391;struct Cyc_Tcenv_Fenv*_tmp1ED=(_tmp391=
_region_malloc(r,sizeof(*_tmp391)),((_tmp391->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp1DE,((_tmp391->type_vars=(struct Cyc_List_List*)_tmp1DF,((_tmp391->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp1E0,((_tmp391->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp1E1),((_tmp391->encloser=(struct Cyc_Absyn_Stmt*)
_tmp1E2,((_tmp391->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp1E3),((_tmp391->capability=(void*)((void*)_tmp1E4),((_tmp391->curr_rgn=(void*)((
void*)_tmp1E5),((_tmp391->in_notreadctxt=(int)_tmp1E6,((_tmp391->fnrgn=(struct
_RegionHandle*)r,_tmp391)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,
_tmp1ED);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*
block_rgn);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*
block_rgn){struct Cyc_Tcenv_SharedFenv*_tmp1F1;struct Cyc_List_List*_tmp1F2;struct
Cyc_RgnOrder_RgnPO*_tmp1F3;struct Cyc_Tcenv_Bindings*_tmp1F4;struct Cyc_Absyn_Stmt*
_tmp1F5;struct Cyc_Tcenv_CtrlEnv*_tmp1F6;void*_tmp1F7;void*_tmp1F8;int _tmp1F9;
const char*_tmp392;struct Cyc_Tcenv_Fenv _tmp1F0=*Cyc_Tcenv_get_fenv(te,((_tmp392="new_named_block",
_tag_dyneither(_tmp392,sizeof(char),16))));_tmp1F1=_tmp1F0.shared;_tmp1F2=
_tmp1F0.type_vars;_tmp1F3=_tmp1F0.region_order;_tmp1F4=_tmp1F0.locals;_tmp1F5=
_tmp1F0.encloser;_tmp1F6=_tmp1F0.ctrl_env;_tmp1F7=(void*)_tmp1F0.capability;
_tmp1F8=(void*)_tmp1F0.curr_rgn;_tmp1F9=_tmp1F0.in_notreadctxt;{const char*
_tmp393;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(
te,((_tmp393="new_block",_tag_dyneither(_tmp393,sizeof(char),10)))));struct Cyc_Absyn_VarType_struct
_tmp396;struct Cyc_Absyn_VarType_struct*_tmp395;void*block_typ=(void*)((_tmp395=
_cycalloc(sizeof(*_tmp395)),((_tmp395[0]=((_tmp396.tag=1,((_tmp396.f1=block_rgn,
_tmp396)))),_tmp395))));{struct Cyc_List_List*_tmp397;_tmp1F2=((_tmp397=_cycalloc(
sizeof(*_tmp397)),((_tmp397->hd=block_rgn,((_tmp397->tl=_tmp1F2,_tmp397))))));}
Cyc_Tcutil_check_unique_tvars(loc,_tmp1F2);_tmp1F3=Cyc_RgnOrder_add_youngest(
_tmp1F1->frgn,_tmp1F3,block_rgn,0,0);{struct Cyc_Absyn_JoinEff_struct _tmp3AA;
struct Cyc_Absyn_AccessEff_struct*_tmp3A9;struct Cyc_Absyn_AccessEff_struct _tmp3A8;
struct Cyc_List_List*_tmp3A7;struct Cyc_List_List*_tmp3A6;struct Cyc_Absyn_JoinEff_struct*
_tmp3A5;_tmp1F7=(void*)((_tmp3A5=_cycalloc(sizeof(*_tmp3A5)),((_tmp3A5[0]=((
_tmp3AA.tag=20,((_tmp3AA.f1=((_tmp3A6=_cycalloc(sizeof(*_tmp3A6)),((_tmp3A6->hd=(
void*)((void*)((_tmp3A9=_cycalloc(sizeof(*_tmp3A9)),((_tmp3A9[0]=((_tmp3A8.tag=
19,((_tmp3A8.f1=(void*)block_typ,_tmp3A8)))),_tmp3A9))))),((_tmp3A6->tl=((
_tmp3A7=_cycalloc(sizeof(*_tmp3A7)),((_tmp3A7->hd=(void*)_tmp1F7,((_tmp3A7->tl=0,
_tmp3A7)))))),_tmp3A6)))))),_tmp3AA)))),_tmp3A5))));}_tmp1F8=block_typ;{struct
Cyc_Tcenv_Fenv*_tmp3AB;struct Cyc_Tcenv_Fenv*_tmp201=(_tmp3AB=_region_malloc(r,
sizeof(*_tmp3AB)),((_tmp3AB->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1F1,((
_tmp3AB->type_vars=(struct Cyc_List_List*)_tmp1F2,((_tmp3AB->region_order=(struct
Cyc_RgnOrder_RgnPO*)_tmp1F3,((_tmp3AB->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp1F4),((_tmp3AB->encloser=(struct Cyc_Absyn_Stmt*)
_tmp1F5,((_tmp3AB->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp1F6),((_tmp3AB->capability=(void*)((void*)_tmp1F7),((_tmp3AB->curr_rgn=(void*)((
void*)_tmp1F8),((_tmp3AB->in_notreadctxt=(int)_tmp1F9,((_tmp3AB->fnrgn=(struct
_RegionHandle*)r,_tmp3AB)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,
_tmp201);}}}static struct Cyc_Absyn_Eq_kb_struct Cyc_Tcenv_rgn_kb={0,(void*)((void*)
3)};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te){struct
Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);Cyc_Tcutil_add_tvar_identity(
t);return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*
loc);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*
r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*loc){
struct Cyc_Tcenv_SharedFenv*_tmp209;struct Cyc_List_List*_tmp20A;struct Cyc_RgnOrder_RgnPO*
_tmp20B;struct Cyc_Tcenv_Bindings*_tmp20C;struct Cyc_Absyn_Stmt*_tmp20D;struct Cyc_Tcenv_CtrlEnv*
_tmp20E;void*_tmp20F;void*_tmp210;int _tmp211;const char*_tmp3AC;struct Cyc_Tcenv_Fenv
_tmp208=*Cyc_Tcenv_get_fenv(te,((_tmp3AC="new_outlives_constraints",
_tag_dyneither(_tmp3AC,sizeof(char),25))));_tmp209=_tmp208.shared;_tmp20A=
_tmp208.type_vars;_tmp20B=_tmp208.region_order;_tmp20C=_tmp208.locals;_tmp20D=
_tmp208.encloser;_tmp20E=_tmp208.ctrl_env;_tmp20F=(void*)_tmp208.capability;
_tmp210=(void*)_tmp208.curr_rgn;_tmp211=_tmp208.in_notreadctxt;for(0;cs != 0;cs=
cs->tl){_tmp20B=Cyc_RgnOrder_add_outlives_constraint(_tmp209->frgn,_tmp20B,(*((
struct _tuple0*)cs->hd)).f1,(*((struct _tuple0*)cs->hd)).f2,loc);}{struct Cyc_Tcenv_Fenv*
_tmp3AD;struct Cyc_Tcenv_Fenv*_tmp212=(_tmp3AD=_region_malloc(r,sizeof(*_tmp3AD)),((
_tmp3AD->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp209,((_tmp3AD->type_vars=(
struct Cyc_List_List*)_tmp20A,((_tmp3AD->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp20B,((_tmp3AD->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp20C),((_tmp3AD->encloser=(struct Cyc_Absyn_Stmt*)_tmp20D,((_tmp3AD->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp20E),((_tmp3AD->capability=(
void*)((void*)_tmp20F),((_tmp3AD->curr_rgn=(void*)((void*)_tmp210),((_tmp3AD->in_notreadctxt=(
int)_tmp211,((_tmp3AD->fnrgn=(struct _RegionHandle*)r,_tmp3AD)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp212);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple5**
oldtv,struct Cyc_Position_Segment*loc);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple5**
oldtv,struct Cyc_Position_Segment*loc){void*_tmp214=Cyc_Tcutil_compress(r1);void*
_tmp215=Cyc_Tcutil_compress(r2);void*_tmp216=Cyc_Tcutil_typ_kind(_tmp214);void*
_tmp217=Cyc_Tcutil_typ_kind(_tmp215);int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp216,
_tmp217);int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp217,_tmp216);if(!r1_le_r2  && !
r2_le_r1){{const char*_tmp3B4;void*_tmp3B3[4];struct Cyc_String_pa_struct _tmp3B2;
struct Cyc_String_pa_struct _tmp3B1;struct Cyc_String_pa_struct _tmp3B0;struct Cyc_String_pa_struct
_tmp3AF;(_tmp3AF.tag=0,((_tmp3AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp217)),((_tmp3B0.tag=0,((_tmp3B0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp216)),((
_tmp3B1.tag=0,((_tmp3B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp215)),((_tmp3B2.tag=0,((_tmp3B2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp214)),((_tmp3B3[0]=& _tmp3B2,((_tmp3B3[
1]=& _tmp3B1,((_tmp3B3[2]=& _tmp3B0,((_tmp3B3[3]=& _tmp3AF,Cyc_Tcutil_terr(loc,((
_tmp3B4="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n",
_tag_dyneither(_tmp3B4,sizeof(char),82))),_tag_dyneither(_tmp3B3,sizeof(void*),4)))))))))))))))))))))))));}
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{if(r1_le_r2  && !
r2_le_r1){struct Cyc_Absyn_Tvar*_tmp21F;void*_tmp220;struct _tuple5 _tmp21E=Cyc_Tcutil_swap_kind(
_tmp215,Cyc_Tcutil_kind_to_bound(_tmp216));_tmp21F=_tmp21E.f1;_tmp220=_tmp21E.f2;{
struct _tuple5*_tmp3B5;*oldtv=((_tmp3B5=_region_malloc(r,sizeof(*_tmp3B5)),((
_tmp3B5->f1=_tmp21F,((_tmp3B5->f2=_tmp220,_tmp3B5))))));}}else{if(!r1_le_r2  && 
r2_le_r1){struct Cyc_Absyn_Tvar*_tmp223;void*_tmp224;struct _tuple5 _tmp222=Cyc_Tcutil_swap_kind(
_tmp214,Cyc_Tcutil_kind_to_bound(_tmp217));_tmp223=_tmp222.f1;_tmp224=_tmp222.f2;{
struct _tuple5*_tmp3B6;*oldtv=((_tmp3B6=_region_malloc(r,sizeof(*_tmp3B6)),((
_tmp3B6->f1=_tmp223,((_tmp3B6->f2=_tmp224,_tmp3B6))))));}}}}{struct _RegionHandle*
_tmp226=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp227=0;if(_tmp214 != (void*)
2  && _tmp214 != (void*)3){struct Cyc_Absyn_AccessEff_struct _tmp3B9;struct Cyc_Absyn_AccessEff_struct*
_tmp3B8;void*eff1=(void*)((_tmp3B8=_cycalloc(sizeof(*_tmp3B8)),((_tmp3B8[0]=((
_tmp3B9.tag=19,((_tmp3B9.f1=(void*)_tmp214,_tmp3B9)))),_tmp3B8))));struct _tuple0*
_tmp3BC;struct Cyc_List_List*_tmp3BB;_tmp227=((_tmp3BB=_region_malloc(_tmp226,
sizeof(*_tmp3BB)),((_tmp3BB->hd=((_tmp3BC=_region_malloc(_tmp226,sizeof(*_tmp3BC)),((
_tmp3BC->f1=eff1,((_tmp3BC->f2=_tmp215,_tmp3BC)))))),((_tmp3BB->tl=_tmp227,
_tmp3BB))))));}if(_tmp215 != (void*)2  && _tmp215 != (void*)3){struct Cyc_Absyn_AccessEff_struct
_tmp3BF;struct Cyc_Absyn_AccessEff_struct*_tmp3BE;void*eff2=(void*)((_tmp3BE=
_cycalloc(sizeof(*_tmp3BE)),((_tmp3BE[0]=((_tmp3BF.tag=19,((_tmp3BF.f1=(void*)
_tmp215,_tmp3BF)))),_tmp3BE))));struct _tuple0*_tmp3C2;struct Cyc_List_List*
_tmp3C1;_tmp227=((_tmp3C1=_region_malloc(_tmp226,sizeof(*_tmp3C1)),((_tmp3C1->hd=((
_tmp3C2=_region_malloc(_tmp226,sizeof(*_tmp3C2)),((_tmp3C2->f1=eff2,((_tmp3C2->f2=
_tmp214,_tmp3C2)))))),((_tmp3C1->tl=_tmp227,_tmp3C1))))));}return Cyc_Tcenv_new_outlives_constraints((
struct _RegionHandle*)_tmp226,te,_tmp227,loc);}}void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*
te);void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*
_tmp230=te->le;if(_tmp230 == 0)return(void*)2;{struct Cyc_Tcenv_Fenv*fe=(struct Cyc_Tcenv_Fenv*)
_tmp230;struct Cyc_Tcenv_Fenv _tmp232;void*_tmp233;struct Cyc_Tcenv_Fenv*_tmp231=fe;
_tmp232=*_tmp231;_tmp233=(void*)_tmp232.curr_rgn;return _tmp233;}}void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*rgn);void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*rgn){const char*
_tmp3C3;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,((_tmp3C3="check_rgn_accessible",
_tag_dyneither(_tmp3C3,sizeof(char),21))));struct Cyc_Tcenv_Fenv _tmp235;void*
_tmp236;struct Cyc_RgnOrder_RgnPO*_tmp237;struct Cyc_Tcenv_Fenv*_tmp234=fe;_tmp235=*
_tmp234;_tmp236=(void*)_tmp235.capability;_tmp237=_tmp235.region_order;if(Cyc_Tcutil_region_in_effect(
0,rgn,_tmp236) || Cyc_Tcutil_region_in_effect(1,rgn,_tmp236))return;{struct Cyc_Absyn_AccessEff_struct
_tmp3C6;struct Cyc_Absyn_AccessEff_struct*_tmp3C5;if(Cyc_RgnOrder_eff_outlives_eff(
_tmp237,(void*)((_tmp3C5=_cycalloc(sizeof(*_tmp3C5)),((_tmp3C5[0]=((_tmp3C6.tag=
19,((_tmp3C6.f1=(void*)rgn,_tmp3C6)))),_tmp3C5)))),_tmp236))return;}{const char*
_tmp3CA;void*_tmp3C9[1];struct Cyc_String_pa_struct _tmp3C8;(_tmp3C8.tag=0,((
_tmp3C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
rgn)),((_tmp3C9[0]=& _tmp3C8,Cyc_Tcutil_terr(loc,((_tmp3CA="Expression accesses unavailable region %s",
_tag_dyneither(_tmp3CA,sizeof(char),42))),_tag_dyneither(_tmp3C9,sizeof(void*),1)))))));}}
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*rgn);void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void*rgn){Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
struct Cyc_Tcenv_Fenv _tmp240;struct Cyc_RgnOrder_RgnPO*_tmp241;const char*_tmp3CB;
struct Cyc_Tcenv_Fenv*_tmp23F=Cyc_Tcenv_get_fenv(te,((_tmp3CB="check_rgn_resetable",
_tag_dyneither(_tmp3CB,sizeof(char),20))));_tmp240=*_tmp23F;_tmp241=_tmp240.region_order;{
void*_tmp242=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp243;_LL82: if(
_tmp242 <= (void*)4)goto _LL84;if(*((int*)_tmp242)!= 1)goto _LL84;_tmp243=((struct
Cyc_Absyn_VarType_struct*)_tmp242)->f1;_LL83: if(!Cyc_RgnOrder_is_region_resetable(
_tmp241,_tmp243)){const char*_tmp3CF;void*_tmp3CE[1];struct Cyc_String_pa_struct
_tmp3CD;(_tmp3CD.tag=0,((_tmp3CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(rgn)),((_tmp3CE[0]=& _tmp3CD,Cyc_Tcutil_terr(loc,((_tmp3CF="Region %s is not resetable",
_tag_dyneither(_tmp3CF,sizeof(char),27))),_tag_dyneither(_tmp3CE,sizeof(void*),1)))))));}
return;_LL84:;_LL85: {const char*_tmp3D2;void*_tmp3D1;(_tmp3D1=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3D2="check_rgn_resetable",
_tag_dyneither(_tmp3D2,sizeof(char),20))),_tag_dyneither(_tmp3D1,sizeof(void*),0)));}
_LL81:;}}}int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*
rt_b);int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp249=te->le;rt_a=Cyc_Tcutil_compress(rt_a);rt_b=Cyc_Tcutil_compress(
rt_b);if(_tmp249 == 0)return rt_a == (void*)3  || rt_a == (void*)2  && rt_b != (void*)3;{
struct Cyc_Tcenv_Fenv*fe=(struct Cyc_Tcenv_Fenv*)_tmp249;struct Cyc_Tcenv_Fenv
_tmp24B;struct Cyc_RgnOrder_RgnPO*_tmp24C;struct Cyc_Tcenv_Fenv*_tmp24A=fe;_tmp24B=*
_tmp24A;_tmp24C=_tmp24B.region_order;{struct Cyc_Absyn_AccessEff_struct _tmp3D5;
struct Cyc_Absyn_AccessEff_struct*_tmp3D4;int _tmp24D=Cyc_RgnOrder_effect_outlives(
_tmp24C,(void*)((_tmp3D4=_cycalloc(sizeof(*_tmp3D4)),((_tmp3D4[0]=((_tmp3D5.tag=
19,((_tmp3D5.f1=(void*)rt_a,_tmp3D5)))),_tmp3D4)))),rt_b);return _tmp24D;}}}
struct _tuple7{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;struct Cyc_Position_Segment*
f4;};void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*eff);void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*eff){struct Cyc_Tcenv_Fenv _tmp252;void*
_tmp253;struct Cyc_RgnOrder_RgnPO*_tmp254;struct Cyc_Tcenv_SharedFenv*_tmp255;
const char*_tmp3D6;struct Cyc_Tcenv_Fenv*_tmp251=Cyc_Tcenv_get_fenv(te,((_tmp3D6="check_effect_accessible",
_tag_dyneither(_tmp3D6,sizeof(char),24))));_tmp252=*_tmp251;_tmp253=(void*)
_tmp252.capability;_tmp254=_tmp252.region_order;_tmp255=_tmp252.shared;if(Cyc_Tcutil_subset_effect(
0,eff,_tmp253))return;if(Cyc_RgnOrder_eff_outlives_eff(_tmp254,eff,_tmp253))
return;{struct _RegionHandle*frgn=_tmp255->frgn;struct _tuple7*_tmp3D9;struct Cyc_List_List*
_tmp3D8;_tmp255->delayed_effect_checks=((_tmp3D8=_region_malloc(frgn,sizeof(*
_tmp3D8)),((_tmp3D8->hd=((_tmp3D9=_region_malloc(frgn,sizeof(*_tmp3D9)),((
_tmp3D9->f1=_tmp253,((_tmp3D9->f2=eff,((_tmp3D9->f3=_tmp254,((_tmp3D9->f4=loc,
_tmp3D9)))))))))),((_tmp3D8->tl=_tmp255->delayed_effect_checks,_tmp3D8))))));}}
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp25A;struct Cyc_Tcenv_SharedFenv*
_tmp25B;const char*_tmp3DA;struct Cyc_Tcenv_Fenv*_tmp259=Cyc_Tcenv_get_fenv(te,((
_tmp3DA="check_delayed_effects",_tag_dyneither(_tmp3DA,sizeof(char),22))));
_tmp25A=*_tmp259;_tmp25B=_tmp25A.shared;{struct Cyc_List_List*_tmp25C=_tmp25B->delayed_effect_checks;
for(0;_tmp25C != 0;_tmp25C=_tmp25C->tl){struct _tuple7 _tmp25E;void*_tmp25F;void*
_tmp260;struct Cyc_RgnOrder_RgnPO*_tmp261;struct Cyc_Position_Segment*_tmp262;
struct _tuple7*_tmp25D=(struct _tuple7*)_tmp25C->hd;_tmp25E=*_tmp25D;_tmp25F=
_tmp25E.f1;_tmp260=_tmp25E.f2;_tmp261=_tmp25E.f3;_tmp262=_tmp25E.f4;if(Cyc_Tcutil_subset_effect(
1,_tmp260,_tmp25F))continue;if(Cyc_RgnOrder_eff_outlives_eff(_tmp261,_tmp260,
_tmp25F))continue;{const char*_tmp3DF;void*_tmp3DE[2];struct Cyc_String_pa_struct
_tmp3DD;struct Cyc_String_pa_struct _tmp3DC;(_tmp3DC.tag=0,((_tmp3DC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp260)),((_tmp3DD.tag=
0,((_tmp3DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp25F)),((_tmp3DE[0]=& _tmp3DD,((_tmp3DE[1]=& _tmp3DC,Cyc_Tcutil_terr(_tmp262,((
_tmp3DF="Capability \n%s\ndoes not cover function's effect\n%s",_tag_dyneither(
_tmp3DF,sizeof(char),51))),_tag_dyneither(_tmp3DE,sizeof(void*),2)))))))))))));}}}}
struct _tuple8{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;struct Cyc_Position_Segment*
f3;};void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*po);void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po){struct Cyc_Tcenv_Fenv*
_tmp267=te->le;if(_tmp267 == 0){for(0;po != 0;po=po->tl){struct Cyc_Absyn_AccessEff_struct
_tmp3E2;struct Cyc_Absyn_AccessEff_struct*_tmp3E1;if(!Cyc_Tcutil_subset_effect(1,(*((
struct _tuple0*)po->hd)).f1,Cyc_Absyn_empty_effect) || !Cyc_Tcutil_subset_effect(
1,(void*)((_tmp3E1=_cycalloc(sizeof(*_tmp3E1)),((_tmp3E1[0]=((_tmp3E2.tag=19,((
_tmp3E2.f1=(void*)(*((struct _tuple0*)po->hd)).f2,_tmp3E2)))),_tmp3E1)))),Cyc_Absyn_empty_effect)){
const char*_tmp3E5;void*_tmp3E4;(_tmp3E4=0,Cyc_Tcutil_terr(loc,((_tmp3E5="the required region ordering is not satisfied here",
_tag_dyneither(_tmp3E5,sizeof(char),51))),_tag_dyneither(_tmp3E4,sizeof(void*),0)));}}
return;}{struct Cyc_Tcenv_Fenv _tmp26D;struct Cyc_RgnOrder_RgnPO*_tmp26E;struct Cyc_Tcenv_SharedFenv*
_tmp26F;struct Cyc_Tcenv_Fenv*_tmp26C=(struct Cyc_Tcenv_Fenv*)_tmp267;_tmp26D=*
_tmp26C;_tmp26E=_tmp26D.region_order;_tmp26F=_tmp26D.shared;if(!Cyc_RgnOrder_satisfies_constraints(
_tmp26E,po,(void*)2,0)){struct _tuple8*_tmp3E8;struct Cyc_List_List*_tmp3E7;
_tmp26F->delayed_constraint_checks=((_tmp3E7=_region_malloc(_tmp26F->frgn,
sizeof(*_tmp3E7)),((_tmp3E7->hd=((_tmp3E8=_region_malloc(_tmp26F->frgn,sizeof(*
_tmp3E8)),((_tmp3E8->f1=_tmp26E,((_tmp3E8->f2=po,((_tmp3E8->f3=loc,_tmp3E8)))))))),((
_tmp3E7->tl=_tmp26F->delayed_constraint_checks,_tmp3E7))))));}}}void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv _tmp274;struct Cyc_Tcenv_SharedFenv*_tmp275;const char*
_tmp3E9;struct Cyc_Tcenv_Fenv*_tmp273=Cyc_Tcenv_get_fenv(te,((_tmp3E9="check_delayed_constraints",
_tag_dyneither(_tmp3E9,sizeof(char),26))));_tmp274=*_tmp273;_tmp275=_tmp274.shared;{
struct Cyc_List_List*_tmp276=_tmp275->delayed_constraint_checks;for(0;_tmp276 != 0;
_tmp276=_tmp276->tl){struct _tuple8 _tmp278;struct Cyc_RgnOrder_RgnPO*_tmp279;
struct Cyc_List_List*_tmp27A;struct Cyc_Position_Segment*_tmp27B;struct _tuple8*
_tmp277=(struct _tuple8*)_tmp276->hd;_tmp278=*_tmp277;_tmp279=_tmp278.f1;_tmp27A=
_tmp278.f2;_tmp27B=_tmp278.f3;if(!Cyc_RgnOrder_satisfies_constraints(_tmp279,
_tmp27A,(void*)2,1)){const char*_tmp3EC;void*_tmp3EB;(_tmp3EB=0,Cyc_Tcutil_terr(
_tmp27B,((_tmp3EC="the required region ordering is not satisfied here",
_tag_dyneither(_tmp3EC,sizeof(char),51))),_tag_dyneither(_tmp3EB,sizeof(void*),0)));}}}}
struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct
Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Fndecl*fd);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Fndecl*fd){struct
Cyc_Tcenv_Bindings*locals=0;struct _dyneither_ptr*_tmp3F9;const char*_tmp3F8;void*
_tmp3F7[1];struct Cyc_String_pa_struct _tmp3F6;struct Cyc_Absyn_Tvar*_tmp3F5;struct
Cyc_Absyn_Tvar*rgn0=(_tmp3F5=_cycalloc(sizeof(*_tmp3F5)),((_tmp3F5->name=((
_tmp3F9=_cycalloc(sizeof(*_tmp3F9)),((_tmp3F9[0]=(struct _dyneither_ptr)((_tmp3F6.tag=
0,((_tmp3F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((
_tmp3F7[0]=& _tmp3F6,Cyc_aprintf(((_tmp3F8="`%s",_tag_dyneither(_tmp3F8,sizeof(
char),4))),_tag_dyneither(_tmp3F7,sizeof(void*),1)))))))),_tmp3F9)))),((_tmp3F5->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp3F5->kind=(void*)((void*)& Cyc_Tcenv_rgn_kb),
_tmp3F5)))))));struct Cyc_List_List*_tmp3FA;struct Cyc_List_List*_tmp27E=(_tmp3FA=
_cycalloc(sizeof(*_tmp3FA)),((_tmp3FA->hd=rgn0,((_tmp3FA->tl=fd->tvs,_tmp3FA)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp27E);{struct Cyc_RgnOrder_RgnPO*_tmp27F=Cyc_RgnOrder_initial_fn_po(
r,fd->tvs,fd->rgn_po,(void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,
rgn0,loc);struct Cyc_Absyn_VarType_struct _tmp3FD;struct Cyc_Absyn_VarType_struct*
_tmp3FC;void*param_rgn=(void*)((_tmp3FC=_cycalloc(sizeof(*_tmp3FC)),((_tmp3FC[0]=((
_tmp3FD.tag=1,((_tmp3FD.f1=rgn0,_tmp3FD)))),_tmp3FC))));struct Cyc_Core_Opt*
_tmp3FE;struct Cyc_Core_Opt*param_rgn_opt=(_tmp3FE=_cycalloc(sizeof(*_tmp3FE)),((
_tmp3FE->v=(void*)param_rgn,_tmp3FE)));struct Cyc_List_List*_tmp280=0;{struct Cyc_List_List*
_tmp281=fd->args;for(0;_tmp281 != 0;_tmp281=_tmp281->tl){struct Cyc_Absyn_Vardecl
_tmp408;union Cyc_Absyn_Nmspace_union _tmp407;struct _tuple1*_tmp406;struct Cyc_Absyn_Vardecl*
_tmp405;struct Cyc_Absyn_Vardecl*_tmp282=(_tmp405=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1),((_tmp405[0]=((_tmp408.sc=(void*)((void*)2),((_tmp408.name=((_tmp406=
_cycalloc(sizeof(*_tmp406)),((_tmp406->f1=(union Cyc_Absyn_Nmspace_union)((union
Cyc_Absyn_Nmspace_union)(((_tmp407.Loc_n).tag=0,_tmp407))),((_tmp406->f2=(*((
struct _tuple9*)_tmp281->hd)).f1,_tmp406)))))),((_tmp408.tq=(*((struct _tuple9*)
_tmp281->hd)).f2,((_tmp408.type=(void*)(*((struct _tuple9*)_tmp281->hd)).f3,((
_tmp408.initializer=0,((_tmp408.rgn=param_rgn_opt,((_tmp408.attributes=0,((
_tmp408.escapes=0,_tmp408)))))))))))))))),_tmp405)));{struct Cyc_List_List _tmp40B;
struct Cyc_List_List*_tmp40A;_tmp280=((_tmp40A=_cycalloc(sizeof(struct Cyc_List_List)
* 1),((_tmp40A[0]=((_tmp40B.hd=_tmp282,((_tmp40B.tl=_tmp280,_tmp40B)))),_tmp40A))));}{
struct Cyc_Absyn_Param_b_struct*_tmp411;struct Cyc_Absyn_Param_b_struct _tmp410;
struct Cyc_Tcenv_Bindings*_tmp40F;locals=((_tmp40F=_region_malloc(r,sizeof(*
_tmp40F)),((_tmp40F->v=(*((struct _tuple9*)_tmp281->hd)).f1,((_tmp40F->b=(void*)((
void*)((_tmp411=_cycalloc(sizeof(*_tmp411)),((_tmp411[0]=((_tmp410.tag=2,((
_tmp410.f1=_tmp282,_tmp410)))),_tmp411))))),((_tmp40F->tl=locals,_tmp40F))))))));}}}
if(fd->cyc_varargs != 0){struct Cyc_Core_Opt*_tmp28D;struct Cyc_Absyn_Tqual _tmp28E;
void*_tmp28F;int _tmp290;struct Cyc_Absyn_VarargInfo _tmp28C=*((struct Cyc_Absyn_VarargInfo*)
_check_null(fd->cyc_varargs));_tmp28D=_tmp28C.name;_tmp28E=_tmp28C.tq;_tmp28F=(
void*)_tmp28C.type;_tmp290=_tmp28C.inject;if(_tmp28D != 0){void*_tmp291=Cyc_Absyn_dyneither_typ(
_tmp28F,param_rgn,_tmp28E,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl _tmp41B;
union Cyc_Absyn_Nmspace_union _tmp41A;struct _tuple1*_tmp419;struct Cyc_Absyn_Vardecl*
_tmp418;struct Cyc_Absyn_Vardecl*_tmp292=(_tmp418=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1),((_tmp418[0]=((_tmp41B.sc=(void*)((void*)2),((_tmp41B.name=((_tmp419=
_cycalloc(sizeof(*_tmp419)),((_tmp419->f1=(union Cyc_Absyn_Nmspace_union)((union
Cyc_Absyn_Nmspace_union)(((_tmp41A.Loc_n).tag=0,_tmp41A))),((_tmp419->f2=(struct
_dyneither_ptr*)_tmp28D->v,_tmp419)))))),((_tmp41B.tq=Cyc_Absyn_empty_tqual(0),((
_tmp41B.type=(void*)_tmp291,((_tmp41B.initializer=0,((_tmp41B.rgn=param_rgn_opt,((
_tmp41B.attributes=0,((_tmp41B.escapes=0,_tmp41B)))))))))))))))),_tmp418)));{
struct Cyc_List_List*_tmp41C;_tmp280=((_tmp41C=_cycalloc(sizeof(*_tmp41C)),((
_tmp41C->hd=_tmp292,((_tmp41C->tl=_tmp280,_tmp41C))))));}{struct Cyc_Absyn_Param_b_struct*
_tmp422;struct Cyc_Absyn_Param_b_struct _tmp421;struct Cyc_Tcenv_Bindings*_tmp420;
locals=((_tmp420=_region_malloc(r,sizeof(*_tmp420)),((_tmp420->v=(struct
_dyneither_ptr*)_tmp28D->v,((_tmp420->b=(void*)((void*)((_tmp422=_cycalloc(
sizeof(*_tmp422)),((_tmp422[0]=((_tmp421.tag=2,((_tmp421.f1=_tmp292,_tmp421)))),
_tmp422))))),((_tmp420->tl=locals,_tmp420))))))));}}else{const char*_tmp425;void*
_tmp424;(_tmp424=0,Cyc_Tcutil_terr(loc,((_tmp425="missing name for varargs",
_tag_dyneither(_tmp425,sizeof(char),25))),_tag_dyneither(_tmp424,sizeof(void*),0)));}}{
struct Cyc_Core_Opt _tmp428;struct Cyc_Core_Opt*_tmp427;fd->param_vardecls=((
_tmp427=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp427[0]=((_tmp428.v=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp280),
_tmp428)),_tmp427))));}{struct Cyc_Tcenv_SharedFenv*_tmp446;struct Cyc_Tcenv_CtrlEnv*
_tmp445;struct Cyc_Absyn_JoinEff_struct*_tmp444;struct Cyc_List_List*_tmp443;
struct Cyc_List_List*_tmp442;struct Cyc_Absyn_AccessEff_struct _tmp441;struct Cyc_Absyn_AccessEff_struct*
_tmp440;struct Cyc_Absyn_JoinEff_struct _tmp43F;struct Cyc_Tcenv_Fenv*_tmp43E;
return(_tmp43E=_region_malloc(r,sizeof(*_tmp43E)),((_tmp43E->shared=(struct Cyc_Tcenv_SharedFenv*)((
_tmp446=_region_malloc(r,sizeof(*_tmp446)),((_tmp446->frgn=r,((_tmp446->return_typ=(
void*)((void*)fd->ret_type),((_tmp446->seen_labels=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp446->needed_labels=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp446->delayed_effect_checks=0,((_tmp446->delayed_constraint_checks=
0,_tmp446)))))))))))))),((_tmp43E->type_vars=(struct Cyc_List_List*)_tmp27E,((
_tmp43E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp27F,((_tmp43E->locals=(
struct Cyc_Tcenv_Bindings*)locals,((_tmp43E->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((
_tmp43E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((_tmp445=_region_malloc(r,sizeof(*
_tmp445)),((_tmp445->ctrl_rgn=r,((_tmp445->continue_stmt=(void*)((void*)0),((
_tmp445->break_stmt=(void*)((void*)0),((_tmp445->fallthru_clause=0,((_tmp445->next_stmt=(
void*)((void*)2),((_tmp445->try_depth=0,_tmp445)))))))))))))),((_tmp43E->capability=(
void*)((void*)((void*)((_tmp444=_cycalloc(sizeof(*_tmp444)),((_tmp444[0]=((
_tmp43F.tag=20,((_tmp43F.f1=((_tmp443=_cycalloc(sizeof(*_tmp443)),((_tmp443->hd=(
void*)((void*)((_tmp440=_cycalloc(sizeof(*_tmp440)),((_tmp440[0]=((_tmp441.tag=
19,((_tmp441.f1=(void*)param_rgn,_tmp441)))),_tmp440))))),((_tmp443->tl=((
_tmp442=_cycalloc(sizeof(*_tmp442)),((_tmp442->hd=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(fd->effect))->v),((_tmp442->tl=0,_tmp442)))))),_tmp443)))))),_tmp43F)))),
_tmp444)))))),((_tmp43E->curr_rgn=(void*)((void*)param_rgn),((_tmp43E->in_notreadctxt=(
int)0,((_tmp43E->fnrgn=(struct _RegionHandle*)r,_tmp43E)))))))))))))))))))));}}}
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*loc,struct
Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*
fd){struct Cyc_Tcenv_Fenv _tmp2B2;struct Cyc_Tcenv_Bindings*_tmp2B3;struct Cyc_RgnOrder_RgnPO*
_tmp2B4;struct Cyc_List_List*_tmp2B5;struct Cyc_Tcenv_SharedFenv*_tmp2B6;struct
_RegionHandle*_tmp2B7;struct Cyc_Tcenv_Fenv*_tmp2B1=old_fenv;_tmp2B2=*_tmp2B1;
_tmp2B3=_tmp2B2.locals;_tmp2B4=_tmp2B2.region_order;_tmp2B5=_tmp2B2.type_vars;
_tmp2B6=_tmp2B2.shared;_tmp2B7=_tmp2B2.fnrgn;{struct _RegionHandle*_tmp2B8=
_tmp2B6->frgn;struct Cyc_Tcenv_Bindings*_tmp2B9=(struct Cyc_Tcenv_Bindings*)
_tmp2B3;struct _dyneither_ptr*_tmp453;const char*_tmp452;void*_tmp451[1];struct Cyc_String_pa_struct
_tmp450;struct Cyc_Absyn_Tvar*_tmp44F;struct Cyc_Absyn_Tvar*rgn0=(_tmp44F=
_cycalloc(sizeof(*_tmp44F)),((_tmp44F->name=((_tmp453=_cycalloc(sizeof(*_tmp453)),((
_tmp453[0]=(struct _dyneither_ptr)((_tmp450.tag=0,((_tmp450.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp451[0]=& _tmp450,Cyc_aprintf(((
_tmp452="`%s",_tag_dyneither(_tmp452,sizeof(char),4))),_tag_dyneither(_tmp451,
sizeof(void*),1)))))))),_tmp453)))),((_tmp44F->identity=Cyc_Tcutil_new_tvar_id(),((
_tmp44F->kind=(void*)((void*)& Cyc_Tcenv_rgn_kb),_tmp44F)))))));{struct Cyc_List_List*
_tmp2BA=fd->tvs;for(0;_tmp2BA != 0;_tmp2BA=_tmp2BA->tl){if(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp2BA->hd)== (void*)3)_tmp2B4=Cyc_RgnOrder_add_youngest(
_tmp2B8,_tmp2B4,(struct Cyc_Absyn_Tvar*)_tmp2BA->hd,0,0);else{if(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp2BA->hd)== (void*)4  || Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp2BA->hd)== (void*)5){const char*_tmp456;void*_tmp455;(_tmp455=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp456="non-intuitionistic tvar in nested_fenv",
_tag_dyneither(_tmp456,sizeof(char),39))),_tag_dyneither(_tmp455,sizeof(void*),0)));}}}}
_tmp2B4=Cyc_RgnOrder_add_youngest(_tmp2B8,_tmp2B4,rgn0,0,0);{struct Cyc_List_List*
_tmp457;struct Cyc_List_List*_tmp2BD=(_tmp457=_cycalloc(sizeof(*_tmp457)),((
_tmp457->hd=rgn0,((_tmp457->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp2B5),_tmp457)))));Cyc_Tcutil_check_unique_tvars(
loc,_tmp2BD);{struct Cyc_Absyn_VarType_struct _tmp45A;struct Cyc_Absyn_VarType_struct*
_tmp459;void*param_rgn=(void*)((_tmp459=_cycalloc(sizeof(*_tmp459)),((_tmp459[0]=((
_tmp45A.tag=1,((_tmp45A.f1=rgn0,_tmp45A)))),_tmp459))));struct Cyc_Core_Opt*
_tmp45B;struct Cyc_Core_Opt*param_rgn_opt=(_tmp45B=_cycalloc(sizeof(*_tmp45B)),((
_tmp45B->v=(void*)param_rgn,_tmp45B)));struct Cyc_List_List*_tmp2BE=0;{struct Cyc_List_List*
_tmp2BF=fd->args;for(0;_tmp2BF != 0;_tmp2BF=_tmp2BF->tl){struct Cyc_Absyn_Vardecl
_tmp465;union Cyc_Absyn_Nmspace_union _tmp464;struct _tuple1*_tmp463;struct Cyc_Absyn_Vardecl*
_tmp462;struct Cyc_Absyn_Vardecl*_tmp2C0=(_tmp462=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1),((_tmp462[0]=((_tmp465.sc=(void*)((void*)2),((_tmp465.name=((_tmp463=
_cycalloc(sizeof(*_tmp463)),((_tmp463->f1=(union Cyc_Absyn_Nmspace_union)((union
Cyc_Absyn_Nmspace_union)(((_tmp464.Loc_n).tag=0,_tmp464))),((_tmp463->f2=(*((
struct _tuple9*)_tmp2BF->hd)).f1,_tmp463)))))),((_tmp465.tq=(*((struct _tuple9*)
_tmp2BF->hd)).f2,((_tmp465.type=(void*)(*((struct _tuple9*)_tmp2BF->hd)).f3,((
_tmp465.initializer=0,((_tmp465.rgn=param_rgn_opt,((_tmp465.attributes=0,((
_tmp465.escapes=0,_tmp465)))))))))))))))),_tmp462)));{struct Cyc_List_List _tmp468;
struct Cyc_List_List*_tmp467;_tmp2BE=((_tmp467=_cycalloc(sizeof(struct Cyc_List_List)
* 1),((_tmp467[0]=((_tmp468.hd=_tmp2C0,((_tmp468.tl=_tmp2BE,_tmp468)))),_tmp467))));}{
struct Cyc_Absyn_Param_b_struct*_tmp46E;struct Cyc_Absyn_Param_b_struct _tmp46D;
struct Cyc_Tcenv_Bindings*_tmp46C;_tmp2B9=(struct Cyc_Tcenv_Bindings*)((_tmp46C=
_region_malloc(_tmp2B8,sizeof(*_tmp46C)),((_tmp46C->v=(*((struct _tuple9*)_tmp2BF->hd)).f1,((
_tmp46C->b=(void*)((void*)((_tmp46E=_cycalloc(sizeof(*_tmp46E)),((_tmp46E[0]=((
_tmp46D.tag=2,((_tmp46D.f1=_tmp2C0,_tmp46D)))),_tmp46E))))),((_tmp46C->tl=
_tmp2B9,_tmp46C))))))));}}}if(fd->cyc_varargs != 0){struct Cyc_Core_Opt*_tmp2CB;
struct Cyc_Absyn_Tqual _tmp2CC;void*_tmp2CD;int _tmp2CE;struct Cyc_Absyn_VarargInfo
_tmp2CA=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));_tmp2CB=
_tmp2CA.name;_tmp2CC=_tmp2CA.tq;_tmp2CD=(void*)_tmp2CA.type;_tmp2CE=_tmp2CA.inject;
if(_tmp2CB != 0){void*_tmp2CF=Cyc_Absyn_dyneither_typ(_tmp2CD,param_rgn,_tmp2CC,
Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl _tmp478;union Cyc_Absyn_Nmspace_union
_tmp477;struct _tuple1*_tmp476;struct Cyc_Absyn_Vardecl*_tmp475;struct Cyc_Absyn_Vardecl*
_tmp2D0=(_tmp475=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp475[0]=((
_tmp478.sc=(void*)((void*)2),((_tmp478.name=((_tmp476=_cycalloc(sizeof(*_tmp476)),((
_tmp476->f1=(union Cyc_Absyn_Nmspace_union)((union Cyc_Absyn_Nmspace_union)(((
_tmp477.Loc_n).tag=0,_tmp477))),((_tmp476->f2=(struct _dyneither_ptr*)_tmp2CB->v,
_tmp476)))))),((_tmp478.tq=Cyc_Absyn_empty_tqual(0),((_tmp478.type=(void*)
_tmp2CF,((_tmp478.initializer=0,((_tmp478.rgn=param_rgn_opt,((_tmp478.attributes=
0,((_tmp478.escapes=0,_tmp478)))))))))))))))),_tmp475)));{struct Cyc_List_List*
_tmp479;_tmp2BE=((_tmp479=_cycalloc(sizeof(*_tmp479)),((_tmp479->hd=_tmp2D0,((
_tmp479->tl=_tmp2BE,_tmp479))))));}{struct Cyc_Absyn_Param_b_struct*_tmp47F;
struct Cyc_Absyn_Param_b_struct _tmp47E;struct Cyc_Tcenv_Bindings*_tmp47D;_tmp2B9=(
struct Cyc_Tcenv_Bindings*)((_tmp47D=_region_malloc(_tmp2B8,sizeof(*_tmp47D)),((
_tmp47D->v=(struct _dyneither_ptr*)_tmp2CB->v,((_tmp47D->b=(void*)((void*)((
_tmp47F=_cycalloc(sizeof(*_tmp47F)),((_tmp47F[0]=((_tmp47E.tag=2,((_tmp47E.f1=
_tmp2D0,_tmp47E)))),_tmp47F))))),((_tmp47D->tl=_tmp2B9,_tmp47D))))))));}}else{
const char*_tmp482;void*_tmp481;(_tmp481=0,Cyc_Tcutil_terr(loc,((_tmp482="missing name for varargs",
_tag_dyneither(_tmp482,sizeof(char),25))),_tag_dyneither(_tmp481,sizeof(void*),0)));}}{
struct Cyc_Core_Opt _tmp485;struct Cyc_Core_Opt*_tmp484;fd->param_vardecls=((
_tmp484=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp484[0]=((_tmp485.v=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2BE),
_tmp485)),_tmp484))));}{struct Cyc_Tcenv_SharedFenv*_tmp4A3;struct Cyc_Tcenv_CtrlEnv*
_tmp4A2;struct Cyc_Absyn_JoinEff_struct*_tmp4A1;struct Cyc_List_List*_tmp4A0;
struct Cyc_List_List*_tmp49F;struct Cyc_Absyn_AccessEff_struct _tmp49E;struct Cyc_Absyn_AccessEff_struct*
_tmp49D;struct Cyc_Absyn_JoinEff_struct _tmp49C;struct Cyc_Tcenv_Fenv*_tmp49B;
return(struct Cyc_Tcenv_Fenv*)((_tmp49B=_region_malloc(_tmp2B8,sizeof(*_tmp49B)),((
_tmp49B->shared=(struct Cyc_Tcenv_SharedFenv*)((_tmp4A3=_region_malloc(_tmp2B8,
sizeof(*_tmp4A3)),((_tmp4A3->frgn=_tmp2B8,((_tmp4A3->return_typ=(void*)((void*)
fd->ret_type),((_tmp4A3->seen_labels=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
_tmp2B8,Cyc_strptrcmp),((_tmp4A3->needed_labels=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
_tmp2B8,Cyc_strptrcmp),((_tmp4A3->delayed_effect_checks=0,((_tmp4A3->delayed_constraint_checks=
0,_tmp4A3)))))))))))))),((_tmp49B->type_vars=(struct Cyc_List_List*)_tmp2BD,((
_tmp49B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2B4,((_tmp49B->locals=(
struct Cyc_Tcenv_Bindings*)_tmp2B9,((_tmp49B->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((
_tmp49B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)((_tmp4A2=
_region_malloc(_tmp2B8,sizeof(*_tmp4A2)),((_tmp4A2->ctrl_rgn=_tmp2B8,((_tmp4A2->continue_stmt=(
void*)((void*)0),((_tmp4A2->break_stmt=(void*)((void*)0),((_tmp4A2->fallthru_clause=
0,((_tmp4A2->next_stmt=(void*)((void*)2),((_tmp4A2->try_depth=0,_tmp4A2))))))))))))))),((
_tmp49B->capability=(void*)((void*)((void*)((_tmp4A1=_cycalloc(sizeof(*_tmp4A1)),((
_tmp4A1[0]=((_tmp49C.tag=20,((_tmp49C.f1=((_tmp4A0=_cycalloc(sizeof(*_tmp4A0)),((
_tmp4A0->hd=(void*)((void*)((_tmp49D=_cycalloc(sizeof(*_tmp49D)),((_tmp49D[0]=((
_tmp49E.tag=19,((_tmp49E.f1=(void*)param_rgn,_tmp49E)))),_tmp49D))))),((_tmp4A0->tl=((
_tmp49F=_cycalloc(sizeof(*_tmp49F)),((_tmp49F->hd=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(fd->effect))->v),((_tmp49F->tl=0,_tmp49F)))))),_tmp4A0)))))),_tmp49C)))),
_tmp4A1)))))),((_tmp49B->curr_rgn=(void*)((void*)param_rgn),((_tmp49B->in_notreadctxt=(
int)0,((_tmp49B->fnrgn=(struct _RegionHandle*)_tmp2B7,_tmp49B))))))))))))))))))))));}}}}}
