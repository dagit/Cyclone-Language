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
void*v;};extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct
Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern
char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*tag;};
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*
tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];
struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{
char*tag;};struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(
void*,void*));int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d);int Cyc_Dict_member(
struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict d,void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_rdelete(
struct _RegionHandle*,struct Cyc_Dict_Dict,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*
Cyc_Set_empty(int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rempty(struct
_RegionHandle*r,int(*cmp)(void*,void*));int Cyc_Set_member(struct Cyc_Set_Set*s,
void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};
struct Cyc_SlowDict_Dict;extern char Cyc_SlowDict_Present[8];struct Cyc_SlowDict_Present_struct{
char*tag;};extern char Cyc_SlowDict_Absent[7];struct Cyc_SlowDict_Absent_struct{
char*tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};
struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{int tag;}
;struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};int Cyc_strptrcmp(struct
_dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};
struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct
_dyneither_ptr*f2;};enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract
 = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register
 = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int
real_const;struct Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz
 = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz
 = 4};enum Cyc_Absyn_Kind{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind
 = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_UniqueRgnKind  = 4,Cyc_Absyn_TopRgnKind  = 5,
Cyc_Absyn_EffKind  = 6,Cyc_Absyn_IntKind  = 7};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed
 = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA
 = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};
struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};
struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{
struct _union_Constraint_Eq_constr Eq_constr;struct
_union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr
No_constr;};struct Cyc_Absyn_Eq_kb_struct{int tag;enum Cyc_Absyn_Kind f1;};struct
Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;enum Cyc_Absyn_Kind f2;};struct Cyc_Absyn_Tvar{struct
_dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_struct{
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
rgn_loc;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;
union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*
zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;
struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_struct{int tag;
void*f1;};struct Cyc_Absyn_Thin_ptrqual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{
int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{
int tag;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;
struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_VoidType_struct{int tag;};struct Cyc_Absyn_Evar_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;}
;struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{
int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;enum 
Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_struct{int
tag;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_ValueofType_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};
struct Cyc_Absyn_HeapRgn_struct{int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;
};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
extern struct Cyc_Absyn_HeapRgn_struct Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct
Cyc_Absyn_UniqueRgn_val;extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct
Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{
Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_struct{
int tag;int f1;};struct Cyc_Absyn_Stdcall_att_struct{int tag;};struct Cyc_Absyn_Cdecl_att_struct{
int tag;};struct Cyc_Absyn_Fastcall_att_struct{int tag;};struct Cyc_Absyn_Noreturn_att_struct{
int tag;};struct Cyc_Absyn_Const_att_struct{int tag;};struct Cyc_Absyn_Aligned_att_struct{
int tag;int f1;};struct Cyc_Absyn_Packed_att_struct{int tag;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_struct{int tag;};
struct Cyc_Absyn_Shared_att_struct{int tag;};struct Cyc_Absyn_Unused_att_struct{int
tag;};struct Cyc_Absyn_Weak_att_struct{int tag;};struct Cyc_Absyn_Dllimport_att_struct{
int tag;};struct Cyc_Absyn_Dllexport_att_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_struct{
int tag;};struct Cyc_Absyn_Constructor_att_struct{int tag;};struct Cyc_Absyn_Destructor_att_struct{
int tag;};struct Cyc_Absyn_No_check_memory_usage_att_struct{int tag;};struct Cyc_Absyn_Format_att_struct{
int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{
int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{int tag;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;union
Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct
_tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7
val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct
_union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times
 = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,
Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte
 = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor
 = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,
Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc
 = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,
Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion
 = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**
elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct
Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;
};struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnknownCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*
f3;};struct Cyc_Absyn_Throw_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{
int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct
Cyc_Absyn_Address_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{
int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{
int tag;struct Cyc_List_List*f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
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
struct Cyc_Absyn_Skip_s_struct{int tag;};struct Cyc_Absyn_Exp_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;
};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*loc;struct Cyc_List_List*
non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_struct{int tag;}
;struct Cyc_Absyn_Var_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*
f2;};struct Cyc_Absyn_Reference_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct
Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{int tag;struct Cyc_Absyn_Tvar*
f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{int tag;struct Cyc_List_List*
f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};
struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*
f3;int f4;};struct Cyc_Absyn_Null_p_struct{int tag;};struct Cyc_Absyn_Int_p_struct{
int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Enum_p_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{
int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};extern
struct Cyc_Absyn_Unresolved_b_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual
tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*defn;
struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*
f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*
f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Datatype_d_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{
int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int
tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);extern
union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;extern void*Cyc_Absyn_empty_effect;
extern struct _tuple1*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(enum Cyc_Absyn_Kind);struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct
Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
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
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_NotLoop_j_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_struct{
int tag;};struct Cyc_Tcenv_FnEnd_j_struct{int tag;};struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};extern struct Cyc_Tcenv_NotLoop_j_struct Cyc_Tcenv_NotLoop_j_val;
extern struct Cyc_Tcenv_CaseEnd_j_struct Cyc_Tcenv_CaseEnd_j_val;extern struct Cyc_Tcenv_FnEnd_j_struct
Cyc_Tcenv_FnEnd_j_val;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict
ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcenv_env_err(struct
_dyneither_ptr msg);struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(struct _RegionHandle*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);struct
Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Fenv*
old_fenv,struct Cyc_Absyn_Fndecl*new_fn);struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _dyneither_ptr*,struct
Cyc_List_List*);void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Datatypedecl**
Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple1*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
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
struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_Tcenv_CList*
f3;};struct _tuple10*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Switch_clause***);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*
loc);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**
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
int Cyc_Tcutil_kind_leq(enum Cyc_Absyn_Kind k1,enum Cyc_Absyn_Kind k2);enum Cyc_Absyn_Kind
Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,enum Cyc_Absyn_Kind def);enum Cyc_Absyn_Kind
Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);void*Cyc_Tcutil_kind_to_bound(
enum Cyc_Absyn_Kind k);struct _tuple11 Cyc_Tcutil_swap_kind(void*t,void*kb);int Cyc_Tcutil_subset_effect(
int may_constrain_evars,void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int
constrain,void*r,void*e);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,
struct Cyc_List_List*);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);char Cyc_Tcenv_Env_error[10]="Env_error";struct Cyc_Tcenv_Env_error_struct{
char*tag;};struct Cyc_Tcenv_Env_error_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg);void*Cyc_Tcenv_env_err(struct
_dyneither_ptr msg){{const char*_tmp315;void*_tmp314[1];struct Cyc_String_pa_struct
_tmp313;(_tmp313.tag=0,((_tmp313.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
msg),((_tmp314[0]=& _tmp313,Cyc_fprintf(Cyc_stderr,((_tmp315="Internal error in tcenv: %s\n",
_tag_dyneither(_tmp315,sizeof(char),29))),_tag_dyneither(_tmp314,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}
struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;struct Cyc_Tcenv_NotLoop_j_struct Cyc_Tcenv_NotLoop_j_val={
0};struct Cyc_Tcenv_CaseEnd_j_struct Cyc_Tcenv_CaseEnd_j_val={1};struct Cyc_Tcenv_FnEnd_j_struct
Cyc_Tcenv_FnEnd_j_val={2};struct Cyc_Tcenv_CtrlEnv{struct _RegionHandle*ctrl_rgn;
void*continue_stmt;void*break_stmt;struct _tuple10*fallthru_clause;void*next_stmt;
int try_depth;};struct Cyc_Tcenv_SharedFenv{struct _RegionHandle*frgn;void*
return_typ;struct Cyc_Dict_Dict seen_labels;struct Cyc_Dict_Dict needed_labels;
struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*
delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*
b;struct Cyc_Tcenv_Bindings*tl;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*
shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;
struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*
ctrl_env;void*capability;void*curr_rgn;int in_notreadctxt;struct _RegionHandle*
fnrgn;};char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_struct{
char*tag;};struct Cyc_Tcenv_NoBinding_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
void*Cyc_Tcenv_lookup_binding(struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v);
void*Cyc_Tcenv_lookup_binding(struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){
for(0;(unsigned int)bs;bs=bs->tl){if(Cyc_strptrcmp(v,bs->v)== 0)return bs->b;}(
int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(
struct _RegionHandle*r);struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct
_RegionHandle*r){struct Cyc_Tcenv_Genv*_tmp316;return(_tmp316=_region_malloc(r,
sizeof(*_tmp316)),((_tmp316->grgn=r,((_tmp316->namespaces=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Set_rempty)(r,Cyc_strptrcmp),((_tmp316->aggrdecls=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp316->datatypedecls=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp316->enumdecls=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp316->typedefs=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp316->ordinaries=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp316->availables=0,_tmp316)))))))))))))))));}struct
_tuple12{void*f1;int f2;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct
_RegionHandle*r);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){{
struct Cyc_Core_Opt*_tmp317;Cyc_Tcutil_empty_var_set=((_tmp317=_cycalloc(sizeof(*
_tmp317)),((_tmp317->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp317))));}{struct Cyc_Tcenv_Genv*
_tmpA=Cyc_Tcenv_empty_genv(r);{struct Cyc_Absyn_Datatypedecl**_tmp318;_tmpA->datatypedecls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**
v))Cyc_Dict_insert)(_tmpA->datatypedecls,(*Cyc_Absyn_exn_name).f2,((_tmp318=
_cycalloc(sizeof(*_tmp318)),((_tmp318[0]=Cyc_Absyn_exn_tud,_tmp318)))));}{struct
Cyc_List_List*_tmpC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;
for(0;_tmpC != 0;_tmpC=_tmpC->tl){struct Cyc_Tcenv_DatatypeRes_struct*_tmp31E;
struct Cyc_Tcenv_DatatypeRes_struct _tmp31D;struct _tuple12*_tmp31C;_tmpA->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct
_tuple12*v))Cyc_Dict_insert)(_tmpA->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)
_tmpC->hd)->name).f2,((_tmp31C=_region_malloc(r,sizeof(*_tmp31C)),((_tmp31C->f1=(
void*)((_tmp31E=_cycalloc(sizeof(*_tmp31E)),((_tmp31E[0]=((_tmp31D.tag=2,((
_tmp31D.f1=Cyc_Absyn_exn_tud,((_tmp31D.f2=(struct Cyc_Absyn_Datatypefield*)_tmpC->hd,
_tmp31D)))))),_tmp31E)))),((_tmp31C->f2=1,_tmp31C)))))));}}{struct Cyc_Dict_Dict
ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),
0,_tmpA);struct Cyc_Tcenv_Tenv*_tmp31F;return(_tmp31F=_region_malloc(r,sizeof(*
_tmp31F)),((_tmp31F->ns=0,((_tmp31F->ae=ae,((_tmp31F->le=0,((_tmp31F->allow_valueof=
0,_tmp31F)))))))));}}}static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*n,
struct Cyc_List_List*ns);static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*n,
struct Cyc_List_List*ns){return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,
loc,n,ns));}static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*
ns);static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0){const char*_tmp320;return((struct Cyc_List_List*(*)(struct
_dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp320="outer_namespace",_tag_dyneither(
_tmp320,sizeof(char),16))));}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
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
struct _handler_cons _tmp12;_push_handler(& _tmp12);{int _tmp14=0;if(setjmp(_tmp12.handler))
_tmp14=1;if(!_tmp14){lookup(ge2,v);{const char*_tmp324;void*_tmp323[1];struct Cyc_String_pa_struct
_tmp322;(_tmp322.tag=0,((_tmp322.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
v),((_tmp323[0]=& _tmp322,Cyc_Tcutil_terr(loc,((_tmp324="%s is ambiguous",
_tag_dyneither(_tmp324,sizeof(char),16))),_tag_dyneither(_tmp323,sizeof(void*),1)))))));};
_pop_handler();}else{void*_tmp13=(void*)_exn_thrown;void*_tmp19=_tmp13;_LL1: {
struct Cyc_Dict_Absent_struct*_tmp1A=(struct Cyc_Dict_Absent_struct*)_tmp19;if(
_tmp1A->tag != Cyc_Dict_Absent)goto _LL3;}_LL2: goto _LL0;_LL3:;_LL4:(void)_throw(
_tmp19);_LL0:;}}}}return;}static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v);static void*Cyc_Tcenv_scoped_lookup(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dyneither_ptr*),struct _dyneither_ptr*v){struct Cyc_List_List*cns=te->ns;
while(1){struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);{struct _handler_cons _tmp1B;
_push_handler(& _tmp1B);{int _tmp1D=0;if(setjmp(_tmp1B.handler))_tmp1D=1;if(!
_tmp1D){{void*result=lookup(ge,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp1E=result;_npop_handler(0);return _tmp1E;}};_pop_handler();}else{void*
_tmp1C=(void*)_exn_thrown;void*_tmp20=_tmp1C;_LL6: {struct Cyc_Dict_Absent_struct*
_tmp21=(struct Cyc_Dict_Absent_struct*)_tmp20;if(_tmp21->tag != Cyc_Dict_Absent)
goto _LL8;}_LL7: goto _LL5;_LL8:;_LL9:(void)_throw(_tmp20);_LL5:;}}}{struct Cyc_List_List*
_tmp22=ge->availables;for(0;_tmp22 != 0;_tmp22=_tmp22->tl){struct Cyc_Tcenv_Genv*
ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,(struct Cyc_List_List*)_tmp22->hd);struct _handler_cons _tmp23;_push_handler(&
_tmp23);{int _tmp25=0;if(setjmp(_tmp23.handler))_tmp25=1;if(!_tmp25){{void*result=
lookup(ge2,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)
_tmp22->hd,_tmp22->tl);{void*_tmp26=result;_npop_handler(0);return _tmp26;}};
_pop_handler();}else{void*_tmp24=(void*)_exn_thrown;void*_tmp28=_tmp24;_LLB: {
struct Cyc_Dict_Absent_struct*_tmp29=(struct Cyc_Dict_Absent_struct*)_tmp28;if(
_tmp29->tag != Cyc_Dict_Absent)goto _LLD;}_LLC: goto _LLA;_LLD:;_LLE:(void)_throw(
_tmp28);_LLA:;}}}}if(cns == 0){struct Cyc_Dict_Absent_struct _tmp327;struct Cyc_Dict_Absent_struct*
_tmp326;(int)_throw((void*)((_tmp326=_cycalloc_atomic(sizeof(*_tmp326)),((
_tmp326[0]=((_tmp327.tag=Cyc_Dict_Absent,_tmp327)),_tmp326)))));}cns=Cyc_Tcenv_outer_namespace(
cns);}}static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,
struct _dyneither_ptr*v);static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*
ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp2C=ge->ordinaries;struct
_tuple12*ans=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))
Cyc_Dict_lookup)(_tmp2C,v);(*ans).f2=1;return(*ans).f1;}static void*Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q);static
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q){struct _tuple1 _tmp2E;union Cyc_Absyn_Nmspace _tmp2F;struct
_dyneither_ptr*_tmp30;struct _tuple1*_tmp2D=q;_tmp2E=*_tmp2D;_tmp2F=_tmp2E.f1;
_tmp30=_tmp2E.f2;{union Cyc_Absyn_Nmspace _tmp31=_tmp2F;int _tmp32;struct Cyc_List_List*
_tmp33;struct Cyc_List_List*_tmp34;struct Cyc_List_List _tmp35;struct _dyneither_ptr*
_tmp36;struct Cyc_List_List*_tmp37;struct Cyc_List_List*_tmp38;_LL10: if((_tmp31.Loc_n).tag
!= 3)goto _LL12;_tmp32=(int)(_tmp31.Loc_n).val;_LL11: goto _LL13;_LL12: if((_tmp31.Rel_n).tag
!= 1)goto _LL14;_tmp33=(struct Cyc_List_List*)(_tmp31.Rel_n).val;if(_tmp33 != 0)
goto _LL14;_LL13: return((void*(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct
_dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,
_tmp30);_LL14: if((_tmp31.Rel_n).tag != 1)goto _LL16;_tmp34=(struct Cyc_List_List*)(
_tmp31.Rel_n).val;if(_tmp34 == 0)goto _LL16;_tmp35=*_tmp34;_tmp36=(struct
_dyneither_ptr*)_tmp35.hd;_tmp37=_tmp35.tl;_LL15: {struct Cyc_Tcenv_Genv*_tmp39=
Cyc_Tcenv_lookup_namespace(te,loc,_tmp36,_tmp37);return Cyc_Tcenv_lookup_ordinary_global_f(
_tmp39,_tmp30);}_LL16: if((_tmp31.Abs_n).tag != 2)goto _LLF;_tmp38=(struct Cyc_List_List*)(
_tmp31.Abs_n).val;_LL17: return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp38),
_tmp30);_LLF:;}}struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns);
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){struct
Cyc_List_List*_tmp3A=te->ns;struct _RegionHandle*_tmp3B=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp3C=0;while(1){struct Cyc_Tcenv_Genv*_tmp3D=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp3A);
struct Cyc_List_List*_tmp3E=_tmp3D->availables;struct Cyc_Set_Set*_tmp3F=_tmp3D->namespaces;{
struct Cyc_List_List*_tmp40=_tmp3E;for(0;_tmp40 != 0;_tmp40=_tmp40->tl){struct Cyc_Set_Set*
_tmp41=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp40->hd))->namespaces;if(((int(*)(
struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp41,n)){struct
Cyc_List_List*_tmp32A;struct Cyc_List_List*_tmp329;_tmp3C=((_tmp329=
_region_malloc(_tmp3B,sizeof(*_tmp329)),((_tmp329->hd=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp3A,((_tmp32A=
_cycalloc(sizeof(*_tmp32A)),((_tmp32A->hd=n,((_tmp32A->tl=ns,_tmp32A))))))),((
_tmp329->tl=_tmp3C,_tmp329))))));}}}if(((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(_tmp3F,n)){struct Cyc_List_List*_tmp32D;
struct Cyc_List_List*_tmp32C;_tmp3C=((_tmp32C=_region_malloc(_tmp3B,sizeof(*
_tmp32C)),((_tmp32C->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp3A,((_tmp32D=_cycalloc(sizeof(*_tmp32D)),((
_tmp32D->hd=n,((_tmp32D->tl=ns,_tmp32D))))))),((_tmp32C->tl=_tmp3C,_tmp32C))))));}
if(_tmp3C != 0){if(_tmp3C->tl != 0){const char*_tmp331;void*_tmp330[1];struct Cyc_String_pa_struct
_tmp32F;(_tmp32F.tag=0,((_tmp32F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
n),((_tmp330[0]=& _tmp32F,Cyc_Tcutil_terr(loc,((_tmp331="%s is ambiguous",
_tag_dyneither(_tmp331,sizeof(char),16))),_tag_dyneither(_tmp330,sizeof(void*),1)))))));}
return(struct Cyc_List_List*)_tmp3C->hd;}if(_tmp3A == 0){struct Cyc_Dict_Absent_struct
_tmp334;struct Cyc_Dict_Absent_struct*_tmp333;(int)_throw((void*)((_tmp333=
_cycalloc_atomic(sizeof(*_tmp333)),((_tmp333[0]=((_tmp334.tag=Cyc_Dict_Absent,
_tmp334)),_tmp333)))));}_tmp3A=Cyc_Tcenv_outer_namespace(_tmp3A);}}static struct
Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*ge,struct
_dyneither_ptr*v);static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp4B=ge->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*
k))Cyc_Dict_lookup)(_tmp4B,v);}struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q);struct
Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q){struct _tuple1 _tmp4D;union Cyc_Absyn_Nmspace _tmp4E;struct
_dyneither_ptr*_tmp4F;struct _tuple1*_tmp4C=q;_tmp4D=*_tmp4C;_tmp4E=_tmp4D.f1;
_tmp4F=_tmp4D.f2;{union Cyc_Absyn_Nmspace _tmp50=_tmp4E;int _tmp51;struct Cyc_List_List*
_tmp52;struct Cyc_List_List*_tmp53;struct Cyc_List_List*_tmp54;struct Cyc_List_List
_tmp55;struct _dyneither_ptr*_tmp56;struct Cyc_List_List*_tmp57;_LL19: if((_tmp50.Loc_n).tag
!= 3)goto _LL1B;_tmp51=(int)(_tmp50.Loc_n).val;_LL1A: goto _LL1C;_LL1B: if((_tmp50.Rel_n).tag
!= 1)goto _LL1D;_tmp52=(struct Cyc_List_List*)(_tmp50.Rel_n).val;if(_tmp52 != 0)
goto _LL1D;_LL1C: return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,
Cyc_Tcenv_lookup_aggrdecl_f,_tmp4F);_LL1D: if((_tmp50.Abs_n).tag != 2)goto _LL1F;
_tmp53=(struct Cyc_List_List*)(_tmp50.Abs_n).val;_LL1E: {struct Cyc_Dict_Dict
_tmp58=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,_tmp53))->aggrdecls;return((struct Cyc_Absyn_Aggrdecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp58,_tmp4F);}
_LL1F: if((_tmp50.Rel_n).tag != 1)goto _LL18;_tmp54=(struct Cyc_List_List*)(_tmp50.Rel_n).val;
if(_tmp54 == 0)goto _LL18;_tmp55=*_tmp54;_tmp56=(struct _dyneither_ptr*)_tmp55.hd;
_tmp57=_tmp55.tl;_LL20: {struct Cyc_Dict_Dict _tmp59=(Cyc_Tcenv_lookup_namespace(
te,loc,_tmp56,_tmp57))->aggrdecls;return((struct Cyc_Absyn_Aggrdecl**(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp59,_tmp4F);}_LL18:;}}
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*
ge,struct _dyneither_ptr*v);static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){struct Cyc_Dict_Dict _tmp5A=ge->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(_tmp5A,v);}struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q);struct
Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1 _tmp5C;union Cyc_Absyn_Nmspace
_tmp5D;struct _dyneither_ptr*_tmp5E;struct _tuple1*_tmp5B=q;_tmp5C=*_tmp5B;_tmp5D=
_tmp5C.f1;_tmp5E=_tmp5C.f2;{union Cyc_Absyn_Nmspace _tmp5F=_tmp5D;int _tmp60;struct
Cyc_List_List*_tmp61;struct Cyc_List_List*_tmp62;struct Cyc_List_List _tmp63;struct
_dyneither_ptr*_tmp64;struct Cyc_List_List*_tmp65;struct Cyc_List_List*_tmp66;
_LL22: if((_tmp5F.Loc_n).tag != 3)goto _LL24;_tmp60=(int)(_tmp5F.Loc_n).val;_LL23:
goto _LL25;_LL24: if((_tmp5F.Rel_n).tag != 1)goto _LL26;_tmp61=(struct Cyc_List_List*)(
_tmp5F.Rel_n).val;if(_tmp61 != 0)goto _LL26;_LL25: return((struct Cyc_Absyn_Datatypedecl**(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Datatypedecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_datatypedecl_f,_tmp5E);_LL26: if((_tmp5F.Rel_n).tag != 1)
goto _LL28;_tmp62=(struct Cyc_List_List*)(_tmp5F.Rel_n).val;if(_tmp62 == 0)goto
_LL28;_tmp63=*_tmp62;_tmp64=(struct _dyneither_ptr*)_tmp63.hd;_tmp65=_tmp63.tl;
_LL27: {struct Cyc_Dict_Dict _tmp67=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp64,
_tmp65))->datatypedecls;return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict
d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp67,_tmp5E);}_LL28: if((_tmp5F.Abs_n).tag
!= 2)goto _LL21;_tmp66=(struct Cyc_List_List*)(_tmp5F.Abs_n).val;_LL29: {struct Cyc_Dict_Dict
_tmp68=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,_tmp66))->datatypedecls;return((struct Cyc_Absyn_Datatypedecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp68,_tmp5E);}
_LL21:;}}static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_xdatatypedecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v);static struct Cyc_Absyn_Datatypedecl**
Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}struct Cyc_Absyn_Datatypedecl***
Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q);struct Cyc_Absyn_Datatypedecl***
Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1 _tmp6A;union Cyc_Absyn_Nmspace
_tmp6B;struct _dyneither_ptr*_tmp6C;struct _tuple1*_tmp69=q;_tmp6A=*_tmp69;_tmp6B=
_tmp6A.f1;_tmp6C=_tmp6A.f2;{union Cyc_Absyn_Nmspace _tmp6D=_tmp6B;struct Cyc_List_List*
_tmp6E;int _tmp6F;struct Cyc_List_List*_tmp70;struct Cyc_List_List _tmp71;struct
_dyneither_ptr*_tmp72;struct Cyc_List_List*_tmp73;struct Cyc_List_List*_tmp74;
_LL2B: if((_tmp6D.Rel_n).tag != 1)goto _LL2D;_tmp6E=(struct Cyc_List_List*)(_tmp6D.Rel_n).val;
if(_tmp6E != 0)goto _LL2D;_LL2C: {struct _handler_cons _tmp75;_push_handler(& _tmp75);{
int _tmp77=0;if(setjmp(_tmp75.handler))_tmp77=1;if(!_tmp77){{struct Cyc_Absyn_Datatypedecl***
_tmp335;struct Cyc_Absyn_Datatypedecl***_tmp79=(_tmp335=_region_malloc(r,sizeof(*
_tmp335)),((_tmp335[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct
Cyc_Tcenv_Genv*,struct _dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp6C),_tmp335)));_npop_handler(0);
return _tmp79;};_pop_handler();}else{void*_tmp76=(void*)_exn_thrown;void*_tmp7B=
_tmp76;_LL34: {struct Cyc_Dict_Absent_struct*_tmp7C=(struct Cyc_Dict_Absent_struct*)
_tmp7B;if(_tmp7C->tag != Cyc_Dict_Absent)goto _LL36;}_LL35: return 0;_LL36:;_LL37:(
void)_throw(_tmp7B);_LL33:;}}}_LL2D: if((_tmp6D.Loc_n).tag != 3)goto _LL2F;_tmp6F=(
int)(_tmp6D.Loc_n).val;_LL2E:{const char*_tmp338;void*_tmp337;(_tmp337=0,Cyc_Tcutil_terr(
loc,((_tmp338="lookup_xdatatypedecl: impossible",_tag_dyneither(_tmp338,sizeof(
char),33))),_tag_dyneither(_tmp337,sizeof(void*),0)));}return 0;_LL2F: if((_tmp6D.Rel_n).tag
!= 1)goto _LL31;_tmp70=(struct Cyc_List_List*)(_tmp6D.Rel_n).val;if(_tmp70 == 0)
goto _LL31;_tmp71=*_tmp70;_tmp72=(struct _dyneither_ptr*)_tmp71.hd;_tmp73=_tmp71.tl;
_LL30: {struct Cyc_Tcenv_Genv*ge;{struct _handler_cons _tmp7F;_push_handler(& _tmp7F);{
int _tmp81=0;if(setjmp(_tmp7F.handler))_tmp81=1;if(!_tmp81){ge=Cyc_Tcenv_lookup_namespace(
te,loc,_tmp72,_tmp73);;_pop_handler();}else{void*_tmp80=(void*)_exn_thrown;void*
_tmp83=_tmp80;_LL39: {struct Cyc_Dict_Absent_struct*_tmp84=(struct Cyc_Dict_Absent_struct*)
_tmp83;if(_tmp84->tag != Cyc_Dict_Absent)goto _LL3B;}_LL3A:{const char*_tmp33B;void*
_tmp33A;(_tmp33A=0,Cyc_Tcutil_terr(loc,((_tmp33B="bad qualified name for @extensible datatype",
_tag_dyneither(_tmp33B,sizeof(char),44))),_tag_dyneither(_tmp33A,sizeof(void*),0)));}{
struct Cyc_Dict_Absent_struct _tmp33E;struct Cyc_Dict_Absent_struct*_tmp33D;(int)
_throw((void*)((_tmp33D=_cycalloc_atomic(sizeof(*_tmp33D)),((_tmp33D[0]=((
_tmp33E.tag=Cyc_Dict_Absent,_tmp33E)),_tmp33D)))));}_LL3B:;_LL3C:(void)_throw(
_tmp83);_LL38:;}}}{struct Cyc_Dict_Dict _tmp89=ge->datatypedecls;struct Cyc_Absyn_Datatypedecl***
_tmp33F;return(_tmp33F=_region_malloc(r,sizeof(*_tmp33F)),((_tmp33F[0]=((struct
Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp89,_tmp6C),_tmp33F)));}}_LL31: if((_tmp6D.Abs_n).tag != 2)goto _LL2A;_tmp74=(
struct Cyc_List_List*)(_tmp6D.Abs_n).val;_LL32: {struct Cyc_Dict_Dict _tmp8B=(((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp74))->datatypedecls;struct Cyc_Absyn_Datatypedecl***_tmp340;return(
_tmp340=_region_malloc(r,sizeof(*_tmp340)),((_tmp340[0]=((struct Cyc_Absyn_Datatypedecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8B,_tmp6C),
_tmp340)));}_LL2A:;}}static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v);static struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){
struct Cyc_Dict_Dict _tmp8D=ge->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8D,v);}struct
Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1
_tmp8F;union Cyc_Absyn_Nmspace _tmp90;struct _dyneither_ptr*_tmp91;struct _tuple1*
_tmp8E=q;_tmp8F=*_tmp8E;_tmp90=_tmp8F.f1;_tmp91=_tmp8F.f2;{union Cyc_Absyn_Nmspace
_tmp92=_tmp90;int _tmp93;struct Cyc_List_List*_tmp94;struct Cyc_List_List*_tmp95;
struct Cyc_List_List _tmp96;struct _dyneither_ptr*_tmp97;struct Cyc_List_List*_tmp98;
struct Cyc_List_List*_tmp99;_LL3E: if((_tmp92.Loc_n).tag != 3)goto _LL40;_tmp93=(int)(
_tmp92.Loc_n).val;_LL3F: goto _LL41;_LL40: if((_tmp92.Rel_n).tag != 1)goto _LL42;
_tmp94=(struct Cyc_List_List*)(_tmp92.Rel_n).val;if(_tmp94 != 0)goto _LL42;_LL41:
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,
_tmp91);_LL42: if((_tmp92.Rel_n).tag != 1)goto _LL44;_tmp95=(struct Cyc_List_List*)(
_tmp92.Rel_n).val;if(_tmp95 == 0)goto _LL44;_tmp96=*_tmp95;_tmp97=(struct
_dyneither_ptr*)_tmp96.hd;_tmp98=_tmp96.tl;_LL43: {struct Cyc_Dict_Dict _tmp9A=(
Cyc_Tcenv_lookup_namespace(te,loc,_tmp97,_tmp98))->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp9A,_tmp91);}
_LL44: if((_tmp92.Abs_n).tag != 2)goto _LL3D;_tmp99=(struct Cyc_List_List*)(_tmp92.Abs_n).val;
_LL45: {struct Cyc_Dict_Dict _tmp9B=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp99))->enumdecls;return((
struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmp9B,_tmp91);}_LL3D:;}}static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v);static struct Cyc_Absyn_Typedefdecl*
Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v){
struct Cyc_Dict_Dict _tmp9C=ge->typedefs;return((struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp9C,v);}struct
Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp9E;union Cyc_Absyn_Nmspace _tmp9F;struct _dyneither_ptr*_tmpA0;struct
_tuple1*_tmp9D=q;_tmp9E=*_tmp9D;_tmp9F=_tmp9E.f1;_tmpA0=_tmp9E.f2;{union Cyc_Absyn_Nmspace
_tmpA1=_tmp9F;int _tmpA2;struct Cyc_List_List*_tmpA3;struct Cyc_List_List*_tmpA4;
struct Cyc_List_List _tmpA5;struct _dyneither_ptr*_tmpA6;struct Cyc_List_List*_tmpA7;
struct Cyc_List_List*_tmpA8;_LL47: if((_tmpA1.Loc_n).tag != 3)goto _LL49;_tmpA2=(int)(
_tmpA1.Loc_n).val;_LL48: goto _LL4A;_LL49: if((_tmpA1.Rel_n).tag != 1)goto _LL4B;
_tmpA3=(struct Cyc_List_List*)(_tmpA1.Rel_n).val;if(_tmpA3 != 0)goto _LL4B;_LL4A:
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dyneither_ptr*),struct _dyneither_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,
_tmpA0);_LL4B: if((_tmpA1.Rel_n).tag != 1)goto _LL4D;_tmpA4=(struct Cyc_List_List*)(
_tmpA1.Rel_n).val;if(_tmpA4 == 0)goto _LL4D;_tmpA5=*_tmpA4;_tmpA6=(struct
_dyneither_ptr*)_tmpA5.hd;_tmpA7=_tmpA5.tl;_LL4C: {struct Cyc_Dict_Dict _tmpA9=(
Cyc_Tcenv_lookup_namespace(te,loc,_tmpA6,_tmpA7))->typedefs;return((struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmpA9,_tmpA0);}
_LL4D: if((_tmpA1.Abs_n).tag != 2)goto _LL46;_tmpA8=(struct Cyc_List_List*)(_tmpA1.Abs_n).val;
_LL4E: {struct Cyc_Dict_Dict _tmpAA=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmpA8))->typedefs;return((
struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(
_tmpAA,_tmpA0);}_LL46:;}}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*
te,struct _dyneither_ptr err_msg);static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(
struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){struct Cyc_Tcenv_Fenv*_tmpAB=
te->le;if((struct Cyc_Tcenv_Tenv*)te == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(
err_msg);return(struct Cyc_Tcenv_Fenv*)_check_null(_tmpAB);}struct _RegionHandle*
Cyc_Tcenv_coerce_heap_region();struct _RegionHandle*Cyc_Tcenv_coerce_heap_region(){
return(struct _RegionHandle*)Cyc_Core_heap_region;}struct _RegionHandle*Cyc_Tcenv_get_fnrgn(
struct Cyc_Tcenv_Tenv*te);struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv*_tmpAC=te->le;if(_tmpAC != 0){struct Cyc_Tcenv_Fenv _tmpAE;
struct _RegionHandle*_tmpAF;struct Cyc_Tcenv_Fenv*_tmpAD=(struct Cyc_Tcenv_Fenv*)
_tmpAC;_tmpAE=*_tmpAD;_tmpAF=_tmpAE.fnrgn;return _tmpAF;}return Cyc_Tcenv_coerce_heap_region();}
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Fenv*fe);static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct
_RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){if(te->le == 0){
const char*_tmp341;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp341="put_fenv",
_tag_dyneither(_tmp341,sizeof(char),9))));}{struct Cyc_Tcenv_Tenv*_tmp342;return(
_tmp342=_region_malloc(l,sizeof(*_tmp342)),((_tmp342->ns=te->ns,((_tmp342->ae=te->ae,((
_tmp342->le=(struct Cyc_Tcenv_Fenv*)fe,((_tmp342->allow_valueof=te->allow_valueof,
_tmp342)))))))));}}static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct
_RegionHandle*l,struct Cyc_Tcenv_Tenv*te);static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(
struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_tmp343;
return(_tmp343=_region_malloc(l,sizeof(*_tmp343)),((_tmp343->ns=te->ns,((_tmp343->ae=
te->ae,((_tmp343->le=0,((_tmp343->allow_valueof=te->allow_valueof,_tmp343)))))))));}
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,
struct Cyc_Tcenv_Fenv*f);static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmpB4;
struct Cyc_List_List*_tmpB5;struct Cyc_RgnOrder_RgnPO*_tmpB6;struct Cyc_Tcenv_Bindings*
_tmpB7;struct Cyc_Absyn_Stmt*_tmpB8;struct Cyc_Tcenv_CtrlEnv*_tmpB9;void*_tmpBA;
void*_tmpBB;int _tmpBC;struct _RegionHandle*_tmpBD;struct Cyc_Tcenv_Fenv _tmpB3=*f;
_tmpB4=_tmpB3.shared;_tmpB5=_tmpB3.type_vars;_tmpB6=_tmpB3.region_order;_tmpB7=
_tmpB3.locals;_tmpB8=_tmpB3.encloser;_tmpB9=_tmpB3.ctrl_env;_tmpBA=_tmpB3.capability;
_tmpBB=_tmpB3.curr_rgn;_tmpBC=_tmpB3.in_notreadctxt;_tmpBD=_tmpB3.fnrgn;{struct
Cyc_Tcenv_Fenv*_tmp344;return(_tmp344=_region_malloc(l,sizeof(*_tmp344)),((
_tmp344->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpB4,((_tmp344->type_vars=(struct
Cyc_List_List*)_tmpB5,((_tmp344->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpB6,((
_tmp344->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmpB7),((
_tmp344->encloser=(struct Cyc_Absyn_Stmt*)_tmpB8,((_tmp344->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmpB9),((_tmp344->capability=(void*)_tmpBA,((_tmp344->curr_rgn=(
void*)_tmpBB,((_tmp344->in_notreadctxt=(int)_tmpBC,((_tmp344->fnrgn=(struct
_RegionHandle*)l,_tmp344)))))))))))))))))))));}}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f);static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmpC0;
struct Cyc_List_List*_tmpC1;struct Cyc_RgnOrder_RgnPO*_tmpC2;struct Cyc_Tcenv_Bindings*
_tmpC3;struct Cyc_Absyn_Stmt*_tmpC4;struct Cyc_Tcenv_CtrlEnv*_tmpC5;void*_tmpC6;
void*_tmpC7;int _tmpC8;struct Cyc_Tcenv_Fenv _tmpBF=*f;_tmpC0=_tmpBF.shared;_tmpC1=
_tmpBF.type_vars;_tmpC2=_tmpBF.region_order;_tmpC3=_tmpBF.locals;_tmpC4=_tmpBF.encloser;
_tmpC5=_tmpBF.ctrl_env;_tmpC6=_tmpBF.capability;_tmpC7=_tmpBF.curr_rgn;_tmpC8=
_tmpBF.in_notreadctxt;{struct _RegionHandle*_tmpCA;void*_tmpCB;void*_tmpCC;struct
_tuple10*_tmpCD;void*_tmpCE;int _tmpCF;struct Cyc_Tcenv_CtrlEnv _tmpC9=*_tmpC5;
_tmpCA=_tmpC9.ctrl_rgn;_tmpCB=_tmpC9.continue_stmt;_tmpCC=_tmpC9.break_stmt;
_tmpCD=_tmpC9.fallthru_clause;_tmpCE=_tmpC9.next_stmt;_tmpCF=_tmpC9.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmp345;struct Cyc_Tcenv_CtrlEnv*_tmpD0=(_tmp345=
_region_malloc(l,sizeof(*_tmp345)),((_tmp345->ctrl_rgn=_tmpCA,((_tmp345->continue_stmt=
_tmpCB,((_tmp345->break_stmt=_tmpCC,((_tmp345->fallthru_clause=_tmpCD,((_tmp345->next_stmt=
_tmpCE,((_tmp345->try_depth=_tmpCF,_tmp345)))))))))))));struct Cyc_Tcenv_Fenv*
_tmp346;return(_tmp346=_region_malloc(l,sizeof(*_tmp346)),((_tmp346->shared=(
struct Cyc_Tcenv_SharedFenv*)_tmpC0,((_tmp346->type_vars=(struct Cyc_List_List*)
_tmpC1,((_tmp346->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpC2,((_tmp346->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmpC3),((_tmp346->encloser=(
struct Cyc_Absyn_Stmt*)_tmpC4,((_tmp346->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)
_tmpD0,((_tmp346->capability=(void*)_tmpC6,((_tmp346->curr_rgn=(void*)_tmpC7,((
_tmp346->in_notreadctxt=(int)_tmpC8,((_tmp346->fnrgn=(struct _RegionHandle*)l,
_tmp346)))))))))))))))))))));}}}void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*
te);void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv
_tmpD5;struct Cyc_Tcenv_SharedFenv*_tmpD6;const char*_tmp347;struct Cyc_Tcenv_Fenv*
_tmpD4=Cyc_Tcenv_get_fenv(te,((_tmp347="return_typ",_tag_dyneither(_tmp347,
sizeof(char),11))));_tmpD5=*_tmpD4;_tmpD6=_tmpD5.shared;{void*_tmpD8;struct Cyc_Tcenv_SharedFenv
_tmpD7=*_tmpD6;_tmpD8=_tmpD7.return_typ;return _tmpD8;}}struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*te);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct
Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpD9=te->le;if(te->le == 0)return 0;{
struct Cyc_List_List*_tmpDB;struct Cyc_Tcenv_Fenv _tmpDA=*((struct Cyc_Tcenv_Fenv*)
_check_null(_tmpD9));_tmpDB=_tmpDA.type_vars;return _tmpDB;}}struct Cyc_Core_Opt*
Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Core_Opt*_tmp348;return(_tmp348=_cycalloc(
sizeof(*_tmp348)),((_tmp348->v=Cyc_Tcenv_lookup_type_vars(te),_tmp348)));}struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*tvs){struct Cyc_Tcenv_Fenv _tmpDF;struct Cyc_Tcenv_SharedFenv*
_tmpE0;struct Cyc_List_List*_tmpE1;struct Cyc_RgnOrder_RgnPO*_tmpE2;struct Cyc_Tcenv_Bindings*
_tmpE3;struct Cyc_Absyn_Stmt*_tmpE4;struct Cyc_Tcenv_CtrlEnv*_tmpE5;void*_tmpE6;
void*_tmpE7;int _tmpE8;const char*_tmp349;struct Cyc_Tcenv_Fenv*_tmpDE=Cyc_Tcenv_get_fenv(
te,((_tmp349="add_type_vars",_tag_dyneither(_tmp349,sizeof(char),14))));_tmpDF=*
_tmpDE;_tmpE0=_tmpDF.shared;_tmpE1=_tmpDF.type_vars;_tmpE2=_tmpDF.region_order;
_tmpE3=_tmpDF.locals;_tmpE4=_tmpDF.encloser;_tmpE5=_tmpDF.ctrl_env;_tmpE6=_tmpDF.capability;
_tmpE7=_tmpDF.curr_rgn;_tmpE8=_tmpDF.in_notreadctxt;Cyc_Tcutil_add_tvar_identities(
tvs);{struct Cyc_List_List*_tmpE9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpE1);Cyc_Tcutil_check_unique_tvars(
loc,_tmpE9);{struct Cyc_Tcenv_Fenv*_tmp34A;struct Cyc_Tcenv_Fenv*_tmpEA=(_tmp34A=
_region_malloc(r,sizeof(*_tmp34A)),((_tmp34A->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpE0,((_tmp34A->type_vars=(struct Cyc_List_List*)_tmpE9,((_tmp34A->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpE2,((_tmp34A->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmpE3),((_tmp34A->encloser=(struct Cyc_Absyn_Stmt*)
_tmpE4,((_tmp34A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmpE5),((_tmp34A->capability=(void*)_tmpE6,((_tmp34A->curr_rgn=(void*)_tmpE7,((
_tmp34A->in_notreadctxt=(int)_tmpE8,((_tmp34A->fnrgn=(struct _RegionHandle*)r,
_tmp34A)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,_tmpEA);}}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*
_tmpEC=te->le;if(_tmpEC == 0)return Cyc_Tcenv_put_emptyfenv(r,te);else{struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,(struct Cyc_Tcenv_Fenv*)_tmpEC);return Cyc_Tcenv_put_fenv(
r,te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*
vd);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){struct _dyneither_ptr*
_tmpED=(*vd->name).f2;{union Cyc_Absyn_Nmspace _tmpEE=(*vd->name).f1;int _tmpEF;
_LL50: if((_tmpEE.Loc_n).tag != 3)goto _LL52;_tmpEF=(int)(_tmpEE.Loc_n).val;_LL51:
goto _LL4F;_LL52:;_LL53: {struct Cyc_Core_Impossible_struct _tmp350;const char*
_tmp34F;struct Cyc_Core_Impossible_struct*_tmp34E;(int)_throw((void*)((_tmp34E=
_cycalloc(sizeof(*_tmp34E)),((_tmp34E[0]=((_tmp350.tag=Cyc_Core_Impossible,((
_tmp350.f1=((_tmp34F="add_local_var: called with Rel_n",_tag_dyneither(_tmp34F,
sizeof(char),33))),_tmp350)))),_tmp34E)))));}_LL4F:;}{struct Cyc_Tcenv_Fenv _tmpF5;
struct Cyc_Tcenv_SharedFenv*_tmpF6;struct Cyc_List_List*_tmpF7;struct Cyc_RgnOrder_RgnPO*
_tmpF8;struct Cyc_Tcenv_Bindings*_tmpF9;struct Cyc_Absyn_Stmt*_tmpFA;struct Cyc_Tcenv_CtrlEnv*
_tmpFB;void*_tmpFC;void*_tmpFD;int _tmpFE;const char*_tmp351;struct Cyc_Tcenv_Fenv*
_tmpF4=Cyc_Tcenv_get_fenv(te,((_tmp351="add_local_var",_tag_dyneither(_tmp351,
sizeof(char),14))));_tmpF5=*_tmpF4;_tmpF6=_tmpF5.shared;_tmpF7=_tmpF5.type_vars;
_tmpF8=_tmpF5.region_order;_tmpF9=_tmpF5.locals;_tmpFA=_tmpF5.encloser;_tmpFB=
_tmpF5.ctrl_env;_tmpFC=_tmpF5.capability;_tmpFD=_tmpF5.curr_rgn;_tmpFE=_tmpF5.in_notreadctxt;{
struct Cyc_Absyn_Local_b_struct*_tmp357;struct Cyc_Absyn_Local_b_struct _tmp356;
struct Cyc_Tcenv_Bindings*_tmp355;struct Cyc_Tcenv_Bindings*_tmpFF=(_tmp355=
_region_malloc(r,sizeof(*_tmp355)),((_tmp355->v=_tmpED,((_tmp355->b=(void*)((
_tmp357=_cycalloc(sizeof(*_tmp357)),((_tmp357[0]=((_tmp356.tag=4,((_tmp356.f1=vd,
_tmp356)))),_tmp357)))),((_tmp355->tl=(struct Cyc_Tcenv_Bindings*)_tmpF9,_tmp355)))))));
struct Cyc_Tcenv_Fenv*_tmp358;struct Cyc_Tcenv_Fenv*_tmp100=(_tmp358=
_region_malloc(r,sizeof(*_tmp358)),((_tmp358->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpF6,((_tmp358->type_vars=(struct Cyc_List_List*)_tmpF7,((_tmp358->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpF8,((_tmp358->locals=(struct Cyc_Tcenv_Bindings*)
_tmpFF,((_tmp358->encloser=(struct Cyc_Absyn_Stmt*)_tmpFA,((_tmp358->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmpFB),((_tmp358->capability=(
void*)_tmpFC,((_tmp358->curr_rgn=(void*)_tmpFD,((_tmp358->in_notreadctxt=(int)
_tmpFE,((_tmp358->fnrgn=(struct _RegionHandle*)r,_tmp358)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp100);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp105=te->le;
if(_tmp105 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*
_tmp107;struct Cyc_List_List*_tmp108;struct Cyc_RgnOrder_RgnPO*_tmp109;struct Cyc_Tcenv_Bindings*
_tmp10A;struct Cyc_Absyn_Stmt*_tmp10B;struct Cyc_Tcenv_CtrlEnv*_tmp10C;void*
_tmp10D;void*_tmp10E;int _tmp10F;struct Cyc_Tcenv_Fenv _tmp106=*_tmp105;_tmp107=
_tmp106.shared;_tmp108=_tmp106.type_vars;_tmp109=_tmp106.region_order;_tmp10A=
_tmp106.locals;_tmp10B=_tmp106.encloser;_tmp10C=_tmp106.ctrl_env;_tmp10D=_tmp106.capability;
_tmp10E=_tmp106.curr_rgn;_tmp10F=_tmp106.in_notreadctxt;{struct Cyc_Tcenv_Fenv*
_tmp359;struct Cyc_Tcenv_Fenv*_tmp110=(_tmp359=_region_malloc(r,sizeof(*_tmp359)),((
_tmp359->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp107,((_tmp359->type_vars=(
struct Cyc_List_List*)_tmp108,((_tmp359->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp109,((_tmp359->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp10A),((_tmp359->encloser=(struct Cyc_Absyn_Stmt*)_tmp10B,((_tmp359->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp10C),((_tmp359->capability=(
void*)_tmp10D,((_tmp359->curr_rgn=(void*)_tmp10E,((_tmp359->in_notreadctxt=(int)
1,((_tmp359->fnrgn=(struct _RegionHandle*)r,_tmp359)))))))))))))))))))));return
Cyc_Tcenv_put_fenv(r,te,_tmp110);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmp112=te->le;
if(_tmp112 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*
_tmp114;struct Cyc_List_List*_tmp115;struct Cyc_RgnOrder_RgnPO*_tmp116;struct Cyc_Tcenv_Bindings*
_tmp117;struct Cyc_Absyn_Stmt*_tmp118;struct Cyc_Tcenv_CtrlEnv*_tmp119;void*
_tmp11A;void*_tmp11B;int _tmp11C;struct Cyc_Tcenv_Fenv _tmp113=*_tmp112;_tmp114=
_tmp113.shared;_tmp115=_tmp113.type_vars;_tmp116=_tmp113.region_order;_tmp117=
_tmp113.locals;_tmp118=_tmp113.encloser;_tmp119=_tmp113.ctrl_env;_tmp11A=_tmp113.capability;
_tmp11B=_tmp113.curr_rgn;_tmp11C=_tmp113.in_notreadctxt;{struct Cyc_Tcenv_Fenv*
_tmp35A;struct Cyc_Tcenv_Fenv*_tmp11D=(_tmp35A=_region_malloc(r,sizeof(*_tmp35A)),((
_tmp35A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp114,((_tmp35A->type_vars=(
struct Cyc_List_List*)_tmp115,((_tmp35A->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp116,((_tmp35A->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp117),((_tmp35A->encloser=(struct Cyc_Absyn_Stmt*)_tmp118,((_tmp35A->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp119),((_tmp35A->capability=(
void*)_tmp11A,((_tmp35A->curr_rgn=(void*)_tmp11B,((_tmp35A->in_notreadctxt=(int)
0,((_tmp35A->fnrgn=(struct _RegionHandle*)r,_tmp35A)))))))))))))))))))));return
Cyc_Tcenv_put_fenv(r,te,_tmp11D);}}}int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*
te);int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*
_tmp11F=te->le;if(_tmp11F == 0)return 0;{struct Cyc_Tcenv_Fenv _tmp121;int _tmp122;
struct Cyc_Tcenv_Fenv*_tmp120=(struct Cyc_Tcenv_Fenv*)_tmp11F;_tmp121=*_tmp120;
_tmp122=_tmp121.in_notreadctxt;return _tmp122;}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Vardecl*vd);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*
vd){struct _dyneither_ptr*_tmp123=(*vd->name).f2;struct Cyc_Tcenv_Fenv _tmp126;
struct Cyc_Tcenv_SharedFenv*_tmp127;struct Cyc_List_List*_tmp128;struct Cyc_RgnOrder_RgnPO*
_tmp129;struct Cyc_Tcenv_Bindings*_tmp12A;struct Cyc_Absyn_Stmt*_tmp12B;struct Cyc_Tcenv_CtrlEnv*
_tmp12C;void*_tmp12D;void*_tmp12E;int _tmp12F;const char*_tmp35B;struct Cyc_Tcenv_Fenv*
_tmp125=Cyc_Tcenv_get_fenv(te,((_tmp35B="add_pat_var",_tag_dyneither(_tmp35B,
sizeof(char),12))));_tmp126=*_tmp125;_tmp127=_tmp126.shared;_tmp128=_tmp126.type_vars;
_tmp129=_tmp126.region_order;_tmp12A=_tmp126.locals;_tmp12B=_tmp126.encloser;
_tmp12C=_tmp126.ctrl_env;_tmp12D=_tmp126.capability;_tmp12E=_tmp126.curr_rgn;
_tmp12F=_tmp126.in_notreadctxt;{struct Cyc_Absyn_Pat_b_struct*_tmp361;struct Cyc_Absyn_Pat_b_struct
_tmp360;struct Cyc_Tcenv_Bindings*_tmp35F;struct Cyc_Tcenv_Bindings*_tmp130=(
_tmp35F=_region_malloc(r,sizeof(*_tmp35F)),((_tmp35F->v=_tmp123,((_tmp35F->b=(
void*)((_tmp361=_cycalloc(sizeof(*_tmp361)),((_tmp361[0]=((_tmp360.tag=5,((
_tmp360.f1=vd,_tmp360)))),_tmp361)))),((_tmp35F->tl=(struct Cyc_Tcenv_Bindings*)
_tmp12A,_tmp35F)))))));struct Cyc_Tcenv_Fenv*_tmp362;struct Cyc_Tcenv_Fenv*_tmp131=(
_tmp362=_region_malloc(r,sizeof(*_tmp362)),((_tmp362->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp127,((_tmp362->type_vars=(struct Cyc_List_List*)_tmp128,((_tmp362->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp129,((_tmp362->locals=(struct Cyc_Tcenv_Bindings*)
_tmp130,((_tmp362->encloser=(struct Cyc_Absyn_Stmt*)_tmp12B,((_tmp362->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp12C),((_tmp362->capability=(
void*)_tmp12D,((_tmp362->curr_rgn=(void*)_tmp12E,((_tmp362->in_notreadctxt=(int)
_tmp12F,((_tmp362->fnrgn=(struct _RegionHandle*)r,_tmp362)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp131);}}void*Cyc_Tcenv_lookup_ordinary(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct
_tuple1*q);void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct Cyc_Tcenv_Fenv*_tmp136=
te->le;struct _tuple1 _tmp138;union Cyc_Absyn_Nmspace _tmp139;struct _dyneither_ptr*
_tmp13A;struct _tuple1*_tmp137=q;_tmp138=*_tmp137;_tmp139=_tmp138.f1;_tmp13A=
_tmp138.f2;{union Cyc_Absyn_Nmspace _tmp13B=_tmp139;int _tmp13C;struct Cyc_List_List*
_tmp13D;_LL55: if((_tmp13B.Loc_n).tag != 3)goto _LL57;_tmp13C=(int)(_tmp13B.Loc_n).val;
_LL56: if(_tmp136 == 0){struct Cyc_Dict_Absent_struct _tmp365;struct Cyc_Dict_Absent_struct*
_tmp364;(int)_throw((void*)((_tmp364=_cycalloc_atomic(sizeof(*_tmp364)),((
_tmp364[0]=((_tmp365.tag=Cyc_Dict_Absent,_tmp365)),_tmp364)))));}goto _LL58;_LL57:
if((_tmp13B.Rel_n).tag != 1)goto _LL59;_tmp13D=(struct Cyc_List_List*)(_tmp13B.Rel_n).val;
if(_tmp13D != 0)goto _LL59;if(!(_tmp136 != 0))goto _LL59;_LL58: {struct Cyc_Tcenv_Fenv
_tmp141;struct Cyc_Tcenv_Bindings*_tmp142;struct Cyc_Tcenv_Fenv*_tmp140=(struct Cyc_Tcenv_Fenv*)
_tmp136;_tmp141=*_tmp140;_tmp142=_tmp141.locals;{struct _handler_cons _tmp143;
_push_handler(& _tmp143);{int _tmp145=0;if(setjmp(_tmp143.handler))_tmp145=1;if(!
_tmp145){{struct Cyc_Tcenv_VarRes_struct _tmp368;struct Cyc_Tcenv_VarRes_struct*
_tmp367;void*_tmp148=(void*)((_tmp367=_region_malloc(r,sizeof(*_tmp367)),((
_tmp367[0]=((_tmp368.tag=0,((_tmp368.f1=(void*)Cyc_Tcenv_lookup_binding(_tmp142,
_tmp13A),_tmp368)))),_tmp367))));_npop_handler(0);return _tmp148;};_pop_handler();}
else{void*_tmp144=(void*)_exn_thrown;void*_tmp14A=_tmp144;_LL5C: {struct Cyc_Tcenv_NoBinding_struct*
_tmp14B=(struct Cyc_Tcenv_NoBinding_struct*)_tmp14A;if(_tmp14B->tag != Cyc_Tcenv_NoBinding)
goto _LL5E;}_LL5D: return(void*)Cyc_Tcenv_lookup_ordinary_global(te,loc,q);_LL5E:;
_LL5F:(void)_throw(_tmp14A);_LL5B:;}}}}_LL59:;_LL5A: {struct _handler_cons _tmp14C;
_push_handler(& _tmp14C);{int _tmp14E=0;if(setjmp(_tmp14C.handler))_tmp14E=1;if(!
_tmp14E){{void*_tmp14F=(void*)Cyc_Tcenv_lookup_ordinary_global(te,loc,q);
_npop_handler(0);return _tmp14F;};_pop_handler();}else{void*_tmp14D=(void*)
_exn_thrown;void*_tmp151=_tmp14D;_LL61: {struct Cyc_Dict_Absent_struct*_tmp152=(
struct Cyc_Dict_Absent_struct*)_tmp151;if(_tmp152->tag != Cyc_Dict_Absent)goto
_LL63;}_LL62: {struct Cyc_Tcenv_VarRes_struct _tmp36B;struct Cyc_Tcenv_VarRes_struct*
_tmp36A;return(void*)((_tmp36A=_region_malloc(r,sizeof(*_tmp36A)),((_tmp36A[0]=((
_tmp36B.tag=0,((_tmp36B.f1=(void*)((void*)& Cyc_Absyn_Unresolved_b_val),_tmp36B)))),
_tmp36A))));}_LL63:;_LL64:(void)_throw(_tmp151);_LL60:;}}}_LL54:;}}void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt);void
Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct
Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv _tmp157;struct Cyc_Tcenv_CtrlEnv*
_tmp158;const char*_tmp36C;struct Cyc_Tcenv_Fenv*_tmp156=Cyc_Tcenv_get_fenv(te,((
_tmp36C="process_continue",_tag_dyneither(_tmp36C,sizeof(char),17))));_tmp157=*
_tmp156;_tmp158=_tmp157.ctrl_env;{void*_tmp159=_tmp158->continue_stmt;struct Cyc_Absyn_Stmt*
_tmp15B;_LL66: {struct Cyc_Tcenv_Stmt_j_struct*_tmp15A=(struct Cyc_Tcenv_Stmt_j_struct*)
_tmp159;if(_tmp15A->tag != 3)goto _LL68;else{_tmp15B=_tmp15A->f1;}}_LL67:{struct
Cyc_List_List*_tmp36D;_tmp15B->non_local_preds=((_tmp36D=_cycalloc(sizeof(*
_tmp36D)),((_tmp36D->hd=s,((_tmp36D->tl=_tmp15B->non_local_preds,_tmp36D))))));}*
sopt=(struct Cyc_Absyn_Stmt*)_tmp15B;return;_LL68: {struct Cyc_Tcenv_NotLoop_j_struct*
_tmp15C=(struct Cyc_Tcenv_NotLoop_j_struct*)_tmp159;if(_tmp15C->tag != 0)goto _LL6A;}
_LL69:{const char*_tmp370;void*_tmp36F;(_tmp36F=0,Cyc_Tcutil_terr(s->loc,((
_tmp370="continue not in a loop",_tag_dyneither(_tmp370,sizeof(char),23))),
_tag_dyneither(_tmp36F,sizeof(void*),0)));}return;_LL6A: {struct Cyc_Tcenv_CaseEnd_j_struct*
_tmp15D=(struct Cyc_Tcenv_CaseEnd_j_struct*)_tmp159;if(_tmp15D->tag != 1)goto _LL6C;}
_LL6B: goto _LL6D;_LL6C: {struct Cyc_Tcenv_FnEnd_j_struct*_tmp15E=(struct Cyc_Tcenv_FnEnd_j_struct*)
_tmp159;if(_tmp15E->tag != 2)goto _LL65;}_LL6D: {const char*_tmp371;((int(*)(struct
_dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp371="bad continue destination",
_tag_dyneither(_tmp371,sizeof(char),25))));}_LL65:;}}void Cyc_Tcenv_process_break(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt);void
Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**
sopt){struct Cyc_Tcenv_Fenv _tmp165;struct Cyc_Tcenv_CtrlEnv*_tmp166;struct Cyc_Tcenv_SharedFenv*
_tmp167;const char*_tmp372;struct Cyc_Tcenv_Fenv*_tmp164=Cyc_Tcenv_get_fenv(te,((
_tmp372="process_break",_tag_dyneither(_tmp372,sizeof(char),14))));_tmp165=*
_tmp164;_tmp166=_tmp165.ctrl_env;_tmp167=_tmp165.shared;{void*_tmp168=_tmp166->break_stmt;
struct Cyc_Absyn_Stmt*_tmp16A;_LL6F: {struct Cyc_Tcenv_Stmt_j_struct*_tmp169=(
struct Cyc_Tcenv_Stmt_j_struct*)_tmp168;if(_tmp169->tag != 3)goto _LL71;else{
_tmp16A=_tmp169->f1;}}_LL70:{struct Cyc_List_List*_tmp373;_tmp16A->non_local_preds=((
_tmp373=_cycalloc(sizeof(*_tmp373)),((_tmp373->hd=s,((_tmp373->tl=_tmp16A->non_local_preds,
_tmp373))))));}*sopt=(struct Cyc_Absyn_Stmt*)_tmp16A;return;_LL71: {struct Cyc_Tcenv_NotLoop_j_struct*
_tmp16B=(struct Cyc_Tcenv_NotLoop_j_struct*)_tmp168;if(_tmp16B->tag != 0)goto _LL73;}
_LL72:{const char*_tmp376;void*_tmp375;(_tmp375=0,Cyc_Tcutil_terr(s->loc,((
_tmp376="break not in a loop or switch",_tag_dyneither(_tmp376,sizeof(char),30))),
_tag_dyneither(_tmp375,sizeof(void*),0)));}return;_LL73: {struct Cyc_Tcenv_FnEnd_j_struct*
_tmp16C=(struct Cyc_Tcenv_FnEnd_j_struct*)_tmp168;if(_tmp16C->tag != 2)goto _LL75;}
_LL74: if(_tmp167->return_typ != (void*)& Cyc_Absyn_VoidType_val){const char*_tmp379;
void*_tmp378;(_tmp378=0,Cyc_Tcutil_terr(s->loc,((_tmp379="break causes function not to return a value",
_tag_dyneither(_tmp379,sizeof(char),44))),_tag_dyneither(_tmp378,sizeof(void*),0)));}
return;_LL75: {struct Cyc_Tcenv_CaseEnd_j_struct*_tmp16D=(struct Cyc_Tcenv_CaseEnd_j_struct*)
_tmp168;if(_tmp16D->tag != 1)goto _LL6E;}_LL76:{const char*_tmp37C;void*_tmp37B;(
_tmp37B=0,Cyc_Tcutil_terr(s->loc,((_tmp37C="break causes outer switch clause to implicitly fallthru",
_tag_dyneither(_tmp37C,sizeof(char),56))),_tag_dyneither(_tmp37B,sizeof(void*),0)));}
return;_LL6E:;}}void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt);void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**
sopt){struct Cyc_Tcenv_Fenv _tmp177;struct Cyc_Tcenv_SharedFenv*_tmp178;const char*
_tmp37D;struct Cyc_Tcenv_Fenv*_tmp176=Cyc_Tcenv_get_fenv(te,((_tmp37D="process_goto",
_tag_dyneither(_tmp37D,sizeof(char),13))));_tmp177=*_tmp176;_tmp178=_tmp177.shared;{
struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp178->seen_labels,l);if(sopt2 == 
0){struct Cyc_Dict_Dict _tmp179=_tmp178->needed_labels;struct Cyc_List_List**slopt=((
struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(
_tmp179,l);struct _RegionHandle*frgn=_tmp178->frgn;if(slopt == 0){struct Cyc_List_List**
_tmp37E;slopt=((_tmp37E=_region_malloc(frgn,sizeof(*_tmp37E)),((_tmp37E[0]=0,
_tmp37E))));}{struct Cyc_List_List*_tmp37F;struct Cyc_List_List*new_needed=(
_tmp37F=_cycalloc(sizeof(*_tmp37F)),((_tmp37F->hd=s,((_tmp37F->tl=*((struct Cyc_List_List**)
_check_null(slopt)),_tmp37F)))));_tmp178->needed_labels=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(
_tmp179,l,new_needed);}}else{struct Cyc_Absyn_Stmt*s=*sopt2;{struct Cyc_List_List*
_tmp380;s->non_local_preds=((_tmp380=_cycalloc(sizeof(*_tmp380)),((_tmp380->hd=s,((
_tmp380->tl=s->non_local_preds,_tmp380))))));}*sopt=(struct Cyc_Absyn_Stmt*)s;}}}
struct _tuple10*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Switch_clause***clauseopt);struct _tuple10*Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***
clauseopt){struct Cyc_Tcenv_Fenv _tmp17F;struct Cyc_Tcenv_CtrlEnv*_tmp180;const char*
_tmp381;struct Cyc_Tcenv_Fenv*_tmp17E=Cyc_Tcenv_get_fenv(te,((_tmp381="process_fallthru",
_tag_dyneither(_tmp381,sizeof(char),17))));_tmp17F=*_tmp17E;_tmp180=_tmp17F.ctrl_env;{
struct _tuple10*_tmp181=(struct _tuple10*)_tmp180->fallthru_clause;if(_tmp181 != 0){{
struct Cyc_List_List*_tmp382;(((*_tmp181).f1)->body)->non_local_preds=((_tmp382=
_cycalloc(sizeof(*_tmp382)),((_tmp382->hd=s,((_tmp382->tl=(((*_tmp181).f1)->body)->non_local_preds,
_tmp382))))));}{struct Cyc_Absyn_Switch_clause**_tmp383;*clauseopt=(struct Cyc_Absyn_Switch_clause**)((
_tmp383=_cycalloc(sizeof(*_tmp383)),((_tmp383[0]=(*_tmp181).f1,_tmp383))));}}
return _tmp181;}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*
r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,
struct Cyc_Absyn_Switch_clause*clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct
Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){struct Cyc_Tcenv_Fenv
_tmp186;struct Cyc_Tcenv_SharedFenv*_tmp187;struct Cyc_List_List*_tmp188;struct Cyc_RgnOrder_RgnPO*
_tmp189;struct Cyc_Tcenv_Bindings*_tmp18A;struct Cyc_Absyn_Stmt*_tmp18B;struct Cyc_Tcenv_CtrlEnv*
_tmp18C;void*_tmp18D;void*_tmp18E;int _tmp18F;const char*_tmp384;struct Cyc_Tcenv_Fenv*
_tmp185=Cyc_Tcenv_get_fenv(te,((_tmp384="set_fallthru",_tag_dyneither(_tmp384,
sizeof(char),13))));_tmp186=*_tmp185;_tmp187=_tmp186.shared;_tmp188=_tmp186.type_vars;
_tmp189=_tmp186.region_order;_tmp18A=_tmp186.locals;_tmp18B=_tmp186.encloser;
_tmp18C=_tmp186.ctrl_env;_tmp18D=_tmp186.capability;_tmp18E=_tmp186.curr_rgn;
_tmp18F=_tmp186.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv _tmp191;struct
_RegionHandle*_tmp192;void*_tmp193;void*_tmp194;struct _tuple10*_tmp195;void*
_tmp196;int _tmp197;struct Cyc_Tcenv_CtrlEnv*_tmp190=_tmp18C;_tmp191=*_tmp190;
_tmp192=_tmp191.ctrl_rgn;_tmp193=_tmp191.continue_stmt;_tmp194=_tmp191.break_stmt;
_tmp195=_tmp191.fallthru_clause;_tmp196=_tmp191.next_stmt;_tmp197=_tmp191.try_depth;{
struct Cyc_List_List*ft_typ=0;for(0;vds != 0;vds=vds->tl){struct Cyc_List_List*
_tmp385;ft_typ=((_tmp385=_region_malloc(_tmp192,sizeof(*_tmp385)),((_tmp385->hd=((
struct Cyc_Absyn_Vardecl*)vds->hd)->type,((_tmp385->tl=ft_typ,_tmp385))))));}{
struct Cyc_Tcenv_CList*_tmp199=(struct Cyc_Tcenv_CList*)((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);struct _tuple10*_tmp388;struct
Cyc_Tcenv_CtrlEnv*_tmp387;struct Cyc_Tcenv_CtrlEnv*_tmp19A=(_tmp387=
_region_malloc(r,sizeof(*_tmp387)),((_tmp387->ctrl_rgn=_tmp192,((_tmp387->continue_stmt=
_tmp193,((_tmp387->break_stmt=_tmp194,((_tmp387->fallthru_clause=((_tmp388=
_region_malloc(_tmp192,sizeof(*_tmp388)),((_tmp388->f1=clause,((_tmp388->f2=
new_tvs,((_tmp388->f3=_tmp199,_tmp388)))))))),((_tmp387->next_stmt=_tmp196,((
_tmp387->try_depth=_tmp197,_tmp387)))))))))))));struct Cyc_Tcenv_Fenv*_tmp389;
struct Cyc_Tcenv_Fenv*_tmp19B=(_tmp389=_region_malloc(r,sizeof(*_tmp389)),((
_tmp389->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp187,((_tmp389->type_vars=(
struct Cyc_List_List*)_tmp188,((_tmp389->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp189,((_tmp389->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp18A),((_tmp389->encloser=(struct Cyc_Absyn_Stmt*)_tmp18B,((_tmp389->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)_tmp19A,((_tmp389->capability=(void*)_tmp18D,((_tmp389->curr_rgn=(
void*)_tmp18E,((_tmp389->in_notreadctxt=(int)_tmp18F,((_tmp389->fnrgn=(struct
_RegionHandle*)r,_tmp389)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,
_tmp19B);}}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te){const char*_tmp38A;struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp38A="clear_fallthru",
_tag_dyneither(_tmp38A,sizeof(char),15)))));struct Cyc_Tcenv_Fenv _tmp1A0;struct
Cyc_Tcenv_CtrlEnv*_tmp1A1;struct Cyc_Tcenv_Fenv*_tmp19F=fe;_tmp1A0=*_tmp19F;
_tmp1A1=_tmp1A0.ctrl_env;_tmp1A1->fallthru_clause=0;return Cyc_Tcenv_put_fenv(r,
te,fe);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
struct Cyc_Tcenv_Fenv _tmp1A5;struct Cyc_Tcenv_SharedFenv*_tmp1A6;struct Cyc_List_List*
_tmp1A7;struct Cyc_RgnOrder_RgnPO*_tmp1A8;struct Cyc_Tcenv_Bindings*_tmp1A9;struct
Cyc_Absyn_Stmt*_tmp1AA;struct Cyc_Tcenv_CtrlEnv*_tmp1AB;void*_tmp1AC;void*_tmp1AD;
int _tmp1AE;const char*_tmp38B;struct Cyc_Tcenv_Fenv*_tmp1A4=Cyc_Tcenv_get_fenv(te,((
_tmp38B="set_in_loop",_tag_dyneither(_tmp38B,sizeof(char),12))));_tmp1A5=*
_tmp1A4;_tmp1A6=_tmp1A5.shared;_tmp1A7=_tmp1A5.type_vars;_tmp1A8=_tmp1A5.region_order;
_tmp1A9=_tmp1A5.locals;_tmp1AA=_tmp1A5.encloser;_tmp1AB=_tmp1A5.ctrl_env;_tmp1AC=
_tmp1A5.capability;_tmp1AD=_tmp1A5.curr_rgn;_tmp1AE=_tmp1A5.in_notreadctxt;{
struct Cyc_Tcenv_Stmt_j_struct*_tmp396;struct Cyc_Tcenv_Stmt_j_struct _tmp395;
struct Cyc_Tcenv_Stmt_j_struct*_tmp394;struct Cyc_Tcenv_Stmt_j_struct _tmp393;
struct Cyc_Tcenv_CtrlEnv*_tmp392;struct Cyc_Tcenv_CtrlEnv*new_cenv=(_tmp392=
_region_malloc(r,sizeof(*_tmp392)),((_tmp392->ctrl_rgn=r,((_tmp392->continue_stmt=(
void*)((_tmp396=_region_malloc(r,sizeof(*_tmp396)),((_tmp396[0]=((_tmp395.tag=3,((
_tmp395.f1=continue_dest,_tmp395)))),_tmp396)))),((_tmp392->break_stmt=(void*)
_tmp1AB->next_stmt,((_tmp392->next_stmt=(void*)((_tmp394=_region_malloc(r,
sizeof(*_tmp394)),((_tmp394[0]=((_tmp393.tag=3,((_tmp393.f1=continue_dest,
_tmp393)))),_tmp394)))),((_tmp392->fallthru_clause=(struct _tuple10*)_tmp1AB->fallthru_clause,((
_tmp392->try_depth=_tmp1AB->try_depth,_tmp392)))))))))))));struct Cyc_Tcenv_Fenv*
_tmp397;struct Cyc_Tcenv_Fenv*new_fenv=(_tmp397=_region_malloc(r,sizeof(*_tmp397)),((
_tmp397->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1A6,((_tmp397->type_vars=(
struct Cyc_List_List*)_tmp1A7,((_tmp397->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp1A8,((_tmp397->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp1A9),((_tmp397->encloser=(struct Cyc_Absyn_Stmt*)_tmp1AA,((_tmp397->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp397->capability=(void*)_tmp1AC,((_tmp397->curr_rgn=(
void*)_tmp1AD,((_tmp397->in_notreadctxt=(int)_tmp1AE,((_tmp397->fnrgn=(struct
_RegionHandle*)r,_tmp397)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,
new_fenv);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te){const char*_tmp398;struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp398="enter_try",
_tag_dyneither(_tmp398,sizeof(char),10)))));struct Cyc_Tcenv_Fenv _tmp1B6;struct
Cyc_Tcenv_CtrlEnv*_tmp1B7;struct Cyc_Tcenv_Fenv*_tmp1B5=fe;_tmp1B6=*_tmp1B5;
_tmp1B7=_tmp1B6.ctrl_env;++ _tmp1B7->try_depth;return Cyc_Tcenv_put_fenv(r,te,fe);}
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp1BB;struct Cyc_Tcenv_CtrlEnv*
_tmp1BC;const char*_tmp399;struct Cyc_Tcenv_Fenv*_tmp1BA=Cyc_Tcenv_get_fenv(te,((
_tmp399="get_try_depth",_tag_dyneither(_tmp399,sizeof(char),14))));_tmp1BB=*
_tmp1BA;_tmp1BC=_tmp1BB.ctrl_env;return _tmp1BC->try_depth;}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){const char*
_tmp39A;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(
te,((_tmp39A="set_in_switch",_tag_dyneither(_tmp39A,sizeof(char),14)))));struct
Cyc_Tcenv_Fenv _tmp1BE;struct Cyc_Tcenv_CtrlEnv*_tmp1BF;struct Cyc_Tcenv_Fenv*
_tmp1BD=fe;_tmp1BE=*_tmp1BD;_tmp1BF=_tmp1BE.ctrl_env;_tmp1BF->break_stmt=_tmp1BF->next_stmt;
_tmp1BF->next_stmt=(void*)& Cyc_Tcenv_CaseEnd_j_val;return Cyc_Tcenv_put_fenv(r,te,
fe);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,void*j);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct
Cyc_Tcenv_Tenv*te,void*j){struct Cyc_Tcenv_SharedFenv*_tmp1C3;struct Cyc_List_List*
_tmp1C4;struct Cyc_RgnOrder_RgnPO*_tmp1C5;struct Cyc_Tcenv_Bindings*_tmp1C6;struct
Cyc_Absyn_Stmt*_tmp1C7;struct Cyc_Tcenv_CtrlEnv*_tmp1C8;void*_tmp1C9;void*_tmp1CA;
int _tmp1CB;const char*_tmp39B;struct Cyc_Tcenv_Fenv _tmp1C2=*Cyc_Tcenv_get_fenv(te,((
_tmp39B="set_next",_tag_dyneither(_tmp39B,sizeof(char),9))));_tmp1C3=_tmp1C2.shared;
_tmp1C4=_tmp1C2.type_vars;_tmp1C5=_tmp1C2.region_order;_tmp1C6=_tmp1C2.locals;
_tmp1C7=_tmp1C2.encloser;_tmp1C8=_tmp1C2.ctrl_env;_tmp1C9=_tmp1C2.capability;
_tmp1CA=_tmp1C2.curr_rgn;_tmp1CB=_tmp1C2.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv*
_tmp39C;struct Cyc_Tcenv_CtrlEnv*new_cenv=(_tmp39C=_region_malloc(r,sizeof(*
_tmp39C)),((_tmp39C->ctrl_rgn=r,((_tmp39C->continue_stmt=(void*)_tmp1C8->continue_stmt,((
_tmp39C->break_stmt=(void*)_tmp1C8->break_stmt,((_tmp39C->next_stmt=j,((_tmp39C->fallthru_clause=(
struct _tuple10*)_tmp1C8->fallthru_clause,((_tmp39C->try_depth=_tmp1C8->try_depth,
_tmp39C)))))))))))));struct Cyc_Tcenv_Fenv*_tmp39D;struct Cyc_Tcenv_Fenv*new_fenv=(
_tmp39D=_region_malloc(r,sizeof(*_tmp39D)),((_tmp39D->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp1C3,((_tmp39D->type_vars=(struct Cyc_List_List*)_tmp1C4,((_tmp39D->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp1C5,((_tmp39D->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp1C6),((_tmp39D->encloser=(struct Cyc_Absyn_Stmt*)
_tmp1C7,((_tmp39D->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp39D->capability=(
void*)_tmp1C9,((_tmp39D->curr_rgn=(void*)_tmp1CA,((_tmp39D->in_notreadctxt=(int)
_tmp1CB,((_tmp39D->fnrgn=(struct _RegionHandle*)r,_tmp39D)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,new_fenv);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*
s){struct Cyc_Tcenv_Fenv _tmp1D0;struct Cyc_Tcenv_SharedFenv*_tmp1D1;const char*
_tmp39E;struct Cyc_Tcenv_Fenv*_tmp1CF=Cyc_Tcenv_get_fenv(te,((_tmp39E="add_label",
_tag_dyneither(_tmp39E,sizeof(char),10))));_tmp1D0=*_tmp1CF;_tmp1D1=_tmp1D0.shared;{
struct Cyc_Dict_Dict needed=_tmp1D1->needed_labels;struct Cyc_List_List**sl_opt=((
struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(
needed,v);struct _RegionHandle*frgn=_tmp1D1->frgn;if(sl_opt != 0){_tmp1D1->needed_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct
_dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{struct Cyc_List_List*_tmp1D2=*
sl_opt;s->non_local_preds=_tmp1D2;for(0;_tmp1D2 != 0;_tmp1D2=_tmp1D2->tl){void*
_tmp1D3=((struct Cyc_Absyn_Stmt*)_tmp1D2->hd)->r;struct Cyc_Absyn_Stmt*_tmp1D5;
struct Cyc_Absyn_Stmt**_tmp1D6;_LL78: {struct Cyc_Absyn_Goto_s_struct*_tmp1D4=(
struct Cyc_Absyn_Goto_s_struct*)_tmp1D3;if(_tmp1D4->tag != 8)goto _LL7A;else{
_tmp1D5=_tmp1D4->f2;_tmp1D6=(struct Cyc_Absyn_Stmt**)& _tmp1D4->f2;}}_LL79:*
_tmp1D6=(struct Cyc_Absyn_Stmt*)s;goto _LL77;_LL7A:;_LL7B:{const char*_tmp39F;((int(*)(
struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp39F="Tcenv: add_label backpatching of non-goto",
_tag_dyneither(_tmp39F,sizeof(char),42))));}goto _LL77;_LL77:;}}}if(((int(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp1D1->seen_labels,
v)){const char*_tmp3A3;void*_tmp3A2[1];struct Cyc_String_pa_struct _tmp3A1;(_tmp3A1.tag=
0,((_tmp3A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3A2[0]=&
_tmp3A1,Cyc_Tcutil_terr(s->loc,((_tmp3A3="Repeated label: %s",_tag_dyneither(
_tmp3A3,sizeof(char),19))),_tag_dyneither(_tmp3A2,sizeof(void*),1)))))));}
_tmp1D1->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp1D1->seen_labels,v,
s);return te;}}int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp1DD;struct Cyc_Tcenv_SharedFenv*
_tmp1DE;const char*_tmp3A4;struct Cyc_Tcenv_Fenv*_tmp1DC=Cyc_Tcenv_get_fenv(te,((
_tmp3A4="all_labels_resolved",_tag_dyneither(_tmp3A4,sizeof(char),20))));_tmp1DD=*
_tmp1DC;_tmp1DE=_tmp1DD.shared;return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(
_tmp1DE->needed_labels);}struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*
te);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){struct
Cyc_Tcenv_Fenv _tmp1E1;struct Cyc_Absyn_Stmt*_tmp1E2;const char*_tmp3A5;struct Cyc_Tcenv_Fenv*
_tmp1E0=Cyc_Tcenv_get_fenv(te,((_tmp3A5="get_encloser",_tag_dyneither(_tmp3A5,
sizeof(char),13))));_tmp1E1=*_tmp1E0;_tmp1E2=_tmp1E1.encloser;return _tmp1E2;}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*
te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct
Cyc_Tcenv_Tenv*te){if(te->le == 0){struct Cyc_Tcenv_Tenv*_tmp3A6;return(_tmp3A6=
_region_malloc(r2,sizeof(*_tmp3A6)),((_tmp3A6->ns=te->ns,((_tmp3A6->ae=te->ae,((
_tmp3A6->le=0,((_tmp3A6->allow_valueof=1,_tmp3A6)))))))));}{struct Cyc_Tcenv_SharedFenv*
_tmp1E5;struct Cyc_List_List*_tmp1E6;struct Cyc_RgnOrder_RgnPO*_tmp1E7;struct Cyc_Tcenv_Bindings*
_tmp1E8;struct Cyc_Absyn_Stmt*_tmp1E9;struct Cyc_Tcenv_CtrlEnv*_tmp1EA;void*
_tmp1EB;void*_tmp1EC;int _tmp1ED;struct Cyc_Tcenv_Fenv _tmp1E4=*((struct Cyc_Tcenv_Fenv*)
_check_null(te->le));_tmp1E5=_tmp1E4.shared;_tmp1E6=_tmp1E4.type_vars;_tmp1E7=
_tmp1E4.region_order;_tmp1E8=_tmp1E4.locals;_tmp1E9=_tmp1E4.encloser;_tmp1EA=
_tmp1E4.ctrl_env;_tmp1EB=_tmp1E4.capability;_tmp1EC=_tmp1E4.curr_rgn;_tmp1ED=
_tmp1E4.in_notreadctxt;{struct Cyc_Tcenv_Fenv*_tmp3A7;struct Cyc_Tcenv_Fenv*
_tmp1EE=(_tmp3A7=_region_malloc(r2,sizeof(*_tmp3A7)),((_tmp3A7->shared=(struct
Cyc_Tcenv_SharedFenv*)_tmp1E5,((_tmp3A7->type_vars=(struct Cyc_List_List*)_tmp1E6,((
_tmp3A7->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1E7,((_tmp3A7->locals=(
struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmp1E8),((_tmp3A7->encloser=(
struct Cyc_Absyn_Stmt*)_tmp1E9,((_tmp3A7->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmp1EA),((_tmp3A7->capability=(void*)_tmp1EB,((_tmp3A7->curr_rgn=(
void*)_tmp1EC,((_tmp3A7->in_notreadctxt=(int)_tmp1ED,((_tmp3A7->fnrgn=(struct
_RegionHandle*)r2,_tmp3A7)))))))))))))))))))));struct Cyc_Tcenv_Tenv*_tmp3A8;
return(_tmp3A8=_region_malloc(r2,sizeof(*_tmp3A8)),((_tmp3A8->ns=te->ns,((
_tmp3A8->ae=te->ae,((_tmp3A8->le=_tmp1EE,((_tmp3A8->allow_valueof=1,_tmp3A8)))))))));}}}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){struct Cyc_Tcenv_SharedFenv*
_tmp1F3;struct Cyc_List_List*_tmp1F4;struct Cyc_RgnOrder_RgnPO*_tmp1F5;struct Cyc_Tcenv_Bindings*
_tmp1F6;struct Cyc_Absyn_Stmt*_tmp1F7;struct Cyc_Tcenv_CtrlEnv*_tmp1F8;void*
_tmp1F9;void*_tmp1FA;int _tmp1FB;const char*_tmp3A9;struct Cyc_Tcenv_Fenv _tmp1F2=*
Cyc_Tcenv_get_fenv(te,((_tmp3A9="set_encloser",_tag_dyneither(_tmp3A9,sizeof(
char),13))));_tmp1F3=_tmp1F2.shared;_tmp1F4=_tmp1F2.type_vars;_tmp1F5=_tmp1F2.region_order;
_tmp1F6=_tmp1F2.locals;_tmp1F7=_tmp1F2.encloser;_tmp1F8=_tmp1F2.ctrl_env;_tmp1F9=
_tmp1F2.capability;_tmp1FA=_tmp1F2.curr_rgn;_tmp1FB=_tmp1F2.in_notreadctxt;{
struct Cyc_Tcenv_Fenv*_tmp3AA;struct Cyc_Tcenv_Fenv*_tmp1FC=(_tmp3AA=
_region_malloc(r,sizeof(*_tmp3AA)),((_tmp3AA->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp1F3,((_tmp3AA->type_vars=(struct Cyc_List_List*)_tmp1F4,((_tmp3AA->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp1F5,((_tmp3AA->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp1F6),((_tmp3AA->encloser=(struct Cyc_Absyn_Stmt*)s,((
_tmp3AA->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp1F8),((
_tmp3AA->capability=(void*)_tmp1F9,((_tmp3AA->curr_rgn=(void*)_tmp1FA,((_tmp3AA->in_notreadctxt=(
int)_tmp1FB,((_tmp3AA->fnrgn=(struct _RegionHandle*)r,_tmp3AA)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp1FC);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,void*rgn,int resetable,int opened){struct Cyc_Absyn_Tvar*tv;{void*_tmp1FE=Cyc_Tcutil_compress(
rgn);struct Cyc_Absyn_Tvar*_tmp200;_LL7D: {struct Cyc_Absyn_VarType_struct*_tmp1FF=(
struct Cyc_Absyn_VarType_struct*)_tmp1FE;if(_tmp1FF->tag != 2)goto _LL7F;else{
_tmp200=_tmp1FF->f1;}}_LL7E: tv=_tmp200;goto _LL7C;_LL7F:;_LL80:{const char*_tmp3AB;
tv=((struct Cyc_Absyn_Tvar*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((
_tmp3AB="bad add region",_tag_dyneither(_tmp3AB,sizeof(char),15))));}goto _LL7C;
_LL7C:;}{struct Cyc_Tcenv_SharedFenv*_tmp204;struct Cyc_List_List*_tmp205;struct
Cyc_RgnOrder_RgnPO*_tmp206;struct Cyc_Tcenv_Bindings*_tmp207;struct Cyc_Absyn_Stmt*
_tmp208;struct Cyc_Tcenv_CtrlEnv*_tmp209;void*_tmp20A;void*_tmp20B;int _tmp20C;
const char*_tmp3AC;struct Cyc_Tcenv_Fenv _tmp203=*Cyc_Tcenv_get_fenv(te,((_tmp3AC="add_region",
_tag_dyneither(_tmp3AC,sizeof(char),11))));_tmp204=_tmp203.shared;_tmp205=
_tmp203.type_vars;_tmp206=_tmp203.region_order;_tmp207=_tmp203.locals;_tmp208=
_tmp203.encloser;_tmp209=_tmp203.ctrl_env;_tmp20A=_tmp203.capability;_tmp20B=
_tmp203.curr_rgn;_tmp20C=_tmp203.in_notreadctxt;_tmp206=Cyc_RgnOrder_add_youngest(
_tmp204->frgn,_tmp206,tv,resetable,opened);{struct Cyc_Absyn_JoinEff_struct
_tmp3BF;struct Cyc_Absyn_AccessEff_struct*_tmp3BE;struct Cyc_Absyn_AccessEff_struct
_tmp3BD;struct Cyc_List_List*_tmp3BC;struct Cyc_List_List*_tmp3BB;struct Cyc_Absyn_JoinEff_struct*
_tmp3BA;_tmp20A=(void*)((_tmp3BA=_cycalloc(sizeof(*_tmp3BA)),((_tmp3BA[0]=((
_tmp3BF.tag=24,((_tmp3BF.f1=((_tmp3BB=_cycalloc(sizeof(*_tmp3BB)),((_tmp3BB->hd=(
void*)((_tmp3BE=_cycalloc(sizeof(*_tmp3BE)),((_tmp3BE[0]=((_tmp3BD.tag=23,((
_tmp3BD.f1=(void*)rgn,_tmp3BD)))),_tmp3BE)))),((_tmp3BB->tl=((_tmp3BC=_cycalloc(
sizeof(*_tmp3BC)),((_tmp3BC->hd=_tmp20A,((_tmp3BC->tl=0,_tmp3BC)))))),_tmp3BB)))))),
_tmp3BF)))),_tmp3BA))));}{struct Cyc_Tcenv_Fenv*_tmp3C0;struct Cyc_Tcenv_Fenv*
_tmp213=(_tmp3C0=_region_malloc(r,sizeof(*_tmp3C0)),((_tmp3C0->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp204,((_tmp3C0->type_vars=(struct Cyc_List_List*)_tmp205,((_tmp3C0->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp206,((_tmp3C0->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp207),((_tmp3C0->encloser=(struct Cyc_Absyn_Stmt*)
_tmp208,((_tmp3C0->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp209),((_tmp3C0->capability=(void*)_tmp20A,((_tmp3C0->curr_rgn=(void*)_tmp20B,((
_tmp3C0->in_notreadctxt=(int)_tmp20C,((_tmp3C0->fnrgn=(struct _RegionHandle*)r,
_tmp3C0)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,_tmp213);}}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_named_block(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Tvar*block_rgn){struct Cyc_Tcenv_SharedFenv*_tmp217;struct Cyc_List_List*
_tmp218;struct Cyc_RgnOrder_RgnPO*_tmp219;struct Cyc_Tcenv_Bindings*_tmp21A;struct
Cyc_Absyn_Stmt*_tmp21B;struct Cyc_Tcenv_CtrlEnv*_tmp21C;void*_tmp21D;void*_tmp21E;
int _tmp21F;const char*_tmp3C1;struct Cyc_Tcenv_Fenv _tmp216=*Cyc_Tcenv_get_fenv(te,((
_tmp3C1="new_named_block",_tag_dyneither(_tmp3C1,sizeof(char),16))));_tmp217=
_tmp216.shared;_tmp218=_tmp216.type_vars;_tmp219=_tmp216.region_order;_tmp21A=
_tmp216.locals;_tmp21B=_tmp216.encloser;_tmp21C=_tmp216.ctrl_env;_tmp21D=_tmp216.capability;
_tmp21E=_tmp216.curr_rgn;_tmp21F=_tmp216.in_notreadctxt;{const char*_tmp3C2;
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,((
_tmp3C2="new_block",_tag_dyneither(_tmp3C2,sizeof(char),10)))));struct Cyc_Absyn_VarType_struct
_tmp3C5;struct Cyc_Absyn_VarType_struct*_tmp3C4;void*block_typ=(void*)((_tmp3C4=
_cycalloc(sizeof(*_tmp3C4)),((_tmp3C4[0]=((_tmp3C5.tag=2,((_tmp3C5.f1=block_rgn,
_tmp3C5)))),_tmp3C4))));{struct Cyc_List_List*_tmp3C6;_tmp218=((_tmp3C6=_cycalloc(
sizeof(*_tmp3C6)),((_tmp3C6->hd=block_rgn,((_tmp3C6->tl=_tmp218,_tmp3C6))))));}
Cyc_Tcutil_check_unique_tvars(loc,_tmp218);_tmp219=Cyc_RgnOrder_add_youngest(
_tmp217->frgn,_tmp219,block_rgn,0,0);{struct Cyc_Absyn_JoinEff_struct _tmp3D9;
struct Cyc_Absyn_AccessEff_struct*_tmp3D8;struct Cyc_Absyn_AccessEff_struct _tmp3D7;
struct Cyc_List_List*_tmp3D6;struct Cyc_List_List*_tmp3D5;struct Cyc_Absyn_JoinEff_struct*
_tmp3D4;_tmp21D=(void*)((_tmp3D4=_cycalloc(sizeof(*_tmp3D4)),((_tmp3D4[0]=((
_tmp3D9.tag=24,((_tmp3D9.f1=((_tmp3D5=_cycalloc(sizeof(*_tmp3D5)),((_tmp3D5->hd=(
void*)((_tmp3D8=_cycalloc(sizeof(*_tmp3D8)),((_tmp3D8[0]=((_tmp3D7.tag=23,((
_tmp3D7.f1=(void*)block_typ,_tmp3D7)))),_tmp3D8)))),((_tmp3D5->tl=((_tmp3D6=
_cycalloc(sizeof(*_tmp3D6)),((_tmp3D6->hd=_tmp21D,((_tmp3D6->tl=0,_tmp3D6)))))),
_tmp3D5)))))),_tmp3D9)))),_tmp3D4))));}_tmp21E=block_typ;{struct Cyc_Tcenv_Fenv*
_tmp3DA;struct Cyc_Tcenv_Fenv*_tmp227=(_tmp3DA=_region_malloc(r,sizeof(*_tmp3DA)),((
_tmp3DA->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp217,((_tmp3DA->type_vars=(
struct Cyc_List_List*)_tmp218,((_tmp3DA->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp219,((_tmp3DA->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp21A),((_tmp3DA->encloser=(struct Cyc_Absyn_Stmt*)_tmp21B,((_tmp3DA->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp21C),((_tmp3DA->capability=(
void*)_tmp21D,((_tmp3DA->curr_rgn=(void*)_tmp21E,((_tmp3DA->in_notreadctxt=(int)
_tmp21F,((_tmp3DA->fnrgn=(struct _RegionHandle*)r,_tmp3DA)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp227);}}}static struct Cyc_Absyn_Eq_kb_struct Cyc_Tcenv_rgn_kb={
0,Cyc_Absyn_RgnKind};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te);struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te){struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)&
Cyc_Tcenv_rgn_kb);Cyc_Tcutil_add_tvar_identity(t);return Cyc_Tcenv_new_named_block(
r,loc,te,t);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*
loc);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*
r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*loc){
struct Cyc_Tcenv_SharedFenv*_tmp22F;struct Cyc_List_List*_tmp230;struct Cyc_RgnOrder_RgnPO*
_tmp231;struct Cyc_Tcenv_Bindings*_tmp232;struct Cyc_Absyn_Stmt*_tmp233;struct Cyc_Tcenv_CtrlEnv*
_tmp234;void*_tmp235;void*_tmp236;int _tmp237;const char*_tmp3DB;struct Cyc_Tcenv_Fenv
_tmp22E=*Cyc_Tcenv_get_fenv(te,((_tmp3DB="new_outlives_constraints",
_tag_dyneither(_tmp3DB,sizeof(char),25))));_tmp22F=_tmp22E.shared;_tmp230=
_tmp22E.type_vars;_tmp231=_tmp22E.region_order;_tmp232=_tmp22E.locals;_tmp233=
_tmp22E.encloser;_tmp234=_tmp22E.ctrl_env;_tmp235=_tmp22E.capability;_tmp236=
_tmp22E.curr_rgn;_tmp237=_tmp22E.in_notreadctxt;for(0;cs != 0;cs=cs->tl){_tmp231=
Cyc_RgnOrder_add_outlives_constraint(_tmp22F->frgn,_tmp231,(*((struct _tuple0*)cs->hd)).f1,(*((
struct _tuple0*)cs->hd)).f2,loc);}{struct Cyc_Tcenv_Fenv*_tmp3DC;struct Cyc_Tcenv_Fenv*
_tmp238=(_tmp3DC=_region_malloc(r,sizeof(*_tmp3DC)),((_tmp3DC->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp22F,((_tmp3DC->type_vars=(struct Cyc_List_List*)_tmp230,((_tmp3DC->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp231,((_tmp3DC->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp232),((_tmp3DC->encloser=(struct Cyc_Absyn_Stmt*)
_tmp233,((_tmp3DC->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp234),((_tmp3DC->capability=(void*)_tmp235,((_tmp3DC->curr_rgn=(void*)_tmp236,((
_tmp3DC->in_notreadctxt=(int)_tmp237,((_tmp3DC->fnrgn=(struct _RegionHandle*)r,
_tmp3DC)))))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,_tmp238);}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*
r1,void*r2,struct _tuple11**oldtv,struct Cyc_Position_Segment*loc);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*
r1,void*r2,struct _tuple11**oldtv,struct Cyc_Position_Segment*loc){void*_tmp23A=
Cyc_Tcutil_compress(r1);void*_tmp23B=Cyc_Tcutil_compress(r2);enum Cyc_Absyn_Kind
_tmp23C=Cyc_Tcutil_typ_kind(_tmp23A);enum Cyc_Absyn_Kind _tmp23D=Cyc_Tcutil_typ_kind(
_tmp23B);int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp23C,_tmp23D);int r2_le_r1=Cyc_Tcutil_kind_leq(
_tmp23D,_tmp23C);if(!r1_le_r2  && !r2_le_r1){{const char*_tmp3E3;void*_tmp3E2[4];
struct Cyc_String_pa_struct _tmp3E1;struct Cyc_String_pa_struct _tmp3E0;struct Cyc_String_pa_struct
_tmp3DF;struct Cyc_String_pa_struct _tmp3DE;(_tmp3DE.tag=0,((_tmp3DE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp23D)),((
_tmp3DF.tag=0,((_tmp3DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp23C)),((_tmp3E0.tag=0,((_tmp3E0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp23B)),((_tmp3E1.tag=0,((_tmp3E1.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp23A)),((
_tmp3E2[0]=& _tmp3E1,((_tmp3E2[1]=& _tmp3E0,((_tmp3E2[2]=& _tmp3DF,((_tmp3E2[3]=&
_tmp3DE,Cyc_Tcutil_terr(loc,((_tmp3E3="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n",
_tag_dyneither(_tmp3E3,sizeof(char),82))),_tag_dyneither(_tmp3E2,sizeof(void*),4)))))))))))))))))))))))));}
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{if(r1_le_r2  && !
r2_le_r1){struct Cyc_Absyn_Tvar*_tmp245;void*_tmp246;struct _tuple11 _tmp244=Cyc_Tcutil_swap_kind(
_tmp23B,Cyc_Tcutil_kind_to_bound(_tmp23C));_tmp245=_tmp244.f1;_tmp246=_tmp244.f2;{
struct _tuple11*_tmp3E4;*oldtv=((_tmp3E4=_region_malloc(r,sizeof(*_tmp3E4)),((
_tmp3E4->f1=_tmp245,((_tmp3E4->f2=_tmp246,_tmp3E4))))));}}else{if(!r1_le_r2  && 
r2_le_r1){struct Cyc_Absyn_Tvar*_tmp249;void*_tmp24A;struct _tuple11 _tmp248=Cyc_Tcutil_swap_kind(
_tmp23A,Cyc_Tcutil_kind_to_bound(_tmp23D));_tmp249=_tmp248.f1;_tmp24A=_tmp248.f2;{
struct _tuple11*_tmp3E5;*oldtv=((_tmp3E5=_region_malloc(r,sizeof(*_tmp3E5)),((
_tmp3E5->f1=_tmp249,((_tmp3E5->f2=_tmp24A,_tmp3E5))))));}}}}{struct _RegionHandle*
_tmp24C=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp24D=0;if(_tmp23A != (void*)&
Cyc_Absyn_HeapRgn_val  && _tmp23A != (void*)& Cyc_Absyn_UniqueRgn_val){struct Cyc_Absyn_AccessEff_struct
_tmp3E8;struct Cyc_Absyn_AccessEff_struct*_tmp3E7;void*eff1=(void*)((_tmp3E7=
_cycalloc(sizeof(*_tmp3E7)),((_tmp3E7[0]=((_tmp3E8.tag=23,((_tmp3E8.f1=(void*)
_tmp23A,_tmp3E8)))),_tmp3E7))));struct _tuple0*_tmp3EB;struct Cyc_List_List*
_tmp3EA;_tmp24D=((_tmp3EA=_region_malloc(_tmp24C,sizeof(*_tmp3EA)),((_tmp3EA->hd=((
_tmp3EB=_region_malloc(_tmp24C,sizeof(*_tmp3EB)),((_tmp3EB->f1=eff1,((_tmp3EB->f2=
_tmp23B,_tmp3EB)))))),((_tmp3EA->tl=_tmp24D,_tmp3EA))))));}if(_tmp23B != (void*)&
Cyc_Absyn_HeapRgn_val  && _tmp23B != (void*)& Cyc_Absyn_UniqueRgn_val){struct Cyc_Absyn_AccessEff_struct
_tmp3EE;struct Cyc_Absyn_AccessEff_struct*_tmp3ED;void*eff2=(void*)((_tmp3ED=
_cycalloc(sizeof(*_tmp3ED)),((_tmp3ED[0]=((_tmp3EE.tag=23,((_tmp3EE.f1=(void*)
_tmp23B,_tmp3EE)))),_tmp3ED))));struct _tuple0*_tmp3F1;struct Cyc_List_List*
_tmp3F0;_tmp24D=((_tmp3F0=_region_malloc(_tmp24C,sizeof(*_tmp3F0)),((_tmp3F0->hd=((
_tmp3F1=_region_malloc(_tmp24C,sizeof(*_tmp3F1)),((_tmp3F1->f1=eff2,((_tmp3F1->f2=
_tmp23A,_tmp3F1)))))),((_tmp3F0->tl=_tmp24D,_tmp3F0))))));}return Cyc_Tcenv_new_outlives_constraints((
struct _RegionHandle*)_tmp24C,te,_tmp24D,loc);}}void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*
te);void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*
_tmp256=te->le;if(_tmp256 == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{struct Cyc_Tcenv_Fenv*
fe=(struct Cyc_Tcenv_Fenv*)_tmp256;struct Cyc_Tcenv_Fenv _tmp258;void*_tmp259;
struct Cyc_Tcenv_Fenv*_tmp257=fe;_tmp258=*_tmp257;_tmp259=_tmp258.curr_rgn;return
_tmp259;}}void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*rgn);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void*rgn){const char*_tmp3F2;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(
te,((_tmp3F2="check_rgn_accessible",_tag_dyneither(_tmp3F2,sizeof(char),21))));
struct Cyc_Tcenv_Fenv _tmp25B;void*_tmp25C;struct Cyc_RgnOrder_RgnPO*_tmp25D;struct
Cyc_Tcenv_Fenv*_tmp25A=fe;_tmp25B=*_tmp25A;_tmp25C=_tmp25B.capability;_tmp25D=
_tmp25B.region_order;if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp25C) || Cyc_Tcutil_region_in_effect(
1,rgn,_tmp25C))return;{struct Cyc_Absyn_AccessEff_struct _tmp3F5;struct Cyc_Absyn_AccessEff_struct*
_tmp3F4;if(Cyc_RgnOrder_eff_outlives_eff(_tmp25D,(void*)((_tmp3F4=_cycalloc(
sizeof(*_tmp3F4)),((_tmp3F4[0]=((_tmp3F5.tag=23,((_tmp3F5.f1=(void*)rgn,_tmp3F5)))),
_tmp3F4)))),_tmp25C))return;}{const char*_tmp3F9;void*_tmp3F8[1];struct Cyc_String_pa_struct
_tmp3F7;(_tmp3F7.tag=0,((_tmp3F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(rgn)),((_tmp3F8[0]=& _tmp3F7,Cyc_Tcutil_terr(loc,((_tmp3F9="Expression accesses unavailable region %s",
_tag_dyneither(_tmp3F9,sizeof(char),42))),_tag_dyneither(_tmp3F8,sizeof(void*),1)))))));}}
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*rgn);void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void*rgn){Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
struct Cyc_Tcenv_Fenv _tmp266;struct Cyc_RgnOrder_RgnPO*_tmp267;const char*_tmp3FA;
struct Cyc_Tcenv_Fenv*_tmp265=Cyc_Tcenv_get_fenv(te,((_tmp3FA="check_rgn_resetable",
_tag_dyneither(_tmp3FA,sizeof(char),20))));_tmp266=*_tmp265;_tmp267=_tmp266.region_order;{
void*_tmp268=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp26A;_LL82: {
struct Cyc_Absyn_VarType_struct*_tmp269=(struct Cyc_Absyn_VarType_struct*)_tmp268;
if(_tmp269->tag != 2)goto _LL84;else{_tmp26A=_tmp269->f1;}}_LL83: if(!Cyc_RgnOrder_is_region_resetable(
_tmp267,_tmp26A)){const char*_tmp3FE;void*_tmp3FD[1];struct Cyc_String_pa_struct
_tmp3FC;(_tmp3FC.tag=0,((_tmp3FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(rgn)),((_tmp3FD[0]=& _tmp3FC,Cyc_Tcutil_terr(loc,((_tmp3FE="Region %s is not resetable",
_tag_dyneither(_tmp3FE,sizeof(char),27))),_tag_dyneither(_tmp3FD,sizeof(void*),1)))))));}
return;_LL84:;_LL85: {const char*_tmp401;void*_tmp400;(_tmp400=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp401="check_rgn_resetable",
_tag_dyneither(_tmp401,sizeof(char),20))),_tag_dyneither(_tmp400,sizeof(void*),0)));}
_LL81:;}}}int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*
rt_b);int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp270=te->le;rt_a=Cyc_Tcutil_compress(rt_a);rt_b=Cyc_Tcutil_compress(
rt_b);if(_tmp270 == 0)return rt_a == (void*)& Cyc_Absyn_UniqueRgn_val  || rt_a == (
void*)& Cyc_Absyn_HeapRgn_val  && rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;{struct
Cyc_Tcenv_Fenv*fe=(struct Cyc_Tcenv_Fenv*)_tmp270;struct Cyc_Tcenv_Fenv _tmp272;
struct Cyc_RgnOrder_RgnPO*_tmp273;struct Cyc_Tcenv_Fenv*_tmp271=fe;_tmp272=*
_tmp271;_tmp273=_tmp272.region_order;{struct Cyc_Absyn_AccessEff_struct _tmp404;
struct Cyc_Absyn_AccessEff_struct*_tmp403;int _tmp274=Cyc_RgnOrder_effect_outlives(
_tmp273,(void*)((_tmp403=_cycalloc(sizeof(*_tmp403)),((_tmp403[0]=((_tmp404.tag=
23,((_tmp404.f1=(void*)rt_a,_tmp404)))),_tmp403)))),rt_b);return _tmp274;}}}
struct _tuple13{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;struct Cyc_Position_Segment*
f4;};void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*eff);void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*eff){struct Cyc_Tcenv_Fenv _tmp279;void*
_tmp27A;struct Cyc_RgnOrder_RgnPO*_tmp27B;struct Cyc_Tcenv_SharedFenv*_tmp27C;
const char*_tmp405;struct Cyc_Tcenv_Fenv*_tmp278=Cyc_Tcenv_get_fenv(te,((_tmp405="check_effect_accessible",
_tag_dyneither(_tmp405,sizeof(char),24))));_tmp279=*_tmp278;_tmp27A=_tmp279.capability;
_tmp27B=_tmp279.region_order;_tmp27C=_tmp279.shared;if(Cyc_Tcutil_subset_effect(
0,eff,_tmp27A))return;if(Cyc_RgnOrder_eff_outlives_eff(_tmp27B,eff,_tmp27A))
return;{struct _RegionHandle*frgn=_tmp27C->frgn;struct _tuple13*_tmp408;struct Cyc_List_List*
_tmp407;_tmp27C->delayed_effect_checks=((_tmp407=_region_malloc(frgn,sizeof(*
_tmp407)),((_tmp407->hd=((_tmp408=_region_malloc(frgn,sizeof(*_tmp408)),((
_tmp408->f1=_tmp27A,((_tmp408->f2=eff,((_tmp408->f3=_tmp27B,((_tmp408->f4=loc,
_tmp408)))))))))),((_tmp407->tl=_tmp27C->delayed_effect_checks,_tmp407))))));}}
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp281;struct Cyc_Tcenv_SharedFenv*
_tmp282;const char*_tmp409;struct Cyc_Tcenv_Fenv*_tmp280=Cyc_Tcenv_get_fenv(te,((
_tmp409="check_delayed_effects",_tag_dyneither(_tmp409,sizeof(char),22))));
_tmp281=*_tmp280;_tmp282=_tmp281.shared;{struct Cyc_List_List*_tmp283=_tmp282->delayed_effect_checks;
for(0;_tmp283 != 0;_tmp283=_tmp283->tl){struct _tuple13 _tmp285;void*_tmp286;void*
_tmp287;struct Cyc_RgnOrder_RgnPO*_tmp288;struct Cyc_Position_Segment*_tmp289;
struct _tuple13*_tmp284=(struct _tuple13*)_tmp283->hd;_tmp285=*_tmp284;_tmp286=
_tmp285.f1;_tmp287=_tmp285.f2;_tmp288=_tmp285.f3;_tmp289=_tmp285.f4;if(Cyc_Tcutil_subset_effect(
1,_tmp287,_tmp286))continue;if(Cyc_RgnOrder_eff_outlives_eff(_tmp288,_tmp287,
_tmp286))continue;{const char*_tmp40E;void*_tmp40D[2];struct Cyc_String_pa_struct
_tmp40C;struct Cyc_String_pa_struct _tmp40B;(_tmp40B.tag=0,((_tmp40B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp287)),((_tmp40C.tag=
0,((_tmp40C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp286)),((_tmp40D[0]=& _tmp40C,((_tmp40D[1]=& _tmp40B,Cyc_Tcutil_terr(_tmp289,((
_tmp40E="Capability \n%s\ndoes not cover function's effect\n%s",_tag_dyneither(
_tmp40E,sizeof(char),51))),_tag_dyneither(_tmp40D,sizeof(void*),2)))))))))))));}}}}
struct _tuple14{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;struct Cyc_Position_Segment*
f3;};void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*po);void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po){struct Cyc_Tcenv_Fenv*
_tmp28E=te->le;if(_tmp28E == 0){for(0;po != 0;po=po->tl){struct Cyc_Absyn_AccessEff_struct
_tmp411;struct Cyc_Absyn_AccessEff_struct*_tmp410;if(!Cyc_Tcutil_subset_effect(1,(*((
struct _tuple0*)po->hd)).f1,Cyc_Absyn_empty_effect) || !Cyc_Tcutil_subset_effect(
1,(void*)((_tmp410=_cycalloc(sizeof(*_tmp410)),((_tmp410[0]=((_tmp411.tag=23,((
_tmp411.f1=(void*)(*((struct _tuple0*)po->hd)).f2,_tmp411)))),_tmp410)))),Cyc_Absyn_empty_effect)){
const char*_tmp414;void*_tmp413;(_tmp413=0,Cyc_Tcutil_terr(loc,((_tmp414="the required region ordering is not satisfied here",
_tag_dyneither(_tmp414,sizeof(char),51))),_tag_dyneither(_tmp413,sizeof(void*),0)));}}
return;}{struct Cyc_Tcenv_Fenv _tmp294;struct Cyc_RgnOrder_RgnPO*_tmp295;struct Cyc_Tcenv_SharedFenv*
_tmp296;struct Cyc_Tcenv_Fenv*_tmp293=(struct Cyc_Tcenv_Fenv*)_tmp28E;_tmp294=*
_tmp293;_tmp295=_tmp294.region_order;_tmp296=_tmp294.shared;if(!Cyc_RgnOrder_satisfies_constraints(
_tmp295,po,(void*)& Cyc_Absyn_HeapRgn_val,0)){struct _tuple14*_tmp417;struct Cyc_List_List*
_tmp416;_tmp296->delayed_constraint_checks=((_tmp416=_region_malloc(_tmp296->frgn,
sizeof(*_tmp416)),((_tmp416->hd=((_tmp417=_region_malloc(_tmp296->frgn,sizeof(*
_tmp417)),((_tmp417->f1=_tmp295,((_tmp417->f2=po,((_tmp417->f3=loc,_tmp417)))))))),((
_tmp416->tl=_tmp296->delayed_constraint_checks,_tmp416))))));}}}void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv _tmp29B;struct Cyc_Tcenv_SharedFenv*_tmp29C;const char*
_tmp418;struct Cyc_Tcenv_Fenv*_tmp29A=Cyc_Tcenv_get_fenv(te,((_tmp418="check_delayed_constraints",
_tag_dyneither(_tmp418,sizeof(char),26))));_tmp29B=*_tmp29A;_tmp29C=_tmp29B.shared;{
struct Cyc_List_List*_tmp29D=_tmp29C->delayed_constraint_checks;for(0;_tmp29D != 0;
_tmp29D=_tmp29D->tl){struct _tuple14 _tmp29F;struct Cyc_RgnOrder_RgnPO*_tmp2A0;
struct Cyc_List_List*_tmp2A1;struct Cyc_Position_Segment*_tmp2A2;struct _tuple14*
_tmp29E=(struct _tuple14*)_tmp29D->hd;_tmp29F=*_tmp29E;_tmp2A0=_tmp29F.f1;_tmp2A1=
_tmp29F.f2;_tmp2A2=_tmp29F.f3;if(!Cyc_RgnOrder_satisfies_constraints(_tmp2A0,
_tmp2A1,(void*)& Cyc_Absyn_HeapRgn_val,1)){const char*_tmp41B;void*_tmp41A;(
_tmp41A=0,Cyc_Tcutil_terr(_tmp2A2,((_tmp41B="the required region ordering is not satisfied here",
_tag_dyneither(_tmp41B,sizeof(char),51))),_tag_dyneither(_tmp41A,sizeof(void*),0)));}}}}
struct _tuple15{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct
Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Fndecl*fd);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Fndecl*fd){struct
Cyc_Tcenv_Bindings*locals=0;struct _dyneither_ptr*_tmp428;const char*_tmp427;void*
_tmp426[1];struct Cyc_String_pa_struct _tmp425;struct Cyc_Absyn_Tvar*_tmp424;struct
Cyc_Absyn_Tvar*rgn0=(_tmp424=_cycalloc(sizeof(*_tmp424)),((_tmp424->name=((
_tmp428=_cycalloc(sizeof(*_tmp428)),((_tmp428[0]=(struct _dyneither_ptr)((_tmp425.tag=
0,((_tmp425.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((
_tmp426[0]=& _tmp425,Cyc_aprintf(((_tmp427="`%s",_tag_dyneither(_tmp427,sizeof(
char),4))),_tag_dyneither(_tmp426,sizeof(void*),1)))))))),_tmp428)))),((_tmp424->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp424->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp424)))))));
struct Cyc_List_List*_tmp429;struct Cyc_List_List*_tmp2A5=(_tmp429=_cycalloc(
sizeof(*_tmp429)),((_tmp429->hd=rgn0,((_tmp429->tl=fd->tvs,_tmp429)))));Cyc_Tcutil_check_unique_tvars(
loc,_tmp2A5);{struct Cyc_RgnOrder_RgnPO*_tmp2A6=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,
fd->rgn_po,(void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,rgn0,loc);
struct Cyc_Absyn_VarType_struct _tmp42C;struct Cyc_Absyn_VarType_struct*_tmp42B;
void*param_rgn=(void*)((_tmp42B=_cycalloc(sizeof(*_tmp42B)),((_tmp42B[0]=((
_tmp42C.tag=2,((_tmp42C.f1=rgn0,_tmp42C)))),_tmp42B))));struct Cyc_Core_Opt*
_tmp42D;struct Cyc_Core_Opt*param_rgn_opt=(_tmp42D=_cycalloc(sizeof(*_tmp42D)),((
_tmp42D->v=param_rgn,_tmp42D)));struct Cyc_List_List*_tmp2A7=0;{struct Cyc_List_List*
_tmp2A8=fd->args;for(0;_tmp2A8 != 0;_tmp2A8=_tmp2A8->tl){struct Cyc_Absyn_Vardecl
_tmp433;struct _tuple1*_tmp432;struct Cyc_Absyn_Vardecl*_tmp431;struct Cyc_Absyn_Vardecl*
_tmp2A9=(_tmp431=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp431[0]=((
_tmp433.sc=Cyc_Absyn_Public,((_tmp433.name=((_tmp432=_cycalloc(sizeof(*_tmp432)),((
_tmp432->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmp432->f2=(*((struct
_tuple15*)_tmp2A8->hd)).f1,_tmp432)))))),((_tmp433.tq=(*((struct _tuple15*)
_tmp2A8->hd)).f2,((_tmp433.type=(*((struct _tuple15*)_tmp2A8->hd)).f3,((_tmp433.initializer=
0,((_tmp433.rgn=param_rgn_opt,((_tmp433.attributes=0,((_tmp433.escapes=0,_tmp433)))))))))))))))),
_tmp431)));{struct Cyc_List_List _tmp436;struct Cyc_List_List*_tmp435;_tmp2A7=((
_tmp435=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp435[0]=((_tmp436.hd=
_tmp2A9,((_tmp436.tl=_tmp2A7,_tmp436)))),_tmp435))));}{struct Cyc_Absyn_Param_b_struct*
_tmp43C;struct Cyc_Absyn_Param_b_struct _tmp43B;struct Cyc_Tcenv_Bindings*_tmp43A;
locals=((_tmp43A=_region_malloc(r,sizeof(*_tmp43A)),((_tmp43A->v=(*((struct
_tuple15*)_tmp2A8->hd)).f1,((_tmp43A->b=(void*)((_tmp43C=_cycalloc(sizeof(*
_tmp43C)),((_tmp43C[0]=((_tmp43B.tag=3,((_tmp43B.f1=_tmp2A9,_tmp43B)))),_tmp43C)))),((
_tmp43A->tl=locals,_tmp43A))))))));}}}if(fd->cyc_varargs != 0){struct Cyc_Core_Opt*
_tmp2B3;struct Cyc_Absyn_Tqual _tmp2B4;void*_tmp2B5;int _tmp2B6;struct Cyc_Absyn_VarargInfo
_tmp2B2=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));_tmp2B3=
_tmp2B2.name;_tmp2B4=_tmp2B2.tq;_tmp2B5=_tmp2B2.type;_tmp2B6=_tmp2B2.inject;if(
_tmp2B3 != 0){void*_tmp2B7=Cyc_Absyn_dyneither_typ(_tmp2B5,param_rgn,_tmp2B4,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp442;struct _tuple1*_tmp441;struct Cyc_Absyn_Vardecl*
_tmp440;struct Cyc_Absyn_Vardecl*_tmp2B8=(_tmp440=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1),((_tmp440[0]=((_tmp442.sc=Cyc_Absyn_Public,((_tmp442.name=((_tmp441=
_cycalloc(sizeof(*_tmp441)),((_tmp441->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((
_tmp441->f2=(struct _dyneither_ptr*)_tmp2B3->v,_tmp441)))))),((_tmp442.tq=Cyc_Absyn_empty_tqual(
0),((_tmp442.type=_tmp2B7,((_tmp442.initializer=0,((_tmp442.rgn=param_rgn_opt,((
_tmp442.attributes=0,((_tmp442.escapes=0,_tmp442)))))))))))))))),_tmp440)));{
struct Cyc_List_List*_tmp443;_tmp2A7=((_tmp443=_cycalloc(sizeof(*_tmp443)),((
_tmp443->hd=_tmp2B8,((_tmp443->tl=_tmp2A7,_tmp443))))));}{struct Cyc_Absyn_Param_b_struct*
_tmp449;struct Cyc_Absyn_Param_b_struct _tmp448;struct Cyc_Tcenv_Bindings*_tmp447;
locals=((_tmp447=_region_malloc(r,sizeof(*_tmp447)),((_tmp447->v=(struct
_dyneither_ptr*)_tmp2B3->v,((_tmp447->b=(void*)((_tmp449=_cycalloc(sizeof(*
_tmp449)),((_tmp449[0]=((_tmp448.tag=3,((_tmp448.f1=_tmp2B8,_tmp448)))),_tmp449)))),((
_tmp447->tl=locals,_tmp447))))))));}}else{const char*_tmp44C;void*_tmp44B;(
_tmp44B=0,Cyc_Tcutil_terr(loc,((_tmp44C="missing name for varargs",
_tag_dyneither(_tmp44C,sizeof(char),25))),_tag_dyneither(_tmp44B,sizeof(void*),0)));}}{
struct Cyc_Core_Opt _tmp44F;struct Cyc_Core_Opt*_tmp44E;fd->param_vardecls=((
_tmp44E=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp44E[0]=((_tmp44F.v=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2A7),
_tmp44F)),_tmp44E))));}{struct Cyc_Tcenv_SharedFenv*_tmp46D;struct Cyc_Tcenv_CtrlEnv*
_tmp46C;struct Cyc_Absyn_JoinEff_struct*_tmp46B;struct Cyc_List_List*_tmp46A;
struct Cyc_List_List*_tmp469;struct Cyc_Absyn_AccessEff_struct _tmp468;struct Cyc_Absyn_AccessEff_struct*
_tmp467;struct Cyc_Absyn_JoinEff_struct _tmp466;struct Cyc_Tcenv_Fenv*_tmp465;
return(_tmp465=_region_malloc(r,sizeof(*_tmp465)),((_tmp465->shared=(struct Cyc_Tcenv_SharedFenv*)((
_tmp46D=_region_malloc(r,sizeof(*_tmp46D)),((_tmp46D->frgn=r,((_tmp46D->return_typ=
fd->ret_type,((_tmp46D->seen_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((
_tmp46D->needed_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((
_tmp46D->delayed_effect_checks=0,((_tmp46D->delayed_constraint_checks=0,_tmp46D)))))))))))))),((
_tmp465->type_vars=(struct Cyc_List_List*)_tmp2A5,((_tmp465->region_order=(struct
Cyc_RgnOrder_RgnPO*)_tmp2A6,((_tmp465->locals=(struct Cyc_Tcenv_Bindings*)locals,((
_tmp465->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp465->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
_tmp46C=_region_malloc(r,sizeof(*_tmp46C)),((_tmp46C->ctrl_rgn=r,((_tmp46C->continue_stmt=(
void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp46C->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((
_tmp46C->fallthru_clause=0,((_tmp46C->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((
_tmp46C->try_depth=0,_tmp46C)))))))))))))),((_tmp465->capability=(void*)((void*)((
_tmp46B=_cycalloc(sizeof(*_tmp46B)),((_tmp46B[0]=((_tmp466.tag=24,((_tmp466.f1=((
_tmp46A=_cycalloc(sizeof(*_tmp46A)),((_tmp46A->hd=(void*)((_tmp467=_cycalloc(
sizeof(*_tmp467)),((_tmp467[0]=((_tmp468.tag=23,((_tmp468.f1=(void*)param_rgn,
_tmp468)))),_tmp467)))),((_tmp46A->tl=((_tmp469=_cycalloc(sizeof(*_tmp469)),((
_tmp469->hd=(void*)((struct Cyc_Core_Opt*)_check_null(fd->effect))->v,((_tmp469->tl=
0,_tmp469)))))),_tmp46A)))))),_tmp466)))),_tmp46B))))),((_tmp465->curr_rgn=(void*)
param_rgn,((_tmp465->in_notreadctxt=(int)0,((_tmp465->fnrgn=(struct _RegionHandle*)
r,_tmp465)))))))))))))))))))));}}}struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*
fd);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){struct Cyc_Tcenv_Fenv
_tmp2D7;struct Cyc_Tcenv_Bindings*_tmp2D8;struct Cyc_RgnOrder_RgnPO*_tmp2D9;struct
Cyc_List_List*_tmp2DA;struct Cyc_Tcenv_SharedFenv*_tmp2DB;struct _RegionHandle*
_tmp2DC;struct Cyc_Tcenv_Fenv*_tmp2D6=old_fenv;_tmp2D7=*_tmp2D6;_tmp2D8=_tmp2D7.locals;
_tmp2D9=_tmp2D7.region_order;_tmp2DA=_tmp2D7.type_vars;_tmp2DB=_tmp2D7.shared;
_tmp2DC=_tmp2D7.fnrgn;{struct _RegionHandle*_tmp2DD=_tmp2DB->frgn;struct Cyc_Tcenv_Bindings*
_tmp2DE=(struct Cyc_Tcenv_Bindings*)_tmp2D8;struct _dyneither_ptr*_tmp47A;const
char*_tmp479;void*_tmp478[1];struct Cyc_String_pa_struct _tmp477;struct Cyc_Absyn_Tvar*
_tmp476;struct Cyc_Absyn_Tvar*rgn0=(_tmp476=_cycalloc(sizeof(*_tmp476)),((_tmp476->name=((
_tmp47A=_cycalloc(sizeof(*_tmp47A)),((_tmp47A[0]=(struct _dyneither_ptr)((_tmp477.tag=
0,((_tmp477.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((
_tmp478[0]=& _tmp477,Cyc_aprintf(((_tmp479="`%s",_tag_dyneither(_tmp479,sizeof(
char),4))),_tag_dyneither(_tmp478,sizeof(void*),1)))))))),_tmp47A)))),((_tmp476->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp476->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp476)))))));{
struct Cyc_List_List*_tmp2DF=fd->tvs;for(0;_tmp2DF != 0;_tmp2DF=_tmp2DF->tl){if(
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2DF->hd,Cyc_Absyn_BoxKind)== Cyc_Absyn_RgnKind)
_tmp2D9=Cyc_RgnOrder_add_youngest(_tmp2DD,_tmp2D9,(struct Cyc_Absyn_Tvar*)_tmp2DF->hd,
0,0);else{if(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2DF->hd,Cyc_Absyn_BoxKind)
== Cyc_Absyn_UniqueRgnKind  || Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp2DF->hd,Cyc_Absyn_BoxKind)== Cyc_Absyn_TopRgnKind){const char*_tmp47D;void*
_tmp47C;(_tmp47C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp47D="non-intuitionistic tvar in nested_fenv",_tag_dyneither(_tmp47D,sizeof(
char),39))),_tag_dyneither(_tmp47C,sizeof(void*),0)));}}}}_tmp2D9=Cyc_RgnOrder_add_youngest(
_tmp2DD,_tmp2D9,rgn0,0,0);{struct Cyc_List_List*_tmp47E;struct Cyc_List_List*
_tmp2E2=(_tmp47E=_cycalloc(sizeof(*_tmp47E)),((_tmp47E->hd=rgn0,((_tmp47E->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
fd->tvs,_tmp2DA),_tmp47E)))));Cyc_Tcutil_check_unique_tvars(loc,_tmp2E2);{struct
Cyc_Absyn_VarType_struct _tmp481;struct Cyc_Absyn_VarType_struct*_tmp480;void*
param_rgn=(void*)((_tmp480=_cycalloc(sizeof(*_tmp480)),((_tmp480[0]=((_tmp481.tag=
2,((_tmp481.f1=rgn0,_tmp481)))),_tmp480))));struct Cyc_Core_Opt*_tmp482;struct Cyc_Core_Opt*
param_rgn_opt=(_tmp482=_cycalloc(sizeof(*_tmp482)),((_tmp482->v=param_rgn,
_tmp482)));struct Cyc_List_List*_tmp2E3=0;{struct Cyc_List_List*_tmp2E4=fd->args;
for(0;_tmp2E4 != 0;_tmp2E4=_tmp2E4->tl){struct Cyc_Absyn_Vardecl _tmp488;struct
_tuple1*_tmp487;struct Cyc_Absyn_Vardecl*_tmp486;struct Cyc_Absyn_Vardecl*_tmp2E5=(
_tmp486=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp486[0]=((_tmp488.sc=
Cyc_Absyn_Public,((_tmp488.name=((_tmp487=_cycalloc(sizeof(*_tmp487)),((_tmp487->f1=(
union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmp487->f2=(*((struct _tuple15*)_tmp2E4->hd)).f1,
_tmp487)))))),((_tmp488.tq=(*((struct _tuple15*)_tmp2E4->hd)).f2,((_tmp488.type=(*((
struct _tuple15*)_tmp2E4->hd)).f3,((_tmp488.initializer=0,((_tmp488.rgn=
param_rgn_opt,((_tmp488.attributes=0,((_tmp488.escapes=0,_tmp488)))))))))))))))),
_tmp486)));{struct Cyc_List_List _tmp48B;struct Cyc_List_List*_tmp48A;_tmp2E3=((
_tmp48A=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp48A[0]=((_tmp48B.hd=
_tmp2E5,((_tmp48B.tl=_tmp2E3,_tmp48B)))),_tmp48A))));}{struct Cyc_Absyn_Param_b_struct*
_tmp491;struct Cyc_Absyn_Param_b_struct _tmp490;struct Cyc_Tcenv_Bindings*_tmp48F;
_tmp2DE=(struct Cyc_Tcenv_Bindings*)((_tmp48F=_region_malloc(_tmp2DD,sizeof(*
_tmp48F)),((_tmp48F->v=(*((struct _tuple15*)_tmp2E4->hd)).f1,((_tmp48F->b=(void*)((
_tmp491=_cycalloc(sizeof(*_tmp491)),((_tmp491[0]=((_tmp490.tag=3,((_tmp490.f1=
_tmp2E5,_tmp490)))),_tmp491)))),((_tmp48F->tl=_tmp2DE,_tmp48F))))))));}}}if(fd->cyc_varargs
!= 0){struct Cyc_Core_Opt*_tmp2EF;struct Cyc_Absyn_Tqual _tmp2F0;void*_tmp2F1;int
_tmp2F2;struct Cyc_Absyn_VarargInfo _tmp2EE=*((struct Cyc_Absyn_VarargInfo*)
_check_null(fd->cyc_varargs));_tmp2EF=_tmp2EE.name;_tmp2F0=_tmp2EE.tq;_tmp2F1=
_tmp2EE.type;_tmp2F2=_tmp2EE.inject;if(_tmp2EF != 0){void*_tmp2F3=Cyc_Absyn_dyneither_typ(
_tmp2F1,param_rgn,_tmp2F0,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl _tmp497;
struct _tuple1*_tmp496;struct Cyc_Absyn_Vardecl*_tmp495;struct Cyc_Absyn_Vardecl*
_tmp2F4=(_tmp495=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp495[0]=((
_tmp497.sc=Cyc_Absyn_Public,((_tmp497.name=((_tmp496=_cycalloc(sizeof(*_tmp496)),((
_tmp496->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmp496->f2=(struct
_dyneither_ptr*)_tmp2EF->v,_tmp496)))))),((_tmp497.tq=Cyc_Absyn_empty_tqual(0),((
_tmp497.type=_tmp2F3,((_tmp497.initializer=0,((_tmp497.rgn=param_rgn_opt,((
_tmp497.attributes=0,((_tmp497.escapes=0,_tmp497)))))))))))))))),_tmp495)));{
struct Cyc_List_List*_tmp498;_tmp2E3=((_tmp498=_cycalloc(sizeof(*_tmp498)),((
_tmp498->hd=_tmp2F4,((_tmp498->tl=_tmp2E3,_tmp498))))));}{struct Cyc_Absyn_Param_b_struct*
_tmp49E;struct Cyc_Absyn_Param_b_struct _tmp49D;struct Cyc_Tcenv_Bindings*_tmp49C;
_tmp2DE=(struct Cyc_Tcenv_Bindings*)((_tmp49C=_region_malloc(_tmp2DD,sizeof(*
_tmp49C)),((_tmp49C->v=(struct _dyneither_ptr*)_tmp2EF->v,((_tmp49C->b=(void*)((
_tmp49E=_cycalloc(sizeof(*_tmp49E)),((_tmp49E[0]=((_tmp49D.tag=3,((_tmp49D.f1=
_tmp2F4,_tmp49D)))),_tmp49E)))),((_tmp49C->tl=_tmp2DE,_tmp49C))))))));}}else{
const char*_tmp4A1;void*_tmp4A0;(_tmp4A0=0,Cyc_Tcutil_terr(loc,((_tmp4A1="missing name for varargs",
_tag_dyneither(_tmp4A1,sizeof(char),25))),_tag_dyneither(_tmp4A0,sizeof(void*),0)));}}{
struct Cyc_Core_Opt _tmp4A4;struct Cyc_Core_Opt*_tmp4A3;fd->param_vardecls=((
_tmp4A3=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp4A3[0]=((_tmp4A4.v=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2E3),
_tmp4A4)),_tmp4A3))));}{struct Cyc_Tcenv_SharedFenv*_tmp4C2;struct Cyc_Tcenv_CtrlEnv*
_tmp4C1;struct Cyc_Absyn_JoinEff_struct*_tmp4C0;struct Cyc_List_List*_tmp4BF;
struct Cyc_List_List*_tmp4BE;struct Cyc_Absyn_AccessEff_struct _tmp4BD;struct Cyc_Absyn_AccessEff_struct*
_tmp4BC;struct Cyc_Absyn_JoinEff_struct _tmp4BB;struct Cyc_Tcenv_Fenv*_tmp4BA;
return(struct Cyc_Tcenv_Fenv*)((_tmp4BA=_region_malloc(_tmp2DD,sizeof(*_tmp4BA)),((
_tmp4BA->shared=(struct Cyc_Tcenv_SharedFenv*)((_tmp4C2=_region_malloc(_tmp2DD,
sizeof(*_tmp4C2)),((_tmp4C2->frgn=_tmp2DD,((_tmp4C2->return_typ=fd->ret_type,((
_tmp4C2->seen_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp2DD,Cyc_strptrcmp),((
_tmp4C2->needed_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp2DD,Cyc_strptrcmp),((
_tmp4C2->delayed_effect_checks=0,((_tmp4C2->delayed_constraint_checks=0,_tmp4C2)))))))))))))),((
_tmp4BA->type_vars=(struct Cyc_List_List*)_tmp2E2,((_tmp4BA->region_order=(struct
Cyc_RgnOrder_RgnPO*)_tmp2D9,((_tmp4BA->locals=(struct Cyc_Tcenv_Bindings*)_tmp2DE,((
_tmp4BA->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp4BA->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)((_tmp4C1=_region_malloc(_tmp2DD,sizeof(*_tmp4C1)),((
_tmp4C1->ctrl_rgn=_tmp2DD,((_tmp4C1->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((
_tmp4C1->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4C1->fallthru_clause=0,((
_tmp4C1->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp4C1->try_depth=0,_tmp4C1))))))))))))))),((
_tmp4BA->capability=(void*)((void*)((_tmp4C0=_cycalloc(sizeof(*_tmp4C0)),((
_tmp4C0[0]=((_tmp4BB.tag=24,((_tmp4BB.f1=((_tmp4BF=_cycalloc(sizeof(*_tmp4BF)),((
_tmp4BF->hd=(void*)((_tmp4BC=_cycalloc(sizeof(*_tmp4BC)),((_tmp4BC[0]=((_tmp4BD.tag=
23,((_tmp4BD.f1=(void*)param_rgn,_tmp4BD)))),_tmp4BC)))),((_tmp4BF->tl=((_tmp4BE=
_cycalloc(sizeof(*_tmp4BE)),((_tmp4BE->hd=(void*)((struct Cyc_Core_Opt*)
_check_null(fd->effect))->v,((_tmp4BE->tl=0,_tmp4BE)))))),_tmp4BF)))))),_tmp4BB)))),
_tmp4C0))))),((_tmp4BA->curr_rgn=(void*)param_rgn,((_tmp4BA->in_notreadctxt=(int)
0,((_tmp4BA->fnrgn=(struct _RegionHandle*)_tmp2DC,_tmp4BA))))))))))))))))))))));}}}}}
