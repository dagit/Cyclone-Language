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
tl;};struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct
_dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*
x);extern char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _dyneither_ptr
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
f1;};struct Cyc_Absyn_Region_d_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;void*
Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_exn_typ;
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct
_dyneither_ptr Cyc_Absyn_attribute2string(void*);extern int Cyc_Absyn_no_regions;
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
char*tag;struct _dyneither_ptr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple3{void*f1;void*f2;};struct _tuple3*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple3*Cyc_Dict_rchoose(
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
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Fenv*
old_fenv,struct Cyc_Absyn_Fndecl*new_fn);void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*
Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct
Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(struct
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
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*
te);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void
Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void*Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_compress(
void*t);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_is_pointer_type(void*t);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;void*
Cyc_Tcutil_kind_to_bound(void*k);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(
void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);struct _tuple5*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,
struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*
k,int allow_evars,void*);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*,void*t);int Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,
struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,
struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple3*eq;int isTrue;};
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part);extern int Cyc_Tcexp_in_stmt_exp;struct _tuple6{
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{
struct _tuple6*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt);void
Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);void
Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct
Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{
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
encloser);void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,
int new_block);static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*
e);static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){void*
_tmp0=(void*)e->r;struct Cyc_Absyn_Exp*_tmp1;void*_tmp2;struct Cyc_Absyn_Exp*_tmp3;
void*_tmp4;_LL1: if(*((int*)_tmp0)!= 5)goto _LL3;_tmp1=((struct Cyc_Absyn_Increment_e_struct*)
_tmp0)->f1;_tmp2=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp0)->f2;if((int)
_tmp2 != 1)goto _LL3;_LL2:{struct Cyc_Absyn_Increment_e_struct _tmp14C;struct Cyc_Absyn_Increment_e_struct*
_tmp14B;(void*)(e->r=(void*)((void*)((_tmp14B=_cycalloc(sizeof(*_tmp14B)),((
_tmp14B[0]=((_tmp14C.tag=5,((_tmp14C.f1=_tmp1,((_tmp14C.f2=(void*)((void*)0),
_tmp14C)))))),_tmp14B))))));}goto _LL0;_LL3: if(*((int*)_tmp0)!= 5)goto _LL5;_tmp3=((
struct Cyc_Absyn_Increment_e_struct*)_tmp0)->f1;_tmp4=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp0)->f2;if((int)_tmp4 != 3)goto _LL5;_LL4:{struct Cyc_Absyn_Increment_e_struct
_tmp14F;struct Cyc_Absyn_Increment_e_struct*_tmp14E;(void*)(e->r=(void*)((void*)((
_tmp14E=_cycalloc(sizeof(*_tmp14E)),((_tmp14E[0]=((_tmp14F.tag=5,((_tmp14F.f1=
_tmp3,((_tmp14F.f2=(void*)((void*)2),_tmp14F)))))),_tmp14E))))));}goto _LL0;_LL5:;
_LL6: goto _LL0;_LL0:;}static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Stmt*s);static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s){Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(
te));s->try_depth=Cyc_Tcenv_get_try_depth(te);}static void Cyc_Tcstmt_pattern_synth(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult
pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block);
static void Cyc_Tcstmt_pattern_synth(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
where_opt,int new_block){struct _tuple6*_tmpA;struct Cyc_List_List*_tmpB;struct Cyc_Tcpat_TcPatResult
_tmp9=pat_res;_tmpA=_tmp9.tvars_and_bounds_opt;_tmpB=_tmp9.patvars;{struct Cyc_List_List*
_tmpC=_tmpA == 0?0:(*_tmpA).f1;struct Cyc_List_List*_tmpD=_tmpA == 0?0:(*_tmpA).f2;
struct _RegionHandle*_tmpE=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(
_tmpE,loc,te,_tmpC);te2=Cyc_Tcenv_new_outlives_constraints(_tmpE,te2,_tmpD,loc);
if(new_block)te2=Cyc_Tcenv_new_block(_tmpE,loc,te2);{struct Cyc_Core_Opt*_tmp150;
struct Cyc_Core_Opt*_tmpF=(_tmp150=_cycalloc(sizeof(*_tmp150)),((_tmp150->v=(void*)
Cyc_Tcenv_curr_rgn(te2),_tmp150)));{struct Cyc_List_List*_tmp10=_tmpB;for(0;
_tmp10 != 0;_tmp10=_tmp10->tl){te2=Cyc_Tcenv_add_pat_var(_tmpE,loc,te2,(struct Cyc_Absyn_Vardecl*)
_tmp10->hd);((struct Cyc_Absyn_Vardecl*)_tmp10->hd)->rgn=_tmpF;}}if(where_opt != 0){
const char*_tmp151;Cyc_Tcexp_tcTest(te2,(struct Cyc_Absyn_Exp*)where_opt,((_tmp151="switch clause guard",
_tag_dyneither(_tmp151,sizeof(char),20))));}if(_tmpB != 0)te2=Cyc_Tcenv_set_encloser(
_tmpE,te2,s);Cyc_Tcstmt_tcStmt(te2,s,0);}}if(_tmpB != 0)Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));}}static struct Cyc_List_List*Cyc_Tcstmt_cmap_c(
struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Tcenv_CList*x);
static struct Cyc_List_List*Cyc_Tcstmt_cmap_c(struct _RegionHandle*r,void*(*f)(void*,
void*),void*env,struct Cyc_Tcenv_CList*x){if(x == 0)return 0;else{struct Cyc_List_List*
_tmp152;return(_tmp152=_region_malloc(r,sizeof(*_tmp152)),((_tmp152->hd=(void*)f(
env,(void*)x->hd),((_tmp152->tl=Cyc_Tcstmt_cmap_c(r,f,env,x->tl),_tmp152)))));}}
static void Cyc_Tcstmt_tcStmtRefine(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s0,void*r1,void*r2,int new_block);static void Cyc_Tcstmt_tcStmtRefine(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s0,void*r1,void*r2,int new_block){struct _RegionHandle*
_tmp14=Cyc_Tcenv_get_fnrgn(te);struct _tuple5*tk=0;struct Cyc_Tcenv_Tenv*_tmp15=
Cyc_Tcenv_add_region_equality(_tmp14,te,r1,r2,& tk,s0->loc);Cyc_Tcstmt_tcStmt(
_tmp15,s0,new_block);if(tk != 0)(void*)(((*((struct _tuple5*)_check_null(tk))).f1)->kind=(
void*)(*((struct _tuple5*)_check_null(tk))).f2);}void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s0,int new_block);void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s0,int new_block){Cyc_Tcstmt_decorate_stmt(te,s0);{void*
_tmp16=(void*)s0->r;struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Stmt*_tmp18;
struct Cyc_Absyn_Stmt*_tmp19;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Exp*
_tmp1B;struct Cyc_Absyn_Stmt*_tmp1C;struct Cyc_Absyn_Stmt*_tmp1D;struct _tuple2
_tmp1E;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_Absyn_Stmt*_tmp20;struct Cyc_Absyn_Stmt*
_tmp21;struct Cyc_Absyn_Exp*_tmp22;struct _tuple2 _tmp23;struct Cyc_Absyn_Exp*_tmp24;
struct Cyc_Absyn_Stmt*_tmp25;struct _tuple2 _tmp26;struct Cyc_Absyn_Exp*_tmp27;
struct Cyc_Absyn_Stmt*_tmp28;struct Cyc_Absyn_Stmt*_tmp29;struct Cyc_Absyn_Stmt*
_tmp2A;struct _tuple2 _tmp2B;struct Cyc_Absyn_Exp*_tmp2C;struct Cyc_Absyn_Stmt*
_tmp2D;struct Cyc_Absyn_Stmt*_tmp2E;struct Cyc_Absyn_Stmt**_tmp2F;struct Cyc_Absyn_Stmt*
_tmp30;struct Cyc_Absyn_Stmt**_tmp31;struct _dyneither_ptr*_tmp32;struct Cyc_Absyn_Stmt*
_tmp33;struct Cyc_Absyn_Stmt**_tmp34;struct Cyc_List_List*_tmp35;struct Cyc_Absyn_Switch_clause**
_tmp36;struct Cyc_Absyn_Switch_clause***_tmp37;struct _dyneither_ptr*_tmp38;struct
Cyc_Absyn_Stmt*_tmp39;struct Cyc_Absyn_Exp*_tmp3A;struct Cyc_List_List*_tmp3B;
struct Cyc_Absyn_Stmt*_tmp3C;struct Cyc_List_List*_tmp3D;struct Cyc_Absyn_Decl*
_tmp3E;struct Cyc_Absyn_Stmt*_tmp3F;struct Cyc_Absyn_Exp*_tmp40;_LL8: if((int)
_tmp16 != 0)goto _LLA;_LL9: return;_LLA: if(_tmp16 <= (void*)1)goto _LLC;if(*((int*)
_tmp16)!= 0)goto _LLC;_tmp17=((struct Cyc_Absyn_Exp_s_struct*)_tmp16)->f1;_LLB: Cyc_Tcexp_tcExp(
te,0,_tmp17);if(!Cyc_Tcexp_in_stmt_exp)Cyc_Tcstmt_simplify_unused_result_exp(
_tmp17);return;_LLC: if(_tmp16 <= (void*)1)goto _LLE;if(*((int*)_tmp16)!= 1)goto
_LLE;_tmp18=((struct Cyc_Absyn_Seq_s_struct*)_tmp16)->f1;_tmp19=((struct Cyc_Absyn_Seq_s_struct*)
_tmp16)->f2;_LLD: {struct _RegionHandle*_tmp41=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Stmt_j_struct
_tmp155;struct Cyc_Tcenv_Stmt_j_struct*_tmp154;struct Cyc_Tcenv_Tenv*_tmp42=Cyc_Tcenv_set_next(
_tmp41,te,(void*)((_tmp154=_cycalloc(sizeof(*_tmp154)),((_tmp154[0]=((_tmp155.tag=
0,((_tmp155.f1=_tmp19,_tmp155)))),_tmp154)))));Cyc_Tcstmt_tcStmt(_tmp42,_tmp18,1);}
Cyc_Tcstmt_tcStmt(te,_tmp19,1);return;}_LLE: if(_tmp16 <= (void*)1)goto _LL10;if(*((
int*)_tmp16)!= 2)goto _LL10;_tmp1A=((struct Cyc_Absyn_Return_s_struct*)_tmp16)->f1;
_LLF: {void*t=Cyc_Tcenv_return_typ(te);if(_tmp1A == 0){void*_tmp45=Cyc_Tcutil_compress(
t);_LL2B: if((int)_tmp45 != 0)goto _LL2D;_LL2C: goto _LL2A;_LL2D:;_LL2E:{const char*
_tmp159;void*_tmp158[1];struct Cyc_String_pa_struct _tmp157;(_tmp157.tag=0,((
_tmp157.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp158[0]=& _tmp157,Cyc_Tcutil_terr(s0->loc,((_tmp159="must return a value of type %s",
_tag_dyneither(_tmp159,sizeof(char),31))),_tag_dyneither(_tmp158,sizeof(void*),1)))))));}
goto _LL2A;_LL2A:;}else{struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp1A;Cyc_Tcexp_tcExp(
te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){{const char*_tmp15E;void*
_tmp15D[2];struct Cyc_String_pa_struct _tmp15C;struct Cyc_String_pa_struct _tmp15B;(
_tmp15B.tag=0,((_tmp15B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp15C.tag=0,((_tmp15C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp15D[0]=& _tmp15C,((_tmp15D[1]=& _tmp15B,Cyc_Tcutil_terr(s0->loc,((_tmp15E="returns value of type %s but requires %s",
_tag_dyneither(_tmp15E,sizeof(char),41))),_tag_dyneither(_tmp15D,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct _RegionHandle*_tmp4D=Cyc_Tcenv_get_fnrgn(te);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp4D,t) && !Cyc_Tcutil_is_noalias_path(
_tmp4D,e)){const char*_tmp161;void*_tmp160;(_tmp160=0,Cyc_Tcutil_terr(e->loc,((
_tmp161="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp161,sizeof(char),49))),_tag_dyneither(_tmp160,sizeof(void*),0)));}}}return;}
_LL10: if(_tmp16 <= (void*)1)goto _LL12;if(*((int*)_tmp16)!= 3)goto _LL12;_tmp1B=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp16)->f1;_tmp1C=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp16)->f2;_tmp1D=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp16)->f3;_LL11: {
struct _tuple3*_tmp52;int _tmp53;const char*_tmp162;struct Cyc_Tcexp_TestEnv _tmp51=
Cyc_Tcexp_tcTest(te,_tmp1B,((_tmp162="if statement",_tag_dyneither(_tmp162,
sizeof(char),13))));_tmp52=_tmp51.eq;_tmp53=_tmp51.isTrue;{struct _tuple3*_tmp54=
_tmp52;struct _tuple3 _tmp55;void*_tmp56;void*_tmp57;_LL30: if(_tmp54 == 0)goto _LL32;
_tmp55=*_tmp54;_tmp56=_tmp55.f1;_tmp57=_tmp55.f2;_LL31: if(_tmp53){Cyc_Tcstmt_tcStmtRefine(
te,_tmp1C,_tmp56,_tmp57,1);Cyc_Tcstmt_tcStmt(te,_tmp1D,1);}else{Cyc_Tcstmt_tcStmt(
te,_tmp1C,1);Cyc_Tcstmt_tcStmtRefine(te,_tmp1D,_tmp56,_tmp57,1);}goto _LL2F;_LL32:;
_LL33: Cyc_Tcstmt_tcStmt(te,_tmp1C,1);Cyc_Tcstmt_tcStmt(te,_tmp1D,1);goto _LL2F;
_LL2F:;}return;}_LL12: if(_tmp16 <= (void*)1)goto _LL14;if(*((int*)_tmp16)!= 4)goto
_LL14;_tmp1E=((struct Cyc_Absyn_While_s_struct*)_tmp16)->f1;_tmp1F=_tmp1E.f1;
_tmp20=_tmp1E.f2;_tmp21=((struct Cyc_Absyn_While_s_struct*)_tmp16)->f2;_LL13: Cyc_Tcstmt_decorate_stmt(
te,_tmp20);{const char*_tmp163;Cyc_Tcexp_tcTest(te,_tmp1F,((_tmp163="while loop",
_tag_dyneither(_tmp163,sizeof(char),11))));}{struct _RegionHandle*_tmp59=Cyc_Tcenv_get_fnrgn(
te);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(_tmp59,te,_tmp20),_tmp21,1);return;}
_LL14: if(_tmp16 <= (void*)1)goto _LL16;if(*((int*)_tmp16)!= 8)goto _LL16;_tmp22=((
struct Cyc_Absyn_For_s_struct*)_tmp16)->f1;_tmp23=((struct Cyc_Absyn_For_s_struct*)
_tmp16)->f2;_tmp24=_tmp23.f1;_tmp25=_tmp23.f2;_tmp26=((struct Cyc_Absyn_For_s_struct*)
_tmp16)->f3;_tmp27=_tmp26.f1;_tmp28=_tmp26.f2;_tmp29=((struct Cyc_Absyn_For_s_struct*)
_tmp16)->f4;_LL15: Cyc_Tcstmt_decorate_stmt(te,_tmp25);Cyc_Tcstmt_decorate_stmt(
te,_tmp28);Cyc_Tcexp_tcExp(te,0,_tmp22);{const char*_tmp164;Cyc_Tcexp_tcTest(te,
_tmp24,((_tmp164="for loop",_tag_dyneither(_tmp164,sizeof(char),9))));}{struct
_RegionHandle*_tmp5B=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*_tmp5C=Cyc_Tcenv_set_in_loop(
_tmp5B,te,_tmp28);Cyc_Tcstmt_tcStmt(_tmp5C,_tmp29,1);Cyc_Tcexp_tcExp(_tmp5C,0,
_tmp27);Cyc_Tcstmt_simplify_unused_result_exp(_tmp27);}return;}_LL16: if(_tmp16 <= (
void*)1)goto _LL18;if(*((int*)_tmp16)!= 13)goto _LL18;_tmp2A=((struct Cyc_Absyn_Do_s_struct*)
_tmp16)->f1;_tmp2B=((struct Cyc_Absyn_Do_s_struct*)_tmp16)->f2;_tmp2C=_tmp2B.f1;
_tmp2D=_tmp2B.f2;_LL17: Cyc_Tcstmt_decorate_stmt(te,_tmp2D);{struct _RegionHandle*
_tmp5D=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(_tmp5D,te,
_tmp2D),_tmp2A,1);{const char*_tmp165;Cyc_Tcexp_tcTest(te,_tmp2C,((_tmp165="do loop",
_tag_dyneither(_tmp165,sizeof(char),8))));}return;}_LL18: if(_tmp16 <= (void*)1)
goto _LL1A;if(*((int*)_tmp16)!= 5)goto _LL1A;_tmp2E=((struct Cyc_Absyn_Break_s_struct*)
_tmp16)->f1;_tmp2F=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Break_s_struct*)
_tmp16)->f1;_LL19: Cyc_Tcenv_process_break(te,s0,_tmp2F);return;_LL1A: if(_tmp16 <= (
void*)1)goto _LL1C;if(*((int*)_tmp16)!= 6)goto _LL1C;_tmp30=((struct Cyc_Absyn_Continue_s_struct*)
_tmp16)->f1;_tmp31=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Continue_s_struct*)
_tmp16)->f1;_LL1B: Cyc_Tcenv_process_continue(te,s0,_tmp31);return;_LL1C: if(
_tmp16 <= (void*)1)goto _LL1E;if(*((int*)_tmp16)!= 7)goto _LL1E;_tmp32=((struct Cyc_Absyn_Goto_s_struct*)
_tmp16)->f1;_tmp33=((struct Cyc_Absyn_Goto_s_struct*)_tmp16)->f2;_tmp34=(struct
Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Goto_s_struct*)_tmp16)->f2;_LL1D: Cyc_Tcenv_process_goto(
te,s0,_tmp32,_tmp34);return;_LL1E: if(_tmp16 <= (void*)1)goto _LL20;if(*((int*)
_tmp16)!= 10)goto _LL20;_tmp35=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp16)->f1;
_tmp36=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp16)->f2;_tmp37=(struct Cyc_Absyn_Switch_clause***)&((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp16)->f2;_LL1F: {struct _tuple4*_tmp5F=Cyc_Tcenv_process_fallthru(
te,s0,_tmp37);if(_tmp5F == 0){{const char*_tmp168;void*_tmp167;(_tmp167=0,Cyc_Tcutil_terr(
s0->loc,((_tmp168="fallthru not in a non-last case",_tag_dyneither(_tmp168,
sizeof(char),32))),_tag_dyneither(_tmp167,sizeof(void*),0)));}return;}{struct Cyc_List_List*
_tmp62=(*_tmp5F).f2;struct Cyc_Tcenv_CList*_tmp63=(*_tmp5F).f3;struct Cyc_List_List*
instantiation=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_make_inst_var,Cyc_Tcenv_lookup_type_vars(te),_tmp62);struct Cyc_List_List*
_tmp64=((struct Cyc_List_List*(*)(struct _RegionHandle*r,void*(*f)(struct Cyc_List_List*,
void*),struct Cyc_List_List*env,struct Cyc_Tcenv_CList*x))Cyc_Tcstmt_cmap_c)(Cyc_Core_heap_region,
Cyc_Tcutil_substitute,instantiation,_tmp63);for(0;_tmp64 != 0  && _tmp35 != 0;(
_tmp64=_tmp64->tl,_tmp35=_tmp35->tl)){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmp35->hd);if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)_tmp35->hd,(void*)
_tmp64->hd)){{const char*_tmp16D;void*_tmp16C[2];struct Cyc_String_pa_struct
_tmp16B;struct Cyc_String_pa_struct _tmp16A;(_tmp16A.tag=0,((_tmp16A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmp64->hd)),((
_tmp16B.tag=0,((_tmp16B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)_tmp35->hd)->topt))->v)),((
_tmp16C[0]=& _tmp16B,((_tmp16C[1]=& _tmp16A,Cyc_Tcutil_terr(s0->loc,((_tmp16D="fallthru argument has type %s but pattern variable has type %s",
_tag_dyneither(_tmp16D,sizeof(char),63))),_tag_dyneither(_tmp16C,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct _RegionHandle*_tmp69=Cyc_Tcenv_get_fnrgn(te);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp69,(void*)_tmp64->hd) && !Cyc_Tcutil_is_noalias_path(
_tmp69,(struct Cyc_Absyn_Exp*)_tmp35->hd)){const char*_tmp170;void*_tmp16F;(
_tmp16F=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp35->hd)->loc,((_tmp170="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp170,sizeof(char),49))),_tag_dyneither(_tmp16F,sizeof(void*),0)));}}}
if(_tmp35 != 0){const char*_tmp173;void*_tmp172;(_tmp172=0,Cyc_Tcutil_terr(s0->loc,((
_tmp173="too many arguments to explicit fallthru",_tag_dyneither(_tmp173,sizeof(
char),40))),_tag_dyneither(_tmp172,sizeof(void*),0)));}if(_tmp64 != 0){const char*
_tmp176;void*_tmp175;(_tmp175=0,Cyc_Tcutil_terr(s0->loc,((_tmp176="too few arguments to explicit fallthru",
_tag_dyneither(_tmp176,sizeof(char),39))),_tag_dyneither(_tmp175,sizeof(void*),0)));}
return;}}_LL20: if(_tmp16 <= (void*)1)goto _LL22;if(*((int*)_tmp16)!= 12)goto _LL22;
_tmp38=((struct Cyc_Absyn_Label_s_struct*)_tmp16)->f1;_tmp39=((struct Cyc_Absyn_Label_s_struct*)
_tmp16)->f2;_LL21: {struct _RegionHandle*_tmp70=Cyc_Tcenv_get_fnrgn(te);{struct
_dyneither_ptr*_tmp183;const char*_tmp182;void*_tmp181[1];struct Cyc_String_pa_struct
_tmp180;struct Cyc_Absyn_Tvar*_tmp17F;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_new_named_block(
_tmp70,s0->loc,Cyc_Tcenv_add_label(te,_tmp38,_tmp39),((_tmp17F=_cycalloc(sizeof(*
_tmp17F)),((_tmp17F->name=((_tmp183=_cycalloc(sizeof(*_tmp183)),((_tmp183[0]=(
struct _dyneither_ptr)((_tmp180.tag=0,((_tmp180.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp38),((_tmp181[0]=& _tmp180,Cyc_aprintf(((_tmp182="`%s",
_tag_dyneither(_tmp182,sizeof(char),4))),_tag_dyneither(_tmp181,sizeof(void*),1)))))))),
_tmp183)))),((_tmp17F->identity=Cyc_Tcutil_new_tvar_id(),((_tmp17F->kind=(void*)
Cyc_Tcutil_kind_to_bound((void*)3),_tmp17F))))))))),_tmp39,0);}return;}_LL22: if(
_tmp16 <= (void*)1)goto _LL24;if(*((int*)_tmp16)!= 9)goto _LL24;_tmp3A=((struct Cyc_Absyn_Switch_s_struct*)
_tmp16)->f1;_tmp3B=((struct Cyc_Absyn_Switch_s_struct*)_tmp16)->f2;_LL23: Cyc_Tcexp_tcExp(
te,0,_tmp3A);{void*_tmp76=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3A->topt))->v;
struct _RegionHandle*_tmp77=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp77,_tmp76) && !Cyc_Tcutil_is_noalias_path(_tmp77,_tmp3A)){const char*_tmp186;
void*_tmp185;(_tmp185=0,Cyc_Tcutil_terr(_tmp3A->loc,((_tmp186="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp186,sizeof(char),49))),_tag_dyneither(_tmp185,sizeof(void*),0)));}{
struct Cyc_Tcenv_Tenv*_tmp7A=Cyc_Tcenv_set_in_switch(_tmp77,te);_tmp7A=Cyc_Tcenv_clear_fallthru(
_tmp77,_tmp7A);{struct Cyc_List_List*scs=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(_tmp3B);for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Pat*_tmp7B=((
struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct Cyc_Tcpat_TcPatResult
_tmp7C=Cyc_Tcpat_tcPat(_tmp7A,_tmp7B,& _tmp76);struct Cyc_List_List*_tmp7D=_tmp7C.tvars_and_bounds_opt
== 0?0:(*_tmp7C.tvars_and_bounds_opt).f1;if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp7B->topt))->v,_tmp76)){{const char*_tmp18B;void*_tmp18A[2];struct
Cyc_String_pa_struct _tmp189;struct Cyc_String_pa_struct _tmp188;(_tmp188.tag=0,((
_tmp188.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp7B->topt))->v)),((_tmp189.tag=0,((
_tmp189.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp76)),((_tmp18A[0]=& _tmp189,((_tmp18A[1]=& _tmp188,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)
scs->hd)->loc,((_tmp18B="switch on type %s, but case expects type %s",
_tag_dyneither(_tmp18B,sizeof(char),44))),_tag_dyneither(_tmp18A,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}else{Cyc_Tcpat_check_pat_regions(_tmp7A,_tmp7B);}{
struct Cyc_Core_Opt*_tmp18C;((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=((
_tmp18C=_cycalloc(sizeof(*_tmp18C)),((_tmp18C->v=_tmp7C.patvars,_tmp18C))));}Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,_tmp7A,_tmp7C,((struct Cyc_Absyn_Switch_clause*)
scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);if(
_tmp7C.tvars_and_bounds_opt != 0  && (*_tmp7C.tvars_and_bounds_opt).f2 != 0)_tmp7A=
Cyc_Tcenv_clear_fallthru(_tmp77,_tmp7A);else{_tmp7A=Cyc_Tcenv_set_fallthru(
_tmp77,_tmp7A,_tmp7D,_tmp7C.patvars,(struct Cyc_Absyn_Switch_clause*)scs->hd);}}}}
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,_tmp3B);return;}_LL24: if(_tmp16 <= (
void*)1)goto _LL26;if(*((int*)_tmp16)!= 14)goto _LL26;_tmp3C=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp16)->f1;_tmp3D=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp16)->f2;_LL25:{
struct Cyc_Absyn_Seq_s_struct _tmp18F;struct Cyc_Absyn_Seq_s_struct*_tmp18E;(void*)(
_tmp3C->r=(void*)((void*)(Cyc_Absyn_new_stmt((void*)((_tmp18E=_cycalloc(sizeof(*
_tmp18E)),((_tmp18E[0]=((_tmp18F.tag=1,((_tmp18F.f1=Cyc_Absyn_new_stmt((void*)
_tmp3C->r,_tmp3C->loc),((_tmp18F.f2=Cyc_Absyn_skip_stmt(_tmp3C->loc),_tmp18F)))))),
_tmp18E)))),_tmp3C->loc))->r));}{struct _RegionHandle*_tmp85=Cyc_Tcenv_get_fnrgn(
te);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(_tmp85,Cyc_Tcenv_enter_try(_tmp85,
te),s0),_tmp3C,1);{struct _RegionHandle*_tmp86=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*
_tmp87=Cyc_Tcenv_set_in_switch(_tmp86,te);_tmp87=Cyc_Tcenv_clear_fallthru(_tmp86,
_tmp87);{struct Cyc_List_List*_tmp88=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(_tmp3D);for(0;_tmp88 != 0;_tmp88=_tmp88->tl){struct Cyc_Absyn_Pat*
_tmp89=((struct Cyc_Absyn_Switch_clause*)_tmp88->hd)->pattern;struct Cyc_Tcpat_TcPatResult
_tmp8A=Cyc_Tcpat_tcPat(_tmp87,_tmp89,& Cyc_Absyn_exn_typ);struct Cyc_List_List*
_tmp8B=_tmp8A.tvars_and_bounds_opt == 0?0:(*_tmp8A.tvars_and_bounds_opt).f1;if(!
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp89->topt))->v,Cyc_Absyn_exn_typ)){
const char*_tmp193;void*_tmp192[1];struct Cyc_String_pa_struct _tmp191;(_tmp191.tag=
0,((_tmp191.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp89->topt))->v)),((_tmp192[0]=&
_tmp191,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)_tmp88->hd)->loc,((
_tmp193="expected xtunion exn but found %s",_tag_dyneither(_tmp193,sizeof(char),
34))),_tag_dyneither(_tmp192,sizeof(void*),1)))))));}else{Cyc_Tcpat_check_pat_regions(
_tmp87,_tmp89);}{struct Cyc_Core_Opt*_tmp194;((struct Cyc_Absyn_Switch_clause*)
_tmp88->hd)->pat_vars=((_tmp194=_cycalloc(sizeof(*_tmp194)),((_tmp194->v=_tmp8A.patvars,
_tmp194))));}Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmp88->hd)->loc,
_tmp87,_tmp8A,((struct Cyc_Absyn_Switch_clause*)_tmp88->hd)->body,((struct Cyc_Absyn_Switch_clause*)
_tmp88->hd)->where_clause,1);if(_tmp8A.tvars_and_bounds_opt != 0  && (*_tmp8A.tvars_and_bounds_opt).f2
!= 0)_tmp87=Cyc_Tcenv_clear_fallthru(_tmp86,_tmp87);else{_tmp87=Cyc_Tcenv_set_fallthru(
_tmp86,_tmp87,_tmp8B,_tmp8A.patvars,(struct Cyc_Absyn_Switch_clause*)_tmp88->hd);}}}}
Cyc_Tcpat_check_catch_overlap(s0->loc,te,_tmp3D);return;}}_LL26: if(_tmp16 <= (
void*)1)goto _LL28;if(*((int*)_tmp16)!= 11)goto _LL28;_tmp3E=((struct Cyc_Absyn_Decl_s_struct*)
_tmp16)->f1;_tmp3F=((struct Cyc_Absyn_Decl_s_struct*)_tmp16)->f2;_LL27: {struct
_dyneither_ptr unimp_msg_part;struct _RegionHandle*_tmp90=Cyc_Tcenv_get_fnrgn(te);{
struct Cyc_Tcenv_Tenv*te2;if(new_block)te2=Cyc_Tcenv_new_block(_tmp90,s0->loc,te);
else{te2=Cyc_Tcenv_copy_tenv(_tmp90,te);}{struct Cyc_Tcenv_Tenv*_tmp91=te2;void*
_tmp92=(void*)_tmp3E->r;struct Cyc_Absyn_Vardecl*_tmp93;struct Cyc_Absyn_Pat*
_tmp94;struct Cyc_Core_Opt*_tmp95;struct Cyc_Core_Opt**_tmp96;struct Cyc_Absyn_Exp*
_tmp97;struct Cyc_List_List*_tmp98;struct Cyc_Absyn_Tvar*_tmp99;struct Cyc_Absyn_Vardecl*
_tmp9A;int _tmp9B;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_Absyn_Tvar*
_tmp9E;struct Cyc_Absyn_Vardecl*_tmp9F;struct Cyc_Absyn_Fndecl*_tmpA0;struct
_dyneither_ptr*_tmpA1;struct Cyc_List_List*_tmpA2;struct _tuple0*_tmpA3;struct Cyc_List_List*
_tmpA4;_LL35: if(_tmp92 <= (void*)2)goto _LL51;if(*((int*)_tmp92)!= 0)goto _LL37;
_tmp93=((struct Cyc_Absyn_Var_d_struct*)_tmp92)->f1;_LL36: {struct Cyc_Absyn_Vardecl
_tmpA6;void*_tmpA7;struct _tuple0*_tmpA8;struct _tuple0 _tmpA9;union Cyc_Absyn_Nmspace_union
_tmpAA;struct _dyneither_ptr*_tmpAB;struct Cyc_Absyn_Tqual _tmpAC;void*_tmpAD;
struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_Core_Opt*_tmpAF;struct Cyc_Core_Opt**_tmpB0;
struct Cyc_List_List*_tmpB1;struct Cyc_Absyn_Vardecl*_tmpA5=_tmp93;_tmpA6=*_tmpA5;
_tmpA7=(void*)_tmpA6.sc;_tmpA8=_tmpA6.name;_tmpA9=*_tmpA8;_tmpAA=_tmpA9.f1;
_tmpAB=_tmpA9.f2;_tmpAC=_tmpA6.tq;_tmpAD=(void*)_tmpA6.type;_tmpAE=_tmpA6.initializer;
_tmpAF=_tmpA6.rgn;_tmpB0=(struct Cyc_Core_Opt**)&(*_tmpA5).rgn;_tmpB1=_tmpA6.attributes;{
void*_tmpB2=Cyc_Tcenv_curr_rgn(_tmp91);int is_local;{void*_tmpB3=_tmpA7;_LL56: if((
int)_tmpB3 != 0)goto _LL58;_LL57: goto _LL59;_LL58: if((int)_tmpB3 != 3)goto _LL5A;
_LL59: goto _LL5B;_LL5A: if((int)_tmpB3 != 4)goto _LL5C;_LL5B: is_local=0;goto _LL55;
_LL5C: if((int)_tmpB3 != 1)goto _LL5E;_LL5D:{const char*_tmp197;void*_tmp196;(
_tmp196=0,Cyc_Tcutil_terr(_tmp3E->loc,((_tmp197="bad abstract scope for local variable",
_tag_dyneither(_tmp197,sizeof(char),38))),_tag_dyneither(_tmp196,sizeof(void*),0)));}
goto _LL5F;_LL5E: if((int)_tmpB3 != 5)goto _LL60;_LL5F: goto _LL61;_LL60: if((int)
_tmpB3 != 2)goto _LL55;_LL61: is_local=1;goto _LL55;_LL55:;}{struct Cyc_Core_Opt*
_tmp199;struct Cyc_Core_Opt*_tmp198;*_tmpB0=is_local?(_tmp199=_cycalloc(sizeof(*
_tmp199)),((_tmp199->v=(void*)_tmpB2,_tmp199))):((_tmp198=_cycalloc(sizeof(*
_tmp198)),((_tmp198->v=(void*)((void*)2),_tmp198))));}{union Cyc_Absyn_Nmspace_union
_tmpB8=_tmpAA;struct Cyc_List_List*_tmpB9;struct Cyc_List_List*_tmpBA;_LL63: if((
_tmpB8.Loc_n).tag != 0)goto _LL65;_LL64: goto _LL62;_LL65: if((_tmpB8.Rel_n).tag != 1)
goto _LL67;_tmpB9=(_tmpB8.Rel_n).f1;if(_tmpB9 != 0)goto _LL67;_LL66:{union Cyc_Absyn_Nmspace_union
_tmp19A;(*_tmp93->name).f1=(union Cyc_Absyn_Nmspace_union)(((_tmp19A.Loc_n).tag=0,
_tmp19A));}goto _LL62;_LL67: if((_tmpB8.Abs_n).tag != 2)goto _LL69;_tmpBA=(_tmpB8.Abs_n).f1;
_LL68: {const char*_tmp19D;void*_tmp19C;(int)_throw(((_tmp19C=0,Cyc_Tcutil_impos(((
_tmp19D="tcstmt: Abs_n declaration",_tag_dyneither(_tmp19D,sizeof(char),26))),
_tag_dyneither(_tmp19C,sizeof(void*),0)))));}_LL69:;_LL6A:{const char*_tmp1A0;
void*_tmp19F;(_tmp19F=0,Cyc_Tcutil_terr(_tmp3E->loc,((_tmp1A0="cannot declare a qualified local variable",
_tag_dyneither(_tmp1A0,sizeof(char),42))),_tag_dyneither(_tmp19F,sizeof(void*),0)));}
goto _LL62;_LL62:;}{void*_tmpC0=Cyc_Tcutil_compress(_tmpAD);struct Cyc_Absyn_ArrayInfo
_tmpC1;void*_tmpC2;struct Cyc_Absyn_Tqual _tmpC3;struct Cyc_Absyn_Exp*_tmpC4;struct
Cyc_Absyn_Conref*_tmpC5;struct Cyc_Position_Segment*_tmpC6;_LL6C: if(_tmpC0 <= (
void*)4)goto _LL6E;if(*((int*)_tmpC0)!= 7)goto _LL6E;_tmpC1=((struct Cyc_Absyn_ArrayType_struct*)
_tmpC0)->f1;_tmpC2=(void*)_tmpC1.elt_type;_tmpC3=_tmpC1.tq;_tmpC4=_tmpC1.num_elts;
if(_tmpC4 != 0)goto _LL6E;_tmpC5=_tmpC1.zero_term;_tmpC6=_tmpC1.zt_loc;if(!(_tmp93->initializer
!= 0))goto _LL6E;_LL6D:{void*_tmpC7=(void*)((struct Cyc_Absyn_Exp*)_check_null(
_tmp93->initializer))->r;union Cyc_Absyn_Cnst_union _tmpC8;struct _dyneither_ptr
_tmpC9;struct Cyc_Absyn_Exp*_tmpCA;struct Cyc_List_List*_tmpCB;struct Cyc_List_List*
_tmpCC;_LL71: if(*((int*)_tmpC7)!= 0)goto _LL73;_tmpC8=((struct Cyc_Absyn_Const_e_struct*)
_tmpC7)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpC7)->f1).String_c).tag != 
5)goto _LL73;_tmpC9=(_tmpC8.String_c).f1;_LL72: _tmpAD=(void*)(_tmp93->type=(void*)
Cyc_Absyn_array_typ(_tmpC2,_tmpC3,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
_get_dyneither_size(_tmpC9,sizeof(char)),0),_tmpC5,_tmpC6));goto _LL70;_LL73: if(*((
int*)_tmpC7)!= 29)goto _LL75;_tmpCA=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpC7)->f2;_LL74: _tmpAD=(void*)(_tmp93->type=(void*)Cyc_Absyn_array_typ(_tmpC2,
_tmpC3,(struct Cyc_Absyn_Exp*)_tmpCA,_tmpC5,_tmpC6));goto _LL70;_LL75: if(*((int*)
_tmpC7)!= 37)goto _LL77;_tmpCB=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpC7)->f2;
_LL76: _tmpCC=_tmpCB;goto _LL78;_LL77: if(*((int*)_tmpC7)!= 28)goto _LL79;_tmpCC=((
struct Cyc_Absyn_Array_e_struct*)_tmpC7)->f1;_LL78: _tmpAD=(void*)(_tmp93->type=(
void*)Cyc_Absyn_array_typ(_tmpC2,_tmpC3,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpCC),0),_tmpC5,
_tmpC6));goto _LL70;_LL79:;_LL7A: goto _LL70;_LL70:;}goto _LL6B;_LL6E:;_LL6F: goto
_LL6B;_LL6B:;}{struct Cyc_List_List*_tmpCD=!is_local?0: Cyc_Tcenv_lookup_type_vars(
_tmp91);int _tmpCE=!is_local?0: 1;Cyc_Tcutil_check_type(s0->loc,_tmp91,_tmpCD,(
void*)1,_tmpCE,_tmpAD);{struct Cyc_Tcenv_Tenv*_tmpCF=Cyc_Tcenv_add_local_var(
_tmp90,_tmp3F->loc,_tmp91,_tmp93);if(_tmpA7 == (void*)3  || _tmpA7 == (void*)4){
const char*_tmp1A3;void*_tmp1A2;(_tmp1A2=0,Cyc_Tcutil_terr(_tmp3E->loc,((_tmp1A3="local extern declarations not yet supported",
_tag_dyneither(_tmp1A3,sizeof(char),44))),_tag_dyneither(_tmp1A2,sizeof(void*),0)));}
if(_tmpAE != 0){Cyc_Tcexp_tcExpInitializer(_tmpCF,(void**)& _tmpAD,(struct Cyc_Absyn_Exp*)
_tmpAE);if(!is_local  && !Cyc_Tcutil_is_const_exp(_tmp91,(struct Cyc_Absyn_Exp*)
_tmpAE)){const char*_tmp1A6;void*_tmp1A5;(_tmp1A5=0,Cyc_Tcutil_terr(_tmp3E->loc,((
_tmp1A6="initializer needs to be a constant expression",_tag_dyneither(_tmp1A6,
sizeof(char),46))),_tag_dyneither(_tmp1A5,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_assign(
_tmpCF,(struct Cyc_Absyn_Exp*)_tmpAE,_tmpAD)){struct _dyneither_ptr _tmpD4=*_tmpAB;
struct _dyneither_ptr _tmpD5=Cyc_Absynpp_typ2string(_tmpAD);struct _dyneither_ptr
_tmpD6=Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmpAE->topt))->v);
if(((_get_dyneither_size(_tmpD4,sizeof(char))+ _get_dyneither_size(_tmpD5,
sizeof(char)))+ _get_dyneither_size(_tmpD6,sizeof(char)))+ 50 < 80){const char*
_tmp1AC;void*_tmp1AB[3];struct Cyc_String_pa_struct _tmp1AA;struct Cyc_String_pa_struct
_tmp1A9;struct Cyc_String_pa_struct _tmp1A8;(_tmp1A8.tag=0,((_tmp1A8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmpD6),((_tmp1A9.tag=0,((_tmp1A9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD5),((_tmp1AA.tag=0,((_tmp1AA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD4),((_tmp1AB[0]=& _tmp1AA,((
_tmp1AB[1]=& _tmp1A9,((_tmp1AB[2]=& _tmp1A8,Cyc_Tcutil_terr(_tmp3E->loc,((_tmp1AC="%s declared with type %s but initialized with type %s.",
_tag_dyneither(_tmp1AC,sizeof(char),55))),_tag_dyneither(_tmp1AB,sizeof(void*),3)))))))))))))))))));}
else{if((_get_dyneither_size(_tmpD4,sizeof(char))+ _get_dyneither_size(_tmpD5,
sizeof(char)))+ 25 < 80  && _get_dyneither_size(_tmpD6,sizeof(char))+ 25 < 80){
const char*_tmp1B2;void*_tmp1B1[3];struct Cyc_String_pa_struct _tmp1B0;struct Cyc_String_pa_struct
_tmp1AF;struct Cyc_String_pa_struct _tmp1AE;(_tmp1AE.tag=0,((_tmp1AE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmpD6),((_tmp1AF.tag=0,((_tmp1AF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD5),((_tmp1B0.tag=0,((_tmp1B0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD4),((_tmp1B1[0]=& _tmp1B0,((
_tmp1B1[1]=& _tmp1AF,((_tmp1B1[2]=& _tmp1AE,Cyc_Tcutil_terr(_tmp3E->loc,((_tmp1B2="%s declared with type %s\n but initialized with type %s.",
_tag_dyneither(_tmp1B2,sizeof(char),56))),_tag_dyneither(_tmp1B1,sizeof(void*),3)))))))))))))))))));}
else{const char*_tmp1B8;void*_tmp1B7[3];struct Cyc_String_pa_struct _tmp1B6;struct
Cyc_String_pa_struct _tmp1B5;struct Cyc_String_pa_struct _tmp1B4;(_tmp1B4.tag=0,((
_tmp1B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD6),((_tmp1B5.tag=0,((
_tmp1B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD5),((_tmp1B6.tag=0,((
_tmp1B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD4),((_tmp1B7[0]=&
_tmp1B6,((_tmp1B7[1]=& _tmp1B5,((_tmp1B7[2]=& _tmp1B4,Cyc_Tcutil_terr(_tmp3E->loc,((
_tmp1B8="%s declared with type \n%s\n but initialized with type \n%s.",
_tag_dyneither(_tmp1B8,sizeof(char),58))),_tag_dyneither(_tmp1B7,sizeof(void*),3)))))))))))))))))));}}
Cyc_Tcutil_unify(_tmpAD,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpAE->topt))->v);
Cyc_Tcutil_explain_failure();}}Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(_tmp90,
_tmpCF,s0),_tmp3F,0);return;}}}}_LL37: if(*((int*)_tmp92)!= 2)goto _LL39;_tmp94=((
struct Cyc_Absyn_Let_d_struct*)_tmp92)->f1;_tmp95=((struct Cyc_Absyn_Let_d_struct*)
_tmp92)->f2;_tmp96=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_struct*)_tmp92)->f2;
_tmp97=((struct Cyc_Absyn_Let_d_struct*)_tmp92)->f3;_LL38: Cyc_Tcexp_tcExpInitializer(
_tmp91,0,_tmp97);{struct Cyc_Tcpat_TcPatResult _tmpE6=Cyc_Tcpat_tcPat(_tmp91,
_tmp94,(void**)((void*)&((struct Cyc_Core_Opt*)_check_null(_tmp97->topt))->v));{
struct Cyc_Core_Opt*_tmp1B9;*_tmp96=((_tmp1B9=_cycalloc(sizeof(*_tmp1B9)),((
_tmp1B9->v=_tmpE6.patvars,_tmp1B9))));}if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp94->topt))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp97->topt))->v)
 && !Cyc_Tcutil_coerce_assign(_tmp91,_tmp97,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp94->topt))->v)){{const char*_tmp1BE;void*_tmp1BD[2];struct Cyc_String_pa_struct
_tmp1BC;struct Cyc_String_pa_struct _tmp1BB;(_tmp1BB.tag=0,((_tmp1BB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp97->topt))->v)),((_tmp1BC.tag=0,((_tmp1BC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp94->topt))->v)),((_tmp1BD[0]=& _tmp1BC,((_tmp1BD[1]=& _tmp1BB,Cyc_Tcutil_terr(
_tmp3E->loc,((_tmp1BE="pattern type %s does not match definition type %s",
_tag_dyneither(_tmp1BE,sizeof(char),50))),_tag_dyneither(_tmp1BD,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp94->topt))->v,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp97->topt))->v);Cyc_Tcutil_explain_failure();}
else{Cyc_Tcpat_check_pat_regions(_tmp91,_tmp94);}Cyc_Tcpat_check_let_pat_exhaustive(
_tmp94->loc,_tmp91,_tmp94);Cyc_Tcstmt_pattern_synth(s0->loc,_tmp91,_tmpE6,_tmp3F,
0,0);return;}_LL39: if(*((int*)_tmp92)!= 3)goto _LL3B;_tmp98=((struct Cyc_Absyn_Letv_d_struct*)
_tmp92)->f1;_LL3A: {void*_tmpEC=Cyc_Tcenv_curr_rgn(_tmp91);struct Cyc_Tcenv_Tenv*
_tmpED=_tmp91;for(0;_tmp98 != 0;_tmp98=_tmp98->tl){struct Cyc_Absyn_Vardecl*_tmpEE=(
struct Cyc_Absyn_Vardecl*)_tmp98->hd;struct Cyc_Absyn_Vardecl _tmpF0;struct _tuple0*
_tmpF1;struct _tuple0 _tmpF2;union Cyc_Absyn_Nmspace_union _tmpF3;void*_tmpF4;struct
Cyc_Core_Opt*_tmpF5;struct Cyc_Core_Opt**_tmpF6;struct Cyc_Absyn_Vardecl*_tmpEF=
_tmpEE;_tmpF0=*_tmpEF;_tmpF1=_tmpF0.name;_tmpF2=*_tmpF1;_tmpF3=_tmpF2.f1;_tmpF4=(
void*)_tmpF0.type;_tmpF5=_tmpF0.rgn;_tmpF6=(struct Cyc_Core_Opt**)&(*_tmpEF).rgn;{
struct Cyc_Core_Opt*_tmp1BF;*_tmpF6=((_tmp1BF=_cycalloc(sizeof(*_tmp1BF)),((
_tmp1BF->v=(void*)_tmpEC,_tmp1BF))));}{union Cyc_Absyn_Nmspace_union _tmpF8=_tmpF3;
struct Cyc_List_List*_tmpF9;struct Cyc_List_List*_tmpFA;_LL7C: if((_tmpF8.Loc_n).tag
!= 0)goto _LL7E;_LL7D: goto _LL7B;_LL7E: if((_tmpF8.Rel_n).tag != 1)goto _LL80;_tmpF9=(
_tmpF8.Rel_n).f1;if(_tmpF9 != 0)goto _LL80;_LL7F:{union Cyc_Absyn_Nmspace_union
_tmp1C0;(*_tmpEE->name).f1=(union Cyc_Absyn_Nmspace_union)(((_tmp1C0.Loc_n).tag=0,
_tmp1C0));}goto _LL7B;_LL80: if((_tmpF8.Abs_n).tag != 2)goto _LL82;_tmpFA=(_tmpF8.Abs_n).f1;
_LL81: {const char*_tmp1C3;void*_tmp1C2;(int)_throw(((_tmp1C2=0,Cyc_Tcutil_impos(((
_tmp1C3="tcstmt: Abs_n declaration",_tag_dyneither(_tmp1C3,sizeof(char),26))),
_tag_dyneither(_tmp1C2,sizeof(void*),0)))));}_LL82:;_LL83:{const char*_tmp1C6;
void*_tmp1C5;(_tmp1C5=0,Cyc_Tcutil_terr(_tmp3E->loc,((_tmp1C6="cannot declare a qualified local variable",
_tag_dyneither(_tmp1C6,sizeof(char),42))),_tag_dyneither(_tmp1C5,sizeof(void*),0)));}
goto _LL7B;_LL7B:;}Cyc_Tcutil_check_type(s0->loc,_tmpED,Cyc_Tcenv_lookup_type_vars(
_tmpED),(void*)1,1,_tmpF4);_tmpED=Cyc_Tcenv_add_local_var(_tmp90,_tmp3F->loc,
_tmpED,_tmpEE);}Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(_tmp90,_tmpED,s0),
_tmp3F,0);return;}_LL3B: if(*((int*)_tmp92)!= 4)goto _LL3D;_tmp99=((struct Cyc_Absyn_Region_d_struct*)
_tmp92)->f1;_tmp9A=((struct Cyc_Absyn_Region_d_struct*)_tmp92)->f2;_tmp9B=((
struct Cyc_Absyn_Region_d_struct*)_tmp92)->f3;_tmp9C=((struct Cyc_Absyn_Region_d_struct*)
_tmp92)->f4;_LL3C:{struct Cyc_Absyn_Seq_s_struct _tmp1C9;struct Cyc_Absyn_Seq_s_struct*
_tmp1C8;(void*)(_tmp3F->r=(void*)((void*)(Cyc_Absyn_new_stmt((void*)((_tmp1C8=
_cycalloc(sizeof(*_tmp1C8)),((_tmp1C8[0]=((_tmp1C9.tag=1,((_tmp1C9.f1=Cyc_Absyn_new_stmt((
void*)_tmp3F->r,_tmp3F->loc),((_tmp1C9.f2=Cyc_Absyn_skip_stmt(_tmp3F->loc),
_tmp1C9)))))),_tmp1C8)))),_tmp3F->loc))->r));}{struct Cyc_Tcenv_Tenv*_tmp102=
_tmp91;struct Cyc_Absyn_Vardecl _tmp104;void*_tmp105;void**_tmp106;struct Cyc_Core_Opt*
_tmp107;struct Cyc_Core_Opt**_tmp108;struct Cyc_Absyn_Vardecl*_tmp103=_tmp9A;
_tmp104=*_tmp103;_tmp105=(void*)_tmp104.type;_tmp106=(void**)&(*_tmp103).type;
_tmp107=_tmp104.rgn;_tmp108=(struct Cyc_Core_Opt**)&(*_tmp103).rgn;{void*_tmp109=
Cyc_Tcenv_curr_rgn(_tmp91);{struct Cyc_Core_Opt*_tmp1CA;*_tmp108=((_tmp1CA=
_cycalloc(sizeof(*_tmp1CA)),((_tmp1CA->v=(void*)_tmp109,_tmp1CA))));}{void*
rgn_typ;if(_tmp9C != 0){struct Cyc_Absyn_Exp*open_exp=(struct Cyc_Absyn_Exp*)_tmp9C;
struct Cyc_List_List*_tmp10B=Cyc_Tcenv_lookup_type_vars(_tmp91);{struct Cyc_Core_Opt*
_tmp1CB;rgn_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,((_tmp1CB=
_cycalloc(sizeof(*_tmp1CB)),((_tmp1CB->v=_tmp10B,_tmp1CB)))));}{struct Cyc_Core_Opt*
_tmp1CC;void*rgn2_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,((
_tmp1CC=_cycalloc(sizeof(*_tmp1CC)),((_tmp1CC->v=_tmp10B,_tmp1CC)))));struct Cyc_Absyn_DynRgnType_struct
_tmp1CF;struct Cyc_Absyn_DynRgnType_struct*_tmp1CE;void*expected=(void*)((_tmp1CE=
_cycalloc(sizeof(*_tmp1CE)),((_tmp1CE[0]=((_tmp1CF.tag=15,((_tmp1CF.f1=(void*)
rgn_typ,((_tmp1CF.f2=(void*)rgn2_typ,_tmp1CF)))))),_tmp1CE))));{struct Cyc_Absyn_RgnHandleType_struct
_tmp1D2;struct Cyc_Absyn_RgnHandleType_struct*_tmp1D1;*_tmp106=(void*)((_tmp1D1=
_cycalloc(sizeof(*_tmp1D1)),((_tmp1D1[0]=((_tmp1D2.tag=14,((_tmp1D2.f1=(void*)
rgn_typ,_tmp1D2)))),_tmp1D1))));}if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(_tmp91,(
void**)& expected,open_exp),expected)){const char*_tmp1D7;void*_tmp1D6[2];struct
Cyc_String_pa_struct _tmp1D5;struct Cyc_String_pa_struct _tmp1D4;(_tmp1D4.tag=0,((
_tmp1D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(open_exp->topt))->v)),((_tmp1D5.tag=0,((
_tmp1D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
expected)),((_tmp1D6[0]=& _tmp1D5,((_tmp1D6[1]=& _tmp1D4,Cyc_Tcutil_terr(open_exp->loc,((
_tmp1D7="expecting %s but found %s",_tag_dyneither(_tmp1D7,sizeof(char),26))),
_tag_dyneither(_tmp1D6,sizeof(void*),2)))))))))))));}Cyc_Tcenv_check_rgn_accessible(
_tmp91,open_exp->loc,rgn2_typ);_tmp102=Cyc_Tcenv_add_region(_tmp90,_tmp102,
rgn_typ,0,1);}}else{{struct Cyc_Absyn_VarType_struct _tmp1DA;struct Cyc_Absyn_VarType_struct*
_tmp1D9;rgn_typ=(void*)((_tmp1D9=_cycalloc(sizeof(*_tmp1D9)),((_tmp1D9[0]=((
_tmp1DA.tag=1,((_tmp1DA.f1=_tmp99,_tmp1DA)))),_tmp1D9))));}{struct Cyc_List_List*
_tmp1DB;_tmp102=Cyc_Tcenv_add_type_vars(_tmp90,s0->loc,_tmp102,((_tmp1DB=
_cycalloc(sizeof(*_tmp1DB)),((_tmp1DB->hd=_tmp99,((_tmp1DB->tl=0,_tmp1DB)))))));}
_tmp102=Cyc_Tcenv_add_region(_tmp90,_tmp102,rgn_typ,_tmp9B,1);}Cyc_Tcutil_check_type(
s0->loc,_tmp91,Cyc_Tcenv_lookup_type_vars(_tmp102),(void*)2,1,*_tmp106);{struct
Cyc_Absyn_RgnHandleType_struct _tmp1DE;struct Cyc_Absyn_RgnHandleType_struct*
_tmp1DD;if(!Cyc_Tcutil_unify(*_tmp106,(void*)((_tmp1DD=_cycalloc(sizeof(*_tmp1DD)),((
_tmp1DD[0]=((_tmp1DE.tag=14,((_tmp1DE.f1=(void*)rgn_typ,_tmp1DE)))),_tmp1DD)))))){
const char*_tmp1E1;void*_tmp1E0;(_tmp1E0=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1E1="region stmt: type of region handle is wrong!",
_tag_dyneither(_tmp1E1,sizeof(char),45))),_tag_dyneither(_tmp1E0,sizeof(void*),0)));}}
if(!Cyc_Absyn_no_regions)_tmp102=Cyc_Tcenv_enter_try(_tmp90,_tmp102);Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_encloser(_tmp90,Cyc_Tcenv_add_local_var(_tmp90,_tmp3F->loc,_tmp102,
_tmp9A),s0),_tmp3F,0);return;}}}_LL3D: if(*((int*)_tmp92)!= 5)goto _LL3F;_tmp9D=((
struct Cyc_Absyn_Alias_d_struct*)_tmp92)->f1;_tmp9E=((struct Cyc_Absyn_Alias_d_struct*)
_tmp92)->f2;_tmp9F=((struct Cyc_Absyn_Alias_d_struct*)_tmp92)->f3;_LL3E: {struct
_RegionHandle*_tmp11D=Cyc_Tcenv_get_fnrgn(_tmp91);{struct Cyc_Absyn_Tvar*_tmp1E2[
1];struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(_tmp11D,s0->loc,_tmp91,((
_tmp1E2[0]=_tmp9E,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_dyneither_ptr))Cyc_List_rlist)(_tmp11D,_tag_dyneither(_tmp1E2,sizeof(struct Cyc_Absyn_Tvar*),
1)))));{struct Cyc_Absyn_VarType_struct _tmp1E5;struct Cyc_Absyn_VarType_struct*
_tmp1E4;te2=Cyc_Tcenv_add_region(_tmp11D,te2,(void*)((_tmp1E4=_cycalloc(sizeof(*
_tmp1E4)),((_tmp1E4[0]=((_tmp1E5.tag=1,((_tmp1E5.f1=_tmp9E,_tmp1E5)))),_tmp1E4)))),
0,1);}Cyc_Tcexp_tcExpInitializer(te2,0,_tmp9D);if(!Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp9D->topt))->v)){{const char*_tmp1E9;
void*_tmp1E8[1];struct Cyc_String_pa_struct _tmp1E7;(_tmp1E7.tag=0,((_tmp1E7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp9D->topt))->v)),((_tmp1E8[0]=& _tmp1E7,Cyc_Tcutil_terr(
_tmp9D->loc,((_tmp1E9="%s is not a pointer type",_tag_dyneither(_tmp1E9,sizeof(
char),25))),_tag_dyneither(_tmp1E8,sizeof(void*),1)))))));}return;}(void*)(
_tmp9F->type=(void*)Cyc_Tcutil_copy_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9D->topt))->v));Cyc_Tcutil_check_type(_tmp9D->loc,_tmp91,Cyc_Tcenv_lookup_type_vars(
te2),(void*)1,1,(void*)_tmp9F->type);{struct Cyc_Core_Opt*_tmp1EA;_tmp9F->rgn=((
_tmp1EA=_cycalloc(sizeof(*_tmp1EA)),((_tmp1EA->v=(void*)Cyc_Tcenv_curr_rgn(te2),
_tmp1EA))));}te2=Cyc_Tcenv_add_local_var(_tmp11D,s0->loc,te2,_tmp9F);te2=Cyc_Tcenv_set_encloser(
_tmp11D,te2,_tmp3F);Cyc_Tcstmt_tcStmt(te2,_tmp3F,0);}Cyc_NewControlFlow_set_encloser(
s0,Cyc_Tcenv_get_encloser(_tmp91));return;}_LL3F: if(*((int*)_tmp92)!= 1)goto
_LL41;_tmpA0=((struct Cyc_Absyn_Fn_d_struct*)_tmp92)->f1;_LL40: {void*_tmp125=Cyc_Tcenv_curr_rgn(
_tmp91);if((void*)_tmpA0->sc != (void*)2){const char*_tmp1ED;void*_tmp1EC;(_tmp1EC=
0,Cyc_Tcutil_terr(_tmp3E->loc,((_tmp1ED="bad storage class for inner function",
_tag_dyneither(_tmp1ED,sizeof(char),37))),_tag_dyneither(_tmp1EC,sizeof(void*),0)));}{
union Cyc_Absyn_Nmspace_union _tmp128=(*_tmpA0->name).f1;struct Cyc_List_List*
_tmp129;struct Cyc_List_List*_tmp12A;_LL85: if((_tmp128.Rel_n).tag != 1)goto _LL87;
_tmp129=(_tmp128.Rel_n).f1;if(_tmp129 != 0)goto _LL87;_LL86: goto _LL84;_LL87: if((
_tmp128.Abs_n).tag != 2)goto _LL89;_tmp12A=(_tmp128.Abs_n).f1;_LL88: {const char*
_tmp1F0;void*_tmp1EF;(_tmp1EF=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1F0="tc: Abs_n in tcStmt var decl",
_tag_dyneither(_tmp1F0,sizeof(char),29))),_tag_dyneither(_tmp1EF,sizeof(void*),0)));}
_LL89:;_LL8A: {const char*_tmp1F3;void*_tmp1F2;(_tmp1F2=0,Cyc_Tcutil_terr(_tmp3E->loc,((
_tmp1F3="explicit namespace not allowed on inner function declaration",
_tag_dyneither(_tmp1F3,sizeof(char),61))),_tag_dyneither(_tmp1F2,sizeof(void*),0)));}
_LL84:;}{union Cyc_Absyn_Nmspace_union _tmp1F4;(*_tmpA0->name).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp1F4.Loc_n).tag=0,_tmp1F4));}Cyc_Tcutil_check_fndecl_valid_type(_tmp3E->loc,
_tmp91,_tmpA0);{void*t=Cyc_Tcutil_fndecl2typ(_tmpA0);_tmpA0->attributes=Cyc_Tcutil_transfer_fn_type_atts(
t,_tmpA0->attributes);{struct Cyc_List_List*atts=_tmpA0->attributes;for(0;(
unsigned int)atts;atts=atts->tl){void*_tmp130=(void*)atts->hd;_LL8C: if((int)
_tmp130 != 5)goto _LL8E;_LL8D: goto _LL8F;_LL8E: if(_tmp130 <= (void*)17)goto _LL90;if(*((
int*)_tmp130)!= 1)goto _LL90;_LL8F:{const char*_tmp1F8;void*_tmp1F7[1];struct Cyc_String_pa_struct
_tmp1F6;(_tmp1F6.tag=0,((_tmp1F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp1F7[0]=& _tmp1F6,Cyc_Tcutil_terr(
_tmp3E->loc,((_tmp1F8="bad attribute %s for function",_tag_dyneither(_tmp1F8,
sizeof(char),30))),_tag_dyneither(_tmp1F7,sizeof(void*),1)))))));}goto _LL8B;
_LL90:;_LL91: goto _LL8B;_LL8B:;}}{struct Cyc_Core_Opt*_tmp1FB;struct Cyc_Absyn_Vardecl*
_tmp1FA;struct Cyc_Absyn_Vardecl*vd=(_tmp1FA=_cycalloc(sizeof(*_tmp1FA)),((
_tmp1FA->sc=(void*)((void*)_tmpA0->sc),((_tmp1FA->name=_tmpA0->name,((_tmp1FA->tq=
Cyc_Absyn_const_tqual(0),((_tmp1FA->type=(void*)Cyc_Absyn_at_typ(t,_tmp125,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref),((_tmp1FA->initializer=0,((_tmp1FA->rgn=((_tmp1FB=
_cycalloc(sizeof(*_tmp1FB)),((_tmp1FB->v=(void*)_tmp125,_tmp1FB)))),((_tmp1FA->attributes=
0,((_tmp1FA->escapes=0,_tmp1FA)))))))))))))))));_tmpA0->fn_vardecl=(struct Cyc_Absyn_Vardecl*)
vd;_tmp91=Cyc_Tcenv_add_local_var(_tmp90,_tmp3E->loc,_tmp91,vd);{struct Cyc_Tcenv_Fenv*
old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(_tmp91->le);_tmp91->le=(struct Cyc_Tcenv_Fenv*)
Cyc_Tcenv_nested_fenv(_tmp3E->loc,old_fenv,_tmpA0);Cyc_Tcstmt_tcStmt(_tmp91,
_tmpA0->body,0);Cyc_Tcenv_check_delayed_effects(_tmp91);Cyc_Tcenv_check_delayed_constraints(
_tmp91);if(!Cyc_Tcenv_all_labels_resolved(_tmp91)){const char*_tmp1FE;void*
_tmp1FD;(_tmp1FD=0,Cyc_Tcutil_terr(_tmp3E->loc,((_tmp1FE="function has goto statements to undefined labels",
_tag_dyneither(_tmp1FE,sizeof(char),49))),_tag_dyneither(_tmp1FD,sizeof(void*),0)));}
_tmp91->le=(struct Cyc_Tcenv_Fenv*)old_fenv;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
_tmp90,_tmp91,s0),_tmp3F,0);return;}}}}_LL41: if(*((int*)_tmp92)!= 10)goto _LL43;
_tmpA1=((struct Cyc_Absyn_Namespace_d_struct*)_tmp92)->f1;_tmpA2=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp92)->f2;_LL42:{const char*_tmp1FF;unimp_msg_part=((_tmp1FF="namespace",
_tag_dyneither(_tmp1FF,sizeof(char),10)));}goto _LL34;_LL43: if(*((int*)_tmp92)!= 
11)goto _LL45;_tmpA3=((struct Cyc_Absyn_Using_d_struct*)_tmp92)->f1;_tmpA4=((
struct Cyc_Absyn_Using_d_struct*)_tmp92)->f2;_LL44:{const char*_tmp200;
unimp_msg_part=((_tmp200="using",_tag_dyneither(_tmp200,sizeof(char),6)));}goto
_LL34;_LL45: if(*((int*)_tmp92)!= 6)goto _LL47;_LL46:{const char*_tmp201;
unimp_msg_part=((_tmp201="type",_tag_dyneither(_tmp201,sizeof(char),5)));}goto
_LL34;_LL47: if(*((int*)_tmp92)!= 7)goto _LL49;_LL48:{const char*_tmp202;
unimp_msg_part=((_tmp202="[x]tunion",_tag_dyneither(_tmp202,sizeof(char),10)));}
goto _LL34;_LL49: if(*((int*)_tmp92)!= 8)goto _LL4B;_LL4A:{const char*_tmp203;
unimp_msg_part=((_tmp203="enum",_tag_dyneither(_tmp203,sizeof(char),5)));}goto
_LL34;_LL4B: if(*((int*)_tmp92)!= 9)goto _LL4D;_LL4C:{const char*_tmp204;
unimp_msg_part=((_tmp204="typedef",_tag_dyneither(_tmp204,sizeof(char),8)));}
goto _LL34;_LL4D: if(*((int*)_tmp92)!= 12)goto _LL4F;_LL4E:{const char*_tmp205;
unimp_msg_part=((_tmp205="extern \"C\"",_tag_dyneither(_tmp205,sizeof(char),11)));}
goto _LL34;_LL4F: if(*((int*)_tmp92)!= 13)goto _LL51;_LL50:{const char*_tmp206;
unimp_msg_part=((_tmp206="extern \"C include\"",_tag_dyneither(_tmp206,sizeof(
char),19)));}goto _LL34;_LL51: if((int)_tmp92 != 0)goto _LL53;_LL52:{const char*
_tmp207;unimp_msg_part=((_tmp207="__cyclone_port_on__",_tag_dyneither(_tmp207,
sizeof(char),20)));}goto _LL34;_LL53: if((int)_tmp92 != 1)goto _LL34;_LL54:{const
char*_tmp208;unimp_msg_part=((_tmp208="__cyclone_port_off__",_tag_dyneither(
_tmp208,sizeof(char),21)));}goto _LL34;_LL34:;}}{const char*_tmp20C;void*_tmp20B[1];
struct Cyc_String_pa_struct _tmp20A;(int)_throw(((_tmp20A.tag=0,((_tmp20A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)unimp_msg_part),((_tmp20B[0]=&
_tmp20A,Cyc_Tcutil_impos(((_tmp20C="tcStmt: nested %s declarations unimplemented",
_tag_dyneither(_tmp20C,sizeof(char),45))),_tag_dyneither(_tmp20B,sizeof(void*),1)))))))));}}
_LL28: if(_tmp16 <= (void*)1)goto _LL7;if(*((int*)_tmp16)!= 15)goto _LL7;_tmp40=((
struct Cyc_Absyn_ResetRegion_s_struct*)_tmp16)->f1;_LL29: {void*rgn_type=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_RgnHandleType_struct
_tmp20F;struct Cyc_Absyn_RgnHandleType_struct*_tmp20E;void*etype=(void*)((_tmp20E=
_cycalloc(sizeof(*_tmp20E)),((_tmp20E[0]=((_tmp20F.tag=14,((_tmp20F.f1=(void*)
rgn_type,_tmp20F)))),_tmp20E))));if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(te,(void**)&
etype,_tmp40),etype)){const char*_tmp213;void*_tmp212[1];struct Cyc_String_pa_struct
_tmp211;(_tmp211.tag=0,((_tmp211.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp40->topt))->v)),((
_tmp212[0]=& _tmp211,Cyc_Tcutil_terr(_tmp40->loc,((_tmp213="expecting region_t but found %s",
_tag_dyneither(_tmp213,sizeof(char),32))),_tag_dyneither(_tmp212,sizeof(void*),1)))))));}
Cyc_Tcenv_check_rgn_resetable(te,s0->loc,rgn_type);return;}_LL7:;}}
