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
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(
struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rrev(struct
_RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_List_List*
Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rtabulate_c(struct
_RegionHandle*r,int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(
struct _RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(
struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(
struct Cyc_Set_Set*s);int Cyc_Set_is_empty(struct Cyc_Set_Set*s);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];void*Cyc_Set_choose(
struct Cyc_Set_Set*s);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct
_dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];struct
_union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
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
Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);
struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;
struct Cyc_List_List*targs;};struct _tuple3{void*f1;struct _tuple1*f2;struct Cyc_Core_Opt*
f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);
struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Position_Segment*
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);extern union Cyc_Absyn_Constraint*
Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_empty_effect;struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(void*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
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
f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;
struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct
Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;int allow_valueof;};struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*rgn);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);int Cyc_Tcutil_kind_leq(void*k1,void*k2);
void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerceable(void*);extern struct Cyc_Core_Opt
Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_trk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;
extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple10{struct Cyc_List_List*
f1;struct _RegionHandle*f2;};struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct _tuple11*Cyc_Tcutil_r_make_inst_var(struct _tuple10*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr err_msg);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
void*,struct Cyc_List_List*fields);int Cyc_Tcutil_is_noalias_pointer(void*t);int
Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar(void*k);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct
Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};struct Cyc_Tcexp_TestEnv
Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr msg_part);struct _tuple12{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple12*tvars_and_bounds_opt;struct Cyc_List_List*
patvars;};struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt);void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct
_tuple13{unsigned int f1;int f2;};struct _tuple13 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct
Cyc_Absyn_Pat*p);static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Pat*p){void*_tmp0=p->r;struct _tuple1*_tmp1;struct _tuple1*
_tmp2;struct Cyc_List_List*_tmp3;int _tmp4;struct Cyc_Absyn_AggrInfo*_tmp5;struct
Cyc_List_List*_tmp6;struct Cyc_List_List*_tmp7;int _tmp8;struct Cyc_Absyn_AggrInfo*
_tmp9;struct Cyc_Absyn_AggrInfo _tmpA;union Cyc_Absyn_AggrInfoU _tmpB;struct _tuple3
_tmpC;void*_tmpD;struct _tuple1*_tmpE;struct Cyc_List_List*_tmpF;struct Cyc_List_List*
_tmp10;struct Cyc_List_List**_tmp11;struct Cyc_List_List*_tmp12;int _tmp13;struct
Cyc_Absyn_Exp*_tmp14;_LL1: if(_tmp0 <= (void*)2)goto _LLB;if(*((int*)_tmp0)!= 12)
goto _LL3;_tmp1=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp0)->f1;_LL2:{struct
_handler_cons _tmp15;_push_handler(& _tmp15);{int _tmp17=0;if(setjmp(_tmp15.handler))
_tmp17=1;if(!_tmp17){{struct _RegionHandle*_tmp18=Cyc_Tcenv_get_fnrgn(te);void*
_tmp19=Cyc_Tcenv_lookup_ordinary(_tmp18,te,p->loc,_tmp1);struct Cyc_Absyn_Datatypedecl*
_tmp1A;struct Cyc_Absyn_Datatypefield*_tmp1B;struct Cyc_Absyn_Enumdecl*_tmp1C;
struct Cyc_Absyn_Enumfield*_tmp1D;void*_tmp1E;struct Cyc_Absyn_Enumfield*_tmp1F;
_LLE: if(*((int*)_tmp19)!= 1)goto _LL10;_LLF: p->r=(void*)0;{const char*_tmp33D;void*
_tmp33C;(_tmp33C=0,Cyc_Tcutil_terr(p->loc,((_tmp33D="struct tag used without arguments in pattern",
_tag_dyneither(_tmp33D,sizeof(char),45))),_tag_dyneither(_tmp33C,sizeof(void*),0)));}
_npop_handler(0);return;_LL10: if(*((int*)_tmp19)!= 2)goto _LL12;_tmp1A=((struct
Cyc_Tcenv_DatatypeRes_struct*)_tmp19)->f1;_tmp1B=((struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp19)->f2;_LL11:{struct Cyc_Absyn_Datatype_p_struct _tmp340;struct Cyc_Absyn_Datatype_p_struct*
_tmp33F;p->r=(void*)((_tmp33F=_cycalloc(sizeof(*_tmp33F)),((_tmp33F[0]=((_tmp340.tag=
6,((_tmp340.f1=_tmp1A,((_tmp340.f2=_tmp1B,((_tmp340.f3=0,((_tmp340.f4=0,_tmp340)))))))))),
_tmp33F))));}_npop_handler(0);return;_LL12: if(*((int*)_tmp19)!= 3)goto _LL14;
_tmp1C=((struct Cyc_Tcenv_EnumRes_struct*)_tmp19)->f1;_tmp1D=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp19)->f2;_LL13:{struct Cyc_Absyn_Enum_p_struct _tmp343;struct Cyc_Absyn_Enum_p_struct*
_tmp342;p->r=(void*)((_tmp342=_cycalloc(sizeof(*_tmp342)),((_tmp342[0]=((_tmp343.tag=
10,((_tmp343.f1=_tmp1C,((_tmp343.f2=_tmp1D,_tmp343)))))),_tmp342))));}
_npop_handler(0);return;_LL14: if(*((int*)_tmp19)!= 4)goto _LL16;_tmp1E=(void*)((
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp19)->f1;_tmp1F=((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp19)->f2;_LL15:{struct Cyc_Absyn_AnonEnum_p_struct _tmp346;struct Cyc_Absyn_AnonEnum_p_struct*
_tmp345;p->r=(void*)((_tmp345=_cycalloc(sizeof(*_tmp345)),((_tmp345[0]=((_tmp346.tag=
11,((_tmp346.f1=(void*)_tmp1E,((_tmp346.f2=_tmp1F,_tmp346)))))),_tmp345))));}
_npop_handler(0);return;_LL16: if(*((int*)_tmp19)!= 0)goto _LLD;_LL17: goto _LLD;
_LLD:;};_pop_handler();}else{void*_tmp16=(void*)_exn_thrown;void*_tmp29=_tmp16;
_LL19: if(_tmp29 != Cyc_Dict_Absent)goto _LL1B;_LL1A: goto _LL18;_LL1B:;_LL1C:(void)
_throw(_tmp29);_LL18:;}}}{union Cyc_Absyn_Nmspace _tmp2A=(*_tmp1).f1;int _tmp2B;
struct Cyc_List_List*_tmp2C;_LL1E: if((_tmp2A.Loc_n).tag != 3)goto _LL20;_tmp2B=(int)(
_tmp2A.Loc_n).val;_LL1F: goto _LL21;_LL20: if((_tmp2A.Rel_n).tag != 1)goto _LL22;
_tmp2C=(struct Cyc_List_List*)(_tmp2A.Rel_n).val;if(_tmp2C != 0)goto _LL22;_LL21:(*
_tmp1).f1=Cyc_Absyn_Loc_n;{struct Cyc_Absyn_Var_p_struct _tmp349;struct Cyc_Absyn_Var_p_struct*
_tmp348;p->r=(void*)((_tmp348=_cycalloc(sizeof(*_tmp348)),((_tmp348[0]=((_tmp349.tag=
0,((_tmp349.f1=Cyc_Absyn_new_vardecl(_tmp1,(void*)0,0),((_tmp349.f2=Cyc_Absyn_new_pat((
void*)0,0),_tmp349)))))),_tmp348))));}return;_LL22:;_LL23:{const char*_tmp34C;
void*_tmp34B;(_tmp34B=0,Cyc_Tcutil_terr(p->loc,((_tmp34C="qualified variable in pattern",
_tag_dyneither(_tmp34C,sizeof(char),30))),_tag_dyneither(_tmp34B,sizeof(void*),0)));}
return;_LL1D:;}_LL3: if(*((int*)_tmp0)!= 13)goto _LL5;_tmp2=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp0)->f1;_tmp3=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f2;_tmp4=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f3;_LL4:{struct _handler_cons _tmp31;
_push_handler(& _tmp31);{int _tmp33=0;if(setjmp(_tmp31.handler))_tmp33=1;if(!
_tmp33){{struct _RegionHandle*_tmp34=Cyc_Tcenv_get_fnrgn(te);void*_tmp35=Cyc_Tcenv_lookup_ordinary(
_tmp34,te,p->loc,_tmp2);struct Cyc_Absyn_Aggrdecl*_tmp36;struct Cyc_Absyn_Datatypedecl*
_tmp37;struct Cyc_Absyn_Datatypefield*_tmp38;_LL25: if(*((int*)_tmp35)!= 1)goto
_LL27;_tmp36=((struct Cyc_Tcenv_AggrRes_struct*)_tmp35)->f1;_LL26: {struct Cyc_List_List*
_tmp39=0;for(0;_tmp3 != 0;_tmp3=_tmp3->tl){struct _tuple14*_tmp34F;struct Cyc_List_List*
_tmp34E;_tmp39=((_tmp34E=_cycalloc(sizeof(*_tmp34E)),((_tmp34E->hd=((_tmp34F=
_cycalloc(sizeof(*_tmp34F)),((_tmp34F->f1=0,((_tmp34F->f2=(struct Cyc_Absyn_Pat*)
_tmp3->hd,_tmp34F)))))),((_tmp34E->tl=_tmp39,_tmp34E))))));}{struct Cyc_Absyn_Aggr_p_struct
_tmp359;struct Cyc_Absyn_Aggrdecl**_tmp358;struct Cyc_Absyn_AggrInfo*_tmp357;
struct Cyc_Absyn_Aggr_p_struct*_tmp356;p->r=(void*)((_tmp356=_cycalloc(sizeof(*
_tmp356)),((_tmp356[0]=((_tmp359.tag=5,((_tmp359.f1=((_tmp357=_cycalloc(sizeof(*
_tmp357)),((_tmp357->aggr_info=Cyc_Absyn_KnownAggr(((_tmp358=_cycalloc(sizeof(*
_tmp358)),((_tmp358[0]=_tmp36,_tmp358))))),((_tmp357->targs=0,_tmp357)))))),((
_tmp359.f2=0,((_tmp359.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp39),((_tmp359.f4=_tmp4,_tmp359)))))))))),_tmp356))));}_npop_handler(0);
return;}_LL27: if(*((int*)_tmp35)!= 2)goto _LL29;_tmp37=((struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp35)->f1;_tmp38=((struct Cyc_Tcenv_DatatypeRes_struct*)_tmp35)->f2;_LL28:{
struct Cyc_Absyn_Datatype_p_struct _tmp35C;struct Cyc_Absyn_Datatype_p_struct*
_tmp35B;p->r=(void*)((_tmp35B=_cycalloc(sizeof(*_tmp35B)),((_tmp35B[0]=((_tmp35C.tag=
6,((_tmp35C.f1=_tmp37,((_tmp35C.f2=_tmp38,((_tmp35C.f3=_tmp3,((_tmp35C.f4=_tmp4,
_tmp35C)))))))))),_tmp35B))));}_npop_handler(0);return;_LL29: if(*((int*)_tmp35)
!= 3)goto _LL2B;_LL2A: goto _LL2C;_LL2B: if(*((int*)_tmp35)!= 4)goto _LL2D;_LL2C:{
const char*_tmp35F;void*_tmp35E;(_tmp35E=0,Cyc_Tcutil_terr(p->loc,((_tmp35F="enum tag used with arguments in pattern",
_tag_dyneither(_tmp35F,sizeof(char),40))),_tag_dyneither(_tmp35E,sizeof(void*),0)));}
p->r=(void*)0;_npop_handler(0);return;_LL2D: if(*((int*)_tmp35)!= 0)goto _LL24;
_LL2E: goto _LL24;_LL24:;};_pop_handler();}else{void*_tmp32=(void*)_exn_thrown;
void*_tmp45=_tmp32;_LL30: if(_tmp45 != Cyc_Dict_Absent)goto _LL32;_LL31: goto _LL2F;
_LL32:;_LL33:(void)_throw(_tmp45);_LL2F:;}}}{const char*_tmp363;void*_tmp362[1];
struct Cyc_String_pa_struct _tmp361;(_tmp361.tag=0,((_tmp361.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2)),((_tmp362[
0]=& _tmp361,Cyc_Tcutil_terr(p->loc,((_tmp363="%s is not a constructor in pattern",
_tag_dyneither(_tmp363,sizeof(char),35))),_tag_dyneither(_tmp362,sizeof(void*),1)))))));}
p->r=(void*)0;return;_LL5: if(*((int*)_tmp0)!= 5)goto _LL7;_tmp5=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f1;if(_tmp5 != 0)goto _LL7;_tmp6=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;
_tmp7=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f3;_tmp8=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f4;_LL6: if(topt == 0){const char*_tmp366;void*_tmp365;(_tmp365=0,Cyc_Tcutil_terr(
p->loc,((_tmp366="cannot determine pattern type",_tag_dyneither(_tmp366,sizeof(
char),30))),_tag_dyneither(_tmp365,sizeof(void*),0)));}{void*t=Cyc_Tcutil_compress(*((
void**)_check_null(topt)));{void*_tmp4B=t;struct Cyc_Absyn_AggrInfo _tmp4C;_LL35:
if(_tmp4B <= (void*)4)goto _LL37;if(*((int*)_tmp4B)!= 10)goto _LL37;_tmp4C=((struct
Cyc_Absyn_AggrType_struct*)_tmp4B)->f1;_LL36:{struct Cyc_Absyn_Aggr_p_struct
_tmp36C;struct Cyc_Absyn_AggrInfo*_tmp36B;struct Cyc_Absyn_Aggr_p_struct*_tmp36A;p->r=(
void*)((_tmp36A=_cycalloc(sizeof(*_tmp36A)),((_tmp36A[0]=((_tmp36C.tag=5,((
_tmp36C.f1=((_tmp36B=_cycalloc(sizeof(*_tmp36B)),((_tmp36B[0]=_tmp4C,_tmp36B)))),((
_tmp36C.f2=_tmp6,((_tmp36C.f3=_tmp7,((_tmp36C.f4=_tmp8,_tmp36C)))))))))),_tmp36A))));}
Cyc_Tcpat_resolve_pat(te,topt,p);goto _LL34;_LL37:;_LL38:{const char*_tmp370;void*
_tmp36F[1];struct Cyc_String_pa_struct _tmp36E;(_tmp36E.tag=0,((_tmp36E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp36F[0]=&
_tmp36E,Cyc_Tcutil_terr(p->loc,((_tmp370="pattern expects aggregate type instead of %s",
_tag_dyneither(_tmp370,sizeof(char),45))),_tag_dyneither(_tmp36F,sizeof(void*),1)))))));}
goto _LL34;_LL34:;}return;}_LL7: if(*((int*)_tmp0)!= 5)goto _LL9;_tmp9=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f1;if(_tmp9 == 0)goto _LL9;_tmpA=*_tmp9;_tmpB=_tmpA.aggr_info;if((_tmpB.UnknownAggr).tag
!= 1)goto _LL9;_tmpC=(struct _tuple3)(_tmpB.UnknownAggr).val;_tmpD=_tmpC.f1;_tmpE=
_tmpC.f2;_tmpF=_tmpA.targs;_tmp10=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;
_tmp11=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;
_tmp12=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f3;_tmp13=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f4;_LL8:{struct _handler_cons _tmp53;_push_handler(& _tmp53);{int _tmp55=0;
if(setjmp(_tmp53.handler))_tmp55=1;if(!_tmp55){{struct Cyc_Absyn_Aggrdecl**_tmp56=
Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmpE);struct Cyc_Absyn_Aggrdecl*_tmp57=*
_tmp56;if(_tmp57->impl == 0){{const char*_tmp378;const char*_tmp377;const char*
_tmp376;void*_tmp375[1];struct Cyc_String_pa_struct _tmp374;(_tmp374.tag=0,((
_tmp374.f1=(struct _dyneither_ptr)(_tmp57->kind == (void*)0?(_tmp377="struct",
_tag_dyneither(_tmp377,sizeof(char),7)):((_tmp378="union",_tag_dyneither(_tmp378,
sizeof(char),6)))),((_tmp375[0]=& _tmp374,Cyc_Tcutil_terr(p->loc,((_tmp376="can't destructure an abstract %s",
_tag_dyneither(_tmp376,sizeof(char),33))),_tag_dyneither(_tmp375,sizeof(void*),1)))))));}
p->r=(void*)0;_npop_handler(0);return;}{int more_exists=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp57->impl))->exist_vars)
- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp11);if(more_exists < 0){{
const char*_tmp37B;void*_tmp37A;(_tmp37A=0,Cyc_Tcutil_terr(p->loc,((_tmp37B="too many existentially bound type variables in pattern",
_tag_dyneither(_tmp37B,sizeof(char),55))),_tag_dyneither(_tmp37A,sizeof(void*),0)));}{
struct Cyc_List_List**_tmp5F=_tmp11;{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp57->impl))->exist_vars);for(0;n != 
0;-- n){_tmp5F=&((struct Cyc_List_List*)_check_null(*_tmp5F))->tl;}}*_tmp5F=0;}}
else{if(more_exists > 0){struct Cyc_List_List*_tmp60=0;for(0;more_exists != 0;--
more_exists){struct Cyc_Absyn_Unknown_kb_struct*_tmp381;struct Cyc_Absyn_Unknown_kb_struct
_tmp380;struct Cyc_List_List*_tmp37F;_tmp60=((_tmp37F=_cycalloc(sizeof(*_tmp37F)),((
_tmp37F->hd=Cyc_Tcutil_new_tvar((void*)((_tmp381=_cycalloc(sizeof(*_tmp381)),((
_tmp381[0]=((_tmp380.tag=1,((_tmp380.f1=0,_tmp380)))),_tmp381))))),((_tmp37F->tl=
_tmp60,_tmp37F))))));}*_tmp11=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp11,_tmp60);}}{struct Cyc_Absyn_Aggr_p_struct
_tmp387;struct Cyc_Absyn_AggrInfo*_tmp386;struct Cyc_Absyn_Aggr_p_struct*_tmp385;p->r=(
void*)((_tmp385=_cycalloc(sizeof(*_tmp385)),((_tmp385[0]=((_tmp387.tag=5,((
_tmp387.f1=((_tmp386=_cycalloc(sizeof(*_tmp386)),((_tmp386->aggr_info=Cyc_Absyn_KnownAggr(
_tmp56),((_tmp386->targs=_tmpF,_tmp386)))))),((_tmp387.f2=*_tmp11,((_tmp387.f3=
_tmp12,((_tmp387.f4=_tmp13,_tmp387)))))))))),_tmp385))));}}};_pop_handler();}
else{void*_tmp54=(void*)_exn_thrown;void*_tmp68=_tmp54;_LL3A: if(_tmp68 != Cyc_Dict_Absent)
goto _LL3C;_LL3B:{const char*_tmp38A;void*_tmp389;(_tmp389=0,Cyc_Tcutil_terr(p->loc,((
_tmp38A="Non-aggregate name has designator patterns",_tag_dyneither(_tmp38A,
sizeof(char),43))),_tag_dyneither(_tmp389,sizeof(void*),0)));}p->r=(void*)0;goto
_LL39;_LL3C:;_LL3D:(void)_throw(_tmp68);_LL39:;}}}return;_LL9: if(*((int*)_tmp0)
!= 14)goto _LLB;_tmp14=((struct Cyc_Absyn_Exp_p_struct*)_tmp0)->f1;_LLA: Cyc_Tcexp_tcExp(
te,0,_tmp14);if(!Cyc_Tcutil_is_const_exp(te,_tmp14)){{const char*_tmp38D;void*
_tmp38C;(_tmp38C=0,Cyc_Tcutil_terr(p->loc,((_tmp38D="non-constant expression in case pattern",
_tag_dyneither(_tmp38D,sizeof(char),40))),_tag_dyneither(_tmp38C,sizeof(void*),0)));}
p->r=(void*)0;}{unsigned int _tmp6E;int _tmp6F;struct _tuple13 _tmp6D=Cyc_Evexp_eval_const_uint_exp(
_tmp14);_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;{struct Cyc_Absyn_Int_p_struct _tmp390;
struct Cyc_Absyn_Int_p_struct*_tmp38F;p->r=(void*)((_tmp38F=_cycalloc(sizeof(*
_tmp38F)),((_tmp38F[0]=((_tmp390.tag=7,((_tmp390.f1=(void*)((void*)2),((_tmp390.f2=(
int)_tmp6E,_tmp390)))))),_tmp38F))));}return;}_LLB:;_LLC: return;_LL0:;}static
struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd);static struct
_dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){return(*vd->name).f2;}
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt);static void*Cyc_Tcpat_any_type(
struct Cyc_List_List*s,void**topt){if(topt != 0)return*topt;{struct Cyc_Core_Opt*
_tmp391;return Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,((_tmp391=
_cycalloc(sizeof(*_tmp391)),((_tmp391->v=s,_tmp391)))));}}static void*Cyc_Tcpat_num_type(
void**topt,void*numt);static void*Cyc_Tcpat_num_type(void**topt,void*numt){if(
topt != 0  && Cyc_Tcutil_coerceable(*topt))return*topt;{void*_tmp73=Cyc_Tcutil_compress(
numt);_LL3F: if(_tmp73 <= (void*)4)goto _LL41;if(*((int*)_tmp73)!= 12)goto _LL41;
_LL40: if(topt != 0)return*topt;goto _LL3E;_LL41:;_LL42: goto _LL3E;_LL3E:;}return
numt;}static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**
v_result_ptr,void*t);static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,
struct Cyc_List_List**v_result_ptr,void*t){vd->type=t;vd->tq=Cyc_Absyn_empty_tqual(
0);{struct Cyc_List_List*_tmp392;*v_result_ptr=(struct Cyc_List_List*)((_tmp392=
_cycalloc(sizeof(*_tmp392)),((_tmp392->hd=vd,((_tmp392->tl=*v_result_ptr,_tmp392))))));}}
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult
res1,struct Cyc_Tcpat_TcPatResult res2);static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(
struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){struct _tuple12*
_tmp76;struct Cyc_List_List*_tmp77;struct Cyc_Tcpat_TcPatResult _tmp75=res1;_tmp76=
_tmp75.tvars_and_bounds_opt;_tmp77=_tmp75.patvars;{struct _tuple12*_tmp79;struct
Cyc_List_List*_tmp7A;struct Cyc_Tcpat_TcPatResult _tmp78=res2;_tmp79=_tmp78.tvars_and_bounds_opt;
_tmp7A=_tmp78.patvars;if(_tmp76 != 0  || _tmp79 != 0){if(_tmp76 == 0){struct _tuple12*
_tmp393;_tmp76=((_tmp393=_cycalloc(sizeof(*_tmp393)),((_tmp393->f1=0,((_tmp393->f2=
0,_tmp393))))));}if(_tmp79 == 0){struct _tuple12*_tmp394;_tmp79=((_tmp394=
_cycalloc(sizeof(*_tmp394)),((_tmp394->f1=0,((_tmp394->f2=0,_tmp394))))));}{
struct _tuple12*_tmp397;struct Cyc_Tcpat_TcPatResult _tmp396;return(_tmp396.tvars_and_bounds_opt=((
_tmp397=_cycalloc(sizeof(*_tmp397)),((_tmp397->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((struct _tuple12*)
_check_null(_tmp76))).f1,(*((struct _tuple12*)_check_null(_tmp79))).f1),((_tmp397->f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((
struct _tuple12*)_check_null(_tmp76))).f2,(*((struct _tuple12*)_check_null(_tmp79))).f2),
_tmp397)))))),((_tmp396.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(_tmp77,_tmp7A),_tmp396)));}}{struct Cyc_Tcpat_TcPatResult
_tmp398;return(_tmp398.tvars_and_bounds_opt=0,((_tmp398.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp77,_tmp7A),
_tmp398)));}}}static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(struct Cyc_Position_Segment*
loc);static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Pat*_tmp399;return(_tmp399=_cycalloc(sizeof(*_tmp399)),((_tmp399->loc=
loc,((_tmp399->topt=0,((_tmp399->r=(void*)0,_tmp399)))))));}struct _tuple15{
struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple16{struct Cyc_Absyn_Aggrfield*f1;
struct Cyc_Absyn_Pat*f2;};static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void**rgn_opt);static
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p,void**topt,void**rgn_opt){Cyc_Tcpat_resolve_pat(te,topt,p);{void*t;struct Cyc_Tcpat_TcPatResult
_tmp39A;struct Cyc_Tcpat_TcPatResult res=(_tmp39A.tvars_and_bounds_opt=0,((_tmp39A.patvars=
0,_tmp39A)));{void*_tmp81=p->r;struct Cyc_Absyn_Vardecl*_tmp82;struct Cyc_Absyn_Pat*
_tmp83;struct Cyc_Absyn_Vardecl*_tmp84;struct Cyc_Absyn_Pat*_tmp85;struct Cyc_Absyn_Tvar*
_tmp86;struct Cyc_Absyn_Vardecl*_tmp87;void*_tmp88;void*_tmp89;void*_tmp8A;struct
Cyc_Absyn_Enumdecl*_tmp8B;void*_tmp8C;struct Cyc_Absyn_Pat*_tmp8D;struct Cyc_List_List*
_tmp8E;struct Cyc_List_List**_tmp8F;int _tmp90;struct Cyc_Absyn_AggrInfo*_tmp91;
struct Cyc_Absyn_AggrInfo _tmp92;union Cyc_Absyn_AggrInfoU _tmp93;struct Cyc_Absyn_Aggrdecl**
_tmp94;struct Cyc_Absyn_Aggrdecl*_tmp95;struct Cyc_List_List*_tmp96;struct Cyc_List_List**
_tmp97;struct Cyc_List_List*_tmp98;struct Cyc_List_List*_tmp99;struct Cyc_List_List**
_tmp9A;int _tmp9B;struct Cyc_Absyn_Datatypedecl*_tmp9C;struct Cyc_Absyn_Datatypefield*
_tmp9D;struct Cyc_List_List*_tmp9E;struct Cyc_List_List**_tmp9F;int _tmpA0;struct
Cyc_Absyn_AggrInfo*_tmpA1;struct Cyc_Absyn_AggrInfo*_tmpA2;struct Cyc_Absyn_AggrInfo
_tmpA3;union Cyc_Absyn_AggrInfoU _tmpA4;struct _tuple3 _tmpA5;_LL44: if((int)_tmp81 != 
0)goto _LL46;_LL45: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);goto
_LL43;_LL46: if(_tmp81 <= (void*)2)goto _LL5A;if(*((int*)_tmp81)!= 0)goto _LL48;
_tmp82=((struct Cyc_Absyn_Var_p_struct*)_tmp81)->f1;_tmp83=((struct Cyc_Absyn_Var_p_struct*)
_tmp81)->f2;_LL47: res=Cyc_Tcpat_tcPatRec(te,_tmp83,topt,rgn_opt);t=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp83->topt))->v;Cyc_Tcpat_set_vd(_tmp82,& res.patvars,
t);goto _LL43;_LL48: if(*((int*)_tmp81)!= 1)goto _LL4A;_tmp84=((struct Cyc_Absyn_Reference_p_struct*)
_tmp81)->f1;_tmp85=((struct Cyc_Absyn_Reference_p_struct*)_tmp81)->f2;_LL49: res=
Cyc_Tcpat_tcPatRec(te,_tmp85,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp85->topt))->v;if(rgn_opt == 0){{const char*_tmp39D;void*_tmp39C;(
_tmp39C=0,Cyc_Tcutil_terr(p->loc,((_tmp39D="* pattern would point into an unknown/unallowed region",
_tag_dyneither(_tmp39D,sizeof(char),55))),_tag_dyneither(_tmp39C,sizeof(void*),0)));}
goto _LL43;}else{if(Cyc_Tcutil_is_noalias_pointer(t)){const char*_tmp3A0;void*
_tmp39F;(_tmp39F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp3A0="* pattern cannot take the address of an alias-free path",_tag_dyneither(
_tmp3A0,sizeof(char),56))),_tag_dyneither(_tmp39F,sizeof(void*),0)));}}{struct
Cyc_Absyn_PointerType_struct _tmp3AA;struct Cyc_Absyn_PtrAtts _tmp3A9;struct Cyc_Absyn_PtrInfo
_tmp3A8;struct Cyc_Absyn_PointerType_struct*_tmp3A7;Cyc_Tcpat_set_vd(_tmp84,& res.patvars,(
void*)((_tmp3A7=_cycalloc(sizeof(*_tmp3A7)),((_tmp3A7[0]=((_tmp3AA.tag=4,((
_tmp3AA.f1=((_tmp3A8.elt_typ=t,((_tmp3A8.elt_tq=Cyc_Absyn_empty_tqual(0),((
_tmp3A8.ptr_atts=((_tmp3A9.rgn=*rgn_opt,((_tmp3A9.nullable=Cyc_Absyn_false_conref,((
_tmp3A9.bounds=Cyc_Absyn_empty_conref(),((_tmp3A9.zero_term=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp3A9.ptrloc=0,_tmp3A9)))))))))),_tmp3A8)))))),
_tmp3AA)))),_tmp3A7)))));}goto _LL43;_LL4A: if(*((int*)_tmp81)!= 2)goto _LL4C;
_tmp86=((struct Cyc_Absyn_TagInt_p_struct*)_tmp81)->f1;_tmp87=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp81)->f2;_LL4B: Cyc_Tcpat_set_vd(_tmp87,& res.patvars,_tmp87->type);{struct
_RegionHandle*_tmpAE=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Absyn_Tvar*_tmp3AB[1];
Cyc_Tcenv_add_type_vars(_tmpAE,p->loc,te,((_tmp3AB[0]=_tmp86,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3AB,sizeof(struct Cyc_Absyn_Tvar*),
1)))));}if(res.tvars_and_bounds_opt == 0){struct _tuple12*_tmp3AC;res.tvars_and_bounds_opt=((
_tmp3AC=_cycalloc(sizeof(*_tmp3AC)),((_tmp3AC->f1=0,((_tmp3AC->f2=0,_tmp3AC))))));}{
struct Cyc_List_List*_tmp3AD;(*res.tvars_and_bounds_opt).f1=((_tmp3AD=_cycalloc(
sizeof(*_tmp3AD)),((_tmp3AD->hd=_tmp86,((_tmp3AD->tl=(*res.tvars_and_bounds_opt).f1,
_tmp3AD))))));}t=Cyc_Absyn_uint_typ;goto _LL43;}_LL4C: if(*((int*)_tmp81)!= 7)goto
_LL4E;_tmp88=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp81)->f1;if((int)_tmp88 != 
1)goto _LL4E;_LL4D: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL43;_LL4E:
if(*((int*)_tmp81)!= 7)goto _LL50;_tmp89=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp81)->f1;if((int)_tmp89 != 2)goto _LL50;_LL4F: goto _LL51;_LL50: if(*((int*)_tmp81)
!= 7)goto _LL52;_tmp8A=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp81)->f1;if((int)
_tmp8A != 0)goto _LL52;_LL51: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto
_LL43;_LL52: if(*((int*)_tmp81)!= 8)goto _LL54;_LL53: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);
goto _LL43;_LL54: if(*((int*)_tmp81)!= 9)goto _LL56;_LL55: t=Cyc_Tcpat_num_type(topt,
Cyc_Absyn_float_typ);goto _LL43;_LL56: if(*((int*)_tmp81)!= 10)goto _LL58;_tmp8B=((
struct Cyc_Absyn_Enum_p_struct*)_tmp81)->f1;_LL57:{struct Cyc_Absyn_EnumType_struct
_tmp3B0;struct Cyc_Absyn_EnumType_struct*_tmp3AF;t=Cyc_Tcpat_num_type(topt,(void*)((
_tmp3AF=_cycalloc(sizeof(*_tmp3AF)),((_tmp3AF[0]=((_tmp3B0.tag=12,((_tmp3B0.f1=
_tmp8B->name,((_tmp3B0.f2=(struct Cyc_Absyn_Enumdecl*)_tmp8B,_tmp3B0)))))),
_tmp3AF)))));}goto _LL43;_LL58: if(*((int*)_tmp81)!= 11)goto _LL5A;_tmp8C=(void*)((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp81)->f1;_LL59: t=Cyc_Tcpat_num_type(topt,
_tmp8C);goto _LL43;_LL5A: if((int)_tmp81 != 1)goto _LL5C;_LL5B: if(topt != 0){void*
_tmpB4=Cyc_Tcutil_compress(*topt);_LL6F: if(_tmpB4 <= (void*)4)goto _LL71;if(*((int*)
_tmpB4)!= 4)goto _LL71;_LL70: t=*topt;goto tcpat_end;_LL71:;_LL72: goto _LL6E;_LL6E:;}{
struct Cyc_Core_Opt*_tmpB5=Cyc_Tcenv_lookup_opt_type_vars(te);{struct Cyc_Absyn_PointerType_struct
_tmp3BA;struct Cyc_Absyn_PtrAtts _tmp3B9;struct Cyc_Absyn_PtrInfo _tmp3B8;struct Cyc_Absyn_PointerType_struct*
_tmp3B7;t=(void*)((_tmp3B7=_cycalloc(sizeof(*_tmp3B7)),((_tmp3B7[0]=((_tmp3BA.tag=
4,((_tmp3BA.f1=((_tmp3B8.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,
_tmpB5),((_tmp3B8.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp3B8.ptr_atts=((_tmp3B9.rgn=
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,_tmpB5),((_tmp3B9.nullable=
Cyc_Absyn_true_conref,((_tmp3B9.bounds=Cyc_Absyn_empty_conref(),((_tmp3B9.zero_term=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp3B9.ptrloc=0,
_tmp3B9)))))))))),_tmp3B8)))))),_tmp3BA)))),_tmp3B7))));}goto _LL43;}_LL5C: if(
_tmp81 <= (void*)2)goto _LL5E;if(*((int*)_tmp81)!= 4)goto _LL5E;_tmp8D=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp81)->f1;_LL5D: {void*inner_typ=(void*)0;void**_tmpBA=0;if(topt != 0){void*
_tmpBB=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmpBC;void*_tmpBD;
_LL74: if(_tmpBB <= (void*)4)goto _LL76;if(*((int*)_tmpBB)!= 4)goto _LL76;_tmpBC=((
struct Cyc_Absyn_PointerType_struct*)_tmpBB)->f1;_tmpBD=_tmpBC.elt_typ;_LL75:
inner_typ=_tmpBD;_tmpBA=(void**)& inner_typ;goto _LL73;_LL76:;_LL77: goto _LL73;
_LL73:;}{void*ptr_rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,
Cyc_Tcenv_lookup_opt_type_vars(te));res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(
te,_tmp8D,_tmpBA,(void**)& ptr_rgn));if(_tmpBA == 0){void*_tmpBE=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp8D->topt))->v);struct Cyc_Absyn_DatatypeFieldInfo
_tmpBF;union Cyc_Absyn_DatatypeFieldInfoU _tmpC0;struct _tuple2 _tmpC1;struct Cyc_Absyn_Datatypedecl*
_tmpC2;struct Cyc_Absyn_Datatypefield*_tmpC3;struct Cyc_List_List*_tmpC4;_LL79: if(
_tmpBE <= (void*)4)goto _LL7B;if(*((int*)_tmpBE)!= 3)goto _LL7B;_tmpBF=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpBE)->f1;_tmpC0=_tmpBF.field_info;if((_tmpC0.KnownDatatypefield).tag != 2)goto
_LL7B;_tmpC1=(struct _tuple2)(_tmpC0.KnownDatatypefield).val;_tmpC2=_tmpC1.f1;
_tmpC3=_tmpC1.f2;_tmpC4=_tmpBF.targs;_LL7A:{struct Cyc_Absyn_DatatypeType_struct
_tmp3C8;struct Cyc_Absyn_Datatypedecl**_tmp3C7;struct Cyc_Core_Opt*_tmp3C6;struct
Cyc_Absyn_DatatypeInfo _tmp3C5;struct Cyc_Absyn_DatatypeType_struct*_tmp3C4;t=(
void*)((_tmp3C4=_cycalloc(sizeof(*_tmp3C4)),((_tmp3C4[0]=((_tmp3C8.tag=2,((
_tmp3C8.f1=((_tmp3C5.datatype_info=Cyc_Absyn_KnownDatatype(((_tmp3C7=_cycalloc(
sizeof(*_tmp3C7)),((_tmp3C7[0]=_tmpC2,_tmp3C7))))),((_tmp3C5.targs=_tmpC4,((
_tmp3C5.rgn=((_tmp3C6=_cycalloc(sizeof(*_tmp3C6)),((_tmp3C6->v=(void*)ptr_rgn,
_tmp3C6)))),_tmp3C5)))))),_tmp3C8)))),_tmp3C4))));}goto _LL78;_LL7B:;_LL7C:{
struct Cyc_Absyn_PointerType_struct _tmp3D2;struct Cyc_Absyn_PtrAtts _tmp3D1;struct
Cyc_Absyn_PtrInfo _tmp3D0;struct Cyc_Absyn_PointerType_struct*_tmp3CF;t=(void*)((
_tmp3CF=_cycalloc(sizeof(*_tmp3CF)),((_tmp3CF[0]=((_tmp3D2.tag=4,((_tmp3D2.f1=((
_tmp3D0.elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp8D->topt))->v,((
_tmp3D0.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp3D0.ptr_atts=((_tmp3D1.rgn=ptr_rgn,((
_tmp3D1.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp3D1.bounds=Cyc_Absyn_empty_conref(),((_tmp3D1.zero_term=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp3D1.ptrloc=0,_tmp3D1)))))))))),_tmp3D0)))))),
_tmp3D2)))),_tmp3CF))));}goto _LL78;_LL78:;}else{struct Cyc_Absyn_PointerType_struct
_tmp3DC;struct Cyc_Absyn_PtrAtts _tmp3DB;struct Cyc_Absyn_PtrInfo _tmp3DA;struct Cyc_Absyn_PointerType_struct*
_tmp3D9;t=(void*)((_tmp3D9=_cycalloc(sizeof(*_tmp3D9)),((_tmp3D9[0]=((_tmp3DC.tag=
4,((_tmp3DC.f1=((_tmp3DA.elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp8D->topt))->v,((
_tmp3DA.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp3DA.ptr_atts=((_tmp3DB.rgn=ptr_rgn,((
_tmp3DB.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp3DB.bounds=Cyc_Absyn_empty_conref(),((_tmp3DB.zero_term=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp3DB.ptrloc=0,_tmp3DB)))))))))),_tmp3DA)))))),
_tmp3DC)))),_tmp3D9))));}goto _LL43;}}_LL5E: if(_tmp81 <= (void*)2)goto _LL60;if(*((
int*)_tmp81)!= 3)goto _LL60;_tmp8E=((struct Cyc_Absyn_Tuple_p_struct*)_tmp81)->f1;
_tmp8F=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_struct*)_tmp81)->f1;
_tmp90=((struct Cyc_Absyn_Tuple_p_struct*)_tmp81)->f2;_LL5F: {struct Cyc_List_List*
_tmpD2=*_tmp8F;struct Cyc_List_List*pat_ts=0;struct Cyc_List_List*topt_ts=0;if(
topt != 0){void*_tmpD3=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmpD4;
_LL7E: if(_tmpD3 <= (void*)4)goto _LL80;if(*((int*)_tmpD3)!= 9)goto _LL80;_tmpD4=((
struct Cyc_Absyn_TupleType_struct*)_tmpD3)->f1;_LL7F: topt_ts=_tmpD4;if(_tmp90){
int _tmpD5=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD2);int _tmpD6=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD4);if(_tmpD5 < _tmpD6){struct
Cyc_List_List*wild_ps=0;{int i=0;for(0;i < _tmpD6 - _tmpD5;++ i){struct Cyc_List_List*
_tmp3DD;wild_ps=((_tmp3DD=_cycalloc(sizeof(*_tmp3DD)),((_tmp3DD->hd=Cyc_Tcpat_wild_pat(
p->loc),((_tmp3DD->tl=wild_ps,_tmp3DD))))));}}*_tmp8F=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpD2,wild_ps);
_tmpD2=*_tmp8F;}else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD2)
== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD4)){const char*_tmp3E0;
void*_tmp3DF;(_tmp3DF=0,Cyc_Tcutil_warn(p->loc,((_tmp3E0="unnecessary ... in tuple pattern",
_tag_dyneither(_tmp3E0,sizeof(char),33))),_tag_dyneither(_tmp3DF,sizeof(void*),0)));}}}
goto _LL7D;_LL80:;_LL81: goto _LL7D;_LL7D:;}else{if(_tmp90){const char*_tmp3E3;void*
_tmp3E2;(_tmp3E2=0,Cyc_Tcutil_terr(p->loc,((_tmp3E3="cannot determine missing fields for ... in tuple pattern",
_tag_dyneither(_tmp3E3,sizeof(char),57))),_tag_dyneither(_tmp3E2,sizeof(void*),0)));}}
for(0;_tmpD2 != 0;_tmpD2=_tmpD2->tl){void**_tmpDC=0;if(topt_ts != 0){_tmpDC=(void**)&(*((
struct _tuple15*)topt_ts->hd)).f2;topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmpD2->hd,_tmpDC,rgn_opt));{
struct _tuple15*_tmp3E6;struct Cyc_List_List*_tmp3E5;pat_ts=((_tmp3E5=_cycalloc(
sizeof(*_tmp3E5)),((_tmp3E5->hd=((_tmp3E6=_cycalloc(sizeof(*_tmp3E6)),((_tmp3E6->f1=
Cyc_Absyn_empty_tqual(0),((_tmp3E6->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Pat*)_tmpD2->hd)->topt))->v,_tmp3E6)))))),((_tmp3E5->tl=pat_ts,
_tmp3E5))))));}}{struct Cyc_Absyn_TupleType_struct _tmp3E9;struct Cyc_Absyn_TupleType_struct*
_tmp3E8;t=(void*)((_tmp3E8=_cycalloc(sizeof(*_tmp3E8)),((_tmp3E8[0]=((_tmp3E9.tag=
9,((_tmp3E9.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
pat_ts),_tmp3E9)))),_tmp3E8))));}goto _LL43;}_LL60: if(_tmp81 <= (void*)2)goto _LL62;
if(*((int*)_tmp81)!= 5)goto _LL62;_tmp91=((struct Cyc_Absyn_Aggr_p_struct*)_tmp81)->f1;
if(_tmp91 == 0)goto _LL62;_tmp92=*_tmp91;_tmp93=_tmp92.aggr_info;if((_tmp93.KnownAggr).tag
!= 2)goto _LL62;_tmp94=(struct Cyc_Absyn_Aggrdecl**)(_tmp93.KnownAggr).val;_tmp95=*
_tmp94;_tmp96=_tmp92.targs;_tmp97=(struct Cyc_List_List**)&(*((struct Cyc_Absyn_Aggr_p_struct*)
_tmp81)->f1).targs;_tmp98=((struct Cyc_Absyn_Aggr_p_struct*)_tmp81)->f2;_tmp99=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp81)->f3;_tmp9A=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Aggr_p_struct*)_tmp81)->f3;_tmp9B=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp81)->f4;_LL61: {struct Cyc_List_List*_tmpE1=*_tmp9A;const char*_tmp3EB;const
char*_tmp3EA;struct _dyneither_ptr aggr_str=_tmp95->kind == (void*)0?(_tmp3EB="struct",
_tag_dyneither(_tmp3EB,sizeof(char),7)):((_tmp3EA="union",_tag_dyneither(_tmp3EA,
sizeof(char),6)));if(_tmp95->impl == 0){{const char*_tmp3EF;void*_tmp3EE[1];struct
Cyc_String_pa_struct _tmp3ED;(_tmp3ED.tag=0,((_tmp3ED.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)aggr_str),((_tmp3EE[0]=& _tmp3ED,Cyc_Tcutil_terr(p->loc,((
_tmp3EF="can't destructure an abstract %s",_tag_dyneither(_tmp3EF,sizeof(char),
33))),_tag_dyneither(_tmp3EE,sizeof(void*),1)))))));}t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));goto _LL43;}if(_tmp98 != 0  || _tmp95->kind == (void*)1  && ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp95->impl))->tagged)rgn_opt=0;{struct _RegionHandle _tmpE5=
_new_region("rgn");struct _RegionHandle*rgn=& _tmpE5;_push_region(rgn);{struct Cyc_List_List*
_tmpE6=0;struct Cyc_List_List*outlives_constraints=0;struct Cyc_List_List*_tmpE7=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp95->impl))->exist_vars;{struct Cyc_List_List*
t=_tmp98;for(0;t != 0;t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(_tmpE7))->hd;_tmpE7=_tmpE7->tl;{void*_tmpE8=Cyc_Absyn_compress_kb(tv->kind);
void*_tmpE9=Cyc_Absyn_compress_kb(uv->kind);int error=0;void*k2;{void*_tmpEA=
_tmpE9;void*_tmpEB;void*_tmpEC;_LL83: if(*((int*)_tmpEA)!= 2)goto _LL85;_tmpEB=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpEA)->f2;_LL84: _tmpEC=_tmpEB;goto
_LL86;_LL85: if(*((int*)_tmpEA)!= 0)goto _LL87;_tmpEC=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmpEA)->f1;_LL86: k2=_tmpEC;goto _LL82;_LL87:;_LL88: {const char*_tmp3F2;void*
_tmp3F1;(_tmp3F1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp3F2="unconstrained existential type variable in aggregate",_tag_dyneither(
_tmp3F2,sizeof(char),53))),_tag_dyneither(_tmp3F1,sizeof(void*),0)));}_LL82:;}{
void*_tmpEF=_tmpE8;void*_tmpF0;struct Cyc_Core_Opt*_tmpF1;struct Cyc_Core_Opt**
_tmpF2;void*_tmpF3;struct Cyc_Core_Opt*_tmpF4;struct Cyc_Core_Opt**_tmpF5;_LL8A:
if(*((int*)_tmpEF)!= 0)goto _LL8C;_tmpF0=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmpEF)->f1;_LL8B: if(!Cyc_Tcutil_kind_leq(k2,_tmpF0))error=1;goto _LL89;_LL8C: if(*((
int*)_tmpEF)!= 2)goto _LL8E;_tmpF1=((struct Cyc_Absyn_Less_kb_struct*)_tmpEF)->f1;
_tmpF2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpEF)->f1;
_tmpF3=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpEF)->f2;_LL8D: _tmpF5=_tmpF2;
goto _LL8F;_LL8E: if(*((int*)_tmpEF)!= 1)goto _LL89;_tmpF4=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpEF)->f1;_tmpF5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpEF)->f1;_LL8F:{struct Cyc_Core_Opt*_tmp3F3;*_tmpF5=((_tmp3F3=_cycalloc(
sizeof(*_tmp3F3)),((_tmp3F3->v=(void*)_tmpE9,_tmp3F3))));}goto _LL89;_LL89:;}if(
error){const char*_tmp3F9;void*_tmp3F8[3];struct Cyc_String_pa_struct _tmp3F7;
struct Cyc_String_pa_struct _tmp3F6;struct Cyc_String_pa_struct _tmp3F5;(_tmp3F5.tag=
0,((_tmp3F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
k2)),((_tmp3F6.tag=0,((_tmp3F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(_tmpE8)),((_tmp3F7.tag=0,((_tmp3F7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp3F8[0]=& _tmp3F7,((_tmp3F8[
1]=& _tmp3F6,((_tmp3F8[2]=& _tmp3F5,Cyc_Tcutil_terr(p->loc,((_tmp3F9="type variable %s has kind %s but must have at least kind %s",
_tag_dyneither(_tmp3F9,sizeof(char),60))),_tag_dyneither(_tmp3F8,sizeof(void*),3)))))))))))))))))));}{
struct Cyc_Absyn_VarType_struct _tmp3FC;struct Cyc_Absyn_VarType_struct*_tmp3FB;
void*vartype=(void*)((_tmp3FB=_cycalloc(sizeof(*_tmp3FB)),((_tmp3FB[0]=((_tmp3FC.tag=
1,((_tmp3FC.f1=tv,_tmp3FC)))),_tmp3FB))));{struct Cyc_List_List*_tmp3FD;_tmpE6=((
_tmp3FD=_region_malloc(rgn,sizeof(*_tmp3FD)),((_tmp3FD->hd=(void*)((void*)
vartype),((_tmp3FD->tl=_tmpE6,_tmp3FD))))));}if(k2 == (void*)3){struct _tuple0*
_tmp400;struct Cyc_List_List*_tmp3FF;outlives_constraints=((_tmp3FF=_cycalloc(
sizeof(*_tmp3FF)),((_tmp3FF->hd=((_tmp400=_cycalloc(sizeof(*_tmp400)),((_tmp400->f1=
Cyc_Absyn_empty_effect,((_tmp400->f2=vartype,_tmp400)))))),((_tmp3FF->tl=
outlives_constraints,_tmp3FF))))));}else{if(k2 == (void*)4  || k2 == (void*)5){
const char*_tmp403;void*_tmp402;(_tmp402=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp403="opened existential had unique or top region kind",
_tag_dyneither(_tmp403,sizeof(char),49))),_tag_dyneither(_tmp402,sizeof(void*),0)));}}}}}}
_tmpE6=Cyc_List_imp_rev(_tmpE6);{struct _RegionHandle*_tmp103=Cyc_Tcenv_get_fnrgn(
te);struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(_tmp103,p->loc,te,_tmp98);
struct Cyc_List_List*_tmp104=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple10
_tmp404;struct _tuple10 _tmp105=(_tmp404.f1=_tmp104,((_tmp404.f2=rgn,_tmp404)));
struct Cyc_List_List*_tmp106=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),struct _tuple10*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp105,
_tmp95->tvs);struct Cyc_List_List*_tmp107=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp95->impl))->exist_vars,
_tmpE6);struct Cyc_List_List*_tmp108=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))Cyc_Core_snd,
_tmp106);struct Cyc_List_List*_tmp109=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))Cyc_Core_snd,
_tmp107);struct Cyc_List_List*_tmp10A=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp106,_tmp107);if(_tmp98 != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp95->impl))->rgn_po != 0){if(res.tvars_and_bounds_opt == 0){struct
_tuple12*_tmp405;res.tvars_and_bounds_opt=((_tmp405=_cycalloc(sizeof(*_tmp405)),((
_tmp405->f1=0,((_tmp405->f2=0,_tmp405))))));}(*res.tvars_and_bounds_opt).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f1,_tmp98);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
outlives_constraints);{struct Cyc_List_List*_tmp10C=0;{struct Cyc_List_List*
_tmp10D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp95->impl))->rgn_po;for(0;
_tmp10D != 0;_tmp10D=_tmp10D->tl){struct _tuple0*_tmp408;struct Cyc_List_List*
_tmp407;_tmp10C=((_tmp407=_cycalloc(sizeof(*_tmp407)),((_tmp407->hd=((_tmp408=
_cycalloc(sizeof(*_tmp408)),((_tmp408->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp10A,(*((
struct _tuple0*)_tmp10D->hd)).f1),((_tmp408->f2=Cyc_Tcutil_rsubstitute(rgn,
_tmp10A,(*((struct _tuple0*)_tmp10D->hd)).f2),_tmp408)))))),((_tmp407->tl=_tmp10C,
_tmp407))))));}}_tmp10C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp10C);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp10C);}}*
_tmp97=_tmp108;{struct Cyc_Absyn_AggrType_struct _tmp412;struct Cyc_Absyn_Aggrdecl**
_tmp411;struct Cyc_Absyn_AggrInfo _tmp410;struct Cyc_Absyn_AggrType_struct*_tmp40F;
t=(void*)((_tmp40F=_cycalloc(sizeof(*_tmp40F)),((_tmp40F[0]=((_tmp412.tag=10,((
_tmp412.f1=((_tmp410.aggr_info=Cyc_Absyn_KnownAggr(((_tmp411=_cycalloc(sizeof(*
_tmp411)),((_tmp411[0]=_tmp95,_tmp411))))),((_tmp410.targs=*_tmp97,_tmp410)))),
_tmp412)))),_tmp40F))));}if(_tmp9B  && _tmp95->kind == (void*)1){const char*_tmp415;
void*_tmp414;(_tmp414=0,Cyc_Tcutil_warn(p->loc,((_tmp415="`...' pattern not allowed in union pattern",
_tag_dyneither(_tmp415,sizeof(char),43))),_tag_dyneither(_tmp414,sizeof(void*),0)));}
else{if(_tmp9B){int _tmp116=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpE1);int _tmp117=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp95->impl))->fields);if(_tmp116 < _tmp117){struct Cyc_List_List*
wild_dps=0;{int i=0;for(0;i < _tmp117 - _tmp116;++ i){struct _tuple14*_tmp418;struct
Cyc_List_List*_tmp417;wild_dps=((_tmp417=_cycalloc(sizeof(*_tmp417)),((_tmp417->hd=((
_tmp418=_cycalloc(sizeof(*_tmp418)),((_tmp418->f1=0,((_tmp418->f2=Cyc_Tcpat_wild_pat(
p->loc),_tmp418)))))),((_tmp417->tl=wild_dps,_tmp417))))));}}*_tmp9A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpE1,
wild_dps);_tmpE1=*_tmp9A;}else{if(_tmp116 == _tmp117){const char*_tmp41B;void*
_tmp41A;(_tmp41A=0,Cyc_Tcutil_warn(p->loc,((_tmp41B="unnecessary ... in struct pattern",
_tag_dyneither(_tmp41B,sizeof(char),34))),_tag_dyneither(_tmp41A,sizeof(void*),0)));}}}}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,void*,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmpE1,_tmp95->kind,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp95->impl))->fields);for(0;fields != 
0;fields=fields->tl){struct _tuple16 _tmp11D;struct Cyc_Absyn_Aggrfield*_tmp11E;
struct Cyc_Absyn_Pat*_tmp11F;struct _tuple16*_tmp11C=(struct _tuple16*)fields->hd;
_tmp11D=*_tmp11C;_tmp11E=_tmp11D.f1;_tmp11F=_tmp11D.f2;{void*_tmp120=Cyc_Tcutil_rsubstitute(
rgn,_tmp10A,_tmp11E->type);res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(
te2,_tmp11F,(void**)& _tmp120,rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp11F->topt))->v,_tmp120)){const char*_tmp422;void*_tmp421[4];
struct Cyc_String_pa_struct _tmp420;struct Cyc_String_pa_struct _tmp41F;struct Cyc_String_pa_struct
_tmp41E;struct Cyc_String_pa_struct _tmp41D;(_tmp41D.tag=0,((_tmp41D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp11F->topt))->v)),((_tmp41E.tag=0,((_tmp41E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp120)),((_tmp41F.tag=
0,((_tmp41F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp420.tag=
0,((_tmp420.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp11E->name),((
_tmp421[0]=& _tmp420,((_tmp421[1]=& _tmp41F,((_tmp421[2]=& _tmp41E,((_tmp421[3]=&
_tmp41D,Cyc_Tcutil_terr(p->loc,((_tmp422="field %s of %s pattern expects type %s != %s",
_tag_dyneither(_tmp422,sizeof(char),45))),_tag_dyneither(_tmp421,sizeof(void*),4)))))))))))))))))))))))));}}}}}}
_npop_handler(0);goto _LL43;;_pop_region(rgn);}}_LL62: if(_tmp81 <= (void*)2)goto
_LL64;if(*((int*)_tmp81)!= 6)goto _LL64;_tmp9C=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp81)->f1;_tmp9D=((struct Cyc_Absyn_Datatype_p_struct*)_tmp81)->f2;_tmp9E=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp81)->f3;_tmp9F=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Datatype_p_struct*)_tmp81)->f3;_tmpA0=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp81)->f4;_LL63: {struct Cyc_List_List*_tmp12A=*_tmp9F;struct _RegionHandle*
_tmp12B=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*tqts=_tmp9D->typs;struct Cyc_List_List*
_tmp12C=Cyc_Tcenv_lookup_type_vars(te);struct _tuple10 _tmp423;struct _tuple10
_tmp12D=(_tmp423.f1=_tmp12C,((_tmp423.f2=_tmp12B,_tmp423)));struct Cyc_List_List*
_tmp12E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(
struct _tuple10*,struct Cyc_Absyn_Tvar*),struct _tuple10*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(_tmp12B,Cyc_Tcutil_r_make_inst_var,& _tmp12D,_tmp9C->tvs);struct
Cyc_List_List*_tmp12F=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple11*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))Cyc_Core_snd,
_tmp12E);if(tqts == 0){struct Cyc_Core_Opt*_tmp426;struct Cyc_Core_Opt*_tmp425;
struct Cyc_Core_Opt*ropt=(_tmp425=_cycalloc(sizeof(*_tmp425)),((_tmp425->v=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,((_tmp426=_cycalloc(
sizeof(*_tmp426)),((_tmp426->v=_tmp12C,_tmp426))))),_tmp425)));struct Cyc_Absyn_DatatypeType_struct
_tmp430;struct Cyc_Absyn_Datatypedecl**_tmp42F;struct Cyc_Absyn_DatatypeInfo
_tmp42E;struct Cyc_Absyn_DatatypeType_struct*_tmp42D;t=(void*)((_tmp42D=_cycalloc(
sizeof(*_tmp42D)),((_tmp42D[0]=((_tmp430.tag=2,((_tmp430.f1=((_tmp42E.datatype_info=
Cyc_Absyn_KnownDatatype(((_tmp42F=_cycalloc(sizeof(*_tmp42F)),((_tmp42F[0]=
_tmp9C,_tmp42F))))),((_tmp42E.targs=_tmp12F,((_tmp42E.rgn=ropt,_tmp42E)))))),
_tmp430)))),_tmp42D))));}else{struct Cyc_Absyn_DatatypeFieldType_struct _tmp436;
struct Cyc_Absyn_DatatypeFieldInfo _tmp435;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp434;t=(void*)((_tmp434=_cycalloc(sizeof(*_tmp434)),((_tmp434[0]=((_tmp436.tag=
3,((_tmp436.f1=((_tmp435.field_info=Cyc_Absyn_KnownDatatypefield(_tmp9C,_tmp9D),((
_tmp435.targs=_tmp12F,_tmp435)))),_tmp436)))),_tmp434))));}if(topt != 0  && tqts == 
0){void*_tmp139=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_DatatypeInfo _tmp13A;
struct Cyc_List_List*_tmp13B;_LL91: if(_tmp139 <= (void*)4)goto _LL95;if(*((int*)
_tmp139)!= 3)goto _LL93;_LL92:{struct Cyc_Absyn_DatatypeFieldType_struct _tmp43C;
struct Cyc_Absyn_DatatypeFieldInfo _tmp43B;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp43A;t=(void*)((_tmp43A=_cycalloc(sizeof(*_tmp43A)),((_tmp43A[0]=((_tmp43C.tag=
3,((_tmp43C.f1=((_tmp43B.field_info=Cyc_Absyn_KnownDatatypefield(_tmp9C,_tmp9D),((
_tmp43B.targs=_tmp12F,_tmp43B)))),_tmp43C)))),_tmp43A))));}goto _LL90;_LL93: if(*((
int*)_tmp139)!= 2)goto _LL95;_tmp13A=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp139)->f1;_tmp13B=_tmp13A.targs;_LL94: {struct Cyc_List_List*_tmp13F=_tmp12F;
for(0;_tmp13F != 0  && _tmp13B != 0;(_tmp13F=_tmp13F->tl,_tmp13B=_tmp13B->tl)){Cyc_Tcutil_unify((
void*)_tmp13F->hd,(void*)_tmp13B->hd);}goto _LL90;}_LL95:;_LL96: goto _LL90;_LL90:;}
if(_tmpA0){int _tmp140=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp12A);
int _tmp141=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);if(_tmp140 < 
_tmp141){struct Cyc_List_List*wild_ps=0;{int i=0;for(0;i < _tmp141 - _tmp140;++ i){
struct Cyc_List_List*_tmp43D;wild_ps=((_tmp43D=_cycalloc(sizeof(*_tmp43D)),((
_tmp43D->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp43D->tl=wild_ps,_tmp43D))))));}}*
_tmp9F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp12A,wild_ps);_tmp12A=*_tmp9F;}else{if(_tmp140 == _tmp141){
const char*_tmp441;void*_tmp440[1];struct Cyc_String_pa_struct _tmp43F;(_tmp43F.tag=
0,((_tmp43F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp9C->name)),((_tmp440[0]=& _tmp43F,Cyc_Tcutil_warn(p->loc,((_tmp441="unnecessary ... in datatype field %s",
_tag_dyneither(_tmp441,sizeof(char),37))),_tag_dyneither(_tmp440,sizeof(void*),1)))))));}}}
for(0;_tmp12A != 0  && tqts != 0;(_tmp12A=_tmp12A->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*
_tmp146=(struct Cyc_Absyn_Pat*)_tmp12A->hd;void*_tmp147=Cyc_Tcutil_rsubstitute(
_tmp12B,_tmp12E,(*((struct _tuple15*)tqts->hd)).f2);res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmp146,(void**)& _tmp147,rgn_opt));if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp146->topt))->v,_tmp147)){const char*
_tmp447;void*_tmp446[3];struct Cyc_String_pa_struct _tmp445;struct Cyc_String_pa_struct
_tmp444;struct Cyc_String_pa_struct _tmp443;(_tmp443.tag=0,((_tmp443.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp146->topt))->v)),((_tmp444.tag=0,((_tmp444.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp147)),((_tmp445.tag=
0,((_tmp445.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp9D->name)),((_tmp446[0]=& _tmp445,((_tmp446[1]=& _tmp444,((_tmp446[2]=& _tmp443,
Cyc_Tcutil_terr(_tmp146->loc,((_tmp447="%s expects argument type %s, not %s",
_tag_dyneither(_tmp447,sizeof(char),36))),_tag_dyneither(_tmp446,sizeof(void*),3)))))))))))))))))));}}
if(_tmp12A != 0){const char*_tmp44B;void*_tmp44A[1];struct Cyc_String_pa_struct
_tmp449;(_tmp449.tag=0,((_tmp449.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9D->name)),((_tmp44A[0]=& _tmp449,Cyc_Tcutil_terr(p->loc,((
_tmp44B="too many arguments for datatype constructor %s",_tag_dyneither(_tmp44B,
sizeof(char),47))),_tag_dyneither(_tmp44A,sizeof(void*),1)))))));}if(tqts != 0){
const char*_tmp44F;void*_tmp44E[1];struct Cyc_String_pa_struct _tmp44D;(_tmp44D.tag=
0,((_tmp44D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp9D->name)),((_tmp44E[0]=& _tmp44D,Cyc_Tcutil_terr(p->loc,((_tmp44F="too few arguments for datatype constructor %s",
_tag_dyneither(_tmp44F,sizeof(char),46))),_tag_dyneither(_tmp44E,sizeof(void*),1)))))));}}
goto _LL43;}_LL64: if(_tmp81 <= (void*)2)goto _LL66;if(*((int*)_tmp81)!= 5)goto _LL66;
_tmpA1=((struct Cyc_Absyn_Aggr_p_struct*)_tmp81)->f1;if(_tmpA1 != 0)goto _LL66;
_LL65: goto _LL67;_LL66: if(_tmp81 <= (void*)2)goto _LL68;if(*((int*)_tmp81)!= 5)goto
_LL68;_tmpA2=((struct Cyc_Absyn_Aggr_p_struct*)_tmp81)->f1;if(_tmpA2 == 0)goto
_LL68;_tmpA3=*_tmpA2;_tmpA4=_tmpA3.aggr_info;if((_tmpA4.UnknownAggr).tag != 1)
goto _LL68;_tmpA5=(struct _tuple3)(_tmpA4.UnknownAggr).val;_LL67: goto _LL69;_LL68:
if(_tmp81 <= (void*)2)goto _LL6A;if(*((int*)_tmp81)!= 12)goto _LL6A;_LL69: goto _LL6B;
_LL6A: if(_tmp81 <= (void*)2)goto _LL6C;if(*((int*)_tmp81)!= 14)goto _LL6C;_LL6B:
goto _LL6D;_LL6C: if(_tmp81 <= (void*)2)goto _LL43;if(*((int*)_tmp81)!= 13)goto _LL43;
_LL6D: t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL43;_LL43:;}
tcpat_end: {struct Cyc_Core_Opt*_tmp450;p->topt=((_tmp450=_cycalloc(sizeof(*
_tmp450)),((_tmp450->v=(void*)t,_tmp450))));}return res;}}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt);struct
Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p,void**topt){struct Cyc_Tcpat_TcPatResult _tmp156=Cyc_Tcpat_tcPatRec(te,p,(void**)
topt,0);struct _RegionHandle*_tmp157=Cyc_Tcenv_get_fnrgn(te);{const char*_tmp451;
Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmp157,Cyc_Tcpat_get_name,_tmp156.patvars),p->loc,((_tmp451="pattern contains a repeated variable",
_tag_dyneither(_tmp451,sizeof(char),37))));}return _tmp156;}void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*_tmp159=p->r;struct Cyc_Absyn_Pat*
_tmp15A;struct Cyc_List_List*_tmp15B;struct Cyc_List_List*_tmp15C;struct Cyc_List_List*
_tmp15D;_LL98: if(_tmp159 <= (void*)2)goto _LLA0;if(*((int*)_tmp159)!= 4)goto _LL9A;
_tmp15A=((struct Cyc_Absyn_Pointer_p_struct*)_tmp159)->f1;_LL99: Cyc_Tcpat_check_pat_regions(
te,_tmp15A);{struct _RegionHandle*_tmp15E=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp15E,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp15A->topt))->v)){{const char*
_tmp454;void*_tmp453;(_tmp453=0,Cyc_Tcutil_terr(p->loc,((_tmp454="Pattern dereferences to a non-aliased pointer; use swap",
_tag_dyneither(_tmp454,sizeof(char),56))),_tag_dyneither(_tmp453,sizeof(void*),0)));}
return;}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp15E,(void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v)){{const char*_tmp457;void*_tmp456;(_tmp456=0,Cyc_Tcutil_terr(
p->loc,((_tmp457="Pattern dereferences a non-aliased pointer; use swap",
_tag_dyneither(_tmp457,sizeof(char),53))),_tag_dyneither(_tmp456,sizeof(void*),0)));}
return;}{void*_tmp163=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_PtrInfo _tmp164;struct Cyc_Absyn_PtrAtts _tmp165;void*_tmp166;
struct Cyc_Absyn_DatatypeInfo _tmp167;struct Cyc_Core_Opt*_tmp168;struct Cyc_Core_Opt
_tmp169;void*_tmp16A;_LLA3: if(_tmp163 <= (void*)4)goto _LLA7;if(*((int*)_tmp163)!= 
4)goto _LLA5;_tmp164=((struct Cyc_Absyn_PointerType_struct*)_tmp163)->f1;_tmp165=
_tmp164.ptr_atts;_tmp166=_tmp165.rgn;_LLA4: _tmp16A=_tmp166;goto _LLA6;_LLA5: if(*((
int*)_tmp163)!= 2)goto _LLA7;_tmp167=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp163)->f1;_tmp168=_tmp167.rgn;if(_tmp168 == 0)goto _LLA7;_tmp169=*_tmp168;
_tmp16A=(void*)_tmp169.v;_LLA6: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp16A);
return;_LLA7:;_LLA8: {const char*_tmp45A;void*_tmp459;(_tmp459=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp45A="check_pat_regions: bad pointer type",
_tag_dyneither(_tmp45A,sizeof(char),36))),_tag_dyneither(_tmp459,sizeof(void*),0)));}
_LLA2:;}}_LL9A: if(*((int*)_tmp159)!= 6)goto _LL9C;_tmp15B=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp159)->f3;_LL9B: for(0;_tmp15B != 0;_tmp15B=_tmp15B->tl){Cyc_Tcpat_check_pat_regions(
te,(struct Cyc_Absyn_Pat*)_tmp15B->hd);}{void*_tmp16D=(void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v;struct Cyc_Absyn_DatatypeInfo _tmp16E;struct Cyc_Core_Opt*
_tmp16F;struct Cyc_Absyn_DatatypeInfo _tmp170;struct Cyc_Core_Opt*_tmp171;struct Cyc_Core_Opt
_tmp172;void*_tmp173;_LLAA: if(_tmp16D <= (void*)4)goto _LLB0;if(*((int*)_tmp16D)!= 
2)goto _LLAC;_tmp16E=((struct Cyc_Absyn_DatatypeType_struct*)_tmp16D)->f1;_tmp16F=
_tmp16E.rgn;if(_tmp16F != 0)goto _LLAC;_LLAB: return;_LLAC: if(*((int*)_tmp16D)!= 2)
goto _LLAE;_tmp170=((struct Cyc_Absyn_DatatypeType_struct*)_tmp16D)->f1;_tmp171=
_tmp170.rgn;if(_tmp171 == 0)goto _LLAE;_tmp172=*_tmp171;_tmp173=(void*)_tmp172.v;
_LLAD: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp173);return;_LLAE: if(*((int*)
_tmp16D)!= 3)goto _LLB0;_LLAF: return;_LLB0:;_LLB1: {const char*_tmp45D;void*
_tmp45C;(_tmp45C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp45D="check_pat_regions: bad datatype type",_tag_dyneither(_tmp45D,sizeof(
char),37))),_tag_dyneither(_tmp45C,sizeof(void*),0)));}_LLA9:;}_LL9C: if(*((int*)
_tmp159)!= 5)goto _LL9E;_tmp15C=((struct Cyc_Absyn_Aggr_p_struct*)_tmp159)->f3;
_LL9D: for(0;_tmp15C != 0;_tmp15C=_tmp15C->tl){Cyc_Tcpat_check_pat_regions(te,(*((
struct _tuple14*)_tmp15C->hd)).f2);}return;_LL9E: if(*((int*)_tmp159)!= 3)goto
_LLA0;_tmp15D=((struct Cyc_Absyn_Tuple_p_struct*)_tmp159)->f1;_LL9F: for(0;_tmp15D
!= 0;_tmp15D=_tmp15D->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp15D->hd);}return;_LLA0:;_LLA1: return;_LL97:;}struct _union_Name_value_Name_v{
int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};
union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct
_union_Name_value_Int_v Int_v;};union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct
_dyneither_ptr s);union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){
union Cyc_Tcpat_Name_value _tmp45E;return((_tmp45E.Name_v).val=s,(((_tmp45E.Name_v).tag=
1,_tmp45E)));}union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i);union Cyc_Tcpat_Name_value
Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _tmp45F;return((_tmp45F.Int_v).val=
i,(((_tmp45F.Int_v).tag=2,_tmp45F)));}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value
name;int arity;int*span;struct Cyc_Absyn_Pat*orig_pat;};struct Cyc_Tcpat_Con_struct{
int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static int Cyc_Tcpat_compare_con(
struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2);static int Cyc_Tcpat_compare_con(
struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){union Cyc_Tcpat_Name_value
_tmp178=c1->name;struct _dyneither_ptr _tmp179;int _tmp17A;_LLB3: if((_tmp178.Name_v).tag
!= 1)goto _LLB5;_tmp179=(struct _dyneither_ptr)(_tmp178.Name_v).val;_LLB4: {union
Cyc_Tcpat_Name_value _tmp17B=c2->name;struct _dyneither_ptr _tmp17C;int _tmp17D;
_LLB8: if((_tmp17B.Name_v).tag != 1)goto _LLBA;_tmp17C=(struct _dyneither_ptr)(
_tmp17B.Name_v).val;_LLB9: return Cyc_strcmp((struct _dyneither_ptr)_tmp179,(struct
_dyneither_ptr)_tmp17C);_LLBA: if((_tmp17B.Int_v).tag != 2)goto _LLB7;_tmp17D=(int)(
_tmp17B.Int_v).val;_LLBB: return - 1;_LLB7:;}_LLB5: if((_tmp178.Int_v).tag != 2)goto
_LLB2;_tmp17A=(int)(_tmp178.Int_v).val;_LLB6: {union Cyc_Tcpat_Name_value _tmp17E=
c2->name;struct _dyneither_ptr _tmp17F;int _tmp180;_LLBD: if((_tmp17E.Name_v).tag != 
1)goto _LLBF;_tmp17F=(struct _dyneither_ptr)(_tmp17E.Name_v).val;_LLBE: return 1;
_LLBF: if((_tmp17E.Int_v).tag != 2)goto _LLBC;_tmp180=(int)(_tmp17E.Int_v).val;
_LLC0: return _tmp17A - _tmp180;_LLBC:;}_LLB2:;}static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(
struct _RegionHandle*r);static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct
_RegionHandle*r){return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(
struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
static int Cyc_Tcpat_one_opt=1;static int Cyc_Tcpat_two_opt=2;static int Cyc_Tcpat_twofiftysix_opt=
256;static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct
Cyc_Absyn_Pat*p);static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct
_RegionHandle*r,struct Cyc_Absyn_Pat*p){const char*_tmp462;struct Cyc_Tcpat_Con_s*
_tmp461;return(_tmp461=_region_malloc(r,sizeof(*_tmp461)),((_tmp461->name=Cyc_Tcpat_Name_v(((
_tmp462="NULL",_tag_dyneither(_tmp462,sizeof(char),5)))),((_tmp461->arity=0,((
_tmp461->span=(int*)& Cyc_Tcpat_two_opt,((_tmp461->orig_pat=(struct Cyc_Absyn_Pat*)
p,_tmp461)))))))));}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct
_RegionHandle*r,struct Cyc_Absyn_Pat*p);static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(
struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){const char*_tmp465;struct Cyc_Tcpat_Con_s*
_tmp464;return(_tmp464=_region_malloc(r,sizeof(*_tmp464)),((_tmp464->name=Cyc_Tcpat_Name_v(((
_tmp465="&",_tag_dyneither(_tmp465,sizeof(char),2)))),((_tmp464->arity=1,((
_tmp464->span=(int*)& Cyc_Tcpat_two_opt,((_tmp464->orig_pat=(struct Cyc_Absyn_Pat*)
p,_tmp464)))))))));}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct
_RegionHandle*r,struct Cyc_Absyn_Pat*p);static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(
struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){const char*_tmp468;struct Cyc_Tcpat_Con_s*
_tmp467;return(_tmp467=_region_malloc(r,sizeof(*_tmp467)),((_tmp467->name=Cyc_Tcpat_Name_v(((
_tmp468="&",_tag_dyneither(_tmp468,sizeof(char),2)))),((_tmp467->arity=1,((
_tmp467->span=(int*)& Cyc_Tcpat_one_opt,((_tmp467->orig_pat=(struct Cyc_Absyn_Pat*)
p,_tmp467)))))))));}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct
_RegionHandle*r,int i,struct Cyc_Absyn_Pat*p);static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(
struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*_tmp469;
return(_tmp469=_region_malloc(r,sizeof(*_tmp469)),((_tmp469->name=Cyc_Tcpat_Int_v(
i),((_tmp469->arity=0,((_tmp469->span=0,((_tmp469->orig_pat=p,_tmp469)))))))));}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _RegionHandle*r,struct
_dyneither_ptr f,struct Cyc_Absyn_Pat*p);static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(
struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*
_tmp46A;return(_tmp46A=_region_malloc(r,sizeof(*_tmp46A)),((_tmp46A->name=Cyc_Tcpat_Name_v(
f),((_tmp46A->arity=0,((_tmp46A->span=0,((_tmp46A->orig_pat=(struct Cyc_Absyn_Pat*)
p,_tmp46A)))))))));}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(struct
_RegionHandle*r,char c,struct Cyc_Absyn_Pat*p);static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(
struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*_tmp46B;
return(_tmp46B=_region_malloc(r,sizeof(*_tmp46B)),((_tmp46B->name=Cyc_Tcpat_Int_v((
int)c),((_tmp46B->arity=0,((_tmp46B->span=(int*)& Cyc_Tcpat_twofiftysix_opt,((
_tmp46B->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp46B)))))))));}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p);static
struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*
p){const char*_tmp46E;struct Cyc_Tcpat_Con_s*_tmp46D;return(_tmp46D=_region_malloc(
r,sizeof(*_tmp46D)),((_tmp46D->name=Cyc_Tcpat_Name_v(((_tmp46E="$",
_tag_dyneither(_tmp46E,sizeof(char),2)))),((_tmp46D->arity=i,((_tmp46D->span=(
int*)& Cyc_Tcpat_one_opt,((_tmp46D->orig_pat=p,_tmp46D)))))))));}static void*Cyc_Tcpat_null_pat(
struct _RegionHandle*r,struct Cyc_Absyn_Pat*p);static void*Cyc_Tcpat_null_pat(
struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct _tmp471;
struct Cyc_Tcpat_Con_struct*_tmp470;return(void*)((_tmp470=_region_malloc(r,
sizeof(*_tmp470)),((_tmp470[0]=((_tmp471.tag=0,((_tmp471.f1=Cyc_Tcpat_null_con(r,
p),((_tmp471.f2=0,_tmp471)))))),_tmp470))));}static void*Cyc_Tcpat_int_pat(struct
_RegionHandle*r,int i,struct Cyc_Absyn_Pat*p);static void*Cyc_Tcpat_int_pat(struct
_RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct _tmp474;
struct Cyc_Tcpat_Con_struct*_tmp473;return(void*)((_tmp473=_region_malloc(r,
sizeof(*_tmp473)),((_tmp473[0]=((_tmp474.tag=0,((_tmp474.f1=Cyc_Tcpat_int_con(r,
i,p),((_tmp474.f2=0,_tmp474)))))),_tmp473))));}static void*Cyc_Tcpat_char_pat(
struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p);static void*Cyc_Tcpat_char_pat(
struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct
_tmp477;struct Cyc_Tcpat_Con_struct*_tmp476;return(void*)((_tmp476=_region_malloc(
r,sizeof(*_tmp476)),((_tmp476[0]=((_tmp477.tag=0,((_tmp477.f1=Cyc_Tcpat_char_con(
r,c,p),((_tmp477.f2=0,_tmp477)))))),_tmp476))));}static void*Cyc_Tcpat_float_pat(
struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p);static void*
Cyc_Tcpat_float_pat(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*
p){struct Cyc_Tcpat_Con_struct _tmp47A;struct Cyc_Tcpat_Con_struct*_tmp479;return(
void*)((_tmp479=_region_malloc(r,sizeof(*_tmp479)),((_tmp479[0]=((_tmp47A.tag=0,((
_tmp47A.f1=Cyc_Tcpat_float_con(r,f,p),((_tmp47A.f2=0,_tmp47A)))))),_tmp479))));}
static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*
p0);static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*
p0){struct Cyc_Tcpat_Con_struct _tmp480;struct Cyc_List_List*_tmp47F;struct Cyc_Tcpat_Con_struct*
_tmp47E;return(void*)((_tmp47E=_region_malloc(r,sizeof(*_tmp47E)),((_tmp47E[0]=((
_tmp480.tag=0,((_tmp480.f1=Cyc_Tcpat_null_ptr_con(r,p0),((_tmp480.f2=((_tmp47F=
_region_malloc(r,sizeof(*_tmp47F)),((_tmp47F->hd=(void*)p,((_tmp47F->tl=0,
_tmp47F)))))),_tmp480)))))),_tmp47E))));}static void*Cyc_Tcpat_ptr_pat(struct
_RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0);static void*Cyc_Tcpat_ptr_pat(
struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){struct Cyc_Tcpat_Con_struct
_tmp486;struct Cyc_List_List*_tmp485;struct Cyc_Tcpat_Con_struct*_tmp484;return(
void*)((_tmp484=_region_malloc(r,sizeof(*_tmp484)),((_tmp484[0]=((_tmp486.tag=0,((
_tmp486.f1=Cyc_Tcpat_ptr_con(r,p0),((_tmp486.f2=((_tmp485=_region_malloc(r,
sizeof(*_tmp485)),((_tmp485->hd=(void*)p,((_tmp485->tl=0,_tmp485)))))),_tmp486)))))),
_tmp484))));}static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,struct Cyc_List_List*
ss,struct Cyc_Absyn_Pat*p);static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,
struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct _tmp489;
struct Cyc_Tcpat_Con_struct*_tmp488;return(void*)((_tmp488=_region_malloc(r,
sizeof(*_tmp488)),((_tmp488[0]=((_tmp489.tag=0,((_tmp489.f1=Cyc_Tcpat_tuple_con(
r,Cyc_List_length(ss),p),((_tmp489.f2=ss,_tmp489)))))),_tmp488))));}static void*
Cyc_Tcpat_con_pat(struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,
struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p);static void*Cyc_Tcpat_con_pat(
struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*
ps,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*_tmp48A;struct Cyc_Tcpat_Con_s*c=(
_tmp48A=_region_malloc(r,sizeof(*_tmp48A)),((_tmp48A->name=Cyc_Tcpat_Name_v(
con_name),((_tmp48A->arity=Cyc_List_length(ps),((_tmp48A->span=span,((_tmp48A->orig_pat=(
struct Cyc_Absyn_Pat*)p,_tmp48A)))))))));struct Cyc_Tcpat_Con_struct _tmp48D;struct
Cyc_Tcpat_Con_struct*_tmp48C;return(void*)((_tmp48C=_region_malloc(r,sizeof(*
_tmp48C)),((_tmp48C[0]=((_tmp48D.tag=0,((_tmp48D.f1=c,((_tmp48D.f2=ps,_tmp48D)))))),
_tmp48C))));}static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*
p);static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
void*s;{void*_tmp19F=p->r;void*_tmp1A0;int _tmp1A1;char _tmp1A2;struct
_dyneither_ptr _tmp1A3;struct Cyc_Absyn_Pat*_tmp1A4;struct Cyc_Absyn_Pat*_tmp1A5;
struct Cyc_Absyn_Pat*_tmp1A6;struct Cyc_Absyn_Datatypedecl*_tmp1A7;struct Cyc_Absyn_Datatypefield*
_tmp1A8;struct Cyc_List_List*_tmp1A9;struct Cyc_List_List*_tmp1AA;struct Cyc_Absyn_AggrInfo*
_tmp1AB;struct Cyc_Absyn_AggrInfo _tmp1AC;union Cyc_Absyn_AggrInfoU _tmp1AD;struct
Cyc_Absyn_Aggrdecl**_tmp1AE;struct Cyc_Absyn_Aggrdecl*_tmp1AF;struct Cyc_List_List*
_tmp1B0;struct Cyc_Absyn_Enumdecl*_tmp1B1;struct Cyc_Absyn_Enumfield*_tmp1B2;void*
_tmp1B3;struct Cyc_Absyn_Enumfield*_tmp1B4;_LLC2: if((int)_tmp19F != 0)goto _LLC4;
_LLC3: goto _LLC5;_LLC4: if(_tmp19F <= (void*)2)goto _LLC6;if(*((int*)_tmp19F)!= 2)
goto _LLC6;_LLC5: s=(void*)0;goto _LLC1;_LLC6: if((int)_tmp19F != 1)goto _LLC8;_LLC7: s=
Cyc_Tcpat_null_pat(r,p);goto _LLC1;_LLC8: if(_tmp19F <= (void*)2)goto _LLCA;if(*((
int*)_tmp19F)!= 7)goto _LLCA;_tmp1A0=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp19F)->f1;_tmp1A1=((struct Cyc_Absyn_Int_p_struct*)_tmp19F)->f2;_LLC9: s=Cyc_Tcpat_int_pat(
r,_tmp1A1,(struct Cyc_Absyn_Pat*)p);goto _LLC1;_LLCA: if(_tmp19F <= (void*)2)goto
_LLCC;if(*((int*)_tmp19F)!= 8)goto _LLCC;_tmp1A2=((struct Cyc_Absyn_Char_p_struct*)
_tmp19F)->f1;_LLCB: s=Cyc_Tcpat_char_pat(r,_tmp1A2,p);goto _LLC1;_LLCC: if(_tmp19F
<= (void*)2)goto _LLCE;if(*((int*)_tmp19F)!= 9)goto _LLCE;_tmp1A3=((struct Cyc_Absyn_Float_p_struct*)
_tmp19F)->f1;_LLCD: s=Cyc_Tcpat_float_pat(r,_tmp1A3,p);goto _LLC1;_LLCE: if(_tmp19F
<= (void*)2)goto _LLD0;if(*((int*)_tmp19F)!= 0)goto _LLD0;_tmp1A4=((struct Cyc_Absyn_Var_p_struct*)
_tmp19F)->f2;_LLCF: s=Cyc_Tcpat_compile_pat(r,_tmp1A4);goto _LLC1;_LLD0: if(_tmp19F
<= (void*)2)goto _LLD2;if(*((int*)_tmp19F)!= 1)goto _LLD2;_tmp1A5=((struct Cyc_Absyn_Reference_p_struct*)
_tmp19F)->f2;_LLD1: s=Cyc_Tcpat_compile_pat(r,_tmp1A5);goto _LLC1;_LLD2: if(_tmp19F
<= (void*)2)goto _LLD4;if(*((int*)_tmp19F)!= 4)goto _LLD4;_tmp1A6=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp19F)->f1;_LLD3:{void*_tmp1B5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1B6;struct Cyc_Absyn_PtrAtts
_tmp1B7;union Cyc_Absyn_Constraint*_tmp1B8;struct Cyc_Absyn_DatatypeInfo _tmp1B9;
struct Cyc_Core_Opt*_tmp1BA;struct Cyc_Core_Opt _tmp1BB;_LLE7: if(_tmp1B5 <= (void*)4)
goto _LLEB;if(*((int*)_tmp1B5)!= 4)goto _LLE9;_tmp1B6=((struct Cyc_Absyn_PointerType_struct*)
_tmp1B5)->f1;_tmp1B7=_tmp1B6.ptr_atts;_tmp1B8=_tmp1B7.nullable;_LLE8: {int
is_nullable=0;int still_working=1;while(still_working){union Cyc_Absyn_Constraint*
_tmp1BC=_tmp1B8;union Cyc_Absyn_Constraint _tmp1BD;union Cyc_Absyn_Constraint*
_tmp1BE;union Cyc_Absyn_Constraint _tmp1BF;int _tmp1C0;union Cyc_Absyn_Constraint
_tmp1C1;int _tmp1C2;_LLEE: _tmp1BD=*_tmp1BC;if((_tmp1BD.Forward_constr).tag != 2)
goto _LLF0;_tmp1BE=(union Cyc_Absyn_Constraint*)(_tmp1BD.Forward_constr).val;_LLEF:*
_tmp1B8=*_tmp1BE;continue;_LLF0: _tmp1BF=*_tmp1BC;if((_tmp1BF.No_constr).tag != 3)
goto _LLF2;_tmp1C0=(int)(_tmp1BF.No_constr).val;_LLF1:{struct
_union_Constraint_Eq_constr*_tmp48E;(_tmp48E=& _tmp1B8->Eq_constr,((_tmp48E->tag=
1,_tmp48E->val=0)));}is_nullable=0;still_working=0;goto _LLED;_LLF2: _tmp1C1=*
_tmp1BC;if((_tmp1C1.Eq_constr).tag != 1)goto _LLED;_tmp1C2=(int)(_tmp1C1.Eq_constr).val;
_LLF3: is_nullable=(int)_tmp1C2;still_working=0;goto _LLED;_LLED:;}{void*ss=Cyc_Tcpat_compile_pat(
r,_tmp1A6);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{s=Cyc_Tcpat_ptr_pat(
r,ss,p);}goto _LLE6;}}_LLE9: if(*((int*)_tmp1B5)!= 2)goto _LLEB;_tmp1B9=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp1B5)->f1;_tmp1BA=_tmp1B9.rgn;if(_tmp1BA == 0)goto _LLEB;_tmp1BB=*_tmp1BA;_LLEA:{
void*_tmp1C4=_tmp1A6->r;struct Cyc_Absyn_Datatypedecl*_tmp1C5;struct Cyc_Absyn_Datatypefield*
_tmp1C6;struct Cyc_List_List*_tmp1C7;_LLF5: if(_tmp1C4 <= (void*)2)goto _LLF7;if(*((
int*)_tmp1C4)!= 6)goto _LLF7;_tmp1C5=((struct Cyc_Absyn_Datatype_p_struct*)_tmp1C4)->f1;
_tmp1C6=((struct Cyc_Absyn_Datatype_p_struct*)_tmp1C4)->f2;_tmp1C7=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp1C4)->f3;_LLF6: {int*span;if(_tmp1C5->is_extensible)span=0;else{int*_tmp48F;
span=((_tmp48F=_region_malloc(r,sizeof(*_tmp48F)),((_tmp48F[0]=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1C5->fields))->v),_tmp48F))));}s=Cyc_Tcpat_con_pat(r,*(*_tmp1C6->name).f2,
span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1C7),p);goto _LLF4;}_LLF7:;
_LLF8: {const char*_tmp492;void*_tmp491;(_tmp491=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp492="non-datatype pattern has datatype type",
_tag_dyneither(_tmp492,sizeof(char),39))),_tag_dyneither(_tmp491,sizeof(void*),0)));}
_LLF4:;}goto _LLE6;_LLEB:;_LLEC: {const char*_tmp495;void*_tmp494;(_tmp494=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp495="pointer pattern does not have pointer type",
_tag_dyneither(_tmp495,sizeof(char),43))),_tag_dyneither(_tmp494,sizeof(void*),0)));}
_LLE6:;}goto _LLC1;_LLD4: if(_tmp19F <= (void*)2)goto _LLD6;if(*((int*)_tmp19F)!= 6)
goto _LLD6;_tmp1A7=((struct Cyc_Absyn_Datatype_p_struct*)_tmp19F)->f1;_tmp1A8=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp19F)->f2;_tmp1A9=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp19F)->f3;_LLD5: {int*span;{void*_tmp1CD=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(p->topt))->v);_LLFA: if(_tmp1CD <= (void*)4)goto _LLFE;
if(*((int*)_tmp1CD)!= 2)goto _LLFC;_LLFB: if(_tmp1A7->is_extensible)span=0;else{
int*_tmp496;span=((_tmp496=_region_malloc(r,sizeof(*_tmp496)),((_tmp496[0]=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1A7->fields))->v),_tmp496))));}goto _LLF9;_LLFC: if(*((int*)
_tmp1CD)!= 3)goto _LLFE;_LLFD: span=(int*)& Cyc_Tcpat_one_opt;goto _LLF9;_LLFE:;
_LLFF:{const char*_tmp499;void*_tmp498;span=((_tmp498=0,((int*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp499="void datatype pattern has bad type",
_tag_dyneither(_tmp499,sizeof(char),35))),_tag_dyneither(_tmp498,sizeof(void*),0))));}
goto _LLF9;_LLF9:;}s=Cyc_Tcpat_con_pat(r,*(*_tmp1A8->name).f2,span,((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,
r,_tmp1A9),p);goto _LLC1;}_LLD6: if(_tmp19F <= (void*)2)goto _LLD8;if(*((int*)
_tmp19F)!= 3)goto _LLD8;_tmp1AA=((struct Cyc_Absyn_Tuple_p_struct*)_tmp19F)->f1;
_LLD7: s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1AA),(
struct Cyc_Absyn_Pat*)p);goto _LLC1;_LLD8: if(_tmp19F <= (void*)2)goto _LLDA;if(*((
int*)_tmp19F)!= 5)goto _LLDA;_tmp1AB=((struct Cyc_Absyn_Aggr_p_struct*)_tmp19F)->f1;
if(_tmp1AB == 0)goto _LLDA;_tmp1AC=*_tmp1AB;_tmp1AD=_tmp1AC.aggr_info;if((_tmp1AD.KnownAggr).tag
!= 2)goto _LLDA;_tmp1AE=(struct Cyc_Absyn_Aggrdecl**)(_tmp1AD.KnownAggr).val;
_tmp1AF=*_tmp1AE;_tmp1B0=((struct Cyc_Absyn_Aggr_p_struct*)_tmp19F)->f3;_LLD9: if(
_tmp1AF->kind == (void*)0){struct Cyc_List_List*ps=0;{struct Cyc_List_List*fields=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AF->impl))->fields;for(0;fields != 
0;fields=fields->tl){const char*_tmp49A;int found=Cyc_strcmp((struct _dyneither_ptr)*((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name,((_tmp49A="",_tag_dyneither(_tmp49A,
sizeof(char),1))))== 0;{struct Cyc_List_List*dlps0=_tmp1B0;for(0;!found  && dlps0
!= 0;dlps0=dlps0->tl){struct _tuple14 _tmp1D2;struct Cyc_List_List*_tmp1D3;struct
Cyc_Absyn_Pat*_tmp1D4;struct _tuple14*_tmp1D1=(struct _tuple14*)dlps0->hd;_tmp1D2=*
_tmp1D1;_tmp1D3=_tmp1D2.f1;_tmp1D4=_tmp1D2.f2;{struct Cyc_List_List*_tmp1D5=
_tmp1D3;struct Cyc_List_List _tmp1D6;void*_tmp1D7;struct _dyneither_ptr*_tmp1D8;
struct Cyc_List_List*_tmp1D9;_LL101: if(_tmp1D5 == 0)goto _LL103;_tmp1D6=*_tmp1D5;
_tmp1D7=(void*)_tmp1D6.hd;if(*((int*)_tmp1D7)!= 1)goto _LL103;_tmp1D8=((struct Cyc_Absyn_FieldName_struct*)
_tmp1D7)->f1;_tmp1D9=_tmp1D6.tl;if(_tmp1D9 != 0)goto _LL103;_LL102: if(Cyc_strptrcmp(
_tmp1D8,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){{struct Cyc_List_List*
_tmp49B;ps=((_tmp49B=_region_malloc(r,sizeof(*_tmp49B)),((_tmp49B->hd=(void*)Cyc_Tcpat_compile_pat(
r,_tmp1D4),((_tmp49B->tl=ps,_tmp49B))))));}found=1;}goto _LL100;_LL103:;_LL104: {
const char*_tmp49E;void*_tmp49D;(_tmp49D=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp49E="bad designator(s)",
_tag_dyneither(_tmp49E,sizeof(char),18))),_tag_dyneither(_tmp49D,sizeof(void*),0)));}
_LL100:;}}}if(!found){const char*_tmp4A1;void*_tmp4A0;(_tmp4A0=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4A1="bad designator",
_tag_dyneither(_tmp4A1,sizeof(char),15))),_tag_dyneither(_tmp4A0,sizeof(void*),0)));}}}
s=Cyc_Tcpat_tuple_pat(r,ps,(struct Cyc_Absyn_Pat*)p);}else{if(!((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1AF->impl))->tagged){const char*_tmp4A4;void*_tmp4A3;(_tmp4A3=0,
Cyc_Tcutil_terr(p->loc,((_tmp4A4="patterns on untagged unions not yet supported.",
_tag_dyneither(_tmp4A4,sizeof(char),47))),_tag_dyneither(_tmp4A3,sizeof(void*),0)));}{
int*_tmp4A5;int*span=(_tmp4A5=_region_malloc(r,sizeof(*_tmp4A5)),((_tmp4A5[0]=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1AF->impl))->fields),_tmp4A5)));int field_name;struct Cyc_List_List*
_tmp1E2=_tmp1B0;struct Cyc_List_List _tmp1E3;struct _tuple14*_tmp1E4;struct _tuple14
_tmp1E5;struct Cyc_List_List*_tmp1E6;struct Cyc_List_List _tmp1E7;void*_tmp1E8;
struct _dyneither_ptr*_tmp1E9;struct Cyc_List_List*_tmp1EA;struct Cyc_Absyn_Pat*
_tmp1EB;struct Cyc_List_List*_tmp1EC;_LL106: if(_tmp1E2 == 0)goto _LL108;_tmp1E3=*
_tmp1E2;_tmp1E4=(struct _tuple14*)_tmp1E3.hd;_tmp1E5=*_tmp1E4;_tmp1E6=_tmp1E5.f1;
if(_tmp1E6 == 0)goto _LL108;_tmp1E7=*_tmp1E6;_tmp1E8=(void*)_tmp1E7.hd;if(*((int*)
_tmp1E8)!= 1)goto _LL108;_tmp1E9=((struct Cyc_Absyn_FieldName_struct*)_tmp1E8)->f1;
_tmp1EA=_tmp1E7.tl;if(_tmp1EA != 0)goto _LL108;_tmp1EB=_tmp1E5.f2;_tmp1EC=_tmp1E3.tl;
if(_tmp1EC != 0)goto _LL108;_LL107:{struct Cyc_List_List*_tmp4A6;s=Cyc_Tcpat_con_pat(
r,*_tmp1E9,span,((_tmp4A6=_region_malloc(r,sizeof(*_tmp4A6)),((_tmp4A6->hd=(void*)
Cyc_Tcpat_compile_pat(r,_tmp1EB),((_tmp4A6->tl=0,_tmp4A6)))))),p);}goto _LL105;
_LL108:;_LL109: {const char*_tmp4A9;void*_tmp4A8;(_tmp4A8=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4A9="bad union pattern",
_tag_dyneither(_tmp4A9,sizeof(char),18))),_tag_dyneither(_tmp4A8,sizeof(void*),0)));}
_LL105:;}}goto _LLC1;_LLDA: if(_tmp19F <= (void*)2)goto _LLDC;if(*((int*)_tmp19F)!= 
10)goto _LLDC;_tmp1B1=((struct Cyc_Absyn_Enum_p_struct*)_tmp19F)->f1;_tmp1B2=((
struct Cyc_Absyn_Enum_p_struct*)_tmp19F)->f2;_LLDB: {int span=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1B1->fields))->v);{int*_tmp4AA;s=Cyc_Tcpat_con_pat(r,*(*_tmp1B2->name).f2,((
_tmp4AA=_region_malloc(r,sizeof(*_tmp4AA)),((_tmp4AA[0]=span,_tmp4AA)))),0,p);}
goto _LLC1;}_LLDC: if(_tmp19F <= (void*)2)goto _LLDE;if(*((int*)_tmp19F)!= 11)goto
_LLDE;_tmp1B3=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp19F)->f1;_tmp1B4=((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp19F)->f2;_LLDD: {struct Cyc_List_List*
fields;{void*_tmp1F2=Cyc_Tcutil_compress(_tmp1B3);struct Cyc_List_List*_tmp1F3;
_LL10B: if(_tmp1F2 <= (void*)4)goto _LL10D;if(*((int*)_tmp1F2)!= 13)goto _LL10D;
_tmp1F3=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp1F2)->f1;_LL10C: fields=
_tmp1F3;goto _LL10A;_LL10D:;_LL10E: {const char*_tmp4AD;void*_tmp4AC;(_tmp4AC=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp4AD="bad type in AnonEnum_p",_tag_dyneither(_tmp4AD,sizeof(char),23))),
_tag_dyneither(_tmp4AC,sizeof(void*),0)));}_LL10A:;}{int span=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(fields);{int*_tmp4AE;s=Cyc_Tcpat_con_pat(r,*(*_tmp1B4->name).f2,((
_tmp4AE=_region_malloc(r,sizeof(*_tmp4AE)),((_tmp4AE[0]=span,_tmp4AE)))),0,p);}
goto _LLC1;}}_LLDE: if(_tmp19F <= (void*)2)goto _LLE0;if(*((int*)_tmp19F)!= 12)goto
_LLE0;_LLDF: goto _LLE1;_LLE0: if(_tmp19F <= (void*)2)goto _LLE2;if(*((int*)_tmp19F)
!= 13)goto _LLE2;_LLE1: goto _LLE3;_LLE2: if(_tmp19F <= (void*)2)goto _LLE4;if(*((int*)
_tmp19F)!= 5)goto _LLE4;_LLE3: goto _LLE5;_LLE4: if(_tmp19F <= (void*)2)goto _LLC1;if(*((
int*)_tmp19F)!= 14)goto _LLC1;_LLE5: s=(void*)0;_LLC1:;}return s;}struct Cyc_Tcpat_Pos_struct{
int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{
int tag;struct Cyc_Set_Set*f1;};struct Cyc_Tcpat_Failure_struct{int tag;void*f1;};
struct Cyc_Tcpat_Success_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{int
tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct
_tuple17{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*
c);static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*
c){void*_tmp1F7=td;struct Cyc_Set_Set*_tmp1F8;_LL110: if(*((int*)_tmp1F7)!= 1)goto
_LL112;_tmp1F8=((struct Cyc_Tcpat_Neg_struct*)_tmp1F7)->f1;_LL111: {struct Cyc_Tcpat_Neg_struct
_tmp4B1;struct Cyc_Tcpat_Neg_struct*_tmp4B0;return(void*)((_tmp4B0=_region_malloc(
r,sizeof(*_tmp4B0)),((_tmp4B0[0]=((_tmp4B1.tag=1,((_tmp4B1.f1=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(
r,_tmp1F8,c),_tmp4B1)))),_tmp4B0))));}_LL112: if(*((int*)_tmp1F7)!= 0)goto _LL10F;
_LL113: {const char*_tmp4B4;void*_tmp4B3;(_tmp4B3=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4B4="add_neg called when td is Positive",
_tag_dyneither(_tmp4B4,sizeof(char),35))),_tag_dyneither(_tmp4B3,sizeof(void*),0)));}
_LL10F:;}static void*Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td);
static void*Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){void*_tmp1FD=
td;struct Cyc_Tcpat_Con_s*_tmp1FE;struct Cyc_Set_Set*_tmp1FF;_LL115: if(*((int*)
_tmp1FD)!= 0)goto _LL117;_tmp1FE=((struct Cyc_Tcpat_Pos_struct*)_tmp1FD)->f1;
_LL116: if(Cyc_Tcpat_compare_con(c,_tmp1FE)== 0)return(void*)0;else{return(void*)
1;}_LL117: if(*((int*)_tmp1FD)!= 1)goto _LL114;_tmp1FF=((struct Cyc_Tcpat_Neg_struct*)
_tmp1FD)->f1;_LL118: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp1FF,c))return(void*)1;else{if(c->span != 0  && *((int*)
_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp1FF)+ 1)return(void*)0;else{return(void*)2;}}_LL114:;}struct _tuple18{struct
Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*Cyc_Tcpat_augment(
struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc);static struct Cyc_List_List*
Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp200=ctxt;struct Cyc_List_List _tmp201;struct _tuple18*
_tmp202;struct _tuple18 _tmp203;struct Cyc_Tcpat_Con_s*_tmp204;struct Cyc_List_List*
_tmp205;struct Cyc_List_List*_tmp206;_LL11A: if(_tmp200 != 0)goto _LL11C;_LL11B:
return 0;_LL11C: if(_tmp200 == 0)goto _LL119;_tmp201=*_tmp200;_tmp202=(struct
_tuple18*)_tmp201.hd;_tmp203=*_tmp202;_tmp204=_tmp203.f1;_tmp205=_tmp203.f2;
_tmp206=_tmp201.tl;_LL11D: {struct _tuple18*_tmp4BA;struct Cyc_List_List*_tmp4B9;
struct Cyc_List_List*_tmp4B8;return(_tmp4B8=_region_malloc(r,sizeof(*_tmp4B8)),((
_tmp4B8->hd=((_tmp4BA=_region_malloc(r,sizeof(*_tmp4BA)),((_tmp4BA->f1=_tmp204,((
_tmp4BA->f2=(struct Cyc_List_List*)((_tmp4B9=_region_malloc(r,sizeof(*_tmp4B9)),((
_tmp4B9->hd=(void*)dsc,((_tmp4B9->tl=_tmp205,_tmp4B9)))))),_tmp4BA)))))),((
_tmp4B8->tl=_tmp206,_tmp4B8)))));}_LL119:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt);static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp20A=ctxt;
struct Cyc_List_List _tmp20B;struct _tuple18*_tmp20C;struct _tuple18 _tmp20D;struct
Cyc_Tcpat_Con_s*_tmp20E;struct Cyc_List_List*_tmp20F;struct Cyc_List_List*_tmp210;
_LL11F: if(_tmp20A != 0)goto _LL121;_LL120: {const char*_tmp4BD;void*_tmp4BC;(
_tmp4BC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp4BD="norm_context: empty context",_tag_dyneither(_tmp4BD,sizeof(char),28))),
_tag_dyneither(_tmp4BC,sizeof(void*),0)));}_LL121: if(_tmp20A == 0)goto _LL11E;
_tmp20B=*_tmp20A;_tmp20C=(struct _tuple18*)_tmp20B.hd;_tmp20D=*_tmp20C;_tmp20E=
_tmp20D.f1;_tmp20F=_tmp20D.f2;_tmp210=_tmp20B.tl;_LL122: {struct Cyc_Tcpat_Pos_struct
_tmp4C0;struct Cyc_Tcpat_Pos_struct*_tmp4BF;return Cyc_Tcpat_augment(r,_tmp210,(
void*)((_tmp4BF=_region_malloc(r,sizeof(*_tmp4BF)),((_tmp4BF[0]=((_tmp4C0.tag=0,((
_tmp4C0.f1=_tmp20E,((_tmp4C0.f2=Cyc_List_rrev(r,_tmp20F),_tmp4C0)))))),_tmp4BF)))));}
_LL11E:;}static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*
ctxt,void*dsc,struct Cyc_List_List*work);static void*Cyc_Tcpat_build_desc(struct
_RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
struct _tuple12 _tmp4C1;struct _tuple12 _tmp216=(_tmp4C1.f1=ctxt,((_tmp4C1.f2=work,
_tmp4C1)));struct Cyc_List_List*_tmp217;struct Cyc_List_List*_tmp218;struct Cyc_List_List*
_tmp219;struct Cyc_List_List*_tmp21A;struct Cyc_List_List*_tmp21B;struct Cyc_List_List
_tmp21C;struct _tuple18*_tmp21D;struct _tuple18 _tmp21E;struct Cyc_Tcpat_Con_s*
_tmp21F;struct Cyc_List_List*_tmp220;struct Cyc_List_List*_tmp221;struct Cyc_List_List*
_tmp222;struct Cyc_List_List _tmp223;struct _tuple17*_tmp224;struct _tuple17 _tmp225;
struct Cyc_List_List*_tmp226;struct Cyc_List_List*_tmp227;_LL124: _tmp217=_tmp216.f1;
if(_tmp217 != 0)goto _LL126;_tmp218=_tmp216.f2;if(_tmp218 != 0)goto _LL126;_LL125:
return dsc;_LL126: _tmp219=_tmp216.f1;if(_tmp219 != 0)goto _LL128;_LL127: goto _LL129;
_LL128: _tmp21A=_tmp216.f2;if(_tmp21A != 0)goto _LL12A;_LL129: {const char*_tmp4C4;
void*_tmp4C3;(_tmp4C3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp4C4="build_desc: ctxt and work don't match",
_tag_dyneither(_tmp4C4,sizeof(char),38))),_tag_dyneither(_tmp4C3,sizeof(void*),0)));}
_LL12A: _tmp21B=_tmp216.f1;if(_tmp21B == 0)goto _LL123;_tmp21C=*_tmp21B;_tmp21D=(
struct _tuple18*)_tmp21C.hd;_tmp21E=*_tmp21D;_tmp21F=_tmp21E.f1;_tmp220=_tmp21E.f2;
_tmp221=_tmp21C.tl;_tmp222=_tmp216.f2;if(_tmp222 == 0)goto _LL123;_tmp223=*_tmp222;
_tmp224=(struct _tuple17*)_tmp223.hd;_tmp225=*_tmp224;_tmp226=_tmp225.f3;_tmp227=
_tmp223.tl;_LL12B: {struct Cyc_Tcpat_Pos_struct _tmp4CA;struct Cyc_List_List*
_tmp4C9;struct Cyc_Tcpat_Pos_struct*_tmp4C8;struct Cyc_Tcpat_Pos_struct*_tmp22A=(
_tmp4C8=_region_malloc(r,sizeof(*_tmp4C8)),((_tmp4C8[0]=((_tmp4CA.tag=0,((
_tmp4CA.f1=_tmp21F,((_tmp4CA.f2=Cyc_List_rappend(r,Cyc_List_rrev(r,_tmp220),((
_tmp4C9=_region_malloc(r,sizeof(*_tmp4C9)),((_tmp4C9->hd=(void*)dsc,((_tmp4C9->tl=
_tmp226,_tmp4C9))))))),_tmp4CA)))))),_tmp4C8)));return Cyc_Tcpat_build_desc(r,
_tmp221,(void*)_tmp22A,_tmp227);}_LL123:;}static void*Cyc_Tcpat_match(struct
_RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,
struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules);static
void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*
allmrules);static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct
Cyc_List_List*allmrules){struct Cyc_List_List*_tmp22E=allmrules;struct Cyc_List_List
_tmp22F;struct _tuple0*_tmp230;struct _tuple0 _tmp231;void*_tmp232;void*_tmp233;
struct Cyc_List_List*_tmp234;_LL12D: if(_tmp22E != 0)goto _LL12F;_LL12E: {struct Cyc_Tcpat_Failure_struct
_tmp4CD;struct Cyc_Tcpat_Failure_struct*_tmp4CC;return(void*)((_tmp4CC=_cycalloc(
sizeof(*_tmp4CC)),((_tmp4CC[0]=((_tmp4CD.tag=0,((_tmp4CD.f1=(void*)dsc,_tmp4CD)))),
_tmp4CC))));}_LL12F: if(_tmp22E == 0)goto _LL12C;_tmp22F=*_tmp22E;_tmp230=(struct
_tuple0*)_tmp22F.hd;_tmp231=*_tmp230;_tmp232=_tmp231.f1;_tmp233=_tmp231.f2;
_tmp234=_tmp22F.tl;_LL130: return Cyc_Tcpat_match(r,_tmp232,0,dsc,0,0,_tmp233,
_tmp234);_LL12C:;}static void*Cyc_Tcpat_match_compile(struct _RegionHandle*r,
struct Cyc_List_List*allmrules);static void*Cyc_Tcpat_match_compile(struct
_RegionHandle*r,struct Cyc_List_List*allmrules){struct Cyc_Tcpat_Neg_struct _tmp4D0;
struct Cyc_Tcpat_Neg_struct*_tmp4CF;return Cyc_Tcpat_or_match(r,(void*)((_tmp4CF=
_region_malloc(r,sizeof(*_tmp4CF)),((_tmp4CF[0]=((_tmp4D0.tag=1,((_tmp4D0.f1=Cyc_Tcpat_empty_con_set(
r),_tmp4D0)))),_tmp4CF)))),allmrules);}static void*Cyc_Tcpat_and_match(struct
_RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules);static void*Cyc_Tcpat_and_match(struct
_RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp239=work;
struct Cyc_List_List _tmp23A;struct _tuple17*_tmp23B;struct _tuple17 _tmp23C;struct
Cyc_List_List*_tmp23D;struct Cyc_List_List*_tmp23E;struct Cyc_List_List*_tmp23F;
struct Cyc_List_List*_tmp240;struct Cyc_List_List _tmp241;struct _tuple17*_tmp242;
struct _tuple17 _tmp243;struct Cyc_List_List*_tmp244;struct Cyc_List_List*_tmp245;
struct Cyc_List_List*_tmp246;struct Cyc_List_List*_tmp247;_LL132: if(_tmp239 != 0)
goto _LL134;_LL133: {struct Cyc_Tcpat_Success_struct _tmp4D3;struct Cyc_Tcpat_Success_struct*
_tmp4D2;return(void*)((_tmp4D2=_region_malloc(r,sizeof(*_tmp4D2)),((_tmp4D2[0]=((
_tmp4D3.tag=1,((_tmp4D3.f1=(void*)right_hand_side,_tmp4D3)))),_tmp4D2))));}
_LL134: if(_tmp239 == 0)goto _LL136;_tmp23A=*_tmp239;_tmp23B=(struct _tuple17*)
_tmp23A.hd;_tmp23C=*_tmp23B;_tmp23D=_tmp23C.f1;if(_tmp23D != 0)goto _LL136;_tmp23E=
_tmp23C.f2;if(_tmp23E != 0)goto _LL136;_tmp23F=_tmp23C.f3;if(_tmp23F != 0)goto
_LL136;_tmp240=_tmp23A.tl;_LL135: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp240,right_hand_side,rules);_LL136: if(_tmp239 == 0)goto _LL131;_tmp241=*
_tmp239;_tmp242=(struct _tuple17*)_tmp241.hd;_tmp243=*_tmp242;_tmp244=_tmp243.f1;
_tmp245=_tmp243.f2;_tmp246=_tmp243.f3;_tmp247=_tmp241.tl;_LL137: if((_tmp244 == 0
 || _tmp245 == 0) || _tmp246 == 0){const char*_tmp4D6;void*_tmp4D5;(_tmp4D5=0,Cyc_Tcutil_impos(((
_tmp4D6="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp4D6,sizeof(
char),38))),_tag_dyneither(_tmp4D5,sizeof(void*),0)));}{struct Cyc_List_List
_tmp24D;void*_tmp24E;struct Cyc_List_List*_tmp24F;struct Cyc_List_List*_tmp24C=(
struct Cyc_List_List*)_tmp244;_tmp24D=*_tmp24C;_tmp24E=(void*)_tmp24D.hd;_tmp24F=
_tmp24D.tl;{struct Cyc_List_List _tmp251;struct Cyc_List_List*_tmp252;struct Cyc_List_List*
_tmp253;struct Cyc_List_List*_tmp250=(struct Cyc_List_List*)_tmp245;_tmp251=*
_tmp250;_tmp252=(struct Cyc_List_List*)_tmp251.hd;_tmp253=_tmp251.tl;{struct Cyc_List_List
_tmp255;void*_tmp256;struct Cyc_List_List*_tmp257;struct Cyc_List_List*_tmp254=(
struct Cyc_List_List*)_tmp246;_tmp255=*_tmp254;_tmp256=(void*)_tmp255.hd;_tmp257=
_tmp255.tl;{struct _tuple17*_tmp4D7;struct _tuple17*_tmp258=(_tmp4D7=
_region_malloc(r,sizeof(*_tmp4D7)),((_tmp4D7->f1=_tmp24F,((_tmp4D7->f2=_tmp253,((
_tmp4D7->f3=_tmp257,_tmp4D7)))))));struct Cyc_List_List*_tmp4D8;return Cyc_Tcpat_match(
r,_tmp24E,_tmp252,_tmp256,ctx,((_tmp4D8=_region_malloc(r,sizeof(*_tmp4D8)),((
_tmp4D8->hd=_tmp258,((_tmp4D8->tl=_tmp247,_tmp4D8)))))),right_hand_side,rules);}}}}
_LL131:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,
struct Cyc_Tcpat_Con_s*pcon,void*dsc);static struct Cyc_List_List*Cyc_Tcpat_getdargs(
struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp25B=dsc;
struct Cyc_Set_Set*_tmp25C;struct Cyc_List_List*_tmp25D;_LL139: if(*((int*)_tmp25B)
!= 1)goto _LL13B;_tmp25C=((struct Cyc_Tcpat_Neg_struct*)_tmp25B)->f1;_LL13A: {
struct Cyc_Tcpat_Neg_struct _tmp4DB;struct Cyc_Tcpat_Neg_struct*_tmp4DA;void*any=(
void*)((_tmp4DA=_region_malloc(r,sizeof(*_tmp4DA)),((_tmp4DA[0]=((_tmp4DB.tag=1,((
_tmp4DB.f1=Cyc_Tcpat_empty_con_set(r),_tmp4DB)))),_tmp4DA))));struct Cyc_List_List*
_tmp25E=0;{int i=0;for(0;i < pcon->arity;++ i){struct Cyc_List_List*_tmp4DC;_tmp25E=((
_tmp4DC=_region_malloc(r,sizeof(*_tmp4DC)),((_tmp4DC->hd=(void*)any,((_tmp4DC->tl=
_tmp25E,_tmp4DC))))));}}return _tmp25E;}_LL13B: if(*((int*)_tmp25B)!= 0)goto _LL138;
_tmp25D=((struct Cyc_Tcpat_Pos_struct*)_tmp25B)->f2;_LL13C: return _tmp25D;_LL138:;}
struct _tuple19{struct _RegionHandle*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*
Cyc_Tcpat_getoarg(struct _tuple19*env,int i);static struct Cyc_List_List*Cyc_Tcpat_getoarg(
struct _tuple19*env,int i){struct _tuple19 _tmp263;struct _RegionHandle*_tmp264;
struct Cyc_List_List*_tmp265;struct _tuple19*_tmp262=env;_tmp263=*_tmp262;_tmp264=
_tmp263.f1;_tmp265=_tmp263.f2;{struct Cyc_List_List*_tmp4DD;return(_tmp4DD=
_region_malloc(_tmp264,sizeof(*_tmp4DD)),((_tmp4DD->hd=(void*)(i + 1),((_tmp4DD->tl=
_tmp265,_tmp4DD)))));}}static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct
_RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj);static struct
Cyc_List_List*Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*
pcon,struct Cyc_List_List*obj){struct _tuple19 _tmp4DE;struct _tuple19 _tmp267=(
_tmp4DE.f1=r,((_tmp4DE.f2=obj,_tmp4DE)));return((struct Cyc_List_List*(*)(struct
_RegionHandle*r,int n,struct Cyc_List_List*(*f)(struct _tuple19*,int),struct
_tuple19*env))Cyc_List_rtabulate_c)(r,pcon->arity,Cyc_Tcpat_getoarg,& _tmp267);}
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,
void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,
struct Cyc_List_List*rules);static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*
p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp269=p;struct Cyc_Tcpat_Con_s*
_tmp26A;struct Cyc_List_List*_tmp26B;_LL13E: if((int)_tmp269 != 0)goto _LL140;_LL13F:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL140: if(_tmp269 <= (void*)1)goto _LL13D;if(*((int*)_tmp269)!= 0)goto
_LL13D;_tmp26A=((struct Cyc_Tcpat_Con_struct*)_tmp269)->f1;_tmp26B=((struct Cyc_Tcpat_Con_struct*)
_tmp269)->f2;_LL141: {void*_tmp26C=Cyc_Tcpat_static_match(_tmp26A,dsc);_LL143:
if((int)_tmp26C != 0)goto _LL145;_LL144: {struct _tuple18*_tmp4E1;struct Cyc_List_List*
_tmp4E0;struct Cyc_List_List*ctx2=(_tmp4E0=_region_malloc(r,sizeof(*_tmp4E0)),((
_tmp4E0->hd=((_tmp4E1=_region_malloc(r,sizeof(*_tmp4E1)),((_tmp4E1->f1=_tmp26A,((
_tmp4E1->f2=0,_tmp4E1)))))),((_tmp4E0->tl=ctx,_tmp4E0)))));struct _tuple17*
_tmp4E2;struct _tuple17*work_frame=(_tmp4E2=_region_malloc(r,sizeof(*_tmp4E2)),((
_tmp4E2->f1=_tmp26B,((_tmp4E2->f2=Cyc_Tcpat_getoargs(r,_tmp26A,obj),((_tmp4E2->f3=
Cyc_Tcpat_getdargs(r,_tmp26A,dsc),_tmp4E2)))))));struct Cyc_List_List*_tmp4E3;
struct Cyc_List_List*work2=(_tmp4E3=_region_malloc(r,sizeof(*_tmp4E3)),((_tmp4E3->hd=
work_frame,((_tmp4E3->tl=work,_tmp4E3)))));return Cyc_Tcpat_and_match(r,ctx2,
work2,right_hand_side,rules);}_LL145: if((int)_tmp26C != 1)goto _LL147;_LL146:
return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);_LL147:
if((int)_tmp26C != 2)goto _LL142;_LL148: {struct _tuple18*_tmp4E6;struct Cyc_List_List*
_tmp4E5;struct Cyc_List_List*ctx2=(_tmp4E5=_region_malloc(r,sizeof(*_tmp4E5)),((
_tmp4E5->hd=((_tmp4E6=_region_malloc(r,sizeof(*_tmp4E6)),((_tmp4E6->f1=_tmp26A,((
_tmp4E6->f2=0,_tmp4E6)))))),((_tmp4E5->tl=ctx,_tmp4E5)))));struct _tuple17*
_tmp4E7;struct _tuple17*work_frame=(_tmp4E7=_region_malloc(r,sizeof(*_tmp4E7)),((
_tmp4E7->f1=_tmp26B,((_tmp4E7->f2=Cyc_Tcpat_getoargs(r,_tmp26A,obj),((_tmp4E7->f3=
Cyc_Tcpat_getdargs(r,_tmp26A,dsc),_tmp4E7)))))));struct Cyc_List_List*_tmp4E8;
struct Cyc_List_List*work2=(_tmp4E8=_region_malloc(r,sizeof(*_tmp4E8)),((_tmp4E8->hd=
work_frame,((_tmp4E8->tl=work,_tmp4E8)))));void*_tmp271=Cyc_Tcpat_and_match(r,
ctx2,work2,right_hand_side,rules);void*_tmp272=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(
r,ctx,Cyc_Tcpat_add_neg(r,dsc,_tmp26A),work),rules);struct Cyc_Tcpat_IfEq_struct
_tmp4EB;struct Cyc_Tcpat_IfEq_struct*_tmp4EA;return(void*)((_tmp4EA=
_region_malloc(r,sizeof(*_tmp4EA)),((_tmp4EA[0]=((_tmp4EB.tag=2,((_tmp4EB.f1=obj,((
_tmp4EB.f2=_tmp26A,((_tmp4EB.f3=(void*)_tmp271,((_tmp4EB.f4=(void*)_tmp272,
_tmp4EB)))))))))),_tmp4EA))));}_LL142:;}_LL13D:;}static void Cyc_Tcpat_check_exhaust_overlap(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),
void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done);static void
Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*not_exhaust)(
struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,void*),void*
env2,int*exhaust_done){void*_tmp279=d;void*_tmp27A;void*_tmp27B;void*_tmp27C;
void*_tmp27D;_LL14A: if(*((int*)_tmp279)!= 0)goto _LL14C;_tmp27A=(void*)((struct
Cyc_Tcpat_Failure_struct*)_tmp279)->f1;_LL14B: if(!(*exhaust_done)){not_exhaust(r,
env1,_tmp27A);*exhaust_done=1;}goto _LL149;_LL14C: if(*((int*)_tmp279)!= 1)goto
_LL14E;_tmp27B=(void*)((struct Cyc_Tcpat_Success_struct*)_tmp279)->f1;_LL14D:
rhs_appears(env2,_tmp27B);goto _LL149;_LL14E: if(*((int*)_tmp279)!= 2)goto _LL149;
_tmp27C=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp279)->f3;_tmp27D=(void*)((
struct Cyc_Tcpat_IfEq_struct*)_tmp279)->f4;_LL14F: Cyc_Tcpat_check_exhaust_overlap(
r,_tmp27C,not_exhaust,env1,rhs_appears,env2,exhaust_done);Cyc_Tcpat_check_exhaust_overlap(
r,_tmp27D,not_exhaust,env1,rhs_appears,env2,exhaust_done);goto _LL149;_LL149:;}
struct _tuple20{int f1;struct Cyc_Position_Segment*f2;};struct _tuple21{void*f1;
struct _tuple20*f2;};struct _tuple22{struct _RegionHandle*f1;int*f2;};static struct
_tuple21*Cyc_Tcpat_get_match(struct _tuple22*env,struct Cyc_Absyn_Switch_clause*
swc);static struct _tuple21*Cyc_Tcpat_get_match(struct _tuple22*env,struct Cyc_Absyn_Switch_clause*
swc){struct _tuple22 _tmp27F;struct _RegionHandle*_tmp280;int*_tmp281;struct
_tuple22*_tmp27E=env;_tmp27F=*_tmp27E;_tmp280=_tmp27F.f1;_tmp281=_tmp27F.f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp280,swc->pattern);struct _tuple20*_tmp4EC;
struct _tuple20*rhs=(_tmp4EC=_region_malloc(_tmp280,sizeof(*_tmp4EC)),((_tmp4EC->f1=
0,((_tmp4EC->f2=(swc->pattern)->loc,_tmp4EC)))));void*sp;if(swc->where_clause != 
0){{struct Cyc_List_List*_tmp4EF;struct Cyc_List_List*_tmp4EE;sp=Cyc_Tcpat_tuple_pat(
_tmp280,((_tmp4EE=_region_malloc(_tmp280,sizeof(*_tmp4EE)),((_tmp4EE->hd=(void*)
sp0,((_tmp4EE->tl=((_tmp4EF=_region_malloc(_tmp280,sizeof(*_tmp4EF)),((_tmp4EF->hd=(
void*)Cyc_Tcpat_int_pat(_tmp280,*_tmp281,0),((_tmp4EF->tl=0,_tmp4EF)))))),
_tmp4EE)))))),0);}*_tmp281=*_tmp281 + 1;}else{struct Cyc_List_List*_tmp4F2;struct
Cyc_List_List*_tmp4F1;sp=Cyc_Tcpat_tuple_pat(_tmp280,((_tmp4F1=_region_malloc(
_tmp280,sizeof(*_tmp4F1)),((_tmp4F1->hd=(void*)sp0,((_tmp4F1->tl=((_tmp4F2=
_region_malloc(_tmp280,sizeof(*_tmp4F2)),((_tmp4F2->hd=(void*)((void*)0),((
_tmp4F2->tl=0,_tmp4F2)))))),_tmp4F1)))))),0);}{struct _tuple21*_tmp4F3;return(
_tmp4F3=_region_malloc(_tmp280,sizeof(*_tmp4F3)),((_tmp4F3->f1=sp,((_tmp4F3->f2=
rhs,_tmp4F3)))));}}}char Cyc_Tcpat_Desc2string[16]="\000\000\000\000Desc2string\000";
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(struct _RegionHandle*r,void*d);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(struct _RegionHandle*r,void*d){
void*_tmp288=d;struct Cyc_Tcpat_Con_s*_tmp289;struct Cyc_List_List*_tmp28A;struct
Cyc_Set_Set*_tmp28B;_LL151: if(*((int*)_tmp288)!= 0)goto _LL153;_tmp289=((struct
Cyc_Tcpat_Pos_struct*)_tmp288)->f1;_tmp28A=((struct Cyc_Tcpat_Pos_struct*)_tmp288)->f2;
_LL152: {union Cyc_Tcpat_Name_value _tmp28C=_tmp289->name;struct Cyc_Absyn_Pat*
_tmp28D=_tmp289->orig_pat;if(_tmp28D == 0)return Cyc_Tcpat_desc2string(r,(void*)((
struct Cyc_List_List*)_check_null(_tmp28A))->hd);{void*_tmp28E=_tmp28D->r;struct
Cyc_Absyn_Vardecl*_tmp28F;struct Cyc_Absyn_Vardecl*_tmp290;struct Cyc_Absyn_Tvar*
_tmp291;struct Cyc_Absyn_Vardecl*_tmp292;struct Cyc_Absyn_Pat*_tmp293;struct Cyc_Absyn_Pat
_tmp294;void*_tmp295;struct Cyc_Absyn_Datatypefield*_tmp296;struct Cyc_Absyn_AggrInfo*
_tmp297;struct Cyc_Absyn_AggrInfo _tmp298;union Cyc_Absyn_AggrInfoU _tmp299;struct
Cyc_Absyn_Aggrdecl**_tmp29A;struct Cyc_Absyn_Aggrdecl*_tmp29B;struct Cyc_List_List*
_tmp29C;struct Cyc_Absyn_Datatypefield*_tmp29D;int _tmp29E;char _tmp29F;struct
_dyneither_ptr _tmp2A0;struct Cyc_Absyn_Enumfield*_tmp2A1;struct Cyc_Absyn_Enumfield*
_tmp2A2;struct Cyc_Absyn_Exp*_tmp2A3;_LL156: if((int)_tmp28E != 0)goto _LL158;_LL157: {
const char*_tmp4F4;return(_tmp4F4="_",_tag_dyneither(_tmp4F4,sizeof(char),2));}
_LL158: if(_tmp28E <= (void*)2)goto _LL168;if(*((int*)_tmp28E)!= 0)goto _LL15A;
_tmp28F=((struct Cyc_Absyn_Var_p_struct*)_tmp28E)->f1;_LL159: return Cyc_Absynpp_qvar2string(
_tmp28F->name);_LL15A: if(*((int*)_tmp28E)!= 1)goto _LL15C;_tmp290=((struct Cyc_Absyn_Reference_p_struct*)
_tmp28E)->f1;_LL15B: {const char*_tmp4F8;void*_tmp4F7[1];struct Cyc_String_pa_struct
_tmp4F6;return(struct _dyneither_ptr)((_tmp4F6.tag=0,((_tmp4F6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp290->name)),((
_tmp4F7[0]=& _tmp4F6,Cyc_aprintf(((_tmp4F8="*%s",_tag_dyneither(_tmp4F8,sizeof(
char),4))),_tag_dyneither(_tmp4F7,sizeof(void*),1))))))));}_LL15C: if(*((int*)
_tmp28E)!= 2)goto _LL15E;_tmp291=((struct Cyc_Absyn_TagInt_p_struct*)_tmp28E)->f1;
_tmp292=((struct Cyc_Absyn_TagInt_p_struct*)_tmp28E)->f2;_LL15D: {const char*
_tmp4FD;void*_tmp4FC[2];struct Cyc_String_pa_struct _tmp4FB;struct Cyc_String_pa_struct
_tmp4FA;return(struct _dyneither_ptr)((_tmp4FA.tag=0,((_tmp4FA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp291->name),((_tmp4FB.tag=0,((_tmp4FB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp292->name)),((
_tmp4FC[0]=& _tmp4FB,((_tmp4FC[1]=& _tmp4FA,Cyc_aprintf(((_tmp4FD="%s<`%s>",
_tag_dyneither(_tmp4FD,sizeof(char),8))),_tag_dyneither(_tmp4FC,sizeof(void*),2))))))))))))));}
_LL15E: if(*((int*)_tmp28E)!= 3)goto _LL160;_LL15F: {const char*_tmp501;void*
_tmp500[1];struct Cyc_String_pa_struct _tmp4FF;return(struct _dyneither_ptr)((
_tmp4FF.tag=0,((_tmp4FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp28A)),((_tmp500[0]=& _tmp4FF,Cyc_aprintf(((_tmp501="$(%s)",_tag_dyneither(
_tmp501,sizeof(char),6))),_tag_dyneither(_tmp500,sizeof(void*),1))))))));}_LL160:
if(*((int*)_tmp28E)!= 4)goto _LL162;_tmp293=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp28E)->f1;_tmp294=*_tmp293;_tmp295=_tmp294.r;if(_tmp295 <= (void*)2)goto _LL162;
if(*((int*)_tmp295)!= 6)goto _LL162;_tmp296=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp295)->f2;_LL161: {const char*_tmp506;void*_tmp505[2];struct Cyc_String_pa_struct
_tmp504;struct Cyc_String_pa_struct _tmp503;return(struct _dyneither_ptr)((_tmp503.tag=
0,((_tmp503.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp28A)),((_tmp504.tag=0,((_tmp504.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp296->name)),((_tmp505[0]=& _tmp504,((
_tmp505[1]=& _tmp503,Cyc_aprintf(((_tmp506="&%s(%s)",_tag_dyneither(_tmp506,
sizeof(char),8))),_tag_dyneither(_tmp505,sizeof(void*),2))))))))))))));}_LL162:
if(*((int*)_tmp28E)!= 4)goto _LL164;_LL163: {const char*_tmp50A;void*_tmp509[1];
struct Cyc_String_pa_struct _tmp508;return(struct _dyneither_ptr)((_tmp508.tag=0,((
_tmp508.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,
_tmp28A)),((_tmp509[0]=& _tmp508,Cyc_aprintf(((_tmp50A="&%s",_tag_dyneither(
_tmp50A,sizeof(char),4))),_tag_dyneither(_tmp509,sizeof(void*),1))))))));}_LL164:
if(*((int*)_tmp28E)!= 5)goto _LL166;_tmp297=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp28E)->f1;if(_tmp297 == 0)goto _LL166;_tmp298=*_tmp297;_tmp299=_tmp298.aggr_info;
if((_tmp299.KnownAggr).tag != 2)goto _LL166;_tmp29A=(struct Cyc_Absyn_Aggrdecl**)(
_tmp299.KnownAggr).val;_tmp29B=*_tmp29A;_tmp29C=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp28E)->f3;_LL165: if(_tmp29B->kind == (void*)1){struct Cyc_List_List*_tmp2B6=
_tmp29C;struct Cyc_List_List _tmp2B7;struct _tuple14*_tmp2B8;struct _tuple14 _tmp2B9;
struct Cyc_List_List*_tmp2BA;struct Cyc_List_List _tmp2BB;void*_tmp2BC;struct
_dyneither_ptr*_tmp2BD;_LL179: if(_tmp2B6 == 0)goto _LL17B;_tmp2B7=*_tmp2B6;_tmp2B8=(
struct _tuple14*)_tmp2B7.hd;_tmp2B9=*_tmp2B8;_tmp2BA=_tmp2B9.f1;if(_tmp2BA == 0)
goto _LL17B;_tmp2BB=*_tmp2BA;_tmp2BC=(void*)_tmp2BB.hd;if(*((int*)_tmp2BC)!= 1)
goto _LL17B;_tmp2BD=((struct Cyc_Absyn_FieldName_struct*)_tmp2BC)->f1;_LL17A: {
const char*_tmp510;void*_tmp50F[3];struct Cyc_String_pa_struct _tmp50E;struct Cyc_String_pa_struct
_tmp50D;struct Cyc_String_pa_struct _tmp50C;return(struct _dyneither_ptr)((_tmp50C.tag=
0,((_tmp50C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp28A)),((_tmp50D.tag=0,((_tmp50D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp2BD),((_tmp50E.tag=0,((_tmp50E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp29B->name)),((_tmp50F[0]=&
_tmp50E,((_tmp50F[1]=& _tmp50D,((_tmp50F[2]=& _tmp50C,Cyc_aprintf(((_tmp510="%s{.%s = %s}",
_tag_dyneither(_tmp510,sizeof(char),13))),_tag_dyneither(_tmp50F,sizeof(void*),3))))))))))))))))))));}
_LL17B:;_LL17C: goto _LL178;_LL178:;}{const char*_tmp515;void*_tmp514[2];struct Cyc_String_pa_struct
_tmp513;struct Cyc_String_pa_struct _tmp512;return(struct _dyneither_ptr)((_tmp512.tag=
0,((_tmp512.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp28A)),((_tmp513.tag=0,((_tmp513.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp29B->name)),((_tmp514[0]=& _tmp513,((
_tmp514[1]=& _tmp512,Cyc_aprintf(((_tmp515="%s{%s}",_tag_dyneither(_tmp515,
sizeof(char),7))),_tag_dyneither(_tmp514,sizeof(void*),2))))))))))))));}_LL166:
if(*((int*)_tmp28E)!= 6)goto _LL168;_tmp29D=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp28E)->f2;_LL167: {const char*_tmp519;void*_tmp518[1];struct Cyc_String_pa_struct
_tmp517;return(struct _dyneither_ptr)((_tmp517.tag=0,((_tmp517.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp29D->name)),((
_tmp518[0]=& _tmp517,Cyc_aprintf(((_tmp519="%s",_tag_dyneither(_tmp519,sizeof(
char),3))),_tag_dyneither(_tmp518,sizeof(void*),1))))))));}_LL168: if((int)
_tmp28E != 1)goto _LL16A;_LL169: {const char*_tmp51A;return(_tmp51A="NULL",
_tag_dyneither(_tmp51A,sizeof(char),5));}_LL16A: if(_tmp28E <= (void*)2)goto _LL176;
if(*((int*)_tmp28E)!= 7)goto _LL16C;_tmp29E=((struct Cyc_Absyn_Int_p_struct*)
_tmp28E)->f2;_LL16B: {const char*_tmp51E;void*_tmp51D[1];struct Cyc_Int_pa_struct
_tmp51C;return(struct _dyneither_ptr)((_tmp51C.tag=1,((_tmp51C.f1=(unsigned long)
_tmp29E,((_tmp51D[0]=& _tmp51C,Cyc_aprintf(((_tmp51E="%d",_tag_dyneither(_tmp51E,
sizeof(char),3))),_tag_dyneither(_tmp51D,sizeof(void*),1))))))));}_LL16C: if(*((
int*)_tmp28E)!= 8)goto _LL16E;_tmp29F=((struct Cyc_Absyn_Char_p_struct*)_tmp28E)->f1;
_LL16D: {const char*_tmp522;void*_tmp521[1];struct Cyc_Int_pa_struct _tmp520;return(
struct _dyneither_ptr)((_tmp520.tag=1,((_tmp520.f1=(unsigned long)((int)_tmp29F),((
_tmp521[0]=& _tmp520,Cyc_aprintf(((_tmp522="%d",_tag_dyneither(_tmp522,sizeof(
char),3))),_tag_dyneither(_tmp521,sizeof(void*),1))))))));}_LL16E: if(*((int*)
_tmp28E)!= 9)goto _LL170;_tmp2A0=((struct Cyc_Absyn_Float_p_struct*)_tmp28E)->f1;
_LL16F: return _tmp2A0;_LL170: if(*((int*)_tmp28E)!= 10)goto _LL172;_tmp2A1=((struct
Cyc_Absyn_Enum_p_struct*)_tmp28E)->f2;_LL171: _tmp2A2=_tmp2A1;goto _LL173;_LL172:
if(*((int*)_tmp28E)!= 11)goto _LL174;_tmp2A2=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp28E)->f2;_LL173: return Cyc_Absynpp_qvar2string(_tmp2A2->name);_LL174: if(*((
int*)_tmp28E)!= 14)goto _LL176;_tmp2A3=((struct Cyc_Absyn_Exp_p_struct*)_tmp28E)->f1;
_LL175: return Cyc_Absynpp_exp2string(_tmp2A3);_LL176:;_LL177:(int)_throw((void*)
Cyc_Tcpat_Desc2string);_LL155:;}}_LL153: if(*((int*)_tmp288)!= 1)goto _LL150;
_tmp28B=((struct Cyc_Tcpat_Neg_struct*)_tmp288)->f1;_LL154: if(((int(*)(struct Cyc_Set_Set*
s))Cyc_Set_is_empty)(_tmp28B)){const char*_tmp523;return(_tmp523="_",
_tag_dyneither(_tmp523,sizeof(char),2));}{struct Cyc_Tcpat_Con_s*_tmp2D2=((struct
Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp28B);struct Cyc_Absyn_Pat*
_tmp2D3=_tmp2D2->orig_pat;if(_tmp2D3 == 0)(int)_throw((void*)Cyc_Tcpat_Desc2string);{
void*_tmp2D4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2D3->topt))->v);void*_tmp2D5;struct Cyc_Absyn_PtrInfo _tmp2D6;struct Cyc_Absyn_PtrAtts
_tmp2D7;struct Cyc_Absyn_DatatypeInfo _tmp2D8;union Cyc_Absyn_DatatypeInfoU _tmp2D9;
struct Cyc_Absyn_Datatypedecl**_tmp2DA;struct Cyc_Absyn_Datatypedecl*_tmp2DB;
struct Cyc_Absyn_AggrInfo _tmp2DC;union Cyc_Absyn_AggrInfoU _tmp2DD;struct Cyc_Absyn_Aggrdecl**
_tmp2DE;struct Cyc_Absyn_Aggrdecl*_tmp2DF;_LL17E: if(_tmp2D4 <= (void*)4)goto _LL188;
if(*((int*)_tmp2D4)!= 5)goto _LL180;_tmp2D5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2D4)->f2;if((int)_tmp2D5 != 0)goto _LL180;_LL17F:{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp2E0=Cyc_Tcpat_char_con(r,(char)i,(struct Cyc_Absyn_Pat*)
_tmp2D3);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp28B,_tmp2E0)){const char*_tmp527;void*_tmp526[1];struct Cyc_Int_pa_struct
_tmp525;return(struct _dyneither_ptr)((_tmp525.tag=1,((_tmp525.f1=(unsigned long)
i,((_tmp526[0]=& _tmp525,Cyc_aprintf(((_tmp527="%d",_tag_dyneither(_tmp527,
sizeof(char),3))),_tag_dyneither(_tmp526,sizeof(void*),1))))))));}}}(int)_throw((
void*)Cyc_Tcpat_Desc2string);_LL180: if(*((int*)_tmp2D4)!= 5)goto _LL182;_LL181:{
unsigned int i=0;for(0;i < 0 - 1;++ i){struct Cyc_Tcpat_Con_s*_tmp2E4=Cyc_Tcpat_int_con(
r,(int)i,_tmp2D3);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp28B,_tmp2E4)){const char*_tmp52B;void*_tmp52A[1];struct Cyc_Int_pa_struct
_tmp529;return(struct _dyneither_ptr)((_tmp529.tag=1,((_tmp529.f1=(unsigned long)((
int)i),((_tmp52A[0]=& _tmp529,Cyc_aprintf(((_tmp52B="%d",_tag_dyneither(_tmp52B,
sizeof(char),3))),_tag_dyneither(_tmp52A,sizeof(void*),1))))))));}}}(int)_throw((
void*)Cyc_Tcpat_Desc2string);_LL182: if(*((int*)_tmp2D4)!= 4)goto _LL184;_tmp2D6=((
struct Cyc_Absyn_PointerType_struct*)_tmp2D4)->f1;_tmp2D7=_tmp2D6.ptr_atts;_LL183: {
union Cyc_Absyn_Constraint*_tmp2E8=_tmp2D7.nullable;int is_nullable=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2E8);if(is_nullable){
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp28B,Cyc_Tcpat_null_con(r,(struct Cyc_Absyn_Pat*)_tmp2D3))){const char*_tmp52C;
return(_tmp52C="NULL",_tag_dyneither(_tmp52C,sizeof(char),5));}}{const char*
_tmp52D;return(_tmp52D="&_",_tag_dyneither(_tmp52D,sizeof(char),3));}}_LL184: if(*((
int*)_tmp2D4)!= 2)goto _LL186;_tmp2D8=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp2D4)->f1;_tmp2D9=_tmp2D8.datatype_info;if((_tmp2D9.KnownDatatype).tag != 2)
goto _LL186;_tmp2DA=(struct Cyc_Absyn_Datatypedecl**)(_tmp2D9.KnownDatatype).val;
_tmp2DB=*_tmp2DA;_LL185: if(_tmp2DB->is_extensible)(int)_throw((void*)Cyc_Tcpat_Desc2string);{
struct Cyc_List_List*_tmp2EB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2DB->fields))->v;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2EB);for(0;(unsigned int)_tmp2EB;_tmp2EB=_tmp2EB->tl){struct _dyneither_ptr n=*(*((
struct Cyc_Absyn_Datatypefield*)_tmp2EB->hd)->name).f2;struct Cyc_List_List*
_tmp2EC=((struct Cyc_Absyn_Datatypefield*)_tmp2EB->hd)->typs;struct Cyc_Tcpat_Con_s*
_tmp52E;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp28B,(struct Cyc_Tcpat_Con_s*)((_tmp52E=_cycalloc(sizeof(*_tmp52E)),((_tmp52E->name=
Cyc_Tcpat_Name_v(n),((_tmp52E->arity=0,((_tmp52E->span=0,((_tmp52E->orig_pat=
_tmp2D3,_tmp52E)))))))))))){if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2EC)== 0)return n;else{const char*_tmp532;void*_tmp531[1];struct Cyc_String_pa_struct
_tmp530;return(struct _dyneither_ptr)((_tmp530.tag=0,((_tmp530.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)n),((_tmp531[0]=& _tmp530,Cyc_aprintf(((
_tmp532="&%s(...)",_tag_dyneither(_tmp532,sizeof(char),9))),_tag_dyneither(
_tmp531,sizeof(void*),1))))))));}}}(int)_throw((void*)Cyc_Tcpat_Desc2string);}
_LL186: if(*((int*)_tmp2D4)!= 10)goto _LL188;_tmp2DC=((struct Cyc_Absyn_AggrType_struct*)
_tmp2D4)->f1;_tmp2DD=_tmp2DC.aggr_info;if((_tmp2DD.KnownAggr).tag != 2)goto _LL188;
_tmp2DE=(struct Cyc_Absyn_Aggrdecl**)(_tmp2DD.KnownAggr).val;_tmp2DF=*_tmp2DE;if(
!(_tmp2DF->kind == (void*)1))goto _LL188;_LL187: {struct Cyc_List_List*_tmp2F1=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2DF->impl))->fields;int span=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2F1);struct _tuple1 _tmp2F3;struct
_dyneither_ptr*_tmp2F4;struct _dyneither_ptr _tmp2F5;struct _tuple1*_tmp2F2=_tmp2DF->name;
_tmp2F3=*_tmp2F2;_tmp2F4=_tmp2F3.f2;_tmp2F5=*_tmp2F4;for(0;(unsigned int)_tmp2F1;
_tmp2F1=_tmp2F1->tl){struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp2F1->hd)->name;
struct Cyc_Tcpat_Con_s*_tmp533;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp28B,(struct Cyc_Tcpat_Con_s*)((_tmp533=_cycalloc(sizeof(*
_tmp533)),((_tmp533->name=Cyc_Tcpat_Name_v(n),((_tmp533->arity=0,((_tmp533->span=
0,((_tmp533->orig_pat=_tmp2D3,_tmp533)))))))))))){const char*_tmp538;void*_tmp537[
2];struct Cyc_String_pa_struct _tmp536;struct Cyc_String_pa_struct _tmp535;return(
struct _dyneither_ptr)((_tmp535.tag=0,((_tmp535.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)n),((_tmp536.tag=0,((_tmp536.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp2F5),((_tmp537[0]=& _tmp536,((_tmp537[1]=& _tmp535,Cyc_aprintf(((
_tmp538="%s{.%s = _}",_tag_dyneither(_tmp538,sizeof(char),12))),_tag_dyneither(
_tmp537,sizeof(void*),2))))))))))))));}}(int)_throw((void*)Cyc_Tcpat_Desc2string);}
_LL188:;_LL189:(int)_throw((void*)Cyc_Tcpat_Desc2string);_LL17D:;}}_LL150:;}
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct _RegionHandle*r,void*d);
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct _RegionHandle*r,void*d){
struct _dyneither_ptr*_tmp539;return(_tmp539=_cycalloc(sizeof(*_tmp539)),((
_tmp539[0]=Cyc_Tcpat_desc2string(r,d),_tmp539)));}static struct _dyneither_ptr Cyc_Tcpat_descs2string(
struct _RegionHandle*r,struct Cyc_List_List*ds);static struct _dyneither_ptr Cyc_Tcpat_descs2string(
struct _RegionHandle*r,struct Cyc_List_List*ds){struct Cyc_List_List*_tmp2FC=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct
_RegionHandle*,void*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r,Cyc_Tcpat_desc2stringptr,r,ds);struct _dyneither_ptr*comma=_init_dyneither_ptr(
_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);{struct Cyc_List_List*
_tmp2FD=_tmp2FC;for(0;_tmp2FD != 0;_tmp2FD=((struct Cyc_List_List*)_check_null(
_tmp2FD))->tl){if(_tmp2FD->tl != 0){{struct Cyc_List_List*_tmp53A;_tmp2FD->tl=(
struct Cyc_List_List*)((_tmp53A=_cycalloc(sizeof(*_tmp53A)),((_tmp53A->hd=comma,((
_tmp53A->tl=_tmp2FD->tl,_tmp53A))))));}_tmp2FD=_tmp2FD->tl;}}}return(struct
_dyneither_ptr)Cyc_strconcat_l(_tmp2FC);}static void Cyc_Tcpat_not_exhaust_err(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,void*desc);static void Cyc_Tcpat_not_exhaust_err(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,void*desc){struct
_handler_cons _tmp300;_push_handler(& _tmp300);{int _tmp302=0;if(setjmp(_tmp300.handler))
_tmp302=1;if(!_tmp302){{struct _dyneither_ptr _tmp303=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp53E;void*_tmp53D[1];struct Cyc_String_pa_struct _tmp53C;(_tmp53C.tag=
0,((_tmp53C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp303),((_tmp53D[0]=&
_tmp53C,Cyc_Tcutil_terr(loc,((_tmp53E="patterns may not be exhaustive.\n\tmissing case for %s",
_tag_dyneither(_tmp53E,sizeof(char),53))),_tag_dyneither(_tmp53D,sizeof(void*),1)))))));};
_pop_handler();}else{void*_tmp301=(void*)_exn_thrown;void*_tmp308=_tmp301;_LL18B:
if(_tmp308 != Cyc_Tcpat_Desc2string)goto _LL18D;_LL18C:{const char*_tmp541;void*
_tmp540;(_tmp540=0,Cyc_Tcutil_terr(loc,((_tmp541="patterns may not be exhaustive.",
_tag_dyneither(_tmp541,sizeof(char),32))),_tag_dyneither(_tmp540,sizeof(void*),0)));}
goto _LL18A;_LL18D:;_LL18E:(void)_throw(_tmp308);_LL18A:;}}}static void Cyc_Tcpat_rule_occurs(
int dummy,struct _tuple20*rhs);static void Cyc_Tcpat_rule_occurs(int dummy,struct
_tuple20*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs);void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
struct _RegionHandle*_tmp30B=Cyc_Tcenv_get_fnrgn(te);int _tmp30C=0;struct _tuple22
_tmp542;struct _tuple22 _tmp30D=(_tmp542.f1=_tmp30B,((_tmp542.f2=& _tmp30C,_tmp542)));
struct Cyc_List_List*_tmp30E=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple21*(*f)(struct _tuple22*,struct Cyc_Absyn_Switch_clause*),struct
_tuple22*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp30B,Cyc_Tcpat_get_match,&
_tmp30D,swcs);void*_tmp30F=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(_tmp30B,_tmp30E);int _tmp310=0;((void(*)(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple20*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp30B,_tmp30F,Cyc_Tcpat_not_exhaust_err,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp310);for(0;_tmp30E != 0;_tmp30E=_tmp30E->tl){
struct _tuple21 _tmp312;struct _tuple20*_tmp313;struct _tuple20 _tmp314;int _tmp315;
struct Cyc_Position_Segment*_tmp316;struct _tuple21*_tmp311=(struct _tuple21*)
_tmp30E->hd;_tmp312=*_tmp311;_tmp313=_tmp312.f2;_tmp314=*_tmp313;_tmp315=_tmp314.f1;
_tmp316=_tmp314.f2;if(!_tmp315){const char*_tmp545;void*_tmp544;(_tmp544=0,Cyc_Tcutil_terr(
_tmp316,((_tmp545="redundant pattern",_tag_dyneither(_tmp545,sizeof(char),18))),
_tag_dyneither(_tmp544,sizeof(void*),0)));}}}struct _tuple23{struct Cyc_Position_Segment*
f1;int f2;};static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct
_tuple23*pr,void*desc);static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*
r,struct _tuple23*pr,void*desc){(*pr).f2=0;{struct _handler_cons _tmp31A;
_push_handler(& _tmp31A);{int _tmp31C=0;if(setjmp(_tmp31A.handler))_tmp31C=1;if(!
_tmp31C){{struct _dyneither_ptr _tmp31D=Cyc_Tcpat_desc2string(r,desc);const char*
_tmp549;void*_tmp548[1];struct Cyc_String_pa_struct _tmp547;(_tmp547.tag=0,((
_tmp547.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp31D),((_tmp548[0]=&
_tmp547,Cyc_Tcutil_warn((*pr).f1,((_tmp549="pattern not exhaustive.\n\tmissing case for %s",
_tag_dyneither(_tmp549,sizeof(char),45))),_tag_dyneither(_tmp548,sizeof(void*),1)))))));};
_pop_handler();}else{void*_tmp31B=(void*)_exn_thrown;void*_tmp322=_tmp31B;_LL190:
if(_tmp322 != Cyc_Tcpat_Desc2string)goto _LL192;_LL191:{const char*_tmp54C;void*
_tmp54B;(_tmp54B=0,Cyc_Tcutil_warn((*pr).f1,((_tmp54C="pattern not exhaustive.",
_tag_dyneither(_tmp54C,sizeof(char),24))),_tag_dyneither(_tmp54B,sizeof(void*),0)));}
goto _LL18F;_LL192:;_LL193:(void)_throw(_tmp322);_LL18F:;}}}}static void Cyc_Tcpat_dummy_fn(
int i,int j);static void Cyc_Tcpat_dummy_fn(int i,int j){return;}int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);
int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p){struct _RegionHandle*_tmp325=Cyc_Tcenv_get_fnrgn(te);
struct _tuple6*_tmp54F;struct Cyc_List_List*_tmp54E;struct Cyc_List_List*_tmp326=(
_tmp54E=_region_malloc(_tmp325,sizeof(*_tmp54E)),((_tmp54E->hd=((_tmp54F=
_region_malloc(_tmp325,sizeof(*_tmp54F)),((_tmp54F->f1=Cyc_Tcpat_compile_pat(
_tmp325,p),((_tmp54F->f2=0,_tmp54F)))))),((_tmp54E->tl=0,_tmp54E)))));void*
_tmp327=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(
_tmp325,_tmp326);struct _tuple23 _tmp550;struct _tuple23 _tmp328=(_tmp550.f1=loc,((
_tmp550.f2=1,_tmp550)));int _tmp329=0;((void(*)(struct _RegionHandle*r,void*d,void(*
not_exhaust)(struct _RegionHandle*,struct _tuple23*,void*),struct _tuple23*env1,
void(*rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(
_tmp325,_tmp327,Cyc_Tcpat_not_exhaust_warn,& _tmp328,Cyc_Tcpat_dummy_fn,0,&
_tmp329);return _tmp328.f2;}static struct _tuple21*Cyc_Tcpat_get_match2(struct
_RegionHandle*r,struct Cyc_Absyn_Switch_clause*swc);static struct _tuple21*Cyc_Tcpat_get_match2(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*swc){void*sp0=Cyc_Tcpat_compile_pat(
r,swc->pattern);struct _tuple20*_tmp551;struct _tuple20*rhs=(_tmp551=
_region_malloc(r,sizeof(*_tmp551)),((_tmp551->f1=0,((_tmp551->f2=(swc->pattern)->loc,
_tmp551)))));struct _tuple21*_tmp552;return(_tmp552=_region_malloc(r,sizeof(*
_tmp552)),((_tmp552->f1=sp0,((_tmp552->f2=rhs,_tmp552)))));}static void Cyc_Tcpat_not_exhaust_err2(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,void*d);static void Cyc_Tcpat_not_exhaust_err2(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,void*d){;}void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs);
void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*swcs){struct _RegionHandle*_tmp32F=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp330=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple21*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp32F,Cyc_Tcpat_get_match2,
_tmp32F,swcs);void*_tmp331=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(_tmp32F,_tmp330);int _tmp332=0;((void(*)(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple20*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp32F,_tmp331,Cyc_Tcpat_not_exhaust_err2,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp332);for(0;_tmp330 != 0;_tmp330=_tmp330->tl){
struct _tuple21 _tmp334;struct _tuple20*_tmp335;struct _tuple20 _tmp336;int _tmp337;
struct Cyc_Position_Segment*_tmp338;struct _tuple21*_tmp333=(struct _tuple21*)
_tmp330->hd;_tmp334=*_tmp333;_tmp335=_tmp334.f2;_tmp336=*_tmp335;_tmp337=_tmp336.f1;
_tmp338=_tmp336.f2;if(!_tmp337){const char*_tmp555;void*_tmp554;(_tmp554=0,Cyc_Tcutil_terr(
_tmp338,((_tmp555="redundant pattern",_tag_dyneither(_tmp555,sizeof(char),18))),
_tag_dyneither(_tmp554,sizeof(void*),0)));}}}
