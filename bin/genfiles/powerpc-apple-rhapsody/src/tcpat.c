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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;void*
Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct
Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_char_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_float_typ;
extern void*Cyc_Absyn_empty_effect;struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,
struct Cyc_Position_Segment*s);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(void*);struct _dyneither_ptr Cyc_Absynpp_kindbound2string(
void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
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
struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple4{struct Cyc_List_List*
f1;struct _RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct _tuple5*Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr err_msg);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
void*,struct Cyc_List_List*fields);int Cyc_Tcutil_is_noalias_pointer(void*t);int
Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar(void*k);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct
Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};struct Cyc_Tcexp_TestEnv
Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr msg_part);struct _tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple6*tvars_and_bounds_opt;struct Cyc_List_List*
patvars;};struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt);void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct
_tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct
Cyc_Absyn_Exp*e);struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
void*_tmp0=(void*)p->r;struct _tuple1*_tmp1;struct _tuple1*_tmp2;struct Cyc_List_List*
_tmp3;int _tmp4;struct Cyc_Absyn_AggrInfo _tmp5;union Cyc_Absyn_AggrInfoU_union _tmp6;
void*_tmp7;struct _tuple1*_tmp8;struct Cyc_List_List*_tmp9;struct Cyc_List_List*
_tmpA;struct Cyc_List_List**_tmpB;struct Cyc_List_List*_tmpC;int _tmpD;struct Cyc_Absyn_Exp*
_tmpE;_LL1: if(_tmp0 <= (void*)2)goto _LL9;if(*((int*)_tmp0)!= 12)goto _LL3;_tmp1=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp0)->f1;_LL2:{struct _handler_cons _tmpF;
_push_handler(& _tmpF);{int _tmp11=0;if(setjmp(_tmpF.handler))_tmp11=1;if(!_tmp11){{
struct _RegionHandle*_tmp12=Cyc_Tcenv_get_fnrgn(te);void*_tmp13=Cyc_Tcenv_lookup_ordinary(
_tmp12,te,p->loc,_tmp1);struct Cyc_Absyn_Datatypedecl*_tmp14;struct Cyc_Absyn_Datatypefield*
_tmp15;struct Cyc_Absyn_Enumdecl*_tmp16;struct Cyc_Absyn_Enumfield*_tmp17;void*
_tmp18;struct Cyc_Absyn_Enumfield*_tmp19;_LLC: if(*((int*)_tmp13)!= 1)goto _LLE;
_LLD:(void*)(p->r=(void*)((void*)0));{const char*_tmp332;void*_tmp331;(_tmp331=0,
Cyc_Tcutil_terr(p->loc,((_tmp332="struct tag used without arguments in pattern",
_tag_dyneither(_tmp332,sizeof(char),45))),_tag_dyneither(_tmp331,sizeof(void*),0)));}
_npop_handler(0);return;_LLE: if(*((int*)_tmp13)!= 2)goto _LL10;_tmp14=((struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp13)->f1;_tmp15=((struct Cyc_Tcenv_DatatypeRes_struct*)_tmp13)->f2;_LLF:{
struct Cyc_Absyn_Datatype_p_struct _tmp335;struct Cyc_Absyn_Datatype_p_struct*
_tmp334;(void*)(p->r=(void*)((void*)((_tmp334=_cycalloc(sizeof(*_tmp334)),((
_tmp334[0]=((_tmp335.tag=6,((_tmp335.f1=_tmp14,((_tmp335.f2=_tmp15,((_tmp335.f3=
0,((_tmp335.f4=0,_tmp335)))))))))),_tmp334))))));}_npop_handler(0);return;_LL10:
if(*((int*)_tmp13)!= 3)goto _LL12;_tmp16=((struct Cyc_Tcenv_EnumRes_struct*)_tmp13)->f1;
_tmp17=((struct Cyc_Tcenv_EnumRes_struct*)_tmp13)->f2;_LL11:{struct Cyc_Absyn_Enum_p_struct
_tmp338;struct Cyc_Absyn_Enum_p_struct*_tmp337;(void*)(p->r=(void*)((void*)((
_tmp337=_cycalloc(sizeof(*_tmp337)),((_tmp337[0]=((_tmp338.tag=10,((_tmp338.f1=
_tmp16,((_tmp338.f2=_tmp17,_tmp338)))))),_tmp337))))));}_npop_handler(0);return;
_LL12: if(*((int*)_tmp13)!= 4)goto _LL14;_tmp18=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp13)->f1;_tmp19=((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp13)->f2;_LL13:{
struct Cyc_Absyn_AnonEnum_p_struct _tmp33B;struct Cyc_Absyn_AnonEnum_p_struct*
_tmp33A;(void*)(p->r=(void*)((void*)((_tmp33A=_cycalloc(sizeof(*_tmp33A)),((
_tmp33A[0]=((_tmp33B.tag=11,((_tmp33B.f1=(void*)_tmp18,((_tmp33B.f2=_tmp19,
_tmp33B)))))),_tmp33A))))));}_npop_handler(0);return;_LL14: if(*((int*)_tmp13)!= 
0)goto _LLB;_LL15: goto _LLB;_LLB:;};_pop_handler();}else{void*_tmp10=(void*)
_exn_thrown;void*_tmp23=_tmp10;_LL17: if(_tmp23 != Cyc_Dict_Absent)goto _LL19;_LL18:
goto _LL16;_LL19:;_LL1A:(void)_throw(_tmp23);_LL16:;}}}{union Cyc_Absyn_Nmspace_union
_tmp24=(*_tmp1).f1;struct Cyc_List_List*_tmp25;_LL1C: if((_tmp24.Loc_n).tag != 0)
goto _LL1E;_LL1D: goto _LL1F;_LL1E: if((_tmp24.Rel_n).tag != 1)goto _LL20;_tmp25=(
_tmp24.Rel_n).f1;if(_tmp25 != 0)goto _LL20;_LL1F:{union Cyc_Absyn_Nmspace_union
_tmp33C;(*_tmp1).f1=(union Cyc_Absyn_Nmspace_union)(((_tmp33C.Loc_n).tag=0,
_tmp33C));}{struct Cyc_Absyn_Var_p_struct _tmp33F;struct Cyc_Absyn_Var_p_struct*
_tmp33E;(void*)(p->r=(void*)((void*)((_tmp33E=_cycalloc(sizeof(*_tmp33E)),((
_tmp33E[0]=((_tmp33F.tag=0,((_tmp33F.f1=Cyc_Absyn_new_vardecl(_tmp1,(void*)0,0),((
_tmp33F.f2=Cyc_Absyn_new_pat((void*)0,0),_tmp33F)))))),_tmp33E))))));}return;
_LL20:;_LL21:{const char*_tmp342;void*_tmp341;(_tmp341=0,Cyc_Tcutil_terr(p->loc,((
_tmp342="qualified variable in pattern",_tag_dyneither(_tmp342,sizeof(char),30))),
_tag_dyneither(_tmp341,sizeof(void*),0)));}return;_LL1B:;}_LL3: if(*((int*)_tmp0)
!= 13)goto _LL5;_tmp2=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f1;_tmp3=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f2;_tmp4=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp0)->f3;_LL4:{struct _handler_cons _tmp2B;_push_handler(& _tmp2B);{int _tmp2D=0;
if(setjmp(_tmp2B.handler))_tmp2D=1;if(!_tmp2D){{struct _RegionHandle*_tmp2E=Cyc_Tcenv_get_fnrgn(
te);void*_tmp2F=Cyc_Tcenv_lookup_ordinary(_tmp2E,te,p->loc,_tmp2);struct Cyc_Absyn_Aggrdecl*
_tmp30;struct Cyc_Absyn_Datatypedecl*_tmp31;struct Cyc_Absyn_Datatypefield*_tmp32;
_LL23: if(*((int*)_tmp2F)!= 1)goto _LL25;_tmp30=((struct Cyc_Tcenv_AggrRes_struct*)
_tmp2F)->f1;_LL24: {struct Cyc_List_List*_tmp33=0;for(0;_tmp3 != 0;_tmp3=_tmp3->tl){
struct _tuple8*_tmp345;struct Cyc_List_List*_tmp344;_tmp33=((_tmp344=_cycalloc(
sizeof(*_tmp344)),((_tmp344->hd=((_tmp345=_cycalloc(sizeof(*_tmp345)),((_tmp345->f1=
0,((_tmp345->f2=(struct Cyc_Absyn_Pat*)_tmp3->hd,_tmp345)))))),((_tmp344->tl=
_tmp33,_tmp344))))));}{struct Cyc_Absyn_Aggr_p_struct _tmp354;union Cyc_Absyn_AggrInfoU_union
_tmp353;struct Cyc_Absyn_Aggrdecl**_tmp352;struct Cyc_Absyn_AggrInfo _tmp351;struct
Cyc_Absyn_Aggr_p_struct*_tmp350;(void*)(p->r=(void*)((void*)((_tmp350=_cycalloc(
sizeof(*_tmp350)),((_tmp350[0]=((_tmp354.tag=5,((_tmp354.f1=((_tmp351.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)(((_tmp353.KnownAggr).tag=1,(((_tmp353.KnownAggr).f1=((
_tmp352=_cycalloc(sizeof(*_tmp352)),((_tmp352[0]=_tmp30,_tmp352)))),_tmp353)))),((
_tmp351.targs=0,_tmp351)))),((_tmp354.f2=0,((_tmp354.f3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp33),((_tmp354.f4=_tmp4,_tmp354)))))))))),
_tmp350))))));}_npop_handler(0);return;}_LL25: if(*((int*)_tmp2F)!= 2)goto _LL27;
_tmp31=((struct Cyc_Tcenv_DatatypeRes_struct*)_tmp2F)->f1;_tmp32=((struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp2F)->f2;_LL26:{struct Cyc_Absyn_Datatype_p_struct _tmp357;struct Cyc_Absyn_Datatype_p_struct*
_tmp356;(void*)(p->r=(void*)((void*)((_tmp356=_cycalloc(sizeof(*_tmp356)),((
_tmp356[0]=((_tmp357.tag=6,((_tmp357.f1=_tmp31,((_tmp357.f2=_tmp32,((_tmp357.f3=
_tmp3,((_tmp357.f4=_tmp4,_tmp357)))))))))),_tmp356))))));}_npop_handler(0);
return;_LL27: if(*((int*)_tmp2F)!= 3)goto _LL29;_LL28: goto _LL2A;_LL29: if(*((int*)
_tmp2F)!= 4)goto _LL2B;_LL2A:{const char*_tmp35A;void*_tmp359;(_tmp359=0,Cyc_Tcutil_terr(
p->loc,((_tmp35A="enum tag used with arguments in pattern",_tag_dyneither(
_tmp35A,sizeof(char),40))),_tag_dyneither(_tmp359,sizeof(void*),0)));}(void*)(p->r=(
void*)((void*)0));_npop_handler(0);return;_LL2B: if(*((int*)_tmp2F)!= 0)goto _LL22;
_LL2C: goto _LL22;_LL22:;};_pop_handler();}else{void*_tmp2C=(void*)_exn_thrown;
void*_tmp40=_tmp2C;_LL2E: if(_tmp40 != Cyc_Dict_Absent)goto _LL30;_LL2F: goto _LL2D;
_LL30:;_LL31:(void)_throw(_tmp40);_LL2D:;}}}{const char*_tmp35E;void*_tmp35D[1];
struct Cyc_String_pa_struct _tmp35C;(_tmp35C.tag=0,((_tmp35C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2)),((_tmp35D[
0]=& _tmp35C,Cyc_Tcutil_terr(p->loc,((_tmp35E="%s is not a constructor in pattern",
_tag_dyneither(_tmp35E,sizeof(char),35))),_tag_dyneither(_tmp35D,sizeof(void*),1)))))));}(
void*)(p->r=(void*)((void*)0));return;_LL5: if(*((int*)_tmp0)!= 5)goto _LL7;_tmp5=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f1;_tmp6=_tmp5.aggr_info;if((((((struct
Cyc_Absyn_Aggr_p_struct*)_tmp0)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL7;
_tmp7=(_tmp6.UnknownAggr).f1;_tmp8=(_tmp6.UnknownAggr).f2;_tmp9=_tmp5.targs;
_tmpA=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;_tmpB=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;_tmpC=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f3;_tmpD=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f4;_LL6:{struct
_handler_cons _tmp44;_push_handler(& _tmp44);{int _tmp46=0;if(setjmp(_tmp44.handler))
_tmp46=1;if(!_tmp46){{struct Cyc_Absyn_Aggrdecl**_tmp47=Cyc_Tcenv_lookup_aggrdecl(
te,p->loc,_tmp8);struct Cyc_Absyn_Aggrdecl*_tmp48=*_tmp47;if(_tmp48->impl == 0){{
const char*_tmp366;const char*_tmp365;const char*_tmp364;void*_tmp363[1];struct Cyc_String_pa_struct
_tmp362;(_tmp362.tag=0,((_tmp362.f1=(struct _dyneither_ptr)((void*)_tmp48->kind == (
void*)0?(_tmp365="struct",_tag_dyneither(_tmp365,sizeof(char),7)):((_tmp366="union",
_tag_dyneither(_tmp366,sizeof(char),6)))),((_tmp363[0]=& _tmp362,Cyc_Tcutil_terr(
p->loc,((_tmp364="can't destructure an abstract %s",_tag_dyneither(_tmp364,
sizeof(char),33))),_tag_dyneither(_tmp363,sizeof(void*),1)))))));}(void*)(p->r=(
void*)((void*)0));_npop_handler(0);return;}{int more_exists=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp48->impl))->exist_vars)
- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmpB);if(more_exists < 0){{
const char*_tmp369;void*_tmp368;(_tmp368=0,Cyc_Tcutil_terr(p->loc,((_tmp369="too many existentially bound type variables in pattern",
_tag_dyneither(_tmp369,sizeof(char),55))),_tag_dyneither(_tmp368,sizeof(void*),0)));}{
struct Cyc_List_List**_tmp50=_tmpB;{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp48->impl))->exist_vars);for(0;n != 
0;-- n){_tmp50=&((struct Cyc_List_List*)_check_null(*_tmp50))->tl;}}*_tmp50=0;}}
else{if(more_exists > 0){struct Cyc_List_List*_tmp51=0;for(0;more_exists != 0;--
more_exists){struct Cyc_Absyn_Unknown_kb_struct*_tmp36F;struct Cyc_Absyn_Unknown_kb_struct
_tmp36E;struct Cyc_List_List*_tmp36D;_tmp51=((_tmp36D=_cycalloc(sizeof(*_tmp36D)),((
_tmp36D->hd=Cyc_Tcutil_new_tvar((void*)((_tmp36F=_cycalloc(sizeof(*_tmp36F)),((
_tmp36F[0]=((_tmp36E.tag=1,((_tmp36E.f1=0,_tmp36E)))),_tmp36F))))),((_tmp36D->tl=
_tmp51,_tmp36D))))));}*_tmpB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB,_tmp51);}}{struct Cyc_Absyn_Aggr_p_struct
_tmp379;union Cyc_Absyn_AggrInfoU_union _tmp378;struct Cyc_Absyn_AggrInfo _tmp377;
struct Cyc_Absyn_Aggr_p_struct*_tmp376;(void*)(p->r=(void*)((void*)((_tmp376=
_cycalloc(sizeof(*_tmp376)),((_tmp376[0]=((_tmp379.tag=5,((_tmp379.f1=((_tmp377.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)(((_tmp378.KnownAggr).tag=1,(((_tmp378.KnownAggr).f1=
_tmp47,_tmp378)))),((_tmp377.targs=_tmp9,_tmp377)))),((_tmp379.f2=*_tmpB,((
_tmp379.f3=_tmpC,((_tmp379.f4=_tmpD,_tmp379)))))))))),_tmp376))))));}}};
_pop_handler();}else{void*_tmp45=(void*)_exn_thrown;void*_tmp5A=_tmp45;_LL33: if(
_tmp5A != Cyc_Dict_Absent)goto _LL35;_LL34:{const char*_tmp37C;void*_tmp37B;(
_tmp37B=0,Cyc_Tcutil_terr(p->loc,((_tmp37C="Non-aggregate name has designator patterns",
_tag_dyneither(_tmp37C,sizeof(char),43))),_tag_dyneither(_tmp37B,sizeof(void*),0)));}(
void*)(p->r=(void*)((void*)0));goto _LL32;_LL35:;_LL36:(void)_throw(_tmp5A);_LL32:;}}}
return;_LL7: if(*((int*)_tmp0)!= 14)goto _LL9;_tmpE=((struct Cyc_Absyn_Exp_p_struct*)
_tmp0)->f1;_LL8: Cyc_Tcexp_tcExp(te,0,_tmpE);if(!Cyc_Tcutil_is_const_exp(te,_tmpE)){{
const char*_tmp37F;void*_tmp37E;(_tmp37E=0,Cyc_Tcutil_terr(p->loc,((_tmp37F="non-constant expression in case pattern",
_tag_dyneither(_tmp37F,sizeof(char),40))),_tag_dyneither(_tmp37E,sizeof(void*),0)));}(
void*)(p->r=(void*)((void*)0));}{unsigned int _tmp60;int _tmp61;struct _tuple7
_tmp5F=Cyc_Evexp_eval_const_uint_exp(_tmpE);_tmp60=_tmp5F.f1;_tmp61=_tmp5F.f2;{
struct Cyc_Absyn_Int_p_struct _tmp382;struct Cyc_Absyn_Int_p_struct*_tmp381;(void*)(
p->r=(void*)((void*)((_tmp381=_cycalloc(sizeof(*_tmp381)),((_tmp381[0]=((_tmp382.tag=
7,((_tmp382.f1=(void*)((void*)2),((_tmp382.f2=(int)_tmp60,_tmp382)))))),_tmp381))))));}
return;}_LL9:;_LLA: return;_LL0:;}static struct _dyneither_ptr*Cyc_Tcpat_get_name(
struct Cyc_Absyn_Vardecl*vd);static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct
Cyc_Absyn_Vardecl*vd){return(*vd->name).f2;}static void*Cyc_Tcpat_any_type(struct
Cyc_List_List*s,void**topt);static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,
void**topt){if(topt != 0)return*topt;{struct Cyc_Core_Opt*_tmp383;return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,((_tmp383=_cycalloc(sizeof(*_tmp383)),((
_tmp383->v=s,_tmp383)))));}}static void*Cyc_Tcpat_num_type(void**topt,void*numt);
static void*Cyc_Tcpat_num_type(void**topt,void*numt){if(topt != 0  && Cyc_Tcutil_coerceable(*
topt))return*topt;{void*_tmp65=Cyc_Tcutil_compress(numt);_LL38: if(_tmp65 <= (void*)
4)goto _LL3A;if(*((int*)_tmp65)!= 12)goto _LL3A;_LL39: if(topt != 0)return*topt;goto
_LL37;_LL3A:;_LL3B: goto _LL37;_LL37:;}return numt;}static void Cyc_Tcpat_set_vd(
struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**v_result_ptr,void*t);static void
Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**v_result_ptr,
void*t){(void*)(vd->type=(void*)t);vd->tq=Cyc_Absyn_empty_tqual(0);{struct Cyc_List_List*
_tmp384;*v_result_ptr=(struct Cyc_List_List*)((_tmp384=_cycalloc(sizeof(*_tmp384)),((
_tmp384->hd=vd,((_tmp384->tl=*v_result_ptr,_tmp384))))));}}static struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult
res2);static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult
res1,struct Cyc_Tcpat_TcPatResult res2){struct _tuple6*_tmp68;struct Cyc_List_List*
_tmp69;struct Cyc_Tcpat_TcPatResult _tmp67=res1;_tmp68=_tmp67.tvars_and_bounds_opt;
_tmp69=_tmp67.patvars;{struct _tuple6*_tmp6B;struct Cyc_List_List*_tmp6C;struct Cyc_Tcpat_TcPatResult
_tmp6A=res2;_tmp6B=_tmp6A.tvars_and_bounds_opt;_tmp6C=_tmp6A.patvars;if(_tmp68 != 
0  || _tmp6B != 0){if(_tmp68 == 0){struct _tuple6*_tmp385;_tmp68=((_tmp385=_cycalloc(
sizeof(*_tmp385)),((_tmp385->f1=0,((_tmp385->f2=0,_tmp385))))));}if(_tmp6B == 0){
struct _tuple6*_tmp386;_tmp6B=((_tmp386=_cycalloc(sizeof(*_tmp386)),((_tmp386->f1=
0,((_tmp386->f2=0,_tmp386))))));}{struct _tuple6*_tmp389;struct Cyc_Tcpat_TcPatResult
_tmp388;return(_tmp388.tvars_and_bounds_opt=((_tmp389=_cycalloc(sizeof(*_tmp389)),((
_tmp389->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)((*((struct _tuple6*)_check_null(_tmp68))).f1,(*((struct
_tuple6*)_check_null(_tmp6B))).f1),((_tmp389->f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((struct _tuple6*)
_check_null(_tmp68))).f2,(*((struct _tuple6*)_check_null(_tmp6B))).f2),_tmp389)))))),((
_tmp388.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp69,_tmp6C),_tmp388)));}}{struct Cyc_Tcpat_TcPatResult
_tmp38A;return(_tmp38A.tvars_and_bounds_opt=0,((_tmp38A.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp69,_tmp6C),
_tmp38A)));}}}static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(struct Cyc_Position_Segment*
loc);static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(struct Cyc_Position_Segment*loc){
struct Cyc_Absyn_Pat*_tmp38B;return(_tmp38B=_cycalloc(sizeof(*_tmp38B)),((_tmp38B->loc=
loc,((_tmp38B->topt=0,((_tmp38B->r=(void*)((void*)0),_tmp38B)))))));}struct
_tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple10{struct Cyc_Absyn_Aggrfield*
f1;struct Cyc_Absyn_Pat*f2;};static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void**rgn_opt);static
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p,void**topt,void**rgn_opt){Cyc_Tcpat_resolve_pat(te,p);{void*t;struct Cyc_Tcpat_TcPatResult
_tmp38C;struct Cyc_Tcpat_TcPatResult res=(_tmp38C.tvars_and_bounds_opt=0,((_tmp38C.patvars=
0,_tmp38C)));{void*_tmp73=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp74;struct Cyc_Absyn_Pat*
_tmp75;struct Cyc_Absyn_Vardecl*_tmp76;struct Cyc_Absyn_Pat*_tmp77;struct Cyc_Absyn_Tvar*
_tmp78;struct Cyc_Absyn_Vardecl*_tmp79;void*_tmp7A;void*_tmp7B;void*_tmp7C;struct
Cyc_Absyn_Enumdecl*_tmp7D;void*_tmp7E;struct Cyc_Absyn_Pat*_tmp7F;struct Cyc_List_List*
_tmp80;struct Cyc_List_List**_tmp81;int _tmp82;struct Cyc_Absyn_AggrInfo _tmp83;
union Cyc_Absyn_AggrInfoU_union _tmp84;struct Cyc_Absyn_Aggrdecl**_tmp85;struct Cyc_Absyn_Aggrdecl*
_tmp86;struct Cyc_List_List*_tmp87;struct Cyc_List_List**_tmp88;struct Cyc_List_List*
_tmp89;struct Cyc_List_List*_tmp8A;struct Cyc_List_List**_tmp8B;int _tmp8C;struct
Cyc_Absyn_Datatypedecl*_tmp8D;struct Cyc_Absyn_Datatypefield*_tmp8E;struct Cyc_List_List*
_tmp8F;struct Cyc_List_List**_tmp90;int _tmp91;struct Cyc_Absyn_AggrInfo _tmp92;
union Cyc_Absyn_AggrInfoU_union _tmp93;_LL3D: if((int)_tmp73 != 0)goto _LL3F;_LL3E: t=
Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);goto _LL3C;_LL3F: if(
_tmp73 <= (void*)2)goto _LL53;if(*((int*)_tmp73)!= 0)goto _LL41;_tmp74=((struct Cyc_Absyn_Var_p_struct*)
_tmp73)->f1;_tmp75=((struct Cyc_Absyn_Var_p_struct*)_tmp73)->f2;_LL40: res=Cyc_Tcpat_tcPatRec(
te,_tmp75,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp75->topt))->v;
Cyc_Tcpat_set_vd(_tmp74,& res.patvars,t);goto _LL3C;_LL41: if(*((int*)_tmp73)!= 1)
goto _LL43;_tmp76=((struct Cyc_Absyn_Reference_p_struct*)_tmp73)->f1;_tmp77=((
struct Cyc_Absyn_Reference_p_struct*)_tmp73)->f2;_LL42: res=Cyc_Tcpat_tcPatRec(te,
_tmp77,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp77->topt))->v;
if(rgn_opt == 0){{const char*_tmp38F;void*_tmp38E;(_tmp38E=0,Cyc_Tcutil_terr(p->loc,((
_tmp38F="* pattern would point into an unknown/unallowed region",_tag_dyneither(
_tmp38F,sizeof(char),55))),_tag_dyneither(_tmp38E,sizeof(void*),0)));}goto _LL3C;}
else{if(Cyc_Tcutil_is_noalias_pointer(t)){const char*_tmp392;void*_tmp391;(
_tmp391=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp392="* pattern cannot take the address of an alias-free path",_tag_dyneither(
_tmp392,sizeof(char),56))),_tag_dyneither(_tmp391,sizeof(void*),0)));}}{struct
Cyc_Absyn_PointerType_struct _tmp39C;struct Cyc_Absyn_PtrAtts _tmp39B;struct Cyc_Absyn_PtrInfo
_tmp39A;struct Cyc_Absyn_PointerType_struct*_tmp399;Cyc_Tcpat_set_vd(_tmp76,& res.patvars,(
void*)((_tmp399=_cycalloc(sizeof(*_tmp399)),((_tmp399[0]=((_tmp39C.tag=4,((
_tmp39C.f1=((_tmp39A.elt_typ=(void*)t,((_tmp39A.elt_tq=Cyc_Absyn_empty_tqual(0),((
_tmp39A.ptr_atts=((_tmp39B.rgn=(void*)*rgn_opt,((_tmp39B.nullable=Cyc_Absyn_false_conref,((
_tmp39B.bounds=Cyc_Absyn_empty_conref(),((_tmp39B.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(),((_tmp39B.ptrloc=0,_tmp39B)))))))))),_tmp39A)))))),
_tmp39C)))),_tmp399)))));}goto _LL3C;_LL43: if(*((int*)_tmp73)!= 2)goto _LL45;
_tmp78=((struct Cyc_Absyn_TagInt_p_struct*)_tmp73)->f1;_tmp79=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp73)->f2;_LL44: Cyc_Tcpat_set_vd(_tmp79,& res.patvars,(void*)_tmp79->type);{
struct _RegionHandle*_tmp9C=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Absyn_Tvar*_tmp39D[
1];Cyc_Tcenv_add_type_vars(_tmp9C,p->loc,te,((_tmp39D[0]=_tmp78,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp39D,sizeof(struct Cyc_Absyn_Tvar*),
1)))));}if(res.tvars_and_bounds_opt == 0){struct _tuple6*_tmp39E;res.tvars_and_bounds_opt=((
_tmp39E=_cycalloc(sizeof(*_tmp39E)),((_tmp39E->f1=0,((_tmp39E->f2=0,_tmp39E))))));}{
struct Cyc_List_List*_tmp39F;(*res.tvars_and_bounds_opt).f1=((_tmp39F=_cycalloc(
sizeof(*_tmp39F)),((_tmp39F->hd=_tmp78,((_tmp39F->tl=(*res.tvars_and_bounds_opt).f1,
_tmp39F))))));}t=Cyc_Absyn_uint_typ;goto _LL3C;}_LL45: if(*((int*)_tmp73)!= 7)goto
_LL47;_tmp7A=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp73)->f1;if((int)_tmp7A != 
1)goto _LL47;_LL46: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL3C;_LL47:
if(*((int*)_tmp73)!= 7)goto _LL49;_tmp7B=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp73)->f1;if((int)_tmp7B != 2)goto _LL49;_LL48: goto _LL4A;_LL49: if(*((int*)_tmp73)
!= 7)goto _LL4B;_tmp7C=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp73)->f1;if((int)
_tmp7C != 0)goto _LL4B;_LL4A: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto
_LL3C;_LL4B: if(*((int*)_tmp73)!= 8)goto _LL4D;_LL4C: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);
goto _LL3C;_LL4D: if(*((int*)_tmp73)!= 9)goto _LL4F;_LL4E: t=Cyc_Tcpat_num_type(topt,
Cyc_Absyn_float_typ);goto _LL3C;_LL4F: if(*((int*)_tmp73)!= 10)goto _LL51;_tmp7D=((
struct Cyc_Absyn_Enum_p_struct*)_tmp73)->f1;_LL50:{struct Cyc_Absyn_EnumType_struct
_tmp3A2;struct Cyc_Absyn_EnumType_struct*_tmp3A1;t=Cyc_Tcpat_num_type(topt,(void*)((
_tmp3A1=_cycalloc(sizeof(*_tmp3A1)),((_tmp3A1[0]=((_tmp3A2.tag=12,((_tmp3A2.f1=
_tmp7D->name,((_tmp3A2.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7D,_tmp3A2)))))),
_tmp3A1)))));}goto _LL3C;_LL51: if(*((int*)_tmp73)!= 11)goto _LL53;_tmp7E=(void*)((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp73)->f1;_LL52: t=Cyc_Tcpat_num_type(topt,
_tmp7E);goto _LL3C;_LL53: if((int)_tmp73 != 1)goto _LL55;_LL54: if(topt != 0){void*
_tmpA2=Cyc_Tcutil_compress(*topt);_LL66: if(_tmpA2 <= (void*)4)goto _LL68;if(*((int*)
_tmpA2)!= 4)goto _LL68;_LL67: t=*topt;goto tcpat_end;_LL68:;_LL69: goto _LL65;_LL65:;}{
struct Cyc_Core_Opt*_tmpA3=Cyc_Tcenv_lookup_opt_type_vars(te);{struct Cyc_Absyn_PointerType_struct
_tmp3AC;struct Cyc_Absyn_PtrAtts _tmp3AB;struct Cyc_Absyn_PtrInfo _tmp3AA;struct Cyc_Absyn_PointerType_struct*
_tmp3A9;t=(void*)((_tmp3A9=_cycalloc(sizeof(*_tmp3A9)),((_tmp3A9[0]=((_tmp3AC.tag=
4,((_tmp3AC.f1=((_tmp3AA.elt_typ=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ak,_tmpA3),((_tmp3AA.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp3AA.ptr_atts=((
_tmp3AB.rgn=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,_tmpA3),((
_tmp3AB.nullable=Cyc_Absyn_true_conref,((_tmp3AB.bounds=Cyc_Absyn_empty_conref(),((
_tmp3AB.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((
_tmp3AB.ptrloc=0,_tmp3AB)))))))))),_tmp3AA)))))),_tmp3AC)))),_tmp3A9))));}goto
_LL3C;}_LL55: if(_tmp73 <= (void*)2)goto _LL57;if(*((int*)_tmp73)!= 4)goto _LL57;
_tmp7F=((struct Cyc_Absyn_Pointer_p_struct*)_tmp73)->f1;_LL56: {void*inner_typ=(
void*)0;void**_tmpA8=0;if(topt != 0){void*_tmpA9=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmpAA;void*_tmpAB;_LL6B: if(_tmpA9 <= (void*)4)goto _LL6D;
if(*((int*)_tmpA9)!= 4)goto _LL6D;_tmpAA=((struct Cyc_Absyn_PointerType_struct*)
_tmpA9)->f1;_tmpAB=(void*)_tmpAA.elt_typ;_LL6C: inner_typ=_tmpAB;_tmpA8=(void**)&
inner_typ;goto _LL6A;_LL6D:;_LL6E: goto _LL6A;_LL6A:;}{void*ptr_rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te));res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmp7F,_tmpA8,(void**)& ptr_rgn));if(_tmpA8 == 0){void*
_tmpAC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp7F->topt))->v);
struct Cyc_Absyn_DatatypeFieldInfo _tmpAD;union Cyc_Absyn_DatatypeFieldInfoU_union
_tmpAE;struct Cyc_Absyn_Datatypedecl*_tmpAF;struct Cyc_Absyn_Datatypefield*_tmpB0;
struct Cyc_List_List*_tmpB1;_LL70: if(_tmpAC <= (void*)4)goto _LL72;if(*((int*)
_tmpAC)!= 3)goto _LL72;_tmpAD=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpAC)->f1;
_tmpAE=_tmpAD.field_info;if((((((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpAC)->f1).field_info).KnownDatatypefield).tag != 1)goto _LL72;_tmpAF=(_tmpAE.KnownDatatypefield).f1;
_tmpB0=(_tmpAE.KnownDatatypefield).f2;_tmpB1=_tmpAD.targs;if(!(!_tmpAF->is_flat))
goto _LL72;_LL71:{struct Cyc_Absyn_DatatypeType_struct _tmp3BF;union Cyc_Absyn_DatatypeInfoU_union
_tmp3BE;struct Cyc_Absyn_Datatypedecl**_tmp3BD;struct Cyc_Core_Opt*_tmp3BC;struct
Cyc_Absyn_DatatypeInfo _tmp3BB;struct Cyc_Absyn_DatatypeType_struct*_tmp3BA;t=(
void*)((_tmp3BA=_cycalloc(sizeof(*_tmp3BA)),((_tmp3BA[0]=((_tmp3BF.tag=2,((
_tmp3BF.f1=((_tmp3BB.datatype_info=(union Cyc_Absyn_DatatypeInfoU_union)(((
_tmp3BE.KnownDatatype).tag=1,(((_tmp3BE.KnownDatatype).f1=((_tmp3BD=_cycalloc(
sizeof(*_tmp3BD)),((_tmp3BD[0]=_tmpAF,_tmp3BD)))),_tmp3BE)))),((_tmp3BB.targs=
_tmpB1,((_tmp3BB.rgn=((_tmp3BC=_cycalloc(sizeof(*_tmp3BC)),((_tmp3BC->v=(void*)
ptr_rgn,_tmp3BC)))),_tmp3BB)))))),_tmp3BF)))),_tmp3BA))));}goto _LL6F;_LL72:;
_LL73:{struct Cyc_Absyn_PointerType_struct _tmp3C9;struct Cyc_Absyn_PtrAtts _tmp3C8;
struct Cyc_Absyn_PtrInfo _tmp3C7;struct Cyc_Absyn_PointerType_struct*_tmp3C6;t=(
void*)((_tmp3C6=_cycalloc(sizeof(*_tmp3C6)),((_tmp3C6[0]=((_tmp3C9.tag=4,((
_tmp3C9.f1=((_tmp3C7.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7F->topt))->v),((_tmp3C7.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp3C7.ptr_atts=((
_tmp3C8.rgn=(void*)ptr_rgn,((_tmp3C8.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((
_tmp3C8.bounds=Cyc_Absyn_empty_conref(),((_tmp3C8.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(),((_tmp3C8.ptrloc=0,_tmp3C8)))))))))),_tmp3C7)))))),
_tmp3C9)))),_tmp3C6))));}goto _LL6F;_LL6F:;}else{struct Cyc_Absyn_PointerType_struct
_tmp3D3;struct Cyc_Absyn_PtrAtts _tmp3D2;struct Cyc_Absyn_PtrInfo _tmp3D1;struct Cyc_Absyn_PointerType_struct*
_tmp3D0;t=(void*)((_tmp3D0=_cycalloc(sizeof(*_tmp3D0)),((_tmp3D0[0]=((_tmp3D3.tag=
4,((_tmp3D3.f1=((_tmp3D1.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp7F->topt))->v),((_tmp3D1.elt_tq=Cyc_Absyn_empty_tqual(0),((
_tmp3D1.ptr_atts=((_tmp3D2.rgn=(void*)ptr_rgn,((_tmp3D2.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(),((_tmp3D2.bounds=Cyc_Absyn_empty_conref(),((_tmp3D2.zero_term=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((_tmp3D2.ptrloc=0,
_tmp3D2)))))))))),_tmp3D1)))))),_tmp3D3)))),_tmp3D0))));}goto _LL3C;}}_LL57: if(
_tmp73 <= (void*)2)goto _LL59;if(*((int*)_tmp73)!= 3)goto _LL59;_tmp80=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp73)->f1;_tmp81=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_struct*)
_tmp73)->f1;_tmp82=((struct Cyc_Absyn_Tuple_p_struct*)_tmp73)->f2;_LL58: {struct
Cyc_List_List*_tmpC0=*_tmp81;struct Cyc_List_List*pat_ts=0;struct Cyc_List_List*
topt_ts=0;if(topt != 0){void*_tmpC1=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*
_tmpC2;_LL75: if(_tmpC1 <= (void*)4)goto _LL77;if(*((int*)_tmpC1)!= 9)goto _LL77;
_tmpC2=((struct Cyc_Absyn_TupleType_struct*)_tmpC1)->f1;_LL76: topt_ts=_tmpC2;if(
_tmp82){int _tmpC3=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC0);int
_tmpC4=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC2);if(_tmpC3 < 
_tmpC4){struct Cyc_List_List*wild_ps=0;{int i=0;for(0;i < _tmpC4 - _tmpC3;++ i){
struct Cyc_List_List*_tmp3D4;wild_ps=((_tmp3D4=_cycalloc(sizeof(*_tmp3D4)),((
_tmp3D4->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp3D4->tl=wild_ps,_tmp3D4))))));}}*
_tmp81=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmpC0,wild_ps);_tmpC0=*_tmp81;}else{if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmpC0)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpC2)){const char*_tmp3D7;void*_tmp3D6;(_tmp3D6=0,Cyc_Tcutil_warn(p->loc,((
_tmp3D7="unnecessary ... in tuple pattern",_tag_dyneither(_tmp3D7,sizeof(char),
33))),_tag_dyneither(_tmp3D6,sizeof(void*),0)));}}}goto _LL74;_LL77:;_LL78: goto
_LL74;_LL74:;}else{if(_tmp82){const char*_tmp3DA;void*_tmp3D9;(_tmp3D9=0,Cyc_Tcutil_terr(
p->loc,((_tmp3DA="cannot determine missing fields for ... in tuple pattern",
_tag_dyneither(_tmp3DA,sizeof(char),57))),_tag_dyneither(_tmp3D9,sizeof(void*),0)));}}
for(0;_tmpC0 != 0;_tmpC0=_tmpC0->tl){void**_tmpCA=0;if(topt_ts != 0){_tmpCA=(void**)&(*((
struct _tuple9*)topt_ts->hd)).f2;topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmpC0->hd,_tmpCA,rgn_opt));{
struct _tuple9*_tmp3DD;struct Cyc_List_List*_tmp3DC;pat_ts=((_tmp3DC=_cycalloc(
sizeof(*_tmp3DC)),((_tmp3DC->hd=((_tmp3DD=_cycalloc(sizeof(*_tmp3DD)),((_tmp3DD->f1=
Cyc_Absyn_empty_tqual(0),((_tmp3DD->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Pat*)_tmpC0->hd)->topt))->v,_tmp3DD)))))),((_tmp3DC->tl=pat_ts,
_tmp3DC))))));}}{struct Cyc_Absyn_TupleType_struct _tmp3E0;struct Cyc_Absyn_TupleType_struct*
_tmp3DF;t=(void*)((_tmp3DF=_cycalloc(sizeof(*_tmp3DF)),((_tmp3DF[0]=((_tmp3E0.tag=
9,((_tmp3E0.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
pat_ts),_tmp3E0)))),_tmp3DF))));}goto _LL3C;}_LL59: if(_tmp73 <= (void*)2)goto _LL5B;
if(*((int*)_tmp73)!= 5)goto _LL5B;_tmp83=((struct Cyc_Absyn_Aggr_p_struct*)_tmp73)->f1;
_tmp84=_tmp83.aggr_info;if((((((struct Cyc_Absyn_Aggr_p_struct*)_tmp73)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL5B;_tmp85=(_tmp84.KnownAggr).f1;_tmp86=*_tmp85;_tmp87=_tmp83.targs;
_tmp88=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_struct*)_tmp73)->f1).targs;
_tmp89=((struct Cyc_Absyn_Aggr_p_struct*)_tmp73)->f2;_tmp8A=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp73)->f3;_tmp8B=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_struct*)
_tmp73)->f3;_tmp8C=((struct Cyc_Absyn_Aggr_p_struct*)_tmp73)->f4;_LL5A: {struct
Cyc_List_List*_tmpCF=*_tmp8B;const char*_tmp3E2;const char*_tmp3E1;struct
_dyneither_ptr aggr_str=(void*)_tmp86->kind == (void*)0?(_tmp3E2="struct",
_tag_dyneither(_tmp3E2,sizeof(char),7)):((_tmp3E1="union",_tag_dyneither(_tmp3E1,
sizeof(char),6)));if(_tmp86->impl == 0){{const char*_tmp3E6;void*_tmp3E5[1];struct
Cyc_String_pa_struct _tmp3E4;(_tmp3E4.tag=0,((_tmp3E4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)aggr_str),((_tmp3E5[0]=& _tmp3E4,Cyc_Tcutil_terr(p->loc,((
_tmp3E6="can't destructure an abstract %s",_tag_dyneither(_tmp3E6,sizeof(char),
33))),_tag_dyneither(_tmp3E5,sizeof(void*),1)))))));}t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));goto _LL3C;}if(_tmp89 != 0  || (void*)_tmp86->kind == (void*)1  && ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp86->impl))->tagged)rgn_opt=0;{struct _RegionHandle _tmpD3=
_new_region("rgn");struct _RegionHandle*rgn=& _tmpD3;_push_region(rgn);{struct Cyc_List_List*
_tmpD4=0;struct Cyc_List_List*outlives_constraints=0;struct Cyc_List_List*_tmpD5=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp86->impl))->exist_vars;{struct Cyc_List_List*
t=_tmp89;for(0;t != 0;t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(_tmpD5))->hd;_tmpD5=_tmpD5->tl;{void*_tmpD6=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmpD7=Cyc_Absyn_compress_kb((void*)uv->kind);int error=0;
void*k2;{void*_tmpD8=_tmpD7;void*_tmpD9;void*_tmpDA;_LL7A: if(*((int*)_tmpD8)!= 2)
goto _LL7C;_tmpD9=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpD8)->f2;_LL7B:
_tmpDA=_tmpD9;goto _LL7D;_LL7C: if(*((int*)_tmpD8)!= 0)goto _LL7E;_tmpDA=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmpD8)->f1;_LL7D: k2=_tmpDA;goto _LL79;_LL7E:;_LL7F: {
const char*_tmp3E9;void*_tmp3E8;(_tmp3E8=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3E9="unconstrained existential type variable in aggregate",
_tag_dyneither(_tmp3E9,sizeof(char),53))),_tag_dyneither(_tmp3E8,sizeof(void*),0)));}
_LL79:;}{void*_tmpDD=_tmpD6;void*_tmpDE;struct Cyc_Core_Opt*_tmpDF;struct Cyc_Core_Opt**
_tmpE0;void*_tmpE1;struct Cyc_Core_Opt*_tmpE2;struct Cyc_Core_Opt**_tmpE3;_LL81:
if(*((int*)_tmpDD)!= 0)goto _LL83;_tmpDE=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmpDD)->f1;_LL82: if(!Cyc_Tcutil_kind_leq(k2,_tmpDE))error=1;goto _LL80;_LL83: if(*((
int*)_tmpDD)!= 2)goto _LL85;_tmpDF=((struct Cyc_Absyn_Less_kb_struct*)_tmpDD)->f1;
_tmpE0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpDD)->f1;
_tmpE1=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpDD)->f2;_LL84: _tmpE3=_tmpE0;
goto _LL86;_LL85: if(*((int*)_tmpDD)!= 1)goto _LL80;_tmpE2=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpDD)->f1;_tmpE3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpDD)->f1;_LL86:{struct Cyc_Core_Opt*_tmp3EA;*_tmpE3=((_tmp3EA=_cycalloc(
sizeof(*_tmp3EA)),((_tmp3EA->v=(void*)_tmpD7,_tmp3EA))));}goto _LL80;_LL80:;}if(
error){const char*_tmp3F0;void*_tmp3EF[3];struct Cyc_String_pa_struct _tmp3EE;
struct Cyc_String_pa_struct _tmp3ED;struct Cyc_String_pa_struct _tmp3EC;(_tmp3EC.tag=
0,((_tmp3EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
k2)),((_tmp3ED.tag=0,((_tmp3ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(_tmpD6)),((_tmp3EE.tag=0,((_tmp3EE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp3EF[0]=& _tmp3EE,((_tmp3EF[
1]=& _tmp3ED,((_tmp3EF[2]=& _tmp3EC,Cyc_Tcutil_terr(p->loc,((_tmp3F0="type variable %s has kind %s but must have at least kind %s",
_tag_dyneither(_tmp3F0,sizeof(char),60))),_tag_dyneither(_tmp3EF,sizeof(void*),3)))))))))))))))))));}{
struct Cyc_Absyn_VarType_struct _tmp3F3;struct Cyc_Absyn_VarType_struct*_tmp3F2;
void*vartype=(void*)((_tmp3F2=_cycalloc(sizeof(*_tmp3F2)),((_tmp3F2[0]=((_tmp3F3.tag=
1,((_tmp3F3.f1=tv,_tmp3F3)))),_tmp3F2))));{struct Cyc_List_List*_tmp3F4;_tmpD4=((
_tmp3F4=_region_malloc(rgn,sizeof(*_tmp3F4)),((_tmp3F4->hd=(void*)((void*)
vartype),((_tmp3F4->tl=_tmpD4,_tmp3F4))))));}if(k2 == (void*)3){struct _tuple0*
_tmp3F7;struct Cyc_List_List*_tmp3F6;outlives_constraints=((_tmp3F6=_cycalloc(
sizeof(*_tmp3F6)),((_tmp3F6->hd=((_tmp3F7=_cycalloc(sizeof(*_tmp3F7)),((_tmp3F7->f1=
Cyc_Absyn_empty_effect,((_tmp3F7->f2=vartype,_tmp3F7)))))),((_tmp3F6->tl=
outlives_constraints,_tmp3F6))))));}else{if(k2 == (void*)4  || k2 == (void*)5){
const char*_tmp3FA;void*_tmp3F9;(_tmp3F9=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3FA="opened existential had unique or top region kind",
_tag_dyneither(_tmp3FA,sizeof(char),49))),_tag_dyneither(_tmp3F9,sizeof(void*),0)));}}}}}}
_tmpD4=Cyc_List_imp_rev(_tmpD4);{struct _RegionHandle*_tmpF1=Cyc_Tcenv_get_fnrgn(
te);struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(_tmpF1,p->loc,te,_tmp89);
struct Cyc_List_List*_tmpF2=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple4 _tmp3FB;
struct _tuple4 _tmpF3=(_tmp3FB.f1=_tmpF2,((_tmp3FB.f2=rgn,_tmp3FB)));struct Cyc_List_List*
_tmpF4=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct
_tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmpF3,_tmp86->tvs);struct Cyc_List_List*_tmpF5=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp86->impl))->exist_vars,_tmpD4);struct Cyc_List_List*_tmpF6=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple5*))Cyc_Core_snd,_tmpF4);struct Cyc_List_List*_tmpF7=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple5*))Cyc_Core_snd,_tmpF5);struct Cyc_List_List*_tmpF8=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(rgn,_tmpF4,_tmpF5);if(_tmp89 != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp86->impl))->rgn_po != 0){if(res.tvars_and_bounds_opt == 0){struct
_tuple6*_tmp3FC;res.tvars_and_bounds_opt=((_tmp3FC=_cycalloc(sizeof(*_tmp3FC)),((
_tmp3FC->f1=0,((_tmp3FC->f2=0,_tmp3FC))))));}(*res.tvars_and_bounds_opt).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f1,_tmp89);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
outlives_constraints);{struct Cyc_List_List*_tmpFA=0;{struct Cyc_List_List*_tmpFB=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp86->impl))->rgn_po;for(0;_tmpFB != 
0;_tmpFB=_tmpFB->tl){struct _tuple0*_tmp3FF;struct Cyc_List_List*_tmp3FE;_tmpFA=((
_tmp3FE=_cycalloc(sizeof(*_tmp3FE)),((_tmp3FE->hd=((_tmp3FF=_cycalloc(sizeof(*
_tmp3FF)),((_tmp3FF->f1=Cyc_Tcutil_rsubstitute(rgn,_tmpF8,(*((struct _tuple0*)
_tmpFB->hd)).f1),((_tmp3FF->f2=Cyc_Tcutil_rsubstitute(rgn,_tmpF8,(*((struct
_tuple0*)_tmpFB->hd)).f2),_tmp3FF)))))),((_tmp3FE->tl=_tmpFA,_tmp3FE))))));}}
_tmpFA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpFA);(*
res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmpFA);}}*
_tmp88=_tmpF6;{struct Cyc_Absyn_AggrType_struct _tmp40E;union Cyc_Absyn_AggrInfoU_union
_tmp40D;struct Cyc_Absyn_Aggrdecl**_tmp40C;struct Cyc_Absyn_AggrInfo _tmp40B;struct
Cyc_Absyn_AggrType_struct*_tmp40A;t=(void*)((_tmp40A=_cycalloc(sizeof(*_tmp40A)),((
_tmp40A[0]=((_tmp40E.tag=10,((_tmp40E.f1=((_tmp40B.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((
_tmp40D.KnownAggr).tag=1,(((_tmp40D.KnownAggr).f1=((_tmp40C=_cycalloc(sizeof(*
_tmp40C)),((_tmp40C[0]=_tmp86,_tmp40C)))),_tmp40D)))),((_tmp40B.targs=*_tmp88,
_tmp40B)))),_tmp40E)))),_tmp40A))));}if(_tmp8C  && (void*)_tmp86->kind == (void*)1){
const char*_tmp411;void*_tmp410;(_tmp410=0,Cyc_Tcutil_warn(p->loc,((_tmp411="`...' pattern not allowed in union pattern",
_tag_dyneither(_tmp411,sizeof(char),43))),_tag_dyneither(_tmp410,sizeof(void*),0)));}
else{if(_tmp8C){int _tmp105=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpCF);int _tmp106=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp86->impl))->fields);if(_tmp105 < _tmp106){struct Cyc_List_List*
wild_dps=0;{int i=0;for(0;i < _tmp106 - _tmp105;++ i){struct _tuple8*_tmp414;struct
Cyc_List_List*_tmp413;wild_dps=((_tmp413=_cycalloc(sizeof(*_tmp413)),((_tmp413->hd=((
_tmp414=_cycalloc(sizeof(*_tmp414)),((_tmp414->f1=0,((_tmp414->f2=Cyc_Tcpat_wild_pat(
p->loc),_tmp414)))))),((_tmp413->tl=wild_dps,_tmp413))))));}}*_tmp8B=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpCF,
wild_dps);_tmpCF=*_tmp8B;}else{if(_tmp105 == _tmp106){const char*_tmp417;void*
_tmp416;(_tmp416=0,Cyc_Tcutil_warn(p->loc,((_tmp417="unnecessary ... in struct pattern",
_tag_dyneither(_tmp417,sizeof(char),34))),_tag_dyneither(_tmp416,sizeof(void*),0)));}}}}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,void*,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmpCF,(void*)
_tmp86->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp86->impl))->fields);
for(0;fields != 0;fields=fields->tl){struct _tuple10 _tmp10C;struct Cyc_Absyn_Aggrfield*
_tmp10D;struct Cyc_Absyn_Pat*_tmp10E;struct _tuple10*_tmp10B=(struct _tuple10*)
fields->hd;_tmp10C=*_tmp10B;_tmp10D=_tmp10C.f1;_tmp10E=_tmp10C.f2;{void*_tmp10F=
Cyc_Tcutil_rsubstitute(rgn,_tmpF8,(void*)_tmp10D->type);res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te2,_tmp10E,(void**)& _tmp10F,rgn_opt));if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp10E->topt))->v,_tmp10F)){const char*
_tmp41E;void*_tmp41D[4];struct Cyc_String_pa_struct _tmp41C;struct Cyc_String_pa_struct
_tmp41B;struct Cyc_String_pa_struct _tmp41A;struct Cyc_String_pa_struct _tmp419;(
_tmp419.tag=0,((_tmp419.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp10E->topt))->v)),((_tmp41A.tag=0,((
_tmp41A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp10F)),((_tmp41B.tag=0,((_tmp41B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)aggr_str),((_tmp41C.tag=0,((_tmp41C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp10D->name),((_tmp41D[0]=& _tmp41C,((_tmp41D[1]=& _tmp41B,((
_tmp41D[2]=& _tmp41A,((_tmp41D[3]=& _tmp419,Cyc_Tcutil_terr(p->loc,((_tmp41E="field %s of %s pattern expects type %s != %s",
_tag_dyneither(_tmp41E,sizeof(char),45))),_tag_dyneither(_tmp41D,sizeof(void*),4)))))))))))))))))))))))));}}}}}}
_npop_handler(0);goto _LL3C;;_pop_region(rgn);}}_LL5B: if(_tmp73 <= (void*)2)goto
_LL5D;if(*((int*)_tmp73)!= 6)goto _LL5D;_tmp8D=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp73)->f1;_tmp8E=((struct Cyc_Absyn_Datatype_p_struct*)_tmp73)->f2;_tmp8F=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp73)->f3;_tmp90=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Datatype_p_struct*)_tmp73)->f3;_tmp91=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp73)->f4;_LL5C: {struct Cyc_List_List*_tmp119=*_tmp90;if(_tmp8D->is_flat)
rgn_opt=0;{struct _RegionHandle*_tmp11A=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
tqts=_tmp8E->typs;struct Cyc_List_List*_tmp11B=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple4 _tmp41F;struct _tuple4 _tmp11C=(_tmp41F.f1=_tmp11B,((_tmp41F.f2=
_tmp11A,_tmp41F)));struct Cyc_List_List*_tmp11D=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp11A,Cyc_Tcutil_r_make_inst_var,&
_tmp11C,_tmp8D->tvs);struct Cyc_List_List*_tmp11E=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp11D);if(tqts == 0  || _tmp8D->is_flat){struct Cyc_Core_Opt*ropt=0;
if(!_tmp8D->is_flat){struct Cyc_Core_Opt*_tmp422;struct Cyc_Core_Opt*_tmp421;ropt=((
_tmp421=_cycalloc(sizeof(*_tmp421)),((_tmp421->v=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,((_tmp422=_cycalloc(sizeof(*_tmp422)),((
_tmp422->v=_tmp11B,_tmp422))))),_tmp421))));}{struct Cyc_Absyn_DatatypeType_struct
_tmp431;union Cyc_Absyn_DatatypeInfoU_union _tmp430;struct Cyc_Absyn_Datatypedecl**
_tmp42F;struct Cyc_Absyn_DatatypeInfo _tmp42E;struct Cyc_Absyn_DatatypeType_struct*
_tmp42D;t=(void*)((_tmp42D=_cycalloc(sizeof(*_tmp42D)),((_tmp42D[0]=((_tmp431.tag=
2,((_tmp431.f1=((_tmp42E.datatype_info=(union Cyc_Absyn_DatatypeInfoU_union)(((
_tmp430.KnownDatatype).tag=1,(((_tmp430.KnownDatatype).f1=((_tmp42F=_cycalloc(
sizeof(*_tmp42F)),((_tmp42F[0]=_tmp8D,_tmp42F)))),_tmp430)))),((_tmp42E.targs=
_tmp11E,((_tmp42E.rgn=ropt,_tmp42E)))))),_tmp431)))),_tmp42D))));}}else{struct
Cyc_Absyn_DatatypeFieldType_struct _tmp43B;union Cyc_Absyn_DatatypeFieldInfoU_union
_tmp43A;struct Cyc_Absyn_DatatypeFieldInfo _tmp439;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp438;t=(void*)((_tmp438=_cycalloc(sizeof(*_tmp438)),((_tmp438[0]=((_tmp43B.tag=
3,((_tmp43B.f1=((_tmp439.field_info=(union Cyc_Absyn_DatatypeFieldInfoU_union)(((
_tmp43A.KnownDatatypefield).tag=1,(((_tmp43A.KnownDatatypefield).f1=_tmp8D,(((
_tmp43A.KnownDatatypefield).f2=_tmp8E,_tmp43A)))))),((_tmp439.targs=_tmp11E,
_tmp439)))),_tmp43B)))),_tmp438))));}if(topt != 0  && (tqts == 0  || _tmp8D->is_flat)){
void*_tmp12A=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_DatatypeInfo _tmp12B;
struct Cyc_List_List*_tmp12C;_LL88: if(_tmp12A <= (void*)4)goto _LL8C;if(*((int*)
_tmp12A)!= 3)goto _LL8A;_LL89:{struct Cyc_Absyn_DatatypeFieldType_struct _tmp445;
union Cyc_Absyn_DatatypeFieldInfoU_union _tmp444;struct Cyc_Absyn_DatatypeFieldInfo
_tmp443;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp442;t=(void*)((_tmp442=
_cycalloc(sizeof(*_tmp442)),((_tmp442[0]=((_tmp445.tag=3,((_tmp445.f1=((_tmp443.field_info=(
union Cyc_Absyn_DatatypeFieldInfoU_union)(((_tmp444.KnownDatatypefield).tag=1,(((
_tmp444.KnownDatatypefield).f1=_tmp8D,(((_tmp444.KnownDatatypefield).f2=_tmp8E,
_tmp444)))))),((_tmp443.targs=_tmp11E,_tmp443)))),_tmp445)))),_tmp442))));}goto
_LL87;_LL8A: if(*((int*)_tmp12A)!= 2)goto _LL8C;_tmp12B=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp12A)->f1;_tmp12C=_tmp12B.targs;_LL8B: {struct Cyc_List_List*_tmp131=_tmp11E;
for(0;_tmp131 != 0  && _tmp12C != 0;(_tmp131=_tmp131->tl,_tmp12C=_tmp12C->tl)){Cyc_Tcutil_unify((
void*)_tmp131->hd,(void*)_tmp12C->hd);}goto _LL87;}_LL8C:;_LL8D: goto _LL87;_LL87:;}
if(_tmp91){int _tmp132=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp119);
int _tmp133=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);if(_tmp132 < 
_tmp133){struct Cyc_List_List*wild_ps=0;{int i=0;for(0;i < _tmp133 - _tmp132;++ i){
struct Cyc_List_List*_tmp446;wild_ps=((_tmp446=_cycalloc(sizeof(*_tmp446)),((
_tmp446->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp446->tl=wild_ps,_tmp446))))));}}*
_tmp90=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp119,wild_ps);_tmp119=*_tmp90;}else{if(_tmp132 == _tmp133){
const char*_tmp44A;void*_tmp449[1];struct Cyc_String_pa_struct _tmp448;(_tmp448.tag=
0,((_tmp448.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp8D->name)),((_tmp449[0]=& _tmp448,Cyc_Tcutil_warn(p->loc,((_tmp44A="unnecessary ... in datatype field %s",
_tag_dyneither(_tmp44A,sizeof(char),37))),_tag_dyneither(_tmp449,sizeof(void*),1)))))));}}}
for(0;_tmp119 != 0  && tqts != 0;(_tmp119=_tmp119->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*
_tmp138=(struct Cyc_Absyn_Pat*)_tmp119->hd;void*_tmp139=Cyc_Tcutil_rsubstitute(
_tmp11A,_tmp11D,(*((struct _tuple9*)tqts->hd)).f2);res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmp138,(void**)& _tmp139,rgn_opt));if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp138->topt))->v,_tmp139)){const char*
_tmp450;void*_tmp44F[3];struct Cyc_String_pa_struct _tmp44E;struct Cyc_String_pa_struct
_tmp44D;struct Cyc_String_pa_struct _tmp44C;(_tmp44C.tag=0,((_tmp44C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp138->topt))->v)),((_tmp44D.tag=0,((_tmp44D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp139)),((_tmp44E.tag=
0,((_tmp44E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp8E->name)),((_tmp44F[0]=& _tmp44E,((_tmp44F[1]=& _tmp44D,((_tmp44F[2]=& _tmp44C,
Cyc_Tcutil_terr(_tmp138->loc,((_tmp450="%s expects argument type %s, not %s",
_tag_dyneither(_tmp450,sizeof(char),36))),_tag_dyneither(_tmp44F,sizeof(void*),3)))))))))))))))))));}}
if(_tmp119 != 0){const char*_tmp454;void*_tmp453[1];struct Cyc_String_pa_struct
_tmp452;(_tmp452.tag=0,((_tmp452.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8E->name)),((_tmp453[0]=& _tmp452,Cyc_Tcutil_terr(p->loc,((
_tmp454="too many arguments for datatype constructor %s",_tag_dyneither(_tmp454,
sizeof(char),47))),_tag_dyneither(_tmp453,sizeof(void*),1)))))));}if(tqts != 0){
const char*_tmp458;void*_tmp457[1];struct Cyc_String_pa_struct _tmp456;(_tmp456.tag=
0,((_tmp456.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp8E->name)),((_tmp457[0]=& _tmp456,Cyc_Tcutil_terr(p->loc,((_tmp458="too few arguments for datatype constructor %s",
_tag_dyneither(_tmp458,sizeof(char),46))),_tag_dyneither(_tmp457,sizeof(void*),1)))))));}}
goto _LL3C;}}_LL5D: if(_tmp73 <= (void*)2)goto _LL5F;if(*((int*)_tmp73)!= 5)goto
_LL5F;_tmp92=((struct Cyc_Absyn_Aggr_p_struct*)_tmp73)->f1;_tmp93=_tmp92.aggr_info;
if((((((struct Cyc_Absyn_Aggr_p_struct*)_tmp73)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL5F;_LL5E: goto _LL60;_LL5F: if(_tmp73 <= (void*)2)goto _LL61;if(*((int*)
_tmp73)!= 12)goto _LL61;_LL60: goto _LL62;_LL61: if(_tmp73 <= (void*)2)goto _LL63;if(*((
int*)_tmp73)!= 14)goto _LL63;_LL62: goto _LL64;_LL63: if(_tmp73 <= (void*)2)goto _LL3C;
if(*((int*)_tmp73)!= 13)goto _LL3C;_LL64: t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));goto _LL3C;_LL3C:;}tcpat_end: {struct Cyc_Core_Opt*_tmp459;p->topt=((_tmp459=
_cycalloc(sizeof(*_tmp459)),((_tmp459->v=(void*)t,_tmp459))));}return res;}}
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p,void**topt);struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt){struct Cyc_Tcpat_TcPatResult _tmp148=Cyc_Tcpat_tcPatRec(
te,p,(void**)topt,0);struct _RegionHandle*_tmp149=Cyc_Tcenv_get_fnrgn(te);{const
char*_tmp45A;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmp149,Cyc_Tcpat_get_name,_tmp148.patvars),p->loc,((_tmp45A="pattern contains a repeated variable",
_tag_dyneither(_tmp45A,sizeof(char),37))));}return _tmp148;}void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*_tmp14B=(void*)p->r;struct
Cyc_Absyn_Pat*_tmp14C;struct Cyc_List_List*_tmp14D;struct Cyc_List_List*_tmp14E;
struct Cyc_List_List*_tmp14F;_LL8F: if(_tmp14B <= (void*)2)goto _LL97;if(*((int*)
_tmp14B)!= 4)goto _LL91;_tmp14C=((struct Cyc_Absyn_Pointer_p_struct*)_tmp14B)->f1;
_LL90: Cyc_Tcpat_check_pat_regions(te,_tmp14C);{struct _RegionHandle*_tmp150=Cyc_Tcenv_get_fnrgn(
te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp150,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp14C->topt))->v)){{const char*_tmp45D;void*_tmp45C;(_tmp45C=0,Cyc_Tcutil_terr(
p->loc,((_tmp45D="Pattern dereferences to a non-aliased pointer; use swap",
_tag_dyneither(_tmp45D,sizeof(char),56))),_tag_dyneither(_tmp45C,sizeof(void*),0)));}
return;}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp150,(void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v)){{const char*_tmp460;void*_tmp45F;(_tmp45F=0,Cyc_Tcutil_terr(
p->loc,((_tmp460="Pattern dereferences a non-aliased pointer; use swap",
_tag_dyneither(_tmp460,sizeof(char),53))),_tag_dyneither(_tmp45F,sizeof(void*),0)));}
return;}{void*_tmp155=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_PtrInfo _tmp156;struct Cyc_Absyn_PtrAtts _tmp157;void*_tmp158;
struct Cyc_Absyn_DatatypeInfo _tmp159;struct Cyc_Core_Opt*_tmp15A;struct Cyc_Core_Opt
_tmp15B;void*_tmp15C;_LL9A: if(_tmp155 <= (void*)4)goto _LL9E;if(*((int*)_tmp155)!= 
4)goto _LL9C;_tmp156=((struct Cyc_Absyn_PointerType_struct*)_tmp155)->f1;_tmp157=
_tmp156.ptr_atts;_tmp158=(void*)_tmp157.rgn;_LL9B: _tmp15C=_tmp158;goto _LL9D;
_LL9C: if(*((int*)_tmp155)!= 2)goto _LL9E;_tmp159=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp155)->f1;_tmp15A=_tmp159.rgn;if(_tmp15A == 0)goto _LL9E;_tmp15B=*_tmp15A;
_tmp15C=(void*)_tmp15B.v;_LL9D: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp15C);
return;_LL9E:;_LL9F: {const char*_tmp463;void*_tmp462;(_tmp462=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp463="check_pat_regions: bad pointer type",
_tag_dyneither(_tmp463,sizeof(char),36))),_tag_dyneither(_tmp462,sizeof(void*),0)));}
_LL99:;}}_LL91: if(*((int*)_tmp14B)!= 6)goto _LL93;_tmp14D=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp14B)->f3;_LL92: for(0;_tmp14D != 0;_tmp14D=_tmp14D->tl){Cyc_Tcpat_check_pat_regions(
te,(struct Cyc_Absyn_Pat*)_tmp14D->hd);}{void*_tmp15F=(void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v;struct Cyc_Absyn_DatatypeInfo _tmp160;struct Cyc_Core_Opt*
_tmp161;struct Cyc_Absyn_DatatypeInfo _tmp162;struct Cyc_Core_Opt*_tmp163;struct Cyc_Core_Opt
_tmp164;void*_tmp165;_LLA1: if(_tmp15F <= (void*)4)goto _LLA7;if(*((int*)_tmp15F)!= 
2)goto _LLA3;_tmp160=((struct Cyc_Absyn_DatatypeType_struct*)_tmp15F)->f1;_tmp161=
_tmp160.rgn;if(_tmp161 != 0)goto _LLA3;_LLA2: return;_LLA3: if(*((int*)_tmp15F)!= 2)
goto _LLA5;_tmp162=((struct Cyc_Absyn_DatatypeType_struct*)_tmp15F)->f1;_tmp163=
_tmp162.rgn;if(_tmp163 == 0)goto _LLA5;_tmp164=*_tmp163;_tmp165=(void*)_tmp164.v;
_LLA4: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp165);return;_LLA5: if(*((int*)
_tmp15F)!= 3)goto _LLA7;_LLA6: return;_LLA7:;_LLA8: {const char*_tmp466;void*
_tmp465;(_tmp465=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp466="check_pat_regions: bad datatype type",_tag_dyneither(_tmp466,sizeof(
char),37))),_tag_dyneither(_tmp465,sizeof(void*),0)));}_LLA0:;}_LL93: if(*((int*)
_tmp14B)!= 5)goto _LL95;_tmp14E=((struct Cyc_Absyn_Aggr_p_struct*)_tmp14B)->f3;
_LL94: for(0;_tmp14E != 0;_tmp14E=_tmp14E->tl){Cyc_Tcpat_check_pat_regions(te,(*((
struct _tuple8*)_tmp14E->hd)).f2);}return;_LL95: if(*((int*)_tmp14B)!= 3)goto _LL97;
_tmp14F=((struct Cyc_Absyn_Tuple_p_struct*)_tmp14B)->f1;_LL96: for(0;_tmp14F != 0;
_tmp14F=_tmp14F->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp14F->hd);}return;_LL97:;_LL98: return;_LL8E:;}struct Cyc_Tcpat_Name_v_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Tcpat_Int_v_struct{int tag;int f1;};
union Cyc_Tcpat_Name_value_union{struct Cyc_Tcpat_Name_v_struct Name_v;struct Cyc_Tcpat_Int_v_struct
Int_v;};struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value_union name;int arity;int*
span;struct Cyc_Absyn_Pat*orig_pat;};struct Cyc_Tcpat_Con_struct{int tag;struct Cyc_Tcpat_Con_s*
f1;struct Cyc_List_List*f2;};static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*
c1,struct Cyc_Tcpat_Con_s*c2);static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*
c1,struct Cyc_Tcpat_Con_s*c2){union Cyc_Tcpat_Name_value_union _tmp168=c1->name;
struct _dyneither_ptr _tmp169;int _tmp16A;_LLAA: if((_tmp168.Name_v).tag != 0)goto
_LLAC;_tmp169=(_tmp168.Name_v).f1;_LLAB: {union Cyc_Tcpat_Name_value_union _tmp16B=
c2->name;struct _dyneither_ptr _tmp16C;_LLAF: if((_tmp16B.Name_v).tag != 0)goto _LLB1;
_tmp16C=(_tmp16B.Name_v).f1;_LLB0: return Cyc_strcmp((struct _dyneither_ptr)_tmp169,(
struct _dyneither_ptr)_tmp16C);_LLB1: if((_tmp16B.Int_v).tag != 1)goto _LLAE;_LLB2:
return - 1;_LLAE:;}_LLAC: if((_tmp168.Int_v).tag != 1)goto _LLA9;_tmp16A=(_tmp168.Int_v).f1;
_LLAD: {union Cyc_Tcpat_Name_value_union _tmp16D=c2->name;int _tmp16E;_LLB4: if((
_tmp16D.Name_v).tag != 0)goto _LLB6;_LLB5: return 1;_LLB6: if((_tmp16D.Int_v).tag != 1)
goto _LLB3;_tmp16E=(_tmp16D.Int_v).f1;_LLB7: return _tmp16A - _tmp16E;_LLB3:;}_LLA9:;}
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r);static
struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){return((struct
Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))
Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}static int Cyc_Tcpat_one_opt=1;static int
Cyc_Tcpat_two_opt=2;static int Cyc_Tcpat_twofiftysix_opt=256;static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p);static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){union Cyc_Tcpat_Name_value_union
_tmp46C;const char*_tmp46B;struct Cyc_Tcpat_Con_s*_tmp46A;return(_tmp46A=
_region_malloc(r,sizeof(*_tmp46A)),((_tmp46A->name=(union Cyc_Tcpat_Name_value_union)(((
_tmp46C.Name_v).tag=0,(((_tmp46C.Name_v).f1=((_tmp46B="NULL",_tag_dyneither(
_tmp46B,sizeof(char),5))),_tmp46C)))),((_tmp46A->arity=0,((_tmp46A->span=(int*)&
Cyc_Tcpat_two_opt,((_tmp46A->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp46A)))))))));}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct
Cyc_Absyn_Pat*p);static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct
_RegionHandle*r,struct Cyc_Absyn_Pat*p){union Cyc_Tcpat_Name_value_union _tmp472;
const char*_tmp471;struct Cyc_Tcpat_Con_s*_tmp470;return(_tmp470=_region_malloc(r,
sizeof(*_tmp470)),((_tmp470->name=(union Cyc_Tcpat_Name_value_union)(((_tmp472.Name_v).tag=
0,(((_tmp472.Name_v).f1=((_tmp471="&",_tag_dyneither(_tmp471,sizeof(char),2))),
_tmp472)))),((_tmp470->arity=1,((_tmp470->span=(int*)& Cyc_Tcpat_two_opt,((
_tmp470->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp470)))))))));}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p);static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){union Cyc_Tcpat_Name_value_union
_tmp478;const char*_tmp477;struct Cyc_Tcpat_Con_s*_tmp476;return(_tmp476=
_region_malloc(r,sizeof(*_tmp476)),((_tmp476->name=(union Cyc_Tcpat_Name_value_union)(((
_tmp478.Name_v).tag=0,(((_tmp478.Name_v).f1=((_tmp477="&",_tag_dyneither(_tmp477,
sizeof(char),2))),_tmp478)))),((_tmp476->arity=1,((_tmp476->span=(int*)& Cyc_Tcpat_one_opt,((
_tmp476->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp476)))))))));}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p);static struct
Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*
p){union Cyc_Tcpat_Name_value_union _tmp47B;struct Cyc_Tcpat_Con_s*_tmp47A;return(
_tmp47A=_region_malloc(r,sizeof(*_tmp47A)),((_tmp47A->name=(union Cyc_Tcpat_Name_value_union)(((
_tmp47B.Int_v).tag=1,(((_tmp47B.Int_v).f1=i,_tmp47B)))),((_tmp47A->arity=0,((
_tmp47A->span=0,((_tmp47A->orig_pat=p,_tmp47A)))))))));}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*
p);static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _RegionHandle*r,struct
_dyneither_ptr f,struct Cyc_Absyn_Pat*p){union Cyc_Tcpat_Name_value_union _tmp47E;
struct Cyc_Tcpat_Con_s*_tmp47D;return(_tmp47D=_region_malloc(r,sizeof(*_tmp47D)),((
_tmp47D->name=(union Cyc_Tcpat_Name_value_union)(((_tmp47E.Name_v).tag=0,(((
_tmp47E.Name_v).f1=f,_tmp47E)))),((_tmp47D->arity=0,((_tmp47D->span=0,((_tmp47D->orig_pat=(
struct Cyc_Absyn_Pat*)p,_tmp47D)))))))));}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(
struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p);static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){union Cyc_Tcpat_Name_value_union
_tmp481;struct Cyc_Tcpat_Con_s*_tmp480;return(_tmp480=_region_malloc(r,sizeof(*
_tmp480)),((_tmp480->name=(union Cyc_Tcpat_Name_value_union)(((_tmp481.Int_v).tag=
1,(((_tmp481.Int_v).f1=(int)c,_tmp481)))),((_tmp480->arity=0,((_tmp480->span=(
int*)& Cyc_Tcpat_twofiftysix_opt,((_tmp480->orig_pat=(struct Cyc_Absyn_Pat*)p,
_tmp480)))))))));}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct
_RegionHandle*r,int i,struct Cyc_Absyn_Pat*p);static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(
struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){union Cyc_Tcpat_Name_value_union
_tmp487;const char*_tmp486;struct Cyc_Tcpat_Con_s*_tmp485;return(_tmp485=
_region_malloc(r,sizeof(*_tmp485)),((_tmp485->name=(union Cyc_Tcpat_Name_value_union)(((
_tmp487.Name_v).tag=0,(((_tmp487.Name_v).f1=((_tmp486="$",_tag_dyneither(_tmp486,
sizeof(char),2))),_tmp487)))),((_tmp485->arity=i,((_tmp485->span=(int*)& Cyc_Tcpat_one_opt,((
_tmp485->orig_pat=p,_tmp485)))))))));}static void*Cyc_Tcpat_null_pat(struct
_RegionHandle*r,struct Cyc_Absyn_Pat*p);static void*Cyc_Tcpat_null_pat(struct
_RegionHandle*r,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct _tmp48A;struct
Cyc_Tcpat_Con_struct*_tmp489;return(void*)((_tmp489=_region_malloc(r,sizeof(*
_tmp489)),((_tmp489[0]=((_tmp48A.tag=0,((_tmp48A.f1=Cyc_Tcpat_null_con(r,p),((
_tmp48A.f2=0,_tmp48A)))))),_tmp489))));}static void*Cyc_Tcpat_int_pat(struct
_RegionHandle*r,int i,struct Cyc_Absyn_Pat*p);static void*Cyc_Tcpat_int_pat(struct
_RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct _tmp48D;
struct Cyc_Tcpat_Con_struct*_tmp48C;return(void*)((_tmp48C=_region_malloc(r,
sizeof(*_tmp48C)),((_tmp48C[0]=((_tmp48D.tag=0,((_tmp48D.f1=Cyc_Tcpat_int_con(r,
i,p),((_tmp48D.f2=0,_tmp48D)))))),_tmp48C))));}static void*Cyc_Tcpat_char_pat(
struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p);static void*Cyc_Tcpat_char_pat(
struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct
_tmp490;struct Cyc_Tcpat_Con_struct*_tmp48F;return(void*)((_tmp48F=_region_malloc(
r,sizeof(*_tmp48F)),((_tmp48F[0]=((_tmp490.tag=0,((_tmp490.f1=Cyc_Tcpat_char_con(
r,c,p),((_tmp490.f2=0,_tmp490)))))),_tmp48F))));}static void*Cyc_Tcpat_float_pat(
struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p);static void*
Cyc_Tcpat_float_pat(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*
p){struct Cyc_Tcpat_Con_struct _tmp493;struct Cyc_Tcpat_Con_struct*_tmp492;return(
void*)((_tmp492=_region_malloc(r,sizeof(*_tmp492)),((_tmp492[0]=((_tmp493.tag=0,((
_tmp493.f1=Cyc_Tcpat_float_con(r,f,p),((_tmp493.f2=0,_tmp493)))))),_tmp492))));}
static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*
p0);static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*
p0){struct Cyc_Tcpat_Con_struct _tmp499;struct Cyc_List_List*_tmp498;struct Cyc_Tcpat_Con_struct*
_tmp497;return(void*)((_tmp497=_region_malloc(r,sizeof(*_tmp497)),((_tmp497[0]=((
_tmp499.tag=0,((_tmp499.f1=Cyc_Tcpat_null_ptr_con(r,p0),((_tmp499.f2=((_tmp498=
_region_malloc(r,sizeof(*_tmp498)),((_tmp498->hd=(void*)p,((_tmp498->tl=0,
_tmp498)))))),_tmp499)))))),_tmp497))));}static void*Cyc_Tcpat_ptr_pat(struct
_RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0);static void*Cyc_Tcpat_ptr_pat(
struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){struct Cyc_Tcpat_Con_struct
_tmp49F;struct Cyc_List_List*_tmp49E;struct Cyc_Tcpat_Con_struct*_tmp49D;return(
void*)((_tmp49D=_region_malloc(r,sizeof(*_tmp49D)),((_tmp49D[0]=((_tmp49F.tag=0,((
_tmp49F.f1=Cyc_Tcpat_ptr_con(r,p0),((_tmp49F.f2=((_tmp49E=_region_malloc(r,
sizeof(*_tmp49E)),((_tmp49E->hd=(void*)p,((_tmp49E->tl=0,_tmp49E)))))),_tmp49F)))))),
_tmp49D))));}static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,struct Cyc_List_List*
ss,struct Cyc_Absyn_Pat*p);static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,
struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct _tmp4A2;
struct Cyc_Tcpat_Con_struct*_tmp4A1;return(void*)((_tmp4A1=_region_malloc(r,
sizeof(*_tmp4A1)),((_tmp4A1[0]=((_tmp4A2.tag=0,((_tmp4A2.f1=Cyc_Tcpat_tuple_con(
r,Cyc_List_length(ss),p),((_tmp4A2.f2=ss,_tmp4A2)))))),_tmp4A1))));}static void*
Cyc_Tcpat_con_pat(struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,
struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p);static void*Cyc_Tcpat_con_pat(
struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*
ps,struct Cyc_Absyn_Pat*p){union Cyc_Tcpat_Name_value_union _tmp4A5;struct Cyc_Tcpat_Con_s*
_tmp4A4;struct Cyc_Tcpat_Con_s*c=(_tmp4A4=_region_malloc(r,sizeof(*_tmp4A4)),((
_tmp4A4->name=(union Cyc_Tcpat_Name_value_union)(((_tmp4A5.Name_v).tag=0,(((
_tmp4A5.Name_v).f1=con_name,_tmp4A5)))),((_tmp4A4->arity=Cyc_List_length(ps),((
_tmp4A4->span=span,((_tmp4A4->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4A4)))))))));
struct Cyc_Tcpat_Con_struct _tmp4A8;struct Cyc_Tcpat_Con_struct*_tmp4A7;return(void*)((
_tmp4A7=_region_malloc(r,sizeof(*_tmp4A7)),((_tmp4A7[0]=((_tmp4A8.tag=0,((
_tmp4A8.f1=c,((_tmp4A8.f2=ps,_tmp4A8)))))),_tmp4A7))));}static void*Cyc_Tcpat_compile_pat(
struct _RegionHandle*r,struct Cyc_Absyn_Pat*p);static void*Cyc_Tcpat_compile_pat(
struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){void*s;{void*_tmp195=(void*)p->r;
void*_tmp196;int _tmp197;char _tmp198;struct _dyneither_ptr _tmp199;struct Cyc_Absyn_Pat*
_tmp19A;struct Cyc_Absyn_Pat*_tmp19B;struct Cyc_Absyn_Pat*_tmp19C;struct Cyc_Absyn_Datatypedecl*
_tmp19D;struct Cyc_Absyn_Datatypefield*_tmp19E;struct Cyc_List_List*_tmp19F;struct
Cyc_List_List*_tmp1A0;struct Cyc_Absyn_AggrInfo _tmp1A1;union Cyc_Absyn_AggrInfoU_union
_tmp1A2;struct Cyc_Absyn_Aggrdecl**_tmp1A3;struct Cyc_Absyn_Aggrdecl*_tmp1A4;
struct Cyc_List_List*_tmp1A5;struct Cyc_Absyn_Enumdecl*_tmp1A6;struct Cyc_Absyn_Enumfield*
_tmp1A7;void*_tmp1A8;struct Cyc_Absyn_Enumfield*_tmp1A9;_LLB9: if((int)_tmp195 != 0)
goto _LLBB;_LLBA: goto _LLBC;_LLBB: if(_tmp195 <= (void*)2)goto _LLBD;if(*((int*)
_tmp195)!= 2)goto _LLBD;_LLBC: s=(void*)0;goto _LLB8;_LLBD: if((int)_tmp195 != 1)goto
_LLBF;_LLBE: s=Cyc_Tcpat_null_pat(r,p);goto _LLB8;_LLBF: if(_tmp195 <= (void*)2)goto
_LLC1;if(*((int*)_tmp195)!= 7)goto _LLC1;_tmp196=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp195)->f1;_tmp197=((struct Cyc_Absyn_Int_p_struct*)_tmp195)->f2;_LLC0: s=Cyc_Tcpat_int_pat(
r,_tmp197,(struct Cyc_Absyn_Pat*)p);goto _LLB8;_LLC1: if(_tmp195 <= (void*)2)goto
_LLC3;if(*((int*)_tmp195)!= 8)goto _LLC3;_tmp198=((struct Cyc_Absyn_Char_p_struct*)
_tmp195)->f1;_LLC2: s=Cyc_Tcpat_char_pat(r,_tmp198,p);goto _LLB8;_LLC3: if(_tmp195
<= (void*)2)goto _LLC5;if(*((int*)_tmp195)!= 9)goto _LLC5;_tmp199=((struct Cyc_Absyn_Float_p_struct*)
_tmp195)->f1;_LLC4: s=Cyc_Tcpat_float_pat(r,_tmp199,p);goto _LLB8;_LLC5: if(_tmp195
<= (void*)2)goto _LLC7;if(*((int*)_tmp195)!= 0)goto _LLC7;_tmp19A=((struct Cyc_Absyn_Var_p_struct*)
_tmp195)->f2;_LLC6: s=Cyc_Tcpat_compile_pat(r,_tmp19A);goto _LLB8;_LLC7: if(_tmp195
<= (void*)2)goto _LLC9;if(*((int*)_tmp195)!= 1)goto _LLC9;_tmp19B=((struct Cyc_Absyn_Reference_p_struct*)
_tmp195)->f2;_LLC8: s=Cyc_Tcpat_compile_pat(r,_tmp19B);goto _LLB8;_LLC9: if(_tmp195
<= (void*)2)goto _LLCB;if(*((int*)_tmp195)!= 4)goto _LLCB;_tmp19C=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp195)->f1;_LLCA:{void*_tmp1AA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1AB;struct Cyc_Absyn_PtrAtts
_tmp1AC;struct Cyc_Absyn_Conref*_tmp1AD;struct Cyc_Absyn_DatatypeInfo _tmp1AE;
struct Cyc_Core_Opt*_tmp1AF;struct Cyc_Core_Opt _tmp1B0;_LLDE: if(_tmp1AA <= (void*)4)
goto _LLE2;if(*((int*)_tmp1AA)!= 4)goto _LLE0;_tmp1AB=((struct Cyc_Absyn_PointerType_struct*)
_tmp1AA)->f1;_tmp1AC=_tmp1AB.ptr_atts;_tmp1AD=_tmp1AC.nullable;_LLDF: {int
is_nullable=0;int still_working=1;while(still_working){union Cyc_Absyn_Constraint_union
_tmp1B1=_tmp1AD->v;struct Cyc_Absyn_Conref*_tmp1B2;int _tmp1B3;_LLE5: if((_tmp1B1.Forward_constr).tag
!= 1)goto _LLE7;_tmp1B2=(_tmp1B1.Forward_constr).f1;_LLE6: _tmp1AD->v=_tmp1B2->v;
continue;_LLE7: if((_tmp1B1.No_constr).tag != 2)goto _LLE9;_LLE8:{union Cyc_Absyn_Constraint_union
_tmp4A9;_tmp1AD->v=(union Cyc_Absyn_Constraint_union)(((_tmp4A9.Eq_constr).tag=0,(((
_tmp4A9.Eq_constr).f1=(void*)0,_tmp4A9))));}is_nullable=0;still_working=0;goto
_LLE4;_LLE9: if((_tmp1B1.Eq_constr).tag != 0)goto _LLE4;_tmp1B3=(_tmp1B1.Eq_constr).f1;
_LLEA: is_nullable=(int)_tmp1B3;still_working=0;goto _LLE4;_LLE4:;}{void*ss=Cyc_Tcpat_compile_pat(
r,_tmp19C);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{s=Cyc_Tcpat_ptr_pat(
r,ss,p);}goto _LLDD;}}_LLE0: if(*((int*)_tmp1AA)!= 2)goto _LLE2;_tmp1AE=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp1AA)->f1;_tmp1AF=_tmp1AE.rgn;if(_tmp1AF == 0)goto _LLE2;_tmp1B0=*_tmp1AF;_LLE1:{
void*_tmp1B5=(void*)_tmp19C->r;struct Cyc_Absyn_Datatypedecl*_tmp1B6;struct Cyc_Absyn_Datatypefield*
_tmp1B7;struct Cyc_List_List*_tmp1B8;_LLEC: if(_tmp1B5 <= (void*)2)goto _LLEE;if(*((
int*)_tmp1B5)!= 6)goto _LLEE;_tmp1B6=((struct Cyc_Absyn_Datatype_p_struct*)_tmp1B5)->f1;
_tmp1B7=((struct Cyc_Absyn_Datatype_p_struct*)_tmp1B5)->f2;_tmp1B8=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp1B5)->f3;_LLED: {int*span;if(_tmp1B6->is_extensible)span=0;else{int*_tmp4AA;
span=((_tmp4AA=_region_malloc(r,sizeof(*_tmp4AA)),((_tmp4AA[0]=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1B6->fields))->v),_tmp4AA))));}s=Cyc_Tcpat_con_pat(r,*(*_tmp1B7->name).f2,
span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1B8),p);goto _LLEB;}_LLEE:;
_LLEF: {const char*_tmp4AD;void*_tmp4AC;(_tmp4AC=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4AD="non-datatype pattern has datatype type",
_tag_dyneither(_tmp4AD,sizeof(char),39))),_tag_dyneither(_tmp4AC,sizeof(void*),0)));}
_LLEB:;}goto _LLDD;_LLE2:;_LLE3: {const char*_tmp4B0;void*_tmp4AF;(_tmp4AF=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4B0="pointer pattern does not have pointer type",
_tag_dyneither(_tmp4B0,sizeof(char),43))),_tag_dyneither(_tmp4AF,sizeof(void*),0)));}
_LLDD:;}goto _LLB8;_LLCB: if(_tmp195 <= (void*)2)goto _LLCD;if(*((int*)_tmp195)!= 6)
goto _LLCD;_tmp19D=((struct Cyc_Absyn_Datatype_p_struct*)_tmp195)->f1;_tmp19E=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp195)->f2;_tmp19F=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp195)->f3;_LLCC: {int*span;{void*_tmp1BE=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(p->topt))->v);_LLF1: if(_tmp1BE <= (void*)4)goto _LLF5;
if(*((int*)_tmp1BE)!= 2)goto _LLF3;_LLF2: if(_tmp19D->is_extensible)span=0;else{
int*_tmp4B1;span=((_tmp4B1=_region_malloc(r,sizeof(*_tmp4B1)),((_tmp4B1[0]=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp19D->fields))->v),_tmp4B1))));}goto _LLF0;_LLF3: if(*((int*)
_tmp1BE)!= 3)goto _LLF5;_LLF4: span=(int*)& Cyc_Tcpat_one_opt;goto _LLF0;_LLF5:;
_LLF6:{const char*_tmp4B4;void*_tmp4B3;span=((_tmp4B3=0,((int*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4B4="void datatype pattern has bad type",
_tag_dyneither(_tmp4B4,sizeof(char),35))),_tag_dyneither(_tmp4B3,sizeof(void*),0))));}
goto _LLF0;_LLF0:;}s=Cyc_Tcpat_con_pat(r,*(*_tmp19E->name).f2,span,((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,
r,_tmp19F),p);goto _LLB8;}_LLCD: if(_tmp195 <= (void*)2)goto _LLCF;if(*((int*)
_tmp195)!= 3)goto _LLCF;_tmp1A0=((struct Cyc_Absyn_Tuple_p_struct*)_tmp195)->f1;
_LLCE: s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1A0),(
struct Cyc_Absyn_Pat*)p);goto _LLB8;_LLCF: if(_tmp195 <= (void*)2)goto _LLD1;if(*((
int*)_tmp195)!= 5)goto _LLD1;_tmp1A1=((struct Cyc_Absyn_Aggr_p_struct*)_tmp195)->f1;
_tmp1A2=_tmp1A1.aggr_info;if((((((struct Cyc_Absyn_Aggr_p_struct*)_tmp195)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LLD1;_tmp1A3=(_tmp1A2.KnownAggr).f1;_tmp1A4=*_tmp1A3;_tmp1A5=((struct
Cyc_Absyn_Aggr_p_struct*)_tmp195)->f3;_LLD0: if((void*)_tmp1A4->kind == (void*)0){
struct Cyc_List_List*ps=0;{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1A4->impl))->fields;for(0;fields != 0;fields=fields->tl){const
char*_tmp4B5;int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)
fields->hd)->name,((_tmp4B5="",_tag_dyneither(_tmp4B5,sizeof(char),1))))== 0;{
struct Cyc_List_List*dlps0=_tmp1A5;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple8 _tmp1C3;struct Cyc_List_List*_tmp1C4;struct Cyc_Absyn_Pat*_tmp1C5;
struct _tuple8*_tmp1C2=(struct _tuple8*)dlps0->hd;_tmp1C3=*_tmp1C2;_tmp1C4=_tmp1C3.f1;
_tmp1C5=_tmp1C3.f2;{struct Cyc_List_List*_tmp1C6=_tmp1C4;struct Cyc_List_List
_tmp1C7;void*_tmp1C8;struct _dyneither_ptr*_tmp1C9;struct Cyc_List_List*_tmp1CA;
_LLF8: if(_tmp1C6 == 0)goto _LLFA;_tmp1C7=*_tmp1C6;_tmp1C8=(void*)_tmp1C7.hd;if(*((
int*)_tmp1C8)!= 1)goto _LLFA;_tmp1C9=((struct Cyc_Absyn_FieldName_struct*)_tmp1C8)->f1;
_tmp1CA=_tmp1C7.tl;if(_tmp1CA != 0)goto _LLFA;_LLF9: if(Cyc_strptrcmp(_tmp1C9,((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){{struct Cyc_List_List*_tmp4B6;
ps=((_tmp4B6=_region_malloc(r,sizeof(*_tmp4B6)),((_tmp4B6->hd=(void*)Cyc_Tcpat_compile_pat(
r,_tmp1C5),((_tmp4B6->tl=ps,_tmp4B6))))));}found=1;}goto _LLF7;_LLFA:;_LLFB: {
const char*_tmp4B9;void*_tmp4B8;(_tmp4B8=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4B9="bad designator(s)",
_tag_dyneither(_tmp4B9,sizeof(char),18))),_tag_dyneither(_tmp4B8,sizeof(void*),0)));}
_LLF7:;}}}if(!found){const char*_tmp4BC;void*_tmp4BB;(_tmp4BB=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4BC="bad designator",
_tag_dyneither(_tmp4BC,sizeof(char),15))),_tag_dyneither(_tmp4BB,sizeof(void*),0)));}}}
s=Cyc_Tcpat_tuple_pat(r,ps,(struct Cyc_Absyn_Pat*)p);}else{if(!((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1A4->impl))->tagged){const char*_tmp4BF;void*_tmp4BE;(_tmp4BE=0,
Cyc_Tcutil_terr(p->loc,((_tmp4BF="patterns on untagged unions not yet supported.",
_tag_dyneither(_tmp4BF,sizeof(char),47))),_tag_dyneither(_tmp4BE,sizeof(void*),0)));}{
int*_tmp4C0;int*span=(_tmp4C0=_region_malloc(r,sizeof(*_tmp4C0)),((_tmp4C0[0]=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1A4->impl))->fields),_tmp4C0)));int field_name;struct Cyc_List_List*
_tmp1D3=_tmp1A5;struct Cyc_List_List _tmp1D4;struct _tuple8*_tmp1D5;struct _tuple8
_tmp1D6;struct Cyc_List_List*_tmp1D7;struct Cyc_List_List _tmp1D8;void*_tmp1D9;
struct _dyneither_ptr*_tmp1DA;struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_Pat*
_tmp1DC;struct Cyc_List_List*_tmp1DD;_LLFD: if(_tmp1D3 == 0)goto _LLFF;_tmp1D4=*
_tmp1D3;_tmp1D5=(struct _tuple8*)_tmp1D4.hd;_tmp1D6=*_tmp1D5;_tmp1D7=_tmp1D6.f1;
if(_tmp1D7 == 0)goto _LLFF;_tmp1D8=*_tmp1D7;_tmp1D9=(void*)_tmp1D8.hd;if(*((int*)
_tmp1D9)!= 1)goto _LLFF;_tmp1DA=((struct Cyc_Absyn_FieldName_struct*)_tmp1D9)->f1;
_tmp1DB=_tmp1D8.tl;if(_tmp1DB != 0)goto _LLFF;_tmp1DC=_tmp1D6.f2;_tmp1DD=_tmp1D4.tl;
if(_tmp1DD != 0)goto _LLFF;_LLFE:{struct Cyc_List_List*_tmp4C1;s=Cyc_Tcpat_con_pat(
r,*_tmp1DA,span,((_tmp4C1=_region_malloc(r,sizeof(*_tmp4C1)),((_tmp4C1->hd=(void*)
Cyc_Tcpat_compile_pat(r,_tmp1DC),((_tmp4C1->tl=0,_tmp4C1)))))),p);}goto _LLFC;
_LLFF:;_LL100: {const char*_tmp4C4;void*_tmp4C3;(_tmp4C3=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4C4="bad union pattern",
_tag_dyneither(_tmp4C4,sizeof(char),18))),_tag_dyneither(_tmp4C3,sizeof(void*),0)));}
_LLFC:;}}goto _LLB8;_LLD1: if(_tmp195 <= (void*)2)goto _LLD3;if(*((int*)_tmp195)!= 
10)goto _LLD3;_tmp1A6=((struct Cyc_Absyn_Enum_p_struct*)_tmp195)->f1;_tmp1A7=((
struct Cyc_Absyn_Enum_p_struct*)_tmp195)->f2;_LLD2: {int span=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1A6->fields))->v);{int*_tmp4C5;s=Cyc_Tcpat_con_pat(r,*(*_tmp1A7->name).f2,((
_tmp4C5=_region_malloc(r,sizeof(*_tmp4C5)),((_tmp4C5[0]=span,_tmp4C5)))),0,p);}
goto _LLB8;}_LLD3: if(_tmp195 <= (void*)2)goto _LLD5;if(*((int*)_tmp195)!= 11)goto
_LLD5;_tmp1A8=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp195)->f1;_tmp1A9=((
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp195)->f2;_LLD4: {struct Cyc_List_List*
fields;{void*_tmp1E3=Cyc_Tcutil_compress(_tmp1A8);struct Cyc_List_List*_tmp1E4;
_LL102: if(_tmp1E3 <= (void*)4)goto _LL104;if(*((int*)_tmp1E3)!= 13)goto _LL104;
_tmp1E4=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp1E3)->f1;_LL103: fields=
_tmp1E4;goto _LL101;_LL104:;_LL105: {const char*_tmp4C8;void*_tmp4C7;(_tmp4C7=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp4C8="bad type in AnonEnum_p",_tag_dyneither(_tmp4C8,sizeof(char),23))),
_tag_dyneither(_tmp4C7,sizeof(void*),0)));}_LL101:;}{int span=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(fields);{int*_tmp4C9;s=Cyc_Tcpat_con_pat(r,*(*_tmp1A9->name).f2,((
_tmp4C9=_region_malloc(r,sizeof(*_tmp4C9)),((_tmp4C9[0]=span,_tmp4C9)))),0,p);}
goto _LLB8;}}_LLD5: if(_tmp195 <= (void*)2)goto _LLD7;if(*((int*)_tmp195)!= 12)goto
_LLD7;_LLD6: goto _LLD8;_LLD7: if(_tmp195 <= (void*)2)goto _LLD9;if(*((int*)_tmp195)
!= 13)goto _LLD9;_LLD8: goto _LLDA;_LLD9: if(_tmp195 <= (void*)2)goto _LLDB;if(*((int*)
_tmp195)!= 5)goto _LLDB;_LLDA: goto _LLDC;_LLDB: if(_tmp195 <= (void*)2)goto _LLB8;if(*((
int*)_tmp195)!= 14)goto _LLB8;_LLDC: s=(void*)0;_LLB8:;}return s;}struct Cyc_Tcpat_Pos_struct{
int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{
int tag;struct Cyc_Set_Set*f1;};struct Cyc_Tcpat_Failure_struct{int tag;void*f1;};
struct Cyc_Tcpat_Success_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{int
tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct
_tuple11{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*
c);static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*
c){void*_tmp1E8=td;struct Cyc_Set_Set*_tmp1E9;_LL107: if(*((int*)_tmp1E8)!= 1)goto
_LL109;_tmp1E9=((struct Cyc_Tcpat_Neg_struct*)_tmp1E8)->f1;_LL108: {struct Cyc_Tcpat_Neg_struct
_tmp4CC;struct Cyc_Tcpat_Neg_struct*_tmp4CB;return(void*)((_tmp4CB=_region_malloc(
r,sizeof(*_tmp4CB)),((_tmp4CB[0]=((_tmp4CC.tag=1,((_tmp4CC.f1=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(
r,_tmp1E9,c),_tmp4CC)))),_tmp4CB))));}_LL109: if(*((int*)_tmp1E8)!= 0)goto _LL106;
_LL10A: {const char*_tmp4CF;void*_tmp4CE;(_tmp4CE=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4CF="add_neg called when td is Positive",
_tag_dyneither(_tmp4CF,sizeof(char),35))),_tag_dyneither(_tmp4CE,sizeof(void*),0)));}
_LL106:;}static void*Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td);
static void*Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){void*_tmp1EE=
td;struct Cyc_Tcpat_Con_s*_tmp1EF;struct Cyc_Set_Set*_tmp1F0;_LL10C: if(*((int*)
_tmp1EE)!= 0)goto _LL10E;_tmp1EF=((struct Cyc_Tcpat_Pos_struct*)_tmp1EE)->f1;
_LL10D: if(Cyc_Tcpat_compare_con(c,_tmp1EF)== 0)return(void*)0;else{return(void*)
1;}_LL10E: if(*((int*)_tmp1EE)!= 1)goto _LL10B;_tmp1F0=((struct Cyc_Tcpat_Neg_struct*)
_tmp1EE)->f1;_LL10F: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp1F0,c))return(void*)1;else{if(c->span != 0  && *((int*)
_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp1F0)+ 1)return(void*)0;else{return(void*)2;}}_LL10B:;}struct _tuple12{struct
Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*Cyc_Tcpat_augment(
struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc);static struct Cyc_List_List*
Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp1F1=ctxt;struct Cyc_List_List _tmp1F2;struct _tuple12*
_tmp1F3;struct _tuple12 _tmp1F4;struct Cyc_Tcpat_Con_s*_tmp1F5;struct Cyc_List_List*
_tmp1F6;struct Cyc_List_List*_tmp1F7;_LL111: if(_tmp1F1 != 0)goto _LL113;_LL112:
return 0;_LL113: if(_tmp1F1 == 0)goto _LL110;_tmp1F2=*_tmp1F1;_tmp1F3=(struct
_tuple12*)_tmp1F2.hd;_tmp1F4=*_tmp1F3;_tmp1F5=_tmp1F4.f1;_tmp1F6=_tmp1F4.f2;
_tmp1F7=_tmp1F2.tl;_LL114: {struct _tuple12*_tmp4D5;struct Cyc_List_List*_tmp4D4;
struct Cyc_List_List*_tmp4D3;return(_tmp4D3=_region_malloc(r,sizeof(*_tmp4D3)),((
_tmp4D3->hd=((_tmp4D5=_region_malloc(r,sizeof(*_tmp4D5)),((_tmp4D5->f1=_tmp1F5,((
_tmp4D5->f2=(struct Cyc_List_List*)((_tmp4D4=_region_malloc(r,sizeof(*_tmp4D4)),((
_tmp4D4->hd=(void*)dsc,((_tmp4D4->tl=_tmp1F6,_tmp4D4)))))),_tmp4D5)))))),((
_tmp4D3->tl=_tmp1F7,_tmp4D3)))));}_LL110:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt);static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp1FB=ctxt;
struct Cyc_List_List _tmp1FC;struct _tuple12*_tmp1FD;struct _tuple12 _tmp1FE;struct
Cyc_Tcpat_Con_s*_tmp1FF;struct Cyc_List_List*_tmp200;struct Cyc_List_List*_tmp201;
_LL116: if(_tmp1FB != 0)goto _LL118;_LL117: {const char*_tmp4D8;void*_tmp4D7;(
_tmp4D7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp4D8="norm_context: empty context",_tag_dyneither(_tmp4D8,sizeof(char),28))),
_tag_dyneither(_tmp4D7,sizeof(void*),0)));}_LL118: if(_tmp1FB == 0)goto _LL115;
_tmp1FC=*_tmp1FB;_tmp1FD=(struct _tuple12*)_tmp1FC.hd;_tmp1FE=*_tmp1FD;_tmp1FF=
_tmp1FE.f1;_tmp200=_tmp1FE.f2;_tmp201=_tmp1FC.tl;_LL119: {struct Cyc_Tcpat_Pos_struct
_tmp4DB;struct Cyc_Tcpat_Pos_struct*_tmp4DA;return Cyc_Tcpat_augment(r,_tmp201,(
void*)((_tmp4DA=_region_malloc(r,sizeof(*_tmp4DA)),((_tmp4DA[0]=((_tmp4DB.tag=0,((
_tmp4DB.f1=_tmp1FF,((_tmp4DB.f2=Cyc_List_rrev(r,_tmp200),_tmp4DB)))))),_tmp4DA)))));}
_LL115:;}static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*
ctxt,void*dsc,struct Cyc_List_List*work);static void*Cyc_Tcpat_build_desc(struct
_RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
struct _tuple6 _tmp4DC;struct _tuple6 _tmp207=(_tmp4DC.f1=ctxt,((_tmp4DC.f2=work,
_tmp4DC)));struct Cyc_List_List*_tmp208;struct Cyc_List_List*_tmp209;struct Cyc_List_List*
_tmp20A;struct Cyc_List_List*_tmp20B;struct Cyc_List_List*_tmp20C;struct Cyc_List_List
_tmp20D;struct _tuple12*_tmp20E;struct _tuple12 _tmp20F;struct Cyc_Tcpat_Con_s*
_tmp210;struct Cyc_List_List*_tmp211;struct Cyc_List_List*_tmp212;struct Cyc_List_List*
_tmp213;struct Cyc_List_List _tmp214;struct _tuple11*_tmp215;struct _tuple11 _tmp216;
struct Cyc_List_List*_tmp217;struct Cyc_List_List*_tmp218;_LL11B: _tmp208=_tmp207.f1;
if(_tmp208 != 0)goto _LL11D;_tmp209=_tmp207.f2;if(_tmp209 != 0)goto _LL11D;_LL11C:
return dsc;_LL11D: _tmp20A=_tmp207.f1;if(_tmp20A != 0)goto _LL11F;_LL11E: goto _LL120;
_LL11F: _tmp20B=_tmp207.f2;if(_tmp20B != 0)goto _LL121;_LL120: {const char*_tmp4DF;
void*_tmp4DE;(_tmp4DE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp4DF="build_desc: ctxt and work don't match",
_tag_dyneither(_tmp4DF,sizeof(char),38))),_tag_dyneither(_tmp4DE,sizeof(void*),0)));}
_LL121: _tmp20C=_tmp207.f1;if(_tmp20C == 0)goto _LL11A;_tmp20D=*_tmp20C;_tmp20E=(
struct _tuple12*)_tmp20D.hd;_tmp20F=*_tmp20E;_tmp210=_tmp20F.f1;_tmp211=_tmp20F.f2;
_tmp212=_tmp20D.tl;_tmp213=_tmp207.f2;if(_tmp213 == 0)goto _LL11A;_tmp214=*_tmp213;
_tmp215=(struct _tuple11*)_tmp214.hd;_tmp216=*_tmp215;_tmp217=_tmp216.f3;_tmp218=
_tmp214.tl;_LL122: {struct Cyc_Tcpat_Pos_struct _tmp4E5;struct Cyc_List_List*
_tmp4E4;struct Cyc_Tcpat_Pos_struct*_tmp4E3;struct Cyc_Tcpat_Pos_struct*_tmp21B=(
_tmp4E3=_region_malloc(r,sizeof(*_tmp4E3)),((_tmp4E3[0]=((_tmp4E5.tag=0,((
_tmp4E5.f1=_tmp210,((_tmp4E5.f2=Cyc_List_rappend(r,Cyc_List_rrev(r,_tmp211),((
_tmp4E4=_region_malloc(r,sizeof(*_tmp4E4)),((_tmp4E4->hd=(void*)dsc,((_tmp4E4->tl=
_tmp217,_tmp4E4))))))),_tmp4E5)))))),_tmp4E3)));return Cyc_Tcpat_build_desc(r,
_tmp212,(void*)_tmp21B,_tmp218);}_LL11A:;}static void*Cyc_Tcpat_match(struct
_RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,
struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules);static
void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*
allmrules);static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct
Cyc_List_List*allmrules){struct Cyc_List_List*_tmp21F=allmrules;struct Cyc_List_List
_tmp220;struct _tuple0*_tmp221;struct _tuple0 _tmp222;void*_tmp223;void*_tmp224;
struct Cyc_List_List*_tmp225;_LL124: if(_tmp21F != 0)goto _LL126;_LL125: {struct Cyc_Tcpat_Failure_struct
_tmp4E8;struct Cyc_Tcpat_Failure_struct*_tmp4E7;return(void*)((_tmp4E7=_cycalloc(
sizeof(*_tmp4E7)),((_tmp4E7[0]=((_tmp4E8.tag=0,((_tmp4E8.f1=(void*)dsc,_tmp4E8)))),
_tmp4E7))));}_LL126: if(_tmp21F == 0)goto _LL123;_tmp220=*_tmp21F;_tmp221=(struct
_tuple0*)_tmp220.hd;_tmp222=*_tmp221;_tmp223=_tmp222.f1;_tmp224=_tmp222.f2;
_tmp225=_tmp220.tl;_LL127: return Cyc_Tcpat_match(r,_tmp223,0,dsc,0,0,_tmp224,
_tmp225);_LL123:;}static void*Cyc_Tcpat_match_compile(struct _RegionHandle*r,
struct Cyc_List_List*allmrules);static void*Cyc_Tcpat_match_compile(struct
_RegionHandle*r,struct Cyc_List_List*allmrules){struct Cyc_Tcpat_Neg_struct _tmp4EB;
struct Cyc_Tcpat_Neg_struct*_tmp4EA;return Cyc_Tcpat_or_match(r,(void*)((_tmp4EA=
_region_malloc(r,sizeof(*_tmp4EA)),((_tmp4EA[0]=((_tmp4EB.tag=1,((_tmp4EB.f1=Cyc_Tcpat_empty_con_set(
r),_tmp4EB)))),_tmp4EA)))),allmrules);}static void*Cyc_Tcpat_and_match(struct
_RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules);static void*Cyc_Tcpat_and_match(struct
_RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp22A=work;
struct Cyc_List_List _tmp22B;struct _tuple11*_tmp22C;struct _tuple11 _tmp22D;struct
Cyc_List_List*_tmp22E;struct Cyc_List_List*_tmp22F;struct Cyc_List_List*_tmp230;
struct Cyc_List_List*_tmp231;struct Cyc_List_List _tmp232;struct _tuple11*_tmp233;
struct _tuple11 _tmp234;struct Cyc_List_List*_tmp235;struct Cyc_List_List*_tmp236;
struct Cyc_List_List*_tmp237;struct Cyc_List_List*_tmp238;_LL129: if(_tmp22A != 0)
goto _LL12B;_LL12A: {struct Cyc_Tcpat_Success_struct _tmp4EE;struct Cyc_Tcpat_Success_struct*
_tmp4ED;return(void*)((_tmp4ED=_region_malloc(r,sizeof(*_tmp4ED)),((_tmp4ED[0]=((
_tmp4EE.tag=1,((_tmp4EE.f1=(void*)right_hand_side,_tmp4EE)))),_tmp4ED))));}
_LL12B: if(_tmp22A == 0)goto _LL12D;_tmp22B=*_tmp22A;_tmp22C=(struct _tuple11*)
_tmp22B.hd;_tmp22D=*_tmp22C;_tmp22E=_tmp22D.f1;if(_tmp22E != 0)goto _LL12D;_tmp22F=
_tmp22D.f2;if(_tmp22F != 0)goto _LL12D;_tmp230=_tmp22D.f3;if(_tmp230 != 0)goto
_LL12D;_tmp231=_tmp22B.tl;_LL12C: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp231,right_hand_side,rules);_LL12D: if(_tmp22A == 0)goto _LL128;_tmp232=*
_tmp22A;_tmp233=(struct _tuple11*)_tmp232.hd;_tmp234=*_tmp233;_tmp235=_tmp234.f1;
_tmp236=_tmp234.f2;_tmp237=_tmp234.f3;_tmp238=_tmp232.tl;_LL12E: if((_tmp235 == 0
 || _tmp236 == 0) || _tmp237 == 0){const char*_tmp4F1;void*_tmp4F0;(_tmp4F0=0,Cyc_Tcutil_impos(((
_tmp4F1="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp4F1,sizeof(
char),38))),_tag_dyneither(_tmp4F0,sizeof(void*),0)));}{struct Cyc_List_List
_tmp23E;void*_tmp23F;struct Cyc_List_List*_tmp240;struct Cyc_List_List*_tmp23D=(
struct Cyc_List_List*)_tmp235;_tmp23E=*_tmp23D;_tmp23F=(void*)_tmp23E.hd;_tmp240=
_tmp23E.tl;{struct Cyc_List_List _tmp242;struct Cyc_List_List*_tmp243;struct Cyc_List_List*
_tmp244;struct Cyc_List_List*_tmp241=(struct Cyc_List_List*)_tmp236;_tmp242=*
_tmp241;_tmp243=(struct Cyc_List_List*)_tmp242.hd;_tmp244=_tmp242.tl;{struct Cyc_List_List
_tmp246;void*_tmp247;struct Cyc_List_List*_tmp248;struct Cyc_List_List*_tmp245=(
struct Cyc_List_List*)_tmp237;_tmp246=*_tmp245;_tmp247=(void*)_tmp246.hd;_tmp248=
_tmp246.tl;{struct _tuple11*_tmp4F2;struct _tuple11*_tmp249=(_tmp4F2=
_region_malloc(r,sizeof(*_tmp4F2)),((_tmp4F2->f1=_tmp240,((_tmp4F2->f2=_tmp244,((
_tmp4F2->f3=_tmp248,_tmp4F2)))))));struct Cyc_List_List*_tmp4F3;return Cyc_Tcpat_match(
r,_tmp23F,_tmp243,_tmp247,ctx,((_tmp4F3=_region_malloc(r,sizeof(*_tmp4F3)),((
_tmp4F3->hd=_tmp249,((_tmp4F3->tl=_tmp238,_tmp4F3)))))),right_hand_side,rules);}}}}
_LL128:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,
struct Cyc_Tcpat_Con_s*pcon,void*dsc);static struct Cyc_List_List*Cyc_Tcpat_getdargs(
struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp24C=dsc;
struct Cyc_Set_Set*_tmp24D;struct Cyc_List_List*_tmp24E;_LL130: if(*((int*)_tmp24C)
!= 1)goto _LL132;_tmp24D=((struct Cyc_Tcpat_Neg_struct*)_tmp24C)->f1;_LL131: {
struct Cyc_Tcpat_Neg_struct _tmp4F6;struct Cyc_Tcpat_Neg_struct*_tmp4F5;void*any=(
void*)((_tmp4F5=_region_malloc(r,sizeof(*_tmp4F5)),((_tmp4F5[0]=((_tmp4F6.tag=1,((
_tmp4F6.f1=Cyc_Tcpat_empty_con_set(r),_tmp4F6)))),_tmp4F5))));struct Cyc_List_List*
_tmp24F=0;{int i=0;for(0;i < pcon->arity;++ i){struct Cyc_List_List*_tmp4F7;_tmp24F=((
_tmp4F7=_region_malloc(r,sizeof(*_tmp4F7)),((_tmp4F7->hd=(void*)any,((_tmp4F7->tl=
_tmp24F,_tmp4F7))))));}}return _tmp24F;}_LL132: if(*((int*)_tmp24C)!= 0)goto _LL12F;
_tmp24E=((struct Cyc_Tcpat_Pos_struct*)_tmp24C)->f2;_LL133: return _tmp24E;_LL12F:;}
struct _tuple13{struct _RegionHandle*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*
Cyc_Tcpat_getoarg(struct _tuple13*env,int i);static struct Cyc_List_List*Cyc_Tcpat_getoarg(
struct _tuple13*env,int i){struct _tuple13 _tmp254;struct _RegionHandle*_tmp255;
struct Cyc_List_List*_tmp256;struct _tuple13*_tmp253=env;_tmp254=*_tmp253;_tmp255=
_tmp254.f1;_tmp256=_tmp254.f2;{struct Cyc_List_List*_tmp4F8;return(_tmp4F8=
_region_malloc(_tmp255,sizeof(*_tmp4F8)),((_tmp4F8->hd=(void*)(i + 1),((_tmp4F8->tl=
_tmp256,_tmp4F8)))));}}static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct
_RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj);static struct
Cyc_List_List*Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*
pcon,struct Cyc_List_List*obj){struct _tuple13 _tmp4F9;struct _tuple13 _tmp258=(
_tmp4F9.f1=r,((_tmp4F9.f2=obj,_tmp4F9)));return((struct Cyc_List_List*(*)(struct
_RegionHandle*r,int n,struct Cyc_List_List*(*f)(struct _tuple13*,int),struct
_tuple13*env))Cyc_List_rtabulate_c)(r,pcon->arity,Cyc_Tcpat_getoarg,& _tmp258);}
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,
void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,
struct Cyc_List_List*rules);static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*
p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp25A=p;struct Cyc_Tcpat_Con_s*
_tmp25B;struct Cyc_List_List*_tmp25C;_LL135: if((int)_tmp25A != 0)goto _LL137;_LL136:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL137: if(_tmp25A <= (void*)1)goto _LL134;if(*((int*)_tmp25A)!= 0)goto
_LL134;_tmp25B=((struct Cyc_Tcpat_Con_struct*)_tmp25A)->f1;_tmp25C=((struct Cyc_Tcpat_Con_struct*)
_tmp25A)->f2;_LL138: {void*_tmp25D=Cyc_Tcpat_static_match(_tmp25B,dsc);_LL13A:
if((int)_tmp25D != 0)goto _LL13C;_LL13B: {struct _tuple12*_tmp4FC;struct Cyc_List_List*
_tmp4FB;struct Cyc_List_List*ctx2=(_tmp4FB=_region_malloc(r,sizeof(*_tmp4FB)),((
_tmp4FB->hd=((_tmp4FC=_region_malloc(r,sizeof(*_tmp4FC)),((_tmp4FC->f1=_tmp25B,((
_tmp4FC->f2=0,_tmp4FC)))))),((_tmp4FB->tl=ctx,_tmp4FB)))));struct _tuple11*
_tmp4FD;struct _tuple11*work_frame=(_tmp4FD=_region_malloc(r,sizeof(*_tmp4FD)),((
_tmp4FD->f1=_tmp25C,((_tmp4FD->f2=Cyc_Tcpat_getoargs(r,_tmp25B,obj),((_tmp4FD->f3=
Cyc_Tcpat_getdargs(r,_tmp25B,dsc),_tmp4FD)))))));struct Cyc_List_List*_tmp4FE;
struct Cyc_List_List*work2=(_tmp4FE=_region_malloc(r,sizeof(*_tmp4FE)),((_tmp4FE->hd=
work_frame,((_tmp4FE->tl=work,_tmp4FE)))));return Cyc_Tcpat_and_match(r,ctx2,
work2,right_hand_side,rules);}_LL13C: if((int)_tmp25D != 1)goto _LL13E;_LL13D:
return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);_LL13E:
if((int)_tmp25D != 2)goto _LL139;_LL13F: {struct _tuple12*_tmp501;struct Cyc_List_List*
_tmp500;struct Cyc_List_List*ctx2=(_tmp500=_region_malloc(r,sizeof(*_tmp500)),((
_tmp500->hd=((_tmp501=_region_malloc(r,sizeof(*_tmp501)),((_tmp501->f1=_tmp25B,((
_tmp501->f2=0,_tmp501)))))),((_tmp500->tl=ctx,_tmp500)))));struct _tuple11*
_tmp502;struct _tuple11*work_frame=(_tmp502=_region_malloc(r,sizeof(*_tmp502)),((
_tmp502->f1=_tmp25C,((_tmp502->f2=Cyc_Tcpat_getoargs(r,_tmp25B,obj),((_tmp502->f3=
Cyc_Tcpat_getdargs(r,_tmp25B,dsc),_tmp502)))))));struct Cyc_List_List*_tmp503;
struct Cyc_List_List*work2=(_tmp503=_region_malloc(r,sizeof(*_tmp503)),((_tmp503->hd=
work_frame,((_tmp503->tl=work,_tmp503)))));void*_tmp262=Cyc_Tcpat_and_match(r,
ctx2,work2,right_hand_side,rules);void*_tmp263=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(
r,ctx,Cyc_Tcpat_add_neg(r,dsc,_tmp25B),work),rules);struct Cyc_Tcpat_IfEq_struct
_tmp506;struct Cyc_Tcpat_IfEq_struct*_tmp505;return(void*)((_tmp505=
_region_malloc(r,sizeof(*_tmp505)),((_tmp505[0]=((_tmp506.tag=2,((_tmp506.f1=obj,((
_tmp506.f2=_tmp25B,((_tmp506.f3=(void*)_tmp262,((_tmp506.f4=(void*)_tmp263,
_tmp506)))))))))),_tmp505))));}_LL139:;}_LL134:;}static void Cyc_Tcpat_check_exhaust_overlap(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),
void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done);static void
Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*not_exhaust)(
struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,void*),void*
env2,int*exhaust_done){void*_tmp26A=d;void*_tmp26B;void*_tmp26C;void*_tmp26D;
void*_tmp26E;_LL141: if(*((int*)_tmp26A)!= 0)goto _LL143;_tmp26B=(void*)((struct
Cyc_Tcpat_Failure_struct*)_tmp26A)->f1;_LL142: if(!(*exhaust_done)){not_exhaust(r,
env1,_tmp26B);*exhaust_done=1;}goto _LL140;_LL143: if(*((int*)_tmp26A)!= 1)goto
_LL145;_tmp26C=(void*)((struct Cyc_Tcpat_Success_struct*)_tmp26A)->f1;_LL144:
rhs_appears(env2,_tmp26C);goto _LL140;_LL145: if(*((int*)_tmp26A)!= 2)goto _LL140;
_tmp26D=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp26A)->f3;_tmp26E=(void*)((
struct Cyc_Tcpat_IfEq_struct*)_tmp26A)->f4;_LL146: Cyc_Tcpat_check_exhaust_overlap(
r,_tmp26D,not_exhaust,env1,rhs_appears,env2,exhaust_done);Cyc_Tcpat_check_exhaust_overlap(
r,_tmp26E,not_exhaust,env1,rhs_appears,env2,exhaust_done);goto _LL140;_LL140:;}
struct _tuple14{int f1;struct Cyc_Position_Segment*f2;};struct _tuple15{void*f1;
struct _tuple14*f2;};struct _tuple16{struct _RegionHandle*f1;int*f2;};static struct
_tuple15*Cyc_Tcpat_get_match(struct _tuple16*env,struct Cyc_Absyn_Switch_clause*
swc);static struct _tuple15*Cyc_Tcpat_get_match(struct _tuple16*env,struct Cyc_Absyn_Switch_clause*
swc){struct _tuple16 _tmp270;struct _RegionHandle*_tmp271;int*_tmp272;struct
_tuple16*_tmp26F=env;_tmp270=*_tmp26F;_tmp271=_tmp270.f1;_tmp272=_tmp270.f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp271,swc->pattern);struct _tuple14*_tmp507;
struct _tuple14*rhs=(_tmp507=_region_malloc(_tmp271,sizeof(*_tmp507)),((_tmp507->f1=
0,((_tmp507->f2=(swc->pattern)->loc,_tmp507)))));void*sp;if(swc->where_clause != 
0){{struct Cyc_List_List*_tmp50A;struct Cyc_List_List*_tmp509;sp=Cyc_Tcpat_tuple_pat(
_tmp271,((_tmp509=_region_malloc(_tmp271,sizeof(*_tmp509)),((_tmp509->hd=(void*)
sp0,((_tmp509->tl=((_tmp50A=_region_malloc(_tmp271,sizeof(*_tmp50A)),((_tmp50A->hd=(
void*)Cyc_Tcpat_int_pat(_tmp271,*_tmp272,0),((_tmp50A->tl=0,_tmp50A)))))),
_tmp509)))))),0);}*_tmp272=*_tmp272 + 1;}else{struct Cyc_List_List*_tmp50D;struct
Cyc_List_List*_tmp50C;sp=Cyc_Tcpat_tuple_pat(_tmp271,((_tmp50C=_region_malloc(
_tmp271,sizeof(*_tmp50C)),((_tmp50C->hd=(void*)sp0,((_tmp50C->tl=((_tmp50D=
_region_malloc(_tmp271,sizeof(*_tmp50D)),((_tmp50D->hd=(void*)((void*)0),((
_tmp50D->tl=0,_tmp50D)))))),_tmp50C)))))),0);}{struct _tuple15*_tmp50E;return(
_tmp50E=_region_malloc(_tmp271,sizeof(*_tmp50E)),((_tmp50E->f1=sp,((_tmp50E->f2=
rhs,_tmp50E)))));}}}char Cyc_Tcpat_Desc2string[16]="\000\000\000\000Desc2string\000";
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(struct _RegionHandle*r,void*d);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(struct _RegionHandle*r,void*d){
void*_tmp279=d;struct Cyc_Tcpat_Con_s*_tmp27A;struct Cyc_List_List*_tmp27B;struct
Cyc_Set_Set*_tmp27C;_LL148: if(*((int*)_tmp279)!= 0)goto _LL14A;_tmp27A=((struct
Cyc_Tcpat_Pos_struct*)_tmp279)->f1;_tmp27B=((struct Cyc_Tcpat_Pos_struct*)_tmp279)->f2;
_LL149: {union Cyc_Tcpat_Name_value_union _tmp27D=_tmp27A->name;struct Cyc_Absyn_Pat*
_tmp27E=_tmp27A->orig_pat;if(_tmp27E == 0)return Cyc_Tcpat_desc2string(r,(void*)((
struct Cyc_List_List*)_check_null(_tmp27B))->hd);{void*_tmp27F=(void*)_tmp27E->r;
struct Cyc_Absyn_Vardecl*_tmp280;struct Cyc_Absyn_Vardecl*_tmp281;struct Cyc_Absyn_Tvar*
_tmp282;struct Cyc_Absyn_Vardecl*_tmp283;struct Cyc_Absyn_Pat*_tmp284;struct Cyc_Absyn_Pat
_tmp285;void*_tmp286;struct Cyc_Absyn_Datatypefield*_tmp287;struct Cyc_Absyn_AggrInfo
_tmp288;union Cyc_Absyn_AggrInfoU_union _tmp289;struct Cyc_Absyn_Aggrdecl**_tmp28A;
struct Cyc_Absyn_Aggrdecl*_tmp28B;struct Cyc_List_List*_tmp28C;struct Cyc_Absyn_Datatypefield*
_tmp28D;int _tmp28E;char _tmp28F;struct _dyneither_ptr _tmp290;struct Cyc_Absyn_Enumfield*
_tmp291;struct Cyc_Absyn_Enumfield*_tmp292;struct Cyc_Absyn_Exp*_tmp293;_LL14D: if((
int)_tmp27F != 0)goto _LL14F;_LL14E: {const char*_tmp50F;return(_tmp50F="_",
_tag_dyneither(_tmp50F,sizeof(char),2));}_LL14F: if(_tmp27F <= (void*)2)goto _LL15F;
if(*((int*)_tmp27F)!= 0)goto _LL151;_tmp280=((struct Cyc_Absyn_Var_p_struct*)
_tmp27F)->f1;_LL150: return Cyc_Absynpp_qvar2string(_tmp280->name);_LL151: if(*((
int*)_tmp27F)!= 1)goto _LL153;_tmp281=((struct Cyc_Absyn_Reference_p_struct*)
_tmp27F)->f1;_LL152: {const char*_tmp513;void*_tmp512[1];struct Cyc_String_pa_struct
_tmp511;return(struct _dyneither_ptr)((_tmp511.tag=0,((_tmp511.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp281->name)),((
_tmp512[0]=& _tmp511,Cyc_aprintf(((_tmp513="*%s",_tag_dyneither(_tmp513,sizeof(
char),4))),_tag_dyneither(_tmp512,sizeof(void*),1))))))));}_LL153: if(*((int*)
_tmp27F)!= 2)goto _LL155;_tmp282=((struct Cyc_Absyn_TagInt_p_struct*)_tmp27F)->f1;
_tmp283=((struct Cyc_Absyn_TagInt_p_struct*)_tmp27F)->f2;_LL154: {const char*
_tmp518;void*_tmp517[2];struct Cyc_String_pa_struct _tmp516;struct Cyc_String_pa_struct
_tmp515;return(struct _dyneither_ptr)((_tmp515.tag=0,((_tmp515.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp282->name),((_tmp516.tag=0,((_tmp516.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp283->name)),((
_tmp517[0]=& _tmp516,((_tmp517[1]=& _tmp515,Cyc_aprintf(((_tmp518="%s<`%s>",
_tag_dyneither(_tmp518,sizeof(char),8))),_tag_dyneither(_tmp517,sizeof(void*),2))))))))))))));}
_LL155: if(*((int*)_tmp27F)!= 3)goto _LL157;_LL156: {const char*_tmp51C;void*
_tmp51B[1];struct Cyc_String_pa_struct _tmp51A;return(struct _dyneither_ptr)((
_tmp51A.tag=0,((_tmp51A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp27B)),((_tmp51B[0]=& _tmp51A,Cyc_aprintf(((_tmp51C="$(%s)",_tag_dyneither(
_tmp51C,sizeof(char),6))),_tag_dyneither(_tmp51B,sizeof(void*),1))))))));}_LL157:
if(*((int*)_tmp27F)!= 4)goto _LL159;_tmp284=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp27F)->f1;_tmp285=*_tmp284;_tmp286=(void*)_tmp285.r;if(_tmp286 <= (void*)2)
goto _LL159;if(*((int*)_tmp286)!= 6)goto _LL159;_tmp287=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp286)->f2;_LL158: {const char*_tmp521;void*_tmp520[2];struct Cyc_String_pa_struct
_tmp51F;struct Cyc_String_pa_struct _tmp51E;return(struct _dyneither_ptr)((_tmp51E.tag=
0,((_tmp51E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp27B)),((_tmp51F.tag=0,((_tmp51F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp287->name)),((_tmp520[0]=& _tmp51F,((
_tmp520[1]=& _tmp51E,Cyc_aprintf(((_tmp521="&%s(%s)",_tag_dyneither(_tmp521,
sizeof(char),8))),_tag_dyneither(_tmp520,sizeof(void*),2))))))))))))));}_LL159:
if(*((int*)_tmp27F)!= 4)goto _LL15B;_LL15A: {const char*_tmp525;void*_tmp524[1];
struct Cyc_String_pa_struct _tmp523;return(struct _dyneither_ptr)((_tmp523.tag=0,((
_tmp523.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,
_tmp27B)),((_tmp524[0]=& _tmp523,Cyc_aprintf(((_tmp525="&%s",_tag_dyneither(
_tmp525,sizeof(char),4))),_tag_dyneither(_tmp524,sizeof(void*),1))))))));}_LL15B:
if(*((int*)_tmp27F)!= 5)goto _LL15D;_tmp288=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp27F)->f1;_tmp289=_tmp288.aggr_info;if((((((struct Cyc_Absyn_Aggr_p_struct*)
_tmp27F)->f1).aggr_info).KnownAggr).tag != 1)goto _LL15D;_tmp28A=(_tmp289.KnownAggr).f1;
_tmp28B=*_tmp28A;_tmp28C=((struct Cyc_Absyn_Aggr_p_struct*)_tmp27F)->f3;_LL15C:
if((void*)_tmp28B->kind == (void*)1){struct Cyc_List_List*_tmp2A6=_tmp28C;struct
Cyc_List_List _tmp2A7;struct _tuple8*_tmp2A8;struct _tuple8 _tmp2A9;struct Cyc_List_List*
_tmp2AA;struct Cyc_List_List _tmp2AB;void*_tmp2AC;struct _dyneither_ptr*_tmp2AD;
_LL170: if(_tmp2A6 == 0)goto _LL172;_tmp2A7=*_tmp2A6;_tmp2A8=(struct _tuple8*)
_tmp2A7.hd;_tmp2A9=*_tmp2A8;_tmp2AA=_tmp2A9.f1;if(_tmp2AA == 0)goto _LL172;_tmp2AB=*
_tmp2AA;_tmp2AC=(void*)_tmp2AB.hd;if(*((int*)_tmp2AC)!= 1)goto _LL172;_tmp2AD=((
struct Cyc_Absyn_FieldName_struct*)_tmp2AC)->f1;_LL171: {const char*_tmp52B;void*
_tmp52A[3];struct Cyc_String_pa_struct _tmp529;struct Cyc_String_pa_struct _tmp528;
struct Cyc_String_pa_struct _tmp527;return(struct _dyneither_ptr)((_tmp527.tag=0,((
_tmp527.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,
_tmp27B)),((_tmp528.tag=0,((_tmp528.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp2AD),((_tmp529.tag=0,((_tmp529.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp28B->name)),((_tmp52A[0]=&
_tmp529,((_tmp52A[1]=& _tmp528,((_tmp52A[2]=& _tmp527,Cyc_aprintf(((_tmp52B="%s{.%s = %s}",
_tag_dyneither(_tmp52B,sizeof(char),13))),_tag_dyneither(_tmp52A,sizeof(void*),3))))))))))))))))))));}
_LL172:;_LL173: goto _LL16F;_LL16F:;}{const char*_tmp530;void*_tmp52F[2];struct Cyc_String_pa_struct
_tmp52E;struct Cyc_String_pa_struct _tmp52D;return(struct _dyneither_ptr)((_tmp52D.tag=
0,((_tmp52D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp27B)),((_tmp52E.tag=0,((_tmp52E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp28B->name)),((_tmp52F[0]=& _tmp52E,((
_tmp52F[1]=& _tmp52D,Cyc_aprintf(((_tmp530="%s{%s}",_tag_dyneither(_tmp530,
sizeof(char),7))),_tag_dyneither(_tmp52F,sizeof(void*),2))))))))))))));}_LL15D:
if(*((int*)_tmp27F)!= 6)goto _LL15F;_tmp28D=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp27F)->f2;_LL15E: {const char*_tmp534;void*_tmp533[1];struct Cyc_String_pa_struct
_tmp532;return(struct _dyneither_ptr)((_tmp532.tag=0,((_tmp532.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp28D->name)),((
_tmp533[0]=& _tmp532,Cyc_aprintf(((_tmp534="%s",_tag_dyneither(_tmp534,sizeof(
char),3))),_tag_dyneither(_tmp533,sizeof(void*),1))))))));}_LL15F: if((int)
_tmp27F != 1)goto _LL161;_LL160: {const char*_tmp535;return(_tmp535="NULL",
_tag_dyneither(_tmp535,sizeof(char),5));}_LL161: if(_tmp27F <= (void*)2)goto _LL16D;
if(*((int*)_tmp27F)!= 7)goto _LL163;_tmp28E=((struct Cyc_Absyn_Int_p_struct*)
_tmp27F)->f2;_LL162: {const char*_tmp539;void*_tmp538[1];struct Cyc_Int_pa_struct
_tmp537;return(struct _dyneither_ptr)((_tmp537.tag=1,((_tmp537.f1=(unsigned long)
_tmp28E,((_tmp538[0]=& _tmp537,Cyc_aprintf(((_tmp539="%d",_tag_dyneither(_tmp539,
sizeof(char),3))),_tag_dyneither(_tmp538,sizeof(void*),1))))))));}_LL163: if(*((
int*)_tmp27F)!= 8)goto _LL165;_tmp28F=((struct Cyc_Absyn_Char_p_struct*)_tmp27F)->f1;
_LL164: {const char*_tmp53D;void*_tmp53C[1];struct Cyc_Int_pa_struct _tmp53B;return(
struct _dyneither_ptr)((_tmp53B.tag=1,((_tmp53B.f1=(unsigned long)((int)_tmp28F),((
_tmp53C[0]=& _tmp53B,Cyc_aprintf(((_tmp53D="%d",_tag_dyneither(_tmp53D,sizeof(
char),3))),_tag_dyneither(_tmp53C,sizeof(void*),1))))))));}_LL165: if(*((int*)
_tmp27F)!= 9)goto _LL167;_tmp290=((struct Cyc_Absyn_Float_p_struct*)_tmp27F)->f1;
_LL166: return _tmp290;_LL167: if(*((int*)_tmp27F)!= 10)goto _LL169;_tmp291=((struct
Cyc_Absyn_Enum_p_struct*)_tmp27F)->f2;_LL168: _tmp292=_tmp291;goto _LL16A;_LL169:
if(*((int*)_tmp27F)!= 11)goto _LL16B;_tmp292=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp27F)->f2;_LL16A: return Cyc_Absynpp_qvar2string(_tmp292->name);_LL16B: if(*((
int*)_tmp27F)!= 14)goto _LL16D;_tmp293=((struct Cyc_Absyn_Exp_p_struct*)_tmp27F)->f1;
_LL16C: return Cyc_Absynpp_exp2string(_tmp293);_LL16D:;_LL16E:(int)_throw((void*)
Cyc_Tcpat_Desc2string);_LL14C:;}}_LL14A: if(*((int*)_tmp279)!= 1)goto _LL147;
_tmp27C=((struct Cyc_Tcpat_Neg_struct*)_tmp279)->f1;_LL14B: if(((int(*)(struct Cyc_Set_Set*
s))Cyc_Set_is_empty)(_tmp27C)){const char*_tmp53E;return(_tmp53E="_",
_tag_dyneither(_tmp53E,sizeof(char),2));}{struct Cyc_Tcpat_Con_s*_tmp2C2=((struct
Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp27C);struct Cyc_Absyn_Pat*
_tmp2C3=_tmp2C2->orig_pat;if(_tmp2C3 == 0)(int)_throw((void*)Cyc_Tcpat_Desc2string);{
void*_tmp2C4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp2C3->topt))->v);void*_tmp2C5;struct Cyc_Absyn_PtrInfo _tmp2C6;struct Cyc_Absyn_PtrAtts
_tmp2C7;struct Cyc_Absyn_DatatypeInfo _tmp2C8;union Cyc_Absyn_DatatypeInfoU_union
_tmp2C9;struct Cyc_Absyn_Datatypedecl**_tmp2CA;struct Cyc_Absyn_Datatypedecl*
_tmp2CB;struct Cyc_Absyn_AggrInfo _tmp2CC;union Cyc_Absyn_AggrInfoU_union _tmp2CD;
struct Cyc_Absyn_Aggrdecl**_tmp2CE;struct Cyc_Absyn_Aggrdecl*_tmp2CF;_LL175: if(
_tmp2C4 <= (void*)4)goto _LL17F;if(*((int*)_tmp2C4)!= 5)goto _LL177;_tmp2C5=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp2C4)->f2;if((int)_tmp2C5 != 0)goto _LL177;
_LL176:{int i=0;for(0;i < 256;++ i){struct Cyc_Tcpat_Con_s*_tmp2D0=Cyc_Tcpat_char_con(
r,(char)i,(struct Cyc_Absyn_Pat*)_tmp2C3);if(!((int(*)(struct Cyc_Set_Set*s,struct
Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp27C,_tmp2D0)){const char*_tmp542;void*
_tmp541[1];struct Cyc_Int_pa_struct _tmp540;return(struct _dyneither_ptr)((_tmp540.tag=
1,((_tmp540.f1=(unsigned long)i,((_tmp541[0]=& _tmp540,Cyc_aprintf(((_tmp542="%d",
_tag_dyneither(_tmp542,sizeof(char),3))),_tag_dyneither(_tmp541,sizeof(void*),1))))))));}}}(
int)_throw((void*)Cyc_Tcpat_Desc2string);_LL177: if(*((int*)_tmp2C4)!= 5)goto
_LL179;_LL178:{unsigned int i=0;for(0;i < 0 - 1;++ i){struct Cyc_Tcpat_Con_s*_tmp2D4=
Cyc_Tcpat_int_con(r,(int)i,_tmp2C3);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp27C,_tmp2D4)){const char*_tmp546;void*_tmp545[1];struct
Cyc_Int_pa_struct _tmp544;return(struct _dyneither_ptr)((_tmp544.tag=1,((_tmp544.f1=(
unsigned long)((int)i),((_tmp545[0]=& _tmp544,Cyc_aprintf(((_tmp546="%d",
_tag_dyneither(_tmp546,sizeof(char),3))),_tag_dyneither(_tmp545,sizeof(void*),1))))))));}}}(
int)_throw((void*)Cyc_Tcpat_Desc2string);_LL179: if(*((int*)_tmp2C4)!= 4)goto
_LL17B;_tmp2C6=((struct Cyc_Absyn_PointerType_struct*)_tmp2C4)->f1;_tmp2C7=
_tmp2C6.ptr_atts;_LL17A: {struct Cyc_Absyn_Conref*_tmp2D8=_tmp2C7.nullable;int
is_nullable=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp2D8);if(is_nullable){if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp27C,Cyc_Tcpat_null_con(r,(struct Cyc_Absyn_Pat*)_tmp2C3))){
const char*_tmp547;return(_tmp547="NULL",_tag_dyneither(_tmp547,sizeof(char),5));}}{
const char*_tmp548;return(_tmp548="&_",_tag_dyneither(_tmp548,sizeof(char),3));}}
_LL17B: if(*((int*)_tmp2C4)!= 2)goto _LL17D;_tmp2C8=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp2C4)->f1;_tmp2C9=_tmp2C8.datatype_info;if((((((struct Cyc_Absyn_DatatypeType_struct*)
_tmp2C4)->f1).datatype_info).KnownDatatype).tag != 1)goto _LL17D;_tmp2CA=(_tmp2C9.KnownDatatype).f1;
_tmp2CB=*_tmp2CA;_LL17C: if(_tmp2CB->is_extensible)(int)_throw((void*)Cyc_Tcpat_Desc2string);{
struct Cyc_List_List*_tmp2DB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2CB->fields))->v;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2DB);for(0;(unsigned int)_tmp2DB;_tmp2DB=_tmp2DB->tl){struct _dyneither_ptr n=*(*((
struct Cyc_Absyn_Datatypefield*)_tmp2DB->hd)->name).f2;struct Cyc_List_List*
_tmp2DC=((struct Cyc_Absyn_Datatypefield*)_tmp2DB->hd)->typs;union Cyc_Tcpat_Name_value_union
_tmp54B;struct Cyc_Tcpat_Con_s*_tmp54A;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp27C,(struct Cyc_Tcpat_Con_s*)((_tmp54A=_cycalloc(sizeof(*
_tmp54A)),((_tmp54A->name=(union Cyc_Tcpat_Name_value_union)(((_tmp54B.Name_v).tag=
0,(((_tmp54B.Name_v).f1=n,_tmp54B)))),((_tmp54A->arity=0,((_tmp54A->span=0,((
_tmp54A->orig_pat=_tmp2C3,_tmp54A)))))))))))){if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp2DC)== 0)return n;else{if(_tmp2CB->is_flat){const char*
_tmp54F;void*_tmp54E[1];struct Cyc_String_pa_struct _tmp54D;return(struct
_dyneither_ptr)((_tmp54D.tag=0,((_tmp54D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)n),((_tmp54E[0]=& _tmp54D,Cyc_aprintf(((_tmp54F="%s(...)",
_tag_dyneither(_tmp54F,sizeof(char),8))),_tag_dyneither(_tmp54E,sizeof(void*),1))))))));}
else{const char*_tmp553;void*_tmp552[1];struct Cyc_String_pa_struct _tmp551;return(
struct _dyneither_ptr)((_tmp551.tag=0,((_tmp551.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)n),((_tmp552[0]=& _tmp551,Cyc_aprintf(((_tmp553="&%s(...)",
_tag_dyneither(_tmp553,sizeof(char),9))),_tag_dyneither(_tmp552,sizeof(void*),1))))))));}}}}(
int)_throw((void*)Cyc_Tcpat_Desc2string);}_LL17D: if(*((int*)_tmp2C4)!= 10)goto
_LL17F;_tmp2CC=((struct Cyc_Absyn_AggrType_struct*)_tmp2C4)->f1;_tmp2CD=_tmp2CC.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp2C4)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL17F;_tmp2CE=(_tmp2CD.KnownAggr).f1;_tmp2CF=*_tmp2CE;if(!((void*)
_tmp2CF->kind == (void*)1))goto _LL17F;_LL17E: {struct Cyc_List_List*_tmp2E5=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2CF->impl))->fields;int span=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2E5);struct _tuple1 _tmp2E7;struct
_dyneither_ptr*_tmp2E8;struct _dyneither_ptr _tmp2E9;struct _tuple1*_tmp2E6=_tmp2CF->name;
_tmp2E7=*_tmp2E6;_tmp2E8=_tmp2E7.f2;_tmp2E9=*_tmp2E8;for(0;(unsigned int)_tmp2E5;
_tmp2E5=_tmp2E5->tl){struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp2E5->hd)->name;
union Cyc_Tcpat_Name_value_union _tmp556;struct Cyc_Tcpat_Con_s*_tmp555;if(!((int(*)(
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp27C,(struct
Cyc_Tcpat_Con_s*)((_tmp555=_cycalloc(sizeof(*_tmp555)),((_tmp555->name=(union Cyc_Tcpat_Name_value_union)(((
_tmp556.Name_v).tag=0,(((_tmp556.Name_v).f1=n,_tmp556)))),((_tmp555->arity=0,((
_tmp555->span=0,((_tmp555->orig_pat=_tmp2C3,_tmp555)))))))))))){const char*
_tmp55B;void*_tmp55A[2];struct Cyc_String_pa_struct _tmp559;struct Cyc_String_pa_struct
_tmp558;return(struct _dyneither_ptr)((_tmp558.tag=0,((_tmp558.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)n),((_tmp559.tag=0,((_tmp559.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp2E9),((_tmp55A[0]=& _tmp559,((_tmp55A[1]=&
_tmp558,Cyc_aprintf(((_tmp55B="%s{.%s = _}",_tag_dyneither(_tmp55B,sizeof(char),
12))),_tag_dyneither(_tmp55A,sizeof(void*),2))))))))))))));}}(int)_throw((void*)
Cyc_Tcpat_Desc2string);}_LL17F:;_LL180:(int)_throw((void*)Cyc_Tcpat_Desc2string);
_LL174:;}}_LL147:;}static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct
_RegionHandle*r,void*d);static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(
struct _RegionHandle*r,void*d){struct _dyneither_ptr*_tmp55C;return(_tmp55C=
_cycalloc(sizeof(*_tmp55C)),((_tmp55C[0]=Cyc_Tcpat_desc2string(r,d),_tmp55C)));}
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*
ds);static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,
struct Cyc_List_List*ds){struct Cyc_List_List*_tmp2F1=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,
r,ds);struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),",",sizeof(char),2);{struct Cyc_List_List*_tmp2F2=_tmp2F1;for(0;
_tmp2F2 != 0;_tmp2F2=((struct Cyc_List_List*)_check_null(_tmp2F2))->tl){if(_tmp2F2->tl
!= 0){{struct Cyc_List_List*_tmp55D;_tmp2F2->tl=(struct Cyc_List_List*)((_tmp55D=
_cycalloc(sizeof(*_tmp55D)),((_tmp55D->hd=comma,((_tmp55D->tl=_tmp2F2->tl,
_tmp55D))))));}_tmp2F2=_tmp2F2->tl;}}}return(struct _dyneither_ptr)Cyc_strconcat_l(
_tmp2F1);}static void Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,void*desc);static void Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,struct
Cyc_Position_Segment*loc,void*desc){struct _handler_cons _tmp2F5;_push_handler(&
_tmp2F5);{int _tmp2F7=0;if(setjmp(_tmp2F5.handler))_tmp2F7=1;if(!_tmp2F7){{struct
_dyneither_ptr _tmp2F8=Cyc_Tcpat_desc2string(r,desc);const char*_tmp561;void*
_tmp560[1];struct Cyc_String_pa_struct _tmp55F;(_tmp55F.tag=0,((_tmp55F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp2F8),((_tmp560[0]=& _tmp55F,Cyc_Tcutil_terr(
loc,((_tmp561="patterns may not be exhaustive.\n\tmissing case for %s",
_tag_dyneither(_tmp561,sizeof(char),53))),_tag_dyneither(_tmp560,sizeof(void*),1)))))));};
_pop_handler();}else{void*_tmp2F6=(void*)_exn_thrown;void*_tmp2FD=_tmp2F6;_LL182:
if(_tmp2FD != Cyc_Tcpat_Desc2string)goto _LL184;_LL183:{const char*_tmp564;void*
_tmp563;(_tmp563=0,Cyc_Tcutil_terr(loc,((_tmp564="patterns may not be exhaustive.",
_tag_dyneither(_tmp564,sizeof(char),32))),_tag_dyneither(_tmp563,sizeof(void*),0)));}
goto _LL181;_LL184:;_LL185:(void)_throw(_tmp2FD);_LL181:;}}}static void Cyc_Tcpat_rule_occurs(
int dummy,struct _tuple14*rhs);static void Cyc_Tcpat_rule_occurs(int dummy,struct
_tuple14*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs);void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
struct _RegionHandle*_tmp300=Cyc_Tcenv_get_fnrgn(te);int _tmp301=0;struct _tuple16
_tmp565;struct _tuple16 _tmp302=(_tmp565.f1=_tmp300,((_tmp565.f2=& _tmp301,_tmp565)));
struct Cyc_List_List*_tmp303=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple15*(*f)(struct _tuple16*,struct Cyc_Absyn_Switch_clause*),struct
_tuple16*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp300,Cyc_Tcpat_get_match,&
_tmp302,swcs);void*_tmp304=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(_tmp300,_tmp303);int _tmp305=0;((void(*)(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple14*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp300,_tmp304,Cyc_Tcpat_not_exhaust_err,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp305);for(0;_tmp303 != 0;_tmp303=_tmp303->tl){
struct _tuple15 _tmp307;struct _tuple14*_tmp308;struct _tuple14 _tmp309;int _tmp30A;
struct Cyc_Position_Segment*_tmp30B;struct _tuple15*_tmp306=(struct _tuple15*)
_tmp303->hd;_tmp307=*_tmp306;_tmp308=_tmp307.f2;_tmp309=*_tmp308;_tmp30A=_tmp309.f1;
_tmp30B=_tmp309.f2;if(!_tmp30A){const char*_tmp568;void*_tmp567;(_tmp567=0,Cyc_Tcutil_terr(
_tmp30B,((_tmp568="redundant pattern",_tag_dyneither(_tmp568,sizeof(char),18))),
_tag_dyneither(_tmp567,sizeof(void*),0)));}}}struct _tuple17{struct Cyc_Position_Segment*
f1;int f2;};static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct
_tuple17*pr,void*desc);static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*
r,struct _tuple17*pr,void*desc){(*pr).f2=0;{struct _handler_cons _tmp30F;
_push_handler(& _tmp30F);{int _tmp311=0;if(setjmp(_tmp30F.handler))_tmp311=1;if(!
_tmp311){{struct _dyneither_ptr _tmp312=Cyc_Tcpat_desc2string(r,desc);const char*
_tmp56C;void*_tmp56B[1];struct Cyc_String_pa_struct _tmp56A;(_tmp56A.tag=0,((
_tmp56A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp312),((_tmp56B[0]=&
_tmp56A,Cyc_Tcutil_warn((*pr).f1,((_tmp56C="pattern not exhaustive.\n\tmissing case for %s",
_tag_dyneither(_tmp56C,sizeof(char),45))),_tag_dyneither(_tmp56B,sizeof(void*),1)))))));};
_pop_handler();}else{void*_tmp310=(void*)_exn_thrown;void*_tmp317=_tmp310;_LL187:
if(_tmp317 != Cyc_Tcpat_Desc2string)goto _LL189;_LL188:{const char*_tmp56F;void*
_tmp56E;(_tmp56E=0,Cyc_Tcutil_warn((*pr).f1,((_tmp56F="pattern not exhaustive.",
_tag_dyneither(_tmp56F,sizeof(char),24))),_tag_dyneither(_tmp56E,sizeof(void*),0)));}
goto _LL186;_LL189:;_LL18A:(void)_throw(_tmp317);_LL186:;}}}}static void Cyc_Tcpat_dummy_fn(
int i,int j);static void Cyc_Tcpat_dummy_fn(int i,int j){return;}struct _tuple18{void*
f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
struct _RegionHandle*_tmp31A=Cyc_Tcenv_get_fnrgn(te);struct _tuple18*_tmp572;
struct Cyc_List_List*_tmp571;struct Cyc_List_List*_tmp31B=(_tmp571=_region_malloc(
_tmp31A,sizeof(*_tmp571)),((_tmp571->hd=((_tmp572=_region_malloc(_tmp31A,sizeof(*
_tmp572)),((_tmp572->f1=Cyc_Tcpat_compile_pat(_tmp31A,p),((_tmp572->f2=0,_tmp572)))))),((
_tmp571->tl=0,_tmp571)))));void*_tmp31C=((void*(*)(struct _RegionHandle*r,struct
Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(_tmp31A,_tmp31B);struct
_tuple17 _tmp573;struct _tuple17 _tmp31D=(_tmp573.f1=loc,((_tmp573.f2=1,_tmp573)));
int _tmp31E=0;((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct
_RegionHandle*,struct _tuple17*,void*),struct _tuple17*env1,void(*rhs_appears)(int,
int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp31A,_tmp31C,
Cyc_Tcpat_not_exhaust_warn,& _tmp31D,Cyc_Tcpat_dummy_fn,0,& _tmp31E);return _tmp31D.f2;}
static struct _tuple15*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
swc);static struct _tuple15*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
swc){void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);struct _tuple14*_tmp574;
struct _tuple14*rhs=(_tmp574=_region_malloc(r,sizeof(*_tmp574)),((_tmp574->f1=0,((
_tmp574->f2=(swc->pattern)->loc,_tmp574)))));struct _tuple15*_tmp575;return(
_tmp575=_region_malloc(r,sizeof(*_tmp575)),((_tmp575->f1=sp0,((_tmp575->f2=rhs,
_tmp575)))));}static void Cyc_Tcpat_not_exhaust_err2(struct _RegionHandle*r,struct
Cyc_Position_Segment*loc,void*d);static void Cyc_Tcpat_not_exhaust_err2(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,void*d){;}void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs);
void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*swcs){struct _RegionHandle*_tmp324=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp325=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple15*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp324,Cyc_Tcpat_get_match2,
_tmp324,swcs);void*_tmp326=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(_tmp324,_tmp325);int _tmp327=0;((void(*)(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple14*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp324,_tmp326,Cyc_Tcpat_not_exhaust_err2,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp327);for(0;_tmp325 != 0;_tmp325=_tmp325->tl){
struct _tuple15 _tmp329;struct _tuple14*_tmp32A;struct _tuple14 _tmp32B;int _tmp32C;
struct Cyc_Position_Segment*_tmp32D;struct _tuple15*_tmp328=(struct _tuple15*)
_tmp325->hd;_tmp329=*_tmp328;_tmp32A=_tmp329.f2;_tmp32B=*_tmp32A;_tmp32C=_tmp32B.f1;
_tmp32D=_tmp32B.f2;if(!_tmp32C){const char*_tmp578;void*_tmp577;(_tmp577=0,Cyc_Tcutil_terr(
_tmp32D,((_tmp578="redundant pattern",_tag_dyneither(_tmp578,sizeof(char),18))),
_tag_dyneither(_tmp577,sizeof(void*),0)));}}}
