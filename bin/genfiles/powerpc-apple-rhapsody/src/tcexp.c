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
_dyneither_ptr msg,struct _dyneither_ptr ap){{void*_tmp507;(_tmp507=0,Cyc_Tcutil_terr(
loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),
_tag_dyneither(_tmp507,sizeof(void*),0)));}if(topt == 0)return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q);static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct
_handler_cons _tmp1;_push_handler(& _tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))
_tmp3=1;if(!_tmp3){{struct _RegionHandle*_tmp4=Cyc_Tcenv_get_fnrgn(te);void*_tmp5=
Cyc_Tcenv_lookup_ordinary(_tmp4,te,e->loc,q);void*_tmp6;struct Cyc_Absyn_Enumdecl*
_tmp7;struct Cyc_Absyn_Enumfield*_tmp8;void*_tmp9;struct Cyc_Absyn_Enumfield*_tmpA;
struct Cyc_Absyn_Tuniondecl*_tmpB;struct Cyc_Absyn_Tunionfield*_tmpC;_LL1: if(*((
int*)_tmp5)!= 0)goto _LL3;_tmp6=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp5)->f1;
_LL2:{struct Cyc_Absyn_Var_e_struct _tmp50A;struct Cyc_Absyn_Var_e_struct*_tmp509;(
void*)(e->r=(void*)((void*)((_tmp509=_cycalloc(sizeof(*_tmp509)),((_tmp509[0]=((
_tmp50A.tag=1,((_tmp50A.f1=q,((_tmp50A.f2=(void*)_tmp6,_tmp50A)))))),_tmp509))))));}
goto _LL0;_LL3: if(*((int*)_tmp5)!= 3)goto _LL5;_tmp7=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp5)->f1;_tmp8=((struct Cyc_Tcenv_EnumRes_struct*)_tmp5)->f2;_LL4:{struct Cyc_Absyn_Enum_e_struct
_tmp50D;struct Cyc_Absyn_Enum_e_struct*_tmp50C;(void*)(e->r=(void*)((void*)((
_tmp50C=_cycalloc(sizeof(*_tmp50C)),((_tmp50C[0]=((_tmp50D.tag=33,((_tmp50D.f1=q,((
_tmp50D.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7,((_tmp50D.f3=(struct Cyc_Absyn_Enumfield*)
_tmp8,_tmp50D)))))))),_tmp50C))))));}goto _LL0;_LL5: if(*((int*)_tmp5)!= 4)goto
_LL7;_tmp9=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f1;_tmpA=((
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f2;_LL6:{struct Cyc_Absyn_AnonEnum_e_struct
_tmp510;struct Cyc_Absyn_AnonEnum_e_struct*_tmp50F;(void*)(e->r=(void*)((void*)((
_tmp50F=_cycalloc(sizeof(*_tmp50F)),((_tmp50F[0]=((_tmp510.tag=34,((_tmp510.f1=q,((
_tmp510.f2=(void*)_tmp9,((_tmp510.f3=(struct Cyc_Absyn_Enumfield*)_tmpA,_tmp510)))))))),
_tmp50F))))));}goto _LL0;_LL7: if(*((int*)_tmp5)!= 2)goto _LL9;_tmpB=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp5)->f1;_tmpC=((struct Cyc_Tcenv_TunionRes_struct*)_tmp5)->f2;_LL8:{struct Cyc_Absyn_Tunion_e_struct
_tmp513;struct Cyc_Absyn_Tunion_e_struct*_tmp512;(void*)(e->r=(void*)((void*)((
_tmp512=_cycalloc(sizeof(*_tmp512)),((_tmp512[0]=((_tmp513.tag=32,((_tmp513.f1=0,((
_tmp513.f2=_tmpB,((_tmp513.f3=_tmpC,_tmp513)))))))),_tmp512))))));}goto _LL0;_LL9:
if(*((int*)_tmp5)!= 1)goto _LL0;_LLA:{const char*_tmp517;void*_tmp516[1];struct Cyc_String_pa_struct
_tmp515;(_tmp515.tag=0,((_tmp515.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(q)),((_tmp516[0]=& _tmp515,Cyc_Tcutil_terr(e->loc,((
_tmp517="bad occurrence of type name %s",_tag_dyneither(_tmp517,sizeof(char),31))),
_tag_dyneither(_tmp516,sizeof(void*),1)))))));}{struct Cyc_Absyn_Var_e_struct
_tmp51A;struct Cyc_Absyn_Var_e_struct*_tmp519;(void*)(e->r=(void*)((void*)((
_tmp519=_cycalloc(sizeof(*_tmp519)),((_tmp519[0]=((_tmp51A.tag=1,((_tmp51A.f1=q,((
_tmp51A.f2=(void*)((void*)0),_tmp51A)))))),_tmp519))))));}goto _LL0;_LL0:;};
_pop_handler();}else{void*_tmp2=(void*)_exn_thrown;void*_tmp1B=_tmp2;_LLC: if(
_tmp1B != Cyc_Dict_Absent)goto _LLE;_LLD:{struct Cyc_Absyn_Var_e_struct _tmp51D;
struct Cyc_Absyn_Var_e_struct*_tmp51C;(void*)(e->r=(void*)((void*)((_tmp51C=
_cycalloc(sizeof(*_tmp51C)),((_tmp51C[0]=((_tmp51D.tag=1,((_tmp51D.f1=q,((
_tmp51D.f2=(void*)((void*)0),_tmp51D)))))),_tmp51C))))));}goto _LLB;_LLE:;_LLF:(
void)_throw(_tmp1B);_LLB:;}}}struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple8*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e);static
struct _tuple8*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){struct _tuple8*
_tmp51E;return(_tmp51E=_cycalloc(sizeof(*_tmp51E)),((_tmp51E->f1=0,((_tmp51E->f2=
e,_tmp51E)))));}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es);static void
Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*_tmp1F=(void*)e1->r;struct
_tuple1*_tmp20;_LL11: if(*((int*)_tmp1F)!= 2)goto _LL13;_tmp20=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp1F)->f1;_LL12: {struct _handler_cons _tmp21;_push_handler(& _tmp21);{int _tmp23=
0;if(setjmp(_tmp21.handler))_tmp23=1;if(!_tmp23){{struct _RegionHandle*_tmp24=Cyc_Tcenv_get_fnrgn(
te);void*_tmp25=Cyc_Tcenv_lookup_ordinary(_tmp24,te,e1->loc,_tmp20);void*_tmp26;
struct Cyc_Absyn_Tuniondecl*_tmp27;struct Cyc_Absyn_Tunionfield*_tmp28;struct Cyc_Absyn_Aggrdecl*
_tmp29;_LL16: if(*((int*)_tmp25)!= 0)goto _LL18;_tmp26=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmp25)->f1;_LL17:{struct Cyc_Absyn_FnCall_e_struct _tmp521;struct Cyc_Absyn_FnCall_e_struct*
_tmp520;(void*)(e->r=(void*)((void*)((_tmp520=_cycalloc(sizeof(*_tmp520)),((
_tmp520[0]=((_tmp521.tag=11,((_tmp521.f1=e1,((_tmp521.f2=es,((_tmp521.f3=0,
_tmp521)))))))),_tmp520))))));}_npop_handler(0);return;_LL18: if(*((int*)_tmp25)
!= 2)goto _LL1A;_tmp27=((struct Cyc_Tcenv_TunionRes_struct*)_tmp25)->f1;_tmp28=((
struct Cyc_Tcenv_TunionRes_struct*)_tmp25)->f2;_LL19: if(_tmp28->typs == 0){const
char*_tmp525;void*_tmp524[1];struct Cyc_String_pa_struct _tmp523;(_tmp523.tag=0,((
_tmp523.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp28->name)),((_tmp524[0]=& _tmp523,Cyc_Tcutil_terr(e->loc,((_tmp525="%s is a constant, not a function",
_tag_dyneither(_tmp525,sizeof(char),33))),_tag_dyneither(_tmp524,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Tunion_e_struct _tmp528;struct Cyc_Absyn_Tunion_e_struct*_tmp527;(
void*)(e->r=(void*)((void*)((_tmp527=_cycalloc(sizeof(*_tmp527)),((_tmp527[0]=((
_tmp528.tag=32,((_tmp528.f1=es,((_tmp528.f2=_tmp27,((_tmp528.f3=_tmp28,_tmp528)))))))),
_tmp527))))));}_npop_handler(0);return;_LL1A: if(*((int*)_tmp25)!= 1)goto _LL1C;
_tmp29=((struct Cyc_Tcenv_AggrRes_struct*)_tmp25)->f1;_LL1B: {struct Cyc_List_List*
_tmp31=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct Cyc_Absyn_Exp*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);{struct Cyc_Absyn_Struct_e_struct
_tmp52B;struct Cyc_Absyn_Struct_e_struct*_tmp52A;(void*)(e->r=(void*)((void*)((
_tmp52A=_cycalloc(sizeof(*_tmp52A)),((_tmp52A[0]=((_tmp52B.tag=30,((_tmp52B.f1=
_tmp29->name,((_tmp52B.f2=0,((_tmp52B.f3=_tmp31,((_tmp52B.f4=(struct Cyc_Absyn_Aggrdecl*)
_tmp29,_tmp52B)))))))))),_tmp52A))))));}_npop_handler(0);return;}_LL1C: if(*((int*)
_tmp25)!= 4)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp25)!= 3)goto _LL15;
_LL1F:{const char*_tmp52F;void*_tmp52E[1];struct Cyc_String_pa_struct _tmp52D;(
_tmp52D.tag=0,((_tmp52D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp20)),((_tmp52E[0]=& _tmp52D,Cyc_Tcutil_terr(e->loc,((_tmp52F="%s is an enum constructor, not a function",
_tag_dyneither(_tmp52F,sizeof(char),42))),_tag_dyneither(_tmp52E,sizeof(void*),1)))))));}
_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*_tmp22=(void*)
_exn_thrown;void*_tmp38=_tmp22;_LL21: if(_tmp38 != Cyc_Dict_Absent)goto _LL23;_LL22:{
struct Cyc_Absyn_FnCall_e_struct _tmp532;struct Cyc_Absyn_FnCall_e_struct*_tmp531;(
void*)(e->r=(void*)((void*)((_tmp531=_cycalloc(sizeof(*_tmp531)),((_tmp531[0]=((
_tmp532.tag=11,((_tmp532.f1=e1,((_tmp532.f2=es,((_tmp532.f3=0,_tmp532)))))))),
_tmp531))))));}return;_LL23:;_LL24:(void)_throw(_tmp38);_LL20:;}}}_LL13:;_LL14:{
struct Cyc_Absyn_FnCall_e_struct _tmp535;struct Cyc_Absyn_FnCall_e_struct*_tmp534;(
void*)(e->r=(void*)((void*)((_tmp534=_cycalloc(sizeof(*_tmp534)),((_tmp534[0]=((
_tmp535.tag=11,((_tmp535.f1=e1,((_tmp535.f2=es,((_tmp535.f3=0,_tmp535)))))))),
_tmp534))))));}return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des);static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
des){if(topt == 0){{struct Cyc_Absyn_Array_e_struct _tmp538;struct Cyc_Absyn_Array_e_struct*
_tmp537;(void*)(e->r=(void*)((void*)((_tmp537=_cycalloc(sizeof(*_tmp537)),((
_tmp537[0]=((_tmp538.tag=28,((_tmp538.f1=des,_tmp538)))),_tmp537))))));}return;}{
void*t=*topt;void*_tmp3F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp40;
union Cyc_Absyn_AggrInfoU_union _tmp41;struct Cyc_Absyn_ArrayInfo _tmp42;void*_tmp43;
struct Cyc_Absyn_Tqual _tmp44;_LL26: if(_tmp3F <= (void*)4)goto _LL2C;if(*((int*)
_tmp3F)!= 10)goto _LL28;_tmp40=((struct Cyc_Absyn_AggrType_struct*)_tmp3F)->f1;
_tmp41=_tmp40.aggr_info;_LL27:{union Cyc_Absyn_AggrInfoU_union _tmp45=_tmp41;
struct Cyc_Absyn_Aggrdecl**_tmp46;struct Cyc_Absyn_Aggrdecl*_tmp47;_LL2F: if((
_tmp45.UnknownAggr).tag != 0)goto _LL31;_LL30: {const char*_tmp53B;void*_tmp53A;(
_tmp53A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp53B="struct type not properly set",_tag_dyneither(_tmp53B,sizeof(char),29))),
_tag_dyneither(_tmp53A,sizeof(void*),0)));}_LL31: if((_tmp45.KnownAggr).tag != 1)
goto _LL2E;_tmp46=(_tmp45.KnownAggr).f1;_tmp47=*_tmp46;_LL32: {struct Cyc_Absyn_Struct_e_struct
_tmp53E;struct Cyc_Absyn_Struct_e_struct*_tmp53D;(void*)(e->r=(void*)((void*)((
_tmp53D=_cycalloc(sizeof(*_tmp53D)),((_tmp53D[0]=((_tmp53E.tag=30,((_tmp53E.f1=
_tmp47->name,((_tmp53E.f2=0,((_tmp53E.f3=des,((_tmp53E.f4=(struct Cyc_Absyn_Aggrdecl*)
_tmp47,_tmp53E)))))))))),_tmp53D))))));}_LL2E:;}goto _LL25;_LL28: if(*((int*)
_tmp3F)!= 7)goto _LL2A;_tmp42=((struct Cyc_Absyn_ArrayType_struct*)_tmp3F)->f1;
_tmp43=(void*)_tmp42.elt_type;_tmp44=_tmp42.tq;_LL29:{struct Cyc_Absyn_Array_e_struct
_tmp541;struct Cyc_Absyn_Array_e_struct*_tmp540;(void*)(e->r=(void*)((void*)((
_tmp540=_cycalloc(sizeof(*_tmp540)),((_tmp540[0]=((_tmp541.tag=28,((_tmp541.f1=
des,_tmp541)))),_tmp540))))));}goto _LL25;_LL2A: if(*((int*)_tmp3F)!= 11)goto _LL2C;
_LL2B:{struct Cyc_Absyn_AnonStruct_e_struct _tmp544;struct Cyc_Absyn_AnonStruct_e_struct*
_tmp543;(void*)(e->r=(void*)((void*)((_tmp543=_cycalloc(sizeof(*_tmp543)),((
_tmp543[0]=((_tmp544.tag=31,((_tmp544.f1=(void*)t,((_tmp544.f2=des,_tmp544)))))),
_tmp543))))));}goto _LL25;_LL2C:;_LL2D:{struct Cyc_Absyn_Array_e_struct _tmp547;
struct Cyc_Absyn_Array_e_struct*_tmp546;(void*)(e->r=(void*)((void*)((_tmp546=
_cycalloc(sizeof(*_tmp546)),((_tmp546[0]=((_tmp547.tag=28,((_tmp547.f1=des,
_tmp547)))),_tmp546))))));}goto _LL25;_LL25:;}}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)es->hd);}}struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(e);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(te,e)){const char*
_tmp54C;void*_tmp54B[2];struct Cyc_String_pa_struct _tmp54A;struct Cyc_String_pa_struct
_tmp549;(_tmp549.tag=0,((_tmp549.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp54A.tag=0,((_tmp54A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((
_tmp54B[0]=& _tmp54A,((_tmp54B[1]=& _tmp549,Cyc_Tcutil_terr(e->loc,((_tmp54C="test of %s has type %s instead of integral or * type",
_tag_dyneither(_tmp54C,sizeof(char),53))),_tag_dyneither(_tmp54B,sizeof(void*),2)))))))))))));}{
void*_tmp56=(void*)e->r;void*_tmp57;struct Cyc_List_List*_tmp58;_LL34: if(*((int*)
_tmp56)!= 3)goto _LL36;_tmp57=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp56)->f1;
_tmp58=((struct Cyc_Absyn_Primop_e_struct*)_tmp56)->f2;_LL35: if(_tmp57 == (void*)5
 || _tmp57 == (void*)6){struct _tuple0 _tmp54D;struct _tuple0 _tmp5A=(_tmp54D.f1=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp58))->hd)->topt))->v,((_tmp54D.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct
Cyc_List_List*)_check_null(_tmp58))->tl))->hd)->topt))->v,_tmp54D)));void*_tmp5B;
void*_tmp5C;void*_tmp5D;void*_tmp5E;_LL39: _tmp5B=_tmp5A.f1;if(_tmp5B <= (void*)4)
goto _LL3B;if(*((int*)_tmp5B)!= 14)goto _LL3B;_tmp5C=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5B)->f1;_tmp5D=_tmp5A.f2;if(_tmp5D <= (void*)4)goto _LL3B;if(*((int*)_tmp5D)!= 
14)goto _LL3B;_tmp5E=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp5D)->f1;
_LL3A: {struct _tuple0*_tmp550;struct Cyc_Tcexp_TestEnv _tmp54F;return(_tmp54F.eq=((
_tmp550=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp550)),((_tmp550->f1=
_tmp5C,((_tmp550->f2=_tmp5E,_tmp550)))))),((_tmp54F.isTrue=_tmp57 == (void*)5,
_tmp54F)));}_LL3B:;_LL3C: goto _LL38;_LL38:;}goto _LL33;_LL36:;_LL37: goto _LL33;
_LL33:;}{struct Cyc_Tcexp_TestEnv _tmp551;return(_tmp551.eq=0,((_tmp551.isTrue=0,
_tmp551)));}}static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
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
Cyc_Absyn_char_typ;goto _LL5F;_LL5F:;}{union Cyc_Absyn_Cnst_union _tmp552;*c=(union
Cyc_Absyn_Cnst_union)(((_tmp552.Char_c).tag=0,(((_tmp552.Char_c).f1=(void*)
_tmp6C,(((_tmp552.Char_c).f2=(char)_tmp69,_tmp552))))));}goto _LL50;_LL53: if(*((
int*)_tmp6B)!= 5)goto _LL55;_tmp6E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp6B)->f1;_tmp6F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp6B)->f2;if((int)
_tmp6F != 1)goto _LL55;_LL54: t=_tmp6E == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;{
union Cyc_Absyn_Cnst_union _tmp553;*c=(union Cyc_Absyn_Cnst_union)(((_tmp553.Short_c).tag=
1,(((_tmp553.Short_c).f1=(void*)_tmp6E,(((_tmp553.Short_c).f2=(short)_tmp69,
_tmp553))))));}goto _LL50;_LL55: if(*((int*)_tmp6B)!= 5)goto _LL57;_tmp70=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp6B)->f1;_tmp71=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp6B)->f2;if((int)_tmp71 != 2)goto _LL57;_LL56: t=_tmp70 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL50;_LL57: if(*((int*)_tmp6B)!= 5)goto _LL59;_tmp72=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp6B)->f1;_tmp73=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp6B)->f2;if((int)_tmp73 != 3)goto _LL59;_LL58: t=_tmp72 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL50;_LL59: if(*((int*)_tmp6B)!= 4)goto _LL5B;if(!(_tmp69
== 0))goto _LL5B;_LL5A:{struct Cyc_Absyn_Const_e_struct _tmp559;union Cyc_Absyn_Cnst_union
_tmp558;struct Cyc_Absyn_Const_e_struct*_tmp557;(void*)(e->r=(void*)((void*)((
_tmp557=_cycalloc(sizeof(*_tmp557)),((_tmp557[0]=((_tmp559.tag=0,((_tmp559.f1=(
union Cyc_Absyn_Cnst_union)(((_tmp558.Null_c).tag=6,_tmp558)),_tmp559)))),_tmp557))))));}{
struct Cyc_List_List*_tmp7B=Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct
_tmp56C;struct Cyc_Core_Opt*_tmp56B;struct Cyc_Absyn_PtrAtts _tmp56A;struct Cyc_Core_Opt*
_tmp569;struct Cyc_Absyn_PtrInfo _tmp568;struct Cyc_Absyn_PointerType_struct*
_tmp567;t=(void*)((_tmp567=_cycalloc(sizeof(*_tmp567)),((_tmp567[0]=((_tmp56C.tag=
4,((_tmp56C.f1=((_tmp568.elt_typ=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ak,((_tmp56B=_cycalloc(sizeof(*_tmp56B)),((_tmp56B->v=_tmp7B,_tmp56B))))),((
_tmp568.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp568.ptr_atts=((_tmp56A.rgn=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,((_tmp569=_cycalloc(
sizeof(*_tmp569)),((_tmp569->v=_tmp7B,_tmp569))))),((_tmp56A.nullable=Cyc_Absyn_true_conref,((
_tmp56A.bounds=Cyc_Absyn_empty_conref(),((_tmp56A.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(),((_tmp56A.ptrloc=0,_tmp56A)))))))))),_tmp568)))))),
_tmp56C)))),_tmp567))));}goto _LL50;}_LL5B: if(*((int*)_tmp6B)!= 18)goto _LL5D;
_tmp74=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp6B)->f1;_LL5C: {struct Cyc_Absyn_ValueofType_struct
_tmp56F;struct Cyc_Absyn_ValueofType_struct*_tmp56E;struct Cyc_Absyn_ValueofType_struct*
_tmp82=(_tmp56E=_cycalloc(sizeof(*_tmp56E)),((_tmp56E[0]=((_tmp56F.tag=17,((
_tmp56F.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp69,0),_tmp56F)))),_tmp56E)));if(
!Cyc_Tcutil_unify(_tmp74,(void*)_tmp82)){{const char*_tmp574;void*_tmp573[2];
struct Cyc_String_pa_struct _tmp572;struct Cyc_String_pa_struct _tmp571;(_tmp571.tag=
0,((_tmp571.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_tmp82)),((_tmp572.tag=0,((_tmp572.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp74)),((_tmp573[0]=& _tmp572,((_tmp573[1]=&
_tmp571,Cyc_Tcutil_terr(loc,((_tmp574="expecting %s but found %s",_tag_dyneither(
_tmp574,sizeof(char),26))),_tag_dyneither(_tmp573,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct Cyc_Absyn_TagType_struct _tmp577;struct Cyc_Absyn_TagType_struct*
_tmp576;t=(void*)((_tmp576=_cycalloc(sizeof(*_tmp576)),((_tmp576[0]=((_tmp577.tag=
18,((_tmp577.f1=(void*)((void*)_tmp82),_tmp577)))),_tmp576))));}goto _LL50;}_LL5D:;
_LL5E: t=_tmp68 == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL50;_LL50:;}
goto _LL3D;_LL4C: if((_tmp62.String_c).tag != 5)goto _LL4E;_tmp6A=(_tmp62.String_c).f1;
_LL4D: {int len=(int)_get_dyneither_size(_tmp6A,sizeof(char));union Cyc_Absyn_Cnst_union
_tmp578;struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)(((
_tmp578.Int_c).tag=2,(((_tmp578.Int_c).f1=(void*)((void*)1),(((_tmp578.Int_c).f2=
len,_tmp578)))))),loc);{struct Cyc_Core_Opt*_tmp579;elen->topt=((_tmp579=
_cycalloc(sizeof(*_tmp579)),((_tmp579->v=(void*)Cyc_Absyn_uint_typ,_tmp579))));}{
struct Cyc_Absyn_Upper_b_struct _tmp57C;struct Cyc_Absyn_Upper_b_struct*_tmp57B;t=
Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(0),(void*)((
_tmp57B=_cycalloc(sizeof(*_tmp57B)),((_tmp57B[0]=((_tmp57C.tag=0,((_tmp57C.f1=
elen,_tmp57C)))),_tmp57B)))),Cyc_Absyn_true_conref);}if(topt != 0){void*_tmp8E=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp8F;struct Cyc_Absyn_Tqual
_tmp90;_LL67: if(_tmp8E <= (void*)4)goto _LL6B;if(*((int*)_tmp8E)!= 7)goto _LL69;
_tmp8F=((struct Cyc_Absyn_ArrayType_struct*)_tmp8E)->f1;_tmp90=_tmp8F.tq;_LL68:
return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmp90,(struct Cyc_Absyn_Exp*)elen,((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),0);_LL69: if(*((int*)
_tmp8E)!= 4)goto _LL6B;_LL6A: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){{struct Cyc_Core_Opt*_tmp57D;e->topt=((_tmp57D=_cycalloc(sizeof(*
_tmp57D)),((_tmp57D->v=(void*)t,_tmp57D))));}Cyc_Tcutil_unchecked_cast(te,e,*
topt,(void*)3);t=*topt;}else{{struct Cyc_Absyn_Upper_b_struct _tmp580;struct Cyc_Absyn_Upper_b_struct*
_tmp57F;t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(void*)((
_tmp57F=_cycalloc(sizeof(*_tmp57F)),((_tmp57F[0]=((_tmp580.tag=0,((_tmp580.f1=
elen,_tmp580)))),_tmp57F)))),Cyc_Absyn_true_conref);}if(!Cyc_Tcutil_unify(*topt,
t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){{struct Cyc_Core_Opt*_tmp581;e->topt=((
_tmp581=_cycalloc(sizeof(*_tmp581)),((_tmp581->v=(void*)t,_tmp581))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);t=*topt;}}goto _LL66;_LL6B:;_LL6C: goto _LL66;_LL66:;}return t;}
_LL4E: if((_tmp62.Null_c).tag != 6)goto _LL3D;_LL4F: {struct Cyc_List_List*_tmp96=
Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct _tmp594;struct
Cyc_Core_Opt*_tmp593;struct Cyc_Absyn_PtrAtts _tmp592;struct Cyc_Core_Opt*_tmp591;
struct Cyc_Absyn_PtrInfo _tmp590;struct Cyc_Absyn_PointerType_struct*_tmp58F;t=(
void*)((_tmp58F=_cycalloc(sizeof(*_tmp58F)),((_tmp58F[0]=((_tmp594.tag=4,((
_tmp594.f1=((_tmp590.elt_typ=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,((
_tmp593=_cycalloc(sizeof(*_tmp593)),((_tmp593->v=_tmp96,_tmp593))))),((_tmp590.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp590.ptr_atts=((_tmp592.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,((_tmp591=_cycalloc(sizeof(*_tmp591)),((
_tmp591->v=_tmp96,_tmp591))))),((_tmp592.nullable=Cyc_Absyn_true_conref,((
_tmp592.bounds=Cyc_Absyn_empty_conref(),((_tmp592.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(),((_tmp592.ptrloc=0,_tmp592)))))))))),_tmp590)))))),
_tmp594)))),_tmp58F))));}goto _LL3D;}_LL3D:;}return t;}static void*Cyc_Tcexp_tcVar(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q,void*b);
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q,void*b){void*_tmp9D=b;struct Cyc_Absyn_Vardecl*_tmp9E;struct
Cyc_Absyn_Fndecl*_tmp9F;struct Cyc_Absyn_Vardecl*_tmpA0;struct Cyc_Absyn_Vardecl*
_tmpA1;struct Cyc_Absyn_Vardecl*_tmpA2;_LL6E: if((int)_tmp9D != 0)goto _LL70;_LL6F: {
const char*_tmp598;void*_tmp597[1];struct Cyc_String_pa_struct _tmp596;return(
_tmp596.tag=0,((_tmp596.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
q)),((_tmp597[0]=& _tmp596,Cyc_Tcexp_expr_err(te,loc,0,((_tmp598="undeclared identifier: %s",
_tag_dyneither(_tmp598,sizeof(char),26))),_tag_dyneither(_tmp597,sizeof(void*),1)))))));}
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
Cyc_Absyn_Nmspace_union _tmp599;(*q).f1=(union Cyc_Absyn_Nmspace_union)(((_tmp599.Loc_n).tag=
0,_tmp599));}return(void*)_tmpA2->type;_LL6D:;}static void Cyc_Tcexp_check_format_args(
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
char*_tmp59C;void*_tmp59B;(_tmp59B=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpAF->hd)->loc,((_tmp59C="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp59C,sizeof(char),49))),_tag_dyneither(_tmp59B,sizeof(void*),0)));}}}}
return;_LL7A:;}if(opt_args != 0){struct Cyc_List_List*_tmpB3=(struct Cyc_List_List*)
opt_args->v;for(0;desc_types != 0  && _tmpB3 != 0;(desc_types=desc_types->tl,_tmpB3=
_tmpB3->tl)){void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
_tmpB3->hd;Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){{
const char*_tmp5A1;void*_tmp5A0[2];struct Cyc_String_pa_struct _tmp59F;struct Cyc_String_pa_struct
_tmp59E;(_tmp59E.tag=0,((_tmp59E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp59F.tag=0,((_tmp59F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp5A0[0]=& _tmp59F,((_tmp5A0[1]=& _tmp59E,Cyc_Tcutil_terr(e->loc,((_tmp5A1="descriptor has type \n%s\n but argument has type \n%s",
_tag_dyneither(_tmp5A1,sizeof(char),51))),_tag_dyneither(_tmp5A0,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct _RegionHandle*_tmpB8=Cyc_Tcenv_get_fnrgn(te);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmpB8,t) && !Cyc_Tcutil_is_noalias_path(
_tmpB8,e)){const char*_tmp5A4;void*_tmp5A3;(_tmp5A3=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpB3->hd)->loc,((_tmp5A4="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp5A4,sizeof(char),49))),_tag_dyneither(_tmp5A3,sizeof(void*),0)));}}}
if(desc_types != 0){const char*_tmp5A7;void*_tmp5A6;(_tmp5A6=0,Cyc_Tcutil_terr(fmt->loc,((
_tmp5A7="too few arguments",_tag_dyneither(_tmp5A7,sizeof(char),18))),
_tag_dyneither(_tmp5A6,sizeof(void*),0)));}if(_tmpB3 != 0){const char*_tmp5AA;void*
_tmp5A9;(_tmp5A9=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpB3->hd)->loc,((
_tmp5AA="too many arguments",_tag_dyneither(_tmp5AA,sizeof(char),19))),
_tag_dyneither(_tmp5A9,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmpBF=p;_LL82: if((
int)_tmpBF != 0)goto _LL84;_LL83: goto _LL85;_LL84: if((int)_tmpBF != 2)goto _LL86;
_LL85: if(!Cyc_Tcutil_is_numeric(e)){const char*_tmp5AE;void*_tmp5AD[1];struct Cyc_String_pa_struct
_tmp5AC;(_tmp5AC.tag=0,((_tmp5AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp5AD[0]=& _tmp5AC,Cyc_Tcutil_terr(loc,((_tmp5AE="expecting arithmetic type but found %s",
_tag_dyneither(_tmp5AE,sizeof(char),39))),_tag_dyneither(_tmp5AD,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;_LL86: if((int)_tmpBF
!= 11)goto _LL88;_LL87: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(
te,e)){const char*_tmp5B2;void*_tmp5B1[1];struct Cyc_String_pa_struct _tmp5B0;(
_tmp5B0.tag=0,((_tmp5B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp5B1[0]=& _tmp5B0,Cyc_Tcutil_terr(loc,((_tmp5B2="expecting integral or * type but found %s",
_tag_dyneither(_tmp5B2,sizeof(char),42))),_tag_dyneither(_tmp5B1,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;_LL88: if((int)_tmpBF != 12)goto _LL8A;_LL89: if(!Cyc_Tcutil_is_integral(
e)){const char*_tmp5B6;void*_tmp5B5[1];struct Cyc_String_pa_struct _tmp5B4;(_tmp5B4.tag=
0,((_tmp5B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp5B5[0]=& _tmp5B4,Cyc_Tcutil_terr(loc,((_tmp5B6="expecting integral type but found %s",
_tag_dyneither(_tmp5B6,sizeof(char),37))),_tag_dyneither(_tmp5B5,sizeof(void*),1)))))));}
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
_LL9A:;_LL9B: {const char*_tmp5B9;void*_tmp5B8;(_tmp5B8=0,Cyc_Tcutil_terr(loc,((
_tmp5B9="can't apply numelts to pointer/array of abstract length",_tag_dyneither(
_tmp5B9,sizeof(char),56))),_tag_dyneither(_tmp5B8,sizeof(void*),0)));}_LL95:;}
goto _LL8E;_LL93:;_LL94: {const char*_tmp5BD;void*_tmp5BC[1];struct Cyc_String_pa_struct
_tmp5BB;(_tmp5BB.tag=0,((_tmp5BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp5BC[0]=& _tmp5BB,Cyc_Tcutil_terr(loc,((_tmp5BD="numelts requires pointer or array type, not %s",
_tag_dyneither(_tmp5BD,sizeof(char),47))),_tag_dyneither(_tmp5BC,sizeof(void*),1)))))));}
_LL8E:;}return Cyc_Absyn_uint_typ;_LL8C:;_LL8D: {const char*_tmp5C0;void*_tmp5BF;(
_tmp5BF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp5C0="Non-unary primop",_tag_dyneither(_tmp5C0,sizeof(char),17))),
_tag_dyneither(_tmp5BF,sizeof(void*),0)));}_LL81:;}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*));static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
if(!checker(e1)){{const char*_tmp5C4;void*_tmp5C3[1];struct Cyc_String_pa_struct
_tmp5C2;(_tmp5C2.tag=0,((_tmp5C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((
_tmp5C3[0]=& _tmp5C2,Cyc_Tcutil_terr(e1->loc,((_tmp5C4="type %s cannot be used here",
_tag_dyneither(_tmp5C4,sizeof(char),28))),_tag_dyneither(_tmp5C3,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}if(!checker(e2)){{
const char*_tmp5C8;void*_tmp5C7[1];struct Cyc_String_pa_struct _tmp5C6;(_tmp5C6.tag=
0,((_tmp5C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((_tmp5C7[0]=& _tmp5C6,
Cyc_Tcutil_terr(e2->loc,((_tmp5C8="type %s cannot be used here",_tag_dyneither(
_tmp5C8,sizeof(char),28))),_tag_dyneither(_tmp5C7,sizeof(void*),1)))))));}return
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
_tmp5CB;void*_tmp5CA;(_tmp5CA=0,Cyc_Tcutil_terr(e1->loc,((_tmp5CB="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp5CB,sizeof(char),50))),_tag_dyneither(_tmp5CA,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp5CE;void*_tmp5CD;(_tmp5CD=0,
Cyc_Tcutil_terr(e1->loc,((_tmp5CE="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp5CE,sizeof(char),54))),_tag_dyneither(_tmp5CD,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){const char*_tmp5D2;void*_tmp5D1[1];struct
Cyc_String_pa_struct _tmp5D0;(_tmp5D0.tag=0,((_tmp5D0.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp5D1[0]=& _tmp5D0,Cyc_Tcutil_terr(
e2->loc,((_tmp5D2="expecting int but found %s",_tag_dyneither(_tmp5D2,sizeof(
char),27))),_tag_dyneither(_tmp5D1,sizeof(void*),1)))))));}_tmpE4=Cyc_Absyn_compress_conref(
_tmpE4);{union Cyc_Absyn_Constraint_union _tmpED=_tmpE4->v;void*_tmpEE;void*_tmpEF;
struct Cyc_Absyn_Exp*_tmpF0;_LLA2: if((_tmpED.Eq_constr).tag != 0)goto _LLA4;_tmpEE=(
_tmpED.Eq_constr).f1;if((int)_tmpEE != 0)goto _LLA4;_LLA3: return t1;_LLA4: if((
_tmpED.Eq_constr).tag != 0)goto _LLA6;_tmpEF=(_tmpED.Eq_constr).f1;if(_tmpEF <= (
void*)1)goto _LLA6;if(*((int*)_tmpEF)!= 0)goto _LLA6;_tmpF0=((struct Cyc_Absyn_Upper_b_struct*)
_tmpEF)->f1;_LLA5: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpE5)){const char*_tmp5D5;void*_tmp5D4;(_tmp5D4=0,Cyc_Tcutil_warn(e1->loc,((
_tmp5D5="pointer arithmetic on thin, zero-terminated pointer may be expensive.",
_tag_dyneither(_tmp5D5,sizeof(char),70))),_tag_dyneither(_tmp5D4,sizeof(void*),0)));}{
struct Cyc_Absyn_PointerType_struct _tmp5DF;struct Cyc_Absyn_PtrAtts _tmp5DE;struct
Cyc_Absyn_PtrInfo _tmp5DD;struct Cyc_Absyn_PointerType_struct*_tmp5DC;struct Cyc_Absyn_PointerType_struct*
_tmpF3=(_tmp5DC=_cycalloc(sizeof(*_tmp5DC)),((_tmp5DC[0]=((_tmp5DF.tag=4,((
_tmp5DF.f1=((_tmp5DD.elt_typ=(void*)_tmpDF,((_tmp5DD.elt_tq=_tmpE0,((_tmp5DD.ptr_atts=((
_tmp5DE.rgn=(void*)_tmpE2,((_tmp5DE.nullable=Cyc_Absyn_true_conref,((_tmp5DE.bounds=
Cyc_Absyn_bounds_dyneither_conref,((_tmp5DE.zero_term=_tmpE5,((_tmp5DE.ptrloc=0,
_tmp5DE)))))))))),_tmp5DD)))))),_tmp5DF)))),_tmp5DC)));Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmpF3,(void*)3);return(void*)_tmpF3;}_LLA6:;_LLA7:{union Cyc_Absyn_Constraint_union
_tmp5E0;_tmpE4->v=(union Cyc_Absyn_Constraint_union)(((_tmp5E0.Eq_constr).tag=0,(((
_tmp5E0.Eq_constr).f1=(void*)((void*)0),_tmp5E0))));}return t1;_LLA1:;}_LL9F:;
_LLA0: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LL9C:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;
void*t1_elt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){if(!Cyc_Tcutil_unify(t1,t2)){{const char*_tmp5E5;void*_tmp5E4[2];struct Cyc_String_pa_struct
_tmp5E3;struct Cyc_String_pa_struct _tmp5E2;(_tmp5E2.tag=0,((_tmp5E2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v)),((_tmp5E3.tag=0,((_tmp5E3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp5E4[0]=& _tmp5E3,((_tmp5E4[1]=& _tmp5E2,Cyc_Tcutil_terr(
e1->loc,((_tmp5E5="pointer arithmetic on values of different types (%s != %s)",
_tag_dyneither(_tmp5E5,sizeof(char),59))),_tag_dyneither(_tmp5E4,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),(void*)1)){const char*_tmp5E8;void*_tmp5E7;(_tmp5E7=0,
Cyc_Tcutil_terr(e1->loc,((_tmp5E8="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp5E8,sizeof(char),50))),_tag_dyneither(_tmp5E7,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp5EB;void*_tmp5EA;(_tmp5EA=0,
Cyc_Tcutil_terr(e1->loc,((_tmp5EB="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp5EB,sizeof(char),54))),_tag_dyneither(_tmp5EA,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){{const char*_tmp5F0;void*_tmp5EF[2];struct
Cyc_String_pa_struct _tmp5EE;struct Cyc_String_pa_struct _tmp5ED;(_tmp5ED.tag=0,((
_tmp5ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp5EE.tag=0,((_tmp5EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp5EF[0]=& _tmp5EE,((_tmp5EF[1]=& _tmp5ED,Cyc_Tcutil_terr(
e2->loc,((_tmp5F0="expecting either %s or int but found %s",_tag_dyneither(
_tmp5F0,sizeof(char),40))),_tag_dyneither(_tmp5EF,sizeof(void*),2)))))))))))));}
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
_tuple0 _tmp5F1;struct _tuple0 _tmp106=(_tmp5F1.f1=Cyc_Tcutil_compress(t1),((
_tmp5F1.f2=Cyc_Tcutil_compress(t2),_tmp5F1)));void*_tmp107;struct Cyc_Absyn_PtrInfo
_tmp108;void*_tmp109;void*_tmp10A;struct Cyc_Absyn_PtrInfo _tmp10B;void*_tmp10C;
void*_tmp10D;void*_tmp10E;_LLA9: _tmp107=_tmp106.f1;if(_tmp107 <= (void*)4)goto
_LLAB;if(*((int*)_tmp107)!= 4)goto _LLAB;_tmp108=((struct Cyc_Absyn_PointerType_struct*)
_tmp107)->f1;_tmp109=(void*)_tmp108.elt_typ;_tmp10A=_tmp106.f2;if(_tmp10A <= (
void*)4)goto _LLAB;if(*((int*)_tmp10A)!= 4)goto _LLAB;_tmp10B=((struct Cyc_Absyn_PointerType_struct*)
_tmp10A)->f1;_tmp10C=(void*)_tmp10B.elt_typ;_LLAA: if(Cyc_Tcutil_unify(_tmp109,
_tmp10C))return Cyc_Absyn_sint_typ;goto _LLA8;_LLAB: _tmp10D=_tmp106.f1;if(_tmp10D
<= (void*)4)goto _LLAD;if(*((int*)_tmp10D)!= 14)goto _LLAD;_tmp10E=_tmp106.f2;if(
_tmp10E <= (void*)4)goto _LLAD;if(*((int*)_tmp10E)!= 14)goto _LLAD;_LLAC: return Cyc_Absyn_sint_typ;
_LLAD:;_LLAE: goto _LLA8;_LLA8:;}{const char*_tmp5F6;void*_tmp5F5[2];struct Cyc_String_pa_struct
_tmp5F4;struct Cyc_String_pa_struct _tmp5F3;(_tmp5F3.tag=0,((_tmp5F3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp5F4.tag=
0,((_tmp5F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp5F5[0]=& _tmp5F4,((_tmp5F5[1]=& _tmp5F3,Cyc_Tcutil_terr(loc,((_tmp5F6="comparison not allowed between %s and %s",
_tag_dyneither(_tmp5F6,sizeof(char),41))),_tag_dyneither(_tmp5F5,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcEqPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){if(Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2)== Cyc_Absyn_sint_typ)
return Cyc_Absyn_sint_typ;{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);{struct _tuple0 _tmp5F7;struct _tuple0 _tmp114=(_tmp5F7.f1=
t1,((_tmp5F7.f2=t2,_tmp5F7)));void*_tmp115;void*_tmp116;void*_tmp117;void*
_tmp118;_LLB0: _tmp115=_tmp114.f1;if(_tmp115 <= (void*)4)goto _LLB2;if(*((int*)
_tmp115)!= 14)goto _LLB2;_tmp116=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp115)->f1;_tmp117=_tmp114.f2;if(_tmp117 <= (void*)4)goto _LLB2;if(*((int*)
_tmp117)!= 14)goto _LLB2;_tmp118=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp117)->f1;_LLB1: return Cyc_Absyn_sint_typ;_LLB2:;_LLB3: goto _LLAF;_LLAF:;}{
const char*_tmp5FC;void*_tmp5FB[2];struct Cyc_String_pa_struct _tmp5FA;struct Cyc_String_pa_struct
_tmp5F9;(_tmp5F9.tag=0,((_tmp5F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp5FA.tag=0,((_tmp5FA.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp5FB[0]=& _tmp5FA,((_tmp5FB[
1]=& _tmp5F9,Cyc_Tcutil_terr(loc,((_tmp5FC="comparison not allowed between %s and %s",
_tag_dyneither(_tmp5FC,sizeof(char),41))),_tag_dyneither(_tmp5FB,sizeof(void*),2)))))))))))));}
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
char*_tmp5FF;void*_tmp5FE;(_tmp5FE=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5FF="bad binary primop",_tag_dyneither(
_tmp5FF,sizeof(char),18))),_tag_dyneither(_tmp5FE,sizeof(void*),0)));}_LLB4:;}
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_List_List*es);static void*Cyc_Tcexp_tcPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_List_List*es){if(p == (void*)2  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
es)== 1)return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd);Cyc_Tcexp_tcExpList(te,es);{void*t;switch(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLD9: {const char*_tmp602;void*
_tmp601;return(_tmp601=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp602="primitive operator has 0 arguments",
_tag_dyneither(_tmp602,sizeof(char),35))),_tag_dyneither(_tmp601,sizeof(void*),0)));}
case 1: _LLDA: t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLDB: t=Cyc_Tcexp_tcBinPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(es))->tl))->hd);break;default: _LLDC: {const char*_tmp605;void*_tmp604;
return(_tmp604=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp605="primitive operator has > 2 arguments",
_tag_dyneither(_tmp605,sizeof(char),37))),_tag_dyneither(_tmp604,sizeof(void*),0)));}}
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
_tmp12E);_tmp13D=_tmp13C.f1;_tmp13E=_tmp13C.f2;if(!_tmp13E){{const char*_tmp608;
void*_tmp607;(_tmp607=0,Cyc_Tcutil_terr(e->loc,((_tmp608="tuple projection cannot use sizeof or offsetof",
_tag_dyneither(_tmp608,sizeof(char),47))),_tag_dyneither(_tmp607,sizeof(void*),0)));}
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
te,_tmp135);return;_LLF3:;_LLF4: goto _LLDE;_LLDE:;}{const char*_tmp60C;void*
_tmp60B[1];struct Cyc_String_pa_struct _tmp60A;(_tmp60A.tag=0,((_tmp60A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp60B[0]=&
_tmp60A,Cyc_Tcutil_terr(e->loc,((_tmp60C="attempt to write a const location: %s",
_tag_dyneither(_tmp60C,sizeof(char),38))),_tag_dyneither(_tmp60B,sizeof(void*),1)))))));}}
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,void*i);static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,void*i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(e)){const char*
_tmp60F;void*_tmp60E;return(_tmp60E=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp60F="increment/decrement of non-lvalue",
_tag_dyneither(_tmp60F,sizeof(char),34))),_tag_dyneither(_tmp60E,sizeof(void*),0)));}
Cyc_Tcexp_check_writable(te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v;if(!Cyc_Tcutil_is_numeric(e)){void*telt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == (void*)0  || i == (
void*)1)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(void*)1)){const char*
_tmp612;void*_tmp611;(_tmp611=0,Cyc_Tcutil_terr(e->loc,((_tmp612="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp612,sizeof(char),50))),_tag_dyneither(_tmp611,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t)){const char*_tmp615;void*_tmp614;(_tmp614=0,
Cyc_Tcutil_terr(e->loc,((_tmp615="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp615,sizeof(char),54))),_tag_dyneither(_tmp614,sizeof(void*),0)));}}
else{const char*_tmp619;void*_tmp618[1];struct Cyc_String_pa_struct _tmp617;(
_tmp617.tag=0,((_tmp617.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp618[0]=& _tmp617,Cyc_Tcutil_terr(e->loc,((_tmp619="expecting arithmetic or ? type but found %s",
_tag_dyneither(_tmp619,sizeof(char),44))),_tag_dyneither(_tmp618,sizeof(void*),1)))))));}}
return t;}}static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3);static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3){struct _tuple0*_tmp169;int _tmp16A;const char*_tmp61A;struct Cyc_Tcexp_TestEnv
_tmp168=Cyc_Tcexp_tcTest(te,e1,((_tmp61A="conditional expression",_tag_dyneither(
_tmp61A,sizeof(char),23))));_tmp169=_tmp168.eq;_tmp16A=_tmp168.isTrue;Cyc_Tcexp_tcExp(
te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_List_List _tmp61B;
struct Cyc_List_List _tmp16B=(_tmp61B.hd=e3,((_tmp61B.tl=0,_tmp61B)));struct Cyc_List_List
_tmp61C;struct Cyc_List_List _tmp16C=(_tmp61C.hd=e2,((_tmp61C.tl=(struct Cyc_List_List*)&
_tmp16B,_tmp61C)));if(!Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)&
_tmp16C)){{const char*_tmp621;void*_tmp620[2];struct Cyc_String_pa_struct _tmp61F;
struct Cyc_String_pa_struct _tmp61E;(_tmp61E.tag=0,((_tmp61E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e3->topt))->v)),((_tmp61F.tag=0,((_tmp61F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v)),((_tmp620[0]=& _tmp61F,((_tmp620[1]=& _tmp61E,Cyc_Tcutil_terr(
loc,((_tmp621="conditional clause types do not match: %s != %s",_tag_dyneither(
_tmp621,sizeof(char),48))),_tag_dyneither(_tmp620,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t;}}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{const char*_tmp622;Cyc_Tcexp_tcTest(
te,e1,((_tmp622="logical-and expression",_tag_dyneither(_tmp622,sizeof(char),23))));}{
const char*_tmp623;Cyc_Tcexp_tcTest(te,e2,((_tmp623="logical-and expression",
_tag_dyneither(_tmp623,sizeof(char),23))));}return Cyc_Absyn_sint_typ;}static void*
Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp624;Cyc_Tcexp_tcTest(te,e1,((_tmp624="logical-or expression",
_tag_dyneither(_tmp624,sizeof(char),22))));}{const char*_tmp625;Cyc_Tcexp_tcTest(
te,e2,((_tmp625="logical-or expression",_tag_dyneither(_tmp625,sizeof(char),22))));}
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
_LL11F:{const char*_tmp628;void*_tmp627;(_tmp627=0,Cyc_Tcutil_terr(loc,((_tmp628="cannot assign to an array",
_tag_dyneither(_tmp628,sizeof(char),26))),_tag_dyneither(_tmp627,sizeof(void*),0)));}
goto _LL11D;_LL120:;_LL121: goto _LL11D;_LL11D:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),(void*)1)){const char*_tmp62B;void*_tmp62A;(_tmp62A=0,Cyc_Tcutil_terr(loc,((
_tmp62B="type is abstract (can't determine size).",_tag_dyneither(_tmp62B,
sizeof(char),41))),_tag_dyneither(_tmp62A,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(
e1)){const char*_tmp62E;void*_tmp62D;return(_tmp62D=0,Cyc_Tcexp_expr_err(te,loc,
topt,((_tmp62E="assignment to non-lvalue",_tag_dyneither(_tmp62E,sizeof(char),25))),
_tag_dyneither(_tmp62D,sizeof(void*),0)));}Cyc_Tcexp_check_writable(te,e1);if(po
== 0){struct _RegionHandle*_tmp17F=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp17F,t2) && !Cyc_Tcutil_is_noalias_path(_tmp17F,e2)){const char*_tmp631;void*
_tmp630;(_tmp630=0,Cyc_Tcutil_terr(e2->loc,((_tmp631="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp631,sizeof(char),49))),_tag_dyneither(_tmp630,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){const char*_tmp636;void*_tmp635[2];struct
Cyc_String_pa_struct _tmp634;struct Cyc_String_pa_struct _tmp633;void*_tmp182=(
_tmp633.tag=0,((_tmp633.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp634.tag=0,((_tmp634.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp635[0]=& _tmp634,((_tmp635[1]=& _tmp633,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp636="type mismatch: %s != %s",_tag_dyneither(_tmp636,sizeof(
char),24))),_tag_dyneither(_tmp635,sizeof(void*),2)))))))))))));Cyc_Tcutil_unify(
t1,t2);Cyc_Tcutil_explain_failure();return _tmp182;}}else{void*_tmp187=(void*)po->v;
void*_tmp188=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp187,e1,e2);if(!(Cyc_Tcutil_unify(
_tmp188,t1) || Cyc_Tcutil_coerceable(_tmp188) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp63B;void*_tmp63A[2];struct Cyc_String_pa_struct _tmp639;struct Cyc_String_pa_struct
_tmp638;void*_tmp189=(_tmp638.tag=0,((_tmp638.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp639.tag=0,((_tmp639.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp63A[0]=&
_tmp639,((_tmp63A[1]=& _tmp638,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp63B="Cannot use this operator in an assignment when the arguments have types %s and %s",
_tag_dyneither(_tmp63B,sizeof(char),82))),_tag_dyneither(_tmp63A,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(_tmp188,t1);Cyc_Tcutil_explain_failure();return _tmp189;}return
_tmp188;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,struct
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
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp18E=Cyc_Tcutil_compress(
t1);void*_tmp18F;void*_tmp190;_LL123: if((int)_tmp18E != 1)goto _LL125;_LL124: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ,(void*)1);t1=dbl_typ;goto _LL122;_LL125: if(_tmp18E <= (void*)4)goto
_LL129;if(*((int*)_tmp18E)!= 5)goto _LL127;_tmp18F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp18E)->f2;if((int)_tmp18F != 0)goto _LL127;_LL126: goto _LL128;_LL127: if(*((int*)
_tmp18E)!= 5)goto _LL129;_tmp190=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp18E)->f2;
if((int)_tmp190 != 1)goto _LL129;_LL128: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);t1=Cyc_Absyn_sint_typ;goto _LL122;_LL129:;_LL12A: goto _LL122;_LL122:;}
for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp192;struct Cyc_List_List*
_tmp193;struct Cyc_Position_Segment*_tmp194;void*_tmp195;struct Cyc_Absyn_Tunionfield
_tmp191=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_tmp192=_tmp191.name;_tmp193=
_tmp191.typs;_tmp194=_tmp191.loc;_tmp195=(void*)_tmp191.sc;if(_tmp193 == 0  || 
_tmp193->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple9*)_tmp193->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)fields->hd);}}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp197;struct Cyc_List_List*_tmp198;struct Cyc_Position_Segment*
_tmp199;void*_tmp19A;struct Cyc_Absyn_Tunionfield _tmp196=*((struct Cyc_Absyn_Tunionfield*)
fields->hd);_tmp197=_tmp196.name;_tmp198=_tmp196.typs;_tmp199=_tmp196.loc;
_tmp19A=(void*)_tmp196.sc;if(_tmp198 == 0  || _tmp198->tl != 0)continue;{void*t2=
Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple9*)_tmp198->hd)).f2);if(Cyc_Tcutil_coerce_arg(
te,e,t2))return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)
fields->hd);}}{const char*_tmp640;void*_tmp63F[2];struct Cyc_String_pa_struct
_tmp63E;struct Cyc_String_pa_struct _tmp63D;(_tmp63D.tag=0,((_tmp63D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp63E.tag=
0,((_tmp63E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
tu)),((_tmp63F[0]=& _tmp63E,((_tmp63F[1]=& _tmp63D,Cyc_Tcutil_terr(e->loc,((
_tmp640="can't find a field in %s to inject a value of type %s",_tag_dyneither(
_tmp640,sizeof(char),54))),_tag_dyneither(_tmp63F,sizeof(void*),2)))))))))))));}
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
_tmp645;void*_tmp644[2];struct Cyc_String_pa_struct _tmp643;struct Cyc_String_pa_struct
_tmp642;(_tmp642.tag=0,((_tmp642.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp643.tag=0,((_tmp643.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp644[0]=& _tmp643,((_tmp644[1]=& _tmp642,Cyc_Tcutil_terr(
e1->loc,((_tmp645="actual argument has type \n\t%s\n but formal has type \n\t%s.",
_tag_dyneither(_tmp645,sizeof(char),57))),_tag_dyneither(_tmp644,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1A1,t2) && !Cyc_Tcutil_is_noalias_path(
_tmp1A1,e1)){const char*_tmp648;void*_tmp647;(_tmp647=0,Cyc_Tcutil_terr(e1->loc,((
_tmp648="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp648,sizeof(char),49))),_tag_dyneither(_tmp647,sizeof(void*),0)));}_tmp1A0=
_tmp1A0->tl;_tmp1B1=_tmp1B1->tl;}{int args_already_checked=0;{struct Cyc_List_List*
a=_tmp1B5;for(0;a != 0;a=a->tl){void*_tmp1BC=(void*)a->hd;void*_tmp1BD;int _tmp1BE;
int _tmp1BF;_LL136: if(_tmp1BC <= (void*)17)goto _LL138;if(*((int*)_tmp1BC)!= 3)goto
_LL138;_tmp1BD=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp1BC)->f1;_tmp1BE=((
struct Cyc_Absyn_Format_att_struct*)_tmp1BC)->f2;_tmp1BF=((struct Cyc_Absyn_Format_att_struct*)
_tmp1BC)->f3;_LL137:{struct _handler_cons _tmp1C0;_push_handler(& _tmp1C0);{int
_tmp1C2=0;if(setjmp(_tmp1C0.handler))_tmp1C2=1;if(!_tmp1C2){{struct Cyc_Absyn_Exp*
_tmp1C3=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,
_tmp1BE - 1);struct Cyc_Core_Opt*fmt_args;if(_tmp1BF == 0)fmt_args=0;else{struct Cyc_Core_Opt*
_tmp649;fmt_args=((_tmp649=_cycalloc(sizeof(*_tmp649)),((_tmp649->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp1BF - 1),_tmp649))));}
args_already_checked=1;{struct _RegionHandle*_tmp1C5=Cyc_Tcenv_get_fnrgn(te_orig);
void*_tmp1C6=_tmp1BD;_LL13B: if((int)_tmp1C6 != 0)goto _LL13D;_LL13C: Cyc_Tcexp_check_format_args(
_tmp1A2,_tmp1C3,fmt_args,_tmp1C5,Cyc_Formatstr_get_format_typs);goto _LL13A;
_LL13D: if((int)_tmp1C6 != 1)goto _LL13A;_LL13E: Cyc_Tcexp_check_format_args(_tmp1A2,
_tmp1C3,fmt_args,_tmp1C5,Cyc_Formatstr_get_scanf_typs);goto _LL13A;_LL13A:;}};
_pop_handler();}else{void*_tmp1C1=(void*)_exn_thrown;void*_tmp1C8=_tmp1C1;_LL140:
if(_tmp1C8 != Cyc_List_Nth)goto _LL142;_LL141:{const char*_tmp64C;void*_tmp64B;(
_tmp64B=0,Cyc_Tcutil_terr(loc,((_tmp64C="bad format arguments",_tag_dyneither(
_tmp64C,sizeof(char),21))),_tag_dyneither(_tmp64B,sizeof(void*),0)));}goto _LL13F;
_LL142:;_LL143:(void)_throw(_tmp1C8);_LL13F:;}}}goto _LL135;_LL138:;_LL139: goto
_LL135;_LL135:;}}if(_tmp1B1 != 0){const char*_tmp64F;void*_tmp64E;(_tmp64E=0,Cyc_Tcutil_terr(
loc,((_tmp64F="too few arguments for function",_tag_dyneither(_tmp64F,sizeof(
char),31))),_tag_dyneither(_tmp64E,sizeof(void*),0)));}else{if((_tmp1A0 != 0  || 
_tmp1B2) || _tmp1B3 != 0){if(_tmp1B2)for(0;_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){Cyc_Tcexp_tcExp(
_tmp1A2,0,(struct Cyc_Absyn_Exp*)_tmp1A0->hd);}else{if(_tmp1B3 == 0){const char*
_tmp652;void*_tmp651;(_tmp651=0,Cyc_Tcutil_terr(loc,((_tmp652="too many arguments for function",
_tag_dyneither(_tmp652,sizeof(char),32))),_tag_dyneither(_tmp651,sizeof(void*),0)));}
else{void*_tmp1D0;int _tmp1D1;struct Cyc_Absyn_VarargInfo _tmp1CF=*_tmp1B3;_tmp1D0=(
void*)_tmp1CF.type;_tmp1D1=_tmp1CF.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp653;struct Cyc_Absyn_VarargCallInfo*_tmp1D2=(_tmp653=_cycalloc(sizeof(*
_tmp653)),((_tmp653->num_varargs=0,((_tmp653->injectors=0,((_tmp653->vai=(struct
Cyc_Absyn_VarargInfo*)_tmp1B3,_tmp653)))))));*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp1D2;if(!_tmp1D1)for(0;_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp1A0->hd;++ _tmp1D2->num_varargs;Cyc_Tcexp_tcExp(
_tmp1A2,(void**)& _tmp1D0,e1);if(!Cyc_Tcutil_coerce_arg(_tmp1A2,e1,_tmp1D0)){{
const char*_tmp658;void*_tmp657[2];struct Cyc_String_pa_struct _tmp656;struct Cyc_String_pa_struct
_tmp655;(_tmp655.tag=0,((_tmp655.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((
_tmp656.tag=0,((_tmp656.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp1D0)),((_tmp657[0]=& _tmp656,((_tmp657[1]=& _tmp655,Cyc_Tcutil_terr(loc,((
_tmp658="vararg requires type %s but argument has type %s",_tag_dyneither(
_tmp658,sizeof(char),49))),_tag_dyneither(_tmp657,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1A1,
_tmp1D0) && !Cyc_Tcutil_is_noalias_path(_tmp1A1,e1)){const char*_tmp65B;void*
_tmp65A;(_tmp65A=0,Cyc_Tcutil_terr(e1->loc,((_tmp65B="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp65B,sizeof(char),49))),_tag_dyneither(_tmp65A,sizeof(void*),0)));}}
else{void*_tmp1D9=Cyc_Tcutil_compress(_tmp1D0);struct Cyc_Absyn_TunionInfo _tmp1DA;
union Cyc_Absyn_TunionInfoU_union _tmp1DB;struct Cyc_Absyn_Tuniondecl**_tmp1DC;
struct Cyc_Absyn_Tuniondecl*_tmp1DD;struct Cyc_List_List*_tmp1DE;struct Cyc_Core_Opt*
_tmp1DF;_LL145: if(_tmp1D9 <= (void*)4)goto _LL147;if(*((int*)_tmp1D9)!= 2)goto
_LL147;_tmp1DA=((struct Cyc_Absyn_TunionType_struct*)_tmp1D9)->f1;_tmp1DB=_tmp1DA.tunion_info;
if((((((struct Cyc_Absyn_TunionType_struct*)_tmp1D9)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL147;_tmp1DC=(_tmp1DB.KnownTunion).f1;_tmp1DD=*_tmp1DC;_tmp1DE=
_tmp1DA.targs;_tmp1DF=_tmp1DA.rgn;_LL146: {struct Cyc_Absyn_Tuniondecl*_tmp1E0=*
Cyc_Tcenv_lookup_tuniondecl(_tmp1A2,loc,_tmp1DD->name);struct Cyc_List_List*
fields=0;if(_tmp1E0->fields == 0){const char*_tmp65F;void*_tmp65E[1];struct Cyc_String_pa_struct
_tmp65D;(_tmp65D.tag=0,((_tmp65D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp1D0)),((_tmp65E[0]=& _tmp65D,Cyc_Tcutil_terr(loc,((
_tmp65F="can't inject into %s",_tag_dyneither(_tmp65F,sizeof(char),21))),
_tag_dyneither(_tmp65E,sizeof(void*),1)))))));}else{fields=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1E0->fields))->v;}if(!Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1DF))->v,Cyc_Tcenv_curr_rgn(_tmp1A2))){const
char*_tmp662;void*_tmp661;(_tmp661=0,Cyc_Tcutil_terr(loc,((_tmp662="bad region for injected varargs",
_tag_dyneither(_tmp662,sizeof(char),32))),_tag_dyneither(_tmp661,sizeof(void*),0)));}{
struct _RegionHandle*_tmp1E6=Cyc_Tcenv_get_fnrgn(_tmp1A2);{struct Cyc_List_List*
_tmp1E7=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp1E6,_tmp1E6,
_tmp1E0->tvs,_tmp1DE);for(0;_tmp1A0 != 0;_tmp1A0=_tmp1A0->tl){++ _tmp1D2->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp1A0->hd;if(!
args_already_checked){Cyc_Tcexp_tcExp(_tmp1A2,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1E6,(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(
_tmp1E6,e1)){const char*_tmp665;void*_tmp664;(_tmp664=0,Cyc_Tcutil_terr(e1->loc,((
_tmp665="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp665,sizeof(char),49))),_tag_dyneither(_tmp664,sizeof(void*),0)));}}{struct
Cyc_Absyn_Tunionfield*_tmp1EA=Cyc_Tcexp_tcInjection(_tmp1A2,e1,_tmp1D0,_tmp1E6,
_tmp1E7,fields);if(_tmp1EA != 0){struct Cyc_List_List*_tmp666;_tmp1D2->injectors=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1D2->injectors,((_tmp666=_cycalloc(sizeof(*_tmp666)),((_tmp666->hd=(struct
Cyc_Absyn_Tunionfield*)_tmp1EA,((_tmp666->tl=0,_tmp666)))))));}}}}}goto _LL144;}}
_LL147:;_LL148:{const char*_tmp669;void*_tmp668;(_tmp668=0,Cyc_Tcutil_terr(loc,((
_tmp669="bad inject vararg type",_tag_dyneither(_tmp669,sizeof(char),23))),
_tag_dyneither(_tmp668,sizeof(void*),0)));}goto _LL144;_LL144:;}}}}}}Cyc_Tcenv_check_effect_accessible(
_tmp1A2,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1AF))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp1A2,loc,_tmp1B4);return _tmp1B0;}_LL133:;_LL134: {const char*_tmp66C;void*
_tmp66B;return(_tmp66B=0,Cyc_Tcexp_expr_err(_tmp1A2,loc,topt,((_tmp66C="expected pointer to function",
_tag_dyneither(_tmp66C,sizeof(char),29))),_tag_dyneither(_tmp66B,sizeof(void*),0)));}
_LL130:;}_LL12E:;_LL12F: {const char*_tmp66F;void*_tmp66E;return(_tmp66E=0,Cyc_Tcexp_expr_err(
_tmp1A2,loc,topt,((_tmp66F="expected pointer to function",_tag_dyneither(_tmp66F,
sizeof(char),29))),_tag_dyneither(_tmp66E,sizeof(void*),0)));}_LL12B:;}}static
void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_exn_typ,e);if(!Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ)){
const char*_tmp673;void*_tmp672[1];struct Cyc_String_pa_struct _tmp671;(_tmp671.tag=
0,((_tmp671.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp672[0]=& _tmp671,Cyc_Tcutil_terr(
loc,((_tmp673="expected xtunion exn but found %s",_tag_dyneither(_tmp673,sizeof(
char),34))),_tag_dyneither(_tmp672,sizeof(void*),1)))))));}return Cyc_Absyn_wildtyp(
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
te),k,1,(void*)ts->hd);{struct _tuple5*_tmp676;struct Cyc_List_List*_tmp675;
instantiation=((_tmp675=_region_malloc(_tmp209,sizeof(*_tmp675)),((_tmp675->hd=((
_tmp676=_region_malloc(_tmp209,sizeof(*_tmp676)),((_tmp676->f1=(struct Cyc_Absyn_Tvar*)
_tmp201->hd,((_tmp676->f2=(void*)ts->hd,_tmp676)))))),((_tmp675->tl=
instantiation,_tmp675))))));}}if(ts != 0){const char*_tmp679;void*_tmp678;return(
_tmp678=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp679="too many type variables in instantiation",
_tag_dyneither(_tmp679,sizeof(char),41))),_tag_dyneither(_tmp678,sizeof(void*),0)));}{
struct Cyc_Absyn_FnType_struct _tmp67F;struct Cyc_Absyn_FnInfo _tmp67E;struct Cyc_Absyn_FnType_struct*
_tmp67D;void*new_fn_typ=Cyc_Tcutil_rsubstitute(_tmp209,instantiation,(void*)((
_tmp67D=_cycalloc(sizeof(*_tmp67D)),((_tmp67D[0]=((_tmp67F.tag=8,((_tmp67F.f1=((
_tmp67E.tvars=_tmp201,((_tmp67E.effect=_tmp202,((_tmp67E.ret_typ=(void*)_tmp203,((
_tmp67E.args=_tmp204,((_tmp67E.c_varargs=_tmp205,((_tmp67E.cyc_varargs=_tmp206,((
_tmp67E.rgn_po=_tmp207,((_tmp67E.attributes=_tmp208,_tmp67E)))))))))))))))),
_tmp67F)))),_tmp67D)))));struct Cyc_Absyn_PointerType_struct _tmp689;struct Cyc_Absyn_PtrAtts
_tmp688;struct Cyc_Absyn_PtrInfo _tmp687;struct Cyc_Absyn_PointerType_struct*
_tmp686;return(void*)((_tmp686=_cycalloc(sizeof(*_tmp686)),((_tmp686[0]=((
_tmp689.tag=4,((_tmp689.f1=((_tmp687.elt_typ=(void*)new_fn_typ,((_tmp687.elt_tq=
_tmp1F9,((_tmp687.ptr_atts=((_tmp688.rgn=(void*)_tmp1FB,((_tmp688.nullable=
_tmp1FC,((_tmp688.bounds=_tmp1FD,((_tmp688.zero_term=_tmp1FE,((_tmp688.ptrloc=0,
_tmp688)))))))))),_tmp687)))))),_tmp689)))),_tmp686))));}}_LL151:;_LL152: goto
_LL14E;_LL14E:;}goto _LL149;_LL14C:;_LL14D: goto _LL149;_LL149:;}{const char*_tmp68D;
void*_tmp68C[1];struct Cyc_String_pa_struct _tmp68B;return(_tmp68B.tag=0,((_tmp68B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((
_tmp68C[0]=& _tmp68B,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp68D="expecting polymorphic type but found %s",
_tag_dyneither(_tmp68D,sizeof(char),40))),_tag_dyneither(_tmp68C,sizeof(void*),1)))))));}}}
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,void**c);static void*Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct
Cyc_Absyn_Exp*e,void**c){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((void**)
_check_null(c))=(void*)1;else{void*crc=Cyc_Tcutil_castable(te,loc,t2,t);if(crc != (
void*)0)*((void**)_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((
void**)_check_null(c))=(void*)1;else{Cyc_Tcutil_unify(t2,t);{const char*_tmp692;
void*_tmp691[2];struct Cyc_String_pa_struct _tmp690;struct Cyc_String_pa_struct
_tmp68F;void*_tmp218=(_tmp68F.tag=0,((_tmp68F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp690.tag=0,((_tmp690.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp691[0]=&
_tmp690,((_tmp691[1]=& _tmp68F,Cyc_Tcexp_expr_err(te,loc,(void**)& t,((_tmp692="cannot cast %s to %s",
_tag_dyneither(_tmp692,sizeof(char),21))),_tag_dyneither(_tmp691,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();return _tmp218;}}}}{struct _tuple0 _tmp693;struct
_tuple0 _tmp21E=(_tmp693.f1=(void*)e->r,((_tmp693.f2=Cyc_Tcutil_compress(t),
_tmp693)));void*_tmp21F;struct Cyc_Absyn_MallocInfo _tmp220;int _tmp221;void*
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
_LL15A: if((Cyc_Evexp_eval_const_uint_exp(_tmp229)).f1 == 1){const char*_tmp696;
void*_tmp695;(_tmp695=0,Cyc_Tcutil_warn(loc,((_tmp696="cast from ? pointer to * pointer will lose size information",
_tag_dyneither(_tmp696,sizeof(char),60))),_tag_dyneither(_tmp695,sizeof(void*),0)));}
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
void**_tmp697;_tmp22C=((_tmp697=_cycalloc(sizeof(*_tmp697)),((_tmp697[0]=_tmp230,
_tmp697))));}_tmp22D=_tmp231;goto _LL15D;_LL160:;_LL161: goto _LL15D;_LL15D:;}{
struct _RegionHandle*_tmp235=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_clear_notreadctxt(
_tmp235,te),_tmp22C,e);if(Cyc_Tcutil_is_noalias_path(_tmp235,e)){const char*
_tmp69A;void*_tmp699;(_tmp699=0,Cyc_Tcutil_terr(e->loc,((_tmp69A="Cannot take the address of an alias-free path",
_tag_dyneither(_tmp69A,sizeof(char),46))),_tag_dyneither(_tmp699,sizeof(void*),0)));}{
void*_tmp238=(void*)e->r;struct Cyc_Absyn_Exp*_tmp239;struct Cyc_Absyn_Exp*_tmp23A;
_LL163: if(*((int*)_tmp238)!= 25)goto _LL165;_tmp239=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp238)->f1;_tmp23A=((struct Cyc_Absyn_Subscript_e_struct*)_tmp238)->f2;_LL164:{
void*_tmp23B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp239->topt))->v);_LL168: if(_tmp23B <= (void*)4)goto _LL16A;if(*((int*)_tmp23B)
!= 9)goto _LL16A;_LL169: goto _LL167;_LL16A:;_LL16B:(void*)(e0->r=(void*)((void*)(
Cyc_Absyn_add_exp(_tmp239,_tmp23A,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp239,
_tmp23A);_LL167:;}goto _LL162;_LL165:;_LL166: goto _LL162;_LL162:;}{int _tmp23D;void*
_tmp23E;struct _tuple6 _tmp23C=Cyc_Tcutil_addressof_props(te,e);_tmp23D=_tmp23C.f1;
_tmp23E=_tmp23C.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp23D){
tq.print_const=1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp23E,tq,Cyc_Absyn_false_conref);return t;}}}}}static
void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t);static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,void*t){if(te->allow_valueof)return Cyc_Absyn_uint_typ;
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);if(!
Cyc_Evexp_okay_szofarg(t)){const char*_tmp69E;void*_tmp69D[1];struct Cyc_String_pa_struct
_tmp69C;(_tmp69C.tag=0,((_tmp69C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp69D[0]=& _tmp69C,Cyc_Tcutil_terr(loc,((_tmp69E="sizeof applied to type %s, which has unknown size here",
_tag_dyneither(_tmp69E,sizeof(char),55))),_tag_dyneither(_tmp69D,sizeof(void*),1)))))));}
if(topt != 0){void*_tmp242=Cyc_Tcutil_compress(*topt);void*_tmp243;_LL16D: if(
_tmp242 <= (void*)4)goto _LL16F;if(*((int*)_tmp242)!= 18)goto _LL16F;_tmp243=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp242)->f1;_LL16E: {struct Cyc_Absyn_Exp*
_tmp244=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct _tmp6A1;
struct Cyc_Absyn_ValueofType_struct*_tmp6A0;struct Cyc_Absyn_ValueofType_struct*
_tmp245=(_tmp6A0=_cycalloc(sizeof(*_tmp6A0)),((_tmp6A0[0]=((_tmp6A1.tag=17,((
_tmp6A1.f1=_tmp244,_tmp6A1)))),_tmp6A0)));if(Cyc_Tcutil_unify(_tmp243,(void*)
_tmp245))return _tmp242;goto _LL16C;}_LL16F:;_LL170: goto _LL16C;_LL16C:;}return Cyc_Absyn_uint_typ;}
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf);int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf){return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 
0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,void*n);static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*_tmp248=n;struct
_dyneither_ptr*_tmp249;unsigned int _tmp24A;_LL172: if(*((int*)_tmp248)!= 0)goto
_LL174;_tmp249=((struct Cyc_Absyn_StructField_struct*)_tmp248)->f1;_LL173: {int
bad_type=1;{void*_tmp24B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp24C;
union Cyc_Absyn_AggrInfoU_union _tmp24D;struct Cyc_Absyn_Aggrdecl**_tmp24E;struct
Cyc_List_List*_tmp24F;_LL177: if(_tmp24B <= (void*)4)goto _LL17B;if(*((int*)_tmp24B)
!= 10)goto _LL179;_tmp24C=((struct Cyc_Absyn_AggrType_struct*)_tmp24B)->f1;_tmp24D=
_tmp24C.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp24B)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL179;_tmp24E=(_tmp24D.KnownAggr).f1;_LL178: if((*_tmp24E)->impl == 0)
goto _LL176;if(!((int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),
struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp249,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp24E)->impl))->fields)){
const char*_tmp6A5;void*_tmp6A4[1];struct Cyc_String_pa_struct _tmp6A3;(_tmp6A3.tag=
0,((_tmp6A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp249),((_tmp6A4[
0]=& _tmp6A3,Cyc_Tcutil_terr(loc,((_tmp6A5="no field of struct/union has name %s",
_tag_dyneither(_tmp6A5,sizeof(char),37))),_tag_dyneither(_tmp6A4,sizeof(void*),1)))))));}
bad_type=0;goto _LL176;_LL179: if(*((int*)_tmp24B)!= 11)goto _LL17B;_tmp24F=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp24B)->f2;_LL17A: if(!((int(*)(int(*pred)(
struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp249,
_tmp24F)){const char*_tmp6A9;void*_tmp6A8[1];struct Cyc_String_pa_struct _tmp6A7;(
_tmp6A7.tag=0,((_tmp6A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp249),((
_tmp6A8[0]=& _tmp6A7,Cyc_Tcutil_terr(loc,((_tmp6A9="no field of struct/union has name %s",
_tag_dyneither(_tmp6A9,sizeof(char),37))),_tag_dyneither(_tmp6A8,sizeof(void*),1)))))));}
bad_type=0;goto _LL176;_LL17B:;_LL17C: goto _LL176;_LL176:;}if(bad_type){const char*
_tmp6AD;void*_tmp6AC[1];struct Cyc_String_pa_struct _tmp6AB;(_tmp6AB.tag=0,((
_tmp6AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp6AC[0]=& _tmp6AB,Cyc_Tcutil_terr(loc,((_tmp6AD="%s is not a known struct/union type",
_tag_dyneither(_tmp6AD,sizeof(char),36))),_tag_dyneither(_tmp6AC,sizeof(void*),1)))))));}
goto _LL171;}_LL174: if(*((int*)_tmp248)!= 1)goto _LL171;_tmp24A=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp248)->f1;_LL175: {int bad_type=1;{void*_tmp259=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_AggrInfo _tmp25A;union Cyc_Absyn_AggrInfoU_union _tmp25B;struct Cyc_Absyn_Aggrdecl**
_tmp25C;struct Cyc_List_List*_tmp25D;struct Cyc_List_List*_tmp25E;struct Cyc_Absyn_TunionFieldInfo
_tmp25F;union Cyc_Absyn_TunionFieldInfoU_union _tmp260;struct Cyc_Absyn_Tunionfield*
_tmp261;_LL17E: if(_tmp259 <= (void*)4)goto _LL186;if(*((int*)_tmp259)!= 10)goto
_LL180;_tmp25A=((struct Cyc_Absyn_AggrType_struct*)_tmp259)->f1;_tmp25B=_tmp25A.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp259)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL180;_tmp25C=(_tmp25B.KnownAggr).f1;_LL17F: if((*_tmp25C)->impl == 0)
goto _LL17D;_tmp25D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp25C)->impl))->fields;
goto _LL181;_LL180: if(*((int*)_tmp259)!= 11)goto _LL182;_tmp25D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp259)->f2;_LL181: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp25D)
<= _tmp24A){const char*_tmp6B2;void*_tmp6B1[2];struct Cyc_Int_pa_struct _tmp6B0;
struct Cyc_Int_pa_struct _tmp6AF;(_tmp6AF.tag=1,((_tmp6AF.f1=(unsigned long)((int)
_tmp24A),((_tmp6B0.tag=1,((_tmp6B0.f1=(unsigned long)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp25D),((_tmp6B1[0]=& _tmp6B0,((_tmp6B1[1]=& _tmp6AF,Cyc_Tcutil_terr(
loc,((_tmp6B2="struct/union has too few components: %d <= %d",_tag_dyneither(
_tmp6B2,sizeof(char),46))),_tag_dyneither(_tmp6B1,sizeof(void*),2)))))))))))));}
bad_type=0;goto _LL17D;_LL182: if(*((int*)_tmp259)!= 9)goto _LL184;_tmp25E=((struct
Cyc_Absyn_TupleType_struct*)_tmp259)->f1;_LL183: if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp25E)<= _tmp24A){const char*_tmp6B7;void*_tmp6B6[2];struct
Cyc_Int_pa_struct _tmp6B5;struct Cyc_Int_pa_struct _tmp6B4;(_tmp6B4.tag=1,((_tmp6B4.f1=(
unsigned long)((int)_tmp24A),((_tmp6B5.tag=1,((_tmp6B5.f1=(unsigned long)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp25E),((_tmp6B6[0]=& _tmp6B5,((_tmp6B6[
1]=& _tmp6B4,Cyc_Tcutil_terr(loc,((_tmp6B7="tuple has too few components: %d <= %d",
_tag_dyneither(_tmp6B7,sizeof(char),39))),_tag_dyneither(_tmp6B6,sizeof(void*),2)))))))))))));}
bad_type=0;goto _LL17D;_LL184: if(*((int*)_tmp259)!= 3)goto _LL186;_tmp25F=((struct
Cyc_Absyn_TunionFieldType_struct*)_tmp259)->f1;_tmp260=_tmp25F.field_info;if((((((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp259)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL186;_tmp261=(_tmp260.KnownTunionfield).f2;_LL185: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp261->typs)< _tmp24A){const char*_tmp6BC;
void*_tmp6BB[2];struct Cyc_Int_pa_struct _tmp6BA;struct Cyc_Int_pa_struct _tmp6B9;(
_tmp6B9.tag=1,((_tmp6B9.f1=(unsigned long)((int)_tmp24A),((_tmp6BA.tag=1,((
_tmp6BA.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp261->typs),((_tmp6BB[0]=& _tmp6BA,((_tmp6BB[1]=& _tmp6B9,Cyc_Tcutil_terr(loc,((
_tmp6BC="tunionfield has too few components: %d < %d",_tag_dyneither(_tmp6BC,
sizeof(char),44))),_tag_dyneither(_tmp6BB,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL17D;_LL186:;_LL187: goto _LL17D;_LL17D:;}if(bad_type){const char*_tmp6C0;
void*_tmp6BF[1];struct Cyc_String_pa_struct _tmp6BE;(_tmp6BE.tag=0,((_tmp6BE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6BF[
0]=& _tmp6BE,Cyc_Tcutil_terr(loc,((_tmp6C0="%s is not a known type",
_tag_dyneither(_tmp6C0,sizeof(char),23))),_tag_dyneither(_tmp6BF,sizeof(void*),1)))))));}
goto _LL171;}_LL171:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e);static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){struct _RegionHandle*_tmp271=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp272=Cyc_Tcenv_clear_notreadctxt(_tmp271,
te_orig);Cyc_Tcexp_tcExp(_tmp272,0,e);{void*t=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp273=t;struct Cyc_Absyn_PtrInfo
_tmp274;void*_tmp275;struct Cyc_Absyn_PtrAtts _tmp276;void*_tmp277;struct Cyc_Absyn_Conref*
_tmp278;struct Cyc_Absyn_Conref*_tmp279;_LL189: if(_tmp273 <= (void*)4)goto _LL18B;
if(*((int*)_tmp273)!= 4)goto _LL18B;_tmp274=((struct Cyc_Absyn_PointerType_struct*)
_tmp273)->f1;_tmp275=(void*)_tmp274.elt_typ;_tmp276=_tmp274.ptr_atts;_tmp277=(
void*)_tmp276.rgn;_tmp278=_tmp276.bounds;_tmp279=_tmp276.zero_term;_LL18A: Cyc_Tcenv_check_rgn_accessible(
_tmp272,loc,_tmp277);Cyc_Tcutil_check_nonzero_bound(loc,_tmp278);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp275),(void*)1)){const char*_tmp6C3;void*_tmp6C2;(_tmp6C2=
0,Cyc_Tcutil_terr(loc,((_tmp6C3="can't dereference abstract pointer type",
_tag_dyneither(_tmp6C3,sizeof(char),40))),_tag_dyneither(_tmp6C2,sizeof(void*),0)));}
return _tmp275;_LL18B:;_LL18C: {const char*_tmp6C7;void*_tmp6C6[1];struct Cyc_String_pa_struct
_tmp6C5;return(_tmp6C5.tag=0,((_tmp6C5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6C6[0]=& _tmp6C5,Cyc_Tcexp_expr_err(
_tmp272,loc,topt,((_tmp6C7="expecting * or @ type but found %s",_tag_dyneither(
_tmp6C7,sizeof(char),35))),_tag_dyneither(_tmp6C6,sizeof(void*),1)))))));}_LL188:;}}
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f);static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr*f){Cyc_Tcexp_tcExpNoPromote(te,0,e);{void*_tmp27F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp280;union Cyc_Absyn_AggrInfoU_union _tmp281;struct Cyc_Absyn_Aggrdecl**_tmp282;
struct Cyc_Absyn_Aggrdecl*_tmp283;struct Cyc_List_List*_tmp284;void*_tmp285;struct
Cyc_List_List*_tmp286;_LL18E: if(_tmp27F <= (void*)4)goto _LL196;if(*((int*)_tmp27F)
!= 10)goto _LL190;_tmp280=((struct Cyc_Absyn_AggrType_struct*)_tmp27F)->f1;_tmp281=
_tmp280.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp27F)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL190;_tmp282=(_tmp281.KnownAggr).f1;_tmp283=*_tmp282;_tmp284=_tmp280.targs;
_LL18F: {struct Cyc_Absyn_Aggrfield*_tmp289=Cyc_Absyn_lookup_decl_field(_tmp283,f);
if(_tmp289 == 0){const char*_tmp6CC;void*_tmp6CB[2];struct Cyc_String_pa_struct
_tmp6CA;struct Cyc_String_pa_struct _tmp6C9;return(_tmp6C9.tag=0,((_tmp6C9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6CA.tag=0,((_tmp6CA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp283->name)),((
_tmp6CB[0]=& _tmp6CA,((_tmp6CB[1]=& _tmp6C9,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp6CC="type %s has no %s field",_tag_dyneither(_tmp6CC,sizeof(char),24))),
_tag_dyneither(_tmp6CB,sizeof(void*),2)))))))))))));}if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp283->impl))->exist_vars != 0){const char*_tmp6CF;void*_tmp6CE;
return(_tmp6CE=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6CF="must use pattern-matching to access fields of existential types",
_tag_dyneither(_tmp6CF,sizeof(char),64))),_tag_dyneither(_tmp6CE,sizeof(void*),0)));}{
void*t2;struct _RegionHandle*_tmp290=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
_tmp291=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp290,_tmp290,
_tmp283->tvs,_tmp284);t2=Cyc_Tcutil_rsubstitute(_tmp290,_tmp291,(void*)_tmp289->type);
if(((void*)_tmp283->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t2)){const char*_tmp6D3;void*_tmp6D2[1];struct Cyc_String_pa_struct _tmp6D1;return(
_tmp6D1.tag=0,((_tmp6D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp6D2[0]=& _tmp6D1,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6D3="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp6D3,sizeof(char),56))),_tag_dyneither(_tmp6D2,sizeof(void*),1)))))));}}
return t2;}}_LL190: if(*((int*)_tmp27F)!= 11)goto _LL192;_tmp285=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp27F)->f1;_tmp286=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp27F)->f2;_LL191: {
struct Cyc_Absyn_Aggrfield*_tmp295=Cyc_Absyn_lookup_field(_tmp286,f);if(_tmp295 == 
0){const char*_tmp6D7;void*_tmp6D6[1];struct Cyc_String_pa_struct _tmp6D5;return(
_tmp6D5.tag=0,((_tmp6D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp6D6[0]=& _tmp6D5,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6D7="type has no %s field",
_tag_dyneither(_tmp6D7,sizeof(char),21))),_tag_dyneither(_tmp6D6,sizeof(void*),1)))))));}
if((_tmp285 == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp295->type)){const char*_tmp6DB;void*_tmp6DA[1];struct Cyc_String_pa_struct
_tmp6D9;return(_tmp6D9.tag=0,((_tmp6D9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp6DA[0]=& _tmp6D9,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp6DB="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp6DB,sizeof(char),56))),_tag_dyneither(_tmp6DA,sizeof(void*),1)))))));}return(
void*)_tmp295->type;}_LL192: if(*((int*)_tmp27F)!= 7)goto _LL194;{const char*
_tmp6DC;if(!(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp6DC="size",_tag_dyneither(
_tmp6DC,sizeof(char),5))))== 0))goto _LL194;}_LL193: goto _LL195;_LL194: if(*((int*)
_tmp27F)!= 4)goto _LL196;{const char*_tmp6DD;if(!(Cyc_strcmp((struct _dyneither_ptr)*
f,((_tmp6DD="size",_tag_dyneither(_tmp6DD,sizeof(char),5))))== 0))goto _LL196;}
_LL195:{const char*_tmp6E1;void*_tmp6E0[1];struct Cyc_String_pa_struct _tmp6DF;(
_tmp6DF.tag=0,((_tmp6DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp6E0[0]=& _tmp6DF,Cyc_Tcutil_warn(e->loc,((_tmp6E1="deprecated `.size' used here -- change to numelts(%s)",
_tag_dyneither(_tmp6E1,sizeof(char),54))),_tag_dyneither(_tmp6E0,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Primop_e_struct _tmp6E7;struct Cyc_List_List*_tmp6E6;struct Cyc_Absyn_Primop_e_struct*
_tmp6E5;(void*)(outer_e->r=(void*)((void*)((_tmp6E5=_cycalloc(sizeof(*_tmp6E5)),((
_tmp6E5[0]=((_tmp6E7.tag=3,((_tmp6E7.f1=(void*)((void*)19),((_tmp6E7.f2=((
_tmp6E6=_cycalloc(sizeof(*_tmp6E6)),((_tmp6E6->hd=e,((_tmp6E6->tl=0,_tmp6E6)))))),
_tmp6E7)))))),_tmp6E5))))));}return Cyc_Absyn_uint_typ;_LL196:;_LL197: {const char*
_tmp6E8;if(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp6E8="size",_tag_dyneither(
_tmp6E8,sizeof(char),5))))== 0){const char*_tmp6EC;void*_tmp6EB[1];struct Cyc_String_pa_struct
_tmp6EA;return(_tmp6EA.tag=0,((_tmp6EA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp6EB[0]=& _tmp6EA,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6EC="expecting struct, union, or array, found %s",
_tag_dyneither(_tmp6EC,sizeof(char),44))),_tag_dyneither(_tmp6EB,sizeof(void*),1)))))));}
else{const char*_tmp6F0;void*_tmp6EF[1];struct Cyc_String_pa_struct _tmp6EE;return(
_tmp6EE.tag=0,((_tmp6EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp6EF[0]=& _tmp6EE,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp6F0="expecting struct or union, found %s",_tag_dyneither(
_tmp6F0,sizeof(char),36))),_tag_dyneither(_tmp6EF,sizeof(void*),1)))))));}}
_LL18D:;}}static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f);static void*Cyc_Tcexp_tcAggrArrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr*f){struct _RegionHandle*_tmp2A9=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(_tmp2A9,te),0,e);{void*_tmp2AA=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp2AB;void*_tmp2AC;struct Cyc_Absyn_PtrAtts _tmp2AD;void*_tmp2AE;struct Cyc_Absyn_Conref*
_tmp2AF;struct Cyc_Absyn_Conref*_tmp2B0;_LL199: if(_tmp2AA <= (void*)4)goto _LL19B;
if(*((int*)_tmp2AA)!= 4)goto _LL19B;_tmp2AB=((struct Cyc_Absyn_PointerType_struct*)
_tmp2AA)->f1;_tmp2AC=(void*)_tmp2AB.elt_typ;_tmp2AD=_tmp2AB.ptr_atts;_tmp2AE=(
void*)_tmp2AD.rgn;_tmp2AF=_tmp2AD.bounds;_tmp2B0=_tmp2AD.zero_term;_LL19A: Cyc_Tcutil_check_nonzero_bound(
loc,_tmp2AF);{void*_tmp2B1=Cyc_Tcutil_compress(_tmp2AC);struct Cyc_Absyn_AggrInfo
_tmp2B2;union Cyc_Absyn_AggrInfoU_union _tmp2B3;struct Cyc_Absyn_Aggrdecl**_tmp2B4;
struct Cyc_Absyn_Aggrdecl*_tmp2B5;struct Cyc_List_List*_tmp2B6;void*_tmp2B7;struct
Cyc_List_List*_tmp2B8;_LL19E: if(_tmp2B1 <= (void*)4)goto _LL1A2;if(*((int*)_tmp2B1)
!= 10)goto _LL1A0;_tmp2B2=((struct Cyc_Absyn_AggrType_struct*)_tmp2B1)->f1;_tmp2B3=
_tmp2B2.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp2B1)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL1A0;_tmp2B4=(_tmp2B3.KnownAggr).f1;_tmp2B5=*_tmp2B4;_tmp2B6=_tmp2B2.targs;
_LL19F: {struct Cyc_Absyn_Aggrfield*_tmp2B9=Cyc_Absyn_lookup_decl_field(_tmp2B5,f);
if(_tmp2B9 == 0){const char*_tmp6F5;void*_tmp6F4[2];struct Cyc_String_pa_struct
_tmp6F3;struct Cyc_String_pa_struct _tmp6F2;return(_tmp6F2.tag=0,((_tmp6F2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6F3.tag=0,((_tmp6F3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2B5->name)),((
_tmp6F4[0]=& _tmp6F3,((_tmp6F4[1]=& _tmp6F2,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp6F5="type %s has no %s field",_tag_dyneither(_tmp6F5,sizeof(char),24))),
_tag_dyneither(_tmp6F4,sizeof(void*),2)))))))))))));}if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp2B5->impl))->exist_vars != 0){const char*_tmp6F8;void*_tmp6F7;
return(_tmp6F7=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6F8="must use pattern-matching to access fields of existential types",
_tag_dyneither(_tmp6F8,sizeof(char),64))),_tag_dyneither(_tmp6F7,sizeof(void*),0)));}{
void*t3;struct _RegionHandle*_tmp2C0=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
_tmp2C1=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp2C0,_tmp2C0,
_tmp2B5->tvs,_tmp2B6);t3=Cyc_Tcutil_rsubstitute(_tmp2C0,_tmp2C1,(void*)_tmp2B9->type);}
if(((void*)_tmp2B5->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t3)){const char*_tmp6FC;void*_tmp6FB[1];struct Cyc_String_pa_struct _tmp6FA;return(
_tmp6FA.tag=0,((_tmp6FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp6FB[0]=& _tmp6FA,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6FC="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp6FC,sizeof(char),56))),_tag_dyneither(_tmp6FB,sizeof(void*),1)))))));}
return t3;}}_LL1A0: if(*((int*)_tmp2B1)!= 11)goto _LL1A2;_tmp2B7=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2B1)->f1;_tmp2B8=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2B1)->f2;_LL1A1: {
struct Cyc_Absyn_Aggrfield*_tmp2C5=Cyc_Absyn_lookup_field(_tmp2B8,f);if(_tmp2C5 == 
0){const char*_tmp700;void*_tmp6FF[1];struct Cyc_String_pa_struct _tmp6FE;return(
_tmp6FE.tag=0,((_tmp6FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp6FF[0]=& _tmp6FE,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp700="type has no %s field",
_tag_dyneither(_tmp700,sizeof(char),21))),_tag_dyneither(_tmp6FF,sizeof(void*),1)))))));}
if((_tmp2B7 == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp2C5->type)){const char*_tmp704;void*_tmp703[1];struct Cyc_String_pa_struct
_tmp702;return(_tmp702.tag=0,((_tmp702.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp703[0]=& _tmp702,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp704="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp704,sizeof(char),56))),_tag_dyneither(_tmp703,sizeof(void*),1)))))));}return(
void*)_tmp2C5->type;}_LL1A2:;_LL1A3: goto _LL19D;_LL19D:;}goto _LL198;_LL19B:;
_LL19C: goto _LL198;_LL198:;}{const char*_tmp708;void*_tmp707[1];struct Cyc_String_pa_struct
_tmp706;return(_tmp706.tag=0,((_tmp706.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp707[0]=& _tmp706,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp708="expecting struct or union pointer, found %s",
_tag_dyneither(_tmp708,sizeof(char),44))),_tag_dyneither(_tmp707,sizeof(void*),1)))))));}}
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index);static void*Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,
struct Cyc_Absyn_Exp*index){unsigned int _tmp2D0;int _tmp2D1;struct _tuple7 _tmp2CF=
Cyc_Evexp_eval_const_uint_exp(index);_tmp2D0=_tmp2CF.f1;_tmp2D1=_tmp2CF.f2;if(!
_tmp2D1){const char*_tmp70B;void*_tmp70A;return(_tmp70A=0,Cyc_Tcexp_expr_err(te,
loc,0,((_tmp70B="tuple projection cannot use sizeof or offsetof",_tag_dyneither(
_tmp70B,sizeof(char),47))),_tag_dyneither(_tmp70A,sizeof(void*),0)));}{struct
_handler_cons _tmp2D4;_push_handler(& _tmp2D4);{int _tmp2D6=0;if(setjmp(_tmp2D4.handler))
_tmp2D6=1;if(!_tmp2D6){{void*_tmp2D7=(*((struct _tuple9*(*)(struct Cyc_List_List*x,
int n))Cyc_List_nth)(ts,(int)_tmp2D0)).f2;_npop_handler(0);return _tmp2D7;};
_pop_handler();}else{void*_tmp2D5=(void*)_exn_thrown;void*_tmp2D9=_tmp2D5;_LL1A5:
if(_tmp2D9 != Cyc_List_Nth)goto _LL1A7;_LL1A6: {const char*_tmp710;void*_tmp70F[2];
struct Cyc_Int_pa_struct _tmp70E;struct Cyc_Int_pa_struct _tmp70D;return(_tmp70D.tag=
1,((_tmp70D.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
ts),((_tmp70E.tag=1,((_tmp70E.f1=(unsigned long)((int)_tmp2D0),((_tmp70F[0]=&
_tmp70E,((_tmp70F[1]=& _tmp70D,Cyc_Tcexp_expr_err(te,loc,0,((_tmp710="index is %d but tuple has only %d fields",
_tag_dyneither(_tmp710,sizeof(char),41))),_tag_dyneither(_tmp70F,sizeof(void*),2)))))))))))));}
_LL1A7:;_LL1A8:(void)_throw(_tmp2D9);_LL1A4:;}}}}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp2DE=Cyc_Tcenv_get_fnrgn(te_orig);
struct Cyc_Tcenv_Tenv*_tmp2DF=Cyc_Tcenv_clear_notreadctxt(_tmp2DE,te_orig);Cyc_Tcexp_tcExp(
_tmp2DF,0,e1);Cyc_Tcexp_tcExp(_tmp2DF,0,e2);{void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!Cyc_Tcutil_coerce_sint_typ(
_tmp2DF,e2)){const char*_tmp714;void*_tmp713[1];struct Cyc_String_pa_struct _tmp712;
return(_tmp712.tag=0,((_tmp712.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp713[0]=& _tmp712,Cyc_Tcexp_expr_err(_tmp2DF,e2->loc,
topt,((_tmp714="expecting int subscript, found %s",_tag_dyneither(_tmp714,
sizeof(char),34))),_tag_dyneither(_tmp713,sizeof(void*),1)))))));}{void*_tmp2E3=
t1;struct Cyc_Absyn_PtrInfo _tmp2E4;void*_tmp2E5;struct Cyc_Absyn_Tqual _tmp2E6;
struct Cyc_Absyn_PtrAtts _tmp2E7;void*_tmp2E8;struct Cyc_Absyn_Conref*_tmp2E9;
struct Cyc_Absyn_Conref*_tmp2EA;struct Cyc_List_List*_tmp2EB;_LL1AA: if(_tmp2E3 <= (
void*)4)goto _LL1AE;if(*((int*)_tmp2E3)!= 4)goto _LL1AC;_tmp2E4=((struct Cyc_Absyn_PointerType_struct*)
_tmp2E3)->f1;_tmp2E5=(void*)_tmp2E4.elt_typ;_tmp2E6=_tmp2E4.elt_tq;_tmp2E7=
_tmp2E4.ptr_atts;_tmp2E8=(void*)_tmp2E7.rgn;_tmp2E9=_tmp2E7.bounds;_tmp2EA=
_tmp2E7.zero_term;_LL1AB: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp2EA)){int emit_warning=1;{union Cyc_Absyn_Constraint_union _tmp2EC=(Cyc_Absyn_compress_conref(
_tmp2E9))->v;void*_tmp2ED;struct Cyc_Absyn_Exp*_tmp2EE;void*_tmp2EF;_LL1B1: if((
_tmp2EC.Eq_constr).tag != 0)goto _LL1B3;_tmp2ED=(_tmp2EC.Eq_constr).f1;if(_tmp2ED
<= (void*)1)goto _LL1B3;if(*((int*)_tmp2ED)!= 0)goto _LL1B3;_tmp2EE=((struct Cyc_Absyn_Upper_b_struct*)
_tmp2ED)->f1;_LL1B2: if(Cyc_Tcutil_is_const_exp(_tmp2DF,e2)){unsigned int _tmp2F1;
int _tmp2F2;struct _tuple7 _tmp2F0=Cyc_Evexp_eval_const_uint_exp(e2);_tmp2F1=
_tmp2F0.f1;_tmp2F2=_tmp2F0.f2;if(_tmp2F2){unsigned int _tmp2F4;int _tmp2F5;struct
_tuple7 _tmp2F3=Cyc_Evexp_eval_const_uint_exp(_tmp2EE);_tmp2F4=_tmp2F3.f1;_tmp2F5=
_tmp2F3.f2;if(_tmp2F5  && _tmp2F4 > _tmp2F1)emit_warning=0;}}goto _LL1B0;_LL1B3: if((
_tmp2EC.Eq_constr).tag != 0)goto _LL1B5;_tmp2EF=(_tmp2EC.Eq_constr).f1;if((int)
_tmp2EF != 0)goto _LL1B5;_LL1B4: emit_warning=0;goto _LL1B0;_LL1B5:;_LL1B6: goto
_LL1B0;_LL1B0:;}if(emit_warning){const char*_tmp717;void*_tmp716;(_tmp716=0,Cyc_Tcutil_warn(
e2->loc,((_tmp717="subscript on thin, zero-terminated pointer could be expensive.",
_tag_dyneither(_tmp717,sizeof(char),63))),_tag_dyneither(_tmp716,sizeof(void*),0)));}}
else{if(Cyc_Tcutil_is_const_exp(_tmp2DF,e2)){unsigned int _tmp2F9;int _tmp2FA;
struct _tuple7 _tmp2F8=Cyc_Evexp_eval_const_uint_exp(e2);_tmp2F9=_tmp2F8.f1;
_tmp2FA=_tmp2F8.f2;if(_tmp2FA)Cyc_Tcutil_check_bound(loc,_tmp2F9,_tmp2E9);}else{
if(Cyc_Tcutil_is_bound_one(_tmp2E9) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp2EA)){const char*_tmp71A;void*_tmp719;(_tmp719=0,Cyc_Tcutil_warn(
e1->loc,((_tmp71A="subscript applied to pointer to one object",_tag_dyneither(
_tmp71A,sizeof(char),43))),_tag_dyneither(_tmp719,sizeof(void*),0)));}Cyc_Tcutil_check_nonzero_bound(
loc,_tmp2E9);}}Cyc_Tcenv_check_rgn_accessible(_tmp2DF,loc,_tmp2E8);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp2E5),(void*)1)){const char*_tmp71D;void*_tmp71C;(_tmp71C=
0,Cyc_Tcutil_terr(e1->loc,((_tmp71D="can't subscript an abstract pointer",
_tag_dyneither(_tmp71D,sizeof(char),36))),_tag_dyneither(_tmp71C,sizeof(void*),0)));}
return _tmp2E5;_LL1AC: if(*((int*)_tmp2E3)!= 9)goto _LL1AE;_tmp2EB=((struct Cyc_Absyn_TupleType_struct*)
_tmp2E3)->f1;_LL1AD: return Cyc_Tcexp_ithTupleType(_tmp2DF,loc,_tmp2EB,e2);_LL1AE:;
_LL1AF: {const char*_tmp721;void*_tmp720[1];struct Cyc_String_pa_struct _tmp71F;
return(_tmp71F.tag=0,((_tmp71F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp720[0]=& _tmp71F,Cyc_Tcexp_expr_err(_tmp2DF,loc,
topt,((_tmp721="subscript applied to %s",_tag_dyneither(_tmp721,sizeof(char),24))),
_tag_dyneither(_tmp720,sizeof(void*),1)))))));}_LL1A9:;}}}static void*Cyc_Tcexp_tcTuple(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_List_List*
es);static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp302=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp303;
_LL1B8: if(_tmp302 <= (void*)4)goto _LL1BA;if(*((int*)_tmp302)!= 9)goto _LL1BA;
_tmp303=((struct Cyc_Absyn_TupleType_struct*)_tmp302)->f1;_LL1B9: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp303)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1B7;for(0;es != 0;(es=es->tl,_tmp303=_tmp303->tl)){
void*_tmp304=(*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp303))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp304,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple9*)_tmp303->hd)).f2);{struct
_tuple9*_tmp724;struct Cyc_List_List*_tmp723;fields=((_tmp723=_cycalloc(sizeof(*
_tmp723)),((_tmp723->hd=((_tmp724=_cycalloc(sizeof(*_tmp724)),((_tmp724->f1=(*((
struct _tuple9*)_tmp303->hd)).f1,((_tmp724->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,_tmp724)))))),((_tmp723->tl=
fields,_tmp723))))));}}done=1;goto _LL1B7;_LL1BA:;_LL1BB: goto _LL1B7;_LL1B7:;}if(!
done)for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)
es->hd);{struct _tuple9*_tmp727;struct Cyc_List_List*_tmp726;fields=((_tmp726=
_cycalloc(sizeof(*_tmp726)),((_tmp726->hd=((_tmp727=_cycalloc(sizeof(*_tmp727)),((
_tmp727->f1=Cyc_Absyn_empty_tqual(0),((_tmp727->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,_tmp727)))))),((_tmp726->tl=
fields,_tmp726))))));}}{struct Cyc_Absyn_TupleType_struct _tmp72A;struct Cyc_Absyn_TupleType_struct*
_tmp729;return(void*)((_tmp729=_cycalloc(sizeof(*_tmp729)),((_tmp729[0]=((
_tmp72A.tag=9,((_tmp72A.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmp72A)))),_tmp729))));}}static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*t,struct Cyc_List_List*
des);static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _tuple2*t,struct Cyc_List_List*des){const char*_tmp72D;void*
_tmp72C;return(_tmp72C=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp72D="tcCompoundLit",
_tag_dyneither(_tmp72D,sizeof(char),14))),_tag_dyneither(_tmp72C,sizeof(void*),0)));}
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,int zero_term,struct Cyc_List_List*des);static void*Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**elt_topt,int
zero_term,struct Cyc_List_List*des){void*res_t2;struct _RegionHandle*_tmp30D=Cyc_Tcenv_get_fnrgn(
te);{int _tmp30E=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple8*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp30D,(struct Cyc_Absyn_Exp*(*)(
struct _tuple8*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Const_e_struct
_tmp733;union Cyc_Absyn_Cnst_union _tmp732;struct Cyc_Absyn_Const_e_struct*_tmp731;
struct Cyc_Absyn_Const_e_struct*_tmp30F=(_tmp731=_cycalloc(sizeof(*_tmp731)),((
_tmp731[0]=((_tmp733.tag=0,((_tmp733.f1=(union Cyc_Absyn_Cnst_union)(((_tmp732.Int_c).tag=
2,(((_tmp732.Int_c).f1=(void*)((void*)1),(((_tmp732.Int_c).f2=_tmp30E,_tmp732)))))),
_tmp733)))),_tmp731)));struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)
_tmp30F,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp310=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp30E - 1);if(!Cyc_Tcutil_is_zero(
_tmp310)){const char*_tmp736;void*_tmp735;(_tmp735=0,Cyc_Tcutil_terr(_tmp310->loc,((
_tmp736="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp736,
sizeof(char),45))),_tag_dyneither(_tmp735,sizeof(void*),0)));}}{struct Cyc_Core_Opt*
_tmp737;sz_exp->topt=((_tmp737=_cycalloc(sizeof(*_tmp737)),((_tmp737->v=(void*)
Cyc_Absyn_uint_typ,_tmp737))));}res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(
0),(struct Cyc_Absyn_Exp*)sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,
0);{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(
te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,
es)){const char*_tmp73B;void*_tmp73A[1];struct Cyc_String_pa_struct _tmp739;(
_tmp739.tag=0,((_tmp739.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
res)),((_tmp73A[0]=& _tmp739,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,((_tmp73B="elements of array do not all have the same type (%s)",
_tag_dyneither(_tmp73B,sizeof(char),53))),_tag_dyneither(_tmp73A,sizeof(void*),1)))))));}}{
int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((
struct _tuple8*)des->hd)).f1;if(ds != 0){void*_tmp31A=(void*)ds->hd;struct Cyc_Absyn_Exp*
_tmp31B;_LL1BD: if(*((int*)_tmp31A)!= 1)goto _LL1BF;_LL1BE:{const char*_tmp73E;void*
_tmp73D;(_tmp73D=0,Cyc_Tcutil_terr(loc,((_tmp73E="only array index designators are supported",
_tag_dyneither(_tmp73E,sizeof(char),43))),_tag_dyneither(_tmp73D,sizeof(void*),0)));}
goto _LL1BC;_LL1BF: if(*((int*)_tmp31A)!= 0)goto _LL1BC;_tmp31B=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp31A)->f1;_LL1C0: Cyc_Tcexp_tcExpInitializer(te,0,_tmp31B);{unsigned int
_tmp31F;int _tmp320;struct _tuple7 _tmp31E=Cyc_Evexp_eval_const_uint_exp(_tmp31B);
_tmp31F=_tmp31E.f1;_tmp320=_tmp31E.f2;if(!_tmp320){const char*_tmp741;void*
_tmp740;(_tmp740=0,Cyc_Tcutil_terr(_tmp31B->loc,((_tmp741="index designator cannot use sizeof or offsetof",
_tag_dyneither(_tmp741,sizeof(char),47))),_tag_dyneither(_tmp740,sizeof(void*),0)));}
else{if(_tmp31F != offset){const char*_tmp746;void*_tmp745[2];struct Cyc_Int_pa_struct
_tmp744;struct Cyc_Int_pa_struct _tmp743;(_tmp743.tag=1,((_tmp743.f1=(
unsigned long)((int)_tmp31F),((_tmp744.tag=1,((_tmp744.f1=(unsigned long)offset,((
_tmp745[0]=& _tmp744,((_tmp745[1]=& _tmp743,Cyc_Tcutil_terr(_tmp31B->loc,((_tmp746="expecting index designator %d but found %d",
_tag_dyneither(_tmp746,sizeof(char),43))),_tag_dyneither(_tmp745,sizeof(void*),2)))))))))))));}}
goto _LL1BC;}_LL1BC:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term);static
void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*
body,int*is_zero_term){Cyc_Tcexp_tcExp(te,0,bound);{void*_tmp327=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(bound->topt))->v);_LL1C2: if(_tmp327 <= (
void*)4)goto _LL1C4;if(*((int*)_tmp327)!= 18)goto _LL1C4;_LL1C3: goto _LL1C1;_LL1C4:;
_LL1C5: if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){const char*_tmp74A;void*_tmp749[
1];struct Cyc_String_pa_struct _tmp748;(_tmp748.tag=0,((_tmp748.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v)),((_tmp749[0]=& _tmp748,Cyc_Tcutil_terr(bound->loc,((
_tmp74A="expecting unsigned int, found %s",_tag_dyneither(_tmp74A,sizeof(char),
33))),_tag_dyneither(_tmp749,sizeof(void*),1)))))));}_LL1C1:;}if(!(vd->tq).real_const){
const char*_tmp74D;void*_tmp74C;(_tmp74C=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp74D="comprehension index variable is not declared const!",
_tag_dyneither(_tmp74D,sizeof(char),52))),_tag_dyneither(_tmp74C,sizeof(void*),0)));}{
struct _RegionHandle*_tmp32D=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(
_tmp32D,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(_tmp32D,loc,te2);te2=Cyc_Tcenv_add_local_var(
_tmp32D,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*_tmp32E=te2;struct Cyc_Absyn_PtrInfo
pinfo;void**_tmp32F=0;struct Cyc_Absyn_Tqual*_tmp330=0;struct Cyc_Absyn_Conref**
_tmp331=0;if(topt != 0){void*_tmp332=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp333;struct Cyc_Absyn_ArrayInfo _tmp334;void*_tmp335;void**_tmp336;struct Cyc_Absyn_Tqual
_tmp337;struct Cyc_Absyn_Tqual*_tmp338;struct Cyc_Absyn_Exp*_tmp339;struct Cyc_Absyn_Conref*
_tmp33A;struct Cyc_Absyn_Conref**_tmp33B;_LL1C7: if(_tmp332 <= (void*)4)goto _LL1CB;
if(*((int*)_tmp332)!= 4)goto _LL1C9;_tmp333=((struct Cyc_Absyn_PointerType_struct*)
_tmp332)->f1;_LL1C8: pinfo=_tmp333;_tmp32F=(void**)((void**)((void*)& pinfo.elt_typ));
_tmp330=(struct Cyc_Absyn_Tqual*)& pinfo.elt_tq;_tmp331=(struct Cyc_Absyn_Conref**)&(
pinfo.ptr_atts).zero_term;goto _LL1C6;_LL1C9: if(*((int*)_tmp332)!= 7)goto _LL1CB;
_tmp334=((struct Cyc_Absyn_ArrayType_struct*)_tmp332)->f1;_tmp335=(void*)_tmp334.elt_type;
_tmp336=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp332)->f1).elt_type;
_tmp337=_tmp334.tq;_tmp338=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp332)->f1).tq;_tmp339=_tmp334.num_elts;_tmp33A=_tmp334.zero_term;_tmp33B=(
struct Cyc_Absyn_Conref**)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp332)->f1).zero_term;
_LL1CA: _tmp32F=(void**)_tmp336;_tmp330=(struct Cyc_Absyn_Tqual*)_tmp338;_tmp331=(
struct Cyc_Absyn_Conref**)_tmp33B;goto _LL1C6;_LL1CB:;_LL1CC: goto _LL1C6;_LL1C6:;}{
void*t=Cyc_Tcexp_tcExp(_tmp32E,_tmp32F,body);if(_tmp32E->le == 0){if(!Cyc_Tcutil_is_const_exp(
_tmp32E,bound)){const char*_tmp750;void*_tmp74F;(_tmp74F=0,Cyc_Tcutil_terr(bound->loc,((
_tmp750="bound is not constant",_tag_dyneither(_tmp750,sizeof(char),22))),
_tag_dyneither(_tmp74F,sizeof(void*),0)));}if(!Cyc_Tcutil_is_const_exp(_tmp32E,
body)){const char*_tmp753;void*_tmp752;(_tmp752=0,Cyc_Tcutil_terr(bound->loc,((
_tmp753="body is not constant",_tag_dyneither(_tmp753,sizeof(char),21))),
_tag_dyneither(_tmp752,sizeof(void*),0)));}}if(_tmp331 != 0  && ((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,*_tmp331)){struct Cyc_Absyn_Exp*
_tmp340=Cyc_Absyn_uint_exp(1,0);{struct Cyc_Core_Opt*_tmp754;_tmp340->topt=((
_tmp754=_cycalloc(sizeof(*_tmp754)),((_tmp754->v=(void*)Cyc_Absyn_uint_typ,
_tmp754))));}bound=Cyc_Absyn_add_exp(bound,_tmp340,0);{struct Cyc_Core_Opt*
_tmp755;bound->topt=((_tmp755=_cycalloc(sizeof(*_tmp755)),((_tmp755->v=(void*)
Cyc_Absyn_uint_typ,_tmp755))));}*is_zero_term=1;}{void*_tmp343=Cyc_Absyn_array_typ(
t,_tmp330 == 0?Cyc_Absyn_empty_tqual(0):*_tmp330,(struct Cyc_Absyn_Exp*)bound,
_tmp331 == 0?Cyc_Absyn_false_conref:*_tmp331,0);return _tmp343;}}}}}struct _tuple10{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**
tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**
ad_opt);static void*Cyc_Tcexp_tcStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*
args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*ad;if(*ad_opt != 
0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{struct _handler_cons
_tmp344;_push_handler(& _tmp344);{int _tmp346=0;if(setjmp(_tmp344.handler))_tmp346=
1;if(!_tmp346){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;_pop_handler();}else{
void*_tmp345=(void*)_exn_thrown;void*_tmp348=_tmp345;_LL1CE: if(_tmp348 != Cyc_Dict_Absent)
goto _LL1D0;_LL1CF:{const char*_tmp759;void*_tmp758[1];struct Cyc_String_pa_struct
_tmp757;(_tmp757.tag=0,((_tmp757.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(*tn)),((_tmp758[0]=& _tmp757,Cyc_Tcutil_terr(loc,((
_tmp759="unbound struct name %s",_tag_dyneither(_tmp759,sizeof(char),23))),
_tag_dyneither(_tmp758,sizeof(void*),1)))))));}return topt != 0?*topt:(void*)0;
_LL1D0:;_LL1D1:(void)_throw(_tmp348);_LL1CD:;}}}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)
ad;*tn=ad->name;}if((void*)ad->kind == (void*)1){const char*_tmp75C;void*_tmp75B;(
_tmp75B=0,Cyc_Tcutil_terr(loc,((_tmp75C="expecting struct but found union",
_tag_dyneither(_tmp75C,sizeof(char),33))),_tag_dyneither(_tmp75B,sizeof(void*),0)));}
if(ad->impl == 0){{const char*_tmp75F;void*_tmp75E;(_tmp75E=0,Cyc_Tcutil_terr(loc,((
_tmp75F="can't build abstract struct",_tag_dyneither(_tmp75F,sizeof(char),28))),
_tag_dyneither(_tmp75E,sizeof(void*),0)));}return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}{struct _RegionHandle*_tmp350=Cyc_Tcenv_get_fnrgn(te);struct _tuple4 _tmp760;
struct _tuple4 _tmp351=(_tmp760.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp760.f2=
_tmp350,_tmp760)));struct Cyc_List_List*_tmp352=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp350,Cyc_Tcutil_r_make_inst_var,&
_tmp351,ad->tvs);struct Cyc_List_List*_tmp353=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp350,Cyc_Tcutil_r_make_inst_var,&
_tmp351,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp354=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp352);struct Cyc_List_List*_tmp355=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmp353);struct Cyc_List_List*_tmp356=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp350,_tmp352,_tmp353);struct Cyc_Absyn_AggrType_struct _tmp76F;union Cyc_Absyn_AggrInfoU_union
_tmp76E;struct Cyc_Absyn_Aggrdecl**_tmp76D;struct Cyc_Absyn_AggrInfo _tmp76C;struct
Cyc_Absyn_AggrType_struct*_tmp76B;struct Cyc_Absyn_AggrType_struct*_tmp357=(
_tmp76B=_cycalloc(sizeof(*_tmp76B)),((_tmp76B[0]=((_tmp76F.tag=10,((_tmp76F.f1=((
_tmp76C.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((_tmp76E.KnownAggr).tag=1,(((
_tmp76E.KnownAggr).f1=((_tmp76D=_cycalloc(sizeof(*_tmp76D)),((_tmp76D[0]=ad,
_tmp76D)))),_tmp76E)))),((_tmp76C.targs=_tmp354,_tmp76C)))),_tmp76F)))),_tmp76B)));
struct Cyc_List_List*_tmp358=*ts;struct Cyc_List_List*_tmp359=_tmp355;while(
_tmp358 != 0  && _tmp359 != 0){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)0,1,(void*)_tmp358->hd);Cyc_Tcutil_unify((void*)_tmp358->hd,(void*)
_tmp359->hd);_tmp358=_tmp358->tl;_tmp359=_tmp359->tl;}if(_tmp358 != 0){const char*
_tmp772;void*_tmp771;(_tmp771=0,Cyc_Tcutil_terr(loc,((_tmp772="too many explicit witness types",
_tag_dyneither(_tmp772,sizeof(char),32))),_tag_dyneither(_tmp771,sizeof(void*),0)));}*
ts=_tmp355;if(topt != 0)Cyc_Tcutil_unify((void*)_tmp357,*topt);{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
_tmp350,loc,args,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){struct _tuple10 _tmp35D;struct Cyc_Absyn_Aggrfield*
_tmp35E;struct Cyc_Absyn_Exp*_tmp35F;struct _tuple10*_tmp35C=(struct _tuple10*)
fields->hd;_tmp35D=*_tmp35C;_tmp35E=_tmp35D.f1;_tmp35F=_tmp35D.f2;{void*_tmp360=
Cyc_Tcutil_rsubstitute(_tmp350,_tmp356,(void*)_tmp35E->type);Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp360,_tmp35F);if(!Cyc_Tcutil_coerce_arg(te,_tmp35F,_tmp360)){{
const char*_tmp779;void*_tmp778[4];struct Cyc_String_pa_struct _tmp777;struct Cyc_String_pa_struct
_tmp776;struct Cyc_String_pa_struct _tmp775;struct Cyc_String_pa_struct _tmp774;(
_tmp774.tag=0,((_tmp774.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp35F->topt))->v)),((_tmp775.tag=0,((
_tmp775.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp360)),((_tmp776.tag=0,((_tmp776.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp777.tag=0,((_tmp777.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp35E->name),((_tmp778[0]=&
_tmp777,((_tmp778[1]=& _tmp776,((_tmp778[2]=& _tmp775,((_tmp778[3]=& _tmp774,Cyc_Tcutil_terr(
_tmp35F->loc,((_tmp779="field %s of struct %s expects type %s != %s",
_tag_dyneither(_tmp779,sizeof(char),44))),_tag_dyneither(_tmp778,sizeof(void*),4)))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}}{struct Cyc_List_List*_tmp367=0;{struct Cyc_List_List*
_tmp368=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;
_tmp368 != 0;_tmp368=_tmp368->tl){struct _tuple0*_tmp77C;struct Cyc_List_List*
_tmp77B;_tmp367=((_tmp77B=_cycalloc(sizeof(*_tmp77B)),((_tmp77B->hd=((_tmp77C=
_cycalloc(sizeof(*_tmp77C)),((_tmp77C->f1=Cyc_Tcutil_rsubstitute(_tmp350,_tmp356,(*((
struct _tuple0*)_tmp368->hd)).f1),((_tmp77C->f2=Cyc_Tcutil_rsubstitute(_tmp350,
_tmp356,(*((struct _tuple0*)_tmp368->hd)).f2),_tmp77C)))))),((_tmp77B->tl=_tmp367,
_tmp77B))))));}}_tmp367=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp367);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp367);return(void*)_tmp357;}}}}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args);static void*Cyc_Tcexp_tcAnonStruct(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*
args){struct _RegionHandle*_tmp371=Cyc_Tcenv_get_fnrgn(te);{void*_tmp372=Cyc_Tcutil_compress(
ts);void*_tmp373;struct Cyc_List_List*_tmp374;_LL1D3: if(_tmp372 <= (void*)4)goto
_LL1D5;if(*((int*)_tmp372)!= 11)goto _LL1D5;_tmp373=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp372)->f1;_tmp374=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp372)->f2;_LL1D4:
if(_tmp373 == (void*)1){const char*_tmp77F;void*_tmp77E;(_tmp77E=0,Cyc_Tcutil_terr(
loc,((_tmp77F="expecting struct but found union",_tag_dyneither(_tmp77F,sizeof(
char),33))),_tag_dyneither(_tmp77E,sizeof(void*),0)));}{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
_tmp371,loc,args,_tmp374);for(0;fields != 0;fields=fields->tl){struct _tuple10
_tmp378;struct Cyc_Absyn_Aggrfield*_tmp379;struct Cyc_Absyn_Exp*_tmp37A;struct
_tuple10*_tmp377=(struct _tuple10*)fields->hd;_tmp378=*_tmp377;_tmp379=_tmp378.f1;
_tmp37A=_tmp378.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)((void**)((void*)&
_tmp379->type)),_tmp37A);if(!Cyc_Tcutil_coerce_arg(te,_tmp37A,(void*)_tmp379->type)){{
const char*_tmp785;void*_tmp784[3];struct Cyc_String_pa_struct _tmp783;struct Cyc_String_pa_struct
_tmp782;struct Cyc_String_pa_struct _tmp781;(_tmp781.tag=0,((_tmp781.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp37A->topt))->v)),((_tmp782.tag=0,((_tmp782.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmp379->type)),((
_tmp783.tag=0,((_tmp783.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp379->name),((
_tmp784[0]=& _tmp783,((_tmp784[1]=& _tmp782,((_tmp784[2]=& _tmp781,Cyc_Tcutil_terr(
_tmp37A->loc,((_tmp785="field %s of struct expects type %s != %s",_tag_dyneither(
_tmp785,sizeof(char),41))),_tag_dyneither(_tmp784,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}goto _LL1D2;}_LL1D5:;_LL1D6: {const char*_tmp788;
void*_tmp787;(_tmp787=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp788="tcAnonStruct: wrong type",_tag_dyneither(_tmp788,
sizeof(char),25))),_tag_dyneither(_tmp787,sizeof(void*),0)));}_LL1D2:;}return ts;}
static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*
tud,struct Cyc_Absyn_Tunionfield*tuf);static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
es,struct Cyc_Absyn_Tuniondecl*tud,struct Cyc_Absyn_Tunionfield*tuf){struct
_RegionHandle*_tmp382=Cyc_Tcenv_get_fnrgn(te);struct _tuple4 _tmp789;struct _tuple4
_tmp383=(_tmp789.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp789.f2=_tmp382,_tmp789)));
struct Cyc_List_List*_tmp384=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp382,Cyc_Tcutil_r_make_inst_var,&
_tmp383,tud->tvs);struct Cyc_List_List*_tmp385=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp384);struct Cyc_Absyn_TunionFieldType_struct _tmp793;union Cyc_Absyn_TunionFieldInfoU_union
_tmp792;struct Cyc_Absyn_TunionFieldInfo _tmp791;struct Cyc_Absyn_TunionFieldType_struct*
_tmp790;void*res=(void*)((_tmp790=_cycalloc(sizeof(*_tmp790)),((_tmp790[0]=((
_tmp793.tag=3,((_tmp793.f1=((_tmp791.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)(((
_tmp792.KnownTunionfield).tag=1,(((_tmp792.KnownTunionfield).f1=tud,(((_tmp792.KnownTunionfield).f2=
tuf,_tmp792)))))),((_tmp791.targs=_tmp385,_tmp791)))),_tmp793)))),_tmp790))));
if(topt != 0){void*_tmp386=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo
_tmp387;struct Cyc_List_List*_tmp388;struct Cyc_Core_Opt*_tmp389;_LL1D8: if(_tmp386
<= (void*)4)goto _LL1DC;if(*((int*)_tmp386)!= 3)goto _LL1DA;_LL1D9: Cyc_Tcutil_unify(*
topt,res);goto _LL1D7;_LL1DA: if(*((int*)_tmp386)!= 2)goto _LL1DC;_tmp387=((struct
Cyc_Absyn_TunionType_struct*)_tmp386)->f1;_tmp388=_tmp387.targs;_tmp389=_tmp387.rgn;
_LL1DB:{struct Cyc_List_List*a=_tmp385;for(0;a != 0  && _tmp388 != 0;(a=a->tl,
_tmp388=_tmp388->tl)){Cyc_Tcutil_unify((void*)a->hd,(void*)_tmp388->hd);}}if(tud->is_flat
 || tuf->typs == 0  && es == 0){{struct Cyc_Core_Opt*_tmp794;e->topt=((_tmp794=
_cycalloc(sizeof(*_tmp794)),((_tmp794->v=(void*)res,_tmp794))));}{struct Cyc_Absyn_TunionType_struct
_tmp7A3;union Cyc_Absyn_TunionInfoU_union _tmp7A2;struct Cyc_Absyn_Tuniondecl**
_tmp7A1;struct Cyc_Absyn_TunionInfo _tmp7A0;struct Cyc_Absyn_TunionType_struct*
_tmp79F;res=(void*)((_tmp79F=_cycalloc(sizeof(*_tmp79F)),((_tmp79F[0]=((_tmp7A3.tag=
2,((_tmp7A3.f1=((_tmp7A0.tunion_info=(union Cyc_Absyn_TunionInfoU_union)(((
_tmp7A2.KnownTunion).tag=1,(((_tmp7A2.KnownTunion).f1=((_tmp7A1=_cycalloc(
sizeof(*_tmp7A1)),((_tmp7A1[0]=tud,_tmp7A1)))),_tmp7A2)))),((_tmp7A0.targs=
_tmp385,((_tmp7A0.rgn=_tmp389,_tmp7A0)))))),_tmp7A3)))),_tmp79F))));}Cyc_Tcutil_unchecked_cast(
te,e,res,(void*)1);}goto _LL1D7;_LL1DC:;_LL1DD: goto _LL1D7;_LL1D7:;}{struct Cyc_List_List*
ts=tuf->typs;for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(
struct Cyc_Absyn_Exp*)es->hd;void*t=Cyc_Tcutil_rsubstitute(_tmp382,_tmp384,(*((
struct _tuple9*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t)){{const char*_tmp7AB;const char*_tmp7AA;void*_tmp7A9[3];struct Cyc_String_pa_struct
_tmp7A8;struct Cyc_String_pa_struct _tmp7A7;struct Cyc_String_pa_struct _tmp7A6;(
_tmp7A6.tag=0,((_tmp7A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(e->topt
== 0?(struct _dyneither_ptr)((_tmp7AB="?",_tag_dyneither(_tmp7AB,sizeof(char),2))):
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v))),((
_tmp7A7.tag=0,((_tmp7A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp7A8.tag=0,((_tmp7A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp7A9[0]=& _tmp7A8,((_tmp7A9[1]=& _tmp7A7,((
_tmp7A9[2]=& _tmp7A6,Cyc_Tcutil_terr(e->loc,((_tmp7AA="tunion constructor %s expects argument of type %s but this argument has type %s",
_tag_dyneither(_tmp7AA,sizeof(char),80))),_tag_dyneither(_tmp7A9,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}if(es != 0){const char*_tmp7AF;void*_tmp7AE[1];
struct Cyc_String_pa_struct _tmp7AD;return(_tmp7AD.tag=0,((_tmp7AD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp7AE[0]=& _tmp7AD,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,
topt,((_tmp7AF="too many arguments for tunion constructor %s",_tag_dyneither(
_tmp7AF,sizeof(char),45))),_tag_dyneither(_tmp7AE,sizeof(void*),1)))))));}if(ts
!= 0){const char*_tmp7B3;void*_tmp7B2[1];struct Cyc_String_pa_struct _tmp7B1;return(
_tmp7B1.tag=0,((_tmp7B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name)),((_tmp7B2[0]=& _tmp7B1,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7B3="too few arguments for tunion constructor %s",
_tag_dyneither(_tmp7B3,sizeof(char),44))),_tag_dyneither(_tmp7B2,sizeof(void*),1)))))));}
return res;}}static int Cyc_Tcexp_zeroable_type(void*t);static int Cyc_Tcexp_zeroable_type(
void*t){void*_tmp3A1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp3A2;
struct Cyc_Absyn_PtrAtts _tmp3A3;struct Cyc_Absyn_Conref*_tmp3A4;struct Cyc_Absyn_ArrayInfo
_tmp3A5;void*_tmp3A6;struct Cyc_List_List*_tmp3A7;struct Cyc_Absyn_AggrInfo _tmp3A8;
union Cyc_Absyn_AggrInfoU_union _tmp3A9;struct Cyc_List_List*_tmp3AA;struct Cyc_List_List*
_tmp3AB;_LL1DF: if((int)_tmp3A1 != 0)goto _LL1E1;_LL1E0: return 1;_LL1E1: if(_tmp3A1 <= (
void*)4)goto _LL1ED;if(*((int*)_tmp3A1)!= 0)goto _LL1E3;_LL1E2: goto _LL1E4;_LL1E3:
if(*((int*)_tmp3A1)!= 1)goto _LL1E5;_LL1E4: goto _LL1E6;_LL1E5: if(*((int*)_tmp3A1)
!= 2)goto _LL1E7;_LL1E6: goto _LL1E8;_LL1E7: if(*((int*)_tmp3A1)!= 3)goto _LL1E9;
_LL1E8: return 0;_LL1E9: if(*((int*)_tmp3A1)!= 4)goto _LL1EB;_tmp3A2=((struct Cyc_Absyn_PointerType_struct*)
_tmp3A1)->f1;_tmp3A3=_tmp3A2.ptr_atts;_tmp3A4=_tmp3A3.nullable;_LL1EA: return((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp3A4);_LL1EB: if(*((
int*)_tmp3A1)!= 5)goto _LL1ED;_LL1EC: goto _LL1EE;_LL1ED: if((int)_tmp3A1 != 1)goto
_LL1EF;_LL1EE: goto _LL1F0;_LL1EF: if(_tmp3A1 <= (void*)4)goto _LL209;if(*((int*)
_tmp3A1)!= 6)goto _LL1F1;_LL1F0: return 1;_LL1F1: if(*((int*)_tmp3A1)!= 7)goto _LL1F3;
_tmp3A5=((struct Cyc_Absyn_ArrayType_struct*)_tmp3A1)->f1;_tmp3A6=(void*)_tmp3A5.elt_type;
_LL1F2: return Cyc_Tcexp_zeroable_type(_tmp3A6);_LL1F3: if(*((int*)_tmp3A1)!= 8)
goto _LL1F5;_LL1F4: return 0;_LL1F5: if(*((int*)_tmp3A1)!= 9)goto _LL1F7;_tmp3A7=((
struct Cyc_Absyn_TupleType_struct*)_tmp3A1)->f1;_LL1F6: for(0;(unsigned int)
_tmp3A7;_tmp3A7=_tmp3A7->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple9*)
_tmp3A7->hd)).f2))return 0;}return 1;_LL1F7: if(*((int*)_tmp3A1)!= 10)goto _LL1F9;
_tmp3A8=((struct Cyc_Absyn_AggrType_struct*)_tmp3A1)->f1;_tmp3A9=_tmp3A8.aggr_info;
_tmp3AA=_tmp3A8.targs;_LL1F8: {struct Cyc_Absyn_Aggrdecl*_tmp3AC=Cyc_Absyn_get_known_aggrdecl(
_tmp3A9);if(_tmp3AC->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp3AC->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp3AD=_new_region("r");
struct _RegionHandle*r=& _tmp3AD;_push_region(r);{struct Cyc_List_List*_tmp3AE=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp3AC->tvs,_tmp3AA);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3AC->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp3AE,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp3AF=0;_npop_handler(0);
return _tmp3AF;}}}{int _tmp3B0=1;_npop_handler(0);return _tmp3B0;}};_pop_region(r);}}
_LL1F9: if(*((int*)_tmp3A1)!= 12)goto _LL1FB;_LL1FA: return 1;_LL1FB: if(*((int*)
_tmp3A1)!= 18)goto _LL1FD;_LL1FC: return 1;_LL1FD: if(*((int*)_tmp3A1)!= 11)goto
_LL1FF;_tmp3AB=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp3A1)->f2;_LL1FE: for(0;
_tmp3AB != 0;_tmp3AB=_tmp3AB->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp3AB->hd)->type))return 0;}return 1;_LL1FF: if(*((int*)_tmp3A1)!= 13)goto _LL201;
_LL200: return 1;_LL201: if(*((int*)_tmp3A1)!= 16)goto _LL203;_LL202: return 0;_LL203:
if(*((int*)_tmp3A1)!= 15)goto _LL205;_LL204: return 0;_LL205: if(*((int*)_tmp3A1)!= 
14)goto _LL207;_LL206: return 0;_LL207: if(*((int*)_tmp3A1)!= 17)goto _LL209;_LL208:
goto _LL20A;_LL209: if((int)_tmp3A1 != 2)goto _LL20B;_LL20A: goto _LL20C;_LL20B: if((
int)_tmp3A1 != 3)goto _LL20D;_LL20C: goto _LL20E;_LL20D: if(_tmp3A1 <= (void*)4)goto
_LL20F;if(*((int*)_tmp3A1)!= 19)goto _LL20F;_LL20E: goto _LL210;_LL20F: if(_tmp3A1 <= (
void*)4)goto _LL211;if(*((int*)_tmp3A1)!= 20)goto _LL211;_LL210: goto _LL212;_LL211:
if(_tmp3A1 <= (void*)4)goto _LL1DE;if(*((int*)_tmp3A1)!= 21)goto _LL1DE;_LL212: {
const char*_tmp7B7;void*_tmp7B6[1];struct Cyc_String_pa_struct _tmp7B5;(_tmp7B5.tag=
0,((_tmp7B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp7B6[0]=& _tmp7B5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp7B7="bad type `%s' in zeroable type",_tag_dyneither(
_tmp7B7,sizeof(char),31))),_tag_dyneither(_tmp7B6,sizeof(void*),1)))))));}_LL1DE:;}
static void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*
loc,void**topt,void*t);static void Cyc_Tcexp_check_malloc_type(int allow_zero,
struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || 
allow_zero  && Cyc_Tcexp_zeroable_type(t))return;if(topt != 0){void*_tmp3B4=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp3B5;void*_tmp3B6;_LL214: if(_tmp3B4 <= (void*)4)
goto _LL216;if(*((int*)_tmp3B4)!= 4)goto _LL216;_tmp3B5=((struct Cyc_Absyn_PointerType_struct*)
_tmp3B4)->f1;_tmp3B6=(void*)_tmp3B5.elt_typ;_LL215: Cyc_Tcutil_unify(_tmp3B6,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL213;_LL216:;_LL217: goto _LL213;_LL213:;}{const char*_tmp7C0;const char*
_tmp7BF;const char*_tmp7BE;void*_tmp7BD[2];struct Cyc_String_pa_struct _tmp7BC;
struct Cyc_String_pa_struct _tmp7BB;(_tmp7BB.tag=0,((_tmp7BB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7BC.tag=0,((
_tmp7BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct
_dyneither_ptr)((_tmp7BF="calloc",_tag_dyneither(_tmp7BF,sizeof(char),7))):(
struct _dyneither_ptr)((_tmp7C0="malloc",_tag_dyneither(_tmp7C0,sizeof(char),7))))),((
_tmp7BD[0]=& _tmp7BC,((_tmp7BD[1]=& _tmp7BB,Cyc_Tcutil_terr(loc,((_tmp7BE="%s cannot be used with type %s\n\t(type needs initialization)",
_tag_dyneither(_tmp7BE,sizeof(char),60))),_tag_dyneither(_tmp7BD,sizeof(void*),2)))))))))))));}}
static void*Cyc_Tcexp_mallocRgn(void*rgn);static void*Cyc_Tcexp_mallocRgn(void*rgn){
void*_tmp3BD=Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn));_LL219: if((int)
_tmp3BD != 4)goto _LL21B;_LL21A: return(void*)3;_LL21B:;_LL21C: return(void*)2;
_LL218:;}static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*ropt,void***t,struct Cyc_Absyn_Exp**e,int*
is_calloc,int*is_fat);static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*ropt,void***t,
struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)2;if(ropt != 0){
struct Cyc_Absyn_RgnHandleType_struct _tmp7C3;struct Cyc_Absyn_RgnHandleType_struct*
_tmp7C2;void*expected_type=(void*)((_tmp7C2=_cycalloc(sizeof(*_tmp7C2)),((
_tmp7C2[0]=((_tmp7C3.tag=14,((_tmp7C3.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp7C3)))),_tmp7C2))));void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)ropt);
void*_tmp3BE=Cyc_Tcutil_compress(handle_type);void*_tmp3BF;_LL21E: if(_tmp3BE <= (
void*)4)goto _LL220;if(*((int*)_tmp3BE)!= 14)goto _LL220;_tmp3BF=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp3BE)->f1;_LL21F: rgn=_tmp3BF;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL21D;_LL220:;_LL221:{const char*_tmp7C7;void*_tmp7C6[1];struct
Cyc_String_pa_struct _tmp7C5;(_tmp7C5.tag=0,((_tmp7C5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type)),((_tmp7C6[0]=& _tmp7C5,
Cyc_Tcutil_terr(ropt->loc,((_tmp7C7="expecting region_t type but found %s",
_tag_dyneither(_tmp7C7,sizeof(char),37))),_tag_dyneither(_tmp7C6,sizeof(void*),1)))))));}
goto _LL21D;_LL21D:;}else{if(topt != 0){void*optrgn=(void*)0;if(Cyc_Tcutil_rgn_of_pointer(*
topt,& optrgn))rgn=Cyc_Tcexp_mallocRgn(optrgn);}}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*
e);{void*elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*is_calloc){if(*t == 
0){const char*_tmp7CA;void*_tmp7C9;(_tmp7C9=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7CA="calloc with empty type",
_tag_dyneither(_tmp7CA,sizeof(char),23))),_tag_dyneither(_tmp7C9,sizeof(void*),0)));}
elt_type=*((void**)_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;one_elt=0;}else{void*_tmp3C7=(void*)(*e)->r;void*_tmp3C8;void*
_tmp3C9;struct Cyc_List_List*_tmp3CA;struct Cyc_List_List _tmp3CB;struct Cyc_Absyn_Exp*
_tmp3CC;struct Cyc_List_List*_tmp3CD;struct Cyc_List_List _tmp3CE;struct Cyc_Absyn_Exp*
_tmp3CF;struct Cyc_List_List*_tmp3D0;_LL223: if(*((int*)_tmp3C7)!= 18)goto _LL225;
_tmp3C8=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3C7)->f1;_LL224:
elt_type=_tmp3C8;{void**_tmp7CB;*t=(void**)((_tmp7CB=_cycalloc(sizeof(*_tmp7CB)),((
_tmp7CB[0]=elt_type,_tmp7CB))));}num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL222;_LL225: if(*((int*)
_tmp3C7)!= 3)goto _LL227;_tmp3C9=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3C7)->f1;if((int)_tmp3C9 != 1)goto _LL227;_tmp3CA=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3C7)->f2;if(_tmp3CA == 0)goto _LL227;_tmp3CB=*_tmp3CA;_tmp3CC=(struct Cyc_Absyn_Exp*)
_tmp3CB.hd;_tmp3CD=_tmp3CB.tl;if(_tmp3CD == 0)goto _LL227;_tmp3CE=*_tmp3CD;_tmp3CF=(
struct Cyc_Absyn_Exp*)_tmp3CE.hd;_tmp3D0=_tmp3CE.tl;if(_tmp3D0 != 0)goto _LL227;
_LL226:{struct _tuple0 _tmp7CC;struct _tuple0 _tmp3D3=(_tmp7CC.f1=(void*)_tmp3CC->r,((
_tmp7CC.f2=(void*)_tmp3CF->r,_tmp7CC)));void*_tmp3D4;void*_tmp3D5;void*_tmp3D6;
void*_tmp3D7;_LL22A: _tmp3D4=_tmp3D3.f1;if(*((int*)_tmp3D4)!= 18)goto _LL22C;
_tmp3D5=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3D4)->f1;_LL22B: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3D5);elt_type=_tmp3D5;{void**_tmp7CD;*t=(void**)((_tmp7CD=
_cycalloc(sizeof(*_tmp7CD)),((_tmp7CD[0]=elt_type,_tmp7CD))));}num_elts=_tmp3CF;
one_elt=0;goto _LL229;_LL22C: _tmp3D6=_tmp3D3.f2;if(*((int*)_tmp3D6)!= 18)goto
_LL22E;_tmp3D7=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3D6)->f1;_LL22D:
Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3D7);elt_type=_tmp3D7;{void**_tmp7CE;*
t=(void**)((_tmp7CE=_cycalloc(sizeof(*_tmp7CE)),((_tmp7CE[0]=elt_type,_tmp7CE))));}
num_elts=_tmp3CC;one_elt=0;goto _LL229;_LL22E:;_LL22F: goto No_sizeof;_LL229:;}goto
_LL222;_LL227:;_LL228: No_sizeof: elt_type=Cyc_Absyn_char_typ;{void**_tmp7CF;*t=(
void**)((_tmp7CF=_cycalloc(sizeof(*_tmp7CF)),((_tmp7CF[0]=elt_type,_tmp7CF))));}
num_elts=*e;one_elt=0;goto _LL222;_LL222:;}*e=num_elts;*is_fat=!one_elt;{void*
_tmp3DB=elt_type;struct Cyc_Absyn_AggrInfo _tmp3DC;union Cyc_Absyn_AggrInfoU_union
_tmp3DD;struct Cyc_Absyn_Aggrdecl**_tmp3DE;struct Cyc_Absyn_Aggrdecl*_tmp3DF;
_LL231: if(_tmp3DB <= (void*)4)goto _LL233;if(*((int*)_tmp3DB)!= 10)goto _LL233;
_tmp3DC=((struct Cyc_Absyn_AggrType_struct*)_tmp3DB)->f1;_tmp3DD=_tmp3DC.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp3DB)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL233;_tmp3DE=(_tmp3DD.KnownAggr).f1;_tmp3DF=*_tmp3DE;_LL232: if(
_tmp3DF->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3DF->impl))->exist_vars
!= 0){const char*_tmp7D2;void*_tmp7D1;(_tmp7D1=0,Cyc_Tcutil_terr(loc,((_tmp7D2="malloc with existential types not yet implemented",
_tag_dyneither(_tmp7D2,sizeof(char),50))),_tag_dyneither(_tmp7D1,sizeof(void*),0)));}
goto _LL230;_LL233:;_LL234: goto _LL230;_LL230:;}{void*(*_tmp3E2)(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term)=Cyc_Absyn_at_typ;
struct Cyc_Absyn_Conref*_tmp3E3=Cyc_Absyn_false_conref;if(topt != 0){void*_tmp3E4=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp3E5;struct Cyc_Absyn_PtrAtts
_tmp3E6;struct Cyc_Absyn_Conref*_tmp3E7;struct Cyc_Absyn_Conref*_tmp3E8;struct Cyc_Absyn_Conref*
_tmp3E9;_LL236: if(_tmp3E4 <= (void*)4)goto _LL238;if(*((int*)_tmp3E4)!= 4)goto
_LL238;_tmp3E5=((struct Cyc_Absyn_PointerType_struct*)_tmp3E4)->f1;_tmp3E6=
_tmp3E5.ptr_atts;_tmp3E7=_tmp3E6.nullable;_tmp3E8=_tmp3E6.bounds;_tmp3E9=_tmp3E6.zero_term;
_LL237: _tmp3E3=_tmp3E9;if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3E7))_tmp3E2=Cyc_Absyn_star_typ;if(((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp3E9) && !(*is_calloc)){{const char*_tmp7D5;void*
_tmp7D4;(_tmp7D4=0,Cyc_Tcutil_warn(loc,((_tmp7D5="converting malloc to calloc to ensure zero-termination",
_tag_dyneither(_tmp7D5,sizeof(char),55))),_tag_dyneither(_tmp7D4,sizeof(void*),0)));}*
is_calloc=1;}{void*_tmp3EC=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp3E8);
struct Cyc_Absyn_Exp*_tmp3ED;_LL23B: if((int)_tmp3EC != 0)goto _LL23D;_LL23C: goto
_LL23A;_LL23D: if(_tmp3EC <= (void*)1)goto _LL23F;if(*((int*)_tmp3EC)!= 0)goto
_LL23F;_tmp3ED=((struct Cyc_Absyn_Upper_b_struct*)_tmp3EC)->f1;if(!(!one_elt))
goto _LL23F;_LL23E: {int _tmp3EE=Cyc_Evexp_c_can_eval(num_elts);if(_tmp3EE  && Cyc_Evexp_same_const_exp(
_tmp3ED,num_elts)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_conref_val(_tmp3E8),_tmp3E3);}{void*_tmp3EF=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(num_elts->topt))->v);void*_tmp3F0;_LL242:
if(_tmp3EF <= (void*)4)goto _LL244;if(*((int*)_tmp3EF)!= 18)goto _LL244;_tmp3F0=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp3EF)->f1;_LL243: {struct Cyc_Absyn_Exp*
_tmp3F1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp3F0,0),0,(
void*)1,0);if(Cyc_Evexp_same_const_exp(_tmp3F1,_tmp3ED)){*is_fat=0;return Cyc_Absyn_atb_typ(
elt_type,rgn,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_conref_val(_tmp3E8),_tmp3E3);}
goto _LL241;}_LL244:;_LL245: goto _LL241;_LL241:;}goto _LL23A;}_LL23F:;_LL240: goto
_LL23A;_LL23A:;}goto _LL235;_LL238:;_LL239: goto _LL235;_LL235:;}if(!one_elt)
_tmp3E2=Cyc_Absyn_dyneither_typ;return _tmp3E2(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),_tmp3E3);}}}static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcSwap(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((
void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp3F2=Cyc_Tcutil_compress(t1);_LL247: if(
_tmp3F2 <= (void*)4)goto _LL249;if(*((int*)_tmp3F2)!= 7)goto _LL249;_LL248:{const
char*_tmp7D8;void*_tmp7D7;(_tmp7D7=0,Cyc_Tcutil_terr(loc,((_tmp7D8="cannot assign to an array",
_tag_dyneither(_tmp7D8,sizeof(char),26))),_tag_dyneither(_tmp7D7,sizeof(void*),0)));}
goto _LL246;_LL249:;_LL24A: goto _LL246;_LL246:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(
t1,& ign_1)){const char*_tmp7DB;void*_tmp7DA;(_tmp7DA=0,Cyc_Tcutil_terr(loc,((
_tmp7DB="Swap not allowed for non-pointer or non-word-sized types.",
_tag_dyneither(_tmp7DB,sizeof(char),58))),_tag_dyneither(_tmp7DA,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp7DE;void*_tmp7DD;return(_tmp7DD=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp7DE="swap non-lvalue",_tag_dyneither(_tmp7DE,sizeof(char),
16))),_tag_dyneither(_tmp7DD,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp7E1;void*_tmp7E0;return(_tmp7E0=0,Cyc_Tcexp_expr_err(te,e2->loc,
topt,((_tmp7E1="swap non-lvalue",_tag_dyneither(_tmp7E1,sizeof(char),16))),
_tag_dyneither(_tmp7E0,sizeof(void*),0)));}{void*t_ign1=(void*)0;void*t_ign2=(
void*)0;int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp7E4;void*_tmp7E3;return(_tmp7E3=0,Cyc_Tcexp_expr_err(te,e1->loc,
topt,((_tmp7E4="swap value in zeroterm array",_tag_dyneither(_tmp7E4,sizeof(char),
29))),_tag_dyneither(_tmp7E3,sizeof(void*),0)));}if(Cyc_Tcutil_is_zero_ptr_deref(
e2,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp7E7;void*_tmp7E6;return(_tmp7E6=0,Cyc_Tcexp_expr_err(
te,e2->loc,topt,((_tmp7E7="swap value in zeroterm array",_tag_dyneither(_tmp7E7,
sizeof(char),29))),_tag_dyneither(_tmp7E6,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(t1,t2)){const char*
_tmp7EC;void*_tmp7EB[2];struct Cyc_String_pa_struct _tmp7EA;struct Cyc_String_pa_struct
_tmp7E9;void*_tmp3FF=(_tmp7E9.tag=0,((_tmp7E9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp7EA.tag=0,((_tmp7EA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7EB[0]=&
_tmp7EA,((_tmp7EB[1]=& _tmp7E9,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7EC="type mismatch: %s != %s",
_tag_dyneither(_tmp7EC,sizeof(char),24))),_tag_dyneither(_tmp7EB,sizeof(void*),2)))))))))))));
return _tmp3FF;}return(void*)0;}}}}int Cyc_Tcexp_in_stmt_exp=0;static void*Cyc_Tcexp_tcStmtExp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Stmt*
s);static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){struct _RegionHandle*_tmp404=Cyc_Tcenv_get_fnrgn(
te);{int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
_tmp404,te,s),s,1);Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp405=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp406;struct Cyc_Absyn_Stmt*_tmp407;struct Cyc_Absyn_Stmt*_tmp408;struct Cyc_Absyn_Decl*
_tmp409;struct Cyc_Absyn_Stmt*_tmp40A;_LL24C: if(_tmp405 <= (void*)1)goto _LL252;if(*((
int*)_tmp405)!= 0)goto _LL24E;_tmp406=((struct Cyc_Absyn_Exp_s_struct*)_tmp405)->f1;
_LL24D: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp406->topt))->v;_LL24E:
if(*((int*)_tmp405)!= 1)goto _LL250;_tmp407=((struct Cyc_Absyn_Seq_s_struct*)
_tmp405)->f1;_tmp408=((struct Cyc_Absyn_Seq_s_struct*)_tmp405)->f2;_LL24F: s=
_tmp408;continue;_LL250: if(*((int*)_tmp405)!= 11)goto _LL252;_tmp409=((struct Cyc_Absyn_Decl_s_struct*)
_tmp405)->f1;_tmp40A=((struct Cyc_Absyn_Decl_s_struct*)_tmp405)->f2;_LL251: s=
_tmp40A;continue;_LL252:;_LL253: {const char*_tmp7EF;void*_tmp7EE;return(_tmp7EE=
0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7EF="statement expression must end with expression",
_tag_dyneither(_tmp7EF,sizeof(char),46))),_tag_dyneither(_tmp7EE,sizeof(void*),0)));}
_LL24B:;}}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1);static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1){void*rgn=(void*)2;if(rgn_handle != 0){struct Cyc_Absyn_RgnHandleType_struct
_tmp7F2;struct Cyc_Absyn_RgnHandleType_struct*_tmp7F1;void*expected_type=(void*)((
_tmp7F1=_cycalloc(sizeof(*_tmp7F1)),((_tmp7F1[0]=((_tmp7F2.tag=14,((_tmp7F2.f1=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(
te)),_tmp7F2)))),_tmp7F1))));void*handle_type=Cyc_Tcexp_tcExp(te,(void**)&
expected_type,(struct Cyc_Absyn_Exp*)rgn_handle);void*_tmp40D=Cyc_Tcutil_compress(
handle_type);void*_tmp40E;_LL255: if(_tmp40D <= (void*)4)goto _LL257;if(*((int*)
_tmp40D)!= 14)goto _LL257;_tmp40E=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp40D)->f1;_LL256: rgn=_tmp40E;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto
_LL254;_LL257:;_LL258:{const char*_tmp7F6;void*_tmp7F5[1];struct Cyc_String_pa_struct
_tmp7F4;(_tmp7F4.tag=0,((_tmp7F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp7F5[0]=& _tmp7F4,Cyc_Tcutil_terr(
rgn_handle->loc,((_tmp7F6="expecting region_t type but found %s",_tag_dyneither(
_tmp7F6,sizeof(char),37))),_tag_dyneither(_tmp7F5,sizeof(void*),1)))))));}goto
_LL254;_LL254:;}else{if(topt != 0){void*optrgn=(void*)0;if(Cyc_Tcutil_rgn_of_pointer(*
topt,& optrgn))rgn=Cyc_Tcexp_mallocRgn(optrgn);}}{void*_tmp414=(void*)e1->r;
struct Cyc_Core_Opt*_tmp415;struct Cyc_List_List*_tmp416;struct Cyc_List_List*
_tmp417;union Cyc_Absyn_Cnst_union _tmp418;struct _dyneither_ptr _tmp419;_LL25A: if(*((
int*)_tmp414)!= 29)goto _LL25C;_LL25B: {void*_tmp41A=Cyc_Tcexp_tcExpNoPromote(te,
topt,e1);void*_tmp41B=Cyc_Tcutil_compress(_tmp41A);struct Cyc_Absyn_ArrayInfo
_tmp41C;void*_tmp41D;struct Cyc_Absyn_Tqual _tmp41E;struct Cyc_Absyn_Exp*_tmp41F;
struct Cyc_Absyn_Conref*_tmp420;_LL265: if(_tmp41B <= (void*)4)goto _LL267;if(*((int*)
_tmp41B)!= 7)goto _LL267;_tmp41C=((struct Cyc_Absyn_ArrayType_struct*)_tmp41B)->f1;
_tmp41D=(void*)_tmp41C.elt_type;_tmp41E=_tmp41C.tq;_tmp41F=_tmp41C.num_elts;
_tmp420=_tmp41C.zero_term;_LL266: {struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)
_check_null(_tmp41F);void*b;{void*_tmp421=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bnd->topt))->v);void*_tmp422;_LL26A: if(_tmp421 <= (void*)4)goto _LL26C;
if(*((int*)_tmp421)!= 18)goto _LL26C;_tmp422=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp421)->f1;_LL26B:{struct Cyc_Absyn_Upper_b_struct _tmp7F9;struct Cyc_Absyn_Upper_b_struct*
_tmp7F8;b=(void*)((_tmp7F8=_cycalloc(sizeof(*_tmp7F8)),((_tmp7F8[0]=((_tmp7F9.tag=
0,((_tmp7F9.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(
_tmp422,0),0,(void*)1,0),_tmp7F9)))),_tmp7F8))));}goto _LL269;_LL26C:;_LL26D: if(
Cyc_Tcutil_is_const_exp(te,bnd)){struct Cyc_Absyn_Upper_b_struct _tmp7FC;struct Cyc_Absyn_Upper_b_struct*
_tmp7FB;b=(void*)((_tmp7FB=_cycalloc(sizeof(*_tmp7FB)),((_tmp7FB[0]=((_tmp7FC.tag=
0,((_tmp7FC.f1=bnd,_tmp7FC)))),_tmp7FB))));}else{b=(void*)0;}_LL269:;}{struct Cyc_Absyn_PointerType_struct
_tmp806;struct Cyc_Absyn_PtrAtts _tmp805;struct Cyc_Absyn_PtrInfo _tmp804;struct Cyc_Absyn_PointerType_struct*
_tmp803;void*res_typ=(void*)((_tmp803=_cycalloc(sizeof(*_tmp803)),((_tmp803[0]=((
_tmp806.tag=4,((_tmp806.f1=((_tmp804.elt_typ=(void*)_tmp41D,((_tmp804.elt_tq=
_tmp41E,((_tmp804.ptr_atts=((_tmp805.rgn=(void*)rgn,((_tmp805.nullable=((struct
Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((_tmp805.bounds=Cyc_Absyn_new_conref(
b),((_tmp805.zero_term=_tmp420,((_tmp805.ptrloc=0,_tmp805)))))))))),_tmp804)))))),
_tmp806)))),_tmp803))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp807;e->topt=((_tmp807=_cycalloc(
sizeof(*_tmp807)),((_tmp807->v=(void*)res_typ,_tmp807))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}return res_typ;}}_LL267:;_LL268: {const char*
_tmp80A;void*_tmp809;(_tmp809=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp80A="tcNew: comprehension returned non-array type",
_tag_dyneither(_tmp80A,sizeof(char),45))),_tag_dyneither(_tmp809,sizeof(void*),0)));}
_LL264:;}_LL25C: if(*((int*)_tmp414)!= 37)goto _LL25E;_tmp415=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp414)->f1;_tmp416=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp414)->f2;
_LL25D:{struct Cyc_Absyn_Array_e_struct _tmp80D;struct Cyc_Absyn_Array_e_struct*
_tmp80C;(void*)(e1->r=(void*)((void*)((_tmp80C=_cycalloc(sizeof(*_tmp80C)),((
_tmp80C[0]=((_tmp80D.tag=28,((_tmp80D.f1=_tmp416,_tmp80D)))),_tmp80C))))));}
_tmp417=_tmp416;goto _LL25F;_LL25E: if(*((int*)_tmp414)!= 28)goto _LL260;_tmp417=((
struct Cyc_Absyn_Array_e_struct*)_tmp414)->f1;_LL25F: {void**elt_typ_opt=0;int
zero_term=0;if(topt != 0){void*_tmp430=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp431;void*_tmp432;void**_tmp433;struct Cyc_Absyn_Tqual _tmp434;struct Cyc_Absyn_PtrAtts
_tmp435;struct Cyc_Absyn_Conref*_tmp436;_LL26F: if(_tmp430 <= (void*)4)goto _LL271;
if(*((int*)_tmp430)!= 4)goto _LL271;_tmp431=((struct Cyc_Absyn_PointerType_struct*)
_tmp430)->f1;_tmp432=(void*)_tmp431.elt_typ;_tmp433=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp430)->f1).elt_typ;_tmp434=_tmp431.elt_tq;_tmp435=_tmp431.ptr_atts;_tmp436=
_tmp435.zero_term;_LL270: elt_typ_opt=(void**)_tmp433;zero_term=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp436);goto _LL26E;_LL271:;
_LL272: goto _LL26E;_LL26E:;}{void*_tmp437=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp417);{struct Cyc_Core_Opt*_tmp80E;e1->topt=((_tmp80E=
_cycalloc(sizeof(*_tmp80E)),((_tmp80E->v=(void*)_tmp437,_tmp80E))));}{void*
res_typ;{void*_tmp439=Cyc_Tcutil_compress(_tmp437);struct Cyc_Absyn_ArrayInfo
_tmp43A;void*_tmp43B;struct Cyc_Absyn_Tqual _tmp43C;struct Cyc_Absyn_Exp*_tmp43D;
struct Cyc_Absyn_Conref*_tmp43E;_LL274: if(_tmp439 <= (void*)4)goto _LL276;if(*((int*)
_tmp439)!= 7)goto _LL276;_tmp43A=((struct Cyc_Absyn_ArrayType_struct*)_tmp439)->f1;
_tmp43B=(void*)_tmp43A.elt_type;_tmp43C=_tmp43A.tq;_tmp43D=_tmp43A.num_elts;
_tmp43E=_tmp43A.zero_term;_LL275:{struct Cyc_Absyn_PointerType_struct _tmp823;
struct Cyc_Absyn_PtrAtts _tmp822;struct Cyc_Absyn_Upper_b_struct _tmp821;struct Cyc_Absyn_Upper_b_struct*
_tmp820;struct Cyc_Absyn_PtrInfo _tmp81F;struct Cyc_Absyn_PointerType_struct*
_tmp81E;res_typ=(void*)((_tmp81E=_cycalloc(sizeof(*_tmp81E)),((_tmp81E[0]=((
_tmp823.tag=4,((_tmp823.f1=((_tmp81F.elt_typ=(void*)_tmp43B,((_tmp81F.elt_tq=
_tmp43C,((_tmp81F.ptr_atts=((_tmp822.rgn=(void*)rgn,((_tmp822.nullable=((struct
Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((_tmp822.bounds=Cyc_Absyn_new_conref((
void*)((_tmp820=_cycalloc(sizeof(*_tmp820)),((_tmp820[0]=((_tmp821.tag=0,((
_tmp821.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp43D),_tmp821)))),_tmp820))))),((
_tmp822.zero_term=_tmp43E,((_tmp822.ptrloc=0,_tmp822)))))))))),_tmp81F)))))),
_tmp823)))),_tmp81E))));}if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp824;e->topt=((_tmp824=_cycalloc(
sizeof(*_tmp824)),((_tmp824->v=(void*)res_typ,_tmp824))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}goto _LL273;_LL276:;_LL277: {const char*
_tmp827;void*_tmp826;(_tmp826=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp827="tcExpNoPromote on Array_e returned non-array type",
_tag_dyneither(_tmp827,sizeof(char),50))),_tag_dyneither(_tmp826,sizeof(void*),0)));}
_LL273:;}return res_typ;}}}_LL260: if(*((int*)_tmp414)!= 0)goto _LL262;_tmp418=((
struct Cyc_Absyn_Const_e_struct*)_tmp414)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp414)->f1).String_c).tag != 5)goto _LL262;_tmp419=(_tmp418.String_c).f1;_LL261: {
void*_tmp448=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(
void*)0,Cyc_Absyn_true_conref);void*_tmp449=Cyc_Tcexp_tcExp(te,(void**)& _tmp448,
e1);struct Cyc_Absyn_Upper_b_struct _tmp82A;struct Cyc_Absyn_Upper_b_struct*_tmp829;
return Cyc_Absyn_atb_typ(_tmp449,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp829=
_cycalloc(sizeof(*_tmp829)),((_tmp829[0]=((_tmp82A.tag=0,((_tmp82A.f1=Cyc_Absyn_uint_exp(
1,0),_tmp82A)))),_tmp829)))),Cyc_Absyn_false_conref);}_LL262:;_LL263: {void**
topt2=0;if(topt != 0){void*_tmp44C=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp44D;void*_tmp44E;void**_tmp44F;struct Cyc_Absyn_Tqual _tmp450;_LL279: if(
_tmp44C <= (void*)4)goto _LL27D;if(*((int*)_tmp44C)!= 4)goto _LL27B;_tmp44D=((
struct Cyc_Absyn_PointerType_struct*)_tmp44C)->f1;_tmp44E=(void*)_tmp44D.elt_typ;
_tmp44F=(void**)&(((struct Cyc_Absyn_PointerType_struct*)_tmp44C)->f1).elt_typ;
_tmp450=_tmp44D.elt_tq;_LL27A: topt2=(void**)_tmp44F;goto _LL278;_LL27B: if(*((int*)
_tmp44C)!= 2)goto _LL27D;_LL27C:{void**_tmp82B;topt2=((_tmp82B=_cycalloc(sizeof(*
_tmp82B)),((_tmp82B[0]=*topt,_tmp82B))));}goto _LL278;_LL27D:;_LL27E: goto _LL278;
_LL278:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);struct Cyc_Absyn_PointerType_struct
_tmp835;struct Cyc_Absyn_PtrAtts _tmp834;struct Cyc_Absyn_PtrInfo _tmp833;struct Cyc_Absyn_PointerType_struct*
_tmp832;void*res_typ=(void*)((_tmp832=_cycalloc(sizeof(*_tmp832)),((_tmp832[0]=((
_tmp835.tag=4,((_tmp835.f1=((_tmp833.elt_typ=(void*)telt,((_tmp833.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp833.ptr_atts=((_tmp834.rgn=(void*)rgn,((_tmp834.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(),((_tmp834.bounds=Cyc_Absyn_bounds_one_conref,((_tmp834.zero_term=
Cyc_Absyn_false_conref,((_tmp834.ptrloc=0,_tmp834)))))))))),_tmp833)))))),
_tmp835)))),_tmp832))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp836;e->topt=((_tmp836=_cycalloc(
sizeof(*_tmp836)),((_tmp836->v=(void*)res_typ,_tmp836))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}return res_typ;}}_LL259:;}}void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(
Cyc_Tcexp_tcExpNoPromote(te,topt,e));void*_tmp457=t;struct Cyc_Absyn_ArrayInfo
_tmp458;void*_tmp459;struct Cyc_Absyn_Tqual _tmp45A;struct Cyc_Absyn_Exp*_tmp45B;
struct Cyc_Absyn_Conref*_tmp45C;_LL280: if(_tmp457 <= (void*)4)goto _LL282;if(*((int*)
_tmp457)!= 7)goto _LL282;_tmp458=((struct Cyc_Absyn_ArrayType_struct*)_tmp457)->f1;
_tmp459=(void*)_tmp458.elt_type;_tmp45A=_tmp458.tq;_tmp45B=_tmp458.num_elts;
_tmp45C=_tmp458.zero_term;_LL281: {void*_tmp45E;struct _tuple6 _tmp45D=Cyc_Tcutil_addressof_props(
te,e);_tmp45E=_tmp45D.f2;{struct Cyc_Absyn_Upper_b_struct _tmp839;struct Cyc_Absyn_Upper_b_struct*
_tmp838;void*_tmp45F=_tmp45B == 0?(void*)((void*)0):(void*)((_tmp838=_cycalloc(
sizeof(*_tmp838)),((_tmp838[0]=((_tmp839.tag=0,((_tmp839.f1=(struct Cyc_Absyn_Exp*)
_tmp45B,_tmp839)))),_tmp838))));t=Cyc_Absyn_atb_typ(_tmp459,_tmp45E,_tmp45A,
_tmp45F,_tmp45C);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);
return t;}}_LL282:;_LL283: return t;_LL27F:;}void*Cyc_Tcexp_tcExpInitializer(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(
te,topt,e);struct _RegionHandle*_tmp462=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp462,t) && !Cyc_Tcutil_is_noalias_path(_tmp462,e)){{const char*_tmp841;const
char*_tmp840;void*_tmp83F[3];struct Cyc_String_pa_struct _tmp83E;struct Cyc_String_pa_struct
_tmp83D;struct Cyc_String_pa_struct _tmp83C;(_tmp83C.tag=0,((_tmp83C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp83D.tag=0,((
_tmp83D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp83E.tag=0,((_tmp83E.f1=(
struct _dyneither_ptr)(topt == 0?(_tmp841="NULL",_tag_dyneither(_tmp841,sizeof(
char),5)):(struct _dyneither_ptr)Cyc_Absynpp_typ2string(*topt)),((_tmp83F[0]=&
_tmp83E,((_tmp83F[1]=& _tmp83D,((_tmp83F[2]=& _tmp83C,Cyc_fprintf(Cyc_stderr,((
_tmp840="topt=%s, e->topt->v=%s, e=%s\n",_tag_dyneither(_tmp840,sizeof(char),30))),
_tag_dyneither(_tmp83F,sizeof(void*),3)))))))))))))))))));}{const char*_tmp844;
void*_tmp843;(_tmp843=0,Cyc_Tcutil_terr(e->loc,((_tmp844="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp844,sizeof(char),49))),_tag_dyneither(_tmp843,sizeof(void*),0)));}}{
void*_tmp46B=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp46C;_LL285: if(*((int*)
_tmp46B)!= 28)goto _LL287;_LL286: goto _LL288;_LL287: if(*((int*)_tmp46B)!= 29)goto
_LL289;_LL288: goto _LL28A;_LL289: if(*((int*)_tmp46B)!= 0)goto _LL28B;_tmp46C=((
struct Cyc_Absyn_Const_e_struct*)_tmp46B)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp46B)->f1).String_c).tag != 5)goto _LL28B;_LL28A: return t;_LL28B:;_LL28C: t=Cyc_Tcutil_compress(
t);{void*_tmp46D=t;struct Cyc_Absyn_ArrayInfo _tmp46E;void*_tmp46F;struct Cyc_Absyn_Tqual
_tmp470;struct Cyc_Absyn_Exp*_tmp471;struct Cyc_Absyn_Conref*_tmp472;_LL28E: if(
_tmp46D <= (void*)4)goto _LL290;if(*((int*)_tmp46D)!= 7)goto _LL290;_tmp46E=((
struct Cyc_Absyn_ArrayType_struct*)_tmp46D)->f1;_tmp46F=(void*)_tmp46E.elt_type;
_tmp470=_tmp46E.tq;_tmp471=_tmp46E.num_elts;_tmp472=_tmp46E.zero_term;_LL28F: {
void*_tmp474;struct _tuple6 _tmp473=Cyc_Tcutil_addressof_props(te,e);_tmp474=
_tmp473.f2;{struct Cyc_Absyn_Upper_b_struct _tmp847;struct Cyc_Absyn_Upper_b_struct*
_tmp846;void*b=_tmp471 == 0?(void*)((void*)0):(void*)((_tmp846=_cycalloc(sizeof(*
_tmp846)),((_tmp846[0]=((_tmp847.tag=0,((_tmp847.f1=(struct Cyc_Absyn_Exp*)
_tmp471,_tmp847)))),_tmp846))));t=Cyc_Absyn_atb_typ(_tmp46F,_tmp474,_tmp470,b,
_tmp472);Cyc_Tcutil_unchecked_cast(te,e,t,(void*)3);return t;}}_LL290:;_LL291:
return t;_LL28D:;}_LL284:;}}static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp477=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp478;_LL293: if(*((int*)_tmp477)!= 13)goto _LL295;_tmp478=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp477)->f1;_LL294: Cyc_Tcexp_tcExpNoInst(
te,topt,_tmp478);(void*)(((struct Cyc_Core_Opt*)_check_null(_tmp478->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(_tmp478->topt))->v,
0));e->topt=_tmp478->topt;goto _LL292;_LL295:;_LL296: Cyc_Tcexp_tcExpNoInst(te,
topt,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0));{void*_tmp479=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp47A;void*_tmp47B;struct Cyc_Absyn_Tqual _tmp47C;struct Cyc_Absyn_PtrAtts _tmp47D;
void*_tmp47E;struct Cyc_Absyn_Conref*_tmp47F;struct Cyc_Absyn_Conref*_tmp480;
struct Cyc_Absyn_Conref*_tmp481;_LL298: if(_tmp479 <= (void*)4)goto _LL29A;if(*((int*)
_tmp479)!= 4)goto _LL29A;_tmp47A=((struct Cyc_Absyn_PointerType_struct*)_tmp479)->f1;
_tmp47B=(void*)_tmp47A.elt_typ;_tmp47C=_tmp47A.elt_tq;_tmp47D=_tmp47A.ptr_atts;
_tmp47E=(void*)_tmp47D.rgn;_tmp47F=_tmp47D.nullable;_tmp480=_tmp47D.bounds;
_tmp481=_tmp47D.zero_term;_LL299:{void*_tmp482=Cyc_Tcutil_compress(_tmp47B);
struct Cyc_Absyn_FnInfo _tmp483;struct Cyc_List_List*_tmp484;struct Cyc_Core_Opt*
_tmp485;void*_tmp486;struct Cyc_List_List*_tmp487;int _tmp488;struct Cyc_Absyn_VarargInfo*
_tmp489;struct Cyc_List_List*_tmp48A;struct Cyc_List_List*_tmp48B;_LL29D: if(
_tmp482 <= (void*)4)goto _LL29F;if(*((int*)_tmp482)!= 8)goto _LL29F;_tmp483=((
struct Cyc_Absyn_FnType_struct*)_tmp482)->f1;_tmp484=_tmp483.tvars;_tmp485=
_tmp483.effect;_tmp486=(void*)_tmp483.ret_typ;_tmp487=_tmp483.args;_tmp488=
_tmp483.c_varargs;_tmp489=_tmp483.cyc_varargs;_tmp48A=_tmp483.rgn_po;_tmp48B=
_tmp483.attributes;_LL29E: if(_tmp484 != 0){struct _RegionHandle*_tmp48C=Cyc_Tcenv_get_fnrgn(
te);struct _tuple4 _tmp848;struct _tuple4 _tmp48D=(_tmp848.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp848.f2=_tmp48C,_tmp848)));struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp48C,Cyc_Tcutil_r_make_inst_var,&
_tmp48D,_tmp484);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);struct Cyc_Absyn_FnType_struct _tmp84E;struct Cyc_Absyn_FnInfo
_tmp84D;struct Cyc_Absyn_FnType_struct*_tmp84C;void*ftyp=Cyc_Tcutil_rsubstitute(
_tmp48C,inst,(void*)((_tmp84C=_cycalloc(sizeof(*_tmp84C)),((_tmp84C[0]=((_tmp84E.tag=
8,((_tmp84E.f1=((_tmp84D.tvars=0,((_tmp84D.effect=_tmp485,((_tmp84D.ret_typ=(
void*)_tmp486,((_tmp84D.args=_tmp487,((_tmp84D.c_varargs=_tmp488,((_tmp84D.cyc_varargs=
_tmp489,((_tmp84D.rgn_po=_tmp48A,((_tmp84D.attributes=_tmp48B,_tmp84D)))))))))))))))),
_tmp84E)))),_tmp84C)))));struct Cyc_Absyn_PointerType_struct _tmp858;struct Cyc_Absyn_PtrAtts
_tmp857;struct Cyc_Absyn_PtrInfo _tmp856;struct Cyc_Absyn_PointerType_struct*
_tmp855;struct Cyc_Absyn_PointerType_struct*_tmp48E=(_tmp855=_cycalloc(sizeof(*
_tmp855)),((_tmp855[0]=((_tmp858.tag=4,((_tmp858.f1=((_tmp856.elt_typ=(void*)
ftyp,((_tmp856.elt_tq=_tmp47C,((_tmp856.ptr_atts=((_tmp857.rgn=(void*)_tmp47E,((
_tmp857.nullable=_tmp47F,((_tmp857.bounds=_tmp480,((_tmp857.zero_term=_tmp481,((
_tmp857.ptrloc=0,_tmp857)))))))))),_tmp856)))))),_tmp858)))),_tmp855)));struct
Cyc_Absyn_Exp*_tmp48F=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Instantiate_e_struct
_tmp85B;struct Cyc_Absyn_Instantiate_e_struct*_tmp85A;(void*)(e->r=(void*)((void*)((
_tmp85A=_cycalloc(sizeof(*_tmp85A)),((_tmp85A[0]=((_tmp85B.tag=14,((_tmp85B.f1=
_tmp48F,((_tmp85B.f2=ts,_tmp85B)))))),_tmp85A))))));}{struct Cyc_Core_Opt*_tmp85C;
e->topt=((_tmp85C=_cycalloc(sizeof(*_tmp85C)),((_tmp85C->v=(void*)((void*)
_tmp48E),_tmp85C))));}}goto _LL29C;_LL29F:;_LL2A0: goto _LL29C;_LL29C:;}goto _LL297;
_LL29A:;_LL29B: goto _LL297;_LL297:;}goto _LL292;_LL292:;}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*loc=e->loc;void*
t;{void*_tmp49B=(void*)e->r;struct Cyc_Absyn_Exp*_tmp49C;struct _tuple1*_tmp49D;
struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_List_List*_tmp49F;struct Cyc_Core_Opt*
_tmp4A0;struct Cyc_List_List*_tmp4A1;union Cyc_Absyn_Cnst_union _tmp4A2;union Cyc_Absyn_Cnst_union*
_tmp4A3;struct _tuple1*_tmp4A4;void*_tmp4A5;void*_tmp4A6;struct Cyc_List_List*
_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A8;void*_tmp4A9;struct Cyc_Absyn_Exp*_tmp4AA;
struct Cyc_Core_Opt*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_Absyn_Exp*
_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_Absyn_Exp*
_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Exp*
_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_Absyn_Exp*
_tmp4B6;struct Cyc_List_List*_tmp4B7;struct Cyc_Absyn_VarargCallInfo*_tmp4B8;
struct Cyc_Absyn_VarargCallInfo**_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BA;struct Cyc_Absyn_Exp*
_tmp4BB;struct Cyc_List_List*_tmp4BC;void*_tmp4BD;struct Cyc_Absyn_Exp*_tmp4BE;
void*_tmp4BF;void**_tmp4C0;struct Cyc_Absyn_Exp*_tmp4C1;struct Cyc_Absyn_Exp*
_tmp4C2;struct Cyc_Absyn_Exp*_tmp4C3;struct Cyc_Absyn_Exp*_tmp4C4;void*_tmp4C5;
void*_tmp4C6;void*_tmp4C7;struct Cyc_Absyn_Exp*_tmp4C8;struct Cyc_Absyn_Exp*
_tmp4C9;struct _dyneither_ptr*_tmp4CA;struct Cyc_Absyn_Exp*_tmp4CB;struct
_dyneither_ptr*_tmp4CC;struct Cyc_Absyn_Exp*_tmp4CD;struct Cyc_Absyn_Exp*_tmp4CE;
struct Cyc_List_List*_tmp4CF;struct _tuple2*_tmp4D0;struct Cyc_List_List*_tmp4D1;
struct Cyc_List_List*_tmp4D2;struct Cyc_Absyn_Stmt*_tmp4D3;struct Cyc_Absyn_Vardecl*
_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D6;int _tmp4D7;int*
_tmp4D8;struct _tuple1*_tmp4D9;struct _tuple1**_tmp4DA;struct Cyc_List_List*_tmp4DB;
struct Cyc_List_List**_tmp4DC;struct Cyc_List_List*_tmp4DD;struct Cyc_Absyn_Aggrdecl*
_tmp4DE;struct Cyc_Absyn_Aggrdecl**_tmp4DF;void*_tmp4E0;struct Cyc_List_List*
_tmp4E1;struct Cyc_List_List*_tmp4E2;struct Cyc_Absyn_Tuniondecl*_tmp4E3;struct Cyc_Absyn_Tunionfield*
_tmp4E4;struct _tuple1*_tmp4E5;struct _tuple1**_tmp4E6;struct Cyc_Absyn_Enumdecl*
_tmp4E7;struct Cyc_Absyn_Enumfield*_tmp4E8;struct _tuple1*_tmp4E9;struct _tuple1**
_tmp4EA;void*_tmp4EB;struct Cyc_Absyn_Enumfield*_tmp4EC;struct Cyc_Absyn_MallocInfo
_tmp4ED;int _tmp4EE;int*_tmp4EF;struct Cyc_Absyn_Exp*_tmp4F0;void**_tmp4F1;void***
_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F3;struct Cyc_Absyn_Exp**_tmp4F4;int _tmp4F5;int*
_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F8;void*_tmp4F9;
_LL2A2: if(*((int*)_tmp49B)!= 13)goto _LL2A4;_tmp49C=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp49B)->f1;_LL2A3: Cyc_Tcexp_tcExpNoInst(te,0,_tmp49C);return;_LL2A4: if(*((int*)
_tmp49B)!= 2)goto _LL2A6;_tmp49D=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp49B)->f1;
_LL2A5: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp49D);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL2A6: if(*((int*)_tmp49B)!= 10)goto _LL2A8;_tmp49E=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp49B)->f1;_tmp49F=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp49B)->f2;_LL2A7:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp49E,_tmp49F);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL2A8: if(*((int*)_tmp49B)!= 37)goto _LL2AA;_tmp4A0=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp49B)->f1;_tmp4A1=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp49B)->f2;
_LL2A9: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp4A1);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL2AA: if(*((int*)_tmp49B)!= 0)goto _LL2AC;_tmp4A2=((struct Cyc_Absyn_Const_e_struct*)
_tmp49B)->f1;_tmp4A3=(union Cyc_Absyn_Cnst_union*)&((struct Cyc_Absyn_Const_e_struct*)
_tmp49B)->f1;_LL2AB: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst_union*)
_tmp4A3,e);goto _LL2A1;_LL2AC: if(*((int*)_tmp49B)!= 1)goto _LL2AE;_tmp4A4=((struct
Cyc_Absyn_Var_e_struct*)_tmp49B)->f1;_tmp4A5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp49B)->f2;_LL2AD: t=Cyc_Tcexp_tcVar(te,loc,_tmp4A4,_tmp4A5);goto _LL2A1;_LL2AE:
if(*((int*)_tmp49B)!= 3)goto _LL2B0;_tmp4A6=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp49B)->f1;_tmp4A7=((struct Cyc_Absyn_Primop_e_struct*)_tmp49B)->f2;_LL2AF: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp4A6,_tmp4A7);goto _LL2A1;_LL2B0: if(*((int*)
_tmp49B)!= 5)goto _LL2B2;_tmp4A8=((struct Cyc_Absyn_Increment_e_struct*)_tmp49B)->f1;
_tmp4A9=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp49B)->f2;_LL2B1: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp4A8,_tmp4A9);goto _LL2A1;_LL2B2: if(*((int*)_tmp49B)!= 4)goto
_LL2B4;_tmp4AA=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp49B)->f1;_tmp4AB=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp49B)->f2;_tmp4AC=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp49B)->f3;_LL2B3: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp4AA,_tmp4AB,_tmp4AC);
goto _LL2A1;_LL2B4: if(*((int*)_tmp49B)!= 6)goto _LL2B6;_tmp4AD=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp49B)->f1;_tmp4AE=((struct Cyc_Absyn_Conditional_e_struct*)_tmp49B)->f2;
_tmp4AF=((struct Cyc_Absyn_Conditional_e_struct*)_tmp49B)->f3;_LL2B5: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp4AD,_tmp4AE,_tmp4AF);goto _LL2A1;_LL2B6: if(*((int*)_tmp49B)!= 7)
goto _LL2B8;_tmp4B0=((struct Cyc_Absyn_And_e_struct*)_tmp49B)->f1;_tmp4B1=((struct
Cyc_Absyn_And_e_struct*)_tmp49B)->f2;_LL2B7: t=Cyc_Tcexp_tcAnd(te,loc,_tmp4B0,
_tmp4B1);goto _LL2A1;_LL2B8: if(*((int*)_tmp49B)!= 8)goto _LL2BA;_tmp4B2=((struct
Cyc_Absyn_Or_e_struct*)_tmp49B)->f1;_tmp4B3=((struct Cyc_Absyn_Or_e_struct*)
_tmp49B)->f2;_LL2B9: t=Cyc_Tcexp_tcOr(te,loc,_tmp4B2,_tmp4B3);goto _LL2A1;_LL2BA:
if(*((int*)_tmp49B)!= 9)goto _LL2BC;_tmp4B4=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp49B)->f1;_tmp4B5=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp49B)->f2;_LL2BB: t=
Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp4B4,_tmp4B5);goto _LL2A1;_LL2BC: if(*((int*)
_tmp49B)!= 11)goto _LL2BE;_tmp4B6=((struct Cyc_Absyn_FnCall_e_struct*)_tmp49B)->f1;
_tmp4B7=((struct Cyc_Absyn_FnCall_e_struct*)_tmp49B)->f2;_tmp4B8=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp49B)->f3;_tmp4B9=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp49B)->f3;_LL2BD: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp4B6,_tmp4B7,_tmp4B9);
goto _LL2A1;_LL2BE: if(*((int*)_tmp49B)!= 12)goto _LL2C0;_tmp4BA=((struct Cyc_Absyn_Throw_e_struct*)
_tmp49B)->f1;_LL2BF: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp4BA);goto _LL2A1;_LL2C0:
if(*((int*)_tmp49B)!= 14)goto _LL2C2;_tmp4BB=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp49B)->f1;_tmp4BC=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp49B)->f2;_LL2C1:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp4BB,_tmp4BC);goto _LL2A1;_LL2C2: if(*((
int*)_tmp49B)!= 15)goto _LL2C4;_tmp4BD=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp49B)->f1;_tmp4BE=((struct Cyc_Absyn_Cast_e_struct*)_tmp49B)->f2;_tmp4BF=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp49B)->f4;_tmp4C0=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp49B)->f4);_LL2C3: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4BD,_tmp4BE,(void**)
_tmp4C0);goto _LL2A1;_LL2C4: if(*((int*)_tmp49B)!= 16)goto _LL2C6;_tmp4C1=((struct
Cyc_Absyn_Address_e_struct*)_tmp49B)->f1;_LL2C5: t=Cyc_Tcexp_tcAddress(te,loc,e,
topt,_tmp4C1);goto _LL2A1;_LL2C6: if(*((int*)_tmp49B)!= 17)goto _LL2C8;_tmp4C2=((
struct Cyc_Absyn_New_e_struct*)_tmp49B)->f1;_tmp4C3=((struct Cyc_Absyn_New_e_struct*)
_tmp49B)->f2;_LL2C7: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4C2,e,_tmp4C3);goto _LL2A1;
_LL2C8: if(*((int*)_tmp49B)!= 19)goto _LL2CA;_tmp4C4=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp49B)->f1;_LL2C9: {void*_tmp4FA=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4C4);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4FA);goto _LL2A1;}_LL2CA: if(*((int*)_tmp49B)!= 18)goto _LL2CC;
_tmp4C5=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp49B)->f1;_LL2CB: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4C5);goto _LL2A1;_LL2CC: if(*((int*)_tmp49B)!= 20)goto _LL2CE;
_tmp4C6=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp49B)->f1;_tmp4C7=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp49B)->f2;_LL2CD: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp4C6,_tmp4C7);goto _LL2A1;_LL2CE: if(*((int*)_tmp49B)!= 21)goto _LL2D0;
_LL2CF:{const char*_tmp85F;void*_tmp85E;(_tmp85E=0,Cyc_Tcutil_terr(loc,((_tmp85F="gen() not in top-level initializer",
_tag_dyneither(_tmp85F,sizeof(char),35))),_tag_dyneither(_tmp85E,sizeof(void*),0)));}
return;_LL2D0: if(*((int*)_tmp49B)!= 22)goto _LL2D2;_tmp4C8=((struct Cyc_Absyn_Deref_e_struct*)
_tmp49B)->f1;_LL2D1: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4C8);goto _LL2A1;_LL2D2:
if(*((int*)_tmp49B)!= 23)goto _LL2D4;_tmp4C9=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp49B)->f1;_tmp4CA=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp49B)->f2;_LL2D3:
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp4C9,_tmp4CA);goto _LL2A1;_LL2D4: if(*((
int*)_tmp49B)!= 24)goto _LL2D6;_tmp4CB=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp49B)->f1;_tmp4CC=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp49B)->f2;_LL2D5: t=
Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp4CB,_tmp4CC);goto _LL2A1;_LL2D6: if(*((int*)
_tmp49B)!= 25)goto _LL2D8;_tmp4CD=((struct Cyc_Absyn_Subscript_e_struct*)_tmp49B)->f1;
_tmp4CE=((struct Cyc_Absyn_Subscript_e_struct*)_tmp49B)->f2;_LL2D7: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp4CD,_tmp4CE);goto _LL2A1;_LL2D8: if(*((int*)_tmp49B)!= 26)goto
_LL2DA;_tmp4CF=((struct Cyc_Absyn_Tuple_e_struct*)_tmp49B)->f1;_LL2D9: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp4CF);goto _LL2A1;_LL2DA: if(*((int*)_tmp49B)!= 27)goto _LL2DC;
_tmp4D0=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp49B)->f1;_tmp4D1=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp49B)->f2;_LL2DB: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4D0,_tmp4D1);goto
_LL2A1;_LL2DC: if(*((int*)_tmp49B)!= 28)goto _LL2DE;_tmp4D2=((struct Cyc_Absyn_Array_e_struct*)
_tmp49B)->f1;_LL2DD: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp4FD=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp4FE;void*_tmp4FF;void**
_tmp500;struct Cyc_Absyn_Conref*_tmp501;_LL2F3: if(_tmp4FD <= (void*)4)goto _LL2F5;
if(*((int*)_tmp4FD)!= 7)goto _LL2F5;_tmp4FE=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4FD)->f1;_tmp4FF=(void*)_tmp4FE.elt_type;_tmp500=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp4FD)->f1).elt_type;_tmp501=_tmp4FE.zero_term;_LL2F4: elt_topt=(void**)_tmp500;
zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp501);
goto _LL2F2;_LL2F5:;_LL2F6: goto _LL2F2;_LL2F2:;}t=Cyc_Tcexp_tcArray(te,loc,
elt_topt,zero_term,_tmp4D2);goto _LL2A1;}_LL2DE: if(*((int*)_tmp49B)!= 38)goto
_LL2E0;_tmp4D3=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp49B)->f1;_LL2DF: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp4D3);goto _LL2A1;_LL2E0: if(*((int*)_tmp49B)!= 29)goto _LL2E2;
_tmp4D4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp49B)->f1;_tmp4D5=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp49B)->f2;_tmp4D6=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp49B)->f3;_tmp4D7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp49B)->f4;
_tmp4D8=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp49B)->f4;_LL2E1: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4D4,_tmp4D5,_tmp4D6,_tmp4D8);goto
_LL2A1;_LL2E2: if(*((int*)_tmp49B)!= 30)goto _LL2E4;_tmp4D9=((struct Cyc_Absyn_Struct_e_struct*)
_tmp49B)->f1;_tmp4DA=(struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp49B)->f1;_tmp4DB=((struct Cyc_Absyn_Struct_e_struct*)_tmp49B)->f2;_tmp4DC=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp49B)->f2;_tmp4DD=((
struct Cyc_Absyn_Struct_e_struct*)_tmp49B)->f3;_tmp4DE=((struct Cyc_Absyn_Struct_e_struct*)
_tmp49B)->f4;_tmp4DF=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp49B)->f4;_LL2E3: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp4DA,_tmp4DC,_tmp4DD,
_tmp4DF);goto _LL2A1;_LL2E4: if(*((int*)_tmp49B)!= 31)goto _LL2E6;_tmp4E0=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp49B)->f1;_tmp4E1=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp49B)->f2;_LL2E5: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4E0,_tmp4E1);goto _LL2A1;
_LL2E6: if(*((int*)_tmp49B)!= 32)goto _LL2E8;_tmp4E2=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp49B)->f1;_tmp4E3=((struct Cyc_Absyn_Tunion_e_struct*)_tmp49B)->f2;_tmp4E4=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp49B)->f3;_LL2E7: t=Cyc_Tcexp_tcTunion(te,loc,
topt,e,_tmp4E2,_tmp4E3,_tmp4E4);goto _LL2A1;_LL2E8: if(*((int*)_tmp49B)!= 33)goto
_LL2EA;_tmp4E5=((struct Cyc_Absyn_Enum_e_struct*)_tmp49B)->f1;_tmp4E6=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp49B)->f1;_tmp4E7=((struct Cyc_Absyn_Enum_e_struct*)
_tmp49B)->f2;_tmp4E8=((struct Cyc_Absyn_Enum_e_struct*)_tmp49B)->f3;_LL2E9:*
_tmp4E6=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp4E8))->name;{struct Cyc_Absyn_EnumType_struct
_tmp862;struct Cyc_Absyn_EnumType_struct*_tmp861;t=(void*)((_tmp861=_cycalloc(
sizeof(*_tmp861)),((_tmp861[0]=((_tmp862.tag=12,((_tmp862.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp4E7))->name,((_tmp862.f2=_tmp4E7,_tmp862)))))),_tmp861))));}goto
_LL2A1;_LL2EA: if(*((int*)_tmp49B)!= 34)goto _LL2EC;_tmp4E9=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp49B)->f1;_tmp4EA=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp49B)->f1;_tmp4EB=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp49B)->f2;
_tmp4EC=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp49B)->f3;_LL2EB:*_tmp4EA=((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp4EC))->name;t=_tmp4EB;goto _LL2A1;
_LL2EC: if(*((int*)_tmp49B)!= 35)goto _LL2EE;_tmp4ED=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp49B)->f1;_tmp4EE=_tmp4ED.is_calloc;_tmp4EF=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp49B)->f1).is_calloc;_tmp4F0=_tmp4ED.rgn;_tmp4F1=_tmp4ED.elt_type;_tmp4F2=(
void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp49B)->f1).elt_type;_tmp4F3=
_tmp4ED.num_elts;_tmp4F4=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp49B)->f1).num_elts;_tmp4F5=_tmp4ED.fat_result;_tmp4F6=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp49B)->f1).fat_result;_LL2ED: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4F0,_tmp4F2,
_tmp4F4,_tmp4EF,_tmp4F6);goto _LL2A1;_LL2EE: if(*((int*)_tmp49B)!= 36)goto _LL2F0;
_tmp4F7=((struct Cyc_Absyn_Swap_e_struct*)_tmp49B)->f1;_tmp4F8=((struct Cyc_Absyn_Swap_e_struct*)
_tmp49B)->f2;_LL2EF: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp4F7,_tmp4F8);goto _LL2A1;
_LL2F0: if(*((int*)_tmp49B)!= 39)goto _LL2A1;_tmp4F9=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp49B)->f1;_LL2F1: if(!te->allow_valueof){const char*_tmp865;void*_tmp864;(
_tmp864=0,Cyc_Tcutil_terr(e->loc,((_tmp865="valueof(-) can only occur within types",
_tag_dyneither(_tmp865,sizeof(char),39))),_tag_dyneither(_tmp864,sizeof(void*),0)));}
t=Cyc_Absyn_sint_typ;goto _LL2A1;_LL2A1:;}{struct Cyc_Core_Opt*_tmp866;e->topt=((
_tmp866=_cycalloc(sizeof(*_tmp866)),((_tmp866->v=(void*)t,_tmp866))));}}
