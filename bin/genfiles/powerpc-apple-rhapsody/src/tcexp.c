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
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;int allow_valueof;};struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Datatypedecl**
Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
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
int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
void*,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int
Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct
_tuple6{int f1;void*f2;};struct _tuple6 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_rgn_of_pointer(void*
t,void**rgn);struct _tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_okay_szofarg(void*t);
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int
new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*);struct
Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct _dyneither_ptr,struct Cyc_Position_Segment*);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;
};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part);extern int Cyc_Tcexp_in_stmt_exp;struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};struct Cyc_CfFlowInfo_UniquePlace{struct Cyc_CfFlowInfo_Place place;struct Cyc_List_List*
path;};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct Cyc_CfFlowInfo_LessNumelts_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};struct Cyc_CfFlowInfo_LessEqNumelts_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};union Cyc_CfFlowInfo_RelnOp_union{struct Cyc_CfFlowInfo_EqualConst_struct
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
Cyc_CfFlowInfo_Aggregate_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_ConsumeInfo{
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
_dyneither_ptr msg,struct _dyneither_ptr ap){{void*_tmp525;(_tmp525=0,Cyc_Tcutil_terr(
loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),
_tag_dyneither(_tmp525,sizeof(void*),0)));}if(topt == 0)return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q);static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct
_handler_cons _tmp1;_push_handler(& _tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))
_tmp3=1;if(!_tmp3){{struct _RegionHandle*_tmp4=Cyc_Tcenv_get_fnrgn(te);void*_tmp5=
Cyc_Tcenv_lookup_ordinary(_tmp4,te,e->loc,q);void*_tmp6;struct Cyc_Absyn_Enumdecl*
_tmp7;struct Cyc_Absyn_Enumfield*_tmp8;void*_tmp9;struct Cyc_Absyn_Enumfield*_tmpA;
struct Cyc_Absyn_Datatypedecl*_tmpB;struct Cyc_Absyn_Datatypefield*_tmpC;_LL1: if(*((
int*)_tmp5)!= 0)goto _LL3;_tmp6=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp5)->f1;
_LL2:{struct Cyc_Absyn_Var_e_struct _tmp528;struct Cyc_Absyn_Var_e_struct*_tmp527;(
void*)(e->r=(void*)((void*)((_tmp527=_cycalloc(sizeof(*_tmp527)),((_tmp527[0]=((
_tmp528.tag=1,((_tmp528.f1=q,((_tmp528.f2=(void*)_tmp6,_tmp528)))))),_tmp527))))));}
goto _LL0;_LL3: if(*((int*)_tmp5)!= 3)goto _LL5;_tmp7=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp5)->f1;_tmp8=((struct Cyc_Tcenv_EnumRes_struct*)_tmp5)->f2;_LL4:{struct Cyc_Absyn_Enum_e_struct
_tmp52B;struct Cyc_Absyn_Enum_e_struct*_tmp52A;(void*)(e->r=(void*)((void*)((
_tmp52A=_cycalloc(sizeof(*_tmp52A)),((_tmp52A[0]=((_tmp52B.tag=33,((_tmp52B.f1=q,((
_tmp52B.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7,((_tmp52B.f3=(struct Cyc_Absyn_Enumfield*)
_tmp8,_tmp52B)))))))),_tmp52A))))));}goto _LL0;_LL5: if(*((int*)_tmp5)!= 4)goto
_LL7;_tmp9=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f1;_tmpA=((
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f2;_LL6:{struct Cyc_Absyn_AnonEnum_e_struct
_tmp52E;struct Cyc_Absyn_AnonEnum_e_struct*_tmp52D;(void*)(e->r=(void*)((void*)((
_tmp52D=_cycalloc(sizeof(*_tmp52D)),((_tmp52D[0]=((_tmp52E.tag=34,((_tmp52E.f1=q,((
_tmp52E.f2=(void*)_tmp9,((_tmp52E.f3=(struct Cyc_Absyn_Enumfield*)_tmpA,_tmp52E)))))))),
_tmp52D))))));}goto _LL0;_LL7: if(*((int*)_tmp5)!= 2)goto _LL9;_tmpB=((struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp5)->f1;_tmpC=((struct Cyc_Tcenv_DatatypeRes_struct*)_tmp5)->f2;_LL8:{struct
Cyc_Absyn_Datatype_e_struct _tmp531;struct Cyc_Absyn_Datatype_e_struct*_tmp530;(
void*)(e->r=(void*)((void*)((_tmp530=_cycalloc(sizeof(*_tmp530)),((_tmp530[0]=((
_tmp531.tag=32,((_tmp531.f1=0,((_tmp531.f2=_tmpB,((_tmp531.f3=_tmpC,_tmp531)))))))),
_tmp530))))));}goto _LL0;_LL9: if(*((int*)_tmp5)!= 1)goto _LL0;_LLA:{const char*
_tmp535;void*_tmp534[1];struct Cyc_String_pa_struct _tmp533;(_tmp533.tag=0,((
_tmp533.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
q)),((_tmp534[0]=& _tmp533,Cyc_Tcutil_terr(e->loc,((_tmp535="bad occurrence of type name %s",
_tag_dyneither(_tmp535,sizeof(char),31))),_tag_dyneither(_tmp534,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Var_e_struct _tmp538;struct Cyc_Absyn_Var_e_struct*_tmp537;(void*)(
e->r=(void*)((void*)((_tmp537=_cycalloc(sizeof(*_tmp537)),((_tmp537[0]=((_tmp538.tag=
1,((_tmp538.f1=q,((_tmp538.f2=(void*)((void*)0),_tmp538)))))),_tmp537))))));}
goto _LL0;_LL0:;};_pop_handler();}else{void*_tmp2=(void*)_exn_thrown;void*_tmp1B=
_tmp2;_LLC: if(_tmp1B != Cyc_Dict_Absent)goto _LLE;_LLD:{struct Cyc_Absyn_Var_e_struct
_tmp53B;struct Cyc_Absyn_Var_e_struct*_tmp53A;(void*)(e->r=(void*)((void*)((
_tmp53A=_cycalloc(sizeof(*_tmp53A)),((_tmp53A[0]=((_tmp53B.tag=1,((_tmp53B.f1=q,((
_tmp53B.f2=(void*)((void*)0),_tmp53B)))))),_tmp53A))))));}goto _LLB;_LLE:;_LLF:(
void)_throw(_tmp1B);_LLB:;}}}struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple8*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e);static
struct _tuple8*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){struct _tuple8*
_tmp53C;return(_tmp53C=_cycalloc(sizeof(*_tmp53C)),((_tmp53C->f1=0,((_tmp53C->f2=
e,_tmp53C)))));}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es);static void
Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*_tmp1F=(void*)e1->r;struct
_tuple1*_tmp20;_LL11: if(*((int*)_tmp1F)!= 2)goto _LL13;_tmp20=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp1F)->f1;_LL12: {struct _handler_cons _tmp21;_push_handler(& _tmp21);{int _tmp23=
0;if(setjmp(_tmp21.handler))_tmp23=1;if(!_tmp23){{struct _RegionHandle*_tmp24=Cyc_Tcenv_get_fnrgn(
te);void*_tmp25=Cyc_Tcenv_lookup_ordinary(_tmp24,te,e1->loc,_tmp20);void*_tmp26;
struct Cyc_Absyn_Datatypedecl*_tmp27;struct Cyc_Absyn_Datatypefield*_tmp28;struct
Cyc_Absyn_Aggrdecl*_tmp29;_LL16: if(*((int*)_tmp25)!= 0)goto _LL18;_tmp26=(void*)((
struct Cyc_Tcenv_VarRes_struct*)_tmp25)->f1;_LL17:{struct Cyc_Absyn_FnCall_e_struct
_tmp53F;struct Cyc_Absyn_FnCall_e_struct*_tmp53E;(void*)(e->r=(void*)((void*)((
_tmp53E=_cycalloc(sizeof(*_tmp53E)),((_tmp53E[0]=((_tmp53F.tag=11,((_tmp53F.f1=
e1,((_tmp53F.f2=es,((_tmp53F.f3=0,_tmp53F)))))))),_tmp53E))))));}_npop_handler(0);
return;_LL18: if(*((int*)_tmp25)!= 2)goto _LL1A;_tmp27=((struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp25)->f1;_tmp28=((struct Cyc_Tcenv_DatatypeRes_struct*)_tmp25)->f2;_LL19: if(
_tmp28->typs == 0){const char*_tmp543;void*_tmp542[1];struct Cyc_String_pa_struct
_tmp541;(_tmp541.tag=0,((_tmp541.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp28->name)),((_tmp542[0]=& _tmp541,Cyc_Tcutil_terr(e->loc,((
_tmp543="%s is a constant, not a function",_tag_dyneither(_tmp543,sizeof(char),
33))),_tag_dyneither(_tmp542,sizeof(void*),1)))))));}{struct Cyc_Absyn_Datatype_e_struct
_tmp546;struct Cyc_Absyn_Datatype_e_struct*_tmp545;(void*)(e->r=(void*)((void*)((
_tmp545=_cycalloc(sizeof(*_tmp545)),((_tmp545[0]=((_tmp546.tag=32,((_tmp546.f1=
es,((_tmp546.f2=_tmp27,((_tmp546.f3=_tmp28,_tmp546)))))))),_tmp545))))));}
_npop_handler(0);return;_LL1A: if(*((int*)_tmp25)!= 1)goto _LL1C;_tmp29=((struct
Cyc_Tcenv_AggrRes_struct*)_tmp25)->f1;_LL1B: {struct Cyc_List_List*_tmp31=((
struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);{struct Cyc_Absyn_Aggregate_e_struct
_tmp549;struct Cyc_Absyn_Aggregate_e_struct*_tmp548;(void*)(e->r=(void*)((void*)((
_tmp548=_cycalloc(sizeof(*_tmp548)),((_tmp548[0]=((_tmp549.tag=30,((_tmp549.f1=
_tmp29->name,((_tmp549.f2=0,((_tmp549.f3=_tmp31,((_tmp549.f4=(struct Cyc_Absyn_Aggrdecl*)
_tmp29,_tmp549)))))))))),_tmp548))))));}_npop_handler(0);return;}_LL1C: if(*((int*)
_tmp25)!= 4)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp25)!= 3)goto _LL15;
_LL1F:{const char*_tmp54D;void*_tmp54C[1];struct Cyc_String_pa_struct _tmp54B;(
_tmp54B.tag=0,((_tmp54B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp20)),((_tmp54C[0]=& _tmp54B,Cyc_Tcutil_terr(e->loc,((_tmp54D="%s is an enum constructor, not a function",
_tag_dyneither(_tmp54D,sizeof(char),42))),_tag_dyneither(_tmp54C,sizeof(void*),1)))))));}
_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*_tmp22=(void*)
_exn_thrown;void*_tmp38=_tmp22;_LL21: if(_tmp38 != Cyc_Dict_Absent)goto _LL23;_LL22:{
struct Cyc_Absyn_FnCall_e_struct _tmp550;struct Cyc_Absyn_FnCall_e_struct*_tmp54F;(
void*)(e->r=(void*)((void*)((_tmp54F=_cycalloc(sizeof(*_tmp54F)),((_tmp54F[0]=((
_tmp550.tag=11,((_tmp550.f1=e1,((_tmp550.f2=es,((_tmp550.f3=0,_tmp550)))))))),
_tmp54F))))));}return;_LL23:;_LL24:(void)_throw(_tmp38);_LL20:;}}}_LL13:;_LL14:{
struct Cyc_Absyn_FnCall_e_struct _tmp553;struct Cyc_Absyn_FnCall_e_struct*_tmp552;(
void*)(e->r=(void*)((void*)((_tmp552=_cycalloc(sizeof(*_tmp552)),((_tmp552[0]=((
_tmp553.tag=11,((_tmp553.f1=e1,((_tmp553.f2=es,((_tmp553.f3=0,_tmp553)))))))),
_tmp552))))));}return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des);static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
des){if(topt == 0){{struct Cyc_Absyn_Array_e_struct _tmp556;struct Cyc_Absyn_Array_e_struct*
_tmp555;(void*)(e->r=(void*)((void*)((_tmp555=_cycalloc(sizeof(*_tmp555)),((
_tmp555[0]=((_tmp556.tag=28,((_tmp556.f1=des,_tmp556)))),_tmp555))))));}return;}{
void*t=*topt;void*_tmp3F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp40;
union Cyc_Absyn_AggrInfoU_union _tmp41;struct Cyc_Absyn_ArrayInfo _tmp42;void*_tmp43;
struct Cyc_Absyn_Tqual _tmp44;_LL26: if(_tmp3F <= (void*)4)goto _LL2C;if(*((int*)
_tmp3F)!= 10)goto _LL28;_tmp40=((struct Cyc_Absyn_AggrType_struct*)_tmp3F)->f1;
_tmp41=_tmp40.aggr_info;_LL27:{union Cyc_Absyn_AggrInfoU_union _tmp45=_tmp41;
struct Cyc_Absyn_Aggrdecl**_tmp46;struct Cyc_Absyn_Aggrdecl*_tmp47;_LL2F: if((
_tmp45.UnknownAggr).tag != 0)goto _LL31;_LL30: {const char*_tmp559;void*_tmp558;(
_tmp558=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp559="struct type not properly set",_tag_dyneither(_tmp559,sizeof(char),29))),
_tag_dyneither(_tmp558,sizeof(void*),0)));}_LL31: if((_tmp45.KnownAggr).tag != 1)
goto _LL2E;_tmp46=(_tmp45.KnownAggr).f1;_tmp47=*_tmp46;_LL32: {struct Cyc_Absyn_Aggregate_e_struct
_tmp55C;struct Cyc_Absyn_Aggregate_e_struct*_tmp55B;(void*)(e->r=(void*)((void*)((
_tmp55B=_cycalloc(sizeof(*_tmp55B)),((_tmp55B[0]=((_tmp55C.tag=30,((_tmp55C.f1=
_tmp47->name,((_tmp55C.f2=0,((_tmp55C.f3=des,((_tmp55C.f4=(struct Cyc_Absyn_Aggrdecl*)
_tmp47,_tmp55C)))))))))),_tmp55B))))));}_LL2E:;}goto _LL25;_LL28: if(*((int*)
_tmp3F)!= 7)goto _LL2A;_tmp42=((struct Cyc_Absyn_ArrayType_struct*)_tmp3F)->f1;
_tmp43=(void*)_tmp42.elt_type;_tmp44=_tmp42.tq;_LL29:{struct Cyc_Absyn_Array_e_struct
_tmp55F;struct Cyc_Absyn_Array_e_struct*_tmp55E;(void*)(e->r=(void*)((void*)((
_tmp55E=_cycalloc(sizeof(*_tmp55E)),((_tmp55E[0]=((_tmp55F.tag=28,((_tmp55F.f1=
des,_tmp55F)))),_tmp55E))))));}goto _LL25;_LL2A: if(*((int*)_tmp3F)!= 11)goto _LL2C;
_LL2B:{struct Cyc_Absyn_AnonStruct_e_struct _tmp562;struct Cyc_Absyn_AnonStruct_e_struct*
_tmp561;(void*)(e->r=(void*)((void*)((_tmp561=_cycalloc(sizeof(*_tmp561)),((
_tmp561[0]=((_tmp562.tag=31,((_tmp562.f1=(void*)t,((_tmp562.f2=des,_tmp562)))))),
_tmp561))))));}goto _LL25;_LL2C:;_LL2D:{struct Cyc_Absyn_Array_e_struct _tmp565;
struct Cyc_Absyn_Array_e_struct*_tmp564;(void*)(e->r=(void*)((void*)((_tmp564=
_cycalloc(sizeof(*_tmp564)),((_tmp564[0]=((_tmp565.tag=28,((_tmp565.f1=des,
_tmp565)))),_tmp564))))));}goto _LL25;_LL25:;}}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)es->hd);}}struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(e);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(te,e)){const char*
_tmp56A;void*_tmp569[2];struct Cyc_String_pa_struct _tmp568;struct Cyc_String_pa_struct
_tmp567;(_tmp567.tag=0,((_tmp567.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp568.tag=0,((_tmp568.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((
_tmp569[0]=& _tmp568,((_tmp569[1]=& _tmp567,Cyc_Tcutil_terr(e->loc,((_tmp56A="test of %s has type %s instead of integral or * type",
_tag_dyneither(_tmp56A,sizeof(char),53))),_tag_dyneither(_tmp569,sizeof(void*),2)))))))))))));}{
void*_tmp56=(void*)e->r;void*_tmp57;struct Cyc_List_List*_tmp58;_LL34: if(*((int*)
_tmp56)!= 3)goto _LL36;_tmp57=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp56)->f1;
_tmp58=((struct Cyc_Absyn_Primop_e_struct*)_tmp56)->f2;_LL35: if(_tmp57 == (void*)5
 || _tmp57 == (void*)6){struct _tuple0 _tmp56B;struct _tuple0 _tmp5A=(_tmp56B.f1=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp58))->hd)->topt))->v,((_tmp56B.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct
Cyc_List_List*)_check_null(_tmp58))->tl))->hd)->topt))->v,_tmp56B)));void*_tmp5B;
void*_tmp5C;void*_tmp5D;void*_tmp5E;_LL39: _tmp5B=_tmp5A.f1;if(_tmp5B <= (void*)4)
goto _LL3B;if(*((int*)_tmp5B)!= 14)goto _LL3B;_tmp5C=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5B)->f1;_tmp5D=_tmp5A.f2;if(_tmp5D <= (void*)4)goto _LL3B;if(*((int*)_tmp5D)!= 
14)goto _LL3B;_tmp5E=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp5D)->f1;
_LL3A: {struct _tuple0*_tmp56E;struct Cyc_Tcexp_TestEnv _tmp56D;return(_tmp56D.eq=((
_tmp56E=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp56E)),((_tmp56E->f1=
_tmp5C,((_tmp56E->f2=_tmp5E,_tmp56E)))))),((_tmp56D.isTrue=_tmp57 == (void*)5,
_tmp56D)));}_LL3B:;_LL3C: goto _LL38;_LL38:;}goto _LL33;_LL36:;_LL37: goto _LL33;
_LL33:;}{struct Cyc_Tcexp_TestEnv _tmp56F;return(_tmp56F.eq=0,((_tmp56F.isTrue=0,
_tmp56F)));}}static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,union Cyc_Absyn_Cnst_union*c,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcConst(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,union Cyc_Absyn_Cnst_union*
c,struct Cyc_Absyn_Exp*e){void*t;{union Cyc_Absyn_Cnst_union _tmp62=*((union Cyc_Absyn_Cnst_union*)
_check_null(c));void*_tmp63;void*_tmp64;void*_tmp65;void*_tmp66;void*_tmp67;void*
_tmp68;int _tmp69;struct _dyneither_ptr _tmp6A;_LL3E: if((_tmp62.Char_c).tag != 0)
goto _LL40;_tmp63=(_tmp62.Char_c).f1;if((int)_tmp63 != 0)goto _LL40;_LL3F: t=Cyc_Absyn_schar_typ;
goto _LL3D;_LL40: if((_tmp62.Char_c).tag != 0)goto _LL42;_tmp64=(_tmp62.Char_c).f1;
if((int)_tmp64 != 1)goto _LL42;_LL41: t=Cyc_Absyn_uchar_typ;goto _LL3D;_LL42: if((
_tmp62.Char_c).tag != 0)goto _LL44;_tmp65=(_tmp62.Char_c).f1;if((int)_tmp65 != 2)
goto _LL44;_LL43: t=Cyc_Absyn_char_typ;goto _LL3D;_LL44: if((_tmp62.Short_c).tag != 1)
goto _LL46;_tmp66=(_tmp62.Short_c).f1;_LL45: t=_tmp66 == (void*)1?Cyc_Absyn_ushort_typ:
Cyc_Absyn_sshort_typ;goto _LL3D;_LL46: if((_tmp62.LongLong_c).tag != 3)goto _LL48;
_tmp67=(_tmp62.LongLong_c).f1;_LL47: t=_tmp67 == (void*)1?Cyc_Absyn_ulonglong_typ:
Cyc_Absyn_slonglong_typ;goto _LL3D;_LL48: if((_tmp62.Float_c).tag != 4)goto _LL4A;
_LL49: t=Cyc_Absyn_float_typ;goto _LL3D;_LL4A: if((_tmp62.Int_c).tag != 2)goto _LL4C;
_tmp68=(_tmp62.Int_c).f1;_tmp69=(_tmp62.Int_c).f2;_LL4B: if(topt == 0)t=_tmp68 == (
void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{void*_tmp6B=Cyc_Tcutil_compress(*
topt);void*_tmp6C;void*_tmp6D;void*_tmp6E;void*_tmp6F;void*_tmp70;void*_tmp71;
void*_tmp72;void*_tmp73;void*_tmp74;_LL51: if(_tmp6B <= (void*)4)goto _LL5D;if(*((
int*)_tmp6B)!= 5)goto _LL53;_tmp6C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp6B)->f1;_tmp6D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp6B)->f2;if((int)
_tmp6D != 0)goto _LL53;_LL52:{void*_tmp75=_tmp6C;_LL60: if((int)_tmp75 != 1)goto
_LL62;_LL61: t=Cyc_Absyn_uchar_typ;goto _LL5F;_LL62: if((int)_tmp75 != 0)goto _LL64;
_LL63: t=Cyc_Absyn_schar_typ;goto _LL5F;_LL64: if((int)_tmp75 != 2)goto _LL5F;_LL65: t=
Cyc_Absyn_char_typ;goto _LL5F;_LL5F:;}{union Cyc_Absyn_Cnst_union _tmp570;*c=(union
Cyc_Absyn_Cnst_union)(((_tmp570.Char_c).tag=0,(((_tmp570.Char_c).f1=(void*)
_tmp6C,(((_tmp570.Char_c).f2=(char)_tmp69,_tmp570))))));}goto _LL50;_LL53: if(*((
int*)_tmp6B)!= 5)goto _LL55;_tmp6E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp6B)->f1;_tmp6F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp6B)->f2;if((int)
_tmp6F != 1)goto _LL55;_LL54: t=_tmp6E == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;{
union Cyc_Absyn_Cnst_union _tmp571;*c=(union Cyc_Absyn_Cnst_union)(((_tmp571.Short_c).tag=
1,(((_tmp571.Short_c).f1=(void*)_tmp6E,(((_tmp571.Short_c).f2=(short)_tmp69,
_tmp571))))));}goto _LL50;_LL55: if(*((int*)_tmp6B)!= 5)goto _LL57;_tmp70=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp6B)->f1;_tmp71=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp6B)->f2;if((int)_tmp71 != 2)goto _LL57;_LL56: t=_tmp70 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL50;_LL57: if(*((int*)_tmp6B)!= 5)goto _LL59;_tmp72=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp6B)->f1;_tmp73=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp6B)->f2;if((int)_tmp73 != 3)goto _LL59;_LL58: t=_tmp72 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL50;_LL59: if(*((int*)_tmp6B)!= 4)goto _LL5B;if(!(_tmp69
== 0))goto _LL5B;_LL5A:{struct Cyc_Absyn_Const_e_struct _tmp577;union Cyc_Absyn_Cnst_union
_tmp576;struct Cyc_Absyn_Const_e_struct*_tmp575;(void*)(e->r=(void*)((void*)((
_tmp575=_cycalloc(sizeof(*_tmp575)),((_tmp575[0]=((_tmp577.tag=0,((_tmp577.f1=(
union Cyc_Absyn_Cnst_union)(((_tmp576.Null_c).tag=6,_tmp576)),_tmp577)))),_tmp575))))));}{
struct Cyc_List_List*_tmp7B=Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct
_tmp58A;struct Cyc_Core_Opt*_tmp589;struct Cyc_Absyn_PtrAtts _tmp588;struct Cyc_Core_Opt*
_tmp587;struct Cyc_Absyn_PtrInfo _tmp586;struct Cyc_Absyn_PointerType_struct*
_tmp585;t=(void*)((_tmp585=_cycalloc(sizeof(*_tmp585)),((_tmp585[0]=((_tmp58A.tag=
4,((_tmp58A.f1=((_tmp586.elt_typ=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ak,((_tmp589=_cycalloc(sizeof(*_tmp589)),((_tmp589->v=_tmp7B,_tmp589))))),((
_tmp586.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp586.ptr_atts=((_tmp588.rgn=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,((_tmp587=_cycalloc(
sizeof(*_tmp587)),((_tmp587->v=_tmp7B,_tmp587))))),((_tmp588.nullable=Cyc_Absyn_true_conref,((
_tmp588.bounds=Cyc_Absyn_empty_conref(),((_tmp588.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(),((_tmp588.ptrloc=0,_tmp588)))))))))),_tmp586)))))),
_tmp58A)))),_tmp585))));}goto _LL50;}_LL5B: if(*((int*)_tmp6B)!= 18)goto _LL5D;
_tmp74=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp6B)->f1;_LL5C: {struct Cyc_Absyn_ValueofType_struct
_tmp58D;struct Cyc_Absyn_ValueofType_struct*_tmp58C;struct Cyc_Absyn_ValueofType_struct*
_tmp82=(_tmp58C=_cycalloc(sizeof(*_tmp58C)),((_tmp58C[0]=((_tmp58D.tag=17,((
_tmp58D.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp69,0),_tmp58D)))),_tmp58C)));if(
!Cyc_Tcutil_unify(_tmp74,(void*)_tmp82)){{const char*_tmp592;void*_tmp591[2];
struct Cyc_String_pa_struct _tmp590;struct Cyc_String_pa_struct _tmp58F;(_tmp58F.tag=
0,((_tmp58F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_tmp82)),((_tmp590.tag=0,((_tmp590.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp74)),((_tmp591[0]=& _tmp590,((_tmp591[1]=&
_tmp58F,Cyc_Tcutil_terr(loc,((_tmp592="expecting %s but found %s",_tag_dyneither(
_tmp592,sizeof(char),26))),_tag_dyneither(_tmp591,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct Cyc_Absyn_TagType_struct _tmp595;struct Cyc_Absyn_TagType_struct*
_tmp594;t=(void*)((_tmp594=_cycalloc(sizeof(*_tmp594)),((_tmp594[0]=((_tmp595.tag=
18,((_tmp595.f1=(void*)((void*)_tmp82),_tmp595)))),_tmp594))));}goto _LL50;}_LL5D:;
_LL5E: t=_tmp68 == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL50;_LL50:;}
goto _LL3D;_LL4C: if((_tmp62.String_c).tag != 5)goto _LL4E;_tmp6A=(_tmp62.String_c).f1;
_LL4D: {int len=(int)_get_dyneither_size(_tmp6A,sizeof(char));union Cyc_Absyn_Cnst_union
_tmp596;struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)(((
_tmp596.Int_c).tag=2,(((_tmp596.Int_c).f1=(void*)((void*)1),(((_tmp596.Int_c).f2=
len,_tmp596)))))),loc);{struct Cyc_Core_Opt*_tmp597;elen->topt=((_tmp597=
_cycalloc(sizeof(*_tmp597)),((_tmp597->v=(void*)Cyc_Absyn_uint_typ,_tmp597))));}{
struct Cyc_Absyn_Upper_b_struct _tmp59A;struct Cyc_Absyn_Upper_b_struct*_tmp599;t=
Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(0),(void*)((
_tmp599=_cycalloc(sizeof(*_tmp599)),((_tmp599[0]=((_tmp59A.tag=0,((_tmp59A.f1=
elen,_tmp59A)))),_tmp599)))),Cyc_Absyn_true_conref);}if(topt != 0){void*_tmp8E=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp8F;struct Cyc_Absyn_Tqual
_tmp90;_LL67: if(_tmp8E <= (void*)4)goto _LL6B;if(*((int*)_tmp8E)!= 7)goto _LL69;
_tmp8F=((struct Cyc_Absyn_ArrayType_struct*)_tmp8E)->f1;_tmp90=_tmp8F.tq;_LL68:
return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmp90,(struct Cyc_Absyn_Exp*)elen,((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),0);_LL69: if(*((int*)
_tmp8E)!= 4)goto _LL6B;_LL6A: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){{struct Cyc_Core_Opt*_tmp59B;e->topt=((_tmp59B=_cycalloc(sizeof(*
_tmp59B)),((_tmp59B->v=(void*)t,_tmp59B))));}Cyc_Tcutil_unchecked_cast(te,e,*
topt,(void*)3);t=*topt;}else{{struct Cyc_Absyn_Upper_b_struct _tmp59E;struct Cyc_Absyn_Upper_b_struct*
_tmp59D;t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(void*)((
_tmp59D=_cycalloc(sizeof(*_tmp59D)),((_tmp59D[0]=((_tmp59E.tag=0,((_tmp59E.f1=
elen,_tmp59E)))),_tmp59D)))),Cyc_Absyn_true_conref);}if(!Cyc_Tcutil_unify(*topt,
t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){{struct Cyc_Core_Opt*_tmp59F;e->topt=((
_tmp59F=_cycalloc(sizeof(*_tmp59F)),((_tmp59F->v=(void*)t,_tmp59F))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);t=*topt;}}goto _LL66;_LL6B:;_LL6C: goto _LL66;_LL66:;}return t;}
_LL4E: if((_tmp62.Null_c).tag != 6)goto _LL3D;_LL4F: {struct Cyc_List_List*_tmp96=
Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct _tmp5B2;struct
Cyc_Core_Opt*_tmp5B1;struct Cyc_Absyn_PtrAtts _tmp5B0;struct Cyc_Core_Opt*_tmp5AF;
struct Cyc_Absyn_PtrInfo _tmp5AE;struct Cyc_Absyn_PointerType_struct*_tmp5AD;t=(
void*)((_tmp5AD=_cycalloc(sizeof(*_tmp5AD)),((_tmp5AD[0]=((_tmp5B2.tag=4,((
_tmp5B2.f1=((_tmp5AE.elt_typ=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,((
_tmp5B1=_cycalloc(sizeof(*_tmp5B1)),((_tmp5B1->v=_tmp96,_tmp5B1))))),((_tmp5AE.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp5AE.ptr_atts=((_tmp5B0.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,((_tmp5AF=_cycalloc(sizeof(*_tmp5AF)),((
_tmp5AF->v=_tmp96,_tmp5AF))))),((_tmp5B0.nullable=Cyc_Absyn_true_conref,((
_tmp5B0.bounds=Cyc_Absyn_empty_conref(),((_tmp5B0.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(),((_tmp5B0.ptrloc=0,_tmp5B0)))))))))),_tmp5AE)))))),
_tmp5B2)))),_tmp5AD))));}goto _LL3D;}_LL3D:;}return t;}static void*Cyc_Tcexp_tcVar(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q,void*b);
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q,void*b){void*_tmp9D=b;struct Cyc_Absyn_Vardecl*_tmp9E;struct
Cyc_Absyn_Fndecl*_tmp9F;struct Cyc_Absyn_Vardecl*_tmpA0;struct Cyc_Absyn_Vardecl*
_tmpA1;struct Cyc_Absyn_Vardecl*_tmpA2;_LL6E: if((int)_tmp9D != 0)goto _LL70;_LL6F: {
const char*_tmp5B6;void*_tmp5B5[1];struct Cyc_String_pa_struct _tmp5B4;return(
_tmp5B4.tag=0,((_tmp5B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
q)),((_tmp5B5[0]=& _tmp5B4,Cyc_Tcexp_expr_err(te,loc,0,((_tmp5B6="undeclared identifier: %s",
_tag_dyneither(_tmp5B6,sizeof(char),26))),_tag_dyneither(_tmp5B5,sizeof(void*),1)))))));}
_LL70: if(_tmp9D <= (void*)1)goto _LL72;if(*((int*)_tmp9D)!= 0)goto _LL72;_tmp9E=((
struct Cyc_Absyn_Global_b_struct*)_tmp9D)->f1;_LL71:*q=*_tmp9E->name;return(void*)
_tmp9E->type;_LL72: if(_tmp9D <= (void*)1)goto _LL74;if(*((int*)_tmp9D)!= 1)goto
_LL74;_tmp9F=((struct Cyc_Absyn_Funname_b_struct*)_tmp9D)->f1;_LL73:*q=*_tmp9F->name;
return Cyc_Tcutil_fndecl2typ(_tmp9F);_LL74: if(_tmp9D <= (void*)1)goto _LL76;if(*((
int*)_tmp9D)!= 4)goto _LL76;_tmpA0=((struct Cyc_Absyn_Pat_b_struct*)_tmp9D)->f1;
_LL75: _tmpA1=_tmpA0;goto _LL77;_LL76: if(_tmp9D <= (void*)1)goto _LL78;if(*((int*)
_tmp9D)!= 3)goto _LL78;_tmpA1=((struct Cyc_Absyn_Local_b_struct*)_tmp9D)->f1;_LL77:
_tmpA2=_tmpA1;goto _LL79;_LL78: if(_tmp9D <= (void*)1)goto _LL6D;if(*((int*)_tmp9D)
!= 2)goto _LL6D;_tmpA2=((struct Cyc_Absyn_Param_b_struct*)_tmp9D)->f1;_LL79:{union
Cyc_Absyn_Nmspace_union _tmp5B7;(*q).f1=(union Cyc_Absyn_Nmspace_union)(((_tmp5B7.Loc_n).tag=
0,_tmp5B7));}return(void*)_tmpA2->type;_LL6D:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,
struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*));static
void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,
struct Cyc_Core_Opt*opt_args,struct _RegionHandle*temp,struct Cyc_List_List*(*
type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,
struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{void*_tmpA7=(void*)
fmt->r;union Cyc_Absyn_Cnst_union _tmpA8;struct _dyneither_ptr _tmpA9;struct Cyc_Absyn_Exp*
_tmpAA;struct Cyc_Absyn_Exp _tmpAB;void*_tmpAC;union Cyc_Absyn_Cnst_union _tmpAD;
struct _dyneither_ptr _tmpAE;_LL7B: if(*((int*)_tmpA7)!= 0)goto _LL7D;_tmpA8=((
struct Cyc_Absyn_Const_e_struct*)_tmpA7)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpA7)->f1).String_c).tag != 5)goto _LL7D;_tmpA9=(_tmpA8.String_c).f1;_LL7C:
_tmpAE=_tmpA9;goto _LL7E;_LL7D: if(*((int*)_tmpA7)!= 15)goto _LL7F;_tmpAA=((struct
Cyc_Absyn_Cast_e_struct*)_tmpA7)->f2;_tmpAB=*_tmpAA;_tmpAC=(void*)_tmpAB.r;if(*((
int*)_tmpAC)!= 0)goto _LL7F;_tmpAD=((struct Cyc_Absyn_Const_e_struct*)_tmpAC)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmpAC)->f1).String_c).tag != 5)goto _LL7F;
_tmpAE=(_tmpAD.String_c).f1;_LL7E: desc_types=type_getter(temp,te,(struct
_dyneither_ptr)_tmpAE,fmt->loc);goto _LL7A;_LL7F:;_LL80: if(opt_args != 0){struct
Cyc_List_List*_tmpAF=(struct Cyc_List_List*)opt_args->v;for(0;_tmpAF != 0;_tmpAF=
_tmpAF->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmpAF->hd);{struct
_RegionHandle*_tmpB0=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpB0,(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)_tmpAF->hd)->topt))->v)
 && !Cyc_Tcutil_is_noalias_path(_tmpB0,(struct Cyc_Absyn_Exp*)_tmpAF->hd)){const
char*_tmp5BA;void*_tmp5B9;(_tmp5B9=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpAF->hd)->loc,((_tmp5BA="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp5BA,sizeof(char),49))),_tag_dyneither(_tmp5B9,sizeof(void*),0)));}}}}
return;_LL7A:;}if(opt_args != 0){struct Cyc_List_List*_tmpB3=(struct Cyc_List_List*)
opt_args->v;for(0;desc_types != 0  && _tmpB3 != 0;(desc_types=desc_types->tl,_tmpB3=
_tmpB3->tl)){void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
_tmpB3->hd;Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){{
const char*_tmp5BF;void*_tmp5BE[2];struct Cyc_String_pa_struct _tmp5BD;struct Cyc_String_pa_struct
_tmp5BC;(_tmp5BC.tag=0,((_tmp5BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp5BD.tag=0,((_tmp5BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp5BE[0]=& _tmp5BD,((_tmp5BE[1]=& _tmp5BC,Cyc_Tcutil_terr(e->loc,((_tmp5BF="descriptor has type \n%s\n but argument has type \n%s",
_tag_dyneither(_tmp5BF,sizeof(char),51))),_tag_dyneither(_tmp5BE,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct _RegionHandle*_tmpB8=Cyc_Tcenv_get_fnrgn(te);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmpB8,t) && !Cyc_Tcutil_is_noalias_path(
_tmpB8,e)){const char*_tmp5C2;void*_tmp5C1;(_tmp5C1=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpB3->hd)->loc,((_tmp5C2="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp5C2,sizeof(char),49))),_tag_dyneither(_tmp5C1,sizeof(void*),0)));}}}
if(desc_types != 0){const char*_tmp5C5;void*_tmp5C4;(_tmp5C4=0,Cyc_Tcutil_terr(fmt->loc,((
_tmp5C5="too few arguments",_tag_dyneither(_tmp5C5,sizeof(char),18))),
_tag_dyneither(_tmp5C4,sizeof(void*),0)));}if(_tmpB3 != 0){const char*_tmp5C8;void*
_tmp5C7;(_tmp5C7=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpB3->hd)->loc,((
_tmp5C8="too many arguments",_tag_dyneither(_tmp5C8,sizeof(char),19))),
_tag_dyneither(_tmp5C7,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmpBF=p;_LL82: if((
int)_tmpBF != 0)goto _LL84;_LL83: goto _LL85;_LL84: if((int)_tmpBF != 2)goto _LL86;
_LL85: if(!Cyc_Tcutil_is_numeric(e)){const char*_tmp5CC;void*_tmp5CB[1];struct Cyc_String_pa_struct
_tmp5CA;(_tmp5CA.tag=0,((_tmp5CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp5CB[0]=& _tmp5CA,Cyc_Tcutil_terr(loc,((_tmp5CC="expecting arithmetic type but found %s",
_tag_dyneither(_tmp5CC,sizeof(char),39))),_tag_dyneither(_tmp5CB,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;_LL86: if((int)_tmpBF
!= 11)goto _LL88;_LL87: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(
te,e)){const char*_tmp5D0;void*_tmp5CF[1];struct Cyc_String_pa_struct _tmp5CE;(
_tmp5CE.tag=0,((_tmp5CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp5CF[0]=& _tmp5CE,Cyc_Tcutil_terr(loc,((_tmp5D0="expecting integral or * type but found %s",
_tag_dyneither(_tmp5D0,sizeof(char),42))),_tag_dyneither(_tmp5CF,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;_LL88: if((int)_tmpBF != 12)goto _LL8A;_LL89: if(!Cyc_Tcutil_is_integral(
e)){const char*_tmp5D4;void*_tmp5D3[1];struct Cyc_String_pa_struct _tmp5D2;(_tmp5D2.tag=
0,((_tmp5D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp5D3[0]=& _tmp5D2,Cyc_Tcutil_terr(loc,((_tmp5D4="expecting integral type but found %s",
_tag_dyneither(_tmp5D4,sizeof(char),37))),_tag_dyneither(_tmp5D3,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;_LL8A: if((int)_tmpBF
!= 19)goto _LL8C;_LL8B:{void*_tmpC9=t;struct Cyc_Absyn_PtrInfo _tmpCA;struct Cyc_Absyn_PtrAtts
_tmpCB;struct Cyc_Absyn_Conref*_tmpCC;_LL8F: if(_tmpC9 <= (void*)4)goto _LL93;if(*((
int*)_tmpC9)!= 7)goto _LL91;_LL90: goto _LL8E;_LL91: if(*((int*)_tmpC9)!= 4)goto
_LL93;_tmpCA=((struct Cyc_Absyn_PointerType_struct*)_tmpC9)->f1;_tmpCB=_tmpCA.ptr_atts;
_tmpCC=_tmpCB.bounds;_LL92:{union Cyc_Absyn_Constraint_union _tmpCD=(Cyc_Absyn_compress_conref(
_tmpCC))->v;void*_tmpCE;void*_tmpCF;_LL96: if((_tmpCD.Eq_constr).tag != 0)goto
_LL98;_tmpCE=(_tmpCD.Eq_constr).f1;if((int)_tmpCE != 0)goto _LL98;_LL97: goto _LL99;
_LL98: if((_tmpCD.Eq_constr).tag != 0)goto _LL9A;_tmpCF=(_tmpCD.Eq_constr).f1;if(
_tmpCF <= (void*)1)goto _LL9A;if(*((int*)_tmpCF)!= 0)goto _LL9A;_LL99: goto _LL95;
_LL9A:;_LL9B: {const char*_tmp5D7;void*_tmp5D6;(_tmp5D6=0,Cyc_Tcutil_terr(loc,((
_tmp5D7="can't apply numelts to pointer/array of abstract length",_tag_dyneither(
_tmp5D7,sizeof(char),56))),_tag_dyneither(_tmp5D6,sizeof(void*),0)));}_LL95:;}
goto _LL8E;_LL93:;_LL94: {const char*_tmp5DB;void*_tmp5DA[1];struct Cyc_String_pa_struct
_tmp5D9;(_tmp5D9.tag=0,((_tmp5D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp5DA[0]=& _tmp5D9,Cyc_Tcutil_terr(loc,((_tmp5DB="numelts requires pointer or array type, not %s",
_tag_dyneither(_tmp5DB,sizeof(char),47))),_tag_dyneither(_tmp5DA,sizeof(void*),1)))))));}
_LL8E:;}return Cyc_Absyn_uint_typ;_LL8C:;_LL8D: {const char*_tmp5DE;void*_tmp5DD;(
_tmp5DD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp5DE="Non-unary primop",_tag_dyneither(_tmp5DE,sizeof(char),17))),
_tag_dyneither(_tmp5DD,sizeof(void*),0)));}_LL81:;}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*));static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
if(!checker(e1)){{const char*_tmp5E2;void*_tmp5E1[1];struct Cyc_String_pa_struct
_tmp5E0;(_tmp5E0.tag=0,((_tmp5E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((
_tmp5E1[0]=& _tmp5E0,Cyc_Tcutil_terr(e1->loc,((_tmp5E2="type %s cannot be used here",
_tag_dyneither(_tmp5E2,sizeof(char),28))),_tag_dyneither(_tmp5E1,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}if(!checker(e2)){{
const char*_tmp5E6;void*_tmp5E5[1];struct Cyc_String_pa_struct _tmp5E4;(_tmp5E4.tag=
0,((_tmp5E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((_tmp5E5[0]=& _tmp5E4,
Cyc_Tcutil_terr(e2->loc,((_tmp5E6="type %s cannot be used here",_tag_dyneither(
_tmp5E6,sizeof(char),28))),_tag_dyneither(_tmp5E5,sizeof(void*),1)))))));}return
Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);return Cyc_Tcutil_max_arithmetic_type(
t1,t2);}}static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);void*_tmpDD=t1;struct Cyc_Absyn_PtrInfo
_tmpDE;void*_tmpDF;struct Cyc_Absyn_Tqual _tmpE0;struct Cyc_Absyn_PtrAtts _tmpE1;
void*_tmpE2;struct Cyc_Absyn_Conref*_tmpE3;struct Cyc_Absyn_Conref*_tmpE4;struct
Cyc_Absyn_Conref*_tmpE5;_LL9D: if(_tmpDD <= (void*)4)goto _LL9F;if(*((int*)_tmpDD)
!= 4)goto _LL9F;_tmpDE=((struct Cyc_Absyn_PointerType_struct*)_tmpDD)->f1;_tmpDF=(
void*)_tmpDE.elt_typ;_tmpE0=_tmpDE.elt_tq;_tmpE1=_tmpDE.ptr_atts;_tmpE2=(void*)
_tmpE1.rgn;_tmpE3=_tmpE1.nullable;_tmpE4=_tmpE1.bounds;_tmpE5=_tmpE1.zero_term;
_LL9E: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpDF),(void*)1)){const char*
_tmp5E9;void*_tmp5E8;(_tmp5E8=0,Cyc_Tcutil_terr(e1->loc,((_tmp5E9="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp5E9,sizeof(char),50))),_tag_dyneither(_tmp5E8,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp5EC;void*_tmp5EB;(_tmp5EB=0,
Cyc_Tcutil_terr(e1->loc,((_tmp5EC="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp5EC,sizeof(char),54))),_tag_dyneither(_tmp5EB,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){const char*_tmp5F0;void*_tmp5EF[1];struct
Cyc_String_pa_struct _tmp5EE;(_tmp5EE.tag=0,((_tmp5EE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp5EF[0]=& _tmp5EE,Cyc_Tcutil_terr(
e2->loc,((_tmp5F0="expecting int but found %s",_tag_dyneither(_tmp5F0,sizeof(
char),27))),_tag_dyneither(_tmp5EF,sizeof(void*),1)))))));}_tmpE4=Cyc_Absyn_compress_conref(
_tmpE4);{union Cyc_Absyn_Constraint_union _tmpED=_tmpE4->v;void*_tmpEE;void*_tmpEF;
struct Cyc_Absyn_Exp*_tmpF0;_LLA2: if((_tmpED.Eq_constr).tag != 0)goto _LLA4;_tmpEE=(
_tmpED.Eq_constr).f1;if((int)_tmpEE != 0)goto _LLA4;_LLA3: return t1;_LLA4: if((
_tmpED.Eq_constr).tag != 0)goto _LLA6;_tmpEF=(_tmpED.Eq_constr).f1;if(_tmpEF <= (
void*)1)goto _LLA6;if(*((int*)_tmpEF)!= 0)goto _LLA6;_tmpF0=((struct Cyc_Absyn_Upper_b_struct*)
_tmpEF)->f1;_LLA5: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpE5)){const char*_tmp5F3;void*_tmp5F2;(_tmp5F2=0,Cyc_Tcutil_warn(e1->loc,((
_tmp5F3="pointer arithmetic on thin, zero-terminated pointer may be expensive.",
_tag_dyneither(_tmp5F3,sizeof(char),70))),_tag_dyneither(_tmp5F2,sizeof(void*),0)));}{
struct Cyc_Absyn_PointerType_struct _tmp5FD;struct Cyc_Absyn_PtrAtts _tmp5FC;struct
Cyc_Absyn_PtrInfo _tmp5FB;struct Cyc_Absyn_PointerType_struct*_tmp5FA;struct Cyc_Absyn_PointerType_struct*
_tmpF3=(_tmp5FA=_cycalloc(sizeof(*_tmp5FA)),((_tmp5FA[0]=((_tmp5FD.tag=4,((
_tmp5FD.f1=((_tmp5FB.elt_typ=(void*)_tmpDF,((_tmp5FB.elt_tq=_tmpE0,((_tmp5FB.ptr_atts=((
_tmp5FC.rgn=(void*)_tmpE2,((_tmp5FC.nullable=Cyc_Absyn_true_conref,((_tmp5FC.bounds=
Cyc_Absyn_bounds_dyneither_conref,((_tmp5FC.zero_term=_tmpE5,((_tmp5FC.ptrloc=0,
_tmp5FC)))))))))),_tmp5FB)))))),_tmp5FD)))),_tmp5FA)));Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmpF3,(void*)3);return(void*)_tmpF3;}_LLA6:;_LLA7:{union Cyc_Absyn_Constraint_union
_tmp5FE;_tmpE4->v=(union Cyc_Absyn_Constraint_union)(((_tmp5FE.Eq_constr).tag=0,(((
_tmp5FE.Eq_constr).f1=(void*)((void*)0),_tmp5FE))));}return t1;_LLA1:;}_LL9F:;
_LLA0: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LL9C:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;
void*t1_elt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){if(!Cyc_Tcutil_unify(t1,t2)){{const char*_tmp603;void*_tmp602[2];struct Cyc_String_pa_struct
_tmp601;struct Cyc_String_pa_struct _tmp600;(_tmp600.tag=0,((_tmp600.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v)),((_tmp601.tag=0,((_tmp601.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp602[0]=& _tmp601,((_tmp602[1]=& _tmp600,Cyc_Tcutil_terr(
e1->loc,((_tmp603="pointer arithmetic on values of different types (%s != %s)",
_tag_dyneither(_tmp603,sizeof(char),59))),_tag_dyneither(_tmp602,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),(void*)1)){const char*_tmp606;void*_tmp605;(_tmp605=0,
Cyc_Tcutil_terr(e1->loc,((_tmp606="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp606,sizeof(char),50))),_tag_dyneither(_tmp605,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp609;void*_tmp608;(_tmp608=0,
Cyc_Tcutil_terr(e1->loc,((_tmp609="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp609,sizeof(char),54))),_tag_dyneither(_tmp608,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){{const char*_tmp60E;void*_tmp60D[2];struct
Cyc_String_pa_struct _tmp60C;struct Cyc_String_pa_struct _tmp60B;(_tmp60B.tag=0,((
_tmp60B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp60C.tag=0,((_tmp60C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp60D[0]=& _tmp60C,((_tmp60D[1]=& _tmp60B,Cyc_Tcutil_terr(
e2->loc,((_tmp60E="expecting either %s or int but found %s",_tag_dyneither(
_tmp60E,sizeof(char),40))),_tag_dyneither(_tmp60D,sizeof(void*),2)))))))))))));}
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
_tuple0 _tmp60F;struct _tuple0 _tmp106=(_tmp60F.f1=Cyc_Tcutil_compress(t1),((
_tmp60F.f2=Cyc_Tcutil_compress(t2),_tmp60F)));void*_tmp107;struct Cyc_Absyn_PtrInfo
_tmp108;void*_tmp109;void*_tmp10A;struct Cyc_Absyn_PtrInfo _tmp10B;void*_tmp10C;
void*_tmp10D;void*_tmp10E;_LLA9: _tmp107=_tmp106.f1;if(_tmp107 <= (void*)4)goto
_LLAB;if(*((int*)_tmp107)!= 4)goto _LLAB;_tmp108=((struct Cyc_Absyn_PointerType_struct*)
_tmp107)->f1;_tmp109=(void*)_tmp108.elt_typ;_tmp10A=_tmp106.f2;if(_tmp10A <= (
void*)4)goto _LLAB;if(*((int*)_tmp10A)!= 4)goto _LLAB;_tmp10B=((struct Cyc_Absyn_PointerType_struct*)
_tmp10A)->f1;_tmp10C=(void*)_tmp10B.elt_typ;_LLAA: if(Cyc_Tcutil_unify(_tmp109,
_tmp10C))return Cyc_Absyn_sint_typ;goto _LLA8;_LLAB: _tmp10D=_tmp106.f1;if(_tmp10D
<= (void*)4)goto _LLAD;if(*((int*)_tmp10D)!= 14)goto _LLAD;_tmp10E=_tmp106.f2;if(
_tmp10E <= (void*)4)goto _LLAD;if(*((int*)_tmp10E)!= 14)goto _LLAD;_LLAC: return Cyc_Absyn_sint_typ;
_LLAD:;_LLAE: goto _LLA8;_LLA8:;}{const char*_tmp614;void*_tmp613[2];struct Cyc_String_pa_struct
_tmp612;struct Cyc_String_pa_struct _tmp611;(_tmp611.tag=0,((_tmp611.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp612.tag=
0,((_tmp612.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp613[0]=& _tmp612,((_tmp613[1]=& _tmp611,Cyc_Tcutil_terr(loc,((_tmp614="comparison not allowed between %s and %s",
_tag_dyneither(_tmp614,sizeof(char),41))),_tag_dyneither(_tmp613,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcEqPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){if(Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2)== Cyc_Absyn_sint_typ)
return Cyc_Absyn_sint_typ;{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);{struct _tuple0 _tmp615;struct _tuple0 _tmp114=(_tmp615.f1=
t1,((_tmp615.f2=t2,_tmp615)));void*_tmp115;void*_tmp116;void*_tmp117;void*
_tmp118;_LLB0: _tmp115=_tmp114.f1;if(_tmp115 <= (void*)4)goto _LLB2;if(*((int*)
_tmp115)!= 14)goto _LLB2;_tmp116=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp115)->f1;_tmp117=_tmp114.f2;if(_tmp117 <= (void*)4)goto _LLB2;if(*((int*)
_tmp117)!= 14)goto _LLB2;_tmp118=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp117)->f1;_LLB1: return Cyc_Absyn_sint_typ;_LLB2:;_LLB3: goto _LLAF;_LLAF:;}{
const char*_tmp61A;void*_tmp619[2];struct Cyc_String_pa_struct _tmp618;struct Cyc_String_pa_struct
_tmp617;(_tmp617.tag=0,((_tmp617.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp618.tag=0,((_tmp618.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp619[0]=& _tmp618,((_tmp619[
1]=& _tmp617,Cyc_Tcutil_terr(loc,((_tmp61A="comparison not allowed between %s and %s",
_tag_dyneither(_tmp61A,sizeof(char),41))),_tag_dyneither(_tmp619,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*
Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp11D=p;
_LLB5: if((int)_tmp11D != 0)goto _LLB7;_LLB6: return Cyc_Tcexp_tcPlus(te,e1,e2);_LLB7:
if((int)_tmp11D != 2)goto _LLB9;_LLB8: return Cyc_Tcexp_tcMinus(te,e1,e2);_LLB9: if((
int)_tmp11D != 1)goto _LLBB;_LLBA: goto _LLBC;_LLBB: if((int)_tmp11D != 3)goto _LLBD;
_LLBC: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LLBD: if((int)
_tmp11D != 4)goto _LLBF;_LLBE: goto _LLC0;_LLBF: if((int)_tmp11D != 13)goto _LLC1;_LLC0:
goto _LLC2;_LLC1: if((int)_tmp11D != 14)goto _LLC3;_LLC2: goto _LLC4;_LLC3: if((int)
_tmp11D != 15)goto _LLC5;_LLC4: goto _LLC6;_LLC5: if((int)_tmp11D != 16)goto _LLC7;
_LLC6: goto _LLC8;_LLC7: if((int)_tmp11D != 17)goto _LLC9;_LLC8: goto _LLCA;_LLC9: if((
int)_tmp11D != 18)goto _LLCB;_LLCA: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);
_LLCB: if((int)_tmp11D != 5)goto _LLCD;_LLCC: goto _LLCE;_LLCD: if((int)_tmp11D != 6)
goto _LLCF;_LLCE: return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);_LLCF: if((int)_tmp11D != 
7)goto _LLD1;_LLD0: goto _LLD2;_LLD1: if((int)_tmp11D != 8)goto _LLD3;_LLD2: goto _LLD4;
_LLD3: if((int)_tmp11D != 9)goto _LLD5;_LLD4: goto _LLD6;_LLD5: if((int)_tmp11D != 10)
goto _LLD7;_LLD6: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_LLD7:;_LLD8: {const
char*_tmp61D;void*_tmp61C;(_tmp61C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp61D="bad binary primop",_tag_dyneither(
_tmp61D,sizeof(char),18))),_tag_dyneither(_tmp61C,sizeof(void*),0)));}_LLB4:;}
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_List_List*es);static void*Cyc_Tcexp_tcPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_List_List*es){if(p == (void*)2  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
es)== 1)return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd);Cyc_Tcexp_tcExpList(te,es);{void*t;switch(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLD9: {const char*_tmp620;void*
_tmp61F;return(_tmp61F=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp620="primitive operator has 0 arguments",
_tag_dyneither(_tmp620,sizeof(char),35))),_tag_dyneither(_tmp61F,sizeof(void*),0)));}
case 1: _LLDA: t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLDB: t=Cyc_Tcexp_tcBinPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(es))->tl))->hd);break;default: _LLDC: {const char*_tmp623;void*_tmp622;
return(_tmp622=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp623="primitive operator has > 2 arguments",
_tag_dyneither(_tmp623,sizeof(char),37))),_tag_dyneither(_tmp622,sizeof(void*),0)));}}
return t;}}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmp124=(void*)e->r;void*
_tmp125;struct Cyc_Absyn_Vardecl*_tmp126;void*_tmp127;struct Cyc_Absyn_Vardecl*
_tmp128;void*_tmp129;struct Cyc_Absyn_Vardecl*_tmp12A;void*_tmp12B;struct Cyc_Absyn_Vardecl*
_tmp12C;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*
_tmp12F;struct _dyneither_ptr*_tmp130;struct Cyc_Absyn_Exp*_tmp131;struct
_dyneither_ptr*_tmp132;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp134;
struct Cyc_Absyn_Exp*_tmp135;_LLDF: if(*((int*)_tmp124)!= 1)goto _LLE1;_tmp125=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp124)->f2;if(_tmp125 <= (void*)1)goto
_LLE1;if(*((int*)_tmp125)!= 2)goto _LLE1;_tmp126=((struct Cyc_Absyn_Param_b_struct*)
_tmp125)->f1;_LLE0: _tmp128=_tmp126;goto _LLE2;_LLE1: if(*((int*)_tmp124)!= 1)goto
_LLE3;_tmp127=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp124)->f2;if(_tmp127 <= (
void*)1)goto _LLE3;if(*((int*)_tmp127)!= 3)goto _LLE3;_tmp128=((struct Cyc_Absyn_Local_b_struct*)
_tmp127)->f1;_LLE2: _tmp12A=_tmp128;goto _LLE4;_LLE3: if(*((int*)_tmp124)!= 1)goto
_LLE5;_tmp129=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp124)->f2;if(_tmp129 <= (
void*)1)goto _LLE5;if(*((int*)_tmp129)!= 4)goto _LLE5;_tmp12A=((struct Cyc_Absyn_Pat_b_struct*)
_tmp129)->f1;_LLE4: _tmp12C=_tmp12A;goto _LLE6;_LLE5: if(*((int*)_tmp124)!= 1)goto
_LLE7;_tmp12B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp124)->f2;if(_tmp12B <= (
void*)1)goto _LLE7;if(*((int*)_tmp12B)!= 0)goto _LLE7;_tmp12C=((struct Cyc_Absyn_Global_b_struct*)
_tmp12B)->f1;_LLE6: if(!(_tmp12C->tq).real_const)return;goto _LLDE;_LLE7: if(*((int*)
_tmp124)!= 25)goto _LLE9;_tmp12D=((struct Cyc_Absyn_Subscript_e_struct*)_tmp124)->f1;
_tmp12E=((struct Cyc_Absyn_Subscript_e_struct*)_tmp124)->f2;_LLE8:{void*_tmp136=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp12D->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp137;struct Cyc_Absyn_Tqual _tmp138;struct Cyc_Absyn_ArrayInfo
_tmp139;struct Cyc_Absyn_Tqual _tmp13A;struct Cyc_List_List*_tmp13B;_LLF6: if(
_tmp136 <= (void*)4)goto _LLFC;if(*((int*)_tmp136)!= 4)goto _LLF8;_tmp137=((struct
Cyc_Absyn_PointerType_struct*)_tmp136)->f1;_tmp138=_tmp137.elt_tq;_LLF7: _tmp13A=
_tmp138;goto _LLF9;_LLF8: if(*((int*)_tmp136)!= 7)goto _LLFA;_tmp139=((struct Cyc_Absyn_ArrayType_struct*)
_tmp136)->f1;_tmp13A=_tmp139.tq;_LLF9: if(!_tmp13A.real_const)return;goto _LLF5;
_LLFA: if(*((int*)_tmp136)!= 9)goto _LLFC;_tmp13B=((struct Cyc_Absyn_TupleType_struct*)
_tmp136)->f1;_LLFB: {unsigned int _tmp13D;int _tmp13E;struct _tuple7 _tmp13C=Cyc_Evexp_eval_const_uint_exp(
_tmp12E);_tmp13D=_tmp13C.f1;_tmp13E=_tmp13C.f2;if(!_tmp13E){{const char*_tmp626;
void*_tmp625;(_tmp625=0,Cyc_Tcutil_terr(e->loc,((_tmp626="tuple projection cannot use sizeof or offsetof",
_tag_dyneither(_tmp626,sizeof(char),47))),_tag_dyneither(_tmp625,sizeof(void*),0)));}
return;}{struct _handler_cons _tmp141;_push_handler(& _tmp141);{int _tmp143=0;if(
setjmp(_tmp141.handler))_tmp143=1;if(!_tmp143){{struct _tuple9 _tmp145;struct Cyc_Absyn_Tqual
_tmp146;struct _tuple9*_tmp144=((struct _tuple9*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(_tmp13B,(int)_tmp13D);_tmp145=*_tmp144;_tmp146=_tmp145.f1;if(!
_tmp146.real_const){_npop_handler(0);return;}};_pop_handler();}else{void*_tmp142=(
void*)_exn_thrown;void*_tmp148=_tmp142;_LLFF: if(_tmp148 != Cyc_List_Nth)goto
_LL101;_LL100: return;_LL101:;_LL102:(void)_throw(_tmp148);_LLFE:;}}}goto _LLF5;}
_LLFC:;_LLFD: goto _LLF5;_LLF5:;}goto _LLDE;_LLE9: if(*((int*)_tmp124)!= 23)goto
_LLEB;_tmp12F=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp124)->f1;_tmp130=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp124)->f2;_LLEA:{void*_tmp149=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp12F->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp14A;union Cyc_Absyn_AggrInfoU_union _tmp14B;struct Cyc_Absyn_Aggrdecl**_tmp14C;
struct Cyc_List_List*_tmp14D;_LL104: if(_tmp149 <= (void*)4)goto _LL108;if(*((int*)
_tmp149)!= 10)goto _LL106;_tmp14A=((struct Cyc_Absyn_AggrType_struct*)_tmp149)->f1;
_tmp14B=_tmp14A.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp149)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL106;_tmp14C=(_tmp14B.KnownAggr).f1;_LL105: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp14C == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp14C,_tmp130);if(sf == 0  || !(sf->tq).real_const)return;goto _LL103;}_LL106: if(*((
int*)_tmp149)!= 11)goto _LL108;_tmp14D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp149)->f2;_LL107: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(
_tmp14D,_tmp130);if(sf == 0  || !(sf->tq).real_const)return;goto _LL103;}_LL108:;
_LL109: goto _LL103;_LL103:;}goto _LLDE;_LLEB: if(*((int*)_tmp124)!= 24)goto _LLED;
_tmp131=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp124)->f1;_tmp132=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp124)->f2;_LLEC:{void*_tmp14E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp131->topt))->v);struct Cyc_Absyn_PtrInfo _tmp14F;void*_tmp150;
_LL10B: if(_tmp14E <= (void*)4)goto _LL10D;if(*((int*)_tmp14E)!= 4)goto _LL10D;
_tmp14F=((struct Cyc_Absyn_PointerType_struct*)_tmp14E)->f1;_tmp150=(void*)
_tmp14F.elt_typ;_LL10C:{void*_tmp151=Cyc_Tcutil_compress(_tmp150);struct Cyc_Absyn_AggrInfo
_tmp152;union Cyc_Absyn_AggrInfoU_union _tmp153;struct Cyc_Absyn_Aggrdecl**_tmp154;
struct Cyc_List_List*_tmp155;_LL110: if(_tmp151 <= (void*)4)goto _LL114;if(*((int*)
_tmp151)!= 10)goto _LL112;_tmp152=((struct Cyc_Absyn_AggrType_struct*)_tmp151)->f1;
_tmp153=_tmp152.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp151)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL112;_tmp154=(_tmp153.KnownAggr).f1;_LL111: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp154 == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp154,_tmp132);if(sf == 0  || !(sf->tq).real_const)return;goto _LL10F;}_LL112: if(*((
int*)_tmp151)!= 11)goto _LL114;_tmp155=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp151)->f2;_LL113: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(
_tmp155,_tmp132);if(sf == 0  || !(sf->tq).real_const)return;goto _LL10F;}_LL114:;
_LL115: goto _LL10F;_LL10F:;}goto _LL10A;_LL10D:;_LL10E: goto _LL10A;_LL10A:;}goto
_LLDE;_LLED: if(*((int*)_tmp124)!= 22)goto _LLEF;_tmp133=((struct Cyc_Absyn_Deref_e_struct*)
_tmp124)->f1;_LLEE:{void*_tmp156=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp133->topt))->v);struct Cyc_Absyn_PtrInfo _tmp157;struct Cyc_Absyn_Tqual
_tmp158;struct Cyc_Absyn_ArrayInfo _tmp159;struct Cyc_Absyn_Tqual _tmp15A;_LL117: if(
_tmp156 <= (void*)4)goto _LL11B;if(*((int*)_tmp156)!= 4)goto _LL119;_tmp157=((
struct Cyc_Absyn_PointerType_struct*)_tmp156)->f1;_tmp158=_tmp157.elt_tq;_LL118:
_tmp15A=_tmp158;goto _LL11A;_LL119: if(*((int*)_tmp156)!= 7)goto _LL11B;_tmp159=((
struct Cyc_Absyn_ArrayType_struct*)_tmp156)->f1;_tmp15A=_tmp159.tq;_LL11A: if(!
_tmp15A.real_const)return;goto _LL116;_LL11B:;_LL11C: goto _LL116;_LL116:;}goto
_LLDE;_LLEF: if(*((int*)_tmp124)!= 13)goto _LLF1;_tmp134=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp124)->f1;_LLF0: _tmp135=_tmp134;goto _LLF2;_LLF1: if(*((int*)_tmp124)!= 14)goto
_LLF3;_tmp135=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp124)->f1;_LLF2: Cyc_Tcexp_check_writable(
te,_tmp135);return;_LLF3:;_LLF4: goto _LLDE;_LLDE:;}{const char*_tmp62A;void*
_tmp629[1];struct Cyc_String_pa_struct _tmp628;(_tmp628.tag=0,((_tmp628.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp629[0]=&
_tmp628,Cyc_Tcutil_terr(e->loc,((_tmp62A="attempt to write a const location: %s",
_tag_dyneither(_tmp62A,sizeof(char),38))),_tag_dyneither(_tmp629,sizeof(void*),1)))))));}}
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,void*i);static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,void*i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(e)){const char*
_tmp62D;void*_tmp62C;return(_tmp62C=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp62D="increment/decrement of non-lvalue",
_tag_dyneither(_tmp62D,sizeof(char),34))),_tag_dyneither(_tmp62C,sizeof(void*),0)));}
Cyc_Tcexp_check_writable(te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v;if(!Cyc_Tcutil_is_numeric(e)){void*telt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == (void*)0  || i == (
void*)1)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(void*)1)){const char*
_tmp630;void*_tmp62F;(_tmp62F=0,Cyc_Tcutil_terr(e->loc,((_tmp630="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp630,sizeof(char),50))),_tag_dyneither(_tmp62F,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t)){const char*_tmp633;void*_tmp632;(_tmp632=0,
Cyc_Tcutil_terr(e->loc,((_tmp633="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp633,sizeof(char),54))),_tag_dyneither(_tmp632,sizeof(void*),0)));}}
else{const char*_tmp637;void*_tmp636[1];struct Cyc_String_pa_struct _tmp635;(
_tmp635.tag=0,((_tmp635.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp636[0]=& _tmp635,Cyc_Tcutil_terr(e->loc,((_tmp637="expecting arithmetic or ? type but found %s",
_tag_dyneither(_tmp637,sizeof(char),44))),_tag_dyneither(_tmp636,sizeof(void*),1)))))));}}
return t;}}static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3);static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3){struct _tuple0*_tmp169;int _tmp16A;const char*_tmp638;struct Cyc_Tcexp_TestEnv
_tmp168=Cyc_Tcexp_tcTest(te,e1,((_tmp638="conditional expression",_tag_dyneither(
_tmp638,sizeof(char),23))));_tmp169=_tmp168.eq;_tmp16A=_tmp168.isTrue;Cyc_Tcexp_tcExp(
te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_List_List _tmp639;
struct Cyc_List_List _tmp16B=(_tmp639.hd=e3,((_tmp639.tl=0,_tmp639)));struct Cyc_List_List
_tmp63A;struct Cyc_List_List _tmp16C=(_tmp63A.hd=e2,((_tmp63A.tl=(struct Cyc_List_List*)&
_tmp16B,_tmp63A)));if(!Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)&
_tmp16C)){{const char*_tmp63F;void*_tmp63E[2];struct Cyc_String_pa_struct _tmp63D;
struct Cyc_String_pa_struct _tmp63C;(_tmp63C.tag=0,((_tmp63C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e3->topt))->v)),((_tmp63D.tag=0,((_tmp63D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v)),((_tmp63E[0]=& _tmp63D,((_tmp63E[1]=& _tmp63C,Cyc_Tcutil_terr(
loc,((_tmp63F="conditional clause types do not match: %s != %s",_tag_dyneither(
_tmp63F,sizeof(char),48))),_tag_dyneither(_tmp63E,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t;}}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{const char*_tmp640;Cyc_Tcexp_tcTest(
te,e1,((_tmp640="logical-and expression",_tag_dyneither(_tmp640,sizeof(char),23))));}{
const char*_tmp641;Cyc_Tcexp_tcTest(te,e2,((_tmp641="logical-and expression",
_tag_dyneither(_tmp641,sizeof(char),23))));}return Cyc_Absyn_sint_typ;}static void*
Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp642;Cyc_Tcexp_tcTest(te,e1,((_tmp642="logical-or expression",
_tag_dyneither(_tmp642,sizeof(char),22))));}{const char*_tmp643;Cyc_Tcexp_tcTest(
te,e2,((_tmp643="logical-or expression",_tag_dyneither(_tmp643,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp177=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(_tmp177,te),0,e1);Cyc_Tcexp_tcExp(
te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),
e2);{void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{void*_tmp178=Cyc_Tcutil_compress(
t1);_LL11E: if(_tmp178 <= (void*)4)goto _LL120;if(*((int*)_tmp178)!= 7)goto _LL120;
_LL11F:{const char*_tmp646;void*_tmp645;(_tmp645=0,Cyc_Tcutil_terr(loc,((_tmp646="cannot assign to an array",
_tag_dyneither(_tmp646,sizeof(char),26))),_tag_dyneither(_tmp645,sizeof(void*),0)));}
goto _LL11D;_LL120:;_LL121: goto _LL11D;_LL11D:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),(void*)1)){const char*_tmp649;void*_tmp648;(_tmp648=0,Cyc_Tcutil_terr(loc,((
_tmp649="type is abstract (can't determine size).",_tag_dyneither(_tmp649,
sizeof(char),41))),_tag_dyneither(_tmp648,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(
e1)){const char*_tmp64C;void*_tmp64B;return(_tmp64B=0,Cyc_Tcexp_expr_err(te,loc,
topt,((_tmp64C="assignment to non-lvalue",_tag_dyneither(_tmp64C,sizeof(char),25))),
_tag_dyneither(_tmp64B,sizeof(void*),0)));}Cyc_Tcexp_check_writable(te,e1);if(po
== 0){struct _RegionHandle*_tmp17F=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp17F,t2) && !Cyc_Tcutil_is_noalias_path(_tmp17F,e2)){const char*_tmp64F;void*
_tmp64E;(_tmp64E=0,Cyc_Tcutil_terr(e2->loc,((_tmp64F="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp64F,sizeof(char),49))),_tag_dyneither(_tmp64E,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){const char*_tmp654;void*_tmp653[2];struct
Cyc_String_pa_struct _tmp652;struct Cyc_String_pa_struct _tmp651;void*_tmp182=(
_tmp651.tag=0,((_tmp651.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp652.tag=0,((_tmp652.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp653[0]=& _tmp652,((_tmp653[1]=& _tmp651,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp654="type mismatch: %s != %s",_tag_dyneither(_tmp654,sizeof(
char),24))),_tag_dyneither(_tmp653,sizeof(void*),2)))))))))))));Cyc_Tcutil_unify(
t1,t2);Cyc_Tcutil_explain_failure();return _tmp182;}}else{void*_tmp187=(void*)po->v;
void*_tmp188=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp187,e1,e2);if(!(Cyc_Tcutil_unify(
_tmp188,t1) || Cyc_Tcutil_coerceable(_tmp188) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp659;void*_tmp658[2];struct Cyc_String_pa_struct _tmp657;struct Cyc_String_pa_struct
_tmp656;void*_tmp189=(_tmp656.tag=0,((_tmp656.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp657.tag=0,((_tmp657.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp658[0]=&
_tmp657,((_tmp658[1]=& _tmp656,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp659="Cannot use this operator in an assignment when the arguments have types %s and %s",
_tag_dyneither(_tmp659,sizeof(char),82))),_tag_dyneither(_tmp658,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(_tmp188,t1);Cyc_Tcutil_explain_failure();return _tmp189;}return
_tmp188;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,
0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct Cyc_Core_Opt*)_check_null(
e2->topt))->v;}static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*
inst,struct Cyc_List_List*fs);static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={6,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp18E=Cyc_Tcutil_compress(
t1);void*_tmp18F;void*_tmp190;_LL123: if((int)_tmp18E != 1)goto _LL125;_LL124: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ,(void*)1);t1=dbl_typ;goto _LL122;_LL125: if(_tmp18E <= (void*)4)goto
_LL129;if(*((int*)_tmp18E)!= 5)goto _LL127;_tmp18F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp18E)->f2;if((int)_tmp18F != 0)goto _LL127;_LL126: goto _LL128;_LL127: if(*((int*)
_tmp18E)!= 5)goto _LL129;_tmp190=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp18E)->f2;
if((int)_tmp190 != 1)goto _LL129;_LL128: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);t1=Cyc_Absyn_sint_typ;goto _LL122;_LL129:;_LL12A: goto _LL122;_LL122:;}
for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp192;struct Cyc_List_List*
_tmp193;struct Cyc_Position_Segment*_tmp194;void*_tmp195;struct Cyc_Absyn_Datatypefield
_tmp191=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp192=_tmp191.name;
_tmp193=_tmp191.typs;_tmp194=_tmp191.loc;_tmp195=(void*)_tmp191.sc;if(_tmp193 == 
0  || _tmp193->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple9*)_tmp193->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Datatypefield*)((
struct Cyc_Absyn_Datatypefield*)fields->hd);}}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp197;struct Cyc_List_List*_tmp198;struct Cyc_Position_Segment*
_tmp199;void*_tmp19A;struct Cyc_Absyn_Datatypefield _tmp196=*((struct Cyc_Absyn_Datatypefield*)
fields->hd);_tmp197=_tmp196.name;_tmp198=_tmp196.typs;_tmp199=_tmp196.loc;
_tmp19A=(void*)_tmp196.sc;if(_tmp198 == 0  || _tmp198->tl != 0)continue;{void*t2=
Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple9*)_tmp198->hd)).f2);if(Cyc_Tcutil_coerce_arg(
te,e,t2))return(struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)
fields->hd);}}{const char*_tmp65E;void*_tmp65D[2];struct Cyc_String_pa_struct
_tmp65C;struct Cyc_String_pa_struct _tmp65B;(_tmp65B.tag=0,((_tmp65B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp65C.tag=
0,((_tmp65C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
tu)),((_tmp65D[0]=& _tmp65C,((_tmp65D[1]=& _tmp65B,Cyc_Tcutil_terr(e->loc,((
_tmp65E="can't find a field in %s to inject a value of type %s",_tag_dyneither(
_tmp65E,sizeof(char),54))),_tag_dyneither(_tmp65D,sizeof(void*),2)))))))))))));}
return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**
vararg_call_info);static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){struct Cyc_List_List*
_tmp1A0=args;struct _RegionHandle*_tmp1A1=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*
_tmp1A2=Cyc_Tcenv_new_block(_tmp1A1,loc,te_orig);Cyc_Tcexp_tcExp(_tmp1A2,0,e);{
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp1A3=t;struct Cyc_Absyn_PtrInfo _tmp1A4;void*_tmp1A5;struct Cyc_Absyn_Tqual
_tmp1A6;struct Cyc_Absyn_PtrAtts _tmp1A7;void*_tmp1A8;struct Cyc_Absyn_Conref*
_tmp1A9;struct Cyc_Absyn_Conref*_tmp1AA;struct Cyc_Absyn_Conref*_tmp1AB;_LL12C: if(
_tmp1A3 <= (void*)4)goto _LL12E;if(*((int*)_tmp1A3)!= 4)goto _LL12E;_tmp1A4=((
struct Cyc_Absyn_PointerType_struct*)_tmp1A3)->f1;_tmp1A5=(void*)_tmp1A4.elt_typ;
_tmp1A6=_tmp1A4.elt_tq;_tmp1A7=_tmp1A4.ptr_atts;_tmp1A8=(void*)_tmp1A7.rgn;
_tmp1A9=_tmp1A7.nullable;_tmp1AA=_tmp1A7.bounds;_tmp1AB=_tmp1A7.zero_term;_LL12D:
Cyc_Tcenv_check_rgn_accessible(_tmp1A2,loc,_tmp1A8);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp1AA);{void*_tmp1AC=Cyc_Tcutil_compress(_tmp1A5);struct Cyc_Absyn_FnInfo
_tmp1AD;struct Cyc_List_List*_tmp1AE;struct Cyc_Core_Opt*_tmp1AF;void*_tmp1B0;
struct Cyc_List_List*_tmp1B1;int _tmp1B2;struct Cyc_Absyn_VarargInfo*_tmp1B3;struct
Cyc_List_List*_tmp1B4;struct Cyc_List_List*_tmp1B5;_LL131: if(_tmp1AC <= (void*)4)
goto _LL133;if(*((int*)_tmp1AC)!= 8)goto _LL133;_tmp1AD=((struct Cyc_Absyn_FnType_struct*)
_tmp1AC)->f1;_tmp1AE=_tmp1AD.tvars;_tmp1AF=_tmp1AD.effect;_tmp1B0=(void*)_tmp1AD.ret_typ;
_tmp1B1=_tmp1AD.args;_tmp1B2=_tmp1AD.c_varargs;_tmp1B3=_tmp1AD.cyc_varargs;
_tmp1B4=_tmp1AD.rgn_po;_tmp1B5=_tmp1AD.attributes;_LL132: if(topt != 0)Cyc_Tcutil_unify(
_tmp1B0,*topt);while(_tmp1A0 != 0  && _tmp1B1 != 0){struct Cyc_Absyn_Exp*e1=(struct
Cyc_Absyn_Exp*)_tmp1A0->hd;void*t2=(*((struct _tuple2*)_tmp1B1->hd)).f3;Cyc_Tcexp_tcExp(
_tmp1A2,(void**)& t2,e1);if(!Cyc_Tcutil_coerce_arg(_tmp1A2,e1,t2)){{const char*
_tmp663;void*_tmp662[2];struct Cyc_String_pa_struct _tmp661;struct Cyc_String_pa_struct
_tmp660;(_tmp660.tag=0,((_tmp660.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp661.tag=0,((_tmp661.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp662[0]=& _tmp661,((_tmp662[1]=& _tmp660,Cyc_Tcutil_terr(
e1->loc,((_tmp663="actual argument has type \n\t%s\n but formal has type \n\t%s.",
_tag_dyneither(_tmp663,sizeof(char),57))),_tag_dyneither(_tmp662,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1A1,t2) && !Cyc_Tcutil_is_noalias_path(
_tmp1A1,e1)){const char*_tmp666;void*_tmp665;(_tmp665=0,Cyc_Tcutil_terr(e1->loc,((
_tmp666="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp666,sizeof(char),49))),_tag_dyneither(_tmp665,sizeof(void*),0)));}_tmp1A0=
_tmp1A0->tl;_tmp1B1=_tmp1B1->tl;}{int args_already_checked=0;{struct Cyc_List_List*
a=_tmp1B5;for(0;a != 0;a=a->tl){void*_tmp1BC=(void*)a->hd;void*_tmp1BD;int _tmp1BE;
int _tmp1BF;_LL136: if(_tmp1BC <= (void*)17)goto _LL138;if(*((int*)_tmp1BC)!= 3)goto
_LL138;_tmp1BD=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp1BC)->f1;_tmp1BE=((
struct Cyc_Absyn_Format_att_struct*)_tmp1BC)->f2;_tmp1BF=((struct Cyc_Absyn_Format_att_struct*)
_tmp1BC)->f3;_LL137:{struct _handler_cons _tmp1C0;_push_handler(& _tmp1C0);{int
_tmp1C2=0;if(setjmp(_tmp1C0.handler))_tmp1C2=1;if(!_tmp1C2){{struct Cyc_Absyn_Exp*
_tmp1C3=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,
_tmp1BE - 1);struct Cyc_Core_Opt*fmt_args;if(_tmp1BF == 0)fmt_args=0;else{struct Cyc_Core_Opt*
_tmp667;fmt_args=((_tmp667=_cycalloc(sizeof(*_tmp667)),((_tmp667->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp1BF - 1),_tmp667))));}
args_already_checked=1;{struct _RegionHandle*_tmp1C5=Cyc_Tcenv_get_fnrgn(te_orig);
void*_tmp1C6=_tmp1BD;_LL13B: if((int)_tmp1C6 != 0)goto _LL13D;_LL13C: Cyc_Tcexp_check_format_args(
_tmp1A2,_tmp1C3,fmt_args,_tmp1C5,Cyc_Formatstr_get_format_typs);goto _LL13A;
_LL13D: if((int)_tmp1C6 != 1)goto _LL13A;_LL13E: Cyc_Tcexp_check_format_args(_tmp1A2,
_tmp1C3,fmt_args,_tmp1C5,Cyc_Formatstr_get_scanf_typs);goto _LL13A;_LL13A:;}};
_pop_handler();}else{void*_tmp1C1=(void*)_exn_thrown;void*_tmp1C8=_tmp1C1;_LL140:
if(_tmp1C8 != Cyc_List_Nth)goto _LL142;_LL141:{const char*_tmp66A;void*_tmp669;(
_tmp669=0,Cyc_Tcutil_terr(loc,((_tmp66A="bad format arguments",_tag_dyneither(
_tmp66A,sizeof(char),21))),_tag_dyneither(_tmp669,sizeof(void*),0)));}goto _LL13F;
_LL142:;_LL143:(void)_throw(_tmp1C8);_LL13F:;}}}goto _LL135;_LL138:;_LL139: goto
_LL135;_LL135:;}}if(_tmp1B1 != 0){const char*_tmp66D;void*_tmp66C;(_tmp66C=0,Cyc_Tcutil_terr(
loc,((_tmp66D="too few arguments for function",_tag_dyneither(_tmp66D,sizeof(
char),31))),_tag_dyneither(_tmp66C,sizeof(void*),0)));}else{if((_tmp1A0 != 0  || 
_tmp1B2) || _tmp1B3 != 0){if(_tmp1B2)for(0;_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){Cyc_Tcexp_tcExp(
_tmp1A2,0,(struct Cyc_Absyn_Exp*)_tmp1A0->hd);}else{if(_tmp1B3 == 0){const char*
_tmp670;void*_tmp66F;(_tmp66F=0,Cyc_Tcutil_terr(loc,((_tmp670="too many arguments for function",
_tag_dyneither(_tmp670,sizeof(char),32))),_tag_dyneither(_tmp66F,sizeof(void*),0)));}
else{void*_tmp1D0;int _tmp1D1;struct Cyc_Absyn_VarargInfo _tmp1CF=*_tmp1B3;_tmp1D0=(
void*)_tmp1CF.type;_tmp1D1=_tmp1CF.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp671;struct Cyc_Absyn_VarargCallInfo*_tmp1D2=(_tmp671=_cycalloc(sizeof(*
_tmp671)),((_tmp671->num_varargs=0,((_tmp671->injectors=0,((_tmp671->vai=(struct
Cyc_Absyn_VarargInfo*)_tmp1B3,_tmp671)))))));*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp1D2;if(!_tmp1D1)for(0;_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp1A0->hd;++ _tmp1D2->num_varargs;Cyc_Tcexp_tcExp(
_tmp1A2,(void**)& _tmp1D0,e1);if(!Cyc_Tcutil_coerce_arg(_tmp1A2,e1,_tmp1D0)){{
const char*_tmp676;void*_tmp675[2];struct Cyc_String_pa_struct _tmp674;struct Cyc_String_pa_struct
_tmp673;(_tmp673.tag=0,((_tmp673.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((
_tmp674.tag=0,((_tmp674.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp1D0)),((_tmp675[0]=& _tmp674,((_tmp675[1]=& _tmp673,Cyc_Tcutil_terr(loc,((
_tmp676="vararg requires type %s but argument has type %s",_tag_dyneither(
_tmp676,sizeof(char),49))),_tag_dyneither(_tmp675,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1A1,
_tmp1D0) && !Cyc_Tcutil_is_noalias_path(_tmp1A1,e1)){const char*_tmp679;void*
_tmp678;(_tmp678=0,Cyc_Tcutil_terr(e1->loc,((_tmp679="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp679,sizeof(char),49))),_tag_dyneither(_tmp678,sizeof(void*),0)));}}
else{void*_tmp1D9=Cyc_Tcutil_compress(_tmp1D0);struct Cyc_Absyn_DatatypeInfo
_tmp1DA;union Cyc_Absyn_DatatypeInfoU_union _tmp1DB;struct Cyc_Absyn_Datatypedecl**
_tmp1DC;struct Cyc_Absyn_Datatypedecl*_tmp1DD;struct Cyc_List_List*_tmp1DE;struct
Cyc_Core_Opt*_tmp1DF;_LL145: if(_tmp1D9 <= (void*)4)goto _LL147;if(*((int*)_tmp1D9)
!= 2)goto _LL147;_tmp1DA=((struct Cyc_Absyn_DatatypeType_struct*)_tmp1D9)->f1;
_tmp1DB=_tmp1DA.datatype_info;if((((((struct Cyc_Absyn_DatatypeType_struct*)
_tmp1D9)->f1).datatype_info).KnownDatatype).tag != 1)goto _LL147;_tmp1DC=(_tmp1DB.KnownDatatype).f1;
_tmp1DD=*_tmp1DC;_tmp1DE=_tmp1DA.targs;_tmp1DF=_tmp1DA.rgn;_LL146: {struct Cyc_Absyn_Datatypedecl*
_tmp1E0=*Cyc_Tcenv_lookup_datatypedecl(_tmp1A2,loc,_tmp1DD->name);struct Cyc_List_List*
fields=0;if(_tmp1E0->fields == 0){const char*_tmp67D;void*_tmp67C[1];struct Cyc_String_pa_struct
_tmp67B;(_tmp67B.tag=0,((_tmp67B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp1D0)),((_tmp67C[0]=& _tmp67B,Cyc_Tcutil_terr(loc,((
_tmp67D="can't inject into %s",_tag_dyneither(_tmp67D,sizeof(char),21))),
_tag_dyneither(_tmp67C,sizeof(void*),1)))))));}else{fields=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1E0->fields))->v;}if(!Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1DF))->v,Cyc_Tcenv_curr_rgn(_tmp1A2))){const
char*_tmp680;void*_tmp67F;(_tmp67F=0,Cyc_Tcutil_terr(loc,((_tmp680="bad region for injected varargs",
_tag_dyneither(_tmp680,sizeof(char),32))),_tag_dyneither(_tmp67F,sizeof(void*),0)));}{
struct _RegionHandle*_tmp1E6=Cyc_Tcenv_get_fnrgn(_tmp1A2);{struct Cyc_List_List*
_tmp1E7=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp1E6,_tmp1E6,
_tmp1E0->tvs,_tmp1DE);for(0;_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){++ _tmp1D2->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp1A0->hd;if(!
args_already_checked){Cyc_Tcexp_tcExp(_tmp1A2,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1E6,(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(
_tmp1E6,e1)){const char*_tmp683;void*_tmp682;(_tmp682=0,Cyc_Tcutil_terr(e1->loc,((
_tmp683="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp683,sizeof(char),49))),_tag_dyneither(_tmp682,sizeof(void*),0)));}}{struct
Cyc_Absyn_Datatypefield*_tmp1EA=Cyc_Tcexp_tcInjection(_tmp1A2,e1,_tmp1D0,_tmp1E6,
_tmp1E7,fields);if(_tmp1EA != 0){struct Cyc_List_List*_tmp684;_tmp1D2->injectors=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1D2->injectors,((_tmp684=_cycalloc(sizeof(*_tmp684)),((_tmp684->hd=(struct
Cyc_Absyn_Datatypefield*)_tmp1EA,((_tmp684->tl=0,_tmp684)))))));}}}}}goto _LL144;}}
_LL147:;_LL148:{const char*_tmp687;void*_tmp686;(_tmp686=0,Cyc_Tcutil_terr(loc,((
_tmp687="bad inject vararg type",_tag_dyneither(_tmp687,sizeof(char),23))),
_tag_dyneither(_tmp686,sizeof(void*),0)));}goto _LL144;_LL144:;}}}}}}Cyc_Tcenv_check_effect_accessible(
_tmp1A2,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1AF))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp1A2,loc,_tmp1B4);return _tmp1B0;}_LL133:;_LL134: {const char*_tmp68A;void*
_tmp689;return(_tmp689=0,Cyc_Tcexp_expr_err(_tmp1A2,loc,topt,((_tmp68A="expected pointer to function",
_tag_dyneither(_tmp68A,sizeof(char),29))),_tag_dyneither(_tmp689,sizeof(void*),0)));}
_LL130:;}_LL12E:;_LL12F: {const char*_tmp68D;void*_tmp68C;return(_tmp68C=0,Cyc_Tcexp_expr_err(
_tmp1A2,loc,topt,((_tmp68D="expected pointer to function",_tag_dyneither(_tmp68D,
sizeof(char),29))),_tag_dyneither(_tmp68C,sizeof(void*),0)));}_LL12B:;}}static
void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_exn_typ,e);if(!Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ)){
const char*_tmp691;void*_tmp690[1];struct Cyc_String_pa_struct _tmp68F;(_tmp68F.tag=
0,((_tmp68F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp690[0]=& _tmp68F,Cyc_Tcutil_terr(
loc,((_tmp691="expected datatype exn but found %s",_tag_dyneither(_tmp691,
sizeof(char),35))),_tag_dyneither(_tmp690,sizeof(void*),1)))))));}return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));}static void*Cyc_Tcexp_tcInstantiate(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*ts);static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
ts){Cyc_Tcexp_tcExpNoInst(te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
0));{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp1F6=t1;struct Cyc_Absyn_PtrInfo _tmp1F7;void*_tmp1F8;struct Cyc_Absyn_Tqual
_tmp1F9;struct Cyc_Absyn_PtrAtts _tmp1FA;void*_tmp1FB;struct Cyc_Absyn_Conref*
_tmp1FC;struct Cyc_Absyn_Conref*_tmp1FD;struct Cyc_Absyn_Conref*_tmp1FE;_LL14A: if(
_tmp1F6 <= (void*)4)goto _LL14C;if(*((int*)_tmp1F6)!= 4)goto _LL14C;_tmp1F7=((
struct Cyc_Absyn_PointerType_struct*)_tmp1F6)->f1;_tmp1F8=(void*)_tmp1F7.elt_typ;
_tmp1F9=_tmp1F7.elt_tq;_tmp1FA=_tmp1F7.ptr_atts;_tmp1FB=(void*)_tmp1FA.rgn;
_tmp1FC=_tmp1FA.nullable;_tmp1FD=_tmp1FA.bounds;_tmp1FE=_tmp1FA.zero_term;_LL14B:{
void*_tmp1FF=Cyc_Tcutil_compress(_tmp1F8);struct Cyc_Absyn_FnInfo _tmp200;struct
Cyc_List_List*_tmp201;struct Cyc_Core_Opt*_tmp202;void*_tmp203;struct Cyc_List_List*
_tmp204;int _tmp205;struct Cyc_Absyn_VarargInfo*_tmp206;struct Cyc_List_List*
_tmp207;struct Cyc_List_List*_tmp208;_LL14F: if(_tmp1FF <= (void*)4)goto _LL151;if(*((
int*)_tmp1FF)!= 8)goto _LL151;_tmp200=((struct Cyc_Absyn_FnType_struct*)_tmp1FF)->f1;
_tmp201=_tmp200.tvars;_tmp202=_tmp200.effect;_tmp203=(void*)_tmp200.ret_typ;
_tmp204=_tmp200.args;_tmp205=_tmp200.c_varargs;_tmp206=_tmp200.cyc_varargs;
_tmp207=_tmp200.rgn_po;_tmp208=_tmp200.attributes;_LL150: {struct _RegionHandle*
_tmp209=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*instantiation=0;for(0;ts != 0
 && _tmp201 != 0;(ts=ts->tl,_tmp201=_tmp201->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp201->hd);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);{struct _tuple5*_tmp694;struct Cyc_List_List*_tmp693;
instantiation=((_tmp693=_region_malloc(_tmp209,sizeof(*_tmp693)),((_tmp693->hd=((
_tmp694=_region_malloc(_tmp209,sizeof(*_tmp694)),((_tmp694->f1=(struct Cyc_Absyn_Tvar*)
_tmp201->hd,((_tmp694->f2=(void*)ts->hd,_tmp694)))))),((_tmp693->tl=
instantiation,_tmp693))))));}}if(ts != 0){const char*_tmp697;void*_tmp696;return(
_tmp696=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp697="too many type variables in instantiation",
_tag_dyneither(_tmp697,sizeof(char),41))),_tag_dyneither(_tmp696,sizeof(void*),0)));}{
struct Cyc_Absyn_FnType_struct _tmp69D;struct Cyc_Absyn_FnInfo _tmp69C;struct Cyc_Absyn_FnType_struct*
_tmp69B;void*new_fn_typ=Cyc_Tcutil_rsubstitute(_tmp209,instantiation,(void*)((
_tmp69B=_cycalloc(sizeof(*_tmp69B)),((_tmp69B[0]=((_tmp69D.tag=8,((_tmp69D.f1=((
_tmp69C.tvars=_tmp201,((_tmp69C.effect=_tmp202,((_tmp69C.ret_typ=(void*)_tmp203,((
_tmp69C.args=_tmp204,((_tmp69C.c_varargs=_tmp205,((_tmp69C.cyc_varargs=_tmp206,((
_tmp69C.rgn_po=_tmp207,((_tmp69C.attributes=_tmp208,_tmp69C)))))))))))))))),
_tmp69D)))),_tmp69B)))));struct Cyc_Absyn_PointerType_struct _tmp6A7;struct Cyc_Absyn_PtrAtts
_tmp6A6;struct Cyc_Absyn_PtrInfo _tmp6A5;struct Cyc_Absyn_PointerType_struct*
_tmp6A4;return(void*)((_tmp6A4=_cycalloc(sizeof(*_tmp6A4)),((_tmp6A4[0]=((
_tmp6A7.tag=4,((_tmp6A7.f1=((_tmp6A5.elt_typ=(void*)new_fn_typ,((_tmp6A5.elt_tq=
_tmp1F9,((_tmp6A5.ptr_atts=((_tmp6A6.rgn=(void*)_tmp1FB,((_tmp6A6.nullable=
_tmp1FC,((_tmp6A6.bounds=_tmp1FD,((_tmp6A6.zero_term=_tmp1FE,((_tmp6A6.ptrloc=0,
_tmp6A6)))))))))),_tmp6A5)))))),_tmp6A7)))),_tmp6A4))));}}_LL151:;_LL152: goto
_LL14E;_LL14E:;}goto _LL149;_LL14C:;_LL14D: goto _LL149;_LL149:;}{const char*_tmp6AB;
void*_tmp6AA[1];struct Cyc_String_pa_struct _tmp6A9;return(_tmp6A9.tag=0,((_tmp6A9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((
_tmp6AA[0]=& _tmp6A9,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6AB="expecting polymorphic type but found %s",
_tag_dyneither(_tmp6AB,sizeof(char),40))),_tag_dyneither(_tmp6AA,sizeof(void*),1)))))));}}}
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,void**c);static void*Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct
Cyc_Absyn_Exp*e,void**c){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((void**)
_check_null(c))=(void*)1;else{void*crc=Cyc_Tcutil_castable(te,loc,t2,t);if(crc != (
void*)0)*((void**)_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((
void**)_check_null(c))=(void*)1;else{Cyc_Tcutil_unify(t2,t);{const char*_tmp6B0;
void*_tmp6AF[2];struct Cyc_String_pa_struct _tmp6AE;struct Cyc_String_pa_struct
_tmp6AD;void*_tmp218=(_tmp6AD.tag=0,((_tmp6AD.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6AE.tag=0,((_tmp6AE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp6AF[0]=&
_tmp6AE,((_tmp6AF[1]=& _tmp6AD,Cyc_Tcexp_expr_err(te,loc,(void**)& t,((_tmp6B0="cannot cast %s to %s",
_tag_dyneither(_tmp6B0,sizeof(char),21))),_tag_dyneither(_tmp6AF,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();return _tmp218;}}}}{struct _tuple0 _tmp6B1;struct
_tuple0 _tmp21E=(_tmp6B1.f1=(void*)e->r,((_tmp6B1.f2=Cyc_Tcutil_compress(t),
_tmp6B1)));void*_tmp21F;struct Cyc_Absyn_MallocInfo _tmp220;int _tmp221;void*
_tmp222;struct Cyc_Absyn_PtrInfo _tmp223;struct Cyc_Absyn_PtrAtts _tmp224;struct Cyc_Absyn_Conref*
_tmp225;struct Cyc_Absyn_Conref*_tmp226;struct Cyc_Absyn_Conref*_tmp227;_LL154:
_tmp21F=_tmp21E.f1;if(*((int*)_tmp21F)!= 35)goto _LL156;_tmp220=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp21F)->f1;_tmp221=_tmp220.fat_result;_tmp222=_tmp21E.f2;if(_tmp222 <= (void*)4)
goto _LL156;if(*((int*)_tmp222)!= 4)goto _LL156;_tmp223=((struct Cyc_Absyn_PointerType_struct*)
_tmp222)->f1;_tmp224=_tmp223.ptr_atts;_tmp225=_tmp224.nullable;_tmp226=_tmp224.bounds;
_tmp227=_tmp224.zero_term;_LL155: if((_tmp221  && !((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp227)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp225)){void*_tmp228=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp226);struct Cyc_Absyn_Exp*_tmp229;_LL159: if(_tmp228 <= (void*)1)goto _LL15B;if(*((
int*)_tmp228)!= 0)goto _LL15B;_tmp229=((struct Cyc_Absyn_Upper_b_struct*)_tmp228)->f1;
_LL15A: if((Cyc_Evexp_eval_const_uint_exp(_tmp229)).f1 == 1){const char*_tmp6B4;
void*_tmp6B3;(_tmp6B3=0,Cyc_Tcutil_warn(loc,((_tmp6B4="cast from ? pointer to * pointer will lose size information",
_tag_dyneither(_tmp6B4,sizeof(char),60))),_tag_dyneither(_tmp6B3,sizeof(void*),0)));}
goto _LL158;_LL15B:;_LL15C: goto _LL158;_LL158:;}goto _LL153;_LL156:;_LL157: goto
_LL153;_LL153:;}return t;}}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e);static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){void**_tmp22C=0;
struct Cyc_Absyn_Tqual _tmp22D=Cyc_Absyn_empty_tqual(0);if(topt != 0){void*_tmp22E=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp22F;void*_tmp230;struct Cyc_Absyn_Tqual
_tmp231;struct Cyc_Absyn_PtrAtts _tmp232;struct Cyc_Absyn_Conref*_tmp233;_LL15E: if(
_tmp22E <= (void*)4)goto _LL160;if(*((int*)_tmp22E)!= 4)goto _LL160;_tmp22F=((
struct Cyc_Absyn_PointerType_struct*)_tmp22E)->f1;_tmp230=(void*)_tmp22F.elt_typ;
_tmp231=_tmp22F.elt_tq;_tmp232=_tmp22F.ptr_atts;_tmp233=_tmp232.zero_term;_LL15F:{
void**_tmp6B5;_tmp22C=((_tmp6B5=_cycalloc(sizeof(*_tmp6B5)),((_tmp6B5[0]=_tmp230,
_tmp6B5))));}_tmp22D=_tmp231;goto _LL15D;_LL160:;_LL161: goto _LL15D;_LL15D:;}{
struct _RegionHandle*_tmp235=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_clear_notreadctxt(
_tmp235,te),_tmp22C,e);if(Cyc_Tcutil_is_noalias_path(_tmp235,e)){const char*
_tmp6B8;void*_tmp6B7;(_tmp6B7=0,Cyc_Tcutil_terr(e->loc,((_tmp6B8="Cannot take the address of an alias-free path",
_tag_dyneither(_tmp6B8,sizeof(char),46))),_tag_dyneither(_tmp6B7,sizeof(void*),0)));}{
void*_tmp238=(void*)e->r;struct Cyc_Absyn_Exp*_tmp239;struct Cyc_Absyn_Exp*_tmp23A;
_LL163: if(*((int*)_tmp238)!= 25)goto _LL165;_tmp239=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp238)->f1;_tmp23A=((struct Cyc_Absyn_Subscript_e_struct*)_tmp238)->f2;_LL164:{
void*_tmp23B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp239->topt))->v);_LL168: if(_tmp23B <= (void*)4)goto _LL16A;if(*((int*)_tmp23B)
!= 9)goto _LL16A;_LL169: goto _LL167;_LL16A:;_LL16B:(void*)(e0->r=(void*)((void*)(
Cyc_Absyn_add_exp(_tmp239,_tmp23A,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp239,
_tmp23A);_LL167:;}goto _LL162;_LL165:;_LL166: goto _LL162;_LL162:;}{void*_tmp23C=(
void*)e->r;int _tmp23D;int _tmp23E;_LL16D: if(*((int*)_tmp23C)!= 23)goto _LL16F;
_tmp23D=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp23C)->f3;if(_tmp23D != 1)goto
_LL16F;_LL16E: goto _LL170;_LL16F: if(*((int*)_tmp23C)!= 24)goto _LL171;_tmp23E=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp23C)->f3;if(_tmp23E != 1)goto _LL171;
_LL170:{const char*_tmp6BB;void*_tmp6BA;(_tmp6BA=0,Cyc_Tcutil_terr(e->loc,((
_tmp6BB="cannot take the address of a @tagged union member",_tag_dyneither(
_tmp6BB,sizeof(char),50))),_tag_dyneither(_tmp6BA,sizeof(void*),0)));}goto _LL16C;
_LL171:;_LL172: goto _LL16C;_LL16C:;}{int _tmp242;void*_tmp243;struct _tuple6 _tmp241=
Cyc_Tcutil_addressof_props(te,e);_tmp242=_tmp241.f1;_tmp243=_tmp241.f2;{struct
Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp242){tq.print_const=1;tq.real_const=
1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
_tmp243,tq,Cyc_Absyn_false_conref);return t;}}}}}static void*Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t);static
void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t){if(te->allow_valueof)return Cyc_Absyn_uint_typ;Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);if(!Cyc_Evexp_okay_szofarg(t)){
const char*_tmp6BF;void*_tmp6BE[1];struct Cyc_String_pa_struct _tmp6BD;(_tmp6BD.tag=
0,((_tmp6BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp6BE[0]=& _tmp6BD,Cyc_Tcutil_terr(loc,((_tmp6BF="sizeof applied to type %s, which has unknown size here",
_tag_dyneither(_tmp6BF,sizeof(char),55))),_tag_dyneither(_tmp6BE,sizeof(void*),1)))))));}
if(topt != 0){void*_tmp247=Cyc_Tcutil_compress(*topt);void*_tmp248;_LL174: if(
_tmp247 <= (void*)4)goto _LL176;if(*((int*)_tmp247)!= 18)goto _LL176;_tmp248=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp247)->f1;_LL175: {struct Cyc_Absyn_Exp*
_tmp249=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct _tmp6C2;
struct Cyc_Absyn_ValueofType_struct*_tmp6C1;struct Cyc_Absyn_ValueofType_struct*
_tmp24A=(_tmp6C1=_cycalloc(sizeof(*_tmp6C1)),((_tmp6C1[0]=((_tmp6C2.tag=17,((
_tmp6C2.f1=_tmp249,_tmp6C2)))),_tmp6C1)));if(Cyc_Tcutil_unify(_tmp248,(void*)
_tmp24A))return _tmp247;goto _LL173;}_LL176:;_LL177: goto _LL173;_LL173:;}return Cyc_Absyn_uint_typ;}
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf);int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf){return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 
0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,void*n);static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*_tmp24D=n;struct
_dyneither_ptr*_tmp24E;unsigned int _tmp24F;_LL179: if(*((int*)_tmp24D)!= 0)goto
_LL17B;_tmp24E=((struct Cyc_Absyn_StructField_struct*)_tmp24D)->f1;_LL17A: {int
bad_type=1;{void*_tmp250=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp251;
union Cyc_Absyn_AggrInfoU_union _tmp252;struct Cyc_Absyn_Aggrdecl**_tmp253;struct
Cyc_List_List*_tmp254;_LL17E: if(_tmp250 <= (void*)4)goto _LL182;if(*((int*)_tmp250)
!= 10)goto _LL180;_tmp251=((struct Cyc_Absyn_AggrType_struct*)_tmp250)->f1;_tmp252=
_tmp251.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp250)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL180;_tmp253=(_tmp252.KnownAggr).f1;_LL17F: if((*_tmp253)->impl == 0)
goto _LL17D;if(!((int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),
struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp24E,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp253)->impl))->fields)){
const char*_tmp6C6;void*_tmp6C5[1];struct Cyc_String_pa_struct _tmp6C4;(_tmp6C4.tag=
0,((_tmp6C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp24E),((_tmp6C5[
0]=& _tmp6C4,Cyc_Tcutil_terr(loc,((_tmp6C6="no field of struct/union has name %s",
_tag_dyneither(_tmp6C6,sizeof(char),37))),_tag_dyneither(_tmp6C5,sizeof(void*),1)))))));}
bad_type=0;goto _LL17D;_LL180: if(*((int*)_tmp250)!= 11)goto _LL182;_tmp254=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp250)->f2;_LL181: if(!((int(*)(int(*pred)(
struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp24E,
_tmp254)){const char*_tmp6CA;void*_tmp6C9[1];struct Cyc_String_pa_struct _tmp6C8;(
_tmp6C8.tag=0,((_tmp6C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp24E),((
_tmp6C9[0]=& _tmp6C8,Cyc_Tcutil_terr(loc,((_tmp6CA="no field of struct/union has name %s",
_tag_dyneither(_tmp6CA,sizeof(char),37))),_tag_dyneither(_tmp6C9,sizeof(void*),1)))))));}
bad_type=0;goto _LL17D;_LL182:;_LL183: goto _LL17D;_LL17D:;}if(bad_type){const char*
_tmp6CE;void*_tmp6CD[1];struct Cyc_String_pa_struct _tmp6CC;(_tmp6CC.tag=0,((
_tmp6CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp6CD[0]=& _tmp6CC,Cyc_Tcutil_terr(loc,((_tmp6CE="%s is not a known struct/union type",
_tag_dyneither(_tmp6CE,sizeof(char),36))),_tag_dyneither(_tmp6CD,sizeof(void*),1)))))));}
goto _LL178;}_LL17B: if(*((int*)_tmp24D)!= 1)goto _LL178;_tmp24F=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp24D)->f1;_LL17C: {int bad_type=1;{void*_tmp25E=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_AggrInfo _tmp25F;union Cyc_Absyn_AggrInfoU_union _tmp260;struct Cyc_Absyn_Aggrdecl**
_tmp261;struct Cyc_List_List*_tmp262;struct Cyc_List_List*_tmp263;struct Cyc_Absyn_DatatypeFieldInfo
_tmp264;union Cyc_Absyn_DatatypeFieldInfoU_union _tmp265;struct Cyc_Absyn_Datatypefield*
_tmp266;_LL185: if(_tmp25E <= (void*)4)goto _LL18D;if(*((int*)_tmp25E)!= 10)goto
_LL187;_tmp25F=((struct Cyc_Absyn_AggrType_struct*)_tmp25E)->f1;_tmp260=_tmp25F.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp25E)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL187;_tmp261=(_tmp260.KnownAggr).f1;_LL186: if((*_tmp261)->impl == 0)
goto _LL184;_tmp262=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp261)->impl))->fields;
goto _LL188;_LL187: if(*((int*)_tmp25E)!= 11)goto _LL189;_tmp262=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp25E)->f2;_LL188: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp262)
<= _tmp24F){const char*_tmp6D3;void*_tmp6D2[2];struct Cyc_Int_pa_struct _tmp6D1;
struct Cyc_Int_pa_struct _tmp6D0;(_tmp6D0.tag=1,((_tmp6D0.f1=(unsigned long)((int)
_tmp24F),((_tmp6D1.tag=1,((_tmp6D1.f1=(unsigned long)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp262),((_tmp6D2[0]=& _tmp6D1,((_tmp6D2[1]=& _tmp6D0,Cyc_Tcutil_terr(
loc,((_tmp6D3="struct/union has too few components: %d <= %d",_tag_dyneither(
_tmp6D3,sizeof(char),46))),_tag_dyneither(_tmp6D2,sizeof(void*),2)))))))))))));}
bad_type=0;goto _LL184;_LL189: if(*((int*)_tmp25E)!= 9)goto _LL18B;_tmp263=((struct
Cyc_Absyn_TupleType_struct*)_tmp25E)->f1;_LL18A: if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp263)<= _tmp24F){const char*_tmp6D8;void*_tmp6D7[2];struct
Cyc_Int_pa_struct _tmp6D6;struct Cyc_Int_pa_struct _tmp6D5;(_tmp6D5.tag=1,((_tmp6D5.f1=(
unsigned long)((int)_tmp24F),((_tmp6D6.tag=1,((_tmp6D6.f1=(unsigned long)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp263),((_tmp6D7[0]=& _tmp6D6,((_tmp6D7[
1]=& _tmp6D5,Cyc_Tcutil_terr(loc,((_tmp6D8="tuple has too few components: %d <= %d",
_tag_dyneither(_tmp6D8,sizeof(char),39))),_tag_dyneither(_tmp6D7,sizeof(void*),2)))))))))))));}
bad_type=0;goto _LL184;_LL18B: if(*((int*)_tmp25E)!= 3)goto _LL18D;_tmp264=((struct
Cyc_Absyn_DatatypeFieldType_struct*)_tmp25E)->f1;_tmp265=_tmp264.field_info;if((((((
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp25E)->f1).field_info).KnownDatatypefield).tag
!= 1)goto _LL18D;_tmp266=(_tmp265.KnownDatatypefield).f2;_LL18C: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp266->typs)< _tmp24F){const char*
_tmp6DD;void*_tmp6DC[2];struct Cyc_Int_pa_struct _tmp6DB;struct Cyc_Int_pa_struct
_tmp6DA;(_tmp6DA.tag=1,((_tmp6DA.f1=(unsigned long)((int)_tmp24F),((_tmp6DB.tag=
1,((_tmp6DB.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp266->typs),((_tmp6DC[0]=& _tmp6DB,((_tmp6DC[1]=& _tmp6DA,Cyc_Tcutil_terr(loc,((
_tmp6DD="datatype field has too few components: %d < %d",_tag_dyneither(_tmp6DD,
sizeof(char),47))),_tag_dyneither(_tmp6DC,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL184;_LL18D:;_LL18E: goto _LL184;_LL184:;}if(bad_type){const char*_tmp6E1;
void*_tmp6E0[1];struct Cyc_String_pa_struct _tmp6DF;(_tmp6DF.tag=0,((_tmp6DF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6E0[
0]=& _tmp6DF,Cyc_Tcutil_terr(loc,((_tmp6E1="%s is not a known type",
_tag_dyneither(_tmp6E1,sizeof(char),23))),_tag_dyneither(_tmp6E0,sizeof(void*),1)))))));}
goto _LL178;}_LL178:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e);static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){struct _RegionHandle*_tmp276=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp277=Cyc_Tcenv_clear_notreadctxt(_tmp276,
te_orig);Cyc_Tcexp_tcExp(_tmp277,0,e);{void*t=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp278=t;struct Cyc_Absyn_PtrInfo
_tmp279;void*_tmp27A;struct Cyc_Absyn_PtrAtts _tmp27B;void*_tmp27C;struct Cyc_Absyn_Conref*
_tmp27D;struct Cyc_Absyn_Conref*_tmp27E;_LL190: if(_tmp278 <= (void*)4)goto _LL194;
if(*((int*)_tmp278)!= 0)goto _LL192;_LL191: {struct Cyc_List_List*_tmp27F=Cyc_Tcenv_lookup_type_vars(
_tmp277);struct Cyc_Core_Opt*_tmp6E2;void*_tmp280=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,((_tmp6E2=_cycalloc(sizeof(*_tmp6E2)),((_tmp6E2->v=_tmp27F,_tmp6E2)))));
struct Cyc_Core_Opt*_tmp6E3;void*_tmp281=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,((_tmp6E3=_cycalloc(sizeof(*_tmp6E3)),((_tmp6E3->v=_tmp27F,
_tmp6E3)))));struct Cyc_Absyn_Conref*_tmp282=Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*
_tmp283=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();struct Cyc_Absyn_PtrAtts
_tmp6E4;struct Cyc_Absyn_PtrAtts _tmp284=(_tmp6E4.rgn=(void*)_tmp281,((_tmp6E4.nullable=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((_tmp6E4.bounds=_tmp282,((
_tmp6E4.zero_term=_tmp283,((_tmp6E4.ptrloc=0,_tmp6E4)))))))));struct Cyc_Absyn_PointerType_struct
_tmp6EA;struct Cyc_Absyn_PtrInfo _tmp6E9;struct Cyc_Absyn_PointerType_struct*
_tmp6E8;struct Cyc_Absyn_PointerType_struct*_tmp285=(_tmp6E8=_cycalloc(sizeof(*
_tmp6E8)),((_tmp6E8[0]=((_tmp6EA.tag=4,((_tmp6EA.f1=((_tmp6E9.elt_typ=(void*)
_tmp280,((_tmp6E9.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp6E9.ptr_atts=_tmp284,
_tmp6E9)))))),_tmp6EA)))),_tmp6E8)));Cyc_Tcutil_unify(t,(void*)_tmp285);_tmp27A=
_tmp280;_tmp27C=_tmp281;_tmp27D=_tmp282;_tmp27E=_tmp283;goto _LL193;}_LL192: if(*((
int*)_tmp278)!= 4)goto _LL194;_tmp279=((struct Cyc_Absyn_PointerType_struct*)
_tmp278)->f1;_tmp27A=(void*)_tmp279.elt_typ;_tmp27B=_tmp279.ptr_atts;_tmp27C=(
void*)_tmp27B.rgn;_tmp27D=_tmp27B.bounds;_tmp27E=_tmp27B.zero_term;_LL193: Cyc_Tcenv_check_rgn_accessible(
_tmp277,loc,_tmp27C);Cyc_Tcutil_check_nonzero_bound(loc,_tmp27D);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp27A),(void*)1)){const char*_tmp6ED;void*_tmp6EC;(_tmp6EC=
0,Cyc_Tcutil_terr(loc,((_tmp6ED="can't dereference abstract pointer type",
_tag_dyneither(_tmp6ED,sizeof(char),40))),_tag_dyneither(_tmp6EC,sizeof(void*),0)));}
return _tmp27A;_LL194:;_LL195: {const char*_tmp6F1;void*_tmp6F0[1];struct Cyc_String_pa_struct
_tmp6EF;return(_tmp6EF.tag=0,((_tmp6EF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6F0[0]=& _tmp6EF,Cyc_Tcexp_expr_err(
_tmp277,loc,topt,((_tmp6F1="expecting *, @, or ? type but found %s",
_tag_dyneither(_tmp6F1,sizeof(char),39))),_tag_dyneither(_tmp6F0,sizeof(void*),1)))))));}
_LL18F:;}}static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f,int*is_tagged,int*is_read);static void*Cyc_Tcexp_tcAggrMember(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
Cyc_Tcexp_tcExpNoPromote(te,0,e);*is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*
_tmp291=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_AggrInfo _tmp292;union Cyc_Absyn_AggrInfoU_union _tmp293;struct Cyc_Absyn_Aggrdecl**
_tmp294;struct Cyc_Absyn_Aggrdecl*_tmp295;struct Cyc_List_List*_tmp296;void*
_tmp297;struct Cyc_List_List*_tmp298;_LL197: if(_tmp291 <= (void*)4)goto _LL19F;if(*((
int*)_tmp291)!= 10)goto _LL199;_tmp292=((struct Cyc_Absyn_AggrType_struct*)_tmp291)->f1;
_tmp293=_tmp292.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp291)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL199;_tmp294=(_tmp293.KnownAggr).f1;_tmp295=*_tmp294;_tmp296=_tmp292.targs;
_LL198: {struct Cyc_Absyn_Aggrfield*_tmp29B=Cyc_Absyn_lookup_decl_field(_tmp295,f);
if(_tmp29B == 0){const char*_tmp6F6;void*_tmp6F5[2];struct Cyc_String_pa_struct
_tmp6F4;struct Cyc_String_pa_struct _tmp6F3;return(_tmp6F3.tag=0,((_tmp6F3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6F4.tag=0,((_tmp6F4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp295->name)),((
_tmp6F5[0]=& _tmp6F4,((_tmp6F5[1]=& _tmp6F3,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp6F6="type %s has no %s field",_tag_dyneither(_tmp6F6,sizeof(char),24))),
_tag_dyneither(_tmp6F5,sizeof(void*),2)))))))))))));}if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp295->impl))->tagged)*is_tagged=1;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp295->impl))->exist_vars != 0){const char*_tmp6F9;void*_tmp6F8;
return(_tmp6F8=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6F9="must use pattern-matching to access fields of existential types",
_tag_dyneither(_tmp6F9,sizeof(char),64))),_tag_dyneither(_tmp6F8,sizeof(void*),0)));}{
void*t2;struct _RegionHandle*_tmp2A2=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
_tmp2A3=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp2A2,_tmp2A2,
_tmp295->tvs,_tmp296);t2=Cyc_Tcutil_rsubstitute(_tmp2A2,_tmp2A3,(void*)_tmp29B->type);
if((((void*)_tmp295->kind == (void*)1  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp295->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t2)){const char*_tmp6FD;void*_tmp6FC[1];struct Cyc_String_pa_struct _tmp6FB;return(
_tmp6FB.tag=0,((_tmp6FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp6FC[0]=& _tmp6FB,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6FD="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp6FD,sizeof(char),56))),_tag_dyneither(_tmp6FC,sizeof(void*),1)))))));}}
return t2;}}_LL199: if(*((int*)_tmp291)!= 11)goto _LL19B;_tmp297=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp291)->f1;_tmp298=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp291)->f2;_LL19A: {
struct Cyc_Absyn_Aggrfield*_tmp2A7=Cyc_Absyn_lookup_field(_tmp298,f);if(_tmp2A7 == 
0){const char*_tmp701;void*_tmp700[1];struct Cyc_String_pa_struct _tmp6FF;return(
_tmp6FF.tag=0,((_tmp6FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp700[0]=& _tmp6FF,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp701="type has no %s field",
_tag_dyneither(_tmp701,sizeof(char),21))),_tag_dyneither(_tmp700,sizeof(void*),1)))))));}
if((_tmp297 == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp2A7->type)){const char*_tmp705;void*_tmp704[1];struct Cyc_String_pa_struct
_tmp703;return(_tmp703.tag=0,((_tmp703.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp704[0]=& _tmp703,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp705="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp705,sizeof(char),56))),_tag_dyneither(_tmp704,sizeof(void*),1)))))));}return(
void*)_tmp2A7->type;}_LL19B: if(*((int*)_tmp291)!= 7)goto _LL19D;{const char*
_tmp706;if(!(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp706="size",_tag_dyneither(
_tmp706,sizeof(char),5))))== 0))goto _LL19D;}_LL19C: goto _LL19E;_LL19D: if(*((int*)
_tmp291)!= 4)goto _LL19F;{const char*_tmp707;if(!(Cyc_strcmp((struct _dyneither_ptr)*
f,((_tmp707="size",_tag_dyneither(_tmp707,sizeof(char),5))))== 0))goto _LL19F;}
_LL19E:{const char*_tmp70B;void*_tmp70A[1];struct Cyc_String_pa_struct _tmp709;(
_tmp709.tag=0,((_tmp709.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp70A[0]=& _tmp709,Cyc_Tcutil_warn(e->loc,((_tmp70B="deprecated `.size' used here -- change to numelts(%s)",
_tag_dyneither(_tmp70B,sizeof(char),54))),_tag_dyneither(_tmp70A,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Primop_e_struct _tmp711;struct Cyc_List_List*_tmp710;struct Cyc_Absyn_Primop_e_struct*
_tmp70F;(void*)(outer_e->r=(void*)((void*)((_tmp70F=_cycalloc(sizeof(*_tmp70F)),((
_tmp70F[0]=((_tmp711.tag=3,((_tmp711.f1=(void*)((void*)19),((_tmp711.f2=((
_tmp710=_cycalloc(sizeof(*_tmp710)),((_tmp710->hd=e,((_tmp710->tl=0,_tmp710)))))),
_tmp711)))))),_tmp70F))))));}return Cyc_Absyn_uint_typ;_LL19F:;_LL1A0: {const char*
_tmp712;if(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp712="size",_tag_dyneither(
_tmp712,sizeof(char),5))))== 0){const char*_tmp716;void*_tmp715[1];struct Cyc_String_pa_struct
_tmp714;return(_tmp714.tag=0,((_tmp714.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp715[0]=& _tmp714,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp716="expecting struct, union, or array, found %s",
_tag_dyneither(_tmp716,sizeof(char),44))),_tag_dyneither(_tmp715,sizeof(void*),1)))))));}
else{const char*_tmp71A;void*_tmp719[1];struct Cyc_String_pa_struct _tmp718;return(
_tmp718.tag=0,((_tmp718.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp719[0]=& _tmp718,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp71A="expecting struct or union, found %s",_tag_dyneither(
_tmp71A,sizeof(char),36))),_tag_dyneither(_tmp719,sizeof(void*),1)))))));}}
_LL196:;}}static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*
is_read);static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*
is_read){struct _RegionHandle*_tmp2BB=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(
_tmp2BB,te),0,e);*is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*_tmp2BC=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp2BD;void*_tmp2BE;struct Cyc_Absyn_PtrAtts _tmp2BF;void*_tmp2C0;struct Cyc_Absyn_Conref*
_tmp2C1;struct Cyc_Absyn_Conref*_tmp2C2;_LL1A2: if(_tmp2BC <= (void*)4)goto _LL1A4;
if(*((int*)_tmp2BC)!= 4)goto _LL1A4;_tmp2BD=((struct Cyc_Absyn_PointerType_struct*)
_tmp2BC)->f1;_tmp2BE=(void*)_tmp2BD.elt_typ;_tmp2BF=_tmp2BD.ptr_atts;_tmp2C0=(
void*)_tmp2BF.rgn;_tmp2C1=_tmp2BF.bounds;_tmp2C2=_tmp2BF.zero_term;_LL1A3: Cyc_Tcutil_check_nonzero_bound(
loc,_tmp2C1);{void*_tmp2C3=Cyc_Tcutil_compress(_tmp2BE);struct Cyc_Absyn_AggrInfo
_tmp2C4;union Cyc_Absyn_AggrInfoU_union _tmp2C5;struct Cyc_Absyn_Aggrdecl**_tmp2C6;
struct Cyc_Absyn_Aggrdecl*_tmp2C7;struct Cyc_List_List*_tmp2C8;void*_tmp2C9;struct
Cyc_List_List*_tmp2CA;_LL1A7: if(_tmp2C3 <= (void*)4)goto _LL1AB;if(*((int*)_tmp2C3)
!= 10)goto _LL1A9;_tmp2C4=((struct Cyc_Absyn_AggrType_struct*)_tmp2C3)->f1;_tmp2C5=
_tmp2C4.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp2C3)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL1A9;_tmp2C6=(_tmp2C5.KnownAggr).f1;_tmp2C7=*_tmp2C6;_tmp2C8=_tmp2C4.targs;
_LL1A8: {struct Cyc_Absyn_Aggrfield*_tmp2CB=Cyc_Absyn_lookup_decl_field(_tmp2C7,f);
if(_tmp2CB == 0){const char*_tmp71F;void*_tmp71E[2];struct Cyc_String_pa_struct
_tmp71D;struct Cyc_String_pa_struct _tmp71C;return(_tmp71C.tag=0,((_tmp71C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp71D.tag=0,((_tmp71D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2C7->name)),((
_tmp71E[0]=& _tmp71D,((_tmp71E[1]=& _tmp71C,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp71F="type %s has no %s field",_tag_dyneither(_tmp71F,sizeof(char),24))),
_tag_dyneither(_tmp71E,sizeof(void*),2)))))))))))));}if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp2C7->impl))->tagged)*is_tagged=1;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp2C7->impl))->exist_vars != 0){const char*_tmp722;void*_tmp721;
return(_tmp721=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp722="must use pattern-matching to access fields of existential types",
_tag_dyneither(_tmp722,sizeof(char),64))),_tag_dyneither(_tmp721,sizeof(void*),0)));}{
void*t3;struct _RegionHandle*_tmp2D2=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
_tmp2D3=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp2D2,_tmp2D2,
_tmp2C7->tvs,_tmp2C8);t3=Cyc_Tcutil_rsubstitute(_tmp2D2,_tmp2D3,(void*)_tmp2CB->type);}
if((((void*)_tmp2C7->kind == (void*)1  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp2C7->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t3)){const char*_tmp726;void*_tmp725[1];struct Cyc_String_pa_struct _tmp724;return(
_tmp724.tag=0,((_tmp724.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp725[0]=& _tmp724,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp726="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp726,sizeof(char),56))),_tag_dyneither(_tmp725,sizeof(void*),1)))))));}
return t3;}}_LL1A9: if(*((int*)_tmp2C3)!= 11)goto _LL1AB;_tmp2C9=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2C3)->f1;_tmp2CA=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2C3)->f2;_LL1AA: {
struct Cyc_Absyn_Aggrfield*_tmp2D7=Cyc_Absyn_lookup_field(_tmp2CA,f);if(_tmp2D7 == 
0){const char*_tmp72A;void*_tmp729[1];struct Cyc_String_pa_struct _tmp728;return(
_tmp728.tag=0,((_tmp728.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp729[0]=& _tmp728,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp72A="type has no %s field",
_tag_dyneither(_tmp72A,sizeof(char),21))),_tag_dyneither(_tmp729,sizeof(void*),1)))))));}
if((_tmp2C9 == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp2D7->type)){const char*_tmp72E;void*_tmp72D[1];struct Cyc_String_pa_struct
_tmp72C;return(_tmp72C.tag=0,((_tmp72C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp72D[0]=& _tmp72C,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp72E="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp72E,sizeof(char),56))),_tag_dyneither(_tmp72D,sizeof(void*),1)))))));}return(
void*)_tmp2D7->type;}_LL1AB:;_LL1AC: goto _LL1A6;_LL1A6:;}goto _LL1A1;_LL1A4:;
_LL1A5: goto _LL1A1;_LL1A1:;}{const char*_tmp732;void*_tmp731[1];struct Cyc_String_pa_struct
_tmp730;return(_tmp730.tag=0,((_tmp730.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp731[0]=& _tmp730,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp732="expecting struct or union pointer, found %s",
_tag_dyneither(_tmp732,sizeof(char),44))),_tag_dyneither(_tmp731,sizeof(void*),1)))))));}}
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index);static void*Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,
struct Cyc_Absyn_Exp*index){unsigned int _tmp2E2;int _tmp2E3;struct _tuple7 _tmp2E1=
Cyc_Evexp_eval_const_uint_exp(index);_tmp2E2=_tmp2E1.f1;_tmp2E3=_tmp2E1.f2;if(!
_tmp2E3){const char*_tmp735;void*_tmp734;return(_tmp734=0,Cyc_Tcexp_expr_err(te,
loc,0,((_tmp735="tuple projection cannot use sizeof or offsetof",_tag_dyneither(
_tmp735,sizeof(char),47))),_tag_dyneither(_tmp734,sizeof(void*),0)));}{struct
_handler_cons _tmp2E6;_push_handler(& _tmp2E6);{int _tmp2E8=0;if(setjmp(_tmp2E6.handler))
_tmp2E8=1;if(!_tmp2E8){{void*_tmp2E9=(*((struct _tuple9*(*)(struct Cyc_List_List*x,
int n))Cyc_List_nth)(ts,(int)_tmp2E2)).f2;_npop_handler(0);return _tmp2E9;};
_pop_handler();}else{void*_tmp2E7=(void*)_exn_thrown;void*_tmp2EB=_tmp2E7;_LL1AE:
if(_tmp2EB != Cyc_List_Nth)goto _LL1B0;_LL1AF: {const char*_tmp73A;void*_tmp739[2];
struct Cyc_Int_pa_struct _tmp738;struct Cyc_Int_pa_struct _tmp737;return(_tmp737.tag=
1,((_tmp737.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
ts),((_tmp738.tag=1,((_tmp738.f1=(unsigned long)((int)_tmp2E2),((_tmp739[0]=&
_tmp738,((_tmp739[1]=& _tmp737,Cyc_Tcexp_expr_err(te,loc,0,((_tmp73A="index is %d but tuple has only %d fields",
_tag_dyneither(_tmp73A,sizeof(char),41))),_tag_dyneither(_tmp739,sizeof(void*),2)))))))))))));}
_LL1B0:;_LL1B1:(void)_throw(_tmp2EB);_LL1AD:;}}}}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp2F0=Cyc_Tcenv_get_fnrgn(te_orig);
struct Cyc_Tcenv_Tenv*_tmp2F1=Cyc_Tcenv_clear_notreadctxt(_tmp2F0,te_orig);Cyc_Tcexp_tcExp(
_tmp2F1,0,e1);Cyc_Tcexp_tcExp(_tmp2F1,0,e2);{void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!Cyc_Tcutil_coerce_sint_typ(
_tmp2F1,e2)){const char*_tmp73E;void*_tmp73D[1];struct Cyc_String_pa_struct _tmp73C;
return(_tmp73C.tag=0,((_tmp73C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp73D[0]=& _tmp73C,Cyc_Tcexp_expr_err(_tmp2F1,e2->loc,
topt,((_tmp73E="expecting int subscript, found %s",_tag_dyneither(_tmp73E,
sizeof(char),34))),_tag_dyneither(_tmp73D,sizeof(void*),1)))))));}{void*_tmp2F5=
t1;struct Cyc_Absyn_PtrInfo _tmp2F6;void*_tmp2F7;struct Cyc_Absyn_Tqual _tmp2F8;
struct Cyc_Absyn_PtrAtts _tmp2F9;void*_tmp2FA;struct Cyc_Absyn_Conref*_tmp2FB;
struct Cyc_Absyn_Conref*_tmp2FC;struct Cyc_List_List*_tmp2FD;_LL1B3: if(_tmp2F5 <= (
void*)4)goto _LL1B7;if(*((int*)_tmp2F5)!= 4)goto _LL1B5;_tmp2F6=((struct Cyc_Absyn_PointerType_struct*)
_tmp2F5)->f1;_tmp2F7=(void*)_tmp2F6.elt_typ;_tmp2F8=_tmp2F6.elt_tq;_tmp2F9=
_tmp2F6.ptr_atts;_tmp2FA=(void*)_tmp2F9.rgn;_tmp2FB=_tmp2F9.bounds;_tmp2FC=
_tmp2F9.zero_term;_LL1B4: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp2FC)){int emit_warning=1;{union Cyc_Absyn_Constraint_union _tmp2FE=(Cyc_Absyn_compress_conref(
_tmp2FB))->v;void*_tmp2FF;struct Cyc_Absyn_Exp*_tmp300;void*_tmp301;_LL1BA: if((
_tmp2FE.Eq_constr).tag != 0)goto _LL1BC;_tmp2FF=(_tmp2FE.Eq_constr).f1;if(_tmp2FF
<= (void*)1)goto _LL1BC;if(*((int*)_tmp2FF)!= 0)goto _LL1BC;_tmp300=((struct Cyc_Absyn_Upper_b_struct*)
_tmp2FF)->f1;_LL1BB: if(Cyc_Tcutil_is_const_exp(_tmp2F1,e2)){unsigned int _tmp303;
int _tmp304;struct _tuple7 _tmp302=Cyc_Evexp_eval_const_uint_exp(e2);_tmp303=
_tmp302.f1;_tmp304=_tmp302.f2;if(_tmp304){unsigned int _tmp306;int _tmp307;struct
_tuple7 _tmp305=Cyc_Evexp_eval_const_uint_exp(_tmp300);_tmp306=_tmp305.f1;_tmp307=
_tmp305.f2;if(_tmp307  && _tmp306 > _tmp303)emit_warning=0;}}goto _LL1B9;_LL1BC: if((
_tmp2FE.Eq_constr).tag != 0)goto _LL1BE;_tmp301=(_tmp2FE.Eq_constr).f1;if((int)
_tmp301 != 0)goto _LL1BE;_LL1BD: emit_warning=0;goto _LL1B9;_LL1BE:;_LL1BF: goto
_LL1B9;_LL1B9:;}if(emit_warning){const char*_tmp741;void*_tmp740;(_tmp740=0,Cyc_Tcutil_warn(
e2->loc,((_tmp741="subscript on thin, zero-terminated pointer could be expensive.",
_tag_dyneither(_tmp741,sizeof(char),63))),_tag_dyneither(_tmp740,sizeof(void*),0)));}}
else{if(Cyc_Tcutil_is_const_exp(_tmp2F1,e2)){unsigned int _tmp30B;int _tmp30C;
struct _tuple7 _tmp30A=Cyc_Evexp_eval_const_uint_exp(e2);_tmp30B=_tmp30A.f1;
_tmp30C=_tmp30A.f2;if(_tmp30C)Cyc_Tcutil_check_bound(loc,_tmp30B,_tmp2FB);}else{
if(Cyc_Tcutil_is_bound_one(_tmp2FB) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp2FC)){const char*_tmp744;void*_tmp743;(_tmp743=0,Cyc_Tcutil_warn(
e1->loc,((_tmp744="subscript applied to pointer to one object",_tag_dyneither(
_tmp744,sizeof(char),43))),_tag_dyneither(_tmp743,sizeof(void*),0)));}Cyc_Tcutil_check_nonzero_bound(
loc,_tmp2FB);}}Cyc_Tcenv_check_rgn_accessible(_tmp2F1,loc,_tmp2FA);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp2F7),(void*)1)){const char*_tmp747;void*_tmp746;(_tmp746=
0,Cyc_Tcutil_terr(e1->loc,((_tmp747="can't subscript an abstract pointer",
_tag_dyneither(_tmp747,sizeof(char),36))),_tag_dyneither(_tmp746,sizeof(void*),0)));}
return _tmp2F7;_LL1B5: if(*((int*)_tmp2F5)!= 9)goto _LL1B7;_tmp2FD=((struct Cyc_Absyn_TupleType_struct*)
_tmp2F5)->f1;_LL1B6: return Cyc_Tcexp_ithTupleType(_tmp2F1,loc,_tmp2FD,e2);_LL1B7:;
_LL1B8: {const char*_tmp74B;void*_tmp74A[1];struct Cyc_String_pa_struct _tmp749;
return(_tmp749.tag=0,((_tmp749.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp74A[0]=& _tmp749,Cyc_Tcexp_expr_err(_tmp2F1,loc,
topt,((_tmp74B="subscript applied to %s",_tag_dyneither(_tmp74B,sizeof(char),24))),
_tag_dyneither(_tmp74A,sizeof(void*),1)))))));}_LL1B2:;}}}static void*Cyc_Tcexp_tcTuple(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_List_List*
es);static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp314=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp315;
_LL1C1: if(_tmp314 <= (void*)4)goto _LL1C3;if(*((int*)_tmp314)!= 9)goto _LL1C3;
_tmp315=((struct Cyc_Absyn_TupleType_struct*)_tmp314)->f1;_LL1C2: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp315)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1C0;for(0;es != 0;(es=es->tl,_tmp315=_tmp315->tl)){
void*_tmp316=(*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp315))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp316,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple9*)_tmp315->hd)).f2);{struct
_tuple9*_tmp74E;struct Cyc_List_List*_tmp74D;fields=((_tmp74D=_cycalloc(sizeof(*
_tmp74D)),((_tmp74D->hd=((_tmp74E=_cycalloc(sizeof(*_tmp74E)),((_tmp74E->f1=(*((
struct _tuple9*)_tmp315->hd)).f1,((_tmp74E->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,_tmp74E)))))),((_tmp74D->tl=
fields,_tmp74D))))));}}done=1;goto _LL1C0;_LL1C3:;_LL1C4: goto _LL1C0;_LL1C0:;}if(!
done)for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)
es->hd);{struct _tuple9*_tmp751;struct Cyc_List_List*_tmp750;fields=((_tmp750=
_cycalloc(sizeof(*_tmp750)),((_tmp750->hd=((_tmp751=_cycalloc(sizeof(*_tmp751)),((
_tmp751->f1=Cyc_Absyn_empty_tqual(0),((_tmp751->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,_tmp751)))))),((_tmp750->tl=
fields,_tmp750))))));}}{struct Cyc_Absyn_TupleType_struct _tmp754;struct Cyc_Absyn_TupleType_struct*
_tmp753;return(void*)((_tmp753=_cycalloc(sizeof(*_tmp753)),((_tmp753[0]=((
_tmp754.tag=9,((_tmp754.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmp754)))),_tmp753))));}}static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*t,struct Cyc_List_List*
des);static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _tuple2*t,struct Cyc_List_List*des){const char*_tmp757;void*
_tmp756;return(_tmp756=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp757="tcCompoundLit",
_tag_dyneither(_tmp757,sizeof(char),14))),_tag_dyneither(_tmp756,sizeof(void*),0)));}
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,int zero_term,struct Cyc_List_List*des);static void*Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**elt_topt,int
zero_term,struct Cyc_List_List*des){void*res_t2;struct _RegionHandle*_tmp31F=Cyc_Tcenv_get_fnrgn(
te);{int _tmp320=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple8*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp31F,(struct Cyc_Absyn_Exp*(*)(
struct _tuple8*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Const_e_struct
_tmp75D;union Cyc_Absyn_Cnst_union _tmp75C;struct Cyc_Absyn_Const_e_struct*_tmp75B;
struct Cyc_Absyn_Const_e_struct*_tmp321=(_tmp75B=_cycalloc(sizeof(*_tmp75B)),((
_tmp75B[0]=((_tmp75D.tag=0,((_tmp75D.f1=(union Cyc_Absyn_Cnst_union)(((_tmp75C.Int_c).tag=
2,(((_tmp75C.Int_c).f1=(void*)((void*)1),(((_tmp75C.Int_c).f2=_tmp320,_tmp75C)))))),
_tmp75D)))),_tmp75B)));struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)
_tmp321,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp322=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp320 - 1);if(!Cyc_Tcutil_is_zero(
_tmp322)){const char*_tmp760;void*_tmp75F;(_tmp75F=0,Cyc_Tcutil_terr(_tmp322->loc,((
_tmp760="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp760,
sizeof(char),45))),_tag_dyneither(_tmp75F,sizeof(void*),0)));}}{struct Cyc_Core_Opt*
_tmp761;sz_exp->topt=((_tmp761=_cycalloc(sizeof(*_tmp761)),((_tmp761->v=(void*)
Cyc_Absyn_uint_typ,_tmp761))));}res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(
0),(struct Cyc_Absyn_Exp*)sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,
0);{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(
te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,
es)){const char*_tmp765;void*_tmp764[1];struct Cyc_String_pa_struct _tmp763;(
_tmp763.tag=0,((_tmp763.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
res)),((_tmp764[0]=& _tmp763,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,((_tmp765="elements of array do not all have the same type (%s)",
_tag_dyneither(_tmp765,sizeof(char),53))),_tag_dyneither(_tmp764,sizeof(void*),1)))))));}}{
int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((
struct _tuple8*)des->hd)).f1;if(ds != 0){void*_tmp32C=(void*)ds->hd;struct Cyc_Absyn_Exp*
_tmp32D;_LL1C6: if(*((int*)_tmp32C)!= 1)goto _LL1C8;_LL1C7:{const char*_tmp768;void*
_tmp767;(_tmp767=0,Cyc_Tcutil_terr(loc,((_tmp768="only array index designators are supported",
_tag_dyneither(_tmp768,sizeof(char),43))),_tag_dyneither(_tmp767,sizeof(void*),0)));}
goto _LL1C5;_LL1C8: if(*((int*)_tmp32C)!= 0)goto _LL1C5;_tmp32D=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp32C)->f1;_LL1C9: Cyc_Tcexp_tcExpInitializer(te,0,_tmp32D);{unsigned int
_tmp331;int _tmp332;struct _tuple7 _tmp330=Cyc_Evexp_eval_const_uint_exp(_tmp32D);
_tmp331=_tmp330.f1;_tmp332=_tmp330.f2;if(!_tmp332){const char*_tmp76B;void*
_tmp76A;(_tmp76A=0,Cyc_Tcutil_terr(_tmp32D->loc,((_tmp76B="index designator cannot use sizeof or offsetof",
_tag_dyneither(_tmp76B,sizeof(char),47))),_tag_dyneither(_tmp76A,sizeof(void*),0)));}
else{if(_tmp331 != offset){const char*_tmp770;void*_tmp76F[2];struct Cyc_Int_pa_struct
_tmp76E;struct Cyc_Int_pa_struct _tmp76D;(_tmp76D.tag=1,((_tmp76D.f1=(
unsigned long)((int)_tmp331),((_tmp76E.tag=1,((_tmp76E.f1=(unsigned long)offset,((
_tmp76F[0]=& _tmp76E,((_tmp76F[1]=& _tmp76D,Cyc_Tcutil_terr(_tmp32D->loc,((_tmp770="expecting index designator %d but found %d",
_tag_dyneither(_tmp770,sizeof(char),43))),_tag_dyneither(_tmp76F,sizeof(void*),2)))))))))))));}}
goto _LL1C5;}_LL1C5:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term);static
void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*
body,int*is_zero_term){Cyc_Tcexp_tcExp(te,0,bound);{void*_tmp339=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(bound->topt))->v);_LL1CB: if(_tmp339 <= (
void*)4)goto _LL1CD;if(*((int*)_tmp339)!= 18)goto _LL1CD;_LL1CC: goto _LL1CA;_LL1CD:;
_LL1CE: if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){const char*_tmp774;void*_tmp773[
1];struct Cyc_String_pa_struct _tmp772;(_tmp772.tag=0,((_tmp772.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v)),((_tmp773[0]=& _tmp772,Cyc_Tcutil_terr(bound->loc,((
_tmp774="expecting unsigned int, found %s",_tag_dyneither(_tmp774,sizeof(char),
33))),_tag_dyneither(_tmp773,sizeof(void*),1)))))));}_LL1CA:;}if(!(vd->tq).real_const){
const char*_tmp777;void*_tmp776;(_tmp776=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp777="comprehension index variable is not declared const!",
_tag_dyneither(_tmp777,sizeof(char),52))),_tag_dyneither(_tmp776,sizeof(void*),0)));}{
struct _RegionHandle*_tmp33F=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(
_tmp33F,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(_tmp33F,loc,te2);te2=Cyc_Tcenv_add_local_var(
_tmp33F,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*_tmp340=te2;struct Cyc_Absyn_PtrInfo
pinfo;void**_tmp341=0;struct Cyc_Absyn_Tqual*_tmp342=0;struct Cyc_Absyn_Conref**
_tmp343=0;if(topt != 0){void*_tmp344=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp345;struct Cyc_Absyn_ArrayInfo _tmp346;void*_tmp347;void**_tmp348;struct Cyc_Absyn_Tqual
_tmp349;struct Cyc_Absyn_Tqual*_tmp34A;struct Cyc_Absyn_Exp*_tmp34B;struct Cyc_Absyn_Conref*
_tmp34C;struct Cyc_Absyn_Conref**_tmp34D;_LL1D0: if(_tmp344 <= (void*)4)goto _LL1D4;
if(*((int*)_tmp344)!= 4)goto _LL1D2;_tmp345=((struct Cyc_Absyn_PointerType_struct*)
_tmp344)->f1;_LL1D1: pinfo=_tmp345;_tmp341=(void**)((void**)((void*)& pinfo.elt_typ));
_tmp342=(struct Cyc_Absyn_Tqual*)& pinfo.elt_tq;_tmp343=(struct Cyc_Absyn_Conref**)&(
pinfo.ptr_atts).zero_term;goto _LL1CF;_LL1D2: if(*((int*)_tmp344)!= 7)goto _LL1D4;
_tmp346=((struct Cyc_Absyn_ArrayType_struct*)_tmp344)->f1;_tmp347=(void*)_tmp346.elt_type;
_tmp348=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp344)->f1).elt_type;
_tmp349=_tmp346.tq;_tmp34A=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp344)->f1).tq;_tmp34B=_tmp346.num_elts;_tmp34C=_tmp346.zero_term;_tmp34D=(
struct Cyc_Absyn_Conref**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp344)->f1).zero_term;
_LL1D3: _tmp341=(void**)_tmp348;_tmp342=(struct Cyc_Absyn_Tqual*)_tmp34A;_tmp343=(
struct Cyc_Absyn_Conref**)_tmp34D;goto _LL1CF;_LL1D4:;_LL1D5: goto _LL1CF;_LL1CF:;}{
void*t=Cyc_Tcexp_tcExp(_tmp340,_tmp341,body);if(_tmp340->le == 0){if(!Cyc_Tcutil_is_const_exp(
_tmp340,bound)){const char*_tmp77A;void*_tmp779;(_tmp779=0,Cyc_Tcutil_terr(bound->loc,((
_tmp77A="bound is not constant",_tag_dyneither(_tmp77A,sizeof(char),22))),
_tag_dyneither(_tmp779,sizeof(void*),0)));}if(!Cyc_Tcutil_is_const_exp(_tmp340,
body)){const char*_tmp77D;void*_tmp77C;(_tmp77C=0,Cyc_Tcutil_terr(bound->loc,((
_tmp77D="body is not constant",_tag_dyneither(_tmp77D,sizeof(char),21))),
_tag_dyneither(_tmp77C,sizeof(void*),0)));}}if(_tmp343 != 0  && ((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,*_tmp343)){struct Cyc_Absyn_Exp*
_tmp352=Cyc_Absyn_uint_exp(1,0);{struct Cyc_Core_Opt*_tmp77E;_tmp352->topt=((
_tmp77E=_cycalloc(sizeof(*_tmp77E)),((_tmp77E->v=(void*)Cyc_Absyn_uint_typ,
_tmp77E))));}bound=Cyc_Absyn_add_exp(bound,_tmp352,0);{struct Cyc_Core_Opt*
_tmp77F;bound->topt=((_tmp77F=_cycalloc(sizeof(*_tmp77F)),((_tmp77F->v=(void*)
Cyc_Absyn_uint_typ,_tmp77F))));}*is_zero_term=1;}{void*_tmp355=Cyc_Absyn_array_typ(
t,_tmp342 == 0?Cyc_Absyn_empty_tqual(0):*_tmp342,(struct Cyc_Absyn_Exp*)bound,
_tmp343 == 0?Cyc_Absyn_false_conref:*_tmp343,0);return _tmp355;}}}}}struct _tuple10{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcAggregate(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**
tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**
ad_opt);static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*
args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*ad;if(*ad_opt != 
0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{struct _handler_cons
_tmp356;_push_handler(& _tmp356);{int _tmp358=0;if(setjmp(_tmp356.handler))_tmp358=
1;if(!_tmp358){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;_pop_handler();}else{
void*_tmp357=(void*)_exn_thrown;void*_tmp35A=_tmp357;_LL1D7: if(_tmp35A != Cyc_Dict_Absent)
goto _LL1D9;_LL1D8:{const char*_tmp783;void*_tmp782[1];struct Cyc_String_pa_struct
_tmp781;(_tmp781.tag=0,((_tmp781.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(*tn)),((_tmp782[0]=& _tmp781,Cyc_Tcutil_terr(loc,((
_tmp783="unbound struct/union name %s",_tag_dyneither(_tmp783,sizeof(char),29))),
_tag_dyneither(_tmp782,sizeof(void*),1)))))));}return topt != 0?*topt:(void*)0;
_LL1D9:;_LL1DA:(void)_throw(_tmp35A);_LL1D6:;}}}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)
ad;*tn=ad->name;}if(ad->impl == 0){{const char*_tmp78B;const char*_tmp78A;const char*
_tmp789;void*_tmp788[1];struct Cyc_String_pa_struct _tmp787;(_tmp787.tag=0,((
_tmp787.f1=(struct _dyneither_ptr)((void*)ad->kind == (void*)0?(_tmp78A="struct",
_tag_dyneither(_tmp78A,sizeof(char),7)):((_tmp78B="union",_tag_dyneither(_tmp78B,
sizeof(char),6)))),((_tmp788[0]=& _tmp787,Cyc_Tcutil_terr(loc,((_tmp789="can't construct abstract %s",
_tag_dyneither(_tmp789,sizeof(char),28))),_tag_dyneither(_tmp788,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{struct
_RegionHandle*_tmp363=Cyc_Tcenv_get_fnrgn(te);struct _tuple4 _tmp78C;struct _tuple4
_tmp364=(_tmp78C.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp78C.f2=_tmp363,_tmp78C)));
struct Cyc_List_List*_tmp365=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp363,Cyc_Tcutil_r_make_inst_var,&
_tmp364,ad->tvs);struct Cyc_List_List*_tmp366=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp363,Cyc_Tcutil_r_make_inst_var,&
_tmp364,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp367=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp365);struct Cyc_List_List*_tmp368=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp366);struct Cyc_List_List*_tmp369=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp363,_tmp365,_tmp366);struct Cyc_Absyn_AggrType_struct _tmp79B;union Cyc_Absyn_AggrInfoU_union
_tmp79A;struct Cyc_Absyn_Aggrdecl**_tmp799;struct Cyc_Absyn_AggrInfo _tmp798;struct
Cyc_Absyn_AggrType_struct*_tmp797;struct Cyc_Absyn_AggrType_struct*_tmp36A=(
_tmp797=_cycalloc(sizeof(*_tmp797)),((_tmp797[0]=((_tmp79B.tag=10,((_tmp79B.f1=((
_tmp798.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((_tmp79A.KnownAggr).tag=1,(((
_tmp79A.KnownAggr).f1=((_tmp799=_cycalloc(sizeof(*_tmp799)),((_tmp799[0]=ad,
_tmp799)))),_tmp79A)))),((_tmp798.targs=_tmp367,_tmp798)))),_tmp79B)))),_tmp797)));
struct Cyc_List_List*_tmp36B=*ts;struct Cyc_List_List*_tmp36C=_tmp368;while(
_tmp36B != 0  && _tmp36C != 0){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)0,1,(void*)_tmp36B->hd);Cyc_Tcutil_unify((void*)_tmp36B->hd,(void*)
_tmp36C->hd);_tmp36B=_tmp36B->tl;_tmp36C=_tmp36C->tl;}if(_tmp36B != 0){const char*
_tmp79E;void*_tmp79D;(_tmp79D=0,Cyc_Tcutil_terr(loc,((_tmp79E="too many explicit witness types",
_tag_dyneither(_tmp79E,sizeof(char),32))),_tag_dyneither(_tmp79D,sizeof(void*),0)));}*
ts=_tmp368;if(topt != 0)Cyc_Tcutil_unify((void*)_tmp36A,*topt);{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,void*,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp363,loc,args,(void*)ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){struct _tuple10 _tmp370;struct Cyc_Absyn_Aggrfield*
_tmp371;struct Cyc_Absyn_Exp*_tmp372;struct _tuple10*_tmp36F=(struct _tuple10*)
fields->hd;_tmp370=*_tmp36F;_tmp371=_tmp370.f1;_tmp372=_tmp370.f2;{void*_tmp373=
Cyc_Tcutil_rsubstitute(_tmp363,_tmp369,(void*)_tmp371->type);Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp373,_tmp372);if(!Cyc_Tcutil_coerce_arg(te,_tmp372,_tmp373)){{
const char*_tmp7AA;const char*_tmp7A9;const char*_tmp7A8;void*_tmp7A7[5];struct Cyc_String_pa_struct
_tmp7A6;struct Cyc_String_pa_struct _tmp7A5;struct Cyc_String_pa_struct _tmp7A4;
struct Cyc_String_pa_struct _tmp7A3;struct Cyc_String_pa_struct _tmp7A2;(_tmp7A2.tag=
0,((_tmp7A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp372->topt))->v)),((_tmp7A3.tag=0,((
_tmp7A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp373)),((_tmp7A4.tag=0,((_tmp7A4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp7A5.tag=0,((_tmp7A5.f1=(
struct _dyneither_ptr)((void*)ad->kind == (void*)0?(_tmp7A9="struct",
_tag_dyneither(_tmp7A9,sizeof(char),7)):((_tmp7AA="union",_tag_dyneither(_tmp7AA,
sizeof(char),6)))),((_tmp7A6.tag=0,((_tmp7A6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp371->name),((_tmp7A7[0]=& _tmp7A6,((_tmp7A7[1]=& _tmp7A5,((
_tmp7A7[2]=& _tmp7A4,((_tmp7A7[3]=& _tmp7A3,((_tmp7A7[4]=& _tmp7A2,Cyc_Tcutil_terr(
_tmp372->loc,((_tmp7A8="field %s of %s %s expects type %s != %s",_tag_dyneither(
_tmp7A8,sizeof(char),40))),_tag_dyneither(_tmp7A7,sizeof(void*),5)))))))))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}}{struct Cyc_List_List*_tmp37D=0;{struct Cyc_List_List*
_tmp37E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;
_tmp37E != 0;_tmp37E=_tmp37E->tl){struct _tuple0*_tmp7AD;struct Cyc_List_List*
_tmp7AC;_tmp37D=((_tmp7AC=_cycalloc(sizeof(*_tmp7AC)),((_tmp7AC->hd=((_tmp7AD=
_cycalloc(sizeof(*_tmp7AD)),((_tmp7AD->f1=Cyc_Tcutil_rsubstitute(_tmp363,_tmp369,(*((
struct _tuple0*)_tmp37E->hd)).f1),((_tmp7AD->f2=Cyc_Tcutil_rsubstitute(_tmp363,
_tmp369,(*((struct _tuple0*)_tmp37E->hd)).f2),_tmp7AD)))))),((_tmp7AC->tl=_tmp37D,
_tmp7AC))))));}}_tmp37D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp37D);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp37D);return(void*)_tmp36A;}}}}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args);static void*Cyc_Tcexp_tcAnonStruct(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*
args){struct _RegionHandle*_tmp387=Cyc_Tcenv_get_fnrgn(te);{void*_tmp388=Cyc_Tcutil_compress(
ts);void*_tmp389;struct Cyc_List_List*_tmp38A;_LL1DC: if(_tmp388 <= (void*)4)goto
_LL1DE;if(*((int*)_tmp388)!= 11)goto _LL1DE;_tmp389=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp388)->f1;_tmp38A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp388)->f2;_LL1DD:
if(_tmp389 == (void*)1){const char*_tmp7B0;void*_tmp7AF;(_tmp7AF=0,Cyc_Tcutil_terr(
loc,((_tmp7B0="expecting struct but found union",_tag_dyneither(_tmp7B0,sizeof(
char),33))),_tag_dyneither(_tmp7AF,sizeof(void*),0)));}{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,void*,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp387,loc,args,(void*)0,_tmp38A);for(0;fields != 0;fields=fields->tl){struct
_tuple10 _tmp38E;struct Cyc_Absyn_Aggrfield*_tmp38F;struct Cyc_Absyn_Exp*_tmp390;
struct _tuple10*_tmp38D=(struct _tuple10*)fields->hd;_tmp38E=*_tmp38D;_tmp38F=
_tmp38E.f1;_tmp390=_tmp38E.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)((void**)((
void*)& _tmp38F->type)),_tmp390);if(!Cyc_Tcutil_coerce_arg(te,_tmp390,(void*)
_tmp38F->type)){{const char*_tmp7B6;void*_tmp7B5[3];struct Cyc_String_pa_struct
_tmp7B4;struct Cyc_String_pa_struct _tmp7B3;struct Cyc_String_pa_struct _tmp7B2;(
_tmp7B2.tag=0,((_tmp7B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp390->topt))->v)),((_tmp7B3.tag=0,((
_tmp7B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_tmp38F->type)),((_tmp7B4.tag=0,((_tmp7B4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp38F->name),((_tmp7B5[0]=& _tmp7B4,((_tmp7B5[1]=& _tmp7B3,((
_tmp7B5[2]=& _tmp7B2,Cyc_Tcutil_terr(_tmp390->loc,((_tmp7B6="field %s of struct expects type %s != %s",
_tag_dyneither(_tmp7B6,sizeof(char),41))),_tag_dyneither(_tmp7B5,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}goto _LL1DB;}_LL1DE:;_LL1DF: {const char*_tmp7B9;
void*_tmp7B8;(_tmp7B8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp7B9="tcAnonStruct: wrong type",_tag_dyneither(_tmp7B9,
sizeof(char),25))),_tag_dyneither(_tmp7B8,sizeof(void*),0)));}_LL1DB:;}return ts;}
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*
tud,struct Cyc_Absyn_Datatypefield*tuf);static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){struct
_RegionHandle*_tmp398=Cyc_Tcenv_get_fnrgn(te);struct _tuple4 _tmp7BA;struct _tuple4
_tmp399=(_tmp7BA.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp7BA.f2=_tmp398,_tmp7BA)));
struct Cyc_List_List*_tmp39A=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp398,Cyc_Tcutil_r_make_inst_var,&
_tmp399,tud->tvs);struct Cyc_List_List*_tmp39B=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp39A);struct Cyc_Absyn_DatatypeFieldType_struct _tmp7C4;union Cyc_Absyn_DatatypeFieldInfoU_union
_tmp7C3;struct Cyc_Absyn_DatatypeFieldInfo _tmp7C2;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp7C1;void*res=(void*)((_tmp7C1=_cycalloc(sizeof(*_tmp7C1)),((_tmp7C1[0]=((
_tmp7C4.tag=3,((_tmp7C4.f1=((_tmp7C2.field_info=(union Cyc_Absyn_DatatypeFieldInfoU_union)(((
_tmp7C3.KnownDatatypefield).tag=1,(((_tmp7C3.KnownDatatypefield).f1=tud,(((
_tmp7C3.KnownDatatypefield).f2=tuf,_tmp7C3)))))),((_tmp7C2.targs=_tmp39B,_tmp7C2)))),
_tmp7C4)))),_tmp7C1))));if(topt != 0){void*_tmp39C=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_DatatypeInfo _tmp39D;struct Cyc_List_List*_tmp39E;struct Cyc_Core_Opt*
_tmp39F;_LL1E1: if(_tmp39C <= (void*)4)goto _LL1E5;if(*((int*)_tmp39C)!= 3)goto
_LL1E3;_LL1E2: Cyc_Tcutil_unify(*topt,res);goto _LL1E0;_LL1E3: if(*((int*)_tmp39C)
!= 2)goto _LL1E5;_tmp39D=((struct Cyc_Absyn_DatatypeType_struct*)_tmp39C)->f1;
_tmp39E=_tmp39D.targs;_tmp39F=_tmp39D.rgn;_LL1E4:{struct Cyc_List_List*a=_tmp39B;
for(0;a != 0  && _tmp39E != 0;(a=a->tl,_tmp39E=_tmp39E->tl)){Cyc_Tcutil_unify((void*)
a->hd,(void*)_tmp39E->hd);}}if(tud->is_flat  || tuf->typs == 0  && es == 0){{struct
Cyc_Core_Opt*_tmp7C5;e->topt=((_tmp7C5=_cycalloc(sizeof(*_tmp7C5)),((_tmp7C5->v=(
void*)res,_tmp7C5))));}{struct Cyc_Absyn_DatatypeType_struct _tmp7D4;union Cyc_Absyn_DatatypeInfoU_union
_tmp7D3;struct Cyc_Absyn_Datatypedecl**_tmp7D2;struct Cyc_Absyn_DatatypeInfo
_tmp7D1;struct Cyc_Absyn_DatatypeType_struct*_tmp7D0;res=(void*)((_tmp7D0=
_cycalloc(sizeof(*_tmp7D0)),((_tmp7D0[0]=((_tmp7D4.tag=2,((_tmp7D4.f1=((_tmp7D1.datatype_info=(
union Cyc_Absyn_DatatypeInfoU_union)(((_tmp7D3.KnownDatatype).tag=1,(((_tmp7D3.KnownDatatype).f1=((
_tmp7D2=_cycalloc(sizeof(*_tmp7D2)),((_tmp7D2[0]=tud,_tmp7D2)))),_tmp7D3)))),((
_tmp7D1.targs=_tmp39B,((_tmp7D1.rgn=_tmp39F,_tmp7D1)))))),_tmp7D4)))),_tmp7D0))));}
Cyc_Tcutil_unchecked_cast(te,e,res,(void*)1);}goto _LL1E0;_LL1E5:;_LL1E6: goto
_LL1E0;_LL1E0:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 0  && ts != 0;(es=es->tl,
ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;void*t=Cyc_Tcutil_rsubstitute(
_tmp398,_tmp39A,(*((struct _tuple9*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(te,(
void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){{const char*_tmp7DC;const char*
_tmp7DB;void*_tmp7DA[3];struct Cyc_String_pa_struct _tmp7D9;struct Cyc_String_pa_struct
_tmp7D8;struct Cyc_String_pa_struct _tmp7D7;(_tmp7D7.tag=0,((_tmp7D7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)(e->topt == 0?(struct _dyneither_ptr)((
_tmp7DC="?",_tag_dyneither(_tmp7DC,sizeof(char),2))): Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v))),((_tmp7D8.tag=0,((
_tmp7D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp7D9.tag=0,((_tmp7D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name)),((_tmp7DA[0]=& _tmp7D9,((_tmp7DA[1]=& _tmp7D8,((_tmp7DA[2]=& _tmp7D7,
Cyc_Tcutil_terr(e->loc,((_tmp7DB="datatype constructor %s expects argument of type %s but this argument has type %s",
_tag_dyneither(_tmp7DB,sizeof(char),82))),_tag_dyneither(_tmp7DA,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}if(es != 0){const char*_tmp7E0;void*_tmp7DF[1];
struct Cyc_String_pa_struct _tmp7DE;return(_tmp7DE.tag=0,((_tmp7DE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp7DF[0]=& _tmp7DE,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,
topt,((_tmp7E0="too many arguments for datatype constructor %s",_tag_dyneither(
_tmp7E0,sizeof(char),47))),_tag_dyneither(_tmp7DF,sizeof(void*),1)))))));}if(ts
!= 0){const char*_tmp7E4;void*_tmp7E3[1];struct Cyc_String_pa_struct _tmp7E2;return(
_tmp7E2.tag=0,((_tmp7E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name)),((_tmp7E3[0]=& _tmp7E2,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7E4="too few arguments for datatype constructor %s",
_tag_dyneither(_tmp7E4,sizeof(char),46))),_tag_dyneither(_tmp7E3,sizeof(void*),1)))))));}
return res;}}static int Cyc_Tcexp_zeroable_type(void*t);static int Cyc_Tcexp_zeroable_type(
void*t){void*_tmp3B7=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp3B8;
struct Cyc_Absyn_PtrAtts _tmp3B9;struct Cyc_Absyn_Conref*_tmp3BA;struct Cyc_Absyn_ArrayInfo
_tmp3BB;void*_tmp3BC;struct Cyc_List_List*_tmp3BD;struct Cyc_Absyn_AggrInfo _tmp3BE;
union Cyc_Absyn_AggrInfoU_union _tmp3BF;struct Cyc_List_List*_tmp3C0;struct Cyc_List_List*
_tmp3C1;_LL1E8: if((int)_tmp3B7 != 0)goto _LL1EA;_LL1E9: return 1;_LL1EA: if(_tmp3B7 <= (
void*)4)goto _LL1F6;if(*((int*)_tmp3B7)!= 0)goto _LL1EC;_LL1EB: goto _LL1ED;_LL1EC:
if(*((int*)_tmp3B7)!= 1)goto _LL1EE;_LL1ED: goto _LL1EF;_LL1EE: if(*((int*)_tmp3B7)
!= 2)goto _LL1F0;_LL1EF: goto _LL1F1;_LL1F0: if(*((int*)_tmp3B7)!= 3)goto _LL1F2;
_LL1F1: return 0;_LL1F2: if(*((int*)_tmp3B7)!= 4)goto _LL1F4;_tmp3B8=((struct Cyc_Absyn_PointerType_struct*)
_tmp3B7)->f1;_tmp3B9=_tmp3B8.ptr_atts;_tmp3BA=_tmp3B9.nullable;_LL1F3: return((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp3BA);_LL1F4: if(*((
int*)_tmp3B7)!= 5)goto _LL1F6;_LL1F5: goto _LL1F7;_LL1F6: if((int)_tmp3B7 != 1)goto
_LL1F8;_LL1F7: goto _LL1F9;_LL1F8: if(_tmp3B7 <= (void*)4)goto _LL212;if(*((int*)
_tmp3B7)!= 6)goto _LL1FA;_LL1F9: return 1;_LL1FA: if(*((int*)_tmp3B7)!= 7)goto _LL1FC;
_tmp3BB=((struct Cyc_Absyn_ArrayType_struct*)_tmp3B7)->f1;_tmp3BC=(void*)_tmp3BB.elt_type;
_LL1FB: return Cyc_Tcexp_zeroable_type(_tmp3BC);_LL1FC: if(*((int*)_tmp3B7)!= 8)
goto _LL1FE;_LL1FD: return 0;_LL1FE: if(*((int*)_tmp3B7)!= 9)goto _LL200;_tmp3BD=((
struct Cyc_Absyn_TupleType_struct*)_tmp3B7)->f1;_LL1FF: for(0;(unsigned int)
_tmp3BD;_tmp3BD=_tmp3BD->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple9*)
_tmp3BD->hd)).f2))return 0;}return 1;_LL200: if(*((int*)_tmp3B7)!= 10)goto _LL202;
_tmp3BE=((struct Cyc_Absyn_AggrType_struct*)_tmp3B7)->f1;_tmp3BF=_tmp3BE.aggr_info;
_tmp3C0=_tmp3BE.targs;_LL201: {struct Cyc_Absyn_Aggrdecl*_tmp3C2=Cyc_Absyn_get_known_aggrdecl(
_tmp3BF);if(_tmp3C2->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp3C2->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp3C3=_new_region("r");
struct _RegionHandle*r=& _tmp3C3;_push_region(r);{struct Cyc_List_List*_tmp3C4=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp3C2->tvs,_tmp3C0);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3C2->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp3C4,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp3C5=0;_npop_handler(0);
return _tmp3C5;}}}{int _tmp3C6=1;_npop_handler(0);return _tmp3C6;}};_pop_region(r);}}
_LL202: if(*((int*)_tmp3B7)!= 12)goto _LL204;_LL203: return 1;_LL204: if(*((int*)
_tmp3B7)!= 18)goto _LL206;_LL205: return 1;_LL206: if(*((int*)_tmp3B7)!= 11)goto
_LL208;_tmp3C1=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp3B7)->f2;_LL207: for(0;
_tmp3C1 != 0;_tmp3C1=_tmp3C1->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp3C1->hd)->type))return 0;}return 1;_LL208: if(*((int*)_tmp3B7)!= 13)goto _LL20A;
_LL209: return 1;_LL20A: if(*((int*)_tmp3B7)!= 16)goto _LL20C;_LL20B: return 0;_LL20C:
if(*((int*)_tmp3B7)!= 15)goto _LL20E;_LL20D: return 0;_LL20E: if(*((int*)_tmp3B7)!= 
14)goto _LL210;_LL20F: return 0;_LL210: if(*((int*)_tmp3B7)!= 17)goto _LL212;_LL211:
goto _LL213;_LL212: if((int)_tmp3B7 != 2)goto _LL214;_LL213: goto _LL215;_LL214: if((
int)_tmp3B7 != 3)goto _LL216;_LL215: goto _LL217;_LL216: if(_tmp3B7 <= (void*)4)goto
_LL218;if(*((int*)_tmp3B7)!= 19)goto _LL218;_LL217: goto _LL219;_LL218: if(_tmp3B7 <= (
void*)4)goto _LL21A;if(*((int*)_tmp3B7)!= 20)goto _LL21A;_LL219: goto _LL21B;_LL21A:
if(_tmp3B7 <= (void*)4)goto _LL1E7;if(*((int*)_tmp3B7)!= 21)goto _LL1E7;_LL21B: {
const char*_tmp7E8;void*_tmp7E7[1];struct Cyc_String_pa_struct _tmp7E6;(_tmp7E6.tag=
0,((_tmp7E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp7E7[0]=& _tmp7E6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp7E8="bad type `%s' in zeroable type",_tag_dyneither(
_tmp7E8,sizeof(char),31))),_tag_dyneither(_tmp7E7,sizeof(void*),1)))))));}_LL1E7:;}
static void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*
loc,void**topt,void*t);static void Cyc_Tcexp_check_malloc_type(int allow_zero,
struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || 
allow_zero  && Cyc_Tcexp_zeroable_type(t))return;if(topt != 0){void*_tmp3CA=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp3CB;void*_tmp3CC;_LL21D: if(_tmp3CA <= (void*)4)
goto _LL21F;if(*((int*)_tmp3CA)!= 4)goto _LL21F;_tmp3CB=((struct Cyc_Absyn_PointerType_struct*)
_tmp3CA)->f1;_tmp3CC=(void*)_tmp3CB.elt_typ;_LL21E: Cyc_Tcutil_unify(_tmp3CC,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL21C;_LL21F:;_LL220: goto _LL21C;_LL21C:;}{const char*_tmp7F1;const char*
_tmp7F0;const char*_tmp7EF;void*_tmp7EE[2];struct Cyc_String_pa_struct _tmp7ED;
struct Cyc_String_pa_struct _tmp7EC;(_tmp7EC.tag=0,((_tmp7EC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7ED.tag=0,((
_tmp7ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct
_dyneither_ptr)((_tmp7F0="calloc",_tag_dyneither(_tmp7F0,sizeof(char),7))):(
struct _dyneither_ptr)((_tmp7F1="malloc",_tag_dyneither(_tmp7F1,sizeof(char),7))))),((
_tmp7EE[0]=& _tmp7ED,((_tmp7EE[1]=& _tmp7EC,Cyc_Tcutil_terr(loc,((_tmp7EF="%s cannot be used with type %s\n\t(type needs initialization)",
_tag_dyneither(_tmp7EF,sizeof(char),60))),_tag_dyneither(_tmp7EE,sizeof(void*),2)))))))))))));}}
static void*Cyc_Tcexp_mallocRgn(void*rgn);static void*Cyc_Tcexp_mallocRgn(void*rgn){
void*_tmp3D3=Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn));_LL222: if((int)
_tmp3D3 != 4)goto _LL224;_LL223: return(void*)3;_LL224:;_LL225: return(void*)2;
_LL221:;}static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*ropt,void***t,struct Cyc_Absyn_Exp**e,int*
is_calloc,int*is_fat);static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*ropt,void***t,
struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)2;if(ropt != 0){
struct Cyc_Absyn_RgnHandleType_struct _tmp7F4;struct Cyc_Absyn_RgnHandleType_struct*
_tmp7F3;void*expected_type=(void*)((_tmp7F3=_cycalloc(sizeof(*_tmp7F3)),((
_tmp7F3[0]=((_tmp7F4.tag=14,((_tmp7F4.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp7F4)))),_tmp7F3))));void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)ropt);
void*_tmp3D4=Cyc_Tcutil_compress(handle_type);void*_tmp3D5;_LL227: if(_tmp3D4 <= (
void*)4)goto _LL229;if(*((int*)_tmp3D4)!= 14)goto _LL229;_tmp3D5=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp3D4)->f1;_LL228: rgn=_tmp3D5;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL226;_LL229:;_LL22A:{const char*_tmp7F8;void*_tmp7F7[1];struct
Cyc_String_pa_struct _tmp7F6;(_tmp7F6.tag=0,((_tmp7F6.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type)),((_tmp7F7[0]=& _tmp7F6,
Cyc_Tcutil_terr(ropt->loc,((_tmp7F8="expecting region_t type but found %s",
_tag_dyneither(_tmp7F8,sizeof(char),37))),_tag_dyneither(_tmp7F7,sizeof(void*),1)))))));}
goto _LL226;_LL226:;}else{if(topt != 0){void*optrgn=(void*)0;if(Cyc_Tcutil_rgn_of_pointer(*
topt,& optrgn))rgn=Cyc_Tcexp_mallocRgn(optrgn);}}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*
e);{void*elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*is_calloc){if(*t == 
0){const char*_tmp7FB;void*_tmp7FA;(_tmp7FA=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7FB="calloc with empty type",
_tag_dyneither(_tmp7FB,sizeof(char),23))),_tag_dyneither(_tmp7FA,sizeof(void*),0)));}
elt_type=*((void**)_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;one_elt=0;}else{void*_tmp3DD=(void*)(*e)->r;void*_tmp3DE;void*
_tmp3DF;struct Cyc_List_List*_tmp3E0;struct Cyc_List_List _tmp3E1;struct Cyc_Absyn_Exp*
_tmp3E2;struct Cyc_List_List*_tmp3E3;struct Cyc_List_List _tmp3E4;struct Cyc_Absyn_Exp*
_tmp3E5;struct Cyc_List_List*_tmp3E6;_LL22C: if(*((int*)_tmp3DD)!= 18)goto _LL22E;
_tmp3DE=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3DD)->f1;_LL22D:
elt_type=_tmp3DE;{void**_tmp7FC;*t=(void**)((_tmp7FC=_cycalloc(sizeof(*_tmp7FC)),((
_tmp7FC[0]=elt_type,_tmp7FC))));}num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL22B;_LL22E: if(*((int*)
_tmp3DD)!= 3)goto _LL230;_tmp3DF=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3DD)->f1;if((int)_tmp3DF != 1)goto _LL230;_tmp3E0=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3DD)->f2;if(_tmp3E0 == 0)goto _LL230;_tmp3E1=*_tmp3E0;_tmp3E2=(struct Cyc_Absyn_Exp*)
_tmp3E1.hd;_tmp3E3=_tmp3E1.tl;if(_tmp3E3 == 0)goto _LL230;_tmp3E4=*_tmp3E3;_tmp3E5=(
struct Cyc_Absyn_Exp*)_tmp3E4.hd;_tmp3E6=_tmp3E4.tl;if(_tmp3E6 != 0)goto _LL230;
_LL22F:{struct _tuple0 _tmp7FD;struct _tuple0 _tmp3E9=(_tmp7FD.f1=(void*)_tmp3E2->r,((
_tmp7FD.f2=(void*)_tmp3E5->r,_tmp7FD)));void*_tmp3EA;void*_tmp3EB;void*_tmp3EC;
void*_tmp3ED;_LL233: _tmp3EA=_tmp3E9.f1;if(*((int*)_tmp3EA)!= 18)goto _LL235;
_tmp3EB=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3EA)->f1;_LL234: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3EB);elt_type=_tmp3EB;{void**_tmp7FE;*t=(void**)((_tmp7FE=
_cycalloc(sizeof(*_tmp7FE)),((_tmp7FE[0]=elt_type,_tmp7FE))));}num_elts=_tmp3E5;
one_elt=0;goto _LL232;_LL235: _tmp3EC=_tmp3E9.f2;if(*((int*)_tmp3EC)!= 18)goto
_LL237;_tmp3ED=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3EC)->f1;_LL236:
Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3ED);elt_type=_tmp3ED;{void**_tmp7FF;*
t=(void**)((_tmp7FF=_cycalloc(sizeof(*_tmp7FF)),((_tmp7FF[0]=elt_type,_tmp7FF))));}
num_elts=_tmp3E2;one_elt=0;goto _LL232;_LL237:;_LL238: goto No_sizeof;_LL232:;}goto
_LL22B;_LL230:;_LL231: No_sizeof: elt_type=Cyc_Absyn_char_typ;{void**_tmp800;*t=(
void**)((_tmp800=_cycalloc(sizeof(*_tmp800)),((_tmp800[0]=elt_type,_tmp800))));}
num_elts=*e;one_elt=0;goto _LL22B;_LL22B:;}*e=num_elts;*is_fat=!one_elt;{void*
_tmp3F1=elt_type;struct Cyc_Absyn_AggrInfo _tmp3F2;union Cyc_Absyn_AggrInfoU_union
_tmp3F3;struct Cyc_Absyn_Aggrdecl**_tmp3F4;struct Cyc_Absyn_Aggrdecl*_tmp3F5;
_LL23A: if(_tmp3F1 <= (void*)4)goto _LL23C;if(*((int*)_tmp3F1)!= 10)goto _LL23C;
_tmp3F2=((struct Cyc_Absyn_AggrType_struct*)_tmp3F1)->f1;_tmp3F3=_tmp3F2.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp3F1)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL23C;_tmp3F4=(_tmp3F3.KnownAggr).f1;_tmp3F5=*_tmp3F4;_LL23B: if(
_tmp3F5->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3F5->impl))->exist_vars
!= 0){const char*_tmp803;void*_tmp802;(_tmp802=0,Cyc_Tcutil_terr(loc,((_tmp803="malloc with existential types not yet implemented",
_tag_dyneither(_tmp803,sizeof(char),50))),_tag_dyneither(_tmp802,sizeof(void*),0)));}
goto _LL239;_LL23C:;_LL23D: goto _LL239;_LL239:;}{void*(*_tmp3F8)(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term)=Cyc_Absyn_at_typ;
struct Cyc_Absyn_Conref*_tmp3F9=Cyc_Absyn_false_conref;if(topt != 0){void*_tmp3FA=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp3FB;struct Cyc_Absyn_PtrAtts
_tmp3FC;struct Cyc_Absyn_Conref*_tmp3FD;struct Cyc_Absyn_Conref*_tmp3FE;struct Cyc_Absyn_Conref*
_tmp3FF;_LL23F: if(_tmp3FA <= (void*)4)goto _LL241;if(*((int*)_tmp3FA)!= 4)goto
_LL241;_tmp3FB=((struct Cyc_Absyn_PointerType_struct*)_tmp3FA)->f1;_tmp3FC=
_tmp3FB.ptr_atts;_tmp3FD=_tmp3FC.nullable;_tmp3FE=_tmp3FC.bounds;_tmp3FF=_tmp3FC.zero_term;
_LL240: _tmp3F9=_tmp3FF;if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3FD))_tmp3F8=Cyc_Absyn_star_typ;if(((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp3FF) && !(*is_calloc)){{const char*_tmp806;void*
_tmp805;(_tmp805=0,Cyc_Tcutil_warn(loc,((_tmp806="converting malloc to calloc to ensure zero-termination",
_tag_dyneither(_tmp806,sizeof(char),55))),_tag_dyneither(_tmp805,sizeof(void*),0)));}*
is_calloc=1;}{void*_tmp402=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp3FE);
struct Cyc_Absyn_Exp*_tmp403;_LL244: if((int)_tmp402 != 0)goto _LL246;_LL245: goto
_LL243;_LL246: if(_tmp402 <= (void*)1)goto _LL248;if(*((int*)_tmp402)!= 0)goto
_LL248;_tmp403=((struct Cyc_Absyn_Upper_b_struct*)_tmp402)->f1;if(!(!one_elt))
goto _LL248;_LL247: {int _tmp404=Cyc_Evexp_c_can_eval(num_elts);if(_tmp404  && Cyc_Evexp_same_const_exp(
_tmp403,num_elts)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_conref_val(_tmp3FE),_tmp3F9);}{void*_tmp405=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(num_elts->topt))->v);void*_tmp406;_LL24B:
if(_tmp405 <= (void*)4)goto _LL24D;if(*((int*)_tmp405)!= 18)goto _LL24D;_tmp406=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp405)->f1;_LL24C: {struct Cyc_Absyn_Exp*
_tmp407=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp406,0),0,(
void*)1,0);if(Cyc_Evexp_same_const_exp(_tmp407,_tmp403)){*is_fat=0;return Cyc_Absyn_atb_typ(
elt_type,rgn,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_conref_val(_tmp3FE),_tmp3F9);}
goto _LL24A;}_LL24D:;_LL24E: goto _LL24A;_LL24A:;}goto _LL243;}_LL248:;_LL249: goto
_LL243;_LL243:;}goto _LL23E;_LL241:;_LL242: goto _LL23E;_LL23E:;}if(!one_elt)
_tmp3F8=Cyc_Absyn_dyneither_typ;return _tmp3F8(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),_tmp3F9);}}}static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcSwap(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((
void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp408=Cyc_Tcutil_compress(t1);_LL250: if(
_tmp408 <= (void*)4)goto _LL252;if(*((int*)_tmp408)!= 7)goto _LL252;_LL251:{const
char*_tmp809;void*_tmp808;(_tmp808=0,Cyc_Tcutil_terr(loc,((_tmp809="cannot assign to an array",
_tag_dyneither(_tmp809,sizeof(char),26))),_tag_dyneither(_tmp808,sizeof(void*),0)));}
goto _LL24F;_LL252:;_LL253: goto _LL24F;_LL24F:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(
t1,& ign_1)){const char*_tmp80C;void*_tmp80B;(_tmp80B=0,Cyc_Tcutil_terr(loc,((
_tmp80C="Swap not allowed for non-pointer or non-word-sized types.",
_tag_dyneither(_tmp80C,sizeof(char),58))),_tag_dyneither(_tmp80B,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp80F;void*_tmp80E;return(_tmp80E=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp80F="swap non-lvalue",_tag_dyneither(_tmp80F,sizeof(char),
16))),_tag_dyneither(_tmp80E,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp812;void*_tmp811;return(_tmp811=0,Cyc_Tcexp_expr_err(te,e2->loc,
topt,((_tmp812="swap non-lvalue",_tag_dyneither(_tmp812,sizeof(char),16))),
_tag_dyneither(_tmp811,sizeof(void*),0)));}{void*t_ign1=(void*)0;void*t_ign2=(
void*)0;int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp815;void*_tmp814;return(_tmp814=0,Cyc_Tcexp_expr_err(te,e1->loc,
topt,((_tmp815="swap value in zeroterm array",_tag_dyneither(_tmp815,sizeof(char),
29))),_tag_dyneither(_tmp814,sizeof(void*),0)));}if(Cyc_Tcutil_is_zero_ptr_deref(
e2,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp818;void*_tmp817;return(_tmp817=0,Cyc_Tcexp_expr_err(
te,e2->loc,topt,((_tmp818="swap value in zeroterm array",_tag_dyneither(_tmp818,
sizeof(char),29))),_tag_dyneither(_tmp817,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(t1,t2)){const char*
_tmp81D;void*_tmp81C[2];struct Cyc_String_pa_struct _tmp81B;struct Cyc_String_pa_struct
_tmp81A;void*_tmp415=(_tmp81A.tag=0,((_tmp81A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp81B.tag=0,((_tmp81B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp81C[0]=&
_tmp81B,((_tmp81C[1]=& _tmp81A,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp81D="type mismatch: %s != %s",
_tag_dyneither(_tmp81D,sizeof(char),24))),_tag_dyneither(_tmp81C,sizeof(void*),2)))))))))))));
return _tmp415;}return(void*)0;}}}}int Cyc_Tcexp_in_stmt_exp=0;static void*Cyc_Tcexp_tcStmtExp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Stmt*
s);static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){struct _RegionHandle*_tmp41A=Cyc_Tcenv_get_fnrgn(
te);{int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
_tmp41A,te,s),s,1);Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp41B=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp41C;struct Cyc_Absyn_Stmt*_tmp41D;struct Cyc_Absyn_Stmt*_tmp41E;struct Cyc_Absyn_Decl*
_tmp41F;struct Cyc_Absyn_Stmt*_tmp420;_LL255: if(_tmp41B <= (void*)1)goto _LL25B;if(*((
int*)_tmp41B)!= 0)goto _LL257;_tmp41C=((struct Cyc_Absyn_Exp_s_struct*)_tmp41B)->f1;
_LL256: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp41C->topt))->v;_LL257:
if(*((int*)_tmp41B)!= 1)goto _LL259;_tmp41D=((struct Cyc_Absyn_Seq_s_struct*)
_tmp41B)->f1;_tmp41E=((struct Cyc_Absyn_Seq_s_struct*)_tmp41B)->f2;_LL258: s=
_tmp41E;continue;_LL259: if(*((int*)_tmp41B)!= 11)goto _LL25B;_tmp41F=((struct Cyc_Absyn_Decl_s_struct*)
_tmp41B)->f1;_tmp420=((struct Cyc_Absyn_Decl_s_struct*)_tmp41B)->f2;_LL25A: s=
_tmp420;continue;_LL25B:;_LL25C: {const char*_tmp820;void*_tmp81F;return(_tmp81F=
0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp820="statement expression must end with expression",
_tag_dyneither(_tmp820,sizeof(char),46))),_tag_dyneither(_tmp81F,sizeof(void*),0)));}
_LL254:;}}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1);static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1){void*rgn=(void*)2;if(rgn_handle != 0){struct Cyc_Absyn_RgnHandleType_struct
_tmp823;struct Cyc_Absyn_RgnHandleType_struct*_tmp822;void*expected_type=(void*)((
_tmp822=_cycalloc(sizeof(*_tmp822)),((_tmp822[0]=((_tmp823.tag=14,((_tmp823.f1=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(
te)),_tmp823)))),_tmp822))));void*handle_type=Cyc_Tcexp_tcExp(te,(void**)&
expected_type,(struct Cyc_Absyn_Exp*)rgn_handle);void*_tmp423=Cyc_Tcutil_compress(
handle_type);void*_tmp424;_LL25E: if(_tmp423 <= (void*)4)goto _LL260;if(*((int*)
_tmp423)!= 14)goto _LL260;_tmp424=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp423)->f1;_LL25F: rgn=_tmp424;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto
_LL25D;_LL260:;_LL261:{const char*_tmp827;void*_tmp826[1];struct Cyc_String_pa_struct
_tmp825;(_tmp825.tag=0,((_tmp825.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp826[0]=& _tmp825,Cyc_Tcutil_terr(
rgn_handle->loc,((_tmp827="expecting region_t type but found %s",_tag_dyneither(
_tmp827,sizeof(char),37))),_tag_dyneither(_tmp826,sizeof(void*),1)))))));}goto
_LL25D;_LL25D:;}else{if(topt != 0){void*optrgn=(void*)0;if(Cyc_Tcutil_rgn_of_pointer(*
topt,& optrgn))rgn=Cyc_Tcexp_mallocRgn(optrgn);}}{void*_tmp42A=(void*)e1->r;
struct Cyc_Core_Opt*_tmp42B;struct Cyc_List_List*_tmp42C;struct Cyc_List_List*
_tmp42D;union Cyc_Absyn_Cnst_union _tmp42E;struct _dyneither_ptr _tmp42F;_LL263: if(*((
int*)_tmp42A)!= 29)goto _LL265;_LL264: {void*_tmp430=Cyc_Tcexp_tcExpNoPromote(te,
topt,e1);void*_tmp431=Cyc_Tcutil_compress(_tmp430);struct Cyc_Absyn_ArrayInfo
_tmp432;void*_tmp433;struct Cyc_Absyn_Tqual _tmp434;struct Cyc_Absyn_Exp*_tmp435;
struct Cyc_Absyn_Conref*_tmp436;_LL26E: if(_tmp431 <= (void*)4)goto _LL270;if(*((int*)
_tmp431)!= 7)goto _LL270;_tmp432=((struct Cyc_Absyn_ArrayType_struct*)_tmp431)->f1;
_tmp433=(void*)_tmp432.elt_type;_tmp434=_tmp432.tq;_tmp435=_tmp432.num_elts;
_tmp436=_tmp432.zero_term;_LL26F: {struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)
_check_null(_tmp435);void*b;{void*_tmp437=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bnd->topt))->v);void*_tmp438;_LL273: if(_tmp437 <= (void*)4)goto _LL275;
if(*((int*)_tmp437)!= 18)goto _LL275;_tmp438=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp437)->f1;_LL274:{struct Cyc_Absyn_Upper_b_struct _tmp82A;struct Cyc_Absyn_Upper_b_struct*
_tmp829;b=(void*)((_tmp829=_cycalloc(sizeof(*_tmp829)),((_tmp829[0]=((_tmp82A.tag=
0,((_tmp82A.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(
_tmp438,0),0,(void*)1,0),_tmp82A)))),_tmp829))));}goto _LL272;_LL275:;_LL276: if(
Cyc_Tcutil_is_const_exp(te,bnd)){struct Cyc_Absyn_Upper_b_struct _tmp82D;struct Cyc_Absyn_Upper_b_struct*
_tmp82C;b=(void*)((_tmp82C=_cycalloc(sizeof(*_tmp82C)),((_tmp82C[0]=((_tmp82D.tag=
0,((_tmp82D.f1=bnd,_tmp82D)))),_tmp82C))));}else{b=(void*)0;}_LL272:;}{struct Cyc_Absyn_PointerType_struct
_tmp837;struct Cyc_Absyn_PtrAtts _tmp836;struct Cyc_Absyn_PtrInfo _tmp835;struct Cyc_Absyn_PointerType_struct*
_tmp834;void*res_typ=(void*)((_tmp834=_cycalloc(sizeof(*_tmp834)),((_tmp834[0]=((
_tmp837.tag=4,((_tmp837.f1=((_tmp835.elt_typ=(void*)_tmp433,((_tmp835.elt_tq=
_tmp434,((_tmp835.ptr_atts=((_tmp836.rgn=(void*)rgn,((_tmp836.nullable=((struct
Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((_tmp836.bounds=Cyc_Absyn_new_conref(
b),((_tmp836.zero_term=_tmp436,((_tmp836.ptrloc=0,_tmp836)))))))))),_tmp835)))))),
_tmp837)))),_tmp834))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp838;e->topt=((_tmp838=_cycalloc(
sizeof(*_tmp838)),((_tmp838->v=(void*)res_typ,_tmp838))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}return res_typ;}}_LL270:;_LL271: {const char*
_tmp83B;void*_tmp83A;(_tmp83A=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp83B="tcNew: comprehension returned non-array type",
_tag_dyneither(_tmp83B,sizeof(char),45))),_tag_dyneither(_tmp83A,sizeof(void*),0)));}
_LL26D:;}_LL265: if(*((int*)_tmp42A)!= 37)goto _LL267;_tmp42B=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp42A)->f1;_tmp42C=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp42A)->f2;
_LL266:{struct Cyc_Absyn_Array_e_struct _tmp83E;struct Cyc_Absyn_Array_e_struct*
_tmp83D;(void*)(e1->r=(void*)((void*)((_tmp83D=_cycalloc(sizeof(*_tmp83D)),((
_tmp83D[0]=((_tmp83E.tag=28,((_tmp83E.f1=_tmp42C,_tmp83E)))),_tmp83D))))));}
_tmp42D=_tmp42C;goto _LL268;_LL267: if(*((int*)_tmp42A)!= 28)goto _LL269;_tmp42D=((
struct Cyc_Absyn_Array_e_struct*)_tmp42A)->f1;_LL268: {void**elt_typ_opt=0;int
zero_term=0;if(topt != 0){void*_tmp446=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp447;void*_tmp448;void**_tmp449;struct Cyc_Absyn_Tqual _tmp44A;struct Cyc_Absyn_PtrAtts
_tmp44B;struct Cyc_Absyn_Conref*_tmp44C;_LL278: if(_tmp446 <= (void*)4)goto _LL27A;
if(*((int*)_tmp446)!= 4)goto _LL27A;_tmp447=((struct Cyc_Absyn_PointerType_struct*)
_tmp446)->f1;_tmp448=(void*)_tmp447.elt_typ;_tmp449=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp446)->f1).elt_typ;_tmp44A=_tmp447.elt_tq;_tmp44B=_tmp447.ptr_atts;_tmp44C=
_tmp44B.zero_term;_LL279: elt_typ_opt=(void**)_tmp449;zero_term=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp44C);goto _LL277;_LL27A:;
_LL27B: goto _LL277;_LL277:;}{void*_tmp44D=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp42D);{struct Cyc_Core_Opt*_tmp83F;e1->topt=((_tmp83F=
_cycalloc(sizeof(*_tmp83F)),((_tmp83F->v=(void*)_tmp44D,_tmp83F))));}{void*
res_typ;{void*_tmp44F=Cyc_Tcutil_compress(_tmp44D);struct Cyc_Absyn_ArrayInfo
_tmp450;void*_tmp451;struct Cyc_Absyn_Tqual _tmp452;struct Cyc_Absyn_Exp*_tmp453;
struct Cyc_Absyn_Conref*_tmp454;_LL27D: if(_tmp44F <= (void*)4)goto _LL27F;if(*((int*)
_tmp44F)!= 7)goto _LL27F;_tmp450=((struct Cyc_Absyn_ArrayType_struct*)_tmp44F)->f1;
_tmp451=(void*)_tmp450.elt_type;_tmp452=_tmp450.tq;_tmp453=_tmp450.num_elts;
_tmp454=_tmp450.zero_term;_LL27E:{struct Cyc_Absyn_PointerType_struct _tmp854;
struct Cyc_Absyn_PtrAtts _tmp853;struct Cyc_Absyn_Upper_b_struct _tmp852;struct Cyc_Absyn_Upper_b_struct*
_tmp851;struct Cyc_Absyn_PtrInfo _tmp850;struct Cyc_Absyn_PointerType_struct*
_tmp84F;res_typ=(void*)((_tmp84F=_cycalloc(sizeof(*_tmp84F)),((_tmp84F[0]=((
_tmp854.tag=4,((_tmp854.f1=((_tmp850.elt_typ=(void*)_tmp451,((_tmp850.elt_tq=
_tmp452,((_tmp850.ptr_atts=((_tmp853.rgn=(void*)rgn,((_tmp853.nullable=((struct
Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((_tmp853.bounds=Cyc_Absyn_new_conref((
void*)((_tmp851=_cycalloc(sizeof(*_tmp851)),((_tmp851[0]=((_tmp852.tag=0,((
_tmp852.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp453),_tmp852)))),_tmp851))))),((
_tmp853.zero_term=_tmp454,((_tmp853.ptrloc=0,_tmp853)))))))))),_tmp850)))))),
_tmp854)))),_tmp84F))));}if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp855;e->topt=((_tmp855=_cycalloc(
sizeof(*_tmp855)),((_tmp855->v=(void*)res_typ,_tmp855))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}goto _LL27C;_LL27F:;_LL280: {const char*
_tmp858;void*_tmp857;(_tmp857=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp858="tcExpNoPromote on Array_e returned non-array type",
_tag_dyneither(_tmp858,sizeof(char),50))),_tag_dyneither(_tmp857,sizeof(void*),0)));}
_LL27C:;}return res_typ;}}}_LL269: if(*((int*)_tmp42A)!= 0)goto _LL26B;_tmp42E=((
struct Cyc_Absyn_Const_e_struct*)_tmp42A)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp42A)->f1).String_c).tag != 5)goto _LL26B;_tmp42F=(_tmp42E.String_c).f1;_LL26A: {
void*_tmp45E=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(
void*)0,Cyc_Absyn_true_conref);void*_tmp45F=Cyc_Tcexp_tcExp(te,(void**)& _tmp45E,
e1);struct Cyc_Absyn_Upper_b_struct _tmp85B;struct Cyc_Absyn_Upper_b_struct*_tmp85A;
return Cyc_Absyn_atb_typ(_tmp45F,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp85A=
_cycalloc(sizeof(*_tmp85A)),((_tmp85A[0]=((_tmp85B.tag=0,((_tmp85B.f1=Cyc_Absyn_uint_exp(
1,0),_tmp85B)))),_tmp85A)))),Cyc_Absyn_false_conref);}_LL26B:;_LL26C: {void**
topt2=0;if(topt != 0){void*_tmp462=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp463;void*_tmp464;void**_tmp465;struct Cyc_Absyn_Tqual _tmp466;_LL282: if(
_tmp462 <= (void*)4)goto _LL286;if(*((int*)_tmp462)!= 4)goto _LL284;_tmp463=((
struct Cyc_Absyn_PointerType_struct*)_tmp462)->f1;_tmp464=(void*)_tmp463.elt_typ;
_tmp465=(void**)&(((struct Cyc_Absyn_PointerType_struct*)_tmp462)->f1).elt_typ;
_tmp466=_tmp463.elt_tq;_LL283: topt2=(void**)_tmp465;goto _LL281;_LL284: if(*((int*)
_tmp462)!= 2)goto _LL286;_LL285:{void**_tmp85C;topt2=((_tmp85C=_cycalloc(sizeof(*
_tmp85C)),((_tmp85C[0]=*topt,_tmp85C))));}goto _LL281;_LL286:;_LL287: goto _LL281;
_LL281:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);struct Cyc_Absyn_PointerType_struct
_tmp866;struct Cyc_Absyn_PtrAtts _tmp865;struct Cyc_Absyn_PtrInfo _tmp864;struct Cyc_Absyn_PointerType_struct*
_tmp863;void*res_typ=(void*)((_tmp863=_cycalloc(sizeof(*_tmp863)),((_tmp863[0]=((
_tmp866.tag=4,((_tmp866.f1=((_tmp864.elt_typ=(void*)telt,((_tmp864.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp864.ptr_atts=((_tmp865.rgn=(void*)rgn,((_tmp865.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(),((_tmp865.bounds=Cyc_Absyn_bounds_one_conref,((_tmp865.zero_term=
Cyc_Absyn_false_conref,((_tmp865.ptrloc=0,_tmp865)))))))))),_tmp864)))))),
_tmp866)))),_tmp863))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp867;e->topt=((_tmp867=_cycalloc(
sizeof(*_tmp867)),((_tmp867->v=(void*)res_typ,_tmp867))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}return res_typ;}}_LL262:;}}void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(
Cyc_Tcexp_tcExpNoPromote(te,topt,e));void*_tmp46D=t;struct Cyc_Absyn_ArrayInfo
_tmp46E;void*_tmp46F;struct Cyc_Absyn_Tqual _tmp470;struct Cyc_Absyn_Exp*_tmp471;
struct Cyc_Absyn_Conref*_tmp472;_LL289: if(_tmp46D <= (void*)4)goto _LL28B;if(*((int*)
_tmp46D)!= 7)goto _LL28B;_tmp46E=((struct Cyc_Absyn_ArrayType_struct*)_tmp46D)->f1;
_tmp46F=(void*)_tmp46E.elt_type;_tmp470=_tmp46E.tq;_tmp471=_tmp46E.num_elts;
_tmp472=_tmp46E.zero_term;_LL28A: {void*_tmp474;struct _tuple6 _tmp473=Cyc_Tcutil_addressof_props(
te,e);_tmp474=_tmp473.f2;{struct Cyc_Absyn_Upper_b_struct _tmp86A;struct Cyc_Absyn_Upper_b_struct*
_tmp869;void*_tmp475=_tmp471 == 0?(void*)((void*)0):(void*)((_tmp869=_cycalloc(
sizeof(*_tmp869)),((_tmp869[0]=((_tmp86A.tag=0,((_tmp86A.f1=(struct Cyc_Absyn_Exp*)
_tmp471,_tmp86A)))),_tmp869))));t=Cyc_Absyn_atb_typ(_tmp46F,_tmp474,_tmp470,
_tmp475,_tmp472);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);
return t;}}_LL28B:;_LL28C: return t;_LL288:;}void*Cyc_Tcexp_tcExpInitializer(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(
te,topt,e);struct _RegionHandle*_tmp478=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp478,t) && !Cyc_Tcutil_is_noalias_path(_tmp478,e)){{const char*_tmp872;const
char*_tmp871;void*_tmp870[3];struct Cyc_String_pa_struct _tmp86F;struct Cyc_String_pa_struct
_tmp86E;struct Cyc_String_pa_struct _tmp86D;(_tmp86D.tag=0,((_tmp86D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp86E.tag=0,((
_tmp86E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp86F.tag=0,((_tmp86F.f1=(
struct _dyneither_ptr)(topt == 0?(_tmp872="NULL",_tag_dyneither(_tmp872,sizeof(
char),5)):(struct _dyneither_ptr)Cyc_Absynpp_typ2string(*topt)),((_tmp870[0]=&
_tmp86F,((_tmp870[1]=& _tmp86E,((_tmp870[2]=& _tmp86D,Cyc_fprintf(Cyc_stderr,((
_tmp871="topt=%s, e->topt->v=%s, e=%s\n",_tag_dyneither(_tmp871,sizeof(char),30))),
_tag_dyneither(_tmp870,sizeof(void*),3)))))))))))))))))));}{const char*_tmp875;
void*_tmp874;(_tmp874=0,Cyc_Tcutil_terr(e->loc,((_tmp875="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp875,sizeof(char),49))),_tag_dyneither(_tmp874,sizeof(void*),0)));}}{
void*_tmp481=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp482;_LL28E: if(*((int*)
_tmp481)!= 28)goto _LL290;_LL28F: goto _LL291;_LL290: if(*((int*)_tmp481)!= 29)goto
_LL292;_LL291: goto _LL293;_LL292: if(*((int*)_tmp481)!= 0)goto _LL294;_tmp482=((
struct Cyc_Absyn_Const_e_struct*)_tmp481)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp481)->f1).String_c).tag != 5)goto _LL294;_LL293: return t;_LL294:;_LL295: t=Cyc_Tcutil_compress(
t);{void*_tmp483=t;struct Cyc_Absyn_ArrayInfo _tmp484;void*_tmp485;struct Cyc_Absyn_Tqual
_tmp486;struct Cyc_Absyn_Exp*_tmp487;struct Cyc_Absyn_Conref*_tmp488;_LL297: if(
_tmp483 <= (void*)4)goto _LL299;if(*((int*)_tmp483)!= 7)goto _LL299;_tmp484=((
struct Cyc_Absyn_ArrayType_struct*)_tmp483)->f1;_tmp485=(void*)_tmp484.elt_type;
_tmp486=_tmp484.tq;_tmp487=_tmp484.num_elts;_tmp488=_tmp484.zero_term;_LL298: {
void*_tmp48A;struct _tuple6 _tmp489=Cyc_Tcutil_addressof_props(te,e);_tmp48A=
_tmp489.f2;{struct Cyc_Absyn_Upper_b_struct _tmp878;struct Cyc_Absyn_Upper_b_struct*
_tmp877;void*b=_tmp487 == 0?(void*)((void*)0):(void*)((_tmp877=_cycalloc(sizeof(*
_tmp877)),((_tmp877[0]=((_tmp878.tag=0,((_tmp878.f1=(struct Cyc_Absyn_Exp*)
_tmp487,_tmp878)))),_tmp877))));t=Cyc_Absyn_atb_typ(_tmp485,_tmp48A,_tmp486,b,
_tmp488);Cyc_Tcutil_unchecked_cast(te,e,t,(void*)3);return t;}}_LL299:;_LL29A:
return t;_LL296:;}_LL28D:;}}static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp48D=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp48E;_LL29C: if(*((int*)_tmp48D)!= 13)goto _LL29E;_tmp48E=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp48D)->f1;_LL29D: Cyc_Tcexp_tcExpNoInst(
te,topt,_tmp48E);(void*)(((struct Cyc_Core_Opt*)_check_null(_tmp48E->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(_tmp48E->topt))->v,
0));e->topt=_tmp48E->topt;goto _LL29B;_LL29E:;_LL29F: Cyc_Tcexp_tcExpNoInst(te,
topt,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0));{void*_tmp48F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp490;void*_tmp491;struct Cyc_Absyn_Tqual _tmp492;struct Cyc_Absyn_PtrAtts _tmp493;
void*_tmp494;struct Cyc_Absyn_Conref*_tmp495;struct Cyc_Absyn_Conref*_tmp496;
struct Cyc_Absyn_Conref*_tmp497;_LL2A1: if(_tmp48F <= (void*)4)goto _LL2A3;if(*((int*)
_tmp48F)!= 4)goto _LL2A3;_tmp490=((struct Cyc_Absyn_PointerType_struct*)_tmp48F)->f1;
_tmp491=(void*)_tmp490.elt_typ;_tmp492=_tmp490.elt_tq;_tmp493=_tmp490.ptr_atts;
_tmp494=(void*)_tmp493.rgn;_tmp495=_tmp493.nullable;_tmp496=_tmp493.bounds;
_tmp497=_tmp493.zero_term;_LL2A2:{void*_tmp498=Cyc_Tcutil_compress(_tmp491);
struct Cyc_Absyn_FnInfo _tmp499;struct Cyc_List_List*_tmp49A;struct Cyc_Core_Opt*
_tmp49B;void*_tmp49C;struct Cyc_List_List*_tmp49D;int _tmp49E;struct Cyc_Absyn_VarargInfo*
_tmp49F;struct Cyc_List_List*_tmp4A0;struct Cyc_List_List*_tmp4A1;_LL2A6: if(
_tmp498 <= (void*)4)goto _LL2A8;if(*((int*)_tmp498)!= 8)goto _LL2A8;_tmp499=((
struct Cyc_Absyn_FnType_struct*)_tmp498)->f1;_tmp49A=_tmp499.tvars;_tmp49B=
_tmp499.effect;_tmp49C=(void*)_tmp499.ret_typ;_tmp49D=_tmp499.args;_tmp49E=
_tmp499.c_varargs;_tmp49F=_tmp499.cyc_varargs;_tmp4A0=_tmp499.rgn_po;_tmp4A1=
_tmp499.attributes;_LL2A7: if(_tmp49A != 0){struct _RegionHandle*_tmp4A2=Cyc_Tcenv_get_fnrgn(
te);struct _tuple4 _tmp879;struct _tuple4 _tmp4A3=(_tmp879.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp879.f2=_tmp4A2,_tmp879)));struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp4A2,Cyc_Tcutil_r_make_inst_var,&
_tmp4A3,_tmp49A);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);struct Cyc_Absyn_FnType_struct _tmp87F;struct Cyc_Absyn_FnInfo
_tmp87E;struct Cyc_Absyn_FnType_struct*_tmp87D;void*ftyp=Cyc_Tcutil_rsubstitute(
_tmp4A2,inst,(void*)((_tmp87D=_cycalloc(sizeof(*_tmp87D)),((_tmp87D[0]=((_tmp87F.tag=
8,((_tmp87F.f1=((_tmp87E.tvars=0,((_tmp87E.effect=_tmp49B,((_tmp87E.ret_typ=(
void*)_tmp49C,((_tmp87E.args=_tmp49D,((_tmp87E.c_varargs=_tmp49E,((_tmp87E.cyc_varargs=
_tmp49F,((_tmp87E.rgn_po=_tmp4A0,((_tmp87E.attributes=_tmp4A1,_tmp87E)))))))))))))))),
_tmp87F)))),_tmp87D)))));struct Cyc_Absyn_PointerType_struct _tmp889;struct Cyc_Absyn_PtrAtts
_tmp888;struct Cyc_Absyn_PtrInfo _tmp887;struct Cyc_Absyn_PointerType_struct*
_tmp886;struct Cyc_Absyn_PointerType_struct*_tmp4A4=(_tmp886=_cycalloc(sizeof(*
_tmp886)),((_tmp886[0]=((_tmp889.tag=4,((_tmp889.f1=((_tmp887.elt_typ=(void*)
ftyp,((_tmp887.elt_tq=_tmp492,((_tmp887.ptr_atts=((_tmp888.rgn=(void*)_tmp494,((
_tmp888.nullable=_tmp495,((_tmp888.bounds=_tmp496,((_tmp888.zero_term=_tmp497,((
_tmp888.ptrloc=0,_tmp888)))))))))),_tmp887)))))),_tmp889)))),_tmp886)));struct
Cyc_Absyn_Exp*_tmp4A5=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Instantiate_e_struct
_tmp88C;struct Cyc_Absyn_Instantiate_e_struct*_tmp88B;(void*)(e->r=(void*)((void*)((
_tmp88B=_cycalloc(sizeof(*_tmp88B)),((_tmp88B[0]=((_tmp88C.tag=14,((_tmp88C.f1=
_tmp4A5,((_tmp88C.f2=ts,_tmp88C)))))),_tmp88B))))));}{struct Cyc_Core_Opt*_tmp88D;
e->topt=((_tmp88D=_cycalloc(sizeof(*_tmp88D)),((_tmp88D->v=(void*)((void*)
_tmp4A4),_tmp88D))));}}goto _LL2A5;_LL2A8:;_LL2A9: goto _LL2A5;_LL2A5:;}goto _LL2A0;
_LL2A3:;_LL2A4: goto _LL2A0;_LL2A0:;}goto _LL29B;_LL29B:;}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*loc=e->loc;void*
t;{void*_tmp4B1=(void*)e->r;struct Cyc_Absyn_Exp*_tmp4B2;struct _tuple1*_tmp4B3;
struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_List_List*_tmp4B5;struct Cyc_Core_Opt*
_tmp4B6;struct Cyc_List_List*_tmp4B7;union Cyc_Absyn_Cnst_union _tmp4B8;union Cyc_Absyn_Cnst_union*
_tmp4B9;struct _tuple1*_tmp4BA;void*_tmp4BB;void*_tmp4BC;struct Cyc_List_List*
_tmp4BD;struct Cyc_Absyn_Exp*_tmp4BE;void*_tmp4BF;struct Cyc_Absyn_Exp*_tmp4C0;
struct Cyc_Core_Opt*_tmp4C1;struct Cyc_Absyn_Exp*_tmp4C2;struct Cyc_Absyn_Exp*
_tmp4C3;struct Cyc_Absyn_Exp*_tmp4C4;struct Cyc_Absyn_Exp*_tmp4C5;struct Cyc_Absyn_Exp*
_tmp4C6;struct Cyc_Absyn_Exp*_tmp4C7;struct Cyc_Absyn_Exp*_tmp4C8;struct Cyc_Absyn_Exp*
_tmp4C9;struct Cyc_Absyn_Exp*_tmp4CA;struct Cyc_Absyn_Exp*_tmp4CB;struct Cyc_Absyn_Exp*
_tmp4CC;struct Cyc_List_List*_tmp4CD;struct Cyc_Absyn_VarargCallInfo*_tmp4CE;
struct Cyc_Absyn_VarargCallInfo**_tmp4CF;struct Cyc_Absyn_Exp*_tmp4D0;struct Cyc_Absyn_Exp*
_tmp4D1;struct Cyc_List_List*_tmp4D2;void*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D4;
void*_tmp4D5;void**_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D7;struct Cyc_Absyn_Exp*
_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4DA;void*_tmp4DB;
void*_tmp4DC;void*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DE;struct Cyc_Absyn_Exp*
_tmp4DF;struct _dyneither_ptr*_tmp4E0;int _tmp4E1;int*_tmp4E2;int _tmp4E3;int*
_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E5;struct _dyneither_ptr*_tmp4E6;int _tmp4E7;int*
_tmp4E8;int _tmp4E9;int*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4EB;struct Cyc_Absyn_Exp*
_tmp4EC;struct Cyc_List_List*_tmp4ED;struct _tuple2*_tmp4EE;struct Cyc_List_List*
_tmp4EF;struct Cyc_List_List*_tmp4F0;struct Cyc_Absyn_Stmt*_tmp4F1;struct Cyc_Absyn_Vardecl*
_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F4;int _tmp4F5;int*
_tmp4F6;struct _tuple1*_tmp4F7;struct _tuple1**_tmp4F8;struct Cyc_List_List*_tmp4F9;
struct Cyc_List_List**_tmp4FA;struct Cyc_List_List*_tmp4FB;struct Cyc_Absyn_Aggrdecl*
_tmp4FC;struct Cyc_Absyn_Aggrdecl**_tmp4FD;void*_tmp4FE;struct Cyc_List_List*
_tmp4FF;struct Cyc_List_List*_tmp500;struct Cyc_Absyn_Datatypedecl*_tmp501;struct
Cyc_Absyn_Datatypefield*_tmp502;struct _tuple1*_tmp503;struct _tuple1**_tmp504;
struct Cyc_Absyn_Enumdecl*_tmp505;struct Cyc_Absyn_Enumfield*_tmp506;struct _tuple1*
_tmp507;struct _tuple1**_tmp508;void*_tmp509;struct Cyc_Absyn_Enumfield*_tmp50A;
struct Cyc_Absyn_MallocInfo _tmp50B;int _tmp50C;int*_tmp50D;struct Cyc_Absyn_Exp*
_tmp50E;void**_tmp50F;void***_tmp510;struct Cyc_Absyn_Exp*_tmp511;struct Cyc_Absyn_Exp**
_tmp512;int _tmp513;int*_tmp514;struct Cyc_Absyn_Exp*_tmp515;struct Cyc_Absyn_Exp*
_tmp516;void*_tmp517;_LL2AB: if(*((int*)_tmp4B1)!= 13)goto _LL2AD;_tmp4B2=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp4B1)->f1;_LL2AC: Cyc_Tcexp_tcExpNoInst(te,0,
_tmp4B2);return;_LL2AD: if(*((int*)_tmp4B1)!= 2)goto _LL2AF;_tmp4B3=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp4B1)->f1;_LL2AE: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp4B3);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL2AF: if(*((int*)_tmp4B1)!= 10)goto _LL2B1;_tmp4B4=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp4B1)->f1;_tmp4B5=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp4B1)->f2;_LL2B0:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp4B4,_tmp4B5);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL2B1: if(*((int*)_tmp4B1)!= 37)goto _LL2B3;_tmp4B6=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp4B1)->f1;_tmp4B7=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp4B1)->f2;
_LL2B2: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp4B7);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL2B3: if(*((int*)_tmp4B1)!= 0)goto _LL2B5;_tmp4B8=((struct Cyc_Absyn_Const_e_struct*)
_tmp4B1)->f1;_tmp4B9=(union Cyc_Absyn_Cnst_union*)&((struct Cyc_Absyn_Const_e_struct*)
_tmp4B1)->f1;_LL2B4: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst_union*)
_tmp4B9,e);goto _LL2AA;_LL2B5: if(*((int*)_tmp4B1)!= 1)goto _LL2B7;_tmp4BA=((struct
Cyc_Absyn_Var_e_struct*)_tmp4B1)->f1;_tmp4BB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp4B1)->f2;_LL2B6: t=Cyc_Tcexp_tcVar(te,loc,_tmp4BA,_tmp4BB);goto _LL2AA;_LL2B7:
if(*((int*)_tmp4B1)!= 3)goto _LL2B9;_tmp4BC=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp4B1)->f1;_tmp4BD=((struct Cyc_Absyn_Primop_e_struct*)_tmp4B1)->f2;_LL2B8: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp4BC,_tmp4BD);goto _LL2AA;_LL2B9: if(*((int*)
_tmp4B1)!= 5)goto _LL2BB;_tmp4BE=((struct Cyc_Absyn_Increment_e_struct*)_tmp4B1)->f1;
_tmp4BF=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp4B1)->f2;_LL2BA: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp4BE,_tmp4BF);goto _LL2AA;_LL2BB: if(*((int*)_tmp4B1)!= 4)goto
_LL2BD;_tmp4C0=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp4B1)->f1;_tmp4C1=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp4B1)->f2;_tmp4C2=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp4B1)->f3;_LL2BC: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp4C0,_tmp4C1,_tmp4C2);
goto _LL2AA;_LL2BD: if(*((int*)_tmp4B1)!= 6)goto _LL2BF;_tmp4C3=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp4B1)->f1;_tmp4C4=((struct Cyc_Absyn_Conditional_e_struct*)_tmp4B1)->f2;
_tmp4C5=((struct Cyc_Absyn_Conditional_e_struct*)_tmp4B1)->f3;_LL2BE: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp4C3,_tmp4C4,_tmp4C5);goto _LL2AA;_LL2BF: if(*((int*)_tmp4B1)!= 7)
goto _LL2C1;_tmp4C6=((struct Cyc_Absyn_And_e_struct*)_tmp4B1)->f1;_tmp4C7=((struct
Cyc_Absyn_And_e_struct*)_tmp4B1)->f2;_LL2C0: t=Cyc_Tcexp_tcAnd(te,loc,_tmp4C6,
_tmp4C7);goto _LL2AA;_LL2C1: if(*((int*)_tmp4B1)!= 8)goto _LL2C3;_tmp4C8=((struct
Cyc_Absyn_Or_e_struct*)_tmp4B1)->f1;_tmp4C9=((struct Cyc_Absyn_Or_e_struct*)
_tmp4B1)->f2;_LL2C2: t=Cyc_Tcexp_tcOr(te,loc,_tmp4C8,_tmp4C9);goto _LL2AA;_LL2C3:
if(*((int*)_tmp4B1)!= 9)goto _LL2C5;_tmp4CA=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp4B1)->f1;_tmp4CB=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp4B1)->f2;_LL2C4: t=
Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp4CA,_tmp4CB);goto _LL2AA;_LL2C5: if(*((int*)
_tmp4B1)!= 11)goto _LL2C7;_tmp4CC=((struct Cyc_Absyn_FnCall_e_struct*)_tmp4B1)->f1;
_tmp4CD=((struct Cyc_Absyn_FnCall_e_struct*)_tmp4B1)->f2;_tmp4CE=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp4B1)->f3;_tmp4CF=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp4B1)->f3;_LL2C6: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp4CC,_tmp4CD,_tmp4CF);
goto _LL2AA;_LL2C7: if(*((int*)_tmp4B1)!= 12)goto _LL2C9;_tmp4D0=((struct Cyc_Absyn_Throw_e_struct*)
_tmp4B1)->f1;_LL2C8: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp4D0);goto _LL2AA;_LL2C9:
if(*((int*)_tmp4B1)!= 14)goto _LL2CB;_tmp4D1=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp4B1)->f1;_tmp4D2=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp4B1)->f2;_LL2CA:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp4D1,_tmp4D2);goto _LL2AA;_LL2CB: if(*((
int*)_tmp4B1)!= 15)goto _LL2CD;_tmp4D3=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp4B1)->f1;_tmp4D4=((struct Cyc_Absyn_Cast_e_struct*)_tmp4B1)->f2;_tmp4D5=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp4B1)->f4;_tmp4D6=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp4B1)->f4);_LL2CC: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4D3,_tmp4D4,(void**)
_tmp4D6);goto _LL2AA;_LL2CD: if(*((int*)_tmp4B1)!= 16)goto _LL2CF;_tmp4D7=((struct
Cyc_Absyn_Address_e_struct*)_tmp4B1)->f1;_LL2CE: t=Cyc_Tcexp_tcAddress(te,loc,e,
topt,_tmp4D7);goto _LL2AA;_LL2CF: if(*((int*)_tmp4B1)!= 17)goto _LL2D1;_tmp4D8=((
struct Cyc_Absyn_New_e_struct*)_tmp4B1)->f1;_tmp4D9=((struct Cyc_Absyn_New_e_struct*)
_tmp4B1)->f2;_LL2D0: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4D8,e,_tmp4D9);goto _LL2AA;
_LL2D1: if(*((int*)_tmp4B1)!= 19)goto _LL2D3;_tmp4DA=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp4B1)->f1;_LL2D2: {void*_tmp518=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4DA);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp518);goto _LL2AA;}_LL2D3: if(*((int*)_tmp4B1)!= 18)goto _LL2D5;
_tmp4DB=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp4B1)->f1;_LL2D4: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4DB);goto _LL2AA;_LL2D5: if(*((int*)_tmp4B1)!= 20)goto _LL2D7;
_tmp4DC=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp4B1)->f1;_tmp4DD=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp4B1)->f2;_LL2D6: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp4DC,_tmp4DD);goto _LL2AA;_LL2D7: if(*((int*)_tmp4B1)!= 21)goto _LL2D9;
_LL2D8:{const char*_tmp890;void*_tmp88F;(_tmp88F=0,Cyc_Tcutil_terr(loc,((_tmp890="gen() not in top-level initializer",
_tag_dyneither(_tmp890,sizeof(char),35))),_tag_dyneither(_tmp88F,sizeof(void*),0)));}
return;_LL2D9: if(*((int*)_tmp4B1)!= 22)goto _LL2DB;_tmp4DE=((struct Cyc_Absyn_Deref_e_struct*)
_tmp4B1)->f1;_LL2DA: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4DE);goto _LL2AA;_LL2DB:
if(*((int*)_tmp4B1)!= 23)goto _LL2DD;_tmp4DF=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp4B1)->f1;_tmp4E0=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp4B1)->f2;_tmp4E1=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp4B1)->f3;_tmp4E2=(int*)&((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp4B1)->f3;_tmp4E3=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp4B1)->f4;_tmp4E4=(
int*)&((struct Cyc_Absyn_AggrMember_e_struct*)_tmp4B1)->f4;_LL2DC: t=Cyc_Tcexp_tcAggrMember(
te,loc,topt,e,_tmp4DF,_tmp4E0,_tmp4E2,_tmp4E4);goto _LL2AA;_LL2DD: if(*((int*)
_tmp4B1)!= 24)goto _LL2DF;_tmp4E5=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp4B1)->f1;
_tmp4E6=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp4B1)->f2;_tmp4E7=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp4B1)->f3;_tmp4E8=(int*)&((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp4B1)->f3;
_tmp4E9=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp4B1)->f4;_tmp4EA=(int*)&((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp4B1)->f4;_LL2DE: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp4E5,_tmp4E6,_tmp4E8,_tmp4EA);goto _LL2AA;_LL2DF: if(*((int*)
_tmp4B1)!= 25)goto _LL2E1;_tmp4EB=((struct Cyc_Absyn_Subscript_e_struct*)_tmp4B1)->f1;
_tmp4EC=((struct Cyc_Absyn_Subscript_e_struct*)_tmp4B1)->f2;_LL2E0: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp4EB,_tmp4EC);goto _LL2AA;_LL2E1: if(*((int*)_tmp4B1)!= 26)goto
_LL2E3;_tmp4ED=((struct Cyc_Absyn_Tuple_e_struct*)_tmp4B1)->f1;_LL2E2: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp4ED);goto _LL2AA;_LL2E3: if(*((int*)_tmp4B1)!= 27)goto _LL2E5;
_tmp4EE=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp4B1)->f1;_tmp4EF=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp4B1)->f2;_LL2E4: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4EE,_tmp4EF);goto
_LL2AA;_LL2E5: if(*((int*)_tmp4B1)!= 28)goto _LL2E7;_tmp4F0=((struct Cyc_Absyn_Array_e_struct*)
_tmp4B1)->f1;_LL2E6: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp51B=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp51C;void*_tmp51D;void**
_tmp51E;struct Cyc_Absyn_Conref*_tmp51F;_LL2FC: if(_tmp51B <= (void*)4)goto _LL2FE;
if(*((int*)_tmp51B)!= 7)goto _LL2FE;_tmp51C=((struct Cyc_Absyn_ArrayType_struct*)
_tmp51B)->f1;_tmp51D=(void*)_tmp51C.elt_type;_tmp51E=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp51B)->f1).elt_type;_tmp51F=_tmp51C.zero_term;_LL2FD: elt_topt=(void**)_tmp51E;
zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp51F);
goto _LL2FB;_LL2FE:;_LL2FF: goto _LL2FB;_LL2FB:;}t=Cyc_Tcexp_tcArray(te,loc,
elt_topt,zero_term,_tmp4F0);goto _LL2AA;}_LL2E7: if(*((int*)_tmp4B1)!= 38)goto
_LL2E9;_tmp4F1=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp4B1)->f1;_LL2E8: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp4F1);goto _LL2AA;_LL2E9: if(*((int*)_tmp4B1)!= 29)goto _LL2EB;
_tmp4F2=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4B1)->f1;_tmp4F3=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp4B1)->f2;_tmp4F4=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp4B1)->f3;_tmp4F5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4B1)->f4;
_tmp4F6=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4B1)->f4;_LL2EA: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4F2,_tmp4F3,_tmp4F4,_tmp4F6);goto
_LL2AA;_LL2EB: if(*((int*)_tmp4B1)!= 30)goto _LL2ED;_tmp4F7=((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp4B1)->f1;_tmp4F8=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp4B1)->f1;_tmp4F9=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp4B1)->f2;_tmp4FA=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_struct*)_tmp4B1)->f2;
_tmp4FB=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp4B1)->f3;_tmp4FC=((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp4B1)->f4;_tmp4FD=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp4B1)->f4;_LL2EC: t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp4F8,_tmp4FA,_tmp4FB,
_tmp4FD);goto _LL2AA;_LL2ED: if(*((int*)_tmp4B1)!= 31)goto _LL2EF;_tmp4FE=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp4B1)->f1;_tmp4FF=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp4B1)->f2;_LL2EE: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4FE,_tmp4FF);goto _LL2AA;
_LL2EF: if(*((int*)_tmp4B1)!= 32)goto _LL2F1;_tmp500=((struct Cyc_Absyn_Datatype_e_struct*)
_tmp4B1)->f1;_tmp501=((struct Cyc_Absyn_Datatype_e_struct*)_tmp4B1)->f2;_tmp502=((
struct Cyc_Absyn_Datatype_e_struct*)_tmp4B1)->f3;_LL2F0: t=Cyc_Tcexp_tcDatatype(te,
loc,topt,e,_tmp500,_tmp501,_tmp502);goto _LL2AA;_LL2F1: if(*((int*)_tmp4B1)!= 33)
goto _LL2F3;_tmp503=((struct Cyc_Absyn_Enum_e_struct*)_tmp4B1)->f1;_tmp504=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp4B1)->f1;_tmp505=((struct Cyc_Absyn_Enum_e_struct*)
_tmp4B1)->f2;_tmp506=((struct Cyc_Absyn_Enum_e_struct*)_tmp4B1)->f3;_LL2F2:*
_tmp504=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp506))->name;{struct Cyc_Absyn_EnumType_struct
_tmp893;struct Cyc_Absyn_EnumType_struct*_tmp892;t=(void*)((_tmp892=_cycalloc(
sizeof(*_tmp892)),((_tmp892[0]=((_tmp893.tag=12,((_tmp893.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp505))->name,((_tmp893.f2=_tmp505,_tmp893)))))),_tmp892))));}goto
_LL2AA;_LL2F3: if(*((int*)_tmp4B1)!= 34)goto _LL2F5;_tmp507=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp4B1)->f1;_tmp508=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp4B1)->f1;_tmp509=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp4B1)->f2;
_tmp50A=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp4B1)->f3;_LL2F4:*_tmp508=((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp50A))->name;t=_tmp509;goto _LL2AA;
_LL2F5: if(*((int*)_tmp4B1)!= 35)goto _LL2F7;_tmp50B=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp4B1)->f1;_tmp50C=_tmp50B.is_calloc;_tmp50D=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp4B1)->f1).is_calloc;_tmp50E=_tmp50B.rgn;_tmp50F=_tmp50B.elt_type;_tmp510=(
void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp4B1)->f1).elt_type;_tmp511=
_tmp50B.num_elts;_tmp512=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp4B1)->f1).num_elts;_tmp513=_tmp50B.fat_result;_tmp514=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp4B1)->f1).fat_result;_LL2F6: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp50E,_tmp510,
_tmp512,_tmp50D,_tmp514);goto _LL2AA;_LL2F7: if(*((int*)_tmp4B1)!= 36)goto _LL2F9;
_tmp515=((struct Cyc_Absyn_Swap_e_struct*)_tmp4B1)->f1;_tmp516=((struct Cyc_Absyn_Swap_e_struct*)
_tmp4B1)->f2;_LL2F8: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp515,_tmp516);goto _LL2AA;
_LL2F9: if(*((int*)_tmp4B1)!= 39)goto _LL2AA;_tmp517=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp4B1)->f1;_LL2FA: if(!te->allow_valueof){const char*_tmp896;void*_tmp895;(
_tmp895=0,Cyc_Tcutil_terr(e->loc,((_tmp896="valueof(-) can only occur within types",
_tag_dyneither(_tmp896,sizeof(char),39))),_tag_dyneither(_tmp895,sizeof(void*),0)));}
t=Cyc_Absyn_sint_typ;goto _LL2AA;_LL2AA:;}{struct Cyc_Core_Opt*_tmp897;e->topt=((
_tmp897=_cycalloc(sizeof(*_tmp897)),((_tmp897->v=(void*)t,_tmp897))));}}
