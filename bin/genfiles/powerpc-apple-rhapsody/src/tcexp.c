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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);
extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[16];
extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*
x,int i);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*
r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{
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
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct
_tuple1*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};union Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct
UnknownTunion;struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{
union Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern
struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct
Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,struct Cyc_Position_Segment*);
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);int Cyc_strcmp(struct _dyneither_ptr s1,struct
_dyneither_ptr s2);struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;
struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);struct
Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct
_RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct
Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
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
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;int allow_valueof;};struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*
Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void
Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*,void*);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);int
Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int
Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*
is_dyneither_ptr);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt
Cyc_Tcutil_trk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;
extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);struct _tuple4{struct Cyc_List_List*f1;struct
_RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*
Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);
void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);
void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);
int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(void*
t);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);int
Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple6{
int f1;void*f2;};struct _tuple6 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);struct _tuple7{unsigned int f1;int
f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2);int Cyc_Evexp_okay_szofarg(void*t);void Cyc_Tcstmt_tcStmt(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*);
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct _dyneither_ptr,struct Cyc_Position_Segment*);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);extern int Cyc_Tcexp_in_stmt_exp;
struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{
int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};struct Cyc_CfFlowInfo_UniquePlace{struct Cyc_CfFlowInfo_Place place;struct
Cyc_List_List*path;};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int
f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*
f2;};struct Cyc_CfFlowInfo_LessNumelts_struct{int tag;struct Cyc_Absyn_Vardecl*f1;}
;struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqNumelts_struct{
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
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap);static void*Cyc_Tcexp_expr_err(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct
_dyneither_ptr msg,struct _dyneither_ptr ap){{void*_tmp4EB;(_tmp4EB=0,Cyc_Tcutil_terr(
loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),
_tag_dyneither(_tmp4EB,sizeof(void*),0)));}if(topt == 0)return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q);static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct
_handler_cons _tmp1;_push_handler(& _tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))
_tmp3=1;if(!_tmp3){{struct _RegionHandle*_tmp4=Cyc_Tcenv_get_fnrgn(te);void*_tmp5=
Cyc_Tcenv_lookup_ordinary(_tmp4,te,e->loc,q);void*_tmp6;struct Cyc_Absyn_Enumdecl*
_tmp7;struct Cyc_Absyn_Enumfield*_tmp8;void*_tmp9;struct Cyc_Absyn_Enumfield*_tmpA;
struct Cyc_Absyn_Tuniondecl*_tmpB;struct Cyc_Absyn_Tunionfield*_tmpC;_LL1: if(*((
int*)_tmp5)!= 0)goto _LL3;_tmp6=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp5)->f1;
_LL2:{struct Cyc_Absyn_Var_e_struct _tmp4EE;struct Cyc_Absyn_Var_e_struct*_tmp4ED;(
void*)(e->r=(void*)((void*)((_tmp4ED=_cycalloc(sizeof(*_tmp4ED)),((_tmp4ED[0]=((
_tmp4EE.tag=1,((_tmp4EE.f1=q,((_tmp4EE.f2=(void*)_tmp6,_tmp4EE)))))),_tmp4ED))))));}
goto _LL0;_LL3: if(*((int*)_tmp5)!= 3)goto _LL5;_tmp7=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp5)->f1;_tmp8=((struct Cyc_Tcenv_EnumRes_struct*)_tmp5)->f2;_LL4:{struct Cyc_Absyn_Enum_e_struct
_tmp4F1;struct Cyc_Absyn_Enum_e_struct*_tmp4F0;(void*)(e->r=(void*)((void*)((
_tmp4F0=_cycalloc(sizeof(*_tmp4F0)),((_tmp4F0[0]=((_tmp4F1.tag=33,((_tmp4F1.f1=q,((
_tmp4F1.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7,((_tmp4F1.f3=(struct Cyc_Absyn_Enumfield*)
_tmp8,_tmp4F1)))))))),_tmp4F0))))));}goto _LL0;_LL5: if(*((int*)_tmp5)!= 4)goto
_LL7;_tmp9=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f1;_tmpA=((
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f2;_LL6:{struct Cyc_Absyn_AnonEnum_e_struct
_tmp4F4;struct Cyc_Absyn_AnonEnum_e_struct*_tmp4F3;(void*)(e->r=(void*)((void*)((
_tmp4F3=_cycalloc(sizeof(*_tmp4F3)),((_tmp4F3[0]=((_tmp4F4.tag=34,((_tmp4F4.f1=q,((
_tmp4F4.f2=(void*)_tmp9,((_tmp4F4.f3=(struct Cyc_Absyn_Enumfield*)_tmpA,_tmp4F4)))))))),
_tmp4F3))))));}goto _LL0;_LL7: if(*((int*)_tmp5)!= 2)goto _LL9;_tmpB=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp5)->f1;_tmpC=((struct Cyc_Tcenv_TunionRes_struct*)_tmp5)->f2;_LL8:{struct Cyc_Absyn_Tunion_e_struct
_tmp4F7;struct Cyc_Absyn_Tunion_e_struct*_tmp4F6;(void*)(e->r=(void*)((void*)((
_tmp4F6=_cycalloc(sizeof(*_tmp4F6)),((_tmp4F6[0]=((_tmp4F7.tag=32,((_tmp4F7.f1=0,((
_tmp4F7.f2=_tmpB,((_tmp4F7.f3=_tmpC,_tmp4F7)))))))),_tmp4F6))))));}goto _LL0;_LL9:
if(*((int*)_tmp5)!= 1)goto _LL0;_LLA:{const char*_tmp4FB;void*_tmp4FA[1];struct Cyc_String_pa_struct
_tmp4F9;(_tmp4F9.tag=0,((_tmp4F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(q)),((_tmp4FA[0]=& _tmp4F9,Cyc_Tcutil_terr(e->loc,((
_tmp4FB="bad occurrence of type name %s",_tag_dyneither(_tmp4FB,sizeof(char),31))),
_tag_dyneither(_tmp4FA,sizeof(void*),1)))))));}{struct Cyc_Absyn_Var_e_struct
_tmp4FE;struct Cyc_Absyn_Var_e_struct*_tmp4FD;(void*)(e->r=(void*)((void*)((
_tmp4FD=_cycalloc(sizeof(*_tmp4FD)),((_tmp4FD[0]=((_tmp4FE.tag=1,((_tmp4FE.f1=q,((
_tmp4FE.f2=(void*)((void*)0),_tmp4FE)))))),_tmp4FD))))));}goto _LL0;_LL0:;};
_pop_handler();}else{void*_tmp2=(void*)_exn_thrown;void*_tmp1B=_tmp2;_LLC: if(
_tmp1B != Cyc_Dict_Absent)goto _LLE;_LLD:{struct Cyc_Absyn_Var_e_struct _tmp501;
struct Cyc_Absyn_Var_e_struct*_tmp500;(void*)(e->r=(void*)((void*)((_tmp500=
_cycalloc(sizeof(*_tmp500)),((_tmp500[0]=((_tmp501.tag=1,((_tmp501.f1=q,((
_tmp501.f2=(void*)((void*)0),_tmp501)))))),_tmp500))))));}goto _LLB;_LLE:;_LLF:(
void)_throw(_tmp1B);_LLB:;}}}struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple8*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e);static
struct _tuple8*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){struct _tuple8*
_tmp502;return(_tmp502=_cycalloc(sizeof(*_tmp502)),((_tmp502->f1=0,((_tmp502->f2=
e,_tmp502)))));}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es);static void
Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*_tmp1F=(void*)e1->r;struct
_tuple1*_tmp20;_LL11: if(*((int*)_tmp1F)!= 2)goto _LL13;_tmp20=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp1F)->f1;_LL12: {struct _handler_cons _tmp21;_push_handler(& _tmp21);{int _tmp23=
0;if(setjmp(_tmp21.handler))_tmp23=1;if(!_tmp23){{struct _RegionHandle*_tmp24=Cyc_Tcenv_get_fnrgn(
te);void*_tmp25=Cyc_Tcenv_lookup_ordinary(_tmp24,te,e1->loc,_tmp20);void*_tmp26;
struct Cyc_Absyn_Tuniondecl*_tmp27;struct Cyc_Absyn_Tunionfield*_tmp28;struct Cyc_Absyn_Aggrdecl*
_tmp29;_LL16: if(*((int*)_tmp25)!= 0)goto _LL18;_tmp26=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmp25)->f1;_LL17:{struct Cyc_Absyn_FnCall_e_struct _tmp505;struct Cyc_Absyn_FnCall_e_struct*
_tmp504;(void*)(e->r=(void*)((void*)((_tmp504=_cycalloc(sizeof(*_tmp504)),((
_tmp504[0]=((_tmp505.tag=11,((_tmp505.f1=e1,((_tmp505.f2=es,((_tmp505.f3=0,
_tmp505)))))))),_tmp504))))));}_npop_handler(0);return;_LL18: if(*((int*)_tmp25)
!= 2)goto _LL1A;_tmp27=((struct Cyc_Tcenv_TunionRes_struct*)_tmp25)->f1;_tmp28=((
struct Cyc_Tcenv_TunionRes_struct*)_tmp25)->f2;_LL19: if(_tmp28->typs == 0){const
char*_tmp509;void*_tmp508[1];struct Cyc_String_pa_struct _tmp507;(_tmp507.tag=0,((
_tmp507.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp28->name)),((_tmp508[0]=& _tmp507,Cyc_Tcutil_terr(e->loc,((_tmp509="%s is a constant, not a function",
_tag_dyneither(_tmp509,sizeof(char),33))),_tag_dyneither(_tmp508,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Tunion_e_struct _tmp50C;struct Cyc_Absyn_Tunion_e_struct*_tmp50B;(
void*)(e->r=(void*)((void*)((_tmp50B=_cycalloc(sizeof(*_tmp50B)),((_tmp50B[0]=((
_tmp50C.tag=32,((_tmp50C.f1=es,((_tmp50C.f2=_tmp27,((_tmp50C.f3=_tmp28,_tmp50C)))))))),
_tmp50B))))));}_npop_handler(0);return;_LL1A: if(*((int*)_tmp25)!= 1)goto _LL1C;
_tmp29=((struct Cyc_Tcenv_AggrRes_struct*)_tmp25)->f1;_LL1B: {struct Cyc_List_List*
_tmp31=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct Cyc_Absyn_Exp*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);{struct Cyc_Absyn_Struct_e_struct
_tmp50F;struct Cyc_Absyn_Struct_e_struct*_tmp50E;(void*)(e->r=(void*)((void*)((
_tmp50E=_cycalloc(sizeof(*_tmp50E)),((_tmp50E[0]=((_tmp50F.tag=30,((_tmp50F.f1=
_tmp29->name,((_tmp50F.f2=0,((_tmp50F.f3=_tmp31,((_tmp50F.f4=(struct Cyc_Absyn_Aggrdecl*)
_tmp29,_tmp50F)))))))))),_tmp50E))))));}_npop_handler(0);return;}_LL1C: if(*((int*)
_tmp25)!= 4)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp25)!= 3)goto _LL15;
_LL1F:{const char*_tmp513;void*_tmp512[1];struct Cyc_String_pa_struct _tmp511;(
_tmp511.tag=0,((_tmp511.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp20)),((_tmp512[0]=& _tmp511,Cyc_Tcutil_terr(e->loc,((_tmp513="%s is an enum constructor, not a function",
_tag_dyneither(_tmp513,sizeof(char),42))),_tag_dyneither(_tmp512,sizeof(void*),1)))))));}
_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*_tmp22=(void*)
_exn_thrown;void*_tmp38=_tmp22;_LL21: if(_tmp38 != Cyc_Dict_Absent)goto _LL23;_LL22:{
struct Cyc_Absyn_FnCall_e_struct _tmp516;struct Cyc_Absyn_FnCall_e_struct*_tmp515;(
void*)(e->r=(void*)((void*)((_tmp515=_cycalloc(sizeof(*_tmp515)),((_tmp515[0]=((
_tmp516.tag=11,((_tmp516.f1=e1,((_tmp516.f2=es,((_tmp516.f3=0,_tmp516)))))))),
_tmp515))))));}return;_LL23:;_LL24:(void)_throw(_tmp38);_LL20:;}}}_LL13:;_LL14:{
struct Cyc_Absyn_FnCall_e_struct _tmp519;struct Cyc_Absyn_FnCall_e_struct*_tmp518;(
void*)(e->r=(void*)((void*)((_tmp518=_cycalloc(sizeof(*_tmp518)),((_tmp518[0]=((
_tmp519.tag=11,((_tmp519.f1=e1,((_tmp519.f2=es,((_tmp519.f3=0,_tmp519)))))))),
_tmp518))))));}return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des);static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
des){if(topt == 0){{struct Cyc_Absyn_Array_e_struct _tmp51C;struct Cyc_Absyn_Array_e_struct*
_tmp51B;(void*)(e->r=(void*)((void*)((_tmp51B=_cycalloc(sizeof(*_tmp51B)),((
_tmp51B[0]=((_tmp51C.tag=28,((_tmp51C.f1=des,_tmp51C)))),_tmp51B))))));}return;}{
void*t=*topt;void*_tmp3F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp40;
union Cyc_Absyn_AggrInfoU_union _tmp41;struct Cyc_Absyn_ArrayInfo _tmp42;void*_tmp43;
struct Cyc_Absyn_Tqual _tmp44;_LL26: if(_tmp3F <= (void*)4)goto _LL2C;if(*((int*)
_tmp3F)!= 10)goto _LL28;_tmp40=((struct Cyc_Absyn_AggrType_struct*)_tmp3F)->f1;
_tmp41=_tmp40.aggr_info;_LL27:{union Cyc_Absyn_AggrInfoU_union _tmp45=_tmp41;
struct Cyc_Absyn_Aggrdecl**_tmp46;struct Cyc_Absyn_Aggrdecl*_tmp47;_LL2F: if((
_tmp45.UnknownAggr).tag != 0)goto _LL31;_LL30: {const char*_tmp51F;void*_tmp51E;(
_tmp51E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp51F="struct type not properly set",_tag_dyneither(_tmp51F,sizeof(char),29))),
_tag_dyneither(_tmp51E,sizeof(void*),0)));}_LL31: if((_tmp45.KnownAggr).tag != 1)
goto _LL2E;_tmp46=(_tmp45.KnownAggr).f1;_tmp47=*_tmp46;_LL32: {struct Cyc_Absyn_Struct_e_struct
_tmp522;struct Cyc_Absyn_Struct_e_struct*_tmp521;(void*)(e->r=(void*)((void*)((
_tmp521=_cycalloc(sizeof(*_tmp521)),((_tmp521[0]=((_tmp522.tag=30,((_tmp522.f1=
_tmp47->name,((_tmp522.f2=0,((_tmp522.f3=des,((_tmp522.f4=(struct Cyc_Absyn_Aggrdecl*)
_tmp47,_tmp522)))))))))),_tmp521))))));}_LL2E:;}goto _LL25;_LL28: if(*((int*)
_tmp3F)!= 7)goto _LL2A;_tmp42=((struct Cyc_Absyn_ArrayType_struct*)_tmp3F)->f1;
_tmp43=(void*)_tmp42.elt_type;_tmp44=_tmp42.tq;_LL29:{struct Cyc_Absyn_Array_e_struct
_tmp525;struct Cyc_Absyn_Array_e_struct*_tmp524;(void*)(e->r=(void*)((void*)((
_tmp524=_cycalloc(sizeof(*_tmp524)),((_tmp524[0]=((_tmp525.tag=28,((_tmp525.f1=
des,_tmp525)))),_tmp524))))));}goto _LL25;_LL2A: if(*((int*)_tmp3F)!= 11)goto _LL2C;
_LL2B:{struct Cyc_Absyn_AnonStruct_e_struct _tmp528;struct Cyc_Absyn_AnonStruct_e_struct*
_tmp527;(void*)(e->r=(void*)((void*)((_tmp527=_cycalloc(sizeof(*_tmp527)),((
_tmp527[0]=((_tmp528.tag=31,((_tmp528.f1=(void*)t,((_tmp528.f2=des,_tmp528)))))),
_tmp527))))));}goto _LL25;_LL2C:;_LL2D:{struct Cyc_Absyn_Array_e_struct _tmp52B;
struct Cyc_Absyn_Array_e_struct*_tmp52A;(void*)(e->r=(void*)((void*)((_tmp52A=
_cycalloc(sizeof(*_tmp52A)),((_tmp52A[0]=((_tmp52B.tag=28,((_tmp52B.f1=des,
_tmp52B)))),_tmp52A))))));}goto _LL25;_LL25:;}}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)es->hd);}}void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(
e);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(
te,e)){const char*_tmp530;void*_tmp52F[2];struct Cyc_String_pa_struct _tmp52E;
struct Cyc_String_pa_struct _tmp52D;(_tmp52D.tag=0,((_tmp52D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp52E.tag=0,((_tmp52E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg_part),((_tmp52F[0]=& _tmp52E,((_tmp52F[1]=& _tmp52D,Cyc_Tcutil_terr(
e->loc,((_tmp530="test of %s has type %s instead of integral or * type",
_tag_dyneither(_tmp530,sizeof(char),53))),_tag_dyneither(_tmp52F,sizeof(void*),2)))))))))))));}}
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,union Cyc_Absyn_Cnst_union*c,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcConst(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,union Cyc_Absyn_Cnst_union*
c,struct Cyc_Absyn_Exp*e){void*t;{union Cyc_Absyn_Cnst_union _tmp56=*((union Cyc_Absyn_Cnst_union*)
_check_null(c));void*_tmp57;void*_tmp58;void*_tmp59;void*_tmp5A;void*_tmp5B;void*
_tmp5C;int _tmp5D;struct _dyneither_ptr _tmp5E;_LL34: if((_tmp56.Char_c).tag != 0)
goto _LL36;_tmp57=(_tmp56.Char_c).f1;if((int)_tmp57 != 0)goto _LL36;_LL35: t=Cyc_Absyn_schar_typ;
goto _LL33;_LL36: if((_tmp56.Char_c).tag != 0)goto _LL38;_tmp58=(_tmp56.Char_c).f1;
if((int)_tmp58 != 1)goto _LL38;_LL37: t=Cyc_Absyn_uchar_typ;goto _LL33;_LL38: if((
_tmp56.Char_c).tag != 0)goto _LL3A;_tmp59=(_tmp56.Char_c).f1;if((int)_tmp59 != 2)
goto _LL3A;_LL39: t=Cyc_Absyn_char_typ;goto _LL33;_LL3A: if((_tmp56.Short_c).tag != 1)
goto _LL3C;_tmp5A=(_tmp56.Short_c).f1;_LL3B: t=_tmp5A == (void*)1?Cyc_Absyn_ushort_typ:
Cyc_Absyn_sshort_typ;goto _LL33;_LL3C: if((_tmp56.LongLong_c).tag != 3)goto _LL3E;
_tmp5B=(_tmp56.LongLong_c).f1;_LL3D: t=_tmp5B == (void*)1?Cyc_Absyn_ulonglong_typ:
Cyc_Absyn_slonglong_typ;goto _LL33;_LL3E: if((_tmp56.Float_c).tag != 4)goto _LL40;
_LL3F: t=Cyc_Absyn_float_typ;goto _LL33;_LL40: if((_tmp56.Int_c).tag != 2)goto _LL42;
_tmp5C=(_tmp56.Int_c).f1;_tmp5D=(_tmp56.Int_c).f2;_LL41: if(topt == 0)t=_tmp5C == (
void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{void*_tmp5F=Cyc_Tcutil_compress(*
topt);void*_tmp60;void*_tmp61;void*_tmp62;void*_tmp63;void*_tmp64;void*_tmp65;
void*_tmp66;void*_tmp67;void*_tmp68;_LL47: if(_tmp5F <= (void*)4)goto _LL53;if(*((
int*)_tmp5F)!= 5)goto _LL49;_tmp60=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f1;_tmp61=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F)->f2;if((int)
_tmp61 != 0)goto _LL49;_LL48:{void*_tmp69=_tmp60;_LL56: if((int)_tmp69 != 1)goto
_LL58;_LL57: t=Cyc_Absyn_uchar_typ;goto _LL55;_LL58: if((int)_tmp69 != 0)goto _LL5A;
_LL59: t=Cyc_Absyn_schar_typ;goto _LL55;_LL5A: if((int)_tmp69 != 2)goto _LL55;_LL5B: t=
Cyc_Absyn_char_typ;goto _LL55;_LL55:;}{union Cyc_Absyn_Cnst_union _tmp531;*c=(union
Cyc_Absyn_Cnst_union)(((_tmp531.Char_c).tag=0,(((_tmp531.Char_c).f1=(void*)
_tmp60,(((_tmp531.Char_c).f2=(char)_tmp5D,_tmp531))))));}goto _LL46;_LL49: if(*((
int*)_tmp5F)!= 5)goto _LL4B;_tmp62=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f1;_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F)->f2;if((int)
_tmp63 != 1)goto _LL4B;_LL4A: t=_tmp62 == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;{
union Cyc_Absyn_Cnst_union _tmp532;*c=(union Cyc_Absyn_Cnst_union)(((_tmp532.Short_c).tag=
1,(((_tmp532.Short_c).f1=(void*)_tmp62,(((_tmp532.Short_c).f2=(short)_tmp5D,
_tmp532))))));}goto _LL46;_LL4B: if(*((int*)_tmp5F)!= 5)goto _LL4D;_tmp64=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5F)->f1;_tmp65=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f2;if((int)_tmp65 != 2)goto _LL4D;_LL4C: t=_tmp64 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL4D: if(*((int*)_tmp5F)!= 5)goto _LL4F;_tmp66=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5F)->f1;_tmp67=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f2;if((int)_tmp67 != 3)goto _LL4F;_LL4E: t=_tmp66 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL4F: if(*((int*)_tmp5F)!= 4)goto _LL51;if(!(_tmp5D
== 0))goto _LL51;_LL50:{struct Cyc_Absyn_Const_e_struct _tmp538;union Cyc_Absyn_Cnst_union
_tmp537;struct Cyc_Absyn_Const_e_struct*_tmp536;(void*)(e->r=(void*)((void*)((
_tmp536=_cycalloc(sizeof(*_tmp536)),((_tmp536[0]=((_tmp538.tag=0,((_tmp538.f1=(
union Cyc_Absyn_Cnst_union)(((_tmp537.Null_c).tag=6,_tmp537)),_tmp538)))),_tmp536))))));}{
struct Cyc_List_List*_tmp6F=Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct
_tmp54B;struct Cyc_Core_Opt*_tmp54A;struct Cyc_Absyn_PtrAtts _tmp549;struct Cyc_Core_Opt*
_tmp548;struct Cyc_Absyn_PtrInfo _tmp547;struct Cyc_Absyn_PointerType_struct*
_tmp546;t=(void*)((_tmp546=_cycalloc(sizeof(*_tmp546)),((_tmp546[0]=((_tmp54B.tag=
4,((_tmp54B.f1=((_tmp547.elt_typ=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ak,((_tmp54A=_cycalloc(sizeof(*_tmp54A)),((_tmp54A->v=_tmp6F,_tmp54A))))),((
_tmp547.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp547.ptr_atts=((_tmp549.rgn=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,((_tmp548=_cycalloc(
sizeof(*_tmp548)),((_tmp548->v=_tmp6F,_tmp548))))),((_tmp549.nullable=Cyc_Absyn_true_conref,((
_tmp549.bounds=Cyc_Absyn_empty_conref(),((_tmp549.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(),((_tmp549.ptrloc=0,_tmp549)))))))))),_tmp547)))))),
_tmp54B)))),_tmp546))));}goto _LL46;}_LL51: if(*((int*)_tmp5F)!= 18)goto _LL53;
_tmp68=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp5F)->f1;_LL52: {struct Cyc_Absyn_ValueofType_struct
_tmp54E;struct Cyc_Absyn_ValueofType_struct*_tmp54D;struct Cyc_Absyn_ValueofType_struct*
_tmp76=(_tmp54D=_cycalloc(sizeof(*_tmp54D)),((_tmp54D[0]=((_tmp54E.tag=17,((
_tmp54E.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp5D,0),_tmp54E)))),_tmp54D)));if(
!Cyc_Tcutil_unify(_tmp68,(void*)_tmp76)){{const char*_tmp553;void*_tmp552[2];
struct Cyc_String_pa_struct _tmp551;struct Cyc_String_pa_struct _tmp550;(_tmp550.tag=
0,((_tmp550.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_tmp76)),((_tmp551.tag=0,((_tmp551.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp68)),((_tmp552[0]=& _tmp551,((_tmp552[1]=&
_tmp550,Cyc_Tcutil_terr(loc,((_tmp553="expecting %s but found %s",_tag_dyneither(
_tmp553,sizeof(char),26))),_tag_dyneither(_tmp552,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct Cyc_Absyn_TagType_struct _tmp556;struct Cyc_Absyn_TagType_struct*
_tmp555;t=(void*)((_tmp555=_cycalloc(sizeof(*_tmp555)),((_tmp555[0]=((_tmp556.tag=
18,((_tmp556.f1=(void*)((void*)_tmp76),_tmp556)))),_tmp555))));}goto _LL46;}_LL53:;
_LL54: t=_tmp5C == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL46;_LL46:;}
goto _LL33;_LL42: if((_tmp56.String_c).tag != 5)goto _LL44;_tmp5E=(_tmp56.String_c).f1;
_LL43: {int len=(int)_get_dyneither_size(_tmp5E,sizeof(char));union Cyc_Absyn_Cnst_union
_tmp557;struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)(((
_tmp557.Int_c).tag=2,(((_tmp557.Int_c).f1=(void*)((void*)1),(((_tmp557.Int_c).f2=
len,_tmp557)))))),loc);{struct Cyc_Core_Opt*_tmp558;elen->topt=((_tmp558=
_cycalloc(sizeof(*_tmp558)),((_tmp558->v=(void*)Cyc_Absyn_uint_typ,_tmp558))));}{
struct Cyc_Absyn_Upper_b_struct _tmp55B;struct Cyc_Absyn_Upper_b_struct*_tmp55A;t=
Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(0),(void*)((
_tmp55A=_cycalloc(sizeof(*_tmp55A)),((_tmp55A[0]=((_tmp55B.tag=0,((_tmp55B.f1=
elen,_tmp55B)))),_tmp55A)))),Cyc_Absyn_true_conref);}if(topt != 0){void*_tmp82=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp83;struct Cyc_Absyn_Tqual
_tmp84;_LL5D: if(_tmp82 <= (void*)4)goto _LL61;if(*((int*)_tmp82)!= 7)goto _LL5F;
_tmp83=((struct Cyc_Absyn_ArrayType_struct*)_tmp82)->f1;_tmp84=_tmp83.tq;_LL5E:
return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmp84,(struct Cyc_Absyn_Exp*)elen,((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),0);_LL5F: if(*((int*)
_tmp82)!= 4)goto _LL61;_LL60: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){{struct Cyc_Core_Opt*_tmp55C;e->topt=((_tmp55C=_cycalloc(sizeof(*
_tmp55C)),((_tmp55C->v=(void*)t,_tmp55C))));}Cyc_Tcutil_unchecked_cast(te,e,*
topt,(void*)3);t=*topt;}else{{struct Cyc_Absyn_Upper_b_struct _tmp55F;struct Cyc_Absyn_Upper_b_struct*
_tmp55E;t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(void*)((
_tmp55E=_cycalloc(sizeof(*_tmp55E)),((_tmp55E[0]=((_tmp55F.tag=0,((_tmp55F.f1=
elen,_tmp55F)))),_tmp55E)))),Cyc_Absyn_true_conref);}if(!Cyc_Tcutil_unify(*topt,
t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){{struct Cyc_Core_Opt*_tmp560;e->topt=((
_tmp560=_cycalloc(sizeof(*_tmp560)),((_tmp560->v=(void*)t,_tmp560))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);t=*topt;}}goto _LL5C;_LL61:;_LL62: goto _LL5C;_LL5C:;}return t;}
_LL44: if((_tmp56.Null_c).tag != 6)goto _LL33;_LL45: {struct Cyc_List_List*_tmp8A=
Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct _tmp573;struct
Cyc_Core_Opt*_tmp572;struct Cyc_Absyn_PtrAtts _tmp571;struct Cyc_Core_Opt*_tmp570;
struct Cyc_Absyn_PtrInfo _tmp56F;struct Cyc_Absyn_PointerType_struct*_tmp56E;t=(
void*)((_tmp56E=_cycalloc(sizeof(*_tmp56E)),((_tmp56E[0]=((_tmp573.tag=4,((
_tmp573.f1=((_tmp56F.elt_typ=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,((
_tmp572=_cycalloc(sizeof(*_tmp572)),((_tmp572->v=_tmp8A,_tmp572))))),((_tmp56F.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp56F.ptr_atts=((_tmp571.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,((_tmp570=_cycalloc(sizeof(*_tmp570)),((
_tmp570->v=_tmp8A,_tmp570))))),((_tmp571.nullable=Cyc_Absyn_true_conref,((
_tmp571.bounds=Cyc_Absyn_empty_conref(),((_tmp571.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(),((_tmp571.ptrloc=0,_tmp571)))))))))),_tmp56F)))))),
_tmp573)))),_tmp56E))));}goto _LL33;}_LL33:;}return t;}static void*Cyc_Tcexp_tcVar(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q,void*b);
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q,void*b){void*_tmp91=b;struct Cyc_Absyn_Vardecl*_tmp92;struct
Cyc_Absyn_Fndecl*_tmp93;struct Cyc_Absyn_Vardecl*_tmp94;struct Cyc_Absyn_Vardecl*
_tmp95;struct Cyc_Absyn_Vardecl*_tmp96;_LL64: if((int)_tmp91 != 0)goto _LL66;_LL65: {
const char*_tmp577;void*_tmp576[1];struct Cyc_String_pa_struct _tmp575;return(
_tmp575.tag=0,((_tmp575.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
q)),((_tmp576[0]=& _tmp575,Cyc_Tcexp_expr_err(te,loc,0,((_tmp577="undeclared identifier: %s",
_tag_dyneither(_tmp577,sizeof(char),26))),_tag_dyneither(_tmp576,sizeof(void*),1)))))));}
_LL66: if(_tmp91 <= (void*)1)goto _LL68;if(*((int*)_tmp91)!= 0)goto _LL68;_tmp92=((
struct Cyc_Absyn_Global_b_struct*)_tmp91)->f1;_LL67:*q=*_tmp92->name;return(void*)
_tmp92->type;_LL68: if(_tmp91 <= (void*)1)goto _LL6A;if(*((int*)_tmp91)!= 1)goto
_LL6A;_tmp93=((struct Cyc_Absyn_Funname_b_struct*)_tmp91)->f1;_LL69:*q=*_tmp93->name;
return Cyc_Tcutil_fndecl2typ(_tmp93);_LL6A: if(_tmp91 <= (void*)1)goto _LL6C;if(*((
int*)_tmp91)!= 4)goto _LL6C;_tmp94=((struct Cyc_Absyn_Pat_b_struct*)_tmp91)->f1;
_LL6B: _tmp95=_tmp94;goto _LL6D;_LL6C: if(_tmp91 <= (void*)1)goto _LL6E;if(*((int*)
_tmp91)!= 3)goto _LL6E;_tmp95=((struct Cyc_Absyn_Local_b_struct*)_tmp91)->f1;_LL6D:
_tmp96=_tmp95;goto _LL6F;_LL6E: if(_tmp91 <= (void*)1)goto _LL63;if(*((int*)_tmp91)
!= 2)goto _LL63;_tmp96=((struct Cyc_Absyn_Param_b_struct*)_tmp91)->f1;_LL6F:{union
Cyc_Absyn_Nmspace_union _tmp578;(*q).f1=(union Cyc_Absyn_Nmspace_union)(((_tmp578.Loc_n).tag=
0,_tmp578));}return(void*)_tmp96->type;_LL63:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,
struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*));static
void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,
struct Cyc_Core_Opt*opt_args,struct _RegionHandle*temp,struct Cyc_List_List*(*
type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,
struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{void*_tmp9B=(void*)
fmt->r;union Cyc_Absyn_Cnst_union _tmp9C;struct _dyneither_ptr _tmp9D;struct Cyc_Absyn_Exp*
_tmp9E;struct Cyc_Absyn_Exp _tmp9F;void*_tmpA0;union Cyc_Absyn_Cnst_union _tmpA1;
struct _dyneither_ptr _tmpA2;_LL71: if(*((int*)_tmp9B)!= 0)goto _LL73;_tmp9C=((
struct Cyc_Absyn_Const_e_struct*)_tmp9B)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp9B)->f1).String_c).tag != 5)goto _LL73;_tmp9D=(_tmp9C.String_c).f1;_LL72:
_tmpA2=_tmp9D;goto _LL74;_LL73: if(*((int*)_tmp9B)!= 15)goto _LL75;_tmp9E=((struct
Cyc_Absyn_Cast_e_struct*)_tmp9B)->f2;_tmp9F=*_tmp9E;_tmpA0=(void*)_tmp9F.r;if(*((
int*)_tmpA0)!= 0)goto _LL75;_tmpA1=((struct Cyc_Absyn_Const_e_struct*)_tmpA0)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmpA0)->f1).String_c).tag != 5)goto _LL75;
_tmpA2=(_tmpA1.String_c).f1;_LL74: desc_types=type_getter(temp,te,(struct
_dyneither_ptr)_tmpA2,fmt->loc);goto _LL70;_LL75:;_LL76: if(opt_args != 0){struct
Cyc_List_List*_tmpA3=(struct Cyc_List_List*)opt_args->v;for(0;_tmpA3 != 0;_tmpA3=
_tmpA3->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmpA3->hd);{struct
_RegionHandle*_tmpA4=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpA4,(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)_tmpA3->hd)->topt))->v)
 && !Cyc_Tcutil_is_noalias_path(_tmpA4,(struct Cyc_Absyn_Exp*)_tmpA3->hd)){const
char*_tmp57B;void*_tmp57A;(_tmp57A=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpA3->hd)->loc,((_tmp57B="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp57B,sizeof(char),49))),_tag_dyneither(_tmp57A,sizeof(void*),0)));}}}}
return;_LL70:;}if(opt_args != 0){struct Cyc_List_List*_tmpA7=(struct Cyc_List_List*)
opt_args->v;for(0;desc_types != 0  && _tmpA7 != 0;(desc_types=desc_types->tl,_tmpA7=
_tmpA7->tl)){void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
_tmpA7->hd;Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){{
const char*_tmp580;void*_tmp57F[2];struct Cyc_String_pa_struct _tmp57E;struct Cyc_String_pa_struct
_tmp57D;(_tmp57D.tag=0,((_tmp57D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp57E.tag=0,((_tmp57E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp57F[0]=& _tmp57E,((_tmp57F[1]=& _tmp57D,Cyc_Tcutil_terr(e->loc,((_tmp580="descriptor has type \n%s\n but argument has type \n%s",
_tag_dyneither(_tmp580,sizeof(char),51))),_tag_dyneither(_tmp57F,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct _RegionHandle*_tmpAC=Cyc_Tcenv_get_fnrgn(te);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmpAC,t) && !Cyc_Tcutil_is_noalias_path(
_tmpAC,e)){const char*_tmp583;void*_tmp582;(_tmp582=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpA7->hd)->loc,((_tmp583="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp583,sizeof(char),49))),_tag_dyneither(_tmp582,sizeof(void*),0)));}}}
if(desc_types != 0){const char*_tmp586;void*_tmp585;(_tmp585=0,Cyc_Tcutil_terr(fmt->loc,((
_tmp586="too few arguments",_tag_dyneither(_tmp586,sizeof(char),18))),
_tag_dyneither(_tmp585,sizeof(void*),0)));}if(_tmpA7 != 0){const char*_tmp589;void*
_tmp588;(_tmp588=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpA7->hd)->loc,((
_tmp589="too many arguments",_tag_dyneither(_tmp589,sizeof(char),19))),
_tag_dyneither(_tmp588,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmpB3=p;_LL78: if((
int)_tmpB3 != 0)goto _LL7A;_LL79: goto _LL7B;_LL7A: if((int)_tmpB3 != 2)goto _LL7C;
_LL7B: if(!Cyc_Tcutil_is_numeric(e)){const char*_tmp58D;void*_tmp58C[1];struct Cyc_String_pa_struct
_tmp58B;(_tmp58B.tag=0,((_tmp58B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp58C[0]=& _tmp58B,Cyc_Tcutil_terr(loc,((_tmp58D="expecting arithmetic type but found %s",
_tag_dyneither(_tmp58D,sizeof(char),39))),_tag_dyneither(_tmp58C,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;_LL7C: if((int)_tmpB3
!= 11)goto _LL7E;_LL7D: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(
te,e)){const char*_tmp591;void*_tmp590[1];struct Cyc_String_pa_struct _tmp58F;(
_tmp58F.tag=0,((_tmp58F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp590[0]=& _tmp58F,Cyc_Tcutil_terr(loc,((_tmp591="expecting integral or * type but found %s",
_tag_dyneither(_tmp591,sizeof(char),42))),_tag_dyneither(_tmp590,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;_LL7E: if((int)_tmpB3 != 12)goto _LL80;_LL7F: if(!Cyc_Tcutil_is_integral(
e)){const char*_tmp595;void*_tmp594[1];struct Cyc_String_pa_struct _tmp593;(_tmp593.tag=
0,((_tmp593.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp594[0]=& _tmp593,Cyc_Tcutil_terr(loc,((_tmp595="expecting integral type but found %s",
_tag_dyneither(_tmp595,sizeof(char),37))),_tag_dyneither(_tmp594,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;_LL80: if((int)_tmpB3
!= 19)goto _LL82;_LL81:{void*_tmpBD=t;struct Cyc_Absyn_PtrInfo _tmpBE;struct Cyc_Absyn_PtrAtts
_tmpBF;struct Cyc_Absyn_Conref*_tmpC0;_LL85: if(_tmpBD <= (void*)4)goto _LL89;if(*((
int*)_tmpBD)!= 7)goto _LL87;_LL86: goto _LL84;_LL87: if(*((int*)_tmpBD)!= 4)goto
_LL89;_tmpBE=((struct Cyc_Absyn_PointerType_struct*)_tmpBD)->f1;_tmpBF=_tmpBE.ptr_atts;
_tmpC0=_tmpBF.bounds;_LL88:{union Cyc_Absyn_Constraint_union _tmpC1=(Cyc_Absyn_compress_conref(
_tmpC0))->v;void*_tmpC2;void*_tmpC3;_LL8C: if((_tmpC1.Eq_constr).tag != 0)goto
_LL8E;_tmpC2=(_tmpC1.Eq_constr).f1;if((int)_tmpC2 != 0)goto _LL8E;_LL8D: goto _LL8F;
_LL8E: if((_tmpC1.Eq_constr).tag != 0)goto _LL90;_tmpC3=(_tmpC1.Eq_constr).f1;if(
_tmpC3 <= (void*)1)goto _LL90;if(*((int*)_tmpC3)!= 0)goto _LL90;_LL8F: goto _LL8B;
_LL90:;_LL91: {const char*_tmp598;void*_tmp597;(_tmp597=0,Cyc_Tcutil_terr(loc,((
_tmp598="can't apply numelts to pointer/array of abstract length",_tag_dyneither(
_tmp598,sizeof(char),56))),_tag_dyneither(_tmp597,sizeof(void*),0)));}_LL8B:;}
goto _LL84;_LL89:;_LL8A: {const char*_tmp59C;void*_tmp59B[1];struct Cyc_String_pa_struct
_tmp59A;(_tmp59A.tag=0,((_tmp59A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp59B[0]=& _tmp59A,Cyc_Tcutil_terr(loc,((_tmp59C="numelts requires pointer or array type, not %s",
_tag_dyneither(_tmp59C,sizeof(char),47))),_tag_dyneither(_tmp59B,sizeof(void*),1)))))));}
_LL84:;}return Cyc_Absyn_uint_typ;_LL82:;_LL83: {const char*_tmp59F;void*_tmp59E;(
_tmp59E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp59F="Non-unary primop",_tag_dyneither(_tmp59F,sizeof(char),17))),
_tag_dyneither(_tmp59E,sizeof(void*),0)));}_LL77:;}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*));static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
if(!checker(e1)){{const char*_tmp5A3;void*_tmp5A2[1];struct Cyc_String_pa_struct
_tmp5A1;(_tmp5A1.tag=0,((_tmp5A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((
_tmp5A2[0]=& _tmp5A1,Cyc_Tcutil_terr(e1->loc,((_tmp5A3="type %s cannot be used here",
_tag_dyneither(_tmp5A3,sizeof(char),28))),_tag_dyneither(_tmp5A2,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}if(!checker(e2)){{
const char*_tmp5A7;void*_tmp5A6[1];struct Cyc_String_pa_struct _tmp5A5;(_tmp5A5.tag=
0,((_tmp5A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((_tmp5A6[0]=& _tmp5A5,
Cyc_Tcutil_terr(e2->loc,((_tmp5A7="type %s cannot be used here",_tag_dyneither(
_tmp5A7,sizeof(char),28))),_tag_dyneither(_tmp5A6,sizeof(void*),1)))))));}return
Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);return Cyc_Tcutil_max_arithmetic_type(
t1,t2);}}static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);void*_tmpD1=t1;struct Cyc_Absyn_PtrInfo
_tmpD2;void*_tmpD3;struct Cyc_Absyn_Tqual _tmpD4;struct Cyc_Absyn_PtrAtts _tmpD5;
void*_tmpD6;struct Cyc_Absyn_Conref*_tmpD7;struct Cyc_Absyn_Conref*_tmpD8;struct
Cyc_Absyn_Conref*_tmpD9;_LL93: if(_tmpD1 <= (void*)4)goto _LL95;if(*((int*)_tmpD1)
!= 4)goto _LL95;_tmpD2=((struct Cyc_Absyn_PointerType_struct*)_tmpD1)->f1;_tmpD3=(
void*)_tmpD2.elt_typ;_tmpD4=_tmpD2.elt_tq;_tmpD5=_tmpD2.ptr_atts;_tmpD6=(void*)
_tmpD5.rgn;_tmpD7=_tmpD5.nullable;_tmpD8=_tmpD5.bounds;_tmpD9=_tmpD5.zero_term;
_LL94: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpD3),(void*)1)){const char*
_tmp5AA;void*_tmp5A9;(_tmp5A9=0,Cyc_Tcutil_terr(e1->loc,((_tmp5AA="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp5AA,sizeof(char),50))),_tag_dyneither(_tmp5A9,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp5AD;void*_tmp5AC;(_tmp5AC=0,
Cyc_Tcutil_terr(e1->loc,((_tmp5AD="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp5AD,sizeof(char),54))),_tag_dyneither(_tmp5AC,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){const char*_tmp5B1;void*_tmp5B0[1];struct
Cyc_String_pa_struct _tmp5AF;(_tmp5AF.tag=0,((_tmp5AF.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp5B0[0]=& _tmp5AF,Cyc_Tcutil_terr(
e2->loc,((_tmp5B1="expecting int but found %s",_tag_dyneither(_tmp5B1,sizeof(
char),27))),_tag_dyneither(_tmp5B0,sizeof(void*),1)))))));}_tmpD8=Cyc_Absyn_compress_conref(
_tmpD8);{union Cyc_Absyn_Constraint_union _tmpE1=_tmpD8->v;void*_tmpE2;void*_tmpE3;
struct Cyc_Absyn_Exp*_tmpE4;_LL98: if((_tmpE1.Eq_constr).tag != 0)goto _LL9A;_tmpE2=(
_tmpE1.Eq_constr).f1;if((int)_tmpE2 != 0)goto _LL9A;_LL99: return t1;_LL9A: if((
_tmpE1.Eq_constr).tag != 0)goto _LL9C;_tmpE3=(_tmpE1.Eq_constr).f1;if(_tmpE3 <= (
void*)1)goto _LL9C;if(*((int*)_tmpE3)!= 0)goto _LL9C;_tmpE4=((struct Cyc_Absyn_Upper_b_struct*)
_tmpE3)->f1;_LL9B: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpD9)){const char*_tmp5B4;void*_tmp5B3;(_tmp5B3=0,Cyc_Tcutil_warn(e1->loc,((
_tmp5B4="pointer arithmetic on thin, zero-terminated pointer may be expensive.",
_tag_dyneither(_tmp5B4,sizeof(char),70))),_tag_dyneither(_tmp5B3,sizeof(void*),0)));}{
struct Cyc_Absyn_PointerType_struct _tmp5BE;struct Cyc_Absyn_PtrAtts _tmp5BD;struct
Cyc_Absyn_PtrInfo _tmp5BC;struct Cyc_Absyn_PointerType_struct*_tmp5BB;struct Cyc_Absyn_PointerType_struct*
_tmpE7=(_tmp5BB=_cycalloc(sizeof(*_tmp5BB)),((_tmp5BB[0]=((_tmp5BE.tag=4,((
_tmp5BE.f1=((_tmp5BC.elt_typ=(void*)_tmpD3,((_tmp5BC.elt_tq=_tmpD4,((_tmp5BC.ptr_atts=((
_tmp5BD.rgn=(void*)_tmpD6,((_tmp5BD.nullable=Cyc_Absyn_true_conref,((_tmp5BD.bounds=
Cyc_Absyn_bounds_dyneither_conref,((_tmp5BD.zero_term=_tmpD9,((_tmp5BD.ptrloc=0,
_tmp5BD)))))))))),_tmp5BC)))))),_tmp5BE)))),_tmp5BB)));Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmpE7,(void*)3);return(void*)_tmpE7;}_LL9C:;_LL9D:{union Cyc_Absyn_Constraint_union
_tmp5BF;_tmpD8->v=(union Cyc_Absyn_Constraint_union)(((_tmp5BF.Eq_constr).tag=0,(((
_tmp5BF.Eq_constr).f1=(void*)((void*)0),_tmp5BF))));}return t1;_LL97:;}_LL95:;
_LL96: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LL92:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;
void*t1_elt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){if(!Cyc_Tcutil_unify(t1,t2)){{const char*_tmp5C4;void*_tmp5C3[2];struct Cyc_String_pa_struct
_tmp5C2;struct Cyc_String_pa_struct _tmp5C1;(_tmp5C1.tag=0,((_tmp5C1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v)),((_tmp5C2.tag=0,((_tmp5C2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp5C3[0]=& _tmp5C2,((_tmp5C3[1]=& _tmp5C1,Cyc_Tcutil_terr(
e1->loc,((_tmp5C4="pointer arithmetic on values of different types (%s != %s)",
_tag_dyneither(_tmp5C4,sizeof(char),59))),_tag_dyneither(_tmp5C3,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),(void*)1)){const char*_tmp5C7;void*_tmp5C6;(_tmp5C6=0,
Cyc_Tcutil_terr(e1->loc,((_tmp5C7="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp5C7,sizeof(char),50))),_tag_dyneither(_tmp5C6,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp5CA;void*_tmp5C9;(_tmp5C9=0,
Cyc_Tcutil_terr(e1->loc,((_tmp5CA="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp5CA,sizeof(char),54))),_tag_dyneither(_tmp5C9,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){{const char*_tmp5CF;void*_tmp5CE[2];struct
Cyc_String_pa_struct _tmp5CD;struct Cyc_String_pa_struct _tmp5CC;(_tmp5CC.tag=0,((
_tmp5CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp5CD.tag=0,((_tmp5CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp5CE[0]=& _tmp5CD,((_tmp5CE[1]=& _tmp5CC,Cyc_Tcutil_terr(
e2->loc,((_tmp5CF="expecting either %s or int but found %s",_tag_dyneither(
_tmp5CF,sizeof(char),40))),_tag_dyneither(_tmp5CE,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t1;}}if(Cyc_Tcutil_is_pointer_type(t1))Cyc_Tcutil_unchecked_cast(
te,e1,Cyc_Absyn_sint_typ,(void*)3);if(Cyc_Tcutil_is_pointer_type(t2))Cyc_Tcutil_unchecked_cast(
te,e2,Cyc_Absyn_sint_typ,(void*)3);return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcAnyBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(e1);int e2_is_num=Cyc_Tcutil_is_numeric(
e2);void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(e1_is_num  && e2_is_num)return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_typ_kind(
t1)== (void*)2  || Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_bk,Cyc_Tcenv_lookup_opt_type_vars(te)))){if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(
te,e2,t1,(void*)3);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_silent_castable(
te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e1,t2,(void*)3);return Cyc_Absyn_sint_typ;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{goto pointer_cmp;}}}}}else{pointer_cmp: {struct
_tuple0 _tmp5D0;struct _tuple0 _tmpFA=(_tmp5D0.f1=Cyc_Tcutil_compress(t1),((_tmp5D0.f2=
Cyc_Tcutil_compress(t2),_tmp5D0)));void*_tmpFB;struct Cyc_Absyn_PtrInfo _tmpFC;
void*_tmpFD;void*_tmpFE;struct Cyc_Absyn_PtrInfo _tmpFF;void*_tmp100;_LL9F: _tmpFB=
_tmpFA.f1;if(_tmpFB <= (void*)4)goto _LLA1;if(*((int*)_tmpFB)!= 4)goto _LLA1;_tmpFC=((
struct Cyc_Absyn_PointerType_struct*)_tmpFB)->f1;_tmpFD=(void*)_tmpFC.elt_typ;
_tmpFE=_tmpFA.f2;if(_tmpFE <= (void*)4)goto _LLA1;if(*((int*)_tmpFE)!= 4)goto _LLA1;
_tmpFF=((struct Cyc_Absyn_PointerType_struct*)_tmpFE)->f1;_tmp100=(void*)_tmpFF.elt_typ;
_LLA0: if(Cyc_Tcutil_unify(_tmpFD,_tmp100))return Cyc_Absyn_sint_typ;goto _LL9E;
_LLA1:;_LLA2: goto _LL9E;_LL9E:;}{const char*_tmp5D5;void*_tmp5D4[2];struct Cyc_String_pa_struct
_tmp5D3;struct Cyc_String_pa_struct _tmp5D2;(_tmp5D2.tag=0,((_tmp5D2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp5D3.tag=
0,((_tmp5D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp5D4[0]=& _tmp5D3,((_tmp5D4[1]=& _tmp5D2,Cyc_Tcutil_terr(loc,((_tmp5D5="comparison not allowed between %s and %s",
_tag_dyneither(_tmp5D5,sizeof(char),41))),_tag_dyneither(_tmp5D4,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*
Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp105=p;
_LLA4: if((int)_tmp105 != 0)goto _LLA6;_LLA5: return Cyc_Tcexp_tcPlus(te,e1,e2);_LLA6:
if((int)_tmp105 != 2)goto _LLA8;_LLA7: return Cyc_Tcexp_tcMinus(te,e1,e2);_LLA8: if((
int)_tmp105 != 1)goto _LLAA;_LLA9: goto _LLAB;_LLAA: if((int)_tmp105 != 3)goto _LLAC;
_LLAB: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LLAC: if((int)
_tmp105 != 4)goto _LLAE;_LLAD: goto _LLAF;_LLAE: if((int)_tmp105 != 13)goto _LLB0;_LLAF:
goto _LLB1;_LLB0: if((int)_tmp105 != 14)goto _LLB2;_LLB1: goto _LLB3;_LLB2: if((int)
_tmp105 != 15)goto _LLB4;_LLB3: goto _LLB5;_LLB4: if((int)_tmp105 != 16)goto _LLB6;
_LLB5: goto _LLB7;_LLB6: if((int)_tmp105 != 17)goto _LLB8;_LLB7: goto _LLB9;_LLB8: if((
int)_tmp105 != 18)goto _LLBA;_LLB9: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);
_LLBA: if((int)_tmp105 != 5)goto _LLBC;_LLBB: goto _LLBD;_LLBC: if((int)_tmp105 != 6)
goto _LLBE;_LLBD: goto _LLBF;_LLBE: if((int)_tmp105 != 7)goto _LLC0;_LLBF: goto _LLC1;
_LLC0: if((int)_tmp105 != 8)goto _LLC2;_LLC1: goto _LLC3;_LLC2: if((int)_tmp105 != 9)
goto _LLC4;_LLC3: goto _LLC5;_LLC4: if((int)_tmp105 != 10)goto _LLC6;_LLC5: return Cyc_Tcexp_tcAnyBinop(
te,loc,e1,e2);_LLC6:;_LLC7: {const char*_tmp5D8;void*_tmp5D7;(_tmp5D7=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5D8="bad binary primop",
_tag_dyneither(_tmp5D8,sizeof(char),18))),_tag_dyneither(_tmp5D7,sizeof(void*),0)));}
_LLA3:;}static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_List_List*es);static void*Cyc_Tcexp_tcPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_List_List*es){if(p == (void*)2  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
es)== 1)return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd);Cyc_Tcexp_tcExpList(te,es);{void*t;switch(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLC8: {const char*_tmp5DB;void*
_tmp5DA;return(_tmp5DA=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp5DB="primitive operator has 0 arguments",
_tag_dyneither(_tmp5DB,sizeof(char),35))),_tag_dyneither(_tmp5DA,sizeof(void*),0)));}
case 1: _LLC9: t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLCA: t=Cyc_Tcexp_tcBinPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(es))->tl))->hd);break;default: _LLCB: {const char*_tmp5DE;void*_tmp5DD;
return(_tmp5DD=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp5DE="primitive operator has > 2 arguments",
_tag_dyneither(_tmp5DE,sizeof(char),37))),_tag_dyneither(_tmp5DD,sizeof(void*),0)));}}
return t;}}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmp10C=(void*)e->r;void*
_tmp10D;struct Cyc_Absyn_Vardecl*_tmp10E;void*_tmp10F;struct Cyc_Absyn_Vardecl*
_tmp110;void*_tmp111;struct Cyc_Absyn_Vardecl*_tmp112;void*_tmp113;struct Cyc_Absyn_Vardecl*
_tmp114;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*
_tmp117;struct _dyneither_ptr*_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct
_dyneither_ptr*_tmp11A;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11C;
struct Cyc_Absyn_Exp*_tmp11D;_LLCE: if(*((int*)_tmp10C)!= 1)goto _LLD0;_tmp10D=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10C)->f2;if(_tmp10D <= (void*)1)goto
_LLD0;if(*((int*)_tmp10D)!= 2)goto _LLD0;_tmp10E=((struct Cyc_Absyn_Param_b_struct*)
_tmp10D)->f1;_LLCF: _tmp110=_tmp10E;goto _LLD1;_LLD0: if(*((int*)_tmp10C)!= 1)goto
_LLD2;_tmp10F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10C)->f2;if(_tmp10F <= (
void*)1)goto _LLD2;if(*((int*)_tmp10F)!= 3)goto _LLD2;_tmp110=((struct Cyc_Absyn_Local_b_struct*)
_tmp10F)->f1;_LLD1: _tmp112=_tmp110;goto _LLD3;_LLD2: if(*((int*)_tmp10C)!= 1)goto
_LLD4;_tmp111=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10C)->f2;if(_tmp111 <= (
void*)1)goto _LLD4;if(*((int*)_tmp111)!= 4)goto _LLD4;_tmp112=((struct Cyc_Absyn_Pat_b_struct*)
_tmp111)->f1;_LLD3: _tmp114=_tmp112;goto _LLD5;_LLD4: if(*((int*)_tmp10C)!= 1)goto
_LLD6;_tmp113=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10C)->f2;if(_tmp113 <= (
void*)1)goto _LLD6;if(*((int*)_tmp113)!= 0)goto _LLD6;_tmp114=((struct Cyc_Absyn_Global_b_struct*)
_tmp113)->f1;_LLD5: if(!(_tmp114->tq).real_const)return;goto _LLCD;_LLD6: if(*((int*)
_tmp10C)!= 25)goto _LLD8;_tmp115=((struct Cyc_Absyn_Subscript_e_struct*)_tmp10C)->f1;
_tmp116=((struct Cyc_Absyn_Subscript_e_struct*)_tmp10C)->f2;_LLD7:{void*_tmp11E=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp115->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp11F;struct Cyc_Absyn_Tqual _tmp120;struct Cyc_Absyn_ArrayInfo
_tmp121;struct Cyc_Absyn_Tqual _tmp122;struct Cyc_List_List*_tmp123;_LLE5: if(
_tmp11E <= (void*)4)goto _LLEB;if(*((int*)_tmp11E)!= 4)goto _LLE7;_tmp11F=((struct
Cyc_Absyn_PointerType_struct*)_tmp11E)->f1;_tmp120=_tmp11F.elt_tq;_LLE6: _tmp122=
_tmp120;goto _LLE8;_LLE7: if(*((int*)_tmp11E)!= 7)goto _LLE9;_tmp121=((struct Cyc_Absyn_ArrayType_struct*)
_tmp11E)->f1;_tmp122=_tmp121.tq;_LLE8: if(!_tmp122.real_const)return;goto _LLE4;
_LLE9: if(*((int*)_tmp11E)!= 9)goto _LLEB;_tmp123=((struct Cyc_Absyn_TupleType_struct*)
_tmp11E)->f1;_LLEA: {unsigned int _tmp125;int _tmp126;struct _tuple7 _tmp124=Cyc_Evexp_eval_const_uint_exp(
_tmp116);_tmp125=_tmp124.f1;_tmp126=_tmp124.f2;if(!_tmp126){{const char*_tmp5E1;
void*_tmp5E0;(_tmp5E0=0,Cyc_Tcutil_terr(e->loc,((_tmp5E1="tuple projection cannot use sizeof or offsetof",
_tag_dyneither(_tmp5E1,sizeof(char),47))),_tag_dyneither(_tmp5E0,sizeof(void*),0)));}
return;}{struct _handler_cons _tmp129;_push_handler(& _tmp129);{int _tmp12B=0;if(
setjmp(_tmp129.handler))_tmp12B=1;if(!_tmp12B){{struct _tuple9 _tmp12D;struct Cyc_Absyn_Tqual
_tmp12E;struct _tuple9*_tmp12C=((struct _tuple9*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(_tmp123,(int)_tmp125);_tmp12D=*_tmp12C;_tmp12E=_tmp12D.f1;if(!
_tmp12E.real_const){_npop_handler(0);return;}};_pop_handler();}else{void*_tmp12A=(
void*)_exn_thrown;void*_tmp130=_tmp12A;_LLEE: if(_tmp130 != Cyc_List_Nth)goto _LLF0;
_LLEF: return;_LLF0:;_LLF1:(void)_throw(_tmp130);_LLED:;}}}goto _LLE4;}_LLEB:;
_LLEC: goto _LLE4;_LLE4:;}goto _LLCD;_LLD8: if(*((int*)_tmp10C)!= 23)goto _LLDA;
_tmp117=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp10C)->f1;_tmp118=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp10C)->f2;_LLD9:{void*_tmp131=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp117->topt))->v);struct Cyc_Absyn_AggrInfo _tmp132;union Cyc_Absyn_AggrInfoU_union
_tmp133;struct Cyc_Absyn_Aggrdecl**_tmp134;struct Cyc_List_List*_tmp135;_LLF3: if(
_tmp131 <= (void*)4)goto _LLF7;if(*((int*)_tmp131)!= 10)goto _LLF5;_tmp132=((struct
Cyc_Absyn_AggrType_struct*)_tmp131)->f1;_tmp133=_tmp132.aggr_info;if((((((struct
Cyc_Absyn_AggrType_struct*)_tmp131)->f1).aggr_info).KnownAggr).tag != 1)goto _LLF5;
_tmp134=(_tmp133.KnownAggr).f1;_LLF4: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp134 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp134,_tmp118);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLF2;}_LLF5: if(*((int*)_tmp131)!= 11)goto _LLF7;_tmp135=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp131)->f2;_LLF6: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp135,
_tmp118);if(sf == 0  || !(sf->tq).real_const)return;goto _LLF2;}_LLF7:;_LLF8: goto
_LLF2;_LLF2:;}goto _LLCD;_LLDA: if(*((int*)_tmp10C)!= 24)goto _LLDC;_tmp119=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp10C)->f1;_tmp11A=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp10C)->f2;_LLDB:{void*_tmp136=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp119->topt))->v);struct Cyc_Absyn_PtrInfo _tmp137;void*_tmp138;
_LLFA: if(_tmp136 <= (void*)4)goto _LLFC;if(*((int*)_tmp136)!= 4)goto _LLFC;_tmp137=((
struct Cyc_Absyn_PointerType_struct*)_tmp136)->f1;_tmp138=(void*)_tmp137.elt_typ;
_LLFB:{void*_tmp139=Cyc_Tcutil_compress(_tmp138);struct Cyc_Absyn_AggrInfo _tmp13A;
union Cyc_Absyn_AggrInfoU_union _tmp13B;struct Cyc_Absyn_Aggrdecl**_tmp13C;struct
Cyc_List_List*_tmp13D;_LLFF: if(_tmp139 <= (void*)4)goto _LL103;if(*((int*)_tmp139)
!= 10)goto _LL101;_tmp13A=((struct Cyc_Absyn_AggrType_struct*)_tmp139)->f1;_tmp13B=
_tmp13A.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp139)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL101;_tmp13C=(_tmp13B.KnownAggr).f1;_LL100: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp13C == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp13C,_tmp11A);if(sf == 0  || !(sf->tq).real_const)return;goto _LLFE;}_LL101: if(*((
int*)_tmp139)!= 11)goto _LL103;_tmp13D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp139)->f2;_LL102: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(
_tmp13D,_tmp11A);if(sf == 0  || !(sf->tq).real_const)return;goto _LLFE;}_LL103:;
_LL104: goto _LLFE;_LLFE:;}goto _LLF9;_LLFC:;_LLFD: goto _LLF9;_LLF9:;}goto _LLCD;
_LLDC: if(*((int*)_tmp10C)!= 22)goto _LLDE;_tmp11B=((struct Cyc_Absyn_Deref_e_struct*)
_tmp10C)->f1;_LLDD:{void*_tmp13E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp11B->topt))->v);struct Cyc_Absyn_PtrInfo _tmp13F;struct Cyc_Absyn_Tqual
_tmp140;struct Cyc_Absyn_ArrayInfo _tmp141;struct Cyc_Absyn_Tqual _tmp142;_LL106: if(
_tmp13E <= (void*)4)goto _LL10A;if(*((int*)_tmp13E)!= 4)goto _LL108;_tmp13F=((
struct Cyc_Absyn_PointerType_struct*)_tmp13E)->f1;_tmp140=_tmp13F.elt_tq;_LL107:
_tmp142=_tmp140;goto _LL109;_LL108: if(*((int*)_tmp13E)!= 7)goto _LL10A;_tmp141=((
struct Cyc_Absyn_ArrayType_struct*)_tmp13E)->f1;_tmp142=_tmp141.tq;_LL109: if(!
_tmp142.real_const)return;goto _LL105;_LL10A:;_LL10B: goto _LL105;_LL105:;}goto
_LLCD;_LLDE: if(*((int*)_tmp10C)!= 13)goto _LLE0;_tmp11C=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp10C)->f1;_LLDF: _tmp11D=_tmp11C;goto _LLE1;_LLE0: if(*((int*)_tmp10C)!= 14)goto
_LLE2;_tmp11D=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp10C)->f1;_LLE1: Cyc_Tcexp_check_writable(
te,_tmp11D);return;_LLE2:;_LLE3: goto _LLCD;_LLCD:;}{const char*_tmp5E5;void*
_tmp5E4[1];struct Cyc_String_pa_struct _tmp5E3;(_tmp5E3.tag=0,((_tmp5E3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp5E4[0]=&
_tmp5E3,Cyc_Tcutil_terr(e->loc,((_tmp5E5="attempt to write a const location: %s",
_tag_dyneither(_tmp5E5,sizeof(char),38))),_tag_dyneither(_tmp5E4,sizeof(void*),1)))))));}}
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,void*i);static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,void*i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(e)){const char*
_tmp5E8;void*_tmp5E7;return(_tmp5E7=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp5E8="increment/decrement of non-lvalue",
_tag_dyneither(_tmp5E8,sizeof(char),34))),_tag_dyneither(_tmp5E7,sizeof(void*),0)));}
Cyc_Tcexp_check_writable(te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v;if(!Cyc_Tcutil_is_numeric(e)){void*telt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == (void*)0  || i == (
void*)1)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(void*)1)){const char*
_tmp5EB;void*_tmp5EA;(_tmp5EA=0,Cyc_Tcutil_terr(e->loc,((_tmp5EB="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp5EB,sizeof(char),50))),_tag_dyneither(_tmp5EA,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t)){const char*_tmp5EE;void*_tmp5ED;(_tmp5ED=0,
Cyc_Tcutil_terr(e->loc,((_tmp5EE="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp5EE,sizeof(char),54))),_tag_dyneither(_tmp5ED,sizeof(void*),0)));}}
else{const char*_tmp5F2;void*_tmp5F1[1];struct Cyc_String_pa_struct _tmp5F0;(
_tmp5F0.tag=0,((_tmp5F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp5F1[0]=& _tmp5F0,Cyc_Tcutil_terr(e->loc,((_tmp5F2="expecting arithmetic or ? type but found %s",
_tag_dyneither(_tmp5F2,sizeof(char),44))),_tag_dyneither(_tmp5F1,sizeof(void*),1)))))));}}
return t;}}static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3);static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3){{const char*_tmp5F3;Cyc_Tcexp_tcTest(te,e1,((_tmp5F3="conditional expression",
_tag_dyneither(_tmp5F3,sizeof(char),23))));}Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(
te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(
te));struct Cyc_List_List _tmp5F4;struct Cyc_List_List _tmp150=(_tmp5F4.hd=e3,((
_tmp5F4.tl=0,_tmp5F4)));struct Cyc_List_List _tmp5F5;struct Cyc_List_List _tmp151=(
_tmp5F5.hd=e2,((_tmp5F5.tl=(struct Cyc_List_List*)& _tmp150,_tmp5F5)));if(!Cyc_Tcutil_coerce_list(
te,t,(struct Cyc_List_List*)& _tmp151)){{const char*_tmp5FA;void*_tmp5F9[2];struct
Cyc_String_pa_struct _tmp5F8;struct Cyc_String_pa_struct _tmp5F7;(_tmp5F7.tag=0,((
_tmp5F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v)),((_tmp5F8.tag=0,((
_tmp5F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((_tmp5F9[0]=& _tmp5F8,((
_tmp5F9[1]=& _tmp5F7,Cyc_Tcutil_terr(loc,((_tmp5FA="conditional clause types do not match: %s != %s",
_tag_dyneither(_tmp5FA,sizeof(char),48))),_tag_dyneither(_tmp5F9,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t;}}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{const char*_tmp5FB;Cyc_Tcexp_tcTest(
te,e1,((_tmp5FB="logical-and expression",_tag_dyneither(_tmp5FB,sizeof(char),23))));}{
const char*_tmp5FC;Cyc_Tcexp_tcTest(te,e2,((_tmp5FC="logical-and expression",
_tag_dyneither(_tmp5FC,sizeof(char),23))));}return Cyc_Absyn_sint_typ;}static void*
Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp5FD;Cyc_Tcexp_tcTest(te,e1,((_tmp5FD="logical-or expression",
_tag_dyneither(_tmp5FD,sizeof(char),22))));}{const char*_tmp5FE;Cyc_Tcexp_tcTest(
te,e2,((_tmp5FE="logical-or expression",_tag_dyneither(_tmp5FE,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp15C=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(_tmp15C,te),0,e1);Cyc_Tcexp_tcExp(
te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),
e2);{void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{void*_tmp15D=Cyc_Tcutil_compress(
t1);_LL10D: if(_tmp15D <= (void*)4)goto _LL10F;if(*((int*)_tmp15D)!= 7)goto _LL10F;
_LL10E:{const char*_tmp601;void*_tmp600;(_tmp600=0,Cyc_Tcutil_terr(loc,((_tmp601="cannot assign to an array",
_tag_dyneither(_tmp601,sizeof(char),26))),_tag_dyneither(_tmp600,sizeof(void*),0)));}
goto _LL10C;_LL10F:;_LL110: goto _LL10C;_LL10C:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),(void*)1)){const char*_tmp604;void*_tmp603;(_tmp603=0,Cyc_Tcutil_terr(loc,((
_tmp604="type is abstract (can't determine size).",_tag_dyneither(_tmp604,
sizeof(char),41))),_tag_dyneither(_tmp603,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(
e1)){const char*_tmp607;void*_tmp606;return(_tmp606=0,Cyc_Tcexp_expr_err(te,loc,
topt,((_tmp607="assignment to non-lvalue",_tag_dyneither(_tmp607,sizeof(char),25))),
_tag_dyneither(_tmp606,sizeof(void*),0)));}Cyc_Tcexp_check_writable(te,e1);if(po
== 0){struct _RegionHandle*_tmp164=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp164,t2) && !Cyc_Tcutil_is_noalias_path(_tmp164,e2)){const char*_tmp60A;void*
_tmp609;(_tmp609=0,Cyc_Tcutil_terr(e2->loc,((_tmp60A="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp60A,sizeof(char),49))),_tag_dyneither(_tmp609,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){const char*_tmp60F;void*_tmp60E[2];struct
Cyc_String_pa_struct _tmp60D;struct Cyc_String_pa_struct _tmp60C;void*_tmp167=(
_tmp60C.tag=0,((_tmp60C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp60D.tag=0,((_tmp60D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp60E[0]=& _tmp60D,((_tmp60E[1]=& _tmp60C,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp60F="type mismatch: %s != %s",_tag_dyneither(_tmp60F,sizeof(
char),24))),_tag_dyneither(_tmp60E,sizeof(void*),2)))))))))))));Cyc_Tcutil_unify(
t1,t2);Cyc_Tcutil_explain_failure();return _tmp167;}}else{void*_tmp16C=(void*)po->v;
void*_tmp16D=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp16C,e1,e2);if(!(Cyc_Tcutil_unify(
_tmp16D,t1) || Cyc_Tcutil_coerceable(_tmp16D) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp614;void*_tmp613[2];struct Cyc_String_pa_struct _tmp612;struct Cyc_String_pa_struct
_tmp611;void*_tmp16E=(_tmp611.tag=0,((_tmp611.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp612.tag=0,((_tmp612.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp613[0]=&
_tmp612,((_tmp613[1]=& _tmp611,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp614="Cannot use this operator in an assignment when the arguments have types %s and %s",
_tag_dyneither(_tmp614,sizeof(char),82))),_tag_dyneither(_tmp613,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(_tmp16D,t1);Cyc_Tcutil_explain_failure();return _tmp16E;}return
_tmp16D;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,
0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct Cyc_Core_Opt*)_check_null(
e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,
struct Cyc_List_List*fs);static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={6,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp173=Cyc_Tcutil_compress(
t1);void*_tmp174;void*_tmp175;_LL112: if((int)_tmp173 != 1)goto _LL114;_LL113: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ,(void*)1);t1=dbl_typ;goto _LL111;_LL114: if(_tmp173 <= (void*)4)goto
_LL118;if(*((int*)_tmp173)!= 5)goto _LL116;_tmp174=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp173)->f2;if((int)_tmp174 != 0)goto _LL116;_LL115: goto _LL117;_LL116: if(*((int*)
_tmp173)!= 5)goto _LL118;_tmp175=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp173)->f2;
if((int)_tmp175 != 1)goto _LL118;_LL117: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);t1=Cyc_Absyn_sint_typ;goto _LL111;_LL118:;_LL119: goto _LL111;_LL111:;}
for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp177;struct Cyc_List_List*
_tmp178;struct Cyc_Position_Segment*_tmp179;void*_tmp17A;struct Cyc_Absyn_Tunionfield
_tmp176=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_tmp177=_tmp176.name;_tmp178=
_tmp176.typs;_tmp179=_tmp176.loc;_tmp17A=(void*)_tmp176.sc;if(_tmp178 == 0  || 
_tmp178->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple9*)_tmp178->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)fields->hd);}}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp17C;struct Cyc_List_List*_tmp17D;struct Cyc_Position_Segment*
_tmp17E;void*_tmp17F;struct Cyc_Absyn_Tunionfield _tmp17B=*((struct Cyc_Absyn_Tunionfield*)
fields->hd);_tmp17C=_tmp17B.name;_tmp17D=_tmp17B.typs;_tmp17E=_tmp17B.loc;
_tmp17F=(void*)_tmp17B.sc;if(_tmp17D == 0  || _tmp17D->tl != 0)continue;{void*t2=
Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple9*)_tmp17D->hd)).f2);if(Cyc_Tcutil_coerce_arg(
te,e,t2))return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)
fields->hd);}}{const char*_tmp619;void*_tmp618[2];struct Cyc_String_pa_struct
_tmp617;struct Cyc_String_pa_struct _tmp616;(_tmp616.tag=0,((_tmp616.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp617.tag=
0,((_tmp617.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
tu)),((_tmp618[0]=& _tmp617,((_tmp618[1]=& _tmp616,Cyc_Tcutil_terr(e->loc,((
_tmp619="can't find a field in %s to inject a value of type %s",_tag_dyneither(
_tmp619,sizeof(char),54))),_tag_dyneither(_tmp618,sizeof(void*),2)))))))))))));}
return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**
vararg_call_info);static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){struct Cyc_List_List*
_tmp185=args;struct _RegionHandle*_tmp186=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*
_tmp187=Cyc_Tcenv_new_block(_tmp186,loc,te_orig);Cyc_Tcexp_tcExp(_tmp187,0,e);{
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp188=t;struct Cyc_Absyn_PtrInfo _tmp189;void*_tmp18A;struct Cyc_Absyn_Tqual
_tmp18B;struct Cyc_Absyn_PtrAtts _tmp18C;void*_tmp18D;struct Cyc_Absyn_Conref*
_tmp18E;struct Cyc_Absyn_Conref*_tmp18F;struct Cyc_Absyn_Conref*_tmp190;_LL11B: if(
_tmp188 <= (void*)4)goto _LL11D;if(*((int*)_tmp188)!= 4)goto _LL11D;_tmp189=((
struct Cyc_Absyn_PointerType_struct*)_tmp188)->f1;_tmp18A=(void*)_tmp189.elt_typ;
_tmp18B=_tmp189.elt_tq;_tmp18C=_tmp189.ptr_atts;_tmp18D=(void*)_tmp18C.rgn;
_tmp18E=_tmp18C.nullable;_tmp18F=_tmp18C.bounds;_tmp190=_tmp18C.zero_term;_LL11C:
Cyc_Tcenv_check_rgn_accessible(_tmp187,loc,_tmp18D);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp18F);{void*_tmp191=Cyc_Tcutil_compress(_tmp18A);struct Cyc_Absyn_FnInfo
_tmp192;struct Cyc_List_List*_tmp193;struct Cyc_Core_Opt*_tmp194;void*_tmp195;
struct Cyc_List_List*_tmp196;int _tmp197;struct Cyc_Absyn_VarargInfo*_tmp198;struct
Cyc_List_List*_tmp199;struct Cyc_List_List*_tmp19A;_LL120: if(_tmp191 <= (void*)4)
goto _LL122;if(*((int*)_tmp191)!= 8)goto _LL122;_tmp192=((struct Cyc_Absyn_FnType_struct*)
_tmp191)->f1;_tmp193=_tmp192.tvars;_tmp194=_tmp192.effect;_tmp195=(void*)_tmp192.ret_typ;
_tmp196=_tmp192.args;_tmp197=_tmp192.c_varargs;_tmp198=_tmp192.cyc_varargs;
_tmp199=_tmp192.rgn_po;_tmp19A=_tmp192.attributes;_LL121: if(topt != 0)Cyc_Tcutil_unify(
_tmp195,*topt);while(_tmp185 != 0  && _tmp196 != 0){struct Cyc_Absyn_Exp*e1=(struct
Cyc_Absyn_Exp*)_tmp185->hd;void*t2=(*((struct _tuple2*)_tmp196->hd)).f3;Cyc_Tcexp_tcExp(
_tmp187,(void**)& t2,e1);if(!Cyc_Tcutil_coerce_arg(_tmp187,e1,t2)){{const char*
_tmp61E;void*_tmp61D[2];struct Cyc_String_pa_struct _tmp61C;struct Cyc_String_pa_struct
_tmp61B;(_tmp61B.tag=0,((_tmp61B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp61C.tag=0,((_tmp61C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp61D[0]=& _tmp61C,((_tmp61D[1]=& _tmp61B,Cyc_Tcutil_terr(
e1->loc,((_tmp61E="actual argument has type \n\t%s\n but formal has type \n\t%s.",
_tag_dyneither(_tmp61E,sizeof(char),57))),_tag_dyneither(_tmp61D,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp186,t2) && !Cyc_Tcutil_is_noalias_path(
_tmp186,e1)){const char*_tmp621;void*_tmp620;(_tmp620=0,Cyc_Tcutil_terr(e1->loc,((
_tmp621="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp621,sizeof(char),49))),_tag_dyneither(_tmp620,sizeof(void*),0)));}_tmp185=
_tmp185->tl;_tmp196=_tmp196->tl;}{int args_already_checked=0;{struct Cyc_List_List*
a=_tmp19A;for(0;a != 0;a=a->tl){void*_tmp1A1=(void*)a->hd;void*_tmp1A2;int _tmp1A3;
int _tmp1A4;_LL125: if(_tmp1A1 <= (void*)17)goto _LL127;if(*((int*)_tmp1A1)!= 3)goto
_LL127;_tmp1A2=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp1A1)->f1;_tmp1A3=((
struct Cyc_Absyn_Format_att_struct*)_tmp1A1)->f2;_tmp1A4=((struct Cyc_Absyn_Format_att_struct*)
_tmp1A1)->f3;_LL126:{struct _handler_cons _tmp1A5;_push_handler(& _tmp1A5);{int
_tmp1A7=0;if(setjmp(_tmp1A5.handler))_tmp1A7=1;if(!_tmp1A7){{struct Cyc_Absyn_Exp*
_tmp1A8=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,
_tmp1A3 - 1);struct Cyc_Core_Opt*fmt_args;if(_tmp1A4 == 0)fmt_args=0;else{struct Cyc_Core_Opt*
_tmp622;fmt_args=((_tmp622=_cycalloc(sizeof(*_tmp622)),((_tmp622->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp1A4 - 1),_tmp622))));}
args_already_checked=1;{struct _RegionHandle*_tmp1AA=Cyc_Tcenv_get_fnrgn(te_orig);
void*_tmp1AB=_tmp1A2;_LL12A: if((int)_tmp1AB != 0)goto _LL12C;_LL12B: Cyc_Tcexp_check_format_args(
_tmp187,_tmp1A8,fmt_args,_tmp1AA,Cyc_Formatstr_get_format_typs);goto _LL129;
_LL12C: if((int)_tmp1AB != 1)goto _LL129;_LL12D: Cyc_Tcexp_check_format_args(_tmp187,
_tmp1A8,fmt_args,_tmp1AA,Cyc_Formatstr_get_scanf_typs);goto _LL129;_LL129:;}};
_pop_handler();}else{void*_tmp1A6=(void*)_exn_thrown;void*_tmp1AD=_tmp1A6;_LL12F:
if(_tmp1AD != Cyc_List_Nth)goto _LL131;_LL130:{const char*_tmp625;void*_tmp624;(
_tmp624=0,Cyc_Tcutil_terr(loc,((_tmp625="bad format arguments",_tag_dyneither(
_tmp625,sizeof(char),21))),_tag_dyneither(_tmp624,sizeof(void*),0)));}goto _LL12E;
_LL131:;_LL132:(void)_throw(_tmp1AD);_LL12E:;}}}goto _LL124;_LL127:;_LL128: goto
_LL124;_LL124:;}}if(_tmp196 != 0){const char*_tmp628;void*_tmp627;(_tmp627=0,Cyc_Tcutil_terr(
loc,((_tmp628="too few arguments for function",_tag_dyneither(_tmp628,sizeof(
char),31))),_tag_dyneither(_tmp627,sizeof(void*),0)));}else{if((_tmp185 != 0  || 
_tmp197) || _tmp198 != 0){if(_tmp197)for(0;_tmp185 != 0;_tmp185=_tmp185->tl){Cyc_Tcexp_tcExp(
_tmp187,0,(struct Cyc_Absyn_Exp*)_tmp185->hd);}else{if(_tmp198 == 0){const char*
_tmp62B;void*_tmp62A;(_tmp62A=0,Cyc_Tcutil_terr(loc,((_tmp62B="too many arguments for function",
_tag_dyneither(_tmp62B,sizeof(char),32))),_tag_dyneither(_tmp62A,sizeof(void*),0)));}
else{void*_tmp1B5;int _tmp1B6;struct Cyc_Absyn_VarargInfo _tmp1B4=*_tmp198;_tmp1B5=(
void*)_tmp1B4.type;_tmp1B6=_tmp1B4.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp62C;struct Cyc_Absyn_VarargCallInfo*_tmp1B7=(_tmp62C=_cycalloc(sizeof(*
_tmp62C)),((_tmp62C->num_varargs=0,((_tmp62C->injectors=0,((_tmp62C->vai=(struct
Cyc_Absyn_VarargInfo*)_tmp198,_tmp62C)))))));*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp1B7;if(!_tmp1B6)for(0;_tmp185 != 0;_tmp185=_tmp185->tl){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp185->hd;++ _tmp1B7->num_varargs;Cyc_Tcexp_tcExp(
_tmp187,(void**)& _tmp1B5,e1);if(!Cyc_Tcutil_coerce_arg(_tmp187,e1,_tmp1B5)){{
const char*_tmp631;void*_tmp630[2];struct Cyc_String_pa_struct _tmp62F;struct Cyc_String_pa_struct
_tmp62E;(_tmp62E.tag=0,((_tmp62E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((
_tmp62F.tag=0,((_tmp62F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp1B5)),((_tmp630[0]=& _tmp62F,((_tmp630[1]=& _tmp62E,Cyc_Tcutil_terr(loc,((
_tmp631="vararg requires type %s but argument has type %s",_tag_dyneither(
_tmp631,sizeof(char),49))),_tag_dyneither(_tmp630,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp186,
_tmp1B5) && !Cyc_Tcutil_is_noalias_path(_tmp186,e1)){const char*_tmp634;void*
_tmp633;(_tmp633=0,Cyc_Tcutil_terr(e1->loc,((_tmp634="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp634,sizeof(char),49))),_tag_dyneither(_tmp633,sizeof(void*),0)));}}
else{void*_tmp1BE=Cyc_Tcutil_compress(_tmp1B5);struct Cyc_Absyn_TunionInfo _tmp1BF;
union Cyc_Absyn_TunionInfoU_union _tmp1C0;struct Cyc_Absyn_Tuniondecl**_tmp1C1;
struct Cyc_Absyn_Tuniondecl*_tmp1C2;struct Cyc_List_List*_tmp1C3;struct Cyc_Core_Opt*
_tmp1C4;_LL134: if(_tmp1BE <= (void*)4)goto _LL136;if(*((int*)_tmp1BE)!= 2)goto
_LL136;_tmp1BF=((struct Cyc_Absyn_TunionType_struct*)_tmp1BE)->f1;_tmp1C0=_tmp1BF.tunion_info;
if((((((struct Cyc_Absyn_TunionType_struct*)_tmp1BE)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL136;_tmp1C1=(_tmp1C0.KnownTunion).f1;_tmp1C2=*_tmp1C1;_tmp1C3=
_tmp1BF.targs;_tmp1C4=_tmp1BF.rgn;_LL135: {struct Cyc_Absyn_Tuniondecl*_tmp1C5=*
Cyc_Tcenv_lookup_tuniondecl(_tmp187,loc,_tmp1C2->name);struct Cyc_List_List*
fields=0;if(_tmp1C5->fields == 0){const char*_tmp638;void*_tmp637[1];struct Cyc_String_pa_struct
_tmp636;(_tmp636.tag=0,((_tmp636.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp1B5)),((_tmp637[0]=& _tmp636,Cyc_Tcutil_terr(loc,((
_tmp638="can't inject into %s",_tag_dyneither(_tmp638,sizeof(char),21))),
_tag_dyneither(_tmp637,sizeof(void*),1)))))));}else{fields=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1C5->fields))->v;}if(!Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1C4))->v,Cyc_Tcenv_curr_rgn(_tmp187))){const
char*_tmp63B;void*_tmp63A;(_tmp63A=0,Cyc_Tcutil_terr(loc,((_tmp63B="bad region for injected varargs",
_tag_dyneither(_tmp63B,sizeof(char),32))),_tag_dyneither(_tmp63A,sizeof(void*),0)));}{
struct _RegionHandle*_tmp1CB=Cyc_Tcenv_get_fnrgn(_tmp187);{struct Cyc_List_List*
_tmp1CC=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp1CB,_tmp1CB,
_tmp1C5->tvs,_tmp1C3);for(0;_tmp185 != 0;_tmp185=_tmp185->tl){++ _tmp1B7->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp185->hd;if(!
args_already_checked){Cyc_Tcexp_tcExp(_tmp187,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1CB,(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(
_tmp1CB,e1)){const char*_tmp63E;void*_tmp63D;(_tmp63D=0,Cyc_Tcutil_terr(e1->loc,((
_tmp63E="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp63E,sizeof(char),49))),_tag_dyneither(_tmp63D,sizeof(void*),0)));}}{struct
Cyc_Absyn_Tunionfield*_tmp1CF=Cyc_Tcexp_tcInjection(_tmp187,e1,_tmp1B5,_tmp1CB,
_tmp1CC,fields);if(_tmp1CF != 0){struct Cyc_List_List*_tmp63F;_tmp1B7->injectors=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1B7->injectors,((_tmp63F=_cycalloc(sizeof(*_tmp63F)),((_tmp63F->hd=(struct
Cyc_Absyn_Tunionfield*)_tmp1CF,((_tmp63F->tl=0,_tmp63F)))))));}}}}}goto _LL133;}}
_LL136:;_LL137:{const char*_tmp642;void*_tmp641;(_tmp641=0,Cyc_Tcutil_terr(loc,((
_tmp642="bad inject vararg type",_tag_dyneither(_tmp642,sizeof(char),23))),
_tag_dyneither(_tmp641,sizeof(void*),0)));}goto _LL133;_LL133:;}}}}}}Cyc_Tcenv_check_effect_accessible(
_tmp187,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp194))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp187,loc,_tmp199);return _tmp195;}_LL122:;_LL123: {const char*_tmp645;void*
_tmp644;return(_tmp644=0,Cyc_Tcexp_expr_err(_tmp187,loc,topt,((_tmp645="expected pointer to function",
_tag_dyneither(_tmp645,sizeof(char),29))),_tag_dyneither(_tmp644,sizeof(void*),0)));}
_LL11F:;}_LL11D:;_LL11E: {const char*_tmp648;void*_tmp647;return(_tmp647=0,Cyc_Tcexp_expr_err(
_tmp187,loc,topt,((_tmp648="expected pointer to function",_tag_dyneither(_tmp648,
sizeof(char),29))),_tag_dyneither(_tmp647,sizeof(void*),0)));}_LL11A:;}}static
void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_exn_typ,e);if(!Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ)){
const char*_tmp64C;void*_tmp64B[1];struct Cyc_String_pa_struct _tmp64A;(_tmp64A.tag=
0,((_tmp64A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp64B[0]=& _tmp64A,Cyc_Tcutil_terr(
loc,((_tmp64C="expected xtunion exn but found %s",_tag_dyneither(_tmp64C,sizeof(
char),34))),_tag_dyneither(_tmp64B,sizeof(void*),1)))))));}return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));}static void*Cyc_Tcexp_tcInstantiate(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*ts);static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
ts){Cyc_Tcexp_tcExpNoInst(te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
0));{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp1DB=t1;struct Cyc_Absyn_PtrInfo _tmp1DC;void*_tmp1DD;struct Cyc_Absyn_Tqual
_tmp1DE;struct Cyc_Absyn_PtrAtts _tmp1DF;void*_tmp1E0;struct Cyc_Absyn_Conref*
_tmp1E1;struct Cyc_Absyn_Conref*_tmp1E2;struct Cyc_Absyn_Conref*_tmp1E3;_LL139: if(
_tmp1DB <= (void*)4)goto _LL13B;if(*((int*)_tmp1DB)!= 4)goto _LL13B;_tmp1DC=((
struct Cyc_Absyn_PointerType_struct*)_tmp1DB)->f1;_tmp1DD=(void*)_tmp1DC.elt_typ;
_tmp1DE=_tmp1DC.elt_tq;_tmp1DF=_tmp1DC.ptr_atts;_tmp1E0=(void*)_tmp1DF.rgn;
_tmp1E1=_tmp1DF.nullable;_tmp1E2=_tmp1DF.bounds;_tmp1E3=_tmp1DF.zero_term;_LL13A:{
void*_tmp1E4=Cyc_Tcutil_compress(_tmp1DD);struct Cyc_Absyn_FnInfo _tmp1E5;struct
Cyc_List_List*_tmp1E6;struct Cyc_Core_Opt*_tmp1E7;void*_tmp1E8;struct Cyc_List_List*
_tmp1E9;int _tmp1EA;struct Cyc_Absyn_VarargInfo*_tmp1EB;struct Cyc_List_List*
_tmp1EC;struct Cyc_List_List*_tmp1ED;_LL13E: if(_tmp1E4 <= (void*)4)goto _LL140;if(*((
int*)_tmp1E4)!= 8)goto _LL140;_tmp1E5=((struct Cyc_Absyn_FnType_struct*)_tmp1E4)->f1;
_tmp1E6=_tmp1E5.tvars;_tmp1E7=_tmp1E5.effect;_tmp1E8=(void*)_tmp1E5.ret_typ;
_tmp1E9=_tmp1E5.args;_tmp1EA=_tmp1E5.c_varargs;_tmp1EB=_tmp1E5.cyc_varargs;
_tmp1EC=_tmp1E5.rgn_po;_tmp1ED=_tmp1E5.attributes;_LL13F: {struct _RegionHandle*
_tmp1EE=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*instantiation=0;for(0;ts != 0
 && _tmp1E6 != 0;(ts=ts->tl,_tmp1E6=_tmp1E6->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp1E6->hd);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);{struct _tuple5*_tmp64F;struct Cyc_List_List*_tmp64E;
instantiation=((_tmp64E=_region_malloc(_tmp1EE,sizeof(*_tmp64E)),((_tmp64E->hd=((
_tmp64F=_region_malloc(_tmp1EE,sizeof(*_tmp64F)),((_tmp64F->f1=(struct Cyc_Absyn_Tvar*)
_tmp1E6->hd,((_tmp64F->f2=(void*)ts->hd,_tmp64F)))))),((_tmp64E->tl=
instantiation,_tmp64E))))));}}if(ts != 0){const char*_tmp652;void*_tmp651;return(
_tmp651=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp652="too many type variables in instantiation",
_tag_dyneither(_tmp652,sizeof(char),41))),_tag_dyneither(_tmp651,sizeof(void*),0)));}{
struct Cyc_Absyn_FnType_struct _tmp658;struct Cyc_Absyn_FnInfo _tmp657;struct Cyc_Absyn_FnType_struct*
_tmp656;void*new_fn_typ=Cyc_Tcutil_rsubstitute(_tmp1EE,instantiation,(void*)((
_tmp656=_cycalloc(sizeof(*_tmp656)),((_tmp656[0]=((_tmp658.tag=8,((_tmp658.f1=((
_tmp657.tvars=_tmp1E6,((_tmp657.effect=_tmp1E7,((_tmp657.ret_typ=(void*)_tmp1E8,((
_tmp657.args=_tmp1E9,((_tmp657.c_varargs=_tmp1EA,((_tmp657.cyc_varargs=_tmp1EB,((
_tmp657.rgn_po=_tmp1EC,((_tmp657.attributes=_tmp1ED,_tmp657)))))))))))))))),
_tmp658)))),_tmp656)))));struct Cyc_Absyn_PointerType_struct _tmp662;struct Cyc_Absyn_PtrAtts
_tmp661;struct Cyc_Absyn_PtrInfo _tmp660;struct Cyc_Absyn_PointerType_struct*
_tmp65F;return(void*)((_tmp65F=_cycalloc(sizeof(*_tmp65F)),((_tmp65F[0]=((
_tmp662.tag=4,((_tmp662.f1=((_tmp660.elt_typ=(void*)new_fn_typ,((_tmp660.elt_tq=
_tmp1DE,((_tmp660.ptr_atts=((_tmp661.rgn=(void*)_tmp1E0,((_tmp661.nullable=
_tmp1E1,((_tmp661.bounds=_tmp1E2,((_tmp661.zero_term=_tmp1E3,((_tmp661.ptrloc=0,
_tmp661)))))))))),_tmp660)))))),_tmp662)))),_tmp65F))));}}_LL140:;_LL141: goto
_LL13D;_LL13D:;}goto _LL138;_LL13B:;_LL13C: goto _LL138;_LL138:;}{const char*_tmp666;
void*_tmp665[1];struct Cyc_String_pa_struct _tmp664;return(_tmp664.tag=0,((_tmp664.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((
_tmp665[0]=& _tmp664,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp666="expecting polymorphic type but found %s",
_tag_dyneither(_tmp666,sizeof(char),40))),_tag_dyneither(_tmp665,sizeof(void*),1)))))));}}}
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,void**c);static void*Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct
Cyc_Absyn_Exp*e,void**c){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((void**)
_check_null(c))=(void*)1;else{void*crc=Cyc_Tcutil_castable(te,loc,t2,t);if(crc != (
void*)0)*((void**)_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((
void**)_check_null(c))=(void*)1;else{Cyc_Tcutil_unify(t2,t);{const char*_tmp66B;
void*_tmp66A[2];struct Cyc_String_pa_struct _tmp669;struct Cyc_String_pa_struct
_tmp668;void*_tmp1FD=(_tmp668.tag=0,((_tmp668.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp669.tag=0,((_tmp669.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp66A[0]=&
_tmp669,((_tmp66A[1]=& _tmp668,Cyc_Tcexp_expr_err(te,loc,(void**)& t,((_tmp66B="cannot cast %s to %s",
_tag_dyneither(_tmp66B,sizeof(char),21))),_tag_dyneither(_tmp66A,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();return _tmp1FD;}}}}{struct _tuple0 _tmp66C;struct
_tuple0 _tmp203=(_tmp66C.f1=(void*)e->r,((_tmp66C.f2=Cyc_Tcutil_compress(t),
_tmp66C)));void*_tmp204;struct Cyc_Absyn_MallocInfo _tmp205;int _tmp206;void*
_tmp207;struct Cyc_Absyn_PtrInfo _tmp208;struct Cyc_Absyn_PtrAtts _tmp209;struct Cyc_Absyn_Conref*
_tmp20A;struct Cyc_Absyn_Conref*_tmp20B;struct Cyc_Absyn_Conref*_tmp20C;_LL143:
_tmp204=_tmp203.f1;if(*((int*)_tmp204)!= 35)goto _LL145;_tmp205=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp204)->f1;_tmp206=_tmp205.fat_result;_tmp207=_tmp203.f2;if(_tmp207 <= (void*)4)
goto _LL145;if(*((int*)_tmp207)!= 4)goto _LL145;_tmp208=((struct Cyc_Absyn_PointerType_struct*)
_tmp207)->f1;_tmp209=_tmp208.ptr_atts;_tmp20A=_tmp209.nullable;_tmp20B=_tmp209.bounds;
_tmp20C=_tmp209.zero_term;_LL144: if((_tmp206  && !((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp20C)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp20A)){void*_tmp20D=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp20B);struct Cyc_Absyn_Exp*_tmp20E;_LL148: if(_tmp20D <= (void*)1)goto _LL14A;if(*((
int*)_tmp20D)!= 0)goto _LL14A;_tmp20E=((struct Cyc_Absyn_Upper_b_struct*)_tmp20D)->f1;
_LL149: if((Cyc_Evexp_eval_const_uint_exp(_tmp20E)).f1 == 1){const char*_tmp66F;
void*_tmp66E;(_tmp66E=0,Cyc_Tcutil_warn(loc,((_tmp66F="cast from ? pointer to * pointer will lose size information",
_tag_dyneither(_tmp66F,sizeof(char),60))),_tag_dyneither(_tmp66E,sizeof(void*),0)));}
goto _LL147;_LL14A:;_LL14B: goto _LL147;_LL147:;}goto _LL142;_LL145:;_LL146: goto
_LL142;_LL142:;}return t;}}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e);static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){void**_tmp211=0;
struct Cyc_Absyn_Tqual _tmp212=Cyc_Absyn_empty_tqual(0);if(topt != 0){void*_tmp213=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp214;void*_tmp215;struct Cyc_Absyn_Tqual
_tmp216;struct Cyc_Absyn_PtrAtts _tmp217;struct Cyc_Absyn_Conref*_tmp218;_LL14D: if(
_tmp213 <= (void*)4)goto _LL14F;if(*((int*)_tmp213)!= 4)goto _LL14F;_tmp214=((
struct Cyc_Absyn_PointerType_struct*)_tmp213)->f1;_tmp215=(void*)_tmp214.elt_typ;
_tmp216=_tmp214.elt_tq;_tmp217=_tmp214.ptr_atts;_tmp218=_tmp217.zero_term;_LL14E:{
void**_tmp670;_tmp211=((_tmp670=_cycalloc(sizeof(*_tmp670)),((_tmp670[0]=_tmp215,
_tmp670))));}_tmp212=_tmp216;goto _LL14C;_LL14F:;_LL150: goto _LL14C;_LL14C:;}{
struct _RegionHandle*_tmp21A=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_clear_notreadctxt(
_tmp21A,te),_tmp211,e);if(Cyc_Tcutil_is_noalias_path(_tmp21A,e)){const char*
_tmp673;void*_tmp672;(_tmp672=0,Cyc_Tcutil_terr(e->loc,((_tmp673="Cannot take the address of an alias-free path",
_tag_dyneither(_tmp673,sizeof(char),46))),_tag_dyneither(_tmp672,sizeof(void*),0)));}{
void*_tmp21D=(void*)e->r;struct Cyc_Absyn_Exp*_tmp21E;struct Cyc_Absyn_Exp*_tmp21F;
_LL152: if(*((int*)_tmp21D)!= 25)goto _LL154;_tmp21E=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp21D)->f1;_tmp21F=((struct Cyc_Absyn_Subscript_e_struct*)_tmp21D)->f2;_LL153:{
void*_tmp220=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp21E->topt))->v);_LL157: if(_tmp220 <= (void*)4)goto _LL159;if(*((int*)_tmp220)
!= 9)goto _LL159;_LL158: goto _LL156;_LL159:;_LL15A:(void*)(e0->r=(void*)((void*)(
Cyc_Absyn_add_exp(_tmp21E,_tmp21F,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp21E,
_tmp21F);_LL156:;}goto _LL151;_LL154:;_LL155: goto _LL151;_LL151:;}{int _tmp222;void*
_tmp223;struct _tuple6 _tmp221=Cyc_Tcutil_addressof_props(te,e);_tmp222=_tmp221.f1;
_tmp223=_tmp221.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp222){
tq.print_const=1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp223,tq,Cyc_Absyn_false_conref);return t;}}}}}static
void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t);static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,void*t){if(te->allow_valueof)return Cyc_Absyn_uint_typ;
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);if(!
Cyc_Evexp_okay_szofarg(t)){const char*_tmp677;void*_tmp676[1];struct Cyc_String_pa_struct
_tmp675;(_tmp675.tag=0,((_tmp675.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp676[0]=& _tmp675,Cyc_Tcutil_terr(loc,((_tmp677="sizeof applied to type %s, which has unknown size here",
_tag_dyneither(_tmp677,sizeof(char),55))),_tag_dyneither(_tmp676,sizeof(void*),1)))))));}
if(topt != 0){void*_tmp227=Cyc_Tcutil_compress(*topt);void*_tmp228;_LL15C: if(
_tmp227 <= (void*)4)goto _LL15E;if(*((int*)_tmp227)!= 18)goto _LL15E;_tmp228=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp227)->f1;_LL15D: {struct Cyc_Absyn_Exp*
_tmp229=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct _tmp67A;
struct Cyc_Absyn_ValueofType_struct*_tmp679;struct Cyc_Absyn_ValueofType_struct*
_tmp22A=(_tmp679=_cycalloc(sizeof(*_tmp679)),((_tmp679[0]=((_tmp67A.tag=17,((
_tmp67A.f1=_tmp229,_tmp67A)))),_tmp679)));if(Cyc_Tcutil_unify(_tmp228,(void*)
_tmp22A))return _tmp227;goto _LL15B;}_LL15E:;_LL15F: goto _LL15B;_LL15B:;}return Cyc_Absyn_uint_typ;}
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf);int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf){return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 
0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,void*n);static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*_tmp22D=n;struct
_dyneither_ptr*_tmp22E;unsigned int _tmp22F;_LL161: if(*((int*)_tmp22D)!= 0)goto
_LL163;_tmp22E=((struct Cyc_Absyn_StructField_struct*)_tmp22D)->f1;_LL162: {int
bad_type=1;{void*_tmp230=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp231;
union Cyc_Absyn_AggrInfoU_union _tmp232;struct Cyc_Absyn_Aggrdecl**_tmp233;struct
Cyc_List_List*_tmp234;_LL166: if(_tmp230 <= (void*)4)goto _LL16A;if(*((int*)_tmp230)
!= 10)goto _LL168;_tmp231=((struct Cyc_Absyn_AggrType_struct*)_tmp230)->f1;_tmp232=
_tmp231.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp230)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL168;_tmp233=(_tmp232.KnownAggr).f1;_LL167: if((*_tmp233)->impl == 0)
goto _LL165;if(!((int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),
struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp22E,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp233)->impl))->fields)){
const char*_tmp67E;void*_tmp67D[1];struct Cyc_String_pa_struct _tmp67C;(_tmp67C.tag=
0,((_tmp67C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp22E),((_tmp67D[
0]=& _tmp67C,Cyc_Tcutil_terr(loc,((_tmp67E="no field of struct/union has name %s",
_tag_dyneither(_tmp67E,sizeof(char),37))),_tag_dyneither(_tmp67D,sizeof(void*),1)))))));}
bad_type=0;goto _LL165;_LL168: if(*((int*)_tmp230)!= 11)goto _LL16A;_tmp234=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp230)->f2;_LL169: if(!((int(*)(int(*pred)(
struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp22E,
_tmp234)){const char*_tmp682;void*_tmp681[1];struct Cyc_String_pa_struct _tmp680;(
_tmp680.tag=0,((_tmp680.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp22E),((
_tmp681[0]=& _tmp680,Cyc_Tcutil_terr(loc,((_tmp682="no field of struct/union has name %s",
_tag_dyneither(_tmp682,sizeof(char),37))),_tag_dyneither(_tmp681,sizeof(void*),1)))))));}
bad_type=0;goto _LL165;_LL16A:;_LL16B: goto _LL165;_LL165:;}if(bad_type){const char*
_tmp686;void*_tmp685[1];struct Cyc_String_pa_struct _tmp684;(_tmp684.tag=0,((
_tmp684.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp685[0]=& _tmp684,Cyc_Tcutil_terr(loc,((_tmp686="%s is not a known struct/union type",
_tag_dyneither(_tmp686,sizeof(char),36))),_tag_dyneither(_tmp685,sizeof(void*),1)))))));}
goto _LL160;}_LL163: if(*((int*)_tmp22D)!= 1)goto _LL160;_tmp22F=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp22D)->f1;_LL164: {int bad_type=1;{void*_tmp23E=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_AggrInfo _tmp23F;union Cyc_Absyn_AggrInfoU_union _tmp240;struct Cyc_Absyn_Aggrdecl**
_tmp241;struct Cyc_List_List*_tmp242;struct Cyc_List_List*_tmp243;struct Cyc_Absyn_TunionFieldInfo
_tmp244;union Cyc_Absyn_TunionFieldInfoU_union _tmp245;struct Cyc_Absyn_Tunionfield*
_tmp246;_LL16D: if(_tmp23E <= (void*)4)goto _LL175;if(*((int*)_tmp23E)!= 10)goto
_LL16F;_tmp23F=((struct Cyc_Absyn_AggrType_struct*)_tmp23E)->f1;_tmp240=_tmp23F.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp23E)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL16F;_tmp241=(_tmp240.KnownAggr).f1;_LL16E: if((*_tmp241)->impl == 0)
goto _LL16C;_tmp242=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp241)->impl))->fields;
goto _LL170;_LL16F: if(*((int*)_tmp23E)!= 11)goto _LL171;_tmp242=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp23E)->f2;_LL170: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp242)
<= _tmp22F){const char*_tmp68B;void*_tmp68A[2];struct Cyc_Int_pa_struct _tmp689;
struct Cyc_Int_pa_struct _tmp688;(_tmp688.tag=1,((_tmp688.f1=(unsigned long)((int)
_tmp22F),((_tmp689.tag=1,((_tmp689.f1=(unsigned long)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp242),((_tmp68A[0]=& _tmp689,((_tmp68A[1]=& _tmp688,Cyc_Tcutil_terr(
loc,((_tmp68B="struct/union has too few components: %d <= %d",_tag_dyneither(
_tmp68B,sizeof(char),46))),_tag_dyneither(_tmp68A,sizeof(void*),2)))))))))))));}
bad_type=0;goto _LL16C;_LL171: if(*((int*)_tmp23E)!= 9)goto _LL173;_tmp243=((struct
Cyc_Absyn_TupleType_struct*)_tmp23E)->f1;_LL172: if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp243)<= _tmp22F){const char*_tmp690;void*_tmp68F[2];struct
Cyc_Int_pa_struct _tmp68E;struct Cyc_Int_pa_struct _tmp68D;(_tmp68D.tag=1,((_tmp68D.f1=(
unsigned long)((int)_tmp22F),((_tmp68E.tag=1,((_tmp68E.f1=(unsigned long)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp243),((_tmp68F[0]=& _tmp68E,((_tmp68F[
1]=& _tmp68D,Cyc_Tcutil_terr(loc,((_tmp690="tuple has too few components: %d <= %d",
_tag_dyneither(_tmp690,sizeof(char),39))),_tag_dyneither(_tmp68F,sizeof(void*),2)))))))))))));}
bad_type=0;goto _LL16C;_LL173: if(*((int*)_tmp23E)!= 3)goto _LL175;_tmp244=((struct
Cyc_Absyn_TunionFieldType_struct*)_tmp23E)->f1;_tmp245=_tmp244.field_info;if((((((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp23E)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL175;_tmp246=(_tmp245.KnownTunionfield).f2;_LL174: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp246->typs)< _tmp22F){const char*_tmp695;
void*_tmp694[2];struct Cyc_Int_pa_struct _tmp693;struct Cyc_Int_pa_struct _tmp692;(
_tmp692.tag=1,((_tmp692.f1=(unsigned long)((int)_tmp22F),((_tmp693.tag=1,((
_tmp693.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp246->typs),((_tmp694[0]=& _tmp693,((_tmp694[1]=& _tmp692,Cyc_Tcutil_terr(loc,((
_tmp695="tunionfield has too few components: %d < %d",_tag_dyneither(_tmp695,
sizeof(char),44))),_tag_dyneither(_tmp694,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL16C;_LL175:;_LL176: goto _LL16C;_LL16C:;}if(bad_type){const char*_tmp699;
void*_tmp698[1];struct Cyc_String_pa_struct _tmp697;(_tmp697.tag=0,((_tmp697.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp698[
0]=& _tmp697,Cyc_Tcutil_terr(loc,((_tmp699="%s is not a known type",
_tag_dyneither(_tmp699,sizeof(char),23))),_tag_dyneither(_tmp698,sizeof(void*),1)))))));}
goto _LL160;}_LL160:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e);static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){struct _RegionHandle*_tmp256=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp257=Cyc_Tcenv_clear_notreadctxt(_tmp256,
te_orig);Cyc_Tcexp_tcExp(_tmp257,0,e);{void*t=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp258=t;struct Cyc_Absyn_PtrInfo
_tmp259;void*_tmp25A;struct Cyc_Absyn_PtrAtts _tmp25B;void*_tmp25C;struct Cyc_Absyn_Conref*
_tmp25D;struct Cyc_Absyn_Conref*_tmp25E;_LL178: if(_tmp258 <= (void*)4)goto _LL17A;
if(*((int*)_tmp258)!= 4)goto _LL17A;_tmp259=((struct Cyc_Absyn_PointerType_struct*)
_tmp258)->f1;_tmp25A=(void*)_tmp259.elt_typ;_tmp25B=_tmp259.ptr_atts;_tmp25C=(
void*)_tmp25B.rgn;_tmp25D=_tmp25B.bounds;_tmp25E=_tmp25B.zero_term;_LL179: Cyc_Tcenv_check_rgn_accessible(
_tmp257,loc,_tmp25C);Cyc_Tcutil_check_nonzero_bound(loc,_tmp25D);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp25A),(void*)1)){const char*_tmp69C;void*_tmp69B;(_tmp69B=
0,Cyc_Tcutil_terr(loc,((_tmp69C="can't dereference abstract pointer type",
_tag_dyneither(_tmp69C,sizeof(char),40))),_tag_dyneither(_tmp69B,sizeof(void*),0)));}
return _tmp25A;_LL17A:;_LL17B: {const char*_tmp6A0;void*_tmp69F[1];struct Cyc_String_pa_struct
_tmp69E;return(_tmp69E.tag=0,((_tmp69E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp69F[0]=& _tmp69E,Cyc_Tcexp_expr_err(
_tmp257,loc,topt,((_tmp6A0="expecting * or @ type but found %s",_tag_dyneither(
_tmp6A0,sizeof(char),35))),_tag_dyneither(_tmp69F,sizeof(void*),1)))))));}_LL177:;}}
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f);static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr*f){Cyc_Tcexp_tcExpNoPromote(te,0,e);{void*_tmp264=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp265;union Cyc_Absyn_AggrInfoU_union _tmp266;struct Cyc_Absyn_Aggrdecl**_tmp267;
struct Cyc_Absyn_Aggrdecl*_tmp268;struct Cyc_List_List*_tmp269;void*_tmp26A;struct
Cyc_List_List*_tmp26B;_LL17D: if(_tmp264 <= (void*)4)goto _LL185;if(*((int*)_tmp264)
!= 10)goto _LL17F;_tmp265=((struct Cyc_Absyn_AggrType_struct*)_tmp264)->f1;_tmp266=
_tmp265.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp264)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL17F;_tmp267=(_tmp266.KnownAggr).f1;_tmp268=*_tmp267;_tmp269=_tmp265.targs;
_LL17E: {struct Cyc_Absyn_Aggrfield*_tmp26E=Cyc_Absyn_lookup_decl_field(_tmp268,f);
if(_tmp26E == 0){const char*_tmp6A5;void*_tmp6A4[2];struct Cyc_String_pa_struct
_tmp6A3;struct Cyc_String_pa_struct _tmp6A2;return(_tmp6A2.tag=0,((_tmp6A2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6A3.tag=0,((_tmp6A3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp268->name)),((
_tmp6A4[0]=& _tmp6A3,((_tmp6A4[1]=& _tmp6A2,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp6A5="type %s has no %s field",_tag_dyneither(_tmp6A5,sizeof(char),24))),
_tag_dyneither(_tmp6A4,sizeof(void*),2)))))))))))));}if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp268->impl))->exist_vars != 0){const char*_tmp6A8;void*_tmp6A7;
return(_tmp6A7=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6A8="must use pattern-matching to access fields of existential types",
_tag_dyneither(_tmp6A8,sizeof(char),64))),_tag_dyneither(_tmp6A7,sizeof(void*),0)));}{
void*t2;struct _RegionHandle*_tmp275=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
_tmp276=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp275,_tmp275,
_tmp268->tvs,_tmp269);t2=Cyc_Tcutil_rsubstitute(_tmp275,_tmp276,(void*)_tmp26E->type);
if(((void*)_tmp268->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t2)){const char*_tmp6AC;void*_tmp6AB[1];struct Cyc_String_pa_struct _tmp6AA;return(
_tmp6AA.tag=0,((_tmp6AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp6AB[0]=& _tmp6AA,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6AC="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp6AC,sizeof(char),56))),_tag_dyneither(_tmp6AB,sizeof(void*),1)))))));}}
return t2;}}_LL17F: if(*((int*)_tmp264)!= 11)goto _LL181;_tmp26A=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp264)->f1;_tmp26B=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp264)->f2;_LL180: {
struct Cyc_Absyn_Aggrfield*_tmp27A=Cyc_Absyn_lookup_field(_tmp26B,f);if(_tmp27A == 
0){const char*_tmp6B0;void*_tmp6AF[1];struct Cyc_String_pa_struct _tmp6AE;return(
_tmp6AE.tag=0,((_tmp6AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp6AF[0]=& _tmp6AE,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6B0="type has no %s field",
_tag_dyneither(_tmp6B0,sizeof(char),21))),_tag_dyneither(_tmp6AF,sizeof(void*),1)))))));}
if((_tmp26A == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp27A->type)){const char*_tmp6B4;void*_tmp6B3[1];struct Cyc_String_pa_struct
_tmp6B2;return(_tmp6B2.tag=0,((_tmp6B2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp6B3[0]=& _tmp6B2,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp6B4="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp6B4,sizeof(char),56))),_tag_dyneither(_tmp6B3,sizeof(void*),1)))))));}return(
void*)_tmp27A->type;}_LL181: if(*((int*)_tmp264)!= 7)goto _LL183;{const char*
_tmp6B5;if(!(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp6B5="size",_tag_dyneither(
_tmp6B5,sizeof(char),5))))== 0))goto _LL183;}_LL182: goto _LL184;_LL183: if(*((int*)
_tmp264)!= 4)goto _LL185;{const char*_tmp6B6;if(!(Cyc_strcmp((struct _dyneither_ptr)*
f,((_tmp6B6="size",_tag_dyneither(_tmp6B6,sizeof(char),5))))== 0))goto _LL185;}
_LL184:{const char*_tmp6BA;void*_tmp6B9[1];struct Cyc_String_pa_struct _tmp6B8;(
_tmp6B8.tag=0,((_tmp6B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp6B9[0]=& _tmp6B8,Cyc_Tcutil_warn(e->loc,((_tmp6BA="deprecated `.size' used here -- change to numelts(%s)",
_tag_dyneither(_tmp6BA,sizeof(char),54))),_tag_dyneither(_tmp6B9,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Primop_e_struct _tmp6C0;struct Cyc_List_List*_tmp6BF;struct Cyc_Absyn_Primop_e_struct*
_tmp6BE;(void*)(outer_e->r=(void*)((void*)((_tmp6BE=_cycalloc(sizeof(*_tmp6BE)),((
_tmp6BE[0]=((_tmp6C0.tag=3,((_tmp6C0.f1=(void*)((void*)19),((_tmp6C0.f2=((
_tmp6BF=_cycalloc(sizeof(*_tmp6BF)),((_tmp6BF->hd=e,((_tmp6BF->tl=0,_tmp6BF)))))),
_tmp6C0)))))),_tmp6BE))))));}return Cyc_Absyn_uint_typ;_LL185:;_LL186: {const char*
_tmp6C1;if(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp6C1="size",_tag_dyneither(
_tmp6C1,sizeof(char),5))))== 0){const char*_tmp6C5;void*_tmp6C4[1];struct Cyc_String_pa_struct
_tmp6C3;return(_tmp6C3.tag=0,((_tmp6C3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp6C4[0]=& _tmp6C3,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6C5="expecting struct, union, or array, found %s",
_tag_dyneither(_tmp6C5,sizeof(char),44))),_tag_dyneither(_tmp6C4,sizeof(void*),1)))))));}
else{const char*_tmp6C9;void*_tmp6C8[1];struct Cyc_String_pa_struct _tmp6C7;return(
_tmp6C7.tag=0,((_tmp6C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp6C8[0]=& _tmp6C7,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp6C9="expecting struct or union, found %s",_tag_dyneither(
_tmp6C9,sizeof(char),36))),_tag_dyneither(_tmp6C8,sizeof(void*),1)))))));}}
_LL17C:;}}static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f);static void*Cyc_Tcexp_tcAggrArrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr*f){struct _RegionHandle*_tmp28E=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(_tmp28E,te),0,e);{void*_tmp28F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp290;void*_tmp291;struct Cyc_Absyn_PtrAtts _tmp292;void*_tmp293;struct Cyc_Absyn_Conref*
_tmp294;struct Cyc_Absyn_Conref*_tmp295;_LL188: if(_tmp28F <= (void*)4)goto _LL18A;
if(*((int*)_tmp28F)!= 4)goto _LL18A;_tmp290=((struct Cyc_Absyn_PointerType_struct*)
_tmp28F)->f1;_tmp291=(void*)_tmp290.elt_typ;_tmp292=_tmp290.ptr_atts;_tmp293=(
void*)_tmp292.rgn;_tmp294=_tmp292.bounds;_tmp295=_tmp292.zero_term;_LL189: Cyc_Tcutil_check_nonzero_bound(
loc,_tmp294);{void*_tmp296=Cyc_Tcutil_compress(_tmp291);struct Cyc_Absyn_AggrInfo
_tmp297;union Cyc_Absyn_AggrInfoU_union _tmp298;struct Cyc_Absyn_Aggrdecl**_tmp299;
struct Cyc_Absyn_Aggrdecl*_tmp29A;struct Cyc_List_List*_tmp29B;void*_tmp29C;struct
Cyc_List_List*_tmp29D;_LL18D: if(_tmp296 <= (void*)4)goto _LL191;if(*((int*)_tmp296)
!= 10)goto _LL18F;_tmp297=((struct Cyc_Absyn_AggrType_struct*)_tmp296)->f1;_tmp298=
_tmp297.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp296)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL18F;_tmp299=(_tmp298.KnownAggr).f1;_tmp29A=*_tmp299;_tmp29B=_tmp297.targs;
_LL18E: {struct Cyc_Absyn_Aggrfield*_tmp29E=Cyc_Absyn_lookup_decl_field(_tmp29A,f);
if(_tmp29E == 0){const char*_tmp6CE;void*_tmp6CD[2];struct Cyc_String_pa_struct
_tmp6CC;struct Cyc_String_pa_struct _tmp6CB;return(_tmp6CB.tag=0,((_tmp6CB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6CC.tag=0,((_tmp6CC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp29A->name)),((
_tmp6CD[0]=& _tmp6CC,((_tmp6CD[1]=& _tmp6CB,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp6CE="type %s has no %s field",_tag_dyneither(_tmp6CE,sizeof(char),24))),
_tag_dyneither(_tmp6CD,sizeof(void*),2)))))))))))));}if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp29A->impl))->exist_vars != 0){const char*_tmp6D1;void*_tmp6D0;
return(_tmp6D0=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6D1="must use pattern-matching to access fields of existential types",
_tag_dyneither(_tmp6D1,sizeof(char),64))),_tag_dyneither(_tmp6D0,sizeof(void*),0)));}{
void*t3;struct _RegionHandle*_tmp2A5=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
_tmp2A6=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp2A5,_tmp2A5,
_tmp29A->tvs,_tmp29B);t3=Cyc_Tcutil_rsubstitute(_tmp2A5,_tmp2A6,(void*)_tmp29E->type);}
if(((void*)_tmp29A->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t3)){const char*_tmp6D5;void*_tmp6D4[1];struct Cyc_String_pa_struct _tmp6D3;return(
_tmp6D3.tag=0,((_tmp6D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp6D4[0]=& _tmp6D3,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6D5="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp6D5,sizeof(char),56))),_tag_dyneither(_tmp6D4,sizeof(void*),1)))))));}
return t3;}}_LL18F: if(*((int*)_tmp296)!= 11)goto _LL191;_tmp29C=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp296)->f1;_tmp29D=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp296)->f2;_LL190: {
struct Cyc_Absyn_Aggrfield*_tmp2AA=Cyc_Absyn_lookup_field(_tmp29D,f);if(_tmp2AA == 
0){const char*_tmp6D9;void*_tmp6D8[1];struct Cyc_String_pa_struct _tmp6D7;return(
_tmp6D7.tag=0,((_tmp6D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp6D8[0]=& _tmp6D7,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6D9="type has no %s field",
_tag_dyneither(_tmp6D9,sizeof(char),21))),_tag_dyneither(_tmp6D8,sizeof(void*),1)))))));}
if((_tmp29C == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp2AA->type)){const char*_tmp6DD;void*_tmp6DC[1];struct Cyc_String_pa_struct
_tmp6DB;return(_tmp6DB.tag=0,((_tmp6DB.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp6DC[0]=& _tmp6DB,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp6DD="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp6DD,sizeof(char),56))),_tag_dyneither(_tmp6DC,sizeof(void*),1)))))));}return(
void*)_tmp2AA->type;}_LL191:;_LL192: goto _LL18C;_LL18C:;}goto _LL187;_LL18A:;
_LL18B: goto _LL187;_LL187:;}{const char*_tmp6E1;void*_tmp6E0[1];struct Cyc_String_pa_struct
_tmp6DF;return(_tmp6DF.tag=0,((_tmp6DF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp6E0[0]=& _tmp6DF,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6E1="expecting struct or union pointer, found %s",
_tag_dyneither(_tmp6E1,sizeof(char),44))),_tag_dyneither(_tmp6E0,sizeof(void*),1)))))));}}
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index);static void*Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,
struct Cyc_Absyn_Exp*index){unsigned int _tmp2B5;int _tmp2B6;struct _tuple7 _tmp2B4=
Cyc_Evexp_eval_const_uint_exp(index);_tmp2B5=_tmp2B4.f1;_tmp2B6=_tmp2B4.f2;if(!
_tmp2B6){const char*_tmp6E4;void*_tmp6E3;return(_tmp6E3=0,Cyc_Tcexp_expr_err(te,
loc,0,((_tmp6E4="tuple projection cannot use sizeof or offsetof",_tag_dyneither(
_tmp6E4,sizeof(char),47))),_tag_dyneither(_tmp6E3,sizeof(void*),0)));}{struct
_handler_cons _tmp2B9;_push_handler(& _tmp2B9);{int _tmp2BB=0;if(setjmp(_tmp2B9.handler))
_tmp2BB=1;if(!_tmp2BB){{void*_tmp2BC=(*((struct _tuple9*(*)(struct Cyc_List_List*x,
int n))Cyc_List_nth)(ts,(int)_tmp2B5)).f2;_npop_handler(0);return _tmp2BC;};
_pop_handler();}else{void*_tmp2BA=(void*)_exn_thrown;void*_tmp2BE=_tmp2BA;_LL194:
if(_tmp2BE != Cyc_List_Nth)goto _LL196;_LL195: {const char*_tmp6E9;void*_tmp6E8[2];
struct Cyc_Int_pa_struct _tmp6E7;struct Cyc_Int_pa_struct _tmp6E6;return(_tmp6E6.tag=
1,((_tmp6E6.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
ts),((_tmp6E7.tag=1,((_tmp6E7.f1=(unsigned long)((int)_tmp2B5),((_tmp6E8[0]=&
_tmp6E7,((_tmp6E8[1]=& _tmp6E6,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6E9="index is %d but tuple has only %d fields",
_tag_dyneither(_tmp6E9,sizeof(char),41))),_tag_dyneither(_tmp6E8,sizeof(void*),2)))))))))))));}
_LL196:;_LL197:(void)_throw(_tmp2BE);_LL193:;}}}}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp2C3=Cyc_Tcenv_get_fnrgn(te_orig);
struct Cyc_Tcenv_Tenv*_tmp2C4=Cyc_Tcenv_clear_notreadctxt(_tmp2C3,te_orig);Cyc_Tcexp_tcExp(
_tmp2C4,0,e1);Cyc_Tcexp_tcExp(_tmp2C4,0,e2);{void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!Cyc_Tcutil_coerce_sint_typ(
_tmp2C4,e2)){const char*_tmp6ED;void*_tmp6EC[1];struct Cyc_String_pa_struct _tmp6EB;
return(_tmp6EB.tag=0,((_tmp6EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp6EC[0]=& _tmp6EB,Cyc_Tcexp_expr_err(_tmp2C4,e2->loc,
topt,((_tmp6ED="expecting int subscript, found %s",_tag_dyneither(_tmp6ED,
sizeof(char),34))),_tag_dyneither(_tmp6EC,sizeof(void*),1)))))));}{void*_tmp2C8=
t1;struct Cyc_Absyn_PtrInfo _tmp2C9;void*_tmp2CA;struct Cyc_Absyn_Tqual _tmp2CB;
struct Cyc_Absyn_PtrAtts _tmp2CC;void*_tmp2CD;struct Cyc_Absyn_Conref*_tmp2CE;
struct Cyc_Absyn_Conref*_tmp2CF;struct Cyc_List_List*_tmp2D0;_LL199: if(_tmp2C8 <= (
void*)4)goto _LL19D;if(*((int*)_tmp2C8)!= 4)goto _LL19B;_tmp2C9=((struct Cyc_Absyn_PointerType_struct*)
_tmp2C8)->f1;_tmp2CA=(void*)_tmp2C9.elt_typ;_tmp2CB=_tmp2C9.elt_tq;_tmp2CC=
_tmp2C9.ptr_atts;_tmp2CD=(void*)_tmp2CC.rgn;_tmp2CE=_tmp2CC.bounds;_tmp2CF=
_tmp2CC.zero_term;_LL19A: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp2CF)){int emit_warning=1;{union Cyc_Absyn_Constraint_union _tmp2D1=(Cyc_Absyn_compress_conref(
_tmp2CE))->v;void*_tmp2D2;struct Cyc_Absyn_Exp*_tmp2D3;void*_tmp2D4;_LL1A0: if((
_tmp2D1.Eq_constr).tag != 0)goto _LL1A2;_tmp2D2=(_tmp2D1.Eq_constr).f1;if(_tmp2D2
<= (void*)1)goto _LL1A2;if(*((int*)_tmp2D2)!= 0)goto _LL1A2;_tmp2D3=((struct Cyc_Absyn_Upper_b_struct*)
_tmp2D2)->f1;_LL1A1: if(Cyc_Tcutil_is_const_exp(_tmp2C4,e2)){unsigned int _tmp2D6;
int _tmp2D7;struct _tuple7 _tmp2D5=Cyc_Evexp_eval_const_uint_exp(e2);_tmp2D6=
_tmp2D5.f1;_tmp2D7=_tmp2D5.f2;if(_tmp2D7){unsigned int _tmp2D9;int _tmp2DA;struct
_tuple7 _tmp2D8=Cyc_Evexp_eval_const_uint_exp(_tmp2D3);_tmp2D9=_tmp2D8.f1;_tmp2DA=
_tmp2D8.f2;if(_tmp2DA  && _tmp2D9 > _tmp2D6)emit_warning=0;}}goto _LL19F;_LL1A2: if((
_tmp2D1.Eq_constr).tag != 0)goto _LL1A4;_tmp2D4=(_tmp2D1.Eq_constr).f1;if((int)
_tmp2D4 != 0)goto _LL1A4;_LL1A3: emit_warning=0;goto _LL19F;_LL1A4:;_LL1A5: goto
_LL19F;_LL19F:;}if(emit_warning){const char*_tmp6F0;void*_tmp6EF;(_tmp6EF=0,Cyc_Tcutil_warn(
e2->loc,((_tmp6F0="subscript on thin, zero-terminated pointer could be expensive.",
_tag_dyneither(_tmp6F0,sizeof(char),63))),_tag_dyneither(_tmp6EF,sizeof(void*),0)));}}
else{if(Cyc_Tcutil_is_const_exp(_tmp2C4,e2)){unsigned int _tmp2DE;int _tmp2DF;
struct _tuple7 _tmp2DD=Cyc_Evexp_eval_const_uint_exp(e2);_tmp2DE=_tmp2DD.f1;
_tmp2DF=_tmp2DD.f2;if(_tmp2DF)Cyc_Tcutil_check_bound(loc,_tmp2DE,_tmp2CE);}else{
if(Cyc_Tcutil_is_bound_one(_tmp2CE) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp2CF)){const char*_tmp6F3;void*_tmp6F2;(_tmp6F2=0,Cyc_Tcutil_warn(
e1->loc,((_tmp6F3="subscript applied to pointer to one object",_tag_dyneither(
_tmp6F3,sizeof(char),43))),_tag_dyneither(_tmp6F2,sizeof(void*),0)));}Cyc_Tcutil_check_nonzero_bound(
loc,_tmp2CE);}}Cyc_Tcenv_check_rgn_accessible(_tmp2C4,loc,_tmp2CD);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp2CA),(void*)1)){const char*_tmp6F6;void*_tmp6F5;(_tmp6F5=
0,Cyc_Tcutil_terr(e1->loc,((_tmp6F6="can't subscript an abstract pointer",
_tag_dyneither(_tmp6F6,sizeof(char),36))),_tag_dyneither(_tmp6F5,sizeof(void*),0)));}
return _tmp2CA;_LL19B: if(*((int*)_tmp2C8)!= 9)goto _LL19D;_tmp2D0=((struct Cyc_Absyn_TupleType_struct*)
_tmp2C8)->f1;_LL19C: return Cyc_Tcexp_ithTupleType(_tmp2C4,loc,_tmp2D0,e2);_LL19D:;
_LL19E: {const char*_tmp6FA;void*_tmp6F9[1];struct Cyc_String_pa_struct _tmp6F8;
return(_tmp6F8.tag=0,((_tmp6F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp6F9[0]=& _tmp6F8,Cyc_Tcexp_expr_err(_tmp2C4,loc,
topt,((_tmp6FA="subscript applied to %s",_tag_dyneither(_tmp6FA,sizeof(char),24))),
_tag_dyneither(_tmp6F9,sizeof(void*),1)))))));}_LL198:;}}}static void*Cyc_Tcexp_tcTuple(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_List_List*
es);static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp2E7=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp2E8;
_LL1A7: if(_tmp2E7 <= (void*)4)goto _LL1A9;if(*((int*)_tmp2E7)!= 9)goto _LL1A9;
_tmp2E8=((struct Cyc_Absyn_TupleType_struct*)_tmp2E7)->f1;_LL1A8: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2E8)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1A6;for(0;es != 0;(es=es->tl,_tmp2E8=_tmp2E8->tl)){
void*_tmp2E9=(*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp2E8))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp2E9,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple9*)_tmp2E8->hd)).f2);{struct
_tuple9*_tmp6FD;struct Cyc_List_List*_tmp6FC;fields=((_tmp6FC=_cycalloc(sizeof(*
_tmp6FC)),((_tmp6FC->hd=((_tmp6FD=_cycalloc(sizeof(*_tmp6FD)),((_tmp6FD->f1=(*((
struct _tuple9*)_tmp2E8->hd)).f1,((_tmp6FD->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,_tmp6FD)))))),((_tmp6FC->tl=
fields,_tmp6FC))))));}}done=1;goto _LL1A6;_LL1A9:;_LL1AA: goto _LL1A6;_LL1A6:;}if(!
done)for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)
es->hd);{struct _tuple9*_tmp700;struct Cyc_List_List*_tmp6FF;fields=((_tmp6FF=
_cycalloc(sizeof(*_tmp6FF)),((_tmp6FF->hd=((_tmp700=_cycalloc(sizeof(*_tmp700)),((
_tmp700->f1=Cyc_Absyn_empty_tqual(0),((_tmp700->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,_tmp700)))))),((_tmp6FF->tl=
fields,_tmp6FF))))));}}{struct Cyc_Absyn_TupleType_struct _tmp703;struct Cyc_Absyn_TupleType_struct*
_tmp702;return(void*)((_tmp702=_cycalloc(sizeof(*_tmp702)),((_tmp702[0]=((
_tmp703.tag=9,((_tmp703.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmp703)))),_tmp702))));}}static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*t,struct Cyc_List_List*
des);static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _tuple2*t,struct Cyc_List_List*des){const char*_tmp706;void*
_tmp705;return(_tmp705=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp706="tcCompoundLit",
_tag_dyneither(_tmp706,sizeof(char),14))),_tag_dyneither(_tmp705,sizeof(void*),0)));}
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,int zero_term,struct Cyc_List_List*des);static void*Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**elt_topt,int
zero_term,struct Cyc_List_List*des){void*res_t2;struct _RegionHandle*_tmp2F2=Cyc_Tcenv_get_fnrgn(
te);{int _tmp2F3=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple8*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp2F2,(struct Cyc_Absyn_Exp*(*)(
struct _tuple8*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Const_e_struct
_tmp70C;union Cyc_Absyn_Cnst_union _tmp70B;struct Cyc_Absyn_Const_e_struct*_tmp70A;
struct Cyc_Absyn_Const_e_struct*_tmp2F4=(_tmp70A=_cycalloc(sizeof(*_tmp70A)),((
_tmp70A[0]=((_tmp70C.tag=0,((_tmp70C.f1=(union Cyc_Absyn_Cnst_union)(((_tmp70B.Int_c).tag=
2,(((_tmp70B.Int_c).f1=(void*)((void*)1),(((_tmp70B.Int_c).f2=_tmp2F3,_tmp70B)))))),
_tmp70C)))),_tmp70A)));struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)
_tmp2F4,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp2F5=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp2F3 - 1);if(!Cyc_Tcutil_is_zero(
_tmp2F5)){const char*_tmp70F;void*_tmp70E;(_tmp70E=0,Cyc_Tcutil_terr(_tmp2F5->loc,((
_tmp70F="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp70F,
sizeof(char),45))),_tag_dyneither(_tmp70E,sizeof(void*),0)));}}{struct Cyc_Core_Opt*
_tmp710;sz_exp->topt=((_tmp710=_cycalloc(sizeof(*_tmp710)),((_tmp710->v=(void*)
Cyc_Absyn_uint_typ,_tmp710))));}res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(
0),(struct Cyc_Absyn_Exp*)sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,
0);{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(
te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,
es)){const char*_tmp714;void*_tmp713[1];struct Cyc_String_pa_struct _tmp712;(
_tmp712.tag=0,((_tmp712.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
res)),((_tmp713[0]=& _tmp712,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,((_tmp714="elements of array do not all have the same type (%s)",
_tag_dyneither(_tmp714,sizeof(char),53))),_tag_dyneither(_tmp713,sizeof(void*),1)))))));}}{
int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((
struct _tuple8*)des->hd)).f1;if(ds != 0){void*_tmp2FF=(void*)ds->hd;struct Cyc_Absyn_Exp*
_tmp300;_LL1AC: if(*((int*)_tmp2FF)!= 1)goto _LL1AE;_LL1AD:{const char*_tmp717;void*
_tmp716;(_tmp716=0,Cyc_Tcutil_terr(loc,((_tmp717="only array index designators are supported",
_tag_dyneither(_tmp717,sizeof(char),43))),_tag_dyneither(_tmp716,sizeof(void*),0)));}
goto _LL1AB;_LL1AE: if(*((int*)_tmp2FF)!= 0)goto _LL1AB;_tmp300=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp2FF)->f1;_LL1AF: Cyc_Tcexp_tcExpInitializer(te,0,_tmp300);{unsigned int
_tmp304;int _tmp305;struct _tuple7 _tmp303=Cyc_Evexp_eval_const_uint_exp(_tmp300);
_tmp304=_tmp303.f1;_tmp305=_tmp303.f2;if(!_tmp305){const char*_tmp71A;void*
_tmp719;(_tmp719=0,Cyc_Tcutil_terr(_tmp300->loc,((_tmp71A="index designator cannot use sizeof or offsetof",
_tag_dyneither(_tmp71A,sizeof(char),47))),_tag_dyneither(_tmp719,sizeof(void*),0)));}
else{if(_tmp304 != offset){const char*_tmp71F;void*_tmp71E[2];struct Cyc_Int_pa_struct
_tmp71D;struct Cyc_Int_pa_struct _tmp71C;(_tmp71C.tag=1,((_tmp71C.f1=(
unsigned long)((int)_tmp304),((_tmp71D.tag=1,((_tmp71D.f1=(unsigned long)offset,((
_tmp71E[0]=& _tmp71D,((_tmp71E[1]=& _tmp71C,Cyc_Tcutil_terr(_tmp300->loc,((_tmp71F="expecting index designator %d but found %d",
_tag_dyneither(_tmp71F,sizeof(char),43))),_tag_dyneither(_tmp71E,sizeof(void*),2)))))))))))));}}
goto _LL1AB;}_LL1AB:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term);static
void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*
body,int*is_zero_term){Cyc_Tcexp_tcExp(te,0,bound);{void*_tmp30C=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(bound->topt))->v);_LL1B1: if(_tmp30C <= (
void*)4)goto _LL1B3;if(*((int*)_tmp30C)!= 18)goto _LL1B3;_LL1B2: goto _LL1B0;_LL1B3:;
_LL1B4: if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){const char*_tmp723;void*_tmp722[
1];struct Cyc_String_pa_struct _tmp721;(_tmp721.tag=0,((_tmp721.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v)),((_tmp722[0]=& _tmp721,Cyc_Tcutil_terr(bound->loc,((
_tmp723="expecting unsigned int, found %s",_tag_dyneither(_tmp723,sizeof(char),
33))),_tag_dyneither(_tmp722,sizeof(void*),1)))))));}_LL1B0:;}if(!(vd->tq).real_const){
const char*_tmp726;void*_tmp725;(_tmp725=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp726="comprehension index variable is not declared const!",
_tag_dyneither(_tmp726,sizeof(char),52))),_tag_dyneither(_tmp725,sizeof(void*),0)));}{
struct _RegionHandle*_tmp312=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(
_tmp312,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(_tmp312,loc,te2);te2=Cyc_Tcenv_add_local_var(
_tmp312,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*_tmp313=te2;struct Cyc_Absyn_PtrInfo
pinfo;void**_tmp314=0;struct Cyc_Absyn_Tqual*_tmp315=0;struct Cyc_Absyn_Conref**
_tmp316=0;if(topt != 0){void*_tmp317=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp318;struct Cyc_Absyn_ArrayInfo _tmp319;void*_tmp31A;void**_tmp31B;struct Cyc_Absyn_Tqual
_tmp31C;struct Cyc_Absyn_Tqual*_tmp31D;struct Cyc_Absyn_Exp*_tmp31E;struct Cyc_Absyn_Conref*
_tmp31F;struct Cyc_Absyn_Conref**_tmp320;_LL1B6: if(_tmp317 <= (void*)4)goto _LL1BA;
if(*((int*)_tmp317)!= 4)goto _LL1B8;_tmp318=((struct Cyc_Absyn_PointerType_struct*)
_tmp317)->f1;_LL1B7: pinfo=_tmp318;_tmp314=(void**)((void**)((void*)& pinfo.elt_typ));
_tmp315=(struct Cyc_Absyn_Tqual*)& pinfo.elt_tq;_tmp316=(struct Cyc_Absyn_Conref**)&(
pinfo.ptr_atts).zero_term;goto _LL1B5;_LL1B8: if(*((int*)_tmp317)!= 7)goto _LL1BA;
_tmp319=((struct Cyc_Absyn_ArrayType_struct*)_tmp317)->f1;_tmp31A=(void*)_tmp319.elt_type;
_tmp31B=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp317)->f1).elt_type;
_tmp31C=_tmp319.tq;_tmp31D=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp317)->f1).tq;_tmp31E=_tmp319.num_elts;_tmp31F=_tmp319.zero_term;_tmp320=(
struct Cyc_Absyn_Conref**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp317)->f1).zero_term;
_LL1B9: _tmp314=(void**)_tmp31B;_tmp315=(struct Cyc_Absyn_Tqual*)_tmp31D;_tmp316=(
struct Cyc_Absyn_Conref**)_tmp320;goto _LL1B5;_LL1BA:;_LL1BB: goto _LL1B5;_LL1B5:;}{
void*t=Cyc_Tcexp_tcExp(_tmp313,_tmp314,body);if(_tmp313->le == 0){if(!Cyc_Tcutil_is_const_exp(
_tmp313,bound)){const char*_tmp729;void*_tmp728;(_tmp728=0,Cyc_Tcutil_terr(bound->loc,((
_tmp729="bound is not constant",_tag_dyneither(_tmp729,sizeof(char),22))),
_tag_dyneither(_tmp728,sizeof(void*),0)));}if(!Cyc_Tcutil_is_const_exp(_tmp313,
body)){const char*_tmp72C;void*_tmp72B;(_tmp72B=0,Cyc_Tcutil_terr(bound->loc,((
_tmp72C="body is not constant",_tag_dyneither(_tmp72C,sizeof(char),21))),
_tag_dyneither(_tmp72B,sizeof(void*),0)));}}if(_tmp316 != 0  && ((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,*_tmp316)){struct Cyc_Absyn_Exp*
_tmp325=Cyc_Absyn_uint_exp(1,0);{struct Cyc_Core_Opt*_tmp72D;_tmp325->topt=((
_tmp72D=_cycalloc(sizeof(*_tmp72D)),((_tmp72D->v=(void*)Cyc_Absyn_uint_typ,
_tmp72D))));}bound=Cyc_Absyn_add_exp(bound,_tmp325,0);{struct Cyc_Core_Opt*
_tmp72E;bound->topt=((_tmp72E=_cycalloc(sizeof(*_tmp72E)),((_tmp72E->v=(void*)
Cyc_Absyn_uint_typ,_tmp72E))));}*is_zero_term=1;}{void*_tmp328=Cyc_Absyn_array_typ(
t,_tmp315 == 0?Cyc_Absyn_empty_tqual(0):*_tmp315,(struct Cyc_Absyn_Exp*)bound,
_tmp316 == 0?Cyc_Absyn_false_conref:*_tmp316,0);return _tmp328;}}}}}struct _tuple10{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**
tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**
ad_opt);static void*Cyc_Tcexp_tcStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*
args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*ad;if(*ad_opt != 
0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{struct _handler_cons
_tmp329;_push_handler(& _tmp329);{int _tmp32B=0;if(setjmp(_tmp329.handler))_tmp32B=
1;if(!_tmp32B){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;_pop_handler();}else{
void*_tmp32A=(void*)_exn_thrown;void*_tmp32D=_tmp32A;_LL1BD: if(_tmp32D != Cyc_Dict_Absent)
goto _LL1BF;_LL1BE:{const char*_tmp732;void*_tmp731[1];struct Cyc_String_pa_struct
_tmp730;(_tmp730.tag=0,((_tmp730.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(*tn)),((_tmp731[0]=& _tmp730,Cyc_Tcutil_terr(loc,((
_tmp732="unbound struct name %s",_tag_dyneither(_tmp732,sizeof(char),23))),
_tag_dyneither(_tmp731,sizeof(void*),1)))))));}return topt != 0?*topt:(void*)0;
_LL1BF:;_LL1C0:(void)_throw(_tmp32D);_LL1BC:;}}}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)
ad;*tn=ad->name;}if((void*)ad->kind == (void*)1){const char*_tmp735;void*_tmp734;(
_tmp734=0,Cyc_Tcutil_terr(loc,((_tmp735="expecting struct but found union",
_tag_dyneither(_tmp735,sizeof(char),33))),_tag_dyneither(_tmp734,sizeof(void*),0)));}
if(ad->impl == 0){{const char*_tmp738;void*_tmp737;(_tmp737=0,Cyc_Tcutil_terr(loc,((
_tmp738="can't build abstract struct",_tag_dyneither(_tmp738,sizeof(char),28))),
_tag_dyneither(_tmp737,sizeof(void*),0)));}return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}{struct _RegionHandle*_tmp335=Cyc_Tcenv_get_fnrgn(te);struct _tuple4 _tmp739;
struct _tuple4 _tmp336=(_tmp739.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp739.f2=
_tmp335,_tmp739)));struct Cyc_List_List*_tmp337=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp335,Cyc_Tcutil_r_make_inst_var,&
_tmp336,ad->tvs);struct Cyc_List_List*_tmp338=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp335,Cyc_Tcutil_r_make_inst_var,&
_tmp336,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp339=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp337);struct Cyc_List_List*_tmp33A=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp338);struct Cyc_List_List*_tmp33B=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp335,_tmp337,_tmp338);struct Cyc_Absyn_AggrType_struct _tmp748;union Cyc_Absyn_AggrInfoU_union
_tmp747;struct Cyc_Absyn_Aggrdecl**_tmp746;struct Cyc_Absyn_AggrInfo _tmp745;struct
Cyc_Absyn_AggrType_struct*_tmp744;struct Cyc_Absyn_AggrType_struct*_tmp33C=(
_tmp744=_cycalloc(sizeof(*_tmp744)),((_tmp744[0]=((_tmp748.tag=10,((_tmp748.f1=((
_tmp745.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((_tmp747.KnownAggr).tag=1,(((
_tmp747.KnownAggr).f1=((_tmp746=_cycalloc(sizeof(*_tmp746)),((_tmp746[0]=ad,
_tmp746)))),_tmp747)))),((_tmp745.targs=_tmp339,_tmp745)))),_tmp748)))),_tmp744)));
struct Cyc_List_List*_tmp33D=*ts;struct Cyc_List_List*_tmp33E=_tmp33A;while(
_tmp33D != 0  && _tmp33E != 0){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)0,1,(void*)_tmp33D->hd);Cyc_Tcutil_unify((void*)_tmp33D->hd,(void*)
_tmp33E->hd);_tmp33D=_tmp33D->tl;_tmp33E=_tmp33E->tl;}if(_tmp33D != 0){const char*
_tmp74B;void*_tmp74A;(_tmp74A=0,Cyc_Tcutil_terr(loc,((_tmp74B="too many explicit witness types",
_tag_dyneither(_tmp74B,sizeof(char),32))),_tag_dyneither(_tmp74A,sizeof(void*),0)));}*
ts=_tmp33A;if(topt != 0)Cyc_Tcutil_unify((void*)_tmp33C,*topt);{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
_tmp335,loc,args,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){struct _tuple10 _tmp342;struct Cyc_Absyn_Aggrfield*
_tmp343;struct Cyc_Absyn_Exp*_tmp344;struct _tuple10*_tmp341=(struct _tuple10*)
fields->hd;_tmp342=*_tmp341;_tmp343=_tmp342.f1;_tmp344=_tmp342.f2;{void*_tmp345=
Cyc_Tcutil_rsubstitute(_tmp335,_tmp33B,(void*)_tmp343->type);Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp345,_tmp344);if(!Cyc_Tcutil_coerce_arg(te,_tmp344,_tmp345)){{
const char*_tmp752;void*_tmp751[4];struct Cyc_String_pa_struct _tmp750;struct Cyc_String_pa_struct
_tmp74F;struct Cyc_String_pa_struct _tmp74E;struct Cyc_String_pa_struct _tmp74D;(
_tmp74D.tag=0,((_tmp74D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp344->topt))->v)),((_tmp74E.tag=0,((
_tmp74E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp345)),((_tmp74F.tag=0,((_tmp74F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp750.tag=0,((_tmp750.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp343->name),((_tmp751[0]=&
_tmp750,((_tmp751[1]=& _tmp74F,((_tmp751[2]=& _tmp74E,((_tmp751[3]=& _tmp74D,Cyc_Tcutil_terr(
_tmp344->loc,((_tmp752="field %s of struct %s expects type %s != %s",
_tag_dyneither(_tmp752,sizeof(char),44))),_tag_dyneither(_tmp751,sizeof(void*),4)))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}}{struct Cyc_List_List*_tmp34C=0;{struct Cyc_List_List*
_tmp34D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;
_tmp34D != 0;_tmp34D=_tmp34D->tl){struct _tuple0*_tmp755;struct Cyc_List_List*
_tmp754;_tmp34C=((_tmp754=_cycalloc(sizeof(*_tmp754)),((_tmp754->hd=((_tmp755=
_cycalloc(sizeof(*_tmp755)),((_tmp755->f1=Cyc_Tcutil_rsubstitute(_tmp335,_tmp33B,(*((
struct _tuple0*)_tmp34D->hd)).f1),((_tmp755->f2=Cyc_Tcutil_rsubstitute(_tmp335,
_tmp33B,(*((struct _tuple0*)_tmp34D->hd)).f2),_tmp755)))))),((_tmp754->tl=_tmp34C,
_tmp754))))));}}_tmp34C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp34C);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp34C);return(void*)_tmp33C;}}}}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args);static void*Cyc_Tcexp_tcAnonStruct(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*
args){struct _RegionHandle*_tmp356=Cyc_Tcenv_get_fnrgn(te);{void*_tmp357=Cyc_Tcutil_compress(
ts);void*_tmp358;struct Cyc_List_List*_tmp359;_LL1C2: if(_tmp357 <= (void*)4)goto
_LL1C4;if(*((int*)_tmp357)!= 11)goto _LL1C4;_tmp358=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp357)->f1;_tmp359=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp357)->f2;_LL1C3:
if(_tmp358 == (void*)1){const char*_tmp758;void*_tmp757;(_tmp757=0,Cyc_Tcutil_terr(
loc,((_tmp758="expecting struct but found union",_tag_dyneither(_tmp758,sizeof(
char),33))),_tag_dyneither(_tmp757,sizeof(void*),0)));}{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
_tmp356,loc,args,_tmp359);for(0;fields != 0;fields=fields->tl){struct _tuple10
_tmp35D;struct Cyc_Absyn_Aggrfield*_tmp35E;struct Cyc_Absyn_Exp*_tmp35F;struct
_tuple10*_tmp35C=(struct _tuple10*)fields->hd;_tmp35D=*_tmp35C;_tmp35E=_tmp35D.f1;
_tmp35F=_tmp35D.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)((void**)((void*)&
_tmp35E->type)),_tmp35F);if(!Cyc_Tcutil_coerce_arg(te,_tmp35F,(void*)_tmp35E->type)){{
const char*_tmp75E;void*_tmp75D[3];struct Cyc_String_pa_struct _tmp75C;struct Cyc_String_pa_struct
_tmp75B;struct Cyc_String_pa_struct _tmp75A;(_tmp75A.tag=0,((_tmp75A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp35F->topt))->v)),((_tmp75B.tag=0,((_tmp75B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmp35E->type)),((
_tmp75C.tag=0,((_tmp75C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp35E->name),((
_tmp75D[0]=& _tmp75C,((_tmp75D[1]=& _tmp75B,((_tmp75D[2]=& _tmp75A,Cyc_Tcutil_terr(
_tmp35F->loc,((_tmp75E="field %s of struct expects type %s != %s",_tag_dyneither(
_tmp75E,sizeof(char),41))),_tag_dyneither(_tmp75D,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}goto _LL1C1;}_LL1C4:;_LL1C5: {const char*_tmp761;
void*_tmp760;(_tmp760=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp761="tcAnonStruct: wrong type",_tag_dyneither(_tmp761,
sizeof(char),25))),_tag_dyneither(_tmp760,sizeof(void*),0)));}_LL1C1:;}return ts;}
static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*
tud,struct Cyc_Absyn_Tunionfield*tuf);static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
es,struct Cyc_Absyn_Tuniondecl*tud,struct Cyc_Absyn_Tunionfield*tuf){struct
_RegionHandle*_tmp367=Cyc_Tcenv_get_fnrgn(te);struct _tuple4 _tmp762;struct _tuple4
_tmp368=(_tmp762.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp762.f2=_tmp367,_tmp762)));
struct Cyc_List_List*_tmp369=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp367,Cyc_Tcutil_r_make_inst_var,&
_tmp368,tud->tvs);struct Cyc_List_List*_tmp36A=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp369);struct Cyc_Absyn_TunionFieldType_struct _tmp76C;union Cyc_Absyn_TunionFieldInfoU_union
_tmp76B;struct Cyc_Absyn_TunionFieldInfo _tmp76A;struct Cyc_Absyn_TunionFieldType_struct*
_tmp769;void*res=(void*)((_tmp769=_cycalloc(sizeof(*_tmp769)),((_tmp769[0]=((
_tmp76C.tag=3,((_tmp76C.f1=((_tmp76A.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)(((
_tmp76B.KnownTunionfield).tag=1,(((_tmp76B.KnownTunionfield).f1=tud,(((_tmp76B.KnownTunionfield).f2=
tuf,_tmp76B)))))),((_tmp76A.targs=_tmp36A,_tmp76A)))),_tmp76C)))),_tmp769))));
if(topt != 0){void*_tmp36B=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo
_tmp36C;struct Cyc_List_List*_tmp36D;struct Cyc_Core_Opt*_tmp36E;_LL1C7: if(_tmp36B
<= (void*)4)goto _LL1CB;if(*((int*)_tmp36B)!= 3)goto _LL1C9;_LL1C8: Cyc_Tcutil_unify(*
topt,res);goto _LL1C6;_LL1C9: if(*((int*)_tmp36B)!= 2)goto _LL1CB;_tmp36C=((struct
Cyc_Absyn_TunionType_struct*)_tmp36B)->f1;_tmp36D=_tmp36C.targs;_tmp36E=_tmp36C.rgn;
_LL1CA:{struct Cyc_List_List*a=_tmp36A;for(0;a != 0  && _tmp36D != 0;(a=a->tl,
_tmp36D=_tmp36D->tl)){Cyc_Tcutil_unify((void*)a->hd,(void*)_tmp36D->hd);}}if(tud->is_flat
 || tuf->typs == 0  && es == 0){{struct Cyc_Core_Opt*_tmp76D;e->topt=((_tmp76D=
_cycalloc(sizeof(*_tmp76D)),((_tmp76D->v=(void*)res,_tmp76D))));}{struct Cyc_Absyn_TunionType_struct
_tmp77C;union Cyc_Absyn_TunionInfoU_union _tmp77B;struct Cyc_Absyn_Tuniondecl**
_tmp77A;struct Cyc_Absyn_TunionInfo _tmp779;struct Cyc_Absyn_TunionType_struct*
_tmp778;res=(void*)((_tmp778=_cycalloc(sizeof(*_tmp778)),((_tmp778[0]=((_tmp77C.tag=
2,((_tmp77C.f1=((_tmp779.tunion_info=(union Cyc_Absyn_TunionInfoU_union)(((
_tmp77B.KnownTunion).tag=1,(((_tmp77B.KnownTunion).f1=((_tmp77A=_cycalloc(
sizeof(*_tmp77A)),((_tmp77A[0]=tud,_tmp77A)))),_tmp77B)))),((_tmp779.targs=
_tmp36A,((_tmp779.rgn=_tmp36E,_tmp779)))))),_tmp77C)))),_tmp778))));}Cyc_Tcutil_unchecked_cast(
te,e,res,(void*)1);}goto _LL1C6;_LL1CB:;_LL1CC: goto _LL1C6;_LL1C6:;}{struct Cyc_List_List*
ts=tuf->typs;for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(
struct Cyc_Absyn_Exp*)es->hd;void*t=Cyc_Tcutil_rsubstitute(_tmp367,_tmp369,(*((
struct _tuple9*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t)){{const char*_tmp784;const char*_tmp783;void*_tmp782[3];struct Cyc_String_pa_struct
_tmp781;struct Cyc_String_pa_struct _tmp780;struct Cyc_String_pa_struct _tmp77F;(
_tmp77F.tag=0,((_tmp77F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(e->topt
== 0?(struct _dyneither_ptr)((_tmp784="?",_tag_dyneither(_tmp784,sizeof(char),2))):
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v))),((
_tmp780.tag=0,((_tmp780.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp781.tag=0,((_tmp781.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp782[0]=& _tmp781,((_tmp782[1]=& _tmp780,((
_tmp782[2]=& _tmp77F,Cyc_Tcutil_terr(e->loc,((_tmp783="tunion constructor %s expects argument of type %s but this argument has type %s",
_tag_dyneither(_tmp783,sizeof(char),80))),_tag_dyneither(_tmp782,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}if(es != 0){const char*_tmp788;void*_tmp787[1];
struct Cyc_String_pa_struct _tmp786;return(_tmp786.tag=0,((_tmp786.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp787[0]=& _tmp786,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,
topt,((_tmp788="too many arguments for tunion constructor %s",_tag_dyneither(
_tmp788,sizeof(char),45))),_tag_dyneither(_tmp787,sizeof(void*),1)))))));}if(ts
!= 0){const char*_tmp78C;void*_tmp78B[1];struct Cyc_String_pa_struct _tmp78A;return(
_tmp78A.tag=0,((_tmp78A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name)),((_tmp78B[0]=& _tmp78A,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp78C="too few arguments for tunion constructor %s",
_tag_dyneither(_tmp78C,sizeof(char),44))),_tag_dyneither(_tmp78B,sizeof(void*),1)))))));}
return res;}}static int Cyc_Tcexp_zeroable_type(void*t);static int Cyc_Tcexp_zeroable_type(
void*t){void*_tmp386=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp387;
struct Cyc_Absyn_PtrAtts _tmp388;struct Cyc_Absyn_Conref*_tmp389;struct Cyc_Absyn_ArrayInfo
_tmp38A;void*_tmp38B;struct Cyc_List_List*_tmp38C;struct Cyc_Absyn_AggrInfo _tmp38D;
union Cyc_Absyn_AggrInfoU_union _tmp38E;struct Cyc_List_List*_tmp38F;struct Cyc_List_List*
_tmp390;_LL1CE: if((int)_tmp386 != 0)goto _LL1D0;_LL1CF: return 1;_LL1D0: if(_tmp386 <= (
void*)4)goto _LL1DC;if(*((int*)_tmp386)!= 0)goto _LL1D2;_LL1D1: goto _LL1D3;_LL1D2:
if(*((int*)_tmp386)!= 1)goto _LL1D4;_LL1D3: goto _LL1D5;_LL1D4: if(*((int*)_tmp386)
!= 2)goto _LL1D6;_LL1D5: goto _LL1D7;_LL1D6: if(*((int*)_tmp386)!= 3)goto _LL1D8;
_LL1D7: return 0;_LL1D8: if(*((int*)_tmp386)!= 4)goto _LL1DA;_tmp387=((struct Cyc_Absyn_PointerType_struct*)
_tmp386)->f1;_tmp388=_tmp387.ptr_atts;_tmp389=_tmp388.nullable;_LL1D9: return((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp389);_LL1DA: if(*((
int*)_tmp386)!= 5)goto _LL1DC;_LL1DB: goto _LL1DD;_LL1DC: if((int)_tmp386 != 1)goto
_LL1DE;_LL1DD: goto _LL1DF;_LL1DE: if(_tmp386 <= (void*)4)goto _LL1F8;if(*((int*)
_tmp386)!= 6)goto _LL1E0;_LL1DF: return 1;_LL1E0: if(*((int*)_tmp386)!= 7)goto _LL1E2;
_tmp38A=((struct Cyc_Absyn_ArrayType_struct*)_tmp386)->f1;_tmp38B=(void*)_tmp38A.elt_type;
_LL1E1: return Cyc_Tcexp_zeroable_type(_tmp38B);_LL1E2: if(*((int*)_tmp386)!= 8)
goto _LL1E4;_LL1E3: return 0;_LL1E4: if(*((int*)_tmp386)!= 9)goto _LL1E6;_tmp38C=((
struct Cyc_Absyn_TupleType_struct*)_tmp386)->f1;_LL1E5: for(0;(unsigned int)
_tmp38C;_tmp38C=_tmp38C->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple9*)
_tmp38C->hd)).f2))return 0;}return 1;_LL1E6: if(*((int*)_tmp386)!= 10)goto _LL1E8;
_tmp38D=((struct Cyc_Absyn_AggrType_struct*)_tmp386)->f1;_tmp38E=_tmp38D.aggr_info;
_tmp38F=_tmp38D.targs;_LL1E7: {struct Cyc_Absyn_Aggrdecl*_tmp391=Cyc_Absyn_get_known_aggrdecl(
_tmp38E);if(_tmp391->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp391->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp392=_new_region("r");
struct _RegionHandle*r=& _tmp392;_push_region(r);{struct Cyc_List_List*_tmp393=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp391->tvs,_tmp38F);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp391->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp393,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp394=0;_npop_handler(0);
return _tmp394;}}}{int _tmp395=1;_npop_handler(0);return _tmp395;}};_pop_region(r);}}
_LL1E8: if(*((int*)_tmp386)!= 12)goto _LL1EA;_LL1E9: return 1;_LL1EA: if(*((int*)
_tmp386)!= 18)goto _LL1EC;_LL1EB: return 1;_LL1EC: if(*((int*)_tmp386)!= 11)goto
_LL1EE;_tmp390=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp386)->f2;_LL1ED: for(0;
_tmp390 != 0;_tmp390=_tmp390->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp390->hd)->type))return 0;}return 1;_LL1EE: if(*((int*)_tmp386)!= 13)goto _LL1F0;
_LL1EF: return 1;_LL1F0: if(*((int*)_tmp386)!= 16)goto _LL1F2;_LL1F1: return 0;_LL1F2:
if(*((int*)_tmp386)!= 15)goto _LL1F4;_LL1F3: return 0;_LL1F4: if(*((int*)_tmp386)!= 
14)goto _LL1F6;_LL1F5: return 0;_LL1F6: if(*((int*)_tmp386)!= 17)goto _LL1F8;_LL1F7:
goto _LL1F9;_LL1F8: if((int)_tmp386 != 2)goto _LL1FA;_LL1F9: goto _LL1FB;_LL1FA: if((
int)_tmp386 != 3)goto _LL1FC;_LL1FB: goto _LL1FD;_LL1FC: if(_tmp386 <= (void*)4)goto
_LL1FE;if(*((int*)_tmp386)!= 19)goto _LL1FE;_LL1FD: goto _LL1FF;_LL1FE: if(_tmp386 <= (
void*)4)goto _LL200;if(*((int*)_tmp386)!= 20)goto _LL200;_LL1FF: goto _LL201;_LL200:
if(_tmp386 <= (void*)4)goto _LL1CD;if(*((int*)_tmp386)!= 21)goto _LL1CD;_LL201: {
const char*_tmp790;void*_tmp78F[1];struct Cyc_String_pa_struct _tmp78E;(_tmp78E.tag=
0,((_tmp78E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp78F[0]=& _tmp78E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp790="bad type `%s' in zeroable type",_tag_dyneither(
_tmp790,sizeof(char),31))),_tag_dyneither(_tmp78F,sizeof(void*),1)))))));}_LL1CD:;}
static void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*
loc,void**topt,void*t);static void Cyc_Tcexp_check_malloc_type(int allow_zero,
struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || 
allow_zero  && Cyc_Tcexp_zeroable_type(t))return;if(topt != 0){void*_tmp399=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp39A;void*_tmp39B;_LL203: if(_tmp399 <= (void*)4)
goto _LL205;if(*((int*)_tmp399)!= 4)goto _LL205;_tmp39A=((struct Cyc_Absyn_PointerType_struct*)
_tmp399)->f1;_tmp39B=(void*)_tmp39A.elt_typ;_LL204: Cyc_Tcutil_unify(_tmp39B,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL202;_LL205:;_LL206: goto _LL202;_LL202:;}{const char*_tmp799;const char*
_tmp798;const char*_tmp797;void*_tmp796[2];struct Cyc_String_pa_struct _tmp795;
struct Cyc_String_pa_struct _tmp794;(_tmp794.tag=0,((_tmp794.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp795.tag=0,((
_tmp795.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct
_dyneither_ptr)((_tmp798="calloc",_tag_dyneither(_tmp798,sizeof(char),7))):(
struct _dyneither_ptr)((_tmp799="malloc",_tag_dyneither(_tmp799,sizeof(char),7))))),((
_tmp796[0]=& _tmp795,((_tmp796[1]=& _tmp794,Cyc_Tcutil_terr(loc,((_tmp797="%s cannot be used with type %s\n\t(type needs initialization)",
_tag_dyneither(_tmp797,sizeof(char),60))),_tag_dyneither(_tmp796,sizeof(void*),2)))))))))))));}}
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*ropt,void***t,struct Cyc_Absyn_Exp**e,int*
is_calloc,int*is_fat);static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*ropt,void***t,
struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)2;if(ropt != 0){
struct Cyc_Absyn_RgnHandleType_struct _tmp79C;struct Cyc_Absyn_RgnHandleType_struct*
_tmp79B;void*expected_type=(void*)((_tmp79B=_cycalloc(sizeof(*_tmp79B)),((
_tmp79B[0]=((_tmp79C.tag=14,((_tmp79C.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp79C)))),_tmp79B))));void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)ropt);
void*_tmp3A2=Cyc_Tcutil_compress(handle_type);void*_tmp3A3;_LL208: if(_tmp3A2 <= (
void*)4)goto _LL20A;if(*((int*)_tmp3A2)!= 14)goto _LL20A;_tmp3A3=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp3A2)->f1;_LL209: rgn=_tmp3A3;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL207;_LL20A:;_LL20B:{const char*_tmp7A0;void*_tmp79F[1];struct
Cyc_String_pa_struct _tmp79E;(_tmp79E.tag=0,((_tmp79E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type)),((_tmp79F[0]=& _tmp79E,
Cyc_Tcutil_terr(ropt->loc,((_tmp7A0="expecting region_t type but found %s",
_tag_dyneither(_tmp7A0,sizeof(char),37))),_tag_dyneither(_tmp79F,sizeof(void*),1)))))));}
goto _LL207;_LL207:;}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*
elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*is_calloc){if(*t == 0){const
char*_tmp7A3;void*_tmp7A2;(_tmp7A2=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7A3="calloc with empty type",
_tag_dyneither(_tmp7A3,sizeof(char),23))),_tag_dyneither(_tmp7A2,sizeof(void*),0)));}
elt_type=*((void**)_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;one_elt=0;}else{void*_tmp3AB=(void*)(*e)->r;void*_tmp3AC;void*
_tmp3AD;struct Cyc_List_List*_tmp3AE;struct Cyc_List_List _tmp3AF;struct Cyc_Absyn_Exp*
_tmp3B0;struct Cyc_List_List*_tmp3B1;struct Cyc_List_List _tmp3B2;struct Cyc_Absyn_Exp*
_tmp3B3;struct Cyc_List_List*_tmp3B4;_LL20D: if(*((int*)_tmp3AB)!= 18)goto _LL20F;
_tmp3AC=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3AB)->f1;_LL20E:
elt_type=_tmp3AC;{void**_tmp7A4;*t=(void**)((_tmp7A4=_cycalloc(sizeof(*_tmp7A4)),((
_tmp7A4[0]=elt_type,_tmp7A4))));}num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL20C;_LL20F: if(*((int*)
_tmp3AB)!= 3)goto _LL211;_tmp3AD=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3AB)->f1;if((int)_tmp3AD != 1)goto _LL211;_tmp3AE=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3AB)->f2;if(_tmp3AE == 0)goto _LL211;_tmp3AF=*_tmp3AE;_tmp3B0=(struct Cyc_Absyn_Exp*)
_tmp3AF.hd;_tmp3B1=_tmp3AF.tl;if(_tmp3B1 == 0)goto _LL211;_tmp3B2=*_tmp3B1;_tmp3B3=(
struct Cyc_Absyn_Exp*)_tmp3B2.hd;_tmp3B4=_tmp3B2.tl;if(_tmp3B4 != 0)goto _LL211;
_LL210:{struct _tuple0 _tmp7A5;struct _tuple0 _tmp3B7=(_tmp7A5.f1=(void*)_tmp3B0->r,((
_tmp7A5.f2=(void*)_tmp3B3->r,_tmp7A5)));void*_tmp3B8;void*_tmp3B9;void*_tmp3BA;
void*_tmp3BB;_LL214: _tmp3B8=_tmp3B7.f1;if(*((int*)_tmp3B8)!= 18)goto _LL216;
_tmp3B9=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3B8)->f1;_LL215: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3B9);elt_type=_tmp3B9;{void**_tmp7A6;*t=(void**)((_tmp7A6=
_cycalloc(sizeof(*_tmp7A6)),((_tmp7A6[0]=elt_type,_tmp7A6))));}num_elts=_tmp3B3;
one_elt=0;goto _LL213;_LL216: _tmp3BA=_tmp3B7.f2;if(*((int*)_tmp3BA)!= 18)goto
_LL218;_tmp3BB=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3BA)->f1;_LL217:
Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3BB);elt_type=_tmp3BB;{void**_tmp7A7;*
t=(void**)((_tmp7A7=_cycalloc(sizeof(*_tmp7A7)),((_tmp7A7[0]=elt_type,_tmp7A7))));}
num_elts=_tmp3B0;one_elt=0;goto _LL213;_LL218:;_LL219: goto No_sizeof;_LL213:;}goto
_LL20C;_LL211:;_LL212: No_sizeof: elt_type=Cyc_Absyn_char_typ;{void**_tmp7A8;*t=(
void**)((_tmp7A8=_cycalloc(sizeof(*_tmp7A8)),((_tmp7A8[0]=elt_type,_tmp7A8))));}
num_elts=*e;one_elt=0;goto _LL20C;_LL20C:;}*e=num_elts;*is_fat=!one_elt;{void*
_tmp3BF=elt_type;struct Cyc_Absyn_AggrInfo _tmp3C0;union Cyc_Absyn_AggrInfoU_union
_tmp3C1;struct Cyc_Absyn_Aggrdecl**_tmp3C2;struct Cyc_Absyn_Aggrdecl*_tmp3C3;
_LL21B: if(_tmp3BF <= (void*)4)goto _LL21D;if(*((int*)_tmp3BF)!= 10)goto _LL21D;
_tmp3C0=((struct Cyc_Absyn_AggrType_struct*)_tmp3BF)->f1;_tmp3C1=_tmp3C0.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp3BF)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL21D;_tmp3C2=(_tmp3C1.KnownAggr).f1;_tmp3C3=*_tmp3C2;_LL21C: if(
_tmp3C3->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3C3->impl))->exist_vars
!= 0){const char*_tmp7AB;void*_tmp7AA;(_tmp7AA=0,Cyc_Tcutil_terr(loc,((_tmp7AB="malloc with existential types not yet implemented",
_tag_dyneither(_tmp7AB,sizeof(char),50))),_tag_dyneither(_tmp7AA,sizeof(void*),0)));}
goto _LL21A;_LL21D:;_LL21E: goto _LL21A;_LL21A:;}{void*(*_tmp3C6)(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term)=Cyc_Absyn_at_typ;
struct Cyc_Absyn_Conref*_tmp3C7=Cyc_Absyn_false_conref;if(topt != 0){void*_tmp3C8=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp3C9;struct Cyc_Absyn_PtrAtts
_tmp3CA;struct Cyc_Absyn_Conref*_tmp3CB;struct Cyc_Absyn_Conref*_tmp3CC;struct Cyc_Absyn_Conref*
_tmp3CD;_LL220: if(_tmp3C8 <= (void*)4)goto _LL222;if(*((int*)_tmp3C8)!= 4)goto
_LL222;_tmp3C9=((struct Cyc_Absyn_PointerType_struct*)_tmp3C8)->f1;_tmp3CA=
_tmp3C9.ptr_atts;_tmp3CB=_tmp3CA.nullable;_tmp3CC=_tmp3CA.bounds;_tmp3CD=_tmp3CA.zero_term;
_LL221: _tmp3C7=_tmp3CD;if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3CB))_tmp3C6=Cyc_Absyn_star_typ;if(((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp3CD) && !(*is_calloc)){{const char*_tmp7AE;void*
_tmp7AD;(_tmp7AD=0,Cyc_Tcutil_warn(loc,((_tmp7AE="converting malloc to calloc to ensure zero-termination",
_tag_dyneither(_tmp7AE,sizeof(char),55))),_tag_dyneither(_tmp7AD,sizeof(void*),0)));}*
is_calloc=1;}{void*_tmp3D0=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp3CC);
struct Cyc_Absyn_Exp*_tmp3D1;_LL225: if((int)_tmp3D0 != 0)goto _LL227;_LL226: goto
_LL224;_LL227: if(_tmp3D0 <= (void*)1)goto _LL229;if(*((int*)_tmp3D0)!= 0)goto
_LL229;_tmp3D1=((struct Cyc_Absyn_Upper_b_struct*)_tmp3D0)->f1;if(!(!one_elt))
goto _LL229;_LL228: {int _tmp3D2=Cyc_Evexp_c_can_eval(num_elts);if(_tmp3D2  && Cyc_Evexp_same_const_exp(
_tmp3D1,num_elts)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_conref_val(_tmp3CC),_tmp3C7);}{void*_tmp3D3=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(num_elts->topt))->v);void*_tmp3D4;_LL22C:
if(_tmp3D3 <= (void*)4)goto _LL22E;if(*((int*)_tmp3D3)!= 18)goto _LL22E;_tmp3D4=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp3D3)->f1;_LL22D: {struct Cyc_Absyn_Exp*
_tmp3D5=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp3D4,0),0,(
void*)1,0);if(Cyc_Evexp_same_const_exp(_tmp3D5,_tmp3D1)){*is_fat=0;return Cyc_Absyn_atb_typ(
elt_type,rgn,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_conref_val(_tmp3CC),_tmp3C7);}
goto _LL22B;}_LL22E:;_LL22F: goto _LL22B;_LL22B:;}goto _LL224;}_LL229:;_LL22A: goto
_LL224;_LL224:;}goto _LL21F;_LL222:;_LL223: goto _LL21F;_LL21F:;}if(!one_elt)
_tmp3C6=Cyc_Absyn_dyneither_typ;return _tmp3C6(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),_tmp3C7);}}}static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcSwap(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((
void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp3D6=Cyc_Tcutil_compress(t1);_LL231: if(
_tmp3D6 <= (void*)4)goto _LL233;if(*((int*)_tmp3D6)!= 7)goto _LL233;_LL232:{const
char*_tmp7B1;void*_tmp7B0;(_tmp7B0=0,Cyc_Tcutil_terr(loc,((_tmp7B1="cannot assign to an array",
_tag_dyneither(_tmp7B1,sizeof(char),26))),_tag_dyneither(_tmp7B0,sizeof(void*),0)));}
goto _LL230;_LL233:;_LL234: goto _LL230;_LL230:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(
t1,& ign_1)){const char*_tmp7B4;void*_tmp7B3;(_tmp7B3=0,Cyc_Tcutil_terr(loc,((
_tmp7B4="Swap not allowed for non-pointer or non-word-sized types.",
_tag_dyneither(_tmp7B4,sizeof(char),58))),_tag_dyneither(_tmp7B3,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp7B7;void*_tmp7B6;return(_tmp7B6=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp7B7="swap non-lvalue",_tag_dyneither(_tmp7B7,sizeof(char),
16))),_tag_dyneither(_tmp7B6,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp7BA;void*_tmp7B9;return(_tmp7B9=0,Cyc_Tcexp_expr_err(te,e2->loc,
topt,((_tmp7BA="swap non-lvalue",_tag_dyneither(_tmp7BA,sizeof(char),16))),
_tag_dyneither(_tmp7B9,sizeof(void*),0)));}{void*t_ign1=(void*)0;void*t_ign2=(
void*)0;int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp7BD;void*_tmp7BC;return(_tmp7BC=0,Cyc_Tcexp_expr_err(te,e1->loc,
topt,((_tmp7BD="swap value in zeroterm array",_tag_dyneither(_tmp7BD,sizeof(char),
29))),_tag_dyneither(_tmp7BC,sizeof(void*),0)));}if(Cyc_Tcutil_is_zero_ptr_deref(
e2,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp7C0;void*_tmp7BF;return(_tmp7BF=0,Cyc_Tcexp_expr_err(
te,e2->loc,topt,((_tmp7C0="swap value in zeroterm array",_tag_dyneither(_tmp7C0,
sizeof(char),29))),_tag_dyneither(_tmp7BF,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(t1,t2)){const char*
_tmp7C5;void*_tmp7C4[2];struct Cyc_String_pa_struct _tmp7C3;struct Cyc_String_pa_struct
_tmp7C2;void*_tmp3E3=(_tmp7C2.tag=0,((_tmp7C2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp7C3.tag=0,((_tmp7C3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7C4[0]=&
_tmp7C3,((_tmp7C4[1]=& _tmp7C2,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7C5="type mismatch: %s != %s",
_tag_dyneither(_tmp7C5,sizeof(char),24))),_tag_dyneither(_tmp7C4,sizeof(void*),2)))))))))))));
return _tmp3E3;}return(void*)0;}}}}int Cyc_Tcexp_in_stmt_exp=0;static void*Cyc_Tcexp_tcStmtExp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Stmt*
s);static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){struct _RegionHandle*_tmp3E8=Cyc_Tcenv_get_fnrgn(
te);{int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
_tmp3E8,te,s),s,1);Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp3E9=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp3EA;struct Cyc_Absyn_Stmt*_tmp3EB;struct Cyc_Absyn_Stmt*_tmp3EC;struct Cyc_Absyn_Decl*
_tmp3ED;struct Cyc_Absyn_Stmt*_tmp3EE;_LL236: if(_tmp3E9 <= (void*)1)goto _LL23C;if(*((
int*)_tmp3E9)!= 0)goto _LL238;_tmp3EA=((struct Cyc_Absyn_Exp_s_struct*)_tmp3E9)->f1;
_LL237: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3EA->topt))->v;_LL238:
if(*((int*)_tmp3E9)!= 1)goto _LL23A;_tmp3EB=((struct Cyc_Absyn_Seq_s_struct*)
_tmp3E9)->f1;_tmp3EC=((struct Cyc_Absyn_Seq_s_struct*)_tmp3E9)->f2;_LL239: s=
_tmp3EC;continue;_LL23A: if(*((int*)_tmp3E9)!= 11)goto _LL23C;_tmp3ED=((struct Cyc_Absyn_Decl_s_struct*)
_tmp3E9)->f1;_tmp3EE=((struct Cyc_Absyn_Decl_s_struct*)_tmp3E9)->f2;_LL23B: s=
_tmp3EE;continue;_LL23C:;_LL23D: {const char*_tmp7C8;void*_tmp7C7;return(_tmp7C7=
0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7C8="statement expression must end with expression",
_tag_dyneither(_tmp7C8,sizeof(char),46))),_tag_dyneither(_tmp7C7,sizeof(void*),0)));}
_LL235:;}}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1);static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1){void*rgn=(void*)2;if(rgn_handle != 0){struct Cyc_Absyn_RgnHandleType_struct
_tmp7CB;struct Cyc_Absyn_RgnHandleType_struct*_tmp7CA;void*expected_type=(void*)((
_tmp7CA=_cycalloc(sizeof(*_tmp7CA)),((_tmp7CA[0]=((_tmp7CB.tag=14,((_tmp7CB.f1=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(
te)),_tmp7CB)))),_tmp7CA))));void*handle_type=Cyc_Tcexp_tcExp(te,(void**)&
expected_type,(struct Cyc_Absyn_Exp*)rgn_handle);void*_tmp3F1=Cyc_Tcutil_compress(
handle_type);void*_tmp3F2;_LL23F: if(_tmp3F1 <= (void*)4)goto _LL241;if(*((int*)
_tmp3F1)!= 14)goto _LL241;_tmp3F2=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp3F1)->f1;_LL240: rgn=_tmp3F2;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto
_LL23E;_LL241:;_LL242:{const char*_tmp7CF;void*_tmp7CE[1];struct Cyc_String_pa_struct
_tmp7CD;(_tmp7CD.tag=0,((_tmp7CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp7CE[0]=& _tmp7CD,Cyc_Tcutil_terr(
rgn_handle->loc,((_tmp7CF="expecting region_t type but found %s",_tag_dyneither(
_tmp7CF,sizeof(char),37))),_tag_dyneither(_tmp7CE,sizeof(void*),1)))))));}goto
_LL23E;_LL23E:;}{void*_tmp3F8=(void*)e1->r;struct Cyc_Core_Opt*_tmp3F9;struct Cyc_List_List*
_tmp3FA;struct Cyc_List_List*_tmp3FB;union Cyc_Absyn_Cnst_union _tmp3FC;struct
_dyneither_ptr _tmp3FD;_LL244: if(*((int*)_tmp3F8)!= 29)goto _LL246;_LL245: {void*
_tmp3FE=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);void*_tmp3FF=Cyc_Tcutil_compress(
_tmp3FE);struct Cyc_Absyn_ArrayInfo _tmp400;void*_tmp401;struct Cyc_Absyn_Tqual
_tmp402;struct Cyc_Absyn_Exp*_tmp403;struct Cyc_Absyn_Conref*_tmp404;_LL24F: if(
_tmp3FF <= (void*)4)goto _LL251;if(*((int*)_tmp3FF)!= 7)goto _LL251;_tmp400=((
struct Cyc_Absyn_ArrayType_struct*)_tmp3FF)->f1;_tmp401=(void*)_tmp400.elt_type;
_tmp402=_tmp400.tq;_tmp403=_tmp400.num_elts;_tmp404=_tmp400.zero_term;_LL250: {
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp403);void*b;{void*
_tmp405=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);
void*_tmp406;_LL254: if(_tmp405 <= (void*)4)goto _LL256;if(*((int*)_tmp405)!= 18)
goto _LL256;_tmp406=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp405)->f1;_LL255:{
struct Cyc_Absyn_Upper_b_struct _tmp7D2;struct Cyc_Absyn_Upper_b_struct*_tmp7D1;b=(
void*)((_tmp7D1=_cycalloc(sizeof(*_tmp7D1)),((_tmp7D1[0]=((_tmp7D2.tag=0,((
_tmp7D2.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp406,0),
0,(void*)1,0),_tmp7D2)))),_tmp7D1))));}goto _LL253;_LL256:;_LL257: if(Cyc_Tcutil_is_const_exp(
te,bnd)){struct Cyc_Absyn_Upper_b_struct _tmp7D5;struct Cyc_Absyn_Upper_b_struct*
_tmp7D4;b=(void*)((_tmp7D4=_cycalloc(sizeof(*_tmp7D4)),((_tmp7D4[0]=((_tmp7D5.tag=
0,((_tmp7D5.f1=bnd,_tmp7D5)))),_tmp7D4))));}else{b=(void*)0;}_LL253:;}{struct Cyc_Absyn_PointerType_struct
_tmp7DF;struct Cyc_Absyn_PtrAtts _tmp7DE;struct Cyc_Absyn_PtrInfo _tmp7DD;struct Cyc_Absyn_PointerType_struct*
_tmp7DC;void*res_typ=(void*)((_tmp7DC=_cycalloc(sizeof(*_tmp7DC)),((_tmp7DC[0]=((
_tmp7DF.tag=4,((_tmp7DF.f1=((_tmp7DD.elt_typ=(void*)_tmp401,((_tmp7DD.elt_tq=
_tmp402,((_tmp7DD.ptr_atts=((_tmp7DE.rgn=(void*)rgn,((_tmp7DE.nullable=((struct
Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((_tmp7DE.bounds=Cyc_Absyn_new_conref(
b),((_tmp7DE.zero_term=_tmp404,((_tmp7DE.ptrloc=0,_tmp7DE)))))))))),_tmp7DD)))))),
_tmp7DF)))),_tmp7DC))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp7E0;e->topt=((_tmp7E0=_cycalloc(
sizeof(*_tmp7E0)),((_tmp7E0->v=(void*)res_typ,_tmp7E0))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}return res_typ;}}_LL251:;_LL252: {const char*
_tmp7E3;void*_tmp7E2;(_tmp7E2=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7E3="tcNew: comprehension returned non-array type",
_tag_dyneither(_tmp7E3,sizeof(char),45))),_tag_dyneither(_tmp7E2,sizeof(void*),0)));}
_LL24E:;}_LL246: if(*((int*)_tmp3F8)!= 37)goto _LL248;_tmp3F9=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp3F8)->f1;_tmp3FA=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp3F8)->f2;
_LL247:{struct Cyc_Absyn_Array_e_struct _tmp7E6;struct Cyc_Absyn_Array_e_struct*
_tmp7E5;(void*)(e1->r=(void*)((void*)((_tmp7E5=_cycalloc(sizeof(*_tmp7E5)),((
_tmp7E5[0]=((_tmp7E6.tag=28,((_tmp7E6.f1=_tmp3FA,_tmp7E6)))),_tmp7E5))))));}
_tmp3FB=_tmp3FA;goto _LL249;_LL248: if(*((int*)_tmp3F8)!= 28)goto _LL24A;_tmp3FB=((
struct Cyc_Absyn_Array_e_struct*)_tmp3F8)->f1;_LL249: {void**elt_typ_opt=0;int
zero_term=0;if(topt != 0){void*_tmp414=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp415;void*_tmp416;void**_tmp417;struct Cyc_Absyn_Tqual _tmp418;struct Cyc_Absyn_PtrAtts
_tmp419;struct Cyc_Absyn_Conref*_tmp41A;_LL259: if(_tmp414 <= (void*)4)goto _LL25B;
if(*((int*)_tmp414)!= 4)goto _LL25B;_tmp415=((struct Cyc_Absyn_PointerType_struct*)
_tmp414)->f1;_tmp416=(void*)_tmp415.elt_typ;_tmp417=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp414)->f1).elt_typ;_tmp418=_tmp415.elt_tq;_tmp419=_tmp415.ptr_atts;_tmp41A=
_tmp419.zero_term;_LL25A: elt_typ_opt=(void**)_tmp417;zero_term=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp41A);goto _LL258;_LL25B:;
_LL25C: goto _LL258;_LL258:;}{void*_tmp41B=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp3FB);{struct Cyc_Core_Opt*_tmp7E7;e1->topt=((_tmp7E7=
_cycalloc(sizeof(*_tmp7E7)),((_tmp7E7->v=(void*)_tmp41B,_tmp7E7))));}{void*
res_typ;{void*_tmp41D=Cyc_Tcutil_compress(_tmp41B);struct Cyc_Absyn_ArrayInfo
_tmp41E;void*_tmp41F;struct Cyc_Absyn_Tqual _tmp420;struct Cyc_Absyn_Exp*_tmp421;
struct Cyc_Absyn_Conref*_tmp422;_LL25E: if(_tmp41D <= (void*)4)goto _LL260;if(*((int*)
_tmp41D)!= 7)goto _LL260;_tmp41E=((struct Cyc_Absyn_ArrayType_struct*)_tmp41D)->f1;
_tmp41F=(void*)_tmp41E.elt_type;_tmp420=_tmp41E.tq;_tmp421=_tmp41E.num_elts;
_tmp422=_tmp41E.zero_term;_LL25F:{struct Cyc_Absyn_PointerType_struct _tmp7FC;
struct Cyc_Absyn_PtrAtts _tmp7FB;struct Cyc_Absyn_Upper_b_struct _tmp7FA;struct Cyc_Absyn_Upper_b_struct*
_tmp7F9;struct Cyc_Absyn_PtrInfo _tmp7F8;struct Cyc_Absyn_PointerType_struct*
_tmp7F7;res_typ=(void*)((_tmp7F7=_cycalloc(sizeof(*_tmp7F7)),((_tmp7F7[0]=((
_tmp7FC.tag=4,((_tmp7FC.f1=((_tmp7F8.elt_typ=(void*)_tmp41F,((_tmp7F8.elt_tq=
_tmp420,((_tmp7F8.ptr_atts=((_tmp7FB.rgn=(void*)rgn,((_tmp7FB.nullable=((struct
Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((_tmp7FB.bounds=Cyc_Absyn_new_conref((
void*)((_tmp7F9=_cycalloc(sizeof(*_tmp7F9)),((_tmp7F9[0]=((_tmp7FA.tag=0,((
_tmp7FA.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp421),_tmp7FA)))),_tmp7F9))))),((
_tmp7FB.zero_term=_tmp422,((_tmp7FB.ptrloc=0,_tmp7FB)))))))))),_tmp7F8)))))),
_tmp7FC)))),_tmp7F7))));}if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp7FD;e->topt=((_tmp7FD=_cycalloc(
sizeof(*_tmp7FD)),((_tmp7FD->v=(void*)res_typ,_tmp7FD))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}goto _LL25D;_LL260:;_LL261: {const char*
_tmp800;void*_tmp7FF;(_tmp7FF=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp800="tcExpNoPromote on Array_e returned non-array type",
_tag_dyneither(_tmp800,sizeof(char),50))),_tag_dyneither(_tmp7FF,sizeof(void*),0)));}
_LL25D:;}return res_typ;}}}_LL24A: if(*((int*)_tmp3F8)!= 0)goto _LL24C;_tmp3FC=((
struct Cyc_Absyn_Const_e_struct*)_tmp3F8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp3F8)->f1).String_c).tag != 5)goto _LL24C;_tmp3FD=(_tmp3FC.String_c).f1;_LL24B: {
void*_tmp42C=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(
void*)0,Cyc_Absyn_true_conref);void*_tmp42D=Cyc_Tcexp_tcExp(te,(void**)& _tmp42C,
e1);struct Cyc_Absyn_Upper_b_struct _tmp803;struct Cyc_Absyn_Upper_b_struct*_tmp802;
return Cyc_Absyn_atb_typ(_tmp42D,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp802=
_cycalloc(sizeof(*_tmp802)),((_tmp802[0]=((_tmp803.tag=0,((_tmp803.f1=Cyc_Absyn_uint_exp(
1,0),_tmp803)))),_tmp802)))),Cyc_Absyn_false_conref);}_LL24C:;_LL24D: {void**
topt2=0;if(topt != 0){void*_tmp430=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp431;void*_tmp432;void**_tmp433;struct Cyc_Absyn_Tqual _tmp434;_LL263: if(
_tmp430 <= (void*)4)goto _LL267;if(*((int*)_tmp430)!= 4)goto _LL265;_tmp431=((
struct Cyc_Absyn_PointerType_struct*)_tmp430)->f1;_tmp432=(void*)_tmp431.elt_typ;
_tmp433=(void**)&(((struct Cyc_Absyn_PointerType_struct*)_tmp430)->f1).elt_typ;
_tmp434=_tmp431.elt_tq;_LL264: topt2=(void**)_tmp433;goto _LL262;_LL265: if(*((int*)
_tmp430)!= 2)goto _LL267;_LL266:{void**_tmp804;topt2=((_tmp804=_cycalloc(sizeof(*
_tmp804)),((_tmp804[0]=*topt,_tmp804))));}goto _LL262;_LL267:;_LL268: goto _LL262;
_LL262:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);struct Cyc_Absyn_PointerType_struct
_tmp80E;struct Cyc_Absyn_PtrAtts _tmp80D;struct Cyc_Absyn_PtrInfo _tmp80C;struct Cyc_Absyn_PointerType_struct*
_tmp80B;void*res_typ=(void*)((_tmp80B=_cycalloc(sizeof(*_tmp80B)),((_tmp80B[0]=((
_tmp80E.tag=4,((_tmp80E.f1=((_tmp80C.elt_typ=(void*)telt,((_tmp80C.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp80C.ptr_atts=((_tmp80D.rgn=(void*)rgn,((_tmp80D.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(),((_tmp80D.bounds=Cyc_Absyn_bounds_one_conref,((_tmp80D.zero_term=
Cyc_Absyn_false_conref,((_tmp80D.ptrloc=0,_tmp80D)))))))))),_tmp80C)))))),
_tmp80E)))),_tmp80B))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp80F;e->topt=((_tmp80F=_cycalloc(
sizeof(*_tmp80F)),((_tmp80F->v=(void*)res_typ,_tmp80F))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}return res_typ;}}_LL243:;}}void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(
Cyc_Tcexp_tcExpNoPromote(te,topt,e));void*_tmp43B=t;struct Cyc_Absyn_ArrayInfo
_tmp43C;void*_tmp43D;struct Cyc_Absyn_Tqual _tmp43E;struct Cyc_Absyn_Exp*_tmp43F;
struct Cyc_Absyn_Conref*_tmp440;_LL26A: if(_tmp43B <= (void*)4)goto _LL26C;if(*((int*)
_tmp43B)!= 7)goto _LL26C;_tmp43C=((struct Cyc_Absyn_ArrayType_struct*)_tmp43B)->f1;
_tmp43D=(void*)_tmp43C.elt_type;_tmp43E=_tmp43C.tq;_tmp43F=_tmp43C.num_elts;
_tmp440=_tmp43C.zero_term;_LL26B: {void*_tmp442;struct _tuple6 _tmp441=Cyc_Tcutil_addressof_props(
te,e);_tmp442=_tmp441.f2;{struct Cyc_Absyn_Upper_b_struct _tmp812;struct Cyc_Absyn_Upper_b_struct*
_tmp811;void*_tmp443=_tmp43F == 0?(void*)((void*)0):(void*)((_tmp811=_cycalloc(
sizeof(*_tmp811)),((_tmp811[0]=((_tmp812.tag=0,((_tmp812.f1=(struct Cyc_Absyn_Exp*)
_tmp43F,_tmp812)))),_tmp811))));t=Cyc_Absyn_atb_typ(_tmp43D,_tmp442,_tmp43E,
_tmp443,_tmp440);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);
return t;}}_LL26C:;_LL26D: return t;_LL269:;}void*Cyc_Tcexp_tcExpInitializer(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(
te,topt,e);struct _RegionHandle*_tmp446=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp446,t) && !Cyc_Tcutil_is_noalias_path(_tmp446,e)){{const char*_tmp81A;const
char*_tmp819;void*_tmp818[3];struct Cyc_String_pa_struct _tmp817;struct Cyc_String_pa_struct
_tmp816;struct Cyc_String_pa_struct _tmp815;(_tmp815.tag=0,((_tmp815.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp816.tag=0,((
_tmp816.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp817.tag=0,((_tmp817.f1=(
struct _dyneither_ptr)(topt == 0?(_tmp81A="NULL",_tag_dyneither(_tmp81A,sizeof(
char),5)):(struct _dyneither_ptr)Cyc_Absynpp_typ2string(*topt)),((_tmp818[0]=&
_tmp817,((_tmp818[1]=& _tmp816,((_tmp818[2]=& _tmp815,Cyc_fprintf(Cyc_stderr,((
_tmp819="topt=%s, e->topt->v=%s, e=%s\n",_tag_dyneither(_tmp819,sizeof(char),30))),
_tag_dyneither(_tmp818,sizeof(void*),3)))))))))))))))))));}{const char*_tmp81D;
void*_tmp81C;(_tmp81C=0,Cyc_Tcutil_terr(e->loc,((_tmp81D="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp81D,sizeof(char),49))),_tag_dyneither(_tmp81C,sizeof(void*),0)));}}{
void*_tmp44F=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp450;_LL26F: if(*((int*)
_tmp44F)!= 28)goto _LL271;_LL270: goto _LL272;_LL271: if(*((int*)_tmp44F)!= 29)goto
_LL273;_LL272: goto _LL274;_LL273: if(*((int*)_tmp44F)!= 0)goto _LL275;_tmp450=((
struct Cyc_Absyn_Const_e_struct*)_tmp44F)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp44F)->f1).String_c).tag != 5)goto _LL275;_LL274: return t;_LL275:;_LL276: t=Cyc_Tcutil_compress(
t);{void*_tmp451=t;struct Cyc_Absyn_ArrayInfo _tmp452;void*_tmp453;struct Cyc_Absyn_Tqual
_tmp454;struct Cyc_Absyn_Exp*_tmp455;struct Cyc_Absyn_Conref*_tmp456;_LL278: if(
_tmp451 <= (void*)4)goto _LL27A;if(*((int*)_tmp451)!= 7)goto _LL27A;_tmp452=((
struct Cyc_Absyn_ArrayType_struct*)_tmp451)->f1;_tmp453=(void*)_tmp452.elt_type;
_tmp454=_tmp452.tq;_tmp455=_tmp452.num_elts;_tmp456=_tmp452.zero_term;_LL279: {
void*_tmp458;struct _tuple6 _tmp457=Cyc_Tcutil_addressof_props(te,e);_tmp458=
_tmp457.f2;{struct Cyc_Absyn_Upper_b_struct _tmp820;struct Cyc_Absyn_Upper_b_struct*
_tmp81F;void*b=_tmp455 == 0?(void*)((void*)0):(void*)((_tmp81F=_cycalloc(sizeof(*
_tmp81F)),((_tmp81F[0]=((_tmp820.tag=0,((_tmp820.f1=(struct Cyc_Absyn_Exp*)
_tmp455,_tmp820)))),_tmp81F))));t=Cyc_Absyn_atb_typ(_tmp453,_tmp458,_tmp454,b,
_tmp456);Cyc_Tcutil_unchecked_cast(te,e,t,(void*)3);return t;}}_LL27A:;_LL27B:
return t;_LL277:;}_LL26E:;}}static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp45B=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp45C;_LL27D: if(*((int*)_tmp45B)!= 13)goto _LL27F;_tmp45C=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp45B)->f1;_LL27E: Cyc_Tcexp_tcExpNoInst(
te,topt,_tmp45C);(void*)(((struct Cyc_Core_Opt*)_check_null(_tmp45C->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(_tmp45C->topt))->v,
0));e->topt=_tmp45C->topt;goto _LL27C;_LL27F:;_LL280: Cyc_Tcexp_tcExpNoInst(te,
topt,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0));{void*_tmp45D=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp45E;void*_tmp45F;struct Cyc_Absyn_Tqual _tmp460;struct Cyc_Absyn_PtrAtts _tmp461;
void*_tmp462;struct Cyc_Absyn_Conref*_tmp463;struct Cyc_Absyn_Conref*_tmp464;
struct Cyc_Absyn_Conref*_tmp465;_LL282: if(_tmp45D <= (void*)4)goto _LL284;if(*((int*)
_tmp45D)!= 4)goto _LL284;_tmp45E=((struct Cyc_Absyn_PointerType_struct*)_tmp45D)->f1;
_tmp45F=(void*)_tmp45E.elt_typ;_tmp460=_tmp45E.elt_tq;_tmp461=_tmp45E.ptr_atts;
_tmp462=(void*)_tmp461.rgn;_tmp463=_tmp461.nullable;_tmp464=_tmp461.bounds;
_tmp465=_tmp461.zero_term;_LL283:{void*_tmp466=Cyc_Tcutil_compress(_tmp45F);
struct Cyc_Absyn_FnInfo _tmp467;struct Cyc_List_List*_tmp468;struct Cyc_Core_Opt*
_tmp469;void*_tmp46A;struct Cyc_List_List*_tmp46B;int _tmp46C;struct Cyc_Absyn_VarargInfo*
_tmp46D;struct Cyc_List_List*_tmp46E;struct Cyc_List_List*_tmp46F;_LL287: if(
_tmp466 <= (void*)4)goto _LL289;if(*((int*)_tmp466)!= 8)goto _LL289;_tmp467=((
struct Cyc_Absyn_FnType_struct*)_tmp466)->f1;_tmp468=_tmp467.tvars;_tmp469=
_tmp467.effect;_tmp46A=(void*)_tmp467.ret_typ;_tmp46B=_tmp467.args;_tmp46C=
_tmp467.c_varargs;_tmp46D=_tmp467.cyc_varargs;_tmp46E=_tmp467.rgn_po;_tmp46F=
_tmp467.attributes;_LL288: if(_tmp468 != 0){struct _RegionHandle*_tmp470=Cyc_Tcenv_get_fnrgn(
te);struct _tuple4 _tmp821;struct _tuple4 _tmp471=(_tmp821.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp821.f2=_tmp470,_tmp821)));struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp470,Cyc_Tcutil_r_make_inst_var,&
_tmp471,_tmp468);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);struct Cyc_Absyn_FnType_struct _tmp827;struct Cyc_Absyn_FnInfo
_tmp826;struct Cyc_Absyn_FnType_struct*_tmp825;void*ftyp=Cyc_Tcutil_rsubstitute(
_tmp470,inst,(void*)((_tmp825=_cycalloc(sizeof(*_tmp825)),((_tmp825[0]=((_tmp827.tag=
8,((_tmp827.f1=((_tmp826.tvars=0,((_tmp826.effect=_tmp469,((_tmp826.ret_typ=(
void*)_tmp46A,((_tmp826.args=_tmp46B,((_tmp826.c_varargs=_tmp46C,((_tmp826.cyc_varargs=
_tmp46D,((_tmp826.rgn_po=_tmp46E,((_tmp826.attributes=_tmp46F,_tmp826)))))))))))))))),
_tmp827)))),_tmp825)))));struct Cyc_Absyn_PointerType_struct _tmp831;struct Cyc_Absyn_PtrAtts
_tmp830;struct Cyc_Absyn_PtrInfo _tmp82F;struct Cyc_Absyn_PointerType_struct*
_tmp82E;struct Cyc_Absyn_PointerType_struct*_tmp472=(_tmp82E=_cycalloc(sizeof(*
_tmp82E)),((_tmp82E[0]=((_tmp831.tag=4,((_tmp831.f1=((_tmp82F.elt_typ=(void*)
ftyp,((_tmp82F.elt_tq=_tmp460,((_tmp82F.ptr_atts=((_tmp830.rgn=(void*)_tmp462,((
_tmp830.nullable=_tmp463,((_tmp830.bounds=_tmp464,((_tmp830.zero_term=_tmp465,((
_tmp830.ptrloc=0,_tmp830)))))))))),_tmp82F)))))),_tmp831)))),_tmp82E)));struct
Cyc_Absyn_Exp*_tmp473=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Instantiate_e_struct
_tmp834;struct Cyc_Absyn_Instantiate_e_struct*_tmp833;(void*)(e->r=(void*)((void*)((
_tmp833=_cycalloc(sizeof(*_tmp833)),((_tmp833[0]=((_tmp834.tag=14,((_tmp834.f1=
_tmp473,((_tmp834.f2=ts,_tmp834)))))),_tmp833))))));}{struct Cyc_Core_Opt*_tmp835;
e->topt=((_tmp835=_cycalloc(sizeof(*_tmp835)),((_tmp835->v=(void*)((void*)
_tmp472),_tmp835))));}}goto _LL286;_LL289:;_LL28A: goto _LL286;_LL286:;}goto _LL281;
_LL284:;_LL285: goto _LL281;_LL281:;}goto _LL27C;_LL27C:;}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*loc=e->loc;void*
t;{void*_tmp47F=(void*)e->r;struct Cyc_Absyn_Exp*_tmp480;struct _tuple1*_tmp481;
struct Cyc_Absyn_Exp*_tmp482;struct Cyc_List_List*_tmp483;struct Cyc_Core_Opt*
_tmp484;struct Cyc_List_List*_tmp485;union Cyc_Absyn_Cnst_union _tmp486;union Cyc_Absyn_Cnst_union*
_tmp487;struct _tuple1*_tmp488;void*_tmp489;void*_tmp48A;struct Cyc_List_List*
_tmp48B;struct Cyc_Absyn_Exp*_tmp48C;void*_tmp48D;struct Cyc_Absyn_Exp*_tmp48E;
struct Cyc_Core_Opt*_tmp48F;struct Cyc_Absyn_Exp*_tmp490;struct Cyc_Absyn_Exp*
_tmp491;struct Cyc_Absyn_Exp*_tmp492;struct Cyc_Absyn_Exp*_tmp493;struct Cyc_Absyn_Exp*
_tmp494;struct Cyc_Absyn_Exp*_tmp495;struct Cyc_Absyn_Exp*_tmp496;struct Cyc_Absyn_Exp*
_tmp497;struct Cyc_Absyn_Exp*_tmp498;struct Cyc_Absyn_Exp*_tmp499;struct Cyc_Absyn_Exp*
_tmp49A;struct Cyc_List_List*_tmp49B;struct Cyc_Absyn_VarargCallInfo*_tmp49C;
struct Cyc_Absyn_VarargCallInfo**_tmp49D;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_Absyn_Exp*
_tmp49F;struct Cyc_List_List*_tmp4A0;void*_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A2;
void*_tmp4A3;void**_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A5;struct Cyc_Absyn_Exp*
_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A8;void*_tmp4A9;
void*_tmp4AA;void*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_Absyn_Exp*
_tmp4AD;struct _dyneither_ptr*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AF;struct
_dyneither_ptr*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B2;
struct Cyc_List_List*_tmp4B3;struct _tuple2*_tmp4B4;struct Cyc_List_List*_tmp4B5;
struct Cyc_List_List*_tmp4B6;struct Cyc_Absyn_Stmt*_tmp4B7;struct Cyc_Absyn_Vardecl*
_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BA;int _tmp4BB;int*
_tmp4BC;struct _tuple1*_tmp4BD;struct _tuple1**_tmp4BE;struct Cyc_List_List*_tmp4BF;
struct Cyc_List_List**_tmp4C0;struct Cyc_List_List*_tmp4C1;struct Cyc_Absyn_Aggrdecl*
_tmp4C2;struct Cyc_Absyn_Aggrdecl**_tmp4C3;void*_tmp4C4;struct Cyc_List_List*
_tmp4C5;struct Cyc_List_List*_tmp4C6;struct Cyc_Absyn_Tuniondecl*_tmp4C7;struct Cyc_Absyn_Tunionfield*
_tmp4C8;struct _tuple1*_tmp4C9;struct _tuple1**_tmp4CA;struct Cyc_Absyn_Enumdecl*
_tmp4CB;struct Cyc_Absyn_Enumfield*_tmp4CC;struct _tuple1*_tmp4CD;struct _tuple1**
_tmp4CE;void*_tmp4CF;struct Cyc_Absyn_Enumfield*_tmp4D0;struct Cyc_Absyn_MallocInfo
_tmp4D1;int _tmp4D2;int*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D4;void**_tmp4D5;void***
_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D7;struct Cyc_Absyn_Exp**_tmp4D8;int _tmp4D9;int*
_tmp4DA;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DC;void*_tmp4DD;
_LL28C: if(*((int*)_tmp47F)!= 13)goto _LL28E;_tmp480=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp47F)->f1;_LL28D: Cyc_Tcexp_tcExpNoInst(te,0,_tmp480);return;_LL28E: if(*((int*)
_tmp47F)!= 2)goto _LL290;_tmp481=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp47F)->f1;
_LL28F: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp481);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL290: if(*((int*)_tmp47F)!= 10)goto _LL292;_tmp482=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp47F)->f1;_tmp483=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp47F)->f2;_LL291:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp482,_tmp483);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL292: if(*((int*)_tmp47F)!= 37)goto _LL294;_tmp484=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp47F)->f1;_tmp485=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp47F)->f2;
_LL293: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp485);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL294: if(*((int*)_tmp47F)!= 0)goto _LL296;_tmp486=((struct Cyc_Absyn_Const_e_struct*)
_tmp47F)->f1;_tmp487=(union Cyc_Absyn_Cnst_union*)&((struct Cyc_Absyn_Const_e_struct*)
_tmp47F)->f1;_LL295: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst_union*)
_tmp487,e);goto _LL28B;_LL296: if(*((int*)_tmp47F)!= 1)goto _LL298;_tmp488=((struct
Cyc_Absyn_Var_e_struct*)_tmp47F)->f1;_tmp489=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp47F)->f2;_LL297: t=Cyc_Tcexp_tcVar(te,loc,_tmp488,_tmp489);goto _LL28B;_LL298:
if(*((int*)_tmp47F)!= 3)goto _LL29A;_tmp48A=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp47F)->f1;_tmp48B=((struct Cyc_Absyn_Primop_e_struct*)_tmp47F)->f2;_LL299: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp48A,_tmp48B);goto _LL28B;_LL29A: if(*((int*)
_tmp47F)!= 5)goto _LL29C;_tmp48C=((struct Cyc_Absyn_Increment_e_struct*)_tmp47F)->f1;
_tmp48D=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp47F)->f2;_LL29B: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp48C,_tmp48D);goto _LL28B;_LL29C: if(*((int*)_tmp47F)!= 4)goto
_LL29E;_tmp48E=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp47F)->f1;_tmp48F=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp47F)->f2;_tmp490=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp47F)->f3;_LL29D: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp48E,_tmp48F,_tmp490);
goto _LL28B;_LL29E: if(*((int*)_tmp47F)!= 6)goto _LL2A0;_tmp491=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp47F)->f1;_tmp492=((struct Cyc_Absyn_Conditional_e_struct*)_tmp47F)->f2;
_tmp493=((struct Cyc_Absyn_Conditional_e_struct*)_tmp47F)->f3;_LL29F: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp491,_tmp492,_tmp493);goto _LL28B;_LL2A0: if(*((int*)_tmp47F)!= 7)
goto _LL2A2;_tmp494=((struct Cyc_Absyn_And_e_struct*)_tmp47F)->f1;_tmp495=((struct
Cyc_Absyn_And_e_struct*)_tmp47F)->f2;_LL2A1: t=Cyc_Tcexp_tcAnd(te,loc,_tmp494,
_tmp495);goto _LL28B;_LL2A2: if(*((int*)_tmp47F)!= 8)goto _LL2A4;_tmp496=((struct
Cyc_Absyn_Or_e_struct*)_tmp47F)->f1;_tmp497=((struct Cyc_Absyn_Or_e_struct*)
_tmp47F)->f2;_LL2A3: t=Cyc_Tcexp_tcOr(te,loc,_tmp496,_tmp497);goto _LL28B;_LL2A4:
if(*((int*)_tmp47F)!= 9)goto _LL2A6;_tmp498=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp47F)->f1;_tmp499=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp47F)->f2;_LL2A5: t=
Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp498,_tmp499);goto _LL28B;_LL2A6: if(*((int*)
_tmp47F)!= 11)goto _LL2A8;_tmp49A=((struct Cyc_Absyn_FnCall_e_struct*)_tmp47F)->f1;
_tmp49B=((struct Cyc_Absyn_FnCall_e_struct*)_tmp47F)->f2;_tmp49C=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp47F)->f3;_tmp49D=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp47F)->f3;_LL2A7: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp49A,_tmp49B,_tmp49D);
goto _LL28B;_LL2A8: if(*((int*)_tmp47F)!= 12)goto _LL2AA;_tmp49E=((struct Cyc_Absyn_Throw_e_struct*)
_tmp47F)->f1;_LL2A9: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp49E);goto _LL28B;_LL2AA:
if(*((int*)_tmp47F)!= 14)goto _LL2AC;_tmp49F=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp47F)->f1;_tmp4A0=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp47F)->f2;_LL2AB:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp49F,_tmp4A0);goto _LL28B;_LL2AC: if(*((
int*)_tmp47F)!= 15)goto _LL2AE;_tmp4A1=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp47F)->f1;_tmp4A2=((struct Cyc_Absyn_Cast_e_struct*)_tmp47F)->f2;_tmp4A3=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp47F)->f4;_tmp4A4=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp47F)->f4);_LL2AD: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4A1,_tmp4A2,(void**)
_tmp4A4);goto _LL28B;_LL2AE: if(*((int*)_tmp47F)!= 16)goto _LL2B0;_tmp4A5=((struct
Cyc_Absyn_Address_e_struct*)_tmp47F)->f1;_LL2AF: t=Cyc_Tcexp_tcAddress(te,loc,e,
topt,_tmp4A5);goto _LL28B;_LL2B0: if(*((int*)_tmp47F)!= 17)goto _LL2B2;_tmp4A6=((
struct Cyc_Absyn_New_e_struct*)_tmp47F)->f1;_tmp4A7=((struct Cyc_Absyn_New_e_struct*)
_tmp47F)->f2;_LL2B1: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4A6,e,_tmp4A7);goto _LL28B;
_LL2B2: if(*((int*)_tmp47F)!= 19)goto _LL2B4;_tmp4A8=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp47F)->f1;_LL2B3: {void*_tmp4DE=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4A8);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4DE);goto _LL28B;}_LL2B4: if(*((int*)_tmp47F)!= 18)goto _LL2B6;
_tmp4A9=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp47F)->f1;_LL2B5: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4A9);goto _LL28B;_LL2B6: if(*((int*)_tmp47F)!= 20)goto _LL2B8;
_tmp4AA=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp47F)->f1;_tmp4AB=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp47F)->f2;_LL2B7: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp4AA,_tmp4AB);goto _LL28B;_LL2B8: if(*((int*)_tmp47F)!= 21)goto _LL2BA;
_LL2B9:{const char*_tmp838;void*_tmp837;(_tmp837=0,Cyc_Tcutil_terr(loc,((_tmp838="gen() not in top-level initializer",
_tag_dyneither(_tmp838,sizeof(char),35))),_tag_dyneither(_tmp837,sizeof(void*),0)));}
return;_LL2BA: if(*((int*)_tmp47F)!= 22)goto _LL2BC;_tmp4AC=((struct Cyc_Absyn_Deref_e_struct*)
_tmp47F)->f1;_LL2BB: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4AC);goto _LL28B;_LL2BC:
if(*((int*)_tmp47F)!= 23)goto _LL2BE;_tmp4AD=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp47F)->f1;_tmp4AE=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp47F)->f2;_LL2BD:
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp4AD,_tmp4AE);goto _LL28B;_LL2BE: if(*((
int*)_tmp47F)!= 24)goto _LL2C0;_tmp4AF=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp47F)->f1;_tmp4B0=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp47F)->f2;_LL2BF: t=
Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp4AF,_tmp4B0);goto _LL28B;_LL2C0: if(*((int*)
_tmp47F)!= 25)goto _LL2C2;_tmp4B1=((struct Cyc_Absyn_Subscript_e_struct*)_tmp47F)->f1;
_tmp4B2=((struct Cyc_Absyn_Subscript_e_struct*)_tmp47F)->f2;_LL2C1: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp4B1,_tmp4B2);goto _LL28B;_LL2C2: if(*((int*)_tmp47F)!= 26)goto
_LL2C4;_tmp4B3=((struct Cyc_Absyn_Tuple_e_struct*)_tmp47F)->f1;_LL2C3: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp4B3);goto _LL28B;_LL2C4: if(*((int*)_tmp47F)!= 27)goto _LL2C6;
_tmp4B4=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp47F)->f1;_tmp4B5=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp47F)->f2;_LL2C5: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4B4,_tmp4B5);goto
_LL28B;_LL2C6: if(*((int*)_tmp47F)!= 28)goto _LL2C8;_tmp4B6=((struct Cyc_Absyn_Array_e_struct*)
_tmp47F)->f1;_LL2C7: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp4E1=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp4E2;void*_tmp4E3;void**
_tmp4E4;struct Cyc_Absyn_Conref*_tmp4E5;_LL2DD: if(_tmp4E1 <= (void*)4)goto _LL2DF;
if(*((int*)_tmp4E1)!= 7)goto _LL2DF;_tmp4E2=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4E1)->f1;_tmp4E3=(void*)_tmp4E2.elt_type;_tmp4E4=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp4E1)->f1).elt_type;_tmp4E5=_tmp4E2.zero_term;_LL2DE: elt_topt=(void**)_tmp4E4;
zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp4E5);
goto _LL2DC;_LL2DF:;_LL2E0: goto _LL2DC;_LL2DC:;}t=Cyc_Tcexp_tcArray(te,loc,
elt_topt,zero_term,_tmp4B6);goto _LL28B;}_LL2C8: if(*((int*)_tmp47F)!= 38)goto
_LL2CA;_tmp4B7=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp47F)->f1;_LL2C9: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp4B7);goto _LL28B;_LL2CA: if(*((int*)_tmp47F)!= 29)goto _LL2CC;
_tmp4B8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp47F)->f1;_tmp4B9=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp47F)->f2;_tmp4BA=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp47F)->f3;_tmp4BB=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp47F)->f4;
_tmp4BC=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp47F)->f4;_LL2CB: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4B8,_tmp4B9,_tmp4BA,_tmp4BC);goto
_LL28B;_LL2CC: if(*((int*)_tmp47F)!= 30)goto _LL2CE;_tmp4BD=((struct Cyc_Absyn_Struct_e_struct*)
_tmp47F)->f1;_tmp4BE=(struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp47F)->f1;_tmp4BF=((struct Cyc_Absyn_Struct_e_struct*)_tmp47F)->f2;_tmp4C0=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp47F)->f2;_tmp4C1=((
struct Cyc_Absyn_Struct_e_struct*)_tmp47F)->f3;_tmp4C2=((struct Cyc_Absyn_Struct_e_struct*)
_tmp47F)->f4;_tmp4C3=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp47F)->f4;_LL2CD: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp4BE,_tmp4C0,_tmp4C1,
_tmp4C3);goto _LL28B;_LL2CE: if(*((int*)_tmp47F)!= 31)goto _LL2D0;_tmp4C4=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp47F)->f1;_tmp4C5=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp47F)->f2;_LL2CF: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4C4,_tmp4C5);goto _LL28B;
_LL2D0: if(*((int*)_tmp47F)!= 32)goto _LL2D2;_tmp4C6=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp47F)->f1;_tmp4C7=((struct Cyc_Absyn_Tunion_e_struct*)_tmp47F)->f2;_tmp4C8=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp47F)->f3;_LL2D1: t=Cyc_Tcexp_tcTunion(te,loc,
topt,e,_tmp4C6,_tmp4C7,_tmp4C8);goto _LL28B;_LL2D2: if(*((int*)_tmp47F)!= 33)goto
_LL2D4;_tmp4C9=((struct Cyc_Absyn_Enum_e_struct*)_tmp47F)->f1;_tmp4CA=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp47F)->f1;_tmp4CB=((struct Cyc_Absyn_Enum_e_struct*)
_tmp47F)->f2;_tmp4CC=((struct Cyc_Absyn_Enum_e_struct*)_tmp47F)->f3;_LL2D3:*
_tmp4CA=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp4CC))->name;{struct Cyc_Absyn_EnumType_struct
_tmp83B;struct Cyc_Absyn_EnumType_struct*_tmp83A;t=(void*)((_tmp83A=_cycalloc(
sizeof(*_tmp83A)),((_tmp83A[0]=((_tmp83B.tag=12,((_tmp83B.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp4CB))->name,((_tmp83B.f2=_tmp4CB,_tmp83B)))))),_tmp83A))));}goto
_LL28B;_LL2D4: if(*((int*)_tmp47F)!= 34)goto _LL2D6;_tmp4CD=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp47F)->f1;_tmp4CE=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp47F)->f1;_tmp4CF=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp47F)->f2;
_tmp4D0=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp47F)->f3;_LL2D5:*_tmp4CE=((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp4D0))->name;t=_tmp4CF;goto _LL28B;
_LL2D6: if(*((int*)_tmp47F)!= 35)goto _LL2D8;_tmp4D1=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp47F)->f1;_tmp4D2=_tmp4D1.is_calloc;_tmp4D3=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp47F)->f1).is_calloc;_tmp4D4=_tmp4D1.rgn;_tmp4D5=_tmp4D1.elt_type;_tmp4D6=(
void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp47F)->f1).elt_type;_tmp4D7=
_tmp4D1.num_elts;_tmp4D8=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp47F)->f1).num_elts;_tmp4D9=_tmp4D1.fat_result;_tmp4DA=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp47F)->f1).fat_result;_LL2D7: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4D4,_tmp4D6,
_tmp4D8,_tmp4D3,_tmp4DA);goto _LL28B;_LL2D8: if(*((int*)_tmp47F)!= 36)goto _LL2DA;
_tmp4DB=((struct Cyc_Absyn_Swap_e_struct*)_tmp47F)->f1;_tmp4DC=((struct Cyc_Absyn_Swap_e_struct*)
_tmp47F)->f2;_LL2D9: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp4DB,_tmp4DC);goto _LL28B;
_LL2DA: if(*((int*)_tmp47F)!= 39)goto _LL28B;_tmp4DD=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp47F)->f1;_LL2DB: if(!te->allow_valueof){const char*_tmp83E;void*_tmp83D;(
_tmp83D=0,Cyc_Tcutil_terr(e->loc,((_tmp83E="valueof(-) can only occur within types",
_tag_dyneither(_tmp83E,sizeof(char),39))),_tag_dyneither(_tmp83D,sizeof(void*),0)));}
t=Cyc_Absyn_sint_typ;goto _LL28B;_LL28B:;}{struct Cyc_Core_Opt*_tmp83F;e->topt=((
_tmp83F=_cycalloc(sizeof(*_tmp83F)),((_tmp83F->v=(void*)t,_tmp83F))));}}
